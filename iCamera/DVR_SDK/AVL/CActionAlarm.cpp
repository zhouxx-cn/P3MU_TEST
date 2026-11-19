// Copyright (C) 1991 - 1999 Rational Software Corporation
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include "CActionAlarm.h"
#include "zwsdk.h"

//移动侦测触发回调函数
MotionCallBack m_MotionCallBack;

CActionAlarm::CActionAlarm()
{
	m_iChannel = 0;
	m_nPushEventIntv[0] = MOTION_ACTION_LINKAGE_INTERVAL_DEF;
	m_nPushEventIntv[1] = HUMAN_SHAPE_ACTION_LINKAGE_INTERVAL_DEF;
    m_nPushEventIntv[2] = ALARMIN_ACTION_LINKAGE_INTERVAL_DEF;
	for (unsigned int i = 0; i < sizeof(m_nPushEventIntv) / sizeof(m_nPushEventIntv[0]); ++i)
	{
		m_nPushEventIntvCnt[i] = 0;
		b_nPushEventEnable[i] = true;
	}
	memset(&m_tAction, 0, sizeof (ACTION_T));
}

int CActionAlarm::Init(int ch, const char *section, const char *pkeystr, int iMaxCh, int iMaxAlarmOut)
{
	m_iChannel = ch;
	pCConfig = CConfig::GetInstance();
	if (pCConfig == NULL)
	{
		ALARM_ERR("[CActionAlarm] pCConfig=NULL\n");
		return -1;
	}
	if (section == NULL || pkeystr == NULL)
	{
		ALARM_ERR("[CActionAlarm] section==NULL||pkeystr==NULL\n");
		return -1;
	}
	strncpy(pSection, section, sizeof(pSection) - 1);
	strncpy(pKeyPrefix, pkeystr, sizeof(pKeyPrefix) - 1);
	if (iMaxCh < 0)
	{
		iMaxCh = 0;
	}
	if (iMaxCh > 32)
	{
		iMaxCh = 32;
	}
	if (iMaxAlarmOut < 0)
	{
		iMaxAlarmOut = 0;
	}
	if (iMaxAlarmOut > 32)
	{
		iMaxAlarmOut = 32;
	}
	m_MaxCh = iMaxCh;
	m_MaxAlarmOut = iMaxAlarmOut;
	LoadParam();
	return 0;
}

//##ModelId=49BDEFDF0009
int CActionAlarm::EnableAction(int Action, int bEnable)
{
	unsigned int Action_l, Action_h;

	Action_l = Action & 0xffff;
	Action_h = Action >> 16;
	//!主/子动作类型最大支持32
	if (Action_h >= MAX_ACTION_NUM || Action_l >= MAX_ACTION_NUM_SUB)
	{
		ALARM_ERR("[CActionAlarm] Action=0x%08x\n", Action);
		return -1;
	}
	if (Action_l)
	{
		ALARM_ERR("[CActionAlarm] Action=0x%08x\n", Action);
		return -1;
	}
	if (bEnable)
	{
		m_tAction.bEnable |= (1 << Action_h);
		if (Action == ACTION_SCREEN)
		{
			m_tAction.bScreenCH = bEnable;
		}
	}
	else
	{
		m_tAction.bEnable &= ~(1 << Action_h);
		//对推送重新计数
		for (unsigned int i = 0; i < sizeof(m_nPushEventIntv) / sizeof(m_nPushEventIntv[0]); ++i)
		{
			m_nPushEventIntvCnt[i] = 0;
			b_nPushEventEnable[i] = true;
		}
	}

	SaveParam();

	return (int) 0;
}

//##ModelId=49BDEFDF0081
int CActionAlarm::IsEnableAction(int Action)
{
	unsigned int Action_l, Action_h;

	int ret = 0;

	Action_l = Action & 0xffff;
	Action_h = Action >> 16;
	//!主/子动作类型最大支持32
	if (Action_h >= MAX_ACTION_NUM || Action_l >= MAX_ACTION_NUM_SUB)
	{
		ALARM_ERR("[CActionAlarm]Action=0x%08x\n", Action);
		return 0;
	}

	if (m_tAction.bEnable & (1 << Action_h))
	{
		ret = 1;
		if (Action == ACTION_SCREEN)
		{
			ret = m_tAction.bScreenCH;
		}
	}

	return (int) ret;
}

//!\brief 获取某个报警联动录像参数
//!\param dwChannelFlag 联动的通道标记 
//每一位表示联动一个通道 1表示联动通道1 
//2表示联动通道2录像 3表示联动通道1和2录像,以此类推
//!\param nPort 某个报警通道号
//!\return 0表示获取成功，-1表示获取失败
//##ModelId=49B7187602BF
int CActionAlarm::GetRecordParam(int *dwChannelFlag)
{
	if (NULL == dwChannelFlag)
	{
		return -1;
	}
	*dwChannelFlag = m_tAction.bRecordCH;
	return 0;
}

//!\brief 设置某个报警联动录像参数
//!\param dwChannelFlag 联动录像通道标记
//!\param nPort 某个报警通道号
//!\return 0表示设置成功 -1表示设置失败
//##ModelId=49B718640232
int CActionAlarm::SetRecordParam(int dwChannelFlag)
{
	if (dwChannelFlag >> m_MaxCh)
	{
		ALARM_WARN("[CActionAlarm]Error param<dwChannelFlag = 0x%08x>\n", dwChannelFlag);
		return -1;
	}
	m_tAction.bRecordCH = dwChannelFlag;
	SaveParam();
	return 0;
}

//!\brief 
//获取某个报警联动拍照参数,拍照张数和间隔由CCapturePic::GetCfg获取
//!\param dwChannelFlag 存储联动拍照通道标记的缓冲区
//!\param nPort 报警联动输入通道号
//!\return 0表示成功 -1表示失败
//##ModelId=49B5CE9A030D
int CActionAlarm::GetSnapshotParam(int *dwChannelFlag)
{
	if (NULL == dwChannelFlag)
	{
		return -1;
	}
	*dwChannelFlag = m_tAction.bSnapshotCH;
	return 0;
}

//!\brief 
//设置某个报警联动拍照参数，拍照张数和拍照间隔由CCapturePi
//c::SetCfg设定
//!\param dwChannelFlag 联动拍照通道标记
//!\param nPort 某个报警通道号
//!\return 0表示设置成功 -1表示失败
//##ModelId=49B5CDFE00BB
int CActionAlarm::SetSnapshotParam(int dwChannelFlag)
{
	if (dwChannelFlag >> m_MaxCh)
	{
		ALARM_WARN("[CActionAlarm]Error param<dwChannelFlag = 0x%08x>\n", dwChannelFlag);
		return -1;
	}
	m_tAction.bSnapshotCH = dwChannelFlag;
	SaveParam();
	return 0;
}

//!\brief 获取某一某个报警通道联动某一云台的动作参数
//!\param nAction 联动动作 
//*nAction=0表示无动作，1联动预置点，2联动巡航，3联动轨迹
//!\param nIndex 联动预置点/巡航/轨迹的编号
//!\param nChannel 云台对应的通道号
//!\param nPort 某个报警通道号
//!\return 0表示获取成功, -1表示获取失败
//##ModelId=49B718B202DE

int CActionAlarm::GetPtzParam(int nChannel, int *nAction, int *nIndex)
{
	if (NULL == nAction || nIndex == NULL)
	{
		return -1;
	}
	if (nChannel < 0 || nChannel >= m_MaxCh)
	{
		return -1;
	}
	if (m_tAction.bPtzCH & (1 << nChannel))
	{
		*nAction = (m_tAction.iPtzAction[nChannel] >> 16) & 0xffff;
		*nIndex = (m_tAction.iPtzAction[nChannel] & 0xffff);
	}
	else
	{
		*nAction = 0;
		*nIndex = 0;
	}
	return 0;
}


//!\brief 设置某个报警通道联动某一个云台的动作
//!\param nAction 联动的动作 0表示无动作 1联动预置点 
//2联动巡航 3联动轨迹
//!\param nIndex 联动的预置点/巡航/轨迹的编号
//!\param nChannel 云台对应的通道号
//!\param nPort 某个报警通道号
//!\return 0表示成功 -1表示失败
//##ModelId=49B718BD0261
int CActionAlarm::SetPtzParam(int nChannel, int nAction, int nIndex)
{
	if (nChannel < 0 || nChannel >= m_MaxCh)
	{
		return -1;
	}
	if (nAction)
	{
		m_tAction.bPtzCH |= (1 << nChannel);
	}
	else
	{
		m_tAction.bPtzCH &= ~(1 << nChannel);
	}
	m_tAction.iPtzAction[nChannel] = (nAction << 16) | (nIndex & 0xffff);
	SaveParam();
	return 0;
}

//!\brief 获取某个报警联动报警输出的参数
//!\param dwOutPortFlag 存储输出通道标记的缓冲区
//!\param nPort 某个报警通道号
//!\return 0表示成功 -1表示失败
//##ModelId=49B718DD030D
int CActionAlarm::GetDopParam(int *dwOutPortFlag)
{
	if (NULL == dwOutPortFlag)
	{
		return -1;
	}
	*dwOutPortFlag = m_tAction.bDopCH;
	return 0;
}

//!\brief 设置某个报警联动报警输出的参数
//!\param dwOutPortFlag 设置输出通道标记
//!\param nPort 某个报警通道号
//!\return 0表示成功 -1表示失败
//##ModelId=49B718ED00AB
int CActionAlarm::SetDopParam(int dwOutPortFlag)
{
	if (dwOutPortFlag >> m_MaxAlarmOut)
	{
		ALARM_WARN("[CActionAlarm]Error param<dwOutPortFlag = 0x%08x>\n", dwOutPortFlag);
		return -1;
	}
	m_tAction.bDopCH = dwOutPortFlag;
	SaveParam();
	return 0;
}

//!\brief 
//获取屏显配置
//!\param dwChannelFlag 
//!\param nPort 报警联动输入通道号
//!\return 0表示成功 -1表示失败
//##ModelId=49B5CE9A030D
int CActionAlarm::GetScreenParam(int *dwChannelFlag)
{
	if (NULL == dwChannelFlag)
	{
		return -1;
	}
	*dwChannelFlag = m_tAction.bScreenCH;
	return 0;
}

//!\brief 
//设置屏显配置
//!\param dwChannelFlag  
//!\param nPort 某个报警通道号
//!\return 0表示设置成功 -1表示失败
//##ModelId=49B5CDFE00BB
int CActionAlarm::SetScreenParam(int dwChannelFlag)
{

	m_tAction.bScreenCH = dwChannelFlag;
	SaveParam();
	return 0;
}

int CActionAlarm::GetSepValue(char *pbuff, int *start_pos, char sep)
{
	char value[256];

	char cVal;

	int retVal = 0;

	int i_pos, i_pos1;

	i_pos = *start_pos;
	i_pos1 = 0;
	while (1)
	{

		cVal = pbuff[i_pos++];

		if (cVal == 0)
		{
			if (i_pos1 == 0)	//空的
			{
				retVal = 0;

				break;
			}
			value[i_pos1] = 0;
			sscanf(value, "%x", &retVal);
			*start_pos = i_pos - 1;
			break;
		}
		if (cVal == sep)
		{
			if (i_pos1 == 0)	//空的
			{
				retVal = 0;
				*start_pos = i_pos;
				break;
			}
			value[i_pos1] = 0;
			sscanf(value, "%x", &retVal);
			*start_pos = i_pos;
			break;
		}
		value[i_pos1++] = cVal;
	}
	return retVal;
}

int CActionAlarm::LoadParam()
{
	int ch;
	char key[100] = {0};
	char value[256] = {0};
	int pos;
	unsigned int mask_ch = 0xFFFFFFFF;
	unsigned int mask_port = 0xFFFFFFFF;

	mask_ch = (mask_ch << (32 - m_MaxCh) >> (32 - m_MaxCh));
	mask_port = (mask_port << (32 - m_MaxAlarmOut) >> (32 - m_MaxAlarmOut));

	//读配置
	memset(&m_tAction, 0, sizeof (ACTION_T));
	snprintf(key, sizeof(key), "%s%02d", pKeyPrefix, m_iChannel);
	pCConfig->GetValue(pSection, key, "", value, 255);
	if (value[0] == 0)			//字符串为空
	{
		return 0;
	}
	//扫描字符串
	pos = 0;

	m_tAction.bEnable = GetSepValue(value, &pos, ':');
	m_tAction.bRecordCH = GetSepValue(value, &pos, ':');
	m_tAction.bSnapshotCH = GetSepValue(value, &pos, ':');
	m_tAction.bDopCH = GetSepValue(value, &pos, ':');
	m_tAction.bScreenCH = GetSepValue(value, &pos, ':');
	m_tAction.bPtzCH = GetSepValue(value, &pos, ':');

	for (ch = 0; ch < MAX_ACTION_CH_NUM; ch++)
	{
		m_tAction.iPtzAction[ch] = GetSepValue(value, &pos, ':');
	}
	m_tAction.bRecordCH &= mask_ch;
	m_tAction.bSnapshotCH &= mask_ch;
	m_tAction.bPtzCH &= mask_ch;
	m_tAction.bDopCH &= mask_port;

	m_nPushEventIntv[0] = pCConfig->GetValue(PUSH_EVENT_INTV, "MotionAction", 120);
	m_nPushEventIntv[1] = pCConfig->GetValue(PUSH_EVENT_INTV, "HumanAction", 120);
	m_nPushEventIntv[2] = pCConfig->GetValue(PUSH_EVENT_INTV, "AlarmInAction", 120);

	return 0;
}

//!brief 保存配置
int CActionAlarm::SaveParam()
{
	char key[100] = {0};
	char value[256] = {0};
	int i = 0;
	int pos = 0;
	int nRet = 0;

	//写配置
	snprintf(key, sizeof(key), "%s%02d", pKeyPrefix, m_iChannel);
	pos = 0;

	nRet = pCConfig->SetValue(pSection, key, value);
	if (nRet == 0 && pCConfig->m_bReadOnlyCfgFlag == 1)
	{
		pCConfig->GetValue(pSection, key, "", value, 255);
		if (value[0] == 0)			//字符串为空
		{
			return 0;
		}
		//扫描字符串
		int GetValuePos = 0;
		int ch;
		unsigned int mask_ch = 0xFFFFFFFF;
		unsigned int mask_port = 0xFFFFFFFF;

		mask_ch = (mask_ch << (32 - m_MaxCh) >> (32 - m_MaxCh));
		mask_port = (mask_port << (32 - m_MaxAlarmOut) >> (32 - m_MaxAlarmOut));

		m_tAction.bEnable = GetSepValue(value, &GetValuePos, ':');
		m_tAction.bRecordCH = GetSepValue(value, &GetValuePos, ':');
		m_tAction.bSnapshotCH = GetSepValue(value, &GetValuePos, ':');
		m_tAction.bDopCH = GetSepValue(value, &GetValuePos, ':');
		m_tAction.bScreenCH = GetSepValue(value, &GetValuePos, ':');
		m_tAction.bPtzCH = GetSepValue(value, &GetValuePos, ':');

		for (ch = 0; ch < MAX_ACTION_CH_NUM; ch++)
		{
			m_tAction.iPtzAction[ch] = GetSepValue(value, &GetValuePos, ':');
		}
		m_tAction.bRecordCH &= mask_ch;
		m_tAction.bSnapshotCH &= mask_ch;
		m_tAction.bPtzCH &= mask_ch;
		m_tAction.bDopCH &= mask_port;

		return 0;
	}

	if (m_tAction.bEnable)
	{
		pos += snprintf(value + pos, sizeof(value) - pos, "%x", m_tAction.bEnable);
	}
	pos += snprintf(value + pos, sizeof(value) - pos, ":");
	if (m_tAction.bRecordCH)
	{
		pos += snprintf(value + pos, sizeof(value) - pos, "%x", m_tAction.bRecordCH);
	}
	pos += snprintf(value + pos, sizeof(value) - pos, ":");
	if (m_tAction.bSnapshotCH)
	{
		pos += snprintf(value + pos, sizeof(value) - pos, "%x", m_tAction.bSnapshotCH);
	}
	pos += snprintf(value + pos, sizeof(value) - pos, ":");
	if (m_tAction.bDopCH)
	{
		pos += snprintf(value + pos, sizeof(value) - pos, "%x", m_tAction.bDopCH);
	}
	pos += snprintf(value + pos, sizeof(value) - pos, ":");
	if (m_tAction.bScreenCH)
	{
		pos += snprintf(value + pos, sizeof(value) - pos, "%x", m_tAction.bScreenCH);
	}
	pos += snprintf(value + pos, sizeof(value) - pos, ":");
	if (m_tAction.bPtzCH)
	{
		pos += snprintf(value + pos, sizeof(value) - pos, "%x", m_tAction.bPtzCH);
	}
	pos += snprintf(value + pos, sizeof(value) - pos, ":");

	for (i = 0; i < MAX_ACTION_CH_NUM; i++)
	{
		if (m_tAction.iPtzAction[i])
		{
			pos += snprintf(value + pos, sizeof(value) - pos, "%x", m_tAction.iPtzAction[i]);
		}
		pos += snprintf(value + pos, sizeof(value) - pos, ":");
	}
	pCConfig->SetValue(pSection, key, value);

	return 0;

}

int CActionAlarm::SetPushEventIntvCnt(int nAlarmType, int nPushEventIntv)
{
	if(nPushEventIntv < 0)
	{
		return -1;
	}

	switch (nAlarmType)
	{
		case ALERT_TYPE_MOTION:
		{
			m_nPushEventIntv[0] = nPushEventIntv;
			pCConfig->SetValue(PUSH_EVENT_INTV, "MotionAction", m_nPushEventIntv[0]);
			break; 
		}
		case ALERT_TYPE_HUMANSHAPE:
        case ALERT_TYPE_SOUNDDETECT:
		{
			m_nPushEventIntv[1] = nPushEventIntv;
			pCConfig->SetValue(PUSH_EVENT_INTV, "HumanAction", m_nPushEventIntv[1]);
			break;
		}
		case ALERT_TYPE_TRIPWIRE:
		{
			m_nPushEventIntv[1] = nPushEventIntv;
			pCConfig->SetValue(PUSH_EVENT_INTV, "HumanAction", m_nPushEventIntv[1]);
			break; 
		}
		case ALERT_TYPE_ALARMIN:
        {
        	m_nPushEventIntv[2] = nPushEventIntv;
			pCConfig->SetValue(PUSH_EVENT_INTV, "AlarmInAction", m_nPushEventIntv[2]);
			break; 
		}
		default:
			 break; 
	}
	return 0;
}

int CActionAlarm::GetPushEventIntvCnt(int nAlarmType, int *nPushEventIntv)
{
	if(NULL == nPushEventIntv)
	{
		return -1;
	}

	switch (nAlarmType)
	{
		case ALERT_TYPE_MOTION:
		{
			*nPushEventIntv = m_nPushEventIntv[0];
			break; 
		}
		case ALERT_TYPE_HUMANSHAPE:
        case ALERT_TYPE_SOUNDDETECT:
		{
			*nPushEventIntv = m_nPushEventIntv[1];
			break;
		}
		case ALERT_TYPE_TRIPWIRE:
		{
			*nPushEventIntv = m_nPushEventIntv[1];
			break; 
		}
		case ALERT_TYPE_ALARMIN:
        {
        	*nPushEventIntv = m_nPushEventIntv[2];
			break; 
		}
		default:
			 break; 
	}
	return 0;
}

int CActionAlarm::UpdateActionIntvCnt(int nAlarmType, int nAlarmStatus)
{
	switch (nAlarmType)
	{
		//!移动侦测
		case ALERT_TYPE_MOTION:
		{
			//根据间隔检查联动是否应该使能
			if (m_nPushEventIntvCnt[0] <= 0)
			{
				b_nPushEventEnable[0] = true;
			}
			else
			{
				b_nPushEventEnable[0] = false;
			}
			
			if (b_nPushEventEnable[0])
			{
				if (1 == nAlarmStatus)	//报警开始
				{
					//对推送重新计数
					m_nPushEventIntvCnt[0] = m_nPushEventIntv[0];
				}
			}
			else
			{
				//更新推送计数
				m_nPushEventIntvCnt[0]--;
			}
			break;
		}

		case ALERT_TYPE_HUMANSHAPE:
        case ALERT_TYPE_SOUNDDETECT:
		{
			//根据间隔检查联动是否应该使能
			if (m_nPushEventIntvCnt[1] <= 0)
			{
				b_nPushEventEnable[1] = true;
			}
			else
			{
				b_nPushEventEnable[1] = false;
			}
			if (b_nPushEventEnable[1])
			{
				if (1 == nAlarmStatus)	//报警开始
				{
					//对推送重新计数
					m_nPushEventIntvCnt[1] = m_nPushEventIntv[1];
				}
			}
			else
			{
				//更新推送计数
				m_nPushEventIntvCnt[1]--;
			}

			break;
		}
		case ALERT_TYPE_TRIPWIRE:
			//根据间隔检查联动是否应该使能
			if (m_nPushEventIntvCnt[1] <= 0)
			{
				b_nPushEventEnable[1] = true;
			}
			else
			{
				b_nPushEventEnable[1] = false;
			}
			if (b_nPushEventEnable[1])
			{
				if (1 == nAlarmStatus)	//报警开始
				{
					//对推送重新计数
					m_nPushEventIntvCnt[1] = m_nPushEventIntv[1];
				}
			}
			else
			{
				//更新推送计数
				m_nPushEventIntvCnt[1]--;
			}
			break;
        //报警输入
        case ALERT_TYPE_ALARMIN:
        {
            //根据间隔检查联动是否应该使能
            if (m_nPushEventIntvCnt[2] <= 0)
            {
                b_nPushEventEnable[2] = true;
            }
            else
            {
                b_nPushEventEnable[2] = false;
            }

            // 当前推送后，30S后才能继续推送下一次
            if (b_nPushEventEnable[2])
            {
                if (1 == nAlarmStatus)	//报警开始
                {
                    //对推送重新计数
                    m_nPushEventIntvCnt[2] = m_nPushEventIntv[2];
                }
            }
            else
            {
                //更新推送计数
                m_nPushEventIntvCnt[2]--;
            }

            break;
        }
		default: break;
	}

	return 0;
}

int CActionAlarm::CheckAction(int Chn,int m_AlarmType, ALARMCALLBACK m_FxnCallBack, int alarmStatus)
{
	int ch = 0;
	int type = 0;
	struct tagAlertInfo m_alertInfo = {0};
	struct tagActionInfo m_actionInfo = {0};

	if (NULL == m_FxnCallBack)
	{
		return 0;
	}
	//触发源
	m_alertInfo.AlertType = m_AlarmType;
	m_alertInfo.nChannel = Chn;
	if (0 == alarmStatus)
	{
		m_alertInfo.dwReserved = 1;
	}
	else
	{
		m_alertInfo.dwReserved = 0;
	}
	//GUI
	type = ACTION_GUI;
	if (m_tAction.bEnable & (1 << (ACTION_GUI >> 16)))
	{
		m_actionInfo.nChannel = 0;
		m_actionInfo.tActionType = type;
		m_actionInfo.dwReserved = 0;
		m_FxnCallBack(&m_alertInfo, &m_actionInfo);
	}
	//录像
	type = ACTION_RECORD;
	if (m_tAction.bEnable & (1 << (ACTION_RECORD >> 16)))
	{
		#if 0
		for (ch = 0; ch < m_MaxCh; ch++)
		{
			if (m_tAction.bRecordCH & (1 << ch))
			{
				m_actionInfo.nChannel = ch;
				m_actionInfo.tActionType = type;
				m_actionInfo.dwReserved = 0;
				m_FxnCallBack(&m_alertInfo, &m_actionInfo);
			}
		}
		#else
		/* 录像不使用通道参数 */
		m_actionInfo.nChannel = 0;
		m_actionInfo.tActionType = type;
		m_actionInfo.dwReserved = 0;
		m_FxnCallBack(&m_alertInfo, &m_actionInfo);
		#endif
	}
	//报警输出
	type = ACTION_ALARMOUT;
	if (m_tAction.bEnable & (1 << (ACTION_ALARMOUT >> 16)))
	{
		//printf("alarmtype = %d ch = %d max = %d dopch = 0x%08x\n",m_AlarmType,m_MaxAlarmOut,m_iChannel,m_tAction.bDopCH);
		for (ch = 0; ch < m_MaxAlarmOut; ch++)
		{
			if (m_tAction.bDopCH & (1 << ch))
			{
				m_actionInfo.nChannel = ch;
				m_actionInfo.tActionType = type;
				m_actionInfo.dwReserved = 0;
				m_FxnCallBack(&m_alertInfo, &m_actionInfo);
			}
		}
	}
	//屏显
	type = ACTION_SCREEN;
	if ((m_tAction.bEnable & (1 << (ACTION_SCREEN >> 16))) && (m_tAction.bEnable & (1 << (ACTION_RECORD >> 16))))
	{
#if 0
		for (ch = 0; ch < MAX_ACTION_CH_NUM; ch++)
		{
			if (m_tAction.bRecordCH & (1 << ch))
			{
				m_actionInfo.nChannel = ch;
				m_actionInfo.tActionType = type;
				m_actionInfo.dwReserved = m_tAction.bScreenCH;

				m_FxnCallBack(&m_alertInfo, &m_actionInfo);
			}
		}
#else
		m_actionInfo.nChannel = m_tAction.bRecordCH;
		m_actionInfo.tActionType = type;
		m_actionInfo.dwReserved = m_tAction.bScreenCH;
		if (m_actionInfo.dwReserved && m_actionInfo.nChannel)
		{
			m_FxnCallBack(&m_alertInfo, &m_actionInfo);
		}
#endif
	}
	//声音
	type = ACTION_BEEP;
	if (m_tAction.bEnable & (1 << (ACTION_BEEP >> 16)))
	{
		m_actionInfo.nChannel = 0;
		m_actionInfo.tActionType = type;
		m_actionInfo.dwReserved = 0;
		m_FxnCallBack(&m_alertInfo, &m_actionInfo);
	}
	//上传报警服务器
	type = ACTION_SENDTOSERVER;
	if (m_tAction.bEnable & (1 << (ACTION_SENDTOSERVER >> 16)))
	{
		m_actionInfo.nChannel = 0;
		m_actionInfo.tActionType = type;
		m_actionInfo.dwReserved = 0;
		m_FxnCallBack(&m_alertInfo, &m_actionInfo);
	}
	//Email
	type = ACTION_EMAIL;
	if (m_tAction.bEnable & (1 << (ACTION_EMAIL >> 16)))
	{
		m_actionInfo.nChannel = 0;
		m_actionInfo.tActionType = type;
		m_actionInfo.dwReserved = 0;
		m_FxnCallBack(&m_alertInfo, &m_actionInfo);
	}
	//短信
	type = ACTION_SMS;
	if (m_tAction.bEnable & (1 << (ACTION_SMS >> 16)))
	{
		m_actionInfo.nChannel = 0;
		m_actionInfo.tActionType = type;
		m_actionInfo.dwReserved = 0;
		m_FxnCallBack(&m_alertInfo, &m_actionInfo);
	}
	//PTZ
	if (m_tAction.bEnable & (1 << (ACTION_PTZ >> 16)))
	{
		for (ch = 0; ch < m_MaxCh; ch++)
		{
			if (m_tAction.bPtzCH & (1 << ch))
			{
				type = ACTION_SCREEN;
				switch (m_tAction.iPtzAction[ch] >> 16)
				{
					case 0:	//无动作
						type = ACTION_PTZ;
						break;
					case 1:	//预置点
						type = ACTION_PTZ_PRESET;
						break;
					case 2:	//巡航
						type = ACTION_PTZ_CRUISE;
						break;
					case 3:	//轨迹
						type = ACTION_PTZ_TRACK;
						break;
					default:
						type = ACTION_PTZ;
						break;
				}
				if (type == ACTION_PTZ)
				{
					continue;
				}
				m_actionInfo.nChannel = ch;
				m_actionInfo.tActionType = type;
				m_actionInfo.dwReserved = m_tAction.iPtzAction[ch] & 0xFFFF;

				m_FxnCallBack(&m_alertInfo, &m_actionInfo);
			}
		}
	}
	//抓图
	type = ACTION_SNAPSHOT;
	if (m_tAction.bEnable & (1 << (ACTION_SNAPSHOT >> 16)))
	{
		for (ch = 0; ch < m_MaxCh; ch++)
		{
			if (m_tAction.bSnapshotCH & (1 << ch))
			{
				m_actionInfo.nChannel = ch;
				m_actionInfo.tActionType = type;
				m_actionInfo.dwReserved = 0;

				m_FxnCallBack(&m_alertInfo, &m_actionInfo);
			}
		}
	}

	//推送
    switch (m_alertInfo.AlertType)
    {
		case ALERT_TYPE_ALARMIN:
		{
			//触发时才需要推送的类型
			if (1 == m_alertInfo.dwReserved)	//报警开始
			{
				type = ACTION_PUSH;
				if (m_tAction.bEnable & (1 << (ACTION_PUSH >> 16)))
				{
					m_actionInfo.nChannel = 0;
					m_actionInfo.tActionType = type;
					m_actionInfo.dwReserved = 0;
					m_FxnCallBack(&m_alertInfo, &m_actionInfo);
				}
			}
			else	//报警结束
			{
				//do something
			}

			break;
		}

		case ALERT_TYPE_HUMANSHAPE:
		{
            if (b_nPushEventEnable[1])
			{
    			//触发时才需要推送的类型
    			if (1 == m_alertInfo.dwReserved)	//报警开始
    			{
    				type = ACTION_PUSH;
    				if (m_tAction.bEnable & (1 << (ACTION_PUSH >> 16)))
    				{
    					m_actionInfo.nChannel = 0;
    					m_actionInfo.tActionType = type;
    					m_actionInfo.dwReserved = 0;
    					m_FxnCallBack(&m_alertInfo, &m_actionInfo);
    				}
    			}
    			else	//报警结束
    			{
    				//do something
    			}
            }
            break;
		}
		case ALERT_TYPE_LONG_AWAY:
		{
			//触发时才需要推送的类型
			if (1 == m_alertInfo.dwReserved)	//报警开始
			{
				type = ACTION_PUSH;
				if (m_tAction.bEnable & (1 << (ACTION_PUSH >> 16)))
				{
					m_actionInfo.nChannel = 0;
					m_actionInfo.tActionType = type;
					m_actionInfo.dwReserved = 0;
					m_FxnCallBack(&m_alertInfo, &m_actionInfo);
				}
			}
			else	//报警结束
			{
				//do something
			}
            break;
		}
        //声音侦测
		case ALERT_TYPE_SOUNDDETECT:
		{
			if (b_nPushEventEnable[1])
			{
				//触发时才需要推送的类型
				if (1 == m_alertInfo.dwReserved)	//报警开始
				{
					type = ACTION_PUSH;
					if (m_tAction.bEnable & (1 << (ACTION_PUSH >> 16)))
					{
						m_actionInfo.nChannel = 0;
						m_actionInfo.tActionType = type;
						m_actionInfo.dwReserved = 0;
						m_FxnCallBack(&m_alertInfo, &m_actionInfo);
					}
				}
				else	//报警结束
				{
					//do something
				}
			}
			
			break;
		}

		case ALERT_TYPE_MOTION:
		{
            if (b_nPushEventEnable[0])
			{
    			//触发时才需要推送的类型
    			if (1 == m_alertInfo.dwReserved)	//报警开始
    			{
    				type = ACTION_PUSH;
    				if (m_tAction.bEnable & (1 << (ACTION_PUSH >> 16)))
    				{
    					m_actionInfo.nChannel = 0;
    					m_actionInfo.tActionType = type;
    					m_actionInfo.dwReserved = 0;
    					m_FxnCallBack(&m_alertInfo, &m_actionInfo);
    				}
    			}
    			else	//报警结束
    			{
    				//do something
    			}
            }
			break;
		}

		//!编码异常
		case ALERT_TYPE_ENCODEERROR:
		//!硬盘错误
		case ALERT_TYPE_DISKERROR:
		//!硬盘满
		case ALERT_TYPE_DISKFULL:
		//!IP冲突
		case ALERT_TYPE_IPCONFLICT:
		//!非法访问
		case ALERT_TYPE_ILLEGEACCESS:
		//!网线断
		case ALERT_TYPE_RETICLEDISCONNECT:
		//!视频丢失
		case ALERT_TYPE_VIDEOLOST:
		//!视频异常
		case ALERT_TYPE_VIDEOEXCEPTION:
		//!盘组异常
		case ALERT_TYPE_DISKGROUP_ERROR:
		//!IP地址变化,用于发送HELLO消息
	    case ALERT_TYPE_IPCHANGED:
		{
			//触发及消警都需要推送的类型
			
			if (1 == m_alertInfo.dwReserved)	//报警开始
			{
				type = ACTION_PUSH;
				if (m_tAction.bEnable & (1 << (ACTION_PUSH >> 16)))
				{
					m_actionInfo.nChannel = 0;
					m_actionInfo.tActionType = type;
					m_actionInfo.dwReserved = 0;
					m_FxnCallBack(&m_alertInfo, &m_actionInfo);
				}
			}
			else	//报警结束
			{
				//推送消警信息
				type = ACTION_PUSH;
				if (m_tAction.bEnable & (1 << (ACTION_PUSH >> 16)))
				{
					m_actionInfo.nChannel = 0;
					m_actionInfo.tActionType = type;
					m_actionInfo.dwReserved = 0;
					m_FxnCallBack(&m_alertInfo, &m_actionInfo);
				}
			}

			break;
		}

		default:
		{
			ALARM_WARN("[CActionAlarm] AlertType Unsupport!<%d>\n", m_alertInfo.AlertType);
		}
    }

	if (m_MotionCallBack)
	{
		m_MotionCallBack(0); //0:ftp  1:sd  2:samba
	}
	
	return 0;
}

int SetMotionCallBack(MotionCallBack CallBack)
{
    if (NULL == CallBack)
    {
        return -1;        
    }
    
    m_MotionCallBack = CallBack;
    
    return 0;
}

