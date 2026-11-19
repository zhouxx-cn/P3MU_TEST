
#include <stdio.h>
#include "CJsonSys.h"
#include "CJsonPub.h"
#include "./MPU/MPU_global.h"
#include "common_pub.h"
// #include "sys_banned.h"
#include "global_gather.h"

static void *RestoreFactoryTest(void *parm)
{
    PRT_TID();
	pthread_detach(pthread_self());
	sleep(1);
#if defined (__IS_SUPPORT_APP_SHARE__)
	if (IsSupportDevShareFunc())
	{
		MPU_SYS_DeviceBind_ResetSCode();
		sleep(3);
	}
#endif
	JSON_INFO("System start restore factory!!!\n");
	USR_ResumeAdminPassword();
	PAR_ForbiddenSaveFile();
	PAR_NET_Resume(__FUNCTION__);
	PAR_UnForbiddenSaveFile();
	pthread_exit(NULL);
}

int Json_FactoryDefault(char *nBuffer , int nBufferLenth, int nTestType)
{
	int nRet = 0;
	pthread_t m_Thread;
	
	if(NULL == nBuffer)
	{
		JSON_ERROR("nBuffer is NULL, FactoryDefault fail\n");
		return -1;
	}

#ifdef __MODULE_4G_SUPPORT__
	// 停止与转发服务器发消息
	MPU_NET_SetConnServerStatus(0);
#endif
    
	//异步处理，让网络回应
	pthread_create(&m_Thread, NULL, RestoreFactoryTest, NULL);

	nRet = CJsonPubRespond(nBuffer, nBufferLenth, 1, nTestType);

	return nRet;
}


int Json_HardResetTest(char *nBuffer , int nBufferLenth, int nTestType)
{
	int nRet = 0;
	int nStatus = 0;

	if(NULL == nBuffer)
	{
		return -1;
	}

	//nRet = ZWSys_GetResetPinStatus(&nStatus);

	nRet = MPU_SYS_GetHardResetStatus(&nStatus);
	
	if(0 != nRet)
	{
		JSON_ERROR("nBuffer is NULL, HardResetTest fail\n");
		return -1;
	}

	nRet = CJsonPubRespond(nBuffer, nBufferLenth, nStatus, nTestType);

    //如果获取到状态有按过复位键， 则清空状态
	if(0 == nRet && 1 == nStatus)
	{
	#if defined (__IS_SUPPORT_APP_SHARE__)
		if (IsSupportDevShareFunc())
		{
			MPU_SYS_DeviceBind_ResetSCode();
		}
	#endif
		MPU_SYS_ClearHardResetStatus();
	}

	return nRet;

}

int Json_SensorCntCorrect(char *nBuffer, int nBufferLenth, char *nFirParam, int nFirParamLenth)
{
    int nRet = 0;
	int nStatus = 0;

    if (NULL == nBuffer)
    {
        JSON_ERROR("nBuffer is NULL, GetWifiTestStatue fail\n");
        return -1;
    }

    int CheckCnt = atoi(nFirParam);
    nStatus = ZWSys_GetSensorConfCorrect(CheckCnt);

    nRet = CJsonPubRespond(nBuffer, nBufferLenth, nStatus, eOtherTest);

    return nRet;
}

#ifdef __ONE_CLICK_CALL__
int Json_HardKeyTocallTest(char *nBuffer , int nBufferLenth, int nTestType)
{
	int nRet = 0;
	int nStatus = 0;
	int nHangupStatus = 0;
	if (NULL == nBuffer)
	{
		return -1;
	}

	nRet = ZWSys_GetCallPinStatus_Test(&nStatus, &nHangupStatus);
	if (0 != nRet)
	{
		JSON_ERROR("nBuffer is NULL, HardResetTest fail\n");
		return -1;
	}

	if (eKeyToCall == nTestType)
    {
    	nRet = CJsonPubRespond(nBuffer, nBufferLenth, nStatus, nTestType);

        //如果获取到状态有按过一键呼叫键， 则清空状态
    	if (0 == nRet && 1 == nStatus)
    	{
    		ZWSys_SetCallPinStatus_Test(0, nHangupStatus);
    	}
    }
    else if (eKeyToCallHangup == nTestType)
    {
    	nRet = CJsonPubRespond(nBuffer, nBufferLenth, nHangupStatus, nTestType);

        //如果获取到状态有按过一键呼叫键， 则清空状态
    	if (0 == nRet && 1 == nHangupStatus)
    	{
    		ZWSys_SetCallPinStatus_Test(nStatus, 0);
    	}
    }

	return nRet;
}
#endif
