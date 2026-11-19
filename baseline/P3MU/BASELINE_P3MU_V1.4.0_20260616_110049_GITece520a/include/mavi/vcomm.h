#ifndef _VIDEO_COMM_H_
#define _VIDEO_COMM_H_

#ifdef __cplusplus
extern "C" {
#endif  //@__cplusplus
#include "vfont.h"
// #include "halmod_types.h"
#include "halmod_video.h"

#define ALL_VODEV (-1) /* 表示全部VO设备 */

#define MAX_MD_COL 22 /* 最大移动侦测区域行数 */
#define MAX_MD_ROW 18 /* 最大移动侦测区域横数 */

#define MAX_OSD_NAME 2 /* 通道标题最大支持2个 */

#define MAX_VO_CHN_NUMS 32
#define MAX_MSSTD_SEGS_SIZE 512 /* 视频编码最大支持MSSTD SEG长度 */

#define VIDEO_FILENAME_MAX_LEN 128 /* 文件名最大长度(包含路径) */
#define MAX_VENC_LOC_CHN_NUMS 8    /* 本地的最大编码通道数 */
#define MAX_VENC_EXT_CHN_NUMS 8    /* 扩展的最大编码通道数 */
#define MAX_VENC_CHN_NUMS (MAX_VENC_LOC_CHN_NUMS + MAX_VENC_EXT_CHN_NUMS)

typedef enum {
    VIDEO_STAT_NONE = 0,
    VIDEO_STAT_ENABLE = BIT32(0), /* 是否使能 */
    VIDEO_STAT_OPEN = BIT32(1),   /* 是否开启 */
    VIDEO_STAT_ALL = 0xFF
} VIDEO_STAT_E;

/* VI通道插入图片是否支持宏定义 */
typedef enum {
    VI_NOVIDEO_PIC_NOT_SUPPORT,
    VI_NOVIDEO_PIC_SUPPORT,
} VI_NOVIDEO_PIC_E;

#define TMP_DEVTYPE_USEFORVO 99 /* 定义用于清除开机画面临时机型名 */

#define VIDEO_WIDTH_960H 960
#define VIDEO_WIDTH_960H_928 928
#define VIDEO_WIDTH_960H_864 864
#define VIDEO_HEIGHT_960H_PAL 576
#define VIDEO_HEIGHT_960H_NTSC 480

#define VIDEO_WIDTH_720P 1280
#define VIDEO_HEIGHT_720P 720

#define VIDEO_WIDTH_288P 512
#define VIDEO_HEIGHT_288P 288

#define VIDEO_WIDTH_1080P 1920
#define VIDEO_HEIGHT_1080P 1080

#define VIDEO_WIDTH_1080H (VIDEO_WIDTH_1080P / 2)
#define VIDEO_HEIGHT_1080H 1080

#define VIDEO_WIDTH_2560_1440 2560
#define VIDEO_HEIGHT_2560_1440 1440

#define VIDEO_WIDTH_2560_1600 2560
#define VIDEO_HEIGHT_2560_1600 1600

#define VIDEO_WIDTH_4K 3840
#define VIDEO_HEIGHT_4K 2160

#define VIDEO_WIDTH_4K_V2 3864
#define VIDEO_HEIGHT_4K_V2 2164

#define VIDEO_WIDTH_5M 2880
#define VIDEO_HEIGHT_5M 1616

#define VIDEO_WIDTH_5M_V2 2904
#define VIDEO_HEIGHT_5M_V2 1620

#define VIDEO_WIDTH_FD1 704
#define VIDEO_HEIGHT_FD1_PAL 576
#define VIDEO_HEIGHT_FD1_NTSC 480

#define VIDEO_WIDTH_HD1 704
#define VIDEO_HEIGHT_HD1_PAL 288
#define VIDEO_HEIGHT_HD1_NTSC 240

#define VIDEO_WIDTH_CIF 352
#define VIDEO_HEIGHT_CIF_PAL 288
#define VIDEO_HEIGHT_CIF_NTSC 240

#define VIDEO_WIDTH_QCIF 176
#define VIDEO_HEIGHT_QCIF_PAL 144
#define VIDEO_HEIGHT_QCIF_NTSC 120

#define VIDEO_WIDTH_1366_768 1366
#define VIDEO_HEIGHT_1366_768 768

#define VIDEO_WIDTH_1024_768 1024
#define VIDEO_HEIGHT_1024_768 768

#define VIDEO_WIDTH_1280_800 1280
#define VIDEO_HEIGHT_1280_800 800

#define VIDEO_WIDTH_800_1280 800
#define VIDEO_HEIGHT_800_1280 1280

#define VIDEO_WIDTH_1280_1024 1280
#define VIDEO_HEIGHT_1280_1024 1024

#define VIDEO_WIDTH_1440_900 1440
#define VIDEO_HEIGHT_1440_900 900

#define VIDEO_WIDTH_1024_600 1024
#define VIDEO_HEIGHT_1024_600 600

#define VIDEO_WIDTH_600_1024 600
#define VIDEO_HEIGHT_600_1024 1024

#define VIDEO_WIDTH_800_600 800
#define VIDEO_HEIGHT_800_600 600

#define VIDEO_WIDTH_800_480 800
#define VIDEO_HEIGHT_800_480 480

#define VIDEO_WIDTH_240_320 240
#define VIDEO_HEIGHT_240_320 320

#define VIDEO_WIDTH_Q720P (VIDEO_WIDTH_720P / 2)
#define VIDEO_HEIGHT_Q720P (VIDEO_HEIGHT_720P / 2)
#define VIDEO_WIDTH_Q960H (VIDEO_WIDTH_960H / 2)
#define VIDEO_WIDTH_Q960H_928 (VIDEO_WIDTH_960H_928 / 2)
#define VIDEO_WIDTH_Q960H_864 (VIDEO_WIDTH_960H_864 / 2)
#define VIDEO_WIDTH_Q960H_432 (VIDEO_WIDTH_960H_864 / 2)
#define VIDEO_HEIGHT_Q960H_PAL (VIDEO_HEIGHT_960H_PAL / 2)
#define VIDEO_HEIGHT_Q960H_NTSC (VIDEO_HEIGHT_960H_NTSC / 2)

#define VIDEO_WIDTH_QQ960H (VIDEO_WIDTH_Q960H / 2)
#define VIDEO_WIDTH_QQ960H_928 (VIDEO_WIDTH_Q960H_928 / 2)
#define VIDEO_WIDTH_QQ960H_864 (VIDEO_WIDTH_Q960H_864 / 2)
#define VIDEO_WIDTH_QQ960H_216 (VIDEO_WIDTH_Q960H_432 / 2)
#define VIDEO_HEIGHT_QQ960H_PAL (VIDEO_HEIGHT_Q960H_PAL / 2)
#define VIDEO_HEIGHT_QQ960H_NTSC (VIDEO_HEIGHT_Q960H_NTSC / 2)

#define VIDEO_WIDTH_VGA 640
#define VIDEO_HEIGHT_VGA 480
#define VIDEO_WIDTH_768 768
#define VIDEO_HEIGHT_432 432
#define VIDEO_WIDTH_720 720
#define VIDEO_HEIGHT_576 576
#define VIDEO_300W_X_OFFSET 128
#define VIDEO_WIDTH_300W 2048
#define VIDEO_WIDTH_300W_CROP (VIDEO_WIDTH_300W - VIDEO_300W_X_OFFSET)
#define VIDEO_HEIGHT_300W 1536
#define VIDEO_WIDTH_400W 2560
#define VIDEO_HEIGHT_400W 1440
#define VIDEO_WIDTH_EX_400W 2688
#define VIDEO_HEIGHT_EX_400W 1520
#define VIDEO_WIDTH_500W 2592
#define VIDEO_HEIGHT_500W 1944
#define VIDEO_WIDTH_600W 3100
#define VIDEO_HEIGHT_600W 2100
#define VIDEO_WIDTH_800W 4096
#define VIDEO_HEIGHT_800W 2160
#define VIDEO_WIDTH_1200W 4000
#define VIDEO_HEIGHT_1200W 3000

/* 定义相对坐标的最小和最大值 */
#define RITIO_COOR_MIN 0
#define RITIO_COOR_MAX 10000

/* 坐标模式 */
typedef enum {
    VIDEO_RITIO_COOR = 0, /* Ratio coordinate */
    VIDEO_ABS_COOR        /* Absolute coordinate */
} VIDEO_COORDINATE_E;

#define MAX_VDEC_MAIN_STREAM_NUMS 4
#define MAX_VDEC_JPEG_STREAM_NUMS 1

#define VO_WIDTH_PAL 720
#define VO_HEIGHT_PAL 576

#define VO_WIDTH_NTSC 720
#define VO_HEIGHT_NTSC 480

#define VO_WIDTH_960H_PAL 960
#define VO_HEIGHT_960H_PAL 576

#define VO_WIDTH_960H_NTSC 960
#define VO_HEIGHT_960H_NTSC 480

#define VO_CHN_NUM_PER_DEV 64

#define MIN_CVBS_OFFSETX 0
#define MAX_CVBS_OFFSETX 100

#define MIN_CVBS_OFFSETY 0
#define MAX_CVBS_OFFSETY 100

#define MIN_CVBS_OFFSETW 0
#define MAX_CVBS_OFFSETW 100

#define MIN_CVBS_OFFSETH 0
#define MAX_CVBS_OFFSETH 100

#define MIN_ZOOM_W 32
#define MIN_ZOOM_H 16
#define MIN_SRC_W 32
#define MIN_SRC_H 32

#define MAX_MDBLOCK_NUM 18
#define MAX_VMAREA_NUM 4
#define MAX_OSDAREA_NUM 2
#define MIN_VMAREA_W 8
#define MIN_VMAREA_H 8
#define MIN_VPP_MDSEN 1
#define MAX_VPP_MDSEN 5

#define MIN_VPP_VCSEN 1
#define MAX_VPP_VCSEN 3

#define MAX_VIDEO_CH 64
#define MAX_VI_CH 32
#define MAX_VOSD_CH 64

#define MAX_CHNAME_LENGTH 128
#define MAX_PHYVDECH_NUM 64

#define MIN_VIOVERLAY_REGION_WIDTH 16
#define MIN_VIOVERLAY_REGION_HEIGHT 16

#define VIDEO_SUBCHN_2_W 640
#define VIDEO_SUBCHN_2_H 720

/* For SDI VENC */
#define MAX_ROIAREA_NUM 8
#define MIN_ROIAREA_W 8
#define MIN_ROIAREA_H 8
#define VENC_MAX_NAME_LEN 16
#define VENC_PIC_ALIGN 16

/* 定义用户层视频参数值(亮度) */
#define VIDEO_MIN_BRIGHT 0
#define VIDEO_DEFAULT_BRIGHT 128
#define VIDEO_MAX_BRIGHT 255

/* 定义用户层视频参数值(对比度) */
#define VIDEO_MIN_CONTRAST 0
#define VIDEO_DEFAULT_CONTRAST 128
#define VIDEO_MAX_CONTRAST 255

/* 定义用户层视频参数值(饱和度) */
#define VIDEO_MIN_SATURATION 0
#define VIDEO_DEFAULT_SATURATION 128
#define VIDEO_MAX_SATURATION 255

/* 定义用户层视频参数值(色度) */
#define VIDEO_MIN_HUE 0
#define VIDEO_DEFAULT_HUE 128
#define VIDEO_MAX_HUE 255

/* 定义去噪开关,若关闭则按默认值(level 0)设置去噪强度 */
#define VIDEO_NR_OFF 0
#define VIDEO_NR_ON 1

#define VO_CHN_BORDER_WIDTH_ALIGN_SIZE 2
#define VO_CHN_BORDER_TOP_WIDTH ALIGN_N(0, VO_CHN_BORDER_WIDTH_ALIGN_SIZE)
#define VO_CHN_BORDER_BOTTOM_WIDTH ALIGN_N(2, VO_CHN_BORDER_WIDTH_ALIGN_SIZE)
#define VO_CHN_BORDER_LEFT_WIDTH ALIGN_N(0, VO_CHN_BORDER_WIDTH_ALIGN_SIZE)
#define VO_CHN_BORDER_RIGHT_WIDTH ALIGN_N(2, VO_CHN_BORDER_WIDTH_ALIGN_SIZE)
#define VO_CHN_BORDER_RGB_COLOR 0x060608

#define VO_CHN_BORDER_TOP_WIDTH_T ALIGN_N(0, VO_CHN_BORDER_WIDTH_ALIGN_SIZE)
#define VO_CHN_BORDER_BOTTOM_WIDTH_T ALIGN_N(4, VO_CHN_BORDER_WIDTH_ALIGN_SIZE)
#define VO_CHN_BORDER_LEFT_WIDTH_T ALIGN_N(0, VO_CHN_BORDER_WIDTH_ALIGN_SIZE)
#define VO_CHN_BORDER_RIGHT_WIDTH_T ALIGN_N(4, VO_CHN_BORDER_WIDTH_ALIGN_SIZE)

#define HDMI_MAX_AUDIO_CAP_COUNT 15
#define HDMI_MAX_AUDIO_SMPRATE_COUNT 10

#define MAX_MAREA_CLO_NUM 16 /* 移动侦测横块数 */
#define MAX_MAREA_ROW_NUM 8  /* 移动侦测竖块数 */
#define MAX_MAREA_NUM 128    /* 移动侦测最大块 */

/* 定义去噪等级参数值 */
typedef enum {
    VIDEO_NR_LEVEL0 = 0,
    VIDEO_NR_LEVEL1,
    VIDEO_NR_LEVEL2,
    VIDEO_NR_LEVEL3,
    VIDEO_NR_LEVEL4,
    VIDEO_NR_LEVEL5,
} VIDEO_NR_LEVEL_E;

/* 定义增益开关,若关闭则按默认值(128)设置增益强度 */
#define VIDEO_AGC_OFF 0
#define VIDEO_AGC_ON 1

#define VLIST_MEMBER_MAX_NUMS 16

typedef struct {
    UINT16 x; /* X轴坐标 */
    UINT16 y; /* Y轴坐标 */
} VPOINT_S, *PVPOINT_S;
#define VPOINT_S_LEN sizeof(VPOINT_S)

typedef struct {
    UINT16 w;
    UINT16 h;
} VIDEO_RES_S, *PVIDEO_RES_S;
#define VIDEO_RES_S_LEN sizeof(VIDEO_RES_S)

typedef struct {
    UINT8 Number;                         /* 列表有效成员个数 */
    UINT8 Resvd[3];                       /* 保留位 */
    SINT32 Member[VLIST_MEMBER_MAX_NUMS]; /* 成员列表 */
} VLIST_S, *PVLIST_S;
#define VLIST_S_LEN sizeof(VLIST_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(VLIST_S, 4 + 4 * VLIST_MEMBER_MAX_NUMS);

typedef struct {
    UINT16 x; /* X轴坐标 */
    UINT16 y; /* Y轴坐标 */
    UINT16 w; /* X轴宽度 */
    UINT16 h; /* Y轴高度 */
} VRECT_S, *PVRECT_S;
#define VRECT_S_LEN sizeof(VRECT_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(VRECT_S, 8);

/* 定义去噪等级参数值 */
typedef enum {
    VCOLOR_RGBA_BLUE_PURE,
    VCOLOR_RGBA_TURQUOISE,
    VCOLOR_RGBA_WHITE,
    VCOLOR_RGBA_RED,
    VCOLOR_RGBA_BLACK,
    VCOLOR_RGBA_BUTT
} VCOLOR_RGBA_E;
#define VCOLOR_RGBA_DEF VCOLOR_RGBA_TURQUOISE

typedef struct {
    UINT32 u32Width;
    UINT32 u32Height;

    UINT32 u32HeaderStride[3];
    UINT32 u32PicStride[3];
    UINT32 u32ExtStride[3];

    UINT64 u64HeaderPhyAddr[3];
    UINT64 u64HeaderVirAddr[3];
    UINT64 u64PhyAddr[3]; /* frame physics address*/
    UINT64 u64VirAddr[3]; /* frame virtual address*/
    UINT64 u64ExtPhyAddr[3];
    UINT64 u64ExtVirAddr[3];

    UINT32 u32BlkId[3];

    UINT32 u32TimeRef;
    UINT64 u64PTS;    /* Payload TimeStamp */
    UINT64 u64SeqNum; /* input frame sequence number */

    UINT32 u32PoolId;

    UINT8 PixFmt; /* 视频帧格式(MV_PIXEL_FORMAT_E) */
    UINT32 u32FrameSize;
} VIDEO_FRM_ALG_S, *PVIDEO_FRM_ALG_S;
#define MAX_VFACE_FRAME_NUM 32
#define MAX_VLINE_FRAME_NUM 32

#define MAX_VMOSAIC_NUM 4

typedef struct {
    UINT16 x1;
    UINT16 y1;
    UINT16 x2;
    UINT16 y2;
} VLINE_S, *PVLINE_S;
#define VLINE_S_LEN sizeof(VLINE_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(VLINE_S, 8);

/* 线段信息结构体 */
typedef struct {
    UINT8 LineW;                       /* 线宽 */
    UINT8 LineCnt;                     /* 线段区域个数 */
    UINT8 bClear;                      /* 是否清除画线,0:不清除, 1:清除 */
    UINT8 bKeep;                       /* 是否保留上次状态,1:保留 */
    UINT8 Color[MAX_VLINE_FRAME_NUM];  /* 线段颜色(VCOLOR_RGBA_E) */
    VLINE_S Line[MAX_VLINE_FRAME_NUM]; /* 线段区域 */
    PVIDEO_FRM_ALG_S pstVideoFrame;
} VLINE_FRAME_S, *PVLINE_FRAME_S;
#define VLINE_FRAME_S_S_LEN sizeof(VLINE_FRAME_S)
#ifndef POINTER_SIZE_8
ASSERT_SIZEOF_TYPEDEF_STRUCT(VLINE_FRAME_S,
                             4 + MAX_VLINE_FRAME_NUM + VLINE_S_LEN * MAX_VLINE_FRAME_NUM + sizeof(PVIDEO_FRM_ALG_S));
#else
ASSERT_SIZEOF_TYPEDEF_STRUCT(VLINE_FRAME_S,
                             8 + MAX_VLINE_FRAME_NUM + VLINE_S_LEN * MAX_VLINE_FRAME_NUM + sizeof(PVIDEO_FRM_ALG_S));
#endif
/* 脸框信息结构体 */
typedef struct {
    UINT8 LineW;                       /* 线宽 */
    UINT8 RectCnt;                     /* 脸框区域表 */
    UINT8 Resvd[2];                    /* 保留位 */
    UINT8 Color[MAX_VFACE_FRAME_NUM];  /* 脸框颜色(VCOLOR_RGBA_E) */
    VRECT_S Rect[MAX_VFACE_FRAME_NUM]; /* 脸框区域 */
    PVIDEO_FRM_ALG_S pstVideoFrame;
} VFACE_FRAME_S, *PVFACE_FRAME_S;
#define VFACE_FRAME_S_S_LEN sizeof(VFACE_FRAME_S)
#ifndef POINTER_SIZE_8
ASSERT_SIZEOF_TYPEDEF_STRUCT(VFACE_FRAME_S,
                             4 + MAX_VFACE_FRAME_NUM + VRECT_S_LEN * MAX_VFACE_FRAME_NUM + sizeof(PVIDEO_FRM_ALG_S));
#else
ASSERT_SIZEOF_TYPEDEF_STRUCT(VFACE_FRAME_S,
                             8 + MAX_VFACE_FRAME_NUM + VRECT_S_LEN * MAX_VFACE_FRAME_NUM + sizeof(PVIDEO_FRM_ALG_S));
#endif
/*mosaic信息结构体*/
typedef struct {
    UINT8 BlkSize; /*马赛克块大小*/
    VRECT_S Rect;  /*马赛克区域*/
} VMOSAIC_REGION_S;

/*OSD叠加信息结构体 */
typedef struct {
    VRECT_S Rect;    /* 区域位置 */
    void *pData;     /* Address of Bitmap's data */
    UINT32 Color;    /*颜色--》 使用平台内存地址方式交接口复用为平台返回的stride值*/
    UINT32 InvThl;   /*反色阈值--》复用为平台绘制缓存个数*/
    UINT32 InvColor; /*反色颜色*/
    UINT8 InvEnable; /*是否开启反色*/
    UINT8 FgAlpha;   /*前景透明度，范围[0,255],值越小，越透明*/
} VOSD_OVERYLAY_S, *PVOSD_OVERYLAY_S;
#define VOSD_OVERYLAY_S_S_LEN sizeof(VOSD_OVERYLAY_S)

/*OSD叠加信息结构体 */
typedef struct {
    UINT32 nX;           /*应用下发点阵起始点x坐标*/
    UINT32 nY;           /*应用下发点阵起始点y坐标*/
    UINT32 nPicWidth;    /*当前绘制码流分辨率宽*/
    UINT32 nPicHeight;   /*当前绘制码流分辨率高*/
    UINT32 alignType;    /*对齐方式：0-左对齐；1-右对齐；2-中心对齐*/
    UINT16 strText[512]; /*!< OSD的文字内容,编码必须为unicode */
    UINT32 Color;        /*颜色*/
    UINT32 InvThl;       /*反色阈值*/
    UINT32 InvColor;     /*发色颜色*/
    UINT8 InvEnable;     /*是否开启反色*/
} VOSD_TEXT_OVERYLAY_S, *PVOSD_TEXT_OVERYLAY_S;
#define VOSD_TEXT_OVERYLAY_S_S_LEN sizeof(VOSD_TEXT_OVERYLAY_S)

typedef enum {
    VDEC_MODE_STREAM = 0, /* 流模式,H.264可使用帧模式或流模式 */
    VDEC_MODE_FRAME,      /* 帧模式,JPEG,MJPEG必须使用帧模式 */
    VDEC_MODE_BUTT
} VDEC_MODE_E;

typedef enum {
    VDEC_CHNTYPE_PLAYBACK, /* 回放解码通道 */
    VDEC_CHNTYPE_LIVE,     /* 预览解码通道 */
    VDEC_CHNTYPE_SNAPSHOT, /* 抓拍解码通道 */
    VDEC_CHNTYPE_PREVIEW,  /* 回放前预览解码通道 */
    VDEC_CHNTYPE_BUTT
} VDEC_CHNTYPE_E;

typedef enum {
    VDEC_DEAL_BLOCKING = -1, /* 阻塞模式 */
    VDEC_DEAL_NONBLOCK = 0,  /* 非阻塞模式 */
    VDEC_DEAL_TIMEOUT = 40,  /* 超时模式(默认是为40毫秒,此值可自指定) */
} VDEC_DEAL_MODE_E;

typedef struct {
    UINT8 vmode;                                    /* 视频制式(VI_MODE_E) */
    UINT8 vsize;                                    /* 视频编码分辨率(VIDEO_PIC_SIZE_E) */
    UINT8 Resvd[2];                                 /* 保留位 */
    SINT32 MaxVdecNum;                              /* 最大支持的解码通道数 */
    char VideoNetNoSglPath[VIDEO_FILENAME_MAX_LEN]; /* IPC无信号视频图片路径 */
} VDEC_CONF_S, *PVDEC_CONF_S;
#define VDEC_CONF_S_LEN sizeof(VDEC_CONF_S)

/* 视频通道状态 */
typedef enum {
    VIDEO_CHNSTAT_OPEN,  /* 打开 */
    VIDEO_CHNSTAT_CLOSE, /* 关闭 */
    VIDEO_CHNSTAT_RUN,   /* 工作 */
    VIDEO_CHNSTAT_PAUSE  /* 暂停 */
} VIDEO_CHNSTAT_E;

typedef enum {
    VISP_INPUT_MODE_INDOOR,  /* 室内 */
    VISP_INPUT_MODE_OUTDOOR, /* 室外 */
    VISP_INPUT_MODE_BUTT
} VISP_INPUT_MODE_E;

typedef enum { VISP_MODE_LINE = 0, VISP_MODE_WDR, VISP_MODE_BUTT } VISP_MODE_TYPE_E;

/* 图像3A模式 */
typedef enum {
    VISP_AAA_MODE_NONE = 0, /* 无 */
    VISP_AAA_MODE_AE,       /* AE */
    VISP_AAA_MODE_AWB,      /* AWB */
    VISP_AAA_MODE_BOTH      /* AE & AWB */
} VISP_AAA_MODE_E;

/* 图像BC模式 */
typedef enum {
    VISP_BC_MODE_NONE = 0,   /* 无 */
    VISP_BC_MODE_BACKL_COMP, /* 背光补偿(Backlight compensation) */
    VISP_BC_MODE_GLARE_SUPP, /* 强光抑制(Glare suppression) */
    VISP_BC_MODE_BUTT
} VISP_BC_MODE_E;

/* 图像BC位置 */
typedef enum {
    VISP_BC_POSI_NONE = 0, /* 无 */
    VISP_BC_POSI_UP,       /* 上 */
    VISP_BC_POSI_DOWN,     /* 下 */
    VISP_BC_POSI_LEFT,     /* 左 */
    VISP_BC_POSI_RIGHT,    /* 右 */
    VISP_BC_POSI_MIDDLE,   /* 中 */
    VISP_BC_POSI_BUTT
} VISP_BC_POSI_E;

/* 图像黑白模式 */
typedef enum {
    VISP_BW_MODE_AUTO = 0, /* 自动 */
    VISP_BW_MODE_COLOR,    /* 彩色 */
    VISP_BW_MODE_BW,       /* 黑白 */
    VISP_BW_MODE_TRANSIT,  /* 过渡 */
    VISP_BW_MODE_BUTT
} VISP_BW_MODE_E;

/* 背光补偿/强光抑制参数的结构体 */
typedef struct {
    UINT8 enable; /* 使能标志位(表示是否使能背光补偿或是强光抑制) */
    UINT8 BcMode; /* 图像BC模式(VISP_BC_MODE_E) */
    UINT8 BcVal;  /* 目标值,区间[0, 100] */
    UINT8 BcPosi; /* 图像BC位置(VISP_BC_POSI_E)(当前未实现) */
    VRECT_S Area; /* 客户手动选择域(当前未实现) */
} VISP_BC_ATTR_S, *PVISP_BC_ATTR_S;

/* 白平衡模式 */
typedef enum {
    VISP_WB_MODE_AUTO = 0, /* 自动 */
    VISP_WB_MODE_MANUAL,   /* 手动 */
    VISP_WB_MODE_NATURAL,  /* 自然光 */
    VISP_WB_MODE_INCANDES, /* 白炽灯 */
    VISP_WB_MODE_BUTT
} VISP_WB_MODE_E;

/* 白平衡参数 */
typedef struct {
    UINT8 WbMode;    /* 模式(VISP_WB_MODE_E) */
    UINT8 Resvd[3];  /* 保留位 */
    UINT16 RedGain;  /* 红色增益 */
    UINT16 BlueGain; /* 蓝色增益 */
} VISP_WB_ATTR_S, *PVISP_WB_ATTR_S;
#define VISP_WB_ATTR_S_LEN sizeof(VISP_WB_ATTR_S)

/* 图像3D降噪参数 */
typedef struct {
    UINT8 enable;     /* 使能 */
    UINT8 SfStrength; /* 空域去噪强度[0, 255] */
    UINT8 TfStrength; /* 时域去噪强度[0, 128] */
    UINT8 Resvd;      /* 保留 */
} VISP_3DENOISE_ATTR_S, *PVISP_3DENOISE_ATTR_S;
#define VISP_3DENOISE_ATTR_S_LEN sizeof(VISP_3DENOISE_ATTR_S)

typedef struct {
    UINT8 enable;    /* 使能 */
    UINT8 Resvd[3];  /* 保留 */
    UINT32 Strength; /* 校正强度(Corrected strength,range at [0, 0xFFF]) */
} VISP_DP_ATTR_S, *PVISP_DP_ATTR_S;
#define VISP_DP_ATTR_S_LEN sizeof(VISP_DP_ATTR_S)

typedef struct {
    UINT8 enable;   /* 使能 */
    UINT8 Strength; /* 强度 */
    UINT8 Resvd[2]; /* 保留 */
} VISP_DEFOG_ATTR_S, *PVISP_DEFOG_ATTR_S;
#define VISP_DEFOG_ATTR_S_LEN sizeof(VISP_DEFOG_ATTR_S)

/* 去伪彩参数 */
typedef struct {
    UINT8 enable;   /* 使能位 */
    UINT8 Strength; /* 去伪彩数值[0x0, 0xFF] */
    UINT8 Resvd[2]; /* 保留 */
} VISP_FALSECOLOR_ATTR_S, *PVISP_FALSECOLOR_ATTR_S;
#define VISP_FALSECOLOR_ATTR_S_LEN sizeof(VISP_FALSECOLOR_ATTR_S)

/* 抗闪烁模式 */
typedef enum {
    VISP_FLICKER_MODE_50HZ = 0, /* 50HZ */
    VISP_FLICKER_MODE_60HZ      /* 60HZ */
} VISP_FLICKER_MODE_E;

/* 抗闪烁参数 */
typedef struct {
    UINT8 enable;   /* 使能位 */
    UINT8 Flicker;  /* 抗闪烁频率值(VISP_FLICKER_MODE_E) */
    UINT8 Resvd[2]; /* 保留 */
} VISP_FLICKER_ATTR_S, *PVISP_FLICKER_ATTR_S;
#define VISP_FLICKER_ATTR_S_LEN sizeof(VISP_FLICKER_ATTR_S)

/* 抗闪烁模式 */
typedef enum {
    VISP_IRIS_STAT_AUTO = 0, /* 自动 */
    VISP_IRIS_STAT_OPEN,     /* 打开 */
    VISP_IRIS_STAT_CLOSE,    /* 关闭 */
    VISP_IRIS_STAT_BUTT
} VISP_IRIS_STAT_E;

typedef struct {
    UINT8 IrisStat; /* 自动光圈状态(VISP_IRIS_STAT_E) */
    UINT8 Resvd[3]; /* 保留 */
} VISP_IRIS_ATTR_S, *PVISP_IRIS_ATTR_S;
#define VISP_IRIS_ATTR_S_LEN sizeof(VISP_IRIS_ATTR_S)

typedef enum {
    VISP_SCENE_MODE_0 = 0,
    VISP_SCENE_MODE_1 = 1,
    VISP_SCENE_MODE_2 = 2,
    VISP_SCENE_MODE_3 = 3,
    VISP_SCENE_MODE_4 = 4,
    VISP_SCENE_MODE_5 = 5,
    VISP_SCENE_MODE_BUTT,
    /* 兼容之前的具体场景名，后续弃用，由应用层自定义 */
    VISP_SCENE_MODE_NORMAL = VISP_SCENE_MODE_0,     /* 普通场景 */
    VISP_SCENE_MODE_BLIGHT = VISP_SCENE_MODE_1,     /* 黑光场景 */
    VISP_SCENE_MODE_STATIC = VISP_SCENE_MODE_2,     /* 静态参数 */
    VISP_SCENE_MODE_BL_STATIC = VISP_SCENE_MODE_3,  /* 黑光静态参数 */
} VISP_SCENE_MODE_E;

/* 视频制式 */
typedef enum { VIDEO_MODE_PAL = 0, VIDEO_MODE_NTSC } VI_MODE_E;

/* 视频输入格式 */
typedef enum {
    VI_FORMAT_PAL = 0,
    VI_FORMAT_NTSC,
    VI_FORMAT_1080P60,
    VI_FORMAT_1080P50,
    VI_FORMAT_1080P30,
    VI_FORMAT_1080P25,
    VI_FORMAT_720P60,
    VI_FORMAT_720P50,
    VI_FORMAT_720P30,
    VI_FORMAT_720P25,
    VI_FORMAT_1080I60 = 10,
    VI_FORMAT_1080I50,
    VI_FORMAT_1080H_P25,
    VI_FORMAT_1080H_P30,
    VI_FORMAT_960H_PAL,
    VI_FORMAT_960H_NTSC = 15,
    VI_FORMAT_ANRT_5M20P,
    VI_FORMAT_ANRT_5M12P,
    VI_FORMAT_ART_3M30P,
    VI_FORMAT_ART_3M25P,
    VI_FORMAT_ANRT_3M18P,
    VI_FORMAT_ART_4M30P,
    VI_FORMAT_ART_4M25P,
    VI_FORMAT_ANRT_4M15P = 30,
    VI_FORMAT_TNRT_5M12P = 40,
    VI_FORMAT_TNRT_3M18P,
    VI_FORMAT_T1080P30,
    VI_FORMAT_T1080P25,
    VI_FORMAT_T720P60,
    VI_FORMAT_T720P50,
    VI_FORMAT_T720P30A,
    VI_FORMAT_T720P30B,
    VI_FORMAT_T720P25B,
    VI_FORMAT_T720P24A,
    VI_FORMAT_C1080P30 = 60,
    VI_FORMAT_C1080P25,
    VI_FORMAT_C720P60,
    VI_FORMAT_C720P50,
    VI_FORMAT_C720P30,
    VI_FORMAT_C720P25,
    VI_FORMAT_EXT
} VI_FORMAT_E;

/* 视频通道960H信号补偿模式 */
typedef enum {
    VI_CVBS_MODE_STD = 0, /* 标准模式 */
    VI_CVBS_MODE_EMC1,    /* 兼容模式一 */
    VI_CVBS_MODE_EMC2,
    VI_CVBS_MODE_EMC3,
    VI_CVBS_MODE_EMC4,
    VI_CVBS_MODE_BUTT
} VI_CVBS_MODE_E;

/* 视频通道CHN的检测模式 */
typedef enum {
    VI_AD_DET_MODE_AUTO = 0, /* AUTO模式 */
    VI_AD_DET_MODE_AHD,      /*  AHD模式 */
    VI_AD_DET_MODE_CVI,      /*  CVI模式 */
    VI_AD_DET_MODE_TVI,      /*  TVI模式 */
    VI_AD_DET_MODE_OTHER,
    VI_AD_DET_MODE_BUTT
} VI_AD_DET_MODE_E;

/* 机型查寻结构,以视频AD复用模式区分机型,CHIP表示DEV个数的意思 */
typedef enum { VI_MULTI_MODE_DEF = 0x00, VI_MULTI_MODE_BUTT } VI_MULTI_MODE_E;

/* 语言类型 */
typedef enum {
    LANG_TYPE_CHE = 0, /* 中文 */
    LANG_TYPE_ENG,     /* 英文 */
} LANG_TYPE_E;

/* type of osd */
typedef enum {
    VOSD_VMASK_STRM = 0, /* 屏蔽码流 */
    VOSD_VMASK_ALL,      /* 屏蔽全部(预览和码流) */
    VOSD_STR_STRM,       /* 将字符串叠加至码流 */
    VOSD_STR_ALL,        /* 将字符串叠加至全部(预览和码流) */
    VOSD_PIC_STRM,       /* 将图片叠加至码流 */
    VOSD_PIC_ALL,        /* 将图片叠加至全部(预览和码流) */
    VOSD_MODE_BUTT
} VOSD_MODE_E;

typedef enum {
    VOSD_TYPE_TITLE,
    VOSD_TYPE_TITLE_EX,
    VOSD_TYPE_TIME,
    VOSD_TYPE_DATE,
    VOSD_TYPE_PIC,
    VOSD_TYPE_BUTT
} VOSD_TYPE_E;
#define MAX_VOSD_TYPE VOSD_TYPE_BUTT

/* type of video regions */
typedef enum {
    VOSD_CHNTYPE_VENC_MAIN,      /* 编码主流 */
    VOSD_CHNTYPE_VENC_MINOR,     /* 编码次流 */
    VOSD_CHNTYPE_VENC_MAIN_SEC,  /* 子板编码主流 */
    VOSD_CHNTYPE_VENC_MINOR_SEC, /* 子板编码次流 */
    VOSD_CHNTYPE_VENC_SNAP,      /* 抓图流 */
    VOSD_CHNTYPE_VENC_SNAP_SEC,  /* 子板抓图流 */
    VOSD_CHNTYPE_VENC_AI_SEC,    /*子板算法流*/
    VOSD_CHNTYPE_VENC_AI,        /* 算法流 */
    VOSD_CHNTYPE_VENC_THIRD, /* 编码复合通道 */
    VOSD_CHNTYPE_BUTT
} VOSD_CHNTYPE_E;
#define MAX_VOSD_CHNTYPE_NUM VOSD_CHNTYPE_BUTT
#define MAX_VOSD_BITMAP_NUM 15

typedef enum {
    VOSD_CHNTYPE_TAB_VENC_MAIN = BIT32(VOSD_CHNTYPE_VENC_MAIN),   /* 编码主流屏蔽 */
    VOSD_CHNTYPE_TAB_VENC_MINOR = BIT32(VOSD_CHNTYPE_VENC_MINOR), /* 编码次流屏蔽 */
    VOSD_CHNTYPE_TAB_VENC_THIRD = BIT32(VOSD_CHNTYPE_VENC_THIRD), /* 编码第三编流屏蔽 */
    VOSD_CHNTYPE_TAB_BUTT = 0xFFFFFFFF,
} VOSD_CHNTYPE_TAB_E;

/* type of video regions */
typedef enum {
    VOSD_RGN_OVERLAY = 0, /* 叠加OSD */
    VOSD_RGN_COVER,       /* 遮挡/马赛克 */
    VOSD_RGN_LINE,        /* 画线 */
    VOSD_RGN_RECT,        /* 画框*/
    VOSD_RGN_BUTT
} VOSD_RGN_TYPE_E;

typedef enum {
    VOSD_TIME_24H = 0, /* 24小时制 */
    VOSD_TIME_12H = 1, /* 12小时制 */
    VOSD_TIME_BUTT
} VOSD_TIME_FORAMT_E;

typedef enum {
    VOSD_DATE_YMD = 0,   /* YYYY-MM-DD 日期格式 */
    VOSD_DATE_DMY,       /* DD-MM-YYYY 日期格式 */
    VOSD_DATE_MDY,       /* MM-DD-YYYY 日期格式 */
    VOSD_DATE_YMD_SLASH, /* YYYY/MM/DD 日期格式 */
    VOSD_DATE_DMY_SLASH, /* DD/MM/YYYY 日期格式 */
    VOSD_DATE_MDY_SLASH, /* MM/DD/YYYY 日期格式 */
    VOSD_DATE_YMD_DOT,   /* YYYY.MM.DD 日期格式 */
    VOSD_DATE_DMY_DOT,   /* DD.MM.YYYY 日期格式 */
    VOSD_DATE_MDY_DOT,   /* MM.DD.YYYY 日期格式 */
    VOSD_DATE_BUTT
} VOSD_DATE_FORAMT_E;

typedef enum { VOSD_ZOOM_HALF = 0, VOSD_ZOOM_1X = 1, VOSD_ZOOM_2X = 2, VOSD_ZOOM_3X = 3, VOSD_ZOOM_BUTT } VOSD_ZOOM_E;

typedef enum {
    VOSD_FONT_1X = 0, /* 小号字体 */
    VOSD_FONT_2X = 1, /* 中号字体 */
    VOSD_FONT_3X = 2, /* 大号字体 */
    VOSD_FONT_BUTT
} VOSD_FONT_E;

typedef struct {
    UINT8 RgnType;  /* region type(VOSD_RGN_TYPE_E) */
    UINT8 Resvd[3]; /* 保留位 */
    VRECT_S Rect;
} VOSD_AREA_S, *PVOSD_AREA_S;
#define VOSD_AREA_S_LEN sizeof(VOSD_AREA_S)

typedef struct {
    SINT8 AreaNums;                   /* 区域屏蔽个数,最大4个,0-表示DISABLE区域屏蔽 */
    UINT8 RgnType;                    /* 区域类型(VOSD_RGN_TYPE_E) */
    UINT8 Resvd[2];                   /* 保留位 */
    VOSD_AREA_S Area[MAX_VMAREA_NUM]; /* 每个区域的坐标(如果是VO输出全屏蔽,需要设置area[0]设置为{0, 0, -1, -1}) */
} VOSD_VM_PARAM_S, *PVOSD_VM_PARAM_S;
#define VOSD_VM_PARAM_S_LENsizeof(VOSD_VM_PARAM_S)

typedef struct {
    UINT8 enOsdTranspa; /* OSD背景是否透明(MV_FALSE-不透明,MV_TRUE-透明) */
    UINT8 enOsdFlick;   /* OSD是否闪烁(MV_FALSE-不闪烁,MV_TRUE-闪烁) */
    UINT8 Resvd[2];     /* 保留位 */
} VOSD_ATTRI_S, *PVOSD_ATTRI_S;
#define VOSD_ATTRI_S_LEN sizeof(VOSD_ATTRI_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(VOSD_ATTRI_S, 4);

typedef struct {
    UINT8 enUpdate; /* 更新PIC OSD标志(MV_FALSE-不更新,MV_TRUE-更新) */
    UINT8 VosdType; /* (VOSD_TYPE_E) */
    UINT8 ShowFlag;
    UINT8 Resvd; /* 保留位 */
    VPOINT_S PicPosi;
    char LogoPath[VIDEO_FILENAME_MAX_LEN]; /* LOGO水印图片路径 */
} VOSD_PICATTR_S, *PVOSD_PICATTR_S;
#define VOSD_PICATTR_S_LEN sizeof(VOSD_PICATTR_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(VOSD_PICATTR_S, 4 + VPOINT_S_LEN + VIDEO_FILENAME_MAX_LEN);

typedef struct {
    UINT8 EnChnName[MAX_OSD_NAME];                      /* 是否叠通道名,1:叠加,0或其它不叠加,以下同 */
    UINT8 EnDate;                                       /* 是否叠加日期 */
    UINT8 EnTime;                                       /* 是否叠加时间 */
    UINT8 EnWeek;                                       /* 是否叠加星期 */
    UINT8 EnPic;                                        /* 是否叠加图片 */
    UINT8 RgnType;                                      /* 区域类型(VOSD_RGN_TYPE_E) */
    UINT8 TimeFmt;                                      /* 时间格式(VOSD_TIME_FORAMT_E) */
    UINT8 DateFmt;                                      /* 日期格式(VOSD_DATE_FORAMT_E) */
    UINT8 FontSize;                                     /* 字体大小(VOSD_FONT_E) */
    UINT8 ChnName[MAX_OSD_NAME][MAX_CHNAME_LENGTH + 1]; /* 叠加通道名 */
    VPOINT_S ChnNamePosi[MAX_OSD_NAME];                 /* 通道名位置 */
    VPOINT_S DateTimePosi[MAX_OSD_NAME];                /* 日期时间位置 */
    VOSD_ATTRI_S OsdDispAttr;                           /* OSD显示属性 */
    VOSD_PICATTR_S OsdPicAttr;                          /* 叠加图片属性*/
} VOSD_OVERLAYPARAM_S, *PVOSD_OVERLAYPARAM_S;
#define VOSD_OVERLAYPARAM_S_LEN sizeof(VOSD_OVERLAYPARAM_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(VOSD_OVERLAYPARAM_S, MAX_OSD_NAME + 8 + MAX_OSD_NAME * (MAX_CHNAME_LENGTH + 1) +
                                                      VPOINT_S_LEN * MAX_OSD_NAME * 2 + VOSD_ATTRI_S_LEN +
                                                      VOSD_PICATTR_S_LEN);

typedef struct {
    UINT8 ltype;      /* 语言类型(LANG_TYPE_E) */
    SINT8 MaxVosdChn; /* VOSD最大通道数 */
    UINT8 Resvd[6];   /* 保留位 */
    FONT_CONF_S FontConf;
} VOSD_CONF_S, *PVOSD_CONF_S;
#define VOSD_CONF_S_LEN sizeof(VOSD_CONF_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(VOSD_CONF_S, 8 + FONT_CONF_S_LEN);

typedef enum {
    VDA_SENSI_LEVEL1 = 1, /* VDA检测灵敏度最低级别 */
    VDA_SENSI_LEVEL2,
    VDA_SENSI_LEVEL3,
    VDA_SENSI_LEVEL4,
    VDA_SENSI_LEVEL5, /* VDA检测灵敏度最高级别 */
    VDA_SENSI_LEVEL_BUTT
} VDA_SENSI_LEVEL_E;
#define MAX_VDA_SENSI_LEVEL_NUMS (VDA_SENSI_LEVEL_BUTT - 1)

typedef struct {
    UINT8 enable;   /* 是否使能视频遮挡报警(MV_BOOL) */
    UINT8 level;    /* 视频遮挡报警灵敏度(VI_OD_SENSI_LEVEL_E,1-低,3-中,5-高) */
    UINT8 Resvd[2]; /* 保留位 */
} VI_OD_PARAM_S, *PVI_OD_PARAM_S;
#define VI_OD_PARAM_S_LEN sizeof(VI_OD_PARAM_S)

/* 设置视频效果的参数亮度,色度,对比度,饱和度*/
typedef struct {
    UINT8 bright;     /* 亮度(0-255),对高清设备(0-48) */
    UINT8 contrast;   /* 对比度(0-255),对高清设备(0-48) */
    UINT8 saturation; /* 饱和度(0-255),对高清设备无效 */
    UINT8 hue;        /* 色度(0-255),对高清设备(0-31) */
} VI_VIDEO_PARAM_S, *PVI_VIDEO_PARAM_S;

typedef struct {
    UINT8 vmode;     /* 视频制式(VI_MODE_E) */
    UINT8 MaxVdaChn; /* VDA最大通道数 */
    UINT8 Resvd[2];  /* 保留位 */
} VDA_CONF_S, *PVDA_CONF_S;
#define VDA_CONF_S_LEN sizeof(VDA_CONF_S)

typedef struct {
    UINT8 enable;                /* 是否使能移动侦测 */
    UINT8 sen;                   /* 侦测灵敏度(1-5)  VDA_SENSI_LEVEL_E */
    UINT8 MbCnt[2];              /* MB个数 wcnt每行块数, hcnt每列块数 */
    UINT32 Blk[MAX_MDBLOCK_NUM]; /* 32*32为单位,最大(704*576)共22*18,每位代表一个单位是否有效 */
    UINT32 snapQuality;
} VDA_MD_PARAM_S, *PVDA_MD_PARAM_S;
#define VDA_MD_PARAM_S_LEN sizeof(VDA_MD_PARAM_S)

typedef struct {
    UINT8 enable;   /* 是否使能视频遮挡报警(MV_BOOL) */
    UINT8 level;    /* 视频遮挡报警灵敏度(VDA_SENSI_LEVEL_E) */
    UINT8 Resvd[2]; /* 保留位 */
} VDA_OD_PARAM_S, *PVDA_OD_PARAM_S;
#define VDA_OD_PARAM_S_LEN sizeof(VDA_OD_PARAM_S)

typedef struct {
    SINT8 VpssChnType; /* (VPSS_CHNTYPE_E) */
    UINT8 VdaSrcChn;   /* 源设备设备号 */
    UINT8 Resvd[2];    /* 保留位 */
} VDA_BIND_CONF_S, *PDA_BIND_CONF_S;
#define VDA_BIND_CONF_S_LEN sizeof(VDA_BIND_CONF_S)

typedef struct {
    UINT8 VdaChnIdx;  /* vda通道号 */
    UINT8 VdaSrcChn;  /* 源设备设备号 */
    UINT8 Rotation;   /* ROTATION_E */
    UINT8 CropRatioH; /* VPSS通道水平裁减比例 */
    UINT8 CropRatioV; /* VPSS通道垂直裁减比例 */
    UINT8 Resvd[3];
    UINT16 Width;
    UINT16 Height;
} VDA_CHN_ATTR_S, *PVDA_CHN_ATTR_S;
#define VDA_CHN_ATTR_S_LEN sizeof(VDA_CHN_ATTR_S)

typedef struct {
    UINT16 BlkNum;              /* 移动块个数 */
    UINT8 MArea[MAX_MAREA_NUM]; /* 移动区块位置标识 */
    UINT32 SnapLen;             /* 移动侦测抓图长度*/
    UINT8 *pSnapData;           /* 移动侦测抓图数据*/
} VDA_MD_AREADATA_S, *PVDA_MD_AREADATA_S;
#define VDA_MD_AREADATA_S_LEN sizeof(VDA_MD_AREADATA_S)
#ifndef POINTER_SIZE_8
ASSERT_SIZEOF_TYPEDEF_STRUCT(VDA_MD_AREADATA_S, 140);
#else
ASSERT_SIZEOF_TYPEDEF_STRUCT(VDA_MD_AREADATA_S, 144);
#endif

typedef struct {
    UINT32 Width;   /* 帧宽 */
    UINT32 Height;  /* 帧高 */
    UINT32 Size;    /* 帧大小 */
    UINT8 *pFrmBuf; /* 帧数据 */
} VDA_QR_FRAME_INFO_S, *PVDA_QR_FRAME_INFO_S;

#define VDA_QR_MAX_PAYLOAD_LEN 256

typedef enum {
    VDA_QR_STATE_DECODE_SUCCESS = 0x00,
    VDA_QR_STATE_NON_EXISTENT = 0x01,
    VDA_QR_STATE_DECODE_FAIL = 0x02,
    VDA_QR_STATE_DATA_OVERFLOW = 0x03,
    VDA_QR_STATE_BUTT
} VDA_QR_DECODE_STATE_E;

typedef struct {
    VDA_QR_DECODE_STATE_E state;
    UINT32 payloadLen;
    UINT8 payload[VDA_QR_MAX_PAYLOAD_LEN];
} VDA_QR_RESULT_INFO_S, *PVDA_QR_RESULT_INFO_S;

typedef int (*fn_bcd_event_callback)(void *); 

typedef struct {
    UINT32 enable;               	/* enable bcd, 0: disable, 1: enable. */
    UINT32 alarmSen;     			/* alarm sensitivity of crying, range is [0, 100]. default 80*/
    UINT32 alarmPeriod;          	/* alarm period(ms). default 3000*/
    UINT32 alarmCountThre; 			/* crying count threshold in one period. >=0 .default 0*/
    UINT32 alarmInterval;        	/* alarm interval(ms) between two period. default 0*/
    fn_bcd_event_callback callback;	/* the callback function pointer. */
} VDA_BCD_PARAM_S, *PVDA_BCD_PARAM_S;
#define VDA_BCD_PARAM_S_LEN sizeof(VDA_BCD_PARAM_S)

typedef enum {
    VIDEO_SRC_TYPE_VI = 0,
    VIDEO_SRC_TYPE_VO,
    VIDEO_SRC_TYPE_VPSS,
    VIDEO_SRC_TYPE_VDEC,
    VIDEO_SRC_TYPE_VENC,
    VIDEO_SRC_TYPE_PCIV,
    VIDEO_SRC_TYPE_BUTT
} VIDEO_SRC_TYPE_E;

typedef enum {
    VIDEO_FPS_05 = 5,
    VIDEO_FPS_10 = 10,
    VIDEO_FPS_20 = 20,
    VIDEO_FPS_25 = 25,
    VIDEO_FPS_PAL = VIDEO_FPS_25,
    VIDEO_FPS_30 = 30,
    VIDEO_FPS_NTSC = VIDEO_FPS_30,
    VIDEO_FPS_50 = 50,
    VIDEO_FPS_60 = 60
} VIDEO_FPS_E;
#define VIDEO_FPS_MAX VIDEO_FPS_60

typedef enum {
    VIDEO_TYPE_QCIF = 0,
    VIDEO_TYPE_QQ960H,
    VIDEO_TYPE_QVGA,
    VIDEO_TYPE_Q960H,
    VIDEO_TYPE_CIF, /*  4 */
    VIDEO_TYPE_DCIF,
    VIDEO_TYPE_HD1,
    VIDEO_TYPE_VGA,
    VIDEO_TYPE_FD1, /*  8 */
    VIDEO_TYPE_SD,
    VIDEO_TYPE_HD, /* 10 */
    VIDEO_TYPE_960H,
    VIDEO_TYPE_720P,
    VIDEO_TYPE_1080P, /* 13 */
    VIDEO_TYPE_1080I,
    VIDEO_TYPE_BUTT
} VIDEO_TYPE_E;

typedef enum {
    VIDEO_PIC_QCIF = 0,      /* 176*144(PAL) */
    VIDEO_PIC_CIF,           /* 352*288(PAL) */
    VIDEO_PIC_2CIF,          /* 352*576(PAL) */
    VIDEO_PIC_HD1,           /* 704*288(PAL) */
    VIDEO_PIC_FD1,           /* 704*576(PAL) */
    VIDEO_PIC_QQ960H,        /* 240*144(PAL) */
    VIDEO_PIC_Q960H,         /* 480*288(PAL) */
    VIDEO_PIC_2Q960H,        /* 480*576(PAL) */
    VIDEO_PIC_H960H,         /* 960*288(PAL) */
    VIDEO_PIC_960H_PAL,      /* 960*576(PAL) */
    VIDEO_PIC_960H_NTSC,     /* 960*480(NTSC) */
    VIDEO_PIC_QVGA = 20,     /* 320*240 */
    VIDEO_PIC_Q720P,         /* 640*360 */
    VIDEO_PIC_VGA,           /* 640*480 */
    VIDEO_PIC_XGA,           /* 1024*768 */
    VIDEO_PIC_SXGA,          /* 1400*1050 */
    VIDEO_PIC_UXGA,          /* 1600*1200 */
    VIDEO_PIC_QXGA,          /* 2048*1536 */
    VIDEO_PIC_WVGA = 40,     /* 854*480 */
    VIDEO_PIC_WSXGA,         /* 1680*1050 */
    VIDEO_PIC_WUXGA,         /* 1920*1200 */
    VIDEO_PIC_WQXGA,         /* 2560*1600 */
    VIDEO_PIC_HD720_RT = 59, /* 1280*720 */
    VIDEO_PIC_HD720 = 60,    /* 1280*720 */
    VIDEO_PIC_HD1080,        /* 1920*1080 */
    VIDEO_PIC_HD1520,        /* 2688*1520 */
    VIDEO_PIC_HD500W,        /* 2592*1944 */
    VIDEO_PIC_HD1080H,       /* 960*1080 */
    VIDEO_PIC_HD300W,        /* 2048*1536 */
    VIDEO_PIC_HD400W,        /* 2560*1440 */
    VIDEO_PIC_HD500W_16_9,   /* 2880*1620 */
    VIDEO_PIC_HD600W,        /* 3200*1800 */
    VIDEO_PIC_HD4K,        	 /* 3840*2160 */
    VIDEO_PIC_BUTT
} VIDEO_PIC_SIZE_E;

/* 音视频流类型 */
typedef enum {
    /* 帧模式流类型 */
    STREAM_TYPE_PRI = 0, /* 主码流 */
    STREAM_TYPE_SEC,     /* 子码流 */
    STREAM_TYPE_SNAP,    /* 抓图码流 */

    /* 包模式流类型 */
    STREAM_TYPE_PRI_MSHEAD,  /* 主码流帧头 */
    STREAM_TYPE_PRI_DATA,    /* 主码流包数据 */
    STREAM_TYPE_SEC_MSHEAD,  /* 子码流头 */
    STREAM_TYPE_SEC_DATA,    /* 子码流数据 */
    STREAM_TYPE_SNAP_MSHEAD, /* 抓图码流头 */
    STREAM_TYPE_SNAP_DATA,   /* 抓图码流数据 */

    STREAM_TYPE_AUDIO_AI,     /* 音频流 */
    STREAM_TYPE_AUDIO_AENC,   /* 音频流 */
    STREAM_TYPE_AUDIO_TALK,   /* 对讲流 */
    STREAM_TYPE_AUDIO_MSHEAD, /* 音频流帧头 */
    STREAM_TYPE_AUDIO_DATA,   /* 音频流数据 */

    /* 帧模式流类型 */
    STREAM_TYPE_MULTIPRI, /* 复合通道主码流 */
    STREAM_TYPE_MULTISEC, /* 复合通道子码流 */

    /* 包模式流类型 */
    STREAM_TYPE_MULTIPRI_MSHEAD, /* 复合通道主码流帧头 */
    STREAM_TYPE_MULTIPRI_DATA,   /* 复合通道主码流包数据 */
    STREAM_TYPE_MULTISEC_MSHEAD, /* 复合通道子码流头 */
    STREAM_TYPE_MULTISEC_DATA,   /* 复合通道子码流数据 */

    /* 包模式流类型 */
    STREAM_TYPE_AUDIO_TALKBACK_MSHEAD, /* 音频流帧头 */
    STREAM_TYPE_AUDIO_TALKBACK_DATA,   /* 音频流数据 */

    /* 包模式流类型 */
    STREAM_TYPE_CROP_JPEG_MSHEAD, /* 抠图图片流帧头 */
    STREAM_TYPE_CROP_JPEG_DATA,   /* 抠图图片数据 */
} STREAM_TYPE_E;

typedef enum {
    VPAYLOAD_TYPE_H264 = 0,
    VPAYLOAD_TYPE_JPEG,
    VPAYLOAD_TYPE_MJPEG,
    VPAYLOAD_TYPE_H265,
    VPAYLOAD_TYPE_BUTT
} VPAYLOAD_TYPE_E;

typedef enum { VO_ENABLE_N = 0, VO_ENABLE_Y } VO_ENABLE_E;

/* 该定义需与hi_comm_vo.h中vo设备接口类型保持一致 */
typedef enum {
    VO_INTFTYPE_CVBS = (0x01L << 0),
    VO_INTFTYPE_YPBPR = (0x01L << 1),
    VO_INTFTYPE_VGA = (0x01L << 2),
    VO_INTFTYPE_BT656 = (0x01L << 3),
    VO_INTFTYPE_BT1120 = (0x01L << 4),
    VO_INTFTYPE_HDMI = (0x01L << 5),
    VO_INTFTYPE_LCD_6 = (0x01L << 9),
    VO_INTFTYPE_MIPI = (0x01L << 14),
    VO_INTFTYPE_BUTT
} VO_INTFTYPE_E;

typedef enum {
    VO_OUTPUT_MODE_STANDARD, /* 标准 */
    VO_OUTPUT_MODE_BRIGHT,   /* 明亮 */
    VO_OUTPUT_MODE_SOFT,     /* 柔和 */
    VO_OUTPUT_MODE_VIVID,    /* 鲜艳 */
    VO_OUTPUT_MODE_SHARP,    /* 锐利 */
    VO_OUTPUT_MODE_BUTT
} VO_OUTPUT_MODE_E;

/* 该定义需与hi_comm_hdmi.h中HI_HDMI_VIDEO_FMT_E保持一致 */
typedef enum {
    HDMI_VIDEO_FMT_1080P_60 = 0,
    HDMI_VIDEO_FMT_1080P_50,
    HDMI_VIDEO_FMT_1080P_30,
    HDMI_VIDEO_FMT_1080P_25,
    HDMI_VIDEO_FMT_1080P_24,

    HDMI_VIDEO_FMT_1080I_60,
    HDMI_VIDEO_FMT_1080I_50,

    HDMI_VIDEO_FMT_720P_60,
    HDMI_VIDEO_FMT_720P_50,

    HDMI_VIDEO_FMT_576P_50,
    HDMI_VIDEO_FMT_480P_60,

    HDMI_VIDEO_FMT_PAL,    /* B D G H I PAL */
    HDMI_VIDEO_FMT_PAL_N,  /* (N)PAL */
    HDMI_VIDEO_FMT_PAL_NC, /* (NC)PAL */

    HDMI_VIDEO_FMT_NTSC,       /* (M)NTSC */
    HDMI_VIDEO_FMT_NTSC_J,     /* NTSC-J */
    HDMI_VIDEO_FMT_NTSC_PAL_M, /* (M)PAL */

    HDMI_VIDEO_FMT_SECAM_SIN, /* SECAM_SIN */
    HDMI_VIDEO_FMT_SECAM_COS, /* SECAM_COS */

    HDMI_VIDEO_FMT_861D_640X480_60,
    HDMI_VIDEO_FMT_VESA_800X600_60,
    HDMI_VIDEO_FMT_VESA_1024X768_60,
    HDMI_VIDEO_FMT_VESA_1280X720_60,
    HDMI_VIDEO_FMT_VESA_1280X800_60,
    HDMI_VIDEO_FMT_VESA_1280X1024_60,
    HDMI_VIDEO_FMT_VESA_1366X768_60,
    HDMI_VIDEO_FMT_VESA_1440X900_60,
    HDMI_VIDEO_FMT_VESA_1440X900_60_RB,
    HDMI_VIDEO_FMT_VESA_1600X900_60_RB,
    HDMI_VIDEO_FMT_VESA_1600X1200_60,
    HDMI_VIDEO_FMT_VESA_1680X1050_60,
    HDMI_VIDEO_FMT_VESA_1920X1080_60,
    HDMI_VIDEO_FMT_VESA_1920X1200_60,
    HDMI_VIDEO_FMT_VESA_2048X1152_60,    /* Currently, not support */
    HDMI_VIDEO_FMT_VESA_CUSTOMER_DEFINE, /* Currently, not support */

    HDMI_VIDEO_FMT_BUTT
} HDMI_VIDEO_FMT_E;

typedef enum {
    BITRATE_TYPE_VBR,
    BITRATE_TYPE_CBR,
    BITRATE_TYPE_ABR,
    BITRATE_TYPE_FIXQP,
    BITRATE_TYPE_QUALITY,
} BITRATE_TYPE_E;

#define VO_SPLIT_NINE_EX_BIG_PIC_BGN 0
#define VO_SPLIT_NINE_EX_ACTUAL_SCREEN 6
#define VO_SPLIT_SIXTEEN_EX_BIG_PIC_BGN 0
#define VO_SPLIT_SIXTEEN_EX_ACTUAL_SCREEN 8
#define VO_SPLIT_SIXTEEN_EX2_BIG_PIC_BGN 5
#define VO_SPLIT_SIXTEEN_EX2_ACTUAL_SCREEN 13
#define VO_SPLIT_TWENTYFIVE_EX_BIG_PIC_BGN 0
#define VO_SPLIT_TWENTYFIVE_EX_BIG_PIC_NUM 4
#define VO_SPLIT_TWENTYFIVE_ACTUAL_SCREEN 13
#define VO_SPLIT_THIRTYSIX_EX_BIG_PIC_BGN 0
#define VO_SPLIT_THIRTYSIX_EX_BIG_PIC_NUM 4
#define VO_SPLIT_THIRTYSIX_ACTUAL_SCREEN 24
#define VO_SPLIT_PATTERN144_EX_BIG_PIC_BGN 0
#define VO_SPLIT_PATTERN144_EX_BIG_PIC_NUM 4
#define VO_SPLIT_PATTERN144_EX_ACTUAL_SCREEN 20

typedef enum {
    VO_SPLIT_ONE = 1,
    VO_SPLIT_TWO = 2,
    VO_SPLIT_FOUR = 4,
    VO_SPLIT_SIX = 6,
    VO_SPLIT_EIGHT = 8,
    VO_SPLIT_NINE = 9,
    VO_SPLIT_NINE_EX = 10, /* 9画面扩展不规则分屏,实际显示6画面(1大+5小) */
    VO_SPLIT_TWELVE = 12,
    VO_SPLIT_THIRTEEN = 13,
    VO_SPLIT_SIXTEEN = 16,
    VO_SPLIT_SIXTEEN_EX = 17,  /* 16画面扩展不规则分屏,实际显示8画面(1大+7小) */
    VO_SPLIT_SIXTEEN_EX2 = 18, /* 16画面扩展不规则分屏,实际显示13画面(中间1大+四周12小) */
    VO_SPLIT_TWENTY = 20,
    VO_SPLIT_TWENTYFOUR = 24,
    VO_SPLIT_TWENTYFIVE = 25,
    VO_SPLIT_THIRTYTWO = 32,
    VO_SPLIT_THIRTYSIX = 36,
    VO_SPLIT_TWENTYFIVE_EX = 40, /* 25画面不规则分屏,实际显示13画面,大4+小9 */
    VO_SPLIT_THIRTYSIX_EX = 41,  /* 36画面不规则分屏,实际显示24画面,大4+小20 */
    VO_SPLIT_PATTERN144 = 44,    /* 12*12画面规则分屏 */
    VO_SPLIT_PATTERN144_EX = 45, /* 12*12画面不规则分屏,实际显示20画面,大4+小16 */
    VO_SPLIT_SIXTY_FOUR = 64,
    VO_SPLIT_MAX
} VO_SPLIT_E;
#define VO_IRREG_SCRN_MAX 6

/* VO设备类型 */
typedef enum {
    VO_DEV_TYPE_HDMI = 0, /* HMDI设备 */
    VO_DEV_TYPE_DVI,      /* DVI设备 */
    VO_DEV_TYPE_VGA,      /* VGA设备 */
    VO_DEV_TYPE_RGB,      /* RGB设备 */
    VO_DEV_TYPE_YPBPR,    /* YPbPr设备(色差端子) */
    VO_DEV_TYPE_SVIDEO,   /* S-Video设备(S端子也叫分量视频) */
    VO_DEV_TYPE_CVBS,     /* CVBS设备(复合视频) */
    VO_DEV_TYPE_BUTT
} VO_DEV_TYPE_E;

/* VO逻辑设备号位表 */
typedef enum {
    VO_DEV_BIT_TAB_HDMI0 = BIT32(VO_DEV_HDMI0),
    VO_DEV_BIT_TAB_HDMI1 = BIT32(VO_DEV_HDMI1),
    VO_DEV_BIT_TAB_VGA0 = BIT32(VO_DEV_VGA0),
    VO_DEV_BIT_TAB_VGA1 = BIT32(VO_DEV_VGA1),
    VO_DEV_BIT_TAB_CVBS0 = BIT32(VO_DEV_CVBS0),
    VO_DEV_BIT_TAB_MULTI = BIT32(VO_DEV_VIRT0),
    VO_DEV_BIT_TAB_ALL = 0xFFFFFFFF,
} VO_DEV_BIT_TAB_E;

typedef enum { VO_ENBLK_N = 0, VO_ENBLK_Y } VO_ENBLK_E;

/* VO设备状态 */
typedef enum {
    VO_DEVSTAT_LV = 0, /* 实时 */
    VO_DEVSTAT_PB,     /* 回放 */
    VO_DEVSTAT_BK      /* 黑屏 */
} VO_DEVSTAT_E;

/* 通道回放状态 */
typedef enum { PB_STAT_NORM, PB_STAT_PAUSE } PB_STAT_E;

/* 机器通道 */
typedef enum {
    DEV_CHN_IDX0 = 0,
    DEV_CHN_IDX1,
    DEV_CHN_IDX2,
    DEV_CHN_IDX3,
    DEV_CHN_IDX4,
    DEV_CHN_IDX5,
    DEV_CHN_IDX6,
    DEV_CHN_IDX7,
    DEV_CHN_IDX8,
    DEV_CHN_IDX9,
    DEV_CHN_IDX10,
    DEV_CHN_IDX11,
    DEV_CHN_IDX12,
    DEV_CHN_IDX13,
    DEV_CHN_IDX14,
    DEV_CHN_IDX15,
    DEV_CHN_IDX16,
    DEV_CHN_IDX17,
    DEV_CHN_IDX18,
    DEV_CHN_IDX19,
    DEV_CHN_IDX20,
    DEV_CHN_IDX21,
    DEV_CHN_IDX22,
    DEV_CHN_IDX23,
    DEV_CHN_IDX24,
    DEV_CHN_IDX25,
    DEV_CHN_IDX26,
    DEV_CHN_IDX27,
    DEV_CHN_IDX28,
    DEV_CHN_IDX29,
    DEV_CHN_IDX30,
    DEV_CHN_IDX31,
    DEV_CHN_IDX32,
    DEV_CHN_IDX33,
    DEV_CHN_IDX34,
    DEV_CHN_IDX35,
    DEV_CHN_IDX36,
    DEV_CHN_IDX37,
    DEV_CHN_IDX38,
    DEV_CHN_IDX39,
    DEV_CHN_IDX40,
    DEV_CHN_IDX41,
    DEV_CHN_IDX42,
    DEV_CHN_IDX43,
    DEV_CHN_IDX44,
    DEV_CHN_IDX45,
    DEV_CHN_IDX46,
    DEV_CHN_IDX47,
    DEV_CHN_IDX48,
    DEV_CHN_IDX49,
    DEV_CHN_IDX50,
    DEV_CHN_IDX51,
    DEV_CHN_IDX52,
    DEV_CHN_IDX53,
    DEV_CHN_IDX54,
    DEV_CHN_IDX55,
    DEV_CHN_IDX56,
    DEV_CHN_IDX57,
    DEV_CHN_IDX58,
    DEV_CHN_IDX59,
    DEV_CHN_IDX60,
    DEV_CHN_IDX61,
    DEV_CHN_IDX62,
    DEV_CHN_IDX63,
    DEV_CHN_IDX_BUTT
} DEV_CHN_IDX_E;

/* 芯片目标ID */
typedef enum {
    TGID_MASTER = 0, /* 主片 */
    TGID_SLAVE_ONE,  /* 从片1 */
    TGID_SLAVE_TWO,  /* 从片2 */
    TGID_SLAVE_THR,  /* 从片3 */
    TGID_MAX
} TGID_E;

typedef enum {
    CHIP_NUM_ONE = 1, /* 当前芯片个数为1 */
    CHIP_NUM_TWO,     /* 当前芯片个数为2 */
    CHIP_NUM_THR,     /* 当前芯片个数为3 */
    CHIP_NUM_FOUR,    /* 当前芯片个数为4 */
    CHIP_NUM_MAX
} CHIP_NUM_E;

typedef enum {
    SLAVECHIP_NUM_ZERO = 0, /* 当前从片个数为0 */
    SLAVECHIP_NUM_ONE,      /* 当前从片个数为1 */
    SLAVECHIP_NUM_TWO,      /* 当前从片个数为2 */
    SLAVECHIP_NUM_THR,      /* 当前从片个数为3 */
    SLAVECHIP_NUM_FOUR,     /* 当前从片个数为4 */
    SLAVECHIP_NUM_MAX
} SLAVECHIP_NUM_E;

typedef enum {
    VIDEO_CMDTYPE_USERGET = 0, /* 用户层GET命令,调用外部接口 */
    VIDEO_CMDTYPE_CHIPGET,     /* 芯片层GET命令,调用内部接口 */
    VIDEO_CMDTYPE_USERSET,     /* 用户层SET命令,调用外部接口 */
    VIDEO_CMDTYPE_CHIPSET,     /* 芯片层SET命令,调用内部接口 */
    VIDEO_CMDTYPE_BUTT,
} VIDEO_CMDTYPE_E;

typedef enum {
    VENC_H264E_MODE_1X = 1,
    VENC_H264E_MODE_2X = 2,
    VENC_H264E_MODE_4X = 5,
    VENC_H264E_MODE_BUTT,
} VENC_ATTR_H264_MODE_E;

typedef enum {
    VIDEO_PIXEL_FORMAT_RGB_1BPP = 0,
    VIDEO_PIXEL_FORMAT_RGB_2BPP,
    VIDEO_PIXEL_FORMAT_RGB_4BPP,
    VIDEO_PIXEL_FORMAT_RGB_8BPP,
    VIDEO_PIXEL_FORMAT_RGB_444,
    VIDEO_PIXEL_FORMAT_RGB_4444,
    VIDEO_PIXEL_FORMAT_RGB_555,
    VIDEO_PIXEL_FORMAT_RGB_565,
    VIDEO_PIXEL_FORMAT_RGB_1555,

    /* 9 Reserved */
    VIDEO_PIXEL_FORMAT_RGB_888,
    VIDEO_PIXEL_FORMAT_RGB_8888,
    VIDEO_PIXEL_FORMAT_RGB_PLANAR_888,
    VIDEO_PIXEL_FORMAT_RGB_BAYER,

    VIDEO_PIXEL_FORMAT_YUV_A422,
    VIDEO_PIXEL_FORMAT_YUV_A444,

    VIDEO_PIXEL_FORMAT_YUV_PLANAR_422,
    VIDEO_PIXEL_FORMAT_YUV_PLANAR_420,
    VIDEO_PIXEL_FORMAT_YUV_PLANAR_444,

    VIDEO_PIXEL_FORMAT_YUV_SEMIPLANAR_422,
    VIDEO_PIXEL_FORMAT_YUV_SEMIPLANAR_420,
    VIDEO_PIXEL_FORMAT_YUV_SEMIPLANAR_444,

    VIDEO_PIXEL_FORMAT_UYVY_PACKAGE_422,
    VIDEO_PIXEL_FORMAT_YUYV_PACKAGE_422,
    VIDEO_PIXEL_FORMAT_VYUY_PACKAGE_422,
    VIDEO_PIXEL_FORMAT_YCbCr_PLANAR,

    VIDEO_PIXEL_FORMAT_BUTT
} VIDEO_PIXEL_FORMAT_E;

typedef enum {
    VIDEO_SCENE_STANDARD = 0, /* 标准 */
    VIDEO_SCENE_INDOOR,       /* 室内 */
    VIDEO_SCENE_OUTDOOR,      /* 室外 */
    VIDEO_SCENE_WEAK_LIGHT,   /* 弱光 */
    VIDEO_SCENE_BUTT
} VIDEO_SCENE_E;

typedef struct {
    UINT8 bright;     /* 亮度(0-255) */
    UINT8 contrast;   /* 对比度(0-255) */
    UINT8 saturation; /* 饱和度(0-255) */
    UINT8 hue;        /* 色度(0-255) */
    UINT8 gain;       /* DAC增益(0-255) */
    UINT8 Resvd[3];   /* 保留位 */
} VIDEO_PARAM_S, *PVIDEO_PARAM_S;
#define VIDEO_PARAM_S_LEN sizeof(VIDEO_PARAM_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(VIDEO_PARAM_S, 8);

typedef struct {
    UINT8 Chn2mpCnt; /* 本地通道<=200W的路数总和 */
    UINT8 Chn3mpCnt; /* 本地通道300W的路数总和 */
    UINT8 Chn4mpCnt; /* 本地通道400W的路数总和 */
    UINT8 Chn5mpCnt; /* 本地通道500W的路数总和 */
} CHN_SRC_RES_INFO_S, *PCHN_SRC_RES_INFO_S;
#define CHN_SRC_RES_INFO_S_LEN sizeof(CHN_SRC_RES_INFO_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(CHN_SRC_RES_INFO_S, 4);

typedef struct {
    UINT8 Dec2mpCnt;  /* 解码通道200W的路数总和 */
    UINT8 Dec3mpCnt;  /* 解码通道300W的路数总和 */
    UINT8 Dec4mpCnt;  /* 解码通道400W的路数总和 */
    UINT8 Dec5mpCnt;  /* 解码通道500W的路数总和 */
    UINT8 Dec6mpCnt;  /* 解码通道600W的路数总和 */
    UINT8 Dec8mpCnt;  /* 解码通道800W的路数总和 */
    UINT8 Dec12mpCnt; /* 解码通道1200W的路数总和 */
    UINT8 Resvd;      /* 保留位 */
} DEC_SRC_RES_INFO_S, *PDEC_SRC_RES_INFO_S;
#define DEC_SRC_RES_INFO_S_LEN sizeof(DEC_SRC_RES_INFO_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(DEC_SRC_RES_INFO_S, 8);

typedef struct {
    UINT32 DecCapability;          /* 最大支持的解码能力 */
    DEC_SRC_RES_INFO_S SrcResInfo; /* 各分辨率支持的最大主流路数 */
} DEC_CAP_RES_INFO_S, *PDEC_CAP_RES_INFO_S;
#define DEC_CAP_RES_INFO_S_LEN sizeof(DEC_CAP_RES_INFO_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(DEC_CAP_RES_INFO_S, 4 + DEC_SRC_RES_INFO_S_LEN);

/* VI通道类型 */
typedef enum {
    VI_CHNTYPE_VIU,  /* 本地VI通道 */
    VI_CHNTYPE_PCIV, /* PCI VI通道 */
    VI_CHNTYPE_CAS,  /* 级联通道 */
} VI_CHNTYPE_E;

typedef struct {
    UINT8 enable;
    UINT8 Resvd[3]; /* 保留位 */
    SINT32 ChnMapTbl[MAX_VENC_LOC_CHN_NUMS];
} VI_PHY_MAP_CFG_S, *PVI_PHY_MAP_CFG_S;
#define VI_PHY_MAP_CFG_S_LEN sizeof(VI_PHY_MAP_CFG_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(VI_PHY_MAP_CFG_S, 4 + sizeof(SINT32) * MAX_VENC_LOC_CHN_NUMS);

typedef enum {
    VISP_SENSOR_NUM_NONE = 0,
    VISP_SENSOR_NUM_ONE,
    VISP_SENSOR_NUM_TWO,
    VISP_SENSOR_NUM_BUTT
} VISP_SENSOR_NUM_E;
#define MAX_VISP_SENSOR_NUM (VISP_SENSOR_NUM_BUTT - 1)

typedef enum { VISP_SENSOR_MODE_RGB, VISP_SENSOR_MODE_IR, VISP_SENSOR_MODE_BUTT } VISP_SENSOR_MODE_E;

typedef enum {
    VISP_SENSOR_SC200AI_MIPI_2M_10BIT_30FPS,
    VISP_SENSOR_SC401AI_MIPI_4M_10BIT_30FPS,
    VISP_SENSOR_SC401AI_MIPI_4M_10BIT_30FPS_WDR2TO1,
    VISP_SENSOR_OS04D10_MIPI_4M_30FPS_10BIT,
    VISP_SENSOR_JXK362P_MIPI_6M_30FPS_10BIT,
    VISP_SENSOR_OS03A10_MIPI_4M_30FPS_12BIT,
    VISP_SENSOR_GC2083_MIPI_2M_30FPS_10BIT,
    VISP_SENSOR_JXK308P_MIPI_8M_30FPS_10BIT,
    VISP_SENSOR_JXF38P_MIPI_2M_30FPS_10BIT,
    VISP_SENSOR_IMX415_MIPI_8M_30FPS_10BIT,
    VISP_SENSOR_JXQ03_MIPI_3M_30FPS_10BIT,
    VISP_SENSOR_GC20B3_MIPI_2M_30FPS_10BIT,
    VISP_SENSOR_OS04L10_MIPI_4M_30FPS_10BIT,
    VISP_SENSOR_IMX335_MIPI_5M_30FPS_10BIT,
	VISP_SENSOR_JXK308P_MIPI_2M_30FPS_10BIT,
	VISP_SENSOR_OS03A10_MIPI_4M_30FPS_12BIT_2LANE,
	VISP_SENSOR_IMX335_MIPI_2M_60FPS_10BIT_4LANE,
	VISP_SENSOR_IMX335_MIPI_5M_30FPS_10BIT_WDR2TO1_4LANE,
	VISP_SENSOR_IMX335_MIPI_5M_30FPS_10BIT_4LANE,
	VISP_SENSOR_OS08E10_MIPI_8M_20FPS_10BIT_4LANE,
	VISP_SENSOR_OS08E10_MIPI_6M_30FPS_10BIT_4LANE,
	VISP_SENSOR_JXF38PA_MIPI_2M_30FPS_10BIT,
    VISP_SENSOR_OS08E10_MIPI_8M_20FPS_10BIT_2LANE,
	VISP_SENSOR_OS08E10_MIPI_6M_30FPS_10BIT_2LANE,
    VISP_SENSOR_JXK06A_MIPI_4M_30FPS_10BIT_2LANE,
    VISP_SENSOR_GC20C3_MIPI_2M_30FPS_10BIT_2LANE,
    VISP_SENSOR_IMX415_MIPI_8M_30FPS_10BIT_4LANE,
    VISP_SENSOR_CS6286_MIPI_8M_30FPS_12BIT_4LANE,
    VISP_SENSOR_CS6286_MIPI_8M_30FPS_12BIT_DCG_4LANE,
    VISP_SENSOR_GC4053_MIPI_4M_30FPS_10BIT_2LANE,
    VISP_SENSOR_GC5623_MIPI_5M_30FPS_10BIT_4LANE,
    VISP_SENSOR_TYPE_BUTT
} VISP_SENSOR_TYPE_E;

typedef enum {
    VISP_TARGET_OBJ_LUM_UNRELIABLE = 0, /* 不靠谱 */
    VISP_TARGET_OBJ_LUM_MAX = 255       /* (0, 255]: 合理值 */
} VISP_TARGET_OBJ_LUM_E;

typedef enum {
    VISP_TARGET_OBJ_AGE_UNRELIABLE = 0, /* 不靠谱 */
    VISP_TARGET_OBJ_AGE_OTHER = 1       /* >0: 合理值 */
} VISP_TARGET_OBJ_AGE_E;

typedef enum {
    VISP_TARGET_OBJ_BLUR_UNRELIABLE = -1, /* 不靠谱 */
    VISP_TARGET_OBJ_BLUR_OTHER = 0        /* 其它值: 合理的 */
} VISP_TARGET_OBJ_BLUR_E;

typedef enum {
    VISP_TARGET_OBJ_WDR_UNKNOWN = -1, /* 未知 */
    VISP_TARGET_OBJ_WDR_NOEXIST = 0,  /* 不存在 */
    VISP_TARGET_OBJ_WDR_EXIST = 1     /* 存在 */
} VISP_TARGET_OBJ_WDR_E;

typedef enum {
    VISP_LENS_ANGLE_12 = 0, /* 12MM焦距镜头 */
    VISP_LENS_ANGLE_8 = 1,  /* 8MM焦距镜头 */
    VISP_LENS_ANGLE_6 = 2,  /* 6MM焦距镜头 */
    VISP_LENS_ANGLE_BUTT
} VISP_LENS_ANGLE_E;

typedef struct {
    UINT8 Type;   /* 图像传感器类型(VISP_SENSOR_TYPE_BUTT) */
    UINT8 Mode;   /* 图像传感器模式(VISP_SENSOR_MODE_E) */
    UINT8 BusId;  /* 图像传感器I2C地址 */
    UINT8 LensId; /* 镜头类型(VISP_LENS_ANGLE_E)*/
} VISP_SENSOR_CONF_S, *PVISP_SENSOR_CONF_S;
#define VISP_SENSOR_CONF_S_LEN sizeof(VISP_SENSOR_CONF_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(VISP_SENSOR_CONF_S, 4);

/* 场景中目标对象的状态 */
typedef struct {
    UINT32 TrackId; /* 目标的ID号 */
    UINT32 Tick;    /* 单位为毫秒 */
    XRS_RECT_S Box; /* 目标对象的坐标 */
    UINT16 Lum;     /* 目标对象的亮度(VISP_TARGET_OBJ_LUM_E) */
    UINT16 Age;     /* 目标对象的年龄(VISP_TARGET_OBJ_AGE_E) */
    SINT32 Blur;    /* 清晰度指标(VISP_TARGET_OBJ_BLUR_E) */
    SINT32 bWdr;    /* 当前是否存在宽动态场景(VISP_TARGET_OBJ_WDR_E) */
} VISP_TARGET_OBJ_INFO_S, *PVISP_TARGET_OBJ_INFO_S;
#define VISP_TARGET_OBJ_INFO_S_LEN sizeof(VISP_TARGET_OBJ_INFO_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(VISP_TARGET_OBJ_INFO_S, 8 + XRS_RECT_S_LEN + 12);

typedef struct {
    UINT32 LumMin;   /* 目标对象的期望亮度的最小值 */
    SINT32 LumMax;   /* 目标对象的期望亮度的最大值 */
    SINT32 WidthMin; /* ISP最小联动宽度,只有目标对象宽度大于该值时才会开始做ISP联动;该值对应最远开门距离 */
} VISP_OBJ_DETECT_PARAM_S, *PVISP_OBJ_DETECT_PARAM_S;
#define VISP_OBJ_DETECT_PARAM_S_LEN sizeof(VISP_OBJ_DETECT_PARAM_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(VISP_OBJ_DETECT_PARAM_S, 12);

typedef struct {
    UINT8 bEnble;        /* 用户指定buffer分配个数使能 */
    UINT8 ViCapBufNum;   /* raw采集vb个数，参数3个;如果是HDR则参考6个 */
    UINT8 NpuBufNum;     /* AI-ISP raw16 vb个数，参数3个 */
    UINT8 MStreamBufNum; /* 主码流buffer个数, 参考3个 (VCAP/VPROC/VENC) */
    UINT8 SStreamBufNum; /* 子码流buffer个数, 参考3个 (VCAP/VPROC/VENC) */
    UINT8 AlgBufNum; /* 算法buffer个数, 参考2个 (VPROC/ALG)；如果使用底层vb缓存，则++缓存块个数 */
    UINT8 VdaBufNum; /* VDA buffer个数, 参考2个 (VPROC/VDA) */
    UINT8 Resvd;     /* 保留位 */
} MEDIA_BUFFER_S, *PMEDIA_BUFFER_S;
#define MEDIA_BUFFER_S_LEN sizeof(MEDIA_BUFFER_S)

typedef struct {
    UINT8 VMode;                                    /* 视频制式(VI_MODE_E) */
    UINT8 VForm;                                    /* 视频输入格式(VI_FORMAT_E) */
    UINT8 VSize;                                    /* 视频编码分辨率(VIDEO_PIC_SIZE_E) */
    UINT8 LType;                                    /* 语言类型(LANG_TYPE_E) */
    UINT8 MaxViChn;                                 /* VI最大通道数 */
    UINT8 MultiConf;                                /* VI设备AD复用模式配置(VI_MULTI_MODE_E) */
    UINT8 MaxSnsNum;                                /* 图像传感器最大数(VISP_SENSOR_NUM_E) */
    UINT8 RealTimeMode;                             /* 实时模式(1-实时;0-非实时) */
    VI_PHY_MAP_CFG_S UsrViPhyMap;                   /* 物理通道映射配置*/
    CHN_SRC_RES_INFO_S SrcResInfo;                  /* 设备接入本地分辨率的类型 */
    char VideoLocNoSglPath[VIDEO_FILENAME_MAX_LEN]; /* 本地无信号视频图片路径 */
    VISP_SENSOR_CONF_S SnsConf[MAX_VISP_SENSOR_NUM];
    MEDIA_BUFFER_S MediaBuf; /* 公共vb个数 */
} VI_CONF_S, *PVI_CONF_S;
#define VI_CONF_S_LEN sizeof(VI_CONF_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(VI_CONF_S, 8 + VI_PHY_MAP_CFG_S_LEN + CHN_SRC_RES_INFO_S_LEN + VIDEO_FILENAME_MAX_LEN +
                                            VISP_SENSOR_CONF_S_LEN * MAX_VISP_SENSOR_NUM + MEDIA_BUFFER_S_LEN);

typedef struct {
    UINT8 enable;   /* 是否使能(MV_BOOL) */
    UINT8 level;    /* 去噪级别(0-5) */
    UINT8 Resvd[2]; /* 保留位 */
} VI_NRPARAM_S, *PVI_NRPARAM_S;
#define VI_NRPARAM_S_LEN sizeof(VI_NRPARAM_S)

/* 区域放大类型 */
typedef enum {
    ZOOM_TYPE_LIVE = 0,  /* 实时预览区域放大*/
    ZOOM_TYPE_PLAYBACK,  /* 回放区域放大*/
    ZOOM_TYPE_PB_PAUSE,  /* 回放暂停区域放大*/
    ZOOM_TYPE_OVERLAYER, /* 悬浮窗口方式区域放大*/
    ZOOM_TYPE_BUTT
} ZOOM_TYPE_E;

typedef enum {
    VO_SWITCH_LIVE = 0, /* VO实时切屏操作*/
    VO_SWITCH_PLAYBACK, /* VO回放切屏操作*/
    VO_SWITCH_BLANK,    /* 切换到黑屏*/
    VO_SWITCH_BUTT
} VO_SWITCH_MODE_E;

/* VO通道类型 */
typedef enum {
    VO_CHNTYPE_NONE = -1, /* 无效的类型 */
    VO_CHNTYPE_VIU = 0,   /* 本地输入通道 */
    VO_CHNTYPE_PCIVOUT,   /* PCIV输出VO通道 */
    VO_CHNTYPE_PCIVIN,    /* PCIV输入VO通道 */
    VO_CHNTYPE_CASOUT,    /* 级联输出VO通道 */
    VO_CHNTYPE_CASIN,     /* 级联输入VO通道 */
    VO_CHNTYPE_ZOOM,      /* 放大VO通道 */
    VO_CHNTYPE_LIVE,      /* 实时通道 */
    VO_CHNTYPE_PLAYBACK,  /* 回放通道 */
    VO_CHNTYPE_PREVIEW,   /* 回放数据提前预览通道 */
} VO_CHNTYPE_E;

typedef enum {
    VO_OUTFRMRATE_USER = 10,   /* VO视频层输出帧率一半 */
    VO_OUTFRMRATE_HALF = 0xFE, /* VO视频层输出帧率一半 */
    VO_OUTFRMRATE_SAME = 0xFF, /* VO视频层输出帧率与设备帧率一致 */
    VO_OUTFRMRATE_BUTT
} VO_OUTFRMRATE_E;

#define VO_NO_VID_PIC_MAX_WIDTH VIDEO_WIDTH_Q720P
#define VO_NO_VID_PIC_MAX_HEIGHT VIDEO_HEIGHT_Q720P

typedef enum {
    VO_NO_VID_PIC_DISABLE = -1,
    VO_NO_VID_PIC_YUV420 = 0,   /* YUV420图片 */
    VO_NO_VID_PIC_BLACK,        /* 黑屏 */
    VO_NO_VID_PIC_YUV420_LOCAL, /* 本地IPC YUV420图片 */
    VO_NO_VID_PIC_BUTT
} VO_NO_VID_PIC_E;

typedef struct {
    UINT16 x; /* 单位像素(要求2像素对齐) */
    UINT16 y; /* 单位像素(要求2像素对齐) */
    UINT16 w; /* 单位像素(要求2像素对齐) */
    UINT16 h; /* 单位像素(要求2像素对齐) */
} VO_AREA_S, *PVO_AREA_S;
#define VO_AREA_S_LEN sizeof(VO_AREA_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(VO_AREA_S, 8);

typedef struct {
    UINT8 enable; /* 使能(0-取消区域放大,1-启用区域放大) */
    UINT8 Chn;
    UINT8 ChnType;     /* VO操作类型(VO_CHNTYPE_E) */
    UINT8 ZoomType;    /* 区域放大类型(ZOOM_TYPE_E) */
    VO_AREA_S AreaSrc; /* 源区域坐标和宽高(按万分比),仅当pip_mode=PIP_MODE_ZOOMCHN有效 */
    VO_AREA_S AreaDst; /* 目标区域坐标和宽高(按万分比),仅当pip_mode=PIP_MODE_OVERLAYER有效 */
} VO_ZOOM_S, *PVO_ZOOM_S;
#define VO_ZOOM_S_LEN sizeof(VO_ZOOM_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(VO_ZOOM_S, 4 + VO_AREA_S_LEN * 2);

typedef struct {
    UINT8 Screen;                       /* 分割方式(VO_SPLIT_E) */
    UINT8 SwitchMode;                   /* VO切屏模式(VO_SWITCH_MODE_E) */
    UINT8 Resvd[2];                     /* 保留位 */
    VO_AREA_S DispArea;                 /* VO实际显示区域(按万分比) */
    UINT8 SplitIdx[VO_CHN_NUM_PER_DEV]; /* 多画面时,每个位置显示的图像通道(从0开始) */
    VO_AREA_S Area[VO_CHN_NUM_PER_DEV]; /* 多画面时,每个位置显示的图像区域 */
} VO_SCREEN_S, *PVO_SCREEN_S;
#define VO_SCREEN_S_LEN sizeof(VO_SCREEN_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(VO_SCREEN_S, 4 + 8 + VO_CHN_NUM_PER_DEV + 8 * VO_CHN_NUM_PER_DEV);

typedef struct {
    UINT8 VoEnable; /* VO逻辑设备是否启用 */
    UINT8 VoMaxChn; /* VO多画面最多显示VO通道数,如64路机型,这个配置为32,则表示需要分屏显示(不区分HDMI0,HDMI1) */
    UINT8 VoDispFrm;   /* VO设备显示帧率 */
    UINT8 VoIntfSync;  /* VO设备分辨率,接口时序(VO_INTFSYNC_E) */
    UINT16 VoIntfType; /* VO设备视频接口类型(VO_INTFTYPE_E) */
    UINT16 bMipiInit;  /* VO设备是否需要初始化mipi属性 */
    UINT8 LcdType;     /* LCD厂商类型 */
    UINT8 Resvd[3];    /* 保留位 */
} VO_CONF_S, *PVO_CONF_S;
#define VO_CONF_S_LEN sizeof(VO_CONF_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(VO_CONF_S, 12);

typedef struct {
    UINT8 vmode;        /* 视频制式(VI_MODE_E) */
    UINT8 bErrVo;       /* 异常VO显示方案使能*/
    UINT16 VoDevBitTab; /* VO逻辑设备的位表((VO_DEV_BIT_TAB_E)) */
    VO_CONF_S VoConf[MAX_VO_DEV_NUM];
} VO_CONFS_S, *PVO_CONFS_S;
#define VO_CONFS_S_LEN sizeof(VO_CONFS_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(VO_CONFS_S, 4 + VO_CONF_S_LEN * MAX_VO_DEV_NUM);

typedef struct {
    UINT8 ChnType;    /* VO操作类型(VO_CHNTYPE_E) */
    UINT8 Resvd[3];   /* 保留位 */
    VO_AREA_S VoArea; /* x,y起始坐标,w-x方向上的缩量,h-y方向上的缩量 */
} VO_CHN_AREA_S, *PVO_CHN_AREA_S;
#define VO_CHN_AREA_S_LEN sizeof(VO_CHN_AREA_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(VO_CHN_AREA_S, 4 + VO_AREA_S_LEN);

typedef struct {
    UINT8 ChnType;     /* VO操作类型(VO_CHNTYPE_E) */
    UINT8 VoSrcChn;    /* VO源设备通道号 */
    UINT8 enVoPrewChn; /* VO PREVIEW通道是否使能 */
    UINT8 Rotation;    /* 旋转(MV_ROTATION_E) */
    UINT8 CropRatioH;  /* 水平裁减比例 */
    UINT8 CropRatioV;  /* 垂直裁减比例 */
    UINT8 resv[2];     /* 保留字段 */
    UINT16 SrcWidth;   /* VO源设备宽度(只有绑定时有效) */
    UINT16 SrcHeight;  /* VO源设备高度(只有绑定时有效) */
    VO_AREA_S VoArea;  /* VO通道显示区域(只有VO_OPERA_TYPE_PB_PREVIEW时有效) */
} VO_BIND_CONF_S, *PVO_BIND_CONF_S;
#define VO_BIND_CONF_S_LEN sizeof(VO_BIND_CONF_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(VO_BIND_CONF_S, 12 + VO_AREA_S_LEN);

typedef struct {
    UINT8 connected;           /**< Whether the devices are connected. */
    UINT8 support_hdmi;        /**< Whether the HDMI is supported by the device.
                                    If the HDMI is not supported by the device, the device is DVI.*/
    UINT8 support_ycbcr;       /**< Whether the YCBCR display is supported.
                                    HI_TRUE: The YCBCR display is supported.
                                    HI_FALSE: Only red-green-blue (RGB) is supported. */
    UINT8 sink_power_on;       /**< Whether the sink device is powered on. */
    UINT8 real_edid;           /**< Whether the EDID obtains the flag from the sink device.
                                    HI_TRUE: The EDID information is correctly read.
                                    HI_FASE: default settings */
    UINT8 native_video_format; /**< Physical resolution of the display device(HDMI_VIDEO_FMT_E). */
    UINT8 video_format_supported[HDMI_VIDEO_FMT_BUTT];
    /**< Video capability set.
         HI_TRUE: This display format is supported.
         HI_FALSE: This display format is not supported. */
    UINT32 max_pcm_channels; /**< Maximum pulse code modulation (PCM) channel number of the audio. */
    UINT8 audio_format_supported[HDMI_MAX_AUDIO_CAP_COUNT];
    /**< Audio capability set. For details, see Table 37 in EIA-CEA-861-D.
         HI_TRUE: This display format is supported.
         HI_FALSE: This display format is not supported. */
    UINT8 speaker; /**< Speaker position. For details,
                        see the definition of SpeakerDATABlock in EIA-CEA-861-D. */
    UINT32 audio_sample_rate_supported[HDMI_MAX_AUDIO_SMPRATE_COUNT];
    /**< Audio sampling rate capability set.
         The value 0 is invalid. Other values are the supported audio sampling rates. */
    UINT8 manufacture_name[4];  /**< Device vendor flag */
    UINT32 product_code;        /**< Device ID. */
    UINT32 serial_number;       /**< Device sequence number. */
    UINT32 week_of_manufacture; /**< Device production data (week). */
    UINT32 year_of_manufacture; /**< Set the production data (year). */
    UINT8 version;              /**< Device version number */
    UINT8 revision;             /**< Device sub version number */
    UINT8 Resvd[2];             /**< resversed */
} VO_HDMI_SINK_CAPABILITY_S, *PVO_HDMI_SINK_CAPABILITY_S;
#define VO_HDMI_SINK_CAPABILITY_S_LEN sizeof(VO_HDMI_SINK_CAPABILITY_S)

/* 编码参数设置的来源 */
typedef enum {
    VENC_PARA_FROM_EXT = 0, /* 编码设置参数由视频模块之外的模块设置标志 */
    VENC_PARA_FROM_INT,     /* 编码设置参数由视频模块内部设置标志 */
    VENC_PARA_FROM_BUTT
} VENC_PARA_SOURCE_E;

typedef enum {
    VSNAP_TYPE_VENC_LIVE = 0, /* 编码抓拍 */
    VSNAP_TYPE_VDEC_LIVE,     /* 解码抓拍 */
    VSNAP_TYPE_VDEC_PLAYBACK, /* 回放抓拍 */
    VSNAP_TYPE_BUTT
} VSNAP_TYPE_E;

/* VENC通道类型 */
typedef enum {
    VPSS_CHNTYPE_MAIN = 0x00, /* 主通道 */
    VPSS_CHNTYPE_MINOR,       /* 次通道 */
    VPSS_CHNTYPE_THIRD,       /* 第三编码流 */
    VPSS_CHNTYPE_SNAP,        /* 抓拍编码通道 */
    VPSS_CHNTYPE_AI,          /* AI通道 */
    VPSS_CHNTYPE_MAIN_SEC,        /* 子板主通道 */
    VPSS_CHNTYPE_MINOR_SEC,       /* 子板次通道 */
    VPSS_CHNTYPE_SNAP_SEC,        /* 子板抓拍编码通道 */
    VPSS_CHNTYPE_AI_SEC,          /* 子板AI通道 */
    VPSS_CHNTYPE_BUTT
} VPSS_CHNTYPE_E;

typedef enum {
    VENC_SNAP_CONT = 0, /* 连续抓拍 */
    VENC_SNAP_ONCE,     /* 单张抓拍 */
    VENC_SNAP_REALTIME,
    VENC_SNAP_CROP_JPEG,
    VENC_SNAP_BUTT
} VENC_SNAP_TYPT_E;

/* VENC通道类型 */
typedef enum {
    VENC_CHNTYPE_MAIN = 0x00, /* 主编码通道 */
    VENC_CHNTYPE_MINOR,       /* 次编码通道 */
    VENC_CHNTYPE_THIRD,       /* 第三编码流 */
    VENC_CHNTYPE_SNAP,        /* 抓拍编码通道 */
    VENC_CHNTYPE_2JPEG,       /* 视频帧信息转化成JPEG图片 */
    VENC_CHNTYPE_VDEC_SNAP,   /* 解码抓拍编码通道 */
    VENC_CHNTYPE_CROP_JPEG,   /* 视频帧信息抠图转化成JPEG图片 */
    VENC_CHNTYPE_MAIN_SEC,    /* 子板主编码通道 */
    VENC_CHNTYPE_MINOR_SEC,   /* 子板次编码通道 */
    VENC_CHNTYPE_THIRD_SEC,   /* 子板第三编码流 */
    VENC_CHNTYPE_SNAP_SEC,    /* 子板抓拍编码通道 */
    VENC_CHNTYPE_BUTT
} VENC_CHNTYPE_E;
#define MAX_VENC_CHNTYPE VENC_CHNTYPE_BUTT
#define MAX_VENC_STREAM_NUMS VENC_CHNTYPE_THIRD

#define MAX_VENC_SNAP_CHN_NUMS 2              /* 最大支持抓拍编码通道数 */
#define MAX_VENC_CROP_CHN_NUMS 1              /* 最大支持抠图编码通道数 */
#define MAX_VENC_MULTI_CHN_NUMS 2             /* 复合通道主流,复合通道子流 */
#define MAX_VENC_JPEG_CHN_NUMS VENC_SNAP_BUTT /* 实时抓怕,连续抓拍,单张抓拍 */

typedef enum {
    VENC_MODE_STREAM = 0, /* 流模式,H.264可使用帧模式或流模式 */
    VENC_MODE_FRAME,      /* 帧模式,JPEG,MJPEG必须使用帧模式 */
    VENC_MODE_BUTT
} VENC_MODE_E;

typedef enum {
    VENC_SRC_TYPE_VIU = 0,  /* venc数据来源为VIU */
    VENC_SRC_TYPE_VPSS,     /* venc数据来源为VPSS */
    VENC_SRC_TYPE_PLAYBACK, /* venc数据来源为回放 */
    VENC_SRC_TYPE_BUTT
} VENC_SRC_TYPE_E;

typedef struct {
    UINT8 ChnType;     /* VENC操作类型(VENC_CHNTYPE_E) */
    UINT8 VencSrcType; /* VENC数据来源类型(VENC_SRC_TYPE_E) */
    UINT8 VencSrcGrp;  /* VENC源设备通道号 */
    UINT8 VencSrcChn;  /* VENC源设备通道号 */
    UINT8 CropRatioH;  /* 水平裁减比例 */
    UINT8 CropRatioV;  /* 垂直裁减比例 */
    UINT8 bOffLine; /*是否绑定到前端vpss: 0表示link; 1表示离线，使用Vpss_Chn_Get_Frm取帧，然后Venc_Chn_Send_Frm*/
    UINT8 Resvd[1];
    UINT16 SrcWidth;  /* VENC源设备宽度(只有绑定时有效) */
    UINT16 SrcHeight; /* VENC源设备高度(只有绑定时有效) */
} VENC_BIND_CONF_S, *PVENC_BIND_CONF_S;
#define VENC_BIND_CONF_S_LEN sizeof(VENC_BIND_CONF_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(VENC_BIND_CONF_S, 12);

typedef enum {
    VENC_DEAL_BLOCKING = -1, /* 阻塞模式 */
    VENC_DEAL_NONBLOCK = 0,  /* 非阻塞模式 */
    VENC_DEAL_TIMEOUT = 40,  /* 超时模式(默认是为40毫秒,此值可自指定) */
} VENC_DEAL_MODE_E;

/* 编码画质等级 */
typedef enum {
    VENC_QP0 = 0, /* 最高画质 */
    VENC_QP1,
    VENC_QP2,
    VENC_QP3,
    VENC_QP4,
    VENC_QP5,
    VENC_MAX
} VENC_QP_E;

typedef struct {
    UINT8 bEnbaleQp;           /* 使能用户配置QP，默认内部赋初值 */
    UINT8 u8MaxPictureQuality; /* 最大图像质量(范围：20-50) */
    UINT16 u16MaxPictureSize;  /* 最大帧大小(单位：KB) */
    UINT32 u32MaxQp;           /* [0,51] */
    UINT32 u32MinQp;           /* [0,51] */
    UINT32 u32MaxIQp;          /* [0,51] ，mjpeg cbr码控模式时，范围为[1,99],越大质量越好*/
    UINT32 u32MinIQp;          /* [0,51] ，mjpeg cbr码控模式时，范围为[1,99],越大质量越好*/
} VENC_QP_CONF_S, *PVENC_QP_CONF_S;
#define VENC_QP_CONF_S_LEN sizeof(VENC_QP_CONF_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(VENC_QP_CONF_S, 20);

typedef struct {
    UINT8 bEnbaleQp;           /* 使能用户配置QP，默认内部赋初值 */
    UINT8 u8MaxPictureQuality; /* 最大图像质量(范围：20-50) */
    UINT16 u16MaxPictureSize;  /* 最大帧大小(单位：KB) */
    UINT32 u32MaxQp;           /* [0,51] */
    UINT32 u32MinQp;           /* [0,51] */
    UINT32 u32MaxIQp;          /* [0,51] */
    UINT32 u32MinIQp;          /* [0,51] */
    UINT32 u32MinQpDelta;      /* [0,4] */
	
	UINT8 u8ChgPos;
	UINT8 u8MaxIProportion;
	UINT8 u8MinIProportion;
	UINT8 u8MinStillPercent;
	UINT8 u8MaxStillQp;
	UINT8 u8RowQpDelta;
	SINT8 u8IpQpDelta;			/* [-10,30] */
    UINT8 u8SceneMode;			/*0-default;1-high scene;2-middle scene;3-PTZ mode(V010需更新PTZ patch支持；V020默认支持)*/	
    UINT8 u8MaxViQp;          	/* [0,51] 虚拟I帧最大QP，smart p模式生效*/
    UINT8 u8MinViQp;          	/* [0,51] 虚拟I帧最小QP，smart p模式生效*/
	SINT8 u8ViQpDelta;			/* [-10,30],虚拟I帧相对于普通P帧的QP差值，smart p模式生效*/
	UINT8 u8ClearStats;			/* 设置新的通道码率后，是否清除码率控制的统计信息，默认值为1。
									0：设置通道码率后不清除RC的帧率和码率统计信息；
									1：设置通道码率后清除RC的帧率和码率统计信息；*/
    UINT8 u8MotionSensitivity;  /*[0,100] 动态灵敏度,默认值100*/
    UINT8 Resvd[3];		   /* 保留位 */
} VENC_QP_CONF_V2_S, *PVENC_QP_CONF_V2_S;
#define VENC_QP_CONF_V2_S_LEN sizeof(VENC_QP_CONF_V2_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(VENC_QP_CONF_V2_S, 40);

typedef enum {
    VENC_MJPEG_FOLLOW_MAIN = 0, /* 跟随主码流 */
    VENC_MJPEG_FOLLOW_MINOR,    /* 跟随子码流 */
    VENC_MJPEG_FOLLOW_BUTT
} VENC_MJPEG_FOLLOW_TYPE_E;

typedef struct {
    UINT8 VPayloadType; /* 编码类型(VPAYLOAD_TYPE_E) */
    UINT8 FrameRate;    /* 视频帧率(VIDEO_FPS_E) */
    SINT8 Quality;      /* 视频质量(范围1-99，越大质量越好) */
    UINT8 BitRateType;  /* 码流类型(BITRATE_TYPE_E),抓拍模式保留参数 */
    UINT16 width;       /* 编码通道宽度 */
    UINT16 height;      /* 编码通道高度 */
    UINT16 BitRate;     /* 16K-40M(Kbps为单位),抓拍模式保留参数 */
    UINT16 KeyInterval; /* 关键帧间隔0-1000,抓拍模式保留参数 */
    UINT32 bOffLine; /*是否绑定到前端vpss: 0表示link; 1表示离线，使用Vpss_Chn_Get_Frm取帧，然后Venc_Chn_Send_Frm*/
    VENC_QP_CONF_S stVencQpConf;
    UINT32 MaxBitRate; /* 16K-40M(Kbps为单位), 最大码率值，补充变码率控制模式下对最大码率控制*/
    UINT8 SmartEncEnable;  /* 智能编码开关：压缩码率.海思平台复用为gop模式（0-normal_p(default);1-smart_p）*/
    UINT8 MjpegFollowType; /* mjpeg流跟踪主子码流类型(VENC_MJPEG_FOLLOW_TYPE_E)*/
	UINT16 BgInterval;	/*长期参考帧（实际I帧）间隔，smart_p模式下生效，需配置为gop的整数倍*/
    UINT32 StreamBufSize; /* 编码流缓存buf大小(单位字节)，需与visp平台设置中编码流参数保持相同 */
} VENC_ENCODEPARAM_S, *PVENC_ENCODEPARAM_S;
#define VENC_ENCODEPARAM_S_LEN sizeof(VENC_ENCODEPARAM_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(VENC_ENCODEPARAM_S, 28 + VENC_QP_CONF_S_LEN);

typedef struct {
    UINT8 enable;     /* 是否开启复合通道编码(MV_BOOL) */
    UINT8 VSize;      /* 视频编码分辨率(VIDEO_PIC_SIZE_E) */
    UINT8 MaxFrmRate; /* 编码最大支持帧率 */
    UINT8 Resvd;      /* 保留位 */
} VENC_CONF_CHN_S, *PVENC_CONF_CHN_S;
#define VENC_CONF_CHN_S_LEN sizeof(VENC_CONF_CHN_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(VENC_CONF_CHN_S, 4);

typedef struct {
    UINT8 VMode;              /* 视频制式(VI_MODE_E) */
    UINT8 MaxVencNum;         /* 最大支持的编码通道数 */
    UINT8 MaxSnapNum;         /* 最大支持的抓拍通道数 */
    UINT8 MaxCropNum;         /* 最大支持的抠图通道数 */
    VENC_CONF_CHN_S MultiChn; /* 复合通道编码参数 */
} VENC_CONF_S, *PVENC_CONF_S;
#define VENC_CONF_S_LEN sizeof(VENC_CONF_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(VENC_CONF_S, 4 + VENC_CONF_CHN_S_LEN);

typedef struct {
    UINT8 TgId;     /* 芯片目标ID(TGID_E) */
    UINT8 ChipNum;  /* 芯片个数(CHIP_NUM_E) */
    UINT8 Resvd[2]; /* 保留位 */
} VIDEO_CHIP_CONF_S, *PVIDEO_CHIP_CONF_S;
#define VIDEO_CHIP_CONF_S_LEN sizeof(VIDEO_CHIP_CONF_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(VIDEO_CHIP_CONF_S, 4);

typedef enum {
    MV_HDMI_EVENT_NO_PLUG, /* <HDMI cable disconnection event */
    MV_HDMI_EVENT_HOTPLUG, /* <HDMI hot-plug event */
} MV_HDMI_EVENT_TYPE_E;

typedef enum {
    MV_SYS_CLK_HIGH_MODE = 0,
    MV_SYS_CLK_HIGH_HOTBALANCE_MODE = 1,
    MV_SYS_CLK_MID_MODE = 2,
    MV_SYS_CLK_MID_HOTBALANCE_MODE = 3,
    MV_SYS_CLK_MAX_BUTT
} MV_SYS_CLK_LEVEL_E;

typedef enum {
    MV_PIXEL_FORMAT_DEFAULT, /* 由原始YUV数据格式来决定输出格式 */

    MV_PIXEL_FORMAT_YVU_NV21,

    MV_PIXEL_FORMAT_LINEAR_YUV_420,
    MV_PIXEL_FORMAT_LINEAR_YUV_422,
    MV_PIXEL_FORMAT_BUTT
} MV_PIXEL_FORMAT_E;

typedef struct {
    UINT8 PixelFmt; /* 视频帧格式(MV_PIXEL_FORMAT_E) */
    UINT8 Resvd[3]; /* 保留位 */
    UINT16 width;   /* 视频帧宽度 */
    UINT16 height;  /* 视频帧高度 */
    char data[];
} VIDEO_YUV_FRAME_S, *PVIDEO_YUV_FRAME_S;
#define VIDEO_YUV_FRAME_S_LEN sizeof(VIDEO_YUV_FRAME_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(VIDEO_YUV_FRAME_S, 8);

typedef struct {
    UINT8 PixelFmt; /* 视频帧格式(MV_PIXEL_FORMAT_E) */
    UINT8 Resvd[3]; /* 保留位 */
    UINT32 width;   /* 视频帧宽度 */
    UINT32 height;  /* 视频帧高度 */
    UINT32 horStride;
    UINT32 verStride;
    void *pData;
} VIDEO_USER_YUV_FRAME_S, *PVIDEO_USER_YUV_FRAME_S;
#define VIDEO_USER_YUV_FRAME_S_LEN sizeof(VIDEO_USER_YUV_FRAME_S)

typedef struct {
    UINT8 PixFmt;  /* 视频帧格式(MV_PIXEL_FORMAT_E) */
    UINT8 Flag[2]; /* 同步取帧标识位 [0 ,0]未检测到人脸释放配对帧 [0, 1]检测到人脸取配对帧 [1 0]取视频帧送检测*/
    UINT8 Resvd[1]; /* 保留位 */
    UINT16 width;   /* 视频帧宽度 */
    UINT16 height;  /* 视频帧高度 */
    BUFF_EXT_S FrmBuff;
} VIDEO_FRM_INFO_S, *PVIDEO_FRM_INFO_S;
#define VIDEO_FRM_INFO_S_LEN sizeof(VIDEO_FRM_INFO_S)
#ifdef POINTER_SIZE_8
ASSERT_SIZEOF_TYPEDEF_STRUCT(VIDEO_FRM_INFO_S, 24);
#else
ASSERT_SIZEOF_TYPEDEF_STRUCT(VIDEO_FRM_INFO_S, 20);
#endif /* POINTER_SIZE_8 */

#define VIDEO_FRM_ALG_S_LEN sizeof(VIDEO_FRM_ALG_S)

/******************************************************************************
 * 函数介绍: 视频数据处理回调
 * 输入参数: stype: 视频流类型(STREAM_TYPE_E)
 *           idx: 通道号,对应前端多个sensor
             pHead: 码流头缓冲区(MSSTD_S)
             Headsize: 码流头长度(MSSTD_S_LEN)
 *           data: 数据接收缓冲区
 *           size: 数据接收缓冲区大小
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
typedef SINT32 (*VData_Callback)(SINT32 stype, SINT32 idx, SINT8 *pHead, SINT32 Headsize, SINT8 *data, SINT32 size);

/******************************************************************************
 * 函数介绍: HDMI热插拔信息处理回调
 * 输入参数: event: HDMI热插拔事件状态
 *           VoDevBitTab: VO逻辑设备位表
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
typedef SINT32 (*Hdmi_Insert_Callback)(MV_HDMI_EVENT_TYPE_E event, VO_DEV_BIT_TAB_E VoDevBitTab);

/******************************************************************************
 * 函数介绍: 视频接入分辨率改变(用于通知SDK层视频分辨率变化)
 * 输入参数: Chn: 通道号
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
typedef SINT32 (*VRes_Modify_Callback)(SINT32 Chn);

/******************************************************************************
 * 函数介绍: 取流通知，调试用
 * 输入参数: Chn: 通道号
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
typedef SINT32 (*VStream_Notify_Callback)(SINT32 Chn, SINT8 *pData);

/*******************************************S***********************************
 * 函数介绍: 设置热平衡模式
 * 输入参数:
 * 输出参数:
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vcomm_Sys_SetLevel(MV_SYS_CLK_LEVEL_E Level);

/******************************************************************************
 * 函数介绍: 获取当前系统PTS
 * 输入参数:
 * 输出参数: pu64CurPTS
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vcomm_Sys_GetPts(UINT64 *pu64CurPTS);

/******************************************************************************
 * 函数功能: 申请媒体层内存
 * 输入参数: size: 内存大小
 * 输出参数: pPhyAddr: 物理地址
 * 返回值  : NULL-申请失败,非0-申请得到的虚拟地址
 * 说明    : phy_addr物理地址值仅在free时用到,请勿对物理内存进行直接操作。
 *****************************************************************************/
void *Vcomm_Mpp_Alloc(UINT64 *pPhyAddr, SINT32 size);

void *Vcomm_Mpp_AllocCached(UINT64 *pPhyAddr, SINT32 size);

void *Vcomm_Mpp_Mmap(UINT64 PhyAddr, UINT32 PhySize);

void *Vcomm_Mpp_MmapCache(UINT64 PhyAddr, UINT32 PhySize);

SINT32 Vcomm_Mpp_Flush(void *pVirAddr, UINT64 PhyAddr, UINT32 PhySize);

SINT32 Vcomm_Mpp_Invalidate(void *pVirAddr, UINT64 PhyAddr, UINT32 PhySize);

SINT32 Vcomm_Mpp_Munmap(void *pVirAddr, UINT32 VirSize);

SINT32 Vcomm_Mpp_PhyCopy(UINT64 SrcPhyAddr, UINT64 DstPhyAddr, UINT64 PhySize);

/******************************************************************************
 * 函数功能: 释放媒体层内存
 * 输入参数: pVirAddr: 申请得到的虚拟地址
 *           PhyAddr: 对应的物理地址值
 * 输出参数: 无
 * 返回值  : 0 - 成功, 非0-失败
 *****************************************************************************/
SINT32 Vcomm_Mpp_Free(void *pVirAddr, UINT64 PhyAddr);

/******************************************************************************
 * 函数介绍: 视频模块功能清理资源退出
 * 输入参数: 无
 * 输出参数: 无
 * 返回值  : 无
 *****************************************************************************/
void Vcomm_Mpp_Exit(void);

#ifdef __cplusplus
}
#endif  //@__cplusplus
#endif  /* _VIDEO_COMM_H_ */
