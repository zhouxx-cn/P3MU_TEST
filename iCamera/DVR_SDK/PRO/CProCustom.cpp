#include <stdio.h>
#include "CProCustom.h"
#include "zwsdk.h"
#include "CJsonPub.h"
#include "CConfig.h"
#include "global_macro.h"
#include "motoLib.h"
#include "global_gather.h"
//#define  MAXCONFIG        (13)

typedef enum
{
	eSpeechLanguageType = 0,
	eVideoOrientation,
	eTimeZone,
	eLightBoardType,
	eLightOpenStatus,
	eAlarmOutDefaultStatus,
	eVideoColor,
	eRecStream,
	eRecScheduleV2,
	eAlarmOutDelayTime,
	eHorizontalAngle,
	eVerticalAngle,
	eMotoSpeed,
	eMotoSpeedV,
	eMotoSpeedH,
	eLensOrientation,
	eHorizontalReverse,
	eVerticalReverse,
	eHorizontalStep,
	eVerticalStep,
	eUserVMaxStep,
	eCustomPicParam,
	eResolutionWidth,
	eResolutionHeight,
	ePtzLeftHorzReverse,
	ePtzLeftVertReverse,
	ePtzRightHorzReverse,
	ePtzRightVertReverse,
	ePtzLRHorzMaxStep,
	ePtzLRVertMaxStep,
	ePtzLRHorzMaxSpeed,
	ePtzLRVertMaxSpeed,
	eOsdTextUnicode,
	eOsdShowType,
	eFullAudioIn,
	eFullAudioOut,
	eHalfAudioIn,
	eHalfAudioOut,
	eKeyToCallType,
	eZoomLens,
	eTcpRelayEnable,
	MAXCONFIG
}eCustomCfgSet;

typedef enum
{
	eSupportVertical = 0,
	eSupportDigitZoom,
	eSupportMotor,
	eSupportAlarmOut,
	ebSptSwapLRViewChannel,
	ebSptSwapLRPTZChannel,
	eSupportResetButton,
	eSupportKeyToCall,
	MAXABILITY
}eCustomAblitySet;

typedef enum
{
	FULLIN,
	FULLOUT,
	HALFIN,
	HALFOUT,
}eAudioMappingType;


typedef struct tagCustomSecKeyCfg
{
	eCustomCfgSet  eCustomNo;
	char szSection[32];
	char szKey[32];
	int  nDefaultValue;
}CustomSecKeyCfg;

typedef struct tagCustomSecKeyAblity
{
	eCustomAblitySet  eCustomNo;
	char szSection[32];
	char szKey[32];
	int  nDefaultValue;
}CustomSecKeyAblity;

static tagCustomSecKeyCfg  s_stCustomCfg[MAXCONFIG] =
{
    {eSpeechLanguageType, 	"PUB",          	"SpeechLanguage",      		0},
    {eVideoOrientation,   	"BoardID-",       	"VideoOrientation",			0},
    {eTimeZone, 		  	"PUB",             	"TimeZone",            		8},
    {eLightBoardType,       "SystemFunction",   "LightBoardType",           6},
    {eLightOpenStatus,      "Device",           "DLopenStatus",             0},
    {eAlarmOutDefaultStatus,"Device",           "AlarmOutDefaultStatus",    0},
    {eVideoColor,         	"ISP_S1",          	"VideoColor",          		1},
    {eRecStream,          	"RecordCfg",       	"RecStream_00",        		0},
    {eRecScheduleV2,        "RecordCfg",        "ScheduleRecordType00",     0},
    {eAlarmOutDelayTime,  	"AlarmOutCfg",     	"AlarmOutDelayTime_1", 		30},
    {eHorizontalAngle,    	"PTZ_Set_0",       	"HorizontalAngle",     		198},
    {eVerticalAngle,     	"PTZ_Set_0",       	"VerticalAngle",       		64},
    {eMotoSpeed,          	"PTZ_Set_0",       	"MotoSpeed",           		5},
    {eMotoSpeedV,         	"PTZ_Set_0",       	"MotorSpeedV",         		5},
    {eMotoSpeedH,         	"PTZ_Set_0",       	"MotorSpeedH",         		5},
    {eLensOrientation,      "SystemFunction",   "LensOrientation",          0},
    {eHorizontalReverse,    "PTZ_Set_0",        "HorizontalReverse",        0},
    {eVerticalReverse,      "PTZ_Set_0",        "VerticalReverse",          0},
    {eHorizontalStep,       "PTZ_Set_0",        "HorizontalStep",          396},
    {eVerticalStep,         "PTZ_Set_0",        "VerticalStep",            128},
    {eUserVMaxStep,         "PTZ_Set_0",        "UserVMaxStep",            160},
    {eCustomPicParam,     	"OpticsCfg1",      	"OpticsValueCh00_Seg0",  	50},
    {eResolutionWidth,   	"RecordCfg",       	"MainCoderFormatWidth00",	1280},
	{ePtzLeftHorzReverse, "MultiPtzDevCfg",  "PtzLeftHorzReverse",			0},
	{ePtzLeftVertReverse, "MultiPtzDevCfg",  "PtzLeftVertReverse",          0},
	{ePtzRightHorzReverse,"MultiPtzDevCfg",  "PtzRightHorzReverse",			0},
	{ePtzRightVertReverse,"MultiPtzDevCfg",  "PtzRightVertReverse",         0},
	{ePtzLRHorzMaxStep,   "MultiPtzDevCfg",  "PtzLRHorzMaxStep",			150},
	{ePtzLRVertMaxStep,   "MultiPtzDevCfg",  "PtzLRVertMaxStep",            150},
	{ePtzLRHorzMaxSpeed,  "MultiPtzDevCfg",  "PtzLRHorzMaxSpeed",			8},
	{ePtzLRVertMaxSpeed,  "MultiPtzDevCfg",  "PtzLRVertMaxSpeed",           8},
	{eOsdTextUnicode,		"OsdCfg",       	"OsdV2Text00Unicode00_00",	1280},
	{eOsdShowType,			"OsdCfg",       	"OsdShowType00",			2},
	{eFullAudioIn,			"AudioMapping",      "FullAudioIn",				92},
	{eFullAudioOut,			"AudioMapping",      "FullAudioOut",			92},
	{eHalfAudioIn,			"AudioMapping",      "HalfAudioIn",				88},
	{eHalfAudioOut,			"AudioMapping",      "HalfAudioOut",			82},
	{eKeyToCallType,		"SystemFunction",    "KeyToCallType",			1},
	{eZoomLens,				"ZoomLensCfg",    	 "ZoomLensCombination",		1},
	{eTcpRelayEnable,         "TcpRelay",        "TcpEnable",               0},
};

static tagCustomSecKeyAblity  s_stCustomAblity[MAXABILITY] =
{
	{eSupportMotor,      	"FunctionSupport", 	"IsSupportMotor",      		0},
	{eSupportVertical,      "FunctionSupport", 	"IsSupportVertical",      	0},
	{eSupportDigitZoom,     "FunctionSupport",  "IsSupportDigitZoom",      	0},
	{eSupportAlarmOut,      "FunctionSupport",  "IsSupportAlarmOut",        0},
    {ebSptSwapLRViewChannel,  "FunctionSupport",  "IsSupportSwapLRViewChannel",     0},
    {ebSptSwapLRPTZChannel,   "FunctionSupport",  "IsSupportSwapLRPTZChannel",      0},
	{eSupportResetButton,   "FunctionSupport",  "IsSupportAlternateResetButton",    0},
    {eSupportKeyToCall,   	"FunctionSupport",  "IsSupportKeyToCall",    			0},
};

//!\brief 具体实现命令
//!\return 成功为0,否则为-1
//!\param eCustomNo 编号, nValue 值
static int ChangeCustomCfg(eCustomCfgSet eCustomNo, int nValue)
{
    int nRet = 0;
    switch (eCustomNo)
    {
    case eSpeechLanguageType:
    {
        nRet = ZWSys_SetSpeechLanType(eSpeechLanguage_Chinese);
    }
    break;
    default:
        JSON_ERROR("eCustomNo error eCustomNo = %d\n", eCustomNo);
        break;
    }
    return nRet;
}

//!\brief 具体实现命令
//!\return 成功为0,否则为-1
//!\param eCustomNo 编号, nValue 值
static int ChangeCustomAblity(eCustomAblitySet eCustomNo, int nValue)
{
	int nRet = 0;
	CConfig *pCfgFile = CConfig::GetInstance();

	switch (eCustomNo)
    {
    case eSupportDigitZoom:
    {
        pCfgFile->SetValue("FunctionSupport", "IsSupportDigitZoom", nValue);
        pCfgFile->SaveFile();
    }
    break;
    case eSupportAlarmOut:
    {
        nRet = ZWSys_AlarmOutSupport(nValue);
    }
    break;
    case ebSptSwapLRViewChannel:
    {
        ZWSys_SetSptSwapLRPTZChannel(nValue);
    }
    break;
#ifdef __ONE_CLICK_CALL__
		case eSupportResetButton:
		{
			ZWSys_SetAlternateResetButton(nValue);
		}
        break;
        case eSupportKeyToCall:
		{
			ZWSys_SetKeyToCall(nValue);
		}
		break;
#endif
		default:
			JSON_ERROR("eCustomNo error eCustomNo = %d\n", eCustomNo);
			break;
        }

        return nRet;
}

//!\brief 配置实现命令
//!\return 成功为0,否则为-1
//!\param
int ProCustomRealize()
{
	int s32Ret = 0;
	int i = 0;
	int nValue = 0;
	int nBoardID = 0;
	char szSection[32] = {0};
	CConfig *pCustomCfg = NULL;

	pCustomCfg = CConfig::NewInstance();

	ZWSys_GetBoardInfo(&nBoardID);

	//从custom定制获取能力支持
	s32Ret = pCustomCfg->LoadFile(CUSTOM_CONFIG_FILE);

	if (0 != s32Ret)
	{
		return -1;
	}

	for(i = 0; i< MAXCONFIG; i++)
	{
		if (!strncmp(s_stCustomCfg[i].szKey, "VideoOrientation", sizeof("VideoOrientation")))
		{
			snprintf(szSection, sizeof(szSection), "BoardID-%x", nBoardID);

			if(pCustomCfg->CheckKeyExist(szSection, s_stCustomCfg[i].szKey))
			{
				nValue = pCustomCfg->GetValue(szSection, s_stCustomCfg[i].szKey, s_stCustomCfg[i].nDefaultValue);

				ChangeCustomCfg(s_stCustomCfg[i].eCustomNo, nValue);
			}
		}
		else
		{
			if(pCustomCfg->CheckKeyExist(s_stCustomCfg[i].szSection, s_stCustomCfg[i].szKey))
			{
				nValue = pCustomCfg->GetValue(s_stCustomCfg[i].szSection, s_stCustomCfg[i].szKey, \
					s_stCustomCfg[i].nDefaultValue);

				ChangeCustomCfg(s_stCustomCfg[i].eCustomNo, nValue);
			}
		}
	}

	CConfig::ReleaseInstance(pCustomCfg);

	CConfig *pCustomAblity = NULL;
	pCustomAblity = CConfig::NewInstance();

	//从custom定制获取能力支持
	s32Ret = pCustomAblity->LoadFile(CUSTOM_ABILITY_FILE);

	if (0 != s32Ret)
	{
		return -1;
	}

	for(i = 0; i< MAXABILITY; i++)
	{
		if(pCustomAblity->CheckKeyExist(s_stCustomAblity[i].szSection, s_stCustomAblity[i].szKey))
		{
			nValue = pCustomAblity->GetValue(s_stCustomAblity[i].szSection, s_stCustomAblity[i].szKey, \
				s_stCustomAblity[i].nDefaultValue);

			ChangeCustomAblity(s_stCustomAblity[i].eCustomNo, nValue);
		}
	}

	CConfig::ReleaseInstance(pCustomAblity);

	return 0;
}
