#include <pthread.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include "CConfig.h"
#include "global_macro.h"
#include "macro_pthread.h"
#include "pthreadmacro.h"
#include "tagTime.h"
//#include <iostream.h>
#include <errno.h>
#include <string.h>
#include "mysystem.h"
#include "debug.h"
#include "dst.h"

//#include "newhie/HieServer_Cfg.h"
// #include "sys_banned.h"
#include "global_gather.h"
#ifndef __MANAGER_TIME_EX_H__
#define __MANAGER_TIME_EX_H__
typedef struct tagZW_DSTWEEKMODE_TIMEPOINT_CFG_EX
{
	int bValid;																		/*!< 本结构体是否有效，0表示无效  */

	unsigned char bytStartMonth;								// 月份  (范围:0-11)
	unsigned char bytWhichWeek;								// 第几周(范围:0-6,6表示最后一周)
	unsigned char bytWeekDay;								// 星期几(范围:0-6)

	unsigned char bytHour;									// 小时[范围:0-23]							
	unsigned char bytMinute;									// 分钟[范围:0-59]
	unsigned char bytSecond;									// 秒[范围:0-59]

	unsigned char bytReserved[2];
}ZW_DSTWEEKMODE_TIMEPOINT_CFG_EX;

typedef enum 
{
	eDSTTimeFormat_Week_EX,
	eDSTTimeFormat_Date_EX,
}eDSTTimeFormat_EX;

typedef struct
{

	tagTime start;

	tagTime end;

	int byDSTBias;
	eDSTTimeFormat_EX   dwMode;							// 0,周模式，1,日模式
	ZW_DSTWEEKMODE_TIMEPOINT_CFG_EX struWeekModeStart;		// 周模式开始时间，dwMode为eDSTTimeFormat_Week时间有效
	ZW_DSTWEEKMODE_TIMEPOINT_CFG_EX struWeekModeStop;		// 周模式结束时间，dwMode为eDSTTimeFormat_Week时间有效

} DVR_DST_EX, *LPDVR_DST_EX;
#endif


static DVR_DST_EX m_Dst;

static int m_DSTStatus = eDstForbidChangeTime;

static int m_bEnable = 0;

static pthread_mutex_t m_mutex = PTHREAD_MUTEX_INITIALIZER;

static CallBack user_func = NULL;

static tagTime m_StartTime;

static tagTime m_EndTime;


static int m_bReboot = 1;

static int update_dst_time(time_t time);

static time_t update_dst(time_t now);

static void *work_thread(void *org);

static time_t weekday_to_day(int year, int mon, int week, int weekday, int hour, int min, int sec);

static void user_func_call(time_t time);
static time_t weekday_to_day(int year,int mon,int week,int weekday,int hour,int min,int sec);

int DST_Init()
{
	// TODO: Add your specialized code here.
	int status = 0;
	int tmp = 0;
	
	char str[100] = {0};

	time_t now;

	pthread_attr_t new_attr;

	pthread_t thread_t;
	pthread_attr_init (&new_attr);
	pthread_attr_setdetachstate (&new_attr, PTHREAD_CREATE_DETACHED);
	int mon=0,day=0,week=0;
	int hour=0,min=0,sec=0;
	m_bEnable = CConfig::GetInstance()->GetValue(SECTION,BENABLE,0);

#if 1

	//因为夏令时时间不写入RTC里面，所以一启动脚本会将rtc时间写到linux系统时间，所以初始化的时候系统时间都不是夏令时时间
	m_DSTStatus = eDSTAllowChangeTime;

#else

	int off_dias = 0;

    CConfig *pCfg = CConfig::NewInstance();
    if (0 == pCfg->LoadFileRO(STATUS_FILE))
    {
        m_DSTStatus = pCfg->GetValue(SECTION, STATUS,  0);
        off_dias = pCfg->GetValue(SECTION, DSTBIAS, 0);
    }
    CConfig::ReleaseInstance(pCfg);

#endif

	m_Dst.byDSTBias = CConfig::GetInstance()->GetValue(SECTION,TIME,3600);
	tmp = CConfig::GetInstance()->GetValue(SECTION,MODE,1);
	m_Dst.dwMode = tmp?eDSTTimeFormat_Date_EX:eDSTTimeFormat_Week_EX;

	memset((void *) str, 0, sizeof (str));
	CConfig::GetInstance()->GetValue(SECTION, BEGIN, "3:2:0-2:0:0", str, sizeof (str));
	sscanf(str, "%d:%d:%d-%d:%d:%d", &m_Dst.start.year, &m_Dst.start.mon, &m_Dst.start.day, &m_Dst.start.hour, &m_Dst.start.min, &m_Dst.start.sec);

	memset((void *) str, 0, sizeof (str));
	CConfig::GetInstance()->GetValue(SECTION, END, "11:1:0-2:0:0", str, sizeof (str));
	sscanf(str, "%d:%d:%d-%d:%d:%d", &m_Dst.end.year, &m_Dst.end.mon, &m_Dst.end.day, &m_Dst.end.hour, &m_Dst.end.min, &m_Dst.end.sec);

	memset((void *)str,0,sizeof(str));
	CConfig::GetInstance()->GetValue(SECTION,BEGIN_WEEK,"3:2:0-2:0:0",str,sizeof(str));
	sscanf(str,"%d:%d:%d-%d:%d:%d",&mon,&week,&day, &hour,&min,&sec);
	m_Dst.struWeekModeStart.bytStartMonth = mon;m_Dst.struWeekModeStart.bytWhichWeek = week;
	m_Dst.struWeekModeStart.bytWeekDay = day;m_Dst.struWeekModeStart.bytHour = hour;
	m_Dst.struWeekModeStart.bytMinute = min;m_Dst.struWeekModeStart.bytSecond = sec;

	memset((void *)str,0,sizeof(str));
	CConfig::GetInstance()->GetValue(SECTION,END_WEEK,"11:1:0-2:0:0",str,sizeof(str));	
	sscanf(str,"%d:%d:%d-%d:%d:%d",&mon,&week,&day, &hour,&min,&sec);
	m_Dst.struWeekModeStop.bytStartMonth = mon;m_Dst.struWeekModeStop.bytWhichWeek = week;
	m_Dst.struWeekModeStop.bytWeekDay = day;m_Dst.struWeekModeStop.bytHour = hour;
	m_Dst.struWeekModeStop.bytMinute = min;m_Dst.struWeekModeStop.bytSecond = sec;

#if 0

    /* 如果使能关闭，且状态为1，则表明需要时间恢复 */
    if (m_bEnable == 0 && eDstForbidChangeTime == m_DSTStatus)
    {

        CSYS_INF("---Need to time calibration sub %d seconds---\n", off_dias);

#if 0
        struct timeval now_time_tv = {0, 0};

        now_time_tv.tv_sec = time(NULL) - off_dias;        
        now_time_tv.tv_usec = 0;

        if (settimeofday(&now_time_tv, NULL))
        {
            fprintf(stderr, "Fail to called settimeofday: %s\n", strerror(errno));
        }

#else

        time_t now_time = 0;
        if (stime(&now_time))
        {
            fprintf(stderr, "Fail to called stime() : %s\n", strerror(errno));
        }
#endif
        else
        {
            // 通知系统SDK对时
//          ZWSys_EncSetCurrentTime(); // 操作DST不能把时间TC时钟里面
            m_DSTStatus = eDSTAllowChangeTime;            // 恢复状态并写入配置
            SAVE_STATUS(m_DSTStatus);            
        }
    }

#endif

	m_bReboot = 1;

	now = time(NULL);
	status = m_DSTStatus;
	update_dst_time(now);
	now = update_dst(now);

	if (INVALID_TIME != now)
	{
		user_func_call(now);
	}

	if (status != m_DSTStatus)
	{
		SAVE_STATUS(m_DSTStatus);
	}

	m_bReboot = 0;

	status = pthread_create(&thread_t, &new_attr, work_thread, NULL);
	if (status != 0)
	{
		perror("创建线程出错\n");
		CSYS_ERR("创建同步线程出错:%s\n", strerror(errno));
		return -1;
	}

	return 0;
}

int IsValidDst(LPDVR_DST_EX Dst)
{
	tagTime BeginTime;
	tagTime EndTime;
	tagTime NowTime;
	time_t TmpTime0;
	time_t TmpTime1;
	struct timeval tv;

	// 星期模式
	if (0 == Dst->dwMode)
	{
		int BeginWeekDay = 0;
		int BeginWeek = 0;
		int EndWeekDay = 0;
		int EndWeek = 0;
	
		gettimeofday(&tv, NULL);
		NowTime.MkTag(tv.tv_sec);
		BeginTime.year = NowTime.year;
		EndTime.year = NowTime.year;
		BeginTime.mon = Dst->struWeekModeStart.bytStartMonth;
		EndTime.mon = Dst->struWeekModeStop.bytStartMonth;
		BeginTime.day = 0;
		EndTime.day = 0;
		BeginTime.hour = Dst->struWeekModeStart.bytHour;
		BeginTime.min = Dst->struWeekModeStart.bytMinute;
		BeginTime.sec = Dst->struWeekModeStart.bytSecond;
		EndTime.hour = Dst->struWeekModeStop.bytHour;
		EndTime.min = Dst->struWeekModeStop.bytMinute;
		EndTime.sec = Dst->struWeekModeStop.bytSecond;
		BeginWeek = Dst->struWeekModeStart.bytWhichWeek;
		BeginWeekDay = Dst->struWeekModeStart.bytWeekDay;
		EndWeek = Dst->struWeekModeStop.bytWhichWeek;
		EndWeekDay = Dst->struWeekModeStart.bytWeekDay;
		TmpTime0 = weekday_to_day(BeginTime.year, BeginTime.mon, BeginWeek, BeginWeekDay, BeginTime.hour, BeginTime.min, BeginTime.sec);
		TmpTime1 = weekday_to_day(EndTime.year, EndTime.mon, EndWeek, EndWeekDay, EndTime.hour, EndTime.min, EndTime.sec);
		if ((INVALID_TIME == TmpTime0) || (INVALID_TIME == TmpTime1))
		{
			return 0;
		}

		if (Dst->byDSTBias > 0)
		{
			// 夏时制不跨年
			if (TmpTime0 <= TmpTime1)
			{
				// 开始时间偏移后超出了结束时间
				if ((TmpTime0 + Dst->byDSTBias) >= TmpTime1)
				{
					return 0;
				}
				BeginTime.year += 1;
				TmpTime0 = weekday_to_day(BeginTime.year, BeginTime.mon, BeginWeek, BeginWeekDay, BeginTime.hour, BeginTime.min, BeginTime.sec);
				// 结束时间偏移后超过了下一次开始时间
				if ((TmpTime1 + Dst->byDSTBias) >= TmpTime0)
				{
					return 0;
				}
			}
			// 跨年
			else
			{
				// 上一次结束时间向后调整不能超过这次开始时间
				if ((TmpTime1 + Dst->byDSTBias) >= TmpTime0)
				{
					return 0;
				}
				// 把结束时间设置到第二年
				EndTime.year += 1;
				TmpTime1 = weekday_to_day(EndTime.year, EndTime.mon, EndWeek, EndWeekDay, EndTime.hour, EndTime.min, EndTime.sec);
				// 开始时间偏移后超出了结束时间
				if ((TmpTime0 + Dst->byDSTBias) >= TmpTime1)
				{
					return 0;
				}
			}
		}
		// 时间向前调整
		else if (Dst->byDSTBias < 0)
		{
			// 跨年
			if (TmpTime1 <= TmpTime0)
			{
				// 开始时间向前调整后不能比上一次结束时间更前
				if ((TmpTime0 - abs(Dst->byDSTBias)) <= TmpTime1)
				{
					return 0;
				}
				EndTime.year += 1;
				// 结束时间向前调整后不能比开始时间更前
				TmpTime1 = weekday_to_day(EndTime.year, EndTime.mon, EndWeek, EndWeekDay, EndTime.hour, EndTime.min, EndTime.sec);
				if ((TmpTime1 - abs(Dst->byDSTBias)) <= TmpTime0)
				{
					return 0;
				}
			}
			// 不跨年,计算一下前一年的结束时间
			else
			{
				// 结束时间向前调整后不能比开始时间更前
				if ((TmpTime1 - abs(Dst->byDSTBias)) <= TmpTime0)
				{
					return 0;
				}
				// 开始时间向前调整后不能比上一次结束时间更前
				EndTime.year -= 1;
				TmpTime1 = weekday_to_day(EndTime.year, EndTime.mon, EndWeek, EndWeekDay, EndTime.hour, EndTime.min, EndTime.sec);
				if ((TmpTime0 - abs(Dst->byDSTBias)) <= TmpTime1)
				{
					return 0;
				}
			}
		}
		else
		{
			return 0;
		}

	}
	else
	{
		gettimeofday(&tv, NULL);
		NowTime.MkTag(tv.tv_sec);
		BeginTime = Dst->start;
		EndTime = Dst->end;
		BeginTime.year = NowTime.year;
		EndTime.year = NowTime.year;
		TmpTime0 = BeginTime.MkSecond();
		TmpTime1 = EndTime.MkSecond();

		if (Dst->byDSTBias > 0)
		{
			// 夏时制不跨年
			if (TmpTime0 <= TmpTime1)
			{
				// 开始时间偏移后超出了结束时间
				if ((TmpTime0 + Dst->byDSTBias) >= TmpTime1)
				{
					return 0;
				}
				BeginTime.year += 1;
				TmpTime0 = BeginTime.MkSecond();
				// 结束时间偏移后超过了下一次开始时间
				if ((TmpTime1 + Dst->byDSTBias) >= TmpTime0)
				{
					return 0;
				}
			}
			// 跨年
			else
			{
				// 上一次结束时间向后调整不能超过这次开始时间
				if ((TmpTime1 + Dst->byDSTBias) >= TmpTime0)
				{
					return 0;
				}
				// 把结束时间设置到第二年
				EndTime.year += 1;
				TmpTime1 = EndTime.MkSecond();
				// 开始时间偏移后超出了结束时间
				if ((TmpTime0 + Dst->byDSTBias) >= TmpTime1)
				{
					return 0;
				}
			}
		}
		// 时间向前调整
		else if (Dst->byDSTBias < 0)
		{
			// 跨年
			if (TmpTime1 <= TmpTime0)
			{
				// 开始时间向前调整后不能比上一次结束时间更前
				if ((TmpTime0 - abs(Dst->byDSTBias)) <= TmpTime1)
				{
					return 0;
				}
				EndTime.year += 1;
				// 结束时间向前调整后不能比开始时间更前
				TmpTime1 = EndTime.MkSecond();
				if ((TmpTime1 - abs(Dst->byDSTBias)) <= TmpTime0)
				{
					return 0;
				}
			}
			// 不跨年,计算一下前一年的结束时间
			else
			{
				// 结束时间向前调整后不能比开始时间更前
				if ((TmpTime1 - abs(Dst->byDSTBias)) <= TmpTime0)
				{
					return 0;
				}
				// 开始时间向前调整后不能比上一次结束时间更前
				EndTime.year -= 1;
				TmpTime1 = EndTime.MkSecond();
				if ((TmpTime0 - abs(Dst->byDSTBias)) <= TmpTime1)
				{
					return 0;
				}
			}
		}
		else
		{
			return 0;
		}
	}
	return 1;
}

int EDVR_MAN_SetDST(LPDVR_DST_EX Dst)
{
	char str[100];

	time_t tmp = INVALID_TIME;

	if (NULL == Dst)
	{
		return -1;
	}

	if (0 == IsValidDst(Dst))
	{
		return -2;
	}
	
	LOCK(m_mutex);

	if (0 == memcmp(&m_Dst, Dst, sizeof (DVR_DST_EX)))
	{
		UNLOCK(m_mutex);
		return 0;
	}

	if (m_bEnable == 0)
	{
		memcpy(&m_Dst, Dst, sizeof (DVR_DST_EX));

		//保存配置
		CConfig::GetInstance()->SetValue(SECTION,TIME,m_Dst.byDSTBias);
		CConfig::GetInstance()->SetValue(SECTION,MODE,(m_Dst.dwMode == eDSTTimeFormat_Date_EX));
		
		memset((void *)str,0,sizeof(str));
		snprintf(str,sizeof(str),"%d:%d:%d-%d:%d:%d",m_Dst.start.year,m_Dst.start.mon,m_Dst.start.day,m_Dst.start.hour,m_Dst.start.min,m_Dst.start.sec);
		CConfig::GetInstance()->SetValue(SECTION,BEGIN,str);
		
		memset((void *)str,0,sizeof(str));
		snprintf(str,sizeof(str),"%d:%d:%d-%d:%d:%d",m_Dst.end.year,m_Dst.end.mon,m_Dst.end.day,m_Dst.end.hour,m_Dst.end.min,m_Dst.end.sec);
		CConfig::GetInstance()->SetValue(SECTION,END,str);

		memset((void *)str,0,sizeof(str));
		snprintf(str,sizeof(str),"%d:%d:%d-%d:%d:%d",m_Dst.struWeekModeStart.bytStartMonth,m_Dst.struWeekModeStart.bytWhichWeek,m_Dst.struWeekModeStart.bytWeekDay,
										m_Dst.struWeekModeStart.bytHour,m_Dst.struWeekModeStart.bytMinute,m_Dst.struWeekModeStart.bytSecond);
		CConfig::GetInstance()->SetValue(SECTION,BEGIN_WEEK,str);
		
		memset((void *)str,0,sizeof(str));
		snprintf(str,sizeof(str),"%d:%d:%d-%d:%d:%d",m_Dst.struWeekModeStop.bytStartMonth,m_Dst.struWeekModeStop.bytWhichWeek,m_Dst.struWeekModeStop.bytWeekDay,
										m_Dst.struWeekModeStop.bytHour,m_Dst.struWeekModeStop.bytMinute,m_Dst.struWeekModeStop.bytSecond);
		CConfig::GetInstance()->SetValue(SECTION,END_WEEK,str);
		
		UNLOCK(m_mutex);
		return 0;
	}

	//计算出禁止后的时间（实际时间）
	tmp = time(NULL);

	//保存配置
	memcpy(&m_Dst, Dst, sizeof (DVR_DST_EX));

	//计算出设置新配置后应该的夏时制时间
	update_dst_time(tmp);

	//保存配置
	CConfig::GetInstance()->SetValue(SECTION,TIME,m_Dst.byDSTBias);
	CConfig::GetInstance()->SetValue(SECTION,MODE,(m_Dst.dwMode == eDSTTimeFormat_Date_EX));
	
	
	memset((void *)str,0,sizeof(str));
	snprintf(str,sizeof(str),"%d:%d:%d-%d:%d:%d",m_Dst.start.year,m_Dst.start.mon,m_Dst.start.day,m_Dst.start.hour,m_Dst.start.min,m_Dst.start.sec);
	CConfig::GetInstance()->SetValue(SECTION,BEGIN,str);
	
	memset((void *)str,0,sizeof(str));
	snprintf(str,sizeof(str),"%d:%d:%d-%d:%d:%d",m_Dst.end.year,m_Dst.end.mon,m_Dst.end.day,m_Dst.end.hour,m_Dst.end.min,m_Dst.end.sec);
	CConfig::GetInstance()->SetValue(SECTION,END,str);

	memset((void *)str,0,sizeof(str));
	snprintf(str,sizeof(str),"%d:%d:%d-%d:%d:%d",m_Dst.struWeekModeStart.bytStartMonth,m_Dst.struWeekModeStart.bytWhichWeek,m_Dst.struWeekModeStart.bytWeekDay,
									m_Dst.struWeekModeStart.bytHour,m_Dst.struWeekModeStart.bytMinute,m_Dst.struWeekModeStart.bytSecond);
	CConfig::GetInstance()->SetValue(SECTION,BEGIN_WEEK,str);
	
	memset((void *)str,0,sizeof(str));
	snprintf(str,sizeof(str),"%d:%d:%d-%d:%d:%d",m_Dst.struWeekModeStop.bytStartMonth,m_Dst.struWeekModeStop.bytWhichWeek,m_Dst.struWeekModeStop.bytWeekDay,
									m_Dst.struWeekModeStop.bytHour,m_Dst.struWeekModeStop.bytMinute,m_Dst.struWeekModeStop.bytSecond);
	CConfig::GetInstance()->SetValue(SECTION,END_WEEK,str);
	
	UNLOCK(m_mutex);
	return 0;
}

int EDVR_MAN_GetDST(LPDVR_DST_EX Dst)
{

	if (NULL == Dst)
	{
		return -1;
	}

	LOCK(m_mutex);
	memcpy(Dst, &m_Dst, sizeof (DVR_DST_EX));
	UNLOCK(m_mutex);

	return 0;
}

int EDVR_MAN_EnableDST(unsigned int enable)
{
	int status;

	enable = !!(enable);

	time_t now;

	LOCK(m_mutex);

	m_bEnable = enable;

	now = time(NULL);

	update_dst_time(now);
	
	status = m_DSTStatus;

	m_DSTStatus = eDSTAllowChangeTime;
	
	if(m_bEnable == 1)
	{
		//允许夏令时修改时间
		now = update_dst(now);
	}

	//表示中间有禁用变为启用
	//m_bReboot = 1;

	if (INVALID_TIME != now && m_bEnable != 0)
	{
		user_func_call(now);
	}

	if (status != m_DSTStatus)
	{
		SAVE_STATUS(m_DSTStatus);
	}

	m_bReboot = 0;

	CConfig::GetInstance()->SetValue(SECTION, BENABLE, m_bEnable);

	UNLOCK(m_mutex);

	return 0;
}

int EDVR_MAN_IsEnableDST()
{
	return (m_bEnable);
}

void EDVR_MAN_ManualRefresh_DST(void)
{
    CSYS_INF("Need to manual refresh dst time!\n");
    
	LOCK(m_mutex);

	time_t now = time(NULL);
	
    m_DSTStatus = eDSTAllowChangeTime;

	//表示中间有禁用变为启用
	//m_bReboot = 1;

	update_dst_time(now);
	now = update_dst(now);

	if (INVALID_TIME != now)
	{
		user_func_call(now);
	}

	m_bReboot = 0;

	UNLOCK(m_mutex);
}

int EDVR_MAN_GetDstBias(void)
{
    return (m_Dst.byDSTBias);
}

int EDVR_MAN_GetDstIsWork(void)
{
    return (m_DSTStatus);
}

void EDVR_MAN_SetCallBack(CallBack callback)
{
	user_func = callback;
}


static int update_dst_time(time_t time)
{
	tagTime T;

	DVR_DST_EX Dst;


	if (1)						//m_Dst.start.year == 0)
	{
		//0年3月：2日-2时：0分：0秒

		T.MkTag(time);
		memcpy(&Dst,&m_Dst,sizeof(DVR_DST_EX));
		
		if (m_Dst.dwMode == eDSTTimeFormat_Date_EX)//m_Dst.start.year == 0
		{			
			Dst.start.year = T.year;
			Dst.end.year = T.year;
		}
		else
		{
			Dst.start.MkTag(weekday_to_day(T.year,m_Dst.struWeekModeStart.bytStartMonth,m_Dst.struWeekModeStart.bytWhichWeek,m_Dst.struWeekModeStart.bytWeekDay,
										m_Dst.struWeekModeStart.bytHour,m_Dst.struWeekModeStart.bytMinute,m_Dst.struWeekModeStart.bytSecond));
			Dst.end.MkTag(weekday_to_day(T.year,m_Dst.struWeekModeStop.bytStartMonth,m_Dst.struWeekModeStop.bytWhichWeek,m_Dst.struWeekModeStop.bytWeekDay,
										m_Dst.struWeekModeStop.bytHour,m_Dst.struWeekModeStop.bytMinute,m_Dst.struWeekModeStop.bytSecond));

		}



		//当前时间大于当前的结束时间
		if (T.MkSecond() >= (Dst.end.MkSecond() + abs(m_Dst.byDSTBias)))
		{
			//不是跨年模式（3月-11月）
			if (Dst.start.MkSecond() <= Dst.end.MkSecond())
			{
				if (m_Dst.dwMode == eDSTTimeFormat_Date_EX)//m_Dst.start.year == 0
				{			
					Dst.start.year = T.year+1;
					Dst.end.year = T.year+1;
				}
				else
				{
					Dst.start.MkTag(weekday_to_day(T.year+1,m_Dst.struWeekModeStart.bytStartMonth,m_Dst.struWeekModeStart.bytWhichWeek,m_Dst.struWeekModeStart.bytWeekDay,
												m_Dst.struWeekModeStart.bytHour,m_Dst.struWeekModeStart.bytMinute,m_Dst.struWeekModeStart.bytSecond));
					Dst.end.MkTag(weekday_to_day(T.year+1,m_Dst.struWeekModeStop.bytStartMonth,m_Dst.struWeekModeStop.bytWhichWeek,m_Dst.struWeekModeStop.bytWeekDay,
												m_Dst.struWeekModeStop.bytHour,m_Dst.struWeekModeStop.bytMinute,m_Dst.struWeekModeStop.bytSecond));

				}
			}
			//跨年模式（11月-3月）
			else
			{
				if (m_Dst.dwMode == eDSTTimeFormat_Date_EX)//m_Dst.start.year == 0
				{			
					Dst.start.year = T.year;
					Dst.end.year = T.year + 1;
				}
				else
				{
					Dst.start.MkTag(weekday_to_day(T.year,m_Dst.struWeekModeStart.bytStartMonth,m_Dst.struWeekModeStart.bytWhichWeek,m_Dst.struWeekModeStart.bytWeekDay,
												m_Dst.struWeekModeStart.bytHour,m_Dst.struWeekModeStart.bytMinute,m_Dst.struWeekModeStart.bytSecond));
					Dst.end.MkTag(weekday_to_day(T.year+1,m_Dst.struWeekModeStop.bytStartMonth,m_Dst.struWeekModeStop.bytWhichWeek,m_Dst.struWeekModeStop.bytWeekDay,
												m_Dst.struWeekModeStop.bytHour,m_Dst.struWeekModeStop.bytMinute,m_Dst.struWeekModeStop.bytSecond));

				}
			}
		}
		else
		{
			//不是跨年模式（3月-11月）
			if (Dst.start.MkSecond() <= Dst.end.MkSecond())
			{
				if (m_Dst.dwMode == eDSTTimeFormat_Date_EX)//m_Dst.start.year == 0
				{			
					Dst.start.year = T.year;
					Dst.end.year = T.year;
				}
				else
				{
					Dst.start.MkTag(weekday_to_day(T.year,m_Dst.struWeekModeStart.bytStartMonth,m_Dst.struWeekModeStart.bytWhichWeek,m_Dst.struWeekModeStart.bytWeekDay,
												m_Dst.struWeekModeStart.bytHour,m_Dst.struWeekModeStart.bytMinute,m_Dst.struWeekModeStart.bytSecond));
					Dst.end.MkTag(weekday_to_day(T.year,m_Dst.struWeekModeStop.bytStartMonth,m_Dst.struWeekModeStop.bytWhichWeek,m_Dst.struWeekModeStop.bytWeekDay,
												m_Dst.struWeekModeStop.bytHour,m_Dst.struWeekModeStop.bytMinute,m_Dst.struWeekModeStop.bytSecond));

				}
			}
			//跨年模式（11月-3月）
			else
			{
				if (m_Dst.dwMode == eDSTTimeFormat_Date_EX)//m_Dst.start.year == 0
				{			
					Dst.start.year = T.year-1;
					Dst.end.year = T.year;
				}
				else
				{
					Dst.start.MkTag(weekday_to_day(T.year-1,m_Dst.struWeekModeStart.bytStartMonth,m_Dst.struWeekModeStart.bytWhichWeek,m_Dst.struWeekModeStart.bytWeekDay,
												m_Dst.struWeekModeStart.bytHour,m_Dst.struWeekModeStart.bytMinute,m_Dst.struWeekModeStart.bytSecond));
					Dst.end.MkTag(weekday_to_day(T.year,m_Dst.struWeekModeStop.bytStartMonth,m_Dst.struWeekModeStop.bytWhichWeek,m_Dst.struWeekModeStop.bytWeekDay,
												m_Dst.struWeekModeStop.bytHour,m_Dst.struWeekModeStop.bytMinute,m_Dst.struWeekModeStop.bytSecond));
				}
			}

		}

		memcpy(&m_StartTime, &Dst.start, sizeof (tagTime));
		memcpy(&m_EndTime, &Dst.end, sizeof (tagTime));

	}
	else
	{							//几月第几个周几-2时：0分：0秒

	}

	//m_StartTime.print();
	//m_EndTime.print();
	return 0;
}

//extern int isDoNtpSync;
//extern int isDoNtpSyncIfSucc;
static time_t update_dst(time_t now)
{

	tagTime T;

	T.MkTag(now);

	//如果已经被禁止
	if (0 == m_bEnable)
	{
		//如果已经是夏时制
		if (eDstForbidChangeTime == m_DSTStatus)
		{
			m_DSTStatus = eDSTAllowChangeTime;//还原成未修改之前的系统时间

#if 0
			//做ntp同步时，需要关闭夏时制然后打开，但关闭操作不能影响系统时间
			if (isDoNtpSync)
			{
        		return  INVALID_TIME;
			}
#endif
			//如果夏令时使能被禁用或并且之前开启夏令时修改过时间，将时间还原
			//避免出现夏令时修改完时间后，将夏令时开始时间设置为大于当前时间，时间不能还原依然保持修改后的时间	
			now -= CConfig::GetInstance()->GetValue(SECTION, TIME, 0);

			return now;
		}

		return INVALID_TIME;
	}

#if 0

	//做ntp同步时且结果失败那么不再去修改时间，因为在关闭夏令时时也为修改系统时间
	//这样即使对时失败也不会修改系统时间
	if(isDoNtpSync && (!isDoNtpSyncIfSucc))
		return  INVALID_TIME;

#endif

	//处理不应该出现的时间段
	//调整时间为正
#if 0
	if (m_Dst.byDSTBias > 0)
	{
		if (now > m_StartTime.MkSecond() && now < (m_StartTime.MkSecond() + m_Dst.byDSTBias))
		{
			m_DSTStatus = eDstForbidChangeTime;
			DEBUG_MSG("启动夏时制不应该有此时间\n");
			return (now + m_Dst.byDSTBias);

		}
	}
	else
	{
		//调整时间为负
		if (now > m_EndTime.MkSecond() && now < (m_EndTime.MkSecond() - m_Dst.byDSTBias))
		{
			m_DSTStatus = eDSTAllowChangeTime;
			DEBUG_MSG("启动夏时制不应该有此时间\n");
			return (now - m_Dst.byDSTBias);

		}
	}
	
#endif
	//在共有的时间段内
	//调整时间为正
	if (m_Dst.byDSTBias > 0)
	{
		if (now <= m_EndTime.MkSecond() && now >= (m_EndTime.MkSecond() - m_Dst.byDSTBias))
		{
			CSYS_ERR("在共有的时间范围内\n");
			return INVALID_TIME;

		}
	}
	else
	{
		//调整时间为负
		if (now <= m_StartTime.MkSecond() && now >= (m_StartTime.MkSecond() + m_Dst.byDSTBias))
		{
			CSYS_ERR("在共有的时间范围内\n");
			return INVALID_TIME;

		}
	}

	//设备刚启动时夏时制处理
	if (m_bReboot)
	{
		//不在夏时制的时间范围内
		CSYS_INF(" %d [%lld %d %d]\n", m_DSTStatus, now, m_StartTime.MkSecond(), m_EndTime.MkSecond());
		if ((now < (m_StartTime.MkSecond())) || (now > (m_EndTime.MkSecond())))
		{
			if (eDstForbidChangeTime == m_DSTStatus)
			{
				m_DSTStatus = eDSTAllowChangeTime;
				CSYS_INF("设备刚启动已经不在夏时制时间范围内\n");
				return (now - m_Dst.byDSTBias);
			}

		}
		else
		{
			if (eDSTAllowChangeTime == m_DSTStatus)
			{
				m_DSTStatus = eDstForbidChangeTime;
				CSYS_INF("设备刚启动已经在夏时制时间范围内\n");
				return (now + m_Dst.byDSTBias);
			}

		}
		m_bReboot = 0;
	}

	//处理进入夏时制
	//当前时间大于等于夏时制开始时间但在MAX_INTERVAL_TIME之内。表示正常进入
	if ((now >= m_StartTime.MkSecond()) && ((now - m_StartTime.MkSecond()) < MAX_INTERVAL_TIME))
	{
		if (eDSTAllowChangeTime == m_DSTStatus)
		{
			m_DSTStatus = eDstForbidChangeTime;
			CSYS_INF("进入夏时制时间范围内\n");
			return (now + m_Dst.byDSTBias);
		}
	}
	else if ((now >= (m_StartTime.MkSecond() + MAX_INTERVAL_TIME)) && ((now < m_EndTime.MkSecond())))
	{
		//当前时间大于等于夏时制开始时间但在结束时间之内。表示是由于修改时间进入的
		if (eDSTAllowChangeTime == m_DSTStatus)
		{
			m_DSTStatus = eDstForbidChangeTime;
			CSYS_INF("修改时间到夏时制时间范围内\n");
			return (now + m_Dst.byDSTBias);
		}
	} 
	else if ((now <= m_StartTime.MkSecond()) && (m_StartTime.MkSecond() - now) <= MAX_INTERVAL_TIME) 
	{
        //当前时间大于等于夏时制开始时间但在结束时间之内。表示是由于修改时间进入的
		if (eDSTAllowChangeTime == m_DSTStatus)
		{
			m_DSTStatus = eDstForbidChangeTime;
			CSYS_INF("修改时间到夏时制时间范围内\n");
			return now + m_Dst.byDSTBias;
		}   

	}

	//处理退出夏时制
	//当前时间大于等于夏时制结束时间但在MAX_INTERVAL_TIME之内。表示正常退出
	if ((now >= m_EndTime.MkSecond()) && ((now - m_EndTime.MkSecond()) < MAX_INTERVAL_TIME))
	{
		if (eDstForbidChangeTime == m_DSTStatus)
		{
            m_DSTStatus = eDSTAllowChangeTime;
			m_bEnable = 0;
			CSYS_INF("退出夏时制\n");
			return (now - m_Dst.byDSTBias);
		}
	}
#if 0
	else if ((now < (m_StartTime.MkSecond())))
	{
		//当前时间小与夏时制开始时间。表示是由于修改时间退出的
		if (eDstForbidChangeTime == m_DSTStatus)
		{
			m_DSTStatus = eDSTAllowChangeTime;
			DEBUG_MSG("退出夏时制\n");
			return INVALID_TIME;
		}
	}
	
#endif
	return INVALID_TIME;

}

static void *work_thread(void *org)
{
    PRT_TID_NAME("Dst_WorkThread");

	time_t now;

	int status;

	while (1)
	{

		LOCK(m_mutex);

		status = m_DSTStatus;
		now = time(NULL);
		update_dst_time(now);
		now = update_dst(now);

		if (INVALID_TIME != now)
		{
			user_func_call(now);
		}

		if (status != m_DSTStatus)
		{
			SAVE_STATUS(m_DSTStatus);
		}


		UNLOCK(m_mutex);
		usleep(1000000);
	}
	return NULL;
}

extern "C" int ZWSys_EncSetCurrentTime( );

static void user_func_call(time_t time)
{
	int nSDKRet;
    char tBuff[64] = {0};
	if (NULL == user_func)
	{
		//设置时间
		nSDKRet = stime(&time);
		if (0 != nSDKRet)
		{
			//设置失败
			ctime_r(&time,tBuff);
			CSYS_ERR("call stime() fail, tNewTime %llu, return %d, mktime %s \n", time, nSDKRet, tBuff);
		}
		else
		{
			// 知系统SDK对时
 			// ZWSys_EncSetCurrentTime(); // DST时间不能写入RTC
		}

	}
	else
	{
		user_func(time);
	}
}


time_t weekday_to_day(int year,int mon,int week,int weekday,int hour,int min,int sec)
{
	tagTime T;

	struct tm tm;

	time_t now;

	time_t retval = INVALID_TIME;

	int i;

	int nweek_index = 1;

	T.year = year;
	T.mon = mon;
	T.hour = hour;
	T.min = min;
	T.sec = sec;

	for (i = 1; i <= 31; i++)
	{
		T.day = i;
		//
		now = T.MkSecond();
		localtime_r(&now, &tm);

		if (i != tm.tm_mday)
		{
			break;
		}

		if (tm.tm_wday == weekday)
		{
			retval = now;
			if (nweek_index == week)
			{
				return retval;
			}
			nweek_index++;
		}
	}

	if (retval == INVALID_TIME)
	{
		CSYS_WRAN("时间不对[year = %d,mon = %d,week = %d,weekday = %d,hour = %d,min = %d,sec = %d]\n", year, mon, week, weekday, hour, min, sec);
	}

	return retval;
}
