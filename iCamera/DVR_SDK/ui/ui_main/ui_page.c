/**
 * @file    ui_page.c
 * @brief   页面栈管理实现。
 */
#include "ui_page.h"
#include "ui_input.h"

#define UI_PAGE_STACK_DEPTH   6

static ui_page_t  *s_registry[UI_PAGE_MAX];       /* id -> 页面实现 */
static ui_page_t  *s_stack[UI_PAGE_STACK_DEPTH];  /* 页面栈 */
static int         s_sp = 0;                       /* 栈顶指针（元素个数） */
static lv_obj_t   *s_content_layer = NULL;

int ui_page_stack_init(lv_obj_t *content_layer)
{
    if (!content_layer) {
        return UI_ERR_PARAM;
    }
    memset(s_registry, 0, sizeof(s_registry));
    memset(s_stack, 0, sizeof(s_stack));
    s_sp = 0;
    s_content_layer = content_layer;
    return UI_OK;
}

int ui_page_register(ui_page_t *page)
{
    if (!page || page->id >= UI_PAGE_MAX) {
        return UI_ERR_PARAM;
    }
    s_registry[page->id] = page;
    page->root  = NULL;
    page->group = NULL;
    return UI_OK;
}

/* 惰性创建页面对象与焦点组 */
static int page_lazy_create(ui_page_t *p)
{
    if (p->root) {
        return UI_OK;   /* 已创建 */
    }
    if (!p->create) {
        return UI_ERR_STATE;
    }
    /* 必须先建焦点组再创建页面对象：页面 create 内会把可聚焦控件
     * 加入 self->group，若此时组还是 NULL，LVGL 会静默丢弃导致按键全无响应 */
    p->group = lv_group_create();
    p->root = p->create(p, s_content_layer);
    if (!p->root) {
        lv_group_del(p->group);
        p->group = NULL;
        UI_LOGE("page[%s] create failed", p->name ? p->name : "?");
        return UI_ERR;
    }
    lv_obj_add_flag(p->root, LV_OBJ_FLAG_HIDDEN);
    return UI_OK;
}

static void page_show(ui_page_t *p)
{
    lv_obj_clear_flag(p->root, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(p->root);
    ui_input_set_group(p->group);   /* 把按键焦点交给本页 */
    /* 首次进入时自动聚焦组内第一个控件，保证首按 OK 即可操作；
     * 再次进入（pop 回来）保留之前的焦点位置 */
    if (p->group && p->group->obj_focus == NULL) {
        lv_group_focus_next(p->group);
    }
    if (p->on_show) {
        p->on_show(p);
    }
}

static void page_hide(ui_page_t *p)
{
    if (p->on_hide) {
        p->on_hide(p);
    }
    if (p->root) {
        lv_obj_add_flag(p->root, LV_OBJ_FLAG_HIDDEN);
    }
}

int ui_page_push(ui_page_id_t id)
{
    ui_page_t *p;

    if (id >= UI_PAGE_MAX || !s_registry[id]) {
        return UI_ERR_PARAM;
    }
    if (s_sp >= UI_PAGE_STACK_DEPTH) {
        UI_LOGW("page stack full");
        return UI_ERR_STATE;
    }

    p = s_registry[id];
    if (page_lazy_create(p) != UI_OK) {
        return UI_ERR;
    }

    if (s_sp > 0) {
        page_hide(s_stack[s_sp - 1]);
    }
    s_stack[s_sp++] = p;
    page_show(p);
    UI_LOGD("push page[%s], sp=%d", p->name, s_sp);
    return UI_OK;
}

int ui_page_pop(void)
{
    ui_page_t *top;

    if (s_sp <= 1) {
        return UI_ERR_STATE;   /* 栈底不出栈 */
    }
    top = s_stack[--s_sp];
    page_hide(top);
    page_show(s_stack[s_sp - 1]);
    UI_LOGD("pop page[%s], sp=%d", top->name, s_sp);
    return UI_OK;
}

int ui_page_replace(ui_page_id_t id)
{
    ui_page_t *p;

    if (id >= UI_PAGE_MAX || !s_registry[id]) {
        return UI_ERR_PARAM;
    }
    p = s_registry[id];
    if (page_lazy_create(p) != UI_OK) {
        return UI_ERR;
    }

    if (s_sp > 0) {
        page_hide(s_stack[s_sp - 1]);
        s_stack[s_sp - 1] = p;
    } else {
        s_stack[s_sp++] = p;
    }
    page_show(p);
    UI_LOGD("replace -> page[%s], sp=%d", p->name, s_sp);
    return UI_OK;
}

ui_page_t *ui_page_top(void)
{
    return (s_sp > 0) ? s_stack[s_sp - 1] : NULL;
}

int ui_page_dispatch(const ui_event_t *evt)
{
    ui_page_t *top = ui_page_top();
    if (top && top->on_event) {
        return top->on_event(top, evt);
    }
    return 0;
}
