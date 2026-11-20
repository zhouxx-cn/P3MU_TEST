/**
 * @file    ui_popup.c
 * @brief   弹窗层实现：toast / 确认框 / 处理中遮罩。
 */
#include "ui_popup.h"
#include "ui_res.h"
#include "ui_theme.h"
#include "ui_input.h"
#include "ui_lang.h"
#include "lvgl.h"

static lv_obj_t     *s_layer = NULL;     /* 顶层父对象 */
static lv_obj_t     *s_toast = NULL;
static lv_timer_t   *s_toast_timer = NULL;
static lv_obj_t     *s_busy = NULL;

/* 确认框上下文 */
static lv_obj_t        *s_confirm = NULL;
static ui_confirm_cb_t  s_confirm_cb = NULL;
static void            *s_confirm_user = NULL;
static lv_group_t      *s_confirm_group = NULL;
static lv_group_t      *s_prev_group = NULL; /* 保存进入确认框前的焦点组，关闭后恢复 */

int ui_popup_init(lv_obj_t *top_layer)
{
    if (!top_layer) {
        return UI_ERR_PARAM;
    }
    s_layer = top_layer;
    return UI_OK;
}

/* ---------------- toast ---------------- */
static void toast_timer_cb(lv_timer_t *t)
{
    UI_UNUSED(t);
    if (s_toast) {
        lv_obj_del(s_toast);
        s_toast = NULL;
    }
    s_toast_timer = NULL;
}

void ui_popup_toast(const char *text, uint32_t ms)
{
    if (!s_layer || !text) {
        UI_LOGI("toast: %s", text ? text : "");
        return;
    }
    /* 复用单个 toast 对象 */
    if (s_toast) {
        lv_obj_del(s_toast);
        s_toast = NULL;
    }
    if (s_toast_timer) {
        lv_timer_del(s_toast_timer);
        s_toast_timer = NULL;
    }

    s_toast = lv_obj_create(s_layer);
    lv_obj_remove_style_all(s_toast);
    lv_obj_set_style_bg_color(s_toast, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(s_toast, LV_OPA_70, 0);
    lv_obj_set_style_radius(s_toast, 6, 0);
    lv_obj_set_style_pad_all(s_toast, 8, 0);
    lv_obj_set_size(s_toast, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_max_width(s_toast, UI_SCREEN_W - 40, 0);

    lv_obj_t *lb = lv_label_create(s_toast);
    lv_label_set_text(lb, text);
    lv_obj_set_style_text_color(lb, UI_COLOR_TEXT, 0);
    lv_obj_set_style_text_font(lb, UI_FONT_NORMAL, 0);
    lv_label_set_long_mode(lb, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(lb, LV_SIZE_CONTENT);

    lv_obj_align(s_toast, LV_ALIGN_BOTTOM_MID, 0, -(UI_STATUSBAR_BOT_H + 20));
    s_toast_timer = lv_timer_create(toast_timer_cb, ms ? ms : 1200, NULL);
    lv_timer_set_repeat_count(s_toast_timer, 1);
}

/* ---------------- 确认框 ---------------- */
static void confirm_close(int confirmed)
{
    ui_confirm_cb_t cb = s_confirm_cb;
    void *user = s_confirm_user;

    if (s_confirm) {
        lv_obj_del(s_confirm);
        s_confirm = NULL;
    }
    if (s_confirm_group) {
        lv_group_del(s_confirm_group);
        s_confirm_group = NULL;
    }
    /* 恢复之前页面的焦点组 */
    if (s_prev_group) {
        ui_input_set_group(s_prev_group);
        s_prev_group = NULL;
    }
    s_confirm_cb = NULL;
    s_confirm_user = NULL;

    if (cb) {
        cb(confirmed, user);
    }
}

static void confirm_yes_cb(lv_event_t *e) { UI_UNUSED(e); confirm_close(1); }
static void confirm_no_cb(lv_event_t *e)  { UI_UNUSED(e); confirm_close(0); }

void ui_popup_confirm(const char *title, ui_confirm_cb_t cb, void *user)
{
    lv_obj_t *box, *lb, *row, *btn_yes, *btn_no, *l;

    if (!s_layer) {
        if (cb) cb(0, user);
        return;
    }
    if (s_confirm) {
        return;   /* 已有确认框 */
    }

    s_confirm_cb = cb;
    s_confirm_user = user;

    /* 全屏半透明遮罩 */
    s_confirm = lv_obj_create(s_layer);
    lv_obj_remove_style_all(s_confirm);
    lv_obj_set_size(s_confirm, UI_SCREEN_W, UI_SCREEN_H);
    lv_obj_set_style_bg_color(s_confirm, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(s_confirm, LV_OPA_50, 0);
    lv_obj_clear_flag(s_confirm, LV_OBJ_FLAG_SCROLLABLE);

    /* 对话框面板 */
    box = lv_obj_create(s_confirm);
    lv_obj_add_style(box, (lv_style_t *)ui_theme_panel(), 0);
    lv_obj_set_style_bg_opa(box, LV_OPA_COVER, 0);
    lv_obj_set_size(box, UI_SCREEN_W - 40, LV_SIZE_CONTENT);
    lv_obj_center(box);
    lv_obj_set_flex_flow(box, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(box, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(box, 10, 0);

    lb = lv_label_create(box);
    lv_label_set_text(lb, title ? title : ui_lang_tr(UI_STR_CONFIRM_TITLE));
    lv_label_set_long_mode(lb, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(lb, lv_pct(100));
    lv_obj_set_style_text_align(lb, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(lb, UI_COLOR_TEXT, 0);

    row = lv_obj_create(box);
    lv_obj_remove_style_all(row);
    lv_obj_set_size(row, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    btn_no = lv_btn_create(row);
    lv_obj_set_size(btn_no, 80, 32);
    ui_theme_apply_focusable(btn_no);
    l = lv_label_create(btn_no);
    lv_label_set_text_fmt(l, "%s %s", UI_ICO_CLOSE, ui_lang_tr(UI_STR_CANCEL));
    lv_obj_center(l);
    lv_obj_add_event_cb(btn_no, confirm_no_cb, LV_EVENT_CLICKED, NULL);

    btn_yes = lv_btn_create(row);
    lv_obj_set_size(btn_yes, 80, 32);
    ui_theme_apply_focusable(btn_yes);
    l = lv_label_create(btn_yes);
    lv_label_set_text_fmt(l, "%s %s", UI_ICO_OK, ui_lang_tr(UI_STR_CONFIRM_OK));
    lv_obj_center(l);
    lv_obj_add_event_cb(btn_yes, confirm_yes_cb, LV_EVENT_CLICKED, NULL);

    /* 为确认框建独立焦点组并接管输入；关闭后恢复 */
    s_confirm_group = lv_group_create();
    lv_group_add_obj(s_confirm_group, btn_no);
    lv_group_add_obj(s_confirm_group, btn_yes);
    lv_group_focus_obj(btn_no);
    s_prev_group = NULL;   /* 由输入层当前组保存可选；样机关闭后由页面 on_show 重设 */
    ui_input_set_group(s_confirm_group);
}

/* ---------------- 处理中遮罩 ---------------- */
void ui_popup_busy_show(const char *text)
{
    lv_obj_t *spin, *lb;
    if (!s_layer) {
        return;
    }
    if (s_busy) {
        return;
    }
    s_busy = lv_obj_create(s_layer);
    lv_obj_remove_style_all(s_busy);
    lv_obj_set_size(s_busy, UI_SCREEN_W, UI_SCREEN_H);
    lv_obj_set_style_bg_color(s_busy, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(s_busy, LV_OPA_60, 0);
    lv_obj_clear_flag(s_busy, LV_OBJ_FLAG_SCROLLABLE);

    spin = lv_spinner_create(s_busy, 1000, 60);
    lv_obj_set_size(spin, 40, 40);
    lv_obj_align(spin, LV_ALIGN_CENTER, 0, -10);
    lv_obj_set_style_arc_color(spin, UI_COLOR_ACCENT, LV_PART_INDICATOR);

    lb = lv_label_create(s_busy);
    lv_label_set_text(lb, text ? text : ui_lang_tr(UI_STR_PROCESSING));
    lv_obj_set_style_text_color(lb, UI_COLOR_TEXT, 0);
    lv_obj_align(lb, LV_ALIGN_CENTER, 0, 28);
}

void ui_popup_busy_hide(void)
{
    if (s_busy) {
        lv_obj_del(s_busy);
        s_busy = NULL;
    }
}
