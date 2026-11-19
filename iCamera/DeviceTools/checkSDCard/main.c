#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/mount.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/in.h>

#include "fdisk.h"

#undef _GNU_SOURCE

#ifdef __USE_EMMC__
#define SD_DEV_NODE        "/dev/mmcblk1"
#define SD_DEV_NODE_PART1  "/dev/mmcblk1p1"
#define DOSFSCK_SD_PART1   "fsck.fat -y -w -V /dev/mmcblk1p1"
#define FORMAT_SD_PART1    "mkfs.fat -F 32 -s 128 /dev/mmcblk1p1"
#define FORMAT_SD_PART1_EXFAT   "mkfs.exfat /dev/mmcblk1p1"
#define SD_CARD_IS_RW      "mount | grep mmcblk1p1 | awk -F \"[,(]\" '{print $2}'"
#define REMOVE_CARD_NODE   "rm -rf /dev/mmcblk1*"
#else
#define SD_DEV_NODE        "/dev/mmcblk0"
#define SD_DEV_NODE_PART1  "/dev/mmcblk0p1"
#define DOSFSCK_SD_PART1   "fsck.fat -y -w -V /dev/mmcblk0p1"
//#define FORMAT_SD_PART1    "mkfs.fat -F 32 -s 128 /dev/mmcblk0p1"
#define FORMAT_SD_PART1    "mkfs.vfat /dev/mmcblk0p1"
#define FORMAT_SD_PART1_EXFAT   "mkfs.exfat /dev/mmcblk0p1"
#define SD_CARD_IS_RW      "mount | grep mmcblk0p1 | awk -F \"[,(]\" '{print $2}'"
#define REMOVE_CARD_NODE   "rm -rf /dev/mmcblk0*"
#endif

#define FDISK_RESET_FLAG	"/mnt/mtd/config/.Fdiskresetdevice"			//因fdisk复位设备标识文件
// 因有些卡异常在主程序运行时无法处理或处理失败，所以记录需要的动作然后在下次启动时checkSDCard里再做处理。记录的动作定义见eSdActionAtStart
#define SD_ACTION_AT_START_FILE  "/mnt/mtd/config/.SDActionAtStart"

#define CLEAR_DMSEG_INFO   "dmesg -c"
#define SD_STATUS_CFG      "/mnt/mtd/config/.dmesg_status.log"
#define MOUNT_DIR          "/tmp/app/disk"

#define SD_STATUS_CFG_TEMP_FILE      "/tmp/.dmesg_statusTemp.log"

#define SYSTEM_TIME_OUT         (5*60*100)	//10ms in unit
#define SECTOE_CHECK_TIMEOUT	(10)		//超过认为卡有问题

#define NEW_PRODUCT		   1
#define FAT32_FILESYSTEM   "FAT32"
#define EXFAT_FILESYSTEM   "EXFAT"
#define FAT32_CLUSTER_16K  32
#define FAT32_CLUSTER_32K  64
#define FAT32_CLUSTER_64K  128
#define DISK_SIZE_32G	   (1 << 15)
#define DISK_SIZE_64G	   (1 << 16)
#define DISK_SIZE_128G	   (1 << 17)


//文件系统类型
typedef enum
{
    FS_FAT32,
    FS_EXFAT,
    FS_NTFS,
    FS_EXT2,
    FS_ERROR_TYPE,
}E_FILE_SYS_TYPE;

typedef enum {
    SD_ACTION_AT_START_INVALID = 0,      // 无效值
    SD_ACTION_AT_START_FORMAT = 1,      // 卡需要格式化
    SD_ACTION_AT_START_REPAIR = 2,      // 卡需要修复
    SD_ACTION_AT_START_NUM      // 错误总数
}eSdActionAtStart;

//fsck.fat返回码
typedef enum {
	NORMAL = 0,										//文件系统无修改，一切正常
	REPAIR_NORMAL,									//文件系统有修改，修复正常
	MALLOC_ERROR,									//内存分配失败
	SET_CODEPAGE_ERROR,								//设置转换字符集失败
	COMMAND_ARGU_NUM_ERROR,							//命令行参数设置个数不对
	COMMAND_ARGU_CONFLICT,							//命令行参数设置有冲突
	OPEN_ERROR,										//Open 块设备失败
	LEESK_ERROR,									//lseek失败
	READ_ERROR,										//read 失败
	WRITE_ERROR,									//write失败
	CLOSE_ERROR,									//close 失败
	ROOTDIR_ALLOC_ERROR,							//根目录下申请目录项空间失败
	ROOTDIR_ALLOC_TOOMUCH_ERROR,					//根目录下申请目录项超过10000
	SECTOR_SIZE_ZERO_ERROR,							//扇区大小为0
	SECTOR_SIZE_MULTIPLE_ERROR,						//扇区大小不为512倍数
	CLUSTER_SIZE_ZERO_ERROR,						//簇大小为0
	FAT_NUM_ERROR,									//分区表数量不对，不为1或者2
	READ_LAST_SECTOR_ERROR,							//测试读取最后一个扇区失败
	FAT_LENGTH_ZERO_ERROR,							//文件分配表（fat）长度为0
	DISKSPACE_LESSTHAN_CLUSTERSIZE_ERROR,			//磁盘空间小于一个簇能表示的大小
	ROOTDIR_CLUSTER_INVALID_ERROR,					//指定根目录的簇号是无效簇号
	ROOTDIR_CHANGE_ERROR,							//修改根目录错误
	FILENAME_TOOLANG_ERROR,							//有文件的文件名过长，超过1023字节
	TOOMUCH_FILE_NEED_REPAIR_ERROR,					//过多文件需要修复，超过9999999
	AUTORENAME_ERROR,								//自动重命名失败
	FILESIZE_TOOBIG_ERROR,							//单个文件的簇链大小超过4G
	CROSSCLUSTER_HANDLER_ERROR,						//交叉簇处理失败
	GET_FATENTRY_ERROR,								//获取fat表项时，簇号超过最大簇号
	FAT_SIZE_ERROR,									//Fat表大小无效
	FAT_ALLBAD_ERROR,								//两个fat表都是坏的
	SET_CLUSTERNUM_ERROR,							//设置的簇号超过最大簇号
	FILE_CLUSTER_HAS_BAD_ERROR,						//某一文件的下一个簇号是坏簇
	LONGENTRY_HANDLER_ERROR,						//长目录文件处理失败
	FILESYSTEM_TOOLANG_ERROR,						//文件系统太大
	NOROOTDIR_ERROR,								//无根目录
	CLUSTER_TOOMUCH_ERROR,							//簇号过多
	FAT_SPACE_TOOLONG_ERROR,						//fat表项过多
	ASSIGN_DELETEDIR_ERROR,							//不可以指定删除目录
	CONVERTCASE_ERROR,								//大小写转换错误	
	CHOICE_PARA_ERROR,								//选择参数错误	
	INVALID_FATENTRY_SIZE,							//无效的fat表项大小
	INVALID_FATENTRY_VALUE,							//fat表项值无效		
	SET_FILEOWNER_ERROR								//循环簇交叉簇处理完后某些文件中的簇还属于多个所有者
}Return_Code;


//分区表中的文件系统类型值
typedef enum {
	FS_FAT32_VALUE = 0x0B,
	FS_EXFAT_VALUE = 0x07
}FS_TYPE;


/* 定义映射表结构体 */
typedef struct
{
	unsigned id;
	const char* desc;
}MY_DESCRIBE_COMM_TYPE;

/* 映射函数 */
static const char* GetDescByID(MY_DESCRIBE_COMM_TYPE *desc, unsigned id)  
{ 
	int i = 0;

	while (0xffff != desc[i].id)
	{
		if(desc[i].id == id)
		{
			return desc[i].desc;
		}
		i++;
	}

	return "Can't find the ID";
}

#define SD_EXP_CMD        "echo 2 > /tmp/.sd_status"

/* 采用宏的#处理方法对宏进行封装 */
#define MY_DECRIBE_DECLARE_BEGIN(type) static MY_DESCRIBE_COMM_TYPE  MY_describe_desc_##type[]={
#define MY_DECRIBE_DECLARE_END  {0xffff, "end"} };
#define MY_DECRIBE_ADD_DESC(id, str) {id, #str},
#define MY_DESCRIBE(type, id) GetDescByID(MY_describe_desc_##type,id)

typedef enum{
	SdCardBootCheck_NoNode        = 0, // 设备无节点(无设备)
	SdCardBootCheck_MountFail     = 1, // 挂载失败(SD卡异常)
	SdCardBootCheck_DiskExp       = 2, // SD卡异常
	SdCardBootCheck_AddPartFail   = 3, // 添加分区失败(SD卡异常)
	SdCardBootCheck_ReadWriteFail = 4, // 读写测试失败(需要格式化)
	SdCardBootCheck_DiskIsNormal  = 5, // SD卡正常
	SdCardFormatStatus_FormatFail = 12,// SD卡格式化失败
}eBootSdCardStatus;

MY_DECRIBE_DECLARE_BEGIN(eBootSdCardStatus)  // 宏定义开始
	MY_DECRIBE_ADD_DESC(SdCardBootCheck_NoNode, echo 0 > /tmp/.sd_status)  // 注意这里的字符串描述不用加引号
	MY_DECRIBE_ADD_DESC(SdCardBootCheck_MountFail, echo 1 > /tmp/.sd_status)
	MY_DECRIBE_ADD_DESC(SdCardBootCheck_DiskExp, echo 2 > /tmp/.sd_status)
	MY_DECRIBE_ADD_DESC(SdCardBootCheck_AddPartFail, echo 3 > /tmp/.sd_status)
	MY_DECRIBE_ADD_DESC(SdCardBootCheck_ReadWriteFail, echo 4 > /tmp/.sd_status)
	MY_DECRIBE_ADD_DESC(SdCardBootCheck_DiskIsNormal, echo 5 > /tmp/.sd_status)
	MY_DECRIBE_ADD_DESC(SdCardFormatStatus_FormatFail, echo 12 > /tmp/.sd_status)
MY_DECRIBE_DECLARE_END  	

// 检测到内核出错则强制重启
#define FORCE_DEV_REBOOT   "killall -9 myWatchDogProcess; reboot"

//错误信息
#define ERR_TMP_FILE		"/tmp/.dmsg_info"
#define ERR_INFO			"I/O error, dev mmcblk0"
#define ERR_INFO_COUNT		(5)

// dosfsck执行的超时时间
#define DOSFSCK_TIMEOUT    (60)

#define MAX_LOG_LINE       (512)
#define SD_LOG_FILE_OPER_LINE_COUNT (300)               //日志文件达到多少行后触发删除逻辑
#define SD_LOG_FILE_RESERVE_LINE_COUNT (200)            //日志文件触发删除后保留多少行

static int FsckTimeOutFlag = 0;				//执行是否超时
static int cluster_size = 0;				//簇大小
static int sd_size = 0;						//sd卡容量
//0-其他 1-fat32 2-exFat
static int fileSystemIsFat32 = 0;			//文件系统类型是fat32
static int has_product_file = 0;			//是否有产测文件
static int product_type = 0;				//产品类型，新或者就产品
static int has_formated = 0;				//是否格式化
static int g_dosfsck_status = -1;			//dosfsck执行返回码
static int g_domkfs_status = -1;			//domkfs执行返回码
static int g_CheckSDcard_status = -1;			//checkSDcard进程结束状态值

static sem_t g_semdomkfsThread;
static sem_t g_semWakeThread;

static int g_FdiskRebootflag = 0;			//fdisk重启设备标识
static eSdActionAtStart g_sdActionAtStart = SD_ACTION_AT_START_INVALID;   //上次应用发现了卡异常，需要这次设备启动时要处理的动作

typedef struct tagDmesgMonitor{
    const char *monitor_string;                       // 需要监控dmesg里面的字符串
    const char *operate_command;                      // 检测到字符串之后，需要执行的命令
}stuDmesgMonitor, *pDmesgMonitor;

stuDmesgMonitor g_dmesg_monitor[] = {
//  {"Kernel panic",         FORCE_DEV_REBOOT},  // 检测内核恐慌
//  {"Internal error: Oops",  FORCE_DEV_REBOOT}, // 检测内核指针异常
#ifdef __USE__EMMC__
    {"mmcblk1: error",       SD_EXP_CMD},        // 检测SD卡好坏
//  {"FAT-fs (mmcblk1p1)",   SD_EXP_CMD}         // 检测SD卡好坏
    {"FAT-fs (mmcblk1p1): error", SD_EXP_CMD},   // 检测SD卡好坏
#else
    {"mmcblk0: error",       SD_EXP_CMD},        // 检测SD卡好坏
//  {"FAT-fs (mmcblk0p1)",   SD_EXP_CMD}         // 检测SD卡好坏
    {"FAT-fs (mmcblk0p1): error", SD_EXP_CMD},   // 检测SD卡好坏
#endif
    {"mmc1: error",          SD_EXP_CMD},        // 检测SD卡好坏 
};

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
    SD_CARD_ERR_NUM      // 错误总数
}eSdErrorNo;

static const char *gSdErrorMsg[SD_CARD_ERR_NUM] = {
    "[Init] check SD Card no node\n",
    "[Init] check SD Card error no card\n",
    "[Init] check SD Card mount fail no card\n",
    "[Init] check SD Card error in first dmesg\n",
    "[Init] check SD Card read and write exception\n",
    "[Init] check SD Card file md5sum check exception\n",
    "[Init] check SD Card error in second dmesg\n",
    "[Init] check SD Card formated in system boot\n",
    "[Init] check SD Card formated in ambarella_test\n",
    "[Init] check SD Card has been clean up\n",
    "[Init] check SD Card is normal\n",
    "[Init] check SD Card add partition fail\n",
    "[Init] check SD Card error in middle dmesg\n",
    "[Init] check SD Card file md5sum check exception in middle\n",
};

//生产测试文件名列表
static const char *ProductionFileList[] = {
	"/tmp/app/disk/output/.authorization.cfg",
	"/tmp/app/disk/ipcUpgrade.bin",
	"/tmp/app/disk/output/.authorization_cfg.ini",
	"/tmp/app/disk/input/antipack_authenticate",
	"/tmp/app/disk/output/.did.ini",
	"/tmp/app/disk/output/.erp_scode.ini",
	"/tmp/app/disk/output/ProductTest",
	"/tmp/app/disk/upg.txt",
	"/tmp/app/disk/aging_test.list"
};

static uint32_t get_mmc_size_M(const char *dev)
{
	int fd;
	uint64_t blkSize64;
	uint32_t mmcSizeM = -1;
	unsigned long long mmcSizeBytes = 0;

	fd = open(dev, O_RDONLY);
	if (0 == ioctl(fd, BLKGETSIZE64, &blkSize64))
	{
		blkSize64 >>= 9;
		if (blkSize64 == (uint32_t)blkSize64)
		{
			mmcSizeBytes = ((unsigned long long)blkSize64 << 9);
			mmcSizeM = (uint32_t)(mmcSizeBytes/(1024*1024));
		}
	}
	if (-1 == mmcSizeM)
	{
 			printf("[%s] get mmc size fail\n", __FUNCTION__);
	}
	close(fd);
	return mmcSizeM;
}

static int mysystem(const char* cmdstring)
{
	pid_t pid = -1;
	int status = -1;

	if (cmdstring == NULL)
	{
		return 1;
    }
    
	if ((pid=vfork()) < 0 )
	{
		printf("vfork failed.cmd:%s\n", cmdstring);
		status = -1;
	}
	else if (pid==0)
	{
		execl("/bin/sh", "sh", "-c", cmdstring, (char*)0);
		_exit(127);
	}
	else
	{
		int i = 0;
		while (0 == waitpid(pid, &status, WNOHANG))	//check if the the time is out
		{
			if (i++ < SYSTEM_TIME_OUT)
			{
				usleep(10000);	//Here, this sleep period cannot be too long, otherwise, mysystem cannot return immediately.
			}
			else
			{
				printf("\n[SYSTEMD]command %s executing time out!\n",cmdstring);
				kill(pid, SIGKILL);
				waitpid(pid, &status, WNOHANG);
				break;
			}
		}
	}

	return status;
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
			buf[strlen(buf)-1] =  '\0'; 

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

/*@ preif GetFileSystemType get the filesystem type of partion
 *@ param [in] dev_node partion dev node
 * @return     the filesystem type. detail for enum E_FILE_SYS_TYPE
 */
static E_FILE_SYS_TYPE GetFileSystemType(const char* dev_node)
{
    if (NULL == dev_node)
    {
        fprintf(stderr, "NULL POINTER EXCEPTION!\n");
        return FS_ERROR_TYPE;
    }

    int dev_fd = -1;
    unsigned char tmpBuffer[1024] = {0};
    unsigned char* pOffset = NULL;
    unsigned int readSize = 0;
    E_FILE_SYS_TYPE ret_type = FS_ERROR_TYPE;
    
    pOffset = &tmpBuffer[0];

    dev_fd = open(dev_node, O_RDONLY);
    if(!dev_fd)
    {
        ret_type = FS_ERROR_TYPE;
        fprintf(stderr, "open device %s failed!\n", dev_node);
        goto RET_EXIT;
    }

    // read the tag data for ntfs/fat32
    readSize = read(dev_fd, pOffset, 256);
    if (!readSize)
    {
        ret_type = FS_ERROR_TYPE;
        fprintf(stderr, "read device file failed!\n");
        goto RET_EXIT;
    }

    // the offset with 0x52 is fat32 tag;
    if (!memcmp((const void*)(pOffset+0x52), (const void*)"FAT32", 5))
    {
        ret_type = FS_FAT32;
        goto RET_EXIT;
    }
    // the offset with 0x3 is ntfs tag;
    else if (!memcmp((const void*)(pOffset+0x3), (const void*)"NTFS", 4))
    {
        ret_type = FS_NTFS;
        goto RET_EXIT;
    }
    // the offset with 0x3 is exfat tag;
    else if (!memcmp((const void*)(pOffset+0x3), (const void*)"EXFAT", 5))
    {
        ret_type = FS_EXFAT;
        goto RET_EXIT;
    }
    else
    {
        // seek to superblock1;
        lseek(dev_fd, sizeof(tmpBuffer), SEEK_SET);

        // read the superblock1 to buffer;
        memset(pOffset, 0x0, sizeof(tmpBuffer));
        readSize = read(dev_fd, pOffset, sizeof(tmpBuffer));
        if(!readSize)
        {
            ret_type = FS_ERROR_TYPE;
            goto RET_EXIT;
        }

        // the offset 0x438 is the tag of ext2:0x53 0xef;
        pOffset = pOffset + 0x38;
        if((0x53 == pOffset[0]) && (0xef == pOffset[1]))
        {
            ret_type = FS_EXT2;
            goto RET_EXIT;
        }
        else
        {
            printf("the offset 0x438 vaule is:%#x %#x!\n", pOffset[0], pOffset[1]);
        }
    }

RET_EXIT:
    if (dev_fd > 0)
    {
        close(dev_fd);
        dev_fd = -1;
    }
    return ret_type;
}

/*@ preif MountvFatNode 封装mount函数，只是针对于VFAT设备的挂载
 *@ param [in] dev 要挂载的目录
 *@ param [in] mnt_point 挂载点
 *@ return -1 挂载失败
 *@        0 挂载成功
 *@ note  较之mount命令，避免了针对异常SD卡挂载塞住的问题
 */
static int MountvFatNode(const char *dev, const char *mnt_point)
{
    char cmd[120];

    if (!dev || !mnt_point)
    {
        FDISK_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if (access(dev, F_OK) || access(mnt_point, F_OK))
    {
        FDISK_ERR("%s or %s not exist!\n", dev, mnt_point);
        return -1;
    }

    int ret = -1;
    E_FILE_SYS_TYPE fs_type = FS_ERROR_TYPE;
    fs_type = GetFileSystemType(dev);
    
#ifdef __FORMAT_VFAT__
    if(fs_type != FS_FAT32)
    {
        FDISK_ERR("fs_type:%d != %d not exist!\n", dev, mnt_point);
        snprintf(cmd, sizeof (cmd), "mkfs.vfat %s", dev);
        mysystem(cmd);
    }
    fs_type = FS_ERROR_TYPE;
    fs_type = GetFileSystemType(dev);
#endif

    switch(fs_type)
    {
        case FS_EXT2:
            ret = mount((const char*)dev, (const char*)mnt_point, (const char*)"ext2", 0, "errors=continue");
            break;
        case FS_EXFAT:
            ret = mount((const char*)dev, (const char*)mnt_point, (const char*)"exfat", 0, "errors=continue");
            break;
        case FS_NTFS:
            ret = mount((const char*)dev, (const char*)mnt_point, (const char*)"ntfs3g", 0, "errors=continue");
            break;
        case FS_FAT32:
            ret = mount((const char*)dev, (const char*)mnt_point, (const char*)"vfat", 0, "errors=continue");
            break;
        default:
            FDISK_ERR("Unknown filesystem type!\n");
    }

    if (ret)
    {
        FDISK_SYS("filesystem type is %d mount fail!\n", fs_type);
    }

    return ret;
}

// 描述:  执行命令并将结果存储到result字符串中 
// 参数:  cmd表示要执行的命令
// 返回值:成功返回0，失败返回-1
static int ShellCmdOut(const char *cmd, char *result, int len) 
{  
	struct timeval now;
	char fileName[64] = {0};
	char command[256] = {0};
	int ret = 0;
	FILE *fp = NULL;

	if(!cmd || !strlen(cmd) || len<=0)	
		return -1;

	gettimeofday(&now, NULL);

	sprintf(fileName, "/tmp/.ShellCmdOut%ld", now.tv_usec);

	memset(result, 0, len); 
	sprintf(command, "%s > %s", cmd, fileName);

	ret = mysystem(command);
	if (ret)
	{
		fprintf(stderr, "fail to called mysystem!\n");
		return -1;
	}
	
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
	
	memset(command, 0, sizeof(command));
	sprintf(command, "rm -rf %s", fileName);
	mysystem(command);
	
	return ret; 							   
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
		FDISK_SYS("Fail to fopen %s\n", pFile);
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

/**
********************************************************************************
 * @函数功能: 防止日志超过大小占用多余配置空间
 * @作者    : HL.LIU
 * @返回值  : 
 * @其他    : 
********************************************************************************
*/
int checkSDLogFileSize()
{
    int ret = 0;
    char command[128] = {0};
    char result[32] = {0};
    unsigned int logLineNum = 0;
    unsigned int logLineNumNew = 0;
    unsigned int needDeletLineNum = 0;

    if (access(SD_STATUS_CFG, F_OK))
    {
        FDISK_DBG("Not found SD log file: %d!\n", SD_STATUS_CFG);
        return 0;
    }

    //获取日志文件是否超过行数阈值
    snprintf(command, sizeof(command), "sed -n '$=' %s", SD_STATUS_CFG);

    ret = ShellCmdOut(command, result, sizeof(result));
    if(0 == ret)
    {
        logLineNum = atoi(result);
    }
    else
    {
        FDISK_ERR("Run command : %s fail\n", command);
    }

    FDISK_DBG("Log file %s lines=%d\n", SD_STATUS_CFG, logLineNum);
    //若行数超过阈值则删掉最老的行数
    if(logLineNum >= SD_LOG_FILE_OPER_LINE_COUNT)
    {
        needDeletLineNum = logLineNum - SD_LOG_FILE_RESERVE_LINE_COUNT;
        snprintf(command, sizeof(command), "sed '1,%dd' %s > %s", needDeletLineNum, SD_STATUS_CFG, SD_STATUS_CFG_TEMP_FILE);
        mysystem(command);

        //检查裁减后的文件的行数来判断是否成功
        if (0 == access(SD_STATUS_CFG_TEMP_FILE, F_OK))
        {
            snprintf(command, sizeof(command), "sed -n '$=' %s", SD_STATUS_CFG_TEMP_FILE);
            ret = ShellCmdOut(command, result, sizeof(result));
            if(0 == ret)
            {
                logLineNumNew = atoi(result);
            }
            else
            {
                FDISK_ERR("Run command : %s fail\n", command);
            }

            if(SD_LOG_FILE_RESERVE_LINE_COUNT == logLineNumNew)
            {
                FDISK_DBG("Cut down %s to %d success!\n", SD_STATUS_CFG, SD_LOG_FILE_RESERVE_LINE_COUNT);
                //将新日志文件覆盖老日志文件
                snprintf(command, sizeof(command), "mv %s %s", SD_STATUS_CFG_TEMP_FILE, SD_STATUS_CFG);
                mysystem(command);
            }
            else
            {
                FDISK_ERR("Cut down %s to %d fail! logLineNumNew = %d\n", SD_STATUS_CFG, SD_LOG_FILE_RESERVE_LINE_COUNT, logLineNumNew);
                snprintf(command, sizeof(command), "rm -rf %s", SD_STATUS_CFG_TEMP_FILE);
                mysystem(command);
            }
        }
        else
        {
            FDISK_ERR("Run command fail : %s!\n", command);
            return -1;
        }
    }

    return 0;
}

/*@breif 获取主应用需要在启动时对卡做的操作,读取完成后需要删除文件
 *@return 返回 1 表示SD卡正常
 *@      返回 0 表示SD卡异常或者无SD卡
 *@note  状态位已经放在全局变量里面
 */
static int GetSDActionAtStart(void)
{
    char command[64] = {0};
    char buff[4] = {0};

    if (access(SD_ACTION_AT_START_FILE, F_OK))
    {
        FDISK_DBG("Not found SD action at start!\n");
        return 0;
    }

    FILE *fp = NULL;
    fp = fopen(SD_ACTION_AT_START_FILE, "r");
    if (!fp)
    {
        FDISK_ERR("Fail to fopen %s\n", SD_ACTION_AT_START_FILE);
        return -1;
    }
    
    if (NULL == fgets(buff, sizeof(buff), fp))
    {
        FDISK_ERR("Fail to fgets %s\n", SD_ACTION_AT_START_FILE);
        fclose(fp);
        return -1;
    }
    fclose(fp);

    snprintf(command, sizeof(command), "rm -rf %s", SD_ACTION_AT_START_FILE);
    mysystem(command);

    unsigned int sdAction = atoi(buff);
    if(sdAction >= SD_ACTION_AT_START_NUM)
    {
        FDISK_ERR("Cannot release sdAction %d\n", sdAction);
        return -1;
    }

    g_sdActionAtStart = sdAction;  

    return 0;
}

/*@ preif RunOperateCommand 针对dmesg监控信息，当监控到之后，执行相应的操作
 *@ return  1 监控到了相关信息，并执行了对应的命令
 *@        0 没有监控到了相关信息
 *@ note  针对相关命令，这里主要是reboot和创建错误标记，故只需要执行一次，以
 *@      后如果需要重复监控，可以修改该函数
 */
static int RunOperateCommand(void)
{
    int i = 0;
    int count = sizeof(g_dmesg_monitor)/sizeof(g_dmesg_monitor[0]);
    for (i=0; i<count; i++)
    {
        if (1 == IsStringInCommand("dmesg", g_dmesg_monitor[i].monitor_string))
        {
            mysystem(g_dmesg_monitor[i].operate_command);
            mysystem("dmesg -c");
            FDISK_DBG("%s - %s over!\n", g_dmesg_monitor[i].monitor_string, g_dmesg_monitor[i].operate_command);
            return 1;
        }
    }
    return 0;
}

/* 检测SD卡进行dosfsck超时(针对异常的SD卡，在执行dosfsck的时候，会塞在很长时间) */
static void *MonitorDosfsckTimeOut(void *lparam)
{
    pthread_detach(pthread_self());

    sem_wait(&g_semWakeThread);

    int time_count = DOSFSCK_TIMEOUT;

    while(1)
    {
        sleep(1);

        if (-1 != g_dosfsck_status)
        {
            FDISK_INFO("fsck.fat finish! timeout check pthread exit!\n");
            break;
        }
        
        /* 超时则杀死fsck.fat命令 */
        if (--time_count <= 0)
        {
        	FsckTimeOutFlag = 1;
			mysystem(MY_DESCRIBE(eBootSdCardStatus, SdCardBootCheck_DiskExp));
            FDISK_ERR("fsck.fat timeout! over %d seconds need kill fsck.fat and continue!\n", DOSFSCK_TIMEOUT);
            mysystem("killall -9 fsck.fat");
			mysystem("killall -9 checkSDCard");

            sleep(1);
            break;
        }
    }

    return NULL;
}

/* 检测SD卡进行dosfsck超时(针对异常的SD卡，在执行dosfsck的时候，会塞在很长时间) */
static void *MonitorDosmkfsTimeOut(void *lparam)
{
    pthread_detach(pthread_self());

    sem_wait(&g_semdomkfsThread);

    int time_count = 60;
    while(1)
    {
		printf("MonitorDosmkfsTimeOut time_count = %d g_domkfs_status = %d\n",time_count,g_domkfs_status);
        sleep(1);

        if (-1 != g_domkfs_status)
        {
        	g_domkfs_status = -1;
            FDISK_INFO("fmkfs.vfat finish! timeout check pthread exit!\n");
            break;
        }
        
        /* 超时则杀死mkfs.vfat命令 */
        if (--time_count <= 0)
        {
            FDISK_ERR("mkfs.vfat timeout! over 60 seconds need  Reboot!\n");
            mysystem(MY_DESCRIBE(eBootSdCardStatus, SdCardFormatStatus_FormatFail));
            mysystem("killall -9 mkfs.vfat");
			mysystem("killall -9 checkSDCard");
            break;
        }
    }

    return NULL;
}

/* 检测CheckSDcard进程超时(针对异常的SD卡，在执行CheckSDcard进程的时候，会在各种地方阻塞长时间) */
static void *MonitorCheckSDcardTimeOut(void *lparam)
{
    pthread_detach(pthread_self());

    int time_count = 90;

    while(1)
    {
        sleep(1);

        if (-1 != g_CheckSDcard_status)
        {
            FDISK_INFO("CheckSDcard finish! timeout check pthread exit!\n");
            break;
        }
        
        /* 超时则杀死checkSDcard进程命令 */
        if (--time_count <= 0)
        {
			mysystem(MY_DESCRIBE(eBootSdCardStatus, SdCardBootCheck_DiskExp));
            FDISK_ERR("checkSDcard timeout! over 90 seconds need kill checkSDcard and continue!\n");
			mysystem("killall -9 checkSDCard");
            sleep(1);
            break;
        }
    }

    return NULL;
}


/* 检测旧产品是否可用使用fsck.fat进行检测修复 */
static int CheckOldProductUseFsckFat(void)
{
	int need_use_fackfat = 0;

#ifndef __EXFAT_SUPPORT__
	if (fileSystemIsFat32)
#else
    //fsck.fat只支持修复fat32
	if (1 == fileSystemIsFat32)
#endif
	{
		if (sd_size <= DISK_SIZE_32G)
		{
			if ((cluster_size == FAT32_CLUSTER_16K) || 
				(cluster_size == FAT32_CLUSTER_32K) ||
				(cluster_size == FAT32_CLUSTER_64K))
			{
				need_use_fackfat = 1;
			}
		}
		else if (sd_size <= DISK_SIZE_64G)
		{
			if ((cluster_size == FAT32_CLUSTER_32K) || 
				(cluster_size == FAT32_CLUSTER_64K))
			{
				need_use_fackfat = 1;
			}
		}
		else if (sd_size <= DISK_SIZE_128G)
		{
			if ((cluster_size == FAT32_CLUSTER_64K))
			{
				need_use_fackfat = 1;
			}
		}
	}	

	return need_use_fackfat;
}


/* 根据fsck.fat 返回值判断是否需要对sd卡进行格式化操作 */
static int HandlerFsckReturnCode(void)
{
	int sd_need_format = 0;
	int real_code = 0;

	if (FsckTimeOutFlag)
	{
		//超时
		sd_need_format = 1;
	}
	else
	{
		if (0 == (g_dosfsck_status & 0xff))
		{
			real_code = (g_dosfsck_status >> 8) & 0xff;

			printf("real_code = %d\n",real_code);
			
			switch (real_code)
			{
				case ROOTDIR_ALLOC_ERROR:							
				case ROOTDIR_ALLOC_TOOMUCH_ERROR:					
				case SECTOR_SIZE_ZERO_ERROR:							
				case SECTOR_SIZE_MULTIPLE_ERROR:						
				case CLUSTER_SIZE_ZERO_ERROR:						
				case FAT_NUM_ERROR:									
				case READ_LAST_SECTOR_ERROR:							
				case FAT_LENGTH_ZERO_ERROR:							
				case DISKSPACE_LESSTHAN_CLUSTERSIZE_ERROR:			
				case ROOTDIR_CLUSTER_INVALID_ERROR:					
				case ROOTDIR_CHANGE_ERROR:							
				case FILENAME_TOOLANG_ERROR:							
				case TOOMUCH_FILE_NEED_REPAIR_ERROR:					
				case AUTORENAME_ERROR:								
				case FILESIZE_TOOBIG_ERROR:							
				case CROSSCLUSTER_HANDLER_ERROR:						
				case GET_FATENTRY_ERROR:								
				case FAT_SIZE_ERROR:									
				case FAT_ALLBAD_ERROR:								
				case SET_CLUSTERNUM_ERROR:							
				case FILE_CLUSTER_HAS_BAD_ERROR:						
				case LONGENTRY_HANDLER_ERROR:						
				case FILESYSTEM_TOOLANG_ERROR:						
				case NOROOTDIR_ERROR:								
				case CLUSTER_TOOMUCH_ERROR:							
				case FAT_SPACE_TOOLONG_ERROR:						
				case ASSIGN_DELETEDIR_ERROR:							
				case INVALID_FATENTRY_SIZE:							
				case INVALID_FATENTRY_VALUE:							
				case SET_FILEOWNER_ERROR:
					sd_need_format = 1;
					break;
				default:
					break;
			}
		}
	}

	return sd_need_format;
}


/* 获取分区大小(M为单位)，分区文件系统类型，簇大小 */
static int GetFileSystemInfo(const char *dev)
{
	struct boot_sector bootinfo;

	if (NULL == dev)
	{
		return -1;
	}

	//获取分区大小
	if (0 == (sd_size = get_hd_space(dev)))
	{
		return -1;
	}

	//获取分区启动扇区数据
	if (-1 == Get_Partition_BootInfo(dev,&bootinfo))
	{
		return -1;
	}

	if (0 == strncmp((void *)bootinfo.fs_type,FAT32_FILESYSTEM,strlen(FAT32_FILESYSTEM)))
	{
		fileSystemIsFat32 = 1;
		cluster_size = bootinfo.cluster_size;
	}
	#ifdef __EXFAT_SUPPORT__  //支持exfat的项目
	else
	{
	    E_FILE_SYS_TYPE ret_type = GetFileSystemType(dev);
	    switch (ret_type)
	    {
	        case FS_FAT32:
	        {
	            fileSystemIsFat32 = 1;
	            break;
	        }
            case FS_EXFAT:
            {
	            fileSystemIsFat32 = 2;
	            break;
	        }
	        default:
	        {
	            fileSystemIsFat32 = 0;
	        }
	    }
		
		cluster_size = bootinfo.cluster_size;
	}
	#endif

	return 0;
}

/*检测sd卡中是否存在产测文件*/
void CheckSdCardIsProduct(void)
{
	for (int i = 0;i < sizeof(ProductionFileList)/sizeof(char *);i++)
	{
		if (0 == access(ProductionFileList[i],F_OK))
		{
			has_product_file = 1;
			break;
		}
	}
}

static int CheckSdCardUseMkfsVfat(void)
{
	//使用mkfs.vfat 对sd卡进行格式化,创建超时处理线程
	pthread_t minitor_id;
	g_domkfs_status = -1;
	pthread_create(&minitor_id, NULL, MonitorDosmkfsTimeOut, NULL);
	sem_post(&g_semdomkfsThread);
	g_domkfs_status = mysystem(FORMAT_SD_PART1);

	return 0;
}

/* 使用fsck.fat 对sd卡进行检测修复 */
static int CheckSdCardUseFsckFat(void)
{
	int need_use_fackfat = 0;
	int sd_size_nolimit = 1;

	//产测sd卡，不用检测
	if (has_product_file)
	{
		FDISK_INFO("sdcard has product file,don't use fsck.fat!\n");
		return 0;
	}

	//本程序中已经对sd卡格式化，无须使用fsck.fat 进行检测修复
	if (has_formated)
	{
		return 0;
	}
	
	GetFileSystemInfo(SD_DEV_NODE_PART1);

	if (product_type == NEW_PRODUCT)
	{
	    #ifndef __EXFAT_SUPPORT__  //不支持exfat的项目
		//新产品对sd卡进行判断，不是fat32或者簇大小不是64k，进行格式化操作
		if ((cluster_size != FAT32_CLUSTER_64K) || (0 == fileSystemIsFat32))
		{
			has_formated = 1;
            FDISK_INFO("cluster_size:%d != %d || 0 == fileSystemIsFat32:%d! start %s\n", cluster_size, FAT32_CLUSTER_64K, fileSystemIsFat32, FORMAT_SD_PART1);
			//格式化SD卡接口
			CheckSdCardUseMkfsVfat();
            mysystem(CLEAR_DMSEG_INFO);
            if (-1 == Change_DevFileSystemType(SD_DEV_NODE,1,FS_FAT32_VALUE))
			{
				WriteLog(SD_STATUS_CFG, "Change partition file system failed\n");
			}
            WriteLog(SD_STATUS_CFG, "SdCard format! cluster_size = %d fileSystemIsFat32 = %d\n",cluster_size,fileSystemIsFat32);
            return 1;
		}
		else
		{
			need_use_fackfat = 1;
		}
		#else   //支持exfat的项目
		//FAT32卡，簇大小不是64k
		//或，非FAT32/exFat卡
		//则进行格式化
		if (((1 == fileSystemIsFat32) && (cluster_size != FAT32_CLUSTER_64K))
		    || (1 != fileSystemIsFat32 && 2 != fileSystemIsFat32))
		{
			has_formated = 1;
			if (sd_size <= DISK_SIZE_32G)
			{
			    FDISK_INFO("cluster_size:%d != %d || fileSystemIsFat32:%d! start %s\n", cluster_size, FAT32_CLUSTER_64K, fileSystemIsFat32, FORMAT_SD_PART1);
			    //格式化SD卡接口
				CheckSdCardUseMkfsVfat();

			    mysystem(CLEAR_DMSEG_INFO);
                if (-1 == Change_DevFileSystemType(SD_DEV_NODE,1,FS_FAT32_VALUE))
    			{
    				WriteLog(SD_STATUS_CFG, "Change partition file system failed\n");
    			}
                WriteLog(SD_STATUS_CFG, "SdCard format! cluster_size = %d fileSystemIsFat32 = %d\n",cluster_size,fileSystemIsFat32);
			}
			else
			{
			    FDISK_INFO("cluster_size:%d != %d || fileSystemIsFat32:%d! start %s\n", cluster_size, FAT32_CLUSTER_64K, fileSystemIsFat32, FORMAT_SD_PART1_EXFAT);
			    mysystem(FORMAT_SD_PART1_EXFAT);

			    mysystem(CLEAR_DMSEG_INFO);
                if (-1 == Change_DevFileSystemType(SD_DEV_NODE, 1, FS_EXFAT_VALUE))
    			{
    				WriteLog(SD_STATUS_CFG, "Change partition file system failed\n");
    			}
                WriteLog(SD_STATUS_CFG, "SdCard format! cluster_size = %d fileSystemIsFat32 = %d\n",cluster_size,fileSystemIsFat32);
			}
            
            return 1;
		}
		else
		{   
		    //fat32才支持做修复
		    if (1 == fileSystemIsFat32)
		    {
			    need_use_fackfat = 1;
			}
		}
		#endif
	}
	else
	{
		need_use_fackfat = CheckOldProductUseFsckFat();
	}

	if (sd_size > DISK_SIZE_128G)
	{
		sd_size_nolimit = 0;
	}

	FDISK_INFO("sd_size:%d sd_size_nolimit:%d\n",sd_size,sd_size_nolimit);

	if ((sd_size_nolimit) && (need_use_fackfat || (SD_ACTION_AT_START_REPAIR == g_sdActionAtStart)))
	{
		//到这里一定是fat32文件系统。需要修改分区表中的类型，相同则不会修改。避免mac不识别
		if (-1 == Change_DevFileSystemType(SD_DEV_NODE,1,FS_FAT32_VALUE))
		{
			WriteLog(SD_STATUS_CFG, "Change partition file system failed\n");
		}
	
		//使用fsck.fat 对sd卡进行检测修复,创建超时处理线程
		pthread_t minitor_id;
        pthread_create(&minitor_id, NULL, MonitorDosfsckTimeOut, NULL);
        sem_post(&g_semWakeThread);
        g_dosfsck_status = mysystem(DOSFSCK_SD_PART1);

		WriteLog(SD_STATUS_CFG, "g_dosfsck_status = 0x%04x FsckTimeOutFlag = %d\n",g_dosfsck_status,FsckTimeOutFlag);
        if (HandlerFsckReturnCode())
        {
        	has_formated = 1;
        	//格式化SD卡接口
			CheckSdCardUseMkfsVfat();			
            mysystem(CLEAR_DMSEG_INFO);
            WriteLog(SD_STATUS_CFG, "handler fsck return code: SdCard format!\n");
			return 1;
        }
	}

	return 0;
}

static unsigned strstrip(char * s)
{
    char *last = NULL ;
    char *dest = s;

    if (s==NULL) return 0;

    last = s + strlen(s);
    while (isspace((int)*s) && *s) s++;
    while (last > s) {
        if (!isspace((int)*(last-1)))
            break ;
        last -- ;
    }
    *last = (char)0;

    memmove(dest,s,last - s + 1);
    return last - s;
}

static void wifi_info_translate2local(const char *path)
{
    if (NULL == path)
    {
        return;
    }
    char wifiName[256] = {0};
    char wifiPassword[256] = {0};
    int wifiPasswordType = 0;
    FILE *in = NULL;

    // 1.解析文件
    in = fopen(path, "r");
    if (NULL == in)
    {
        return;
    }

    char line[512] = {0};
    char key[512] = {0};
    char value[512] = {0};
    int len = 0;

    while (fgets(line, sizeof(line), in)!=NULL)
    {
        len = (int)strlen(line)-1; 
        if (len<=0)
        {
            continue;
        }
        if (line[len]!='\n' && !feof(in))
        {
            fclose(in);
            return;
        }
        // 去掉尾部多余的换行和空格
        while ((len>=0) &&
                ((line[len]=='\n') || (isspace(line[len])))) {
            line[len]=0 ;
            len-- ;
        }
        if (len <= 0) {
            continue;
        }
        // 解析参数
        if (sscanf(line, "%[^:] : \"%[^\"]\"", key, value) == 2
            || sscanf(line, "%[^:] : \'%[^\']\'", key, value) == 2)
        {
            strstrip(key);
        }
        else if (sscanf(line, "%[^:] : %[^;#]", key, value) == 2)
        {
            strstrip(key);
            strstrip(value);
            if (!strcmp(value, "\"\"") || (!strcmp(value, "''")))
            {
                value[0] = 0;
            }
        }
        else if (sscanf(line, "%[^:] : %[;#]", key, value) == 2
                || sscanf(line, "%[^:] %[:]", key, value) == 2)
        {
            strstrip(key);
            value[0] = 0;
        }
        else
        {
            continue;
        }
        //printf("[key: %s] [value: %s]\n", key, value);
        if (strcmp(key, "WifiName") == 0)
        {
            snprintf(wifiName, sizeof(wifiName), "%s", value);
        }
        else if (strcmp(key, "Password") == 0)
        {
            snprintf(wifiPassword, sizeof(wifiPassword), "%s", value);
        }
        else if (strcmp(key, "PasswordType") == 0)
        {
            wifiPasswordType = (int)atoi(value);
        }
        else
        {
            printf("[%s %d] Unknown Key[%s]\n", __FUNCTION__, __LINE__, key);
        }
    }
    fclose(in);
    // 2.生成本地文件
    if (strlen(wifiName) == 0)
    {
        // 无WiFi名称, 配置无效
        return;
    }
    in = fopen("/tmp/SdTypeNetCfg.ini", "w");
    if (NULL == in)
    {
        return;
    }
    fprintf(in, "[INet_Cfg]\n");
    fprintf(in, "Wlan_SSID=%s\n", wifiName);
    fprintf(in, "Wlan_Key=%s\n", wifiPassword);
    fclose(in);
    
    return;
}

/*-****************************************************************** 
去掉字符串首尾的  空格,回车、换行 \r \n 字符
-------------------------- 
参数 pcInOutStr[inout]:输入输出字符串
-------------------------- 
返回 ：无
*******************************************************************-*/ 
static void __TrimSpace(char *pcInOutStr)
{
	char *pcStart = pcInOutStr - 1;
	char *pcEnd = pcInOutStr;
	char *pcInStr = pcInOutStr;

	while(*pcInStr)
	{
		switch(*pcInStr)
		{
			case ' ':
			case '\r':
			case '\n':
				if(pcStart + 1 == pcInStr)
				{
					pcStart = pcInStr;
				}
				break;

			default: 
				break;
		}
		++pcInStr;
	}

	//现在来到了字符串的尾部 反向向前
	--pcInStr;
	++pcStart;

	if(*pcStart == 0)
	{	
		//已经到字符串的末尾了
		*pcInOutStr = 0;
		
		return;
	}

	pcEnd = pcInStr + 1;
	while(pcInStr > pcStart)
	{
		switch(*pcInStr)
		{
			case ' ':
			case '\r':
			case '\n':
				if(pcEnd - 1 == pcInStr)
				{
					pcEnd = pcInStr;
				}
				break;

			default:   
				break;
		}
		--pcInStr;
	}
	
	memmove(pcInOutStr, pcStart, pcEnd - pcStart);
	*(pcInOutStr + (pcEnd - pcStart)) = 0;
}

#define SD_CUSTOM_ALL_PATH       		"/tmp/app/disk/input"
#define SD_CUSTOM_ALL_SPEECH_FILE       "SdUpdateAllAudio.ini"
#define CUSTOM_ALL_SPEECH_TAR_PATH      "/tmp/AllSpeech.tar"
#define SD_CUSTOMMODE_FILE				"custommode.ini"
#define CUSTOM_CUSTOMMODE_FILE_PATH		"/tmp/custommode.ini"

static int SdCheckAndCopyUpdateAudioFile()
{
    char cfg_file[128] = {0};
    snprintf(cfg_file, sizeof(cfg_file)-1, "%s/%s", SD_CUSTOM_ALL_PATH, SD_CUSTOM_ALL_SPEECH_FILE);

    if (access(cfg_file, F_OK) != 0)
    {
        return 0;
    }

    char buff[128]={0};
    FILE *fp = NULL;

    /* 从配置文件中获取语音压缩包名 */
    fp = fopen(cfg_file, "r");
    if (!fp)
    {
        fprintf(stderr, "fopen %s failed!\n", cfg_file);
        return -1;
    }

    if (NULL == fgets(buff, sizeof(buff), fp))
    {
        fprintf(stderr, "Fail to fgets %s\n", cfg_file);
        fclose(fp);
        return -1;
    }
    fclose(fp);

    __TrimSpace(buff);

    /* 检查语音压缩文件是否存在 */
    if (strlen(buff) <= 0)
    {
        fprintf(stderr, "Err: buff is zero\n");
        return -1;
    }

    char update_file[256] = {0};
    snprintf(update_file, sizeof(update_file)-1, "%s/%s", SD_CUSTOM_ALL_PATH, buff);

    if (0 != access(update_file, F_OK))
    {
        fprintf(stderr, "Err: %s file does not exist!\n", update_file);
        return -1;
    }

    /* 拷贝到临时目录 */
    char szCmd[256] = {0};
    snprintf(szCmd, sizeof(szCmd), "cp -rf %s %s", update_file, CUSTOM_ALL_SPEECH_TAR_PATH);
    mysystem(szCmd);

    return 0;
}

static int SdCheckAndCopyUpdateCustomFile()
{
    char cfg_file[128] = {0};
    snprintf(cfg_file, sizeof(cfg_file)-1, "%s/%s", SD_CUSTOM_ALL_PATH, SD_CUSTOMMODE_FILE);

    if (access(cfg_file, F_OK) != 0)
    {
        return 0;
    }
    char szCmd[256] = {0};
    snprintf(szCmd, sizeof(szCmd), "cp -rf %s %s", cfg_file, CUSTOM_CUSTOMMODE_FILE_PATH);
	printf("[CYADD]SdCheckAndCopyUpdateCustomFile [%s]\n",szCmd);
    mysystem(szCmd);
	
    return 0;
}


/* SD卡文件完整性检测 */
static int SdCardReadWriteTest(void)
{
    FDISK_DBG("Start to Sd Card Read and Write test!\n");

    if (access("/tmp/app/disk", F_OK))
    {
        mysystem("mkdir -p /tmp/app/disk");
    }

    int ret = 0;

    char md5_before[32] = {0};
    char md5_after[32]  = {0};

    /* Step 1: 挂载 */
    ret = MountvFatNode(SD_DEV_NODE_PART1, "/tmp/app/disk");
    if (ret)
    {
        FDISK_SYS("SD Card mount fail!\n");
        return -1;
    }

    /* Step 2: 写测试 */
	/* Step 2.1 先删除原来存在的测试文件(如果有)，然后创建新的文件 */
	mysystem("rm -rf /tmp/app/disk/test_file.txt");
    mysystem("echo test_file > /tmp/app/disk/test_file.txt");

    /* 如果测试文件不存在，则表示创建失败 */
    if (access("/tmp/app/disk/test_file.txt", F_OK))
    {
        FDISK_SYS("SD Card write fail!\n");
        goto TEST_FAIL;
    }

	/* Step 2.2.1 把SD卡里的配网文件*/
	/* 如果测试文件不存在，则表示创建失败 */
    /* Step 2.1.1 把SD卡里的配网文件拷到临时目录*/
    if (0 == access("/tmp/app/disk/WifiInfo.txt", F_OK))
    {
        // wifi简化配置格式 
        // WifiName:
        // Password:
        // PasswordType:
        wifi_info_translate2local("/tmp/app/disk/WifiInfo.txt");
    }
    else if (0 == access("/tmp/app/disk/SdTypeNetCfg.ini", F_OK))
    {
        mysystem("cp /tmp/app/disk/SdTypeNetCfg.ini /tmp/.");
    }

	/* Step 2.1.2 把SD卡里的电机自转使能文件拷到临时目录*/
    if (0 == access("/tmp/app/disk/ptzTestEn", F_OK))
    {
        mysystem("cp /tmp/app/disk/ptzTestEn /tmp/.");
    }
	
    /* Step 2.1.3 把SD卡里的老化测试项目文件拷到临时目录*/
    if (0 == access("/tmp/app/disk/aging_test.list", F_OK))
    {
        mysystem("cp /tmp/app/disk/aging_test.list /tmp/.");
    }

    if (0 == access("/tmp/app/disk/aging_test.g711u", F_OK))
    {
        mysystem("cp /tmp/app/disk/aging_test.g711u /tmp/.");
    }

    /* Step 2.1.4 把SD卡里的提示音升级文件拷到临时目录 */
    SdCheckAndCopyUpdateAudioFile();
	SdCheckAndCopyUpdateCustomFile();
	
	 /* Step 2.1.5 鎶奡D鍗￠噷鐨凱roductTest鏂囦欢鎷疯礉鍒颁复鏃剁洰褰?*/
	if (0 == access("/tmp/app/disk/output/ProductTest", F_OK))
    {
        mysystem("cp /tmp/app/disk/output/ProductTest /tmp/.");
    }

    /* Step 2.2 计算MD5值 */
    ret = ShellCmdOut("md5sum /tmp/app/disk/test_file.txt", md5_before, sizeof(md5_before));
    if (ret)
    {
        FDISK_SYS("md5sum test file fail!\n");
        goto TEST_FAIL;
    }

    /* Step 2.3 卸载分区 */
    ret = mysystem("umount /tmp/app/disk/");
    if (ret)
    {
        FDISK_SYS("SD Card umount fail!\n");
        mysystem("rm -rf /tmp/app/disk/test_file.txt");
        return -1;
    }

    /* Step 3: 读测试 */
    
    /* Step 3.1 重新挂载分区 */
    ret = MountvFatNode(SD_DEV_NODE_PART1, "/tmp/app/disk");
    if (ret)
    {
        FDISK_SYS("SD Card mount fail!\n");
        return -1;
    }

    /* Step 3.2 重新计算MD5值 */
    ret = ShellCmdOut("md5sum /tmp/app/disk/test_file.txt", md5_after, sizeof(md5_after));
    if (ret)
    {
        FDISK_ERR("md5sum test file fail!\n");
        goto TEST_FAIL;
    }
    
    mysystem("rm -rf /tmp/app/disk/test_file.txt");
    mysystem("umount /tmp/app/disk");

    /* Step 4: 对比两个MD5值的区别 */
    if (0 == strncasecmp(md5_after, md5_before, sizeof(md5_after)))
    {
        FDISK_INFO("Good! Sd Card test Ok!\n");
        return 0;
    }
    else
    {
        FDISK_ERR("Bad! Sd Card test Fail!\n");
        return -1;
    }

TEST_FAIL:
    mysystem("rm -rf /tmp/app/disk/test_file.txt");
    mysystem("umount /tmp/app/disk");

    return -1;
}

/* 检测SD卡读写属性 */
static int CheckSdCardRWAttr(void)
{
    if (access("/tmp/app/disk", F_OK))
    {
        FDISK_SYS("/tmp/app/disk dirent not exist!\n");
        return -1;
    }

    /* Step 1: 挂载 */
    if (MountvFatNode(SD_DEV_NODE_PART1, "/tmp/app/disk"))
    {
        FDISK_SYS("SD Card mount fail!\n");
        return -1;
    }

    /* 检查是否为可读写SD卡 */
    if (0 == IsStringInCommand(SD_CARD_IS_RW, "rw"))
    {
        FDISK_ERR("Check sd card kernel error message!\n");
        mysystem("umount /tmp/app/disk");
        return -1;
    }

    mysystem("umount /tmp/app/disk");
    FDISK_DBG("Sd card read and write check ok!\n");

    return 0;
}

//初始化重启标识
static void Fdisk_RebootFlag_init()
{
	char cmdstring[128];

	//设备因重新分区重启设备
    if (0 == access(FDISK_RESET_FLAG,F_OK))
    {
		g_FdiskRebootflag = 1;

		memset(cmdstring,0,sizeof(cmdstring));
		snprintf(cmdstring,sizeof(cmdstring)-1,"rm %s -rf",FDISK_RESET_FLAG);
		mysystem(cmdstring);
    }
}

//创建fdisk 重启标识
static void Fdisk_RebootFlag_Create()
{
	char cmdstring[128];

	memset(cmdstring,0,sizeof(cmdstring));
	snprintf(cmdstring,sizeof(cmdstring)-1,"touch %s",FDISK_RESET_FLAG);
	mysystem(cmdstring);
}


//检测错误信息
static int CheckDmsegInfo()
{
	char command[128] = {0};
	char result[128] = {0};
	int foundNum = 0;
	int ret = 0;

	memset(result, 0, sizeof(result));
	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), "dmesg > %s",ERR_TMP_FILE);	
	mysystem(command);
	
	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), "cat %s | grep -rn \"%s\" | wc -l",ERR_TMP_FILE,ERR_INFO);
	ret = ShellCmdOut(command, result, sizeof(result));

	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), "rm  %s",ERR_TMP_FILE);	
	mysystem(command);
	
	if(0 == ret)
	{
		foundNum = atoi(result);
		FDISK_DBG("Check dmesg info: %s found=%d\n", ERR_INFO, foundNum);
		if (foundNum >= ERR_INFO_COUNT)
		{
			return -1;
		}
	}
	else
	{
		FDISK_ERR("Run command : %s fail\n", command);
	}

	return 0;
}



//检测sd卡扇区是否可读
static int CheckSdCardRead()
{
	FDISK_DBG("CheckSdCardRead Start!\n");

	char command[128] = {0};
	struct timespec startTime, endTime;

	memset(&startTime, 0, sizeof(struct timeval));
    memset(&endTime, 0, sizeof(struct timeval));
	clock_gettime(CLOCK_MONOTONIC, &startTime);

	if (0 == access(SD_DEV_NODE_PART1,F_OK))
	{//存在分区设备，测试分区
		memset(command, 0, sizeof(command));
		snprintf(command, sizeof(command), "mount %s %s",SD_DEV_NODE_PART1,MOUNT_DIR);
		mysystem(command);

		memset(command, 0, sizeof(command));
		snprintf(command, sizeof(command), "umount %s",MOUNT_DIR);
		mysystem(command);
	}
	else if (0 == access(SD_DEV_NODE,F_OK))
	{//只有mmcblk0
		memset(command, 0, sizeof(command));
		snprintf(command, sizeof(command), "mount %s %s",SD_DEV_NODE,MOUNT_DIR);
		mysystem(command);

		memset(command, 0, sizeof(command));
		snprintf(command, sizeof(command), "umount %s",MOUNT_DIR);
		mysystem(command);
	}

	clock_gettime(CLOCK_MONOTONIC, &endTime);

	FDISK_DBG("Check Use Time %d\n",endTime.tv_sec - startTime.tv_sec);
	if ((endTime.tv_sec - startTime.tv_sec) > SECTOE_CHECK_TIMEOUT)
	{
		FDISK_ERR("SectorReadCheck Check Timeout %d !\n",endTime.tv_sec - startTime.tv_sec);
		return -1;
	}

	return 0;
}


/* 检测SD卡挂载 */
static int CheckSdCardMount(int bNeedDosfsck)
{
    int ret = 0;
    
RE_TEST:

    /* 1. 检测SD卡是否存在，即是否存在/dev/mmcblkx节点 */
#ifdef __USE_EMMC__
    if (0 == IsStringInCommand("ls /dev/mmcblk1*", SD_DEV_NODE))
#else
    if (0 == IsStringInCommand("ls /dev/mmcblk0*", SD_DEV_NODE))
#endif
    {
        FDISK_DBG("SD Card not exist!\n");
        WriteLog(SD_STATUS_CFG, gSdErrorMsg[SD_NO_NODE]);
        mysystem(REMOVE_CARD_NODE);
        mysystem(MY_DESCRIBE(eBootSdCardStatus, SdCardBootCheck_NoNode));
        return -1;
    }

    /* 2. 检测SD卡是否存在mmcblkx节点 */
#ifdef __USE_EMMC__
    if (0 == IsStringInCommand("ls /dev/mmcblk1*", SD_DEV_NODE_PART1))
#else
    if (0 == IsStringInCommand("ls /dev/mmcblk0*", SD_DEV_NODE_PART1))
#endif
    {
        if (has_formated)
        {
            FDISK_DBG("Check SD Card not exist after format!\n");
            WriteLog(SD_STATUS_CFG, gSdErrorMsg[SD_CARD_ERR]);
            mysystem(REMOVE_CARD_NODE);
            mysystem(MY_DESCRIBE(eBootSdCardStatus, SdCardBootCheck_NoNode));
            return -1;
        }
        else
        {
			if (g_FdiskRebootflag)
			{
				FDISK_DBG("Check SD Card not exist after fdisk and reboot device!\n");
				WriteLog(SD_STATUS_CFG, gSdErrorMsg[SD_CARD_ERR]);
				mysystem(REMOVE_CARD_NODE);
				mysystem(MY_DESCRIBE(eBootSdCardStatus, SdCardBootCheck_AddPartFail));
				return -1;
			}
			else
			{
				/*遇上mmcblk0p1节点不存在的卡,修复分区之后如果节点还不存在就定性为坏卡*/
				Fdisk_RebootFlag_Create();
				FDISK_DBG("SD Card %s not exist! need to partition!\n", SD_DEV_NODE_PART1);
				/*todo: ZXJ XT5H 海思平台有的卡会导致 ioctl(fd, BLKRRPART) 卡住超过1分钟以上*/
				ret = fdisk(SD_DEV_NODE, 1);
				if (ret)
				{
					FDISK_ERR("SD Card %s partition fail! exit!\n", SD_DEV_NODE);
					WriteLog(SD_STATUS_CFG, gSdErrorMsg[SD_ADD_PART_ERR]);
					mysystem(REMOVE_CARD_NODE);
					mysystem(MY_DESCRIBE(eBootSdCardStatus, SdCardBootCheck_AddPartFail));
					return -1;
				}
				FDISK_DBG("SD Card %s partition successful! next reboot device!\n", SD_DEV_NODE);
				sleep(1);
				mysystem("reboot -f");
				return 0;
			}

        }
    }

    //若本次启动需要程序做格式化，则先格式化处理
    if ((SD_ACTION_AT_START_FORMAT == g_sdActionAtStart) && (!has_formated))
    {
        FDISK_ERR("Format sd card! respond action from %s!\n", SD_ACTION_AT_START_FILE);
        has_formated = 1;
        
	    #ifndef __EXFAT_SUPPORT__  //不支持exfat的项目
    	//格式化SD卡接口
		CheckSdCardUseMkfsVfat();
    	mysystem(CLEAR_DMSEG_INFO);
        if (-1 == Change_DevFileSystemType(SD_DEV_NODE, 1, FS_FAT32_VALUE))
    	{
    		WriteLog(SD_STATUS_CFG, "Change partition file system failed\n");
    	}
    	#else
    	GetFileSystemInfo(SD_DEV_NODE_PART1);
    	if (sd_size <= DISK_SIZE_32G)
    	{
    	    //格式化SD卡接口
			CheckSdCardUseMkfsVfat();
    	    mysystem(CLEAR_DMSEG_INFO);
            if (-1 == Change_DevFileSystemType(SD_DEV_NODE, 1, FS_FAT32_VALUE))
        	{
        		WriteLog(SD_STATUS_CFG, "Change partition file system failed\n");
        	}
    	}
    	else
    	{
    	    mysystem(FORMAT_SD_PART1_EXFAT);
    	    mysystem(CLEAR_DMSEG_INFO);
            if (-1 == Change_DevFileSystemType(SD_DEV_NODE, 1, FS_EXFAT_VALUE))
        	{
        		WriteLog(SD_STATUS_CFG, "Change partition file system failed\n");
        	}
    	}
    	#endif        
    }

#if 0
    /* 3. 进行dosfsck如果有需要 */
    if (bNeedDosfsck)
    {
        pthread_t minitor_id;
        pthread_create(&minitor_id, NULL, MonitorDosfsckTimeOut, NULL);
        sem_post(&g_semWakeThread);
        g_dosfsck_status = mysystem(DOSFSCK_SD_PART1);
        sleep(1);
    }
#endif

    /* 4. 存在mmcblkx节点,则进行挂载测试 */
    if (access("/tmp/app/disk", F_OK))
    {
        mysystem("mkdir -p /tmp/app/disk");
    }

    /* 5. 进行挂载测试 */
    ret = MountvFatNode(SD_DEV_NODE_PART1, "/tmp/app/disk");
    if (ret)
    {
        if (has_formated)
        {
            FDISK_ERR("SD Card mount fail! no sd card!\n");
            WriteLog(SD_STATUS_CFG, gSdErrorMsg[CARD_MOUNT_ERR]);            
            mysystem(REMOVE_CARD_NODE);
            mysystem(MY_DESCRIBE(eBootSdCardStatus, SdCardBootCheck_MountFail));
            return -1;
        }
        else
        {
            FDISK_ERR("SD Card mount fail! need format sd card! format sd card!\n");
            has_formated = 1;
            #ifndef __EXFAT_SUPPORT__  //不支持exfat的项目
			//格式化SD卡接口
			CheckSdCardUseMkfsVfat();
            mysystem(CLEAR_DMSEG_INFO);
            if (-1 == Change_DevFileSystemType(SD_DEV_NODE, 1, FS_FAT32_VALUE))
			{
				WriteLog(SD_STATUS_CFG, "Change partition file system failed\n");
			}
        	#else
        	GetFileSystemInfo(SD_DEV_NODE_PART1);
        	if (sd_size <= DISK_SIZE_32G)
        	{
        	    //格式化SD卡接口
				CheckSdCardUseMkfsVfat();
                mysystem(CLEAR_DMSEG_INFO);
                if (-1 == Change_DevFileSystemType(SD_DEV_NODE, 1, FS_FAT32_VALUE))
    			{
    				WriteLog(SD_STATUS_CFG, "Change partition file system failed\n");
    			}
        	}
        	else
        	{
        	    mysystem(FORMAT_SD_PART1_EXFAT);
                mysystem(CLEAR_DMSEG_INFO);
                if (-1 == Change_DevFileSystemType(SD_DEV_NODE, 1, FS_EXFAT_VALUE))
    			{
    				WriteLog(SD_STATUS_CFG, "Change partition file system failed\n");
    			}
        	}
        	#endif
            
            WriteLog(SD_STATUS_CFG, gSdErrorMsg[INIT_FORMAT]);
            goto RE_TEST;
        }
    }

    CheckSdCardIsProduct();

    mysystem("umount /tmp/app/disk");

    ret = CheckSdCardUseFsckFat();
    if (ret)
    {
		goto RE_TEST;
    }

    FDISK_DBG("Sd card mount check ok!\n");

    return 0;
}

/* 检查SD卡的相关属性 */
static int CheckSdCardStatus(void)
{
    /* 1. 第一次检查Dmesg信息 */
    if (RunOperateCommand())
    {
        FDISK_ERR("First check sd card kernel error message!\n");
        WriteLog(SD_STATUS_CFG, gSdErrorMsg[FIRST_DSG_EXP]);
        if(has_formated)
        {
            mysystem(MY_DESCRIBE(eBootSdCardStatus, SdCardBootCheck_DiskExp));
        }
        return SdCardBootCheck_DiskExp;
    }

    /* 2. 检查是否为可读写SD卡 */
    if (CheckSdCardRWAttr())
    {
        FDISK_ERR("Check sd card read and write error!\n");
        WriteLog(SD_STATUS_CFG, gSdErrorMsg[CARD_RW_EXP]);  
        if(has_formated)
        {
            mysystem(MY_DESCRIBE(eBootSdCardStatus, SdCardBootCheck_ReadWriteFail));
        }
        return SdCardBootCheck_ReadWriteFail;
    }

    /* 3. 检查SD卡文件完整性 */
    if (SdCardReadWriteTest())
    {
        FDISK_ERR("Sd Card read write fail!\n");
        WriteLog(SD_STATUS_CFG, gSdErrorMsg[FILE_MD5_EXP]);
        if(has_formated)
        {
            mysystem(MY_DESCRIBE(eBootSdCardStatus, SdCardBootCheck_ReadWriteFail));
        }
        return SdCardBootCheck_ReadWriteFail;
    }

    /* 4. 第二次检查Dmesg信息 */
    if (RunOperateCommand())
    {
        FDISK_ERR("Second check sd card kernel error message!\n");
        WriteLog(SD_STATUS_CFG, gSdErrorMsg[SEC_DSG_EXP]);
        if(has_formated)
        {
            mysystem(MY_DESCRIBE(eBootSdCardStatus, SdCardBootCheck_DiskExp));
        }
        return SdCardBootCheck_DiskExp;
    }

    /* 5. SD卡检测成功 */
    FDISK_INFO("Sd Card is normal(rw)! Checking successful!\n");
    mysystem(MY_DESCRIBE(eBootSdCardStatus, SdCardBootCheck_DiskIsNormal));
    WriteLog(SD_STATUS_CFG, gSdErrorMsg[SD_CARD_NORMAL]);

    return 0;
}

int main(int argc, const char *argv[])
{
    if (argc != 2)
    {
        FDISK_ERR("Usage: %s bNeedForceDosfsck<0|1>\n", argv[0]);
        return -1;
    }
	//使用mkfs.vfat 对sd卡进行格式化,创建超时处理线程
	pthread_t minitor_mainid;
	g_CheckSDcard_status = -1;
	pthread_create(&minitor_mainid, NULL, MonitorCheckSDcardTimeOut, NULL);

    product_type = atoi(argv[1]);

    //检查日志文件是否有超大小
    checkSDLogFileSize();

    //获取主应用上次运行时记录的需要在这次设备启动中对卡的操作
    GetSDActionAtStart();

    Fdisk_RebootFlag_init();

    mysystem("rm -rf /tmp/.sd_status");

    sem_init(&g_semWakeThread, 0, 0);
    sem_init(&g_semdomkfsThread, 0, 0);

	if ((CheckSdCardRead()))
	{
		mysystem(MY_DESCRIBE(eBootSdCardStatus, SdCardBootCheck_AddPartFail));
		//退出checkSDcard进程则将标志位置为0，退出检测线程
		g_CheckSDcard_status = 0;
		return -1;
	}

    if (CheckSdCardMount(atoi(argv[1])))
    {
        FDISK_ERR("Check sd card mount fail!\n");
		//退出checkSDcard进程则将标志位置为0，退出检测线程
		g_CheckSDcard_status = 0;

        return -1;
    }

CHECK_SD_STATUS:
    if (CheckSdCardStatus())
    {
        FDISK_ERR("Check sd card status fail!\n");
        //如果未做过格式化则对卡做一下格式化再检测
        if (0 == has_formated)
        {
            FDISK_ERR("Try to format, and check again!\n");
            has_formated = 1;
			
			#ifndef __EXFAT_SUPPORT__  //不支持exfat的项目
        	//格式化SD卡接口
			CheckSdCardUseMkfsVfat();
            if (-1 == Change_DevFileSystemType(SD_DEV_NODE, 1, FS_FAT32_VALUE))
			{
				WriteLog(SD_STATUS_CFG, "Change partition file system failed\n");
			}
        	#else
        	GetFileSystemInfo(SD_DEV_NODE_PART1);
        	if (sd_size <= DISK_SIZE_32G)
        	{
        	    //格式化SD卡接口
				CheckSdCardUseMkfsVfat();
                if (-1 == Change_DevFileSystemType(SD_DEV_NODE, 1, FS_FAT32_VALUE))
    			{
    				WriteLog(SD_STATUS_CFG, "Change partition file system failed\n");
    			}
        	}
        	else
        	{
        	    mysystem(FORMAT_SD_PART1_EXFAT);
                if (-1 == Change_DevFileSystemType(SD_DEV_NODE, 1, FS_EXFAT_VALUE))
    			{
    				WriteLog(SD_STATUS_CFG, "Change partition file system failed\n");
    			}
        	}
        	#endif
            mysystem(CLEAR_DMSEG_INFO);
            WriteLog(SD_STATUS_CFG, "SdCard format!\n");
            goto CHECK_SD_STATUS;
        }
		//退出checkSDcard进程则将标志位置为0，退出检测线程
		g_CheckSDcard_status = 0;
        return -1;
    }
	//退出checkSDcard进程则将标志位置为0，退出检测线程
	g_CheckSDcard_status = 0;
    return 0;
}
