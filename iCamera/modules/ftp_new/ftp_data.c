/* FTP Data Functions
   $Revision: 1.4
   $Date: 2017-10-24 14:44:40
   $Author: FanCong
*/

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "ftp.h"

#undef TEST_MEM_INFO

int ftp_open_data_pasv(struct ftpconnection *connection, char *address, int port)
{
    int response = 0;                                                    //打开数据分析?
    struct sockaddr_in dataaddr;
    char *buffer = NULL, *line = NULL, *temp = NULL, *temp2 = NULL;
    int length = 0;
    
    if (!ftp_check_ready(connection, 1))
    {
        return 0;
    }
    
    if (!address)
    {
        ftp_sendline(connection, "PASV\r\n");
#ifndef TEST_MEM_INFO
        line = (char *)malloc(MAX_LINE_SIZE);
#else
		line  = (char *)MemoryMalloc(MAX_LINE_SIZE, "ftp_data", __FILE__, __FUNCTION__, __LINE__);
#endif
        if(NULL == line)
        {    
            return 1;
        }
        response = ftp_getrc(connection, line, MAX_LINE_SIZE, 0);
        switch (response)
        {
            case 227 : temp = strstr(line, "(");
                   temp++;
                   temp2 = strstr(temp, ")");
                   length = temp2-temp;            //()之间的长度
#ifndef TEST_MEM_INFO
					buffer = (char *)malloc(length + 1);
#else
					buffer = (char *)MemoryMalloc(length + 1, "ftp_data", __FILE__, __FUNCTION__, __LINE__);
#endif
                   if(NULL == buffer)
                   {
                        return 1;
                   }
                   memcpy(buffer, temp, length);
                   buffer[length] = 0;
#ifndef TEST_MEM_INFO
					address = (char *)malloc(20);
#else
					address = (char *)MemoryMalloc(20, "ftp_data", __FILE__, __FUNCTION__, __LINE__);
#endif
                   if(NULL == address)
                   {
                        return 1;
                   }
                   port = ftp_parse_pasv(buffer, address);    //传地址获取端口号
                   FREE(line);
                   FREE(buffer);
                   break;
            case 421 : FtpClientDbg("Service unavailable\n");
                   FREE(line);
                   ftp_disconnect(connection);
                   return 0;
                   break;
            case 500 : FtpClientDbg("Server doesn't understand PASV\n");
                   FREE(line);
                   return 0;
                   break;
            case 501 : FtpClientDbg("Server doesn't understand PASV parameters\n");
                   FREE(line);
                   return 0;
                   break;
            case 502 : FtpClientDbg("Server doesn't understand PASV\n");
                   FREE(line);
                   return 0;
                   break;
            case 530 : FtpClientDbg("Not logged in\n");
                   FREE(line);
                   return 0;
                   break;
            default  : FtpClientDbg("Unknown response to PASV: %d\n", response);
                   FREE(line);
                   return 0;
                   break;
        }
    }
    
    // 建立数据链接
    if ((connection->datafd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        FtpClientSys("socket\n");
        return 1;
    }
    FtpClientDbg("Opening a data connection to %s:%d\n",address, port);
    memset(&dataaddr, 0, sizeof(dataaddr));
    dataaddr.sin_family = AF_INET;
    dataaddr.sin_port = htons(port);
    if (inet_aton(address, &dataaddr.sin_addr) <= 0)
    {
        FtpClientSys("Fail to inet_aton\n");
        return 1;
    }

    if (connect(connection->datafd,(SOCKADDR *)&dataaddr, sizeof(dataaddr)) < 0)
    {
        FtpClientSys("Fail to connect\n");
        return 1;
    }
    connection->status = STATUS_TRANSFER;
    FREE(address);
    return 0;
}

int ftp_close_data(struct ftpconnection *connection)
{
    SAFE_CLOSE(connection->dataconfd);
    SAFE_CLOSE(connection->datafd);

    connection->status = STATUS_IDLE;
    FtpClientDbg("Data connection closed\n");
    return 0;
}

int ftp_read_data(struct ftpconnection *connection, char *buffer, int max_size)
{
    int n = 0;
    // 传输状态正确才可以读
    if (connection->status != STATUS_TRANSFER)
    {
        return -1;
    }
    memset(buffer, 0, max_size);
    n = read(connection->datafd, buffer, max_size);
    if (n < 0)
    {
        FtpClientSys("read\n");
    }
    return n;
}

int ftp_read_data_line(struct ftpconnection *connection, char *buffer, int max_size)
{
    int n = 0;
    char *temp = NULL;
    
    if (connection->status != STATUS_TRANSFER)
    {
        return -1;
    }

    memset(buffer, 0, max_size);
    temp = buffer;
    // 将读取的内容变为一行一行的
    while ((n = read(connection->datafd, temp, 1)) > 0)
    {
        if (temp[0] == '\r')
        {
            continue;
        }
        if (temp[0] == '\n')
        {
            break;
        }
        if (temp == (buffer + max_size))
        {
            break;
        }
        temp++;
    }

    temp[0] = 0;
    if (n < 0)
    {
        FtpClientSys("read fail!\n");
        return -1;
    }

    if (buffer[0] == 0)
    {
        return 0;
    }

    return n;
}

int ftp_write_data(struct ftpconnection *connection, char *buffer, int size)
{
    int n = 0;
    int ret = 0;

    /* 在MAX_FTP_WRITE_TIMEOUT时间之内如果还不能进行ftp传输，则超时退出 */
    ret = ftp_write_timeout(connection->datafd, MAX_FTP_WRITE_TIMEOUT);
    if(0 == ret)    // 检测到描述符可写
    {
        // 将数据写进buffer中去
        if ((n = write(connection->datafd, buffer, size)) < 0)
        {
            FtpClientSys("write data fail!\n");
            return -1;
        }
    }
    else if(-1==ret && errno==ETIMEDOUT)  // 超时即指定事件内为检测到套接字可写
    {
        FtpClientErr("ftp write timeout!\n"); // 进行超时处理
        return -1;
    }
    else
    {
        FtpClientErr("ftp write error!\n");  // 进行错误处理
        return -1;
    }

    return n;
}

int ftp_open_data(struct ftpconnection *connection)
{
    struct sockaddr_in servaddr;
    struct in_addr sin;
    char *line = NULL;
    int h1=0, h2=0, h3=0, h4=0, p1=0, p2=0, port=0;
    int response = 0;
    SOCKLEN len = 0;

    if ((connection->dataconfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        FtpClientSys("socket fail!\n");
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));            //服务器的网络信息
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = 0;
    if (bind(connection->dataconfd, (SOCKADDR *)&servaddr, sizeof(servaddr)) < 0)
    {
        FtpClientSys("bind fail!\n");
        return -1;
    }

    if (listen(connection->dataconfd, 1) < 0)
    {
        FtpClientSys("listen fail!\n");
        return -1;
    }

    len = 128;
    if (getsockname(connection->dataconfd, (SOCKADDR *)&servaddr, &len) < 0)
    {
        FtpClientSys("getsockname fail!\n");
        return -1;
    }
#ifndef TEST_MEM_INFO
    line = (char *)malloc(MAX_LINE_SIZE);
#else
	line = (char *)MemoryMalloc(MAX_LINE_SIZE, "ftp_data", __FILE__, __FUNCTION__, __LINE__);
#endif
    if(NULL == line)
    {
        FtpClientSys("malloc fail!\n");
        return -1;
    }
    port = servaddr.sin_port;
    sin.s_addr = connection->localip;
    snprintf(line, MAX_LINE_SIZE, "%s.%d\r\n", inet_ntoa(sin), port);
    sscanf(line, "%d.%d.%d.%d.%d", &h1, &h2, &h3, &h4, &port);
    p1 = (port / 256);
    p2 = port - (256 * p1);
    snprintf(line, MAX_LINE_SIZE, "PORT %d,%d,%d,%d,%d,%d\r\n", h1, h2, h3, h4, p2, p1);
    connection->status = STATUS_WAITING;
    ftp_sendline(connection, line);
    FREE(line);
    response = ftp_getrc(connection, NULL, 0, 0);
    switch (response)
    {
        case 200 : return 0;
        case 421 : FtpClientDbg("Service unavailable\n");
           ftp_disconnect(connection);
           return response;
        case 500 : FtpClientDbg("Server doesn't understand PORT\n");
           return response;
        case 501 : FtpClientDbg("Server doesn't understand PORT parameters\n");
           return response;
        case 502 : FtpClientDbg("Server doesn't understand PORT\n");
           return response;
        case 530 : FtpClientDbg("Not logged in\n");
           return response;
        default  : FtpClientDbg("Unknown response to PORT: %d\n", response);
           return response;
    }
    return 0;
}

int ftp_wait_data(struct ftpconnection *connection)
{
    // 等待数据传输
    struct sockaddr_in servaddr;
    socklen_t len = sizeof(servaddr);
    if (connection->status != STATUS_WAITING)
    {
        return 0;
    }
    connection->datafd = accept(connection->dataconfd, (SOCKADDR *)&servaddr, &len);
    FtpClientDbg("Data connection opened to %s\n", inet_ntoa(servaddr.sin_addr));
    connection->status = STATUS_TRANSFER;
    return 1;
}

