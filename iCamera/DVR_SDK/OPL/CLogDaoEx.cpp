
/*!
*\file   CUserDaoEx.cpp
*\brief  文本日志数据库访问控制类函数定义
*\author yuk@higheasyrd.com
*\date	  2009/04/30
*\version 0.1
*/

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include "idvr_errno.h"
#include "CLogDaoEx.h"
#include "OPLTrace.h"

#define TRACE printf

CLogDaoEx *CLogDaoEx::m_pInstance = NULL;

CLogDaoEx::CLogDaoEx():m_nLogCount(0), m_pDb(NULL)
{
}

CLogDaoEx::~CLogDaoEx()
{
}

CLogDaoEx *CLogDaoEx::GetInstance()
{
	//创建唯一实例
	if (m_pInstance == 0)
	{
		m_pInstance = new CLogDaoEx();
	}

	//如已经创建则直接返回
	return m_pInstance;
}

void CLogDaoEx::DestroyInstance(CLogDaoEx * pInstance)
{
	if (pInstance != NULL)
	{
		delete pInstance;
	}
	else
	{
		OPL_DBG("para error, pInstance=%p\n", pInstance);
	}
}

int CLogDaoEx::BindDb(char *szOplDbFileName)
{
	//数据库文件名不能为空
	if (NULL == szOplDbFileName)
	{
		TRACE("[%s]ERR:para err", __FUNCTION__);
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	//判断是否已经绑定到一个用户数据库
	if (m_pDb != NULL)
	{
		TRACE("[%s]ERR:para err, already bind a database", __FUNCTION__);
		return -1;
	}

	//打开数据库,如果没有该文件则创建该文件
	int ret = open(szOplDbFileName, O_CREAT | O_RDWR);

	if (ret != 0)
	{
		TRACE("[%s]ERR:open or create db %s failed, ret=%d", __FUNCTION__, szOplDbFileName, ret);
		return IDVR_USR_ERROR_DB_OPEN_FAILED;
	}

	close(ret);
	OPL_DBG("ret=%d\n", ret);
	return (int) 0;
}

int CLogDaoEx::InsertLog(const LOG & tagLog)
{
	int nResult = 0;

	return (int) nResult;
}

int CLogDaoEx::GetLogs(PLOG pLogBuf, int nLogBufLen, EDVR_EVENT_TYPE enType, int nPageId, int nPageItem)
{
	int ret = -1;

	return (int) ret;
}

int CLogDaoEx::GetLogs(PLOG pLogBuf, int nLogBufLen, EDVR_EVENT_TYPE enType, time_t begintime, time_t endtime, int nPageId, int nPageItem)
{
	int ret = -1;

	return (int) ret;
}
