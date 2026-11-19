
/*!
*\file   AlarmCfg.h 
*\brief  报警参数的配置及读取
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/


//#include <iostream.h>
#include <string.h>
#include <sys/select.h>
#include "AlarmCfg.h"
#include "idvr_errno.h"
#include "ddt_so.h"
#include "debug.h"

#include "zwsdk.h"
#include "PAR/CConfig.h"

#include "STR_global.h"

/**************************************************************************
*                            全局宏定义                                  *
 **************************************************************************/

//下面为保存参数时定义的section及key值
//模块section




//声明全局函数
extern int GetEncChNum();

extern int GetAlarmOutNum();


//动作类型个数
const int MAX_ALARM_ACTION = 14;

struct __ActionDec__
{
	char key[30];				//在配置文件中的key
	int nCh;					//是否与通道有关.无关为0，有关为实际通道个数
	int Child;					//子类的个数
};

struct __ActionDec__ ActionDec[] = {
	{
	 GUI_KEY, 0, 0},
	{
	 RECORD_KEY, 0, 0},
	{
	 ALARMOUT_KEY, 0, 0},
	{
	 SCREEN_KEY, 0, 0},
	{
	 BEEP_KEY, 0, 0},
	{
	 TOSERVER_KEY, 0, 0},
	{
	 EMAIL_KEY, 0, 0},
	{
	 SMS_KEY, 0, 0},
	{
	 PTZ_KEY, 0, 3},
	{
	 SNAPSHOT_KEY, 0, 0},
	{
	 FTP_KEY, 0, 0},
	{
	 PUSH_KEY, 0, 0},
	 {
	 MASK_KEY, 0, 0},
	{
	 LIGHTALARM_KEY, 0, 0}
};

static const char *g_actionkey[] = { GUI_KEY, RECORD_KEY, ALARMOUT_KEY, SCREEN_KEY, BEEP_KEY, TOSERVER_KEY, EMAIL_KEY, SMS_KEY, PTZ_KEY, SNAPSHOT_KEY, FTP_KEY, PUSH_KEY, MASK_KEY, LIGHTALARM_KEY};



//!\brief 设置回调。
//!\param int Type 错误类型
//!\param CBAlert callabck 回调指针
//!\return 成功为0，失败为错误码
int CAlarm::SetCallBack(int AlarmType, CBAlert callabck)
{
	LOCK(mutex);
	m_nAlarmType = AlarmType;
	m_AlarmCallBack = callabck;
	DEBUG_MSG("异常%s 设置回调函数%p\n", m_section, callabck);
	UNLOCK(mutex);
	return 0;
}

int CAlarm::PushAlarm(void *org, struct tagAlertInfo *Info)
{
	//int DiskErrorType = *((int *)org);
	int i, j;

	int errortype = (int) ((int *) org);

	struct tagAlertInfo AlertInfo;

	struct tagActionInfo ActionInfo;

	LOCK(mutex);
	if (Info != NULL)
	{
		TRACE("出现异常:%d m_section = %s Ch = %d\n", m_nAlarmType, m_section, Info->nChannel);
	}
	else
	{
		TRACE("出现异常:%d m_section = %s Ch = %d\n", m_nAlarmType, m_section, 0);
	}

	if (NULL == m_AlarmCallBack)
	{
		TRACE("没有设置异常回调函数\n");
		UNLOCK(mutex);
		return -1;
	}

	if (NULL == m_tagAlarmActionCfg)
	{
		TRACE("申请配置空间失败\n");
		UNLOCK(mutex);
		return IDVR_ERROR_ALLOC;
	}

	memset(&AlertInfo, 0, sizeof (struct tagAlertInfo));
	memset(&ActionInfo, 0, sizeof (struct tagActionInfo));

	if (Info != NULL)
	{
		AlertInfo.nChannel = Info->nChannel;
		AlertInfo.dwReserved = Info->dwReserved;
	}


	if (0 != errortype)
	{
		//所有硬盘出现错误
		TRACE("特殊出现错误m_nAlarmType = %d,errortype = %d AlertInfo.nChannel = %d\n", m_nAlarmType, errortype, AlertInfo.nChannel);
	}

	AlertInfo.dwReserved = errortype;

	if (1 == errortype)
	{
		AlertInfo.AlertType = m_nAlarmType;
		UNLOCK(mutex);
		m_AlarmCallBack(&AlertInfo, &ActionInfo);
		return 0;
	}


	//typedef int (*CBAlert)(struct tagAlertInfo *pAlertInfo, struct tagActionInfo *pActionInfo)
	for (i = 0; i < MAX_ALARM_ACTION; i++)
	{
		//如果启用了该报警动作
		if (m_tagAlarmActionCfg[i].bEnable)
		{
			//如果存在通道概念
			if (ActionDec[i].nCh)
			{
				for (j = 0; j < ActionDec[i].nCh; j++)
				{
					if (m_tagAlarmActionCfg[i].ChInfo[j].bEnable)
					{
						//如果存在子类
						if (ActionDec[i].Child)
						{
							AlertInfo.AlertType = m_nAlarmType;
							ActionInfo.nChannel = j;
							ActionInfo.tActionType = ((MAKEACTIONTYPE(i)) + m_tagAlarmActionCfg[i].ChInfo[j].Type + 1);
							ActionInfo.dwReserved = m_tagAlarmActionCfg[i].ChInfo[j].No;
							UNLOCK(mutex);
							m_AlarmCallBack(&AlertInfo, &ActionInfo);
							LOCK(mutex);
						}
						else
						{
							AlertInfo.AlertType = m_nAlarmType;
							ActionInfo.nChannel = j;
							ActionInfo.tActionType = (MAKEACTIONTYPE(i));
							UNLOCK(mutex);
							m_AlarmCallBack(&AlertInfo, &ActionInfo);
							LOCK(mutex);
						}
					}			//if ( m_tagAlarmActionCfg[i].ChInfo[j].bEnable)
					else
					{
						DEBUG_MSG("动作:%x没有联动通道%d\n", i, j);
					}
				}				//for ( j = 0; j < GetEncChNum(); j++ )
			}					//end if ( ActionDec[i].bCh )
			else
			{
				AlertInfo.AlertType = m_nAlarmType;
				ActionInfo.nChannel = 0;
				ActionInfo.tActionType = (MAKEACTIONTYPE(i));
				UNLOCK(mutex);
				m_AlarmCallBack(&AlertInfo, &ActionInfo);
				LOCK(mutex);
			}
		}						//end if (ActionCfg[i].bEnable)
		else
		{
			DEBUG_MSG("动作:%x没有联动\n", i);
		}
	}							//end for ( i = 0; i < MAX_ALARM_ACTION; i++ )

	UNLOCK(mutex);
	return 0;
}

//!\brief 开启/禁止某个报警联动动作
//!\praram int AlarmType 报警类型。VIDEO_LOST 
//!\praram int Action    !      ACTION_SCREEN 屏幕显示
//  !      ACTION_BEEP 声音提示
//  !      ACTION_ALARMOUT 报警输出
//  !      ACTION_SENDTOSERVER 上传报警服务器
//  !      ACTION_EMAIL 联动Email
//  !      ACTION_SMS 联动短信
//!\bool bEnahle true为开启，false为禁止
//!\return 成功为0，失败为错误码
//##ModelId=49BDBA5C02A0
int CAlarm::EnableAction(int AlarmType, tagActionType Action, bool bEnable)
{
	char key[20];

	CConfig *theCConfig = CConfig::GetInstance();
DEBUG_MSG("my add: EnableAction AlarmType=%d, Action=0x%x, bEnable=%d\n",AlarmType,Action,bEnable);
	LOCK(mutex);

	if (NULL == m_tagAlarmActionCfg)
	{
		TRACE("申请配置空间失败\n");
		UNLOCK(mutex);
		return IDVR_ERROR_ALLOC;
	}

	//动作的主类型
	if (0 == (Action & 0xffff))
	{
		m_tagAlarmActionCfg[ACTIONTYPE(Action)].bEnable = bEnable;

		if (NULL != theCConfig)
		{
			snprintf(key, sizeof(key), "%s", g_actionkey[ACTIONTYPE(Action)]);
			theCConfig->SetValue(m_section, key, (int) m_tagAlarmActionCfg[ACTIONTYPE(Action)].bEnable);
		}
		DEBUG_MSG("my add: m_section = %s, key=%s\n",m_section, key);
	}
	else
	{

/*		if (true == bEnable)
		{
			m_tagAlarmActionCfg[ACTIONTYPE(Action)].ChInfo[ACTIONCHILDTYPE(Action)].Type = ACTIONCHILDTYPE(Action);
		}
		else
		{
			m_tagAlarmActionCfg[ACTIONTYPE(Action)].ChInfo[ACTIONCHILDTYPE(Action)].Type = 0;
		}
		
		if ( NULL != theCConfig)
		{
			snprintf( key, sizeof(key), "%s", g_actionkey[ACTIONTYPE(Action)]);
			
			theCConfig->SetValue(m_section, key,\
				((m_tagAlarmActionCfg[ACTIONTYPE(Action)].ChInfo[ACTIONCHILDTYPE(Action)].Type) << 16) + \
				m_tagAlarmActionCfg[ACTIONTYPE(Action)].ChInfo[ACTIONCHILDTYPE(Action)].No);
		}
		//动作的辅助类型
		m_tagAlarmActionCfg[ACTIONTYPE(Action)].ChInfo[ACTIONCHILDTYPE(Action)].bEnable = bEnable;
*/
	}

	UNLOCK(mutex);

	return 0;
}


//!\brief 判断某个报警联动动作是否开启
//!\praram int AlarmType 报警类型。
//!\praram int Action    !      ACTION_SCREEN 屏幕显示
//  !      ACTION_BEEP 声音提示
//  !      ACTION_ALARMOUT 报警输出
//  !      ACTION_SENDTOSERVER 上传报警服务器
//  !      ACTION_EMAIL 联动Email
//  !      ACTION_SMS 联动短信
//!\return false 为禁止联动 true 为开启联动
//##ModelId=49BDBA5C02A0
bool CAlarm::IsEnableAction(int AlarmType, tagActionType Action)
{
	bool ret = false;
	CConfig *theCConfig = CConfig::GetInstance();
	char key[20] ={0};
	
	LOCK(mutex);

	if (NULL == m_tagAlarmActionCfg)
	{
		TRACE("申请配置空间失败\n");
		UNLOCK(mutex);
		return IDVR_ERROR_ALLOC;
	}

	if (0 == (Action & 0xffff))
	{
#if 0
		ret = m_tagAlarmActionCfg[ACTIONTYPE(Action)].bEnable;
#else
	if (NULL != theCConfig)
	{
		snprintf(key, sizeof(key), "%s", g_actionkey[ACTIONTYPE(Action)]);
		ret = theCConfig->GetValue(m_section, key, 1);
	}
#endif		
	}

	UNLOCK(mutex);
	
	DEBUG_MSG("my add: IsEnableAction AlarmType=%d, Action=0x%x enable=%d\n",AlarmType,Action,ret);
	return ret;
}


//!\brief 设置某个联动动作涉及的通道。无通道概念的不需要设置
//!\param int nCh 通道
//!\praram int AlarmType 报警类型
//!\praram tagActionType Action   动作类型
//!\praram int AlarmCh 联动动作涉及到通道。
//!\praram bool bEnable 启用/不启用该通道的配置
//!\praram int dwReserved 保留字。对于预置点、轨迹、巡航线路输入上述编号
//!\return  成功为0，失败为-1
//##ModelId=49BDBB6C0341
int CAlarm::SetActionChParam(int AlarmType, tagActionType Action, int AlarmCh, bool bEnable, int nReserved)
{
	char key[20];

	int ret = IDVR_ERROR_PARAMETER;

	CConfig *theCConfig = CConfig::GetInstance();

	LOCK(mutex);

	if (NULL == m_tagAlarmActionCfg[ACTIONTYPE(Action)].ChInfo)
	{
		TRACE("该动作类型:%d不存在通道概念\n", (int) Action);
		UNLOCK(mutex);
		return ret;
	}


	if (NULL == m_tagAlarmActionCfg)
	{
		TRACE("申请配置空间失败\n");
		UNLOCK(mutex);
		return ret;
	}


	if ((AlarmCh >= ActionDec[ACTIONTYPE(Action)].nCh))
	{
		//实际通道个数
		TRACE("动作类型：%x 通道参数错误\n", Action);
		UNLOCK(mutex);
		return ret;
	}

	//动作的主类型
	if (0 == (Action & 0xffff))
	{
		m_tagAlarmActionCfg[ACTIONTYPE(Action)].ChInfo[AlarmCh].bEnable = bEnable;

		if (NULL != theCConfig)
		{
			if (ActionDec[ACTIONTYPE(Action)].nCh && (ActionDec[ACTIONTYPE(Action)].Child == 0))
			{
				snprintf(key, sizeof(key), "%s-%s%d", g_actionkey[ACTIONTYPE(Action)], CH_KEY, AlarmCh);
				ret = theCConfig->SetValue(m_section, key, bEnable);
			}
			else
			{
				TRACE("参数检查出错\n");
			}
		}
	}
	else
	{
		if (true == bEnable)
		{
			m_tagAlarmActionCfg[ACTIONTYPE(Action)].ChInfo[AlarmCh].Type = ACTIONCHILDTYPE(Action) + 1;
			m_tagAlarmActionCfg[ACTIONTYPE(Action)].ChInfo[AlarmCh].No = nReserved;
		}

		//动作的辅助类型
		m_tagAlarmActionCfg[ACTIONTYPE(Action)].ChInfo[AlarmCh].bEnable = bEnable;

		if (NULL != theCConfig)
		{
			snprintf(key, sizeof(key), "%s-%s%d", g_actionkey[ACTIONTYPE(Action)], CH_KEY, AlarmCh);
			ret = theCConfig->SetValue(m_section, key, ((ACTIONCHILDTYPE(Action) + 1) << 16) + nReserved);
		}
	}

	UNLOCK(mutex);
	return ret;
}



//!\brief 获取某个联动动作涉及的通道或者编号
//!\praram int AlarmType 报警类型
//!\praram tagActionType Action 动作类型
//!\praram bool *szCh 通道数组，true为联动了该通道，false为没有联动该通道
//!\praram int *szReserved 对应通道涉及到的子类型数据。目前仅相对于预置点编号等
//!\return  成功为0，失败为-1
int CAlarm::GetActionChParam(int AlarmType, tagActionType Action, bool * szCh, int *szReserved)
{

	int i, ret = 0;

	LOCK(mutex);

	if (NULL == szCh)
	{
		TRACE("参数错误:NULL == szCh\n");
		UNLOCK(mutex);
		return IDVR_ERROR_PARAMETER;
	}

	if (NULL == m_tagAlarmActionCfg)
	{
		TRACE("申请配置空间失败\n");
		UNLOCK(mutex);
		return IDVR_ERROR_ALLOC;
	}

	if (NULL == m_tagAlarmActionCfg[ACTIONTYPE(Action)].ChInfo)
	{
		TRACE("该动作类型:%d不存在通道概念\n", (int) Action);
		UNLOCK(mutex);
		return IDVR_ERROR_PARAMETER;
	}
	else if (NULL == szCh)
	{
		TRACE("参数错误:NULL == szCh\n");
		UNLOCK(mutex);
		return IDVR_ERROR_PARAMETER;
	}

	for (i = 0; i < ActionDec[ACTIONTYPE(Action)].nCh; i++)
	{
		//辅类型
		if (0 != (Action & 0xffff))
		{

			if ((Action & 0xffff) == m_tagAlarmActionCfg[ACTIONTYPE(Action)].ChInfo[i].Type)
			{
				szCh[i] = m_tagAlarmActionCfg[ACTIONTYPE(Action)].ChInfo[i].bEnable;
				szReserved[i] = m_tagAlarmActionCfg[ACTIONTYPE(Action)].ChInfo[ACTIONCHILDTYPE(Action)].No;
			}
			else
			{
				szCh[i] = false;
				szReserved[i] = 0;
			}
		}
		else
		{
			szCh[i] = m_tagAlarmActionCfg[ACTIONTYPE(Action)].ChInfo[i].bEnable;
		}
	}

	UNLOCK(mutex);
	return ret;
}


//!\brief 读取配置
//!\praram 无
//!\return  成功为0,失败为-1
int CAlarm::ReadCfg()
{
	CConfig *theCConfig = CConfig::GetInstance();

	int ch;

	int i;

	char key[20];

	LOCK(mutex);
#ifndef TEST_MEM_INFO
	m_tagAlarmActionCfg = new tagActionCfg[MAX_ALARM_ACTION];
#else
	m_tagAlarmActionCfg = (tagActionCfg *)MemoryMalloc(sizeof(tagActionCfg) * MAX_ALARM_ACTION, "STR", __FILE__, __FUNCTION__, __LINE__);
#endif


	if (NULL == m_tagAlarmActionCfg)
	{
		TRACE("申请配置空间失败\n");
		UNLOCK(mutex);
		return IDVR_ERROR_ALLOC;
	}

	ActionDec[ACTIONTYPE(ACTION_RECORD)].nCh = GetEncChNum();
	ActionDec[ACTIONTYPE(ACTION_SCREEN)].nCh = GetEncChNum();
	ActionDec[ACTIONTYPE(ACTION_PTZ)].nCh = GetEncChNum();
	ActionDec[ACTIONTYPE(ACTION_SNAPSHOT)].nCh = GetEncChNum();

	ActionDec[ACTIONTYPE(ACTION_ALARMOUT)].nCh = GetAlarmOutNum();

	memset(m_tagAlarmActionCfg, 0, MAX_ALARM_ACTION * sizeof (tagActionCfg));
	DEBUG_MSG("ACTIONTYPE(ACTION_RECORD) = %d\n", ACTIONTYPE(ACTION_RECORD));

	for (i = 0; i < MAX_ALARM_ACTION; i++)
	{
		if (ActionDec[i].nCh > 0)
		{
#ifndef TEST_MEM_INFO
			m_tagAlarmActionCfg[i].ChInfo = new struct __CHINFO__[ActionDec[i].nCh];
#else
			m_tagAlarmActionCfg[i].ChInfo = (struct __CHINFO__ *)MemoryMalloc(sizeof(struct __CHINFO__) * (ActionDec[i].nCh), "STR", __FILE__, __FUNCTION__, __LINE__);
#endif
		}
		else
		{
			m_tagAlarmActionCfg[i].ChInfo = NULL;
		}
	}

	if (NULL == theCConfig)
	{
		DEBUG_MSG("读取配置失败\n");
		UNLOCK(mutex);
		return IDVR_ERROR_CONFIGFILE;
	}

	for (i = 0; i < MAX_ALARM_ACTION; i++)
	{
		//参数
		snprintf(key, sizeof(key), "%s", g_actionkey[i]);
		m_tagAlarmActionCfg[i].bEnable = theCConfig->GetValue(m_section, key, 0);
		DEBUG_MSG("[setion:%s key:%s]   = %d\n", m_section, key, theCConfig->GetValue(m_section, key, 0));

		//通道概念的动作类型
		for (ch = 0; ch < ActionDec[i].nCh; ch++)
		{
			int Value;

			snprintf(key, sizeof(key), "%s-%s%d", g_actionkey[i], CH_KEY, ch);
			Value = theCConfig->GetValue(m_section, key, 0);
			DEBUG_MSG("[setion:%s key:%s] = %d\n", m_section, key, theCConfig->GetValue(m_section, key, 0));

			if (ActionDec[i].Child)
			{
				m_tagAlarmActionCfg[i].ChInfo[ch].bEnable = ((Value & 0xffff0000) > 0);
				m_tagAlarmActionCfg[i].ChInfo[ch].No = (Value & 0xffff);
				m_tagAlarmActionCfg[i].ChInfo[ch].Type = ((Value >> 16) & 0xffff);
			}
			else
			{
				m_tagAlarmActionCfg[i].ChInfo[ch].bEnable = theCConfig->GetValue(m_section, key, 0);
			}
		}
	}

	UNLOCK(mutex);
	return 0;
}


//!\brief 初始该配置的段名称
//!\praram char *section 段名称
//!\return  成功为0,失败为-1
int CAlarm::Init(const char *section)
{
	if (section == NULL)
	{
		memset(m_section, 0, sizeof (m_section));
		return -1;
	}
	m_AlarmCallBack = NULL;//my add

	snprintf(m_section, sizeof(m_section), "%s", section);

	INIT_MUTEX(mutex);

	return 0;
}

extern "C" int ZWSys_GetTotalEncChannels();//MP4Sys_GetTotalEncChannels

int GetEncChNum()
{
	return ZWSys_GetTotalEncChannels();//1;
}

#if 1
//extern "C" int MP4Sys_GetBoardInfo();

//#define MP4Sys_GetBoardID()    MP4Sys_GetBoardInfo()
//#define MP4Sys_GetBoardID()    0xc005
#endif
int GetAlarmOutNum()
{
	static int nBoardId = 0;

	static DeviceDescription ddt;

	if (0 == nBoardId)
	{
		//nBoardId = MP4Sys_GetBoardID();
		if (ZWSys_GetBoardInfo(&nBoardId) != 0)
			{
			printf("\n GetBoardInfo!　BoardId ?ffail...\n");
		}
		EDVRS_DDT_GetDeviceDescriptionByID(&ddt, nBoardId);
	}

	return ddt.bytAlarmOutChannels;
}


extern void rsleep(unsigned int ntime)
{
	fd_set rfds;

	struct timeval tv;

	FD_ZERO(&rfds);
	FD_SET(0, &rfds);			/* Wait up to five seconds. */
	tv.tv_sec = ntime / 1000000;
	tv.tv_usec = ntime % 1000000;
	select(0, &rfds, NULL, NULL, &tv);	/* Don't rely on the value of tv now! */
}

#undef ACTIONTYPE
