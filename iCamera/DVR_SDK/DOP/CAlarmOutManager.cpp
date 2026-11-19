// Copyright (C) 1991 - 1999 Rational Software Corporation
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <endian.h>

#include "global_macro.h"
#include "CAlarmOutManager.h"
#include "newhie/HieServer.h"
#include "newhie/HieServer_Common.h"
#include "DOP_global.h"
#include "zwsdk.h"

extern "C" int ZWSys_SetAlarmLightStayTime(int nEnable,int nStayTime);

typedef struct tagBITMAPINFOHEADER
{
	unsigned int biSize;
	int biWidth;
	int biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned int biCompression;
	unsigned int biSizeImage;
	int biXPelsPerMeter;
	int biYPelsPerMeter;
	unsigned int biClrUsed;
	unsigned int biClrImportant;
} __attribute__ ((packed)) BITMAPINFOHEADER, *PBITMAPINFOHEADER;

typedef struct tagBITMAPFILEHEADER
{
	unsigned short bfType;
	unsigned int bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned int bfOffBits;
} __attribute__ ((packed)) BITMAPFILEHEADER, *PBITMAPFILEHEADER;

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define SWAP16(x) (x)
#define SWAP32(x) (x)
#else
#define SWAP16(x) ((((x)&0xff)<<8)|(((x)>>8)&0xff))
#define SWAP32(x)  ((SWAP16((x)&0xffff)<<16)|(SWAP16((x)>>16)))
#endif

static void *Fnx_CheckThread(void *data);

static const char *AlarmOutSectionStr = "AlarmOutCfg";
static const char *AlarmLightSection = "AlarmLightcfg";

static const char *TimeScheduleModeKey = "TimeScheduleMode";
static const char *TimeScheduleStartKey = "TimeScheduleStart";
static const char *TimeScheduleStopKey = "TimeScheduleStop";

static const char *DayAreaStartTimeKey = "DayAreaStartTime";
static const char *NightAreaStartTimeKey = "NightAreaStartTime";

//点阵文件路径
static const char *FileNameAlarmOutPixel = OSD_PIXEL;

CAlarmOutManager *CAlarmOutManager::m_pInstance = NULL;

int CAlarmOutManager::m_iInstance_Cnt = 0;

#if 0
// 判断当前时间是否在给出的时间段之内(支持跨天)
static int IsCurrTimeInTimeArea(const pTimeArea pBeginTime, const pTimeArea pEndTime)
{
    if (NULL==pBeginTime || NULL==pEndTime)
    {
        fprintf(stderr, "NULL POINTER EXCEPTION!\n");
        return -1;
    }

    time_t curr_timestamp = 0;
    struct tm curr_tm = {0};
    unsigned int curr_sec = 0;
    unsigned int start_sec = 0;
    unsigned int stop_sec = 0;
    int b_in_time_area = 0;

    curr_timestamp = time(NULL);
    localtime_r(&curr_timestamp, &curr_tm);
    curr_sec = (curr_tm.tm_hour * 3600 + curr_tm.tm_min * 60 + curr_tm.tm_sec);

    start_sec = pBeginTime->nHour * 3600 + pBeginTime->nMinute * 60 + pBeginTime->nSecond;
    stop_sec  = pEndTime->nHour * 3600 + pEndTime->nMinute * 60 + pEndTime->nSecond;

    if(start_sec > stop_sec)
    {
        if(((curr_sec >= start_sec) && (curr_tm.tm_hour <= 23)) 
         || (curr_sec <= stop_sec))
        {
            b_in_time_area = 1;
        }
        else
        {
            b_in_time_area = 0;
        }
    }
    else
    {
        if((curr_sec >= start_sec) && (curr_sec <= stop_sec))
        {
            b_in_time_area = 1;
        }
        else
        {
            b_in_time_area = 0;
        }
    }

    return b_in_time_area;
}
#endif

//##ModelId=499E618A02D4
CAlarmOutManager::CAlarmOutManager()
{
	m_bInit = 0;
	theCEmbAlarmOut = NULL;
	theCExtAlarmOut = NULL;
	m_iEmbAlarmOutNum = 0;
	m_iExtAlarmOutNum = 0;
	m_hCheckThread = 0;
}

//##ModelId=499E618C013D
CAlarmOutManager::~CAlarmOutManager()
{
	DOP_UnInit();
}

//!\brief 初始化
//!\return 0成功，其它失败
int CAlarmOutManager::DOP_Init()
{
	int ret;

	int i;

	if (m_bInit)
	{
		TRACE("[CAlarmOutManager:%s]already inited\n", __FUNCTION__);
		return 0;
	}
	pCConfig = CConfig::GetInstance();
	if (pCConfig == NULL)
	{
		TRACE_ERR("[CAlarmOutManager:%s]pCConfig==NULL\n", __FUNCTION__);
		return -1;
	}
	//实例化基本alarmout
	theCEmbAlarmOut = CEmbAlarmOut::GetInstance();
	if (NULL == theCEmbAlarmOut)
	{
		TRACE_ERR("[CAlarmOutManager:%s]CEmbAlarmOut GetInstance Failed \n", __FUNCTION__);
		DOP_UnInit();
		return -1;
	}
	//实例化扩展alarmout
	theCExtAlarmOut = CExtAlarmOut::GetInstance();
	if (NULL == theCExtAlarmOut)
	{
		TRACE_ERR("[CAlarmOutManager:%s]CExtAlarmOut  GetInstance Failed \n", __FUNCTION__);
		DOP_UnInit();
		return -1;
	}
	if (theCEmbAlarmOut->Open())
	{
		TRACE_ERR("[CAlarmOutManager:%s]theCEmbAlarmOut open Failed \n", __FUNCTION__);
		DOP_UnInit();
		return -1;
	}
	if (theCExtAlarmOut->Open())
	{
		TRACE_ERR("[CAlarmOutManager:%s]theCExtAlarmOut open Failed \n", __FUNCTION__);
		DOP_UnInit();
		return -1;
	}
	//获取端口数
	m_iEmbAlarmOutNum = theCEmbAlarmOut->GetNum();
	m_iExtAlarmOutNum = theCExtAlarmOut->GetNum();

    memset(&m_stuAlarmOutTimeSchedule, 0, sizeof(m_stuAlarmOutTimeSchedule));
    memset(&m_stuDayAreaStartTime, 0, sizeof(m_stuDayAreaStartTime));
    memset(&m_stuNightAreaStartTime, 0, sizeof(m_stuNightAreaStartTime));

	//导入配置
	LoadParam();

    ZWSys_SetAlarmLightStayTime(m_iAlarmLightEn, m_iAlarmLightStayTime);
    
	for (i = 0; i < MAX_ALARMOUT_NUM; i++)
	{
		m_bState[i] = 0;
	}
	m_bInit = 1;

	m_bCheck_exit = 0;
	//创建计划检测线程
	ret = pthread_create(&m_hCheckThread, NULL, Fnx_CheckThread, this);
	if (ret != 0)
	{
		TRACE_ERR("[CAlarmOutManager:%s]pthread_create   failed (%d)\n", __FUNCTION__, ret);
		DOP_UnInit();
		return -1;
	}
	return 0;

}

//!\brief 释放资源
//!\return 0成功，其它失败
int CAlarmOutManager::DOP_UnInit()
{


	//结束计划检测线程
	m_bCheck_exit = 1;
	if (0 != m_hCheckThread)
	{
		pthread_cancel(m_hCheckThread);
		pthread_join(m_hCheckThread, NULL);
		m_hCheckThread = 0;
	}
	//置未初始化状态
	m_bInit = 0;
	//关闭端口


	if (NULL != theCEmbAlarmOut)
	{
		theCEmbAlarmOut->Close();
		CEmbAlarmOut::Release();
		theCEmbAlarmOut = NULL;
	}
	if (NULL != theCExtAlarmOut)
	{
		theCExtAlarmOut->Close();
		CExtAlarmOut::Release();
		theCExtAlarmOut = NULL;
	}

	m_iEmbAlarmOutNum = 0;
	m_iExtAlarmOutNum = 0;
	return 0;
}

int CAlarmOutManager::LoadParam()
{
	int i, j;

	char key[30];

	int nPort;

	char value[255];

	AlarmOutSchedule *cfg;

	DIR *pdir;

	//检查文件路径是否存在
	pdir = opendir(FileNameAlarmOutPixel);
	if (NULL == pdir)
	{
		mkdir(FileNameAlarmOutPixel, 777);

	}
	else
	{
		closedir(pdir);
	}
	//导入模式
	//读模式配置
	m_Enable = pCConfig->GetValue("AlarmOutCfg", "AlarmOutEnable", 1);
	m_iMode = pCConfig->GetValue("AlarmInCfg", "AlarmInMode", 0);
    m_iAlarmLightEn = pCConfig->GetValue(AlarmLightSection, "AlarmLightEn", 0);
	m_iAlarmLightStayTime = pCConfig->GetValue(AlarmLightSection, "AlarmLightStayTime", 5);

    // 报警输出时间计划
    m_stuAlarmOutTimeSchedule.nTimeMode = pCConfig->GetValue(AlarmOutSectionStr, TimeScheduleModeKey, 0);

    // 时间计划的起始时间
    char time_segment_str[16] = {0};
    pCConfig->GetValue(AlarmOutSectionStr, TimeScheduleStartKey, "20:00:00", time_segment_str, sizeof(time_segment_str));
    sscanf(time_segment_str, "%d:%d:%d", &m_stuAlarmOutTimeSchedule.struTimeSegment[0].struStartTime.nHour,
        &m_stuAlarmOutTimeSchedule.struTimeSegment[0].struStartTime.nMinute,
        &m_stuAlarmOutTimeSchedule.struTimeSegment[0].struStartTime.nSecond);

    // 时间计划的结束时间
    memset(time_segment_str, 0, sizeof(time_segment_str));
    pCConfig->GetValue(AlarmOutSectionStr, TimeScheduleStopKey, "08:00:00", time_segment_str, sizeof(time_segment_str));
    sscanf(time_segment_str, "%d:%d:%d", &m_stuAlarmOutTimeSchedule.struTimeSegment[0].struStopTime.nHour,
        &m_stuAlarmOutTimeSchedule.struTimeSegment[0].struStopTime.nMinute,
        &m_stuAlarmOutTimeSchedule.struTimeSegment[0].struStopTime.nSecond);

    // 白天的开始时间
    memset(time_segment_str, 0, sizeof(time_segment_str));
    pCConfig->GetValue(AlarmOutSectionStr, DayAreaStartTimeKey, "08:00:00", time_segment_str, sizeof(time_segment_str));
    sscanf(time_segment_str, "%d:%d:%d", &m_stuDayAreaStartTime.nHour, &m_stuDayAreaStartTime.nMinute, &m_stuDayAreaStartTime.nSecond);

    // 晚上的开始时间
    memset(time_segment_str, 0, sizeof(time_segment_str));
    pCConfig->GetValue(AlarmOutSectionStr, NightAreaStartTimeKey, "20:00:00", time_segment_str, sizeof(time_segment_str));
    sscanf(time_segment_str, "%d:%d:%d", &m_stuNightAreaStartTime.nHour, &m_stuNightAreaStartTime.nMinute, &m_stuNightAreaStartTime.nSecond);

	for (nPort = 0; nPort < (m_iEmbAlarmOutNum + m_iExtAlarmOutNum); nPort++)
	{
		//读名字
		snprintf(key, sizeof(key), "AlarmOutName_%02d", nPort);
		pCConfig->GetValue(AlarmOutSectionStr, key, "", m_strName[nPort], MAX_DOP_NAME_LEN);
		//读延迟时间
		snprintf(key, sizeof(key), "AlarmOutDelayTime_%02d", nPort);
		m_iDelayTime[nPort] = pCConfig->GetValue(AlarmOutSectionStr, key, 0);
		// 读计划--
		//ScheduleCfgEnable[ch]=[启用]
		//ScheduleCfg[ch]_[星期]=[48]--[启用][星期][半小时]
		cfg = &m_ptAlarmOutSchedule[nPort];
		snprintf(key, sizeof(key), "AOScheduleCfgEnable%02d", nPort);
		cfg->bytState = pCConfig->GetValue(AlarmOutSectionStr, key, 0);
		cfg->bytReserve[0] = 0;
		cfg->bytReserve[1] = 0;
		cfg->bytReserve[2] = 0;
		for (i = 0; i < 7; i++)
		{
			snprintf(key, sizeof(key), "AOScheduleCfg%02d_%01d", nPort, i);
			pCConfig->GetValue(AlarmOutSectionStr, key, "000000000000000000000000000000000000000000000000", value, 255);
			for (j = 0; j < 48; j++)
			{
				cfg->dwTimeSeg[i][j] = value[j] - '0';
			}

		}

	}

	return 0;
}

//##ModelId=49A23D07033B
CAlarmOutManager *CAlarmOutManager::DOP_GetInstance()
{
	if (NULL == m_pInstance)
	{
		m_pInstance = new CAlarmOutManager;
		if (NULL == m_pInstance)
		{
			TRACE_ERR("[CAlarmOutManager:%s]new CAlarmOutManager failed\n", __FUNCTION__);
			return NULL;
		}
		else
		{
			m_pInstance->DOP_Init();
		}

	}
	//实例引用计数
	m_iInstance_Cnt++;

	return (CAlarmOutManager *) m_pInstance;
}

//!\brief 释放实例
//!\return 0-释放,其它为引用计数
int CAlarmOutManager::DOP_Release()
{
	if (NULL != m_pInstance)
	{
		//
		m_iInstance_Cnt--;
		//  if(m_iInstance_Cnt<=0)
		{
			delete m_pInstance;

			m_pInstance = NULL;
		}
	}
	return m_iInstance_Cnt;

}

int CAlarmOutManager::DOP_SetAlarmLightEnable(int nEnable,  int nStayTime)
{
	int nRet = 0;

	if (!m_bInit)
	{
		TRACE("[CAlarmOutManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if(nStayTime <=0  || nStayTime > 60)
	{
		return -1;
	}

	if((m_iAlarmLightEn == nEnable) && (nStayTime == m_iAlarmLightStayTime))
	{
		return 0;
	}

	m_iAlarmLightEn = nEnable;

	m_iAlarmLightStayTime = nStayTime;

	nRet = ZWSys_SetAlarmLightStayTime(m_iAlarmLightEn, m_iAlarmLightStayTime);
	
	if(0 == nRet)
	{
		pCConfig->SetValue(AlarmLightSection, "AlarmLightEn", m_iAlarmLightEn);
		pCConfig->SetValue(AlarmLightSection, "AlarmLightStayTime", m_iAlarmLightStayTime);
	}


	return 0;
}

int CAlarmOutManager::DOP_GetAlarmLightEnable(int *nEnable,  int *nStayTime)
{
	if (!m_bInit)
	{
		TRACE("[CAlarmOutManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if(NULL == nStayTime || NULL == nEnable)
	{
		TRACE_ERR("[CAlarmOutManager:%s]invalid argument (pCfg = NULL)\n", __FUNCTION__);
		return -1;
	}

	*nEnable = m_iAlarmLightEn;

	*nStayTime = m_iAlarmLightStayTime;

	return 0;
}

/* 设置报警输出事件计划配置 */
int CAlarmOutManager::DOP_SetAlarmOutScheduleCfg(const pAlarmOutTimeSchedule pAlarmOutSchedule)
{
	if (!m_bInit)
	{
		TRACE("[CAlarmOutManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if (NULL == pAlarmOutSchedule)
	{
		TRACE_ERR("[CAlarmOutManager:%s]invalid argument (pAlarmOutSchedule = NULL)\n", __FUNCTION__);
		return -1;
	}

    if (pAlarmOutSchedule->nTimeMode < 0 || pAlarmOutSchedule->nTimeMode > 3)
    {
		TRACE_ERR("[CAlarmOutManager:%s]invalid argument (pAlarmOutSchedule->nTimeMode = %d out of [0,3])\n", 
		          __FUNCTION__, pAlarmOutSchedule->nTimeMode);
		return -1;
    }

    if (3 == pAlarmOutSchedule->nTimeMode &&
      (pAlarmOutSchedule->struTimeSegment[0].struStartTime.nHour < 0 
    || pAlarmOutSchedule->struTimeSegment[0].struStartTime.nHour > 24
    || pAlarmOutSchedule->struTimeSegment[0].struStopTime.nHour < 0 
    || pAlarmOutSchedule->struTimeSegment[0].struStopTime.nHour > 24
    || pAlarmOutSchedule->struTimeSegment[0].struStartTime.nMinute < 0 
    || pAlarmOutSchedule->struTimeSegment[0].struStartTime.nMinute > 59
    || pAlarmOutSchedule->struTimeSegment[0].struStopTime.nMinute < 0 
    || pAlarmOutSchedule->struTimeSegment[0].struStopTime.nMinute > 59
    || pAlarmOutSchedule->struTimeSegment[0].struStartTime.nSecond < 0 
    || pAlarmOutSchedule->struTimeSegment[0].struStartTime.nSecond > 59
    || pAlarmOutSchedule->struTimeSegment[0].struStopTime.nSecond < 0 
    || pAlarmOutSchedule->struTimeSegment[0].struStopTime.nSecond > 59)
    )
    {
		TRACE_ERR("[CAlarmOutManager:%s] custom mode time format invaild [%02d:%02d:%02d - %02d:%02d:%02d]\n", 
                    __FUNCTION__, 
                    pAlarmOutSchedule->struTimeSegment[0].struStartTime.nHour,
                    pAlarmOutSchedule->struTimeSegment[0].struStartTime.nMinute,
                    pAlarmOutSchedule->struTimeSegment[0].struStartTime.nSecond,
                    pAlarmOutSchedule->struTimeSegment[0].struStopTime.nHour,
                    pAlarmOutSchedule->struTimeSegment[0].struStopTime.nMinute,
                    pAlarmOutSchedule->struTimeSegment[0].struStopTime.nSecond);

        return -1;
    }

    pCConfig->SetValue(AlarmOutSectionStr, TimeScheduleModeKey, pAlarmOutSchedule->nTimeMode);

    char time_segment_str[16] = {0};

    snprintf(time_segment_str, sizeof(time_segment_str), "%02d:%02d:%02d", 
              pAlarmOutSchedule->struTimeSegment[0].struStartTime.nHour,
              pAlarmOutSchedule->struTimeSegment[0].struStartTime.nMinute,
              pAlarmOutSchedule->struTimeSegment[0].struStartTime.nSecond);

    pCConfig->SetValue(AlarmOutSectionStr, TimeScheduleStartKey, time_segment_str);

    memset(time_segment_str, 0, sizeof(time_segment_str));
    snprintf(time_segment_str, sizeof(time_segment_str), "%02d:%02d:%02d", 
            pAlarmOutSchedule->struTimeSegment[0].struStopTime.nHour,
            pAlarmOutSchedule->struTimeSegment[0].struStopTime.nMinute,
            pAlarmOutSchedule->struTimeSegment[0].struStopTime.nSecond);

    pCConfig->SetValue(AlarmOutSectionStr, TimeScheduleStopKey, time_segment_str);

    memcpy(&m_stuAlarmOutTimeSchedule, pAlarmOutSchedule, sizeof(stuAlarmOutTimeSchedule));

	return 0;
}

/* 获取报警输出事件计划配置 */
int CAlarmOutManager::DOP_GetAlarmOutScheduleCfg(pAlarmOutTimeSchedule pAlarmOutSchedule)
{
	if (!m_bInit)
	{
		TRACE("[CAlarmOutManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if (NULL == pAlarmOutSchedule)
	{
		TRACE_ERR("[CAlarmOutManager:%s]invalid argument (NULL == pAlarmOutSchedule)\n", __FUNCTION__);
		return -1;
	}

    memcpy(pAlarmOutSchedule, &m_stuAlarmOutTimeSchedule, sizeof(stuAlarmOutTimeSchedule));

    return 0;
}

//##ModelId=49B5E6F000BB
int CAlarmOutManager::DOP_SetMode(int nMode)
{
	int result;

	if (!m_bInit)
	{
		TRACE("[CAlarmOutManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nMode < 0 || nMode > 1)
	{
		TRACE_ERR("[CAlarmOutManager:%s]invalid argument (nMode = %d)\n", __FUNCTION__, nMode);
		return -1;
	}
	m_iMode = nMode;
	//写模式配置
	//   result=pCConfig->SetValue(AlarmOutSectionStr,"AlarmOutMode",m_iMode);
	//与报警输入同模式
	result = pCConfig->SetValue("AlarmInCfg", "AlarmInMode", m_iMode);
	if (result)
	{
		TRACE_ERR("[CAlarmOutManager:%s]SetValue Failed\n", __FUNCTION__);
		return result;
	}


	return (int) 0;
}

int CAlarmOutManager::DOP_GetMode()
{
	if (!m_bInit)
	{
		TRACE("[CAlarmOutManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	m_iMode = pCConfig->GetValue("AlarmInCfg", "AlarmInMode", 0);
	return (int) m_iMode;
}

int CAlarmOutManager::DOP_SetEnable(int nEn)
{
	int result = 0;

	if (!m_bInit)
	{
		TRACE("[CAlarmOutManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nEn < 0 || nEn > 1)
	{
		TRACE_ERR("[CAlarmOutManager:%s]invalid argument (nEn = %d)\n", __FUNCTION__, nEn);
		return -1;
	}

    if (m_Enable != nEn)
    {
    	m_Enable = nEn;

    	//与报警输入同模式
    	result = pCConfig->SetValue("AlarmOutCfg", "AlarmOutEnable", m_Enable);
    	if (result)
    	{
    		TRACE_ERR("[CAlarmOutManager:%s]SetEnable Failed\n", __FUNCTION__);
    	}
    }

	return result;
}

int CAlarmOutManager::DOP_GetEnable()
{
	if (!m_bInit)
	{
		TRACE("[CAlarmOutManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	return m_Enable;
}

//##ModelId=49B5DE8400EA
int CAlarmOutManager::DOP_GetName(int nPort, char *strName, int strNameSize,int nLength, char *strPixel, int pixelSize)
{
	int len = 0;

//    FILE *fp; 
	char fileName[255];

//    int width,height,wBytes;
//    char info[3];
//    int result;
//    int pixelLen;
//    BITMAPFILEHEADER    hdr;
//    BITMAPINFOHEADER  hbi;
	if (!m_bInit)
	{
		TRACE("[CAlarmOutManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (NULL == strName)
	{
		TRACE_ERR("[CAlarmOutManager:%s]nPort = %d invalid argument (NULL == strName)\n", __FUNCTION__, nPort);
		return -1;
	}

	if (nPort < 0 || nPort >= (m_iEmbAlarmOutNum + m_iExtAlarmOutNum))
	{
		TRACE_ERR("[CAlarmOutManager:%s]nPort = %d -->[0,%d]\n", __FUNCTION__, nPort, m_iExtAlarmOutNum + m_iEmbAlarmOutNum - 1);
		return -1;
	}
	len = strlen(m_strName[nPort]);

	if (len > nLength)
	{
		TRACE_ERR("[CAlarmOutManager:%s]nPort = %d invalid argument (nLength =%d  >= %d)\n", __FUNCTION__, nPort, nLength, len);
		return -1;
	}


	strncpy(strName, m_strName[nPort], strNameSize - 1);


	snprintf(fileName, sizeof(fileName), "%sAlarmOut_%02d.bmp", FileNameAlarmOutPixel, nPort + 1);
#if 0
	fp = fopen(fileName, "rb");
	if (fp == NULL)
	{
		TRACE_ERR("[CAlarmOutManager:%s]open file (%s)failed\n", __FUNCTION__, fileName);
		return 0;
	}
	result = fread(&hdr, 1, sizeof (BITMAPFILEHEADER), fp);
	if (result != sizeof (BITMAPFILEHEADER))
	{
		fclose(fp);
		TRACE_ERR("[CAlarmOutManager:%s]read bmp header failed->ch=%d\n", __FUNCTION__, nPort);
		return -1;
	}
	if (strPixel == NULL)
	{
		fclose(fp);
		return SWAP32(hdr.bfSize);
	}
	result = fread(&hbi, 1, sizeof (BITMAPINFOHEADER), fp);
	if (result != sizeof (BITMAPINFOHEADER))
	{
		fclose(fp);
		TRACE_ERR("[CAlarmOutManager:%s]read bmp info failed->ch=%d\n", __FUNCTION__, nPort);
		return -1;
	}

	if (24 != SWAP16(hbi.biBitCount))
	{
		fclose(fp);
		TRACE_ERR("[CAlarmOutManager:%s]not unsuppored bmp type->ch=%d\n", __FUNCTION__, nPort);
		return -1;
	}
	if (strPixel != NULL)
	{
		//开始填缓冲
		fseek(fp, 0, SEEK_SET);
		if (SWAP32(hdr.bfSize) != fread(strPixel, 1, SWAP32(hdr.bfSize), fp))
		{
			fclose(fp);
			TRACE_ERR("[CAlarmOutManager:%s]read bmp data failed ->ch=%d\n", __FUNCTION__, nPort);
			return -1;

		}
	}
	fclose(fp);

	return (int) SWAP32(hdr.bfSize);
#else
	if (strPixel != NULL)
	{
		strncpy(strPixel, fileName,pixelSize - 1);
	}
	return 0;
#endif
}

//##ModelId=49B5DE8C036B
int CAlarmOutManager::DOP_SetName(int nPort, char *strName, char *strPixel)
{
	int result;

	char key[100];

	FILE *fp;

	char fileName[255];

//    int width,height,wBytes;
//    char info[3];
//    int pixelLen;
	char *pdata = 0;

	struct stat statbuf;

	BITMAPFILEHEADER *hdr;

	BITMAPINFOHEADER *hbi;

	if (!m_bInit)
	{
		TRACE("[CAlarmOutManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (NULL == strName)
	{
		TRACE_ERR("[CAlarmOutManager:%s]nPort = %d invalid argument (NULL == strName)\n", __FUNCTION__, nPort);
		return -1;
	}
	if (strlen(strName) >= MAX_DOP_NAME_LEN)
	{
		TRACE_ERR("[CAlarmOutManager:%s]nPort = %d invalid argument (strlen(strName)=%d  >= %d)\n", __FUNCTION__, nPort, strlen(strName),
				  MAX_DOP_NAME_LEN);
		return -1;
	}
	if (nPort < 0 || nPort >= (m_iEmbAlarmOutNum + m_iExtAlarmOutNum))
	{
		TRACE_ERR("[CAlarmOutManager:%s]nPort = %d -->[0,%d]\n", __FUNCTION__, nPort, m_iExtAlarmOutNum + m_iEmbAlarmOutNum - 1);
		return -1;
	}
	strncpy(m_strName[nPort], strName,sizeof(m_strName[nPort]) - 1);

	if (NULL != strPixel)
	{
		result = lstat(strPixel, &statbuf);
		if (result)
		{
			TRACE_ERR("[CAlarmOutManager:%s]lstat file (%s)failed\n", __FUNCTION__, strPixel);
			return -1;
		}
		if (statbuf.st_size > 1024 * 1024)
		{
			TRACE_ERR("[CAlarmOutManager:%s]statbuf.st_size = %d\n", __FUNCTION__, statbuf.st_size);
			return -1;
		}
		fp = fopen(strPixel, "rb");
		if (fp == NULL)
		{
			TRACE_ERR("[CAlarmOutManager:%s]open file (%s)failed\n", __FUNCTION__, strPixel);
			return -1;
		}

		pdata = new char[statbuf.st_size];

		if (pdata == NULL)
		{
			TRACE_ERR("[CAlarmOutManager:%s]new char[%d] failed\n", __FUNCTION__, statbuf.st_size);
			if (fp)
				fclose(fp);
			return -1;
		}
		result = fread(pdata, 1, statbuf.st_size, fp);
		if (result != statbuf.st_size)
		{
			TRACE_ERR("[CAlarmOutManager:%s]fread failed(%d)\n", __FUNCTION__, result);
			fclose(fp);
			delete []pdata;

			return -1;
		}
		fclose(fp);
		hdr = (BITMAPFILEHEADER *) pdata;
		hbi = (BITMAPINFOHEADER *) (pdata + sizeof (BITMAPFILEHEADER));

		snprintf(fileName, sizeof(fileName), "%sAlarmOut_%02d.bmp", FileNameAlarmOutPixel, nPort + 1);
		fp = fopen(fileName, "wb+");
		if (fp == NULL)
		{
			if (pdata)
				delete []pdata;
			pdata = 0;
			TRACE_ERR("[CAlarmOutManager:%s]open file (%s)failed\n", __FUNCTION__, fileName);
			return -1;
		}
		if (24 != SWAP16(hbi->biBitCount) || SWAP16(hdr->bfType) != 0x4d42)
		{
			fclose(fp);
			if (pdata)
				delete []pdata;
			pdata = 0;

			TRACE_ERR("[CAlarmOutManager:%s]not unsuppored bmp type(bitcount =%d,bftype=%x)->ch=%d\n", __FUNCTION__, SWAP16(hbi->biBitCount),
					  SWAP16(hdr->bfType), nPort);
			return -1;
		}
		//开始写文件

		if (SWAP32(hdr->bfSize) != fwrite(pdata, 1, SWAP32(hdr->bfSize), fp))
		{
			fclose(fp);
			if (pdata)
				delete []pdata;
			pdata = 0;

			TRACE_ERR("[CAlarmOutManager:%s]write bmp data failed ->ch=%d\n", __FUNCTION__, nPort);
			return -1;

		}
		fclose(fp);
	}
	//写模式配置
	snprintf(key, sizeof(key), "AlarmOutName_%02d", nPort);
	result = pCConfig->SetValue(AlarmOutSectionStr, key, strName);
	if (result)
	{
		if (pdata)
			delete []pdata;
		pdata = 0;

		TRACE_ERR("[CAlarmOutManager:%s]SetValue Failed\n", __FUNCTION__);
		return result;
	}

	if (pdata)
		delete []pdata;
	pdata = 0;

	return (int) 0;
}

//##ModelId=49B5DEEF00EA
int CAlarmOutManager::DOP_GetLastTime(int nPort)
{
	if (!m_bInit)
	{
		TRACE("[CAlarmOutManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nPort < 0 || nPort >= (m_iEmbAlarmOutNum + m_iExtAlarmOutNum))
	{
		TRACE_ERR("[CAlarmOutManager:%s]nPort = %d -->[0,%d]\n", __FUNCTION__, nPort, m_iExtAlarmOutNum + m_iEmbAlarmOutNum - 1);
		return -1;
	}
	return m_iDelayTime[nPort];

}

//##ModelId=49B5DFA203B9
int CAlarmOutManager::DOP_SetLastTime(int nPort, int nSecond)
{
	char key[30];

	int result;

	if (!m_bInit)
	{
		TRACE("[CAlarmOutManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nPort < 0 || nPort >= (m_iEmbAlarmOutNum + m_iExtAlarmOutNum))
	{
		TRACE_ERR("[CAlarmOutManager:%s]nPort = %d -->[0,%d]\n", __FUNCTION__, nPort, m_iExtAlarmOutNum + m_iEmbAlarmOutNum - 1);
		return -1;
	}

	//写延迟时间
	snprintf(key, sizeof(key), "AlarmOutDelayTime_%02d", nPort);
	result = pCConfig->SetValue(AlarmOutSectionStr, key, nSecond);
	if (result)
	{
		TRACE_ERR("[CAlarmOutManager:%s]SetValue Failed nPort=%d\n", __FUNCTION__, nPort);
		return result;
	}
	m_iDelayTime[nPort] = nSecond;
	
 	return (int) 0;
}

//##ModelId=49B8C599030D
int CAlarmOutManager::DOP_GetPortNum()
{
	if (!m_bInit)
	{
		TRACE("[CAlarmOutManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	return (int) (m_iEmbAlarmOutNum + m_iExtAlarmOutNum);
}

//##ModelId=49B8C5C00213
int CAlarmOutManager::DOP_GetAlarmOutState(int nPort)
{
	int ret;

	if (!m_bInit)
	{
		TRACE("[CAlarmOutManager:%s]not inited\n", __FUNCTION__);
		return 0;
	}
	if (nPort < 0 || nPort >= (m_iEmbAlarmOutNum + m_iExtAlarmOutNum))
	{
		TRACE_ERR("[CAlarmOutManager:%s]nPort = %d -->[0,%d]\n", __FUNCTION__, nPort, m_iExtAlarmOutNum + m_iEmbAlarmOutNum - 1);
		return 0;
	}
	if (nPort < m_iEmbAlarmOutNum)
	{							//基本端口
		ret = theCEmbAlarmOut->GetAlarmOutState(nPort);
	}
	else
	{
		//扩展端口
		ret = theCExtAlarmOut->GetAlarmOutState(nPort - m_iEmbAlarmOutNum);
	}
	if (ret == -1)
	{
		ret = m_bState[nPort];
	}
	return ret;
}

//##ModelId=49B8C5A702AF
int CAlarmOutManager::DOP_SetAlarmOutState(int nPort, int bOn)
{
//    int day;
//    int seg;
//    time_t curr_time;
//    struct tm curr_tm;
	int result = 0;

	if (!m_bInit)
	{
		TRACE("[CAlarmOutManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if (nPort < 0 || nPort >= (m_iEmbAlarmOutNum + m_iExtAlarmOutNum))
	{
		TRACE_ERR("[CAlarmOutManager:%s]nPort = %d -->[0,%d]\n", __FUNCTION__, nPort, m_iExtAlarmOutNum + m_iEmbAlarmOutNum - 1);
		return -1;
	}
	//检查计划
	if ((!CheckSchedule(nPort) || !m_Enable) && bOn)
	{
		return 0;
	}

	if (nPort < m_iEmbAlarmOutNum)
	{							//基本端口
		result = theCEmbAlarmOut->SetAlarmOutState(nPort, bOn);

	}
	else
	{							//扩展端口
		result = theCExtAlarmOut->SetAlarmOutState((nPort - m_iEmbAlarmOutNum), bOn);

	}
	if (result)
	{
		return result;
	}

	m_bState[nPort] = bOn;
	//更新时间
	if (bOn)
	{
		m_iStartTime[nPort] = time(NULL);
	}
	return 0;

}


int CAlarmOutManager::CheckSchedule(int nPort)
{
#if 1
	int day;

	int seg;

	time_t curr_time;

	struct tm curr_tm;

	//一般模式下总是计划中
#if 0
	if (!m_iMode)
	{
		return 1;
	}
#endif

	//获取当前时间段信息
	curr_time = time(NULL);
	localtime_r(&curr_time, &curr_tm);
	day = curr_tm.tm_wday;
	seg = (curr_tm.tm_hour << 1) + (curr_tm.tm_min >= 30);
	//printf("dop status = %d state = %d\n",m_ptAlarmOutSchedule[nPort].dwTimeSeg[day][seg],m_ptAlarmOutSchedule[nPort].bytState);
	//计划检测
	if (m_ptAlarmOutSchedule[nPort].bytState == 1)
	{							//计划启用且为高级模式
		if (m_ptAlarmOutSchedule[nPort].dwTimeSeg[day][seg])
		{						//布防
			return 1;
		}
	}
	else
	{
		return 1;
	}

#else

    stuTimeArea start_time_seg = {0};
    stuTimeArea stop_time_seg = {0};
    /* 模式 */
    switch(m_stuAlarmOutTimeSchedule.nTimeMode)
    {
        case 0: // 夜晚
        {
            memcpy(&start_time_seg, &m_stuNightAreaStartTime, sizeof(stuTimeArea));
            memcpy(&stop_time_seg, &m_stuDayAreaStartTime, sizeof(stuTimeArea));
            break;
        }
        case 1: // 白天
        {
            memcpy(&start_time_seg, &m_stuDayAreaStartTime, sizeof(stuTimeArea));
            memcpy(&stop_time_seg, &m_stuNightAreaStartTime, sizeof(stuTimeArea));
            break;
        }
        case 2: // 全天
        {
            start_time_seg.nHour   = 0;
            start_time_seg.nMinute = 0;
            start_time_seg.nSecond = 0;
            stop_time_seg.nHour    = 23;
            stop_time_seg.nMinute  = 59;
            stop_time_seg.nSecond  = 59;
            break;
        }
        case 3: // 自定义
        {
            memcpy(&start_time_seg, &m_stuAlarmOutTimeSchedule.struTimeSegment[0].struStartTime, sizeof(stuTimeArea));
            memcpy(&stop_time_seg, &m_stuAlarmOutTimeSchedule.struTimeSegment[0].struStopTime, sizeof(stuTimeArea));
            break;
        }
    }

    /* 判断当前时间是否符合给定的时间 */
    if (IsCurrTimeInTimeArea(&start_time_seg, &stop_time_seg))
    {
        /* 计划检测 */
        if (1 == m_ptAlarmOutSchedule[nPort].bytState)
        {
            return 1;
        }
    }

#endif

	return 0;
}

//##ModelId=49B5DE6C01A5
int CAlarmOutManager::DOP_GetSchedule(int nPort, AlarmOutSchedule * pAlarmOutSchedule)
{
	if (!m_bInit)
	{
		TRACE("[CAlarmOutManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (NULL == pAlarmOutSchedule)
	{
		TRACE_ERR("[CAlarmOutManager:%s]nPort = %d invalid argument (NULL == pAlarmOutSchedule)\n", __FUNCTION__, nPort);
		return -1;
	}
	if (nPort < 0 || nPort >= (m_iEmbAlarmOutNum + m_iExtAlarmOutNum))
	{
		TRACE_ERR("[CAlarmOutManager:%s]nPort = %d -->[0,%d]\n", __FUNCTION__, nPort, m_iExtAlarmOutNum + m_iEmbAlarmOutNum - 1);
		return -1;
	}
	memcpy(pAlarmOutSchedule, &m_ptAlarmOutSchedule[nPort], sizeof (AlarmOutSchedule));
	return (int) 0;
}

//##ModelId=49B5DE7501E4
int CAlarmOutManager::DOP_SetSchedule(int nPort, AlarmOutSchedule * pAlarmOutSchedule)
{
	char value[255];

	char key_str[30];

	int i, j;

	int result;

	if (!m_bInit)
	{
		TRACE("[CAlarmOutManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (NULL == pAlarmOutSchedule)
	{
		TRACE_ERR("[CAlarmOutManager:%s]nPort = %d invalid argument (NULL == pAlarmOutSchedule)\n", __FUNCTION__, nPort);
		return -1;
	}
	if (nPort < 0 || nPort >= (m_iEmbAlarmOutNum + m_iExtAlarmOutNum))
	{
		TRACE_ERR("[CAlarmOutManager:%s]nPort = %d -->[0,%d]\n", __FUNCTION__, nPort, m_iExtAlarmOutNum + m_iEmbAlarmOutNum - 1);
		return -1;
	}


	//计划--
	//ScheduleCfgEnable[ch]=[启用]
	//ScheduleCfg[ch]_[星期]=[48]--[启用][星期][半小时]
	snprintf(key_str, sizeof(key_str), "AOScheduleCfgEnable%02d", nPort);
	result = pCConfig->SetValue(AlarmOutSectionStr, key_str, pAlarmOutSchedule->bytState);
	if (result)
	{
		TRACE_ERR("[CAlarmOutManager:%s]AOScheduleCfgEnable SetValue Failed (%d)\n", __FUNCTION__, result);
		return result;
	}
	for (i = 0; i < 7; i++)
	{
		for (j = 0; j < 48; j++)
		{
			value[j] = pAlarmOutSchedule->dwTimeSeg[i][j] + '0';
		}
		value[j] = 0;
		snprintf(key_str, sizeof(key_str), "AOScheduleCfg%02d_%01d", nPort, i);

		result = pCConfig->SetValue(AlarmOutSectionStr, key_str, value);
		if (result)
		{
			TRACE_ERR("[CAlarmOutManager:%s]AOScheduleCfg SetValue Failed (%d)\n", __FUNCTION__, result);
			return result;
		}
		TRACE("[CAlarmOutManager:%s]pAlarmOutSchedule->bytState=%d,Write:%s\n", __FUNCTION__, pAlarmOutSchedule->bytState, value);
	}
	//应用计划
	memcpy(&m_ptAlarmOutSchedule[nPort], pAlarmOutSchedule, sizeof (AlarmOutSchedule));
	return (int) 0;
}

int CAlarmOutManager::CheckStaus()
{
	int i;

	time_t curr_time;

	int bSchedule;
	int alarmOutDelay = 0;

	if (!m_bInit)
	{
		TRACE_WARN("not inited\n", __FUNCTION__);
		return 0;
	}
	if (m_bCheck_exit)
	{
		return -1;
	}
	curr_time = time(NULL);
	for (i = 0; i < (m_iEmbAlarmOutNum + m_iExtAlarmOutNum); i++)
	{
		//本来就是关闭的就不处理
		if (!m_bState[i])
		{
			continue;
		}
		
		bSchedule = CheckSchedule(i);
		if (m_iDelayTime[i] == 0 && bSchedule)
		{						//保持
			continue;
		}
		
		//延迟不能为1，因为报警输入线程每秒去检测一次如果触发
		//输入就设置报警输出并开始计时（为了后面计算报警输出延迟），
		//报警输出计划线程也会每秒去检测计划及输出延迟是否到了，
		//又因为计算是否输出延迟到了是采用time函数获取的所以1秒
		//很容易就到了继而触发关闭报警输出，下次报警输入线程又去打开所以周而复始
		if(1 == m_iDelayTime[i])
			alarmOutDelay = 2;
		else
			alarmOutDelay = m_iDelayTime[i];
		//延迟时间到,或不在计划中，则关掉
		if (curr_time - m_iStartTime[i] >= alarmOutDelay || curr_time - m_iStartTime[i] < 0 || (!bSchedule) || (!m_Enable))
		{
			if (i < m_iEmbAlarmOutNum)
			{					//基本端口 关掉
				theCEmbAlarmOut->SetAlarmOutState(i, 0);
			}
			else
			{
				//扩展端口 关掉
				theCExtAlarmOut->SetAlarmOutState((i - m_iEmbAlarmOutNum), 0);
			}
			m_bState[i] = 0;

            HieServer_RefreshEvent(USEREVENT_ALARM_NOTICE, ALERT_ALARMOUT, 0, m_bState[i]);
		}

	}
	return 0;

}

static void *Fnx_CheckThread(void *lParam)
{
    PRT_TID_NAME("AlarmOutCheckThread");

	int ret = 0;

	CAlarmOutManager *pcAOM = (CAlarmOutManager *) lParam;

	if (NULL == pcAOM)
	{
		TRACE_ERR("[CAlarmOutManager:%s]check thread failed (NULL==pcAOM)\n", __FUNCTION__);
		return NULL;
	}
	while (1)
	{
		ret = pcAOM->CheckStaus();
		if (ret < 0)			//线程退出
		{
			TRACE_ERR("[CAlarmOutManager:%s] check thread quit\n", __FUNCTION__);
			break;
		}
		sleep(1);				//一秒检测一次
	}
	return NULL;
}
