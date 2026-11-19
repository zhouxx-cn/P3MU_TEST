#include <stdio.h>
#include <unistd.h>
#define _GNU_SOURCE
#include <string.h>
#undef _GNU_SOURCE
#include <time.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ioctl.h>
#include <syslog.h>
#include <stdarg.h>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <dirent.h>
#include <pthread.h>
#include <mtd/mtd-user.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#ifdef __MODULE_4G_SUPPORT__
#include <termios.h>
#include "zwsdk.h"
#include "md5Lib.h"
#endif
//#include "mysystem.h"
#include "utils.h"
#include "upgrade.h"
#include <sys/shm.h>
#include "../../../DVR_SDK/include/Authority.h"
#include "../../../DVR_SDK/PUB_BORAD/PUB_BoradInfo.h"

#define TEST_NAND_RW
#define CFG_AND_CUSTOM_USE_TAR

#define UPDATE_STATE_FILE "/tmp/uploadstate" //被写入状态的文件
//状态宏
#define UPGRADE_UNSTARTED 0			//未开启
#define UPGRADE_CHECKING 1			//校验升级包
#define UPGRADE_CHECKING_FAIL 2		//校验失败
#define UPGRADE_WRITING 4			//写入中
#define UPGRADE_SOFTWARE_EXC 5		//软件错误
#define UPGRADE_COMPLETE 9			//升级完成

#define UPDATEFILENAME_SUCC					"/tmp/app/.update_success"
#define UPDATEFILENAME_SUCC_CLOUD_UPGRADE   "/tmp/app/.update_success_cloud_upgrade"
#define UPGRADE_PASSPORT_FILE       		"/mnt/mtd/config/.upgradePassport"
#define UPDATE_PROCESS_PATH        			"/mnt/app/bin/update"
#define UPDATE_PROCESS_PATH_TMP     		"/tmp/update"
#define UPGRADE_PASSPORT_TMP_FILE   		"/tmp/.upgradePassport"
#define READ_BUF_SIZE           256
#define APP_NAME                		"xrscam"
#define PROCESS_NAME_BOA                "boa"
#define PROCESS_NAME_MONITOR            "MonitorProcess"
#define PROCESS_NAME_WPA                "wpa_supplicant"
#define PROCESS_NAME_UDHCPC             "udhcpc"
#define PROCESS_NAME_HOSTAPD            "hostapd"
#define PROCESS_NAME_LBH                "lbh_server"
#define MAX_PATH                255
#define MAX_WAIT_SEC			(1 * 60)
#define BOARD_TYPE_MAX_LENTH    64
#define MIN(x,y) (x < y ? x : y)

#define CLOUD_UPGRADE_RET_DELAY (5)    //云升级返回延时
#define UPDATEFILENAME          "/tmp/app/zw_device_update.bin"
#define DESTROY_UPLOAD_OVER_FLAG    "/tmp/app/.destroy_over"
#define DSP_ADDR_INFO "/tmp/app/.dspAddrInfo"
#define DSP_ADDR_INFO_FOR_MMZ "/tmp/app/.dspAddrInfoForMmz"
#define KEY_NAME   "/tmp"
#define KEY_ID     (0x18)

#define DEVICE_INFO_FILE_CFG          "/mnt/app/device/DeviceType.txt"

int processPid=0;
static unsigned int g_dspAddr=0;
static unsigned int g_dspPhyAddr=0;
static unsigned int g_dspLen=0;
static unsigned int g_readPoint=0;
static char g_ifUseDspMem=0;

typedef struct tageBoardT
{
	unsigned int  boardInfo;
	char ImageCode[64];
} BOARD_INFO;

#define MAX_BOARD_NUM 50

#ifdef __MODULE_4G_SUPPORT__
#define MODULE4G_ERR(format, args...) do{printf("\033[1;31m[GPRS][%s-%d]\033[m" format, __FILE__, __LINE__, ##args);}while(0)
#define MODULE4G_DBG(format, args...) do{printf("\033[1;32m[GPRS][%s-%d]\033[m" format, __FILE__, __LINE__, ##args);}while(0)
#define MODULE4G_INF(format, args...) do{printf("\033[1;35m[GPRS][%s-%d]\033[m" format, __FILE__, __LINE__, ##args);}while(0)
#define MODULE4G_SYS(format, args...) do{printf("\033[1;32m[GPRS][%s-%d](%s) \033[m" format, __FILE__, __LINE__, strerror(errno), ##args);}while(0)

#define TTY_DEVICE          "/dev/ttyUSB1"					//与模组通讯串口设备结点
//移远EG800Q-EU
#define EG800QEU_TTY_DEVICE "/dev/ttyUSB2"					//与模组通讯串口设备结点
#define TTY_DEVICE1         "/dev/ttyUSB0"					//与模组通讯串口设备结点
#define TTY_DEVICE_H        "/dev/ttyUSB3"					//与模组通讯串口设备结点
#define TTY_DEVICE_H_1      "/dev/ttyUSB4"					//与模组通讯串口设备结点

#define MEIG_MODULE_USBID	"0903"							//usb id
#define ZX800_MODULE_USBID  "0001"							//中云信安ZX800
#define EG800QEU_MODULE_USBID	"6007"						//移远EG800Q-EU usb id
#define ZX1602_MODULE_USBID "3010"							//中云信安1602
#define ZX800SG_MODULE_USBID  "3010"						//中云信安ZX800-SG
#define ZX800HG_MODULE_USBID  "3361"						//中云信安ZX800-HG

#define MODULE_4G_LOG       "/tmp/.4g_module_log" 			//日志文件
#define MODULE_UPDATE_PATH	"/tmp/"	//4G模块升级文件路径
#define MODULE_UPDATE_PATH_BAK	"/tmp/app/disk/mmcblk0p1/output/"	//4G模块升级文件路径
#define MODULE_UPDATE_FILE_MD5 "/tmp/updatefile_md5.txt"	//4G模块升级文件md5值
#define UPDATE_RESET_FLAG	"/mnt/mtd/config/.update4gresetdevice"			//因升级4g失败复位设备标识文件
#define AT_RESP_OK_STR		"OK"				//at命令处理正常返回值
#define AI_RESET_MODULE     "AT+CFUN=1,1"		//at复位命令处理错误返回值 
#define AT_MSG_SEPARATOR	"\r\n"				//消息分割符
#define AT_REPORT_SEPARATOR	"+"					//上报消息标识
#define MAX_VERSION_LEN       	(128)			//模组固件版本长度
#define SEND_WAIT_TIME			(2)				//发送at命令等待超时时间 s
#define MAX_UPDATE_RESET_WAIT_TIME  (30)        //升级过程中模组重启等待时长
#define MAX_UPDATE_TIME				(600)		//最长升级时间，超过该时间认为升级失败
#define CHECK_MODULE_UPDATE_TIME	(3)			//检测1602模组是否在升级模式，s
#define MAX_RECV_DATA_LEN		(1024)			//接收缓冲区大小
#define MAX_SERIAL_RETRY_COUNT	(3)				//串口通讯无响应重试次数
#define SEND_RETRY_SLEEP_TIME	(1)				//重发at命令休眠时间 s

//4G升级各状态
typedef enum {
	UPDATE_CHECK_UPDATING_STATE,//检测是否正在升级，因上次断电未升级完成
	UPDATE_CHECK_STATE,			//检测是否需要升级，通过对比固件和升级文件版本
	UPDATE_SENDFILE_STATE,		//下发升级文件，下发完后4G模组会进行重启一次
	UPDATE_WAIT_FINISH_STATE,   //等待升级完成，升级完成后4G模组会再重启一次
}eUPDATETASK_STATE;

/*AT 管理体结构定义*/
typedef struct
{
	int 	iSerial_fd;								/*与模组通讯串口句柄*/
	int		uSubState;								/*子任务状态*/
    int 	nUpdate4GFlag;							/*升级标识，标识是否执行升级4G模组流程*/
	int		nNeedCheckUpdateAgain;					/*重新检测标识，标识是否再次检测4G模组升级*/
	int 	uUpdateCheckThreadExitFlag;				/*升级检测线程退出标识*/
	int 	nDonotSendStartUpdateFlag;				/*不下发开始升级指令标识*/	
	char	sVersion[MAX_VERSION_LEN];				/*模组固件版本*/
    int     rndistype;								/*协议类型*/
	int     netcardtype;							/*网卡模式*/
	pthread_mutex_t MutexParaAccess;
	pthread_mutex_t MutexProductDataAccess;
}G4_Control_Manager_t;

//解析AT返回字符串回调函数
typedef int	(*RECEIVE_DATA_PARSE_CALLBACK)(const char *at_cmd,const char * rece_buf,int rece_len);

typedef struct {
	char cmd[32];
	RECEIVE_DATA_PARSE_CALLBACK handler;
}Receive_Parse_Map_t;

//4G 控制结构
static G4_Control_Manager_t  s_4GHandle;
//模组类型
static int s_ModuleType = UNKNOW_MODULE;
//设备结点
static char szTtyDevice[32] = {0};

#define BYTE_PER_SEND 64

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

static int QGMR_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len);
static int QCFG_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len);
static int NAT_Receive_Parse_handler(const char *at_cmd,const char *rece_buf,int rece_len);

//AT命令返回解析函数
static Receive_Parse_Map_t s_Receive_Pares_Map[] = {
	{"AT+QGMR",QGMR_Receive_Parse_handler},
	{"AT+QCFG=\"usbnet\"",QCFG_Receive_Parse_handler}, 
	{"AT+QCFG=\"nat\"",NAT_Receive_Parse_handler},
};
#endif

long file_size(char *path)
{
	struct stat buf;

	int fd;

	fd = open(path, O_RDONLY);
	if (fd == -1)
		return 0;
	fstat(fd, &buf);
	close(fd);
	return buf.st_size;
}

#define LOCALTIME(StartTime,tm,year,nMonth,nDay,nHour,nMinute,nSecond)		localtime_r(&StartTime, &tm);\
				year = tm.tm_year + 1900;\
			nMonth = tm.tm_mon + 1;\
			nDay = tm.tm_mday;\
			nHour = tm.tm_hour;\
			nMinute  = tm.tm_min;\
			nSecond  = tm.tm_sec;

#define write_disk_error(x...)  \
{ \
		int len = file_size("/tmp/app/disk_error.txt");\
		FILE *file;\
		if (len > 1024 * 1024)\
		{\
					file = fopen("/tmp/app/disk_error.txt","wb");\
				}\
		else\
		{\
					file = fopen("/tmp/app/disk_error.txt","ab+");\
				}\
		if ( NULL == file)\
		{\
					printf("\n[%s %d] open disk_error.txt failed\n",__FILE__,__LINE__);\
				}\
		else\
		{\
					struct tm tm;time_t time1 = time(NULL);\
					int year,month,day,hour,mini,sec; \
					LOCALTIME(time1,tm,year,month,day,hour,mini,sec);\
					fprintf(file,"%d - %d - %d  %d -%d -%d\n",year,month,day,hour,mini,sec);\
					fprintf(file,"[%s %d]\n",__FUNCTION__,__LINE__);\
					fprintf(file,x);\
					fprintf(file,"\n");\
					fclose(file);\
				}\
}

/* 安全删除宏函数(unlink只能删除文件，remove文件目录都可以删除，尽量避免使用命令rm) */
#define SafeRemove(x) do{  \
    if (unlink(x))         \
    {                      \
        remove(x);         \
    }                      \
}while(0)

//#define DEBUG
static int programStart = 0;
static int startTime = 0;
static int rebootFlag = 0;//有时会出现reboot执行后不重启的现象，所以增加此标志来停止喂狗

#define CHECK_INTERVAL			5

typedef struct t_MsgRecvBuf
{
	int m_nType;
	int m_nMsg;
	int m_nTaskID;
	int m_nTimeSpace;
	char m_szTaskName[128];
} T_MSGRBUF;

#define MSGBUF_LEN	(sizeof(T_MSGRBUF))
static int active_thread_number;

//1 被监控任务总数为10，有效ID为0-9
#define MAX_WATCH_TASK_ID	(10)

typedef struct
{
	int WatchEn;
	int NotifyCount;
	char TaskName[128];
} TASK_INFO;

TASK_INFO task_info[MAX_WATCH_TASK_ID];

int g_nTimeSpace[MAX_WATCH_TASK_ID];

//watch dog相关
static int startWatchDog = 0;
static int fd_wdt=0;
static int noFeedDog=0;

#define WATCHDOG_DEV_NODE   "/dev/watchdog"
#define	WATCHDOG_IOCTL_BASE	'W'
#define	WDIOS_DISABLECARD	0x0001	/* Turn off the watchdog timer */

#define WDIOC_GETSUPPORT     _IOR(WATCHDOG_IOCTL_BASE, 0, struct watchdog_info)
#define WDIOC_GETSTATUS      _IOR(WATCHDOG_IOCTL_BASE, 1, unsigned int)
#define WDIOC_GETBOOTSTATUS  _IOR(WATCHDOG_IOCTL_BASE, 2, unsigned int)
#define WDIOC_SETOPTIONS     _IOWR(WATCHDOG_IOCTL_BASE, 4, unsigned int)
#define WDIOC_KEEPALIVE      _IO(WATCHDOG_IOCTL_BASE, 5)
#define WDIOC_SETTIMEOUT     _IOWR(WATCHDOG_IOCTL_BASE, 6, int)
#define WDIOC_GETTIMEOUT     _IOR(WATCHDOG_IOCTL_BASE, 7, int)

static int mysystem(const char *cmdstring)
{
	pid_t pid;

	int status;

	if (cmdstring == NULL)
		return 1;
	if ((pid = vfork()) < 0)
		status = -1;
	else if (pid == 0)
	{
		execl("/bin/sh", "sh", "-c", cmdstring, (char *) 0);
		_exit(127);
	}
	else
	{
		while (waitpid(pid, &status, 0) < 0)
			if (errno != EINTR)
			{
				status = -1;
				break;
			}
	}
	return status;
}

//!\brief 将升级过程中的各个状态写到文件
//!\praram int state 状态值
//!\return  
void WriteStateToFile(int state)
{
    char fst[64];
    if (0 != access(UPDATE_STATE_FILE,F_OK))
    {
        memset(fst,0,sizeof(fst));
        snprintf(fst, sizeof (fst), "touch %s",UPDATE_STATE_FILE);
        mysystem(fst);
        
        memset(fst,0,sizeof(fst));
        snprintf(fst, sizeof (fst), "echo state=%d > %s",state,UPDATE_STATE_FILE);
        mysystem(fst);
    }else
    {
        memset(fst,0,sizeof(fst));
        snprintf(fst, sizeof (fst), "echo state=%d > %s",state,UPDATE_STATE_FILE);
        mysystem(fst);
    }

}

static int enable_watch_dog(int enable)
{
	int timeout;
	char cmd[128];
		
	if(enable)
	{
		if(0 == access("/tmp/app/watchDogReboot",F_OK))//使能看门狗时需要删除此文件
		{
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "rm %s", "/tmp/app/watchDogReboot");
			system(cmd);
		}
	
		if(startWatchDog)
		{
			printf("watch dog has been enbale !\n");
			return 0;
		}
		if ((fd_wdt = open(WATCHDOG_DEV_NODE, O_RDWR, 0)) < 0) 
		{
			perror(WATCHDOG_DEV_NODE);
			return -1;
		}
		
		/*if (ioctl(fd_wdt, WDIOC_SETOPTIONS, WDIOS_ENABLECARD) < 0) 
		{
			perror("WDIOC_SETOPTIONS");
			return -1;
		}*/
		int a=30;
		if (ioctl(fd_wdt, WDIOC_SETTIMEOUT, &a) < 0) 
		{
			perror("WDIOC_SETTIMEOUT");
			return -1;
		}
		if (ioctl(fd_wdt, WDIOC_GETTIMEOUT, &timeout) < 0) 
		{
			perror("WDIOC_GETTIMEOUT");
			return -1;
		}
		printf("watch dog timeout=%d\n",timeout);
		startWatchDog = 1;
	}
	else
    {
        if(0 == startWatchDog)
        {
            printf("watch dog has been closed !\n");
            return 0;
        }
        int b=WDIOS_DISABLECARD;
        if (ioctl(fd_wdt, WDIOC_SETOPTIONS, &b) < 0) 
        {
            perror("WDIOC_SETOPTIONS");
            return -1;
        }
        printf("watch dog close !\n");
        startWatchDog = 0;
    }

	return 0;
}

int clear_watch_dog()
{
	//如果需要重启则不喂狗
	if(0 == access("/tmp/app/watchDogReboot",F_OK))
	{
		printf("**********need reboot, don't feed dog!**********\n");
		return 0;		
	}

	if(!startWatchDog)
	{
		printf("watch dog don't open !\n");
		return -1;
	}
	
	if(0 == noFeedDog)
	{
		if (ioctl(fd_wdt, WDIOC_KEEPALIVE) < 0) 
		{
			perror("WDIOC_KEEPALIVE");
			return -1;
		}
	}
	return 0;
}


int stop_watch_dog()
{
	int stop = 1;

	if ((fd_wdt = open(WATCHDOG_DEV_NODE, O_RDWR, 0)) < 0) 
	{
		perror(WATCHDOG_DEV_NODE);
		return -1;
	}

	if (ioctl(fd_wdt, WDIOC_SETOPTIONS, &stop) < 0) 
	{
		perror("WDIOC_SETOPTIONS");
        close(fd_wdt);
		return -1;
	}

	close(fd_wdt);

	return 0;
}

int WATCH_LOG(__const char *__restrict __format, ...)
{
	char s[500];

	va_list argptr;

	int cnt;

	memset(s, 0, sizeof (s));

	va_start(argptr, __format);
	cnt = vsnprintf(s,sizeof(s), __format, argptr);
	va_end(argptr);

	printf("%s", s);

	//openlog ("watch", LOG_PID, LOG_USER);
	syslog(LOG_INFO, "%s", s);
	//closelog ();

	return (cnt);
}

void feed_dog()
{
	clear_watch_dog();
}

/*   C H E C K   T I M E   O U T   */

/*-------------------------------------------------------------------------
    Owner: xy@higheayrd.com
 
    检查是否有超时的线程
    input: 计时开始的时间戳
-------------------------------------------------------------------------*/
void CheckTimeOut()
{
	int i;

	for (i = 0; i < MAX_WATCH_TASK_ID; ++i)
	{
		if (task_info[i].WatchEn == 1)
		{
			printf("i=%d,NotifyCount=%d\n",i,task_info[i].NotifyCount);
			if (task_info[i].NotifyCount < 0)
			{
				syslog(LOG_ERR, "[Watch Daemon] Task halt, ID = %d, TaskName = %s, call reboot\n", i, task_info[i].TaskName);
#ifdef DEBUG
				printf("[Watch Daemon] Task halt, ID = %d, TaskName = %s, call reboot\n", i, task_info[i].TaskName);
#endif
				write_disk_error("[Watch Daemon] Task halt, ID = %d, TaskName = %s, call reboot\n", i, task_info[i].TaskName);
				mysystem("sync"); //第一次调用，将内存中的基础数据写入磁盘
				mysystem("sync"); //第二次调用，确保第一次中可能遗漏的、较晚到达的元数据也被写入
				sleep(1);
				system("reboot -f");

				rebootFlag = 1;
				exit(0);
			}
#ifdef DEBUG
			//printf("task_info[%d].NotifyCount  = %d \n", i, task_info[i].NotifyCount);
#endif
			//任务计数
			task_info[i].NotifyCount -= CHECK_INTERVAL;
		}
	}

#ifdef DEBUG
	//printf("active_thread_number  = %d \n", active_thread_number);
#endif

	if(0 == programStart)
	{
		startTime += 5;
		if(startTime >= 120)
		{
			printf("*************start timeout !*************\n");
			system("reboot");
			rebootFlag = 1;
			exit(0);
		}
	}

	if(0 == rebootFlag)
		feed_dog();

}

void msg_proc(T_MSGRBUF * pMsg)
{

#ifdef DEBUG
printf("TaskID %d Msg %d TimeSpace %d\n",pMsg->m_nTaskID,pMsg->m_nMsg,g_nTimeSpace[pMsg->m_nTaskID]);
#endif
	if (10 != pMsg->m_nType)
	{
		//WATCH_LOG ("[Watch Daemon] err, Type = %d\n", pMsg->m_nType);
		syslog(LOG_ERR, "[Watch Daemon] err, Type = %d\n", pMsg->m_nType);
		return;
	}

	if ((pMsg->m_nTaskID < 0) || (pMsg->m_nTaskID >= MAX_WATCH_TASK_ID))
	{
		//WATCH_LOG ("[Watch Daemon] err, TaskID = %d\n", pMsg->m_nTaskID);
		syslog(LOG_ERR, "[Watch Daemon] err, TaskID = %d\n", pMsg->m_nTaskID);
		return;
	}

	//1 通知消息
	if (3 == pMsg->m_nMsg)
	{
		//g_nTimeNotify[pMsg->m_nTaskID] = run_times;
		//g_nTimeNotify[pMsg->m_nTaskID] ++;
		task_info[pMsg->m_nTaskID].NotifyCount = g_nTimeSpace[pMsg->m_nTaskID];
		//WATCH_LOG ("[Watch Daemon] notify, TaskID = %d\n", pMsg->m_nTaskID);
		return;
	}

	//1 添加任务
	if (1 == pMsg->m_nMsg)
	{
#if 0
		if (0 >= pMsg->m_nTimeSpace)
		{
			WATCH_LOG("[Watch Daemon] err, Msg = %d, TimeSpace = %d\n", pMsg->m_nMsg, pMsg->m_nTimeSpace);
			return;
		}
#endif
		strncpy(task_info[pMsg->m_nTaskID].TaskName, pMsg->m_szTaskName, sizeof(task_info[pMsg->m_nTaskID].TaskName));
		active_thread_number++;
		task_info[pMsg->m_nTaskID].WatchEn = 1;
		g_nTimeSpace[pMsg->m_nTaskID] = pMsg->m_nTimeSpace;
		task_info[pMsg->m_nTaskID].NotifyCount = g_nTimeSpace[pMsg->m_nTaskID];
		//g_nTimeNotify[pMsg->m_nTaskID] = run_times;
		//strncpy(g_szTaskName[pMsg->m_nTaskID], pMsg->m_szTaskName, sizeof(g_szTaskName[pMsg->m_nTaskID]));
		WATCH_LOG("[Watch Daemon] add, TaskID = %d, TimeSpace = %d, pMsg->m_szTaskName = %s\n", pMsg->m_nTaskID, pMsg->m_nTimeSpace,
				  pMsg->m_szTaskName);
		programStart = 1;//程序启动
		return;
	}

	//1 删除任务
	if (2 == pMsg->m_nMsg)
	{
		task_info[pMsg->m_nTaskID].WatchEn = 0;
		memset(&task_info[pMsg->m_nTaskID], 0, sizeof (TASK_INFO));
		active_thread_number--;
		g_nTimeSpace[pMsg->m_nTaskID] = 0;
		WATCH_LOG("[Watch Daemon] remove, TaskID = %d\n", pMsg->m_nTaskID);
		startTime = 0;//开始倒计时
		programStart = 0;//程序结束
		return;
	}

	//WATCH_LOG("[Watch Daemon] err, Msg = %d\n", pMsg->m_nMsg);
}

int init_timer()
{

	struct itimerval tick;

	memset(&tick, 0, sizeof (struct itimerval));

	tick.it_value.tv_sec = CHECK_INTERVAL;
	tick.it_interval.tv_sec = CHECK_INTERVAL;

	signal(SIGALRM, (void *) CheckTimeOut);

	setitimer(ITIMER_REAL, &tick, NULL);

	return 0;

}

int find_pid_by_name( char* pidName)
{
	DIR *dir;
	struct dirent *next;
	int pid=0;

	mysystem("sync");
	dir = opendir("/proc");
	if (!dir)
		printf("Cannot open /proc");

	while ((next = readdir(dir)) != NULL) 
	{
		FILE *status;
		char filename[READ_BUF_SIZE+32];
		char buffer[READ_BUF_SIZE];
		char name[READ_BUF_SIZE];

		/* Must skip ".." since that is outside /proc */
		if (strcmp(next->d_name, "..") == 0)
			continue;

		/* If it isn't a number, we don't want it */
		if (!isdigit(*next->d_name))
			continue;

		snprintf(filename, sizeof(filename), "/proc/%s/status", next->d_name);
		if (! (status = fopen(filename, "r")) ) 
		{
			continue;
		}
		if (fgets(buffer, READ_BUF_SIZE-1, status) == NULL) 
		{
			fclose(status);
			continue;
		}
		fclose(status);

		/* Buffer should contain a string like "Name:?? binary_name" */
		sscanf(buffer, "%*s %s", name);
		if (strcmp(name, pidName) == 0) 
		{
			sscanf(next->d_name, "%d", &pid);
			printf("find process %s pid = %d\n", pidName, pid);
		}
	}
	closedir(dir);

	return pid;
}

// 描述:  执行命令并将结果存储到result字符串中 
// 参数:  cmd表示要执行的命令
// 返回值:成功返回0，失败返回-1
static int ShellCmdOut(const char *cmd, char *result, int len) 
{  
	struct timeval now;
	char fileName[64] = {0};
	char command2[256] = {0};
	int ret = 0;
	FILE *fp = NULL;

	if((!cmd) || (!strlen(cmd)) || (len <= 0))
	{
		return -1;
	}

	gettimeofday(&now, NULL);

	snprintf(fileName, sizeof(fileName), "/tmp/.ShellCmdOut%lld", now.tv_usec);

	memset(result, 0, len); 
	snprintf(command2, sizeof(command2), "%s > %s", cmd, fileName);

	mysystem(command2);
	if ((fp = fopen(fileName, "r")) == NULL)
	{
		perror("fopen");
		ret = -1;
	}
	else
	{
		fread(result, sizeof(char), len, fp);
		
		if(fclose(fp))
		{
			perror("fclose");
			ret = -1;
		}
	}
	memset(command2, 0, sizeof(command2));
	snprintf(command2, sizeof(command2), "rm -rf %s", fileName);
	mysystem(command2);
	
	return ret; 							   
}

#ifdef __MODULE_4G_SUPPORT__
/*@ preif 在命令command的输出中寻找字符串string
 *@ param [in] command 要执行的命令
 *@ param [in] string  要寻找的字符串(忽略大小写)
 *@ return 0 不存在该字符串
 *@        1 存在该字符串
 *@       -1 出错
 *@ note  忽略字符串的大小写，如果不想忽略，则可以用strstr代替strcasestr
 *@ 	      使用strcasestr的时候，要使用宏_GNU_SOURCE，否则会出现警告(不影响使用)
 *@       可以拓展成命令行字符串的定位
 */
static int IsStringInCommand(const char *command, const char *string)
{	
	int iRet = 0;
	char *point = NULL;
	FILE *fp = NULL;
	char buf[1024] = {0};
	struct timeval now;
	char fileName[64] = {0};
	char command2[256] = {0};

	gettimeofday(&now, NULL);

	snprintf(fileName, sizeof(fileName), "/tmp/.command%lld", now.tv_usec);

	snprintf(command2, sizeof(command2), "%s > %s", command, fileName);
	mysystem(command2);
	/* 将获取到的信息放到缓冲区buf中 */
	if ((fp = fopen(fileName, "r")) == NULL)
	{
		perror("fopen");
		memset(command2, 0, sizeof(command2));
		snprintf(command2, sizeof(command2), "rm -rf %s", fileName);
		mysystem(command2);
		return -1;
	}
	
	while(fgets(buf, sizeof(buf), fp) != NULL)
	{
		if (buf[strlen(buf)-1] == '\n') 
		{
			buf[strlen(buf)-1] =  '\0'; 
		}

		point = strcasestr(buf, string);
		if (NULL == point)
		{
			iRet = 0;
			continue;
		}
		else
		{
			iRet = 1;
			break;
		}
	}
	if (-1 == fclose(fp))
	{
		perror("fclose");
		iRet =	-1;
	}
	memset(command2, 0, sizeof(command2));
	snprintf(command2, sizeof(command2), "rm -rf %s", fileName);
	mysystem(command2);
	
	return iRet;
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

	MODULE4G_ERR("parse len:%d  str: %s\n",msg_len,out_msg);
	
	return 1;
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
				ParaAccessUnLock();

				MODULE4G_ERR("len = %d sVersion = %s \n",strlen(s_4GHandle.sVersion),s_4GHandle.sVersion);
				break;
			}
		}
	}

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

/*@breif Module4GIsOnline 4G模组是否存在
 *@       存在返回0
 *@       不存在返回-1      
 */
static int Module4GIsOnline()
{
	int ret = -1;
	if((1 == IsStringInCommand("lsusb", MEIG_MODULE_USBID)) && (0 == access(TTY_DEVICE,F_OK)))
	{
		s_ModuleType = QUECTEL_EC800;
		snprintf(szTtyDevice,sizeof(szTtyDevice),TTY_DEVICE);
		ret = 0;
	}

	if((1 == IsStringInCommand("lsusb", ZX800_MODULE_USBID)) && (0 == access(TTY_DEVICE,F_OK)))
	{
		s_ModuleType = ZHONGYUN_ZX800;
		snprintf(szTtyDevice,sizeof(szTtyDevice),TTY_DEVICE);
		ret = 0;
	}

	if((1 == IsStringInCommand("lsusb", ZX1602_MODULE_USBID)) && (0 == access(TTY_DEVICE1,F_OK)))
	{
		s_ModuleType = ZHONGYUN_ZX1602;
		snprintf(szTtyDevice,sizeof(szTtyDevice),TTY_DEVICE1);
		ret = 0;
	}
	
	
	if((1 == IsStringInCommand("lsusb", EG800QEU_MODULE_USBID)) && (0 == access(EG800QEU_TTY_DEVICE,F_OK)))
	{
		s_ModuleType = QUECTEL_EG800;
		snprintf(szTtyDevice,sizeof(szTtyDevice),EG800QEU_TTY_DEVICE);
		ret = 0;
	}
	if((1 == IsStringInCommand("lsusb", ZX800HG_MODULE_USBID)) && (0 == access(TTY_DEVICE_H,F_OK)))
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
	return ret;
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
        MODULE4G_SYS("Fail to open serial port %s!\n", ttyName);
    }
    else
    {
        MODULE4G_INF("Open serial port %s ok!\n", ttyName);
    }

    return tty_fd;
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
        MODULE4G_SYS("Fail to called tcgetattr\n");  
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
        MODULE4G_SYS("Fail to called tcsetattr!\n");
        return -1;  
    }

    return 0;  
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
        close(tty_fd);
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
		
		if ((rece_len = RecvData(s_4GHandle.iSerial_fd, recv_buf, sizeof(recv_buf))) > 0)
        {
			MODULE4G_INF("AT receive: %s\n",recv_buf);
            if (strcasestr(recv_buf, AT_RESP_OK_STR))
            {              
            	//根据不同指令,解析返回数据
				AtReceiveHandler(at_cmd,recv_buf,rece_len);

                return 0;
            }
        }
		// 中云信安模组 复位指令只接收一次
		else if((ZHONGYUN_ZX800  == s_ModuleType || ZHONGYUN_ZX1602  == s_ModuleType) && (strcasestr(at_cmd, AI_RESET_MODULE)) && (rece_len <=0))
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

/*@breif SwitchSubState 切换子状态
 *@param [In] state 状态
 *@
 *@       
 */
static void SwitchSubState(int state)
{	
	s_4GHandle.uSubState = state;
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
    MODULE4G_DBG("EC25 4G Module init fail!\n");
    return -1;
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

    // 获取模组硬件版本号
    /*if (-1 == GetModuleHardWareVersion())
    {
        return; 
    }*/

	printf("[version] start to check version len: %d \n", strlen(ModuleVersion));
	
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


/*@breif GetUpdateFile 获取升级文件名
 *@param 
 *@out filename 文件名
 *@return 0 成功
 *@       -1 失败
 */
static int GetUpdateFile(char *filename, int len)
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
					|| (0 == strncmp(ptr->d_name,"800SGFFRI30G",strlen("800SGFFRI30G")))
					|| (0 == strncmp(ptr->d_name, "800VGBFAI11G", strlen("800VGBFAI11G")))
                    || (0 == strncmp(ptr->d_name, "800VGFFAI11G", strlen("800VGBFAI11G"))))
	   	    	{
	        		//找到升级文件
	            	snprintf(filename, len, "%s/%s", MODULE_UPDATE_PATH, ptr->d_name);
	            	ret = 0;
	            }
	        }
			else if (ZHONGYUN_ZX800HG == s_ModuleType)
			{
				if (0 == strncmp(ptr->d_name,"800HGBFFI11G",strlen("800HGBFFI11G"))
                    || (0 == strncmp(ptr->d_name,"800HGBFEI11G",strlen("800HGBFEI11G")))
                    || (0 == strncmp(ptr->d_name,"800HGFFXI11G",strlen("800HGFFXI11G")))
                    || (0 == strncmp(ptr->d_name,"800HGBFXI11G",strlen("800HGBFXI11G")))
                    )
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
static int CheckUpdateFileMd5(char *updatefile, char *md5file)
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
	else if (ZHONGYUN_ZX1602 == s_ModuleType || ZHONGYUN_ZX800HG == s_ModuleType)
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

	// 等待模组重启完毕
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
	MODULE4G_INF("update file len: %d  sleep_time: %lld\n", total_len, sleep_time);

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

		if (ZHONGYUN_ZX1602 != s_ModuleType
			&& ZHONGYUN_ZX800HG != s_ModuleType)
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
	if (ZHONGYUN_ZX1602 == s_ModuleType
		||ZHONGYUN_ZX800HG == s_ModuleType)
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
		if (ZHONGYUN_ZX1602 == s_ModuleType
			||ZHONGYUN_ZX800HG == s_ModuleType)
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
	char update_file[512] = {0};

	//获取升级文件名
	if (-1 == GetUpdateFile(update_file, sizeof(update_file)-1))
	{
		goto Failed;
	}

	//检测升级文件是否被破坏，通过md5值
	if (1 != CheckUpdateFileMd5(update_file, MODULE_UPDATE_FILE_MD5))
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

    // 获取模组硬件版本号
    /*if (-1 == GetModuleHardWareVersion())
    {
        return; 
    }*/

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
	char update_file[512] = {0};

	MODULE4G_INF("start to get filename !\n"); 

	//获取升级文件名
	if (-1 == GetUpdateFile(update_file, sizeof(update_file)))
	{
		goto Failed;
	}

	//下发升级文件
	if (-1 == UpdateSendProcess(update_file, s_4GHandle.iSerial_fd))
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
			mysystem("savetime");
			mysystem("reboot");
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
			if(s_ModuleType != ZHONGYUN_ZX800HG)
			{
				if (0 == access(check_tty,F_OK))
				{
					//结点生成不对，重启设备
					memset(cmd,0,sizeof(cmd));
					snprintf(cmd,sizeof(cmd)-1,"touch %s",UPDATE_RESET_FLAG);
					mysystem(cmd);
					
					MODULE4G_ERR("module has %s\n",check_tty);
					mysystem("savetime");
					mysystem("reboot");
				}
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
	if(s_ModuleType != ZHONGYUN_ZX800HG)
	{
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
                mysystem("touch /mnt/mtd/4gUpgSucc");
				break;
			}
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
				mysystem("savetime");
				mysystem("reboot");
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

void update_4G()
{
    memset(&s_4GHandle, 0, sizeof(s_4GHandle));
    s_4GHandle.iSerial_fd = -1;
    s_4GHandle.nUpdate4GFlag = 1;
    pthread_mutex_init(&s_4GHandle.MutexParaAccess, NULL);
    pthread_mutex_init(&s_4GHandle.MutexProductDataAccess, NULL);
    do 
    {
        Update4GStateHandler();
        GprsSleep(1);
    } while (1 == s_4GHandle.nUpdate4GFlag);
    
    return;
}

static int read_md5_file(const char *md5file, char *md5, char *filename)
{
    FILE *fp = fopen(md5file, "r");
    if (!fp)
    {
        perror("fopen");
        return -1;
    }

    if (fscanf(fp, "%32s %255s", md5, filename) != 2)
    {
        fclose(fp);
        return -1;
    }

    fclose(fp);

    printf("md5_txt  = [%s]\n", md5);
    printf("filename = [%s]\n", filename);
    return 0;
}

static int get_file_md5(const char *filepath, char *md5)
{
    char cmd[512] = {0};
    char buf[128] = {0};
    FILE *fp;

    snprintf(cmd, sizeof(cmd), "md5sum \"%s\"", filepath);

    fp = popen(cmd, "r");
    if (!fp)
    {
        perror("popen");
        return -1;
    }

    if (!fgets(buf, sizeof(buf), fp))
    {
        pclose(fp);
        return -1;
    }

    pclose(fp);

    if (sscanf(buf, "%32s", md5) != 1)
        return -1;

    return 0;
}

static int copy_file(const char *src, const char *dst_tmp)
{
    int fd_src, fd_dst;
    ssize_t rlen, wlen;
    char buf[4096];

    fd_src = open(src, O_RDONLY);
    if (fd_src < 0)
    {
        perror("open src");
        return -1;
    }

    fd_dst = open(dst_tmp, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_dst < 0)
    {
        perror("open dst");
        close(fd_src);
        return -1;
    }

    while ((rlen = read(fd_src, buf, sizeof(buf))) > 0)
    {
        char *p = buf;
        ssize_t left = rlen;

        while (left > 0)
        {
            wlen = write(fd_dst, p, left);
            if (wlen <= 0)
            {
                perror("write");
                close(fd_src);
                close(fd_dst);
                return -1;
            }
            left -= wlen;
            p += wlen;
        }
    }

    fsync(fd_dst);
    close(fd_src);
    close(fd_dst);
    return 0;
}

/* 拷贝 + rename */
static int copy_with_rename(const char *src, const char *dst)
{
    char tmp[520];

    snprintf(tmp, sizeof(tmp), "%s.tmp", dst);

    if (copy_file(src, tmp) != 0)
        return -1;

    unlink(dst);

    if (rename(tmp, dst) != 0)
    {
        perror("rename");
        unlink(tmp);
        return -1;
    }

    return 0;
}

int check_copy_and_verify(const char *md5file, const char *src_dir, const char *dst_dir)
{
    char expect_md5[33] = {0};
    char src_md5[33] = {0};
    char dst_md5[33] = {0};
    char filename[256] = {0};

    char src_bin[512];
    char dst_bin[512];

    if (read_md5_file(md5file, expect_md5, filename) != 0)
        return -1;

    snprintf(src_bin, sizeof(src_bin), "%s/%s", src_dir, filename);
    if (strncmp(filename, "800SG", 5) == 0 || strncmp(filename, "800VG", 5) == 0)
    {
        snprintf(dst_bin, sizeof(dst_bin), "%s/%s", dst_dir, "ZX800SG-Upgrade.bin");
    }
    else if(strncmp(filename, "800HG", 5) == 0)
    {
        snprintf(dst_bin, sizeof(dst_bin), "%s/%s", dst_dir, "ZX800HG-Upgrade.bin");
    }
    else
   	{
		return -1;
	}

    /* 校验源文件 MD5 */
    if (get_file_md5(src_bin, src_md5) != 0)
        return -1;

    printf("src_md5 = [%s]\n", src_md5);

    if (strcmp(expect_md5, src_md5) != 0)
    {
        printf("source md5 check failed\n");
        return -1;
    }

    /* 拷贝 bin */
    if (copy_with_rename(src_bin, dst_bin) != 0)
    {
        printf("copy bin failed\n");
        return -1;
    }

    /* 再校验目标 bin MD5 */
    if (get_file_md5(dst_bin, dst_md5) != 0)
        return -1;

    printf("dst_md5 = [%s]\n", dst_md5);

    if (strcmp(expect_md5, dst_md5) != 0)
    {
        printf("destination md5 check failed\n");
        return -1;
    }

    printf("All copy and verify success\n");
    return 0;
}


#endif

//return 0:进程已杀掉；1:进程还未退出; 2:杀的次数超过限制
int kill_app(char *processName)
{
	int pid=0,ret;
	char command[64];
	char findcmd[64];
	char cmdvalue[32];
	static int count = 0;

	pid = find_pid_by_name(processName);

	if((0 == processPid) && (0 == pid))
	{
		memset(findcmd,0,sizeof(findcmd));
		memset(cmdvalue,0,sizeof(cmdvalue));
		snprintf(findcmd, sizeof(findcmd),"ps | grep -v grep | grep %s | awk '{print $1}'",processName);
		ShellCmdOut(findcmd, cmdvalue, sizeof(cmdvalue));
		if(strlen(cmdvalue) == 0)
			{
				printf("NO process %s ,But we have get g_dspAddr\n",processName);
				return 0;
			}
	
		printf("no find process %s!\n", processName);
		return -1;
	}
	else if((0 != processPid) && (0 == pid))
	{
		printf("process %s(%d) is killed!\n", processName, processPid);
		return 0;
	}
	else if(0 != pid)
	{
		if (processPid)
			printf("process %s(%d) is alive!\n", processName, processPid);
		ret = 1;
	}
		
	processPid = pid;

	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), "kill -9 %d", processPid);
	mysystem(command);
	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), "killall %s", APP_NAME);
	mysystem(command);

	if(++count > 10)
	{
		printf("kill process %s(%d) timeout!\n", processName, processPid);
		processPid = 0;
		count = 0;
		ret = 2;
	}
	return ret;
}

void kill_Process()
{
	// 升级前杀掉进程，避免反初始化后，进程有其他访问导致崩溃
	// 没有跑的进程是否需要杀？
	kill_app(PROCESS_NAME_BOA);
	kill_app(PROCESS_NAME_MONITOR);
	kill_app(PROCESS_NAME_WPA);
	kill_app(PROCESS_NAME_UDHCPC);
	kill_app(PROCESS_NAME_HOSTAPD);
	kill_app(PROCESS_NAME_LBH);

	return;
}

/* 判断Mac地址是否合法, return 1:合法；return 0:非法 */
int MacIsOK(char *mac)
{
    int iRet     =0;
    int     i    =0;
    int InVailed =0;
    unsigned int ucMac[6];
 
    iRet = sscanf(mac,"%02x:%02x:%02x:%02x:%02x:%02x",  \
            &ucMac[0],&ucMac[1],&ucMac[2],&ucMac[3],&ucMac[4],&ucMac[5]);
    for(i=0; i<6; i++)
    {
        if(ucMac[i]<0x0||ucMac[i]>0xff)
        {
            InVailed =1;
            break;
        }
        InVailed =0;
    }
 
    if(InVailed || ucMac[0]%2==1|| iRet!=6)
    {
        iRet =0;
    }else{
        iRet =1;
    }
 
    return iRet;
}

/*
*监测MAC地址是否为默认设置的，如果不是则需要保存此MAC地址,
*因为烧录ptb会清除所有环境变量。
*/
static int saveMac()
{
	char command[64];
	char buf[256];
	FILE *fp;
	int ret;
	
	memset(command, 0, sizeof(command));

	if ((!access("/mnt/mtd/config/authmac", 0)) && 
		(!access("/mnt/mtd/config/authorization.cfg", 0)))
	{
		snprintf(command, sizeof(command), "cat /mnt/mtd/config/authmac > /mnt/mtd/config/savemac");
	}
	else if (!access("/mnt/mtd/config/randmac",0))
	{
		snprintf(command, sizeof(command), "cat /mnt/mtd/config/randmac > /mnt/mtd/config/savemac");
	}
	else
	{
		snprintf(command, sizeof(command), "echo 00:00:00:00:00:00 > /mnt/mtd/config/savemac");
	}

	mysystem(command);
	mysystem("sync");

	fp = fopen("/mnt/mtd/config/savemac", "r");
	if(fp == NULL)
	{
		printf("saveMac---fopen %s faile!\n", "/mnt/mtd/config/savemac");
		return 1;
	}

	memset(buf, 0, sizeof(buf));
	fgets(buf, 255, fp);

	ret = MacIsOK(buf);

	if(0 == ret)
	{
		fclose(fp);
		memset(command, 0, sizeof(command));
		snprintf(command, sizeof(command), "rm -fv /mnt/mtd/config/savemac");
		mysystem(command);
		return 0;	
	}
	else
	{
		fclose(fp);
		return 0;
	}
	
	return 0;
}

void Upgrade_Notify(int wParam, int *plParam)
{
	printf("Progress[%d]\n", wParam);

	return;
}

#define SENSOR_NAME_PATH			"/tmp/.SensorName"
static int GetSensorNameFromCfg(char *sensor_name, unsigned int sensor_name_size)
{
	FILE *fp = NULL;
	
	if (NULL == sensor_name)
	{
		printf("NULL POINTER EXCEPTION!\n");
		return -1;
	}

	fp = fopen(SENSOR_NAME_PATH, "r");
	if (NULL == fp)
	{
		printf("Fail to fopen %s\n", SENSOR_NAME_PATH);
		return -1;
	}
	
	fread(sensor_name, sizeof(char), sensor_name_size, fp);
	
	fclose(fp);

	/* 去掉结尾的回车换行 */
	if (strlen(sensor_name))
	{
		//sensor_name[strlen(sensor_name)-1] = '\0';
	}
	else
	{
		return -1;
	}

	return 0;
}

/*@breif GetDefaultBoardIDBySensor 通过sensor选取一个默认支持该sensor的BoardID
 *@param [IN]  sensor_name 需要匹配的SensorName
 *@return 如果找到了BoardID，返回该BoardID
 *@      没有找到返回0
 *@note  默认版型用于在授权出错的时候，返回一个默认支持的作为系统的BoardId
 */
static unsigned long GetDefaultBoardIDBySensor(const char *sensor_name)
{
    if (NULL == sensor_name)
    {
        printf("NULL POINTER EXCEPTION!\n");
        return 0;
    }

    int i = 0;

    for (i=0; i<gstuBoardInfoSize; i++)
    {
        if (0 == strncmp(sensor_name, (const char *)gstuBoardInfo[i].szSensorName, sizeof(gstuBoardInfo[i].szSensorName)))
        {
            return gstuBoardInfo[i].dwBoardId;
        }
    }    

    return 0;
}

static int GetBoardTypeFromCfg(char *buf, unsigned int bufsize)
{
    if (NULL == buf)
    {
        printf("NULL POINTER EXCEPTION!\n");
        return -1;
    }
    memset(buf, 0, bufsize);

    struct stat fileStat;

    if (stat(DEVICE_INFO_FILE_CFG, &fileStat))
    {
        return -1;
    }

    FILE *fp = NULL;
    char tmp[128] = {0};

    fp = fopen(DEVICE_INFO_FILE_CFG, "r");
    if (NULL == fp)
    {
        printf("Fail to fopen %s\n", DEVICE_INFO_FILE_CFG);
        return -1;
    }
    fread(tmp, sizeof(char), sizeof(tmp), fp);
    fclose(fp);
    fp = NULL;

    if (strlen(tmp))
    {
        sscanf(tmp, "TYPE_%s", buf);
    }
    else
    {
        return -1;
    }

    return 0;
}

/*@breif GetDefaultBoardIDBySensor 获取取一个默认支持该sensor的BoardID
 *@return 如果找到了BoardID，返回该BoardID
 *@      没有找到返回0
 *@note  默认版型用于在授权出错的时候，返回一个默认支持的作为系统的BoardId
 */
unsigned long GetDefaultBoardID(void)
{
    int rc = 0;
    unsigned long boardID = 0;
    char szSensorName[32] = {0};
	char szBoardType[64] = {0};
	// 1. 根据sensor获取默认ID
    rc = GetSensorNameFromCfg(szSensorName, sizeof(szSensorName));
    if (0 == rc)
    {
        boardID = GetDefaultBoardIDBySensor(szSensorName);
    }

    // 2. 存在版型信息, 获取指定版型boardID
    rc = GetBoardTypeFromCfg(szBoardType, sizeof(szBoardType));
    if (0 == rc)
    {
        int i = 0;

        for (i=0; i<gstuBoardInfoSize; i++)
        {
            if (0 == strncasecmp(szBoardType, (const char *)gstuBoardInfo[i].szBoardType, sizeof(gstuBoardInfo[i].szBoardType)))
            {
                boardID = gstuBoardInfo[i].dwBoardId;
                break;
            }
        }
    }
    return boardID;
}

int update(char *_path)
{
	int i = 0;
	int nBoardID = 0;
	char szDevType[64] = {0};
	int nRet = 0;
	char command[64] = {0};

	UPGRADE_MEM_INFO_S stMemInfo;

	nRet = saveMac();
	if (nRet != 0)
	{
		printf("save mac error! ret = %d\n", nRet);
        
        WriteStateToFile(UPGRADE_SOFTWARE_EXC);
        sleep(5);

		return -1;
	}

	nBoardID = GetDefaultBoardID();

	//获取版型所对应的型号。GetDefaultBoardID 多个版型是同一个Sensor时，会默认获取到第一个版型。所以这里处理下

	printf("[%s] [%d] nBoardID=%#x\n", __FUNCTION__, __LINE__,nBoardID);
	for (i = 0; i < gstuBoardInfoSize; i++)
	{
		if(nBoardID == gstuBoardInfo[i].dwBoardId)
		{
			memset(szDevType, 0, sizeof(szDevType));
			snprintf(szDevType, sizeof(szDevType), "TYPE_%s", gstuBoardInfo[i].szBoardType);
			printf("[%s-%d]this board type is %s\n", __FUNCTION__, __LINE__, szDevType);
			break;
		}
	}
	
	nRet = Upgrade_Open(Upgrade_Notify, szDevType);
	if (nRet < 0)
	{
		printf("[%s] Upgrade_Open failed, nRet=%d\n", __FUNCTION__, nRet);
		WriteStateToFile(UPGRADE_SOFTWARE_EXC);
		sleep(5);
		return -1;
	}

	stMemInfo.Cnt = 1;
	stMemInfo.Mem[0].Addr = (void *)g_dspAddr;
	stMemInfo.Mem[0].Size = g_dspLen;

	nRet = Upgrade_Mem_UpdateEx(0, &stMemInfo, NULL);
	if (nRet < 0)
	{
		printf("[%s] Upgrade_Mem_UpdateEx failed, nRet=%d\n", __FUNCTION__, nRet);
		WriteStateToFile(UPGRADE_SOFTWARE_EXC);
		sleep(5);
		return -1;
	}

	Upgrade_Close(0);

	printf("update success!\n");

	//备份云升级凭证
	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), "mv %s %s", 
	    UPGRADE_PASSPORT_TMP_FILE, UPGRADE_PASSPORT_FILE);
	mysystem(command);

    WriteStateToFile(UPGRADE_COMPLETE);
    sleep(5);

	return 1;
}

/*
*如果使用的DSP内存来升级的话，就必须重新初始化遍sdk，来映射出
*DSP的地址，因为主程序被kill调后DSP内存就会释放，如果再初始化一遍
*的话主程序即使被干掉了DSP内存也不会被释放
*/
int haveATry()
{
	return 0;
}

/*根据主进程申请的MMZ内存物理地址映射至当前进程*/
void * phyMemMap(unsigned int pPhyAddr, unsigned int nSize)
{
	void * pVirAddr = NULL;
	
	pVirAddr = memmap(pPhyAddr, nSize);
	if(!pVirAddr)
	{
		printf("FH_SYS_Mmap failed, pPhyAddr = %#x, nSize = %#x\n", pPhyAddr, nSize);
		return NULL;
	}
	
	return pVirAddr;
}

int phyMemUnMap(void *pVirAddr, unsigned int nSize)
{
	int nRet = 0;
	
	nRet = memunmap(pVirAddr);
	
	return nRet;
}

void * check_update_file(void *content)
{
	int ret, i = 0;
	char command[128];
 
	while(1)
	{
		if(!access(UPDATEFILENAME_SUCC, 0)
           || !access(UPDATEFILENAME_SUCC_CLOUD_UPGRADE, 0))//如果升级包存在
		{
			/* 如果升级包存在，则需要阻塞式判断网络层是否调用Destroy进程，
             * 如果调用，则表明升级包传输完毕且文件头校验正确，否则一直等
             * 待,等待超时时间为30秒，即30秒还没有调用Destroy则退出继续
             */
            if (!access(UPDATEFILENAME_SUCC, 0))
            {
                while (i++ < 30)
                {
                    sleep(1);
                    if (!access(DESTROY_UPLOAD_OVER_FLAG, F_OK))
                    {
                        printf("Calling Destroy over! update continue!\n");
                        remove(DESTROY_UPLOAD_OVER_FLAG);
                        sleep(2);
                        break;
                    }
                }
            }
            else if (!access(UPDATEFILENAME_SUCC_CLOUD_UPGRADE, 0))
            {
                //确保云升级状态返回后再启动升级( 杀死主进程)
                sleep(CLOUD_UPGRADE_RET_DELAY);
            }

			g_ifUseDspMem = 0;
			g_dspAddr = 0;
			g_dspPhyAddr = 0;
			g_dspLen = 0;
			g_readPoint = 0;

			if (0 == access(DSP_ADDR_INFO, F_OK))
			{
				ret = haveATry();
				if(ret)
				{
					mysystem("reboot");
					rebootFlag = 1;
					exit(0);
				}
			}
			else if (0 == access(DSP_ADDR_INFO_FOR_MMZ, F_OK))
			{
			    FILE *pfile = NULL;
				char fbuf[128] = { 0 };
        		pfile = fopen(DSP_ADDR_INFO_FOR_MMZ, "r");
        		if (NULL == pfile)
        		{
        			printf("open %s faile!\n", DSP_ADDR_INFO_FOR_MMZ);
        			return NULL;
        		}
				fread(fbuf, sizeof(char), sizeof(fbuf), pfile);
				sscanf(fbuf, "%d:%d:%d", &g_dspAddr, &g_dspPhyAddr, &g_dspLen);
        		// fscanf(pfile, "%d:%d:%d", &g_dspAddr, &g_dspPhyAddr, &g_dspLen);
        		fclose(pfile);

        		printf("DSP ADDR INFO: g_dspAddr = %#x, g_dspPhyAddr = %#x, g_dspLen = %#x\n", g_dspAddr, g_dspPhyAddr, g_dspLen);
			}

		while(1)
		{
			kill_Process();
			ret = kill_app(APP_NAME);
			for (i = 0; i < MAX_WATCH_TASK_ID; ++i)
			{
				programStart = 1;//不让看门狗重启
				if (task_info[i].WatchEn == 1)
				{
					task_info[i].WatchEn = 0;
					task_info[i].NotifyCount = 30;
				}
			}
			
			if((-1 == ret) || (2 == ret))//杀不死
			{
				memset(command, 0, sizeof(command));
				snprintf(command, sizeof(command), "rm %s %s", UPDATEFILENAME_SUCC, UPDATEFILENAME_SUCC_CLOUD_UPGRADE);
				mysystem(command);
				mysystem("reboot");
				rebootFlag = 1;
				exit(0);
			}
			else
			{
				if(1 == ret)
				{
					sleep(3);
				}
				else if(0 == ret)//进程已被干掉
				{
					mysystem("sync");
					sleep(1);

					g_dspAddr = (unsigned int)(phyMemMap(g_dspPhyAddr, g_dspLen));
					if (0 == g_dspAddr || 0xffffffff == g_dspAddr)
					{
						printf("\n[%s][%d]phyAddr map faild!\n", __FUNCTION__, __LINE__);
						mysystem("reboot");
						rebootFlag = 1;
						exit(0);
					}

					g_ifUseDspMem = 1;

#if !defined(__STATIC_LINK_STDC__)  
					//升级前先将小程序拷贝到tmp下，因为升级后分区已擦除
					memset(command, 0, sizeof(command));
					snprintf(command, sizeof(command), "cp %s %s", UPDATE_PROCESS_PATH, UPDATE_PROCESS_PATH_TMP);
					mysystem(command);
#endif
					update(UPDATEFILENAME);

#ifdef __MODULE_4G_SUPPORT__
                    printf("[%s:%d] start upgrade 4g.bin!\n", __FILE__, __LINE__);
                    if (check_copy_and_verify(MODULE_UPDATE_FILE_MD5, MODULE_UPDATE_PATH, MODULE_UPDATE_PATH_BAK) != -1)
                    {
                        update_4G();
                    }
#endif

#if !defined(__STATIC_LINK_STDC__)
					// 在升级完后执行update小程序（用来强制替换当前配置）
					memset(command, 0, sizeof(command));
					snprintf(command, sizeof(command), "%s", UPDATE_PROCESS_PATH_TMP);
					mysystem(command);
#endif
					memset(command, 0, sizeof(command));
					snprintf(command, sizeof(command), "rm %s %s", UPDATEFILENAME_SUCC, UPDATEFILENAME_SUCC_CLOUD_UPGRADE);
					mysystem(command);

					if (0 == access(DSP_ADDR_INFO_FOR_MMZ, F_OK))
					{
						phyMemUnMap((void *)g_dspAddr, g_dspLen);
					}

					sleep(1);
					mysystem("reboot");
					rebootFlag = 1;
					exit(0);
				}
			}

			sleep(1);
			}
		}
		sleep(1);
	}
}

int main(int argc, char *argv[])
{
	key_t key;

	int msgid, reval;
	pthread_t tid;

	T_MSGRBUF msg_rbuf;

	if((2 == argc) && (!strcmp(argv[1], "stop")))
	{
		stop_watch_dog();
		printf("stop wdt and return !\n");
		return 0;
	}
	else
	{
		enable_watch_dog(1);
	}

	key = ftok("/linux/msgqueue/higheasy/WatchTask", 'a');
	msgid = msgget(key, IPC_CREAT | 00666);
	if (-1 == msgid)
	{
		syslog(LOG_ERR, "[Watch Daemon] msgqueue create failed, call reboot\n");
		printf("msgqueue create failed, call reboot\n");
		mysystem("reboot");
		rebootFlag = 1;
		exit(0);
	}

	syslog(LOG_INFO, "[Watch Daemon] setup ok!running..........\n");

	init_timer();
	pthread_create(&tid, NULL, check_update_file, NULL);

	while (1)
	{
		memset(&msg_rbuf, 0, sizeof (msg_rbuf));
		reval = msgrcv(msgid, &msg_rbuf, MSGBUF_LEN, 0, MSG_NOERROR);

		if (reval != -1)
		{
			msg_proc(&msg_rbuf);
		}

	}

	syslog(LOG_INFO, "[Watch Daemon] quit !\n");
	return 0;
}
