
/*!
*\file   CHardDisk.h 
*\brief  硬盘声明
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CHARDDISK_49A745AA015A_INCLUDED
#define _INC_CHARDDISK_49A745AA015A_INCLUDED
#include "CDisk.h"
extern "C"
{
#include "disk.h"
#include "usb.h"
}

//!\brief 硬盘类
class CHardDisk:public CDisk
{
  public:
	//!\brief 获取硬盘物理位置的编号
	//!\param 无
	//!\return  返回编号
	int GetBusNumber();

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
	int GetDiskStatus();

	//!\brief 更新硬盘当前的状态
	//!\return 正常为0，正在写入为1，异常为2，警告为3
	int UpdateDiskStatus();

	//!\brief 判断硬盘是否为新硬盘
	//!\param 无
	//!\return  新硬盘为1，否则不是新硬盘
	int IsNewDisk();

	//!\brief 获取磁盘信息
	//!\param tagDiskInfo *Info 硬盘信息结构体指针
	//!\return  成功为0，失败为-1
	int GetInfo(tagDiskInfo * Info);

	int GetLastInfo(tagDiskInfo * Info);

	//!\brief 格式化分区
	//!\param int nPartitionIndex 分区索引值
	//!\param int Type 格式化类型。0为jfs，1为流式存储
	//!\return  成功为0，失败为-1
	int Format(int nPartitionIndex);


	//!\brief 挂载分区
	//!\param int nPartitionIndex 分区索引值
	//!\param int Type 格式化类型。0为jfs，1为流式存储
	//!\return  成功为0，失败为-1
	int Mount(int nPartitionIndex);

	//!\brief 挂载硬盘
	//!\param 无
	//!\param int Type 格式化类型。0为jfs，1为流式存储
	//!\return  成功为0，失败为-1
	int Mount();

	//!\brief 卸载分区
	//!\param int nPartitionIndex 分区索引值
	//!\return  成功为0，失败为-1
	int UnMount(int nPartitionIndex);

	//!\brief 卸载硬盘
	//!\param 无
	//!\return  成功为0，失败为-1
	int UnMount();

	//!\brief 创建分区
	//!\param int nSize 大小（单位M）
	//!\return  成功为0，失败为-1
	int CreatePartiton(int nSize);

	//!\brief 删除分区
	//!\param int nSize 大小（单位M）
	//!\return  成功为0，失败为-1
	int DelPartiton(int nPartition);

	//!\brief 获取硬盘的节点
	//!\param char *Dev 节点号(输出)
	//!\return  成功为0，失败为-1
	int GetDev(char *Dev, int strlen);

	int Index;
	//格式化互斥
	int diskformating;
  private:
	int thePartitionNum;
	int m_status;
	int tagmemcpy(void *info, tagDiskInfo * temp);
	int Update();
	void print();
	DiskDeviceInfoEx diskinfo;
	class CPartition *theCPartition;
};

#endif							/* _INC_CHARDDISK_49A745AA015A_INCLUDED */
