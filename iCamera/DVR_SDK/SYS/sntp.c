/*
文件:sntp.c
功能:以NTP服务器为基准，同步网内计算机或嵌入式设备的时间
作者:樊聪
时间:2017年5月18日 17:40:44
版本:V1.0.3
修改:添加从配置文件获取备份服务器功能
协议：
    客户端向NTP服务器发送请求数据包，NTP服务器回应一个数据包。
    分别记录客户端发送请求包时的时刻t1和接收到回应包的时刻t4，服务器回应的数据包内包含了
    服务器接收到请求包的时刻t2和服务器发送回应包的时刻t3。
    t4-t1表示整个消息传递过程所需要的时间；
    t3-t2表示消息传递过程在服务器停留的时间；
    (t4-t1)-(t3-t2)是来回路上消耗的时间，如果来回传输所用的时间一样，那么，单程的时间为：
    t = ( (t4 - t1) - (t3 - t2) )/2;
    假定客户端相对于服务器的时间误差是dis，则有下列等式：
    t2 = t1 + dis + t;
    t4 = t3 - dis + t;
    则：dis = (( t2 - t1 ) + ( t3 - t4 )) / 2;
    根据此差值重新设置时间即可。
说明：
    服务器发返回的时间是以1900年为基准计算的，而linux是以1970年为基准的，所以需要用到一个
    常数做转化, 在计算与服务器的差值时会用到此值：
    #define JAN_1970   (2208988800UL)        // 1970 - 1900 in seconds
*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "sntp.h"
#include "global_gather.h"
#include "mysystem.h"
#include "debug.h"

#define ISOCTAL(c)       ((c) >= '0' && (c) < '8')

typedef struct    
{  
    unsigned char LiVnMode;  
    unsigned char Stratum;  
    unsigned char Poll;  
    unsigned char Precision;  
    long int RootDelay;  
    long int RootDispersion;  
    char     RefID[4];  
    long int RefTimeInt;  
    long int RefTimeFraction;  
    long int OriTimeInt;  
    long int OriTimeFraction;  
    long int RecvTimeInt;  
    long int RecvTimeFraction;  
    long int TranTimeInt;  
    long int TranTimeFraction;  
}STNP_Header;

static int is_ip_address(const char * address)
{	
	int i = 0;
	int index = 0;
	unsigned short byte_value = 0;

	int total_len = strlen(address);
	if(total_len > 15)
		return 1;	

	for(i=0; i<4; i++)
	{
		if((address[index] < '0') || (address[index] > '9'))
			return 1;

		if(((address[index +1] < '0') || (address[index +1] > '9')) 
			&& (address[index +1] != '.'))
		{
			if((address[index +1] == '\0') && (i == 3))
				return 0;
			return 1;
		}
		if(address[index +1] == '.'){index+=2;continue;}

		if(((address[index +2] < '0') || (address[index +2] > '9'))	
			&& (address[index +2] != '.'))
		{
			if((address[index +2] == '\0') && (i == 3))
				return 0;
			return 1;
		}
		if(address[index +2] == '.'){index+=3;continue;}

		if(i < 3)	
		{
			if(address[index +3] != '.')
				return 1;
		}

		byte_value = (address[index] - '0') *100 + 
			(address[index +1] -'0') *10 + (address[index +2] - '0');

		if(byte_value > 255)
			return 1;

		if(i < 3)
			index += 4;
		else	
			index += 3;
	}

	if(index != total_len)
		return 1;	

	return 0;
}

static unsigned long hostname_to_ip(const char *host) 
{
    struct addrinfo hints;
    struct addrinfo *result, *result_pointer;
    int ret;
    /* obtaining address matching host */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;
    hints.ai_protocol = 0;  /* any protocol */

    ret = getaddrinfo(host, NULL, &hints, &result);
    if (ret != 0)
	{
        CSYS_ERR("getaddrinfo: %s host=[%s]\n", gai_strerror(ret), host);
        return -1;
    }
	
    /* traverse the returned list and output the ip addresses */
    for (result_pointer = result; result_pointer != NULL; result_pointer = result_pointer->ai_next) 
	{
        char hostname[256] = "";
        ret = getnameinfo(result_pointer->ai_addr, result_pointer->ai_addrlen, hostname, sizeof(hostname), NULL, 0, NI_NUMERICHOST);
        if (ret != 0) 
		{
            CSYS_ERR("error in getnameinfo: %s \n", gai_strerror(ret));
            continue;
        } 
		else 
        {
            //CSYS_INF("******ntp IP: %s \n", hostname);
            if (NULL != result)
            {
			    freeaddrinfo(result);
            }
			return (inet_addr(hostname));
        }
    }
	
    if (NULL != result)
    {
        freeaddrinfo(result);
    }
	
    return -1;
}

static int GetNTPTime(STNP_Header *H_SNTP, const char *pServerName, int nport)
{  
    if (NULL==H_SNTP || NULL==pServerName)
    {
        CSYS_ERR("H_SNTP or pServerName is NULL!\n");
        return -1;
    }

    int sockfd = -1;
    int ret    = -1;
    struct sockaddr_in server;  
    
    //bzero((void*)H_SNTP, sizeof(STNP_Header));  
    memset(H_SNTP, 0, sizeof(STNP_Header));
    H_SNTP->LiVnMode = 0x1b;  
	
    server.sin_family = AF_INET;  
    if (0 == is_ip_address(pServerName))
		server.sin_addr.s_addr = inet_addr(pServerName);
	else
		server.sin_addr.s_addr = hostname_to_ip(pServerName);
    server.sin_port = htons(nport);  

    if(-1 == (int)server.sin_addr.s_addr)
    {
        CSYS_ERR("server.sin_addr.s_addr = -1!\n");        
        return -1;  
    } 
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);  
    if(sockfd < 0)  
    {
    	fprintf(stderr, "ServerName:%s; Fail to call socket: %s\n", pServerName, strerror(errno));
        return -1;  
    }
    
    if(sendto(sockfd, (void*)H_SNTP, sizeof(STNP_Header), 0, (struct sockaddr*)&server, sizeof(server)) < 0)  
    {  
    	fprintf(stderr, "ServerName:%s; Fail to call sendto: %s\n", pServerName, strerror(errno));
        close(sockfd);
        return -1;  
    }
    
    fd_set r;  
    FD_ZERO(&r);  
    FD_SET(sockfd, &r);  
    struct timeval timeout;  
    timeout.tv_sec = 5;  
    timeout.tv_usec = 0;  

    ret = select(sockfd+1, &r, NULL, NULL, &timeout);
    if(1 != ret)
    {
    	fprintf(stderr, "ServerName:%s; Fail to call select!\n", pServerName);
        close(sockfd);
        return -1;  
    }
    
    if(recv(sockfd, (void*)H_SNTP, sizeof(STNP_Header), 0)<0)  
    {
    	fprintf(stderr, "ServerName:%s; Fail to call recv: %s\n", pServerName, strerror(errno));
        close(sockfd);
        return -1;  
    }
    
    close(sockfd);
	sockfd = -1;

	/**
     * RFC 958 / Network Time Protocol: 
     * In some cases a particular timestamp may not be available, such as
     * when the protocol first starts up. In these cases the 64-bit field
     * is set to zero, indicating the value is invalid or undefined.
     */
    if (0 == H_SNTP->RecvTimeInt || 0 == H_SNTP->TranTimeInt)
    {
        CSYS_ERR("invalid recv [%ld,%ld]\n", H_SNTP->RecvTimeInt, H_SNTP->TranTimeInt);
        return -1;
    }
    
    return 0;  
}  

/* 进行非时区对时，即时区为0 */
static int SntpSyncTimeNoTimeZone(const char *pServerName, int nport)
{
    if (NULL == pServerName)
    {
        CSYS_ERR("name is NULL!\n");
        return -1;
    }

    STNP_Header HeaderSNTP;  
    time_t t1, t2, t3, t4, dis;  
    memset(&HeaderSNTP,0,sizeof(HeaderSNTP));
    time(&t1);  
    t1 += JAN_1970;  
      
    // DBG_LOG("sync time from %s\n", pServerName);  

    if(GetNTPTime(&HeaderSNTP, pServerName, nport) < 0)
    {
        CSYS_ERR("Fail to call GetNTPTime!\n");
        return -1;  
    }
    
    time(&t4);  
    t4 += JAN_1970;  
    
    t2 = ntohl(HeaderSNTP.RecvTimeInt);  
    t3 = ntohl(HeaderSNTP.TranTimeInt);  

	if (t4 - t1 > 30)//解决可能出现的2023问题，由于IPC和NVR都在对时，导致t4时间与t1时间异常，正常情况下t4比t1不会大多少
	{
		CSYS_ERR("t4 greater than t1 30 secends\n");
		return -1;
	}

    dis = ((t2-t1) + (t3-t4)) / 2;
    
    if(dis <= 0)
    {
        CSYS_INF("local time is faster then server %d seconds\n", (int)-dis);  
    }
    else
    {
        CSYS_INF("local time is slower then server %d seconds\n", (int)dis);  
    }
    
    struct timeval tv;  
    char tBuf[64] = { 0 };
    gettimeofday(&tv, 0);  
    tv.tv_sec += dis;  
    ctime_r(&tv.tv_sec, tBuf);
    CSYS_INF("tBuf: %s", tBuf);

    if (settimeofday(&tv, NULL))
    {
    	CSYS_ERR("ServerName:%s; Fail to call settimeofday: %s\n", pServerName, strerror(errno));
        return -1;  
    }
    
    return 0;  
}  

/*@prief     DoSntpTimeZone 对时的时候需要处理时区
 *@param[in] host_name ntp主机名
 *@param[in] time_zone 时区
 *@return 0对是成功; -1对时失败
 *@note 时区支持整数和负数
 */
static int DoSntpTimeZone(const char *host_name, int time_zone)
{
	if (NULL == host_name)
    {
        CSYS_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }
    
    int ret = -1;
    
    struct timeval curTime;
    /* SNTP对时 */
    ret = SntpSyncTimeNoTimeZone(host_name, 123);
    if (ret)
    {
        CSYS_ERR("Fail to Called SyncTime!\n");
        return -1;
    }
    
    /* 时区计算 */    
    gettimeofday(&curTime, NULL);
    //curTime.tv_sec = curTime.tv_sec + 3600*time_zone;
    curTime.tv_sec = curTime.tv_sec + time_zone;
    if (settimeofday(&curTime, NULL))
    {
    	mysystem("date -R");
    	CSYS_ERR("ServerName:%s; Fail to call settimeofday: %s\n", host_name, strerror(errno));
    	return -1;
    }
	mysystem("date -R");

    return 0;
}

/* 如果进行指定的服务器ntp对时失败，则需要从配置文件获取服务器进行对时 */
/* 如果进行指定的服务器ntp对时失败，则需要从配置文件获取服务器进行对时 */
static int RunNtpBack(int nTimeZone)
{
    int ret = 0;    
    FILE *fp = NULL;
    char buffer[1024] = {0};

    fp = fopen(NTP_SERVER, "r");
    if (NULL == fp)
    {
    	CSYS_ERR("Fail to open %s: %s\n", NTP_SERVER, strerror(errno));
        return -1;
    }
    
    /* 从配置文件逐行获取ntp服务器，放在数组里面 */
    while(!feof(fp))
    {
        memset(buffer, 0, sizeof(buffer));
        fgets(buffer, sizeof(buffer), fp);

		if (strlen(buffer) <= 1)
			continue;
		
		if (buffer[strlen(buffer)-1] == '\r' || buffer[strlen(buffer)-1] == '\n')
			buffer[strlen(buffer)-1] = '\0';
		
        ret = DoSntpTimeZone(buffer, nTimeZone);
        if (0 == ret)
        {
            CSYS_ERR("ntpserver: %s. sntp sync time successful!\n", buffer);
            break;
        }
    }

    fclose(fp);
    fp = NULL;

    return ret;
}

/*@prief     SyncNtpDate 利用sntp协议进行ntp对时
 *@param[in] host_name ntp主机名
 *@param[in] time_zone 时区
 *@return 0对是成功; -1对时失败
 *@note 时区支持整数和负数
 *对时原则: 首先是利用指定服务器对时，如果对时失败，则使用备份的配置文件里面的服务器进行对时
 *          首先逐一进行对时，成功则跳出，否则进行下一个，直至测试完毕为止
 */
int SyncNtpDate(const char *host_name, int time_zone)
{
	if (NULL == host_name)
    {
        CSYS_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if (DoSntpTimeZone(host_name, time_zone))
    {
        CSYS_INF("Sync Ntp Date Fail! start to run the ntp servers back!\n");
        if (RunNtpBack(time_zone))
        {
            CSYS_ERR("Run Ntp Back Fail!\n");            
            return -1;
        }
    }
    return 0;
}
