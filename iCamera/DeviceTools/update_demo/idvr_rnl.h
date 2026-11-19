/*!
 *\file   idvr_rnl.h
 *\brief  提供iDVR的运行日志输出函数
 *\author 张林
 *\date	  2009/4/2
 *\version 0.1
 */

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_IDVRRNL_499BBFC2010F_INCLUDED
#define _INC_IDVRRNL_499BBFC2010F_INCLUDED



enum LOG_LEVEL
{
    LOG_LEVEL_DEBUG,
        LOG_LEVEL_INFO,
        LOG_LEVEL_ERROR
};




/*!
*  \fn  idvr_rnl_log
*  \brief 通用日志输出函数
*  \param lv
*       日志等级
*  \param strModule
*      模块名称，字符串长度<32个字节
*  \param fmt
*      格式化字符串
*  \return NULL
*/
#ifdef __cplusplus
extern "C" {
#endif


void idvr_rnl_log(enum LOG_LEVEL lv, const char *strModule, const char *fmt, ...);

#ifdef __cplusplus
};
#endif


/*!当程序要发布时,使用此宏*/
#define PROGRAM_RNL_RELEASE

/*!当程序处于调试状态，使用此宏*/
//#define PROGRAM_RNL_DEBUG


/*!中断当前程序执行，打印中断发生的文件、行号，该宏一般在单调时使用*/
//#define ASSERT_EXIT_M

/*!打印程序发生错误或异常的文件,行号,继续进行后续的操作，该宏一般在联调时使用*/
#define ASSERT_CONTINUE_M


/*!定义IDVR_RNL_ASSERT宏,用于诊断*/
#ifdef ASSERT_EXIT_M
#define IDVR_RNL_ASSERT(module, condition) idr_rnl_assert_exit(module, condition)
#else
#define IDVR_RNL_ASSERT(module, condition) idvr_rnl_assert_ingore(module, condition)
#endif

/*!定义IDVR_RNL_INFO宏,用于输出跟踪日志*/
#define IDVR_RNL_INFO(module, x...)  idvr_rnl_log(LOG_LEVEL_INFO, module, x)



/*!定义IDVR_RNL_ERROR宏,用于输出错误日志*/
#define IDVR_RNL_ERROR(module, x...) idvr_rnl_log(LOG_LEVEL_ERROR,module, x)


/*!定义IDVR_RNL_DEBUG宏,用于输出调试日志*/
#ifdef PROGRAM_RNL_DEBUG
#define IDVR_RNL_DEBUG(module, x...) idvr_rnl_log(LOG_LEVEL_DEBUG, module, x)
#else
#define IDVR_RNL_DEBUG(module, x...) 
#endif
#endif
