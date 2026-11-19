/*****************************************************************************
 * Copyright (C) 2022 All rights reserved.
 *  文件名称：4g_mode.c
 *  创 建 者：
 *  邮    箱：
 *  创建日期：2022年05月11日 星期三
 *  描    述：4G模块相关操作具体实现
 *          
 *   
 ******************************************************************************/
#ifdef __MODULE_4G_SUPPORT__

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <net/if.h>
#include <netinet/in.h>
#include <dirent.h>
#include <pthread.h>
#include <arpa/nameser.h>
#include <resolv.h>
#undef _GNU_SOURCE

#include "mysystem.h"
#include "zwsdk.h"
#include "4g_mode.h"
#include "global_macro.h"
#include "LogManager.h"
#include "common_pub.h"
#include "pkg_interface.h"
#include "inifile.h"
#include "watch_api.h"
#include "ping.h"
#include "md5Lib.h"
#ifdef __SUPPORT_SAVE_TF_LOG__
#include "tflog.h"
#endif
#include "common_pub.h"

/* 消除本文件中的snprintf截断警告 */
#pragma GCC diagnostic ignored "-Wformat-truncation"

//未插入sim卡时，提示字符串
#define NO_SIMCARD		"No Sim Card"
#define COMMAND_SYNC	"sync"

// 语音播放超时时间
#define SPEECH_PLAY_TIME_OUT_SEC    (10)
#define BYTE_PER_SEND 64

#define TEST_SIM_CSQ        "/tmp/app/disk/mmcblk0p1/output/sim_csq"
#define DONT_CHANGESIM_FLAG	"/mnt/mtd/config/.DontChangSIM"			//由于网络连接失败。重启4G模组前切换SIM， 创建标记，重启模组后不需要再去切卡
#define PRODUCTTEST_SIM     "/tmp/app/disk/mmcblk0p1/output/PTestSim"
#define SIM_INFO_PATH       "/mnt/mtd/config/SIMInfo"       //SIM卡信息：Imei;Version;Imsi;Iccid;ACT;Oper;Band;Csq;

static int CGSN_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len);
static int CIMI_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len);
static int COPS_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len);
static int ICCID_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len);
static int CREG_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len);
static int CEREG_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len);
static int CGREG_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len);
static int CSQ_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len);
static int QNWINFO_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len);
static int QGMR_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len);
static int QENG_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len);
static int QNETDEVCTL_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len);
static int QCFG_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len);
static int NAT_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len);
static int FHWVERS_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len);
static int FCISCFG_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len);
static int NETDNS_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len);
static int UCWKMODE_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len);
static int UCWKMODESTATUS_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len);
static int SWITCHSIM_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len);
static int BAND_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len);
static int SETUCWKMODE_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len);
static int RSLPMode_Receive_Parse_handler(const char *at_cmd, const char *rece_buf, int rece_len);
static int Module4G_SetModuleInternetConnStatus(int status);
static int POWER_ON_SIM1_CARD_SLOT_STATUS(const char *at_cmd,const char *rece_buf,int rece_len);

//解析AT返回字符串回调函数
typedef int	(*RECEIVE_DATA_PARSE_CALLBACK)(const char *at_cmd,const char * rece_buf,int rece_len);

typedef struct {
	char cmd[32];
	RECEIVE_DATA_PARSE_CALLBACK handler;
}Receive_Parse_Map_t;

// 网络异常情况达到重启条件对应的次数
#define SIMCARDERRTIMES (20)
#define CREGERRTIMES (240)
#define DIALERRTIMES (10)
#define DHCPERRTIMES (10)
#define CSQERRTIMES  (3)

// 中云新安的欧版模组和国内模组硬件版本号前6位是相同的，第七位如果是A的话为国内，如果是E或者S的话为国外版本
#define ZYXAHardVersion "800SGL"
#define ZYXCHardVersion "800SGC"
#define ZYXAHardVersion2 "800VGL"
#define ZYXAHardVersion3 "800HGB"
#define ZYXAHardVersion4 "800VGC"
#define ZYXAHardVersion5 "800HGL"
#define ZYXAHardVersion6 "800HGC"

#define MODULE4G_ERR(format...)        LOG_E(LOG_MODULE_4G,format)
#define MODULE4G_WARN(format...)       LOG_W(LOG_MODULE_4G,format)
#define MODULE4G_INF(format...)        LOG_I(LOG_MODULE_4G,format)
#define MODULE4G_DBG(format...)        LOG_D(LOG_MODULE_4G,format)

static int s_4GNetErrTimes[MaxErr] = {0};

//ACT 网络制式
static char  s_Act_type[][MAX_ACT_LEN] = {
	"GSM",
	"Unknow",
	"UTRAN",
	"GSM W/EGPRS",
	"UTRAN W/HSDPA",
	"UTRAN W/HSUPA",
	"UTRAN W/HSDPA and HSUPA",
	"E-UTRAN",
	"UTRAN HSPA+"
};

//4G 控制结构
static G4_Control_Manager_t  s_4GHandle;

// 获取硬件版本号
static int GetModuleHardWareVersion(void);

//看门狗id
static int WatchDog_Handler = -1;
//喂狗计数器
static int WatchDog_Count = 0;

//模组类型
static int s_ModuleType = UNKNOW_MODULE;

//设备结点
static char szTtyDevice[32] = {0};

// 是否是SG模组
static int s_IsSGModule = 0;
const char *ModuleVersionType = "800SG";
const char *ModuleVersionType_1605 = "800VG";
const char *ModuleVersionType_HaiSi = "800HG";


static int resettime = 0;

//AT命令返回解析函数
static Receive_Parse_Map_t s_Receive_Pares_Map[] = {
	{"AT+CGSN",CGSN_Receive_Parse_handler},
	{"AT+CIMI",CIMI_Receive_Parse_handler},
	{"AT+COPS?",COPS_Receive_Parse_handler},
	{"AT+QSIMSTAT?",POWER_ON_SIM1_CARD_SLOT_STATUS},
	{"AT+QCCID",ICCID_Receive_Parse_handler},
	{"AT+CREG",CREG_Receive_Parse_handler},
	{"AT+CEREG",CEREG_Receive_Parse_handler},
	{"AT+CGREG",CGREG_Receive_Parse_handler},
	{"AT+CSQ",CSQ_Receive_Parse_handler},
	{"AT+QNWINFO",QNWINFO_Receive_Parse_handler},
	{"AT+QGMR",QGMR_Receive_Parse_handler},
	{"AT+QENG=\"SERVINGCELL\"",QENG_Receive_Parse_handler},
	{"AT+QNETDEVCTL?",QNETDEVCTL_Receive_Parse_handler},
	{"AT+QCFG=\"usbnet\"",QCFG_Receive_Parse_handler}, 
	{"AT+QCFG=\"nat\"",NAT_Receive_Parse_handler},
	{"AT+FCISCFG=RSLPMode", RSLPMode_Receive_Parse_handler},
	{"AT+FHWVER?",FHWVERS_Receive_Parse_handler}, 
	{"AT+FCISCFG",FCISCFG_Receive_Parse_handler},		
	{"AT*NETDNS",NETDNS_Receive_Parse_handler},
	{"AT^UCWKMODE?",UCWKMODE_Receive_Parse_handler},
	{"AT^UCVSIMSTATUS?",UCWKMODESTATUS_Receive_Parse_handler},
	{"AT+SWITCHSIM?",SWITCHSIM_Receive_Parse_handler},
	{"AT*BAND?",BAND_Receive_Parse_handler},
	{"AT^UCWKMODE=1",SETUCWKMODE_Receive_Parse_handler},
};

static SETCLOUDCARD_CAP_CB s_SetCloudCardCb = NULL;

//获取设备S_码 表示设备是否被绑定
static char s_code[128] = {0};

int Module4G_GetSystemHandleInfo(G4_Control_Manager_t **handle)
{
	*handle = &s_4GHandle;
	return 0;
}

// 重新实现一个sleep函数，不使用系统自带的
static int GprsSleep(const int sleeptime)
{
	if(sleeptime < 1)
	{
		MODULE4G_ERR("invalid GprsSleep param \n");
		return -1;
	}


	for (int nscount = 0; nscount < sleeptime; ++nscount)
	{
		for (int nucount = 0; nucount < 10; ++nucount)
		{
			usleep(100*1000);
		}
	}

	return 0;
}

#if 0
// 测试程序
#define GPRSERR_FILE "/config/usr/local/etc/4GErrType"


static int Get4GErrType(int *RetRoute)
{
	int nRet = 0;
	if (!RetRoute)
	{
		return -1;
	}

    if (0 == access(GPRSERR_FILE, F_OK))
    {
		s_4GHandle.uSimCardStatus[s_4GHandle.nCurSimCardIndex] = read_profile_int("4GNETERRTYPE", "SimCardErr", 1, GPRSERR_FILE);
		s_4GHandle.uCregStatus[s_4GHandle.nCurSimCardIndex] = read_profile_int("4GNETERRTYPE", "CregErr", 1, GPRSERR_FILE);
		s_4GHandle.nDialState = read_profile_int("4GNETERRTYPE", "DialErr", 1, GPRSERR_FILE);
		*RetRoute = 0;
    }
	return 0;
}
#endif

//获取S码
static int DeviceBind_GetSCode()
{
	int nRet = 0;
	char szSCodeInfo[128] = {0};
	char *p = NULL;

    if (0 == access(SCODE_FILE, F_OK))
    {
		if (read_profile_string("ERP", "SCode", szSCodeInfo, sizeof(szSCodeInfo), NULL, SCODE_FILE))
		{
			p = strchr(szSCodeInfo, '@');
			if (NULL != p)
			{
				memcpy(s_code, szSCodeInfo, p-szSCodeInfo);
			}
		}
    }
    
    return nRet;
}


/*@breif SetSpeechContent 播报语音
 *@param [IN] nSpeechType 语音类型
 *@       
 *@       
 */
static void SetSpeechContent(int nSpeechType)
{
	static unsigned char speedflag[(SPEECH_MAX)-(SPEECH_4G_SYS_RESET)+1] = {0};
	
	int nCounter = 0;

	if (nSpeechType < SPEECH_4G_SYS_RESET || nSpeechType > SPEECH_MAX)
	{
		return;
	}

	//控制语音播报只播报一次,后面触发就不播放了
	if (SPEECH_4G_CONNECTING_PLEASE_WAIT == nSpeechType)
	{
		if(3 <= speedflag[nSpeechType-SPEECH_4G_SYS_RESET])
		{
			return;
		}
	
	}
	//4g信号播报播叠音如99，此时允许重复播报
	else if (speedflag[nSpeechType-SPEECH_4G_SYS_RESET])
	{
		return;
	}

	
	if (SPEECH_4G_CONNECTING_PLEASE_WAIT == nSpeechType)
	{
		speedflag[nSpeechType-SPEECH_4G_SYS_RESET]++;
	}
	else
	{
		speedflag[nSpeechType-SPEECH_4G_SYS_RESET] = 1;
	}

	int nSupportBoardCast = 0;
	int ret = ZWSys_GetBroadCastEnable(&nSupportBoardCast);
	if((0 == strlen(s_code))  || ((ret == 0) && nSupportBoardCast))
	{
		ZWSys_SetSpeechContent(nSpeechType);
	    //循环查询，等待播放结束
	    nCounter = 0;
	    while (1)
	    {
	        GprsSleep(1);
	        if (0 == ZWSys_GetSpeechPlayingStatus())
	        {
	            break;
	        }
	        
	        if (++nCounter > SPEECH_PLAY_TIME_OUT_SEC)
	        {
	            nCounter = 0;
	            break;
	        }
	    }
    }
}


/*@breif ParaAccessLock 进入临界区
 *@       
 *@       MutexProductDataAccess
 */
static void ParaAccessLock()
{
	pthread_mutex_lock(&s_4GHandle.MutexParaAccess);
}


/*@breif ParaAccessUnLock 退出临界区
 *@       
 *@       
 */
static void ParaAccessUnLock()
{
	pthread_mutex_unlock(&s_4GHandle.MutexParaAccess);
}


/*@breif ParaProductDataAccessLock 进入临界区
 *@       
 *@       ParaProductDataAccessLock
 */
static void ParaProductDataAccessLock()
{
	pthread_mutex_lock(&s_4GHandle.MutexProductDataAccess);
}


/*@breif ParaProductDataAccessUnLock 退出临界区
 *@       
 *@       
 */
static void ParaProductDataAccessUnLock()
{
	pthread_mutex_unlock(&s_4GHandle.MutexProductDataAccess);
}

/*@breif SetSerialAttr 串口参数配置
 *@param [IN] serial_fd open打开的文件描述符
 *@param [IN] nspeed 波特率: 2400|4800|9600|115200
 *@param [IN] nBits 数据位数: 7|8
 *@param [IN] nEvent 奇偶校验: '0'|'E'|'N'
 *@param [IN] nStop 停止位:  1|2
 *@return 成功返回0
 *@       失败返回-1
 */
static int SetSerialAttr(int serial_fd, int nSpeed, int nBits, char nEvent, int nStop)  
{
    if (serial_fd < 0)
    {
        MODULE4G_ERR("serial fd is %d invaild!\n", serial_fd);
        return -1;
    }

    struct termios newttys1, oldttys1;  

    memset(&newttys1, 0, sizeof(newttys1));
    memset(&oldttys1, 0, sizeof(oldttys1));

    /* 保存原先串口配置 */
    if(tcgetattr(serial_fd, &oldttys1)!=0)         
    {  
        MODULE4G_ERR("Fail to called tcgetattr\n");  
        return -1;  
    }  

    /* CREAD 开启串行数据接收，CLOCAL并打开本地连接模式 */
    newttys1.c_cflag |= (CLOCAL | CREAD);

    /* 设置数据位数 */
    newttys1.c_cflag &= ~CSIZE;

    /* 选择数据位 */
    switch(nBits)
    {
    case 7:  
        newttys1.c_cflag |= CS7;  
        break;  
    case 8:  
        newttys1.c_cflag |= CS8;  
        break;  
    }  

    /* 设置校验位 */
    switch(nEvent)
    {
    case '0':       // 奇校验    
        newttys1.c_cflag |= PARENB;             // 开启奇偶校验    
        newttys1.c_iflag |= (INPCK | ISTRIP);   // INPCK打开输入奇偶校验；ISTRIP去除字符的第八个比特    
        newttys1.c_cflag |= PARODD;             // 启用奇校验(默认为偶校验)    
        break;  
    case 'E' :      // 偶校验    
        newttys1.c_cflag |= PARENB;             // 开启奇偶校验    
        newttys1.c_iflag |= ( INPCK | ISTRIP);  // 打开输入奇偶校验并去除字符第八个比特    
        newttys1.c_cflag &= ~PARODD;            // 启用偶校验；    
        break;
    case 'N':       // 关闭奇偶校验
        newttys1.c_cflag &= ~PARENB;
        break;
    }

    /* 设置波特率 */
    switch(nSpeed)         
    {  
    case 2400:
        cfsetispeed(&newttys1, B2400);     // 设置输入速度
        cfsetospeed(&newttys1, B2400);     // 设置输出速度
        break;  
    case 4800:  
        cfsetispeed(&newttys1, B4800);  
        cfsetospeed(&newttys1, B4800);  
        break;  
    case 9600:  
        cfsetispeed(&newttys1, B9600);  
        cfsetospeed(&newttys1, B9600);  
        break;  
    case 115200:  
        cfsetispeed(&newttys1, B115200);  
        cfsetospeed(&newttys1, B115200);  
        break;  
    default:  
        cfsetispeed(&newttys1, B9600);  
        cfsetospeed(&newttys1, B9600);  
        break;  
    }

    /* 设置停止位: 若停止位为1，则清除CSTOPB，若停止位为2，则激活CSTOPB */
    if( nStop == 1)                      
    {  
        newttys1.c_cflag &= ~CSTOPB;      // 默认为送一位停止位
    }  
    else if( nStop == 2)  
    {  
        newttys1.c_cflag |= CSTOPB;       // CSTOPB表示送两位停止位
    }  

    /* 设置最少字符和等待时间，对于接收字符和等待时间没有特别的要求时 */
    newttys1.c_cc[VTIME] = 1;             // 非规范模式读取时的超时时间；    
    newttys1.c_cc[VMIN]  = 1;             // 非规范模式读取时的最小字符数；    

    /* tcflush清空终端未完成的输入/输出请求及数据；TCIFLUSH表示清空正收到的数据，且不读取出来 */
    tcflush(serial_fd, TCIFLUSH);

    /* 在完成配置后，需要激活配置使其生效 */
    if((tcsetattr(serial_fd, TCSANOW,&newttys1)) != 0) // TCSANOW不等数据传输完毕就立即改变属性
    {
        MODULE4G_ERR("Fail to called tcsetattr!\n");
        return -1;  
    }

    return 0;  
} 


/*@ preif OpenSerialPort 打开设备
 *@ param [in] ttyName 设备名
 *@ return -1 打开失败  
 *@        其他设备句柄
 */
static int OpenSerialPort(const char *ttyName)
{
    if (NULL == ttyName)
    {
        MODULE4G_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if (access(ttyName, F_OK))
    {
        MODULE4G_ERR("tty %s not exist!\n", ttyName);
        return -1;
    }

    int tty_fd = -1;

    tty_fd = open(ttyName, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (tty_fd < 0)
    {
        MODULE4G_ERR("Fail to open serial port %s!\n", ttyName);
    }
    else
    {
        MODULE4G_INF("Open serial port %s ok!\n", ttyName);
    }

    return tty_fd;
}


/*@ preif CloseSerialPort 关闭设备
 *@ param [in] tty_fd 设备句柄
 *@ return 
 *@        
 */
static void CloseSerialPort(int tty_fd)
{
    if (tty_fd > 0)
    {
        int ret = close(tty_fd);
		
		MODULE4G_INF("CloseSerialPort tty_fd=%d, ret=%d!\n", tty_fd, ret);
    }
}


/*@ preif SendData 发送数据
 *@ param [in] tty_fd 设备句柄
 *@ param [in] pBuf 发送缓冲区
 *@ param [in] nLength 发送缓冲区长度
 *@ return -1 发送失败 
 *@        nLen 实际发送长度
 */
static int SendData(int tty_fd, const char *pBuf, int nLength)
{
    if (tty_fd < 0)
    {
        MODULE4G_ERR("serial fd is %d invaild!\n", tty_fd);
        return -1;
    }

    // 还剩下多少数据没发送
    int nLeftLen = nLength;

    int nLen = 0;

    while (nLen < nLength)
    {
        int nWriting = 0;

        // 将要发送的长度
        nWriting = (nLeftLen > BYTE_PER_SEND) ? BYTE_PER_SEND : nLeftLen;

        // 剩余的长度
        nLeftLen = nLeftLen - nWriting;

        // 实际写入的长度
        int nWrited = write(tty_fd, pBuf + nLen, nWriting);

        // 写入失败
        if (nWrited < nWriting)
        {
            tcflush(tty_fd, TCOFLUSH);
            break;
        }
        // 总共发送的长度
        nLen += nWrited;
    }

    MODULE4G_INF("AT send cmd: %s\n",pBuf);

    // 实际发送的数据与要求发送的数据长度不同
    if (nLen != nLength)
    {
        MODULE4G_INF("Total Length = %d, Write Length = %d\n", nLength, nLen);
        return -1;
    }

    return nLen;
}


/*@ preif RecvData 接收数据
 *@ param [in] tty_fd 设备句柄
 *@ param [in] pBuf 接收缓冲区
 *@ param [in] nLength 接收缓冲区长度
 *@ return -1 接收失败 
 *@        nLen 实际接收数据长度
 */
static int RecvData(int tty_fd, void *pBuf, int nLength)
{
    if (tty_fd < 0)
    {
        MODULE4G_ERR("serial fd is %d invaild!\n", tty_fd);
        return -1;
    }

    int nRead = 0;
    fd_set fs_read;
    struct timeval tv_timeout;


    // 循环读取,最多读取缓冲区长度
    while (nRead < nLength)
    {
        FD_CLR(tty_fd, &fs_read);
        FD_ZERO(&fs_read);
        FD_SET(tty_fd, &fs_read);
        tv_timeout.tv_sec = SEND_WAIT_TIME;	        // TIMEOUT_SEC(nLength, m_dwBaudrate);
        tv_timeout.tv_usec = 0;	        // TIMEOUT_USEC;

        select(tty_fd + 1, &fs_read, NULL, NULL, &tv_timeout);
        if (FD_ISSET(tty_fd, &fs_read))
        {
            int nReadLen = read(tty_fd, (unsigned char *) pBuf + nRead, nLength - nRead);

            //没读到数据
            if (nReadLen <= 0)
            {
                break;
            }
            // 总共读到多少数据
            nRead += nReadLen;			
        }
        else
        {
            // 没等到数据
            return (int) nRead;
        }

		// 有ok 或者error 返回。模组已经成功返回指令数据
        if ((NULL != strstr(pBuf,"OK")) || (NULL != strstr(pBuf,"ERROR")))
        {	
			break;
        }
    }
    if (nRead <= 0)
    {
        usleep(50000);
    }

    return (int) nRead;
}


/*@ preif CheckModuleStartUpdate 检测模组是否可以下发升级文件
 *@ param [in] tty_fd 设备句柄
 *@ return 
 *@         -1  接收数据失败
 *@         0  未等待下发升级文件
 *@		    1  	等待下发升级文件
 */
static int CheckModuleStartUpdate(int tty_fd)
{
    if (tty_fd < 0)
    {
        MODULE4G_ERR("serial fd is %d invaild!\n", tty_fd);
        return -1;
    }
	char ReceBuf[64];
    int nRead = 0;
    fd_set fs_read;
    struct timeval tv_timeout;
    char *pBuf = &ReceBuf[0];
    int nLength = sizeof(ReceBuf);
    char *find_str = NULL;
    int ret = 0;
	int diffflag = 0;
	memset(ReceBuf,0,sizeof(ReceBuf));

	
    // 循环读取,最多读取缓冲区长度
    while (nRead < nLength)
    {
        FD_CLR(tty_fd, &fs_read);
        FD_ZERO(&fs_read);
        FD_SET(tty_fd, &fs_read);
        tv_timeout.tv_sec = 1;	        // 等待1s
        tv_timeout.tv_usec = 0;	        // TIMEOUT_USEC;

        select(tty_fd + 1, &fs_read, NULL, NULL, &tv_timeout);
        if (FD_ISSET(tty_fd, &fs_read))
        {
            int nReadLen = read(tty_fd, (unsigned char *) pBuf + nRead, nLength - nRead);

            //没读到数据
            if (nReadLen < 0)
            {
                return -1;
            }
            // 总共读到多少数据
            nRead += nReadLen;
			
			if(nRead <= 0 && 0 == diffflag)
			{
				MODULE4G_INF("nRead value :%d \n", nRead);
				mysystem("ls /dev/ttyUSB*");
				diffflag = 1;
			}
			
        }
        else
        {
            // 没等到数据
			MODULE4G_INF("no value will break  nRead: %d \n", nRead);
            break;
        }

		//有数据了
        if (NULL != strstr(pBuf,"\r\n"))
        {	
			break;
        }
    }

	if (0 == nRead)
	{
		//没有等到数据，认为没有在升级
		ret = 0;
	}
	else 
	{
		find_str = strstr(pBuf,"FILESTART");
	    if (NULL != find_str)
	    {
			MODULE4G_INF("FILESTART  ready !\n");
			ret = 1;
	    }
	    else
	    {
			MODULE4G_INF("no  update ! \n");
			ret = 0;
	    }
    }

    return ret;
}



/*@ preif CheckModuleUpdating 接收数据
 *@ param [in] tty_fd 设备句柄
 *@ return 
 *@         -1  接收数据失败
 *@         0  没有在升级
 *@			其他   正在升级，升级百分比
 */
static int CheckModuleUpdating(int tty_fd)
{
    if (tty_fd < 0)
    {
        MODULE4G_ERR("serial fd is %d invaild!\n", tty_fd);
        return -1;
    }
	char ReceBuf[64];
    int nRead = 0;
    fd_set fs_read;
    struct timeval tv_timeout;
    char *pBuf = &ReceBuf[0];
    int nLength = sizeof(ReceBuf);
    char *find_str = NULL;
    char *find_str1 = NULL;
    char strtmp[16];
    int ret = 0;

	memset(ReceBuf,0,sizeof(ReceBuf));
	
    // 循环读取,最多读取缓冲区长度
    while (nRead < nLength)
    {
        FD_CLR(tty_fd, &fs_read);
        FD_ZERO(&fs_read);
        FD_SET(tty_fd, &fs_read);
        tv_timeout.tv_sec = 5;	        // 等待5s
        tv_timeout.tv_usec = 0;	        // TIMEOUT_USEC;

        select(tty_fd + 1, &fs_read, NULL, NULL, &tv_timeout);
        if (FD_ISSET(tty_fd, &fs_read))
        {
            int nReadLen = read(tty_fd, (unsigned char *) pBuf + nRead, nLength - nRead);

            //没读到数据
            if (nReadLen < 0)
            {
                return -1;
            }
            // 总共读到多少数据
            nRead += nReadLen;
        }
        else
        {
            // 没等到数据
            break;
        }

		//有数据了
        if (NULL != strstr(pBuf,"\r\n"))
        {	
			break;
        }
    }

	if (0 == nRead)
	{
		//没有等到数据，认为没有在升级
		ret = 0;
	}
	else 
	{
		find_str = strstr(pBuf,"UPDATING");
	    if (NULL != find_str)
	    {
			//模组正在升级
			find_str1 = strstr(find_str,"\r\n");
			if (NULL != find_str1)
			{
				memset(strtmp,0,sizeof(strtmp));
				memcpy(strtmp,find_str+10,find_str1-find_str-10);
				ret = atoi(strtmp);
				MODULE4G_INF("Module Update: %d\n",ret);
			}
			else
			{
				//几乎不可能到这里
				ret = 1;
				MODULE4G_ERR("Updating str failed,no end str\n");
			}
	    }
	    else
	    {
			ret = 0;
	    }
    }

    return ret;
}


/*@breif Get_Msg_Next_Separator 使用指定分隔符分割消息
 *@param [IN] in_msg 源字符串
 *@param [OUT] out_msg 截取消息
 *@param [OUT] out_ptr 下一次源字符串
 *@       未找到sep返回0
 *@       入参有空指针返回-1
 *@       解析字符串长度过长-2
 *@       成功返回数据 1
 */
static int Get_Msg_Next_Separator(const char *in_msg,char *out_msg,char ** out_ptr,int maxlen,const char *sep)
{
	char *find_str = NULL;
	int msg_len = 0;

	if ((NULL == in_msg) || (NULL == out_msg) || (NULL == out_ptr))
	{
		MODULE4G_ERR("in_msg or out_msg or out_ptr is NULL!\n");
		return -1;
	}

	find_str = strstr(in_msg,sep);

	if (NULL == find_str)
	{
		MODULE4G_ERR("Get_Msg_Next_Separator 444\n");
		//结束
		*out_ptr = NULL;
		return 0;
	}

	msg_len = (int)(find_str - in_msg);

	if (msg_len > maxlen)
	{
		*out_ptr = NULL;
		MODULE4G_ERR("str len to long ! len = %d  maxlen = %d\n",msg_len,maxlen);
		return -2;
	}
	
	memcpy(out_msg,in_msg,msg_len);
	*out_ptr = find_str + strlen(sep);

	MODULE4G_INF("parse len:%d  str: %s\n",msg_len,out_msg);
	return 1;
}


/*@breif CGSN_Receive_Parse_handler CGSN命令返回数据处理
 *@param [IN] at_cmd 发送命令
 *@param [IN] rece_buf 返回字符串
 *@param [IN] rece_len 返回字符串长度
 *@       成功返回0
 *@       失败返回-1
 */
static int CGSN_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len)
{
	char outmsgbuf[128] = {0};
	char *next_str = NULL;
	const char *tmp_str = NULL;
	int ret = 0;

	char sImei[MAX_IMEI_LEN] = {0};
	if ((NULL == at_cmd) || (NULL == rece_buf))
	{
		return -1;
	}

	tmp_str = rece_buf;

	while(1)
	{
		memset(outmsgbuf,0,sizeof(outmsgbuf));
		ret = Get_Msg_Next_Separator(tmp_str,outmsgbuf,&next_str,sizeof(outmsgbuf),AT_MSG_SEPARATOR);
		if ((0 == ret) || (-1 == ret))
		{
			break;
		}

		tmp_str = next_str;

		if (1 == ret)
		{
			if ((strlen(outmsgbuf) > 0) && (NULL == strstr(outmsgbuf,AT_REPORT_SEPARATOR)))
			{
				ParaAccessLock();
				memcpy(s_4GHandle.sImei,outmsgbuf,MAX_IMEI_LEN-1);
				memcpy(sImei, s_4GHandle.sImei, sizeof(sImei));
				ParaAccessUnLock();
#ifdef __SUPPORT_SAVE_TF_LOG__ 
				TF_GPRS_LOG_INFO("4G module Imei: %s \n", sImei);
#endif
				break;
			}
		}
	}

	return 0;
}

/*@breif CIMI_Receive_Parse_handler CIMI命令返回数据处理
 *@param [IN] at_cmd 发送命令
 *@param [IN] rece_buf 返回字符串
 *@param [IN] rece_len 返回字符串长度
 *@       成功返回0
 *@       失败返回-1
 */
static int CIMI_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len)
{
	G4_Control_Manager_t *fd = NULL;
	Module4G_GetSystemHandleInfo(&fd);

	char outmsgbuf[128] = {0};
	char *next_str = NULL;
	const char *tmp_str = NULL;
	int ret = 0;

	if ((NULL == at_cmd) || (NULL == rece_buf))
	{
		return -1;
	}

	tmp_str = rece_buf;

	while(1)
	{
		memset(outmsgbuf,0,sizeof(outmsgbuf));
		ret = Get_Msg_Next_Separator(tmp_str,outmsgbuf,&next_str,sizeof(outmsgbuf),AT_MSG_SEPARATOR);
		if ((0 == ret) || (-1 == ret))
		{
			break;
		}

		tmp_str = next_str;

		if (1 == ret)
		{
			if ((strlen(outmsgbuf) > 0) && (NULL == strstr(outmsgbuf,AT_REPORT_SEPARATOR)))
			{
				ParaAccessLock();
				memcpy(fd->sImsi[fd->nCurSimCardIndex],outmsgbuf,MAX_IMSI_LEN-1);
				ParaAccessUnLock();
				MODULE4G_ERR("len:%d IMSI:%s\n",strlen(fd->sImsi[fd->nCurSimCardIndex]),fd->sImsi[fd->nCurSimCardIndex]);
				break;
			}
		}
	}

	return 0;
}

/*@breif COPS_Receive_Parse_handler COPS命令返回数据处理
 *@param [IN] at_cmd 发送命令
 *@param [IN] rece_buf 返回字符串
 *@param [IN] rece_len 返回字符串长度
 *@       成功返回0
 *@       失败返回-1
 */
static int COPS_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len)
{
	G4_Control_Manager_t *fd = NULL;
	Module4G_GetSystemHandleInfo(&fd);

	char *find_str = NULL;
	char *find_comma1 = NULL;
	char *find_comma2 = NULL;
	char *find_comma3 = NULL;
	char *find_str_end = NULL;
	char act_str[8];
	int act_type = 0;

	//计算分界符位置
	find_str = strstr(rece_buf,"+COPS:");
	if (NULL != find_str)
	{
		find_comma1 = strstr(find_str,",");
	}

	if (NULL != find_comma1)
	{
		find_comma2 = strstr(find_comma1+1,",");
	}

	if (NULL != find_comma2)
	{
		find_comma3 = strstr(find_comma2+1,",");
	}

	if (NULL != find_comma3)
	{
		find_str_end = strstr(find_comma3,"\r\n");
	}
	else if(NULL == find_comma3 && NULL != find_comma2 && ZHONGYUN_ZX800HG == s_ModuleType)
	{
		find_str_end = strstr(find_comma2,"\r\n");
	}
	
	if(ZHONGYUN_ZX800HG == s_ModuleType)
	{
		if (   (NULL == find_str)
		|| (NULL == find_comma1)
		|| (NULL == find_comma2)
		|| (NULL == find_str_end))
		{
			return -1;
		}
	}
	else
	{
		if (   (NULL == find_str)
				|| (NULL == find_comma1)
				|| (NULL == find_comma2)
				|| (NULL == find_comma3)
				|| (NULL == find_str_end))
			{
				return -1;
			}
	}
	memset(act_str,0,sizeof(act_str));
	if(NULL != find_comma3)
	memcpy(act_str,find_comma3+1,find_str_end-find_comma3-1);
	else if(NULL == find_comma3 && ZHONGYUN_ZX800HG == s_ModuleType)
	memcpy(act_str,find_comma2+1,find_str_end-find_comma2-1);

	
	act_type = atoi(act_str);

	ParaAccessLock();
	memset(fd->sOper[fd->nCurSimCardIndex],0,sizeof(fd->sOper[fd->nCurSimCardIndex]));
	

	if(NULL != find_comma3)
	memcpy(fd->sOper[fd->nCurSimCardIndex],find_comma2+2,find_comma3-find_comma2-3);
	else if(NULL == find_comma3 && ZHONGYUN_ZX800HG == s_ModuleType)
	memcpy(fd->sOper[fd->nCurSimCardIndex],find_comma2+2,find_str_end-find_comma2-3);
	if(strcmp(fd->sOper[fd->nCurSimCardIndex],"CHINA TELECOM") == 0)
	{
		MODULE4G_INF("sOper[%d] is [%s] , Should  changed CHN-CT\n",fd->nCurSimCardIndex,fd->sOper[fd->nCurSimCardIndex]);
		memset(fd->sOper[fd->nCurSimCardIndex],0,sizeof(fd->sOper[fd->nCurSimCardIndex]));
		strncpy(fd->sOper[fd->nCurSimCardIndex],"CHN-CT",MAX_OPER_LEN);
	}
	MODULE4G_INF("nCurSimCardIndex(%d)oper(%s)producOper(%s)\n", fd->nCurSimCardIndex,fd->sOper[fd->nCurSimCardIndex],fd->s_ProductTestData.sOper[fd->nCurSimCardIndex]);

	if (0 == strlen(fd->s_ProductTestData.sOper[fd->nCurSimCardIndex]))
	{
		MODULE4G_INF("producttest no have ops, now refresh ops, s_4GHandle.nCurSimCardIndex=%d\n", fd->nCurSimCardIndex);
		memset(fd->s_ProductTestData.sOper[fd->nCurSimCardIndex],0,sizeof(fd->s_ProductTestData.sOper[fd->nCurSimCardIndex]));
		memcpy(fd->s_ProductTestData.sOper[fd->nCurSimCardIndex],fd->sOper[fd->nCurSimCardIndex],sizeof(fd->s_ProductTestData.sOper[fd->nCurSimCardIndex]));
	}

	//获取act		
	memset(fd->sACT[fd->nCurSimCardIndex],0,sizeof(fd->sACT[fd->nCurSimCardIndex]));
	if (act_type < (sizeof(s_Act_type)/sizeof(s_Act_type[0])))
	{
		strncpy(fd->sACT[fd->nCurSimCardIndex],s_Act_type[act_type],MAX_ACT_LEN);
	}
	else
	{
		strncpy(fd->sACT[fd->nCurSimCardIndex],"UnKnow",MAX_ACT_LEN);
	}
	MODULE4G_INF("nCurSimCardIndex(%d)act(%s)\n", fd->nCurSimCardIndex,fd->sACT[fd->nCurSimCardIndex]);
	
	ParaAccessUnLock();
	

	return 0;
}

/*@breif ICCID_Receive_Parse_handler ICCID命令返回数据处理
 *@param [IN] at_cmd 发送命令
 *@param [IN] rece_buf 返回字符串
 *@param [IN] rece_len 返回字符串长度
 *@       成功返回0
 *@       失败返回-1
 */
static int ICCID_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len)
{
	G4_Control_Manager_t *fd = NULL;
	Module4G_GetSystemHandleInfo(&fd);

	char *find_str = NULL;
	char *find_end = NULL;

	if ((NULL == at_cmd) || (NULL == rece_buf))
	{
		fd->uSimCardStatus[fd->nCurSimCardIndex] = ABNORMAL;
		MODULE4G_ERR("NULL pointer !\n");
		return -1;
	}

	find_str = strstr(rece_buf,"+QCCID:");

	if (NULL != find_str)
	{
		find_end = strstr(find_str+strlen("+QCCID:"),"\r\n");
	}
	
	if (NULL == find_str || NULL == find_end)
	{
		fd->uSimCardStatus[fd->nCurSimCardIndex] = ABNORMAL;
		MODULE4G_ERR("NULL pointer !\n");
		return -1;
	}
	else
	{
		if ((find_end-find_str-strlen("+QCCID: ")) > MAX_ICCID_LEN)
		{
			fd->uSimCardStatus[fd->nCurSimCardIndex] = ABNORMAL;
			MODULE4G_ERR("s_4GHandle.uSimCardStatus is abnormal !\n");
			return -1;
		}
	
		ParaAccessLock();
		memcpy(fd->sIccid[fd->nCurSimCardIndex],find_str+strlen("+QCCID: "),find_end-find_str-strlen("+QCCID: "));
		if (0 != strlen(fd->sIccid[fd->nCurSimCardIndex]))
		{
			fd->uSimCardStatus[fd->nCurSimCardIndex] = NORMAL;
			MODULE4G_ERR("s_4GHandle.uSimCardStatus is normal ^_^ \n");
		}
		else
		{
			fd->uSimCardStatus[fd->nCurSimCardIndex] = ABNORMAL;
			MODULE4G_ERR("sIccid is NULL !\n");
		}
		ParaAccessUnLock();
	}

	return 0;
}

/*@breif AtReceiveHandler CREG命令返回数据处理
 *@param [IN] at_cmd 发送命令
 *@param [IN] rece_buf 返回字符串
 *@param [IN] rece_len 返回字符串长度
 *@       成功返回0
 *@       失败返回-1
 */
static int CREG_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len)
{
	G4_Control_Manager_t *fd = NULL;
	Module4G_GetSystemHandleInfo(&fd);

	char *find_str = NULL;

	if ((NULL == at_cmd) || (NULL == rece_buf))
	{
		return -1;
	}

	find_str = strstr(rece_buf,"+CREG:");
	if (NULL == find_str)
	{
		return -1;
	}
	else
	{
		ParaAccessLock();

		if (('1' == find_str[9]) || ('5' == find_str[9]))
		{
			fd->uCregStatus[fd->nCurSimCardIndex] = 1;
		}
		else
		{
			fd->uCregStatus[fd->nCurSimCardIndex] = 0;
		}
		
		ParaAccessUnLock();
	}

	return 0;
}

/*@breif AtReceiveHandler CEREG命令返回数据处理
 *@param [IN] at_cmd 发送命令
 *@param [IN] rece_buf 返回字符串
 *@param [IN] rece_len 返回字符串长度
 *@       成功返回0
 *@       失败返回-1
 */
static int CEREG_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len)
{
	G4_Control_Manager_t *fd = NULL;
	Module4G_GetSystemHandleInfo(&fd);

	char *find_str = NULL;

	if ((NULL == at_cmd) || (NULL == rece_buf))
	{
		return -1;
	}

	find_str = strstr(rece_buf,"+CEREG:");
	if (NULL == find_str)
	{
		return -1;
	}
	else
	{
		ParaAccessLock();

		if (('1' == find_str[10]) || ('5' == find_str[10]))
		{
			fd->uCregStatus[fd->nCurSimCardIndex] = 1;
		}
		else
		{
			fd->uCregStatus[fd->nCurSimCardIndex] = 0;
		}
		
		ParaAccessUnLock();
	}

	return 0;
}


/*@breif AtReceiveHandler CGREG命令返回数据处理
 *@param [IN] at_cmd 发送命令
 *@param [IN] rece_buf 返回字符串
 *@param [IN] rece_len 返回字符串长度
 *@       成功返回0
 *@       失败返回-1
 */
static int CGREG_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len)
{
	G4_Control_Manager_t *fd = NULL;
	Module4G_GetSystemHandleInfo(&fd);

	char *find_str = NULL;

	if ((NULL == at_cmd) || (NULL == rece_buf))
	{
		return -1;
	}

	find_str = strstr(rece_buf,"+CGREG:");
	if (NULL == find_str)
	{
		return -1;
	}
	else
	{
		ParaAccessLock();

		if (('1' == find_str[10]) || ('5' == find_str[10]))
		{
			fd->uCregStatus[fd->nCurSimCardIndex] = 1;
		}
		else
		{
			fd->uCregStatus[fd->nCurSimCardIndex] = 0;
		}
		
		ParaAccessUnLock();
	}

	return 0;
}



/*@breif Csq_Calc 计算csq值
 *@param [IN] csq 映射前值
 *@       根据手册计算后的值
 *@       
 */
static int Csq_Calc(int csq)
{
    int value = 0;

	#if 0
    if (csq >= 0 && csq <= 31)
    {
        value = 2*csq - 113;
    }
    #endif

    value = csq * 100 / 31;
    value = value > 100 ? 0 : value;
    value = value < 0 ? 0 : value;

	return value;
}



/*@breif CSQ_Receive_Parse_handler CSQ命令返回数据处理
 *@param [IN] at_cmd 发送命令
 *@param [IN] rece_buf 返回字符串
 *@param [IN] rece_len 返回字符串长度
 *@       成功返回0
 *@       失败返回-1
 */
static int CSQ_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len)
{
	G4_Control_Manager_t *fd = NULL;
	Module4G_GetSystemHandleInfo(&fd);

	char *find_str = NULL;
	char *find_str1 = NULL;
	char csq_str[8];
	int csq = 0;

	if ((NULL == at_cmd) || (NULL == rece_buf))
	{
		return -1;
	}

	find_str = strstr(rece_buf,"+CSQ:");

	if (NULL != find_str)
	{
		find_str1 = strstr(find_str,",");
	}
	
	if ((NULL == find_str) || (NULL == find_str1))
	{
		return -1;
	}
	else
	{
		memset(csq_str,0,sizeof(csq_str));

		//获取csq值
		memcpy(csq_str,find_str+6,find_str1-find_str-6);
		csq = atoi(csq_str);
		csq = Csq_Calc(csq);
	
		ParaAccessLock();
		fd->iCsq[fd->nCurSimCardIndex] = csq;

		//把当前在使用的sim卡的信号强度更新到产测结构体中，app页面去产测结构体里拿的话就是最新的sim卡信号强度
		s_4GHandle.s_ProductTestData.iCsq[fd->nCurSimCardIndex] = fd->iCsq[fd->nCurSimCardIndex];
				
		ParaAccessUnLock();
	}

	return 0;
}


/*@breif QNWINFO_Receive_Parse_handler SIMCROSS命令返回数据处理
 *@param [IN] at_cmd 发送命令
 *@param [IN] rece_buf 返回字符串
 *@param [IN] rece_len 返回字符串长度
 *@       成功返回0
 *@       失败返回-1
 */
static int QNWINFO_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len)
{
	G4_Control_Manager_t *fd = NULL;
	Module4G_GetSystemHandleInfo(&fd);

	char *find_str = NULL;
	char *find_comma0 = NULL;
	char *find_comma1 = NULL;
	char *find_comma2 = NULL;
	char *find_comma3 = NULL;

	if ((NULL == at_cmd) || (NULL == rece_buf))
	{
		return -1;
	}

	//计算分界符位置
	find_str = strstr(rece_buf,"+QNWINFO:");

	if (NULL != find_str)
	{
		find_comma0 = strstr(find_str,":");
	}

	if (NULL != find_comma0)
	{
		find_comma1 = strstr(find_comma0,",");
	}

	if (NULL != find_comma1)
	{
		find_comma2 = strstr(find_comma1+1,",");
	}

	if (NULL != find_comma2)
	{
		find_comma3 = strstr(find_comma2+1,",");
	}

	if (   (NULL == find_str)
		|| (NULL == find_comma0)
		|| (NULL == find_comma1)
		|| (NULL == find_comma2)
		|| (NULL == find_comma3))
	{
		return -1;
	}
	else
	{
		ParaAccessLock();
		/*若 SIM 卡为移动卡则需要判断频段信息*/
        // 提取 "FDD LTE" 部分
        char network_type[32] = {0};
        char *quote_start = strchr(find_comma0, '\"');  // 找到第一个引号
        if (quote_start)
        {
            char *quote_end = strchr(quote_start + 1, '\"');  // 找到第二个引号
            if (quote_end)
            {
                int len = quote_end - (quote_start + 1);
                if (len > 0 && len < sizeof(network_type))
                {
                    memcpy(network_type, quote_start + 1, len);
                    network_type[len] = '\0';
                    
                    //检查是否包含 "FDD LTE"
                    if (strstr(network_type, "FDD LTE") != NULL)
                    {
                    	if(strstr(fd->sOper[fd->nCurSimCardIndex],"CHINA MOBILE"))
                    	{
	                        fd->s_chinaMobileBand = 1;
	                        MODULE4G_INF("China Mobile Found FDD LTE network: %s\n", network_type);
                    	}
                    }
                    else
                    {
                       	if(strstr(fd->sOper[fd->nCurSimCardIndex],"CHINA MOBILE"))
                       	{
	                        fd->s_chinaMobileBand = 0;
	                        MODULE4G_INF("China Mobile TDD LTE network: %s\n", network_type);
                       	}
                    }
                }
            }
        }

		memset(fd->sBand[fd->nCurSimCardIndex],0,sizeof(fd->sBand[fd->nCurSimCardIndex]));
		memcpy(fd->sBand[fd->nCurSimCardIndex],find_comma2+2,find_comma3-find_comma2-3);
		ParaAccessUnLock();
	}

	return 0;
}

//通知sdk模组的类型
static int SetModuleTypeToSdk(const char *version)
{

	if (NULL == version)
	{
		return -1;
	}

	MODULE4G_INF("SetModuleTypeToSdk version =%s,s_ModuleType=%d\n", version, s_ModuleType);
	MODULE4G_INF("[harware] SetModuleTypeToSdk=%s\n", s_4GHandle.sHardVersion);

	if(ZHONGYUN_ZX1602 == s_ModuleType)
	{
		if((strlen(s_4GHandle.sHardVersion)) && (0 == strncmp(s_4GHandle.sHardVersion, ZYXAHardVersion, strlen(ZYXAHardVersion))))
		{
			if('S' == s_4GHandle.sHardVersion[6] || 'E' == s_4GHandle.sHardVersion[6])
			{
				MODULE4G_INF("[harware] is Eurp module\n");
				ZWSys_SetModuleType(ZHONGYUN_ZX800SG_S);
			}
            else if('N' == s_4GHandle.sHardVersion[6])
            {
                MODULE4G_INF("[harware] is Ameri module\n");
				ZWSys_SetModuleType(ZHONGYUN_ZX800SG_A);
            }
			else if('A' == s_4GHandle.sHardVersion[6])
			{
				MODULE4G_INF("[harware] is China module\n");

				MODULE4G_INF("ZWSys_SetModuleType ZHONGYUN_ZX800SG\n");
				
				ZWSys_SetModuleType(ZHONGYUN_ZX800SG);
			}
			if('C' == s_4GHandle.sHardVersion[5])
			{
				ZWSys_SetCloudCardmode(1);
			}
			else
			{
				ZWSys_SetCloudCardmode(0);
			}
		}
	}
	else if(ZHONGYUN_ZX800HG == s_ModuleType)
	{
		if((strlen(s_4GHandle.sHardVersion)) && (0 == strncmp(s_4GHandle.sHardVersion, ZYXAHardVersion5, strlen(ZYXAHardVersion5))))
		{
			if('S' == s_4GHandle.sHardVersion[6] || 'E' == s_4GHandle.sHardVersion[6])
			{
				MODULE4G_INF("[harware] is Eurp module\n");
				ZWSys_SetModuleType(ZHONGYUN_ZX800HG_S);
			}
            else if('N' == s_4GHandle.sHardVersion[6])
            {
                MODULE4G_INF("[harware] is Ameri module\n");
				ZWSys_SetModuleType(ZHONGYUN_ZX800HG_A);
            }
			else if('A' == s_4GHandle.sHardVersion[6])
			{
				MODULE4G_INF("[harware] is China module\n");

				MODULE4G_INF("ZWSys_SetModuleType ZHONGYUN_ZX800SG\n");
				
				ZWSys_SetModuleType(ZHONGYUN_ZX800HG);
			}
			if('C' == s_4GHandle.sHardVersion[5])
			{
				ZWSys_SetCloudCardmode(1);
			}
			else
			{
				ZWSys_SetCloudCardmode(0);
			}
		}

	}
	else if (0 == strncmp(version, "EG800QEULC", strlen("EG800QEULC")))
	{
		ZWSys_SetModuleType(QUECTEL_EG800);
	}
	else
	{
		ZWSys_SetModuleType(UNKNOW_MODULE);
	}

	return 0;
}

//设置4g模组真实支持的sim卡数量，默认为单卡，欧版模组为单卡。针对欧版4g模组：倒数第4个字符为S，D为双卡则为800SGLS11V3001SS18
int Set4GRealSimCardCount(int simCardCount)
{	
	
	if (s_4GHandle.s_4GSimCardCount != simCardCount)
	{
		MODULE4G_INF("Set4GRealSimCardCount s_4GSimCardCount=%d, simCardCount=%d\n", s_4GHandle.s_4GSimCardCount, simCardCount);
		s_4GHandle.s_4GSimCardCount = simCardCount;
	}
	return 0;
}


//获取4g模组真实支持的sim卡数量，默认为单卡，欧版模组为单卡。针对欧版4g模组：倒数第4个字符为S，D为双卡则为800SGLS11V3001SS18
int Get4GRealSimCardCount()
{
	MODULE4G_INF("Get4GRealSimCardCount s_4GSimCardCount=%d\n", s_4GHandle.s_4GSimCardCount);

	return s_4GHandle.s_4GSimCardCount;
}

//800  S   G  L  A13V2120DS18
//           普通
//800  S   G  C  S11V3001SS18
//           云卡
//查询完硬件版本后先立即设置一遍，免得获取板型(如G4WL-47POM-S/G)获取不到
static int SetModuleTypeToSdkAfterFirm()
{
	MODULE4G_INF("SetModuleTypeToSdkAfterFirm s_ModuleType=%d\n", s_ModuleType);
	MODULE4G_INF("[harware] SetModuleTypeToSdk=%s \n", s_4GHandle.sHardVersion);
	if (ZHONGYUN_ZX1602 == s_ModuleType)
	{

		if ((strlen(s_4GHandle.sHardVersion)) && 
            ((0 == strncmp(s_4GHandle.sHardVersion, ZYXAHardVersion, strlen(ZYXAHardVersion))) ||
            (0 == strncmp(s_4GHandle.sHardVersion, ZYXCHardVersion, strlen(ZYXCHardVersion))) ||
            (0 == strncmp(s_4GHandle.sHardVersion, ZYXAHardVersion2, strlen(ZYXAHardVersion2))) ||
            (0 == strncmp(s_4GHandle.sHardVersion, ZYXAHardVersion4, strlen(ZYXAHardVersion4)))))
		{
			if('S' == s_4GHandle.sHardVersion[6] || 'E' == s_4GHandle.sHardVersion[6])
			{
                //欧版
				MODULE4G_INF("[harware] is Eurp module\n");
				ZWSys_SetModuleType(ZHONGYUN_ZX800SG_S);

				if ('S' == s_4GHandle.sHardVersion[14])
				{
					MODULE4G_INF("[harware] is Eurp module and only one card\n");
					Set4GRealSimCardCount(1);
				}
                else if ('D' == s_4GHandle.sHardVersion[14])
				{
					MODULE4G_INF("[harware] is Eurp module and double card\n");
				}
			}
            else if('N' == s_4GHandle.sHardVersion[6])
            {
                //美版
                ZWSys_SetModuleType(ZHONGYUN_ZX800SG_A);
                if ('S' == s_4GHandle.sHardVersion[14])
				{
					MODULE4G_INF("[harware] is Ameri module and only one card\n");
					Set4GRealSimCardCount(1);
				}
            }
			else if('A' == s_4GHandle.sHardVersion[6])
			{
				MODULE4G_INF("[harware] is China module\n");
                ZWSys_SetModuleType(ZHONGYUN_ZX800SG);
			}
			if('C' == s_4GHandle.sHardVersion[5])
			{
				ZWSys_SetCloudCardmode(1);
			}
			else
			{
				ZWSys_SetCloudCardmode(0);
			}
		}
	}
	else if (ZHONGYUN_ZX800HG == s_ModuleType)
	{
		if((strlen(s_4GHandle.sHardVersion)) && 
					((0 == strncmp(s_4GHandle.sHardVersion, ZYXAHardVersion5, strlen(ZYXAHardVersion5))) ||
					(0 == strncmp(s_4GHandle.sHardVersion, ZYXAHardVersion6, strlen(ZYXAHardVersion6)))))
		{
			if('S' == s_4GHandle.sHardVersion[6] || 'E' == s_4GHandle.sHardVersion[6])
			{
                //欧版
				MODULE4G_INF("[harware] is Eurp module\n");
				ZWSys_SetModuleType(ZHONGYUN_ZX800HG_S);

				if ('S' == s_4GHandle.sHardVersion[14])
				{
					MODULE4G_INF("[harware] is Eurp module and only one card\n");
					Set4GRealSimCardCount(1);
				}
                else if ('D' == s_4GHandle.sHardVersion[14])
				{
					MODULE4G_INF("[harware] is Eurp module and double card\n");
				}
			}
            else if('N' == s_4GHandle.sHardVersion[6])
            {
                //美版
                ZWSys_SetModuleType(ZHONGYUN_ZX800HG_A);
                if ('S' == s_4GHandle.sHardVersion[14])
				{
					MODULE4G_INF("[harware] is Ameri module and only one card\n");
					Set4GRealSimCardCount(1);
				}
            }
			else if('A' == s_4GHandle.sHardVersion[6])
			{
				MODULE4G_INF("[harware] is China module\n");
                ZWSys_SetModuleType(ZHONGYUN_ZX800HG);
			}
			if('C' == s_4GHandle.sHardVersion[5])
			{
				ZWSys_SetCloudCardmode(1);
			}
			else
			{
				ZWSys_SetCloudCardmode(0);
			}

		}

	}
	else
	{
		ZWSys_SetModuleType(UNKNOW_MODULE);
	}
	s_4GHandle.nSetModuleTypeEnd = 1;
	return 0;
}

/*@breif QGMR_Receive_Parse_handler QGMR命令返回数据处理
 *@param [IN] at_cmd 发送命令
 *@param [IN] rece_buf 返回字符串
 *@param [IN] rece_len 返回字符串长度
 *@       成功返回0
 *@       失败返回-1
 */
static int QGMR_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len)
{
	char *find_str = NULL;
	char outmsgbuf[128] = {0};
	char *next_str = NULL;
	const char *tmp_str = NULL;
	char *find_end = NULL;
	int ret = 0;

	if ((NULL == at_cmd) || (NULL == rece_buf))
	{
		return -1;
	}

	find_str = strstr(rece_buf,"+QGMR:");
	if (NULL != find_str)
	{
		find_end = strstr(find_str+strlen("+QGMR:"),"\r\n");

		if (NULL == find_end)
		{
			MODULE4G_ERR("NULL pointer !\n");
			return -1;
		}
		else
		{
			if ((find_end-find_str-strlen("+QGMR: ")) > MAX_VERSION_LEN)
			{
				MODULE4G_ERR("too long !\n");
				return -1;
			}
		
			ParaAccessLock();
			memcpy(s_4GHandle.sVersion,find_str+strlen("+QGMR: "),find_end-find_str-strlen("+QGMR: "));
			SetModuleTypeToSdk(&s_4GHandle.sVersion[0]);
			ParaAccessUnLock();

			return 0;
		}
	}

	tmp_str = rece_buf;

	while(1)
	{
		memset(outmsgbuf,0,sizeof(outmsgbuf));
		ret = Get_Msg_Next_Separator(tmp_str,outmsgbuf,&next_str,sizeof(outmsgbuf),AT_MSG_SEPARATOR);
		if ((0 == ret) || (-1 == ret))
		{
			break;
		}

		tmp_str = next_str;

		if (1 == ret)
		{
			if ((strlen(outmsgbuf) > 0) && (NULL == strstr(outmsgbuf,AT_REPORT_SEPARATOR)))
			{
				if (strlen(outmsgbuf) > MAX_VERSION_LEN)
				{
					return -1;
				}
			
				ParaAccessLock();
				memset(s_4GHandle.sVersion,0,sizeof(s_4GHandle.sVersion));
				memcpy(s_4GHandle.sVersion,outmsgbuf,strlen(outmsgbuf));
				SetModuleTypeToSdk(&s_4GHandle.sVersion[0]);
				ParaAccessUnLock();

				MODULE4G_ERR("len = %d sVersion = %s \n",strlen(s_4GHandle.sVersion),s_4GHandle.sVersion);
				break;
			}
		}
	}

	return 0;
}

/*@breif SWITCHSIM_Receive_Parse_handler SWITCHSIM命令返回数据处理
 *@param [IN] at_cmd 发送命令
 *@param [IN] rece_buf 返回字符串
 *@param [IN] rece_len 返回字符串长度
 *@       成功返回0
 *@       失败返回-1
 */

static int SWITCHSIM_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len)
{
	char *find_str = NULL;
	char sim_status[5];
	int sim_No = 0;
	
	MODULE4G_INF("SWITCHSIM_Receive_Parse_handler %s\n", rece_buf);

	if ((NULL == at_cmd) || (NULL == rece_buf))	{
		return -1;
	}

	find_str = strstr(rece_buf,"+SWITCHSIM:");

	if (NULL == find_str){
		return -1;
	}else{
		memset(sim_status,0,sizeof(sim_status));

		//获取SIM卡当前是那张卡
		memcpy(sim_status, find_str+12, 4);
		sim_No = sim_status[3]-'0';
		
        ParaAccessLock();
    	s_4GHandle.nCurSimCardIndex = sim_No-1; //SIM2-->1   SIM1-->0
    	ParaAccessUnLock();
		MODULE4G_INF("sim_status:%s, m_DefaultSim %d\n", sim_status, s_4GHandle.nCurSimCardIndex);
	}

	return 0;
}


/*@breif QENG_Receive_Parse_handler QENG命令返回数据处理
 *@param [IN] at_cmd 发送命令
 *@param [IN] rece_buf 返回字符串
 *@param [IN] rece_len 返回字符串长度
 *@       成功返回0
 *@       失败返回-1
 */
static int QENG_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len)
{
	char *find_str = NULL;
	char *find_str1 = NULL;
	char csq_str[16];
	int csq = 0;
	int i = 0;

	if ((NULL == at_cmd) || (NULL == rece_buf))
	{
		return -1;
	}

	//计算分界符位置
	find_str = strstr(rece_buf,"servingcell");
	if (NULL == find_str)
	{
		return -1;
	}

	for (i = 0;i < 13;i++)
	{
		find_str = strstr(find_str+1,",");
		if (NULL == find_str)
		{
			return -1;
		}
	}

	find_str1 = strstr(find_str+1,",");
	if (NULL == find_str1)
	{
		return -1;
	}

	memset(csq_str,0,sizeof(csq_str));
	memcpy(csq_str,find_str+1,find_str1-find_str-1);
	csq = atoi(csq_str);

    if (ZHONGYUN_ZX1602 == s_ModuleType)
    {//信号质量映射
        if (csq > 0)
        {
            csq = csq - 140;
        }
    }
    
	ParaAccessLock();
	if (csq > -85)
	{
		s_4GHandle.iSignal4G = 4;
	}
	else if (csq <= -85 && csq > -95)
	{
		s_4GHandle.iSignal4G = 3;
	}
	else if (csq <= -95 && csq > -105)
	{
		s_4GHandle.iSignal4G = 2;
	}
	else if (csq <= -105 && csq > -115)
	{
		s_4GHandle.iSignal4G = 1;
	}
	else
	{
		s_4GHandle.iSignal4G = 0;
	}

	MODULE4G_INF("csq = %d iSignal4G = %d \n",csq,s_4GHandle.iSignal4G);
	ParaAccessUnLock();
	
	return 0;
}

/*@breif QNETDEVCTL_Receive_Parse_handler QNETDEVCTL命令返回数据处理
 *@param [IN] at_cmd 发送命令
 *@param [IN] rece_buf 返回字符串
 *@param [IN] rece_len 返回字符串长度
 *@       成功返回0
 *@       失败返回-1
 */
static int QNETDEVCTL_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len)
{
	char *find_str = NULL;
	char *find_str1 = NULL;
	char state_str[16];
	int state = 0;
	int i = 0;

	if ((NULL == at_cmd) || (NULL == rece_buf))
	{
		return -1;
	}

	//计算分界符位置
	find_str = strstr(rece_buf,"+QNETDEVCTL:");
	if (NULL == find_str)
	{
		return -1;
	}

	for (i = 0;i < 3;i++)
	{
		find_str = strstr(find_str+1,",");
		if (NULL == find_str)
		{
			return -1;
		}
	}

	find_str1 = strstr(find_str+1,"\r\n");
	if (NULL == find_str1)
	{
		return -1;
	}

	memset(state_str,0,sizeof(state_str));
	memcpy(state_str,find_str+1,find_str1-find_str-1);
	state = atoi(state_str);

	ParaAccessLock();
	s_4GHandle.nDialState = state;
	MODULE4G_INF("dial_state = %d \n",s_4GHandle.nDialState);
	ParaAccessUnLock();

	return 0;
}

/*@breif QCFG_Receive_Parse_handler QCFG=\"usbnet\" 命令返回数据处理
 *@param [IN] at_cmd 发送命令
 *@param [IN] rece_buf 返回字符串
 *@param [IN] rece_len 返回字符串长度
 *@       成功返回0
 *@       失败返回-1
 */
static int QCFG_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len)
{
	char *find_str = NULL;
	char *find_comma1 = NULL;

	if ((NULL == at_cmd) || (NULL == rece_buf))
	{
		return -1;
	}
	MODULE4G_ERR("[attest] rndistype  at_cmd :%s rece_buf  :%s \n", at_cmd, rece_buf);
	find_str = strstr(rece_buf,"usbnet");
	MODULE4G_ERR("[attest] find_str :%s \n", find_str);
	if (NULL == find_str)
	{
		return -1;
	}
	else
	{
		ParaAccessLock();
		if (NULL != find_str)
		{
			find_comma1 = strstr(find_str,",");
		}
		
		if (NULL != find_comma1)
		{
			MODULE4G_ERR("[attest] find_str1 :%s \n", find_comma1);
			
			if ('1' == find_comma1[1])
			{
				s_4GHandle.rndistype = 1;
			}
			else if ('3' == find_comma1[1])
			{
				s_4GHandle.rndistype = 3;
			}
		}
		ParaAccessUnLock();
	}

	return 0;
}

static int RSLPMode_Receive_Parse_handler(const char *at_cmd, const char *rece_buf, int rece_len)
{
    G4_Control_Manager_t *fd = NULL;
    Module4G_GetSystemHandleInfo(&fd);

    char *find_str = NULL;
    char *find_colon = NULL;
    int mode = 0;

    if ((NULL == at_cmd) || (NULL == rece_buf))
    {
        return -1;
    }

    // 查找 "+FCISCFG:" 字符串
    find_str = strstr(rece_buf, "+FCISCFG:");
    if (NULL == find_str)
    {
        return -1;
    }

    // 查找 "RSLPMode" 字符串
    find_str = strstr(find_str, "RSLPMode");
    if (NULL == find_str)
    {
        return -1;
    }

    // 查找冒号 ":" 的位置
    find_colon = strstr(find_str, ":");
    if (NULL == find_colon)
    {
        return -1;
    }

    // 提取冒号后的值
    mode = atoi(find_colon + 1);

    // 更新全局变量
    ParaAccessLock();
    fd->nRSLPMode = mode;
    ParaAccessUnLock();

    MODULE4G_INF("RSLPMode: %d\n", mode);

    return 0;
}

/*@breif NAT_Receive_Parse_handler QCFG=\"nat\" 命令返回数据处理
 *@param [IN] at_cmd 发送命令
 *@param [IN] rece_buf 返回字符串
 *@param [IN] rece_len 返回字符串长度
 *@       成功返回0
 *@       失败返回-1
 */
static int NAT_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len)
{
	char *find_str = NULL;
	char *find_comma1 = NULL;
	if ((NULL == at_cmd) || (NULL == rece_buf))
	{
		return -1;
	}
	MODULE4G_ERR("[attest] nat at_cmd :%s rece_buf  :%s \n", at_cmd, rece_buf);

	find_str = strstr(rece_buf,"nat");
	MODULE4G_ERR("[attest] find_str :%s \n", find_str);
	
	if (NULL == find_str)
	{
		return -1;
	}
	else
	{
		ParaAccessLock();
		find_comma1 = strstr(find_str, ",");
		if (find_comma1)
		{
			MODULE4G_ERR("[attest] findcomma1 :%s \n", find_comma1);
			s_4GHandle.netcardtype = find_comma1[1] - 48;
		}
		else
		{
			MODULE4G_ERR("[attest] end to findcomma1 \n");
		}
		ParaAccessUnLock();
	}

	return 0;
}



/*@breif NAT_Receive_Parse_handler AT+FHWVER? 命令返回数据处理
 *@param [IN] at_cmd 发送命令
 *@param [IN] rece_buf 返回字符串
 *@param [IN] rece_len 返回字符串长度
 *@       成功返回0
 *@       失败返回-1
 */
static int FHWVERS_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len)
{
	char *find_str1 = NULL;
	char *find_str2 = NULL;
	char *find_str3 = NULL;
	char *find_str = NULL;
	char *find_comma1 = NULL;
	if ((NULL == at_cmd) || (NULL == rece_buf))
	{
		MODULE4G_ERR("[harware] invalid param  \n");
		return -1;
	}

	find_str1 = strstr(rece_buf,"800SG");
	find_str2 = strstr(rece_buf,"800VG"); //支持1605 获取硬件版本号
	find_str3 = strstr(rece_buf,"800HG"); //支持海思4g模组 获取硬件版本号

	ParaAccessLock();
	if(find_str1){
		find_comma1 = strstr(find_str1, ",");
		find_str = find_str1;
		s_4GHandle.s_4GSimCardCount = 1;   //通过4G版本来区分单双卡。1602的只有单卡设备。
	}else if(find_str2){		
		find_comma1 = strstr(find_str2, ",");
		find_str = find_str2;
	}
	else if(find_str3){		
		find_comma1 = strstr(find_str3, ",");
		find_str = find_str3;
	}
	
	if (find_comma1)
	{
		MODULE4G_INF("[harware] findcomma1 :%s \n", find_comma1);
		memcpy(s_4GHandle.sHardVersion, find_str, find_comma1-find_str-1);
		MODULE4G_INF("[harware] final version  :%s \n", s_4GHandle.sHardVersion);

		SetModuleTypeToSdkAfterFirm();
	}
	else
	{
		MODULE4G_INF("[harware] end to findcomma1 \n");
		memset(s_4GHandle.sHardVersion, 0, sizeof(s_4GHandle.sHardVersion));
	}
	ParaAccessUnLock();

	return 0;
}


/*@breif NETDNS_Receive_Parse_handler AT*NETDNS=1 命令返回数据处理
 *@param [IN] at_cmd 发送命令
 *@param [IN] rece_buf 返回字符串
 *@param [IN] rece_len 返回字符串长度
 *@       成功返回0
 *@       失败返回-1
 */
/*
返回数据示例：
[GPRS][4g_mode.c-2149]AT receive: 
*NETDNS: 61.139.2.69,218.6.200.139

OK

*/
static int NETDNS_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len)
{
	char *find_str = NULL;
	char *dns1start = NULL;	
	char *dns2start = NULL;
	char *dns1end = NULL;	
	char *dns2end = NULL;
	char *find_end = NULL;
	//以下为产测用本地字符串，仅在 #if 0 块中使用，标注 unused 以抑制告警
	char testbuf1[] __attribute__((unused)) = "*NETDNS: 61.139.2.69\r\n \r\nOK";
	char testbuf2[] __attribute__((unused)) = "*NETDNS: 61.139.2.69,1.1.1.1\r\n \r\nOK";
	char testbuf3[] __attribute__((unused)) = "*NETDNS: 611.139.2.69,202.103.244.68,1.1.1.1\r\n \r\nOK";

	
	if ((NULL == at_cmd) || (NULL == rece_buf))
	{
		return -1;
	}

	MODULE4G_INF("[netdns] nat at_cmd :%s rece_buf  :%s \n", at_cmd, rece_buf);
	find_str = strstr(rece_buf,"NETDNS:");
	#if 0
	if (0 == access("/config/usr/local/etc/.netdns1", F_OK))
	{
		find_str = strstr(testbuf1, "NETDNS:");
	}
	else if (0 == access("/config/usr/local/etc/.netdns2", F_OK))
	{
		find_str = strstr(testbuf2, "NETDNS:");
	}
	else if (0 == access("/config/usr/local/etc/.netdns3", F_OK))
	{
		find_str = strstr(testbuf3, "NETDNS:");
	}
	MODULE4G_INF("[netdns] find_str :%s len :%d \n", find_str, strlen(find_str));
	#endif
	
	if (NULL == find_str)
	{
		return -1;
	}
	else
	{		
		// 获取dns1起始地址 (认为字符串第一个数字是dns1的起始地址，豆号为dns1的结束地址)
		for (int i = 0; i < strlen(find_str); i++)
		{
			if ((find_str[i]>='0') && (find_str[i]<='9'))
			{
				dns1start=find_str+i;
				break;
			}
		}
		if (NULL == dns1start)
		{
			return -1;
		}
		// 获取所有dns的结束地址
		find_end = strstr(find_str+strlen("NETDNS:"),"\r\n");
		if (!find_end)
		{
			MODULE4G_ERR("[netdns] no find end ? \n");
		}

		MODULE4G_INF("[netdns] num start :%s \n", dns1start);
		
		ParaAccessLock();
		memset(s_4GHandle.szATDnsAddr[0], 0, sizeof(s_4GHandle.szATDnsAddr[0]));
		dns1end = strstr(dns1start, ",");
		if (dns1end)
		{
			// 找到了 说明有多个dns
			memcpy(s_4GHandle.szATDnsAddr[0], dns1start, dns1end-dns1start);
			MODULE4G_INF("[netdns] dns1 :%s \n", s_4GHandle.szATDnsAddr[0]);

			dns2start=dns1end+1;
			memset(s_4GHandle.szATDnsAddr[1], 0, sizeof(s_4GHandle.szATDnsAddr[1]));
			dns2end=strstr(dns2start, ","); 
			if (dns2end)
			{
				memcpy(s_4GHandle.szATDnsAddr[1], dns2start, dns2end-dns2start);
			}
			else
			{
				if (find_end)
				{
					memcpy(s_4GHandle.szATDnsAddr[1], dns2start, find_end-dns2start);
				}
				
			}
			MODULE4G_INF("[netdns] dns2 :%s \n", s_4GHandle.szATDnsAddr[1]);
		}
		else
		{
			//只有一个dns
			if (find_end)
			{
				memcpy(s_4GHandle.szATDnsAddr[0], dns1start, find_end-dns1start);
				MODULE4G_INF("[netdns] dns1 :%s \n", s_4GHandle.szATDnsAddr[0]);
			}
		}
		ParaAccessUnLock();
		#if 0
		for (int i = 0;  i < 2; ++i)
		{
			MODULE4G_INF("[netdns] s_4GHandle.szATDnsAddr[%d] :%s  strlen: %d \n", i, s_4GHandle.szATDnsAddr[i], strlen(s_4GHandle.szATDnsAddr[i]));
		}
		#endif
		
	}

	return 0;
}

/*@breif SETUCWKMODE_Receive_Parse_handler FHWVER命令返回数据处理
 *@param [IN] at_cmd 发送命令
 *@param [IN] rece_buf 返回字符串
 *@param [IN] rece_len 返回字符串长度
 *@       成功返回0
 *@       失败返回-1
 */
static int SETUCWKMODE_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len)
{
	char *find_str = NULL;
	char *find_comma1 = NULL;
	if ((NULL == at_cmd) || (NULL == rece_buf))
	{
		return -1;
	}
	MODULE4G_ERR("[attest] nat at_cmd :%s rece_buf  :%s \n", at_cmd, rece_buf);

	find_str = strstr(rece_buf,"nat");
	MODULE4G_ERR("[attest] find_str :%s \n", find_str);
	
	if (NULL == find_str)
	{
		return -1;
	}
	else
	{
		ParaAccessLock();
		find_comma1 = strstr(find_str, ",");
		if (find_comma1)
		{
			MODULE4G_ERR("[attest] findcomma1 :%s \n", find_comma1);
			s_4GHandle.netcardtype = find_comma1[1] - 48;
		}
		else
		{
			MODULE4G_ERR("[attest] end to findcomma1 \n");
		}
		ParaAccessUnLock();
	}

	return 0;
}

static int POWER_ON_SIM1_CARD_SLOT_STATUS(const char *at_cmd,const char *rece_buf,int rece_len)
{
	if ((NULL == at_cmd) || (NULL == rece_buf))
	{
		return -1;
	}

	if (strstr(rece_buf,"0,1"))
	{
		ParaAccessLock();
		s_4GHandle.sim_01_card_status = 1;
		ParaAccessUnLock();
	}
	else if(strstr(rece_buf,"0,0"))
	{
		ParaAccessLock();
		s_4GHandle.sim_01_card_status = 0;
		ParaAccessUnLock();
	}
	else
	{
		ParaAccessLock();
		s_4GHandle.sim_01_card_status = 2;
		ParaAccessUnLock();
	}

	MODULE4G_INF("POWER_ON_SIM1_CARD_SLOT_STATUS rece_buf:%s , sim_01_card_status:%d\n", rece_buf, s_4GHandle.sim_01_card_status);

	return 0;
}

/*@breif UCWKMODE_Receive_Parse_handler UCWKMODE命令返回数据处理
 *@param [IN] at_cmd 发送命令
 *@param [IN] rece_buf 返回字符串
 *@param [IN] rece_len 返回字符串长度
 *@       成功返回0
 *@       失败返回-1
 */
static int UCWKMODE_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len)
{
	char *find_str = NULL;
	char *find_end = NULL;

	if ((NULL == at_cmd) || (NULL == rece_buf))
	{
		return -1;
	}

	find_str = strstr(rece_buf,"^UCWKMODE:");
	if (NULL != find_str)
	{
		find_end = strstr(find_str+strlen("^UCWKMODE:"),"\r\n");

		if (NULL == find_end)
		{
			MODULE4G_ERR("NULL pointer !\n");
			return -1;
		}
		else
		{
			if ((find_end-find_str-strlen("^UCWKMODE: ")) > MAX_VERSION_LEN)
			{
				MODULE4G_ERR("too long !\n");
				return -1;
			}
		
			ParaAccessLock();
			//ucWorkMode 通过 sscanf 解析整型
			if (sscanf(find_str, "%*[^0-9]%d%*s", &s_4GHandle.ucWorkMode) == 1) 
			{
				MODULE4G_INF("The  s_4GHandle.ucworkmode is: %d\n", s_4GHandle.ucWorkMode);  
			} 
			else 
			{
				MODULE4G_INF("No integer found.\n");	
			}

			ParaAccessUnLock();
		}
	}

	return 0;
}

/*@breif UCWKMODESTATUS_Receive_Parse_handler UCVSIMSTATUS命令返回数据处理
 *@param [IN] at_cmd 发送命令
 *@param [IN] rece_buf 返回字符串
 *@param [IN] rece_len 返回字符串长度
 *@       成功返回0
 *@       失败返回-1
 */
static int UCWKMODESTATUS_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len)
{
	char *find_str = NULL;
	char *find_end = NULL;
    char outTmpbuf[16] = {0};
	char *tmp_str = NULL;

	if ((NULL == at_cmd) || (NULL == rece_buf))
	{
		return -1;
	}
	find_str = strstr(rece_buf,"^UCVSIMSTATUS: ");
	if (NULL != find_str)
	{
		find_str = find_str+strlen("^UCVSIMSTATUS: ");
		find_end = strstr(find_str,"\r\n");
		if (NULL == find_end)
		{
			MODULE4G_ERR("NULL pointer !\n");
			return -1;
		}
		else
		{
			if ((find_end-find_str) > MAX_UCWORSTATUS_LEN)
			{
				MODULE4G_ERR("too long !\n");
				return -1;
			}
		
            ParaAccessLock();
            //^UCVSIMSTATUS: 2,100,0
            tmp_str = strstr(find_str, ",");
            if(NULL == tmp_str)
            {
                ParaAccessUnLock();
                MODULE4G_ERR("\n");
                return -1;
            }
            memset(outTmpbuf, 0, sizeof(outTmpbuf));
            memcpy(outTmpbuf, find_str, tmp_str-find_str);
            s_4GHandle.ucOpenusStatus = atoi(outTmpbuf);
    
            find_str = strstr(tmp_str+1, ",");
            if(NULL == find_str)
            {
                ParaAccessUnLock();
                MODULE4G_ERR("\n");
                return -1;
            }
            memset(outTmpbuf, 0, sizeof(outTmpbuf));
            memcpy(outTmpbuf, tmp_str+1, find_str-tmp_str-1);
            s_4GHandle.ucOpenusProcess = atoi(outTmpbuf);
    
            memset(outTmpbuf, 0, sizeof(outTmpbuf));
            memcpy(outTmpbuf, find_str+1, find_end-find_str-1);
            s_4GHandle.ucOpenusErrno = atoi(outTmpbuf);
            ParaAccessUnLock();
    
            MODULE4G_INF("ucOpenusStatus %d %d %d\n", s_4GHandle.ucOpenusStatus, s_4GHandle.ucOpenusProcess, s_4GHandle.ucOpenusErrno);

			return 0;
		}
	}

	return 0;
}

/*@breif FCISCFG_Receive_Parse_handler AT+FCISCFG 命令返回数据处理
 *@param [IN] at_cmd 发送命令
 *@param [IN] rece_buf 返回字符串
 *@param [IN] rece_len 返回字符串长度
 *@       成功返回0
 *@       失败返回-1
 */

static int FCISCFG_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len)
{
	char *find_str = NULL; 
	char *find_flag = NULL; 
	char *find_flag2 = NULL; 
	char *find_colon = NULL; 
	if ((NULL == at_cmd) || (NULL == rece_buf))
	{
		return -1;
	}
	MODULE4G_INF("[attest] fciscfg at_cmd :%s rece_buf  :%s \n", at_cmd, rece_buf);

	find_str = strstr(rece_buf,"FCISCFG");
	MODULE4G_INF("[attest] find_str :%s \n", find_str);
	
	if (NULL == find_str)
	{
		return -1;
	}
	else
	{
		find_flag = strstr(find_str, "mifiFlag");
		
		if (NULL == find_flag)
		{
			find_flag2 = strstr(find_str, "RSLPMode");
			if(NULL == find_flag2)
			{
				return -1;
			}
		}
	
		ParaAccessLock();
		if(find_flag)
		{
			find_colon = strstr(find_flag, ":");
			
			if (find_colon)
			{
				MODULE4G_ERR("[attest] find_colon :%s  firset ele: %c \n", find_colon, find_colon[1]);
				s_4GHandle.nmifiFlag = find_colon[1]-48;
			}
		}
		
		if(find_flag2)
		{
			find_colon = strstr(find_flag2, ":");
			
			if (find_colon)
			{
				MODULE4G_ERR("[attest] find_colon :%s  firset ele: %c \n", find_colon, find_colon[1]);
				s_4GHandle.iModuleMode = find_colon[1]-48;
			}
		}		

		ParaAccessUnLock();
	}

	return 0;	
}


/*@breif BAND_Receive_Parse_handler BAND命令返回数据处理
 *@param [IN] at_cmd 发送命令
 *@param [IN] rece_buf 返回字符串
 *@param [IN] rece_len 返回字符串长度
 *@       成功返回0
 *@       失败返回-1
 "*BAND: 5, 0, 0, 482, 149, 0, 1, 2, 0, 0"
 *BAND:[mode,gsmband,umtsband,ltebandh,ltebandl,roamingConfig,srvDomain,bandPriorityFlag,isLiteDualLink,ItebandExt]
 ltebandh: TD LTE band; ltebandl: FDD LTE band
*/
static int BAND_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len)
{

	char *find_str = NULL;
	char *find_end = NULL;
    char outTmpbuf[64] = {0};
    int mode = 0, gsmband = 0, umtsband = 0, ltebandh = 0, ltebandl = 0;
	if ((NULL == at_cmd) || (NULL == rece_buf))
	{
		return -1;
	}
	find_str = strstr(rece_buf,"*BAND: ");
	if (NULL != find_str)
	{
		find_str = find_str+strlen("*BAND: ");
		find_end = strstr(find_str,"\r\n");
		if (NULL == find_end)
		{
			MODULE4G_ERR("NULL pointer !\n");
			return -1;
		}
		else
		{
			if ((find_end-find_str) > sizeof(outTmpbuf))
			{
				MODULE4G_ERR("too long !\n");
				return -1;
			}

            ParaAccessLock();
            if (sscanf(find_str, "%d,%d,%d,%d,%d", &mode, &gsmband, &umtsband, &ltebandh, &ltebandl) >= 5)
            {
                MODULE4G_INF("Get Band Info:[%d,%d,%d,%d,%d]\n", mode, gsmband, umtsband, ltebandh, ltebandl);
                s_4GHandle.s_ltebandlBand = ltebandl;
            }
            else
            {
                /*获取失败时使用默认值*/
                s_4GHandle.s_ltebandlBand = 149;
                MODULE4G_ERR("Failed Get BAND\n");
                ParaAccessUnLock();
                return -1;
            }
            ParaAccessUnLock();
		}
	}

	return 0;
}

/*@breif ParseDnsAddr 解析DNS地址
 *@       成功返回0
 *@       失败返回-1
 */
static int ParseDnsAddr(void)
{
	FILE *fp = NULL;
	char buf_line[128] = {0}; 
	char *find_start = NULL;
	char *find_end = NULL;
	int num = 0;

	fp = fopen(DNS_4G_FILE, "r+");
	if (NULL == fp)
	{
		MODULE4G_ERR("fp is NULL!\n");
        return -1;
	}

	while(!feof(fp))
    {
		if (num >= DNS_ADDR_NUM)
		{
			break;
		}

    	memset(buf_line,0, sizeof(buf_line));
        fgets(buf_line, sizeof(buf_line), fp);

        find_start = strstr(buf_line,"nameserver");
        find_end = strstr(buf_line,"\n");

        if ((NULL != find_start) && (NULL != find_end))
        {
			if (find_end - find_start - strlen("nameserver ") < DNS_ADDR_LEN)
			{
				memset(s_4GHandle.szDnsAddr[num],0,DNS_ADDR_LEN);
				memcpy(s_4GHandle.szDnsAddr[num],find_start+strlen("nameserver "),find_end - find_start - strlen("nameserver "));
	        	MODULE4G_INF("DNSADDR[%d] = %s \n",num,s_4GHandle.szDnsAddr[num]);
#ifdef __SUPPORT_SAVE_TF_LOG__ 
				TF_GPRS_LOG_INFO("DNSADDR[%d] = %s \n",num,s_4GHandle.szDnsAddr[num]);
#endif		
	        	num++;
	        }
        }
    }

    fclose(fp);

    return 0;
}


/*@breif UpdateDnsAddr 更新DNS地址至/etc/relosve.conf
 *@       成功返回0
 *@       失败返回-1
 */
static void UpdateDnsAddr(void)
{
	int i = 0;
	char cmd_string[128];

	const char *PingHostAddr = NULL;
	const char *PingHostAddrBak = NULL;
	if(s_4GHandle.device_did_info)
	{
		PingHostAddr = PING_HOST_ADDR_YL;
		PingHostAddrBak = PING_HOST_ADDR_YL_BAK;
	}
	else
	{
		PingHostAddr = PING_HOST_ADDR;
		PingHostAddrBak = PING_HOST_ADDR_BAK;
	}

	memset(cmd_string,0,sizeof(cmd_string));
	snprintf(cmd_string,sizeof(cmd_string)-1,"echo nameserver %s > /etc/resolv.conf",PingHostAddr);
	mysystem(cmd_string);

	memset(cmd_string,0,sizeof(cmd_string));
	snprintf(cmd_string,sizeof(cmd_string)-1,"echo nameserver %s >> /etc/resolv.conf",PingHostAddrBak);
	mysystem(cmd_string);

	for (i = 0;i < DNS_ADDR_NUM;i++)
	{
		if (strlen(s_4GHandle.szDnsAddr[i]) > 0)
		{
			memset(cmd_string,0,sizeof(cmd_string));
			snprintf(cmd_string,sizeof(cmd_string)-1,"echo nameserver %s >> /etc/resolv.conf",s_4GHandle.szDnsAddr[i]);
			mysystem(cmd_string);
		}
	}


    //设备更新网络后，如果路由器的网段不一致，会引起域名解析失效    
    res_init();

    return ;
}



/*@breif AtReceiveHandler 根据不同at命令处理返回数据
 *@param [IN] at_cmd 发送命令
 *@param [IN] rece_buf 返回字符串
 *@param [IN] rece_len 返回字符串长度
 *@       成功返回0
 *@       失败返回-1
 */
static int AtReceiveHandler(const char *at_cmd,const char *rece_buf,int rece_len)
{
	int found = -1;
	int i = 0;

	if ((NULL == at_cmd) || (NULL == rece_buf))
    {
		MODULE4G_ERR("at_cmd or rece_buf is NULL!\n");
        return -1;
    }

    for (i = 0;i < sizeof(s_Receive_Pares_Map) / sizeof(Receive_Parse_Map_t);i++)
    {
		if (strcasestr(at_cmd,s_Receive_Pares_Map[i].cmd))
		{
			found = i;
			break;
		}
    }

    if (-1 != found)
    {
		return s_Receive_Pares_Map[i].handler(at_cmd,rece_buf,rece_len);
    }

    return 0;
}


/*@breif AtCommandSendAndReceiveHandler 发送at命令并处理返回数据
 *@param [IN] tty_fd open打开的文件描述符
 *@param [IN] bEnable 1:打开 0:关闭
 *@       成功返回0
 *@       失败返回-1
 */
static int AtCommandSendAndReceiveHandler(const char *at_cmd)
{
	int send_count = 0;
	char recv_buf[MAX_RECV_DATA_LEN] = {0};
	int rece_len = 0;

	if (s_4GHandle.iSerial_fd < 0)
    {
        MODULE4G_ERR("serial fd is %d invaild!\n", s_4GHandle.iSerial_fd);
        goto failed;
    }

    if (NULL == at_cmd)
    {
		MODULE4G_ERR("at_cmd is NULL!\n");
        goto failed;
    }

    while (send_count < MAX_SERIAL_RETRY_COUNT)
    {
    	//发送次数控制
		send_count++;

		usleep(200000);
		if (SendData(s_4GHandle.iSerial_fd, at_cmd, strlen(at_cmd)) < 0)
		{
			goto failed;
		}
		
		if(strncmp(at_cmd,"AT+SWITCHSIM=",13) == 0)
		{
			sleep(8);
		}
		if(strncmp(at_cmd,"AT+SWITCHSIM?",13) == 0)
		{
			sleep(1);
		}
		if ((rece_len = RecvData(s_4GHandle.iSerial_fd, recv_buf, sizeof(recv_buf))) > 0)
        {
			MODULE4G_INF("AT receive: %s\n",recv_buf);
            if (strcasestr(recv_buf, AT_RESP_OK_STR))
            {              
            	//根据不同指令,解析返回数据
				AtReceiveHandler(at_cmd,recv_buf,rece_len);

                return 0;
            }
			else if (strcasestr(at_cmd, AI_RESET_MODULE))
			{
				MODULE4G_ERR("[testing]  rec at_cmd :%s , will return \n", at_cmd);
				return 0;
			}
            else if ((1 == s_IsSGModule) && (strcasestr(at_cmd, AT_GET_DNS)))
            {
                //根据不同指令,解析返回数据
                MODULE4G_ERR("[testing]  rec at_cmd :%s , will return \n", at_cmd);
                return 0;
            }
			else if (strcasestr(at_cmd, AI_UC_MODULE) && strcasestr(recv_buf, AT_RESP_ERR_STR))
			{
				s_4GHandle.ucWorkMode = 0;
				//根据不同指令,解析返回数据
                MODULE4G_ERR("[testing]  rec at_cmd :%s , will return \n", at_cmd);
                return 0;
			}
			else if (strcasestr(at_cmd, AI_QSIMSTAT) && strcasestr(recv_buf, AT_RESP_ERR_STR))
			{
				s_4GHandle.sim_01_card_status = 2;
				//根据不同指令,解析返回数据
                MODULE4G_ERR("[testing]  rec at_cmd :%s , will return  s_4GHandle.sim_01_card_status = %d\n", at_cmd,s_4GHandle.sim_01_card_status);
                return 0;
			}
        }
		// 中云信安模组 复位指令只接收一次
		else if((ZHONGYUN_ZX800  == s_ModuleType 
		    || ZHONGYUN_ZX1602  == s_ModuleType) 
		    && (strcasestr(at_cmd, AI_RESET_MODULE)) && (rece_len <=0))
		{
			MODULE4G_ERR("rec at_cmd :%s rece_len :%d , will return \n", at_cmd, rece_len);
			return 0;
		}
			

        GprsSleep(SEND_RETRY_SLEEP_TIME);
    }

failed:
	return -1;
}


/*@breif SetATCommandATE0 禁止回显
 *@       成功返回0
 *@       失败返回-1
 */
static int SetATCommandATE0()
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "ATE0\r\n");

    return AtCommandSendAndReceiveHandler(at_cmd);
}

// 查询mififlag 判断是否使用公网dns
static int SetATCommandGetMIfIFlag()
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+FCISCFG=mifiFlag\r\n");
    return AtCommandSendAndReceiveHandler(at_cmd);
}

// 设置mififlag     0：使用公网dns 1：使用局域网dns
static int SetATCommandSetMIfIFlag(int flag)
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+FCISCFG=mifiFlag,%d\r\n", flag);
    return AtCommandSendAndReceiveHandler(at_cmd);
}

/*@breif AT+FCISCFG=RSLPMode 获取模组当前正在使用哪种模式？ <mode>是业务形态，0:常电, 1:低功耗, 2:AOV
 *@       成功返回0
 *@       失败返回-1
 */
static int GetATCommandRSLPMode()
{
    char at_cmd[128] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+FCISCFG=RSLPMode\r\n");
    return AtCommandSendAndReceiveHandler(at_cmd);
}

/*@breif AT+FCISCFG=RSLPMode,<mode>     //<mode>是业务形态，0:常电, 1:低功耗, 2:AOV
 *@       成功返回0
 *@       失败返回-1
 */
static int SetATCommandRSLPMode(const int mode)
{
    char at_cmd[128] = {0};

    snprintf(at_cmd, sizeof(at_cmd), "AT+FCISCFG=RSLPMode,%d\r\n", mode);
    return AtCommandSendAndReceiveHandler(at_cmd);
}

// 查询公网dns
static int SetATCommandGetDNS()
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT*NETDNS=1\r\n");
    return AtCommandSendAndReceiveHandler(at_cmd);
}

/*@breif SetATCommandCGSN 查询IMEI
 *@       成功返回0
 *@       失败返回-1
 */
static int SetATCommandCGSN()
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+CGSN\r\n");

    return AtCommandSendAndReceiveHandler(at_cmd);
}

/*@breif SetATCommandCIMI 查询IMSI
 *@       成功返回0
 *@       失败返回-1
 */
static int SetATCommandCIMI()
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+CIMI\r\n");

    return AtCommandSendAndReceiveHandler(at_cmd);
}

/*@breif SetATCommandCIMI 查询act oper
 *@       成功返回0
 *@       失败返回-1
 */
static int SetATCommandCOPS()
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+COPS?\r\n");

    return AtCommandSendAndReceiveHandler(at_cmd);
}

/*@breif SetATCommandCOPS 设置act oper级别
 *@       成功返回0
 *@       失败返回-1
 */
static int SetATCommandCOPSNUM()
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+COPS=3,0\r\n");

    return AtCommandSendAndReceiveHandler(at_cmd);
}

/*@breif SetATCommandCFUN 打开关闭飞行模式
 *@param [IN] bEnable 1:打开 4:关闭
 *@       成功返回0
 *@       失败返回-1
 */
static int SetATCommandCFUN(int bEnable)
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+CFUN=%d\r\n", bEnable);

    return AtCommandSendAndReceiveHandler(at_cmd);
}

/*@breif SetATCommandResetModlue AT指令复位模组
 *@param 
 *@       成功返回0
 *@       失败返回-1
 */
__attribute__((unused)) static int SetATCommandResetModlue()
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+CFUN=1,1\r\n");

    return AtCommandSendAndReceiveHandler(at_cmd);
}

/*@breif GetATCommandSIM01CardStatus 查询sim1卡是否插卡
 *@       成功返回0
 *@       失败返回-1
 */
static int GetATCommandSIM01CardStatus()
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+QSIMSTAT?\r\n");

    return AtCommandSendAndReceiveHandler(at_cmd);
}

/*@breif SetATCommandQNWINFO 查询band信息
 *@       成功返回0
 *@       失败返回-1
 */
static int SetATCommandQNWINFO()
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+QNWINFO\r\n");

    return AtCommandSendAndReceiveHandler(at_cmd);
}

/*@breif SetATCommandBAND 查询可用频段
*@       成功返回0
*@       失败返回-1
*/
int SetATCommandBAND()
{
	char at_cmd[64] = {0};
	snprintf(at_cmd, sizeof(at_cmd), "AT*BAND?\r\n");

	return AtCommandSendAndReceiveHandler(at_cmd);
}

/*@breif SetATCommandBAND 设置移动卡自动频段
*@       成功返回0
*@       失败返回-1
*/
int SetATCommandChinaMobileAutoBAND()
{
	char at_cmd[64] = {0};
	snprintf(at_cmd, sizeof(at_cmd), "AT*BAND=5,0,0,482,149\r\n");
	return AtCommandSendAndReceiveHandler(at_cmd);
}

/*@breif SetATCommandBAND 设置锁死移动卡FDD频段
*@       成功返回0
*@       失败返回-1
*/
int SetATCommandChinaMobileFDDBAND()
{
	char at_cmd[64] = {0};
	snprintf(at_cmd, sizeof(at_cmd), "AT*BAND=5,0,0,0,%d\r\n", s_4GHandle.s_ltebandlBand);
    MODULE4G_INF("Force Set China Mobile FDD Band[%s]\n", at_cmd);
	return AtCommandSendAndReceiveHandler(at_cmd);
}

/*@breif SetATCommandICCID 查询ICCID
 *@       成功返回0
 *@       失败返回-1
 */
static int SetATCommandICCID()
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+QCCID\r\n");

    return AtCommandSendAndReceiveHandler(at_cmd);
}

/*@breif SetATCommandGetCREG 查询入网信息
 *@       成功返回0
 *@       失败返回-1
 */
__attribute__((unused)) static int SetATCommandGetCREG()
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+CREG?\r\n");

    return AtCommandSendAndReceiveHandler(at_cmd);
}

/*@breif SetATCommandGetCREG 查询入网信息
 *@       成功返回0
 *@       失败返回-1
 */
static int SetATCommandGetCEREG()
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+CEREG?\r\n");

    return AtCommandSendAndReceiveHandler(at_cmd);
}

/*@breif SetATCommandGetCREG 查询入网信息
 *@       成功返回0
 *@       失败返回-1
 */
/*中云模组不支持该命令!*/
static int SetATCommandGetCGREG()
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+CGREG?\r\n");

    return AtCommandSendAndReceiveHandler(at_cmd);
}

/*@breif SetATCommandSetCEREG 设置入网信息查询等级
 *@param [IN] nIndex sim卡索引
 *@       成功返回0
 *@       失败返回-1
 */
static int SetATCommandSetCEREG(int nIndex)
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+CEREG=%d\r\n", nIndex);

    return AtCommandSendAndReceiveHandler(at_cmd);
}

/*@breif SetATCommandSetCGREG 设置入网信息查询等级
 *@param [IN] nIndex sim卡索引
 *@       成功返回0
 *@       失败返回-1
 */
 /*中云模组不支持该命令!*/
static int SetATCommandSetCGREG(int nIndex)
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+CGREG=%d\r\n", nIndex);

    return AtCommandSendAndReceiveHandler(at_cmd);
}

/*@breif SetATCommandSetCREG 设置入网信息查询等级
 *@param [IN] nIndex sim卡索引
 *@       成功返回0
 *@       失败返回-1
 */
__attribute__((unused)) static int SetATCommandSetCREG(int nIndex)
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+CREG=%d\r\n", nIndex);

    return AtCommandSendAndReceiveHandler(at_cmd);
}


/*@breif SetATCommandCSQ 查询信号质量
 *@       成功返回0
 *@       失败返回-1
 */
static int SetATCommandCSQ()
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+CSQ\r\n");

    return AtCommandSendAndReceiveHandler(at_cmd);
}


/*@breif SetATCommandCGDCONT 查询PDP上下文
 *@       成功返回0
 *@       失败返回-1
 */
__attribute__((unused)) static int SetATCommandCGDCONT()
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+CGDCONT?\r\n");

    return AtCommandSendAndReceiveHandler(at_cmd);
}


/*@breif SetATCommandCGPADDR 查询PDP地址
 *@       成功返回0
 *@       失败返回-1
 */
__attribute__((unused)) static int SetATCommandCGPADDR()
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+CGPADDR\r\n");

    return AtCommandSendAndReceiveHandler(at_cmd);
}


/*@breif SetATCommandQCFG 设置接口协议
 *@param [In] type 接口协议  1 ECM  3 RNDIS
 *@       成功返回0
 *@       失败返回-1
 */
static int SetATCommandQCFG(int type)
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+QCFG=\"usbnet\",%d\r\n",type);

    return AtCommandSendAndReceiveHandler(at_cmd);
}

/*@breif SetATCommandQCFG 获取接口协议
 *@param [In] type 接口协议  1 ECM  3 RNDIS
 *@       成功返回0
 *@       失败返回-1
 */
static int SetATCommandGetQCFG()
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+QCFG=\"usbnet\"\r\n");

    return AtCommandSendAndReceiveHandler(at_cmd);
}

/*@breif SetATCommandQCFGNat 设置网卡工作模式
 *@param [In] mode 工作模式  0 路由模式  1 网卡模式
 *@       成功返回0
 *@       失败返回-1
 */
static int SetATCommandQCFGNat(int mode)
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+QCFG=\"nat\",%d\r\n",mode);

    return AtCommandSendAndReceiveHandler(at_cmd);
}


/*@breif SetATCommandQCFGNat 获取网卡工作模式
 *@param [In] mode 工作模式  0 路由模式  1 网卡模式
 *@       成功返回0
 *@       失败返回-1
 */
static int SetATCommandGetQCFGNat(void)
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+QCFG=\"nat\"\r\n");

    return AtCommandSendAndReceiveHandler(at_cmd);
}

/*@breif SetATCommandQNETDEVCTL 连接USB网卡
 *@param [In] nEnable 使能  1 手动连接  0 断开  3自动连接 
 *@       成功返回0
 *@       失败返回-1
 */
static int SetATCommandQNETDEVCTL(int nEnable)
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+QNETDEVCTL=%d,1\r\n",nEnable);

    return AtCommandSendAndReceiveHandler(at_cmd);
}

/*@breif SetATCommandGetQNETDEVCTL 连接USB网卡
 *@param [In] nEnable 使能  1 手动连接  0 断开  3自动连接 
 *@       成功返回0
 *@       失败返回-1
 */
static int SetATCommandGetQNETDEVCTL()
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+QNETDEVCTL?\r\n");

    return AtCommandSendAndReceiveHandler(at_cmd);
}


/*@breif SetATCommandATTEST 测试at指令通讯是否正常
 *@       成功返回0
 *@       失败返回-1
 */
static int SetATCommandATTEST()
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT\r\n");

    return AtCommandSendAndReceiveHandler(at_cmd);
}



/*@breif  SetATCommandATCommandFHWVER 获取4G模组硬件版本
 *@       成功返回0
 *@       失败返回-1
 */
static int SetATCommandATCommandFHWVER()
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+FHWVER?\r\n");

    return AtCommandSendAndReceiveHandler(at_cmd);
}



/*@breif SetATCommandQGMR 获取4G模组固件版本
 *@       成功返回0
 *@       失败返回-1
 */
static int SetATCommandQGMR()
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+QGMR\r\n");

    return AtCommandSendAndReceiveHandler(at_cmd);
}

/*@breif SetATCommandQENG 获取4G信号强度
 *@       成功返回0
 *@       失败返回-1
 */
static int SetATCommandQENG()
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+QENG=\"SERVINGCELL\"\r\n");

    return AtCommandSendAndReceiveHandler(at_cmd);
}


/*@breif SetATCommandQFOTADL 发送升级指令
 *@       成功返回0
 *@       失败返回-1
 */
static int SetATCommandQFOTADL(int total_len)
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd),"AT+QFOTADL=\"FILE:%d\"\r\n",total_len);

    return AtCommandSendAndReceiveHandler(at_cmd);
}

/*@breif SetATCommandQSCLKEX 启用增强型休眠模式
 *@       成功返回0
 *@       失败返回-1
 */
__attribute__((unused)) static int SetATCommandQSCLKEX()
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+QSCLKEX=1,3,30\r\n");

    return AtCommandSendAndReceiveHandler(at_cmd);
}


/*@breif SetATCommandCMEE 显示错误类型
 *@       成功返回0
 *@       失败返回-1
 */
static int SetATCommandCMEE(int value) 
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+CMEE=%d\r\n",value);

    return AtCommandSendAndReceiveHandler(at_cmd);
}

/*@breif SetATCommandCPIN 是否检测到卡
 *@       成功返回0
 *@       失败返回-1
 */
static int SetATCommandCPIN()
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+CPIN?\r\n");

    return AtCommandSendAndReceiveHandler(at_cmd);
}

/*@breif AT+QCFG="urc/ri/other" 规避设备升级失败问题
 *@       成功返回0
 *@       失败返回-1
 */
static int SetATCommandQCFGOther(const int type)
{
    char at_cmd[128] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+QCFG=\"urc/ri/other\",\"%s\"\r\n", type? "pulse":"off");
    return AtCommandSendAndReceiveHandler(at_cmd);
}

/*@breif AT+SWITCHSIM=x 切换SIM卡（x=0：SIM1 x=1：SIM2）
 *@       成功返回0
 *@       失败返回-1
 */
static int SetATCommandSwitchSim(const int nSimNo)
{
	G4_Control_Manager_t *fd = NULL;
	Module4G_GetSystemHandleInfo(&fd);

    int nRet = 0;
    char at_cmd[128] = {0};

    if ((fd->s_4GSimCardCount != 0) && (nSimNo >= fd->s_4GSimCardCount))
    {
		MODULE4G_ERR("SetATCommandSwitchSim ERR!\n");
        return -1;
    }
    
    snprintf(at_cmd, sizeof(at_cmd), "AT+SWITCHSIM=%d\r\n", nSimNo);
    nRet = AtCommandSendAndReceiveHandler(at_cmd);
	if (0 == nRet)
	{
		//成功切换则修改当前使用SIM号
		ParaAccessLock();
		fd->nCurSimCardIndex = nSimNo;
		ParaAccessUnLock();
	}
	return nRet;
	
}



int SetATCommandGetCurrentSimSta()
{
	char at_cmd[64] = {0};
	snprintf(at_cmd, sizeof(at_cmd), "AT+SWITCHSIM=?\r\n");
	return AtCommandSendAndReceiveHandler(at_cmd);
}

/*@breif AT+ SWITCHSIM? 获取当前使用的SIM卡（x=0：SIM1 x=1：SIM2）
 *@       成功返回0
 *@       失败返回-1
 */
int SetATCommandGetCurrentSwitchSim()
{
	char at_cmd[64] = {0};
	snprintf(at_cmd, sizeof(at_cmd), "AT+SWITCHSIM?\r\n");
	return AtCommandSendAndReceiveHandler(at_cmd);
}




/*@breif AT^UCWKMODE? 设置设备强制使用云卡
 *@       成功返回0
 *@       失败返回-1
 */
static int SETATCommandUCWKMODEther()
{
    char at_cmd[128] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT^UCWKMODE=1\r\n");
    return AtCommandSendAndReceiveHandler(at_cmd);
}


/*@breif Module4G_WatchDog_Init 看门狗处理，防止该线程阻塞
 *@       
 *@       
 */
static void Module4G_WatchDog_Handler(void)
{
	int ret = 0;

	//fjz-notice
	if (-1 == WatchDog_Handler)
	{
		//添加看门狗任务
		ret = WATCH_AddTask(WATCHDOX_MAX_TIME,&WatchDog_Handler,"GPRS_HANDLER_THREAD");
		if (0 != ret)
		{
			MODULE4G_ERR("WATCH_AddTask failed ret = %d!\n", ret);	
		}
	}
	else
	{
		if (WatchDog_Count >= WATCHDOG_FEED)
		{
			WATCH_NotifyTaskLive(WatchDog_Handler);
			WatchDog_Count = 0;
		}
	}
}

/*@breif AT^UCWKMODE? 获取设备云卡工作模式
 *@       成功返回0
 *@       失败返回-1
 */
static int GetATCommandUCWKMODEther(void)
{
    char at_cmd[128] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT^UCWKMODE?\r\n");
    return AtCommandSendAndReceiveHandler(at_cmd);
}


/*@breif AT^UCWKMODE? 获取设备云卡当前状态
 *@       成功返回0
 *@       失败返回-1
 */
static int GetATCommandUCWKSTATUSther(void)
{
    char at_cmd[128] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT^UCVSIMSTATUS?\r\n");
    return AtCommandSendAndReceiveHandler(at_cmd);
}

/*@breif Module4G_KillUphcpc 杀掉后台执行的udhcpc
 *@param [In] pNetCard 网卡
 *@return 成功返回 0
 *@       失败返回-1
 */
static int Module4G_KillUphcpc(const char *pNetCard)
{
	int ret;
	char buf[16] = {0};
	int pid = 0;
	char command[1024] ={0};
	
	snprintf(command, sizeof(command),"ps | grep \"udhcpc -t 11 -b -i %s -x hostname IPCAM -R\" | grep -v grep | awk -F \" \" '{print $1}'", pNetCard);
	ret = ShellCmdOut(command, buf, sizeof(buf));//PID最大为5位
    MODULE4G_INF("##############command, buf=%s\n", buf);

	if(ret)
	  MODULE4G_ERR("Not found udhcpc -t 11 -b -i %s -x hostname IPCAM\n",pNetCard);
	else
	{
	  pid = atoi(buf);
	  if((pid > 100) && (pid <= 0x8000))//100是经验值，0x8000是linux最大pid号
	  {
		  memset(buf, 0, sizeof(buf));
		  snprintf(buf, sizeof(buf),"kill %d", pid);
		  mysystem(buf);
	  }
	  else
		  MODULE4G_ERR("found udhcpc -b -i %s -x hostname IPCAM PID number is invalid\n",pNetCard);
	}
	MODULE4G_INF("##############kill  Dhcp, pid=%d\n", pid);

	return 0;	
}


/*@breif Module4G_KillUphcpc 关闭拨号
 *@param 
 *@return 成功返回 0
 *@       失败返回-1
 */
static int Module4G_DailDown()
{
	char cmd_string[128];

	//关闭后台dhcp进程
	Module4G_KillUphcpc(MEIG_SLM322M_ETH);

	//删除usb0 路由
	memset(cmd_string,0,sizeof(cmd_string));
	snprintf(cmd_string,sizeof(cmd_string)-1,"/usr/sbin/operate_router delRoute %s",MEIG_SLM322M_ETH);
	mysystem(cmd_string);

	memset(cmd_string,0,sizeof(cmd_string));
	snprintf(cmd_string,sizeof(cmd_string)-1,"ifconfig %s down",MEIG_SLM322M_ETH);
	mysystem(cmd_string);

	/* 设置路由之后，需要清除一下ARP缓存，以免不能上外网 */
    mysystem("arp -n|awk '/^[1-9]/{print \"arp -d  \" $1}'");
    
    return 0;
}


/*@breif Module4G_StartDial 开启4G模组
 *@param [In] pNetCard 网卡
 *@return 成功返回 0
 *@       失败返回-1
 */
static int Module4G_StartDial()
{
	char cmd_string[128];

	memset(cmd_string,0,sizeof(cmd_string));
	snprintf(cmd_string,sizeof(cmd_string)-1,"ifconfig %s up",MEIG_SLM322M_ETH);
	mysystem(cmd_string);

	memset(cmd_string,0,sizeof(cmd_string));
	snprintf(cmd_string,sizeof(cmd_string)-1,"udhcpc -t 11 -b -i %s -x hostname:IPCAM -R",MEIG_SLM322M_ETH);
	mysystem(cmd_string);

	return 0;
}


/*@breif ResetUsbHost 复位usb host
 *@       
 *@       
 */
static void ResetUsbHost()
{

}


/*@breif Reset4GModule 4G模组关闭状态
 *@       
 *@       
 */
static void PowerDown4GModule()
{
	s_4GHandle.nModuleOpenFlag = 0;
}


/*@breif Reset4GModule 4G模组开机状态
 *@       
 *@       
 */
static void PowerUp4GModule()
{
	s_4GHandle.nModuleOpenFlag = 1;
}


/*@breif Reset4GModule 断上电复位4G模组
 *@       
 *@       
 */
static void Reset4GModule()
{
	PowerDown4GModule();

	//控制复位引脚，对模组进行复位
	ZWSys_4GModuleReset();

	PowerUp4GModule();
}

static void ChangeOtherSim(void)
{

	G4_Control_Manager_t *fd = NULL;
	Module4G_GetSystemHandleInfo(&fd);
	/* 非多卡设备直接返回 */
	if(fd->s_4GSimCardCount <= 1)
	{
		return;
	}
	int nSimNo = 0;
	nSimNo = fd->nCurSimCardIndex==0 ? 1: 0;
	SetATCommandSwitchSim(nSimNo);
	sleep(10);//切卡后等待10秒
	MODULE4G_INF("ChangeOtherSim!\n");	
}


/*@breif Module4GIsOnline 4G模组是否存在
 *@       存在返回0
 *@       不存在返回-1      
 */
static int Module4GIsOnline()
{
	int ret = -1;
	memset(szTtyDevice,0,sizeof(szTtyDevice));

	//fjz-notice
	if(1 == IsStringInCommand("lsusb", ZX1602_MODULE_USBID))
	{
		if(0 == access(TTY_DEVICE1,F_OK))
		{
			s_ModuleType = ZHONGYUN_ZX1602;
			snprintf(szTtyDevice,sizeof(szTtyDevice),TTY_DEVICE1);
			ret = 0;
		}
		else if(0 == access(TTY_DEVICE,F_OK))
		{
			s_ModuleType = ZHONGYUN_ZX1602;
			snprintf(szTtyDevice,sizeof(szTtyDevice),TTY_DEVICE);
			ret = 0;
		}

	}
	else if((1 == IsStringInCommand("lsusb", MEIG_MODULE_USBID)) && (0 == access(TTY_DEVICE,F_OK)))
	{
		s_ModuleType = QUECTEL_EC800;
		snprintf(szTtyDevice,sizeof(szTtyDevice),TTY_DEVICE);
		ret = 0;
	}
    else if((1 == IsStringInCommand("lsusb", ZX800_MODULE_USBID)) && (0 == access(TTY_DEVICE,F_OK)))
	{
		s_ModuleType = ZHONGYUN_ZX800;
		snprintf(szTtyDevice,sizeof(szTtyDevice),TTY_DEVICE);
		ret = 0;
	}
	else if((1 == IsStringInCommand("lsusb", EG800QEU_MODULE_USBID)) && (0 == access(EG800QEU_TTY_DEVICE,F_OK)))
	{
		s_ModuleType = QUECTEL_EG800;
		snprintf(szTtyDevice,sizeof(szTtyDevice),EG800QEU_TTY_DEVICE);
		ret = 0;
	}
	else if((1 == IsStringInCommand("lsusb", ZX800HG_MODULE_USBID)) && (0 == access(TTY_DEVICE_H,F_OK)))
	{
		s_ModuleType = ZHONGYUN_ZX800HG;
		snprintf(szTtyDevice,sizeof(szTtyDevice),TTY_DEVICE_H);
		ret = 0;
	}
	else if((1 == IsStringInCommand("lsusb", ZX800HG_MODULE_USBID)) && (0 == access(TTY_DEVICE_H_1,F_OK)))
	{
		s_ModuleType = ZHONGYUN_ZX800HG;
		snprintf(szTtyDevice,sizeof(szTtyDevice),TTY_DEVICE_H_1);
		ret = 0;
	}
	mysystem("ls /dev/tty*");
	mysystem("lsusb");
	return ret;
}


/*@breif Open4GModule 打开与模组连接
 *@       成功返回0
 *@       失败返回-1
 */
static int Open4GModule(void)
{
    int open_fd = -1;
	char nTty_Devic[16] = {0};


	strncpy(nTty_Devic,szTtyDevice,sizeof(nTty_Devic)-1);
	
    /* 打开串口节点 */
    open_fd = OpenSerialPort(nTty_Devic);
    if (open_fd < 0)
    {
        MODULE4G_ERR("Fail to open %s\n", nTty_Devic);
#ifdef __SUPPORT_SAVE_TF_LOG__        
        TF_GPRS_LOG_INFO("Fail to open %s\n", nTty_Devic);
#endif        
        return -1;
    }

    /* 设置串口节点参数(波特率，数据位，奇偶校验位等) */
    if (SetSerialAttr(open_fd, 115200, 8, 'N', 1))
    {
        MODULE4G_ERR("Fail to called SetSerialAttr\n");
        goto FAIL_EXIT;
    }

    MODULE4G_INF("4G Module open successful!\n");

    s_4GHandle.iSerial_fd = open_fd;

    return open_fd;

FAIL_EXIT:
    MODULE4G_ERR("EC25 4G Module init fail!\n");
    return -1;
}

/*@breif SwitchTaskState 切换主状态
 *@param [In] state 状态
 *@
 *@       
 */
__attribute__((unused)) static void SwitchTaskState(int state)
{
	s_4GHandle.uState = state;
}

/*@breif SwitchSubState 切换子状态
 *@param [In] state 状态
 *@
 *@       
 */
static void SwitchSubState(int state)
{	
	s_4GHandle.uSubState = state;
}

/*@breif ManagerDateReset 重置数据
 *@param 
 *@
 *@       
 */
static void ManagerDateReset()
{	
	ParaAccessLock();
	
	s_4GHandle.nPingControltime = 0;
	s_4GHandle.nPingFailedCount = 0;
	s_4GHandle.nAirPlaneTestCount = 0;
	s_4GHandle.nUsbResetCount = 0;
	s_4GHandle.nModuleResetCount = 0;
	s_4GHandle.nNetFailedResetModuleFlag = 0;
	s_4GHandle.nHasCutCardFlag = 0;

	memset(s_4GHandle.uSimCardStatus, 0, sizeof(s_4GHandle.uSimCardStatus));
	memset(s_4GHandle.uCregStatus, 0, sizeof(s_4GHandle.uCregStatus));
	s_4GHandle.uInternetConnStatus = 0;
    s_4GHandle.uRealNetConnStatus = 0;

	memset(s_4GHandle.szDnsAddr,0,sizeof(s_4GHandle.szDnsAddr));
	s_4GHandle.nCregCheckCount = 0;
	s_4GHandle.nDialState = 0;
	s_4GHandle.nDialStateCheckCount = 0;
	s_4GHandle.rndistype = 0;	
	s_4GHandle.netcardtype = 0;
	ParaAccessUnLock();
}


/*@breif GetTaskState 获取当前主状态，根据参数
 *@param [Out] cur 状态
 *@
 *@       
 */
static void GetTaskState(int *cur)
{
	int cur_state = 0;
	int next_state = 0;

	ParaAccessLock();
	cur_state = s_4GHandle.uState;
	next_state = s_4GHandle.uState;

	if (s_4GHandle.nUpdate4GFlag)
	{
		//优先处理4G升级
		next_state = TASK_UPDATE_MODULE_STATE;
	}
	else if (s_4GHandle.nProductTestFlag)
	{
		//优先处理产测
		next_state = TASK_PRODUCTTEST_STATE;
	}
	else
	{
		if (s_4GHandle.uAllowFlag)
		{
			if (s_4GHandle.uProductMode || UNBINDING_MODE == s_4GHandle.uWorkMode)
			{
				//产测模式和 标准模式，优先使用有线
				if (s_4GHandle.uWireConnStatus)
				{
					//标准模式，有线连接成功，关闭4g模组
					next_state = TASK_SLEEP_STATE;
				}
				else
				{
					//非内置卡绑定，无有线连接
					if (!s_4GHandle.uCheckErrorFlag)
					{
						next_state = TASK_USE4G_STATE;
					}
					else
					{
						next_state = TASK_CHECK_ERROR_STATE;
					}
				}
			}
			else if (BINDING_MODE == s_4GHandle.uWorkMode)
			{
				if (s_4GHandle.uAllowUseEth0Flag)
				{
					//优先使用有线
					if (s_4GHandle.uWireConnStatus)
					{
						//有线连接成功，关闭4g模组
						next_state = TASK_SLEEP_STATE;
					}
					else
					{
						//无有线连接
						if (!s_4GHandle.uCheckErrorFlag)
						{
							next_state = TASK_USE4G_STATE;
						}
						else
						{
							next_state = TASK_CHECK_ERROR_STATE;
						}
					}
				}
				else
				{
					//运营模式
					if (!s_4GHandle.uCheckErrorFlag)
					{
						next_state = TASK_USE4G_STATE;
					}
					else
					{
						next_state = TASK_CHECK_ERROR_STATE;
					}
				}
			}
		}
		else
		{
			//关闭4g模组
			next_state = TASK_SLEEP_STATE;
		}
	}

	//更新当前状态
	s_4GHandle.uState = next_state;
	*cur = s_4GHandle.uState;
	ParaAccessUnLock();

	if (cur_state != next_state)
	{
		//复位控制数据
		ManagerDateReset();
	
		//子状态重置
		s_4GHandle.uSubState = 0;

		MODULE4G_ERR("next_state = %d, cur_state = %d \n", next_state, cur_state);

#ifdef __SUPPORT_SAVE_TF_LOG__ 		
		TF_GPRS_LOG_INFO("next_state = %d, cur_state = %d \n", next_state, cur_state);
		TF_GPRS_LOG_INFO("uAllowFlag:%d uProductMode:%d uWorkMode:%d  \
						uWireConnStatus:%d uAllowUseEth0Flag: %d uCheckErrorFlag: %d\n", \
						s_4GHandle.uAllowFlag,s_4GHandle.uProductMode, \
						s_4GHandle.uWorkMode,s_4GHandle.uWireConnStatus, \
						s_4GHandle.uAllowUseEth0Flag,s_4GHandle.uCheckErrorFlag);
#endif						
	}
}


/*@breif WaitModuleOnline 等待模组上线，防止枚举时间过长导致启动问题
 *@param 
 *@return 
 *@       
 */
static void WaitModuleOnline()
{
	int count = 0;
	int online = 0;

	while(count <= WAIT_MODULE_ONLINE)
	{
		if (0 == Module4GIsOnline())
		{
			online = 1;
			MODULE4G_INF("wait online time: %d\n",count);
			break;
		}

		count++;
		GprsSleep(1);
	}

	if (0 == online)
	{
		MODULE4G_INF("wait online failed: %d\n",WAIT_MODULE_ONLINE);
	}
}



/*@breif CheckModlueHandler 检测模组
 *@param 
 *@return 
 *@       
 */
static int CheckModlueHandler(int next_state)
{
	int ret = 0;

	PowerUp4GModule();
	
	if (-1 == (ret = Module4GIsOnline()))
	{
		if (s_4GHandle.nModuleResetCount < MAX_MODULE_RESET_COUNT)
		{
			MODULE4G_ERR("Check Module Failed,Reset Module %d\n",s_4GHandle.nModuleResetCount);

#ifdef __SUPPORT_SAVE_TF_LOG__ 			
			TF_GPRS_LOG_INFO("Check Module Failed,Reset Module %d\n",s_4GHandle.nModuleResetCount);
#endif			

			//模组检测失败，播报语音
			SetSpeechContent(SPEECH_4G_MODULE_CHECK_FAILED);

			//复位模组
			CloseSerialPort(s_4GHandle.iSerial_fd);
			s_4GHandle.iSerial_fd = -1;	
			Reset4GModule();

            //这里须将当前使用SIM卡编号置位
            ParaAccessLock();
        	s_4GHandle.nCurSimCardIndex = 0;
        	ParaAccessUnLock();
			GprsSleep(MAX_MODULEPOWER_RESET_TIME);

			s_4GHandle.nModuleResetCount++;
		}
		else if (s_4GHandle.nUsbResetCount < MAX_USB_RESET_COUNT)
		{
			//模组检测失败，播报语音
			SetSpeechContent(SPEECH_4G_MODULE_CHECK_FAILED);
		
			//复位usb host
			ResetUsbHost();
			GprsSleep(MAX_USB_RESET_TIME);

			s_4GHandle.nUsbResetCount++;
		}
		else if (!s_4GHandle.nDeviceResetFlag)
		{
			MODULE4G_ERR("Check Module Failed,Reboot Device\n");
#ifdef __SUPPORT_SAVE_TF_LOG__ 			
			TF_GPRS_LOG_INFO("Check Module Failed,Reboot Device\n");
#endif			
		
			RebootLinux(REBOOT_GPRSDEVICE_REST);
		}
		else
		{
			MODULE4G_ERR("Check Module Failed,Speech\n");
#ifdef __SUPPORT_SAVE_TF_LOG__ 			
			TF_GPRS_LOG_INFO("Check Module Failed,Speech\n");
#endif			
		
			//设备检测失败，播报语音
			SetSpeechContent(SPEECH_4G_MODULE_CHECK_FAILED);
			
			ParaAccessLock();
			//检测无模组
			s_4GHandle.uModuleStatus = 0;
			s_4GHandle.uCheckErrorFlag = 1;
			ParaAccessUnLock();
		}
	}
	else
	{
		ParaAccessLock();
		s_4GHandle.uModuleStatus = 1;
		ParaAccessUnLock();
		
		SetSpeechContent(SPEECH_4G_MODULE_CHECK_SUCCESS);
		SwitchSubState(next_state);
	}

	return ret;
}

/*@ preif 从TF卡获取SIM卡产测配置联网SIM卡序号
 *@ return -1 出错
 *@        其他 SIM卡下标
 */
static int ReadProductSIMConfig()
{
	int ret = 0;
	FILE *pFd = NULL;	
	char *find_str = NULL;
	int nSimCardIndex = 0;

	pFd = fopen(PRODUCTTEST_SIM, "r");
	if (NULL == pFd)
	{
		MODULE4G_ERR("Open file fail! [%s]\n", PRODUCTTEST_SIM);
		return -1;
	}

	char pBuff[8] = {0};
	char sNo[2] = {0};

	do
	{
		if (fgets(pBuff, sizeof(pBuff), pFd) == NULL)
		{
            MODULE4G_ERR("fgets %s fail!\n", PRODUCTTEST_SIM);
            ret = -1;
            goto end;
		}
		
		find_str = strstr(pBuff,"SIM:");		
		if (NULL == find_str)
		{
			ret = -1;
			goto end;
		} else {
			memcpy(sNo, find_str+strlen("SIM:"), 1);
			nSimCardIndex = sNo[0]-'0';
			MODULE4G_INF("find_str[%s] -- sNo[%s] -- Index[%d]\r\n", find_str, sNo, nSimCardIndex);

            if ((nSimCardIndex < 0) || (nSimCardIndex > MAX_SIMCARD_CNT-1))
            {
                ret = -1;
    			goto end;
            }

		}
	}while(0);

end:
	MODULE4G_INF("ReadProductSIMConfig end [%s] -- sNo[%s] -- Index[%d]\r\n", find_str, sNo, nSimCardIndex);
		
    SAFE_FCLOSE(pFd);
	return ((ret == -1) ? ret : nSimCardIndex);
}


/*@ZXSG 模组，mifiFlag设置为1，非1则复位模组
*@param  
*@return 1 需要复位模组   0 否
*@
*/
#define MOUDULERESETTIME (2)
static int DealMiFiFlag(int next_state,int err_state)
{
	int ret = 0;

 	// 复位次数加限制, 避免因为mifiFlag 频繁复位
 	if(resettime >= MOUDULERESETTIME)
 	{
		MODULE4G_INF("[mififlag] reset times over %d , return \n", MOUDULERESETTIME);
		return 0;
	}

    //中云信安模组获取硬件版本号
	if (ZHONGYUN_ZX1602 != s_ModuleType)
	{
	    //获取模组内固件版本
    	if (-1 == SetATCommandQGMR())
    	{
    		goto end;
    	}
            
    	if ((0 == strncmp(s_4GHandle.sVersion, ModuleVersionType, strlen(ModuleVersionType)))
			|| (0 == strncmp(s_4GHandle.sVersion, ModuleVersionType_1605, strlen(ModuleVersionType_1605))))
    	{
    		MODULE4G_INF("[mifiFlag]1 detect 800 SG type \n");
    		s_IsSGModule = 1;
    	}
    	else
    	{
    		MODULE4G_INF("[mifiFlag] no SG !\n");
    		return 0;
    	}
         
    }
    else
    {
        if (-1 == GetModuleHardWareVersion())
    	{
            goto end;
        }
        
        if ((0 == strncmp(s_4GHandle.sHardVersion, ModuleVersionType, strlen(ModuleVersionType)))||
			(0 == strncmp(s_4GHandle.sHardVersion, ModuleVersionType_1605, strlen(ModuleVersionType_1605)))||
			(0 == strncmp(s_4GHandle.sHardVersion, ModuleVersionType_HaiSi, strlen(ModuleVersionType_HaiSi))))
        {
            MODULE4G_INF("[mifiFlag]1 detect 800 SG  / 800VG type \n");
            s_IsSGModule = 1;
        }
        else
        {
            MODULE4G_INF("[mifiFlag] no SG !\n");
            return 0;
        }
    }

	if(1 == s_IsSGModule)
	{	
		//获取mififlag, 如果不是1就置为1，同时复位4G模组
		ret = SetATCommandGetMIfIFlag();
		if (-1 == ret)
		{
			goto end; 
		}
		
		MODULE4G_INF("[mifiFlag] value: %d  resettime: %d \n", s_4GHandle.nmifiFlag, resettime);

		if (0 == access("/config/usr/local/etc/.mifiFlag", F_OK))
		{
			s_4GHandle.nmifiFlag = 0;
		}
		
		if (1 != s_4GHandle.nmifiFlag)
		{
			MODULE4G_INF("[mifiFlag] start to reset module \n");
			ret = SetATCommandSetMIfIFlag(1);
			if (-1 == ret)
			{
				goto end; 
			}

			// 立即复位
			s_4GHandle.uCheckErrorFlag = 1;
            s_4GHandle.uErrorNotSleepFlag = 1;
			resettime++;
			return 1;
			
		}
		else
		{
			MODULE4G_INF("[mifiFlag] 1 == nmifiFlag, will return ^_^ \n");
			return 0;
		}
			
	}

	end:
	if (-1 == ret)
	{
		s_4GHandle.uComErrorState = 0;
		SwitchSubState(err_state);
	}
	
	return 0;
}

/*@breif ModlueInitHandler 初始化模组
 *@param 
 *@return 
 *@       
 */
static int ModlueInitHandler(int next_state,int err_state)
{
	int ret = 0;

	Module4G_DailDown();

	if (s_4GHandle.iSerial_fd < 0)
	{
		ret = Open4GModule();
		if (-1 == ret)
		{
			goto end; 
		}
	}

	//at测试通讯
	ret = SetATCommandATTEST();
	if (-1 == ret)
	{
		goto end; 
	}

	//禁止回显
	ret = SetATCommandATE0();
	if (-1 == ret)
	{
		goto end; 
	}


    //这里如果硬件版本号为空的话，需要再获取一次
    if('\0'== s_4GHandle.sHardVersion[0])
    {
        ret = GetModuleHardWareVersion();	
    	if (-1 == ret)
    	{
    		goto end; 
    	}

        if(s_4GHandle.sHardVersion[5] == 'C')
        {
            //设置云卡设备能力集
            if(NULL != s_SetCloudCardCb)
            {
                s_SetCloudCardCb(1);
            }
            //如果是云卡版本，则设置为单卡
            Set4GRealSimCardCount(1);
        }
    }

	//只在中云1605模组上去做模式获取以及设置
	if(strstr(s_4GHandle.sHardVersion,"800VG"))
	{	
		ret = GetATCommandRSLPMode();
		if (-1 == ret)
		{
			goto end; 
		}

		//如果4G模组工作模式不为常电模式则需要设置为常电模式。
		if(s_4GHandle.iModuleMode){
			ret = SetATCommandRSLPMode(0);
			if (-1 == ret)
			{
				goto end; 
			}
			Reset4GModule();
			sleep(3);
			//复位模组后重新判断一次模组节点以及ID
			int ret = Module4GIsOnline();
			if(ret < 0)
			{
				MODULE4G_ERR("Module4GIsOnline ERR return %d\n", ret);
				goto end; 
			}
			
		}
	}
		

    // 不切到next_state, 只可能切换到err_state
	ret = DealMiFiFlag(next_state, err_state);
	if (1 == ret)
	{
		// 模组需要做复位操作，不需要再走下面这些AT
		goto end; 
	}

	// 获取协议类型
	ret = SetATCommandGetQCFG();
	MODULE4G_ERR("[attest] init get rndistype ret :%d \n", ret);
	if (-1 == ret)
	{
		goto end; 
	}

	MODULE4G_ERR("[attest] init get rndistype :%d \n", s_4GHandle.rndistype);

    //只设置一次
    if((1 != s_4GHandle.nDonotSendRndisFlag) || (QUECTEL_EC800 == s_ModuleType)||(QUECTEL_EG800 == s_ModuleType))
    {
    	if (3 != s_4GHandle.rndistype)
    	{
    		//设置协议 rndis	
    		ret = SetATCommandQCFG(3);
    		if (-1 == ret)
    		{
    			goto end; 
    		}

            s_4GHandle.nDonotSendRndisFlag = 1;
    	}
    }
	
	if (QUECTEL_EC800 == s_ModuleType || QUECTEL_EG800 == s_ModuleType)
	{
		// 获取网卡模式
		ret = SetATCommandGetQCFGNat();
		MODULE4G_ERR("[attest] init get netcardtype ret :%d \n", ret);
		if (-1 == ret)
		{
			goto end;
		}
		MODULE4G_ERR("[attest] init get netcardtype :%d \n", s_4GHandle.netcardtype);
		if (1 != s_4GHandle.netcardtype)
		{
			//设置网卡模式
			ret = SetATCommandQCFGNat(1);
			if (-1 == ret)
			{
				goto end; 
			}
		}

	}
#ifdef __MICRO_POWER_SUPPORT__
	if(QUECTEL_EG800 != s_ModuleType)
	{
		//设置增强型休眠
		ret = SetATCommandQSCLKEX();
		if (-1 == ret)
		{
			goto end; 
		}
	}
#endif

end:
	if (-1 == ret)
	{
		s_4GHandle.uComErrorState = 0;
		SwitchSubState(err_state);
	}
	else
	{
		SwitchSubState(next_state);
	}

	return ret;	
}

/*@breif CheckCSQSwitchSIMCard 双卡设备通过信号质量切换SIM卡
 *@param 
 *@return 
 *@ 	  
 */
static void CheckCSQSwitchSIMCard(G4_Control_Manager_t *fd)
{
	int i = 0;
	int ret = 0;

    /* 单卡设备直接退出 */
    if (fd->s_4GSimCardCount <= 1)
    {        
    	MODULE4G_INF("[return] Is One SIM Card !!\n");
        return;
    }

    do {
        /* 1、未授权时不做切换检测 */
        unsigned int nAuthState = 0;
        unsigned char szAuthCode[256] = {0};
        ZWSys_GetAuthInfo(szAuthCode, &nAuthState);
        if (0 == nAuthState)
        {
            break;
        }

        /* 2、产测指定SIM卡联网 */
        int Index = ReadProductSIMConfig();
        if (Index != -1)
        {
            if (Index != fd->nCurSimCardIndex)
            {
                MODULE4G_INF("[Force] Switch SIM card [%d] ==> [%d]\n", fd->nCurSimCardIndex, Index);
                ret = SetATCommandSwitchSim(Index);
                SetATCommandGetCurrentSwitchSim();
            }
            break;
        }

        /* 3、由于上次网络异常导致的切卡，此处不再做切换信号检测，直接删除标记报个假的信号值直接联网 */
        if (0 == access(DONT_CHANGESIM_FLAG, F_OK))
        {
            MODULE4G_INF("Remove %s\n", DONT_CHANGESIM_FLAG);
            SAFE_REMOVE(DONT_CHANGESIM_FLAG);
            fd->iCsq[fd->nCurSimCardIndex]= 90;
            break;
        }

        /* 4、产测模式，默认使用电信卡做产测 */
        if (0 == access(PRODUCTTEST_FILE, F_OK))
        {
            if (1 != fd->nCurSimCardIndex)
            {
                MODULE4G_INF("Switch SIM card [%d] ==> [1]\n", fd->nCurSimCardIndex);
                SetATCommandSwitchSim(1);
                SetATCommandGetCurrentSwitchSim();
				MODULE4G_INF("Switch SIM card end\n", fd->nCurSimCardIndex);
            }
            break;
        }

        /* 5、用户已绑定，直接退出，不做信号检测切卡动作 */
        char acSCode[256] = {0};
        int iSCodeLen = sizeof(acSCode);
        //ret = MPU_SYS_DeviceBind_GetSCode(acSCode, &iSCodeLen);
    	ret = fd->GetSCode(acSCode, &iSCodeLen);
        if (((0 == ret) && (iSCodeLen > 0)) || (0 == access("/data/config/config/noSupportShare",F_OK)))
		{
			MODULE4G_INF("ret = %d,iSCodeLen = %d or existence /data/config/config/noSupportShare\n",ret,iSCodeLen);
			break;
        }

        /* 6、用户未绑定，且不是产测模式，该场景是最终用户绑定前 */
        MODULE4G_INF("Start Check!\n");
#if 1
        //方便测试使用
        int T_CSQ0 = -1, T_CSQ1 = -1;
        if (access(TEST_SIM_CSQ, F_OK) == 0)
        {
            char *endptr;
            char result[8] = {0};
            char command[128] = {0};

            snprintf(command, sizeof(command), "awk -F: '/CSQ0/ {print $2}' %s", TEST_SIM_CSQ);
            ret = ShellCmdOut(command, result, sizeof(result));
            if (ret == 0)
            {
                T_CSQ0 = strtol(result, &endptr, 10);
            }

            snprintf(command, sizeof(command), "awk -F: '/CSQ1/ {print $2}' %s", TEST_SIM_CSQ);
            ret = ShellCmdOut(command, result, sizeof(result));
            if (ret == 0)
            {
                T_CSQ1 = strtol(result, &endptr, 10);
            }

            MODULE4G_INF("CSQ_TEST: T_CSQ [%d---%d]", T_CSQ0, T_CSQ1);
        }
#endif

        //多卡设备 保障每次起来初始连接的是SIM卡2(电信卡)
        if (fd->nCurSimCardIndex != 1)
        {
            MODULE4G_INF("Switch SIM card [%d] ==> [1]\n", fd->nCurSimCardIndex);
            ret = SetATCommandSwitchSim(1);
            if (-1 == ret) {
                fd->uSimCardStatus[1] = ABNORMAL;
                MODULE4G_ERR("Switch SIM2 card failed!\n");
            }
            else {
                sleep(10);
            }
            SetATCommandGetCurrentSwitchSim();
        }

        //如果当前SIM卡仍然是SIM卡1，则说明切换SIM卡失败
        if (fd->nCurSimCardIndex != 1)
        {
            MODULE4G_INF("Switch SIM card failed!\n");
            break;
        }

        //内置卡查询信号质量稍慢，可能需等待一会，基本上等两秒左右就能查到
        for (i = 0; i < 3; i++)
        {
            ret = SetATCommandCSQ();

            if (fd->iCsq[fd->nCurSimCardIndex] != 0)
            {
                fd->uSimCardStatus[fd->nCurSimCardIndex] = NORMAL;
                MODULE4G_INF("ProduceTestNet, s_4GHandle.iCsq[%d]:%d\n", fd->nCurSimCardIndex, fd->iCsq[fd->nCurSimCardIndex]);
                break;
            }
            sleep(2);
            if (2 == i)
            {
                fd->uSimCardStatus[fd->nCurSimCardIndex] = ABNORMAL;
            }
        }

        //方便测试使用
        fd->iCsq[fd->nCurSimCardIndex] = ((T_CSQ1 == -1) ? fd->iCsq[fd->nCurSimCardIndex] : T_CSQ1);

        //通过信号质量判断是否需要做切换SIM卡动作
        if (fd->iCsq[fd->nCurSimCardIndex] >= 70)
        {
            fd->uSimCardStatus[fd->nCurSimCardIndex] = NORMAL;
        }
        else
        {
            /* 切换至SIM卡1(移动卡) */
            MODULE4G_INF("Switch SIM card [%d] ==> [0]\n", fd->nCurSimCardIndex);
            ret = SetATCommandSwitchSim(0);
            if (-1 == ret) {
                fd->uSimCardStatus[0] = ABNORMAL;
                MODULE4G_ERR("Switch SIM1 card failed!\n");
            }
            else {
                sleep(10);
            }
            /* 切换后重新获取当前使用的SIM卡 */
            SetATCommandGetCurrentSwitchSim();

            //如果当前SIM卡仍然是SIM卡2，则说明切换SIM卡失败
            if (fd->nCurSimCardIndex != 0)
            {
                MODULE4G_INF("Switch SIM card failed!\n");
                break;
            }

            for (i= 0; i < 3; i++)
            {
                ret = SetATCommandCSQ();

                if (fd->iCsq[fd->nCurSimCardIndex] != 0)
                {
                    fd->uSimCardStatus[fd->nCurSimCardIndex] = NORMAL;
                    MODULE4G_INF("ProduceTestNet, s_4GHandle.iCsq[%d]:%d\n", fd->nCurSimCardIndex, fd->iCsq[fd->nCurSimCardIndex]);
                    break;
                }
                sleep(2);
                if (2 == i)
                {
                    fd->uSimCardStatus[fd->nCurSimCardIndex] = ABNORMAL;
                }
            }

            //方便测试使用
            fd->iCsq[fd->nCurSimCardIndex] = ((T_CSQ0 == -1) ? fd->iCsq[fd->nCurSimCardIndex] : T_CSQ0);

             if ((fd->iCsq[0] < 70) || (fd->iCsq[0] < fd->iCsq[1]))
            {
                MODULE4G_INF("Switch SIM card [%d] ==> [1]\n", fd->nCurSimCardIndex);
                ret = SetATCommandSwitchSim(1);
                if (-1 == ret) {
                    fd->uSimCardStatus[1] = ABNORMAL;
                    MODULE4G_ERR("Switch SIM2 card failed!\n");
                }
                else {
                    sleep(10);
                }
                /* 切换后重新获取当前使用的SIM卡 */
                SetATCommandGetCurrentSwitchSim();
            }
        }

		MODULE4G_INF("end Switch SIM card [%d]\n", fd->nCurSimCardIndex);
    } while(0);

	return; 
}

#define _TEST_4GERROR   (0)
/*@breif BindGetCurSimHandler 获取当前使用sim卡
 *@param 
 *@return 
 *@       
 */
static int BindCheckSimHandler(int next_state,int err_state)
{
	G4_Control_Manager_t *fd = NULL;
		Module4G_GetSystemHandleInfo(&fd);
		int i = 0;
		int ret = 0;
	
		if (fd->ucWorkMode == 1)
		{
			SwitchSubState(next_state);
			return ret;
		}
	
		ret = SetATCommandCMEE(1);
	
		ret = SetATCommandGetCurrentSwitchSim();
		if (-1 == ret)
		{
			MODULE4G_ERR("GetCurrentSwitchSim is abnormal !\n");
			goto end; 
		}
	
		/* 判断是否需要切换SIM卡逻辑 */
		CheckCSQSwitchSIMCard(fd);
	
		//连续3次
		for (i = 0; i < 3; i++)
		{
			ret = SetATCommandCPIN();
			if (0 == ret)
			{
				break;
			}
			sleep(2);
		}
		
		if(i >= 3)
		{
			fd->uSimCardStatus[fd->nCurSimCardIndex] = ABNORMAL;
			goto end1;
		}
		//连续3次对单张卡检测不到才认为检测不到
		for (i = 0; i < 3; i++)
		{
			ret = SetATCommandICCID();	//若未插卡则返回的是-1
			if (0 == ret)
			{
				break;
			}
		}

		GetATCommandSIM01CardStatus();
#if _TEST_4GERROR
		if (0 == access("/mnt/mtd/test_simfail", F_OK))
		{
			MODULE4G_INF("Test SIM card detection failed\n");
			ret = -1;
		}
#endif
	
		if (-1 == ret
			|| ((0 == fd->sim_01_card_status) &&  (1 == fd->device_did_info) && ('C' == s_4GHandle.sHardVersion[5])))   //云卡设备：新版本模组，非YL号段。走云卡
		{
			fd->uSimCardStatus[fd->nCurSimCardIndex] = ABNORMAL;
		}
		
	end1:
	
		if (ABNORMAL == fd->uSimCardStatus[fd->nCurSimCardIndex])
		{
			if(fd->s_SwitchCardsState == 1)
			{
				MODULE4G_INF("SwitchSim ERR!!!\r\n");
				//要切的卡不存在，切回去，结束切卡
				ChangeOtherSim();
				ParaAccessLock();
				fd->s_SwitchCardsState = 0;
				fd->s_SwitchCardsResult = 1;//没检测到sim卡
				ParaAccessUnLock();
				SwitchSubState(CHECK_SIM_STATE);
				return 0;
			}
			//卡不存在，报sim卡检测错误语音
			SetSpeechContent(SPEECH_4G_SIM_CHECK_FAILED);
	
			fd->uCheckErrorFlag = 1;
			s_4GNetErrTimes[SimCardErr]++;
			MODULE4G_ERR("s_4GNetErrTimes[SimCardErr] :%d \n", s_4GNetErrTimes[SimCardErr]);
	
			return -1;
		}
		else if (NORMAL == fd->uSimCardStatus[fd->nCurSimCardIndex])
		{
			s_4GNetErrTimes[SimCardErr] = 0;
		}
	
		//卡存在，报sim卡检测成功
		SetSpeechContent(SPEECH_4G_SIM_CHECK_SUCCESS);
		//DealNetLinkLedStatus(NETWORK_CONNECTING);
	
	end:
		if (-1 == ret)
		{
			fd->uComErrorState = 0;
			SwitchSubState(err_state);
		}
		else
		{
			SwitchSubState(next_state);
		}
	
		return ret;

}


/*@breif LinkSpeechHandler 连接语音播报控制
 *@param 
 *@return 
 *@       
 */
static void LinkSpeechHandler()
{
	int ConnStatus = 0;
	int Control = 0;
	int uWireConnStatus = 0;
	int nTutkStatus = 0;

	ParaAccessLock();
	ConnStatus = s_4GHandle.uInternetConnStatus;
	Control = s_4GHandle.eLinkSpeechControl;
	//ProductMode = s_4GHandle.uProductMode;
	uWireConnStatus = s_4GHandle.uWireConnStatus;
	nTutkStatus = s_4GHandle.nTutkStatus;
	ParaAccessUnLock();

	//防止开机产测，导致播报语音
	if (TASK_USE4G_STATE != s_4GHandle.uState)
	{
		ConnStatus = 0;
	}
	
	if (LINKSPEECH_ORIGINAL == Control || LINKSPEECH_NOTORIGINAL_UNBIND == Control)
	{
		if (ConnStatus && (2 == nTutkStatus))
		{
			//播报网络连接成功
			SetSpeechContent(SPEECH_4G_CONNECT_SUCCESS);
		}
	}
	else if (LINKSPEECH_NOTORIGINAL_BIND == Control)
	{
		if (0 == uWireConnStatus)
		{
			//播报非原厂卡
			SetSpeechContent(SPEECH_4G_SIM_NOT_ORIGINAL);
		}
	}
}


/*@breif GetInfoHandler 获取信息
 *@param 
 *@return 
 *@       
 */
static int GetInfoHandler(int next_state,int err_state)
{
	int ret = 0;
	
    ret = SetATCommandGetCurrentSwitchSim();
	if (-1 == ret)
	{
		MODULE4G_ERR("GetInfoHandler SetATCommandGetCurrentSwitchSim  is abnormal !\n");
		goto end; 
	}
	
	//查询ICCID--位置提前，优先判断卡状态
	ret = SetATCommandICCID();
	if (-1 == ret)
	{
		MODULE4G_ERR("s_4GHandle.uSimCardStatus is abnormal !\n");
		goto end; 
	}

	//获取模组当前版本
	ret = SetATCommandQGMR();
	if (-1 == ret)
	{
		goto end; 
	}

	// 获取硬件版本号
	ret = GetModuleHardWareVersion();	
	if (-1 == ret)
	{
		goto end; 
	}

	//获取IMEI
	ret = SetATCommandCGSN();
	if (-1 == ret)
	{
		goto end; 
	}

	//获取IMSI
	ret = SetATCommandCIMI();
	if (-1 == ret)
	{
		goto end; 
	}
	
	if(ZHONGYUN_ZX800HG == s_ModuleType)
	{
		//查询act oper前先设置
		ret = SetATCommandCOPSNUM();
		if (-1 == ret)
		{
			goto end; 
		}
	}

	//查询act oper
	ret = SetATCommandCOPS();
	if (-1 == ret)
	{
		goto end; 
	}

	//查询band
	ret = SetATCommandQNWINFO();
	if (-1 == ret)
	{
		goto end; 
	}
	//移动卡自动切换频段操作
	if ((strstr(s_4GHandle.sOper[s_4GHandle.nCurSimCardIndex],"CHINA MOBILE")) && (ZHONGYUN_ZX1602 == s_ModuleType || ZHONGYUN_ZX800SG == s_ModuleType || ZHONGYUN_ZX800HG == s_ModuleType ))
	{
		/*获取 TDD 当前的频段信息*/
		SetATCommandBAND();
		/*强制切换 FDD 号段*/
		SetATCommandChinaMobileFDDBAND();
		sleep(10);
		/*获取信号值*/
		SetATCommandCSQ();
		MODULE4G_INF("China Mobile 4G CSQ = %d\n", s_4GHandle.iCsq[s_4GHandle.nCurSimCardIndex]);
		if (s_4GHandle.iCsq[s_4GHandle.nCurSimCardIndex] < 70)
		{
			/*如果FDD号段信号质量小于 70 则切换到自动选择频段*/
			SetATCommandChinaMobileAutoBAND();
			MODULE4G_INF("China Mobile CHG To Auto BAND\n");
			sleep(10);
		}
	}

end:
	if(s_4GHandle.s_SwitchCardsState == 1 && -1 == ret)
	{
		MODULE4G_INF("GetInfoHandler ERR!!!\r\n");
		//获取SIM卡信息失败，切回去，结束切卡
		ChangeOtherSim();
		ParaAccessLock();
		s_4GHandle.s_SwitchCardsState = 0;
		s_4GHandle.s_SwitchCardsResult = 2;//获取SIM卡信息失败
		ParaAccessUnLock();
		SwitchSubState(CHECK_SIM_STATE);
		return 0;
	}

	if (-1 == ret)
	{
		s_4GHandle.uComErrorState = 0;
		SwitchSubState(err_state);
	}
	else
	{
		SwitchSubState(next_state);
	}

	return ret;	
}

/*@breif EnterAirHandler 进入飞行模式
 *@param 
 *@return 
 *@       
 */
static int EnterAirHandler(int next_state,int err_state)
{
	int ret = 0;

	//把网络状态设置为不通
 	Module4G_SetModuleInternetConnStatus(0);

	//取消网卡绑定
	ret = SetATCommandQNETDEVCTL(0);
	if (-1 == ret)
	{
		goto end; 
	}

	//进入飞行模式
	ret = SetATCommandCFUN(0);
	if (-1 == ret)
	{
		goto end; 
	}

	GprsSleep(AIR_WAIT_TIME);

end:
	if (-1 == ret)
	{
		s_4GHandle.uComErrorState = 0;
		SwitchSubState(err_state);
	}
	else
	{
		SwitchSubState(next_state);
	}

	return ret;		
}

/*@breif QuitAirHandler 退出飞行模式
 *@param 
 *@return 
 *@       
 */
static int QuitAirHandler(int next_state,int err_state)
{
	int ret = 0;

	//把网络状态设置为不通
 	Module4G_SetModuleInternetConnStatus(0);

	//退出飞行模式
	ret = SetATCommandCFUN(1);
	if (-1 == ret)
	{
		goto end; 
	}

	GprsSleep(AIR_WAIT_TIME);

end:
	if (-1 == ret)
	{
		s_4GHandle.uComErrorState = 0;
		SwitchSubState(err_state);
	}
	else
	{
		SwitchSubState(next_state);
	}

	return ret;
}

/*@breif ECMDialHandler ecm拨号
 *@param 
 *@return 
 *@       
 */
static int ECMDialHandler(int next_state,int err_state)
{
	int ret = 0;

	ret = SetATCommandQNETDEVCTL(1);
	if (-1 == ret)
	{
		goto end; 
	}

end:
	if(s_4GHandle.s_SwitchCardsState == 1 && -1 == ret)
	{
		MODULE4G_INF("ECMDialHandler ERR!!!\r\n");
		//拨号失败，切回去，结束切卡
		ChangeOtherSim();
		ParaAccessLock();
		s_4GHandle.s_SwitchCardsState = 0;
		s_4GHandle.s_SwitchCardsResult = 3;//拨号失败
		ParaAccessUnLock();
		SwitchSubState(CHECK_SIM_STATE);
		return 0;
	}

	if (-1 == ret)
	{
		s_4GHandle.uComErrorState = 0;
		SwitchSubState(err_state);
	}
	else
	{
		SwitchSubState(next_state);
	}

	return ret;
}


/*@breif BindCheckDialStateHandler 拨号状态检测
*@param 
*@return 
*@       
*/
static int BindCheckDialStateHandler(int next_state,int err_state)
{
	int ret = 0;

	ret = SetATCommandGetQNETDEVCTL();
	if (-1 == ret)
	{
		goto end; 
	}

	#if 0
	int RetRoute =0;
	Get4GErrType(&RetRoute);
	MODULE4G_ERR("[testneterr] s_4GHandle.nDialState :%d \n", s_4GHandle.nDialState);
	#endif
	if (!s_4GHandle.nDialState)
	{
		s_4GHandle.nDialStateCheckCount++;
		if (s_4GHandle.nDialStateCheckCount > DIAL_MAX_TIME)
		{
		
#ifdef __SUPPORT_SAVE_TF_LOG__ 
			TF_GPRS_LOG_INFO("Check Dial Failed!\n");
#endif

#ifdef __SUPPORT_SAVE_TF_LOG__ 
			if (1 == s_4GHandle.uInternetConnStatus)
			{
				TF_GPRS_LOG_INFO("4G Link Failed,Dial!\n");
			}
#endif
			if(s_4GHandle.s_SwitchCardsState == 1)
			{
				MODULE4G_INF("BindCheckDialStateHandler ERR!!!\r\n");
				//拨号失败，切回去，结束切卡
				ChangeOtherSim();
				ParaAccessLock();
				s_4GHandle.s_SwitchCardsState = 0;
				s_4GHandle.s_SwitchCardsResult = 3;//拨号失败
				ParaAccessUnLock();
				SwitchSubState(CHECK_SIM_STATE);
				return 0;
			}

			s_4GHandle.uInternetConnStatus = 0;
            s_4GHandle.uRealNetConnStatus = 0;


			s_4GHandle.uCheckErrorFlag = 1;

			s_4GNetErrTimes[DialErr]++;
			MODULE4G_ERR("s_4GNetErrTimes[DialErr] :%d \n", s_4GNetErrTimes[DialErr]);
			return -1;
		}

		return 1;
	}

	s_4GNetErrTimes[DialErr]=0;
	Module4G_StartDial();

	ParseDnsAddr();
	UpdateDnsAddr();

    //SG模组获取公网dns
    if (1 == s_IsSGModule)
    {
        ret = SetATCommandGetDNS();
        if (-1 == ret)
        {
            goto end; 
        }
    }

end:
	if(s_4GHandle.s_SwitchCardsState == 1 && -1 == ret)
	{
		MODULE4G_INF("BindCheckDialStateHandler ERR!!!\r\n");
		//拨号失败，切回去，结束切卡
		ChangeOtherSim();
		ParaAccessLock();
		s_4GHandle.s_SwitchCardsState = 0;
		s_4GHandle.s_SwitchCardsResult = 3;//拨号失败
		ParaAccessUnLock();
		SwitchSubState(CHECK_SIM_STATE);
		return 0;
	}

	if (-1 == ret)
	{
		s_4GHandle.uComErrorState = 0;
		SwitchSubState(err_state);
	}
	else
	{
		SwitchSubState(next_state);
	}

	return ret;
}


/*@breif BindCheckCregHandler creg检测
 *@param 
 *@return 
 *@       
 */
static int BindCheckCregHandler(int next_state,int air_state,int err_state)
{
	int ret = 0;

	int creg_max_time = 70;

	ret = SetATCommandSetCEREG(0);
	if (-1 == ret)
	{
		goto end; 
	}

	if (1 == s_4GHandle.s_IsSgOrVg4gModule)
	{
		ret = SetATCommandSetCEREG(2);
	}
	else
	{
		ret = SetATCommandSetCEREG(0);
	}


	ret = SetATCommandGetCEREG();
	if (-1 == ret)
	{
		goto end; 
	}

	if (QUECTEL_EC800 == s_ModuleType || QUECTEL_EG800 == s_ModuleType)
	{
		if (!s_4GHandle.uCregStatus[s_4GHandle.nCurSimCardIndex])
		{
			ret = SetATCommandSetCGREG(0);
			if (-1 == ret)
			{
				MODULE4G_ERR(" send CGREG cmd err \n");
				goto end; 
			}

			ret = SetATCommandGetCGREG();
			if (-1 == ret)
			{
				MODULE4G_ERR(" send CGREG cmd err \n");
				goto end; 
			}
		}
	}

	if (!s_4GHandle.uCregStatus[s_4GHandle.nCurSimCardIndex])
	{
		s_4GHandle.nCregCheckCount++;
/*
优化：
1、非云卡设备（使用在国内），无法连接网络时，最长2分钟内要播放网络连接失败
2、云卡设备，使用云卡连接网络时，如果无法连接，最长2分钟内要播放网络连接失败
3、云卡设备，使用外置卡连接网络时，如果无法连接，最长5分钟内要播放网络连接失败
*/
		if(!(s_4GHandle.sHardVersion[5] == 'C'))
		{
			//2分钟，AT命令会消耗一些时间，70次尝试，大约2分钟 
			creg_max_time = 70;
		}
		else if((s_4GHandle.sHardVersion[5] == 'C') && (s_4GHandle.ucWorkMode == 1))
		{
			creg_max_time = 70;
		}
		else if((s_4GHandle.sHardVersion[5] == 'C') && (s_4GHandle.ucWorkMode == 0))
		{
			//5分钟
			creg_max_time = 175;
		}


		MODULE4G_INF("Creg fail CheckCount: %d, CREG_MAX_TIME=%d\n", s_4GHandle.nCregCheckCount, creg_max_time);

		// 第一次等待时间要短一些30S 之后大约是2min
        if ((20 == s_4GHandle.nCregCheckCount) || (20+ CREG_BROADCAST_TIME == s_4GHandle.nCregCheckCount) || (20 + 2*CREG_BROADCAST_TIME == s_4GHandle.nCregCheckCount))
        {
            //等待了 CREG_BROADCAST_TIME，播报网络连接中语音
			//网络连接中，请稍候...
			SetSpeechContent(SPEECH_4G_CONNECTING_PLEASE_WAIT);       
        }
		if (s_4GHandle.nCregCheckCount > creg_max_time)
		{			
			if(s_4GHandle.s_SwitchCardsState == 1)
			{
				MODULE4G_INF("BindCheckCregHandler ERR!!!\r\n");
				//入网失败，切回去，结束切卡
				ChangeOtherSim();
				ParaAccessLock();
				s_4GHandle.s_SwitchCardsState = 0;
				s_4GHandle.s_SwitchCardsResult = 4;//入网失败
				ParaAccessUnLock();
				SwitchSubState(CHECK_SIM_STATE);
				return 0;
			}
			s_4GHandle.nCregCheckCount = 0;
			//未入网，播报语音
			SetSpeechContent(SPEECH_4G_CONNECT_FAILED_CHECK_SIMCARD);

#ifdef __SUPPORT_SAVE_TF_LOG__ 
			TF_GPRS_LOG_INFO("Check Not Register CregStatus\n");
#endif		

#ifdef __SUPPORT_SAVE_TF_LOG__ 
			if (1 == s_4GHandle.uInternetConnStatus)
			{
				TF_GPRS_LOG_INFO("4G Link Failed!\n");
			}
#endif

			s_4GHandle.uInternetConnStatus = 0;
            s_4GHandle.uRealNetConnStatus = 0;

			s_4GHandle.uCheckErrorFlag = 1;
            s_4GHandle.uErrorNotSleepFlag = 1;
			s_4GNetErrTimes[CregErr]++;
			MODULE4G_ERR("s_4GNetErrTimes[CregErr] :%d \n", s_4GNetErrTimes[CregErr]);

			return -1;

#if 0
			s_4GHandle.nAirPlaneTestCount++;
			if (s_4GHandle.nAirPlaneTestCount >= MAX_AIRPLANE_COUNT)
			{
#ifdef __SUPPORT_SAVE_TF_LOG__ 		
				TF_GPRS_LOG_INFO("Not Register CregStatus Enter air %d\n",s_4GHandle.nAirPlaneTestCount);
#endif			
				s_4GHandle.nAirPlaneTestCount = 0;
				s_4GHandle.uCheckErrorFlag = 1;


#ifdef __SUPPORT_SAVE_TF_LOG__ 
				if (1 == s_4GHandle.uInternetConnStatus)
				{
					TF_GPRS_LOG_INFO("4G Link Failed!\n");
				}
#endif
				s_4GHandle.uInternetConnStatus = 0;
				
				return -1;
				
			}
			else
			{
				SwitchSubState(air_state);
				return -1;
			}
#endif	
		}
		else
		{
			return 1;
		}
	}
	s_4GNetErrTimes[CregErr]=0;
	s_4GHandle.nCregCheckCount = 0;

end:
	if(s_4GHandle.s_SwitchCardsState == 1 && -1 == ret)
	{
		MODULE4G_INF("BindCheckCregHandler ERR!!!\r\n");
		//入网失败，切回去，结束切卡
		ChangeOtherSim();
		ParaAccessLock();
		s_4GHandle.s_SwitchCardsState = 0;
		s_4GHandle.s_SwitchCardsResult = 4;//入网失败
		ParaAccessUnLock();
		SwitchSubState(CHECK_SIM_STATE);
		return 0;
	}
	if (-1 == ret)
	{
		s_4GHandle.uComErrorState = 0;
		SwitchSubState(err_state);
	}
	else
	{
		SwitchSubState(next_state);
	}

	return ret;
}


/*@breif CheckCsqHandler 获取csq数据
 *@param 
 *@return 
 *@       
 */
static int CheckCsqHandler(int next_state,int err_state)
{
	static int powercount = 0;

	int ret = 0;
	int signalvalue = 0;
	ret = SetATCommandCSQ();
	if (-1 == ret)
	{
		goto end; 
	}
	
	// 通知sdk, 播报信号质量
	ParaAccessLock();
	signalvalue = s_4GHandle.iCsq[s_4GHandle.nCurSimCardIndex];
	ParaAccessUnLock();
	MODULE4G_INF("init signalvalue :%d \n", signalvalue);
	signalvalue = signalvalue < 0 ? 0:signalvalue;
	signalvalue = signalvalue >= 100 ? 99:signalvalue;
	MODULE4G_INF("init after signalvalue :%d,  powercount=%d\n", signalvalue, powercount);

	SetSpeechContent(SPEECH_4G_SIGNAL_0+signalvalue);

	//第一次快速进入，不用等一个周期
	s_4GHandle.nPingControltime = PING_PERIOD;

end:
	if(s_4GHandle.s_SwitchCardsState == 1 && (-1 == ret || 40 >= s_4GHandle.iCsq[s_4GHandle.nCurSimCardIndex]))
	{
		MODULE4G_INF("BindCheckDialStateHandler ERR!!!\r\n");
		//查询信号失败或信号小于40，切回去，结束切卡
		ChangeOtherSim();
		ParaAccessLock();
		s_4GHandle.s_SwitchCardsState = 0;
		s_4GHandle.s_SwitchCardsResult = 5;//信号异常，返回切卡失败
		ParaAccessUnLock();
		SwitchSubState(CHECK_SIM_STATE);
		return 0;
	}
	if (-1 == ret)
	{
		s_4GHandle.uComErrorState = 0;
		SwitchSubState(err_state);
	}
	else
	{
		SwitchSubState(next_state);
	}

	return ret;
}


//ping 顺序处理
static int PingCheck(void)
{
	int ret = 0;
	int i = 0;
	// 如果不是SG 模组, dns按dhcp获取到的
	if (0 == s_IsSGModule)
	{
		for (i = 0;i < DNS_ADDR_NUM;i++)
		{
			if (strlen(s_4GHandle.szDnsAddr[i]) > 0)
			{
			    MODULE4G_INF("[netdns] s_4GHandle.szDnsAddr[%d]   %s  strlen  %d \n", i, s_4GHandle.szDnsAddr[i], strlen(s_4GHandle.szDnsAddr[i]));
				ret = myping(MEIG_SLM322M_ETH,s_4GHandle.szDnsAddr[i]);
				if (1 == ret)
				{
					return ret;
				}
			}
		}
	}
	else
	{
		for (i = 0;i < 2;i++)
		{
			if (strlen(s_4GHandle.szATDnsAddr[i]) > 0)
			{
				MODULE4G_INF("[netdns] s_4GHandle.szATDnsAddr[%d]   %s  strlen  %d \n", i, s_4GHandle.szATDnsAddr[i], strlen(s_4GHandle.szATDnsAddr[i]));
				ret = myping(MEIG_SLM322M_ETH,s_4GHandle.szATDnsAddr[i]);
				if (1 == ret)
				{
					return ret;
				}
			}
		}
	}

	const char *PingHostAddr = NULL;
	const char *PingHostAddrBak = NULL;
	if(s_4GHandle.device_did_info){
		PingHostAddr = PING_HOST_ADDR_YL;
		PingHostAddrBak = PING_HOST_ADDR_YL_BAK;
	}
	else{
		PingHostAddr = PING_HOST_ADDR;
		PingHostAddrBak = PING_HOST_ADDR_BAK;
	}

    MODULE4G_INF("[netdns] %s  strlen  %d \n", PingHostAddr, strlen(PingHostAddr));
	ret = myping(MEIG_SLM322M_ETH,PingHostAddr);
	if (1 == ret)
	{
		return ret;
	}

    MODULE4G_INF("[netdns] %s  strlen  %d \n", PingHostAddrBak, strlen(PingHostAddrBak));
	ret = myping(MEIG_SLM322M_ETH,PingHostAddrBak);

	return ret;
}

/*@breif BindCheckPingHandler ping检测
 *@param 
 *@return -1 :失败
 *@       1 :成功，为了退出上层循环
 */
static int BindCheckPingHandler(int next_state,int air_state,int err_state)
{
	int ret = 0;
	//第一次快速检测（仅赋值以保持行为，外部逻辑不依赖其值；标注 (void) 抑制 set-but-not-used 告警）
	static int first_fast_check = 1;
	(void)first_fast_check;
	static int IsRouteErr = 0;

	int max_ping_failed_count = MAX_PING_FAILED_COUNT;
	if (0 != s_4GNetErrTimes[DhcpErr])//dhcp有问题时，改为连续ping60次，约2分钟
	{
		max_ping_failed_count = MAX_PING_FAILED_COUNT_MODIFY;
	}

    if(1 == s_4GHandle.ucWorkMode)
    {
        ret = GetATCommandUCWKSTATUSther();
        if (-1 == ret)
        {
            goto end; 
        }

        if(1 == s_4GHandle.ucOpenusStatus)
        {
            //表示云卡正在启动中
            MODULE4G_INF("4G mode ucworkstatus == 1 replacement card starting!!!\n");
            SwitchSubState(CLOUD_CARD_STATE);
            return 0;
        }

        if(2 == s_4GHandle.ucOpenusStatus && 100 == s_4GHandle.ucOpenusProcess && 0 == s_4GHandle.ucOpenusErrno)
        {
            //表示云卡已启动完成
            MODULE4G_INF("4G mode ucworkstatus == 2,100,0 unite card start OK!!!\n");
        }

        //异常处理
        if(3 == s_4GHandle.ucOpenusStatus)
        {
            MODULE4G_INF("4G mode ucworkstatus == 3 replacement card!!!\n");
            SwitchSubState(CLOUD_CARD_STATE);
            return 0;
        }
    }
    
	ret = SetATCommandGetCEREG();
	if (-1 == ret)
	{
		goto end; 
	}

	if (QUECTEL_EC800 == s_ModuleType || QUECTEL_EG800 == s_ModuleType)
	{
		if (!s_4GHandle.uCregStatus[s_4GHandle.nCurSimCardIndex])
		{
			ret = SetATCommandGetCGREG();
			if (-1 == ret)
			{
				goto end; 
			}
		}
	}

	ret = SetATCommandGetQNETDEVCTL();
	if (-1 == ret)
	{
		goto end; 
	}

	//产测时未获取到当前卡的运营商名称，在checkping时可一直尝试刷新获取(发生概率较小)
	int opsLength = strlen(s_4GHandle.sOper[s_4GHandle.nCurSimCardIndex]);
	if (0 == opsLength)
	{
		MODULE4G_INF("ProduceTestNet() no ops, now checkping to get ops simcardindex=%d\n", s_4GHandle.nCurSimCardIndex);
		if(ZHONGYUN_ZX800HG == s_ModuleType)
		{
			//查询act oper前先设置
			ret = SetATCommandCOPSNUM();
			if (-1 == ret)
			{
				goto end; 
			}
		}
		ret = SetATCommandCOPS();
		if (-1 == ret)
		{
			goto end; 
		}
	}

	if ((0 == s_4GHandle.uCregStatus[s_4GHandle.nCurSimCardIndex]) || (0 == s_4GHandle.nDialState))
	{
		//未入网或者拨号掉了
		s_4GHandle.nPingControltime = PING_PERIOD;
	}
	s_4GHandle.nPingControltime++;

	//if (s_4GHandle.uInternetConnStatus || (0 == first_fast_check))
	if (s_4GHandle.uInternetConnStatus)
	{
		if (s_4GHandle.nPingControltime < PING_PERIOD)
		{
			//未到检测周期，退出，同时也退出外层循环

			//MODULE4G_INF("000 s_4GHandle.nPingControltime=%d, first_fast_check=%d\n", s_4GHandle.nPingControltime, first_fast_check);
			return 1;
		}
	}
	else
	{
		//网络状态未切换时，1s一次
		if (s_4GHandle.nPingControltime < 1)
		{
			//未到检测周期，退出，同时也退出外层循环
		
			//MODULE4G_INF("111 s_4GHandle.nPingControltime=%d, first_fast_check=%d\n", s_4GHandle.nPingControltime, first_fast_check);
			return 1;
		}
	}

	s_4GHandle.nPingControltime = 0;
	//背景：
	//目前获取信号强度有两个指令SetATCommandCSQ，和SetATCommandQENG，只是返回格式不同。前者是0-31之间，要计算转化为具体数值。后者是负数，根据其大小判定0-4等级
	//获取双卡信息接口/System/DeviceSIMCardInfo(产测数据中的信号强度)是通过发的SetATCommandCSQ命令去获取信号强度只是开机时获取,后续不会更新。要显示格数，app根据具体数值去转换0—4数值
	//之前预览界面获取信号强度的老接口/Network/Interfaces/3/4GCardInfo是通过SetATCommandQENG()获取信号强度显示在预览界面,会更新。设备已经返回具体的0-4数值，直接显示格数
	
	//修改原因：
	//目前安卓已经预览界面信号强度换成了新的获取双卡信息接口，用双卡信息接口中的信号强度在预览界面显示
	//因此在这里SetATCommandCSQ也要把这个信号强度不断更新到当前卡的信号强度中，让用户看到实际的信号强度

	//相关：
	//目前ios仍然是通过老接口(/Network/Interfaces/3/4GCardInfo)通过SetATCommandQENG()获取信号强度显示在预览界面
	//因此ios预览界面会存在通过老接口利用SetATCommandQENG获取信号强度时在ping时会更新，而获取双卡具体信息接口利用SetATCommandCSQ获取强度不会更新
	//这样会导致双卡具体信息界面中当前使用卡的信号强度和预览界面强度不一致。据欧阳志昂反馈，后续ios也会调用双卡信息接口

	//待ios修改后,所有的信号强度都会保持为当前使用卡的具体信号强度，且会更新
	
	ret = SetATCommandCSQ();
	if (-1 == ret)
	{
		goto end; 
	}

	ret = SetATCommandGetCEREG();
	if (-1 == ret)
	{
		goto end; 
	}

	ret = SetATCommandQENG();
	if (-1 == ret)
	{
		goto end; 
	}

	if ((0 == s_4GHandle.uCregStatus[s_4GHandle.nCurSimCardIndex]) || (0 == s_4GHandle.nDialState))
	{
		ret = 0;
		s_4GHandle.nPingFailedCount = max_ping_failed_count;
		
		MODULE4G_ERR("[checkpingtime] s_4GHandle.uCregStatus:%d  s_4GHandle.nDialState:%d!\n",s_4GHandle.uCregStatus[s_4GHandle.nCurSimCardIndex],s_4GHandle.nDialState);
#ifdef __SUPPORT_SAVE_TF_LOG__ 
		TF_GPRS_LOG_INFO("s_4GHandle.uCregStatus:%d  s_4GHandle.nDialState:%d!\n",s_4GHandle.uCregStatus[s_4GHandle.nCurSimCardIndex],s_4GHandle.nDialState);
#endif			
	}
	else if (0 == IsStringInCommand("route -n", MEIG_SLM322M_ETH))
	{
		//不存在usb0 的路由
		MODULE4G_INF("4G No Route! s_4GHandle.nPingFailedCount=%d, max_ping_failed_count=%d\n", s_4GHandle.nPingFailedCount, max_ping_failed_count);

#ifdef __SUPPORT_SAVE_TF_LOG__ 
		TF_GPRS_LOG_INFO("4G No Route!\n");
#endif		
		//当做ping 失败处理
		ret = 0;
		IsRouteErr = 1;

		s_4GHandle.uInternetConnStatus = 0;
        s_4GHandle.uRealNetConnStatus = 0;
	}
	else
	{
		//发送ping命令
		ret = PingCheck();
		IsRouteErr = 0;
	}

	if (1 == ret)
	{		
		if(s_4GHandle.s_SwitchCardsState == 1)
		{
			MODULE4G_INF("BindCheckPingHandler end!!!\r\n");
			//切卡成功，结束切卡
			ParaAccessLock();
			s_4GHandle.s_SwitchCardsState = 0;
			s_4GHandle.s_SwitchCardsResult = 0;
			ParaAccessUnLock();
			return 0;
		}
		MODULE4G_INF("4G Ping ok!\n");

		s_4GHandle.nPingFailedCount = 0;

		//网络连接成功
		ParaAccessLock();

#ifdef __SUPPORT_SAVE_TF_LOG__ 
		if (0 == s_4GHandle.uInternetConnStatus)
		{
			TF_GPRS_LOG_INFO("4G Link Success!\n");
		}
#endif
		
		s_4GHandle.uInternetConnStatus = 1;
        s_4GHandle.uRealNetConnStatus = 1;

		s_4GHandle.nAirPlaneTestCount = 0;
		ParaAccessUnLock();
		first_fast_check = 0;
		memset(s_4GNetErrTimes, 0, sizeof(s_4GNetErrTimes));
		return 1;
	}
	else
	{
		MODULE4G_INF("4G Ping no Response!, s_4GHandle.nPingFailedCount=%d, max_ping_failed_count=%d\n", s_4GHandle.nPingFailedCount, max_ping_failed_count);
	
		//无响应或者超时
		s_4GHandle.nPingFailedCount++;

		if (s_4GHandle.nPingFailedCount >= max_ping_failed_count)	
		{			
			if(s_4GHandle.s_SwitchCardsState == 1)
			{
				MODULE4G_INF("4G Ping ERR!!!\r\n");
				//切卡后网络不通，切回去，结束切卡
				ChangeOtherSim();
				ParaAccessLock();
				s_4GHandle.s_SwitchCardsResult = 6;//网络不通，返回切卡失败
				s_4GHandle.s_SwitchCardsState = 0;
				ParaAccessUnLock();
				SwitchSubState(CHECK_SIM_STATE);
				return 0;
			}
			ParaAccessLock();			
#ifdef __SUPPORT_SAVE_TF_LOG__ 
			if (1 == s_4GHandle.uInternetConnStatus)
			{
				TF_GPRS_LOG_INFO("4G Link Failed!\n");
			}
#endif
			s_4GHandle.uInternetConnStatus = 0;
            s_4GHandle.uRealNetConnStatus = 0;

			ParaAccessUnLock();
			//网络连接失败，播报语音
			SetSpeechContent(SPEECH_4G_CONNECT_FAILED);
		
			s_4GHandle.nPingFailedCount = 0;
			//尝试进入飞行模式次数
			s_4GHandle.nAirPlaneTestCount++;
			if (s_4GHandle.nAirPlaneTestCount >= MAX_AIRPLANE_COUNT)
			{
				MODULE4G_INF("4G Enter Air Mode 3,set CheckErrorFlag!\n");

#ifdef __SUPPORT_SAVE_TF_LOG__ 
				TF_GPRS_LOG_INFO("4G Enter Air Mode 3,set CheckErrorFlag!\n");
#endif				
				
				s_4GHandle.nAirPlaneTestCount = 0;
				s_4GHandle.uCheckErrorFlag = 1;
				
				if (1 == IsRouteErr)
				{
					s_4GNetErrTimes[DhcpErr]++;
					MODULE4G_ERR("s_4GNetErrTimes[DhcpErr] :%d \n", s_4GNetErrTimes[DhcpErr]);
#ifdef __SUPPORT_SAVE_TF_LOG__ 
					TF_GPRS_LOG_INFO("4G Route Err !\n");		
#endif
				}
				else
				{
					s_4GNetErrTimes[DhcpErr] = 0;
				}
				first_fast_check = 0;

				//网络连接失败
				ParaAccessLock();
				
#ifdef __SUPPORT_SAVE_TF_LOG__ 
				if (1 == s_4GHandle.uInternetConnStatus)
				{
					TF_GPRS_LOG_INFO("4G Link Failed!\n");
				}
#endif
				
				s_4GHandle.uInternetConnStatus = 0;
				ParaAccessUnLock();
				return -1;
			}
			else
			{
				MODULE4G_INF("4G Enter Air Mode!\n");
#ifdef __SUPPORT_SAVE_TF_LOG__ 				
				TF_GPRS_LOG_INFO("4G Enter Air Mode!\n");
#endif				
				SwitchSubState(air_state);
				return -1;
			}
		}

		return -1;
	}	

end:
	if(s_4GHandle.s_SwitchCardsState == 1 && -1 == ret)
	{
		MODULE4G_INF("4G Ping ERR!!!\r\n");
		//切卡后网络不通，切回去，结束切卡
		ChangeOtherSim();
		ParaAccessLock();
		s_4GHandle.s_SwitchCardsState = 0;
		s_4GHandle.s_SwitchCardsResult = 6;//网络不通，返回切卡失败
		ParaAccessUnLock();
		SwitchSubState(CHECK_SIM_STATE);
		return 0;
	}

	if (-1 == ret)
	{
		s_4GHandle.uComErrorState = 0;
		SwitchSubState(err_state);
	}

	return ret;	
}

/*@breif CommuniteErrorHandler 通讯错误处理
 *@param 
 *@return 
 *@       
 */
static int CommuniteErrorHandler(int next_state)
{
	int ret = 0;
	int fd = -1;

	if (COMERROR_RESET_STATE == s_4GHandle.uComErrorState)
	{
	    if(3 == s_4GHandle.ucOpenusStatus && 1001 == s_4GHandle.ucOpenusErrno)
        {
            MODULE4G_INF("Insufficient Balance,Reset Module!\n");
#ifdef __SUPPORT_SAVE_TF_LOG__ 		
            TF_GPRS_LOG_INFO("Insufficient Balance,Reset Module!\n");
#endif  
        }
        else
        {
            MODULE4G_INF("4G Read Write Error,Reset Module!\n");
#ifdef __SUPPORT_SAVE_TF_LOG__ 		
            TF_GPRS_LOG_INFO("4G Read Write Error,Reset Module!\n");
#endif  
        }
		
		//关闭拨号
		Module4G_DailDown();

		//复位模组
		CloseSerialPort(s_4GHandle.iSerial_fd);
		s_4GHandle.iSerial_fd = -1;	
		Reset4GModule();
		sleep(1);
		//复位模组后重新判断一次模组节点以及ID
		ret = Module4GIsOnline();
		if(ret < 0)
		{
			MODULE4G_ERR("Module4GIsOnline return -1\n");
		}

		//这里须将当前使用SIM卡编号置位
        ParaAccessLock();
    	s_4GHandle.nCurSimCardIndex = 0;
    	ParaAccessUnLock();
		
		s_4GHandle.uComErrorState = COMERROR_WAIT_STATE;
		s_4GHandle.nComErrorResetSleepCount = 0;
	}
	else if (COMERROR_WAIT_STATE == s_4GHandle.uComErrorState)
	{
		s_4GHandle.nComErrorResetSleepCount++;
		if (s_4GHandle.nComErrorResetSleepCount > MAX_MODULEPOWER_RESET_TIME)
		{
			s_4GHandle.uComErrorState = COMERROR_TEST_STATE;
		}
	}
	else if (COMERROR_TEST_STATE == s_4GHandle.uComErrorState)
	{
		//测试读写
		ret = Module4GIsOnline();
		if(ret < 0)
		{
			MODULE4G_ERR("Module4GIsOnline return -1\n");
		}
		
		MODULE4G_INF("OpenSerialPort start szTtyDevice:%s\n",szTtyDevice);

		fd = OpenSerialPort(szTtyDevice);
		
		if (fd < 0)
		{
			s_4GHandle.uComErrorState = COMERROR_REBOOT_STATE;
			s_4GHandle.nComErrorResetSleepCount = 0;
			goto end;
		}

		/* 设置串口节点参数(波特率，数据位，奇偶校验位等) */
	    if (SetSerialAttr(fd, 115200, 8, 'N', 1))
	    {
	        MODULE4G_ERR("Fail to called SetSerialAttr\n");
#ifdef __SUPPORT_SAVE_TF_LOG__ 	        
	        TF_GPRS_LOG_INFO("Fail to called SetSerialAttr\n");
#endif	        
	        s_4GHandle.uComErrorState = COMERROR_REBOOT_STATE;
			s_4GHandle.nComErrorResetSleepCount = 0;
			CloseSerialPort(fd);
	        goto end;
	    }

		s_4GHandle.iSerial_fd = fd;

		//at测试通讯
		ret = SetATCommandATTEST();
		if (-1 == ret)
		{
			s_4GHandle.uComErrorState = COMERROR_REBOOT_STATE;
			s_4GHandle.nComErrorResetSleepCount = 0;
			CloseSerialPort(fd);
			s_4GHandle.iSerial_fd = -1;
			goto end; 
		}

		CloseSerialPort(fd);
		s_4GHandle.iSerial_fd = -1;

		SwitchSubState(next_state);
	}
	else if (COMERROR_REBOOT_STATE == s_4GHandle.uComErrorState)
	{
		if (s_4GHandle.uComErrorRebootFlag)
		{
			s_4GHandle.nComErrorResetSleepCount++;
			if (s_4GHandle.nComErrorResetSleepCount > ERROR_COMREBOOT_SLEEP_TIME)
			{
				s_4GHandle.uComErrorRebootFlag = 0;
			}
		}
		else
		{
			MODULE4G_INF("4G Read Write Error,Reset Device!\n");
#ifdef __SUPPORT_SAVE_TF_LOG__ 			
			TF_GPRS_LOG_INFO("4G Read Write Error,Reset Device!\n");
#endif			

			RebootLinux(REBOOT_GPRSDEVICE_COMERROR);
		}
	}
	
end:
	return 1;
}


/*@breif SwitchCardsHandler 切卡状态处理
 *@param 
 *@return 
 *@ 	  
 */
static int SwitchCardsHandler(int next_state)
{
	G4_Control_Manager_t *fd = NULL;
	Module4G_GetSystemHandleInfo(&fd);

	//切卡
	ChangeOtherSim();
	sleep(10);
	
	//获取当前使用的卡
	SetATCommandGetCurrentSwitchSim();

	SwitchSubState(next_state);
	return 0;
}

static int CloudCardHandler(int next_state, int err_state)
{
	G4_Control_Manager_t *fd = NULL;
	Module4G_GetSystemHandleInfo(&fd);

    int iRet = -1;
    static int iDetectTimes = 0;
    static int iSpeechFlag = 0;
    static int i24HTimes = 0;
    struct timeval tv = {0};
    unsigned int iCurTimes = 0;

	if(fd->device_did_info == 1 && fd->sim_01_card_status != 2)    //云卡设备：新版本模组，非YL号段。走云卡
	{
		SwitchSubState(next_state);
		MODULE4G_INF("fd->device_did_info == %d\n",fd->device_did_info);
		return 0;
	}

    //普通版本直接返回
    if(s_4GHandle.sHardVersion[5] == 'L')
    {
        SwitchSubState(next_state);
        MODULE4G_INF("Not ucwk mode !!!\n");
        return 0;
    }
    //实体卡模式。直接返回
    if(0 == s_4GHandle.ucWorkMode && s_4GHandle.IsSupportOnlyCloudCard != 1)
    {
        SwitchSubState(next_state);
        MODULE4G_INF("Not ucwk mode !!!\n");
        return 0;
    }
	else if(s_4GHandle.IsSupportOnlyCloudCard == 1)
	{
		SETATCommandUCWKMODEther();
		iRet = GetATCommandUCWKMODEther();
        if(-1 == iRet)
        {
            MODULE4G_ERR("get ucwk mode error\n");
            //暂时这么处理
            s_4GHandle.uComErrorState = COMERROR_RESET_STATE;
            s_4GHandle.ucWorkMode = 99;
            SwitchSubState(err_state);
            return 0;
        }
	}

    //未知模式，重新获取云卡模式
    if(99 == s_4GHandle.ucWorkMode)
    {
        iRet = GetATCommandUCWKMODEther();
        if(-1 == iRet)
        {
            MODULE4G_ERR("get ucwk mode error\n");
            //暂时这么处理
            s_4GHandle.uComErrorState = COMERROR_RESET_STATE;
            s_4GHandle.ucWorkMode = 99;
            SwitchSubState(err_state);
            return 0;
        }

        if(99 == s_4GHandle.ucWorkMode)
        {
            s_4GHandle.uComErrorState = COMERROR_RESET_STATE;
            SwitchSubState(err_state);
            return 0;
        }
        
        if(0 == s_4GHandle.ucWorkMode)
        {
            GetATCommandUCWKSTATUSther();
            if(-1 == iRet)
            {
                MODULE4G_ERR("GetATCommandUCWKSTATUSther error\n");
                s_4GHandle.uComErrorState = COMERROR_RESET_STATE;
                SwitchSubState(err_state);
                return 0;
            }
            if(0 == s_4GHandle.ucOpenusStatus && 0 == s_4GHandle.ucOpenusProcess && 0 == s_4GHandle.ucOpenusErrno)
            {
                s_4GHandle.ucWorkMode = 0;
                s_4GHandle.uComErrorState = COMERROR_RESET_STATE;
                SwitchSubState(next_state);
                return 0;
            }
            else
            {
                s_4GHandle.ucWorkMode = 1;
            }
        }
    }

    //卡存在，报sim卡检测成功
	s_4GHandle.uNeedSpeechSimSuccess = 1;

    //接着获取云卡启用状态
    iRet = GetATCommandUCWKSTATUSther();
    if(-1 == iRet)
    {
        MODULE4G_ERR("GetATCommandUCWKSTATUSther error\n");
        s_4GHandle.uComErrorState = COMERROR_RESET_STATE;
        SwitchSubState(err_state);
        return 0;
    }

    if(0 == s_4GHandle.ucOpenusStatus)
    {
        //表示未处于云卡模式
        MODULE4G_ERR("4G mode ucworkstatus == 0 not unite card mode!!!\n");
        s_4GHandle.uComErrorState = COMERROR_RESET_STATE;
        SwitchSubState(err_state);
        return 0;
    }

    if(1 == s_4GHandle.ucOpenusStatus)
    {
        //表示云卡正在启动中
        MODULE4G_INF("4G mode ucworkstatus == 1 replacement card starting!!!\n");
        SwitchSubState(CLOUD_CARD_STATE);
        return 1;
    }

    if(2 == s_4GHandle.ucOpenusStatus && 100 == s_4GHandle.ucOpenusProcess && 0 == s_4GHandle.ucOpenusErrno)
    {
        //表示云卡已启动完成
        MODULE4G_INF("4G mode ucworkstatus == 2,100,0 unite card start OK!!!\n");
        s_4GHandle.ucDetectTimes = 0;
        i24HTimes = 0;
        iSpeechFlag = 1;
        SwitchSubState(next_state);
        return 0;
    }

    //异常处理
    if(3 == s_4GHandle.ucOpenusStatus)
    {
        iDetectTimes++;
        gettimeofday(&tv, NULL);
        iCurTimes = tv.tv_sec;
        if(0 == s_4GHandle.ucDetectTimes)
        {
            s_4GHandle.ucDetectTimes = iCurTimes;
        }
        if(i24HTimes == 0)
        {
            i24HTimes = iCurTimes;
        }
        if(1001 == s_4GHandle.ucOpenusErrno)
        {
            //余额不足，1H复位
            //直接播报余额不足
            if(1 == s_4GHandle.ucDetectTimes)
            {
                s_4GHandle.ucDetectTimes = iCurTimes;
                MODULE4G_ERR("4G mode ucucworkstatus == 3,0,1001 Detection_times >= 3600 4Gmoderest!!!\n");
                s_4GHandle.uComErrorState = COMERROR_RESET_STATE;
                SwitchSubState(err_state);
                return 0;
            }
            else
            {
                if(iCurTimes >= s_4GHandle.ucDetectTimes)
                {
                    if(iCurTimes - s_4GHandle.ucDetectTimes >= 3600)
                    {
                        s_4GHandle.ucDetectTimes = iCurTimes;
                        MODULE4G_ERR("4G mode ucucworkstatus == 3,0,1001 Detection_times >= 3600 4Gmoderest!!!\n");
                        s_4GHandle.uComErrorState = COMERROR_RESET_STATE;
                        SwitchSubState(err_state);
                        return 0;
                    }
                }
                else
                {
                    s_4GHandle.ucDetectTimes = iCurTimes;
                }
            }
        }
        else
        {
            //24H复位
            if(iCurTimes >= i24HTimes)
            {
                if(iCurTimes - i24HTimes >= 86400)
                {
                    i24HTimes = iCurTimes;
                    MODULE4G_ERR("4G mode ucucworkstatus == 3 Detection_times >= 86400 4Gmoderest!!!\n");
                    s_4GHandle.uComErrorState = COMERROR_RESET_STATE;
                    SwitchSubState(err_state);
                    return 0;
                }
            }
            else
            {
                i24HTimes = iCurTimes;
            }
        }

        if(10 == iDetectTimes)
        {
            if(0 == iSpeechFlag)
            {
                //网络连接失败，播报语音,这里语音只在启动时播报
                if(1001 == s_4GHandle.ucOpenusErrno)
                {
                    SetSpeechContent(SPEECH_4G_INSUFFICIENT_BALANCE);
                }
                else
                {
    			    SetSpeechContent(SPEECH_4G_CONNECT_FAILED);
                }

                iSpeechFlag = 1;
            }
            MODULE4G_INF("Connect Fail,ucOpenusStatus %d %d %d\n", s_4GHandle.ucOpenusStatus, s_4GHandle.ucOpenusProcess, s_4GHandle.ucOpenusErrno);
        }
        return 1;
    }

    return 0;
}

/*@breif BindStateHandler 绑定模式处理
 *@param 
 *@return 
 *@       
 */
static void BindStateHandler()
{
	int ret = 0;

	do {
		switch (s_4GHandle.uSubState)
		{
			case CHECK_MODULE_STATE:
				ret = CheckModlueHandler(MODULE_INIT_STATE);
				break;
			case MODULE_INIT_STATE:
				ret = ModlueInitHandler(CLOUD_CARD_STATE,COMMUNITE_ERROR_STATE);
				break;
			case CHECK_SIM_STATE:
				ret = BindCheckSimHandler(GET_INFO_STATE,COMMUNITE_ERROR_STATE);
				break;	
			case GET_INFO_STATE:
				ret = GetInfoHandler(CHECK_CREG_STATE,COMMUNITE_ERROR_STATE);
				break;
			case ENTER_AIR_STATE:
				ret = EnterAirHandler(QUIT_AIR_STATE,COMMUNITE_ERROR_STATE);
				break;
			case QUIT_AIR_STATE:
				ret = QuitAirHandler(MODULE_INIT_STATE,COMMUNITE_ERROR_STATE);
				break;	
			case DIAL_STATE:
				ret = ECMDialHandler(CHECK_DIAL_STATE,COMMUNITE_ERROR_STATE);
				break;
			case CHECK_DIAL_STATE:
				ret = BindCheckDialStateHandler(CHECK_CSQ_STATE,COMMUNITE_ERROR_STATE);
				break;
			case CHECK_CREG_STATE:
				ret = BindCheckCregHandler(DIAL_STATE,ENTER_AIR_STATE,COMMUNITE_ERROR_STATE);
				break;
			case CHECK_CSQ_STATE:
				ret = CheckCsqHandler(CHECK_PING_STATE,COMMUNITE_ERROR_STATE);
				break;
			case CHECK_PING_STATE:
				ret = BindCheckPingHandler(CHECK_PING_STATE,ENTER_AIR_STATE,COMMUNITE_ERROR_STATE);
				break;
			case CLOUD_CARD_STATE:
				ret = CloudCardHandler(CHECK_SIM_STATE, COMMUNITE_ERROR_STATE);
				break;
			case COMMUNITE_ERROR_STATE:
				ret = CommuniteErrorHandler(CHECK_MODULE_STATE);
				break;
			case SIMSWITCH_CARDS_STATE:
				ret = SwitchCardsHandler(CHECK_SIM_STATE);
				break;
			default:
				break;
		}
	}while(0 == ret);
}


/*@breif ProduceTestInit 产测流程初始化
 *@param 
 *@return 
 *@       
 */
static int ProduceTestInit()
{
	int ret = 0;

	Module4G_DailDown();

	if (s_4GHandle.iSerial_fd < 0)
	{
		ret = Open4GModule();
		if (-1 == ret)
		{
			goto end; 
		}
	}

	//at测试通讯
	ret = SetATCommandATTEST();
	if (-1 == ret)
	{
		goto end; 
	}

	//禁止回显
	ret = SetATCommandATE0();
	if (-1 == ret)
	{
		goto end; 
	}

	//设置协议 rndis
	ret = SetATCommandQCFG(3);
	if (-1 == ret)
	{
		goto end; 
	}

end:
	return ret;
}

/*@ preif 从SIM卡保存文件中读出SIM信息，并配置全局变量
 *@ 内容：Version;Imei;SimCardCnt;
 *@ 内容：SIM1\SIM2: Imsi;Iccid;ACT;Oper;Band;Csq;
 *@ return -1 出错
 *@        0  成功
 */
static int ReadSIMInfoFile(G4_Control_Manager_t * const fd)
{
	int ret = 0;
	FILE *pFd = NULL;
	char *find_str = NULL;
	char *find_semicolon = NULL;
    int FileSimCardCnt = 0;

	pFd = fopen(SIM_INFO_PATH, "r");
	if (NULL == pFd)
	{
		MODULE4G_ERR("Open %s fail!\n", SIM_INFO_PATH);
		return -1;
	}

	char pBuff[1024] = {0};
	char tmp_string[16] = {0};
	char sNum[8] = {0};

	do
	{
		if (fgets(pBuff, sizeof(pBuff), pFd) == NULL)
		{
			MODULE4G_ERR("fgets %s fail!\n", SIM_INFO_PATH);
			ret = -1;
			goto end;
		}

		find_str = strstr(pBuff,"Version:");		
		if (NULL == find_str)
		{
			MODULE4G_ERR("strstr Version fail!\n");
			ret = -1;
			goto end;
		}
        else
        {           
			ParaAccessLock();			
			find_semicolon = strstr(find_str, ";");
			if (find_semicolon) {
				memcpy(fd->sVersion, find_str+strlen("Version:"), find_semicolon-find_str-strlen("Version:"));
				MODULE4G_INF("Read Version: %s\n", fd->sVersion);
			}
            else {
				memset(fd->sVersion, 0, sizeof(fd->sVersion));
                MODULE4G_INF("Version is Empty\n");
			}
			ParaAccessUnLock();
		}

		find_str = strstr(pBuff, "Imei:");
		if (NULL == find_str)
		{
			MODULE4G_ERR("strstr Imei fail!\n");
			ret = -1;
			goto end;
		}
        else
        {           
			ParaAccessLock();			
			find_semicolon = strstr(find_str, ";");
			if (find_semicolon) {
				memcpy(fd->sImei, find_str+strlen("Imei:"), find_semicolon-find_str-strlen("Imei:"));
				MODULE4G_INF("Read Imei: %s\n", fd->sImei);
			}
            else {
				memset(fd->sImei, 0, sizeof(fd->sImei));
                MODULE4G_INF("Imei is Empty\n");
			}
			ParaAccessUnLock();
		}

		find_str = strstr(pBuff, "SimCardCnt:");
		if (NULL == find_str)
		{
			MODULE4G_ERR("strstr SimCardCnt fail!\n");
			ret = -1;
			goto end;
		}
        else
        {           
			ParaAccessLock();			
			find_semicolon = strstr(find_str, ";");
			if (find_semicolon) {
                memset(sNum, 0, sizeof(sNum));
				memcpy(sNum, find_str+strlen("SimCardCnt:"), find_semicolon-find_str-strlen("SimCardCnt:"));
                fd->s_4GSimCardCount = atoi(sNum);
				MODULE4G_INF("Read SimCardCnt: %d\n", fd->s_4GSimCardCount);
			}
            else {
				fd->s_4GSimCardCount = 0;
                MODULE4G_INF("SimCardCnt is Empty\n");
			}
			ParaAccessUnLock();
		}

        ParaAccessLock();
		for (int i = 0; i < MAX_SIMCARD_CNT; i++)
		{
            memset(tmp_string, 0, sizeof(tmp_string));
			snprintf(tmp_string, sizeof(tmp_string)-1, "Imsi%d:", i);
			find_str = strstr(pBuff, tmp_string);
			if (NULL == find_str)
			{
                MODULE4G_ERR("strstr Imsi [%d] fail!\n", i);
			}
            else
            {
				find_semicolon = strstr(find_str, ";");
				if (find_semicolon) {
					memcpy(fd->sImsi[i], find_str+strlen("Imsi0:"), find_semicolon-find_str-strlen("Imsi0:"));
					MODULE4G_INF("Read sImsi[%d]: %s\n", i, fd->sImsi[i]);
				} 
                else {
					memset(fd->sImsi[i], 0, sizeof(fd->sImsi[i]));
                    MODULE4G_INF("Imsi[%d] is Empty\n", i);
				}
			}

            //如果SIM i的imsi不为空，表示当前有SIM卡，继续读取信息
			if (strlen(fd->sImsi[i]) != 0)
            {
                FileSimCardCnt++;
                fd->uSimCardStatus[i]= NORMAL;

                memset(tmp_string, 0, sizeof(tmp_string));
				snprintf(tmp_string, sizeof(tmp_string)-1, "Iccid%d:", i);
				find_str = strstr(pBuff, tmp_string);
				if (NULL == find_str)
				{
                    MODULE4G_ERR("strstr Imsi [%d] fail!\n", i);
					ret = -1;
				}
                else
                {
					find_semicolon = strstr(find_str, ";");
					if (find_semicolon)
                    {
						memcpy(fd->sIccid[i], find_str+strlen("Iccid0:"), find_semicolon-find_str-strlen("Iccid0:"));
						MODULE4G_INF("Read Iccid[%d]: %s\n", i, fd->sIccid[i]);
					} else {
						memset(fd->sIccid[i], 0, sizeof(fd->sIccid[i]));
                        MODULE4G_INF("sIccid[%d] is Empty\n", i);
					}
				}

                memset(tmp_string, 0, sizeof(tmp_string));
				snprintf(tmp_string, sizeof(tmp_string)-1, "ACT%d:", i);
				find_str = strstr(pBuff, tmp_string);
				if (NULL == find_str)
				{
                    MODULE4G_ERR("strstr ACT [%d] fail!\n", i);
					ret = -1;
				}
                else
                {
					find_semicolon = strstr(find_str, ";");
					if (find_semicolon) {
						memcpy(fd->sACT[i], find_str+strlen("ACT0:"), find_semicolon-find_str-strlen("ACT0:"));
						MODULE4G_INF("Read sACT[%d]: %s\n", i, fd->sACT[i]);
					} else {
						memset(fd->sACT[i], 0, sizeof(fd->sACT[i]));
                        MODULE4G_INF("sACT[%d] is Empty\n", i);
					}
				}

                memset(tmp_string, 0, sizeof(tmp_string));
				snprintf(tmp_string, sizeof(tmp_string)-1, "Oper%d:", i);
				find_str = strstr(pBuff, tmp_string);
				if (NULL == find_str)
				{
                    MODULE4G_ERR("strstr Oper [%d] fail!\n", i);
					ret = -1;
				}
                else
                {           
					find_semicolon = strstr(find_str, ";");
					if (find_semicolon) {
						memcpy(fd->sOper[i], find_str+strlen("Oper0:"), find_semicolon-find_str-strlen("Oper0:"));
						MODULE4G_INF("Read sOper[%d]: %s\n", i, fd->sOper[i]);
					} else {
						memset(fd->sOper[i], 0, sizeof(fd->sOper[i]));
                        MODULE4G_INF("sOper[%d] is Empty\n", i);
					}
				}

                memset(tmp_string, 0, sizeof(tmp_string));
				snprintf(tmp_string, sizeof(tmp_string)-1, "Band%d:", i);
				find_str = strstr(pBuff, tmp_string);
				if (NULL == find_str)
				{
                    MODULE4G_ERR("strstr Band [%d] fail!\n", i);
					ret = -1;
				}
                else
                {           
					find_semicolon = strstr(find_str, ";");
					if (find_semicolon) {
						memcpy(fd->sBand[i], find_str+strlen("Band0:"), find_semicolon-find_str-strlen("Band0:"));
						MODULE4G_INF("Read sBand[%d]: %s\n", i, fd->sBand[i]);
					} else {
						memset(fd->sBand[i], 0, sizeof(fd->sBand[i]));
                        MODULE4G_INF("sBand[%d] is Empty\n", i);
					}
				}

                memset(tmp_string, 0, sizeof(tmp_string));
				snprintf(tmp_string, sizeof(tmp_string)-1, "Csq%d:", i);
				find_str = strstr(pBuff, tmp_string);
				if (NULL == find_str)
				{
                    MODULE4G_ERR("strstr Csq [%d] fail!\n", i);
					ret = -1;
				}
                else
                {           
					find_semicolon = strstr(find_str, ";");
					if (find_semicolon) {
                        memset(sNum, 0, sizeof(sNum));
						memcpy(sNum, find_str+strlen("Csq0:"), find_semicolon-find_str-strlen("Csq0:"));
						fd->iCsq[i] = atoi(sNum);
						MODULE4G_INF("Read iCsq[%d]: %d\n", i, fd->iCsq[i]);
					} else {
						fd->iCsq[i] = 0;
                        MODULE4G_INF("iCsq[%d] is Empty\n", i);
					}
				}
			}
		}
        ParaAccessUnLock();
	}while(0);

    /* SIMInfo文件中SIM卡个数为0，或者和之前记录的个数不一致，返回错误 */
    if (FileSimCardCnt == 0 || FileSimCardCnt != fd->s_4GSimCardCount)
    {    
        MODULE4G_INF("FileSimCardCnt[%d] s_4GSimCardCount[%d]\n", FileSimCardCnt, fd->s_4GSimCardCount);
        ret = -1;
    }

    /* 如果SIM卡1(外置卡)不正常，直接返回错误 */
    if (fd->uSimCardStatus[0] != NORMAL)
    {    
        MODULE4G_INF("Get SIM0 Info Failed! FileSimCardCnt[%d] s_4GSimCardCount[%d]\n", FileSimCardCnt, fd->s_4GSimCardCount);
        ret = -1;
    }
	
	/* 双卡设备，如果双SIM卡信息相同，直接返回错误 */
	if ((FileSimCardCnt == 2) && (0 == strncmp(fd->sIccid[0], fd->sIccid[1], strlen(fd->sIccid[0]))))
	{
		MODULE4G_INF("DualCard, But the information is consistent. Iccid[%s]\n", fd->sIccid[0]);
		ret = -1;
	}

	/*如果出现SIM卡信号强度为0，则判断为信号获取时未插天线，需要重新读取*/
	if(fd->s_4GSimCardCount == 2 && (fd->iCsq[0] == 0 ||fd->iCsq[1] == 0))
	{
		MODULE4G_INF("Get SIM CSQ ERR ,Has 0 to CSQ !!!CSQ[%d][%d]\n", fd->iCsq[0],fd->iCsq[1]);
		ret = -1;
	}
end:
    SAFE_FCLOSE(pFd);
	return ret;
}



/*@ preif 把SIM卡相关信息写入配置文件中
 *@ 内容：Version;Imei;SimCardCnt;
 *@ 内容：SIM1\SIM2: Imsi;Iccid;ACT;Oper;Band;Csq;
 *@ return -1 出错
 *@        0  成功
 */
static int WriteSIMInfoFile(const G4_Control_Manager_t * const fd)
{
	FILE *pFd = NULL;
	pFd = fopen(SIM_INFO_PATH, "w");
	if (NULL == pFd)
	{
		MODULE4G_ERR("Open %s fail!\n", SIM_INFO_PATH);
		return -1;
	}

	char tmp[512] = {0};
	int ret = 0, len = 0, len1 = 0, len2 = 0;

	len = snprintf(tmp,sizeof(tmp)-1,"Version:%s;Imei:%s;SimCardCnt:%d;", fd->sVersion, fd->sImei, fd->s_4GSimCardCount);
	MODULE4G_INF("tmp:[%s] --- len:[%d]\n",tmp, len);

	if ((strlen(fd->sImsi[0]) > 0) && (fd->uSimCardStatus[0] == NORMAL))
	{
		len1 = snprintf(tmp+len, sizeof(tmp)-1-len, "Imsi0:%s;Iccid0:%s;ACT0:%s;Oper0:%s;Band0:%s;Csq0:%d;",
										fd->sImsi[0],fd->sIccid[0],fd->sACT[0],
										fd->sOper[0],fd->sBand[0],fd->iCsq[0]);

    	MODULE4G_INF("tmp:[%s] --- len1:[%d]\n",tmp, len1);
	}
	if ((strlen(fd->sImsi[1]) > 0) && (fd->uSimCardStatus[1] == NORMAL))
	{
		len2 = snprintf(tmp+len+len1, sizeof(tmp)-1-len-len1, "Imsi1:%s;Iccid1:%s;ACT1:%s;Oper1:%s;Band1:%s;Csq1:%d;",
										fd->sImsi[1],fd->sIccid[1],fd->sACT[1],
										fd->sOper[1],fd->sBand[1],fd->iCsq[1]);

        MODULE4G_INF("tmp:[%s] --- len2:[%d]\n",tmp, len2);
	}

	len = fwrite(tmp, 1, strlen(tmp), pFd);
	if (len != strlen(tmp))
    {
        ret = -1;
		MODULE4G_ERR("SIM_INFO_PATH write fail!\n");
	}
    else
    {
        fflush(pFd);
	}

    SAFE_FCLOSE(pFd);
	return ret;
}




/*@breif ProduceTestNet 网络检测
 *@param 
 *@return 
 *@       
 */
static int ProduceTestNet()
{
	int ret = 0;
	int i = 0;
	
	//获取IMSI
	for (i = 0; i < 3; i++)
	{
		ret = SetATCommandCIMI();
		
		if (strlen(s_4GHandle.sImsi[s_4GHandle.nCurSimCardIndex] )!= 0){	
			break;
		}
		sleep(2);
	}

	//查询band
	for (i = 0; i < 3; i++)
	{
		ret = SetATCommandQNWINFO();
		
		if (!ret){	
			break;
		}
		sleep(2);
	}

	for (i = 0; i < 3; i++)
	{
		ret = SetATCommandSetCEREG(0);
		
		if (!ret){	
			break;
		}
		sleep(2);
	}

	for (i = 0; i < 3; i++)
	{
		ret = SetATCommandGetCEREG();
		
		if (!ret){	
			break;
		}
		sleep(2);
	}	

	//内置卡查询信号质量稍慢，可能需等待一会，基本上等两秒左右就能查到
	for (i = 0; i < 3; i++)
	{
		ret = SetATCommandCSQ();
		
		if (!s_4GHandle.iCsq[s_4GHandle.nCurSimCardIndex]||!ret)
		{	
			MODULE4G_INF("ProduceTestNet, s_4GHandle.iCsq[%d]:%d\n", s_4GHandle.nCurSimCardIndex,s_4GHandle.iCsq[s_4GHandle.nCurSimCardIndex]);
			break;
		}
		sleep(2);
	}	

	//问题：在产测阶段，外置卡运营商偶现有几率查不到，内置卡运营商产测基本查不到(要入网才能才能查到)
	//修改方法如下：
	//此时：1.外置卡运营商字段为空，此时尝试再次查询act oper，最多查询三次。
	//      2.内置卡运营商字段为空，首先从记录的内置卡保存信息文件中查询，若查到则写入文件，后续便可直接读，若没查到，则作罢
	//另外：如果当前卡的运营商仍没查到，在checkping里面增加运营商查询，同时刷新到产测数据里

	for (i = 0; i < 3; i++)
	{
		if(ZHONGYUN_ZX800HG == s_ModuleType)
		{
			//查询act oper前先设置
			ret = SetATCommandCOPSNUM();
		}
		ret = SetATCommandCOPS();
		
		if (0 != strlen(s_4GHandle.sOper[s_4GHandle.nCurSimCardIndex]))
		{
			MODULE4G_INF("ProduceTestNet s_4GHandle.sOper[%d]:%s\n", s_4GHandle.nCurSimCardIndex,s_4GHandle.sOper[s_4GHandle.nCurSimCardIndex]);
			break;
		}

		sleep(2);		
	}

	//查询band
	ret = SetATCommandQNWINFO();
	if (-1 == ret)
	{
		goto end; 
	}


	if (1 == s_4GHandle.s_IsSgOrVg4gModule)
	{
		ret = SetATCommandSetCEREG(2);
	}
	else
	{
		ret = SetATCommandSetCEREG(0);
	}
	
	return 0;

end:
	return ret;
}

/*@breif ProduceTestCheckHandler 执行云卡产测流程
 *@param 
 *@return 
 *@       
 */
static void ProduceTestCloudCardHandler(void)
{
    int iRet = -1;

    //首先就要获取云卡启用状态，只有启用成功，才能获取ICCID
    iRet = GetATCommandUCWKSTATUSther();
    if(-1 == iRet)
    {
        MODULE4G_ERR("GetATCommandUCWKSTATUSther error\n");
        goto Checkend;
    }

    if(1 == s_4GHandle.ucOpenusStatus)
    {
        //表示云卡正在启动中
        MODULE4G_INF("4G mode ucworkstatus == 1 replacement card starting!!!\n");
        SwitchSubState(PRODUCTTEST_CLOUD_CARD_STATE);
        return;
    }

    if(3 == s_4GHandle.ucOpenusStatus)
    {
        //表示云卡正在启动中
        MODULE4G_INF("4G mode ucworkstatus == 3 !!!\n");
        goto Checkend;
    }

    iRet = SetATCommandICCID();
	if (-1 == iRet)
	{
		s_4GHandle.uSimCardStatus[0] = ABNORMAL;
		goto Checkend;
	}

	if (NORMAL == s_4GHandle.uSimCardStatus[0])
	{
		//检测当前卡
		iRet = ProduceTestNet();
		if (-1 == iRet)
		{
			MODULE4G_ERR("ProduceTestNet error\n");
		}
	}
Checkend:
    SwitchSubState(PRODUCTTEST_CHECKFINISH_STATE);
}


/*@breif ProduceTestCheckHandler 执行产测流程
 *@param 
 *@return 
 *@       
 */
static void ProduceTestCheckHandler()
{
	G4_Control_Manager_t *fd = NULL;
	Module4G_GetSystemHandleInfo(&fd);

    int i = 0;
	int ret = 0;
	int NextSimCardIdx = 0;
    int nValidSimCnt = 0;
	unsigned int nAuthState = 0;
    unsigned char szAuthCode[256] = {0};

	fd->nProductTestFinish = 0;


	//如果没有授权则不进行相关信息检测。
    ZWSys_GetAuthInfo(szAuthCode, &nAuthState);
	MODULE4G_INF("szAuthCode:%s  nAuthState:%u\n",szAuthCode,nAuthState);
	if(!nAuthState)
	{
		MODULE4G_INF("ProduceTestCheckHandler not AuthInfo\n");
		goto end;
	}

	PowerUp4GModule();	
	//模组检测
	ret = Module4GIsOnline();
	if (-1 == ret)
	{
		goto end; 
	}

	ParaAccessLock();
	s_4GHandle.uModuleStatus = 1;
	ParaAccessUnLock();

	//模组初始化
	ret = ProduceTestInit();
	if (-1 == ret)
	{
		goto end; 
	}
	
	// 获取硬件版本号
	ret = GetModuleHardWareVersion();	
	if (-1 == ret)
	{
		goto end; 
	}

	//获取IMEI
	ret = SetATCommandCGSN();
	if (-1 == ret)
	{
		goto end; 
	}

	ret = SetATCommandGetCurrentSwitchSim();
	if (-1 == ret)
	{
		goto end; 
	}

	//获取模组内固件版本
	if (-1 == SetATCommandQGMR())
	{
		goto end;
	}
	
    //首先根据硬件版本，判断是普通版本还是云卡版本
    if(fd->sHardVersion[5] == 'C' &&
		(((fd->sim_01_card_status != 2) && (fd->device_did_info !=1))|| ((fd->sim_01_card_status == 2))))
    {
        //设置云卡设备能力集
        if(NULL != s_SetCloudCardCb)
        {
            s_SetCloudCardCb(1);
        }
        //如果是云卡版本，则设置为单卡
        Set4GRealSimCardCount(1);
        
        //说明是云卡版本，获取云卡的工作模式
        ret = GetATCommandUCWKMODEther();
        if(-1 == ret)
        {
            MODULE4G_ERR("get ucwk mode error\n");
            //暂时这么处理
            fd->ucWorkMode = 99;
            goto end;
        }

        //云卡模式为0，还需要根据状态来判断是否是实体卡模式
        if(0 == fd->ucWorkMode)
        {
            ret = GetATCommandUCWKSTATUSther();
            if (-1 == ret)
        	{
                MODULE4G_ERR("get ucwk status error\n");
        		goto end; 
        	}
            if(0 == fd->ucOpenusStatus && 0 == fd->ucOpenusProcess && 0 == fd->ucOpenusErrno)
            {
                fd->ucWorkMode = 0;
            }
            else
            {
                fd->ucWorkMode = 1;
            }
        }
    }
    else
    {
       fd->ucWorkMode = 0;//普通版本，则为实体卡模式
    }

    //如果是云卡模式，则需要进云卡检测流程
    if(1 == fd->ucWorkMode)
    {
       SwitchSubState(PRODUCTTEST_CLOUD_CARD_STATE);
       return;
    }

	//新增的(fd->sHardVersion[5] == 'C')判断，主要是因为之前产品需求是YL号段不走云卡逻辑。
	//HG云卡设备会判断为双卡走到下面读SIMINFO信息，造成设备的ccid一直采用SIMINFO里面的。
    /* 兼容以前单卡设备，不做切卡检测 */
    if (fd->s_4GSimCardCount <= 1 || (fd->sHardVersion[5] == 'C' && fd->device_did_info == 1))
    {
        /* 获取当前SIM卡ICCID信息，尝试3次 */
        for (int cnt = 2; cnt >= 0; cnt--)
        {
			if(fd->sHardVersion[5] == 'C' && fd->device_did_info == 1 && fd->sim_01_card_status == 0) //YL号段云卡，认为没有插入外置卡所以qccid返回异常
			{
				ret=-1;
			}
			else
			{
				ret = SetATCommandICCID();
			}
            if ((-1 == ret) || (strlen(fd->sIccid[fd->nCurSimCardIndex]) == 0))
            {
                if (cnt == 0)
                {
                    fd->uSimCardStatus[fd->nCurSimCardIndex] = ABNORMAL;
                    MODULE4G_ERR("Get SimCardStatus[%d] = 0\n", fd->nCurSimCardIndex);
                    break;
                }

                usleep(1*1000*1000);
                continue;
            }

            /* SIM卡状态正常 */
            nValidSimCnt++;
            fd->uSimCardStatus[fd->nCurSimCardIndex] = NORMAL;
            MODULE4G_INF("Get sIccid[%d]:%s\n", fd->nCurSimCardIndex, fd->sIccid[fd->nCurSimCardIndex]);
            /* 检测当前卡, 获取SIM卡信息 */
            ProduceTestNet();
            break;
        }

        fd->s_4GSimCardCount = nValidSimCnt;
		goto end;
    }

	/* 双卡设备，读取SIM信息，通过文件是否存在判断设备是否已经遍历过 */
    ret = ReadSIMInfoFile(fd);
    if (0 != ret)
    {
        MODULE4G_INF("SIMInfo Error, Start Get SimInfo\n");
    
        //获取卡是否存在,这里获取多卡存在状态
        for (i = 0; i < MAX_SIMCARD_CNT; i++)
        {
            /* 获取当前SIM卡ICCID信息，尝试3次 */
            for (int cnt = 2; cnt >= 0; cnt--)
            {
            	ret = SetATCommandICCID();
            	if ((-1 == ret) || (strlen(fd->sIccid[fd->nCurSimCardIndex]) == 0))
            	{
            		if (cnt == 0)
            		{
                        fd->uSimCardStatus[fd->nCurSimCardIndex] = ABNORMAL;
                        MODULE4G_ERR("Get SimCardStatus[%d] = 0\n", fd->nCurSimCardIndex);
                        break;
            		}

            		usleep(1*1000*1000);
            		continue;
            	}

                /* SIM卡状态正常 */
                nValidSimCnt++;
                fd->uSimCardStatus[fd->nCurSimCardIndex] = NORMAL;
                MODULE4G_INF("Get sIccid[%d]:%s\n", fd->nCurSimCardIndex, fd->sIccid[fd->nCurSimCardIndex]);

                /* 检测当前卡, 获取SIM卡信息 */
                ProduceTestNet();
                break;
            }

            if (0 == i)
            {
                /* 切换至另一张SIM卡 */
                NextSimCardIdx = (fd->nCurSimCardIndex == 0 ? 1 : 0);
                MODULE4G_INF("Switch SIM card [%d] ==> [%d]\n", fd->nCurSimCardIndex, NextSimCardIdx);

                ret = SetATCommandSwitchSim(NextSimCardIdx);
                if (-1 == ret)
                {
					fd->uSimCardStatus[NextSimCardIdx] = ABNORMAL;
                }

                /* 切换后重新获取当前使用的SIM卡 */
                SetATCommandGetCurrentSwitchSim();
            }
        }

        fd->s_4GSimCardCount = nValidSimCnt;

        /* 获取完成，将双SIM卡信息写入到文件中 */
        WriteSIMInfoFile(fd);

        /* 如果当前SIM卡异常，切回去 */
        if (fd->uSimCardStatus[fd->nCurSimCardIndex] == ABNORMAL)
        {
            NextSimCardIdx = (fd->nCurSimCardIndex == 0 ? 1 : 0);
            MODULE4G_INF("Switch SIM card [%d] ==> [%d]\n", fd->nCurSimCardIndex, NextSimCardIdx);
            ret = SetATCommandSwitchSim(NextSimCardIdx);
            if (0 == ret)
            {
                sleep(10);
            }
            SetATCommandGetCurrentSwitchSim();
        }
    }
    else
    {
        /* SIM卡信息读取成功，但是是单卡设备，还是去检测下ICCID是不是存在,测试拔卡操作 */
        if (fd->s_4GSimCardCount == 1)
        {
            /* 获取当前SIM卡ICCID信息，尝试3次 */
            for (int cnt = 2; cnt >= 0; cnt--)
            {
                ret = SetATCommandICCID();
                if ((-1 == ret) || (strlen(fd->sIccid[fd->nCurSimCardIndex]) == 0))
                {
                    if (cnt == 0)
                    {
                        fd->uSimCardStatus[fd->nCurSimCardIndex] = ABNORMAL;
                        MODULE4G_ERR("Get SimCardStatus[%d] = 0\n", fd->nCurSimCardIndex);
                        break;
                    }

                    usleep(1*1000*1000);
                    continue;
                }

                /* SIM卡状态正常 */
                fd->uSimCardStatus[fd->nCurSimCardIndex] = NORMAL;
                MODULE4G_INF("Get sIccid[%d]:%s\n", fd->nCurSimCardIndex, fd->sIccid[fd->nCurSimCardIndex]);

                /* 检测当前卡, 获取SIM卡信息 */
                ProduceTestNet();
                break;
            }

    		goto end;
        }
    }



end:
	MODULE4G_INF("SimCardCount[%d] CurSimCardIndex[%d] SimCardStatus[%d---%d]\n", 
        fd->s_4GSimCardCount,fd->nCurSimCardIndex, fd->uSimCardStatus[0], fd->uSimCardStatus[1]);
	SwitchSubState(PRODUCTTEST_CHECKFINISH_STATE);
	 return;
}

int Module4G_SetDevDidType(int DiDType)
{
	s_4GHandle.device_did_info = DiDType;
	return 0;
}

/*@breif ProduceTestCheckFinishHandler 产测结束
 *@param 
 *@return 
 *@       
 */
static void ProduceTestCheckFinishHandler()
{ 

	int i = 0;
	
	memset(&s_4GHandle.s_ProductTestData,0,sizeof(Module4G_ProductTest_Data_t));
	Module4G_ProductTest_Data_t *out_buf = &s_4GHandle.s_ProductTestData;

	//断开网卡
	SetATCommandQNETDEVCTL(0);
	//关闭拨号
	Module4G_DailDown();

	ParaAccessLock();
	ParaProductDataAccessLock();
	
	out_buf->uModuleStatus = s_4GHandle.uModuleStatus;
	out_buf->uInternetConnStatus = s_4GHandle.uInternetConnStatus;
	memcpy(out_buf->sImei,s_4GHandle.sImei,sizeof(s_4GHandle.sImei));
	memcpy(out_buf->sVersion,s_4GHandle.sVersion,sizeof(s_4GHandle.sVersion));
	
    //如果能获取到双卡的IMSI信息，则表示有双卡，否则设备判断为单卡
    if ((s_4GHandle.uSimCardStatus[0] == NORMAL) && (s_4GHandle.uSimCardStatus[1] == NORMAL))
    {
        s_4GHandle.s_4GSimCardCount = 2;
    }
    else
    {
        s_4GHandle.s_4GSimCardCount = 1;
    }

    for (i = 0; i < s_4GHandle.s_4GSimCardCount; i++)
    {
        out_buf->uCregStatus[i] = s_4GHandle.uCregStatus[i];
	    out_buf->iCsq[i] = s_4GHandle.iCsq[i];
    	out_buf->uSimCardStatus[i] = s_4GHandle.uSimCardStatus[i];
    	memcpy(out_buf->sImsi[i], s_4GHandle.sImsi[i], sizeof(s_4GHandle.sImsi[i]));
    	memcpy(out_buf->sIccid[i], s_4GHandle.sIccid[i], sizeof(s_4GHandle.sIccid[i]));
    	memcpy(out_buf->sACT[i], s_4GHandle.sACT[i], sizeof(s_4GHandle.sACT[i]));
    	memcpy(out_buf->sOper[i], s_4GHandle.sOper[i], sizeof(s_4GHandle.sOper[i]));
    	memcpy(out_buf->sBand[i], s_4GHandle.sBand[i], sizeof(s_4GHandle.sBand[i]));		
	}

	//清除产测标识
	s_4GHandle.uInternetConnStatus = 0;
    s_4GHandle.uRealNetConnStatus = 0;
	s_4GHandle.nProductTestFlag = 0;
	s_4GHandle.nProductTestFinish = 1;

	ParaAccessUnLock();
	ParaProductDataAccessUnLock();
}

/*@breif ProductTestStateHandler 产测模式处理
 *@param 
 *@return 
 *@       
 */
static void ProductTestStateHandler()
{
	switch (s_4GHandle.uSubState)
	{
		case PRODUCTTEST_CHECK_STATE:
			ProduceTestCheckHandler();
			break;
        case PRODUCTTEST_CLOUD_CARD_STATE:
			ProduceTestCloudCardHandler();
			break;
		case PRODUCTTEST_CHECKFINISH_STATE:
			ProduceTestCheckFinishHandler();
			break;
		default:
			break;
	}
}

/*@breif CheckErrorStateHandler 模组检测错误处理
 *@param 
 *@return 
 *@       
 */
static void CheckErrorStateHandler()
{
	int needReset = 0;
	int needReboot = 0;
	char cmd[128] = {0};
    int checkCount = 0;
	s_4GHandle.uErrorSleepTime++;

	if (s_4GHandle.uModuleStatus)
	{
        if (1 == s_4GHandle.uErrorNotSleepFlag)
        {//无需等待
            s_4GHandle.uErrorSleepTime = ERROR_SLEEP_TIME;
        }
    
		if (s_4GHandle.uErrorSleepTime >= ERROR_SLEEP_TIME)
		{
			s_4GHandle.uErrorSleepTime = 0;
			s_4GHandle.uCheckErrorFlag = 0;
			needReset = 1;

			MODULE4G_ERR("4G module NetWorking Failed,Reset Module\n");

#ifdef __SUPPORT_SAVE_TF_LOG__ 
			TF_GPRS_LOG_INFO("4G module NetWorking Failed,Reset Module\n");
#endif
		}
	}
	else
	{
		//模组检测异常错误
		if (s_4GHandle.uErrorSleepTime >= ERROR_NOMODULE_SLEEP_TIME)
		{
			s_4GHandle.uErrorSleepTime = 0;
			s_4GHandle.uCheckErrorFlag = 0;
			s_4GHandle.nDeviceResetFlag = 0;
			needReset = 1;

			MODULE4G_ERR("4G module Check Failed,Reset Module\n");
#ifdef __SUPPORT_SAVE_TF_LOG__ 
			TF_GPRS_LOG_INFO("4G module Check Failed,Reset Module\n");
#endif
		}
	}

	// 检测设备是否需要重启
	for (int i = 0; i < MaxErr; ++i)
	{
		if (s_4GNetErrTimes[SimCardErr] >= SIMCARDERRTIMES)
		{
			needReboot = 1;
#ifdef __SUPPORT_SAVE_TF_LOG__ 
		    TF_GPRS_LOG_INFO("4G module Check SimCardErr, Reboot Device\n");
#endif
			break;
		}
		else if (s_4GNetErrTimes[CregErr] >= CREGERRTIMES)
		{
			needReboot = 1;
#ifdef __SUPPORT_SAVE_TF_LOG__ 
			TF_GPRS_LOG_INFO("4G module Check CregErr, Reboot Device\n");
#endif
			break;
		}
		else if (s_4GNetErrTimes[DialErr] >= DIALERRTIMES)
		{
			needReboot = 1;
#ifdef __SUPPORT_SAVE_TF_LOG__ 
			TF_GPRS_LOG_INFO("4G module Check DialErr, Reboot Device\n");
#endif
			break;
		}
		else if (s_4GNetErrTimes[DhcpErr] >= DHCPERRTIMES)
		{
			needReboot = 1;
#ifdef __SUPPORT_SAVE_TF_LOG__ 
			TF_GPRS_LOG_INFO("4G module Check DhcpErr, Reboot Device\n");
#endif
			break;
		}
		
	}

	// 重启设备
	if (needReboot)
	{
		memset(s_4GNetErrTimes, 0, sizeof(s_4GNetErrTimes));
		MODULE4G_ERR("Net Err up to Max times , will reboot \n");
		//创建复位标识，复位设备
		RebootLinux(REBOOT_GPRSDEVICE_REST);
	}

	if (needReset)
	{
		/* 双卡设备尝试切换SIM卡 */
        if (s_4GHandle.s_4GSimCardCount > 1)
        {
            int other_simcard = (s_4GHandle.nCurSimCardIndex == 0 ? 1 : 0);
            MODULE4G_INF("Switch SIM card [%d] ==> [%d]\n", s_4GHandle.nCurSimCardIndex, other_simcard);

           SetATCommandSwitchSim(other_simcard);

            /* 切换后重新获取当前使用的SIM卡 */
            SetATCommandGetCurrentSwitchSim();

    		/* 生成标记文件，下次4G启动过程中不需要再切卡，直接就用当前切换后的卡联网 */
    		memset(cmd, 0, sizeof(cmd));
    		snprintf(cmd, sizeof(cmd)-1, "touch %s", DONT_CHANGESIM_FLAG);
    		mysystem(cmd);
        }
        else
        {
    		//这里须将当前使用SIM卡编号置位
    		ParaAccessLock();
    		s_4GHandle.nCurSimCardIndex = 0;
    		ParaAccessUnLock();
        }

		//复位4G模组
		Module4G_DailDown();
		
		//复位模组
		CloseSerialPort(s_4GHandle.iSerial_fd);
		s_4GHandle.iSerial_fd = -1;	
		Reset4GModule();
		sleep(1);
		//复位模组后重新判断一次模组节点以及ID
		int ret = Module4GIsOnline();
		if(ret < 0)
		{
			MODULE4G_ERR("Module4GIsOnline ERR return %d\n", ret);
		}

        //这里须将当前使用SIM卡编号置位
        ParaAccessLock();
    	s_4GHandle.nCurSimCardIndex = 0;
    	ParaAccessUnLock();

		MODULE4G_INF("4G module Reset Module Finish\n");
#ifdef __SUPPORT_SAVE_TF_LOG__ 
		TF_GPRS_LOG_INFO("4G module Reset Module Finish\n");
#endif

		GprsSleep(12);
        if (1 == s_4GHandle.uErrorNotSleepFlag)
        {//等待4G模组枚举成功即可
            while(checkCount <= MAX_MODULEPOWER_RESET_TIME)
        	{
        		if (0 == Module4GIsOnline())
        		{
        			MODULE4G_INF("wait online time: %d\n",checkCount);
        			break;
        		}

        		checkCount++;
        		GprsSleep(1);
        	}
        }
        else
        {//多等待一会，模组入网需要时间
            GprsSleep(MAX_MODULEPOWER_RESET_TIME);
        }

        s_4GHandle.uErrorNotSleepFlag = 0;
	}
}


/*@breif SleepGetInfoHandler 不使用4G模组时周期性获取信息
 *@param 
 *@return 
 *@       
 */
static void SleepGetInfoHandler()
{
#define GETINFO_PERIOD	30
	static int preiod_count = 30;
	int ret = 0;

	preiod_count++;
	if (preiod_count >= GETINFO_PERIOD)
	{
		preiod_count = 0;

		//模组检测
		ret = Module4GIsOnline();
		if (-1 == ret)
		{
			return;
		}

		if (s_4GHandle.iSerial_fd < 0)
		{
			ret = Open4GModule();
			if (-1 == ret)
			{
				return;
			}
		}

		//禁止回显
		ret = SetATCommandATE0();
		if (-1 == ret)
		{
			return;
		}

		//获取模组当前版本
		ret = SetATCommandQGMR();
		if (-1 == ret)
		{
			return; 
		}

		// 获取模组硬件版本号
		ret = GetModuleHardWareVersion();	
		if (-1 == ret)
		{
			return; 
		}

		//获取IMEI
		ret = SetATCommandCGSN();
		if (-1 == ret)
		{
			return;
		}

		//优先判断卡是否存在
#if 0
		ret = SetATCommandQSIMSTAT();
#else
		ret = SetATCommandICCID();
#endif
		if (-1 == ret)
		{
			return;
		}

		if (NORMAL == s_4GHandle.uSimCardStatus[s_4GHandle.nCurSimCardIndex])
		{
			//获取IMSI
			ret = SetATCommandCIMI();
			if (-1 == ret)
			{
				return;
			}
			
			if(ZHONGYUN_ZX800HG == s_ModuleType)
			{
				//查询act oper前先设置
				ret = SetATCommandCOPSNUM();
				if (-1 == ret)
				{
					return; 
				}
			}
			//查询act oper
			ret = SetATCommandCOPS();
			if (-1 == ret)
			{
				return;
			}

			//查询band
			ret = SetATCommandQNWINFO();
			if (-1 == ret)
			{
				return;
			}

			//查询入网状态
			ret = SetATCommandGetCEREG();
			if (-1 == ret)
			{
				return;
			}

			//查询信号质量
			ret = SetATCommandCSQ();
			if (-1 == ret)
			{
				return;
			}
		}
	}
}



/*@breif SleepStateHandler 休眠处理
 *@param 
 *@return 
 *@       
 */
static void SleepStateHandler()
{
	if (s_4GHandle.nModuleOpenFlag)
	{
		MODULE4G_ERR("SleepStateHandler  Close 4G Module\n");
#ifdef __SUPPORT_SAVE_TF_LOG__ 		
		TF_GPRS_LOG_INFO("SleepStateHandler  Close 4G Module\n");
#endif		
		//断开网卡
		SetATCommandQNETDEVCTL(0);
	
		//关闭4G网卡dhcp ，删除网卡路由
		Module4G_DailDown();
		
		//模组关闭
		PowerDown4GModule();

		//清除数据
		ManagerDateReset();
	}

	SleepGetInfoHandler();
}


/*@breif GetUpdateFile 获取升级文件名
 *@param 
 *@out filename 文件名
 *@return 0 成功
 *@       -1 失败
 */
static int GetUpdateFile(char *filename,int len)
{
	int ret = -1;

	if (NULL == filename)
	{
		MODULE4G_ERR("filename is NULL \n"); 
		return -1;
	}

	DIR  *dir = NULL;
    struct dirent *ptr = NULL;

    dir = opendir(MODULE_UPDATE_PATH);
    if (dir == NULL)
    {
        MODULE4G_ERR("open dir %s fail: %s\n", MODULE_UPDATE_PATH, strerror(errno));
        return -1;
    }

	MODULE4G_INF("GetUpdateFile s_ModuleType: %d  \n", s_ModuleType); 

    /* 读取pdir文件夹下所有文件名放入file_prefix_num数组里面 */
    while(NULL != (ptr=readdir(dir)))
    {
        /* 只判断普通文件 */
        if (ptr->d_type == 8)
        {
        	if (QUECTEL_EC800 == s_ModuleType)
        	{
	        	if (0 == strncmp(ptr->d_name,"EC800ECNLC",strlen("EC800ECNLC")))
	        	{
	        		//找到升级文件
	            	snprintf(filename, len, "%s/%s", MODULE_UPDATE_PATH, ptr->d_name);
	            	ret = 0;
	            }
	        }
	        else if (ZHONGYUN_ZX800 == s_ModuleType)
	        {
				if (0 == strncmp(ptr->d_name,"800EGFFRD30G",strlen("800EGFFRD30G")))
	        	{
	        		//找到升级文件
	            	snprintf(filename, len, "%s/%s", MODULE_UPDATE_PATH, ptr->d_name);
	            	ret = 0;
	            }
	        }
			else if (QUECTEL_EG800 == s_ModuleType)
			{
				if (0 == strncmp(ptr->d_name,"EG800QEULC",strlen("EG800QEULC")))
				{
					//找到升级文件
					snprintf(filename, len, "%s/%s", MODULE_UPDATE_PATH, ptr->d_name);
					ret = 0;
				}
			}
			// 只有800SGBFEI00G是给欧版ZX800-SG模组用的
	        else if (ZHONGYUN_ZX1602 == s_ModuleType)
	        {
				if ((0 == strncmp(ptr->d_name,"800RGBFRI00G",strlen("800RGBFRI00G")))
					|| (0 == strncmp(ptr->d_name,"800SGBFRI00G",strlen("800SGBFRI00G")))
					|| (0 == strncmp(ptr->d_name,"800SGBFEI00G",strlen("800SGBFEI00G")))
					|| (0 == strncmp(ptr->d_name,"800SGFFRI30G",strlen("800SGFFRI30G"))))
	        	{
	        		//找到升级文件
	            	snprintf(filename, len, "%s/%s", MODULE_UPDATE_PATH, ptr->d_name);
	            	ret = 0;
	            }
	        }
        }
    }
    closedir(dir);
    dir = NULL;

	MODULE4G_INF("final  filename :%s	\n", filename); 
	return ret;
}


/*@breif CheckUpdateFileMd5 校验升级文件md5值
 *@param 
 *@out filename 文件名
 *@return 0 校验失败
 *@       -1 失败
 *@		  1 校验成功	
 */
static int CheckUpdateFileMd5(char *updatefile,char *md5file)
{
	char calc_md5[64] = {0};
	char file_md5[64] = {0};
	int nFileMd5Len = 0;
	int nCalcMd5Len = 0;
	int fd = -1;
	int nRet = 0;

	if (NULL == updatefile || NULL == md5file)
	{
		MODULE4G_ERR("updatefile or md5file is NULL \n"); 
		return -1;
	}

	MODULE4G_INF("md5file = %s\n",md5file);
	fd = open(md5file, O_RDONLY);   
    if (-1 == fd)
    {
        MODULE4G_ERR("open file error:%s\n", md5file);
        return -1;  
    }

	memset(file_md5,0,sizeof(file_md5));
    nRet = read(fd, file_md5, 32);  
    MODULE4G_INF("nRet = %d\n",nRet);
    if (-1 == nRet)
    {  
        MODULE4G_ERR("read file error:%s\n", md5file); 
        close(fd);
        return -1;  
    }  

    close(fd);

	memset(calc_md5,0,sizeof(calc_md5));
	ComputeFileMd5sum(updatefile, calc_md5);

    nFileMd5Len = strlen(file_md5);
    nCalcMd5Len = strlen(calc_md5);
	

	MODULE4G_INF("len:%d file_md5:%s\n", nFileMd5Len,file_md5); 
	MODULE4G_INF("len:%d calc_md5:%s\n", nCalcMd5Len,calc_md5); 
	//比较md5值是否相等
    if ((nFileMd5Len == nCalcMd5Len) && (0 == strncmp(calc_md5,file_md5,nFileMd5Len)))
    {
		return 1;
    }

    return 0;
}


/*@breif CheckAllowUpdate 根据版本判断是否允许升级
 *@param 
 *@in filename 升级文件名
 *@in moduleversion 固件版本
 *@return 0 允许
 *@       -1 不允许
 */
static int CheckAllowUpdate(char *flashfilename,char *moduleversion)
{
	char *file_version_ptr = NULL;
	char *file_version_ptr_end = NULL;
	char *module_version_ptr = NULL;

	if (NULL == flashfilename || NULL == moduleversion)
	{
		MODULE4G_ERR("flashfilename or moduleversion is NULL \n");
		return -1;
	}

	if (QUECTEL_EC800 == s_ModuleType)
	{
		file_version_ptr = strstr(flashfilename,"EC800ECNLC");
		module_version_ptr = strstr(moduleversion,"EC800ECNLC");
	}
	else if (ZHONGYUN_ZX800 == s_ModuleType)
	{
		file_version_ptr = strstr(flashfilename,"800EGFFRD30G");
		module_version_ptr = strstr(moduleversion,"800EGFFRD30G");
	}
	else if (QUECTEL_EG800 == s_ModuleType)
	{
		file_version_ptr = strstr(flashfilename,"EG800QEULC");
		module_version_ptr = strstr(moduleversion,"EG800QEULC");
	}
	else if (ZHONGYUN_ZX1602 == s_ModuleType)
	{
		char Version_str[32] = {0};

		memset(Version_str,0,sizeof(Version_str));
		memcpy(Version_str,moduleversion,12);

		file_version_ptr = strstr(flashfilename,Version_str);
		module_version_ptr = moduleversion;
	}
	
	if (NULL == file_version_ptr || NULL == module_version_ptr)
	{
		MODULE4G_ERR("flashfilename :%s  moduleversion:%s\n",flashfilename,moduleversion);
		return -1;
	}

	file_version_ptr_end = strstr(file_version_ptr,"-");
	if (NULL == file_version_ptr_end)
	{
		MODULE4G_ERR("flashfilename :%s  moduleversion:%s\n",flashfilename,moduleversion);
		return -1;
	}

	MODULE4G_INF("flashfilename :%s  moduleversion:%s\n",flashfilename,moduleversion);

	file_version_ptr_end[0] = 0;

	MODULE4G_INF("flashfilename :%s  moduleversion:%s\n",file_version_ptr,module_version_ptr);

	if ((strlen(file_version_ptr) == strlen(module_version_ptr)) && (0 == strncmp(module_version_ptr,file_version_ptr,strlen(moduleversion))))
	{
		return 0;
	}

	return -1;
}


/*@breif GetFilelen 获取升级文件名
 *@param 
 *@in filename 升级文件名
 *@return  文件长度
 *@       -1 不允许
 */
static int GetFilelen(char* filename)
{
    int len;
    struct stat fileStat;

	if (NULL == filename)
	{
		MODULE4G_ERR("filename is NULL \n");
		return -1;
	}
    
    if (stat(filename,&fileStat) < 0)
    {
    	MODULE4G_ERR("stat failed \n");
		return -1;
    }
    
    len=fileStat.st_size;
 
    return len;
}


/*@breif ZY1602_UpdateWait 中云1602模组升级特殊处理
 *@param 
 *@in 
 *@return  
 *@       -1 失败
 */
static int ZY1602_UpdateWait(void)
{
	int count = 0;
	int ret = 0;

	//模组会重启，关闭句柄，防止生成的ttyUSB结点序号不对。
	CloseSerialPort(s_4GHandle.iSerial_fd);
	s_4GHandle.iSerial_fd = -1;

 	//在模组进行差分升级时，多等一会等模组正常，否则升级容易有问题(尤其针对欧版4g模组)
	MODULE4G_INF("diff test GprsSleep fuction start \n");
	int nwaittime = 0;
	while(1)
	{
		if(nwaittime > 20)
		{
			break;
		}
		usleep(500*1000);
		nwaittime ++;
	}
	MODULE4G_INF("diff test GprsSleep fuction over \n");
	
	MODULE4G_INF("diff upgrade  before download \n");
	mysystem("ls /dev/ttyUSB*");


	while(1)
	{
		if (-1 == Module4GIsOnline())
		{
			count++;
			if (count >= MAX_UPDATE_RESET_WAIT_TIME)
			{
				MODULE4G_ERR("module not online\n");
				s_4GHandle.nUpdate4GFlag = 0;
				return -1;
			}
		}
		else
		{
			break;
		}

		GprsSleep(1);
	}

	MODULE4G_INF("diff upgrade  before open  \n");
	mysystem("ls /dev/ttyUSB*");

	if (-1 == Open4GModule())
	{
		//打开串口失败
		MODULE4G_ERR("Open4GModule failed\n");
		s_4GHandle.nUpdate4GFlag = 0;
		return -1;
	}

	MODULE4G_INF("diff upgrade over open \n");
	mysystem("ls /dev/ttyUSB*");

	count = 0;
	while(1)
	{
		ret = CheckModuleStartUpdate(s_4GHandle.iSerial_fd);
		MODULE4G_INF("ZY1602_UpdateWait  ret :%d \n", ret);
		if (1 != ret)
		{
			count++;
			if (count > 3)
			{
				//打开串口失败
				MODULE4G_ERR("CheckModuleStartUpdate failed\n");
				s_4GHandle.nUpdate4GFlag = 0;
				return -1;
			}
		}
		else
		{
			break;
		}

		GprsSleep(1);
	}

	return 0;
}



/*@breif UpdateSendProcess 获取升级文件名
 *@param 
 *@in file 升级文件名
 *@in dev_fd 串口句柄
 *@return  0 成功
 *@       -1 失败
 */
static int UpdateSendProcess(char *file,int dev_fd)
{
	int total_len = 0;
	int current_pos = 0;
	char send_buf[1024];
	int send_len = 0;
	int file_fd = -1;
	int ret = -1;
	int readret = -1;
	uint64_t sleep_time = 1000000*(sizeof(send_buf)/(14*1024.0));

	if (NULL == file)
	{
		MODULE4G_ERR("filename is null \n");		
		return ret;
	}

	MODULE4G_INF("update file: %s\n",file);
	
	file_fd = open(file,O_RDONLY);
	if (file_fd < 0)
	{
		MODULE4G_ERR("file open failed \n");
		return ret;
	}
	total_len = GetFilelen(file);
	if (total_len < 0)
	{
		MODULE4G_ERR("GetFilelen failed \n");
        close(file_fd);
		return ret;
	}

	if (0 == s_4GHandle.nDonotSendStartUpdateFlag)
	{
		//发送升级指令
		if (SetATCommandQFOTADL(total_len) < 0)
		{
			MODULE4G_ERR("SetATCommandQFOTADL failed \n");
			return ret;
		}

		if (ZHONGYUN_ZX1602 == s_ModuleType)
		{
			if (-1 == ZY1602_UpdateWait())
			{
				MODULE4G_ERR("ZY1602_UpdateWait failed \n");
				return -1;
			}

			dev_fd = s_4GHandle.iSerial_fd;
		}
	}

	GprsSleep(2);
	MODULE4G_INF("update file len: %d  sleep_time: %d\n",total_len,sleep_time);

	while(current_pos != total_len)
	{
		if ((current_pos + sizeof(send_buf)) < total_len)
		{
			send_len = sizeof(send_buf);
		}
		else
		{
			send_len = total_len - current_pos;
		}

		if (lseek(file_fd, current_pos, 0) != current_pos)
		{
			MODULE4G_ERR("file lseek failed \n");
            close(file_fd);
			return ret;
		}
	
    	if ((readret = read(file_fd ,send_buf, send_len)) != send_len)
    	{
    		MODULE4G_ERR("file read failed ret = %d\n",readret);
            close(file_fd);
			return ret;
    	}


    	if ((readret = write(dev_fd ,send_buf, send_len)) != send_len)
    	{
    		MODULE4G_ERR("devfd write failed ret = %d\n",readret);
			break;
    	}

		current_pos += send_len;

		MODULE4G_INF("current_pos = %d\n",current_pos);

		if (ZHONGYUN_ZX1602 != s_ModuleType)
		{
			usleep(sleep_time);
		}
		else
		{
			usleep(5000);
		}
	}

	close(file_fd);

	MODULE4G_INF("send file finish\n");

	return 0;
}


/*
升级前要获取模组版本，进行特殊处理（仅针对于移远的欧版模组）
查询模组版本AT+QGMR:
如果版本是：EG800QEULCR01A08M04_01.201.01.201或者可以提取R01A08作为判断。
就要设置如下指令：AT+QCFG="urc/ri/other","off"（指令立即生效，不用重启）

如果版本不是：EG800QEULCR01A08M04_01.201.01.201或者可以提取R01A08作为判断。
就要设置如下指令：AT+QCFG="urc/ri/other","pulse"（指令立即生效，不用重启）

AT+QCFG="urc/ri/other"	  //为查询指令，设置返回结果：
+QCFG: "urc/ri/other","pulse",120,1
+QCFG: "urc/ri/other","off",120,1
*/
static void SetUpdateCmdByModuleVersion(void)
{
	if (QUECTEL_EG800 != s_ModuleType)
	{
		MODULE4G_ERR("no EG800QEU module \n");
		return;
	}

	const char *ModuleVersion = "EG800QEULCR01A08M04_01.201.01.201";
	int type = 0;
	
	if (-1 == SetATCommandATE0())
	{
		goto Failed; 
	}

	//获取模组内固件版本
	if (-1 == SetATCommandQGMR())
	{
		goto Failed;
	}
	
	if (0 == strncmp(s_4GHandle.sVersion, ModuleVersion, strlen(ModuleVersion)))
	{
		MODULE4G_INF("[version] detect EG800QEULCR01A08M04 version \n");
	}
	else
	{
		type = 1;
	}
	
	SetATCommandQCFGOther(type);

	return;
	
Failed:
	//关闭串口，清除4G检测标识
	CloseSerialPort(s_4GHandle.iSerial_fd);
	s_4GHandle.iSerial_fd = -1;
	s_4GHandle.nUpdate4GFlag = 0;	

}


// 中云信安的模组获取硬件版本号
static int GetModuleHardWareVersion(void)
{
	if (ZHONGYUN_ZX1602 != s_ModuleType && ZHONGYUN_ZX800HG != s_ModuleType)
	{
		MODULE4G_INF("[return]s_ModuleType not 1602|ZX800HG \n");
		return 0;
	}

	MODULE4G_INF("start to get hardware version info \n");

	//获取模组内固件版本
	if (-1 == SetATCommandATCommandFHWVER())
	{
		return -1;
	}

	
	return 0;
}



/*@breif UpdateCheckUpdatingHandler 检测4G模组是否正在升级
 *@param 
 *@return 
 *@       
 */
static void UpdateCheckUpdatingHandler()
{
	int ret = -1;
	char cmdstring[128] = {0};
	int count = 0;

	if (0 == access(UPDATE_RESET_FLAG,F_OK))
	{
		memset(cmdstring,0,sizeof(cmdstring));
		snprintf(cmdstring,sizeof(cmdstring)-1,"rm %s -rf",UPDATE_RESET_FLAG);
		mysystem(cmdstring);

		if (ZHONGYUN_ZX1602 != s_ModuleType)
		{
			//已经升级失败一次，这次不升级
			goto Failed;
		}
	}

	if (-1 == Module4GIsOnline())
	{
		//无4G模组结点
		goto Failed;
	}

	if (-1 == Open4GModule())
	{
		//打开串口失败
		goto Failed;
	}
	
	SetUpdateCmdByModuleVersion();


	// 检查串口是否有打印 FILESTART, 如果有则需要进行下发差分升级文件
	if (ZHONGYUN_ZX1602 == s_ModuleType)
	{	
		while(1)
		{
			ret = CheckModuleStartUpdate(s_4GHandle.iSerial_fd);
			if (1 == ret)
			{
				MODULE4G_INF("count = %d\n",count);
				//要重新下载升级包
				s_4GHandle.nDonotSendStartUpdateFlag = 1;
				SwitchSubState(UPDATE_SENDFILE_STATE);

				return;
			}

			count++;
			if (count >= CHECK_MODULE_UPDATE_TIME)
			{
				MODULE4G_INF("time out count = %d\n",count);
				break;
			}
		}
	}
	

	if (-1 == SetATCommandATTEST())
	{
		// 中云模组无法响应at指令也认为需要重新下发升级包
		if (ZHONGYUN_ZX1602 == s_ModuleType)
		{
			//要重新下载升级包
			s_4GHandle.nDonotSendStartUpdateFlag = 1;
			MODULE4G_INF("at test no respond ! \n");
			SwitchSubState(UPDATE_SENDFILE_STATE);

			return;
		}


		ret = CheckModuleUpdating(s_4GHandle.iSerial_fd);
		if (ret > 0)
		{
			//正在升级
			//MODULE4G_ERR("[upgrade check] upgrade ret :%d \n", ret);
			s_4GHandle.nNeedCheckUpdateAgain = 1;
			SwitchSubState(UPDATE_WAIT_FINISH_STATE);
		}
		else if (ret == 0)
		{
			//没有升级
			SwitchSubState(UPDATE_CHECK_STATE);
		}
		else
		{
			goto Failed;
		}
	}
	else
	{
		//没有升级
		SwitchSubState(UPDATE_CHECK_STATE);
	}

	return;

Failed:
	//关闭串口，清除4G检测标识
	CloseSerialPort(s_4GHandle.iSerial_fd);
	s_4GHandle.iSerial_fd = -1;
	s_4GHandle.nUpdate4GFlag = 0;
}


/*@breif UpdateCheckHandler 检测4G模组是否需要升级
 *@param 
 *@return 
 *@       
 */
static void UpdateCheckHandler()
{
	char update_file[256] = {0};

	//获取升级文件名
	if (-1 == GetUpdateFile(&update_file[0],sizeof(update_file)-1))
	{
		goto Failed;
	}

	//检测升级文件是否被破坏，通过md5值
	if (1 != CheckUpdateFileMd5(update_file,MODULE_UPDATE_FILE_MD5))
	{
		goto Failed;
	}

	//获取模组内固件版本
	if (-1 == SetATCommandATE0())
	{
		goto Failed; 
	}

	//获取模组内固件版本
	if (-1 == SetATCommandQGMR())
	{
		goto Failed;
	}

	//判断版本是否允许升级
	if (0 == CheckAllowUpdate(&update_file[0],&s_4GHandle.sVersion[0]))
	{
		SwitchSubState(UPDATE_SENDFILE_STATE);
	}
	else
	{
		goto Failed;
	}

	return;

Failed:
	//关闭串口，清除4G检测标识
	CloseSerialPort(s_4GHandle.iSerial_fd);
	s_4GHandle.iSerial_fd = -1;
	s_4GHandle.nUpdate4GFlag = 0;	
}


/*@breif UpdateSendFileHandler 下发升级文件
 *@param 
 *@return 
 *@       
 */
static void UpdateSendFileHandler()
{
	char update_file[256] = {0};

	MODULE4G_INF("start to get filename !\n"); 

	//获取升级文件名
	if (-1 == GetUpdateFile(&update_file[0],sizeof(update_file)))
	{
		goto Failed;
	}

	//下发升级文件
	if (-1 == UpdateSendProcess(&update_file[0],s_4GHandle.iSerial_fd))
	{
		goto Failed;
	}
	
	//模组会重启，关闭句柄，防止生成的ttyUSB结点序号不对。
	CloseSerialPort(s_4GHandle.iSerial_fd);
	s_4GHandle.iSerial_fd = -1;
	SwitchSubState(UPDATE_WAIT_FINISH_STATE);

	if (ZHONGYUN_ZX1602 != s_ModuleType)
	{
		GprsSleep(6);
	}
	
	return;

Failed:
	//关闭串口，清除4G检测标识
	CloseSerialPort(s_4GHandle.iSerial_fd);
	s_4GHandle.iSerial_fd = -1;
	s_4GHandle.nUpdate4GFlag = 0;	
}


/*@breif UpdateCheckThread 检测升级状态
 *@param 
 *@return 
 *@       
 */
static void* UpdateCheckThread(void *lparam)
{
    pthread_detach(pthread_self());

    //获取当前实例
    G4_Control_Manager_t *pControl = (G4_Control_Manager_t *)lparam;
	int total_count = 0;
	char cmd[128] = {0};

    while(1)
    {
		if (pControl->uUpdateCheckThreadExitFlag)
		{
			MODULE4G_INF("Update Finish Exit\n");
			break;
		}		

		total_count++;
		if (total_count > MAX_UPDATE_TIME)
		{
			//模组升级超时，重启设备
			memset(cmd,0,sizeof(cmd));
			snprintf(cmd,sizeof(cmd)-1,"touch %s",UPDATE_RESET_FLAG);
			mysystem(cmd);
			MODULE4G_ERR("Update Long time %d\n",total_count);
			RebootLinux(REBOOT_GPRSDEVICE_REST);
		}
	
		GprsSleep(1);
    }

    return NULL;
}


/*@breif CreateUpdateCheckThread 创建模组升级检测线程
 *@param 
 *@return 
 *@       
 */
static void CreateUpdateCheckThread()
{
	pthread_t pThread;
	int ret = 0;

	s_4GHandle.uUpdateCheckThreadExitFlag = 0;

	// 创建秒钟计时器线程
    ret = pthread_create(&pThread, NULL, UpdateCheckThread, &s_4GHandle);
    if (ret < 0)
    {
        MODULE4G_ERR("pthread_create<UpdateCheckThread> failed(%d)\n", ret);
    }
}


/*@breif ExitUpdateCheckThread 销毁模组升级检测线程
 *@param 
 *@return 
 *@       
 */
static void ExitUpdateCheckThread()
{
	s_4GHandle.uUpdateCheckThreadExitFlag = 1;
}


/*@breif SpeechSimCardThread 播报sim检测成功，优化时间
 *@param 
 *@return 
 *@       
 */
static void* SpeechSimCardThread(void *lparam)
{
    pthread_detach(pthread_self());

    //获取当前实例
    G4_Control_Manager_t *pControl = (G4_Control_Manager_t *)lparam;

    while(1)
    {
		if (pControl->uNeedSpeechSimSuccess)
		{
			SetSpeechContent(SPEECH_4G_SIM_CHECK_SUCCESS);
			pControl->uNeedSpeechSimSuccess = 0;
		}		

		WatchDog_Count++;
	
		GprsSleep(1);
    }

    return NULL;
}



/*@breif CreateSpeechSimCardThread 创建播报sim卡语音线程
 *@param 
 *@return 
 *@       
 */
static void CreateSpeechSimCardThread()
{
	pthread_t pThread;
	int ret = 0;

	s_4GHandle.uNeedSpeechSimSuccess = 0;

	// 创建秒钟计时器线程
    ret = pthread_create(&pThread, NULL, SpeechSimCardThread, &s_4GHandle);
    if (ret < 0)
    {
        MODULE4G_ERR("pthread_create<UpdateCheckThread> failed(%d)\n", ret);
    }
}



/*@breif UpdateWaitFinishHandler 等待升级完成
 *@param 
 *@return 
 *@       
 */
static void UpdateWaitFinishHandler()
{
	int count = 0;
	int progress = 0;
	char check_tty[32] = {0};
	char cmd[128] = {0};

	if (QUECTEL_EC800 == s_ModuleType)
	{
		snprintf(check_tty,sizeof(check_tty),"/dev/ttyUSB3");
	}
	else if (ZHONGYUN_ZX800 == s_ModuleType)
	{
		snprintf(check_tty,sizeof(check_tty),"/dev/ttyUSB6");
	}
	else if (ZHONGYUN_ZX1602 == s_ModuleType)
	{
		snprintf(check_tty,sizeof(check_tty),"/dev/ttyUSB4");
	}
	else if (QUECTEL_EG800 == s_ModuleType)
	{
		snprintf(check_tty,sizeof(check_tty),"/dev/ttyUSB4");
	}
	
	while(1)
	{
		GprsSleep(1);
	
		if (-1 == Module4GIsOnline())
		{
			count++;
			if (count >= MAX_UPDATE_RESET_WAIT_TIME)
			{
				MODULE4G_ERR("module not online\n");
				s_4GHandle.nUpdate4GFlag = 0;
				return;
			}

			if (0 == access(check_tty,F_OK))
			{
				//结点生成不对，重启设备
				memset(cmd,0,sizeof(cmd));
				snprintf(cmd,sizeof(cmd)-1,"touch %s",UPDATE_RESET_FLAG);
				mysystem(cmd);
				
				MODULE4G_ERR("module has %s\n",check_tty);
				RebootLinux(REBOOT_GPRSDEVICE_REST);
			}
		}
		else
		{
			break;
		}
	}

	
	if (0 == s_4GHandle.nNeedCheckUpdateAgain)
	{
		//下发了升级文件后，来到这里
		if (-1 == Open4GModule())
		{
			//打开串口失败
			MODULE4G_ERR("Open4GModule failed\n");
			s_4GHandle.nUpdate4GFlag = 0;
			return;
		}
	}

	CreateUpdateCheckThread();

	count = 0;
	while(1)
	{
		progress = CheckModuleUpdating(s_4GHandle.iSerial_fd);
		if (100 == progress)
		{
			//升级完成
			MODULE4G_INF("Update finish %d\n",s_4GHandle.iSerial_fd);
			CloseSerialPort(s_4GHandle.iSerial_fd);
			s_4GHandle.iSerial_fd = -1;
			break;
		}
	}

	GprsSleep(6);

	count = 0;
	while(1)
	{
		if (-1 == Module4GIsOnline())
		{
			count++;
			if (count >= MAX_UPDATE_RESET_WAIT_TIME)
			{
				MODULE4G_ERR("module not online\n");
				s_4GHandle.nUpdate4GFlag = 0;
				return;
			}

			if (0 == access(check_tty,F_OK))
			{
				//结点生成不对，重启设备
				memset(cmd,0,sizeof(cmd));
				snprintf(cmd,sizeof(cmd)-1,"touch %s",UPDATE_RESET_FLAG);
				mysystem(cmd);
				MODULE4G_ERR("module has %s\n",check_tty);
				RebootLinux(REBOOT_GPRSDEVICE_REST);
			}
		}
		else
		{
			break;
		}

		GprsSleep(1);
	}

	if (s_4GHandle.nNeedCheckUpdateAgain)
	{
		SwitchSubState(UPDATE_CHECK_UPDATING_STATE);
		s_4GHandle.nNeedCheckUpdateAgain = 0;
	}
	else
	{
		s_4GHandle.nUpdate4GFlag = 0;
	}

	ExitUpdateCheckThread();

	//升级结束
}



/*@breif Update4GStateHandler 升级4G模块处理
 *@param 
 *@return 
 *@       
 */
static void Update4GStateHandler()
{
	switch (s_4GHandle.uSubState)
	{
		case UPDATE_CHECK_UPDATING_STATE:
			UpdateCheckUpdatingHandler();
			break;
		case UPDATE_CHECK_STATE:
			UpdateCheckHandler();
			break;
		case UPDATE_SENDFILE_STATE:
			UpdateSendFileHandler();
			break;
		case UPDATE_WAIT_FINISH_STATE:
			UpdateWaitFinishHandler();
			break;	
		default:
			break;
	}
}


/*@breif ManagerProcessHandler 主状态处理
 *@param 
 *@return 
 *@       
 */
static void ManagerProcessHandler()
{
	int cur_state = 0;

	GetTaskState(&cur_state);
	LinkSpeechHandler();
	Module4G_WatchDog_Handler();

	switch (cur_state)
	{
		case TASK_USE4G_STATE:
			BindStateHandler();
			break;
		case TASK_PRODUCTTEST_STATE:
			ProductTestStateHandler();
			break;
		case TASK_CHECK_ERROR_STATE:
			CheckErrorStateHandler();
			break;
		case TASK_SLEEP_STATE:
			SleepStateHandler();
			break;
		case TASK_UPDATE_MODULE_STATE:
			Update4GStateHandler();
			break;
		default:
			break;
	}
}


/*@breif GPRS_Init GPRS模块初始化
 *@return 成功返回 0,更改模式返回1
 *@      失败返回-1
 */
void Module4G_Init(void)
{
	char cmdstring[128];

    memset(&s_4GHandle, 0, sizeof(s_4GHandle));	
	memset(&s_4GHandle.s_ProductTestData,0,sizeof(s_4GHandle.s_ProductTestData));
    s_4GHandle.iSerial_fd = -1;
    s_4GHandle.ucWorkMode = 99;
	s_4GHandle.s_4GSimCardCount = MAX_SIMCARD_CNT;//4g模组真正支持的sim卡数量，为兼容欧版4g单卡模组逻辑和国内4g模组
	memset(s_4GNetErrTimes, 0, sizeof(s_4GNetErrTimes));
    pthread_mutex_init(&s_4GHandle.MutexParaAccess, NULL);
    pthread_mutex_init(&s_4GHandle.MutexProductDataAccess, NULL);
	s_4GHandle.s_SwitchCardsState = 0; //切卡状态默认未切卡
	s_4GHandle.s_SwitchCardsResult = -1; //切卡结果默认-1，表示无效状态

	//设备因无模组重启标识
    if (0 == access(DEVICE_RESET_FLAG,F_OK))
    {
		s_4GHandle.nDeviceResetFlag = 1;

		memset(cmdstring,0,sizeof(cmdstring));
		snprintf(cmdstring,sizeof(cmdstring)-1,"rm %s -rf",DEVICE_RESET_FLAG);
		mysystem(cmdstring);
    }

    //设备因读写失败重启标识
    if (0 == access(DEVICE_RESET_COM_FLAG,F_OK))
    {
		s_4GHandle.uComErrorRebootFlag = 1;

		memset(cmdstring,0,sizeof(cmdstring));
		snprintf(cmdstring,sizeof(cmdstring)-1,"rm %s -rf",DEVICE_RESET_COM_FLAG);
		mysystem(cmdstring);
    }

	//开机优先检测是否需要升级4G固件
    s_4GHandle.nUpdate4GFlag = 1;
    s_4GHandle.eLinkSpeechControl = LINKSPEECH_WAIT;

    PowerUp4GModule();
	s_4GHandle.uBindMode = 0;

	DeviceBind_GetSCode();

	//优化播报网络连接成功时间
    CreateSpeechSimCardThread();

#ifdef __SUPPORT_SAVE_TF_LOG__
    TFLOG_Init();
#endif    

#ifdef __SUPPORT_SAVE_TF_LOG__ 
	TF_GPRS_LOG_INFO("4G Init\n");
#endif	
}

/*@breif GPRS_UnInit GPRS模块反初始化 */
void Module4G_UnInit(void)
{
    Module4G_DailDown();
    PowerDown4GModule();
    if (s_4GHandle.iSerial_fd > 0)
    {
        GprsSleep(5);
        CloseSerialPort(s_4GHandle.iSerial_fd);
    }

    pthread_mutex_destroy(&s_4GHandle.MutexParaAccess);
    pthread_mutex_destroy(&s_4GHandle.MutexProductDataAccess);
}


/*@breif GPRS_GetModuleSignalQuality 检查Sim卡的信号强度
 *@param [OUT] signal_quality 当前模块的信号强度
 *@return 成功返回 0
 *@       失败返回-1
 */
int Module4G_GetModuleSignalQuality(int *signal_quality)
{
    if (!signal_quality)
    {
        MODULE4G_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

	ParaAccessLock();
	*signal_quality = s_4GHandle.iCsq[s_4GHandle.nCurSimCardIndex];
	ParaAccessUnLock();

    return 0;
}


/*@breif Module4G_GetModule4GSignalQuality 获取4G信号强度
 *@param [OUT] signal_quality 当前模块的信号强度
 *@return 成功返回 0
 *@       失败返回-1
 */
int Module4G_GetModule4GSignalQuality(int *signal_quality)
{
    if (!signal_quality)
    {
        MODULE4G_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

	ParaAccessLock();
	*signal_quality = s_4GHandle.iSignal4G;

	MODULE4G_INF("Module4G_GetSimInfo, s_4GHandle.iSignal4G[%d]=%d\n",s_4GHandle.nCurSimCardIndex, s_4GHandle.iSignal4G);
	
	ParaAccessUnLock();

    return 0;
}

/*@breif Module4G_GetSimIndexSignalQuality 获取指定sim卡一些信息
 *@param [OUT] nIsInUse 指定sim卡:是否在用，szOperator:运营商，nSignalStrength:信号强度，szICCID:iccid, szIMEI:imei
 *@return 成功返回 0
 *@ 	  失败返回-1
 */
int Module4G_GetSimIndexInfo(int nSim, int* nIsInUse, char *szOperator, int *nSignalStrength, char *szICCID, char *szIMEI)
{
	if (!nIsInUse || !szOperator || !nSignalStrength || !szICCID || !szIMEI)
	{
		MODULE4G_ERR("NULL POINTER EXCEPTION!\n");
		return -1;
	}

	G4_Control_Manager_t *fd = NULL;
	Module4G_GetSystemHandleInfo(&fd);

	/* 获取指定sim卡信息 */
	int i = nSim;

	ParaAccessLock();
	if (i == fd->nCurSimCardIndex)
	{
		MODULE4G_INF("nSim=%d inuse\n", nSim);
		*nIsInUse = 1;
	}
	else 
	{
		*nIsInUse = 0;
	}

    memcpy(szOperator, fd->s_ProductTestData.sOper[i], sizeof(fd->s_ProductTestData.sOper[i]));
	memcpy(szICCID, fd->s_ProductTestData.sIccid[i], sizeof(fd->s_ProductTestData.sIccid[i]));
    memcpy(szIMEI, fd->s_ProductTestData.sImei, sizeof(fd->s_ProductTestData.sImei));
    *nSignalStrength = fd->s_ProductTestData.iCsq[i];

	if (i >= fd->s_4GSimCardCount)
	{
		memset(szOperator, 0, MAX_OPER_LEN);
		memset(szICCID, 0, MAX_ICCID_LEN);
		*nSignalStrength = 0;
	}

    MODULE4G_INF("i=%d szOperator=%s, szIMEI=%s, szICCID=%s, *nSignalStrength=%d,uSimCardStatus[i]=%d\n", 
        i, szOperator, szIMEI, szICCID, *nSignalStrength, fd->s_ProductTestData.uSimCardStatus[i]);
	ParaAccessUnLock();


	return 0;
}



/*@breif Module4G_GetModuleSimStatus 获取sim卡状态
 *@param [OUT] CregStatus 
 *@return 成功返回 0
 *@       失败返回-1
 */
int Module4G_GetModuleSimStatus(int *SimStatus)
{
	if (!SimStatus)
    {
        MODULE4G_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }


	ParaAccessLock();
	*SimStatus = s_4GHandle.uSimCardStatus[s_4GHandle.nCurSimCardIndex];
	ParaAccessUnLock();

    return 0;
}

/*@breif Module4G_GetModuleInternetConnStatus 设置当前模块的联网状态为不通网
 *@return 成功返回 0
 *@       失败返回-1
 */
static int Module4G_SetModuleInternetConnStatus(int status)
{
	ParaAccessLock();
	s_4GHandle.uInternetConnStatus = status;
    s_4GHandle.uRealNetConnStatus = status;
	ParaAccessUnLock();
	
    return 0;
}



/*@breif Module4G_GetModuleInternetConnStatus 获取当前模块的联网状态
 *@param [OUT] status 当前模块的联网状态
 *@return 成功返回 0
 *@       失败返回-1
 */
int Module4G_GetModuleInternetConnStatus(int *status)
{
    if (!status)
    {
        MODULE4G_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

	ParaAccessLock();
	if (s_4GHandle.nProductTestFlag)
	{
		//产测下，认为网络没有连接
		*status = 0;
	}
	else
	{
		*status = s_4GHandle.uInternetConnStatus;
	}
	ParaAccessUnLock();

    return 0;
}

int Module4G_GetModuleRealInternetConnStatus(int *status)
{
    if (!status)
    {
        MODULE4G_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

	ParaAccessLock();
	if (s_4GHandle.nProductTestFlag)
	{
		//产测下，认为网络没有连接
		*status = 0;
	}
	else
	{
		*status = s_4GHandle.uRealNetConnStatus;
	}
	ParaAccessUnLock();

    return 0;
}


/*@breif Module4G_GetIccid 获取当前卡的iccid
 *@param [OUT] Iccid 当前卡的iccid
 *@return 成功返回 0
 *@       失败返回-1
 */
int Module4G_GetIccid(char *Iccid)
{
    if (!Iccid)
    {
        MODULE4G_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }
	G4_Control_Manager_t *fd = NULL;
	Module4G_GetSystemHandleInfo(&fd);

	ParaAccessLock();
	memcpy(Iccid, s_4GHandle.sIccid[s_4GHandle.nCurSimCardIndex], sizeof(s_4GHandle.sIccid[s_4GHandle.nCurSimCardIndex]));
	ParaAccessUnLock();

    return 0;
}


/*@breif Module4G_GetVersion 获取模组当前版本
 *@param [OUT] Iccid 当前卡的iccid
 *@return 成功返回 0
 *@       失败返回-1
 */
int Module4G_GetVersion(char *Version)
{
    if (!Version)
    {
        MODULE4G_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

	ParaAccessLock();
	memcpy(Version,s_4GHandle.sVersion,strlen(s_4GHandle.sVersion));

	MODULE4G_INF("Module4G_GetVersion, s_4GHandle.sVersion=%s\n",s_4GHandle.sVersion);
	
	ParaAccessUnLock();

    return 0;
}



/*@breif Module4G_SetWorkMode 设置绑定模式
 *@param [OUT] mode 是否自动切卡
 *@return 成功返回 0
 *@       失败返回-1
 */
int Module4G_SetWorkMode(int mode)
{
	if (mode >= MAX_WORKMODE)
    {
		MODULE4G_ERR("para error mode = %d\n",mode);
        return -1;
    }
	
	ParaAccessLock();
	s_4GHandle.uWorkMode = mode;	
	MODULE4G_ERR("Module4G_SetWorkMode  = %d\n",mode);
	ParaAccessUnLock();

    return 0;
}

/*@breif Module4G_SetBindMode 设置绑定模式
 *@param [OUT] mode 设备是否已绑定 0未绑定，1 已绑定
 *@return 成功返回 0
 *@       失败返回-1
 */
int Module4G_SetBindMode(int mode)
{
	if (mode >= MAX_WORKMODE)
    {
		MODULE4G_ERR("Module4G_SetBindMode error mode = %d\n",mode);
        return -1;
    }
	
	ParaAccessLock();
	s_4GHandle.uBindMode = mode;	
	MODULE4G_ERR("Module4G_SetBindMode  = %d\n",mode);
	ParaAccessUnLock();

    return 0;
}


/*@breif Module4G_SetWireConnStatus 设置当前有线连接状态
 *@param [In] status 有线连接状态
 *@return 成功返回 0
 *@       失败返回-1
 */
int Module4G_SetWireConnStatus(int status)
{
    if (status > MAX_STATUS)
    {
		MODULE4G_ERR("para error status = %d\n",status);
        return -1;
    }

	ParaAccessLock();
	s_4GHandle.uWireConnStatus = status;
	ParaAccessUnLock();

    return 0;
}


/*@breif Module4G_SetAllowFlag 设置是否允许使用4G
 *@param [In] flag 标识
 *@return 成功返回 0
 *@       失败返回-1
 */
int Module4G_SetAllowFlag(int flag)
{
	ParaAccessLock();
	s_4GHandle.uAllowFlag = flag;
	ParaAccessUnLock();

    return 0;
}

/*@breif Module4G_LinkSpeechControl 设置是否允许使用4G
 *@param [In] flag 标识
 *@return 成功返回 0
 *@       失败返回-1
 */
int Module4G_LinkSpeechControl(int eControl)
{
	ParaAccessLock();
	s_4GHandle.eLinkSpeechControl = eControl;
	ParaAccessUnLock();

    return 0;
}

//获取云卡设备工作模式
int Module4G_GetUcWorkMode(int *piUcWorkMode)
{
    if(NULL == piUcWorkMode)
    {
        MODULE4G_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    ParaAccessLock();
	*piUcWorkMode = s_4GHandle.ucWorkMode;
	ParaAccessUnLock();

    return 0;
}

//获取是否是云卡设备
int Module4G_GetIsCloudCardDev(void)
{
    int iIsCloudCardDev = 0;

    ParaAccessLock();
    if(s_4GHandle.sHardVersion[5] == 'C')
    {
        iIsCloudCardDev = 1;
    }
    else
    {
        iIsCloudCardDev = 0;
    }
    ParaAccessUnLock();

    return iIsCloudCardDev;
}


/*@breif Module4G_GetSimInfo 获取当前卡的信息
 *@param [OUT] Iccid 当前卡的信息
 *@return 成功返回 0
 *@       失败返回-1
 */
int Module4G_GetSimInfo(char *imsi, 
								char *imei, 
								char *act, 
								char *oper, 
								char *band,
								char *iccid,
								char *Version)
{
    if (!iccid)
    {
        MODULE4G_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

	ParaAccessLock();
	MODULE4G_INF("Module4G_GetSimInfo s_4GHandle.uSimCardStatus[%d] :%d \n", s_4GHandle.nCurSimCardIndex,s_4GHandle.uSimCardStatus[s_4GHandle.nCurSimCardIndex]);

	memcpy(imei,s_4GHandle.sImei,sizeof(s_4GHandle.sImei));
	if (s_4GHandle.uSimCardStatus[s_4GHandle.nCurSimCardIndex])
	{
		memcpy(iccid,s_4GHandle.sIccid[s_4GHandle.nCurSimCardIndex],sizeof(s_4GHandle.sIccid[s_4GHandle.nCurSimCardIndex]));
		memcpy(imsi,s_4GHandle.sImsi[s_4GHandle.nCurSimCardIndex],sizeof(s_4GHandle.sImsi[s_4GHandle.nCurSimCardIndex]));
		memcpy(act,s_4GHandle.sACT[s_4GHandle.nCurSimCardIndex],sizeof(s_4GHandle.sACT[s_4GHandle.nCurSimCardIndex]));
		memcpy(oper,s_4GHandle.sOper[s_4GHandle.nCurSimCardIndex],sizeof(s_4GHandle.sOper[s_4GHandle.nCurSimCardIndex]));
		memcpy(band,s_4GHandle.sBand[s_4GHandle.nCurSimCardIndex],sizeof(s_4GHandle.sBand[s_4GHandle.nCurSimCardIndex]));
        memcpy(Version,s_4GHandle.sVersion,sizeof(s_4GHandle.sVersion));
	}
	else
	{
		memcpy(iccid,NO_SIMCARD,strlen(NO_SIMCARD));
		memcpy(imsi,NO_SIMCARD,strlen(NO_SIMCARD));
		memcpy(act,NO_SIMCARD,strlen(NO_SIMCARD));
		memcpy(oper,NO_SIMCARD,strlen(NO_SIMCARD));
		memcpy(band,NO_SIMCARD,strlen(NO_SIMCARD));
        memcpy(Version,NO_SIMCARD,strlen(NO_SIMCARD));
	}
	ParaAccessUnLock();

	MODULE4G_INF("Module4G_GetSimInfo, MSI=%s, MEI=%s, ACT=%s, Operator=%s, Band=%s, ICCID=%s, Version=%s\n",
			imsi, imei, act, oper, band,iccid,Version);

    return 0;
}



/*@breif Module4G_GetProductTestData 获取产测数据
 *@param [OUT] out_buf 
 *@return 成功返回 0
 *@       失败返回-1
 *@       正在测试-2
 */
int Module4G_GetProductTestData(Module4G_ProductTest_Data_t *out_buf)
{
    int i = 0;
    
    if (!out_buf)
    {
        MODULE4G_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

	//判断是否产测结束
	ParaAccessLock();
    if (!s_4GHandle.nProductTestFinish)
    {
    	ParaAccessUnLock();		
        MODULE4G_ERR("s_4GHandle.nProductTestFinish =% d!\n",s_4GHandle.nProductTestFinish);
		return -2;
    }
    ParaAccessUnLock();

	ParaProductDataAccessLock();

	out_buf->uModuleStatus = s_4GHandle.s_ProductTestData.uModuleStatus;
	out_buf->uInternetConnStatus = s_4GHandle.s_ProductTestData.uInternetConnStatus;
	memcpy(out_buf->sImei,s_4GHandle.s_ProductTestData.sImei,sizeof(s_4GHandle.s_ProductTestData.sImei));
	memcpy(out_buf->sVersion,s_4GHandle.s_ProductTestData.sVersion,sizeof(s_4GHandle.s_ProductTestData.sVersion));

    for (i = 0; i < s_4GHandle.s_4GSimCardCount; i++)
    {
        if(i >= s_4GHandle.s_4GSimCardCount)
        {
            continue;
        }
        out_buf->uCregStatus[i] = s_4GHandle.s_ProductTestData.uCregStatus[i];
        out_buf->iCsq[i] = s_4GHandle.s_ProductTestData.iCsq[i];
		if(s_4GHandle.s_ProductTestData.uSimCardStatus[i])
		{
			out_buf->uSimCardStatus[i] = s_4GHandle.s_ProductTestData.uSimCardStatus[i];
			memcpy(out_buf->sImsi[i],s_4GHandle.s_ProductTestData.sImsi[i], sizeof(s_4GHandle.s_ProductTestData.sImsi[i]));
			memcpy(out_buf->sIccid[i], s_4GHandle.s_ProductTestData.sIccid[i], sizeof(s_4GHandle.s_ProductTestData.sIccid[i]));
			memcpy(out_buf->sACT[i], s_4GHandle.s_ProductTestData.sACT[i], sizeof(s_4GHandle.s_ProductTestData.sACT[i]));
			memcpy(out_buf->sOper[i], s_4GHandle.s_ProductTestData.sOper[i], sizeof(s_4GHandle.s_ProductTestData.sOper[i]));
			memcpy(out_buf->sBand[i], s_4GHandle.s_ProductTestData.sBand[i], sizeof(s_4GHandle.s_ProductTestData.sBand[i]));
		}
	}
	
	MODULE4G_INF("Module4G_GetProductTestData %d d!\n",i);
	ParaProductDataAccessUnLock();

    return 0;
}

/*@breif Module4G_SetProductTest 产测开始
 *@param [In] TestStart 设置产测开始
 *@return 成功返回 0
 *@       失败返回-1
 */
int Module4G_SetProductTest(int TestStart)
{
	ParaAccessLock();
    s_4GHandle.nProductTestFlag = TestStart;
    ParaAccessUnLock();

    return 0;
}


/*@breif Module4G_SetProductMode 产测开始
 *@param [In] TestStart 设置产测开始
 *@return 成功返回 0
 *@       失败返回-1
 */
int Module4G_SetProductMode(int nMode)
{
	ParaAccessLock();
    s_4GHandle.uProductMode = nMode;
    ParaAccessUnLock();

    return 0;
}

/*@breif Module4G_SetCloudCardOnly 设置是否只使用云卡
 *@param [In] TestStart 设置只使用云卡模式
 *@return 成功返回 0
 *@       失败返回-1
 */
int Module4G_SetCloudCardOnly(int CloudCardStatus)
{
	ParaAccessLock();
    s_4GHandle.IsSupportOnlyCloudCard = CloudCardStatus;
    ParaAccessUnLock();

    return 0;
}


/*@breif Module4G_SetTutkStatus 设置tutk状态
 *@param [In] nTutkStatus tutk状态
 *@return 成功返回 0
 *@       失败返回-1
 */
int Module4G_SetTutkStatus(int nTutkStatus)
{
	ParaAccessLock();
    s_4GHandle.nTutkStatus = nTutkStatus;
    ParaAccessUnLock();

    return 0;
}


/*@breif Module4G_SetAllowUseEth0Flag 设置是否允许使用有线标识
 *@param [In] uAllowUseEth0Flag 
 *@return 成功返回 0
 *@       失败返回-1
 */
int Module4G_SetAllowUseEth0Flag(int uAllowUseEth0Flag)
{
	ParaAccessLock();
    s_4GHandle.uAllowUseEth0Flag = uAllowUseEth0Flag;
    ParaAccessUnLock();

    return 0;
}


/*@breif Module4G_IsModuleTypeSet 模组类型是否设置完毕
 *@param 空
 *@return 返回模组类型是否设置完毕
 */
int Module4G_IsModuleTypeSet        ()
{
    return (1 == s_4GHandle.nSetModuleTypeEnd)?1:0;
}

/*@breif AT+FCISCFG=RSLPMode 查询设备4g模块版本
 *@       成功返回0
 *@       失败返回-1
 */
int SetATCommandGetRSLPMode()
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+FCISCFG=RSLPMode\r\n");
    return AtCommandSendAndReceiveHandler(at_cmd);
}

/*@breif AT+FCISCFG=RSLPMode,<mode> 设置设备4g模块版本
 *@       成功返回0
 *@       失败返回-1
 */
int SetATCommandSetRSLPMode(int mode)
{
    char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+FCISCFG=RSLPMode,%d\r\n", mode);
    return AtCommandSendAndReceiveHandler(at_cmd);
}

/*@breif AT+RESET 设备4g模块软复位
 *@       成功返回0
 *@       失败返回-1
 */
int SetATCommandSoftResetModlue()
{
	char at_cmd[64] = {0};
    snprintf(at_cmd, sizeof(at_cmd), "AT+RESET\r\n");
    return AtCommandSendAndReceiveHandler(at_cmd);
}


int NET_4GModule_DealModuleType(void)
{
	G4_Control_Manager_t *fd = NULL;
	Module4G_GetSystemHandleInfo(&fd);

	int ret = 0;

	/* 1. 初始化4G模组 */
	/* 打开串口 */
	if (fd->iSerial_fd < 0)
	{
		ret = Open4GModule();
		if (-1 == ret)
		{
			return -1;
		}
	}

	fd->uModuleStatus = 1;

	/* AT测试通讯 */
	ret = SetATCommandATTEST();
	if (0 != ret)
	{
		return -1;
	}

	/* 关回显 */
	ret = SetATCommandATE0();
	if (0 != ret)
	{
		return -1;
	}

    /* 获取硬件版本 */
	ret = GetModuleHardWareVersion();	
	if (-1 == ret)
	{
		return -1; 
	}

    /* 当前是1605模组，检测模组工作模式 */
	if (NULL != strstr(fd->sHardVersion, ModuleVersionType_1605)||
		NULL != strstr(fd->sHardVersion, ModuleVersionType_HaiSi))
    {
		fd->s_IsSgOrVg4gModule = 1;
    
        /* 获取模组工作模式 */
        ret = SetATCommandGetRSLPMode();
        MODULE4G_INF("ret(%d) CurrentRSLPMode(%d)\n", ret, fd->nRSLPMode);

        /* 当前工作模式不是常电模式 */
        if (fd->nRSLPMode != RSLPMode_NORMAL)
        {
            /* 设置模组工作模式为常电模式 */
            ret = SetATCommandSetRSLPMode(RSLPMode_NORMAL);
            if (0 != ret)
            {
                MODULE4G_ERR("SetRSLPMode Failed(%d)\n", ret);
                return -1;
            }

            //设置模组工作模式后需要ReSet模组
            SetATCommandSoftResetModlue();
            CloseSerialPort(fd->iSerial_fd);
            fd->iSerial_fd = -1;
            MODULE4G_INF("Set RSLPMode, Reset the 4G module!\n");
			
			/* 打开串口 */
			if (fd->iSerial_fd < 0)
			{
				ret = Open4GModule();
				if (-1 == ret)
				{
					return -1;
				}
			}

            /* 再次获取模组工作模式 */
            ret = SetATCommandGetRSLPMode();
            if (0 != ret)
            {
                MODULE4G_ERR("GetRSLPMode Failed(%d)\n", ret);
                return -1;
            }

            if (fd->nRSLPMode != RSLPMode_NORMAL)
            {
                MODULE4G_ERR("Switch RSLPMode Failed(%d) (%d)\n", ret, fd->nRSLPMode);
                return -1;
            }
        }

        //双卡，走产测流程
        Module4G_SetProductTest(1);
    }

	return 0;	
}


/*@breif Module4G_ManagerProcessThread 4G模组处理主处理线程
 *@param [In] param 线程传入参数
 *@return 成功返回 0
 *@       失败返回-1
 */
void *Module4G_ManagerProcessThread(void *param)
{
	G4_Control_Manager_t *arg = (G4_Control_Manager_t *)param;
	s_4GHandle.GetSCode = arg->GetSCode;

	//之前是12s，语音播报，设备启动完成后，才能播报模块检测成功。
	//现在需要先做一次产测，可缩短等待时间

	//防止模组枚举慢导致产测失败。
	WaitModuleOnline();

	//判断当前4g模块是1605还是1602版本
	NET_4GModule_DealModuleType();
	
	if (0 == s_4GHandle.s_IsSgOrVg4gModule)
	{
		//1602模组，设置实际支持sim卡数
		Set4GRealSimCardCount(1);
	}

	GetATCommandSIM01CardStatus();	
	MODULE4G_INF("sim_01_card_status:%d,device_did_info:%d\n",s_4GHandle.sim_01_card_status,s_4GHandle.device_did_info);

	while(1)
	{
		ManagerProcessHandler();
		GprsSleep(1);
	}

	return NULL;
}

//设置云卡能力集回调
int Module4G_SetCloudCardCapCallback(SETCLOUDCARD_CAP_CB pCallback)
{
    s_SetCloudCardCb = pCallback;
    return 0;
}


/*设置4G模块配置*/
int Module4G_Set4GControlConfig(int nValid, int nCardID)
{
	int nRet = 0;
	
	G4_Control_Manager_t *fd = NULL;
	Module4G_GetSystemHandleInfo(&fd);
	if (NULL == fd)
	{
		fd->s_SwitchCardsResult = -1;
		MODULE4G_ERR("4G module not init\n");
		return -1;
	}
	//参数检查
	if(nValid == 0 || (nCardID > 1 || nCardID < 0))
	{
		fd->s_SwitchCardsResult = -1;
		MODULE4G_ERR("parm err!!!!");
		return IDVR_CIU_COMMAND_4G_SIMSWITCH_INVALIDPARAM;
	}
	//要切的卡是当前卡
	if(nCardID == fd->nCurSimCardIndex)
	{
		fd->s_SwitchCardsResult = -1;
		MODULE4G_ERR("pcfg->nCardID=%d is same as cur!!!!");
		return IDVR_CIU_COMMAND_4G_SIMSWITCH_INVALIDPARAM;
	}
	//单卡设备不支持切卡
	if(fd->s_4GSimCardCount < 2)
	{
		fd->s_SwitchCardsResult = -1;
		MODULE4G_ERR("4G module not support switch card\n");
		return IDVR_CIU_COMMAND_4G_SIMSWITCH_NOSUPPORT;
	}
	//切卡状态不能在响应切卡
	ParaAccessLock();
	if(fd->s_SwitchCardsState == 1)
	{
		fd->s_SwitchCardsResult = -1;
		MODULE4G_ERR("4G_SIMSWITCH_ING !!!!!!\n");
		ParaAccessUnLock();
		return IDVR_CIU_COMMAND_4G_SIMSWITCH_ING;
	}
	ParaAccessUnLock();
	//若4G子状态不在PING，则不允许切卡
	if(fd->uSubState != CHECK_PING_STATE)
	{
		fd->s_SwitchCardsResult = -1;
		MODULE4G_ERR("4G module state err,not CHECK_PING_STATE\n");
		return IDVR_CIU_COMMAND_4G_SIMSWITCH_STATES_FAILD;
	}
	ParaAccessLock();
	//参数校验成功后，将切卡状态切到切卡中
	fd->s_SwitchCardsState = 1;
	ParaAccessUnLock();
	//4G子状态切到切卡状态
	SwitchSubState(SIMSWITCH_CARDS_STATE);

	
	return nRet;
}

/*获取4G模块配置*/
int Module4G_Get4GControlConfig(int *nValid,int *nCardID,char *cICCID,int *nAutoSwitchCard)
{
	G4_Control_Manager_t *fd = NULL;
	Module4G_GetSystemHandleInfo(&fd);
	int nRet = 0;
	if (NULL == fd)
	{
		MODULE4G_ERR("4G module not init\n");
		return -1;
	}
	*nValid = 1;
	*nCardID = fd->nCurSimCardIndex;
	memcpy(cICCID, fd->sIccid[fd->nCurSimCardIndex], sizeof(fd->sIccid[fd->nCurSimCardIndex]));
	*nAutoSwitchCard = fd->s_4GSimCardCount<2 ? 1 : 0;
	MODULE4G_INF("nCardID=%d,ICCID=%s,nAutoSwitchCard=%d",*nCardID,cICCID,*nAutoSwitchCard);
	return nRet;

}

/*获取4G切卡结果*/
int Module4G_Get4GSwitchResult(int *nValid,int *iSelectErrCode)
{
	G4_Control_Manager_t *fd = NULL;
	Module4G_GetSystemHandleInfo(&fd);
	int nRet = 0;
	if (NULL == fd)
	{
		MODULE4G_ERR("4G module not init\n");
		return -1;
	}
	*nValid = 1;
	if(fd->s_SwitchCardsState == 1)
	{
		*iSelectErrCode = 7;
		MODULE4G_INF("nSwitchResult=%d",*iSelectErrCode);
		return nRet;
	}
	*iSelectErrCode = fd->s_SwitchCardsResult;
	MODULE4G_INF("nSwitchResult=%d",*iSelectErrCode);
	return nRet;

}


#endif
