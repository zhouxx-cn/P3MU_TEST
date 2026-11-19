
/*!
*\file   CLogDaoEx.h
*\brief  文本式日志数据库访问控制类定义
*\author yuk@higheasyrd.com
*\date	  2009/04/28
*\version 0.1
* 功能:支持数据库中日志的添加、查询
* 说明:本类为单实例类,主要供CLogManager使用
*/

#ifndef __CLOGDAOEX_H_
#define __CLOGDAOEX_H_

#include <time.h>
#include <string.h>
#include "PUB/sqlite3.h"
#include "tagLog.h"

//!\brief 日志数据库访问控制类
class CLogDaoEx
{
  public:
	//!\brief 获取类的唯一实例
	//!\return 成功则返回实例的指针,否则返回NULL
	static CLogDaoEx *GetInstance();

	//!\brief 销毁类的实例
	//!\param pInstance 指向实例的指针
	//!\return 无
	static void DestroyInstance(CLogDaoEx * pInstance);

	//!\brief 初始化日志数据库 
	//!\param szOplDbFileName 指定数据库的文件名，
	//! 如果没有该文件或数据库文件中没有日志表
	//! 系统会自动创建文件添加日志表
	//!\return 0表示绑定成功 其他表示错误码
	//!\note 如果数据库文件无法创建或无法打开则返回错误
	int BindDb(char *szOplDbFileName);

	//!\brief 添加新的用户
	//!\param tagLog 要添加的用户信息
	//!\return 0表示添加成功 其他表示错误码
	int InsertLog(const LOG & tagLog);

	//!\brief 按类型查询日志
	//!\param pLogBuf 存储获得的日志的缓冲区
	//!\param enType 日志类型
	//!\param nPageId 页码
	//!\param nPageItem 页大小
	//!\return -1表示查询失败 其他表示查询到的日志的条数
	int GetLogs(PLOG pLogBuf, int nLogBufLen, EDVR_EVENT_TYPE enType, int nPageId, int nPageItem);

	//!\brief 按时间和类型查询日志
	//!\param  pLogBuf 存储获得的日志的缓冲区
	//!\param enType 日志类型
	//!\param begintime 查询起始时间
	//!\param endtime 查询截止时间
	//!\param nPageId 页码
	//!\param nPageItem 页大小
	//!\return -1表示查询失败 其他表示查询到的日志的条数
	int GetLogs(PLOG pLogBuf, int nLogBufLen, EDVR_EVENT_TYPE enType, time_t begintime, time_t endtime, int nPageId, int nPageItem);


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
	  CLogDaoEx();

	//!\brief 析构函数
	 ~CLogDaoEx();

  private:
	//!\brief 日志数据库访问类的惟一实例
	static CLogDaoEx *m_pInstance;

	int m_nLogCount;

	void *m_pDb;
};

#endif
