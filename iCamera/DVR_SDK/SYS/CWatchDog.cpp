
/*!
*\file   CUsbUpdate.cpp 
*\brief  USB升级的实现
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/

#include "CWatchDog.h"
#include "watch_api.h"
// #include "sys_banned.h"
#include "global_gather.h"

/**************************************************************************
*                            常量                                        *
**************************************************************************/

/**************************************************************************
*                            宏                                          *
**************************************************************************/

/**************************************************************************
*                          数据类型                                       *
**************************************************************************/


/**************************************************************************
*                           全局变量                                      *
**************************************************************************/

/**************************************************************************
*                           局部函数原型                                  *
 **************************************************************************/

/**************************************************************************
*                           方法实现                                      *
**************************************************************************/


//!\brief 添加一个看门狗任务
//!\praram char *szDogName 看门狗名字
//!\praram int nTimer 
//需要喂狗的时间。喂狗时间大于该时间可能会导致失败重启
//!\return  成功为该任务的句柄，失败为INVALID_HANDLE
//##ModelId=49BB333003E6
HANDLE CWatchDog::AddDogTask(const char *szDogName, int nTimer)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	int Id = 0;

	WATCH_AddTask(nTimer, &Id, szDogName);
	return (HANDLE) Id;
}


//!\brief 注销一个看门狗任务
//!\praram HWND handle 看门狗任务句柄
//!\return  成功为0，失败为-1
//##ModelId=49BB33310013
int CWatchDog::DestroyDogTask(HANDLE handle)
{
	// TODO: Add your specialized code here.
	int ret;

	ret = WATCH_RemoveTask((int) handle);
	return (int) ret;
}


//!\brief 喂狗操作
//!\praram HWND handle 看门狗任务句柄
//!\return  成功为0，失败为-1
//##ModelId=49BB33310026
int CWatchDog::WatchDog(HANDLE handle)
{
	int ret;

	ret = WATCH_NotifyTaskLive((int) handle);
	return (int) ret;
}
