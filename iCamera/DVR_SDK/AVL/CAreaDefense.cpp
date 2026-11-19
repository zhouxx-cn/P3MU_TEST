#include <string.h>
#include <time.h>
#include <math.h>
#include "global_macro.h"
#include "CAreaDefense.h"
#include "CActionAlarm.h"


//缺省计划配置
static const char *LinkCfg_Default = "a03:1:1::::::::::::::::::::::::::::::::::::";

CAreaDefense::CAreaDefense(int nCh, void *hHandle)
{
	m_iChannel = nCh;
	m_henc = hHandle;
	m_bMotionEnable = false;
	m_bHumanShapeEnable = false;
	pCConfig = NULL;
	m_iRectNum = 0;
} 

CAreaDefense::~CAreaDefense()
{
	UnInit();
}

int CAreaDefense::Init(int nMaxCh, int nMaxAlarmOut)
{
	pCConfig = CConfig::GetInstance();
	if (NULL == pCConfig)
	{
		AVL_ERR("[CHumanShape:%s]NULL == pCConfig\n", __FUNCTION__);
		UnInit();
		return -1;
	}

#ifndef TEST_MEM_INFO
	m_pActionAlarm = new CActionAlarm;
#else
	m_pActionAlarm = CommonKit::TCreateObject<CActionAlarm>::CreateObject(__FILE__, __FUNCTION__, __LINE__);
#endif

	if (m_pActionAlarm == NULL)
	{
		UnInit();
		AVL_ERR("[CHumanShape:%s]new CActionAlarm failed\n", __FUNCTION__);
		return -1;
	}

	int i, j = 0;
	char key[64] = {0};
	if (!pCConfig->CheckSectionExist(DEFENCE_AREA_CFG_SECTION))
	{
		// 没有该项参数生成默认配置
		{
			snprintf(key, sizeof(key), "PolygonArea_00_Enable");
			pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 1);
			snprintf(key, sizeof(key), "PolygonArea_00_Size");
			pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 4);
			snprintf(key, sizeof(key), "PolygonArea_00_P00_X");
			pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
			snprintf(key, sizeof(key), "PolygonArea_00_P00_Y");
			pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
			snprintf(key, sizeof(key), "PolygonArea_00_P01_X");
			pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 9999);
			snprintf(key, sizeof(key), "PolygonArea_00_P01_Y");
			pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
			snprintf(key, sizeof(key), "PolygonArea_00_P02_X");
			pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 9999);
			snprintf(key, sizeof(key), "PolygonArea_00_P02_Y");
			pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 9999);
			snprintf(key, sizeof(key), "PolygonArea_00_P03_X");
			pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
			snprintf(key, sizeof(key), "PolygonArea_00_P03_Y");
			pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 9999);
			snprintf(key, sizeof(key), "PolygonArea_00_P04_X");
			pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
			snprintf(key, sizeof(key), "PolygonArea_00_P04_Y");
			pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
			snprintf(key, sizeof(key), "PolygonArea_00_P05_X");
			pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
			snprintf(key, sizeof(key), "PolygonArea_00_P05_Y");
			pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
			snprintf(key, sizeof(key), "PolygonArea_00_P06_X");
			pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
			snprintf(key, sizeof(key), "PolygonArea_00_P06_Y");
			pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
			snprintf(key, sizeof(key), "PolygonArea_00_P07_X");
			pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
			snprintf(key, sizeof(key), "PolygonArea_00_P07_Y");
			pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
		}

		for (i=1; i < MAX_AREADEFENSE_NUM; i++)
		{
			snprintf(key, sizeof(key), "PolygonArea_%02d_Enable", i);
			pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
			snprintf(key, sizeof(key), "PolygonArea_%02d_Size", i);
			pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 0);

			for (j=0; j < 8; j++)
			{
				snprintf(key, sizeof(key), "PolygonArea_%02d_P%02d_X", i, j);
				pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
				snprintf(key, sizeof(key), "PolygonArea_%02d_P%02d_Y", i, j);
				pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
			}
		}

		for (i=0; i < MAX_ENC_CHANNEL_NUM; i++)
		{
			snprintf(key, sizeof(key), "AreaDefenseAction%02d", i);
			pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, LinkCfg_Default);

			snprintf(key, sizeof(key), "ScheduleCfgEnable%02d", i);
			pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
			for (j = 0; j < 7; j++)
			{
				snprintf(key, sizeof(key), "ScheduleCfg%02d_%01d", i, j);
				pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, SCHEDULE_TIME_ONE);
			}
		}
		snprintf(key, sizeof(key), "Motion_Enable");
		pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
		snprintf(key, sizeof(key), "HumanShape_Enable");
		pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 1);
		snprintf(key, sizeof(key), "FaceCap_Enable");
		pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 1);
	}

	m_pActionAlarm->Init(m_iChannel, DEFENCE_AREA_CFG_SECTION, "AreaDefenseAction", nMaxCh, nMaxAlarmOut);
	LoadParam();

	return 0;
}

int CAreaDefense::UnInit()
{
	if (m_pActionAlarm != NULL)
	{
#ifndef TEST_MEM_INFO
		delete m_pActionAlarm;
#else
		CommonKit::TDestroyObject<CActionAlarm>::DestroyObject(m_pActionAlarm);
#endif
		m_pActionAlarm = NULL;
	}

	return 0;
}

int CAreaDefense::LoadParam()
{
	unsigned int i, j = 0;
	char key[64] = {0};
	char val_str[256] = {0};
	int nBoardID;
	ZWSys_GetBoardInfo(&nBoardID);

	// 获取使能
	snprintf(key, sizeof(key), "Motion_Enable");
	m_bMotionEnable = pCConfig->GetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
	snprintf(key, sizeof(key), "HumanShape_Enable");
	//SigmaStar平台的F23版本不需要人形功能，关闭使能
	if (0x20a6 == nBoardID)
	{
		m_bHumanShapeEnable = 0;
	}
	else
	{
		m_bHumanShapeEnable = pCConfig->GetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
	}
	snprintf(key, sizeof(key), "FaceCap_Enable");
	m_bFaceCapEnable = pCConfig->GetValue(DEFENCE_AREA_CFG_SECTION, key, 0);

	snprintf(key, sizeof(key), "CarDetect_Enable");
	m_bCarDetectEnable = pCConfig->GetValue(DEFENCE_AREA_CFG_SECTION, key, 0);

	//区域(将保存的坐标取出来放入全局数组中)
	for (i=0; i < MAX_AREADEFENSE_NUM; i++)
	{
		snprintf(key, sizeof(key), "PolygonArea_%02d_Enable", i);
		m_RegionInfos[i].bValid = pCConfig->GetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
		// 支持3-8边形
		snprintf(key, sizeof(key), "PolygonArea_%02d_Size", i);
		m_RegionInfos[i].size = pCConfig->GetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
		if ((m_RegionInfos[i].size < 3) || (m_RegionInfos[i].size > 8))
		{
			m_RegionInfos[i].bValid = 0;
			m_RegionInfos[i].size = 0;
		}
		
		for (j = 0; j < sizeof(m_RegionInfos[i].point)/sizeof(m_RegionInfos[i].point[0]); j++)
		{
			snprintf(key, sizeof(key), "PolygonArea_%02d_P%02d_X", i, j);
			m_RegionInfos[i].point[j].x = pCConfig->GetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
			snprintf(key, sizeof(key), "PolygonArea_%02d_P%02d_Y", i, j);
			m_RegionInfos[i].point[j].y = pCConfig->GetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
			if (m_RegionInfos[i].point[j].x < 0)
			{
				m_RegionInfos[i].point[j].x = 0;
			}
			if (m_RegionInfos[i].point[j].x > 9999)
			{
				m_RegionInfos[i].point[j].x = 9999;
			}
			if (m_RegionInfos[i].point[j].y < 0)
			{
				m_RegionInfos[i].point[j].y = 0;
			}
			if (m_RegionInfos[i].point[j].y > 9999)
			{
				m_RegionInfos[i].point[j].y = 9999;
			}
			if (m_RegionInfos[i].bValid == 0)
			{
				continue;
			}
			m_iRectNum++;
		}
	}

	//导入计划
	//ScheduleCfgEnable[ch]=[启用]
	//ScheduleCfg[ch]_[星期]=[48]--[启用][星期][半小时]
	snprintf(key, sizeof(key), "ScheduleCfgEnable%02d", m_iChannel);
	m_ScheduleCfg.bytState = pCConfig->GetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
	m_ScheduleCfg.bytReserve[0] = 0;
	m_ScheduleCfg.bytReserve[1] = 0;
	m_ScheduleCfg.bytReserve[2] = 0;
	for (i = 0; i < 7; i++)
	{
		snprintf(key, sizeof(key), "ScheduleCfg%02d_%01d", m_iChannel, i);
		pCConfig->GetValue(DEFENCE_AREA_CFG_SECTION, key, SCHEDULE_TIME_ONE, val_str, 256);
		for (j = 0; j < 48; j++)
		{
			m_ScheduleCfg.dwTimeSeg[i][j] = val_str[j] - '0';
		}
	}

	return 0;
}

bool CAreaDefense::Motion_IsEnable()
{
	return m_bMotionEnable;
}

bool CAreaDefense::HumanShape_IsEnable()
{
	return m_bHumanShapeEnable;
}

bool CAreaDefense::FaceCap_IsEnable()
{
	return m_bFaceCapEnable;
}

bool CAreaDefense::CarDetect_IsEnable()
{
	return m_bCarDetectEnable;
}

int CAreaDefense::Motion_SetEnable(bool e)
{
	m_bMotionEnable = e;
	int val = (e == true)?1:0;
	pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, "Motion_Enable", val);
	return 0;
}

int CAreaDefense::HumanShape_SetEnable(bool e)
{
	int nBoardID;
	ZWSys_GetBoardInfo(&nBoardID);

	if (0x20a6 == nBoardID)//此版型没有人形功能
	{
		e = 0;
	}
	m_bHumanShapeEnable = e;
	int val = (e == true)?1:0;
	pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, "HumanShape_Enable", val);
	return 0;
}

int CAreaDefense::FaceCap_SetEnable(bool e)
{
	return 0;
}

int CAreaDefense::CarDetect_SetEnable(bool e)
{
	m_bCarDetectEnable = e;
	int val = (e == true)?1:0;
	pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, "CarDetect_Enable", val);
	return 0;
}

int CAreaDefense::GetSchedule(tagMotionSchedule * cfg)
{
	if (NULL == cfg)
	{
		AVL_ERR("[CHumanShape:%s]cfg==NULL\n", __FUNCTION__);
		return -1;
	}
	memcpy(cfg, &m_ScheduleCfg, sizeof (tagMotionSchedule));
	return 0;
}

int CAreaDefense::SetSchedule(const tagMotionSchedule * cfg)
{
	char value[255];
	char key_str[30];
	int i, j;

	if (NULL == cfg)
	{
		AVL_ERR("[CAreaDefense:%s]cfg==NULL\n", __FUNCTION__);
		return -1;
	}
	if (cfg->bytState > 2)
	{
		AVL_ERR("[CAreaDefense:%s]cfg->bytState=%d\n", __FUNCTION__, cfg->bytState);
		return -1;
	}

	//移动计划--
	//ScheduleCfgEnable[ch]=[启用]
	//ScheduleCfg[ch]_[星期]=[48]--[启用][星期][半小时]
	snprintf(key_str, sizeof(key_str), "ScheduleCfgEnable%02d", m_iChannel);
	pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key_str, cfg->bytState);

	for (i = 0; i < 7; i++)
	{
		for (j = 0; j < 48; j++)
		{
			value[j] = cfg->dwTimeSeg[i][j] + '0';
		}
		value[j] = 0;
		snprintf(key_str, sizeof(key_str), "ScheduleCfg%02d_%01d", m_iChannel, i);

		pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key_str, value);
		AVL_DBG("[CAreaDefense:%s]cfg->bytState=%d,Write:%s\n", __FUNCTION__, cfg->bytState, value);
	}
	//更新计划
	memcpy(&m_ScheduleCfg, cfg, sizeof (tagMotionSchedule));

	AiScehdule AreaDefenseSchedule;
	AreaDefenseSchedule.state = m_ScheduleCfg.bytState;
	memcpy(AreaDefenseSchedule.dwTimeSeg, m_ScheduleCfg.dwTimeSeg, SEH_MAX_DAYS * SEH_MAX_SEG_DAYS* sizeof(unsigned char));
	//布放计划传到sdk,直接用之前人形的。因智能事件不会同时存在
	ZWSys_EncSetHumanShapeSchedule(m_henc, &AreaDefenseSchedule);
    
	return 0;
}

int CAreaDefense::EnableAction(int Action, int bEnable)
{
	return m_pActionAlarm->EnableAction(Action, bEnable);
}

int CAreaDefense::IsEnableAction(int Action)
{
	return m_pActionAlarm->IsEnableAction(Action);
}

int CAreaDefense::GetRecordParam(int *dwChannelFlag)
{
	return m_pActionAlarm->GetRecordParam(dwChannelFlag);
}

int CAreaDefense::SetRecordParam(int dwChannelFlag)
{
	return m_pActionAlarm->SetRecordParam(dwChannelFlag);
}

int CAreaDefense::GetSnapshotParam(int *dwChannelFlag)
{
	return m_pActionAlarm->GetSnapshotParam(dwChannelFlag);
}

int CAreaDefense::SetSnapshotParam(int dwChannelFlag)
{
	return m_pActionAlarm->SetSnapshotParam(dwChannelFlag);
}

int CAreaDefense::GetPtzParam(int nChannel, int *nAction, int *nIndex)
{
	return m_pActionAlarm->GetPtzParam(nChannel, nAction, nIndex);
}

int CAreaDefense::SetPtzParam(int nChannel, int nAction, int nIndex)
{
	return m_pActionAlarm->SetPtzParam(nChannel, nAction, nIndex);
}

int CAreaDefense::GetDopParam(int *dwOutPortFlag)
{
	return m_pActionAlarm->GetDopParam(dwOutPortFlag);
}

int CAreaDefense::SetDopParam(int dwOutPortFlag)
{
	return m_pActionAlarm->SetDopParam(dwOutPortFlag);
}

int CAreaDefense::GetPolygonRegionCfg(struct tagPolygon *pRegionCfg, int nRegionNum)
{
	if (pRegionCfg == NULL)
	{
		AVL_ERR("[CHumanShape:%s] pcfg == NULL\n", __FUNCTION__);
		return 0;
	}

	AVL_INFO("===Into CHumanShape::GetEnableMagicBlock_CGI begin===\n");
	// 将 struct tagPolygon 转换为 struct tagMotionRegion
	int size = (m_iRectNum>nRegionNum)?nRegionNum:m_iRectNum;
	for (int i = 0; i < size; i++)
	{
		memcpy(&pRegionCfg[i], &m_RegionInfos[i], sizeof(struct tagPolygon));
	}
	return size;
}

int CAreaDefense::SetRegionCfg(tagMotionRegion *pRegionCfg, int nRegionNum)
{
    int i = 0;
    char key[30] = {0};

	if (nRegionNum < 0 || nRegionNum > 16)
	{
		AVL_ERR("[CHumanShape:%s] nRegionAreas=%d MAX_REGION=%d \n", 
			__FUNCTION__, nRegionNum, 16);
		return -1;
	}
	
	if (pRegionCfg == NULL)
	{
		AVL_ERR("[CHumanShape:%s] pcfg == NULL \n", __FUNCTION__);
		return -1;
	}
	
	m_iRectNum = 0;
	memset(m_RegionInfos, 0, sizeof(m_RegionInfos));

	/* 将坐标框选出对应的区域转化为宏块 */
	// 仅支持一个布防区域
	int nRegNum = 1;
	for (i=0; i<nRegNum; i++)  // CGI添加的区域个数(IE端限制为4个，中间层扩展为16个)
	{
		if ((pRegionCfg[i].bValid == 0) 
			|| ((pRegionCfg[i].bValid != 0) && (pRegionCfg[i].bValid != 1))
			|| ((pRegionCfg[i].nMotionAreaTopLeftX >= pRegionCfg[i].nMotionAreaBottomRightX)
				|| (pRegionCfg[i].nMotionAreaTopLeftY >= pRegionCfg[i].nMotionAreaBottomRightY)))
		{
			//将配置清零
			m_RegionInfos[i].bValid = 0;
			snprintf(key, sizeof(key), "PolygonArea_%02d_Enable", i);
			pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
			m_RegionInfos[i].size = 0;
			snprintf(key, sizeof(key), "PolygonArea_%02d_Size", i);
			pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
			for (int j = 0; j < 8; j++)
			{
				m_RegionInfos[i].point[j].x = 0;
				snprintf(key, sizeof(key), "PolygonArea_%02d_P%02d_X", i, j);
				pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
				m_RegionInfos[i].point[j].y = 0;
				snprintf(key, sizeof(key), "PolygonArea_%02d_P%02d_Y", i, j);
				pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
			}
			continue;
		}

    	/* 侦测区域,坐标范围位于[704, 576] */
    	if (pRegionCfg[i].nMotionAreaTopLeftX > 703)
    	{
    		AVL_ERR("nMotionAreaTopLeftX = %d is overflow\n", pRegionCfg[i].nMotionAreaTopLeftX);
    		pRegionCfg[i].nMotionAreaTopLeftX = 703;
    	}
		else if (pRegionCfg[i].nMotionAreaTopLeftX < 0)
    	{
    		AVL_ERR("nMotionAreaTopLeftX = %d is overflow\n", pRegionCfg[i].nMotionAreaTopLeftX);
    		pRegionCfg[i].nMotionAreaTopLeftX = 0;
    	}
    	if (pRegionCfg[i].nMotionAreaTopLeftY > 575)
    	{
    		AVL_ERR("nMotionAreaTopLeftY = %d is overflow\n", pRegionCfg[i].nMotionAreaTopLeftY);
    		pRegionCfg[i].nMotionAreaTopLeftY = 575;
    	}
		else if (pRegionCfg[i].nMotionAreaTopLeftY < 0)
    	{
    		AVL_ERR("nMotionAreaTopLeftY = %d is overflow\n", pRegionCfg[i].nMotionAreaTopLeftY);
    		pRegionCfg[i].nMotionAreaTopLeftY = 0;
    	}
    	if (pRegionCfg[i].nMotionAreaBottomRightX > 703)
    	{
    		AVL_ERR("nMotionAreaBottomRightX = %d is overflow\n", pRegionCfg[i].nMotionAreaBottomRightX);
    		pRegionCfg[i].nMotionAreaBottomRightX = 703;
    	}
		else if (pRegionCfg[i].nMotionAreaBottomRightX < 0)
    	{
    		AVL_ERR("nMotionAreaBottomRightX = %d is overflow\n", pRegionCfg[i].nMotionAreaBottomRightX);
    		pRegionCfg[i].nMotionAreaBottomRightX = 0;
    	}
    	if (pRegionCfg[i].nMotionAreaBottomRightY > 575)
    	{
    		AVL_ERR("nMotionAreaBottomRightY = %d is overflow\n", pRegionCfg[i].nMotionAreaBottomRightY);
    		pRegionCfg[i].nMotionAreaBottomRightY = 575;
    	}
		else if (pRegionCfg[i].nMotionAreaBottomRightY < 0)
    	{
    		AVL_ERR("nMotionAreaBottomRightY = %d is overflow\n", pRegionCfg[i].nMotionAreaBottomRightY);
    		pRegionCfg[i].nMotionAreaBottomRightY = 0;
    	}

		m_RegionInfos[i].bValid = 1;
		snprintf(key, sizeof(key), "PolygonArea_%02d_Enable", i);
		pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, pRegionCfg[i].bValid);
		m_RegionInfos[i].size = 4;
		snprintf(key, sizeof(key), "PolygonArea_%02d_Size", i);
		pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, m_RegionInfos[i].size);

		m_RegionInfos[i].point[0].x = (pRegionCfg[i].nMotionAreaTopLeftX*10000)/704;
		snprintf(key, sizeof(key), "PolygonArea_%02d_P00_X", i);
		pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, m_RegionInfos[i].point[0].x);
		m_RegionInfos[i].point[0].y = (pRegionCfg[i].nMotionAreaTopLeftY*10000)/576;
		snprintf(key, sizeof(key), "PolygonArea_%02d_P00_Y", i);
		pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, m_RegionInfos[i].point[0].y);
		m_RegionInfos[i].point[1].x = (pRegionCfg[i].nMotionAreaBottomRightX*10000)/704;
		snprintf(key, sizeof(key), "PolygonArea_%02d_P01_X", i);
		pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, m_RegionInfos[i].point[1].x);
		m_RegionInfos[i].point[1].y = (pRegionCfg[i].nMotionAreaTopLeftY*10000)/576;
		snprintf(key, sizeof(key), "PolygonArea_%02d_P01_Y", i);
		pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, m_RegionInfos[i].point[1].y);
		m_RegionInfos[i].point[2].x = (pRegionCfg[i].nMotionAreaBottomRightX*10000)/704;
		snprintf(key, sizeof(key), "PolygonArea_%02d_P02_X", i);
		pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, m_RegionInfos[i].point[2].x);
		m_RegionInfos[i].point[2].y = (pRegionCfg[i].nMotionAreaBottomRightY*10000)/576;
		snprintf(key, sizeof(key), "PolygonArea_%02d_P02_Y", i);
		pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, m_RegionInfos[i].point[2].y);
		m_RegionInfos[i].point[3].x = (pRegionCfg[i].nMotionAreaTopLeftX*10000)/704;
		snprintf(key, sizeof(key), "PolygonArea_%02d_P03_X", i);
		pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, m_RegionInfos[i].point[3].x);
		m_RegionInfos[i].point[3].y = (pRegionCfg[i].nMotionAreaBottomRightY*10000)/576;
		snprintf(key, sizeof(key), "PolygonArea_%02d_P03_Y", i);
		pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, m_RegionInfos[i].point[3].y);
		snprintf(key, sizeof(key), "PolygonArea_%02d_P04_X", i);
		pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
		snprintf(key, sizeof(key), "PolygonArea_%02d_P04_Y", i);
		pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
		snprintf(key, sizeof(key), "PolygonArea_%02d_P05_X", i);
		pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
		snprintf(key, sizeof(key), "PolygonArea_%02d_P05_Y", i);
		pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
		snprintf(key, sizeof(key), "PolygonArea_%02d_P06_X", i);
		pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
		snprintf(key, sizeof(key), "PolygonArea_%02d_P06_Y", i);
		pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
		snprintf(key, sizeof(key), "PolygonArea_%02d_P07_X", i);
		pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
		snprintf(key, sizeof(key), "PolygonArea_%02d_P07_Y", i);
		pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
		m_iRectNum++;
	}

	return (int) 0;
}

int CAreaDefense::SetPolygonRegionCfg(struct tagPolygon *pRegionCfg, int nRegionNum)
{
    int i = 0;
    char key[30] = {0};

	if (nRegionNum < 0 || nRegionNum > MAX_AREADEFENSE_NUM)
	{
		AVL_ERR("[CHumanShape:%s] nRegionAreas=%d MAX_REGION=%d \n", 
			__FUNCTION__, nRegionNum, MAX_AREADEFENSE_NUM);
		return -1;
	}
	
	if (pRegionCfg == NULL)
	{
		AVL_ERR("[CHumanShape:%s] pcfg == NULL \n", __FUNCTION__);
		return -1;
	}
	
	m_iRectNum = 0;
	memset(m_RegionInfos, 0, sizeof(m_RegionInfos));

	/* 将坐标框选出对应的区域转化为宏块 */
	int nRegNum = (nRegionNum<MAX_AREADEFENSE_NUM)?nRegionNum:MAX_AREADEFENSE_NUM;
	for (i=0; i<nRegNum; i++)  //
	{
		if (pRegionCfg[i].bValid != 1)
		{
			//将配置清零
			m_RegionInfos[i].bValid = 0;
			snprintf(key, sizeof(key), "PolygonArea_%02d_Enable", i);
			pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
			m_RegionInfos[i].size = 0;
			snprintf(key, sizeof(key), "PolygonArea_%02d_Size", i);
			pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 4);
			for (int j = 0; j < 8; j++)
			{
				m_RegionInfos[i].point[j].x = 0;
				snprintf(key, sizeof(key), "PolygonArea_%02d_P%02d_X", i, j);
				pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
				m_RegionInfos[i].point[j].y = 0;
				snprintf(key, sizeof(key), "PolygonArea_%02d_P%02d_Y", i, j);
				pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, 0);
			}
			continue;
		}

		for (int j = 0; j < pRegionCfg[i].size; j++)
		{
			if (pRegionCfg[i].point[j].x < 0)
			{
				pRegionCfg[i].point[j].x = 0;
			}
			if (pRegionCfg[i].point[j].x > 9999)
			{
				pRegionCfg[i].point[j].x = 9999;
			}
			if (pRegionCfg[i].point[j].y < 0)
			{
				pRegionCfg[i].point[j].y = 0;
			}
			if (pRegionCfg[i].point[j].y > 9999)
			{
				pRegionCfg[i].point[j].y = 9999;
			}
		}

		memcpy(&m_RegionInfos[i], &pRegionCfg[i], sizeof(struct tagPolygon));
		snprintf(key, sizeof(key), "PolygonArea_%02d_Enable", i);
		pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, m_RegionInfos[i].bValid);
		snprintf(key, sizeof(key), "PolygonArea_%02d_Size", i);
		pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, m_RegionInfos[i].size);

		for (int j = 0; j < m_RegionInfos[i].size; j++)
		{
			snprintf(key, sizeof(key), "PolygonArea_%02d_P%02d_X", i, j);
			pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, m_RegionInfos[i].point[j].x);
			snprintf(key, sizeof(key), "PolygonArea_%02d_P%02d_Y", i, j);
			pCConfig->SetValue(DEFENCE_AREA_CFG_SECTION, key, m_RegionInfos[i].point[j].y);
		}
		m_iRectNum++;
	}

	return (int) 0;
}
