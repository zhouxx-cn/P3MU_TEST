// Copyright (C) 1991 - 1999 Rational Software Corporation

/*!
*\file		CAlarmOutManager.h
*\brief	 报警输出管理模块
*\author	周庆丰
*\date		2009/04/24
*/

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CALARMOUTMANAGER_499E616303C9_INCLUDED
#define _INC_CALARMOUTMANAGER_499E616303C9_INCLUDED
#include "pthread.h"
#include "tagAlertInfo.h"
#include "tagActionInfo.h"
#include "tagAlarmOutSchedule.h"
#include "CEmbAlarmOut.h"
#include "CExtAlarmOut.h"
#include "CConfig.h"


#define MAX_ALARMOUT_NUM (32)
#define MAX_DOP_NAME_LEN     (20)

//!\brief 报警输出模块接口类
//!1.设置高级/一般模式
//!2.设置和获取输出端口别名
//!3.设置布防时间表
//!4.设置报警输出延时
//!5.设置和获取报警输出状态
class CAlarmOutManager
{
  public:

	//!\brief 获得CAlarmOutManager的实例
	//!\return 返回CAlarmOutManager的指针，NULL表示获取失败
	static CAlarmOutManager *DOP_GetInstance();

	//!\brief 释放实例
	//!\return 0-释放,其它为引用计数
	static int DOP_Release();

	//!\brief 初始化
	//!\return 0成功，其它失败
	int DOP_Init();

	//!\brief 释放资源
	//!\return 0成功，其它失败
	int DOP_UnInit();

	//!\brief 获取布防时间表
	//!\param bEnable 是否启用时间表,不启用时间表则为全天布防
	//!\param pAlarmOutSchedule 布防时间表缓冲区
	//!\param nPort  报警输出通道号
	//!\return 
	//0表示获取成功，时间表存入pAlarmSchedule缓冲区中,-1表示失败
	int DOP_GetSchedule(int nPort, AlarmOutSchedule * pAlarmOutSchedule);

	//!\brief 获取布防时间表
	//!\param pAlarmOutSchedule 时间表缓冲区
	//!\param nPort 报警输出通道号
	//!\return 0表示设置成功，-1表示设置失败
	int DOP_SetSchedule(int nPort, AlarmOutSchedule * pAlarmOutSchedule);

	//!\brief 设置高级/一般模式
	//!\param nMode 0表示一般模式，1表示高级模式
	//!\return 0表示设置成功，-1表示设置失败
	int DOP_SetMode(int nMode);

	//!\brief 获取模式,高级/一般模式
	//!\param nMode 0表示一般模式，1表示高级模式
	//!\return 0表示一般模式，1表示高级模式，-1表示失败
	int DOP_GetMode();

	//!\brief 设置开关
	//!\param nEn
	//!\return 0表示设置成功，-1表示设置失败
	int DOP_SetEnable(int nEn);

	//!\brief 获取模式,高级/一般模式
	//!\return 0表示关，1表示开
	int DOP_GetEnable();

	//!\brief 获取通道别名
	//!\param strName 通道别名缓冲区
	//!\param nLength 缓冲区长度
	//!\param char *strPixel BMP文件名
	//!\param nPort 通道号
	//!\return >=0 返回位图大小,-1表示失败
	int DOP_GetName(int nPort, char *strName, int strNameSize,int nLength, char *strPixel = NULL, int pixelSize = 0);

	//!\brief 设置通道别名
	//!\param strName 通道别名字符串
	//!\param char *strPixel BMP文件名
	//!\param nPort 通道号
	//!\return 0表示设置成功 -1表示设置失败
	int DOP_SetName(int nPort, char *strName, char *strPixel = NULL);

	//!\brief 获取延时时间
	//!\param nPort 报警输出通道号
	//!\return -1为获取失败；其他为获取成功
	int DOP_GetLastTime(int nPort);

	//!\brief 设置延时时间
	//!\param nSecond 延时的秒数,0-为保持
	//!\param nPort 报警输出通道号
	//!\return 0表示设置成功，-1表示失败
	int DOP_SetLastTime(int nPort, int nSecond);

	//!\brief 获取输出端口数
	//!\return -1表示获取失败，其他表示获取的通道数
	int DOP_GetPortNum();

	//!\brief 获取端口输出状态
	//!\param nPort 输出通道号
	//!\return true表示正在输出;false表示已关闭输出
	int DOP_GetAlarmOutState(int nPort);

	//\brief 设置端口输出状态
	//\param bOn true表示打开输出;false表示关闭输出
	//\param nPort 输出通道号
	//\return 0表示设置成功，-1表示设置失败
	int DOP_SetAlarmOutState(int nPort, int bOn);

    int DOP_SetAlarmLightEnable(int nEnable,  int nStayTime);

    int DOP_GetAlarmLightEnable(int *nEnable,  int *nStayTime);

    /* 设置报警输出事件计划配置 */
    int DOP_SetAlarmOutScheduleCfg(const pAlarmOutTimeSchedule pAlarmOutSchedule);

    /* 获取报警输出事件计划配置 */
    int DOP_GetAlarmOutScheduleCfg(pAlarmOutTimeSchedule pAlarmOutSchedule);

  public:
	//////////////////////////////////////////////////////////////////////////
	//以下函数为内部使用，外部不要调用
	//////////////////////////////////////////////////////////////////////////

	//!\brief 单次计划检测
	//!\param void
	//!\return 成功为0，失败为-1，即退出检测
	int CheckStaus();

  private:
	//实例指针
	static CAlarmOutManager *m_pInstance;

	//实例引用计数
	static int m_iInstance_Cnt;

	//是否初始化
	int m_bInit;

	//基本报警输出端口数
	int m_iEmbAlarmOutNum;

	//扩展报警输出端口数
	int m_iExtAlarmOutNum;

	//基本报警输出
	CEmbAlarmOut *theCEmbAlarmOut;

	//扩展报警输出
	CExtAlarmOut *theCExtAlarmOut;

	//报警模式
	int m_iMode;

	//报警模式
	int m_Enable;

	//延迟时间
	int m_iDelayTime[MAX_ALARMOUT_NUM];

	//开始时间
	int m_iStartTime[MAX_ALARMOUT_NUM];

	//通道别名
	char m_strName[MAX_ALARMOUT_NUM][MAX_DOP_NAME_LEN];

	//报警输出计划
	AlarmOutSchedule m_ptAlarmOutSchedule[MAX_ALARMOUT_NUM];

	//状态
	int m_bState[MAX_ALARMOUT_NUM];

	//配置文件
	CConfig *pCConfig;

    // 声光报警灯光报警使能
	int m_iAlarmLightEn;

	// 声光报警持续时间
	int m_iAlarmLightStayTime;

	//计划检测线程
	pthread_t m_hCheckThread;

	//计划检测线程是否退出
	int m_bCheck_exit;

    // 报警输出时间计划
    stuAlarmOutTimeSchedule m_stuAlarmOutTimeSchedule;

    stuTimeArea m_stuDayAreaStartTime;
    stuTimeArea m_stuNightAreaStartTime;

  private:
	//构造函数
	  CAlarmOutManager();

	//
	 ~CAlarmOutManager();

	//配置导入
	int LoadParam();

	//!\brief 检查是否在计划内
	//!\param int nPort 端口号
	//!\return 1-计划时间段内，0-非计划段
	int CheckSchedule(int nPort);




};

#endif							/* _INC_CALARMOUTMANAGER_499E616303C9_INCLUDED */
