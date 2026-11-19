
/*!
*\file   CIP2PasswdResume.cpp 
*\brief  复位的实现
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/


#include <pthread.h>
#include "global_macro.h"
#include "debug.h"
#include "idvr_errno.h"
#include "macro_pthread.h"
#include "CIP2PasswdResume.h"
// #include "sys_banned.h"
#include "global_gather.h"
extern "C"
{
	//int MP4Sys_GetSecretPinStatus();
	//int MP4Sys_InitDSPs();
	int ZWSys_Init();
	int ZWSys_GetResetPinStatus(int *pResetStatus);
};

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
sem_t CIP2PasswdResume::cond;

/**************************************************************************
*                           局部函数原型                                  *
 **************************************************************************/

/**************************************************************************
*                           方法实现                                      *
 **************************************************************************/

//!\brief 开始复位检测
//!\praram 无
//!\return  成功为0，失败为-1
//##ModelId=49BB333102B1
int CIP2PasswdResume::StartResumeTask()
{
	// TODO: Add your specialized code here.
	int status;

	pthread_attr_t new_attr;

	pthread_t thread_t;

	pthread_attr_init(&new_attr);
	pthread_attr_setdetachstate(&new_attr, PTHREAD_CREATE_DETACHED);

	CSYS_INF("创建工作线程\n");
	if (bWorkTrheadRun == 0)
	{
		bWorkTrheadRun = 1;
		status = pthread_create(&thread_t, &new_attr, CIP2PasswdResume::work_thread, this);

		if (status != 0)
		{
			CSYS_ERR("创建工作线程失败\n");
			return IDVR_STR_ERROR_DISK_THREAD_FAILED;
		}
	}
	CSYS_INF("创建工作线程成功\n");
	return 0;
}

//!\brief 构造函数
CIP2PasswdResume::CIP2PasswdResume()
{
	bWorkTrheadRun = 0;
	m_CallBack = NULL;
	m_nProductTestMode = 0;
	m_nHardResetStatus =0;
	sem_init(&cond, 0, 0);
}

//!\brief 工作线程
void *CIP2PasswdResume::work_thread(void *org)
{
    PRT_TID_NAME("CIP2PasswdResume");

	struct timespec ts;

	ts.tv_sec = 1;
	ts.tv_nsec = 0;
	int nSleepSum = 0;

	int ret = 1;

	CIP2PasswdResume *handle = (CIP2PasswdResume *) org;

	if (NULL == handle)
	{
		CSYS_ERR("实例未创建，无法进行检测\n");
		return NULL;
	}
	BEGINRUN_THREAD;

	while (handle->bWorkTrheadRun)
	{
		ZWSys_GetResetPinStatus(&ret);
		//ret = 0;
		//CSYS_INF("ret = %d\n", ret);
		if (0 == ret)
		{
			// TRACE("检测到复位信号\n");
			if (1 == handle->m_nProductTestMode)
			{
				handle->m_nHardResetStatus = 1;
			}

			//复位逻辑使用CheckSecretPinStatusThread里的
			#if 0
			if ((NULL != handle->m_CallBack) &&  (0 == handle->m_nProductTestMode))
			{
				handle->m_CallBack();
			}
			#endif
		}
		//CSYS_INF("磁盘检测线程正在运行\n");

		nSleepSum = 0;

		//下面的睡眠方式，防止线程退出过慢的问题
		while ((nSleepSum++) < 1 /*需要扩大 */ )
		{
			if (handle->bWorkTrheadRun)
			{
				ts.tv_sec = 1;
				ts.tv_nsec = 0;
				nanosleep(&ts, NULL);
			}
			else
			{
				nSleepSum = 0;
				break;
			}
		}
	}
	CSYS_INF("复位工作线程退出\n");
	sem_post(&cond);
	ENDRUN_THREAD;
	return NULL;
}

//!\brief 开始复位检测
//!\praram 无
//!\return  成功为0，失败为-1
//##ModelId=49BB333102B1
int CIP2PasswdResume::StopResumeTask()
{
	// TODO: Add your specialized code here.
	bWorkTrheadRun = 0;
	sem_wait(&cond);
	sem_destroy(&cond);
	return (int) 0;
}

//!\brief 开始复位检测
//!\praram void(*CallBack)(void) 回调函数指针
//!\return  成功为0，失败为-1
//##ModelId=49BB333102BB
int CIP2PasswdResume::SetResumeCallBack(void (*CallBack) (void))
{
	// TODO: Add your specialized code here.
	m_CallBack = CallBack;
	return (int) 0;
}

//!\brief 设置测试模式
//!\praram nTestMode 模式值 0-出厂模式， 1-工厂模式
//!\return  成功为0，失败为-1
int CIP2PasswdResume::SetProductTestMode(int nTestMode)
{
	if(nTestMode < 0)
	{
		return -1;
	}

	m_nProductTestMode = nTestMode;

	return 0;
}

//!\brief 获取复位状态
//!\praram nStatus 状态值 0-没有复位 1-已经按下复位
//!\return  成功为0，失败为-1
int CIP2PasswdResume::GetHardResetStatus(int *nStatus)
{
	if(NULL == nStatus)
	{
		return -1;
	}

	*nStatus = m_nHardResetStatus;
	
	return 0;
}

//!\brief 清空硬复位状态
void  CIP2PasswdResume::ClearHardResetStatus()
{
	m_nHardResetStatus =0;
}

