/**
 * @file    ui_setting.c
 * @brief   参数设置页实现：滚动列表 + 开关/多选项 + 动作项（含确认框）。
 *
 * @details
 *  列表项类型：
 *    - TOGGLE : 开关（循环录像/WDR/音频/水印/WiFi）
 *    - CHOICE : 多选项循环切换（分辨率/翻转/红外）
 *    - ACTION : 动作（格式化/恢复出厂/WiFi 配网），危险动作走确认框
 *  OK：toggle 翻转 / choice 切下一个 / action 执行
 *  BACK：保存并返回菜单。
 */
#include "ui_page.h"
#include "ui_setting.h"
#include "ui_res.h"
#include "ui_theme.h"
#include "ui_popup.h"
#include "ui_cam.h"
#include "ui_msgq.h"
#include <stdio.h>

static ui_page_t        s_page;
static ui_settings_t    s_cfg;
static ui_settings_ops_t s_ops = {0};

/* 设置项类型 */
typedef enum { ITEM_TOGGLE, ITEM_CHOICE, ITEM_ACTION } item_kind_t;

/* 动作 ID */
typedef enum { ACT_FORMAT, ACT_FACTORY, ACT_WIFI } action_id_t;

typedef struct {
    const char   *name;
    item_kind_t   kind;
    /* toggle: 指向 cfg 中的 uint8_t 字段；choice: 指向枚举字段 + 选项表；action: id */
    void         *field;
    const char  **choices;
    int           choice_cnt;
    action_id_t   action;
} setting_item_t;

static const char *res_choices[]  = { "2304x1296", "1920x1080", "1280x720" };
static const char *flip_choices[] = { "关闭", "上下翻", "左右镜像", "旋转180" };
static const char *ir_choices[]   = { "自动", "常开", "关闭" };

static setting_item_t s_items[] = {
    { "分辨率",   ITEM_CHOICE, &s_cfg.resolution, res_choices,  3, 0 },
    { "循环录像", ITEM_TOGGLE, &s_cfg.loop_record, NULL,        0, 0 },
    { "WDR宽动态",ITEM_TOGGLE, &s_cfg.wdr,         NULL,        0, 0 },
    { "画面翻转", ITEM_CHOICE, &s_cfg.flip,        flip_choices, 4, 0 },
    { "红外夜视", ITEM_CHOICE, &s_cfg.ir,          ir_choices,   3, 0 },
    { "录音",     ITEM_TOGGLE, &s_cfg.audio,       NULL,        0, 0 },
    { "时间水印", ITEM_TOGGLE, &s_cfg.watermark,   NULL,        0, 0 },
    { "WiFi",     ITEM_TOGGLE, &s_cfg.wifi_on,     NULL,        0, 0 },
    { "WiFi配网", ITEM_ACTION, NULL,               NULL,        0, ACT_WIFI },
    { "格式化",   ITEM_ACTION, NULL,               NULL,        0, ACT_FORMAT },
    { "恢复出厂", ITEM_ACTION, NULL,               NULL,        0, ACT_FACTORY },
};

static lv_obj_t *s_rows[UI_ARRAY_SIZE(s_items)];
static lv_obj_t *s_vals[UI_ARRAY_SIZE(s_items)];

void ui_setting_set_ops(const ui_settings_ops_t *ops)
{
    if (ops) s_ops = *ops;
    else     memset(&s_ops, 0, sizeof(s_ops));
}

/* 刷新某行右侧值显示 */
static void refresh_value(int i)
{
    const setting_item_t *it = &s_items[i];
    lv_obj_t *v = s_vals[i];
    if (!v) return;

    if (it->kind == ITEM_TOGGLE) {
        uint8_t on = *(uint8_t *)it->field;
        lv_label_set_text(v, on ? UI_ICO_OK " 开" : "关");
        lv_obj_set_style_text_color(v, on ? UI_COLOR_OK : UI_COLOR_TEXT_DIM, 0);
    } else if (it->kind == ITEM_CHOICE) {
        int idx = *(int *)it->field;
        idx = UI_CLAMP(idx, 0, it->choice_cnt - 1);
        lv_label_set_text(v, it->choices[idx]);
        lv_obj_set_style_text_color(v, UI_COLOR_ACCENT, 0);
    } else {
        lv_label_set_text(v, UI_ICO_RIGHT);
        lv_obj_set_style_text_color(v, UI_COLOR_TEXT_DIM, 0);
    }
}

/* 危险动作确认回调 */
static void on_format_confirm(int ok, void *user)
{
    UI_UNUSED(user);
    if (!ok) return;
    ui_popup_busy_show("格式化中...");
    if (s_ops.format_storage) s_ops.format_storage();
    ui_popup_busy_hide();
    ui_popup_toast("格式化完成", 1200);
}

static void on_factory_confirm(int ok, void *user)
{
    UI_UNUSED(user);
    if (!ok) return;
    if (s_ops.factory_reset) s_ops.factory_reset();
    /* 恢复默认值并刷新界面 */
    memset(&s_cfg, 0, sizeof(s_cfg));
    s_cfg.resolution = UI_RES_2304x1296;
    s_cfg.audio = 1;
    for (unsigned i = 0; i < UI_ARRAY_SIZE(s_items); i++) refresh_value(i);
    ui_popup_toast("已恢复出厂设置", 1200);
}

/* 执行动作项 */
static void do_action(action_id_t act)
{
    switch (act) {
    case ACT_FORMAT:
        ui_popup_confirm("确定格式化存储?\n所有文件将被删除", on_format_confirm, NULL);
        break;
    case ACT_FACTORY:
        ui_popup_confirm("确定恢复出厂设置?", on_factory_confirm, NULL);
        break;
    case ACT_WIFI:
        if (s_ops.wifi_config_start) s_ops.wifi_config_start();
        ui_popup_toast("请用APP扫码配网", 1500);
        break;
    default:
        break;
    }
}

/* 行点击/确认：根据类型处理 */
static void row_activate(int i)
{
    setting_item_t *it = &s_items[i];
    switch (it->kind) {
    case ITEM_TOGGLE: {
        uint8_t *f = (uint8_t *)it->field;
        *f = !*f;
        refresh_value(i);
        if (s_ops.apply) s_ops.apply(&s_cfg);
        /* 音频开关同步到状态栏（经消息队列，保持同一 UI 线程语义） */
        if (it->field == &s_cfg.audio) {
            ui_event_t e = { .type = UI_EVT_BIZ };
            e.u.biz.type  = UI_BIZ_AUDIO;
            e.u.biz.value = s_cfg.audio;
            ui_msgq_post(&e);
        }
        break;
    }
    case ITEM_CHOICE: {
        int *f = (int *)it->field;
        *f = (*f + 1) % it->choice_cnt;
        refresh_value(i);
        if (s_ops.apply) s_ops.apply(&s_cfg);
        break;
    }
    case ITEM_ACTION:
        do_action(it->action);
        break;
    }
}

static void row_click_cb(lv_event_t *e)
{
    int i = (int)(intptr_t)lv_event_get_user_data(e);
    row_activate(i);
}

static lv_obj_t *setting_create(ui_page_t *self, lv_obj_t *parent)
{
    lv_obj_t *root, *list, *title;
    unsigned i;

    /* 载入设置（桩：用当前默认值） */
    if (s_ops.load) {
        s_ops.load(&s_cfg);
    } else {
        memset(&s_cfg, 0, sizeof(s_cfg));
        s_cfg.resolution = UI_RES_2304x1296;
        s_cfg.audio = 1;
        s_cfg.watermark = 1;
    }

    root = lv_obj_create(parent);
    lv_obj_remove_style_all(root);
    lv_obj_set_size(root, UI_SCREEN_W, UI_SCREEN_H);
    lv_obj_set_style_bg_color(root, UI_COLOR_BG, 0);
    lv_obj_set_style_bg_opa(root, LV_OPA_COVER, 0);
    lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);

    title = lv_label_create(root);
    lv_label_set_text(title, UI_ICO_SETTING "  设置");
    lv_obj_set_style_text_font(title, UI_FONT_MEDIUM, 0);
    lv_obj_set_style_text_color(title, UI_COLOR_TEXT, 0);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 8, UI_STATUSBAR_TOP_H + 4);

    /* 可滚动设置列表 */
    list = lv_obj_create(root);
    lv_obj_remove_style_all(list);
    lv_obj_set_size(list, UI_SCREEN_W - 12,
                    UI_SCREEN_H - UI_STATUSBAR_TOP_H - UI_STATUSBAR_BOT_H - 40);
    lv_obj_align(list, LV_ALIGN_TOP_MID, 0, UI_STATUSBAR_TOP_H + 28);
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(list, 3, 0);
    lv_obj_set_scroll_dir(list, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(list, LV_SCROLLBAR_MODE_AUTO);

    for (i = 0; i < UI_ARRAY_SIZE(s_items); i++) {
        lv_obj_t *row = lv_btn_create(list);
        lv_obj_remove_style_all(row);
        lv_obj_set_width(row, lv_pct(100));
        lv_obj_set_height(row, 32);
        ui_theme_apply_focusable(row);
        lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_hor(row, 8, 0);

        lv_obj_t *nm = lv_label_create(row);
        lv_label_set_text(nm, s_items[i].name);

        lv_obj_t *val = lv_label_create(row);
        s_vals[i] = val;
        refresh_value(i);

        lv_obj_add_event_cb(row, row_click_cb, LV_EVENT_CLICKED, (void *)(intptr_t)i);
        lv_group_add_obj(self->group, row);
        s_rows[i] = row;
    }

    return root;
}

static int setting_on_event(ui_page_t *self, const ui_event_t *evt)
{
    UI_UNUSED(self);
    if (evt->type == UI_EVT_KEY && evt->u.key.act == UI_KEY_ACT_PRESS &&
        evt->u.key.code == UI_KEY_BACK) {
        if (s_ops.save) s_ops.save(&s_cfg);
        ui_cam_set_mode(UI_MODE_RECORD);
        ui_page_pop();
        return 1;
    }
    return 0;
}

void ui_page_setting_register(void)
{
    memset(&s_page, 0, sizeof(s_page));
    s_page.id       = UI_PAGE_SETTING;
    s_page.name     = "setting";
    s_page.create   = setting_create;
    s_page.on_event = setting_on_event;
    ui_page_register(&s_page);
}
