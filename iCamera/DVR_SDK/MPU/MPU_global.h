
/*!
 *\file		MPU_global.h
 *\brief	MPU模块内部使用全局宏、结构、变量、函数定义
 *\author	林云
 *\date		2009/03/19
 */

#ifndef _INC_MPU_GLOBAL_H_
#define _INC_MPU_GLOBAL_H_

#pragma once


//标准库头文件
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <assert.h>

//全局头文件
#include "MPU_API.h"			//导出接口
#include "idvr_errno.h"			//错误码
#include "global_macro.h"		//全工程宏定义

//模块内部头文件
#include "MPU_debug.h"			//调试工具

//其它工程头文件
#include "PAR/CConfig.h"		//配置文件访问
//#include "hisdk.h"				//系统SDK
#include "zwsdk.h"
#include "newhie/HieServer.h"	//网络SDK
#include "newhie/ZDebug.h"		//网络SDK

#include "global_gather.h"
/**************************************************************************
 *                        结构体定义                                            *
 **************************************************************************/
typedef struct tag_MPU_Manager_Info
{
	CBAlert m_pCallback_AlarmIn;
	CBAlert m_pCallback_Motion;
	CBAlert m_pCallback_HumanShape;
    CBAlert m_pCallback_Sound;
	CBAlert m_pCallback_VideoLost;
	CBAlert m_pCallback_EncodeError;
	CBAlert m_pCallback_VideoException;
	CBAlert m_pCallback_DiskFull;
	CBAlert m_pCallback_DiskError;
	CBAlert m_pCallback_IpConflict;
	CBAlert m_pCallback_NetworkDisconnect;
	CBAlert m_pCallback_IllegeAccess;
	CBAlert m_pCallback_DiskGroupError;

	//记录联动PTZ的码值及线路索引
	unsigned long m_dwPtzCode_AlarmAction[MAX_ENC_CHANNEL_NUM];
	unsigned long m_dwCruiseIndex_AlarmAction[MAX_ENC_CHANNEL_NUM];
}
MPU_MANAGER_INFO, *PMPU_MANAGER_INFO;

/* 设备功能映射表 */
typedef struct tagDeviceFunction
{
    char sFunctionString[32];
    int  eDeviceEnum;
}stuDeviceFunctionMap, *pDeviceFunctionMap;

/**************************************************************************
 *                        常量                                            *
 **************************************************************************/
//是否启动关机过程
extern BOOL g_bShutdown;

/**************************************************************************
 *                          宏定义                                         *
 **************************************************************************/


/**************************************************************************
 *                          模块内部接口                                  *
 **************************************************************************/


/**************************************************************************
 *                          参数管理                                      *
 **************************************************************************/

//!\brief   逆初始化参数管理器
//!\return  0表示成功，其它为错误码
int PAR_Uninit();

int PAR_LockBakFile();

/**************************************************************************
 *                          预览管理                                      *
 **************************************************************************/

//!\brief   初始化预览管理模块
//!\return  成功为0，失败为-1
int PRW_Init();

//!\brief   逆初始化预览管理模块
//!\return  成功为0，失败为-1
int PRW_Uninit();

//!\brief 初始化
//!\return  成功为0，失败为-1
int ISP_Init();

/**************************************************************************
 *                         云台管理                                      *
 **************************************************************************/

//!\brief   云台模块初始化
//!\return  0表示初始化成功 -1表示初始化失败
int PTZ_Init();

//!\brief   云台模块资源清除
//!\return  0表示成功 其他表示错误码
int PTZ_Uninit();


/**************************************************************************
 *                          日志管理                                      *
 **************************************************************************/

//!\brief   日志模块初始化
//!\return  0表示初始化成功 -1表示初始化失败
int OPL_Init();

//!\brief   日志模块资源清除
//!\return  0表示成功 其他表示错误码
int OPL_Uninit();


/**************************************************************************
 *                          用户管理                                      *
 **************************************************************************/

//!\brief   初始化用户管理模块
//!\return  0表示成功，其它为错误码
int USR_Init();

//!\brief   释放用户管理模块资源
//!\return  0表示成功，其它为错误码
int USR_Uninit();

//!\brief   获取缺省用户句柄
//!\return  返回用户句柄，INVALID_HANDLE表示失败
HANDLE USR_GetDefault();

//!\brief   恢复管理员密码
int USR_ResumeAdminPassword();

//获取用户句柄
HANDLE USR_GetHandle(char *pszUserName);

/**************************************************************************
 *                          系统管理                                      *
 **************************************************************************/

//!\brief   初始化系统管理
//!\return  0表示成功，其它为错误码
int SYS_Init();

//!\brief   逆初始化系统管理
//!\return  0表示成功，其它为错误码
int SYS_Uninit();

//!\brief       设置当前启动的夏时制地区
//!\param   int nZoneID     地区ID
//!\param   bool bEnable     启用还是禁止。当为启用时，zone_id有效
//!\return  成功为0，失败为-1
int SYS_SetCurrentDST(int nZoneID, bool bEnable);

//!\brief       获取当前启动的夏时制地区
//!\param   int *pnZoneID           地区ID
//!\param   unsigned int *pbEnable  启用还是禁止。当为启用时，pnZoneID有效
//!\return   成功为0，失败为-1
int SYS_GetCurrentDST(int *pnZoneID, unsigned int *pbEnable);


/**************************************************************************
 *                          存储管理                                      *
 **************************************************************************/

//!\brief   初始化存储管理
//!\return  0表示成功，其它为错误码
int STR_Init();

//!\brief   逆初始化存储管理
//!\return  0表示成功，其它为错误码
int STR_Uninit();

//!\brief 设置测试模式
//!\praram nTestMode 模式值 0-出厂模式， 1-工厂模式
//!\return  成功为0，失败为-1
int MPU_SYS_SetProductTestMode(int nTestMode);

//!\brief 清空硬复位状态
int MPU_SYS_ClearHardResetStatus();

//!\brief 获取复位状态
//!\praram nStatus 状态值 0-没有复位 1-已经按下复位
//!\return  成功为0，失败为-1
int MPU_SYS_GetHardResetStatus(int *nStatus);

/**************************************************************************
 *                          通道管理                                      *
 **************************************************************************/

//!\brief   初始化通道管理器
//!\return  0表示成功，其它为错误码
int AVL_Init();

//!\brief   逆初始化通道管理器
//!\return  0表示成功，其它为错误码
int AVL_Uninit();

//!\brief 移动触发报警录像
//!\return  成功为0，失败为-1
int AVL_Record_SetMotionAlarmRec(int nCh);

//!\brief 智能检测报警录像
//!\return  成功为0，失败为-1
int AVL_Record_SetAiAlarmRec(int nCh);

//!\brief 报警输入触发报警录像
//!\return  成功为0，失败为-1
int AVL_Record_SetAlarmInAlarmRec(int nCh);

/**************************************************************************
 *                                                              DDNS                                                                           *
 **************************************************************************/

//!\brief   初始化DDNS
//!\return  0表示成功，其它为错误码
int DDNS_Init();

//!\brief   逆初始化DDNS
//!\return  0表示成功，其它为错误码
int DDNS_Uninit();


/**************************************************************************
 *                                                        本地网络                                                                *
 **************************************************************************/

//!\brief   初始化本地网络
//!\return  0表示成功，其它为错误码
int NET_Init();

//!\brief   逆初始化本地网络
//!\return  0表示成功，其它为错误码
int NET_Uninit();

//!\brief   恢复出厂IP地址
void NET_Resume();

void PAR_NET_Resume(const char *func);

int PAR_UnForbiddenSaveFile();

int PAR_ForbiddenSaveFile();

/**************************************************************************
 *                                                            xDSL                                                                             *
 **************************************************************************/

//!\brief   初始化PPPoE
//!\return  0表示成功，其它为错误码
int PPPoE_Init();

//!\brief   逆初始化PPPoE
//!\return  0表示成功，其它为错误码
int PPPoE_Uninit();


/**************************************************************************
 *                                                        NTP服务器                                                                 *
 **************************************************************************/

//!\brief   初始化NTP服务器
//!\return  0表示成功，其它为错误码
int NTP_Init();

//!\brief   逆初始化NTP服务器
//!\return  0表示成功，其它为错误码
int NTP_Uninit();


/**************************************************************************
 *                                                        远程访问                                                                 *
 **************************************************************************/

//!\brief       初始化远程接入模块
//!\return  0表示成功，其它为错误码
int CIU_Init();

//!\brief       逆初始化远程接入
//!\return  0表示成功，其它为错误码
int CIU_Uninit();

//!\brief       异步报警事件通知
//!\param   pAlertInfo      报警事件信息
//!\return  0表示成功，其它为错误码
int CIU_RefreshAlarmEvent(struct tagAlertInfo *pAlertInfo);

#ifdef __ONE_CLICK_CALL__
// 收到一键呼叫接听动作
int CIU_CALL_SetRecvCallState(int state);
int CIU_CALL_GetCallConnectionId(DWORD ConnectionId);
int CIU_CALL_GetHangupConnectionId(DWORD ConnectionId);
#endif


/**************************************************************************
 *                                                        报警输出                                                                 *
 **************************************************************************/

//!\brief   初始化报警输出
//!\return  0表示成功，其它为错误码
int DOP_Init();

//!\brief   逆初始化报警输出
//!\return  0表示成功，其它为错误码
int DOP_Uninit();


/**************************************************************************
 *                                                        报警输入                                                                 *
 **************************************************************************/

//!\brief   初始化报警输入
//!\return  0表示成功，其它为错误码
int DIP_Init();

//!\brief   逆初始化报警输入
//!\return  0表示成功，其它为错误码
int DIP_Uninit();


/**************************************************************************
 *                                                       报警、异常处理                                                *
 **************************************************************************/

extern PMPU_MANAGER_INFO g_pMpuInfo;

//!\brief       报警、异常联动处理
//!\return   成功为0，其它为错误码
int callback_AlarmAction(struct tagAlertInfo *pAlertInfo, struct tagActionInfo *pActionInfo);


/**************************************************************************
 *                                                       PUB处理                                                                       *
 **************************************************************************/

//!\brief       获取面板ID
//!\return  面板ID
int PUB_GetDvrID();

//!\brief       设置面板ID
//!\param   nID 新的面板ID
//!\return  0表示成功，其它为错误码
int PUB_SetDvrID(int nID);

#if 0
//!\brief       获取别名，需外部将缓冲区清零
int PUB_GetAlias(const char *szSection, const char *szDefaultName, LPHY_DVR_ALIAS_CFG pcfg);

//设置别名
int PUB_SetAlias(const char *szSection, LPHY_DVR_ALIAS_CFG pcfg);
#endif

//!\brief   判断是否是80XXHC或HS
//!\return  TRUE表示是，FALSE表示不是
BOOL PUB_Is80XX_HC_HS();

//!\brief   初始化中心管理服务器
//!\return  0表示成功，其它为错误码
int NMS_Init();

//!\brief       重启动NMS注册服务
//!\return  1表示启动，0表示没有启动
int NMS_Reset();

#ifdef __IS_SUPPORT_CLOUDWORLD__
int MPU_SetCloudStorageMode(int mode);

int MPU_GetCloudStorageMode(int *mode);

int GetCustomCloudStorageMode();
#endif

#ifdef __DIGITAL_ZOOM_DRAG_SUPPORT__
//!\brief   ZOOM初始化
//!\return  0表示成功，其它为错误码
int ZOOM_Init();

//!\brief   逆初始化变倍管理
//!\return  0表示成功，其它为错误码
int ZOOM_Uninit();
#endif

#if defined(__IS_SUPPORT_LCD_MANAGER__)
//!\brief   LCD初始化
//!\return  0表示成功，其它为错误码
int LCD_Init();

//!\brief   逆初始化LCD管理
//!\return  0表示成功，其它为错误码
int LCD_Uninit();
#endif

#endif							//_INC_MPU_GLOBAL_H_
