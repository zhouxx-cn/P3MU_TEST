# CircleQueue 循环缓冲区

基于 IPC 项目共流缓冲区改造而来，用于拇指相机项目的**录像取流**及后续共流场景。

模块提供固定容量的内存循环缓冲区：生产者（venc/audin）持续写入码流，消费者（录像模块、APP 预览、UVC）按需读取。

---

## 1. 文件结构

| 文件 | 说明 |
| --- | --- |
| `include/CircleQueue.h` | 对外 C 接口、参数结构体、错误码定义 |
| `include/DataQueue.h` | 内部实现：数据节点 `DataNode`、队列 `DataQueue`、帧类型枚举 |
| `include/User.h` | 内部实现：消费者 `User` |
| `CircleQueue.cpp` | C 接口导出层，负责加锁与句柄转换 |
| `DataQueue.cpp` | 队列核心逻辑：写入、读取、回收、清空、统计 |
| `User.cpp` | 消费者读写进度管理 |

---

## 2. 核心概念

### 2.1 双队列结构

```
m_listQueue    正常队列，保存当前有效的所有帧（按写入时间升序）
m_listExpired  超时队列，保存已被挤出但仍被消费者占用的节点
```

节点被挤出正常队列时，若仍被某消费者持有（引用计数 > 0），则移入 `m_listExpired` 延迟回收；消费者释放后立即释放内存。

### 2.2 节点序号（seq）

- 每个节点有单调递增的 `s32Seq`，**只增不减，从不重置**。
- 消费者通过 `m_s32SeqLast` 记录已读到的序号。
- 清空缓冲区后新写入的帧 seq 必然大于所有老消费者的 seq，因此老消费者能通过
  `m_listQueue.begin()->Seq() > pUser->Seq()` 自动判断"队列已被重置"，无缝切换到新数据流。

### 2.3 引用计数

- `GetNextNode` 命中节点时 `AddRef()`，`ReleaseNode` 时 `Release()`。
- 同一节点可被多个消费者同时持有，只有计数归零才会真正释放内存。
- 消费者**必须**在 `CircleQueue_GetData` 之后调用 `CircleQueue_ReleaseData`，否则节点无法回收。

---

## 3. 生产者契约

调用方（venc/audin 回调）需保证：

1. **帧类型必须准确标注**。`s32FrameType` 决定该帧是否为 I 帧，录像模块依赖它定位起始帧。
   I 帧类型取值见 `include/DataQueue.h` 的 `FrameType_t`：
   `PktIFrames` / `PktQCIFIFrames` / `PktSQCIFIFrames` / `PktFourthIFrames` /
   `PktCaptureIFrames` 及 `ForS1` / `ForS2` / `ForS3` 系列。

2. **时间戳**：`s64Timestamp` 为 unix 时间戳（微秒），用于生成 MP4 容器的时间戳。

3. **内存归属**由队列创建时的 `enMode` 决定：

   | 模式 | 行为 | 调用方责任 |
   | --- | --- | --- |
   | `CQ_MODE_COPY` | 队列内部 `malloc` + `memcpy` 一份副本 | 调用方可自行释放 `pData` |
   | `CQ_MODE_BYPASS` | 队列不拷贝，直接持有调用方的指针 | 调用方**不得**释放 `pData`，由队列负责释放 |

4. 队列**不感知生产者状态**。venc 异常退出时由录像模块自行超时退出。

---

## 4. 接口说明

### 4.1 老接口（保持不变，兼容 IPC 现有调用）

| 接口 | 说明 |
| --- | --- |
| `CircleQueue_Create` | 创建队列 |
| `CircleQueue_Destroy` | 销毁队列 |
| `CircleQueue_AddUser` | 添加消费者，**首次取流从最新一帧开始** |
| `CircleQueue_DeleteUser` | 删除消费者 |
| `CircleQueue_AddData` | 写入一帧 |
| `CircleQueue_GetData` | 读取一帧（引用计数 +1） |
| `CircleQueue_ReleaseData` | 释放一帧（引用计数 -1） |
| `CircleQueue_SetLogLevel` | 设置日志级别 |

### 4.2 新增接口（录像/预录制使用）

#### `CircleQueue_AddUserEx` — 支持起始策略

```c
typedef struct xrsCQ_USER_PARAM_EX_S
{
    CQ_USER_PARAM_S stBase;        // 通道/码流过滤条件
    CQ_START_POLICY_E enStartPolicy;// 首次取流的起始策略
    int32_t s32StartOffsetMs;      // 预录制回溯时长（毫秒）
} CQ_USER_PARAM_EX_S;
```

起始策略：

| 策略 | 含义 | 适用场景 |
| --- | --- | --- |
| `CQ_START_NEWEST` | 从最新一帧开始（等同老接口行为） | APP 预览、UVC 预览 |
| `CQ_START_LATEST_IDR` | 从缓冲区中**最近的一个 I 帧**开始顺序取流 | 普通录像 |
| `CQ_START_IDR_BEFORE_OFFSET` | 从 `[当前时刻 - s32StartOffsetMs]` 之前的**最近一个 I 帧**开始顺序取流 | 预录制录像 |

> **注意**：`s32Stream == 3`（MJPEG）时老接口会强制每次拉取最新帧。
> `AddUserEx` 中仅当策略为 `CQ_START_NEWEST` 时才保留该行为；
> 录像场景指定了 I 帧策略后按顺序取流，否则会漏帧。

#### `CircleQueue_Clear` — 清空缓冲区

结束录像时调用，快速清空且不阻塞状态机：

- 锁内仅做链表节点搬运（O(n) 指针操作），**内存释放放在锁外**，避免长时间占用锁影响 venc 写入。
- 仍被消费者占用的节点移入超时队列，等消费者释放后回收。
- **seq 不重置**，老消费者在缓冲区重新有数据后自动切换到新数据流。
- 同时释放并重置所有消费者的当前占用与进度。

#### `CircleQueue_FlushUser` — 重置单个消费者

强制释放该消费者当前占用的节点并重置其读取进度，下次取流按其起始策略重新定位。用于录像模块中途重连。

#### `CircleQueue_GetStatus` — 查询缓冲区状态

```c
typedef struct xrsCQ_STATUS_S
{
    int32_t s32UsedSpace;   // 已占用内存（字节）
    int32_t s32TotalSpace;  // 缓冲区总容量（字节）
    int32_t s32NodeCount;   // 当前有效帧数
    int32_t s32BufferedMs;  // 缓冲区覆盖的时间长度（毫秒）
    int32_t s32HasIdr;      // 缓冲区中是否存在 I 帧
} CQ_STATUS_S;
```

---

## 5. 错误码

| 错误码 | 值 | 说明 |
| --- | --- | --- |
| `XRS_CQ_OK` | 19000 | 成功 |
| `XRS_CQ_ERR_INVALID_HANDLE` | 19001 | 队列句柄无效 |
| `XRS_CQ_ERR_INVALID_USER` | 19002 | 消费者句柄无效 |
| `XRS_CQ_ERR_TOOMUCH_USER` | 19003 | 消费者数量达上限（`CQ_MAX_USER` = 16） |
| `XRS_CQ_ERR_INVALID_PARAM` | 19004 | 参数错误 |
| `XRS_CQ_ERR_BUSY` | 19005 | 队列繁忙 |
| `XRS_CQ_ERR_USER_BUSY` | 19006 | 消费者繁忙（未释放上一帧） |
| `XRS_CQ_ERR_EMPTY` | 19007 | 缓冲区为空 |
| `XRS_CQ_ERR_NOSUCH_DATA` | 19008 | 节点不存在 |
| `XRS_CQ_ERR_OOM` | 19009 | 内存不足 |
| `XRS_CQ_ERR_TAIL` | 19010 | 已读到缓冲区结尾 |
| `XRS_CQ_ERR_NO_IDR` | **19011** | **缓冲区内未找到 I 帧**（录像起始定位失败） |
| `XRS_CQ_ERR_INVALID_STATE` | 19012 | 当前状态不允许该操作 |

---

## 6. 典型使用流程

### 6.1 普通录像

```
用户点击开始录像
    │
    ├─ venc 开始编码 → 码流持续写入 CircleQueue
    │
    ├─ CircleQueue_AddUserEx(policy = CQ_START_LATEST_IDR)
    │
    ├─ CircleQueue_GetData() 循环取流
    │     └─ 首次定位到最近的 I 帧（含 SPS/PPS/VPS），之后顺序取流
    │     └─ 非目标通道/码流的帧会被自动跳过并释放
    │
用户点击结束录像
    │
    ├─ 停止取流，CircleQueue_DeleteUser()
    ├─ 停止 venc 编码
    └─ CircleQueue_Clear()  ← 快速清空，不阻塞状态机
```

### 6.2 预录制录像

```
预录制开启 → venc 持续编码，缓冲区滚动保留最近 N 秒
    │
用户点击开始录像
    │
    ├─ CircleQueue_AddUserEx(policy = CQ_START_IDR_BEFORE_OFFSET, offset = N*1000)
    │
    ├─ 定位逻辑：
    │     ① 找到时间点 [now - N秒] 之前最近的 I 帧 → 从此开始取流
    │     ② 若缓冲区覆盖不到 N 秒（预录时长超出缓冲范围）
    │        → 回退到缓冲区内最老的 I 帧（尽力而为）
    │        → 录像模块可通过对比首帧时间戳得知实际预录长度
    │
    └─ 后续同普通录像
```

### 6.3 时序约束（重要）

**在状态机停止 venc 流之前，必须先停止录像模块的接收。**

否则 venc 停止后仍可能有数据在写入缓冲区，录像模块会取到不完整的一帧序列。

### 6.4 SPS/PPS/VPS 处理

SPS/PPS/VPS **随 I 帧一起入队**。因此从 I 帧起始取流时，MP4 封装无需额外处理头节点，
首个写入 MP4 的包即为带参数集的 I 帧。

---

## 7. 容量与预录时长估算

缓冲区容量按码率估算：

```
所需容量 ≈ 码率 × 预录时长
```

参考（当前为 IPC 模拟硬件配置，后续按实测定）：

| 码率 | 10 秒预录 | 30 秒预录 |
| --- | --- | --- |
| 20 Mbps | 25 MB | 75 MB |
| 40 Mbps | 50 MB | 150 MB |

当前最大缓冲区暂定 **10 MB**。40 Mbps × 10 秒 ≈ 50 MB，加上音频与余量约 64 MB，
按需在 `CircleQueue_Create` 的 `s32MaxSize` 中调整。

> GOP 暂定 2 秒。I 帧间隔直接决定了预录制回溯的精度：
> 从 I 帧起始取流意味着实际预录时长在 `[设定值 - GOP, 设定值]` 区间内浮动。

---

## 8. 使用约束

1. 消费者数量上限 `CQ_MAX_USER` = 16。
2. `GetData` 与 `ReleaseData` 必须成对调用，未释放的节点不会被回收。
3. 同一消费者在未 `ReleaseData` 前再次 `GetData` 会返回 `XRS_CQ_ERR_USER_BUSY`。
4. `CQ_MODE_BYPASS` 模式下调用方不得释放 `pData`，由队列统一回收。
5. 队列不做生产者状态感知，异常退出需由上层处理。
6. 慢动作场景不考虑预录制。

---

## 9. 已知限制

- `CQ_HANDLE` / `CQ_USER` 为 `uint32_t`，仅适用于 32 位平台（交叉编译目标 ARM）。
- `CircleQueue_SetLogLevel` 尚未实现，日志级别由编译期的 `CQ_LOG_LEVEL` 宏控制。
