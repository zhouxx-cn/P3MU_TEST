/**
 * @file    ui_video.h
 * @brief   视频预览模块：接收 IPC 240x320 子码流，作为 UI 最底层背景。
 *
 * @details
 *  对外核心接口 ui_video_refresh(yuv_buf, ...)：由 IPC 子码流线程调用，推送一帧。
 *  本模块内部：
 *    - 双缓冲保存最近一帧 RGB565（UI 线程读、推流线程写，带锁）
 *    - 通过消息队列通知 UI 线程"有新帧"，UI 线程再把帧贴到 canvas
 *    - YUV→RGB 转换通过可替换钩子实现：样机用软转；真机可换海思 VGS/IVE 硬转
 *    - 预留 VDEC/VENC 对接接口，解码/编码逻辑不写死在 UI，方便移植
 *
 *  线程约定：
 *    - ui_video_refresh()           : 任意推流线程调用（线程安全）
 *    - ui_video_init/on_frame_ready : 仅 UI 线程调用
 */
#ifndef __UI_VIDEO_H__
#define __UI_VIDEO_H__

#include "ui_common.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief 输入帧像素格式（预留多格式，样机常用 NV12/NV21/YUYV）。 */
typedef enum {
    UI_PIX_NV12 = 0,   /**< YUV420SP, Y plane + 交织 UV */
    UI_PIX_NV21,       /**< YUV420SP, Y plane + 交织 VU */
    UI_PIX_YUYV,       /**< YUV422 packed */
    UI_PIX_RGB565,     /**< 已是 RGB565，直接拷贝 */
} ui_pix_fmt_t;

/**
 * @brief YUV→RGB565 转换钩子原型。
 * @param fmt     源格式
 * @param src     源数据（YUV）
 * @param src_len 源字节数
 * @param w,h     宽高
 * @param dst565  目标 RGB565 缓冲（容量 >= w*h*2）
 * @return 0 成功
 * @note  真机可注册海思硬件转换实现替换默认软转。
 */
typedef int (*ui_yuv2rgb_fn)(ui_pix_fmt_t fmt, const uint8_t *src, uint32_t src_len,
                             uint16_t w, uint16_t h, uint16_t *dst565);

/**
 * @brief VDEC/VENC 对接钩子集合（预留，不绑定海思 SDK）。
 * @details 样机阶段可全部为 NULL；真机移植时实现并通过 ui_video_set_codec 注入。
 */
typedef struct {
    int (*vdec_open)(void *cfg);                 /**< 打开解码器 */
    int (*vdec_close)(void);                     /**< 关闭解码器 */
    int (*venc_start_record)(const char *path);  /**< 开始录像(主码流) */
    int (*venc_stop_record)(void);               /**< 停止录像 */
    int (*venc_snap)(const char *path);          /**< 抓拍一张(主码流) */
} ui_video_codec_ops_t;

/**
 * @brief 初始化视频预览画布。
 * @param parent 视频底层父对象（由 ui_main 提供 lv_layer_bottom）
 * @return UI_OK / UI_ERR
 * @note  仅 UI 线程调用。
 */
int ui_video_init(lv_obj_t *parent);

/**
 * @brief 反初始化，释放缓冲。
 */
void ui_video_deinit(void);

/**
 * @brief 注册自定义 YUV→RGB 转换实现（可选，默认内置软转）。
 * @param fn 转换函数，传 NULL 恢复默认
 */
void ui_video_set_yuv2rgb(ui_yuv2rgb_fn fn);

/**
 * @brief 注册 VDEC/VENC 对接实现（可选，预留给真机）。
 * @param ops 钩子集合（浅拷贝保存指针内容）
 */
void ui_video_set_codec(const ui_video_codec_ops_t *ops);

/**
 * @brief 【对外主接口】推送一帧子码流（由 IPC 推流线程调用，线程安全）。
 * @param fmt     像素格式
 * @param buf     帧数据
 * @param len     字节数
 * @param w,h     分辨率（应为 240x320）
 * @param seq     帧序号（用于统计丢帧）
 * @return UI_OK 已接收；UI_ERR 丢弃（缓冲忙）
 * @note  本函数内部完成 YUV→RGB 转换并存入后台缓冲，然后 post 一个
 *        UI_EVT_VIDEO_FRAME 事件通知 UI 线程贴图。不直接操作 LVGL。
 */
int ui_video_refresh(ui_pix_fmt_t fmt, const uint8_t *buf, uint32_t len,
                     uint16_t w, uint16_t h, uint32_t seq);

/**
 * @brief UI 线程收到 UI_EVT_VIDEO_FRAME 后调用：把最近帧贴到 canvas。
 * @param info 视频事件负载
 * @note  仅 UI 线程调用（由 ui_main 主循环驱动）。
 */
void ui_video_on_frame_ready(const ui_evt_video_t *info);

/**
 * @brief 显示/隐藏预览画布（如进入设置页可隐藏以省功耗，可选）。
 * @param show 1 显示 0 隐藏
 */
void ui_video_set_visible(int show);

#ifdef __cplusplus
}
#endif

#endif /* __UI_VIDEO_H__ */
