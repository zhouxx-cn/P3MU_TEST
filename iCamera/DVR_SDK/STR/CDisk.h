
/*!
*\file   CDiskGroup.h 
*\brief  磁盘的接口的声明
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CDISK_49A5FA5501AD_INCLUDED
#define _INC_CDISK_49A5FA5501AD_INCLUDED

#include "CPartition.h"

struct tagDiskInfo;

//!\brief 1、提供磁盘的各种操作
//##ModelId=49A5FA5501AD
class CDisk
{
  public:
	//!\brief 获取硬盘物理位置的编号
	//!\param 无
	//!\return  返回编号
	//##ModelId=49A6030A01C6
	int GetBusNumber();

	//!\brief 判断硬盘是否为新硬盘
	//!\param 无
	//!\return  新硬盘为1，否则不是新硬盘
	//##ModelId=49A603890241
	int IsNewDisk();

	//!\brief 获取磁盘信息
	//!\param tagDiskInfo *Info 硬盘信息结构体指针
	//!\return  成功为0，失败为-1
	//##ModelId=49AF8124014A
	int GetInfo(tagDiskInfo * Info);

	//!\brief 格式化分区
	//!\param int nPartitionIndex 分区索引值
	//!\param int Type 格式化类型。0为jfs，1为流式存储
	//!\return  成功为0，失败为-1
	//##ModelId=49AF8124014A
	int Format(int nPartitionIndex, int Type);


	//!\brief 挂载分区
	//!\param int nPartitionIndex 分区索引值
	//!\param int Type 格式化类型。0为jfs，1为流式存储
	//!\return  成功为0，失败为-1
	//##ModelId=49AF8124014A
	int Mount(int nPartitionIndex);

	//!\brief 挂载硬盘
	//!\param 无
	//!\param int Type 格式化类型。0为jfs，1为流式存储
	//!\return  成功为0，失败为-1
	//##ModelId=49AF8124014A
	int Mount();


	//!\brief 卸载分区
	//!\param int nPartitionIndex 分区索引值
	//!\return  成功为0，失败为-1
	//##ModelId=49AF8124014A
	int UnMount(int nPartitionIndex);

	//!\brief 卸载硬盘
	//!\param 无
	//!\return  成功为0，失败为-1
	//##ModelId=49AF8124014A
	int UnMount();


  private:

};

#endif							/* _INC_CDISK_49A5FA5501AD_INCLUDED */
