#ifndef _AENC_H_
#define _AENC_H_

#include "acomm.h"
#include "vcomm.h"

#define AENC_CHN_MAX	(3)

typedef enum 
{
    AENC_ERROR_START = COMP_ERROR_START, 
	AENC_ERROR_UNINIT,									/* 模块未初始化*/
	AENC_ERROR_REINIT,									/* 模块重复初始化*/
    AENC_ERROR_SDK_API_ABN,                            	/* sdk接口报错33 */
    AENC_ERROR_DEVTYPE,                            		/* AUDIO设备类型不支持 */
    AENC_ERROR_CHN,                            			/* AENC 通道号超出 */
    AENC_ERROR_ENCTYPE,                            		/* 通道编码类型不支持 */
} AENC_ERROR_E;

typedef enum 
{
    AENC_CMD_NULL = CMDNO(0, COMP_AUDIOENC), 
} AENC_CMD_E;

#define AENC_TRACE_ERR(str, args...)  TRACE_ERR(COMP_AUDIOENC, str, ##args)
#define AENC_TRACE_WARN(str, args...) TRACE_WARN(COMP_AUDIOENC, str, ##args)
#define AENC_TRACE_FLOW(str, args...) TRACE_FLOW(COMP_AUDIOENC, str, ##args)
#define AENC_TRACE_INFO(str, args...) TRACE_INFO(COMP_AUDIOENC, str, ##args)
#define AENC_TRACE_DBG0(str, args...) TRACE_DBG0(COMP_AUDIOENC, str, ##args)

#define AENC_CHECK_NULL_PTR(ptr)\
    do{\
        if(NULL == ptr)\
         {\
            AENC_TRACE_ERR("NULL pointer\n");\
            return ERRNO(COMMON_ERROR_PARAM, COMP_AUDIOENC);;\
         }\
      }while(0)

#define AENC_CHECK_IS_INITED(inited)\
	do{\
		if (0 == inited)\
		{\
    		AENC_TRACE_ERR("aenc module not inited!\n");\
    		return ERRNO(AENC_ERROR_UNINIT, COMP_AUDIOENC);\
    	}\
	} while (0)

#define AENC_CHECK_IS_REINIT(inited)\
	do{\
		if (1 == inited)\
		{\
    		AENC_TRACE_ERR("aenc module reinit!\n");\
    		return ERRNO(AENC_ERROR_REINIT, COMP_AUDIOENC);\
    	}\
	} while (0)

#define AENC_CHECK_CHN_VALID(chn)\
	do{\
		if (chn >= AENC_CHN_MAX)\
		{\
			AENC_TRACE_ERR("aenc unsurpport chn(%d)!\n", chn);\
			return ERRNO(AENC_ERROR_CHN, COMP_AUDIOENC);\
		}\
	} while (0)

#define AENC_CHECK_DEV_TYPE_VALID(devType)\
	do{\
		if (AUDIO_DEV_AI_TALK != devType)\
		{\
    		AENC_TRACE_ERR("aenc unsurpport chnType(%d)!\n", devType);\
    		return ERRNO(AENC_ERROR_DEVTYPE, COMP_AUDIOENC);\
    	}\
	} while (0)

#define AENC_CHECK_ENC_TYPE_VALID(encType)\
	do{\
		if ((AUDIO_G711A != encType)\
			&& (AUDIO_G711U != encType)\
			&& (AUDIO_AAC != encType)\
			&& (AUDIO_PCM != encType))\
		{\
    		AENC_TRACE_ERR("aenc unsurpport encType(%d)!\n", encType);\
    		return ERRNO(AENC_ERROR_ENCTYPE, COMP_AUDIOENC);\
    	}\
	} while (0)


/******************************************************************************
 * 函数功能: 打开模块,分配系统资源,模块初始化
 * 输入参数: MaxChn - 当前设备最大支持视频路数(本地音频路数 + 对讲音频路数)
 *           ADataFunc -- 数据流回调
 *           NotifyFunc -- 消息流回调
 * 输出参数: 无
 * 返回值  : >0-模块句柄,<0-错误代码
 *****************************************************************************/
SINT32 Aenc_Open(SINT32 MaxChn, VData_Callback ADataFunc, Notify_Callback NotifyFunc);

/******************************************************************************
 * 函数功能: 模块开启运行
 * 输入参数: handle: 模块句柄,此处无效
 *           priority: 线程优先级
 * 输出参数: 无
 * 返回值  : >0-模块句柄,<0-错误代码
 *****************************************************************************/
SINT32 Aenc_Start(SINT32 handle, SINT32 priority);

/******************************************************************************
 * 函数介绍: 停止VO模块
 * 输入参数: handle: 模块句柄,此处无效
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Aenc_Stop(SINT32 handle);

/******************************************************************************
 * 函数介绍: 打开编码通道
 * 输入参数: AeChn: 编码通道
 *           EncType: 音频解码类型
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Aenc_Chn_Open(SINT32 handle, SINT32 AeChn, AUDIO_TYPE_E EncType, UINT32 u32NumPerFrm);

/******************************************************************************
 * 函数介绍: 关闭编码通道
 * 输入参数: AeChn: 编码通道
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Aenc_Chn_Close(SINT32 handle, SINT32 AeChn);

/******************************************************************************
 * 函数介绍: AENC通道绑定
 * 输入参数: handle: 模块句柄,此处无效
 *           BindConf: 绑定参数
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Aenc_Chn_Bind(SINT32 handle, AENC_BIND_CONF_S BindConf);

/******************************************************************************
 * 函数介绍: AENC通道解绑
 * 输入参数: handle: 模块句柄,此处无效
 *           BindConf: 绑定参数
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Aenc_Chn_UnBind(SINT32 handle, AENC_BIND_CONF_S BindConf);

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
SINT32 Aenc_IoCtrl(SINT32 handle, SINT32 cmd, SINT32 Chn, SINT32 *param, SINT32 param_len);

/******************************************************************************
 * 函数介绍: 关闭模块,释放系统资源
 * 输入参数: handle: 模块句柄,此处无效
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Aenc_Close(SINT32 handle);

/******************************************************************************
 * 函数功能: AENC通道重采样配置。当aenc编码与ai采用率不一致时使用;
 			 需在Aenc_Chn_Open接口前调用,当前仅对AAC编码通道生效;
 * 输入参数: handle: 模块句柄,此处无效
 *           AeChn: 编码通道
 *           Enable: 重采样使能
 *           InputRate: 重采样输入采样率,同ai模块初始化配置采样率
 *           OutRate: 重采样输出采样率,编码使用的采样率
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Aenc_Chn_Set_Resample(SINT32 handle, SINT32 AeChn, XRS_BOOL Enable, AUDIO_SAMPLE_RATE_E InputRate, AUDIO_SAMPLE_RATE_E OutRate);

#endif
