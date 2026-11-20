/**
 * @file    thumbcam_demo.c
 * @brief   拇指相机 UI 接入 demo：IPC 子码流(H264) -> openh264 软解码(YUV) -> UI 框架 -> LCD。
 *
 * @details
 *  - 取流：参考录像写卡模式，经 CIU_GetSubStreamRecordInterface() 拿到循环缓冲区
 *          消费接口，ifRecordUserAdd 添加消费者后，循环 ifRecordGetData 取子码流
 *          H264 裸流(SDK 内部已剥离 28 字节私有帧头)，用完 ifRecordReleaseData 释放。
 *  - 解码：openh264 (ISVCDecoder::DecodeFrame2) 把 H264 解成 I420(YUV420P)。
 *  - 送 UI：把 I420 重排为 NV12 后调用 ui_video_refresh()，交给 ui 工程师的 UI 框架;
 *          UI 线程把"视频底层 + 菜单浮层"合成整帧 240x320 RGB565, 经 lcd_flush 回调
 *          Lcd_SendFrame() 送 LCD。
 *
 *  屏幕/子码流均为 240x320 竖屏(见 UI_PREVIEW_W/UI_PREVIEW_H)。
 */
#include "thumbcam_demo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <limits.h> /* UCHAR_MAX */

/* ui 工程师的 UI 框架 */
#include "ui_main.h"
#include "ui_video.h"
#include "ui_input.h"

/* 子码流循环缓冲区取流接口(SDK) */
#include "MemRecord.h"       /* RecordInterface */
#include "MPU_CIU_Preview.h" /* CIU_GetSubStreamRecordInterface */

/* openh264 解码器 */
#include "codec_api.h"
#include "codec_def.h"

/* 海思 baseline 底层 LCD */
#include "lcd.h"

/* ======================= 物理按键(模拟 IPC 三键) =======================
 * S1 呼叫键 / S2 挂断键 / S3 复位键。为避免引入过重的 Hi_MPP.h(会级联
 * vi/vpss/venc 等平台头), 这里按 zwsdk 既有 C 接口自行声明读键函数。
 * 语义(zwsdk defs.h KEY_STATUS_E): 返回 0 成功; 出参 0=按下(KEY_PUSH) 1=松开。
 * 两接口为"电平读取"(非锁存), 轮询不会与一键呼叫业务争抢事件。 */
extern int Hi_MPP_GetCallKeyStatus(int *pCallKeySt, int *pHangupKeySt);
extern int Hi_MPP_GetRestKeyStatus(int *pResetKeySt);

#define TC_KEY_PRESSED   0   /* KEY_PUSH: 0=按下 */

#define TC_W (UI_PREVIEW_W)                /* 240 */
#define TC_H (UI_PREVIEW_H)                /* 320 */
#define TC_NV12_SIZE (TC_W * TC_H * 3 / 2) /* 115200 */

static volatile int s_run = 0;
static pthread_t s_stream_tid;
static ISVCDecoder *s_decoder = NULL;

/* NV12 中转缓冲(推流线程独占) */
static unsigned char s_nv12[TC_NV12_SIZE];

/* ======================= 1) LCD 整帧送显回调 ======================= */
/* UI 框架把合成好的整帧 240x320 RGB565 交给本回调, 直接送底层 LCD。 */
static int tc_lcd_flush(uint8_t *pData, uint32_t size)
{
    Lcd_SendFrame(0, (UINT8 *)pData, size);
    return 0;
}

/* ======================= 2) openh264 解码器 ======================= */
static int tc_decoder_open(void)
{
    SDecodingParam decParam;

    if (WelsCreateDecoder(&s_decoder) != 0 || NULL == s_decoder)
    {
        UI_LOGE("WelsCreateDecoder failed");
        s_decoder = NULL;
        return -1;
    }

    memset(&decParam, 0, sizeof(decParam));
    decParam.uiTargetDqLayer = (unsigned char)UCHAR_MAX;        /* 解码所有层 */
    decParam.eEcActiveIdc = ERROR_CON_SLICE_COPY;               /* 错误隐藏策略 */
    decParam.sVideoProperty.eVideoBsType = VIDEO_BITSTREAM_AVC; /* H.264 裸流 */

    if ((*s_decoder)->Initialize(s_decoder, &decParam) != 0)
    {
        UI_LOGE("openh264 Initialize failed");
        WelsDestroyDecoder(s_decoder);
        s_decoder = NULL;
        return -1;
    }
    return 0;
}

static void tc_decoder_close(void)
{
    if (NULL != s_decoder)
    {
        (*s_decoder)->Uninitialize(s_decoder);
        WelsDestroyDecoder(s_decoder);
        s_decoder = NULL;
    }
}

/* ======================= 3) I420(YUV420P) -> NV12 ======================= */
/* openh264 输出为带 stride 的 I420(Y/U/V 三平面); UI 框架需要紧凑 NV12。 */
static void tc_i420_to_nv12(unsigned char *yuv[3], const int stride[3], int w, int h, unsigned char *nv12)
{
    unsigned char *dst_y = nv12;
    unsigned char *dst_uv = nv12 + w * h;
    const unsigned char *src_y = yuv[0];
    const unsigned char *src_u = yuv[1];
    const unsigned char *src_v = yuv[2];
    int ys = stride[0], us = stride[1], vs = stride[2];
    int i, j;

    for (i = 0; i < h; i++)
    {
        memcpy(dst_y + i * w, src_y + i * ys, w);
    }
    for (i = 0; i < h / 2; i++)
    {
        for (j = 0; j < w / 2; j++)
        {
            dst_uv[i * w + 2 * j] = src_u[i * us + j];
            dst_uv[i * w + 2 * j + 1] = src_v[i * vs + j];
        }
    }
}

/* ======================= 4) 子码流取流 + 解码 + 送显线程 ======================= */
static void *tc_stream_thread(void *arg)
{
    RecordInterface tIf;
    unsigned int conn = 0;
    unsigned int seq = 0;

    (void)arg;

    memset(&tIf, 0, sizeof(tIf));
    if (0 != CIU_GetSubStreamRecordInterface(&tIf) || NULL == tIf.ifRecordUserAdd || NULL == tIf.ifRecordGetData ||
        NULL == tIf.ifRecordReleaseData || NULL == tIf.ifRecordUserDelete)
    {
        UI_LOGE("get sub-stream record interface failed");
        return NULL;
    }

    /* 添加循环缓冲区消费者(通道0, 0xff=所有码流, SDK 内部仅回子码流视频帧) */
    if (0 != tIf.ifRecordUserAdd(0, 0xff, &conn))
    {
        UI_LOGE("add circlequeue user failed");
        return NULL;
    }

    UI_LOGI("thumbcam stream thread running");

    while (s_run)
    {
        char *pStream = NULL;
        unsigned int nLen = 0;
        int r = tIf.ifRecordGetData(conn, &pStream, &nLen);

        if (0 != r)
        {
            /* -2:无数据  -3:非子码流帧(已释放)  -1:异常 */
            if (-2 == r)
            {
                usleep(10 * 1000);
            }
            continue;
        }

        /* H264 -> I420 */
        if (NULL != s_decoder && NULL != pStream && nLen > 0)
        {
            unsigned char *pDst[3] = {NULL, NULL, NULL};
            SBufferInfo sDstInfo;
            DECODING_STATE st;

            memset(&sDstInfo, 0, sizeof(sDstInfo));
            st = (*s_decoder)->DecodeFrame2(s_decoder, (const unsigned char *)pStream, (int)nLen, pDst, &sDstInfo);

            if (0 == st && 1 == sDstInfo.iBufferStatus && NULL != pDst[0] && NULL != pDst[1] && NULL != pDst[2])
            {
                int w = sDstInfo.UsrData.sSystemBuffer.iWidth;
                int h = sDstInfo.UsrData.sSystemBuffer.iHeight;

                if (TC_W == w && TC_H == h)
                {
                    int stride[3];
                    stride[0] = sDstInfo.UsrData.sSystemBuffer.iStride[0];
                    stride[1] = sDstInfo.UsrData.sSystemBuffer.iStride[1];
                    stride[2] = sDstInfo.UsrData.sSystemBuffer.iStride[1];

                    tc_i420_to_nv12(pDst, stride, w, h, s_nv12);

                    /* 推给 UI 框架(线程安全), UI 线程合成后送 LCD */
                    ui_video_refresh(UI_PIX_NV12, s_nv12, TC_NV12_SIZE, (uint16_t)TC_W, (uint16_t)TC_H, seq++);
                }
            }
        }

        tIf.ifRecordReleaseData(conn, pStream, nLen);
    }

    tIf.ifRecordUserDelete(conn);
    UI_LOGI("thumbcam stream thread exit");
    return NULL;
}

/* ======================= 5) 物理按键扫描(UI 框架弱函数强实现) =======================
 * 用户定义: S1/S2 做选择跳转(上/下), S3 做确认(OK)。
 * 返回"当前按下"的单个逻辑键; 消抖与按下/抬起边沿由 ui_input_key.c 扫描线程统一处理。
 * 本强实现覆盖 libui.a 中 ui_input_key.c 的 __attribute__((weak)) ui_key_scan_hw。 */
ui_key_code_t ui_key_scan_hw(void)
{
    int call = 1, hangup = 1, reset = 1; /* 1=松开 */

    /* S3 复位键 -> 确认(OK), 优先级最高 */
    if (0 == Hi_MPP_GetRestKeyStatus(&reset) && TC_KEY_PRESSED == reset)
    {
        return UI_KEY_OK;
    }

    /* S1 呼叫键 -> 上(UP); S2 挂断键 -> 下(DOWN) */
    if (0 == Hi_MPP_GetCallKeyStatus(&call, &hangup))
    {
        if (TC_KEY_PRESSED == call)
        {
            return UI_KEY_UP;
        }
        if (TC_KEY_PRESSED == hangup)
        {
            return UI_KEY_DOWN;
        }
    }

    return UI_KEY_NONE;
}

/* ======================= 对外入口 ======================= */
int ThumbCamDemo_Start(void)
{
    ui_main_cfg_t cfg;

    if (s_run)
    {
        return 0; /* 已启动 */
    }

    /* 1)  创建 openh264 解码器 */
    if (0 != tc_decoder_open())
    {
        return -1;
    }

    /* 2) 初始化并启动 UI(LVGL 全部在内部 UI 线程) */
    memset(&cfg, 0, sizeof(cfg));
    cfg.lcd_flush = tc_lcd_flush;
    if (UI_OK != ui_main_init(&cfg))
    {
        UI_LOGE("ui_main_init failed");
        tc_decoder_close();
        return -1;
    }
    ui_main_start();

    /* 3) 启动物理按键源: 本文件强实现 ui_key_scan_hw() 读三键
     *    S1->上(UP)  S2->下(DOWN)  S3->确认(OK); 扫描线程负责消抖与边沿 */
    ui_input_key_start();

    /* 4) 启动子码流取流 + 解码 + 送显线程 */
    s_run = 1;
    if (0 != pthread_create(&s_stream_tid, NULL, tc_stream_thread, NULL))
    {
        UI_LOGE("create stream thread failed");
        s_run = 0;
        ui_input_key_stop();
        ui_main_stop();
        tc_decoder_close();
        return -1;
    }

    UI_LOGI("ThumbCamDemo started (%dx%d sub-stream H264 -> openh264 -> UI -> LCD)", TC_W, TC_H);
    return 0;
}

void ThumbCamDemo_Stop(void)
{
    if (!s_run)
    {
        return;
    }
    s_run = 0;
    pthread_join(s_stream_tid, NULL);

    ui_input_key_stop();
    ui_main_stop();
    tc_decoder_close();
}
