/**
 * @file    ui_port.c
 * @brief   LVGL 移植层实现：display driver + tick 线程 + RGB565 flush。
 */
#include "ui_port.h"
#include <pthread.h>
#include <time.h>
#include <unistd.h>

/* 显示缓冲：240 宽 * 40 行双缓冲，RGB565。约 240*40*2 = 18.75KB/块。
 * 对 240x320 小屏，部分刷新(partial)足够流畅，又省内存。 */
#define UI_DISP_BUF_LINES   40
#define UI_DISP_BUF_PIXELS  (UI_SCREEN_W * UI_DISP_BUF_LINES)

static lv_disp_draw_buf_t s_draw_buf;
static lv_color_t         s_buf1[UI_DISP_BUF_PIXELS];
static lv_color_t         s_buf2[UI_DISP_BUF_PIXELS];
static lv_disp_drv_t      s_disp_drv;

static ui_lcd_flush_fn    s_flush_fn = NULL;

static pthread_t          s_tick_thread;
static volatile int       s_tick_run = 0;

/* ---------- tick 线程：每 5ms 调一次 lv_tick_inc(5) ---------- */
static void *tick_thread_entry(void *arg)
{
    UI_UNUSED(arg);
    struct timespec ts = { 0, 5 * 1000 * 1000 }; /* 5ms */
    while (s_tick_run) {
        nanosleep(&ts, NULL);
        lv_tick_inc(5);
    }
    return NULL;
}

/* ---------- 弱 flush：未提供 LCD 回调时的占位（仅调试）---------- */
static int default_flush(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                         uint8_t *data, unsigned int size)
{
    UI_UNUSED(data);
    UI_LOGD("flush (%u,%u) %ux%u size=%u (no lcd cb)", x, y, w, h, size);
    return 0;
}

/* ---------- LVGL flush_cb：把渲染区域送到 LCD ---------- */
static void disp_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p)
{
    uint16_t x = (uint16_t)area->x1;
    uint16_t y = (uint16_t)area->y1;
    uint16_t w = (uint16_t)(area->x2 - area->x1 + 1);
    uint16_t h = (uint16_t)(area->y2 - area->y1 + 1);
    unsigned int size = (unsigned int)w * h * sizeof(lv_color_t); /* RGB565 => 2B */

    if (s_flush_fn) {
        s_flush_fn(x, y, w, h, (uint8_t *)color_p, size);
    }

    lv_disp_flush_ready(drv);
}

int ui_port_disp_init(ui_lcd_flush_fn flush_fn)
{
    s_flush_fn = flush_fn ? flush_fn : default_flush;

    lv_disp_draw_buf_init(&s_draw_buf, s_buf1, s_buf2, UI_DISP_BUF_PIXELS);

    lv_disp_drv_init(&s_disp_drv);
    s_disp_drv.draw_buf   = &s_draw_buf;
    s_disp_drv.flush_cb   = disp_flush_cb;
    s_disp_drv.hor_res    = UI_SCREEN_W;
    s_disp_drv.ver_res    = UI_SCREEN_H;
    s_disp_drv.full_refresh = 0;   /* 部分刷新，省内存 */
    if (lv_disp_drv_register(&s_disp_drv) == NULL) {
        UI_LOGE("lv_disp_drv_register failed");
        return UI_ERR;
    }

    /* 启动 tick 线程 */
    s_tick_run = 1;
    if (pthread_create(&s_tick_thread, NULL, tick_thread_entry, NULL) != 0) {
        UI_LOGE("create tick thread failed");
        s_tick_run = 0;
        return UI_ERR;
    }

    UI_LOGI("ui_port disp init ok (%dx%d RGB565)", UI_SCREEN_W, UI_SCREEN_H);
    return UI_OK;
}

void ui_port_disp_deinit(void)
{
    if (s_tick_run) {
        s_tick_run = 0;
        pthread_join(s_tick_thread, NULL);
    }
}

uint32_t ui_port_tick_get(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint32_t)(ts.tv_sec * 1000UL + ts.tv_nsec / 1000000UL);
}

void ui_port_msleep(uint32_t ms)
{
    struct timespec ts;
    ts.tv_sec  = ms / 1000;
    ts.tv_nsec = (long)(ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}
