# 拇指相机 UI 原型 —— 架构说明与使用教程

> 面向后续 AI / 同事的上手文档。涵盖：这是什么、目录与职责、架构原理、编译方法、如何接入主程序、对外接口速查、移植到真机的改动点、APP 指令协议、注意事项。

---

## 1. 这是什么

基于 **LVGL 8.3（v8.3.11）** 的**纯 C** 拇指相机 UI 原型样机。

- **屏幕**：240×320 竖屏，RGB565。
- **当前硬件载体**：海思 IPC 摄像头（Hi3516CV608 / 产品型号 `P3MU`），**仅用于临时仿真拇指相机**，不是最终量产硬件。
- **码流**：IPC 两路码流——主码流 2304×1296（录像保存）、子码流 240×320（送 UI 层做实时预览背景）。
- **交互**：本样机**无触摸屏**，两类输入统一封装：
  1. 物理按键（上/下/确认/返回）做焦点导航；
  2. APP 下发 JSON 网络指令，模拟屏幕点击坐标 / 按键，和物理按键复用同一套交互逻辑。
- **设计目标**：严格模块化、低耦合、海思相关耦合全部收敛到少量钩子，**方便后续把 UI 代码整体迁移到真实拇指相机硬件**。

---

## 2. 目录结构与模块职责

所有 UI 代码位于 `iCamera/DVR_SDK/ui/` 下（与 SDK 的 COM/NET/PAR/LCD 等模块平级）。源码按 8 个子模块目录隔离，**最终聚合成一个 `libui.a`**；第三方 LVGL 独立编成 `liblvgl.a`。

```
iCamera/DVR_SDK/ui/
├── makefile              # 聚合构建：收集 8 个模块的 .o → 单个 libui.a；并驱动 lvgl 子构建
│
├── ui_common/            # 公共基础层：被所有模块依赖，零业务逻辑
│   ├── lv_conf.h         #   LVGL 8.3 裁剪配置（240x320 / RGB565 / 自定义 tick / 小字体集）
│   ├── ui_common.h       #   屏幕尺寸、通用宏、日志、返回码；统一包含头
│   ├── ui_event.h        #   【统一事件契约】按键/点击/视频帧/业务状态 事件枚举+联合体
│   ├── ui_theme.h/.c     #   LVGL style：面板/列表项/焦点高亮/文字 样式
│   └── ui_res.h/.c       #   配色、字体、图标符号（LVGL 内置 symbol）集中管理
│
├── ui_main/              # UI 入口与基础设施（唯一持有 LVGL 的线程）
│   ├── ui_port.h/.c      #   移植层：tick 线程 + 显示 flush → LCD 回调（海思解耦点）
│   ├── ui_msgq.h/.c      #   线程安全消息队列：跨线程 → UI 线程的唯一通道
│   ├── ui_page.h/.c      #   页面栈：push/pop/replace + 页面统一接口(create/show/hide/event)
│   ├── ui_main.h/.c      #   ui_main_init/start/stop、建 UI 线程、LVGL 初始化、三层画布、主循环
│   └── ui_integration_example.c  # 接入参考示例（不编入库）
│
├── ui_video/             # 底层视频预览画布（最底图层）
│   └── ui_video.h/.c     #   ui_video_refresh(yuv) 对外；双缓冲；YUV→RGB 钩子；VDEC/VENC 预留
│
├── ui_input/             # 输入抽象层：屏蔽来源，向上只给 LVGL 事件
│   ├── ui_input.h/.c         # 注册 keypad + pointer 两个 LVGL indev
│   ├── ui_input_key.h/.c     # 源1：物理按键扫描（ui_key_scan_hw 弱函数，真机强实现）
│   └── ui_input_app.c        # 源2：APP JSON 指令解析（复用 DVR_SDK/NET/cJSON）
│
├── ui_statusbar/         # 顶部/底部状态栏
│   └── ui_statusbar.h/.c #   录制红点闪烁/时间/WiFi/电量/剩余容量/音频/防抖
│
├── ui_menu/              # 相机操作页（业务控制器 + 预览 + 菜单 + 回放）
│   ├── ui_cam.h/.c           # 业务控制器：模式/录制状态机 + 相机动作钩子(ui_cam_ops_t)
│   ├── ui_menu.h             # 三个页面的注册声明
│   ├── ui_page_preview.c     # 预览页：视频底层+状态栏，快门定格动画，延时，1Hz tick
│   ├── ui_page_menu.c        # 模式主菜单：录像/拍照/延时/回放/设置
│   └── ui_page_playback.c    # 回放页：缩略图浏览/播放/删除（ui_pb_source_t 钩子）
│
├── ui_setting/           # 参数设置页
│   └── ui_setting.h/.c   #   分辨率/循环/WDR/翻转/红外/音频/水印/格式化/恢复出厂/WiFi
│                         #   动作经 ui_settings_ops_t 钩子下发
│
├── ui_popup/             # 弹窗层（最顶）
│   └── ui_popup.h/.c     #   toast 提示 / 确认框 / 处理中遮罩
│
└── lvgl/                 # 第三方 LVGL v8.3.11（仅 lvgl.h + src/），独立编 liblvgl.a
    └── makefile
```

---

## 3. 架构原理（三个关键机制）

### 3.1 三层画布（视频永远在底，菜单浮层叠加其上）

在 `ui_main.c` 的 `build_layers()` 里建立：

| 层 | 实现 | 内容 |
|---|---|---|
| 底层 | `lv_scr_act()` 上的一个全屏容器 + `lv_obj_move_background()` | 视频预览画布（`ui_video` 的 canvas） |
| 内容层 | 活动屏幕 `lv_scr_act()` 本身（透明） | 页面栈内容（预览/菜单/设置/回放） |
| 顶层 | `lv_layer_top()` | 状态栏 + 弹窗，始终最上 |

> ⚠️ LVGL 8.3 **没有** `lv_layer_bottom()`（那是 9.0 的 API）。所以底层用“屏幕上的专用容器 + move_background”实现。

### 3.2 线程模型（严禁跨线程碰 LVGL）

```
 [按键扫描线程]  [APP 网络线程]   [IPC 子码流线程]   [业务上报线程]
        │              │                │                 │
   ui_input_key_  ui_input_app_    ui_video_refresh   (biz 事件)
     inject()       on_json()                              │
        └──────────────┴────────┬───────┴─────────────────┘
                                 ▼
                     ui_msgq_post()  (线程安全环形队列)
                                 ▼
                 ┌───────────────────────────────┐
                 │        UI 线程（独占 LVGL）     │
                 │  ui_msgq_poll → 分发 → lv_timer_handler │
                 └───────────────────────────────┘
```

- **只有 UI 线程操作 LVGL 对象**。
- 其它线程一律通过 `ui_msgq_post()` 投递 `ui_event_t`，UI 线程 `ui_msgq_poll()` 取出后才动 LVGL。
- 视频帧事件在队列满时自动丢弃（保实时、不堆积、不阻塞推流线程）。

### 3.3 输入抽象（来源无关）

`ui_input` 注册两个 LVGL indev：
- **keypad indev**：上/下/确认/返回 → `LV_KEY_PREV/NEXT/ENTER/ESC`，驱动焦点组导航；
- **pointer indev**：APP 下发坐标 → 模拟触摸点击。

物理按键和 APP 指令最终都变成同一套事件，**上层页面不区分来源**。

---

## 4. 编译方法

### 4.1 工具链

- 交叉编译器：`arm-v01c02s020-linux-musleabi-gcc`（musl, Cortex-A7, 位于 `/opt/toolchains/hi3516c_v610/.../sbin/`，已在 PATH）。
- 产品类型：`PRODUCT_TYPE=P3MU` → 自动解析为 `TYPE=Hi3516CV608`。

### 4.2 一键构建

```bash
cd iCamera/DVR_SDK/ui
make PRODUCT_TYPE=P3MU          # 编 liblvgl.a + libui.a 并安装
```

其它目标：
```bash
make libui PRODUCT_TYPE=P3MU    # 只编自研 UI（需 lvgl 源码头文件在位）
make lvgl  PRODUCT_TYPE=P3MU    # 只编 LVGL
make clean PRODUCT_TYPE=P3MU
make echo  PRODUCT_TYPE=P3MU    # 打印收集到的源码/安装路径，便于排错
```

### 4.3 产物

| 产物 | 内容 | 体积 | 安装位置 |
|---|---|---|---|
| `libui.a` | 8 个模块 17 个 .o | ~87K | `lib_Hi3516CV608-static/P3MU/` |
| `liblvgl.a` | LVGL 192 个 .o | ~741K | `lib_Hi3516CV608-static/P3MU/` |

`my_gui/Makefile` 会自动 glob `lib_Hi3516CV608-static/P3MU/*.a` 并放进 `--start-group`，**无需改动 my_gui**。之后正常执行：

```bash
cd iCamera/my_gui
./build.sh p3mu d     # 或在仓库约定的构建入口
```

---

## 5. 如何接入主程序（my_gui / 设备主控）

参考 `ui_main/ui_integration_example.c`。核心四步：

```c
#include "ui_main.h"
#include "ui_input.h"
#include "ui_video.h"

/* 1) 初始化并启动 UI（LVGL 全在内部 UI 线程） */
ui_main_cfg_t cfg = {0};
cfg.lcd_flush = my_lcd_send;   /* 整帧送显回调；内部调底层 Lcd_SendFrame(pData, size) */
ui_main_init(&cfg);
ui_main_start();

/* 2) 启动物理按键源（真机实现 ui_key_scan_hw 即可） */
ui_input_key_start();

/* 3) IPC 子码流线程里，每来一帧 240x320 YUV 就推给 UI */
ui_video_refresh(UI_PIX_NV12, yuv_buf, yuv_len, 240, 320, seq++);

/* 4) APP 网络线程里，收到一行 JSON 指令就交给输入层 */
ui_input_app_on_json("{\"type\":\"click\",\"x\":120,\"y\":200}");
```

---

## 6. 对外接口速查

### ui_main（`ui_main/ui_main.h`）
| 接口 | 说明 |
|---|---|
| `int ui_main_init(const ui_main_cfg_t *cfg)` | 初始化（`cfg.lcd_flush` 为 LCD 刷新回调，可 NULL 裸跑） |
| `int ui_main_start(void)` | 启动 UI 线程 |
| `void ui_main_stop(void)` | 停止并反初始化 |

`lcd_flush` 原型（整帧送显，与底层 `Lcd_SendFrame` 一致）：
```c
int (*)(uint8_t *pData, uint32_t size);   /* pData=整帧240x320 RGB565, size=153600 */
```
> LVGL 会把"视频底层 + 菜单浮层"**合成为一整帧**再回调，你只需把 `pData` 整块交给
> `Lcd_SendFrame(pData, size)`。若 `Lcd_SendFrame` 返回 void，用一行适配函数包一下。
> 送屏为【同步】用单缓冲(默认)，【异步 DMA】则在 `ui_port.c` 把 `UI_DISP_DOUBLE_BUF` 置 1。

### ui_video（`ui_video/ui_video.h`）
| 接口 | 说明 |
|---|---|
| `int ui_video_refresh(fmt, buf, len, w, h, seq)` | **主接口**，推一帧子码流（任意线程，线程安全） |
| `void ui_video_set_yuv2rgb(ui_yuv2rgb_fn fn)` | 替换 YUV→RGB 实现（真机换硬转） |
| `void ui_video_set_codec(const ui_video_codec_ops_t *ops)` | 注入 VDEC/VENC 实现（预留） |

像素格式：`UI_PIX_NV12 / UI_PIX_NV21 / UI_PIX_YUYV / UI_PIX_RGB565`。

### ui_input（`ui_input/ui_input.h`）
| 接口 | 说明 |
|---|---|
| `int ui_input_key_start(void)` / `ui_input_key_stop()` | 启停物理按键扫描线程 |
| `void ui_input_key_inject(code, act)` | 直接注入一个逻辑按键（测试/复用） |
| `int ui_input_app_on_json(const char *json)` | 解析一条 APP JSON 指令并投递 |

---

## 7. 迁移到真机需要改的点（钩子清单）

海思 / 板级耦合全部收敛在以下少数位置，**页面与交互逻辑无需改动**：

| 钩子 | 位置 | 样机默认 | 真机实现 |
|---|---|---|---|
| LCD 整帧送显 | `ui_main_cfg_t.lcd_flush` | 打印占位 | 接底层 `Lcd_SendFrame(pData,size)` |
| 物理按键读取 | `ui_key_scan_hw()`（弱函数，`ui_input_key.c`） | 返回无键 | 读 GPIO/ADC 返回 `UI_KEY_*` |
| YUV→RGB | `ui_video_set_yuv2rgb()` | 内置软转 | 海思 VGS/IVE 硬转 |
| VDEC/VENC | `ui_video_set_codec()` | 无 | 录像/抓拍/延时接 SnapShot 及编码 |
| 相机动作 | `ui_cam_set_ops()`（`ui_cam.h`） | 桩（仅改状态+toast） | 真实开始/停止录像、拍照等 |
| 设置读写/生效 | `ui_setting_set_ops()`（`ui_setting.h`） | 默认值 | 持久化 + 下发 ISP/编码 + 格式化/恢复出厂/配网 |
| 回放文件源 | `ui_pb_set_source()`（`ui_page_playback.c`） | 假数据 | 接 SnapShot/录像文件管理 |

---

## 8. APP JSON 指令协议

由 APP 网络线程收到后调用 `ui_input_app_on_json()`：

```jsonc
{"type":"key","code":"up|down|ok|back","act":"press|release"}  // 模拟按键；act 省略=按下+抬起
{"type":"click","x":120,"y":200}      // 一次完整点击（按下+抬起）
{"type":"press","x":120,"y":200}      // 仅按下
{"type":"release","x":120,"y":200}    // 仅抬起
```

坐标系：原点左上，单位像素，范围 0~239 / 0~319（内部会 clamp）。

---

## 9. 业务与页面导航

工作模式（`ui_work_mode_t`）：录像(默认)/拍照/延时摄影/回放/设置。

```
         ┌──────────── 预览页 (PREVIEW) ────────────┐
         │  OK=快门/录制   ▲▼=切拍摄模式   BACK=菜单   │
         └───────────────────┬──────────────────────┘
                             BACK
                              ▼
                   模式菜单 (MENU)  ──OK──► 回放(PLAYBACK) / 设置(SETTING)
                              │                         │
                              └───────── BACK ──────────┘ 回到预览
```

- 录像中按 BACK 不进菜单（防误触），需先停录。
- 预览页 1Hz 定时器驱动 `ui_cam_tick_1hz()`：累加录制时长、递减剩余容量、广播给状态栏。

---

## 10. 注意事项 / 已知事项

1. **LVGL 源码纳管**：`ui/lvgl/src` 含约 192 个第三方文件（~16M）。如不想纳入自有仓库，可在 `.gitignore` 忽略 `iCamera/DVR_SDK/ui/lvgl/`，改用 git submodule 或构建时拉取（当前是直接放入源码）。
2. **LVGL 配置**：裁剪在 `ui_common/lv_conf.h`。编译时用 `-DLV_CONF_INCLUDE_SIMPLE` + `-Iui_common` 让 LVGL 找到它。调控件开关、字体、内存池大小都改这里。
3. **公共头依赖**：任何对外 `.h` 若用到 `lv_*` 类型，必须 `#include "lvgl.h"`（曾因 `ui_popup.h` 漏包含在交叉编译时报错，已修）。
4. **工具链路径**：`my_gui/Makefile` 里写的 `/opt/linux/...` 在本机不存在，实际工具链在 `/opt/toolchains/hi3516c_v610/...`，已在 PATH，`ui/makefile` 直接按名字调用。
5. **内存**：显示缓冲为 240×40 双缓冲（部分刷新），LVGL 内存池 48KB，均可在 `ui_port.c` / `lv_conf.h` 调整。

---

## 11. 验证记录（2026-11-19）

- ✅ `liblvgl.a`（192 objs, 741K, armv7）交叉编译通过。
- ✅ `libui.a`（17 objs / 8 模块, 87K, armv7）在 `-Wall -Wextra` 下零警告通过。
- ✅ 模拟 `my_gui` 主程序将 `libui.a + liblvgl.a` 以 `--start-group` 链接，**零未定义符号**，产出合法 ARM musl 可执行文件——自研 UI ↔ 真实 LVGL 8.3 全栈打通。
- ✅ 关键符号 `ui_main_init / ui_video_refresh / ui_input_app_on_json / ui_page_push` 正确导出。
```
