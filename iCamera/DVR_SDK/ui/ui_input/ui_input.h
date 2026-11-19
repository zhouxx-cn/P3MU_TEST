/**
 * @file    ui_input.h
 * @brief   输入抽象层：屏蔽输入来源（物理按键 / APP 模拟点击），统一喂给 LVGL。
 *
 * @details
 *  上层页面不区分来源，只通过 LVGL 的焦点组(keypad) 或点击(pointer) 收事件。
 *  本层注册两个 LVGL indev：
 *    - keypad indev  : 上/下/确认/返回 → LV_KEY_*，驱动焦点组导航
 *    - pointer indev : APP 下发坐标 → 模拟触摸点击
 *  两个"源"（ui_input_key / ui_input_app）在各自线程里采集，经 ui_msgq 投递，
 *  UI 线程 ui_input_feed() 把事件缓存，LVGL read_cb 再取用。
 *
 *  迁移真机：换掉 ui_input_key 的底层扫描即可，抽象层与页面零改动。
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

/* ============ 源 2：APP JSON 网络指令 ============ */

/**
 * @brief 解析一条 APP 下发的 JSON 指令并转成统一事件投递。
 * @param json  JSON 字符串，形如：
 *              {"type":"key","code":"ok"}               // 模拟按键
 *              {"type":"click","x":120,"y":200}          // 模拟点击坐标
 *              {"type":"press","x":120,"y":200}          // 按下
 *              {"type":"release","x":120,"y":200}        // 抬起
 * @return UI_OK 解析成功并已投递；UI_ERR 格式错误
 * @note  由 APP 网络接收线程调用（线程安全，内部走 ui_msgq）。
 */
int ui_input_app_on_json(const char *json);

#ifdef __cplusplus
}
#endif

#endif /* __UI_INPUT_H__ */
