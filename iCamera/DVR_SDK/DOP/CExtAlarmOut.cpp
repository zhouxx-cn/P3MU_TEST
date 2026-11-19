// Copyright (C) 1991 - 1999 Rational Software Corporation
#include "DOP_global.h"
#include "CExtAlarmOut.h"
//#include <iostream>


CExtAlarmOut *CExtAlarmOut::m_pInstance = NULL;

int CExtAlarmOut::m_iInstance_Cnt = 0;

//##ModelId=49BDC4E400FA
CExtAlarmOut::CExtAlarmOut()
{
	m_nNum = 0;
}

CExtAlarmOut::~CExtAlarmOut()
{
	m_nNum = 0;
}



//##ModelId=49BDC4580196
int CExtAlarmOut::GetNum()
{
	return m_nNum;
}

//##ModelId=49BDC45801A5
int CExtAlarmOut::Open()
{
//    int i;
	m_nNum = 0;
	return 0;
}

//##ModelId=49BDC45801B5
int CExtAlarmOut::Close()
{
	m_nNum = 0;
	return 0;
}

//##ModelId=49BDC45801C5
int CExtAlarmOut::GetAlarmOutState(int nPort)
{
	if (nPort < 0 || nPort >= m_nNum)
	{
		TRACE_ERR("[CExtAlarmOut:%s]nPort =%d MaxNum=%d\n", __FUNCTION__, nPort, m_nNum);
		return -1;
	}
	//暂不支持此接口
	return (int) -1;
}

//##ModelId=49BDC4580203
int CExtAlarmOut::SetAlarmOutState(int bOn, int nPort)
{
//    int result = 0;
	if (nPort < 0 || nPort >= m_nNum)
	{
		TRACE_ERR("[CExtAlarmOut:%s]nPort =%d MaxNum=%d\n", __FUNCTION__, nPort, m_nNum);
		return -1;
	}
	//状态控制

/*
   // result = MP4Sys_AlarmOut(nPort,bOn);
    result = ZWSys_AlarmOut(nPort,bOn);
    if (result)
    {
        TRACE_ERR("[CExtAlarmOut:%s]new CExtAlarmOut failed (%d)\n",__FUNCTION__,result);
        return -1;
    }*/

	return (int) 0;
}

//##ModelId=49BDC4580138
CExtAlarmOut *CExtAlarmOut::GetInstance()
{
	if (NULL == m_pInstance)
	{
		m_pInstance = new CExtAlarmOut;
		if (NULL == m_pInstance)
		{
			TRACE_ERR("[CExtAlarmOut:%s]new CExtAlarmOut failed\n", __FUNCTION__);
			return NULL;
		}

	}
	//实例引用计数
	m_iInstance_Cnt++;

	return (CExtAlarmOut *) m_pInstance;
}

//##ModelId=49BDC4580158
int CExtAlarmOut::Release()
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
