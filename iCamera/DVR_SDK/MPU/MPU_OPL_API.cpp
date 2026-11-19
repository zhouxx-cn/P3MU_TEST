
/*!
 *\file		MPU_OPL_API.cpp
 *\brief	MPU模块，日志相关接口实现
 *\author	林云
 *\date		2009/04/10

功能：
-#	支持日志的写入、查询（按类型与时间段）
-#	支持日志文件的回滚
-#	用于记录用户的操作信息（包括登录、注销、报警事件等）
-#	该模块只提供写入、读取的方法，具体日志内容有使用者模块决定

 */


#include "MPU_global.h"

#include "OPL/CLogManager.h"


/**************************************************************************
 *                            声明局部宏定义                              *
 **************************************************************************/

#ifndef DEBUG_NOT_USE_OPL_LIB	//调试时，不调用OPL库

//调用日志管理器执行动作
#define MPU_OPL_CALL(x)									\
do														\
{														\
	int nRet;												\
														\
	CLogManager *pMgr = CLogManager::OPL_GetInstance();		\
														\
	if (NULL != pMgr)										\
	{													\
		nRet = pMgr->x;									\
														\
		if (IDVR_OK != nRet)								\
		{												\
			MPU_ERR("%s, nRet = %d\n", #x, nRet);			\
		}												\
	}													\
	else													\
	{													\
		MPU_ERR("Get CLogManager instance fail!\n");			\
		MPU_CHK(NULL != pMgr);							\
		nRet = IDVR_MPU_OPL_NO_MANAGER_INSTANCE;			\
	}													\
														\
	return nRet;											\
}														\
while (0)

#else

//调用日志管理器执行动作
#define MPU_OPL_CALL(x)									\
do														\
{														\
	return IDVR_OK;										\
}														\
while (0)

#endif							/* #ifndef DEBUG_NOT_USE_OPL_LIB */


/**************************************************************************
 *                            模块内部接口                                *
 **************************************************************************/

//!\brief   日志模块初始化
//!\return  0表示初始化成功 -1表示初始化失败
int OPL_Init()
{
#ifndef DEBUG_NOT_USE_OPL_LIB
	CLogManager::OPL_GetInstance();
#endif

	return IDVR_OK;
}

//!\brief   日志模块资源清除
//!\return  0表示成功 其他表示错误码
int OPL_Uninit()
{
#ifndef DEBUG_NOT_USE_OPL_LIB
	CLogManager *pMgr = CLogManager::OPL_GetInstance();

	CLogManager::OPL_DestroyInstance(pMgr);
#endif

	return IDVR_OK;
}


/**************************************************************************
 *                            日志                                        *
 **************************************************************************/

//!\brief   添加新日志
//!\param   nLogType            日志类型
//!\param   nDetail             日志详细信息
//!\param   nAlertChannel       产生日志信息的通道号
//!\param   dwRecordChannelFlag 联动录像的通道标记
//!\param   dwLocalOrRemote     本地或远程（IP）,0表示本地，其他表示IP
//!\param   strUsername         用户名
//!\return  0添加成功 -1表示失败
int MPU_OPL_AddLog(int nLogType, int nDetail, int nAlertChannel, unsigned long dwRecordChannelFlag, unsigned long dwLocalOrRemote, char *strUsername)
{
#ifndef DEBUG_NOT_USE_OPL_LIB	//调试时，不调用OPL库
	//调用日志管理器执行动作
	MPU_OPL_CALL(OPL_AddLog(nLogType, nDetail, nAlertChannel, dwRecordChannelFlag, dwLocalOrRemote, strUsername));
#endif
    return IDVR_OK;
}

//!\brief   开始一个查询
//!\param   nType               日志类型
//!\param   nStartTime          查询起始时间
//!\param   nStopTime           查询结束时间
//!\param   nPageItem           每页包含的日志数
//!\param   pQueryCallback      查询准备好后的回调typedef 
//!\note    void(CALLBACK* CBQueryLogCallback)(HANDLE hQueryHandle, int inPageNum);
//!\note    参数    hQueryHandle 查询句柄
//!\note            nPageNum    日志总数
//!\param   pHandle             保存查询句柄
//!\return  0表示查询成功,-1表示查询失败
int MPU_OPL_QueryStart(int nType, int nStartTime, int nStopTime, int nPageItem, CBQueryLogCallback pQueryCallback, HANDLE * pHandle)
{
#ifndef DEBUG_NOT_USE_OPL_LIB	//调试时，不调用OPL库
	//调用日志管理器执行动作
	MPU_OPL_CALL(OPL_QueryStart(nType, nStartTime, nStopTime, nPageItem, pQueryCallback, pHandle));
#endif

    return IDVR_OK;
}

//!\brief   获取日志的某一页
//!\param   hQueryLog   查询句柄
//!\param   pLogBuf     日志信息缓冲区指针
//!\param   nLogBufLen  志信息缓冲区长度（容纳多少条日志）
//!\param   nPageNum    要查询的页码
//!\param   nLogItem    存储实际查询到的日志条数
//!\return  0表示查询成功 其他为查询失败
int MPU_OPL_QueryPage(HANDLE hQueryLog, struct tagLog *pLogBuf, int nLogBufLen, int nPageNum, int *nLogItem)
{
#ifndef DEBUG_NOT_USE_OPL_LIB	//调试时，不调用OPL库
	//调用日志管理器执行动作
	MPU_OPL_CALL(OPL_QueryPage(hQueryLog, pLogBuf, nLogBufLen, nPageNum, nLogItem));
#endif
    return IDVR_OK;
}

//!\brief   销毁日志查询资源
//!\param   hQueryLog           日志查询句柄
//!\return  0表示成功,其他表示错误
int MPU_OPL_QueryStop(HANDLE hQueryLog)
{
#ifndef DEBUG_NOT_USE_OPL_LIB	//调试时，不调用OPL库
	//调用日志管理器执行动作
	MPU_OPL_CALL(OPL_QueryStop(hQueryLog));
#endif
    return IDVR_OK;
}


/**************************************************************************
 *                            注销局部宏定义                              *
 **************************************************************************/

#undef MPU_OPL_CALL				//调用日志管理器执行动作
