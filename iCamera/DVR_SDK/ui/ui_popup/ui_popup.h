/**
 * @file    ui_popup.h
 * @brief   弹窗层：toast 提示、确认框、进度/保存中提示。
 *
 * @details
 *  弹窗挂在顶层(lv_layer_top)，叠在所有页面之上。toast 自动定时消失；
 *  确认框通过回调返回用户选择。所有接口仅 UI 线程调用。
 */
#ifndef __UI_POPUP_H__
#define __UI_POPUP_H__

#include "ui_common.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief 确认框结果回调。 @param confirmed 1 确认 0 取消；@param user 透传数据。 */
typedef void (*ui_confirm_cb_t)(int confirmed, void *user);

/**
 * @brief 初始化弹窗层。
 * @param top_layer 顶层父对象（ui_main 的 lv_layer_top）
 * @return UI_OK / UI_ERR
 */
int ui_popup_init(lv_obj_t *top_layer);

/**
 * @brief 轻提示 toast（底部浮现，自动消失）。
 * @param text   文本
 * @param ms     持续毫秒
 */
void ui_popup_toast(const char *text, uint32_t ms);

/**
 * @brief 弹出确认框（是/否）。
 * @param title 标题/内容
 * @param cb    结果回调
 * @param user  透传数据
 */
void ui_popup_confirm(const char *title, ui_confirm_cb_t cb, void *user);

/**
 * @brief 显示"处理中"遮罩（如格式化/保存），需配对 ui_popup_busy_hide。
 * @param text 文本
 */
void ui_popup_busy_show(const char *text);

/** @brief 隐藏"处理中"遮罩。 */
void ui_popup_busy_hide(void);

#ifdef __cplusplus
}
#endif

#endif /* __UI_POPUP_H__ */
