#ifndef _INC_FDISK_H_
#define _INC_FDISK_H_

#include <stdio.h>
#include <stdint.h>
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

#define TRUE 						(1)
#define FALSE 					    (0)
#define MAX_DISK_NUM 			    (10)
#define READONLY    				(0)
#define READWRITE   				(1)
#define MAX_DISK_PARTITIONSEX	    (14)

typedef unsigned char 		uint8_t;
typedef unsigned short 		uint16_t;
typedef unsigned int 		uint32_t;

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

//fat32 启动扇区结构
struct boot_sector {
    uint8_t ignored[3];		/* Boot strap short or near jump */
    uint8_t system_id[8];	/* Name - can be used to special case
				   partition manager volumes */
    uint8_t sector_size[2];	/* bytes per logical sector */ //扇区大小 512字节
    uint8_t cluster_size;	/* sectors/cluster */		//每簇扇区数 16k = 32个        4k = 8个
    uint16_t reserved;		/* reserved sectors */		//保留扇区数量  
    uint8_t fats;		/* number of FATs */			//fat文件表个数，一般2个，一个主，一个备份
    uint8_t dir_entries[2];	/* root directory entries */ //FAT32必须等于0，FAT12/FAT16为根目录中目录的个数
    uint8_t sectors[2];		/* number of sectors */ //FAT32必须等于0，FAT12/FAT16为扇区总数
    uint8_t media;		/* media code (unused) */ //哪种存储介质，0xF8标准值，可移动存储介质。
    uint16_t fat_length;	/* sectors/FAT */  //FAT32必须为0，FAT12/FAT16为?个FAT 表所占的扇区数。 
    uint16_t secs_track;	/* sectors per track */ //每磁道扇区数，只对于  “特殊形状”  （由磁头和柱?分割为若?磁道）的存储介质有效，0x003F=63。 
    uint16_t heads;		/* number of heads */ //磁头数，只对特殊的介质才有效，0x00FF=255。 
    uint32_t hidden;		/* hidden sectors (unused) */ //EBR分区之前所隐藏的扇区数
    uint32_t total_sect;	/* number of sectors (if sectors == 0) */ //文件系统总扇区数

    /* The following fields are only used by FAT32 */
    uint32_t fat32_length;	/* sectors/FAT */  //每个FAT表使用的扇区数
    uint16_t flags;		/* bit 8: fat mirroring, low 4: active fat */
    uint8_t version[2];		/* major, minor filesystem version */
    uint32_t root_cluster;	/* first cluster in root directory */  //根目录所在簇的簇号，0x02
    uint16_t info_sector;	/* filesystem info sector */ //FSINFO（文件系统信息扇区）扇区号0x01，该扇区为操作系统提供关于空簇总数及下一个可用簇的信息。 
    uint16_t backup_boot;	/* backup boot sector */ //备份引导扇区的位置。备份引导扇区总是位于文件系统的6号扇区
    uint8_t reserved2[12];	/* Unused */

    uint8_t drive_number;	/* Logical Drive Number */
    uint8_t boot_flags;		/* bit 0: dirty, bit 1: need surface test */

    uint8_t extended_sig;	/* Extended Signature (0x29) */
    uint32_t serial;		/* Serial number */ //卷序列号。通常为随机值
    uint8_t label[11];		/* FS label */ //卷标  （ASCII码），如果创建文件系统的时候指定了卷标，会保存在此
    uint8_t fs_type[8];		/* FS Type */ //文件系统格式的ASCII码，FAT32

    /* fill up to 512 bytes */
    uint8_t junk[422];
} __attribute__ ((packed));

//分区表项
typedef struct {
	unsigned char boot_flag;			//启动标志
	unsigned char start_magnetic[3];	//磁头号、扇区号、柱面号
	unsigned char fs_type;			//文件系统类型
	unsigned char end_magnetic[3];	//磁头号、扇区号、柱面号
	unsigned char start_sector[4];	//起始扇区
	unsigned char total_sector[4];	    //总扇区数
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
#define SDCARD_MOUNTPOINT_PATH	"/dat/mmcblk"
#define IS_SDCARD(dev)				((strstr(dev, SDCARD_DEVNODE_PATH) == dev) && (strlen(dev) > 11))

#define IS_SDCARD_MOUNTPOINT(dev)	((strstr(dev, SDCARD_MOUNTPOINT_PATH) == dev) && (strlen(dev) > 11))
#define IS_SDCARD_PARTITION(dev)	((strstr(dev, SDCARD_DEVNODE_PATH) == dev) && (strlen(dev) >= 14) && ('p' == dev[12]))

#define IS_FAT_PARTITION(x)     ((0x6 == (x)) ||(0xB == (x)) || (0xC == (x)) || (0xE == (x))||(0x83==(x))||(0x7==(x)))
#define IS_EXTERN_PARTITION(x)	((0x5 == (x)) ||(0xF == (x)))

#define FDISK_ERR(format, args...)  do{printf("\033[1;31m[FDISK][%s-%d]\033[m" format, __FILE__, __LINE__, ##args);}while(0)
#define FDISK_DBG(format, args...)  do{printf("\033[1;32m[FDISK][%s-%d]\033[m" format, __FILE__, __LINE__, ##args);}while(0)
#define FDISK_INFO(format, args...)  do{printf("\033[1;35m[FDISK][%s-%d]\033[m" format, __FILE__, __LINE__, ##args);}while(0)
#define FDISK_SYS(format, args...)  do{printf("\033[1;32m[FDISK][%s-%d](%s) \033[m" format, __FILE__, __LINE__, strerror(errno), ##args);}while(0)

#ifdef __cplusplus
    extern "C" {
#endif

int fdisk(const char *hd_device, int parts);

/*!
 * \brief 初始化硬盘。初始化后，该硬盘只有一个系统保留区域(大小为nSize)一个扩展分区
 * \param pcDev 设备节点名
 * \param int nPartitionIndex 分区索引
 * \return 成功返回0,-1为没有找到该分区，-2为分区忙
 */
int InitDev(const char *dev, unsigned long nSize);

/*!
 * \brief 添加分区
 * \param dev 设备节点名
 * \param int nSize 大小单位(M)
 * \return 成功返回0,
 * -1分区过大，请缩小分区容量.-2无连续空间分配给该分区-3已经达到最大分区个数
 * -4该硬盘未被初始化，无法添加分区
 */
int AddPartition(char *dev, unsigned long nSize);

/*!
 * \brief 删除分区
 * \param dev		硬盘设备节点名
 * \param part		分区号
 * \return 成功返回0,-1为没有找到该分区，-2为分区忙,-3系统保留分区无法删除
 */
int DelPartition(char *dev, int part);


int GetPartitionInfo(const char *dev, struct Disk_Desc *ptDiskDesc);

#ifdef __cplusplus
}
#endif

#endif							//_INC_FDISK_H_
