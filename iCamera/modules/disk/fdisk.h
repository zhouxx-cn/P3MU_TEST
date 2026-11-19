#ifndef _INC_FDISK_H_
#define _INC_FDISK_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <mntent.h>
#include <sys/vfs.h>
#include <stdarg.h>
#include <dirent.h>

#include <fcntl.h>				/* O_RDWR */
#include <errno.h>				/* ERANGE */
#include <getopt.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <linux/unistd.h>		/* _syscall */
#include <linux/hdreg.h>		/* HDIO_GETGEO */
#include <sys/time.h>

typedef struct
{
	unsigned char h, s, c;
} __attribute__ ((packed)) chs;	/* has some c bits in s */

struct partition
{
	unsigned char bootable;		/* 0 or 0x80 */
	chs begin_chs;
	unsigned char sys_type;
	chs end_chs;
	unsigned int start_sect;	/* starting sector counting from 0 */
	unsigned int nr_sects;		/* nr of sectors in partition */
} __attribute__ ((packed));


struct Part_Desc
{
	unsigned int dwSector;
	unsigned int nStart_Sec;
	unsigned int nEnd_Sec;
	unsigned char Type;
};

//分区表中的文件系统类型值
typedef enum {
	FS_FAT32_VALUE = 0x0B
}FS_TYPE;


//分区表项
typedef struct {
	unsigned char boot_flag;			//启动标志
	unsigned char start_magnetic[3];	//磁头号、扇区号、柱面号
	unsigned char fs_type;			//文件系统类型
	unsigned char end_magnetic[3];	//磁头号、扇区号、柱面号
	unsigned char start_sector[4];	//起始扇区
	unsigned char total_sector[4];	    //结束扇区
}__attribute__ ((packed)) PartitionTableEntry;


//MBR扇区结构
typedef struct {
	unsigned char boot[446];
	PartitionTableEntry ptb[4];
	unsigned char finish[2];
}__attribute__ ((packed)) Mbr_Sector;

//分区最多的情况是14个FAT32的逻辑分区
#define MAX_DISK_PARTITIONS		(MAX_DISK_PARTITIONSEX + 4)

struct Disk_Desc
{
	int iPartNum;
	struct Part_Desc atPartitions[MAX_DISK_PARTITIONS];
};

//SD卡分区设备节点名如 /dev/mmcblk0
#define SDCARD_DEVNODE_PATH		"/dev/mmcblk"
#define SDCARD_MOUNTPOINT_PATH	"/tmp/app/disk/mmcblk"
#define IS_SDCARD(dev)				((strstr(dev, SDCARD_DEVNODE_PATH) == dev) && (strlen(dev) > 11))

#define IS_SDCARD_MOUNTPOINT(dev)	((strstr(dev, SDCARD_MOUNTPOINT_PATH) == dev) && (strlen(dev) > 11))
#define IS_SDCARD_PARTITION(dev)		((strstr(dev, SDCARD_DEVNODE_PATH) == dev) && (strlen(dev) >= 14) && ('p' == dev[12]))

#define IS_FAT_PARTITION(x)   ((0x6 == (x)) ||(0xB == (x)) || (0xC == (x)) || (0xE == (x))||(0x83==(x))||(0x7==(x)))
#define IS_EXTERN_PARTITION(x)	((0x5 == (x)) ||(0xF == (x)))

unsigned long get_hd_space(const char *dev);

void Get_Partition_DevNode(const char *dev, int no_part, char *dev_part, int len);

//修改分区表中分区的文件系统类型
int Change_DevFileSystemType(const char *dev,int partition,unsigned char fs_type);

//获取分区表中指定分区的扇区总数以及开始扇区
int Get_PartitionSector(const char *dev,int partition,unsigned int *nTotalSector,unsigned int *nStartSector);

//获取硬盘总容量. 以扇区为单位
unsigned long Get_HdTotalSector(const char *dev);

//重新读取设备分区并生成结点
int fdisk_reread_partition(char *dev);


#define DEBUG
#ifdef DEBUG
#define dprintf(x...)		printf("[DISK][%s:%d]: ",__FUNCTION__,__LINE__);printf(x)
#else
#define dprintf(x...)
#endif

//#define DPRINTF(x...) printf("\n[%s %d]",__FUNCTION__,__LINE__);printf(x)
#ifndef DPRINTF
#define DPRINTF(x...)
#endif

//#define DEBUG_MSG(x...) printf("\n[%s %d]",__FUNCTION__,__LINE__);printf(x)
#ifndef DEBUG_MSG
#define DEBUG_MSG(x...)
#endif

#define DISK_ERR(x...)												\
do																\
{																\
	printf("[DISK][ERR][%s-%s:%d]: ", __FILE__, __FUNCTION__, __LINE__);	\
	printf(x);														\
}																\
while (0)



#define DISK_BEGIN_TIME					\
{	struct timeval g_tv, g_tv_last;				\
	unsigned long g_dwRealDisTime;			\
	gettimeofday (&g_tv_last, NULL);


#define DISK_END_TIME					\
	gettimeofday (&g_tv, NULL);				\
	g_dwRealDisTime = (((g_tv.tv_sec * 1000) + (g_tv.tv_usec / 1000)) - ((g_tv_last.tv_sec * 1000) + (g_tv_last.tv_usec / 1000)));	\
	dprintf("use %lu ms\n", g_dwRealDisTime);	\
}

#if 0
#define DISK_USE_TIME(x)					\
do										\
{	struct timeval tv, tv_last;					\
	unsigned long dwRealDisTime;			\
	gettimeofday (&tv_last, NULL);			\
	x;									\
	gettimeofday (&tv, NULL);				\
	dwRealDisTime = (((tv.tv_sec * 1000) + (tv.tv_usec / 1000)) - ((tv_last.tv_sec * 1000) + (tv_last.tv_usec / 1000)));	\
	dprintf("use %lu ms\n", dwRealDisTime);		\
}								\
while (0)
#else
#define USE_TIME(x)	\
do					\
{					\
	x;				\
}					\
while (0)
#endif



#endif							//_INC_FDISK_H_
