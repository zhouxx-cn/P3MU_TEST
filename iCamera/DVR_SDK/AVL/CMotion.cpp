// Copyright (C) 1991 - 2015 Rational Software Corporation
#include <string.h>
#include <time.h>
#include <math.h>
#include "CMotion.h"
#include "CActionAlarm.h"


int CMotion::m_AlarmType = 0;

int CMotion::m_iCustomMode = 0;

ALARMCALLBACK CMotion::m_FxnCallBack = NULL;

#ifdef TRACE_AVL
#undef TRACE_AVL
#define TRACE_AVL 0?1:printf
#endif

// 关于CGI的打印，调试的时候建议打开该宏，在串口上面可以看见设置的点阵信息
//#define CGI_PRINT

//#define MOTION_DEBUG// 以点阵形式输出侦测区域

//##ModelId=49B0EEBF0039
CMotion::CMotion(int nCh, void *hHandle)
{
	m_iChannel = nCh;
	m_AlarmType = 0;

	m_henc = hHandle;
	m_FxnCallBack = NULL;
	m_bisMotion = 0;
	m_lastMotion = 0;
    m_bCgiIsEffective = 1;
	m_bPedestrianFilteringEnable = 0;
	m_Chn = 0;
    memset(m_blockEnable, 0, sizeof(int)*6);
	
	pCConfig = NULL;
	m_pActionAlarm = NULL;
	m_iRectNum = 0;
	memset(m_blockEnableCgi, 0, 6*sizeof(int));
	memset(&m_AudioAlarmScheduleCfg, 0, sizeof(tagAudioAlarmSchedule));
	m_iRectNumCgi = 0;
	m_RegionCnt = 0;

}

CMotion::~CMotion()
{
	UnInit();
}

int CMotion::Init(int nMaxCh, int nMaxAlarmOut)
{
	int i = 0;
	int result = 0;

	m_pMotionDetectToiInfo = new MotionDetectGridToi_t;
	m_pMotionDetectToiInfoCGI = new MotionDetectGridToi_t;

    /* 当前有效的参数存放点 */
    int nEffectRectNum = 0;
    MotionDetectGridToi_t *pEffectMotionDetectToiInfo = new MotionDetectGridToi_t;
    memset(pEffectMotionDetectToiInfo, 0, sizeof(MotionDetectGridToi_t));
    
	pCConfig = CConfig::GetInstance();
	if (NULL == pCConfig)
	{
		AVL_ERR("[CMotion:%s]NULL == pCConfig\n", __FUNCTION__);
		UnInit();
		return -1;
	}

	m_pActionAlarm = new CActionAlarm;
	if (m_pActionAlarm == NULL)
	{
		UnInit();
		AVL_ERR("[CMotion:%s]new CActionAlarm failed\n", __FUNCTION__);
		return -1;
	}
	m_pActionAlarm->Init(m_iChannel, MotionCfgSection, "MotionAction", nMaxCh, nMaxAlarmOut);

	LoadParam();
	
	m_iRectNum = 0;
	memset(m_pRect, 0, MAX_MAGIC_BLOCK_NUM);
	memset(m_pBit_Rect, 0, MAX_MAGIC_BLOCK_NUM/8);
	
	for (i = 0; i < 32; i++)
	{
		if (m_blockEnable[0] & (1 << i))
		{
			m_pRect[i] = 1;
			m_pBit_Rect[i/8] |= 1 << (i&7);
			m_iRectNum++;
		}
		if (m_blockEnable[1] & (1 << i))
		{
			m_pRect[32 + i] = 1;
			m_pBit_Rect[4+i/8] |= 1 << (i&7);
			m_iRectNum++;
		}
		if (m_blockEnable[2] & (1 << i))
		{
			m_pRect[32 * 2 + i] = 1;
			m_pBit_Rect[4*2+i/8] |= 1 << (i&7);
			m_iRectNum++;
		}
		if (m_blockEnable[3] & (1 << i))
		{
			m_pRect[32 * 3 + i] = 1;
			m_pBit_Rect[4*3+i/8] |= 1 << (i&7);
			m_iRectNum++;
		}
		if (m_blockEnable[4] & (1 << i))
		{
			m_pRect[32 * 4 + i] = 1;
			m_pBit_Rect[4*4+i/8] |= 1 << (i&7);
			m_iRectNum++;
		}
		if (m_blockEnable[5] & (1 << i))
		{
			m_pRect[32 * 5 + i] = 1;
			m_pBit_Rect[4*5+i/8] |= 1 << (i&7);
			m_iRectNum++;
		}
	}
	
	
#ifdef MOTION_DEBUG
	TRACE_AVL("\n====打印侦测区域点阵信息======\n");
	print_bit(16*12,16,m_pRect);
	print_char(24, 16, m_pBit_Rect);
#endif

	m_pMotionDetectToiInfo->listOfAreas = (unsigned char *)m_pBit_Rect;
	m_pMotionDetectToiInfo->nBlockNum = m_iRectNum;
	m_pMotionDetectToiInfo->threshold = DEFAULT_THRESHOLD;
	m_pMotionDetectToiInfo->sensitivity = (unsigned short)m_iPrecision;
	
    m_pMotionDetectToiInfoCGI->listOfAreas = (unsigned char *)m_pBit_RectCgi;
    m_pMotionDetectToiInfoCGI->nBlockNum = m_iRectNumCgi;
    m_pMotionDetectToiInfoCGI->threshold = DEFAULT_THRESHOLD;
    m_pMotionDetectToiInfoCGI->sensitivity = (unsigned short)m_iPrecision;
    
	if (0xff == m_iPrecision)	
	{
		TRACE_AVL("\n--------- m_iPrecision = 0xff ----关闭移动侦测---\n");
		result = ZWSys_EncStopMotionDetection(m_henc);
		if (result)
		{
			AVL_ERR("[CMotion:%s]ZWSys_EncStopMotionDetection failed(%d)\n", __FUNCTION__, result);
			return result;
		}
	}
	else
	{
        TRACE_AVL("\n---------m_iRectNum:%d, m_iRectNumCgi:%d ----------\n", m_iRectNum, m_iRectNumCgi);

        /* 如果最后一次是私有设置的，就以私有设置起效 */ 
        if (0 == m_bCgiIsEffective)
        {
            nEffectRectNum = m_iRectNum;
            memcpy(pEffectMotionDetectToiInfo, m_pMotionDetectToiInfo, sizeof(MotionDetectGridToi_t));
        }
        else/* 否则最后一次设置是CGI设置的，就以CGI设置起效 */
        {
            nEffectRectNum = m_iRectNumCgi;
            memcpy(pEffectMotionDetectToiInfo, m_pMotionDetectToiInfoCGI, sizeof(MotionDetectGridToi_t));
        }

		result = ZWSys_EncSetupMotionDetection(m_henc, pEffectMotionDetectToiInfo);
		if (result)
		{
			AVL_ERR("[CMotion:%s]ZWSys_EncSetupMotionDetection failed(%d)\n", __FUNCTION__, result);
			return result;
		}

		if (0 != nEffectRectNum)
		{
			if (m_bMotionEnable)
			{
				TRACE_AVL("\n======startMotion=====启动移动侦测==\n");
				result = ZWSys_EncStartMotionDetection(m_henc);
				if (result)
				{
					AVL_ERR("[CMotion:%s]ZWSys_EncStartMotionDetection failed(%d)\n", __FUNCTION__, result);
					m_bMotionEnable = 0;
					return result;
				}
            }
        }
        else
        {
			result = ZWSys_EncStopMotionDetection(m_henc);
			if (result)
			{
				AVL_ERR("[CMotion:%s]ZWSys_EncStopMotionDetection failed(%d)\n", __FUNCTION__, result);
				return result;
			}
        }

        /* 释放该临时变量 */
        if (pEffectMotionDetectToiInfo != NULL)
        {
            delete pEffectMotionDetectToiInfo;
            pEffectMotionDetectToiInfo = NULL;    
        }

	}

	TRACE_AVL("\n================CMotion_Init end======222222222\n");
	return result;
}

int CMotion::UnInit()
{
	int result;
	TRACE_AVL("\n======UnInit()=====关闭移动侦测==\n");
	result = ZWSys_EncStopMotionDetection(m_henc);
	if (result)
	{
		AVL_ERR("[CMotion:%s]ZWSys_EncStopMotionDetection failed(%d)\n", __FUNCTION__, result);

	}

	if (m_pActionAlarm != NULL)
	{
		delete m_pActionAlarm;

		m_pActionAlarm = NULL;
	}
	m_iRectNum = 0;
	
    m_iRectNumCgi = 0;
	m_bMotionEnable = 0;
	m_RegionCnt = 0;
	
	if (m_pMotionDetectToiInfo != NULL)
		delete m_pMotionDetectToiInfo;
	m_pMotionDetectToiInfo=NULL;
	
	if (m_pMotionDetectToiInfoCGI != NULL)
	    delete m_pMotionDetectToiInfoCGI;
	m_pMotionDetectToiInfoCGI = NULL;    
	
	memset(&m_AudioAlarmScheduleCfg, 0, sizeof(tagAudioAlarmSchedule));
	
	return 0;

}

//##ModelId=49B0E69303CA
int CMotion::DealMotionData(int Chn,char *sBuff, int nLen)
{
	int isMotion;
#if 0
	isMotion = 0;
	//sBuff 为24个字节,每个字节的位为单元格使能状态
	for (i = 0; i < MAX_MAGIC_BLOCK_NUM/8; i++)
	{

		if (sBuff[i] & 0XFF)
		{
			isMotion = 1;
			break;
		}

	}
#endif
	isMotion = 1; /*当前没有宏块检测,默认为1*/

	m_lastMotion |= isMotion;
	m_Chn = Chn;
	//  TRACE("m_bMotionEnable=%d m_bisMotion=%d m_iPrecision=%d\n",__FUNCTION__,m_bMotionEnable,m_bisMotion,m_iPrecision);
	//开始联动处理
	//CheckMotion();//通道管理线程每秒钟会调用CheckMotion
	return 0;
}


//##ModelId=49BDEC840040
int CMotion::SetDetectPrecision(int iGrade)
{
	char key[100];
	int result = 0;

	m_iPrecision = iGrade;

    //目前网页上面都是支持高中低三个等级，对应2.3.4，因此在此做钳位，如果网页上需要增加灵敏度等级，此处需要对应修改。
	if (m_iPrecision >= 0 && m_iPrecision < 2)
		m_iPrecision = 2;

	if (m_iPrecision > 4)
		m_iPrecision = 4;

	snprintf(key, sizeof(key), "MotionPrecision%02d", m_iChannel);
	result = pCConfig->SetValue(MotionCfgSection, key, m_iPrecision);
	if (result == 0 && pCConfig->m_bReadOnlyCfgFlag == 1)
	{
		m_iPrecision = pCConfig->GetValue(MotionCfgSection, key, 0);
	}

	TRACE_AVL("\n----设置灵敏度=%d--------\n",m_iPrecision);
	m_pMotionDetectToiInfo->sensitivity = (unsigned short)m_iPrecision;
	m_pMotionDetectToiInfoCGI->sensitivity = (unsigned short)m_iPrecision;

	if (0xff == m_iPrecision)	
	{
		TRACE_AVL("\n--------- m_iPrecision = 0xff ----关闭移动侦测---\n");
		result = ZWSys_EncStopMotionDetection(m_henc);
		if (result)
		{
			AVL_ERR("[CMotion:%s]ZWSys_EncStopMotionDetection failed(%d)\n", __FUNCTION__, result);
			return result;
		}
	}
	else
	{
		if(0 == m_bCgiIsEffective)
		{
			TRACE_AVL("\n--------- ZWSys_EncSetupMotionDetection ----设置移动侦测---\n");
			result = ZWSys_EncSetupMotionDetection(m_henc, m_pMotionDetectToiInfo);
			if (result)
			{
				AVL_ERR("[CMotion:%s]ZWSys_EncSetupMotionDetection failed(%d)\n", __FUNCTION__, result);
				return result;
			}
			if (0 == m_iRectNum)
			{
				result = ZWSys_EncStopMotionDetection(m_henc);
				if (result)
				{
					AVL_ERR("[CMotion:%s]ZWSys_EncStopMotionDetection failed(%d)\n", __FUNCTION__, result);
					return result;
				}
			}
		}
		else
			{
		        TRACE_AVL("\n--------- ZWSys_EncSetupMotionDetection ----设置移动侦测CGI---\n");
				result = ZWSys_EncSetupMotionDetection(m_henc, m_pMotionDetectToiInfoCGI);
				if (result)
				{
					AVL_ERR("[CMotion:%s]ZWSys_EncSetupMotionDetection failed(%d)\n", __FUNCTION__, result);
					return result;
				}
				if (0 == m_iRectNumCgi)
				{
					result = ZWSys_EncStopMotionDetection(m_henc);
					if (result)
					{
						AVL_ERR("[CMotion:%s]ZWSys_EncStopMotionDetection failed(%d)\n", __FUNCTION__, result);
						return result;
					}
				}
			}
	}

	return (int) result;
}

//##ModelId=49BDEC840086
int CMotion::GetDetectPrecision()
{
	return (int) m_iPrecision;
}

//##ModelId=49BDEC8400A4
int CMotion::EnableMagicBlock(unsigned char *pMagicBlock, int nBlockNum)
{
	int i = 0;	
	int result = 0;
	int curr_num = 0;
	unsigned char nowMagicBlock[16 * 12] = {0};
	int			  nowBlockNum = 0;
	char key[100] = {0};

	//printf("my add: ----EnableMagicBlock--nBlockNum %d--\n", nBlockNum);
	if (nBlockNum < 0 || nBlockNum > MAX_MAGIC_BLOCK_NUM)
	{
		AVL_ERR("[CMotion:%s] nBlockNum=%d MAX_BLOCK=%d \n", __FUNCTION__, nBlockNum, MAX_MAGIC_BLOCK_NUM);
		return -1;
	}
	if (pMagicBlock == NULL)
	{
		AVL_ERR("[CMotion:%s] pMagicBlock == NULL \n", __FUNCTION__);
		return -1;
	}

	//如果当前已是私有宏块且区域没有改变则不设置
	nowBlockNum = GetEnableMagicBlock(nowMagicBlock, 16 * 12);
	if ((0 == m_bCgiIsEffective) && (nowBlockNum == nBlockNum) && (0 == memcmp((const char*)nowMagicBlock, (const char*)pMagicBlock, 16*12)))
	{
		return 0;
	}

	m_iRectNum = 0;

    memset(m_blockEnable, 0 , 6*sizeof(int));
	memset(m_pRect, 0, MAX_MAGIC_BLOCK_NUM);
	memset(m_pBit_Rect, 0, MAX_MAGIC_BLOCK_NUM/8);
	for (i = 0; i < nBlockNum; i++)
	{
		curr_num = pMagicBlock[i];
		if (curr_num < 0 || curr_num >= MAX_MAGIC_BLOCK_NUM)
		{
			continue;
		}
		m_pRect[curr_num] = 1;      // 标记宏块，放入全局数组中
		// i >> 5, 即i/32，m_blockEnable的下标
		m_blockEnable[curr_num >> 5] |= 1 << (curr_num & 31);
		m_pBit_Rect[curr_num >> 3] |= 1 << (curr_num & 7);
		m_iRectNum++;
	}

	snprintf(key, sizeof(key), "MotionMagicBlock%02d_%02d", m_iChannel, 0);
	pCConfig->SetValue(MotionCfgSection, key, m_blockEnable[0]);
	snprintf(key, sizeof(key), "MotionMagicBlock%02d_%02d", m_iChannel, 1);
	pCConfig->SetValue(MotionCfgSection, key, m_blockEnable[1]);
	snprintf(key, sizeof(key), "MotionMagicBlock%02d_%02d", m_iChannel, 2);
	pCConfig->SetValue(MotionCfgSection, key, m_blockEnable[2]);
	snprintf(key, sizeof(key), "MotionMagicBlock%02d_%02d", m_iChannel, 3);
	pCConfig->SetValue(MotionCfgSection, key, m_blockEnable[3]);
	snprintf(key, sizeof(key), "MotionMagicBlock%02d_%02d", m_iChannel, 4);
	pCConfig->SetValue(MotionCfgSection, key, m_blockEnable[4]);
	snprintf(key, sizeof(key), "MotionMagicBlock%02d_%02d", m_iChannel, 5);
	pCConfig->SetValue(MotionCfgSection, key, m_blockEnable[5]);

    m_bCgiIsEffective = 0;
	pCConfig->SetValue(MotionCfgSection, "MotionCurrentCgiIsEffective", m_bCgiIsEffective);

	m_pMotionDetectToiInfo->listOfAreas = (unsigned char *)m_pBit_Rect;
	m_pMotionDetectToiInfo->nBlockNum = m_iRectNum;
	m_pMotionDetectToiInfo->threshold = DEFAULT_THRESHOLD;
	m_pMotionDetectToiInfo->sensitivity = (unsigned short)m_iPrecision;

#ifdef MOTION_DEBUG
	print_bit(16*12,16,m_pRect);
	print_char(24, 16, m_pBit_Rect);
	TRACE_AVL("\n---EnableMagicBlock()---Motion->nBlockNUM=%d-----Motion->sensitivity=%d---\n",m_iRectNum,m_iPrecision);
#endif
	if (0xff == m_iPrecision)	
	{
		TRACE_AVL("\n--------- m_iPrecision = 0xff ----关闭移动侦测---\n");
		result = ZWSys_EncStopMotionDetection(m_henc);
		if (result)
		{
			AVL_ERR("[CMotion:%s]ZWSys_EncStopMotionDetection failed(%d)\n", __FUNCTION__, result);
			return result;
		}
	}
	else
	{
		if (0 != m_iRectNum)
		{
			TRACE_AVL("\n--------- ZWSys_EncSetupMotionDetection ----设置移动侦测---\n");
			result = ZWSys_EncSetupMotionDetection(m_henc, m_pMotionDetectToiInfo);
			if (result)
			{
				AVL_ERR("[CMotion:%s]ZWSys_EncSetupMotionDetection failed(%d)\n", __FUNCTION__, result);
				return result;
			}
		}else{
			result = ZWSys_EncStopMotionDetection(m_henc);
			if (result)
			{
				AVL_ERR("[CMotion:%s]ZWSys_EncStopMotionDetection failed(%d)\n", __FUNCTION__, result);
				return result;
			}
		}
	}
	
	if (m_bMotionEnable)
	{
		if (0 != m_iRectNum)
		{
			TRACE_AVL("\n---CMotion::EnableMagicBlock---开启侦测----\n");
			result = ZWSys_EncStartMotionDetection(m_henc);
			if (result)
			{
				AVL_ERR("[CMotion:%s]ZWSys_EncStartMotionDetection failed(%d)\n", __FUNCTION__, result);
				return result;
			}
		}else{
			result = ZWSys_EncStopMotionDetection(m_henc);
			if (result)
			{
				AVL_ERR("[CMotion:%s]ZWSys_EncStopMotionDetection failed(%d)\n", __FUNCTION__, result);
				return result;
			}
		}
	}
	else
	{
        result = ZWSys_EncStopMotionDetection(m_henc);
        if (result)
        {
            AVL_ERR("[CMotion:%s]ZWSys_EncStopMotionDetection failed(%d)\n", __FUNCTION__, result);
            return result;
        }
	}	

	return (int) 0;
}

//##ModelId=49BDEC840126
int CMotion::GetEnableMagicBlock(unsigned char *pMagicBlock, int nMaxBlockNum)
{
	int i;

	int cnt;

	if (pMagicBlock == NULL)
	{
		AVL_ERR("[CMotion:%s] pMagicBlock == NULL\n", __FUNCTION__);
		return m_iRectNum;
	}
	if (nMaxBlockNum < m_iRectNum)
	{
		AVL_ERR("[CMotion:%s] nMaxBlockNum=%d MAX_BLOCK=%d \n", __FUNCTION__, nMaxBlockNum, MAX_MAGIC_BLOCK_NUM);
		return -1;
	}

	cnt = 0;
	memset(pMagicBlock, 0, nMaxBlockNum);
	for (i = 0; i < 32; i++)
	{
		if (m_blockEnable[0] & (1 << i))
		{
			pMagicBlock[cnt++] = i;
		}
		if (m_blockEnable[1] & (1 << i))
		{
			pMagicBlock[cnt++] = i + 32;
		}
		if (m_blockEnable[2] & (1 << i))
		{
			pMagicBlock[cnt++] = i + 32 * 2;
		}
		if (m_blockEnable[3] & (1 << i))
		{
			pMagicBlock[cnt++] = i + 32 * 3;
		}
		if (m_blockEnable[4] & (1 << i))
		{
			pMagicBlock[cnt++] = i + 32 * 4;
		}
		if (m_blockEnable[5] & (1 << i))
		{
			pMagicBlock[cnt++] = i + 32 * 5;
		}
	}

#ifdef CGI_PRINT
    for (i=0; i<6; i++)
        printf("Get: m_blockEnable[%d] = %d\n", i, m_blockEnable[i]);
#endif

	return cnt;
}

/* 判断一个值是否是浮点数 */
static int IsFloat(float value)
{
    float c = 0.0;
    int b = 0;
    b = value;
    c = b;

    if (value != c)
        return 1;
    else
        return 0;
}

//##ModelId=49BDEC8400A4
int CMotion::EnableMagicBlock_CGI(tagMotionRegion * pcfg, int nRegionAreas)
{
	int result = 0;
	int curr_num = 0;
    int nCount = 0;
    int nTScopeX = 0; 
    int nTScopeY = 0;
    float nBScopeX = 0.0;
    float nBScopeY = 0.0;

    int i = 0;
    int x = 0;
    int y = 0;

    char key[64] = {0};
    
	unsigned char byMagicBlock[16 * 12] = {0};   // 将整幅图片划成16 * 12 = 192个宏块

    //printf("my add: ----EnableMagicBlock_CGI----\n");
	if (nRegionAreas < 0 || nRegionAreas > MAX_MOTION_REGION_NUM)
	{
		AVL_ERR("[CMotion:%s] nRegionAreas=%d MAX_REGION=%d \n", __FUNCTION__, nRegionAreas, MAX_MOTION_REGION_NUM);
		return -1;
	}
	
	if (pcfg == NULL)
	{
		AVL_ERR("[CMotion:%s] pcfg == NULL \n", __FUNCTION__);
		return -1;
	}
	
    /* 将坐标框选出对应的区域转化为宏块 */
    for (i=0; i<nRegionAreas; i++)  // CGI添加的区域个数(IE端限制为4个，中间层扩展为16个)
    {
#ifdef CGI_PRINT
        printf("Set Area_%d: %d [(%d,%d); (%d,%d)]\n", i, pcfg[i].bValid,
            pcfg[i].nMotionAreaTopLeftX,pcfg[i].nMotionAreaTopLeftY,
            pcfg[i].nMotionAreaBottomRightX,pcfg[i].nMotionAreaBottomRightY);
#endif            
		if ((pcfg[i].bValid == 0) || ((pcfg[i].bValid != 0) && (pcfg[i].bValid != 
		1)) || ((pcfg[i].nMotionAreaTopLeftX >= pcfg[i].nMotionAreaBottomRightX) || 
			(pcfg[i].nMotionAreaTopLeftY >= pcfg[i].nMotionAreaBottomRightY)))
		{
			//将配置清零
			snprintf(key, sizeof(key), "MotionArea_%d_Enable", i);
			pCConfig->SetValue(MotionCfgSection, key, 0);	 
			snprintf(key, sizeof(key), "MotionArea_%d_TopLeftX", i);
			pCConfig->SetValue(MotionCfgSection, key, 0);
			snprintf(key, sizeof(key), "MotionArea_%d_TopLeftY", i);
			pCConfig->SetValue(MotionCfgSection, key, 0);
			snprintf(key, sizeof(key), "MotionArea_%d_BottomRightX", i);
			pCConfig->SetValue(MotionCfgSection, key, 0);
			snprintf(key, sizeof(key), "MotionArea_%d_BottomRightY", i);
			pCConfig->SetValue(MotionCfgSection, key, 0);

            continue;
        }

    	/* 侦测区域,坐标范围位于[704, 576] */
    	if (pcfg[i].nMotionAreaTopLeftX > 703)
    	{
    		AVL_ERR("nMotionAreaTopLeftX = %d is overflow\n", pcfg[i].nMotionAreaTopLeftX);
    		pcfg[i].nMotionAreaTopLeftX = 703;
    	}
		else
	    	if (pcfg[i].nMotionAreaTopLeftX < 0)
	    	{
	    		AVL_ERR("nMotionAreaTopLeftX = %d is overflow\n", pcfg[i].nMotionAreaTopLeftX);
	    		pcfg[i].nMotionAreaTopLeftX = 0;
	    	}
    	if (pcfg[i].nMotionAreaTopLeftY > 575)
    	{
    		AVL_ERR("nMotionAreaTopLeftY = %d is overflow\n", pcfg[i].nMotionAreaTopLeftY);
    		pcfg[i].nMotionAreaTopLeftY = 575;
    	}
		else
	    	if (pcfg[i].nMotionAreaTopLeftY < 0)
	    	{
	    		AVL_ERR("nMotionAreaTopLeftY = %d is overflow\n", pcfg[i].nMotionAreaTopLeftY);
	    		pcfg[i].nMotionAreaTopLeftY = 0;
	    	}
    	if (pcfg[i].nMotionAreaBottomRightX > 703)
    	{
    		AVL_ERR("nMotionAreaBottomRightX = %d is overflow\n", pcfg[i].nMotionAreaBottomRightX);
    		pcfg[i].nMotionAreaBottomRightX = 703;
    	}
		else
	    	if (pcfg[i].nMotionAreaBottomRightX < 0)
	    	{
	    		AVL_ERR("nMotionAreaBottomRightX = %d is overflow\n", pcfg[i].nMotionAreaBottomRightX);
	    		pcfg[i].nMotionAreaBottomRightX = 0;
	    	}
    	if (pcfg[i].nMotionAreaBottomRightY > 575)
    	{
    		AVL_ERR("nMotionAreaBottomRightY = %d is overflow\n", pcfg[i].nMotionAreaBottomRightY);
    		pcfg[i].nMotionAreaBottomRightY = 575;
    	}
		else
	    	if (pcfg[i].nMotionAreaBottomRightY < 0)
	    	{
	    		AVL_ERR("nMotionAreaBottomRightY = %d is overflow\n", pcfg[i].nMotionAreaBottomRightY);
	    		pcfg[i].nMotionAreaBottomRightY = 0;
	    	}

        snprintf(key, sizeof(key), "MotionArea_%d_Enable", i);
        pCConfig->SetValue(MotionCfgSection, key, pcfg[i].bValid);   
        snprintf(key, sizeof(key), "MotionArea_%d_TopLeftX", i);
        pCConfig->SetValue(MotionCfgSection, key, pcfg[i].nMotionAreaTopLeftX);
        snprintf(key, sizeof(key), "MotionArea_%d_TopLeftY", i);
        pCConfig->SetValue(MotionCfgSection, key, pcfg[i].nMotionAreaTopLeftY);
        snprintf(key, sizeof(key), "MotionArea_%d_BottomRightX", i);
        pCConfig->SetValue(MotionCfgSection, key, pcfg[i].nMotionAreaBottomRightX);
        snprintf(key, sizeof(key), "MotionArea_%d_BottomRightY", i);
        pCConfig->SetValue(MotionCfgSection, key, pcfg[i].nMotionAreaBottomRightY);
        
    	/* X[0~15]  Y[0~11] */
        nTScopeX = pcfg[i].nMotionAreaTopLeftX * 16 / 704;
        nTScopeY = pcfg[i].nMotionAreaTopLeftY * 12 / 576;
        nBScopeX = pcfg[i].nMotionAreaBottomRightX * 16.0 / 704.0;
        nBScopeY = pcfg[i].nMotionAreaBottomRightY * 12.0 / 576.0;
        
        /* 如果超出边界部分，则向上取整，否则不变 */
        if (IsFloat(nTScopeX)) nTScopeX = ceil(nTScopeX);
        if (IsFloat(nTScopeY)) nTScopeY = ceil(nTScopeY);
        if (IsFloat(nBScopeX)) nBScopeX = ceil(nBScopeX);
        if (IsFloat(nBScopeY)) nBScopeY = ceil(nBScopeY);

        if (nTScopeX > 16.0) nTScopeX = 16.0;
        if (nTScopeY > 12.0) nTScopeY = 12.0;
        if (nBScopeX > 16.0) nBScopeX = 16.0;
        if (nBScopeY > 12.0) nBScopeY = 12.0;
        
        int j = 0, k = 0, flag = 0;
        for (y=nTScopeY; y<(int)nBScopeY; y++)      // 一共(nBScopeY-nTScopeY)行 
        {
            // 将序列号放到数组中，数组中存放的是已经标记了的宏块的序列号(x行y列)
            for (x=nTScopeX; x<(int)nBScopeX; x++)  // 一共(nBScopeX-nTScopeX)列
            {
                // 在数组byMagicBlock里面查找是否在已有的下标，如果存在，则表示该宏块已经被标记，跳过该标记，执行下一个宏块
                k = y * 16 + x;
                for (j=0; j<nCount; j++)
                {
                    if (byMagicBlock[j] == k)
                        flag = 1;
                }
                if (flag == 1)
                {
                    flag = 0;
                    continue;
                }
                byMagicBlock[nCount++] = k;
            }
        }
#ifdef CGI_PRINT
        printf("nTScopeX=%d, nTScopeY=%d, nBScopeX=%d, nBScopeY=%d, nCount=%d\n", nTScopeX, nTScopeY, (int)nBScopeX, (int)nBScopeY, nCount);
#endif
    }   
	
	m_bCgiIsEffective = 1;
	pCConfig->SetValue(MotionCfgSection, "MotionCurrentCgiIsEffective", m_bCgiIsEffective);

    memcpy(m_RegionInfo, pcfg, sizeof(tagMotionRegion)*MAX_MOTION_REGION_NUM);

    m_RegionCnt = nCount;

    m_iRectNumCgi = 0;

    memset(m_blockEnableCgi, 0, 6*sizeof(int));
	memset(m_pRectCgi, 0, MAX_MAGIC_BLOCK_NUM);
	memset(m_pBit_RectCgi, 0, MAX_MAGIC_BLOCK_NUM/8);
	for (i = 0; i < m_RegionCnt; i++)
	{
	    /* pMagicBlock数组中存放的是已经被标记了的宏块的序列号[0, 191] */
		curr_num = byMagicBlock[i];   // 当前宏块的标号(从0开始，对应数组标号)
		if (curr_num < 0 || curr_num >= MAX_MAGIC_BLOCK_NUM)
		{
			continue;
		}
		m_pRectCgi[curr_num] = 1;      // 标记宏块，放入全局数组中
		// i >> 5, 即i/32，m_blockEnable的下标
		m_blockEnableCgi[curr_num >> 5] |= (1 << (curr_num & 31));
        // i >> 3, 即i/8 ，m_pBit_Rect的下标
		m_pBit_RectCgi[curr_num >> 3] |= (1 << (curr_num & 7));
		m_iRectNumCgi++;
	}

#ifdef CGI_PRINT
    printf("******************************Set CGI*******************************");
    for (i=0; i<192; i++)
    {
        if (i%16 == 0)
            putchar('\n');
        if (m_pRectCgi[i] == 1)
            printf("1");
        else
            printf("0");
    }
    printf("\n****************************End CGI*******************************\n");
#endif

    //for (i=0; i<6; i++)
    //    printf("CGI => Set: m_blockEnableCgi[%d] = %d\n", i, m_blockEnableCgi[i]);

    printf("CGI => Set: m_iRectNumCgi = %d\n", m_iRectNumCgi);

	m_pMotionDetectToiInfoCGI->listOfAreas = (unsigned char *)m_pBit_RectCgi;
	m_pMotionDetectToiInfoCGI->nBlockNum = m_iRectNumCgi;
	m_pMotionDetectToiInfoCGI->threshold = DEFAULT_THRESHOLD;
	m_pMotionDetectToiInfoCGI->sensitivity = (unsigned short)m_iPrecision;

	if (0xff == m_iPrecision)	
	{
		TRACE_AVL("\n--------- m_iPrecision = 0xff ----关闭移动侦测CGI---\n");
		result = ZWSys_EncStopMotionDetection(m_henc);
		if (result)
		{
			AVL_ERR("[CMotion:%s]ZWSys_EncStopMotionDetection failed(%d)\n", __FUNCTION__, result);
			return result;
		}
	}
	else
	{
		if (0 != m_iRectNumCgi)
		{
			TRACE_AVL("\n--------- ZWSys_EncSetupMotionDetection ----设置移动侦测CGI---\n");
			result = ZWSys_EncSetupMotionDetection(m_henc, m_pMotionDetectToiInfoCGI);
			if (result)
			{
				AVL_ERR("[CMotion:%s]ZWSys_EncSetupMotionDetection failed(%d)\n", __FUNCTION__, result);
				return result;
			}
		}else{
			result = ZWSys_EncStopMotionDetection(m_henc);
			if (result)
			{
				AVL_ERR("[CMotion:%s]ZWSys_EncStopMotionDetection failed(%d)\n", __FUNCTION__, result);
				return result;
			}
		}
	}
	
	if (m_bMotionEnable)
	{
		if (0 != m_iRectNumCgi)
		{
			TRACE_AVL("\n---CMotion::EnableMagicBlock---开启侦测CGI----\n");
			result = ZWSys_EncStartMotionDetection(m_henc);
			if (result)
			{
				AVL_ERR("[CMotion:%s]ZWSys_EncStartMotionDetection failed(%d)\n", __FUNCTION__, result);
				return result;
			}
		}else{
			result = ZWSys_EncStopMotionDetection(m_henc);
			if (result)
			{
				AVL_ERR("[CMotion:%s]ZWSys_EncStopMotionDetection failed(%d)\n", __FUNCTION__, result);
				return result;
			}
		}
	}
	else
	{
        result = ZWSys_EncStopMotionDetection(m_henc);
        if (result)
        {
            AVL_ERR("[CMotion:%s]ZWSys_EncStopMotionDetection failed(%d)\n", __FUNCTION__, result);
            return result;
        }
	}	

	return (int) 0;
}

//##ModelId=49BDEC840126
int CMotion::GetEnableMagicBlock_CGI(tagMotionRegion * pcfg, int nRegionAreas)
{    
    if (pcfg == NULL)
	{
		AVL_ERR("[CMotion:%s] pcfg == NULL\n", __FUNCTION__);
		return m_iRectNumCgi;
	}
//  printf("===Into CMotion::GetEnableMagicBlock_CGI begin===\n");

    memcpy(pcfg, m_RegionInfo, sizeof(tagMotionRegion)*MAX_MOTION_REGION_NUM);

//  printf("===Into CMotion::GetEnableMagicBlock_CGI end===\n");

	return m_iRectNumCgi;
}

//##ModelId=49BDEC840176
int CMotion::Enable(int nEnable)
{

#if defined (__T41L__)
	//nEnable = 0;
#endif

	int result = 0;

	char key[100] = {0};


	if (m_bMotionEnable == nEnable)
	{
		return 0;
	}


	m_pMotionDetectToiInfo->listOfAreas = (unsigned char *)m_pBit_Rect;
	m_pMotionDetectToiInfo->nBlockNum = m_iRectNum;
	m_pMotionDetectToiInfo->threshold = DEFAULT_THRESHOLD;
	m_pMotionDetectToiInfo->sensitivity = (unsigned short)m_iPrecision;

	m_pMotionDetectToiInfoCGI->listOfAreas = (unsigned char *)m_pBit_RectCgi;
	m_pMotionDetectToiInfoCGI->nBlockNum = m_iRectNumCgi;
	m_pMotionDetectToiInfoCGI->threshold = DEFAULT_THRESHOLD;
	m_pMotionDetectToiInfoCGI->sensitivity = (unsigned short)m_iPrecision;

	m_bMotionEnable = (nEnable != 0);
	snprintf(key, sizeof(key), "MotionEnable%02d", m_iChannel);
	result = pCConfig->SetValue(MotionCfgSection, key, m_bMotionEnable);
	if (result == 0 && pCConfig->m_bReadOnlyCfgFlag == 1)
	{
		m_bMotionEnable = pCConfig->GetValue(MotionCfgSection, key, 0);
	}

	if (0xff == m_iPrecision)	
	{
		TRACE_AVL("\n--------- m_iPrecision = 0xff ----关闭移动侦测---\n");
		result = ZWSys_EncStopMotionDetection(m_henc);
		if (result)
		{
			AVL_ERR("[CMotion:%s]ZWSys_EncStopMotionDetection failed(%d)\n", __FUNCTION__, result);
			return result;
		}
	}
	else
	{
		if (0 != m_iRectNum)
		{	
            TRACE_AVL("\n--------- ZWSys_EncSetupMotionDetection ----设置移动侦测---\n");
			result = ZWSys_EncSetupMotionDetection(m_henc, m_pMotionDetectToiInfo);
			if (result)
			{
				AVL_ERR("[CMotion:%s]ZWSys_EncSetupMotionDetection failed(%d)\n", __FUNCTION__, result);
				return result;
			}		
		}

		if (0 != m_iRectNumCgi)
		{
            TRACE_AVL("\n--------- ZWSys_EncSetupMotionDetection ----设置移动侦测CGI---\n");
			result = ZWSys_EncSetupMotionDetection(m_henc, m_pMotionDetectToiInfoCGI);
			if (result)
			{
				AVL_ERR("[CMotion:%s]ZWSys_EncSetupMotionDetection failed(%d)\n", __FUNCTION__, result);
				return result;
			}		
		}

        if ( !m_iRectNum && !m_iRectNumCgi )
        {
			result = ZWSys_EncStopMotionDetection(m_henc);
			if (result)
			{
				AVL_ERR("[CMotion:%s]ZWSys_EncStopMotionDetection failed(%d)\n", __FUNCTION__, result);
				return result;
			}
        }
	}

	if (m_bMotionEnable)
	{
		if (0 != m_iRectNum || 0 != m_iRectNumCgi)
		{
			TRACE_AVL("\n---------CMotion::Enable----开启移动侦测---\n");
			result = ZWSys_EncStartMotionDetection(m_henc);
			if (result)
			{
				AVL_ERR("[CMotion:%s]ZWSys_EncStartMotionDetection failed(%d)\n", __FUNCTION__, result);
				return result;
			}
		}else{
			result = ZWSys_EncStopMotionDetection(m_henc);
			if (result)
			{
				AVL_ERR("[CMotion:%s]ZWSys_EncStopMotionDetection failed(%d)\n", __FUNCTION__, result);
				return result;
			}
		}
	}
	else
	{
    	TRACE_AVL("\n---------CMotion::Enable----关闭移动侦测---\n");
		result = ZWSys_EncStopMotionDetection(m_henc);
		if (result)
		{
			AVL_ERR("[CMotion:%s]ZWSys_EncStopMotionDetection failed(%d)\n", __FUNCTION__, result);
			return result;
		}
	}
	
//  printf("[%s]MotionEnable = %d\n", __FUNCTION__, m_bMotionEnable);

	return (int) 0;
}

int CMotion::SetAudioAlarmSchedule(tagAudioAlarmSchedule *stSchedule)
{
	char value[64] = {0};
	char key_str[64] = {0};
	int i, j;

	if (NULL == stSchedule)
	{
		AVL_ERR("[CMotion:%s]cfg==NULL\n", __FUNCTION__);
		return -1;
	}
	
	if (stSchedule->bytState > 2)
	{
		AVL_ERR("[CMotion:%s]cfg->bytState=%d\n", __FUNCTION__, stSchedule->bytState);
		return -1;
	}

	//ScheduleCfgEnable[ch]=[启用]
	//ScheduleCfg[ch]_[星期]=[48]--[启用][星期][半小时]
	snprintf(key_str, sizeof(key_str), "AudioAlarmScheduleCfgEnable%02d", m_iChannel);
	pCConfig->SetValue(MotionCfgSection, key_str, stSchedule->bytState);
	snprintf(key_str, sizeof(key_str), "AudioAlarmScheduleCfgEnableDN%02d", m_iChannel);
	pCConfig->SetValue(MotionCfgSection, key_str, stSchedule->nDayMode);

	for (i = 0; i < 7; i++)
	{
		for (j = 0; j < 48; j++)
		{
			value[j] = stSchedule->dwTimeSeg[i][j] + '0';
		}
		value[j] = 0;
		snprintf(key_str, sizeof(key_str), "AudioAlarmScheduleCfg%02d_%01d", m_iChannel, i);

		pCConfig->SetValue(MotionCfgSection, key_str, value);
		TRACE_AVL("[CMotion:%s]cfg->bytState=%d,Write:%s\n", __FUNCTION__, stSchedule->bytState, value);
	}
	
	//更新计划
	memcpy(&m_AudioAlarmScheduleCfg, stSchedule, sizeof (tagAudioAlarmSchedule));

	return 0;
}

int CMotion::GetAudioAlarmSchedule(tagAudioAlarmSchedule *stSchedule)
{
	if (NULL == stSchedule)
	{
		AVL_ERR("[CMotion:%s]cfg==NULL\n", __FUNCTION__);
		return -1;
	}
	memcpy(stSchedule, &m_AudioAlarmScheduleCfg, sizeof (tagAudioAlarmSchedule));
	return 0;
}

int CMotion::CheckAudioAlarmInSchedule()
{
	int sdkRet = 0;
	int day = 0;
	int seg = 0;
	time_t curr_time = 0;
	struct tm curr_tm = {0};
	int dayNightStatus = -1;
	int nResult = 0;
	
	sdkRet = ZWsys_EncGetDayNightStatus(m_henc, &dayNightStatus);
	if (0 != sdkRet)
	{
		AVL_ERR("ZWsys_EncGetDayNightStatus failed! ret=%x\n", sdkRet);
		return 0;
	}

	if (1 == m_AudioAlarmScheduleCfg.nDayMode)//白天
	{
		if (1 == dayNightStatus)
		{
			nResult = 1;
		}
	}
	else if (0 == m_AudioAlarmScheduleCfg.nDayMode)//夜晚
	{
		if (0 == dayNightStatus)
		{
			nResult = 1;
		}
	}
	else if (2 == m_AudioAlarmScheduleCfg.nDayMode && 0 == m_AudioAlarmScheduleCfg.bytState)//全天
	{
		nResult = 1;
	}
	else if (1 == m_AudioAlarmScheduleCfg.bytState)//自定义
	{
		//获取当前时间段信息
		curr_time = time(NULL);
		localtime_r(&curr_time, &curr_tm);
		day = curr_tm.tm_wday;
		seg = (curr_tm.tm_hour << 1) + (curr_tm.tm_min >= 30);
		TRACE_AVL("wday=%d hour=%d min=%d---\n", day, curr_tm.tm_hour, curr_tm.tm_min);
		
		if (m_AudioAlarmScheduleCfg.dwTimeSeg[day][seg])
		{
			nResult = 1;
		}
	}

	return nResult;
}

int CMotion::SetAudioAlarmCfg(int nVoiceType, int nBroadcastCount)
{
	return 0;
}

int CMotion::GetAudioAlarmCfg(int *nVoiceType, int *nBroadcastCount)
{
	*nVoiceType = 0;
	*nBroadcastCount = 0;
	return 0;
}

//##ModelId=49BDEC840176
int CMotion::PedestrianFilteringEnable(int nEnable)
{
	return (int) 0;
}

//##ModelId=49BDEC8401C6
int CMotion::IsEnable()
{
    //printf("[%s]MotionEnable = %d\n", __FUNCTION__, m_bMotionEnable);
	return (int) m_bMotionEnable;
}

int CMotion::IsPedestrianFilteringEnable()
{
	return 0;
}

//##ModelId=49BDEC8401EE
int CMotion::SetAlarmCallBack(int AlarmType, int (*CallBack) (struct tagAlertInfo * pAlertInfo, struct tagActionInfo * pActionInfo))
{
	if (CallBack == NULL)
	{
		AVL_ERR("[CMotion:%s]CallBack==NULL\n", __FUNCTION__);
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
int CMotion::EnableAction(int Action, int bEnable)
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
int CMotion::IsEnableAction(int Action)
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
int CMotion::GetRecordParam(int *dwChannelFlag)
{
	return m_pActionAlarm->GetRecordParam(dwChannelFlag);
}

//!\brief 设置某个报警联动录像参数
//!\param dwChannelFlag 联动录像通道标记
//!\param nPort 某个报警通道号
//!\return 0表示设置成功 -1表示设置失败
//##ModelId=49B718640232
int CMotion::SetRecordParam(int dwChannelFlag)
{
	return m_pActionAlarm->SetRecordParam(dwChannelFlag);
}

//!\brief 
//获取某个报警联动拍照参数,拍照张数和间隔由CCapturePic::GetCfg获取
//!\param dwChannelFlag 存储联动拍照通道标记的缓冲区
//!\param nPort 报警联动输入通道号
//!\return 0表示成功 -1表示失败
//##ModelId=49B5CE9A030D
int CMotion::GetSnapshotParam(int *dwChannelFlag)
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
int CMotion::SetSnapshotParam(int dwChannelFlag)
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

int CMotion::GetPtzParam(int nChannel, int *nAction, int *nIndex)
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
int CMotion::SetPtzParam(int nChannel, int nAction, int nIndex)
{
	return m_pActionAlarm->SetPtzParam(nChannel, nAction, nIndex);
}

//!\brief 获取某个报警联动报警输出的参数
//!\param dwOutPortFlag 存储输出通道标记的缓冲区
//!\param nPort 某个报警通道号
//!\return 0表示成功 -1表示失败
//##ModelId=49B718DD030D
int CMotion::GetDopParam(int *dwOutPortFlag)
{
	return m_pActionAlarm->GetDopParam(dwOutPortFlag);
}

//!\brief 设置某个报警联动报警输出的参数
//!\param dwOutPortFlag 设置输出通道标记
//!\param nPort 某个报警通道号
//!\return 0表示成功 -1表示失败
//##ModelId=49B718ED00AB
int CMotion::SetDopParam(int dwOutPortFlag)
{
	return m_pActionAlarm->SetDopParam(dwOutPortFlag);
}

//!\brief 获取移动的时间计划
//!\param tagSchedule* cfg 时间计划结构体
//!\return 成功为0，失败为-1
//##ModelId=49B7164C0058
int CMotion::GetSchedule(tagMotionSchedule * cfg)
{
	if (NULL == cfg)
	{
		AVL_ERR("[CMotion:%s]cfg==NULL\n", __FUNCTION__);
		return -1;
	}
	memcpy(cfg, &m_ScheduleCfg, sizeof (tagMotionSchedule));
	return 0;
}

int CMotion::SetPushEventIntvCnt(int nPushEventIntv)
{
	return m_pActionAlarm->SetPushEventIntvCnt(m_AlarmType, nPushEventIntv);
}

int CMotion::GetPushEventIntvCnt(int *nPushEventIntv)
{
	return m_pActionAlarm->GetPushEventIntvCnt(m_AlarmType, nPushEventIntv);
}

//!\brief 设置移动的时间计划
//!\param ttagSchedule cfg 时间计划结构体
//!\return 成功为0，失败为-1
//##ModelId=49B716980242
int CMotion::SetSchedule(const tagMotionSchedule * cfg)
{
	char value[255];

	char key_str[30];

	int i, j;

	if (NULL == cfg)
	{
		AVL_ERR("[CMotion:%s]cfg==NULL\n", __FUNCTION__);
		return -1;
	}
	if (cfg->bytState > 2)
	{
		AVL_ERR("[CMotion:%s]cfg->bytState=%d\n", __FUNCTION__, cfg->bytState);
		return -1;
	}

	//移动计划--
	//ScheduleCfgEnable[ch]=[启用]
	//ScheduleCfg[ch]_[星期]=[48]--[启用][星期][半小时]
	snprintf(key_str, sizeof(key_str), "ScheduleCfgEnable%02d", m_iChannel);
	pCConfig->SetValue(MotionCfgSection, key_str, cfg->bytState);

	for (i = 0; i < 7; i++)
	{
		for (j = 0; j < 48; j++)
		{
			value[j] = cfg->dwTimeSeg[i][j] + '0';
		}
		value[j] = 0;
		snprintf(key_str, sizeof(key_str), "ScheduleCfg%02d_%01d", m_iChannel, i);

		pCConfig->SetValue(MotionCfgSection, key_str, value);
		TRACE_AVL("[CMotion:%s]cfg->bytState=%d,Write:%s\n", __FUNCTION__, cfg->bytState, value);
	}
	//更新计划
	memcpy(&m_ScheduleCfg, cfg, sizeof (tagMotionSchedule));

	AiScehdule MotionSchedule;
	MotionSchedule.state = m_ScheduleCfg.bytState;
	memcpy(MotionSchedule.dwTimeSeg, m_ScheduleCfg.dwTimeSeg, SEH_MAX_DAYS * SEH_MAX_SEG_DAYS* sizeof(unsigned char));
	//布放计划传到sdk
	ZWSys_EncSetMotionSchedule(m_henc, &MotionSchedule);
	
	return 0;
}

int CMotion::LoadParam()
{
	int i, j;

	char key[100] = {0};

	char value[256] = {0};

    int x=0,y=0,nCount=0;	
    int curr_num = 0;
    
    int nTScopeX, nTScopeY;  
    float  nBScopeX, nBScopeY;
    
	unsigned char byMagicBlock[16 * 12] = {0};   // 将整幅图片划成16 * 12 = 192个宏块

	tagMotionSchedule *cfg;
	tagAudioAlarmSchedule *audioAlarmSche = NULL;

	//移动
	snprintf(key, sizeof(key), "MotionEnable%02d", m_iChannel);
	m_bMotionEnable = pCConfig->GetValue(MotionCfgSection, key, 0);

	m_bPedestrianFilteringEnable = 0;
	
	snprintf(key, sizeof(key), "MotionPrecision%02d", m_iChannel);
	m_iPrecision = pCConfig->GetValue(MotionCfgSection, key, DEFAULT_GRADE_M);

    //当前是否CGI配置起效
    m_bCgiIsEffective = pCConfig->GetValue(MotionCfgSection, "MotionCurrentCgiIsEffective", 1);

	//块
	for (i = 0; i < 6; i++)
	{
		snprintf(key, sizeof(key), "MotionMagicBlock%02d_%02d", m_iChannel, i);
		m_blockEnable[i] = pCConfig->GetValue(MotionCfgSection, key, 0);
	}

    //区域(将保存的坐标取出来放入全局数组中)
    for (i=0; i<MAX_MOTION_REGION_NUM; i++)
    {
        snprintf(key, sizeof(key), "MotionArea_%d_Enable", i);
        m_RegionInfo[i].bValid = pCConfig->GetValue(MotionCfgSection, key, 0);
        snprintf(key, sizeof(key), "MotionArea_%d_TopLeftX", i);
        m_RegionInfo[i].nMotionAreaTopLeftX = pCConfig->GetValue(MotionCfgSection, key, 0);
        snprintf(key, sizeof(key), "MotionArea_%d_TopLeftY", i);
        m_RegionInfo[i].nMotionAreaTopLeftY = pCConfig->GetValue(MotionCfgSection, key, 0);
        snprintf(key, sizeof(key), "MotionArea_%d_BottomRightX", i);
        m_RegionInfo[i].nMotionAreaBottomRightX = pCConfig->GetValue(MotionCfgSection, key, 0);
        snprintf(key, sizeof(key), "MotionArea_%d_BottomRightY", i);
        m_RegionInfo[i].nMotionAreaBottomRightY = pCConfig->GetValue(MotionCfgSection, key, 0);

        printf("Get Area_%d: %d [(%d,%d); (%d,%d)]\n", i, m_RegionInfo[i].bValid,
            m_RegionInfo[i].nMotionAreaTopLeftX,
            m_RegionInfo[i].nMotionAreaTopLeftY,
            m_RegionInfo[i].nMotionAreaBottomRightX,
            m_RegionInfo[i].nMotionAreaBottomRightY);

        if (m_RegionInfo[i].bValid == 0)
            continue;  
        
    	/*侦测区域,坐标范围位于[704, 576] */
    	if (m_RegionInfo[i].nMotionAreaTopLeftX > 703)
    	{
    		AVL_ERR("nMotionAreaTopLeftX = %d is overflow\n", m_RegionInfo[i].nMotionAreaTopLeftX);
    		m_RegionInfo[i].nMotionAreaTopLeftX = 703;
    	}
		else
	    	if (m_RegionInfo[i].nMotionAreaTopLeftX < 0)
	    	{
	    		AVL_ERR("nMotionAreaTopLeftX = %d is overflow\n", m_RegionInfo[i].nMotionAreaTopLeftX);
	    		m_RegionInfo[i].nMotionAreaTopLeftX = 0;
	    	}
		
    	if (m_RegionInfo[i].nMotionAreaTopLeftY > 575)
    	{
    		AVL_ERR("nMotionAreaTopLeftY = %d is overflow\n", m_RegionInfo[i].nMotionAreaTopLeftY);
    		m_RegionInfo[i].nMotionAreaTopLeftY = 575;
    	}
		else
	    	if (m_RegionInfo[i].nMotionAreaTopLeftY < 0)
	    	{
	    		AVL_ERR("nMotionAreaTopLeftY = %d is overflow\n", m_RegionInfo[i].nMotionAreaTopLeftY);
	    		m_RegionInfo[i].nMotionAreaTopLeftY = 0;
	    	}
		
    	if (m_RegionInfo[i].nMotionAreaBottomRightX > 703)
    	{
    		AVL_ERR("nMotionAreaBottomRightX = %d is overflow\n", m_RegionInfo[i].nMotionAreaBottomRightX);
    		m_RegionInfo[i].nMotionAreaBottomRightX = 703;
    	}
		else
	    	if (m_RegionInfo[i].nMotionAreaBottomRightX < 0)
	    	{
	    		AVL_ERR("nMotionAreaBottomRightX = %d is overflow\n", m_RegionInfo[i].nMotionAreaBottomRightX);
	    		m_RegionInfo[i].nMotionAreaBottomRightX = 0;
	    	}

    	if (m_RegionInfo[i].nMotionAreaBottomRightY > 575)
    	{
    		AVL_ERR("nMotionAreaBottomRightY = %d is overflow\n", m_RegionInfo[i].nMotionAreaBottomRightY);
    		m_RegionInfo[i].nMotionAreaBottomRightY = 575;
    	}
		else
	    	if (m_RegionInfo[i].nMotionAreaBottomRightY < 0)
	    	{
	    		AVL_ERR("nMotionAreaBottomRightY = %d is overflow\n", m_RegionInfo[i].nMotionAreaBottomRightY);
	    		m_RegionInfo[i].nMotionAreaBottomRightY = 0;
	    	}

    	/* X[0~15]  Y[0~11] */
        nTScopeX = m_RegionInfo[i].nMotionAreaTopLeftX * 16 / 704;
        nTScopeY = m_RegionInfo[i].nMotionAreaTopLeftY * 12 / 576;
        nBScopeX = m_RegionInfo[i].nMotionAreaBottomRightX * 16.0 / 704.0;
        nBScopeY = m_RegionInfo[i].nMotionAreaBottomRightY * 12.0 / 576.0;
        
        /* 如果超出边界部分，则向上取整，否则不变 */
        if (IsFloat(nTScopeX)) nTScopeX = ceil(nTScopeX);
        if (IsFloat(nTScopeY)) nTScopeY = ceil(nTScopeY);
        if (IsFloat(nBScopeX)) nBScopeX = ceil(nBScopeX);
        if (IsFloat(nBScopeY)) nBScopeY = ceil(nBScopeY);

        if (nTScopeX > 16.0) nTScopeX = 16.0;
        if (nTScopeY > 12.0) nTScopeY = 12.0;
        if (nBScopeX > 16.0) nBScopeX = 16.0;
        if (nBScopeY > 12.0) nBScopeY = 12.0;
              
        int j = 0, k = 0, flag = 0;
        for (y=nTScopeY; y<(int)nBScopeY; y++)      // 一共(nBScopeY-nTScopeY)行 
        {
            // 将序列号放到数组中，数组中存放的是已经标记了的宏块的序列号(x行y列)
            for (x=nTScopeX; x<(int)nBScopeX; x++)  // 一共(nBScopeX-nTScopeX)列
            {
                // 在数组byMagicBlock里面如果存在该下标，则表示该宏块已经被标记，执行下一个宏块
                k = y * 16 + x;
                for (j=0; j<nCount; j++)
                {
                    if (byMagicBlock[j] == k)
                        flag = 1;
                }
                if (flag == 1)
                {
                    flag = 0;
                    continue;
                }
                byMagicBlock[nCount++] = k;
            }
        }
    }
    m_RegionCnt = nCount;
    m_iRectNumCgi = 0;
    memset(m_blockEnableCgi, 0, 6*sizeof(int));
	memset(m_pRectCgi, 0, MAX_MAGIC_BLOCK_NUM);
	memset(m_pBit_RectCgi, 0, MAX_MAGIC_BLOCK_NUM/8);
	for (i = 0; i < m_RegionCnt; i++)
	{
	    /* pMagicBlock数组中存放的是已经被标记了的宏块的序列号[0, 191] */
		curr_num = byMagicBlock[i];   // 当前宏块的标号(从0开始，对应数组标号)
		if (curr_num < 0 || curr_num >= MAX_MAGIC_BLOCK_NUM)
		{
			continue;
		}
		m_pRectCgi[curr_num] = 1;      // 标记宏块，放入全局数组中
		// i >> 5, 即i/32，m_blockEnable的下标
		m_blockEnableCgi[curr_num >> 5] |= 1 << (curr_num & 31);
        // i >> 3, 即i/8 ，m_pBit_Rect的下标
		m_pBit_RectCgi[curr_num >> 3] |= 1 << (curr_num & 7);
		m_iRectNumCgi++;
	}

#ifdef CGI_PRINT
    printf("******************************Load CGI*******************************\n");
    for (i=0; i<192; i++)
    {
        if (i%16 == 0)
            putchar('\n');
        if (m_pRectCgi[i] == 1)
            printf("1");
        else
            printf("0");
    }
    printf("\n****************************End CGI*******************************\n");
    
    printf("CGI => Load: m_iRectNumCgi = %d\n", m_iRectNumCgi);
#endif

    printf("[%s]MotionEnable = %d\n", __FUNCTION__, m_bMotionEnable);
    
	//导入计划
	cfg = &m_ScheduleCfg;
	//移动计划--
	//ScheduleCfgEnable[ch]=[启用]
	//ScheduleCfg[ch]_[星期]=[48]--[启用][星期][半小时]
	snprintf(key, sizeof(key), "ScheduleCfgEnable%02d", m_iChannel);
	cfg->bytState = pCConfig->GetValue(MotionCfgSection, key, 0);
	cfg->bytReserve[0] = 0;
	cfg->bytReserve[1] = 0;
	cfg->bytReserve[2] = 0;
	for (i = 0; i < 7; i++)
	{
		snprintf(key, sizeof(key), "ScheduleCfg%02d_%01d", m_iChannel, i);
		pCConfig->GetValue(MotionCfgSection, key, SCHEDULE_TIME_ZERO, value, 256);
		for (j = 0; j < 48; j++)
		{
			cfg->dwTimeSeg[i][j] = value[j] - '0';
		}
	}

	AiScehdule MotionSchedule;
	MotionSchedule.state = m_ScheduleCfg.bytState;
	memcpy(MotionSchedule.dwTimeSeg, m_ScheduleCfg.dwTimeSeg, SEH_MAX_DAYS * SEH_MAX_SEG_DAYS* sizeof(unsigned char));
	//布放计划传到sdk
	ZWSys_EncSetMotionSchedule(m_henc, &MotionSchedule);

	//导入声音报警计划
	audioAlarmSche = &m_AudioAlarmScheduleCfg;
	snprintf(key, sizeof(key), "AudioAlarmScheduleCfgEnable%02d", m_iChannel);
	audioAlarmSche->bytState = pCConfig->GetValue(MotionCfgSection, key, 0);//自定义、全天
	snprintf(key, sizeof(key), "AudioAlarmScheduleCfgEnableDN%02d", m_iChannel);
	audioAlarmSche->nDayMode = pCConfig->GetValue(MotionCfgSection, key, 2);//白天，夜晚
	for (i = 0; i < 7; i++)
	{
		snprintf(key, sizeof(key), "AudioAlarmScheduleCfg%02d_%01d", m_iChannel, i);
		pCConfig->GetValue(MotionCfgSection, key, SCHEDULE_TIME_ZERO, value, sizeof(value));
		for (j = 0; j < 48; j++)
		{
			audioAlarmSche->dwTimeSeg[i][j] = value[j] - '0';
		}
	}
	
	return 0;
}

int CMotion::SetMode(int nCustomMode)
{
	if (nCustomMode < 0 || nCustomMode > 1)
	{
		AVL_ERR("[CMotion:%s]nCustomMode=%d\n", __FUNCTION__, nCustomMode);
		return -1;
	}
	//定制模式--RecCustomMode
	m_iCustomMode = nCustomMode;
	return (int) 0;
}

//##ModelId=49B8653002CA
int CMotion::GetMode()
{
	return m_iCustomMode;

}

int CMotion::CheckMotion()
{
	static int motionStatus=0;
	static int noMotionCount=0;
	int result = 0;

	m_bisMotion = m_lastMotion;
	m_lastMotion = 0;
	
	//if(m_iChannel == 0) TRACE_WARN("[%s] type = %d enable = %d,ismotion = %d mode = %d stat = %d %d\n",__FUNCTION__,m_AlarmType,m_bMotionEnable,m_bisMotion,m_iCustomMode,m_ScheduleCfg.bytState,CLOCKS_PER_SEC);
	if (m_bMotionEnable && m_bisMotion)	//检测到移动了
	{
		printf("\n-------检测到移动--m_iCustomMode=%d--bytState=%d---\n",
			m_iCustomMode, m_ScheduleCfg.bytState);
		if (m_iCustomMode && m_ScheduleCfg.bytState == 1)
		{						//高级模式且有效
			int day = 0;
			int seg = 0;
			time_t curr_time = 0;
			struct tm curr_tm = {0};

			//获取当前时间段信息
			curr_time = time(NULL);
			localtime_r(&curr_time, &curr_tm);
			day = curr_tm.tm_wday;
			seg = (curr_tm.tm_hour << 1) + (curr_tm.tm_min >= 30);
			TRACE_AVL("wday=%d hour=%d min=%d---\n", day, curr_tm.tm_hour, curr_tm.tm_min);
			if (!m_ScheduleCfg.dwTimeSeg[day][seg])
			{					//不处理
				return 0;
			}

		}
		motionStatus = 1;
		noMotionCount = 0;

		m_pActionAlarm->UpdateActionIntvCnt(m_AlarmType, 1);
		result = m_pActionAlarm->CheckAction(m_Chn,m_AlarmType, m_FxnCallBack, 0);
		
		return result;
	}
	else
	{
		m_pActionAlarm->UpdateActionIntvCnt(m_AlarmType, 0);
		if (1 == motionStatus)
		{
			noMotionCount++;
		}
		if ((1 == motionStatus) && (noMotionCount >= SEND_END_EVNET_TIME_INTERNAL || 0 == m_bMotionEnable))//移动停止15秒后或者移动侦测功能关闭时发送结束通知
		{
			motionStatus = 0;
			result = m_pActionAlarm->CheckAction(m_Chn,m_AlarmType, m_FxnCallBack, 1);
		}
	}
	
	return 0;
}

//!\brief 如果移动侦测没有使能则使能，如果没有选择区域则设置为全屏
//!\return 成功为0，失败为-1
int CMotion::startMotionCfgAdjust()
{
	int result = 0;

	if(m_bCgiIsEffective && (0 == m_iRectNumCgi))
	{
		printf("m_iRectNumCgi is null, add one Rect [0,0,703,575]\n");
		tagMotionRegion pcfg[MAX_MOTION_REGION_NUM];
		pcfg[0].bValid = 1;
		pcfg[0].nMotionAreaTopLeftX = 0;
		pcfg[0].nMotionAreaTopLeftY = 0;
		pcfg[0].nMotionAreaBottomRightX = 703;
		pcfg[0].nMotionAreaBottomRightY = 575;
		
		result = EnableMagicBlock_CGI(pcfg, 1);
		if(result)
		{
			AVL_ERR("EnableMagicBlock_CGI fail!\n");
		}
	}

	if((0 == m_bCgiIsEffective) && (0 == m_iRectNum))
	{
		unsigned char magicBlock[MAX_MAGIC_BLOCK_NUM] = {0};
		int i;

		for(i = 0; i < MAX_MAGIC_BLOCK_NUM; i++)
		{
			magicBlock[i] = i;
		}
		
		result = EnableMagicBlock(magicBlock, MAX_MAGIC_BLOCK_NUM);
		if(result)
		{
			AVL_ERR("EnableMagicBlock fail!\n");
		}
	}

	if(0 == m_bMotionEnable)
	{
		printf("Motion is disable, force enable!\n");
		result = Enable(1);
		if(result)
		{
			AVL_ERR("EnableMagicBlock_CGI fail!\n");
		}

	}

	return 0;
}

