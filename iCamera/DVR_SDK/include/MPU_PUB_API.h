
/*!
 *\file		MPU_PUB_API.h
 *\brief	MPU模块，公共基础相关API定义
 *\author	林云
 *\date		2009/03/20

功能：
-#	全局函数定义
 */

#ifndef _INC_MPU_PUB_API_H_
#define _INC_MPU_PUB_API_H_

#include <time.h>
#include "PUB/DEVICEINFO.h"
#include "PUB/DEVICECAP.h"
#include "PUB/DEVICEVERSION.h"
#include "newhie/HieServer_Common.h"
#include "newhie/HieServer_Cfg.h"	//网络SDK


typedef int (*PANEL_CALLBACK) (int nCmd, int nParam);

//升级模式
typedef enum
{
    NOT_UPGRADE_MODE = 0,   //非升级模式
    LOCAL_UPGRADE_MODE,     //本地升级模式(网页，easytool等局域网方式)
    CLOUD_UPGRADE_MODE,     // 云升级模式(APP等互联网升级方式)
    MODE_BUTT
}UPGRADE_MODE;

//!\brief       设置面板操作回调函数
//!\param   pCallback   回调函数指针
//!\return  0表示成功，其它为错误码
int MPU_PUB_SetPanelCallback(PANEL_CALLBACK pCallback);

//!\brief   获取设备信息
//!\param   pDI             设备信息
//!\return  0表示成功，其它为错误码
int MPU_PUB_GetDeviceInfo(PDEVICEINFO pDI);

//!\brief   设置设备信息
//!\param   pDI             设备信息
//!\return  0表示成功，其它为错误码
int MPU_PUB_SetDeviceInfo(PDEVICEINFO pDI);

//!\brief   获取设备能力
//!\param   pDC             设备能力
//!\return  0表示成功，其它为错误码
int MPU_PUB_GetDeviceCap(PDEVICECAP pDC);

//!\brief   获取设备能力
//!\param   pDV             设备版本
//!\return  0表示成功，其它为错误码
int MPU_PUB_GetDeviceVersion(PDEVICEVERSION pDV,int resVersion);

//!\brief   设置设备系统时间
//!\param   tNewTime        新的系统时间
//!\return  0表示成功，其它为错误码
int MPU_PUB_SetSystemTime(time_t tNewTime);

//!\brief       获取设备别名
//!\param   szAlias     别名缓冲区(21 bytes)
//!\param   nLen        缓冲区大小
//!\return  0表示成功，其它为错误码
int MPU_PUB_GetDevAlias(char *szAlias, int nLen);

//!\brief       获取视频通道别名
//!\param   nChannel    通道号
//!\param   szAlias     别名缓冲区(21 bytes)
//!\param   nLen        缓冲区大小
//!\return  0表示成功，其它为错误码
int MPU_PUB_GetCameraAlias(int nCh, char *szAlias, int nLen);

//!\brief       获取报警输入别名
//!\param   nCh 通道号
//!\param   szAlias     别名缓冲区(21 bytes)
//!\param   nLen        缓冲区大小
//!\return  0表示成功，其它为错误码
int MPU_PUB_GetAlarmInputAlias(int nCh, char *szAlias, int nLen);

//!\brief       获取报警输出别名
//!\param   nCh 通道号
//!\param   szAlias     别名缓冲区(21 bytes)
//!\param   nLen        缓冲区大小
//!\return  0表示成功，其它为错误码
int MPU_PUB_GetAlarmOutputAlias(int nCh, char *szAlias, int nLen);

//!\brief       获取别名，需外部将缓冲区清零
int MPU_PUB_GetAlias(const char *szSection, const char *szDefaultName, LPHY_DVR_ALIAS_CFG_1 pcfg);

//设置别名
int MPU_PUB_SetAlias(const char *szSection, LPHY_DVR_ALIAS_CFG_1 pcfg);

//!\brief   判断是否是8008HT或8016HT
//!\return  TRUE表示是，FALSE表示不是
int MPU_PUB_Is8008_or_8016HT();

//设置SPOT模式
int MPU_PUB_SetSpotMode(int nMode);

int MPU_PUB_GetSpotMode();

int MPU_PUB_GetCustomeFunction(LPHY_DVR_CUSTOMER_INFO pDV);
int MPU_PUB_GetCustomer(LPHY_DVR_CUSTOMER_INFO pDV);
int MPU_PUB_GetDevInstallInfo(ZW_MULTI_CHANNEL_INSTALLTION_INFO *pcfg);

int MPU_PUB_SetDeviceFunction(const char *pBuffer);

int MPU_PUB_GetDeviceFunction(char *pBuffer);

int MPU_PUB_SetCustomeFunction(LPHY_DVR_CUSTOMER_INFO pDV);
int MPU_PUB_SetCustomer(LPHY_DVR_CUSTOMER_INFO pDV);
int MPU_PUB_SetDevInstallInfo(ZW_MULTI_CHANNEL_INSTALLTION_INFO *pcfg);

//获取当前升级模式
int MPU_PUB_SetCurrentUpgradeMode(UPGRADE_MODE enUpgradeMode);

//设置当前升级模式
int MPU_PUB_GetCurrentUpgradeMode(UPGRADE_MODE *pUpgradeMode);


#ifdef __IS_SUPPORT_CLOUDWORLD__
//!\brief       设置设备云存储模式 国内/国际
//!\return  0表示成功，其它为错误码
int MPU_PUB_SetCloudModeCfg(HY_DVR_CLOUD_STORAGE_MODE *pcfg);

//!\brief       获取设备云存储模式
//!\return  0表示成功，其它为错误码
int MPU_PUB_GetCloudModeCfg(HY_DVR_CLOUD_STORAGE_MODE *pcfg);
#endif

#ifdef __IS_SUPPORT_CLOUDWORLD__
//!\brief       设置设备云存储模式 国内/国际
//!\return  0表示成功，其它为错误码
int MPU_PUB_SetCloudModeCfg(HY_DVR_CLOUD_STORAGE_MODE *pcfg);

//!\brief       获取设备云存储模式
//!\return  0表示成功，其它为错误码
int MPU_PUB_GetCloudModeCfg(HY_DVR_CLOUD_STORAGE_MODE *pcfg);
#endif


//!\brief       设置设备云存储信息
//!\return  0表示成功，其它为错误码
int MPU_PUB_SetCloudStorageInfo(HY_DVR_CLOUD_STORAGE *pcfg);
//!\brief       获取设备云存储信息
//!\return  0表示成功，其它为错误码
int MPU_PUB_GetCloudStorageInfo(HY_DVR_CLOUD_STORAGE *pcfg);
//!\brief		获取设备云存储信息
//!\return	0表示成功，其它为错误码
int MPU_PUB_GetCloudStorageBaseCfg(HY_DVR_CLOUD_STROAGE_BASE_CONFIG *pcfg);
//!\brief       设置设备云存储信息
//!\return  0表示成功，其它为错误码
int MPU_PUB_SetCloudStorageBaseCfg(HY_DVR_CLOUD_STROAGE_BASE_CONFIG *pcfg);
//!\brief		设置设备云存储信息
//!\return	0表示成功，其它为错误码
int MPU_PUB_SetCloudStorageSubscribeStatus(HY_DVR_CLOUD_STROAGE_OPEN_STATUS *pcfg);
//!\brief		获取设备云存储录像配置
//!\return	0表示成功，其它为错误码
int MPU_PUB_GetCloudStorageRecordCfg(HY_DVR_CLOUD_STROAGE_RECORD_PLAN *pcfg);
//!\brief		设置设备云存储录像配置
//!\return	0表示成功，其它为错误码
int MPU_PUB_SetCloudStorageRecordCfg(HY_DVR_CLOUD_STROAGE_RECORD_PLAN *pcfg);

// 获取设备云存储录像配置（多通道）
int MPU_PUB_GetCloudStorageRecordCfg_V2(HY_DVR_CLOUD_STROAGE_RECORD_PLAN *pcfg, int nChannelMaxNum);

// 设置设备云存储录像配置（多通道）
int MPU_PUB_SetCloudStorageRecordCfg_V2(HY_DVR_CLOUD_STROAGE_RECORD_PLAN *pcfg, int nChannelMaxNum);

#endif							/* _INC_MPU_PUB_API_H_ */
