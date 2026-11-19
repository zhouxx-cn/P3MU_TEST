
/*!
*\file   tagRecSeg.h 
*\brief  录像片段结构体声明
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/


#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_TAGRECSEG_49BE047200E9_INCLUDED
#define _INC_TAGRECSEG_49BE047200E9_INCLUDED

#include <time.h>
//!\brief 录像片段信息
//##ModelId=49BE047200E9
struct tagRecSeg
{
	time_t tBeginTime;			//起始时间点
	time_t tEndTime;			//结束时间点
	int iRecType;				//该时间段的录像类型
	struct tagRecSeg *ptNext;

};

#endif							/* _INC_TAGRECSEG_49BE047200E9_INCLUDED */
