
/*!
*\file   CSnapshotManager.cpp 
*\brief  抓图管理
*\author 樊聪
*\date	 2016年7月1日
*\version 0.1
*/

#include "CSnapShot.h"
#include "CSnapshotManager.h"
#include "pthreadmacro.h"
#include "idvr_errno.h"
#include "debug.h"

#include "SnapShot_global.h"

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

CSnapshotManager *CSnapshotManager::theCSnapshotManager = NULL;

/**************************************************************************
*                           局部函数原型                                  *
 **************************************************************************/


/**************************************************************************
*                           方法实现                                      *
 **************************************************************************/

int CSnapshotManager::CapturePic(time_t ntime, time_t utime, char *buffer_data, int nsize)
{
	int ret = 0;

    if (NULL == buffer_data || nsize <= 0)
    {
        return -1;
    }
#if defined (__USE_EMAIL__) || defined (__USE_FTP__) || defined (__SUPPORT_SD_CARD__)
	ret = pCSnapShot->CapturePic(ntime, utime, buffer_data, nsize);
#endif
	return ret;
}

//!\brief   触发搬图
//!\param   nalarm_type  触发联动事件类型(0:Email 1:FTP 2:SD 3:PUSH)
//!\param   ntime  事件触发的时间(秒)
//!\param   utime  事件触发的时间(微秒)
//!\return  0表示成功，其它为错误码
int CSnapshotManager::TrigEvent(int nalarm_type, time_t ntime, time_t utime)
{
	int ret = -1;
#if defined (__USE_EMAIL__) || defined (__USE_FTP__) || defined (__SUPPORT_SD_CARD__)	
	ret = pCSnapShot->TrigEvent(nalarm_type, ntime, utime);
	if (ret)
	{
		ret = IDVR_STR_ERROR_NOTPARTITION;
	}
#endif	
	return ret;
}

//!\brief   获取操作的文件名
//!\param   nalarm_type  触发联动事件的类型(0:Email 1:FTP 2:SD 3:PUSH)
//!\return  0表示成功，其它为错误码
int CSnapshotManager::GetNeedSendPicFileName(int nalarm_type, char *pic_file)
{
	int ret = -1;
#if defined (__USE_EMAIL__) || defined (__USE_FTP__) || defined (__SUPPORT_SD_CARD__)	
	ret = pCSnapShot->GetOpsPicFileName(nalarm_type, pic_file);
	if (ret)
	{
		ret = IDVR_STR_ERROR_NOTPARTITION;
	}
#endif	
	return ret;
}

/*@breif SetSnapShotParam 设置SD卡抓图参数
 *@param [IN] bEnable 是否使能SD卡抓图
 *@param [IN] nTimeInterval 抓图的时间间隔
 *@return 成功返回0
 *@       失败返回-1
 *@note   抓图时间间隔范围在MIN_SNAPSHOT_TIME_INTERVAL和MAX_SNAPSHOT_TIME_INTERVAL之间
 */
int CSnapshotManager::SetSdSnapShotParam(int bEnable, int nTimeInterval)
{
	int ret = -1;
#if defined (__SUPPORT_SD_CARD__)	
	ret = pCSnapShot->SetSdSnapShotParam(bEnable, nTimeInterval);
	if (ret)
	{
		ret = IDVR_STR_ERROR_NOTPARTITION;
	}
#endif	
	return ret;
}

/*@breif GetSdSnapShotParam 获取SD卡抓图参数
 *@param [OUT] pEnable 是否使能SD卡抓图
 *@param [OUT] pTimeInterval 抓图的时间间隔
 *@return 成功返回0
 *@       失败返回-1
 *@note   抓图时间间隔范围在MIN_SNAPSHOT_TIME_INTERVAL和MAX_SNAPSHOT_TIME_INTERVAL之间
 */
int CSnapshotManager::GetSdSnapShotParam(int *pEnable, int *pTimeInterval)
{
	int ret = -1;
#if defined (__SUPPORT_SD_CARD__)	
	ret = pCSnapShot->GetSdSnapShotParam(pEnable, pTimeInterval);
	if (ret)
	{
		ret = IDVR_STR_ERROR_NOTPARTITION;
	}
#endif	
	return ret;
}

/*@breif SetFTPSnapShotParam 设置FTP抓图参数
 *@param [IN] bEnable 是否使能FTP抓图
 *@param [IN] nTimeInterval 抓图的时间间隔
 *@return 成功返回0
 *@       失败返回-1
 *@note   抓图时间间隔范围在MIN_SNAPSHOT_TIME_INTERVAL和MAX_SNAPSHOT_TIME_INTERVAL之间
 */
int CSnapshotManager::SetFTPSnapShotParam(int bEnable, int nTimeInterval)
{
	int ret = -1;
#if defined (__USE_FTP__)
	ret = pCSnapShot->SetFTPSnapShotParam(bEnable, nTimeInterval);
	if (ret)
	{
		ret = IDVR_STR_ERROR_NOTPARTITION;
	}
#endif	
	return ret;
}

/*@breif GetFTPSnapShotParam 获取FTP抓图参数
 *@param [OUT] pEnable 是否使能FTP抓图
 *@param [OUT] pTimeInterval 抓图的时间间隔
 *@return 成功返回0
 *@       失败返回-1
 *@note   抓图时间间隔范围在MIN_SNAPSHOT_TIME_INTERVAL和MAX_SNAPSHOT_TIME_INTERVAL之间
 */
int CSnapshotManager::GetFTPSnapShotParam(int *pEnable, int *pTimeInterval)
{
	int ret = -1;
#if defined (__USE_FTP__)
	ret = pCSnapShot->GetFTPSnapShotParam(pEnable, pTimeInterval);
	if (ret)
	{
		ret = IDVR_STR_ERROR_NOTPARTITION;
	}
#endif	
	return ret;
}

int CSnapshotManager::SetTimedCaptureCallBack(int (*CallBack)(int capture_type))
{
	int ret = -1;
	
	ret = pCSnapShot->SetTimedCaptureCallBack(CallBack);
	if (ret)
	{
		ret = IDVR_STR_ERROR_NOTPARTITION;
	}

	return ret;
}

CSnapshotManager *CSnapshotManager::GetInstance()
{
	// TODO: Add your specialized code here.

	if (NULL == theCSnapshotManager)
	{
		//初始化快照管理器
#ifndef TEST_MEM_INFO
		theCSnapshotManager = new CSnapshotManager();
#else
		theCSnapshotManager = CommonKit::TCreateObject<CSnapshotManager>::CreateObject(__FILE__, __FUNCTION__, __LINE__);
#endif
	}

	return (CSnapshotManager *) theCSnapshotManager;
}

int CSnapshotManager::ReleaseInstance()
{
	// TODO: Add your specialized code here.    
	if (NULL != theCSnapshotManager)
	{
#ifndef TEST_MEM_INFO
		delete theCSnapshotManager;
#else
		CommonKit::TDestroyObject<CSnapshotManager>::DestroyObject(theCSnapshotManager);
#endif

		theCSnapshotManager = NULL;
	}

	return (int) 0;
}

CSnapshotManager::CSnapshotManager()
{
    pCSnapShot = CSnapShot::GetInstance();
    if (pCSnapShot == NULL)
    {
        TRACE("CSnapShot==NULL\n");
    }
}

CSnapshotManager::~CSnapshotManager()
{
    
}
