
/*!
 *\file		MPU_SYS_API.h
 *\brief	MPU模块，系统管理相关接口定义
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


#ifndef _INC_MPU_SYS_API_H_
#define _INC_MPU_SYS_API_H_


#include "HANDLE.h"
#include "tagTime.h"
#include "SYS/enumBuzzerType.h"
#include "SYS/CReminderSchedule.h"
#include "AVL/tagMotionSchedule.h"

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



/**************************************************************************
 *                            夏时制及时区                                      *
 **************************************************************************/

//!\brief       设置夏时制规则
//!\return  成功为0，失败为-1
int MPU_SYS_SetDST(LPDVR_DST_EX pDst);

//!\brief       获取夏时制规则
//!\return  成功为0，失败为-1
int MPU_SYS_GetDST(LPDVR_DST_EX pDst);

//!\brief       启用夏时制
//!\return  成功为0，失败为-1
int MPU_SYS_EnableDST(int enable);

//!\brief       获取是否启用夏时制
//!\return  成功为0，失败为-1
int MPU_SYS_IsEnableDST();

//!\brief 获取当前DST的偏移时间(秒)
//!\return 偏移(秒)
int MPU_SYS_GetDstBias();

//!\brief 获取当前DST是否已经工作(起效)
//!\return 0不工作(时间未到或者DST关闭) 1起效
int MPU_SYS_GetDstIsWork();

//!\brief       设置时区
//!\return  成功为0，失败为-1
int MPU_SYS_SetTimeZone(int nTimeZone);

//!\brief       获取时区
//!\return  成功为0，失败为-1
int MPU_SYS_GetTimeZone();

/**************************************************************************
 *                            NTP                                         *
 **************************************************************************/

//!\brief 设置ntp使能
//!\return  成功为0，失败为-1
int MPU_SYS_SetNtpEnable(int enable);

//!\brief 获取ntp使能
int MPU_SYS_GetNtpEnable();

//!\brief 设置ntp同步时间间隔
//!\return  成功为0，失败为-1
int MPU_SYS_SetNtpInterval(int nIntervalSec);

//!\brief 获取ntp同步间隔
int MPU_SYS_GetNtpInterval();

//!\brief 获取ntp同步成功标识
int MPU_SYS_GetNtpSuccessFlag();

//!\brief 设置ntp服务器
//!\return  成功为0，失败为-1
int MPU_SYS_SetNtpServer(const char *ntp_server);

//!\brief 获取ntp服务器
//!\return  成功为0，失败为-1
int MPU_SYS_GetNtpServer(char *ntp_server, int nbufLen);

int MPU_SYS_DoNtpSync();

int MPU_SYS_SetCheckInterNetStatusCallBack(int (*pCallBack) (int *)); 

/**************************************************************************
 *                            蜂鸣器                                      *
 **************************************************************************/

//!\brief   设置蜂鸣器工作
//!\param   enumBuzzerType enumBuzzer   蜂鸣类型、为NOBUZZER停止蜂鸣
//!\return  成功为0，失败为-1
int MPU_SYS_KeepBuzzer(enumBuzzerType enumBuzzer);

//!\brief   蜂鸣一次
//!\param  enumBuzzerType enumBuzzer   蜂鸣类型
//!\return  成功为0，失败为-1
int MPU_SYS_Buzzer(enumBuzzerType enumBuzzer);


//!\brief   led灯频率
//!\param  enumBuzzerType enumBuzzer   蜂鸣类型
//!\return  成功为0，失败为-1
int MPU_SYS_Led(int ledHZ);
/**************************************************************************
 *                            关机                                        *
 **************************************************************************/

//!\brief   重启设备
//!\return  成功为0，失败为-1
int MPU_SYS_Reboot_safe();

//!\brief 关闭设备
//!\return  成功为0，失败为-1
int MPU_SYS_Halt_safe();


/**************************************************************************
 *                            复位                                        *
 **************************************************************************/

//!\brief   验证系统板
//!\return  成功为0，失败为-1
int MPU_SYS_BoardAuthen();

//!\brief   设置复位检测回调函数
//!\param  void(*pCallBack)(void) 回调函数指针
//!\return  成功为0，失败为-1
int MPU_SYS_SetResumeCallBack(void (*pCallBack) (void));


/**************************************************************************
 *                            看门狗                                      *
 **************************************************************************/

//!\brief   添加一个看门狗任务
//!\param  char *szDogName     任务名称
//!\param  int nTimer          需要喂狗的时间。喂狗时间大于该时间可能会导致失败重启
//!\return  成功为该任务的句柄，失败为NULL
HANDLE MPU_SYS_AddDogTask(char *szDogName, int nTimer);

//!\brief   注销一个看门狗任务
//!\param  HANDLE handle       看门狗任务句柄
//!\return  成功为0，失败为-1
int MPU_SYS_DestroyDogTask(HANDLE handle);

//!\brief   喂狗操作
//!\param  HWND handle         看门狗任务句柄
//!\return  成功为0，失败为-1
int MPU_SYS_WatchDog(HANDLE handle);


/**************************************************************************
 *                            自动维护                                    *
 **************************************************************************/

//!\brief 设置维护回调函数
//!\praram void (*callback)() 回调函数指针
//!\return  成功为0，失败为-1
int MPU_SYS_Maintenance_SetCallBack(void (*callback) ());

//!\brief   设置维护配置模式
//!\param  int nMode           维护模式 0为每天、1为每周、2为单次
//!\return  成功为0，失败为-1
int MPU_SYS_Maintenance_SetMode(int nMode);

//!\brief   获取维护配置模式
//!\return  维护模式 0为每天、1为每周、2为单次
int MPU_SYS_Maintenance_GetMode();

//!\brief   启用/不启用自动维护
//!\param  BOOL nCustomize 0为取消定制，1为定制
//!\return  成功为0，失败为-1
int MPU_SYS_Maintenance_Enable(bool bEnable);

//!\brief   判断是否启用该功能
//!\return  0为无定制，1为定制
bool MPU_SYS_Maintenance_IsEnable();

//!\brief   获取上一次维护的时间
//!\param  tagTime* pTime      维护时间
//!\return  成功为0，失败为-1
int MPU_SYS_Maintenance_GetLastWorkTime(tagTime * pTime);

//!\brief       获取下一次维护的时间
//!\param   tagTime* ptime
//!\return  成功为0，失败为-1
int MPU_SYS_Maintenance_GetNextWorkTime(tagTime * pTime);

//!\brief 根据参数获取下一次维护时间
//!\praram int nMode 维护模式 0为每天、1为每周、2为单次
//!\praram tagTime time 时间点
//!\praram int WeekDay = 0 星期几
//!\return  成功为0，失败为-1
int MPU_SYS_Maintenance_GetNextWorkTime(tagTime * pTime, int nMode, tagTime T, int nWeekDay = 0);

//!\brief   设置星期启用
//!\param  int nWeekDay 0星期天 1星期1，2星期2...
//!\param  bool bEnable false 不启用，true启用
//!\return  成功为0，失败为-1
int MPU_SYS_Maintenance_EnableWeekDay(int nWeekDay, bool bEnable);

//!\brief   获取星期启用情况
//!\param  int nWeekDay 0星期天 1星期1，2星期2...
//!\return  false 不启用，true启用
bool MPU_SYS_Maintenance_IsEnableWeekDay(int nWeekDay);

//!\brief   设置维护配置时间
//!\param  tagTime tTime 维护时间
//!\return  成功为0，失败为-1
int MPU_SYS_Maintenance_SetMaintenanceTime(tagTime tTime);

//!\brief   获取维护配置时间
//!\param  tagTime *time 维护时间指针
//!\return  成功为0，失败为-1
int MPU_SYS_Maintenance_GetMaintenanceTime(tagTime * time);

//!\brief   定制/取消定制功能
//!\param  BOOL bCustomize FALSE为取消定制，TRUE为定制
//!\return  成功为0，失败为-1
int MPU_SYS_Maintenance_SetWorkMode(BOOL bCustomize);

//!\brief   获取是否定制/取消定制
//!\return  FALSE 为无定制，TRUE 为定制
BOOL MPU_SYS_Maintenance_GetWorkMode();


//!\brief   获取设备开机启动时间
//!\return  成功为0，失败为-1
int MPU_SYS_GetDeviceOpenTime(long long *deviceOpenTime);

void MPU_SYS_HardReset();

/**************************************************************************
 *                            升级                                        *
 **************************************************************************/

//!\brief       检测升级包是否合法
//!\param   const char *szPath 升级包路径
//!\return   合法为1，版本不对0，升级包错误为-1
int MPU_SYS_Update_CheckVersion(const char *szPath);

#if defined (__IS_SUPPORT_APP_SHARE__)
/**************************************************************************
 *                         设备绑定和解绑                                        *
 **************************************************************************/

//!\brief 	根据绑定状态获取S码
//!\return  成功为0，失败为-1
int MPU_SYS_DeviceBind_GetSCodeOfBindState(char *code, int *len);

//!\brief 	获取复位前的S码
//!\return  成功为0，失败为-1
int MPU_SYS_DeviceBind_GetSCodeBak(char *code, int *len);

//!\brief    获取设备的S码
//!\return   成功   0，失败 -1
int MPU_SYS_DeviceBind_GetSCode(char *code, int *len);

//!\brief       设置设备的S码
//!\return   成功   0，失败 -1
int MPU_SYS_DeviceBind_SetSCode(const char *code, int len);

//!\brief       复位设备的绑定状态
//!\return   成功   0，失败 -1
int MPU_SYS_DeviceBind_ResetSCode();

//!\brief 清除S码
int MPU_SYS_DeviceBind_ClearSCode();

//!\brief 根据DID生成SS码
int MPU_SYS_DeviceBind_CreateSCode(char *uid);
#endif

//!\brief 设置睡眠计划回调函数
//!\return  成功为0，失败为-1
int MPU_SYS_AwakeSchedule_SetCallBack(void (*callback) (int AwakeMode));

//!\brief   获取设备当前睡眠状态
//!\return  0为休眠，1为唤醒
bool MPU_SYS_AwakeSchedule_GetDevSleepStatus();

//!\brief   获取 立即睡眠 状态
int MPU_SYS_AwakeSchedule_GetDorEnable(int *bEnable);

//!\brief   启用/禁用立即休眠
int MPU_SYS_AwakeSchedule_DorEnable(int bEnable);

//!\brief   获取 立即睡眠 状态
int MPU_SYS_AwakeSchedule_GetScheduleEnable(int *bEnable);

//!\brief   启用/禁用睡眠计划
int MPU_SYS_AwakeSchedule_SetScheduleEnable(int bEnable);

//!\brief   获取休眠唤醒配置时间
//!\return  成功为0，失败为-1
int MPU_SYS_AwakeSchedule_GetAwakeScheduleTime(tagMotionSchedule *cfg);

//!\brief 设置睡眠唤醒配置时间
//!\return  成功为0，失败为-1
int MPU_SYS_AwakeSchedule_SetAwakeScheduleTime(tagMotionSchedule *cfg);

//!\brief 获取设备结束本次睡眠时间
//!\return  成功为0，失败为-1
int MPU_SYS_AwakeSchedule_GetAwakeScheduleEndTime(int *bValid, int *nday, int *nhour);

#if defined (__IS_SUPPORT_REMINDER_SCHEDULE__)

//!\brief 新增定时提醒
//!\return  成功为0，失败为-1
int MPU_SYS_RemindSchedule_AddReminder(const tagReminderItem* item);

//!\brief 删除定时提醒
//!\return  成功为0，失败为-1
int MPU_SYS_RemindSchedule_DeleteReminder(int nID);

//!\brief 修改定时提醒
//!\return  成功为0，失败为-1
int MPU_SYS_RemindSchedule_ModifyReminder(const tagReminderItem* item);

//!\brief 打断定时提醒
//!\return  成功为0，失败为-1
int MPU_SYS_RemindSchedule_InterruptReminder();

//!\brief 查询定时提醒
//!\return  成功为0，失败为-1
int MPU_SYS_RemindSchedule_GetReminder(int nID, tagReminderItem* item);

//!\brief 获取当前已有提醒数量
//!\return  成功为0，失败为-1
int MPU_SYS_RemindSchedule_GetCurReminderCount();

//!\brief获取最大提醒数量
//!\return  成功为0，失败为-1
int MPU_SYS_RemindSchedule_GetMaxReminderCount();

//!\brief 添加定时提醒自定义语音信息
//!\return  成功为0，失败为-1
int MPU_SYS_RemindSchedule_AddCustomSpeech(const tagCusSpeechInfo* item);

//!\brief 删除定时提醒自定义语音信息
//!\return  成功为0，失败为-1
int MPU_SYS_RemindSchedule_DeleteCustomSpeech(int nID);

//!\brief 获取定时提醒自定义语音信息
//!\return  成功为0，失败为-1
int MPU_SYS_RemindSchedule_GetCustomSpeech(int nID, tagCusSpeechInfo* item);

//!\brief 获取最大定时提醒自定义语音数量
//!\return  成功为0，失败为-1
int MPU_SYS_RemindSchedule_GetMaxCustomSpeechCount();

//!\brief 测试播放定时提醒语音
//!\return  成功为0，失败为-1
int MPU_SYS_RemindSchedule_TestPlaySpeech(int nType);
#endif

#endif							/* _INC_MPU_SYS_API_H_ */
