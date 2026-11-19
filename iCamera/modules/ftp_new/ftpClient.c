
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <dirent.h>
#include <unistd.h>
#include <semaphore.h>
#include <net/if.h>  
#include <netinet/in.h>  
#include <net/if_arp.h>  
#include <sys/ioctl.h>
#include <sys/prctl.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */

#undef _GNU_SOURCE

#undef TEST_MEM_INFO

#include "ftpClient.h"
#include "Exmod_Ftp.h"
#include "list.h"
#include "global_macro.h"

#define FTP_ERR(format, args...)  do{printf("\033[1;31m[FTP][%s-%d]\033[m" format, __FILE__, __LINE__, ##args);}while(0)
#define FTP_DBG(format, args...)  do{printf("\033[1;32m[FTP][%s-%d]\033[m" format, __FILE__, __LINE__, ##args);}while(0)
#define FTP_SYS(format, args...)  do{printf("\033[1;32m[FTP][%s-%d](%s) \033[m" format, __FILE__, __LINE__, strerror(errno), ##args);}while(0)

#define FTP_SAFE_FREE(x) do{    \
    if (x)                      \
    {                           \
        free(x);                \
        x = NULL;               \
    }                           \
}while(0)

#define MAX_FTP_UPLOAD_FILE_NUM        (15)

typedef struct tag_ftp_param
{ 
    int  bFtpEnable;
    char hostname[DVR_MAX_HOST_LENTH];
    int  port;
    char username[DVR_MAX_USERNAME_LEN];
    char password[DVR_MAX_PASSWORD_LEN]; 
    char local_pic_dir[256];
    char local_video_dir[256];    
    char remote_pic_dir[256];
    char remote_video_dir[256];
}ftp_config, *pftp_config;

typedef struct tagFtpUpload
{
    char acTransferName[128];
    char acDestFileName[128];
} T_FtpUpload, *PT_FtpUpload;

static int g_ftp_conn_status = 0; // 1表示连接正常  0表示连接失败
static int g_bftp_has_init   = 0;   // ftp是否已经初始化完毕
static ftp_config g_ftp_cfg;
static eFtpTransType g_transfer_type = SEND_NONE;

static char g_ftp_trans_name[128] = {0};   // 上传的文件
static char g_ftp_dest_name[128]  = {0};   // 输出文件名
static char g_ftp_rmPath[256]     = {0};   // 远程路径
static int  g_ftp_mode = 0;                // ftp 模式0,表示主动模式，1表示被动模式
static char g_ftp_did[128]        = {0};   // tutk did

static pthread_mutex_t global_ftp_mutex;
static pthread_mutex_t g_ftp_transfer_mutex; // 同一时刻只可能发生一种传输
static pthread_mutex_t s_hFtpListLock  = PTHREAD_MUTEX_INITIALIZER;

static sem_t g_sem_start_ftp;

static list_t *g_ftp_list = NULL;

/* 获取系统的MAC地址 */
static int GetMacAddr(const char *if_name, char *address)
{
    struct ifreq ifr;

    int sock = -1; 
    int ret  = 0;

    unsigned char *ptr = NULL;

    if ((!if_name) || (!address))
    {
        return 1;
    }
    
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
    {
        return 2;
    }
    
    strncpy(ifr.ifr_name, if_name, 16);
    if (ioctl(sock, SIOCGIFHWADDR, &ifr) >= 0)
    {
        ptr = (unsigned char *) & ifr.ifr_ifru.ifru_hwaddr.sa_data[0];
        snprintf(address, 20, "%02X_%02X_%02X_%02X_%02X_%02X", *ptr, *(ptr + 1), *(ptr + 2), *(ptr + 3), *(ptr + 4), *(ptr + 5));    
    }
    else
    {
        ret = 3;
    }

    close(sock);

    return ret;
}

/*@prief AnalysisSendFile 分析发送文件类型是否符合avi或者jpg内容
 *@param[IN]  strFileName 源文件名称(限定文件类型只可以是JPG图片或者AVI录像)
 *@return 0符合 -1不符合或者参数错误
 */
static int AnalysisSendFile(const char *strFileName)
{
    if (NULL == strFileName)                //只支持jpg和avi
    {
        FTP_ERR("NULL POINTER EXCEPRION!\n");
        return -1;
    }

    char *temp = NULL;

    temp = strcasestr(strFileName, ".avi");
    if(temp != NULL)
    {
        return 0;
    }

    temp = strcasestr(strFileName, ".jpg");
    if(temp != NULL)
    {
        return 0;
    }

    return -1;
}

/* 删除指定目录的普通文件 */
__attribute__((unused)) static int UnlinkRegularFile(const char *dir_path)
{
    if (!dir_path)
    {
        FTP_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    DIR  *dir = NULL;
    struct dirent *ptr = NULL;
    char file_name[128] = {0};

    dir = opendir(dir_path);
    if (dir == NULL)
    {
        FTP_SYS("open dir %s fail\n", dir_path);
        return -1;
    }

    /* 读取pdir文件夹下所有文件名放入file_prefix_num数组里面 */
    while(NULL != (ptr=readdir(dir)))
    {
        /* 只判断普通文件 */
        if (ptr->d_type == 8)
        {
            snprintf(file_name, sizeof(file_name), "%s/%s", dir_path, ptr->d_name);
            SAFE_UNLINK(file_name);
        }
    }
    closedir(dir);
    dir = NULL;

    return 0;
}


/* 检测当前传输的文件和设置的传输类型是否匹配 */
static int IsMatchWithTransType(const char *filename, int *p_curr_type)
{
    if (NULL==filename || NULL==p_curr_type)
    {
        FTP_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    *p_curr_type = 0;

    switch(g_transfer_type)
    {
        case SEND_NONE: return 0;
        
        case SEND_ALL:
        {
            if (!strcasestr(filename, "avi") && !strcasestr(filename, "jpg"))
            {
            
                FTP_ERR("ftp send file %s invail must be avi or jpg!\n", filename);
                return -1;
            }
            
            if (NULL == strcasestr(filename, "avi"))
            {
                *p_curr_type = 1;
            }
            else
            {
                *p_curr_type = 2;                
            }
            break;
        }
        
        case SEND_PICTURE:
        {
            if (NULL == strcasestr(filename, "jpg"))
            {
                FTP_ERR("ftp send file %s can't match with jpg!\n", filename);
                return -1;
            }
            *p_curr_type = 1;
            break;
        }
        
        case SEND_RECORDS:
        {
            if (NULL == strcasestr(filename, "avi"))
            {
                FTP_ERR("ftp send file %s can't match with avi!\n", filename);
                return -1;
            }
            *p_curr_type = 2;
            break;
        }

        default:return -1;
    }

    return 0;
}


/*@ brief 将用户保存的远程目录 转化为标准目录./开头,否则会异常
 *@ param_in pUsrDir 用户设置的远程目录
 *@ return 0成功 其他为错误码
 *@ note
 */
static int SetFtpRemoteDir(const char *pUsrDir)
{
    if(NULL == pUsrDir)
    {
        FTP_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if(strlen(pUsrDir) >= 200)
    {
        FTP_ERR("remote dir %s is too long\n", pUsrDir);
        return -2;
    }

    time_t now = 0;
    struct tm curr_tm = {0,0};
    char acPrefix[128] = {0};
    char *ptr = 0;

    time(&now);   
    localtime_r(&now, &curr_tm);

    //如果P2P did值存在，则将did值保存到文件名
    if (strlen(g_ftp_did))
    {
        ptr = strchr(g_ftp_did, ',');
        if (ptr)
        {
            *ptr = '\0';
        }
        snprintf(acPrefix, sizeof(acPrefix), "%s", g_ftp_did);
    }
    else
    {
        //did不存在时，则用mac地址代替
        //目前用有线的MAC地址，后续可能会考虑wlan
        GetMacAddr(IFNAME, acPrefix);
    }

    //图片目录
    snprintf(g_ftp_cfg.remote_pic_dir, sizeof(g_ftp_cfg.remote_pic_dir), "./%.80s/%.80s/%04d-%02d-%02d/picture",
             pUsrDir, acPrefix,
             (curr_tm.tm_year+1900), (curr_tm.tm_mon+1), curr_tm.tm_mday);

    //视频目录
    snprintf(g_ftp_cfg.remote_video_dir, sizeof(g_ftp_cfg.remote_video_dir), "./%.80s/%.80s/%04d-%02d-%02d/video",
             pUsrDir, acPrefix,
             (curr_tm.tm_year+1900), (curr_tm.tm_mon+1), curr_tm.tm_mday);

    return 0;
}

/* 发送文件到ftp服务器上 */
static int SendTypeFileToFtpServer(void)
{
    int current_type = 0;

    /* 需要对录像类型进行匹配判断 */
    if (IsMatchWithTransType(g_ftp_trans_name, &current_type))
    {
        FTP_ERR("current transfer type is %d, transfer file is %s not march!\n", g_transfer_type, g_ftp_trans_name);
        return -1;
    }

    int ret = 0;
    char local_file_abs[512] = {0};

    ret = SetFtpRemoteDir(g_ftp_rmPath);//设置文件夹名字
    if (ret)
    {
        FTP_ERR("Fail to SetFtpRemoteDir %s\n", g_ftp_rmPath);
        return -1;
    }

    /* 需要上传到ftp的文件名称 */
    FTP_DBG("Ftp transfer local file is %s\n", g_ftp_trans_name);

    switch(g_transfer_type)
    {
        case SEND_PICTURE:
        {
            if (1 != current_type)
            {
                FTP_ERR("transfer type is not march picture!\n");
                return -1;
            }

            /* 解析为远程文件名称 */
            ret = AnalysisSendFile(g_ftp_trans_name);
            if (ret)
            {
                FTP_ERR("Called AnalysisSendFile %s Fail!\n", g_ftp_trans_name);
            }
            else
            {
                /* 开始上传图片,生成绝对路径 */
                snprintf(local_file_abs, sizeof(local_file_abs), "%s/%s", g_ftp_cfg.local_pic_dir, g_ftp_trans_name);
                pthread_mutex_lock(&g_ftp_transfer_mutex);
                ret = ftp_upload(g_ftp_cfg.hostname, 
                                 g_ftp_cfg.port, 
                                 g_ftp_cfg.remote_pic_dir,
                                 g_ftp_cfg.username, 
                                 g_ftp_cfg.password, 
                                 local_file_abs, 
                                 g_ftp_dest_name);
                pthread_mutex_unlock(&g_ftp_transfer_mutex);
            }
        }    
        break;

        case SEND_RECORDS:
        {   
            if (2 != current_type)
            {
                FTP_ERR("transfer type is not march video!\n");
                return -1;
            }

            /* 解析为远程文件名称 */
            ret = AnalysisSendFile(g_ftp_trans_name);
            if (ret)
            {
                FTP_ERR("Called AnalysisSendFile %s Fail!\n", g_ftp_trans_name);
            }
            else                    
            {
                /* 开始上传录像 */                
                snprintf(local_file_abs, sizeof(local_file_abs), "%s/%s", g_ftp_cfg.local_video_dir, g_ftp_trans_name);

                pthread_mutex_lock(&g_ftp_transfer_mutex);
                ret = ftp_upload(g_ftp_cfg.hostname, 
                                 g_ftp_cfg.port, 
                                 g_ftp_cfg.remote_video_dir,
                                 g_ftp_cfg.username, 
                                 g_ftp_cfg.password, 
                                 local_file_abs, 
                                 g_ftp_dest_name);
                pthread_mutex_unlock(&g_ftp_transfer_mutex);
            }
        }    
        break;

        case SEND_ALL:
        {
            if (1 == current_type)
            {
                /* 解析为远程文件名称 */
                ret = AnalysisSendFile(g_ftp_trans_name);
                if (ret)
                {
                    FTP_ERR("Called AnalysisSendFile %s Fail!\n", g_ftp_trans_name);
                }
                else
                {
                    /* 开始上传图片,生成绝对路径 */
                    snprintf(local_file_abs, sizeof(local_file_abs), "%s/%s", g_ftp_cfg.local_pic_dir, g_ftp_trans_name);
                    pthread_mutex_lock(&g_ftp_transfer_mutex);
                    ret = ftp_upload(g_ftp_cfg.hostname, 
                                     g_ftp_cfg.port, 
                                     g_ftp_cfg.remote_pic_dir,
                                     g_ftp_cfg.username, 
                                     g_ftp_cfg.password, 
                                     local_file_abs, 
                                     g_ftp_dest_name);
                    pthread_mutex_unlock(&g_ftp_transfer_mutex);
                }
            }
            else
            {
                /* 解析为远程文件名称 */
                ret = AnalysisSendFile(g_ftp_trans_name);
                if (ret)
                {
                    FTP_ERR("Called AnalysisSendFile %s Fail!\n", g_ftp_trans_name);
                }
                else                    
                {
                    /* 开始上传录像 */                
                    snprintf(local_file_abs, sizeof(local_file_abs), "%s/%s", g_ftp_cfg.local_video_dir, g_ftp_trans_name);

                    pthread_mutex_lock(&g_ftp_transfer_mutex);
                    ret = ftp_upload(g_ftp_cfg.hostname, 
                                     g_ftp_cfg.port, 
                                     g_ftp_cfg.remote_video_dir,
                                     g_ftp_cfg.username, 
                                     g_ftp_cfg.password, 
                                     local_file_abs, 
                                     g_ftp_dest_name);
                    pthread_mutex_unlock(&g_ftp_transfer_mutex);
                }
            }
        }
        break;

        default: 
        {
            FTP_ERR("Not support ftp transfer type is %d\n", g_transfer_type);
            ret = -1;
        }
        break;
    }

    if (0 == ret)
    {
        FTP_DBG("Ftp transfer file %s upload succeed!\n", g_ftp_trans_name);
    }
    else
    {
        FTP_DBG("Ftp transfer file %s upload fail!\n", g_ftp_trans_name);
    }
    
    g_ftp_conn_status = (ret==0) ? 1 : 0;
    return ret;
}

/*@prief SetFtpTransferName 设置传输文件的文件名到传输列表里面，并发送传输命令
 *@param[IN]  pfilename 源文件名称(限定文件类型只可以是JPG图片或者AVI录像)
 *@param[IN]  pdestfile 目标文件名(限定文件类型只可以是JPG图片或者AVI录像)
 *@return 0 成功 -1失败
 *@note 当前操作不能被阻塞
 */
extern int SetFtpTransferName(const unsigned char *pfilename, const unsigned char *pdestfile)
{
    if(NULL==pfilename || NULL==pdestfile)
    {
        FTP_ERR("NULL POINTER EXECPTION!\n");
        return -1;
    }

    FTP_DBG("into SetFtpTransferName: %s %s\n", pfilename, pdestfile);

    T_FtpUpload *pstFtpUpload = NULL;
    char pic_file_path[512] = {0};
	char tmp[128] = {0};
    char *p = NULL;

    strncpy(tmp, (const char *)pdestfile, sizeof(tmp));

    /* 针对抓图，去掉字符串最后面的短横以及后面的标号，如T_201807281512_01.jpg需要改为T_201807281512.jpg */
    if ((NULL!=strcasestr(tmp, ".jpg")) && ((p=strrchr(tmp, '_'))!=NULL))
    {
        *p = '\0';
        strncat(tmp, ".jpg", 5);
    }

    pthread_mutex_lock(&s_hFtpListLock);
    if (g_ftp_list && g_bftp_has_init)
    {
        if (g_ftp_list->len >= MAX_FTP_UPLOAD_FILE_NUM)
        {
            FTP_DBG("ftp transfer list has too many transfer files\n");

            //将已经拷贝到/tmp/picture/pic_for_ftp的文件删除掉
            snprintf(pic_file_path, sizeof(pic_file_path), "%s/%s", g_ftp_cfg.local_pic_dir, pfilename);
            SAFE_UNLINK(pic_file_path);
            pthread_mutex_unlock(&s_hFtpListLock);
            return -1;
        }

#ifndef TEST_MEM_INFO
		pstFtpUpload = (T_FtpUpload *)malloc(sizeof(T_FtpUpload));
#else
		pstFtpUpload = (T_FtpUpload *)MemoryMalloc(sizeof(T_FtpUpload), "ftlClient", __FILE__, __FUNCTION__, __LINE__);
#endif
        if (!pstFtpUpload)
        {
            FTP_SYS("ftp node malloc fail! need to delete %s!\n", pfilename);
            //将已经拷贝到/tmp/picture/pic_for_ftp的文件删除掉
            snprintf(pic_file_path, sizeof(pic_file_path), "%s/%s", g_ftp_cfg.local_pic_dir, pfilename);
            SAFE_UNLINK(pic_file_path);
            pthread_mutex_unlock(&s_hFtpListLock);
            return -1;
        }

        /* 不用管传输类型，只需要将文件名放到传输链表里面，然后发送即可 */
        strncpy(pstFtpUpload->acTransferName, (const char *)pfilename, sizeof(pstFtpUpload->acTransferName));
        strncpy(pstFtpUpload->acDestFileName, tmp, sizeof(pstFtpUpload->acDestFileName));

        list_rpush(g_ftp_list, list_node_new(pstFtpUpload));

        /* 通知开始FTP传输 */
        sem_post(&g_sem_start_ftp);
    }
    pthread_mutex_unlock(&s_hFtpListLock);

    FTP_DBG("out of SetFtpTransferName: %s %s\n", pfilename, tmp);

    return 0;
}

/* 发送测试文件 */
extern int SendFtpTestFile(void)
{
    if(0 == g_ftp_cfg.bFtpEnable)
    {
        FTP_ERR("ftp not enable\n");
        return -1;
    }

    int ret = 0;

    pthread_mutex_lock(&g_ftp_transfer_mutex);            //通过ftp上传文件
    ret = ftp_upload(g_ftp_cfg.hostname, 
                     g_ftp_cfg.port, 
                     g_ftp_cfg.remote_pic_dir, 
                     g_ftp_cfg.username, 
                     g_ftp_cfg.password, 
                     "/mnt/mtd/config/config.ini", 
                     "ftp_test_file");
    pthread_mutex_unlock(&g_ftp_transfer_mutex);

    if (0 == ret)
    {
        FTP_DBG("Ftp send test_file upload succeed!\n");
    }
    else
    {
        FTP_DBG("Ftp send test_file upload fail!\n");
    }

    g_ftp_conn_status = (ret==0) ? 1 : 0;

    return ret;
}

/* 设置ftp的参数到全局变量里面 */
extern int SetFtpParam(const pFTP_user_param pUsrInfo, char* pszDid)//将用户输入的信息放到全局变量中
{
    if (NULL == pUsrInfo)
    {
        FTP_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }
    
    pthread_mutex_lock(&global_ftp_mutex);

    memset(g_ftp_did, 0, sizeof(g_ftp_did));
    strncpy(g_ftp_did, pszDid, sizeof(g_ftp_did));      
    g_ftp_cfg.port = pUsrInfo->port;
    g_transfer_type = pUsrInfo->transt_type;
    g_ftp_cfg.bFtpEnable = pUsrInfo->ftpenable;
    g_ftp_mode = pUsrInfo->nFtpMode;
    strncpy(g_ftp_cfg.hostname, pUsrInfo->host, DVR_MAX_HOST_LENTH);    
    strncpy(g_ftp_cfg.username, pUsrInfo->user, DVR_MAX_USERNAME_LEN);    
    strncpy(g_ftp_cfg.password, pUsrInfo->pass, DVR_MAX_PASSWORD_LEN);
    strncpy(g_ftp_rmPath,pUsrInfo->rmPath,256);
    FTP_DBG("g_ftp_cfg.port       = %d\n", g_ftp_cfg.port);
    FTP_DBG("g_transfer_type      = %d\n", g_transfer_type);
    FTP_DBG("g_ftp_cfg.bFtpEnable = %d\n", g_ftp_cfg.bFtpEnable);
    FTP_DBG("g_ftp_cfg.hostname   = %s\n", g_ftp_cfg.hostname);
    FTP_DBG("g_ftp_cfg.username   = %s\n", g_ftp_cfg.username);
    FTP_DBG("g_ftp_cfg.password   = %s\n", g_ftp_cfg.password);
    FTP_DBG("g_ftp_conn_status    = %d\n", g_ftp_conn_status);
    FTP_DBG("g_ftp_mode           = %d\n", g_ftp_mode);
    FTP_DBG("g_ftp_rmPath         = %s\n", g_ftp_rmPath);    
    FTP_DBG("g_ftp_did            = %s\n", g_ftp_did);
    SetFtpRemoteDir(g_ftp_rmPath);
    strncpy(g_ftp_cfg.local_pic_dir, LOCAL_FTP_PIC_DIR, 256);
    strncpy(g_ftp_cfg.local_video_dir, LOCAL_FTP_VIDEO_DIR, 256);
    pthread_mutex_unlock(&global_ftp_mutex);

    g_bftp_has_init = 1;

    return 0;
}

/*@prief SendFileToFtpServer 发送指定的文件到远程文件夹
 *@param[IN] RemoteDir 远程端文件夹
 *@param[IN] RemoteFileName 远程端文件名
 *@param[IN] LocalFile 本地文件(全路径)
 *@return 0:成功 1:失败 -1:错误(参数错误或者ftp未使能)
 */
extern int SendFileToFtpServer(const char* RemoteDir, const char *RemoteFileName, const char *LocalFile)
{                                    //远程文件夹       远程文件           本地文件
    if (!RemoteDir || !RemoteFileName || !LocalFile)
    {
        FTP_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    /* FTP参数需要先初始化 */
    if (0 == g_bftp_has_init)
    {
        FTP_ERR("ftp not init!\n");
        return -1;
    }

    /* FTP功能需要使能 */
    if (0 == g_ftp_cfg.bFtpEnable)
    {
        FTP_DBG("ftp is disabled!\n");
        return -1;
    }

    int ret = 0;
    int current_type = 0;

    /* 查看传输的文件需要符合设置的FTP传输类型 */
    if (0 != IsMatchWithTransType(LocalFile, &current_type))    //文件是不是指定的文件类型
    {
        FTP_ERR("local file %s can't match with setting type %d, current type %d!\n", LocalFile, g_transfer_type, current_type);
        return -1;
    }
    
    pthread_mutex_lock(&g_ftp_transfer_mutex);
    ret = ftp_upload(g_ftp_cfg.hostname, 
                     g_ftp_cfg.port, 
                     RemoteDir, g_ftp_cfg.username, 
                     g_ftp_cfg.password, 
                     LocalFile, 
                     RemoteFileName);
    pthread_mutex_unlock(&g_ftp_transfer_mutex);
    
    g_ftp_conn_status = (ret==0) ? 1 : 0;
    
    return ret;
}

/* 获取ftp当前的主机连接状态 */
extern int GetFtpConnStatus(void)
{    
    return g_ftp_conn_status;
}

/* 获取ftp是否使能 */
extern int GetFtpEnable(void)
{
    return g_ftp_cfg.bFtpEnable;
}

/* 对外接口，FTP发送线程 */
extern void *SendFileToFtpServerThread(void *lparam)
{
    PRT_TID();

    pthread_detach(pthread_self());

    list_node_t *ftp_node = NULL;
    PT_FtpUpload pstFtp = NULL;
    char local_pic_file[512] = {0};

    //init ftp list
    pthread_mutex_lock(&s_hFtpListLock);
    if (!g_ftp_list)
    {
         g_ftp_list = list_new();
         if (!g_ftp_list)
         {
            FTP_ERR("Fail to init ftp list!\n");
            pthread_mutex_unlock(&s_hFtpListLock);
            return NULL;
        }
        sem_init(&g_sem_start_ftp, 0, 0);
    }

    //g_ftp_list->free = ftp_free_node;
    pthread_mutex_unlock(&s_hFtpListLock);

    if (0 == g_bftp_has_init)
    {
        FTP_ERR("ftp not init!\n");
        FTP_SAFE_FREE(g_ftp_list);
        return NULL;
    }

    while(1)
    {  
        /* 如果链表里面的文件都传输完毕了，则需要等待数据到来 */
        if (g_ftp_list->len == 0)
        {
            sem_wait(&g_sem_start_ftp);
        }

        /* 如果链表里面的文件还有很多，则需要一个一个上传到FTP服务器 */
        while (g_ftp_list->len > 0)
        {
            pthread_mutex_lock(&s_hFtpListLock);
            ftp_node = list_lpop(g_ftp_list);
            pthread_mutex_unlock(&s_hFtpListLock);     

            if (ftp_node)
            {
                pstFtp = (PT_FtpUpload)ftp_node->val;
                memset(g_ftp_trans_name, 0, sizeof(g_ftp_trans_name));
                snprintf(g_ftp_trans_name, sizeof(g_ftp_trans_name), "%s", pstFtp->acTransferName);
                snprintf(g_ftp_dest_name,  sizeof(g_ftp_dest_name), "%s", pstFtp->acDestFileName);

                if (g_ftp_cfg.bFtpEnable)
                {
                    pthread_mutex_lock(&global_ftp_mutex);
                    FTP_DBG("SendTypeFileToFtpServer start!\n");
                    SendTypeFileToFtpServer();            //实现传输图片和录像到服务器
                    FTP_DBG("SendTypeFileToFtpServer over!\n");
                    pthread_mutex_unlock(&global_ftp_mutex);
                }

                /* 不管有没有发送成功，都需要将原始图片及时删除 */
                snprintf(local_pic_file, sizeof(local_pic_file), "%s/%s", g_ftp_cfg.local_pic_dir, g_ftp_trans_name);
                SAFE_UNLINK(local_pic_file);

                /* 记得释放干净，避免出现内存泄漏 */
                FTP_SAFE_FREE(ftp_node->val);
                FTP_SAFE_FREE(ftp_node);
            }
        }

        //usleep(300 * 1000);
        sleep(1);
    }
    return NULL;
}
