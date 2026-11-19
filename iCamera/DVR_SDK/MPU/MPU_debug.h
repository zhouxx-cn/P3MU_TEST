
/*!
 *\file		MPU_debug.h
 *\brief	MPU模块，调试接口定义
 *\author	林云
 *\date		2009/03/23
 */

#ifndef _INC_MPU_DEBUG_H_
#define _INC_MPU_DEBUG_H_


#pragma once


#include <stdio.h>
#include <string.h>
#include "global_macro.h"		//全工程宏定义
#include "RNL/idvr_rnl.h"
#include "LogManager.h"


/**************************************************************************
 *                              宏定义                                    *
 **************************************************************************/

#if 0
//#define DEBUG_NOT_USE_AVL_LIB     //调试时，不使用AVL库
//#define DEBUG_NOT_USE_AVP_LIB     //调试时，不使用AVP库
#define DEBUG_NOT_USE_BAK_LIB	//调试时，不调用BAK库
#define DEBUG_NOT_USE_DIP_LIB	//调试时，不调用DIP库
#define DEBUG_NOT_USE_DOP_LIB	//调试时，不调用DOP库
#define DEBUG_NOT_USE_NET_LIB	//调试时，不调用NET库
#define DEBUG_NOT_USE_OPL_LIB	//调试时，不使用OPL库
#define DEBUG_NOT_USE_PRW_LIB	//调试时，不使用PRW库
#define DEBUG_NOT_USE_PTZ_LIB	//调试时，不使用PTZ库
#define DEBUG_NOT_USE_SNP_LIB	//调试时，不使用Snapshot库
//#define DEBUG_NOT_USE_STR_LIB     //调试时，不使用STR库
#define DEBUG_NOT_USE_SYS_LIB	//调试时，不使用SYS库
#define DEBUG_NOT_USE_USR_LIB	//调试时，不调用USR库
#else
//#define DEBUG_NOT_USE_STR_LIB
//#define DEBUG_NOT_USE_SNP_LIB
#define DEBUG_NOT_USE_DIP_LIB	//调试时，不调用DIP库
#endif

#ifdef USE_ICAMERA
#define DEBUG_NOT_USE_AVP_LIB	//调试时，不使用AVP库
#define DEBUG_NOT_USE_PRW_LIB	//调试时，不使用PRW库
#define DEBUG_NOT_USE_BAK_LIB	//调试时，不调用BAK库
#endif

//#define   USE_MPU_RNL         //是否使用RNL库

//#define   USE_MPU_DEBUG               //输出调试信息
//#define   USE_MPU_INFO			//输出日志信息
//#define   USE_MPU_ERROR			//输出错误信息

//使用诊断宏    0表示不使用，1表示出现错误退出程序（用于模块单调），2表示出现错误继续运行（用于模块联调）
#define	USE_MPU_ASSERT			2


//模块名称
#define MPU_MODULE_NAME			"[MPU]"

#define  MPU_ERR(format...)     LOG_E(LOG_MODULE_MPU,format) 
#define  MPU_WARN(format...)    LOG_W(LOG_MODULE_MPU,format) 
#define  MPU_INFO(format...)    LOG_I(LOG_MODULE_MPU,format) 
#define  MPU_DBG(format...)     LOG_D(LOG_MODULE_MPU,format) 


//诊断宏
#if (USE_MPU_ASSERT == 0)
//不使用
#define MPU_CHK(condition)		((void)0)
#elif (USE_MPU_ASSERT == 1)
//退出
#define MPU_CHK(condition)		idvr_rnl_assert_ex(MPU_MODULE_NAME, !!(condition), 1, "Assert Failed! [%s-%s:%d]: %s", __FILE__, __FUNCTION__, __LINE__, #condition)
#else
//不退出
#define MPU_CHK(condition)		idvr_rnl_assert_ex(MPU_MODULE_NAME, !!(condition), 0, "Assert Failed! [%s-%s:%d]: %s", __FILE__, __FUNCTION__, __LINE__, #condition)
#endif


#ifdef USE_MPU_DEBUG
#include "PUB/timex.h"			//时间转换函数

//打印时间信息
#define MPU_PRINT_TIME(x)				\
do										\
{										\
	char szTime[REC_TIME_LEN] = { 0 };		\
	time_to_rectime(szTime, x);				\
	MPU_DBG("%s \t %s\n", #x, szTime);		\
}										\
while (0)

#else

#define MPU_PRINT_TIME(x...)	((void)0)

#endif

//判断指针型参数是否为空
#define MPU_CHECK_POINTER_PARAMETER(x)					\
do														\
{														\
	if (NULL == x)										\
	{													\
		MPU_ERR ("NULL == %s\n", #x);					\
		MPU_CHK(NULL != x);								\
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;	\
	}													\
}														\
while (0)
#define PRINT_INFO(fmt, args...)  \
do{   \
	printf("\033[1;34m [%s:%d->%s] " fmt,__FILE__,__LINE__,__func__,##args);\
	printf("\033[0m\n");}while(0)

#endif							/* #ifndef _INC_MPU_DEBUG_H_ */
