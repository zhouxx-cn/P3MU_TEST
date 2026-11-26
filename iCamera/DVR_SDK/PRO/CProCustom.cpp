#include <stdio.h>
#include "CProCustom.h"
#include "zwsdk.h"
#include "CJsonPub.h"
#include "CConfig.h"
#include "global_macro.h"
#include "./MPU/MPU_global.h"
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
	IrcutTimeModeInfo info;
	void *hChannelHandle = ZWSys_EncChannelOpen(0);
	CConfig *pCfgFile = CConfig::GetInstance();
	char szMirrorModeSection[30] = {0};
	int BoardInfo = 0;

	switch (eCustomNo)
	{
		case eSpeechLanguageType:
		{
			int nLanguagetype = 0;
			if ((int)eSpeechLanguage_English == nValue || (int)eSpeechLanguage_Thai == nValue)
			{
				nLanguagetype = nValue;
			}
			else
			{
				nLanguagetype = (int)eSpeechLanguage_Chinese;
			}
			nRet = ZWSys_SetSpeechLanType((eSpeechLanguage)nLanguagetype);
		}
		break;
		case eVideoOrientation:
		{
			// 获取版型
			ZWSys_GetBoardInfo(&BoardInfo);

			snprintf(szMirrorModeSection, sizeof(szMirrorModeSection), "BoardID-%x", BoardInfo);

			if(nValue != pCfgFile->GetValue(szMirrorModeSection, "VideoOrientation", 0))
			{
				pCfgFile->SetValue(szMirrorModeSection, "VideoOrientation", nValue);
				FLIP_ROTATE_MODE_E nVideoOrientation;

				if (0 == nValue)//正常
				{
					nVideoOrientation = FR_NORMAL;
				}
				else if (1 == nValue)//镜像
				{
					nVideoOrientation = FR_HORI_FLIP;
				}
				else if (2 == nValue)//翻转+ 镜像
				{
					nVideoOrientation = FR_VERT_FLIP;
				}
				else if (3 == nValue)//翻转
				{
					nVideoOrientation = FR_ROTATE_180;
				}
				else
				{
					nVideoOrientation = FR_NORMAL;
				}
				nRet = ZWSys_SetFlipRotateMode(hChannelHandle,nVideoOrientation);
			}
		}
		break;
		case eTimeZone:
		{
			CConfig *pCfg = CConfig::NewInstance();
			nRet = pCfg->LoadFileRO(CUSTOM_CONFIGMODE_FILE);
            if (nRet == 0)
            {
                char strTimeZone[6] = {0};
                pCfg->GetValue("PUB", "TimeZone", "8", strTimeZone, sizeof(strTimeZone)-1);
                nValue = (int)(atof(strTimeZone) * 60);
            }
            else
            {
                nValue = (int)(nValue * 60);
            }
            CConfig::ReleaseInstance(pCfg);

			nRet = MPU_SYS_SetTimeZone(nValue);
		}
		break;
		case eLightBoardType:
		{
			nRet = ZWSys_LightBoardType((int)nValue);
		}
		break;
        case eLightOpenStatus:
		{
			nRet = ZWSys_SetLightOpenStatus(nValue);
		}
		break;
		case eAlarmOutDefaultStatus:
		{
			nRet = ZWSys_AlarmOutDefaultStatus(nValue);
		}
		break;
		case eVideoColor:
		{
			info.nStartHour = pCfgFile->GetValue("ISP_S1", "DayNightStartHour", 7);
			info.nStartMin = pCfgFile->GetValue("ISP_S1", "DayNightStartMin", 0);
			info.nStartSec= pCfgFile->GetValue("ISP_S1", "DayNightStartSec", 0);
			info.nStopHour = pCfgFile->GetValue("ISP_S1", "DayNightStopHour", 19);
			info.nStopMin= pCfgFile->GetValue("ISP_S1", "DayNightStopMin", 0);
			info.nStopSec= pCfgFile->GetValue("ISP_S1", "DayNightStopSec", 0);
			nRet = ZWSys_EncSetDayNightMode(hChannelHandle, (DAYNIGHTMODE_T)nValue, &info);
		}
		break;
		case eRecStream:
		{
#if defined (__SUPPORT_SD_CARD__)
            nRet =  MPU_AVL_Record_SetRecStreamIndex(0, nValue);
#endif
		}
		break;
        case eRecScheduleV2:
        {
#if defined (__SUPPORT_SD_CARD__)
            tagRecordScheduleV2 sched = {0};
            //获取录像计划时间表
            nRet = MPU_AVL_Record_GetNewSchedule(&sched);
            if (0 == nRet)
            {
                sched.nRecordType = nValue;
                nRet = MPU_AVL_Record_SetNewSchedule(0, &sched);
            }
#endif
		}
        break;
        case eLensOrientation:
        {
            nRet = ZWSys_SetLensOrientation((unsigned int)nValue);
        }
		break;
		case eCustomPicParam:
		{
			CConfig *pCfg = CConfig::NewInstance();
			char nPicParam[20] = {0};
			int nBright = 0;
			int Contrast = 0;
			int Saturation = 0;
			int Chroma =0;
			int Sharpness = 0;
			int ret = -1;
			nRet = pCfg->LoadFile(CUSTOM_CONFIGMODE_FILE);
			if(0 == nRet)
			{
				pCfg->GetValue("OpticsCfg1", "OpticsValueCh00_Seg0", "50,50,50,50,50", nPicParam, sizeof(nPicParam));
				sscanf(nPicParam, "%d,%d,%d,%d,%d", &nBright, &Contrast, &Saturation, &Chroma, &Sharpness);
				/*设置亮度 对比度 饱和度 色度 锐度*/
				ret = MPU_AVL_Pic_SetPicParam(0, 0, 0, nBright);
					MPU_CHK(0 == ret);
				ret = MPU_AVL_Pic_SetPicParam(0, 0, 1, Contrast);
					MPU_CHK(0 == ret);
				ret = MPU_AVL_Pic_SetPicParam(0, 0, 2, Saturation);
					MPU_CHK(0 == ret);
				ret = MPU_AVL_Pic_SetPicParam(0, 0, 3, Chroma);
					MPU_CHK(0 == ret);
				ret = MPU_AVL_Pic_SetPicParam(0, 0, 4, Sharpness);
					MPU_CHK(0 == ret);
			}
			CConfig::ReleaseInstance(pCfg);
		}
		break;
		case eResolutionWidth:
		{
			char key_str[36] = {0};
			int m_channel = 0;
			int nMainVideoWidth = 0;
			int nMainVideoHeight = 0;
			tagEncCfg EncCfg;
			CConfig *pCConfig = CConfig::NewInstance();
			memset(&EncCfg, 0, sizeof (EncCfg));
			nRet = pCConfig->LoadFile(CUSTOM_CONFIGMODE_FILE);
			if(0 == nRet)
			{
				snprintf(key_str,sizeof(key_str), "MainCoderFormatWidth%02d", m_channel);
				nMainVideoWidth = pCConfig->GetValue("RecordCfg", key_str, 0);

				snprintf(key_str,sizeof(key_str), "MainCoderFormatHeight%02d", m_channel);
				nMainVideoHeight = pCConfig->GetValue("RecordCfg", key_str, 0);
				/* 获取主码流编码参数 */
				nRet = MPU_AVL_Record_GetEncCfg(0, &EncCfg, 0);
				EncCfg.dwResWidth = nMainVideoWidth;
				EncCfg.dwResHeight = nMainVideoHeight;
				nRet = MPU_AVL_Record_SetEncCfg(0, &EncCfg, 0);
			}
			CConfig::ReleaseInstance(pCConfig);
		}
		break;
		case eResolutionHeight:
		{
			/*因需同时设置，故放在上面*/
		}
		break;
#if (DEVICE_SUPPORT_PTZ_NUM >= 3)
        /*多云台反转*/
        case ePtzLeftHorzReverse:
        {
            nRet = MPU_PTZ_SetHorizontalReverse(PTZ_LERF, nValue);
        }
        break;
        case ePtzRightHorzReverse:
        {
            nRet = MPU_PTZ_SetHorizontalReverse(PTZ_RIGHT, nValue);
        }
        break;
        case ePtzLeftVertReverse:
        {
            nRet = MPU_PTZ_SetVerticalReverse(PTZ_LERF, nValue);
        }
        break;
        case ePtzRightVertReverse:
        {
            nRet = MPU_PTZ_SetVerticalReverse(PTZ_RIGHT, nValue);
        }
        break;
        /*多云台最大步长*/
        case ePtzLRHorzMaxStep:
        {
            nRet = MPU_PTZ_SetHorizontalStep(PTZ_LERF, nValue);
        }
        break;
        case ePtzLRVertMaxStep:
        {
            nRet = MPU_PTZ_SetVerticalStep(PTZ_LERF, nValue);
        }
        break;
        /*多云台最大速度*/
        case ePtzLRHorzMaxSpeed:
        {
            nRet = MPU_PTZ_SetMaxSpeedH(PTZ_LERF, nValue);
        }
        break;
        case ePtzLRVertMaxSpeed:
        {
            nRet = MPU_PTZ_SetMaxSpeedV(PTZ_LERF, nValue);
        }
        break;
#endif
		case eOsdTextUnicode:
		{
			char name_unicde[256] = {0};
			char key_str[32] = {0};
			int m_channel = 0;
			tagEncCfg EncCfg;
			CConfig *pCConfig = CConfig::NewInstance();
			memset(&EncCfg, 0, sizeof (EncCfg));
			nRet = pCConfig->LoadFile(CUSTOM_CONFIGMODE_FILE);
			if(0 == nRet)
			{
				snprintf(key_str,sizeof(key_str), "OsdV2Text00Unicode00_%02d", m_channel);
				pCConfig->GetValue("OsdCfg",key_str,"",name_unicde,sizeof(name_unicde));
				MPU_AVL_SetTextOSD_Cprocustom(name_unicde);
			}
			CConfig::ReleaseInstance(pCConfig);
		}
		break;
		case eOsdShowType:
		{
			MPU_AVL_OSD_SetType(0,nValue);
		}
		break;
		case eFullAudioIn:
		{
			MPU_AVL_SetAudioMappingVol(0,nValue,FULLIN);
		}
		break;
		case eFullAudioOut:
		{
			MPU_AVL_SetAudioMappingVol(0,nValue,FULLOUT);
		}
		break;
		case eHalfAudioIn:
		{
			MPU_AVL_SetAudioMappingVol(0,nValue,HALFIN);
		}
		break;
		case eHalfAudioOut:
		{
			MPU_AVL_SetAudioMappingVol(0,nValue,HALFOUT);
		}
		break;
#ifdef __ONE_CLICK_CALL__
        case eKeyToCallType:
		{
			ZWSys_SetKeyToCallHardwareType(nValue);
		}
		break;
#endif
#ifdef __IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__
		case eZoomLens:
		{
			MPU_ZOOM_SetZoomLens(nValue);
		}
		break;
#endif
		case eTcpRelayEnable:
		{
			nRet = MPU_Net_TcpRelay_SetEnable(&nValue);
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
