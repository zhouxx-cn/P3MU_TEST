/**
****************************************************************************************
 * @FilePath: upgrade_types.h
 * @Author: HL.LIU
 * @Date: 2022-09-01 17:25:32
 * @LastEditors: 
 * @LastEditTime: 2022-09-01 17:25:32
 * @Copyright: 2022 RUISION CO.,LTD. All Rights Reserved.
 * @Descripttion: 
****************************************************************************************
*/
#ifndef __UPGRADE_TYPES_H__
#define __UPGRADE_TYPES_H__

#include "xrsdef.h"

/******************************************************************************
 * 升级文件存储结构示意图，头共8K
 * +++++++++++++++++++++++++++++++++<--------------------+<-------------------+
 * +                               +                     +                    +
 * +    UPGRADE FILE TYPE信息      +    144 Bytes        +                    +
 * +                               +                     +                    +
 * +-------------------------------+<--------------------+<-------------------+
 * +-                             -+                     +                    +
 * +-   UPGRADE FILE KEY信息      -+    768 Bytes        +                    +
 * +-                             -+                     +                    +
 * +-------------------------------+<--------------------+<-------------------+
 * +-                             -+                     +                    +
 * +-   UPGRADE FILE HEADERS信息  -+    4608 Bytes       +                    +
 * +-                             -+                     +                    +
 * +-------------------------------+<--------------------+<-------------------+
 * +-                             -+                     +                    +
 * +-   RESERVED                  -+    2672 Bytes       +                    +
 * +-                             -+                     +                    +
 * +-------------------------------+<--------------------+<-------------------+
 * +-                             -+                     +                    +
 * +-   UPGRADE FILE DATA         -+                     +                    +
 * +-                             -+                     +                    +
 * +-------------------------------+<--------------------+<-------------------+
 ******************************************************************************/

#define UPGRADE_MAGIC				"BINMAGIC"					/* Magic */
#define UPGRADE_MAGIC_LEN			(8)							/* Magic 长度 */
#define UPGRADE_VER					"1.0"						/* 升级包包头的版本 */
#define UPGRADE_VER_LEN				(4)							/* 升级包版本字段长度 */
#define PACKAGE_VER_LEN				(32)						/* 设备版本号最长长度 */
#define PACKAGE_CHECKVALUE_LEN		(32)						/* 校验值最长长度 */
#define BOARD_TYPE_NAME_LEN			(32)						/* 设备版型最长长度 */
#define PACKAGE_SUPPORT_BOARD_NUM	(8)							/* 升级包最多支持可升级的版型个数 */
#define PACKAGE_SUPPORT_HW_CODE_NUM (16)						/* 4个GPIO口最多支持16种机型 */
#define PACKAGE_MAX_PARTITION_NUM   (32)						/* 升级包包含的分区及文件的最大总个数 */
#define PACKAGE_IMG_PATH_LEN		(64)						/* 路径最大长度 */

typedef enum
{
    FLASH_PART_BIN, 
    FLASH_PART_CRAMFS, 
    FLASH_PART_YAFFS2, 
    FLASH_PART_JFFS2, 
    FLASH_PART_SQUASHFS, 
    FLASH_PART_EXT4, 
    FLASH_PART_UBIFS, 
    FLASH_PART_BUTT
} FLASH_PART_TYPE_E;

/* 升级包的类型 */
typedef enum
{
	PACKAGE_TYPE_UNKNOW = 0,
	PACKAGE_TYPE_UPGRADE,
	PACKAGE_TYPE_RESTORE,
	PACKAGE_TYPE_RESUME,
} PACKAGE_TYPE_E;

/* 分区的类型 */
typedef enum
{
	PARTITION_TYPE_BOOT,
	PARTITION_TYPE_ENV,
	PARTITION_TYPE_ROOTFS,
	PARTITION_TYPE_ROOTFSB,
	PARTITION_TYPE_DATA,
	PARTITION_TYPE_DATB,
	PARTITION_TYPE_USR,			/* 非分区，用户文件，根据UPGRADE_TYPE_E来决定操作 */
	PARTITION_TYPE_BUTT,
} PARTITION_TYPE_E;

/* 镜像的升级类型 */
typedef enum
{
	UPGRADE_TYPE_ERASURE,		/* 擦除方式 */
	UPGRADE_TYPE_TAR,			/* tar解压方式 */
	UPGRADE_TYPE_OVERLAY,		/* 覆盖方式 */
	UPGRADE_TYPE_BUTT  
} UPGRADE_TYPE_E;

/* 校验类型 */
typedef enum CheckType
{
	CHECK_TYPE_CRC32 = 0,
	CHECK_TYPE_MD5,
	CHECK_TYPE_BUTT
} CHECK_TYPE_E;

/* 分区的信息结构体 128B */
typedef struct 
{
	unsigned int  Type;								/* 分区的类型，PARTITION_TYPE_E */
	unsigned int  ImageType;						/* 分区镜像的类型，UPGRADE_TYPE_E */
	unsigned int  ImageOffset;
	unsigned int  ImageSize;
	char Path[PACKAGE_IMG_PATH_LEN];				/* 
														升级文件绝对路径, 
														ImageType=UPGRADE_TYPE_ERASURE时，无意义
														ImageType=UPGRADE_TYPE_TAR时，将该文件解压到Path目录下 
														ImageType=UPGRADE_TYPE_OVERLAY时，将该文件写入到Path下，Path含文件名 
													*/
	unsigned char Rsvd[48];
}PARTITION_INFO_S;
#define PARTITION_INFO_S_LEN	sizeof(PARTITION_INFO_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(PARTITION_INFO_S, 128);

/* 主机升级的头信息 4K+512B */
typedef struct 
{
	unsigned int  Valid;									/* 0:不存在该包，1：存在 */
	unsigned int  Count;									/* 分区个数 */
	PARTITION_INFO_S Info[PACKAGE_MAX_PARTITION_NUM];		/* 分区信息 */
	unsigned char Rsvd[504];								/* 保留 */
}HOST_UPGRADE_INFO_HEADER_S;
#define HOST_UPGRADE_INFO_HEADER_S_LEN	sizeof(HOST_UPGRADE_INFO_HEADER_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(HOST_UPGRADE_INFO_HEADER_S, 4 * 1024 + 512);

/* 外设升级文件信息 128B */
typedef struct 
{
	unsigned int  Valid;									/* 0:无效，1：有效 */
	unsigned int  ImageOffset;								/* 外设升级包的偏移地址 */
	unsigned int  ImageSize;								/* 外设升级包的包长度 */

	unsigned char Rsvd[116];								/* 保留 */
} SLAVE_UPGRADE_FILE_INFO_S;
#define SLAVE_UPGRADE_FILE_INFO_S_LEN	sizeof(SLAVE_UPGRADE_FILE_INFO_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(SLAVE_UPGRADE_FILE_INFO_S, 128);

/* 升级包信息结构体，包含主机跟外设从机升级包信息 6KB */
typedef struct 
{
	HOST_UPGRADE_INFO_HEADER_S	HostInfo;		/* 主机升级包信息 */
	SLAVE_UPGRADE_FILE_INFO_S	SlaveInfo;		/* 从机升级包信息 */
	unsigned char Rsvd[1408];					/* 保留 */
} UPGRAGE_FILE_ALL_INFO_S;
#define UPGRAGE_FILE_ALL_INFO_S_LEN	sizeof(UPGRAGE_FILE_ALL_INFO_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(UPGRAGE_FILE_ALL_INFO_S, 6 * 1024);

/* 升级包支持的版型信息 260B */
typedef struct 
{
	unsigned int  count;									/* 升级包支持的版型个数，最大UPGRAGE_FILE_MAX_SUPPORT_BOARD_TYPE_COUNT */
	char BoardTypeList[PACKAGE_SUPPORT_BOARD_NUM][BOARD_TYPE_NAME_LEN];/* 版型信息 */
} BOARD_TYPE_SUPPORT_INFO_S;
#define BOARD_TYPE_SUPPORT_INFO_S_LEN	sizeof(BOARD_TYPE_SUPPORT_INFO_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(BOARD_TYPE_SUPPORT_INFO_S, 260);

/* 升级包校验信息 64B */
typedef struct 
{
	unsigned int  Type;										/* CHECK_TYPE_E 校验值的类型，默认0：crc32校验，1：MD5全包校验，2：摘要校验（规则待定） */
	char CheckValue[PACKAGE_VER_LEN];	/* 校验值，计算时需要调过此字段 */
	unsigned char Rsvd[28];									/* 保留 */
} UPGRAGE_FILE_CHECK_INFO_S;
#define UPGRAGE_FILE_CHECK_INFO_S_LEN	sizeof(UPGRAGE_FILE_CHECK_INFO_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(UPGRAGE_FILE_CHECK_INFO_S, 64);

/* 升级限制信息,704B */
typedef struct
{
	char UpgradePackageVersion[PACKAGE_VER_LEN];	/* 当前升级包的版本 */
	char UpgradeBaseVersion[PACKAGE_VER_LEN];	/* 升级包升级限制的基础版本，低于此版本无法升级 */
	unsigned int  HwIdCode; 										/* 硬件识别码(4个GPIO组成的机型), 共支持16种硬件机型，置位代表支持 */
	BOARD_TYPE_SUPPORT_INFO_S BoardTypeSupportInfo;				    /* 支持升级的版型信息 */
    unsigned char Rsvd[376];
} UPGRADE_KEY_S, *PUPGRADE_KEY_S;
#define UPGRADE_KEY_S_LEN	sizeof(UPGRADE_KEY_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(UPGRADE_KEY_S, 704);

/* 升级包包头信息,8K */
typedef struct 
{
	char Magic[UPGRADE_MAGIC_LEN];					/* BINMAGIC */
	char Version[UPGRADE_VER_LEN];                  /* 包头版本号 UPGRADE_FILE_HEADER_VER */
	unsigned int PackageType;						/* 升级包类型，PACKAGE_TYPE_E */
	unsigned int EncryptType;						/* 升级包加密类型，ENCRYPT_TYPE_E */
	unsigned char Rsvd0[124];
    UPGRAGE_FILE_CHECK_INFO_S UpgrageFileCheckInfo;	/* 升级包的校验值信息 */
	UPGRADE_KEY_S UpgradeKeyInfo;					/* 升级包的升级限制等信息（校验、机型、版本、版型） */
	UPGRAGE_FILE_ALL_INFO_S	UpgradeFileAllInfo;		/* 主机升级包分区信息，及从机升级包信息 */
	unsigned int PayloadSize;						/* 负载大小 */
	unsigned char Rsvd1[1132];
} UPGRADE_FILE_INFO_HEADER_S,*PUPGRADE_FILE_INFO_HEADER_S;
#define UPGRADE_FILE_INFO_HEADER_S_LEN	sizeof(UPGRADE_FILE_INFO_HEADER_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(UPGRADE_FILE_INFO_HEADER_S, 8 * 1024);

#endif // __UPGRADE_TYPES_H__