/**
 * @file    ui_input.h
 * @brief   输入抽象层：屏蔽输入来源（物理按键 / 触摸屏），统一喂给 LVGL。
 *
 * @details
 *  上层页面不区分来源，只通过 LVGL 的焦点组(keypad) 或点击(pointer) 收事件。
 *  本层注册两个 LVGL indev：
 *    - keypad indev  : 上/下/确认 → LV_KEY_*，驱动焦点组导航
 *    - pointer indev : 触摸坐标 → 点击/滑动（当前样机无触摸屏，注册后休眠，
 *                      为真机触摸屏预留，接入点见 ui_input_feed()）
 *
 *  输入源在各自线程里采集，经 ui_msgq 投递（UI_EVT_KEY / UI_EVT_POINT），
 *  UI 线程 ui_input_feed() 把事件缓存，LVGL read_cb 再取用。
 *
 *  迁移真机：按键换掉 ui_input_key 的底层扫描、触摸屏驱动把坐标经
 *  ui_msgq 投递 UI_EVT_POINT 即可，抽象层与页面零改动。
 */
#ifndef __UI_INPUT_H__
#define __UI_INPUT_H__

#include "ui_common.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化输入抽象层：注册 keypad + pointer 两个 indev。
 * @return UI_OK / UI_ERR
 * @note  仅 UI 线程调用（在 ui_main_init 内）。
 */
int ui_input_init(void);

/**
 * @brief 反初始化。
 */
void ui_input_deinit(void);

/**
 * @brief 把当前页面的焦点组绑定到 keypad indev（切页时调用）。
 * @param group 页面焦点组
 */
void ui_input_set_group(lv_group_t *group);

/** @brief 取当前 keypad indev 绑定的焦点组（如弹窗关闭后恢复页面焦点用）。 */
lv_group_t *ui_input_get_group(void);

/**
 * @brief UI 线程收到 KEY/POINT 事件后调用，缓存到 indev 读取点。
 * @param evt 事件（仅处理 UI_EVT_KEY / UI_EVT_POINT，其它忽略）
 * @note  仅 UI 线程调用（由 ui_main 主循环驱动）。
 */
void ui_input_feed(const ui_event_t *evt);

/* ============ 源 1：物理按键 ============ */

/**
 * @brief 启动物理按键扫描源（创建按键扫描线程）。
 * @return UI_OK / UI_ERR
 * @note  可在任意时机（通常主程序启动时）调用；内部通过 ui_msgq 投递按键事件。
 *        样机的具体 GPIO/ADC 扫描在 .c 里以弱函数预留，真机实现替换即可。
 */
int ui_input_key_start(void);

/** @brief 停止按键扫描源。 */
void ui_input_key_stop(void);

/**
 * @brief 直接注入一个逻辑按键（供测试或其它按键驱动复用）。
 * @param code 逻辑按键码
 * @param act  按下/抬起
 */
void ui_input_key_inject(ui_key_code_t code, ui_key_act_t act);

/* ============ 源 2：触摸屏（真机预留，当前样机无触摸屏） ============ */
/*
 * 接入点：触摸驱动线程把坐标经 ui_msgq 投递 UI_EVT_POINT 事件
 *（x/y/pressed），ui_input_feed() 会自动喂给 pointer indev，
 * LVGL 即识别为点击/长按/拖动，页面层零改动。
 * 样机阶段无触摸硬件，pointer indev 注册后保持休眠（坐标 0,0 / 抬起态）。
 */

#ifdef __cplusplus
}
#endif

#endif /* __UI_INPUT_H__ */
