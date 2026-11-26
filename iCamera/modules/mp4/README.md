# MP4 录像 / 回放模块

面向嵌入式监控设备的 MP4 封装与解封装模块，直接读写 H.264 Annex-B 裸流，**不做二次编码**。
模块不依赖 ffmpeg / libav / libmov，只使用 POSIX libc。

## 1. 文件结构

```
modules/mp4/
├── Makefile              # include ../common.mak，产出 libmp4.a / libmp4.so
├── README.md             # 本文档
├── mp4_common.c/.h       # 大端读写、可增长缓冲区 MP4Buf（全部带边界检查）
├── mp4_h264.c/.h         # Annex-B 解析、SPS/PPS 提取、AVCC<->Annex-B 转换、SPS 宽高解析
├── mp4_writer.c/.h       # fMP4 写入：moov 首写 + 每分片 moof/mdat + mfra
└── mp4_player.c/.h       # fMP4 回放：moof 索引、seek、pop_one_frame_from_sector_MP4
```

公共头文件在 `modules/mp4/include/`，其他模块通过 `-I../mp4/include` 引用即可。

## 2. 编译依赖

- 仅依赖 POSIX libc：`open/write/read/pread/fsync/close/access/malloc`。
- **不依赖 ffmpeg、libav、libmov**，不需要设备侧安装任何多媒体库。
- 需要编译器支持 C99（代码使用 `stdint.h`、`long long`、声明与语句混写）。
- 大文件支持：源文件内部定义了
  `_FILE_OFFSET_BITS=64` 与 `_POSIX_C_SOURCE=200809L`（用于 `pread` / `fsync`），
  无需在外部额外传递宏；若工程整体已定义这两个宏也不会冲突。

### 工程内编译

模块通过 `modules/common.mak` 编译，库名取自目录名，即 `libmp4.a` / `libmp4.so`：

```sh
cd iCamera/modules/mp4
make STATIC=true          # 静态库 libmp4.a
make                      # 动态库 libmp4.so
make clean
```

若需随 `modules/` 目录统一编译，在 `modules/makefile` 的 `MODS` 中加入 `mp4`：

```make
MODS += mp4
```

然后：

```sh
cd iCamera/modules && make
```

交叉编译由 `common.mak` 内的 `CROSS_COMPILE_PREFIX` 自动处理，无需单独配置。

### 主机快速自检（不进入交叉编译链）

```sh
gcc -std=gnu99 -c mp4_common.c mp4_h264.c mp4_writer.c mp4_player.c \
    -Iinclude -Wall -Werror -Werror=return-type -Wpointer-arith -Wundef -Wcast-qual -Os
```

## 3. 写入侧 API

```c
#include "mp4_writer.h"

MP4WriterInfo info = {
    .width      = 0,        /* 0 = 从 SPS 自动解析 */
    .height     = 0,
    .fps        = 25,
    .start_time = 0         /* 0 = 使用当前系统时间 */
};

MP4WriterConfig cfg = {
    .max_samples      = 30,     /* 单分片最多 30 帧 */
    .max_bytes        = 512*1024,
    .max_duration_ms  = 2000,   /* 单分片最长 2 秒 */
    .sync_interval_ms = 2000    /* 每 2 秒强制 fsync 一次 */
};

MP4Writer *w = MP4Writer_Create("/mnt/sd/20260101/120000.mp4", &info, &cfg);
if (!w) { /* 打开失败，记录 MP4Writer_GetLastError 需要句柄，此处读 errno */ }

/* 每来一帧就调用一次，data 必须是完整的 H.264 Annex-B 访问单元 */
while (recording) {
    int ret = MP4Writer_WriteFrame(w, frame, size, pts_us, dts_us, keyframe);
    if (ret != 0) {
        /* SD 卡满 / 写失败：记录 MP4Writer_GetLastError(w) 后停止录像 */
        break;
    }
}

MP4Writer_Close(w);     /* 写完剩余帧 + mfra 索引 + fsync */
MP4Writer_Destroy(w);
```

要点：

- `keyframe` 传 `-1` 时模块自动检测 IDR（NAL type 5）。
- 首帧之前如果还没拿到 SPS/PPS，模块会丢弃这些帧（返回 0），不会写出无法解码的文件。
- 首帧如果不是 I 帧会被丢弃，保证每个分片都从 I 帧开始，便于 seek 和不完整分片丢弃。
- 允许 PTS/DTS 为负（B 帧场景），内部以首帧 DTS 为基准归一化，不会压缩时间轴。
- 时间戳异常（例如非单调、跳变）时按标称帧率补齐，避免分片时长错乱。
- 所有内存操作都有边界检查；单个 NAL / 帧大小超过 `MP4_MAX_FRAME_SIZE` 会返回错误而不是越界。

## 4. 回放侧 API

```c
#include "mp4_player.h"

MP4PopHandle h;
memset(&h, 0, sizeof(h));
MP4PopHandle_Init(&h, "/mnt/sd/20260101/120000.mp4", start_sec, 0);

char *buf = NULL;
int   size = 0, frametype = 0, keyframe = 0, need_seek = 0, vip = 0;
unsigned int sec = 0, usec = 0;

int ret = pop_one_frame_from_sector_MP4(&h, 0, &buf, &size,
                                        &frametype, &keyframe,
                                        &sec, &usec, &need_seek, &vip);
switch (ret) {
case 0:  /* 成功：buf/size 是原始 H.264 Annex-B 帧，keyframe=1 为 I 帧 */
    break;
case 1:  /* 文件读取完毕 */
    break;
case -1: /* 读取/解析错误 */
    break;
case 2:  /* 文件不存在，直接跳过；此时不会写 p_frame_buffer */
    break;
}

MP4PopHandle_Close(&h);
```

返回值约定：

| 返回值 | 含义 | 对 `p_frame_buffer` 的处理 |
| --- | --- | --- |
| `0`  | 成功取到一帧 | 指向模块内部缓冲区，下次调用前有效 |
| `-1` | 读取 / 解析错误 | 不改动 |
| `1`  | 文件读取完毕 | 不改动 |
| `2`  | 文件不存在 | **绝对不写 `p_frame_buffer`**，直接跳过 |

时间戳输出：

- `*pRealTimeSec` / `*pRealTimeUSec` 为**绝对时间**（微秒精度），
  等于文件 `creation_time` + 该帧相对 PTS；
- 关键帧标记 `*pKeyframe` 为 1 时表示 I 帧，可直接用于解码器重启；
- `*pNeedSeek` 在需要上层重新 seek 时置 1。

seek：

```c
MP4Player *p = MP4Player_Open(path);
MP4Player_SeekUS(p, 2 * 1000000ULL);  /* 定位到 <=2s 的最近一个 I 帧分片 */
```

`MP4Player_SeekUS` 通过对分片 `base_dts` 二分查找实现，因每个分片首帧必为 I 帧，
定位后从该分片第一帧开始输出，保证解码器可立即刷新。

## 5. 掉电可靠性设计（untrunc 思路）

标准 MP4 的 `moov` 索引在文件末尾一次性写入，掉电后 `moov` 缺失，整段录像不可播放，
只能事后用 untrunc 之类工具扫描重建。本模块采用 **fMP4（分片 MP4）** 布局规避该问题：

```
ftyp
moov(mvhd + trak + mvex/trex)     <- 首帧前写入并 fsync，一次成型
moof + mdat                       <- 每个分片：索引(moof)紧挨着数据(mdat)写入
moof + mdat
...
mfra(tfra + mfro)                 <- Close 时追加，随机访问索引，掉电缺失不影响播放
```

- **索引不拖到文件结束**：每个分片的 `moof` 在写 `mdat` 之前就写入文件，
  等价于 untrunc 追求的“索引与数据同步落盘”，掉电后已写入的分片全部可独立解析。
- **分片内自包含**：`moof` 内含 `tfhd/tfdt/trun`，sample 大小、时长、偏移、关键帧标记齐全，
  不需要任何全局索引。
- **不完整尾分片可丢弃**：回放侧校验 `moof` 声明长度与实际文件长度，
  若最后一个分片被截断（掉电发生在写入过程中），直接丢弃该分片，前缀照常播放。
- **周期刷盘**：`sync_interval_ms` 控制 `fsync` 周期（默认 2 秒），
  同时按帧数（30）/字节数（512KB）/时长（2 秒）切分片，避免 SD 卡上频繁小写。
  即使进程被 `kill -9`，最多丢失最后一个未刷盘分片。
- **`mfra` 是纯优化**：即使 `mfra` 缺失，播放器仍可通过顺序扫描 `moof` 建立索引。

## 6. 异常断电可靠性验证方法

### 6.1 模拟掉电（不调用 Close）

```sh
# 写入 N 帧后直接 _exit(0)，模拟断电即进程被杀、文件未 Close
./mp4test crash 80 /tmp/crash.mp4
```

验证已落盘前缀可播放：

```sh
ffprobe -v error -show_entries format=duration -of default=nw=1 /tmp/crash.mp4
ffmpeg  -v error -i /tmp/crash.mp4 -f null -
```

### 6.2 真实 kill -9 / 拔电

```sh
# 录像进行中直接强杀
kill -9 $(pidof recorder)
```

或直接在设备上断电/拔卡，重新上电后：

```sh
# 1) 容器是否可解析
ffprobe -v error -show_entries format=duration,size -of default=nw=1 xxx.mp4

# 2) 完整解码一遍，检查是否有解码错误
ffmpeg -v error -i xxx.mp4 -f null -

# 3) 用本模块回放，检查能读到的帧数与关键帧数
```

### 6.3 尾部截断（模拟最后分片写一半）

```sh
truncate -s -200 /tmp/out.mp4     # 去掉文件末尾 200 字节
```

预期：`ffprobe` 可能报告最后分片异常，但本模块回放会丢弃不完整分片，
前缀所有帧均可正常读出（实测截断 200 字节后仍读到 100 帧 / 4 个关键帧）。

## 7. MP4 拆回 H.264 的验证命令

模块生成的 MP4 必须可**无损解封装**，完整还原原始 H.264 帧数据。可用 ffmpeg 仅做
`copy`（不重编码）+ `h264_mp4toannexb` 比特流过滤器拆分：

```sh
# 1) 解封装为 H.264 Annex-B 裸流（-c:v copy 保证不重编码）
ffmpeg -v error -i out.mp4 -map 0:v:0 -c:v copy \
       -bsf:v h264_mp4toannexb -f h264 extracted.h264

# 2) 统计帧数，应与原始码流一致
ffprobe -v error -count_frames -select_streams v:0 \
        -show_entries stream=nb_read_frames -of default=nw=1 extracted.h264

# 3) 逐帧 MD5 对比，验证解码结果逐帧一致（含 B 帧场景）
ffmpeg -v error -i src.h264  -f framemd5 src.md5
ffmpeg -v error -i out.mp4   -f framemd5 out.md5
diff src.md5 out.md5
```

> 注：本机若 `ffmpeg` 因 `LD_LIBRARY_PATH` 指向其他 libstdc++ 而启动失败，
> 可用 `env -u LD_LIBRARY_PATH ffmpeg ...` 运行。设备侧不依赖 ffmpeg，
> 上述命令仅用于 PC 端验证。

其他常用检查：

```sh
# 查看封装结构（应看到 ftyp/moov/moof/mdat/mfra）
ffprobe -v trace -i out.mp4 2>&1 | grep -E "type:'(ftyp|moov|moof|mdat|mfra)'"

# 查看关键帧分布
ffprobe -v error -select_streams v:0 -show_frames \
        -show_entries frame=pts_time,key_frame -of csv out.mp4 | grep ',1$'
```

## 8. 实测结果

使用 ffmpeg 生成的 H.264 源（320x240/25fps/125 帧；另测 640x360、bframes=3）验证：

| 项目 | 结果 |
| --- | --- |
| 编译 | `-Werror -Wall -Wundef -Wcast-qual` 全通过，无警告 |
| 容器解析 | `ffprobe` 报告 320x240 / 25fps / duration=5.000000 |
| 解码 | `ffmpeg -f null -` 无错误 |
| 帧数 | 解封装后 125 帧，与源一致 |
| 无损性 | 逐帧 framemd5 对比，无 B 帧与含 B 帧场景均 125/125 一致 |
| 回放 | 读到 125 帧 / 5 个关键帧，末尾返回 1 |
| seek | seek 2.1s 定位到 2.0s 的 I 帧 |
| 绝对时间戳 | start_time=1735689600 + 2s -> 输出 1735689602.000000 |
| 掉电模拟 | 写入 80 帧后 `_exit`，文件 3.000000s，前 75 帧可播放（丢弃未刷盘的最后一个 GOP） |
| 尾部截断 | 去掉 200 字节，回放丢弃不完整分片，仍读到 100 帧 / 4 个关键帧 |
| 文件不存在 | 返回 2，`p_frame_buffer` 保持调用前指针不被修改 |
