
/*!
*\file   debug.h 
*\brief  关于系统运行中打印消息的宏
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/
#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdio.h>
#include <string.h>

/**************************************************************************
*                            常量                                        *
**************************************************************************/




/**************************************************************************
*                            宏                                          *
**************************************************************************/
//模块名称.每个模块需要重定义该模块名称
#define __MODULE_NAME__ "[SnapShot]"
//#define _RNL_

/**************************************************************************
*                          数据类型                                       *
**************************************************************************/


/**************************************************************************
*                           全局变量                                      *
**************************************************************************/


/**************************************************************************
*                           局部函数原型                                  *
 **************************************************************************/

//在函数失败时打印该消息，以便分析失败类型

//记录日志
#ifdef _RNL_
#define SnapShot_LOG(level, x...)												\
do																			\
{																			\
	char szInfo[512] = {0};															\
	int nLen;																	\
	snprintf(szInfo, sizeof(szInfo), "[%s-%s:%d]: ", __FILE__, __FUNCTION__, __LINE__);	\
	nLen = strlen(szInfo);														\
	snprintf(&szInfo[nLen], sizeof(szInfo) - nLen, x);									\
	idvr_rnl_log(level, __MODULE_NAME__, "%s", szInfo);							\
}																			\
while (0)
#else
#define SnapShot_LOG(level, x...)													\
do																				\
{																				\
	char szInfo[512];																\
	int nLen;																		\
	snprintf(szInfo, sizeof(szInfo), "[SnapShot][%s-%s:%d]: ", __FILE__, __FUNCTION__, __LINE__);	\
	nLen = strlen(szInfo);															\
	snprintf(&szInfo[nLen], sizeof(szInfo) - nLen, x);										\
	printf("%s", szInfo);																\
}																				\
while (0)
#endif


#if 1
#define ERROR_MSG(x...)	SnapShot_LOG(LOG_LEVEL_ERROR, x)
#else
#define  ERROR_MSG(x...)
#endif


//对一些模块认为不正常的参数进行提示
#if 1
#define WARN_MSG(x)														\
do																		\
{	\
	if (!(x))																\
	{																	\
		SnapShot_LOG(LOG_LEVEL_DEBUG, "此处接口返回值错误\n");	\
	}																	\
}																		\
while (0)
#else
#define WARN_MSG(x)
#endif


//在函数失败时打印该消息，以便分析失败类型
#if 0
#define DEBUG_MSG(x...)	SnapShot_LOG(LOG_LEVEL_DEBUG, x)
#else
#define  DEBUG_MSG(x...)
#endif

//跟踪业务执行路径
#if 1
#define TRACE(x...)	SnapShot_LOG(LOG_LEVEL_INFO, x)
#else
#define  TRACE(x...)

#endif
#endif
