// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CHUMANSHAPE_49B0E4200206_INCLUDED
#define _INC_CHUMANSHAPE_49B0E4200206_INCLUDED
#include "global_macro.h"
#include "tagAlertInfo.h"
#include "tagActionInfo.h"
#include "CConfig.h"
#ifdef DO_SATA
#include "hisdk_sata.h"
#else
//#include "hisdk.h"
#include "zwsdk.h"
#endif
#include "RECT.h"
#include "AVL_global.h"
#include "tagMotionSchedule.h"


class CActionAlarm;

//移动侦测参数配置及报警检测
//##ModelId=49B0E4200206
class CHumanShape
{
  public:
	//!\brief 实例化一个对象
	//!\praram int(*CallBack)(int nCh) 回调指针
	//!\return  成功为0，设备为-1
	//##ModelId=49B0EEBF0039
	CHumanShape(int nCh, void *hHandle);

    ~CHumanShape();

	//!\brief 初始化
	//!\return 成功为0,否则为-1
	//!\param int nMaxCh, int nMaxAlarmOut分别为最大通道数、最大报警输出数
	int Init(int nMaxCh, int nMaxAlarmOut);

	//!\brief 释放  资源
	//!\return 成功为0，否则为-1
	int UnInit();

	//!\brief 处理人形检测帧
	//!\praram sBuff : char*移动帧数据Buff
	//!\praram int nLen 移动帧数据长度
	//!\return  成功为0，失败为-1
	//##ModelId=49B0E69303CA
	int DealHumanShapeData(int Chn,char *sBuff, int nLen);

	//获取人形帧数据
    //注意使用完成后,第一时间释放
    //否则下次获取将会阻塞(死锁)
    int GetHumanShapeFrame(char **pBuff, int *pLen);
    
    //释放人形帧数据
    //与GetHumanShapeFrame配合使用
    int ReleaseHumanShapeFrame(char *pBuff);

	//!\brief 设置人形检测灵敏度
	//!\praram int nCh 通道
	//!\praram int iGrade 灵敏度
	//!\return  成功为0，失败为-1
	//##ModelId=49BDEC840040
	int SetDetectPrecision(int iGrade);

	//!\brief 获取灵敏度
	//!\praram int nCh 通道
	//!\return  返回灵敏度
	//##ModelId=49BDEC840086
	int GetDetectPrecision();

    //!\brief 人形检测区域设置
	//!\praram int nCh 通道
	//!\praram  tagMotionRegion * pRegionCfg 区域结构体
	//!\praram  int nRegionNum  区域块数
	//!\return  成功为0，失败为-1
    int SetRegionCfg(tagMotionRegion *pRegionCfg, int nRegionNum);

	//!\brief 人形检测不规则区域设置
	//!\praram int nCh 通道
	//!\praram  struct tagPolygon * pRegionCfg 区域结构体
	//!\praram  int nRegionNum  区域块数
	//!\return  成功为0，失败为-1
    int SetPolygonRegionCfg(struct tagPolygon *pRegionCfg, int nRegionNum);

    //!\brief 人形检测区域获取
	//!\praram int nCh 通道
	//!\praram  tagMotionRegion * pRegionCfg 区域结构体
	//!\praram  int nRegionNum  区域块数
	//!\return  成功为0，失败为-1
    int GetRegionCfg(tagMotionRegion *pRegionCfg, int nRegionNum);

	//!\brief 人形检测不规则区域获取
	//!\praram int nCh 通道
	//!\praram  tagMotionRegion * pRegionCfg 区域结构体
	//!\praram  int nRegionNum  区域块数
	//!\return  成功为0，失败为-1
    int GetPolygonRegionCfg(struct tagPolygon *pRegionCfg, int nRegionNum);

	//!\brief 启动/不启动人形检测
	//!\praram int nCh 通道
	//!\praram int nEnable false为不启动，true为启动
	//!\return  成功为0，失败为-1
	//##ModelId=49BDEC840176
	int Enable(int nEnable);
	
	//!\brief 判断一个通道是否启动了人形检测
	//!\praram int nCh 通道
	//!\return  false为不启动，true为启动
	//##ModelId=49BDEC8401C6
	int IsEnable();

	//!\brief 设置报警联动回调函数
	//!\praram int(*CallBack)(struct tAlertInfo *pAlertInfo,  
	//struct tActionInfo *pActionInfo)) 报警回调指针
	//!\return  成功为0，失败为-1
	//##ModelId=49BDEC8401EE
	static int SetAlarmCallBack(int AlarmType, int (*CallBack) (struct tagAlertInfo * pAlertInfo, struct tagActionInfo * pActionInfo));

	//!\brief 启用某个报警联动动作
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
	int EnableAction(int Action, int bEnable);

	//!\brief 判断某个报警联动动作是否开启
	//!\praram int Action    !      ACTION_SCREEN 屏幕显示
	//  !      ACTION_BEEP 声音提示
	//  !      ACTION_ALARMOUT 报警输出
	//  !      ACTION_SENDTOSERVER 上传报警服务器
	//  !      ACTION_EMAIL 联动Email
	//  !      ACTION_SMS 联动短信
	//!\return false 为禁止联动 true 为开启联动,
	//对于联动屏幕显示0为不启用，1为联动主显,2为联动辅显,3为联动主显和辅显
	//##ModelId=49BDEFDF0081
	int IsEnableAction(int Action);

	//!\brief 获取某个报警联动录像参数
	//!\param dwChannelFlag 联动的通道标记 
	//每一位表示联动一个通道 1表示联动通道1 
	//2表示联动通道2录像 3表示联动通道1和2录像,以此类推
	//!\param nPort 某个报警通道号
	//!\return 0表示获取成功，-1表示获取失败
	//##ModelId=49B7187602BF
	int GetRecordParam(int *dwChannelFlag);

	//!\brief 设置某个报警联动录像参数
	//!\param dwChannelFlag 联动录像通道标记
	//!\param nPort 某个报警通道号
	//!\return 0表示设置成功 -1表示设置失败
	//##ModelId=49B718640232
	int SetRecordParam(int dwChannelFlag);

	//!\brief 
	//获取某个报警联动拍照参数,拍照张数和间隔由CCapturePic::GetCfg获取
	//!\param dwChannelFlag 存储联动拍照通道标记的缓冲区
	//!\param nPort 报警联动输入通道号
	//!\return 0表示成功 -1表示失败
	//##ModelId=49B5CE9A030D
	int GetSnapshotParam(int *dwChannelFlag);

	//!\brief 
	//设置某个报警联动拍照参数，拍照张数和拍照间隔由CCapturePi
	//c::SetCfg设定
	//!\param dwChannelFlag 联动拍照通道标记
	//!\param nPort 某个报警通道号
	//!\return 0表示设置成功 -1表示失败
	//##ModelId=49B5CDFE00BB
	int SetSnapshotParam(int dwChannelFlag);

	//!\brief 获取某一某个报警通道联动某一云台的动作参数
	//!\param nAction 联动动作 
	//*nAction=0表示无动作，1联动预置点，2联动巡航，3联动轨迹
	//!\param nIndex 联动预置点/巡航/轨迹的编号
	//!\param nChannel 云台对应的通道号
	//!\param nPort 某个报警通道号
	//!\return 0表示获取成功, -1表示获取失败
	//##ModelId=49B718B202DE
	int GetPtzParam(int nChannel, int *nAction, int *nIndex);

	//!\brief 设置某个报警通道联动某一个云台的动作
	//!\param nAction 联动的动作 0表示无动作 1联动预置点 
	//2联动巡航 3联动轨迹
	//!\param nIndex 联动的预置点/巡航/轨迹的编号
	//!\param nChannel 云台对应的通道号
	//!\param nPort 某个报警通道号
	//!\return 0表示成功 -1表示失败
	//##ModelId=49B718BD0261
	int SetPtzParam(int nChannel, int nAction, int nIndex = 0);

	//!\brief 获取某个报警联动报警输出的参数
	//!\param dwOutPortFlag 存储输出通道标记的缓冲区
	//!\param nPort 某个报警通道号
	//!\return 0表示成功 -1表示失败
	//##ModelId=49B718DD030D
	int GetDopParam(int *dwOutPortFlag);

	//!\brief 设置某个报警联动报警输出的参数
	//!\param dwOutPortFlag 设置输出通道标记
	//!\param nPort 某个报警通道号
	//!\return 0表示成功 -1表示失败
	//##ModelId=49B718ED00AB
	int SetDopParam(int dwOutPortFlag);

	int SetPushEventIntvCnt(int nPushEventIntv);

	int GetPushEventIntvCnt(int *nPushEventIntv);

	//!\brief 获取人形的时间计划
	//!\param tagSchedule* cfg 时间计划结构体
	//!\return 成功为0，失败为-1
	//##ModelId=49B7164C0058
	int GetSchedule(tagMotionSchedule * cfg);

	//!\brief 设置人形的时间计划
	//!\param ttagSchedule cfg 时间计划结构体
	//!\return 成功为0，失败为-1
	//##ModelId=49B716980242
	int SetSchedule(const tagMotionSchedule * cfg);

	int SetHumanOverlayInfo(char nMainStreamOverlay, char nSubStreamOverlay);
		
	int GetHumanOverlayInfo(char *nMainStreamOverlay, char *nSubStreamOverlay);

	int SetDetectAreaOverlayInfo(int nMainStreamOverlay, int nSubStreamOverlay);

	int GetDetectAreaOverlayInfo(int *nMainStreamOverlay, int *nSubStreamOverlay);
#if defined (__IS_SUPPORT_LONGAWAYHOME__)
	int ProcessLongAway(const struct tm &curr_tm, time_t now);

    int GetLongAwayEnable(int *nEnable);

    int SetLongAwayEnable(int nEnable);

    int GetLongAwayDuration(int *AlarmDuration);

    int SetLongAwayDuration(int AlarmDuration);
    
    int SetLongAwayPushAlarmTime(time_t stime, time_t etime);
    
    int GetLongAwayPushAlarmTime(time_t *stime, time_t *etime);
#endif

	//检测人形
	int CheckHumanShape();

  private:
	//通道号
	int m_iChannel;
	//编码句柄
	HANDLE m_henc;
	//人形过滤使能
	int m_bEnable;
	//AI265+使能
	int m_bAi265Enable;
	//AIFACE使能
	int m_bAiFaceEnable;
	//主码流人形框叠加使能
	int m_bMainStreamHumanOverlay;
	//子码流人形框叠加使能
	int m_bSubStreamHumanOverlay;
	//主码流布防框框叠加使能
	int m_bMainStreamDetectAreaOverlay;
	//子码流布防框叠加使能
	int m_bSubStreamDetectAreaOverlay;
	//检测精度
	int m_iPrecision;
	//有效区域个数
	int m_iRectNum;
    //人形检测区域坐标信息
    tagMotionRegion  m_RegionInfo[MAX_MOTION_REGION_NUM];
	struct tagPolygon m_RegionInfos[MAX_HUMANSHAPE_REGION_NUM];
	//是否有触发.
	int m_bTrigger;
	//触发时的通道号
	int	m_nChn;
	//一秒内是否有触发，或者上次读取后是否有触发
	int m_lastTrigger;
#if defined (__IS_SUPPORT_LONGAWAYHOME__)

	//长久离家使能
	int m_awayHomeEnable;

	// 长久离家时间,单位:小时;4-4小时,6-6小时,8-8小时,10-10小时,12-12小时 
	int m_awayHomeAlarmDuration; 
	
	// 上次离家报警时间，用于周期性报警
	time_t m_lastAlarmTime;
	time_t m_PushAlarmSTime;
	time_t m_PushAlarmETime;
#endif
	//报警类型
	static int m_AlarmType;
	//动作处理
	CActionAlarm *m_pActionAlarm;
	//配置文件
	CConfig *pCConfig;
	//移动计划
	tagMotionSchedule m_ScheduleCfg;
	//回调函数
	static ALARMCALLBACK m_FxnCallBack;
	
private:
	//!\brief 载入设置
	int LoadParam();
	//人形配置应用
	int HumanShapeCfgApply();
};
#endif							/* _INC_CHUMANSHAPE_49B0E4200206_INCLUDED */

