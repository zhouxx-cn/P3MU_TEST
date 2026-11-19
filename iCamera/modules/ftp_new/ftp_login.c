/* FTP Functions
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
#include "rc.h"
#include "ftp.h"

int ftp_login(struct ftpconnection *connection)
{
	int response = 0;
	int show_motd = 0;						//ftp登录
	char line[4096] = {0};
	if (connection->status == STATUS_IDLE)
	{
		ftp_sendline(connection, "REIN\r\n");			//重进
		response = ftp_getrc(connection, NULL, 0, 0);
		switch (response)
		{
			case 220 : break; /* OK REIN */
			case 120 : FtpClientDbg("Service not ready... try again later\n");
				   ftp_disconnect(connection);
				   return response;
			case 421 : FtpClientDbg("Service unavailable\n");
				   ftp_disconnect(connection);
				   return response;
			case 500 : FtpClientDbg("This server doesn't allow you to re-login\n");
				   return response;
			case 502 : FtpClientDbg("This server doesn't allow you to re-login\n");
				   return response;
				   break;
			default	 : FtpClientDbg("Unknown response %d\n", response);
				   return response;
		}
	} 
	else if (connection->status != STATUS_WAIT_LOGIN)
	{
		FtpClientDbg("ftp_login: Server is not waiting for login\n");
		return -1;
	}
	FtpClientDbg("Send username %s\n", connection->username);
	snprintf(line, sizeof(line), "USER %s\r\n", connection->username);
	response = ftp_sendline(connection, line);		//发送用户
	response = ftp_getrc(connection, NULL, 0, 0);
	switch (response)
	{
		case 230 : FtpClientDbg("Don't need to send password\n");
			   connection->status = STATUS_IDLE;
			   break;
		case 530 : FtpClientDbg("Not logged in as %s\n", connection->username);
			   return response;
		case 500 : FtpClientDbg("Server doesn't understand USER\n");
			   ftp_disconnect(connection);
			   return response;
		case 501 : FtpClientDbg("Server can't understand USER command\n");
			   ftp_disconnect(connection);
			   return response;
		case 421 : FtpClientDbg("Service unavailable\n");
			   ftp_disconnect(connection);
			   return response;
		case 332 : FtpClientDbg("This program does not support user accounts\n");
			   return response;
		case 331 : FtpClientDbg("Send password xxxxxxx\n");
			   snprintf(line, sizeof(line), "PASS %s\r\n", connection->password);
			   response = ftp_sendline(connection, line);			//发送password
			   //if (log_get_level() >= 5) show_motd = 1;
			   response = ftp_getrc(connection, NULL, 0, show_motd);
			   switch (response)
			   {
				   case 230 :
				   case 202 : break;
				   case 530 : FtpClientDbg("Login incorrect\n");
					      ftp_disconnect(connection);
					      connection->status = STATUS_WAIT_CONNECT;
					      return response;
				   case 500 :
				   case 503 :
				   case 501 : FtpClientDbg("Server doesn't understand PASS\n");
					      ftp_disconnect(connection);
					      return response;
				   case 421 : FtpClientDbg("ftp_login: Service is unavailable\n");
					      ftp_disconnect(connection);
					      return response;
				   case 332 :
				   default  : FtpClientDbg("Unknown response to PASS: %d\n", response);
					      return response;
			   }
			   break;
		default  : FtpClientDbg("Unknown response to USER: %d\n", response);
			   return response;
			   break;
	}
	connection->status = STATUS_IDLE;
//	printf("Getting remote system type\n");
	ftp_sendline(connection, "SYST\r\n");					//远程系统类型
	response = ftp_getrc(connection, connection->system, 128, 0);
	if (response == 215)
	{
		FtpClientDbg("Remote system type is %s\n", connection->system);
	}
	else
	{
		memset(connection->system, 0, sizeof(connection->system));
	}
	FtpClientDbg("Logged in to %s as %s\n", connection->hostname,connection->username);
	connection->status = STATUS_IDLE;
	ftp_pwd(connection);
	return 0;
}

