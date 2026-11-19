
/*!
*\file   CDiskManager.cpp 
*\brief  磁盘管理实现
*\author 樊聪
*\date	 2017年5月15日 14:22:48
*\version 0.2
*/
#ifdef __SUPPORT_SD_CARD__
#include <pthread.h>
//#include <iostream.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "global_macro.h"
#include "CDiskGroupManager.h"
#include "CDiskGroup.h"
#include "CDPManager.h"
#include "tagDiskInfo.h"
#include "tagPartition.h"
#include "CDiskManager.h"
#include "pkg_interface.h"

#include "idvr_errno.h"
#include "debug.h"
#include "PAR/CConfig.h"
#include "CHardDisk.h"
#include "AlarmCfg.h"
#include "pthreadmacro.h"
#include "macro_pthread.h"
#include "fm_shm.h"
#include "mysystem.h"
#include "str_fs.h"

#include "STR_global.h"
#include <iostream>
#include "common_pub.h"

extern "C"
{
#include "disk.h"
}

extern "C"
{
#include "usb.h"
}

/**************************************************************************
*                            宏定义                                       *
 **************************************************************************/
#define _DISK_VERSION__  "1.0.0 svn:1234"

/**************************************************************************
*                            全局变量初始化                               *
 **************************************************************************/


//硬盘个数
const int DISKMANGER_MAX_DISK_NUM = 1;//8;

sem_t CDiskManager::cond;

//static pthread_mutex_t m_mutex = PTHREAD_MUTEX_INITIALIZER;

static unsigned int bSupportSDCard = 0;

/**************************************************************************
*                               方法实现                                  *
 **************************************************************************/


//!\brief 获取硬盘个数
//!\param 无
	//!\return 返回磁盘个数
int CDiskManager::GetNum(int *Num)
{
	// TODO: Add your specialized code here.    
	*Num = (unsigned int) EDVR_HD_GetTotalDiskCount();

	if ((unsigned int) (*Num) != nDiskNum)
	{
		Update();
	}

	return 0;
}

//!\brief 获取磁盘物理位置编号
//!\param int nIndex 硬盘索引号
	//!\return 成功为0，失败见错误号
int CDiskManager::GetBusNumber(const unsigned int nIndex, int *BusNo)
{
	// TODO: Add your specialized code here.
	if (nIndex >= nDiskNum)
	{
		DEBUG_MSG("%d超过硬盘个数%d:\n", nIndex, nDiskNum);
		return IDVR_ERROR_PARAMETER;
	}
	*BusNo = theCHardDisk[nIndex].GetBusNumber();
	return 0;
}

//!\brief 设置硬盘被卸载或者插入的回调函数
//!\param callabck 回调指针
//!\return 成功为0，失败为错误码
int CDiskManager::SetDiskStatusCallBack(void (*CallBack) (char *Dev, bool bAdd))
{
	DiskStatusCallBack = CallBack;
	return 0;
}

int CDiskManager::IsNew(const unsigned int nIndex, bool * bNew)
{
	if (nIndex >= nDiskNum)
	{
		DEBUG_MSG("%d超过硬盘个数%d:\n", nIndex, nDiskNum);
		return IDVR_ERROR_PARAMETER;
	}

	*bNew = theCHardDisk[nIndex].IsNewDisk();
	return 0;
}

int CDiskManager::GetInfo(const unsigned int nIndex, tagDiskInfo * Info)
{
	int ret = 0;

	ret = theCHardDisk[nIndex].GetDiskStatus();
	printf("***** get disk status %d...\n", ret);
	if(12 == ret)
	{
		theCHardDisk[nIndex].GetLastInfo(Info);
		return 12;
	}

	printf("start get disk info...\n");

	Update();
	
	printf("--GetInfo--%d--%d--------\n", nIndex, nDiskNum);

	ret = theCHardDisk[nIndex].GetInfo(Info);

	return ret;
}

int CDiskManager::GetPartitionInfo(const unsigned int nDiskIndex, const unsigned int nPartitionIndex, tagPartition * Info)
{
	tagDiskInfo DiskInfo;

	int ret;

	if (nDiskIndex >= nDiskNum)
	{
		DEBUG_MSG("%d超过硬盘个数%d:\n", nDiskIndex, nDiskNum);
		return IDVR_ERROR_PARAMETER;
	}

	ret = GetInfo(nDiskIndex, &DiskInfo);
	if ((5!=ret) && (13!=ret) && (23!=ret) && (24!=ret))
	{
		DEBUG_MSG("SD Card status is %d error!\n", ret);
		return IDVR_STR_ERROR_DISK_INFO_FAILED;
	}

	if (DiskInfo.Partitions < nPartitionIndex)
	{
		DEBUG_MSG("%d超过分区个数%d:\n", nPartitionIndex, DiskInfo.Partitions);
		return IDVR_ERROR_PARAMETER;
	}

	memcpy(&DiskInfo.tPartitions[nPartitionIndex], Info, sizeof (tagPartition));

	return 0;
}

int CDiskManager::FormatPartition(const unsigned int nDiskIndex, const unsigned int nPartitionIndex)	//, const unsigned int Type)
{
	int ret = 0;

	if (nDiskIndex >= nDiskNum)
	{
		TRACE("%d超过硬盘个数%d:\n", nDiskIndex, nDiskNum);
		return IDVR_ERROR_PARAMETER;
	}

printf("into FormatPartition %d-%d\n",nDiskIndex,nPartitionIndex);
	UpdateDiskData(-1);
#ifdef __PKGSTREAM_STORTAGE__
	if (0 == nPartitionIndex)
#elif defined(__JFS_STORAGE__)
	if (1)
#endif
	{
#ifdef USE_SD_PKG
	ret = theCHardDisk[nDiskIndex].Format(nPartitionIndex);
	if (ret == -1)
	{						//分区未被卸载，无法格式化
		TRACE("Format\n");
		ret = -1;
	}
#else
	ret = theCHardDisk[nDiskIndex].UnMount(nPartitionIndex);

	if (ret == -1)
	{						//分区未被卸载，无法格式化
		TRACE("IDVR_STR_ERROR_PARTITION_NOUNMOUNT\n");
		ret = IDVR_STR_ERROR_PARTITION_NOUNMOUNT;
	}

	if (0 == ret)
	{						//卸载成功
		ret = theCHardDisk[nDiskIndex].Format(nPartitionIndex);
	}

	if (0 == ret)
	{						//卸载或者格式化成功
		ret = theCHardDisk[nDiskIndex].Mount(nPartitionIndex);
	}
#endif
	}
	else						// if (1 == Type)
	{
	
	printf("11111 FormatPartition\n");
		DEBUG_MSG("格式化硬盘%d分区%d\n", nDiskIndex, nPartitionIndex);
		ret = Macro_DiskPartFormat(nDiskIndex, nPartitionIndex);
	}
	printf("22222s FormatPartition\n");

	UpdateDiskData(1);
	Update();

	return ret;
}

int CDiskManager::UnMount(const unsigned int nBusNo /*现在为传入的BusNo */ )
{
	int ret = 0;

	tagDiskInfo diskinfo;

	unsigned int nDiskIndex = 0, i;

	CDiskGroupManager *theCDiskGroupManager = CDiskGroupManager::GetInstance();


	LOCK(mutex);
	for (i = 0; i < nDiskNum; i++)
	{
		if (theCHardDisk[i].GetBusNumber() == (int) nBusNo)
		{
			nDiskIndex = i;
			break;
		}
	}

	if (i >= nDiskNum)
	{
		TRACE("没有找到该硬盘%d\n", nBusNo);
		UNLOCK(mutex);
		return -1;
	}

	if (theCHardDisk[nDiskIndex].GetBusNumber() != (int) nBusNo)
	{
		TRACE("该硬盘%d可能已经不存在\n", nBusNo);
		UNLOCK(mutex);
		return -1;
	}

	theCHardDisk[nDiskIndex].GetInfo(&diskinfo);

	if (theCHardDisk[nDiskIndex].GetBusNumber() != (int) nBusNo)
	{
		TRACE("该硬盘%d可能已经不存在\n", nBusNo);
		UNLOCK(mutex);
		return -1;
	}
#ifdef __PKGSTREAM_STORTAGE__
	/*卸载系统保留分区 */
	ret = UnMount(nDiskIndex, 0);


	if (-1 == ret)
	{
		TRACE("卸载系统保留分区时出错\n");
		UNLOCK(mutex);
		return ret;
	}

	if (theCHardDisk[nDiskIndex].GetBusNumber() != (int) nBusNo)
	{
		TRACE("该硬盘%d可能已经不存在\n", nBusNo);
		UNLOCK(mutex);
		return -1;
	}

#endif
	char dev[20];

	theCHardDisk[nDiskIndex].GetDev(dev,sizeof(dev));
	Macro_HotplugDiskRemove(dev);

	if (NULL != DiskStatusCallBack)
	{
		DiskStatusCallBack(dev, false);
	}

	//在所有盘组内没有找到该硬盘的分区
	ret = theCHardDisk[nDiskIndex].UnMount();
	UNLOCK(mutex);
	if (ret == 0)
	{
		UpdateDiskData(-1);
		ret = Update();
	}
	else
	{
		Macro_HotplugDiskAdd(dev);

		if (NULL != DiskStatusCallBack)
		{
			DiskStatusCallBack(dev, true);
		}
#ifdef __PKGSTREAM_STORTAGE__
		/*挂载系统保留分区 */
		Mount(nDiskIndex, 0);
#endif

#ifdef __JFS_STORAGE__
		theCHardDisk[nDiskIndex].Mount();
#endif

	}


	if (NULL != theCDiskGroupManager)
	{
		theCDiskGroupManager->Update();
	}


	return ret;
}

/*@brief GetDiskStatus 获取硬盘当前的状态
 *@param int nIndex 硬盘索引号
 *@return 见下表
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
int CDiskManager::GetDiskStatus(const unsigned int nIndex)
{
	int ret = 0;

	tagDiskInfo diskinfo;

	if (nIndex >= nDiskNum)
	{
		DEBUG_MSG("%d超过硬盘个数%d:\n", nIndex, nDiskNum);
		return IDVR_ERROR_PARAMETER;
	}

	ret = theCHardDisk[nIndex].GetDiskStatus();

    /* 仅仅在正常情况下获取信息，其他非正常情况下，返回状态 */
	if ((0==ret) || (1==ret) || (6==ret) || (8==ret))
	{
		theCHardDisk[nIndex].GetInfo(&diskinfo);
	}
	else
	{
		return ret;
	}

#ifdef __JFS_STORAGE__
	TRACE("diskinfo.IsWritting = %d\n",diskinfo.IsWritting);
	return diskinfo.IsWritting;
#endif
	printf("index = %d ret = %d\n", nIndex, ret);
	return ret;

}

/*@ brief 获取硬盘当前格式化的状态
 *@ param int nIndex 硬盘索引号
 *@ return  格式化状态:
 *@                  0: 未格式化;
 *@                  1: 格式化中
 *@                  2: 格式化成功
 *@                  3: 格式化失败
 *@                  4: 卸载失败
 *@                  5: 设备忙
 *@                  6: 无SD卡
 */
int CDiskManager::GetDiskFormatStatus(const unsigned int nIndex)
{
	if (nIndex >= nDiskNum)
	{
		DEBUG_MSG("%d超过硬盘个数%d:\n", nIndex, nDiskNum);
		return IDVR_ERROR_PARAMETER;
	}
	return ZW_PkgSd_GetSdFormatStatus();
}

//!\brief 获取当前硬盘写的状态
//!\param int nIndex 硬盘索引号
//!\return  当前硬盘写状态: 0.SD卡空闲, 1.SD卡正在写, 2.SD卡写完成, 3.SD卡写失败
int CDiskManager::GetDiskWriteStatus(const unsigned int nIndex)
{
	if (nIndex >= nDiskNum)
	{
		DEBUG_MSG("%d超过硬盘个数%d:\n", nIndex, nDiskNum);
		return IDVR_ERROR_PARAMETER;
	}

    int writeStatus = 0;

    ZW_PkgSd_GetDiskWriteStatus(&writeStatus);

	return writeStatus;
}

int CDiskManager::UnMount(const unsigned int nDiskIndex, const unsigned int nPartitionIndex)
{
	int ret;

	if (nDiskIndex >= nDiskNum)
	{
		DEBUG_MSG("%d超过硬盘个数%d:\n", nDiskIndex, nDiskNum);
		return IDVR_ERROR_PARAMETER;
	}

	ret = theCHardDisk[nDiskIndex].UnMount(nPartitionIndex);

	return ret;
}

//!\brief 挂载该分区
//!\param int nPartitionIndex 分区在硬盘中的索引值
//!\param int nDiskIndex 硬盘索引号 
//!\param int nDiskIndex 分区索引号
//!\return  成功为0，失败为-1
int CDiskManager::Mount(const unsigned int nIndex, const unsigned int nPartitionIndex)
{
	int ret = IDVR_ERROR_PARAMETER;

	if (nIndex >= nDiskNum)
	{
		DEBUG_MSG("%d超过硬盘个数%d:\n", nIndex, nDiskNum);
		return IDVR_ERROR_PARAMETER;
	}

	if (0 == nPartitionIndex)
	{							//系统保留分区，可以调用磁盘库挂载
		DEBUG_MSG("挂载分区:%d\n", nPartitionIndex);
		ret = theCHardDisk[nIndex].Mount(nPartitionIndex);
	}

	return ret;
}

bool CDiskManager::IsBusy(int nDiskIndex)
{
	tagDiskGroupInfo groupinfo;

	int i, j;

	tagDiskInfo diskinfo;

	//CDiskManager *theCDiskManager           = CDiskManager::GetInstance();
	CDiskGroupManager *theCDiskGroupManager = CDiskGroupManager::GetInstance();

	theCHardDisk[nDiskIndex].GetInfo(&diskinfo);

	for (i = 0; i < MAX_DISK_GROUP_NUM; i++)
	{							//获取盘组信息
		DEBUG_MSG("盘组%d\n", i);
		theCDiskGroupManager->GetInfo(i, &groupinfo);
		for (j = 0; j < groupinfo.Partitions; j++)
		{
			if (groupinfo.Partitions)
			{
				//if ( 0 == strcmp(groupinfo.tPartitions[j].acPartName, diskinfo.tPartitions[nIndexPartition].Name))
				{
					DEBUG_MSG("比较分区:%s-%s\n", groupinfo.tPartitions[j].acModelName, diskinfo.DeviceName);
					//DEBUG_MSG("strcmp(groupinfo.tPartitions[j].acModelName, diskinfo.DeviceName) = %d\n", strcmp(groupinfo.tPartitions[j].acModelName, diskinfo.DeviceName));
					//DEBUG_MSG("strcmp(groupinfo.tPartitions[j].acModelName = %d, diskinfo.DeviceName) = %d\n", strlen(groupinfo.tPartitions[j].acModelName),strlen(diskinfo.DeviceName));
					if (0 == strcmp(groupinfo.tPartitions[j].acModelName, diskinfo.DeviceName))
					{
						DEBUG_MSG("比较分区:%s-%s\n", groupinfo.tPartitions[j].acSerialNo, diskinfo.SerialNo);
						if (0 == strcmp(groupinfo.tPartitions[j].acSerialNo, diskinfo.SerialNo))
						{
							TRACE("盘组%d 硬盘[%s]忙\n", i, diskinfo.DeviceName);
							return true;
						}
					}
				}
			}					//end if (groupinfo.Partitions)
		}						//end for ( j = 0; j < groupinfo.Partitions; j++ )
	}							//end for ( i = 0; i < MAX_DISK_GROUP_NUM; i++ )   

	return false;
}

extern void rsleep(unsigned int ntime);

int CDiskManager::CreatePartition(const unsigned int nDiskIndex, const unsigned int nSize)
{
	int ret = IDVR_ERROR_PARAMETER;

	int add;

	tagDiskInfo DiskInfo;

	CDiskManager *theCDiskManager = CDiskManager::GetInstance();

	if (nDiskIndex >= nDiskNum)
	{
		DEBUG_MSG("%d超过硬盘个数%d:\n", nDiskIndex, nDiskNum);
		return IDVR_ERROR_PARAMETER;
	}

	if (IsBusy(nDiskIndex))
	{
		DEBUG_MSG("硬盘%d忙\n", nDiskIndex);
		return IDVR_STR_ERROR_DISK_UMOUNT_BUSY;
	}

	UpdateDiskData(-1);
	//卸载系统保留分区
	if (NULL != theCDiskManager)
	{
		ret = theCDiskManager->UnMount(nDiskIndex, 0);

		if (0 != ret)
		{
			DEBUG_MSG("硬盘%d卸载保留分区出错，无法创建分区\n", nDiskIndex);
			UpdateDiskData(1);
			return ret;
		}
	}

	//创建分区
	add = theCHardDisk[nDiskIndex].CreatePartiton(nSize);

	//挂载系统保留分区
	if (NULL != theCDiskManager)
	{
		ret = theCDiskManager->Mount(nDiskIndex, 0);

		if (0 != ret)
		{
			DEBUG_MSG("硬盘%d挂载保留分区出错\n", nDiskIndex);
			UpdateDiskData(1);
			return ret;
		}
	}

	Update();


	if (add != 0)
	{
		//创建分区不成功,记录创建分区失败的错误号
		ret = add;
	}
	else
	{
		theCDiskManager->GetInfo(nDiskIndex, &DiskInfo);
		theCDiskManager->FormatPartition(nDiskIndex, DiskInfo.Partitions - 1);
	}
	UpdateDiskData(1);
	return ret;
}

int CDiskManager::DestroyPartition(const unsigned int nDiskIndex, const unsigned int nIndexPartition)
{
	int ret;

	int i;

	if (nDiskIndex >= nDiskNum)
	{
		DEBUG_MSG("%d超过硬盘个数%d:\n", nDiskIndex, nDiskNum);
		return IDVR_ERROR_PARAMETER;
	}

	DEBUG_MSG("删除磁盘%d 分区%d\n", nDiskIndex, nIndexPartition);


	if (IsBusy(nDiskIndex))
	{
		DEBUG_MSG("硬盘%d忙\n", nDiskIndex);
		return IDVR_STR_ERROR_DISK_UMOUNT_BUSY;
	}

	UpdateDiskData(-1);
	if (NULL != theCDiskManager)
	{
		ret = theCDiskManager->UnMount(nDiskIndex, 0);


		if (0 != ret)
		{
			DEBUG_MSG("硬盘%d卸载保留分区出错，无法创建分区\n", nDiskIndex);
			UpdateDiskData(1);
			return ret;
		}
	}


	ret = theCHardDisk[nDiskIndex].DelPartiton(nIndexPartition);


	if (NULL != theCDiskManager)
	{
		i = theCDiskManager->Mount(nDiskIndex, 0);


		if (0 != i)
		{
			DEBUG_MSG("硬盘%d挂载保留分区出错\n", nDiskIndex);
			UpdateDiskData(1);
			return ret;
		}
	}

	if (0 == ret)
	{
		DEBUG_MSG("删除磁盘%d 分区%d成功\n", nDiskIndex, nIndexPartition);
	}


	Update();

	UpdateDiskData(1);
	return ret;
}

int CDiskManager::Update()
{
	int i;

	LOCK(mutex);
	if (theCHardDisk == NULL)
	{
//#ifndef TEST_MEM_INFO
		theCHardDisk = new CHardDisk[DISKMANGER_MAX_DISK_NUM];
//#else
//		theCHardDisk = CommonKit::TCreateObject<CHardDisk>::CreateObject(DISKMANGER_MAX_DISK_NUM, __FILE__, __FUNCTION__, __LINE__);
//#endif
	}

	if (theCHardDisk == NULL)
	{
		ERROR_MSG("初始化硬盘实例失败\n");
		UNLOCK(mutex);
		return IDVR_STR_ERROR_DISK_INIT_FAILED;
	}

	nDiskNum = 1;//(unsigned int) EDVR_HD_GetTotalDiskCount();

	TRACE("nDiskNum = %d\n", nDiskNum);

	for (i = 0; i < (int) nDiskNum; i++)
	{
		theCHardDisk[i].Index = i;
		theCHardDisk[i].UpdateDiskStatus();
	}

	for (i = nDiskNum; i < DISKMANGER_MAX_DISK_NUM; i++)
	{
		theCHardDisk[i].Index = -1;
		theCHardDisk[i].UpdateDiskStatus();
	}

	UNLOCK(mutex);
	return 0;
}


//!\brief 设置硬盘错误回调
//!\param int Type 硬盘错误类型
//!\param CBAlert callabck 回调指针
//!\return 成功为0，失败为错误码
int CDiskManager::SetDiskErrorCallBack(int AlarmType, CBAlert callabck)
{
	int ret;

	if (NULL == ADiskError)
	{
		return IDVR_ERROR_ALLOC;
	}

	ret = ADiskError->SetCallBack(AlarmType, callabck);

	return ret;
}

//!\brief 判断某个报警联动动作是否开启
//!\praram int AlarmType 报警类型。VIDEO_LOST 
//!\praram int Action    !      ACTION_SCREEN 屏幕显示
//  !      ACTION_BEEP 声音提示
//  !      ACTION_ALARMOUT 报警输出
//  !      ACTION_SENDTOSERVER 上传报警服务器
//  !      ACTION_EMAIL 联动Email
//  !      ACTION_SMS 联动短信
//!\return 成功为0，失败为错误码
int CDiskManager::EnableAction(int AlarmType, tagActionType Action, bool bEnable)
{
	int ret;

	if (NULL == ADiskError)
	{
		return IDVR_ERROR_ALLOC;
	}

	ret = ADiskError->EnableAction(AlarmType, Action, bEnable);

	return ret;
}

//!\brief 判断某个报警联动动作是否开启
//!\praram int AlarmType 报警类型。
//!\praram int Action   动作类型
//!\return false 为禁止联动 true 为开启联动
bool CDiskManager::IsEnableAction(int AlarmType, tagActionType Action)
{
	bool ret;

	if (NULL == ADiskError)
	{
		return false;
	}

	ret = ADiskError->IsEnableAction(AlarmType, Action);

	return ret;
}

//!\brief 设置某个联动动作涉及的通道。无通道概念的不需要设置
//!\param int nCh 通道
//!\praram int AlarmType 报警类型
//!\praram tagActionType Action   动作类型
//!\praram int AlarmCh 联动动作涉及到通道。
//!\praram bool bEnable 启用/不启用该通道的配置
//!\praram int dwReserved 保留字。对于预置点、轨迹、巡航线路输入上述编号
//!\return  成功为0，失败为-1
int CDiskManager::SetActionChParam(int AlarmType, tagActionType Action, int AlarmCh, bool bEnable, int nReserved)
{
	int ret;

	if (NULL == ADiskError)
	{
		return IDVR_ERROR_ALLOC;
	}

	ret = ADiskError->SetActionChParam(AlarmType, Action, AlarmCh, bEnable, nReserved);

	return ret;
}



//!\brief 获取某个联动动作涉及的通道或者编号
//!\praram int AlarmType 报警类型
//!\praram tagActionType Action 动作类型
//!\praram bool *szCh 通道数组，true为联动了该通道，false为没有联动该通道
//!\praram int *szReserved 对应通道涉及到的子类型数据。目前仅相对于预置点编号等
//!\return  成功为0，失败为-1
int CDiskManager::GetActionChParam(int AlarmType, tagActionType Action, bool * szCh, int *szReserved)
{
	bool ret;

	if (NULL == ADiskError)
	{
		return false;
	}

	ret = ADiskError->GetActionChParam(AlarmType, Action, szCh, szReserved);

	return ret;
}

//!\brief 向上报警
//!\praram int DiskErrorType 硬盘的错误类型
//!\return  成功为0，失败为-1
int CDiskManager::PushAlarm(void *org)
{
	if (NULL == ADiskError)
	{
		return false;
	}

	ADiskError->PushAlarm(org);
	return 0;
}

//!\brief 初始化一个硬盘
//!\param int Type 硬盘错误类型
//!\param CBAlert callabck 回调指针
//!\return 成功为0，失败为错误码
int CDiskManager::InitDisk(char *dev)
{
	int ret;

	DiskDeviceInfoEx info;
	memset(&info, 0, sizeof(DiskDeviceInfoEx));

	CDiskManager *theCDiskManager = CDiskManager::GetInstance();

	TRACE("获取磁盘状态\n");

	ret = EDVR_HD_IsNewDiskEx(dev);

	TRACE("硬盘%s为%s\n", dev, (ret == 0) ? "老硬盘" : "新硬盘");

	if (0 != ret)
	{
#ifndef USE_ICAMERA
		if (NULL != theCDiskManager->DiskCallBack)
		{
			TRACE("由上层初始化\n");

			BusNo = EDVR_HD_GetBusNumberEx(dev);

			if (0 != theCDiskManager->DiskCallBack(BusNo))
			{
				TRACE("上层初始化失败\n");

				EDVR_HD_RemoveDiskCallBack(dev);

				ret = EDVR_SATA_DeviceRemove(BusNo);

				if (-1 == ret)
				{
					TRACE("卸载硬盘%d出错", BusNo);
					return 0;
				}

				Update();
				return -1;
			}
		}
		else
#endif
		{
			//缺省初始化过程

			TRACE("初始化磁盘%s\n", dev);
			EDVR_HD_Init(dev);

#ifdef USE_ICAMERA
			rsleep(1000000);
#else
			rsleep(5000000);
#endif

			TRACE("dev = %s\n", dev);
			EDVR_HD_GetDiskInfoExFromDev(dev, &info);
			TRACE("partition num is %d name is %s\n",info.PartitionsNum,info.Partitions[0].DevName);

			if (info.PartitionsNum > 0)
			{
				EDVR_HD_FormatPartition(info.Partitions[0].DevName);
			}
		}
	}
	else
	{
		EDVR_HD_GetDiskInfoExFromDev(dev, &info);
	}

#ifdef __JFS_STORAGE__
	TRACE("挂载分区%s\n", info.Partitions[0].DevName);
	DEBUG_MSG("my add: PartitionsNum=%d\n", info.PartitionsNum);
	if (info.PartitionsNum > 0)
	{
		EDVR_HD_MountPartition(info.Partitions[0].DevName);
	}

#ifdef __JFS_STORAGE__
	for (int i = 1; i < info.PartitionsNum; i++)
	{
		TRACE("挂载分区%s\n", info.Partitions[i].DevName);
		EDVR_HD_MountPartition(info.Partitions[i].DevName);
	}
#endif
#endif

	Update();

#ifdef __JFS_STORAGE__
	Macro_HotplugDiskAdd(info.Dev);
#endif

	if (NULL != theCDiskManager->DiskStatusCallBack)
	{
		theCDiskManager->DiskStatusCallBack(info.Dev, true);
	}

#ifdef __JFS_STORAGE__
//  Macro_HotplugDiskAdd(info.Dev);
#endif

	TRACE("完毕\n");
	return 0;
}

typedef struct __THREAD_CREATE_SEM__
{
	sem_t sem;
	void *nParam;
} THREAD_CREATE_SEM, *pTHREAD_CREATE_SEM;

void *STR_NewDisk(HotplugInfo hpInfo)
{
	CDiskManager *theCDiskManager = CDiskManager::GetInstance();

	CDPManager *theCDPManager = CDPManager::GetInstance();

	CDiskGroupManager *theCDiskGroupManager = CDiskGroupManager::GetInstance();


	printf("插入一个磁盘%s\n", hpInfo.DeviceName);

	DEBUG_MSG("run here\n");
	theCDiskManager->InitDisk(hpInfo.DeviceName);

	DEBUG_MSG("run here\n");
	Macro_HotplugDiskAdd(hpInfo.DeviceName);

	DEBUG_MSG("run here\n");
	theCDiskManager->UpdateDiskData(1);

	DEBUG_MSG("run here\n");
	if (NULL != theCDPManager)
	{
		theCDPManager->Capture();
	}

	DEBUG_MSG("run here\n");
	if (NULL != theCDiskGroupManager)
	{	//更新盘组信息
		theCDiskGroupManager->Update();
	}

	DEBUG_MSG("run here\n");
	if (NULL != theCDPManager)
	{
		theCDPManager->Release();
	}
	printf("插入一个磁盘%s完成\n", hpInfo.DeviceName);
	return NULL;
}

void *STR_DelDisk(HotplugInfo hpInfo)
{

	CDiskManager *theCDiskManager = CDiskManager::GetInstance();

	CDPManager *theCDPManager = CDPManager::GetInstance();

	CDiskGroupManager *theCDiskGroupManager = CDiskGroupManager::GetInstance();

	printf("拔下一个硬盘%s\n", hpInfo.DeviceName);
	EDVR_HD_RemoveDiskCallBack(hpInfo.DeviceName);

	DEBUG_MSG("run here\n");
	Macro_HotplugDiskRemove(hpInfo.DeviceName);


	DEBUG_MSG("run here\n");

	if (NULL != theCDPManager)
	{
		theCDPManager->Capture();
	}

	DEBUG_MSG("run here\n");
	if (NULL != theCDiskManager)
	{							//更新磁盘信息
		theCDiskManager->Update();
	}

	DEBUG_MSG("run here\n");
	if (NULL != theCDiskGroupManager)
	{							//更新盘组信息
		theCDiskGroupManager->Update();
	}

	DEBUG_MSG("run here\n");
	if (NULL != theCDiskManager->DiskStatusCallBack)
	{
		theCDiskManager->DiskStatusCallBack(hpInfo.DeviceName, false);
	}

	DEBUG_MSG("run here\n");
	if (NULL != theCDPManager)
	{
		theCDPManager->Release();
	}
	DEBUG_MSG("run here\n");
	printf("拔下一个硬盘%s完成\n", hpInfo.DeviceName);
	return NULL;

}

static char m_adddev[30];

static char m_deldev[30];

static int bCheckHotplug = 1;

static sem_t bsem_add;
static sem_t bsem_del;

static void *CheckAddHotplug(void *org)
{
    PRT_TID();

	BEGINRUN_THREAD;
	while (bCheckHotplug)
	{	
		if (0 == sem_wait(&bsem_add))
		{
			HotplugInfo hpinfo;
			snprintf(hpinfo.DeviceName, sizeof(hpinfo.DeviceName), "%s", m_adddev);
			STR_NewDisk(hpinfo);
		}
		else
		{
			perror("sem_wait error\n");
		}
		rsleep(1000000);
	}

	ENDRUN_THREAD;
	return NULL;
}

static void *CheckDelHotplug(void *org)
{
    PRT_TID();

	BEGINRUN_THREAD;
	while (bCheckHotplug)
	{
		if (0 == sem_wait(&bsem_del))
		{
			HotplugInfo hpinfo;
			snprintf(hpinfo.DeviceName, sizeof(hpinfo.DeviceName), "%s", m_deldev);
			STR_DelDisk(hpinfo);
		}
		else
		{
			perror("sem_wait error\n");
		}
		rsleep(1000000);
	}

	ENDRUN_THREAD;
	return NULL;
}

static int StartCheckHotplug()
{
	int nRet;

	pthread_t thread_t;

	pthread_attr_t new_attr;

	pthread_attr_init(&new_attr);
	pthread_attr_setdetachstate(&new_attr, PTHREAD_CREATE_DETACHED);
	sem_init(&bsem_add, 0, 0);
	sem_init(&bsem_del, 0, 0);
	nRet = pthread_create(&thread_t, &new_attr, CheckAddHotplug, NULL);
	if (nRet != 0)
	{
		printf("create mount disk thread error.....\n");
		return -1;
	}

	nRet = pthread_create(&thread_t, &new_attr, CheckDelHotplug, NULL);
	if (nRet != 0)
	{
		printf("create mount disk thread error.....\n");
		return -1;
	}

	return 0;
}

#if 0
static int StopCheckHotplug()
{
	bCheckHotplug = 0;
	sem_wait(&bsem);
	sem_destroy(&bsem);
	printf("子进程读取管道结束\n");
	return 0;
}
#endif

//!\brief 初始化硬盘
//!\param int nBusNo 硬盘物理位置编号
//!\return  成功为0，失败为-1
int CDiskManager::InitDisk(const unsigned int nBusNo)
{
	unsigned int nDiskIndex = 0, i;

	DiskDeviceInfoEx info;

	char Dev[100];

	Update();
	LOCK(mutex);
	for (i = 0; i < nDiskNum; i++)
	{
		if (theCHardDisk[i].GetBusNumber() == (int) nBusNo)
		{
			nDiskIndex = i;

			TRACE("nDiskIndex = %d,i = %d\n", nDiskIndex, i);
			break;
		}
	}

	if (i >= nDiskNum)
	{
		TRACE("nDiskIndex = %d,i = %d\n", nDiskIndex, i);
		TRACE("没有找到该硬盘%d\n", nBusNo);
		UNLOCK(mutex);
		return -1;
	}

	if (theCHardDisk[nDiskIndex].GetBusNumber() != (int) nBusNo)
	{
		TRACE("该硬盘%d可能已经不存在\n", nBusNo);
		UNLOCK(mutex);
		return -1;
	}

	theCHardDisk[nDiskIndex].GetDev(Dev, sizeof(Dev));

	//防止盘组内已经含有该分区，删除
	Macro_HotplugDiskRemove(Dev);

	TRACE("初始化磁盘%s\n", Dev);

	EDVR_HD_Init(Dev);

#ifdef USE_ICAMERA
	rsleep(1000000);
#else
	rsleep(5000000);
#endif

	TRACE("获取磁盘信息%s\n", Dev);
	EDVR_HD_GetDiskInfoExFromDev(Dev, &info);

	if (info.PartitionsNum > 0)
	{
		EDVR_HD_FormatPartition(info.Partitions[0].DevName);
	}

	UNLOCK(mutex);


	TRACE("挂载分区%s\n", info.Partitions[0].DevName);
	if (info.PartitionsNum > 0)
	{
		EDVR_HD_MountPartition(info.Partitions[0].DevName);
	}

	Update();

#ifdef __JFS_STORAGE__
	Macro_HotplugDiskAdd(info.Dev);
#endif
	if (NULL != DiskStatusCallBack)
	{
		DiskStatusCallBack(info.Dev, true);
	}

	return 0;

}

#ifdef USE_ICAMERA
static void SD_AddCallBack(const char *dev)
{
	printf("1 插入一个磁盘%s \n", dev);

	memset(m_adddev, 0, sizeof (m_adddev));
	strncpy(m_adddev, dev, sizeof (m_adddev) - 1);
	while( 0 != sem_post(&bsem_add));
}
#else
void Sata_AddCallBack(HotplugInfo hpInfo)
{
	int BusNo;

	if (hpInfo.DevType != 3)
		return;

	printf("插入一个磁盘%s 写入管道\n", hpInfo.DeviceName);

	if (m_addpipe[1] != 0)
	{
		write(m_addpipe[1], hpInfo.DeviceName, sizeof (hpInfo.DeviceName));
	}
	else
	{
		printf("[%s %d]创建管道失败，无法实现热插拔功能，请重启设备\n", __FILE__, __LINE__);

		EDVR_HD_RemoveDiskCallBack(hpInfo.DeviceName);

		BusNo = EDVR_HD_GetBusNumberEx(hpInfo.DeviceName);
		EDVR_SATA_DeviceRemove(BusNo);
	}
}
#endif

#ifdef USE_ICAMERA
void SD_RemoveCallBack(const char *dev)
{
	printf("1 拔下一个磁盘%s 写入管道\n", dev);

	memset(m_deldev, 0, sizeof (m_deldev));
	strncpy(m_deldev, dev, sizeof (m_deldev) - 1);
	while( 0 != sem_post(&bsem_del));
}
#else
void Sata_RemoveCallBack(HotplugInfo hpInfo)
{
	int BusNo;

	//Only sata disk support hotplug
	if (hpInfo.DevType != 3)
		return;

	printf("拔下一个磁盘%s 写入管道\n", hpInfo.DeviceName);

	if (m_delpipe[1] != 0)
	{
		write(m_delpipe[1], hpInfo.DeviceName, sizeof (hpInfo.DeviceName));
	}
	else
	{
		printf("[%s %d]创建管道失败，无法实现热插拔功能，请重启设备\n", __FILE__, __LINE__);
		EDVR_HD_RemoveDiskCallBack(hpInfo.DeviceName);

		BusNo = EDVR_HD_GetBusNumberEx(hpInfo.DeviceName);
		EDVR_SATA_DeviceRemove(BusNo);
	}
}
#endif

int CDiskManager::Init()
{
	int ret;

	int i, num;

	DiskDeviceInfoEx info[16];

	if (1 == bWorkTrheadRun)
	{
		DEBUG_MSG("已经初始过\n");
		return 0;
	}

	if (NULL != ADiskError)
	{
		ADiskError->Init("disk_error");
		ADiskError->ReadCfg();
	}

#ifdef __JFS_STORAGE__
	CRecordFile::Init(NULL);
#endif

	TRACE("********************初始化磁盘管理器********************\n");

#ifdef USE_ICAMERA
	EDVR_HD_DeviceAddEvent(SD_AddCallBack);
	EDVR_HD_DeviceRemoveEvent(SD_RemoveCallBack);
	//EDVR_HD_SetEventType((DiskActType) (DISKACT_SDCARD | DISKACT_USB));
#else
	EDVR_USB_SetEventType(ACT_SATA);
	EDVR_USB_DeviceRemoveEvent(Sata_RemoveCallBack);
	EDVR_USB_DeviceAddEvent(Sata_AddCallBack);
#endif

	StartCheckHotplug();

	ret = Update();

	DEBUG_MSG("共有硬盘%d\n", nDiskNum);

	num = nDiskNum;

	for (i = 0; i < (int) num; i++)
	{
		EDVR_HD_GetDiskInfoEx(i, &info[i]);
	}

	for (i = 0; i < (int) num; i++)
	{
		TRACE("初始化磁盘：%d\n", i);
		InitDisk(info[i].Dev);
	}

	UpdateDiskData(0);

	sem_init(&cond, 0, 0);
	if (ret == 0)
	{
		ret = StartWorkThread();
	}

#ifdef USE_SD_PKG

    if(!access(CUSTOM_CONFIG, 0))
    {
        //获取新的配置文件读取实例
        CConfig *pCfg = CConfig::NewInstance();
        int nRet = pCfg->LoadFileRO(CUSTOM_CONFIG);

		if (0 == nRet)
		{
			bSupportSDCard = pCfg->GetValue("SDCARD", "bSupportSDCard", 0);
		}		
		CConfig::ReleaseInstance(pCfg);
    }

    /* SD卡初始化 */
    if (bSupportSDCard)
    {
		// 根据版型计算环形缓冲区和最大帧大小
		ZW_PKG_PARAM_T pkgParam;
		pkgParam.maxFrameBufSize = 512*1024;
		pkgParam.maxRingBufSize = 1*1024*1024;
		GetRecordBufSizeByBoardID(&pkgParam.maxRingBufSize, &pkgParam.maxFrameBufSize);
		printf("RingBufSize: %d MaxFrameSize: %d\n", pkgParam.maxRingBufSize, pkgParam.maxFrameBufSize);
	    ret = ZW_PkgSD_Init(&pkgParam);
    }

#endif

	TRACE("********************完成初始化磁盘管理器********************\n");

	return (int) ret;
}


CDiskManager *CDiskManager::theCDiskManager = NULL;

int CDiskManager::UnInit()
{
	if (bWorkTrheadRun == 0)
	{
		return -1;
	}
	bWorkTrheadRun = 0;
	sem_wait(&cond);
	sem_destroy(&cond);
	if (NULL != ADiskError)
	{
#ifndef TEST_MEM_INFO
		delete ADiskError;
#else
		CommonKit::TDestroyObject<CAlarm>::DestroyObject(ADiskError);
#endif

		ADiskError = NULL;
	}
	return (int) 0;
}

CDiskManager *CDiskManager::GetInstance()
{
	if (NULL == theCDiskManager)
	{
#ifndef TEST_MEM_INFO
		theCDiskManager = new CDiskManager;
#else
		theCDiskManager = CommonKit::TCreateObject<CDiskManager>::CreateObject(__FILE__, __FUNCTION__, __LINE__);
#endif

		if (NULL == theCDiskManager)
		{
			DEBUG_MSG("获取管理器实例失败\n");
			return NULL;
		}

		DEBUG_MSG(_DISK_VERSION__);

#ifndef TEST_MEM_INFO
		theCDiskManager->ADiskError = new CAlarm();
#else
		theCDiskManager->ADiskError = CommonKit::TCreateObject<CAlarm>::CreateObject(__FILE__, __FUNCTION__, __LINE__);
#endif

		//theCDiskManager->Init();
	}
	return (CDiskManager *) theCDiskManager;
}

int CDiskManager::ReleaseInstance()
{
	if (NULL != theCDiskManager)
	{
		theCDiskManager->UnInit();
#ifndef TEST_MEM_INFO
		delete theCDiskManager;
#else
		CommonKit::TDestroyObject<CDiskManager>::DestroyObject(theCDiskManager);
#endif

		theCDiskManager = NULL;
	}
	return (int) 0;
}

static int g_nDiskNumSum = 0;	//应该存在的应该个数。为插入的硬盘数- 卸载的硬盘数，

static int g_nErrorDiskNum = 0;	//可以生用的硬盘个数

static int g_nDiskNum = 0;		//可以获取的硬盘个数

/**************************************************************************
* 函数名称： int UpdateDiskData(int bAdd)
* 功能描述： 更新磁盘的数据
* 输入参数： 1:总磁盘数+1,0:总磁盘数自动获取,-1:总磁盘数-1
* 输出参数： 无
* 返 回 值：     成功为0，失败为-1
* 其它说明： 无
* 修改日期    版本号     修改人	     修改内容
* -----------------------------------------------
* 02/08/01	     V1.0	    XXXX	      XXXX
**************************************************************************/
int CDiskManager::UpdateDiskData(int bAdd)
{
	pthread_mutex_lock(&diskmutex);
	g_nDiskNum = EDVR_HD_GetTotalDiskCount();

	if (bAdd == 0)
	{
		g_nDiskNumSum = g_nDiskNum;
	}
	else
	{
		g_nDiskNumSum += bAdd;
	}

	printf("UpdateDiskData:g_nDiskNumSum = %d,g_nDiskNum = %d,g_nErrorDiskNum = %d\n", g_nDiskNumSum, g_nDiskNum, g_nErrorDiskNum);
	pthread_mutex_unlock(&diskmutex);
	return 0;
}


/**************************************************************************
* 函数名称： int GetErrorDiskNum()
* 功能描述： 获取异常硬盘的个数
* 输入参数： 无
* 输出参数： 无
* 返 回 值：     异常硬盘的个数
* 其它说明： 无
* 修改日期    版本号     修改人	     修改内容
* -----------------------------------------------
* 02/08/01	     V1.0	    XXXX	      XXXX
**************************************************************************/
int CDiskManager::GetErrorDiskNum(int Disks)
{
	int Num = 0;

	int i;

	unsigned long DiskSize = 0;

	for (i = 0; i < Disks; i++)
	{
		DiskSize = theCHardDisk[i].UpdateDiskStatus();

		if (DiskSize == 2)
		{						//2所有分区可用容量和为0，表示该硬盘异常
			Num++;
		}
	}

	return Num;
}

//暂停SD卡录像
int CDiskManager::PauseSDRecord()
{
    return ZW_PkgSD_Pause();
}

//恢复SD卡录像
int CDiskManager::ResumeSDRecord()
{    
    return ZW_PkgSD_Resume();
}

int CDiskManager::Detect()
{
	return (int) 0;
}

int CDiskManager::SetNewDiskCallBack(NEWDISKCALLBACK callback)
{
	DiskCallBack = callback;
	return 0;
}

CDiskManager::CDiskManager()
{
	theCHardDisk = NULL;
	DiskCallBack = NULL;
	nDiskNum = 0;
	bWorkTrheadRun = 0;
	DiskStatusCallBack = NULL;

	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_init(&diskmutex, NULL);
}

CDiskManager::~CDiskManager()
{
}

#ifndef USE_SD_PKG
/*!
 * \brief
 * 检测硬盘上是否有打开的文件，以便决定是否让其休眠.
 * 
 * \param dev
 * 硬盘设备节点名.
 * 
 * \returns
 * 不存在打开的文件返回1，否则返回非0.
 */
static int nofileopen(const char *dev)
{
	pid_t pid_dvr;

	char fdpath[120];

	char mountpoint[40];

	DIR *dp;

	struct dirent *entry;

	struct stat statbuf;

	int ret;

	pid_dvr = getpid();
	snprintf(fdpath, sizeof (fdpath), "/proc/%d/fd", (int) pid_dvr);

	if ((dp = opendir(fdpath)) == NULL)
	{
		TRACE("can't open directory: %s\n", fdpath);
		return 0;
	}

	EDVR_HD_GetMountPoint(dev, mountpoint, sizeof (mountpoint));

	ret = 1;
	while ((entry = readdir(dp)) != NULL)
	{
		char procfile[120];

		snprintf(procfile, sizeof (procfile), "%s/%s", fdpath, entry->d_name);

		lstat(procfile, &statbuf);

		if (S_ISLNK(statbuf.st_mode))
		{
			char name[256];

			unsigned int len;

			len = readlink(procfile, name, sizeof (name) - 1);
			if (len >= 0)
			{
				if (len > (sizeof (name) - 1))
				{
					name[sizeof (name) - 1] = '\0';
				}
				else
				{
					name[len] = '\0';
				}

				if (name == strstr(name, mountpoint))
				{
					DEBUG_MSG("find file %s -> %s\n", procfile, name);

					ret = 0;
					break;
				}

				if (name == strstr(name, dev))
				{
					DEBUG_MSG("find file %s -> %s\n", procfile, name);

					ret = 0;
					break;
				}
			}
		}
	}

	closedir(dp);
	return ret;
}

/*!
* \brief
* 使SATA硬盘休眠的线程函数，每10分钟运行一次.
*
* \returns
* void *.
*/
static void *sd_standby(void *org)
{
    PRT_TID();

	struct timespec ts;

	ts.tv_sec = 1;
	ts.tv_nsec = 0;
	BEGINRUN_THREAD;

	while (1)
	{
		int i;

		ts.tv_sec = 60 * 20;
		ts.tv_nsec = 0;
		nanosleep(&ts, NULL);

		for (i = 0; i < 16; i++)
		{
			char dev[50];

			if (0 == EDVR_HD_Index2DevNode(i, dev, sizeof (dev)))
			{
				if (DISKACT_SATA == EDVR_HD_GetDiskType(dev))
				{
					if (nofileopen(dev))
					{
						char cmd[100];

						memset(cmd, 0, sizeof (cmd));

						snprintf(cmd, sizeof (cmd), "/usr/sbin/sdparm -C stop %s", dev);

						mysystem(cmd);
					}
				}
			}
		}
	}

	ENDRUN_THREAD;
	return NULL;
}
#endif

int CDiskManager::StartWorkThread()
{
	int status;

	pthread_attr_t new_attr;

	pthread_t thread_t;

	pthread_attr_init(&new_attr);
	pthread_attr_setdetachstate(&new_attr, PTHREAD_CREATE_DETACHED);

	if (bWorkTrheadRun == 0)
	{
		bWorkTrheadRun = 1;
		status = pthread_create(&thread_t, &new_attr, CDiskManager::work_thread, NULL);

		if (status != 0)
		{
			TRACE("创建工作线程失败\n");
			return IDVR_STR_ERROR_DISK_THREAD_FAILED;
		}
#ifndef USE_SD_PKG
		status = pthread_create(&thread_t, &new_attr, sd_standby, NULL);

		if (status != 0)
		{
			TRACE("创建工作线程失败\n");
			return IDVR_STR_ERROR_DISK_THREAD_FAILED;
		}
#endif
	}
	return 0;
}


void *CDiskManager::work_thread(void *org)
{
    PRT_TID();

	struct timespec ts;

	ts.tv_sec = 1;
	ts.tv_nsec = 0;
	int nSleepSum = 0;

	CDiskManager *handle = CDiskManager::GetInstance();

	BEGINRUN_THREAD;
	while (handle->bWorkTrheadRun)
	{
		//CDiskManager::GetInstance()
		if (NULL != handle)
		{
			handle->Detect();
		}
		DEBUG_MSG("磁盘检测线程正在运行\n");

		nSleepSum = 0;

		//下面的睡眠方式，防止线程退出过慢的问题
		while ((nSleepSum++) < 60 /*需要扩大 */ )
		{
			if (handle->bWorkTrheadRun)
			{
				ts.tv_sec = 1;
				ts.tv_nsec = 0;
				nanosleep(&ts, NULL);
			}
			else
			{
				nSleepSum = 0;
				break;
			}
		}
	}
	sem_post(&cond);
	ENDRUN_THREAD;
	DEBUG_MSG("硬盘异常检测线程退出\n");
	return NULL;
}
#endif
