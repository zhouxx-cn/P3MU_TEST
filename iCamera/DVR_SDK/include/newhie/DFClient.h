#ifndef DFCLIENT_H

//DFClient接口定义文件
#define DFCLIENT_H

#if defined( __cplusplus)
#define DFCLIENT_API  extern "C" 
#else
#define DFCLIENT_API  
#endif

#include "DFClient_Common.h"

/*!
* @brief
*	启动服务
* @param interval
*	间隔时间
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
DFCLIENT_API int DFClient_Start(DF_IN HY_DVR_DF_INPUT_PARAM cInputParam, DF_OUT HY_DVR_DF_INTERVAL_INFO* pIntervalInfo);

/*!
* @brief
*	获取配置
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
DFCLIENT_API int DFClient_GetDeviceCFG(DF_IN char* DID, DF_OUT HY_DVR_DF_DEVICE_CFG* pDeviceCfg);


/*!
* @brief
*	获取simcard类型，是否是北纬卡
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
DFCLIENT_API int DFClient_GetBWCardType(DF_IN char* Iccid, DF_IN int Type, DF_OUT int* SimCardType);

/*!
* @brief
*	获取simcard类型，是否是北纬卡
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
DFCLIENT_API int DFClient_GetBWCardTypeV2(DF_IN char* Iccid, DF_IN int Type, DF_OUT int* SimCardType, DF_IN HY_DVR_DF_BWCARD_INFO* pBWCardInfo);

/*!
* @brief
*	查询是否在云仓库中
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
DFCLIENT_API int DFClient_GetIsFlowCard(DF_IN char* Iccid, DF_OUT int* IsFlowCard);

/*!
* @brief
*	停止服务,断开所有协议及远程连接
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
DFCLIENT_API int DFClient_Stop();

/*!
* @brief
*	查询经销商
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
DFCLIENT_API int DFClient_GetDealerInfo(DF_IN char* Iccid, DF_IN char* Did, DF_OUT int *resp);

#endif 
