/**
 * @file    ui_common.h
 * @brief   拇指相机 UI 公共定义：屏幕参数、通用宏、日志、返回码。
 * @note    纯 C。所有 UI 模块统一包含本头。LVGL 相关配置见 lv_conf.h。
 */
#ifndef __UI_COMMON_H__
#define __UI_COMMON_H__

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include "ui_event.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== 屏幕参数（240x320 竖屏） ==================== */
#define UI_SCREEN_W            240
#define UI_SCREEN_H            320

/* 子码流预览分辨率（IPC 送入 UI 层） */
#define UI_PREVIEW_W           240
#define UI_PREVIEW_H           320

/* 状态栏高度 */
#define UI_STATUSBAR_TOP_H     22      /**< 顶部状态栏 */
#define UI_STATUSBAR_BOT_H     24      /**< 底部信息栏 */

/* ==================== 通用返回码 ==================== */
#define UI_OK                  (0)
#define UI_ERR                 (-1)
#define UI_ERR_PARAM           (-2)
#define UI_ERR_NOMEM           (-3)
#define UI_ERR_STATE           (-4)

/* ==================== 通用工具宏 ==================== */
#define UI_ARRAY_SIZE(a)       (sizeof(a) / sizeof((a)[0]))
#define UI_MIN(a, b)           ((a) < (b) ? (a) : (b))
#define UI_MAX(a, b)           ((a) > (b) ? (a) : (b))
#define UI_CLAMP(v, lo, hi)    UI_MAX((lo), UI_MIN((v), (hi)))
#define UI_UNUSED(x)           ((void)(x))

/* ==================== 日志（迁移时只需改这里对接平台日志） ==================== */
#ifndef UI_LOG_TAG
#define UI_LOG_TAG "UI"
#endif

#include <stdio.h>
#define UI_LOGE(fmt, ...) fprintf(stderr, "[%s][E] " fmt "\n", UI_LOG_TAG, ##__VA_ARGS__)
#define UI_LOGW(fmt, ...) fprintf(stderr, "[%s][W] " fmt "\n", UI_LOG_TAG, ##__VA_ARGS__)
#define UI_LOGI(fmt, ...) fprintf(stdout, "[%s][I] " fmt "\n", UI_LOG_TAG, ##__VA_ARGS__)
#ifdef UI_DEBUG
#define UI_LOGD(fmt, ...) fprintf(stdout, "[%s][D] " fmt "\n", UI_LOG_TAG, ##__VA_ARGS__)
#else
#define UI_LOGD(fmt, ...) do {} while (0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __UI_COMMON_H__ */
