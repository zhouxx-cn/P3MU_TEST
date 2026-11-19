/**
****************************************************************************************
 * @FilePath: partition_tags.h
 * @Author: HL.LIU
 * @Date: 2022-08-31 17:55:33
 * @LastEditors:
 * @LastEditTime: 2022-08-31 20:04:46
 * @Copyright: 2022 RUISION CO.,LTD. All Rights Reserved.
 * @Descripttion:
****************************************************************************************
*/

/******************************************************************************
 * 升级文件存储结构示意图，头共8K
 * +++++++++++++++++++++++++++++++++<--------------------+<-------------------+
 * +                               +                     +                    +
 * +    PARTITION_TAGS_S 信息      +   4096 Bytes        +                    +
 * +                               +                     +                    +
 * +-------------------------------+<--------------------+<-------------------+
 * +-                             -+                     +                    +
 * +-   UIMAGE信息                -+        Bytes        +                    +
 * +-                             -+                     +                    +
 * +-------------------------------+<--------------------+<-------------------+
 * +-                             -+                     +                    +
 * +-   DTB信息                   -+         Bytes       +                    +
 * +-                             -+                     +                    +
 * +-------------------------------+<--------------------+<-------------------+
 * +-                             -+                     +                    +
 * +-   ROOTFS信息                -+         Bytes       +                    +
 * +-                             -+                     +                    +
 * +-------------------------------+<--------------------+<-------------------+
 * +-                             -+                     +                    +
 * +-   ...............           -+                     +                    +
 * +-                             -+                     +                    +
 * +-------------------------------+<--------------------+<-------------------+
 ******************************************************************************/

#ifndef __PARTITION_TAGS_H__
#define __PARTITION_TAGS_H__

#include "xrsdef.h"

#define PARTITION_TAGS_MAGIC    "XRS"
#define PARTITION_TAGS_VER      "1.0"

/* image 类型 */
typedef enum XrsImageType
{
    XRS_IMAGE_TYPE_ROOTFS = 0,             /* rootfs must set first */
    XRS_IMAGE_TYPE_UIMAGE,                 /* uImage */
    XRS_IMAGE_TYPE_DTB,                    /* dtb */
    XRS_IMAGE_TYPE_ATF,                    /* ATF */
    XRS_IMAGE_TYPE_BUTT
} XRS_IMAGE_TYPE_E;
#define XRS_IMAGE_TYPE_MAX     8               /* image最大数量 */

/* 加密类型 */
typedef enum XrsCheckType
{
    CHECK_TYPE_CRC32 = 0,
    CHECK_TYPE_MD5,
    CHECK_TYPE_BUTT
} XRS_CHECK_TYPE_E;

typedef struct XrsImageInfo
{
    unsigned int ImageType;         /* image 的类型 XRS_IMAGE_TYPE_E,如果是XRS_IMAGE_TYPE_ROOTFS则要对已有的数据按块对齐后放rootfs镜像 */
    unsigned int EncryptType;       /* 加密类型,0:无加密 */
    unsigned int ImageOffset;
    unsigned int ImageSize;
    unsigned char Resv[368];
} XRS_IMAGE_INFO_S, * PXRS_IMAGE_INFO_S;

typedef struct XrsPartitionTags
{
    unsigned char Magic[8];                     /* PARTITION_TAGS_MAGIC */
    unsigned int CheckType;                     /* 校验方式 */
    unsigned char CheckValue[32];               /* header + payload 的校验值（MD5或HAS256或CRC32） */
    unsigned int PayloadSize;                  	/* payload 大小 */
    unsigned char Version[4];                   /* 版本号 */
    unsigned char Resv1[12];
    unsigned int ImageNum;                    	/* image 数量 */
    XRS_IMAGE_INFO_S Images[XRS_IMAGE_TYPE_MAX];   	/* image 信息列表 */
    unsigned char Resv2[956];
} XRS_PARTITION_TAGS_S, * PXRS_PARTITION_TAGS_S;
ASSERT_SIZEOF_TYPEDEF_STRUCT(XRS_PARTITION_TAGS_S, 4096);

#endif