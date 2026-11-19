#ifndef _VDA_H_
#define _VDA_H_

#include "vcomm.h"
#include "xrsstd.h"


typedef enum { VDA_WORK_MODE_MD = 0, VDA_WORK_MODE_OD, VDA_WORK_MODE_END } VDA_WORK_MODE_E;

typedef enum {
    VDA_CHANNEL_RGB_MD = 0,
    VDA_CHANNEL_IR_MD,
    VDA_CHANNEL_RBG_OD,
    VDA_CHANNEL_IR_OD,
    VDA_CHANNEL_END
} VDA_CHANNEL_TYPE_E;

typedef enum {
    VDA_ERROR_START = COMP_ERROR_START, /* 用户模块起始错误号 */
    VDA_ERROR_UNINIT,                   /* 模块未初始化 */
    VDA_ERROR_REINIT,                   /* 模块重复初始化 */
    VDA_ERROR_SDK_API_ABN,              /* sdk接口报错 */
    VDA_ERROR_INIT_MD,                  /* 初始化MD错误 */
    VDA_ERROR_DEINIT_MD,                /* 反初始化MD错误 */
    VDA_ERROR_NONE_QR,                  /* 未解码到QR数据 */
} VDA_ERROR_E;

#define VDA_TRACE_ERR(str, args...) TRACE_ERR(COMP_VIDEOVDA, str, ##args)
#define VDA_TRACE_WARN(str, args...) TRACE_WARN(COMP_VIDEOVDA, str, ##args)
#define VDA_TRACE_FLOW(str, args...) TRACE_FLOW(COMP_VIDEOVDA, str, ##args)
#define VDA_TRACE_INFO(str, args...) TRACE_INFO(COMP_VIDEOVDA, str, ##args)
#define VDA_TRACE_DBG0(str, args...) TRACE_DBG0(COMP_VIDEOVDA, str, ##args)

#define VDA_CHECK_NULL_PTR(ptr)                              \
    do {                                                     \
        if (NULL == ptr) {                                   \
            VDA_TRACE_ERR("NULL pointer\n");                 \
            return ERRNO(COMMON_ERROR_PARAM, COMP_VIDEOVDA); \
            ;                                                \
        }                                                    \
    } while (0)

#define VDA_CHECK_IS_INITED(inited)                        \
    do {                                                   \
        if (0 == inited) {                                 \
            VDA_TRACE_ERR("vda module not inited!\n");     \
            return ERRNO(VDA_ERROR_UNINIT, COMP_VIDEOVDA); \
        }                                                  \
    } while (0)

#define VDA_CHECK_IS_REINIT(inited)                        \
    do {                                                   \
        if (1 == inited) {                                 \
            VDA_TRACE_ERR("vda module reinit!\n");         \
            return ERRNO(VDA_ERROR_REINIT, COMP_VIDEOVDA); \
        }                                                  \
    } while (0)

/******************************************************************************
 * 函数功能: 打开模块,分配系统资源,模块初始化
 * 输入参数: MaxChn: 当前视频最大输入数
 *           ChipConf: 芯片配置信息
 *           VdaConf: 视频DA配置信息,其中max_vda_chn为vda最大通道数,正常情况下此两值相等
 * 输出参数: 无
 * 返回值  : >0-模块句柄,<0-错误代码
 *****************************************************************************/
SINT32 Vda_Open(SINT32 MaxChn, VIDEO_CHIP_CONF_S ChipConf, VDA_CONF_S VdaConf);

/******************************************************************************
 * 函数介绍: 启动模块
 * 输入参数: handle: 模块句柄(此处无效)
 *           priority: 线程优先级1~99
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vda_Start(SINT32 handle, SINT32 priority, SINT32 VdaChn);

/******************************************************************************
 * 函数介绍: 停止模块
 * 输入参数: handle: 模块句柄(此处无效)
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vda_Stop(SINT32 handle);

/******************************************************************************
 * 函数功能: 设置智能回放状态
 * 输入参数: handle: 模块句柄(此处无效)
 *           EnSmartSearch: 智能搜索状态(1-开启智能搜索,0-关闭智能搜索)
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vda_Set_Smart_SearchStat(SINT32 handle, UINT8 EnSmartSearch);

/******************************************************************************
 * 函数功能: 打开视频移动侦测
 * 输入参数: handle: 模块句柄,此处无效
 *           VdaChn: vda通道逻辑id,调用close时需要传递
 *		     width: 暂时无效,设置0即可,底层默认352
 *		     height: 暂时无效,设置0即可,底层默认288
 * 输出参数: 无
 * 返回值  : >0-移动侦测通道句柄,<0-错误代码
 *****************************************************************************/
SINT32 Vda_Chn_Open(SINT32 handle, VDA_CHN_ATTR_S VdaChnAttr);

/******************************************************************************
 * 函数功能: 销毁视频移动侦测
 * 输入参数: handle: 模块句柄,此处无效
 *           VdaChn: 对应open的vda_chn
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vda_Chn_Close(SINT32 handle, SINT32 VdaChn);

/******************************************************************************
 * 函数功能: VDA通道绑定(vi --> vpss --> vda)
 * 输入参数: handle: 模块句柄(此处无效)
 *           VdaChn: open时打开的vda_chn,此处传入绑定到对应的视频通道
 *           BindConf: 其中vda_src_chn填充值为视频的逻辑号
 * 输出参数: 无
 * 返回值  : >0-发送数据长度,<0-错误代码
 *****************************************************************************/
SINT32 Vda_Chn_Bind(SINT32 handle, SINT32 VdaChn, VDA_BIND_CONF_S BindConf);

/******************************************************************************
 * 函数功能: VDA通道解绑定
 * 输入参数: handle: 模块句柄(此处无效)
 *           VdaChn: 视频DA通道号
 *           BindConf: 编码数据缓冲区指针
 * 输出参数: 无
 * 返回值  : >0-发送数据长度,<0-错误代码
 *****************************************************************************/
SINT32 Vda_Chn_UnBind(SINT32 handle, SINT32 VdaChn, VDA_BIND_CONF_S BindConf);

/******************************************************************************
 * 函数功能: 设置移动侦测参数(VDA_MD_PARAM_S)
 * 输入参数: handle: 模块句柄,此处无效
 *           VdaChn: vda绑定对应视频通道号(ALL_CHANNEL表示设置所有通道)
 *           MdParam: 设置移动侦测的参数,如灵敏度,使能状态
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vda_Chn_Set_MdParam(SINT32 handle, SINT32 VdaChn, VDA_MD_PARAM_S MdParam);

/******************************************************************************
 * 函数功能: 获取移动侦测参数(VDA_MD_PARAM_S)
 * 输入参数: handle: 模块句柄,此处无效
 *           VdaChn: vda绑定对应视频通道号(不支持ALL_CHANNEL)
 * 输出参数: pMdParam: 移动侦测的配置参数指针
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vda_Chn_Get_MdParam(SINT32 handle, SINT32 VdaChn, PVDA_MD_PARAM_S pMdParam);

/******************************************************************************
 * 函数功能: 获取移动侦测状态
 * 输入参数: handle: 模块句柄,此处无效
 *           VdaChn: vda绑定对应视频通道号(不支持ALL_CHANNEL)
 * 输出参数: pMdStat: 移动侦测状态参数指针,是否有移动
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vda_Chn_Get_MdStat(SINT32 handle, SINT32 VdaChn, XRS_BOOL *pMdStat);

/******************************************************************************
 * 函数功能: 清除移动侦测状态
 * 输入参数: handle: 模块句柄,此处无效
 *           channel: vda绑定对应视频通道号(ALL_CHANNEL表示设置所有通道)
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vda_Chn_Clr_MdStat(SINT32 handle, SINT32 VdaChn);

/******************************************************************************
 * 函数功能: 获取移动侦测数据(PVDA_MD_AREADATA_S)
 * 输入参数: handle: 模块句柄,此处无效
 *           channel: 通道号
 * 输出参数: pMdData: 移动侦测数据结构指针
 * 返回值  : 0-成功,<0-错误码
 *****************************************************************************/
SINT32 Vda_Chn_Get_MdData(SINT32 handle, SINT32 VdaChn, PVDA_MD_AREADATA_S pMdData);

/******************************************************************************
 * 函数功能: 设置视频遮挡参数(主要包括视频和灵敏度)
 * 输入参数: handle: 模块句柄,此处无效
 *           VdaChn: 视频输入通道号(从0开始)
 *           OdParam: 对应视频遮挡设置参数
 * 输出参数: 无
 * 返回值  : >=0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vda_Chn_Set_OdParam(SINT32 handle, SINT32 VdaChn, VDA_OD_PARAM_S OdParam);

/******************************************************************************
 * 函数功能: 获取视频遮挡参数
 * 输入参数: handle: 模块句柄,此处无效
 *		     VdaChn: 视频输入通道号(从0开始)
 * 输出参数: pOdParam: 视频遮挡数据结构指针
 * 返回值  : >=0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vda_Chn_Get_OdParam(SINT32 handle, SINT32 VdaChn, PVDA_OD_PARAM_S pOdParam);

/******************************************************************************
 * 函数功能: 获取视频遮挡状态
 * 输入参数: handle: 模块句柄,此处无效
 *           VdaChn: 视频输入通道号(从0开始)
 * 输出参数: pOdStat: 对应channel通道的视频遮挡状态
 * 返回值  : >=0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vda_Chn_Get_OdStat(SINT32 handle, SINT32 VdaChn, XRS_BOOL *pOdStat);

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
SINT32 Vda_IoCtrl(SINT32 handle, SINT32 cmd, SINT32 Chn, SINT32 *param, SINT32 param_len);

/******************************************************************************
 * 函数介绍: 关闭VO模块,释放系统资源
 * 输入参数: handle: 模块句柄(此处无效)
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vda_Close(SINT32 handle);

/******************************************************************************
 * 函数介绍: 设置马赛克参数，供移动侦测使用排除马赛克区域使用
 * 输入参数: handle: 模块句柄(此处无效)
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vda_Set_MosaicParam(VMOSAIC_REGION_S MosaicRgn[], SINT32 MosaicCnt);

/******************************************************************************
 * 函数介绍: 解码二维码数据字符串
 * 输入参数: pFrm: yuv帧数据信息
 * 输出参数: pResult：qr结果信息
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 VDA_DECODE_QR_INFO(PVDA_QR_FRAME_INFO_S pFrm, PVDA_QR_RESULT_INFO_S pResult);

/******************************************************************************
 * 函数功能: 移动侦测送测及获取结果
 * 输入参数: handle: 模块句柄(此处无效)
 *           Chn: 视频输出通道号(从0开始)
 *           pVFrmAlg: 送入的检测帧信息
 * 输出参数: pMdData: 移动侦测数据结构指针
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vda_Chn_Send_Frame_And_Get_MdData(SINT32 handle, SINT32 VdaChn, PVIDEO_FRM_ALG_S pVFrmAlg,
                                         PVDA_MD_AREADATA_S pMdData);

/******************************************************************************
 * 函数功能: 设置哭声检测参数(主要包括使能和灵敏度等)
 * 输入参数: handle: 模块句柄,此处无效
 *           VdaChn: 视频输入通道号(从0开始)
 *           BcdParam: 对应哭声检测设置参数
 * 输出参数: 无
 * 返回值  : >=0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vda_Chn_Set_BcdParam(SINT32 handle, SINT32 VdaChn, VDA_BCD_PARAM_S BcdParam);

/******************************************************************************
 * 函数功能: 获取哭声检测参数(主要包括使能和灵敏度等)
 * 输入参数: handle: 模块句柄,此处无效
 *           VdaChn: 视频输入通道号(从0开始)
 * 输出参数: OdParam: 对应哭声检测设置参数
 * 返回值  : >=0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vda_Chn_Get_BcdParam(SINT32 handle, SINT32 VdaChn, PVDA_BCD_PARAM_S pBcdParam);

#endif /* _VDA_H_ */
