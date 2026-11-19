// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_TAGRECORDSCHEDULE_49B7185A03BC_INCLUDED
#define _INC_TAGRECORDSCHEDULE_49B7185A03BC_INCLUDED

#include "global_constant.h"

//##ModelId=49B7185A03BC
typedef struct _tagRecordSchedule_
{
  public:

	//!<0:不录像, 1:定时录像, 2:移动侦测 
	//4:动测|报警
	//##ModelId=49B77FFB03CC
	unsigned char bytRecordTime[MAX_DAYS][HY_DVR_RECORDSEG_DAY];

	//!< 状态,0为禁用,1为启用,2为不支持
	//##ModelId=49B77FC300AB
	unsigned char bytState;

	//!< 保留位
	//##ModelId=49B7803403C4
	unsigned char bytReserve[3];

} tagRecordSchedule;


typedef struct tagRecordTime
{
	int nHour;																/*!< 时 */
	int nMinute;															/*!< 分 */
	int nSecond;															/*!< 秒 */

} Record_TIME, *LPRecord_TIME_V2;

typedef struct _tagRecordScheduleV2_
{
  public:

	//录像状态,0-不开启录像，1-开启录像
	int nEnable;
	//录像类型,0-连续录像,1-事件录像
	int nRecordType;

	//录像开始时间
	Record_TIME startTime;

	//录像结束时间
	Record_TIME stopTime;

} tagRecordScheduleV2;


#endif							/* _INC_TAGSCHEDULE_49B7185A03BC_INCLUDED */
