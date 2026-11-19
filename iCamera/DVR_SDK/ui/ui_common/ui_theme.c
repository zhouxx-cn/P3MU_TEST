/**
 * @file    ui_theme.c
 * @brief   ui_theme 样式实现。
 */
#include "ui_theme.h"
#include "ui_res.h"

static lv_style_t s_transparent;
static lv_style_t s_panel;
static lv_style_t s_item;
static lv_style_t s_item_focused;
static lv_style_t s_text;
static lv_style_t s_text_dim;
static int        s_inited = 0;

void ui_theme_init(void)
{
    if (s_inited) {
        return;
    }
    s_inited = 1;

    /* 透明浮层容器：无背景、无边框、无内边距，完全露出底层视频 */
    lv_style_init(&s_transparent);
    lv_style_set_bg_opa(&s_transparent, LV_OPA_TRANSP);
    lv_style_set_border_width(&s_transparent, 0);
    lv_style_set_outline_width(&s_transparent, 0);
    lv_style_set_pad_all(&s_transparent, 0);
    lv_style_set_radius(&s_transparent, 0);

    /* 半透明面板：菜单/设置底衬，保证文字在视频上可读 */
    lv_style_init(&s_panel);
    lv_style_set_bg_color(&s_panel, UI_COLOR_PANEL);
    lv_style_set_bg_opa(&s_panel, LV_OPA_80);
    lv_style_set_border_width(&s_panel, 0);
    lv_style_set_radius(&s_panel, 6);
    lv_style_set_pad_all(&s_panel, 6);

    /* 列表项普通态 */
    lv_style_init(&s_item);
    lv_style_set_bg_color(&s_item, UI_COLOR_PANEL);
    lv_style_set_bg_opa(&s_item, LV_OPA_0);
    lv_style_set_text_color(&s_item, UI_COLOR_TEXT);
    lv_style_set_text_font(&s_item, UI_FONT_NORMAL);
    lv_style_set_border_width(&s_item, 0);
    lv_style_set_radius(&s_item, 4);
    lv_style_set_pad_hor(&s_item, 8);
    lv_style_set_pad_ver(&s_item, 6);

    /* 列表项焦点态：高亮橙背景 + 左侧强调 */
    lv_style_init(&s_item_focused);
    lv_style_set_bg_color(&s_item_focused, UI_COLOR_ACCENT);
    lv_style_set_bg_opa(&s_item_focused, LV_OPA_COVER);
    lv_style_set_text_color(&s_item_focused, lv_color_black());
    lv_style_set_radius(&s_item_focused, 4);

    /* 主文字 */
    lv_style_init(&s_text);
    lv_style_set_text_color(&s_text, UI_COLOR_TEXT);
    lv_style_set_text_font(&s_text, UI_FONT_NORMAL);

    /* 次级文字 */
    lv_style_init(&s_text_dim);
    lv_style_set_text_color(&s_text_dim, UI_COLOR_TEXT_DIM);
    lv_style_set_text_font(&s_text_dim, UI_FONT_SMALL);
}

const lv_style_t *ui_theme_transparent(void) { return &s_transparent; }
const lv_style_t *ui_theme_panel(void)       { return &s_panel; }
const lv_style_t *ui_theme_item(void)        { return &s_item; }
const lv_style_t *ui_theme_item_focused(void){ return &s_item_focused; }
const lv_style_t *ui_theme_text(void)        { return &s_text; }
const lv_style_t *ui_theme_text_dim(void)    { return &s_text_dim; }

void ui_theme_apply_focusable(lv_obj_t *obj)
{
    if (!obj) {
        return;
    }
    lv_obj_add_style(obj, &s_item, 0);
    lv_obj_add_style(obj, &s_item_focused, LV_STATE_FOCUSED);
}
