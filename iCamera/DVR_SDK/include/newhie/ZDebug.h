/********************************************************************
文件名称: 	ZDebug.h
内容描述:	本文件定义模块对外提供的接口约定
创建者:		kevin2hy
创建时间:	2020年01月24日
*********************************************************************/

#ifndef H_ZDEBUG
#define H_ZDEBUG

#if defined(WIN32)

#ifdef ZDEBUG_EXPORTS
#define ZDEBUG_API __declspec(dllexport)
#else
#define ZDEBUG_API __declspec(dllimport)
#endif

#elif defined( __cplusplus)

#define ZDEBUG_API  extern "C" 

#else

#define ZDEBUG_API  

#endif

#include "./ZDebugCommon.h"

namespace	ZDebug
{

	/********************************************************************************************
	*										日志消息接口										*
	********************************************************************************************/

	/*!
	* @brief
	*	日志消息输出接口
	* @param level
	*	日志消息级别，详见eLogLevel类型定义
	* @param module
	*	模块名称，用于区分不同模块的日志消息
	* @param title
	*	日志标题，用于区分不同功能的日志消息
	* @param format
	*	日志消息格式，支持标准C字符串格式化规范
	* @return
	*	详见eErrorCode类型定义
	* @note
	*	接口支持根据配置将日志消息输出至控制台或者日志工具等软件中
	*	最大消息长度为MAX_LOG_LENGTH字节，详见MAX_LOG_LENGTH定义
	*/
	ZDEBUG_API ZDebug::eErrorCode ZLog(IN ZDebug::eLogLevel level, IN const char *module, IN const char *title, IN const char *format, IN ...);

	/*!
	* @brief
	*	日志消息输出接口
	* @param level
	*	日志消息级别，详见eLogLevel类型定义
	* @param module
	*	模块名称，用于区分不同模块的日志消息
	* @param title
	*	日志标题，用于区分不同功能的日志消息
	* @param file
	*	源文件字符串，NULL不显示
	* @param function
	*	函数名字符串，NULL不显示
	* @param line
	*	行号，-1不显示
	* @param format
	*	日志消息格式，支持标准C字符串格式化规范
	* @return
	*	详见eErrorCode类型定义
	* @note
	*	接口支持根据配置将日志消息输出至控制台或者日志工具等软件中
	*	最大消息长度为MAX_LOG_LENGTH字节，详见MAX_LOG_LENGTH定义
	*/
	ZDEBUG_API ZDebug::eErrorCode ZLogV1(IN ZDebug::eLogLevel level, IN const char *module, IN const char *title,IN const char *file, IN const char *function, IN const int line, IN const char *format, IN ...);

	/*!
	* @brief
	*	日志消息输出接口
	* @param level
	*	日志消息级别，详见eLogLevel类型定义
	* @param module
	*	模块名称，用于区分不同模块的日志消息
	* @param title
	*	日志标题，用于区分不同功能的日志消息
	* @param content
	*	日志消息内容
	* @param length
	*	日志消息内容长度，长度不受MAX_LOG_LENGTH定义的限制
	* @return
	*	详见eErrorCode类型定义
	* @note
	*	接口支持根据配置将日志消息输出至控制台或者日志工具等软件中
	*/
	ZDEBUG_API ZDebug::eErrorCode ZLogs(IN ZDebug::eLogLevel level, IN const char *module, IN const char *title, IN const char *content, IN int length);

	/*!
	* @brief
	*	日志消息输出接口
	* @param level
	*	日志消息级别，详见eLogLevel类型定义
	* @param module
	*	模块名称，用于区分不同模块的日志消息
	* @param title
	*	日志标题，用于区分不同功能的日志消息
	* @param file
	*	源文件字符串，NULL不显示
	* @param function
	*	函数名字符串，NULL不显示
	* @param line
	*	行号，-1不显示
	* @param content
	*	日志消息内容
	* @param length
	*	日志消息内容长度，长度不受MAX_LOG_LENGTH定义的限制
	* @return
	*	详见eErrorCode类型定义
	* @note
	*	接口支持根据配置将日志消息输出至控制台或者日志工具等软件中
	*/
	ZDEBUG_API ZDebug::eErrorCode ZLogsV1(IN ZDebug::eLogLevel level, IN const char *module, IN const char *title, IN const char *file, IN const char *function, IN const int line, IN const char *content, IN int length);

	/********************************************************************************************
	*										模块状态输出接口									*
	********************************************************************************************/
	/*!
	* @brief
	*	模块状态回调注册接口
	* @param module
	*	模块名称，请使用常量或者静态字符串，不做深拷贝
	* @param title
	*	模块标识，请使用常量或者静态字符串，不做深拷贝
	* @param cb
	*	工作状态输出回调
	* @param user
	*	用户数据
	* @return
	*	详见eErrorCode类型定义
	* @note
	*/
	ZDEBUG_API ZDebug::eErrorCode ModuleStateRegister(IN const char *module, IN const char *title, IN ZDebug::cb_fprintf cb, IN void *user);

	/*!
	* @brief
	*	模块状态回调注销接口
	* @param module
	*	模块名称
	* @param title
	*	模块标识
	* @return
	*	详见eErrorCode类型定义
	* @note
	*/
	ZDEBUG_API ZDebug::eErrorCode ModuleStateUnregister(IN const char *module, IN const char *title);

	/*!
	* @brief
	*	模块设参回调注册接口
	* @param module
	*	模块名称，请使用常量或者静态字符串，不做深拷贝
	* @param title
	*	模块标识，请使用常量或者静态字符串，不做深拷贝
	* @param cb
	*	工作状态输出回调
	* @param user
	*	用户数据
	* @return
	*	详见eErrorCode类型定义
	* @note
	*/
	ZDEBUG_API ZDebug::eErrorCode ModuleSetParamRegister(IN const char *module, IN const char *title, IN ZDebug::cb_setParam cb, IN void *user);

	/*!
	* @brief
	*	模块设参回调注销接口
	* @param module
	*	模块名称
	* @param title
	*	模块标识
	* @return
	*	详见eErrorCode类型定义
	* @note
	*/
	ZDEBUG_API ZDebug::eErrorCode ModuleSetParamUnregister(IN const char *module, IN const char *title);
	/********************************************************************************************
	*										模块属性接口										*
	********************************************************************************************/

	/*!
	* @brief
	*	模块属性设置接口
	* @param attribute
	*	属性类型，详见eAttribute类型定义
	* @param option
	*	属性选项，根据attribute的值见eAttribute各类型定义注释说明填充
	* @param value
	*	属性选项值
	* @param length
	*	属性选项值长度
	* @return
	*	详见eErrorCode类型定义
	* @note
	*	接口示例：
	*	eResult = SetAttribute(eAttributeOutputMode, eOutputSystem, &enable, sizeof(enable));
	*	eResult = SetAttribute(eAttributeLogLevel, eLogDebug, &enable, sizeof(enable));
	*/
	ZDEBUG_API ZDebug::eErrorCode SetAttribute(IN ZDebug::eAttribute attribute, IN int option, IN const void *value, IN int length);

	/*!
	* @brief
	*	设置特定模块属性设置接口
	* @param attribute
	*	属性类型，详见eAttribute类型定义
	* @param option
	*	属性选项，根据attribute的值见eAttribute各类型定义注释说明填充
	* @param value
	*	属性选项值
	* @param length
	*	属性选项值长度
	* @param modle
	*	需要设置熟悉的模块（协议）
	* @return
	*	详见eErrorCode类型定义
	* @note
	*/
	ZDEBUG_API ZDebug::eErrorCode SetAttributeByModule(IN ZDebug::eAttribute attribute, IN int option, IN const void *value, IN int length, IN char *modle);

	/*!
	* @brief
	*	模块注册接口
	* @param module
	*	模块名
	* @return
	*	详见eErrorCode类型定义
	* @note
	*/
	ZDEBUG_API ZDebug::eErrorCode registerModule(IN char *module, IN char *version);

	/*!
	* @brief
	*	模块注销接口
	* @param module
	*	模块名
	* @return
	*	详见eErrorCode类型定义
	* @note
	*/
	ZDEBUG_API ZDebug::eErrorCode UnregisterModule(IN char *module);

	/*!
	* @brief
	*	初始化接口
	* @return
	*	详见eErrorCode类型定义
	* @note
	*/
	ZDEBUG_API ZDebug::eErrorCode Initialize();

	/*!
	* @brief
	*	初设置配置文件路径接口
	* @param path
	*	zdebug配置文件路径 例如：/tmp/zdebug.ini , 设置多个的话以离初始化最近的一次生效
	* @return
	*	详见eErrorCode类型定义
	* @note
	*/
	ZDEBUG_API ZDebug::eErrorCode SetConfigFilePath(IN const char *path);

	/*!
	* @brief
	*	设置日志文件路径接口
	* @param path
	*	日志文件路径 例如：/tmp/zdebug.log , 设置多个的话以离初始化最近的一次生效
	* @return
	*	详见eErrorCode类型定义
	* @note
	*/
	ZDEBUG_API ZDebug::eErrorCode SetLogFilePath(IN const char *path);

	/*!
	* @brief
	*	将缓冲区内容写入到文件
	* @return
	*	详见eErrorCode类型定义
	* @note
	*/
	ZDEBUG_API ZDebug::eErrorCode WriteBufferContent2File();	
	/********************************************************************************************
	*										网络调试接口										*
	********************************************************************************************/

	/*!
	* @brief
	*	网络调试控制台创建接口
	* @param address
	*	接收调试请求地址，参数NULL为本地所有地址，为了确保系统安全，建议使用本地地址LOCAL_ADDRESS定义，并使用本地客户端调试
	* @param port
	*	接收调试请求端口，参数0为默认端口，默认端口见DEBUG_NETWORK_PORT定义
	* @return
	*	详见eErrorCode类型定义
	* @note
	*/
	ZDEBUG_API ZDebug::eErrorCode NetworkConsoleCreate(IN const char *address, IN unsigned short port);

	/*!
	* @brief
	*	网络调试控制台销毁接口
	* @return
	*	详见eErrorCode类型定义
	* @note
	*/
	ZDEBUG_API ZDebug::eErrorCode NetworkConsoleDestroy(void);

}

#endif