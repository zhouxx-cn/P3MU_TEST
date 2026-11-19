
/*!
*\file   CUpdateManager.cpp 
*\brief  升级管理模块的实现
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/

#include "CUpdateManager.h"
#include "idvr_errno.h"
#include "debug.h"
// #include "sys_banned.h"
#include "global_gather.h"


/**************************************************************************
*                            常量                                        *
**************************************************************************/


/**************************************************************************
*                            宏                                          *
**************************************************************************/

/**************************************************************************
*                          数据类型                                       *
**************************************************************************/


/**************************************************************************
*                           全局变量                                      *
**************************************************************************/

CUpdateManager *CUpdateManager::theCUpdateManager = NULL;

/**************************************************************************
*                           局部函数原型                                  *
 **************************************************************************/


/**************************************************************************
*                           全局函数函数实现                               *
 **************************************************************************/

//!\brief 获取句柄
//!\return  成功为为句柄，失败为NULL
//##ModelId=49BB3331008A
CUpdateManager *CUpdateManager::GetInstance()
{
	// TODO: Add your specialized code here.

	if (NULL == theCUpdateManager)
	{
		theCUpdateManager = new CUpdateManager();
	}

	return (CUpdateManager *) theCUpdateManager;
}

extern int check_version(const char *_path);

//!\brief 检测升级包是否合法
//!\praram const char *Path 升级包路径
//!\return  合法为1，版本不对0，升级包错误为-1
int CUpdateManager::CheckVersion(const char *Path)
{
	return check_version(Path);
}

//!\brief 构造函数
//!\return  无
CUpdateManager::CUpdateManager()
{
	m_enumUpdateMode = Update_MIN;
}

