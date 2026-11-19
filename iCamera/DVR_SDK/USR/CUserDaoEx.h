/*!
*\file   CUserDao.h
*\brief  用户数据库访问控制类定义
*\author yuk@higheasyrd.com
*\date	  2009/04/14
*\version 0.1
* 功能:支持数据库中用户的添加、删除、查询、修改
* 说明:本类为单实例类,主要供CUserManager使用
*/

#ifndef __CUSERDAOEX_H_
#define __CUSERDAOEX_H_
#include "CUser.h"

//!\brief 用户文本数据库访问控制类
class CUserDaoEx
{
public:
	//!\brief 获取类的唯一实例
	//!\return 成功则返回实例的指针,否则返回NULL
	static CUserDaoEx *GetInstance();

	//!\brief 销毁类的实例
	//!\param pInstance 指向实例的指针
	//!\return 无
	static void DestroyInstance(CUserDaoEx *pInstance);

	//!\brief 初始化用户数据库 
	//!\param szUsrDbFileName 指定数据库的文件名，
	//! 如果没有该文件,系统会自动创建文件
	int BindDb(const char *szUsrDbFileName, int reset);

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

	//!\brief 查询某个用户是否存在
	//!\param szUsername 用户名
	//!\return true表示有该用户 false 表示没有该用户
	bool HaveUser(const char *szUserName);
	
	//!\brief 删除指定用户
	//!\param nUserId 要删除的用户的ID
	//!\return 0表示删除成功 其他表示错误码
	int DeleteUser(int nUserId);

    int ReadOldUser();

private:
	//!\brief 构造函数
	CUserDaoEx();

	//!\brief 析构函数
	~CUserDaoEx();

	//!\brief 将用户读入到数组中
	int ReadUsers();

	//!\brief 将用户写入到文件
	int WriteUsers();

	//!\brief 用户信息占用的字节数
	static int UserSize();

private:
	//!\brief 用户数据访问类的惟一实例
	static CUserDaoEx *m_pInstance;

	//!\brief 绑定的用户数据文件的大小
	static char *m_szBindFile;

	//!\brief 用户数组
	CUser m_tUsers[IDVR_MAX_USER];

	//!\brief 用户数
	int m_nUserCount;

	//!\brief 用户索引
	int m_nUserIndex;
};
#endif

