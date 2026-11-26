
#include <stdio.h>
#include <string.h>
#include "CJsonAlarm.h"
#include "cJSON.h"
#include "CJsonPub.h"
#include "zwsdk.h"
#include "newhie/HieServer.h"
#include "MPU_STR_API.h"
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
