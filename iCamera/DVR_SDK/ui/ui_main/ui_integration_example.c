/**
 * @file    ui_integration_example.c
 * @brief   拇指相机 UI 接入示例（参考用；真机把本文件逻辑并入 my_gui/主控启动流程）。
 *
 * @details
 *  演示四件事：
 *    1) 用 LCD 刷新回调初始化并启动 UI；
 *    2) 物理按键源接入（样机可用弱实现/注入）；
 *    3) IPC 子码流线程把 240x320 YUV 推给 ui_video_refresh；
 *    4) 触摸屏接入（真机预留，样机无触摸硬件不启动）。
 *
 *  编译：本文件仅作示例，默认不随库编译（无 makefile 收录）。如需本地验证，
 *  可把它加入 my_gui 的源目录，或单独写 main 调用 app_ui_demo_main()。
 */
#include "ui_main.h"
#include "ui_input.h"
#include "ui_video.h"
#include "ui_menu.h"     /* ui_cam_set_ops 等（如需注入真机动作） */
#include <pthread.h>
#include <unistd.h>
#include <string.h>

/* ========== 1) LCD 整帧送显回调：桥接到 IPC 模拟机底层 ==========
 * 真机底层提供 Lcd_SendFrame(UINT8 *pData, UINT32 Size)，只接受一整张
 * 240x320 RGB565。若其返回 void，用下面的一行适配函数包一下即可。 */
extern void Lcd_SendFrame(uint8_t *pData, uint32_t Size);   /* 由底层屏幕驱动提供 */

static int demo_lcd_flush(uint8_t *pData, uint32_t size)
{
    /* 真机：Lcd_SendFrame(pData, size);  —— 整帧(视频+菜单已合成)直接送屏 */
    (void)pData; (void)size;
    return 0;
}

/* ========== 3) IPC 子码流推流线程（示例） ========== */
static volatile int s_stream_run = 0;

static void *ipc_substream_thread(void *arg)
{
    (void)arg;
    static uint8_t nv12[UI_PREVIEW_W * UI_PREVIEW_H * 3 / 2];
    uint32_t seq = 0;

    while (s_stream_run) {
        /* 真机：从海思 VDEC/VPSS 取一帧 240x320 子码流 YUV 到 nv12。
         * 这里用渐变填充模拟一帧。 */
        memset(nv12, (seq & 0xFF), sizeof(nv12));

        ui_video_refresh(UI_PIX_NV12, nv12, sizeof(nv12),
                         UI_PREVIEW_W, UI_PREVIEW_H, seq++);
        usleep(33 * 1000);   /* ~30fps */
    }
    return NULL;
}

/* ========== 4) 触摸屏线程（真机预留示例，样机无触摸硬件） ========== */
static void *touch_thread(void *arg)
{
    (void)arg;
    /* 真机触摸屏：读触摸 IC 得到坐标/按下态后，经 ui_msgq 投递 UI_EVT_POINT，
     * ui_input_feed() 会自动喂给 pointer indev，页面零改动。
     * 样机阶段不启动本线程，仅保留按键输入。 */
    return NULL;
}

/* ========== 启动入口 ========== */
int app_ui_demo_main(void)
{
    pthread_t t_stream;

    /* 1) 初始化并启动 UI（LVGL 全在内部 UI 线程） */
    ui_main_cfg_t cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.lcd_flush = demo_lcd_flush;   /* 真机：在 demo_lcd_flush 里调 Lcd_SendFrame */
    if (ui_main_init(&cfg) != UI_OK) {
        return -1;
    }
    ui_main_start();

    /* 2) 启动物理按键源（样机无实体键时，ui_key_scan_hw 弱实现返回无键，
     *    可改用 ui_input_key_inject 驱动测试） */
    ui_input_key_start();

    /* 3) 启动 IPC 子码流推流线程 */
    s_stream_run = 1;
    pthread_create(&t_stream, NULL, ipc_substream_thread, NULL);

    /* 4) 触摸屏线程：样机无触摸硬件不启动（真机见 touch_thread 注释） */

    /* 主线程可做其它事；此处演示阻塞运行 */
    for (;;) {
        sleep(1);
    }

    /* 退出清理（示例不可达） */
    s_stream_run = 0;
    pthread_join(t_stream, NULL);
    ui_input_key_stop();
    ui_main_stop();
    return 0;
}
