#ifndef PUB_LANGUAGE_H__
#define PUB_LANGUAGE_H__

#include "../MPU/MPU_global.h"
#include <string.h>

#define SPEENC_LANGUAGE_PATH "/mnt/app/custom/speech"

//新添语言已WIKI上管理的为准
typedef struct AUDIO_ALARM_LANGUAGE_TYPE_S
{
	int m_bVaild;                       //当前设备是否支持该种语言
    char m_bDevicecap[32];              //对应DeviceCap下面能力名
	char m_bDirName[32];                //对应分区里语言目录名
    eSpeechLanguage m_bIpcLanguageType; //对应IPC语言枚举
	eSystemLanguage m_bLanguageType;    //对应协议语言枚举
}AUDIO_ALARM_LANGUAGE_TYPE;

#ifdef __cplusplus
extern "C" {
#endif

extern AUDIO_ALARM_LANGUAGE_TYPE language_type[37];

#ifdef __cplusplus
}
#endif

int PUB_GetLanguageFromCfg (eSpeechLanguage *peLanguage);

int PUB_GetDeviceSupportLanguageType(char *dirName, int *result);

int PUB_LanguageInit();

int PUB_CheckLanguageType(eSystemLanguage voice_language);

int PUB_GetLanguageCustomValue(eSpeechLanguage *value);

int PUB_SetLanguageType(eSpeechLanguage value);

int PUB_GetLanguageTypeFromCfg();

int PUB_GetVoiceLanguage(HY_DVR_VOICE_BROADCAST_LANGUAGE *pcfg);

int PUB_SetVoiceLanguage(HY_DVR_VOICE_BROADCAST_LANGUAGE *pcfg);

int PUB_changetoHiLanguage_str(eSpeechLanguage ipc_lauguage, char *hi_language, size_t lenth);

int PUB_changetoHiLanguage(eSpeechLanguage ipc_lauguage, eSystemLanguage *hi_language);

int PUB_changetoIpcLanguage(char* hi_language, eSpeechLanguage *ipc_language);

const char* PUB_SpeechLanguage_enum2str();

#endif // PUB_LANGUAGE_H__