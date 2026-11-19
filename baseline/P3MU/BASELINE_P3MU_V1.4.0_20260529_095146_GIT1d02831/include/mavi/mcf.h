#ifndef __MCF_H__
#define __MCF_H__

#ifdef __cplusplus
extern "C" {
#endif  //@__cplusplus
#include "vcomm.h"

typedef enum {
    MCF_ERROR_START = COMP_ERROR_START, /* 用户模块起始错误号 */
    MCF_ERROR_PARAM,                    /* 参数错误*/
    MCF_ERROR_UNINIT,                   /* 模块未初始化*/
    MCF_ERROR_REINIT,                   /* 模块重复初始化*/
    MCF_ERROR_GRP_OPEN,                 /* 创建组错误 */
    MCF_ERROR_GRP_CLOSE,                /* 关闭组错误 */
    MCF_ERROR_CHN_OPEN,                 /* 打开通道错误 */
    MCF_ERROR_CHN_CLOSE,                /* 关闭通道错误 */
    MCF_ERROR_BIND_VI,                  /* 绑定VI失败 */
} MCF_ERROR_E;

#define MCF_ERRNO(no) ERRNO(no, COMP_VIDEOVPSS)

#define MCF_ERR(str, args...) TRACE_ERR(COMP_VIDEOVPSS, str, ##args)
#define MCF_WARN(str, args...) TRACE_WARN(COMP_VIDEOVPSS, str, ##args)
#define MCF_FLOW(str, args...) TRACE_FLOW(COMP_VIDEOVPSS, str, ##args)
#define MCF_INFO(str, args...) TRACE_INFO(COMP_VIDEOVPSS, str, ##args)
#define MCF_DBG0(str, args...) TRACE_DBG0(COMP_VIDEOVPSS, str, ##args)

typedef struct McfPipeAttr {
    UINT8 PipeId;  /* 管道ID */
    UINT8 VpssGrp; /* 绑定VPSS 组ID*/
    UINT16 Width;  /* 图像宽度 */
    UINT16 Height; /* 图像高度 */
    UINT16 Depth;  /* 缓冲区深度 */
} MCF_PIPE_ATTR_S, *PMCF_PIPE_ATTR_S;

typedef struct mcf_grp_attr {
    UINT16 Width;  /* 图像宽度 */
    UINT16 Height; /* 图像高度 */
    MCF_PIPE_ATTR_S MonoPipe;
    MCF_PIPE_ATTR_S ColorPipe;
} MCF_GRP_ATTR_S, *PMCF_GRP_ATTR_S;

typedef struct mcf_chn_attr {
    UINT16 Width;  /* 图像宽度 */
    UINT16 Height; /* 图像高度 */
} MCF_CHN_ATTR_S, *PMCF_CHN_ATTR_S;

SINT32 Mcf_Open(void);
void Mcf_Close(void);
SINT32 Mcf_BindVi(SINT32 McfGrp, SINT32 McfPipe, SINT32 ViChn);
SINT32 Mcf_UnBindVi(SINT32 McfGrp, SINT32 McfPipe, SINT32 ViChn);
SINT32 Mcf_BindVpss(SINT32 McfGrp, SINT32 McfChn, SINT32 VpssGrp, SINT32 VpssChn);
SINT32 Mcf_UnBindVpss(SINT32 McfGrp, SINT32 McfChn, SINT32 VpssGrp, SINT32 VpssChn);

/*****************************************************************************
 * 函数功能: 开启MCF组
 * 输入参数: handle: 模块句柄,此处无效
 *			McfGrp: MCF组号
 *			pMcfAttr: MCF组属性
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Mcf_StartGrp(SINT32 handle, SINT32 McfGrp, MCF_GRP_ATTR_S *pMcfAttr);

/*****************************************************************************
 * 函数功能: 停止MCF组
 * 输入参数: handle: 模块句柄,此处无效
 *			McfGrp: MCF组号
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Mcf_StopGrp(SINT32 handle, SINT32 McfGrp);

/*****************************************************************************
 * 函数功能: 开启MCF通道
 * 输入参数: handle: 模块句柄,此处无效
 *			McfGrp: MCF组号
 *			pMcfAttr: MCF组属性
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Mcf_StartChn(SINT32 handle, SINT32 McfGrp, SINT32 McfChn, MCF_CHN_ATTR_S *pMcfAttr);

/*****************************************************************************
 * 函数功能: 停止MCF通道
 * 输入参数: handle: 模块句柄,此处无效
 *			McfGrp: MCF组号
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Mcf_StopChn(SINT32 handle, SINT32 McfGrp, SINT32 McfChn);

SINT32 Mcf_StartFusion(SINT32 handle, SINT32 McfGrp);
SINT32 Mcf_StopFusion(SINT32 handle, SINT32 McfGrp);

#ifdef __cplusplus
}
#endif  //@__cplusplus
#endif  /* __MCF_H__ */