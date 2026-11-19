/*@ 
 *@ file	 ftp_process.c
 *@ brief	 ftp相关操作对外接口具体实现
 *@ author	 樊聪
 *@ date	 2016年3月10日 18:59:24
 *@ version	 0.0.2
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <dirent.h>

//#define TEST

#ifndef TEST

#include "msg_net.h"
#include "ncftp.h"
#include "ncftp_errno.h"
#include "ftp_process.h"

pthread_mutex_t ftp_mutex;
pthread_mutex_t global_ftp_mutex;

static FTP_param_handle global_ftp_param;

int updateftpstatus = 0;
int bIsInited       = 0;
int g_ftpstatus     = 0;

int LogLevel        = 0;

/* 获取ftp传输Video的文件名 */
static void getFtpVideoName(char *filename)
{
	char buf[32];
	FILE *fp = NULL;

	memset(buf, 0, sizeof(buf));

	fp = fopen(FTPUPLOADINGFILEFLAG, "r");
	if (fp==NULL)
	{
	    strcpy(filename, "1.avi");
	    return;
	}
	fgets(buf, sizeof(buf), fp);
	fclose(fp);

	memcpy(filename, buf, strlen(buf));
}

static void time2string(char *stime)
{
    time_t aclock;

    time(&aclock);
    strftime(stime, 15, "%Y%m%d%H%M%S", localtime(&aclock)); 
}

/* 获取默认的图片名称(在消息队列为空的情况下调用) */
static void getFtpDefaultPicName(char *filename)
{
    char  file_index[128][32];       // 存放128个文件的文件名
	DIR  *dir = NULL;
    int  i=0, j=0;

	struct dirent *ptr;

    /* 找到最开始生成的那个图片 */
    dir = opendir("/ramdisk0/picture");
    if (dir == NULL)
    {
       perror("open dir error!");
       return;
    }
    /* 读取/ramdisk0/picture文件夹下所有文件名并放入file_index数组里面 */
    while(NULL != (ptr=readdir(dir)))
    {
       /* 只判断普通的.jpg文件 */
       if ((ptr->d_type==8) && strstr(ptr->d_name, ".jpg")) 
       {
           snprintf(file_index[i],sizeof(file_index[i]), "%s", ptr->d_name);
           if(++i >= 128)
           {
               break;
           }
       }
    }
    closedir(dir);

    if (i == 0)
    {
        DEBUG("no pic file! random no exist name!\n");
        snprintf(filename, sizeof(filename), "%s", "no_exist_file.jpg");
        return;
    }
    unsigned int tt = time(NULL);
    j = rand_r(&tt)%i; /* 产生0~i的随机数作为下标，传出 */

    strcpy(filename, file_index[j]); 

    INFO("i=%d, file_index[%d]=%s\n", i, j, file_index[j]);

    return;
}

static int FTP_IsDir(pFtpLibParam pftplib, char *diraddress)
{
	int ret = 0;

	if (pftplib==NULL  || diraddress == NULL)
	{
		ERROR("ncftpput:FTPIsDir param error!!!\n");
		return -1;
	}
	ret = FTPIsDir(&pftplib->ftpcip, diraddress);

//  INFO("FTPIsDir: %d\n", ret);
    
	return ret;
}

static int remote_mkdir(pFtpLibParam pftplib, const char *dir, int loff0, int loff1)
{
	int ret = 0;
	if (pftplib == NULL || dir == NULL)
	{
		ERROR("remote_mkdir:FTPMkDir param error!!!\n");
		ret = -1;
	}
	else
	{
		ret = FTPMkdir(&pftplib->ftpcip, dir, 1);
		if (ret)
		{
			ERROR("Fail to called FTPMkdir:%s\n", FTPStrError(ret));
		}
	}
	return ret;
}

static int FTPCreateDir(char *dir_addr, pFtpLibParam pftplib)
{
	char dest[80];
	int length;
	int result;
	int offset0,offset1,offset2;
	char *ptr;
	int ret;

	if (!dir_addr)
	{
		return 0;
	}

	strcpy(dest, dir_addr);

	if (FTP_IsDir(pftplib,  dest) >= 0)
	{
		return 1;
	}

//  INFO("[%s]--[%s]: dir:%s\n", __FILE__, __FUNCTION__, dest);
    
#if 1
	INFO("[%s]--[%s]: dir:%s\n", __FILE__, __FUNCTION__, dest);

	length = strlen(dest);
	offset0 = 0;
	offset1 = length & 1;
	if (dest[0] == '/')
	{
		offset0 = 1;
	}

	offset2 = 1;
	if (length >= 1)
	{
		if ( !offset1 )
		{
			goto LABEL_28;
		}
		offset1 = dest[1];
		if ( dest[1] == '/' )
		{
			++offset0;
		}
		offset2 = 2;
		if ( length >= 2 )
		{
LABEL_28:
			do
			{
				offset1 = offset2 + 1;
				if ( dest[offset2] == '/' )
				{
					++offset0;
				}
				if ( dest[offset2 + 1] == '/')
				{
					++offset0;
				}
				offset2 += 2;
			}
			while(offset1 + 1 <= length);
		}
	}
	if (offset0 == 1)
	{
		if (remote_mkdir(pftplib, dest, offset1, offset2) >= 0)
		{
			return 1;		
		}
		ERROR("FTPMkDir Fail!\n");
		result = 0;
	}
	else
	{
		ptr = strrchr(dest, '/');
		*ptr = 0;	 
		if (!FTPCreateDir(dest, pftplib))
		{
			return 0;
		}

		*ptr  = '/';
		ret = remote_mkdir(pftplib, dest, offset1, offset2);
		if (ret >= 0)
		{
			return 1;
		}
		ERROR("FTPMkDir Fail! Error!\n");

		result = 0;
	}
	
#else
    
    ret = FTPMkdir(&pftplib->ftpcip, dest, kRecursiveYes);
    if (ret)
    {
        ERROR("FTPMkdir: mkdir failed. ret = %d\n", ret);
    }

#endif
	return result;
}

int FTPClose(FtpLibParam *plibptr)
{
	int ret = 0;
    
    FTPCIPtr cip = &(plibptr->ftpcip);

	pthread_mutex_lock((pthread_mutex_t *)&ftp_mutex);

	ret = FTPCloseHost(cip);
	cip = NULL;
	pthread_mutex_unlock((pthread_mutex_t *)&ftp_mutex);
	
	return ret;
}

signed int  FTPGetFileSize(FtpLibParam *plibptr, char *file, signed long long *psize)
{
	int ret;

	*(signed long long *)psize = -1LL;
	if ( plibptr==NULL || file==NULL )
	{
		ERROR("ncftpput:FTPGetFileSize param error!!!\n");
		return -1;
	}

	ret = FTPFileSize(&plibptr->ftpcip, file, psize, kTypeBinary);
	if ( ret >= 0 )
	{
		return ret;
	}

    ERROR("Fail to called FTPFileSize:%s\n", FTPStrError(ret));

	return ret;
}

/* 主要功能:
 * 1. 初始化相关库；
 * 2. 填充相关结构体；
 * 3. 重启主机服务器；
 * 4. 创建必要的ftp远程文件夹
 */
int FTP_Init(void *handle)
{
	int ret = 0;
	FTP_param_handle * ftphandle = (FTP_param_handle *)handle;

	pthread_mutex_lock((pthread_mutex_t *)&ftp_mutex);
	if(handle == NULL)
	{
		pthread_mutex_unlock((pthread_mutex_t *)&ftp_mutex);
		return -1;
	}

	/* 关闭host */
    if (g_ftpstatus==1)
    {
        ret = FTPCloseHost(&ftphandle->libs_param.ftpcip);
        if (ret)
        {
            ERROR("Fail to called FTPCloseHost:%s\n", FTPStrError(ret));
            pthread_mutex_unlock((pthread_mutex_t *)&ftp_mutex);            
            return -1;
        }
    }

    /* 初始化lib库 */
	ret = FTPInitLibrary(&ftphandle->libs_param.ftplip);
	if(ret < 0)
	{
        ERROR("Fail to called FTPInitLibrary:%s\n", FTPStrError(ret));
		pthread_mutex_unlock((pthread_mutex_t *)&ftp_mutex);
		return -1;
	}
	
    /* 初始化默认值 */
	ret = FTPInitConnectionInfo(&ftphandle->libs_param.ftplip, &ftphandle->libs_param.ftpcip, 0x8000);
	if(ret < 0)
	{
        ERROR("Fail to called FTPInitConnectionInfo:%s\n", FTPStrError(ret));
		pthread_mutex_unlock((pthread_mutex_t *)&ftp_mutex);
		return -1;
	}
    ftphandle->libs_param.ftpcip.port = ftphandle->user_param.port;
    // ftphandle->libs_param.ftpcip.debugLog = stdout; // 自定义打印级别
    strncpy(ftphandle->libs_param.ftpcip.user, ftphandle->user_param.user, 128);
    strncpy(ftphandle->libs_param.ftpcip.pass, ftphandle->user_param.pass, 256);
    strncpy(ftphandle->libs_param.ftpcip.host, ftphandle->user_param.host, 128);

    /* 重新打开host */
	ret = FTPOpenHost(&ftphandle->libs_param.ftpcip);
	if(ret < 0)
	{
        ERROR("Fail to called FTPOpenHost:%s\n", FTPStrError(ret));
		pthread_mutex_unlock((pthread_mutex_t *)&ftp_mutex);
		return -1;
	}

	INFO("FTPOpen success\n");

#if 1
    /* 创建远程图片存放目录 */
	ret = FTPCreateDir(REMOTE_FTP_PIC_DIR, &global_ftp_param.libs_param);
//  ret = FTPMkdir(&global_ftp_param.libs_param.ftpcip, REMOTE_FTP_PIC_DIR, kRecursiveYes);
    if (ret == 0)
    {
		ERROR("FTPCreateDir error create %s, ret = %d\n", REMOTE_FTP_PIC_DIR, ret);
		pthread_mutex_unlock((pthread_mutex_t *)&ftp_mutex);
		return -1;
    }
    
    /* 创建远程录像存放目录 */
	ret = FTPCreateDir(REMOTE_FTP_VIDEO_DIR, &global_ftp_param.libs_param);
//  ret = FTPMkdir(&global_ftp_param.libs_param.ftpcip, REMOTE_FTP_VIDEO_DIR, kRecursiveYes);
    if (ret == 0)
    {
		ERROR("FTPCreateDir error create %s, ret = %d\n", REMOTE_FTP_PIC_DIR, ret);
		pthread_mutex_unlock((pthread_mutex_t *)&ftp_mutex);
		return -1;
    }

	INFO("FTPCreateDir success\n");
#endif

	pthread_mutex_unlock((pthread_mutex_t *)&ftp_mutex);

	return 0;
}


/* 对外接口，设置用户相关参数 */
int SetFtpParam(const pFTP_user_param pUsrInfo)
{
    LogLevel = 2; 
    
    /* 判断参数是否发生改变 */
    if (   (global_ftp_param.user_param.ftpenable   == pUsrInfo->ftpenable)
        && (global_ftp_param.user_param.port        == pUsrInfo->port)
        && (global_ftp_param.user_param.transt_type == pUsrInfo->transt_type)
        && (strcmp(global_ftp_param.user_param.host, pUsrInfo->host) == 0)
        && (strcmp(global_ftp_param.user_param.user, pUsrInfo->user) == 0)
        && (strcmp(global_ftp_param.user_param.pass, pUsrInfo->pass) == 0)
       )
    {
        DEBUG("ftp param is not change!\n");
        return 0;
    }

    pthread_mutex_lock((pthread_mutex_t *)&global_ftp_mutex);

    memset(&global_ftp_param, 0, sizeof(global_ftp_param));
    
    /* 参数填充(填充全局结构体) */
	global_ftp_param.user_param.ftpenable   = pUsrInfo->ftpenable;
	global_ftp_param.user_param.port        = pUsrInfo->port;
	global_ftp_param.user_param.transt_type = pUsrInfo->transt_type;
	strncpy(global_ftp_param.user_param.host, pUsrInfo->host, 128);    
	strncpy(global_ftp_param.user_param.user, pUsrInfo->user, 128);    
	strncpy(global_ftp_param.user_param.pass, pUsrInfo->pass, 256);

    /* 每次设置之后，表示没有初始化，则需要重新初始化 */
    bIsInited = 0;

    pthread_mutex_unlock((pthread_mutex_t *)&global_ftp_mutex);

    INFO("global_ftp_param.user_param.ftpenable = %d\n", global_ftp_param.user_param.ftpenable);
    INFO("global_ftp_param.user_param.port = %d\n", global_ftp_param.user_param.port);
    INFO("global_ftp_param.user_param.transt_type = %d\n", global_ftp_param.user_param.transt_type);
    INFO("global_ftp_param.user_param.host = %s\n", global_ftp_param.user_param.host);
    INFO("global_ftp_param.user_param.user = %s\n", global_ftp_param.user_param.user);
    INFO("global_ftp_param.user_param.pass = %s\n", global_ftp_param.user_param.pass);

	return 0;
}

/* 对外接口，获取用户相关参数 */
void GetFtpParam(pFTP_user_param pUsrInfo)
{
    memcpy(pUsrInfo, &global_ftp_param.user_param, sizeof(FTP_user_param));
}

/* 对外接口，获取当前FTP连接状态 */
int GetFtpConnStatus(void)
{
    return g_ftpstatus;
}

/* 对外接口，FTP发送线程 */
void *SendFileToFtpServerThread(void *lparam)
{
	int ret = 0;

	char buf[15];
	char pic_file_name[32];          // 当前上传图片名称
	char video_file_name[32];        // 当前上传录像名称
	
	char local_ftp_all_path[100];    // 本地欲上传文件全路径
	char remote_ftp_all_path[100];   // 远程文件全路径
	
    char remote_file_old_name[150];  // 远程文件的旧名字
    char remote_file_new_name[150];  // 远程文件的新名字

    static char last_transfer_pic_file[100];
    static char last_transfer_video_file[100];

    memset(last_transfer_pic_file, 0, sizeof(last_transfer_pic_file));
    memset(last_transfer_video_file, 0, sizeof(last_transfer_video_file));

    /* 当ftp使能的时候开始进行ftp上传 */
	while(1)
	{
	    if (global_ftp_param.user_param.ftpenable)
	    {
    		if (bIsInited==0)
    		{
                pthread_mutex_lock((pthread_mutex_t *)&global_ftp_mutex);
    			ret = FTP_Init(&global_ftp_param);
    			if(ret < 0)
    			{
    				bIsInited = 0;
    				g_ftpstatus = 0;
                    pthread_mutex_unlock((pthread_mutex_t *)&global_ftp_mutex);
    				sleep(1);
    				continue;
    			}
    			bIsInited = 1;
    			g_ftpstatus = 1;
                pthread_mutex_unlock((pthread_mutex_t *)&global_ftp_mutex);
    		}
    		else
    		{
    			if (global_ftp_param.user_param.transt_type == SEND_PICTURE)         /* 只发送抓图 */
    			{
    				/* 获取当前要上传哪一张图片 */
    				ret = GetInfoFromMsg("/etc/shadow", pic_file_name);
    				if (ret)
    				{
    				    INFO("Message is Empty! Get Picture Name From Msg Error, Picture Not Exist!\n");
    //                  getFtpDefaultPicName(pic_file_name);
                        strcpy(pic_file_name, "no_exist_pic.jpg");
    				}
    				/* 准备本地文件和远程文件的全路径 */
    				memset(buf, 0, sizeof(buf));
    				time2string(buf);  

    				memset(local_ftp_all_path, 0, sizeof(local_ftp_all_path));
    				memset(remote_ftp_all_path, 0, sizeof(remote_ftp_all_path));
                    memset(remote_file_old_name, 0, sizeof(remote_file_old_name));
                    memset(remote_file_new_name, 0, sizeof(remote_file_new_name));
    				snprintf(local_ftp_all_path, sizeof(local_ftp_all_path), "%s/%s", LOCAL_FTP_PIC_DIR, pic_file_name);
    				snprintf(remote_ftp_all_path, sizeof(remote_ftp_all_path), "%s", REMOTE_FTP_PIC_DIR);
    				snprintf(remote_file_old_name, sizeof(remote_file_old_name), "%s/%s", REMOTE_FTP_PIC_DIR, pic_file_name);
    				snprintf(remote_file_new_name, sizeof(remote_file_new_name), "%s/%s.jpg", REMOTE_FTP_PIC_DIR, buf);

    			}
    			else if (global_ftp_param.user_param.transt_type == SEND_RECORDS) /* 只发送录像 */
    			{ 
    				/* 获取当前要上传哪一个录像 */
    				getFtpVideoName(video_file_name);
                    
    				/* 准备本地文件和远程文件的全路径 */
    				memset(buf, 0, sizeof(buf));
                    time2string(buf);
    				
    				memset(local_ftp_all_path, 0, sizeof(local_ftp_all_path));
    				memset(remote_ftp_all_path, 0, sizeof(remote_ftp_all_path));				
                    memset(remote_file_old_name, 0, sizeof(remote_file_old_name));
                    memset(remote_file_new_name, 0, sizeof(remote_file_new_name));
    				snprintf(local_ftp_all_path, sizeof(local_ftp_all_path), "%s/%s", LOCAL_FTP_VIDEO_DIR, video_file_name);
    				snprintf(remote_ftp_all_path, sizeof(remote_ftp_all_path), "%s", REMOTE_FTP_VIDEO_DIR);
    				snprintf(remote_file_old_name, sizeof(remote_file_old_name), "%s/%s", REMOTE_FTP_VIDEO_DIR, video_file_name);
    				snprintf(remote_file_new_name, sizeof(remote_file_new_name), "%s/%s.avi", REMOTE_FTP_VIDEO_DIR, buf);
                    
    			}
    			else if (global_ftp_param.user_param.transt_type == SEND_ALL) /* 发送抓图和录像 */
    			{    
    				/* 获取当前要上传哪一张图片 */
    				ret = GetInfoFromMsg("/etc/shadow", pic_file_name);
    				if (ret)
    				{
    				    INFO("Message is Empty! Get Picture Name From Msg Error, Picture Not Exist!\n");
    //					getFtpDefaultPicName(pic_file_name);
                        strcpy(pic_file_name, "no_exist_pic.jpg");
    				}
    				
    				/* 获取当前要上传哪一个录像 */
    				getFtpVideoName(video_file_name);
                    
    			}
    			else if (global_ftp_param.user_param.transt_type == SEND_NONE)   /* 都不发送 */ 
    			{                         
                    sleep(1);
    				continue;
    			}
    			else
    			{
    				ERROR("transt_type is %d, out of [0~3]\n", global_ftp_param.user_param.transt_type);
    				continue;
    			}

    			/* 上传 */
    			if (global_ftp_param.user_param.transt_type == SEND_ALL)
    			{
                    memset(buf, 0, sizeof(buf));
                    time2string(buf);
                    
                    /* 准备本地文件和远程文件的全路径 */
    				memset(local_ftp_all_path, 0, sizeof(local_ftp_all_path));
    				memset(remote_ftp_all_path, 0, sizeof(remote_ftp_all_path));
                    memset(remote_file_old_name, 0, sizeof(remote_file_old_name));
                    memset(remote_file_new_name, 0, sizeof(remote_file_new_name));
                    snprintf(local_ftp_all_path, sizeof(local_ftp_all_path), "%s/%s", LOCAL_FTP_PIC_DIR, pic_file_name);
    				snprintf(remote_ftp_all_path, sizeof(remote_ftp_all_path), "%s", REMOTE_FTP_PIC_DIR);
    				snprintf(remote_file_old_name, sizeof(remote_file_old_name), "%s/%s", REMOTE_FTP_PIC_DIR, pic_file_name);
    				snprintf(remote_file_new_name, sizeof(remote_file_new_name), "%s/%s.jpg", REMOTE_FTP_PIC_DIR, buf);
                    /* 开始上传 */
                    if (strcmp(last_transfer_pic_file, local_ftp_all_path))
                    {
                        INFO("Local:%s, remote:%s\n", local_ftp_all_path, remote_ftp_all_path);
                        ret = FTPPutFiles3(&global_ftp_param.libs_param.ftpcip, local_ftp_all_path, remote_ftp_all_path, kRecursiveNo, kGlobYes, kTypeBinary, kAppendNo, NULL, NULL, kResumeNo, kDeleteNo, kNoFTPConfirmResumeUploadProc, 0);
                        if (ret)
                        {
                            /* 返回-158表示远程服务器关闭了连接 */
                            ERROR("Fail to called FTPPutFiles3:%s!\n", FTPStrError(ret));
                            if (ret == kErrRemoteHostClosedConnection)
                            {
                                INFO("need to reconnect to host!\n");
                                pthread_mutex_lock((pthread_mutex_t *)&global_ftp_mutex);
                                bIsInited = 0;
                                pthread_mutex_unlock((pthread_mutex_t *)&global_ftp_mutex);
                                continue;
                            }
                        }
                        else
                        {
                            /* 远程文件重命名 */
                            ret = FTPRename(&global_ftp_param.libs_param.ftpcip, remote_file_old_name, remote_file_new_name);
                            if (ret)
                            {
                                ERROR("Fail to called FTPRename:%s\n", FTPStrError(ret));
                            }
                        }
                        strncpy(last_transfer_pic_file, local_ftp_all_path, sizeof(local_ftp_all_path));
                    }

    				/* 准备本地文件和远程文件的全路径 */
    				memset(local_ftp_all_path, 0, sizeof(local_ftp_all_path));
    				memset(remote_ftp_all_path, 0, sizeof(remote_ftp_all_path));
                    memset(remote_file_old_name, 0, sizeof(remote_file_old_name));
                    memset(remote_file_new_name, 0, sizeof(remote_file_new_name));
    				snprintf(local_ftp_all_path, sizeof(local_ftp_all_path), "%s/%s", LOCAL_FTP_VIDEO_DIR, video_file_name);
    				snprintf(remote_ftp_all_path, sizeof(remote_ftp_all_path), "%s", REMOTE_FTP_VIDEO_DIR);
    				snprintf(remote_file_old_name, sizeof(remote_file_old_name), "%s/%s", REMOTE_FTP_VIDEO_DIR, video_file_name);
    				snprintf(remote_file_new_name, sizeof(remote_file_new_name), "%s/%s.avi", REMOTE_FTP_VIDEO_DIR, buf);

    				/* 开始上传 */
                    if (strcmp(last_transfer_video_file, local_ftp_all_path))
                    {
                        INFO("Local:%s, remote:%s\n", local_ftp_all_path, remote_ftp_all_path);
        				ret = FTPPutFiles3(&global_ftp_param.libs_param.ftpcip, local_ftp_all_path, remote_ftp_all_path, kRecursiveNo, kGlobYes, kTypeBinary, kAppendNo, NULL, NULL, kResumeNo, kDeleteNo, kNoFTPConfirmResumeUploadProc, 0);
        				if (ret)
        				{
                            /* 返回-158表示远程服务器关闭了连接 */
                            ERROR("Fail to called FTPPutFiles3:%s!\n", FTPStrError(ret));
                            if (ret == kErrRemoteHostClosedConnection)
                            {
                                INFO("need to reconnect to host!\n");
                                pthread_mutex_lock((pthread_mutex_t *)&global_ftp_mutex);
                                bIsInited = 0;
                                pthread_mutex_unlock((pthread_mutex_t *)&global_ftp_mutex);
                                continue;
                            }
        				}
        				else
        				{
        				    /* 远程文件重命名 */
        				    ret = FTPRename(&global_ftp_param.libs_param.ftpcip, remote_file_old_name, remote_file_new_name);
        				    if (ret)
        				    {
                                ERROR("Fail to called FTPRename:%s\n", FTPStrError(ret));
        				    }
                        }
                        strncpy(last_transfer_video_file, local_ftp_all_path, sizeof(last_transfer_video_file));
                    }
    			}
    			else
    			{
    				/* 开始上传 */
                    if (strcmp(last_transfer_pic_file, local_ftp_all_path))
                    {
                        INFO("Local:%s, remote:%s\n", local_ftp_all_path, remote_ftp_all_path);
        				ret = FTPPutFiles3(&global_ftp_param.libs_param.ftpcip, local_ftp_all_path, remote_ftp_all_path, kRecursiveNo, kGlobYes, kTypeBinary, kAppendNo, NULL, NULL, kResumeNo, kDeleteNo, kNoFTPConfirmResumeUploadProc, 0);
        				if (ret)
        				{
                            /* 返回-158表示远程服务器关闭了连接 */
                            ERROR("Fail to called FTPPutFiles3:%s!\n", FTPStrError(ret));
                            if (ret == kErrRemoteHostClosedConnection)
                            {
                                INFO("need to reconnect to host!\n");
                                pthread_mutex_lock((pthread_mutex_t *)&global_ftp_mutex);
                                bIsInited = 0;
                                pthread_mutex_unlock((pthread_mutex_t *)&global_ftp_mutex);
                                continue;
                            }
        				}
        				else
        				{
        				    /* 远程文件重命名 */
        				    ret = FTPRename(&global_ftp_param.libs_param.ftpcip, remote_file_old_name, remote_file_new_name);
        				    if (ret)
        				    {
                                ERROR("Fail to called FTPRename:%s\n", FTPStrError(ret));
        				    }				    
        				}
                        strncpy(last_transfer_pic_file, local_ftp_all_path, sizeof(local_ftp_all_path));
    				}
    			}
    		}
		}
		/* 一秒钟检测一次状态 */
		sleep(1);  
	}
	return NULL;
}

#else

#include "ncftp.h"
#include "ncftp_errno.h"
#include "ftp_process.h"

int main(int argc, char *argv[])
{
    int ret = 0;

    int LogLevel        = 2;
    
    FTPLibraryInfo li;
    FTPConnectionInfo ci;

    memset(&li, 0, sizeof(li));
    memset(&ci, 0, sizeof(ci));

    if (argc != 6)
    {
        INFO("Usage: %s usr pass host remote_dir put_file\n", argv[0]);
        return 0;
    }

    ret = FTPInitLibrary(&li);
    if (ret)
    {
        fprintf(stderr, "Fail to called FTPInitLibrary: %s.\n", FTPStrError(ret));
        return ret;
    }
    
    ret = FTPInitConnectionInfo(&li, &ci, kDefaultFTPBufSize);
    if (ret)
    {
        fprintf(stderr, "Fail to called FTPInitConnectionInfo: %s.\n", FTPStrError(ret));
        return ret;
    }

    ci.debugLog = stdout;
    strcpy(ci.user, argv[1]);
    strcpy(ci.pass, argv[2]);
    strcpy(ci.host, argv[3]);

    ret = FTPOpenHost(&ci);
    if (ret < 0) 
    {
        fprintf(stderr, "Fail to called FTPOpenHost: %s.\n", FTPStrError(ret));
        return ret;
    }

    ret = FTPMkdir(&ci, argv[4], kRecursiveYes);
    if (ret < 0) 
    {
        fprintf(stderr, "Fail to called FTPMkdir: %s.\n", FTPStrError(ret));
        return ret;
    }

    ret = FTPPutFiles3(&ci, argv[5], argv[4],
                    kRecursiveNo, kGlobYes, kTypeBinary, kAppendNo,
                    NULL, NULL, kResumeNo, kDeleteNo,
                    kNoFTPConfirmResumeUploadProc, 0);
    if (ret < 0) 
    {
        fprintf(stderr, "Fail to called FTPPutFiles3: %s.\n", FTPStrError(ret));
        return ret;
    }

    ret = FTPCloseHost(&ci);
    if (ret < 0) 
    {
        fprintf(stderr, "Fail to called FTPCloseHost: %s.\n", FTPStrError(ret));
        return ret;
    }

    return 0;
}

#endif



