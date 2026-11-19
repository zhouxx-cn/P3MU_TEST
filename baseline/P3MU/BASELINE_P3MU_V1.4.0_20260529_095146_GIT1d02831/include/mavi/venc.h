#ifndef _VENC_H_
#define _VENC_H_

#ifdef __cplusplus
extern "C" {
#endif  //@__cplusplus
#include "vcomm.h"
#include "xrsstd.h"

#define MAX_EXTOSDATTR_NUM 4
#define MAC_VENC_OSD_TITLE_LEN 32

typedef enum {
    VENC_ERROR_MSSTDOPEN = COMP_ERROR_START, /* 打开MSSTD错误(32) */
    VENC_ERROR_UNINIT,                       /* 模块未初始化33 */
    VENC_ERROR_REINIT,                       /* 模块重复初始化34 */
    VENC_ERROR_CREATEGROUP,                  /* 创建编码通道组错误33 */
    VENC_ERROR_DESTROYROUP,                  /* 销毁编码通道组错误34 */
    VDEC_ERROR_VPSSVENCBIND,                 /* VPSS到VENC绑定错误35 */
    VENC_ERROR_VPSSVENCUNBIND,               /* VPSS到VENC解绑定错误36 */
    VENC_ERROR_REGISTERCHN,                  /* 编码通道REGISTER错误37 */
    VENC_ERROR_UNREGISTERCHN,                /* 编码通道UNREGISTER错误38 */
    VENC_ERROR_QUERYIFRAME,                  /* 获取关键帧错误39 */
    VENC_ERROR_INITALLVENC,                  /* 初始化从片视频编码错误40 */
    VENC_ERROR_EXITALLVENC,                  /* 退出从片视频编码错误41 */
    VENC_ERROR_INITSLAVEPCIV,                /* 初始化从片PCIV错误42 */
    VENC_ERROR_PCIVDATA,                     /* PCI总线上数据出错43 */
    VENC_ERROR_PCIVNODTAT,                   /* PCI总线上无数据44 */
    VENC_ERROR_SETMEMCONFIG,                 /* 设置内存配置错误45 */
    VENC_ERROR_STARTVENC,                    /* 启动编码错误46 */
    VENC_ERROR_VPSSVENCBIND,                 /* VPSS绑定VENC错误47 */
    VENC_ERROR_MEMCONF,                      /* 内存配置错误48 */
    VENC_ERROR_STOPVENC,                     /* 停止编码错误50 */
    VENC_ERROR_CREATECHN,                    /* 创建编码通道错误51 */
    VENC_ERROR_DESTROYCHN,                   /* 销毁编码通道错误52 */
    VENC_ERROR_GETCHNATTR,                   /* 获取编码通道错误53 */
    VENC_ERROR_SDKAPIABN,                    /* sdk encoder接口报错54 */
    VENC_ERROR_INITJPEG,                     /* 初始化JPEG编码错误56 */
    VENC_ERROR_ENCODEJPEG,                   /* 执行JPEG编码错误57 */
    VENC_ERROR_DEINITPJPEG,                  /* 反初始化JPEG编码错误58 */
    VENC_ERROR_INVALIDCHN,                   /* 不支持的通道 */
} VENC_ERROR_E;

#define VENC_TRACE_ERR(str, args...) TRACE_ERR(COMP_VIDEOENC, str, ##args)
#define VENC_TRACE_WARN(str, args...) TRACE_WARN(COMP_VIDEOENC, str, ##args)
#define VENC_TRACE_FLOW(str, args...) TRACE_FLOW(COMP_VIDEOENC, str, ##args)
#define VENC_TRACE_INFO(str, args...) TRACE_INFO(COMP_VIDEOENC, str, ##args)
#define VENC_TRACE_DBG0(str, args...) TRACE_DBG0(COMP_VIDEOENC, str, ##args)

#define VENC_CHECK_NULL_PTR(ptr)                             \
    do {                                                     \
        if (NULL == ptr) {                                   \
            VENC_TRACE_ERR("NULL pointer\n");                \
            return ERRNO(COMMON_ERROR_PARAM, COMP_VIDEOENC); \
            ;                                                \
        }                                                    \
    } while (0)

#define VENC_CHECK_IS_INITED(inited)                        \
    do {                                                    \
        if (0 == inited) {                                  \
            VENC_TRACE_ERR("venc module not inited!\n");    \
            return ERRNO(VENC_ERROR_UNINIT, COMP_VIDEOENC); \
        }                                                   \
    } while (0)

#define VENC_CHECK_IS_REINIT(inited)                        \
    do {                                                    \
        if (1 == inited) {                                  \
            VENC_TRACE_ERR("venc module reinit!\n");        \
            return ERRNO(VENC_ERROR_REINIT, COMP_VIDEOENC); \
        }                                                   \
    } while (0)

typedef enum {
    VENC_ROI_OPPOSITE = 0, /* 相对的 */
    VENC_ROI_ABSOLUTE,     /* 绝对的 */
} VENC_ROI_TYPE;

typedef struct {
    UINT8 RoiType;  /* ROI类型(VENC_ROI_TYPE):VENC_ROI_OPPOSITE(相对QP),VENC_ROI_ABSOLUTE(绝对QP)*/
    SINT8 QpValue;  /* VENC_ROI_ABSOLUTE:[0,51] VENC_ROI_OPPOSITE:[-51,51] */
    UINT8 Resvd[2]; /* 保留位 */
    VRECT_S Rect;
} VENC_ROI_AREA_S, *PVENC_ROI_AREA_S;
#define VENC_ROI_AREA_S_LEN sizeof(VENC_ROI_AREA_S)

typedef struct {
    UINT8 num;                             /* 感兴趣区域的个数,最大4个,0-表示DISABLE感兴趣区域 */
    UINT8 Resvd[3];                        /* 保留位 */
    VENC_ROI_AREA_S Area[MAX_ROIAREA_NUM]; /* 每个区域的坐标 */
} VENC_ROI_PARAM_S, *PVENC_ROI_PARAM_S;
#define VENC_ROI_PARAM_S_LEN sizeof(VENC_ROI_PARAM_S)

/* GUI使用 */
typedef struct {
    UINT8 DateFormat; /* OSD格式0-XXXX-XX-XX年月日,1-XX-XX-XXXX月日年,2-XXXX年XX月XX日,3-XX月XX日XXXX年*/
    UINT8 osdStyle;   /* OSD样式1-透明闪烁,2-透明不闪烁,3-闪烁不透明,4-不透明不闪烁 */
    UINT8 osdAlpha;   /* osd透明度0-不透明,1-透明 */
    UINT8 Resvd;      /* 保留位 */
} VENC_HEAD_OSD_ATTR_S, *PVENC_HEAD_OSD_ATTR_S;
#define VENC_HEAD_OSD_ATTR_S_LEN sizeof(VENC_HEAD_OSD_ATTR_S)

typedef struct {
    UINT16 width;      /* 需抓拍图片宽度 */
    UINT16 height;     /* 需抓拍图片高度 */
    char *pSnapBuff;   /* 抓拍数据缓冲区 */
    UINT16 BuffLen;    /* 抓拍缓冲区长度*/
    UINT16 RealPicLen; /* 实际抓拍数据长度*/
} VENC_SNAP_S, *PVENC_SNAP_S;
#define VENC_SNAP_S_LEN sizeof(VENC_SNAP_S)

typedef struct {
    UINT8 enShowTitle;                   /* 使能显示通道名称 */
    UINT8 enShowDate;                    /* 使能显示时间日期 */
    UINT8 enShowWeek;                    /* 使能显示星期 */
    UINT8 FrameRate;                     /* 编码帧率(VIDEO_FPS_E) */
    VPOINT_S TitlePosi;                  /* 通道名称坐标 */
    VPOINT_S DatePosi;                   /* 时间日期坐标 */
    UINT8 title[MAC_VENC_OSD_TITLE_LEN]; /* 通道名称 */
} VENC_HEAD_OSD_INFO_S, *PVENC_HEAD_OSD_INFO_S;
#define VENC_HEAD_OSD_INFO_S_LEN sizeof(VENC_HEAD_OSD_INFO_S)

typedef struct {
    UINT8 enShowTitle;  /* 使能显示通道名称 */
    UINT8 osdStyle;     /* OSD样式1-透明闪烁,2-透明不闪烁,3-闪烁不透明,4-不透明不闪烁*/
    UINT8 osdAlpha;     /* OSD透明度0-不透明,1-透明 */
    UINT8 Resvd;        /* 保留位 */
    VPOINT_S TitlePosi; /* 通道名称坐标 */
    UINT8 Title[MAC_VENC_OSD_TITLE_LEN]; /* 通道名称 */
} VENC_EXT_OSD_ATTR_S, *PVENC_EXT_OSD_ATTR_S;
#define VENC_EXT_OSD_ATTR_S_LEN sizeof(VENC_EXT_OSD_ATTR_S)

typedef struct {
    VENC_EXT_OSD_ATTR_S ExtAsdAttr[MAX_EXTOSDATTR_NUM];
} VENC_EXT_OSD_INFO_S, *PVENC_EXT_OSD_INFO_S;
#define VENC_EXT_OSD_INFO_S_LEN sizeof(VENC_EXT_OSD_INFO_S)

typedef struct {
    UINT8 ShowChnNum;        /* 需显示的通道数*/
    UINT8 Resvd[3];          /* 保留位 */
    UINT8 Chn[MAX_VIDEO_CH]; /* 复合通道对应位置的通道索引*/
} VENC_MULTI_CHN_INFO_S, *PVENC_MULTI_CHN_INFO_S;
#define VENC_MULTI_CHN_INFO_S_LEN sizeof(VENC_MULTI_CHN_INFO_S)

#ifndef POINTER_SIZE_8
typedef struct {
    UINT16 width;                      /* 编码数据宽度 */
    UINT16 height;                     /* 编码数据高度 */
    UINT8 *pData;                      /* 编码数据缓冲区指针 */
    SINT32 DataSize;                   /* 编码数据长度 */
    UINT8 RectCnt;                     /* 脸框区域个数 */
    UINT8 Resvd[3];                    /* 保留位 */
    VRECT_S Rect[MAX_VFACE_FRAME_NUM]; /* 脸框区域 */
} VENC_CROP_JPEG_DATA_S, *PVENC_CROP_JPEG_DATA_S;
#define VENC_CROP_JPEG_DATA_S_LEN sizeof(VENC_CROP_JPEG_DATA_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(VENC_CROP_JPEG_DATA_S, 16 + VRECT_S_LEN * MAX_VFACE_FRAME_NUM);
#else
typedef struct {
    UINT16 width;                      /* 编码数据宽度 */
    UINT16 height;                     /* 编码数据高度 */
    UINT8 Resvdp[4];                   /* 保留位 */
    UINT8 *pData;                      /* 编码数据缓冲区指针 */
    SINT32 DataSize;                   /* 编码数据长度 */
    UINT8 RectCnt;                     /* 脸框区域个数 */
    UINT8 Resvd[3];                    /* 保留位 */
    VRECT_S Rect[MAX_VFACE_FRAME_NUM]; /* 脸框区域 */
} VENC_CROP_JPEG_DATA_S, *PVENC_CROP_JPEG_DATA_S;
#define VENC_CROP_JPEG_DATA_S_LEN sizeof(VENC_CROP_JPEG_DATA_S)
#endif

#ifndef POINTER_SIZE_8
typedef struct {
    UINT8 *pBuff;                         /* 存放图片的缓冲区指针 */
    SINT32 BuffSize;                      /* 存放图片的缓冲区长度 */
    UINT8 JpegCnt;                        /* 图片个数 */
    UINT8 Resvd[3];                       /* 保留位 */
    SINT32 JpegSize[MAX_VFACE_FRAME_NUM]; /* 图片数据长度 */
} VENC_CROP_JPEG_BUFF_S, *PVENC_CROP_JPEG_BUFF_S;
#define VENC_CROP_JPEG_BUFF_S_LEN sizeof(VENC_CROP_JPEG_BUFF_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(VENC_CROP_JPEG_BUFF_S, 12 + 4 * MAX_VFACE_FRAME_NUM);
#else
typedef struct {
    UINT8 *pBuff;                         /* 存放图片的缓冲区指针 */
    SINT32 BuffSize;                      /* 存放图片的缓冲区长度 */
    UINT8 JpegCnt;                        /* 图片个数 */
    UINT8 Resvd[3];                       /* 保留位 */
    SINT32 JpegSize[MAX_VFACE_FRAME_NUM]; /* 图片数据长度 */
} VENC_CROP_JPEG_BUFF_S, *PVENC_CROP_JPEG_BUFF_S;
#define VENC_CROP_JPEG_BUFF_S_LEN sizeof(VENC_CROP_JPEG_BUFF_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(VENC_CROP_JPEG_BUFF_S, 16 + 4 * MAX_VFACE_FRAME_NUM);
#endif

#ifndef POINTER_SIZE_8
typedef struct {
    UINT8 enReopen; /* 是否需要重新创建编码通道(MV_BOOL) */
    UINT8 Resvd[3]; /* 保留位 */
    VENC_CROP_JPEG_DATA_S Data;
    VENC_CROP_JPEG_BUFF_S Buff;
} VENC_CROP_JPEG_CONF_S, *PVENC_CROP_JPEG_CONF_S;
#define VENC_CROP_JPEG_CONF_S_LEN sizeof(VENC_CROP_JPEG_CONF_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(VENC_CROP_JPEG_CONF_S, 4 + VENC_CROP_JPEG_DATA_S_LEN + VENC_CROP_JPEG_BUFF_S_LEN);
#else
typedef struct {
    UINT8 enReopen; /* 是否需要重新创建编码通道(MV_BOOL) */
    UINT8 Resvd[7]; /* 保留位 */
    VENC_CROP_JPEG_DATA_S Data;
    VENC_CROP_JPEG_BUFF_S Buff;
} VENC_CROP_JPEG_CONF_S, *PVENC_CROP_JPEG_CONF_S;
#define VENC_CROP_JPEG_CONF_S_LEN sizeof(VENC_CROP_JPEG_CONF_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(VENC_CROP_JPEG_CONF_S, 8 + VENC_CROP_JPEG_DATA_S_LEN + VENC_CROP_JPEG_BUFF_S_LEN);
#endif

typedef struct {
    UINT32 u32Width;  /* 宽*/
    UINT32 u32Height; /* 高*/
    UINT32 u32Size;   /* 数据长度*/
    UINT8 *pData;     /* 数据指针*/
} VENC_YUV_INFO_S, *PVENC_YUV_INFO_S;

typedef enum {
	VENC_INTRA_REFRESH_ROW = 0,
	VENC_INTRA_REFRESH_COLUMN,
	VENC_INTRA_REFRESH_BUTT
} VENC_INTRA_REFRESH_MODE_E;

typedef struct {
    XRS_BOOL Enable;
    VENC_INTRA_REFRESH_MODE_E Mode;/*I宏块刷新模式，分为按行刷新和按列刷新*/
    UINT32 RefNum;/*每次I宏块刷新行数或者列数*/
} VENC_INTRA_REFRESH_S, *PVENC_INTRA_REFRESH_S;

#define VENC_MAX_SVC_RECT_NUM  32

typedef enum {
    VENC_SVC_V1 = 0,
    VENC_SVC_V2,
    VENC_SVC_BUTT,
} VENC_SVC_VERSION_E;

typedef enum {
    VENC_SVC_RECT_TYPE0 = 0,
    VENC_SVC_RECT_TYPE1,
    VENC_SVC_RECT_TYPE2,
    VENC_SVC_RECT_TYPE3,
    VENC_SVC_RECT_TYPE4,
    VENC_SVC_RECT_TYPE5,
    VENC_SVC_RECT_TYPE6,
    VENC_SVC_RECT_TYPE7,
    VENC_SVC_RECT_TYPE8,
    VENC_SVC_RECT_TYPE9,
    VENC_SVC_RECT_TYPE10,
    VENC_SVC_RECT_TYPE11,
    VENC_SVC_RECT_TYPE12,
    VENC_SVC_RECT_TYPE13,
    VENC_SVC_RECT_TYPE14,
    VENC_SVC_RECT_TYPE15,
    VENC_SVC_RECT_TYPE_BUTT
} VENC_SVC_RECT_TYPE;

/*【注意事项】
qpmap_value_i，qpmap_value_p取值的意义：
● 当取值为0,1,2,…,31：对应deltaQP:0,1,2,…,31
● 当取值为32,33,34,…,63：对应deltaQP:-32,-31,…,-1
● 当取值为64,65,…,115：H.264 H.265对应绝对QP:0,1,2,…,51
● 当取值为64,65,...，127：SVAC3.0 对应绝对QP:0~63
● 当取值为[115, 127]：H.264 H.265对应绝对QP为51
● 当取值为[128,255]：对应强制skip模式
skipmap_value取值的意义：
● skipmap_value对应左右两个LCU的Skipweight值；
● 高4bit代表左LCU的Skipweight值；
● 低4bit代表右LCU的Skipweight值；
● 建议高4bit与低4bit设定的skipweight一致，且为避免图像出现缺块现象，
skipweight设定值不宜过强。
● I帧时，背景与Activity区域的skipweight为0。
● H.265\SVAC3在CVBR码控下，skipweight倾向性失效；*/
typedef struct {
    UINT8 qpmap_value_i;   /* I帧的QpMap值。取值范围：[0,255] */
    UINT8 qpmap_value_p;   /* P帧的QpMap值。取值范围：[0,255] */
    UINT8 skipmap_value;  /* P帧前景SkipMap值。使用更强的Skip倾向性可以降低码率，但会牺牲质量.取值范围：[0,255],默认0*/
} VENC_SVC_MAP_PARAM_S;


typedef struct {
    UINT32 rect_num; /*智能检测目标个数，取值范围[0,VENC_MAX_SVC_RECT_NUM]*/
    VIDEO_RES_S base_resolution; /*智能目标识别基础分辨率，用于目标框长、宽缩放计算*/
    VRECT_S rect_attr[VENC_MAX_SVC_RECT_NUM]; /*智能检测目标框坐标信息*/
    VENC_SVC_RECT_TYPE detect_type[VENC_MAX_SVC_RECT_NUM]; /*智能目标属性类别*/
    UINT64 pts;/*平台时间戳：可选用算法检测帧中PTS*/
} VENC_SVC_RECT_INFO_S;

typedef struct {
    VENC_SVC_MAP_PARAM_S  fg_region[VENC_SVC_RECT_TYPE_BUTT]; /*前景区域的QpMap和SkipMap值*/
    VENC_SVC_MAP_PARAM_S  activity_region; /*可能有目标区域的QpMap和SkipMap值*/
    VENC_SVC_MAP_PARAM_S  bg_region; /*背景区域的QpMap和SkipMap值*/
    XRS_BOOL fg_protect_adaptive_en; /* 对面积较大的前景物体少量升高QP，避免质量过好，以减少对码率的消耗。默认值为1*/
} VENC_SVC_PARAM_V1_S;

typedef struct {
    UINT32 max_ref_num;		/*长期参考个数，范围[2,4]*/
    UINT32 refresh_interval; /*参考刷新间隔，范围[1,21]*/
    UINT32 qp_delta[VENC_SVC_RECT_TYPE_BUTT]; /*前景和背景qp调节值，范围[-16，15]*/
} VENC_SVC_PARAM_V2_S;

typedef struct {
	XRS_BOOL Enable;
    VENC_SVC_VERSION_E svc_version; /*智能编码版本号*/
    union {
        VENC_SVC_PARAM_V1_S svc_param_v1; /* 智能编码1.0参数 */
        VENC_SVC_PARAM_V2_S svc_param_v2; /* 智能编码2.0参数 */
    };
} VENC_SVC_PARAM_S;

typedef enum {
    VENC_SUPER_FRAME_NONE = 0,                  /* sdk don't care super frame */
    VENC_SUPER_FRAME_DISCARD,                   /* the super frame is discarded */
    VENC_SUPER_FRAME_REENCODE,                  /* the super frame is re-encode */
    VENC_SUPER_FRAME_BUTT
} VENC_SUPER_FRAME_MODE_E;

typedef enum {
    VENC_REENCODE_BIT_RATE_FIRST = 1,
    VENC_REENCODE_FRAME_BITS_FIRST,
    VENC_REENCODE_BUTT,
} VENC_REENCODE_PRIORITY_E;

typedef struct {
    UINT32 qp_delta; /* RW; range:[1, 51], for svac3: [1, 63]; for mjpeg : [1, 99]; default 1*/
    SINT32 max_lost_times; /* RW; range:[0, 1000]; default 0-不限制丢帧次数*/
} VENC_DISCARD_PARAM_S;

typedef struct {
    VENC_SUPER_FRAME_MODE_E super_frame_mode;
    UINT32 i_frame_bits_threshold;
    UINT32 p_frame_bits_threshold;
    UINT32 b_frame_bits_threshold;
    VENC_REENCODE_PRIORITY_E reencode_priority;
    VENC_DISCARD_PARAM_S discard_param;
} VENC_SUPER_FRAME_STRATEGY_S;

typedef struct {
    XRS_BOOL enable;
    SINT32  qpDelta[4];
    SINT32  frameNum[4];
} VENC_HIERARCHICAL_QP_S, *PVENC_HIERARCHICAL_QP_S;

typedef enum {
    VENC_DUPLICATE_FRAME_STRATEGE_RECODE = 0,/*输入图像增帧策略*/
    VENC_DUPLICATE_FRAME_STRATEGE_COPY,/*输出码流拷贝增帧策略*/
    VENC_DUPLICATE_FRAME_STRATEGE_BUTT
} VENC_DUPLICATE_FRAME_STRATEGY_E;

typedef enum {
    VENC_FRAME_LOST_NORMAL = 0,
    VENC_FRAME_LOST_P_SKIP,
    VENC_FRAME_LOST_BUTT,
} VENC_FRAME_LOST_MODE_E;

typedef struct {
    XRS_BOOL Enable;
    UINT32 BitRateThreshold;
    VENC_FRAME_LOST_MODE_E Mode;
    UINT32 FrameGap;
} VENC_FRAME_LOST_STRATEGY_S, *PVENC_FRAME_LOST_STRATEGY_S;

/******************************************************************************
 * 函数功能: 打开模块,分配系统资源,模块初始化
 * 输入参数: MaxChn: 当前视频最大输入数
 *           ChipConf: 芯片配置信息
 *           VencConf: 编码配置信息
 *           VDataFunc: 视频数据回调函数
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_Open(SINT32 MaxChn, VIDEO_CHIP_CONF_S ChipConf, VENC_CONF_S VencConf, VData_Callback VDataFunc,
                 VRes_Modify_Callback ResModifyFunc);

/******************************************************************************
 * 函数功能: 设置编码通道流回调函数
 * 输入参数: ChnType: 编码流类型
 *           VDataFunc: 视频数据回调函数
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_SetChnCallback(VENC_CHNTYPE_E ChnType, VData_Callback VDataFunc, VStream_Notify_Callback StreamNotifyFunc);

/******************************************************************************
 * 函数功能: 通过码流类型获取实际编码通道号
 * 输入参数: handle: 模块句柄,此处无效
 *           ChnType: 编码流类型
 * 输出参数: VencChn: 编码通道号
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_GetChnByChnType(SINT32 handle, VENC_CHNTYPE_E ChnType, SINT32 *VencChn);

/******************************************************************************
 * 函数功能: 通过编码通道号获取对应的码流类型
 * 输入参数: handle: 模块句柄,此处无效
 *           VencChn: 编码通道号
 * 输出参数: ChnType: 编码流类型
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_GetChnTypeByChn(SINT32 handle, SINT32 VencChn, VENC_CHNTYPE_E *ChnType);

/******************************************************************************
 * 函数介绍: 启动模块
 * 输入参数: handle: 模块句柄,此处无效
 *           priority: 线程优先级1~99
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_Start(SINT32 handle, SINT32 priority);

/******************************************************************************
 * 函数介绍: 停止模块
 * 输入参数: handle: 模块句柄,此处无效
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_Stop(SINT32 handle);

SINT32 Venc_Chn_Send_Frm(SINT32 handle, VENC_CHNTYPE_E ChnType, SINT32 VencChn, PVIDEO_FRM_ALG_S pVFrmInfo);

/******************************************************************************
 * 函数功能: 创建视频编码通道
 * 输入参数: handle: 模块句柄(此处无效)
 *           ChnType: 编码通道类型，区分主码流子码流
 *           VencChn: 编码通道号，对应前端多个sensor
 *           EncParam: 编码参数
 *           SrcFrmRate: 编码通道源帧率,前端模块帧率
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_Chn_Open(SINT32 handle, VENC_CHNTYPE_E ChnType, SINT32 VencChn, VENC_ENCODEPARAM_S EncParam,
                     UINT32 SrcFrmRate);

SINT32 Venc_Chn_Start(SINT32 handle, VENC_CHNTYPE_E ChnType, SINT32 VencChn, XRS_BOOL enReset);

SINT32 Venc_Chn_Stop(SINT32 handle, VENC_CHNTYPE_E ChnType, SINT32 VencChn);

/******************************************************************************
 * 函数功能: 销毁视频编码通道
 * 输入参数: handle: 模块句柄(此处无效)
 *           ChnType: 编码通道类型
 *           VencChn: 视频输入通道号,对应Venc_Chn_Open的通道号
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_Chn_Close(SINT32 handle, VENC_CHNTYPE_E ChnType, SINT32 VencChn);

/******************************************************************************
 * 函数功能: VENC通道绑定
 * 输入参数: handle: 模块句柄,此处无效
 *           BindConf: VENC绑定属性信息
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 ******************************************************************************/
SINT32 Venc_Chn_Bind(SINT32 handle, SINT32 VencChn, VENC_BIND_CONF_S BindConf);

/******************************************************************************
 * 函数功能: VENC通道解绑定
 * 输入参数: handle: 模块句柄,此处无效
 *           BindConf: VENC解绑定属性信息
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 ******************************************************************************/
SINT32 Venc_Chn_UnBind(SINT32 handle, SINT32 VencChn, VENC_BIND_CONF_S BindConf);

/******************************************************************************
 * 函数功能: 请求码流关键帧
 * 输入参数: handle: 模块句柄,此处无效
 *           ChnType: 编码通道类型
 *           VencChn: 编码通道号(从0开始)
 * 输出参数: 无
 * 返回值  : >=0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_Chn_Request_IFrm(SINT32 handle, VENC_CHNTYPE_E ChnType, SINT32 VencChn);

/******************************************************************************
 * 函数功能: 设置视频编码参数
 * 输入参数: handle: 模块句柄,此处无效
 *           ChnType: 编码通道类型
 *           VencChn: 编码通道号(从0开始)
 *           EncParam: 编码参数
 *           SrcFrmRate: 编码通道源帧率
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_Chn_Set_EncodeParam(SINT32 handle, VENC_CHNTYPE_E ChnType, SINT32 VencChn, VENC_ENCODEPARAM_S EncParam,
                                SINT32 SrcFrmRate);

/******************************************************************************
 * 函数功能: 获取视频编码参数
 * 输入参数: handle: 模块句柄,此处无效
 *           ChnType: 编码通道类型
 *           VencChn: 编码通道号(从0开始)
 * 输出参数: pEncParam: 编码参数
 *           pSrcFrmRate: 编码通道源帧率
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_Chn_Get_EncodeParam(SINT32 handle, VENC_CHNTYPE_E ChnType, SINT32 VencChn, PVENC_ENCODEPARAM_S pEncParam,
                                SINT32 *pSrcFrmRate);

/******************************************************************************
 * 函数功能: 设置视频编码QP属性
 * 输入参数: handle: 模块句柄,此处无效
 *           ChnType: 编码通道类型
 *           VencChn: 编码通道号(从0开始)
 *           pQpAttr: 编码QP参数
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_Chn_Set_QpAttr(SINT32 handle, VENC_CHNTYPE_E ChnType, SINT32 VencChn, VENC_QP_CONF_V2_S QpAttr);

/******************************************************************************
 * 函数功能: 获取视频编码QP属性
 * 输入参数: handle: 模块句柄,此处无效
 *           ChnType: 编码通道类型
 *           VencChn: 编码通道号(从0开始)
 * 输出参数: pQpAttr: 编码QP参数
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_Chn_Get_QpAttr(SINT32 handle, VENC_CHNTYPE_E ChnType, SINT32 VencChn, PVENC_QP_CONF_V2_S pQpAttr);

/******************************************************************************
 * 函数功能: 设置视频编码帧率
 * 输入参数: handle: 模块句柄,此处无效
 *           ChnType: 编码通道类型
 *           VencChn: 编码通道号(从0开始)
 *           SrcFrmRate: 编码通道源帧率
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_Chn_Set_FrmRate(SINT32 handle, VENC_CHNTYPE_E ChnType, SINT32 VencChn, SINT32 SrcFrmRate);

/******************************************************************************
 * 函数功能: 设置视频编码I帧间隔
 * 输入参数: handle: 模块句柄,此处无效
 *           ChnType: 编码通道类型
 *           VencChn: 编码通道号(从0开始)
 *           IFrmInter: 编码通道I帧间隔
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_Chn_Set_IFrmInter(SINT32 handle, VENC_CHNTYPE_E ChnType, SINT32 VencChn, UINT32 IFrmInter);

/******************************************************************************
 * 函数功能: 设置P帧帧内刷新功能，将I帧宏块均分到其后P帧上，以达到降低I帧大小目的
 * 输入参数: handle: 模块句柄,此处无效
 *           ChnType: 编码通道类型
 *           VencChn: 编码通道号(从0开始)
 *           pIntraRef: 帧内刷新信息
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_Chn_Set_IntraRefresh(SINT32 handle, VENC_CHNTYPE_E ChnType, SINT32 VencChn, VENC_INTRA_REFRESH_S *pIntraRef);

/******************************************************************************
 * 函数功能: 设置智能编码版本。支持V1.0和V2.0；需在创建通道之前设置
 * 输入参数: handle: 模块句柄(此处无效)
 *           ChnType: 编码通道类型，区分主码流子码流
 *           VencChn: 编码通道号(从0开始)
 *           Version: 智能编码版本
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_Chn_SetSvcVersion(SINT32 handle, VENC_CHNTYPE_E ChnType, SINT32 VencChn, VENC_SVC_VERSION_E Version);

/******************************************************************************
 * 函数功能: 设置智能编码属性。需在创建通道之后设置
 * 输入参数: handle: 模块句柄(此处无效)
 *           ChnType: 编码通道类型，区分主码流子码流
 *           VencChn: 编码通道号(从0开始)
 *           Param: 智能编码属性
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_Chn_SetSvcParam(SINT32 handle, VENC_CHNTYPE_E ChnType, SINT32 VencChn, VENC_SVC_PARAM_S Param);

/******************************************************************************
 * 函数功能: 设置智能检测目标框属性信息：需已开启智能编码
 * 输入参数: handle: 模块句柄(此处无效)
 *           ChnType: 编码通道类型，区分主码流子码流
 *           VencChn: 编码通道号(从0开始)
 *           RectInfo: 智能检测目标框属性信息
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_Chn_SetSvcRegion(SINT32 handle, VENC_CHNTYPE_E ChnType, SINT32 VencChn, VENC_SVC_RECT_INFO_S RectInfo);

/******************************************************************************
 * 函数功能: 设置编码超大帧配置
 * 输入参数: handle: 模块句柄(此处无效)
 *           ChnType: 编码通道类型，区分主码流子码流
 *           VencChn: 编码通道号(从0开始)
 *           Strategy: 编码超大帧配置
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_Chn_SetSuperFrameStrategy(SINT32 handle, VENC_CHNTYPE_E ChnType, SINT32 VencChn, VENC_SUPER_FRAME_STRATEGY_S Strategy);

/******************************************************************************
 * 函数功能: 获取编码超大帧配置
 * 输入参数: handle: 模块句柄(此处无效)
 *           ChnType: 编码通道类型，区分主码流子码流
 *           VencChn: 编码通道号(从0开始)
 * 输出参数: Strategy: 编码超大帧配置
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_Chn_GetSuperFrameStrategy(SINT32 handle, VENC_CHNTYPE_E ChnType, SINT32 VencChn, VENC_SUPER_FRAME_STRATEGY_S *Strategy);

/******************************************************************************
 * 函数功能: 设置编码分层qp，建议在Venc_Chn_Open和Venc_Chn_Start之间调用
 * 输入参数: handle: 模块句柄(此处无效)
 *           ChnType: 编码通道类型，区分主码流子码流
 *           VencChn: 编码通道号(从0开始)
 *           hierarQP: 编码分层qp配置
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_Chn_SetHierarchicalQp(SINT32 handle, VENC_CHNTYPE_E ChnType, SINT32 VencChn, VENC_HIERARCHICAL_QP_S hierarQP);

/******************************************************************************
 * 函数功能: 获取编码编码分层qp
 * 输入参数: handle: 模块句柄(此处无效)
 *           ChnType: 编码通道类型，区分主码流子码流
 *           VencChn: 编码通道号(从0开始)
 * 输出参数: hierarQP: 编码分层qp配置
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_Chn_GetHierarchicalQp(SINT32 handle, VENC_CHNTYPE_E ChnType, SINT32 VencChn, VENC_HIERARCHICAL_QP_S *hierarQP);

/******************************************************************************
 * 函数功能: 设置增帧策略。需在Venc_Chn_Open接口之前调用, SDK 010及以上版本支持
 * 输入参数: handle: 模块句柄(此处无效)
 *           ChnType: 编码通道类型，区分主码流子码流
 *           VencChn: 编码通道号(从0开始)
 *           Strategy: 编码增帧策略
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_Chn_SetDuplicateFrameStrategy(SINT32 handle, VENC_CHNTYPE_E ChnType, SINT32 VencChn, VENC_DUPLICATE_FRAME_STRATEGY_E Strategy);

/******************************************************************************
 * 函数功能: 设置增帧策略。需在Venc_Chn_Open接口之后调用, SDK 010及以上版本支持
 * 			 不支持slice低延时/复合编码/水印/高级跳帧参考/intra_refresh
 * 输入参数: handle: 模块句柄(此处无效)
 *           ChnType: 编码通道类型，区分主码流子码流
 *           VencChn: 编码通道号(从0开始)
 *           SrcFrmRate: 源帧率。输出码流拷贝增帧策略时需等于gop，否则功能不生效
 *           DstFrmRate: 目的帧率。输出码流拷贝增帧策略时，H.265最大DstFrmRate为240,
 *						 不支持2倍及其以上增帧；H.264最大支持5倍码流增帧。		 			 
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_Chn_SetDuplicateFrameParam(SINT32 handle, VENC_CHNTYPE_E ChnType, SINT32 VencChn, SINT32 SrcFrmRate, SINT32 DstFrmRate);

/******************************************************************************
 * 函数功能: 设置编码通道瞬时码率超过阈值时丢帧策略
 * 输入参数: handle: 模块句柄(此处无效)
 *           ChnType: 编码通道类型，区分主码流子码流
 *           VencChn: 编码通道号(从0开始)
 *           Strategy: 瞬时码率超过阈值时的丢帧策略参数
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_Chn_SetFrameLostStrategy(SINT32 handle, VENC_CHNTYPE_E ChnType, SINT32 VencChn, VENC_FRAME_LOST_STRATEGY_S Strategy);

/******************************************************************************
 * 函数功能: 获取编码通道瞬时码率超过阈值时丢帧策略
 * 输入参数: handle: 模块句柄(此处无效)
 *           ChnType: 编码通道类型，区分主码流子码流
 *           VencChn: 编码通道号(从0开始)
 * 输出参数: Strategy: 瞬时码率超过阈值时的丢帧策略参数
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_Chn_GetFrameLostStrategy(SINT32 handle, VENC_CHNTYPE_E ChnType, SINT32 VencChn, VENC_FRAME_LOST_STRATEGY_S *Strategy);

/******************************************************************************
 * 函数功能: 设置感兴趣区域
 * 输入参数: handle: 模块句柄,此处无效
 *           ChnType: 编码通道类型
 *           VencChn: 编码通道号(从0开始)
 *           RoiParam: 感兴趣区域参数
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_Chn_Set_Roi(SINT32 handle, VENC_CHNTYPE_E ChnType, SINT32 VencChn, VENC_ROI_PARAM_S RoiParam);

/******************************************************************************
 * 函数功能: 获取感兴趣区域信息
 * 输入参数: handle: 模块句柄,此处无效
 *           ChnType: 编码通道类型
 *           VencChn: 编码通道号(从0开始)
 * 输出参数: pRoiParam: 感兴趣区域参数
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_Chn_Get_Roi(SINT32 handle, VENC_CHNTYPE_E ChnType, SINT32 VencChn, PVENC_ROI_PARAM_S pRoiParam);

/******************************************************************************
 * 函数功能: 设置视频编码通道画面格式(只适用于复合通道编码)
 * 输入参数: handle: 模块句柄,此处无效
 *           ChnType: 编码通道类型(只适用于复合通道编码)
 *           VencChn: 编码通道号(从0开始)
 *           VoScreen: 分屏格式参数(其中参数vo_dev无需调用者填充)
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_Chn_Set_Screen(SINT32 handle, VENC_CHNTYPE_E ChnType, SINT32 VencChn, VO_SCREEN_S VoScreen);

/******************************************************************************
 * 函数功能: 通过图片YUV数据流和位置信息抠出相应的JPEG图片
 * 输入参数: handle: 模块句柄(此处无效)
 *           VencChn: 编码通道号(从0开始)(此处暂时无效)
 *           EncParam: 编码参数
 *           pCropJpegConf: 图片抠图配置参数
 *           exp_res: jpeg期望编码分辨率
 * 输出参数: pCropJpegConf->Buff: 图片的数据信息
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_Chn_StrmToCropJpeg(SINT32 handle, VENC_CHNTYPE_E ChnType, SINT32 VencChn, VENC_ENCODEPARAM_S EncParam,
                               PVENC_CROP_JPEG_CONF_S pCropJpegConf, VIDEO_RES_S exp_res);

/******************************************************************************
 * 函数功能: 单张抓图,写回调存盘
 * 输入参数: handle: 模块句柄,此处无效
 *           Chn: 编码或者解码逻辑通道号
 *           VSnapType: 抓拍类型(VSNAP_TYPE_E)
 *			 PlaybackChn: 回放通道号(回放抓拍时使用)
 * 输出参数: 无
 * 返回值  : >=0-成功,图片大小,<0-错误代码
 *****************************************************************************/
SINT32 Venc_Chn_Get_OneJpeg(SINT32 handle, SINT32 Chn, VSNAP_TYPE_E VSnapType, SINT32 PlaybackChn);

/******************************************************************************
 * 函数功能: 可选发送数据到VDEC模块,并输出一张JPEG图片(用于邮件抓拍等)
 * 输入参数: handle: 模块句柄(此处无效)
 *           Chn: 编码或者解码通道ID
 *           VSnapType: 抓拍类型(VSNAP_TYPE_E)
 *           EncParam:
 *抓拍参数(仅供传递图片宽和高--使用此结构体是为了下层代码接口统一) WrtMode:
 *解码写入阻塞模式(VDEC_DEAL_MODE_E) pBuff: 存放JPEG图片的缓冲区指针
 *(调用者分配) BuffSize: 存放JPEG图片的缓冲区长度 (调用者赋值) 输出参数: pBuff:
 *JPEG图片的数据,带MSSTD头信息 返回值  :
 *>=0-JPEG图片的大小,包括MSSTD头大小,<0-错误代码
 *****************************************************************************/
SINT32 Venc_Chn_Get_OneJpeg_RealTime(SINT32 handle, SINT32 Chn, VSNAP_TYPE_E VSnapType, VENC_ENCODEPARAM_S EncParam,
                                     VDEC_DEAL_MODE_E WrtMode, UINT8 *pBuff, SINT32 BuffSize);

/******************************************************************************
 * 函数功能: 模块控制
 * 输入参数: handle: 模块句柄,此处无效
 *           cmd: 具体的控制参数命令
 *           Chn: 视频输出通道号(从0开始)
 *           param: 对应命令的参数
 *           param_len:
 *param长度,特别对于GET命令时,输出参数应先判断缓冲区是否足够 输出参数: param:
 *输出参数 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_IoCtrl(SINT32 handle, SINT32 cmd, SINT32 Chn, SINT32 *param, SINT32 param_len);

/******************************************************************************
 * 函数介绍: 关闭模块,释放系统资源
 * 输入参数: handle: 模块句柄,此处无效
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_Close(SINT32 handle);

/******************************************************************************
 * 函数介绍: 编码资源释放
 * 输入参数: handle: 模块句柄,此处无效
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_ResourceRel(SINT32 handle);

/******************************************************************************
 * 函数功能: 通过图片YUV数据硬编JPEG图片
 * 输入参数: handle: 模块句柄(此处无效)
 *			 YuvInfo: YUV数据信息（宽高、数据指针及大小）
 *			 pJpegBuf: jpeg缓存数据指针
 *			 BufSize: jpeg缓存数据长度
 * 输出参数: JpegSize: jpeg实际长度
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_YuvEncToJpeg(SINT32 handle, VENC_YUV_INFO_S YuvInfo, UINT8 *pJpegBuf, SINT32 BufSize, SINT32 *JpegSize,
                         UINT32 Quality);

/******************************************************************************
 * 函数功能: 通过图片YUV数据裁剪后硬编JPEG图片
 * 输入参数: handle: 模块句柄(此处无效)
 *			 YuvInfo: YUV数据信息（宽高、数据指针及大小）
 *			 Rect: 裁剪区域
 *			 pJpegBuf: jpeg缓存数据指针
 *			 BufSize: jpeg缓存数据长度
 * 输出参数: JpegSize: jpeg实际长度
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_YuvCropEncToJpeg(SINT32 handle, VENC_YUV_INFO_S YuvInfo, VRECT_S Rect, UINT8 *pJpegBuf, SINT32 BufSize,
                             SINT32 *JpegSize, UINT32 Quality);

/******************************************************************************
 * 函数功能: YUV帧数据编码JPEG图片
 * 输入参数: handle: 模块句柄(此处无效)
 *			 pVFrmAlg: YUV帧数据信息。注：需要保证宽、高，物理地址参数有效
 *			 pJpegBuf: jpeg缓存数据指针
 *			 BufSize: jpeg缓存数据长度
 *			 Quality: jpeg编码质量
 * 输出参数: JpegSize: jpeg实际长度
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_FrameEncToJpeg(SINT32 handle, PVIDEO_FRM_ALG_S pVFrmAlg, UINT8 *pJpegBuf, SINT32 BufSize, SINT32 *JpegSize,
                           UINT32 Quality);

/******************************************************************************
 * 函数功能: YUV帧数据编码JPEG图片并叠加osd位图（CLUT4格式）
 * 输入参数: handle: 模块句柄(此处无效)
 *			 pVFrmAlg: YUV帧数据信息。注：需要保证宽、高，物理地址参数有效
 *			 pJpegBuf: jpeg缓存数据指针
 *			 BufSize: jpeg缓存数据长度
 *			 Quality: jpeg编码质量
 *			 PVosdOverLay: osd位图绘制信息
 * 输出参数: JpegSize: jpeg实际长度
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_FrameEncToJpegWithOsd(SINT32 handle, PVIDEO_FRM_ALG_S pVFrmAlg, UINT8 *pJpegBuf, SINT32 BufSize, SINT32 *JpegSize, UINT32 Quality, PVOSD_OVERYLAY_S PVosdOverLay);

/******************************************************************************
 * 函数功能: 申请平台媒体内存
 * 输入参数: size: 需要申请的内存大小
 * 输出参数: physAddr: 返回的物理地址
 *			 pVirtAddr：返回的虚拟地址
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_MediaMemMalloc(UINT32 *physAddr, void **pVirtAddr, UINT32 size);

/******************************************************************************
 * 函数功能: 释放平台媒体内存
 * 输入参数: physAddr: 物理地址
 *			 pVirtAddr：虚拟地址
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_MediaMemFree(UINT32 physAddr, void *pVirtAddr);

/******************************************************************************
 * 函数功能: rmem虚拟地址映射物理地址
 * 输入参数: paddr: 虚拟地址
 * 返回值  : 物理地址
 *****************************************************************************/
intptr_t Venc_MediaMemVirtToPhys(intptr_t vaddr);

/******************************************************************************
 * 函数功能: rmem物理地址映射虚拟地址
 * 输入参数: paddr: 物理地址
 * 返回值  : 虚拟地址
 *****************************************************************************/
intptr_t video_MediaMemPhysToVirt(intptr_t paddr);

/******************************************************************************
 * 函数功能: 通过平台算子YUV缩放
 * 输入参数: srcYuvInfo: 源yuv数据信息（宽高、数据指针）
 *			 dstYuvInfo: 目的YUV数据信息（宽高、数据指针）
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_YuvResize(VENC_YUV_INFO_S srcYuvInfo, VENC_YUV_INFO_S dstYuvInfo);

/******************************************************************************
 * 函数功能: 通过平台算子YUV扣图
 * 输入参数: pSrcVFrmAlg: 源yuv数据信息。注：需要保证宽、高，物理地址参数有效
 *			 crop: 抠图坐标信息
 *			 pDstVFrmAlg：目的YUV数据信息。注：需要保证宽、高，物理地址参数有效
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_YuvCrop(PVIDEO_FRM_ALG_S pSrcVFrmAlg, VRECT_S crop, PVIDEO_FRM_ALG_S pDstVFrmAlg);

/******************************************************************************
 * 函数功能: 通过平台算子YUV先扣图后缩放
 * 输入参数: pSrcVFrmAlg: 源yuv数据信息。注：需要保证宽、高，物理地址参数有效
 *			 crop: 抠图坐标信息
 *			 pDstVFrmAlg：目的YUV数据信息。注：需要保证宽、高，物理地址参数有效
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_YuvCropAndResize(PVIDEO_FRM_ALG_S pSrcVFrmAlg, VRECT_S crop, PVIDEO_FRM_ALG_S pDstVFrmAlg);

/******************************************************************************
 * 函数功能: 通过从framesource中直接获取的Frame数据硬编JPEG图片
 * 输入参数: handle: 模块句柄(此处无效)
 *			 YuvInfo: YUV数据信息（宽高、数据指针及大小）
 * 					 其中数据buf需由Venc_MediaMemMalloc_MultiProc接口申请
 * 					 或者由多进程取帧返回的物理地址通过video_EncoderMemPhysToVirt转化
 *			 pJpegBuf:
 *jpeg缓存数据指针,需由Venc_MediaMemMalloc_MultiProc接口申请 BufSize:
 *jpeg缓存数据长度 输出参数: JpegSize: jpeg实际长度 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Venc_FrameEncToJpeg_MultiProc(SINT32 handle, VENC_YUV_INFO_S YuvInfo, UINT8 *pJpegBuf, SINT32 BufSize,
                                     SINT32 *JpegSize, UINT32 Quality);

/******************************************************************************
 * 函数功能: 申请平台媒体内存
 * 输入参数: size: 需要申请的内存大小
 *			 align: 需要对齐比特位数
 * 返回值  : 申请的buf地址
 *****************************************************************************/
void *Venc_EncoderMemMalloc_MultiProc(SINT32 size, SINT32 align);

/******************************************************************************
 * 函数功能: 释放平台媒体内存
 * 输入参数: pBuf: 需要释放的内存buf
 * 返回值  : 无
 *****************************************************************************/
void Venc_EncoderMemFree_MultiProc(void *pBuf);

/******************************************************************************
 * 函数功能: rmem虚拟地址映射物理地址
 * 输入参数: paddr: 虚拟地址
 * 返回值  : 物理地址
 *****************************************************************************/
intptr_t Venc_EncoderMemVirtToPhys(intptr_t vaddr);

/******************************************************************************
 * 函数功能: rmem物理地址映射虚拟地址
 * 输入参数: paddr: 物理地址
 * 返回值  : 虚拟地址
 *****************************************************************************/
intptr_t Venc_EncoderMemPhysToVirt(intptr_t paddr);

#ifdef __cplusplus
}
#endif  //@__cplusplus
#endif  /* _VENC_H_ */
