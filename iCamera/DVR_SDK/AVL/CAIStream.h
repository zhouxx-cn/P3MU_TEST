// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CAISTREAM_49B0E4200207_INCLUDED
#define _INC_CAISTREAM_49B0E4200207_INCLUDED
#include "CConfig.h"
#include "AVL_global.h"
#include "XRS_PTZ_TRACKING_STRUCT.h"


//智能帧分析
//##ModelId=49B0E4200207
class CAIStream
{
  public:
	//!\brief 实例化一个对象
	//!\praram int(*CallBack)(int nCh) 回调指针
	//!\return  成功为0，设备为-1
	//##ModelId=49B0EEBF0039
	CAIStream(int nCh, void *hHandle);

    ~CAIStream();

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
	int DealAIStreamData(char *sBuff, int nLen);

 	int CheckAIStream();

	int SetTrackStayTime(int nStayTimeSec);

	int GetTrackStayTime(int *nStayTimeSec);

	int GetTrackStatCfg(int *nSmartTrackEnable, int *linkAction);
	int SetTrackLinkStart(int linkStart);
    //!\brief 设置人形跟踪使能
	//!\return 0表示成功 -1表示失败
	int SetTrackSwitchParam(int iEnable);

	//!\brief 获取人形跟踪使能
	//!\return 1表示开启 0表示关闭 -1表示失败
	int GetTrackSwitchParam();
#if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)
	 int SetTrackZoomEnable(int nEnable);
 
	 int GetTrackZoomEnable(int *nEnable);
 
	 //!\brief 恢复到人形跟踪自动Zoom前的倍数
	 //!\return  0表示成功 -1表示失败
	 int RecoverSaveTrackZoomRatio();
 
	 //!\brief 人形在画面消失后，恢复到特定倍数
	 //!\return  0表示成功 -1表示失败
	 int RecoverTmpTrackZoomRatio();
#endif

 private:
	//通道号
	int m_iChannel;
	//编码句柄
	HANDLE m_henc;

	// 是否响应围栏/越界的联动项(联动录像一定响应)
	int m_linkAction;
	int m_linkStart;

	CConfig *pCConfig;

	int m_Track_Status;  //m_Track_Status = 0;未跟踪 >0:跟踪中

	int m_nHorizonMaxStep;//球机水平总步长，来自定制文件（手动测量得出）
	int m_nVerticalMaxStep;

	PTZ_TRACK_INPUT_PARAM_ST param;

	int m_nHorizonMaxStepInPic;//水平视野的总步长
	int m_nVerticalMaxStepInPic;
#if defined(__IS_SUPPORT_LONG_SHORT_FOCUS_ZOOM__)
	int m_nHorizonMaxStepInPic_Long;//水平视野的总步长
	int m_nVerticalMaxStepInPic_Long;
#endif
	int m_panThr;//目标在画面中允许偏离视野中心的水平偏差
	int m_tiltThr;//目标在画面中允许偏离视野中心的垂直偏差

	double m_HorizonviewingAngle;//水平视场角
	double m_VorizonviewingAngle;//垂直视场角

	int m_horizontalTotalFieldAngle;//球机水平可转动的角度
	int m_VerticalTotalFieldAngle;//球机垂直可转动的角度
	int m_trackSwitch;      //人形跟踪开关
	int m_trackSwitchBak;   //人形跟踪开关(备份)
 	int m_TrackZoomEn = 0;	//人形跟踪变倍功能使能

    void *pTrackHandle;//跟踪算法的handle
};
#endif							/* _INC_CAISTREAM_49B0E4200207_INCLUDED */

