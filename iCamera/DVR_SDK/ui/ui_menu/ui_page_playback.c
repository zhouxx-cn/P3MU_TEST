/**
 * @file    ui_page_playback.c
 * @brief   文件回放页：缩略图浏览、播放、删除。
 *
 * @details
 *  文件列表通过可注入的 ui_pb_source_t 钩子提供（样机用桩造几条假数据），
 *  真机把钩子接到 DVR_SDK 的 SnapShot/录像文件管理即可，页面逻辑不变。
 *  交互：UP/DOWN 选择缩略图，OK 播放，长按/菜单键删除（样机用 OK 播放、
 *  在选中态下 BACK 之外提供删除确认弹窗）。
 */
#include "ui_page.h"
#include "ui_menu.h"
#include "ui_res.h"
#include "ui_theme.h"
#include "ui_popup.h"
#include "ui_cam.h"
#include <stdio.h>

/** @brief 回放文件项。 */
typedef struct {
    char    name[48];   /* 文件名 */
    int     is_video;   /* 1 视频 0 照片 */
    uint32_t duration;  /* 视频时长秒（照片为0） */
} ui_pb_item_t;

/** @brief 回放文件源钩子（真机注入）。 */
typedef struct {
    int (*count)(void);                               /* 文件数量 */
    int (*get)(int index, ui_pb_item_t *out);         /* 取第 index 项 */
    int (*play)(int index);                           /* 播放第 index 项 */
    int (*remove)(int index);                         /* 删除第 index 项 */
} ui_pb_source_t;

static ui_page_t      s_page;
static lv_obj_t      *s_grid = NULL;
static lv_obj_t      *s_empty = NULL;
static ui_pb_source_t s_src = {0};
static int            s_sel = 0;

/* ---------------- 桩文件源（样机默认） ---------------- */
static ui_pb_item_t s_stub[] = {
    { "VID_0001.mp4", 1, 65 },
    { "VID_0002.mp4", 1, 12 },
    { "IMG_0001.jpg", 0, 0  },
    { "IMG_0002.jpg", 0, 0  },
    { "IMG_0003.jpg", 0, 0  },
};
static int stub_count(void) { return (int)UI_ARRAY_SIZE(s_stub); }
static int stub_get(int i, ui_pb_item_t *o)
{
    if (i < 0 || i >= stub_count()) return UI_ERR;
    *o = s_stub[i];
    return UI_OK;
}
static int stub_play(int i)   { UI_LOGI("stub play %d", i); return UI_OK; }
static int stub_remove(int i) { UI_LOGI("stub remove %d", i); return UI_OK; }
static const ui_pb_source_t s_stub_src = { stub_count, stub_get, stub_play, stub_remove };

/** @brief 注入真机回放文件源（可选）。 */
void ui_pb_set_source(const ui_pb_source_t *src)
{
    s_src = src ? *src : s_stub_src;
}

/* 删除确认回调 */
static void on_delete_confirm(int confirmed, void *user)
{
    int idx = (int)(intptr_t)user;
    if (confirmed && s_src.remove) {
        s_src.remove(idx);
        ui_popup_toast("已删除", 1000);
        /* 简化：删除后回菜单重进刷新（样机足够） */
        ui_page_pop();
    }
}

/* 缩略图点击：播放 */
static void thumb_click_cb(lv_event_t *e)
{
    int idx = (int)(intptr_t)lv_event_get_user_data(e);
    s_sel = idx;
    if (s_src.play) {
        s_src.play(idx);
    }
    ui_popup_toast("播放中...", 1000);
}

static void build_grid(ui_page_t *self)
{
    int n, i;
    ui_pb_item_t it;

    n = s_src.count ? s_src.count() : 0;
    if (n <= 0) {
        lv_obj_clear_flag(s_empty, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(s_grid, LV_OBJ_FLAG_HIDDEN);
        return;
    }
    lv_obj_add_flag(s_empty, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(s_grid, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clean(s_grid);

    for (i = 0; i < n; i++) {
        if (!s_src.get || s_src.get(i, &it) != UI_OK) {
            continue;
        }
        lv_obj_t *cell = lv_btn_create(s_grid);
        lv_obj_remove_style_all(cell);
        lv_obj_set_size(cell, 100, 74);
        ui_theme_apply_focusable(cell);
        lv_obj_set_style_bg_color(cell, UI_COLOR_PANEL, 0);
        lv_obj_set_style_bg_opa(cell, LV_OPA_COVER, 0);

        /* 缩略图占位：真机此处贴解码出的 RGB565 小图 */
        lv_obj_t *ico = lv_label_create(cell);
        lv_label_set_text(ico, it.is_video ? UI_ICO_PLAY : UI_ICO_PHOTO);
        lv_obj_set_style_text_font(ico, UI_FONT_LARGE, 0);
        lv_obj_align(ico, LV_ALIGN_CENTER, 0, -6);

        lv_obj_t *nm = lv_label_create(cell);
        if (it.is_video) {
            lv_label_set_text_fmt(nm, "%02d:%02d", it.duration / 60, it.duration % 60);
        } else {
            lv_label_set_text(nm, "IMG");
        }
        lv_obj_set_style_text_font(nm, UI_FONT_SMALL, 0);
        lv_obj_align(nm, LV_ALIGN_BOTTOM_MID, 0, -2);

        lv_obj_add_event_cb(cell, thumb_click_cb, LV_EVENT_CLICKED, (void *)(intptr_t)i);
        lv_group_add_obj(self->group, cell);
    }
}

static lv_obj_t *pb_create(ui_page_t *self, lv_obj_t *parent)
{
    lv_obj_t *root, *title;

    root = lv_obj_create(parent);
    lv_obj_remove_style_all(root);
    lv_obj_set_size(root, UI_SCREEN_W, UI_SCREEN_H);
    lv_obj_set_style_bg_color(root, UI_COLOR_BG, 0);
    lv_obj_set_style_bg_opa(root, LV_OPA_COVER, 0);   /* 回放页不透明，省得视频干扰 */
    lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);

    title = lv_label_create(root);
    lv_label_set_text(title, UI_ICO_PLAYBACK "  回放");
    lv_obj_set_style_text_font(title, UI_FONT_MEDIUM, 0);
    lv_obj_set_style_text_color(title, UI_COLOR_TEXT, 0);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 8, UI_STATUSBAR_TOP_H + 4);

    /* 缩略图网格（两列） */
    s_grid = lv_obj_create(root);
    lv_obj_remove_style_all(s_grid);
    lv_obj_set_size(s_grid, UI_SCREEN_W - 16,
                    UI_SCREEN_H - UI_STATUSBAR_TOP_H - UI_STATUSBAR_BOT_H - 40);
    lv_obj_align(s_grid, LV_ALIGN_TOP_MID, 0, UI_STATUSBAR_TOP_H + 28);
    lv_obj_set_flex_flow(s_grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(s_grid, LV_FLEX_ALIGN_SPACE_EVENLY,
                          LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(s_grid, 6, 0);
    lv_obj_set_style_pad_column(s_grid, 6, 0);

    s_empty = lv_label_create(root);
    lv_label_set_text(s_empty, "暂无文件");
    lv_obj_set_style_text_color(s_empty, UI_COLOR_TEXT_DIM, 0);
    lv_obj_center(s_empty);
    lv_obj_add_flag(s_empty, LV_OBJ_FLAG_HIDDEN);

    /* 默认用桩源 */
    if (!s_src.count) {
        s_src = s_stub_src;
    }
    return root;
}

static void pb_on_show(ui_page_t *self)
{
    build_grid(self);
}

static int pb_on_event(ui_page_t *self, const ui_event_t *evt)
{
    UI_UNUSED(self);
    if (evt->type == UI_EVT_KEY && evt->u.key.act == UI_KEY_ACT_PRESS) {
        if (evt->u.key.code == UI_KEY_BACK) {
            ui_cam_set_mode(UI_MODE_RECORD);
            ui_page_pop();
            return 1;
        }
    }
    return 0;
}

void ui_page_playback_register(void)
{
    memset(&s_page, 0, sizeof(s_page));
    s_page.id       = UI_PAGE_PLAYBACK;
    s_page.name     = "playback";
    s_page.create   = pb_create;
    s_page.on_show  = pb_on_show;
    s_page.on_event = pb_on_event;
    ui_page_register(&s_page);
    UI_UNUSED(on_delete_confirm);  /* 删除确认在长按菜单接入，样机保留接口 */
}
