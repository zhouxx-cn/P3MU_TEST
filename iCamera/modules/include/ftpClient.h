#ifndef __FTP_PROCESS_H__
#define __FTP_PROCESS_H__

/*@ 
 *@ file	 ftpClient.h
 *@ brief	 ftp相关操作对外接口
 *@ author	 樊聪
 *@ date	 2017年10月17日 14:56:31
 *@ version	 0.0.1
 */

#include <unistd.h>


#define LOCAL_FTP_PIC_DIR       "/tmp/picture/pic_for_ftp"

#if defined (__USE_EMMC__)
#define LOCAL_FTP_VIDEO_DIR     "/tmp/app/disk/mmcblk1p1/video"
#else
#define LOCAL_FTP_VIDEO_DIR     "/tmp/app/disk/mmcblk0p1/video"
#endif

#define REMOTE_FTP_PIC_DIR      "./ftp/record/picture"
#define REMOTE_FTP_VIDEO_DIR    "./ftp/record/video"

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

#define DVR_MAX_HOST_LENTH	            (128)
/** @brief 最大用户名长度														          		 */
#define DVR_MAX_USERNAME_LEN 			(64)
/** @brief 最大密码长度															          		 */
#define DVR_MAX_PASSWORD_LEN 			(64)

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

/*@prief SetFtpTransferName 设置传输文件的文件名到传输列表里面，并发送传输命令
 *@param[IN]  pfilename 源文件名称(限定文件类型只可以是JPG图片或者AVI录像)
 *@param[IN]  pdestfile 目标文件名(限定文件类型只可以是JPG图片或者AVI录像)
 *@return 0 成功 -1失败
 */
extern int SetFtpTransferName(const unsigned char *pfilename, const unsigned char *pdestfile);

/* 发送测试文件 */
extern int SendFtpTestFile(void);

/* 设置ftp的参数到全局变量里面 */
extern int SetFtpParam(const pFTP_user_param pUsrInfo, char* pszDid);

/*@prief SendFileToFtpServer 发送指定的文件到远程文件夹
 *@param[IN] RemoteDir 远程端文件夹
 *@param[IN] RemoteFileName 远程端文件名
 *@param[IN] LocalFile 本地文件(全路径)
 *@return 0:成功 1:失败 -1:错误(参数错误或者ftp未使能)
 */
extern int SendFileToFtpServer(const char* RemoteDir, const char *RemoteFileName, const char *LocalFile);

/* 获取ftp当前的主机连接状态 */
extern int GetFtpConnStatus(void);

/* 获取ftp是否使能 */
extern int GetFtpEnable(void);

/* 对外接口，FTP发送线程 */
extern void *SendFileToFtpServerThread(void *lparam);


#ifdef __cplusplus
}
#endif


#endif // end of __FTP_PROCESS_H__
