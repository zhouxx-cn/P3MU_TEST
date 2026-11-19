
/*!
*\file   CBaseUpdate.h
*\brief  升级的基类声明
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CBASEUPDATE_49A236F60003_INCLUDED
#define _INC_CBASEUPDATE_49A236F60003_INCLUDED

#include <pthread.h>


/**************************************************************************
*                            常量                                        *
**************************************************************************/

/**************************************************************************
*                            宏                                          *
**************************************************************************/
//状态宏
/**************************************************************************
*                          数据类型                                       *
**************************************************************************/
extern "C" void WriteStateToFile(int state);

//##ModelId=49A236F60003
class CBaseUpdate
{
  public:		  

	  CBaseUpdate(char *Filter);

	 ~CBaseUpdate();

  private:

	//是否正在升级
	int bUpdate;

	//文件过滤利用后缀
	char m_Filter[20];

	//进度回调函数
	typedef void (*CallBack) (int nPercent);

	CallBack m_funCallBack;

	//源文件
	char m_src[256];

	//目的文件
	char m_des[256];

	//符合条件的文件个数
	unsigned int m_nCount;
};

#endif							/* _INC_CBASEUPDATE_49A236F60003_INCLUDED */
