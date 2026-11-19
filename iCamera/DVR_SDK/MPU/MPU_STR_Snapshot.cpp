
/*!
 *\file		MPU_STR_Snapshot.cpp
 *\brief	MPU模块，快照管理相关接口实现
 *\author	林云
 *\date		2009/04/07

功能：
-#	快照文件的列表查询、信息获取、锁定与解锁、存盘。
 */


#include "MPU_global.h"

#include "SnapShot/CSnapshotManager.h"

/**************************************************************************
 *                     局部宏定义                                         *
 **************************************************************************/

#ifndef DEBUG_NOT_USE_SNP_LIB	//调试时，不使用STR库

//调用快照管理器执行动作
#define MPU_STR_SSM_CALL(x)									\
do															\
{															\
	int nRet;													\
															\
	CSnapshotManager *pMgr = CSnapshotManager::GetInstance();		\
															\
	if (NULL != pMgr)											\
	{														\
		nRet = pMgr->x;										\
		if (IDVR_OK != nRet)									\
		{													\
			MPU_ERR("%s, nRet = %d\n", #x, nRet);				\
		}													\
	}														\
	else														\
	{														\
		MPU_ERR("Get CSnapshotManager instance fail!\n");			\
		MPU_CHK(NULL != pMgr);								\
		nRet = IDVR_MPU_STR_NO_SNAPSHOT_MANAGER_INSTANCE;	\
	}														\
															\
	return nRet;												\
}															\
while (0)

//调用快照管理器获取参数
#define MPU_STR_SSM_GET(DataType, DefaultValue, Function)		\
do															\
{															\
	DataType retValue;											\
															\
	CSnapshotManager *pMgr = CSnapshotManager::GetInstance();		\
															\
	if (NULL != pMgr)											\
	{														\
		retValue = pMgr->Function;								\
	}														\
	else														\
	{														\
		MPU_ERR("Get CSnapshotManager instance fail!\n");			\
		MPU_CHK(NULL != pMgr);								\
		retValue = DefaultValue;								\
	}														\
															\
	return retValue;											\
}															\
while (0)

#else

//调用快照管理器执行动作
#define MPU_STR_SSM_CALL(x)									\
do															\
{															\
	return IDVR_OK;											\
}															\
while (0)

//调用快照管理器获取参数
#define MPU_STR_SSM_GET(DataType, DefaultValue, Function)		\
do															\
{															\
	return DefaultValue;										\
}															\
while (0)

#endif							/* #ifndef DEBUG_NOT_USE_SNP_LIB */

/**************************************************************************
 *                     快照管理                                           *
 **************************************************************************/

//!\brief   锁定文件
//!\param   hHandle : HANDLE 查询的句柄
//!\param   int nIndex 文件在总个数中的索引。从0开始
//!\return  成功为0，其它为错误码
int MPU_STR_SnapShotM_Lock(HANDLE hHandle, int nIndex)
{
	//MPU_DBG("enter\n");

	//调用快照管理器执行
	//MPU_STR_SSM_CALL(Lock(hHandle, nIndex));
    return 0;
}

//!\brief   解锁文件
//!\param   hHandle : HANDLE 查询的句柄
//!\param   int nIndex 文件在总个数中的索引。从0开始
//!\return  成功为0，其它为错误码
int MPU_STR_SnapShotM_Unlock(HANDLE hHandle, int nIndex)
{
	//MPU_DBG("enter\n");

	//调用快照管理器执行
	//MPU_STR_SSM_CALL(Unlock(hHandle, nIndex));	
    return 0;
}


//!\brief   锁定文件
//!\param   char *FileName : 文件全路径
//!\return  成功为0，其它为错误码
int MPU_STR_SnapShotM_Lock(char *FileName)
{
	//MPU_STR_SSM_CALL(Lock(FileName));
	return 0;
}

//!\brief   解锁文件
//!\param   char *FileName : 文件全路径
//!\return  成功为0，其它为错误码
int MPU_STR_SnapShotM_Unlock(char *FileName)
{
	// (Unlock(FileName));	
    return 0;
}

//!\brief   获取最后一次抓图的文件名(文件有可能被淘汰)
//!\param   int nalarm_type   触发联动事件类型(0:Email 1:FTP 2:SD 3:PUSH))
//!\param   char *szFileName  输出文件名称(根据触发事件类型不同，获取到的名字组成不同)
//!\return  成功为0，失败为-1
int MPU_STR_SnapShotM_GetLastPic(int nCh, int nalarm_type, char *szFileName)
{
	MPU_STR_SSM_CALL(GetNeedSendPicFileName(nalarm_type, szFileName));
}

//!\brief   触发搬图
//!\param   int nalarm_type   触发联动事件类型(0:Email 1:FTP 2:SD 3:PUSH))
//!\param   ntime  事件触发的时间(秒)
//!\param   utime  事件触发的时间(微秒)
//!\return  0表示成功，其它为错误码
int MPU_STR_TrigEventCapturePic(int nalarm_type, time_t ntime, time_t utime)
{
    MPU_STR_SSM_CALL(TrigEvent(nalarm_type, ntime, utime));
}

/*@breif MPU_STR_SetSdSnapShotParam 设置SD卡抓图参数
 *@param [IN] bEnable 是否使能SD卡抓图
 *@param [IN] nTimeInterval 抓图的时间间隔
 *@return 成功返回0
 *@       失败返回-1
 *@note   抓图时间间隔范围在MIN_SNAPSHOT_TIME_INTERVAL和MAX_SNAPSHOT_TIME_INTERVAL之间
 */
int MPU_STR_SetSdSnapShotParam(int bEnable, int nTimeInterval)
{
	MPU_STR_SSM_CALL(SetSdSnapShotParam(bEnable, nTimeInterval));
}

/*@breif MPU_STR_GetSdSnapShotParam 获取SD卡抓图参数
 *@param [OUT] pEnable 是否使能SD卡抓图
 *@param [OUT] pTimeInterval 抓图的时间间隔
 *@return 成功返回0
 *@       失败返回-1
 *@note   抓图时间间隔范围在MIN_SNAPSHOT_TIME_INTERVAL和MAX_SNAPSHOT_TIME_INTERVAL之间
 */
int MPU_STR_GetSdSnapShotParam(int *pEnable, int *pTimeInterval)
{
	MPU_STR_SSM_CALL(GetSdSnapShotParam(pEnable, pTimeInterval));
}

/*@breif MPU_STR_SetFTPSnapShotParam 设置FTP抓图参数
 *@param [IN] bEnable 是否使能FTP抓图
 *@param [IN] nTimeInterval 抓图的时间间隔
 *@return 成功返回0
 *@       失败返回-1
 *@note   抓图时间间隔范围在MIN_SNAPSHOT_TIME_INTERVAL和MAX_SNAPSHOT_TIME_INTERVAL之间
 */
int MPU_STR_SetFTPSnapShotParam(int bEnable, int nTimeInterval)
{
	MPU_STR_SSM_CALL(SetFTPSnapShotParam(bEnable, nTimeInterval));
}

/*@breif MPU_STR_GetFTPSnapShotParam 获取FTP抓图参数
 *@param [OUT] pEnable 是否使能FTP抓图
 *@param [OUT] pTimeInterval 抓图的时间间隔
 *@return 成功返回0
 *@       失败返回-1
 *@note   抓图时间间隔范围在MIN_SNAPSHOT_TIME_INTERVAL和MAX_SNAPSHOT_TIME_INTERVAL之间
 */
int MPU_STR_GetFTPSnapShotParam(int *pEnable, int *pTimeInterval)
{
	MPU_STR_SSM_CALL(GetFTPSnapShotParam(pEnable, pTimeInterval));
}

int MPU_STR_SetTimedCaptureCallBack(int (*CallBack)(int capture_type))
{
	MPU_STR_SSM_CALL(SetTimedCaptureCallBack(CallBack));
}

/**************************************************************************
 *                     注销局部宏定义                                     *
 **************************************************************************/
#undef MPU_STR_SSM_CALL			//调用快照管理器执行动作
#undef MPU_STR_SSM_GET			//调用快照管理器获取参数
