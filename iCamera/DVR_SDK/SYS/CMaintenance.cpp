
/*!
*\file   CMaintenance.cpp 
*\brief  维护功能的实现
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/

#include "global_macro.h"
#include "debug.h"
#include "CMaintenance.h"
#include "idvr_errno.h"
#include "PAR/CConfig.h"
#include "mysystem.h"
// #include "sys_banned.h"
#include "global_gather.h"
/**************************************************************************
*                            常量                                        *
**************************************************************************/

/**************************************************************************
*                            宏                                          *
**************************************************************************/



/**************************************************************************
*                          数据类型                                       *
**************************************************************************/


/**************************************************************************
*                           全局变量                                      *
**************************************************************************/

/*!< 配置文件的段头*/
static const char *section = "maintenance";

/*!< 工作模式*/
static const char *key_workmode = "workmode";

/*!< 维护模式*/
static const char *key_mode = "mode";

/*!< 是否启用维护*/
static const char *key_enable = "benable";

/*!< 维护时间点*/
static const char *key_year = "year";

static const char *key_mon = "mon";

static const char *key_day = "day";

static const char *key_hour = "hour";

static const char *key_min = "min";

static const char *key_sec = "second";

/*!<记录星期 比特位从低到高分别为：日、1、2、3、4、5、、6*/
static const char *key_week = "week";

/*!< 全局维护实例*/
CMaintenance *CMaintenance::theCMaintenance = NULL;

/**************************************************************************
*                           局部函数原型                                  *
 **************************************************************************/

/**************************************************************************
*                           方法实现                                      *
**************************************************************************/


//!\brief 获取维护配置
//!\praram char *key 关键字
//!\praramint value 值
//!\return  成功为0，失败为-1
int CMaintenance::GetValue(const char *key, int value)
{
	CConfig *theCConfig = CConfig::GetInstance();

	if (NULL != theCConfig)
	{
		value = theCConfig->GetValue(section, key, value);
	}
	return value;
}


//!\brief 读取最后一次维护时间
//!\return  成功为0，失败为-1   
int CMaintenance::ReadLastMaiantenanceTime()
{
	FILE *file;

	file = fopen(LAST_MAINTENANCETIME_FILE, "rb");
	memset(&m_LastTime, 0, sizeof (tagTime));

	if (NULL == file)
	{
		CSYS_WRAN("前面没有进行维护过\n");
		return 0;
	}

	fread(&m_LastTime, sizeof (tagTime), 1, file);
	fclose(file);
	CSYS_INF("上次维护时间：");
	m_LastTime.print();
	return 0;
}

//!\brief 写入最后一次维护时间
//!\return  成功为0，失败为-1
int CMaintenance::WriteLastMaiantenanceTime()
{
	FILE *file;

	file = fopen(LAST_MAINTENANCETIME_FILE, "wb");

	if (NULL == file)
	{
		CSYS_ERR("写入维护时间失败\n");
		return -1;
	}

	fwrite(&m_LastTime, sizeof (tagTime), 1, file);
	fprintf(file, "上一次维护时间：%d年%d月%d日 %d时%d分%d秒\n", m_LastTime.year, m_LastTime.mon, m_LastTime.day, m_LastTime.hour, m_LastTime.min,
			m_LastTime.sec);
	fclose(file);
	return 0;
}



//!\brief 读取配置信息
//!\return  成功为0，失败为-1
int CMaintenance::Read()
{

	//工作模式：表示是否定制了该功能.(最好放到上层做)
	m_WorkMode = GetValue(key_workmode, 1);

	//维护模式
	m_MaintenanceMode = GetValue(key_mode, 0);

	//是否启用了该功能  
	m_bEnable = GetValue(key_enable, 0);

	//维护时间配置
	m_Time.year = GetValue(key_year, 1970);
	m_Time.mon = GetValue(key_mon, 1);
	m_Time.day = GetValue(key_day, 1);
	m_Time.hour = GetValue(key_hour, 0);
	m_Time.min = GetValue(key_min, 0);
	m_Time.sec = GetValue(key_sec, 0);

	ReadLastMaiantenanceTime();

	//星期配置
	m_Week = GetValue(key_week, 0);

	return 0;
}

//!\brief 设置维护配置
//!\praram char *key 关键字
//!\praramint value 值
//!\return  成功为0，失败为-1
int CMaintenance::SetValue(const char *key, int value)
{
	int ret = -1;

	CConfig *theCConfig = CConfig::GetInstance();

	if (NULL != theCConfig)
	{
		ret = theCConfig->SetValue(section, key, value);
	}

	return ret;
}

//!\brief 设置维护配置模式
//!\praram int nMode 维护模式 0为每天、1为每周、2为单次
//!\return  成功为0，失败为-1
//##ModelId=499CF1AD0121
int CMaintenance::SetMaintenanceMode(int nMode)
{
	// TODO: Add your specialized code here.

	int ret = IDVR_ERROR_PARAMETER;

	if (nMode < 0 || nMode > 2)
	{
		CSYS_ERR("参数错误:%d\n", nMode);
		return ret;
	}

	ret = SetValue(key_mode, nMode);
	m_MaintenanceMode = nMode;
	return (int) ret;
}

//!\brief 获取维护配置模式
//!\praram tagMaintenance *cfg 维护配置指针
//!\return  维护模式 0为每天、1为每周、2为单次
//##ModelId=499CF1C600A4
int CMaintenance::GetMaintenanceMode()
{
	// TODO: Add your specialized code here.
	return (int) m_MaintenanceMode;
}

//!\brief 定制/获取取消定制功能
//!\praram BOOL nCustomize 0为取消定制，1为定制
//!\return  成功为0，失败为-1
//##ModelId=49B89FE201A4
int CMaintenance::SetWorkMode(bool nCustomize)
{
	// TODO: Add your specialized code here.
	SetValue(key_workmode, nCustomize);
	m_WorkMode = nCustomize;
	return (int) 0;
}

//!\brief 定制/获取取消定制功能
//!\praram BOOL nCustomize 0为取消定制，1为定制
//!\return  false 为无定制，true 为定制
//##ModelId=49BE0BCF0360
bool CMaintenance::GetWorkMode()
{
	// TODO: Add your specialized code here.
	return (bool) m_WorkMode;
}

//!\brief 禁用/启用
//!\praram BOOL nCustomize 0为取消定制，1为定制
//!\return  成功为0，失败为-1
//##ModelId=49BE0BCF0388
int CMaintenance::Enable(bool bEnable)
{
	// TODO: Add your specialized code here.
	SetValue(key_enable, bEnable);
	m_bEnable = bEnable;
	return (int) 0;
}

//!\brief 判断是否启用该功能
//!\praram 
//!\return  0为无定制，1为定制
//##ModelId=49B8A047020D
bool CMaintenance::IsEnable()
{
	// TODO: Add your specialized code here.
	return (bool) m_bEnable;
}

//!\brief 构造函数
CMaintenance::CMaintenance()
{
	bWorkTrheadRun = 0;
	CSYS_INF("test sizeof(tagTime) = %d\n", sizeof (tagTime));
	m_CallBack = NULL;
}

//!\brief 获取句柄
//!\return  成功为为句柄，失败为NULL
//##ModelId=49BB33310243
CMaintenance *CMaintenance::GetInstance()
{
	// TODO: Add your specialized code here.
	if (NULL == theCMaintenance)
	{
		theCMaintenance = new CMaintenance();

		if (NULL != theCMaintenance)
		{
			theCMaintenance->Read();

			theCMaintenance->StartMaintenanceTask();
		}
	}
	return (CMaintenance *) theCMaintenance;
}


//!\brief 销毁实例
//!\praram CMaintenance *handle 句柄
//!\return  成功为0，失败为-1
//##ModelId=49BB3331024D
int CMaintenance::ReleaseInstance()
{
	// TODO: Add your specialized code here.
	if (NULL != theCMaintenance)
	{
		theCMaintenance->bWorkTrheadRun = 0;
		delete theCMaintenance;

		theCMaintenance = NULL;
	}
	return (int) 0;
}

//!\brief 获取上一次维护的时间
//!\praram tagTime* time
//!\return  成功为0，失败为-1
//##ModelId=49BB33310293
int CMaintenance::GetLastMainTenanceTime(tagTime * Time)
{
	// TODO: Add your specialized code here.
	if (0 == m_LastTime.year)
	{
		CSYS_ERR("没有进行过维护\n");
		return -1;
	}

	memcpy(Time, &m_LastTime, sizeof (tagTime));
	return (int) 0;
}

//!\brief 获取下一次维护的时间
//!\praram tagTime* time
//!\return  成功为0，失败为-1
//##ModelId=49BB33310293
int CMaintenance::GetNextMainTenanceTime(tagTime * Time)
{
	// TODO: Add your specialized code here.

	return GetNextMainTenanceTime(Time, m_MaintenanceMode, m_Time, m_Week);

	tagTime now;

	now.MkTag(time(NULL));

	memset(Time, 0, sizeof (tagTime));

	if (m_MaintenanceMode == M_ONCE)
	{
		//单次维护模式
		if (now.MkSecond() < m_Time.MkSecond())
		{
			memcpy(Time, &m_Time, sizeof (tagTime));
		}
	}
	else if (m_MaintenanceMode == M_WEEK)
	{
		//周维护模式
		tagTime tmp;

		int i;

		memcpy(&tmp, &now, sizeof (tagTime));
		tmp.hour = m_Time.hour;
		tmp.min = m_Time.min;
		tmp.sec = m_Time.sec;
		for (i = 0; i < 8; i++)
		{
			if ((1 << tmp.GetWeekDay()) & m_Week)
			{
				if (tmp.MkSecond() > now.MkSecond())
				{
					memcpy(Time, &tmp, sizeof (tagTime));
					break;
				}
			}

			tmp.MkTag(tmp.MkSecond() + 3600 * 24);
		}

		if (i >= 8)
		{
			CSYS_ERR("没有找到下次的维护时间,m_Week = %d\n", m_Week);
			return -1;
		}


	}
	else if (m_MaintenanceMode == M_DAY)
	{
		//每天维护模式
		tagTime tmp;

		memcpy(&tmp, &m_Time, sizeof (tagTime));

		tmp.year = now.year;
		tmp.mon = now.mon;
		tmp.day = now.day;

		if (now.MkSecond() > tmp.MkSecond())
		{
			tmp.MkTag(tmp.MkSecond() + 24 * 3600);
		}

		memcpy(Time, &tmp, sizeof (tagTime));

	}
	else
	{
		CSYS_ERR("维护模式:%d错误\n", m_MaintenanceMode);
		return -1;
	}

	return (int) 0;
}


//!\brief 根据参数获取下一次维护时间
//!\praram int nMode 维护模式 0为每天、1为每周、2为单次
//!\praram tagTime time 时间点
//!\praram int WeekDay = 0 星期几
//!\return  成功为0，失败为-1
int CMaintenance::GetNextMainTenanceTime(tagTime * Time, int nMode, tagTime T, int WeekDay)
{
	// TODO: Add your specialized code here.
	tagTime now;

	now.MkTag(time(NULL));

	memset(Time, 0, sizeof (tagTime));

	if (nMode == M_ONCE)
	{
		//单次维护模式
		if (now.MkSecond() < T.MkSecond())
		{
			memcpy(Time, &T, sizeof (tagTime));
		}
	}
	else if (nMode == M_WEEK)
	{
		//周维护模式
		tagTime tmp;

		int i;

		memcpy(&tmp, &now, sizeof (tagTime));
		tmp.hour = T.hour;
		tmp.min = T.min;
		tmp.sec = T.sec;
		for (i = 0; i < 8; i++)
		{
			if ((1 << tmp.GetWeekDay()) & WeekDay)
			{
				if (tmp.MkSecond() > now.MkSecond())
				{
					memcpy(Time, &tmp, sizeof (tagTime));
					break;
				}
			}

			tmp.MkTag(tmp.MkSecond() + 3600 * 24);
		}

		if (i >= 8)
		{
			CSYS_ERR("没有找到下次的维护时间,m_Week = %d\n", WeekDay);
			return -1;
		}
	}
	else if (nMode == M_DAY)
	{
		//每天维护模式
		tagTime tmp;

		memcpy(&tmp, &T, sizeof (tagTime));

		tmp.year = now.year;
		tmp.mon = now.mon;
		tmp.day = now.day;

		if (now.MkSecond() > tmp.MkSecond())
		{
			tmp.MkTag(tmp.MkSecond() + 24 * 3600);
		}

		memcpy(Time, &tmp, sizeof (tagTime));

	}
	else
	{
		CSYS_ERR("维护模式:%d错误\n", nMode);
		return -1;
	}

	return (int) 0;
}


//!\brief 设置星期启用
//!\praram int nWeekDay 0星期天 1星期1，2星期2...
//!\praram bool bEnable false 不启用，true启用
//!\return  成功为0，失败为-1
//##ModelId=49BE0BCF0157
int CMaintenance::EnableWeekDay(int nWeekDay, bool bEnable)
{
	// TODO: Add your specialized code here.

	int ret = IDVR_ERROR_PARAMETER;

	if (nWeekDay < 0 || nWeekDay > 6)
	{
		CSYS_ERR("参数错误nWeekDay = %d\n", nWeekDay);
		return ret;
	}

	if (bEnable)
	{
		m_Week |= 1 << nWeekDay;    /* 把m_Week的第nWeekDay位置1 */
	}
	else
	{
		m_Week &= ~(1 << nWeekDay); /* 把m_Week的第nWeekDay位置0 */
	}

	SetValue(key_week, m_Week);

	return (int) 0;
}


//!\brief 获取星期启用情况
//!\praram int nWeekDay 0星期天 1星期1，2星期2...
//!\praram bool bEnable false 不启用，true启用
//!\return  成功为0，失败为-1
//##ModelId=49BE0BCF01F8
bool CMaintenance::IsEnableWeekDay(int nWeekDay)
{
	// TODO: Add your specialized code here.
	bool ret = false;

	if (m_Week & (1 << nWeekDay))
	{
		ret = true;
	}

	return (bool) ret;
}


//!\brief 设置维护配置时间
//!\praram tagTime time 维护时间。秒无效
//!\return  成功为0，失败为-1
//##ModelId=49BE0BCF0252
int CMaintenance::SetMaintenanceTime(tagTime time)
{
	// TODO: Add your specialized code here.
	/*
	   if ( -1 == time.Check())
	   {
	   DEBUG_MSG("输入参数错误\n");
	   time.print();
	   return IDVR_ERROR_PARAMETER;
	   }
	 */

	time.sec = 0;

	memcpy(&m_Time, &time, sizeof (tagTime));
	SetValue(key_year, m_Time.year);
	SetValue(key_mon, m_Time.mon);
	SetValue(key_day, m_Time.day);
	SetValue(key_hour, m_Time.hour);
	SetValue(key_min, m_Time.min);
	SetValue(key_sec, m_Time.sec);
	return (int) 0;
}


//!\brief 获取维护配置时间
//!\praram tagTime *time 维护时间指针
//!\return  成功为0，失败为-1
int CMaintenance::GetMaintenanceTime(tagTime * time)
{
	// TODO: Add your specialized code here.
	memcpy(time, &m_Time, sizeof (tagTime));
	return (int) 0;
}

void *MaintenanceWorkThread(void *org)
{
    PRT_TID();

    CMaintenance *handle = (CMaintenance *)org;

    handle->work_thread(org);
    return NULL;
}

//##ModelId=49BB33320013
int CMaintenance::StartMaintenanceTask()
{
	// TODO: Add your specialized code here.
	int status;

	pthread_attr_t new_attr;

	pthread_t thread_t;

	pthread_attr_init(&new_attr);
	pthread_attr_setdetachstate(&new_attr, PTHREAD_CREATE_DETACHED);

	CSYS_INF("创建工作线程\n");
	if (bWorkTrheadRun == 0)
	{
		bWorkTrheadRun = 1;
		status = pthread_create(&thread_t, &new_attr, MaintenanceWorkThread, this);

		if (status != 0)
		{
			CSYS_ERR("创建工作线程失败\n");
			return IDVR_STR_ERROR_DISK_THREAD_FAILED;
		}
	}
	CSYS_INF("创建工作线程成功\n");
	return 0;
}

//!\brief 设置维护回调函数
//!\praram void (*callback)() 回调函数指针
//!\return  成功为0，失败为-1
int CMaintenance::SetMaintenanceCallBack(void (*callback) ())
{
	m_CallBack = callback;
	return 0;
}

void *CMaintenance::work_thread(void *org)
{

	struct timespec ts;

	ts.tv_sec = 1;
	ts.tv_nsec = 0;
	int nSleepSum = 0;

	tagTime now;

	int Intervaltime = 0x7fffffff;

	int nTestSum = 0;

	while (bWorkTrheadRun)
	{
		nSleepSum = 0;

		CSYS_DBG("m_WorkMode = %d,m_MaintenanceMode = %d,m_bEnable = %d\n", m_WorkMode, m_MaintenanceMode, m_bEnable);
		//m_Time.print();

		//下面的睡眠方式，防止线程退出过慢的问题
		while ((nSleepSum++) < 1 /*需要扩大时使用 */ )
		{
			if (bWorkTrheadRun) /* 如果工作线程没有运行 */
			{
				ts.tv_sec  = 1;  /* 暂停的时间: 秒 */
				ts.tv_nsec = 0;  /* 暂停的时间: 毫秒 */
				nanosleep(&ts, NULL);    /* 延时1秒 */
			}
			else
			{
				nSleepSum = 0;
				break;
			}
		}

		if (m_WorkMode == 0 || m_bEnable == 0)
		{
			//没有定制或者没有启用
			//DEBUG_MSG("没有定制或者没有启用m_WorkMode = %d,m_bEnable = %d\n",m_WorkMode,m_bEnable);
			continue;
		}

		now.MkTag();  /* 将当前时间转化为结构体形式 */
		Intervaltime = 0x7fffffff; /* -1 */

        /* 用现在的时间秒减去预定的时间秒，其范围在0到10秒以内，则进行维护，
         * 即现在的时间超过预定的时间0~10s以内，则进行维护，这样可以控制误差 
         */
		if (m_MaintenanceMode == M_ONCE) 
		{
			//单次维护模式
			Intervaltime = now.MkSecond() - m_Time.MkSecond();
		}
		else if (m_MaintenanceMode == M_WEEK) /*  */
		{
			//周维护模式
			tagTime tmp;

			if ((1 << now.GetWeekDay()) & m_Week)
			{
			    /* 周维护不管年月日，只管时分秒，因为已经处于同一星期的同一天了 */
				memcpy(&tmp, &m_Time, sizeof (tagTime));

				tmp.year = now.year;
				tmp.mon = now.mon;
				tmp.day = now.day;

				Intervaltime = now.MkSecond() - tmp.MkSecond();
			}

		}
		else if (m_MaintenanceMode == M_DAY)
		{
			//每天维护模式
			tagTime tmp;
            /* 已经处于同一天的不同时间了，所以只需计算时分秒 */
			memcpy(&tmp, &m_Time, sizeof (tagTime));

			tmp.year = now.year;
			tmp.mon = now.mon;
			tmp.day = now.day;

			Intervaltime = now.MkSecond() - tmp.MkSecond();
		}
		else
		{
			CSYS_ERR("维护模式:%d错误\n", m_MaintenanceMode);
		}

		//cout<<"间隔时间:"<<Intervaltime<<endl;
		/* 现在时间秒 - 预定时间秒 = [0, 10)则进行维护 */
		if ((Intervaltime < 10) && (Intervaltime >= 0))
		{
			nTestSum++;
			CSYS_INF("检测到维护%d次\n", nTestSum);
			if (nTestSum == 1)
			{

				if (NULL != m_CallBack)
				{
					CSYS_INF("维护时间到，开始维护\n");
					m_LastTime.MkTag();          /* 最后一次维护的时间 */
					m_LastTime.print();
					WriteLastMaiantenanceTime(); /* 写最后一次维护的时间 */
					mysystem("date > systemerror_automaintain.txt");
					m_CallBack();                /* 回调 */
				}
				else
				{
					CSYS_WRAN("还没有设置回调\n");
					nTestSum = 0;
				}
			}
		}
		else
		{
			nTestSum = 0;
		}
	}
	return NULL;
}
