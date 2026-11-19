
/*!
*\file   CLogDao.h
*\brief  日志数据库访问控制类定义
*\author yuk@higheasyrd.com
*\date	  2009/04/10
*\version 0.1
* 功能:支持数据库中日志的添加、查询
* 说明:本类为单实例类,主要供CLogManager使用
*/

#ifndef __CLOGDAO_H_
#define __CLOGDAO_H_

#include <time.h>
#include <string.h>
#include <pthread.h>
#include "PUB/sqlite3.h"
#include "tagLog.h"
#include "PUB/HANDLE.h"

//!\brief 日志数据库访问控制类
class CLogDao
{
  public:
	//!\brief 获取类的唯一实例
	//!\return 成功则返回实例的指针,否则返回NULL
	static CLogDao *GetInstance();

	//!\brief 销毁类的实例
	//!\param pInstance 指向实例的指针
	//!\return 无
	static void DestroyInstance(CLogDao * pInstance);

	//!\brief 初始化日志数据库 
	//!\param szOplDbFileName 指定数据库的文件名，
	//! 如果没有该文件或数据库文件中没有日志表
	//! 系统会自动创建文件添加日志表
	//!\return 0表示绑定成功 其他表示错误码
	//!\note 如果数据库文件无法创建或无法打开则返回错误
	int BindDb(const char *szOplDbFileName);

	//!\brief 添加新的用户
	//!\param tagLog 要添加的用户信息
	//!\return 0表示添加成功 其他表示错误码
	int InsertLog(const LOG & tagLog);

	//!\brief 创建新的查询
	int CreateQuery(HANDLE hQuery);

	int DestroyQuery(HANDLE hQuery);

	//!\brief 按时间和类型查询日志
	//!\param  pLogBuf 存储获得的日志的缓冲区
	//!\param enType 日志类型
	//!\param begintime 查询起始时间
	//!\param endtime 查询截止时间
	//!\param nPageId 页码
	//!\param nPageItem 页大小
	//!\return -1表示查询失败 其他表示查询到的日志的条数
	int GetLogs(HANDLE hQuery, PLOG pLogBuf, int nLogBufLen, int nPageId);


	//!\brief 删除指定日志
	//!\param nLogId 要删除的日志的ID
	//!\return 0表示删除成功 其他表示错误码
	//int DeleteLog(int nLogId);

	//!\brief 删除指定时间以前日志
	//!\param endtime 指定的时间
	//!\return 表示删除的日志的条数 -1其他表示错误
	//int DeleteLog(time_t endtime);

  private:
	//!\brief 构造函数
	  CLogDao();

	//!\brief 析构函数
	 ~CLogDao();

	//!\brief 获取一行数据
	void GetRow(PLOG pLog, sqlite3_stmt * pStmt);

	int Lock();

	int UnLock();
  private:
	//!\brief 日志数据库访问类的惟一实例
	static CLogDao *m_pInstance;

	int m_nLogCount;

	sqlite3 *m_pDb;

	pthread_mutex_t m_mtxQuery;

	//!\brief 添加日志
	static sqlite3_stmt *m_pInsertLogStmt;

	//!\brief 按类型查询日志
	static sqlite3_stmt *m_pGetLogByTypeStmt;

	//!\brief 按时间和类型查询日志
	static sqlite3_stmt *m_pGetLogByTypeAndTimeStmt;

	//!\brief 按时间查询日志
	static sqlite3_stmt *m_pGetLogByTimeStmt;

	//!\brief 根据id删除日志
	static sqlite3_stmt *m_pDeleteLogByIdStmt;

	//!\brief 根据时间删除日志
	static sqlite3_stmt *m_pDeleteLogByTimeStmt;

	//!\brief 自动淘汰日志
	static sqlite3_stmt *m_pDeleteLogStmt;

	static sqlite3_stmt *m_pCreateTmpTableStmt;

	static sqlite3_stmt *m_pCreateTmpTableByTypeStmt;

	static sqlite3_stmt *m_pCreateTmpTableByTimeStmt;

	static sqlite3_stmt *m_pCreateTmpTableByTypeAndTimeStmt;

	static sqlite3_stmt *m_pGetQueryCountStmt;

	static sqlite3_stmt *m_pGetLogFromTmpStmt;

	//!\brief 插入日志SQL语句
	const static char *SQL_INSERT_LOG;

	//!\brief 按类型查询日志SQL语句
	const static char *SQL_GET_LOG_BY_TYPE;

	//!\brief 按时间查询日志SQL语句
	const static char *SQL_GET_LOG_BY_TIME;

	//!\brief 按类型和时间删除日志SQL语句
	const static char *SQL_GET_LOG_BY_TYPE_AND_TIME;

	//!\brief 创建查询临时表
	const static char *SQL_CREATE_TMP_TABLE;

	//!\brief 创建查询临时表
	const static char *SQL_CREATE_TMP_TABLE_BY_TYPE;

	//!\brief 创建查询临时表
	const static char *SQL_CREATE_TMP_TABLE_BY_TIME;

	//!\brief 创建查询临时表
	const static char *SQL_CREATE_TMP_TABLE_BY_TYPE_AND_TIME;

	const static char *SQL_DROP_TMP_TABLE;

	//!\brief 按ID删除日志SQL语句
	//const static char *SQL_DELETE_LOG_BY_ID;

	//!\brief 按时间删除日志SQL语句
	//const static char *SQL_DELETE_LOG_BY_TIME;

	//!\brief 淘汰最先的1000条日志的语句
	const static char *SQL_DELETE_LOG;

};

#endif
