# 拇指相机 UI 原型 —— 架构说明与使用教程

> 面向后续 AI / 同事的上手文档。涵盖：这是什么、目录与职责、架构原理、编译方法、如何接入主程序、对外接口速查、移植到真机的改动点、触摸屏接入预留、注意事项。

---

## 1. 这是什么

基于 **LVGL 8.3（v8.3.11）** 的**纯 C** 拇指相机 UI 原型样机。

- **屏幕**：240×320 竖屏，RGB565。
- **当前硬件载体**：海思 IPC 摄像头（Hi3516CV608 / 产品型号 `P3MU`），**仅用于临时仿真拇指相机**，不是最终量产硬件。
- **码流**：IPC 两路码流——主码流 2304×1296（录像保存）、子码流 240×320（送 UI 层做实时预览背景）。
- **交互**：本样机**无触摸屏**，两类输入统一封装：
  1. 物理按键（上/下/确认，共 3 键，无返回键）做焦点导航与模式切换；
  2. 触摸屏（真机预留）：pointer indev 已注册休眠，触摸驱动投递 UI_EVT_POINT 即生效，和物理按键复用同一套交互逻辑。
- **设计目标**：严格模块化、低耦合、海思相关耦合全部收敛到少量钩子，**方便后续把 UI 代码整体迁移到真实拇指相机硬件**。

---

## 2. 目录结构与模块职责

所有 UI 代码位于 `iCamera/DVR_SDK/ui/` 下（与 SDK 的 COM/NET/PAR/LCD 等模块平级）。源码按 9 个子模块目录隔离，**最终聚合成一个 `libui.a`**；第三方 LVGL 独立编成 `liblvgl.a`。

```
iCamera/DVR_SDK/ui/
├── makefile              # 聚合构建：收集 9 个模块的 .o → 单个 libui.a；并驱动 lvgl 子构建
│
├── ui_common/            # 公共基础层：被所有模块依赖，零业务逻辑
│   ├── lv_conf.h         #   LVGL 8.3 裁剪配置（240x320 / RGB565 / 自定义 tick / 中文字库默认字体）
│   ├── ui_common.h       #   屏幕尺寸、通用宏、日志、返回码；统一包含头
│   ├── ui_event.h        #   【统一事件契约】按键/点击/视频帧/业务状态 事件枚举+联合体
│   ├── ui_theme.h/.c     #   LVGL style：面板/列表项/焦点高亮/文字 样式
│   ├── ui_res.h/.c       #   配色、字体、图标符号（LVGL 内置 symbol）集中管理
│   └── ui_lang.h/.c      #   【多语言】11 种语言文案表 + ui_lang_tr() 翻译接口（文案唯一来源）
│
├── ui_font/              # 思源黑体裁剪字库（由 tools/font_gen.py 自动生成，勿手改）
│   └── ui_font.h         #   ui_font_sc_12/14/16/20 声明；symbol 经 fallback 回退 montserrat
│
├── tools/                # 构建辅助工具
│   └── font_gen.py       #   字库生成器：扫描 ui_lang.c 字符集 → 裁剪渲染 → LVGL C 字体
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
│   └── ui_input_key.h/.c     # 源1：物理按键扫描（ui_key_scan_hw 弱函数，真机强实现）
│                             # 源2：触摸屏（预留）——驱动投递 UI_EVT_POINT 即生效
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
│   └── ui_setting.h/.c   #   分辨率/循环/WDR/翻转/红外/音频/水印/语言/格式化/恢复出厂/WiFi
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
 [按键扫描线程]  [触摸屏驱动(预留)]  [IPC 子码流线程]   [业务上报线程]
        │              │                │                 │
   ui_input_key_  (投递 UI_EVT_     ui_video_refresh   (biz 事件)
     inject()       POINT)                 │
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
- **keypad indev**：上/下/确认 → `LV_KEY_PREV/NEXT/ENTER`，驱动焦点组导航；
- **pointer indev**：触摸坐标 → 点击/长按/拖动（**真机触摸屏预留**，样机无触摸硬件，注册后休眠）。

> 样机阶段输入只有物理按键 3 键（上/下/确认）。APP JSON 模拟点击逻辑已移除
> （原 `ui_input_app.c` 已删除，cJSON 依赖随之解除）；触摸屏真机接入方式见第 8 章。

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
| `libui.a` | 9 个模块 22 个 .o（含 4 个字库） | ~293K | `lib_Hi3516CV608-static/P3MU/` |
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

/* 4) 触摸屏（真机预留）：触摸驱动线程里把坐标/按下态投递 UI_EVT_POINT */
/* 样机无触摸硬件，无需实现 */
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

---

## 7. 迁移到真机需要改的点（钩子清单）

海思 / 板级耦合全部收敛在以下少数位置，**页面与交互逻辑无需改动**：

| 钩子 | 位置 | 样机默认 | 真机实现 |
|---|---|---|---|
| LCD 整帧送显 | `ui_main_cfg_t.lcd_flush` | 打印占位 | 接底层 `Lcd_SendFrame(pData,size)` |
| 物理按键读取 | `ui_key_scan_hw()`（弱函数，`ui_input_key.c`） | 返回无键 | 读 GPIO/ADC 返回 `UI_KEY_*` |
| 触摸屏 | pointer indev（`ui_input.c`，休眠） | 无触摸硬件 | 触摸驱动投递 `UI_EVT_POINT`（见第 8 章） |
| YUV→RGB | `ui_video_set_yuv2rgb()` | 内置软转 | 海思 VGS/IVE 硬转 |
| VDEC/VENC | `ui_video_set_codec()` | 无 | 录像/抓拍/延时接 SnapShot 及编码 |
| 相机动作 | `ui_cam_set_ops()`（`ui_cam.h`） | 桩（仅改状态+toast） | 真实开始/停止录像、拍照等 |
| 设置读写/生效 | `ui_setting_set_ops()`（`ui_setting.h`） | 默认值 | 持久化 + 下发 ISP/编码 + 格式化/恢复出厂/配网 |
| 回放文件源 | `ui_pb_set_source()`（`ui_page_playback.c`） | 假数据 | 接 SnapShot/录像文件管理 |

---

## 8. 触摸屏接入（真机预留，样机无触摸硬件）

样机阶段输入仅物理按键 3 键；真机触摸屏按以下方式接入，**页面与抽象层零改动**：

1. `ui_input.c` 已注册 **pointer indev**（当前休眠，坐标 0,0 / 抬起态）；
2. 触摸驱动线程读到坐标/按下态后，经 `ui_msgq_post()` 投递 `UI_EVT_POINT` 事件
   （`evt.u.point = {x, y, pressed}`，坐标范围 0~239 / 0~319，原点左上）；
3. UI 线程 `ui_input_feed()` 自动把事件喂给 pointer indev，
   LVGL 即识别点击/长按/拖动，已有页面的按钮/列表/弹窗天然可点。

> 原样机用 APP JSON 模拟触摸/按键的逻辑（`ui_input_app.c`）已删除，
> cJSON 依赖随之解除；真机如需远程调试注入，可按 `UI_EVT_KEY/POINT` 事件结构自行接回。

---

## 9. 多语言与中文字库（i18n）

### 9.1 架构：文案唯一来源 + 表驱动翻译

- **所有用户可见文案**（菜单/设置/toast/确认框/提示）统一收敛到 `ui_common/ui_lang.c` 的语言表，页面代码只写 `ui_lang_tr(UI_STR_XXX)`，**禁止再硬编码 UI 字符串**。
- 支持 **11 种语言**：英语 / 简体中文（默认）/ 繁体中文 / 日语 / 俄语 / 泰语 / 越南语 / 德语 / 法语 / 意大利语 / 葡萄牙语。
- 某语言未翻译的条目填 `NULL`，运行时自动回退英语；`ui_lang_tr()` 永不返回 NULL。
- 语言切换入口：设置页新增 **"语言"** 项（选项用各自母语写法展示：English / 简体中文 / 繁體中文 / 日本語 / Русский / ไทย / Tiếng Việt / Deutsch / Français / Italiano / Português），切换后立即整页刷新名称与取值，并随设置持久化（`ui_settings_t.language`）。
- 关键接口（`ui_common/ui_lang.h`）：

```c
const char  *ui_lang_tr(ui_str_id_t id);      /* 翻译：取当前语言文案 */
void         ui_lang_set(ui_lang_id_t lang);  /* 切语言（须在 UI 线程调用） */
ui_lang_id_t ui_lang_get(void);
const char  *ui_lang_name(ui_lang_id_t lang); /* 语言自称（母语写法） */
```

### 9.2 中文字库：思源黑体裁剪生成

**背景**：LVGL 内置 montserrat 只覆盖拉丁字符，中文显示为方块/空白——这是"中文不显示"的根因。现已引入思源黑体（SourceHanSansSC-Regular）裁剪字库。

- 生成产物：`ui_font/ui_font_sc_12/14/16/20.c`（LVGL 原生 fmt_txt 格式，4bpp 抗锯齿），共约 156KB flash 占用。
- **字符集自动收集**：生成脚本扫描 `ui_lang.c` 全部语言表的字符 + ASCII + 箭头/全角标点补充集，当前 477 字符。**改文案/加语言后必须重新生成字库**，否则新字符无字形。
- **symbol 图标**：`LV_SYMBOL_*`（FontAwesome 私有区）不在中文字库内，各字库通过 `.fallback` 回退到同字号 `lv_font_montserrat_*` 渲染——因此 `lv_conf.h` 中 montserrat 12/14/16/20 **必须保持开启**。
- 字体映射（`ui_res.h`）：`UI_FONT_SMALL=12 / NORMAL=14 / MEDIUM=16 / LARGE=20`；`LV_FONT_DEFAULT` 已切到 `&ui_font_sc_14`，主题/未显式指定字体的标签直接可显中文。
- 重新生成字库：

```bash
pip3 install Pillow                      # 依赖（已验证 Pillow 9.0.1 / fontTools 可用）
cd iCamera/DVR_SDK/ui
python3 tools/font_gen.py <SourceHanSansSC-Regular.ttf路径>   # 默认输出 ui_font/
make libui install PRODUCT_TYPE=P3MU
```

### 9.3 扩展新语言的步骤

1. `ui_common/ui_lang.h`：在 `UI_LANG_MAX` 前加 `UI_LANG_XXX` 枚举；
2. `ui_common/ui_lang.c`：新增 `ui_str_xxx[]` 表（**顺序与 `ui_str_id_t` 完全一致**，缺失条目填 `NULL`），并登记到 `s_tables[]` 和 `s_lang_names[]`；
3. 需要用户切换的话，`s_lang_names[]` 已是选项表，无需改设置页；
4. 重新运行 `tools/font_gen.py` 生成字库 → `make libui install`；
5. 若新增了文案 ID，所有语言表同步补齐（暂时缺的填 NULL 自动回退英语）。

### 9.4 已知限制

- **泰语**：LVGL 8.3 无复杂文本整形（shaping），泰语元音/声调组合符无法精确上下定位，渲染为"基字符 + 组合符近似叠加"，可读但非出版级。需出版级效果需升级 LVGL 9.x（内置简化 shaping）或整行位图方案。
- 越南语（预组合拉丁扩展）、俄语（西里尔）、日语（假名+汉字）均正常。
- 字库当前覆盖"全部语言 UI 文案的实际字符"，不含任意汉字全集；显示新词前先跑 font_gen.py。

---

## 10. 业务与页面导航

工作模式（`ui_work_mode_t`）：录像(默认)/拍照/延时摄影/回放/设置。

**3 键布局（上/下/确认，无实体返回键）**：

```
      ┌──────────── 预览页 (PREVIEW) ───────────────┐
      │  OK=快门/录制   ▲▼=循环切 5 个模式            │
      └───────┬───────────────────┬─────────────────┘
        切到回放(▲▼)         切到设置(▲▼)
              ▼                   ▼
      回放 (PLAYBACK) ◄─返回─┐ 设置 (SETTING) ◄─返回─┘
              │              │        │
              └──[屏幕"返回"按钮]──┘ 回到预览并恢复拍摄模式
```

- 预览页 ▲▼ 在全部 5 个模式间循环：切到"回放/设置"时自动进入对应页面；页面内 UP/DOWN/OK 走 LVGL 焦点组导航。
- **页面退出**：无实体返回键，统一用屏幕右上角"返回"按钮（设置页/回放页），点击后 `ui_cam_mode_back()` 恢复进入前的拍摄模式并回预览。
- 模式菜单页 (MENU) 保留注册（当前 3 键布局下无入口，页面与钩子保留备用）；选中项即退出。
- 预览页 1Hz 定时器驱动 `ui_cam_tick_1hz()`：累加录制时长、递减剩余容量、广播给状态栏。

---

## 11. 注意事项 / 已知事项

1. **LVGL 源码纳管**：`ui/lvgl/src` 含约 192 个第三方文件（~16M）。如不想纳入自有仓库，可在 `.gitignore` 忽略 `iCamera/DVR_SDK/ui/lvgl/`，改用 git submodule 或构建时拉取（当前是直接放入源码）。
2. **LVGL 配置**：裁剪在 `ui_common/lv_conf.h`。编译时用 `-DLV_CONF_INCLUDE_SIMPLE` + `-Iui_common` 让 LVGL 找到它。调控件开关、字体、内存池大小都改这里。
3. **公共头依赖**：任何对外 `.h` 若用到 `lv_*` 类型，必须 `#include "lvgl.h"`（曾因 `ui_popup.h` 漏包含在交叉编译时报错，已修）。
4. **工具链路径**：`my_gui/Makefile` 里写的 `/opt/linux/...` 在本机不存在，实际工具链在 `/opt/toolchains/hi3516c_v610/...`，已在 PATH，`ui/makefile` 直接按名字调用。
5. **内存**：显示缓冲为 240×40 双缓冲（部分刷新），LVGL 内存池 48KB，均可在 `ui_port.c` / `lv_conf.h` 调整。

---

## 12. 验证记录

### 2026-11-21（修复确认弹窗关闭后按键失效）

- ✅ 根因：确认框关闭时删除了自己的焦点组，但 keypad indev 未恢复页面组 → 悬空指针，按键全失效。
- ✅ 修复：`ui_popup_confirm()` 打开时保存当前组（新增 `ui_input_get_group()`），关闭时恢复并重新聚焦原对象。

### 2026-11-21（移除 APP JSON 输入，预留触摸屏）

- ✅ 删除 `ui_input_app.c`（JSON 模拟点击/按键解析整体移除，cJSON 依赖解除，libui 不再链接 cJSON）。
- ✅ pointer indev 保留并休眠，作为真机触摸屏接入点：触摸驱动投递 `UI_EVT_POINT` 即生效，页面零改动（README 第 8 章）。
- ✅ `ui_integration_example.c` 示例同步改为触摸预留写法。
- ✅ `libui.a` 重建 + 链接自检通过（my_gui 无 `ui_input_app_on_json` 调用点，无需改动主程序）。

### 2026-11-21（3 键布局改造）

- ✅ 删除 `UI_KEY_BACK`（框架、按键映射、各页面 BACK 拦截全部清理，零残留引用）。
- ✅ 预览页 ▲▼ 循环切换全部 5 个模式（录像/拍照/延时/回放/设置），切到回放/设置自动进页。
- ✅ 设置页/回放页新增屏幕"返回"按钮（`UI_ICO_LEFT` + 多语言文案，焦点组内可达）。
- ✅ 新增 `ui_cam_mode_back()`：退出回放/设置时恢复进入前的拍摄模式。
- ✅ 新增 `UI_STR_BACK` 文案（11 语言表同步），字库重新生成（489 字符）。
- ✅ `libui.a` `-Wall -Wextra` 零警告重建 + 链接自检通过（libui+liblvgl+libcommon，零未定义符号）。

### 2026-11-20（多语言 + 中文字库）

- ✅ `libui.a`（22 objs / 9 模块, 293K, armv7）在 `-Wall -Wextra` 下零警告通过。
- ✅ 思源黑体裁剪字库 4 个（12/14/16/20px, 477 字符, 4bpp）生成并编入 `libui.a`。
- ✅ `LV_FONT_DEFAULT` 切换中文字库后 liblvgl.a 重新编译通过。
- ✅ 模拟 `my_gui` 链接 `libui.a + liblvgl.a`（`--start-group`），**零未定义符号**，产出合法 ARM musl ELF。
- ✅ 全部硬编码 UI 文案迁移至 `ui_lang.c`（脚本扫描确认：字符串字面量中文仅存在于 ui_lang.c）。
- ✅ 字库垂直度量与 LVGL 绘制公式核对（`ofs_y = 字形底到基线距离`，对照 montserrat_14 '0'/'a'/'g' 实测数据）。

### 2026-11-19（初版）

- ✅ `liblvgl.a`（192 objs, 741K, armv7）交叉编译通过。
- ✅ `libui.a`（17 objs / 8 模块, 87K, armv7）在 `-Wall -Wextra` 下零警告通过。
- ✅ 模拟 `my_gui` 主程序将 `libui.a + liblvgl.a` 以 `--start-group` 链接，**零未定义符号**，产出合法 ARM musl 可执行文件——自研 UI ↔ 真实 LVGL 8.3 全栈打通。
- ✅ 关键符号 `ui_main_init / ui_video_refresh / ui_page_push` 正确导出。
```
