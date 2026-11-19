
/*!
*\file   CHardDisk.h 
*\brief  分区操作声明
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CPARTITION_49A5FA440311_INCLUDED
#define _INC_CPARTITION_49A5FA440311_INCLUDED

struct tagPartition;

//!\brief  1、提供磁盘分区的各种操作 
//2、在构造函数内判断该分区实例是否已经存在，如果已经存在则直
//接返回已经存在的实例
//##ModelId=49A5FA440311
class CPartition
{
  public:
	//!\brief 获取分区信息
	//!\param tagPa *Info 分区信息结构体指针
	//!\return  成功为0，失败为-1
	//##ModelId=49A5FD7202A3
	int GetInfo(tagPartition * info);

	//!\brief 格式化该分区
	//!\param int Type 0为jfs文件系统，1为流存储文件系统
	//!\return  成功为0，失败为-1
	//##ModelId=49A5FD87039E
	int Format(int Type);

	//!\brief 挂载该分区
	//!\param 无
	//!\return  成功为0，失败为-1
	//##ModelId=49A5FDF503C4
	int Mount();

	//!\brief 卸载该分区
	//!\param 无
	//!\return  成功为0，失败为-1
	//##ModelId=49A5FDF802B0
	int UnMount();

  private:
	//##ModelId=49A5FD530046
	static int Num;

	//##ModelId=49A60657003A
	static CPartition **handle;

};

#endif							/* _INC_CPARTITION_49A5FA440311_INCLUDED */
