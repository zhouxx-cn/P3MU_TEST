/**
 * @file    ui_integration_example.c
 * @brief   拇指相机 UI 接入示例（参考用；真机把本文件逻辑并入 my_gui/主控启动流程）。
 *
 * @details
 *  演示四件事：
 *    1) 用 LCD 刷新回调初始化并启动 UI；
 *    2) 物理按键源接入（样机可用弱实现/注入）；
 *    3) IPC 子码流线程把 240x320 YUV 推给 ui_video_refresh；
 *    4) APP 网络线程把 JSON 指令交给 ui_input_app_on_json。
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

/* ========== 1) LCD 刷新回调：桥接到 DVR_SDK/LCD 层 ==========
 * 真机把这里替换为调用 lcd_layer 的 REFRESH_CALLBACK（签名完全一致）。
 * 例如直接把 Layer_SetRefreshCallback 注册的那个函数名填进来即可。 */
extern int lcd_hw_refresh(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                          uint8_t *data, unsigned int size); /* 由 LCD 层提供 */

/* 若暂时没有真实 LCD 回调，可用下面的占位（仅打印） */
static int demo_lcd_flush(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                          uint8_t *data, unsigned int size)
{
    (void)x; (void)y; (void)w; (void)h; (void)data; (void)size;
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

/* ========== 4) APP 网络指令线程（示例） ========== */
static void *app_cmd_thread(void *arg)
{
    (void)arg;
    /* 真机：从 P2P/socket 收到一行 JSON 后调用 ui_input_app_on_json。
     * 这里演示 3 秒后模拟点一次"菜单键"，再点屏幕中心。 */
    sleep(3);
    ui_input_app_on_json("{\"type\":\"key\",\"code\":\"back\"}");   /* 进菜单 */
    sleep(1);
    ui_input_app_on_json("{\"type\":\"click\",\"x\":120,\"y\":160}"); /* 点选项 */
    return NULL;
}

/* ========== 启动入口 ========== */
int app_ui_demo_main(void)
{
    pthread_t t_stream, t_app;

    /* 1) 初始化并启动 UI（LVGL 全在内部 UI 线程） */
    ui_main_cfg_t cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.lcd_flush = demo_lcd_flush;   /* 真机改为 lcd_hw_refresh */
    if (ui_main_init(&cfg) != UI_OK) {
        return -1;
    }
    ui_main_start();

    /* 2) 启动物理按键源（样机无实体键时，ui_key_scan_hw 弱实现返回无键，
     *    可改用 ui_input_key_inject 或纯 APP 指令驱动） */
    ui_input_key_start();

    /* 3) 启动 IPC 子码流推流线程 */
    s_stream_run = 1;
    pthread_create(&t_stream, NULL, ipc_substream_thread, NULL);

    /* 4) 启动 APP 指令线程 */
    pthread_create(&t_app, NULL, app_cmd_thread, NULL);

    /* 主线程可做其它事；此处演示阻塞运行 */
    pthread_join(t_app, NULL);
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
