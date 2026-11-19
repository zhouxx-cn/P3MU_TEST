#ifndef _HALMOD_VIDEO_H_
#define _HALMOD_VIDEO_H_

/* 定义视频用户层视频参数值(增益) */
#define VIDEO_MIN_AGC   0
#define VIDEO_DEF_AGC   128
#define VIDEO_MAX_AGC   255

/* 该定义需与hi_comm_vo.h中VO_INTF_SYNC_E保持一致 */
typedef enum
{
    VO_INTFSYNC_PAL = 0, /* PAL standard */
    VO_INTFSYNC_NTSC, /* NTSC standard */

    VO_INTFSYNC_1080P24, /* 1920 x 1080 at 24 Hz. */
    VO_INTFSYNC_1080P25, /* 1920 x 1080 at 25 Hz. */
    VO_INTFSYNC_1080P30, /* 1920 x 1080 at 30 Hz. */

    VO_INTFSYNC_720P50, /* 1280 x  720 at 50 Hz. */
    VO_INTFSYNC_720P60, /* 1280 x  720 at 60 Hz. */
    VO_INTFSYNC_1080I50, /* 1920 x 1080 at 50 Hz, interlace. */
    VO_INTFSYNC_1080I60, /* 1920 x 1080 at 60 Hz, interlace. */
    VO_INTFSYNC_1080P50, /* 1920 x 1080 at 50 Hz. */
    VO_INTFSYNC_1080P60, /* 1920 x 1080 at 60 Hz. */

    VO_INTFSYNC_576P50, /* 720  x  576 at 50 Hz. */
    VO_INTFSYNC_480P60, /* 720  x  480 at 60 Hz. */

    VO_INTFSYNC_800x600_60, /* VESA 800 x 600 at 60 Hz (non-interlaced) */
    VO_INTFSYNC_1024x768_60, /* VESA 1024 x 768 at 60 Hz (non-interlaced) */
    VO_INTFSYNC_1280x1024_60, /* VESA 1280 x 1024 at 60 Hz (non-interlaced) */
    VO_INTFSYNC_1366x768_60, /* VESA 1366 x 768 at 60 Hz (non-interlaced) */
    VO_INTFSYNC_1440x900_60, /* VESA 1440 x 900 at 60 Hz (non-interlaced) CVT Compliant */
    VO_INTFSYNC_1280x800_60, /* 1280*800@60Hz VGA@60Hz */
    VO_INTFSYNC_1600x1200_60, /* VESA 1600 x 1200 at 60 Hz (non-interlaced) */
    VO_INTFSYNC_1680x1050_60, /* VESA 1680 x 1050 at 60 Hz (non-interlaced) */
    VO_INTFSYNC_1920x1200_60, /* VESA 1920 x 1600 at 60 Hz (non-interlaced) CVT (Reduced Blanking) */
    VO_INTFSYNC_640x480_60, /* VESA 640 x 480 at 60 Hz (non-interlaced) CVT */
    VO_INTFSYNC_960H_PAL, /* ITU-R BT.1302 960 x 576 at 50 Hz (interlaced) */
    VO_INTFSYNC_960H_NTSC, /* ITU-R BT.1302 960 x 480 at 60 Hz (interlaced) */
    VO_INTFSYNC_1920x2160_30, /* 1920x2160_30 */
    VO_INTFSYNC_2560x1440_30, /* 2560x1440_30 */
    VO_INTFSYNC_2560x1440_60, /* 2560x1440_60 */
    VO_INTFSYNC_2560x1600_60, /* 2560x1600_60 */
    VO_INTFSYNC_3840x2160_24, /* 3840x2160_24 */
    VO_INTFSYNC_3840x2160_25, /* 3840x2160_25 */
    VO_INTFSYNC_3840x2160_30, /* 3840x2160_30 */
    VO_INTFSYNC_3840x2160_50, /* 3840x2160_50 */
    VO_INTFSYNC_3840x2160_60, /* 3840x2160_60 */
    VO_INTFSYNC_4096x2160_24, /* 4096x2160_24 */
    VO_INTFSYNC_4096x2160_25, /* 4096x2160_25 */
    VO_INTFSYNC_4096x2160_30, /* 4096x2160_30 */
    VO_INTFSYNC_4096x2160_50, /* 4096x2160_50 */
    VO_INTFSYNC_4096x2160_60, /* 4096x2160_60 */
    VO_INTFSYNC_320x240_60, /* For ota5182 at 60 Hz (8bit)  */
    VO_INTFSYNC_320x240_50, /* For ili9342 at 50 Hz (6bit)  */
    VO_INTFSYNC_240x320_50, /* Hi3559AV100: For ili9341 at 50 Hz (6bit) */
                            /* Hi3556AV100/Hi3519AV100: For st7789 at 50Hz(6bit) */
    VO_INTFSYNC_240x320_60, /* For ili9341 at 60 Hz (16bit) */
    VO_INTFSYNC_800x600_50, /* For LCD     at 50 Hz (24bit) */
    VO_INTFSYNC_720x1280_60, /* For MIPI DSI Tx 720 x1280 at 60 Hz */
    VO_INTFSYNC_1080x1920_60, /* For MIPI DSI Tx 1080x1920 at 60 Hz */
    VO_INTFSYNC_7680x4320_30, /* For HDMI2.1 at 30 Hz         */
    VO_INTFSYNC_USER_800x480_60, /* User timing 800x480 at 60 Hz. */
    VO_INTFSYNC_USER_1024x600_60, /* User timing 1024x600 at 60 Hz. */
    VO_INTFSYNC_USER_800x1280_60, /* User timing 800x1280 at 60 Hz. */
    VO_INTFSYNC_USER_600x1024_60, /* User timing 600x1024 at 60 Hz. */

    VO_INTFSYNC_BUTT
} VO_INTFSYNC_E;

/* VO逻辑设备 */
typedef enum
{
    VO_DEV_HDMI0 = 0, /* HMDI设备 */    
    VO_DEV_HDMI1,     /* HMDI设备 */
    VO_DEV_VGA0,      /* VGA设备 */
    VO_DEV_VGA1,      /* VGA设备 */  
    VO_DEV_CVBS0,     /* CVBS设备(复合视频) */
    VO_DEV_VIRT0,     /* 虚拟设备 */
    VO_DEV_DVI0,      /* DVI设备 */
    VO_DEV_RGB0,      /* RGB设备 */
    VO_DEV_YPBPR0,    /* YPbPr设备(色差端子) */
    VO_DEV_SVIDEO0,   /* S-Video设备(S端子也叫分量视频) */
    VO_DEV_MAX
} VO_DEV_E;
#define MAX_VO_DEV_NUM  6

#endif /* _HALMOD_VIDEO_H_ */
