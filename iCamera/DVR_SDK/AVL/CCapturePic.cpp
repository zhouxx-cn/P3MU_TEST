// Copyright (C) 1991 - 1999 Rational Software Corporation

#include <string.h>
#include <stdlib.h>
#include "mysystem.h"
#include <unistd.h>
#include <sys/time.h>
#include "CCoder.h"
#include "common_pub.h"
#include "CCapturePic.h"

//抓图缓冲MAX_PICTURE_BUF_SIZE

static const int BUFFSIZE = MAX_PICTURE_BUF_SIZE;


#if !defined (__USE_MJPEG_CIRCLE_BUFFER__)
static void* CircleCaptureThread(void * lParam);
#endif

CCapturePic::CCapturePic(int nCh, void *hHandle)
{
	int i = 0;
	
	m_iCount = 0;
	m_iIntervalTime = 0;
    m_stCapRes.m_iCapWidth = 0;
    m_stCapRes.m_iCapHeight = 0;
	m_iFormat = 0;
	m_iQuality = 0;
	use_cnt = 0;
	m_buflen = 0;
	m_FrameType = 0;
	m_tFrameTime = 0;
	m_tFrameTime_usec = 0;
	
	//帧缓冲区信息
	m_pFrameBuffer = NULL;

	m_iChannel = nCh;
	m_henc = hHandle;
	m_iLastCaptureTime = 0;
	m_iCount_already = 0;
	m_bAlarm = 0;
	m_pBuff = NULL;

#if defined (__USE_EMAIL__) || defined (__USE_FTP__) || defined (__SUPPORT_SD_CARD__)	

#if !defined (__USE_MJPEG_CIRCLE_BUFFER__)
	m_pFrameBuffer = (char *)malloc(MAX_PICTURE_BUF_SIZE);
	if(NULL == m_pFrameBuffer)
	{
		AVL_ERR("malloc size %d fail!\n", MAX_PICTURE_BUF_SIZE);
		return;
	}
#endif

#endif	
	m_last_AlarmTime = 0;
	if (use_cnt <= 0)
	{
		use_cnt = 0;
		XTRACE_MUTEX_INIT(&m_Capturelock, NULL);
	}
	use_cnt++;

	m_isize = BUFFSIZE;
	pCConfig = CConfig::GetInstance();
#ifndef DEBUG_NOT_USE_SNP_LIB
	pCSnapshotManager = CSnapshotManager::GetInstance();
#endif
	for (i = 0; i < ALARM_ALART_SIZE; i++)
	{
		m_AlarmAlart[i] = 0;
	}

	LoadParam();
	
	XTRACE_MUTEX_INIT(&m_frame_mutex, NULL);

#if !defined (__USE_MJPEG_CIRCLE_BUFFER__)
	pthread_create(&m_CircleCaptureThread, NULL, CircleCaptureThread, this);
#endif
}

CCapturePic::~CCapturePic()
{
#if defined (__USE_EMAIL__) || defined (__USE_FTP__) || defined (__SUPPORT_SD_CARD__)	
	if(m_pFrameBuffer)
	{
		free(m_pFrameBuffer);
		m_pFrameBuffer = NULL;
	}
#endif

	if (use_cnt > 0)
	{
		use_cnt--;
		if (use_cnt == 0)
		{
			XTRACE_MUTEX_DESTORY(&m_Capturelock);
		}
	}
}

//现在抓图部分的代码逻辑
//报警开始抓图，直到本次联动抓图结束，才能引起第二次联动抓图。
//例如：联动60张，间隔5秒，则如果第一次联动发生在第一秒报警就结束了，则在60*5秒内不会响应其他的联动抓图
int CCapturePic::Capture(int bAlarm, int AlarmType, int AlarmCh)
{
	int result = 0;

//	printf("capture pic bAlarm %d AlarmType %d AlarmCh %d\n",bAlarm,AlarmType,AlarmCh);

    /* bAlarm=0 手动抓图；bAlarm=1 联动抓图 */
	if (bAlarm)
	{
		if (AlarmCh < 0 || AlarmCh > (MAX_CHN_NUM-1))
		{
			AVL_ERR("AlarmCh = %d illegal [0,%d]\n", AlarmCh, (MAX_CHN_NUM-1));
			return -1;
		}

		if (AlarmType == ALERT_TYPE_ALARMIN)
		{
			if (m_AlarmAlart[AlarmCh] > 5 && (!m_bAlarm))
			{
				m_bAlarm = 1;
			}
			m_AlarmAlart[AlarmCh] = 0;
		}
		else if (AlarmType == ALERT_TYPE_MOTION)
		{
			if (m_AlarmAlart[MAX_CHN_NUM + AlarmCh] > 5 && (!m_bAlarm))
			{
				m_bAlarm = 1;
			}
			m_AlarmAlart[MAX_CHN_NUM + AlarmCh] = 0;
		}
		else if (ALERT_TYPE_HUMANSHAPE == AlarmType)
		{
			if (m_AlarmAlart[2 * MAX_CHN_NUM + AlarmCh] > 5 && (!m_bAlarm))
			{
				m_bAlarm = 1;
			}
			m_AlarmAlart[2 * MAX_CHN_NUM + AlarmCh] = 0;
		}
		else if (AlarmType == ALERT_TYPE_SOUNDDETECT)
		{
			if (m_AlarmAlart[3 * MAX_CHN_NUM + AlarmCh] > 5 && (!m_bAlarm))
			{
				m_bAlarm = 1;
			}
			m_AlarmAlart[3 * MAX_CHN_NUM + AlarmCh] = 0;
		}
		else
		{
			AVL_ERR("unsupported type 0x%08x\n", AlarmType);
			return -1;
		}
	}
	else
	{
		XTRACE_MUTEX_LOCK(&m_Capturelock);
		//修改为按需分配，降低内存的峰值
		m_isize = BUFFSIZE;
		if(NULL == m_pBuff)
		{
			m_pBuff = (char*)malloc(m_isize);
			if(NULL == m_pBuff)
			{
				AVL_ERR("malloc size %d failed\n", m_isize);
				XTRACE_MUTEX_UNLOCK(&m_Capturelock);
				return -1;
			}
		}

	#if defined (__USE_MJPEG_CIRCLE_BUFFER__)
		if (0 != (result = GetMjpegBuffer(0, 0, &m_pBuff, (int *)&m_isize, BUFFSIZE, NULL)))
		{
			if(m_pBuff)
			{
				free(m_pBuff);
				m_pBuff = NULL;
			}
			AVL_ERR("GetMjpegBuffer failed, result=%d\n", result);
			XTRACE_MUTEX_UNLOCK(&m_Capturelock);
			return -1;
		}
	#else
		AVL_ERR("Capture failed\n");
		XTRACE_MUTEX_UNLOCK(&m_Capturelock);
		return -1;
	#endif
		AVL_INFO("size=%d\n", m_isize);

		FILE *fd=NULL;
		fd = fopen(REMOTE_CAPTURE_IMAGE_FILE_NAME, "wb");
		if(fd == NULL)
		{
			if(m_pBuff)
			{
				free(m_pBuff);
				m_pBuff = NULL;
			}
			AVL_ERR("open file %s failed\n", REMOTE_CAPTURE_IMAGE_FILE_NAME);
			XTRACE_MUTEX_UNLOCK(&m_Capturelock);
			return -1;
		}
		fwrite(m_pBuff,m_isize,1,fd);
		fclose(fd);

		if(m_pBuff)
		{
			free(m_pBuff);
			m_pBuff = NULL;
		}
		XTRACE_MUTEX_UNLOCK(&m_Capturelock);
	}

	return (int) 0;
}

//!\brief 从流里面获取一帧数据作为抓图数据
//!\note  时间精确到ms，图片大小不超过MAX_PICTURE_BUF_SIZE，可以和远程抓图同时进行
//!\return  成功为0，失败为-1
int CCapturePic::Capture()
{
#if defined (__USE_MJPEG_CIRCLE_BUFFER__)
	return 0;
#endif

	int result = -1;
	int buflen = 0;
	time_t tFrameTime = 0;
	time_t tFrameTime_usec = 0;

#if defined (__USE_EMAIL__) || defined (__USE_FTP__) || defined (__SUPPORT_SD_CARD__)	
	char framebuffer[MAX_PICTURE_BUF_SIZE] = {0};

	XTRACE_MUTEX_LOCK(&m_frame_mutex);

	buflen = m_buflen;
	tFrameTime = m_tFrameTime;
	tFrameTime_usec = m_tFrameTime_usec;
	if (m_buflen <= MAX_PICTURE_BUF_SIZE)
	{
	    memcpy(framebuffer, m_pFrameBuffer, m_buflen);
	}
	XTRACE_MUTEX_UNLOCK(&m_frame_mutex);
	
#ifndef DEBUG_NOT_USE_SNP_LIB
	result = pCSnapshotManager->CapturePic(tFrameTime, tFrameTime_usec, framebuffer, buflen);
#endif
#endif

	return result;
}

//!\brief 获取一帧流数据，放入缓冲区，用于抓图
//!\return  成功为0，失败为-1
int CCapturePic::InputData(void *buff, int buflen, int FrameType, time_t tFrameTime, time_t tFrameTime_usec)
{
#if defined (__USE_MJPEG_CIRCLE_BUFFER__)
	return 0;
#endif

    if (NULL == buff || buflen <= 0)
    {
		AVL_ERR("NULL==buff || buflen = %d <= 0\n", buflen);
		return -1;
    }

#if defined (__USE_EMAIL__) || defined (__USE_FTP__) || defined (__SUPPORT_SD_CARD__)    
    H264DATAPACKETHEADER *pPack = (H264DATAPACKETHEADER *)buff;
    // H264:0x00, H265:0x04, MJPEG:0x05(暂时只支持MJPEG流抓图)
    if (0x05 != pPack->byLoadType)
    {
        AVL_ERR("Frame LoadType %#x is not mjpeg(0x05)!\n", pPack->byLoadType);
        return -1;
    }
    
	if(0 == XTRACE_MUTEX_TRYLOCK(&m_frame_mutex))  
	{
		//memset(m_framebuffer, 0, m_buflen);
	    m_buflen = buflen-28;                       // 帧缓冲大小(最大暂定为MAX_PICTURE_BUF_SIZE)
	    m_FrameType = FrameType;                    // 帧类型
	    m_tFrameTime = tFrameTime;                  // 时间戳(秒)
	    m_tFrameTime_usec = tFrameTime_usec;        // 时间戳(微秒)
	    if (m_buflen <= MAX_PICTURE_BUF_SIZE)
	    {
	        memcpy(m_pFrameBuffer, (char *)buff+28, m_buflen);  // 帧缓冲区(纯数据)(最大暂定为MAX_PICTURE_BUF_SIZE)
	    }
	    
	    XTRACE_MUTEX_UNLOCK(&m_frame_mutex);
	}
#endif
	
	return 0;
}

//##ModelId=49BDEFE00263
int CCapturePic::GetCaptureCfg(int *nCount, int *nIntervalTime, int *nFormat, int *nWidth, int *nHeight, int *nQuality)
{
	if (NULL == nCount || NULL == nIntervalTime)
	{
		AVL_ERR("NULL==nCount||NULL==nIntervalTime\n");
		return -1;
	}

	*nCount = m_iCount;
	*nIntervalTime = m_iIntervalTime;

	if (NULL != nWidth && NULL != nHeight)
	{
	    *nWidth = m_stCapRes.m_iCapWidth;
	    *nHeight = m_stCapRes.m_iCapHeight;
	}

	if (NULL != nQuality)
	{
		*nQuality = m_iQuality;
	}
	return (int) 0;
}

//##ModelId=49BDEFE002DB
int CCapturePic::SetCaptureCfg(int nCount, int nIntervalTime, int nFormat, int nWidth, int nHeight, int nQuality)
{
	if (nCount < 0)
	{
		return -1;
	}

	CConfig::GetInstance()->SetValue(CAPTURE_PIC_CFG_SECTION, KEY_CAPTURE_PIC_COUNT, nCount);
	CConfig::GetInstance()->SetValue(CAPTURE_PIC_CFG_SECTION, KEY_CAPTURE_PIC_INTERVAL, nIntervalTime);
    if (nWidth > 0 && nHeight > 0 && (nWidth != m_stCapRes.m_iCapWidth || nHeight != m_stCapRes.m_iCapHeight))
    {
	    CConfig::GetInstance()->SetValue(CAPTURE_PIC_CFG_SECTION, KEY_CAPTURE_PIC_WIDTH, nWidth);
	    CConfig::GetInstance()->SetValue(CAPTURE_PIC_CFG_SECTION, KEY_CAPTURE_PIC_HEIGHT, nHeight);

	    m_stCapRes.m_iCapWidth = nWidth;
	    m_stCapRes.m_iCapHeight = nHeight;
	}

	CConfig::GetInstance()->SetValue(CAPTURE_PIC_CFG_SECTION, KEY_CAPTURE_PIC_QUALITY, nQuality);
	m_iCount = nCount;
	m_iIntervalTime = nIntervalTime;

	m_iQuality = nQuality;//0是高，1是中等，2是低

	ZWSys_EncSetSnapPara(m_henc, (int)&m_stCapRes, m_iQuality);
	return (int) 0;
}

int CCapturePic::GetCaptureStreamType(char *nStreamType)
{
	if (NULL == nStreamType)
	{
		AVL_ERR("NULL==nStreamType\n");
		return -1;
	}

	*nStreamType = m_nStreamType;

	return (int) 0;
}

int CCapturePic::SetCaptureCfgV2(int nQualitychar, char nStreamType)
{
	if ((nStreamType < 0 || nStreamType > 2) || (nQualitychar < 0 || nQualitychar > 2))
	{
		AVL_ERR("StreamType = %d Qualitychar = %d\n", nStreamType, nQualitychar);
		return -1;
	}

	CConfig::GetInstance()->SetValue(CAPTURE_PIC_CFG_SECTION, KEY_CAPTURE_PIC_QUALITY, nQualitychar);
	m_iQuality = nQualitychar;

	CConfig::GetInstance()->SetValue(CAPTURE_PIC_CFG_SECTION, KEY_CAPTURE_PIC_STREAMTYPE, nStreamType);
	m_nStreamType = nStreamType;

	ZWSys_EncSetSnapParaV2(m_henc, m_iQuality, m_nStreamType);
	
	return (int) 0;
}

int CCapturePic::LoadParam()
{
	m_iCount = pCConfig->GetValue(CAPTURE_PIC_CFG_SECTION, KEY_CAPTURE_PIC_COUNT, 1);	
	m_iIntervalTime = pCConfig->GetValue(CAPTURE_PIC_CFG_SECTION, KEY_CAPTURE_PIC_INTERVAL, 5);

	m_stCapRes.m_iCapWidth = pCConfig->GetValue(CAPTURE_PIC_CFG_SECTION, KEY_CAPTURE_PIC_WIDTH, 352);
	m_stCapRes.m_iCapHeight = pCConfig->GetValue(CAPTURE_PIC_CFG_SECTION, KEY_CAPTURE_PIC_HEIGHT, 288);
	m_iQuality = pCConfig->GetValue(CAPTURE_PIC_CFG_SECTION, KEY_CAPTURE_PIC_QUALITY, 1);
	m_nStreamType = pCConfig->GetValue(CAPTURE_PIC_CFG_SECTION, KEY_CAPTURE_PIC_STREAMTYPE, 0);

	return 0;
}

int CCapturePic::CheckCapturePic()
{
	int i;
	time_t curr_time;
//  struct timeval now_tv;

	if (NULL == m_henc)
	{
		return -1;
	}
	//空闲计数
	for (i = 0; i < ALARM_ALART_SIZE; i++)
	{
		m_AlarmAlart[i]++;
	}

	if (m_bAlarm)
	{
		curr_time = time(NULL);
		if (m_iCount_already <= 0)
		{
			m_iLastCaptureTime = 0;
		}
		//  TRACE_WARN("[%s] left=%d,ctime=%d,ltime=%d,itime=%d\n",__FUNCTION__,m_iCountLeft,curr_time,m_iLastCaptureTime,m_iIntervalTime);
		if (m_iCount_already < m_iCount && (curr_time - m_iLastCaptureTime >= m_iIntervalTime))
		{
			//时间	
		}
		if (m_iCount_already >= m_iCount)
		{
			m_iLastCaptureTime = 0;
			m_iCount_already = 0;
			m_bAlarm = 0;

		}
		else if (curr_time - m_iLastCaptureTime < 0)
		{
			m_iLastCaptureTime = curr_time;
		}
	}
	return 0;
}

#if !defined (__USE_MJPEG_CIRCLE_BUFFER__)
void* CCapturePic::CircleCaptureThreadProcess()
{
    int result = 0;
    
    while(1)
    {
        result = Capture();
        if (result)
        {
            AVL_ERR("Circle Capture Picture fail!, ret = %d\n", result);
            //continue;
        }
        
        usleep(200000);
    }
    return NULL;
}

static void* CircleCaptureThread(void * lParam)
{
    PRT_TID();

	//获取当前实例
	CCapturePic *pCCapturePic = (CCapturePic *)lParam;

	if (NULL != pCCapturePic)
	{
      	 	pCCapturePic->CircleCaptureThreadProcess();
	}
	else
	{
		AVL_ERR("get pNet instance fail\n");
	}

	return NULL;
}
#endif
