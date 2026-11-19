#ifndef __TAG_ALARMOUTSCHEDULE_H_
#define __TAG_ALARMOUTSCHEDULE_H_

#include "global_constant.h"

//!报警输出布防时间表
struct tagAlarmOutSchedule
{

	//!0为布防 1为不布防
	unsigned char dwTimeSeg[MAX_DAYS][HY_DVR_ALARMOUTSEG_DAY];

	//!状态,0为禁用,1为启用,2为不支持
	unsigned char bytState;

	//! 保留位
	unsigned char bytReserve[3];
};

typedef struct tagAlarmOutSchedule AlarmOutSchedule;

#endif
