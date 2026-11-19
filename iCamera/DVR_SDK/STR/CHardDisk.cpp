// Copyright (C) 1991 - 1999 Rational Software Corporation

/*!
*\file   CHardDisk.cpp 
*\brief  硬盘实现
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/
//#include <iostream.h>
#include "global_macro.h"
#include "CHardDisk.h"
#include "debug.h"
#include "idvr_errno.h"
#include "tagDiskInfo.h"
#include "CDiskGroup.h"
#include "str_fs.h"
// #include "sys_banned.h"
#include "global_gather.h"
extern "C"
{
#include "disk.h"
#include "pkg_interface.h"
}
int CHardDisk::tagmemcpy(void *src, tagDiskInfo * temp)
{
	unsigned int i;

	DiskDeviceInfoEx *info;

	info = (DiskDeviceInfoEx *) src;
	snprintf(temp->DeviceName, sizeof(temp->DeviceName), "%s", info->DeviceName);
	snprintf(temp->SerialNo, sizeof(temp->SerialNo), "%s", info->SerialNo);
	temp->IsWritting = info->IsWritting;
	temp->Partitions = info->PartitionsNum;
	//cout<<temp->Partitions<<endl;
	temp->Total_space = info->Total_disk_space;
	for (i = 0; i < temp->Partitions; i++)
	{
		temp->tPartitions[i].bMount = info->Partitions[i].Partition_has_mounted;
		temp->tPartitions[i].Available_space = info->Partitions[i].Available_partitions_space;

		if (strlen(info->Partitions[i].DevName) == 0)
		{
			DEBUG_MSG("磁盘%s获取第%d个分区名称失败\n", info->DeviceName, i);
			return -1;
		}

		snprintf(temp->tPartitions[i].Name, sizeof(temp->tPartitions[i].Name), "%s", info->Partitions[i].DevName);
		snprintf(temp->tPartitions[i].szMountPoint, sizeof(temp->tPartitions[i].szMountPoint), "%s", info->Partitions[i].Mount_point);
		temp->tPartitions[i].Total_space = info->Partitions[i].Total_partitions_space;
		temp->tPartitions[i].dwStartCHS = info->Partitions[i].dwStartCHS;
		if (i == 0)
		{
#ifdef __PKGSTREAM_STORTAGE__
			temp->tPartitions[i].dwEndtCHS = info->Partitions[i].dwEndtCHS + 16065;
			temp->tPartitions[i].Total_space = info->Partitions[i].Total_partitions_space + (16065 / 2 / 1024);
#elif defined(__JFS_STORAGE__)
			temp->tPartitions[i].dwEndtCHS = info->Partitions[i].dwEndtCHS;
			temp->tPartitions[i].Total_space = info->Partitions[i].Total_partitions_space;
#endif
		}
		else
		{
			temp->tPartitions[i].dwEndtCHS = info->Partitions[i].dwEndtCHS;
		}

	}
	//cout<<temp->Partitions<<endl;
	return 0;
}

//!\brief 获取硬盘物理位置的编号
//!\param 无
//!\return  返回编号
int CHardDisk::GetBusNumber()
{
	int ret;

	ret = EDVR_HD_GetBusNumber(Index);
	return ret;
}

//!\brief 判断硬盘是否为新硬盘
//!\param 无
//!\return  新硬盘为1，否则不是新硬盘
int CHardDisk::IsNewDisk()
{
	int ret;

	ret = EDVR_HD_IsNewDiskEx(diskinfo.Dev);
	return ret;
}

//!\brief 获取磁盘信息
//!\param tagDiskInfo *Info 硬盘信息结构体指针
//!\return  成功为0，失败为-1
int CHardDisk::GetInfo(tagDiskInfo * Info)
{
	int ret = 0;

	Update();
	ret = tagmemcpy((void *) &diskinfo, Info);
	print();
	return ret;
}

int CHardDisk::GetLastInfo(tagDiskInfo * Info)
{
	int ret = 0;

	ret = tagmemcpy((void *) &diskinfo, Info);

	return ret;
}

/*@brief UpdateDiskStatus 更新硬盘当前的状态
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
int CHardDisk::UpdateDiskStatus()
{
	int ret = 0;

    int read_write = 0;
    int disk_status = 0;
    int format_status = 0;
    unsigned long total_space = 0;
    unsigned long available_space = 0;

    ret = ZW_PkgSd_GetDiskInfo(&disk_status, &total_space, &available_space);
    if (ret)
    {
        printf("Fail to called ZW_PkgSd_GetDiskInfo\n");
        m_status = 11;
        return 11;
    }

    /*
        NoSdCard         = 0, // 无SD卡(启动检测)
        SdCardErr        = 1, // SD卡错误(启动/运行检测)
        SdCardReadOnly   = 2, // SD卡只读(启动/运行检测)
        SdCardReadWrite  = 3, // SD卡读写(启动/运行检测)
        SdCardFatfsErr   = 4, // SD卡FATFS错误(运行检测)
        SdCardMountErr   = 5, // SD卡挂载错误(启动检测)
        SdCardAddPartErr = 6, // SD卡添加分区错误(启动检测)
    */
    switch(disk_status)
    {
        case 0: m_status = 9;  break;  // 无卡
        case 1: m_status = 11; break;  // 需要换卡
        case 2: m_status = 10; break;  // 需要格式化
        case 3: m_status =  8; break;  // SD卡读写
        case 4: m_status = 10; break;  // 需要格式化
        case 5: m_status = 11; break;  // 需要换卡
        case 6: m_status = 11; break;  // 需要换卡
        default: m_status = 9; break;  // 无卡
    }
    printf("disk_status = %d, m_status = %d\n", disk_status, m_status);

    if (8 == m_status)
    {
        ZW_PkgSd_GetDiskWriteStatus(&read_write);
        if (1 == read_write)
        {
            m_status = 1;
        }
    }

    /*
        UnFormatted  = 0, // 未格式化
        Formatting   = 1, // 格式化中
        FormatOk     = 2, // 格式化成功
        FormatErr    = 3, // 格式化失败
        UmountFail   = 4, // 卸载失败
        DeviceIsBusy  = 5, // 设备忙
        NoDiskFound  = 6, // 无SD卡
    */
    format_status = ZW_PkgSd_GetSdFormatStatus();
    if (2 != format_status)
    {
        switch(format_status)
        {
            case 1:  m_status = 6;  break;
            case 3:  m_status = 11; break;
            case 4:  m_status = 12; break;
            case 5:  m_status = 12; break;
            case 6:  m_status = 9;  break;
        }
        return m_status;
    }

    printf("format_status = %d, m_status = %d\n", format_status, m_status);

	Update();

	return ret;
}

/*@brief GetDiskStatus 获取磁盘状态
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
int CHardDisk::GetDiskStatus()
{
	return m_status;
}

void CHardDisk::print()
{
#if 0
	int i;

	printf("\n***************硬盘%u的信息如下*****************\n", Index + 1);
	printf("硬盘名称:\t%s\n", diskinfo.DeviceName);
	printf("硬盘序列号:\t%s\n", diskinfo.SerialNo);
	printf("硬盘总容量:\t%lu\n", diskinfo.Total_disk_space);
	printf("分区个数:\t%u\n", diskinfo.PartitionsNum);
	printf("是否正在写入:\t%u\n", diskinfo.IsWritting);
	for (i = 0; i < diskinfo.PartitionsNum; i++)
	{
		printf("分区名称:\t%s\n", diskinfo.Partitions[i].DevName);
		printf("分区分布:\t%lu - %lu\n", diskinfo.Partitions[i].dwStartCHS, diskinfo.Partitions[i].dwEndtCHS);
		printf("分区总容量:\t%lu\n", diskinfo.Partitions[i].Total_partitions_space);
		printf("分区剩余容量:\t%lu\n", diskinfo.Partitions[i].Available_partitions_space);

		if (diskinfo.Partitions[i].Partition_has_mounted)
		{
			printf("分区挂载点:\t%s\n", diskinfo.Partitions[i].Mount_point);
		}
	}
	printf("\n***************上面是硬盘%d的信息*****************\n", Index + 1);
#endif
}

//!\brief 获取硬盘的节点
//!\param char *Dev 节点号(输出)
//!\return  成功为0，失败为-1
//##ModelId=49AF8124014A
int CHardDisk::GetDev(char *Dev, int strlen)
{
	Update();
	snprintf(Dev, strlen, "%s", diskinfo.Dev);
	return 0;
}

int CHardDisk::Update()
{
	if (Index >= 8)
	{
		return -1;
	}

	if(6 == m_status)
	{
		printf("--CHardDisk::Update--disk is formating!--\n");
		return -1;
	}
	
	int ret = EDVR_HD_GetDiskInfoEx(Index, &diskinfo);
    //printf("my add: diskinfo.IsWritting = %d, PartitionsNum = %d, Partitions[0].Available_partitions_space=%ld, ret = %d\n", diskinfo.IsWritting, diskinfo.PartitionsNum,diskinfo.Partitions[0].Available_partitions_space, ret);
	if (ret == -2)
	{
		EDVR_HD_UMountPartition(diskinfo.Partitions[0].Mount_point);
	}

	return ret;
}

//!\brief 格式化分区
//!\param int nPartitionIndex 分区索引值
//!\return  成功为0，失败返回错误值
int CHardDisk::Format(int nPartitionIndex)
{
	int ret = 0;
	int lastStatus = m_status;

	printf("***********set Format status! m_status = %d\n", m_status);
	if(6 == m_status)
	{
		printf("###### disk is formating!\n");
		return -1;
	}
	diskformating = 1;
	m_status = 6;
	Update();

	printf("Format 2\n");
	ret = ZW_PkgSd_DiskPartFormat(diskinfo.Partitions[nPartitionIndex].DevName);
	if (ret == -1)
	{							//分区未被卸载，无法格式化
		printf("###### disk format error!\n");
		ret = IDVR_STR_ERROR_PARTITION_NOUNMOUNT;
	}
	diskformating = 0;
	m_status = lastStatus;

	return ret;
}


//!\brief 挂载分区
//!\param int nPartitionIndex 分区索引值
//!\param int Type 格式化类型。0为jfs，1为流式存储
//!\return  成功为0，失败为-1
int CHardDisk::Mount(int nPartitionIndex)
{
	int ret = 0;

	Update();

	if (nPartitionIndex >= diskinfo.PartitionsNum)
	{
		DEBUG_MSG("%d超过分区个数个数%d:\n", nPartitionIndex, diskinfo.PartitionsNum);
		return IDVR_ERROR_PARAMETER;
	}

	ret = EDVR_HD_MountPartition(diskinfo.Partitions[nPartitionIndex].DevName);

	return ret;
}

//!\brief 卸载分区
//!\param int nPartitionIndex 分区索引值
//!\return  成功为0，失败为-1
int CHardDisk::UnMount(int nPartitionIndex)
{
	int ret = 0;

	Update();

	if (nPartitionIndex >= diskinfo.PartitionsNum)
	{
		ERROR_MSG("%d超过分区个数个数%d:\n", nPartitionIndex, diskinfo.PartitionsNum);
		return IDVR_ERROR_PARAMETER;
	}


	if (diskinfo.Partitions[nPartitionIndex].Partition_has_mounted)
	{
		ret = EDVR_HD_UMountPartition(diskinfo.Partitions[nPartitionIndex].Mount_point);
	}


	return ret;
}

//!\brief 挂载硬盘
//!\return  成功为0，失败为-1
int CHardDisk::Mount()
{
	return 0;
}

//!\brief 卸载硬盘
//!\return  成功为0，失败为-1
int CHardDisk::UnMount()
{
	int i = 0;

	int ret = 0;

	char devname[50];

#ifndef USE_ICAMERA

	int BusNo = EDVR_HD_GetBusNumber(Index);

	ret = EDVR_SATA_Index2Name(BusNo, devname);

#else

	ret = EDVR_HD_Index2DevNode(Index, devname, sizeof (devname));

#endif

	TRACE("卸载硬盘%d:%s\n", Index, devname);

	if (-1 == ret)
	{
		DEBUG_MSG("获取磁盘设备名出错\n");
		return -1;
	}

	for (i = 0; i < diskinfo.PartitionsNum; i++)
	{							//卸载所有分区
		ret = UnMount(i);

		if (-1 == ret)
		{
			break;
		}
	}

	//卸载分区库失败
	if (-1 == ret)
	{
		TRACE("卸载分区失败，无法完成硬盘卸载\n");
		Update();
		return ret;
	}

	EDVR_HD_RemoveDiskCallBack(devname);

#ifndef USE_ICAMERA
	TRACE("调用USB库EDVR_SATA_DeviceRemove\n");
	ret = EDVR_SATA_DeviceRemove(BusNo);
	WARN_MSG(0 == ret);
	TRACE("调用USB库EDVR_SATA_DeviceRemove成功:ret = %d\n", ret);
#endif

	Update();

	TRACE("卸载硬盘%d:%s成功\n", Index, devname);
	return ret;
}


extern void rsleep(unsigned int ntime);

//!\brief 创建分区
//!\param int nSize 大小（单位M）
//!\return  成功为0，失败为-1
int CHardDisk::CreatePartiton(int nSize)
{
	int ret = 0;

	Update();

	WARN_MSG(diskinfo.PartitionsNum <= 5);

	if (diskinfo.PartitionsNum >= 5)
	{
		//超过了运行的最大分区数目
		return IDVR_STR_ERROR_PARTITION_NUMERROR;
	}

	ret = EDVR_HD_MakePartitionFromSize(Index, nSize);

	if (ret == 1)
	{
		ret = IDVR_STR_ERROR_DISK_NOFIND;
	}
	else if (2 == ret)
	{
		ret = IDVR_STR_ERROR_PARTITION_NUMERROR;
	}
	else if (-4 == ret)
	{
		ret = IDVR_STR_ERROR_DISK_UMOUNT_BUSY;
	}

	rsleep(1000000 * 5);

	return ret;
}


//!\brief 删除分区
//!\param int nPartitionIndex 分区索引值
//!\return  成功为0，失败为-1
int CHardDisk::DelPartiton(int nPartition)
{
	int ret = 0;

	Update();

	if (diskinfo.PartitionsNum < 2)
	{
		return IDVR_STR_ERROR_SYS_PARTITION;
	}

	ret = EDVR_HD_DelPartition(Index, nPartition);

	if (ret == 1)
	{
		ret = IDVR_STR_ERROR_DISK_NOFIND;
	}
	else if (2 == ret)
	{
		ret = IDVR_STR_ERROR_PARTITION_NUMERROR;
	}
	else if (-1 == ret)
	{
		ret = IDVR_STR_ERROR_PARTITION_NOFIND;
	}
	else if (-2 == ret)
	{
		ret = IDVR_STR_ERROR_PARTITION_NOUNMOUNT;
	}
	else if (-3 == ret)
	{
		ret = IDVR_STR_ERROR_SYS_PARTITION;
	}
	else if (-4 == ret)
	{
		ret = IDVR_STR_ERROR_DISK_UMOUNT_BUSY;
	}

	rsleep(1000000 * 5);

	Update();

	if (0 == ret)
	{
		Macro_UpdateGroupInfo(diskinfo.Dev);
	}

	return ret;
}
