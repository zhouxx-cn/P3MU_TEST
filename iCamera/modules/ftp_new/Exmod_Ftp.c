#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <glob.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <poll.h>  /* 避免 sys/poll.h 触发 SDK 重定向 #warning */
#include <sys/sendfile.h>
#include <netinet/tcp.h>

//#define TEST_MEM_INFO
#include "ftp.h"
#include "text.h"
#include "rc.h"
#include "Exmod_Ftp.h"

#define VERSION               "1.0.7"

/* error number */
#define DIRECTION_NONE        (0)
#define DIRECTION_OUT         (1)
#define DIRECTION_PUT         (1)
#define DIRECTION_IN          (2)
#define DIRECTION_GET         (2)
#define DIRECTION_FXP         (3)

#define COMMAND_NONE          (0)

#define STATREGEX1        "^ *([-a-z]+) +([0-9]+) +([a-z]+) +([a-z]+) +([0-9]+) (.{12}) (.*)$"

#define MAX_READ_SIZE         (524288)//(16384)

/* Increase this for a steadier but less accurate ETA and Speed readout */
#define SPEEDS_AVERAGE        (5)

/* Maximum number of concurrent connections */
#define MAX_CONNECTIONS       (3)

/* Define a couple of very handy macros */
#define CURRENT_CONNECTION    connection[atoi(config(variables, "CURRENT_CONNECTION"))]
#define LAST_CONNECTION       connection[atoi(config(variables, "LAST_CONNECTION"))]

static struct ftpconnection *connection[MAX_CONNECTIONS];
static struct variable_list *variables = NULL;

struct qitem 
{
    int  id;
    int  permissions;
    char direction;
    char command;
    char status;
    char remotedir[256];
    char localdir[256];
    char localfilename[256];
    char remotefilename[256];
    struct ftpconnection *remoteconnection;
    struct ftpconnection *localconnection;
    unsigned long size;
    struct qitem *next;
};

/* 封装sendfile，加上超时检测机制 */
__attribute__((unused)) static int MySendFile(int socket_fd, int file_fd, off_t file_size)
{
    off_t offset = 0;
    while (offset < file_size)
    {
        /* 一次发完，加上了poll超时检测机制 */
        int rv = sendfile(socket_fd, file_fd, &offset, file_size-offset);
        if (rv == -1)
        {
            if (errno == EAGAIN)
            {
                struct pollfd poll_fd;
                poll_fd.fd = socket_fd;
                /* 监测fd可写(POLLOUT), fd错误(POLLERR)，fd挂起(POLLHUP)，fd非法(POLLNVAL) */
                poll_fd.events = POLLOUT | POLLERR | POLLHUP | POLLNVAL;
                if (poll(&poll_fd, 1, -1) == -1)
                {
                    perror("poll");
                    break;
                }
                continue;
            }
            return rv;
        }
        offset += rv;
    }
    return (int)offset;
}

static int getconnected(struct ftpconnection *connection) 
{
    char *dir = NULL;
    if (!connection)
    {
        return 0;
    }
    if (!connection->hostname)
    {
        return 0;
    }
    if (connection->status > STATUS_WAIT_CONNECT)
    {
        return 1;
    }
    if (connection->remotedir[0] != '\0') 
    {
        dir = strdup(connection->remotedir);
    }
    if (ftp_connect(connection)) 
    {
        FREE(dir);
        return 0;
    }
    if (ftp_login(connection)) 
    {
        FREE(dir);
        ftp_disconnect(connection);
        return 0;
    }
    if (dir) 
    {
        if (!ftp_cwd(connection, dir))
        {
            FtpClientErr("Can't change directory to %s\n", dir);
        }
        FREE(dir);
    }
    connection->status = STATUS_IDLE;
    setconfig(&variables, "USERNAME", connection->username);
    return 1;
}

static int FTP_Connect(char *ip)
{
    if(NULL == ip)
    {
        FtpClientErr("arguments error!\n");
        return -1;
    }
    
    if (!parseurl(ip, CURRENT_CONNECTION)) 
    {
        FtpClientErr("Can't open %s\n", ip);
        return -1;
    }
    if (strlen(CURRENT_CONNECTION->password) == 0) 
    {
        if (config(variables, "email"))
        {
            strncpy(CURRENT_CONNECTION->password, config(variables, "email"), 32);
        }
        else
        {
            strncpy(CURRENT_CONNECTION->password, "user@quftp.com", 32);
        }
    }
    if (CURRENT_CONNECTION->remotedir[0] != '\0')
    {
        getconnected(CURRENT_CONNECTION);
    }
    setconfig(&variables, "HOSTNAME", CURRENT_CONNECTION->hostname);
    
    return 0;
}

static int FTP_Login(const char *UserName, const char *Password)
{
    if(NULL == UserName || NULL == Password)
    {
        FtpClientErr("arguments error!\n");
        return -1;
    }

    if (CURRENT_CONNECTION->status >= STATUS_IDLE) 
    {
        ftp_disconnect(CURRENT_CONNECTION);
    }
    strncpy(CURRENT_CONNECTION->username, UserName, 32);
    strncpy(CURRENT_CONNECTION->password, Password, 32);

    CURRENT_CONNECTION->status = STATUS_WAIT_CONNECT;

    return 0;
}

static int FTP_Close()
{
    int i = 0;
    ftp_disconnect(CURRENT_CONNECTION);
    for (i = 0; i < MAX_CONNECTIONS; i++) 
    {
        FREE(connection[i]);
    }
    while(variables)
    {
        setconfig(&variables, variables->key, NULL);
    }
    config(variables, NULL);
    return 0;
}

/*@ breif 进行ftp传输
 *@ param [IN] FilePath 全路径本地文件
 *@ param [IN] FileName 远程文件名
 *@ return -1 失败 0 成功
 */
int FTP_UploadFile(const char *FilePath, const char *FileName)
{
    if(NULL == FilePath || NULL == FileName)
    {
        FtpClientErr("arguments error!\n");
        return -1;
    }

    struct ftpconnection *remoteconnection = NULL;
    char line[MAX_LINE_SIZE] = {0};
    char uploadfile[128] = {0};
    unsigned long n = 0;
    int response = 0;

    if (!getconnected(CURRENT_CONNECTION)) 
    {
        FtpClientDbg("Not connected\n");
        return -1;
    }
    remoteconnection = CURRENT_CONNECTION;

    ftp_sendline(remoteconnection, "TYPE I\r\n");
    ftp_getrc(remoteconnection, NULL, 0, 0);

    ftp_open_data_pasv(remoteconnection, NULL, 0);

    snprintf(line, sizeof(line), "STOR %s\r\n", FileName);
    ftp_sendline(remoteconnection, line);
    response = ftp_getrc(remoteconnection, NULL, 0, 0);
    switch (response) 
    {
        case 125 :
        case 150 : 
             break;
        case 532 : FtpClientDbg("Server requires an account.. not supported yet\n");
               ftp_close_data(remoteconnection);
               return -response;
               break;
        case 450 : FtpClientDbg("Remote file is busy\n");
               ftp_close_data(remoteconnection);
               return -response;
               break;
        case 452 : FtpClientDbg("Insufficient storage space\n");
               ftp_close_data(remoteconnection);
               return -response;
               break;
        case 553 : FtpClientDbg("Permission denied to upload \"%s\"\n", FileName);
               ftp_close_data(remoteconnection);
               return -response;
               break;
        case 500 : FtpClientDbg("Server doesn't understand STOR command\n");
               ftp_close_data(remoteconnection);
               return -response;
               break;
        case 501 : FtpClientDbg("Syntax error in STOR command\n");
               ftp_close_data(remoteconnection);
               return -response;
               break;
        case 421 : FtpClientDbg("Server shutting down\n");
               ftp_close_data(remoteconnection);
               ftp_disconnect(remoteconnection);
               return -response;
               break;
        case 530 : FtpClientDbg("Server says not logged in!\n");
               ftp_close_data(remoteconnection);
               ftp_disconnect(remoteconnection);
               return -response;
               break;
        default  : FtpClientDbg("Unrecognised error %d\n", response);
               ftp_close_data(remoteconnection);
               ftp_disconnect(remoteconnection);
               break;
    }

    FtpClientDbg("Start Send Data!\n");

    /* 本地文件路径 */
    snprintf(uploadfile, sizeof(uploadfile), "%s", FilePath); 

    int ret = -1;
    int update_fd = -1;
    int filesize  = -1;
    struct stat statbuff;

    stat(uploadfile, &statbuff);
    filesize = statbuff.st_size;

    update_fd = open(uploadfile, O_RDONLY, S_IRWXU);
    if(update_fd < 0)
    {
        FtpClientSys("Fail to open %s\n", uploadfile);
        goto ERR_CLOSE_DATA;
    }

    /* 开启TCP_CORK */
    int on = 1;
    setsockopt(remoteconnection->datafd, SOL_TCP, TCP_CORK, &on, sizeof(on));

#if 0
    char read_buffer[MAX_READ_SIZE] = {0};

    /* 预计一个6.8MB的文件在4Mbps的速率下面需要15秒左右传输完毕 */
    while(1)
    {
        /* 每次读取MAX_READ_SIZE个字节文件就开始传输 */
        if ((read_size=read(update_fd, read_buffer, sizeof(read_buffer))) <= 0)
        {
            break;
        }
RetryWrite:
        /* 向远程写入实际读取到的数据 */
        n = ftp_write_data(remoteconnection, read_buffer, read_size);
        if(n == -1)
        {
            if(errno == 11)
            {
                ret11count++;
                FtpClientErr("n = %ld <= 0!\n", n);

                if(ret11count >= 200)
                {
                    FtpClientErr("n = %ld <= 0! ret11count = %d\n", n, ret11count);
                    ret = -1;
                    goto ERR_CLOSE_FD;
                }

                usleep(1000000);
                goto RetryWrite;
            }
            break;
        }
        usleep(1000000);
    }
#else

#if 0
    /* 分几次传完 */
    int i = 0;
    int send_times = filesize / MAX_READ_SIZE;
    int rest_size = filesize % MAX_READ_SIZE;

    for (i=0; i<send_times; i++)
    {
        sendfile(remoteconnection->datafd, update_fd, NULL, MAX_READ_SIZE);
        sleep(1);
    }
    if (rest_size)
    {
        sendfile(remoteconnection->datafd, update_fd, NULL, rest_size);
    }

#else

#if 0
    ret = MySendFile(remoteconnection->datafd, update_fd, NULL, filesize);
    if (ret < filesize)
    {
        FtpClientSys("sendfile data fail!\n");
        ret = -1;
        goto ERR_CLOSE_FD;
    }
#else
    /* 在MAX_FTP_WRITE_TIMEOUT时间之内如果还不能进行ftp传输，则超时退出 */
    ret = ftp_write_timeout(remoteconnection->datafd, MAX_FTP_WRITE_TIMEOUT);
    if(0 == ret)    // 检测到描述符可写
    {
        /* 采用一次传完 */
        if ((n = sendfile(remoteconnection->datafd, update_fd, NULL, filesize)) < 0)
        {
            FtpClientSys("sendfile data fail!\n");
            ret = -1;
            goto ERR_CLOSE_FD;
        }
    }
    else if(-1==ret && errno==ETIMEDOUT)  // 超时即指定事件内为检测到套接字可写
    {
        FtpClientErr("ftp sendfile timeout!\n"); // 进行超时处理
        ret = -1;
        goto ERR_CLOSE_FD;
    }
    else
    {
        FtpClientErr("ftp sendfile error!\n");  // 进行错误处理
        ret = -1;
        goto ERR_CLOSE_FD;
    }

#endif

#endif

#endif

ERR_CLOSE_FD:
    /* 恢复(关闭)TCP_CORK */
    on = 0;
    setsockopt(remoteconnection->datafd, SOL_TCP, TCP_CORK, &on, sizeof(on));
    /* 关闭文件句柄 */
    SAFE_CLOSE(update_fd);

ERR_CLOSE_DATA:
    ftp_close_data(remoteconnection);
    ftp_getrc(remoteconnection, NULL, 0, 0);

    FtpClientDbg("Send Data Over!\n");

    return ret;
}

int cmd_quit (char *command_line) 
{
    return -1;
}

//改变目录命令
//返回: 失败返回0 成功返回1
static int cmd_cd(char *command_line) 
{
    char *param = NULL;
    if (!getconnected(CURRENT_CONNECTION)) 
    {
        FtpClientDbg("cd: Not connected\n");
        return 0;
    }
    getnextword(command_line, GNW_START);
    param = getnextword(command_line, GNW_NEXT);
    if (!param || !*param) 
    {
        FtpClientDbg("Must specify a directory to change\n");
        return 0;
    }
    else
    {
        if (!ftp_cwd(CURRENT_CONNECTION, param))
        {
            return 0;
        }
    }
    return 1;
}

__attribute__((unused)) static char * cmd_pwd(void)
{
    if (!getconnected(CURRENT_CONNECTION))
    {
        FtpClientDbg("cd: Not connected\n");
        return 0;
    }

    return (ftp_pwd(CURRENT_CONNECTION));
}

static int cmd_mkdir(char *command_line) 
{
    char *param = NULL;
    if (!getconnected(CURRENT_CONNECTION)) 
    {
        FtpClientDbg("mkdir: Not connected\n");
        return 0;
    }
    getnextword(command_line, GNW_START);
    param = getnextword(command_line, GNW_NEXT);
    if (!param || !*param) 
    {
        FtpClientDbg("Must specify a directory to create\n");
        return 0;
    } 
    else 
    {
        if (!ftp_mkd(CURRENT_CONNECTION, param))
        {
            return 0;
        }
        else
        {
            FtpClientDbg("Created directory %s on %s in %s\n", param, CURRENT_CONNECTION->hostname, CURRENT_CONNECTION->remotedir);
        }
    }
    return 1;
}

__attribute__((unused)) static int cmd_rmdir(char *command_line)
{
    char *param = NULL;
    if (!getconnected(CURRENT_CONNECTION)) 
    {
        FtpClientDbg("mkdir: Not connected\n");
        return 0;
    }
    getnextword(command_line, GNW_START);
    param = getnextword(command_line, GNW_NEXT);
    if (!param || !*param) 
    {
        FtpClientDbg("Must specify a directory to create\n");
        return 0;
    } 
    else 
    {
        if (!ftp_rmd(CURRENT_CONNECTION, param)) 
        {
            return 0;
        }
        else
        {
            FtpClientDbg("Remove directory %s on %s in %s\n", param, CURRENT_CONNECTION->hostname, CURRENT_CONNECTION->remotedir);
        }
    }
    return 1;
}

/* 
 * 通过man strtok_r了解到其改变第一个参数的值。
 * 通过gdb调试，查看内存，发现tokens引用到就是之前tmp的内存地址，
 * 因此，我们释放内存到时候，需要释放之前分配到内存的指针，
 * 可是tmp在调用strtok_r的时候已经置为空了，需要额外到指针记录分配到地址。
 * @param_in dir_path 待创建目录名eg ./ftp/record/picture
 * @return 成功返回0
 */
int ftp_mkdir(const char *dir_path)
{
    if (NULL == dir_path)
    {
        FtpClientErr("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    char *tokens[32];//记号
    char *saveptr = NULL;
    char *tmp = strdup(dir_path); // 此处使用了strdup，分配了多余的内存，故在后面需要释放该内存
    char *p = tmp;
    char dir[64] = {0};
    char line[MAX_LINE_SIZE] = {0};

    int i = 0;
    int j = 0;

    /* 处理根目录和当前目录的问题 */
    if ('/' == tmp[0])
    {
        strncpy(dir, "/", 2);
    }
    if (!strncmp(tmp, "./", 2))
    {
        tmp += 2;
    }

    while((tokens[i]=strtok_r(tmp, "/", &saveptr)) != NULL) 
    {
        i ++;
        tmp = NULL;
    }

    for (j=0; j<i; j++)
    {
        /* 防止字符串长度超过限定而产生段错误 */
        if (sizeof(dir) > strlen(dir) + strlen(tokens[j]) + 1)
        {
            strncat(dir, tokens[j], strlen(tokens[j]));
        }

        snprintf(line, sizeof(line), "mkdir %s", dir);
        if (0 == cmd_mkdir(line))
        {
            FtpClientErr("***FTP exec %s fail!\n", line);
#ifndef TEST_MEM_INFO
            free(p);        // 释放strdup分配的内存
#else
			MemoryFree(p);
#endif
            p = NULL;
            return -1;
        }

        if (sizeof(dir) > strlen(dir) + 2)
        {
            strcat(dir, "/");
        }
    }
    
    /* 释放strdup分配的内存 */
#ifndef TEST_MEM_INFO
	free(p);		// 释放strdup分配的内存
#else
	MemoryFree(p);
#endif
    p = NULL;

    return 0;
}

/*@prief ftp_upload 通过FTP进行文件上传
 *@param[IN] ServerName 服务器名称
 *@param[IN] ServerPort 服务器端口号
 *@param[IN] FolderPath 远程文件夹名称./开头<64字符
 *@param[IN] UserName 服务器用户名
 *@param[IN] Password 服务器密码
 *@param[IN] FilePath 本地文件名(全路径)
 *@param[IN] FileName 远程文件名
 *@return 0 成功 -1失败
 */
int ftp_upload(const char *ServerName, int ServerPort, 
                   const char *FolderPath, 
    			   const char *UserName, 
                   const char *Password, 
    			   const char *FilePath, 
                   const char *FileName) 
{
    char tmpUrl[256] = {0};
    if(NULL == ServerName || NULL == FolderPath || NULL == UserName || NULL == Password || NULL == FilePath || NULL == FileName)
    {
        FtpClientErr("arguments error!\n");
        return -1;
    }

    if (access(FilePath, F_OK))
    {
        FtpClientErr("Local File %s not exist!\n", FilePath);
        return -1;
    }

    int i = 0;
    int ret = -1;
    char line[MAX_LINE_SIZE] = {0};

    setconfig(&variables, "PROGRAM_NAME", "quftp");                //设置配置变量
    setconfig(&variables, "VERSION", VERSION);
    setconfig(&variables, "CURRENT_CONNECTION", "0");
    setconfig(&variables, "LAST_CONNECTION", "1");

    for (i = 0; i < MAX_CONNECTIONS; i++) 
    {
        connection[i] = ftp_new();
        connection[i]->variables = variables;
        getcwd((char *)connection[i]->localdir, 256);
    }
    
    snprintf(tmpUrl, sizeof(tmpUrl), "%s:%d", ServerName, ServerPort);
    ret = FTP_Connect(tmpUrl);        //链接ftp服务器
    if(ret == -1)
    {
        FtpClientErr("ftp connect fail\n");
        FTP_Close();
        return -1;
    }
    
    ret = FTP_Login(UserName, Password);
    if(ret == -1)
    {
        FtpClientErr("ftp login fail\n");
        FTP_Close();
        return -1;
    }

    snprintf(line, sizeof(line), "cd %s", FolderPath);
    if(cmd_cd(line) == 0)
    {
    	//先返回到根目录，防止出现在创建多重目录的情况
   		snprintf(line, sizeof(line), "cd /");
		cmd_cd(line);
		
        //进入目录失败，则创建目录
        if (ftp_mkdir(FolderPath))
        {
            FTP_Close();
            cmd_quit("");
            return -1;
        }
        else
        {
            //创建成功，进入创建的目录
            snprintf(line, sizeof(line), "cd %s", FolderPath);
            if(cmd_cd(line) == 0)
            {
                FTP_Close();
                return -1;
            }
        }
    }

    ret = FTP_UploadFile(FilePath, FileName);     //上传文件
    if(ret == -1)
    {
        FtpClientErr("ftp %s to %s fail\n", FilePath, FileName);
        FTP_Close();
        return -1;
    }

    FTP_Close();

    return 0;
}
