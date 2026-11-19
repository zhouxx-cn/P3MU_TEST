#ifndef __FTP_PROCESS_H__
#define __FTP_PROCESS_H__

/*@ 
 *@ file	 ftp_process.h
 *@ brief	 ftp相关操作对外接口
 *@ author	 樊聪
 *@ date	 2016年3月10日 18:59:24
 *@ version	 0.0.2
 */

#include "ncftp.h"
#include "ncftp_errno.h"

#define PRTCOLOR
#define MODENAME   "FTP"
extern int LogLevel;

typedef enum {
	ERROR_LEVEL = 0,
	DEBUG_LEVEL = 1,
	INFO_LEVEL  = 2,
} log_level;

#ifdef PRTCOLOR
#define NONE         "\033[m"  
#define RED          "\033[0;32;31m"
#define LIGHT_RED    "\033[1;31m"
#define GREEN        "\033[0;32;32m"
#define LIGHT_GREEN  "\033[1;32m"
#define BLUE         "\033[0;32;34m"
#define LIGHT_BLUE   "\033[1;34m"
#define DARY_GRAY    "\033[1;30m"
#define CYAN         "\033[0;36m"
#define LIGHT_CYAN   "\033[1;36m"
#define PURPLE       "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN        "\033[0;33m"
#define YELLOW       "\033[1;33m"
#define LIGHT_GRAY   "\033[0;37m"
#define WHITE        "\033[1;37m"
#else
#define NONE
#define RED
#define LIGHT_RED
#define GREEN
#define LIGHT_GREEN
#define BLUE
#define LIGHT_BLUE
#define DARY_GRAY
#define CYAN
#define LIGHT_CYAN
#define PURPLE
#define LIGHT_PURPLE
#define BROWN
#define YELLOW
#define LIGHT_GRAY
#define WHITE
#endif

#define	PRINT(mylog, LOG_LEVEL, format, args...)	 do {	 \
       if (mylog >= LOG_LEVEL) { \
        printf(format, ##args);  \
       }	 \
      } while (0)

#define ERROR(format, args...)	 PRINT(LogLevel, ERROR_LEVEL, LIGHT_RED "[%s][ERROR][%s:%d]" NONE format, MODENAME, __FILE__, __LINE__, ##args)
#define DEBUG(format, args...)	 PRINT(LogLevel, DEBUG_LEVEL, LIGHT_GREEN "[%s][DEBUG]" NONE format, MODENAME, ##args)
#define INFO(format, args...)	 PRINT(LogLevel, INFO_LEVEL, LIGHT_PURPLE "[%s][INFO]" NONE format, MODENAME, ##args)


/* Ftp传输类型枚举 */
typedef enum
{
    SEND_PICTURE = 0,
    SEND_RECORDS,
    SEND_ALL,
    SEND_NONE,
}eFtpTransType;

/* 用户外部传入的相关参数 */
typedef struct ftp_user_param
{
    int  ftpenable;             // ftp使能开关 
    char host[128];             // ftp主机名
    char user[128];             // ftp用户名    
    char pass[256];             // ftp密码
    unsigned short port;          // ftp端口
    eFtpTransType transt_type;  // ftp传输类型
} FTP_user_param, *pFTP_user_param;

typedef struct ftp_lib_param
{
	FTPLibraryInfo    ftplip;
	FTPConnectionInfo ftpcip;
} FtpLibParam, *pFtpLibParam;

typedef struct ftp_param_handle
{
    FTP_user_param   user_param;  // ftp用户传入参数
	FtpLibParam      libs_param;  // ftp库操作
} FTP_param_handle, *pFTP_param_handle;

#ifdef __cplusplus
extern "C" {
#endif


/* 设置ftp的参数到全局变量里面(必须最先调用) */
int SetFtpParam(const pFTP_user_param pUsrInfo, char* pszDid);//将用户输入的信息放到全局变量中

/* 获取ftp的参数和当前的主机连接状态 */
void GetFtpParam(pFTP_user_param pUsrInfo);

/* 获取ftp当前的主机连接状态 */
int GetFtpConnStatus(void);

/* ftp文件传输线程 */
void *SendFileToFtpServerThread(void *lparam);


#ifdef __cplusplus
}
#endif


#endif // end of __FTP_PROCESS_H__
