// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "CExtAlarmIn.h"
// #include "sys_banned.h"
#include "global_gather.h"
CExtAlarmIn::CExtAlarmIn()
{
	m_iNum = 0;
}

CExtAlarmIn::~CExtAlarmIn()
{

}

int CExtAlarmIn::GetNum()
{
	return m_iNum;
}

int CExtAlarmIn::GetAlarmInState(int nPort)
{

	return 0;
}
