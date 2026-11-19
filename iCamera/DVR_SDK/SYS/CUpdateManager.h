
/*!
*\file   CUpdateManager.cpp 
*\brief  升级管理模块的声明
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CUpdateManager_49A241920117_INCLUDED
#define _INC_CUpdateManager_49A241920117_INCLUDED

/**************************************************************************
*                            常量                                        *
**************************************************************************/

/**************************************************************************
*                            宏                                          *
**************************************************************************/

/**************************************************************************
*                          数据类型                                       *
**************************************************************************/

class CBaseUpdate;

typedef enum
{
	Update_MIN,
	Update_USB,
	Update_MAX
} enumUpdateMode;

//##ModelId=49A241920117
class CUpdateManager
{
  public:
	//!\brief 获取句柄
	//!\return  成功为为句柄，失败为NULL
	//##ModelId=49BB3331008A
	static CUpdateManager *GetInstance();

	//!\brief 检测升级包是否合法
	//!\praram const char *Path 升级包路径
	//!\return  合法为1，版本不对0，升级包错误为-1
	static int CheckVersion(const char *Path);

  private:

	//全局实例句柄
	static CUpdateManager *theCUpdateManager;	

	//升级的类型
	enumUpdateMode m_enumUpdateMode;

	  CUpdateManager();
};

#endif							/* _INC_CUpdateManager_49A241920117_INCLUDED */
