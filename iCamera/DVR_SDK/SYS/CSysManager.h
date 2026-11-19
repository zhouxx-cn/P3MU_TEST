
/*!
*\file   CSysManager.h
*\brief  系统管理的声明
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/


#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CSysManager_49B637B10259_INCLUDED
#define _INC_CSysManager_49B637B10259_INCLUDED

#include "enumBuzzerType.h"
#include "HANDLE.h"
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

#ifndef NO_USE_BUZZER
class CBuzzer;
#endif

//class CSysStart;

class CIP2PasswdResume;

class CMaintenance;

class CWatchDog;

class CBoardAuthen;


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

typedef void (*DSTCallBack) (tagTime t);

//##ModelId=49B637B10259
class CSysManager
{
  public:
	//!\brief 获取系统管理句柄
	//!\return  成功为为句柄，失败为NULL
	//##ModelId=49BB333100F8
	static CSysManager *GetInstance();

	//!\brief 销毁实例
	//!\return  成功为0，失败为-1
	//##ModelId=49BB333100FA
	static int ReleaseInstance();

	//!\brief 设置蜂鸣器蜂鸣持续时间
	//!\praram int times 蜂鸣器持续蜂鸣时间.0为持续蜂鸣
	//!\return  成功为0，失败为-1
	int SetBuzzerTime(unsigned int times);

	//!\brief 获取蜂鸣器蜂鸣持续时间
	//!\praram int times 蜂鸣器持续蜂鸣时间.0为持续蜂鸣
	//!\return  成功为0，失败为-1
	int GetBuzzerTime();

	//!\brief 设置蜂鸣器工作
	//!\praram enumBuzzerType enumBuzzer 蜂鸣类型、为NOBUZZER停止蜂鸣
	//!\return  成功为0，失败为-1
	//##ModelId=49BB33310103
	int KeepBuzzer(enumBuzzerType enumBuzzer);

	//!\brief 蜂鸣一次
	//!\praram enumBuzzerType enumBuzzer 蜂鸣类型
	//!\return  成功为0，失败为-1
	//##ModelId=49BB33310106
	int Buzzer(enumBuzzerType enumBuzzer);

	//!\brief 重启设备
	//!\praram void
	//!\return  成功为0，失败为-1
	//##ModelId=49BB3331010C
	int Reboot_safe();

	//!\brief 关闭设备
	//!\praram void
	//!\return  成功0，失败-1
	//##ModelId=49BB3331010D
	int Halt_safe();

	//!\brief 验证系统板
	//!\praram 无
	//!\return  成功为0，失败为-1
	//##ModelId=49BB3331010E
	int BoardAuthen(void);


	//!\brief 开始复位检测
	//!\praram void(*CallBack)(void) 回调函数指针
	//!\return  成功为0，失败为-1
	//##ModelId=49BB33310116
	int SetResumeCallBack(void (*CallBack) (void));

	//!\brief 设置测试模式
	//!\praram nTestMode 模式值 0-出厂模式， 1-工厂模式
	//!\return  成功为0，失败为-1
	int SetProductTestMode(int nTestMode);
	
	//!\brief 获取复位状态
	//!\praram nStatus 状态值 0-没有复位 1-已经按下复位
	//!\return  成功为0，失败为-1
	int GetHardResetStatus(int *nStatus);

	//!\brief 清空硬复位状态
	int  ClearHardResetStatus();
	
	//!\brief 添加一个看门狗任务
	//!\praram char *szDogName 看门狗名字
	//!\praram int nTimer 
	//需要喂狗的时间。喂狗时间大于该时间可能会导致失败重启
	//!\return  成功为该任务的句柄，失败为INVALID_HANDLE
	//##ModelId=49BB33310118
	HANDLE AddDogTask(const char *szDogName, int nTimer);

	//!\brief 注销一个看门狗任务
	//!\praram HWND handle 看门狗任务句柄
	//!\return  成功为0，失败为-1
	//##ModelId=49BB33310120
	int DestroyDogTask(HANDLE handle);

	//!\brief 喂狗操作
	//!\praram HWND handle 看门狗任务句柄
	//!\return  成功为0，失败为-1
	//##ModelId=49BB33310122
	int WatchDog(HANDLE handle);

	//!\brief 设置夏时制规则
	//!\return  成功为0，失败为-1
	int SetDST(LPDVR_DST_EX Dst);

	//!\brief 获取夏时制规则
	//!\return  成功为0，失败为-1
	int GetDST(LPDVR_DST_EX Dst);

	//!\brief 启用夏时制
	//!\return  成功为0，失败为-1
	int EnableDST(unsigned int enable);

	//!\brief 获取是否启用夏时制
	//!\return  成功为0，失败为-1
	int IsEnableDST();

    //!\brief 获取当前DST的偏移时间(秒)
    //!\return 偏移(秒)
    int GetDstBias();

	//!\brief 获取当前DST是否已经工作(起效)
    //!\return 0不工作(时间未到或者DST关闭) 1起效
    int GetDstIsWork();

	//!\brief 设置时区
	//!\return  成功为0，失败为-1
	int SetTimeZone(int nTimeZone);

	//!\brief 设置时区
	//!\return  成功为0，失败为-1
	int GetTimeZone();

	//!\brief 设置启用夏时制时的回调函数
	//!\return  成功为0，失败为-1
	//##ModelId=49BB33310122
	int SetDSTCallBack(DSTCallBack Cb);
	
	int Led(int ledHZ);

	//!\brief 设置ntp使能
	//!\return  成功为0，失败为-1
	int SetNtpEnable(int enable);

	//!\brief 获取ntp使能
	int GetNtpEnable();

	//!\brief 设置ntp同步时间间隔
	//!\return  成功为0，失败为-1
	int SetNtpInterval(int nIntervalSec);

	//!\brief 获取ntp同步间隔
	int GetNtpInterval();

	//!\brief 设置ntp服务器
	//!\return  成功为0，失败为-1
	int SetNtpServer(const char *ntp_server);

	//!\brief 获取ntp服务器
	//!\return  成功为0，失败为-1
	int GetNtpServer(char *ntp_server, int nbufLen);

	int DoNtpSync();
    
	//!\brief 设置ntp同步成功标识
	void SetNtpSuccessFlag(int flag);

	//!\brief 获取ntp同步成功标识
	int GetNtpSuccessFlag();

    // 设置检测当前互联网状态回调函数
    int SetCheckInterNetStatusCallBack(int (*CallBack)(int *));

#if defined (__IS_SUPPORT_APP_SHARE__)
	// 初始化设备S码状态
    int InitDevSCodeState();
    // 根据绑定状态获取S码
    int GetSCodeOfBindState(char *code, int *codeLen);
    // 获取复位前的S码
    int GetSCodeBak(char *code, int *codeLen);
    // 获取S码
    int GetSCode(char *code, int *codeLen);
    // 设置S码
    int SetSCode(const char *code, int codeLen);
    //!\brief 清除S码
	int ClearSCode();
    // 设备复位,清除绑定状态
    int ResetSCode();
    //!\brief 根据DID生成SS码
	int CreateSCode(char *uid);

	int m_GetSCodeValid;
#endif

	//ntp自动同步时间间隔
	int m_nIntervalSec;//单位，小时

	//获取设备启动时间
	int GetDeviceOpenTime(long long *deviceOpenTime);

	//ntp是否使能
	int m_ntp_enable;

	//如时间区，ntp使能，ntp服务器，间隔等的状态改变
	int m_configChanged;

    int m_TimeCountFlag;//开始计时标识
    int m_TimeToSyncFlag;//计时时间到标识
  
    int m_net_has_connected;
	int m_CheckNetThreadExitFlag;//检测互联网线程退出标志
    int m_ntp_request_is_success;
  private:

	//!\brief   开始复位检测
	//!\praram  bool bStart 是否
	//!\return  成功为0，失败为-1
	//##ModelId=49BB33310110
	int SetResumeTask(bool bStart);

	//!\brief 开始蜂鸣器工作任务
	//!\praram void
	//!\return  成功为0，失败为-1
	//##ModelId=49BB33310102
	int StartBuzzerTask();
	


	static void EDVR_CallBack(time_t t);

	static DSTCallBack DstCb;

	static CSysManager *theCSysManager;
	//##ModelId=49B63B77026C
#ifndef NO_USE_BUZZER
	CBuzzer *theCBuzzer;
#endif

	//##ModelId=49B63B7C01B5
//	CSysStart *theCSysStart;

	//##ModelId=49B63B8A01F1
	CIP2PasswdResume *theCIP2PasswdResume;

	//##ModelId=49BB33310135
	CWatchDog *theCWatchDog;

	CBoardAuthen *theCBoardAuthen;	

    int m_timezone;//时间区


	//ntp服务器地址
	char m_ntp_server[256];

	//自动对时
	pthread_t m_autopNtp;
	//计时器线程ID
	pthread_t m_ntp_TimethrdID;

	//检测互联网连通线程ID
	pthread_t m_CheckInternetthreadID;

#if defined (__IS_SUPPORT_APP_SHARE__)
	//s_code操作锁
	pthread_mutex_t m_MutexSCode;
#endif

	CSysManager();
	~CSysManager();

};

#endif							/* _INC_CSysManager_49B637B10259_INCLUDED */
