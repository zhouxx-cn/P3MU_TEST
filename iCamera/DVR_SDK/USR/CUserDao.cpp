/*!
*\file   CUserDao.cpp
*\brief  用户数据库访问控制类函数定义
*\author yuk@higheasyrd.com
*\date	  2009/04/10
*\version 0.1
*/

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "idvr_errno.h"
#include "CUserDao.h"
#include "USRTrace.h"
#include "mysystem.h"
// #include "sys_banned.h"
#include "global_gather.h"


//初始化实例为NULL
CUserDao *CUserDao::m_pInstance= NULL;

#ifndef NO_USE_SQLITE3

//数据库handle初始化
sqlite3 *CUserDao::m_pDb = NULL;

sqlite3_stmt *CUserDao::m_pInsertUserStmt = NULL;

sqlite3_stmt *CUserDao::m_pSaveUserStmt = NULL;

sqlite3_stmt *CUserDao::m_pGetUserStmt = NULL;

sqlite3_stmt *CUserDao::m_pHaveUserStmt= NULL;

sqlite3_stmt *CUserDao::m_pDeleteUserStmt= NULL;
#endif

const char *CUserDao::SQL_CREATE_TABLE = "CREATE TABLE IF NOT EXISTS [USER] (\
									[USERID] INTEGER PRIMARY KEY AUTOINCREMENT,\
									[USERNAME] TEXT(48),\
									[PASSWORD] TEXT(48),\
									[BINDMAC] TEXT(48),\
									[EMAIL] TEXT(256),\
									[ENABLE] INTEGER,\
									[PREVIEW0] INTEGER,\
									[PREVIEW1] INTEGER,\
									[PLAYBACK0] INTEGER,\
									[PLAYBACK1] INTEGER,\
									[PTZCTRL0] INTEGER,\
									[PTZCTRL1] INTEGER,\
									[MANUALRECORD0] INTERGER,\
									[MANUALRECORD1] INTERGER,\
									[LOCAL] INTEGER,\
									[REMOTE] INTEGER\
									)";

const char *CUserDao::SQL_INSERT_USR = "INSERT INTO [USER]\
(\
[USERNAME], [PASSWORD], [BINDMAC], [EMAIL], [ENABLE],\
[PREVIEW0], [PREVIEW1], [PLAYBACK0], [PLAYBACK1], [PTZCTRL0], [PTZCTRL1],\
[MANUALRECORD0], [MANUALRECORD1], [LOCAL], [REMOTE]\
)\
VALUES\
(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

const char *CUserDao::SQL_SAVE_USR = "UPDATE [USER] SET \
[PASSWORD] = ?,\
[BINDMAC] = ?,\
[EMAIL] = ?,\
[ENABLE] = ?,\
[PREVIEW0] = ?,\
[PREVIEW1] = ?,\
[PLAYBACK0] = ?,\
[PLAYBACK1] = ?,\
[PTZCTRL0] = ?,\
[PTZCTRL1] = ?,\
[MANUALRECORD0] = ?,\
[MANUALRECORD1] = ?,\
[LOCAL] = ?,\
[REMOTE] = ?\
WHERE [USERID] = ?";

//查询所有的用户的信息
const char *CUserDao::SQL_GET_USRS = "SELECT * FROM [USER] ORDER BY [USERID]";

//判断是否有某个用户
//参数 1 用户名
const char *CUserDao::SQL_HAVE_USR = "SELECT * FROM [USER] WHERE [USERNAME] = ?";

//删除某个用户
//参数 1 用户id
const char *CUserDao::SQL_DELETE_USR = "DELETE FROM [USER] WHERE [USERID] = ?";


CUserDao::CUserDao()
{
}

CUserDao::~CUserDao()
{
}

CUserDao *CUserDao::GetInstance()
{
	//未创建唯一实例,则新建
	if (NULL == m_pInstance)
	{
		m_pInstance = new CUserDao();
	}
	
	//否则直接返回
	return m_pInstance;
}

void CUserDao::DestroyInstance(CUserDao *pInstance)
{
#ifndef NO_USE_SQLITE3
	//检查参数
	if (pInstance != NULL)
	{
		//关闭数据库
		if (pInstance->m_pDb != NULL)
		{
			USR_DBG("Close Db\n");
			sqlite3_stmt *pStmt;
			while( (pStmt = sqlite3_next_stmt(m_pDb, 0))!= NULL)
			{
			    sqlite3_finalize(pStmt);
			}
			sqlite3_close(m_pDb);
			m_pDb = NULL;
		}

		m_pInsertUserStmt = NULL;
		m_pGetUserStmt = NULL;
		m_pHaveUserStmt = NULL;
		m_pSaveUserStmt = NULL;
		m_pDeleteUserStmt = NULL;
		delete pInstance;
	}
	//参数有误
	else
	{
		USR_ERROR("[%s]ERR: para error, pInstance=%p", __FUNCTION__, pInstance);
	}
#endif
}

int CUserDao::BindDb(char *szUsrDbFileName)
{
#ifndef NO_USE_SQLITE3
	//数据库文件名不能为空
	if (NULL == szUsrDbFileName)
	{
		USR_ERROR("[%s]ERR:para err, szUsrDbFileName=%s", __FUNCTION__, szUsrDbFileName);
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	//判断是否已经绑定到一个用户数据库
	if (m_pDb != NULL)
	{
		USR_ERROR("[%s]ERR:para err, already bind a database", __FUNCTION__);
		return -1;
	}
	
	//打开数据库,如果没有该文件则创建该文件
	int ret = sqlite3_open(szUsrDbFileName, &m_pDb);

	if (ret != SQLITE_OK)
	{
		USR_ERROR("[%s]ERR:open or create db %s failed, ret=%d", __FUNCTION__, szUsrDbFileName, ret);
		return IDVR_USR_ERROR_DB_OPEN_FAILED;
	}

#ifdef CHECK_SQL_FILE
	int integrityVerified = 0;
	sqlite3_stmt *integrity = NULL;
	
	printf("Start check %s\n", szUsrDbFileName);
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
		
		snprintf(command,sizeof(command), "rm -rf %s", szUsrDbFileName);
		mysystem(command);

		//打开数据库,如果没有该文件则创建该文件
		int ret = sqlite3_open(szUsrDbFileName, &m_pDb);
		if (ret != SQLITE_OK)
		{
			USR_ERROR("[%s]ERR:open or create db %s failed, ret=%d", __FUNCTION__, szUsrDbFileName, ret);
			return IDVR_USR_ERROR_DB_OPEN_FAILED;
		}

	}
#endif

	//准备创建用户表的语句
	sqlite3_stmt *pCreateTableStmt;

	ret = sqlite3_prepare_v2(m_pDb, SQL_CREATE_TABLE, strlen(SQL_CREATE_TABLE), &pCreateTableStmt, NULL);
	if (ret != SQLITE_OK)
	{
		USR_ERROR("[%s]ERR:prepare create table stmt failed, ret=%d", __FUNCTION__, ret);
		sqlite3_finalize(pCreateTableStmt);
		return -1;
	}

	//执行该语句
	ret = sqlite3_step(pCreateTableStmt);
	
	//创建失败
	if (SQLITE_DONE != ret)
	{
		USR_ERROR("Create Table User failed");
	}
	else
	{
		ret = 0;
	}

	sqlite3_finalize(pCreateTableStmt);
	USR_DBG("Create Table User Success\n");

	//int nResult = 0;
	//ret = 0;
	//预处理插入用户的SQL语句
	//ret = sqlite3_prepare_v2(m_pDb
	//				, SQL_INSERT_USR
	//				, strlen(SQL_INSERT_USR)
	//				, &m_pInsertUserStmt
	//				, NULL);
	//nResult |= ret;
	//USR_DBG("ret = %d\n", ret);

	//预处理保存用户信息的SQL语句
	//ret = sqlite3_prepare_v2(m_pDb
	//				, SQL_SAVE_USR
	//				, strlen(SQL_SAVE_USR)
	//				, &m_pSaveUserStmt
	//				, NULL);
	//USR_DBG("ret = %d\n", ret);
	//nResult |= ret;
	
	//预处理查询用户是否存在的SQL语句
	//ret = sqlite3_prepare_v2(m_pDb
	//				, SQL_HAVE_USR
	//				, strlen(SQL_HAVE_USR)
	//				, &m_pHaveUserStmt
	//				, NULL);
	//USR_DBG("ret = %d\n", ret);

	//预处理获取所有用户信息的SQL语句
	//ret = sqlite3_prepare_v2(m_pDb
	//				, SQL_GET_USRS
	//				, strlen(SQL_GET_USRS)
	//				, &m_pGetUserStmt
	//				, NULL);
	//USR_DBG("ret = %d\n", ret);
	//nResult |= ret;

	//预处理删除用户的SQL语句
	//ret = sqlite3_prepare_v2(m_pDb
	//				, SQL_DELETE_USR
	//				, strlen(SQL_DELETE_USR)
	//				, &m_pDeleteUserStmt
	//				, NULL);
	//USR_DBG("ret = %d\n", ret);
	//nResult |= ret;

	//预编译语句过程失败
	//if (nResult != 0)
	//{
	//	USR_ERROR("[%s]ERR:Prepare stmt failed", __FUNCTION__);
	//	return -1;
	//}
	
	USR_DBG("User Data base Initialize Success\n");

	return (int)ret;
#else
    return 0;
#endif
}

int CUserDao::InsertUser(const CUser &objUser)
{
#ifndef NO_USE_SQLITE3
	if (HaveUser(objUser.m_szUsername))
	{
		USR_DBG("User Already Exsits\n");
		return IDVR_USR_ERROR_USER_EXSIT_ALREADY;
	}

	int nResult = 0;
	int ret = 0;

	BEGIN_TIME(1);
	ret = sqlite3_prepare_v2(m_pDb, SQL_INSERT_USR, strlen(SQL_INSERT_USR), &m_pInsertUserStmt, NULL);

	//绑定用户名参数值
	ret = sqlite3_bind_text(m_pInsertUserStmt, 1, objUser.m_szUsername, -1, SQLITE_STATIC);
	nResult |= ret;
	USR_DBG("ret = %d\n", ret);

	//绑定密码参数值
	ret = sqlite3_bind_text(m_pInsertUserStmt, 2, objUser.m_szPassword, -1, SQLITE_STATIC);
	nResult |= ret;
	USR_DBG("ret = %d\n", ret);

	//绑定MAC参数值
	ret = sqlite3_bind_blob(m_pInsertUserStmt, 3, objUser.m_szMacAddress, MAC_ADDRESS_BYTES, SQLITE_STATIC);
	nResult |= ret;
	USR_DBG("ret = %d\n", ret);

	//绑定Email参数值
	ret = sqlite3_bind_text(m_pInsertUserStmt, 4, objUser.m_szEmail, -1, SQLITE_STATIC);
	nResult |= ret;
	USR_DBG("ret = %d\n", ret);

	//绑定用户是否启用参数值
	ret = sqlite3_bind_int(m_pInsertUserStmt, 5, objUser.m_bEnable);
	nResult |= ret;
	USR_DBG("ret = %d\n", ret);

	//绑定本地预览权限参数值
	ret = sqlite3_bind_int(m_pInsertUserStmt, 6, objUser.m_dwPreview[0]);
	nResult |= ret;
	USR_DBG("ret = %d\n", ret);

	//绑定远程预览权限参数值
	ret = sqlite3_bind_int(m_pInsertUserStmt, 7, objUser.m_dwPreview[1]);
	nResult |= ret;
	USR_DBG("ret = %d\n", ret);

	//绑定本地回放权限参数值
	ret = sqlite3_bind_int(m_pInsertUserStmt, 8, objUser.m_dwPlayback[0]);
	nResult |= ret;
	USR_DBG("ret = %d\n", ret);

	//绑定远程回放权限参数值
	ret = sqlite3_bind_int(m_pInsertUserStmt, 9, objUser.m_dwPlayback[1]);
	nResult |= ret;
	USR_DBG("ret = %d\n", ret);

	//绑定本地云台控制权限参数值
	ret = sqlite3_bind_int(m_pInsertUserStmt, 10, objUser.m_dwPtzCtrl[0]);
	nResult |= ret;
	USR_DBG("ret = %d\n", ret);

	//绑定远程云台控制权限参数值
	ret = sqlite3_bind_int(m_pInsertUserStmt, 11, objUser.m_dwPtzCtrl[1]);
	nResult |= ret;
	USR_DBG("ret = %d\n", ret);

	//绑定本地手动录像权限参数值
	ret = sqlite3_bind_int(m_pInsertUserStmt, 12, objUser.m_dwManualRecord[0]);
	nResult |= ret;
	USR_DBG("ret = %d\n", ret);

	//绑定远程手动录像权限参数值
	ret = sqlite3_bind_int(m_pInsertUserStmt, 13, objUser.m_dwManualRecord[1]);
	nResult |= ret;
	USR_DBG("ret = %d\n", ret);

	//绑定本地操作权限参数值
	ret = sqlite3_bind_int(m_pInsertUserStmt, 14, objUser.m_dwLocal);
	nResult |= ret;
	USR_DBG("ret = %d\n", ret);

	//绑定远程操作权限参数值
	ret = sqlite3_bind_int(m_pInsertUserStmt, 15, objUser.m_dwRemote);
	nResult |= ret;
	USR_DBG("ret = %d\n", ret);

	if (nResult != 0)
	{
		USR_ERROR("[%s]ERR: Bind Parameter Error", __FUNCTION__);
		ret = sqlite3_finalize(m_pInsertUserStmt);
		return -1;
	}
	//执行SQL语句
	//BEGIN_TIME(1)
	ret = sqlite3_step(m_pInsertUserStmt);
	USR_DBG("ret = %d\n", ret);
	//END_TIME
	END_TIME


	//执行错误
	if (ret != SQLITE_DONE)
	{
		USR_ERROR("[%s]ERR: Insert User Failed\n", __FUNCTION__);
		ret = sqlite3_finalize(m_pInsertUserStmt);
		return -1;
	}
	else
	{
		ret = sqlite3_finalize(m_pInsertUserStmt);
		USR_DBG("ret = %d\n", ret);
	}

	USR_DBG("Insert User Success\n");
#endif
	return (int)0;
}

int CUserDao::GetUsers(CUser *pUserList, int nCount)
{
#ifndef NO_USE_SQLITE3
	if ((pUserList == 0) || (nCount == 0))
	{
		USR_ERROR("[%s]ERR:para error, pUserList=%p, nCount=%d", __FUNCTION__, pUserList, nCount);
		return -1;
	}

	int i = 0;

	//将语句置为查询初始状态
	int ret = sqlite3_prepare_v2(m_pDb, SQL_GET_USRS, strlen(SQL_GET_USRS), &m_pGetUserStmt, NULL);

	while (i < nCount)
	{
		ret = sqlite3_step(m_pGetUserStmt);

		//查询到一行数据
		if (SQLITE_ROW == ret)
		{
			//获取每一列的数据
			//获取列的数据时列的编号从0开始,与绑定参数不同
			pUserList[i].m_nUserId = sqlite3_column_int(m_pGetUserStmt, 0);
			strcpy(pUserList[i].m_szUsername, (const char *)sqlite3_column_text(m_pGetUserStmt, 1)); 
			strcpy(pUserList[i].m_szPassword, (const char *)sqlite3_column_text(m_pGetUserStmt, 2)); 
			memcpy(pUserList[i].m_szMacAddress, sqlite3_column_blob(m_pGetUserStmt, 3), MAC_ADDRESS_BYTES); 
			strcpy(pUserList[i].m_szEmail, (const char *)sqlite3_column_text(m_pGetUserStmt, 4)); 
			
			pUserList[i].m_bEnable = sqlite3_column_int(m_pGetUserStmt, 5);
			pUserList[i].m_dwPreview[0] = sqlite3_column_int(m_pGetUserStmt, 6);
			pUserList[i].m_dwPreview[1] = sqlite3_column_int(m_pGetUserStmt, 7);
			pUserList[i].m_dwPlayback[0] = sqlite3_column_int(m_pGetUserStmt, 8);
			pUserList[i].m_dwPlayback[1] = sqlite3_column_int(m_pGetUserStmt, 9);
			pUserList[i].m_dwPtzCtrl[0] = sqlite3_column_int(m_pGetUserStmt, 10);
			pUserList[i].m_dwPtzCtrl[1] = sqlite3_column_int(m_pGetUserStmt, 11);
			pUserList[i].m_dwManualRecord[0] = sqlite3_column_int(m_pGetUserStmt, 12);
			pUserList[i].m_dwManualRecord[1] = sqlite3_column_int(m_pGetUserStmt, 13);
			pUserList[i].m_dwLocal = sqlite3_column_int(m_pGetUserStmt, 14);
			pUserList[i].m_dwRemote = sqlite3_column_int(m_pGetUserStmt, 15);

			USR_DBG("nUserId=%d, szUsername=%s, szPassword=%s, szEmail=%s, bEnable=%d\n", pUserList[i].m_nUserId, pUserList[i].m_szUsername,
					pUserList[i].m_szPassword, pUserList[i].m_szEmail, pUserList[i].m_bEnable);
			i++;
		}
		else
		{
			USR_DBG("ret = %d\n", ret);
			break;
		}
	}

	sqlite3_finalize(m_pGetUserStmt);
	return (int)i;
#else
    return 0;
#endif
}

int CUserDao::SaveUser(const CUser &objUser)
{
#ifndef NO_USE_SQLITE3
	if ((-1 == objUser.m_nUserId) || !HaveUser(objUser.m_szUsername))
	{
		USR_ERROR("[%s]ERR:User %s Not Exsit", __FUNCTION__, objUser.m_szUsername);
		return IDVR_USR_ERROR_USER_NOEXSIT;
	}
	
	int ret = 0;
	int nResult = 0;

	//预处理保存用户信息的SQL语句
	ret = sqlite3_prepare_v2(m_pDb, SQL_SAVE_USR, strlen(SQL_SAVE_USR), &m_pSaveUserStmt, NULL);
	USR_DBG("ret = %d\n", ret);

	ret = sqlite3_bind_text(m_pSaveUserStmt, 1, objUser.m_szPassword, -1, SQLITE_STATIC);
	nResult |= ret;
	USR_DBG("ret = %d\n", ret);

	ret = sqlite3_bind_blob(m_pSaveUserStmt, 2, objUser.m_szMacAddress, MAC_ADDRESS_BYTES, SQLITE_STATIC);
	nResult |= ret;
	USR_DBG("ret = %d\n", ret);

	ret = sqlite3_bind_text(m_pSaveUserStmt, 3, objUser.m_szEmail, -1, SQLITE_STATIC);
	nResult |= ret;
	USR_DBG("ret = %d\n", ret);

	ret = sqlite3_bind_int(m_pSaveUserStmt, 4, objUser.m_bEnable);
	nResult |= ret;
	USR_DBG("ret = %d\n", ret);

	ret = sqlite3_bind_int(m_pSaveUserStmt, 5, objUser.m_dwPreview[0]);
	nResult |= ret;
	USR_DBG("ret = %d\n", ret);

	ret = sqlite3_bind_int(m_pSaveUserStmt, 6, objUser.m_dwPreview[1]);
	nResult |= ret;
	USR_DBG("ret = %d\n", ret);

	ret = sqlite3_bind_int(m_pSaveUserStmt, 7, objUser.m_dwPlayback[0]);
	nResult |= ret;
	USR_DBG("ret = %d\n", ret);

	ret = sqlite3_bind_int(m_pSaveUserStmt, 8, objUser.m_dwPlayback[1]);
	nResult |= ret;
	USR_DBG("ret = %d\n", ret);

	ret = sqlite3_bind_int(m_pSaveUserStmt, 9, objUser.m_dwPtzCtrl[0]);
	nResult |= ret;
	USR_DBG("ret = %d\n", ret);

	ret = sqlite3_bind_int(m_pSaveUserStmt, 10, objUser.m_dwPtzCtrl[1]);
	nResult |= ret;
	USR_DBG("ret = %d\n", ret);

	ret = sqlite3_bind_int(m_pSaveUserStmt, 11, objUser.m_dwManualRecord[0]);
	nResult |= ret;
	USR_DBG("ret = %d\n", ret);

	ret = sqlite3_bind_int(m_pSaveUserStmt, 12, objUser.m_dwManualRecord[1]);
	nResult |= ret;
	USR_DBG("ret = %d\n", ret);

	ret = sqlite3_bind_int(m_pSaveUserStmt, 13, objUser.m_dwLocal);
	nResult |= ret;
	USR_DBG("ret = %d\n", ret);

	ret = sqlite3_bind_int(m_pSaveUserStmt, 14, objUser.m_dwRemote);
	nResult |= ret;
	USR_DBG("ret = %d\n", ret);

	ret = sqlite3_bind_int(m_pSaveUserStmt, 15, objUser.m_nUserId);
	nResult |= ret;
	USR_DBG("ret = %d\n", ret);

	if (nResult != 0)
	{
		USR_ERROR("[%s]ERR: bind para error", __FUNCTION__);
		sqlite3_finalize(m_pSaveUserStmt);
		return -1;
	}
	
	ret = 0;
	ret = sqlite3_step(m_pSaveUserStmt);
	USR_DBG("ret = %d\n", ret);
	if (SQLITE_DONE != ret)
	{
		USR_ERROR("[%s]ERR: sqlite error, save user", __FUNCTION__);
		sqlite3_finalize(m_pSaveUserStmt);
		return -1;
	}
	else
	{
		USR_DBG("ret = %d\n", ret);
	}

	sqlite3_finalize(m_pSaveUserStmt);
#endif
	return (int)0;
}

int CUserDao::HaveUser(const char *szUserName)
{
#ifndef NO_USE_SQLITE3
	if (NULL == szUserName)
	{
		USR_ERROR("[%s]ERR:para error, szUserName=%s", __FUNCTION__, szUserName);
		return false;
	}
	USR_DBG("szUsername=%s\n", szUserName);

	int ret = sqlite3_prepare_v2(m_pDb, SQL_HAVE_USR, strlen(SQL_HAVE_USR), &m_pHaveUserStmt, NULL);
	int bResult = 0;

	ret = sqlite3_bind_text(m_pHaveUserStmt, 1, szUserName, -1, SQLITE_STATIC);
	if (ret != SQLITE_OK)
	{
		USR_DBG("Bind Param error\n");
		sqlite3_finalize(m_pHaveUserStmt);
		return 0;
	}

	ret = sqlite3_step(m_pHaveUserStmt);
	USR_DBG("ret = %d\n", ret);
	
	//查询到结果
	if (SQLITE_ROW == ret)
	{
		USR_DBG("Has User %s\n", szUserName);
		bResult = sqlite3_column_int(m_pHaveUserStmt, 0);
	}
	//查询结果数为0
	else if (SQLITE_DONE == ret)
	{
		USR_DBG("Do Not Has User %s\n", szUserName);
	}
	//查询出现错误
	else
	{
		USR_ERROR("[%s]ERR:sqlite error, have user\n", __FUNCTION__);
	}

	sqlite3_finalize(m_pHaveUserStmt);

	return bResult;
#else
    return 0;
#endif
}

int CUserDao::DeleteUser(int nUserId)
{
#ifndef NO_USE_SQLITE3
	if (-1 == nUserId)
	{
		USR_ERROR("[%s]ERR:para error, nUserId=%d", __FUNCTION__, nUserId);
		return IDVR_ERROR_PARAMETER;
	}
	
	int ret = sqlite3_prepare_v2(m_pDb, SQL_DELETE_USR, strlen(SQL_DELETE_USR), &m_pDeleteUserStmt, NULL);
	int nResult = -1;
	ret = sqlite3_bind_int(m_pDeleteUserStmt, 1, nUserId);
	if (ret != SQLITE_OK)
	{
		USR_DBG("Bind Param Error\n");
		sqlite3_finalize(m_pDeleteUserStmt);
		return -1;
	}

	ret = sqlite3_step(m_pDeleteUserStmt);
	if (SQLITE_DONE == ret)
	{
		if (1 == sqlite3_changes(m_pDb))
		{
			USR_DBG("Delete 1 user\n");
			nResult = 0;
		}
		else
		{
			USR_DBG("No UserId = %d\n", nUserId);
			nResult = IDVR_USR_ERROR_USER_NOEXSIT;
		}
	}
	else
	{
		USR_ERROR("[%s]ERR:sqlite error, ret = %d", __FUNCTION__, ret);
		nResult = -1;
	}

	sqlite3_finalize(m_pDeleteUserStmt);
	USR_DBG("nResult=%d\n", nResult);
	return (int)nResult;
#else
    return 0;
#endif
}

