/**
 * @file    ui_font.h
 * @brief   思源黑体裁剪字库声明（由 tools/font_gen.py 生成对应 .c）。
 *
 * @details
 *  字库覆盖 ui_lang.c 全部语言字符 + ASCII；LVGL symbol（LV_SYMBOL_*）
 *  不在其中，各字体经 .fallback 回退到同字号 lv_font_montserrat_* 渲染，
 *  因此 lv_conf.h 中 LV_FONT_MONTSERRAT_12/14/16/20 必须保持开启。
 *
 *  修改文案或新增语言后，重新运行：
 *      python3 tools/font_gen.py <SourceHanSansSC-Regular.ttf>
 */
#ifndef __UI_FONT_H__
#define __UI_FONT_H__

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/** 12px 思源黑体（状态栏/提示，对应原 UI_FONT_SMALL） */
extern const lv_font_t ui_font_sc_12;
/** 14px 思源黑体（正文/toast，对应原 UI_FONT_NORMAL） */
extern const lv_font_t ui_font_sc_14;
/** 16px 思源黑体（标题，对应原 UI_FONT_MEDIUM） */
extern const lv_font_t ui_font_sc_16;
/** 20px 思源黑体（大字/中心提示，对应原 UI_FONT_LARGE） */
extern const lv_font_t ui_font_sc_20;

#ifdef __cplusplus
}
#endif

#endif /* __UI_FONT_H__ */
