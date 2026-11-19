/**
 * @file    ui_video.c
 * @brief   视频预览画布实现：双缓冲 + 软 YUV→RGB + LVGL canvas 贴图。
 *
 * @details
 *  推流线程: ui_video_refresh() -> YUV2RGB -> 写"后台缓冲" -> post 事件
 *  UI 线程 : ui_video_on_frame_ready() -> 把后台缓冲拷到 canvas 缓冲 -> 失效重绘
 *
 *  只有 UI 线程碰 LVGL（canvas）。两线程通过 s_lock 保护帧缓冲交换。
 */
#include "ui_video.h"
#include "ui_msgq.h"
#include <pthread.h>
#include <stdlib.h>

/* canvas 缓冲：UI 线程独占，LVGL 直接显示它 */
static lv_obj_t     *s_canvas = NULL;
static lv_color_t   *s_canvas_buf = NULL;     /* w*h 个 RGB565 */

/* 后台帧缓冲：推流线程写，UI 线程读，交换由锁保护 */
static uint16_t     *s_back_buf = NULL;       /* w*h 个 RGB565 */
static pthread_mutex_t s_lock = PTHREAD_MUTEX_INITIALIZER;
static int           s_back_ready = 0;        /* 后台是否有未取用的新帧 */

static uint16_t      s_w = UI_PREVIEW_W;
static uint16_t      s_h = UI_PREVIEW_H;
static uint32_t      s_last_seq = 0;
static uint32_t      s_drop_cnt = 0;

static ui_yuv2rgb_fn s_yuv2rgb = NULL;
static ui_video_codec_ops_t s_codec = {0};

/* ============ 默认软件 YUV→RGB565 转换（可被 set_yuv2rgb 替换） ============ */
static inline uint16_t rgb888_to_565(int r, int g, int b)
{
    r = UI_CLAMP(r, 0, 255);
    g = UI_CLAMP(g, 0, 255);
    b = UI_CLAMP(b, 0, 255);
    return (uint16_t)(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}

/* NV12/NV21 -> RGB565 */
static void nv12_to_rgb565(const uint8_t *src, uint16_t w, uint16_t h,
                           uint16_t *dst, int is_nv21)
{
    const uint8_t *y_plane = src;
    const uint8_t *uv_plane = src + (uint32_t)w * h;
    int x, y;
    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            int Y = y_plane[y * w + x];
            int uv_index = (y / 2) * w + (x & ~1);
            int U, V;
            if (is_nv21) {
                V = uv_plane[uv_index] - 128;
                U = uv_plane[uv_index + 1] - 128;
            } else {
                U = uv_plane[uv_index] - 128;
                V = uv_plane[uv_index + 1] - 128;
            }
            int C = Y - 16;
            int R = (298 * C + 409 * V + 128) >> 8;
            int G = (298 * C - 100 * U - 208 * V + 128) >> 8;
            int B = (298 * C + 516 * U + 128) >> 8;
            dst[y * w + x] = rgb888_to_565(R, G, B);
        }
    }
}

/* YUYV(YUV422 packed) -> RGB565 */
static void yuyv_to_rgb565(const uint8_t *src, uint16_t w, uint16_t h, uint16_t *dst)
{
    int i, total = w * h;
    for (i = 0; i < total; i += 2) {
        int Y0 = src[0], U = src[1] - 128, Y1 = src[2], V = src[3] - 128;
        int C0 = Y0 - 16, C1 = Y1 - 16;
        dst[i]     = rgb888_to_565((298*C0 + 409*V + 128) >> 8,
                                   (298*C0 - 100*U - 208*V + 128) >> 8,
                                   (298*C0 + 516*U + 128) >> 8);
        dst[i + 1] = rgb888_to_565((298*C1 + 409*V + 128) >> 8,
                                   (298*C1 - 100*U - 208*V + 128) >> 8,
                                   (298*C1 + 516*U + 128) >> 8);
        src += 4;
    }
}

static int default_yuv2rgb(ui_pix_fmt_t fmt, const uint8_t *src, uint32_t src_len,
                           uint16_t w, uint16_t h, uint16_t *dst565)
{
    UI_UNUSED(src_len);
    switch (fmt) {
    case UI_PIX_NV12:   nv12_to_rgb565(src, w, h, dst565, 0); break;
    case UI_PIX_NV21:   nv12_to_rgb565(src, w, h, dst565, 1); break;
    case UI_PIX_YUYV:   yuyv_to_rgb565(src, w, h, dst565);    break;
    case UI_PIX_RGB565: memcpy(dst565, src, (uint32_t)w * h * 2); break;
    default: return UI_ERR;
    }
    return UI_OK;
}

/* ============ 初始化 / 反初始化 ============ */
int ui_video_init(lv_obj_t *parent)
{
    if (!parent) {
        return UI_ERR_PARAM;
    }
    s_w = UI_PREVIEW_W;
    s_h = UI_PREVIEW_H;

    s_canvas_buf = (lv_color_t *)malloc((size_t)s_w * s_h * sizeof(lv_color_t));
    s_back_buf   = (uint16_t *)malloc((size_t)s_w * s_h * sizeof(uint16_t));
    if (!s_canvas_buf || !s_back_buf) {
        UI_LOGE("video buf alloc failed");
        ui_video_deinit();
        return UI_ERR_NOMEM;
    }
    memset(s_canvas_buf, 0, (size_t)s_w * s_h * sizeof(lv_color_t));
    memset(s_back_buf, 0, (size_t)s_w * s_h * sizeof(uint16_t));

    s_canvas = lv_canvas_create(parent);
    lv_canvas_set_buffer(s_canvas, s_canvas_buf, s_w, s_h, LV_IMG_CF_TRUE_COLOR);
    lv_obj_align(s_canvas, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_canvas_fill_bg(s_canvas, lv_color_black(), LV_OPA_COVER);

    s_yuv2rgb = default_yuv2rgb;
    UI_LOGI("ui_video init %dx%d", s_w, s_h);
    return UI_OK;
}

void ui_video_deinit(void)
{
    pthread_mutex_lock(&s_lock);
    if (s_canvas) {
        lv_obj_del(s_canvas);
        s_canvas = NULL;
    }
    free(s_canvas_buf); s_canvas_buf = NULL;
    free(s_back_buf);   s_back_buf = NULL;
    s_back_ready = 0;
    pthread_mutex_unlock(&s_lock);
}

void ui_video_set_yuv2rgb(ui_yuv2rgb_fn fn)
{
    s_yuv2rgb = fn ? fn : default_yuv2rgb;
}

void ui_video_set_codec(const ui_video_codec_ops_t *ops)
{
    if (ops) {
        s_codec = *ops;
    } else {
        memset(&s_codec, 0, sizeof(s_codec));
    }
}

/* ============ 推流线程入口 ============ */
int ui_video_refresh(ui_pix_fmt_t fmt, const uint8_t *buf, uint32_t len,
                     uint16_t w, uint16_t h, uint32_t seq)
{
    ui_event_t evt;

    if (!buf || !s_back_buf) {
        return UI_ERR_PARAM;
    }
    if (w != s_w || h != s_h) {
        /* 样机只接受 240x320 子码流；真机如需缩放在此接缩放钩子 */
        UI_LOGW("frame %ux%u != %ux%u, drop", w, h, s_w, s_h);
        return UI_ERR_PARAM;
    }

    /* 若上一帧 UI 还没取走，直接丢弃本帧，保证低延迟不堆积 */
    if (pthread_mutex_trylock(&s_lock) != 0) {
        s_drop_cnt++;
        return UI_ERR;
    }
    if (s_back_ready) {
        /* UI 尚未消费，覆盖为最新帧即可（丢旧帧） */
        s_drop_cnt++;
    }
    if (s_yuv2rgb(fmt, buf, len, w, h, s_back_buf) != UI_OK) {
        pthread_mutex_unlock(&s_lock);
        return UI_ERR;
    }
    s_back_ready = 1;
    s_last_seq = seq;
    pthread_mutex_unlock(&s_lock);

    /* 通知 UI 线程贴图（队列满会自动丢弃视频帧事件） */
    evt.type = UI_EVT_VIDEO_FRAME;
    evt.u.video.width  = w;
    evt.u.video.height = h;
    evt.u.video.seq    = seq;
    ui_msgq_post(&evt);
    return UI_OK;
}

/* ============ UI 线程贴图 ============ */
void ui_video_on_frame_ready(const ui_evt_video_t *info)
{
    UI_UNUSED(info);
    if (!s_canvas || !s_canvas_buf || !s_back_buf) {
        return;
    }

    pthread_mutex_lock(&s_lock);
    if (s_back_ready) {
        memcpy(s_canvas_buf, s_back_buf, (size_t)s_w * s_h * sizeof(uint16_t));
        s_back_ready = 0;
    }
    pthread_mutex_unlock(&s_lock);

    lv_obj_invalidate(s_canvas);   /* 触发 LVGL 重绘该区域 */
}

void ui_video_set_visible(int show)
{
    if (!s_canvas) {
        return;
    }
    if (show) {
        lv_obj_clear_flag(s_canvas, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(s_canvas, LV_OBJ_FLAG_HIDDEN);
    }
}
