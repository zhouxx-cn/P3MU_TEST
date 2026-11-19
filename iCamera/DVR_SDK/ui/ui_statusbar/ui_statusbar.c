/**
 * @file    ui_statusbar.c
 * @brief   状态栏实现：顶部栏 + 底部栏 + 录制红点闪烁。
 *
 * @details
 *  顶部栏(22px)：左=录制红点+已录时长，中=系统时间，右=WiFi+电量
 *  底部栏(24px)：左=当前模式图标，中=剩余可录/拍，右=音频+防抖
 *  半透明底衬保证视频背景上的可读性。
 */
#include "ui_statusbar.h"
#include "ui_res.h"
#include "ui_theme.h"
#include <stdio.h>

/* 顶部控件 */
static lv_obj_t *s_top_bar = NULL;
static lv_obj_t *s_rec_dot = NULL;    /* 录制红点（LED） */
static lv_obj_t *s_rec_time = NULL;   /* 已录时长 */
static lv_obj_t *s_clock = NULL;      /* 时间 */
static lv_obj_t *s_wifi = NULL;       /* WiFi 图标 */
static lv_obj_t *s_batt = NULL;       /* 电量图标+百分比 */

/* 底部控件 */
static lv_obj_t *s_bot_bar = NULL;
static lv_obj_t *s_mode = NULL;       /* 模式图标+文字 */
static lv_obj_t *s_capacity = NULL;   /* 剩余可录 */
static lv_obj_t *s_audio = NULL;      /* 音频开关图标 */
static lv_obj_t *s_eis = NULL;        /* 防抖图标 */

/* 录制红点闪烁定时器 */
static lv_timer_t *s_blink_timer = NULL;
static int s_rec_on = 0;

static const char *mode_text(ui_work_mode_t m)
{
    switch (m) {
    case UI_MODE_RECORD:    return UI_ICO_REC " REC";
    case UI_MODE_PHOTO:     return UI_ICO_PHOTO " PHOTO";
    case UI_MODE_TIMELAPSE: return UI_ICO_TIMELAPSE " TL";
    case UI_MODE_PLAYBACK:  return UI_ICO_PLAYBACK " PLAY";
    case UI_MODE_SETTING:   return UI_ICO_SETTING " SET";
    default:                return "";
    }
}

/* 创建一个透明条，内部用 flex 横向排布 */
static lv_obj_t *make_bar(lv_obj_t *parent, int y, int h)
{
    lv_obj_t *bar = lv_obj_create(parent);
    lv_obj_remove_style_all(bar);
    lv_obj_set_size(bar, UI_SCREEN_W, h);
    lv_obj_set_pos(bar, 0, y);
    lv_obj_set_style_bg_color(bar, UI_COLOR_MASK, 0);
    lv_obj_set_style_bg_opa(bar, LV_OPA_40, 0);   /* 半透明黑衬 */
    lv_obj_set_style_pad_hor(bar, 4, 0);
    lv_obj_clear_flag(bar, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(bar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(bar, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    return bar;
}

static lv_obj_t *make_label(lv_obj_t *parent, const char *txt, const lv_font_t *font,
                            lv_color_t color)
{
    lv_obj_t *lb = lv_label_create(parent);
    lv_label_set_text(lb, txt);
    lv_obj_set_style_text_font(lb, font, 0);
    lv_obj_set_style_text_color(lb, color, 0);
    return lb;
}

/* 录制红点闪烁回调 */
static void blink_cb(lv_timer_t *t)
{
    UI_UNUSED(t);
    static int on = 0;
    on = !on;
    if (s_rec_dot) {
        lv_led_set_brightness(s_rec_dot, on ? 255 : 40);
    }
}

int ui_statusbar_init(lv_obj_t *top_layer)
{
    lv_obj_t *left, *right;

    if (!top_layer) {
        return UI_ERR_PARAM;
    }

    /* ---------------- 顶部栏 ---------------- */
    s_top_bar = make_bar(top_layer, 0, UI_STATUSBAR_TOP_H);

    /* 左：录制红点 + 已录时长 */
    left = lv_obj_create(s_top_bar);
    lv_obj_remove_style_all(left);
    lv_obj_set_size(left, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(left, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(left, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(left, 3, 0);
    s_rec_dot = lv_led_create(left);
    lv_led_set_color(s_rec_dot, UI_COLOR_REC);
    lv_obj_set_size(s_rec_dot, 8, 8);
    lv_led_off(s_rec_dot);
    s_rec_time = make_label(left, "00:00", UI_FONT_SMALL, UI_COLOR_TEXT);
    lv_obj_add_flag(s_rec_time, LV_OBJ_FLAG_HIDDEN);

    /* 中：时间 */
    s_clock = make_label(s_top_bar, "--:--", UI_FONT_SMALL, UI_COLOR_TEXT);

    /* 右：WiFi + 电量 */
    right = lv_obj_create(s_top_bar);
    lv_obj_remove_style_all(right);
    lv_obj_set_size(right, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(right, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(right, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(right, 4, 0);
    s_wifi = make_label(right, UI_ICO_WIFI, UI_FONT_SMALL, UI_COLOR_TEXT_DIM);
    s_batt = make_label(right, UI_ICO_BATTERY_FULL " 100%", UI_FONT_SMALL, UI_COLOR_TEXT);

    /* ---------------- 底部栏 ---------------- */
    s_bot_bar = make_bar(top_layer, UI_SCREEN_H - UI_STATUSBAR_BOT_H, UI_STATUSBAR_BOT_H);
    s_mode = make_label(s_bot_bar, mode_text(UI_MODE_RECORD), UI_FONT_SMALL, UI_COLOR_ACCENT);
    s_capacity = make_label(s_bot_bar, "01:23:45", UI_FONT_SMALL, UI_COLOR_TEXT);

    right = lv_obj_create(s_bot_bar);
    lv_obj_remove_style_all(right);
    lv_obj_set_size(right, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(right, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(right, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(right, 4, 0);
    s_audio = make_label(right, UI_ICO_AUDIO_ON, UI_FONT_SMALL, UI_COLOR_OK);
    s_eis = make_label(right, UI_ICO_EIS, UI_FONT_SMALL, UI_COLOR_TEXT_DIM);

    UI_LOGI("ui_statusbar init ok");
    return UI_OK;
}

void ui_statusbar_on_biz(const ui_evt_biz_t *biz)
{
    char buf[32];

    if (!biz) {
        return;
    }
    switch (biz->type) {
    case UI_BIZ_BATTERY:
        snprintf(buf, sizeof(buf), "%s %d%%",
                 biz->aux ? UI_ICO_CHARGE : ui_res_battery_symbol(biz->value), biz->value);
        if (s_batt) lv_label_set_text(s_batt, buf);
        break;

    case UI_BIZ_WIFI:
        if (s_wifi) {
            lv_label_set_text(s_wifi, UI_ICO_WIFI);
            lv_obj_set_style_text_color(s_wifi,
                biz->aux ? UI_COLOR_TEXT : UI_COLOR_TEXT_DIM, 0);
        }
        break;

    case UI_BIZ_SDCARD:
        if (s_capacity) {
            if (biz->aux == 0) {
                lv_label_set_text(s_capacity, UI_ICO_SD " --");
            } else if (biz->aux == 2) {
                lv_label_set_text(s_capacity, UI_ICO_WARNING " SD");
            } else {
                /* 剩余可录分钟 → HH:MM */
                int mm = biz->value;
                snprintf(buf, sizeof(buf), "%s %02d:%02d", UI_ICO_SD, mm / 60, mm % 60);
                lv_label_set_text(s_capacity, buf);
            }
        }
        break;

    case UI_BIZ_REC_STATE:
        if (biz->value) {
            /* 录制中：显示时长并开始闪烁 */
            int sec = biz->aux;
            snprintf(buf, sizeof(buf), "%02d:%02d", sec / 60, sec % 60);
            if (s_rec_time) {
                lv_label_set_text(s_rec_time, buf);
                lv_obj_clear_flag(s_rec_time, LV_OBJ_FLAG_HIDDEN);
            }
            ui_statusbar_set_rec_blink(1);
        } else {
            if (s_rec_time) lv_obj_add_flag(s_rec_time, LV_OBJ_FLAG_HIDDEN);
            ui_statusbar_set_rec_blink(0);
        }
        break;

    case UI_BIZ_MODE:
        if (s_mode) lv_label_set_text(s_mode, mode_text((ui_work_mode_t)biz->value));
        break;

    case UI_BIZ_AUDIO:
        if (s_audio) {
            lv_label_set_text(s_audio, biz->value ? UI_ICO_AUDIO_ON : UI_ICO_AUDIO_OFF);
            lv_obj_set_style_text_color(s_audio, biz->value ? UI_COLOR_OK : UI_COLOR_TEXT_DIM, 0);
        }
        break;

    case UI_BIZ_EIS:
        if (s_eis) {
            lv_obj_set_style_text_color(s_eis, biz->value ? UI_COLOR_OK : UI_COLOR_TEXT_DIM, 0);
        }
        break;

    case UI_BIZ_TIME: {
        /* value = Unix 秒；此处仅取时:分（真机可接 RTC/localtime） */
        time_t t = (time_t)biz->value;
        struct tm tmv;
        localtime_r(&t, &tmv);
        snprintf(buf, sizeof(buf), "%02d:%02d", tmv.tm_hour, tmv.tm_min);
        if (s_clock) lv_label_set_text(s_clock, buf);
        break;
    }
    default:
        break;
    }
}

void ui_statusbar_set_visible(int show)
{
    if (s_top_bar) {
        if (show) lv_obj_clear_flag(s_top_bar, LV_OBJ_FLAG_HIDDEN);
        else      lv_obj_add_flag(s_top_bar, LV_OBJ_FLAG_HIDDEN);
    }
    if (s_bot_bar) {
        if (show) lv_obj_clear_flag(s_bot_bar, LV_OBJ_FLAG_HIDDEN);
        else      lv_obj_add_flag(s_bot_bar, LV_OBJ_FLAG_HIDDEN);
    }
}

void ui_statusbar_set_rec_blink(int blinking)
{
    if (blinking) {
        if (s_rec_dot) lv_led_on(s_rec_dot);
        if (!s_blink_timer) {
            s_blink_timer = lv_timer_create(blink_cb, 500, NULL);
        }
        s_rec_on = 1;
    } else {
        if (s_blink_timer) {
            lv_timer_del(s_blink_timer);
            s_blink_timer = NULL;
        }
        if (s_rec_dot) lv_led_off(s_rec_dot);
        s_rec_on = 0;
    }
}
