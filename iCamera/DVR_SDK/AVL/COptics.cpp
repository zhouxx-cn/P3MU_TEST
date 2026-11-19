// Copyright (C) 1991 - 1999 Rational Software Corporation
#include <string.h>
#include "COptics.h"


int COptics::m_bWorkMode = 0;
int COptics::m_SupportDeFrog = 0;
int COptics::m_SupportShutterMode= 0;
int COptics::m_SupportLDC = 0;

//配置区名
//static const char *OpticsCfgSection = "OpticsCfg";

//初始化全局变量
COptics* COptics::theCOptManager = NULL;

//!\brief 获取句柄
//!\return  成功为为句柄，失败为NULL
//##ModelId=49BB333100F8
COptics* COptics::GetInstance(int nCh, void *hHandle)
{
	// TODO: Add your specialized code here.

	if (NULL == theCOptManager)
	{
		theCOptManager = new COptics(nCh, hHandle);
	}
	
	return (COptics*)theCOptManager;
}


//##ModelId=49B0EEE301D5
COptics::COptics(int nCh, void *hHandle)
{
	m_iChannel = nCh;
	m_henc = hHandle;
	
	ZWSys_GetSensorType(&SensorType);

//	MP4Sys_GetSensorType(&SensorType);//获取sensor类型
//	SensorType=BOARD_SENSOR_NORM5150;
	pCConfig = CConfig::GetInstance();
	LoadParam();

	ZWSys_RegisterKeyStautsCallBack(GetKeyStateCallback);
	
	m_bCfgChange = 1;

	m_itimeSeg = 0;
	m_bEdit = 0;



}

//##ModelId=49BDEEBD023D
int COptics::GetWorkMode()
{
	return (int) m_bWorkMode;
}

int COptics::GetSupport(int type)
{
	int ret=-1;
	switch(type)
	{
		case 0:
			ret = m_SupportDeFrog;break;
		case 1:
			ret = m_SupportShutterMode;break;
		case 2:
			ret = m_SupportLDC;break;
		default:
			break;
	}
	return ret;
}

//##ModelId=49BDEEBD0265
int COptics::SetWorkMode(int nWorkMode)
{
	char key[100];
	
	char OpticCfg[20];
/*	
	BoardSensorType_t SensorType=BOARD_SENSOR_NORM5150;
	//MP4Sys_GetSensorType(&SensorType);//获取sensor类型
	if (SensorType == BOARD_SENSOR_NORM5150 || SensorType == BOARD_SENSOR_OV7725)
	{
		snprintf(OpticCfg, sizeof(OpticCfg), sizeof(OpticCfg), "OpticsCfg%d",1);
	}
	else
	{
		snprintf(OpticCfg, sizeof(OpticCfg), sizeof(OpticCfg), "OpticsCfg%d",0);
	}
*/
	m_bWorkMode = (nWorkMode != 0);
	snprintf(key, sizeof(key), "OpticsWorkMode");
	CConfig::GetInstance()->SetValue(OpticCfg, key, nWorkMode);
	return (int) 0;
}

//!\brief 设置是否为编辑状态，图像参数
//!\praram int bEdit 1-开始编辑，暂停自动配置。0-退出编辑，开始启动自动
//!\return  成功为0，失败为-1
//##ModelId=49BDEEBD02BF
int COptics::EnterEdit(int bEdit)
{
	m_bEdit = (bEdit != 0);
	if (!m_bEdit)
	{
		m_bCfgChange = 1;
	}
	return 0;
}

//##ModelId=49BDEEBD02BF
int COptics::SetPicParam(int timesec, int Type, int value)
{
	char key[100];

	char svalue[100];

	int result;

  	char OpticCfg[20];
 		
	snprintf(OpticCfg, sizeof(OpticCfg), "OpticsCfg%d",1);

	if (timesec < 0 || timesec > 3)
	{
		AVL_ERR("[COptics]ch = %d timesec %d \n", m_iChannel, timesec);
		return -1;
	}
	if (Type < 0 || Type > 4)
	{
		AVL_ERR("[COptics]ch = %d Type %d \n", m_iChannel, Type);
		return -1;
	}
	m_iValue[timesec][Type] = value;

	snprintf(key, sizeof(key), "OpticsValueCh%02d_Seg%d", m_iChannel, timesec);
	snprintf(svalue, sizeof(svalue), "%d,%d,%d,%d,%d", m_iValue[timesec][0], m_iValue[timesec][1], m_iValue[timesec][2], m_iValue[timesec][3], m_iValue[timesec][4]);
	pCConfig->SetValue(OpticCfg, key, svalue);

	m_bCfgChange = 1;
	if (m_bEdit)
	{
		AVL_DBG("\n--1---[software]--ZWSys_EncSetVideoPara---%d--%d--%d--%d--%d---\n", m_iValue[timesec][0], m_iValue[timesec][2], m_iValue[timesec][3], m_iValue[timesec][1],127);
		result = ZWSys_EncSetVideoPara(m_henc, m_iValue[timesec][0], m_iValue[timesec][2], m_iValue[timesec][3],128,128);
		if (result)
		{
			AVL_ERR("[COptics]ZWSys_EncSetVideoPara failed (%d)\n", result);
			return -1;
		}
	}

	return (int) 0;
}

//##ModelId=49BDEEBD0373
int COptics::GetPicParam(int timesec, int Type)
{
	if (timesec < 0 || timesec > 3)
	{
		AVL_ERR("[COptics]ch = %d timesec %d \n", m_iChannel, timesec);
		return -1;
	}
	if (Type < 0 || Type > 4)
	{
		AVL_ERR("[COptics]ch = %d Type %d \n", m_iChannel, Type);
		return -1;
	}


	return (int) m_iValue[timesec][Type];
}

//##ModelId=49BDEEBE000D
int COptics::SetTimeCfg(int timesec, int iStartTime, int iStopTime)
{
	char key[100];

	char value[100];
	
	char OpticCfg[20];
	
	memset(key, 0, sizeof(key));
	memset(value, 0, sizeof(value));
	memset(OpticCfg, 0, sizeof(OpticCfg));
	snprintf(OpticCfg, sizeof(OpticCfg), "OpticsCfg%d", SensorType);

	if (timesec < 0 || timesec > 3)
	{
		AVL_ERR("[COptics]ch = %d timesec %d \n", m_iChannel, timesec);
		return -1;
	}

	m_iStartTime_h[timesec] = iStartTime / 3600;
	m_iStartTime_m[timesec] = (iStartTime / 60) % 60;
	m_iStopTime_h[timesec] = iStopTime / 3600;
	m_iStopTime_m[timesec] = (iStopTime / 60) % 60;
	snprintf(key, sizeof(key), "OpticsTimeCh%02d_Seg%d", m_iChannel, timesec);
	snprintf(value, sizeof(value), "%02d:%02d-%02d:%02d", m_iStartTime_h[timesec], m_iStartTime_m[timesec], m_iStopTime_h[timesec], m_iStopTime_m[timesec]);
	pCConfig->SetValue(OpticCfg, key, value);


	return (int) 0;
}

//##ModelId=49BDEEBE00C2
int COptics::GetTimeCfg(int timesec, int *piStartTime, int *piStopTime)
{
	if (timesec < 0 || timesec > 3)
	{
		AVL_ERR("[COptics]ch = %d timesec %d \n", m_iChannel, timesec);
		return -1;
	}
	if (piStartTime == NULL || piStopTime == NULL)
	{
		AVL_ERR("[COptics]ch = %d piStartTime == NULL || piStopTime == NULL \n", m_iChannel);
		return -1;
	}
	*piStartTime = m_iStartTime_h[timesec] * 3600 + m_iStartTime_m[timesec] * 60;
	*piStopTime = m_iStopTime_h[timesec] * 3600 + m_iStopTime_m[timesec] * 60;

	return (int) 0;
}

int COptics::LoadParam()
{
	char key[100];
	
	char value[100];

	int i;

	char OpticCfg[20];
	
	snprintf(OpticCfg, sizeof(OpticCfg), "OpticsCfg%d",1);

	for (i = 0; i < 4; i++)
	{

		//参数值
		snprintf(key, sizeof(key), "OpticsValueCh%02d_Seg%d", m_iChannel, i);
		//if (SensorType == CMOS_SENSOR_IMX122 || SensorType == CMOS_SENSOR_IMX138 )
		{
			pCConfig->GetValue(OpticCfg, key, "50,50,50,50,50", value, 100);

		}

		sscanf(value, "%d,%d,%d,%d,%d", &m_iValue[i][0], &m_iValue[i][1], &m_iValue[i][2], &m_iValue[i][3], &m_iValue[i][4]);
		if (m_iValue[i][0] > 100)
		{
			m_iValue[i][0] = 100;
		}

		if (m_iValue[i][1] > 100)
		{
			m_iValue[i][1] = 100;
		}

		if (m_iValue[i][2] > 100)
		{
			m_iValue[i][2] = 100;
		}

		if (m_iValue[i][3] > 100)
		{
			m_iValue[i][3] = 100;
		}
		
		if (m_iValue[i][4] > 100)
		{
			m_iValue[i][4] = 100;
		}

		//获得模式
		m_bWorkMode = pCConfig->GetValue(OpticCfg, "OpticsWorkMode", 0);
		//printf("****************m_bWorkMode=%d***************\n",m_bWorkMode);
		
		
		//时间段
		snprintf(key, sizeof(key), "OpticsTimeCh%02d_Seg%d", m_iChannel, i);
		pCConfig->GetValue(OpticCfg, key, "00:00-00:00", value, 100);
		sscanf(value, "%02d:%02d-%02d:%02d", &m_iStartTime_h[i], &m_iStartTime_m[i], &m_iStopTime_h[i], &m_iStopTime_m[i]);
		if (m_iStartTime_h[i] >= 24)
		{
			m_iStartTime_h[i] = 0;
		}
		if (m_iStartTime_m[i] >= 60)
		{
			m_iStartTime_m[i] = 0;
		}
		if (m_iStopTime_h[i] >= 24)
		{
			m_iStopTime_h[i] = 0;
		}
		if (m_iStopTime_m[i] >= 60)
		{
			m_iStopTime_m[i] = 0;
		}

	}

	return 0;
}

int COptics::CheckOptics()
{

	int result;

	int curr_seg = 0;

	time_t curr_time;

	struct tm curr_tm;

	unsigned int curr_min;

	if (m_bEdit)
	{
		return 0;
	}

	if (m_bWorkMode)			//高级模式,且时间段有效
	{

		//获取当前时间段信息
		curr_time = time(NULL);
		localtime_r(&curr_time, &curr_tm);

		curr_min = (curr_tm.tm_hour * 60 + curr_tm.tm_min);
		for (curr_seg = 1; curr_seg < 4; curr_seg++)
		{
			if ((m_iStartTime_h[curr_seg] * 60 + m_iStartTime_m[curr_seg]) <= curr_min &&
				(m_iStopTime_h[curr_seg] * 60 + m_iStopTime_m[curr_seg]) > curr_min)
			{
				break;
			}
		}
		if (curr_seg == 4)
		{
			curr_seg = 0;
		}
	}
	else
	{
		curr_seg = 0;
	}
	if ((m_itimeSeg != curr_seg) || (m_bCfgChange))
	{
		m_itimeSeg = curr_seg;
		if (NULL != m_henc)
		{
			AVL_DBG("\n--2---[software]--ZWSys_EncSetVideoPara---%d--%d--%d--%d--%d---\n", m_iValue[m_itimeSeg][0], m_iValue[m_itimeSeg][1], m_iValue[m_itimeSeg][2], m_iValue[m_itimeSeg][3], m_iValue[m_itimeSeg][4]);

			//onvif过审，去掉限制，但对0值需要特殊处理，和协议交互是0，设备给sdk是1
            if (0 == m_iValue[m_itimeSeg][4])
            {
			    result = ZWSys_EncSetVideoPara(m_henc, m_iValue[m_itimeSeg][0], m_iValue[m_itimeSeg][1], m_iValue[m_itimeSeg][2], m_iValue[m_itimeSeg][3], 1);
            }
            else
            {
			    result = ZWSys_EncSetVideoPara(m_henc, m_iValue[m_itimeSeg][0], m_iValue[m_itimeSeg][1], m_iValue[m_itimeSeg][2], m_iValue[m_itimeSeg][3], m_iValue[m_itimeSeg][4]);
            }

			if (result != 0)
			{
				AVL_ERR("[COptics]ZWSys_EncSetVideoPara failed (%d)\n", result);
				return -1;
			}
		}
		else
		{
			return -1;
		}
	}
	m_bCfgChange = 0;

	return 0;
}


//!\brief 设置恢复出厂回调
//!\praram void(*CallBack)(void) 回调函数指针
//!\return  成功为0，失败为-1
//##ModelId=49BB33310116
int COptics::SetResumeOptCallBack(void (*CallBack)(void))
{
	// TODO: Add your specialized code here.
	
	int ret = -1;
	
	if (NULL != theCOptManager)
	{
		theCOptManager->m_OptCallBack = CallBack;
		return (int) 0;
	}
	
	return (int)ret;
}

//!\brief 获取按键状态
//!\praram KEYCLICK_T status 按键状态	 
//!\根据按键状态做切换模式、恢复出厂操作	
//!\return	成功为0，失败为错误码
int COptics::GetKeyStateCallback(KEYCLICK_T status)
{
	int nRet = IDVR_OK;

    if (KEYCLICK_RESUME == status)//恢复出厂
	{
		if (NULL != theCOptManager->m_OptCallBack)
		{
			theCOptManager->m_OptCallBack();
		}
	}

	return nRet;
}
