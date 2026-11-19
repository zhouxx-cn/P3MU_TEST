
/*!
*\file		CPtzAdjuster.cpp
*\brief		PTZ模块，云台守望者接口实现
*\author	喻坤
*\date		2009/11/20
*/

#include <time.h>

#include "global_macro.h"
#include "idvr_errno.h"
#include "PUB/macro_pthread.h"
#include "PUB/boottime.h"
#include "PTZTrace.h"
#include "CPtzAdjuster.h"
#include "CPtzDev.h"
#include "PAR/CConfig.h"
#include "CPtzManager.h"
#include "tagPtzCode.h"
#include "tagActionInfo.h"
#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
#include "motoLib.h"
#endif

CPtzAdjuster *CPtzAdjuster::m_pInstance = NULL;

//!\brief 判断通道号是否合法
#define ASSERT_CHANNEL(ch)	(((ch) >= 0) && ((ch) < 4))
//!\brief  合成动作参数
#define MAKEACTION(type, index) (((dwIndex) << 24) | (dwActionType))




//!\brief 获取配置器
#define PTZRESET_SETCONFIG(key, value)								\
do																	\
{																	\
	CConfig *pConfig = CConfig::GetInstance();						\
	if (NULL == pConfig)											\
	{																\
		return IDVR_ERROR_CONFIGFILE;								\
	}																\
	int ret = pConfig->SetValue(PTZRESET_SECTION, key, value);		\
	if (0 != ret)													\
	{																\
		return ret;													\
	}																\
} while(0)

//!\brief 外连rsleep 定义在STR中:(
extern void rsleep(unsigned int utime);

static void InternalSleep(unsigned int utime);

static void SetActionToHW(unsigned int uAction, CPtzManager * pManager, int nChannel);

static void SetCycleToHW(time_t lCycle, CPtzManager * pManager, int nChannel);

static void SetEnableToHW(int bEnable, CPtzManager * pManager, int nChannel);

CPtzAdjuster::CPtzAdjuster():m_pManager(NULL), m_nChannelCount(0)
{
}

CPtzAdjuster::~CPtzAdjuster()
{
}

CPtzAdjuster *CPtzAdjuster::GetInstance()
{
	if (NULL == m_pInstance)
	{
		m_pInstance = new CPtzAdjuster();
	}

	return m_pInstance;
}

//!\brief 初始化复位器
//!\param PTZManager
//!\param pDev云台设备数组
//!\param 传入通道数
//!\return 0表示失败,其他表示错误码
int CPtzAdjuster::Init(CPtzManager * pManager, int nChNum)
{
	pthread_t tThread;

	m_pManager = pManager;
	m_nChannelCount = nChNum;

	//读取自动复位参数
	CConfig *pConfig = CConfig::GetInstance();

	if (NULL == pConfig)
	{
		return IDVR_ERROR_CONFIGFILE;
	}

	int i = 0;

	char acKeyName[64];
	char szSectionName[64] = {0};
	char acKeyValue[256];

	CConfig *pCfg = CConfig::NewInstance();
    int s32Ret = pCfg->LoadFile(CUSTOM_ABILITY_FILE);
    if (0 == s32Ret)
    {
	#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
        m_bSupportMotor = pCfg->GetValue("FunctionSupport", "IsSupportMotor", 0);
		m_bSupportMotor	= (m_bSupportMotor == 1) ? 1 : 0;
	#endif
    }
    CConfig::ReleaseInstance(pCfg);

	for (i = 0; i < nChNum; i++)
	{
		//读取守望是否启用
		snprintf(acKeyName,sizeof(acKeyName), PTZRESET_ENABLE_PREFIX "_%02d", i);
		m_bEnable[i] = pConfig->GetValue(PTZRESET_SECTION, acKeyName, 0);
		//读取守望周期
		snprintf(acKeyName,sizeof(acKeyName), PTZRESET_CYCLE_PREFIX "_%02d", i);
		m_lResetCycle[i] = pConfig->GetValue(PTZRESET_SECTION, acKeyName, 0);
		//读取守望动作
		snprintf(acKeyName,sizeof(acKeyName), PTZRESET_ACTION_PREFIX "_%02d", i);
		pConfig->GetValue(PTZRESET_SECTION, acKeyName, "0x00000000", acKeyValue, 256);
		//读取PTZ使能
		snprintf(acKeyName,sizeof(acKeyName), PTZRESET_ENABLE_PTZ "_%02d", i);
		m_bEnablePTZ[i]=pConfig->GetValue(PTZRESET_SECTION, acKeyName, 0);
		//判断action value是否以0x开头
		if (('0' != acKeyValue[0]) || ('x' != acKeyValue[1]))
		{
			m_uDefaultAction[i] = 0;
		}
		else
		{
			m_uDefaultAction[i] = strtol(acKeyValue, NULL, 16);
		}
		m_uAlarmAction[i] = 0;
		m_lIdleTime[i] = 0;
		if (pManager->PTZ_HasFunction(PTZ_FLAG_AUXHWGUARD, i))
		{
			SetEnableToHW(m_bEnable[i], pManager, i);
			SetActionToHW(m_uDefaultAction[i], pManager, i);
			SetCycleToHW(m_lResetCycle[i], pManager, i);
		}
	}
    if(pConfig)
    {
	#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
		m_nCruiseEnable = pConfig->GetValue(PTZRESET_ADVANCE_CFG, "CruiseEnable", 1);
		m_nBeginPresetNumForCruise= pConfig->GetValue(PTZRESET_ADVANCE_CFG, "BeginPresetNumForCruise", 98);
		m_nBeginPresetActionForCruise= pConfig->GetValue(PTZRESET_ADVANCE_CFG, "BeginPresetActionForCruise", 2);
		m_nEndPresetNumForCruise= pConfig->GetValue(PTZRESET_ADVANCE_CFG, "EndPresetNumForCruise", 98);
		m_nEndPresetActionForCruise= pConfig->GetValue(PTZRESET_ADVANCE_CFG, "EndPresetActionForCruise", 1);
		m_nWatchEnable= pConfig->GetValue(PTZRESET_ADVANCE_CFG, "WatchEnable", 1);
		m_nBeginPresetNumForWatch= pConfig->GetValue(PTZRESET_ADVANCE_CFG, "BeginPresetNumForWatch", 125);
		m_nBeginPresetActionForWatch= pConfig->GetValue(PTZRESET_ADVANCE_CFG, "BeginPresetActionForWatch", 1);
		m_nEndPresetNumForWatch= pConfig->GetValue(PTZRESET_ADVANCE_CFG, "EndPresetNumForWatch", 125);
		m_nEndPresetActionForWatch= pConfig->GetValue(PTZRESET_ADVANCE_CFG, "EndPresetActionForWatch", 2);
		m_nTrackEnable = pConfig->GetValue(PTZRESET_ADVANCE_CFG, "TrackEnable", 1);
		m_nBeginPresetNumForTrack= pConfig->GetValue(PTZRESET_ADVANCE_CFG, "BeginPresetNumForTrack", 97);
		m_nBeginPresetActionForTrack= pConfig->GetValue(PTZRESET_ADVANCE_CFG, "BeginPresetActionForTrack", 2);
		m_nEndPresetNumForTrack= pConfig->GetValue(PTZRESET_ADVANCE_CFG, "EndPresetNumForTrack", 96);
		m_nEndPresetActionForTrack= pConfig->GetValue(PTZRESET_ADVANCE_CFG, "EndPresetActionForTrack", 1);
	#endif

		snprintf(szSectionName,sizeof(szSectionName), "PTZ_Set_%d", 0);
		m_nSpeedV[PTZ_MAIN] = pConfig->GetValue(szSectionName, "MotorSpeedV", 1);
		m_nSpeedV[PTZ_MAIN] = m_nSpeedV[PTZ_MAIN] > 9 ? 9 : (m_nSpeedV[PTZ_MAIN] < 0 ? 0 : m_nSpeedV[PTZ_MAIN]);
		
		m_nSpeedH[PTZ_MAIN] = pConfig->GetValue(szSectionName, "MotorSpeedH", 1);
		m_nSpeedH[PTZ_MAIN] = m_nSpeedH[PTZ_MAIN] > 9 ? 9 : (m_nSpeedH[PTZ_MAIN] < 0 ? 0: m_nSpeedH[PTZ_MAIN]);

		m_nSpeed[PTZ_MAIN] = m_nSpeedH[PTZ_MAIN];
#if (DEVICE_SUPPORT_PTZ_NUM) >= 3
		memset(szSectionName, 0, sizeof(szSectionName));
		snprintf(szSectionName,sizeof(szSectionName), PTZ_MULTIDEV_CFG_SECTION);
		m_nSpeedH[PTZ_LERF] = pConfig->GetValue(szSectionName, PTZ_LR_HORZ_MAXSPEED, 1);
		m_nSpeedH[PTZ_RIGHT] = pConfig->GetValue(szSectionName, PTZ_LR_HORZ_MAXSPEED, 1);
		m_nSpeedV[PTZ_LERF] = pConfig->GetValue(szSectionName, PTZ_LR_VERT_MAXSPEED, 1);
		m_nSpeedV[PTZ_RIGHT] = pConfig->GetValue(szSectionName, PTZ_LR_VERT_MAXSPEED, 1);
		m_nSpeed[PTZ_LERF] = m_nSpeedH[PTZ_LERF];
		m_nSpeed[PTZ_RIGHT] = m_nSpeedH[PTZ_RIGHT];
#endif
	}

	//创建守望线程
	int ret = pthread_create(&tThread, NULL, CheckTimer, (void *) nChNum);

	if (0 == ret)
	{
		pthread_detach(tThread);
	}
	else
	{
		PTZ_INFO("Create PTZ Reset Daemon Thread\n");
	}

	return ret;
}


//!\brief 设置是否启用云台守望功能
//!\param bEnable 0表示不启用，其他表示启用
//!\param nChannel 通道号
//!\return 0表示设置成功，其他表示错误码
int CPtzAdjuster::EnableKeeper(int bEnable, int nChannel)
{
	if (!ASSERT_CHANNEL(nChannel))
	{
		PTZ_ERROR("Channel = %d\n", nChannel);
		return IDVR_ERROR_PARAMETER;
	}

	//检查参数是否改动
	if (bEnable == m_bEnable[nChannel])
	{
		return 0;
	}

	char acKeyName[50];

	snprintf(acKeyName,sizeof(acKeyName), PTZRESET_ENABLE_PREFIX "_%02d", nChannel);
	PTZRESET_SETCONFIG(acKeyName, bEnable);
	m_bEnable[nChannel] = bEnable;
	ResetTimer(nChannel);
	if (m_pManager->PTZ_HasFunction(PTZ_FLAG_AUXHWGUARD, nChannel))
	{
		SetEnableToHW(bEnable, m_pManager, nChannel);
	}

	return 0;
}

//!\brief 获取云台是否启用守望功能
//!\param nChannel 通道号
//!\return 0表示不启用,其他表示启用
int CPtzAdjuster::IsEnableKeeper(int nChannel)
{
	if (!ASSERT_CHANNEL(nChannel))
	{
		PTZ_ERROR("Channel = %d\n", nChannel);
		return 0;
	}
	return m_bEnable[nChannel];
}

//!\brief 设置是否启用云台
//!\param bEnable 0表示不启用，其他表示启用
//!\param nChannel 通道号
//!\return 0表示设置成功，其他表示错误码
int CPtzAdjuster::EnablePTZ(int bEnable, int nChannel)
{
	if (!ASSERT_CHANNEL(nChannel))
	{
		PTZ_ERROR("Channel = %d\n", nChannel);
		return IDVR_ERROR_PARAMETER;
	}

	//检查参数是否改动
	if (bEnable == m_bEnablePTZ[nChannel])
	{
		return 0;
	}

	char acKeyName[50];
	snprintf(acKeyName,sizeof(acKeyName), PTZRESET_ENABLE_PTZ"_%02d", nChannel);
	PTZRESET_SETCONFIG(acKeyName, bEnable);
	m_bEnablePTZ[nChannel] = bEnable;
	//ResetTimer(nChannel);

	return 0;
}

//!\brief 获取云台是否启用
//!\param nChannel 通道号
//!\return 0表示不启用,其他表示启用
int CPtzAdjuster::IsEnablePTZ(int nChannel)
{
	if (!ASSERT_CHANNEL(nChannel))
	{
		PTZ_ERROR("Channel = %d\n", nChannel);
		return 0;
	}
	return m_bEnablePTZ[nChannel];
}

//!\brief 获取云台复位周期
//!\param nChannel 云台通道号
//!\return 0表示设置成功 其他表示错误码
time_t CPtzAdjuster::GetResetCycle(int nChannel)
{
	if (!ASSERT_CHANNEL(nChannel))
	{
		PTZ_ERROR("Channel = %d\n", nChannel);
		return 0;
	}
	return m_lResetCycle[nChannel];
}

//!\brief 设置云台复位周期
//!\param nSecond 设置云台复位需要空闲的秒数
//!\param nChannel 云台通道号
//!\return 0表示设置成功 其他表示错误码
int CPtzAdjuster::SetResetCycle(time_t nSecond, int nChannel)
{
	if (!ASSERT_CHANNEL(nChannel))
	{
		PTZ_ERROR("Channel = %d\n", nChannel);
		return -1;
	}

	//检查参数
	if (nSecond == m_lResetCycle[nChannel])
	{
		return 0;
	}

	char acKeyName[50];

	snprintf(acKeyName,sizeof(acKeyName), PTZRESET_CYCLE_PREFIX "_%02d", nChannel);
	PTZRESET_SETCONFIG(acKeyName, nSecond);
	m_lResetCycle[nChannel] = nSecond;
	ResetTimer(nChannel);
	if (m_pManager->PTZ_HasFunction(PTZ_FLAG_AUXHWGUARD, nChannel))
	{
		SetCycleToHW(m_lResetCycle[nChannel], m_pManager, nChannel);
	}

	return 0;
}

//!\brief 获取云台默认动作的接口
//!\param pdwActionType 默认动作类型
//!\param pdwIndex 动作相关参数，（预置点号，巡航线路号，轨迹号，扫描线路号）
//!\param nChannel 云台通道号
int CPtzAdjuster::GetDefaultAction(unsigned long *pdwActionType, unsigned long *pdwIndex, int nChannel)
{
	if (!ASSERT_CHANNEL(nChannel))
	{
		PTZ_ERROR("Channel = %d\n", nChannel);
		return IDVR_ERROR_PARAMETER;
	}
	*pdwActionType = PTZ_ACTIONTYPE(m_uDefaultAction[nChannel]);
	*pdwIndex = ACTIONINDEX(m_uDefaultAction[nChannel]);
	return 0;
}

//!\brief 设置云台默认动作的接口
//!\param dwActionType 默认动作类型
//!\param dwIndex 动作相关参数，（预置点号，巡航线路号，轨迹号，扫描线路号）
//!\param nChannel 云台通道号
int CPtzAdjuster::SetDefaultAction(unsigned long dwActionType, unsigned long dwIndex, int nChannel)
{
	if (!ASSERT_CHANNEL(nChannel))
	{
		PTZ_ERROR("Channel = %d\n", nChannel);
		return IDVR_ERROR_PARAMETER;
	}
	unsigned int uAction = MAKEACTION(dwActionType, dwIndex);

	//参数未变化，不修改
	if (uAction == m_uDefaultAction[nChannel])
	{
		return 0;
	}
	PTZ_DBG("uAction = 0x%08lX, dwIndex = 0x%02lX, dwActionType = 0x%06lX\n", uAction, dwIndex, dwActionType);
	char acKeyName[50];

	char acKeyValue[256];

	snprintf(acKeyName,sizeof(acKeyName), PTZRESET_ACTION_PREFIX "_%02d", nChannel);
	snprintf(acKeyValue, sizeof(acKeyValue), "0x%08x", uAction);
	PTZRESET_SETCONFIG(acKeyName, acKeyValue);
	m_uDefaultAction[nChannel] = uAction;
	ResetTimer(nChannel);
	if (m_pManager->PTZ_HasFunction(PTZ_FLAG_AUXHWGUARD, nChannel))
	{
		SetActionToHW(m_uDefaultAction[nChannel], m_pManager, nChannel);
	}

	return 0;
}

int CPtzAdjuster::Alarm(unsigned long dwActionType, unsigned long dwIndex, int nChannel)
{
	if (!ASSERT_CHANNEL(nChannel))
	{
		PTZ_ERROR("Channel = %d\n", nChannel);
		return IDVR_ERROR_PARAMETER;
	}
	m_uAlarmAction[nChannel] = MAKEACTION(dwActionType, nChannel);
	ResetTimer(nChannel);
	return 0;
}

//!\brief 通知云台退出报警联动状态
//!\param nChannel 云台通道号
//!\return 无
int CPtzAdjuster::ClearAlarm(int nChannel)
{
	if (!ASSERT_CHANNEL(nChannel))
	{
		PTZ_ERROR("Channel = %d\n", nChannel);
		return IDVR_ERROR_PARAMETER;
	}
	m_uAlarmAction[nChannel] = 0;
	ResetTimer(nChannel);
	return 0;
}

void CPtzAdjuster::GetAlarmAction(unsigned long *pdwActionType, unsigned long *pdwIndex, int nChannel)
{
	*pdwActionType = PTZ_ACTIONTYPE(m_uAlarmAction[nChannel]);
	*pdwIndex = ACTIONINDEX(m_uAlarmAction[nChannel]);
}

//!\brief 根据配置和状态使云台复位
//!\param nChannel 通道号
//!\return 0表示复位成功后, 其他表示错误码
int CPtzAdjuster::Reset(int nChannel)
{
	unsigned long dwActionType = 0;

	unsigned long dwIndex = 0;

	if (m_pManager->PTZ_HasFunction(PTZ_FLAG_AUXHWGUARD, nChannel))
	{
		return 0;
	}

	GetAlarmAction(&dwActionType, &dwIndex, nChannel);
	if (0 == dwActionType)
	{
		GetDefaultAction(&dwActionType, &dwIndex, nChannel);
	}

	int nPresetNum = 0;

	int nCruiseNum = 0;

	int nTrackNum = 0;

	int nAutoNum = 0;

	m_pManager->PTZ_GetTechnicalData(nChannel, &nPresetNum, &nCruiseNum, &nTrackNum, &nAutoNum, NULL);
	//int ret = m_aProtocolLib[nChannel]->objProtocol.PtzNormalCtrl(nChannel, m_aCPtzDev[nChannel]->GetAddress(), dwPtzCode, dwSpeed);
	switch (dwActionType)
	{
		case ACTION_PTZ_PRESET:
			if (dwIndex < (unsigned long) nPresetNum)
			{
				m_pManager->PTZ_NormalCtrl(TTY_PTZ_RIGHT, 1, nChannel);
				m_pManager->PTZ_NormalCtrl(TTY_PTZ_RIGHT_STOP, 0, nChannel);
				InternalSleep(50000);
				m_pManager->PTZ_PresetCtrl(TTY_PTZ_PRESETCALL, dwIndex, 0, 0, 0, nChannel);
			}
			//m_aProtocolLib[nChannel]->objProtocol.PtzPresetCtrl(nChannel, m_aCPtzDev[nChannel]->GetAddress(), 0, dwIndex, TTY_PTZ_PRESETCALL, 0, 0);
			break;
		case ACTION_PTZ_CRUISE:
			if (dwIndex < (unsigned long) nCruiseNum)
			{
				m_pManager->PTZ_NormalCtrl(TTY_PTZ_RIGHT, 1, nChannel);
				m_pManager->PTZ_NormalCtrl(TTY_PTZ_RIGHT_STOP, 0, nChannel);
				InternalSleep(50000);
				m_pManager->PTZ_AutoCtrl(TTY_PTZ_SETROUTE, 0, 0, dwIndex, 0, 0, 1, nChannel);
			}
			//m_aProtocolLib[nChannel]->objProtocol->PtzAutoCtrl(nChannel, m_aCPtzDev[nChannel]->GetAddress(), TTY_PTZ_SETROUTE, 0, 0, dwIndex, 0, 0, 1);
			break;
		case ACTION_PTZ_TRACK:
			if (dwIndex < (unsigned long) nTrackNum)
			{
				m_pManager->PTZ_NormalCtrl(TTY_PTZ_RIGHT, 1, nChannel);
				m_pManager->PTZ_NormalCtrl(TTY_PTZ_RIGHT_STOP, 0, nChannel);
				InternalSleep(50000);
				m_pManager->PTZ_AutoCtrl(TTY_PTZ_TRACKCALL, 0, 0, dwIndex, 0, 0, 1, nChannel);
				//m_aProtocolLib[nChannel]->objProtocol->PtzAutoCtrl(nChannel, m_aCPtzDev[nChannel]->GetAddress(), TTY_PTZ_TRACKCALL, 0, 0, dwIndex, 0, 0, 1);
			}
			//m_aProtocolLib[nChannel]->objProtocol->PtzAutoCtrl(nChannel, m_aCPtzDev[nChannel]->GetAddress(), TTY_PTZ_TRACKCALL, 0, 0, dwIndex, 0, 0, 1);
			break;
		case ACTION_PTZ_SCAN:
			if (dwIndex < (unsigned long) nAutoNum)
			{
				m_pManager->PTZ_NormalCtrl(TTY_PTZ_RIGHT, 1, nChannel);
				m_pManager->PTZ_NormalCtrl(TTY_PTZ_RIGHT_STOP, 0, nChannel);
				InternalSleep(50000);
				m_pManager->PTZ_AutoCtrl(TTY_PTZ_AUTO, 0, 0, dwIndex, 0, 0, 1, nChannel);
			}
			//m_aProtocolLib[nChannel]->objProtocol->PtzAutoCtrl(nChannel, m_aCPtzDev[nChannel]->GetAddress(), TTY_PTZ_AUTO, 0, 0, dwIndex, 0, 0, 1);
			break;
		default:
			break;
	}

	return 0;
}

//!\brief 重置计时器
//!\param nChannel 指定通道号
//!\return 无
void CPtzAdjuster::ResetTimer(int nChannel)
{
	m_lIdleTime[nChannel] = GetTimeSinceBoot();
}

//!\brief 检查指定的通道是否超时
//!\param nTime 当前时间
//!\param nChannel 通道号
//!\return 0表示未超时 1表示已超时
inline int CPtzAdjuster::IsTimeOut(int nTime, int nChannel)
{
	//如果没有启用守望或者周期为0则永不超时
	if ((!m_bEnable[nChannel]) || (0 == m_lResetCycle[nChannel]))
	{
		return 0;
	}

	return ((nTime - m_lIdleTime[nChannel]) >= m_lResetCycle[nChannel]);
}

int CPtzAdjuster::SetSpeed(int speed, int nChannel)
{
	CConfig *pConfig = CConfig::GetInstance();
	char szSectionName[20] = {0};

	if (!ASSERT_CHANNEL(nChannel))
	{
		PTZ_ERROR("Channel = %d\n", nChannel);
		return IDVR_ERROR_PARAMETER;
	}

	if((speed <= 0) || (speed > 10))
	{
		PTZ_ERROR("speed = %d\n", speed);
		return IDVR_ERROR_PARAMETER;
	}
	m_nSpeed[nChannel] = speed -1;
#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
	switch(nChannel)
	{
		case PTZ_MAIN:
		{
			m_nSpeedH[nChannel] = m_nSpeed[nChannel];
			m_nSpeedV[nChannel] = m_nSpeedH[nChannel];
			MOTO_SetSpeed(m_nSpeedH[nChannel], m_nSpeedV[nChannel],nChannel);
			snprintf(szSectionName,sizeof(szSectionName), "PTZ_Set_%d", nChannel);
			pConfig->SetValue(szSectionName, "MotorSpeedH", m_nSpeedH[nChannel]);
			pConfig->SetValue(szSectionName, "MotorSpeedV", m_nSpeedV[nChannel]);

			break;
		}
#if (DEVICE_SUPPORT_PTZ_NUM) >= 3
		case PTZ_LERF:
		case PTZ_RIGHT:
		{
			m_nSpeedH[PTZ_LERF] = m_nSpeed[PTZ_LERF];
			m_nSpeedV[PTZ_LERF] = m_nSpeedH[PTZ_LERF];
			MOTO_SetSpeed(m_nSpeedH[PTZ_LERF], m_nSpeedV[PTZ_LERF],PTZ_LERF);
			m_nSpeedH[PTZ_RIGHT] = m_nSpeed[PTZ_RIGHT];
			m_nSpeedV[PTZ_RIGHT] = m_nSpeedH[PTZ_RIGHT];
			MOTO_SetSpeed(m_nSpeedH[PTZ_RIGHT], m_nSpeedV[PTZ_RIGHT],PTZ_RIGHT);
			pConfig->SetValue(PTZ_MULTIDEV_CFG_SECTION, PTZ_LR_HORZ_MAXSPEED,  m_nSpeedH[nChannel]);
			pConfig->SetValue(PTZ_MULTIDEV_CFG_SECTION, PTZ_LR_VERT_MAXSPEED,  m_nSpeedV[nChannel]);
			pConfig->SaveFile();
			break;
		}
#endif
	}
#endif
	return 0;
}

int CPtzAdjuster::GetSpeed(int nChannel)
{

	if (!ASSERT_CHANNEL(nChannel))
	{
		PTZ_ERROR("Channel = %d\n", nChannel);
		return IDVR_ERROR_PARAMETER;
	}

	return m_nSpeed[nChannel] + 1;
}

#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
//!\brief	设置ptzV速度
//!\param	nChannel  通道号 	nSpeedV  速度
//!\return	-1为失败 
int CPtzAdjuster::SetSpeedV(int nSpeedV, int nChannel)
{
	CConfig *pConfig = CConfig::GetInstance();
	char szSectionName[20] = {0};

	if (!ASSERT_CHANNEL(nChannel))
	{
		PTZ_ERROR("Channel = %d\n", nChannel);
		return IDVR_ERROR_PARAMETER;
	}

 	if ((nSpeedV < 0) || (nSpeedV > 9))
 	{
		PTZ_ERROR("speed = %d\n", nSpeedV);
		return IDVR_ERROR_PARAMETER;
	}

	switch(nChannel)
	{
		case PTZ_MAIN:
		{
			m_nSpeedV[nChannel] = nSpeedV;
			MOTO_SetSpeed(m_nSpeedH[nChannel], m_nSpeedV[nChannel],nChannel);
			snprintf(szSectionName,sizeof(szSectionName), "PTZ_Set_%d", nChannel);
			pConfig->SetValue(szSectionName, "MotorSpeedV", m_nSpeedV[nChannel]);

			break;
		}
#if (DEVICE_SUPPORT_PTZ_NUM) >= 3

		case PTZ_LERF:
		case PTZ_RIGHT:
		{
			m_nSpeedV[PTZ_LERF] = nSpeedV;
			MOTO_SetSpeed(m_nSpeedH[PTZ_LERF], m_nSpeedV[PTZ_LERF],PTZ_LERF);
			m_nSpeedV[PTZ_RIGHT] = nSpeedV;
			MOTO_SetSpeed(m_nSpeedH[PTZ_RIGHT], m_nSpeedV[PTZ_RIGHT],PTZ_RIGHT);
			pConfig->SetValue(PTZ_MULTIDEV_CFG_SECTION, PTZ_LR_VERT_MAXSPEED,  m_nSpeedV[nChannel]);
			pConfig->SaveFile();
			break;
		}
#endif
	}
	
	return 0;
}

//!\brief	设置ptzH速度
//!\param	nChannel  通道号 	nSpeedH  速度
//!\return	-1为失败
int CPtzAdjuster::SetSpeedH(int nSpeedH, int nChannel)
{
	CConfig *pConfig = CConfig::GetInstance();
	char szSectionName[20] = {0};

	if (!ASSERT_CHANNEL(nChannel))
	{
		PTZ_ERROR("Channel = %d\n", nChannel);
		return IDVR_ERROR_PARAMETER;
	}

	if ((nSpeedH < 0) || (nSpeedH > 9))
	{
		PTZ_ERROR("speed = %d\n", nSpeedH);
		return IDVR_ERROR_PARAMETER;
	}
	switch(nChannel)
	{
		case PTZ_MAIN:
		{
			m_nSpeedH[nChannel] = nSpeedH;
			MOTO_SetSpeed(m_nSpeedH[nChannel], m_nSpeedH[nChannel],nChannel);
			snprintf(szSectionName,sizeof(szSectionName), "PTZ_Set_%d", nChannel);
			pConfig->SetValue(szSectionName, "MotorSpeedH", m_nSpeedH[nChannel]);

			break;
		}
#if (DEVICE_SUPPORT_PTZ_NUM) >= 3

		case PTZ_LERF:
		case PTZ_RIGHT:
		{
			m_nSpeedH[PTZ_LERF] = nSpeedH;
			MOTO_SetSpeed(m_nSpeedH[PTZ_LERF], m_nSpeedH[PTZ_LERF],PTZ_LERF);
			m_nSpeedH[PTZ_RIGHT] = nSpeedH;
			MOTO_SetSpeed(m_nSpeedH[PTZ_RIGHT], m_nSpeedH[PTZ_RIGHT],PTZ_RIGHT);
			pConfig->SetValue(PTZ_MULTIDEV_CFG_SECTION, PTZ_LR_HORZ_MAXSPEED,  m_nSpeedH[nChannel]);
			pConfig->SaveFile();
			break;
		}
#endif

	}
	
	return 0;

}

int CPtzAdjuster::EnablePTZCruise(int enable)
{
	int ret = 0;
	
	if(m_nCruiseEnable)
	{
		if(enable)
		{
			if(m_nBeginPresetNumForCruise)
			{
				switch(m_nBeginPresetActionForCruise)
				{
					case 1://set
						ret = m_pManager->PTZ_PresetCtrl(TTY_PTZ_PRESETSET, m_nBeginPresetNumForCruise - 1, 0, 0, 0, 0);
						break;
					case 2://call
						ret = m_pManager->PTZ_PresetCtrl(TTY_PTZ_PRESETCALL, m_nBeginPresetNumForCruise - 1, 0, 0, 0, 0);
						break;
					default:
						;
				}
			}
		}
		else
			{
				if(0 == m_nEndPresetNumForCruise)
				{
					ret = m_pManager->PTZ_NormalCtrl(TTY_PTZ_LEFT_STOP, 0, 0);
				}
				else
					{
						switch(m_nEndPresetActionForCruise)
						{
							case 1://set
								ret = m_pManager->PTZ_PresetCtrl(TTY_PTZ_PRESETSET, m_nEndPresetNumForCruise - 1, 0, 0, 0, 0);
								break;
							case 2://call
								ret = m_pManager->PTZ_PresetCtrl(TTY_PTZ_PRESETCALL, m_nEndPresetNumForCruise - 1, 0, 0, 0, 0);
								break;
							default:
								;
						}
					}
			}
	}

	return ret;
}

int CPtzAdjuster::EnablePTZWatch(int enable)
{
	int ret = 0;
	
	if(m_nWatchEnable)
	{
		if(enable)
		{
			if(m_nBeginPresetNumForWatch)
			{
				switch(m_nBeginPresetActionForWatch)
				{
					case 1://set
						ret = m_pManager->PTZ_PresetCtrl(TTY_PTZ_PRESETSET, m_nBeginPresetNumForWatch - 1, 0, 0, 0, 0);
						break;
					case 2://call
						ret = m_pManager->PTZ_PresetCtrl(TTY_PTZ_PRESETCALL, m_nBeginPresetNumForWatch - 1, 0, 0, 0, 0);
						break;
					default:
						;
				}
			}
		}
		else
			{
				if(0 == m_nEndPresetNumForWatch)
				{
					ret = m_pManager->PTZ_NormalCtrl(TTY_PTZ_LEFT_STOP, 0, 0);
				}
				else
					{
						switch(m_nEndPresetActionForWatch)
						{
							case 1://set
								ret = m_pManager->PTZ_PresetCtrl(TTY_PTZ_PRESETSET, m_nEndPresetNumForWatch - 1, 0, 0, 0, 0);
								break;
							case 2://call
								ret = m_pManager->PTZ_PresetCtrl(TTY_PTZ_PRESETCALL, m_nEndPresetNumForWatch - 1, 0, 0, 0, 0);
								break;
							default:
								;
						}
					}
			}
	}

	return ret;
}

int CPtzAdjuster::EnablePTZTrack(int enable)
{
	int ret = 0;
	
	if(m_nTrackEnable)
	{
		if(enable)
		{
			if(m_nBeginPresetNumForTrack)
			{
				switch(m_nBeginPresetActionForTrack)
				{
					case 1://set
						ret = m_pManager->PTZ_PresetCtrl(TTY_PTZ_PRESETSET, m_nBeginPresetNumForTrack - 1, 0, 0, 0, 0);
						break;
					case 2://call
						ret = m_pManager->PTZ_PresetCtrl(TTY_PTZ_PRESETCALL, m_nBeginPresetNumForTrack - 1, 0, 0, 0, 0);
						break;
					default:
						;
				}
			}
		}
		else
		{
			if(0 == m_nEndPresetNumForTrack)
			{
				ret = m_pManager->PTZ_NormalCtrl(TTY_PTZ_LEFT_STOP, 0, 0);
			}
			else
				{
					switch(m_nEndPresetActionForTrack)
					{
						case 1://set
							ret = m_pManager->PTZ_PresetCtrl(TTY_PTZ_PRESETSET, m_nEndPresetNumForTrack - 1, 0, 0, 0, 0);
							break;
						case 2://call
							ret = m_pManager->PTZ_PresetCtrl(TTY_PTZ_PRESETCALL, m_nEndPresetNumForTrack - 1, 0, 0, 0, 0);
							break;
						default:
							;
					}
				}
		}
	}

	return ret;
}
#endif

//!\brief 定时器检查线程
void *CPtzAdjuster::CheckTimer(void *arg)
{
    PRT_TID();

	BEGINRUN_THREAD;
	const int CHANNEL_COUNT = (int) arg;

	int bInited[CHANNEL_COUNT];

	int i = 0;

	for (i = 0; i < CHANNEL_COUNT; i++)
	{
		bInited[i] = 0;
	}

	while (1)
	{
		int i = 0;

#if 0
		int ret = 0;

		ret = m_pInstance->m_pManager->PTZ_GetType(0);
		PTZ_INFO("Get Type MainType=%d, SubType=%d\n", ret >> 16, ret & 0xFFFF);
		ret = m_pInstance->m_pManager->PTZ_GetTemperature(0, &i);
		PTZ_INFO("Get Temperature ret=%d, Temperate=%d.%02d\n", ret, i / 100, i % 100);
		int nHorizontalAngle;

		int nVerticalAngle;

		int nMultiple;

		int nOrientation;

		ret = m_pInstance->m_pManager->PTZ_GetCoordinate(0, &nHorizontalAngle, &nVerticalAngle, &nMultiple, &nOrientation);
		PTZ_INFO("GetCoordinate ret=%d, HorizontalAngle=%d.%02d, VerticalAngle=%d.%02d, Multiple=%d, nOrientation=%d\n", ret, nHorizontalAngle / 100,
				 nHorizontalAngle % 100, nVerticalAngle / 100, nVerticalAngle % 100, nMultiple, nOrientation);
		rsleep(2000000);
#else
		int nTime = GetTimeSinceBoot();

		for (i = 0; i < CHANNEL_COUNT; i++)
		{
			//开机执行第一次
			if (!bInited[i])
			{
				bInited[i] = 1;
				if (m_pInstance->m_bEnable[i])
				{
					//调用PTZManager执行动作,并将计时器归0
					m_pInstance->Reset(i);
				}
			}
			//如果是启用守望并且周期不为0，其计时器满
			if (m_pInstance->IsTimeOut(nTime, i))
			{
				//调用PTZManager执行动作,并将计时器归0
				m_pInstance->Reset(i);
			}
		}
		rsleep(1000000);
#endif
	}
	ENDRUN_THREAD;

	return NULL;
}

#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
int CPtzAdjuster::SetAdvanceCfg(int nChannel, char nCruiseEnable, 
									int nBeginPresetNumForCruise,
									int nBeginPresetActionForCruise,
									int nEndPresetNumForCruise,
									int nEndPresetActionForCruise,
									char nWatchEnable,
									int nBeginPresetNumForWatch,
									int nBeginPresetActionForWatch,
									int nEndPresetNumForWatch,
									int nEndPresetActionForWatch)
{
	// TODO: Add your specialized code here.
	int ret = 0;

	if (!ASSERT_CHANNEL(nChannel))
	{
		PTZ_ERROR("Channel = %d\n", nChannel);
		return IDVR_ERROR_PARAMETER;
	}

	if((nCruiseEnable == m_nCruiseEnable) && (nBeginPresetNumForCruise == m_nBeginPresetNumForCruise) && (nBeginPresetActionForCruise == m_nBeginPresetActionForCruise)
		&& (nEndPresetNumForCruise == m_nEndPresetNumForCruise) && (nEndPresetActionForCruise == m_nEndPresetActionForCruise)
		&& (nWatchEnable == m_nWatchEnable) && (nBeginPresetNumForWatch == m_nBeginPresetNumForWatch) && (nBeginPresetActionForWatch == m_nBeginPresetActionForWatch)
		&& (nEndPresetNumForWatch == m_nEndPresetNumForWatch) && (nEndPresetActionForWatch == m_nEndPresetActionForWatch))
	{
		return 0;
	}

	m_nCruiseEnable = nCruiseEnable;
	m_nBeginPresetNumForCruise = nBeginPresetNumForCruise;
	m_nBeginPresetActionForCruise = nBeginPresetActionForCruise;
	m_nEndPresetNumForCruise = nEndPresetNumForCruise;
	m_nEndPresetActionForCruise = nEndPresetActionForCruise;
	m_nWatchEnable = nWatchEnable;
	m_nBeginPresetNumForWatch = nBeginPresetNumForWatch;
	m_nBeginPresetActionForWatch = nBeginPresetActionForWatch;
	m_nEndPresetNumForWatch = nEndPresetNumForWatch;
	m_nEndPresetActionForWatch = nEndPresetActionForWatch;

	CConfig *pConfig = CConfig::GetInstance();

	if(pConfig)
	{
		pConfig->SetValue(PTZRESET_ADVANCE_CFG, "CruiseEnable", m_nCruiseEnable);
		pConfig->SetValue(PTZRESET_ADVANCE_CFG, "BeginPresetNumForCruise", m_nBeginPresetNumForCruise);
		pConfig->SetValue(PTZRESET_ADVANCE_CFG, "BeginPresetActionForCruise", m_nBeginPresetActionForCruise);
		pConfig->SetValue(PTZRESET_ADVANCE_CFG, "EndPresetNumForCruise", m_nEndPresetNumForCruise);
		pConfig->SetValue(PTZRESET_ADVANCE_CFG, "EndPresetActionForCruise", m_nEndPresetActionForCruise);
		pConfig->SetValue(PTZRESET_ADVANCE_CFG, "WatchEnable", m_nWatchEnable);
		pConfig->SetValue(PTZRESET_ADVANCE_CFG, "BeginPresetNumForWatch", m_nBeginPresetNumForWatch);
		pConfig->SetValue(PTZRESET_ADVANCE_CFG, "BeginPresetActionForWatch", m_nBeginPresetActionForWatch);
		pConfig->SetValue(PTZRESET_ADVANCE_CFG, "EndPresetNumForWatch", m_nEndPresetNumForWatch);
		pConfig->SetValue(PTZRESET_ADVANCE_CFG, "EndPresetActionForWatch", m_nEndPresetActionForWatch);
	}
	
	return ret;
}

int CPtzAdjuster::SetAdvanceCfgV1(int nChannel, char nCruiseEnable, 
								int nBeginPresetNumForCruise,
								int nBeginPresetActionForCruise,
								int nEndPresetNumForCruise,
								int nEndPresetActionForCruise,
								char nWatchEnable,
								int nBeginPresetNumForWatch,
								int nBeginPresetActionForWatch,
								int nEndPresetNumForWatch,
								int nEndPresetActionForWatch,
								char nTrackEnable,
								int nBeginPresetNumForTrack,
								int nBeginPresetActionForTrack,
								int nEndPresetNumForTrack,
								int nEndPresetActionForTrack)
{
	// TODO: Add your specialized code here.
	int ret = 0;

	if (!ASSERT_CHANNEL(nChannel))
	{
		PTZ_ERROR("Channel = %d\n", nChannel);
		return IDVR_ERROR_PARAMETER;
	}

	if((nCruiseEnable == m_nCruiseEnable) && (nBeginPresetNumForCruise == m_nBeginPresetNumForCruise) && (nBeginPresetActionForCruise == m_nBeginPresetActionForCruise)
		&& (nEndPresetNumForCruise == m_nEndPresetNumForCruise) && (nEndPresetActionForCruise == m_nEndPresetActionForCruise)
		&& (nWatchEnable == m_nWatchEnable) && (nBeginPresetNumForWatch == m_nBeginPresetNumForWatch) && (nBeginPresetActionForWatch == m_nBeginPresetActionForWatch)
		&& (nEndPresetNumForWatch == m_nEndPresetNumForWatch) && (nEndPresetActionForWatch == m_nEndPresetActionForWatch)
		&& (nTrackEnable == m_nTrackEnable) && (nBeginPresetNumForTrack == m_nBeginPresetNumForTrack) && (nBeginPresetActionForTrack == m_nBeginPresetActionForTrack)
		&& (nEndPresetNumForTrack == m_nEndPresetNumForTrack) && (nEndPresetActionForTrack == m_nEndPresetActionForTrack))
	{
		return 0;
	}

	m_nCruiseEnable = nCruiseEnable;
	m_nBeginPresetNumForCruise = nBeginPresetNumForCruise;
	m_nBeginPresetActionForCruise = nBeginPresetActionForCruise;
	m_nEndPresetNumForCruise = nEndPresetNumForCruise;
	m_nEndPresetActionForCruise = nEndPresetActionForCruise;
	m_nWatchEnable = nWatchEnable;
	m_nBeginPresetNumForWatch = nBeginPresetNumForWatch;
	m_nBeginPresetActionForWatch = nBeginPresetActionForWatch;
	m_nEndPresetNumForWatch = nEndPresetNumForWatch;
	m_nEndPresetActionForWatch = nEndPresetActionForWatch;
	m_nTrackEnable = nTrackEnable;
	m_nBeginPresetNumForTrack = nBeginPresetNumForTrack;
	m_nBeginPresetActionForTrack = nBeginPresetActionForTrack;
	m_nEndPresetNumForTrack = nEndPresetNumForTrack;
	m_nEndPresetActionForTrack = nEndPresetActionForTrack;

	CConfig *pConfig = CConfig::GetInstance();

	if(pConfig)
	{
		pConfig->SetValue(PTZRESET_ADVANCE_CFG, "CruiseEnable", m_nCruiseEnable);
		pConfig->SetValue(PTZRESET_ADVANCE_CFG, "BeginPresetNumForCruise", m_nBeginPresetNumForCruise);
		pConfig->SetValue(PTZRESET_ADVANCE_CFG, "BeginPresetActionForCruise", m_nBeginPresetActionForCruise);
		pConfig->SetValue(PTZRESET_ADVANCE_CFG, "EndPresetNumForCruise", m_nEndPresetNumForCruise);
		pConfig->SetValue(PTZRESET_ADVANCE_CFG, "EndPresetActionForCruise", m_nEndPresetActionForCruise);
		pConfig->SetValue(PTZRESET_ADVANCE_CFG, "WatchEnable", m_nWatchEnable);
		pConfig->SetValue(PTZRESET_ADVANCE_CFG, "BeginPresetNumForWatch", m_nBeginPresetNumForWatch);
		pConfig->SetValue(PTZRESET_ADVANCE_CFG, "BeginPresetActionForWatch", m_nBeginPresetActionForWatch);
		pConfig->SetValue(PTZRESET_ADVANCE_CFG, "EndPresetNumForWatch", m_nEndPresetNumForWatch);
		pConfig->SetValue(PTZRESET_ADVANCE_CFG, "EndPresetActionForWatch", m_nEndPresetActionForWatch);
		pConfig->SetValue(PTZRESET_ADVANCE_CFG, "TrackEnable", m_nTrackEnable);
		pConfig->SetValue(PTZRESET_ADVANCE_CFG, "BeginPresetNumForTrack", m_nBeginPresetNumForTrack);
		pConfig->SetValue(PTZRESET_ADVANCE_CFG, "BeginPresetActionForTrack", m_nBeginPresetActionForTrack);
		pConfig->SetValue(PTZRESET_ADVANCE_CFG, "EndPresetNumForTrack", m_nEndPresetNumForTrack);
		pConfig->SetValue(PTZRESET_ADVANCE_CFG, "EndPresetActionForTrack", m_nEndPresetActionForTrack);
	}
	
	return ret;
}

int CPtzAdjuster::GetAdvanceCfg(int nChannel, char *nCruiseEnable, 
									int *nBeginPresetNumForCruise,
									int *nBeginPresetActionForCruise,
									int *nEndPresetNumForCruise,
									int *nEndPresetActionForCruise,
									char *nWatchEnable,
									int *nBeginPresetNumForWatch,
									int *nBeginPresetActionForWatch,
									int *nEndPresetNumForWatch,
									int *nEndPresetActionForWatch)
{
	if (!ASSERT_CHANNEL(nChannel))
	{
		PTZ_ERROR("Channel = %d\n", nChannel);
		return IDVR_ERROR_PARAMETER;
	}

	*nCruiseEnable = m_nCruiseEnable;
	*nBeginPresetNumForCruise = m_nBeginPresetNumForCruise;
	*nBeginPresetActionForCruise = m_nBeginPresetActionForCruise;
	*nEndPresetNumForCruise = m_nEndPresetNumForCruise;
	*nEndPresetActionForCruise = m_nEndPresetActionForCruise;
	*nWatchEnable = m_nWatchEnable;
	*nBeginPresetNumForWatch = m_nBeginPresetNumForWatch;
	*nBeginPresetActionForWatch = m_nBeginPresetActionForWatch;
	*nEndPresetNumForWatch = m_nEndPresetNumForWatch;
	*nEndPresetActionForWatch = m_nEndPresetActionForWatch;

	return 0;
}

int CPtzAdjuster::GetAdvanceCfgV1(int nChannel, char *nCruiseEnable, 
									int *nBeginPresetNumForCruise,
									int *nBeginPresetActionForCruise,
									int *nEndPresetNumForCruise,
									int *nEndPresetActionForCruise,
									char *nWatchEnable,
									int *nBeginPresetNumForWatch,
									int *nBeginPresetActionForWatch,
									int *nEndPresetNumForWatch,
									int *nEndPresetActionForWatch,
									char *nTrackEnable,
									int *nBeginPresetNumForTrack,
									int *nBeginPresetActionForTrack,
									int *nEndPresetNumForTrack,
									int *nEndPresetActionForTrack)
{
	if (!ASSERT_CHANNEL(nChannel))
	{
		PTZ_ERROR("Channel = %d\n", nChannel);
		return IDVR_ERROR_PARAMETER;
	}

	*nCruiseEnable = m_nCruiseEnable;
	*nBeginPresetNumForCruise = m_nBeginPresetNumForCruise;
	*nBeginPresetActionForCruise = m_nBeginPresetActionForCruise;
	*nEndPresetNumForCruise = m_nEndPresetNumForCruise;
	*nEndPresetActionForCruise = m_nEndPresetActionForCruise;
	*nWatchEnable = m_nWatchEnable;
	*nBeginPresetNumForWatch = m_nBeginPresetNumForWatch;
	*nBeginPresetActionForWatch = m_nBeginPresetActionForWatch;
	*nEndPresetNumForWatch = m_nEndPresetNumForWatch;
	*nEndPresetActionForWatch = m_nEndPresetActionForWatch;
	*nTrackEnable = m_nTrackEnable;
	*nBeginPresetNumForTrack = m_nBeginPresetNumForTrack;
	*nBeginPresetActionForTrack = m_nBeginPresetActionForTrack;
	*nEndPresetNumForTrack = m_nEndPresetNumForTrack;
	*nEndPresetActionForTrack = m_nEndPresetActionForTrack;

	return 0;
}

int CPtzAdjuster::ReturnToOriginalSpeed(int nChannel)
{
	CConfig *pConfig = CConfig::GetInstance();
	char szSectionName[20];

	snprintf(szSectionName,sizeof(szSectionName), "PTZ_Set_%d", nChannel);
	int speedH = pConfig->GetValue(szSectionName, "MotorSpeedH", 5);
	int speedV = pConfig->GetValue(szSectionName, "MotorSpeedV", 5);

 	if ((speedH <= 0) || (speedH > 10) || (speedV <= 0) || (speedV > 10))
 	{
		PTZ_ERROR("speedH = %d, speedV = %d\n", speedH, speedV);
		return IDVR_ERROR_PARAMETER;
	}

	if (0 != m_bSupportMotor)
	{
		PTZ_DBG("speedH = %d, speedV = %d\n", speedH, speedV);
		MOTO_SetSpeed(speedH, speedV,nChannel);
	}
	return 0;
}

int CPtzAdjuster::SetTrackSpeed(int speedH, int speedV, int nChannel)
{
	if (!ASSERT_CHANNEL(nChannel))
	{
		PTZ_ERROR("Channel = %d\n", nChannel);
		return IDVR_ERROR_PARAMETER;
	}

 	if ((speedH < 0) || (speedH > 9) || (speedV < 0) || (speedV > 9))
 	{
		PTZ_ERROR("speedH = %d, speedV = %d\n", speedH, speedV);
		return IDVR_ERROR_PARAMETER;
	}

	if (0 != m_bSupportMotor)
	{
		MOTO_SetSpeed(speedH, speedV,nChannel);
	}
	
	return 0;
}
#endif

void InternalSleep(unsigned int utime)
{
#if defined(USE_ICAMERA)
	rsleep(utime);
#endif
}

void SetActionToHW(unsigned int uAction, CPtzManager * pManager, int nChannel)
{
	int nAction = PTZ_ACTIONTYPE(uAction) - ACTION_PTZ_PRESET;

	int nIndex = ((!nAction) ? ACTIONINDEX(uAction) : 0);

	pManager->PTZ_PresetCtrl(TTY_PTZ_GUARDSETACTION, nIndex, nAction, 0, 0, nChannel);
}

void SetCycleToHW(time_t lCycle, CPtzManager * pManager, int nChannel)
{
	pManager->PTZ_PresetCtrl(TTY_PTZ_GUARDSETCYCLE, 0, 0, 0, lCycle, nChannel);
}

void SetEnableToHW(int bEnable, CPtzManager * pManager, int nChannel)
{
	pManager->PTZ_AutoCtrl(TTY_PTZ_GUARD, 0, 0, 0, 0, 0, (bEnable ? 1 : 0), nChannel);
}

#undef PTZ_ACTIONTYPE
