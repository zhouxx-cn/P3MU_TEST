/**
 * @file    ui_page_preview.c
 * @brief   预览页：实时视频取景 + 焦点/录制提示 + 快门/录制触发。
 *
 * @details
 *  - 视频由 ui_video 画在底层，本页只放"浮层 UI"：中心快门提示、延时倒计时、
 *    底部操作提示；本页背景透明，完全露出视频。
 *  - 交互：OK=快门/录制开关（按模式）；UP/DOWN=切换模式；BACK=进入模式菜单。
 *  - 拍照定格动画：OK 拍照时做一次白闪 + 缩放，模拟快门。
 *  - 1Hz 定时器驱动 ui_cam_tick_1hz（录制时长/容量）。
 */
#include "ui_page.h"
#include "ui_cam.h"
#include "ui_video.h"
#include "ui_res.h"
#include "ui_theme.h"
#include "ui_menu.h"
#include <stdio.h>

static ui_page_t s_page;

/* 浮层控件 */
static lv_obj_t  *s_hint = NULL;        /* 底部操作提示 */
static lv_obj_t  *s_center = NULL;      /* 中心模式提示（拍照/延时） */
static lv_obj_t  *s_flash = NULL;       /* 拍照白闪遮罩 */
static lv_obj_t  *s_tl_count = NULL;    /* 延时倒计时标签 */
static lv_timer_t *s_sec_timer = NULL;  /* 1Hz 业务定时器 */

/* 拍照白闪动画结束：隐藏遮罩 */
static void flash_anim_cb(void *obj, int32_t v)
{
    lv_obj_set_style_bg_opa((lv_obj_t *)obj, (lv_opa_t)v, 0);
}

static void do_shutter_flash(void)
{
    if (!s_flash) {
        return;
    }
    lv_obj_clear_flag(s_flash, LV_OBJ_FLAG_HIDDEN);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, s_flash);
    lv_anim_set_values(&a, LV_OPA_COVER, LV_OPA_TRANSP);
    lv_anim_set_time(&a, 260);
    lv_anim_set_exec_cb(&a, flash_anim_cb);
    lv_anim_start(&a);
}

/* 根据模式刷新中心/底部提示文案 */
static void refresh_hint(void)
{
    ui_work_mode_t m = ui_cam_mode();
    const char *center = "";
    const char *hint = "OK " LV_SYMBOL_BULLET " 录制    BACK " LV_SYMBOL_BULLET " 菜单";

    switch (m) {
    case UI_MODE_RECORD:
        center = "";
        hint = "OK:录像  ▲▼:切模式  BACK:菜单";
        break;
    case UI_MODE_PHOTO:
        center = UI_ICO_PHOTO;
        hint = "OK:拍照  ▲▼:切模式  BACK:菜单";
        break;
    case UI_MODE_TIMELAPSE:
        center = UI_ICO_TIMELAPSE;
        hint = "OK:延时  ▲▼:切模式  BACK:菜单";
        break;
    default:
        break;
    }
    if (s_center) lv_label_set_text(s_center, center);
    if (s_hint)   lv_label_set_text(s_hint, hint);
}

/* 1Hz 定时器 */
static void sec_timer_cb(lv_timer_t *t)
{
    UI_UNUSED(t);
    ui_cam_tick_1hz();
}

/* ---------- 页面接口实现 ---------- */
static lv_obj_t *preview_create(ui_page_t *self, lv_obj_t *parent)
{
    UI_UNUSED(self);
    lv_obj_t *root = lv_obj_create(parent);
    lv_obj_remove_style_all(root);
    lv_obj_set_size(root, UI_SCREEN_W, UI_SCREEN_H);
    lv_obj_set_style_bg_opa(root, LV_OPA_TRANSP, 0);   /* 透明，露出视频 */
    lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);

    /* 中心模式图标提示 */
    s_center = lv_label_create(root);
    lv_obj_set_style_text_font(s_center, UI_FONT_LARGE, 0);
    lv_obj_set_style_text_color(s_center, UI_COLOR_TEXT, 0);
    lv_obj_center(s_center);
    lv_label_set_text(s_center, "");

    /* 延时倒计时 */
    s_tl_count = lv_label_create(root);
    lv_obj_set_style_text_font(s_tl_count, UI_FONT_LARGE, 0);
    lv_obj_set_style_text_color(s_tl_count, UI_COLOR_ACCENT, 0);
    lv_obj_align(s_tl_count, LV_ALIGN_CENTER, 0, -24);
    lv_obj_add_flag(s_tl_count, LV_OBJ_FLAG_HIDDEN);

    /* 底部操作提示（在底部状态栏之上一点） */
    s_hint = lv_label_create(root);
    lv_obj_set_style_text_font(s_hint, UI_FONT_SMALL, 0);
    lv_obj_set_style_text_color(s_hint, UI_COLOR_TEXT_DIM, 0);
    lv_obj_align(s_hint, LV_ALIGN_BOTTOM_MID, 0, -(UI_STATUSBAR_BOT_H + 4));

    /* 拍照白闪遮罩（覆盖全屏，默认隐藏） */
    s_flash = lv_obj_create(root);
    lv_obj_remove_style_all(s_flash);
    lv_obj_set_size(s_flash, UI_SCREEN_W, UI_SCREEN_H);
    lv_obj_set_style_bg_color(s_flash, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(s_flash, LV_OPA_TRANSP, 0);
    lv_obj_add_flag(s_flash, LV_OBJ_FLAG_HIDDEN);

    return root;
}

static void preview_on_show(ui_page_t *self)
{
    UI_UNUSED(self);
    refresh_hint();
    ui_video_set_visible(1);
    if (!s_sec_timer) {
        s_sec_timer = lv_timer_create(sec_timer_cb, 1000, NULL);
    }
}

static void preview_on_hide(ui_page_t *self)
{
    UI_UNUSED(self);
    if (s_sec_timer) {
        lv_timer_del(s_sec_timer);
        s_sec_timer = NULL;
    }
}

static int preview_on_event(ui_page_t *self, const ui_event_t *evt)
{
    UI_UNUSED(self);
    if (evt->type != UI_EVT_KEY || evt->u.key.act != UI_KEY_ACT_PRESS) {
        return 0;
    }

    switch (evt->u.key.code) {
    case UI_KEY_OK:
        switch (ui_cam_mode()) {
        case UI_MODE_RECORD:
            ui_cam_toggle_record();
            break;
        case UI_MODE_PHOTO:
            do_shutter_flash();
            ui_cam_snap();
            break;
        case UI_MODE_TIMELAPSE:
            ui_cam_toggle_timelapse(3);  /* 样机默认 3s 间隔 */
            break;
        default:
            break;
        }
        return 1;

    case UI_KEY_UP:
        /* 向前切换可用拍摄模式（录像↔拍照↔延时循环） */
        {
            int m = (int)ui_cam_mode();
            m = (m + UI_MODE_TIMELAPSE) % (UI_MODE_TIMELAPSE + 1); /* -1 回绕 */
            ui_cam_set_mode((ui_work_mode_t)m);
            refresh_hint();
        }
        return 1;

    case UI_KEY_DOWN:
        {
            int m = (int)ui_cam_mode();
            m = (m + 1) % (UI_MODE_TIMELAPSE + 1);
            ui_cam_set_mode((ui_work_mode_t)m);
            refresh_hint();
        }
        return 1;

    case UI_KEY_BACK:
        /* 录制中不允许直接进菜单，先提示 */
        if (ui_cam_is_recording()) {
            return 1;
        }
        ui_page_push(UI_PAGE_MENU);
        return 1;

    default:
        break;
    }
    return 0;
}

void ui_page_preview_register(void)
{
    memset(&s_page, 0, sizeof(s_page));
    s_page.id       = UI_PAGE_PREVIEW;
    s_page.name     = "preview";
    s_page.create   = preview_create;
    s_page.on_show  = preview_on_show;
    s_page.on_hide  = preview_on_hide;
    s_page.on_event = preview_on_event;
    ui_page_register(&s_page);

    ui_cam_init();
}
