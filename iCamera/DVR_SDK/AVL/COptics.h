// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_COPTICS_49B0E53B03A7_INCLUDED
#define _INC_COPTICS_49B0E53B03A7_INCLUDED
#include <time.h>
#include "global_macro.h"
#include "CConfig.h"
#ifdef DO_SATA
#include "hisdk_sata.h"
#else
//#include "hisdk.h"
#include "zwsdk.h"
#endif
#include "AVL_global.h"

//光学参数配置
//分为一般模式与高级模式，之间可以相互切换
//##ModelId=49B0E53B03A7
class COptics
{
  public:
	//!\brief 获取句柄
	//!\return  成功为为句柄，失败为NULL
	//##ModelId=49BB333100F8
	static COptics *GetInstance(int nCh, void *hHandle);
	  
	//##ModelId=49B0EEE301D5
	COptics(int nCh, void *hHandle);

	//!\brief 获取图像参数配置的工作模式
	//!\return  
	//成功为工作模式。0为普通工作模式，1为高级工作模式，失败为
	//-1
	//##ModelId=49BDEEBD023D
	static int GetWorkMode();

	//!\brief 设置图像参数配置的工作模式
	//!\praram int nWorkMode 
	//0为普通工作模式，1为高级工作模式，失败为-1
	//!\return  成功为0，失败为-1
	//##ModelId=49BDEEBD0265
	static int SetWorkMode(int nWorkMode);

	//!\brief 设置图像参数
	static int GetSupport(int type);
	//!\praram int TimeSec 
	//时间段。0为缺省参数（普通工作模式下只需要设置缺省参数即
	//可），1为时间段1参数...   
	//!\praram  int Type 
	//参数类型。0为亮度，1为色度，2为对比度，3为饱和度
	//!\praram  int value 数值。0-255
	//!\return  成功为0，失败为-1
	//##ModelId=49BDEEBD02BF
	int SetPicParam(int TimeSec, int Type, int value);

	//!\brief 设置是否为编辑状态，图像参数
	//!\praram int bEdit 1-开始编辑，暂停自动配置。0-退出编辑，开始启动自动
	//!\return  成功为0，失败为-1
	//##ModelId=49BDEEBD02BF
	int EnterEdit(int bEdit);

	//!\brief 获取图像参数
	//!\praram int TimeSec 
	//时间段。0为缺省参数（普通工作模式下只需要设置缺省参数即
	//可），1为时间段1参数...   
	//!\praram  int Type 
	//参数类型。0为亮度，1为色度，2为对比度，3为饱和度
	//!\return  成功为对应参数，失败为-1
	//##ModelId=49BDEEBD0373
	int GetPicParam(int TimeSec, int Type);

	//!\brief 设置图像时间段参数
	//!\praram int TimeSec 时间段。1为时间段1参数...    
	//!\praram  int iStartTime 开始时间 相对00:00时的秒数   
	//!\praram  int iStopTime 结束时间  相对00:00时的秒数   
	//!\return  成功为0，失败为-1
	//##ModelId=49BDEEBE000D
	int SetTimeCfg(int timesec, int iStartTime, int iStopTime);

	//!\brief 获取图像时间段参数
	//!\praram int TimeSec 时间段。1为时间段1参数...    
	//!\praram  int *piStartTime 开始时间 相对00:00时的秒数 
	//!\praram  int *piStopTime 结束时间    相对00:00时的秒数   
	//!\return  
	//1为启用了该时间段，0为没有启用该时间段，-1获取失败
	//##ModelId=49BDEEBE00C2
	int GetTimeCfg(int timesec, int *piStartTime, int *piStopTime);

	//检测时间段
	int CheckOptics();

	static int GetKeyStateCallback(KEYCLICK_T status);

	//!\brief 开始复位检测
	//!\praram void(*CallBack)(void) 回调函数指针
	//!\return  成功为0，失败为-1
	//##ModelId=49BB33310116
	int SetResumeOptCallBack(void (*CallBack) (void));
	
  private:
	static COptics *theCOptManager;
	//通道号
	int m_iChannel;
	//编码句柄
	HANDLE m_henc;
	//当前时间段模式,0-缺省,1,2,3分别为时间段1,2,3
	int m_itimeSeg;
	//图像参数值
	unsigned int m_iValue[4][5];
	//时间段
	unsigned int m_iStartTime_h[4];
	unsigned int m_iStartTime_m[4];
	unsigned int m_iStopTime_h[4];
	unsigned int m_iStopTime_m[4];

	//工作模式
	static int m_bWorkMode;

	//标记参数改变
	int m_bCfgChange;
	
	//senser类型
	BoardSensorType_t SensorType;

	//是否处于编辑状态
	int m_bEdit;

	typedef void (*ResumeOptCallBack)();

	ResumeOptCallBack m_OptCallBack;

	static int m_SupportDeFrog;
	static int m_SupportShutterMode;
	static int m_SupportLDC;

	//配置文件
	CConfig *pCConfig;
  private:
	int LoadParam();

};

#endif							/* _INC_COPTICS_49B0E53B03A7_INCLUDED */
