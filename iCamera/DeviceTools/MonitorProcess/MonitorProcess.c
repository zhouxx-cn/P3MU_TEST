#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <pthread.h>
#include <wchar.h>
#include <time.h>
#include <stdarg.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>


#include <sys/ioctl.h>
#include <net/route.h>
#include <netdb.h>
#include <netinet/if_ether.h>
#include <netpacket/packet.h>
#include <linux/if.h>
#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <linux/mii.h>
#include <arpa/inet.h>
#include <sys/socket.h>



#undef _GNU_SOURCE

#define DETC_ERR(format, args...) do{printf("\033[1;31m[MONITOR][%s-%d]\033[m" format, __FILE__, __LINE__, ##args);}while(0)
#define DETC_DBG(format, args...) do{printf("\033[1;32m[MONITOR][%s-%d]\033[m" format, __FILE__, __LINE__, ##args);}while(0)
#define DETC_INF(format, args...) do{printf("\033[1;35m[MONITOR][%s-%d]\033[m" format, __FILE__, __LINE__, ##args);}while(0)
#define DETC_SYS(format, args...) do{printf("\033[1;32m[MONITOR][%s-%d](%s) \033[m" format, __FILE__, __LINE__, strerror(errno), ##args);}while(0)

#define NETSERVER_HAS_START   "/tmp/.otherProtocolStarted"

/* 监听对象 */
#define LISTEN_CLOSE_WAIT     "CLOSE_WAIT"
#define LISTEN_ONVIF_BLOCK    "STREAM     CONNECTING         0 /var/run/onvifd.sck"
#define LISTEN_ONVIF_3702     "3702"
#define LISTEN_BOA_PORT       "boa"
#define LISTEN_5555_BLOCK     "0.0.0.0:5555"

#define LISTEN_WLAN_BLOCK     "ps | grep -v grep | grep ifconfig"

#define REBOOT_IPC_DEVICE     "reboot"

#define REBOOT_BOA_SERVICE    "killall -9 boa; /mnt/app/bin/boa -c /home/http -f /mnt/mtd/etc/conf.d/boa/boa.conf"

/* 内核异常检测 */
#define KERNEL_PANIC          "Kernel panic"         // 检测内核恐慌
#define KERNEL_OOPS           "Internal error: Oops" // 检测内核指针异常

/* SD卡异常检测(针对SD卡异常的操作，需要和系统启动检测程序checkSDCard以及中间层一起修改) */
#ifdef __USE_EMMC__
#define MMCBLK_ERR            "mmcblk1: error"               // 检测SD卡好坏
#define MMC1_ERROR            "mmc1: error"                  // 检测SD卡好坏
#define FAT_FS_ERR            "FAT-fs (mmcblk1p1): error"    // FAT FS系统被破坏
#else
#define MMCBLK0_ERR            "mmcblk0: error"               // 检测SD卡好坏
#define MMCBLK1_ERR            "mmcblk1: error"               // 检测SD卡好坏
#define MMC0_ERROR            "mmc0: error"                  // 检测SD卡好坏
#define MMC1_ERROR            "mmc1: error"                  // 检测SD卡好坏
#define FAT_FS_ERR            "FAT-fs (mmcblk0p1): error"    // FAT FS系统被破坏
#define FAT_FS_READ_ERR       "FAT-fs (mmcblk0p1): FAT read failed"    // FAT FS系统被破坏
#endif

/* 检测到SD卡异常需要执行的操作 */
#define SD_IS_DAMAGED         "echo 7 > /tmp/.sd_status"  // 磁盘错误
#define FAT_FS_EXP            "echo 8 > /tmp/.sd_status"  // FAT文件系统异常

/* dmesg 检测log信息 */
#define DMESG_STATUS_LOG      "/tmp/.dmesg_status.log"

/* 检测到内核出错则强制重启 */
#define FORCE_DEV_REBOOT      "killall -9 myWatchDogProcess; reboot"

#define WPA_LOG_FILE          "/tmp/wpa.log"
#define WPA_LOG_FILE_MAX_SIZE "65536"
#define MAX_LOG_LINE          (512)

typedef enum{
    MONITOR_NOT_EXIST = 0,
    MONITOR_EXIST     = 1,
}eNetStatMonitorType;

typedef enum{
    SdCardRunCheck_DiskReadOnly   = 6,  // SD卡只读(要格式化)
    SdCardRunCheck_DiskIsError    = 7,  // SD卡异常(换卡)
    SdCardRunCheck_FatfsIsError   = 8,  // FAT系统错误
    SdCardRunCheck_DiskIsWritting = 9,  // SD卡正在写
    SdCardRunCheck_DiskIsIdle     = 10, // SD卡空闲
}eRunSdCardStatus;

typedef enum {
    SD_NO_NODE = 0,      // 无节点无卡
    SD_CARD_ERR,         // 卡错误无卡
    CARD_MOUNT_ERR,      // 挂载失败无卡
    FIRST_DSG_EXP,       // 第一次dmesg异常
    CARD_RW_EXP,         // 卡读写异常
    FILE_MD5_EXP,        // 文件验证异常
    SEC_DSG_EXP,         // 第二次dmesg异常
    INIT_FORMAT,         // 初始化时格式化
    MIDDLE_FORMAT,       // 系统运行中格式化
    CLEAN_ALL_CARD,      // 清空SD卡
    SD_CARD_NORMAL,      // 卡正常
    SD_ADD_PART_ERR,     // 创建分区失败
    MIDDLE_DSG_EXP,      // 中间层dmesg检测异常
    MIDDLE_FILE_MD5_EXP, // 中间层文件验证异常
    KERNEL_PANIC_EXP,    // dmesg监测到kernel panic异常
    KERNEL_OOPS_EXP,     // dmesg监测到kernel oops异常
    SD_CARD_ERR_NUM      // 错误总数
}eSdErrorNo;

static const char *gSdErrorMsg[SD_CARD_ERR_NUM] = {
    "[Monitor] check SD Card no node\n",
    "[Monitor] check SD Card error no card\n",
    "[Monitor] check SD Card mount fail no card\n",
    "[Monitor] check SD Card error in first dmesg\n",
    "[Monitor] check SD Card read and write exception\n",
    "[Monitor] check SD Card file md5sum check exception\n",
    "[Monitor] check SD Card error in second dmesg\n",
    "[Monitor] check SD Card formated in system boot\n",
    "[Monitor] check SD Card formated in ambarella_test\n",
    "[Monitor] check SD Card has been clean up\n",
    "[Monitor] check SD Card is normal\n",
    "[Monitor] check SD Card add partition fail\n",
    "[Monitor] check SD Card error in middle dmesg\n",
    "[Monitor] check SD Card file md5sum check exception in middle\n",
    "[Monitor] check Kernel panic error in dmesg\n",
    "[Monitor] check Kernel internal oops error in dmesg\n"
};


static const char *IFNAME = "eth0";

static int mysystem(const char* cmdstring)
{
	if (cmdstring == NULL)
	{
		return 1;
    }

	pid_t pid;
	int status;

	if ( ( pid=vfork() ) < 0 )
	{
		printf("vfork failed\n");
		status = -1;
	}
	else if (pid==0)
	{
		execl("/bin/sh", "sh", "-c", cmdstring, (char*)0);
		_exit(127);
	}
	else
	{
		while(waitpid(pid, &status, 0)<0)
		{
			if(errno!=EINTR)
			{
				status=-1;
				break;
			}
		}
	}

	return status;
}

/*@brief      FindAfterStringEndline 从指定文件里面找出指定字符串后面的内容，限定为一行
 *@param[in]  filepath     欲查找的文件
 *@param[in]  indexString  指定的字符串
 *@param[out] resultString 获取到的结果字符串
 *@return    字符串获取成功返回1，不存在则返回0，失败返回-1
 */
static int FindAfterStringEndline(const char *filepath, const char *indexString, char *resultString)
{
	if (NULL==filepath || NULL==indexString || NULL==resultString)
	{
		printf("NULL POINTER EXCEPTION\n");
		return -1;
	}

	char buff[1024] = {0};
	FILE *fp = NULL;
	char *point = NULL;
	int ret = -1;

	fp = fopen(filepath, "r");
	if(NULL == fp)
	{
		fprintf(stderr, "Fail to fopen %s: %s\n", filepath, strerror(errno));
		return -1;
	}

	while (!feof(fp))
	{
		memset(buff, 0, sizeof (buff));
		fgets(buff, sizeof (buff), fp);
		if (buff[0] != '#')   // 忽略注释
		{
			point = strstr(buff, indexString);
			if (point)
			{
				point += strlen(indexString);
				sprintf(resultString, "%s", point);
				resultString[strlen(resultString)-1] = '\0'; // 去掉后面的'\n'
				ret = 1;
				break;
			}
		}
		ret = 0;
	}

	fclose(fp);
	fp = NULL;

	return ret;
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

	if(!cmd || !strlen(cmd) || len<=0)	
		return -1;

	gettimeofday(&now, NULL);

	bzero(fileName, sizeof(fileName));
	sprintf(fileName, "/tmp/.ShellCmdOut%ld", now.tv_usec);

	memset(result, 0, len); 
	bzero(command2, sizeof(command2));
	sprintf(command2, "%s > %s", cmd, fileName);

	mysystem(command2);
	if ((fp = fopen(fileName, "r")) == NULL)
	{
		perror("fopen");
		ret = -1;
	}
	else
	{
		fread(result, sizeof(char), len-1, fp);
		
		if(fclose(fp))
		{
			perror("fclose");
			ret = -1;
		}
	}
	
	bzero(command2, sizeof(command2));
	sprintf(command2, "rm -rf %s", fileName);
	mysystem(command2);
	
	return ret; 							   
}

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
	FILE *fp    = NULL;
	char buf[1024] = {0};

	/* 将获取到的信息放到缓冲区buf中 */
	if ((fp = popen(command, "r")) == NULL)
	{
		perror("popen");
		return -1;
	}

	while(fgets(buf, sizeof(buf), fp) != NULL)
	{
		if (buf[strlen(buf)-1] == '\n')
		{
			buf[strlen(buf)-1] =  '\0'; 
        }

		point = strcasestr(buf, string);
		if (point == NULL)
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
	if (pclose(fp) == -1)
	{
		perror("pclose");
		return -1;
	}
	return iRet;
}


/*@brief     GetNetlinkStatus 获取网卡if_name的连接状态
 *@param[in] if_name     网卡名字eth0, wlan0或ra0
 *@return    -1 -- error, details can check errno
 *            1 -- interface link up
 *            0 -- interface link down
 *@note      如果是在虚拟机上测试，需要root权限
 *           可以通过getuid()!=0判断非root用户
 */
static int GetNetlinkStatus(const char *if_name)  
{  
	static int valid = 0;
	static int lastStatus = 1;

	valid = !valid;

	if(0 == valid)//每两次调用做一次
	{
		return lastStatus;
	}
		
    if (NULL == if_name)
    {
        printf("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    int skfd = -1;
    struct ifreq ifr;
    struct ethtool_value edata;
	int status = 0;
	int i;
	struct mii_ioctl_data *mii = NULL;
	int mii_val;

	//检测网络是否断开
	if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		DETC_ERR(" socket on eth0 failed: %s\n", strerror(errno));
	}
	else
	{
		strncpy(ifr.ifr_name, IFNAME, IFNAMSIZ);
		edata.cmd = ETHTOOL_GLINK;

		ifr.ifr_data = (char *) &edata;
		if ((i = ioctl(skfd, SIOCETHTOOL, &ifr)) == -1)//获取当前网络连接状态
		{
			DETC_ERR("ETHTOOL_GLINK failed: %s\n", strerror(errno));

			//将继续进行 SIOCGMIIPHY
			if (ioctl(skfd, SIOCGMIIPHY, &ifr) < 0)
			{
				DETC_ERR("SIOCGMIIPHY on eth0 failed: %s\n", strerror(errno));
			}
			else
			{
				mii = (struct mii_ioctl_data *) & ifr.ifr_data;
				mii->reg_num = MII_BMSR;
				if (ioctl(skfd, SIOCGMIIREG, &ifr) < 0)
				{
					DETC_ERR("SIOCGMIIREG on eth0 failed: %s\n", strerror(errno));
				}
				mii_val = mii->val_out;

				if (!(mii_val & BMSR_LSTATUS))
				{
					status = 0;
				}
			}

		}
		else
		{
			status = (edata.data ? 1 : 0);
		}
		close(skfd);

	}
	lastStatus = status;

	return status;
}  



static int GetFileLines(const char* pFile)
{
    if (NULL == pFile)
    {
        fprintf(stderr, "NULL POINTER EXCEPTION\n");
        return -1;
    }

    FILE *fp = NULL; 
    char buf_line[512] = {0}; 
    int lines = 0;

    fp = fopen(pFile, "r");
    if(NULL == fp)
    {
        fprintf(stderr, "Fail to fopen %s: %s\n", pFile, strerror(errno));
        return 0;
    }

    while(!feof(fp))
    {
        fgets(buf_line, sizeof(buf_line), fp);
        lines ++;
    }

    fclose(fp);
    fp = NULL;

    return lines;
}

static int WriteLog(const char* pFile, const char *format, ...)
{
    if (!pFile)
    {
        printf("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    int lines = 0;
    FILE *fp = NULL;

    lines = GetFileLines(pFile);
    if (lines >= MAX_LOG_LINE)
    {
        fp = fopen(pFile, "r+");
    }
    else
    {
        fp = fopen(pFile, "a+");
    }

    if (!fp)
    {
		fprintf(stderr, "Fail to fopen %s: %s\n", pFile, strerror(errno));
		return -1;
    }

    va_list arg;
    int done = 0;

    va_start (arg, format);

	//获取当前时间段信息
	struct tm tm_log;
    time_t time_log = time(NULL);
	localtime_r(&time_log, &tm_log);
	
    fprintf(fp, "[%04d-%02d-%02d %02d:%02d:%02d] ", 
            tm_log.tm_year + 1900, 
            tm_log.tm_mon + 1, 
            tm_log.tm_mday, 
            tm_log.tm_hour, 
            tm_log.tm_min, 
            tm_log.tm_sec);

    done = vfprintf (fp, format, arg);
    va_end (arg);

    fflush(fp);
    fclose(fp);
    fp = NULL;
        
    return done;
}

/* 网络相关性能监控(主要是通过netstat命令监控) */
typedef struct tagNetStatus{
	char                 listen_key[128];  // 监听对象    
	char                 option_cmd[128];  // 时间到达之后执行的动作    
	int                  listen_freq;      // 监听频率(s)    
	int                  listen_count;     // 监听个数    
	int                  event_duration;   // 监听事件持续时间(s)
	eNetStatMonitorType  listen_type;      // 监听类型 MONITOR_NOT_EXIST:监听对象不存在则计时；
	                                       //         MONITOR_EXIST:监听对象存在则计时
}stuNetStatusInfo, *pNetStatusInfo;

typedef enum{
    eKernelMsgMonitorType_kernelPanic   = 0,  // 
    eKernelMsgMonitorType_kernelOops,  // 
    eKernelMsgMonitorType_mmcblk0Err,  // 
    eKernelMsgMonitorType_mmc0Err,  // 
    eKernelMsgMonitorType_mmcblk1Err, // 
	eKernelMsgMonitorType_mmc1Err,  // 
	eKernelMsgMonitorType_fatfsErr,  // 
	eKernelMsgMonitorType_fatfsReadErr,  //
	eKernelMsgMonitorType_num,  // 
}eKernelMsgMonitorType;

/* 内核相关性能监控(主要是通过dmesg命令监控) */
typedef struct tagKernelMsgStatus{
	eKernelMsgMonitorType monitorType; //监听类型  
	char listen_key[128];  // 监听对象    
	int  Abnormal_count;     // 异常信息的次数阈值
	char option_cmd[128];  // 时间到达之后执行的动作
}stuKernelMsgStatus, *pKernelMsgStatus;

static pthread_mutex_t g_net_status_lock = PTHREAD_MUTEX_INITIALIZER;

static stuNetStatusInfo g_netstat_status_info[] = {
    /*-------对象------------------动作------频率--个数--事件持续时间----监听类型--------*/ 
    {LISTEN_CLOSE_WAIT,  REBOOT_IPC_DEVICE,  600,   10,     1200,   MONITOR_EXIST},      // 监听CLOSE_WAIT存在10个且持续300S则重启IPC
    {LISTEN_BOA_PORT,    REBOOT_BOA_SERVICE, 600,    1,     1200,   MONITOR_NOT_EXIST},  // 监听BOA端口不存在且持续120S则重启BOA服务
    // {LISTEN_5555_BLOCK,  REBOOT_IPC_DEVICE,  300,    1,     600,    MONITOR_NOT_EXIST},  // 监听5555端口塞住的问题
    // Add what you want to netstat monitor!
};

static stuNetStatusInfo g_netstat_status_info_1[] = {
	{LISTEN_ONVIF_BLOCK, REBOOT_IPC_DEVICE,  600,	20, 	1200,	MONITOR_EXIST}, 	 // 监听ONVIF连接阻塞存在20个且持续120S则重启IPC
	{LISTEN_ONVIF_3702,  REBOOT_IPC_DEVICE,  600,	 1, 	1200,	MONITOR_NOT_EXIST},  // 监听ONVIF 3702端口不存在且持续900S则重启IPC
};

static stuKernelMsgStatus g_dmesg_status_info[] = {
    /*----对象------------动作---------*/ 
    {eKernelMsgMonitorType_kernelPanic, KERNEL_PANIC, 1, FORCE_DEV_REBOOT},
    {eKernelMsgMonitorType_kernelOops, KERNEL_OOPS, 1,  FORCE_DEV_REBOOT},
    {eKernelMsgMonitorType_mmcblk0Err, MMCBLK0_ERR, 10,   SD_IS_DAMAGED},
    {eKernelMsgMonitorType_mmc0Err, MMC0_ERROR, 10,   SD_IS_DAMAGED},
    {eKernelMsgMonitorType_mmcblk1Err, MMCBLK1_ERR, 10,   SD_IS_DAMAGED},
    {eKernelMsgMonitorType_mmc1Err, MMC1_ERROR, 10,   SD_IS_DAMAGED},
    {eKernelMsgMonitorType_fatfsErr, FAT_FS_ERR, 10,   FAT_FS_EXP},
    {eKernelMsgMonitorType_fatfsReadErr, FAT_FS_READ_ERR, 1,   FAT_FS_EXP}
};
//记录g_dmesg_status_info里的异常条数是否达到了阈值，达到阈值后才做动作
static int g_kernelMsgAbnormalStatistics[eKernelMsgMonitorType_num] = {0};

static unsigned int g_dmesg_monitor_time_interval = 0;

/*@ preif RunOperateCommand 针对dmesg监控信息，当监控到之后，执行相应的操作
 *@ return  1 监控到了相关信息，并执行了对应的命令
 *@        0 没有监控到了相关信息
 *@ note  针对相关命令，这里主要是reboot和创建错误标记
 */
static int RunOperateCommand(void)
{
    int i = 0;
    int count = sizeof(g_dmesg_status_info)/sizeof(g_dmesg_status_info[0]);
    for (i=0; i<count; i++)
    {
        if (1 == IsStringInCommand("dmesg", g_dmesg_status_info[i].listen_key))
        {
            mysystem(g_dmesg_status_info[i].option_cmd);
            return 1;
        }
    }
    return 0;
}

/*@ preif RunOperateCommandV2 针对dmesg监控信息，当监控到之后，执行相应的操作
 *@ return  1 监控到了相关信息，并执行了对应的命令
 *@        0 没有监控到了相关信息
 *@ note  针对相关命令，这里主要是reboot和创建错误标记
 */
static int RunOperateCommandV2(void)
{
    int i = 0;
	int ret = 0;
    int count = sizeof(g_dmesg_status_info)/sizeof(g_dmesg_status_info[0]);
	char command[128] = {0};
	char result[128] = {0};
	int foundNum = 0;
	int kernelAbnormal = 0;

    for (i=eKernelMsgMonitorType_kernelPanic; i <= eKernelMsgMonitorType_kernelOops; i++)
    {
        if (1 == IsStringInCommand("dmesg", g_dmesg_status_info[i].listen_key))
        {
            mysystem(g_dmesg_status_info[i].option_cmd);
            return 1;
        }
    }

	//统计异常出现的次数
	for (i=eKernelMsgMonitorType_mmcblk0Err; i < eKernelMsgMonitorType_num; i++)
	{
		foundNum = 0;
		memset(command, 0, sizeof(command));
		memset(result, 0, sizeof(result));
		snprintf(command, sizeof(command), "dmesg | grep -rn \"%s\" | wc -l", g_dmesg_status_info[i].listen_key);
		ret = ShellCmdOut(command, result, sizeof(result));
		if(0 == ret)
		{
			foundNum = atoi(result);
			g_kernelMsgAbnormalStatistics[i] += foundNum;
			DETC_DBG("Check dmesg info: %s found=%d, total count=%d\n", g_dmesg_status_info[i].listen_key, foundNum, g_kernelMsgAbnormalStatistics[i]);
		}
		else
		{
			DETC_ERR("Run command : %s fail\n", command);
		}

		if(foundNum)
			kernelAbnormal = 1;
	}

	//判断监测的异常信息是否超过了阈值，若超过再做做处理
	for (i=eKernelMsgMonitorType_mmcblk0Err; i < eKernelMsgMonitorType_num; i++)
	{
		if(g_kernelMsgAbnormalStatistics[i] >= g_dmesg_status_info[i].Abnormal_count)
		{
			DETC_ERR("Dmesg abnormal: %s count=%d > %d\n", g_dmesg_status_info[i].listen_key, g_kernelMsgAbnormalStatistics[i], g_dmesg_status_info[i].Abnormal_count);
			DETC_ERR("Run operate: %s\n", g_dmesg_status_info[i].option_cmd);
			mysystem(g_dmesg_status_info[i].option_cmd);
		}
	}

	//为了统计下次异常的次数，若本次出现了异常则清掉dmesg缓存
	if(kernelAbnormal)
	{
		mysystem("dmesg -c");
	}

    return 0;
}

static void *netstat_status_check(void *param)
{
	pthread_detach(pthread_self());

	int count = 0; 
	int continueTime = 0;
	char buf[256] = {0};
	char command[64] = {0};
	char port[16] = {0};
	char http_port[32] = {0};
	FILE *fp = NULL;
	char *point = NULL;
	char *point_6060 = NULL;
	char *point_6066 = NULL;

	pNetStatusInfo plisten_status = (pNetStatusInfo)param;

	if (NULL == plisten_status)
	{
		printf("Fail to create netstat_status_check\n");
		return NULL;
	}

	/* 对于Onvif信息的监控，前提是需要存在Onvif库，对于Onvif库不存在，则没必要监控 */
	if ((0 == strcmp(LISTEN_ONVIF_3702, plisten_status->listen_key))
	 || (0 == strcmp(LISTEN_ONVIF_BLOCK, plisten_status->listen_key)))
	{
		if (access("/usr/lib/libhyonvif.so", F_OK) || access("/usr/lib/libonvif.so", F_OK))
		{
			printf("libonvif or libhyonvif not exist, not monitor 3702 port!\n");
			return NULL;
		}
	}

	if (plisten_status->listen_freq > plisten_status->event_duration)
	{
		plisten_status->event_duration = plisten_status->listen_freq;
	}

	while (1)
	{
		pthread_mutex_lock(&g_net_status_lock);

		bzero(command, sizeof(command));
		sprintf(command, "netstat -an > /tmp/app/monitor_process");
		mysystem(command);		
		mysystem("sync");

		fp = fopen("/tmp/app/monitor_process", "r");
		if(fp == NULL)		
		{			
			fprintf(stderr, "fopen %s fail: %s\n", "/tmp/app/monitor_process", strerror(errno));
			pthread_mutex_unlock(&g_net_status_lock);
			sleep(20);
			continue;
		}

		memset(port, 0, sizeof(port));
		memset(http_port, 0, sizeof(http_port));
		/* 对于boa端口的监听，需要从boa配置文件里面获取对应的端口 */
		if (0 == strcmp(LISTEN_BOA_PORT, plisten_status->listen_key))
		{
			if (1 == FindAfterStringEndline("/mnt/mtd/etc/conf.d/boa/boa.conf", "Port ", port))
			{
				printf("port = %s\n", port);
				sprintf(http_port, "0.0.0.0:%s", port); // 0.0.0.0:80
			}
		}

		count = 0;
		bzero(buf, sizeof(buf));
		while(NULL != fgets(buf, 255, fp))
		{
			if (0 == strcmp(LISTEN_BOA_PORT, plisten_status->listen_key))
			{
				if (http_port[0] != '\0')
				{
					point = strcasestr(buf, http_port);
				}
				else
				{
					point = NULL;
				}
			}
			else
			{
				point = strcasestr(buf, plisten_status->listen_key);
			}

			if(NULL != point)               // 如果存在则开始计数
			{				
				if( 0 == strcmp(LISTEN_CLOSE_WAIT,plisten_status->listen_key))
				{
					point_6060 = strcasestr(buf, "6060");
					point_6066 = strcasestr(buf, "6066");
					if(point_6060 != NULL || point_6066 != NULL)
					{
						count ++;  
					}
				}
				else
				{
					count ++;
				}             
			}

			bzero(buf, sizeof(buf));
		}
		fclose(fp);        
		fp = NULL;

#if 0
		printf("key: %s, cmd: %s, freq: %d, count: %d, time: %d, type: %d, ncount: %d!\n",
				plisten_status->listen_key,
				plisten_status->option_cmd, 
				plisten_status->listen_freq,
				plisten_status->listen_count,
				plisten_status->event_duration,
				plisten_status->listen_type,
				count);
#endif

		if (MONITOR_EXIST == plisten_status->listen_type)
		{
			if(count >= plisten_status->listen_count) //数量达到plisten_status->listen_count个开始计时        
			{			
				continueTime += plisten_status->listen_freq;
				printf("===listen exist! continueTime:%d===\n", continueTime);
			}		
			else
			{			
				continueTime = 0;        
			}
		}
		else
		{
			if (0 == count) // 如果不存在该监听对象则需要开始计数
			{
				continueTime += plisten_status->listen_freq;
				printf("===listen not exist! continueTime:%d===\n", continueTime);
			}
			else            // 如果存在则直接清空
			{
				continueTime = 0;        
			}
		}

		if((0 == GetNetlinkStatus(IFNAME)) && (0 == strcmp(LISTEN_ONVIF_3702, plisten_status->listen_key)))
		{
			continueTime = 0; 
			printf("Type %s cleanup continuTime %d\n", plisten_status->listen_key,continueTime);
			 
		}

		// 如果监听时间到达，则需要执行对应的命令
		if(continueTime >= plisten_status->event_duration)		
		{			
			printf("==========listen \"%s\" condition OK!, need to exec %s==========\n", 
			        plisten_status->listen_key, plisten_status->option_cmd);
			mysystem(plisten_status->option_cmd);
			continueTime = 0;
		}

		pthread_mutex_unlock(&g_net_status_lock);

		sleep(plisten_status->listen_freq);	
	}
	return NULL;
}

static void* MonitorDmesgInfo(void *lparam)
{
    pthread_detach(pthread_self());

    while(1)
    {
        /* 如果监测到需要执行操作的dmesg信息，则需要进行对应的操作 */
        if (RunOperateCommandV2())
        {
			DETC_INF("Detect exception and exec some command over!\n");
        }
        /* 监控频率默认为3分钟 */
        sleep(g_dmesg_monitor_time_interval);
    }

    return NULL;
}

static int MonitorWPALogSize()
{
	char buf[256] = {0};
	char size[8] = {0};
	int ret = 0;
	
	if(0 == access(WPA_LOG_FILE, 0))
	{
		snprintf(buf, sizeof(buf), "ls -l %s | awk -F \" \" '{print $5}'", WPA_LOG_FILE);
		ret = ShellCmdOut(buf, size, sizeof(size));
		if(0 == ret)
		{
			if(strncmp(size, WPA_LOG_FILE_MAX_SIZE, sizeof(size)) > 0)
			{
				DETC_INF("file %s is more than %s, clear old log!\n", WPA_LOG_FILE, WPA_LOG_FILE_MAX_SIZE);
				memset(buf, 0, sizeof(buf));
				snprintf(buf, sizeof(buf), "echo > %s", WPA_LOG_FILE);
				mysystem(buf);
			}
		}
	}

	return 0;
}

static void* MonitorFileSize(void *lparam)
{
    pthread_detach(pthread_self());

    while(1)
    {
        /* 如果监测到需要执行操作的dmesg信息，则需要进行对应的操作 */
        if (MonitorWPALogSize())
        {
			DETC_INF("Detect exception and exec some command over!\n");
        }
		
        sleep(600);
    }

    return NULL;
}

int main(int argc, const char *argv[])
{
    if (argc > 2)
    {
        DETC_ERR("Usage: %s [monitor_dmesg_time_interval (s)]\n", argv[0]);
        return -1;
    }
    else if (argc > 1)
    {
        g_dmesg_monitor_time_interval = atoi(argv[1]);
    }
    else
    {
        /* 监控频率默认为3分钟 */
        g_dmesg_monitor_time_interval = 180;
    }

    DETC_INF("dmesg monitor frequency is %d seconds once!\n", g_dmesg_monitor_time_interval);

	/* 监测配置文件是否超过大小*/
    pthread_t monitor_fileSize_id = 0;	
    if (pthread_create(&monitor_fileSize_id, NULL, MonitorFileSize, NULL) != 0) 
    {
        DETC_SYS("########### create thread MonitorFileSize fail ###########\n");
    }
	
    /* 监测dmesg各种内核错误异常情况 */
    pthread_t monitor_dmesg_id = 0;	
    if (pthread_create(&monitor_dmesg_id, NULL, MonitorDmesgInfo, NULL) != 0) 
    {
        DETC_SYS("########### create thread MonitorDmesgInfo fail ###########\n");
    }

	/* 查看网络协议是否起来，如果起来则生成该文件，则需要去监测 */
	while(access(NETSERVER_HAS_START, 0))
	{
		sleep(1);
	}

	DETC_INF("Net Service has started, Begin to monitor netstat!\n");

    /* 监测netstat各种网络端口异常情况 */
	int i = 0;
	pthread_t threadId_thread = 0;
	int nLineCount = sizeof(g_netstat_status_info)/sizeof(g_netstat_status_info[0]);

	for (i=0; i<nLineCount; i++)
	{
		if (pthread_create(&threadId_thread, NULL, netstat_status_check, (void*)&g_netstat_status_info[i]) != 0) 
		{
			DETC_SYS("########### create thread netstatus[%d] pthread fail ###########\n", i);
		}
		sleep(1);
	}

#if !defined(__NOT_CHECK_ONVIF__)
	/*onvif 能力定制 */
	int nOnvifCap = 1;
	FILE *fp = NULL;
	char buf[256] = {0};
	fp = fopen("/mnt/mtd/config/customability.ini", "r");		
	if(fp != NULL)		
	{		
		while(NULL != fgets(buf, 255, fp))
		{
			if(strcasestr(buf, "IsSupportOnvif") != NULL)
			{
				nOnvifCap =  atoi((buf+15));
				DETC_SYS("############  nOnvifCap  %d\n", nOnvifCap);
			}						
		}
		fclose(fp);        
		fp = NULL;
	}


	if(nOnvifCap)
	{
		nLineCount = sizeof(g_netstat_status_info_1)/sizeof(g_netstat_status_info_1[0]);

		for (i=0; i<nLineCount; i++)
		{
			if (pthread_create(&threadId_thread, NULL, netstat_status_check, (void*)&g_netstat_status_info_1[i]) != 0) 
			{
				DETC_SYS("########### create thread netstatus[%d] pthread fail ###########\n", i);
			}
			sleep(1);
		}
	}
#endif	

	

	while(0 == access(NETSERVER_HAS_START, 0))
	{
		sleep(1);
	}

	return 0;
}
