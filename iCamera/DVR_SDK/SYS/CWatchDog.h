// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CWATCHDOG_49B5BEB301CE_INCLUDED
#define _INC_CWATCHDOG_49B5BEB301CE_INCLUDED

#include "HANDLE.h"

/**************************************************************************
*                            常量                                        *
**************************************************************************/

/**************************************************************************
*                            宏                                          *
**************************************************************************/

/**************************************************************************
*                          数据类型                                       *
**************************************************************************/
//支持注册多个被检测的线程或进程，若存在一个线程或者进程停止喂
//狗，则看门狗自动重启设备。
//
//如果需要多个模块使用该类，则建议作为基础模块，任意一个模块需
//要使用可以任意使用，待讨论
//##ModelId=49B5BEB301CE
class CWatchDog
{
  public:
	//!\brief 添加一个看门狗任务
	//!\praram char *szDogName 看门狗名字
	//!\praram int nTimer 
	//需要喂狗的时间。喂狗时间大于该时间可能会导致失败重启
	//!\return  成功为该任务的句柄，失败为INVALID_HANDLE
	//##ModelId=49BB333003E6
	HANDLE AddDogTask(const char *szDogName, int nTimer);

	//!\brief 注销一个看门狗任务
	//!\praram HWND handle 看门狗任务句柄
	//!\return  成功为0，失败为-1
	//##ModelId=49BB33310013
	int DestroyDogTask(HANDLE handle);

	//!\brief 喂狗操作
	//!\praram HWND handle 看门狗任务句柄
	//!\return  成功为0，失败为-1
	//##ModelId=49BB33310026
	int WatchDog(HANDLE handle);

};

#endif							/* _INC_CWATCHDOG_49B5BEB301CE_INCLUDED */
