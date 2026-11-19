#include <stdlib.h>
#include <sys/types.h>
#include "CSoundDetect.h"
#include "CActionAlarm.h"
#include "SYS/CAwakeSchedule.h"

static const char *SoundDetectSection = "SoundDetectCfg";
int CSoundDetectManager::m_AlarmType = 0;
ALARMCALLBACK CSoundDetectManager::m_FxnCallBack = NULL;

CSoundDetectManager::CSoundDetectManager(int nCh, void *hHandle)
{
    m_FxnCallBack = NULL;
    m_AlarmType = 0;
    m_iChannel = nCh;
    m_henc = hHandle;
    m_SoundDetectEnable[m_iChannel] = 0;
	m_bisSound = 0;
	m_lastSound = 0;
}

CSoundDetectManager::~CSoundDetectManager()
{
    UnInit();
}

int CB_SoundDetectProc(LpAudioDetectCbData pData, void *UserData)
{
    CSoundDetectManager * pSoundDetectManager = (CSoundDetectManager *)UserData;

    pSoundDetectManager->PushEvent(pData->bTrigger);
    ZWSys_EncSetSlowFrameMode(2);

    return 0;
}

int CSoundDetectManager::Init(int nMaxCh, int nMaxAlarmOut)
{
    int iRet = SUCCESS;
    AudioDetectCfg SetCfg = {0};
    char key[24];

    pCConfig = CConfig::GetInstance();
	if (NULL == pCConfig)
	{
		UnInit();
		return -1;
	}
	ZWSys_RegisterAudioDetectCallback(CB_SoundDetectProc, this);

    memset(key, 0, sizeof(key));
    snprintf(key, sizeof(key), "Enable%02d", m_iChannel);
	m_SoundDetectEnable[m_iChannel] = pCConfig->GetValue(SoundDetectSection, key, 0);
	snprintf(key, sizeof(key), "Precision%02d", m_iChannel);
	m_SoundDetectSentive[m_iChannel] = pCConfig->GetValue(SoundDetectSection, key, 2);

    TRACE_AVL("[SOUNDDETECT]LoadParam enable %d sentive %d\n", m_SoundDetectEnable[m_iChannel], m_SoundDetectSentive[m_iChannel]);

    SetCfg.nEnable = m_SoundDetectEnable[m_iChannel];
    SetCfg.nSenstive = m_SoundDetectSentive[m_iChannel];
    iRet = ZWSys_SetAudioDetectCfg(m_henc, &SetCfg);
    if (SUCCESS != iRet)
    {
        AVL_ERR("[SOUNDDETECT]set audio cfg failed!!!ret[%d]", iRet);
    }

    m_pActionAlarm = new CActionAlarm;
	if (m_pActionAlarm == NULL)
	{
        UnInit();
		return -1;
	}

	m_pActionAlarm->Init(m_iChannel, (const char *)SoundDetectSection, "SoundDetectAction", nMaxCh, nMaxAlarmOut);
	m_pActionAlarm->EnableAction(ACTION_GUI, 1);
    m_pActionAlarm->EnableAction(ACTION_PUSH, 1);
    m_pActionAlarm->EnableAction(ACTION_ALARMOUT, 1);
	return SUCCESS;
}

void CSoundDetectManager::UnInit()
{
    return;
}

int CSoundDetectManager::SetSoundDetectCfg(int nCh, SOUND_DETECT_PARAM_S stParam)
{
    int iRet = SUCCESS;
    char key[24];
    AudioDetectCfg SetCfg ={0};

    if (m_SoundDetectEnable[nCh] != stParam.iSwitch)
    {
        m_SoundDetectEnable[nCh] = stParam.iSwitch;

        snprintf(key, sizeof(key), "Enable%02d", m_iChannel);
        pCConfig->SetValue(SoundDetectSection, key, stParam.iSwitch);
    }

    if (m_SoundDetectSentive[nCh] != stParam.iSentive)
    {
        m_SoundDetectSentive[nCh] = stParam.iSentive;

        snprintf(key, sizeof(key), "Precision%02d", m_iChannel);
        pCConfig->SetValue(SoundDetectSection, key, stParam.iSentive);
    }

    SetCfg.nEnable = m_SoundDetectEnable[nCh];
    SetCfg.nSenstive = m_SoundDetectSentive[nCh];
    iRet = ZWSys_SetAudioDetectCfg(m_henc, &SetCfg);
    if (SUCCESS != iRet)
    {
        AVL_ERR("[SOUNDDETECT]set audio cfg failed!!!ret[%d]", iRet);
        return iRet;
    }

    return SUCCESS;
}

int CSoundDetectManager::GetSoundDetectCfg(int nCh, SOUND_DETECT_PARAM_S *pstParam)
{
    pstParam->iSwitch =  m_SoundDetectEnable[nCh];
    pstParam->iSentive = m_SoundDetectSentive[nCh];
    
    return SUCCESS;
}

int CSoundDetectManager::SetAlarmCallBack(int AlarmType, int (*CallBack) (struct tagAlertInfo * pAlertInfo, struct tagActionInfo * pActionInfo))
{
	if (CallBack == NULL)
	{
		return FAILED;
	}
	m_AlarmType = AlarmType;
	m_FxnCallBack = CallBack;//callback_AlarmAction
	return (int) SUCCESS;
}

int CSoundDetectManager::PushEvent( int bSoundStatus)
{	
	//m_pActionAlarm->CheckAction(m_AlarmType, m_FxnCallBack,0);
    m_lastSound |= bSoundStatus;
    
	return SUCCESS;
}

int CSoundDetectManager::CheckSound()
{
	static int soundStatus=0;
	static int noSoundCount=0;
    
	int result = 0;

	m_bisSound = m_lastSound;
	m_lastSound = 0;
    
	if (m_SoundDetectEnable[0] && m_bisSound)	//检测到声音	
	{
		printf("------检测到声音------\n");

		soundStatus = 1;
		noSoundCount = 0;

		m_pActionAlarm->UpdateActionIntvCnt(m_AlarmType, 1);
		result = m_pActionAlarm->CheckAction(0,m_AlarmType, m_FxnCallBack, 0);
		
		return result;
	}
	else
	{
		m_pActionAlarm->UpdateActionIntvCnt(m_AlarmType, 0);
		if (1 == soundStatus)
		{
			noSoundCount++;
		}

        if ((1 == soundStatus) && (noSoundCount >= 3))//停止后发送通知
		{		
			soundStatus = 0;
			result = m_pActionAlarm->CheckAction(0,m_AlarmType, m_FxnCallBack, 1);
		}
	}
	
	return 0;
}

int CSoundDetectManager::IsEnableAction(int Action)
{
	return m_pActionAlarm->IsEnableAction(Action);
}

int CSoundDetectManager::SetPushEventIntvCnt(int nPushEventIntv)
{
	return m_pActionAlarm->SetPushEventIntvCnt(m_AlarmType, nPushEventIntv);
}

int CSoundDetectManager::GetPushEventIntvCnt(int *nPushEventIntv)
{
	return m_pActionAlarm->GetPushEventIntvCnt(m_AlarmType, nPushEventIntv);
}

