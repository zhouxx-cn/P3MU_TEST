/***********************************************************************************
 * Copyright (C) 2018 All rights reserved.
 * 文件名称：Authority.h
 * 创 建 者：樊聪
 * 邮    箱：fancong20@163.com
 * 创建日期：2018年08月16日 星期四
 * 描    述：授权解密库操作接口
 *  
 * 更新日志：
 * 2018-08-22 初版完成 
 * 2018-11-12 添加错误码AUTH_AUTH_AUTHORIZED_ERR 只允许对未授权的设备进行授权
 * 2018-11-16 添加错误码AUTH_CANCLE_UNAUTHORIZED_ERR 只允许对授权的设备进行取消授权
 *  
 ************************************************************************************/

#ifndef __AUTHORITY_H__
#define __AUTHORITY_H__
#define LICENSE_CODE_MAX_LEN (256)
#define MAX_CONNECT_STRING_LEN (256)
#define MAX_SERIAL_NUM_LEN	   (32)
#define MAC_ADDR_LEN		   (6)
#define MAX_DID_NUM_LEN		   (63)
#define MAX_BOARD_TYPE_NAME_LEN     (32)

#define MAX_PUSH_ID_LENGTH     (8)       //推送ID总长度
#define MAX_PUSH_ID_SFD        ('#')     //推送ID起始定界符

/* 授权码授权的错误码 */
#define AUTH_MODULE_ID                  (80000)

/* 错误类型(分为未授权，授权成功，授权失败) */
#define AUTH_NOT_AUTHORIZE              (AUTH_MODULE_ID)         // 未授权
#define AUTH_SUCCESS                    (AUTH_MODULE_ID+100)     // 授权成功
#define AUTH_FAIL                       (AUTH_MODULE_ID+200)     // 授权失败
#define REMOVE_AUTH_SUCCESS             (AUTH_MODULE_ID+300)     // 取消授权成功

/* 授权失败的错误状态(七大类) */
#define AUTH_LIC_CODE_ERR               (AUTH_FAIL+10)            // 授权码错误
#define AUTH_CUSTOM_CODE_ERR            (AUTH_FAIL+20)            // 授权用户码错误
#define AUTH_BOARD_ID_ERR               (AUTH_FAIL+30)            // 授权版型错误
#define AUTH_MAC_ERR                    (AUTH_FAIL+40)            // 授权MAC地址错误
#define AUTH_DID_ERR                    (AUTH_FAIL+50)            // 授权DID错误
#define AUTH_SERIAL_NUM_ERR             (AUTH_FAIL+60)            // 授权序列号错误
#define AUTH_RE_OPERATE_ERR             (AUTH_FAIL+70)            // 重复操作错误

/* 每一类分成若干小类 */
#define AUTH_LIC_CODE_INVALID           (AUTH_LIC_CODE_ERR+1)     // 授权码非法
#define AUTH_LIC_CODE_LEN_INVALID       (AUTH_LIC_CODE_ERR+2)     // 授权码长度非法
#define AUTH_LIC_CODE_ARG_ERR           (AUTH_LIC_CODE_ERR+3)     // 授权参数错误
#define AUTH_LIC_CODE_ALLOC_MEM_ERR     (AUTH_LIC_CODE_ERR+4)     // 授权内存分配失败

#define AUTH_CUSTOM_CODE_INVALID        (AUTH_CUSTOM_CODE_ERR+1)  // 授权用户码非法

#define AUTH_BOARD_ID_NOT_MATCH         (AUTH_BOARD_ID_ERR+1)     // 授权版型不匹配
#define AUTH_BOARD_ID_INVALID           (AUTH_BOARD_ID_ERR+2)     // 授权版型非法

#define AUTH_MAC_INVALID                (AUTH_MAC_ERR+1)          // 授权MAC地址非法

#define AUTH_DID_INVALID                (AUTH_DID_ERR+1)          // 授权DID号非法
#define AUTH_DID_UNSUPPORT              (AUTH_DID_ERR+2)          // 授权DID号不支持

#define AUTH_SERIAL_NUM_INVALID         (AUTH_SERIAL_NUM_ERR+1)   // 授权序列号非法

#define AUTH_AUTH_AUTHORIZED_ERR        (AUTH_RE_OPERATE_ERR+1)   // 只允许对未授权的设备进行授权

#define AUTH_CANCLE_UNAUTHORIZED_ERR    (AUTH_RE_OPERATE_ERR+2)   // 只允许对授权的设备进行取消授权

typedef struct tagAlpha
{
	unsigned char szType;
	unsigned char szLen;
	unsigned char szDid[MAX_DID_NUM_LEN];
}StuCloudInfo;

/* 共享内存的授权信息 */
typedef struct tagShmAuthInfo{
	unsigned long  dwAuthVerifyErrno;                       /* 授权状态错误码 */
	unsigned short dCustomID;                               /* 客户编号         */
	unsigned long  dwBoardID;                               /* 版型ID         */
	unsigned long  dwAtime;                                 /* 时间戳          */
	unsigned long  dwFuncion;                               /* 激活码          */
	unsigned char  szAuthCode[LICENSE_CODE_MAX_LEN];        /* 授权码          */
	unsigned char  szMac[MAC_ADDR_LEN];                     /* MAC地址        */
	unsigned char  szSerialNum[MAX_SERIAL_NUM_LEN];         /* 序列号          */
    unsigned char  szConnectString[MAX_CONNECT_STRING_LEN]; /* 连接串          */
    StuCloudInfo   struCloudInfo;                           /* DID信息        */
}stuShmAuthInfo, *pShmAuthInfo;

typedef struct tagConnectInfo
{
	char szPlatformName[16];
	char szConnectString[256];
	char szAuthWord[32];
	char sz4GCloud[128];
}stuConnectInfo, *pConnectInfo;


//auth p2p信息
typedef struct
{
    //是否支持P2PHDR，0-不支持，1-支持
    int iP2PHDR;
	//保留字节
	char res[128];
}StuAuthP2pInfo;

//MAUTH USR数据
typedef struct stMauthUsrData
{
    StuAuthP2pInfo AuthP2pInfo;
	//后续其他数据在此增加
} MauthUsrData;

//MAUTH USR保存至CFG数据
typedef struct stMauthUsrDataCfg
{
    unsigned int Crc32;       /* CRC32校验值,包含结构体信息*/
    MauthUsrData nMauthUsrData;
} MauthUsrDataCfg;
#define MAUTH_USR_DATA_CFG_LEN    sizeof(MauthUsrDataCfg)


#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif

/*@breif GetDefaultBoardIDBySensor 获取取一个默认支持该sensor的BoardID
 *@return 如果找到了BoardID，返回该BoardID
 *@      没有找到返回0
 *@note  默认版型用于在授权出错的时候，返回一个默认支持的作为系统的BoardId
 */
unsigned long GetDefaultBoardID(void);

/*@breif DeEncryptLicense 解密授权码
 *@param [IN] encrypt_license 需要创建的共享内存的首地址 
 *@param [OUT] p_auth_info 解密出的信息
 *@return 返回错误码
 */
int DeEncryptLicense(const unsigned char *encrypt_license, pShmAuthInfo p_auth_info);

/*@breif InitLicenseAuthorize 初始化Licene授权
 *@return 成功返回0
 *@      失败返回-1
 *@note  系统启动的时候调用，包含如下操作：
 *@      1. uboot和配置下的授权码获取，校验，检测，重写；
 *@      2. 授权状态均已经写入共享内存；
 *@      3. 只允许在系统启动的时候调用一次；
 */
int InitLicenseAuthorize(void);

/*@breif SetLicenseAuthorize 设置授权码
 *@return 成功返回0
 *@      失败返回-1
 *@note  设置授权码，需要操作以下两步：
 *@      1. 修改DEVINFO分区;
 *@      2. 修改AUTHORIZE_LICENSE_FILE_CFG配置文件;
 */
int SetLicenseAuthorize(const pShmAuthInfo p_auth_info);

/*@breif RemoveLicenseAuthorize 强制删除授权
 *@return 无
 *@note  删除(取消)授权，需要操作以下两步：
 *@      1. 清除DEVINFO分区;
 *@      2. 删除AUTHORIZE_LICENSE_FILE_CFG配置文件;
 */
void RemoveLicenseAuthorize(void);

/*@breif Authorize_SetConnectInfo 获取连接串信息
 *@return 成功返回0
 *@       失败返回-1
 *@note  获取连接串信息，2种用法：
 *@      1. lineNum = LINENUM_MAX，获取全部信息;
 *@      2. lineNum = 其他，获取某一项信息;
 */
int Authorize_GetConnectInfo(int lineNum, stuConnectInfo* pconnect_info);

/*@breif Authorize_SetConnectInfo 设置连接串信息
 *@return 成功返回0
 *@       失败返回-1
 *@note  设置连接串信息，2种用法：
 *@      1. lineNum = LINENUM_MAX，设置全部信息;
 *@      2. lineNum = 其他，设置某一项信息;
 *@      先保存至配置，同样也保存连接串至DEVINFO分区;
 */
int Authorize_SetConnectInfo(int lineNum, const stuConnectInfo* pconnect_info);

/*@breif Authorize_GetAuthDevInfo 获取授权信息
 *@return 成功返回0
 *@       失败返回-1
 *@note  获取授权信息，仅操作全局变量
 */
int Authorize_GetAuthDevInfo(stuShmAuthInfo *pAuthInfo);

/*@breif Authorize_SetAuthDevInfo 设置授权信息
 *@return 成功返回0
 *@       失败返回-1
 *@note  设置(修改)授权，需要操作以下两步：
 *@      1. 修改全局变量gAuthInfo;
 *@      2. 修改DEVINFO分区;
 */
int Authorize_SetAuthDevInfo(const stuShmAuthInfo *pAuthInfo);

int Auth_SetAuthP2pInfo(const StuAuthP2pInfo *pstP2pInfo);
int Auth_GetAuthP2pInfo(StuAuthP2pInfo *pstP2pInfo);


void PrintAuthorizeInfo(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif // end of __AUTHORITY_H__

