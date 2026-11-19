// Copyright (C) 2018 - 2025 Rational Software Corporation
#include <string.h>
#include <time.h>
#include <math.h>
#include "global_macro.h"
#include "CHumanShape.h"
#include "CActionAlarm.h"
#include "AiFrameDef.h"
#include "MPU_LCD_API.h"

static const int BUFFSIZE = MAX_PICTURE_BUF_SIZE + sizeof(PEOPLE_FRAME_HEAD);

int CHumanShape::m_AlarmType = 0;
ALARMCALLBACK CHumanShape::m_FxnCallBack = NULL;

#ifdef TRACE_AVL
#undef TRACE_AVL
#define TRACE_AVL 0?1:printf
#endif

//##ModelId=49B0EEBF0039
CHumanShape::CHumanShape(int nCh, void *hHandle)
{
	m_iChannel = nCh;
	m_AlarmType = 0;

	m_henc = hHandle;
	m_FxnCallBack = NULL;
	m_bTrigger = 0;
	m_nChn = 0;
	m_lastTrigger = 0;
	m_bEnable = 0;
	m_bAi265Enable = 0;
#if defined (__IS_SUPPORT_LONGAWAYHOME__) 	
	m_awayHomeEnable = 0;
	m_awayHomeAlarmDuration = 0;
	m_lastAlarmTime = 0;
#endif
	m_bMainStreamHumanOverlay = 0;
	m_bSubStreamHumanOverlay = 1;//默认开启子码流画人形框
	m_bMainStreamDetectAreaOverlay = 0;
	m_bSubStreamDetectAreaOverlay = 0;
	
	pCConfig = NULL;
	m_pActionAlarm = NULL;
	m_iRectNum = 0;
}

CHumanShape::~CHumanShape()
{
	UnInit();
}

int CHumanShape::Init(int nMaxCh, int nMaxAlarmOut)
{
	int result = 0;
	
	pCConfig = CConfig::GetInstance();
	if (NULL == pCConfig)
	{
		AVL_ERR("[CHumanShape:%s]NULL == pCConfig\n", __FUNCTION__);
		UnInit();
		return -1;
	}

	m_pActionAlarm = new CActionAlarm;
	if (m_pActionAlarm == NULL)
	{
		UnInit();
		AVL_ERR("[CHumanShape:%s]new CActionAlarm failed\n", __FUNCTION__);
		return -1;
	}
	
	m_pActionAlarm->Init(m_iChannel, HumanShapeCfgSection, "HumanShapeAction", nMaxCh, nMaxAlarmOut);
	LoadParam();

	result = ZWSys_SetHumanOverlayInfo(m_henc, m_bMainStreamHumanOverlay, m_bSubStreamHumanOverlay);
	if (result)
	{
		AVL_ERR("[CHumanShape:%s]ZWSys_SetHumanOverlayInfo failed(%d)\n", __FUNCTION__, result);
		return result;
	}

	if (m_bEnable
		&& 0xff != m_iPrecision
		&& m_iRectNum > 0)
	{
		TRACE_AVL("\n======startMotion=====启动人形检测==\n");
		result = ZWSys_EncSetPedestrianFiltering(m_henc, 1);
		if (result)
		{
			AVL_ERR("[CHumanShape:%s]ZWSys_EncStartMotionDetection failed(%d)\n", __FUNCTION__, result);
			return result;
		}
		
		TRACE_AVL("\n--------- HumanShapeCfgApply ----设置人形配置---\n");
		result = HumanShapeCfgApply();
		if (result)
		{
			AVL_ERR("[CHumanShape:%s]HumanShapeCfgApply failed(%d)\n", __FUNCTION__, result);
			return result;
		}
	}
	else	
	{
		TRACE_AVL("\n--------- m_iPrecision = 0xff ----关闭人形检测---\n");
		result = ZWSys_EncSetPedestrianFiltering(m_henc, 0);
		if (result)
		{
			AVL_ERR("[CHumanShape:%s]ZWSys_EncStopMotionDetection failed(%d)\n", __FUNCTION__, result);
			return result;
		}	
	}
	
	TRACE_AVL("\n================CHumanShape_Init end======222222222\n");
	return result;
}

int CHumanShape::UnInit()
{
	int result = 0;
	
	TRACE_AVL("\n======UnInit()=====关闭移动侦测==\n");
	result = ZWSys_EncSetPedestrianFiltering(m_henc, 0);
	if (result)
	{
		AVL_ERR("[CHumanShape:%s]ZWSys_EncStopMotionDetection failed(%d)\n", __FUNCTION__, result);

	}

	if (m_pActionAlarm != NULL)
	{
		delete m_pActionAlarm;
		m_pActionAlarm = NULL;
	}
	
	m_iRectNum = 0;
	m_bEnable = 0;
	
	return 0;
}

//##ModelId=49B0E69303CA
int CHumanShape::DealHumanShapeData(int Chn,char *sBuff, int nLen)
{
	//PEOPLE_FRAME_HEAD *pFrameHead = 0;
	if (NULL == sBuff || nLen <= 0)
	{
		return -1;
	}
	
	//这里需要将人形帧保存起来
	//供后续联动动作使用
	//pFrameHead = (PEOPLE_FRAME_HEAD *)sBuff;
	
	//这里需要将人形帧保存起来
	//供后续联动动作使用
//	XTRACE_MUTEX_LOCK(&s_HumanShapeFrameBufLock);
//	memcpy(s_HumanShapeFrameBuf, sBuff, nLen);
//	s_nHumanShapeFrameLen = nLen;
//	XTRACE_MUTEX_UNLOCK(&s_HumanShapeFrameBufLock);

    m_lastTrigger = 1;
	m_nChn = Chn;
	return 0;
}

//获取人形帧数据
//注意使用完成后,第一时间释放
//否则下次获取将会阻塞(死锁)
int CHumanShape::GetHumanShapeFrame(char **pBuff, int *pLen)
{
#if 0
	if (NULL == pBuff || NULL == pLen)
	{
		return -1;
	}
	
	XTRACE_MUTEX_LOCK(&s_HumanShapeFrameBufLock);
	*pBuff = s_HumanShapeFrameBuf;
	*pLen = s_nHumanShapeFrameLen;
#endif
	return 0;
}

//释放人形帧数据
//与GetHumanShapeFrame配合使用
int CHumanShape::ReleaseHumanShapeFrame(char *pBuff)
{
#if 0
	if (s_HumanShapeFrameBuf != pBuff)
	{
        XTRACE_MUTEX_UNLOCK(&s_HumanShapeFrameBufLock);
		return -1;
	}
	XTRACE_MUTEX_UNLOCK(&s_HumanShapeFrameBufLock);
#endif
	return 0;
}

//##ModelId=49BDEC840040
int CHumanShape::SetDetectPrecision(int iGrade)
{
	char key[100];
	int result = 0;

	if (m_iPrecision == iGrade)
	{
		return 0;
	}

	m_iPrecision = iGrade;
	if (m_iPrecision > 5
		&& m_iPrecision != 0xff)
	{
		m_iPrecision = 5;
	}
	else if (m_iPrecision < 0)
	{
		m_iPrecision = 0;
	}

	snprintf(key, sizeof(key), "Precision%02d", m_iChannel);
	result = pCConfig->SetValue(HumanShapeCfgSection, key, m_iPrecision);
	if (result == 0 && pCConfig->m_bReadOnlyCfgFlag == 1)
	{
		m_iPrecision = pCConfig->GetValue(HumanShapeCfgSection, key, 0);
	}
	TRACE_AVL("\n----设置灵敏度=%d--------\n",m_iPrecision);
	
	if (m_bEnable
		&& 0xff != m_iPrecision
		&& m_iRectNum > 0)
	{
		TRACE_AVL("\n======startMotion=====启动人形检测==\n");
		result = ZWSys_EncSetPedestrianFiltering(m_henc, 1);
		if (result)
		{
			AVL_ERR("[CHumanShape:%s]ZWSys_EncStartMotionDetection failed(%d)\n", __FUNCTION__, result);
			return result;
		}
		
		TRACE_AVL("\n--------- HumanShapeCfgApply ----设置人形配置---\n");
		result = HumanShapeCfgApply();
		if (result)
		{
			AVL_ERR("[CHumanShape:%s]HumanShapeCfgApply failed(%d)\n", __FUNCTION__, result);
			return result;
		}
	}
	else	
	{
		TRACE_AVL("\n--------- m_iPrecision = 0xff ----关闭人形检测---\n");
		result = ZWSys_EncSetPedestrianFiltering(m_henc, 0);
		if (result)
		{
			AVL_ERR("[CHumanShape:%s]ZWSys_EncStopMotionDetection failed(%d)\n", __FUNCTION__, result);
			return result;
		}	
	}

	return (int) result;
}

//##ModelId=49BDEC840086
int CHumanShape::GetDetectPrecision()
{
	return (int)m_iPrecision;
}

//##ModelId=49BDEC8400A4
int CHumanShape::SetRegionCfg(tagMotionRegion *pRegionCfg, int nRegionNum)
{
	int result = 0;
    int i = 0;
    char key[30] = {0};

	if (nRegionNum < 0 || nRegionNum > MAX_MOTION_REGION_NUM)
	{
		AVL_ERR("[CHumanShape:%s] nRegionAreas=%d MAX_REGION=%d \n", 
			__FUNCTION__, nRegionNum, MAX_MOTION_REGION_NUM);
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
	//int nRegNum = (nRegionNum<4)?nRegionNum:4;
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
			pCConfig->SetValue(HumanShapeCfgSection, key, 0);
			m_RegionInfos[i].size = 0;
			snprintf(key, sizeof(key), "PolygonArea_%02d_Size", i);
			pCConfig->SetValue(HumanShapeCfgSection, key, 0);
			for (int j = 0; j < 8; j++)
			{
				m_RegionInfos[i].point[j].x = 0;
				snprintf(key, sizeof(key), "PolygonArea_%02d_P%02d_X", i, j);
				pCConfig->SetValue(HumanShapeCfgSection, key, 0);
				m_RegionInfos[i].point[j].y = 0;
				snprintf(key, sizeof(key), "PolygonArea_%02d_P%02d_Y", i, j);
				pCConfig->SetValue(HumanShapeCfgSection, key, 0);
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
		pCConfig->SetValue(HumanShapeCfgSection, key, pRegionCfg[i].bValid);
		m_RegionInfos[i].size = 4;
		snprintf(key, sizeof(key), "PolygonArea_%02d_Size", i);
		pCConfig->SetValue(HumanShapeCfgSection, key, 4);

		m_RegionInfos[i].point[0].x = (pRegionCfg[i].nMotionAreaTopLeftX*10000)/704;
		snprintf(key, sizeof(key), "PolygonArea_%02d_P00_X", i);
		pCConfig->SetValue(HumanShapeCfgSection, key, m_RegionInfos[i].point[0].x);
		m_RegionInfos[i].point[0].y = (pRegionCfg[i].nMotionAreaTopLeftY*10000)/576;
		snprintf(key, sizeof(key), "PolygonArea_%02d_P00_Y", i);
		pCConfig->SetValue(HumanShapeCfgSection, key, m_RegionInfos[i].point[0].y);
		m_RegionInfos[i].point[1].x = (pRegionCfg[i].nMotionAreaBottomRightX*10000)/704;
		snprintf(key, sizeof(key), "PolygonArea_%02d_P01_X", i);
		pCConfig->SetValue(HumanShapeCfgSection, key, m_RegionInfos[i].point[1].x);
		m_RegionInfos[i].point[1].y = (pRegionCfg[i].nMotionAreaTopLeftY*10000)/576;
		snprintf(key, sizeof(key), "PolygonArea_%02d_P01_Y", i);
		pCConfig->SetValue(HumanShapeCfgSection, key, m_RegionInfos[i].point[1].y);
		m_RegionInfos[i].point[2].x = (pRegionCfg[i].nMotionAreaBottomRightX*10000)/704;
		snprintf(key, sizeof(key), "PolygonArea_%02d_P02_X", i);
		pCConfig->SetValue(HumanShapeCfgSection, key, m_RegionInfos[i].point[2].x);
		m_RegionInfos[i].point[2].y = (pRegionCfg[i].nMotionAreaBottomRightY*10000)/576;
		snprintf(key, sizeof(key), "PolygonArea_%02d_P02_Y", i);
		pCConfig->SetValue(HumanShapeCfgSection, key, m_RegionInfos[i].point[2].y);
		m_RegionInfos[i].point[3].x = (pRegionCfg[i].nMotionAreaTopLeftX*10000)/704;
		snprintf(key, sizeof(key), "PolygonArea_%02d_P03_X", i);
		pCConfig->SetValue(HumanShapeCfgSection, key, m_RegionInfos[i].point[3].x);
		m_RegionInfos[i].point[3].y = (pRegionCfg[i].nMotionAreaBottomRightY*10000)/576;
		snprintf(key, sizeof(key), "PolygonArea_%02d_P03_Y", i);
		pCConfig->SetValue(HumanShapeCfgSection, key, m_RegionInfos[i].point[3].y);

		m_iRectNum++;
	}
	if (m_bEnable
		&& 0xff != m_iPrecision
		&& m_iRectNum > 0)
	{
		TRACE_AVL("\n======startMotion=====启动人形检测==\n");
		result = ZWSys_EncSetPedestrianFiltering(m_henc, 1);
		if (result)
		{
			AVL_ERR("[CHumanShape:%s]ZWSys_EncStartMotionDetection failed(%d)\n", __FUNCTION__, result);
			return result;
		}
		
		TRACE_AVL("\n--------- HumanShapeCfgApply ----设置人形配置---\n");
		result = HumanShapeCfgApply();
		if (result)
		{
			AVL_ERR("[CHumanShape:%s]HumanShapeCfgApply failed(%d)\n", __FUNCTION__, result);
			return result;
		}
	}
	else	
	{
		TRACE_AVL("\n--------- m_iPrecision = 0xff ----关闭人形检测---\n");
		result = ZWSys_EncSetPedestrianFiltering(m_henc, 0);
		if (result)
		{
			AVL_ERR("[CHumanShape:%s]ZWSys_EncStopMotionDetection failed(%d)\n", __FUNCTION__, result);
			return result;
		}	
	}

	return (int) 0;
}

int CHumanShape::SetPolygonRegionCfg(struct tagPolygon *pRegionCfg, int nRegionNum)
{
	int result = 0;
    int i = 0;
    char key[30] = {0};
	int nZeorPositionNum = 0;

	if (nRegionNum < 0 || nRegionNum > MAX_MOTION_REGION_NUM)
	{
		AVL_ERR("[CHumanShape:%s] nRegionAreas=%d MAX_REGION=%d \n", 
			__FUNCTION__, nRegionNum, MAX_MOTION_REGION_NUM);
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
	int nRegNum = (nRegionNum<4)?nRegionNum:4;
	for (i=0; i<nRegNum; i++)  //
	{
		if (pRegionCfg[i].bValid != 1)
		{
			//将配置清零
			m_RegionInfos[i].bValid = 0;
			snprintf(key, sizeof(key), "PolygonArea_%02d_Enable", i);
			pCConfig->SetValue(HumanShapeCfgSection, key, 0);
			m_RegionInfos[i].size = 0;
			snprintf(key, sizeof(key), "PolygonArea_%02d_Size", i);
			pCConfig->SetValue(HumanShapeCfgSection, key, 4);
			for (int j = 0; j < 8; j++)
			{
				m_RegionInfos[i].point[j].x = 0;
				snprintf(key, sizeof(key), "PolygonArea_%02d_P%02d_X", i, j);
				pCConfig->SetValue(HumanShapeCfgSection, key, 0);
				m_RegionInfos[i].point[j].y = 0;
				snprintf(key, sizeof(key), "PolygonArea_%02d_P%02d_Y", i, j);
				pCConfig->SetValue(HumanShapeCfgSection, key, 0);
			}
			continue;
		}

		nZeorPositionNum = 0;
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

			//printf("[%s %d] num=%d, i=%d, x=%d, y=%d\n", __FUNCTION__, __LINE__, pRegionCfg[i].size, j, pRegionCfg[i].point[j].x, pRegionCfg[i].point[j].y);
			if (0 == pRegionCfg[i].point[j].x && 0 == pRegionCfg[i].point[j].y)
			{
				nZeorPositionNum++;
			}
		}
		
		if (nZeorPositionNum >= 2 && nZeorPositionNum < pRegionCfg[i].size)
		{
			printf("[%s %d] ZeorPositionNum greater than or equal to 2\n", __FUNCTION__, __LINE__);
			return -1;
		}

		memcpy(&m_RegionInfos[i], &pRegionCfg[i], sizeof(struct tagPolygon));
		snprintf(key, sizeof(key), "PolygonArea_%02d_Enable", i);
		pCConfig->SetValue(HumanShapeCfgSection, key, m_RegionInfos[i].bValid);
		snprintf(key, sizeof(key), "PolygonArea_%02d_Size", i);
		pCConfig->SetValue(HumanShapeCfgSection, key, m_RegionInfos[i].size);

		for (int j = 0; j < m_RegionInfos[i].size; j++)
		{
			snprintf(key, sizeof(key), "PolygonArea_%02d_P%02d_X", i, j);
			pCConfig->SetValue(HumanShapeCfgSection, key, m_RegionInfos[i].point[j].x);
			snprintf(key, sizeof(key), "PolygonArea_%02d_P%02d_Y", i, j);
			pCConfig->SetValue(HumanShapeCfgSection, key, m_RegionInfos[i].point[j].y);
		}
		m_iRectNum++;
	}
	
	TRACE_AVL("\n--------- HumanShapeCfgApply ----设置人形配置---\n");
	result = HumanShapeCfgApply();
	if (result)
	{
		AVL_ERR("[CHumanShape:%s]HumanShapeCfgApply failed(%d)\n", __FUNCTION__, result);
		return result;
	}
	
	return (int) 0;
}

//##ModelId=49BDEC840126
int CHumanShape::GetRegionCfg(tagMotionRegion *pRegionCfg, int nRegionNum)
{
	if (pRegionCfg == NULL)
	{
		AVL_ERR("[CHumanShape:%s] pcfg == NULL\n", __FUNCTION__);
		return 0;
	}
	
	TRACE_AVL("===Into CHumanShape::GetEnableMagicBlock_CGI begin===\n");
	// 将 struct tagPolygon 转换为 struct tagMotionRegion
	//int size = (m_iRectNum>nRegionNum)?nRegionNum:m_iRectNum;
	// 仅支持一个不规则区域
	int size = 1;
	int left, top, right, bottom = 0;
	for (int i = 0; i < size; i++)
	{
		pRegionCfg[i].bValid = m_RegionInfos[i].bValid;
		// 暂时只考虑四边形
		left = right = m_RegionInfos[i].point[0].x;
		top = bottom = m_RegionInfos[i].point[0].y;

		for (int j = 1; j < m_RegionInfos[i].size; j++) 
		{
			if (m_RegionInfos[i].point[j].x < left)
			{
				left = m_RegionInfos[i].point[j].x; 
			}
			if (m_RegionInfos[i].point[j].x > right)
			{
				right = m_RegionInfos[i].point[j].x; 
			}
			if (m_RegionInfos[i].point[j].y < top)
			{
				top = m_RegionInfos[i].point[j].y; 
			}
			if (m_RegionInfos[i].point[j].y > bottom)
			{
				bottom = m_RegionInfos[i].point[j].y;
			}
		}
		pRegionCfg[i].nMotionAreaTopLeftX = (left*704)/10000;
		pRegionCfg[i].nMotionAreaTopLeftY = (top*576)/10000;
		pRegionCfg[i].nMotionAreaBottomRightX = (right*704)/10000;
		pRegionCfg[i].nMotionAreaBottomRightY = (bottom*576)/10000;
	}
	//memcpy(pRegionCfg, m_RegionInfo, sizeof(tagMotionRegion)*MAX_MOTION_REGION_NUM);
	TRACE_AVL("===Into CHumanShape::GetEnableMagicBlock_CGI end===\n");

	return size;
}

int CHumanShape::GetPolygonRegionCfg(struct tagPolygon *pRegionCfg, int nRegionNum)
{
	if (pRegionCfg == NULL)
	{
		AVL_ERR("[CHumanShape:%s] pcfg == NULL\n", __FUNCTION__);
		return 0;
	}

	TRACE_AVL("===Into CHumanShape::GetEnableMagicBlock_CGI begin===\n");
	// 将 struct tagPolygon 转换为 struct tagMotionRegion
	int size = (m_iRectNum>nRegionNum)?nRegionNum:m_iRectNum;
	for (int i = 0; i < size; i++)
	{
		if (m_RegionInfos[i].size > 8 || m_RegionInfos[i].size < 3)
		{
			m_RegionInfos[i].size = 0;
		}
		memcpy(&pRegionCfg[i], &m_RegionInfos[i], sizeof(struct tagPolygon));
	}
    //memcpy(pRegionCfg, m_RegionInfo, sizeof(tagMotionRegion)*MAX_MOTION_REGION_NUM);
	TRACE_AVL("===Into CHumanShape::GetEnableMagicBlock_CGI end===\n");

	return size;
}

//##ModelId=49BDEC840176
int CHumanShape::Enable(int nEnable)
{
	int result = 0;
	char key[100] = {0};

	if (m_bEnable == nEnable)
	{
		return 0;
	}
	m_bEnable = (nEnable != 0);

	snprintf(key, sizeof(key), "Enable%02d", m_iChannel);
	result = pCConfig->SetValue(HumanShapeCfgSection, key, m_bEnable);
	if (result == 0 && pCConfig->m_bReadOnlyCfgFlag == 1)
	{
		m_bEnable = pCConfig->GetValue(HumanShapeCfgSection, key, 1);
	}
	TRACE_AVL("[%s]MotionEnable = %d\n", __FUNCTION__, m_bEnable);

	if (m_bEnable
		&& 0xff != m_iPrecision
		&& m_iRectNum > 0)
	{
		TRACE_AVL("\n--------- HumanShapeCfgApply ----设置人形配置---\n");
		result = HumanShapeCfgApply();
		if (result)
		{
			AVL_ERR("[CHumanShape:%s]HumanShapeCfgApply failed(%d)\n", __FUNCTION__, result);
			return result;
		}

		TRACE_AVL("\n======startMotion=====启动人形检测==\n");
		result = ZWSys_EncSetPedestrianFiltering(m_henc, 1);
		if (result)
		{
			AVL_ERR("[CHumanShape:%s]ZWSys_EncStartMotionDetection failed(%d)\n", __FUNCTION__, result);
			return result;
		}
	}
	else
	{
		TRACE_AVL("\n--------- m_iPrecision = 0xff ----关闭人形检测---\n");
		result = ZWSys_EncSetPedestrianFiltering(m_henc, 0);
		if (result)
		{
			AVL_ERR("[CHumanShape:%s]ZWSys_EncStopMotionDetection failed(%d)\n", __FUNCTION__, result);
			return result;
		}	
	}

	return (int) 0;
}

int CHumanShape::IsEnable()
{
    TRACE_AVL("[%s]m_bEnable = %d\n", __FUNCTION__, m_bEnable);
	return (int)m_bEnable;
}

//##ModelId=49BDEC8401EE
int CHumanShape::SetAlarmCallBack(int AlarmType, int (*CallBack) (struct tagAlertInfo * pAlertInfo, struct tagActionInfo * pActionInfo))
{
	if (CallBack == NULL)
	{
		AVL_ERR("[CHumanShape:%s]CallBack==NULL\n", __FUNCTION__);
		return -1;
	}
	m_AlarmType = AlarmType;
	m_FxnCallBack = CallBack;//callback_AlarmAction
	return (int) 0;
}

//!\brief 启用某个报警联动动作
//!\praram int Action    !      ACTION_SCREEN 屏幕显示
//  !      ACTION_BEEP 声音提示
//  !      ACTION_ALARMOUT 报警输出
//  !      ACTION_SENDTOSERVER 上传报警服务器
//  !      ACTION_EMAIL 联动Email
//  !      ACTION_SMS 联动短信
//!\praram int bEnable false 为禁止联动 true 为开启联动
//对于联动屏幕显示0为不启用，1为联动主显,2为联动辅显,3为联动主显和辅显
//!\return  成功为0，失败为-1
//##ModelId=49BDEFDF0009
int CHumanShape::EnableAction(int Action, int bEnable)
{
	return m_pActionAlarm->EnableAction(Action, bEnable);
}

//!\brief 判断某个报警联动动作是否开启
//!\praram int Action    !      ACTION_SCREEN 屏幕显示
//  !      ACTION_BEEP 声音提示
//  !      ACTION_ALARMOUT 报警输出
//  !      ACTION_SENDTOSERVER 上传报警服务器
//  !      ACTION_EMAIL 联动Email
//  !      ACTION_SMS 联动短信
//!\return false 为禁止联动 true 为开启联动,
//对于联动屏幕显示0为不启用，1为联动主显,2为联动辅显,3为联动主显和辅显
//##ModelId=49BDEFDF0081
int CHumanShape::IsEnableAction(int Action)
{
	return m_pActionAlarm->IsEnableAction(Action);
}

//!\brief 获取某个报警联动录像参数
//!\param dwChannelFlag 联动的通道标记 
//每一位表示联动一个通道 1表示联动通道1 
//2表示联动通道2录像 3表示联动通道1和2录像,以此类推
//!\param nPort 某个报警通道号
//!\return 0表示获取成功，-1表示获取失败
//##ModelId=49B7187602BF
int CHumanShape::GetRecordParam(int *dwChannelFlag)
{
	return m_pActionAlarm->GetRecordParam(dwChannelFlag);
}

//!\brief 设置某个报警联动录像参数
//!\param dwChannelFlag 联动录像通道标记
//!\param nPort 某个报警通道号
//!\return 0表示设置成功 -1表示设置失败
//##ModelId=49B718640232
int CHumanShape::SetRecordParam(int dwChannelFlag)
{
	return m_pActionAlarm->SetRecordParam(dwChannelFlag);
}

//!\brief 
//获取某个报警联动拍照参数,拍照张数和间隔由CCapturePic::GetCfg获取
//!\param dwChannelFlag 存储联动拍照通道标记的缓冲区
//!\param nPort 报警联动输入通道号
//!\return 0表示成功 -1表示失败
//##ModelId=49B5CE9A030D
int CHumanShape::GetSnapshotParam(int *dwChannelFlag)
{
	return m_pActionAlarm->GetSnapshotParam(dwChannelFlag);
}

//!\brief 
//设置某个报警联动拍照参数，拍照张数和拍照间隔由CCapturePi
//c::SetCfg设定
//!\param dwChannelFlag 联动拍照通道标记
//!\param nPort 某个报警通道号
//!\return 0表示设置成功 -1表示失败
//##ModelId=49B5CDFE00BB
int CHumanShape::SetSnapshotParam(int dwChannelFlag)
{
	return m_pActionAlarm->SetSnapshotParam(dwChannelFlag);
}

//!\brief 获取某一某个报警通道联动某一云台的动作参数
//!\param nAction 联动动作 
//*nAction=0表示无动作，1联动预置点，2联动巡航，3联动轨迹
//!\param nIndex 联动预置点/巡航/轨迹的编号
//!\param nChannel 云台对应的通道号
//!\param nPort 某个报警通道号
//!\return 0表示获取成功, -1表示获取失败
//##ModelId=49B718B202DE
int CHumanShape::GetPtzParam(int nChannel, int *nAction, int *nIndex)
{
	return m_pActionAlarm->GetPtzParam(nChannel, nAction, nIndex);
}

//!\brief 设置某个报警通道联动某一个云台的动作
//!\param nAction 联动的动作 0表示无动作 1联动预置点 
//2联动巡航 3联动轨迹
//!\param nIndex 联动的预置点/巡航/轨迹的编号
//!\param nChannel 云台对应的通道号
//!\param nPort 某个报警通道号
//!\return 0表示成功 -1表示失败
//##ModelId=49B718BD0261
int CHumanShape::SetPtzParam(int nChannel, int nAction, int nIndex)
{
	return m_pActionAlarm->SetPtzParam(nChannel, nAction, nIndex);
}

//!\brief 获取某个报警联动报警输出的参数
//!\param dwOutPortFlag 存储输出通道标记的缓冲区
//!\param nPort 某个报警通道号
//!\return 0表示成功 -1表示失败
//##ModelId=49B718DD030D
int CHumanShape::GetDopParam(int *dwOutPortFlag)
{
	return m_pActionAlarm->GetDopParam(dwOutPortFlag);
}

//!\brief 设置某个报警联动报警输出的参数
//!\param dwOutPortFlag 设置输出通道标记
//!\param nPort 某个报警通道号
//!\return 0表示成功 -1表示失败
//##ModelId=49B718ED00AB
int CHumanShape::SetDopParam(int dwOutPortFlag)
{
	return m_pActionAlarm->SetDopParam(dwOutPortFlag);
}

int CHumanShape::SetPushEventIntvCnt(int nPushEventIntv)
{
	return m_pActionAlarm->SetPushEventIntvCnt(m_AlarmType, nPushEventIntv);
}

int CHumanShape::GetPushEventIntvCnt(int *nPushEventIntv)
{
	return m_pActionAlarm->GetPushEventIntvCnt(m_AlarmType, nPushEventIntv);
}

//!\brief 获取移动的时间计划
//!\param tagSchedule* cfg 时间计划结构体
//!\return 成功为0，失败为-1
//##ModelId=49B7164C0058
int CHumanShape::GetSchedule(tagMotionSchedule * cfg)
{
	if (NULL == cfg)
	{
		AVL_ERR("[CHumanShape:%s]cfg==NULL\n", __FUNCTION__);
		return -1;
	}
	memcpy(cfg, &m_ScheduleCfg, sizeof (tagMotionSchedule));
	return 0;
}

//!\brief 设置移动的时间计划
//!\param ttagSchedule cfg 时间计划结构体
//!\return 成功为0，失败为-1
//##ModelId=49B716980242
int CHumanShape::SetSchedule(const tagMotionSchedule * cfg)
{
	char value[255];
	char key_str[30];
	int i, j;

	if (NULL == cfg)
	{
		AVL_ERR("[CHumanShape:%s]cfg==NULL\n", __FUNCTION__);
		return -1;
	}
	if (cfg->bytState > 2)
	{
		AVL_ERR("[CHumanShape:%s]cfg->bytState=%d\n", __FUNCTION__, cfg->bytState);
		return -1;
	}

	//移动计划--
	//ScheduleCfgEnable[ch]=[启用]
	//ScheduleCfg[ch]_[星期]=[48]--[启用][星期][半小时]
	snprintf(key_str, sizeof(key_str), "ScheduleCfgEnable%02d", m_iChannel);
	pCConfig->SetValue(HumanShapeCfgSection, key_str, cfg->bytState);

	for (i = 0; i < MAX_DAYS; i++)
	{
		for (j = 0; j < HY_DVR_MOTIONSEG_DAY; j++)
		{
			value[j] = cfg->dwTimeSeg[i][j] + '0';
		}
		value[j] = 0;
		snprintf(key_str, sizeof(key_str), "ScheduleCfg%02d_%01d", m_iChannel, i);

		pCConfig->SetValue(HumanShapeCfgSection, key_str, value);
		TRACE_AVL("[CHumanShape:%s]cfg->bytState=%d,Write:%s\n", __FUNCTION__, cfg->bytState, value);
	}
	//更新计划
	memcpy(&m_ScheduleCfg, cfg, sizeof (tagMotionSchedule));

	AiScehdule humanShapeSchedule;
	humanShapeSchedule.state = cfg->bytState;
	memcpy(humanShapeSchedule.dwTimeSeg, cfg->dwTimeSeg, SEH_MAX_DAYS * SEH_MAX_SEG_DAYS* sizeof(unsigned char));
	//布放计划传到sdk
	ZWSys_EncSetHumanShapeSchedule(m_henc, &humanShapeSchedule);
	
	return 0;
}

int CHumanShape::SetHumanOverlayInfo(char nMainStreamOverlay, char nSubStreamOverlay)
{
	char key[100] = {0};
	
	snprintf(key, sizeof(key), "MainStreamHumanOverlay%02d", m_iChannel);
	pCConfig->SetValue(HumanShapeCfgSection, key, nMainStreamOverlay);

	snprintf(key, sizeof(key), "SubStreamHumanOverlay%02d", m_iChannel);
	pCConfig->SetValue(HumanShapeCfgSection, key, nSubStreamOverlay);

	m_bMainStreamHumanOverlay = nMainStreamOverlay;
	m_bSubStreamHumanOverlay = nSubStreamOverlay;
	
	ZWSys_SetHumanOverlayInfo(m_henc, nMainStreamOverlay, nSubStreamOverlay);

	return 0;
}

int CHumanShape::GetHumanOverlayInfo(char *nMainStreamOverlay, char *nSubStreamOverlay)
{
	*nMainStreamOverlay = m_bMainStreamHumanOverlay;
	*nSubStreamOverlay = m_bSubStreamHumanOverlay;

	return 0;
}

int CHumanShape::SetDetectAreaOverlayInfo(int nMainStreamOverlay, int nSubStreamOverlay)
{
	char key[100] = {0};
	
	snprintf(key, sizeof(key), "MainStreamDetectAreaOverlay%02d", m_iChannel);
	pCConfig->SetValue(HumanShapeCfgSection, key, nMainStreamOverlay);

	snprintf(key, sizeof(key), "SubStreamDetectAreaOverlay%02d", m_iChannel);
	pCConfig->SetValue(HumanShapeCfgSection, key, nSubStreamOverlay);

	m_bMainStreamDetectAreaOverlay = nMainStreamOverlay;
	m_bSubStreamDetectAreaOverlay = nSubStreamOverlay;
	
	return 0;
}

int CHumanShape::GetDetectAreaOverlayInfo(int *nMainStreamOverlay, int *nSubStreamOverlay)
{
	*nMainStreamOverlay = m_bMainStreamDetectAreaOverlay;
	*nSubStreamOverlay = m_bSubStreamDetectAreaOverlay;

	return 0;
}

int CHumanShape::LoadParam()
{
	unsigned int i, j;
	char key[100] = {0};
	char value[256] = {0};
	tagMotionSchedule *cfg = NULL;
	int nBoardID;
	ZWSys_GetBoardInfo(&nBoardID);
    
    int nForceEnable = 0;//0：默认值，都不操作；1：3MP——>2MP，使能人形人脸；2：2MP——>3MP，禁用人形人脸
    
	snprintf(key, sizeof(key), "Enable%02d", m_iChannel);
	m_bEnable = pCConfig->GetValue(HumanShapeCfgSection, key, 0);

	nForceEnable = pCConfig->GetValue("PUB", "ForceAIEnable", 0);
    if (1 == nForceEnable)
    {
        m_bEnable = 1;
        pCConfig->SetValue(HumanShapeCfgSection, key, m_bEnable);
    }
    else if (2 == nForceEnable)
    {
        m_bEnable = 0;
        pCConfig->SetValue(HumanShapeCfgSection, key, m_bEnable);
    }

	//SigmaStar平台的F23版本不需要人形功能，关闭使能
	if (0x20a6 == nBoardID)
	{
		m_bEnable = 0;
        pCConfig->SetValue(HumanShapeCfgSection, key, m_bEnable);
	}
    
	snprintf(key, sizeof(key), "Ai265Enable%02d", m_iChannel);
	m_bAi265Enable = pCConfig->GetValue(HumanShapeCfgSection, key, 0);
	snprintf(key, sizeof(key), "AiFaceEnable%02d", m_iChannel);
	m_bAiFaceEnable = pCConfig->GetValue(HumanShapeCfgSection, key, 0);
	snprintf(key, sizeof(key), "Precision%02d", m_iChannel);
	m_iPrecision = pCConfig->GetValue(HumanShapeCfgSection, key, DEFAULT_GRADE_HS);
	snprintf(key, sizeof(key), "MainStreamHumanOverlay%02d", m_iChannel);
	m_bMainStreamHumanOverlay = pCConfig->GetValue(HumanShapeCfgSection, key, 0);
	snprintf(key, sizeof(key), "SubStreamHumanOverlay%02d", m_iChannel);
	m_bSubStreamHumanOverlay = pCConfig->GetValue(HumanShapeCfgSection, key, 1);//默认子码流画人形框
	snprintf(key, sizeof(key), "MainStreamDetectAreaOverlay%02d", m_iChannel);
	m_bMainStreamDetectAreaOverlay = pCConfig->GetValue(HumanShapeCfgSection, key, 0);
	snprintf(key, sizeof(key), "SubStreamDetectAreaOverlay%02d", m_iChannel);
	m_bSubStreamDetectAreaOverlay = pCConfig->GetValue(HumanShapeCfgSection, key, 0);

	snprintf(key, sizeof(key), "PolygonArea_00_Enable");
	if (!pCConfig->CheckKeyExist(HumanShapeCfgSection, key))
	{
		// 配置文件无配置, 生成默认配置
		{
			snprintf(key, sizeof(key), "PolygonArea_00_Enable");
			pCConfig->SetValue(HumanShapeCfgSection, key, 1);
			snprintf(key, sizeof(key), "PolygonArea_00_Size");
			pCConfig->SetValue(HumanShapeCfgSection, key, 4);
			snprintf(key, sizeof(key), "PolygonArea_00_P00_X");
			pCConfig->SetValue(HumanShapeCfgSection, key, 0);
			snprintf(key, sizeof(key), "PolygonArea_00_P00_Y");
			pCConfig->SetValue(HumanShapeCfgSection, key, 0);
			snprintf(key, sizeof(key), "PolygonArea_00_P01_X");
			pCConfig->SetValue(HumanShapeCfgSection, key, 9999);
			snprintf(key, sizeof(key), "PolygonArea_00_P01_Y");
			pCConfig->SetValue(HumanShapeCfgSection, key, 0);
			snprintf(key, sizeof(key), "PolygonArea_00_P02_X");
			pCConfig->SetValue(HumanShapeCfgSection, key, 9999);
			snprintf(key, sizeof(key), "PolygonArea_00_P02_Y");
			pCConfig->SetValue(HumanShapeCfgSection, key, 9999);
			snprintf(key, sizeof(key), "PolygonArea_00_P03_X");
			pCConfig->SetValue(HumanShapeCfgSection, key, 0);
			snprintf(key, sizeof(key), "PolygonArea_00_P03_Y");
			pCConfig->SetValue(HumanShapeCfgSection, key, 9999);
			snprintf(key, sizeof(key), "PolygonArea_00_P04_X");
			pCConfig->SetValue(HumanShapeCfgSection, key, 0);
			snprintf(key, sizeof(key), "PolygonArea_00_P04_Y");
			pCConfig->SetValue(HumanShapeCfgSection, key, 0);
			snprintf(key, sizeof(key), "PolygonArea_00_P05_X");
			pCConfig->SetValue(HumanShapeCfgSection, key, 0);
			snprintf(key, sizeof(key), "PolygonArea_00_P05_Y");
			pCConfig->SetValue(HumanShapeCfgSection, key, 0);
			snprintf(key, sizeof(key), "PolygonArea_00_P06_X");
			pCConfig->SetValue(HumanShapeCfgSection, key, 0);
			snprintf(key, sizeof(key), "PolygonArea_00_P06_Y");
			pCConfig->SetValue(HumanShapeCfgSection, key, 0);
			snprintf(key, sizeof(key), "PolygonArea_00_P07_X");
			pCConfig->SetValue(HumanShapeCfgSection, key, 0);
			snprintf(key, sizeof(key), "PolygonArea_00_P07_Y");
			pCConfig->SetValue(HumanShapeCfgSection, key, 0);
		}

		for (i=1; i < MAX_HUMANSHAPE_REGION_NUM; i++)
		{
			snprintf(key, sizeof(key), "PolygonArea_%02d_Enable", i);
			pCConfig->SetValue(HumanShapeCfgSection, key, 0);
			snprintf(key, sizeof(key), "PolygonArea_%02d_Size", i);
			pCConfig->SetValue(HumanShapeCfgSection, key, 0);

			for (j=0; j < 8; j++)
			{
				snprintf(key, sizeof(key), "PolygonArea_%02d_P%02d_X", i, j);
				pCConfig->SetValue(HumanShapeCfgSection, key, 0);
				snprintf(key, sizeof(key), "PolygonArea_%02d_P%02d_Y", i, j);
				pCConfig->SetValue(HumanShapeCfgSection, key, 0);
			}
		}
	}
	
    //区域(将保存的坐标取出来放入全局数组中)
    for (i=0; i < MAX_HUMANSHAPE_REGION_NUM; i++)
    {
		snprintf(key, sizeof(key), "PolygonArea_%02d_Enable", i);
		m_RegionInfos[i].bValid = pCConfig->GetValue(HumanShapeCfgSection, key, 0);
		// 支持3-8边形
		snprintf(key, sizeof(key), "PolygonArea_%02d_Size", i);
		m_RegionInfos[i].size = pCConfig->GetValue(HumanShapeCfgSection, key, 0);
		if ((m_RegionInfos[i].size < 3) || (m_RegionInfos[i].size > 8))
		{
			m_RegionInfos[i].bValid = 0;
			m_RegionInfos[i].size = 0;
		}
		
		for (j = 0; j < sizeof(m_RegionInfos[i].point)/sizeof(m_RegionInfos[i].point[0]); j++)
		{
			snprintf(key, sizeof(key), "PolygonArea_%02d_P%02d_X", i, j);
			m_RegionInfos[i].point[j].x = pCConfig->GetValue(HumanShapeCfgSection, key, 0);
			snprintf(key, sizeof(key), "PolygonArea_%02d_P%02d_Y", i, j);
			m_RegionInfos[i].point[j].y = pCConfig->GetValue(HumanShapeCfgSection, key, 0);
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
	cfg = &m_ScheduleCfg;
	//移动计划--
	//ScheduleCfgEnable[ch]=[启用]
	//ScheduleCfg[ch]_[星期]=[48]--[启用][星期][半小时]
	snprintf(key, sizeof(key), "ScheduleCfgEnable%02d", m_iChannel);
	cfg->bytState = pCConfig->GetValue(HumanShapeCfgSection, key, 0);
	cfg->bytReserve[0] = 0;
	cfg->bytReserve[1] = 0;
	cfg->bytReserve[2] = 0;
	for (i = 0; i < 7; i++)
	{
		snprintf(key, sizeof(key), "ScheduleCfg%02d_%01d", m_iChannel, i);
		pCConfig->GetValue(HumanShapeCfgSection, key, SCHEDULE_TIME_ONE, value, 256);
		for (j = 0; j < 48; j++)
		{
			cfg->dwTimeSeg[i][j] = value[j] - '0';
		}
	}
#if defined (__IS_SUPPORT_LONGAWAYHOME__) 
	snprintf(key, sizeof(key), "Enable");
	m_awayHomeEnable = pCConfig->GetValue(LongAwayCfgSection, key, 0);

	snprintf(key, sizeof(key), "AlarmDuration");
	m_awayHomeAlarmDuration = pCConfig->GetValue(LongAwayCfgSection, key, 8);
#endif

	AiScehdule humanShapeSchedule;
	humanShapeSchedule.state = m_ScheduleCfg.bytState;
	memcpy(humanShapeSchedule.dwTimeSeg, m_ScheduleCfg.dwTimeSeg, SEH_MAX_DAYS * SEH_MAX_SEG_DAYS* sizeof(unsigned char));
	//布放计划传到sdk
	ZWSys_EncSetHumanShapeSchedule(m_henc, &humanShapeSchedule);

	return 0;
}

int CHumanShape::HumanShapeCfgApply()
{
	int i = 0;
	int nZeorPositionNum = 0;
	
	HumanShapeCfgV2 stHumanShapeCfg = {0};
	int HumanShapeRgnCntMax = sizeof(stHumanShapeCfg.rcDetectRegion)/sizeof(stHumanShapeCfg.rcDetectRegion[0]);

	memset(&stHumanShapeCfg, 0, sizeof(stHumanShapeCfg));
	stHumanShapeCfg.nSenstive = m_iPrecision;

	for (i = 0; i < MAX_HUMANSHAPE_REGION_NUM; i++)
	{
		if (0 == m_RegionInfos[i].bValid)
		{
			continue;
		}

		if ((m_RegionInfos[i].size < 3) || (m_RegionInfos[i].size > 8))
		{
			continue;
		}

		stHumanShapeCfg.rcDetectRegion[stHumanShapeCfg.nDetectRegionNum].nValidNum = m_RegionInfos[i].size;
		nZeorPositionNum = 0;
		for (int j = 0; j < m_RegionInfos[i].size; j++)
		{
			stHumanShapeCfg.rcDetectRegion[stHumanShapeCfg.nDetectRegionNum].point[j].x = (m_RegionInfos[i].point[j].x*MAX_HUMAN_SHAPE_REGION_WIDTH)/10000;
			stHumanShapeCfg.rcDetectRegion[stHumanShapeCfg.nDetectRegionNum].point[j].y = (m_RegionInfos[i].point[j].y*MAX_HUMAN_SHAPE_REGION_HEIGHT)/10000;
			
			//printf("[%s %d] num=%d, i=%d, x=%d, y=%d\n", __FUNCTION__, __LINE__, m_RegionInfos[i].size, j, stHumanShapeCfg.rcDetectRegion[stHumanShapeCfg.nDetectRegionNum].point[j].x, stHumanShapeCfg.rcDetectRegion[stHumanShapeCfg.nDetectRegionNum].point[j].y);
			if (0 == stHumanShapeCfg.rcDetectRegion[stHumanShapeCfg.nDetectRegionNum].point[j].x && 0 == stHumanShapeCfg.rcDetectRegion[stHumanShapeCfg.nDetectRegionNum].point[j].y)
			{
				nZeorPositionNum++;
			}
		}

		if (nZeorPositionNum >= 2 && nZeorPositionNum < m_RegionInfos[i].size)
		{
			printf("[%s %d] ZeorPositionNum greater than or equal to 2\n", __FUNCTION__, __LINE__);
			return -1;
		}
		
		stHumanShapeCfg.nDetectRegionNum++;
		if (stHumanShapeCfg.nDetectRegionNum >= HumanShapeRgnCntMax)
		{
			stHumanShapeCfg.nDetectRegionNum = HumanShapeRgnCntMax;
			break;
		}
	}

	int rc = 0; 
	rc = ZWSys_EncSetHumanShapeCfgV2(m_henc, &stHumanShapeCfg);
	return rc;
}

#if defined (__IS_SUPPORT_LONGAWAYHOME__)
int CHumanShape::GetLongAwayEnable(int *nEnable)
{
    if (nEnable != NULL)
    {
    	*nEnable = m_awayHomeEnable;
    }
	return 0;
}

int CHumanShape::SetLongAwayEnable(int nEnable)
{
	char key[24] = {0};

	if (m_awayHomeEnable == nEnable)
	{
		return 0;
	}

	m_awayHomeEnable = nEnable;

	snprintf(key, sizeof(key), "Enable");
	pCConfig->SetValue(LongAwayCfgSection, key, m_awayHomeEnable);

	AVL_INFO("[SetLongAwayEnable] %d\n", m_awayHomeEnable);
	m_lastAlarmTime = 0;
	return 0;
}

int CHumanShape::GetLongAwayDuration(int *AlarmDuration)
{
    if (AlarmDuration != NULL)
    {
    	*AlarmDuration = m_awayHomeAlarmDuration;
    }
	return 0;
}

int CHumanShape::SetLongAwayDuration(int AlarmDuration)
{
	char key[24] = {0};

	if (m_awayHomeAlarmDuration == AlarmDuration)
	{
		return 0;
	}

	m_awayHomeAlarmDuration = AlarmDuration;

	snprintf(key, sizeof(key), "AlarmDuration");
	pCConfig->SetValue(LongAwayCfgSection, key, m_awayHomeAlarmDuration);	

	AVL_INFO("[SetLongAwayDuration] %d\n", m_awayHomeAlarmDuration);
	return 0;
}

int CHumanShape::SetLongAwayPushAlarmTime(time_t stime, time_t etime)
{
    m_PushAlarmSTime = stime;
    m_PushAlarmETime = etime;
	AVL_DBG("[SetLongAwayPushAlarmTime] [%lld] [%lld]\n", stime, etime);
	return 0;
}

int CHumanShape::GetLongAwayPushAlarmTime(time_t *stime, time_t *etime)
{
    *stime = m_PushAlarmSTime;
    *etime = m_PushAlarmETime;

    m_PushAlarmSTime = 0;
    m_PushAlarmETime = 0;

	AVL_DBG("[GetLongAwayPushAlarmTime] [%lld] [%lld]\n", *stime, *etime);
	return 0;
}

int CHumanShape::ProcessLongAway(const struct tm &curr_tm, time_t now)
{
	int result = 0;

	//长久离家
	if (m_awayHomeEnable)
	{
		int curHour = curr_tm.tm_hour;
	    int curMin = curr_tm.tm_min;

		// 转换为分钟
		int curTotalMinutes = curHour * 60 + curMin;
		int startTotalMinutes = 7 * 60;     // 开始时间 7:00
		int endTotalMinutes = 20 * 60;      // 结束时间 20:00

		if (curTotalMinutes < startTotalMinutes || curTotalMinutes >= endTotalMinutes) 
		{		
			m_lastAlarmTime = 0;
	    	return result;
		}

		if (m_bTrigger || !m_bEnable)
		{
			//时间计数清零
			m_lastAlarmTime = 0;
		}
		else
		{
            if (m_lastAlarmTime == 0)
	        {
				m_lastAlarmTime = now;
	            OPEN_LOG_I("[LongAway] StartTime: %02d:%02d\n", curHour, curMin);
	        }

			//算距离上次报警的时间
	        time_t timeSinceLastAlarm = now - m_lastAlarmTime;
	        int minutesSinceLastAlarm = timeSinceLastAlarm / 60;

			//符合条件推送报警
            if (minutesSinceLastAlarm >= m_awayHomeAlarmDuration * 60)
            {
                SetLongAwayPushAlarmTime(m_lastAlarmTime, now);
                result = m_pActionAlarm->CheckAction(m_nChn, ALERT_TYPE_LONG_AWAY, m_FxnCallBack, 0);
                m_lastAlarmTime = now;
                OPEN_LOG_I("[LongAway] PushTime: %02d:%02d result:%d\n", curHour, curMin, result);
            }
        }
    }

	return result;
}
#endif

int CHumanShape::CheckHumanShape()
{
	int result = 0;
	static int nTriggerStatus = 0;
	static int nNoTriggerCount = 0;
	static int bCheckAction = 0;

	m_bTrigger = m_lastTrigger;
	m_lastTrigger = 0;

    time_t now = time(NULL);
    struct tm curr_tm;
    localtime_r(&now, &curr_tm);	
	
#if defined (__IS_SUPPORT_LONGAWAYHOME__) 
	 ProcessLongAway(curr_tm, now);
#endif

	if (m_ScheduleCfg.bytState == 1)
	{
		int day = 0;
		int seg = 0;

		day = curr_tm.tm_wday;
		seg = (curr_tm.tm_hour << 1) + (curr_tm.tm_min >= 30);
		TRACE_AVL("wday=%d hour=%d min=%d---\n", day, curr_tm.tm_hour, curr_tm.tm_min);
		if (!m_ScheduleCfg.dwTimeSeg[day][seg])
		{					//不处理
			ZWSys_EncSetDetectSchedule(m_henc, 0);
			return 0;
		}
	}
	
	if (m_bEnable && m_bTrigger)	//检测到人形了
	{
        ZWSys_EncSetDetectSchedule(m_henc, 1);

        //人形事件联动LCD屏幕
        MPU_LCD_WakeUp();

		nTriggerStatus = 1;
		nNoTriggerCount = 0;

		m_pActionAlarm->UpdateActionIntvCnt(m_AlarmType, 1);
		result = m_pActionAlarm->CheckAction(m_nChn, m_AlarmType, m_FxnCallBack, 0);
		bCheckAction = 0;
		
		return result;
	}
	else
	{
		m_pActionAlarm->UpdateActionIntvCnt(m_AlarmType, 0);
		if (1 == nTriggerStatus)
		{
			nNoTriggerCount++;
		}
		if ((1 == nTriggerStatus) && (nNoTriggerCount >= SEND_END_EVNET_TIME_INTERNAL))//移动停止后发送通知
		{
			nTriggerStatus = 0;
			if (bCheckAction == 0)
	        {
	            result = m_pActionAlarm->CheckAction(m_nChn, m_AlarmType, m_FxnCallBack, 1);
	            bCheckAction = 1;
	        }
		}
	}

	return 0;
}
