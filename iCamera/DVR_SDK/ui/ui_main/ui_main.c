/**
 * @file    ui_main.c
 * @brief   UI 系统入口与 UI 线程主循环实现。
 *
 * @details
 *  三层画布（从底到顶）：
 *    - s_video_layer : 视频预览底层（ui_video 在此挂 canvas，显示子码流）
 *    - 内容层(屏幕)  : 页面栈内容（预览/菜单/设置/回放）——透明，露出视频
 *    - s_top_layer   : 状态栏 + 弹窗（始终可见，叠在最上）
 *
 *  主循环：出队事件 → 分发(输入/业务/页面) → lv_timer_handler() 渲染 → 睡眠。
 */
#include "ui_main.h"
#include "ui_msgq.h"
#include "ui_page.h"
#include "ui_input.h"
#include "ui_video.h"
#include "ui_statusbar.h"
#include "ui_theme.h"
#include "ui_res.h"
#include "ui_popup.h"
#include <pthread.h>

/* 外部页面实现（各 ui_* 模块导出注册函数） */
extern void ui_page_preview_register(void);
extern void ui_page_menu_register(void);
extern void ui_page_setting_register(void);
extern void ui_page_playback_register(void);

static pthread_t     s_ui_thread;
static volatile int  s_running = 0;
static int           s_inited  = 0;

static lv_obj_t     *s_video_layer = NULL;  /* 视频底层 */
static lv_obj_t     *s_content_layer = NULL;/* 页面内容层 */
static lv_obj_t     *s_top_layer = NULL;    /* 状态栏/弹窗顶层 */

lv_obj_t *ui_main_video_layer(void) { return s_video_layer; }
lv_obj_t *ui_main_top_layer(void)   { return s_top_layer; }

/* ---------- 构建三层画布 ----------
 * LVGL 8.3 没有 lv_layer_bottom()，底层用"活动屏幕上的专用容器"实现：
 *   - s_video_layer : 活动屏幕的第一个子对象，常驻最底（move_background）
 *   - 内容层(屏幕)  : 页面根对象挂屏幕上，显示时 move_foreground，盖在视频之上
 *   - s_top_layer   : lv_layer_top()，状态栏/弹窗，始终最上
 */
static void build_layers(void)
{
    lv_obj_t *scr = lv_scr_act();
    lv_obj_remove_style_all(scr);
    lv_obj_set_style_bg_color(scr, UI_COLOR_BG, 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

    /* 视频底层：屏幕上的专用全屏容器，置于最底 */
    s_video_layer = lv_obj_create(scr);
    lv_obj_remove_style_all(s_video_layer);
    lv_obj_set_size(s_video_layer, UI_SCREEN_W, UI_SCREEN_H);
    lv_obj_set_pos(s_video_layer, 0, 0);
    lv_obj_clear_flag(s_video_layer, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_move_background(s_video_layer);

    /* 内容层：活动屏幕本身，页面挂其上，背景透明以露出视频 */
    s_content_layer = scr;

    /* 顶层：LVGL 的 top layer，状态栏/弹窗挂这里 */
    s_top_layer = lv_layer_top();
    lv_obj_remove_style_all(s_top_layer);
    lv_obj_clear_flag(s_top_layer, LV_OBJ_FLAG_SCROLLABLE);
}

int ui_main_init(const ui_main_cfg_t *cfg)
{
    if (s_inited) {
        return UI_OK;
    }

    if (ui_msgq_init() != UI_OK) {
        UI_LOGE("msgq init failed");
        return UI_ERR;
    }

    lv_init();

    if (ui_port_disp_init(cfg ? cfg->lcd_flush : NULL) != UI_OK) {
        UI_LOGE("disp init failed");
        return UI_ERR;
    }

    ui_theme_init();
    build_layers();

    /* 输入：注册 keypad + pointer 两个 indev（来源封装在 ui_input） */
    if (ui_input_init() != UI_OK) {
        UI_LOGE("input init failed");
        return UI_ERR;
    }

    /* 视频预览底层画布 */
    if (ui_video_init(s_video_layer) != UI_OK) {
        UI_LOGE("video init failed");
        return UI_ERR;
    }

    /* 状态栏（顶层） */
    if (ui_statusbar_init(s_top_layer) != UI_OK) {
        UI_LOGE("statusbar init failed");
        return UI_ERR;
    }

    /* 弹窗层（顶层） */
    if (ui_popup_init(s_top_layer) != UI_OK) {
        UI_LOGE("popup init failed");
        return UI_ERR;
    }

    /* 页面栈 + 注册各页面 */
    ui_page_stack_init(s_content_layer);
    ui_page_preview_register();
    ui_page_menu_register();
    ui_page_setting_register();
    ui_page_playback_register();

    /* 默认进入预览页（录像模式） */
    ui_page_push(UI_PAGE_PREVIEW);

    s_inited = 1;
    UI_LOGI("ui_main init ok");
    return UI_OK;
}

/* ---------- 事件分发：业务/视频给对应模块，输入/其它给页面 ---------- */
static void handle_event(const ui_event_t *evt)
{
    switch (evt->type) {
    case UI_EVT_VIDEO_FRAME:
        ui_video_on_frame_ready(&evt->u.video);
        /* 视频帧也给页面一次机会（例如回放进度） */
        ui_page_dispatch(evt);
        break;

    case UI_EVT_BIZ:
        /* 业务状态：状态栏先吃，再给页面 */
        ui_statusbar_on_biz(&evt->u.biz);
        ui_page_dispatch(evt);
        break;

    case UI_EVT_KEY:
    case UI_EVT_POINT:
        /* 输入事件：先喂给 LVGL indev（ui_input 已在读回调里取数据），
         * 这里把语义事件也转交页面，便于页面做按键级别的自定义处理。 */
        ui_page_dispatch(evt);
        break;

    case UI_EVT_QUIT:
        s_running = 0;
        break;

    default:
        break;
    }
}

/* ---------- UI 线程主循环 ---------- */
static void *ui_thread_entry(void *arg)
{
    ui_event_t evt;
    UI_UNUSED(arg);

    UI_LOGI("ui thread start");
    while (s_running) {
        /* 1) 尽量排空事件队列（非阻塞），把输入塞进 indev、驱动页面 */
        while (ui_msgq_poll(&evt, 0) == UI_OK) {
            ui_input_feed(&evt);   /* 将 key/point 事件喂给输入抽象层缓存 */
            handle_event(&evt);
            if (!s_running) {
                break;
            }
        }

        /* 2) 让 LVGL 处理定时器/动画/重绘 */
        uint32_t idle = lv_timer_handler();
        if (idle > 30) {
            idle = 30;   /* 封顶，保证视频刷新及时 */
        }

        /* 3) 空闲时等待下一个事件或超时，降低 CPU 占用 */
        if (ui_msgq_poll(&evt, (int)idle) == UI_OK) {
            ui_input_feed(&evt);
            handle_event(&evt);
        }
    }
    UI_LOGI("ui thread exit");
    return NULL;
}

int ui_main_start(void)
{
    if (!s_inited) {
        return UI_ERR_STATE;
    }
    if (s_running) {
        return UI_OK;
    }
    s_running = 1;
    if (pthread_create(&s_ui_thread, NULL, ui_thread_entry, NULL) != 0) {
        s_running = 0;
        UI_LOGE("create ui thread failed");
        return UI_ERR;
    }
    return UI_OK;
}

void ui_main_stop(void)
{
    ui_event_t quit = { .type = UI_EVT_QUIT };
    if (!s_inited) {
        return;
    }
    if (s_running) {
        ui_msgq_post(&quit);
        pthread_join(s_ui_thread, NULL);
    }
    ui_video_deinit();
    ui_port_disp_deinit();
    ui_msgq_deinit();
    s_inited = 0;
}
