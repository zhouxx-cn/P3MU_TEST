/**
 * @file    ui_page_menu.c
 * @brief   模式主菜单：录像 / 拍照 / 延时 / 回放 / 设置。
 *
 * @details
 *  半透明面板叠在视频上，纵向列表 + 焦点高亮。UP/DOWN 移焦点，OK 选择，
 *  BACK 返回预览。选择拍摄类模式后切模式并回到预览；回放/设置则入对应页。
 *  列表项同时可被 APP 模拟点击触发（复用 LVGL CLICKED 事件）。
 */
#include "ui_page.h"
#include "ui_menu.h"
#include "ui_cam.h"
#include "ui_res.h"
#include "ui_theme.h"
#include "ui_lang.h"

static ui_page_t s_page;

typedef struct {
    const char     *icon;
    ui_str_id_t     str_id;   /* 多语言文案 ID */
    ui_work_mode_t  mode;     /* 对应模式；UI_MODE_MAX 表示非切模式项 */
    ui_page_id_t    page;     /* 需要跳转的页面；UI_PAGE_MAX 表示仅切模式回预览 */
} menu_item_t;

static const menu_item_t s_items[] = {
    { UI_ICO_REC,       UI_STR_REC,       UI_MODE_RECORD,    UI_PAGE_MAX      },
    { UI_ICO_PHOTO,     UI_STR_PHOTO,     UI_MODE_PHOTO,     UI_PAGE_MAX      },
    { UI_ICO_TIMELAPSE, UI_STR_TIMELAPSE, UI_MODE_TIMELAPSE, UI_PAGE_MAX      },
    { UI_ICO_PLAYBACK,  UI_STR_PLAYBACK,  UI_MODE_MAX,       UI_PAGE_PLAYBACK },
    { UI_ICO_SETTING,   UI_STR_SETTINGS,  UI_MODE_MAX,       UI_PAGE_SETTING  },
};

static lv_obj_t *s_list_btns[UI_ARRAY_SIZE(s_items)];

/* 执行某个菜单项 */
static void activate_item(int idx)
{
    const menu_item_t *it;
    if (idx < 0 || idx >= (int)UI_ARRAY_SIZE(s_items)) {
        return;
    }
    it = &s_items[idx];

    if (it->page != UI_PAGE_MAX) {
        if (it->mode == UI_MODE_MAX && it->page == UI_PAGE_SETTING) {
            ui_cam_set_mode(UI_MODE_SETTING);
        }
        ui_page_push(it->page);
    } else {
        /* 拍摄类模式：切模式并回预览 */
        ui_cam_set_mode(it->mode);
        ui_page_pop();   /* 回到预览 */
    }
}

/* 列表项点击回调（APP 模拟点击/LVGL ENTER 都会触发 CLICKED） */
static void item_click_cb(lv_event_t *e)
{
    int idx = (int)(intptr_t)lv_event_get_user_data(e);
    activate_item(idx);
}

static lv_obj_t *menu_create(ui_page_t *self, lv_obj_t *parent)
{
    lv_obj_t *root, *panel, *title;
    unsigned i;

    root = lv_obj_create(parent);
    lv_obj_remove_style_all(root);
    lv_obj_set_size(root, UI_SCREEN_W, UI_SCREEN_H);
    lv_obj_set_style_bg_opa(root, LV_OPA_TRANSP, 0);
    lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);

    /* 半透明面板，避开上下状态栏 */
    panel = lv_obj_create(root);
    lv_obj_add_style(panel, (lv_style_t *)ui_theme_panel(), 0);
    lv_obj_set_size(panel, UI_SCREEN_W - 24,
                    UI_SCREEN_H - UI_STATUSBAR_TOP_H - UI_STATUSBAR_BOT_H - 24);
    lv_obj_align(panel, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_flex_flow(panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(panel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(panel, 4, 0);
    lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);

    title = lv_label_create(panel);
    lv_label_set_text(title, ui_lang_tr(UI_STR_MODE_MENU));
    lv_obj_set_style_text_font(title, UI_FONT_MEDIUM, 0);
    lv_obj_set_style_text_color(title, UI_COLOR_TEXT, 0);
    lv_obj_set_style_pad_bottom(title, 4, 0);

    /* 菜单项按钮 */
    for (i = 0; i < UI_ARRAY_SIZE(s_items); i++) {
        lv_obj_t *btn = lv_btn_create(panel);
        lv_obj_remove_style_all(btn);
        lv_obj_set_width(btn, lv_pct(100));
        lv_obj_set_height(btn, 34);
        ui_theme_apply_focusable(btn);

        lv_obj_t *lab = lv_label_create(btn);
        lv_label_set_text_fmt(lab, "%s  %s", s_items[i].icon, ui_lang_tr(s_items[i].str_id));
        lv_obj_center(lab);

        lv_obj_add_event_cb(btn, item_click_cb, LV_EVENT_CLICKED, (void *)(intptr_t)i);
        lv_group_add_obj(self->group, btn);
        s_list_btns[i] = btn;
    }

    return root;
}

static void menu_on_show(ui_page_t *self)
{
    /* 默认焦点落在当前模式对应项 */
    unsigned i;
    ui_work_mode_t cur = ui_cam_mode();
    for (i = 0; i < UI_ARRAY_SIZE(s_items); i++) {
        if (s_items[i].mode == cur) {
            lv_group_focus_obj(s_list_btns[i]);
            break;
        }
    }
    UI_UNUSED(self);
}

static int menu_on_event(ui_page_t *self, const ui_event_t *evt)
{
    /* 3 键布局：UP/DOWN/OK 由 LVGL keypad+group 自动处理焦点与点击，
     * 无实体返回键；选中某项即离开本页（切模式回预览或进入子页）。 */
    UI_UNUSED(self);
    UI_UNUSED(evt);
    return 0;
}

void ui_page_menu_register(void)
{
    memset(&s_page, 0, sizeof(s_page));
    s_page.id       = UI_PAGE_MENU;
    s_page.name     = "menu";
    s_page.create   = menu_create;
    s_page.on_show  = menu_on_show;
    s_page.on_event = menu_on_event;
    ui_page_register(&s_page);
}
