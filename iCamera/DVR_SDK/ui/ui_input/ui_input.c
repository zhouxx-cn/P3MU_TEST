/**
 * @file    ui_input.c
 * @brief   输入抽象层实现：keypad + pointer 两个 LVGL indev。
 *
 * @details
 *  LVGL 的 read_cb 在 UI 线程被周期调用。我们把"来源线程 → ui_msgq → UI 线程”
 *  取到的按键/点击，缓存在本模块静态变量里，read_cb 读取这些缓存返回给 LVGL。
 */
#include "ui_input.h"

/* ---------- keypad indev 状态 ---------- */
static lv_indev_drv_t s_kp_drv;
static lv_indev_t    *s_kp_indev = NULL;
static lv_group_t    *s_cur_group = NULL;

static volatile uint32_t s_key_val = 0;     /* 最近一次 LVGL key */
static volatile uint8_t  s_key_pressed = 0; /* 是否处于按下 */

/* ---------- pointer indev 状态 ---------- */
static lv_indev_drv_t s_pt_drv;
static lv_indev_t    *s_pt_indev = NULL;
static volatile int16_t s_pt_x = 0;
static volatile int16_t s_pt_y = 0;
static volatile uint8_t s_pt_pressed = 0;

/* 逻辑按键 → LVGL key 映射 */
static uint32_t map_key(ui_key_code_t code)
{
    switch (code) {
    case UI_KEY_UP:   return LV_KEY_PREV;   /* 上：焦点上一个 */
    case UI_KEY_DOWN: return LV_KEY_NEXT;   /* 下：焦点下一个 */
    case UI_KEY_OK:   return LV_KEY_ENTER;  /* 确认 */
    case UI_KEY_BACK: return LV_KEY_ESC;    /* 返回 */
    default:          return 0;
    }
}

/* ---------- LVGL read 回调 ---------- */
static void keypad_read_cb(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    UI_UNUSED(drv);
    data->key = s_key_val;
    data->state = s_key_pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}

static void pointer_read_cb(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    UI_UNUSED(drv);
    data->point.x = s_pt_x;
    data->point.y = s_pt_y;
    data->state = s_pt_pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}

int ui_input_init(void)
{
    /* keypad */
    lv_indev_drv_init(&s_kp_drv);
    s_kp_drv.type = LV_INDEV_TYPE_KEYPAD;
    s_kp_drv.read_cb = keypad_read_cb;
    s_kp_indev = lv_indev_drv_register(&s_kp_drv);
    if (!s_kp_indev) {
        return UI_ERR;
    }

    /* pointer（APP 模拟点击） */
    lv_indev_drv_init(&s_pt_drv);
    s_pt_drv.type = LV_INDEV_TYPE_POINTER;
    s_pt_drv.read_cb = pointer_read_cb;
    s_pt_indev = lv_indev_drv_register(&s_pt_drv);
    if (!s_pt_indev) {
        return UI_ERR;
    }

    UI_LOGI("ui_input init ok (keypad+pointer)");
    return UI_OK;
}

void ui_input_deinit(void)
{
    s_kp_indev = NULL;
    s_pt_indev = NULL;
    s_cur_group = NULL;
}

void ui_input_set_group(lv_group_t *group)
{
    s_cur_group = group;
    if (s_kp_indev && group) {
        lv_indev_set_group(s_kp_indev, group);
    }
}

void ui_input_feed(const ui_event_t *evt)
{
    if (!evt) {
        return;
    }
    if (evt->type == UI_EVT_KEY) {
        uint32_t k = map_key(evt->u.key.code);
        if (k != 0) {
            s_key_val = k;
            s_key_pressed = (evt->u.key.act == UI_KEY_ACT_PRESS) ? 1 : 0;
        }
    } else if (evt->type == UI_EVT_POINT) {
        s_pt_x = evt->u.point.x;
        s_pt_y = evt->u.point.y;
        s_pt_pressed = evt->u.point.pressed;
    }
}
