#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <netinet/ip_icmp.h>

#include "ping.h"
// #include "sys_banned.h"
#include "global_gather.h"


/* ICMP报文发送与接收缓存 */
static unsigned char aucSendBuf[MAX_ALLOC_SIZE] = {0};
static unsigned char aucRecvBuf[MAX_ALLOC_SIZE] = {0};

/* 结构体定义 */
typedef struct tagIcmpStatic
{
    unsigned int uiSendPktNum;
    unsigned int uiRcvPktNum;
    unsigned int uiLlegalMsgNum;
    float fMinTime;
    float fMaxTime;
    float fArgTime;
}ICMP_STATIC_S;

/* 全局数据结构 */
ICMP_STATIC_S g_stPktStatic;      /* ICMP报文统计 */
struct timeval stSendTime = {0};  /* ECHO-REQUEST报文发送时间 */
struct timeval stRcvTime ={0};    /* ECHO-REPLY报文接收时间 */

/* 输出报文统计信息 */
int showStatic(const ICMP_STATIC_S *pstStInfo)
{
    if (NULL == pstStInfo)
    {
        fprintf(stderr, "showStatic: NULL POINTER EXCEPTION!\n");
        return -1;
    }

    unsigned int uiSend = 0;
    unsigned int uiRecv = 0;

    uiSend = pstStInfo->uiSendPktNum;
    uiRecv = pstStInfo->uiRcvPktNum;
    
    printf("***PING Statistics***");
    printf("Packets:Send = %u,Recveived = %u,Lost = %u", 
            uiSend, uiRecv, uiSend - uiRecv);
    printf("Time:Minimum = %.1fms,Maximum = %.1fms,Average=%.2fms\n", 
            pstStInfo->fMinTime, pstStInfo->fMaxTime, pstStInfo->fArgTime);
    return 0;
 }

/* 计算时间差,返回时间以毫秒为单位 */
unsigned int timeSub(const struct timeval *pstOut, const struct timeval *pstIn)
{
    if (NULL==pstOut || NULL==pstIn)
    {
        fprintf(stderr, "timeSub: NULL POINTER EXCEPTION!\n");
        return -1;
    }
    
    unsigned int uiSec = 0;
    int iUsec = 0;

    uiSec = pstOut->tv_sec - pstIn->tv_sec;
    iUsec = pstOut->tv_usec - pstIn->tv_usec;
    if (0 > iUsec)
    {
        iUsec += 1000000;
        uiSec--;
    }
    return uiSec * 1000 + (unsigned int)(iUsec / 1000);
}

/* 校验和计算 */
unsigned short calcIcmpChkSum(const void *pPacket, int iPktLen)
{
    if (NULL == pPacket)
    {
        fprintf(stderr, "calcIcmpChkSum: NULL POINTER EXCEPTION!\n");
        return -1;
    }

    unsigned short usChkSum   = 0;
    const unsigned short *pusOffset = NULL;

    pusOffset = (const unsigned short *)pPacket;
    while(1 < iPktLen)
    {
        usChkSum += *pusOffset++;
        iPktLen -= sizeof(unsigned short);
    }
    if (1 == iPktLen)
    {
        usChkSum += *((const char *)pusOffset);
    }
    usChkSum = (usChkSum >> 16) + (usChkSum & 0xffff);
    usChkSum += (usChkSum >>16);

    return ~usChkSum;
}

/* ICMP报文填充 */
int newIcmpEcho(const int iPacketNum, const int iDataLen)
{
    struct icmp *pstIcmp = NULL;

    memset(aucSendBuf, 0, sizeof(aucSendBuf));
    
    pstIcmp = (struct icmp *)aucSendBuf;
    pstIcmp->icmp_type = ICMP_ECHO;
    pstIcmp->icmp_code = 0;
    pstIcmp->icmp_seq = htons((unsigned short)iPacketNum);
    pstIcmp->icmp_id = htons((unsigned short)getpid());
    pstIcmp->icmp_cksum = 0;
    pstIcmp->icmp_cksum = calcIcmpChkSum(pstIcmp, iDataLen + 8);
    
    return iDataLen + 8;
}

/* 解析ECHO-REPLY响应报文 */
int parseIcmp(const struct sockaddr_in *pstFromAddr, char *pRecvBuf, const int iLen)
{
    if (NULL==pstFromAddr || NULL==pRecvBuf)
    {
        fprintf(stderr, "parseIcmp: NULL POINTER EXCEPTION!\n");
        return -1;
    }
    
    int iIpHeadLen = 0;
    int iIcmpLen = 0;
    struct ip *pstIp = NULL;
    struct icmp *pstIcmpReply = NULL;
    char str[32] = { 0 };
    pstIp = (struct ip *)pRecvBuf;
    iIpHeadLen = pstIp->ip_hl << 2;
    pstIcmpReply = (struct icmp *)(pRecvBuf + iIpHeadLen);
    /* 报文长度非法 */
    iIcmpLen = iLen - iIpHeadLen;
    if (8 > iIcmpLen)
    {
        printf("[Error]Bad ICMP Echo-reply\n");
        return -1;
    }
    /* 报文类型非法 */
    if ((pstIcmpReply->icmp_type != ICMP_ECHOREPLY) ||
        (pstIcmpReply->icmp_id != htons((unsigned short)getpid())))
    {
        fprintf(stderr, "Illegal data message num = %d!\n", ++g_stPktStatic.uiLlegalMsgNum);
        return -1;
    }
    sleep(1);
    printf("%d bytes reply from %s: icmp_seq=%u Time=%dms TTL=%d\n", iIcmpLen, 
    inet_ntop(AF_INET, &pstFromAddr->sin_addr, str, sizeof(str)), ntohs(pstIcmpReply->icmp_seq), timeSub(&stRcvTime, &stSendTime), pstIp->ip_ttl);

    return 1;
}

/* Echo响应报文接收 */
int recvIcmp(const int fd)
{
    if (fd < 0)
    {
        fprintf(stderr, "Invaild fd!\n");
        return -1;
    }

    int iRet = 0;
    int iRecvLen = 0;
    unsigned int uiInterval = 0;
    socklen_t fromLen = sizeof(struct sockaddr_in);
    struct sockaddr_in stFromAddr = {0};

    /* 清空接收缓存，并准备接收响应报文 */
    memset(aucRecvBuf, 0, MAX_ALLOC_SIZE);
    iRecvLen = recvfrom(fd, (void *)aucRecvBuf, sizeof(aucRecvBuf), 0, (struct sockaddr *)&stFromAddr,&fromLen); 
    gettimeofday(&stRcvTime, NULL);
    if (iRecvLen < 0)
    {
        if (EAGAIN == errno)
        {
            /* 请求超时 */
//          printf("Request time out.\n");
            g_stPktStatic.fMaxTime = ~0;
        }
        else
        {
            /* 接收数据包出错 */
            fprintf(stderr, "Fail to recv ICMP: %s\n", strerror(errno));
        }
        return -1;
    }
    /* 获取统计参数 */
    g_stPktStatic.uiRcvPktNum++;
    uiInterval = timeSub(&stRcvTime, &stSendTime);
    g_stPktStatic.fArgTime = (g_stPktStatic.fArgTime * (g_stPktStatic.uiSendPktNum - 1) + uiInterval)/g_stPktStatic.uiSendPktNum;
    if (uiInterval < g_stPktStatic.fMinTime)
    {
        g_stPktStatic.fMinTime = uiInterval;
    }
    if (uiInterval > g_stPktStatic.fMaxTime)
    {
        g_stPktStatic.fMaxTime = uiInterval;
    }
    /* 解析ICMP响应报文 */
    iRet = parseIcmp(&stFromAddr, (char *)aucRecvBuf, iRecvLen);
    if (0 > iRet)
    {
        return -1;
    }
    
    return 0;
}

/* 发送ICMP报文 */
int sendIcmp(const int fd, const struct sockaddr_in *pstDestAddr)
{
    if (NULL == pstDestAddr)
    {
        fprintf(stderr, "sendIcmp: NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if (fd < 0)
    {
        fprintf(stderr, "Invaild fd!\n");
        return -1;
    }
    
    unsigned char ucEchoNum = 0;
    int iPktLen = 0;
    int iRet    = 0;

    while(ICMP_ECHO_MAX > ucEchoNum)
    {
        iPktLen = newIcmpEcho(ucEchoNum, ICMP_DATA_LEN); 
        /* 记录发送起始时间 */
        g_stPktStatic.uiSendPktNum++;
        gettimeofday(&stSendTime, NULL);
        /* 发送ICMP-ECHO报文 */
        iRet = sendto(fd, (const void *)aucSendBuf, iPktLen, 0, (const struct sockaddr *)pstDestAddr, sizeof(struct sockaddr_in));
        if(0 > iRet)
        {
            fprintf(stderr, "Fail to sendto ICMP: %s\n", strerror(errno));
            /* 为了防止死循环 */
            ucEchoNum++;
            continue;
        }
        /* 等待接收响应报文 */
        recvIcmp(fd);
        ucEchoNum++;
    }
    return 0;
}

/* 进行域名解析(不使用gethostbyname) */
static int GetHostByName(const char *host_name, unsigned int *host_addr)
{
    if (NULL==host_name || NULL==host_addr)
    {
        fprintf(stderr, "hostname_to_ip: NULL POINTER EXCEPTION!\n");
        return -1;
    }

    struct addrinfo  hints;
    struct addrinfo *result = NULL;
    struct addrinfo *result_pointer = NULL;    
    char host_ip[64] = {0};
    int ret = -1;

    /* obtaining address matching host */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_CANONNAME;
    hints.ai_protocol = 0;  /* any protocol */

    ret = getaddrinfo(host_name, NULL, &hints, &result);
    if (ret)
	{
        fprintf(stderr, "getaddrinfo: %s host=[%s]\n", strerror(errno), host_name);
        return -1;
    }

    /* traverse the returned list and output the ip addresses */
    for (result_pointer=result; result_pointer!=NULL; result_pointer=result_pointer->ai_next) 
	{
	    memset(host_ip, 0, sizeof(host_ip));
        ret = getnameinfo(result_pointer->ai_addr, result_pointer->ai_addrlen, host_ip, sizeof(host_ip), NULL, 0, NI_NUMERICHOST);
        if (ret != 0) 
		{
            fprintf(stderr, "error in getnameinfo: %s\n", strerror(errno));
            continue;
        } 
		else 
        {
            printf("host name [%s] to ip addr [%s] successful!\n", host_name, host_ip);
            if (NULL != result)
            {
			    freeaddrinfo(result);
            }
			*host_addr = inet_addr(host_ip);
			return 0;
        }
    }

    if (NULL != result)
    {
        freeaddrinfo(result);
    }
    return -1;
}

/*@ brief ping 函数实现Ping命令(支持绑定网卡)
 *@ param[in] ifname 针对的网卡名字
 *@ param[in] host_addr 主机名或者IP地址
 *@ return 1 ping成功; 0 ping失败 -1:错误
 */
int ping(const char * ifname, const char *host_addr)
{
    if (!ifname || !host_addr)
    {
        fprintf(stderr, "ping: NULL POINTER EXCEPTION!\n");
        return -1;
    }

    int fd = -1;
    int iRcvBufSize = MAX_ALLOC_SIZE;
    unsigned int s_addr = 0;
    struct sockaddr_in stDestAddr = {0};
    struct protoent *pProtoIcmp = NULL;
    struct ifreq interface;
    in_addr_t stHostAddr;
    
    g_stPktStatic.uiSendPktNum = 0;
    g_stPktStatic.uiRcvPktNum =0;
    g_stPktStatic.fMinTime = 1000000.0;
    g_stPktStatic.fMaxTime = -1.0;
    g_stPktStatic.fArgTime = 0.0;
    g_stPktStatic.uiLlegalMsgNum = 0;

    /* 获取ICMP协议类型  */
    pProtoIcmp = getprotobyname("icmp");//这里文件系统里/etc目录下需要放protocols文件，找不到该文件会ping不通网络
    if (NULL == pProtoIcmp)
    {
        fprintf(stderr, "Fail to getprotobyname: %s\n", strerror(errno));
        return -1;
    }
    
    /* 创建ICMP使用的SOCKET */
    fd = socket(PF_INET, SOCK_RAW, pProtoIcmp->p_proto);
    if (0 > fd)
    {
        fprintf(stderr, "Fail to socket: %s\n", strerror(errno));
        return -1;
    }

    /* 绑定网卡 */
    strncpy(interface.ifr_ifrn.ifrn_name, ifname, IFNAMSIZ - 1);
    if (setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, (char *)&interface, sizeof(interface)) < 0)
    {
        fprintf(stderr, "Fail to bind interface %s: %s\n", ifname, strerror(errno));
        close(fd);
        return -1;
    }

#if 0
    /* ROOT权限回收 */
    if (setuid(getuid()) < 0)
    {
        fprintf(stderr, "Fail to setuid: %s\n", strerror(errno));
        close(fd);
        return -1;
    }
#endif
    
    /* 设置SOCKET选项 */
    struct timeval timeout = {ICMP_REQUEST_TIMEOUT, 0}; // ICMP_REQUEST_TIMEOUT                                                                   
    setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &iRcvBufSize, sizeof(iRcvBufSize));
    setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

    /* 地址解析 */
    stHostAddr = inet_addr(host_addr);
    if (INADDR_NONE == stHostAddr)
    {
        /* 根据主机名称解析IP地址 */
        if (-1 == GetHostByName(host_addr, &s_addr))
        {
            fprintf(stderr, "Fail to called GetHostByName: %s", host_addr);
            close(fd);
            return -1;
        }
        stDestAddr.sin_addr.s_addr = s_addr;
    }
    else
    {
        memcpy((char *)&stDestAddr.sin_addr, (char *)&stHostAddr, sizeof(stHostAddr));
    }

    /* 发送ICMP报文 */
    sendIcmp(fd, &stDestAddr);
    close(fd);
    
 // showStatic(&g_stPktStatic);
    
    /* 以下判断为经验值，需要进一步验证 */
    if ((g_stPktStatic.uiRcvPktNum > 0)                                   // 有接收到数据
     && (g_stPktStatic.uiRcvPktNum >= g_stPktStatic.uiSendPktNum>>1)      // 接收到的数据不能太少
     && (g_stPktStatic.uiLlegalMsgNum <= g_stPktStatic.uiSendPktNum>>1))  // 非法的报文数不能太多
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/* ICMP报文发送与接收缓存 */
static unsigned char myaucSendBuf[MAX_ALLOC_SIZE] = {0};
static unsigned char myaucRecvBuf[MAX_ALLOC_SIZE] = {0};

struct timeval mystSendTime = {0};  /* ECHO-REQUEST报文发送时间 */
struct timeval mystRcvTime ={0};    /* ECHO-REPLY报文接收时间 */


/* 解析ECHO-REPLY响应报文 */
int myParseIcmp(const struct sockaddr_in *pstFromAddr, char *pRecvBuf, const int iLen)
{
    if (NULL==pstFromAddr || NULL==pRecvBuf)
    {
        fprintf(stderr, "parseIcmp: NULL POINTER EXCEPTION!\n");
        return -1;
    }
    
    int iIpHeadLen = 0;
    int iIcmpLen = 0;
	char str[32] = { 0 };
    struct ip *pstIp = NULL;
    struct icmp *pstIcmpReply = NULL;

    pstIp = (struct ip *)pRecvBuf;
    iIpHeadLen = pstIp->ip_hl << 2;
    pstIcmpReply = (struct icmp *)(pRecvBuf + iIpHeadLen);
    /* 报文长度非法 */
    iIcmpLen = iLen - iIpHeadLen;
    if (8 > iIcmpLen)
    {
        printf("[Error]Bad ICMP Echo-reply\n");
        return -1;
    }
    /* 报文类型非法 */
    if ((pstIcmpReply->icmp_type != ICMP_ECHOREPLY) ||
        (pstIcmpReply->icmp_id != htons((unsigned short)getpid())))
    {
    	fprintf(stderr, "Illegal data message num = %d!\n", ++g_stPktStatic.uiLlegalMsgNum);
        return -1;
    }

	printf("%d bytes reply from %s: icmp_seq=%u Time=%dms TTL=%d\n", iIcmpLen, inet_ntop(AF_INET, &pstFromAddr->sin_addr, str, sizeof(str)), ntohs(pstIcmpReply->icmp_seq), timeSub(&stRcvTime, &stSendTime), pstIp->ip_ttl);

    return 1;
}



/* ICMP报文填充 */
int myNewIcmpEcho(const int iPacketNum, const int iDataLen)
{
    struct icmp *pstIcmp = NULL;

    memset(myaucSendBuf, 0, sizeof(myaucSendBuf));
    
    pstIcmp = (struct icmp *)myaucSendBuf;
    pstIcmp->icmp_type = ICMP_ECHO;
    pstIcmp->icmp_code = 0;
    pstIcmp->icmp_seq = htons((unsigned short)iPacketNum);
    pstIcmp->icmp_id = htons((unsigned short)getpid());
    pstIcmp->icmp_cksum = 0;
    pstIcmp->icmp_cksum = calcIcmpChkSum(pstIcmp, iDataLen + 8);
    
    return iDataLen + 8;
}


/* Echo响应报文接收 */
int myRecvIcmp(const int fd)
{
    if (fd < 0)
    {
        fprintf(stderr, "Invaild fd!\n");
        return -1;
    }

    int iRet = 0;
    int iRecvLen = 0;
    socklen_t fromLen = sizeof(struct sockaddr_in);
    struct sockaddr_in stFromAddr = {0};

    /* 清空接收缓存，并准备接收响应报文 */
    memset(myaucRecvBuf, 0, MAX_ALLOC_SIZE);
    iRecvLen = recvfrom(fd, (void *)myaucRecvBuf, sizeof(myaucRecvBuf), 0, (struct sockaddr *)&stFromAddr,&fromLen); 
	gettimeofday(&mystRcvTime, NULL);

    if (iRecvLen < 0)
    {
        if (EAGAIN == errno)
        {
            /* 请求超时 */
            printf("Request time out.\n");
        }
        else
        {
            /* 接收数据包出错 */
            fprintf(stderr, "Fail to recv ICMP: %s\n", strerror(errno));
        }
        
        return 0;
    }
    
    /* 解析ICMP响应报文 */
    iRet = myParseIcmp(&stFromAddr, (char *)myaucRecvBuf, iRecvLen);
    if (0 > iRet)
    {
        return 0;
    }
    
    return 1;
}



/* 发送ICMP报文 */
int mySendIcmp(const int fd, const struct sockaddr_in *pstDestAddr)
{
    if (NULL == pstDestAddr)
    {
        fprintf(stderr, "sendIcmp: NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if (fd < 0)
    {
        fprintf(stderr, "Invaild fd!\n");
        return -1;
    }
    
    int iPktLen = 0;
    int iRet    = 0;

    iPktLen = myNewIcmpEcho(0, ICMP_DATA_LEN); 
    gettimeofday(&mystSendTime, NULL);

    /* 发送ICMP-ECHO报文 */
    iRet = sendto(fd, myaucSendBuf, iPktLen, 0, (const struct sockaddr *)pstDestAddr, sizeof(struct sockaddr_in));
    if(0 > iRet)
    {
        return 0;
    }
    
    /* 等待接收响应报文 */
    return myRecvIcmp(fd);
}



/*@ brief myping 函数实现Ping命令(支持绑定网卡)
 *@ param[in] ifname 针对的网卡名字
 *@ param[in] host_addr 主机名或者IP地址
 *@ return 1 ping成功; 0 ping失败 -1:错误
 */
int myping(const char * ifname, const char *host_addr)
{
    if (!ifname || !host_addr)
    {
        fprintf(stderr, "ping: NULL POINTER EXCEPTION!\n");
        return -1;
    }

    int fd = -1;
    int iRcvBufSize = MAX_ALLOC_SIZE;
    unsigned int s_addr = 0;
    struct sockaddr_in stDestAddr = {0};
    struct ifreq interface;
    in_addr_t stHostAddr;
    int ret = 0;

	//1 是 ICMP
    fd = socket(PF_INET, SOCK_RAW, 1);
    if (0 > fd)
    {
        fprintf(stderr, "Fail to socket: %s\n", strerror(errno));
        return -1;
    }

    /* 绑定网卡 */
    //strncpy(interface.ifr_ifrn.ifrn_name, ifname, IFNAMSIZ);
    strncpy(interface.ifr_name, ifname, IFNAMSIZ);
    if (setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, (char *)&interface, sizeof(interface)) < 0)
    {
        fprintf(stderr, "Fail to bind interface %s: %s\n", ifname, strerror(errno));
        close(fd);
        return -1;
    }

#if 0
    /* ROOT权限回收 */
    if (setuid(getuid()) < 0)
    {
        fprintf(stderr, "Fail to setuid: %s\n", strerror(errno));
        close(fd);
        return -1;
    }
#endif

    /* 设置SOCKET选项 */
    struct timeval timeout = {ICMP_REQUEST_TIMEOUT, 0}; // ICMP_REQUEST_TIMEOUT                                                                   
    setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &iRcvBufSize, sizeof(iRcvBufSize));
    setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
    setsockopt(fd, SOL_SOCKET, SO_BROADCAST, (const void *)1, sizeof(int));

    /* 地址解析 */
    stHostAddr = inet_addr(host_addr);
    if (INADDR_NONE == stHostAddr)
    {
        /* 根据主机名称解析IP地址 */
        if (-1 == GetHostByName(host_addr, &s_addr))
        {
            fprintf(stderr, "Fail to called GetHostByName: %s", host_addr);
            close(fd);
            return -1;
        }
        stDestAddr.sin_addr.s_addr = s_addr;
    }
    else
    {
        memcpy((char *)&stDestAddr.sin_addr, (char *)&stHostAddr, sizeof(stHostAddr));
    }

    /* 发送ICMP报文 */
    ret = mySendIcmp(fd, &stDestAddr);
    close(fd);

	return ret;
}

