
/*!
*\file   CUserDao.cpp
*\brief  用户数据库访问控制类函数定义
*\author yuk@higheasyrd.com
*\date	  2009/04/10
*\version 0.1
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "idvr_errno.h"
#include "CLogDao.h"
#include "OPLTrace.h"
#include "QueryLog.h"
#include "mysystem.h"

#define TRACE printf
#define CHECK_SQL_FILE

#ifndef DEBUG_NOT_USE_OPL_LIB
sqlite3_stmt *CLogDao::m_pInsertLogStmt = NULL;

sqlite3_stmt *CLogDao::m_pGetLogByTypeStmt = NULL;

sqlite3_stmt *CLogDao::m_pGetLogByTypeAndTimeStmt = NULL;

sqlite3_stmt *CLogDao::m_pGetLogByTimeStmt = NULL;

sqlite3_stmt *CLogDao::m_pCreateTmpTableStmt = NULL;

sqlite3_stmt *CLogDao::m_pCreateTmpTableByTypeStmt = NULL;

sqlite3_stmt *CLogDao::m_pCreateTmpTableByTimeStmt = NULL;

sqlite3_stmt *CLogDao::m_pCreateTmpTableByTypeAndTimeStmt = NULL;

sqlite3_stmt *CLogDao::m_pGetQueryCountStmt = NULL;

sqlite3_stmt *CLogDao::m_pGetLogFromTmpStmt = NULL;

//sqlite3_stmt *CLogDao::m_pDeleteLogByIdStmt = NULL;

//sqlite3_stmt *CLogDao::m_pDeleteLogByTimeStmt = NULL;

sqlite3_stmt *CLogDao::m_pDeleteLogStmt = NULL;
#endif

//初始化各操作的SQL语句
const char *CLogDao::SQL_INSERT_LOG = "INSERT INTO [LOG]\
	(\
		[TYPE],\
		[DETAIL],\
		[CHANNEL],\
		[RECORDCHANNEL],\
		[REMOTE],\
		[USERNAME],\
		[LOGTIME]\
	)\
	VALUES (?, ?, ?, ?, ?, ?, ?)";

const char *CLogDao::SQL_GET_LOG_BY_TYPE = "SELECT * FROM [LOG] WHERE [TYPE] = ?  LIMIT ?, ?";

const char *CLogDao::SQL_GET_LOG_BY_TYPE_AND_TIME = "SELECT * FROM [LOG] WHERE [TYPE] = ? AND [LOGTIME] >= ? AND [LOGTIME] <= ? LIMIT ?, ?";

const char *CLogDao::SQL_GET_LOG_BY_TIME = "SELECT * FROM [LOG] WHERE [LOGTIME] >= ? AND [LOGTIME] <= ? LIMIT ?, ?";

const char *CLogDao::SQL_CREATE_TMP_TABLE = "CREATE TEMP TABLE [%s] AS SELECT * FROM [LOG] ORDER BY [LOGID] DESC";

const char *CLogDao::SQL_CREATE_TMP_TABLE_BY_TYPE = "CREATE TEMP TABLE [%s] AS SELECT * FROM [LOG] WHERE [TYPE] = ?  ORDER BY [LOGID] DESC";

const char *CLogDao::SQL_CREATE_TMP_TABLE_BY_TIME =
	"CREATE TEMP TABLE [%s] AS SELECT * FROM [LOG] WHERE [LOGTIME] >= ? AND [LOGTIME] <= ?  ORDER BY [LOGID] DESC";

const char *CLogDao::SQL_CREATE_TMP_TABLE_BY_TYPE_AND_TIME =
	"CREATE TEMP TABLE [%s] AS SELECT * FROM [LOG] WHERE [TYPE] = ? AND [LOGTIME] >= ? AND [LOGTIME] <= ?  ORDER BY [LOGID] DESC";

const char *CLogDao::SQL_DROP_TMP_TABLE = "DROP TABLE [%s]";

//const char *CLogDao::SQL_DELETE_LOG_BY_ID =
//  "DELETE FROM [LOG] WHERE [LOGID] = ?";

//const char *CLogDao::SQL_DELETE_LOG_BY_TIME =
//  "DELETE FROM [LOG] WHERE [LOGTIME] < ?";

const char *CLogDao::SQL_DELETE_LOG = "DELETE FROM [LOG] WHERE [LOGID] IN (SELECT [LOGID] FROM [LOG] LIMIT 0, 300)";

CLogDao *CLogDao::m_pInstance = NULL;

CLogDao::CLogDao():m_nLogCount(0), m_pDb(NULL)
{
	pthread_mutex_init(&m_mtxQuery, NULL);
}

CLogDao::~CLogDao()
{
#ifndef DEBUG_NOT_USE_OPL_LIB
	if (NULL != m_pInsertLogStmt);
	{
		sqlite3_finalize(m_pInsertLogStmt);
	}

	if (NULL != m_pDeleteLogStmt);
	{
		sqlite3_finalize(m_pDeleteLogStmt);
	}
#endif
	pthread_mutex_destroy(&m_mtxQuery);
}

CLogDao *CLogDao::GetInstance()
{
	//创建唯一实例
	if (m_pInstance == 0)
	{
		m_pInstance = new CLogDao();
	}

	//如已经创建则直接返回
	return m_pInstance;
}

void CLogDao::DestroyInstance(CLogDao * pInstance)
{
	if (pInstance != NULL)
	{
		delete pInstance;
	}
	else
	{
		OPL_ERROR("para error, pInstance=%p\n", pInstance);
	}
}

int CLogDao::BindDb(const char *szOplDbFileName)
{
#ifndef DEBUG_NOT_USE_OPL_LIB
	//数据库文件名不能为空
	if (NULL == szOplDbFileName)
	{
		TRACE("[%s]ERR:para err, szOplDbFileName=%s", __FUNCTION__, szOplDbFileName);
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	//判断是否已经绑定到一个用户数据库
	if (m_pDb != NULL)
	{
		TRACE("[%s]ERR:para err, already bind a database", __FUNCTION__);
		return -1;
	}

	//打开数据库,如果没有该文件则创建该文件
	int ret = sqlite3_open(szOplDbFileName, &m_pDb);

	if (ret != SQLITE_OK)
	{
		TRACE("[%s]ERR:open or create db %s failed, ret=%d", __FUNCTION__, szOplDbFileName, ret);
		return IDVR_USR_ERROR_DB_OPEN_FAILED;
	}

#ifdef CHECK_SQL_FILE
	int integrityVerified = 0;
	sqlite3_stmt *integrity = NULL;
	
	printf("Start check %s\n", szOplDbFileName);
	if ( sqlite3_prepare_v2( m_pDb, "PRAGMA integrity_check;", -1, &integrity, NULL ) == SQLITE_OK ) 
	{
		while ( sqlite3_step( integrity ) == SQLITE_ROW ) 
		{
			const unsigned char *result = sqlite3_column_text( integrity, 0 );
			if ( result && strcmp( ( const char * )result, (const char *)"ok" ) == 0 ) 
			{
				integrityVerified = 1;
				break;
			}
		}

		sqlite3_finalize( integrity );
	}

	if(0 == integrityVerified)//数据库损坏则删除原始的数据库
	{
		char command[128] = {0};
		
		sqlite3_close( m_pDb );
		
		snprintf(command,sizeof(command), "rm -rf %s", szOplDbFileName);
		mysystem(command);

		//打开数据库,如果没有该文件则创建该文件
		int ret = sqlite3_open(szOplDbFileName, &m_pDb);
		if (ret != SQLITE_OK)
		{
			TRACE("[%s]ERR:open or create db %s failed, ret=%d", __FUNCTION__, szOplDbFileName, ret);
			return IDVR_USR_ERROR_DB_OPEN_FAILED;
		}

	}
#endif

	//创建日志表的SQL语句
	char *szSqlCreateTable = "CREATE TABLE IF NOT EXISTS [LOG] (\
									[LOGID] INTEGER PRIMARY KEY AUTOINCREMENT,\
									[TYPE] INTEGER,\
									[DETAIL] INTEGER,\
									[CHANNEL] INTEGER,\
									[RECORDCHANNEL] INTEGER,\
									[REMOTE] INTEGER,\
									[USERNAME] TEXT(48),\
									[LOGTIME] INTEGER\
									)";

	//创建用户表
	sqlite3_stmt *pCreateTableStmt;

	//预处理SQL语句
	ret = sqlite3_prepare_v2(m_pDb, szSqlCreateTable, strlen(szSqlCreateTable), &pCreateTableStmt, NULL);
	OPL_DBG("ret = %d\n", ret);
	if (ret != 0)
	{
		return -1;
	}

	//执行SQL语句
	ret = sqlite3_step(pCreateTableStmt);

	//创建失败
	if (SQLITE_DONE != ret)
	{
		OPL_DBG("Create Table OPL failed\n");
		return -1;
	}

	sqlite3_finalize(pCreateTableStmt);

	OPL_INFO("Create or Open Table OPL Success");

	sqlite3_stmt *pStmt = NULL;

	char *szSqlGetLogCount = "SELECT COUNT(*) FROM [LOG]";

	ret = sqlite3_prepare_v2(m_pDb, szSqlGetLogCount, strlen(szSqlGetLogCount), &pStmt, NULL);
	OPL_DBG("prepare GetLogCount, ret = %d", ret);
	ret = sqlite3_step(pStmt);
	OPL_DBG("exec GetLogCount, ret = %d", ret);
	m_nLogCount = sqlite3_column_int(pStmt, 0);
	sqlite3_finalize(pStmt);
	OPL_INFO("m_nLogCount=%d\n", m_nLogCount);

	//尝试用触发器在日志条数超过3000条时删除300条
	//测试发现触发器大大降低了插入速度,一般插入需20ms左右
	//加触发器后增加到100ms甚至更高(日志条数越多越耗时)
#if 0
	char *szSqlInsertTrigger = "CREATE TRIGGER [DELETE_LOGS] INSERT ON [LOG] \
								BEGIN \
									DELETE FROM [LOG] WHERE (EXISTS (SELECT [LOGID] FROM [LOG] ORDER BY [LOGID] ASC LIMIT 4999, 1))\
														AND ([LOGID] IN (SELECT [LOGID] FROM [LOG] ORDER BY [LOGID] ASC LIMIT 0, 1000));\
								END";

	sqlite3_stmt *pInsertTrigger = NULL;

	ret = sqlite3_prepare_v2(m_pDb, szSqlInsertTrigger, strlen(szSqlInsertTrigger), &pInsertTrigger, NULL);

	//ret = sqlite3_step(pInsertTrigger);

	//创建失败
	//if (SQLITE_DONE != ret)
	//{
	//  OPL_DBG("Create Insert Trigger failed, ret=%d\n", ret);
	//  return -1;
	//}
#endif

	//预处理日志插入SQL语句
	ret = sqlite3_prepare_v2(m_pDb, SQL_INSERT_LOG, strlen(SQL_INSERT_LOG), &m_pInsertLogStmt, NULL);
	OPL_DBG("ret=%d\n", ret);

	//预处理按时间删除日志SQL语句
	ret = sqlite3_prepare_v2(m_pDb, SQL_DELETE_LOG, strlen(SQL_DELETE_LOG), &m_pDeleteLogStmt, NULL);

	//预处理按ID删除日志SQL语句
	//sqlite3_prepare_v2(m_pDb
	//              , SQL_DELETE_LOG_BY_ID
	//              , strlen(SQL_DELETE_LOG_BY_ID)
	//              , &m_pDeleteLogByIdStmt
	//              , NULL);

	//预处理按时间删除日志SQL语句
	//sqlite3_prepare_v2(m_pDb
	//              , SQL_DELETE_LOG_BY_TIME
	//              , strlen(SQL_DELETE_LOG_BY_TIME)
	//              , &m_pDeleteLogByTimeStmt
	//              , NULL);
#endif
	return (int) 0;
}

int CLogDao::InsertLog(const LOG & tagLog)
{
#ifndef DEBUG_NOT_USE_OPL_LIB
	int ret = 0;

	int nResult = 0;

	Lock();
	if (m_nLogCount >= 500)  // 日志文件每到了500条就减少300条
	{
		BEGIN_TIME(1) ret = sqlite3_step(m_pDeleteLogStmt);
		OPL_INFO("delete top 300 logs, ret = %d", ret);

		if (SQLITE_DONE == ret)
		{
			m_nLogCount -= 300;
		}
		END_TIME ret = sqlite3_reset(m_pDeleteLogStmt);
	}
	BEGIN_TIME(1) sqlite3_reset(m_pInsertLogStmt);
	sqlite3_clear_bindings(m_pInsertLogStmt);

	//将对应的值绑定到SQL语句的参数
	//参数顺序编号从1开始
	//绑定日志类型
	ret = sqlite3_bind_int(m_pInsertLogStmt, 1, tagLog.dwEventType);
	//OPL_DBG("ret = %d\n", ret);

	//绑定日志详细信息
	ret = sqlite3_bind_int(m_pInsertLogStmt, 2, tagLog.dwEventDetail);
	//OPL_DBG("ret = %d\n", ret);

	//绑定产生日志的通道
	ret = sqlite3_bind_int(m_pInsertLogStmt, 3, tagLog.dwChannel);
	//OPL_DBG("ret = %d\n", ret);

	//绑定日志事件联动的通道
	ret = sqlite3_bind_int64(m_pInsertLogStmt, 4, tagLog.u64RecordChannelFlag);
	//OPL_DBG("ret = %d\n", ret);

	//绑定日志是否是远程事件
	ret = sqlite3_bind_int(m_pInsertLogStmt, 5, tagLog.dwRemote);
	//OPL_DBG("ret = %d\n", ret);

	//绑定日志对应的用户名
	ret = sqlite3_bind_text(m_pInsertLogStmt, 6, tagLog.strUsername, -1, SQLITE_STATIC);
	//OPL_DBG("ret = %d\n", ret);

	time_t tNow;

	time(&tNow);
	ret = sqlite3_bind_int(m_pInsertLogStmt, 7, tNow);
	//OPL_DBG("ret = %d\n", ret);

	//执行插入语句
	ret = sqlite3_step(m_pInsertLogStmt);
	//OPL_DBG("ret = %d\n", ret);

	if (SQLITE_DONE == ret)
	{
		nResult = sqlite3_changes(m_pDb);
		//OPL_DBG("Insert Log, Affected Rows = %d\n", nResult);
	}
	else
	{
		nResult = 0;
		OPL_ERROR("Insert Log Failed, ret = %d\n", ret);
	}

	//提交到数据库
	sqlite3_reset(m_pInsertLogStmt);
	m_nLogCount++;

	END_TIME UnLock();

	ret = (1 == nResult) ? 0 : -1;
	return (int) nResult;
#else
    return 0;
#endif
}

int CLogDao::GetLogs(HANDLE hQuery, PLOG pLogBuf, int nLogBufLen, int nPageId)
{
#ifndef DEBUG_NOT_USE_OPL_LIB
	int ret = 0;

	QueryParam *pQuery = (QueryParam *) hQuery;

	sqlite3_stmt *pStmt = (sqlite3_stmt *) (pQuery->pData);

	if (nPageId * (pQuery->nPageItem) >= pQuery->nCount)//页码*页大小>=页总数
	{
		return 0;
	}

	if (NULL == pStmt)
	{
		return -1;
	}

	Lock();
	//绑定页码和页大小用于查询指定页日志
	ret = sqlite3_bind_int(pStmt, 1, nPageId * (pQuery->nPageItem));
	ret |= sqlite3_bind_int(pStmt, 2, pQuery->nPageItem);
	OPL_DBG("ret = %d\n", ret);

	//计数器,统计查询到的条数
	int nLogCount = 0;

	//循环执行查询语句
	while (1)
	{
		if (nLogCount >= nLogBufLen)
		{
			break;
		}

		BEGIN_TIME(1) ret = sqlite3_step(pStmt);
		END_TIME OPL_DBG("ret = %d", ret);

		if (SQLITE_ROW == ret)//SQLITE_ROW 返回一个单行结果集
		{
			OPL_DBG("Get Log Row\n");

			//填充到结果集
			GetRow(&(pLogBuf[nLogCount]), pStmt);
			nLogCount++;
		}
		else if (SQLITE_DONE == ret)//SQLITE_DONE 执行成功，正常返回
		{
			OPL_DBG("End Select\n");
			break;
		}
		else
		{
			OPL_ERROR("Select Stmt Failed\n");
			nLogCount = -1;
			break;
		}
	}
	sqlite3_reset(pStmt);
	sqlite3_clear_bindings(pStmt);
	UnLock();
	return (int) nLogCount;
#else
    return 0;
#endif
}

#if 0
int CLogDao::DeleteLog(int nLogId)
{
	int ret = -1;

	int nResult = 0;

	sqlite3_reset(m_pDeleteLogByIdStmt);
	sqlite3_clear_bindings(m_pDeleteLogByIdStmt);

	//绑定ID参数
	ret = sqlite3_bind_int(m_pDeleteLogByIdStmt, 1, (int) nLogId);
	OPL_DBG("ret = %d\n", ret);

	//执行删除
	ret = sqlite3_step(m_pDeleteLogByIdStmt);
	OPL_DBG("ret = %d\n", ret);

	if (SQLITE_DONE == ret)
	{
		nResult = sqlite3_changes(m_pDb);
		OPL_DBG("Delete Log, nLogCount = %d\n", ret);
	}
	else
	{
		nResult = -1;
		OPL_DBG("Delete Log Failed\n, Affected Rows = %d\n", nResult);
	}

	sqlite3_reset(m_pDeleteLogByIdStmt);
	return (int) nResult;
}

int CLogDao::DeleteLog(time_t endtime)
{
	int ret = -1;

	int nResult = 0;

	sqlite3_reset(m_pDeleteLogByTimeStmt);
	sqlite3_clear_bindings(m_pDeleteLogByTimeStmt);

	//绑定时间参数
	ret = sqlite3_bind_int(m_pDeleteLogByTimeStmt, 1, (int) endtime);
	OPL_DBG("ret = %d\n", ret);

	//执行删除命令
	ret = sqlite3_step(m_pDeleteLogByTimeStmt);
	OPL_DBG("ret = %d\n", ret);

	if (SQLITE_DONE == ret)
	{
		nResult = sqlite3_changes(m_pDb);
		OPL_DBG("Delete Log, Affected Rows = %d\n", nResult);
	}
	else
	{
		nResult = -1;
		OPL_DBG("Delete Log Failed\n, Affected Rows = %d\n", nResult);
	}
	return (int) nResult;
}
#endif

void CLogDao::GetRow(PLOG pLog, sqlite3_stmt * pStmt)
{
#ifndef DEBUG_NOT_USE_OPL_LIB
	pLog->dwEventType = sqlite3_column_int(pStmt, 1);
	pLog->dwEventDetail = sqlite3_column_int(pStmt, 2);
	pLog->dwChannel = sqlite3_column_int(pStmt, 3);
	pLog->u64RecordChannelFlag = sqlite3_column_int64(pStmt, 4);
	pLog->dwRemote = sqlite3_column_int(pStmt, 5);
	strncpy(pLog->strUsername, (const char *) sqlite3_column_text(pStmt, 6),sizeof(pLog->strUsername) - 1);
	pLog->dwTime = sqlite3_column_int(pStmt, 7);
#endif
}

int CLogDao::CreateQuery(HANDLE hQuery)
{
#ifndef DEBUG_NOT_USE_OPL_LIB
	sqlite3_stmt *pStmt = NULL;

	int nParamIndex = 1;

	int ret = 0;

	char szTableName[256];

	char szSqlCreateTmpTable[512];

	QueryParam *pQuery = (QueryParam *) hQuery;

	Lock();
	BEGIN_TIME(1) snprintf(szTableName, sizeof(szTableName), "LOGTMP%u", (unsigned int) hQuery);
	pQuery->nCount = 0;

	if (QUERY_BY_TYPE == pQuery->nQueryMode)
	{
		if (EDVR_EVENT_ALL == pQuery->nLogType)
		{
			snprintf(szSqlCreateTmpTable,sizeof(szSqlCreateTmpTable), SQL_CREATE_TMP_TABLE, szTableName);
			ret = sqlite3_prepare_v2(m_pDb, szSqlCreateTmpTable, strlen(szSqlCreateTmpTable), &pStmt, NULL);
		}
		else
		{
			snprintf(szSqlCreateTmpTable,sizeof(szSqlCreateTmpTable), SQL_CREATE_TMP_TABLE_BY_TYPE, szTableName);
			ret = sqlite3_prepare_v2(m_pDb, szSqlCreateTmpTable, -1, &pStmt, NULL);

			//绑定类型
			ret |= sqlite3_bind_int(pStmt, nParamIndex, pQuery->nLogType);
			nParamIndex++;
			OPL_DBG("ret = %d\n", ret);
		}
	}
	else if (QUERY_BY_TYPE_AND_TIME == pQuery->nQueryMode)
	{
		if (EDVR_EVENT_ALL == pQuery->nLogType)
		{
			snprintf(szSqlCreateTmpTable,sizeof(szSqlCreateTmpTable), SQL_CREATE_TMP_TABLE_BY_TIME, szTableName);
			ret = sqlite3_prepare_v2(m_pDb, szSqlCreateTmpTable, strlen(szSqlCreateTmpTable), &pStmt, NULL);
		}
		else
		{
			snprintf(szSqlCreateTmpTable,sizeof(szSqlCreateTmpTable), SQL_CREATE_TMP_TABLE_BY_TYPE_AND_TIME, szTableName);
			OPL_DBG(szSqlCreateTmpTable);
			ret = sqlite3_prepare_v2(m_pDb, szSqlCreateTmpTable, strlen(szSqlCreateTmpTable), &pStmt, NULL);
			OPL_DBG("ret = %d\n", ret);

			//绑定类型
			ret |= sqlite3_bind_int(pStmt, nParamIndex, pQuery->nLogType);
			OPL_DBG("nParamIndex = %d, ret = %d\n", nParamIndex, ret);
			nParamIndex++;
		}

		//绑定查询开始时间
		ret |= sqlite3_bind_int(pStmt, nParamIndex, (int) pQuery->tStartTime);
		OPL_DBG("nParamIndex = %d, ret = %d, pQuery->tStartTime=%lu\n", nParamIndex, ret, pQuery->tStartTime);
		nParamIndex++;

		//绑定查询结束时间
		ret |= sqlite3_bind_int(pStmt, nParamIndex, (int) pQuery->tStopTime);
		OPL_DBG("nParamIndex = %d, ret = %d, pQuery->tStopTime=%lu\n", nParamIndex, ret, pQuery->tStopTime);
	}
	else
	{
		UnLock();
		OPL_ERROR("QueryMode is Invalid\n");
		return -1;
	}

	if (ret != SQLITE_OK)
	{
		UnLock();
		OPL_ERROR("Bind Parameter failed\n");
		return -1;
	}

	//创建临时表
	ret = sqlite3_step(pStmt);
	//ret = sqlite3_exec(m_pDb, "CREATE TEMP TABLE [LOGTMP%u] AS SELECT * FROM [LOG] WHERE [TYPE]=0 AND [LOGTIME] > 1241349523 AND [LOGTIME] < 1241435923", NULL, NULL, NULL);
	END_TIME if (ret != SQLITE_DONE)
	{
		UnLock();
		OPL_ERROR("Create Tmp Table, ret = %d", ret);
		return -1;
	}

	ret = sqlite3_finalize(pStmt);
	OPL_DBG("finalize Create temp table stmt, ret = %d", ret);

#if 1
	char szSqlGetQueryCount[256] = "";

	snprintf(szSqlGetQueryCount, sizeof(szSqlGetQueryCount), "SELECT COUNT(*) FROM [%s]", szTableName);
	OPL_DBG(szSqlGetQueryCount);
	ret = sqlite3_prepare_v2(m_pDb, szSqlGetQueryCount, strlen(szSqlGetQueryCount), &m_pGetQueryCountStmt, NULL);
	if ((SQLITE_OK == ret) && (SQLITE_ROW == sqlite3_step(m_pGetQueryCountStmt)))
	{
		pQuery->nCount = sqlite3_column_int(m_pGetQueryCountStmt, 0);
		OPL_DBG("pQuery->nCount = %d\n", pQuery->nCount);

		if (pQuery->nCount > 0)
		{
			char szSqlGetLogFromTmp[256] = "";

			snprintf(szSqlGetLogFromTmp, sizeof(szSqlGetLogFromTmp), "SELECT * FROM [%s] LIMIT ?, ?", szTableName);
			ret = sqlite3_prepare_v2(m_pDb, szSqlGetLogFromTmp, strlen(szSqlGetLogFromTmp), (sqlite3_stmt **) & (pQuery->pData), NULL);
			OPL_DBG("Prepare GetLogFromTmp, ret = %d\n", ret);
		}
	}
	else
	{
		UnLock();
		OPL_ERROR("GetQueryCount, ret = %d\n", ret);
		return -1;
	}

	sqlite3_finalize(m_pGetQueryCountStmt);
	OPL_DBG("reset GetQueryCount stmt, ret = %d\n", ret);
#endif
	UnLock();
	return pQuery->nCount;
#else
    return 0;
#endif  
}

int CLogDao::DestroyQuery(HANDLE hQuery)
{
#ifndef DEBUG_NOT_USE_OPL_LIB
	QueryParam *pQuery = (QueryParam *) hQuery;

	sqlite3_stmt *pStmt = NULL;

	int ret = 0;

	Lock();
	if (pQuery->nCount != 0)
	{
		ret = sqlite3_finalize((sqlite3_stmt *) (pQuery->pData));
		OPL_DBG("ret = %d\n", ret);
	}

	char szSqlDropTmp[512];

	char szTableName[256];

	snprintf(szTableName, sizeof(szTableName), "LOGTMP%u", (unsigned int) hQuery);
	snprintf(szSqlDropTmp, sizeof(szSqlDropTmp), SQL_DROP_TMP_TABLE, szTableName);
	OPL_DBG(szSqlDropTmp);
	ret = sqlite3_prepare_v2(m_pDb, szSqlDropTmp, strlen(szSqlDropTmp), &pStmt, NULL);
	OPL_DBG("ret = %d\n", ret);

	ret = sqlite3_step(pStmt);
	OPL_DBG("Drop tmp table %s, ret = %d\n", szTableName, ret);

	ret = sqlite3_finalize(pStmt);
	OPL_DBG("ret = %d\n", ret);
	UnLock();
#endif
	return 0;
}

inline int CLogDao::Lock()
{
	return pthread_mutex_lock(&m_mtxQuery);
}

inline int CLogDao::UnLock()
{
	return pthread_mutex_unlock(&m_mtxQuery);
}
