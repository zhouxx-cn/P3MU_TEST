/********************************************************************
文件名称: 	ZDebugCommon.h
内容描述:	本文件定义模块对外提供的接口依赖类型约定
创建者:		kevin2hy
创建时间:	2020年01月24日
*********************************************************************/

#ifndef H_ZDEBUG_COMMON

#define H_ZDEBUG_COMMON

namespace	ZDebug
{

	/********************************************************************************************
	*											宏定义											*
	********************************************************************************************/

	//参数属性
#ifndef IN
	/** @brief 入参																						*/
#define IN
#endif

#ifndef OUT
	/** @brief 出参																						*/
#define OUT
#endif

#ifndef IN_OUT
	/** @brief 入出参																					*/
#define IN_OUT
#endif

#ifndef CALLBACK
	/** @brief 回调																						*/
#define CALLBACK
#endif

#ifndef ENABLE_UINT
	/** @brief 功能启用值																				*/
#define ENABLE_UINT						((unsigned int)1)
#endif

#ifndef ENABLE_BOOL
	/** @brief 功能启用值																				*/
#define ENABLE_BOOL						(true)
#endif

#ifndef DISABLE_UINT
	/** @brief 功能禁用值																				*/
#define DISABLE_UINT					((unsigned int)0)
#endif

#ifndef DISABLE_BOOL
	/** @brief 功能禁用值																				*/
#define DISABLE_BOOL					(false)
#endif

	//模块约束
#ifndef MAX_PATH_LENGTH
	/** @brief 最大路径长度																				*/
#define MAX_PATH_LENGTH					(256)
#endif

#ifndef MAX_IPV4_ADDRESS
	/** @brief 最大IP地址缓存长度																		*/
#define MAX_IPV4_ADDRESS				(32)
#endif

#ifndef MAX_LOG_LENGTH
	/** @brief 最大日志消息长度（只读）																	*/
#define MAX_LOG_LENGTH					(8 * 1024)
#endif

#ifndef LOCAL_ADDRESS
	/** @brief 本地地址																					*/
#define LOCAL_ADDRESS					("127.0.0.1")
#endif

#ifndef DEBUG_NETWORK_PORT
	/** @brief 网络调试端口（默认）																		*/
#define DEBUG_NETWORK_PORT				(8799)
#endif

	/********************************************************************************************
	*											枚举定义										*
	********************************************************************************************/

	//错误码定义
	typedef enum eErrorCode
	{
		eErrorSuccessed						= 0,	//成功
		eErrorFailed,								//失败
		eErrorNone,									//正常
		eErrorParameter,							//参数错误
		eErrorTimeout,								//超时错误
		eErrorSystem,								//系统接口错误
		eErrorRepeated,								//重复调用
	}eErrorCode;

	//模块属性定义
	typedef enum eAttribute
	{
		eAttributeBegin						= 0,	//开始
		eAttributeOutputMode,						//输出方式属性，见eOutputMode定义
		eAttributeLogLevel,							//日志消息输出级别属性，见eLogLevel定义
		eAttributeLogProperty,						//日志消息输出属性，见eLogProperty定义
		eAttributeConfigFile,						//配置文件属性,文件的绝对路径 例如：/tmp/zdebug.ini
		eAttributeEnd,								//结束
	}eAttribute;

	//日志消息输出方式
	typedef enum eOutputMode
	{
		eOutputConsole						= 0,	//控制台输出，通过printf输出
		eOutputSystem,								//系统日志输出，Windows平台通过OutputDebugString输出，Linux平台通过syslog输出
		eOutputFile,								//日志文件输出，默认为当前目录，支持通过eLogPropertyPathFormat设置文件路径
	}eOutputMode;

	//日志消息输出级别
	//各个日志消息输出级别分别支持使能控制
	typedef enum eLogLevel
	{
		eLogBegin							= 0,	//开始
		eLogDebug,									//调试程序，开发与调试时输出
		eLogInfo,									//运行日志，跟踪模块及流程的执行日志
		eLogWarning,								//警告提示，一般性错误，软件可继续执行
		eLogError,									//错误提示，出现意外错误，已影响用户使用，但软件可继续执行
		eLogFatal,									//致命错误，出现严重错误，可能导致软件退出
		eLogEnd,									//结束
	}eLogLevel;

	//日志消息输出属性
	typedef enum eLogProperty
	{
		eLogPropertyDateTime				= 0,	//日志输出增加日期时间信息
		eLogPropertyPathFormat,						//日志输出路径格式，例：value = "/tmp/"，模块会增加日期时间参数
		eLogPropertyRedirectConsole,				//重定向标准输出至控制台，仅支持在linux平台时使用
		eLogPropertyRedirectTTY,					//重定向标准输出至虚拟终端，仅支持在linux平台时使用
		eLogPropertyFocusModule,					//通过高亮的颜色显示焦点模块的日志输出
		eLogPropertyThreadID,						//日志输出增加线程ID信息
		eLogPropertyFFLShow,							//是否显式文件/函数/行号信息
		eLogPropertyLogFileMaxSize,				//每个日志文件可存储的最大大小
		eLogPropertyLogFileDuration,				//每个日志文件可存储的最长时长
		eLogPropertyLogcount,				//日志文件可存储的最大个数
	}eLogProperty;

	/********************************************************************************************
	*											回调定义										*
	********************************************************************************************/
	/*!
	*  \brief
	*	工作状态输出回调
	* @param fd
	*	输出句柄标识符
	* @param module
	*	模块名称
	* @param title
	*	模块标识
	* @param user
	*	用户数据
	* @return
	*	返回0
	* @note
	*	通过ssize_t result = write(fd, message, length)输出工作状态信息
	*/
	typedef int(CALLBACK *cb_fprintf)(IN int fd, IN const char *module, IN const char *title, IN void *user);
	typedef int(CALLBACK *cb_setParam)(IN int value, IN void *user);

}

#endif
