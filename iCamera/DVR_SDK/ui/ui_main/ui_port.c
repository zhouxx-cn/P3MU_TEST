/**
 * @file    ui_port.c
 * @brief   LVGL 移植层实现：display driver + tick 线程 + 整帧 RGB565 送显。
 *
 * @details
 *  整帧模式：draw_buf 为全屏 240x320，full_refresh=1，LVGL 每帧把整屏
 *  （视频底层 + 菜单浮层，已合成）渲染进一块缓冲，flush_cb 一次拿到整帧，
 *  直接交给 Lcd_SendFrame(pData, size)。
 *
 *  缓冲策略（见 UI_DISP_DOUBLE_BUF）：
 *   - 单缓冲（默认）：适用于 Lcd_SendFrame 为【同步】送屏（送完才返回）。
 *     送屏期间 LVGL 阻塞在 flush 里，不会改缓冲，无撕裂，省一半内存（150KB）。
 *   - 双缓冲：适用于 Lcd_SendFrame 为【异步 DMA】（立即返回）。把下面宏置 1，
 *     LVGL 渲染下一帧到另一块缓冲，和正在送的那块错开，防撕裂（共 300KB）。
 */
#include "ui_port.h"
#include <pthread.h>
#include <time.h>
#include <unistd.h>

/* 0=单缓冲（同步送屏）；1=双缓冲（异步 DMA 送屏，防撕裂） */
#define UI_DISP_DOUBLE_BUF 0

/* 全屏缓冲：240*320 个 RGB565 像素 = 150KB/块 */
#define UI_DISP_BUF_PIXELS (UI_SCREEN_W * UI_SCREEN_H)

static lv_disp_draw_buf_t s_draw_buf;
static lv_color_t         s_buf1[UI_DISP_BUF_PIXELS];
#if UI_DISP_DOUBLE_BUF
static lv_color_t         s_buf2[UI_DISP_BUF_PIXELS];
#endif
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
static int default_flush(uint8_t *pData, uint32_t size)
{
    UI_UNUSED(pData);
    UI_LOGD("flush whole frame size=%u (no lcd cb)", size);
    return 0;
}

/* ---------- LVGL flush_cb：整帧送到 LCD ----------
 * full_refresh=1 时，area 恒为全屏(0,0,239,319)，color_p 是整帧起始像素。
 * 直接把整帧交给 Lcd_SendFrame。 */
static void disp_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t size = (uint32_t)UI_SCREEN_W * UI_SCREEN_H * sizeof(lv_color_t); /* 150KB */
    UI_UNUSED(area);

    if (s_flush_fn) {
        s_flush_fn((uint8_t *)color_p, size);
    }

    lv_disp_flush_ready(drv);
}

int ui_port_disp_init(ui_lcd_flush_fn flush_fn)
{
    s_flush_fn = flush_fn ? flush_fn : default_flush;

    /* 整帧缓冲：单/双缓冲由 UI_DISP_DOUBLE_BUF 控制 */
#if UI_DISP_DOUBLE_BUF
    lv_disp_draw_buf_init(&s_draw_buf, s_buf1, s_buf2, UI_DISP_BUF_PIXELS);
#else
    lv_disp_draw_buf_init(&s_draw_buf, s_buf1, NULL, UI_DISP_BUF_PIXELS);
#endif

    lv_disp_drv_init(&s_disp_drv);
    s_disp_drv.draw_buf = &s_draw_buf;
    s_disp_drv.flush_cb = disp_flush_cb;
    s_disp_drv.hor_res = UI_SCREEN_W;
    s_disp_drv.ver_res = UI_SCREEN_H;
    s_disp_drv.full_refresh = 1; /* 整屏刷新：flush_cb 每次拿到一整帧 */
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

    UI_LOGI("ui_port disp init ok (%dx%d RGB565, full-frame, %s-buf)", UI_SCREEN_W, UI_SCREEN_H,
            UI_DISP_DOUBLE_BUF ? "double" : "single");
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
