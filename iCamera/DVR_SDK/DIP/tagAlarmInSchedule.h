#ifndef __TAG_ALARMINSCHEDULE_H_
#define __TAG_ALARMINSCHEDULE_H_

#include "global_constant.h"

//!\brief 报警输入时间表
struct tagAlarmInSchedule
{

	//!0为布防 1为不布防
	unsigned char dwTimeSeg[MAX_DAYS][HY_DVR_ALARMINSEG_DAY];

	//!状态,0为禁用,1为启用,2为不支持
	unsigned char bytState;

	//! 保留位
	unsigned char bytReserve[3];
};

typedef tagAlarmInSchedule AlarmInSchedule;

#endif
