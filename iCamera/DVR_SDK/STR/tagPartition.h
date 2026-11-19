
/*!
*\file   tagPartition.h 
*\brief  分区结构体声明
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/


#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_TAGPARTITION_49AF671C0043_INCLUDED
#define _INC_TAGPARTITION_49AF671C0043_INCLUDED

//##ModelId=49AF671C0043
struct tagPartition
{
  public:
	//!<分区名称
	//##ModelId=49AF675700F3
	char Name[30];

	//!<总容量
	//##ModelId=49AF67570142
	unsigned long Total_space;

	unsigned long dwStartCHS;	// 分区起始扇区

	unsigned long dwEndtCHS;	// 分区大小

	//!<可用容量
	//##ModelId=49AF67570160
	unsigned long Available_space;

	//是否已经挂载
	bool bMount;

	//分区的挂载点
	char szMountPoint[50];

};

#endif							/* _INC_TAGPARTITION_49AF671C0043_INCLUDED */
