
/*!
*\file   tagDiskGroup.h
*\brief  声明盘组信息
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_TAGDISKGROUPINFO_49AF66C4039A_INCLUDED
#define _INC_TAGDISKGROUPINFO_49AF66C4039A_INCLUDED

#include <time.h>
#include "tagGroupPartitionInfo.h"
#include "global_macro.h"


#define DISKGRUP_NAME_LEN_MAX 32
#define MAX_GROUP_PARTITIONS 32
#define  MAX_BIND_CH_NUM       MAX_ENC_CHANNEL_NUM
//磁盘组信息结构体
//##ModelId=49AF66C4039A
struct tagDiskGroupInfo
{
  public:
	//!<盘组名称
	//##ModelId=49AF6757019C
	char Name[DISKGRUP_NAME_LEN_MAX];

	// !<分组创建时间
	time_t tCreateTime;

	//!<分区个数
	//##ModelId=49AF675701CF
	int Partitions;

	//!<分区信息
	//##ModelId=49B0D64303D9
	tagGroupPartitionInfo tPartitions[MAX_GROUP_PARTITIONS];

	unsigned char bCh[MAX_BIND_CH_NUM];

	unsigned long nDataKeepTime;

};

#endif							/* _INC_TAGDISKGROUPINFO_49AF66C4039A_INCLUDED */
