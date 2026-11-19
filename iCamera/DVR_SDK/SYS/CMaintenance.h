
/*!
*\file   CMaintenance.h
*\brief  维护功能的声明
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CMAINTENANCE_499CF127031D_INCLUDED
#define _INC_CMAINTENANCE_499CF127031D_INCLUDED

#include "tagTime.h"


/**************************************************************************
*                            常量                                        *
**************************************************************************/

/**************************************************************************
*                            宏                                          *
**************************************************************************/

/**************************************************************************
*                          数据类型                                       *
**************************************************************************/
//0为每天、1为每周、2为单次
enum
{
	M_DAY = 0,
	M_WEEK,
	M_ONCE
};

//##ModelId=499CF127031D
class CMaintenance
{
  public:


	//!\brief 根据参数获取下一次维护时间
	//!\praram int nMode 维护模式 0为每天、1为每周、2为单次
	//!\praram tagTime time 时间点
	//!\praram int WeekDay = 0 星期几
	//!\return  成功为0，失败为-1
	int GetNextMainTenanceTime(tagTime * Time, int nMode, tagTime T, int WeekDay = 0);

	//!\brief 设置维护回调函数
	//!\praram void (*callback)() 回调函数指针
	//!\return  成功为0，失败为-1
	int SetMaintenanceCallBack(void (*callback) ());

	//!\brief 设置维护配置模式
	//!\praram int nMode 维护模式 0为每天、1为每周、2为单次
	//!\return  成功为0，失败为-1
	//##ModelId=499CF1AD0121
	int SetMaintenanceMode(int nMode);

	//!\brief 获取维护配置模式
	//!\praram tagMaintenance *cfg 维护配置指针
	//!\return  维护模式 0为每天、1为每周、2为单次
	//##ModelId=499CF1C600A4
	int GetMaintenanceMode();

	//!\brief 定制/获取取消定制功能
	//!\praram BOOL nCustomize 0为取消定制，1为定制
	//!\return  成功为0，失败为-1
	//##ModelId=49B89FE201A4
	int SetWorkMode(bool nCustomize);

	//!\brief 定制/获取取消定制功能
	//!\praram BOOL nCustomize 0为取消定制，1为定制
	//!\return  false 为无定制，true 为定制
	//##ModelId=49BE0BCF0360
	bool GetWorkMode();


	//!\brief 判断是否启用该功能
	//!\praram 
	//!\return  0为无定制，1为定制
	//##ModelId=49B8A047020D
	bool IsEnable();

	//!\brief 禁用/启用
	//!\praram BOOL nCustomize 0为取消定制，1为定制
	//!\return  成功为0，失败为-1
	//##ModelId=49BE0BCF0388
	int Enable(bool bEnable);


	//!\brief 获取句柄
	//!\return  成功为为句柄，失败为NULL
	//##ModelId=49BB33310243
	static CMaintenance *GetInstance();

	//!\brief 销毁实例
	//!\praram CMaintenance *handle 句柄
	//!\return  成功为0，失败为-1
	//##ModelId=49BB3331024D
	static int ReleaseInstance();

	//!\brief 获取上一次维护的时间
	//!\praram tagTime* time
	//!\return  成功为0，失败为-1
	//##ModelId=49BB33310293
	int GetLastMainTenanceTime(tagTime * time);


	//!\brief 获取下一次维护的时间
	//!\praram tagTime* time
	//!\return  成功为0，失败为-1
	//##ModelId=49BB33310293
	int GetNextMainTenanceTime(tagTime * Time);

	//!\brief 设置星期启用
	//!\praram int nWeekDay 0星期天 1星期1，2星期2...
	//!\praram bool bEnable false 不启用，true启用
	//!\return  成功为0，失败为-1
	//##ModelId=49BE0BCF0157
	int EnableWeekDay(int nWeekDay, bool bEnable);

	//!\brief 获取星期启用情况
	//!\praram int nWeekDay 0星期天 1星期1，2星期2...
	//!\praram bool bEnable false 不启用，true启用
	//!\return  成功为0，失败为-1
	//##ModelId=49BE0BCF01F8
	bool IsEnableWeekDay(int nWeekDay);

	//!\brief 设置维护配置时间
	//!\praram tagTime time 维护时间。秒无效
	//!\return  成功为0，失败为-1
	//##ModelId=49BE0BCF0252
	int SetMaintenanceTime(tagTime time);

	//!\brief 获取维护配置时间
	//!\praram tagTime *time 维护时间指针
	//!\return  成功为0，失败为-1
	//##ModelId=49BE0BCF02D4
	int GetMaintenanceTime(tagTime * time);
  private:


	//!\brief 读取维护配置
	//!\return  成功为0，失败为-1
	int Read();

	//!\brief 读取最后一次维护时间
	//!\return  成功为0，失败为-1
	int ReadLastMaiantenanceTime();

	//!\brief 写入最后一次维护时间
	//!\return  成功为0，失败为-1
	int WriteLastMaiantenanceTime();

	//!\brief 设置维护配置
	//!\praram char *key 关键字
	//!\praramint value 值
	//!\return  成功为0，失败为-1
	int SetValue(const char *key, int value);

	//构造函数
	  CMaintenance();

	//!\brief 获取维护配置
	//!\praram char *key 关键字
	//!\praramint value 值
	//!\return  成功为0，失败为-1
	int GetValue(const char *key, int value);

	//工作线程是否正在运行
	int bWorkTrheadRun;

	//该功能是否定制
	int m_WorkMode;

	//维护模式
	int m_MaintenanceMode;

	//是否启用了该功能
	int m_bEnable;

	//星期
	int m_Week;

	//维护时间
	tagTime m_Time;
	time_t m_TimeSec;

	//上一次维护时间
	tagTime m_LastTime;

	//维护实例
	static CMaintenance *theCMaintenance;

	friend void *MaintenanceWorkThread(void *org);

	//开始维护线程
	int StartMaintenanceTask();

	void *work_thread(void *org);


	typedef void (*CallBack) ();

	//维护回调函数
	CallBack m_CallBack;
};

#endif							/* _INC_CMAINTENANCE_499CF127031D_INCLUDED */
