#ifndef _VDEC_H_
#define _VDEC_H_

#ifdef __cplusplus
extern "C" {
#endif  //@__cplusplus
#include "vcomm.h"
#include "xrsstd.h"

#define MAX_VDEC_CHN_NUM 1


typedef struct {
    UINT32 Width;  /* 视频宽度 */
    UINT32 Height;   /* 视频高度 */
    UINT32 Resvd[2];   /* 保留位 */
}VDEC_CHN_ATTR_S, *PVDEC_CHN_ATTR_S;

typedef enum {
    VDEC_ERROR_MSSTDOPEN = COMP_ERROR_START, /* 打开MSSTD错误(32) */
    VDEC_ERROR_UNINIT,                       /* 模块未初始化33 */
    VDEC_ERROR_REINIT,                       /* 模块重复初始化34 */
    VDEC_ERROR_STARTVDEC,                    /* 启动编码错误46 */
    VDEC_ERROR_STOPVDEC,                     /* 停止编码错误50 */
    VDEC_ERROR_CREATECHN,                    /* 创建编码通道错误51 */
    VDEC_ERROR_DESTROYCHN,                   /* 销毁编码通道错误52 */
    VDEC_ERROR_SDKAPIABN,                    /* sdk encoder接口报错54 */
    VDEC_ERROR_INVALIDCHN,                   /* 不支持的通道 */
} VDEC_ERROR_E;

#define VDEC_TRACE_ERR(str, args...) TRACE_ERR(COMP_VIDEODEC, str, ##args)
#define VDEC_TRACE_WARN(str, args...) TRACE_WARN(COMP_VIDEODEC, str, ##args)
#define VDEC_TRACE_FLOW(str, args...) TRACE_FLOW(COMP_VIDEODEC, str, ##args)
#define VDEC_TRACE_INFO(str, args...) TRACE_INFO(COMP_VIDEODEC, str, ##args)
#define VDEC_TRACE_DBG0(str, args...) TRACE_DBG0(COMP_VIDEODEC, str, ##args)

#define VDEC_CHECK_NULL_PTR(ptr)                             \
    do {                                                     \
        if (NULL == ptr) {                                   \
            VDEC_TRACE_ERR("NULL pointer\n");                \
            return ERRNO(COMMON_ERROR_PARAM, COMP_VIDEODEC); \
            ;                                                \
        }                                                    \
    } while (0)

#define VDEC_CHECK_IS_INITED(inited)                        \
    do {                                                    \
        if (0 == inited) {                                  \
            VDEC_TRACE_ERR("vdec module not inited!\n");    \
            return ERRNO(VDEC_ERROR_UNINIT, COMP_VIDEODEC); \
        }                                                   \
    } while (0)

#define VDEC_CHECK_IS_REINIT(inited)                        \
    do {                                                    \
        if (1 == inited) {                                  \
            VDEC_TRACE_ERR("vdec module reinit!\n");        \
            return ERRNO(VDEC_ERROR_REINIT, COMP_VIDEODEC); \
        }                                                   \
    } while (0)

#define VDEC_CHECK_CHN_VALID(chn)                            \
    do {                                                     \
        if (chn >= MAX_VDEC_CHN_NUM) {                       \
            VDEC_TRACE_ERR("vdec chn is unvalid!\n");              \
            return ERRNO(COMMON_ERROR_PARAM, COMP_VIDEODEC); \
        }                                                    \
    } while (0)

/******************************************************************************
 * 函数介绍: 向LCD发送RGB565帧数据回调函数
 * 输入参数: handle: 句柄
 *           pData: RGB565数据
 *           size: 数据大小
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
typedef SINT32 (*Lcd_SendFrame_Callback)(SINT32 handle, UINT8 *pData, UINT32 Size);

/******************************************************************************
 * 函数功能: 打开模块,分配系统资源,模块初始化
 * 输入参数: VdecConf: 解码配置信息
 *           SendFunc: 向LCD发送数据回调函数
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vdec_Open(VDEC_CONF_S VdecConf, Lcd_SendFrame_Callback SendFunc);

/******************************************************************************
 * 函数介绍: 启动模块
 * 输入参数: handle: 模块句柄,此处无效
 *           priority: 线程优先级1~99
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vdec_Start(SINT32 handle, SINT32 priority);

/******************************************************************************
 * 函数介绍: 停止模块
 * 输入参数: handle: 模块句柄,此处无效
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vdec_Stop(SINT32 handle);

/******************************************************************************
 * 函数功能: 创建视频解码通道
 * 输入参数: handle: 模块句柄(此处无效)
 *           Type: 解码类型
 *           VdecChn: 解码通道号，暂无用
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vdec_Chn_Open(SINT32 handle, VPAYLOAD_TYPE_E Type, SINT32 VdecChn, VDEC_CHN_ATTR_S *pAttr) ;

/******************************************************************************
 * 函数功能: 启动视频解码通道
 * 输入参数: handle: 模块句柄(此处无效)
 *           Type: 解码类型
 *           VdecChn: 解码通道号，暂无用
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vdec_Chn_Start(SINT32 handle, VPAYLOAD_TYPE_E Type, SINT32 VdecChn);

/******************************************************************************
 * 函数功能: 停止视频解码通道
 * 输入参数: handle: 模块句柄(此处无效)
 *           Type: 解码类型
 *           VdecChn: 解码通道号，暂无用
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vdec_Chn_Stop(SINT32 handle, VPAYLOAD_TYPE_E Type, SINT32 VdecChn);

/******************************************************************************
 * 函数功能: 销毁视频解码通道
 * 输入参数: handle: 模块句柄(此处无效)
 *           ChnType: 编码通道类型
 *           VdecChn: 视频输入通道号,对应Vdec_Chn_Open的通道号
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vdec_Chn_Close(SINT32 handle, VPAYLOAD_TYPE_E Type, SINT32 VdecChn);

/******************************************************************************
 * 函数功能: 向解码器送流并送入LCD显示
 * 输入参数: handle: 模块句柄(此处无效)
 *           Type: 解码类型
 *           VdecChn: 解码通道号，暂无用
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vdec_Chn_SendStream(SINT32 handle, VPAYLOAD_TYPE_E Type, SINT32 VdecChn, UINT8 *pData, UINT32 Size);

/******************************************************************************
 * 函数功能: 停止向解码器发流
 * 输入参数: handle: 模块句柄(此处无效)
 *           Type: 解码类型
 *           VdecChn: 解码通道号，暂无用
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vdec_Chn_StopStream(SINT32 handle, VPAYLOAD_TYPE_E Type, SINT32 VdecChn);

/******************************************************************************
 * 函数功能: 向解码器送Jpeg并转为RGB565帧数据
 * 输入参数: handle: 模块句柄(此处无效)
 *           Type: 解码类型
 *           VdecChn: 解码通道号，暂无用
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vdec_Chn_SendJpegToRgb565(SINT32 handle, VPAYLOAD_TYPE_E Type, SINT32 VdecChn, UINT8 *pInData, UINT32 InSize, UINT8 *pOutData, UINT32 *OutSize);

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
SINT32 Vdec_IoCtrl(SINT32 handle, SINT32 cmd, SINT32 Chn, SINT32 *param, SINT32 param_len);

/******************************************************************************
 * 函数介绍: 关闭模块,释放系统资源
 * 输入参数: handle: 模块句柄,此处无效
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Vdec_Close(SINT32 handle);

#ifdef __cplusplus
}
#endif  //@__cplusplus
#endif  /* _VDEC_H_ */
