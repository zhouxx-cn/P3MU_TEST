/*!
*\file   CUserDao.h
*\brief  用户数据库访问控制类定义
*\author yuk@higheasyrd.com
*\date	  2009/04/10
*\version 0.1
* 功能:支持数据库中用户的添加、删除、查询、修改
* 说明:本类为单实例类,主要供CUserManager使用
*/

#ifndef __CUSERDAO_H_
#define __CUSERDAO_H_

#include "USR/CUser.h"

#ifndef NO_USE_SQLITE3
#include "PUB/sqlite3.h"
#endif

//#include "PUB/sqlite3ext.h"

//!\brief 用户数据库访问控制类
class CUserDao
{
public:
	//!\brief 获取类的唯一实例
	//!\return 成功则返回实例的指针,否则返回NULL
	static CUserDao *GetInstance();

	//!\brief 销毁类的实例
	//!\param pInstance 指向实例的指针
	//!\return 无
	static void DestroyInstance(CUserDao *pInstance);

	//!\brief 初始化用户数据库 
	//!\param szUsrDbFileName 指定数据库的文件名，
	//! 如果没有该文件或数据库文件中没有用户表
	//! 系统会自动创建文件添加用户表并添加admin和default两个初始用户
	int BindDb(char *szUsrDbFileName);

	//!\brief 添加新的用户
	//!\param objUser 要添加的用户信息
	//!\return 0表示添加成功 其他表示错误码
	int InsertUser(const CUser &objUser);

	//!\brief 获取所有用户信息
	//!\param pUserList 用户列表缓冲区
	//!\param nCount 缓冲区
	//!\return 获得用户的个数
	int GetUsers(CUser *pUserList, int nCount);

	//!\brief 保存指定用户信息
	//!\param objUser 要保存的用户信息
	//!\return 0表示保存成功 其他表示错误码
	int SaveUser(const CUser &objUser);

	//!\brief 查询数据库中是否有某个用户
	//!\param szUsername 用户名
	//!\return >0表示有该用户 0 表示没有该用户
	int HaveUser(const char *szUserName);
	
	//!\brief 删除指定用户
	//!\param nUserId 要删除的用户的ID
	//!\return 0表示删除成功 其他表示错误码
	int DeleteUser(int nUserId);
private:
	//!\brief 构造函数
	CUserDao();

	//!\brief 析构函数
	~CUserDao();

private:
	//!\brief 用户数据库访问类的惟一实例
	static CUserDao *m_pInstance;

#ifndef NO_USE_SQLITE3
	//!\brief 打开的数据库handle的指针
	static sqlite3 *m_pDb;

	//!\brief 添加用户语句
	static sqlite3_stmt *m_pInsertUserStmt;

	//!\brief 保存用户语句
	static sqlite3_stmt *m_pSaveUserStmt;

	//!\brief 获取用户语句
	static sqlite3_stmt *m_pGetUserStmt;

	//!\brief 查询用户是否存在语句
	static sqlite3_stmt *m_pHaveUserStmt;

	//!\brief 删除用户语句
	static sqlite3_stmt *m_pDeleteUserStmt;
#endif
	//!\brief 创建用户表SQL语句
	const static char *SQL_CREATE_TABLE;

	//!\brief 新增用户SQL语句
	const static char *SQL_INSERT_USR;

	//!\brief 修改用户信息语句
	const static char *SQL_SAVE_USR;

	//!\brief 获取所有用户SQL语句
	const static char *SQL_GET_USRS;

	//!\brief 判断用户是否存在的语句
	const static char *SQL_HAVE_USR;

	//!\brief 删除用户的SQL语句
	const static char *SQL_DELETE_USR;
};

#endif

