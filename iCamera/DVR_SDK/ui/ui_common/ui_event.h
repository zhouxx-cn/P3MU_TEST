/**
 * @file    ui_event.h
 * @brief   拇指相机 UI 统一事件契约层。
 *
 * @details
 *  所有"非 UI 线程"（物理按键扫描线程、APP 网络指令线程、IPC 子码流线程、
 *  业务状态上报线程）与 UI 线程之间，只允许通过本文件定义的事件结构体，
 *  经由 ui_msgq 消息队列单向传递。上层页面只消费 LVGL 事件，不感知来源。
 *
 *  设计目标：输入来源可插拔（按键 / APP 模拟点击），迁移到真实拇指相机硬件
 *  时只需替换 ui_input_key / ui_input_app 的"源"，事件契约保持不变。
 *
 * @note  纯 C，禁止 C++。
 */
#ifndef __UI_EVENT_H__
#define __UI_EVENT_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief 事件大类。 */
typedef enum {
    UI_EVT_NONE = 0,    /**< 空事件 */
    UI_EVT_KEY,         /**< 物理按键 / APP 下发的按键码（焦点导航） */
    UI_EVT_POINT,       /**< APP 下发的模拟点击坐标（等效触摸） */
    UI_EVT_VIDEO_FRAME, /**< IPC 子码流一帧就绪（数据在 ui_video 内部缓冲） */
    UI_EVT_BIZ,         /**< 业务状态变化（电量/WiFi/SD/录制状态等） */
    UI_EVT_QUIT,        /**< 请求 UI 线程退出 */
} ui_evt_type_t;

/** @brief 逻辑按键码。来源无关，按键与 APP 指令统一映射到此集合。 */
typedef enum {
    UI_KEY_NONE = 0,
    UI_KEY_UP,      /**< 上：焦点上移 / 数值增 */
    UI_KEY_DOWN,    /**< 下：焦点下移 / 数值减 */
    UI_KEY_OK,      /**< 确认：进入 / 触发 / 快门 */
    UI_KEY_BACK,    /**< 返回：退出当前页 / 取消 */
} ui_key_code_t;

/** @brief 按键动作（预留长按/双击扩展，样机先用按下/抬起）。 */
typedef enum {
    UI_KEY_ACT_RELEASE = 0, /**< 抬起 */
    UI_KEY_ACT_PRESS,       /**< 按下 */
} ui_key_act_t;

/** @brief 业务状态子类型（供状态栏/页面刷新）。 */
typedef enum {
    UI_BIZ_BATTERY = 0, /**< value: 电量百分比 0~100；aux: 是否充电 0/1 */
    UI_BIZ_WIFI,        /**< value: 信号 0~100；aux: 0 断开/1 连接 */
    UI_BIZ_SDCARD,      /**< value: 剩余可录分钟；aux: 0 无卡/1 正常/2 异常 */
    UI_BIZ_REC_STATE,   /**< value: 0 停止/1 录制中；aux: 已录秒数 */
    UI_BIZ_MODE,        /**< value: ui_work_mode_t */
    UI_BIZ_AUDIO,       /**< value: 0 关/1 开 */
    UI_BIZ_EIS,         /**< value: 0 关/1 开（防抖） */
    UI_BIZ_TIME,        /**< value: Unix 时间戳(秒) */
} ui_biz_type_t;

/** @brief 工作模式（严格对齐拇指相机业务）。 */
typedef enum {
    UI_MODE_RECORD = 0, /**< 录像模式（默认） */
    UI_MODE_PHOTO,      /**< 拍照模式 */
    UI_MODE_TIMELAPSE,  /**< 延时摄影模式 */
    UI_MODE_PLAYBACK,   /**< 回放模式 */
    UI_MODE_SETTING,    /**< 设置模式 */
    UI_MODE_MAX,
} ui_work_mode_t;

/** @brief 按键事件负载。 */
typedef struct {
    ui_key_code_t code;
    ui_key_act_t  act;
} ui_evt_key_t;

/** @brief 模拟点击事件负载（屏幕坐标，原点左上，单位像素）。 */
typedef struct {
    int16_t x;
    int16_t y;
    uint8_t pressed; /**< 1 按下 / 0 抬起 */
} ui_evt_point_t;

/** @brief 视频帧就绪负载。帧数据由 ui_video 内部双缓冲持有，这里只做"就绪通知"。 */
typedef struct {
    uint16_t width;
    uint16_t height;
    uint32_t seq;    /**< 帧序号，用于丢帧统计 */
} ui_evt_video_t;

/** @brief 业务状态负载。 */
typedef struct {
    ui_biz_type_t type;
    int32_t       value;
    int32_t       aux;
} ui_evt_biz_t;

/** @brief 统一事件对象（放入消息队列按值拷贝，禁止携带需跨线程生命周期的裸指针）。 */
typedef struct {
    ui_evt_type_t type;
    union {
        ui_evt_key_t   key;
        ui_evt_point_t point;
        ui_evt_video_t video;
        ui_evt_biz_t   biz;
    } u;
} ui_event_t;

#ifdef __cplusplus
}
#endif

#endif /* __UI_EVENT_H__ */
