
/*!
 *\file	MPU_CIU_TransparentChannel.cpp
 *\brief	MPU模块，透明通道相关接口实现
 *\author	林云
 *\date	2009/11/9

功能：
-#	透明通道的启动，停止，读写操作

 */

#include "MPU_CIU_global.h"

//!\brief       向透明通道写数据的回调
//!\param   hIndex              透明通道句柄
//!\param   pTransparentParaInfo    透明通道数据信息
//!\return  0表示成功，其它为错误码
static int callback_TransparentChannelWrite(HANDLE hIndex, const LPTransparentParaInfo pTransparentParaInfo)
{
	return HieServer_TransparentChannelWrite(hIndex, pTransparentParaInfo);
}

//!\brief       初始化透明通道
//!\return  0表示成功，其它为错误码
int CIU_TransparentChanne_Init()
{
	int nSDKRet;

	LPTransparentChanInterface pIF;

	//从PTZ模块获取透明通道相关接口
	pIF = MPU_TPC_GetNetInterface();
	if (NULL == pIF)
	{
		MPU_ERR("call MPU_TPC_GetNetInterface() fail\n");
		return IDVR_CIU_CALLSDK_FAIL;
	}

	//设置接口到网络库
	nSDKRet = HieServer_TransparentChannelIf(pIF);
	if (0 != nSDKRet)
	{
		MPU_ERR("call HieServer_TransparentChannelIf( 0x%08X ) fail\n", (int) pIF);
		return IDVR_CIU_CALLSDK_FAIL;
	}

	//设置向透明通道写数据的回调
	nSDKRet = MPU_TPC_SetWriteCallback(callback_TransparentChannelWrite);
	if (0 != nSDKRet)
	{
		MPU_ERR("call MPU_TPC_SetWriteCallback() fail\n");
		return IDVR_CIU_CALLSDK_FAIL;
	}

	return IDVR_OK;
}
