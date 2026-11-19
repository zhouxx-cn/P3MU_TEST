/* FTP Directory Functions
   $Revision: 1.4 $
   $Date: 2003/03/10 22:31:27 $
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

/* 显示当前ftp路径 */
char *ftp_pwd(struct ftpconnection *connection)
{
	char line[MAX_LINE_SIZE] = {0};
	char *temp = NULL, *temp2 = NULL;
	int length = 0, response = -1;
	if (!ftp_check_ready(connection, 1)) return NULL;
	ftp_sendline(connection, "PWD\r\n");
	response = ftp_getrc(connection, line, MAX_LINE_SIZE, 0);
	switch (response)
	{
		case 257 : break;
		case 500 : FtpClientDbg("Server doesn't understand PWD\n");
			   return NULL;
			   break;
		case 501 : FtpClientDbg("Server doesn't understand PWD parameters\n");
			   return NULL;
			   break;
		case 502 : FtpClientDbg("Server doesn't understand PWD\n");
			   return NULL;
			   break;
		case 421 : FtpClientDbg("Service unavailable\n");
			   ftp_disconnect(connection);
			   return NULL;
			   break;
		case 550 : FtpClientDbg("Directory information unavailable: %d\n", response);
			   return NULL;
			   break;
		default  : FtpClientDbg("Unknown response to PWD: %d\n", response);
			   return NULL;
			   break;
	}
	temp = strstr(line, "\"");
	temp++;
	temp2 = strstr(temp, "\"");
	length = temp2-temp;
	strncpy(connection->remotedir, temp, length);	//将tmp路径复制到远程文件夹
	connection->remotedir[length] = 0;
	return connection->remotedir;
}

/* 改变当前ftp路径 成功返回1*/
int ftp_cwd(struct ftpconnection *connection, char *path)
{
	char line[MAX_LINE_SIZE] = {0};
	int response = -1;
	if (!ftp_check_ready(connection, 1)) return 0;
	if (strcmp(path, connection->remotedir) == 0) return 1;	//比较远程路径和传过来的路径
	snprintf(line, sizeof(line), "CWD %s\r\n", path);
	ftp_sendline(connection, line);
	response = ftp_getrc(connection, NULL, 0, 0);			//查看命令响应
	switch (response)
	{
		case 250 : ftp_pwd(connection);
		   return 1;
		   break;
		case 530 : FtpClientDbg("Not logged in\n");
		   return 0;
		   break;
		case 500 : FtpClientDbg("Server doesn't understand CWD\n");
		   return 0;
		   break;
		case 501 : FtpClientDbg("Server doesn't understand CWD parameters\n");
		   return 0;
		   break;
		case 502 : FtpClientDbg("Server doesn't understand CWD\n");
		   return 0;
		   break;
		case 421 : FtpClientDbg("Service unavailable\n");
		   ftp_disconnect(connection);
		   return 0;
		   break;
		case 550 : FtpClientDbg("Directory \"%s\" does not exist\n", path);
		   return 0;
		   break;
		default  : FtpClientDbg("Unknown response to CWD: %d\n", response);
		   return 0;
		   break;
	}
	return 0;
}

/* 创建一个远程ftp目录(单层，不支持-p选项，需要自行处理) 
 *  @return 成功返回1
 */
int ftp_mkd(struct ftpconnection *connection, char *path)
{
	char line[MAX_LINE_SIZE] = {0};
	int response = -1;
	if (!ftp_check_ready(connection, 1)) return 0;
	snprintf(line, sizeof(line), "MKD %s\r\n", path);
	ftp_sendline(connection, line);					//发送命令
	response = ftp_getrc(connection, NULL, 0, 0);	//查看响应
	switch (257) 
	{
		case 257 : //printf("Created %s on %s\n",path, connection->remotedir);
		   return 1;
		   break;
		case 530 : FtpClientDbg("Not logged in\n");
		   return 0;
		   break;
		case 500 : FtpClientDbg("Server doesn't understand \"MKD %s\"\n", path);
		   return 0;
		   break;
		case 501 : FtpClientDbg("Server doesn't understand MKD parameters\n");
		   return 0;
		   break;
		case 502 : FtpClientDbg("Server doesn't understand MKD\n");
		   return 0;
		   break;
		case 421 : FtpClientDbg("Service unavailable\n");
		   ftp_disconnect(connection);
		   return 0;
		   break;
		case 550 : FtpClientDbg("Not allowed to create directory %s\n", path);
		   return 0;
		   break;
		default  : FtpClientDbg("Unknown response to MKD: %d\n", response);
		   return 0;
		   break;
	}
	return 0;
}

/* 移除一个ftp远程文件夹 */
int ftp_rmd(struct ftpconnection *connection, char *path)
{
    char line[MAX_LINE_SIZE] = {0};
    int response = -1;
    if (!ftp_check_ready(connection, 1)) return 0;
	snprintf(line, sizeof(line), "RMD %s\r\n", path);
    ftp_sendline(connection, line);
    response = ftp_getrc(connection, NULL, 0, 0);
    switch (response)
    {
        case 250 : FtpClientDbg("Removed %s in %s\n",path, connection->remotedir);
            return 1;
            break;
        case 530 : FtpClientDbg("Not logged in\n");
            return 0;
            break;
        case 500 : FtpClientDbg("Server doesn't understand \"RMD %s\"\n", path);
            return 0;
            break;
        case 501 : FtpClientDbg("Server doesn't understand RMD parameters\n");
            return 0;
            break;
        case 502 : FtpClientDbg("Server doesn't understand RMD\n");
            return 0;
            break;
        case 421 : FtpClientDbg("Service unavailable\n");
            ftp_disconnect(connection);
            return 0;
            break;
        case 550 : FtpClientDbg("Not allowed to remove directory %s\n", path);
            return 0;
            break;
        default  : FtpClientDbg("Unknown response to RMD: %d\n", response);
            return 0;
            break;
    }
    return 0;
}

