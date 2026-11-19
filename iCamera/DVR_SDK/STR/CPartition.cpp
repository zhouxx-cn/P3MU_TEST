
/*!
*\file   CPartition.cpp 
*\brief  分区操作实现
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/
#include "tagPartition.h"
#include "CPartition.h"
// #include "sys_banned.h"
#include "global_gather.h"


//##ModelId=49A5FD530046
int CPartition::Num = 0;

//##ModelId=49A60657003A
CPartition **CPartition::handle;

//##ModelId=49A5FD7202A3
int CPartition::GetInfo(tagPartition * info)
{
	return (int) 0;
}

//##ModelId=49A5FD87039E
int CPartition::Format(int Type)
{
	return (int) 0;
}

//##ModelId=49A5FDF503C4
int CPartition::Mount()
{
	return (int) 0;
}

//##ModelId=49A5FDF802B0
int CPartition::UnMount()
{
	return (int) 0;
}
