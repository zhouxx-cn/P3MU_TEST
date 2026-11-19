
/*!
*\file   tagGroupPartitionInfo.h 
*\brief  盘组结构体声明
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_TAGGROUPPARTITIONINFO_49B0D4120378_INCLUDED
#define _INC_TAGGROUPPARTITIONINFO_49B0D4120378_INCLUDED
#define  MAX_DEVICE_INFO_STRING   32
//##ModelId=49B0D4120378
struct tagGroupPartitionInfo
{
	char acModelName[MAX_DEVICE_INFO_STRING];	// 分区所在磁盘的型号
	char acSerialNo[MAX_DEVICE_INFO_STRING];	// 分区所在磁盘的序列号
	int iDevNo;					// 该分区在这个磁盘的分区号(从1开始)
	char acPartName[12];		// 分区名(分区的结点名)
	unsigned long dwPartSize;	// 分区大小(单位：M)

};

#endif							/* _INC_TAGGROUPPARTITIONINFO_49B0D4120378_INCLUDED */
