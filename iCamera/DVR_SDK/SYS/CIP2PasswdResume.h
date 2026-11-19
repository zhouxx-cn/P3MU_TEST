
/*!
*\file   CIP2PasswdResume.h
*\brief  复位的声明
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/


#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CIP2PASSWDRESUME_49B5C4C000C7_INCLUDED
#define _INC_CIP2PASSWDRESUME_49B5C4C000C7_INCLUDED
#include <semaphore.h>

/**************************************************************************
*                            常量                                        *
**************************************************************************/

/**************************************************************************
*                            宏                                          *
**************************************************************************/

/**************************************************************************
*                          数据类型                                       *
**************************************************************************/

//!密码/IP复位类
//##ModelId=49B5C4C000C7
class CIP2PasswdResume
{
  public:
	//!\brief 开始复位检测
	//!\praram 无
	//!\return  成功为0，失败为-1
	//##ModelId=49BB333102B1
	int StartResumeTask();

	//!\brief 开始复位检测
	//!\praram 无
	//!\return  成功为0，失败为-1
	//##ModelId=49BB333102B1
	int StopResumeTask();

	//!\brief 开始复位检测
	//!\praram void(*CallBack)(void) 回调函数指针
	//!\return  成功为0，失败为-1
	//##ModelId=49BB333102BB
	int SetResumeCallBack(void (*CallBack) (void));

	//!\brief 设置测试模式
	//!\praram nTestMode 模式值 0-出厂模式， 1-工厂模式
	//!\return  成功为0，失败为-1
	int SetProductTestMode(int nTestMode);
	
	//!\brief 获取复位状态
	//!\praram nStatus 状态值 0-没有复位 1-已经按下复位
	//!\return	成功为0，失败为-1
	int GetHardResetStatus(int *nStatus);

	//!\brief 清空硬复位状态
	void  ClearHardResetStatus();
	
	CIP2PasswdResume();
  private:

	static void *work_thread(void *org);
	static sem_t cond;
	typedef void (*ResumeCallBack) ();

	ResumeCallBack m_CallBack;

	int bWorkTrheadRun;

	int m_nProductTestMode;  //工厂测试模式下， 不进行硬复位 1-工厂模式 0-表普通朴实
	int m_nHardResetStatus;

};

#endif							/* _INC_CIP2PASSWDRESUME_49B5C4C000C7_INCLUDED */
