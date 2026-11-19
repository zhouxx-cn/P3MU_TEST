#include <stdlib.h>

#include "CAwakeSchedule.h"
#include "idvr_errno.h"
#include "PAR/CConfig.h"
#include "global_macro.h"
#include "macro_pthread.h"
#include "mysystem.h"
#include "debug.h"
#include "MPU/MPU_global.h"
#include "motoLib.h"
#include "PUB/timex.h"

// 配置区名
static const char *SleepCfgSection = "SleepModecfg";
// 缺省计划配置
static const char *Scheduletime = "000000000000000000000000000000000000000000000000";

enum AwakeStatus{
    Enum_SleepMode = 0,
    Enum_AwakeMode = 1
};

// 全局实例
CAwakeSchedule *CAwakeSchedule::theCAwakeSchedule = NULL;

//!\brief 获取配置
//!\return  成功为0，失败为-1
int CAwakeSchedule::GetValue(const char *key, int value)
{
	CConfig *theCConfig = CConfig::GetInstance();

	if (NULL != theCConfig)
	{
		value = theCConfig->GetValue(SleepCfgSection, key, value);
	}
	return value;
}

char * CAwakeSchedule::GetValue_Str(const char *key, char *value_str, int val_max_len)
{
	char *val_str = NULL;
	
	CConfig *theCConfig = CConfig::GetInstance();
	if (NULL != theCConfig)
	{
		val_str = theCConfig->GetValue(SleepCfgSection, key, Scheduletime, value_str, val_max_len);
	}
	return val_str;
}

int CAwakeSchedule::LoadParam()
{
    int i = 0;
    int j = 0;
	char key[100] = {0};
	char value[256] = {0};
    tagMotionSchedule *cfg = NULL;

	m_bSleepEnable = GetValue("SleepEnable", 0);
    m_bScheduleCfgEnable = GetValue("ScheduleCfgEnable", 0);

	//导入计划
	cfg = &m_ScheduleCfg;
	cfg->bytState = GetValue("ScheduleCfgState", 0);

	//ScheduleCfgEnable[ch]=[启用]
	//ScheduleCfg[ch]_[星期]=[48]--[启用][星期][半小时]
	cfg->bytReserve[0] = 0;
	cfg->bytReserve[1] = 0;
	cfg->bytReserve[2] = 0;
	for (i = 0; i < 7; i++)
	{
		snprintf(key, sizeof(key), "ScheduleCfg_%01d", i);
        
		GetValue_Str(key, value, 256);
		for (j = 0; j < 48; j++)
		{
			cfg->dwTimeSeg[i][j] = value[j] - '0';
		}
	}
    return 0;
}

//!\brief 读取配置信息
//!\return  成功为0，失败为-1
int CAwakeSchedule::Init()
{
    /***********************************
    设备休眠唤醒暂不用报警联动
    m_pActionAlarm = new CActionAlarm;
	if (m_pActionAlarm)
	{
		m_pActionAlarm->Init(0, (char *)SleepCfgSection, "SleepAction", 0, 0);
		m_pActionAlarm->EnableAction(ACTION_GUI, 1);
	}
	***********************************/

    LoadParam();

	return 0;
}

//!\brief 设置配置
//!\praram char *key 关键字
//!\praramint value 值
//!\return  成功为0，失败为-1
int CAwakeSchedule::SetValue(const char *key, int value)
{
	int ret = -1;

	CConfig *theCConfig = CConfig::GetInstance();
	if (NULL != theCConfig)
	{
		ret = theCConfig->SetValue(SleepCfgSection, key, value);
	}

	return ret;
}

//!\brief 设置配置
//!\praram char *key 关键字
//!\praramint value 值
//!\return  成功为0，失败为-1
int CAwakeSchedule::SetValue_Str(const char *key, const char *val_str)
{
	int ret = -1;

	CConfig *theCConfig = CConfig::GetInstance();
	if (NULL != theCConfig)
	{
		ret = theCConfig->SetValue(SleepCfgSection, key, val_str);
	}

	return ret;
}

//!\brief 禁用/启用立即睡眠
//!\return  成功为0，失败为-1
int CAwakeSchedule::SleepEnable(int bEnable)
{
    // 立即睡眠与计划睡眠互斥，关闭睡眠计划
    if (1 == m_bScheduleCfgEnable)
    {
        m_bScheduleCfgEnable = 0;
        SetValue("ScheduleCfgEnable",0);
    }

    // APP传0表示立即睡眠，1表示立即唤醒
    m_bSleepEnable = (bEnable == 0)?1:0;

    SetValue("SleepEnable",m_bSleepEnable);

    CConfig *theCConfig = CConfig::GetInstance();
    theCConfig->SaveFileReal();

	return 0;
}

int CAwakeSchedule::GetSleepEnable(int *bEnable)
{
    // 0 表示立即睡眠关 1表示立即睡眠开
    *bEnable = m_bSleepEnable;
    return 0;
}

int CAwakeSchedule::ScheduleCfgEnable(int bEnable)
{
    if (m_bScheduleCfgEnable == bEnable)
    {
        return 0;
    }
    
    m_bScheduleCfgEnable = bEnable;
    SetValue("ScheduleCfgEnable",bEnable);

    // 立即睡眠与计划睡眠互斥，关闭立即睡眠
    if (1 == m_bSleepEnable)
    {
        m_bSleepEnable = 0;
        SetValue("SleepEnable",0);
    }

    CConfig *theCConfig = CConfig::GetInstance();
    theCConfig->SaveFileReal();

    return 0;
}

int CAwakeSchedule::GetScheduleCfgEnable(int *bEnable)
{
    *bEnable = m_bScheduleCfgEnable;
    return 0;
}

//!\brief 获取设备当前睡眠状态
//!\praram 
//!\return  0为休眠，1为唤醒
int CAwakeSchedule::GetDevSleepStatus()
{
	return (m_IsDevSleep == 1)?0:1;
}

//!\brief 构造函数
CAwakeSchedule::CAwakeSchedule()
{
    m_bInited = 0;
    m_bWorkTrheadRun = 0;
    m_bAwakeModuleReadyFlag = 0;
	
	m_bSleepEnable = 0;
    m_bScheduleCfgEnable = 0;
    m_IsDevSleep = 0;

    memset(&m_ScheduleCfg, 0, sizeof(tagMotionSchedule));

    m_pActionAlarm = NULL;	
	m_CallBack = NULL;
}

//!\brief 获取句柄
//!\return  成功为为句柄，失败为NULL
CAwakeSchedule *CAwakeSchedule::GetInstance()
{
	// TODO: Add your specialized code here.
	if (NULL == theCAwakeSchedule)
	{
#ifndef TEST_MEM_INFO
		theCAwakeSchedule = new CAwakeSchedule();
#else
		theCAwakeSchedule = CommonKit::TCreateObject<CAwakeSchedule>::CreateObject(__FILE__, __FUNCTION__, __LINE__);
#endif

		if (NULL != theCAwakeSchedule)
		{
			theCAwakeSchedule->Init();

			theCAwakeSchedule->StartAwakeScheduleTask();
		}
	}
	return (CAwakeSchedule *) theCAwakeSchedule;
}


//!\brief 销毁实例
//!\praram CMaintenance *handle 句柄
//!\return  成功为0，失败为-1
int CAwakeSchedule::ReleaseInstance()
{
	if (NULL != theCAwakeSchedule)
	{
		if (theCAwakeSchedule->m_pActionAlarm)
		{
			delete theCAwakeSchedule->m_pActionAlarm;
			theCAwakeSchedule->m_pActionAlarm = NULL;
		}
		theCAwakeSchedule->m_bWorkTrheadRun = 0;
#ifndef TEST_MEM_INFO
		delete theCAwakeSchedule;
#else
		CommonKit::TDestroyObject<CAwakeSchedule>::DestroyObject(theCAwakeSchedule);
#endif

		theCAwakeSchedule = NULL;
	}
	return (int) 0;
}

//!\brief 设置睡眠计划时间
//!\return  成功为0，失败为-1
int CAwakeSchedule::SetAwakeScheduleTime(const tagMotionSchedule * cfg)
{
    char value[255] = {0};
    char key_str[30] = {0};
    int  IsAllDay = 1;

    int i, j;

    if (NULL == cfg)
    {
        CSYS_ERR("cfg == NULL\n");
        return -1;
    }
    if (cfg->bytState > 2)
    {
        CSYS_ERR("cfg->bytState=%d\n",cfg->bytState);
        return -1;
    }

    for (i = 0; i < 7; i++)
    {
        for (j = 0; j < 48; j++)
        {
            value[j] = cfg->dwTimeSeg[i][j] + '0';
            if (cfg->dwTimeSeg[i][j] == 0) {
                IsAllDay = 0;
            }
        }
        value[j] = 0;
        snprintf(key_str, sizeof(key_str), "ScheduleCfg_%01d", i);

        SetValue_Str(key_str, value);
        CSYS_DBG("[CAwakeSchedule:%s]IsAllDay[%d],cfg->bytState=%d,Write:%s\n", __FUNCTION__, IsAllDay, cfg->bytState, value);
    }

	//更新计划
    memcpy(&m_ScheduleCfg, cfg, sizeof (tagMotionSchedule));

    //bytState变量需要设备端控制，在全天休眠时，需要强制置0
    if (IsAllDay == 1)
    {
        SetValue("ScheduleCfgState", 0);
        m_ScheduleCfg.bytState = 0;
    } 
    else 
    {
        SetValue("ScheduleCfgState", 1);
        m_ScheduleCfg.bytState = 1;
    }

    CConfig *theCConfig = CConfig::GetInstance();
    theCConfig->SaveFileReal();
    return 0;
}

//!\brief 获取睡眠计划时间
//!\return 成功为0，失败为-1
int CAwakeSchedule::GetAwakeScheduleTime(tagMotionSchedule * cfg)
{
	if (NULL == cfg)
	{
		return -1;
	}

    memcpy(cfg, &m_ScheduleCfg, sizeof(tagMotionSchedule));
	return 0;
}

int CAwakeSchedule::GetAwakeScheduleEndTime(int *bValid, int *nday, int *nhour)
{
	if (NULL == bValid || NULL == nday || NULL == nhour)
	{
		return -1;
	}

    if (0 == m_bScheduleCfgEnable)
    {
        *bValid = 0;
        return 0;
    }

    int i = 0;
    int day = 0;
    int seg = 0;
    int result = 0;
    time_t curr_time = 0;
    struct tm curr_tm = {0};

    curr_time = time(NULL);
    localtime_r(&curr_time, &curr_tm);
    day = curr_tm.tm_wday;
    seg = (curr_tm.tm_hour << 1) + (curr_tm.tm_min >= 30) + 1;

    for (i=0; i<7; i++)
    {
        for (; seg<48; seg++)
        {
            if (!(m_ScheduleCfg.dwTimeSeg[day][seg]))
            {
                result = 1;
                break;
            }
        }
        if (result == 1)
        {
            break;
        }
        seg = 0;
        day += 1;
        if (day == 7)
        {
            day = 0;
        }
    }

    if (day == 0)
    {
        day = 7;
    }

    if (result == 1)
    {
        *bValid = 1;
        *nhour = seg/2;
        *nday = day-1;
    }
    return 0;
}

void *AwakeScheduleWorkThread(void *org)
{
    PRT_TID();
	BEGINRUN_THREAD;

	CAwakeSchedule *handle = (CAwakeSchedule *) org;

	handle->work_thread(org);

	ENDRUN_THREAD;
	return NULL;
}

int CAwakeSchedule::StartAwakeScheduleTask()
{
	// TODO: Add your specialized code here.
	int status;
	pthread_attr_t new_attr;
	pthread_t thread_t;

	pthread_attr_init(&new_attr);
	pthread_attr_setdetachstate(&new_attr, PTHREAD_CREATE_DETACHED);

	CSYS_INF("创建工作线程\n");
	if (m_bWorkTrheadRun == 0)
	{
		m_bWorkTrheadRun = 1;
		status = pthread_create(&thread_t, &new_attr, AwakeScheduleWorkThread, this);

		if (status != 0)
		{
			CSYS_ERR("创建工作线程失败\n");
			return IDVR_STR_ERROR_DISK_THREAD_FAILED;
		}
	}
	CSYS_INF("创建工作线程成功\n");
	return 0;
}

//!\brief 设置回调函数
//!\praram void (*callback)(int AwakeMode) 回调函数指针
//!\return  成功为0，失败为-1
int CAwakeSchedule::SetAwakeScheduleCallBack(void (*callback) (int AwakeMode))
{
	m_CallBack = callback;
	return 0;
}

void CAwakeSchedule::ChangeMode(int mode)
{
    if (m_IsDevSleep == 1 && Enum_SleepMode == mode)
    {
        return;
    }

    if (m_IsDevSleep == 0 && Enum_AwakeMode == mode)
    {
        return;
    }

    if (m_CallBack == NULL)
    {
        return;
    }

    m_CallBack(mode);

    if (Enum_SleepMode == mode) 
    {
        m_IsDevSleep = 1;
    }
    else 
    {
        m_IsDevSleep = 0;
    }
}

int CAwakeSchedule::CheckSchedule()
{
    int day = 0;
    int seg = 0;
    time_t curr_time = 0;
    struct tm curr_tm = {0};

    curr_time = time(NULL);
    localtime_r(&curr_time, &curr_tm);
    day = curr_tm.tm_wday;
    seg = (curr_tm.tm_hour << 1) + (curr_tm.tm_min >= 30);
    
    CSYS_INF("wday=%d hour=%d min=%d---\n", day, curr_tm.tm_hour, curr_tm.tm_min);
    
	if (m_ScheduleCfg.dwTimeSeg[day][seg])
	{
		return 1;
	}

	return 0;
}

void *CAwakeSchedule::work_thread(void *org)
{
	int iState = 0;
	int iCount = 0;
    
	//此线程可在设备停下来之后，再开启
	while (1)
	{
		MOTO_GetState(&iState);
		if(0 == iState)
		{
			CSYS_INF("the ptz has stopped\n");
			break;
		}
		else
		{
			iCount++;
			if(iCount >= 20)
			{
				break;
			}
		}
		sleep(1);
	}

	while (1)
	{
		if (!m_bWorkTrheadRun) /* 如果工作线程没有运行 */
		{
            break;
        }
        sleep(1);    /* 延时1秒 */

        //等待系统时间同步完成
        if (0 == get_time_sync_flag())
        {
            continue;
        }

        //判断休眠状态是否已确定
        if (0 == m_bAwakeModuleReadyFlag && m_bInited)
        {
            //标志 休眠模块准备就绪
		    m_bAwakeModuleReadyFlag = 1;
        }

		if (!m_bInited && m_CallBack && !access("/tmp/.otherProtocolStarted", F_OK))
		{
			m_bInited = 1;
		}

		if (0 == m_bInited)
		{
			continue;
		}

        if (m_bSleepEnable) //立即睡眠是否打开
        {
            ChangeMode(Enum_SleepMode);
            continue;
        }

		if (0 == m_bScheduleCfgEnable) //睡眠计划是否生效
		{
            ChangeMode(Enum_AwakeMode);
            continue;
		}

        //当前时间在计划时间之内--应该是睡眠状态
        if (1 == CheckSchedule())
        {
            ChangeMode(Enum_SleepMode);
        }
        else
        {
            ChangeMode(Enum_AwakeMode);
        }
	}
	return NULL;
}

