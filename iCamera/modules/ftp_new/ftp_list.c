/* FTP Directory List Functions
   $Revision: 1.3 $
   $Date: 2002/06/24 04:04:32 $
*/

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
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
#include "rc.h"
#include "text.h"
//#define TEST_MEM_INFO
int ftp_list(struct ftpconnection *connection, char *params, char showthem)
{
    char *buffer = NULL;                                                //列举文件
    char *outputformat = NULL;
    int response = 0, index = 0;

    if (!ftp_check_ready(connection, 1)) return -1;
    ftp_sendline(connection, "TYPE A\r\n");    // Go to ASCII mode                 类型
    ftp_getrc(connection, NULL, 0, 0);
    buffer = config(connection->variables, "passive");
    if (!buffer || strcasecmp(buffer, "true") == 0)
    {
        response = ftp_open_data_pasv(connection, NULL, 0);
    }
    else 
    {
        response = ftp_open_data(connection);
    }

    if (response)
    {
        FtpClientDbg("ftp_list: bad response from ftp_open_data (%d)\n",response);
        return -response;    // Can't open a data connection
    }

    if (params && *params)
    {
#ifndef TEST_MEM_INFO
		buffer = (char *)malloc(MAX_LINE_SIZE);
#else
		buffer = (char *)MemoryMalloc(MAX_LINE_SIZE, "ftp_list", __FILE__, __FUNCTION__, __LINE__);
#endif
        if(NULL == buffer)
        {
            return -1;
        }
        snprintf(buffer, MAX_LINE_SIZE, "LIST %s\r\n", params);
        ftp_sendline(connection, buffer);
        FREE(buffer);
    }
    else
    {
        ftp_sendline(connection, "LIST\r\n");
    }
    response = ftp_getrc(connection, NULL, 0, 0);    //列出文件夹
    switch (response)
    {
        case 125 : /* Already a data connection */
               break;
        case 150 : ftp_wait_data(connection);    /* Wait for the data connection */
               break;
        case 250 :
        case 226 : FtpClientDbg("No files available\n");
               return response;
               break;
        case 425 :
        case 426 :
        case 451 :
        case 450 : FtpClientDbg("Could not list directory\n");
               return response;
               break;
        case 421 : FtpClientDbg("Service unavailable\n");
               ftp_disconnect(connection);
               return response;
               break;
        case 500 : FtpClientDbg("Server doesn't understand LIST\n");
               return response;
               break;
        case 501 : FtpClientDbg("Server doesn't understand LIST parameters\n");
               return response;
               break;
        case 502 : FtpClientDbg("Server doesn't understand LIST\n");
               return response;
               break;
        case 530 : FtpClientDbg("Not logged in\n");
               return 0;
               break;
        default  : FtpClientDbg("Unknown response to LIST: %d\n", response);
               return response;
               break;
    }
    FtpClientDbg("ftp_list: processing list\n");
#ifndef TEST_MEM_INFO
    buffer = (char *)malloc(4096);
#else
	buffer = (char *)MemoryMalloc(4096, "ftp_list", __FILE__, __FUNCTION__, __LINE__);
#endif
    if(NULL == buffer)
    {
        return -1;
    }
    outputformat = config(connection->variables, "lsformat");       //列出参数
    while (1)
    {
        char *temp = NULL;
        memset(buffer, 0, 4096);
        if ((response = ftp_read_data_line(connection, buffer, 4096)) == 0) //读取一行
        {
            break; /* Read Line */
        }

        if (strlen(buffer) == 0) continue;    /* Got crap? */
        if (ftp_parse_permissions(buffer) < 0) continue;
        /* Parse -rw-r--r-- etc */
        temp = strchr(buffer, '\n');
        if (temp) *temp = 0;
        if (showthem)
        {
            if (outputformat && *outputformat)
            {
                FtpClientDbg("%s\n", listoutput(buffer, outputformat));
            }
            else
            {
                FtpClientDbg("%s\n", buffer);
            }
        }
        index++;        /* Keep a count of files */
    }
    FREE(buffer);
    ftp_close_data(connection);        /* Close the data connection */
    response = ftp_getrc(connection, NULL, 0, 0);    /* Server says ok? */    //获取服务器响应
    return index;
}

int ftp_nlst(struct ftpconnection *connection, char *params, char *list[], int max_items)
{
    char *buffer = NULL;                                                                //列举文件
    int index = 0, response = -1;

    if (!ftp_check_ready(connection, 0)) return -1;
    if (max_items > 0) memset(list, 0, sizeof(*list) * max_items);
    ftp_sendline(connection, "TYPE A\r\n");
    ftp_getrc(connection, NULL, 0, 0);

    buffer = config(connection->variables, "passive");
    if (!buffer || strcasecmp(buffer, "true") == 0)
    {
        response = ftp_open_data_pasv(connection, NULL, 0);            //打开数据传输
    }
    else
    {
        response = ftp_open_data(connection);
    }

    index = 0;
    if (params && *params)
    {
#ifndef TEST_MEM_INFO
        buffer = (char *)malloc(MAX_LINE_SIZE);
#else
		buffer = (char *)MemoryMalloc(MAX_LINE_SIZE, "ftp_list", __FILE__, __FUNCTION__, __LINE__);
#endif
        if(NULL == buffer)
        {
            return -1;
        }
        snprintf(buffer, MAX_LINE_SIZE, "NLST %s\r\n", params);
        ftp_sendline(connection, buffer);
        FREE(buffer);
    }
    else
    {
        ftp_sendline(connection, "NLST\r\n");
    }
    response = ftp_getrc(connection, NULL, 0, 0);
    switch (response)
    {
        case 125 : /* Already a data connection */
               break;
        case 150 : ftp_wait_data(connection);    /* Wait for the data connection */
               break;
        case 250 :
        case 226 : FtpClientDbg("No files available\n");
               connection->status = STATUS_IDLE;
               return response;
               break;
        case 425 :
        case 426 :
        case 451 :
        case 450 : FtpClientDbg("Could not list directory\n");
               connection->status = STATUS_IDLE;
               return response;
               break;
        case 421 : FtpClientDbg("Service unavailable\n");
               ftp_disconnect(connection);
               return response;
               break;
        case 500 : FtpClientDbg("Server doesn't understand NLST\n");
               connection->status = STATUS_IDLE;
               return response;
               break;
        case 501 : FtpClientDbg("Server doesn't understand NLST parameters\n");
               connection->status = STATUS_IDLE;
               return response;
               break;
        case 502 : FtpClientDbg("Server doesn't understand NLST\n");
               connection->status = STATUS_IDLE;
               return response;
               break;
        case 530 : FtpClientDbg("Not logged in\n");
               ftp_disconnect(connection);
               return 0;
               break;
        case 550 : FtpClientDbg("No files found matching %s\n", params); 
               connection->status = STATUS_IDLE;
               return 0;
               break;
        default  : FtpClientDbg("Unknown response to NLST: %d\n", response);
               connection->status = STATUS_IDLE;
               return response;
               break;
    }
#ifndef TEST_MEM_INFO
    buffer = (char *)malloc(4096);
#else
	buffer = (char *)MemoryMalloc(4096, "ftp_list", __FILE__, __FUNCTION__, __LINE__);
#endif
    if(NULL == buffer)
    {
        return -1;
    }
    while ((response = ftp_read_data_line(connection, buffer, 4096) > 0))
    {
        if (strlen(buffer) == 0) continue;
        if (strcmp(buffer, ".") == 0) continue;
        if (strcmp(buffer, "..") == 0) continue;
        if (max_items > 0) list[index] = strdup(buffer);
        index++;
        if (max_items > 0) if (index > max_items) break;
    }
    if (index >= max_items) index = max_items;
    /* else for (temp = index; temp < max_items; temp++) list[temp] = 0; */
    list[index] = NULL;
    ftp_getrc(connection, NULL, 0, 0);
    FREE(buffer);
    ftp_close_data(connection);
    return index;
}

char *listoutput(char *input, char *format)
{
    struct filedetails details;                    //列举输出
    int inputindex = -1;
    char output[MAX_LINE_SIZE] = {0}, *temp = NULL;
    /* 临时缓冲区用于追加，避免 snprintf 源/目的重叠；尺寸加倍以容纳追加内容 */
    char tmpBuf[MAX_LINE_SIZE * 2] = {0};

    strcpy(output, input);
    if (ftp_split_ls(output, &details))
    {
        return NULL;
    }
    memset(output, 0, MAX_LINE_SIZE);
    for (inputindex = 0; format[inputindex] != 0; inputindex++) //列举文件的一些信息
    {
        if (format[inputindex] == '%')
        {
            inputindex++;
            memset(tmpBuf, 0, sizeof(tmpBuf));
            switch (format[inputindex])
            {
                case '%' : snprintf(tmpBuf, sizeof(tmpBuf), "%s%%", output);
                   break;
                case 'f' : snprintf(tmpBuf, sizeof(tmpBuf), "%s%.200s", output, details.filename);
                   break;
                case 's' : snprintf(tmpBuf, sizeof(tmpBuf), "%s%lu", output, details.size);
                   break;
                case 't' : snprintf(tmpBuf, sizeof(tmpBuf), "%s%.24s", output, asctime(&details.timestamp));
                   break;
                case 'p' :
                   snprintf(tmpBuf, sizeof(tmpBuf), "%s%s", output, temp ? temp : "");
                   FREE(temp);
                   break;
                case 'o' : snprintf(tmpBuf, sizeof(tmpBuf), "%s%.63s", output, details.owner);
                   break;
                case 'g' : snprintf(tmpBuf, sizeof(tmpBuf), "%s%.63s", output, details.group);
                   break;
                case 'l' : snprintf(tmpBuf, sizeof(tmpBuf), "%s%d", output, details.links);
                   break;
                default:  snprintf(tmpBuf, sizeof(tmpBuf), "%s", output);
                   break;
            }
            /* 拷贝回 output 缓冲区 */
            strncpy(output, tmpBuf, MAX_LINE_SIZE - 1);
            output[MAX_LINE_SIZE - 1] = '\0';
        }
        else
        {
            memset(tmpBuf, 0, sizeof(tmpBuf));
            snprintf(tmpBuf, sizeof(tmpBuf), "%s%c", output, format[inputindex]);
            strncpy(output, tmpBuf, MAX_LINE_SIZE - 1);
            output[MAX_LINE_SIZE - 1] = '\0';
        }
    }
    return strdup(output);
}

