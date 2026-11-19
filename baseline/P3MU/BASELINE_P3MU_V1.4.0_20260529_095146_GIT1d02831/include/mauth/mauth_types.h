/**
****************************************************************************************
 * @FilePath: mauth_types.h
 * @Author: HL.LIU
 * @Date: 2022-09-01 17:28:42
 * @LastEditors: 
 * @LastEditTime: 2022-09-01 17:28:43
 * @Copyright: 2022 RUISION CO.,LTD. All Rights Reserved.
 * @Descripttion: 
****************************************************************************************
*/
#ifndef _MAUTH_TYPES_H_
#define _MAUTH_TYPES_H_

#include "xrsstd.h"
#include "xrsdef.h"

/**************************************************************************************************************
 * 文件存储结构示意图:
 * 1.     EEPROM占用   4KB,使用前 4KB字节;
 * 2.  NOR FLASH占用 128KB,使用前128KB字节;
 * 3. NAND FLASH占用   2MB,使用前128KB字节;
 * 3. eMMC FLASH占用   2MB,使用前128KB字节.
 * +++++++++++++++++++++++++++++++++<-----------------------------------------------+<----------+<------------|
 * +                               +                          +                     +            +            |
 * +    ENCRYPT DATA MD5 or SHA256 + MAX SIZE       32 Bytes  +     32 Bytes        +            +            |
 * +                               +                          +                     +            +            |
 * +++++++++++++++++++++++++++++++++<-------------------------+<--------------------+ 512 Bytes  +            |
 * +-                             -+                          +                     +            +            |
 * +-   MAUTH INFO                -+ MAUTH INFO    224 Bytes  +     224 Bytes       +            +            |
 * +-                             -+                          +                     +            +            |
 * +-------------------------------+<-------------------------+<--------------------+------------+ 128K Bytes |
 * +-   MAUTH DEVS DATA           -+ DEV ATTR     1536 Bytes  +     1536 Bytes      + 1536 Bytes +            |
 * +-------------------------------+<-------------------------+<--------------------+<-----------+            |
 * +-                             -+                          +                     +            +            |
 * +-                             -+                          +                     +            +            |
 * +-   MAUTH OEM DATA            -+ OEM DATA      256 Bytes  +      1024 Bytes     + 1024 Bytes +            |
 * +-                             -+ RESERVED      768 KBytes +                     +            +            |
 * +-                             -+                          +                     +            +            |
 * +-                             -+                          +                     +            +            |
 * +-------------------------------+<-------------------------+<--------------------+<-----------+            |
 * +-   MAUTH USR DATA            -+ USR DATA      62 KBytes  +  62 KBytes          +  61 KBytes +            |
 * +-------------------------------+<-------------------------+<--------------------+<-----------+            |
 * +-   RESERVED DATA             -+ RESERVED      64 KBytes  +  64 KBytes          +  64 KBytes +            |
 * +++++++++++++++++++++++++++++++++<-----------------------------------------------+<----------+<----------  |
 **************************************************************************************************************/
#define MAX_ENCRYPT_DATA_LEN    32          /* MD5 or SHA256 */
#define MAUTH_FILE_HEAD_FLAG    0x00535258  /* "XRS ": MAUTH HEADER */
#define MAX_MAUTH_FILE_SIZE     2048
#define MAX_MAUTH_OEMD_SIZE     1024
#define MAX_MAUTH_AES_RAND      0xFFFF
#define MAX_SERIAL_NUM_LEN	   	(32)
#define MAX_MAUTH_USRD_SIZE     (61 * 1024)
#define MAX_MAUTH_USR_HEAD_LEN  (16)                                            /* USR HEAD长度 */
#define MAX_MAUTH_USR_DATA_LEN  (MAX_MAUTH_USRD_SIZE - MAX_MAUTH_USR_HEAD_LEN)  /* USR DATA长度 */
#define MAX_MAUTH_AI_SIZE     (64 * 1024)
#define MAX_MAUTH_AI_HEAD_LEN  (16)                                         /* AI HEAD长度 */
#define MAX_MAUTH_AI_DATA_LEN  (MAX_MAUTH_AI_SIZE - MAX_MAUTH_AI_HEAD_LEN)  /* AI DATA长度 */
//#define MAX_MAC_STR_LEN 		(6)
#define MAX_DID_LEN		        (64)
#define LICENSE_CODE_MAX_LEN 	(256)
#define CONNECT_STRING_MAX_LEN 	(256)
#define BOARD_TYPE_NAME_LEN     (32)
#define ACTIVE_CODE_MAX_LEN     (129)           // 128(激活码实际字符串数据)+1(结束符)
#define FUNCTION_MAX_LEN		(8)
#define CLOULD_DOMAIN_MAX_LEN   (128)			//云存域名长度

typedef enum
{
    MAUTH_AES_ENCRYPTION = 0,
    MAUTH_AES_DECRYPTION = 1,
} MAUTH_AES_TYPE_E;

typedef enum
{
    MAUTH_ENCRYPT_MD5, 
    MAUTH_ENCRYPT_SHA256, 
    MAUTH_ENCRYPT_BUTT
} MAUTH_ENCRYPT_TYPE_E;

typedef enum
{
    MAUTH_USR_DATA_TYPE_NONE = 0, 			/* 无效，USR未使用 */
    MAUTH_USR_DATA_TYPE_XRS,    			/* XRS USR Data */
    MAUTH_USR_DATA_TYPE_BUTT
} MAUTH_USR_DATA_TYPE_E;

/* MAuth algorithm */
typedef enum
{
    MAUTH_ALG_TYPE_INVALID   	= 0,			/* 不加密 */
    MAUTH_ALG_TYPE_AES_128BIT,            		/* Advanced encryption standard (AES) algorithm */
    MAUTH_ALG_TYPE_BUTT
} MAUTH_ALG_TYPE_E;

typedef struct
{
    UINT32 Crc32;   						/* CRC32校验值,包含结构体信息, 模块内部校验*/
    UINT32 Size;    						/* Data使用的数据大小， */
    UINT8 Type;     						/* 数据类型(MAUTH_USR_DATA_TYPE_E) */
    UINT8 Rsvd[7]; 							/* 保留位 */
    UINT8 Data[MAX_MAUTH_USR_DATA_LEN];		/* 加密部分 */
} MAUTH_USR_DATA_S, *PMAUTH_USR_DATA_S;
#define MAUTH_USR_DATA_S_LEN    sizeof(MAUTH_USR_DATA_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(MAUTH_USR_DATA_S, MAX_MAUTH_USRD_SIZE);

typedef struct
{
    UINT32 Crc32;   						/* CRC32校验值,包含结构体信息, 模块内部校验*/
    UINT32 Size;    						/* Data使用的数据大小， */
    UINT8 Type;     						/* 数据类型(MAUTH_USR_DATA_TYPE_E) */
    UINT8 Rsvd[7]; 							/* 保留位 */
    UINT8 Data[MAX_MAUTH_AI_DATA_LEN];		/* 加密部分 */
} MAUTH_AI_DATA_S, *PMAUTH_AI_DATA_S;
#define MAUTH_AI_DATA_S_LEN    sizeof(MAUTH_AI_DATA_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(MAUTH_AI_DATA_S, MAX_MAUTH_AI_SIZE);

typedef enum
{
    MAUTH_OEM_DATA_TYPE_NONE = 0, 			/* 无效，OEM未使用 */
    MAUTH_OEM_DATA_TYPE_BUTT
} MAUTH_OEM_DATA_TYPE_E;

#define MAX_MAUTH_OEM_DATA_LEN  (248) 		/* OEM_DATA长度 */
typedef struct
{
    unsigned int Crc32;   					/* CRC32校验值,包含结构体信息, 模块内部校验 */
    unsigned short Size;    				/* Data使用的数据大小 */
    unsigned char Type;						/* 数据类型(MAUTH_OEM_DATA_TYPE_E),对应Data不同的数据格式 */
    unsigned char Rsvd; 					/* 保留位 */	
    UINT8 Data[MAX_MAUTH_OEM_DATA_LEN];		/* 加密部分 */
} MAUTH_OEM_DATA_S, *PMAUTH_OEM_DATA_S;
#define MAUTH_OEM_DATA_S_LEN    sizeof(MAUTH_OEM_DATA_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(MAUTH_OEM_DATA_S, 8 + MAX_MAUTH_OEM_DATA_LEN);

typedef struct
{
	MAUTH_OEM_DATA_S OemData;
    unsigned char Rsvd[MAX_MAUTH_OEMD_SIZE - 256];
} MAUTH_OEMD_CONF_S, *PMAUTH_OEMD_CONF_S;
#define MAUTH_OEMD_CONF_S_LEN   sizeof(MAUTH_OEMD_CONF_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(MAUTH_OEMD_CONF_S, MAX_MAUTH_OEMD_SIZE);

typedef struct 
{
	unsigned int szType;
	unsigned int szLen;
	unsigned char szDid[MAX_DID_LEN];
} DID_INFO_S;

typedef struct
{
	unsigned int    Crc32;   								/* CRC32校验值, 计算时不包含该字段，模块内部校验 */
	unsigned char   AuthCode[LICENSE_CODE_MAX_LEN];		    /* 授权码 */
	unsigned short  CustomID;                 			    /* 客户编号 */
	unsigned char   Rsvd1[2]; 								/* 保留位 */
	unsigned int    BoardID;                    			/* 版型ID */
	unsigned int    Function;                   			/* 激活码 */
	unsigned char   SerialNum[MAX_SERIAL_NUM_LEN]; 		    /* 序列号 */
    DID_INFO_S      DIDInfo;                       		    /* DID信息 */
	unsigned char   MacAddr[MAX_MAC_STR_LEN];               /* MAC地址 */
	unsigned char   Rsvd2[2]; 								/* 保留位 */
	unsigned char   ConnectString[CONNECT_STRING_MAX_LEN];  /* 连接串 */
    char            BoardTypeName[BOARD_TYPE_NAME_LEN];     /* 设备版型名 */
    char            AuthCodeEx[LICENSE_CODE_MAX_LEN];       /* 扩展授权信息：运营商固化信息 */
    char            ActiveCode[ACTIVE_CODE_MAX_LEN];        /* 功能激活码 */
	unsigned char   Rsvd3[3]; 								/* 保留位 */
	unsigned int	FunctionV2[FUNCTION_MAX_LEN];			/* 激活码功能位V2.只保存外部激活 */
    char 			ClouldDomain[CLOULD_DOMAIN_MAX_LEN];	/* 云存域名 */
    unsigned char   P2PHdr;                                 /* 白名单域名访问 */
    unsigned char   Rsvd[315];                              /* 保留位 */
} MAUTH_DEV_ATTR_S, *PMAUTH_DEV_ATTR_S;
#define MAUTH_DEV_ATTR_S_LEN  sizeof(MAUTH_DEV_ATTR_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(MAUTH_DEV_ATTR_S, 1536);

/*************************************************************
 * 结构体介绍: 制造商文件头信息
 *************************************************************/
typedef struct
{
    unsigned int Crc32;   								/* CRC32校验值,除此字段外整个结构体校验信息, 模块内部校验 */
    unsigned int Producer;    							/* 制造商文件头标准,必须为MAUTH_FILE_HEAD_FLAG */
	unsigned int AlgType;								/* 加密类型，MAUTH_ALG_TYPE_E,若有加密则先加密后再计算DataCrc32值，
															控制devinfo所有需要加密数据的加密 */
    unsigned char Rsvd[468];      						/* 保留位 */
} MAUTH_INFO_S, *PMAUTH_INFO_S;
#define MAUTH_INFO_S_LEN sizeof(MAUTH_INFO_S)
ASSERT_SIZEOF_TYPEDEF_STRUCT(MAUTH_INFO_S, 512 - 32);

#endif