
#include <stdio.h>
#include "CJsonLightTest.h"
#include "CJsonPub.h"
#include "zwsdk.h"
// #include "sys_banned.h"
#include "global_gather.h"
int Json_WhiteLightTest(char *nBuffer, int nBufferLenth, int nTestType)
{
	int nRet = 0;
	static int s_nWhiteLightStatus =0;
	AssistantCtrlInfo stAssitInfo;
	if(NULL == nBuffer)
	{
		JSON_ERROR("nBuffer is NULL, pls check!\n");
		return -1;
	}
	
	stAssitInfo.enAssisstantType = PDEV_WHITE_LED;
	if(s_nWhiteLightStatus)
	{
		stAssitInfo.nCtrlStatus = 0;
		s_nWhiteLightStatus = 0;
	}
	else
	{
		stAssitInfo.nCtrlStatus = 1;
		s_nWhiteLightStatus = 1;
	}
	
	
	nRet = ZWSys_AssistantControl(&stAssitInfo);
	if (0 != nRet)
	{
		JSON_ERROR("White Led test errror\n");
		return -1;
	}
	nRet = CJsonPubRespond(nBuffer, nBufferLenth, 1, nTestType);
	return nRet;
}

int Json_RedLightTest(char *nBuffer, int nBufferLenth, int nTestType)
{
	int nRet = 0;
	static int s_nRedLightStatus =0;
	AssistantCtrlInfo stAssitInfo;

	if(NULL == nBuffer)
	{
		JSON_ERROR("nBuffer is NULL, pls check!\n");
		return -1;
	}
	
	stAssitInfo.enAssisstantType = PDEV_INFRARED_LED;
	if(s_nRedLightStatus)
	{
		stAssitInfo.nCtrlStatus = 0;
		s_nRedLightStatus = 0;
	}
	else
	{
		stAssitInfo.nCtrlStatus = 1;
		s_nRedLightStatus = 1;
	}
	
	nRet = ZWSys_AssistantControl(&stAssitInfo);
	if (0 != nRet)
	{
		JSON_ERROR("White Led test errror\n");
		return -1;
	}
	nRet = CJsonPubRespond(nBuffer, nBufferLenth, 1, nTestType);
	
	return nRet;
}


int Json_AdcStatus(char *nBuffer, int nBufferLenth, int nTestType)
{
	int nRet = 0;
	int nAdcStatus = 0;

	if(NULL == nBuffer)
	{
		JSON_ERROR("nBuffer is NULL, pls check!\n");
		return -1;
	}

	nRet = ZWSys_GetAdcStatus(&nAdcStatus);
	if(nRet)
	{
		JSON_ERROR("GetAdcStatus fail!\n");
		return -1;
	}

	nRet = CJsonPubRespond(nBuffer, nBufferLenth, nAdcStatus, nTestType);
	
	return nRet;
}



