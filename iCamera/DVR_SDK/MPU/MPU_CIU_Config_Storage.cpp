
/*!
 *\file	 MPU_CIU_Config_Storage.cpp
 *\brief	MPU模块，远程配置，存储管理
 *\author	林云
 *\date	2009/8/17

功能：
-#	远程配置，存储管理

说明：
-#	本文件使用新的网络库实现
 */

#include "MPU_CIU_global.h"
#include "AVL/enumRecType.h"
#include "disk.h"

extern int GetDevCustom_Storage(LPHY_DVR_WORKMODE pcfg);

//extern int SetDevCustom_Storage(DWORD dwUserID, LPHY_DVR_WORKMODE pcfg);
#if 0
#define BEGIN_TIME 	\
do {\
	struct timeval tv1;\
	struct timeval tv2;\
	gettimeofday(&tv1, NULL);

#define END_TIME \
	gettimeofday(&tv2, NULL);\
	MPU_INFO("Get Storage Config, time = %ld", (tv2.tv_sec-tv1.tv_sec)*1000+(tv2.tv_usec-tv1.tv_usec)/1000); \
} while (0)
#else
#define BEGIN_TIME
#define END_TIME
#endif

//设置盘组信息
static int SetDiskGroupInfo(int nDiskGroupType, LPHY_DVR_DISK_GROUP pcfg)
{
	int nRet = IDVR_OK;
#if defined (__SUPPORT_SD_CARD__)
	int nSDKRet, i, nCount;

	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid, nDiskGroupType = %d\n", nDiskGroupType);
		return IDVR_CIU_STRUCT_UNVALID;
	}


	//数据保持时间
	nSDKRet = MPU_STR_DiskGroup_DataKeepTime(nDiskGroupType, pcfg->nDataKeepTime);
	MPU_CHK(0 == nSDKRet);

	//是否启用本盘组
	if (0 == pcfg->bEnable)
	{
		MPU_ERR("0 == pcfg->bEnable, nDiskGroupType = %d\n", nDiskGroupType);
		return nRet;
	}

	//获取视频通道数
	nCount = MAX_ENC_CHANNEL_NUM;
	if (nCount > DVR_MAX_VIDEOIN_NUM)
	{
		nCount = DVR_MAX_VIDEOIN_NUM;
	}
	
	for (i = 0; i < nCount; ++i)
	{
		if (1 == nDiskGroupType)
		{
			//报警盘组
			if (IS_SET_BIT(pcfg->nBindChannelMask, i))
			{
				nSDKRet = MPU_STR_DiskGroup_BindDataCH(nDiskGroupType, i, RECORD_TYPE_ALARM);
				MPU_CHK(0 == nSDKRet);
			}
			else
			{
				nSDKRet = MPU_STR_DiskGroup_UnBindDataCH(nDiskGroupType, i, RECORD_TYPE_ALARM);
				MPU_CHK(0 == nSDKRet);
			}
		}
		else if (2 == nDiskGroupType)
		{
			//冗余盘组
			if (IS_SET_BIT(pcfg->nBindChannelMask, i))
			{
				nSDKRet = MPU_STR_DiskGroup_BindDataCH(nDiskGroupType, i, RECORD_TYPE_GENERAL);
				MPU_CHK(0 == nSDKRet);
				nSDKRet = MPU_STR_DiskGroup_BindDataCH(nDiskGroupType, i, RECORD_TYPE_ALARM);
				MPU_CHK(0 == nSDKRet);
				nSDKRet = MPU_STR_DiskGroup_BindDataCH(nDiskGroupType, i, RECORD_TYPE_MANUAL);
				MPU_CHK(0 == nSDKRet);
				nSDKRet = MPU_STR_DiskGroup_BindDataCH(nDiskGroupType, i, RECORD_TYPE_MOTION);
				MPU_CHK(0 == nSDKRet);
				#if 0	//fjz-notice:待讨论
				nSDKRet = MPU_STR_DiskGroup_BindDataCH(nDiskGroupType, i, RECORD_TYPE_HUMANSHAPE);
				MPU_CHK(0 == nSDKRet);
				#endif
			}
			else
			{
				nSDKRet = MPU_STR_DiskGroup_UnBindDataCH(nDiskGroupType, i, RECORD_TYPE_GENERAL);
				MPU_CHK(0 == nSDKRet);
				nSDKRet = MPU_STR_DiskGroup_UnBindDataCH(nDiskGroupType, i, RECORD_TYPE_ALARM);
				MPU_CHK(0 == nSDKRet);
				nSDKRet = MPU_STR_DiskGroup_UnBindDataCH(nDiskGroupType, i, RECORD_TYPE_MANUAL);
				MPU_CHK(0 == nSDKRet);
				nSDKRet = MPU_STR_DiskGroup_UnBindDataCH(nDiskGroupType, i, RECORD_TYPE_MOTION);
				MPU_CHK(0 == nSDKRet);
				#if 0	//fjz-notice:待讨论
				nSDKRet = MPU_STR_DiskGroup_UnBindDataCH(nDiskGroupType, i, RECORD_TYPE_HUMANSHAPE);
				MPU_CHK(0 == nSDKRet);
				#endif
			}
		}
	}
#endif
	return nRet;
}
#if 0
//获取硬盘效验码，需外部将pcfg清零
static int GetDiskMark(int nIndex, LPHY_DVR_DISK_MARK pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	//填写验证码
	pcfg->nVerifyMark = MPU_STR_Disk_GetVerifyMark(nIndex);

//  MPU_DBG("nIndex = %d, VerifyMark = %d\n", nIndex, pcfg->nVerifyMark);

	return nRet;
}
#endif
#if 0
static int GetDiskType(char *szDevName, LPHY_DVR_DISK_TYPE pcfg)
{
	int nRet = IDVR_OK;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	pcfg->nDiskType = (int) EDVR_HD_GetDiskType(szDevName);

	return nRet;
}
#endif

//获取硬盘信息，需外部将pcfg清零
static int GetDiskInfo(int nIndex, ZW_DISK_COMMON_CFG *pcfg)
{
	int nRet = IDVR_OK, nSDKRet;
#if defined (__SUPPORT_SD_CARD__)	

	tagDiskInfo Info;

	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	memset(&Info, 0, sizeof(tagDiskInfo));
	nSDKRet = MPU_STR_Disk_GetInfo(nIndex, &Info);
	MPU_CHK(0 == nSDKRet);
	if(4 == nSDKRet)
	{
		pcfg->bValid = 1;
		pcfg->dwStatus = eZWDiskStatus_Formatting;
		MPU_DBG("is formating!\n");
		return 0;
	}
	//填写验证码
	pcfg->nVerifyMark = MPU_STR_Disk_GetVerifyMark(nIndex);

	/*@ 磁盘状态见下表
	 eZWDiskStatus_Idle = 0,
	 eZWDiskStatus_Writting = 1,
	 eZWDiskStatus_Reading,
	 eZWDiskStatus_Exception,
	 eZWDiskStatus_Error,
	 eZWDiskStatus_Readonly,
	 eZWDiskStatus_Formatting = 6,
	 eZWDiskStatus_Warn,
	 eZWDiskStatus_ReadWrite = 8,
	 eZWDiskStatus_NoDisk = 9, // 无磁盘
	 eZWDiskStatus_ExpNeedFormat = 10, // 磁盘异常需要格式化
	 eZWDiskStatus_ExpNeedChangeDisk = 11, // 磁盘异常需要换卡
	 eZWDiskStatus_FormatFailNeedReboot = 12 // 磁盘格式化失败需要重启
	 */
	pcfg->dwStatus = (eZWDiskStatus)MPU_STR_Disk_GetDiskStatus(nIndex);

	//硬盘类型
	strncpy(pcfg->szProductType, "SD", sizeof (pcfg->szProductType) - 1);
	pcfg->dwBusID = 0;
	//物理位置,SATA1
	nSDKRet = MPU_STR_Disk_GetBusNumber(nIndex, (int *)&pcfg->dwBusID);
	MPU_CHK(0 == nSDKRet);

	//容量,M为单位
	pcfg->dwTotalCapacity = Info.Total_space;
	
	//分区信息
	pcfg->dwAvailableCapacity=0;
	pcfg->dwAvailableCapacity = Info.tPartitions[0].Available_space;

	/*
		UnFormatted  = 0, // 未格式化
		Formatting	 = 1, // 格式化中
		FormatOk	 = 2, // 格式化成功
		FormatErr	 = 3, // 格式化失败
		UmountFail	 = 4, // 卸载失败
		DeviceIsBusy  = 5, // 设备忙
	*/
	pcfg->nFormatStatus = MPU_STR_Disk_GetDiskFormatStatus(nIndex);
	pcfg->nFormatStatus = pcfg->nFormatStatus > 5 ? 3 : pcfg->nFormatStatus;

	if (eZWDiskFormatStatus_Formatting == pcfg->nFormatStatus)
	{
		pcfg->dwStatus = eZWDiskStatus_Formatting;
	}
	else if (eZWDiskFormatStatus_UmountFail == pcfg->nFormatStatus)
	{
		pcfg->dwStatus = eZWDiskStatus_FormatFailNeedReboot;
	}
	else if (eZWDiskFormatStatus_FormatErr == pcfg->nFormatStatus)
	{
		if (eZWDiskStatus_NoDisk != pcfg->dwStatus)
		{
			pcfg->dwStatus = eZWDiskStatus_ExpNeedChangeDisk;
		}
	}

	MPU_AVL_GetDiskResourceSize(0, &pcfg->nRecordCapacityPercent, &pcfg->nPictureCapacityPercent);

    MPU_DBG("--GetDiskInfo--%ld--%s--%d--%ld--%ld--%d--%d--%d\n",
    	pcfg->dwBusID,
    	pcfg->szProductType,
    	pcfg->dwStatus,
    	pcfg->dwTotalCapacity,
    	pcfg->dwAvailableCapacity,
    	pcfg->dwType,
    	pcfg->nVerifyMark,
    	pcfg->nFormatStatus);
#endif
	return nRet;
}

//获取存储管理配置信息，需外部将pcfg清零
static int GetStorageConfig(LPHY_DVR_STORAGE_CFG pcfg)
{
	int nRet = IDVR_OK, i;
#if defined (__SUPPORT_SD_CARD__)
	//! 本结构体是否有效，=0表示无效
	pcfg->bValid = 1;

	GetDiskInfo(0, &pcfg->struDiskCommon[0]);

	for (i = 1; i < 4; ++i)
	{
		pcfg->struDiskGroup[i].bValid = 1;
		pcfg->struDiskGroup[i].bEnable = 0;
	}
#endif
	return nRet;
}

/*设置存储管理配置信息*/
static int SetStorageConfig(LPHY_DVR_STORAGE_CFG pcfg)
{
	int nRet = IDVR_OK, i, nStorageWorkMode;
#if defined (__SUPPORT_SD_CARD__)
	//! 本结构体是否有效，=0表示无效
	if (0 == pcfg->bValid)
	{
		MPU_DBG("0 == pcfg->bValid\n");
		return IDVR_CIU_STRUCT_UNVALID;
	}

	//高级存储管理，0为禁用，1为启用
	//SetDevCustom_Storage(dwUserID, &pcfg->struWorkMode);


	//int nDiskGroupType  盘组类型。0为普通盘组，1为报警盘组，2为冗余盘组，3为备份盘组
	SetDiskGroupInfo(0, &pcfg->struDiskGroup[0]);

	nStorageWorkMode = MPU_STR_DiskGroup_GetWorkMode();
	if (nStorageWorkMode > 0)
	{
		for (i = 1; i < 4; ++i)
		{
			SetDiskGroupInfo(i, &pcfg->struDiskGroup[i]);
		}
	}
#endif
	return nRet;
}

int CIU_GetConfigure_Storage(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, BYTE * pBuffer, DWORD * pdwBufLen)
{
	int nRet = IDVR_OK;
#if defined (__SUPPORT_SD_CARD__)
	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			/*获取存储管理配置信息 */
		case STORAGEINFO_ALL:
			{
				BEGIN_TIME;
				LPHY_DVR_STORAGE_CFG pcfg = (LPHY_DVR_STORAGE_CFG) pBuffer;

				//判断buf空间是否足够
				if (*pdwBufLen < STORAGE_CFG_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", STORAGE_CFG_LEN, *pdwBufLen);
					break;
				}

				memset(pcfg, 0, STORAGE_CFG_LEN);
				*pdwBufLen = STORAGE_CFG_LEN;

				nRet = GetStorageConfig(pcfg);
				END_TIME;
			}
			break;

		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}

#endif
	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}

int CIU_SetConfigure_Storage(DWORD dwUserID, DWORD dwMajorType, DWORD dwMinorType, const BYTE * pBuffer, DWORD dwBufLen)
{
	int nRet = IDVR_OK;
#if defined (__SUPPORT_SD_CARD__)
	MPU_DBG("ID %d, Major %d, Minor %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType);

	switch (dwMinorType)
	{
			/*设置存储管理配置信息 */
		case STORAGEINFO_ALL:
			{
				///////////////////////////////////////////////////////////////////////////////
				//判断buf空间是否足够
				if (dwBufLen < STORAGE_CFG_LEN)
				{
					nRet = IDVR_CIU_BUF_SMALL;
					MPU_ERR("buffer is too small, need %d, real %lu\n", STORAGE_CFG_LEN, dwBufLen);
					break;
				}

				LPHY_DVR_STORAGE_CFG pcfg = (LPHY_DVR_STORAGE_CFG) pBuffer;

				nRet = SetStorageConfig(pcfg);
			}
			break;

		default:
			{
				nRet = IDVR_CIU_UNSUPPORTED_MINORTYPE;
				MPU_ERR("unsupported dwMinorType = %lu\n", dwMinorType);
			}
			break;
	}

#endif
	MPU_DBG("ID %d, Major %d, Minor %d, return %d\n", (int) dwUserID, (int) dwMajorType, (int) dwMinorType, nRet);
	return nRet;
}
