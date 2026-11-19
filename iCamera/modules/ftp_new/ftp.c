/* FTP Functions
   $Revision: 1.5 $
   $Date: 2003/03/10 22:31:26 $
   vim: sw=4 ts=4
*/

#define _XOPEN_SOURCE 500
//#define TEST_MEM_INFO
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <linux/if_ether.h>
#include <net/if.h>
#include <errno.h>


#include <stdlib.h>
#include <features.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <malloc.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <arpa/inet.h>
#include <fnmatch.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "text.h"
#include "ftp.h"

/*
 * Global Variables
 */

/* STAT or LIST Formats
 * %P    UNIX style permissions
 * %L    Number of hard links
 * %O    Owner
 * %G    Group
 * %S    Size
 * %N    File Name
 *
 * %Y    Year
 * %M    Month name
 * %n    Month number
 * %D    Day of month
 * %h    Hour
 * %m    Minute
 * %s    Second
 */

char *stat_formats[] = {
    /* drwxr-xr-x   2 ftp      ftp          1024 Jan 23 21:37 incoming */
    "%P %L %O %G %S %M %D %h:%m %N",

    /* d--x--x--x   2 ftp      ftp          1024 Oct 17  1998 bin */
    "%P %L %O %G %S %M %D %Y %N"
};

/*@ breif ftp_write_timeout 写超时检测函数，不含写操作
 *@ param [IN] fd 文件描述符
 *@ param [IN] wait_seconds 等待超时秒数，如果为0则表示不检测超时
 *@ return 成功(未超时)返回0，失败返回-1，超时返回-1并且errno=ETIMEDOUT
 */
int ftp_write_timeout(int fd, unsigned int wait_seconds)
{
    if (fd <= 0)
    {
        return -1;
    }

    /* 默认为0，当wait_seconds==0时，不检测直接返回0 */
    int ret = 0;

    /* 如果需要检测超时 */
    if(wait_seconds > 0)
    {
        fd_set write_fdset;      // 描述符集合
        struct timeval timeout;  // 超时时间

        FD_ZERO(&write_fdset);
        FD_SET(fd, &write_fdset);

        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;

        do
        {
           /* select会阻塞直到检测到事件或则超时，如果超时，select会返回0， 如果检测到事件会返回1，
            * 如果异常会返回-1，如果是由于信号中断引起的异常errno==EINTR 
            */
            ret = select(fd+1, NULL, &write_fdset, NULL, &timeout); 
        }while(ret<0 && errno == EINTR);

        /* 如果是有信号引起的异常则继续阻塞select，直到检测到事件或则超时 */
        if(0 == ret)    // select超时退出
        {
            ret = -1; 
            errno = ETIMEDOUT;
        }
        else if(1 == ret) // select检测到可写事件 
        {
            ret = 0;
        }
    }
    return ret;
}

/*@ breif ftp_read_timeout 读超时检测函数，不含读操作
 *@ param [IN] fd 文件描述符
 *@ param [IN] wait_seconds 等待超时秒数，如果为0则表示不检测超时
 *@ return 成功(未超时)返回0，失败返回-1，超时返回-1并且errno=ETIMEDOUT
 */
int ftp_read_timeout(int fd, unsigned int wait_seconds)
{
    if (fd <= 0)
    {
        return -1;
    }

    /* 默认为0，当wait_seconds==0时，不检测直接返回0 */
    int ret = 0;

    /* 如果当前需要检测超时 */
    if(wait_seconds > 0)
    {
        fd_set read_fdset;       // 描述符集合
        struct timeval timeout;  // 超时时间

        FD_ZERO(&read_fdset);
        FD_SET(fd, &read_fdset);

        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;

        /* select会阻塞直到检测到事件或则超时，如果超时，select会返回0，如果检测
         * 到事件会返回1，如果异常会返回-1，如果是由于信号中断引起的异常errno==EINTR
         * 如果是有信号引起的异常则继续阻塞select，直到检测到事件或则超时
         */
        do
        {
            ret = select(fd+1, &read_fdset, NULL, NULL, &timeout);
        }while(ret < 0 && errno == EINTR);

        if(0 == ret)      // select超时退出
        {
            ret = -1;
            errno = ETIMEDOUT;
        }
        else if(1 == ret) // select检测到可读事件
        {
            ret = 0;
        }
    }
    return ret;
}

/*@ breif ftp_accept_timeout 带超时的accept函数
 *@ param [IN] fd 文件描述符
 *@ param [IN] addr 输出参数，accept返回的对等方的地址结构
 *@ param [IN] wait_seconds：等待超时秒数，如果为0则表示正常模式
 *@ return 成功（未超时）返回已连接的套接字，失败返回-1，超时返回-1并且errno=ETIMEDOUT
 */
 int ftp_accept_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds)
 {
     if (fd <= 0)
     {
         return -1;
     }

     /* 默认为0，当wait_seconds==0时，不检测直接返回0 */
     int ret = 0;
     socklen_t addrlen = sizeof(struct sockaddr_in);

     /* 如果当前需要检测超时 */
     if(wait_seconds > 0)
     {
         fd_set accept_fdset;     // 描述符集合
         struct timeval timeout;  // 超时时间

         FD_ZERO(&accept_fdset);
         FD_SET(fd, &accept_fdset);

         timeout.tv_sec = wait_seconds;
         timeout.tv_usec = 0;
         /* select会阻塞直到检测到事件或则超时，如果超时，select会返回0，如果检测到事件
          * 会返回1，如果异常会返回-1，如果是由于信号中断引起的异常errno==EINTR
          * 如果是有信号引起的异常则继续阻塞select，直到检测到事件或则超时
          */
         do
         {
             ret = select(fd+1, &accept_fdset, NULL, NULL, &timeout);
         }while(ret < 0 && errno == EINTR);

         if(-1 == ret) //失败
         {
             return -1;
         }
         if(0 == ret) //select超时退出
         {
             errno= ETIMEDOUT;
             return -1;
         }
     }

     /* select在指定时间内检测到套接字可连接即三次握手完成或者不需要select检测 */
     if(NULL != addr)
     {
         ret = accept(fd, (struct sockaddr *)addr, &addrlen);
     }
     else
     {
         ret = accept(fd, NULL, NULL);
     }
     if (-1 == ret) // 连接失败
     {
        perror("accept");
     }  
     return ret;   // 返回连接的套接字
}

struct ftpconnection *ftp_new(void)
{
    struct ftpconnection *connection;
#ifndef TEST_MEM_INFO
    connection = malloc(sizeof(struct ftpconnection));
#else
	connection = MemoryMalloc(sizeof(struct ftpconnection), "ftp", __FILE__, __FUNCTION__, __LINE__);
#endif
    if(NULL == connection)
    {
        return NULL;                                            //初始化一个ftp
    }

    memset(connection->hostname, 0, 64);    //清除连接
    memset(connection->system, 0, 128);
    memset(connection->username, 0, 32);
    memset(connection->password, 0, 32);
    memset(connection->remotedir, 0, 256);//清除远程连接dir
    memset(connection->localdir, 0, 256);//清除本地dir
    connection->port = 0;
    connection->status = STATUS_NEW;    //连接状态
    connection->controlfd = 0;
    connection->datafd = 0;
    connection->dataconfd = 0;
    return connection;
}

/* ftp连接函数 */
int ftp_connect(struct ftpconnection *connection)
{
    struct hostent *hp = NULL;
    int response = 0;
    socklen_t len = sizeof(struct sockaddr_in);
    int readynow = 0;
    char resultstring[4096] = {0};
    struct sockaddr_in controladdr;
    struct timeval timeout;

    if (connection->status < STATUS_WAIT_CONNECT) //没有连接上
    {
        return -1;
    }
    if (!connection->hostname || !*connection->hostname) //先初始化ftpconnection 结构体
    {
        FtpClientDbg("Must initialize struct ftpconnection first\n");
        return -1;
    }

    if (!(hp = (struct hostent *)gethostbyname(connection->hostname))) //得到hostnet 结构体传入域名主机名
    {
        /* musl 没有 hstrerror，直接使用 h_errno 整数值；HERROR 在 musl 下展开为 h_errno，类型为 int */
        FtpClientDbg("Can't resolve %s: h_errno=%d\n", connection->hostname, h_errno);
        return -1;
    }

    if ((connection->controlfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) //网络文件fd
    {
        FtpClientSys("socket");
        return -1;
    }

    fflush(stdout);    //刷新
    memset(&controladdr, 0, sizeof(controladdr));//网络数据包清空
    controladdr.sin_family = AF_INET;
    controladdr.sin_port = htons(connection->port);//网络端口

    if(hp == NULL || hp->h_addr_list == NULL || hp->h_addr_list[0] == NULL)
    {
        shutdown(connection->controlfd, SHUT_RDWR);                //关闭连接
        close(connection->controlfd);
        return -1;
    }

    controladdr.sin_addr = *(struct in_addr *)hp->h_addr_list[0];    //ip地址

    timeout.tv_sec  = 1;      //秒
    timeout.tv_usec = 0;

    setsockopt(connection->controlfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));
    setsockopt(connection->controlfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

    if (connect(connection->controlfd, (SOCKADDR *)&controladdr, sizeof(controladdr)) < 0)
    {
        FtpClientDbg("Connection refused to %s\n", connection->hostname);        //是否拒绝连接
        shutdown(connection->controlfd,SHUT_RDWR);                //关闭连接
        close(connection->controlfd);
        return -1;
    }

    //获取sock的名字是否正确
    if (getsockname(connection->controlfd, (SOCKADDR *)&controladdr, &len)< 0)
    {
        perror("getsockname");
        shutdown(connection->controlfd, SHUT_RDWR);                //关闭连接
        close(connection->controlfd);
        return -1;
    }
    connection->localip = controladdr.sin_addr.s_addr;    //本地ip
    connection->status = STATUS_WAIT_LOGIN;
    while (!readynow)
    {
        response = ftp_getrc(connection, resultstring, 4096, 0);    //ftp服务器是否有响应
        switch (response)
        {
            case 120 : 
            {
                FtpClientDbg("Service not yet ready.. waiting for 15 seconds\n");
                sleep(15);
                break;
            }
            case 220 : 
            {
                readynow = 1;                            //连接上
                FtpClientDbg("Connected to %s on port %d\n",connection->hostname, connection->port);
                break;
            }
            default  :
            {
               FtpClientDbg("%s\n", ftp_error_message(response));
               return response;
               break;
            }
        }
    }
    return 0;
}

/* 断开ftp连接 */
int ftp_disconnect(struct ftpconnection *connection) 
{
//FtpClientDbg("connection->status = %d(STATUS_WAIT_LOGIN=%d)\n", connection->status, STATUS_WAIT_LOGIN);
    if (connection->status >= STATUS_WAIT_LOGIN) //STATUS_IDLE)
    {
        int response = -1;
        response = ftp_sendline(connection, "QUIT\r\n");        //发送指令    推出连接
        response = ftp_getrc(connection, NULL, 0, 0);        //获取连接状态看是否成功断开
        if (500 == response)
        {
            FtpClientDbg("Braindead server doesn't understand QUIT.. closing connection\n");
        }

        if (connection->controlfd)
        {
            if (shutdown(connection->controlfd, SHUT_RDWR))
            {
                FtpClientSys("Fail to called shutdown socket: %d\n", connection->controlfd);
            }
            else
            {
                FtpClientDbg("shutdown socket %d over!\n", connection->controlfd);
            }

            if (close(connection->controlfd) < 0)
            {
                FtpClientSys("Fail to called close socket: %d\n", connection->controlfd);
                return -1;
            }
            else
            {
                FtpClientDbg("close socket %d over!\n", connection->controlfd);
            }
        }
        connection->controlfd = 0;
    }
    if (connection->hostname)        //从哪个主机断开连接
    {
        FtpClientDbg("Disconnected from %s\n", connection->hostname);
    }
    connection->controlfd = 0;

    return 0;
}

/* 复位ftp连接 */
int ftp_reset(struct ftpconnection *connection) 
{
    if (connection->status >= STATUS_WAIT_LOGIN)
    {
        FtpClientDbg("Still connected. Call ftp_disconnect() first!\n");
        return 1;
    }
    memset(connection, 0, sizeof(struct ftpconnection));
    connection->status = STATUS_NEW;
    return 0;
}

/* ftp读取一行 */
char *ftp_readline(struct ftpconnection *connection)
{
    int n = 0;
    int length = 0;
    char *temp = NULL, *buffer = NULL;
    static char *read_line = 0;

    FREE(read_line); //将一行清空
    read_line = (char *)NULL;

#ifndef TEST_MEM_INFO
    buffer = (char *)malloc(MAX_LINE_SIZE); //200
#else
	buffer = (char *)MemoryMalloc(MAX_LINE_SIZE, "ftp", __FILE__, __FUNCTION__, __LINE__);
#endif
    if(NULL == buffer)
    {
        return NULL;
    }

    if (connection->controlfd <= 0)
    {
        FtpClientDbg("ftp_readline: Not connected\n");
        FREE(buffer);
        return 0;
    }
    
    temp = buffer;
    //读到\n \0就退出
    while ((n = read(connection->controlfd, temp, 1)) > 0)
    {
        if (*temp == '\r') continue;
        if (*temp == '\n') break;
        if (*temp == '\0') break;
        if (length == MAX_LINE_SIZE) break;
        temp++; length++;                                //读取的长度
    }
    if (n < 0)
    {
        FtpClientSys("read\n");
        FREE(buffer);
        return NULL;
    }
    if (n == 0)
    {
        FtpClientDbg("FTP server disconnected\n");
        FREE(buffer);
        return NULL;
    }
#ifndef TEST_MEM_INFO
    read_line = (char *)malloc(length + 1);
#else
	read_line = (char *)MemoryMalloc(length + 1, "ftp", __FILE__, __FUNCTION__, __LINE__);
#endif
    if(NULL == read_line)
    {
        FREE(buffer);
        return NULL;
    }
    strncpy(read_line, buffer, length);    //将buffer的内容复制到read_line
    read_line[length] = '\0';                
    //log_hex(LOG_DATA, "Read", read_line, length);
    FREE(buffer);
    return read_line;
}

/* ftp发送一行 */
int ftp_sendline(struct ftpconnection *connection, char *line)
{
    if (!line || !*line)
    {
        return 0;
    }

    int n = 0;
    int size = 0;

    size = strlen(line);            //发送的长度
    if (size == 0)
    {
        return 0;
    }

    if (connection->status <= STATUS_WAIT_CONNECT)
    {
        FtpClientDbg("ftp_sendline: Not connected\n");
        return 0;
    }

    // 将一行的内容写到line中去
    if ((n = write(connection->controlfd, line, size)) < 0)
    {
        FtpClientSys("ftp_sendline: write\n");
    }
    return n;
}

int ftp_getrc(struct ftpconnection *connection, char *data, int max_size, int print_line)
{
    char *line = NULL;
    int response = 0;

    // 没有连接上
    if (connection->status <= STATUS_WAIT_CONNECT)
    {
        FtpClientDbg("ftp_getrc: Not connected\n");                
        return 0;
    }
    
    // ftp读取一行
    while ((line = ftp_readline(connection)))
    {
        if (!*line)
        {
            FtpClientDbg("Big bad error.. empty string returned by ftp_readline\n");
            break;
        }
        if (print_line)
        {
            if (line[0] == ' ')
            {
                FtpClientDbg("%s\n", (char *)(line + 1));
            }
            else
            {
                FtpClientDbg("%s\n", (char *)(line + 4));
            }
        }
        if (IS_NUMBER(line[0]) && IS_NUMBER(line[1]) && IS_NUMBER(line[2]) && line[3] == ' ')//0-9
        {
            break;
        }
    }

    if (!line || !*line) return 0;
    sscanf(line, "%d ", &response);        //读取想要的字符
    if (data)
    {
        strncpy(data, (line + 4), max_size);    //拷贝到data中来
    }
    if (response >= 100)
    {
        return response;
    }

    FREE(line);
    
    return 0;
}

int ftp_parse_pasv(char *line, char *address)
{
    int h1, h2, h3, h4, p1, p2, port;                //ftp解析pasv?
    if (line == NULL) return 0;
    sscanf(line, "%i,%i,%i,%i,%i,%i", &h1, &h2, &h3, &h4, &p1, &p2);
    port = (p1 << 8) + p2;
    sprintf(address, "%d.%d.%d.%d", h1, h2, h3, h4);    //获取地地址 ?
    return port;
}

/* 解析 文件是否有权限drwxrwxrwx */
int ftp_parse_permissions(char *perm)
{
    int permissions = 0;
    if (perm[1] != 'r' && perm[1] != '-') return -1;
    if (perm[0] == 'd') permissions |= 2048;
    if (perm[1] == 'r') permissions |= 256;
    if (perm[2] == 'w') permissions |= 128;
    if (perm[3] == 'x') permissions |= 64;
    if (perm[4] == 'r') permissions |= 32;
    if (perm[5] == 'w') permissions |= 16;
    if (perm[6] == 'x') permissions |= 8;
    if (perm[7] == 'r') permissions |= 4;
    if (perm[8] == 'w') permissions |= 2;
    if (perm[9] == 'x') permissions |= 1;
    return permissions;
}

/* 检查连接状态 */
int ftp_check_ready(struct ftpconnection *connection, int print)
{
    if (connection->status <= STATUS_WAIT_CONNECT)
    {
        if (print) FtpClientDbg("Not connected\n");
        return 0;
    }
    if (connection->status == STATUS_WAIT_LOGIN)
    {
        if (print) FtpClientDbg("Not logged in\n");
        return 0;
    }
    if (connection->status == STATUS_ERROR)
    {
        if (print) FtpClientDbg("Connection is in error state!\n");
        return 0;
    }
    if (connection->status >= STATUS_WAITING)
    {
        if (print) FtpClientDbg("Server is busy. Please wait\n");
        return 0;
    }
    return 1;
}

int ftp_stat(struct ftpconnection *connection, char *filename, struct filedetails *details)
{
    char *buffer = NULL;
    int result = 0;                                        //关于statd的
    if (!details) return -1;
    if (!ftp_check_ready(connection, 1)) return -1;            //连接上
    if (details) memset(details, 0, sizeof(struct filedetails));

#ifndef TEST_MEM_INFO
    buffer = (char *)malloc(strlen(filename) + 15);
#else
	buffer = (char *)MemoryMalloc(strlen(filename) + 15, "ftp", __FILE__, __FUNCTION__, __LINE__);
#endif
    if(NULL == buffer)
    {
        return -1;
    }
    sprintf(buffer, "STAT %s\r\n", filename);
    ftp_sendline(connection, buffer);            //将buffer中的内容发送出去
    FREE(buffer);
    buffer = ftp_readline(connection);            //将读取到的内容放在buffer中
    result = atoi(buffer);                        //将buffer 转为数字
    switch (result)
    {
        case 211 :
        case 212 :
        case 213 :
        case 250 : break;
        case 500 : FtpClientDbg("Server doesn't understand STAT\n");
                return result; break;
        case 501 : FtpClientDbg("Bad parameters to STAT\n");
                return result; break;
        case 502 : FtpClientDbg("Server doesn't understand STAT\n"); 
                return result; break;
        case 421 : FtpClientDbg("Service not available\n");
                return result; break;
        default  : FtpClientDbg("Unknown response to STAT: %d\n", result); 
                return result; break;
    }

    if (buffer[3] == '-')
    {
        /* Read a line at a time */
        while ((buffer = ftp_readline(connection)))
        {
            if (strstr(buffer, "total ") == buffer) continue;
            break;
        }
        if (buffer[0] == '2' && buffer[1] == '1') return 1;
        if (!buffer)
        {
            FtpClientDbg("Disconnected!\n");
            ftp_disconnect(connection);
            return 1;
        }
        if (strstr(buffer, "/bin/ls") != 0)
        {
            ftp_readline(connection);
            return 10;
        }
        // 分割
        if (ftp_split_ls(buffer, details) == 1) 
        {
            ;
            ftp_strip_illegal(details->filename);    //去掉非法的
            ftp_getrc(connection, NULL, 0, 0);
            return 0;
        }
        else
        {
            ftp_getrc(connection, NULL, 0, 0);
            return 1;
        }
    } 
    else 
    {
        FtpClientDbg("Bad response to STAT: %s\n", buffer);
    }

    FREE(buffer);

    return 0;
}

int ftp_split_ls(char *buffer, struct filedetails *details)
{    
    int stat_index, complete_match = 0;

    if (!buffer || !*buffer) return 0;

    /* Try matching STAT output */
    stat_index = 0;
    while (stat_formats[stat_index])
    {                        //分割文件内容找到相关信息
        char *format_p, *buffer_p;
        complete_match = 1;
        buffer_p = buffer;
        format_p = stat_formats[stat_index];

        while (*format_p)
        {
            while (*buffer_p == ' ' || *buffer_p == '\t')
                buffer_p++;                /* Skip whitespace */
            if (*format_p == '%')
            {
                switch (*++format_p)
                {
                    case '%' : if (*buffer_p == '%') {    /* %% means a literal % character */
                                   buffer_p++;
                               } else {
                                   complete_match = 0;
                               }
                               break;
                    case 'P' : { char *permissions_p; /* UNIX style permissions */
#ifndef TEST_MEM_INFO
								char *permissions = (char *)malloc(4096);
#else
								char *permissions = (char *)MemoryMalloc(4096, "ftp", __FILE__, __FUNCTION__, __LINE__);
#endif
                               if(NULL == permissions)
                               {    
                                    return 0;
                               }
                               memset(permissions, 0, 4096);
                               permissions_p = permissions;
                               format_p++;
                               while (*buffer_p && (*buffer_p != *format_p)) 
                                   *permissions_p++ = *buffer_p++; /* Loop and grab everything up until the next character in the format */
                               details->permissions = ftp_parse_permissions(permissions);
                               if (details->permissions == 0) complete_match = 0;
                               FREE(permissions);
                               break; }
                    case 'L' : { char *links_p; /* UNIX style links */
#ifndef TEST_MEM_INFO
								char *links = (char *)malloc(4096);
#else
								char *links = (char *)MemoryMalloc(4096, "ftp", __FILE__, __FUNCTION__, __LINE__);
#endif
                               if(NULL == links)
                               {    
                                    return 0;
                               }
                               memset(links, 0, 4096);
                               links_p = links;
                               format_p++;
                               while (*buffer_p && (*buffer_p != *format_p)) 
                                   *links_p++ = *buffer_p++; /* Loop and grab everything up until the next character in the format */
                               for (links_p = links; *links_p; links_p++)
                                   if (*links_p < '0' || *links_p > '9') complete_match = 0;
                               details->links = atoi(links);
                               FREE(links);
                               break; }
                    case 'O' : { char *owner_p; /* UNIX style owner */
#ifndef TEST_MEM_INFO
								char *owner = (char *)malloc(4096);
#else
								char *owner = (char *)MemoryMalloc(4096, "ftp", __FILE__, __FUNCTION__, __LINE__);
#endif
                               if(NULL == owner)
                               {    
                                   return 0;
                               }
                               memset(owner, 0, 4096);
                               owner_p = owner;
                               format_p++;
                               while (*buffer_p && (*buffer_p != *format_p)) 
                                   *owner_p++ = *buffer_p++; /* Loop and grab everything up until the next character in the format */
                               strncpy(details->owner, owner, 63);
                               FREE(owner);
                               break; }
                    case 'G' : { char *group_p; /* UNIX style group */
#ifndef TEST_MEM_INFO
								char *group = (char *)malloc(4096);
#else
								char *group = (char *)MemoryMalloc(4096, "ftp", __FILE__, __FUNCTION__, __LINE__);
#endif
                               if(NULL == group)
                               {    
                                    return 0;
                               }
                               memset(group, 0, 4096);
                               group_p = group;
                               format_p++;
                               while (*buffer_p && (*buffer_p != *format_p)) 
                                   *group_p++ = *buffer_p++; /* Loop and grab everything up until the next character in the format */
                               strncpy(details->group, group, 63);
                               FREE(group);
                               break; }
                    case 'S' : { char *size_p; /* UNIX style size */
#ifndef TEST_MEM_INFO
								char *size = (char *)malloc(4096);
#else
								char *size = (char *)MemoryMalloc(4096, "ftp", __FILE__, __FUNCTION__, __LINE__);
#endif
                               if(NULL == size)
                               {    
                                    return 0;
                               }
                               memset(size, 0, 4096);
                               size_p = size;
                               format_p++;
                               while (*buffer_p && (*buffer_p != *format_p)) 
                                   *size_p++ = *buffer_p++; /* Loop and grab everything up until the next character in the format */
                               for (size_p = size; *size_p; size_p++)
                                   if (*size_p < '0' || *size_p > '9') complete_match = 0;
                               details->size = atol(size);
                               FREE(size);
                               break; }
                    case 'N' : { char *filename_p; /* UNIX style filename */
#ifndef TEST_MEM_INFO
								char *filename = (char *)malloc(4096);
#else
								char *filename = (char *)MemoryMalloc(4096, "ftp", __FILE__, __FUNCTION__, __LINE__);
#endif
                               if(NULL == filename)
                               {    
                                    return 0;
                               }
                               memset(filename, 0, 4096);
                               filename_p = filename;
                               format_p++;
                               while (*buffer_p && (*buffer_p != *format_p)) 
                                   *filename_p++ = *buffer_p++; /* Loop and grab everything up until the next character in the format */
                               strncpy(details->filename, filename, 254);
                               FREE(filename);
                               break; }
                    case 'Y' : { char *year_p; /* UNIX style year */
#ifndef TEST_MEM_INFO
								char *year = (char *)malloc(4096);
#else
								char *year = (char *)MemoryMalloc(4096, "ftp", __FILE__, __FUNCTION__, __LINE__);
#endif
                               if(NULL == year)
                               {    
                                    return 0;
                               }
                               memset(year, 0, 4096);
                               year_p = year;
                               format_p++;
                               while (*buffer_p && (*buffer_p != *format_p)) 
                                   *year_p++ = *buffer_p++; /* Loop and grab everything up until the next character in the format */
                               for (year_p = year; *year_p; year_p++)
                                   if (*year_p < '0' || *year_p > '9') complete_match = 0;
                               FREE(year);
                               break; }
                    case 'M' : { char *month_p; /* UNIX style month name */
#ifndef TEST_MEM_INFO
								char *month = (char *)malloc(4096);
#else
								char *month = (char *)MemoryMalloc(4096, "ftp", __FILE__, __FUNCTION__, __LINE__);
#endif
                               if(NULL == month)
                               {    
                                    return 0;
                               }
                               memset(month, 0, 4096);
                               month_p = month;
                               format_p++;
                               while (*buffer_p && (*buffer_p != *format_p)) 
                                   *month_p++ = *buffer_p++; /* Loop and grab everything up until the next character in the format */
                               FREE(month);
                               break; }
                    case 'n' : { char *month_p; /* UNIX style month number */
#ifndef TEST_MEM_INFO
								char *month = (char *)malloc(4096);
#else
								char *month = (char *)MemoryMalloc(4096, "ftp", __FILE__, __FUNCTION__, __LINE__);
#endif
                               if(NULL == month)
                               {    
                                    return 0;
                               }
                               memset(month, 0, 4096);
                               month_p = month;
                               format_p++;
                               while (*buffer_p && (*buffer_p != *format_p)) 
                                   *month_p++ = *buffer_p++; /* Loop and grab everything up until the next character in the format */
                               for (month_p = month; *month_p; month_p++)
                                   if (*month_p < '0' || *month_p > '9') complete_match = 0;
                               FREE(month);
                               break; }
                    case 'm' : { char *minute_p; /* UNIX style minute number */
#ifndef TEST_MEM_INFO
								char *minute = (char *)malloc(4096);
#else
								char *minute = (char *)MemoryMalloc(4096, "ftp", __FILE__, __FUNCTION__, __LINE__);
#endif
                               if(NULL == minute)
                               {    
                                    return 0;
                               }
                               memset(minute, 0, 4096);
                               minute_p = minute;
                               format_p++;
                               while (*buffer_p && (*buffer_p != *format_p)) 
                                   *minute_p++ = *buffer_p++; /* Loop and grab everything up until the next character in the format */
                               for (minute_p = minute; *minute_p; minute_p++)
                                   if (*minute_p < '0' || *minute_p > '9') complete_match = 0;
                               FREE(minute);
                               break; }
                    case 'D' : { char *mday_p; /* UNIX style mday number */
#ifndef TEST_MEM_INFO
								char *mday = (char *)malloc(4096);
#else
								char *mday = (char *)MemoryMalloc(4096, "ftp", __FILE__, __FUNCTION__, __LINE__);
#endif
                               if(NULL == mday)
                               {    
                                    return 0;
                               }
                               memset(mday, 0, 4096);
                               mday_p = mday;
                               format_p++;
                               while (*buffer_p && (*buffer_p != *format_p)) 
                                   *mday_p++ = *buffer_p++; /* Loop and grab everything up until the next character in the format */
                               for (mday_p = mday; *mday_p; mday_p++)
                                   if (*mday_p < '0' || *mday_p > '9') complete_match = 0;
                               FREE(mday);
                               break; }
                    case 's' : { char *second_p; /* UNIX style second number */
#ifndef TEST_MEM_INFO
								char *second = (char *)malloc(4096);
#else
								char *second = (char *)MemoryMalloc(4096, "ftp", __FILE__, __FUNCTION__, __LINE__);
#endif
                               if(NULL == second)
                               {    
                                    return 0;
                               }
                               memset(second, 0, 4096);
                               second_p = second;
                               format_p++;
                               while (*buffer_p && (*buffer_p != *format_p)) 
                                   *second_p++ = *buffer_p++; /* Loop and grab everything up until the next character in the format */
                               for (second_p = second; *second_p; second_p++)
                                   if (*second_p < '0' || *second_p > '9') complete_match = 0;
                               FREE(second);
                               break; }
                    case 'h' : { char *hour_p; /* UNIX style hour number */
#ifndef TEST_MEM_INFO
								char *hour = (char *)malloc(4096);
#else
								char *hour = (char *)MemoryMalloc(4096, "ftp", __FILE__, __FUNCTION__, __LINE__);
#endif
                               if(NULL == hour)
                               {    
                                    return 0;
                               }
                               memset(hour, 0, 4096);
                               hour_p = hour;
                               format_p++;
                               while (*buffer_p && (*buffer_p != *format_p)) 
                                   *hour_p++ = *buffer_p++; /* Loop and grab everything up until the next character in the format */
                               for (hour_p = hour; *hour_p; hour_p++)
                                   if (*hour_p < '0' || *hour_p > '9') complete_match = 0;
                               FREE(hour);
                               break; }
                    default  : FtpClientDbg("Unknown character %c in STAT format description\n", *format_p);
                               break;
                }
            }
            if (complete_match == 0) break;
            if (!*format_p++) break;
        }
        if (complete_match == 1) break;
        stat_index++;
    }
    return complete_match;
}

/* 删除文件 */
int ftp_delete(struct ftpconnection *connection, char *filename)
{
    int result = 0;
    char line[4096] = {0};
    char resultstring[4096] = {0};
    if (!filename) return 0;
    if (!ftp_check_ready(connection, 1)) return 0;
    sprintf(line, "DELE %s\r\n", filename);
    ftp_sendline(connection, line);
    result = ftp_getrc(connection, resultstring, 4096, 0);
    switch (result)
    {
        case 250 : FtpClientDbg("Deleted %s\n", filename); 
                return 1; break;
        case 450 : FtpClientDbg("Can't delete %s: %s\n", filename, resultstring);
                break;
        case 421 : FtpClientDbg("%s\n", resultstring); 
                break;
        case 500 : FtpClientDbg("This server does not support the DELE command\n");
                break;
        case 501 : FtpClientDbg("Invalid parameters to DELE command\n");
                break;
        case 502 : FtpClientDbg("This server does not support the DELE command\n");
                break;
        case 530 : FtpClientDbg("Not logged in correctly\n"); 
                break;
        case 550 : FtpClientDbg("Can't delete %s: %s\n", filename, resultstring);
                break;
        case 553 : FtpClientDbg("Not allowed to delete %s\n", filename); 
                break;
        default  : FtpClientDbg("Unrecognised response to DELE: %d %s\n", result, resultstring);
                break;
    }
    return 0;
}

char *ftp_glob(struct ftpconnection *connection, char *wildcard, int status)
{
    static int index = 0;
    static char pathname[4096]={0}, working[4096]={0};
    int response = 0;
    static char *return_list[500];
    char *retstring = NULL;
    if (!ftp_check_ready(connection, 1)) return NULL;
    if (status == 0)
    {
        memset(pathname, 0, 4096);
        memset(working, 0, 4096);
        memset(return_list, 0, sizeof(*return_list) * 500);
        // Separate the path and filename
        index = strlen(wildcard);        //分离文件路径和文件名
        while (index)
        {
            if (wildcard[index] == '/')
            {
                strncpy(pathname, wildcard, index + 1);
                strncpy(working, &wildcard[index + 1], 4096);
                break;
            }
            index--;
        }
        if (strlen(working) == 0) strncpy(working, wildcard, 4096);
        response = ftp_nlst(connection, pathname, return_list, 500);
        if (response <= 0) return NULL;
        index = 0;
    }
    while (return_list[index] != NULL) 
    {
        int eflags = FNM_PATHNAME | FNM_PERIOD | (1 << 3);
        if (fnmatch(wildcard, return_list[index], eflags) != 0)
        {
            index++;
            continue;
        }
        retstring = (char *)strdup(return_list[index]);
        return_list[index] = 0;
        index++;
        return retstring;
    }
    return NULL;
}

/* 检测ftp服务器上的文件是否存在 */
int ftp_exists(struct ftpconnection *connection, char *filename)
{
    /* Check if a file exists on the server */
    /* Return codes
       < 0        Don't know
         0        Doesn't exist
       > 0        Does exist
     */
    char line[4096] = {0};
    int result = 0;

    if (!filename) return 0;
    if (!ftp_check_ready(connection, 1)) return 0;

    sprintf(line, "STAT %s\r\n", filename);
    ftp_sendline(connection, line);
    result = ftp_getrc(connection, NULL, 0, 0);    //获取连接反应
    switch (result)
    {
        case 213 : return 1;
        case 211 : return 1;
        case 212 : return 1;
        case 421 : FtpClientDbg("STAT error.. %d\n", result); 
                   return -1; break;
        case 450 : return 0;
        case 500 : break;
        case 501 : break;
        case 502 : break;
        case 530 : FtpClientDbg("Not logged in correctly\n"); 
                   return -1; break;
        default  : break;
     }
    return -1;
}

/* ftp错误信息 */
char *ftp_error_message(int error)
{
    switch(error)
    {
        case 110 : return "Restart marker reply"; break;
        case 120 : return "Service ready in nnn minutes."; break;
        case 125 : return "Data connection already open; transfer starting."; break;
        case 150 : return "File status okay; about to open data connection."; break;
        case 200 : return "Command okay."; break;
        case 202 : return "Command not implemented, superfluous at this site."; break;
        case 211 : return "System status, or system help reply."; break;
        case 212 : return "Directory status."; break;
        case 213 : return "File status."; break;
        case 214 : return "Help message."; break;
        case 215 : return "NAME system type."; break;
        case 220 : return "Service ready for new user."; break;
        case 221 : return "Service closing control connection."; break;
        case 225 : return "Data connection open; no transfer in progress."; break;
        case 226 : return "Closing data connection successfully."; break;
        case 227 : return "Entering Passive Mode (h1,h2,h3,h4,p1,p2)."; break;
        case 230 : return "User logged in, proceed."; break;
        case 250 : return "Requested file action okay, completed."; break;
        case 257 : return "PATHNAME created."; break;
        case 331 : return "User name okay, need password."; break;
        case 332 : return "Need account for login."; break;
        case 350 : return "Requested file action pending further information."; break;
        case 421 : return "Service not available, closing control connection."; break;
        case 425 : return "Can't open data connection."; break;
        case 426 : return "Connection closed; transfer aborted."; break;
        case 450 : return "Requested file action not taken - File unavailable."; break;
        case 451 : return "Requested action aborted: local error in processing."; break;
        case 452 : return "Requested action not taken - Insufficient storage space in system."; break;
        case 500 : return "Syntax error, command unrecognized."; break;
        case 501 : return "Syntax error in parameters or arguments."; break;
        case 502 : return "Command not implemented."; break;
        case 503 : return "Bad sequence of commands."; break;
        case 504 : return "Command not implemented for that parameter."; break;
        case 530 : return "Not logged in."; break;
        case 532 : return "Need account for storing files."; break;
        case 550 : return "Requested action not taken - File unavailable."; break;
        case 551 : return "Requested action aborted: page type unknown."; break;
        case 552 : return "Requested file action aborted - Exceeded storage allocation"; break;
        case 553 : return "Requested action not taken - File name not allowed."; break;
        default  : return "Unknown error";
    }
}

