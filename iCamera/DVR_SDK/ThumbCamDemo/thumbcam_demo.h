/**
 * @file    thumbcam_demo.h
 * @brief   拇指相机 UI 接入 demo 对外入口。
 *
 * @details
 *  数据流：IPC 子码流(H264,240x320) 在循环缓冲区(CircleQueue) 中
 *          --[参考录像写卡的 RecordInterface 取流]--> openh264 软解码得到 YUV(I420)
 *          --[I420 转 NV12]--> ui_video_refresh() 推给 ui 工程师的 UI 框架
 *          --> UI 线程把"视频底层+菜单浮层"合成整帧 RGB565
 *          --> lcd_flush 回调 Lcd_SendFrame() 送 240x320 LCD 显示。
 *
 *  在 mytest.c 的 main() 末尾调用 ThumbCamDemo_Start() 即可。
 */
#ifndef __THUMBCAM_DEMO_H__
#define __THUMBCAM_DEMO_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 启动拇指相机 UI demo。
 * @return 0 成功, -1 失败。
 * @note  内部会：打开 LCD、创建 openh264 解码器、初始化并启动 UI 线程、
 *        启动物理按键源、创建子码流取流+解码+送显线程。函数立即返回(非阻塞)。
 */
int ThumbCamDemo_Start(void);

/**
 * @brief 停止拇指相机 UI demo 并释放资源。
 */
void ThumbCamDemo_Stop(void);

#ifdef __cplusplus
}
#endif

#endif /* __THUMBCAM_DEMO_H__ */
