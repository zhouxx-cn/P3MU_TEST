/**
 * @file    ui_port.h
 * @brief   LVGL 移植层：显示 flush、tick、输入读取回调的对接点。
 *
 * @details
 *  这是 UI 与"具体硬件/平台"之间唯一的耦合点。样机阶段 flush 走 LCD 层的
 *  REFRESH_CALLBACK(x,y,w,h,rgb565,size)；迁移真机只改本文件的回调实现，
 *  上层 UI 完全不动。
 *
 *  - tick：由内部创建的高精度定时线程周期调用 lv_tick_inc()，
 *    对应 lv_conf.h 中 LV_TICK_CUSTOM=0。
 *  - flush：LVGL 渲染好的 RGB565 区域 → lcd 刷新回调。
 */
#ifndef __UI_PORT_H__
#define __UI_PORT_H__

#include "ui_common.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief LCD 刷新回调原型（与 DVR_SDK/LCD/lcd_layer.h 的 REFRESH_CALLBACK 一致）。
 * @param x,y   区域左上角
 * @param w,h   区域宽高
 * @param data  RGB565 像素，按行连续，每像素 2 字节
 * @param size  data 字节数（= w*h*2）
 * @return 0 成功
 */
typedef int (*ui_lcd_flush_fn)(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                               uint8_t *data, unsigned int size);

/**
 * @brief 初始化显示移植层：注册 LVGL display driver + 启动 tick 线程。
 * @param flush_fn  LCD 刷新回调（传 NULL 时内部用弱实现，仅打印，便于裸跑调试）
 * @return UI_OK / UI_ERR
 * @note  必须在 lv_init() 之后、创建任何页面之前调用。仅 UI 线程调用。
 */
int ui_port_disp_init(ui_lcd_flush_fn flush_fn);

/**
 * @brief 反初始化：停止 tick 线程，释放显示缓冲。
 */
void ui_port_disp_deinit(void);

/**
 * @brief 获取当前毫秒时标（供非 LVGL 代码使用，基于 CLOCK_MONOTONIC）。
 */
uint32_t ui_port_tick_get(void);

/**
 * @brief 让出 CPU 指定毫秒（UI 主循环空闲时调用）。
 */
void ui_port_msleep(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif /* __UI_PORT_H__ */
