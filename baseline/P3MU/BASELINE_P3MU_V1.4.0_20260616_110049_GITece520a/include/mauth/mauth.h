/**
****************************************************************************************
 * @FilePath: mauth.h
 * @Author: HL.LIU
 * @Date: 2022-09-01 21:15:31
 * @LastEditors: 
 * @LastEditTime: 2022-09-01 21:15:31
 * @Copyright: 2022 RUISION CO.,LTD. All Rights Reserved.
 * @Descripttion: 
****************************************************************************************
*/
#ifndef _MAUTH_H_
#define _MAUTH_H_

#include "mauth_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef __KERNEL__
#define COMP_ERROR_START    32
#endif /* __KERNEL__ */

#define MAUTH_TRACE_ERR(str, args...) TRACE_ERR(COMP_MAUTH, str, ##args)
#define MAUTH_TRACE_WARN(str, args...) TRACE_WARN(COMP_MAUTH, str, ##args)
#define MAUTH_TRACE_FLOW(str, args...) TRACE_FLOW(COMP_MAUTH, str, ##args)
#define MAUTH_TRACE_INFO(str, args...) TRACE_INFO(COMP_MAUTH, str, ##args)
#define MAUTH_TRACE_DBG0(str, args...) TRACE_DBG0(COMP_MAUTH, str, ##args)

#define MAUTH_INVALID_VALUE (-1)

#define MAX_MAUTH_OPT_ID_LEN    32
#define MAX_MAUTH_FILE_PATH_LEN 64

#define MAUTH_MMC_RPMB_BLK_ADDR 0
#define MAUTH_MMC_RPMB_BLK_CNT  (MAX_MAUTH_FILE_SIZE / 256)

#define MAX_MAUTH_DATA_HEX_DUMP 1024
#define MAUTH_DATA_HEX_DUMP(T, D, O, S)   do {          \
    SINT32 _HexDumpCnt = 0, _HexDumpSz = S;             \
    MAUTH_TRACE_DBG(T, S);                              \
    if (_HexDumpSz > MAX_MAUTH_DATA_HEX_DUMP) {         \
        _HexDumpSz = MAX_MAUTH_DATA_HEX_DUMP;           \
        MAUTH_TRACE_DBG("Warning: Dump Data(%d) Too "   \
            "Long, Only Dump(%d)\n", S, _HexDumpSz);    \
    }                                                   \
    for (_HexDumpCnt = 0; _HexDumpCnt < _HexDumpSz;     \
        _HexDumpCnt++) {                                \
        if (_HexDumpCnt && (0 == (_HexDumpCnt % 16))) { \
            MAUTH_TRACE_PRI("\n");                      \
        }                                               \
        MAUTH_TRACE_PRI("%02X ", D[(O) + _HexDumpCnt]); \
    }                                                   \
    MAUTH_TRACE_PRI("\n"); } while (0)
#ifdef MAUTH_DATA_HEX_DUMP_ENABLE
#define MAUTH_DATA_HEX_DDBG(T, D, O, S) MAUTH_DATA_HEX_DUMP(T, D, O, S)
#else
#define MAUTH_DATA_HEX_DDBG(T, D, O, S)
#endif /* MAUTH_DATA_HEX_DUMP_ENABLE */

#ifdef MAUTH_MTD_NAND_ENABLE
#define MAUTH_DEV_CRC_EN        1   /* enbale data CRC check */
#endif /* MAUTH_MTD_NAND_ENABLE */

#define MAUTH_FILE_SIZE_ALIGN   MAX_MAUTH_FILE_SIZE
#define MAUTH_DATA_DEF_OFFSET   (MAX_ENCRYPT_DATA_LEN + MAUTH_FILE_INFO_S_LEN)
#ifdef MAUTH_DEV_CRC_EN
#define MAUTH_DATA_CRC32_SIZE   (MAUTH_FILE_SIZE_ALIGN - MAUTH_DATA_DEF_OFFSET - 4)
#else
#define MAUTH_DATA_CRC32_SIZE   (MAUTH_FILE_SIZE_ALIGN - MAUTH_DATA_DEF_OFFSET)
#endif /* MAUTH_DEV_CRC_EN */

#define MIN_MAUTH_AES_KEY_BUFF_LEN  260

#define MAUTH_PLATFORM_STRS_MAX_LEN     16
#define MAUTH_PLATFORM_STRS_X86         "X86"
#define MAUTH_PLATFORM_STRS_AX620U      "AX620U"
#define MAUTH_PLATFORM_STRS_RV1109      "RV1109"
#define MAUTH_PLATFORM_STRS_HI3516DV300 "HI3516DV300"

typedef enum
{
    MAUTH_PLATFORM_TYPE_X86, 
    MAUTH_PLATFORM_TYPE_AX620U, 
    MAUTH_PLATFORM_TYPE_RV1109, 
    MAUTH_PLATFORM_TYPE_HI3516DV300, 
    MAUTH_PLATFORM_TYPE_BUTT
} MAUTH_PLATFORM_TYPE_E;
#define MAUTH_PLATFORM_TYPE_MAX MAUTH_PLATFORM_TYPE_BUTT

#define MAUTH_ERRNO(err) ERRNO(err, COMP_MAUTH)

typedef enum
{
    MAUTH_ERROR_OPEN = COMP_ERROR_START,    /* 设备打开失败 */
    MAUTH_ERROR_NOT_OPEN,                   /* 模块未打开 */
    MAUTH_ERROR_PARAM,                      /* 参数错误 */
    MAUTH_ERROR_READ_BLOCK_DEVICE,          /* 读取块设备失败 */
    MAUTH_ERROR_WRITE_BLOCK_DEVICE,         /* 写入块设备失败 */
    MAUTH_ERROR_CHECK,                      /* 数据校验失败 */
    MAUTH_ERROR_INTERNAL,                   /* 内部错误 */
    MAUTH_ERROR_BUTT
} MAUTH_ERROR_E;

#ifndef __KERNEL__
/*************************************************************
 * 函数介绍: 随机数播种
 * 输入参数: Seed - 随机数种子(如果为0,将取当前系统时间秒数做种子)
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *************************************************************/
SINT32 MAuth_SRand(UINT32 Seed);

/*************************************************************
 * 函数介绍: 生成随机数(RangeMin, RangeMax]
 * 输入参数: RangeMin - 随机数范围最小值
 *           RangeMax - 随机数范围最大值
 * 输出参数: 无
 * 返回值  : >=0-成功,生成随机数,<0-错误代码
 *************************************************************/
SINT32 MAuth_Rand(SINT32 RangeMin, SINT32 RangeMax);
#endif /* __KERNEL__ */

/*************************************************************
 * 函数介绍: 根据数据生成CRC32校验
 * 输入参数: InitCrc32 - CRC32初始化值,通常为0
 *           pBuf - 要生成生成CRC32的数据长度
 *           BufLen - 要生成CRC32的数据缓冲区指针
 * 输出参数: 无
 * 返回值  : CRC32校验值
 *************************************************************/
UINT32 MAuth_Crc32(UINT32 InitCrc32, const void *pBuf, SINT32 BufLen);

/*************************************************************
 * 函数介绍: 根据数据生成SHA256哈希值
 * 输入参数: pData - 要生成生成SHA256哈希值的数据缓冲区指针
 *           DataSize - 要生成生成SHA256哈希值的数据长度
 *           Hash - 要生成SHA256哈希值的数据缓冲区指针
 * 输出参数: Hash - 要生成SHA256哈希值的数据
 * 返回值  : 0-成功,<0-错误代码
 *************************************************************/
SINT32 MAuth_Sha256(const char *pData, SINT32 DataSize, UINT8 Hash[32]);

/*************************************************************
 * 函数介绍: AES加密(对称加密)
 * 输入参数: pDataEnc - 需要加密数据缓冲区指针
 *           pDataOut - 存放加密数据缓冲区指针
 *           DataSize - 需要加密数据长度
 *           AesRand  - AES加密随机数
 * 输出参数: pDataOut - 加密数据
 * 返回值  : 0-成功,<0-错误代码
 *************************************************************/
SINT32 MAuth_Aes_Encrypt(const UINT8 *pDataEnc, UINT8 *pDataOut, SINT32 DataSize, UINT8 AesRand);

/*************************************************************
 * 函数介绍: AES解密(对称解密)
 * 输入参数: pDataDec - 需要解密数据缓冲区指针
 *           pDataOut - 存放解密数据缓冲区指针
 *           DataSize - 需要解密数据长度
 *           AesRand  - AES解密随机数
 * 输出参数: pDataOut - 解密数据
 * 返回值  : 0-成功,<0-错误代码
 *************************************************************/
SINT32 MAuth_Aes_Decrypt(const UINT8 *pDataDec, UINT8 *pDataOut, SINT32 DataSize, UINT8 AesRand);

#if 0/* 只实现最基本内容，其他后面按需实现 */
/*************************************************************
 * 函数介绍: 是否使能加密REG进行AES加密(对称加密)
 * 输入参数: pDataEnc - 需要加密数据缓冲区指针
 *           pDataOut - 存放加密数据缓冲区指针
 *           DataSize - 需要加密数据长度
 *           AesRand  - AES加密随机数
 *           enEncReg - 是否使能加密REG
 *           pEncReg  - 加密REG缓冲区指针
 *           EncRegLen- 加密REG缓冲区长度
 * 输出参数: pDataOut - 加密数据
 *           pEncReg  - 加密REG数据
 * 返回值  : 0-成功,<0-错误代码
 *************************************************************/
SINT32 MAuth_Aes_EncryptReg(const UINT8 *pDataEnc, UINT8 *pDataOut, SINT32 DataSize, 
    UINT8 AesRand, MV_BOOL enEncReg, UINT8 *pEncReg, SINT32 EncRegLen);

/*************************************************************
 * 函数介绍: 是否使能解密REG进行AES解密(对称解密)
 * 输入参数: pDataDec - 需要解密数据缓冲区指针
 *           pDataOut - 存放解密数据缓冲区指针
 *           DataSize - 需要解密数据长度
 *           AesRand  - AES解密随机数
 *           enDecReg - 是否使能解密REG
 *           pDecReg  - 解密REG缓冲区指针
 *           DecRegLen- 解密REG缓冲区长度
 * 输出参数: pDataOut - 解密数据
 *           pDecReg  - 解密REG数据
 * 返回值  : 0-成功,<0-错误代码
 *************************************************************/
SINT32 MAuth_Aes_DecryptReg(const UINT8 *pDataDec, UINT8 *pDataOut, SINT32 DataSize, 
    UINT8 AesRand, MV_BOOL enDecReg, UINT8 *pDecReg, SINT32 DecRegLen);

#if !(defined(__KERNEL__) || defined(PLATFORM_X86))
/*************************************************************
 * 函数介绍: 加密密钥创建
 * 输入参数: KeyType - 密钥类型
 *           Time - 加密密钥时间(0-函数内部获取系统时间)
 *           Random  - 加密密钥随机数(0-函数内部获取系统随机数)
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *************************************************************/
SINT32 MAuth_KeyCreate(MAUTH_KEY_TYPE_E KeyType, UINT32 Time, UINT32 Random);

/*************************************************************
 * 函数介绍: 加密密钥更新
 * 输入参数: KeyType - 密钥类型
 *           Time - 加密密钥时间(0-函数内部获取系统时间)
 *           Random  - 加密密钥随机数(0-函数内部获取系统随机数)
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *************************************************************/
SINT32 MAuth_KeyUpdate(MAUTH_KEY_TYPE_E KeyType, UINT32 Time, UINT32 Random);

/*************************************************************
 * 函数介绍: 加密密钥销毁
 * 输入参数: KeyType - 密钥类型
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *************************************************************/
SINT32 MAuth_KeyDestroy(MAUTH_KEY_TYPE_E KeyType);

/*************************************************************
 * 函数介绍: 加密密钥校验
 * 输入参数: KeyType - 密钥类型
 * 输出参数: 无
 * 返回值  : 0-不存在,1-存在,<0-错误代码
 *************************************************************/
SINT32 MAuth_KeyCheck(MAUTH_KEY_TYPE_E KeyType);

/*************************************************************
 * 函数介绍: 加密(对称加密)
 * 输入参数: KeyType - 密钥类型
 *           pDataEnc - 需要加密数据缓冲区指针
 *           pDataOut - 存放加密数据缓冲区指针
 *           DataSize - 需要加密数据长度
 * 输出参数: pDataOut - 加密数据
 * 返回值  : 0-成功,<0-错误代码
 *************************************************************/
SINT32 MAuth_KeyEncrypt(MAUTH_KEY_TYPE_E KeyType, const UINT8 *pDataEnc, UINT8 *pDataOut, SINT32 DataSize);

/*************************************************************
 * 函数介绍: 解密(对称解密)
 * 输入参数: KeyType - 密钥类型
 *           pDataDec - 需要解密数据缓冲区指针
 *           pDataOut - 存放解密数据缓冲区指针
 *           DataSize - 需要解密数据长度
 * 输出参数: pDataOut - 解密数据
 * 返回值  : 0-成功,<0-错误代码
 *************************************************************/
SINT32 MAuth_KeyDecrypt(MAUTH_KEY_TYPE_E KeyType, const UINT8 *pDataDec, UINT8 *pDataOut, SINT32 DataSize);

/*************************************************************
 * 函数介绍: AES加密创建密钥
 * 输入参数: pAesKeyBuff - AES加密密钥缓冲区指针
 *           AesKeyBuffLen - AES加密密钥缓冲区长度
 *           AesType - AES加密密钥类型
 *           pPasswd - AES加密密钥密码
 *           Time - AES加密密钥时间
 *           Random - AES加密密钥随机数
 * 输出参数: pAesKey - AES加密密钥
 * 返回值  : >0-成功,AES加密密钥长度,<0-错误代码
 *************************************************************/
SINT32 MAuth_Aes_CreateKey(UINT8 *pAesKeyBuff, SINT32 AesKeyBuffLen, 
    MAUTH_AES_TYPE_E AesType, const char *pPasswd, UINT32 Time, UINT32 Random);

/*************************************************************
 * 函数介绍: AES加密(对称加密)
 * 输入参数: pAesKeyBuff - AES加密密钥缓冲区指针
 *           AesKeyBuffLen - AES加密密钥缓冲区长度
 *           pDataEnc - 需要加密数据缓冲区指针
 *           pDataOut - 存放加密数据缓冲区指针
 *           DataSize - 需要加密数据长度
 * 输出参数: pDataOut - 加密数据
 * 返回值  : 0-成功,<0-错误代码
 *************************************************************/
SINT32 MAuth_Aes_EncryptExt(UINT8 *pAesKeyBuff, SINT32 AesKeyBuffLen, 
    const UINT8 *pDataEnc, UINT8 *pDataOut, SINT32 DataSize);

/*************************************************************
 * 函数介绍: AES解密(对称解密)
 * 输入参数: pAesKeyBuff - AES加密密钥缓冲区指针
 *           AesKeyBuffLen - AES加密密钥缓冲区长度
 *           pDataDec - 需要解密数据缓冲区指针
 *           pDataOut - 存放解密数据缓冲区指针
 *           DataSize - 需要解密数据长度
 * 输出参数: pDataOut - 解密数据
 * 返回值  : 0-成功,<0-错误代码
 *************************************************************/
SINT32 MAuth_Aes_DecryptExt(UINT8 *pAesKeyBuff, SINT32 AesKeyBuffLen, 
    const UINT8 *pDataDec, UINT8 *pDataOut, SINT32 DataSize);
#endif /* Not __KERNEL__ And PLATFORM_X86 */

/*************************************************************
 * 函数介绍: AES配置(硬)
 * 输入参数: KeyBits - AES加密密钥位宽
 *           WorkMode - AES加密密钥模式
 *           AesType - AES加解密类型
 *           Handle - AES加密密钥句柄
 * 输出参数: 无
 * 返回值  : 0-成功,,<0-错误代码
 *************************************************************/
SINT32 MAuth_Aes_HardConf(UINT32 KeyBits, MAUTH_WORK_MODE_E WorkMode, MAUTH_AES_TYPE_E AesType, UINT32 Handle);

#ifdef CONFIG_MVCA_MAUTH
/*************************************************************
 * 函数介绍: AES解密(硬)
 * 输入参数: KeyBits - AES解密密钥位宽
 *           WorkMode - AES解密密钥模式
 *           pIn - AES解密数据缓冲区指针
 *           pOut - AES解密数据输出缓冲区
 *           Len - AES解密数据缓冲区长度
 * 输出参数: pOut - AES解密数据
 * 返回值  : 0-成功,<0-错误代码
 *************************************************************/
SINT32 MAuth_Aes_HardDecrypt(UINT32 KeyBits, MAUTH_WORK_MODE_E WorkMode, const UINT8 *pIn, UINT8 *pOut, size_t Len);
#endif /* CONFIG_MVCA_MAUTH */

/*************************************************************
 * 函数介绍: AES加密(软)
 * 输入参数: KeyBits - AES加密密钥位宽
 *           WorkMode - AES加密密钥模式
 *           pIn - AES加密数据缓冲区指针
 *           pOut - AES加密数据输出缓冲区
 *           Len - AES加密数据缓冲区长度
 *           pPlatform - 平台名(例如: MAUTH_PLATFORM_STRS_AX620U)
 * 输出参数: pOut - AES加密数据
 * 返回值  : 0-成功,<0-错误代码
 *************************************************************/
SINT32 MAuth_Aes_SoftEncrypt(UINT32 KeyBits, MAUTH_WORK_MODE_E WorkMode, 
    const UINT8 *pIn, UINT8 *pOut, size_t Len, const char *pPlatform);

/*************************************************************
 * 函数介绍: AES解密(软)
 * 输入参数: KeyBits - AES解密密钥位宽
 *           WorkMode - AES解密密钥模式
 *           pIn - AES解密数据缓冲区指针
 *           pOut - AES解密数据输出缓冲区
 *           Len - AES解密数据缓冲区长度
 *           pPlatform - 平台名(例如: MAUTH_PLATFORM_STRS_AX620U)
 * 输出参数: pOut - AES解密数据
 * 返回值  : 0-成功,<0-错误代码
 *************************************************************/
SINT32 MAuth_Aes_SoftDecrypt(UINT32 KeyBits, MAUTH_WORK_MODE_E WorkMode, 
    const UINT8 *pIn, UINT8 *pOut, size_t Len, const char *pPlatform);
#endif

#if !(defined(__KERNEL__) || defined(PLATFORM_X86))
SINT32 MAuth_Dev_Open(void);
SINT32 MAuth_Dev_Close(SINT32 handle);
SINT32 MAuth_Dev_SetDevAttr(SINT32 handle, const PMAUTH_DEV_ATTR_S pDevAttr);
SINT32 MAuth_Dev_GetDevAttr(SINT32 handle, PMAUTH_DEV_ATTR_S pDevAttr);
SINT32 MAuth_Dev_SetOemDat(SINT32 handle, const PMAUTH_OEM_DATA_S pOemDat);
SINT32 MAuth_Dev_GetOemDat(SINT32 handle, PMAUTH_OEM_DATA_S pOemDat);
SINT32 MAuth_Dev_GetUsrDat(SINT32 handle, PMAUTH_USR_DATA_S pUsrDat);
SINT32 MAuth_Dev_SetUsrDat(SINT32 handle, const PMAUTH_USR_DATA_S pUsrDat);
SINT32 MAuth_Dev_GetAiDat(SINT32 handle, PMAUTH_AI_DATA_S pAiDat);
SINT32 MAuth_Dev_SetAiDat(SINT32 handle, const PMAUTH_AI_DATA_S pAiDat);
SINT32 MAuth_Dev_SetWriteMode(SINT32 handle, SINT32 EnWrite);
SINT32 MAuth_Dev_GetWriteMode(SINT32 handle, SINT32 *pEnWrite);
SINT32 MAuth_Dev_Update(SINT32 handle);
#endif /* Not __KERNEL__ And PLATFORM_X86 */

#ifdef MAUTH_SOFT_ENC_DEC_ENABLE
#if (defined(PLATFORM_X86))
SINT32 Mauth_RsaDatPack(const char *pPlatform, const UINT8 *pDatBuf, 
    SINT32 DatBufLen, UINT8 *pDatOut, SINT32 DatOutLen, SINT32 *pDatOutLen, 
    UINT8 *pRsaPubBuf, SINT32 RsaPubBufLen, UINT8 *pHash, SINT32 HashLen);
SINT32 Mauth_RsaDatUnPack(const char *pPlatform, const UINT8 *pDatBuf, 
    SINT32 DatBufLen, SINT32 DatBinLen, UINT8 *pDatOut, SINT32 DatOutLen);
#endif /* PLATFORM_X86 */
#endif /* MAUTH_SOFT_ENC_DEC_ENABLE */

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _MAUTH_H_ */
