#ifndef __CAWAKE_SCHEDULE_H__
#define __CAWAKE_SCHEDULE_H__

#include "tagMotionSchedule.h"
#include "CActionAlarm.h"

class CAwakeSchedule
{
public:
    // 设置睡眠唤醒回调函数
	int SetAwakeScheduleCallBack(void (*callback) (int AwakMode));

    // 获取设备当前睡眠状态
	int GetDevSleepStatus();

    // 禁用/启用立即睡眠
	int SleepEnable(int bEnable);

    // 获取立即睡眠开关状态
    int GetSleepEnable(int *bEnable);

    // 禁用/启用睡眠计划
    int ScheduleCfgEnable(int bEnable);

    // 获取睡眠计划开关状态
    int GetScheduleCfgEnable(int *bEnable);

	// 获取句柄，成功为为句柄，失败为NULL
	static CAwakeSchedule *GetInstance();

	// 销毁实例，成功为0，失败为-1
	static int ReleaseInstance();

	// 设置休眠唤醒配置时间，成功为0，失败为-1
	int SetAwakeScheduleTime(const tagMotionSchedule * cfg);

	// 获取休眠唤醒配置时间，成功为0，失败为-1
	int GetAwakeScheduleTime(tagMotionSchedule * cfg);

    // 获取下次睡眠计划结束时间
    int GetAwakeScheduleEndTime(int *bValid, int *nday, int *nhour);

private:
	// 读取休眠唤醒配置，成功为0，失败为-1
	int Init();

	int GetValue(const char *key, int value);

	char * GetValue_Str(const char *key, char *value_str, int val_max_len);

	int SetValue(const char *key, int value);

	int SetValue_Str(const char *key, const char *val_str);

    int LoadParam();

    void ChangeMode(int mode);
    
    int CheckSchedule();

public:
    CAwakeSchedule();

	static CAwakeSchedule *theCAwakeSchedule;

	friend void *AwakeScheduleWorkThread(void *org);

	int StartAwakeScheduleTask();

	void *work_thread(void *org);

	typedef void (*CallBack) (int AwakeMode);

private:
	int m_bInited;			            //用于上电后第一次状态转换
	int m_bAwakeModuleReadyFlag;        //休眠模块是否准备就绪
	int m_bWorkTrheadRun;               //工作线程是否正在运行

	int m_bSleepEnable;		            //立即睡眠开关状态
	int m_bScheduleCfgEnable;           //睡眠计划是否打开
	int m_IsDevSleep;                   //是否处于睡眠状态

	tagMotionSchedule m_ScheduleCfg;    //睡眠计划
	CActionAlarm *m_pActionAlarm;
	CallBack m_CallBack;
};

#endif
