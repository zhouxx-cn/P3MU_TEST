#ifndef _ADEC_H_
#define _ADEC_H_

#include "acomm.h"

#define ADEC_CHN_MAX	(1)

typedef enum 
{
    ADEC_ERROR_START = COMP_ERROR_START, 
	ADEC_ERROR_UNINIT,									/* 模块未初始化*/
	ADEC_ERROR_REINIT,									/* 模块重复初始化*/
    ADEC_ERROR_SDK_API_ABN,                            	/* sdk接口报错33 */
    ADEC_ERROR_DEVTYPE,                            		/* AUDIO设备类型不支持 */
    ADEC_ERROR_CHN,                            			/* ADEC 通道号超出 */
    ADEC_ERROR_BINDED,                            		/* AI已被绑定 */
    ADEC_ERROR_UNBIND,                            		/* AI未被绑定 */
} ADEC_ERROR_E;


#define ADEC_TRACE_ERR(str, args...)  TRACE_ERR(COMP_AUDIODEC, str, ##args)
#define ADEC_TRACE_WARN(str, args...) TRACE_WARN(COMP_AUDIODEC, str, ##args)
#define ADEC_TRACE_FLOW(str, args...) TRACE_FLOW(COMP_AUDIODEC, str, ##args)
#define ADEC_TRACE_INFO(str, args...) TRACE_INFO(COMP_AUDIODEC, str, ##args)
#define ADEC_TRACE_DBG0(str, args...) TRACE_DBG0(COMP_AUDIODEC, str, ##args)

#define ADEC_CHECK_NULL_PTR(ptr)\
    do{\
        if(NULL == ptr)\
         {\
            ADEC_TRACE_ERR("NULL pointer\n");\
            return ERRNO(COMMON_ERROR_PARAM, COMP_AUDIODEC);;\
         }\
      }while(0)

#define ADEC_CHECK_IS_INITED(inited)\
	do{\
		if (0 == inited)\
		{\
    		ADEC_TRACE_ERR("adec module not inited!\n");\
    		return ERRNO(ADEC_ERROR_UNINIT, COMP_AUDIODEC);\
    	}\
	} while (0)

#define ADEC_CHECK_IS_REINIT(inited)\
	do{\
		if (1 == inited)\
		{\
    		ADEC_TRACE_ERR("adec module reinit!\n");\
    		return ERRNO(ADEC_ERROR_REINIT, COMP_AUDIODEC);\
    	}\
	} while (0)

#define ADEC_CHECK_CHN_VALID(chn)\
	do{\
		if (chn >= ADEC_CHN_MAX)\
		{\
			ADEC_TRACE_ERR("adec unsurpport chn(%d)!\n", chn);\
			return ERRNO(ADEC_ERROR_CHN, COMP_AUDIODEC);\
		}\
	} while (0)


#define ADEC_CHECK_DEV_TYPE_VALID(devType)\
	do{\
		if (AUDIO_DEV_AI_TALK != devType)\
		{\
    		ADEC_TRACE_ERR("adec unsurpport chnType(%d)!\n", devType);\
    		return ERRNO(ADEC_ERROR_DEVTYPE, COMP_AUDIODEC);\
    	}\
	} while (0)

/******************************************************************************
 * 函数功能: 打开模块,分配系统资源,模块初始化
 * 输入参数: handle: 模块句柄(此处无效)
 *           MaxChn - 设备支持最大解码路数
 * 输出参数: 无
 * 返回值  : >0-模块句柄,<0-错误代码
 *****************************************************************************/
SINT32 Adec_Open(SINT32 handle, SINT32 MaxChn);

/******************************************************************************
 * 函数功能: 启动模块
 * 输入参数: handle: 模块句柄(此处无效)
 *           priority: 线程优先级
 * 输出参数: 无
 * 返回值  : >=0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Adec_Start(SINT32 handle, SINT32 priority);

/******************************************************************************
 * 函数功能: 停止模块
 * 输入参数: handle: 模块句柄(此处无效)
 * 输出参数: 无
 * 返回值  : >=0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Adec_Stop(SINT32 handle);

SINT32 Adec_ClearBuf(SINT32 handle, SINT32 AdecChn);

/******************************************************************************
 * 函数功能: 打开解码通道
 * 输入参数: handle: 模块句柄(此处无效)
 *           AdecChn: 解码通道号
 *           ChnConf: 解码配置信息结构体
 * 输出参数: 无
 * 返回值  : >=0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Adec_Chn_Open(SINT32 handle, SINT32 AdecChn, ADEC_CHN_CONF_S ChnConf);

/******************************************************************************
 * 函数功能: 关闭解码通道
 * 输入参数: handle: 模块句柄(此处无效)
 *           AdecChn: 解码通道号
 * 输出参数: 无
 * 返回值  : >=0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Adec_Chn_Close(SINT32 handle, SINT32 AdecChn);

/******************************************************************************
 * 函数功能: 接收音频数据,送ADEC音频解码(报警,对讲和回放)
 * 输入参数: handle: 模块句柄(此处无效)
 *           AdecChn: 解码通道号
 *           pData: 解码音频数据指针
 *           size: 音频数据大小
 *           WrtMode: 解码写入阻塞模式
 * 输出参数: 无
 * 返回值  : >=0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Adec_Chn_Write(SINT32 handle, SINT32 AdecChn, UINT8 *pData, UINT32 size, ADEC_WRITE_MODE_E WrtMode, ADEC_WRITE_FINISH_E wrtFin);

/******************************************************************************
 * 函数功能: 接收音频数据,送ADEC音频转码(实际是开源库转码AAC/MP3)
 * 输入参数: handle: 模块句柄(此处无效)
 *           AdecChn: 解码通道号
 *           pInData: 解码音频数据指针
 *           InSize: 音频数据大小
 *           pOutPcm: 转码后输出的PCM数据
 *           pAudioInfo: 输出PCM音频的信息
 * 输出参数: 无
 * 返回值  : >=0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Adec_Chn_User_Decode(SINT32 handle, SINT32 AdecChn, UINT8 *pInData, UINT32 InSize, UINT8 *pOutPcm, AIUDIO_USER_FRAME_INFO_S *pAudioInfo);

/******************************************************************************
 * 函数功能: 设置ADEC音频解码配置(报警,对讲和回放)
 * 输入参数: handle: 模块句柄(此处无效)
 *           AdecChn: 解码通道号
 *           ChnConf: 解码配置信息结构体
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Adec_Chn_Set_Conf(SINT32 handle, SINT32 AdecChn, ADEC_CHN_CONF_S ChnConf);

/******************************************************************************
 * 函数功能: 接收音频数据,送ADEC音频解码(报警,对讲和回放)
 * 输入参数: handle: 模块句柄(此处无效)
 *           AdecChn: 解码通道号
 *           pChnConf: 解码配置信息结构体指针
 * 输出参数: pChnConf: 解码配置信息结构体
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Adec_Chn_Get_Conf(SINT32 handle, SINT32 AdecChn, PADEC_CHN_CONF_S pChnConf);

/******************************************************************************
 * 函数功能: 模块控制
 * 输入参数: handle: 模块句柄(此处无效)
 *           cmd: 控制参数命令
 *           Chn: 通道号(从0开始)
 *           param: 对应命令的参数
 *           param_len: param长度,特别对于GET命令时,输出参数应先判断缓冲区是否足够
 * 输出参数: param: 输出参数
 * 返回值  : >=0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Adec_IoCtrl(SINT32 handle, SINT32 cmd, SINT32 Chn, SINT32 *param, SINT32 param_len);

/******************************************************************************
 * 函数介绍: 关闭模块,释放系统资源
 * 输入参数: handle: 模块句柄(此处无效)
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Adec_Close(SINT32 handle);

/******************************************************************************
 * 函数功能: amr编码音频解码为pcm格式
 * 输入参数: handle: 模块句柄(此处无效)
 *           pInFilePath: 解码文件路径
 *           pOutFilePath: 解码后输出文件路径
 * 输出参数: 无
 * 返回值  : >=0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Adec_DecAmr2Pcm(SINT32 handle, UINT8 *pInFilePath, UINT8 *pOutFilePath);

/******************************************************************************
 * 函数功能: amr编码音频转码为g771u格式
 * 输入参数: handle: 模块句柄(此处无效)
 *           pInFilePath: 解码文件路径
 *           pOutFilePath: 转码后输出文件路径
 * 输出参数: 无
 * 返回值  : >=0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Adec_DecAmr2G711u(SINT32 handle, UINT8 *pInFilePath, UINT8 *pOutFilePath);

/******************************************************************************
 * 函数功能: 解析帧头获取帧数据长度
 * 输入参数: handle: 模块句柄(此处无效)
 *           frmHead: 帧头数据
 * 输出参数: frmLen: 帧长
 * 返回值  : >=0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Adec_GetAmrFrmLen(SINT32 handle, UINT8 frmHead, UINT32 *frmLen);
#endif /* _ADEC_H_ */
