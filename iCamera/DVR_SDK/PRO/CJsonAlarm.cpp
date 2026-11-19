
#include <stdio.h>
#include <string.h>
#include "CJsonAlarm.h"
#include "cJSON.h"
#include "CJsonPub.h"
#include "zwsdk.h"
#include "newhie/HieServer.h"
#include "MPU_STR_API.h"
#include "MPU_DOP_API.h"
#include "MPU_AVL_API.h"
// #include "sys_banned.h"
#include "global_gather.h"



int Json_TryListenAlaramAudio(char *nBuffer, int nBufferLenth, int nTestType)
{
	int nRet = 0;

	if (NULL == nBuffer)
	{
		JSON_ERROR("nBuffer is NULL, GetSdInfo fail\n");
		return -1;
	}

	nRet = ZWSys_SetSpeechContent(SPEECH_FACTORY_AUDIO_TEST);
	if (0 != nRet)
	{
		JSON_ERROR("TryAlarmVoice error\n");
		return -1;
	}

	nRet = CJsonPubRespond(nBuffer, nBufferLenth, 1, nTestType);

	return nRet; 
}


//alarm,port默认为0，暂只测试0端口
int Json_AlarmOut(char *nBuffer, int nBufferLenth, int nTestType)
{

	int nRet = 0;
	static int nAlarmOutStatue = 1;

	if(NULL == nBuffer)
	{
		JSON_ERROR("nBuffer is NULL, GetSdInfo fail\n");
		return -1;
	}

	if(nAlarmOutStatue)
	{
		MPU_DOP_SetAlarmOutState(0, 1);
		nAlarmOutStatue = 0;
	}
	else
	{
		MPU_DOP_SetAlarmOutState(0, 0);
		nAlarmOutStatue = 1;
	}

	nRet = CJsonPubRespond(nBuffer, nBufferLenth, 1, nTestType);


	return nRet;
}



