/*!
 *\file		MPU_CloudUpgrade_API.h
 *\brief		MPU模块，云升级相关接口定义
 *\author	樊景柱
 *\date		2017/12/29

功能：
-#	设备升级列表服务器配置管理
-#	设备升级包服务器配置管理
-#   设备升级列表获取
-#   云端升级包下载及状态查询
-#   云升级启动及状态返回
-#   云升级结果查询
*/

#ifndef _INC_MPU_CLOUD_UPGRADE_API_H_
#define _INC_MPU_CLOUD_UPGRADE_API_H_

#include "newhie/HieServer_Cfg.h"
//!\brief      设置设备升级列表服务器配置
int MPU_CloudUpgrade_SetUpgradeListServerCfg(UPGRADE_LIST_SERVER_CFG *pCfg);

//!\brief      获取设备升级列表(阻塞式)
int MPU_CloudUpgrade_GetUpgradeList(UPGRADE_FIRMWARE_INFO_LIST *pCfg);

//!\brief      指定并启动升级包下载(非阻塞式)
int MPU_CloudUpgrade_UpgradeFileDownloadStart(UPGRADE_FILE_DOWNLOAD_START *pCfg);

#if defined (__IS_SUPPORT_P6SMESSAGE__)
int MPU_CloudUpgrade_UpgradeFileDownloadStart_V1(UPGRADE_FILE_DOWNLOAD_START_V1 *pCfg);
#endif

//!\brief      获取升级包下载状态
int MPU_CloudUpgrade_GetUpgradeFileDownloadStatus(UPGRADE_FILE_DOWNLOAD_STATUS *pCfg);

//!\brief      启动升级(非阻塞式)
int MPU_CloudUpgrade_UpgradeStart(UPGRADE_CONTROL_START *pCfg);

//!\brief      获取升级结果
int MPU_CloudUpgrade_GetUpgradeResult(UPGRADE_RESULT *pCfg);

//!\brief      取消下载(阻塞式)
int MPU_CloudUpgrade_UpgradeFileDownloadStop(UPGRADE_FILE_DOWNLOAD_STOP *pCfg);

//!\brief      获取云升级设备信息
int MPU_CloudUpgrade_GetUpgradeInfo(HY_DVR_CLOUD_UPDATE_BASIC_INFO *psDev);

#endif

