// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "CEmbAlarmIn.h"
#include "DIP_global.h"
//extern "C" unsigned char MP4Sys_AlarmIn();
extern "C" int     ZWSys_AlarmIn(int *pAlarmIndata );


CEmbAlarmIn::CEmbAlarmIn()
{
	m_iNum = 16;
}

CEmbAlarmIn::~CEmbAlarmIn()
{
}

int CEmbAlarmIn::GetNum()
{
	return m_iNum;
}

//!\brief 设置端口数目
//!\return 端口个数
int CEmbAlarmIn::SetNum(int iNum)
{
	if (iNum < 0)
	{
		iNum = 0;
		TRACE_WARN("[%s]unsupported AlarmIn numbers (%d),clip to 0\n", __FUNCTION__, iNum);
	}
	if (iNum > 32)
	{
		iNum = 32;
		TRACE_WARN("[%s]unsupported AlarmIn numbers (%d),clip to 32\n", __FUNCTION__, iNum);
	}
	m_iNum = iNum;

	return 0;
}

int CEmbAlarmIn::GetAlarmInState(int nPort)
{
	int status;

	//status = MP4Sys_AlarmIn();
	ZWSys_AlarmIn(&status);
	return (status >> nPort) & 1;
}
