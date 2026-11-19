
/*!
*\file   tagDiskInfo.h 
*\brief  磁盘结构体声明
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_TAGDISKINFO_49AF6714001A_INCLUDED
#define _INC_TAGDISKINFO_49AF6714001A_INCLUDED

#include "tagPartition.h"
#define  DISK_NAME_LEN_MAX 32
#define MAX_DISK_USER_PARTITIONS 5
//磁盘结构体
//##ModelId=49AF6714001A
struct tagDiskInfo
{
  public:
	//!<设备型号
	//##ModelId=49AF6757027A
	char DeviceName[DISK_NAME_LEN_MAX];
	//!<设备序列号
	char SerialNo[DISK_NAME_LEN_MAX];
	//!<分区个数
	//##ModelId=49AF675702AB
	unsigned int Partitions;

	//!<是否正在写入
	//##ModelId=49AF675702C9
	int IsWritting;

	//!<总容量
	//##ModelId=49AF675702E7
	unsigned long Total_space;

	//!<分区信息
	//##ModelId=49BF47220309
	tagPartition tPartitions[MAX_DISK_USER_PARTITIONS];

};

#endif							/* _INC_TAGDISKINFO_49AF6714001A_INCLUDED */
