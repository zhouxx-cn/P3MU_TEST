#ifndef _AO_H_
#define _AO_H_

#include "xrsstd.h"
#include "acomm.h"

typedef enum
{
    AO_ERROR_START = COMP_ERROR_START,
	AO_ERROR_UNINIT,									/* 模块未初始化*/
	AO_ERROR_REINIT,									/* 模块重复初始化*/
    AO_ERROR_SDK_API_ABN,                            	/* sdk接口报错33 */
    AO_ERROR_BINDED,                            		/* AO已被绑定 */
    AO_ERROR_UNBIND,                            		/* AO未被绑定 */
    AO_ERROR_CHN,                            			/* AO通道不支持 */
    AO_ERROR_SRC_DEV_TYPE,                            	/* AO绑定源设备类型不支持 */
} AO_ERROR_E;

#define AO_TRACE_ERR(str, args...)  TRACE_ERR(COMP_AUDIOOUT, str, ##args)
#define AO_TRACE_WARN(str, args...) TRACE_WARN(COMP_AUDIOOUT, str, ##args)
#define AO_TRACE_FLOW(str, args...) TRACE_FLOW(COMP_AUDIOOUT, str, ##args)
#define AO_TRACE_INFO(str, args...) TRACE_INFO(COMP_AUDIOOUT, str, ##args)
#define AO_TRACE_DBG0(str, args...) TRACE_DBG0(COMP_AUDIOOUT, str, ##args)

#define AO_CHECK_NULL_PTR(ptr)\
    do{\
        if(NULL == ptr)\
         {\
            AO_TRACE_ERR("NULL pointer\n");\
            return ERRNO(COMMON_ERROR_PARAM, COMP_AUDIOOUT);;\
         }\
      }while(0)

#define AO_CHECK_IS_INITED(inited)\
	do{\
		if (0 == inited)\
		{\
    		AO_TRACE_ERR("ao module not inited!\n");\
    		return ERRNO(AO_ERROR_UNINIT, COMP_AUDIOOUT);\
    	}\
	} while (0)

#define AO_CHECK_IS_REINIT(inited)\
	do{\
		if (1 == inited)\
		{\
    		AO_TRACE_ERR("ao module reinit!\n");\
    		return ERRNO(AO_ERROR_REINIT, COMP_AUDIOOUT);\
    	}\
	} while (0)

#define AO_CHECK_CHN_VALID(chn)\
	do{\
		if (chn > 0)\
		{\
    		AO_TRACE_ERR("ao unsurpport chn(%d)!\n", chn);\
    		return ERRNO(AO_ERROR_CHN, COMP_AUDIOOUT);\
    	}\
	} while (0)

#define AO_CHECK_SRC_DEV_TYPE_VALID(devType)\
	do{\
		if ((AUDIO_DEV_ADEC != devType)\
			&& (AUDIO_DEV_AO != devType))\
		{\
    		AO_TRACE_ERR("ao unsurpport devType(%d)!\n", devType);\
    		return ERRNO(AO_ERROR_SRC_DEV_TYPE, COMP_AUDIOOUT);\
    	}\
	} while (0)

/******************************************************************************
 * 函数功能: 打开模块,分配系统资源,模块初始化
 * 输入参数: handle: 模块句柄,此处无效
 *           AudioCfgMode - 当前设备的音频配置模式
 * 输出参数: 无
 * 返回值  : >0-模块句柄,<0-错误代码
 *****************************************************************************/
SINT32 Ao_Open(SINT32 handle, AUDIO_CONF_S AoConf, AUDIO_CFG_MODE_E AudioCfgMode);

/******************************************************************************
 * 函数功能: 模块开启运行
 * 输入参数: handle: 模块句柄,此处无效
 *           priority: 线程优先级
 * 输出参数: 无
 * 返回值  : >0-模块句柄,<0-错误代码
 *****************************************************************************/
SINT32 Ao_Start(SINT32 handle, SINT32 priority);

/******************************************************************************
 * 函数介绍: 停止模块
 * 输入参数: handle: 模块句柄,此处无效
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Ao_Stop(SINT32 handle);

/******************************************************************************
 * 函数功能: 设置AO设备音量参数
 * 输入参数: handle: 模块句柄,此处无效
 *           AoDev: AO设备号
 *           Volume: 音量参数,范围[0-100]
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Ao_Set_Volume(SINT32 handle, AO_DEV_E AoDev, SINT32 Volume);

/******************************************************************************
 * 函数功能: 获取AO设备音量参数
 * 输入参数: handle: 模块句柄,此处无效
 *           AoDev: AO设备号
 *           pVolume: 音量参数指针
 * 输出参数: pVolume: 音量参数,范围[0-100]
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Ao_Get_Volume(SINT32 handle, AO_DEV_E AoDev, SINT32 *pVolume);

/******************************************************************************
 * 函数功能: 获取AO设备通道帧数据
 * 输入参数: handle: 模块句柄,此处无效
 *           AoDev: AO设备号
 *           enMute: 静音参数
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Ao_Set_Mute(SINT32 handle, AO_DEV_E AoDev, XRS_BOOL enMute);

/******************************************************************************
 * 函数功能: AO播放
 * 输入参数: handle:    模块句柄,此处无效
 *           AoDev:     AO设备号
 *           pData:     播放数据
 *           size:      数据长度
 *           pWriteInfo:数据写入标志位信息
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Ao_Chn_Write(SINT32 handle, SINT32 AoChn, UINT8 *pData, UINT32 size, ADEC_WRITE_MODE_E WrtMode, AO_WRITE_FINISH_E wrtFin);

/******************************************************************************
 * 函数功能: 数据重采样后通过AO播放
 * 输入参数: handle:            模块句柄,此处无效
 *           AoDev:         AO设备号
 *           pData:         播放数据
 *           size:          数据长度
 *           pResampleInfo: 重采样信息结构体
 *           pWriteInfo:    数据写入标志位信息结构体
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Ao_Chn_Resample_Write(SINT32 handle, SINT32 AoChn, UINT8 *pData, UINT32 size, int src_rate, int dst_rate);

/******************************************************************************
 * 函数功能: 获取AO设备静音参数
 * 输入参数: handle: 模块句柄,此处无效
 *           AoDev: AO设备号
 *           pEnMute: 静音参数指针
 * 输出参数: pEnMute: 静音参数
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Ao_Get_Mute(SINT32 handle, AO_DEV_E AoDev, XRS_BOOL *pEnMute);

/******************************************************************************
 * 函数介绍: AO通道绑定
 * 输入参数: handle: 模块句柄,此处无效
 * 输出参数: BindConf: 绑定参数
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Ao_Chn_Bind(SINT32 handle, AO_BIND_CONF_S BindConf);

/******************************************************************************
 * 函数介绍: AO通道解绑
 * 输入参数: handle: 模块句柄,此处无效
 * 输出参数: BindConf:绑定参数
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Ao_Chn_UnBind(SINT32 handle, AO_BIND_CONF_S BindConf);

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
SINT32 Ao_IoCtrl(SINT32 handle, SINT32 cmd, SINT32 Chn, SINT32 *param, SINT32 param_len);

/******************************************************************************
 * 函数介绍: 关闭模块,释放系统资源
 * 输入参数: handle: 模块句柄,此处无效
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Ao_Close(SINT32 handle);

/******************************************************************************
 * 函数介绍: 清楚AO通道缓存数据
 * 输入参数: handle: 模块句柄,此处无效
 *           AoChn: 视频输出通道号(从0开始)
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 AO_Chn_ClearBuf(SINT32 handle, SINT32 AoChn);

/******************************************************************************
 * 函数功能: 设置AO设备增益
 * 输入参数: handle: 模块句柄,此处无效
 *           AoDev: AO设备号
 *           Volume: 增益参数,范围[0-31]
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Ao_Set_Gain(SINT32 handle, AO_DEV_E AoDev, SINT32 Gain);

/******************************************************************************
 * 函数功能: 获取AO设备增益
 * 输入参数: handle: 模块句柄,此处无效
 *           AoDev: AO设备号
 *           pGain: 音量参数指针
 * 输出参数: pGain: 音量参数,范围[0-100]
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Ao_Get_Gain(SINT32 handle, AO_DEV_E AoDev, SINT32 *pGain);

/******************************************************************************
 * 函数功能: 设置AO设备重采样使能
 * 输入参数: handle: 模块句柄,此处无效
 *           AoChn: AO通道号
 *           Enable: 重采样使能
 *           InputRate: 重采样输入采样率
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Ao_Chn_Set_Resample(SINT32 handle, SINT32 AoChn, XRS_BOOL Enable, AUDIO_SAMPLE_RATE_E InputRate);

/******************************************************************************
 * 函数功能: 设置AO设备抗啸叫算法使能：语音播报时关闭，对讲时开启。
 * 输入参数: handle: 模块句柄,此处无效
 *           AoChn: AO通道号
 *           Enable: 抗啸叫算法使能
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Ao_Chn_Set_HS(SINT32 handle, SINT32 AoChn, XRS_BOOL Enable);

/******************************************************************************
 * 函数功能: 查询AO设备通道缓存状态
 * 输入参数: handle: 模块句柄,此处无效
 *           AoChn: AO通道号
 * 输出参数: pStatus：AO通道缓存状态信息
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Ao_Chn_Query_State(SINT32 handle, SINT32 AoChn, AO_CHN_BUF_STATUS_S *pStatus);

#endif /* _AO_H_ */
