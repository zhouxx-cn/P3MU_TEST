#ifndef __VI_H__
#define __VI_H__

#ifdef __cplusplus
extern "C" {
#endif  //@__cplusplus

#include "xrsstd.h"
#include "vcomm.h"

typedef enum
{
    VI_ERROR_START = COMP_ERROR_START, /* 用户模块起始错误号32 */
    VI_ERROR_UNINIT,                   /* 模块未初始化*/
    VI_ERROR_REINIT,                   /* 模块重复初始化*/
    VI_ERROR_PARAM,                    /* 参数错误 */
    VI_ERROR_INTER,                    /* 内部错误 */
} VI_ERRNO_E;
#define VI_ERRNO(err) ERRNO(err, COMP_VIDEOIN)

#define VI_ERR(str, args...) TRACE_ERR(COMP_VIDEOIN, str, ##args)
#define VI_WARN(str, args...) TRACE_WARN(COMP_VIDEOIN, str, ##args)
#define VI_FLOW(str, args...) TRACE_FLOW(COMP_VIDEOIN, str, ##args)
#define VI_INFO(str, args...) TRACE_INFO(COMP_VIDEOIN, str, ##args)
#define VI_DBG0(str, args...) TRACE_DBG0(COMP_VIDEOIN, str, ##args)

typedef struct
{
    UINT16 width;       /* 图像宽度 */
    UINT16 height;      /* 图像高度 */
    UINT16 CropX;       /* 裁剪的起始x坐标 */
    UINT16 CropY;       /* 裁剪的起始y坐标 */
    UINT8 MainFrame;    /* 主属性帧率 */
    UINT8 ViSrcType;    /* 视频源类型(CVI TVI AHD AUTO) */
    UINT8 RealTimeMode; /* 1-实时模式,0-非实时模式 */
    UINT8 Resvd;        /* 保留位 */
} VI_CHN_CONF_S, *PVI_CHN_CONF_S;
#define VI_CHN_CONF_S_LEN sizeof(VI_CHN_CONF_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(VI_CHN_CONF_S, 12);

typedef struct
{
    XRS_BOOL bEnable; /* 当前通道是否已经使能 */
    XRS_BOOL bChoas;  /* 当前通道是否处于异常状态 */
    UINT8 MaxFrmRate; /* 当前通道配置的最大帧率 */
    UINT8 CurFrmRate; /* 当前通道的实时帧率 */
    UINT8 Resv[2];
    UINT32 LostFrame; /* 当前通道丢帧的数目 */
} VI_CHN_RT_STATUS_S, *PVI_CHN_RT_STATUS_S;
#define VI_CHN_RT_STATUS_S_LEN sizeof(VI_CHN_RT_STATUS_S)

/******************************************************************************
 * 函数介绍: 查询通道的实时工作状态。
 * 输入参数: handle: 模块句柄,此处无效
 *           如果实时帧率小于当前isp档位的设置帧率，pStatus中的bChoase会被置1，显示状态异常
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vi_Chn_Get_RT_Status(SINT32 handle, UINT32 ViChn, PVI_CHN_RT_STATUS_S pStatus);

/******************************************************************************
 * 函数功能: 打开模块,分配系统资源,模块初始化
 * 输入参数: MaxChn: 当前视频最大输入数
 *           ChipConf: 芯片配置信息
 *           ViConf: 视频输入配置信息
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vi_Open(SINT32 MaxChn, VIDEO_CHIP_CONF_S ChipConf, VI_CONF_S ViConf);

/******************************************************************************
 * 函数介绍: 启动模块
 * 输入参数: handle: 模块句柄,此处无效
 *           priority: 线程优先级1~99
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vi_Start(SINT32 handle, SINT32 priority);

/******************************************************************************
 * 函数介绍: 停止模块
 * 输入参数: handle: 模块句柄,此处无效
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vi_Stop(SINT32 handle);

/******************************************************************************
 * 函数功能: 创建视频输入通道
 * 输入参数: handle: 模块句柄(此处无效)
 *           Chn: 视频输入通道号
 *           ViChnConf: 视频输入通道属性
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vi_Chn_Open(SINT32 handle, SINT32 Chn, VI_CHN_CONF_S ViChnConf);

/******************************************************************************
 * 函数功能: 销毁视频输入通道
 * 输入参数: handle: 模块句柄(此处无效)
 *           Chn: 视频输入通道号--Vi_Chn_Open时传递的Chn
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vi_Chn_Close(SINT32 handle, SINT32 Chn);

/******************************************************************************
 * 函数功能: 根据情景模式获取视频参数(亮度,对比度,饱和度,色度)
 * 输入参数: Chn: 视频输入通道号(从0开始)
 *           scene: VIDEO_SCENE_E
 * 输出参数: pViVParam: 视频参数结构体指针
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vi_Chn_Get_VideoParamByScene(SINT32 handle, SINT32 Chn, SINT32 scene, PVI_VIDEO_PARAM_S pViVParam);

/******************************************************************************
 * 函数功能: 获取通道接插视频格式 (转换ad驱动提供的视频格式给SDK层调用)
 *           调用者在获取到视频格式后,需要根据机型来设置对应的分辨率,
 *           如5104E-C检测到的是1080P,那么SDK传递open参数该为1080N。
 * 输入参数: handle: 模块句柄,此处无效
 *           Chn: VI通道号(目前是逻辑通道号)
 * 输出参数: pViFmt: 视频格式(VI_FORMAT_E)
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vi_Chn_Get_VideoFormat(SINT32 handle, SINT32 Chn, SINT32 *pViFmt);

/******************************************************************************
 * 函数功能: 设置VI通道属性(暂时用于vi主属性帧率设置)
 * 输入参数: handle: 模块句柄,此处无效
 *           ViChn: 通道号
 *           MainFrmRate: VI主属性帧率(次属性帧率=通道OPEN时配置帧率-MainFrmRate)在设备开启主次属性时有效
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vi_Chn_Set_Attr(SINT32 handle, SINT32 ViChn, FLOAT32 MainFrmRate);

/******************************************************************************
 * 函数功能: 模块控制
 * 输入参数: handle: 模块句柄,此处无效
 *           cmd: 具体的控制参数命令
 *           Chn: 视频输出通道号(从0开始)
 *           param: 对应命令的参数
 *           param_len: param长度,特别对于GET命令时,输出参数应先判断缓冲区是否足够
 * 输出参数: param: 输出参数
 * 返回值  : >=0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vi_IoCtrl(SINT32 handle, SINT32 cmd, SINT32 Chn, SINT32 *param, SINT32 param_len);

/******************************************************************************
 * 函数介绍: 关闭模块,释放系统资源
 * 输入参数: handle: 模块句柄,此处无效
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vi_Close(SINT32 handle);

#ifdef __cplusplus
}
#endif  //@__cplusplus
#endif /* __VI_H__ */
