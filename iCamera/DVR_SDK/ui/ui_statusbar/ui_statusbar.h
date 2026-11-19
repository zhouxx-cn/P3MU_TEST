/**
 * @file    ui_statusbar.h
 * @brief   拇指相机状态栏：顶部（录制/时间/WiFi/电量）+ 底部（模式/容量/音频/防抖）。
 *
 * @details
 *  状态栏挂在最顶层(lv_layer_top)，始终叠加在视频与页面之上。
 *  通过订阅 UI_EVT_BIZ 事件自动刷新图标，页面无需关心状态栏细节。
 */
#ifndef __UI_STATUSBAR_H__
#define __UI_STATUSBAR_H__

#include "ui_common.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化状态栏（创建顶/底两条栏）。
 * @param top_layer 顶层父对象（ui_main 的 lv_layer_top）
 * @return UI_OK / UI_ERR
 * @note  仅 UI 线程调用。
 */
int ui_statusbar_init(lv_obj_t *top_layer);

/**
 * @brief 处理业务状态事件，刷新对应图标。
 * @param biz 业务事件负载
 * @note  仅 UI 线程调用（由 ui_main 分发）。
 */
void ui_statusbar_on_biz(const ui_evt_biz_t *biz);

/**
 * @brief 显示/隐藏整个状态栏（某些全屏页面可隐藏）。
 * @param show 1 显示 0 隐藏
 */
void ui_statusbar_set_visible(int show);

/**
 * @brief 设置录制红点闪烁开关（录像中由预览页驱动）。
 * @param blinking 1 开始闪烁 0 停止
 */
void ui_statusbar_set_rec_blink(int blinking);

#ifdef __cplusplus
}
#endif

#endif /* __UI_STATUSBAR_H__ */
