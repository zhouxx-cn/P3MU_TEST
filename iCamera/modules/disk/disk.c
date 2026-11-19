
/*!
*\file   disk.c 
*\brief  磁盘的操作
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/

#include <stdio.h>
#include <linux/kdev_t.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <unistd.h>
#include <sys/wait.h>
#include <scsi/scsi_ioctl.h>
#include <scsi/scsi.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/prctl.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */

#ifdef TEST_MEM_INFO
#define MODULE_NAME "disk"
#include "MemoryInterface.h"
#endif

#include "macro_pthread.h"
#include "disk.h"
#include "fdisk.h"
#include "mysystem.h"
#include "../include/sys_banned.h"
#include "global_macro.h"

#define DEBUG_SHOW_WRITEFILE_BITRATE

/**************************************************************************
*                            常量                                        *
**************************************************************************/

/**************************************************************************
*                            宏                                          *
**************************************************************************/

#define CALLBACK

#ifndef MAX_SDCARD_NUM
#ifdef __USE_EMMC__
#define MAX_SDCARD_NUM		('9' - '0' + 1)//('9' - '0' + 1)	//支持查找 mmcblk0 mmcblk1
#else
#define MAX_SDCARD_NUM		('0' - '0' + 1)//('9' - '0' + 1)	//!< only 1 SD卡
#endif
#endif

#ifndef MAX_USB_DISK_NUM
#define MAX_USB_DISK_NUM	('z' - 'a' + 1)	//!< only 1 U盘
#endif

#define SYS_TIMEOUT (-2)

#define DEV_PATH     "/dev/mmcblk"
#define DEV_PATH_FORMAT   "/dev/mmcblk%d"

#define DISK_STATUS_FLAG   "/tmp/.sd_status"

/**************************************************************************
*                          数据类型                                       *
**************************************************************************/

struct mount_part
{
	char dev[20];				//!< 设备节点名
	char mount_point[40];		//!< 挂载点位置
	char fs_type[20];			//!< 文件系统类型
	char read_write[100];		//!< 挂载点读写属性
	struct mount_part *next;	//!< 下一挂载点指针
};

struct sector
{
	struct sector *next;		//!< 下一扇区指针
	unsigned long sectornumber;	//!< 扇区序号
	int to_be_written;			//!< 写标记
	char data[512];				//!< 扇区数据
};



/**************************************************************************
*                           全局变量                                      *
**************************************************************************/

static pthread_mutex_t makePartLock = PTHREAD_MUTEX_INITIALIZER;
//#define PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP   {0, 0, 0, PTHREAD_MUTEX_RECURSIVE_NP, __LOCK_INITIALIZER}
//static pthread_mutex_t makePartLock = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

pthread_mutex_t m_RecCap;   //录像和抓图的互斥锁
static pthread_t thread_ReadOnlyCheck;
static int disk_readonly_first = 0;
int Hotplug_flag = 0;

/**************************************************************************
*                           局部函数原型                                  *
 **************************************************************************/

extern int fdisk(const char *hd_device, int parts);	// 在fdisk.c中实现，对硬盘实现分区

static void *disk_readonly();

extern int GetPartitionInfo(const char *dev, struct Disk_Desc *ptDiskDesc);


/**************************************************************************
*                           方法实现                                      *
**************************************************************************/

#include <scsi/sg.h>
#include <scsi/scsi.h>

///Used to get disk serial no
#define SG_CHECK_CONDITION    0x02
#define SG_DRIVER_SENSE        0x08
#define SG_ATA_16        0x85
#define SG_ATA_16_LEN        16
#define SG_ATA_LBA48        1
#define SG_ATA_PROTO_NON_DATA    ( 3 << 1)
#define SG_ATA_PROTO_PIO_IN    ( 4 << 1)
#define SG_ATA_PROTO_PIO_OUT    ( 5 << 1)
#define SG_ATA_PROTO_DMA    ( 6 << 1)
#define SG_ATA_PROTO_UDMA_IN    (11 << 1)	/* not yet supported in libata */
#define SG_ATA_PROTO_UDMA_OUT    (12 << 1)	/* not yet supported in libata */

#define ATA_USING_LBA        (1 << 6)

enum
{
	ATA_OP_CHECKPOWERMODE1 = 0xe5,
	ATA_OP_CHECKPOWERMODE2 = 0x98,
	ATA_OP_DOORLOCK = 0xde,
	ATA_OP_DOORUNLOCK = 0xdf,
	ATA_OP_FLUSHCACHE = 0xe7,
	ATA_OP_FLUSHCACHE_EXT = 0xea,
	ATA_OP_IDENTIFY = 0xec,
	ATA_OP_PIDENTIFY = 0xa1,
	ATA_OP_SECURITY_DISABLE = 0xf6,
	ATA_OP_SECURITY_ERASE_PREPARE = 0xf3,
	ATA_OP_SECURITY_ERASE_UNIT = 0xf4,
	ATA_OP_SECURITY_FREEZE_LOCK = 0xf5,
	ATA_OP_SECURITY_SET_PASS = 0xf1,
	ATA_OP_SECURITY_UNLOCK = 0xf2,
	ATA_OP_SETFEATURES = 0xef,
	ATA_OP_SETIDLE1 = 0xe3,
	ATA_OP_SETIDLE2 = 0x97,
	ATA_OP_SLEEPNOW1 = 0xe5,
	ATA_OP_SLEEPNOW2 = 0x99,
	ATA_OP_SMART = 0xb0,
	ATA_OP_STANDBYNOW1 = 0xe0,
	ATA_OP_STANDBYNOW2 = 0x94,
};

enum
{
	SG_CDB2_TLEN_NODATA = 0 << 0,
	SG_CDB2_TLEN_FEAT = 1 << 0,
	SG_CDB2_TLEN_NSECT = 2 << 0,

	SG_CDB2_TLEN_BYTES = 0 << 2,
	SG_CDB2_TLEN_SECTORS = 1 << 2,

	SG_CDB2_TDIR_TO_DEV = 0 << 3,
	SG_CDB2_TDIR_FROM_DEV = 1 << 3,

	SG_CDB2_CHECK_COND = 1 << 5,
};

typedef enum
{
    FS_FAT32,
    FS_EXFAT,
    FS_NTFS,
    FS_EXT2,
    FS_ERROR_TYPE,
}E_FILE_SYS_TYPE;

#define START_SERIAL 10			/* ASCII serial number */
#define LENGTH_SERIAL 10		/* 10 words (20 bytes or characters) */

static int get_sdcard_count(void);

//获取SD卡Sys文件系统设备节点名
static void Get_SDCard_SysPath(int no, char *syspath, int len)
{
	memset(syspath, 0, len);

	snprintf(syspath, len, "/sys/block/mmcblk%d", no);
}

/*
*@breif CheckDiskRWIsDamage 从标记文件里面获取SD卡是否出现读写异常(该状态在系统启动的时候传入)
 *@return   返回0:TF卡为读写异常
 *@         返回1:TF卡为其他异常
 *@         返回-1: TF卡状态读取错误
 *@note  不修改之前的CheckDiskDamage判断，新增一个对于已经出现TF卡读写异常的判断
            SdCardBootCheck_ReadWriteFail = 4,读写测试失败(此时能挂载但读写异常)
*/
int CheckDiskRWIsDamage(void)
{
    if (access(DISK_STATUS_FLAG, F_OK))
    {
        fprintf(stderr, "Disk status file %s not exist!\n", DISK_STATUS_FLAG);
        return -1;
    }

    int status = 0;
    FILE *fp = NULL;
    fp = fopen(DISK_STATUS_FLAG, "r");
    if (!fp)
    {
        fprintf(stderr, "Fail to fopen %s: %s\n", DISK_STATUS_FLAG, strerror(errno));
        return -1;
    }

    char buff[4] = {0};
    if (NULL == fgets(buff, sizeof(buff), fp))
    {
        fprintf(stderr, "Fail to fgets %s: %s\n", DISK_STATUS_FLAG, strerror(errno));
        fclose(fp);
        fp = NULL;
        return -1;
    }

    fclose(fp);
    fp = NULL;

    status = atoi(buff);

    if (4 == status)
    {
        fprintf(stderr, "Disk check status: %d ==  ReadWriteFail\n", status);
        return 0;
    }
    else
    {    
        return 1;
    }    
}

/*@breif CheckDiskDamage 从标记文件里面获取SD卡是否损坏(该状态在系统启动的时候传入)
 *@return 返回1表示SD卡无卡或正常
 *@      返回0表示SD卡异常
 *@      返回-1表示参数错误
 *@note  针对异常的SD卡，如果访问会出现塞住的情况
 */
static int CheckDiskDamage(void)
{
    if (access(DISK_STATUS_FLAG, F_OK))
    {
        fprintf(stderr, "Disk status file %s not exist!\n", DISK_STATUS_FLAG);
        return -1;
    }

    int status = 0;
    FILE *fp = NULL;
    fp = fopen(DISK_STATUS_FLAG, "r");
    if (!fp)
    {
        fprintf(stderr, "Fail to fopen %s: %s\n", DISK_STATUS_FLAG, strerror(errno));
        return -1;
    }

    /* 
        SdCardBootCheck_NoNode        = 0,
        SdCardBootCheck_MountFail     = 1,
        SdCardBootCheck_DiskExp       = 2,
        SdCardBootCheck_AddPartFail   = 3,
        SdCardBootCheck_ReadWriteFail = 4,
        SdCardBootCheck_DiskIsNormal  = 5,
    */
    char buff[4] = {0};
    if (NULL == fgets(buff, sizeof(buff), fp))
    {
        fprintf(stderr, "Fail to fgets %s: %s\n", DISK_STATUS_FLAG, strerror(errno));
        fclose(fp);
        fp = NULL;
        return -1;
    }

    fclose(fp);
    fp = NULL;

    status = atoi(buff);

    if (5 != status)
    {
        fprintf(stderr, "Disk check error: %d\n", status);
        return 0;
    }
    else
    {    
        return 1;
    }
}




//获取设备Sys文件系统设备节点名
static void Get_Disk_SysPath(const char *dev, char *syspath, int len)
{
	memset(syspath, 0, len);

	snprintf(syspath, len, "/sys/block/%s", dev + 5);
}

/*!
 * \brief
 * 该函数用于判断指定硬盘是否还可以访问
 *
 * \param dev
 * 设备文件名.
 *
 * \return
 * 成功返回0，失败返回－1
 */
static int CheckDiskUsable(char *szDevice) __attribute__((unused));
static int TestMountPointIsWR(char *MountPoint) __attribute__((unused));

static int CheckDiskUsable(char *szDevice)
{
	int fd = 0;
	unsigned char args[512] = { 0 };
	void *data = (void *) (args);
	unsigned int data_bytes = 512;
	unsigned char cdb[SG_ATA_16_LEN] = { 0 };
	unsigned char sb[32], *desc;
	unsigned char ata_status;
	struct sg_io_hdr io_hdr;
	char acDev[10];

	if (NULL == szDevice)
	{
		printf("[disk]%s: Param is NULL!\n", __FUNCTION__);
		return -1;
	}

	memset(args, 0, sizeof (args));
	memset(acDev, 0, sizeof (acDev));

	strncpy(acDev, szDevice, sizeof(acDev) - 1);

	//打开设备
	fd = open(szDevice, O_RDONLY);
	if (fd < 0)
	{
		printf("[disk]%s: open %s error\n", __FUNCTION__, szDevice);
		return -1;
	}
	//设置cmdp
	cdb[0] = SG_ATA_16;
	cdb[1] = SG_ATA_PROTO_PIO_IN;
	cdb[2] = SG_CDB2_CHECK_COND;
	cdb[2] |= SG_CDB2_TLEN_NSECT | SG_CDB2_TLEN_SECTORS;
	cdb[2] |= SG_CDB2_TDIR_FROM_DEV;
	cdb[13] = ATA_USING_LBA;
	cdb[14] = ATA_OP_IDENTIFY;
	//设置sdp
	memset(&(sb[0]), 0, sizeof (sb));
	//设置sg_io_hdr结构
	memset(&io_hdr, 0, sizeof (struct sg_io_hdr));
	io_hdr.interface_id = 'S';
	io_hdr.cmd_len = SG_ATA_16_LEN;
	io_hdr.mx_sb_len = sizeof (sb);
	io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;
	io_hdr.dxfer_len = data_bytes;
	io_hdr.dxferp = data;
	io_hdr.cmdp = cdb;
	io_hdr.sbp = sb;
	io_hdr.timeout = 10000;		/* msecs */
	//调用ioctl
	if (ioctl(fd, SG_IO, &io_hdr) == -1)
	{
		printf("[disk]SG_IO ioctl not supported!\n");
		close(fd);
		return -1;				/* SG_IO not supported */
	}
	close(fd);
	//检查sg_io_hdr中的状态值
	if (io_hdr.host_status || io_hdr.driver_status != SG_DRIVER_SENSE || (io_hdr.status && io_hdr.status != SG_CHECK_CONDITION))
	{
		printf("[disk]io_hdr status err!\n");
		return -1;
	}
	//检查sdp中的状态值
	if (sb[0] != 0x72 || sb[7] < 14)
	{
		printf("[disk]io_hdr status sb err!\n");
		return -1;
	}
	desc = sb + 8;
	if (desc[0] != 9 || desc[1] < 12)
	{
		printf("[disk]io_hdr status desc err!\n");
		return -1;
	}
	ata_status = desc[13];
	if (ata_status & 0x01)
	{							/* ERR_STAT */
		printf("[disk]io_hdr status ata_status err!\n");
		return -1;
	}
	return 0;
}

static int TestMountPointIsWR(char *MountPoint)
{
	char buff[100];
	int ret = 0;
	FILE *file;

	snprintf(buff, sizeof(buff), "mkdir -p %s/videoout", MountPoint);
	mysystem(buff);
	snprintf(buff, sizeof(buff), "%s/videoout/.testfile", MountPoint);
	file = fopen(buff, "wb");
	if (NULL == file)
	{
		DISK_ERR("MountPoint = %s is not wr\n", MountPoint);
		return 0;
	}

	ret = fwrite(buff, 1, sizeof (buff), file);
	fclose(file);
	remove(buff);
	if (ret == sizeof (buff))
	{
		return 1;
	}

	DISK_ERR("MountPoint = %s is not wr\n", MountPoint);
	return 0;
}


//!\brief       根据磁盘索引获取设备节点名
//!\param   int index       硬盘编号(从0开始，到硬盘总数-1)
//!\param   char *dev       返回磁盘设备节点名
//!\param   int len     缓冲区大小
//!\return  成功返回0，失败返回-1.
int EDVR_HD_Index2DevNode(int index, char *dev, int len)
{
    if (!CheckDiskDamage())
    {
        printf("[%s-%d] Check disk is damaged!\n", __func__, __LINE__);
        return -1;
    }

	DIR *dp;

	int no, count = 0;

	char syspath[40];

	if ((NULL == dev) || (len <= 0))
	{
		DISK_ERR("(NULL == dev) || (len <= 0)\n");
		return -1;
	}

	memset(dev, 0, len);

	if (index < 0)
	{
		return -1;
	}

	//检测SD卡
	for (no = 0; no < MAX_SDCARD_NUM; ++no)
	{
		Get_SDCard_SysPath(no, syspath, sizeof (syspath));

		if ((dp = opendir(syspath)) == NULL)
		{
			DPRINTF("can't open directory: %s\n", syspath);
			continue;
		}

		++count;
		closedir(dp);

		if (index < count)
		{
			//获取SD卡设备节点
			snprintf(dev, len, DEV_PATH_FORMAT, no);
			return 0;
		}
	}

	return -1;
}

//获取分区设备节点
void Get_Partition_DevNode(const char *dev, int no_part, char *dev_part, int len)
{
	memset(dev_part, 0, len);

	if (IS_SDCARD(dev))
	{
		snprintf(dev_part, len, "%sp%d", dev, no_part);
	}
	else
	{
		snprintf(dev_part, len, "%s%d", dev, no_part);
	}
}

//!\brief       根据设备节点名获取挂载点
//!\param   const char *pszDevNode      磁盘或分区的设备节点名
//!\param   int nPartitionIndex         分区号，从1开始
//!\param   char *pszMountPoint         返回挂载点
//!\param   int nBufLen                 缓冲区大小
//!\return  成功返回0，失败返回-1.
int EDVR_HD_GetMountPoint(const char *pszDevNode, char *pszMountPoint, int nBufLen)
{
	DiskActType eType;

	memset(pszMountPoint, 0, nBufLen);

	eType = EDVR_HD_GetDiskType(pszDevNode);
	if (DISKACT_SDCARD == eType)
	{
		snprintf(pszMountPoint, nBufLen, "%s%s", MOUNTPOINT, pszDevNode + 5);
	}
	else
	{
		DISK_ERR("invalid dev node: %s\n", pszDevNode);
		return -1;
	}

	return 0;
}

//!\brief       根据磁盘设备节点名获取挂载点
//!\param   const char *pszDiskDevNode  磁盘设备节点名
//!\param   int nPartitionIndex         分区号，从1开始
//!\param   char *pszMountPoint         返回挂载点
//!\param   int nBufLen                 缓冲区大小
//!\return  成功返回0，失败返回-1.
int EDVR_HD_GetPartitionMountPoint(const char *pszDiskDevNode, int nPartitionIndex, char *pszMountPoint, int nBufLen)
{
	DiskActType eType;

	memset(pszMountPoint, 0, nBufLen);

	eType = EDVR_HD_GetDiskType(pszDiskDevNode);

	if (DISKACT_SDCARD == eType)
	{
		snprintf(pszMountPoint, nBufLen, "%s%sp%d", MOUNTPOINT, pszDiskDevNode + 5, nPartitionIndex);
	}
	else
	{
		DISK_ERR("invalid dev node: %s\n", pszDiskDevNode);
		return -1;
	}

	return 0;
}

//获取SD卡设备所以
static int Get_Disk_DevIndex(const char *dev)
{
	int no = -1;

	if (NULL != dev)
	{
		if (IS_SDCARD(dev))
		{
			if (dev[11] <= '9')
			{
				no = dev[11] - '0';	// DEV_PATH"0"
			}
		}
	}

	return no;
}

//!\brief       判断给定设备节点是否有效
//!\param   const char *dev 设备节点，如/dev/sda
//!\return  是有效磁盘返回1，否则返回0.
int EDVR_HD_IsValidDisk(const char *dev)
{
	if (Get_Disk_DevIndex(dev) < 0)
	{
		return 0;
	}

	return 1;
}

//获取SD卡分区索引
static int Get_Partition_DevIndex(const char *dev)
{
	const char *p;

	if (IS_SDCARD_PARTITION(dev))
	{
		p = dev + 13;			// DEV_PATH"0p1"
	}
	else
	{
		return -1;
	}

	return atoi(p);
}

//!\brief       根据分区挂载点，返回磁盘设备节点名
//!\param   const char *pszMountPoint       分区挂载点
//!\param   char *pszDiskDevNode                返回磁盘设备节点名
//!\param   int nBufLen                 缓冲区大小
//!\return  成功返回0，失败返回-1.
int EDVR_HD_GetDiskDevNode(const char *pszMountPoint, char *pszDiskDevNode, int nBufLen)
{
	if (IS_SDCARD_MOUNTPOINT(pszMountPoint))
	{
#ifdef USE_SD_PKG
FILE *tmp;

//测试硬盘是否已经UMount成功，如果没有成功，返回-1
tmp = fopen("/proc/mounts", "r");
if (NULL == tmp)
{
	DISK_ERR("can't open file /proc/mounts\n");
	perror("fopen");
	return 0;
}
while (!feof(tmp))
{
	char buf[256];

	char *mountstring;

	mountstring = fgets(buf, sizeof (buf), tmp);

	if (NULL != mountstring && NULL != strstr(mountstring, pszMountPoint))
	{
		//snprintf(pszDiskDevNode, nBufLen, "/dev/%s",  );
		memset(pszDiskDevNode,0,nBufLen);
		memcpy(pszDiskDevNode,mountstring,14);
		fclose(tmp);
		return 0;
	}
}

fclose(tmp);
perror("no mount!\n");
return -1;
#else
		snprintf(pszDiskDevNode, nBufLen, "/dev/%s", pszMountPoint + 5);
		memset(pszDiskDevNode + 12, 0, nBufLen - 12);
#endif
	}
	else
	{
		memset(pszDiskDevNode, 0, nBufLen);
		return -1;
	}

	return 0;
}


//!\brief       判断分区上是否存在文件被打开
//!\param   char *mountpoint    挂载点
//!\return  有文件被打开为1，无为0
static int hasfileopen(const char *mountpoint)
{
	pid_t pid_dvr;

	char fdpath[120];

	DIR *dp;

	struct dirent *entry;

	struct stat statbuf;

	int ret;

	pid_dvr = getpid();
	snprintf(fdpath, sizeof (fdpath), "/proc/%d/fd", (int) pid_dvr);

	if ((dp = opendir(fdpath)) == NULL)
	{
		DISK_ERR("can't open directory: %s\n", fdpath);
		return 0;
	}

	ret = 0;
	while ((entry = readdir(dp)) != NULL)
	{
		char procfile[256];

		snprintf(procfile, sizeof (procfile), "%s/%.100s", fdpath, entry->d_name);

		lstat(procfile, &statbuf);

		if (S_ISLNK(statbuf.st_mode))
		{
			char name[256];
			int len;
			char disk_name[256] = {0};

			len = readlink(procfile, name, sizeof (name) - 1);
			if (len >= 0)
			{
				if (len > (sizeof (name) - 1))
				{
					name[sizeof (name) - 1] = '\0';
				}
				else
				{
					name[len] = '\0';
				}

				/* 去掉前面的/dev */
				memcpy(disk_name, name + 4, 256 - 4);
				
				if (strstr(name, mountpoint))
				{
					if ((name == strstr(name, mountpoint)) || (!strncmp(disk_name, strstr(name, mountpoint), sizeof(disk_name))))
					{
						//dprintf("find file %s -> %s\n", procfile, name);

						ret = 1;
						break;
					}
				}				
			}
		}
	}

	closedir(dp);
#ifdef USE_SD_PKG
if(1 == ret)
{
	ZW_PkgSd_GetDiskWriteStatus(&ret);
}
#endif
	
	return ret;
}

//!\brief       判断设备是否被挂载
//!\param   char *dev    挂载点或设备节点
//!\return  挂载为1，无为0
static int hasdevmount(const char *dev)
{
	FILE *tmp;

	//测试硬盘是否已经UMount成功，如果没有成功，返回-1
	tmp = fopen("/proc/mounts", "r");
	if (NULL == tmp)
	{
		DISK_ERR("can't open file /proc/mounts\n");
		perror("fopen");
		return 0;
	}

	while (!feof(tmp))
	{
		char buf[256];

		char *mountstring;

		mountstring = fgets(buf, sizeof (buf), tmp);

		if (NULL != mountstring && NULL != strstr(mountstring, dev))
		{
			dprintf("%s\n", mountstring);
			dprintf("%s is mounted\n", dev);

			fclose(tmp);
			return 1;
		}
	}

	fclose(tmp);
	return 0;
}

//!\brief       关闭挂载点上所有被打开的文件
//!\param   char *mountpoint    挂载点
//!\return  成功为0,失败为-1
static int closedevfile(const char *mountpoint)
{
	pid_t pid_dvr;

	char fdpath[120];

	DIR *dp;

	struct dirent *entry;

	struct stat statbuf;

	pid_dvr = getpid();
	snprintf(fdpath, sizeof (fdpath), "/proc/%d/fd", (int) pid_dvr);

	if ((dp = opendir(fdpath)) == NULL)
	{
		DISK_ERR("can't open directory: %s\n", fdpath);
		return -1;
	}

	while ((entry = readdir(dp)) != NULL)
	{
		char procfile[256];

		snprintf(procfile, sizeof (procfile), "%s/%.100s", fdpath, entry->d_name);

		lstat(procfile, &statbuf);

		if (S_ISLNK(statbuf.st_mode))
		{
			char name[256];

			int len;

			len = readlink(procfile, name, sizeof (name) - 1);
			if (len >= 0)
			{
				if (len > (sizeof (name) - 1))
				{
					name[sizeof (name) - 1] = '\0';
				}
				else
				{
					name[len] = '\0';
				}

				if (name == strstr(name, mountpoint))
				{
					int fd = atoi(entry->d_name);

					close(fd);
					dprintf("close file: %s -> %s\n", procfile, name);
				}
			}
		}
	}

	closedir(dp);
	return 0;
}

//!\brief       判断给定设备节点的磁盘类型
//!\param   const char *dev 设备节点，如/dev/sda
//!\return  DiskActType
DiskActType EDVR_HD_GetDiskType(const char *dev)
{
	if (IS_SDCARD(dev))
	{
		return DISKACT_SDCARD;
	}
	else
	{
		return DISKACT_NONE;
	}
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

/*@ preif EDVR_HD_MountNode 封装mount函数，只是针对于VFAT设备的挂载
 *@ param [in] dev 要挂载的目录
 *@ param [in] mnt_point 挂载点
 *@ return -1 挂载失败
 *@         0 挂载成功
 *@ note  较之mount命令，避免了针对异常SD卡挂载塞住的问题
 */
int EDVR_HD_MountNode(const char *dev, const char *mnt_point)
{
    if (!dev || !mnt_point)
    {
        printf("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if (access(dev, F_OK) || access(mnt_point, F_OK))
    {
        printf("%s or %s not exist!\n", dev, mnt_point);
        return -1;
    }

    int ret = -1;
    E_FILE_SYS_TYPE fs_type = FS_ERROR_TYPE;
    fs_type = GetFileSystemType(dev);

    switch(fs_type)
    {
        case FS_EXT2:
            ret = mount((const char*)dev, (const char*)mnt_point, (const char*)"ext2", 8u, "errors=continue");
            break;
        case FS_EXFAT:
            ret = mount((const char*)dev, (const char*)mnt_point, (const char*)"exfat", 8u, "errors=continue");
            break;
        case FS_NTFS:
            ret = mount((const char*)dev, (const char*)mnt_point, (const char*)"ntfs3g", 8u, "errors=continue");
            break;
        case FS_FAT32:
            ret = mount((const char*)dev, (const char*)mnt_point, (const char*)"vfat", 8u, "errors=continue");
            break;
        default:
            printf("Unknown filesystem type!\n");
            break;
    }

    return ret;
}

/*!
 * \brief
 * 获取磁盘的序列号
 * 
 * \param char *pcDev
 * 设备节点.
 * 
 * \param char *pcSerialNo
 *返回的序列号
 * 
 * 
 * \returns
 * 成功返回0，失败返回非0.
 * 
 */
static void get_diskSerialNo(const char *pcDev, char *pcSerialNo, int serialLen)
{
#ifdef __FAT32__
#define _FAT32_EXT3_
#endif

#ifdef __EXT3__
#define _FAT32_EXT3_
#endif

#ifdef _FAT32_EXT3_

	FILE *fSerial;

	char *cp;

	int iCount;

	char szNode[12];

	char szSerialPath[40];

	char szSerialString[40];

	memset(szNode, 0, sizeof (szNode));
	memset(szSerialPath, 0, sizeof (szSerialPath));

	if (NULL == pcDev || NULL == pcSerialNo)
	{
		return;
	}

	cp = strrchr(pcDev, '/');
	if (NULL == cp)
	{
		strncpy(pcSerialNo, "UnKnown", serialLen - 1);
		return;
	}

	strncpy(szNode, cp + 1, sizeof (szNode) - 1);
	szNode[sizeof (szNode) - 1] = '\0';

	snprintf(szSerialPath, sizeof (szSerialPath), "/sys/block/%s/device/serial", szNode);

	fSerial = fopen(szSerialPath, "r");
	if (NULL == fSerial)
	{
		strncpy(pcSerialNo, "UnKnown", serialLen - 1);
		return;
	}
	else
	{
		if (NULL != fgets(szSerialString, sizeof (szSerialString), fSerial))
		{
			strncpy(pcSerialNo, szSerialString, serialLen - 1);
			for (iCount = strlen(pcSerialNo) - 1; iCount >= 0; iCount--)
			{
				if (isspace(pcSerialNo[iCount]) && isalnum(pcSerialNo[iCount - 1]))
				{
					pcSerialNo[iCount] = '\0';
					break;
				}
			}
		}
		else
		{
			strncpy(pcSerialNo, "UnKnown", serialLen - 1);
		}
	}
	fclose(fSerial);

#else

	int iCount;

	char *cp;
	struct hd_driveid id = { 0 };
	int fd = open(pcDev, O_RDONLY);

	if (fd < 0)
	{
		dprintf("open dev %s error!\n", pcDev);
		strncpy(pcSerialNo, "UnKnown", serialLen - 1);
		return;
	}
 
	if (ioctl(fd, HDIO_GET_IDENTITY, &id) < 0)
	{
		dprintf("dev %s ioctl HDIO_GET_IDENTITY fail\n", pcDev);
		strncpy(pcSerialNo, "UnKnown", serialLen - 1);
		perror("HDIO_GET_IDENTITY");
	}
	else
	{
		strncpy(pcSerialNo, id.serial_no, serialLen - 1);
		//strncpy(pcSerialNo, "0x1aade8e1", serialLen - 1);//-

		cp = pcSerialNo;
		for (iCount = 0; iCount < 30; iCount++)
		{
			if (isspace(pcSerialNo[iCount]))
			{
				cp++;
				continue;
			}
			break;
		}
		strncpy(pcSerialNo, cp, serialLen - 1);
	}
	close(fd);
#endif

	return;
}




/*!
 * \brief
 * 目录创建函数.
 * 
 * \param fullpath
 * 目录路径.
 * 
 * \returns
 * 成功返回0，失败返回-1.
 * 
 */
static int CreateDir(char *fullpath)
{
	int pos = 1;
	char tmpstr[256];
	memset(tmpstr, 0, 256);

	if (fullpath[0] != '/')		//!< 需要是绝对路径
	{
		DISK_ERR("it's not a full path!\n");
		return -1;
	}

	while (fullpath[pos] != '\0')
	{
		pos++;
		if (fullpath[pos] == '/')	//!< 发现一级目录,建立一级目录
		{
			memcpy(tmpstr, fullpath, pos);
			mkdir(tmpstr, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		}
	}

	mkdir(fullpath, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	return 0;
}


/*!
 * \brief
 * 创建设备节点文件.
 * 
 * \param dev
 * 设备节点名称.
 * 
 * \returns
 * 成功返回0，失败返回-1.
 * 
 */
static int MK_HD_Node(const char *dev)
{
	//dprintf("dev = %s\n", dev);
	return 0;
}

static int get_sdcard_count(void)
{
	DIR *dp;

	int no, count = 0;

	char syspath[40];

	//检测SD卡
	for (no = 0; no < MAX_SDCARD_NUM; ++no)
	{
		Get_SDCard_SysPath(no, syspath, sizeof (syspath));///sys/block/mmcblk0

		if ((dp = opendir(syspath)) == NULL)
		{
			DPRINTF("can't open directory: %s\n", syspath);
			continue;
		}

		++count;
		closedir(dp);
	}

	DPRINTF("sd card count = %d\n", count);
	return count;
}

//!\brief       获取系统内硬盘总数
//!\return  成功硬盘个数
int EDVR_HD_GetTotalDiskCount(void)
{
return 1;
    if (!CheckDiskDamage())
    {
        printf("[%s-%d] Check disk is damaged!\n", __func__, __LINE__);
        return 0;
    }

	int counts = 0;				//!< SATA硬盘个数

	pthread_mutex_lock(&makePartLock);
#ifndef USE_SD_PKG
	//start the partition read only checking thread
	if (0 == disk_readonly_first)
	{
		int ret;

		ret = pthread_create(&thread_ReadOnlyCheck, NULL, disk_readonly, NULL);

		if (ret != 0 || (thread_ReadOnlyCheck == 0))
		{
			if (EAGAIN == ret)
				printf("[DISK]%s EROOR *********pthread_create failed with EAGAIN (thread num limit arrived)*********\n", __FUNCTION__);
			else if (EINVAL == ret)
				printf("[DISK]%s ERROR *********pthread_create failed with EINVAL (create thread with invalid parameters)***********.\n",
					   __FUNCTION__);
			else if (ENOMEM == ret)
				printf("[DISK]%s ERROR *********pthread_create failed with ENOMEM (no memeory to create thread)*********.\n", __FUNCTION__);
			else
				printf("[DISK]%s ERROR *********pthread_create failed with unkonen error code %d *************\n", __FUNCTION__, ret);
		}
		disk_readonly_first = 1;
	}
#endif
	pthread_mutex_unlock(&makePartLock);

	//检测SD卡
	counts = get_sdcard_count();

	//dprintf("counts = %d\n", counts);
	return counts;
}

int EDVR_HD_GetSdCardCount(void)
{
	int counts = 0;
	//检测SD卡
	counts = get_sdcard_count();
	return counts;
}

//!\brief       获取指定硬盘的总线编号
//!\param   int index       硬盘编号(从0开始，到硬盘总数-1)
//!\return  成功返回硬盘总线编号,失败返回-1
int EDVR_HD_GetBusNumber(int index)
{
	if (index < 0)
	{
		return -1;
	}

	return index;
}

//!\brief       获取指定硬盘的总线编号
//!\param   const char *dev       硬盘设备节点名，如/dev/sda
//!\return  成功返回硬盘总线编号,失败返回-1
int EDVR_HD_GetBusNumberEx(const char *dev)
{
#ifdef __FAT32__

	int nBusNo = Get_Disk_DevIndex(dev);

	return nBusNo;

#else

	int fd, busno;

	pthread_mutex_lock(&makePartLock);

	fd = open(dev, O_WRONLY);
	if (fd < 0)
	{
		DISK_ERR("can't open %s\n", dev);
		pthread_mutex_unlock(&makePartLock);
		return -1;
	}

	if (0 != ioctl(fd, SCSI_IOCTL_GET_BUS_NUMBER, &busno))
	{
		close(fd);
		DISK_ERR("%s unsupported ioctl SCSI_IOCTL_GET_BUS_NUMBER\n", dev);
		pthread_mutex_unlock(&makePartLock);
		return -1;
	}

	close(fd);
	pthread_mutex_unlock(&makePartLock);
	return busno;
#endif
}

static unsigned long kscale(long b, long bs)
{
	return (b * (long long) bs + 1024 / 2) / 1024 / 1024;
}

static int get_mount_point_spas(const char *mount_point, int *pbMounted, unsigned long *pdwTotalSize, unsigned long *pdwAvailableSize)
{
	FILE *tmp;

	char buf[120];

	struct statfs s;

	DPRINTF("%s\n", mount_point);

	tmp = fopen("/proc/mounts", "r");
	if (tmp == NULL)
	{
		DISK_ERR("can't open file /proc/mounts\n");
		perror("fopen");
		return -1;
	}

	while (1)
	{
		if (NULL != fgets(buf, sizeof (buf), tmp))
		{
			if (NULL != strstr(buf, mount_point))
			{
				fclose(tmp);
				if (NULL != pbMounted)
				{
					*pbMounted = 1;
				}
				break;
			}
		}

		if (feof(tmp))
		{
			fclose(tmp);
			DPRINTF("part %s not mounted\n", mount_point);
			return 0;
		}
	}

	if (statfs(mount_point, &s) != 0)
	{
		DISK_ERR("statfs return failed, return hd size 0!\n");
		return 0;
	}

	if (NULL != pdwTotalSize)
	{
		*pdwTotalSize = kscale(s.f_blocks, s.f_bsize);
//		printf("total size: %lu\n", *pdwTotalSize);
	}

	if (NULL != pdwAvailableSize)
	{
		*pdwAvailableSize = kscale(s.f_bavail, s.f_bsize);
//		printf("available size: %lu\n", *pdwAvailableSize);
	}
#if 0
	if (!TestMountPointIsWR(mount_point))
	{
		if (NULL != pdwAvailableSize)
		{
			*pdwAvailableSize = 0;
//			printf("---available size: %lu\n", *pdwAvailableSize);
		}
	}
#endif
	return 0;
}

//!\brief       获取分区挂载点空间大小
//!\param   const char *pszMountPoint       分区挂载点
//!\param   int *pbMounted              返回是否挂载
//!\param   unsigned long  *pdwTotalSize        返回总的空间大小
//!\param   unsigned long  *pdwAvailableSize    返回剩余空间大小
//!\return  成功返回0，失败返回-1.
int EDVR_HD_GetMountPointSpace(const char *pszMountPoint, int *pbMounted, unsigned long *pdwTotalSize, unsigned long *pdwAvailableSize)
{
    if (!CheckDiskDamage())
    {
        printf("[%s-%d] Check disk is damaged!\n", __func__, __LINE__);
        return -1;
    }

	int ret;

	if (NULL != pbMounted)
	{
		*pbMounted = 0;
	}

	if (NULL != pdwTotalSize)
	{
		*pdwTotalSize = 0L;
	}

	if (NULL != pdwAvailableSize)
	{
		*pdwAvailableSize = 0L;
	}

	if (NULL == pszMountPoint)
	{
		DISK_ERR("NULL == pszMountPoint\n");
		return -1;
	}

	pthread_mutex_lock(&makePartLock);

	if (IS_SDCARD_MOUNTPOINT(pszMountPoint))
	{
		ret = get_mount_point_spas(pszMountPoint, pbMounted, pdwTotalSize, pdwAvailableSize);
	}
	else
	{
		ret = -1;
	}
	pthread_mutex_unlock(&makePartLock);
	return ret;
}

/*!
 * \brief
 * 获取硬盘总容量.
 * 
 * \param dev
 * 硬盘设备节点.
 * 
 * \returns
 * 成功返回硬盘总容量，失败返回0.
 * 
 */
unsigned long get_hd_space(const char *dev)
{
	unsigned long size;

	int fd = -1;

	MK_HD_Node(dev);
	fd = open(dev, /*O_RDWR */ O_RDONLY);
	if (fd < 0)
	{
		DISK_ERR("open %s error!\n", dev);
		return 0L;
	}

	if (ioctl(fd, BLKGETSIZE, &size))
	{
		size = 0L;
		DISK_ERR("%s: can not get size\n", dev);
	}

	close(fd);
    fd = -1;

	return (unsigned long) size / (1024 * 2);
}

//!\brief       有新硬盘加入时的回调.对硬盘各分区作mount操作
//!\param   const char *dev       硬盘设备节点名，如/dev/sda
//!\return  添加成功返回该硬盘被成功挂载的分区个数，失败返回-1.
int EDVR_HD_AddDiskCallBack(const char *dev)
{
	int ret;

	int mountno;

	DIR *dp;

	char syspath[120];

	char fsck_comm[100];

	char dev_part[20];

	char mount_point[40];

	struct dirent *entry;

	//判断磁盘设备名称是否合法
	if (!EDVR_HD_IsValidDisk(dev))
	{
		DISK_ERR("invalid disk dev name: %s\n", dev);
		return -1;
	}

    if (!CheckDiskDamage())
    {
        printf("[%s-%d] Check disk is damaged!\n", __func__, __LINE__);
        return -1;
    }

	pthread_mutex_lock(&makePartLock);

	//!读出/sys/block/下的分区，逐一挂载
	Get_Disk_SysPath(dev, syspath, sizeof (syspath));

	if ((dp = opendir(syspath)) == NULL)
	{
		dprintf("can't open directory: %s\n", syspath);
		pthread_mutex_unlock(&makePartLock);
		return -1;
	}

	mountno = 0;
	while ((entry = readdir(dp)) != NULL)
	{
		//!<找到一个分区
		if (strstr(entry->d_name, syspath + 11))
		{
			memset(dev_part, 0, sizeof (dev_part));
			snprintf(dev_part, sizeof (dev_part), "/dev/%.14s", entry->d_name);

			MK_HD_Node(dev_part);

			EDVR_HD_GetMountPoint(dev_part, mount_point, sizeof (mount_point));

			CreateDir(mount_point);	//!<创建挂载点目录和设备节点文件

#ifdef __JFS__
			snprintf(fsck_comm, sizeof(fsck_comm), "fsck.jfs -p -y %s\n", dev_part);
#elif defined(__EXT3__)
			snprintf(fsck_comm, sizeof(fsck_comm), "fsck.ext3 -p %s\n", dev_part);
#elif defined(__FAT32__)
			snprintf(fsck_comm, sizeof(fsck_comm), "dosfsck -y %s\n", dev_part);
#elif defined(__NTFS__)
			snprintf(fsck_comm, sizeof(fsck_comm), "ntfsfix %s\n", dev_part);;
#else
#error no define __JFS__ and __EXT3__
#endif
			mysystem(fsck_comm);

            ret = EDVR_HD_MountNode(dev_part, mount_point); //!< 挂载分区
			if (0 == ret)
			{
				dprintf("mount %s on %s Successfully\n", dev_part, mount_point);
				mountno++;
			}
			else
			{
				//!<此处作挂载失败处理，对该分区作强制性检测，然后再挂


#ifdef __JFS__
				snprintf(fsck_comm, sizeof(fsck_comm), "fsck.jfs -f -y %s\n", dev_part);
#elif defined(__EXT3__)
				snprintf(fsck_comm, sizeof(fsck_comm), "fsck.ext3 -f -y %s\n", dev_part);
#elif defined(__FAT32__)
				snprintf(fsck_comm, sizeof(fsck_comm), "dosfsck -f -y %s\n", dev_part);
#elif defined(__NTFS__)
			;
#else
#error no define __JFS__ and __EXT3__
#endif
				mysystem(fsck_comm);

				if (0 == EDVR_HD_MountNode(dev_part, mount_point))	//!< 挂载分区
				{
					dprintf("mount %s on %s Successfully\n", dev_part, mount_point);
					mountno++;
				}
			}
		}
	}

	closedir(dp);
	pthread_mutex_unlock(&makePartLock);
	return mountno;
}

//!\brief   重建分区表
//!\param   const char *dev  硬盘分区设备节点名，如/dev/sda(SD卡设备为/dev/mmcblk0)
//!\return  成功返回0，失败返回-1.
int EDVR_HD_ReBuildPartition(const char *dev, int part)
{
    int ret = -1;

    if (NULL == dev)
    {
        DISK_ERR("NULL == dev or part:%d error\n", part );
        return -1;
    }

	//经测试 fdisk可以直接对TF卡重建分区
    ret = fdisk(dev, part);
    return ret;
}

//!\brief       格式化分区(分区必须卸载后才能格式化)
//!\param   const char *dev       硬盘分区设备节点名，如/dev/sda1(SD卡设备为/dev/mmcblk0p1)
//!\return  成功返回0，失败返回-1.
int EDVR_HD_FormatPartition(const char *dev)
{
	char cmd[120];

	if (NULL == dev)
	{
		DISK_ERR("NULL == dev\n");
		return -1;
	}
	
    if (!CheckDiskDamage())
    {
        printf("[%s-%d] Check disk is damaged!\n", __func__, __LINE__);
        return -1;
    }

	if (IS_SDCARD_PARTITION(dev))
	{
	}
	else
	{
		DISK_ERR("invalid partition dev name %s\n", dev);
		return -1;
	}

	pthread_mutex_lock(&makePartLock);

	//正在挂载的分区不能格式化
	if (1 == hasdevmount(dev))
	{
		DISK_ERR("%s is mounted\n", dev);
		pthread_mutex_unlock(&makePartLock);
		return -1;
	}
 
	snprintf(cmd, sizeof (cmd), "mkfs.vfat %s", dev);

	dprintf("%s\n", cmd);
	mysystem(cmd);
	dprintf("format ok\n");
	pthread_mutex_unlock(&makePartLock);
	return 0;
}

//!\brief       删除一个分区
//!\param   int index               硬盘编号(从0开始，到硬盘总数-1)
//!\param   int nPartitionIndex 分区编号(从0开始，到有效分区总数-1)
//!\return  成功返回0, 失败返回错误码

/*
说明:
	硬盘只有所有分区被卸载以后，才能删除分区。
	可以调用EDVR_HD_RemoveDiskCallBack来卸载所有分区。
	卸载时，会自动先关闭该硬盘上所有打卡的文件。
	
错误码:	
	1	没有找到指定的硬盘
	2	超过最大允许的分区数
	-1	没有找到指定的分区
	-2	该硬盘有分区被挂载
	-3	该分区是系统保留分区
	-4	该硬盘有文件打开
*/
int DelPartition(char *dev, int nPartitionIndex);

int EDVR_HD_DelPartition(int index, int nPartitionIndex)
{
    if (!CheckDiskDamage())
    {
        printf("[%s-%d] Check disk is damaged!\n", __func__, __LINE__);
        return -1;
    }

	int part, ret;

	char dev[50];

	DiskDeviceInfoEx DiskInfo;

	if ((nPartitionIndex < 0) || (nPartitionIndex >= MAX_DISK_PARTITIONSEX))
	{
		DISK_ERR("invalid nPartitionIndex %d\n", nPartitionIndex);
		return -1;
	}

	ret = EDVR_HD_Index2DevNode(index, dev, sizeof (dev));
	if (0 != ret)
	{
		DISK_ERR("can't find disk, index = %d\n", index);
		return 1;
	}

	EDVR_HD_GetDiskInfoExFromDev(dev, &DiskInfo);

	pthread_mutex_lock(&makePartLock);

#ifdef __FAT32__
	if (DiskInfo.PartitionsNum <= 0)
	{
		DISK_ERR("%s has no partitions\n", dev);
		pthread_mutex_unlock(&makePartLock);
		return -1;
	}
#else
	if (DiskInfo.PartitionsNum <= 1)
	{
		DISK_ERR("%s has %d partitions\n", dev, DiskInfo.PartitionsNum);
		pthread_mutex_unlock(&makePartLock);
		return -3;
	}

	if (nPartitionIndex == 0)
	{
		DISK_ERR("can't delete reserved partition 0\n");
		pthread_mutex_unlock(&makePartLock);
		return -3;
	}
#endif

	if (nPartitionIndex >= DiskInfo.PartitionsNum)
	{
		DISK_ERR("nPartitionIndex %d >= PartitionsNum %d\n", nPartitionIndex, DiskInfo.PartitionsNum);
		pthread_mutex_unlock(&makePartLock);
		return -1;
	}

	//删除分区，需要卸载该硬盘所有分区

	if (1 == DiskInfo.IsWritting)
	{
		DISK_ERR("%s has opened files\n", dev);
		pthread_mutex_unlock(&makePartLock);
		return -4;
	}

	if (1 == hasdevmount(dev))
	{
		DISK_ERR("%s has mounted partition\n", dev);
		pthread_mutex_unlock(&makePartLock);
		return -2;
	}

	//获取分区号
	part = Get_Partition_DevIndex(DiskInfo.Partitions[nPartitionIndex].DevName);
	ret = DelPartition(dev, part);
	pthread_mutex_unlock(&makePartLock);
	return ret;
}

/*!
 * \brief
 * 对指定硬盘分区.
 * 
 * \param index
 * 硬盘序号,取值从1到实际硬盘数.
 * 
 * \param unsigned long nSize 大小。单位：M
 * 分区数.
 *\return 0为成功，1硬盘不存在，2为超过最大分区数量，3其他错误
 * 
 */

int AddPartition(char *dev, unsigned long nSize);

int EDVR_HD_MakePartitionFromSize(int index, unsigned long nSize)
{
    if (!CheckDiskDamage())
    {
        printf("[%s-%d] Check disk is damaged!\n", __func__, __LINE__);
        return -1;
    }

	char dev[50];

	DiskDeviceInfoEx DiskInfo;

	if (0 != EDVR_HD_Index2DevNode(index, dev, sizeof (dev)))
	{
		DISK_ERR("can't find disk, index = %d\n", index);
		return 1;
	}

	EDVR_HD_GetDiskInfoExFromDev(dev, &DiskInfo);

	pthread_mutex_lock(&makePartLock);

	if (MAX_DISK_PARTITIONSEX <= DiskInfo.PartitionsNum)
	{
		DISK_ERR("disk have too many partitions %d\n", DiskInfo.PartitionsNum);
		pthread_mutex_unlock(&makePartLock);
		return 2;
	}

	//创建分区，需要卸载该硬盘所有分区

	if (1 == DiskInfo.IsWritting)
	{
		DISK_ERR("%s has opened files\n", dev);
		pthread_mutex_unlock(&makePartLock);
		return -4;
	}

	if (1 == hasdevmount(dev))
	{
		DISK_ERR("%s has mounted partition\n", dev);
		pthread_mutex_unlock(&makePartLock);
		return -2;
	}

	if (0 != AddPartition(dev, nSize))
	{
		DISK_ERR("%s AddPartition fail\n", dev);
		pthread_mutex_unlock(&makePartLock);
		return 3;
	}

	pthread_mutex_unlock(&makePartLock);
	return 0;
}

//!\brief       卸载指定分区
//!\param   const char *mountpoint    分区挂载点，如/dat/sda1
//!\return  成功返回0，失败返回-1.
int EDVR_HD_UMountPartition(const char *mountpoint)
{
    int ret = -1;

	if (NULL == mountpoint)
	{
		DISK_ERR("NULL == mountpoint\n");
		return -1;
	}

	if (IS_SDCARD_MOUNTPOINT(mountpoint))
	{
	}
	else
	{
		DISK_ERR("invalid mountpoint %s\n", mountpoint);
		return -1;
	}

	pthread_mutex_lock(&makePartLock);

	//关闭挂载点上所有打开的文件
	closedevfile(mountpoint);
	mysystem("sync");

	// 不判断返回值 umount成功返回0
	ret = umount(mountpoint);
	if (ret)
	{
        dprintf("umount %s fail: %s\n", mountpoint, strerror(errno));
	}

	usleep(500000);				//延时500ms，以更新/proc/mounts
	if (0 == hasdevmount(mountpoint))
	{
		dprintf("umount %s ok\n", mountpoint);
		pthread_mutex_unlock(&makePartLock);
		return 0;
	}
	else
	{
//	    bzero(command, sizeof(command));
//	    snprintf(command, sizeof(command), "umount -l %s", mountpoint);
//      mysystem(command);
	}
	
	closedevfile(mountpoint);
	mysystem("sync");
	
	// 不判断返回值 umount成功返回0
	ret = umount(mountpoint);
	if (ret)
	{
        dprintf("umount %s fail: %s\n", mountpoint, strerror(errno));
	}

	usleep(500000);				//延时500ms，以更新/proc/mounts
	if (0 == hasdevmount(mountpoint))
	{
		dprintf("umount %s ok\n", mountpoint);
		pthread_mutex_unlock(&makePartLock);
		return 0;
	}

	DISK_ERR("umount %s fail\n", mountpoint);
	pthread_mutex_unlock(&makePartLock);
	return -1;
}


int IsInited(const char *dev);

//!\brief       判断给定硬盘是否为新硬盘(SD卡设备的判断标准为至少有一个FAT32分区)
//!\param   const char *dev       硬盘设备节点名，如/dev/sda(SD卡设备为/dev/mmcblk0)
//!\return  是新硬盘返回1,否则返回0,错误返回-1
BOOL EDVR_HD_IsNewDiskEx(const char *dev)
{
    if (!CheckDiskDamage())
    {
        printf("[%s-%d] Check disk is damaged!\n", __func__, __LINE__);
        return -1;
    }

	int ret;

	//判断磁盘设备名称是否合法
	if (!EDVR_HD_IsValidDisk(dev))
	{
		DISK_ERR("invalid disk dev name: %s\n", dev);
		return -1;
	}

	ret = IsInited(dev);
	if (ret == 1)
	{
		ret = 0;
	}
	else if (ret == 0)
	{
		ret = 1;
		dprintf("%s is new disk\n", dev);
	}
	else
	{
		dprintf("%s check fail %d\n", dev, ret);
		ret = -1;
	}

	return (ret);
}

//!\brief       初始化硬盘(SD卡设备会初始化为1个FAT32主分区，包含所有存储空间)
//!\param   const char *dev       硬盘设备节点名，如/dev/sda(SD卡设备为/dev/mmcblk0)
//!\return  是新硬盘返回1,否则返回0,错误返回-1
int InitDev(const char *dev, unsigned long nSize);

int EDVR_HD_Init(const char *dev)
{
    if (!CheckDiskDamage())
    {
        printf("[%s-%d] Check disk is damaged!\n", __func__, __LINE__);
        return -1;
    }

	int ret;

	//判断磁盘设备名称是否合法
	if (!EDVR_HD_IsValidDisk(dev))
	{
		DISK_ERR("invalid disk dev name: %s\n", dev);
		return -1;
	}

	dprintf("dev = %s\n", dev);

	pthread_mutex_lock(&makePartLock);

	ret = InitDev(dev, (unsigned long) (10 * 1024));//- 10 * 1024

	pthread_mutex_unlock(&makePartLock);

	return ret;
}

int my_force_format_ext3(const char *dev)
{
	char comm[128];
	char buff[512];
	char *path = "/tmp/mount";
	FILE *fp=NULL;
	
//	printf("into force format to ext3\n");
	memset(comm, 0, sizeof(comm));
	memset(buff, 0, sizeof(buff));
	snprintf(comm, sizeof(comm), "mount | grep ext3 > %s", path);
	mysystem(comm);
	fp = fopen(path, "r");
	if(fp == NULL)
	{
		printf("open %s fail!\n", path);
		goto nofind;
		return -1;
	}
	fgets(buff, sizeof(buff)-1, fp);
	if(fp!=NULL)
		fclose(fp);

	if(NULL == strstr(buff, "ext3"))
	{
	nofind:	
		printf("need formate !\n");
		memset(comm, 0, sizeof(comm));
		snprintf(comm, sizeof(comm), "mkfs.ext3 %s", dev);
		mysystem(comm);
	}
	else
	{
		printf("it has been ext3!\n");
	}

	return 0;
}

//!\brief       挂载指定分区
//!\param   const char *dev       硬盘分区设备节点名，如/dev/sda1(SD卡设备为/dev/mmcblk0p1)
//!\return  成功返回0，失败返回-1.
int EDVR_HD_MountPartition(const char *dev)
{
    if (!CheckDiskDamage())
    {
        printf("[%s-%d] Check disk is damaged!\n", __func__, __LINE__);
        return -1;
    }

	char fsck_comm[100];
	char mount_point[40];

	int ret;

	if (NULL == dev)
	{
		DISK_ERR("NULL == dev\n");
		return -1;
	}

	//判断是否合法设备
	if (IS_SDCARD_PARTITION(dev))
	{
	}
	else
	{
		DISK_ERR("invalid partition dev name %s\n", dev);
		return -1;
	}

	pthread_mutex_lock(&makePartLock);

	EDVR_HD_GetMountPoint(dev, mount_point, sizeof (mount_point));

	CreateDir(mount_point);		//!<创建挂载点目录
    printf("my add: start check mmc\n");

	snprintf(fsck_comm, sizeof(fsck_comm), "dosfsck -y %s\n", dev);//没有此命令

//SD卡强制检测放到起始脚本执行,以免影响程序运行
#if 0
	mysystem(fsck_comm);
#endif
    printf("my add: start mount mmc\n");

    ret = EDVR_HD_MountNode(dev, mount_point);	//!< 挂载分区
	if (0 == ret)
	{
		dprintf("mount %s on %s ok\n", dev, mount_point);
	}
	else
	{
	    printf("my add: mount fail,start force check\n");
		//!<此处作挂载失败处理，对该分区作强制性检测，然后再挂
		snprintf(fsck_comm, sizeof(fsck_comm), "dosfsck -f -y %s\n", dev);

		mysystem(fsck_comm);
        printf("my add: start mount again\n");

        ret = EDVR_HD_MountNode(dev, mount_point);  //!< 挂载分区
		if (0 == ret)
		{
			dprintf("mount %s on %s ok\n", dev, mount_point);
		}
		else
		{
			DISK_ERR("mount partition %s fail\n", dev);
			pthread_mutex_unlock(&makePartLock);
			return -1;
		}
	}

	pthread_mutex_unlock(&makePartLock);
	return 0;
}

//!\brief   挂载指定分区(不进行dosfsck)
//!\param   const char *dev       硬盘分区设备节点名，如/dev/sda1(SD卡设备为/dev/mmcblk0p1)
//!\return  成功返回0，失败返回-1.
int DiskMountPartitionNoDosfsck(const char *dev)
{
	char mount_point[40];

	int ret;

	if (NULL == dev)
	{
		DISK_ERR("NULL == dev\n");
		return -1;
	}

	//判断是否合法设备
	if (IS_SDCARD_PARTITION(dev))
	{
	}
	else
	{
		DISK_ERR("invalid partition dev name %s\n", dev);
		return -1;
	}

	pthread_mutex_lock(&makePartLock);

	EDVR_HD_GetMountPoint(dev, mount_point, sizeof(mount_point));

	CreateDir(mount_point);		//!<创建挂载点目录

    ret = EDVR_HD_MountNode(dev, mount_point); //!< 挂载分区
	if (0 == ret)
	{
		dprintf("mount %s on %s ok\n", dev, mount_point);
	}
	else
	{
	    /* 如果失败，重新再来挂载 */
	    mysystem("sync");
	    usleep(500000);
		ret = EDVR_HD_MountNode(dev, mount_point); //!< 挂载分区
		if (0 == ret)
		{
			dprintf("mount %s on %s ok\n", dev, mount_point);
		}
		else
		{
			DISK_ERR("mount partition %s fail\n", dev);
			pthread_mutex_unlock(&makePartLock);
			return -1;
		}
	}

	pthread_mutex_unlock(&makePartLock);
	return 0;
}

//!\brief 获取磁盘型号
//!\praram const char *pcDev 磁盘节点
//!\praram char *pcModelName 磁盘型号(输出参数)
//!\return  无
static void get_diskModelName(const char *pcDev, char *pcModelName, int modelNameLen)
{
	FILE *fModel;

	char *cp;

	int iCount;

	char acNode[12];

	char acModelPath[40];

	char acModelString[40];

	if (NULL == pcDev || NULL == pcModelName)
	{
		return;
	}

	cp = strrchr(pcDev, '/');
	if (NULL == cp)
	{
		strncpy(pcModelName, "UnKnown", modelNameLen - 1);
		return;
	}

	memset(acNode, 0, sizeof (acNode));
	memset(acModelPath, 0, sizeof (acModelPath));
	strncpy(acNode, cp + 1, sizeof(acNode) - 1);
	if (IS_SDCARD(pcDev))
	{
		snprintf(acModelPath, sizeof (acModelPath), "/sys/block/%s/device/type", acNode);
	}
	fModel = fopen(acModelPath, "r");
	if (NULL == fModel)
	{
		strncpy(pcModelName, "UnKnown", modelNameLen - 1);
		return;
	}
	else
	{
		if (NULL != fgets(acModelString, sizeof (acModelString), fModel))
		{
			strncpy(pcModelName, acModelString, modelNameLen - 1);
			for (iCount = strlen(pcModelName) - 1; iCount >= 0; iCount--)
			{
				if (isspace(pcModelName[iCount]) && isalnum(pcModelName[iCount - 1]))
				{
					pcModelName[iCount] = '\0';
					break;
				}
			}

		}
		else
		{
			strncpy(pcModelName, "Unknown", modelNameLen - 1);
		}
	}
	fclose(fModel);
	return;
}

//!\brief       获取指定硬盘信息.
//!\param   const char *dev       硬盘设备节点名，如/dev/sda
//!\param   DiskDeviceInfoEx * diskinfo 保存磁盘信息的结构体的指针.
//!\return  成功返回0，失败返回-1
int EDVR_HD_GetDiskInfoExFromDev(const char *dev, DiskDeviceInfoEx * diskinfo)
{
#if 0
    if (!CheckDiskDamage())
    {
        printf("[%s-%d] Check disk is damaged!\n", __func__, __LINE__);
        return -1;
    }
#endif

	int i = 0;
	int parts = 0;
	int partmounted = 0;
	int cylindersize = 0;
	char dev_part[30] = {0};
	char diskmountpoint[40] = {0};
	char tmpname[100] = {0};
	struct Disk_Desc tDiskDesc = {0};	// 磁盘信息
	struct Part_Desc *ptPart = NULL;

	//DISK_BEGIN_TIME;

	if (NULL == diskinfo)
	{
		DISK_ERR("NULL == diskinfo\n");
		return -1;
	}

	memset(diskinfo, 0, sizeof (DiskDeviceInfoEx));

	//判断磁盘设备名称是否合法
	if (!EDVR_HD_IsValidDisk(dev))
	{
		DISK_ERR("invalid disk dev name: %s\n", dev);
		return -1;
	}

//	dprintf("dev = %s\n", dev);
	pthread_mutex_lock(&makePartLock);

	//硬盘型号
	get_diskModelName(dev, tmpname, sizeof(tmpname));
	strncpy(diskinfo->DeviceName, tmpname, sizeof (diskinfo->DeviceName) - 1);

	//硬盘序列号
	get_diskSerialNo(dev, tmpname, sizeof(tmpname));
	strncpy(diskinfo->SerialNo, tmpname, sizeof (diskinfo->SerialNo) - 1);

	//硬盘设备节点名
	strncpy(diskinfo->Dev, dev, sizeof (diskinfo->Dev) - 1);

	diskinfo->RW_Status = 1;
	diskinfo->Total_disk_space = get_hd_space(dev);

	cylindersize = GetPartitionInfo(dev, &tDiskDesc);

	ptPart = &(tDiskDesc.atPartitions[0]);
	for (i = 0, parts = 0; i < MAX_DISK_PARTITIONS; i++, ptPart++)
	{
		unsigned long space;

		if (!IS_FAT_PARTITION(ptPart->Type))
		{
			//printf("my add: partition %d type is 0x%x \n", i, ptPart->Type);
			continue;
		}

		//获取分区节点名
		Get_Partition_DevNode(dev, i + 1, dev_part, sizeof (dev_part));
		space = get_hd_space(dev_part);
		if (0 == space)
		{
			printf("my add: hd space=0\n");
			//无效分区
			continue;
		}
		diskinfo->Partitions[parts].Type = ptPart->Type;
		diskinfo->Partitions[parts].Total_partitions_space = space;

		snprintf(diskinfo->Partitions[parts].DevName, sizeof(diskinfo->Partitions[parts].DevName), "%s", dev_part);
		
		EDVR_HD_GetMountPoint(dev_part, diskinfo->Partitions[parts].Mount_point, sizeof (diskinfo->Partitions[parts].Mount_point));
		pthread_mutex_unlock(&makePartLock);
		//printf("my add:mount point = %s\n",diskinfo->Partitions[parts].Mount_point);

		if (0 == EDVR_HD_GetMountPointSpace(diskinfo->Partitions[parts].Mount_point, &partmounted, NULL, &space))
		{
			diskinfo->Partitions[parts].Available_partitions_space = space;
			diskinfo->Partitions[parts].Partition_has_mounted = partmounted;
		}
		else
		{
			diskinfo->Partitions[parts].Available_partitions_space = 0;
			diskinfo->Partitions[parts].Partition_has_mounted = 0;
		}
		
		//printf("my add: parts=%d, Total=%d, Available=%d\n",parts,diskinfo->Partitions[parts].Total_partitions_space,diskinfo->Partitions[parts].Available_partitions_space);
		pthread_mutex_lock(&makePartLock);

		diskinfo->Partitions[parts].dwStartCHS = ptPart->nStart_Sec * cylindersize;
		diskinfo->Partitions[parts].dwEndtCHS = ptPart->nEnd_Sec * cylindersize;

		parts++;

		if (parts >= MAX_DISK_PARTITIONSEX)
		{
			//系统只管理14个有效分区
			printf("parts >= MAX_DISK_PARTITIONSEX\n");
			break;
		}
	}
	diskinfo->PartitionsNum = parts;

	EDVR_HD_GetMountPoint(dev, diskmountpoint, sizeof (diskmountpoint));
	diskinfo->IsWritting = hasfileopen(diskmountpoint);

	pthread_mutex_unlock(&makePartLock);

	//DISK_END_TIME;
	return 0;
}

//!\brief       获取指定硬盘信息.
//!\param   int index       硬盘编号(从0开始，到硬盘总数-1)
//!\param   DiskDeviceInfoEx * diskinfo 保存磁盘信息的结构体的指针.
//!\return  成功返回0，失败返回-1
int EDVR_HD_GetDiskInfoEx(int index, DiskDeviceInfoEx * diskinfo)
{
    memset(diskinfo, 0, sizeof (DiskDeviceInfoEx));

	//目前没有其他异常状态的TF卡供测试，只处理读写异常
	if (!CheckDiskDamage() && CheckDiskRWIsDamage())
    {
        printf("[%s-%d] Check disk is damaged!\n", __func__, __LINE__);
        return -1;
    }

	char dev[50] = {0};
	DiskDeviceInfoEx mdiskinfo = {0};
	int ret = -1;
	
	memset(&mdiskinfo, 0, sizeof (DiskDeviceInfoEx));

	//dprintf("index = %d\n", index);
	if(index < 0)
	{	
		printf("index = %d, return -1!\n", index);
		return -1;
	}

	if (0 != EDVR_HD_Index2DevNode(index, dev, sizeof (dev)))
	{
		DISK_ERR("can't find disk, index = %d\n", index);
		return -1;
	}

	//获取硬盘信息
	ret = EDVR_HD_GetDiskInfoExFromDev(dev, &mdiskinfo);
	memcpy(diskinfo, &mdiskinfo, sizeof (DiskDeviceInfoEx));
	return ret;

}

 //!\brief      移除硬盘的回调.对硬盘各分区作umount操作
//!\param   const char *dev       硬盘设备节点名，如/dev/sda
//!\return  移除成功返回0，失败返回-1.
int EDVR_HD_RemoveDiskCallBack(const char *dev)
{
    if (!CheckDiskDamage())
    {
        printf("[%s-%d] Check disk is damaged!\n", __func__, __LINE__);
        return -1;
    }

	int i;

	char mountpoint[40];

	//判断磁盘设备名称是否合法
	if (!EDVR_HD_IsValidDisk(dev))
	{
		DISK_ERR("invalid disk dev name: %s\n", dev);
		return -1;
	}

	pthread_mutex_lock(&makePartLock);

	EDVR_HD_GetMountPoint(dev, mountpoint, sizeof (mountpoint));

	//关闭该硬盘所有分区挂载点上的文件
	closedevfile(mountpoint);
	mysystem("sync");

	{
		//卸载该硬盘所有被挂载的分区
		for (i = 0; i <= MAX_DISK_PARTITIONS; i++)
		{
			EDVR_HD_GetPartitionMountPoint(dev, i, mountpoint, sizeof (mountpoint));
			umount(mountpoint);	//!<不判断返回值 umount成功，返回0，失败返回-1
		}
	}

	usleep(500000);				//延时500ms，以更新/proc/mounts
	if (1 == hasdevmount(dev))
	{
		DISK_ERR("umount disk %s all partitions ok\n", dev);
		pthread_mutex_unlock(&makePartLock);
		return -1;
	}

	dprintf("umount disk %s all partitions ok\n", dev);
	pthread_mutex_unlock(&makePartLock);
	return 0;
}


/*********************************************************************
 *
 *      用线程检测挂载点是否变为ro，若是，调用回调函数处理
 *
 *      线程在EDVR_HD_GetTotalDiskCount()启动,只被启动一次
 *
 *      EDVR_HD_SetMountPointRemove &  EDVR_HD_SetMountPointAdd应先于
 *
 *      EDVR_HD_GetTotalDiskCount()被调用。
 *
 ********************************************************************/

void (CALLBACK * MountPointRemoveCB) (char *MountPoint) = NULL;

void (CALLBACK * MountPointAddCB) (char *MountPoint) = NULL;

//!\brief       检测到挂载点被置为RO，通知EDVR将之移除
//!\param   MountpointRemoved   处理挂载点被移除的函数指针.
//!\return  无
void EDVR_HD_SetMountPointRemove(void (*MountpointRemoved) (char *MountPoing))
{
    if (!CheckDiskDamage())
    {
        printf("[%s-%d] Check disk is damaged!\n", __func__, __LINE__);
        return;
    }

	if (NULL != MountpointRemoved)
	{
		MountPointRemoveCB = MountpointRemoved;
	}
}

//!\brief       检测到挂载点被置为RO，将之重新挂载后，通知EDVR将之添加
//!\param   MountpointAdded 处理挂载点添加的函数指针.
//!\return  无
void EDVR_HD_SetMountPointAdd(void (*MountpointAdded) (char *MountPoing))
{
    if (!CheckDiskDamage())
    {
        printf("[%s-%d] Check disk is damaged!\n", __func__, __LINE__);
        return;
    }

	if (NULL != MountpointAdded)
	{
		MountPointAddCB = MountpointAdded;
	}
}

/*!
 * \brief
 * 检测到挂载点被置为RO，并采用回调函数处理
 * 
 * \returns
 * 无.
 */
static void disk_readonly_check(void)
{
	char device[25];

	char mount_point[40];

	char fs_type[20];

	char read_write[100];

	FILE *tmp;

	char buf[150];

	tmp = fopen("/proc/mounts", "r");
	if (tmp == NULL)
	{
		DISK_ERR("can't open file /proc/mounts\n");
		perror("fopen");
		return;
	}

	pthread_mutex_lock(&makePartLock);
	while (1)
	{
		char *mountstring;

		mountstring = fgets(buf, sizeof (buf), tmp);
		if (NULL != mountstring && (strstr(mountstring, MOUNTPOINT) || strstr(mountstring, "/usb/")) && NULL != strstr(mountstring, "ro"))
		{
			int j, k;

			//获取挂载点信息
			memset(device, '\0', sizeof (device));
			memset(mount_point, '\0', sizeof (mount_point));
			memset(fs_type, '\0', sizeof (fs_type));
			memset(read_write, '\0', sizeof (read_write));
			for (j = 0; j < strlen(buf); j++)
			{
				if (buf[j] == ' ')
					break;
				device[j] = buf[j];
			}

			for (k = j = j + 1; j < strlen(buf); j++)
			{
				if (buf[j] == ' ')
					break;
				mount_point[j - k] = buf[j];
			}

			for (k = j = j + 1; j < strlen(buf); j++)
			{
				if (buf[j] == ' ')
					break;
				fs_type[j - k] = buf[j];
			}

			for (k = j = j + 1; j < strlen(buf); j++)
			{
				if (buf[j] == ' ')
					break;
				read_write[j - k] = buf[j];
			}

			//对只读挂载点进行处理
			if (0 == strcmp(read_write, "ro"))
			{
				char cmd[100];

				int mountret;

				int umountret;

				closedevfile(mount_point);	//关闭打开的文件

				umountret = umount(mount_point);	//强制性umount
				if (0 == umountret)
				{
					if (NULL != MountPointRemoveCB)
					{
						char mountpoint_tmp[50];

						memset(mountpoint_tmp, 0, sizeof (mountpoint_tmp));	//if value of string changes.
						strncpy(mountpoint_tmp, mount_point, sizeof(mountpoint_tmp) - 1);
						pthread_mutex_unlock(&makePartLock);
						(*MountPointRemoveCB) (mountpoint_tmp);
						pthread_mutex_lock(&makePartLock);
					}

					memset(cmd, 0, sizeof cmd);
#ifdef __JFS__
					snprintf(cmd, sizeof(cmd), "fsck.jfs -p -y %s\n", device);	//文件完整性检测
#elif defined(__EXT3__)
					snprintf(cmd, sizeof(cmd), "fsck.ext3 -p %s\n", device);	//文件完整性检测
#elif defined(__FAT32__)
					snprintf(cmd, sizeof(cmd), "dosfsck -y %s\n", device);	//文件完整性检测                    
#elif defined(__NTFS__)
			;
#else
#error no define __JFS__ and __EXT3__
#endif
					mysystem(cmd);
                    
                    mountret = EDVR_HD_MountNode(device, mount_point);  //!< 挂载分区
					if (0 == mountret)
					{
						char mountpoint_tmp[50];

						memset(mountpoint_tmp, 0, sizeof (mountpoint_tmp));	//if value of string changes.
						strncpy(mountpoint_tmp, mount_point, sizeof(mountpoint_tmp) - 1);
						if (NULL != MountPointAddCB)
						{
							pthread_mutex_unlock(&makePartLock);
							(*MountPointAddCB) (mountpoint_tmp);
							pthread_mutex_lock(&makePartLock);
						}
					}
					else
					{
						memset(cmd, 0, sizeof (cmd));
#ifdef __JFS__
						snprintf(cmd, sizeof(cmd), "fsck.jfs -f -y %s\n", device);	//强制检测
#elif defined(__EXT3__)
						snprintf(cmd, sizeof(cmd), "fsck.ext3 -f -y %s\n", device);	//强制检测
#elif defined(__FAT32__)
						snprintf(cmd, sizeof(cmd), "dosfsck -f -y %s\n", device);	//强制检测
#elif defined(__NTFS__)
			;
#else
#error no define __JFS__ and __EXT3__
#endif
						mysystem(cmd);

                        mountret = EDVR_HD_MountNode(device, mount_point);  //!< 挂载分区
						if (0 == mountret)
						{
							char mountpoint_tmp[50];

							memset(mountpoint_tmp, 0, sizeof (mountpoint_tmp));	//if value of string changes.
							strncpy(mountpoint_tmp, mount_point, sizeof(mountpoint_tmp) - 1);
							if (NULL != MountPointAddCB)
							{
								pthread_mutex_unlock(&makePartLock);
								(*MountPointAddCB) (mountpoint_tmp);
								pthread_mutex_lock(&makePartLock);
							}
						}
					}
				}
			}					//只读挂载点处理结束
		}

		if (feof(tmp))
		{
			fclose(tmp);
			break;
		}

	}							//end while(1)

	pthread_mutex_unlock(&makePartLock);
	return;
}

/*
*/
void EDVR_HD_ReMountRo()
{
    if (!CheckDiskDamage())
    {
        printf("[%s-%d] Check disk is damaged!\n", __func__, __LINE__);
        return;
    }

	disk_readonly_check();
}

/*!
 * \brief
 * 检测到挂载点被置为RO的线程函数.
 *
 * \returns
 * void *.
 */
static void *disk_readonly()
{
    PRT_TID();

	BEGINRUN_THREAD;
	while (1)
	{
		sleep(30);
		disk_readonly_check();
	}

	ENDRUN_THREAD;
	pthread_exit(0);
}

#define USE_ONE_DISKTRANSFERHANDLE
#define WRITEFILE_BUF_LEN	(1 * 1024 * 1024)
//#define DEBUG_SHOW_WRITEFILE_BITRATE

typedef struct tag_UsbTransferHandle
{
	int m_nDevType;				// 0 表示普遍读写，1表示USB读写
	int m_nHandle;
	unsigned char m_pBuf[WRITEFILE_BUF_LEN];
	unsigned long m_dwLen;
}
USBTRANSFERHANDLE, *LPUSBTRANSFERHANDLE;

typedef struct tag_DiskTransferHandle
{
	int m_nDevType;				// 0 表示普遍读写，1表示USB读写
	unsigned char m_pBuf0[WRITEFILE_BUF_LEN];
	unsigned char m_pBuf1[WRITEFILE_BUF_LEN];
	unsigned char *m_pBufIn;	//输入缓冲区
	unsigned long m_dwLenIn;	//输入数据大小,缓冲区数据现有大小
	unsigned char *m_pBufOut;	//输出缓冲区
	unsigned long m_dwLenOut;	//输入数据大小
	unsigned long m_bWriteReturn;	// 1表示写文件成功，0表示失败
	volatile int m_bRun;		//线程退出标志
	int m_nFD;
	pthread_t m_thrdID;			//线程ID
	pthread_mutex_t m_Mutex;	//互斥锁
	sem_t m_semWake;			//唤醒线程信号
#ifdef DEBUG_SHOW_WRITEFILE_BITRATE
	unsigned long m_dwBitrate;
#endif
}
DISKTRANSFERHANDLE, *LPDISKTRANSFERHANDLE;

static void *thread_disk_writefile(void *pContext)
{
    PRT_TID();

	BEGINRUN_THREAD;

	LPDISKTRANSFERHANDLE handle = (LPDISKTRANSFERHANDLE) pContext;

	int bRun, fd;

	unsigned char *pBuf;

	unsigned long dwSize;

	unsigned long dwSizeWrite;//需要写入的数据长度
	int write_num = 0;
	int ret;

	do
	{
		//等待唤醒
		//dprintf("wait\n");
		sem_wait(&handle->m_semWake);
		//dprintf("wake up\n");
		write_num = 0;
		dwSize = 0;

		pthread_mutex_lock(&handle->m_Mutex);	//加锁
		bRun = handle->m_bRun;
		fd = handle->m_nFD;
		pBuf = handle->m_pBufOut;
		dwSizeWrite = handle->m_dwLenOut;
		//printf("my add: dwSizeWrite = %d\n",dwSizeWrite);
		pthread_mutex_unlock(&handle->m_Mutex);	//解锁

		//判断是否需要写数据到文件中
		if (NULL != pBuf)
		{
#ifdef DEBUG_SHOW_WRITEFILE_BITRATE
			unsigned long dwUseTime, dwBitrate = 0;
#endif

			if (0 != fd)
			{
#ifdef DEBUG_SHOW_WRITEFILE_BITRATE
				struct timeval tvEnd, tvBegin;

				gettimeofday(&tvBegin, NULL);
#endif

				pthread_mutex_lock(&m_RecCap);

				time_t begin = time(NULL);
#if 0

				//写入数据并清除写缓冲
				do
				{
					ret = write(fd, pBuf + dwSize, dwSizeWrite - dwSize);	
					if (-1 != ret)
					{
						dwSize += ret;
					}
					write_num++;

					if (dwSizeWrite != dwSize)
					{			//还是没有写完数据
						DISK_ERR("write %lu bytes return %lu\n", dwSizeWrite, dwSize);
						perror("write error\n");
					}
				}
				while (dwSizeWrite != dwSize && write_num <= 10);
#endif	

				do
				{
					if(dwSizeWrite - dwSize > 128*1024)//512*1024
					ret = write(fd, pBuf + dwSize, 128*1024);//dwSizeWrite - dwSize);
					else
					ret = write(fd, pBuf + dwSize, dwSizeWrite - dwSize);	
					if (-1 != ret)
					{
						dwSize += ret;
					}
					write_num++;
					usleep(5);
				}
				while (dwSizeWrite != dwSize && write_num <= 20*512/128);
				
			
				if (dwSizeWrite != dwSize)
				{			//还是没有写完数据
					DISK_ERR("write %lu bytes return %lu\n", dwSizeWrite, dwSize);
					perror("write error\n");
				}

				time_t end = time(NULL);

				if ((end - begin) > 10)
					printf("write %lu size use %lld s\n", dwSizeWrite, (long long)(end - begin));

				pthread_mutex_unlock(&m_RecCap);

#ifdef DEBUG_SHOW_WRITEFILE_BITRATE
				gettimeofday(&tvEnd, NULL);
				dwUseTime = (((tvEnd.tv_sec * 1000) + (tvEnd.tv_usec / 1000)) - ((tvBegin.tv_sec * 1000) + (tvBegin.tv_usec / 1000)));
				if (dwUseTime > 0)
				{
					dwBitrate = (dwSize * 1000 * 8) / (dwUseTime << 10);
				}

				if(dwUseTime > 100)
				{
					printf("dwUseTime = %lx \n",dwUseTime);
					dprintf("write time %lu ms, speed  %lu kbps\n", dwUseTime, dwBitrate);
				}
#endif
			}
			else
			{
				dwSize = 0;
			}


			//写入数据成功，清除写数据标志
			pthread_mutex_lock(&handle->m_Mutex);	//加锁
			handle->m_pBufOut = NULL;
			handle->m_bWriteReturn = (dwSizeWrite == dwSize) ? 1 : 0;
#ifdef DEBUG_SHOW_WRITEFILE_BITRATE
			handle->m_dwBitrate = dwBitrate;
#endif		
			pthread_mutex_unlock(&handle->m_Mutex);	//解锁
		}
	}
	while (bRun);

	ENDRUN_THREAD;
	return NULL;
}

/**
 *  \brief		Open File and return the Handler
 *
 *  \param	char *pszDestFileName	File name
 *
 *  \return	Success:  return File Description Handle
 *			Failure:  return -1
 */
int EDVR_HD_BufferTransferStart(char *pszDestFileName)
{
    if (!CheckDiskDamage())
    {
        printf("[%s-%d] Check disk is damaged!\n", __func__, __LINE__);
        return -1;
    }

	int ret;
	
	if (NULL == pszDestFileName)
	{
		return -1;
	}

	{
		int nSDKRet;

#ifdef USE_ONE_DISKTRANSFERHANDLE
		static LPDISKTRANSFERHANDLE handle = NULL;
#else
		LPDISKTRANSFERHANDLE handle = NULL;
#endif

		if (NULL == handle)
		{
#ifndef TEST_MEM_INFO
			handle = (LPDISKTRANSFERHANDLE) malloc(sizeof (DISKTRANSFERHANDLE));
#else
			handle = (LPDISKTRANSFERHANDLE)MemoryMalloc(sizeof (DISKTRANSFERHANDLE), "disk", __FILE__, __FUNCTION__, __LINE__);
#endif
			if (NULL == handle)
			{
				DISK_ERR("malloc %d bytes fail\n", sizeof (DISKTRANSFERHANDLE));
				return -1;
			}

#ifdef USE_ONE_DISKTRANSFERHANDLE
			handle->m_bRun = 0;
#endif
		}

		//初始化buf控制
		handle->m_nDevType = 0;	
		handle->m_dwLenIn = 0;
		handle->m_pBufOut = NULL;
		handle->m_bWriteReturn = 1;
		handle->m_pBufIn = handle->m_pBuf0;
		//将文件系统换成EXT3后，创建的文件权限为0，所以加上S_IRWXU
		//创建录像文件
		handle->m_nFD = open(pszDestFileName, O_CREAT | O_TRUNC  | O_WRONLY,S_IRWXU);//-
		if (-1 == handle->m_nFD)
		{
			DISK_ERR("open file %s fail\n", pszDestFileName);
			perror("open");

#ifndef USE_ONE_DISKTRANSFERHANDLE
#ifndef TEST_MEM_INFO
			free(handle);
#else
			MemoryFree(handle);
#endif
			handle = NULL;
#endif
			return -1;
		}

#ifdef USE_ONE_DISKTRANSFERHANDLE
		if (0 == handle->m_bRun)
#endif
		{
			//创建线程互斥锁
			nSDKRet = pthread_mutex_init(&handle->m_Mutex, NULL);
			
			if (0 != nSDKRet)
			{
				DISK_ERR("pthread_mutex_init() return %d\n", nSDKRet);
				perror("pthread_mutex_init");

				close(handle->m_nFD);
				handle->m_nFD = 0;
#ifndef USE_ONE_DISKTRANSFERHANDLE
#ifndef TEST_MEM_INFO
				free(handle);
#else
				MemoryFree(handle);
#endif
				handle = NULL;
#endif
				return -1;
			}
			//创建录像、抓图互斥锁
			nSDKRet = pthread_mutex_init(&m_RecCap, NULL);
			if (0 != nSDKRet)
			{
				DISK_ERR("pthread_mutex_init() return %d\n", nSDKRet);
				perror("pthread_mutex_init");

				pthread_mutex_destroy(&handle->m_Mutex);
				close(handle->m_nFD);
				handle->m_nFD = 0;
#ifndef USE_ONE_DISKTRANSFERHANDLE
#ifndef TEST_MEM_INFO
				free(handle);
#else
				MemoryFree(handle);
#endif
				handle = NULL;
#endif
				return -1;
			}

			//创建线程唤醒信号
			nSDKRet = sem_init(&handle->m_semWake, 0, 0);
			if (0 != nSDKRet)
			{
				DISK_ERR("sem_init() return %d\n", nSDKRet);
				perror("sem_init");
				pthread_mutex_destroy(&m_RecCap);
				pthread_mutex_destroy(&handle->m_Mutex);
				close(handle->m_nFD);
				handle->m_nFD = 0;
#ifndef USE_ONE_DISKTRANSFERHANDLE
#ifndef TEST_MEM_INFO
				free(handle);
#else
				MemoryFree(handle);
#endif
				handle = NULL;
#endif
				return -1;
			}

			handle->m_bRun = 1;
			struct sched_param param;
			pthread_attr_t thrd_attr;
			pthread_attr_init(&thrd_attr);
			pthread_attr_getschedparam(&thrd_attr, & param); 
			param.sched_priority--; 
			pthread_attr_setschedparam(&thrd_attr, & param); 

			//创建写文件线程
			nSDKRet = pthread_create(&handle->m_thrdID, &thrd_attr, thread_disk_writefile, handle);
			if (0 != nSDKRet)
			{
				DISK_ERR("pthread_create( thread_disk_writefile ) JOINABLE return %d\n", nSDKRet);
				perror("pthread_create");

				sem_destroy(&handle->m_semWake);
				pthread_mutex_destroy(&m_RecCap);
				pthread_mutex_destroy(&handle->m_Mutex);
				close(handle->m_nFD);
				handle->m_nFD = 0;
#ifdef USE_ONE_DISKTRANSFERHANDLE
				handle->m_bRun = 0;
#else
#ifndef TEST_MEM_INFO
				free(handle);
#else
				MemoryFree(handle);
#endif
				handle = NULL;
#endif
				return -1;
			}
		}

		ret = (int) handle;
	}

	return ret;
}

/**
 *  \brief		copy the data of buffer to the file in usb device
 *
 *  \param	int nTranferHandle		file handle
 *		 	char *pBuffer           	pointing to the address filled with user's data
 *			unsigned long	nSize	the size of buffer
 *
 *  \return	Success: return the size of written bytes
 *			Failure: return -1
 */
int EDVR_HD_BufferTransfer(int nTransferHandle, char *pBuffer, unsigned long nSize)
{
    if (!CheckDiskDamage())
    {
        printf("[%s-%d] Check disk is damaged!\n", __func__, __LINE__);
        return -1;
    }

	int size;

	if (nTransferHandle == 0 || nTransferHandle == -1)
	{
		return -1;
	}

	size = (int) nSize;

	{
		//是SD卡或硬盘
		LPDISKTRANSFERHANDLE handle = (LPDISKTRANSFERHANDLE) nTransferHandle;

		while (nSize > 0)
		{
			if ((handle->m_dwLenIn + nSize) < WRITEFILE_BUF_LEN)
			{
				memcpy(&handle->m_pBufIn[handle->m_dwLenIn], pBuffer, nSize);
				handle->m_dwLenIn += nSize;

				pthread_mutex_lock(&handle->m_Mutex);	//加锁

				//更新返回值
				if (0 == handle->m_bWriteReturn)
				{
					size = -1;
				}

				//切换导入导出缓冲区
				if (handle->m_pBufOut == NULL)
				{
					if(handle->m_dwLenIn >= 64*1024)
					{
						//printf("write   handle->m_dwLenOut = %d \n",handle->m_dwLenOut);
						handle->m_pBufOut = handle->m_pBufIn;
						handle->m_dwLenOut = handle->m_dwLenIn;
						pthread_mutex_unlock(&handle->m_Mutex);	
						//激活写入线程
						//dprintf("post full\n");
						sem_post(&handle->m_semWake);

						if (handle->m_pBuf0 == handle->m_pBufIn)
						{
							handle->m_pBufIn = handle->m_pBuf1;
						}
						else
						{
							handle->m_pBufIn = handle->m_pBuf0;
						}
						handle->m_dwLenIn = 0;
					}
                    else
				    	pthread_mutex_unlock(&handle->m_Mutex);	//加锁
				}
				else
				{
					pthread_mutex_unlock(&handle->m_Mutex);	
				}

				break;
			}
			else
			{
				unsigned long len = WRITEFILE_BUF_LEN - handle->m_dwLenIn;

				memcpy(&handle->m_pBufIn[handle->m_dwLenIn], pBuffer, len);

#ifdef DEBUG_SHOW_WRITEFILE_BITRATE
				unsigned long times = 0, bytes = 0;
#endif

				pthread_mutex_lock(&handle->m_Mutex);	//加锁
				
				while (NULL != handle->m_pBufOut)
				{
#ifdef DEBUG_SHOW_WRITEFILE_BITRATE
					if (0 == bytes)
					{
						bytes = WRITEFILE_BUF_LEN >> 10;
						dprintf("need wait fwrite %lu bytes ok\n", bytes);
					}
					++times;
#endif

					pthread_mutex_unlock(&handle->m_Mutex);	//解锁
					usleep(5 * 1000);	//延时5ms
					pthread_mutex_lock(&handle->m_Mutex);	//加锁
				}

#ifdef DEBUG_SHOW_WRITEFILE_BITRATE
				if (times > 0)
				{
					dprintf("wait write %lu KB use %lu ms, %lu kbps\n", bytes, 5 * times, handle->m_dwBitrate);
				}
#endif

				//切换导入导出缓冲区
				handle->m_pBufOut = handle->m_pBufIn;
				handle->m_dwLenOut = WRITEFILE_BUF_LEN;

				//更新返回值
				if (0 == handle->m_bWriteReturn)
				{
					size = -1;
				}
				pthread_mutex_unlock(&handle->m_Mutex);	//解锁
				
				//激活写入线程
				//dprintf("post full\n");
				printf("write  2  handle->m_dwLenOut = %lu \n",handle->m_dwLenOut);
				sem_post(&handle->m_semWake);
				if (handle->m_pBuf0 == handle->m_pBufIn)
				{
					handle->m_pBufIn = handle->m_pBuf1;
				}
				else
				{
					handle->m_pBufIn = handle->m_pBuf0;
				}				
				handle->m_dwLenIn = 0;
				nSize -= len;
				pBuffer += len;
			}
		}
	}

	return size;
}

/**
 *  \brief		Close the openned file
 *
 *  \param	int nTransferHandle		Openned file handle
 *
 *  \return	Success: return 0
 *			Failure: return -1
 */
int EDVR_HD_BufferTransferStop(int nTransferHandle)
{
    if (!CheckDiskDamage())
    {
        printf("[%s-%d] Check disk is damaged!\n", __func__, __LINE__);
        return -1;
    }

	int ret;

	if (nTransferHandle == 0 || nTransferHandle == -1)
	{
		return -1;
	}

	{
		LPDISKTRANSFERHANDLE handle = (LPDISKTRANSFERHANDLE) nTransferHandle;

#ifdef USE_ONE_DISKTRANSFERHANDLE

		//等待导出缓冲区清空
		pthread_mutex_lock(&handle->m_Mutex);	//加锁
		while (NULL != handle->m_pBufOut)
		{
			pthread_mutex_unlock(&handle->m_Mutex);	//解锁
			usleep(5 * 1000);	//延时5ms
			pthread_mutex_lock(&handle->m_Mutex);	//加锁
		}
		pthread_mutex_unlock(&handle->m_Mutex);	//解锁

#else

		int nSDKRet;

		//设置标志，停止线程
		pthread_mutex_lock(&handle->m_Mutex);
		handle->m_bRun = 0;
		pthread_mutex_unlock(&handle->m_Mutex);

		//激活线程
		sem_post(&handle->m_semWake);

		//等待线程退出
		nSDKRet = pthread_join(handle->m_thrdID, NULL);
		if (0 != nSDKRet)
		{
			DISK_ERR("pthread_join() return %d\n", nSDKRet);
			perror("pthread_join");
		}

		//销毁线程同步资源
		nSDKRet = sem_destroy(&handle->m_semWake);
		if (0 != nSDKRet)
		{
			DISK_ERR("sem_destroy() return %d\n", nSDKRet);
			perror("sem_destroy");
		}

		nSDKRet = pthread_mutex_destroy(&handle->m_Mutex);
		if (0 != nSDKRet)
		{
			DISK_ERR("pthread_mutex_destroy() return %d\n", nSDKRet);
			perror("pthread_mutex_destroy");
		}

#endif

		//写入最后的数据
		if (handle->m_dwLenIn > 0)
		{
			write(handle->m_nFD, handle->m_pBufIn, handle->m_dwLenIn);
			handle->m_pBufIn = NULL;
			handle->m_dwLenIn = 0;
		}

		ret = close(handle->m_nFD);
		handle->m_nFD = 0;
#ifndef USE_ONE_DISKTRANSFERHANDLE
#ifndef TEST_MEM_INFO
		free(handle);
#else
		MemoryFree(handle);
#endif
		handle = NULL;
#endif
	}

	return ret;
}

static HD_EventCallback s_hRemoveEventHandle = NULL;	/* handle disk remove event */

static HD_EventCallback s_hAddEventHandle = NULL;	/* handle disk add event */

static DiskActType s_DiskActType = DISKACT_NONE;	/* event type by handled */

static int s_bCheckSDCardHotplug = 0;

static int s_bSDCardStatus = -1;

static pthread_t s_threadCheckSDCardHotplug;

#if defined(EGUI_FOR_ICAMERA_NXP8850_BASED_DVR)

/*!
 * \brief
 * 检测到SD卡热插拔的线程函数.
 *
 * \returns
 * void *.
 */
static void *sdcard_hotplug()
{
    PRT_TID();

	DIR *dp;

	int no, nStatus;

	char syspath[40];

	static char szCurrentSDDev[20] = DEV_PATH"0";

	BEGINRUN_THREAD;

	while (1)
	{
		sleep(1);

		//检测SD卡, 0 表示有SD卡，1表示没有SD卡
		if (Hotplug_flag == 1)
        {
            continue;
        }
		nStatus = 1;
		for (no = 0; no < MAX_SDCARD_NUM; ++no)
		{
			Get_SDCard_SysPath(no, syspath, sizeof (syspath));

			if ((dp = opendir(syspath)) == NULL)
			{
				DPRINTF("can't open directory: %s\n", syspath);
				continue;
			}
			closedir(dp);

			snprintf(szCurrentSDDev, sizeof (szCurrentSDDev), DEV_PATH_FORMAT, no);
			nStatus = 0;
			break;
		}

		if (-1 == s_bSDCardStatus)
		{
			s_bSDCardStatus = nStatus;
		}
		else if (s_bSDCardStatus != nStatus)
		{
			if (0 == nStatus)
			{
				if (NULL != s_hAddEventHandle)
				{
					dprintf("add sd card\n");
					sleep(3);
					(*s_hAddEventHandle) (szCurrentSDDev);
				}
			}
			else
			{
			    sleep(6);
                if (Hotplug_flag == 1)
                {
                    continue;
                }
				if (NULL != s_hRemoveEventHandle)
				{
					dprintf("remove sd card\n");
					(*s_hRemoveEventHandle) (szCurrentSDDev);
				}
			}

			s_bSDCardStatus = nStatus;
		}
	}

	ENDRUN_THREAD;
	pthread_exit(0);
}
#endif

/**
 *  \brief  setting callback functions handling the kind of hotplug-events
 *
 *  \param  DeviceActType devtype
 *			ACT_ALL     sending hotplug-events to all removable devices
 *			ACT_SATA    sending hotplug-events to removable sata devices
 *			ACT_USB	    sending hotplug-events to removable usb devices
 *			ACT_CDROM	sending hotplug-events to removable cdrom devices
 *			ACT_SDCARD	sending hotplug-events to removable sdcard devices
 *			ACT_NONE	not handle hotplug-events
 *
 *  \return void
 */
void EDVR_HD_SetEventType(DiskActType devtype)
{
	if (DISKACT_SDCARD & devtype)
	{
		//start the partition read only checking thread
		if (0 == s_bCheckSDCardHotplug)
		{
			int ret;

			ret = pthread_create(&s_threadCheckSDCardHotplug, NULL, sdcard_hotplug, NULL);

			if (ret != 0 || (s_threadCheckSDCardHotplug == 0))
			{
				if (EAGAIN == ret)
					DISK_ERR("pthread_create failed with EAGAIN (thread num limit arrived)\n");
				else if (EINVAL == ret)
					DISK_ERR("pthread_create failed with EINVAL (create thread with invalid parameters)\n");
				else if (ENOMEM == ret)
					DISK_ERR("pthread_create failed with ENOMEM (no memeory to create thread)\n");
				else
					DISK_ERR("pthread_create failed with unkonen error code %d\n", ret);
			}
			s_bCheckSDCardHotplug = 1;
		}

	}

	s_DiskActType = devtype;
}

/**
 *  \brief handle usb device remove event, and invoke call-back functions
 *
 *  \param	HD_EventCallback pRemoveCallback
 *			call-back functions for usb device unplugged
 *
 *  \return	0	successed
 *			1	event has already been installed
 *			2      error while installed signal
 */
int EDVR_HD_DeviceRemoveEvent(HD_EventCallback pRemoveCallback)
{
	s_hRemoveEventHandle = pRemoveCallback;
	return 0;
}

/**
 *  \brief handle usb device add event, and invoke call-back functions
 *
 *  \param	HD_EventCallback pAddCallback
 *			call-back functions for usb device plugged
 *
 *  \return	0	successed
 *			1	event has already been installed
 *			2	error while installed signal
 */
int EDVR_HD_DeviceAddEvent(HD_EventCallback pAddCallback)
{
	s_hAddEventHandle = pAddCallback;
	return 0;
}
