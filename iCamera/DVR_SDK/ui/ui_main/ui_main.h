/**
 * @file    ui_main.h
 * @brief   拇指相机 UI 系统入口：初始化、启动/停止 UI 线程。
 *
 * @details
 *  对外暴露极简接口，供主程序（my_gui 或设备主控）在开机时调用：
 *    1. ui_main_init()  —— 初始化 LVGL/显示/输入/页面/状态栏（不启动线程）
 *    2. ui_main_start() —— 创建并运行独立 UI 线程
 *    3. 其它线程通过 ui_input_* / ui_video_refresh / ui_biz_* 投递事件
 *    4. ui_main_stop()  —— 退出 UI 线程并清理
 *
 *  线程模型：LVGL 全部操作只发生在本模块创建的 UI 线程里。
 */
#ifndef __UI_MAIN_H__
#define __UI_MAIN_H__

#include "ui_common.h"
#include "ui_port.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief UI 初始化配置。 */
typedef struct {
    ui_lcd_flush_fn lcd_flush;   /**< LCD RGB565 刷新回调（对接 DVR_SDK/LCD） */
} ui_main_cfg_t;

/**
 * @brief 初始化 UI 系统（同步，调用线程内完成 LVGL 初始化）。
 * @param cfg 配置（lcd_flush 可为 NULL 用于裸跑调试）
 * @return UI_OK / UI_ERR
 * @note  内部会 lv_init、建显示、建分层、建页面与状态栏。
 */
int ui_main_init(const ui_main_cfg_t *cfg);

/**
 * @brief 启动 UI 线程（异步）。
 * @return UI_OK / UI_ERR
 */
int ui_main_start(void);

/**
 * @brief 停止 UI 线程并反初始化。
 */
void ui_main_stop(void);

/**
 * @brief 获取三层画布中的"视频底层"对象（供 ui_video 挂画布）。
 * @return 视频底层对象，未初始化返回 NULL
 * @note  仅 UI 线程调用。
 */
lv_obj_t *ui_main_video_layer(void);

/**
 * @brief 获取"状态/弹窗顶层"对象（供 ui_statusbar / ui_popup 挂载）。
 * @return 顶层对象
 */
lv_obj_t *ui_main_top_layer(void);

#ifdef __cplusplus
}
#endif

#endif /* __UI_MAIN_H__ */
