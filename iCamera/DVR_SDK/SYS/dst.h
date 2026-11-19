#include <pthread.h>
#include "CConfig.h"
#include "macro_pthread.h"
#include "pthreadmacro.h"
#include "mysystem.h"
//#include "newhie/HieServer_Cfg.h"

#ifndef __MANAGER_TIME_EX_H__
#define __MANAGER_TIME_EX_H__

typedef struct tagZW_DSTWEEKMODE_TIMEPOINT_CFG_EX
{
	int bValid;																		/*!< 本结构体是否有效，0表示无效  */

	unsigned char bytStartMonth;								// 月份  (范围:0-11)
	unsigned char bytWhichWeek;								// 第几周(范围:0-6,6表示最后一周)
	unsigned char bytWeekDay;								// 星期几(范围:0-6)

	unsigned char bytHour;									// 小时[范围:0-23]							
	unsigned char bytMinute;									// 分钟[范围:0-59]
	unsigned char bytSecond;									// 秒[范围:0-59]

	unsigned char bytReserved[2];
}ZW_DSTWEEKMODE_TIMEPOINT_CFG_EX;

typedef enum 
{
	eDSTTimeFormat_Week_EX,
	eDSTTimeFormat_Date_EX,
}eDSTTimeFormat_EX;

typedef struct
{
	
	tagTime start;
	
	tagTime end;
	
	int byDSTBias;

	eDSTTimeFormat_EX   dwMode;							// 0,周模式，1,日模式
	ZW_DSTWEEKMODE_TIMEPOINT_CFG_EX struWeekModeStart;		// 周模式开始时间，dwMode为eDSTTimeFormat_Week时间有效
	ZW_DSTWEEKMODE_TIMEPOINT_CFG_EX struWeekModeStop;		// 周模式结束时间，dwMode为eDSTTimeFormat_Week时间有效
	
} DVR_DST_EX, *LPDVR_DST_EX;
#endif

typedef enum
{
	eDSTAllowChangeTime,//表示当前允许夏令时修改时间（当前还是未修改之前的系统时间）
	eDstForbidChangeTime,//夏令时启用并且已经修改过时间，此时在当前配置下不允许再修改时间（夏时制时间）
}eDDSTTimeStatus;

typedef void (*CallBack) (time_t t);

#define SAVE_STATUS(x) {\
						\
	m_DSTStatus = x;		\
	CConfig::GetInstance()->SetValue(SECTION,N_STATUS,m_DSTStatus);\
	CConfig::GetInstance()->SaveFile();\
}

const static time_t INVALID_TIME = (0xffffffff);


int DST_Init();

int EDVR_MAN_SetDST(LPDVR_DST_EX Dst);

int EDVR_MAN_GetDST(LPDVR_DST_EX Dst);

int EDVR_MAN_EnableDST(unsigned int enable);

int EDVR_MAN_IsEnableDST();

void EDVR_MAN_ManualRefresh_DST(void);

int EDVR_MAN_GetDstIsWork(void);

int EDVR_MAN_GetDstBias(void);

void EDVR_MAN_SetCallBack(CallBack callback);
