/**
 * @file    ui_port.h
 * @brief   LVGL 移植层：显示 flush、tick、输入读取回调的对接点。
 *
 * @details
 *  这是 UI 与"具体硬件/平台"之间唯一的耦合点。显示采用【整帧送显】模式：
 *  LVGL 把"视频底层 + 菜单浮层"合成成一整屏 240x320 RGB565，经 flush 回调
 *  一次性送屏——匹配 IPC 模拟机底层接口 Lcd_SendFrame(pData, size)
 *  （只接受一整张 RGB565）。迁移真机只改本文件的回调实现，上层 UI 完全不动。
 *
 *  - tick：由内部创建的定时线程周期调用 lv_tick_inc()，对应 LV_TICK_CUSTOM=0。
 *  - flush：LVGL 渲染好的整帧 RGB565 → Lcd_SendFrame。
 */
#ifndef __UI_PORT_H__
#define __UI_PORT_H__

#include "ui_common.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief LCD 整帧送显回调原型（与 IPC 模拟机的 Lcd_SendFrame 一致）。
 * @param pData 一整屏 RGB565 像素（240x320，按行连续，小端，每像素 2 字节）
 * @param size  字节数（= 240*320*2 = 153600）
 * @return 0 成功
 * @note  真机把它接到 Lcd_SendFrame(UINT8 *pData, UINT32 Size)。
 *        若 Lcd_SendFrame 返回 void，用一个一行适配函数包一下即可，例如：
 *          static int lcd_send(uint8_t *p, uint32_t n){ Lcd_SendFrame(p, n); return 0; }
 */
typedef int (*ui_lcd_flush_fn)(uint8_t *pData, uint32_t size);

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
