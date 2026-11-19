
/**
*server.c
*服务起端
*/
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/time.h>
// #include "sys_banned.h"
#include "global_gather.h"
int port = 8245;

//char *host_name = "127.0.0.1";
char *host_name = "checkip.dyndns.com";

//char *host_name = "58.49.56.137";
int getexternip(char *ip, int iplen);
#if 0
int main()
{
	char ip[20];

	memset(ip, 0, sizeof (ip));
	getexternip(ip, sizeof (ip));
	printf("ip = %s\n", ip);
	return 0;
}
#endif

int ip_host_cmp(char *ip,char *hostname)
{
	struct hostent *server_host_name;
	if ((server_host_name = gethostbyname(hostname)) != 0)
	{
		if (inet_addr(ip) == (((struct in_addr *) (server_host_name->h_addr))->s_addr))
		{
			return 0;
		}
		else
		{
			printf("ip = %x host = %x\n",inet_addr(ip),(((struct in_addr *) (server_host_name->h_addr))->s_addr));
			return 1;
		}
	}
	else
	{		
		printf("gethostbyname(%s)\n",hostname);
		perror("gethostbyname:");
	}
	
	return 1;
}

int getexternip(char *ip, int iplen)
{
	struct sockaddr_in pin;
	struct hostent *server_host_name;
	int sock_des;

    char *str = "GET /index.php HTTP/1.1\r\n"   \
            "Accept: */*\r\n"                           \
            "Accept-Language: zh-cn\r\n"                \
            "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727)\r\n"   \
            "Host: checkip.dyndns.com\r\n" "Connection: Close\r\n\r\n"; 
	char buf[20000];

	struct sigaction sigact;

	memset(&sigact, 0, sizeof (sigact));
	sigact.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sigact, 0);

	if ((server_host_name = gethostbyname(host_name)) == 0)
	{
		perror("gethostbyname fail ");
		return -1;
	}
	
	bzero(&pin, sizeof (pin));
	pin.sin_family = AF_INET;
	pin.sin_addr.s_addr = htonl(INADDR_ANY);
	pin.sin_addr.s_addr = ((struct in_addr *) (server_host_name->h_addr))->s_addr;
	//pin.sin_addr.s_addr = inet_addr(host_name);
	pin.sin_port = htons(port);
	if ((sock_des = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("call socket fail !");
		return -1;
	}

	//增加发送、接收超时设置
	struct timeval tv = { 30, 0 };
	setsockopt(sock_des, SOL_SOCKET, SO_SNDTIMEO, (void *) &tv, sizeof (tv));
	setsockopt(sock_des, SOL_SOCKET, SO_RCVTIMEO, (void *) &tv, sizeof (tv));
	
	if (connect(sock_des, (struct sockaddr*)&pin, sizeof (pin)) == -1)
	{
		perror("connect fail !");
		close(sock_des);
		return -1;
	}
	
	while (1)
	{

		if (send(sock_des, str, strlen(str), 0) == -1)
		{
			printf("begin send data error\n");
			close(sock_des);
			return -1;
		}

		memset(buf, 0, sizeof (buf));
		if (recv(sock_des, buf, 20000, 0) == -1)
		{
			perror("recv fail !");
			close(sock_des);
			return -1;
		}
		
		break;
	}

	close(sock_des);
	char *tmp;
	int ip1, ip2, ip3, ip4;

	if (NULL != (tmp = strstr(buf, "Current IP Address: ")))
	{
		tmp = tmp + sizeof ("Current IP Address: ") - 1;
		sscanf(tmp, "%d.%d.%d.%d", &ip1, &ip2, &ip3, &ip4);
		snprintf(ip, iplen, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
        printf("current extern ip: %d.%d.%d.%d\n", ip1, ip2, ip3, ip4);
		return 0;
	}
	else
	{
		printf("recive from client : %s \n", buf);
		return -1;
	}

	return 0;
}
