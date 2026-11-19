/*** 
 * @Author: Sun Yuhui
 * @Date: 2022-06-09 20:30:28
 * @LastEditTime: 2022-08-08 10:55:53
 * @LastEditors: Sun Yuhui
 * @Description: 对KS MegROM部分接口封装，防止小程序链接太多的库导致过大
 * @FilePath: /deepsky/modules/pkgks/pkgks.h
 * @deepelves.com
 */

// 链接说明：
//   使用该库，你需要按顺序连接如下库：-lpkgks -lupgrade -lhalmod -lmauth -lusart -lmvstd -lssl -lcrypto -lz -laes -lpthread -lm -ldl -lrt
//   使用升级模块接口不需要初始化halmod

#ifndef __PKGKS_H__
#define __PKGKS_H__

/**
 * @description: 开启看门狗
 * @param {int} timeout 超时时间
 * @return {int} 0-成功 !0-失败
 */
int Pkgks_HalMod_WatchDog_Open(int timeout);

/**
 * @description: 关闭看门狗
 * @return {int} 0-成功 !0-失败
 */
int Pkgks_HalMod_WatchDog_Close();

/**
 * @description: 喂狗
 * @return {int} 0-成功 !0-失败
 */
int Pkgks_HalMod_WatchDog_Feed();


typedef enum data_unit_type
{
    DATA_UNIT_TYPE_AUTHCODE = 0,
    DATA_UNIT_TYPE_LINKCODE,
    DATA_UNIT_TYPE_DID,
    DATA_UNIT_TYPE_NUM,
} DATA_UNIT_TYPE_E;

#define MAX_DATA_UNIT_NUM   32
#define UNIT_DATA_SIZE      (1024-60-4)
#define USR_DATA_SIZE       (64 * 1024 - 16)
typedef struct data_unit
{
    unsigned int    crc32;
    char   rsv[60];
    char   data[UNIT_DATA_SIZE];
} DATA_UNIT_S;

typedef struct usrdata
{
    DATA_UNIT_S units[MAX_DATA_UNIT_NUM];
    char rsv[USR_DATA_SIZE - (1024 * MAX_DATA_UNIT_NUM)];
} XRS_USR_DATA;

/**
 * @description: 存储单元数据
 * @param {char*} data 数据
 * @param {DATA_UNIT_TYPE_E} eType 数据类型
 * @return {int} 0:成功 0<失败
 */
int Pkgks_HalMod_SetDataUnit(char* data, unsigned int size, DATA_UNIT_TYPE_E eType);

/**
 * @description:
 * @param {char} *buffer 存储地址
 * @param {unsigned int} size 存储区大小
 * @param {DATA_UNIT_TYPE_E} eType 单元数据类型
 * @return {int} 0:成功 0<失败
 */
int Pkgks_HalMod_GetDataUnit(char* buffer, unsigned int size, DATA_UNIT_TYPE_E eType);

/***
 * @description: 升级回调函数
 * @param {int} wParam 固定0
 * @param {int} *plParam 升级进度0-100
 * @return {void} 无
 */
typedef void (*Upgrade_Notify_Callback)(int wParam, int *plParam);

// 设备版型的最大长度
#define MAX_UPGRADE_DEVTYPE_LEN 512
#define MAX_UPGRADE_VER_LEN 64

typedef struct upgrade_info
{
    char Ver[MAX_UPGRADE_VER_LEN];         /* 升级文件版本信息 */
    char DevList[MAX_UPGRADE_DEVTYPE_LEN]; /* 每个机型使用"|"隔开(UPGRADE_IGNORE_STR或NULL-表示忽落此项检查) */
} UPGRADE_INFO_S;

/***
 * @description:
 * @param {Upgrade_Notify_Callback} NotifyFunc 回调函数
 * @param {char[MAX_DEVTYPE_LEN]} DevType 版型
 * @return {int} 0-成功，非0-失败
 */
int Pkgks_Upgrade_Init(Upgrade_Notify_Callback NotifyFunc, char DevType[MAX_UPGRADE_DEVTYPE_LEN]);

/**
 * @description: 升级文件校验及获取升级文件相关信息
 * @param {void} *pUpgradeAddr 升级文件地址
 * @param {int} UpgradeSize	升级文件大小
 * @param {UPGRADE_INFO_S} *pUpgradeInfo 升级文件信息
 * @return {int} 0-成功，非0-失败
 */
int Pkgks_Upgrade_Check(void *pUpgradeAddr, int UpgradeSize, UPGRADE_INFO_S *pUpgradeInfo);

/***
 * @description: 升级
 * @param {char} *pUpgradeAddr 升级文件地址
 * @param {int} UpgradeSize 升级文件大小
 * @return {int} 0-成功，非0-失败
 */
int Pkgks_Upgrade_Update(char *pUpgradeAddr, int UpgradeSize);

/***
 * @description: 关闭升级功能
 * @return {*} 无
 */
int Pkgks_Upgrade_DeInit();

#endif