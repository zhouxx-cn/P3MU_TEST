/**
****************************************************************************************
* @FilePath: upgrade.h
* @Author: HL.LIU
* @Date: 2022-09-01 19:43:37
* @LastEditors:
* @LastEditTime: 2022-09-01 19:43:37
* @Copyright: 2022 RUISION CO.,LTD. All Rights Reserved.
* @Descripttion:
****************************************************************************************
*/
#ifndef _UPGRADE_H_
#define _UPGRADE_H_

#include "xrsstd.h"
#include "upgrade_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define UPGRADE_TRACE_ERR(str, args...) TRACE_ERR(COMP_UPGRADE, str, ##args)
#define UPGRADE_TRACE_WARN(str, args...) TRACE_WARN(COMP_UPGRADE, str, ##args)
#define UPGRADE_TRACE_FLOW(str, args...) TRACE_FLOW(COMP_UPGRADE, str, ##args)
#define UPGRADE_TRACE_INFO(str, args...) TRACE_INFO(COMP_UPGRADE, str, ##args)
#define UPGRADE_TRACE_DBG0(str, args...) TRACE_DBG0(COMP_UPGRADE, str, ##args)

#define MIN_UPGRADE_BUFF_SIZE   (8 << 20)

/*************************************************************
 * 枚举介绍：错误定义
 *************************************************************/
#define UPGRADE_ERRNO(err) ERRNO(err, COMP_UPGRADE)

typedef enum
{
    UPGRADE_ERROR_MOD_UNOPENED = COMP_ERROR_START,    /* 模块未开启 */
    UPGRADE_ERROR_BIN_OPEN,         /* 升级文件打开错误 */
    UPGRADE_ERROR_BIN_READ,         /* 升级文件读错误 */
    UPGRADE_ERROR_BIN_SEEK,         /* BIN设备SEEK错误 */
    UPGRADE_ERROR_HEADER_READ,      /* 文件头读取错误 */
    UPGRADE_ERROR_UNSUPPORTED_CHECK_TYPE,   /* 不支持的校验类型 */
    UPGRADE_ERROR_CHECK_CRC32,      /* CRC32校验错误 */
    UPGRADE_ERROR_KEY_OPEN,         /* KEY文件打开错误 */
    UPGRADE_ERROR_KEY_READ,         /* KEY文件读取错误 */
    UPGRADE_ERROR_MACHINE_KEY,      /* 主机版本不匹配 */
    UPGRADE_ERROR_TEMP_DIR_ACCESS,  /* 临时目录访问权限错误 */
    UPGRADE_ERROR_TEMP_FILE_CREATE, /* 升级临时文件创建错误 */
    UPGRADE_ERROR_TEMP_FILE_WRITE,  /* 临时文件写入错误 */
    UPGRADE_ERROR_TEMP_FILE_REMOVE, /* 临时文件删除错误 */
    UPGRADE_ERROR_UNKOWN_TYPE,      /* 未知升级文件类型 */
    UPGRADE_ERROR_UNSUPPORTED_TYPE, /* 不支持的升级文件类型 */
    UPGRADE_ERROR_DEV_VALID,        /* DEV设备非法 */
    UPGRADE_ERROR_DEV_OPEN,         /* DEV设备打开错误 */
    UPGRADE_ERROR_DEV_READ,         /* 读设备错误 */
    UPGRADE_ERROR_DEV_ENOENT,       /* 设备不存在 */
    UPGRADE_ERROR_DEV_WRITE,        /* DEV设备写入错误 */
    UPGRADE_ERROR_DEV_ERASE,        /* DEV设备擦除错误 */
    UPGRADE_ERROR_DEV_SEEK,         /* DEV设备SEEK错误 */
    UPGRADE_ERROR_DEV_REWIND,       /* 复位设备指针错误 */
    UPGRADE_ERROR_DEV_BADBLOCK,     /* FLASH坏块错误 */
    UPGRADE_ERROR_DEV_FSYNC,        /* DEV设备数据同步错误 */
    UPGRADE_ERROR_DEV_CLOSE,        /* DEV设备关闭错误 */
    UPGRADE_ERROR_FILE_VALID,       /* 升级文件非法 */
    UPGRADE_ERROR_FILE_TOOBIG,      /* 升级文件过大(0:1M,1:1M,2:4M,3:26M) */
    UPGRADE_ERROR_FILE_OPEN,        /* 文件打开错误 */
    UPGRADE_ERROR_FILE_REWIND,      /* 文件指针复位错误 */
    UPGRADE_ERROR_FILE_READ,        /* 文件读取错误 */
    UPGRADE_ERROR_FILE_WRITE,       /* 文件写入错误 */
    UPGRADE_ERROR_FILE_CLOSE,       /* 文件关闭错误 */
    UPGRADE_ERROR_VERIFY,           /* 升级文件校验错误 */
    UPGRADE_ERROR_TAR_OPEN,         /* 打开TAR文件错误 */
    UPGRADE_ERROR_TAR_LIST,         /* 显示TAR文件列表错误 */
    UPGRADE_ERROR_TAR_EXTRACT,      /* 解压TAR文件错误 */
    UPGRADE_ERROR_TAR_BLOCK_READ,   /* 读取TAR文件错误 */
    UPGRADE_ERROR_TAR_CLOSE,        /* 关闭TAR文件错误 */
    UPGRADE_ERROR_ZIP_OPEN,         /* 打开ZIP文件错误 */
    UPGRADE_ERROR_STAT,
    UPGRADE_ERROR_OPEN,             /* 打开文件错误 */
    UPGRADE_ERROR_SEEK,             /* 定位文件指针错误 */
    UPGRADE_ERROR_READ,             /* 读取文件数据错误 */
    UPGRADE_ERROR_WRITE,            /* 写入文件数据错误 */
    UPGRADE_ERROR_CLOSE,            /* 关闭文件错误 */
    UPGRADE_ERROR_FCNTL,
    UPGRADE_ERROR_PART_OFFSET,
    UPGRADE_ERROR_DEV_TYPE,         /* 主机设备类型不匹配 */
    UPGRADE_ERROR_OEM_TYPE,         /* 主机OEM类型不匹配 */
    UPGRADE_ERROR_HWID,             /* 硬件识别码扩展错误 */
    UPGRADE_ERROR_SWVEREXT,         /* 软件版本扩展不匹配 */
    UPGRADE_ERROR_MOUNT,
    UPGRADE_ERROR_UMOUNT,
    UPGRADE_ERROR_NOMEM,            /* 内存不足 */
    UPGRADE_ERROR_INTERNAL,         /* 内部错误 */
} UPGRADE_ERROR_E;

typedef enum
{
    TYPE_XRS = 0,   /* XRS */
    TYPE_NEUTRAL,   /* 中性 */
    TYPE_OEM,       /* OEM */
    TYPE_MAX
} OEM_TYPE_E;

// 最大支持的升级包存储内存个数
#define UPGRADE_MAX_MEM_NUM     16

// 升级包内存信息
typedef struct UpgradeMem
{
    void *Addr;
    UINT32 Size;
} UPGRADE_MEM_S, * PUPGRADE_MEM_S;

typedef struct UpgradeMemInfo
{
    UINT32 Cnt;
    UPGRADE_MEM_S Mem[UPGRADE_MAX_MEM_NUM];
} UPGRADE_MEM_INFO_S, * PUPGRADE_MEM_INFO_S;

typedef struct UpgradeImgPackInfo {
    PARTITION_TYPE_E partType;       /* 分区类型 */
    UPGRADE_TYPE_E upType;           /* 升级方式 */
    char imgPath[MAX_FILE_PATH_LEN]; /* 源文件路径 */
    char dstPath[MAX_FILE_PATH_LEN]; /* 目标路径，分区类型为USR时有效 */
} UPGRADE_IMG_PACK_INFO_S;

typedef struct UpgradeRevertPackInfo {
    PACKAGE_TYPE_E packageType;                                 /* 升级包类型 */
    char pkgVer[PACKAGE_VER_LEN];                               /* 升级包版本 */
    char pkgBaseVer[PACKAGE_VER_LEN];                           /* 升级包基础版本 */
    UINT32 hwCodeMsk;                                           /* 硬件id掩码 */
    ENCRYPT_TYPE_E encryptType;                                 /* 加密类型 */
    CHECK_TYPE_E checkType;                                     /* 检验类型 */
    BOARD_TYPE_SUPPORT_INFO_S boardTypeList;                    /* 支持版型列表 */
    UINT32 imgCnt;                                              /* 镜像数量 */
    UPGRADE_IMG_PACK_INFO_S imgList[PACKAGE_MAX_PARTITION_NUM]; /* 镜像列表 */
} UPGRADE_PACK_INFO_S;

/*************************************************************
 * 函数介绍: 打开升级模块
 * 输入参数: NotifyFunc - 消息回调函数
 *           DevType: 机型通配符
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *************************************************************/
SINT32 Upgrade_Open(Notify_Callback NotifyFunc, char DevType[BOARD_TYPE_NAME_LEN]);
#if 0
/******************************************************************************
 * 函数介绍: 获取DEV设备属性信息
 * 输入参数: pDevsAttr: DEV设备属性结构体指针
 * 输出参数: pDevsAttr: DEV设备属性结构体指针,DEV设备属性
 * 返回值  : 0-成功,<0-失败
 *****************************************************************************/
SINT32 Upgrade_Get_DevAttr(SINT32 handle, PMTD_DEVS_ATTR_S pDevsAttr);

/******************************************************************************
 * 函数功能: 升级文件校验扩展接口
 * 输入参数: handle: 模块句柄,此处无效
 *           pFileInfo: 升级文件结构信息
 *           pFilePath: 升级文件绝对路径(如果为空,表示以内存方式升级)
 *           pTmpFileDir: 升级文件过程中临时文件存放目录(为空时,默认存放在/tmp)
 * 输出参数: pUpgradeKeyInfo: 返回升级包升级限制信息，由应用层判断是否可以升级
 * 返回值  : >=0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Upgrade_File_CheckExt(SINT32 handle, void *pUpgradeAddr, PUPGRADE_KEY_S pUpgradeKeyInfo, char *pFilePath, char *pTmpFileDir);

/******************************************************************************
 * 函数功能: 升级文件升级扩展接口
 * 输入参数: handle: 模块句柄,此处无效
 *           pFilePath: 升级文件绝对路径(如果为空,表示以内存方式升级)
 *           pUpgradeAddr: 升级文件内存地址
 *           UpgradeSize: 升级文件内存大小
 *           pTmpFileDir: 升级文件过程中临时文件存放目录(为空时,默认存放在/tmp)
 * 输出参数: 无
 * 返回值  : >=0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Upgrade_File_UpdateExt(SINT32 handle, char *pFilePath, char *pUpgradeAddr, SINT32 UpgradeSize, char *pTmpFileDir);
#endif

/******************************************************************************
 * 函数功能: 内存升级文件校验(只校验升级包是否损坏。版型+机型是否匹配，版本限制由上层判断)
 * 输入参数: handle: 模块句柄,此处无效
 *           pUpgradeAddr: 升级文件内存地址
 * 输出参数: pUpgradeKeyInfo: 返回升级包升级限制信息，由应用层判断是否可以升级
 * 返回值  : >=0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Upgrade_Mem_Check(SINT32 handle, void* pUpgradeAddr, PUPGRADE_KEY_S pUpgradeKeyInfo);

/******************************************************************************
 * 函数功能: 升级文件校验(只校验升级包是否损坏。版型+机型是否匹配，版本限制由上层判断)
 * 输入参数: handle: 模块句柄,此处无效
 *           pMemInfo: 升级文件内存存储信息
 * 输出参数: pUpgradeKeyInfo: 返回升级包升级限制信息，由应用层判断是否可以升级
 * 返回值  : >=0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Upgrade_Mem_CheckEx(SINT32 handle, PUPGRADE_MEM_INFO_S pMemInfo, PUPGRADE_KEY_S pUpgradeKeyInfo);

/******************************************************************************
 * 函数功能: 内存升级文件升级
 * 输入参数: handle: 模块句柄,此处无效
 *           pUpgradeAddr: 升级文件内存地址
 *           UpgradeSize: 升级文件内存大小
 *           pTmpFileDir: 升级文件过程中临时文件存放目录(为空时,默认存放在/tmp)
 * 输出参数: 无
 * 返回值  : >=0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Upgrade_Mem_Update(SINT32 handle, char* pUpgradeAddr, SINT32 UpgradeSize, char* pTmpFileDir);

/******************************************************************************
 * 函数功能: 扩展内存升级（多块内存地址升级）
 * 输入参数: handle: 模块句柄,此处无效
 *           MemInfo: 升级文件内存地址信息
 *           pTmpFileDir: 升级文件过程中临时文件存放目录(为空时,默认存放在/tmp)
 * 输出参数: 无
 * 返回值  : >=0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Upgrade_Mem_UpdateEx(SINT32 handle, PUPGRADE_MEM_INFO_S MemInfo, char* pTmpFileDir);

/*************************************************************
 * 函数介绍: 升级包打包接口，仅支持upgrade,restore,resume
 * 输入参数: pPackInfo： 升级打包相关信息
 *           imgPath: 升级包文件列表
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *************************************************************/
SINT32 Upgrade_ImgPack(UPGRADE_PACK_INFO_S* pPackInfo, char* imgPath);

/******************************************************************************
 * 函数介绍: 关闭升级模块,释放所有资源
 * 输入参数: handle:升级句柄(此处无效)
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Upgrade_Close(SINT32 handle);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _UPGRADE_H_ */
