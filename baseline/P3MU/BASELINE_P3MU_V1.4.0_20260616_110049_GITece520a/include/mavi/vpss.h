#ifndef __VPSS_H__
#define __VPSS_H__

#ifdef __cplusplus
extern "C" {
#endif  //@__cplusplus
// #include "xrsstd.h"
#include "vcomm.h"

#define MAX_USER_VPSS_CHN_NUMS 3

typedef enum {
    VPSS_ERROR_START = COMP_ERROR_START, /* 用户模块起始错误号 */
    VPSS_ERROR_PARAM,                    /* 参数错误*/
    VPSS_ERROR_UNINIT,                   /* 模块未初始化*/
    VPSS_ERROR_REINIT,                   /* 模块重复初始化*/
    VPSS_ERROR_OVER_MAX_CHN,             /* 通道号超出支持个数 */
    VPSS_ERROR_SDK_API_ABN,              /* sdk FS接口报错 */
    VPSS_ERROR_FRAME_NOT_RELEASE,        /* 获取帧未释放 */
    VPSS_ERROR_FRAME_NOT_GET,            /* 未获取帧 */
    VPSS_ERROR_GRP_NOT_OPEN,             /* 组未打开 */
    VPSS_ERROR_GRP_OPEN,                 /* 创建组错误 */
    VPSS_ERROR_GRP_START,                /* 启动组错误 */
    VPSS_ERROR_GRP_STOP,                 /* 停止组错误 */
    VPSS_ERROR_GRP_CLOSE,                /* 组停止错误 */
    VPSS_ERROR_CHN_NOT_OPEN,             /* 通道未打开 */
    VPSS_ERROR_CREATE_CHN,               /* 创建通道错误 */
    VPSS_ERROR_DESTROY_CHN,              /* 销毁通道错误 */
    VPSS_ERROR_SET_CHN_ATTR,             /* 设置通道属性错误 */
    VPSS_ERROR_GET_CHN_ATTR,             /* 获取通道属性错误 */
    VPSS_ERROR_ENABLE_CHN,               /* 使能通道错误 */
    VPSS_ERROR_DISABLE_CHN,              /* 关闭通道错误 */
    VPSS_ERROR_GET_FRAME,                /* 获取帧数据错误 */
    VPSS_ERROR_RELEASE_FRAME,            /* 释放帧数据错误 */
    VPSS_ERROR_GET_AI_FRAME,             /* 获取AI帧数据错误 */
    VPSS_ERROR_RELEASE_AI_FRAME,         /* 释放AI帧数据错误 */
    VPSS_ERROR_INIT_AI,                  /* 初始化算法流错误 */
    VPSS_ERROR_DEINIT_AI,                /* 反初始化算法流错误 */
    VPSS_ERROR_BIND_AI,                  /* 绑定算法流向错误 */
    VPSS_ERROR_UNBIND_AI,                /* 解绑算法流向错误 */
    VPSS_ERROR_SET_SCALE_COEF,           /* 设置缩放系数等级错误 */
    VPSS_ERROR_GET_SCALE_COEF,           /* 获取缩放系数等级错误 */
} VPSS_ERROR_E;
#define VPSS_ERRNO(no) ERRNO(no, COMP_VIDEOVPSS)

#define VPSS_ERR(str, args...) TRACE_ERR(COMP_VIDEOVPSS, str, ##args)
#define VPSS_WARN(str, args...) TRACE_WARN(COMP_VIDEOVPSS, str, ##args)
#define VPSS_FLOW(str, args...) TRACE_FLOW(COMP_VIDEOVPSS, str, ##args)
#define VPSS_INFO(str, args...) TRACE_INFO(COMP_VIDEOVPSS, str, ##args)
#define VPSS_DBG0(str, args...) TRACE_DBG0(COMP_VIDEOVPSS, str, ##args)

typedef struct VpssScaleAttr {
    UINT8 EnFov;
    UINT16 Width;  /* 缩放目标宽度 */
    UINT16 Height; /* 缩放目标高度 */
} VPSS_SCALE_ATTR_S;

typedef struct VpssCropAttr {
    UINT8 Enable;/* 裁剪使能 */
	VRECT_S Rect;/* 裁剪坐标属性 */
} VPSS_CROP_ATTR_S;

typedef struct VpssCenterZoomAttr {
    UINT8 Enable;/* 中心缩放使能 */
	FLOAT64 WidthRatio;/* 与当前画面宽的比例：（0.0-1.0] */
	FLOAT64 HeightRatio;/* 与当前画面高的的比例：（0.0-1.0] */
} VPSS_CENTER_ZOOM_ATTR_S, *PVPSS_CENTER_ZOOM_ATTR_S;

typedef struct VpssZoomAttr {
    UINT8 Enable;/* 裁剪使能 */
	VRECT_S Rect;/* 裁剪坐标属性 */
} VPSS_ZOOM_ATTR_S, *PVPSS_ZOOM_ATTR_S;

typedef struct VpssGrpAttr {
    UINT16 Width;                /* VPSS组输出宽度 */
    UINT16 Height;               /* VPSS组输出高度 */
    UINT8 OutFrmRate;            /* 输出帧率(0-无效,>0有效) */
    UINT8 Rotation;              /* 旋转(MV_ROTATION_E) */
    UINT8 EnMcf;                 /* 是否开启MCF */
    UINT8 EnScale;               /* 是否开启缩放 */
    VPSS_SCALE_ATTR_S ScaleAttr; /* 缩放属性 */
} VPSS_GRP_ATTR_S, *PVPSS_GRP_ATTR_S;

typedef struct {
    UINT8 SrcChn;       /* VPSS通道原通道 */
    UINT8 SrcChnType;   /* VPSS通道源类型(VIDEO_SRC_TYPE_E) */
    UINT8 OutFrmRate;   /* 输出帧率(0-无效,>0有效) */
    UINT8 DisBakFrm;    /* 禁止开启备份帧 */
    UINT8 Rotation;     /* 旋转(MV_ROTATION_E) */
    UINT8 Depth;        /* 通道深度 */
    UINT8 CropRatioH;   /* 水平裁减比例 */
    UINT8 CropRatioV;   /* 垂直裁减比例 */
    UINT8 bReinjection; /* 是否回灌模式 */
    UINT8 Resvd[3];
    UINT16 width;        /* VPSS通道输出宽度 */
    UINT16 height;       /* VPSS通道输出高度 */
    UINT8 bQrcode;       /* 是否打开二维码*/
    UINT8 QrcodeFrmRate; /* 二维码输出帧率(0-无效,>0有效) */
    UINT8 Resvd1[2];
    UINT16 QrcodeWidth;  /* 二维码VPSS通道宽度 */
    UINT16 QrcodeHeight; /* 二维码VPSS通道高度 */
} VPSS_CHN_ATTR_S, *PVPSS_CHN_ATTR_S;
#define VPSS_CHN_ATTR_S_LEN sizeof(VPSS_CHN_ATTR_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(VPSS_CHN_ATTR_S, 24);

typedef struct {
    XRS_BOOL enable;        /* RW; Range: [0, 1] */
    UINT32 svc_interval;    /* RW; Range: [1, 10]; Mark one frame every svc_interval frames */
    SINT32 svc_chn;    		/* RW; Range: [0, OT_VPSS_MAX_PHYS_CHN_NUM); Mark other chns based on svc chn */
} VPSS_GRP_SVC_PARAM_S, *PVPSS_GRP_SVC_PARAM_S;

//scale range: dst/src
typedef enum  {
    VPSS_SCALE_RANGE_0 = 0, /* scale range <   8/64 */
    VPSS_SCALE_RANGE_1, /* scale range >=  8/64 */
    VPSS_SCALE_RANGE_2, /* scale range >= 10/64 */
    VPSS_SCALE_RANGE_3, /* scale range >= 15/64 */
    VPSS_SCALE_RANGE_4, /* scale range >= 19/64 */
    VPSS_SCALE_RANGE_5, /* scale range >= 24/64 */
    VPSS_SCALE_RANGE_6, /* scale range >= 29/64 */
    VPSS_SCALE_RANGE_7, /* scale range >= 33/64 */
    VPSS_SCALE_RANGE_8, /* scale range >= 35/64 */
    VPSS_SCALE_RANGE_9, /* scale range >= 38/64 */
    VPSS_SCALE_RANGE_10, /* scale range >= 42/64 */
    VPSS_SCALE_RANGE_11, /* scale range >= 45/64 */
    VPSS_SCALE_RANGE_12, /* scale range >= 48/64 */
    VPSS_SCALE_RANGE_13, /* scale range >= 51/64 */
    VPSS_SCALE_RANGE_14, /* scale range >= 53/64 */
    VPSS_SCALE_RANGE_15, /* scale range >= 55/64 */
    VPSS_SCALE_RANGE_16, /* scale range >= 57/64 */
    VPSS_SCALE_RANGE_17, /* scale range >= 60/64 */
    VPSS_SCALE_RANGE_18, /* scale range >  1     */
    VPSS_SCALE_RANGE_BUTT,
} VPSS_SCALE_RANGE_TYPE_E;

typedef enum  {
    VPSS_COEF_LEVEL_0 = 0, /* coefficient level 0 */
    VPSS_COEF_LEVEL_1, /* coefficient level 1 */
    VPSS_COEF_LEVEL_2, /* coefficient level 2 */
    VPSS_COEF_LEVEL_3, /* coefficient level 3 */
    VPSS_COEF_LEVEL_4, /* coefficient level 4 */
    VPSS_COEF_LEVEL_5, /* coefficient level 5 */
    VPSS_COEF_LEVEL_6, /* coefficient level 6 */
    VPSS_COEF_LEVEL_7, /* coefficient level 7 */
    VPSS_COEF_LEVEL_8, /* coefficient level 8 */
    VPSS_COEF_LEVEL_9, /* coefficient level 9 */
    VPSS_COEF_LEVEL_10, /* coefficient level 10 */
    VPSS_COEF_LEVEL_11, /* coefficient level 11 */
    VPSS_COEF_LEVEL_12, /* coefficient level 12 */
    VPSS_COEF_LEVEL_13, /* coefficient level 13 */
    VPSS_COEF_LEVEL_14, /* coefficient level 14 */
    VPSS_COEF_LEVEL_15, /* coefficient level 15 */
    VPSS_COEF_LEVEL_16, /* coefficient level 16 */
    VPSS_COEF_LEVEL_17, /* coefficient level 17 */
    VPSS_COEF_LEVEL_18, /* coefficient level 18 */
    VPSS_COEF_LEVEL_19, /* coefficient level 19 */
    VPSS_COEF_LEVEL_BUTT,
} VPSS_COEF_LEVEL_E;

typedef struct {
    VPSS_COEF_LEVEL_E hor_luma; /* horizontal luminance   coefficient level */
    VPSS_COEF_LEVEL_E hor_chroma; /* horizontal chrominance coefficient level */
    VPSS_COEF_LEVEL_E ver_luma; /* vertical   luminance   coefficient level */
    VPSS_COEF_LEVEL_E ver_chroma; /* vertical   chrominance coefficient level */
} VPSS_SCALE_COEF_LEVEL_S, *PVPSS_SCALE_COEF_LEVEL_S;

typedef struct {
    VPSS_SCALE_RANGE_TYPE_E hor;
    VPSS_SCALE_RANGE_TYPE_E ver;
} VPSS_SCALE_RANGE_S;

/******************************************************************************
 * 函数功能: 打开模块,分配系统资源,模块初始化
 * 输入参数: MaxChn: 当前视频最大输入数
 *           ChipConf: 芯片配置信息
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vpss_Open(SINT32 MaxChn, VIDEO_CHIP_CONF_S ChipConf);

/******************************************************************************
 * 函数介绍: 启动模块
 * 输入参数: handle: 模块句柄,此处无效
 *           priority: 线程优先级1~99
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vpss_Start(SINT32 handle, SINT32 priority);

/******************************************************************************
 * 函数介绍: 停止模块
 * 输入参数: handle: 模块句柄,此处无效
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vpss_Stop(SINT32 handle);

/******************************************************************************
 * 函数功能: 打开VPSS 组
 * 输入参数: handle: 模块句柄,此处无效
 *           ChnAttr: VPSS通道属性
 * 输出参数: 无
 * 返回值  : >=0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vpss_Grp_Open(SINT32 handle, SINT32 VpssGrp, VPSS_GRP_ATTR_S *pChnAttr);

/******************************************************************************
 * 函数功能: 关闭VPSS组
 * 输入参数: handle: 模块句柄,此处无效
 *          VpssGrp: VPSS通道号
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vpss_Grp_Close(SINT32 handle, SINT32 VpssGrp);

/******************************************************************************
 * 函数功能: 设置VPSS组中心缩放属性
 * 输入参数: handle: 模块句柄,此处无效
 *          VpssGrp: VPSS通道号
 *          Attr: 中心缩放配置
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vpss_Grp_Set_CenterZoom(SINT32 handle, SINT32 VpssGrp, VPSS_CENTER_ZOOM_ATTR_S Attr);

/******************************************************************************
 * 函数功能: 获取VPSS组中心缩放属性
 * 输入参数: handle: 模块句柄,此处无效
 *          VpssGrp: VPSS通道号
 *          Attr: 中心缩放配置
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vpss_Grp_Get_CenterZoom(SINT32 handle, SINT32 VpssGrp, PVPSS_CENTER_ZOOM_ATTR_S pAttr);

/******************************************************************************
 * 函数功能: 设置VPSS组缩放属性
 * 输入参数: handle: 模块句柄,此处无效
 *          VpssGrp: VPSS通道号
 *          Attr: 缩放属性
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vpss_Grp_Set_Zoom(SINT32 handle, SINT32 VpssGrp, VPSS_ZOOM_ATTR_S Attr);

/******************************************************************************
 * 函数功能: 获取VPSS组缩放属性
 * 输入参数: handle: 模块句柄,此处无效
 *          VpssGrp: VPSS通道号
 * 输出参数: pAttr：缩放属性
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vpss_Grp_Get_Zoom(SINT32 handle, SINT32 VpssGrp, PVPSS_ZOOM_ATTR_S pAttr);

/******************************************************************************
 * 函数功能: 设置VPSS组智能编码配置
 * 输入参数: handle: 模块句柄,此处无效
 *          VpssGrp: VPSS通道号
 *          Param: 智能编码配置置
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vpss_Grp_Set_SvcParam(SINT32 handle, SINT32 VpssGrp, VPSS_GRP_SVC_PARAM_S Param);

/******************************************************************************
 * 函数功能: 获取VPSS组智能编码配置
 * 输入参数: handle: 模块句柄,此处无效
 *          VpssGrp: VPSS通道号
 *          Param: 智能编码配置
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vpss_Grp_Get_SvcParam(SINT32 handle, SINT32 VpssGrp, PVPSS_GRP_SVC_PARAM_S Param);

/******************************************************************************
 * 函数功能: 打开VPSS通道
 * 输入参数: handle: 模块句柄,此处无效
 *          VpssGrp：组号
 *           ChnAttr: VPSS通道属性
 * 输出参数: 无
 * 返回值  : >=0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vpss_Chn_Open(SINT32 handle, SINT32 VpssGrp, SINT32 VpssChn, VPSS_CHN_ATTR_S ChnAttr);

/******************************************************************************
 * 函数功能: 关闭VPSS通道
 * 输入参数: handle: 模块句柄,此处无效
 *          VpssGrp：组号
 *           VpssChn: VPSS通道号
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vpss_Chn_Close(SINT32 handle, SINT32 VpssGrp, SINT32 VpssChn);

/******************************************************************************
 * 函数功能: 使能VPSS通道，开始出流。需保证整个流向设置完成
 * 输入参数: handle: 模块句柄,此处无效
 *          VpssGrp：组号
 *           ChnAttr: VPSS通道属性
 * 输出参数: 无
 * 返回值  : >=0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vpss_Chn_Enable(SINT32 handle, SINT32 VpssGrp, SINT32 VpssChn);

/******************************************************************************
 * 函数功能: 禁用VPSS通道，停止出流
 * 输入参数: handle: 模块句柄,此处无效
 *          VpssGrp：组号
 *           VpssChn: VPSS通道号
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vpss_Chn_Disable(SINT32 handle, SINT32 VpssGrp, SINT32 VpssChn);

/******************************************************************************
 * 函数功能: 获取VPSS通道属性信息
 * 输入参数: handle: 模块句柄,此处无效
 *          VpssGrp：组号
 *           VpssChn: VPSS通道号
 *           pChnAttr: VPSS通道属性指针
 * 输出参数: pChnAttr: VPSS通道属性
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vpss_Chn_Get_Attr(SINT32 handle, SINT32 VpssGrp, SINT32 VpssChn, PVPSS_CHN_ATTR_S pChnAttr);

/******************************************************************************
 * 函数功能: 设置VPSS通道属性信息
 * 输入参数: handle: 模块句柄,此处无效
 *          VpssGrp：组号
 *           VpssChn: VPSS通道号
 *           ChnAttr: VPSS通道属性
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vpss_Chn_Set_Attr(SINT32 handle, SINT32 VpssGrp, SINT32 VpssChn, VPSS_CHN_ATTR_S ChnAttr);

/*****************************************************************************
 * 函数功能: 获取VPSS通道帧数据信息,取算法yuv
 * 输入参数: handle: 模块句柄,此处无效
 *          VpssGrp：组号
 *           VpssChn: VPSS通道号
 *           pVFrmInfo: 帧数据缓冲区指针
 *           TmOutMSec: 获取帧数据超时等待时间(-1: 阻塞模式; 0: 非阻塞模式; >0: 超时模式(超时时间,单位: 毫秒))
 * 输出参数: pVFrmInfo: 帧数据
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vpss_Chn_Get_FrmInfo(SINT32 handle, SINT32 VpssGrp, SINT32 VpssChn, PVIDEO_FRM_INFO_S pVFrmInfo,
                            SINT32 TmOutMSec);

/*****************************************************************************
 * 函数功能: 获取VPSS通道帧数据信息，取算法yuv---,新取帧接口，内部不拷贝
 * 输入参数: handle: 模块句柄,此处无效
 *          VpssGrp：组号
 *           VpssChn: VPSS通道号:Vpss_Chn_Open返回值，取算法帧;
 *           pVFrmAlg: 帧数据缓冲区指针
 *           TmOutMSec: 获取帧数据超时等待时间(-1: 阻塞模式; 0: 非阻塞模式; >0: 超时模式(超时时间,单位: 毫秒))
 * 输出参数: pVFrmAlg: 帧数据
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vpss_Chn_Get_Frm(SINT32 handle, SINT32 VpssGrp, SINT32 VpssChn, PVIDEO_FRM_ALG_S pVFrmAlg, SINT32 TmOutMSec);

SINT32 Vpss_Frm_PhyCopy(SINT32 handle, SINT32 VpssGrp, PVIDEO_FRM_ALG_S pVFrmAlg, PVIDEO_FRM_ALG_S pDstPhyAddr);

SINT32 Vpss_Chn_Rel_Frm(SINT32 handle, SINT32 VpssGrp, SINT32 VpssChn, PVIDEO_FRM_ALG_S pVFrmAlg);

/*****************************************************************************
 * 函数功能: 获取VPSS通道帧数据信息，取编码yuv---,新取帧接口，内部不拷贝
 * 输入参数: handle: 模块句柄,此处无效
 *           pVFrmAlg: 帧数据缓冲区指针
 *           TmOutMSec: 获取帧数据超时等待时间(-1: 阻塞模式; 0: 非阻塞模式; >0: 超时模式(超时时间,单位: 毫秒))
 * 输出参数: pVFrmAlg: 帧数据
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vpss_Chn_Get_EncodeFrm(SINT32 handle, SINT32 VpssGrp, PVIDEO_FRM_ALG_S pVFrmAlg, SINT32 TmOutMSec);

SINT32 Vpss_Chn_Rel_EncodeFrm(SINT32 handle, SINT32 VpssGrp, PVIDEO_FRM_ALG_S pVFrmAlg);

/*****************************************************************************
 * 函数功能: 获取VPSS通道帧数据信息--同步取帧(只有推图在用，没有并发调用的情况)
 * 输入参数: handle: 模块句柄,此处无效
 *          VpssGrp：组号
 *           VpssChn: VPSS通道号
 *           pVFrmInfo: 帧数据缓冲区指针
 *           TmOutMSec: 获取帧数据超时等待时间(-1: 阻塞模式; 0: 非阻塞模式; >0: 超时模式(超时时间,单位: 毫秒))
 * 输出参数: pVFrmInfo: 帧数据
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vpss_Chn_Get_FrmInfo_Sync(SINT32 handle, SINT32 VpssGrp, SINT32 VpssChn, PVIDEO_FRM_INFO_S pVFrmInfo,
                                 SINT32 TmOutMSec);

SINT32 Vpss_Chn_Get_QrcodeFrmInfo(SINT32 handle, SINT32 VpssGrp, SINT32 VpssChn, PVIDEO_FRM_INFO_S pVFrmInfo,
                                  SINT32 TmOutMSec);

SINT32 Vpss_Chn_Get_SnapFrmInfo(SINT32 handle, SINT32 VpssGrp, VISP_SENSOR_MODE_E SensorType,
                                PVIDEO_FRM_INFO_S pVFrmInfo, SINT32 TmOutMSec);
SINT32 Vpss_Chn_Get_PhotoFrmInfo(SINT32 handle, SINT32 VpssGrp, SINT32 VpssChn, PVIDEO_FRM_INFO_S pVFrmInfo,
                                 SINT32 TmOutMSec);

/******************************************************************************
 * 函数介绍: 关闭模块,释放系统资源
 * 输入参数: handle: 模块句柄,此处无效
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vpss_Close(SINT32 handle);

/*****************************************************************************
 * 函数功能: 多进程间获取VPSS通道帧数据信息，取算法yuv---,新取帧接口，内部不拷贝
 * 输入参数: handle: 模块句柄,此处无效
 *			 VpssChn: VPSS通道号:Vpss_Chn_Open返回值，取算法帧;
 *			 pVFrmAlg: 帧数据缓冲区指针
 *			 TmOutMSec: 获取帧数据超时等待时间(-1: 阻塞模式; 0: 非阻塞模式; >0: 超时模式(超时时间,单位:
 *毫秒)) 输出参数: pVFrmAlg: 帧数据 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vpss_Chn_Get_Frm_Ex(SINT32 handle, SINT32 VpssGrp, SINT32 VpssChn, PVIDEO_FRM_ALG_S pVFrmAlg, SINT32 TmOutMSec);

/*****************************************************************************
 * 函数功能: 多进程间释放VPSS通道帧数据信息
 * 输入参数: handle: 模块句柄,此处无效
 *			 VpssChn: VPSS通道号:Vpss_Chn_Open返回值，取算法帧;
 *			 pVFrmAlg: 帧数据缓冲区指针
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vpss_Chn_Rel_Frm_Ex(SINT32 handle, SINT32 VpssGrp, SINT32 VpssChn, PVIDEO_FRM_ALG_S pVFrmAlg);

/******************************************************************************
 * 函数功能: 模块控制
 * 输入参数: handle: 模块句柄,此处无效
 *           cmd: 具体的控制参数命令
 *           Chn: 视频输出通道号(从0开始)
 *           param: 对应命令的参数
 *           param_len: param长度,特别对于GET命令时,输出参数应先判断缓冲区是否足够
 * 输出参数: param: 输出参数
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vpss_IoCtrl(SINT32 handle, SINT32 cmd, SINT32 Chn, SINT32 *param, SINT32 param_len);

/******************************************************************************
 * 函数功能: 获取VPSS模块缩放系数等级信息
 * 输入参数: handle: 模块句柄,此处无效
 * 输出参数: pScaleCoefInfo:缩放系数等级信息
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vpss_Chn_Get_ScaleCoefInfo(SINT32 handle, VPSS_SCALE_RANGE_S ScaleRange, PVPSS_SCALE_COEF_LEVEL_S pScaleCoefLevel);

/******************************************************************************
 * 函数功能: 获取VPSS模块缩放系数等级：在保持缩放比例范围不变的情况下，增大缩放
			 系数的等级时，会使该缩放比例范围的图像看起来更锐利，反之则更模糊；
			 增大缩放系数的等级时，会使得画面锯齿程度变严重
 * 输入参数: handle: 模块句柄,此处无效
 *			 ScaleCoefInfo：缩放系数等级信息
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vpss_Chn_Set_ScaleCoefInfo(SINT32 handle, VPSS_SCALE_RANGE_S ScaleRange, VPSS_SCALE_COEF_LEVEL_S ScaleCoefLevel);

#ifdef __cplusplus
}
#endif  //@__cplusplus
#endif  /* __VPSS_H__ */
