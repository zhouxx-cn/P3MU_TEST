
/*!
 *\file		MPU_SYS_API.cpp
 *\brief	MPU模块，系统管理相关接口实现
 *\author	林云
 *\date		2009/03/20

功能：
-#	支持蜂鸣器的管理与操作
-#	支持系统的关机及重启操作
-#	支持看门狗的管理
-#	提供升级功能相关的接口及进度信息的回调及获取
-#	提供维护的配置及业务
-#	密码复位
 */


#include "MPU_global.h"

#include "SYS/CSysManager.h"	//系统管理器
#include "SYS/CMaintenance.h"	//自动维护
#include "SYS/CUpdateManager.h"	//升级
#include "SYS/CAwakeSchedule.h"	//唤醒睡眠
#include "PUB/mysystem.h"
#include "AVL/COptics.h"

/**************************************************************************
 *                            声明局部宏定义                              *
 **************************************************************************/

#ifndef DEBUG_NOT_USE_SYS_LIB	//调试时，不使用SYS库

//调用系统管理器执行动作
#define MPU_SYS_CALL(x)									\
do														\
{														\
	int nRet;												\
														\
	if (!g_bShutdown)										\
	{													\
		CSysManager *pMgr = CSysManager::GetInstance();		\
														\
		if (NULL != pMgr)									\
		{												\
			nRet = pMgr->x;								\
			if (IDVR_OK != nRet)							\
			{											\
				MPU_ERR("%s, nRet = %d\n", #x, nRet);		\
			}											\
		}												\
		else												\
		{												\
			MPU_ERR("Get CSysManager instance fail!\n");		\
			MPU_CHK(NULL != pMgr);						\
			nRet = IDVR_MPU_SYS_NO_SYS_MANAGER_INSTANCE;	\
		}												\
	}													\
	else													\
	{													\
		MPU_ERR("in shutdown process\n");					\
		nRet = IDVR_MPU_IN_SHUTDOWN_PROCESS;				\
	}													\
														\
	return nRet;											\
}														\
while (0)


//调用系统管理器获取参数
#define MPU_SYS_GET(DataType, DefaultValue, Function)		\
do														\
{														\
	DataType retValue;										\
														\
	if (!g_bShutdown)										\
	{													\
		CSysManager *pMgr = CSysManager::GetInstance();		\
														\
		if (NULL != pMgr)									\
		{												\
			retValue = pMgr->Function;						\
		}												\
		else												\
		{												\
			MPU_ERR("Get CMaintenance instance fail!\n");		\
			MPU_CHK(NULL != pMgr);						\
			retValue = DefaultValue;						\
		}												\
	}													\
	else													\
	{													\
		MPU_ERR("in shutdown process\n");					\
		retValue = DefaultValue;							\
	}													\
														\
	return retValue;										\
}														\
while (0)


//调用自动维护管理器执行动作
#define MPU_MAINTENANCE_CALL(x)									\
do																\
{																\
	int nRet;														\
																\
	CMaintenance *pMgr = CMaintenance::GetInstance();					\
																\
	if (NULL != pMgr)												\
	{															\
		nRet = pMgr->x;											\
																\
		if (IDVR_OK != nRet)										\
		{														\
			MPU_ERR("%s, nRet = %d\n", #x, nRet);					\
		}														\
	}															\
	else															\
	{															\
		MPU_ERR("Get CMaintenance instance fail!\n");					\
		MPU_CHK(NULL != pMgr);									\
		nRet = IDVR_MPU_SYS_NO_MAINTENANCE_MANAGER_INSTANCE;		\
	}															\
																\
	return nRet;													\
}																\
while (0)

//调用自动维护管理器获取参数
#define MPU_MAINTENANCE_GET(DataType, DefaultValue, Function)		\
do																\
{																\
	DataType retValue;												\
																\
	CMaintenance *pMgr = CMaintenance::GetInstance();					\
																\
	if (NULL != pMgr)												\
	{															\
		retValue = pMgr->Function;									\
	}															\
	else															\
	{															\
		MPU_ERR("Get CMaintenance instance fail!\n");					\
		MPU_CHK(NULL != pMgr);									\
		retValue = DefaultValue;									\
	}															\
																\
	return retValue;												\
}																\
while (0)

//调用升级管理器执行动作
#define MPU_UPDATE_CALL(x)								\
do														\
{														\
	int nRet;												\
														\
	CUpdateManager *pMgr = CUpdateManager::GetInstance();		\
														\
	if (NULL != pMgr)										\
	{													\
		nRet = pMgr->x;									\
		if (IDVR_OK != nRet)								\
		{												\
			MPU_ERR("%s, nRet = %d\n", #x, nRet);			\
		}												\
	}													\
	else													\
	{													\
		MPU_ERR("Get CUpdateManager instance fail!\n");		\
		MPU_CHK(NULL != pMgr);							\
		nRet = IDVR_MPU_SYS_NO_UPDATE_MANAGER_INSTANCE;	\
	}													\
														\
	return nRet;											\
}														\
while (0)

//调用升级管理器获取参数
#define MPU_UPDATE_GET(DataType, DefaultValue, Function)		\
do														\
{														\
	DataType retValue;										\
														\
	CUpdateManager *pMgr = CUpdateManager::GetInstance();		\
														\
	if (NULL != pMgr)										\
	{													\
		retValue = pMgr->Function;							\
	}													\
	else													\
	{													\
		MPU_ERR("Get CUpdateManager instance fail!\n");		\
		MPU_CHK(NULL != pMgr);							\
		retValue = DefaultValue;							\
	}													\
														\
	return retValue;										\
}														\
while (0)

//调用休眠唤醒管理器执行动作
#define MPU_AWAKESCHEDULE_CALL(x)									\
do																\
{																\
	int nRet;														\
																\
	CAwakeSchedule *pMgr = CAwakeSchedule::GetInstance();					\
																\
	if (NULL != pMgr)												\
	{															\
		nRet = pMgr->x;											\
																\
		if (IDVR_OK != nRet)										\
		{														\
			MPU_ERR("%s, nRet = %d\n", #x, nRet);					\
		}														\
	}															\
	else															\
	{															\
		MPU_ERR("Get CAwakeSchedule instance fail!\n");					\
		MPU_CHK(NULL != pMgr);									\
		nRet = IDVR_MPU_SYS_NO_MAINTENANCE_MANAGER_INSTANCE;		\
	}															\
																\
	return nRet;													\
}																\
while (0)

//调用休眠唤醒管理器获取参数
#define MPU_AWAKESCHEDULE_GET(DataType, DefaultValue, Function)		\
do																\
{																\
	DataType retValue;												\
																\
	CAwakeSchedule *pMgr = CAwakeSchedule::GetInstance();				\
																\
	if (NULL != pMgr)												\
	{															\
		retValue = pMgr->Function;									\
	}															\
	else															\
	{															\
		MPU_ERR("Get CAwakeSchedule instance fail!\n");					\
		MPU_CHK(NULL != pMgr);									\
		retValue = DefaultValue;									\
	}															\
																\
	return retValue;												\
}																\
while (0)

//调用定时提醒管理器执行动作
#define MPU_REMINDSCHEDULE_CALL(x)									\
do																\
{																\
	int nRet;														\
																\
	CReminderSchedule *pMgr = CReminderSchedule::GetInstance();					\
																\
	if (NULL != pMgr)												\
	{															\
		nRet = pMgr->x;											\
																\
		if (IDVR_OK != nRet)										\
		{														\
			MPU_ERR("%s, nRet = %d\n", #x, nRet);					\
		}														\
	}															\
	else															\
	{															\
		MPU_ERR("Get CReminderSchedule instance fail!\n");					\
		MPU_CHK(NULL != pMgr);									\
		nRet = IDVR_MPU_SYS_NO_MAINTENANCE_MANAGER_INSTANCE;		\
	}															\
																\
	return nRet;													\
}																\
while (0)

//调用休眠唤醒管理器获取参数
#define MPU_REMINDSCHEDULE_GET(DataType, DefaultValue, Function)		\
do																\
{																\
	DataType retValue;												\
																\
	CReminderSchedule *pMgr = CReminderSchedule::GetInstance();				\
																\
	if (NULL != pMgr)												\
	{															\
		retValue = pMgr->Function;									\
	}															\
	else															\
	{															\
		MPU_ERR("Get CReminderSchedule instance fail!\n");					\
		MPU_CHK(NULL != pMgr);									\
		retValue = DefaultValue;									\
	}															\
																\
	return retValue;												\
}																\
while (0)

#else

//调用系统管理器执行动作
#define MPU_SYS_CALL(x)									\
do														\
{														\
	return IDVR_OK;										\
}														\
while (0)

//调用系统管理器获取参数
#define MPU_SYS_GET(DataType, DefaultValue, Function)		\
do														\
{														\
	return IDVR_OK;										\
}														\
while (0)


//调用自动维护管理器执行动作
#define MPU_MAINTENANCE_CALL(x)									\
do																\
{																\
	return IDVR_OK;												\
}																\
while (0)

//调用自动维护管理器获取参数
#define MPU_MAINTENANCE_GET(DataType, DefaultValue, Function)		\
do																\
{																\
	return DefaultValue;											\
}																\
while (0)

//调用升级管理器执行动作
#define MPU_UPDATE_CALL(x)								\
do														\
{														\
	return IDVR_OK;										\
}														\
while (0)

//调用升级管理器获取参数
#define MPU_UPDATE_GET(DataType, DefaultValue, Function)		\
do														\
{														\
	return DefaultValue;									\
}														\
while (0)

#define MPU_AWAKESCHEDULE_CALL(x)									\
do														\
{														\
	return IDVR_OK;										\
}														\
while (0)

#define MPU_AWAKESCHEDULE_GET(DataType, DefaultValue, Function)		\
do														\
{														\
	return DefaultValue;									\
}														\
while (0)

#define MPU_REMINDSCHEDULE_CALL(x)									\
do														\
{														\
	return IDVR_OK;										\
}														\
while (0)

#define MPU_REMINDSCHEDULE_GET(DataType, DefaultValue, Function)		\
do														\
{														\
	return DefaultValue;									\
}														\
while (0)

#endif							/* #ifndef DEBUG_NOT_USE_PRW_LIB */


/**************************************************************************
 *                            模块内部接口	                              *
 **************************************************************************/

extern void rsleep(unsigned int ntime);
//!\brief       复位密码
static void callback_Resume(void)
{
	int i = 0;

	// 播报复位提示音
#ifdef __MODULE_4G_SUPPORT__
    ZWSys_SetSpeechContent(SPEECH_4G_SYS_RESET);
#else
    ZWSys_SetSpeechContent(SPEECH_SYS_QRCODE_SCAN_OVER);
    //循环查询，等待播放结束
    while (1)
    {
        sleep(1);
        if (0 == ZWSys_GetSpeechPlayingStatus())
        {
            break;
        }
        if (++i > 10)
        {
            i = 0;
            break;
        }
    }

	i = 0;
    ZWSys_SetSpeechContent(SPEECH_SYS_RESET);
#endif
	//循环查询，等待播放结束
    while (1)
    {
        sleep(1);
        if (0 == ZWSys_GetSpeechPlayingStatus())
        {
            break;
        }
        if (++i > (10))
        {
            i = 0;
            break;
        }
    }

#if defined (__IS_SUPPORT_APP_SHARE__)
    // 解除设备绑定
    MPU_SYS_DeviceBind_ResetSCode();
#endif
	USR_ResumeAdminPassword();
	PAR_ForbiddenSaveFile();
	PAR_NET_Resume(__FUNCTION__);
	PAR_UnForbiddenSaveFile();
}

//!\brief      通知改时间
static void Callback_SDT(tagTime t)
{
	time_t tNewTime = t.MkSecond();

	MPU_PUB_SetSystemTime(tNewTime);
}


//!\brief   初始化系统管理
//!\return  0表示成功，其它为错误码
int SYS_Init()
{
#ifndef DEBUG_NOT_USE_SYS_LIB	//调试时，不使用SYS库
	CSysManager *pMgr = CSysManager::GetInstance();

	if (NULL != pMgr)
	{
		pMgr->SetResumeCallBack(callback_Resume);
		pMgr->SetDSTCallBack(Callback_SDT);
	}

	CMaintenance::GetInstance();
    CAwakeSchedule::GetInstance();
#if defined (__IS_SUPPORT_REMINDER_SCHEDULE__)
	CReminderSchedule::GetInstance();  
#endif
#endif

	COptics *pOptMgr = COptics::GetInstance(0 ,ZWSys_EncChannelOpen(0));

	if (NULL != pOptMgr)
	{
		pOptMgr->SetResumeOptCallBack(callback_Resume);
	}
	return 0;
}

//!\brief   逆初始化系统管理
//!\return  0表示成功，其它为错误码
int SYS_Uninit()
{
	MPU_DBG("enter\n");

#ifndef DEBUG_NOT_USE_SYS_LIB	//调试时，不使用SYS库
	CSysManager::ReleaseInstance();
	CMaintenance::ReleaseInstance();
#endif

	MPU_DBG("ok\n");

	return 0;
}


//!\brief 设置测试模式
//!\praram nTestMode 模式值 0-出厂模式， 1-工厂模式
//!\return  成功为0，失败为-1
int MPU_SYS_SetProductTestMode(int nTestMode)
{
	MPU_SYS_CALL(SetProductTestMode(nTestMode));

}

//!\brief 获取复位状态
//!\praram nStatus 状态值 0-没有复位 1-已经按下复位
//!\return  成功为0，失败为-1
int MPU_SYS_GetHardResetStatus(int *nStatus)
{
	MPU_SYS_CALL(GetHardResetStatus(nStatus));
}

//!\brief 清空硬复位状态
int MPU_SYS_ClearHardResetStatus()
{
	MPU_SYS_CALL(ClearHardResetStatus());
}


/**************************************************************************
 *                            夏时制                                      *
 **************************************************************************/

//!\brief       设置夏时制规则
//!\return  成功为0，失败为-1
int MPU_SYS_SetDST(LPDVR_DST_EX pDst)
{
	MPU_SYS_CALL(SetDST(pDst));
}

//!\brief       获取夏时制规则
//!\return  成功为0，失败为-1
int MPU_SYS_GetDST(LPDVR_DST_EX pDst)
{
	MPU_SYS_CALL(GetDST(pDst));
}

//!\brief       启用夏时制
//!\return  成功为0，失败为-1
int MPU_SYS_EnableDST(int enable)
{
	MPU_SYS_CALL(EnableDST(enable));
}

//!\brief       获取是否启用夏时制
//!\return  成功为0，失败为-1
int MPU_SYS_IsEnableDST()
{
	MPU_SYS_GET(int, 0, IsEnableDST());
}

//!\brief 获取当前DST的偏移时间(秒)
//!\return 偏移(秒)
int MPU_SYS_GetDstBias()
{
	MPU_SYS_GET(int, 0, GetDstBias());
}

//!\brief 获取当前DST是否已经工作(起效)
//!\return 0不工作(时间未到或者DST关闭) 1起效
int MPU_SYS_GetDstIsWork()
{
	MPU_SYS_GET(int, 0, GetDstIsWork());
}

//!\brief       设置时区
//!\param       int nTimeZone       时区ID
//!\return  成功为0，失败为-1
int MPU_SYS_SetTimeZone(int nTimeZone)
{
	MPU_SYS_CALL(SetTimeZone(nTimeZone));
}

//!\brief       获取时区
//!\return  成功返回时区ID，失败为0
int MPU_SYS_GetTimeZone()
{
	MPU_SYS_GET(int, 0, GetTimeZone());
}

/**************************************************************************
 *                            NTP                                         *
 **************************************************************************/
//!\brief 设置ntp使能
//!\return  成功为0，失败为-1
int MPU_SYS_SetNtpEnable(int enable)
{
    MPU_SYS_CALL(SetNtpEnable(enable));
}

//!\brief 获取ntp使能
int MPU_SYS_GetNtpEnable()
{
    MPU_SYS_GET(int, 0, GetNtpEnable());
}

//!\brief 设置ntp同步时间间隔
//!\return  成功为0，失败为-1
int MPU_SYS_SetNtpInterval(int nIntervalSec)
{
    MPU_SYS_CALL(SetNtpInterval(nIntervalSec));
}

//!\brief 获取ntp同步间隔
int MPU_SYS_GetNtpInterval()
{
    MPU_SYS_GET(int, 0, GetNtpInterval());
}

//!\brief 获取ntp同步成功标识
int MPU_SYS_GetNtpSuccessFlag()
{
    MPU_SYS_GET(int, 0, GetNtpSuccessFlag());
}

//!\brief 设置ntp服务器
//!\return  成功为0，失败为-1
int MPU_SYS_SetNtpServer(const char *ntp_server)
{
    MPU_SYS_CALL(SetNtpServer(ntp_server));
}

//!\brief 获取ntp服务器
//!\return  成功为0，失败为-1
int MPU_SYS_GetNtpServer(char *ntp_server, int nbufLen)
{
    MPU_SYS_CALL(GetNtpServer(ntp_server, nbufLen));
}

int MPU_SYS_DoNtpSync()
{
    MPU_SYS_CALL(DoNtpSync());
}

int MPU_SYS_SetCheckInterNetStatusCallBack(int (*pCallBack) (int *))
{
    MPU_SYS_CALL(SetCheckInterNetStatusCallBack(pCallBack));
}

/**************************************************************************
 *                            蜂鸣器                                      *
 **************************************************************************/

//!\brief 设置蜂鸣器工作
//!\param enumBuzzerType enumBuzzer    蜂鸣类型、为NOBUZZER停止蜂鸣
//!\return  成功为0，失败为-1
//##ModelId=49BB33310103
int MPU_SYS_KeepBuzzer(enumBuzzerType enumBuzzer)
{
	MPU_SYS_CALL(KeepBuzzer(enumBuzzer));
}

//!\brief   蜂鸣一次
//!\param  enumBuzzerType enumBuzzer   蜂鸣类型
//!\return  成功为0，失败为-1
int MPU_SYS_Buzzer(enumBuzzerType enumBuzzer)
{
	//调用系统管理器执行动作
	MPU_SYS_CALL(Buzzer(enumBuzzer));
}

//!\brief   led灯频率
//!\param  enumBuzzerType enumBuzzer   蜂鸣类型
//!\return  成功为0，失败为-1
int MPU_SYS_Led(int ledHZ)
{
	//调用系统管理器执行动作
	MPU_SYS_CALL(Led(ledHZ));
}

/**************************************************************************
 *                            关机                                        *
 **************************************************************************/

//!\brief   重启设备
//!\return  成功为0，失败为-1
int MPU_SYS_Reboot_safe()
{
	//return mysystem("reboot_safe");
	return mysystem("reboot");//(hw)
}

//!\brief 关闭设备
//!\return  成功为0，失败为-1
int MPU_SYS_Halt_safe()
{
	//return mysystem("halt_safe");
	return mysystem("halt");//(hw)
}


/**************************************************************************
 *                            复位                                        *
 **************************************************************************/

//!\brief   验证系统板
//!\return  成功为0，失败为-1
int MPU_SYS_BoardAuthen()
{
	//调用系统管理器执行动作
	MPU_SYS_CALL(BoardAuthen());
}

//!\brief   设置复位检测回调函数
//!\param  void(*pCallBack)(void) 回调函数指针
//!\return  成功为0，失败为-1
int MPU_SYS_SetResumeCallBack(void (*pCallBack) (void))
{
	//调用系统管理器执行动作
	MPU_SYS_CALL(SetResumeCallBack(pCallBack));
}


/**************************************************************************
 *                            看门狗                                      *
 **************************************************************************/

//!\brief   添加一个看门狗任务
//!\param  char *pszDogName    任务名称
//!\param  int nTimer          需要喂狗的时间。喂狗时间大于该时间可能会导致失败重启
//!\return  成功为该任务的句柄，失败为NULL
HANDLE MPU_SYS_AddDogTask(char *pszDogName, int nTimer)
{
	MPU_SYS_GET(HANDLE, NULL, AddDogTask(pszDogName, nTimer));
}

//!\brief   注销一个看门狗任务
//!\param  HANDLE handle       看门狗任务句柄
//!\return  成功为0，失败为-1
int MPU_SYS_DestroyDogTask(HANDLE handle)
{
	//调用系统管理器执行动作
	MPU_SYS_CALL(DestroyDogTask(handle));
}

//!\brief   喂狗操作
//!\param  HWND handle         看门狗任务句柄
//!\return  成功为0，失败为-1
int MPU_SYS_WatchDog(HANDLE handle)
{
	//调用系统管理器执行动作
	MPU_SYS_CALL(WatchDog(handle));
}


/**************************************************************************
 *                            自动维护                                    *
 **************************************************************************/

//!\brief 设置维护回调函数
//!\praram void (*callback)() 回调函数指针
//!\return  成功为0，失败为-1
int MPU_SYS_Maintenance_SetCallBack(void (*callback) ())
{
	MPU_MAINTENANCE_GET(BOOL, FALSE, SetMaintenanceCallBack(callback));
}

//!\brief   设置维护配置模式
//!\param  int nMode           维护模式 0为每天、1为每周、2为单次
//!\return  成功为0，失败为-1
int MPU_SYS_Maintenance_SetMode(int nMode)
{
	//调用自动维护管理器执行动作
	MPU_MAINTENANCE_CALL(SetMaintenanceMode(nMode));
}

//!\brief   获取维护配置模式
//!\return  维护模式 0为每天、1为每周、2为单次
int MPU_SYS_Maintenance_GetMode()
{
	//调用自动维护管理器获取参数
	MPU_MAINTENANCE_GET(int, 0, GetMaintenanceMode());
}

//!\brief   启用/不启用自动维护
//!\param  BOOL nCustomize 0为取消定制，1为定制
//!\return  成功为0，失败为-1
int MPU_SYS_Maintenance_Enable(bool bEnable)
{
	//调用自动维护管理器执行动作
	MPU_MAINTENANCE_CALL(Enable(bEnable));
}

//!\brief   判断是否启用该功能
//!\return  0为无定制，1为定制
bool MPU_SYS_Maintenance_IsEnable()
{
	//调用自动维护管理器获取参数
	MPU_MAINTENANCE_GET(bool, false, IsEnable());
}

//!\brief   获取上一次维护的时间
//!\param  tagTime* pTime      维护时间
//!\return  成功为0，失败为-1
int MPU_SYS_Maintenance_GetLastWorkTime(tagTime * pTime)
{
	//调用自动维护管理器执行动作
	MPU_MAINTENANCE_CALL(GetLastMainTenanceTime(pTime));
}

//!\brief       获取下一次维护的时间
//!\param   tagTime* pTime
//!\return  成功为0，失败为-1
int MPU_SYS_Maintenance_GetNextWorkTime(tagTime * pTime)
{
	MPU_MAINTENANCE_CALL(GetNextMainTenanceTime(pTime));
}

//!\brief 根据参数获取下一次维护时间
//!\praram int nMode 维护模式 0为每天、1为每周、2为单次
//!\praram tagTime time 时间点
//!\praram int WeekDay = 0 星期几
//!\return  成功为0，失败为-1
int MPU_SYS_Maintenance_GetNextWorkTime(tagTime * pTime, int nMode, tagTime T, int nWeekDay)
{
	MPU_MAINTENANCE_CALL(GetNextMainTenanceTime(pTime, nMode, T, nWeekDay));
}

//!\brief   设置星期启用
//!\param  int nWeekDay 0星期天 1星期1，2星期2...
//!\param  bool bEnable false 不启用，true启用
//!\return  成功为0，失败为-1
int MPU_SYS_Maintenance_EnableWeekDay(int nWeekDay, bool bEnable)
{
	//调用自动维护管理器执行动作
	MPU_MAINTENANCE_CALL(EnableWeekDay(nWeekDay, bEnable));
}

//!\brief   获取星期启用情况
//!\param  int nWeekDay 0星期天 1星期1，2星期2...
//!\return  false 不启用，true启用
bool MPU_SYS_Maintenance_IsEnableWeekDay(int nWeekDay)
{
	//调用自动维护管理器获取参数
	MPU_MAINTENANCE_GET(bool, false, IsEnableWeekDay(nWeekDay));
}

//!\brief 设置维护配置时间
//!\param tagTime time 维护时间
//!\return  成功为0，失败为-1
int MPU_SYS_Maintenance_SetMaintenanceTime(tagTime tTime)
{
	//调用自动维护管理器执行动作
	MPU_MAINTENANCE_CALL(SetMaintenanceTime(tTime));
}

//!\brief   获取维护配置时间
//!\param  tagTime *ptTime 维护时间指针
//!\return  成功为0，失败为-1
int MPU_SYS_Maintenance_GetMaintenanceTime(tagTime * ptTime)
{
	//调用自动维护管理器执行动作
	MPU_MAINTENANCE_CALL(GetMaintenanceTime(ptTime));
}

//!\brief   定制/取消定制功能
//!\param  BOOL bCustomize false为取消定制，true为定制
//!\return  成功为0，失败为-1
int MPU_SYS_Maintenance_SetWorkMode(BOOL bCustomize)
{
	//调用自动维护管理器执行动作
	MPU_MAINTENANCE_CALL(SetWorkMode(bCustomize));
}

//!\brief   获取是否定制/取消定制
//!\return  FALSE 为无定制，TRUE为定制
BOOL MPU_SYS_Maintenance_GetWorkMode()
{
	//调用自动维护管理器获取参数
	MPU_MAINTENANCE_GET(BOOL, FALSE, GetWorkMode());
}


/**************************************************************************
 *                            升级                                        *
 **************************************************************************/

//!\brief       检测升级包是否合法
//!\param   const char *szPath 升级包路径
//!\return   合法为1，版本不对0，升级包错误为-1
int MPU_SYS_Update_CheckVersion(const char *szPath)
{
	return CUpdateManager::CheckVersion(szPath);
}

#if defined (__IS_SUPPORT_APP_SHARE__)
int MPU_SYS_DeviceBind_GetSCodeOfBindState(char *code, int *len)
{
    MPU_SYS_CALL(GetSCodeOfBindState(code, len));
}

//!\brief 获取复位前的S码
//!\return  成功为0，失败为-1
int MPU_SYS_DeviceBind_GetSCodeBak(char *code, int *len)
{
    MPU_SYS_CALL(GetSCodeBak(code, len));
}

int MPU_SYS_DeviceBind_GetSCode(char *code, int *len)
{
    MPU_SYS_CALL(GetSCode(code, len));
}

int MPU_SYS_DeviceBind_SetSCode(const char *code, int len)
{
    MPU_SYS_CALL(SetSCode(code, len));
}

int MPU_SYS_DeviceBind_ResetSCode()
{
    MPU_SYS_CALL(ResetSCode());
}

//!\brief 清除S码
int MPU_SYS_DeviceBind_ClearSCode()
{
    MPU_SYS_CALL(ClearSCode());
}

//!\brief 根据DID生成SS码
int MPU_SYS_DeviceBind_CreateSCode(char *uid)
{
    MPU_SYS_CALL(CreateSCode(uid));
}
#endif

//获取设备开机启动时间
int MPU_SYS_GetDeviceOpenTime(long long *deviceOpenTime)
{
	MPU_SYS_GET(int, 0,  GetDeviceOpenTime(deviceOpenTime));
}

//!\brief 设置唤醒计划回调函数
//!\praram void (*callback)(int AwakeMode) 回调函数指针
//!\return  成功为0，失败为-1
int MPU_SYS_AwakeSchedule_SetCallBack(void (*callback) (int AwakeMode))
{
	MPU_AWAKESCHEDULE_GET(BOOL, FALSE, SetAwakeScheduleCallBack(callback));
}

//!\brief   获取设备当前睡眠状态
//!\return  0为睡眠，1为唤醒
bool MPU_SYS_AwakeSchedule_GetDevSleepStatus()
{
	MPU_AWAKESCHEDULE_GET(BOOL, FALSE, GetDevSleepStatus());
}

//!\brief   获取 立即睡眠 状态
int MPU_SYS_AwakeSchedule_GetDorEnable(int *bEnable)
{
	MPU_AWAKESCHEDULE_CALL(GetSleepEnable(bEnable));
}

//!\brief   启用/关闭 立即睡眠
int MPU_SYS_AwakeSchedule_DorEnable(int bEnable)
{
	MPU_AWAKESCHEDULE_CALL(SleepEnable(bEnable));
}

//!\brief   获取 睡眠计划 状态
int MPU_SYS_AwakeSchedule_GetScheduleEnable(int *bEnable)
{
	MPU_AWAKESCHEDULE_CALL(GetScheduleCfgEnable(bEnable));
}

//!\brief   启用/关闭 睡眠计划
int MPU_SYS_AwakeSchedule_SetScheduleEnable(int bEnable)
{
	MPU_AWAKESCHEDULE_CALL(ScheduleCfgEnable(bEnable));
}

//!\brief   获取休眠唤醒配置时间
//!\return  成功为0，失败为-1
int MPU_SYS_AwakeSchedule_GetAwakeScheduleTime(tagMotionSchedule *cfg)
{
	MPU_AWAKESCHEDULE_CALL(GetAwakeScheduleTime(cfg));
}

//!\brief 设置睡眠唤醒配置时间
//!\return  成功为0，失败为-1
int MPU_SYS_AwakeSchedule_SetAwakeScheduleTime(tagMotionSchedule *cfg)
{
	MPU_AWAKESCHEDULE_CALL(SetAwakeScheduleTime(cfg));
}

//!\brief 获取设备结束本次睡眠时间
//!\return  成功为0，失败为-1
int MPU_SYS_AwakeSchedule_GetAwakeScheduleEndTime(int *bValid, int *nday, int *nhour)
{
    MPU_AWAKESCHEDULE_CALL(GetAwakeScheduleEndTime(bValid, nday, nhour));
}

#if defined (__IS_SUPPORT_REMINDER_SCHEDULE__)
//!\brief 新增定时提醒
//!\return  成功为0，失败为-1
int MPU_SYS_RemindSchedule_AddReminder(const tagReminderItem* item)
{
    MPU_REMINDSCHEDULE_CALL(AddReminder(item));
}

//!\brief 删除定时提醒
//!\return  成功为0，失败为-1
int MPU_SYS_RemindSchedule_DeleteReminder(int nID)
{
    MPU_REMINDSCHEDULE_CALL(DeleteReminder(nID));
}

//!\brief 修改定时提醒
//!\return  成功为0，失败为-1
int MPU_SYS_RemindSchedule_ModifyReminder(const tagReminderItem* item)
{
    MPU_REMINDSCHEDULE_CALL(ModifyReminder(item));
}

//!\brief 打断定时提醒
//!\return  成功为0，失败为-1
int MPU_SYS_RemindSchedule_InterruptReminder()
{
    MPU_REMINDSCHEDULE_GET(int, 0, InterruptReminder());
}

//!\brief 查询定时提醒
//!\return  成功为0，失败为-1
int MPU_SYS_RemindSchedule_GetReminder(int nID, tagReminderItem* item)
{
    MPU_REMINDSCHEDULE_GET(int, -1, GetReminder(nID,item));
}

//!\brief获取当前已有提醒数量
//!\return  成功为0，失败为-1
int MPU_SYS_RemindSchedule_GetCurReminderCount()
{
    MPU_REMINDSCHEDULE_GET(int, 0, GetCurReminderCount());
}

//!\brief获取最大支持提醒数量
//!\return  成功为0，失败为-1
int MPU_SYS_RemindSchedule_GetMaxReminderCount()
{
    MPU_REMINDSCHEDULE_GET(int, 0, GetMaxReminderCount());
}

//!\brief 添加定时提醒自定义语音信息
//!\return  成功为0，失败为-1
int MPU_SYS_RemindSchedule_AddCustomSpeech(const tagCusSpeechInfo* item)
{
    MPU_REMINDSCHEDULE_CALL(AddCustomSpeech(item));
}

//!\brief 删除定时提醒自定义语音信息
//!\return  成功为0，失败为-1
int MPU_SYS_RemindSchedule_DeleteCustomSpeech(int nID)
{
    MPU_REMINDSCHEDULE_CALL(DeleteCustomSpeech(nID));
}

//!\brief 获取定时提醒自定义语音信息
//!\return  成功为0，失败为-1
int MPU_SYS_RemindSchedule_GetCustomSpeech(int nID, tagCusSpeechInfo* item)
{
    MPU_REMINDSCHEDULE_GET(int, 0, GetCustomSpeech(nID, item));
}

//!\brief 获取最大定时提醒自定义语音数量
//!\return  成功为0，失败为-1
int MPU_SYS_RemindSchedule_GetMaxCustomSpeechCount()
{
    MPU_REMINDSCHEDULE_GET(int, 0, GetMaxCustomSpeechCount());
}

//!\brief 测试播放定时提醒语音
//!\return  成功为0，失败为-1
int MPU_SYS_RemindSchedule_TestPlaySpeech(int nType)
{
    MPU_REMINDSCHEDULE_CALL(TestPlaySpeech(nType));
}
#endif


/**************************************************************************
 *                            注销局部宏定义                              *
 **************************************************************************/
#undef MPU_SYS_CALL				//调用系统管理器接口执行动作
#undef MPU_SYS_GET				//调用系统管理器接口获取参数
#undef MPU_MAINTENANCE_CALL		//调用自动维护管理器接口执行动作
#undef MPU_MAINTENANCE_GET		//调用自动维护管理器接口获取参数
#undef MPU_UPDATE_CALL			//调用升级管理器接口执行动作
#undef MPU_UPDATE_GET			//调用升级维护管理器接口获取参数
#undef MPU_AWAKESCHEDULE_CALL   //调用唤醒休眠管理器接口执行动作
#undef MPU_AWAKESCHEDULE_GET	//调用唤醒休眠维护管理器接口获取参数
#undef MPU_REMINDSCHEDULE_CALL	//调用定时提醒管理器接口执行动作
#undef MPU_REMINDSCHEDULE_GET	//调用定时提醒维护管理器接口获取参数
