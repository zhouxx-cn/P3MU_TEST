
/*!
*\file   CLogManager.h
*\brief  日志管理类定义
*\author yuk@higheasyrd.com
*\date	  2009/04/28
*\version 0.1
*/

#ifndef __CLOGMANAGER_H_
#define __CLOGMANAGER_H_

#include <time.h>
#include "HANDLE.h"
#include "CBQueryLogCallback.h"
#include "tagLog.h"
#include "PUB/DVR_LIST.h"

//!\brief 日志管理类
//##ModelId=499E60290204
class CLogManager
{
  public:
	//!\brief 开始一个查询
	//!\param nType 日志类型
	//!\param nStartTime 查询起始时间
	//!\param nStopTime 查询结束时间
	//!\param nPageItem 每页包含的日志数
	//!\param pQueryCallback 查询准备好后的回调typedef 
	//void(CALLBACK* CBQueryLogCallback)(HANDLE hQueryHandle, int inPageNum);
	//!参数 hQueryHandle 查询句柄
	//!     nPageNum 日志总数
	//!\param pHandle  保存查询句柄
	//!\return 0表示查询成功,-1表示查询失败
	//##ModelId=49B4B4A2001F
	int OPL_QueryStart(int nType, time_t tStartTime, time_t tStopTime, int nPageItem, CBQueryLogCallback pQueryCallback, HANDLE * pHandle);

	//!\brief 添加新日志
	//!\param nLogType 日志类型
	//!\param nDetail 日志详细信息
	//!\param nAlertChannel  产生日志信息的通道号
	//!\param dwRecordChannelFlag 联动录像的通道标记
	//!\param dwLocalOrRemote 本地或远程（IP）,0表示本地，其他表示IP
	//!\param strUsername 用户名
	//!\return 0添加成功 -1表示失败
	//##ModelId=49B4B2D402AF
	int OPL_AddLog(int nLogType, int nDetail, int nAlertChannel, unsigned long long u64RecordChannelFlag, unsigned long dwLocalOrRemote,
				   char *strUsername);

	//!\brief  获取日志的某一页
	//!\param hQueryLog 查询句柄
	//!\param pLogBuf 日志信息缓冲区指针
	//!\param nLogBufLen日志信息缓冲区长度（容纳多少条日志）
	//!\param nPageNum 要查询的页码
	//!\param nLogItem 存储实际查询到的日志条数
	//!\return 0表示查询成功 其他为查询失败
	//##ModelId=49B4B74A01D4
	int OPL_QueryPage(HANDLE hQueryLog, struct tagLog *pLogBuf, int nLogBufLen, int nPageNum, int *nLogItem);

	//!\brief 销毁日志查询资源
	//!\param hQueryLog 指向日志查询句柄的指针
	//!\return 0表示成功,其他表示错误
	//!\note 销毁资源后,句柄不再有效，不能再次用于OPL_QueryPage查询
	//##ModelId=49B4B739000F
	int OPL_QueryStop(HANDLE hQueryLog);

	//!\brief 获得CLogManager的实例
	//!\return NULL表示获取失败.其他表示CLogManager实例的地址
	//##ModelId=49BA0A6600BB
	static CLogManager *OPL_GetInstance();

	//!\brief 销毁CLogManager的实例的资源
	//!\param pInstance 指向的CLogManager的实例
	//!\return 无
	//##ModelId=49BA0AEC002E
	static void OPL_DestroyInstance(CLogManager * pInstance);

  private:
	//##ModelId=499E6049005C
	  CLogManager();

	//##ModelId=499E604B039E
	 ~CLogManager();

	static void *QueryLog(void *arg);

	static CLogManager *m_pInstance;

	dvr_list_head m_listQuery;
};

#endif							/* _INC_CLOGMANAGER_499E60290204_INCLUDED */
