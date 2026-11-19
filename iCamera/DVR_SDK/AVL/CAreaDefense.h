// Copyright (C) 1991 - 1999 Rational Software Corporation

#ifndef __AREADEFENSE_H__
#define __AREADEFENSE_H__

#include "global_macro.h"
#include "tagAlertInfo.h"
#include "tagActionInfo.h"
#include "CConfig.h"

#ifdef DO_SATA
#include "hisdk_sata.h"
#else
#include "zwsdk.h"
#endif

#include "RECT.h"
#include "AVL_global.h"
#include "tagMotionSchedule.h"


class CActionAlarm;

// 区域布防相关
// 该类用于统一管理不规则区域布防的配置, 不控制具体功能
class CAreaDefense
{
  public:
	//!\brief 实例化一个对象
	//!\praram int(*CallBack)(int nCh) 回调指针
	//!\return  成功为0，设备为-1
	//##ModelId=49B0EEBF0039
	CAreaDefense(int nCh, void *hHandle);

    ~CAreaDefense();

	//!\brief 初始化
	//!\return 成功为0,否则为-1
	//!\param int nMaxCh, int nMaxAlarmOut分别为最大通道数、最大报警输出数
	int Init(int nMaxCh, int nMaxAlarmOut);

	//!\brief 释放  资源
	//!\return 成功为0，否则为-1
	int UnInit();

	bool Motion_IsEnable();
	bool HumanShape_IsEnable();
	bool FaceCap_IsEnable();
	bool CarDetect_IsEnable();

	int Motion_SetEnable(bool e);
	int HumanShape_SetEnable(bool e);
	int FaceCap_SetEnable(bool e);
	int CarDetect_SetEnable(bool e);

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

	//!\brief 获取区域布防的时间计划
	//!\param tagSchedule* cfg 时间计划结构体
	//!\return 成功为0，失败为-1
	//##ModelId=49B7164C0058
	int GetSchedule(tagMotionSchedule * cfg);

	//!\brief 设置区域布防的时间计划
	//!\param ttagSchedule cfg 时间计划结构体
	//!\return 成功为0，失败为-1
	//##ModelId=49B716980242
	int SetSchedule(const tagMotionSchedule * cfg);
		
	int GetPolygonRegionCfg(struct tagPolygon *pRegionCfg, int nRegionNum);

	int SetPolygonRegionCfg(struct tagPolygon *pRegionCfg, int nRegionNum);

	int SetRegionCfg(tagMotionRegion *pRegionCfg, int nRegionNum);

  private:
	// 通道号
	int m_iChannel;
	// 编码句柄
	HANDLE m_henc;

	// 使能状态
	bool m_bMotionEnable;
	bool m_bHumanShapeEnable;
	bool m_bFaceCapEnable;
	bool m_bCarDetectEnable;

	// //有效区域个数
	int m_iRectNum;
	struct tagPolygon m_RegionInfos[MAX_AREADEFENSE_NUM];

	// 报警类型
	// static int m_AlarmType;
	// 动作处理
	CActionAlarm *m_pActionAlarm;
	// 配置文件
	CConfig *pCConfig;
	// 布防时间
	tagMotionSchedule m_ScheduleCfg;

private:
	//!\brief 载入设置
	int LoadParam();
};
#endif							/* _INC_CHUMANSHAPE_49B0E4200206_INCLUDED */

