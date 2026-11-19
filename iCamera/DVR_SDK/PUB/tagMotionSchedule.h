#ifndef __TAG_MOTIONSCHEDULE_H_
#define __TAG_MOTIONSCHEDULE_H_

#include "global_constant.h"

//!报警输出布防时间表
struct tagMotionSchedule
{


	//!0为布防 1为不布防
	unsigned char dwTimeSeg[MAX_DAYS][HY_DVR_MOTIONSEG_DAY];

	//!状态,0为禁用,1为启用,2为不支持
	unsigned char bytState;

	//! 保留位
	unsigned char bytReserve[3];
};

typedef struct tagMotionSchedule MotionSchedule;

//!声音报警布防时间表
struct tagAudioAlarmSchedule
{
	//!0为布防 1为不布防
	unsigned char dwTimeSeg[MAX_DAYS][HY_DVR_MOTIONSEG_DAY];

	//!状态,0为禁用,1为启用,2为不支持
	unsigned char bytState;

	int nDayMode;		//0:白天	1:夜晚
	
	//! 保留位
	unsigned char bytReserve[8];
};
typedef struct tagAudioAlarmSchedule AudioAlarmSchedule;

//!CGI Motion传入的坐标信息
struct tagMotionRegion 
{
    int bValid;
    int nMotionAreaTopLeftX;               /*!< 移动侦测区域的左上点x相对坐标（0-703）  */
    int nMotionAreaTopLeftY;               /*!< 移动侦测区域的左上点y相对坐标（0-575）  */
    int nMotionAreaBottomRightX;           /*!< 移动侦测区域的右下点x相对坐标（0-703）  */
    int nMotionAreaBottomRightY;           /*!< 移动侦测区域的右下点y相对坐标（0-575）  */ 
};
typedef struct tagMotionRegion MotionRegion;

struct tagPoint {
	int x;
	int y;
};

struct tagPolygon {
	int bValid;
	int size;										/*!< 不规则图像的有效大小 (>=3)  */ 
	struct tagPoint point[MAX_POLYGON_SIZE];		/*!< 不规则图像点位置  */
};

#endif
