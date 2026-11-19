#ifndef __FTP_PROCESS_H__
#define __FTP_PROCESS_H__

/*@ 
 *@ file	 ftpClient.h
 *@ brief	 ftp相关操作对外接口
 *@ author	 樊聪
 *@ date	 2016年8月25日 14:27:02
 *@ version	 0.0.1
 */

#include "global_constant.h"

#define SAFE_UNLINK(x)   do             \
{                                       \
    if (0 == access(x, F_OK))           \
    {                                   \
        unlink(x);                      \
    }                                   \
}while(0)

/* Ftp传输类型枚举 */
typedef enum
{
	SEND_NONE =0,
    SEND_PICTURE,
    SEND_RECORDS,
    SEND_ALL,
    
}eFtpTransType;

/* 用户外部传入的相关参数 */
typedef struct ftp_user_param
{
    int  ftpenable;             // ftp使能开关 
    char host[DVR_MAX_HOST_LENTH];             // ftp主机名
    char user[DVR_MAX_USERNAME_LEN];             // ftp用户名    
    char pass[DVR_MAX_PASSWORD_LEN];             // ftp密码
    unsigned short port;          // ftp端口
    eFtpTransType transt_type;  // ftp传输类型
    int 	nFtpMode;			//0,表示主动模式，1表示被动模式
	char 	rmPath[256];		//FTP的录像文件远程服务器存储路径
} FTP_user_param, *pFTP_user_param;

#ifdef __cplusplus
extern "C" {
#endif

/* 初始化FTP参数(需要最先调用) */
void InitFtpParam(void);

/* 设置ftp的参数到全局变量里面 */
int SetFtpParam(const pFTP_user_param pUsrInfo, char* pszDid);//将用户输入的信息放到全局变量中

/*@prief SendFileToFtpServer 发送指定的文件到远程文件夹
 *@param[IN] ServerDir 远程端文件夹
 *@param[IN] ServerFile 远程端文件名
 *@param[IN] ClientFile 本地文件(全路径)
 *@return 0:成功 1:失败 -1:参数错误
 */
int SendFileToFtpServer(const char * RemoteDir, const char * RemoteFileName, const char * LocalFile);

/* 开始FTP传输(条件达到，可以进行ftp传输，函数内部自动清零) */
void StartFTPTransfor(void);

/* 获取ftp当前的主机连接状态 */
int GetFtpConnStatus(void);

/* 获取ftp是否使能 */
int GetFtpEnable(void);

/* 对外接口，FTP发送线程 */
void *SendFileToFtpServerThread(void *lparam);

/* 对外接口，设置传送的图片名*/
extern int SetFtpTransferName(const unsigned char *pfilename, const unsigned char *pdestfile);

/*发送测试文件*/
int SendFtpTestFile(void);

#ifdef __cplusplus
}
#endif


#endif // end of __FTP_PROCESS_H__
