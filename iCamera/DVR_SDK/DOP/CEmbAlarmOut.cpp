// Copyright (C) 1991 - 1999 Rational Software Corporation

//#include <iostream>
#include "DOP_global.h"
#include "CEmbAlarmOut.h"
#include "ddt_so.h"

//##ModelId=49BDC4580264
CEmbAlarmOut *CEmbAlarmOut::m_pInstance = NULL;

int CEmbAlarmOut::m_iInstance_Cnt = 0;


//extern "C" int MP4Sys_AlarmOut(long nPort, int isOpen);	
extern "C" int     ZWSys_AlarmOut (long nPort, int bOpen);
//extern "C" int MP4Sys_AlarmOutModel(int iAuto);	//iAuto为0,表示报警输出为手动,否则为自动


extern "C" int     ZWSys_GetBoardInfo(int *pBoardInfo );

//##ModelId=49BDC4E400FA
CEmbAlarmOut::CEmbAlarmOut()
{
	m_nNum = 0;
}

CEmbAlarmOut::~CEmbAlarmOut()
{
	m_nNum = 0;
}



//##ModelId=49BDC4580196
int CEmbAlarmOut::GetNum()
{
	return m_nNum;
}

//##ModelId=49BDC45801A5
int CEmbAlarmOut::Open()
{
    int nBoardID=0;
	
	int result;

	DeviceDescription dds;

	//result = EDVRS_DDT_GetDeviceDescriptionByID(&dds, MP4Sys_GetBoardInfo());
	ZWSys_GetBoardInfo(&nBoardID);
	result = EDVRS_DDT_GetDeviceDescriptionByID(&dds, nBoardID);
	
	if (result)
	{
		m_nNum = 8;
	}
	else
	{
		m_nNum = dds.bytAlarmOutChannels;
	}

//	MP4Sys_AlarmOutModel(0);

	return 0;
}

//##ModelId=49BDC45801B5
int CEmbAlarmOut::Close()
{
	m_nNum = 0;
	return 0;
}

//##ModelId=49BDC45801C5
int CEmbAlarmOut::GetAlarmOutState(int nPort)
{
	if (nPort < 0 || nPort >= m_nNum)
	{
		TRACE_ERR("[CEmbAlarmOut:%s]nPort =%d MaxNum=%d\n", __FUNCTION__, nPort, m_nNum);
		return -1;
	}
	//暂不支持此接口
	return (int) -1;
}

//##ModelId=49BDC4580203
int CEmbAlarmOut::SetAlarmOutState(int nPort, int bOn)
{
	int result = 0;

	if (nPort < 0 || nPort >= m_nNum)
	{
		TRACE_ERR("[CEmbAlarmOut:%s]nPort =%d MaxNum=%d\n", __FUNCTION__, nPort, m_nNum);
		return -1;
	}

	result =  ZWSys_AlarmOut(nPort, bOn);
	if (result)
	{
		TRACE_ERR("[CEmbAlarmOut:%s]ZWSys_AlarmOut return (%d)\n", __FUNCTION__, result);
		return -1;
	}
	return (int) 0;
}

//##ModelId=49BDC4580138
CEmbAlarmOut *CEmbAlarmOut::GetInstance()
{
	if (NULL == m_pInstance)
	{
		m_pInstance = new CEmbAlarmOut;
		if (NULL == m_pInstance)
		{
			TRACE_ERR("[CEmbAlarmOut:%s]new CEmbAlarmOut failed\n", __FUNCTION__);
			return NULL;
		}

	}
	//实例引用计数
	m_iInstance_Cnt++;

	return (CEmbAlarmOut *) m_pInstance;
}

//##ModelId=49BDC4580158
int CEmbAlarmOut::Release()
{
	if (NULL != m_pInstance)
	{
		//
		m_iInstance_Cnt--;
		if (m_iInstance_Cnt <= 0)
		{
			delete m_pInstance;

			m_pInstance = NULL;
		}
	}
	return m_iInstance_Cnt;
}
