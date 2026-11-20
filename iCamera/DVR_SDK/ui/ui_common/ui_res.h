/**
 * @file    ui_res.h
 * @brief   拇指相机 UI 资源集中管理：配色、字体、图标符号。
 * @details 样机采用 LVGL 内置矢量符号字体做图标，轻量化、无需外部位图资源，
 *          适配 240x320 低分辨率屏。迁移真机时如需替换为品牌图标，只改本模块。
 */
#ifndef __UI_RES_H__
#define __UI_RES_H__

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== 主题配色（消费类便携相机：深色沉稳 + 高亮橙） ==================== */
#define UI_COLOR_BG          lv_color_hex(0x101216)   /**< 页面底色（半透明叠加视频用） */
#define UI_COLOR_PANEL       lv_color_hex(0x1C1F26)   /**< 面板/卡片 */
#define UI_COLOR_PANEL_SEL   lv_color_hex(0x2A2F3A)   /**< 面板选中态 */
#define UI_COLOR_ACCENT      lv_color_hex(0xFF8A00)   /**< 主高亮（橙）：焦点/选中 */
#define UI_COLOR_REC         lv_color_hex(0xFF3B30)   /**< 录制红 */
#define UI_COLOR_TEXT        lv_color_hex(0xF2F2F2)   /**< 主文字 */
#define UI_COLOR_TEXT_DIM    lv_color_hex(0x9AA0AA)   /**< 次级文字 */
#define UI_COLOR_OK          lv_color_hex(0x34C759)   /**< 正常/成功 */
#define UI_COLOR_WARN        lv_color_hex(0xFFCC00)   /**< 警告 */
#define UI_COLOR_MASK        lv_color_hex(0x000000)   /**< 遮罩 */

/* ==================== 字体（思源黑体裁剪字库，见 ui_font/ui_font.h） ==================== */
/* montserrat 12/14/16/20 仍编译，作为 LVGL symbol 图标的回退字体 */
#include "ui_font.h"
#define UI_FONT_SMALL        (&ui_font_sc_12)
#define UI_FONT_NORMAL       (&ui_font_sc_14)
#define UI_FONT_MEDIUM       (&ui_font_sc_16)
#define UI_FONT_LARGE        (&ui_font_sc_20)

/* ==================== 图标符号（LVGL 内置，轻量） ==================== */
#define UI_ICO_REC           LV_SYMBOL_VIDEO
#define UI_ICO_PHOTO         LV_SYMBOL_IMAGE
#define UI_ICO_TIMELAPSE     LV_SYMBOL_LOOP
#define UI_ICO_PLAYBACK      LV_SYMBOL_DIRECTORY
#define UI_ICO_SETTING       LV_SYMBOL_SETTINGS
#define UI_ICO_WIFI          LV_SYMBOL_WIFI
#define UI_ICO_BATTERY_FULL  LV_SYMBOL_BATTERY_FULL
#define UI_ICO_BATTERY_3     LV_SYMBOL_BATTERY_3
#define UI_ICO_BATTERY_2     LV_SYMBOL_BATTERY_2
#define UI_ICO_BATTERY_1     LV_SYMBOL_BATTERY_1
#define UI_ICO_BATTERY_EMPTY LV_SYMBOL_BATTERY_EMPTY
#define UI_ICO_SD            LV_SYMBOL_SD_CARD
#define UI_ICO_AUDIO_ON      LV_SYMBOL_AUDIO
#define UI_ICO_AUDIO_OFF     LV_SYMBOL_MUTE
#define UI_ICO_OK            LV_SYMBOL_OK
#define UI_ICO_CLOSE         LV_SYMBOL_CLOSE
#define UI_ICO_LEFT          LV_SYMBOL_LEFT
#define UI_ICO_RIGHT         LV_SYMBOL_RIGHT
#define UI_ICO_UP            LV_SYMBOL_UP
#define UI_ICO_DOWN          LV_SYMBOL_DOWN
#define UI_ICO_PLAY          LV_SYMBOL_PLAY
#define UI_ICO_TRASH         LV_SYMBOL_TRASH
#define UI_ICO_CHARGE        LV_SYMBOL_CHARGE
#define UI_ICO_WARNING       LV_SYMBOL_WARNING
#define UI_ICO_EIS           LV_SYMBOL_GPS   /* 防抖占位图标（真机换品牌防抖图标） */

/**
 * @brief 根据电量百分比返回合适的电池符号。
 * @param percent 0~100
 * @return 对应电池图标符号字符串
 */
const char *ui_res_battery_symbol(int percent);

#ifdef __cplusplus
}
#endif

#endif /* __UI_RES_H__ */
