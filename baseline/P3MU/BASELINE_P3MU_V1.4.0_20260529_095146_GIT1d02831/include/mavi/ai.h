#ifndef _AI_H_
#define _AI_H_

#include "acomm.h"

//ai err code
typedef enum
{
    AI_ERROR_START = COMP_ERROR_START,                /* 用户模块起始错误号32 */
	AI_ERROR_UNINIT,									/* 模块未初始化*/
	AI_ERROR_REINIT,									/* 模块重复初始化*/
    AI_ERROR_SDK_API_ABN,                            	/* sdk接口报错33 */
    AI_ERROR_BINDED,                            		/* AI已被绑定 */
    AI_ERROR_UNBIND,                            		/* AI未被绑定 */
    AI_ERROR_CHN,                            			/* AI通道不支持 */
    AI_ERROR_CHNTYPE,                            		/* AI通道类型不支持 */
    AI_ERROR_DEVTYPE,                            		/* AUDIO设备类型不支持 */
    AI_ERROR_BIND_DST_CHN,                            	/* AI通道不支持 */
}AI_ERROR_E;

#define AI_TRACE_ERR(str, args...)  TRACE_ERR(COMP_AUDIOIN, str, ##args)
#define AI_TRACE_WARN(str, args...) TRACE_WARN(COMP_AUDIOIN, str, ##args)
#define AI_TRACE_FLOW(str, args...) TRACE_FLOW(COMP_AUDIOIN, str, ##args)
#define AI_TRACE_INFO(str, args...) TRACE_INFO(COMP_AUDIOIN, str, ##args)
#define AI_TRACE_DBG0(str, args...) TRACE_DBG0(COMP_AUDIOIN, str, ##args)

#define AI_BIND_DST_CHN_MAX 	(3)
#define AI_CHN_MAX 	(2)

#define AI_CHECK_NULL_PTR(ptr)\
    do{\
        if(NULL == ptr)\
         {\
            AI_TRACE_ERR("NULL pointer\n");\
            return ERRNO(COMMON_ERROR_PARAM, COMP_AUDIOIN);;\
         }\
      }while(0)

#define AI_CHECK_IS_INITED(inited)\
	do{\
		if (0 == inited)\
		{\
    		AI_TRACE_ERR("ai module not inited!\n");\
    		return ERRNO(AI_ERROR_UNINIT, COMP_AUDIOIN);\
    	}\
	} while (0)

#define AI_CHECK_IS_REINIT(inited)\
	do{\
		if (1 == inited)\
		{\
    		AI_TRACE_ERR("ai module reinit!\n");\
    		return ERRNO(AI_ERROR_REINIT, COMP_AUDIOIN);\
    	}\
	} while (0)

#define AI_CHECK_CHN_VALID(chn)\
	do{\
		if (chn >= AI_CHN_MAX)\
		{\
    		AI_TRACE_ERR("ai unsurpport chn(%d)!\n", chn);\
    		return ERRNO(AI_ERROR_CHN, COMP_AUDIOIN);\
    	}\
	} while (0)

#define AI_CHECK_CHN_TYPE_VALID(chnType)\
	do{\
		if (AI_CHTYPE_TALK != chnType)\
		{\
    		AI_TRACE_ERR("ai unsurpport chnType(%d)!\n", chnType);\
    		return ERRNO(AI_ERROR_CHNTYPE, COMP_AUDIOIN);\
    	}\
	} while (0)

#define AI_CHECK_DEV_TYPE_VALID(devType)\
	do{\
		if (AUDIO_DEV_AI_TALK != devType)\
		{\
    		AI_TRACE_ERR("ai unsurpport chnType(%d)!\n", devType);\
    		return ERRNO(AI_ERROR_DEVTYPE, COMP_AUDIOIN);\
    	}\
	} while (0)

#define AI_CHECK_BIND_DST_CHN_VALID(chn)\
	do{\
		if (chn > AI_BIND_DST_CHN_MAX)\
		{\
    		AI_TRACE_ERR("ai unsurpport chn(%d)!\n", chn);\
    		return ERRNO(AI_ERROR_CHN, COMP_AUDIOIN);\
    	}\
	} while (0)

/******************************************************************************
 * 函数功能: 打开模块,分配系统资源,模块初始化
 * 输入参数: handle: 模块句柄,此处无效
 *           AiConf -- AI模块配置信息
 *           AudioCfgMode: 音频配置模式(AUDIO_CFG_MODE_E)
 * 输出参数: 无
 * 返回值  : >0-模块句柄,<0-错误代码
 *****************************************************************************/
SINT32 Ai_Open(SINT32 handle, AI_CONF_S AiConf, AUDIO_CFG_MODE_E AudioCfgMode);

/******************************************************************************
 * 函数功能: 模块开启运行
 * 输入参数: handle: 模块句柄,此处无效
 *           priority: 线程优先级
 * 输出参数: 无
 * 返回值  : >0-模块句柄,<0-错误代码
 *****************************************************************************/
SINT32 Ai_Start(SINT32 handle, SINT32 priority);

/******************************************************************************
 * 函数介绍: 停止VO模块
 * 输入参数: handle: 模块句柄,此处无效
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Ai_Stop(SINT32 handle);

/******************************************************************************
 * 函数介绍: 打开ai通道
 * 输入参数: handle: 模块句柄,此处无效
 *           ChnType: 通道类型
 *           AiChn: 通道号
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Ai_Chn_Open(SINT32 handle, AI_CHTYPE_E ChnType, SINT32 AiChn, AUDIO_CONF_S AudioConf);

/******************************************************************************
 * 函数介绍: 关闭ai通道
 * 输入参数: handle: 模块句柄,此处无效
 *           ChnType: 通道类型
 *           AiChn: 通道号
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Ai_Chn_Close(SINT32 handle, AI_CHTYPE_E ChnType, SINT32 AiChn);

SINT32 Ai_Set_Volume(SINT32 handle,  AUDIO_DEV_TYPE_E ChnType, SINT32 AiChn, SINT32 Volume);

SINT32 Ai_Get_Volume(SINT32 handle, AUDIO_DEV_TYPE_E ChnType, SINT32 AiChn, SINT32 *pVolume);

SINT32 Ai_Chn_Get_FrmInfo(SINT32 handle, AUDIO_DEV_TYPE_E ChnType, SINT32 AiChn, PAUDIO_FRM_INFO_S pAFrmInfo);

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
SINT32 Ai_IoCtrl(SINT32 handle, SINT32 cmd, SINT32 Chn, SINT32 *param, SINT32 param_len);

/******************************************************************************
 * 函数介绍: 关闭模块,释放系统资源
 * 输入参数: handle: 模块句柄,此处无效
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Ai_Close(SINT32 handle);

/******************************************************************************
 * 函数功能: 设置AI设备增益
 * 输入参数: handle: 模块句柄,此处无效
 *           ChnType: AI设备类型
 *           AiChn: AI设备通道号
 *           Gain: 增益参数,范围[0-31]
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Ai_Set_Gain(SINT32 handle,  AUDIO_DEV_TYPE_E ChnType, SINT32 AiChn, SINT32 Gain);

/******************************************************************************
 * 函数功能: 获取AI设备增益
 * 输入参数: handle: 模块句柄,此处无效
 *           ChnType: AI设备类型
 *           AiChn: AI设备通道号
 *           pGain: AI设备增益参数指针
 * 输出参数: pGain: 增益参数,范围[0-31]
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Ai_Get_Gain(SINT32 handle, AUDIO_DEV_TYPE_E ChnType, SINT32 AiChn, SINT32 *pGain);

/******************************************************************************
 * 函数功能: 设置AI设备回声消除使能
 * 输入参数: handle: 模块句柄,此处无效
 *           AiChn: Ai通道号
 *           Enable: 回声消除使能
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Ai_Chn_Set_Aec(SINT32 handle, SINT32 AiChn, XRS_BOOL Enable);

/******************************************************************************
 * 函数功能: 设置AI设备通道自动增益配置：talkV2版本算法生效
 * 输入参数: handle: 模块句柄,此处无效
 *           AiChn: Ai通道号
 *           AgcCfg: 自动增益配置
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Ai_Chn_Set_Agc(SINT32 handle, SINT32 AiChn, AI_AGC_CONF_S AgcCfg);

#endif /* _HB_AUDIO_H_ */
