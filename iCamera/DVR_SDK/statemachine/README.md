# hsm — 拇指相机整机层次化状态机（HFSM）模块

拇指相机整机业务的层次化状态机（Hierarchical Finite State Machine）核心模块。
纯 C、零第三方依赖（仅 pthread），基于开源 HSM 引擎（MIT）封装，
配套消息队列、单线程派发器和系统定时器，可对接 LVGL GUI、按键检测、
APP 远程协议等任意事件生产者。

本 README 是状态机设计与使用的唯一文档。

---

## 1. 目录结构

```
statemachine/
├── hsm.h / hsm.c               开源 HSM 引擎（MIT，https://github.com/Ye-Zenghui/hsm 拷贝适配）
├── thumcam_hsm.h               对外唯一公共接口（消息枚举 / 状态声明 / API）
├── thumcam_hsm_internal.h      框架内部结构（业务层勿 include）
├── thumcam_hsm.c               框架封装：消息队列 + 单线程派发 + 系统定时器
├── thumcam_states.c            全部 19 个状态实现（硬件动作仅占位注释）
├── makefile                    复用 DVR_SDK/common.mak，产出 libstatemachine.a
└── README.md                   本文档
```

---

## 2. 架构分层

```
事件生产者（多线程）                    上层业务实现
┌─────────────────────────────────────────────────┐
│  GPIO按键检测   LVGL触摸回调   WiFi/APP协议线程  │
│  硬件检测线程(SD/电池/温度)   媒体完成通知       │
└──────────────────┬──────────────────────────────┘
                   │ ThumCam_HSM_PostEvent(sig, param)   ← 线程安全
                   ▼
┌─────────────────────────────────────────────────┐
│  statemachine                                    │
│  消息队列(64) → HFSM派发线程 → hsm引擎状态跳转  │
│  屏保/休眠/自动关机定时器（每轮循环轮询）         │
└──────────────────┬──────────────────────────────┘
                   │ ThumCam_HSM_RegisterStateChangeCb() ← 状态切换通知
                   ▼
┌─────────────────────────────────────────────────┐
│  GUI 刷新（LVGL）/ 业务响应                      │
└─────────────────────────────────────────────────┘
```

**分层原则（重要）**：

1. 按键、LVGL GUI、APP 协议只负责**产生事件**，投递消息给状态机；
2. 状态机变更状态后，通过状态切换回调**通知 GUI 刷新**；
3. 状态机**不直接调用** UI 和媒体驱动。ISP/VENC/AAC/SD/LVGL 等硬件与媒体动作
   在 Enter/Exit 回调里只写 `TODO(业务层)` 占位注释（统一 `HWSLOT` 宏输出日志），
   由上层业务实现。

---

## 3. 状态层级

```
top_state（引擎内置根，兜底吞掉一切未处理事件）
├── POWER_OFF                关机（插USB仅充电，不自动开机）
├── PRODUCT_TEST             产测模式（上电检测SD产测标记；当前仅状态壳）
└── WORKING【超级父：SD拔出/低电/过热/长按关机/自动关机 统一处理】
    ├── IDLE_PREVIEW         待机预览（EXIT 不关 ISP → 录像快起）
    ├── SCREEN_SAVER         屏保：只关背光，业务不停，唤醒回原状态
    ├── SYS_SLEEP            休眠：关 ISP/VENC/音频，任意键唤醒回预览
    ├── REC_GROUP            录像父：屏蔽休眠、允许屏保、停止录像统一入口
    │   ├── REC_NORMAL       普通录像（预录 + 分段 + 循环）
    │   ├── REC_TIMELAPSE    延时缩时录像（MSG_MODE_KEY 循环切换）
    │   └── REC_SLOW_MOTION  慢动作录像
    ├── PHOTO_GROUP          拍照父：拍完 MSG_PHOTO_DONE 统一回预览
    │   ├── PHOTO_SINGLE     单拍
    │   └── PHOTO_DELAY      倒计时拍照（倒计时中再按 = 取消）
    ├── PLAYBACK             回放（循环翻页 / 删除 / 触摸返回）
    ├── SETTING_MENU         设置菜单（只改内存配置，不碰硬件）
    └── USB_GROUP            USB 插入：弹 UI 选模式；家族内禁录像/拍照
        ├── USB_CHG_ONLY     仅充电（默认）
        ├── USB_UVC          UVC（MSG_USB_MODE_SELECT 选择）
        └── USB_MASS_STORAGE U 盘
```

**关键机制**：

| 机制 | 实现方式 |
|---|---|
| 事件上浮 | 子状态 `return HSM_UNHANDLED()` → 自动交给父状态 → 直到根 |
| 录像屏蔽休眠 | REC_GROUP 对 `MSG_SLEEP_TIMEOUT` 返回 UNHANDLED，WORKING 丢弃 |
| 录像允许屏保 | REC_GROUP 对 `MSG_SAVER_TIMEOUT` 用 `HSM_TRAN_KEEP` 跳 SCREEN_SAVER：业务状态链**不退出**，录像不中断 |
| 屏保唤醒回原状态 | SCREEN_SAVER 用 `HSM_RESUME` 恢复：只发屏保的 EXIT，**不重发**业务链 ENTRY，录像不重启 |
| 屏保下的业务事件 | 引擎在事件上浮时把叠加态未处理的事件先交给被保留的业务链（分片结束/停录不会丢） |
| USB 插入 | 任意状态插 USB → 停止当前业务（录像则停录）→ 切 USB_GROUP（WORKING 兜底） |
| USB 禁录像 | USB_GROUP 父状态吞掉 REC/PHOTO/REMOTE 类消息 |
| 兄弟状态互斥 | HSM 单 current 天然互斥（USB 三模式、录像三模式） |

**屏保（SCREEN_SAVER）是"叠加态"而不是普通模式**：

普通模式之间切换是"退出旧状态链 → 进入新状态链"，屏保不能这么做——一退出
`REC_NORMAL`/`REC_GROUP` 的 EXIT，录像就被业务层停掉了，与"屏保只关背光、
录像继续"的产品要求冲突。因此引擎提供了一组正交区域（浅历史）跳转：

```c
HSM_TRAN_KEEP(&screen_saver_state);  /* 进入叠加态：不退出源业务状态链 */
HSM_RESUME();                         /* 恢复被保留的业务状态：不重发 ENTRY */
```

进入时引擎把源业务状态保存在 `me->saved`；离开叠加态时（无论是唤醒恢复还是
切到别的模式）引擎负责退出这条被保留的业务链，保证业务层只收到一次 EXIT。

退出顺序由引擎统一计算：把"叠加态链 + 被保留业务链"求并集、去掉仍属于目标
状态链的状态后去重，再按**深度从深到浅**依次发 EXIT——因此公共祖先（如
`WORKING`）只会被退出一次，且父状态一定在子状态之后退出。

---

## 4. 各状态设计说明（Enter / Exit / 事件处理与设计理由）

### POWER_OFF 关机（初始态）

- Enter：全部媒体硬件关闭（ISP/VENC/AUDIO）；屏幕熄灭；低功耗待命，
  维持电池检测、按键检测。
- Exit：电源按键短按开机 → `WORKING`（其 INIT 下钻到 `IDLE_PREVIEW`）。
- 事件处理：电源键短按=开机；USB 插入=仅充电，**不自动开机**。

> 理由：拇指相机装电池/插 USB 即上电，"上电 ≠ 开机"，POWER_OFF 是通电待命
> 基态；关机插 USB 只充电，不启动业务。

### PRODUCT_TEST 产测模式

- 触发：上电阶段检测 SD 卡产测标记文件，直接切入，跳过正常业务。
- Enter：关闭 ISP 预览；加载产测私有 WiFi，启动产测指令服务
  【当前仅搭建状态壳，具体产测业务后续实现】。
- Exit：产测标记消失 / 收到产测退出指令，切回 `POWER_OFF`。

> 理由：独立分支，工厂产测逻辑不污染正常业务状态。

### WORKING【超级父状态，所有正常业务的父】

- Enter：初始化电池管理、自动关机/休眠/屏保计时器；开启基础外设；不开 ISP。
- Exit：跳转关机，停止全部定时器。
- 子状态不处理的上浮事件在此统一处理：
  1. `MSG_SD_REMOVE`：正在录像则停录，弹窗，切回预览；
  2. `MSG_BAT_LOW`：低电告警；阈值过低触发关机；
  3. `MSG_OVER_TEMP`：停止编码、弹窗，严重直接关机；
  4. `MSG_POWER_KEY_LONG`：长按电源执行关机流程；
  5. `MSG_AUTO_POWEROFF`：自动关机；
  6. `MSG_SLEEP_TIMEOUT`：只有非录像态才允许跳 `SYS_SLEEP`，
     录像家族上浮的休眠事件直接丢弃；
  7. `MSG_USB_INSERT`：**非预览态插 USB → 停止当前业务（录像则停录）→
     切 USB_GROUP**。"先停业务再切换"的顺序由状态链退出语义天然保证
     （跳转时源业务链的 EXIT 会执行，业务层在 EXIT 里停录/停拍）。

> 核心价值：SD 异常、低电、过热、关机逻辑只写一份，所有子状态共享。

### IDLE_PREVIEW 待机预览（常态）

> 屏幕 YUV 直输出 LCD，ISP 3A/EIS 运行，VENC 编码器关闭；等待用户操作。

- Enter：打开 ISP/VPSS 通路 YUV 直送 LCD；开 3A、EIS 防抖、重力旋转；
  VENC/AAC 保持关闭；重置屏保/休眠/自动关机计时器；点亮屏幕。
- Exit：**不关闭 ISP**（切录像复用 ISP 避免卡顿，实现快起）；
  只有切休眠/关机才关闭 ISP。
- 事件：录像键 → `REC_NORMAL`；拍照键 → `PHOTO_SINGLE`；回放键 → `PLAYBACK`；
  设置键 → `SETTING_MENU`；USB 插入 → `USB_GROUP`；
  屏保超时 → `HSM_TRAN_KEEP(&screen_saver_state)`（叠加进入，本状态链不退出）；
  休眠超时 → `SYS_SLEEP`；APP 远程录像/拍照消息同物理按键。

### SCREEN_SAVER 屏保（叠加态，机制详见第 3 节）

- Enter：引擎已把业务状态链存入 `me->saved`；关闭 LCD 背光；
  ISP/VENC/AAC **保持不变**（业务链 EXIT 不触发）；屏保计时停止，
  重置休眠/自动关机计时器。
- Exit：打开背光；唤醒用 `HSM_RESUME()` 回被保留的业务链
  （可能是 IDLE_PREVIEW / REC_NORMAL / PLAYBACK 等任意状态）。
- 叠加期事件路由：屏保不关心的业务事件（分片完成、停录等）由引擎交给
  被保留的业务链处理，不会被屏保吞掉。
- 查询语义：判断"是否在录像"用 `IsInState`（叠加感知），
  判断"当前屏幕显示什么"用 `GetStateName`（见第 11 节第 10 条）。

> 与 SYS_SLEEP 对比：屏保轻量、唤醒快、录像可用；休眠彻底、唤醒慢、省更多电。

### SYS_SLEEP 休眠低功耗

- Enter：熄屏；关闭 ISP/VENC/AAC；维持电池检测、按键检测。
- Exit：按键唤醒回 `IDLE_PREVIEW`，重新打开 ISP 预览通路。
- 入口：只有 IDLE_PREVIEW 无操作超时才进；**录像家族屏蔽此事件**
  （上浮到 WORKING 被丢弃）。

### REC_GROUP【录像父状态】

- Enter：打开 AAC 音频编码；初始化 MP4 封装上下文。
- Exit：停止 AAC；关闭 MP4 句柄；释放媒体资源。
- 公共事件：
  - `MSG_REC_STOP` / `MSG_REMOTE_REC_STOP`：无论哪种子录像模式统一停录回预览；
  - `MSG_SLEEP_TIMEOUT`：返回 UNHANDLED 拒绝休眠（WORKING 丢弃）；
  - `MSG_SAVER_TIMEOUT`：**本家族正常处理**——`HSM_TRAN_KEEP` 叠加进屏保，
    只关屏幕录像不停；
  - `MSG_SD_REMOVE`：UNHANDLED 交 WORKING 统一停机；
  - `MSG_PHOTO_KEY` / `MSG_REMOTE_SNAP`：忽略（录像中不允许拍照，
    如需录像抓拍后续扩展）。

#### REC_NORMAL 普通录像（唯一支持预录）

- Enter：读取配置（分辨率/编码格式、**循环/分段参数**、重力旋转）；
  启动主码流 VENC；初始化**预录制环形缓存**；启动 MP4 分片/循环封装。
- Exit：停止编码；结束 MP4 写入；回收预录缓存。
- 内部消息：`MSG_REC_SEGMENT_DONE` 分段完成 → 关当前段、开新段（循环录制）。

> 预录制/分段/循环的**具体业务逻辑放在媒体模块**，状态机只负责
> "开始预录"（Enter 触发）、"停止预录"（Exit 触发）和分段完成消息。

#### REC_TIMELAPSE 延时缩时录像（无预录）

- Enter：加载缩时间隔；配置 ISP 帧抽取；修改时间戳逻辑；启动 MP4 封装；
  录像文件跟随重力旋转。
- Exit：停止编码，完成缩时 MP4 输出。

> 时间戳修改业务逻辑放媒体工作队列，**不要阻塞 HFSM 线程**，状态机仅控制启停。

#### REC_SLOW_MOTION 慢动作录像（无预录）

- Enter：Sensor 切换高帧率（1080P-60/120fps）；配置 VENC；
  业务层做时间戳压缩实现慢放；录像文件跟随重力旋转。
- Exit：关闭高帧率模式，恢复预览帧率。

### PHOTO_GROUP【拍照父状态】

- Enter：初始化 JPEG 抓拍相关资源。
- Exit：释放抓拍临时资源。
- 上浮事件：抓拍完成（`MSG_PHOTO_DONE`）统一切回 IDLE_PREVIEW。

#### PHOTO_SINGLE 普通单拍

- Enter：触发 Snapshot，取 ISP YUV 编码 JPEG 写入 SD 卡，完成直接切回预览。

#### PHOTO_DELAY 倒计时定时拍照

- 入口：`MSG_PHOTO_DELAY` 从 IDLE_PREVIEW 进入（GUI 触摸 / APP 远程 /
  长按拍照键任一来源）。
- Enter：启动倒计时软件定时器；倒计时结束触发 Snapshot 抓拍。
- 倒计时期间再按一次拍照键 → **取消抓拍**，直接返回 IDLE_PREVIEW
  （不走 PHOTO_SINGLE）。
- 倒计时期间**不响应屏保超时**（倒计时 tick 与屏保计时互斥，避免倒计时被冻结）。

> 拍照属瞬时动作，拍完立刻返回预览，符合拇指相机使用习惯。

### PLAYBACK 文件回放

- Enter：关闭 VENC；初始化解码器；读取 SD 相册列表；解码 JPEG/MP4 输出 LCD；
  **关闭重力画面旋转（回放不需要旋转）**。
- Exit：销毁解码器资源，释放内存，回到 IDLE_PREVIEW。
- 事件：上下翻页**循环翻页**（最后一个再翻 → 第一个）；删除文件；
  触摸返回退出。

### SETTING_MENU 设置菜单

- Enter：UI 切换设置页面；停止录制业务；屏幕保持点亮。
- Exit：退出菜单回 IDLE_PREVIEW。

> **重要原则：菜单只修改内存配置，不直接操作 ISP/VENC 硬件；配置在进入
> 录像/预览的 Enter 中加载生效。** 例：修改分辨率仅写全局配置结构体，
> 真正改变编码参数是 REC_NORMAL 的 Enter。

### USB_GROUP【USB 插入父状态】

- 插入即**弹 UI 让用户必须选择一个模式**（默认仅充电）；
  家族内禁止本地录像/拍照（父状态吞掉 REC/PHOTO/REMOTE 类消息）。
- 插 USB 统一规则（任意状态生效）：停止当前业务 → 切 USB_GROUP，
  由 WORKING 兜底处理；重复 `MSG_USB_INSERT` 直接忽略。
- `HSM_INIT_SIG`：按用户选择的模式 `HSM_TRAN` 进入对应子状态。
- Exit：USB 拔出，回 IDLE_PREVIEW。

子状态互斥三选一：

1. **USB_CHG_ONLY 仅充电**：相机完整功能可用（预览、回放、设置），只是不允许录像拍照；
2. **USB_UVC UVC 模式**：ISP 切换为 UVC 输出通路；关闭本地 VENC、SD 存储；
3. **USB_MASS_STORAGE U 盘模式**：SD 卡挂载给 USB 主机；本地禁止读写 SD 卡。

---

## 5. hsm 引擎核心 API

框架来自开源 HSM（MIT 许可证）vendored 拷贝到本目录，
核心仅 110 行 hsm.h + 121 行 hsm.c，零依赖，适合嵌入式。

> **上游原文件保持不动**，拷贝件相对上游的差异：① `HSM_ASSERT` 由
> while(1) 死循环改为日志；② 正交区域扩展（`saved` 字段、`HSM_TRAN_KEEP`/
> `HSM_RESUME`、退出链收集）；③ `current == NULL` 空指针防护。
> 升级上游时按该清单逐条重新套用，不要整文件覆盖。

| 函数/宏 | 作用 | 我们的用法 |
|---|---|---|
| `hsm_init(&me, &init_state)` | 初始化状态机 | HFSM 线程启动时调用，从初始状态开始 |
| `hsm_dispatch(&me, &event)` | 分发事件 | HFSM 线程循环：取消息 → dispatch |
| `HSM_TRAN(&target)` | 常规跳转（退源链+进目标链） | 状态函数里 return `HSM_TRAN(...)`；**自跳转是空操作**（不发 EXIT/ENTRY） |
| `HSM_TRAN_KEEP(&target)` | **叠加态跳转**：保留源状态链，只进入目标链 | 屏保叠加到任意业务态（录像不中断） |
| `HSM_RESUME()` | **恢复被保留的业务链**：只退叠加态，不重入业务链 | 屏保唤醒回原业务态；**只能在叠加期间调用**，误用引擎断言告警并保持原状态 |
| `HSM_HANDLED()` | 事件已处理，不再上浮 | 子状态处理完的事件 |
| `HSM_UNHANDLED()` | 事件未处理，上浮到父状态 | 父状态屏蔽子状态事件的关键机制 |
| `HSM_ENTRY_SIG` / `HSM_EXIT_SIG` | Entry/Exit 内置信号 | 每个状态函数 switch 里处理 |
| `HSM_INIT_SIG` | 初始化信号（INIT 下钻） | 父状态进入默认子状态（WORKING→IDLE_PREVIEW、USB 选模式） |

跳转的三种引擎模式（`hsm_tran_ex()` 内处理）：

| 模式 | 触发宏 | 行为 |
|---|---|---|
| TRAN_NORMAL | `HSM_TRAN()` | 退出"叠加态链 + 被保留业务链"，进入目标链，清 `saved` |
| TRAN_KEEP | `HSM_TRAN_KEEP()` | 不退任何链，进入目标链，`saved` = 源状态链 |
| TRAN_RESUME | `HSM_RESUME()` | 只退叠加态链，恢复 `saved` 业务链，不进入新链 |

叠加态期间事件路由：引擎在事件上浮过程中先把事件交给 `saved` 业务链处理，
避免被叠加态吞掉（详见第 3 节）。

---

## 6. 线程模型与安全说明

```
多生产者（任意线程）                    单消费者（唯一派发线程）
ThumCam_HSM_PostEvent()  ──互斥队列──►  hsm_init() / hsm_dispatch()
ThumCam_HSM_NotifyActivity()            （hsm 引擎 current/last/saved 仅此线程修改）
ThumCam_HSM_SetTimeouts()
```

- **消息队列**：`pthread_mutex + pthread_cond`，多生产者单消费者；队满丢弃并告警；
- **hsm 引擎**：仅在派发线程内修改，对外通过 `last_state` 快照只读（GetStateName /
  IsInState 任意线程可调）；
- **状态机初始化**：在派发线程内完成（`ThumCam_HSM_Start()` 等待初始化完成后返回），
  所有 ENTRY/EXIT 都在派发线程上下文，业务层的 Enter/Exit 钩子不会跨线程；
- **定时器变量**：均在 q_lock 内读写；
- **用户回调**：注册/注销加锁，调用时快照局部变量执行，防"注销后悬空"；
- **编译期断言**：`_Static_assert` 保证 `hsm_obj_t super` 位于结构体首地址
  （hsm 引擎按基类指针访问的强转依赖）。

注意：`state_change_cb` 在派发线程上下文执行，回调里不要做重活，
LVGL 界面刷新请 post 到 LVGL 自己的线程任务。

**状态切换通知的时序**：回调在**事件派发完成之后**触发，即目标状态的 ENTRY
已执行、旧状态的 EXIT 已收尾，回调里查询到的状态与硬件实际动作是一致的。
一次派发最多产生一次通知（开机这类"POWER_OFF → WORKING → IDLE_PREVIEW"的
连续跳转只通知一次 `POWER_OFF → IDLE_PREVIEW`）。

---

## 7. 消息速查（完整定义见 thumcam_hsm.h）

| 分类 | 消息 |
|---|---|
| 物理按键 | MSG_POWER_KEY_SHORT / LONG、MSG_REC_KEY、MSG_PHOTO_KEY、MSG_PLAYBACK_KEY、MSG_SETTING_KEY、MSG_MODE_KEY |
| 触摸 | MSG_TOUCH_NEXT / PREV / DELETE / BACK、MSG_USB_MODE_SELECT（param=模式值） |
| APP 远程 | MSG_REMOTE_REC_START / REC_STOP / SNAP / PREVIEW |
| 定时器 | MSG_SAVER_TIMEOUT、MSG_SLEEP_TIMEOUT、MSG_AUTO_POWEROFF、MSG_DELAY_PHOTO_TICK |
| 硬件检测 | MSG_USB_INSERT / REMOVE、MSG_SD_REMOVE、MSG_BAT_LOW、MSG_OVER_TEMP |
| 业务完成 | MSG_PHOTO_DONE、MSG_REC_SEGMENT_DONE、MSG_REC_STOP、MSG_PRODUCT_TEST_EXIT |
| 业务触发 | MSG_PHOTO_DELAY（触发倒计时定时拍照，GUI/APP/长按拍照键产生） |

WiFi 仅作为 APP 通信通道（AP 模式），WiFi 连接/断开本身不联动状态机；
APP 指令以 MSG_REMOTE_* 进状态机，保证录像屏蔽休眠等全局规则一致生效。

**用户活动事件**：上表中的物理按键、触摸、USB 插拔、APP 远程指令、
`MSG_REC_STOP`、`MSG_PHOTO_DELAY` 都登记在 `is_user_activity()` 中，
派发时会重置屏保/休眠/自动关机的计时。新增此类事件时必须同步登记，
否则对应的超时定时器不会重新武装。

---

## 8. 使用示例

```c
#include "thumcam_hsm.h"

static void on_state_change(const char *from, const char *to)
{
    /* 在 GUI 自己的线程里刷新页面，这里只 post 任务 */
    gui_post_state_change(from, to);
}

int main(void)
{
    /* 可选：启动前配置定时器（ms），0=禁用；不配置则用默认值
     * 屏保30s / 休眠60s / 自动关机禁用 */
    ThumCam_HSM_SetTimeouts(30 * 1000, 60 * 1000, 0);
    ThumCam_HSM_SetDelayPhotoSeconds(3);
    ThumCam_HSM_RegisterStateChangeCb(on_state_change);

    /* 上电检测 SD 产测标记文件 */
    int product_test = access("/mnt/sd/PRODUCTTEST", F_OK) == 0;

    if (ThumCam_HSM_Start(product_test) != 0)
        return -1;                      /* 初始态 = POWER_OFF 或 PRODUCT_TEST */

    /* ... 各事件生产者线程启动，例如按键线程： ... */
    /* ThumCam_HSM_PostEvent(MSG_POWER_KEY_SHORT, NULL);   // 开机 */

    /* ... 收到关机完成通知后 ... */
    ThumCam_HSM_Stop();
    return 0;
}
```

投递带值事件（param 传值转指针）：

```c
ThumCam_HSM_PostEvent(MSG_USB_MODE_SELECT,
                      (void *)(intptr_t)THUMCAM_USB_UVC);
```

> param 生命周期：事件被处理前必须保持有效；传值请转指针（如上）。

---

## 9. 如何新增状态

以"新增直播状态 LIVE"（挂在 WORKING 下）为例：

1. `thumcam_hsm.h`：extern 声明 `extern hsm_t live_state;`；
2. `thumcam_states.c`：定义状态对象并实现状态函数：

   ```c
   static HSM_RET_T live_state_func(hsm_obj_t *me, hsm_event_t *e)
   {
       (void)me;
       switch (e->sig)
       {
           case HSM_ENTRY_SIG: /* TODO(业务层): 开直播推流 */ return HSM_HANDLED();
           case HSM_EXIT_SIG:  /* TODO(业务层): 停推流 */     return HSM_HANDLED();
           default:            return HSM_UNHANDLED();
       }
   }
   hsm_t live_state = { .name = "LIVE", .parent = &working_state,
                        .state_func = HSM_STATE_FUNC(live_state_func) };
   ```

3. 在需要进入的位置 `return HSM_TRAN(&live_state);`（如 IDLE_PREVIEW 的某个入口事件）；
4. 若是新消息：`thumcam_hsm.h` 的 `thumcam_msg_e` 中新增 ID
   （若需计为用户活动，同步在 thumcam_hsm.c 的 `is_user_activity()` 中登记）；
5. 若新状态需要"叠加在现有业务之上"（类似屏保），用
   `HSM_TRAN_KEEP()` 进入、用 `HSM_RESUME()` 退出，不要用普通 `HSM_TRAN()`，
   否则源业务状态链会被退出（录像会被停掉）。

---

## 10. 编译

```bash
cd iCamera/DVR_SDK/statemachine
make release PRODUCT_TYPE=P3MU     # 产出 libstatemachine.a 安装到 ../lib_${TYPE}-static/${PRODUCT_TYPE}/
make debug   PRODUCT_TYPE=P3MU
make clean   PRODUCT_TYPE=P3MU
```

纯 C 模块，无第三方依赖；交叉工具链与编译选项由 `../common.mak` 统一提供。

---

## 11. 框架移植注意点

1. `hsm_event_make()` 内部用 static 缓存，仅限派发线程使用（框架内已保证），
   业务多线程投递一律走 `ThumCam_HSM_PostEvent()`；
2. `HSM_ASSERT` 已从原版 `while(1)` 死循环改为日志输出；如需接入
   DVR_SDK 日志模块，可在编译选项覆盖该宏；
3. 模块日志统一走 `THUMCAM_HSM_LOG` 宏（默认输出 stderr），接入 DVR_SDK
   日志同样在编译选项覆盖该宏即可：
   `-DTHUMCAM_HSM_LOG(fmt,...) do{ my_log(fmt, ##__VA_ARGS__); }while(0)`；
4. 屏保/休眠/自动关机超时事件由派发线程每轮循环轮询产生（200ms tick），
   **按"截止时刻"周期性重投**：到点投递一次，若被当前状态拒绝（如录像态
   拒绝休眠），下一个周期会自动重新武装，不会出现"定时器被拒绝一次后永久
   静默、设备再也不休眠"的问题；用户活动（按键/触摸/远程/USB 插拔等）或
   `NotifyActivity()` 会把计时整体推后；
5. 倒计时拍照 tick（1s 周期）仅在当前状态为 PHOTO_DELAY 时投递；
   PHOTO_DELAY 可由 `MSG_PHOTO_DELAY` 从 IDLE_PREVIEW 进入（GUI/APP/长按
   拍照键均可触发）；
6. **ENTRY/EXIT 回调里禁止发起状态跳转**（`HSM_TRAN` 等）。引擎会检测并
   断言告警，同时回滚 `current/last`，但正确做法是让 Enter/Exit 只做
   资源准备与释放，状态切换交给事件处理函数；
7. 状态机内部只维护一个 `current`，兄弟状态天然互斥；若业务上需要"两个模式
   同时存在"，请像屏保一样使用 `HSM_TRAN_KEEP` / `HSM_RESUME` 叠加态，
   而不是硬塞成兄弟状态；
8. **`HSM_RESUME()` 禁令**：只能在 `HSM_TRAN_KEEP` 叠加期间调用。无叠加态时
   误调用（`saved == NULL`）引擎会断言告警并保持原状态不变，不会崩溃
   （引擎在 `hsm_tran_ex` 与 `hsm_dispatch` 都有空指针防护）；
9. **自跳转语义**：`HSM_TRAN()` 到当前状态是空操作（不发送 EXIT/ENTRY），
   与经典 HSM 的"退出再重入"不同；需要重置业务场景请先跳到中间状态；
10. **状态查询是"叠加感知"的**：屏保叠加期间，`ThumCam_HSM_GetStateName()`
    返回叠加态叶子（`SCREEN_SAVER`），但 `ThumCam_HSM_IsInState(&rec_group_state)`
    返回 true（被保留的业务链仍在运行）——业务层判断"是否在录像"请用
    `IsInState`，判断"当前屏幕显示什么"请用 `GetStateName`。
