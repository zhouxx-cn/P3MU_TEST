/**
 * @file    ui_theme.h
 * @brief   拇指相机 UI 公共样式：集中管理 LVGL style，统一观感。
 * @details 小巧简洁、消费相机风格；所有页面复用，保证视觉一致，降低内存占用。
 */
#ifndef __UI_THEME_H__
#define __UI_THEME_H__

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化全局样式对象（进程内只需调用一次，由 ui_main 初始化时调用）。
 */
void ui_theme_init(void);

/** @brief 透明容器样式（用于叠在视频之上的浮层根容器）。 */
const lv_style_t *ui_theme_transparent(void);

/** @brief 半透明面板样式（菜单/设置列表背景）。 */
const lv_style_t *ui_theme_panel(void);

/** @brief 列表项普通态样式。 */
const lv_style_t *ui_theme_item(void);

/** @brief 列表项焦点/选中态样式（高亮橙边框）。 */
const lv_style_t *ui_theme_item_focused(void);

/** @brief 主文字样式。 */
const lv_style_t *ui_theme_text(void);

/** @brief 次级文字样式。 */
const lv_style_t *ui_theme_text_dim(void);

/**
 * @brief 将"焦点高亮"样式应用到一个对象，使其在 LV_STATE_FOCUSED 时高亮。
 * @param obj 目标对象
 */
void ui_theme_apply_focusable(lv_obj_t *obj);

#ifdef __cplusplus
}
#endif

#endif /* __UI_THEME_H__ */
