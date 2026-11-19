#ifndef _VOSD_H_
#define _VOSD_H_

#ifdef __cplusplus
extern "C" {
#endif  //@__cplusplus
#include "vcomm.h"
#include "xrsstd.h"

// vosd err code
typedef enum {
    VOSD_ERROR_START = COMP_ERROR_START, /* 用户模块起始错误号32 */
    VOSD_ERROR_UNINIT,                   /* 模块未初始化*/
    VOSD_ERROR_REINIT,                   /* 模块重复初始化*/
    VOSD_ERROR_SDK_API_ABN,              /* sdk接口报错35 */
    VOSD_ERROR_CHN_UN_SUPPORT,           /* osd通道类型不支持 */
    VOSD_ERROR_CREATE_GROUP,             /* 创建osd组错误 */
    VOSD_ERROR_DESTORY_GROUP,            /* 销毁osd组错误 */
    VOSD_ERROR_START_GROUP,              /* 开始osd组显示错误 */
    VOSD_ERROR_STOP_GROUP,               /* 停止osd组显示错误40 */
    VOSD_ERROR_SET_GROUPRGN_ATTR,        /* 设置osd组区域属性 */
    VOSD_ERROR_GET_GROUPRGN_ATTR,        /* 获取osd组区域属性 */
    VOSD_ERROR_CREATE_RGN,               /* 创建osd区域错误 */
    VOSD_ERROR_DESTORY_RGN,              /* 销毁osd区域错误 */
    VOSD_ERROR_REGISTER_RGN,             /* 注册osd区域错误45 */
    VOSD_ERROR_UNREGISTER_RGN,           /* 注销osd区域错误 */
    VOSD_ERROR_SET_RGN_ATTR,             /* 设置osd区域属性错误 */
    VOSD_ERROR_GET_RGN_ATTR,             /* 获取osd区域属性错误 */
    VOSD_ERROR_SHOW_RGN,                 /* 显示osd区域错误 */
    VOSD_ERROR_GET_BIT_BY_TEXT,          /* 字符串转点阵数据失败50 */
} VOSD_ERROR_E;
#define VOSD_ERRNO(no) ERRNO(COMP_VIDEOVOSD, no)

#define VOSD_ERR(str, args...) TRACE_ERR(COMP_VIDEOVOSD, str, ##args)
#define VOSD_WARN(str, args...) TRACE_WARN(COMP_VIDEOVOSD, str, ##args)
#define VOSD_FLOW(str, args...) TRACE_FLOW(COMP_VIDEOVOSD, str, ##args)
#define VOSD_INFO(str, args...) TRACE_INFO(COMP_VIDEOVOSD, str, ##args)
#define VOSD_DBG0(str, args...) TRACE_DBG0(COMP_VIDEOVOSD, str, ##args)

/******************************************************************************
 * 函数功能: 打开模块,分配系统资源,模块初始化 (调用不分先后顺序)
 * 输入参数: MaxChn: 当前视频最大输入数 (最大为视频输入的路数)
 *           ChipConf: 芯片配置信息
 *		     VosdConf: 配置字库参数,语言类型,最大osd通道数<=MaxChn(视频输入路数)
 * 输出参数: 无
 * 返回值  : >0-模块句柄,<0-错误代码
 *****************************************************************************/
SINT32 Vosd_Open(SINT32 MaxChn, VIDEO_CHIP_CONF_S ChipConf, VOSD_CONF_S VosdConf);

/******************************************************************************
 * 函数介绍: 启动模块 (调用不分先后顺序)
 * 输入参数: handle: 模块句柄(此处无效)
 *           priority: 线程优先级1~99
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vosd_Start(SINT32 handle, SINT32 priority);

/******************************************************************************
 * 函数介绍: 停止模块
 * 输入参数: handle: 模块句柄(此处无效)
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vosd_Stop(SINT32 handle);

/******************************************************************************
 * 函数功能: 设置设备工作语言用于组织叠加语言
 * 输入参数: LType: 语言种类
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vosd_Set_LanguageType(SINT32 handle, LANG_TYPE_E LType);

/******************************************************************************
 * 函数功能: 获取设备工作语言
 * 输入参数: 无
 * 输出参数: pLType: 语言类型
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vosd_Get_LanguageType(SINT32 handle, LANG_TYPE_E *pLType);

/******************************************************************************
 * 函数功能: 设置叠加时间及通道名,同时设置osd参数
 * 输入参数: handle: 句柄参数(此处无效)
 *           Chn: 通道号
 *           OsdParam: 叠加osd的参数,如通道名,时间,格式等
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vosd_Chn_Set_OsdParam(SINT32 handle, SINT32 Chn, VOSD_OVERLAYPARAM_S OsdParam);

/******************************************************************************
 * 函数功能: 获取对应通道的OSD参数
 * 输入参数: handle: 句柄参数(此处无效)
 *           Chn: 查询通道号
 * 输出参数: pOsdParam: osd参数结构指针
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vosd_Chn_Get_OsdParam(SINT32 handle, SINT32 Chn, PVOSD_OVERLAYPARAM_S pOsdParam);

/******************************************************************************
 * 函数功能: 删除叠加时间及通道名
 * 输入参数: handle: 句柄参数(此处无效)
 *		     Chn: 通道号
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vosd_Chn_Del_OsdParam(SINT32 handle, SINT32 Chn);

/******************************************************************************
 * 函数功能: 获取视频叠加cover色块的参数
 * 输入参数: handle: 句柄参数(此处无效)
 *           Chn: 通道号
 * 输出参数: pVmParam: 区域屏蔽参数指针结构
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vosd_Chn_Get_VmParam(SINT32 handle, SINT32 Chn, PVOSD_VM_PARAM_S pVmParam);

/******************************************************************************
 * 函数功能: 设置视频叠加cover色块
 * 输入参数: handle: 句柄参数(此处无效)
 *		     Chn: 通道号
 *           VosdChnType: cover类型
 *			 VmParam: 屏蔽参数
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vosd_Chn_Set_CoverScreen(SINT32 handle, SINT32 Chn, VOSD_CHNTYPE_E VosdChnType, VOSD_VM_PARAM_S VmParam);

/******************************************************************************
 * 函数功能: 删除叠加cover色块
 * 输入参数: handle: 句柄参数(此处无效)
 *           Chn: 通道号
 *           VosdChnType: cover类型(VOSD_RGN_TYPE_E)
 *           enHide: 通道是否禁用(MV_BOOL)
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vosd_Chn_Del_CoverScreen(SINT32 handle, SINT32 Chn, VOSD_CHNTYPE_E VosdChnType, SINT32 enHide);

/******************************************************************************
 * 函数功能: 设置视频画线
 * 输入参数: handle: 句柄参数(此处无效)
 *		     Chn: 通道号
 *           VosdChnType: osd类型
 *			 VLineFrm: 画框参数
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vosd_Chn_Draw_Line(SINT32 handle, VOSD_CHNTYPE_E ChnType, SINT32 Chn, VLINE_FRAME_S VLineFrm);

/******************************************************************************
 * 函数功能: 删除视频画框
 * 输入参数: handle: 句柄参数(此处无效)
 *		     Chn: 通道号
 *           VosdChnType: osd类型
 *			 VLineFrm: 画框参数
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vosd_Chn_Del_Line(SINT32 handle, VOSD_CHNTYPE_E ChnType, SINT32 Chn);

/******************************************************************************
 * 函数功能: 设置视频画框
 * 输入参数: handle: 句柄参数(此处无效)
 *		     Chn: 通道号
 *           VosdChnType: osd类型
 *			 VFaceFrm: 画框参数
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vosd_Chn_Draw_Rect(SINT32 handle, VOSD_CHNTYPE_E ChnType, SINT32 Chn, VFACE_FRAME_S VFaceFrm);

/******************************************************************************
 * 函数功能: 设置视频画框
 * 输入参数: handle: 句柄参数(此处无效)
 *		     Chn: 通道号
 *           VosdChnType: osd类型
 *			 VFaceFrm: 画框参数
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vosd_Chn_Del_Rect(SINT32 handle, VOSD_CHNTYPE_E ChnType, SINT32 Chn);

/******************************************************************************
 * 函数功能: 新接口--设置venc位图叠加(用户自行实现字库管理，生成位图)，调用此接口完成osd叠加到venc通道
 * 输入参数: handle: 句柄参数(此处无效)
 *		     Chn: 位图id(一个id对应1个位图资源，多个位图此值不能一样)
 *           VOSD_CHNTYPE_E: 叠加通道类型，选择VOSD_CHNTYPE_VENC_MAIN或	VOSD_CHNTYPE_VENC_THIRD叠加到主码流或子码流
 *			 VosdOverLay: 位图参数
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vosd_Chn_SetBitMap(SINT32 handle, VOSD_CHNTYPE_E VosdChnType, SINT32 Chn, VOSD_OVERYLAY_S VosdOverLay);

/******************************************************************************
 * 函数功能:
 *新接口--通过unicode编码格式字符串设置venc位图叠加(用户自行实现字库管理，生成位图)，调用此接口完成osd叠加到venc通道
 * 输入参数: handle: 句柄参数(此处无效)
 *		     Chn: 位图id(一个id对应1个位图资源，多个位图此值不能一样)
 *           VOSD_CHNTYPE_E: 叠加通道类型，选择VOSD_CHNTYPE_VENC_MAIN或	VOSD_CHNTYPE_VENC_THIRD叠加到主码流或子码流
 *			 VosdTextOverLay: 位图参数
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vosd_Chn_SetBitMapByText(SINT32 handle, SINT32 Chn, VOSD_CHNTYPE_E VosdChnType,
                                VOSD_TEXT_OVERYLAY_S VosdTextOverLay);

/******************************************************************************
 * 函数功能:
 *新接口--获取平台位图叠加区域位图首地址(用户自行实现字库管理，生成位图)，调用此接口完成osd叠加到venc通道的属性设置，并返回平台的位图首地址
 * 输入参数: handle:句柄参数(此处无效)
 *		     Chn: 位图id(一个id对应1个位图资源，多个位图此值不能一样)
 *           VOSD_CHNTYPE_E: 叠加通道类型，选择VOSD_CHNTYPE_VENC_MAIN或	VOSD_CHNTYPE_VENC_THIRD叠加到主码流或子码流
 *			 PVosdOverLay: 位图参数，其中pData参数为平台返回位图首地址
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vosd_Chn_SetBitMapAttrToGetAddrFromPlat(SINT32 handle, SINT32 Chn, VOSD_CHNTYPE_E VosdChnType,
                                               PVOSD_OVERYLAY_S PVosdOverLay);

/******************************************************************************
 * 函数功能:
 *新接口--更新平台位图数据及显示状态(用户自行实现字库管理，生成位图)：与接口Vosd_Chn_SetBitMapAttrToGetAddrFromPlat配套使用;
 *			 更新点阵数据前停止显示，点阵数据拷贝完成后，使能显示
 * 输入参数: handle:句柄参数(此处无效)
 *			 chn:位图id(一个id对应1个位图资源，多个位图此值不能一样)
 *           VOSD_CHNTYPE_E: 叠加通道类型，选择VOSD_CHNTYPE_VENC_MAIN或	VOSD_CHNTYPE_VENC_THIRD叠加到主码流或子码流
 *			 show: 位图显示使能
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vosd_Chn_UpdateBitMapDataByPlatAddr(SINT32 handle, SINT32 Chn, VOSD_CHNTYPE_E VosdChnType, int show);

/******************************************************************************
 * 函数功能:
 *新接口--获取平台位图叠加区域位图首地址(用户自行实现字库管理，生成位图)，调用此接口完成osd叠加到venc通道的属性设置，并返回平台的位图首地址
 * 输入参数: Chn: 通道id，传0即可
 *           VOSD_CHNTYPE_E: 叠加通道类型，选择VOSD_CHNTYPE_VENC_MAIN或	VOSD_CHNTYPE_VENC_THIRD叠加到主码流或子码流
 *			 handle:位图id(一个id对应1个位图资源，多个位图此值不能一样)
 * 输出参数: pData: 平台返回位图首地址
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vosd_Chn_GetAddrFromPlatByHandle(SINT32 handle, SINT32 Chn, VOSD_CHNTYPE_E VosdChnType, void **pData);

SINT32 Vosd_Chn_DelBitMap(SINT32 handle, VOSD_CHNTYPE_E VosdChnType, SINT32 Chn);

/******************************************************************************
 * 函数功能: 模块控制
 * 输入参数: handle: 模块句柄(此处无效)
 *           cmd: 具体的控制参数命令
 *           Chn: 视频输出通道号(从0开始)
 *           param: 对应命令的参数
 *           param_len: param长度,特别对于GET命令时,输出参数应先判断缓冲区是否足够
 * 输出参数: param: 输出参数
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vosd_IoCtrl(SINT32 handle, SINT32 cmd, SINT32 Chn, SINT32 *param, SINT32 param_len);

/******************************************************************************
 * 函数介绍: 关闭VOSD模块,释放系统资源
 * 输入参数: handle: 模块句柄(此处无效)
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vosd_Close(SINT32 handle);

/******************************************************************************
 * 函数功能: 设置视频马赛克
 * 输入参数: handle: 句柄参数(此处无效)
 *          ChnType: 通道类型
 *          Chn: 马赛克通道
 *			VMosaic: 马赛克参数
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vosd_Chn_Set_MosaicParam(SINT32 handle, VOSD_CHNTYPE_E ChnType, SINT32 Chn, VMOSAIC_REGION_S VMosaic);

/******************************************************************************
 * 函数功能: 删除视频马赛克
 * 输入参数: handle: 句柄参数(此处无效)
 *			ChnType: 通道类型
 *          Chn: 马赛克通道
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vosd_Chn_Del_MosaicParam(SINT32 handle, VOSD_CHNTYPE_E ChnType, SINT32 Chn);

/******************************************************************************
 * 函数功能: 在NV12帧上叠加点阵功能
 * 输入参数: handle: 模块句柄(此处无效)
 *			 pFrame: NV12帧信息
 *			 pVosdOverLay: osd点阵信息
 * 输出参数: pFrame: 叠加了osd点阵的NV12帧信息
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vosd_DrawBitMapInNV12(SINT32 handle, VIDEO_FRM_ALG_S *pFrame, VOSD_OVERYLAY_S *pVosdOverLay);

#ifdef __cplusplus
}
#endif  //@__cplusplus
#endif  /* _VOSD_H_ */
