// Copyright (C) 1991 - 1999 Rational Software Corporation


/*!
*\file		CAlarmOInManager.h
*\brief	 报警输出管理模块
*\author	周庆丰
*\date		2009/04/24
*/

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CALARMINMANAGER_499E619E0156_INCLUDED
#define _INC_CALARMINMANAGER_499E619E0156_INCLUDED
#include "tagAlertInfo.h"
#include "tagActionInfo.h"
#include "CBAlert.h"
#include "tagAlarmInSchedule.h"
#include "CEmbAlarmIn.h"
#include "CExtAlarmIn.h"
#include "CConfig.h"
#include "tagMotionSchedule.h"
#include "zwsdk.h"


class CActionAlarm;

//!\brief 报警输入接口类
//##ModelId=499E619E0156
class CAlarmInManager
{
  public:

	//!\brief 获取CAlarmInManager实例的指针
	//!\return NULL表示获取失败,其他表示CAlarmInManager实例的指针
	//##ModelId=49B9FCA80157
	static CAlarmInManager *DIP_GetInstance();

	//!\brief 销毁CAlarmInManager实例
	//!\return 返回引用计数,当计数为0时，将释放实例
	//##ModelId=49B9FDEF03C8
	static int DIP_Release();

	//!\brief 初始化
	//!\return 0成功，其它失败
	int DIP_Init();

	//!\brief 释放资源
	//!\return 0成功，其它失败
	int DIP_UnInit();


	//!\brief 获取报警输入通道布防时间表
	//!\param pAlarmInSchedule 布防时间表缓冲区
	//!\param nPort 通道号
	//!\return 0表示设置成功,-1表示设置失败
	//##ModelId=49B5CE7002FD
	int DIP_GetSchedule(int nPort, AlarmInSchedule * pAlarmInSchedule);

	//!\brief 设置报警输入通道布防时间表
	//!\param pAlarmInSchedule 布防时间表指针
	//!\param nPort 通道号
	//!\return 0表示设置成功,-1表示设置失败
	//##ModelId=49B5CD6201A5
	int DIP_SetSchedule(int nPort, AlarmInSchedule * pAlarmInSchedule);


	//!\brief 设置报警输入是否使能,用于一键布防/撤防
	//!\param int bEnable 1-布防,0-撤防
	//!\return 0表示设置成功,-1表示设置失败
	int DIP_SetEnable(int bEnable);

	//!\brief 设置报警输入是否使能,用于一键布防/撤防
	//!\param int bEnable 1-布防,0-撤防
	//!\return 0表示撤防,1表示布防
	int DIP_GetEnable();


	//!\brief 设置报警输入设置模式 一般/高级
	//!\param nMode 0表示一般模式,1表示高级模式
	//!\return 0表示设置成功,-1表示失败
	//##ModelId=49B5E6CC0128
	int DIP_SetMode(int nMode);

	//!\brief 获取报警输入设置模式 
	//!\return 0表示一般模式，1表示高级模式,-1表示失败
	//##ModelId=49B5E6CC0128
	int DIP_GetMode();

	//!\brief 获取报警输入通道数
	//!\return -1表示获取失败，其他表示报警输入通道数
	//##ModelId=49B9FA560251
	int DIP_GetPortNum();

	//!\brief 获取报警输入通道名
	//!\param strName 通道名缓冲区
	//!\param nLength 缓冲区程度
	//!\param char *strPixel BMP文件名
	//!\param nPort 报警输入通道号
	//!\return >=0 返回位图大小,-1表示失败
	//##ModelId=49B5E0B80271
	int DIP_GetName(int nPort, char *strName, int StrNameSize, int nLength, char *strPixel = NULL, int PixelSize = 0);

	//!\brief 设置报警输出通道号
	//\param strName 通道名字符串
	//\param nPort 报警输入通道号
	//!\param char *strPixel BMP文件名
	//\return 0表示设置成功,-1表示设置失败
	//##ModelId=49B5E0BF0000
	int DIP_SetName(int nPort, char *strName, char *strPixel = NULL);

	//!\brief 获取报警输入通道类型
	//!\param nPort 通道号
	//!\return -1表示失败,0表示常开,1表示常闭
	//##ModelId=49B5CE26002E
	int DIP_GetType(int nPort);

	//!\brief 设置通道类型
	//!\param nType 0为常开,1为常闭
	//!\param nPort 通道号
	//!\return 0表示设置成功,-1表示设置失败
	//##ModelId=49B5CCC2031C
	int DIP_SetType(int nPort, int nType);

	//!\brief   获取端口输入状态
	//!\param   nPort               输出通道号
	//!\return  true 有输入，false无输入
	int DIP_GetState(int nPort);

	//!\brief   获取端口输入状态
	//!\return  位俺码
	int DIP_GetAllState();


	//!\brief 获取报警输入联动录像参数
	//!\param dwChannelFlag 联动的通道标记 
	//每一位表示联动一个通道 1表示联动通道1 
	//2表示联动通道2录像 3表示联动通道1和2录像,以此类推
	//!\param nPort 报警输入通道号
	//!\return 0表示获取成功，-1表示获取失败
	//##ModelId=49B7187602BF
	int DIP_GetRecordParam(int nPort, int *dwChannelFlag);

	//!\brief 设置报警输入联动录像参数
	//!\param dwChannelFlag 联动录像通道标记
	//!\param nPort 报警输入通道号
	//!\return 0表示设置成功 -1表示设置失败
	//##ModelId=49B718640232
	int DIP_SetRecordParam(int nPort, int dwChannelFlag);

	//!\brief 
	//获取报警输入联动拍照参数,拍照张数和间隔由CCapturePic::GetCfg获取
	//!\param dwChannelFlag 存储联动拍照通道标记的缓冲区
	//!\param nPort 报警联动输入通道号
	//!\return 0表示成功 -1表示失败
	//##ModelId=49B5CE9A030D
	int DIP_GetSnapshotParam(int nPort, int *dwChannelFlag);

	//!\brief 
	//设置报警输入联动拍照参数，拍照张数和拍照间隔由CCapturePi
	//c::SetCfg设定
	//!\param dwChannelFlag 联动拍照通道标记
	//!\param nPort 报警输入通道号
	//!\return 0表示设置成功 -1表示失败
	//##ModelId=49B5CDFE00BB
	int DIP_SetSnapshotParam(int nPort, int dwChannelFlag);

	//!\brief 获取某一报警输入通道联动某一云台的动作参数
	//!\param nAction 联动动作 
	//*nAction=0表示无动作，1联动预置点，2联动巡航，3联动轨迹
	//!\param nIndex 联动预置点/巡航/轨迹的编号
	//!\param nChannel 云台对应的通道号
	//!\param nPort 报警输入通道号
	//!\return 0表示获取成功, -1表示获取失败
	//##ModelId=49B718B202DE

	int DIP_GetPtzParam(int nPort, int *nAction, int nChannel, int *nIndex);


	//!\brief 设置报警输入通道联动某一个云台的动作
	//!\param nAction 联动的动作 0表示无动作 1联动预置点 
	//2联动巡航 3联动轨迹
	//!\param nIndex 联动的预置点/巡航/轨迹的编号
	//!\param nChannel 云台对应的通道号
	//!\param nPort 报警输入通道号
	//!\return 0表示成功 -1表示失败
	//##ModelId=49B718BD0261
	int DIP_SetPtzParam(int nPort, int nAction, int nChannel = 0, int nIndex = 0);

	//!\brief 获取报警输入联动报警输出的参数
	//!\param dwOutPortFlag 存储输出通道标记的缓冲区
	//!\param nPort 报警输入通道号
	//!\return 0表示成功 -1表示失败
	//##ModelId=49B718DD030D
	int DIP_GetDopParam(int nPort, int *dwOutPortFlag);

	//!\brief 设置报警输入联动报警输出的参数
	//!\param dwOutPortFlag 设置输出通道标记
	//!\param nPort 报警输入通道号
	//!\return 0表示成功 -1表示失败
	//##ModelId=49B718ED00AB
	int DIP_SetDopParam(int nPort, int dwOutPortFlag);

	int SetPushEventIntvCnt(int nPort,int nPushEventIntv);

	int GetPushEventIntvCnt(int nPort,int *nPushEventIntv);

	//!\brief 获取报警输入的联动的某一项是否启用
	//!\param action 联动的动作
	//!      ACTION_SCREEN 屏幕显示
	//!      ACTION_BEEP 声音提示
	//!      ACTION_RECORD 通道录像
	//!      ACTION_SNAPSHOT 通道拍照
	//!      ACTION_ALARMOUT 报警输出
	//!      ACTION_SENDTOSERVER 上传报警服务器
	//!      ACTION_PTZ 联动云台
	//!      ACTION_EMAIL 联动Email
	//!      ACTION_SMS 联动短信
	//!\param nPort 报警输入通道号
	//!\return 
	//-1为失败,0为不启用，1为启用；对于联动屏幕提示，0为不启用
	//，1为联动主显,2为联动辅显,3为联动主显和辅显;
	//##ModelId=49B5CE9202AF
	int DIP_GetEnableAction(int nPort, int action);

	//!\brief 设置是否启用某项联动
	//!\param bEnable 是否启用该项联动,0为不启用,1为启用,对于联动屏幕显示0为不启用，1
	//为联动主显,2为联动辅显,3为联动主显和辅显
	//!\param action 联动动作
	//!       ACTION_SCREEN 屏幕显示
	//!       ACTION_BEEP 声音提示
	//!       ACTION_RECORD 通道录像
	//!       ACTION_SNAPSHOT 通道拍照
	//!       ACTION_ALARMOUT 报警输出
	//!       ACTION_SENDTOSERVER 上传报警服务器
	//!       ACTION_PTZ 联动云台
	//!       ACTION_EMAIL 联动Email
	//!       ACTION_SMS 联动短信
	//!\param nPort 报警输入端口号
	//!\return 0为设置成功，-1为设置失败
	//##ModelId=49B5CD810222
	int DIP_SetEnableAction(int nPort, int action, int bEnable);

	int DIP_GetAlarmInAudioAlarmSchedule(int nPort, tagAudioAlarmSchedule *stSchedule);

	int DIP_SetAlarmInAudioAlarmSchedule(int nPort, tagAudioAlarmSchedule *stSchedule);

	int DIP_GetAlarmInAudioAlarmCfg(int nPort, int *nVoiceType, int *nBroadcastCount);

	int DIP_SetAlarmInAudioAlarmCfg(int nPort, int nVoiceType, int nBroadcastCount);

	int DIP_CheckAudioAlarmInSchedule(int nPort);

	//!\brief 设置报警输入的回调
	//!\param int AlarmType 报警类型。VIDEO_LOST 
	//为视频丢失报警，VIDEO_MOTION为移动报警、CODE_ERROR编码错
	//!\param AlarmInCallback 报警输入联动其他模块的回调函数 
	//!typedef int (*CB_Alert)(struct tagAlertInfo *pAlertInfo, 
	// struct tActionInfo *pActionInfo) 
	//!参数 pAlertInfo 存储报警信息
	//!         pActionInfo 存储联动信息
	//!\return 0表示设置成功，其他表示设置失败
	//!\sa tagAlertInfo,tagActionInfo
	//##ModelId=49AE1547030D
	static int DIP_SetAlarmCallback(int AlarmType, CBAlert AlarmInCallback);



  public:
	//检测报警输入状态
	int CheckStaus();

  private:

	//##ModelId=49A34D320096
	static CAlarmInManager *m_pInstance;

	//实例引用计数
	static int m_iInstance_Cnt;

	//是否初始化
	int m_bInit;

	// 编码句柄
	HANDLE m_henc;

	//基本报警输入端口数
	int m_iEmbAlarmInNum;

	//扩展报警输入端口数
	int m_iExtAlarmInNum;

	//##ModelId=49A216DD029D
	CEmbAlarmIn *theCEmbAlarmIn;

	//##ModelId=49A216E00020
	CExtAlarmIn *theCExtAlarmIn;

	//报警输入状态
	int m_bAlarmInStatus;



	//报警模式
	int m_iMode;

	//报警类型 0-常开,1-常闭
	int m_bType[MAX_ALARMIN_NUM];

	//通道别名
	char m_strName[MAX_ALARMIN_NUM][MAX_DIP_NAME_LEN];

	//报警输出计划
	AlarmInSchedule m_ptAlarmInSchedule[MAX_ALARMIN_NUM];

	//声音报警布防时间
	tagAudioAlarmSchedule m_AudioAlarmScheduleCfg[MAX_ALARMIN_NUM];

	//动作处理
	CActionAlarm *m_pActionAlarm[MAX_ALARMIN_NUM];

	//配置文件
	CConfig *pCConfig;

	//计划检测线程
	pthread_t m_hCheckThread;

	//计划检测线程是否退出
	int m_bCheck_exit;

	//报警类型
	static int m_AlarmType;

	//一键布防,1-布防,0-撤防
	int m_bEnable;

	//回调函数
	static CBAlert m_FxnCallBack;


  private:
	//##ModelId=499E620500AA
	  CAlarmInManager();

	//##ModelId=499E62070017
	 ~CAlarmInManager();

	//配置导入
	int LoadParam();

	//!\brief 检查是否在计划内
	//!\param int nPort 端口号
	//!\return 1-计划时间段内，0-非计划段
	int CheckSchedule(int nPort);


};

#endif							/* _INC_CALARMINMANAGER_499E619E0156_INCLUDED */
