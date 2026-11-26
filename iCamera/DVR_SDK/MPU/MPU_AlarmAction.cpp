
/*!
 *\file	MPU_AlarmAction.cpp
 *\brief	MPU模块，报警联动处理
 *\author	林云
 *\date		2009/04/09
 */

#include "MPU_global.h"
#include "tagPtzCode.h"
#include <sys/time.h>
#include "MPU_CIU_global.h"
#include "common_pub.h"
#if defined (__IS_SUPPORT_LONGAWAYHOME__)
#include "include/newhie/P6SMSGPush_Common.h"
#endif

/**************************************************************************
 *                                                     声明局部宏定义                                                  *
 **************************************************************************/

//调用MPU接口执行动作
#define MPU_CALL(x)																\
do																				\
{																				\
	nRet = x;																		\
																				\
	if (IDVR_OK != nRet)															\
	{																			\
		MPU_ERR("%s, return %d\n", #x, nRet);										\
	}																			\
}																				\
while (0)

static int  g_bHasGetDeviceCap = 0;
static int  g_bUiEnableAI = 0;
static int  g_bUiFC = 0;
static int  g_bUiOP = 0;
static int  g_bUiAI = 0;
static int  g_bSupportSubKey = 0;

unsigned long long get_ms()
{
    struct timeval time_v = {0};
    gettimeofday(&time_v, NULL);
    return (unsigned long long)time_v.tv_sec * 1000 + time_v.tv_usec / 1000;
}

static int callback_AlarmAction_GUI(struct tagAlertInfo *pAlertInfo, struct tagActionInfo *pActionInfo)
{
	int nSDKRet;
    UNUSED(nSDKRet);

	//printf("my add: ---callback_AlarmAction_GUI--pAlertInfo->AlertType=%d\n",pAlertInfo->AlertType);
	//通知GUI
	switch (pAlertInfo->AlertType)
	{
		case ALERT_TYPE_ALARMIN:
			{
				//通知GUI界面
				if (NULL != g_pMpuInfo->m_pCallback_AlarmIn)
				{
					(*g_pMpuInfo->m_pCallback_AlarmIn) (pAlertInfo, pActionInfo);
				}

				//报警输入事件，通知网络
				nSDKRet = CIU_RefreshAlarmEvent(pAlertInfo);
				//MPU_CHK(0 == nSDKRet);
			}
			break;

		case ALERT_TYPE_MOTION:
			{
				if (NULL != g_pMpuInfo->m_pCallback_Motion)
				{
					(*g_pMpuInfo->m_pCallback_Motion) (pAlertInfo, pActionInfo);
				}

				//报警输入事件，通知网络
				nSDKRet = CIU_RefreshAlarmEvent(pAlertInfo);
				//MPU_CHK(0 == nSDKRet);
			}
			break;
			
		case ALERT_TYPE_HUMANSHAPE:
			{
				if (NULL != g_pMpuInfo->m_pCallback_HumanShape)
				{
					(*g_pMpuInfo->m_pCallback_HumanShape) (pAlertInfo, pActionInfo);
				}

				//报警输入事件，通知网络
				nSDKRet = CIU_RefreshAlarmEvent(pAlertInfo);
				//MPU_CHK(0 == nSDKRet);
			}
			break;

		case ALERT_TYPE_SOUNDDETECT:
			{
				if (NULL != g_pMpuInfo->m_pCallback_Sound)
				{
					(*g_pMpuInfo->m_pCallback_Sound) (pAlertInfo, pActionInfo);
				}

				//报警输入事件，通知网络
				nSDKRet = CIU_RefreshAlarmEvent(pAlertInfo);
				//MPU_CHK(0 == nSDKRet);
			}
			break;

		case ALERT_TYPE_VIDEOLOST:
			{
				if (NULL != g_pMpuInfo->m_pCallback_VideoLost)
				{
					(*g_pMpuInfo->m_pCallback_VideoLost) (pAlertInfo, pActionInfo);
				}

				//视频丢失，通知网络
				//pAlertInfo->dwReserved == 0   表示视频丢失
				//pActionInfo->dwReserved == 1  表示有布防
				if ((0 == pAlertInfo->dwReserved) && (1 == pActionInfo->dwReserved))
				{
					nSDKRet = CIU_RefreshAlarmEvent(pAlertInfo);
					//MPU_CHK(0 == nSDKRet);
				}
				//MPU_DBG("nChannel  %d  ALERT_TYPE_VIDEOLOST\n", pAlertInfo->nChannel);
			}
			break;

		case ALERT_TYPE_ENCODEERROR:
			{
				if (NULL != g_pMpuInfo->m_pCallback_EncodeError)
				{
					(*g_pMpuInfo->m_pCallback_EncodeError) (pAlertInfo, pActionInfo);
				}

				//编码错误,通知网络
				nSDKRet = CIU_RefreshAlarmEvent(pAlertInfo);
				//MPU_CHK(0 == nSDKRet);
			}
			break;

		case ALERT_TYPE_VIDEOEXCEPTION:
			{
				if (NULL != g_pMpuInfo->m_pCallback_VideoException)
				{
					(*g_pMpuInfo->m_pCallback_VideoException) (pAlertInfo, pActionInfo);
				}

				//视频异常,通知网络
				nSDKRet = CIU_RefreshAlarmEvent(pAlertInfo);
				//MPU_CHK(0 == nSDKRet);
			}
			break;

		case ALERT_TYPE_DISKERROR:
			{
				if (NULL != g_pMpuInfo->m_pCallback_DiskError)
				{
					(*g_pMpuInfo->m_pCallback_DiskError) (pAlertInfo, pActionInfo);
				}

				//硬盘错误，通知网络
				nSDKRet = CIU_RefreshAlarmEvent(pAlertInfo);
				//MPU_CHK(0 == nSDKRet);
			}
			break;

		case ALERT_TYPE_DISKFULL:
			{
				if (NULL != g_pMpuInfo->m_pCallback_DiskFull)
				{
					(*g_pMpuInfo->m_pCallback_DiskFull) (pAlertInfo, pActionInfo);
				}

				//硬盘满，通知网络
				nSDKRet = CIU_RefreshAlarmEvent(pAlertInfo);
				//MPU_CHK(0 == nSDKRet);
			}
			break;

		case ALERT_TYPE_IPCONFLICT:
			{
				if (NULL != g_pMpuInfo->m_pCallback_IpConflict)
				{
					(*g_pMpuInfo->m_pCallback_IpConflict) (pAlertInfo, pActionInfo);
				}

				//IP冲突，通知网络
				nSDKRet = CIU_RefreshAlarmEvent(pAlertInfo);
				//MPU_CHK(0 == nSDKRet);
			}
			break;

		case ALERT_TYPE_RETICLEDISCONNECT:
			{
				if (NULL != g_pMpuInfo->m_pCallback_NetworkDisconnect)
				{
					(*g_pMpuInfo->m_pCallback_NetworkDisconnect) (pAlertInfo, pActionInfo);
				}
			}
			break;

		case ALERT_TYPE_ILLEGEACCESS:
			{
				if (NULL != g_pMpuInfo->m_pCallback_IllegeAccess)
				{
					(*g_pMpuInfo->m_pCallback_IllegeAccess) (pAlertInfo, pActionInfo);
				}

				//非法访问，通知网络
				nSDKRet = CIU_RefreshAlarmEvent(pAlertInfo);
				//MPU_CHK(0 == nSDKRet);
			}
			break;

		case ALERT_TYPE_DISKGROUP_ERROR:
			{
				if (NULL != g_pMpuInfo->m_pCallback_DiskGroupError)
				{
					(*g_pMpuInfo->m_pCallback_DiskGroupError) (pAlertInfo, pActionInfo);
				}

				//盘组异常，通知网络
				nSDKRet = CIU_RefreshAlarmEvent(pAlertInfo);
				//MPU_CHK(0 == nSDKRet);
			}
			break;
		case ALERT_TYPE_IMODE:
			{
				//盘组异常，通知网络
				nSDKRet = CIU_RefreshAlarmEvent(pAlertInfo);
				//MPU_CHK(0 == nSDKRet);
			}
			break;
		case ALERT_TYPE_IPCHANGED:
			{		
				//盘组异常，通知网络
				nSDKRet = CIU_RefreshAlarmEvent(pAlertInfo);
				//MPU_CHK(0 == nSDKRet);
			}
			break;

		default:
			{
				MPU_ERR("unsupported AlertType = %d\n", pAlertInfo->AlertType);
			}
			break;
	};

	return IDVR_OK;
}

//推送动作
static int callback_AlarmAction_Push(int Chn,struct tagAlertInfo *pAlertInfo, struct tagActionInfo *pActionInfo)
{
	if (NULL == pAlertInfo || NULL == pActionInfo)
	{
		MPU_ERR("\n[%s][%d]Param is NULL\n",
				__FUNCTION__, __LINE__);
		return -1;
	}

	int nSDKRet = IDVR_OK;
	int nTimeZone = 0;
	char szTmp[256] = {0};
    struct timeval now_tv = {0, 0};
	char *pPicBuffer = NULL;

	ZWP2P_PUSH_EVENT_INFO struEventInfo = {0};
	CConfig *pConfig = NULL;

	memset(&struEventInfo, 0, sizeof(struEventInfo));

	/* 每次推送都需要获取配置文件，这样是否合适？ */
	pConfig = CConfig::GetInstance();
	if (NULL != pConfig)
	{
		pConfig->GetValue("iDVR_Alias", "NameUTF8", 
			"IPC", struEventInfo.cDeviceName, sizeof(struEventInfo.cDeviceName));
		snprintf(szTmp, sizeof(szTmp), "ChannelAlias_%d", pAlertInfo->nChannel);
		pConfig->GetValue(szTmp, "NameUTF8", 
			"IPC", struEventInfo.cChannelName, sizeof(struEventInfo.cChannelName));
	}

    /* 只允许在开始的时候读取一次配置就行了 */
    if (0 == g_bHasGetDeviceCap)
    {
        CConfig *pCfg = CConfig::NewInstance();
        if (pCfg)
        {
            if(0 == pCfg->LoadFileRO(CUSTOM_INFO))
            {
                g_bUiEnableAI = pCfg->GetValue("PushAI", "UiEnableAI", 0);
                g_bUiFC = pCfg->GetValue("PushAI", "UiFC", 0);
                g_bUiOP = pCfg->GetValue("PushAI", "UiOP", 0);
                g_bUiAI = pCfg->GetValue("PushAI", "UiAI", 0);
            }
            pCfg->ReleaseInstance(pCfg); 
            pCfg = NULL;
        }

        /* 获取当前是否支持Subkey */
        g_bSupportSubKey = MPU_NET_IsSupportSetSubkeyByDID();

        g_bHasGetDeviceCap = 1;
    }

    nTimeZone = MPU_SYS_GetTimeZone(); // 获取时区
	struEventInfo.dwTimeZone = MPU_SYS_GetTimeZone();
	gettimeofday(&now_tv, NULL); // 获取时间
	struEventInfo.uiDeviceTime = now_tv.tv_sec;
	// APP上显示的事件时间
    ISO8601TimeGet(now_tv.tv_sec, nTimeZone, struEventInfo.cEventCreateTime, sizeof(struEventInfo.cEventCreateTime));
#ifdef __USE_CIRCLEQUEUE__
    PullRealStreamInterface MjpegStreamInterface = {0};
	DWORD ConnectionId = 0;
	RealStreamMedia realStreamMedia;

	memset(&realStreamMedia, 0, sizeof(RealStreamMedia));
	if(CIU_GetMjpegStreamCircleQueueInterface(&MjpegStreamInterface) < 0)
	{
		return -1;
	}
	
	nSDKRet = MjpegStreamInterface.ifRealStreamConnectionAdd(eProtocolBegin, 0, Chn, eMediaCodefourthStream, NULL, &ConnectionId);
	if(!nSDKRet)
	{
		nSDKRet = MjpegStreamInterface.ifRealStreamConnectionGetData(ConnectionId, &realStreamMedia, NULL);			
		if(!nSDKRet)
		{
			if(realStreamMedia.cBuffer.dwBufLen > 28)
			{
				pPicBuffer = (char *)malloc(realStreamMedia.cBuffer.dwBufLen);
				if(pPicBuffer)
				{
					memcpy(pPicBuffer, realStreamMedia.cBuffer.pBuffer + 28, realStreamMedia.cBuffer.dwBufLen - 28);
					struEventInfo.uiResourceType = 1;
					struEventInfo.pMediaData = pPicBuffer;
					struEventInfo.nMediaDataLen = realStreamMedia.cBuffer.dwBufLen - 28;
				}
			}
			else
			{
				MPU_ERR("[%s][%d]get mjpeg frame length=%d error!\n", __FUNCTION__, __LINE__, realStreamMedia.cBuffer.dwBufLen);
			}

			nSDKRet = MjpegStreamInterface.ifRealStreamConnectionReleaseData(ConnectionId, &realStreamMedia, NULL);	
			if(nSDKRet)
			{
				MPU_ERR("[%s][%d]ifRealStreamConnectionReleaseData fail!\n", __FUNCTION__, __LINE__);
			}
		}
		else
		{
			MPU_ERR("[%s][%d]ifRealStreamConnectionGetData fail!\n", __FUNCTION__, __LINE__);
		}

		nSDKRet = MjpegStreamInterface.ifRealStreamConnectionDelete(ConnectionId, NULL);
		if(nSDKRet)
		{
			MPU_ERR("[%s][%d]ifRealStreamConnectionDelete fail!\n", __FUNCTION__, __LINE__);
		}
	}
	else
	{
		MPU_ERR("[%s][%d]ifRealStreamConnectionAdd fail!\n", __FUNCTION__, __LINE__);
	}
#endif


    struEventInfo.uiEnableAI = g_bUiEnableAI;
    struEventInfo.uiFC = g_bUiFC;
    struEventInfo.uiOP = g_bUiOP;
    struEventInfo.uiAI = g_bUiAI;
    int tutkStatus = 0;
    MPU_NET_GetTutkStatus(&tutkStatus);
    if (eP2PStauts_InternetReady != tutkStatus)
    {
        printf("[PUSHEVENT] my add: tutkStatus=%d is no ready!\n",tutkStatus);
        if (pPicBuffer)
        {
            free(pPicBuffer);
            pPicBuffer = NULL;
        }
        return 0;
    }
    
	switch (pAlertInfo->AlertType)
	{
		case ALERT_TYPE_MOTION:
		{
			MPU_INFO("ALERT_TYPE_MOTION\n");
			strncpy(struEventInfo.cPushTitle, "Motion Alert", sizeof(struEventInfo.cPushTitle)-1);
			nSDKRet = MPU_Net_ZwP2pPushEvent(eDeviceChannelEventTypeMotion, Chn+1, &struEventInfo);
			break;
		}
        case ALERT_TYPE_HUMANSHAPE:
		{
            MPU_INFO("ALERT_TYPE_HUMANSHAPE\n");
            strncpy(struEventInfo.cPushTitle, "HumanShape Alert", sizeof(struEventInfo.cPushTitle)-1);
            nSDKRet = MPU_Net_ZwP2pPushEvent(eDeviceChannelEventTypeAIPeopleDetect, Chn+1, &struEventInfo);
            break;
		}
		case ALERT_TYPE_SOUNDDETECT:
		{
			MPU_INFO("ALERT_SOUNDDETECT\n");
			strncpy(struEventInfo.cPushTitle, "SoundDetect Alert", sizeof(struEventInfo.cPushTitle)-1);
			nSDKRet = MPU_Net_ZwP2pPushEvent(eDeviceEventCrying, 1, &struEventInfo);
			break;
		}

#if defined (__IS_SUPPORT_LONGAWAYHOME__)
		case ALERT_TYPE_LONG_AWAY:
		{
            MPU_INFO("ALERT_TYPE_LONG_AWAY\n");

            time_t stime = 0, etime = 0;
            struct tm start_tm, end_tm, curr_tm;

            /* 获取长久离家检测时间 */
            MPU_AVL_HumanShape_GetLongAwayPushAlarmTime(0, &stime, &etime);
            localtime_r(&stime, &start_tm);
            localtime_r(&etime, &end_tm);
            localtime_r(&now_tv.tv_sec, &curr_tm);

            P6SMSG_PUSH_LONG_AWAY struLongAway = {0};
            strncpy(struLongAway.stDeviceInfo.strOperator, "LongAway", sizeof(struLongAway.stDeviceInfo.strOperator)-1);
            ZWSys_GetSerialNum((unsigned char *)struLongAway.stDeviceInfo.strSerialNumber);
            strncpy(struLongAway.stDeviceInfo.strPlatformID, "1234556666", sizeof(struLongAway.stDeviceInfo.strPlatformID)-1);
            struLongAway.nEventID = 1;
            // 格式：YYYYMMDDTHHMMSS+08
            strftime(struLongAway.strTime, sizeof(struLongAway.strTime), "%Y%m%dT%H%M%S+08", &curr_tm);
            strftime(struLongAway.strStartTime, sizeof(struLongAway.strStartTime), "%Y%m%dT%H%M%S+08", &start_tm);
            strftime(struLongAway.strEndTime, sizeof(struLongAway.strEndTime), "%Y%m%dT%H%M%S+08", &end_tm);

            struEventInfo.pStructInfo = (ZWP2P_UINT32*)(&struLongAway);
            strncpy(struEventInfo.cPushTitle, "LongAway Alert", sizeof(struEventInfo.cPushTitle)-1);
            nSDKRet = MPU_Net_ZwP2pPushEvent(eDeviceCustomEventTypeLongAway, 1, &struEventInfo);
            break;
		}
#endif
	};

    if (pPicBuffer)
    {
        free(pPicBuffer);
        pPicBuffer = NULL;
    }

	return nSDKRet;
}

//!\brief       报警、异常联动处理
//!\return   成功为0，其它为错误码
int callback_AlarmAction(struct tagAlertInfo *pAlertInfo, struct tagActionInfo *pActionInfo)
{
	int nRet = IDVR_OK;

    if (NULL == g_pMpuInfo)
	{
		return IDVR_ERROR_INVALID_POINTER;
	}

	if (g_bShutdown)
	{
		return IDVR_MPU_IN_SHUTDOWN_PROCESS;
	}

	//判断输入参数合法性
	MPU_CHECK_POINTER_PARAMETER(pAlertInfo);
	MPU_CHECK_POINTER_PARAMETER(pActionInfo);

    //printf("my add: --callback_AlarmAction--pAlertInfo->AlertType=%d, pActionInfo->tActionType = %d \n", pAlertInfo->AlertType, pActionInfo->tActionType);
    switch (pActionInfo->tActionType)
	{
			//1 通知界面
		case ACTION_GUI:
			{
				if (ALERT_TYPE_LONG_AWAY == pAlertInfo->AlertType)
				{
					/*Do NoTing*/
				}
				else
				{
					MPU_CALL(callback_AlarmAction_GUI(pAlertInfo, pActionInfo));			
				}
			}
			break;

			//1 联动录像
		case ACTION_RECORD:
			{
                //pActionInfo->nChannel = 0;     // 通道会出现为1的情况    TODO
				if (ALERT_TYPE_ALARMIN == pAlertInfo->AlertType)
				{
					MPU_CALL(AVL_Record_SetAlarmInAlarmRec(pActionInfo->nChannel));
				}
				else if (ALERT_TYPE_MOTION == pAlertInfo->AlertType)
				{
					MPU_CALL(AVL_Record_SetMotionAlarmRec(pActionInfo->nChannel));
				}
				else if (ALERT_TYPE_HUMANSHAPE == pAlertInfo->AlertType)
				{
					MPU_CALL(AVL_Record_SetAiAlarmRec(pActionInfo->nChannel));
				}
				else
				{
					MPU_ERR("unsupported AlertType = %d\n", pAlertInfo->AlertType);
					MPU_CHK(FALSE);
					nRet = IDVR_ERROR_UNSUPPORT;
				}
			}
			break;

			//1 联动报警输出
            case ACTION_ALARMOUT:
                break;

            case ACTION_SCREEN:
            {
                int i = 0;

                for (i = 0; i < MAX_SPOT_NUM + 2; i++)
                {
					if (pActionInfo->dwReserved & (1 << i))
					{
						MPU_CALL(MPU_PRW_SetAlarmCh(i, pActionInfo->nChannel));
					}
				}
            }
            break;

                //1 联动声音报警
            case ACTION_BEEP:
            {
				//MPU_SYS_KeepBuzzer(BUZZER500);
				
				if (pAlertInfo->dwReserved)
				{
					if (ALERT_TYPE_LONG_AWAY == pAlertInfo->AlertType)
					{
						/*Do NoTing*/
					}
					else if (ALERT_TYPE_MOTION == pAlertInfo->AlertType)
					{
						if (1 == MPU_AVL_AudioAlarm_CheckAudioAlarmInSchedule(pActionInfo->nChannel))
						{
							ZWSys_SetSpeechContent(SPEECH_ALARM_MOTION);
							ZWSys_SetAlarmLightStart();
						}
					}
					else if (ALERT_TYPE_HUMANSHAPE == pAlertInfo->AlertType)
					{
						if (1 == MPU_AVL_AudioAlarm_CheckAudioAlarmInSchedule(pActionInfo->nChannel))
						{
							ZWSys_SetSpeechContent(SPEECH_ALARM_HUMAN);
							ZWSys_SetAlarmLightStart();
						}
					}
					else
					{
						if (1 == MPU_AVL_AudioAlarm_CheckAudioAlarmInSchedule(pActionInfo->nChannel))
						{
							ZWSys_SetSpeechContent(SPEECH_ALARM_MOTION);
							ZWSys_SetAlarmLightStart();
						}
					}
				}
			}
			break;

			//1 联动上传报警中心
		case ACTION_SENDTOSERVER:
			{
				//CIU_RefreshAlarmEvent(pAlertInfo);
			}
			break;

			//1 联动Email通知
		case ACTION_EMAIL:
			{
				//callback_AlarmAction_Email(pAlertInfo, pActionInfo);
			}
			break;

                //1 联动抓图
            case ACTION_SNAPSHOT:
            {
                if (ALERT_TYPE_LONG_AWAY == pAlertInfo->AlertType)
                {
                    /*Do NoTing*/
                }
                else
                {
                    if (pAlertInfo->dwReserved)
                    {
                        MPU_AVL_Pic_Capture(pActionInfo->nChannel, TRUE, pAlertInfo->AlertType, pAlertInfo->nChannel);
                    }
                }
            }
            break;

                //联动推送
            case ACTION_PUSH:
            {
                if (pAlertInfo->dwReserved)
                {
                    MPU_CALL(callback_AlarmAction_Push(pAlertInfo->nChannel, pAlertInfo, pActionInfo));
                }
            }
            break;
            case ACTION_PTZ: //1 联动云台控制
            case ACTION_SMS: //1 联动短信通知
            default:
            {
                MPU_ERR("unsupported ActionType = %d\n", pActionInfo->tActionType);
                MPU_CHK(FALSE);
				nRet = IDVR_ERROR_UNSUPPORT;
            }
            break;
            }

#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
	//if (!linkAction)
	{
		MPU_AVL_SetTrackLinkStart(0, 1);
	}
#endif

	return nRet;
}

//!\brief   MPU模块清除报警
//!\return  成功为0，其它为错误码
int MPU_ClearAlarm()
{
	int i, nCount;

	//清除声音
	MPU_SYS_KeepBuzzer(NOBUZZER);

    //清除屏幕联动
    for (i = 0; i < MAX_SPOT_NUM + 2; i++)
    {
        MPU_PRW_ClearAlarm(i);
    }

    //清除PTZ联动
	if (NULL != g_pMpuInfo)
	{
		nCount = MAX_ENC_CHANNEL_NUM;

		for (i = 0; i < nCount; ++i)
        {
            g_pMpuInfo->m_dwPtzCode_AlarmAction[i] = 0;
            g_pMpuInfo->m_dwCruiseIndex_AlarmAction[i] = 0;
        }
    }

    return IDVR_OK;
}

//!\brief       设置异常处理是否显示桌面图标
//!\param   nExceptionType
//!\return  返回是否处理
int MPU_Exception_IsEnableDesktopIcon(int nExceptionType)
{
	//缺省返回1
	int nValue = 1;

	//从配置文件初始化
	CConfig *pConfig = CConfig::GetInstance();

	if (NULL != pConfig)
	{
		switch (nExceptionType)
		{
			case ALERT_TYPE_DISKERROR:
				nValue = pConfig->GetValue("Exception", "DISKERROR", 1);
				break;

			case ALERT_TYPE_IPCONFLICT:
				nValue = pConfig->GetValue("Exception", "IPCONFLICT", 1);
				break;

			case ALERT_TYPE_ILLEGEACCESS:
				nValue = pConfig->GetValue("Exception", "ILLEGEACCESS", 1);
				break;

			case ALERT_TYPE_RETICLEDISCONNECT:
				nValue = pConfig->GetValue("Exception", "RETICLEDISCONNECT", 1);
				break;

			case ALERT_TYPE_DISKFULL:
				nValue = pConfig->GetValue("Exception", "DISKFULL", 1);
				break;

			case ALERT_TYPE_DISKGROUP_ERROR:
				nValue = pConfig->GetValue("Exception", "DISKGROUPERROR", 1);
				break;

			default:
				MPU_ERR("Exception type is overflow, %d\n", nExceptionType);
				break;
		}
	}
	else
	{
		//写取配置文件失败
		MPU_ERR("no CConfig instance, [Exception]\n");
		MPU_CHK(NULL != pConfig);
	}

	return nValue;
}

//!\brief       设置异常处理是否显示桌面图标
//!\param   nExceptionType  异常类型
//!\param   bEnable         是否处理
//!\return  0表示成功，其它为错误码
int MPU_Exception_EnableDesktopIcon(int nExceptionType, int bEnable)
{
	int nRet = IDVR_OK;

	CConfig *pConfig;

	//从配置文件初始化
	pConfig = CConfig::GetInstance();

	if (NULL != pConfig)
	{
		switch (nExceptionType)
		{
			case ALERT_TYPE_DISKERROR:
				pConfig->SetValue("Exception", "DISKERROR", bEnable);
				break;

			case ALERT_TYPE_IPCONFLICT:
				pConfig->SetValue("Exception", "IPCONFLICT", bEnable);
				break;

			case ALERT_TYPE_ILLEGEACCESS:
				pConfig->SetValue("Exception", "ILLEGEACCESS", bEnable);
				break;

			case ALERT_TYPE_RETICLEDISCONNECT:
				pConfig->SetValue("Exception", "RETICLEDISCONNECT", bEnable);
				break;

			case ALERT_TYPE_DISKFULL:
				pConfig->SetValue("Exception", "DISKFULL", bEnable);
				break;

			case ALERT_TYPE_DISKGROUP_ERROR:
				pConfig->SetValue("Exception", "DISKGROUPERROR", bEnable);
				break;

			default:
				MPU_ERR("Exception type is overflow, %d\n", bEnable);
				nRet = IDVR_ERROR_CONFIGFILE;
				break;
		}
	}
	else
	{
		//写取配置文件失败
		MPU_ERR("no CConfig instance, [Exception]\n");
		MPU_CHK(NULL != pConfig);

		nRet = IDVR_ERROR_CONFIGFILE;
	}

	return nRet;
}

/**************************************************************************
 *                            注销局部宏定义                              *
 **************************************************************************/
#undef MPU_CALL					//调用初始化动作
