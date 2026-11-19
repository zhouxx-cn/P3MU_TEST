 
/*!
 *\file		MPU_STR_API.cpp
 *\brief	MPU模块，存储管理相关接口实现
 *\author	林云
 *\date		2009/03/20

功能：
-#	模块初始化、逆初始化
-#	公共任务
 */


#include "MPU_global.h"

#ifndef DEBUG_NOT_USE_STR_LIB
#include "STR/CDiskGroupManager.h"
#include "STR/CDiskManager.h"
#include "STR/CDPManager.h"
#endif

#ifndef DEBUG_NOT_USE_SNP_LIB
#include "SnapShot/CSnapshotManager.h"
#endif


static NEWDISKCALLBACK s_pNewDiskCallback = NULL;

static DISKSTATUSCALLBACK s_pDiskStatusCallback = NULL;


#ifndef DEBUG_NOT_USE_STR_LIB

#ifndef USE_ICAMERA
//!\brief   发现新硬盘，处理回调函数
//!\param   BusNo       硬盘物理编号
//!\return  0表示成功，其它为错误码
static int callback_FindNewDisk(int BusNo);
#endif

//!\brief   硬盘插拔状态通知，处理回调函数
//!\param   pszDev      硬盘名称
//!\param   bAdd        true表示添加，false表示删除
//!\return  0表示成功，其它为错误码
static void callback_DiskHotPlug(char *pszDev, bool bAdd);

#endif


//!\brief   初始化存储管理
//!\return  0表示成功，其它为错误码
int STR_Init()
{
//  MPU_DBG("enter\n");

#ifndef DEBUG_NOT_USE_STR_LIB
	//初始化硬盘
	CDiskManager *pDM = CDiskManager::GetInstance();

	if (NULL != pDM)
	{
#ifndef USE_ICAMERA
		pDM->SetNewDiskCallBack(callback_FindNewDisk);	//设置新硬盘通知回调
#endif
		pDM->SetDiskStatusCallBack(callback_DiskHotPlug);	//设置硬盘插拔通知回调
		pDM->Init();

		MPU_DBG("CDiskManager init ok\n");
	}
	else
	{
		MPU_ERR("no CDiskManager instance\n");
		MPU_CHK(NULL != pDM);
		return IDVR_MPU_STR_NO_DISK_MANAGER_INSTANCE;
	}

	//初始化盘组
	CDiskGroupManager *pDGM = CDiskGroupManager::GetInstance();

	if (NULL != pDGM)
	{
		pDGM->Init();
		MPU_DBG("CDiskGroupManager init ok\n");
	}
	else
	{
		MPU_ERR("no CDiskGroupManager instance\n");
		MPU_CHK(NULL != pDGM);
		return IDVR_MPU_STR_NO_DISKGROUP_MANAGER_INSTANCE;
	}

	//初始化数据管理
	CDPManager *pDPM = CDPManager::GetInstance();

	if (NULL != pDPM)
	{
		pDPM->Init();

		MPU_DBG("CDPManager init ok\n");
	}
	else
	{
		MPU_ERR("no CDPManager instance\n");
		MPU_CHK(NULL != pDPM);
		return IDVR_MPU_STR_NO_RECORD_MANAGER_INSTANCE;
	}
#endif

	return IDVR_OK;
}

//!\brief   逆初始化存储管理
//!\return  0表示成功，其它为错误码
int STR_Uninit()
{

#ifndef DEBUG_NOT_USE_STR_LIB
	//数据管理
	CDPManager::ReleaseInstance();

	//盘组管理
	CDiskGroupManager::ReleaseInstance();

	//硬盘管理
	CDiskManager::ReleaseInstance();
#endif

	return IDVR_OK;
}


#ifndef DEBUG_NOT_USE_STR_LIB

#ifndef USE_ICAMERA
//!\brief   发现新硬盘，处理回调函数
//!\param   BusNo       硬盘物理编号
//!\return  0表示成功，其它为错误码
static int callback_FindNewDisk(int BusNo)
{
//  MPU_DBG("enter\n");

	if (NULL != s_pNewDiskCallback)
	{
		return (*s_pNewDiskCallback) (BusNo);
	}
	return IDVR_OK;
}
#endif

//!\brief   硬盘插拔状态通知，处理回调函数
//!\param   pszDev      硬盘名称
//!\param   bAdd        true表示添加，false表示删除
//!\return  0表示成功，其它为错误码
static void callback_DiskHotPlug(char *pszDev, bool bAdd)
{
//  MPU_DBG("enter\n");

	if (NULL != s_pDiskStatusCallback)
	{
		(*s_pDiskStatusCallback) (pszDev, bAdd);
	}
}

#endif


//!\brief       当系统插入一个无法识别（没有保留分区）的硬盘时，提示用户
//!\param   回调函数指针
//!\return  成功为0，失败为错误码
int MPU_STR_Disk_SetNewDiskCallBack(NEWDISKCALLBACK pCallback)
{
	//MPU_DBG("enter\n");

	s_pNewDiskCallback = pCallback;
	return IDVR_OK;
}

//!\brief 设置硬盘被卸载或者插入的回调函数
//!\param callabck 回调指针
//!\return 成功为0，失败为错误码
int MPU_STR_Disk_SetDiskStatusCallBack(DISKSTATUSCALLBACK pCallback)
{
	//MPU_DBG("enter\n");

	s_pDiskStatusCallback = pCallback;
	return IDVR_OK;
}
