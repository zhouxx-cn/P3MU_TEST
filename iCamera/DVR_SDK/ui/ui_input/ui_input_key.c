/**
 * @file    ui_input_key.c
 * @brief   输入源 1：物理按键扫描（上/下/确认/返回）。
 *
 * @details
 *  样机阶段：具体的按键读取 ui_key_scan_hw() 以 __attribute__((weak)) 弱实现，
 *  默认返回"无按键"。真机移植时在板级代码里强实现该函数（GPIO/ADC/矩阵键盘），
 *  本文件的扫描线程、消抖、事件投递逻辑完全复用，无需改动。
 *
 *  扫描线程 → 消抖 → 组装 ui_event_t(UI_EVT_KEY) → ui_msgq_post → UI 线程。
 */
#include "ui_input.h"
#include "ui_msgq.h"
#include "ui_port.h"
#include <pthread.h>

#define KEY_SCAN_INTERVAL_MS   20
#define KEY_DEBOUNCE_COUNT     2    /* 连续 2 次相同才确认，抗抖 */

static pthread_t     s_scan_thread;
static volatile int  s_scan_run = 0;

/**
 * @brief 板级按键读取钩子（弱实现）。
 * @return 当前被按下的逻辑键码，无则返回 UI_KEY_NONE。
 * @note   真机请强实现：读 GPIO/ADC，返回对应 UI_KEY_*。
 *         样机如需本地调试，可改为读取 stdin 或通过 ui_input_key_inject 注入。
 */
__attribute__((weak)) ui_key_code_t ui_key_scan_hw(void)
{
    return UI_KEY_NONE;
}

/* 组装并投递按键事件 */
static void post_key(ui_key_code_t code, ui_key_act_t act)
{
    ui_event_t evt;
    evt.type = UI_EVT_KEY;
    evt.u.key.code = code;
    evt.u.key.act  = act;
    ui_msgq_post(&evt);
}

void ui_input_key_inject(ui_key_code_t code, ui_key_act_t act)
{
    post_key(code, act);
}

/* 扫描线程：消抖 + 边沿检测（按下/抬起各发一次） */
static void *key_scan_entry(void *arg)
{
    ui_key_code_t last_stable = UI_KEY_NONE;
    ui_key_code_t last_raw = UI_KEY_NONE;
    int same_cnt = 0;
    UI_UNUSED(arg);

    while (s_scan_run) {
        ui_key_code_t raw = ui_key_scan_hw();

        if (raw == last_raw) {
            if (same_cnt < KEY_DEBOUNCE_COUNT) {
                same_cnt++;
            }
        } else {
            same_cnt = 0;
            last_raw = raw;
        }

        /* 达到消抖阈值，认为状态稳定 */
        if (same_cnt >= KEY_DEBOUNCE_COUNT && raw != last_stable) {
            if (last_stable != UI_KEY_NONE) {
                post_key(last_stable, UI_KEY_ACT_RELEASE);  /* 上一个键抬起 */
            }
            if (raw != UI_KEY_NONE) {
                post_key(raw, UI_KEY_ACT_PRESS);            /* 新键按下 */
            }
            last_stable = raw;
        }

        ui_port_msleep(KEY_SCAN_INTERVAL_MS);
    }
    return NULL;
}

int ui_input_key_start(void)
{
    if (s_scan_run) {
        return UI_OK;
    }
    s_scan_run = 1;
    if (pthread_create(&s_scan_thread, NULL, key_scan_entry, NULL) != 0) {
        s_scan_run = 0;
        UI_LOGE("create key scan thread failed");
        return UI_ERR;
    }
    UI_LOGI("ui_input_key start");
    return UI_OK;
}

void ui_input_key_stop(void)
{
    if (s_scan_run) {
        s_scan_run = 0;
        pthread_join(s_scan_thread, NULL);
    }
}
