
/*!
 *\file		idvr_rnl.c
 *\brief	RNL模块，利用syslog实现系统调试日志、跟踪日志、错误日志以及诊断宏功能
 *\author	张林
 *\date		2009/04/07
 */
#include        <syslog.h>
#include        <stdarg.h>
#include        <string.h>
#include        <stdlib.h>
#include        "idvr_rnl.h"
#include        "global_constant.h"
/**************************************************************************
 *                            常量                                        *
 **************************************************************************/




 /**************************************************************************
 *                            宏                                          *
 **************************************************************************/

/*!定义输入参数申明宏*/
#ifndef IN
#define IN
#endif

/*!定义输出参数申明宏*/
#ifndef OUT
#define OUT
#endif


/*!定义输入、输出参数申明宏*/
#ifndef IN_OUT
#define IN_OUT
#endif

#ifndef BOOL
#define BOOL char
#endif

#ifndef NULL
#define NULL
#endif

/**************************************************************************
 *                          数据类型                                       *
 **************************************************************************/


/**************************************************************************
 *                           全局变量                                      *
 **************************************************************************/
// char szVersion_DVRSDK_RNL [] = "0.0.2.1057 Build " __DATE__ " " __TIME__;

/**************************************************************************
 *                           局部函数原型                                  *
 **************************************************************************/


/**************************************************************************
 *                           全局函数实现                                 *
 **************************************************************************/

/*!
*  \fn  idvr_rnl_assert_exit
*  \brief 诊断函数,诊断失败时终止程序运行
*  \param strModule
*      模块名称，字符串长度<32个字节
*  \param condtion
*      条件
*  \return NULL
*/
void idvr_rnl_assert_exit(IN const char *strModule, IN BOOL condition)
{
	if ((condition))
	{
		/*do nothing */
	}
	else
	{
		char szModule[100] = { 0 };
		char sKeyValue[] = "[ASSERT]";
		strncpy(szModule, strModule, sizeof(szModule) - 1);
		strlcat(szModule, sKeyValue, sizeof(szModule));
		openlog(szModule, LOG_PID | LOG_CONS | LOG_PERROR, LOG_USER);
		syslog(LOG_EMERG, "***ASSERT FAILED***, Programm Terminated! [FILE = %s ,LINE = %d]", __FILE__, __LINE__);
		closelog();
		exit(-1);
	}
}



/*!
*  \fn  idvr_rnl_assert_ignore
*  \brief 诊断函数,诊断失败时程序继续运行，打印输出错误信息
*  \param strModule
*      模块名称，字符串长度<32个字节
*  \param condtion
*      条件
*  \return NULL
*/
void idvr_rnl_assert_ignore(IN const char *strModule, IN BOOL condition)
{
	if ((condition))
	{
		/*do nothing */
	}
	else
	{
		char szModule[MAX_LOG_LEN] = { 0 };

		strncpy(szModule, strModule, sizeof(szModule) - 1);
		strncat(szModule, "[ASSERT]", 9);
		openlog(szModule, LOG_PID | LOG_CONS | LOG_PERROR, LOG_USER);
		syslog(LOG_EMERG, "***ASSERT FAILED***, Programm Terminated! [FILE = %s ,LINE = %d]", __FILE__, __LINE__);
		closelog();
	}
}


/*!
*  \fn  idvr_rnl_assert_ex
*  \brief 诊断函数,诊断失败时程序继续运行，打印输出错误信息
*  \param strModule
*      模块名称，字符串长度<32个字节
*  \param condtion
*      条件不成立时，诊断失败
*  \param bExitWhenFailed
*      诊断失败时是否退出进程（一般用于调试版本）
*  \param fmt
*      诊断失败时输出的格式化字符串
*  \return NULL
*/
void idvr_rnl_assert_ex(IN const char *strModule, IN BOOL condition, IN BOOL bExitWhenFailed, IN const char *fmt, ...)
{
	if ((condition))
	{
		/*do nothing */
	}
	else
	{
		char szModule[MAX_LOG_LEN] = { 0 };

		va_list arg_ptr;

		strncpy(szModule, strModule, sizeof(szModule) - 1);
		strncat(szModule, "[ASSERT]", 9);

		openlog(szModule, LOG_PID | LOG_CONS | LOG_PERROR, LOG_USER);
		/*!输出诊断失败用户自定义信息 */
		va_start(arg_ptr, fmt);
		vsyslog(LOG_EMERG, fmt, arg_ptr);
		va_end(arg_ptr);

		syslog(LOG_EMERG, bExitWhenFailed ? "***ASSERT FAILED***, Programm Terminated! " : "***ASSERT FAILED***, Programm Continued! ");
		closelog();

		if (bExitWhenFailed)
			exit(-1);
	}
}

/*!
*  \fn  idvr_rnl_info
*  \brief 跟踪日志输出函数
*  \param strModule
*      模块名称，字符串长度<32个字节
*  \param fmt
*      格式化字符串
*  \return NULL
*/
void idvr_rnl_info(IN const char *strModule, IN const char *fmt, ...)
{
	char szModule[MAX_LOG_LEN] = { 0 };

	va_list arg_ptr;

	strncpy(szModule, strModule, sizeof(szModule) - 1);
	strncat(szModule, "[INFO]", 7);

	openlog(szModule, LOG_PID | LOG_CONS | LOG_PERROR, LOG_USER);

	va_start(arg_ptr, fmt);
	vsyslog(LOG_INFO, fmt, arg_ptr);
	va_end(arg_ptr);
	closelog();
}

/*!
*  \fn  idvr_rnl_error
*  \brief 错误日志输出函数
*  \param strModule
*      模块名称，字符串长度<32个字节
*  \param fmt
*      格式化字符串
*  \return NULL
*/
void idvr_rnl_error(IN const char *strModule, IN const char *fmt, ...)
{
	char szModule[MAX_LOG_LEN] = { 0 };

	va_list arg_ptr;

	strncpy(szModule, strModule, sizeof(szModule) - 1);
	strncat(szModule, "[ERROR]", 8);

	openlog(szModule, LOG_PID | LOG_CONS | LOG_PERROR, LOG_USER);

	va_start(arg_ptr, fmt);
	vsyslog(LOG_ERR, fmt, arg_ptr);
	va_end(arg_ptr);
	closelog();
}



/*!
*  \fn  idvr_rnl_debug
*  \brief 调试日志输出函数
*  \param strModule
*      模块名称，字符串长度<32个字节
*  \param fmt
*      格式化字符串
*  \return NULL
*/
void idvr_rnl_debug(IN const char *strModule, IN const char *fmt, ...)
{
	char szModule[MAX_LOG_LEN] = { 0 };

	va_list arg_ptr;

	strncpy(szModule, strModule, sizeof(szModule) - 1);
	strncat(szModule, "[DEBUG]", 8);

	openlog(szModule, LOG_PID | LOG_CONS | LOG_PERROR, LOG_USER);

	va_start(arg_ptr, fmt);
	vsyslog(LOG_DEBUG, fmt, arg_ptr);
	va_end(arg_ptr);
	closelog();
}


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
void idvr_rnl_log(IN enum LOG_LEVEL lv, IN const char *strModule, IN const char *fmt, ...)
{
	char szModule[MAX_LOG_LEN] = { 0 };

	int log_pririoty;


	va_list arg_ptr;

	strncpy(szModule, strModule, sizeof(szModule) - 1);
	switch (lv)
	{
		case LOG_LEVEL_DEBUG:
			{
				strncat(szModule, "[DEBUG]", 8);
				log_pririoty = LOG_DEBUG;
				break;
			}
		case LOG_LEVEL_INFO:
			{
				strncat(szModule, "[INFO]", 7);
				log_pririoty = LOG_INFO;
				break;
			}
		case LOG_LEVEL_ERROR:
			{
				strncat(szModule, "[ERROR]", 8);
				log_pririoty = LOG_ERR;
				break;
			}

		default:
			{
				strncat(szModule, "[DEBUG]", 8);
				log_pririoty = LOG_DEBUG;
				break;
			}
	}


	openlog(szModule, LOG_PID | LOG_CONS | LOG_PERROR, LOG_USER);

	va_start(arg_ptr, fmt);
	vsyslog(log_pririoty, fmt, arg_ptr);
	va_end(arg_ptr);
	closelog();
}
