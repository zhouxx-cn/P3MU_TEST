
/*!
 *\file		MPU_STR_Recor.cpp
 *\brief	MPU模块，录像相关接口实现
 *\author	林云
 *\date		2009/04/07

功能：
-#	录像查询信息（时间段，录像类型等）
-#	支持动态弹出方式，支持查询方式
-#	启动、停止录像
-#	录像计划管理
 */


#include "MPU_global.h"

#ifdef __SUPPORT_SD_CARD__
#include "STR/CDPManager.h"
#endif

/**************************************************************************
 *                     声明局部宏定义                                     *
 **************************************************************************/

#ifndef DEBUG_NOT_USE_STR_LIB	//调试时，不使用STR库

//调用录像管理器执行动作
#define MPU_STR_DPM_CALL(x)									\
do															\
{															\
	int nRet;													\
															\
	CDPManager *pMgr = CDPManager::GetInstance();				\
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
		MPU_ERR("Get CDPManager instance fail!\n");				\
		MPU_CHK(NULL != pMgr);								\
		nRet = IDVR_MPU_STR_NO_RECORD_MANAGER_INSTANCE;		\
	}														\
															\
	return nRet;												\
}															\
while (0)

#else

//调用录像管理器执行动作
#define MPU_STR_DPM_CALL(x)									\
do															\
{															\
	return IDVR_OK;											\
}															\
while (0)

#endif							/* #ifndef DEBUG_NOT_USE_STR_LIB */

/**************************************************************************
 *                     录像信息查询                                       *
 **************************************************************************/

//!\brief   创建一个数据片段信息弹出器
//!\param   void
//!\return  成功为弹出器句柄，失败为NULL
void *MPU_STR_DSM_Create()
{
#ifndef DEBUG_NOT_USE_STR_LIB
	void *hHandle;

	//获取录像管理器实例指针
	CDPManager *pMgr = CDPManager::GetInstance();

	if (NULL != pMgr)
	{
		hHandle = pMgr->DSM_Create();
		if (NULL == hHandle)
		{
			MPU_ERR("DSM_Create() fail\n");
			MPU_CHK(NULL != hHandle);
		}
	}
	else
	{
		MPU_ERR("Get CDPManager instance fail!\n");
		MPU_CHK(NULL != pMgr);
		hHandle = NULL;
	}

//  MPU_DBG("return %p\n", hHandle);
	return hHandle;
#else
	return NULL;
#endif
}

//!\brief   销毁一个数据片段信息弹出器
//!\param   void* hHandle       弹出器句柄
//!\return  成功为0，其它为错误码
int MPU_STR_DSM_Destroy(void *hHandle)
{
//  MPU_DBG("enter\n");

	//调用录像管理器执行动作
	MPU_STR_DPM_CALL(DSM_Destroy(hHandle));
}

//!\brief   设置弹出器属性
//!\param   void* hHandle       弹出器句柄
//!\param   int nDiskGroupMask   0表示查询普通分组+报警分组、1表示冗余分组、2表示备份分组
//!\param   time_t tStartTime   开始时间
//!\param   time_t tStopTime    结束时间
//!\param   SEGMENT_CALLBACK pCallback  录像片段信息回调函数
//!\return  成功为0，其它为错误码
int MPU_STR_DSM_SetAttr(void *hHandle, int nDiskGroupMask, time_t tStartTime, time_t tStopTime, SEGMENT_CALLBACK pCallback)
{
//  MPU_DBG("nDiskGroupMask = %d, tStartTime = %lu, tStopTime = %lu\n", nDiskGroupMask, tStartTime, tStopTime);
//  MPU_PRINT_TIME(tStartTime);
//  MPU_PRINT_TIME(tStopTime);

	//调用录像管理器执行动作
	MPU_STR_DPM_CALL(DSM_SetAttr(hHandle, nDiskGroupMask, tStartTime, tStopTime, pCallback));
}

//!\brief   添加一个通道
//!\param   void* hHandle       弹出器句柄
//!\param   int nChannel        通道号
//!\param   int bDynPop         新片段是否动态弹出
//!\return  成功为0，其它为错误码
int MPU_STR_DSM_AddCh(void *hHandle, int nChannel, int bDynPop)
{
//  MPU_DBG("nChannel = %d, bDynPop = %d\n", nChannel, bDynPop);

	//调用录像管理器执行动作
	MPU_STR_DPM_CALL(DSM_AddCh(hHandle, nChannel, bDynPop));
}

//!\brief   删除一个通道
//!\param   void* hHandle       弹出器句柄
//!\param   int nChannel        通道号
//!\return  成功为0，其它为错误码
int MPU_STR_DSM_DelCh(void *hHandle, int nChannel)
{
//  MPU_DBG("nChannel = %d\n", nChannel);

	//调用录像管理器执行动作
	MPU_STR_DPM_CALL(DSM_DelCh(hHandle, nChannel));
}

//!\brief   开始弹出
//!\param   void* hHandle       弹出器句柄
//!\return  成功为0，其它为错误码
int MPU_STR_DSM_StartPop(void *hHandle)
{
//  MPU_DBG("enter\n");

	//调用录像管理器执行动作
	MPU_STR_DPM_CALL(DSM_StartPop(hHandle));
}

//!\brief   结束弹出
//!\param   void* hHandle       弹出器句柄
//!\return  成功为0，其它为错误码
int MPU_STR_DSM_StopPop(void *hHandle)
{
//  MPU_DBG("enter\n");

	//调用录像管理器执行动作
	MPU_STR_DPM_CALL(DSM_StopPop(hHandle));
}

//!\brief   获取录像片段信息
//!\param   int nDiskGroupMask      0表示查询普通分组+报警分组、1表示冗余分组、2表示备份分组
//!\param   int nChannel            通道
//!\param   time_t tStartTime       开始时间
//!\param   time_t tStopTime        结束时间    
//!\param   int nRecType            录像类型
//!\param   RECSEG **pRecSegHead    返回录像片段指针
//!\param   int *pnCount            返回录像片段的个数
//!\return  成功为0，其它为错误码
int MPU_STR_DSM_GetRecSegList(int nDiskGroupMask, int nChannel, time_t tStartTime, time_t tStopTime, int nRecType, tagRecSeg ** pRecSegHead,
							  int *pnCount)
{
	//MPU_DBG("enter\n");

	//调用录像管理器执行动作
	MPU_STR_DPM_CALL(DSM_GetRecSegList(nDiskGroupMask, nChannel, tStartTime, tStopTime, nRecType, pRecSegHead, pnCount));
}

//!\brief   释放录像片段信息
//!\param   tagRecSeg *pRecSegHead      链表指针
//!\return  成功为0，其它为错误码
int MPU_STR_DSM_ReleaseRecSegList(tagRecSeg * pRecSegHead)
{
	//MPU_DBG("enter\n");

	//调用录像管理器执行动作
	MPU_STR_DPM_CALL(DSM_ReleaseRecSegList(pRecSegHead));
}


/**************************************************************************
 *                     录像备份管理                                       *
 **************************************************************************/

//!\brief        判断某天是否有录像数据
//!\param   int nDiskGroupMask 0表示查询普通分组+报警分组、1表示冗余分组、2表示备份分组
//!\param   int nYear
//!\param   int nMon
//!\param   int nDay
//!\return  有数据返回true,无数据返回false
bool MPU_STR_DP_IsFileExist(int nDiskGroupMask, int nYear, int nMon, int nDay)
{
//  MPU_DBG("nDiskGroupMask %d, nYear %d, nMon %d, nDay %d\n", nDiskGroupMask, nYear, nMon, nDay);
#ifndef DEBUG_NOT_USE_STR_LIB
	return CDPManager::IsFileExist(nDiskGroupMask, nYear, nMon, nDay);
#else
	return -1;
#endif
}

//!\brief   判断某月是否有录像数据
//!\param   int DiskGroupMask 0表示查询普通分组+报警分组、1表示冗余分组、2表示备份分组
//!\param   int nYear
//!\param   int nMon
//!\return  0-30分别代表1日制31日
int MPU_STR_DP_IsFileExist(int nDiskGroupMask, int nYear, int nMon)
{
//  MPU_DBG("nDiskGroupMask %d, nYear %d, nMon %d\n", nDiskGroupMask, nYear, nMon);
#ifndef DEBUG_NOT_USE_STR_LIB

	return CDPManager::IsFileExist(nDiskGroupMask, nYear, nMon);
#else
	return -1;
#endif

}

#undef MPU_STR_DPM_CALL			//调用录像管理器执行动作
