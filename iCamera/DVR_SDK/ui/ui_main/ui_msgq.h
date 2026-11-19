/**
 * @file    ui_msgq.h
 * @brief   拇指相机 UI 线程安全消息队列（跨线程 → UI 线程的唯一通道）。
 *
 * @details
 *  其它线程（按键、APP 网络、IPC 子码流、业务上报）只能通过 ui_msgq_post()
 *  把 ui_event_t 投递给 UI 线程；UI 线程在主循环里 ui_msgq_poll() 取出后，
 *  才去操作 LVGL 对象。以此严格杜绝跨线程直接访问 LVGL。
 *
 *  实现：固定容量环形缓冲 + pthread_mutex + pthread_cond，按值拷贝事件。
 */
#ifndef __UI_MSGQ_H__
#define __UI_MSGQ_H__

#include "ui_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief 队列容量（样机够用；满时丢弃最旧的视频帧事件以保证实时性）。 */
#define UI_MSGQ_CAPACITY   64

/**
 * @brief 初始化消息队列。
 * @return UI_OK / UI_ERR
 */
int ui_msgq_init(void);

/**
 * @brief 销毁消息队列。
 */
void ui_msgq_deinit(void);

/**
 * @brief 投递一个事件（线程安全，非阻塞）。
 * @param evt 事件（按值拷贝入队）
 * @return UI_OK；队列满返回 UI_ERR（视频帧事件会被自动去重/覆盖，见实现）
 * @note  可在任意非 UI 线程调用。
 */
int ui_msgq_post(const ui_event_t *evt);

/**
 * @brief 取出一个事件（仅 UI 线程调用）。
 * @param evt       [out] 取出的事件
 * @param timeout_ms 阻塞超时毫秒；0 表示非阻塞；<0 表示无限等待
 * @return UI_OK 取到；UI_ERR 超时或空
 */
int ui_msgq_poll(ui_event_t *evt, int timeout_ms);

#ifdef __cplusplus
}
#endif

#endif /* __UI_MSGQ_H__ */
