// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CMOTION_49B0E4200206_INCLUDED
#define _INC_CMOTION_49B0E4200206_INCLUDED
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
class CMotion
{
  public:
	//!\brief 实例化一个对象
	//!\praram int(*CallBack)(int nCh) 回调指针
	//!\return  成功为0，设备为-1
	//##ModelId=49B0EEBF0039
	CMotion(int nCh, void *hHandle);

	 ~CMotion();

	//!\brief 初始化
	//!\return 成功为0,否则为-1
	//!\param int nMaxCh, int nMaxAlarmOut分别为最大通道数、最大报警输出数
	int Init(int nMaxCh, int nMaxAlarmOut);

	//!\brief 释放  资源
	//!\return 成功为0，否则为-1
	int UnInit();


	//!\brief 定制移动模式
	//!\param int nCustomMode 0为一般模式，1为高级模式
	//!\return 成功为0，失败为-1
	//##ModelId=49B864580125
	static int SetMode(int nCustomMode);

	//!\brief 获取定制的信息
	//!\return 0为一般模式，1为高级模式
	//##ModelId=49B8653002CA
	static int GetMode();

	//!\brief 处理移动侦测帧
	//!\praram sBuff : char*移动帧数据Buff
	//!\praram int nLen 移动帧数据长度
	//!\return  成功为0，失败为-1
	//##ModelId=49B0E69303CA
	int DealMotionData(int Chn,char *sBuff, int nLen);



	//!\brief 设置移动侦测灵敏度
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

	//!\brief 移动一个移动块的移动侦测
	//!\praram int pMagicBlock 启用块的索引序列数组
	//!\praram  int nBlockNum  使能的块数
	//!\return  成功为0，失败为-1
	//##ModelId=49BDEC8400A4
	int EnableMagicBlock(unsigned char *pMagicBlock, int nBlockNum);

	//!\brief 获得启用了移动侦测的块
	//!\praram int pMagicBlock 启用块的索引序列数组
	//!\praram  int nMaxBlockNum  缓冲最大可接受的块数(一般 16x12块)
	//!\return  >=0 块个数,否则错误;pMagicBlock = NULL 时返回块个数,缓冲大小不够时返回错误
	//##ModelId=49BDEC840126
	int GetEnableMagicBlock(unsigned char *pMagicBlock, int nMaxBlockNum);

    int EnableMagicBlock_CGI(tagMotionRegion * pcfg, int nRegionAreas);

    int GetEnableMagicBlock_CGI(tagMotionRegion * pcfg, int nRegionAreas);

	//!\brief 启动/不启动移动侦测
	//!\praram int nCh 通道
	//!\praram int nEnable false为不启动，true为启动
	//!\return  成功为0，失败为-1
	//##ModelId=49BDEC840176
	int Enable(int nEnable);

	int PedestrianFilteringEnable(int nEnable);
	
	//!\brief 判断一个通道是否启动了移动侦测
	//!\praram int nCh 通道
	//!\return  false为不启动，true为启动
	//##ModelId=49BDEC8401C6
	int IsEnable();

	int IsPedestrianFilteringEnable();

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

	//!\brief 获取移动的时间计划
	//!\param tagSchedule* cfg 时间计划结构体
	//!\return 成功为0，失败为-1
	//##ModelId=49B7164C0058
	int GetSchedule(tagMotionSchedule * cfg);

	//!\brief 设置移动的时间计划
	//!\param ttagSchedule cfg 时间计划结构体
	//!\return 成功为0，失败为-1
	//##ModelId=49B716980242
	int SetSchedule(const tagMotionSchedule * cfg);

	//!\brief 如果移动侦测没有使能则使能，如果没有选择区域则设置为全屏
	//!\return 成功为0，失败为-1
	int startMotionCfgAdjust();

	//检测移动
	int CheckMotion();

	int SetAudioAlarmSchedule(tagAudioAlarmSchedule *stSchedule);

	int GetAudioAlarmSchedule(tagAudioAlarmSchedule *stSchedule);

	int CheckAudioAlarmInSchedule();

	int SetAudioAlarmCfg(int nVoiceType, int nBroadcastCount);

	int GetAudioAlarmCfg(int *nVoiceType, int *nBroadcastCount);

  private:
	//通道号
	int m_iChannel;
	//编码句柄
	HANDLE m_henc;
	//移动使能
	int m_bMotionEnable;
	//人形过滤使能
	int m_bPedestrianFilteringEnable;
	//检测精度
	int m_iPrecision;
	//有效矩形块
	int m_iRectNum;
	//移动矩形块
	char m_pRect[MAX_MAGIC_BLOCK_NUM];
	//按位设置 24个字节
	char m_pBit_Rect[MAX_MAGIC_BLOCK_NUM/8];
	//块使能=192/32=6
	int m_blockEnable[6];		//1~32

    //移动侦测坐标信息(CGI)
    tagMotionRegion  m_RegionInfo[MAX_MOTION_REGION_NUM];
    //移动侦测区域个数(CGI)
    int m_RegionCnt;
	//有效矩形块(CGI)
	int m_iRectNumCgi;
	//移动矩形块(CGI) //MotionDetectGridToi_t
	char m_pRectCgi[MAX_MAGIC_BLOCK_NUM];
	//按位设置24个字节，表示这里的16*19=192个宏块需要192/8=24个字节来设置(CGI)
	char m_pBit_RectCgi[MAX_MAGIC_BLOCK_NUM/8];
	//块使能，整型占4个字节，占32位， 六个整型为24个字节，每个字节8位，一共有6 * 32 = 192位，对应192个宏块设置
	int m_blockEnableCgi[6];

    //当前是否CGI配置起效
    int m_bCgiIsEffective;

	//定制移动模式,0-一般,1-高级
	static int m_iCustomMode;

	//移动结果
	int *m_pMotionResult;
	
	//是否有移动.
	int m_bisMotion;

	//通道号
	int m_Chn;
	
	//一秒内是否有移动，或者上次读取后是否有移动
	int m_lastMotion;

	//报警类型
	static int m_AlarmType;
	
	//动作处理
	CActionAlarm *m_pActionAlarm;
	
	//配置文件
	CConfig *pCConfig;

	//移动计划
	tagMotionSchedule m_ScheduleCfg;

	//声音报警布防时间
	tagAudioAlarmSchedule m_AudioAlarmScheduleCfg;

	//回调函数
	static ALARMCALLBACK m_FxnCallBack;
	
  private:

	//移动侦测属性
	MotionDetectGridToi_t *m_pMotionDetectToiInfo;
	MotionDetectGridToi_t *m_pMotionDetectToiInfoCGI;
	
	//!\brief 载入设置
	int LoadParam();
};

#endif							/* _INC_CMOTION_49B0E4200206_INCLUDED */
