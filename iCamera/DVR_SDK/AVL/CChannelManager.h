// Copyright (C) 1991 - 1999 Rational Software Corporation

/*!
*\file		CChannelManager.h
*\brief	 报警输出管理模块
*\author	周庆丰
*\date		2009/04/24
*/

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CCHANNELMANAGER_49A251E001B9_INCLUDED
#define _INC_CCHANNELMANAGER_49A251E001B9_INCLUDED

#include <pthread.h>
#include "global_macro.h"
#include "enumRecordMode.h"
#include "tagEncCfg.h"
#include "tagAdvancEncCfg.h"
#include "tagRecordSchedule.h"
#include "RECT.h"
#include "HANDLE.h"
#include "tagAlertInfo.h"
#include "tagActionInfo.h"
#include "zwrtmp.h"
//#include "hisdk.h"
#include "zwsdk.h"
#include "tagCharBitMap.h"
#include "MPU/MPU_global.h"
#include "CSoundDetect.h"

//最大通道管理器个数
#define MAX_CH_MANAGER_NUM  MAX_ENC_CHANNEL_NUM

class CAreaDefense;

class CMotion;

class CHumanShape;

class COptics;

class CRecord;

class COsd;

class CCoder;

class CCapturePic;

class CSysManager;

class CConfig;

//!\brief1、录像数据的切换，采取数据与定时器共同驱动的方式。由
//通道管
//!\brief理器负责启用定时器，各个通道提供检测的方法的方式处理
//录像数据的切换。
//!\brief2、录像数据的读取由通道管理器负责统一读取，然后分发给
//个各个通道对象，自行处理各自的数据。
//##ModelId=49A251E001B9
class CChannelManager
{
  private:
	CChannelManager();
	~CChannelManager();
  public:

	//!\brief 初始化
	//!\param void
	//!\return 成功为0，失败为-1
	//##ModelId=49BF1B46026D
	int Init();

	//!\brief 释放资源
	//!\param void
	//!\return 成功为0，失败为-1
	//##ModelId=49BF1B4D0140
	int UnInit();

	//!\brief 获取通道个数
	//!\param void
	//!\return 返回编码通道个数
	//##ModelId=49B4856A0139
	int GetChNum();

	//!\brief 获取制式
	//!\param int *pStandard,接受制式,1-NTSC,2-PAL,4-SECAM,0x80000000-none
	//!\return 成功0,否则失败
	//##ModelId=49B4856A0139
	int GetStandard(int *pStandard);

	//!\brief 设置制式
	//!\param int Standard,1-NTSC,2-PAL,4-SECAM
	//!\return 成功0,否则失败
	//##ModelId=49B4856A0139
	int SetStandard(int Standard);

	//!\brief 暂停/继续录像
	//!\param int bPause 暂停或者继续的标志。
	//!\param int nCh 通道
	//!\return  成功为0，失败为-1
	//##ModelId=49BDB39801E1
	int Record_Pause(int nCh, int bPause);

	//!\brief 启动录像模式：手动、自动、停止三种模式
	//!\param enumRecordeMode model 录像模式的一种
	//!\param int nCh 通道
	//!\return  上次录像模式，失败为-1
	//##ModelId=49BDB3980227
	int Record_StartMode(int nCh, enumRecordMode model);

	//!\brief 获取录像模式：手动、自动、停止三种模式
	//!\param int nCh 通道
	//!\return  录像模式enumRecordeMode model 录像模式的一种
	enumRecordMode Record_GetRecMode(int nCh);

	//!\brief   设置录像码流索引
	//!\param   int nCh         通道
	//!\param   int nIndex  0表示主码流，1表示子码流，2表示高清子码流
	//!\return 成功为0，其它为错误码
	int Record_SetRecStreamIndex(int nCh, int nIndex);

	//!\brief       获取录像码率索引
	//!\param   int nCh 通道
	//!\return   录像码流索引(0表示主码流，1表示子码流，2表示高清子码流)
	int Record_GetRecStreamIndex(int nCh);

	//!\brief 获取当前录像的状态
	//!\param int nCh 通道
	//!\return  返回当前录像的状态，失败为-1
	//##ModelId=49BDB3980277
	int Record_GetState(int nCh);

	//!\brief 获取编码参数
	//!\param int nCh 通道
	//!\param tagEncCfg *Info 编码参数结构体指针
	//!\param int nEnctype 录像的编码类型 0-一般,1-手动,2-事件
	//!\return 成功为0，失败为-1
	//##ModelId=49BDB398028B
	int Record_GetEncCfg(int nCh, tagEncCfg * Info, int nEnctype);

	//!\brief 设置编码参数
	//!\param int nCh 通道
	//!\param tagEncCfg Info 编码参数结构体
	//!\param int nEnctype 录像的编码类型 0-一般,1-手动,2-事件
	//!\return 成功为0，失败为-1
	//##ModelId=49BDB3980317
	int Record_SetEncCfg(int nCh, tagEncCfg * Info, int nEnctype);

	//!\brief   恢复编码参数到出厂设置
	//!\param   int nCh             通道
	//!\param   int nEnctype        录像的编码类型
	//!\return 成功为0，失败为-1
	int Record_ResumeEncCfg(int nCh, int nEnctype);

	//!\brief 获取预录时间
	//!\param int nCh 通道
	//!\param void
	//!\return 预录时间
	//##ModelId=49BDB398039A
	int Record_GetPreTime(int nCh);

	//!\brief 设置预录时间
	//!\param int nCh 通道
	//!\param int nTime 预录时间 0-不限时间
	//!\return 成功为0，失败为-1
	//##ModelId=49BDB39803B8
	int Record_SetPreTime(int nCh, int nTime);

	//!\brief 获取延迟时间
	//!\param int nCh 通道
	//!\param void
	//!\return 延迟时间
	//##ModelId=49BDB399002A
	int Record_GetDelayTime(int nCh);

	//!\brief 设置延迟时间
	//!\param int nCh 通道
	//!\param int nTime 延迟时间
	//!\return 成功为0，失败为-1
	//##ModelId=49BDB3990048
	int Record_SetDelayTime(int nCh, int nTime);

	//!\brief 获取录像的时间计划
	//!\param int nCh 通道
	//!\param tagRecordSchedule* cfg 时间计划结构体
	//!\return 成功为0，失败为-1
	//##ModelId=49BDB39900A2
	int Record_GetSchedule(int nCh, tagRecordSchedule * cfg);

	//!\brief 设置录像的时间计划
	//!\param int nCh 通道
	//!\param ttagSchedule cfg 时间计划结构体
	//!\return 成功为0，失败为-1
	//##ModelId=49BDB39900FC
	int Record_SetSchedule(int nCh, tagRecordSchedule * cfg);
	
	//!\brief 设置新的录像的时间计划
	//!\param int nCh 通道
	//!\param tagRecordScheduleV2 cfg 时间计划结构体
	//!\return 成功为0，失败为-1
    int Record_SetNewSchedule(int nCh, tagRecordScheduleV2 * cfg);

    //!\brief 获取新的录像时间计划
	//!\param int nCh 通道
	//!\param tagRecordScheduleV2* cfg 时间计划结构体
	//!\return 成功为0，失败为-1
    int Record_GetNewSchedule(tagRecordScheduleV2 *cfg);

	//!\brief 定制录像模式
	//!\param int nCh 通道
	//!\param int nCustomMode 0为一般模式，1为高级模式
	//!\return 成功为0，失败为-1
	//##ModelId=49BDB3990160
	int Record_SetRecordCustomMode(int nCustomMode);

	//!\brief 获取定制的信息
	//!\param int nCh 通道
	//!\param int* nCustomMode 0为一般模式，1为高级模式
	//!\return 成功为0，失败为-1
	//##ModelId=49BDB39901C4
	int Record_GetRecordCustomMode(int *nCustomMode);

	//!\brief 移动触发报警录像
	//!\return  成功为0，失败为-1
	int Record_SetMotionAlarmRec(int nCh);

	//!\brief 智能检测报警录像
    //!\return	成功为0，失败为-1
    int Record_SetAiAlarmRec(int nCh);

	//!\brief 报警输入触发报警录像
	//!\return  成功为0，失败为-1
	int Record_SetAlarmInAlarmRec(int nCh);

	//!\brief 获取OSD显示模式
	//!\param int nCh 通道
	//!\praram int* nShowType 
	//0为通道时间均不显示、1为显示通道，2、为显示时间、3为两者
	//均显示
	//!\return  成功为0，失败为-1
	//##ModelId=49BDB54F01BA
	int OSD_GetType(int nCh, int *nShowType);

	//!\brief 设置报警联动回调函数
	//!\param AlarmType //!报警输入
	//    ALERT_TYPE_ALARMIN,
	//   //!移动侦测
	//    ALERT_TYPE_MOTION,
	//!编码异常
	//    ALERT_TYPE_ENCODEERROR,
	//!硬盘错误
	//    ALERT_TYPE_DISKERROR,
	//!硬盘满
	//    ALERT_TYPE_DISKFULL,
	//!IP冲突
	//    ALERT_TYPE_IPCONFLICT,
	//!非法访问
	//     ALERT_TYPE_ILLEGEACCESS,
	//!网线断
	//     ALERT_TYPE_RETICLEDISCONNECT,
	//!视频丢失
	//     ALERT_TYPE_VIDEOLOST,
	//!视频异常
	//     ALERT_TYPE_VIDEOEXCEPTION,
	//!\praram int(*CallBack)(struct tAlertInfo *pAlertInfo,  
	//struct tActionInfo *pActionInfo)) 报警回调指针
	//!\return  成功为0，失败为-1
	//##ModelId=49BDB54F0264
	int SetAlarmCallBack(int AlarmType, int (*CallBack) (struct tagAlertInfo * pAlertInfo, struct tagActionInfo * pActionInfo));

	//!\brief 启用某个报警联动动作
	//!\param ch 通道号
	//!\param AlarmType //!报警输入
	//    ALERT_TYPE_ALARMIN,
	//   //!移动侦测
	//    ALERT_TYPE_MOTION,
	//!编码异常
	//    ALERT_TYPE_ENCODEERROR,
	//!硬盘错误
	//    ALERT_TYPE_DISKERROR,
	//!硬盘满
	//    ALERT_TYPE_DISKFULL,
	//!IP冲突
	//    ALERT_TYPE_IPCONFLICT,
	//!非法访问
	//     ALERT_TYPE_ILLEGEACCESS,
	//!网线断
	//     ALERT_TYPE_RETICLEDISCONNECT,
	//!视频丢失
	//     ALERT_TYPE_VIDEOLOST,
	//!视频异常
	//     ALERT_TYPE_VIDEOEXCEPTION,
	//!\praram int Action    !      ACTION_SCREEN 屏幕显示
	//  !      ACTION_BEEP 声音提示
	//  !      ACTION_ALARMOUT 报警输出
	//  !      ACTION_SENDTOSERVER 上传报警服务器
	//  !      ACTION_EMAIL 联动Email
	//  !      ACTION_SMS 联动短信
	//!\praram int bEnable false 为禁止联动 true 为开启联动
	//对于联动屏幕显示0为不启用，1为联动主显,2为联动辅显,3为联动主显和辅显
	//!\return  成功为0，失败为-1
	//##ModelId=49BDEFDF0009
	int EnableAction(int nCh, int AlarmType, int Action, int bEnable);

	int AreaDefense_EnableAction(int nCh, int Action, int bEnable);

	//!\brief 判断某个报警联动动作是否开启
	//!\param ch 通道号
	//!\param AlarmType //!报警输入
	//    ALERT_TYPE_ALARMIN,
	//   //!移动侦测
	//    ALERT_TYPE_MOTION,
	//!编码异常
	//    ALERT_TYPE_ENCODEERROR,
	//!硬盘错误
	//    ALERT_TYPE_DISKERROR,
	//!硬盘满
	//    ALERT_TYPE_DISKFULL,
	//!IP冲突
	//    ALERT_TYPE_IPCONFLICT,
	//!非法访问
	//     ALERT_TYPE_ILLEGEACCESS,
	//!网线断
	//     ALERT_TYPE_RETICLEDISCONNECT,
	//!视频丢失
	//     ALERT_TYPE_VIDEOLOST,
	//!视频异常
	//     ALERT_TYPE_VIDEOEXCEPTION,
	//!\praram int Action    !      ACTION_SCREEN 屏幕显示
	//  !      ACTION_BEEP 声音提示
	//  !      ACTION_ALARMOUT 报警输出
	//  !      ACTION_SENDTOSERVER 上传报警服务器
	//  !      ACTION_EMAIL 联动Email
	//  !      ACTION_SMS 联动短信
	//!\return false 为禁止联动 true 为开启联动,
	//对于联动屏幕显示0为不启用，1为联动主显,2为联动辅显,3为联动主显和辅显
	//##ModelId=49BDEFDF0081
	int IsEnableAction(int nCh, int AlarmType, int Action);

	int AreaDefense_IsEnableAction(int nCh, int Action);

	//!\brief 获取某个报警联动录像参数
	//!\param ch 通道号
	//!\param AlarmType //!报警输入
	//    ALERT_TYPE_ALARMIN,
	//   //!移动侦测
	//    ALERT_TYPE_MOTION,
	//!编码异常
	//    ALERT_TYPE_ENCODEERROR,
	//!硬盘错误
	//    ALERT_TYPE_DISKERROR,
	//!硬盘满
	//    ALERT_TYPE_DISKFULL,
	//!IP冲突
	//    ALERT_TYPE_IPCONFLICT,
	//!非法访问
	//     ALERT_TYPE_ILLEGEACCESS,
	//!网线断
	//     ALERT_TYPE_RETICLEDISCONNECT,
	//!视频丢失
	//     ALERT_TYPE_VIDEOLOST,
	//!视频异常
	//     ALERT_TYPE_VIDEOEXCEPTION,
	//!\param dwChannelFlag 联动的通道标记 
	//每一位表示联动一个通道 1表示联动通道1 
	//2表示联动通道2录像 3表示联动通道1和2录像,以此类推
	//!\param nPort 某个报警通道号
	//!\return 0表示获取成功，-1表示获取失败
	//##ModelId=49B7187602BF
	int GetRecordParam(int nCh, int AlarmType, int *dwChannelFlag);

	int AreaDefense_GetRecordParam(int nCh, int *dwChannelFlag);
	//!\brief 设置某个报警联动录像参数
	//!\param ch 通道号
	//!\param AlarmType //!报警输入
	//    ALERT_TYPE_ALARMIN,
	//   //!移动侦测
	//    ALERT_TYPE_MOTION,
	//!编码异常
	//    ALERT_TYPE_ENCODEERROR,
	//!硬盘错误
	//    ALERT_TYPE_DISKERROR,
	//!硬盘满
	//    ALERT_TYPE_DISKFULL,
	//!IP冲突
	//    ALERT_TYPE_IPCONFLICT,
	//!非法访问
	//     ALERT_TYPE_ILLEGEACCESS,
	//!网线断
	//     ALERT_TYPE_RETICLEDISCONNECT,
	//!视频丢失
	//     ALERT_TYPE_VIDEOLOST,
	//!视频异常
	//     ALERT_TYPE_VIDEOEXCEPTION,
	//!\param dwChannelFlag 联动录像通道标记
	//!\param nPort 某个报警通道号
	//!\return 0表示设置成功 -1表示设置失败
	//##ModelId=49B718640232
	int SetRecordParam(int nCh, int AlarmType, int dwChannelFlag);

	int AreaDefense_SetRecordParam(int nCh, int dwChannelFlag);
	//!\brief 
	//获取某个报警联动拍照参数,拍照张数和间隔由CCapturePic::GetCfg获取
	//!\param ch 通道号
	//!\param AlarmType //!报警输入
	//    ALERT_TYPE_ALARMIN,
	//   //!移动侦测
	//    ALERT_TYPE_MOTION,
	//!编码异常
	//    ALERT_TYPE_ENCODEERROR,
	//!硬盘错误
	//    ALERT_TYPE_DISKERROR,
	//!硬盘满
	//    ALERT_TYPE_DISKFULL,
	//!IP冲突
	//    ALERT_TYPE_IPCONFLICT,
	//!非法访问
	//     ALERT_TYPE_ILLEGEACCESS,
	//!网线断
	//     ALERT_TYPE_RETICLEDISCONNECT,
	//!视频丢失
	//     ALERT_TYPE_VIDEOLOST,
	//!视频异常
	//     ALERT_TYPE_VIDEOEXCEPTION,
	//!\param dwChannelFlag 存储联动拍照通道标记的缓冲区
	//!\param nPort 报警联动输入通道号
	//!\return 0表示成功 -1表示失败
	//##ModelId=49B5CE9A030D
	int GetSnapshotParam(int nCh, int AlarmType, int *dwChannelFlag);

	int AreaDefense_GetSnapshotParam(int nCh, int *dwChannelFlag);
	//!\brief 
	//设置某个报警联动拍照参数，拍照张数和拍照间隔由CCapturePi
	//c::SetCfg设定
	//!\param ch 通道号
	//!\param AlarmType //!报警输入
	//    ALERT_TYPE_ALARMIN,
	//   //!移动侦测
	//    ALERT_TYPE_MOTION,
	//!编码异常
	//    ALERT_TYPE_ENCODEERROR,
	//!硬盘错误
	//    ALERT_TYPE_DISKERROR,
	//!硬盘满
	//    ALERT_TYPE_DISKFULL,
	//!IP冲突
	//    ALERT_TYPE_IPCONFLICT,
	//!非法访问
	//     ALERT_TYPE_ILLEGEACCESS,
	//!网线断
	//     ALERT_TYPE_RETICLEDISCONNECT,
	//!视频丢失
	//     ALERT_TYPE_VIDEOLOST,
	//!视频异常
	//     ALERT_TYPE_VIDEOEXCEPTION,
	//!\param dwChannelFlag 联动拍照通道标记
	//!\param nPort 某个报警通道号
	//!\return 0表示设置成功 -1表示失败
	//##ModelId=49B5CDFE00BB
	int SetSnapshotParam(int nCh, int AlarmType, int dwChannelFlag);

	int AreaDefense_SetSnapshotParam(int nCh, int dwChannelFlag);

	//!\brief 获取某一某个报警通道联动某一云台的动作参数
	//!\param ch 通道号
	//!\param AlarmType //!报警输入
	//    ALERT_TYPE_ALARMIN,
	//   //!移动侦测
	//    ALERT_TYPE_MOTION,
	//!编码异常
	//    ALERT_TYPE_ENCODEERROR,
	//!硬盘错误
	//    ALERT_TYPE_DISKERROR,
	//!硬盘满
	//    ALERT_TYPE_DISKFULL,
	//!IP冲突
	//    ALERT_TYPE_IPCONFLICT,
	//!非法访问
	//     ALERT_TYPE_ILLEGEACCESS,
	//!网线断
	//     ALERT_TYPE_RETICLEDISCONNECT,
	//!视频丢失
	//     ALERT_TYPE_VIDEOLOST,
	//!视频异常
	//     ALERT_TYPE_VIDEOEXCEPTION,
	//!\param nAction 联动动作 
	//*nAction=0表示无动作，1联动预置点，2联动巡航，3联动轨迹
	//!\param nIndex 联动预置点/巡航/轨迹的编号
	//!\param nChannel 云台对应的通道号
	//!\param nPort 某个报警通道号
	//!\return 0表示获取成功, -1表示获取失败
	//##ModelId=49B718B202DE

	int GetPtzParam(int nCh, int AlarmType, int nChannel, int *nAction, int *nIndex);

	int AreaDefense_GetPtzParam(int nCh, int nChannel, int *nAction, int *nIndex);

	//!\brief 设置某个报警通道联动某一个云台的动作
	//!\param ch 通道号
	//!\param AlarmType //!报警输入
	//    ALERT_TYPE_ALARMIN,
	//   //!移动侦测
	//    ALERT_TYPE_MOTION,
	//!编码异常
	//    ALERT_TYPE_ENCODEERROR,
	//!硬盘错误
	//    ALERT_TYPE_DISKERROR,
	//!硬盘满
	//    ALERT_TYPE_DISKFULL,
	//!IP冲突
	//    ALERT_TYPE_IPCONFLICT,
	//!非法访问
	//     ALERT_TYPE_ILLEGEACCESS,
	//!网线断
	//     ALERT_TYPE_RETICLEDISCONNECT,
	//!视频丢失
	//     ALERT_TYPE_VIDEOLOST,
	//!视频异常
	//     ALERT_TYPE_VIDEOEXCEPTION,
	//!\param nAction 联动的动作 0表示无动作 1联动预置点 
	//2联动巡航 3联动轨迹
	//!\param nIndex 联动的预置点/巡航/轨迹的编号
	//!\param nChannel 云台对应的通道号
	//!\param nPort 某个报警通道号
	//!\return 0表示成功 -1表示失败
	//##ModelId=49B718BD0261
	int SetPtzParam(int nCh, int AlarmType, int nChannel, int nAction, int nIndex = 0);

	int AreaDefense_SetPtzParam(int nCh, int nChannel, int nAction, int nIndex = 0);

	//!\brief 获取某个报警联动报警输出的参数
	//!\param ch 通道号
	//!\param AlarmType //!报警输入
	//    ALERT_TYPE_ALARMIN,
	//   //!移动侦测
	//    ALERT_TYPE_MOTION,
	//!编码异常
	//    ALERT_TYPE_ENCODEERROR,
	//!硬盘错误
	//    ALERT_TYPE_DISKERROR,
	//!硬盘满
	//    ALERT_TYPE_DISKFULL,
	//!IP冲突
	//    ALERT_TYPE_IPCONFLICT,
	//!非法访问
	//     ALERT_TYPE_ILLEGEACCESS,
	//!网线断
	//     ALERT_TYPE_RETICLEDISCONNECT,
	//!视频丢失
	//     ALERT_TYPE_VIDEOLOST,
	//!视频异常
	//     ALERT_TYPE_VIDEOEXCEPTION,
	//!\param dwOutPortFlag 存储输出通道标记的缓冲区
	//!\param nPort 某个报警通道号
	//!\return 0表示成功 -1表示失败
	//##ModelId=49B718DD030D
	int GetDopParam(int nCh, int AlarmType, int *dwOutPortFlag);

	int GetPushEventIntvCnt(int nCh, int AlarmType, int *nPushEventIntv);

	int AreaDefense_GetDopParam(int nCh, int *dwOutPortFlag);
	//!\brief 设置某个报警联动报警输出的参数
	//!\param ch 通道号
	//!\param AlarmType //!报警输入
	//    ALERT_TYPE_ALARMIN,
	//   //!移动侦测
	//    ALERT_TYPE_MOTION,
	//!编码异常
	//    ALERT_TYPE_ENCODEERROR,
	//!硬盘错误
	//    ALERT_TYPE_DISKERROR,
	//!硬盘满
	//    ALERT_TYPE_DISKFULL,
	//!IP冲突
	//    ALERT_TYPE_IPCONFLICT,
	//!非法访问
	//     ALERT_TYPE_ILLEGEACCESS,
	//!网线断
	//     ALERT_TYPE_RETICLEDISCONNECT,
	//!视频丢失
	//     ALERT_TYPE_VIDEOLOST,
	//!视频异常
	//     ALERT_TYPE_VIDEOEXCEPTION,s
	//!\param dwOutPortFlag 设置输出通道标记
	//!\param nPort 某个报警通道号
	//!\return 0表示成功 -1表示失败
	//##ModelId=49B718ED00AB
	int SetDopParam(int nCh, int AlarmType, int dwOutPortFlag);

	int SetPushEventIntvCnt(int nCh, int AlarmType, int nPushEventIntv);

	int AreaDefense_SetDopParam(int nCh, int dwOutPortFlag);

	//!\brief 获取时间格式
	//!\param int nCh 通道
	//!\praram int* nShowFortMat 
	//时间格式。根据底层的实际情况来确定可以设置哪些格式
	//!\return  成功为0，失败为-1
	//##ModelId=49BDB54F03AF
	int OSD_GetTimeFormat(int nCh, int *nShowFortMat);



	//!\brief 
	//设置OSD参数配置(需要实现通道别名+位置，时间格式+位置)
	//!\param int nCh 通道
	//!\praram int nShowType OSD类型
	//!\return  成功为0，失败为-1
	//##ModelId=49BDB550012F
	int OSD_SetType(int nCh, int nShowType);

	//!\brief 设置时间或者字符的位置
	//!\param int nCh 通道
	//!\praram int nShowType OSD类型
	//!\return  成功为0，失败为-1
	//##ModelId=49BDB55001EE
	int OSD_SetPosition(int nCh, int OSD_TYPE, int x, int y);

	//!\brief 设置时间或者字符的位置
	//!\param int OSD_TYPE OSD类型 1-文本,2-时间,3-时间+文本
	//!\param int Time_x, int Time_y, int Text_x, int Text_y时间与文件的位置
	//!\return  成功为0，失败为-1
	int OSD_SetPositionAll(int nCh, int Time_x, int Time_y, int Text_x, int Text_y, int OSD_TYPE = 3);

	//!\brief 获取通道字符点阵
	//!\param int nCh 通道
	//!\praram char * CharBitMap BMP文件名
	//!\param int buffSize 缓冲大小
	//!\return  成功为0，失败为-1，当pCharBitMap == NULL 时返回实际大小
	//##ModelId=49BDB550007B
	int OSD_GetCharBitMap(int nCh, char *CharBitMap, int buffSize);

	//!\brief 获取时间点阵宽高
	//!\praram int* pWidth,int *pHeight
	//!\return  成功为0，失败为-1
	//##ModelId=49B767A50356
	int OSD_GetTimeWH(int nCh, int *pWidth, int *pHeight);



	//!\brief 设置时间格式
	//!\param int nCh 通道
	//!\praram int nShowFortMat 
	//时间格式。根据底层的实际情况来确定可以设置哪些格式
	// //0-YYYYMMDD12,1-YYYYMMDD24,2-MMDDYYYY12,3-MMDDYYYY24
	//!\return  成功为0，失败为-1
	//##ModelId=49BDB550034C
	int OSD_SetTimeFormat(int nCh, int nShowFortMat);

	//!\brief 设置通道字符点阵
	//!\param int nCh 通道
	//!\praram char CharBitMap BMP文件名
	//!\return  成功为0，失败为-1
	//##ModelId=49BDB551002C
	int OSD_SetCharBitMap(int nCh, char *CharBitMap, unsigned char *byLattice ,int FontSize, int Width, int Height);

	int OSD_SetTextEx(char *OSDToken, unsigned short *strText, int CharSet);
	
	int OSD_SetTextOSD_Cprocustom(char *unicde);
	
	int OSD_SetOSDV2(int index, int bEnable, char *OSDToken, unsigned short *strText, int nX, int nY);

	int OSD_GetOSDV2TextByNum(int i, int *bEnable, char *OSDToken, unsigned short *strText, int *nX, int *nY);

	int OSD_GetTextExByToken(char *OSDToken, unsigned short *strText, int *CharSet);
		
	int OSD_GetTextExByNum(int num, char *OSDToken, int tokenSize, unsigned short *strText, int *CharSet);

	//!\brief 设置编码高级参数
	//!\param tagAdvanceEncCfg cfg 高级编码参数结构体
	//!\return 成功为0，失败为-1
	//##ModelId=49B716980242
    int SetAdvanceEncParam(int nCh, int NormalAdvEncType, int EventAdvEncType, tagVIDEO_SHELTER * pCfg);

	//!\brief 获取音量大小
	//!\praram int nCh 通道
	//!\return	成功为音量大小值，失败为-1
	int GetAudioInVol(int nCh);
	
	int SetAudioInVol(int nCh, int vol);

	int GetAudioOutVol(int nCh);

	int SetAudioOutVol(int nCh, int vol);
	
	int SetAudioMappingVol(int nCh, int vol,int Type);

	int SetAudioAlarmConfig(int nCh, int enable, int audioType, int audioDelayOutput, int audioVolume, int iAudioPlayTimes);

	int TryListenAlarmVoice(int nCh, int enable, int audioType, int audioDelayOutput, int audioVolume, int alarmType);

	int SetAudioCustomVoice(int nCh, int customtype, char *pBuff, int len);

	int SetFireAudioAlarmCustomVoice(int nCh, char *pBuff, int len);

	int GetAudioAlarmConfig(int nCh, int *enable, int *audioType, int *audioDelayOutput, int *audioVolume, int *iAudioPlayTimes);

	int GetGetRealKeyFrameInterval(int nCh, int stream,int *nFrameRateInterval, int *nKeyFrameInterval);

	int GetKeyFrameInterval(int nCh, int stream, int *value);

	//nCh   0:main stream   1:sub stream   2:hd sub stream
	int SetKeyFrameInterval(int nCh, int stream, int value);

	//!\brief 获取编码高级参数
	//!\param tagAdvanceEncCfg* cfg 高级编码参数结构体
	//!\return 成功为0，失败为-1
	//##ModelId=49B7164C0058
    int GetAdvanceEncParam(int nCh, int * NormalAdvEncType, int * EventAdvEncType, tagVIDEO_SHELTER * pCfg);

    int SetAuxAdvEncParam(int nCh, int AuxAdvEncType);

    int GetAuxAdvEncParam(int nCh, int *pAuxAdvEncType);

    int SetHDAuxAdvEncParam(int nCh, int HDAuxAdvEncType);

    int GetHDAuxAdvEncParam(int nCh, int *pHDAuxAdvEncType);
    int SetAuxHDAdvEncParam(int nCh, int AuxAdvEncType, int HDAuxAdvEncType);

    int GetAuxHDAdvEncParam(int nCh, int *pAuxAdvEncType, int *pHDAuxAdvEncType);

    int SetFourthStreamAdvEncParam(int nCh, int FourthStreamAdvEncType);

    int GetFourthStreamAdvEncParam(int nCh, int *pFourthStreamAdvEncType);

	//!\brief 设置录像写入状态 0:暂停  1:写入
    int SetRecordWriteStatus(int nCh, int record_write_status);

    /*@breif SetCurrentAviNumPerPage 设置每页显示多少个AVI文件
     *@param [IN]  avi_num_per_page 每页显示的AVI文件个数
     *@return 成功返回0
     *@       失败返回-1
     *@note  每页显示的AVI文件个数不能超过当前有效的AVI个数g_read_curr_avi_num，
     *@      每页显示的AVI文件个数不能超过最大能显示的AVI个数MAX_AVI_NUM_ONE_PAGE
     */
    int SetCurrentAviNumPerPage(int nCh, int avi_num_per_page);

    /*@breif GetCurrentAviNumPerPage 获取显示多少个AVI文件
     *@return 成功返回AVI文件个数
     *@       失败返回-1
     */
    int GetCurrentAviNumPerPage(int nCh);

    /*@breif GetCurrentAviTotalNum 获取当前AVI总个数
     *@return 成功返回当前总的AVI文件个数
     *@       失败返回-1
     */
    int GetCurrentAviTotalNum(int nCh);

	/*@breif SetSwitchPageIsChoose 设置当前选中的是哪一个页面和当前录像路径类型
     *@param [IN]  choose_page 选中的页面(从1开始)
     *@param [IN]  path_type 当前录像路径类型 0: http路径  1:绝对路径
     *@return 成功返回0
     *@       失败返回-1
     */
    int SetSwitchPageIsChoose(int nCh, int choose_page, int path_type);

    /*@breif GetSwitchPageIsChoose 获取当前选中的是哪一个页面
     *@return 成功返回当前选中的是哪一个页面
     *@       失败返回-1
     */
    int GetSwitchPageIsChoose(int nCh);

    /*@breif GetCurrentAviListInfo 从给定的页面里面获取当前AVI文件列表
     *@param [OUT] p_avi_list 获取到的AVI文件列表
     *@param [OUT] p_avi_num_curr_page 当前页面有多少个AVI文件
     *@return 成功返回0
     *@       失败返回-1
     *@note   需要先调用SetSwitchPageIsChoose设置当前选中哪一页
     *@       需要先调用SetCurrentAviNumPerPage设置当前用户需要设置每页显示多少个AVI文件
     */
    int GetCurrentAviListInfo(int nCh, void **p_avi_list, int *p_avi_num_curr_page);

    /* 设置当前要寻找的AVI的时间段 */
    int SetCurrentAviSearchTimeSeg(int nCh, DateTime start_time, DateTime stop_time);

    /* 设置当前要寻找的PIC的时间段 */
    int SetCurrentPicSearchTimeSeg(int nCh, DateTime start_time, DateTime stop_time);

    /*@breif SetCurrentPicNumPerPage 设置每页显示多少个图片文件
     *@param [IN]  pic_num_per_page 每页显示的图片文件个数
     *@return 成功返回0
     *@       失败返回-1
     *@note  每页显示的图片文件个数不能超过当前有效的图片个数g_read_curr_pic_num，
     *@      每页显示的图片文件个数不能超过最大能显示的图片个数MAX_PIC_NUM_ONE_PAGE
     */
    int SetCurrentPicNumPerPage(int nCh, int pic_num_per_page);

    /*@breif GetCurrentPicNumPerPage 获取显示多少个图片文件
     *@return 成功返回图片文件个数
     *@       失败返回-1
     */
    int GetCurrentPicNumPerPage(int nCh);

    /*@breif GetCurrentPicTotalNum 获取当前图片总个数
     *@return 成功返回当前总的图片文件个数
     *@       失败返回-1
     */
    int GetCurrentPicTotalNum(int nCh);

    /*@breif SetSwitchPicPageIsChoose 设置当前选中的是哪一个页面和当前图片路径类型
     *@param [IN]  choose_page 选中的页面(从1开始)
     *@param [IN]  path_type 当前图片路径类型 0: http路径  1:绝对路径
     *@return 成功返回0
     *@       失败返回-1
     */
    int SetSwitchPicPageIsChoose(int nCh, int choose_page, int path_type);

    /*@breif GetSwitchPicPageIsChoose 获取当前选中的是哪一个页面
     *@return 成功返回当前选中的是哪一个页面
     *@       失败返回-1
     */
    int GetSwitchPicPageIsChoose(int nCh);

    /*@breif GetCurrentPicListInfo 从给定的页面里面获取当前图片文件列表
     *@param [OUT] p_pic_list 获取到的图片文件列表
     *@param [OUT] p_pic_num_curr_page 当前页面有多少个图片文件
     *@return 成功返回0
     *@       失败返回-1
     *@note   需要先调用SetSwitchpicPageIsChoose设置当前选中哪一页
     *@       需要先调用SetCurrentpicNumPerPage设置当前用户需要设置每页显示多少个图片文件
     */
    int GetCurrentPicListInfo(int nCh, void **p_pic_list, int *p_pic_num_curr_page);

    /* 获取文件对应的文件信息 */
    int GetRecordFileInfoByFileName(int nCh, const char* pFileName, void* pFileInfo);

    /* 获取磁盘资源占用情况(单位: MB) */
    int GetDiskResourceSize(int nCh, int *pRecordMB, int *pPictureMB);

    /* 设置录像存放的比例(范围0~100) */
    int SetVideoUsedSpaceRate(int nCh, int nVideoSpace);

    /* 获取录像存放的比例(范围0~100) */
    int GetVideoUsedSpaceRate(int nCh);

    /*@breif GetFileReadWriteStatus 获取当前文件的读写状态
     *@param [IN]  file_name 需要操作的文件名(绝对路径)
     *@param [OUT] read_write_status 
     *@            0:  当前文件空闲
     *@            1:  当前文件正在读
     *@            2:  当前文件正在写
     *@param [OUT] file_size 文件大小(单位: 字节) 
     *@return 返回0表示成功
     *@      返回-1表示失败
     */
    int GetFileReadWriteStatus(int nCh, 
                                      const char *file_name, 
                                      unsigned int *read_write_status,
                                      unsigned int *file_size);

    /* 设置获取时间偏移回调 */
    int SetGetTimeOffsetCallBack(int nCh, 
                                              int (*CallBack)(int *, int *, int *));

    int HttpTriggerRecord(int nCh);

    /* 传入的图片名字组成pic_name: 时间戳_标号，如:1532477851_01需要改为1532477851 */
    int UploadPictureToSdCard(int nCh, const char *pic_name, unsigned int trig_type);

	//!\brief 获取时间或者字符的位置
	//!\param int nCh 通道
	//!\praram int OSD_TYPE OSD类型 
	//!\praram  int* x osdx位置
	//!\praram  int* y osdxy位置
	//!\return  成功为0，失败为-1
	//##ModelId=49BDC3C10352
	int OSD_GetPosition(int nCh, int OSD_TYPE, int *x, int *y);

	int SetTrigRecordOverCallBack(int nCh, int (*CallBack)(unsigned char *));
	
	//!\brief 强制一个I帧
	//!\praram int nCh 通道
	//!\return  成功为0，失败为-1
	//##ModelId=49BDC5120004
	int EncCaptureIFrame(int nCh, int nSensorIndex);

	//!\brief 判断是否启用码流
	//!\praram int nCh 通道
	//!\return  int bEnable false 为不启用，true为启用
	//##ModelId=49BDC52D01E3
	int AuxCoder_IsEnable(int nCh);

	//!\brief 设置子码流帧率
	//!\praram int nCh 通道
	//!\return  成功为0，失败为-1
	//##ModelId=49BDC53C00E0
	int AuxCoder_SetFrameRate(int nCh, int nFrameRate);

	//!\brief 获取子码流帧率
	//!\praram int nCh 通道
	//!\return  子码流帧率，失败为-1
	//##ModelId=49BDC5900041
	int AuxCoder_GetFrameRate(int nCh);

	//!\brief 设置子码流码率
	//!\praram int nCh 通道
	//!\return  成功为0，失败为-1
	//##ModelId=49BDC5BD03B7
	int AuxCoder_SetBitRate(int nCh, int bBitRate);

	//!\brief 获取子码流码率
	//!\praram int nCh 通道
	//!\return  成功为子码流码率，失败为-1
	//##ModelId=49BDC75800AB
	int AuxCoder_GetBitRate(int nCh);

	int AuxCoder_SetFormat(int nCh, int byResolution, int width, int height);

	//!\brief 获取子码流尺寸
	//!\praram int nCh 通道
	//!\return  成功为子码流尺寸，失败为-1
	//##ModelId=49BDC75800AB
	int AuxCoder_GetFormat(int nCh, int *width, int *height);

	//!\brief 启用/不启用子码流
	//!\praram int nCh 通道
	//!\praram int bEnable false 为不启用，true为启用
	//!\return  成功为0，失败为-1
	//##ModelId=49BDC8050122
	int AuxCoder_Enable(int nCh, int bEnable);

	//!\brief 获取子码流码率
	//!\praram int nCh 通道
	//!\return  成功为子码流信息，单位（Kb）,失败为-1
	int AuxCoder_GetBitInfo(int nCh);

    //!\brief 获取子码流码率质量
	//!\praram int nCh 通道
	//!\return  成功为0，失败为-1
    int AuxCoder_GetQuality(int nCh);

    //!\brief 设置子码流码率质量
	//!\praram int nCh 通道
	//!\return  成功为0，失败为-1
    int AuxCoder_SetQuality(int nCh, int dwQuality);
    
    //!\brief 获取子码流码率类型
	//!\praram int nCh 通道
	//!\return  成功为0，失败为-1
    int AuxCoder_GetBitrateType(int nCh);

    //!\brief 设置子码流码率类型
	//!\praram int nCh 通道
	//!\return  成功为0，失败为-1
    int AuxCoder_SetBitrateType(int nCh, int dwBitrateType);
    
    //!\brief 获取子码流码流类型
	//!\praram int nCh 通道
	//!\return  成功为0，失败为-1
    int AuxCoder_GetStreamType(int nCh);

    //!\brief 设置子码流流类型
	//!\praram int nCh 通道
	//!\return  成功为0，失败为-1
    int AuxCoder_SetStreamType(int nCh, int dwStreamType);


	//tutk 设置的编码参数，由中间层决定设置到哪个流配置里
	int TUTK_GetMedia(int nCh, int *whichMedia);

	int	TUTK_SetQualityCfg(int nCh, int tutkQuality);
	
	int	TUTK_GetQualityCfg(int nCh, int *tutkQuality);
	
	int	TUTK_GetQualityboardCfg(int nCh, int count, int *dwStreamType);

#ifdef USE_HDAUX
	//!\brief 启用/不启用高清子码流
	//!\param int nCh 通道
	//!\param BOOL bEnable FALSE 为不启用，TRUE为启用
	//!\return  成功为0，失败为-1
	int HDAuxCoder_Enable(int nCh, BOOL bEnable);

	//!\brief 判断是否启用码流
	//!\praram int nCh 通道
	//!\return  int bEnable false 为不启用，true为启用
	//##ModelId=49BDC52D01E3
	int HDAuxCoder_IsEnable(int nCh);

	//!\brief   获取高清子码流参数
	//!\param   int nCh 通道
	//!\param   tagEncCfg * pInfo 编码参数
	//!\return  成功为0，失败为-1
	int HDAuxCoder_GetEncCfg(int nCh, tagEncCfg * pInfo);

	//!\brief       设置高清子码流参数
	//!\param   int nCh 通道
	//!\param   tagEncCfg * pInfo 编码参数
	//!\return  成功为0，失败为-1
	int HDAuxCoder_SetEncCfg(int nCh, tagEncCfg * pInfo);

	//!\brief 获取高清子码流码率
	//!\praram int nCh 通道
	//!\return  成功为子码流信息，单位（Kb）,失败为-1
	int HDAuxCoder_GetBitInfo(int nCh);
#endif

#ifdef USE_FOURTH_STREAM

    //!\brief 获取第四码流码率
    //!\praram int nCh 通道
    //!\return  成功为子码流信息，单位（Kb）,失败为-1
    int FourthStream_GetEncCfg(int nCh, tagEncCfg * pInfo);

    //!\brief   设置第四码流参数
    //!\param   int nCh 通道
    //!\param   tagEncCfg * pInfo 编码参数
    //!\return  成功为0，失败为-1
    int FourthStream_SetEncCfg(int nCh, tagEncCfg * pInfo);

#endif

	//!\brief 获取主码流码率
	//!\praram int nCh 通道
	//!\return  成功为主码流信息，单位（Kb）,失败为-1

	int Coder_GetBitInfo(int nCh);

	//!\breif 获取设备能力
	//!\praram_in type 0表示去雾能力，1是快门模式，2是镜头矫正
	//!\return 0不支持，1支持，-1出错
	int Pic_GetSupport(int type);

	//!\brief 获取图像参数配置的工作模式
	//!\return  
	//成功为工作模式。0为普通工作模式，1为高级工作模式，失败为
	//-1
	//##ModelId=49BDCD8C02B7
	int Pic_GetWorkMode();

	//!\brief 设置图像参数
	//!\praram int nCh 通道
	//!\praram int TimeSec 
	//时间段。0为缺省参数（普通工作模式下只需要设置缺省参数即
	//可），1为时间段1参数...   
	//!\praram  int Type 
	//参数类型。0为亮度，1为色度，2为对比度，3为饱和度
	//!\praram  int value 数值。0-255
	//!\return  成功为0，失败为-1
	//##ModelId=49BDCA430330
	int Pic_SetPicParam(int nCh, int TimeSec, int Type, int value);

	//!\brief 设置图像时间段参数
	//!\praram int nCh 通道
	//!\praram int TimeSec 时间段。1为时间段1参数...    
	//!\praram  int iStartTime 开始时间 相对00:00时的秒数   
	//!\praram  int iStopTime 结束时间  相对00:00时的秒数   
	//!\return  成功为0，失败为-1
	//##ModelId=49BDCA98006B
	int Pic_SetTimeCfg(int nCh, int timesec, int iStartTime, int iStopTime);

	//!\brief 设置图像参数配置的工作模式
	//!\praram int nWorkMode 
	//0为普通工作模式，1为高级工作模式，失败为-1
	//!\return  成功为0，失败为-1
	//##ModelId=49BDD24E0128
	int Pic_SetWorkMode(int nWorkMode);

	//!\brief 获取图像参数
	//!\praram int nCh 通道
	//!\praram int TimeSec 
	//时间段。0为缺省参数（普通工作模式下只需要设置缺省参数即
	//可），1为时间段1参数...   
	//!\praram  int Type 
	//参数类型。0为亮度，1为色度，2为对比度，3为饱和度
	//!\return  成功为对应参数，失败为-1
	//##ModelId=49BDD24E0330
	int Pic_GetPicParam(int nCh, int TimeSec, int Type);

	//!\brief 获取图像时间段参数
	//!\praram int nCh 通道
	//!\praram int TimeSec 时间段。1为时间段1参数...    
	//!\praram  int *piStartTime 开始时间 相对00:00时的秒数 
	//!\praram  int *piStopTime 结束时间    相对00:00时的秒数   
	//!\return  
	//1为启用了该时间段，0为没有启用该时间段，-1获取失败
	//##ModelId=49BDD24F01F1
	int Pic_GetTimeCfg(int nCh, int timesec, int *piStartTime, int *piStopTime);

	//!\brief 设置是否为编辑状态，图像参数
	//!\praram int bEdit 1-开始编辑，暂停自动配置。0-退出编辑，开始启动自动
	//!\return  成功为0，失败为-1
	//##ModelId=49BDEEBD02BF
	int Pic_EnterEdit(int nCh, int bEdit);

	//!\brief 抓图
	//!\praram int nCh 通道
	//!\praram int bAlarm 
	//是否为联动抓图。false为手动抓图，true为联动抓图
	//!\param int AlarmType 触发源类型
	//!\param int AlarmCh 触发源通道
	//!\return  成功为0，失败为-1
	//##ModelId=49BDD2890141
	int Pic_Capture(int nCh, int bAlarm, int AlarmType, int AlarmCh);


	//!\brief 抓图
	//!\praram int nCh 通道
	//!\praram char *FileName 传出文件名称
	//!\return  成功为0，失败为-1
	//##ModelId=49BDD2890141
	int Pic_Capture(int nCh, char *FileName);

	//!\brief 获取抓拍参数。手动抓图不受该参数影响
	//!\praram int nCount 连续抓拍多少张
	//!\praram int nIntervalTime 间隔时间
	//!\return  成功为0，失败为-1
	//##ModelId=49BDD2D300CF
	int Pic_GetCaptureCfg(int nCh,int *nCount, int *nIntervalTime, int *nFormat, int *nWidth, int *nHeight, int *nQuality);

	//!\brief 设置抓拍参数。手动抓图不受该参数影响
	//!\praram int nCount 连续抓拍多少张
	//!\praram int nIntervalTime 间隔时间
	//!\return  成功为0，失败为-1
	//##ModelId=49BDD4D60075
	int Pic_SetCaptureCfg(int nCh,int nCount, int nIntervalTime, int nFormat, int nWidth, int nHeight, int nQuality);

	//!\brief   获取抓码流类型
	//!\param   char nStreamType 0表示主码流，1表示第一子码流，2表示第二子码流
	//!\return  成功为0，失败为-1
	int Pic_GetCaptureStreamType(int nCh, char *nStreamType);

	//!\brief	设置抓码流类型
	//!\param	char nStreamType 0表示主码流，1表示第一子码流，2表示第二子码流
	//!\return	成功为0，失败为-1
	int Pic_SetCaptureCfgV2(int nCh, int nQualitychar, char nStreamType);
	
	//!\brief 设置移动侦测灵敏度
	//!\praram int nCh 通道
	//!\praram int iGrade 灵敏度
	//!\return  成功为0，失败为-1
	//##ModelId=49BDE6000091
	int Motion_SetDetectPrecision(int nCh, int iGrade);

    //!\brief 设置人形检测灵敏度
	//!\praram int nCh 通道
	//!\praram int iGrade 灵敏度
	//!\return  成功为0，失败为-1
	int HumanShape_SetDetectPrecision(int nCh, int iGrade);

	//!\brief 获取灵敏度
	//!\praram int nCh 通道
	//!\return  返回灵敏度
	//##ModelId=49BDE7850394
	int Motion_GetDetectPrecision(int nCh);

    //!\brief 获取灵敏度
	//!\praram int nCh 通道
	//!\return  返回灵敏度
	int HumanShape_GetDetectPrecision(int nCh);
	
	//!\brief 移动多个移动块的移动侦测
	//!\praram int nCh 通道
	//!\praram int pMagicBlock 启用块的索引序列数组
	//!\praram  int nBlockNum  使能的块数
	//启动该宏块的移动侦测。false为不启动，true为启动
	//!\return  成功为0，失败为-1
	//##ModelId=49BDE672019A
	int Motion_EnableMagicBlock(int nCh, unsigned char *pMagicBlock, int nBlockNum);

	//!\brief 获得启用了移动侦测的块
	//!\praram int nCh 通道
	//!\praram int pMagicBlock 启用块的索引序列数组
	//!\praram  int nMaxBlockNum  缓冲最大可接受的块数(一般 16x12块)
	//!\return  >=0 块个数,否则错误;pMagicBlock = NULL 时返回块个数,缓冲大小不够时返回错误
	//##ModelId=49BDE739029A
	int Motion_GetEnableMagicBlock(int nCh, unsigned char *pMagicBlock, int nMaxBlockNum);

	//!\brief 移动多个移动侦测区域的移动侦测
	//!\praram int nCh 通道
	//!\praram  tagMotionRegion * cfg 区域结构体
	//!\praram  int nRegionAreas  区域块数
	//启动该宏块的移动侦测。false为不启动，true为启动
	//!\return  成功为0，失败为-1
	//##ModelId=49BDE672019A
    int Motion_EnableMagicBlock_CGI(int nCh, tagMotionRegion * pRegionMotionCfg, int nMaxRegionAreas);

    //!\brief 人形检测区域设置
	//!\praram int nCh 通道
	//!\praram  tagMotionRegion * pRegionCfg 区域结构体
	//!\praram  int nRegionNum  区域块数
	//!\return  成功为0，失败为-1
    int HumanShape_SetRegionCfg(int nCh, tagMotionRegion *pRegionCfg, int nRegionNum);
    
	//!\brief 获得启用了移动侦测的块
	//!\praram int nCh 通道
	//!\praram tagMotionRegion * cfg 区域结构体
	//!\praram  int nRegionAreas  区域块数
	//!\return  >=0 块个数,否则错误;pMagicBlock = NULL 时返回块个数,缓冲大小不够时返回错误
	//##ModelId=49BDE739029A
    int Motion_GetEnableMagicBlock_CGI(int nCh, tagMotionRegion * pRegionMotionCfg, int nMaxRegionAreas);

	int Motion_SetAudioAlarmSchedule(int nCh, tagAudioAlarmSchedule *stSchedule);

	int Motion_GetAudioAlarmSchedule(int nCh, tagAudioAlarmSchedule *stSchedule);

	int Motion_CheckAudioAlarmInSchedule(int nCh);

	int Motion_SetAudioAlarmCfg(int nCh, int nVoiceType, int nBroadcastCount);

	int Motion_GetAudioAlarmCfg(int nCh, int *nVoiceType, int *nBroadcastCount);
	
    //!\brief 人形检测区域获取
	//!\praram int nCh 通道
	//!\praram tagMotionRegion * cfg 区域结构体
	//!\praram  int nRegionAreas  区域块数
	//!\return  >=0 块个数,否则错误
    int HumanShape_GetRegionCfg(int nCh, tagMotionRegion *pRegionCfg, int nRegionNum);

	//!\brief 布防不规则区域获取
	//!\praram int nCh 通道
	//!\praram struct tagPolygon * cfg 区域结构体
	//!\return  >=0 块个数,否则错误
	int GetPolygonRegionCfg(int nCh, struct tagPolygon *pRegionCfg);

	//!\brief 布防不规则区域设置
	//!\praram int nCh 通道
	//!\praram struct tagPolygon * cfg 区域结构体
	//!\return  >=0 块个数,否则错误
	int SetPolygonRegionCfg(int nCh, struct tagPolygon *pRegionCfg);
    
	//!\brief 如果移动侦测没有使能则使能，如果没有选择区域则设置为全屏
	//!\praram int nCh 通道
	//!\return 成功为0，失败为-1
	int Motion_StartMotionAdjust(int nCh);

	//!\brief 启动/不启动移动侦测
	//!\praram int nCh 通道
	//!\praram int nEnable false为不启动，true为启动
	//!\return  成功为0，失败为-1
	//##ModelId=49BDE7B100B2
	int Motion_Enable(int nCh, int nEnable);

	//!\brief 当前是否开启区域功能
	//!\praram int nCh 通道
	//!\praram int *nFunc 开启的区域功能类型
	//!\return  成功为0，失败为-1
	int AreaDefense_IsEnableFunction(int nCh, int *nFunc);

	//!\brief 设置区域功能的使能
	//!\nFunc 开启的区域功能
	//!\nEnable 区域功能是否使能
	//!\exclusion 功能是否互斥
	//!\return  成功为0，失败为-1
	int AIDetect_SetAIAllFunc(int nCh, int nFunc, int nEnable, int exclusion);

	//!\brief 启动/不启动区域布防移动侦测
	//!\praram int nCh 通道
	//!\praram int nEnable false为不启动，true为启动
	//!\return  成功为0，失败为-1
	//##ModelId=49BDE7B100B2
	int AreaDefense_Motion_Enable(int nCh, int nEnable);

	int PedestrianFiltering_Enable(int nCh, int nEnable);

    //!\brief 启动/不启动人形检测(新接口)
	//!\praram int nCh 通道
	//!\praram int nEnable false为不启动，true为启动
	//!\return  成功为0，失败为-1
	int HumanShape_Enable(int nCh, int nEnable);
	
	//!\brief 启动/不启动区域布防人形检测
	//!\praram int nCh 通道
	//!\praram int nEnable false为不启动，true为启动
	//!\return  成功为0，失败为-1
	int AreaDefense_HumanShape_Enable(int nCh, int nEnable);
	
	int AreaDefense_FaceCap_Enable(int nCh, int nEnable);

	//!\brief 判断一个通道是否启动了移动侦测
	//!\praram int nCh 通道
	//!\return  false为不启动，true为启动
	//##ModelId=49BDE7D101E5
	int Motion_IsEnable(int nCh);

	//!\brief 判断一个通道是区域布防否启动了移动侦测
	//!\praram int nCh 通道
	//!\return  false为不启动，true为启动
	//##ModelId=49BDE7D101E5
	int AreaDefense_Motion_IsEnable(int nCh);

	int PedestrianFiltering_IsEnable(int nCh);

    //!\brief 判断一个通道是否启动了人形检测
	//!\praram int nCh 通道
	//!\return  false为不启动，true为启动
	int HumanShape_IsEnable(int nCh);

	//!\brief 判断一个通道区域布防是否启动了人形检测
	//!\praram int nCh 通道
	//!\return  false为不启动，true为启动
	int AreaDefense_HumanShape_IsEnable(int nCh);
	
	int AreaDefense_FaceCap_IsEnable(int nCh);

	//!\brief 获取移动的时间计划
	//!\param tagSchedule* cfg 时间计划结构体
	//!\return 成功为0，失败为-1
	//##ModelId=49B7164C0058
	int Motion_GetSchedule(int nCh, tagMotionSchedule * cfg);

	//!\brief 获取人形检测时间计划
    //!\param tagSchedule* cfg 时间计划结构体
    //!\return 成功为0，失败为-1
    int HumanShape_GetSchedule(int nCh, tagMotionSchedule *cfg);

	//!\brief 获取区域布防时间计划
    //!\param tagSchedule* cfg 时间计划结构体
    //!\return 成功为0，失败为-1
    int AreaDefense_GetSchedule(int nCh, tagMotionSchedule *cfg);

	//!\brief 设置移动的时间计划
	//!\param ttagSchedule cfg 时间计划结构体
	//!\return 成功为0，失败为-1
	//##ModelId=49B716980242
	int Motion_SetSchedule(int nCh, const tagMotionSchedule * cfg);

	//!\brief 设置区域布防时间计划
    //!\param ttagSchedule cfg 时间计划结构体
    //!\return 成功为0，失败为-1
    int AreaDefense_SetSchedule(int nCh, const tagMotionSchedule *cfg);

	//!\brief 设置声音报警布防时间计划
    //!\param ttagSchedule cfg 时间计划结构体
    //!\return 成功为0，失败为-1
    int AudioAlarm_SetSchedule(int nCh, const tagAudioAlarmSchedule *cfg);

	//!\brief 获取声音报警布防时间计划
    //!\param tagSchedule* cfg 时间计划结构体
    //!\return 成功为0，失败为-1
    int AudioAlarm_GetSchedule(int nCh, tagAudioAlarmSchedule *cfg);

	//检查音频报警时间表
    int AudioAlarm_CheckAudioAlarmInSchedule(int nCh);

	//!\brief 启动/不启动人脸检测
	//!\praram int nCh 通道
	//!\praram int nEnable false为不启动，true为启动
	//!\return  成功为0，失败为-1
	int FaceDetect_Enable(int nCh, int nEnable);

	//!\brief 判断一个通道是否启动了人脸检测
	//!\praram int nCh 通道
	//!\return  false为不启动，true为启动
	int FaceDetect_IsEnable(int nCh);

	//!\brief 获取人脸检测时间计划
    //!\param tagSchedule* cfg 时间计划结构体
    //!\return 成功为0，失败为-1
    int FaceDetect_GetSchedule(int nCh, tagMotionSchedule *cfg);

    //!\brief 设置人脸检测时间计划
    //!\param ttagSchedule cfg 时间计划结构体
    //!\return 成功为0，失败为-1
    int FaceDetect_SetSchedule(int nCh, const tagMotionSchedule *cfg);

	int FaceDetect_SetFaceOverlayInfo(int nCh, int nMainStreamOverlay, int nSubStreamOverlay);

	int FaceDetect_GetFaceOverlayInfo(int nCh, int *nMainStreamOverlay, int *nSubStreamOverlay);

	int HumanShape_SetHumanOverlayInfo(int nCh, char nMainStreamOverlay, char nSubStreamOverlay);
		
	int HumanShape_GetHumanOverlayInfo(int nCh, char *nMainStreamOverlay, char *nSubStreamOverlay);

	int HumanShape_SetDetectAreaOverlayInfo(int nCh, int nMainStreamOverlay, int nSubStreamOverlay);

	int HumanShape_GetDetectAreaOverlayInfo(int nCh, int *nMainStreamOverlay, int *nSubStreamOverlay);

    //!\brief 人形跟踪使能设置
	//!\praram int nCh 通道
	//!\praram int nEnable 人形跟踪使能
	//!\return 0表示成功 -1表示失败
	int SetHumanShapeTrackEnable(int nCh,int nEnable);

	//!\brief 人形跟踪使能获取
	//!\praram int nCh 通道
	//!\return 1=使能开启，0=使能关闭，-1=失败
	int GetHumanShapeTrackEnable(int nCh);

    //!\brief 设置人形检测时间计划
    //!\param ttagSchedule cfg 时间计划结构体
    //!\return 成功为0，失败为-1
    int HumanShape_SetSchedule(int nCh, const tagMotionSchedule *cfg);

    //获取人形帧数据
    //注意使用完成后,第一时间释放
    //否则下次获取将会阻塞(死锁)
    int HumanShape_GetHumanShapeFrame(int nCh, char **pBuff, int *pLen);
	
	int StopRecordAndUinitMemRecord(int nCh);
    //释放人形帧数据
    //与HumanShape_GetHumanShapeFrame配合使用
    int HumanShape_ReleaseHumanShapeFrame(int nCh, char *pBuff);

    int FaceCapSimplify_SetFilterCfg(int nCh, int MinWidth, int MinHeight, int MaxWidth, int MaxHeight);
    
    int FaceCapSimplify_GetFilterCfg(int nCh, int *MinWidth, int *MinHeight, int *MaxWidth, int *MaxHeight);

    int FaceCapSimplify_SetRegionCfg(int nCh, int AreaTopLeftX, int AreaTopLeftY, int AreaBottomRightX, int AreaBottomRightY);
    
    int FaceCapSimplify_GetRegionCfg(int nCh, int *AreaTopLeftX, int *AreaTopLeftY, int *AreaBottomRightX, int *AreaBottomRightY);

    int FaceCapSimplify_GetFaceEventFrame(int nCh, char **pBuff, int *pLen);

    int FaceCapSimplify_ReleaseFaceEventFrame(int nCh, char *pBuff);

    int FaceCapSimplify_SetEventCallBack(int nCh, int (*CallBack) (struct tagAlertInfo *pAlertInfo));

    int AIDetect_GetEventFrame(int nCh, int FrameType, char **pBuff, int *pLen);

    int AIDetect_ReleaseEventFrame(int nCh, int FrameType, char *pBuff);

    int AIDetect_GetEventFrameForPush(int nCh, int FrameType, char **pBuff, int *pLen);

    int AIDetect_ReleaseEventFrameForPush(int nCh, int FrameType, char *pBuff);

	int AIDetect_SetAudioAlarmSchedule(int nCh, tagAudioAlarmSchedule *stSchedule, int nAlarmType);

	int AIDetect_GetAudioAlarmSchedule(int nCh, tagAudioAlarmSchedule *stSchedule, int nAlarmType);

	int AIDetect_CheckAudioAlarmInSchedule(int nCh, int nAlarmType);

	int AIDetect_SetAudioAlarmCfg(int nCh, int nVoiceType, int nBroadcastCount, int nBroadcastVolume, int nAlarmType);

	int AIDetect_GetAudioAlarmCfg(int nCh, int *nVoiceType, int *nBroadcastCount, int *nBroadcastVolume, int nAlarmType);
	
	int AIDetect_SetAudioAlarmCfgV1(int nCh, int nEnterVoiceType,int nLeaveVoiceType, int nEnterVoiceEnable,int nLeaveVoiceEnable,int nBroadcastCount,int nBroadcastVolume,int nAlarmType);

	int AIDetect_GetAudioAlarmCfgV1(int nCh, int *nEnterVoiceType,int *nLeaveVoiceType, int *nEnterVoiceEnable,int *nLeaveVoiceEnable,int *nBroadcastCount,int *nBroadcastVolume,int nAlarmType);

	int TriggerEvent(int nCh, unsigned long dwEventType, unsigned long dwSubEventType, unsigned long dwParam1, unsigned long dwParam2, unsigned long dwParam3, char * pBuffer, unsigned long dwBufferLen);

    /******************************声音侦测***********************************/
	int SoundDetect_SetCfg(int nCh, int nEnable, int nSentive);
	int SoundDetect_GetCfg(int nCh, int *pEnable, int *pSentive);

#ifdef __USE_CIRCLEQUEUE__
	//!\brief 获取码流类型
	//!\praram int nChannel 通道
	//!\return  码流类型
	int GetStreamType(int nChannel);

	//!\brief 获取录像码流索引
	//!\praram int nCh 通道
	//!\return  录像码流索引
	int GetRecStreamIndex(int nCh);
#endif	

	int AudioIn_SetInterface(int mode);

	int AudioIn_GetInterface();

    //G711A G711U  
	int AudioCodec_SetInterface(int ACodec);

	int AudioCodec_GetInterface();	

	int SetTrackStayTime(int nCh, int nStayTimeSec);

	int GetTrackStayTime(int nCh, int *nStayTimeSec);

	int GetTrackStatCfg(int nCh, int *trackEnable, int *linkAction);
	int SetTrackLinkStart(int nCh, int linkStart);

    int SetTrackDataBuf(int nCh, char *sBuff, int nLen);

    //!\brief 设置外部需要音视频数据的回调函数
	//!\param AvCallBack回调函数指针
	//!\param unsigned int tFrameTime 时间戳（秒）
	//!\parma unsigned int tFrameTime_usec 微秒
	//!\return 成功为0，失败为-1
	//##ModelId=49BF41FD021E
	int SetAVCallBack(int (*AvCallBack)
					  (int nCh, char *Buff, int nBuffLen, int Type, int nStreamType, unsigned int tFrameTime, unsigned int tFrameTime_usec));

	//!\brief 获取单个实例
	//!\return 失败为NULL，否则成功
	static CChannelManager *GetInstanceHandle();

	//!\brief 释放实例，只有在实例不再使用时释放（即使用统计为0时）
	static void Release();

    //!\brief 更新睡眠模式
    int UpdateDorStatus(int bDorEnable);

    //!\brief 获取设备当前的睡眠/唤醒状态值
    int GetDorStatus();

  public:
	//////////////////////////////////////////////////////////////////////////
	//以下函数为内部使用，外部不要调用
	//////////////////////////////////////////////////////////////////////////
	//!\brief 音视频数据的回调函数
	//!\param channelNumber通道
	//!\return 成功为0，失败为-1
	static int AvCallBack_FNX(unsigned long  channelNumber);

    static int HumanTrackerCallBack_FNX(char *sBuff, int nLen);

	//!\brief 音视频数据的读取函数
	//!\param hannelNumber通道
	//!\return 成功为0，失败为-1
	int AvReadStream(int  channelNumber,char CallBackType);

	//!\brief 单次状态检测
	//!\param void
	//!\return 成功为0，失败为-1，即退出检测
	int CheckStaus();

	//!\brief 单次计划检测
	//!\param void
	//!\return 成功为0，失败为-1，即退出检测
	int CheckSchedule();

	//!\brief 单次录像新计划检测
	//!\param void
	//!\return 成功为0，失败为-1，即退出检测
	int CheckScheduleV2();
	
	//!\brief 单次喂狗检测
	//!\param void
	//!\return 成功为0，失败为-1，即退出检测
	int CheckWatch();

	//!\brief 抓图检测
	//!\param void
	//!\return 成功为0，失败为-1，即退出检测
	int CheckCapture();

//#ifdef __USE_RTMP__
	int Reconsitution_channel(int reserver);

	int Record_GetPushAddressCfg(char *bsupport, char *enable, char *pushurl );

	int Record_SetPushAddressCfg(char enable ,char *PushURL);

	int Record_SetRtmpParam(int tpye, int nCh, char *szUserName ,char *szPassword ,char *szStreamPullingAddress,unsigned int eable, 
		unsigned int status, char cPushAudioEnable);
	int Record_GetRtmpParam(int tpye, int nCh, char *szUserName ,int userNameSize,char *szPassword , int PassWordSize,
	char *szStreamPullingAddress, int StrPullAddrSize, unsigned int *eable, unsigned int *status, char *szPlayURL, int playURLSize, char *cPushAudioEnable);

	int GetRtmpEable(); 

	int Record_Notif_Creat_Channel(int *ret);
	
	int Record_SetActionCode(char *pCode,int uAllActivedStatusMask);
	
	int Record_GetActionCode(char *pCode,int codeSize,  char *pCodeFlag, char *activestatus, 
		unsigned int *puSupportActivedFeatureMask, unsigned int *puAllActivedStatusMask);
	
#if defined (__IS_SUPPORT_LONGAWAYHOME__)
    int HumanShape_GetLongAwayEnable(int nCh, int *nEnable);
    
    int HumanShape_GetLongAwayDuration(int nCh, int *AlarmDuration);
    
    int HumanShape_SetLongAwayEnable(int nCh, int nEnable);
    
    int HumanShape_SetLongAwayDuration(int nCh, int AlarmDuration);

    int HumanShape_GetLongAwayPushAlarmTime(int nCh, time_t *stime, time_t *etime);
#endif

	//rtmp 句柄
	int RtmpHandle[DVR_MAX_STREAM_NUM][MAX_CH_MANAGER_NUM];
	int ChangeFlag[DVR_MAX_STREAM_NUM][MAX_CH_MANAGER_NUM];	  //是否有参数变化，参数有变化的，要销毁通道重新创建
	unsigned int RtmpStatus[DVR_MAX_STREAM_NUM][MAX_CH_MANAGER_NUM];
	int Veable[DVR_MAX_STREAM_NUM][MAX_CH_MANAGER_NUM];	  

	ZW_RTMP_CLIENT_CONNECT_PARAM ClientConnectParam[DVR_MAX_STREAM_NUM][MAX_CH_MANAGER_NUM];
	char PushAddressEnable; //当前是否使能自动注册推流
	char PushAddressSupport; //系统是否支持自动注册推流
	char szPushURLServer[128];				  // 生成推流地址的服务器URL
	char m_ncPushAudioEnable;  //是否推送音频数据
	int ActionCodeOk;
	int RtmpInit;
	char ActiveCode[64];
//#endif

  private:

	//!\brief 码流信息统计
	//!return 成功为0，其它为错误
	int BitsInfoStat(int nCh);
       int (*m_AlarmCallBack) (struct tagAlertInfo * pAlertInfo, struct tagActionInfo * pActionInfo);


  private:
	static CChannelManager *pCChannelManger;
	//实例使用统计
	static int m_iInstanceCnt;
	//是否初始化
	int m_bInit;
	//是否设置流回调
	int m_cInit;

	//##ModelId=49B0E5690191
	CAreaDefense *theCAreaDefense[MAX_CH_MANAGER_NUM];
	
	CMotion *theCMotion[MAX_CH_MANAGER_NUM];
	CHumanShape *theCHumanShape[MAX_CH_MANAGER_NUM];
    CSoundDetectManager *theCSoundDetect[MAX_CH_MANAGER_NUM];

    //##ModelId=49B0E57002A9
    COptics *theCOptics[MAX_CH_MANAGER_NUM];

	//##ModelId=49B0EEFD0042
	CRecord *theCRecord[MAX_CH_MANAGER_NUM];

	//##ModelId=49B0EEFF0316
	COsd *theCOsd[MAX_CH_MANAGER_NUM];

	//##ModelId=49B0EF030069
	CCoder *theCCoder[MAX_CH_MANAGER_NUM];

	//##ModelId=49B5C8D90115
	CCapturePic *theCCapturePic[MAX_CH_MANAGER_NUM];

	//编码器句柄
	HANDLE theCoderHandle[MAX_CH_MANAGER_NUM];

	//数据缓冲
	char *m_PDataBuf;

	//数据长度
	int m_iLength;

	//主码流信息
	unsigned int m_uiRealBits[MAX_CH_MANAGER_NUM];
	//主码流统计信息
	unsigned long long m_ullRealBits_Stat[MAX_CH_MANAGER_NUM];	
	
	//主码流上一次开始统计时间
	int m_iLastTime[MAX_CH_MANAGER_NUM];

	//辅码流信息
	unsigned int m_uiAuxRealBits[MAX_CH_MANAGER_NUM];
	//辅码流统计信息
	unsigned long long m_ullAuxRealBits_Stat[MAX_CH_MANAGER_NUM];	
	//辅码流上一次开始统计时间
	int m_iAuxLastTime[MAX_CH_MANAGER_NUM];

#ifdef USE_HDAUX
	//高清辅码流信息
	unsigned int m_uiHDAuxRealBits[MAX_CH_MANAGER_NUM];
	//高清辅码流统计信息
	unsigned long long m_ullHDAuxRealBits_Stat[MAX_CH_MANAGER_NUM];	
	//高清辅码流上一次开始统计时间
	int m_iHDAuxLastTime[MAX_CH_MANAGER_NUM];
#endif

	//高清辅码流信息
	unsigned int m_uiFourthRealBits[MAX_CH_MANAGER_NUM];
	//高清辅码流统计信息
	unsigned int m_uiFourthRealBits_Stat[MAX_CH_MANAGER_NUM];
	//高清辅码流上一次开始统计时间
	int m_iFourthLastTime[MAX_CH_MANAGER_NUM];

	//编码通道数
	int m_iEncChannelCnt;

	int m_mainStreamType;	//主码流流类型
	int m_auxStreamType;	//子码流流类型
	int m_HDauxStreamType;	//高清子码流流类型

	//1S帧率计数，用于看门狗重启
	int m_iAV_FrameCnt[MAX_CH_MANAGER_NUM][DVR_MAX_STREAM_NUM];

	//码流操作状态
	int m_iAV_status[MAX_CH_MANAGER_NUM];

	friend class CRecordFile;

	CConfig *pCConfig;
	int m_AudioInChannel;

    //G711A G711U
    int m_AudioCodec;

	//看门狗
	CSysManager *theCSysManager;
	HANDLE m_hWatch;

	//喂狗检测线程
	pthread_t m_hWatchThread;

	//计划检测线程
	pthread_t m_hCheckScheduleThread;

	//新录像计划检测线程	
	pthread_t m_hCheckScheduleThreadV2;
	
	//状态检测线程,联动
	pthread_t m_hCheckThread;

	//抓图检测线程
	pthread_t m_hCheckThread_capture;
	//计划检测线程是否退出
	int m_bCheck_exit;

    //睡眠状态
    int m_bDorEnable;

#ifdef __USE_RTMP__
		//rtmp线程
		pthread_t m_RtmpThread;
#endif

	//外部回调
	int (*m_FnxAvCallBack) (int nCh, char *Buff, int nBuffLen, int Type, int nStreamType, unsigned int tFrameTime, unsigned int tFrameTime_usec);
};

#endif							/* _INC_CCHANNELMANGER_49A251E001B9_INCLUDED */
