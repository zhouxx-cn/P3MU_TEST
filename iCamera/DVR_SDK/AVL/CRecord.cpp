// Copyright (C) 1991 - 1999 Rational Software Corporation
#include <string.h>
#include "CRecord.h"
#include "zwsdk.h"
#include "ddt_so.h"
//!\sleep()支持
#include <unistd.h>
#include "AVL_global.h"
#include "DeviceAbility.h"
#include "MemRecord.h"

////////////////////

extern int bSupportRtmp;

extern "C" int mysystem(const char *cmdstringl);

void* GetCurrentResolutionThread(void * lParam);

//预录缓冲大小
static const int PREV_MAX_LEN = (sizeof (PREV_DATA_T));//(6 * 1024 * 1024);加大内存后打开此功能

//配置文件-字符串值缓冲大小
static const int VALUE_MAX_LEN = (256);

//录像配置区名
static const char *RecordCfgSection = "RecordCfg";

static const char *HighPicQuality = "HighPicQuality_";

static const char *NorPicQuality = "NorPicQuality_";

static const char *ComPicQuality = "ComPicQuality_";

static const char *CustomPicQuality = "CustomPicQuality_";

//缺省编码配置
static const char *DefEncCfgStr = "0:255:1:0:0:0:512:0";

/* 时间转化为时间戳 */
#define TIME_ULONG_TO_TIMESTAMP(year, mon, day, hour, min, sec)                     \
        ({                                                                          \
            struct tm tm;                                                           \
            tm.tm_year = year - 1900;                                               \
            tm.tm_mon = mon - 1;                                                    \
            tm.tm_mday = day;                                                       \
            tm.tm_hour = hour;                                                      \
            tm.tm_min = min;                                                        \
            tm.tm_sec = sec;                                                        \
            (mktime(&tm));                                                          \
         })

//缺省计划配置


static const int PREV_DATA_HEAD_LEN = (sizeof (PREV_DATA_T));

int CRecord::m_iCustomMode = 0;
int CRecord::m_iStandard = StandardPAL;

//##ModelId=49A260A10246
CRecord::CRecord(int nCh, HANDLE henc)
{
	m_channel = nCh;
	m_iDelayTime = 0;
    m_iPreTime   = 0;

	m_iRecState = RECORD_TYPE_NO;
	m_iRecSchedulType = RECORD_TYPE_NO;
	m_iLastRecType = RECORD_TYPE_NO;
	m_iLastRecMode = RECORD_MODE_CLOSE;
	m_iRecMode = RECORD_MODE_CLOSE;

    m_iRecStreamIndex = AUX_STREAM_RECORD_INDEX;

	m_bMotionAlarmRec = 0;
	m_bAiAlarmRec = 0;
	
	m_bAlarmInAlarmRec = 0;

    m_bHttpEventIsComming = 0;
        
    m_iHttpEventRecTime = 0;

    m_iShowAviNumPerPage = 50;

    m_iShowPicNumPerPage = 50;

    m_iSwitchPageIsChoose = 1;

    m_iSwitchPicPageIsChoose = 1;

    m_iRecordPathType = 0;

    m_iPicturePathType = 0;

    m_iVideoRate = 90;
#ifdef __SUPPORT_SD_CARD__
	pCDPManager = NULL;
#endif	
	pCConfig = NULL;
	m_bPause = 0;
	m_bPrevRecord = 0;
	m_hEnc = henc;
	m_bChangeCfg = 0;
	pRecordCfg = NULL;
	m_iStandard = 0;
	m_bNeedSwitch = 0;
	m_bForceSwitch = 0;
	//sensor 输入频率
	m_SensorFrequency = CMOS_FREQUENCY_50HZ;
	memset(m_szType, 0 , sizeof(m_szType));
	strncpy(m_szType[0], "HC", sizeof(m_szType[0]) - 1);	//CIF
	strncpy(m_szType[1], "HD", sizeof(m_szType[1]) - 1);	//D1
	strncpy(m_szType[2], "H2C", sizeof(m_szType[2]) - 1);	//2CIF
	strncpy(m_szType[3], "720P", sizeof(m_szType[3]) - 1);	//720P 1280*720
	strncpy(m_szType[4], "SXGA", sizeof(m_szType[4]) - 1);	//SXGA 1280*1024
	strncpy(m_szType[5], "UXGA", sizeof(m_szType[5]) - 1);	//UXGA 1600*1200
	strncpy(m_szType[6], "1080P", sizeof(m_szType[6]) - 1);	//1080P 1920*1080 
	strncpy(m_szType[7], "576P", sizeof(m_szType[7]) - 1);	//576P 960*
	strncpy(m_szType[8], "VGA", sizeof(m_szType[8]) - 1);	//576P 960*576 

	memset(&m_ScheduleCfg, 0, sizeof (tagRecordSchedule));
    memset(&m_AdvanceEncCfg, 0, sizeof(tagAdvanceEncCfg));
	memset(&m_ScheduleCfgV2, 0, sizeof(tagRecordScheduleV2));

    m_nSearchAviStartTime = 0;
    m_nSearchAviStopTime  = 0;
    m_nSearchPicStartTime = 0;
    m_nSearchPicStopTime  = 0;

    m_nSearchAviTotalNum = 0;
    m_nSearchPicTotalNum = 0;

	DefEncCfg();
}

CRecord::~CRecord()
{
	ZWSys_EncStopVideoCapture(m_hEnc);
	XTRACE_MUTEX_DESTORY(&m_lock);
}

int CRecord::Init()
{
	int result;
	int ch=0;
	int total_channel=ZWSys_GetTotalEncChannels();

	#ifdef __SUPPORT_SD_CARD__
	pCDPManager = CDPManager::GetInstance();
	if (pCDPManager == NULL)
	{
		AVL_ERR("[CRecord:%s]pCDPManager==NULL ch=%d\n", __FUNCTION__, m_channel);
		return -1;
	}
	#endif

	pCConfig = CConfig::GetInstance();
	if (pCConfig == NULL)
	{
		AVL_ERR("[CRecord:%s]pCConfig==NULL\n", __FUNCTION__);
		return -1;
	}
	//需要放到最开始，否则会在input函数里出现概率性锁未初始化
	XTRACE_MUTEX_INIT(&m_lock, NULL);

	//开始load配置[RecordCfg]
	//定制模式--RecCustomMode
	//普通配置--GeneralEncCfg[ch]--分辨率：帧率：码流类型：码率类型：质量：量化系数：码率上限：码率下限
	//手动配置--ManualEncCfg[ch]
	//事件配置--EventEncCfg[ch]
	//录像计划--
	//ScheduleCfgEnable[ch]=[启用]
	//ScheduleCfg[ch]_[星期]=[48]--[启用][星期][半小时]
	//预录时间--PrevRecTime[ch]
	//延迟录时间--DelayRecTime[ch]
	
	LoadParam();
	
	//开始编码
	StartEncCapture();
	
	//设置制式
	//应用制式
//	result = MP4Sys_SetSystemStandard((VideoOutStandard_t) m_iStandard);
	for (ch=0;ch<total_channel;ch++)
	{
		result = ZWSys_SetCVBSOutStandard(ZWSys_EncChannelOpen(ch),(VideoOutStandard_t)m_iStandard);
		if (result)
		{
			AVL_ERR("ZWSys_SetCVBSOutStandard(%d)\n", result);
		}
	}

    result = pthread_create(&m_GetCurrentResolutionThread, NULL, GetCurrentResolutionThread, this);
    if (result < 0)
    {
        AVL_ERR("pthread_create<m_GetCurrentResolutionThread> failed(%d)\n", result);
    }

	return (int) 0;
}

//!\brief 获取制式
//!\param int *pStandard,接受制式,1-NTSC,2-PAL,4-SECAM,0x80000000-none
//!\return 成功0,否则失败
//##ModelId=49B4856A0139
int CRecord::GetStandard(int *pStandard)
{
	if (pStandard == NULL)
	{
		AVL_ERR("pStandard == NULL\n");
		return -1;
	}

	BoardSensorType_t SensorType;
	char szSection[30];
	CConfig *pCfgFile = CConfig::GetInstance();
	int inputF;
	
	ZWSys_GetSensorType(&SensorType);
    SensorType = (BoardSensorType_t)0x1;
	snprintf(szSection, sizeof(szSection), "ISP_S%d", SensorType);
	inputF = pCfgFile->GetValue(szSection, "InputFrequency", 0);
	if(1 == inputF)//60Hz
		*pStandard = StandardNTSC;
	else
		*pStandard = StandardPAL;
	
	return 0;
}

//!\brief 设置制式
//!\param int Standard,1-NTSC,2-PAL,4-SECAM
//!\return 成功0,否则失败
int CRecord::SetStandard(int Standard)
{
	int result;
	int Total_channel=ZWSys_GetTotalEncChannels();
	int ch=0;

	if (!(Standard == StandardNTSC || Standard == StandardPAL || Standard == StandardSECAM))
	{
		AVL_ERR("Standard = %d [%d,%d,%d]\n", Standard, StandardNTSC, StandardPAL, StandardSECAM);
		return -1;
	}
	//制式未改变
	if (Standard == m_iStandard)
	{
		AVL_WARN("standard =%d ,no Change\n", Standard);
		return 0;
	}
	m_iStandard = Standard;
	result = CConfig::GetInstance()->SetValue(RecordCfgSection, "VideoStandard", m_iStandard);
	if (result)
	{
		AVL_ERR("SetValue Failed\n");
		return -1;
	}
	
	//应用制式

	for (ch=0;ch<Total_channel;ch++)
	{
		result = ZWSys_SetCVBSOutStandard(ZWSys_EncChannelOpen(ch),(VideoOutStandard_t) m_iStandard);

		if (result)
		{
			AVL_ERR("ZWSys_SetCVBSOutStandard(%d)\n", result);
			return -1;
		}
	}
	return 0;

}

int CRecord::ForceSwicth()
{
	m_bForceSwitch = 1;
	return 0;
}

//!\brief 开启编码
//!\return 成功0,其它失败
int CRecord::StartEncCapture()
{
	int result;

	//应用配置
	m_bChangeCfg = 1;
	CheckEncCfg();
	TRACE_AVL("\n...CRecord::StartEncCapture()....brief 开启编码.....\n");
	result = ZWSys_EncStartVideoCapture(m_hEnc);
	if (result)
	{
		AVL_ERR("ZWSys_EncStartVideoCapture(%d),ch=%d\n", result, m_channel);
		return result;
	}
	TRACE_AVL("\n...CRecord::StartEncCapture()....brief 开启编码成功....\n");
	return 0;
}

//!\brief 停止编码
//!\return 成功0,其它失败
int CRecord::StopEncCapture()
{
	int result;

	//停止编码
	TRACE_AVL("\n...CRecord::StopEncCapture()....停止编码.....\n");
	result = ZWSys_EncStopVideoCapture(m_hEnc);
	if (result)
	{
		AVL_ERR("MP4Sys_EncStopCodec(%d),ch=%d\n", result, m_channel);
		return result;
	}
	TRACE_AVL("\n...CRecord::StopEncCapture()....停止编码完成.....\n");
	return 0;
}

int CRecord::DefEncCfg()
{
	m_iEncCfgtype = 0;			//普通配置
	m_iCustomMode = 0;			//一般模式
	RecordCfg[0].byBitrateType = 1;	//定码率
	RecordCfg[0].byPicQuality = 1;	//标准质量
	RecordCfg[0].byResolution = eResolutionIndex_D1;	//D1
	RecordCfg[0].byStreamType = 1;	//音视频流
	RecordCfg[0].dwQuotiety = 5;	//量化系数最大
	RecordCfg[0].dwVideoBitrateH = 1024;	//1M上限
	RecordCfg[0].dwVideoBitrateL = 32;	//32K下限
	RecordCfg[0].dwVideoFrameRate = 0xFF;	//满帧

	RecordCfg[1].byBitrateType = 1;	//定码率
	RecordCfg[1].byPicQuality = 1;	//标准质量
	RecordCfg[1].byResolution = eResolutionIndex_CIF;	//CIF
	RecordCfg[1].byStreamType = 0;	//音视频流
	RecordCfg[1].dwQuotiety = 5;	//量化系数最大
	RecordCfg[1].dwVideoBitrateH = 1024;	//1M上限
	RecordCfg[1].dwVideoBitrateL = 32;	//32K下限
	RecordCfg[1].dwVideoFrameRate = 0xFF;	//满帧

	RecordCfg[2].byBitrateType = 1;	//定码率
	RecordCfg[2].byPicQuality = 1;	//标准质量
	RecordCfg[2].byResolution = eResolutionIndex_CIF;	//CIF
	RecordCfg[2].byStreamType = 1;	//音视频流
	RecordCfg[2].dwQuotiety = 5;	//量化系数最大
	RecordCfg[2].dwVideoBitrateH = 1024;	//1M上限
	RecordCfg[2].dwVideoBitrateL = 32;	//32K下限
	RecordCfg[2].dwVideoFrameRate = 0xFF;	//满帧
	return 0;

}

//##ModelId=49A25F8E02EB
int CRecord::Pause(int bPause)
{
	return 0;
}

int CRecord::StartRec(enumRecType recType)
{
	if (recType != m_iRecSchedulType)
    {
#ifdef __SUPPORT_SD_CARD__
        pCDPManager->DR_SetRecordType(0, (int)recType);
#endif
        m_iRecSchedulType = recType;
    }
	return 0;
}

//!\brief 设置录像写入状态 0:暂停  1:写入
int CRecord::SetRecordWriteStatus(int record_write_status)
{
#ifdef __SUPPORT_SD_CARD__
	return pCDPManager->DR_SetRecordWriteStatus(0, record_write_status);
#else
	return 0;
#endif
}

int CRecord::StartMode(enumRecordMode model)
{
	return (int) 0;
}

/*@breif SetCurrentAviNumPerPage 设置每页显示多少个AVI文件
 *@param [IN]  avi_num_per_page 每页显示的AVI文件个数
 *@return 成功返回0
 *@       失败返回-1
 *@note  每页显示的AVI文件个数不能超过当前有效的AVI个数g_read_curr_avi_num，
 *@      每页显示的AVI文件个数不能超过最大能显示的AVI个数MAX_AVI_NUM_ONE_PAGE
 */
int CRecord::SetCurrentAviNumPerPage(int avi_num_per_page)
{
    if (avi_num_per_page == (int)m_iShowAviNumPerPage)
    {
        return (int) 0;
    }
#ifdef __SUPPORT_SD_CARD__
    pCDPManager->DR_SetCurrentAviNumPerPage(avi_num_per_page);
#endif
    m_iShowAviNumPerPage = avi_num_per_page;

	pCConfig->SetValue(RecordCfgSection, "ShowAviNumPerPage", m_iShowAviNumPerPage);

    return 0;
}

/*@breif GetCurrentAviNumPerPage 获取显示多少个AVI文件
 *@return 成功返回AVI文件个数
 *@      失败返回-1
 */
int CRecord::GetCurrentAviNumPerPage(void)
{
    return m_iShowAviNumPerPage;
}

/*@breif GetCurrentAviTotalNum 获取当前AVI总个数
 *@return 成功返回当前总的AVI文件个数
 *@      失败返回-1
 */
int CRecord::GetCurrentAviTotalNum(void)
{
#ifdef __SUPPORT_SD_CARD__	
    return pCDPManager->DR_GetCurrentAviTotalNum();
#else
	return 0;
#endif	
}

/*@breif SetSwitchPageIsChoose 设置当前选中的是哪一个页面和当前录像路径类型
 *@param [IN]  choose_page 选中的页面(从1开始)
 *@param [IN]  path_type 当前录像路径类型 0: http路径  1:绝对路径
 *@return 成功返回0
 *@      失败返回-1
 */
int CRecord::SetSwitchPageIsChoose(int choose_page, int path_type)
{
    if (choose_page==(int)m_iSwitchPageIsChoose && path_type==(int)m_iRecordPathType)
    {
        return (int) 0;
    }

    m_iSwitchPageIsChoose = choose_page;
    m_iRecordPathType = path_type;
#ifdef __SUPPORT_SD_CARD__
    return pCDPManager->DR_SetSwitchPageIsChoose(choose_page, path_type);
#else
	return 0;
#endif	
}

/*@breif GetSwitchPageIsChoose 获取当前选中的是哪一个页面
 *@return 成功返回当前选中的是哪一个页面
 *@       失败返回-1
 */
int CRecord::GetSwitchPageIsChoose(void)
{
    return m_iSwitchPageIsChoose;
}

/*@breif GetCurrentAviListInfo 从给定的页面里面获取当前AVI文件列表
 *@param [OUT] p_avi_list 获取到的AVI文件列表
 *@param [OUT] p_avi_num_curr_page 当前页面有多少个AVI文件
 *@return 成功返回0
 *@      失败返回-1
 *@note  需要先调用SetSwitchPageIsChoose设置当前选中哪一页
 *@      需要先调用SetCurrentAviSearchTimeSeg
 */
int CRecord::GetCurrentAviListInfo(void **p_avi_list, int *p_avi_num_curr_page)
{
#ifdef __SUPPORT_SD_CARD__	
    return pCDPManager->DR_GetCurrentAviListInfo(   m_iSwitchPageIsChoose, p_avi_list, p_avi_num_curr_page);
#else
	return 0;
#endif
}

/* 设置当前要寻找的AVI的时间段 */
int CRecord::SetCurrentAviSearchTimeSeg(DateTime start_time, DateTime stop_time)
{
    unsigned int nStartTime = 0;
    nStartTime = TIME_ULONG_TO_TIMESTAMP(start_time.nYear, 
                                         start_time.nMonth, 
                                         start_time.nDay, 
                                         start_time.nHour, 
                                         start_time.nMinute, 
                                         start_time.nSecond);
    unsigned int nStopTime = 0;
    nStopTime = TIME_ULONG_TO_TIMESTAMP(stop_time.nYear, 
                                        stop_time.nMonth, 
                                        stop_time.nDay, 
                                        stop_time.nHour, 
                                        stop_time.nMinute, 
                                        stop_time.nSecond);

    if (nStartTime >= nStopTime)
    {
        printf("avi start_time %d >= stop_time %d invalid!\n", nStartTime, nStopTime);
        return -1;
    }
#ifdef __SUPPORT_SD_CARD__
    pCDPManager->DR_UpdateAviListByTime(nStartTime, nStopTime, &m_nSearchAviTotalNum);
#endif
    return 0;
}

/* 设置当前要寻找的PIC的时间段 */
int CRecord::SetCurrentPicSearchTimeSeg(DateTime start_time, DateTime stop_time)
{
    unsigned int nStartTime = 0;
    nStartTime = TIME_ULONG_TO_TIMESTAMP(start_time.nYear, 
                                         start_time.nMonth, 
                                         start_time.nDay, 
                                         start_time.nHour, 
                                         start_time.nMinute, 
                                         start_time.nSecond);
    unsigned int nStopTime = 0;
    nStopTime = TIME_ULONG_TO_TIMESTAMP(stop_time.nYear, 
                                        stop_time.nMonth, 
                                        stop_time.nDay, 
                                        stop_time.nHour, 
                                        stop_time.nMinute, 
                                        stop_time.nSecond);

    if (nStartTime >= nStopTime)
    {
        printf("pic start_time %d >= stop_time %d invalid!\n", nStartTime, nStopTime);
        return -1;
    }
#ifdef __SUPPORT_SD_CARD__
    pCDPManager->DR_UpdatePicListByTime(nStartTime, nStopTime, &m_nSearchPicTotalNum);
#endif
    return 0;
}

/*@breif SetCurrentPicNumPerPage 设置每页显示多少个图片文件
 *@param [IN]  pic_num_per_page 每页显示的图片文件个数
 *@return 成功返回0
 *@      失败返回-1
 *@note  每页显示的图片文件个数不能超过当前有效的图片个数g_read_curr_pic_num，
 *@      每页显示的图片文件个数不能超过最大能显示的图片个数MAX_PIC_NUM_ONE_PAGE
 */
int CRecord::SetCurrentPicNumPerPage(int pic_num_per_page)
{
    if (pic_num_per_page == (int)m_iShowPicNumPerPage)
    {
        return (int) 0;
    }
#ifdef __SUPPORT_SD_CARD__
    pCDPManager->DR_SetCurrentPicNumPerPage(pic_num_per_page);
#endif
    m_iShowPicNumPerPage = pic_num_per_page;

	pCConfig->SetValue(RecordCfgSection, "ShowPicNumPerPage", m_iShowPicNumPerPage);

    return 0;
}

/*@breif GetCurrentAviNumPerPage 获取显示多少个图片文件
 *@return 成功返回图片文件个数
 *@       失败返回-1
 */
int CRecord::GetCurrentPicNumPerPage(void)
{
    return m_iShowPicNumPerPage;
}

/*@breif GetCurrentPicTotalNum 获取当前图片总个数
 *@return 成功返回当前总的图片文件个数
 *@       失败返回-1
 */
int CRecord::GetCurrentPicTotalNum(void)
{
#ifdef __SUPPORT_SD_CARD__
    return pCDPManager->DR_GetCurrentPicTotalNum();
#else
	return 0;
#endif
}

/*@breif SetSwitchPicPageIsChoose 设置当前选中的是哪一个页面和当前图片路径类型
 *@param [IN]  choose_page 选中的页面(从1开始)
 *@param [IN]  path_type 当前图片路径类型 0: http路径  1:绝对路径
 *@return 成功返回0
 *@       失败返回-1
 */
int CRecord::SetSwitchPicPageIsChoose(int choose_page, int path_type)
{
    if (choose_page==(int)m_iSwitchPicPageIsChoose && path_type==(int)m_iPicturePathType)
    {
        return (int) 0;
    }

    m_iSwitchPicPageIsChoose = choose_page;
    m_iPicturePathType = path_type;

    return 0;
}

/*@breif GetSwitchPageIsChoose 获取当前选中的是哪一个页面
 *@return 成功返回当前选中的是哪一个页面
 *@       失败返回-1
 */
int CRecord::GetSwitchPicPageIsChoose(void)
{
    return m_iSwitchPicPageIsChoose;
}

/*@breif GetCurrentPicListInfo 从给定的页面里面获取当前图片文件列表
 *@param [OUT] p_pic_list 获取到的图片文件列表
 *@param [OUT] p_pic_num_curr_page 当前页面有多少个图片文件
 *@return 成功返回0
 *@      失败返回-1
 *@note  需要先调用SetSwitchPicPageIsChoose设置当前选中哪一页
 *@      需要先调用SetCurrentPicNumPerPage设置当前用户需要设置每页显示多少个图片文件
 */
int CRecord::GetCurrentPicListInfo(void **read_pic_page, int *p_pic_num_curr_page)
{
#ifdef __SUPPORT_SD_CARD__
    return pCDPManager->DR_GetCurrentPicListInfo(m_iSwitchPicPageIsChoose, read_pic_page, p_pic_num_curr_page);
#else
	return 0;
#endif
}

/* 获取文件对应的文件信息 */
int CRecord::GetRecordFileInfoByFileName(const char *file_name, void* p_file_info)
{
#ifdef __SUPPORT_SD_CARD__
    return pCDPManager->DR_GetRecordFileInfoByFileName(file_name, p_file_info);
#else
	return 0;
#endif	
}

/* 获取磁盘资源占用情况(单位: MB) */
int CRecord::GetDiskResourceSize(int *pRecordMB, int *pPictureMB)
{
#ifdef __SUPPORT_SD_CARD__
    return pCDPManager->DR_GetDiskResourceSize(pRecordMB, pPictureMB);
#else
	return 0;
#endif	
}

/* 获取录像存放的比例(范围0~100) */
int CRecord::GetVideoUsedSpaceRate(void)
{
    //return pCDPManager->DR_GetVideoUsedSpaceRate();
    return m_iVideoRate;
}

/* 设置录像存放的比例(范围0~100) */
int CRecord::SetVideoUsedSpaceRate(int nVideoSpace)
{
    if (nVideoSpace == (int)m_iVideoRate)
    {
        return (int) 0;
    }
#ifdef __SUPPORT_SD_CARD__
    pCDPManager->DR_SetVideoUsedSpaceRate(nVideoSpace);
#endif
    m_iVideoRate = nVideoSpace;

	pCConfig->SetValue(RecordCfgSection, "VideoRate", m_iVideoRate);

    return 0;
}

/*@breif GetFileReadWriteStatus 获取当前文件的读写状态
 *@param [IN]  file_name 需要操作的文件名(绝对路径)
 *@param [OUT] read_write_status 
 *@            0:  当前文件空闲
 *@            1:  当前文件正在读
 *@            2:  当前文件正在写
 *@param [OUT] file_size 文件大小(单位: 字节) 
 *@return 返回0表示成功
 *@      返回-1表示失败
 */
int CRecord::GetFileReadWriteStatus(const char *file_name, 
                                           unsigned int *read_write_status,
                                           unsigned int *file_size)
{
	#ifdef __SUPPORT_SD_CARD__
    return pCDPManager->DR_GetFileReadWriteStatus(file_name, read_write_status, file_size);
	#else
	return 0;
	#endif
}

/* 设置获取时间偏移回调 */
int CRecord::SetGetTimeOffsetCallBack(int (*CallBack)(int *, int *, int *))
{
#ifdef __SUPPORT_SD_CARD__
    return pCDPManager->DR_SetGetTimeOffsetCallBack(CallBack);
#else
	return 0;
#endif	
}

/* 传入的图片名字组成pic_name: 时间戳_标号，如:1532477851_01需要改为1532477851 */
int CRecord::UploadPictureToSdCard(const char *pic_name, unsigned int trig_type)
{
#ifdef __SUPPORT_SD_CARD__	
    return pCDPManager->DR_UploadPictureToSdCard(pic_name, trig_type);
#else
	return 0;
#endif	
}

int CRecord::StartAviRecord(int record_type)
{
    int wight = 0;
    int height = 0;
    int bH265 = 0;
    CODEC_PARA_S CodecPara;

    ZWSys_EncGetCodecPara(m_hEnc, m_iRecStreamIndex, &CodecPara);
	
	/* 从SDK里面获取当前录像码流的分辨率 */
	wight = CodecPara.PictureRes.m_PicWidth;
	height = CodecPara.PictureRes.m_PicHeight;

    /* 编码类型(默认如果不是H265则为H264) */
    bH265 = (LtH265Video==CodecPara.EncType || LtH265PlusVideo==CodecPara.EncType) ? 1 : 0;
#ifdef __SUPPORT_SD_CARD__
    /* 设置录像的尺寸和编码类型 */
    pCDPManager->DR_SetRecordSize(wight, height, bH265);

    /* 开启录像 */
    pCDPManager->DR_StartRecordData(0, record_type, bH265);
#endif
    return (int) 0;
}

enumRecordMode CRecord::GetRecMode()
{
	return (enumRecordMode) 0;
}

//!\brief   设置录像码流索引
//!\param   int nIndex  0表示主码流，1表示子码流，2表示高清子码流
//!\return 成功为0，其它为错误码
int CRecord::SetRecStreamIndex(int nIndex)
{
	char key[100] = {0};

#ifdef USE_HDAUX
    if ((nIndex < 0) || (nIndex > 2))
    {
        AVL_ERR("[CRecord:%s]stream index err, nIndex %d\n", __FUNCTION__, nIndex);
        return -1;
    }
#else
    if ((nIndex < 0) || (nIndex > 1))
    {
        AVL_ERR("[CRecord:%s]stream index err, nIndex %d\n", __FUNCTION__, nIndex);
        return -1;
    }
#endif

	if (m_iRecStreamIndex == nIndex)
	{
		return 0;
	}

	snprintf(key, sizeof(key), "RecStream_%02d", m_channel);
	pCConfig->SetValue(RecordCfgSection, key, nIndex);

	m_iRecStreamIndex = nIndex;

	//强迫切换录像类型
	ForceSwicth();

	return 0;
}

//!\brief       获取录像码率索引
//!\return   录像码流索引(0表示主码流，1表示子码流，2表示高清子码流)
int CRecord::GetRecStreamIndex()
{
	return m_iRecStreamIndex;
}

/* 触发录像完毕之后，发送消息的回调函数 */
int CRecord::SetTrigRecordOverCallBack(int nCh, int (*CallBack)(unsigned char *))
{
#ifdef __SUPPORT_SD_CARD__	
    pCDPManager->DR_SetTrigRecordOverCallBack(nCh, CallBack);
#endif	
    return 0;
}


// 当前是否有移动侦测联动录像的动作(即当前有移动侦测且联动录像使能)
int CRecord::Motion_SetAlarmRec()
{
	//已经开启了，就不用再开启
	if (m_bMotionAlarmRec)
	{
		return 0;
	}

	m_bMotionAlarmRec = 1;
	return (int) 0;
}

int CRecord::Ai_SetAlarmRec()
{
	//已经开启了，就不用再开启
	if (m_bAiAlarmRec)
	{
		return 0;
	}

	m_bAiAlarmRec = 1;
	return (int) 0;
}

// 当前是否有报警输入联动录像的动作(即当前有报警输入且联动录像使能)
int CRecord::AlarmIn_SetAlarmRec()
{
	//   TRACE_WARN("[%s] \n",__FUNCTION__);
	//已经开启了，就不用再开启
	if (m_bAlarmInAlarmRec)
	{
		return 0;
	}
	m_bAlarmInAlarmRec = 1;
	return (int) 0;

}

/* 获取当前分辨率 */
void *CRecord::GetCurrentResolutionThreadProcess()
{
    CODEC_PARA_S CodecPara;

    while(1)
    {
        memset(&CodecPara, 0, sizeof(CodecPara));
        ZWSys_EncGetCodecPara(m_hEnc, m_iRecStreamIndex, &CodecPara);

        m_nH264Width = CodecPara.PictureRes.m_PicWidth;
        m_nH264Height = CodecPara.PictureRes.m_PicHeight;
        sleep(1);
    }
    return NULL;
}

void* GetCurrentResolutionThread(void * lParam)
{	
    PRT_TID();
	pthread_detach(pthread_self());
	
	//获取当前实例
	CRecord *pCRecord = (CRecord *)lParam;

	if (NULL != pCRecord)
	{
       pCRecord->GetCurrentResolutionThreadProcess();
	}
	else
	{
		AVL_ERR("get pCRecord instance fail\n");
	}

	return NULL;
}


//##ModelId=49A25FB00286
int CRecord::GetState()
{
	return (int) m_iRecState;
}

int CRecord::LoadParam()
{
	char value[VALUE_MAX_LEN] = {0};
	int tmp = 0;
	int nRet = 0;
	char key_str[36] = {0};
	tagEncCfg *Info = NULL;
	tagRecordSchedule *cfg = NULL;
	int i, j;
	
	int BoardInfo;
	char boardIDSection[24] = {0};
	int firstStreamDefault=0;
	//根据版型来修改默认配置参数
	ZWSys_GetBoardInfo(&BoardInfo);
	memset(boardIDSection, 0, sizeof(boardIDSection));
	snprintf(boardIDSection, sizeof(boardIDSection), "BoardID-%x", BoardInfo);
	firstStreamDefault = pCConfig->GetValue(boardIDSection, "FirstStreamDefault", 0);
	if(1 == firstStreamDefault)
	{
		pCConfig->SetValue(boardIDSection, "FirstStreamDefault", 0);
		TRACE_AVL("\n[software] [%s-%d]===main stream enc goto default==\n",__FUNCTION__,__LINE__);
		snprintf(key_str, sizeof(key_str), "DeGeneralEncCfg%02d", m_channel);
		pCConfig->GetValue(boardIDSection, key_str, DefEncCfgStr, value, VALUE_MAX_LEN);
		snprintf(key_str, sizeof(key_str), "GeneralEncCfg%02d", m_channel);
		pCConfig->SetValue(RecordCfgSection, key_str, value);

		snprintf(key_str, sizeof(key_str), "DeManualEncCfg%02d", m_channel);
		pCConfig->GetValue(boardIDSection, key_str, DefEncCfgStr, value, VALUE_MAX_LEN);
		snprintf(key_str, sizeof(key_str), "ManualEncCfg%02d", m_channel);
		pCConfig->SetValue(RecordCfgSection, key_str, value);

		snprintf(key_str, sizeof(key_str), "DeEventEncCfg%02d", m_channel);
		pCConfig->GetValue(boardIDSection, key_str, DefEncCfgStr, value, VALUE_MAX_LEN);
		snprintf(key_str, sizeof(key_str), "EventEncCfg%02d", m_channel);
		pCConfig->SetValue(RecordCfgSection, key_str, value);

		snprintf(key_str, sizeof(key_str), "DeMainCoderKeyFrameInterval%02d", m_channel);
		pCConfig->GetValue(boardIDSection, key_str, DefEncCfgStr, value, VALUE_MAX_LEN);
		snprintf(key_str, sizeof(key_str), "MainCoderKeyFrameInterval%02d", m_channel);
		pCConfig->SetValue(RecordCfgSection, key_str, value);

		snprintf(key_str, sizeof(key_str), "DeMainCoderFormatWidth%02d", m_channel);
		tmp = pCConfig->GetValue(boardIDSection, key_str, 0);
		if (0 != tmp)
		{
			snprintf(key_str, sizeof(key_str), "MainCoderFormatWidth%02d", m_channel);
			pCConfig->SetValue(RecordCfgSection, key_str, tmp);
		}

		snprintf(key_str, sizeof(key_str), "DeMainCoderFormatHeight%02d", m_channel);
		tmp = pCConfig->GetValue(boardIDSection, key_str, 0);
		if (0 != tmp)
		{
			snprintf(key_str, sizeof(key_str), "MainCoderFormatHeight%02d", m_channel);
			pCConfig->SetValue(RecordCfgSection, key_str, tmp);
		}

		snprintf(key_str, sizeof(key_str), "DeMainCoderKeyFrameInterval%02d", m_channel);
		tmp = pCConfig->GetValue(boardIDSection, key_str, 0);
		if (0 != tmp)
		{
			snprintf(key_str, sizeof(key_str), "MainCoderKeyFrameInterval%02d", m_channel);
			pCConfig->SetValue(RecordCfgSection, key_str, tmp);
		}
	}

	// 导入高级编码设置
	snprintf(key_str, sizeof(key_str), "nVideoAdvanceCodec%02d", m_channel);
	pCConfig->GetValue(RecordCfgSection, key_str, "0:0", value, VALUE_MAX_LEN);
	sscanf(value, "%d:%d", &m_AdvanceEncCfg.nVideoCodecModeNormal[m_channel], &m_AdvanceEncCfg.nVideoCodecModeEvent[m_channel]);


	// 导入视频遮挡参数
	snprintf(key_str, sizeof(key_str), "nVideoShelter%02d", m_channel);
	pCConfig->GetValue(RecordCfgSection, key_str, "0:0:0:0", value, VALUE_MAX_LEN);
	sscanf(value, "%hu:%hu:%hu:%hu", &m_AdvanceEncCfg.struShelter[m_channel].wHideAreaTopLeftX, 
								 &m_AdvanceEncCfg.struShelter[m_channel].wHideAreaTopLeftY,
								 &m_AdvanceEncCfg.struShelter[m_channel].wHideAreaWidth,
								 &m_AdvanceEncCfg.struShelter[m_channel].wHideAreaHeight);

	//导入模式
	m_iCustomMode = pCConfig->GetValue(RecordCfgSection, "RecCustomMode", 0);
	//制式
	m_iStandard = pCConfig->GetValue(RecordCfgSection, "VideoStandard", StandardPAL);

	//开始load配置[RecordCfg]
	//定制模式--RecCustomMode
	//普通配置--GeneralEncCfg[ch]--分辨率：帧率：码流类型：码率类型：质量：量化系数：码率上限：码率下限
	//手动配置--ManualEncCfg[ch]
	//事件配置--EventEncCfg[ch]

	//导入一般模式
	Info = &RecordCfg[0];
	snprintf(key_str, sizeof(key_str), "GeneralEncCfg%02d", m_channel);
	pCConfig->GetValue(RecordCfgSection, key_str, DefEncCfgStr, value, VALUE_MAX_LEN);
	sscanf(value, "%d:%d:%d:%d:%d:%d:%d:%d",
		   (int *)&Info->byResolution, &Info->dwVideoFrameRate, &Info->byStreamType, &Info->byBitrateType,
		   &Info->byPicQuality, &Info->dwQuotiety, &Info->dwVideoBitrateH, &Info->dwVideoBitrateL);

    snprintf(key_str, sizeof(key_str), "MainCoderFormatWidth%02d", m_channel);
    Info->dwResWidth = pCConfig->GetValue(RecordCfgSection, key_str, 1920);
    
    snprintf(key_str, sizeof(key_str), "MainCoderFormatHeight%02d", m_channel);
    Info->dwResHeight = pCConfig->GetValue(RecordCfgSection, key_str, 1080);

    nRet = IsSupportV2(Info->dwResWidth, Info->dwResHeight);
    if (nRet != 0)
    {
		printf("EncFormat not support\n");
		Info->dwResWidth = 1920;
		Info->dwResHeight = 1080;
	}

	if (Info->dwVideoFrameRate != 0xFF && (Info->dwVideoFrameRate < 1 || Info->dwVideoFrameRate > 30))
	{
		Info->dwVideoFrameRate = 0xFF;
	}
	if (Info->dwQuotiety < 0 || Info->dwQuotiety > 5)
	{
		Info->dwQuotiety = 5;
	}
	if (Info->byStreamType > 1 || Info->byStreamType < 0)
	{
		Info->byStreamType = 1;
	}
	if (Info->byPicQuality > 3 || Info->byPicQuality < 0)
	{
		Info->byPicQuality = 1;
	}
	if (Info->byBitrateType > 3 || Info->byBitrateType < 0)
	{
		Info->byBitrateType = 1;
	}
	
	SetEncCfg(Info, 0);

	//导入手动
	Info = &RecordCfg[1];
	snprintf(key_str, sizeof(key_str), "ManualEncCfg%02d", m_channel);
	pCConfig->GetValue(RecordCfgSection, key_str, DefEncCfgStr, value, VALUE_MAX_LEN);
	sscanf(value, "%d:%d:%d:%d:%d:%d:%d:%d",
		   (int *)&Info->byResolution, &Info->dwVideoFrameRate, &Info->byStreamType, &Info->byBitrateType,
		   &Info->byPicQuality, &Info->dwQuotiety, &Info->dwVideoBitrateH, &Info->dwVideoBitrateL);

	if (Info->dwVideoFrameRate != 0xFF && (Info->dwVideoFrameRate < 1 || Info->dwVideoFrameRate > 30))
	{
		Info->dwVideoFrameRate = 0xFF;
	}
	if (Info->dwQuotiety < 0 || Info->dwQuotiety > 5)
	{
		Info->dwQuotiety = 5;
	}
	if (Info->byStreamType > 1 || Info->byStreamType < 0)
	{
		Info->byStreamType = 1;
	}
	if (Info->byPicQuality > 3 || Info->byPicQuality < 0)
	{
		Info->byPicQuality = 1;
	}
	if (Info->byBitrateType > 1 || Info->byBitrateType < 0)
	{
		Info->byBitrateType = 1;
	}
	SetEncCfg(Info, 1);

	//导入事件
	Info = &RecordCfg[2];
	snprintf(key_str, sizeof(key_str), "EventEncCfg%02d", m_channel);
	pCConfig->GetValue(RecordCfgSection, key_str, DefEncCfgStr, value, VALUE_MAX_LEN);
	sscanf(value, "%d:%d:%d:%d:%d:%d:%d:%d",
		   (int *)&Info->byResolution, &Info->dwVideoFrameRate, &Info->byStreamType, &Info->byBitrateType,
		   &Info->byPicQuality, &Info->dwQuotiety, &Info->dwVideoBitrateH, &Info->dwVideoBitrateL);

	if (Info->dwVideoFrameRate != 0xFF && (Info->dwVideoFrameRate < 1 || Info->dwVideoFrameRate > 30))
	{
		Info->dwVideoFrameRate = 0xFF;

	}
	if (Info->dwQuotiety < 0 || Info->dwQuotiety > 5)
	{
		Info->dwQuotiety = 5;
	}
	if (Info->byStreamType > 1 || Info->byStreamType < 0)
	{
		Info->byStreamType = 1;
	}
	if (Info->byPicQuality > 3 || Info->byPicQuality < 0)
	{
		Info->byPicQuality = 1;
	}
	if (Info->byBitrateType > 1 || Info->byBitrateType < 0)
	{
		Info->byBitrateType = 1;
	}

	SetEncCfg(Info, 2);
	//导入计划
	cfg = &m_ScheduleCfg;
	//录像计划--
	//ScheduleCfgEnable[ch]=[启用]
	//ScheduleCfg[ch]_[星期]=[48]--[启用][星期][半小时]
	snprintf(key_str, sizeof(key_str), "ScheduleCfgEnable%02d", m_channel);
	cfg->bytState = pCConfig->GetValue(RecordCfgSection, key_str, 0);
	cfg->bytReserve[0] = 0;
	cfg->bytReserve[1] = 0;
	cfg->bytReserve[2] = 0;
	for (i = 0; i < 7; i++)
	{
		snprintf(key_str, sizeof(key_str), "ScheduleCfg%02d_%01d", m_channel, i);
		pCConfig->GetValue(RecordCfgSection, key_str, SCHEDULE_TIME_ONE, value, VALUE_MAX_LEN);
		for (j = 0; j < 48; j++)
		{
			cfg->bytRecordTime[i][j] = value[j] - '0';
		}

	}
	//预录时间--PrevRecTime[ch]
	//延迟录时间--DelayRecTime[ch]
	snprintf(key_str, sizeof(key_str), "PrevRecTime%02d", m_channel);
	m_iPreTime = pCConfig->GetValue(RecordCfgSection, key_str, 10);
#ifdef __SUPPORT_SD_CARD__	
    pCDPManager->DR_SetPrevTimeSec(m_channel, m_iPreTime);
#endif
	//延迟录时间--DelayRecTime[ch]
	snprintf(key_str, sizeof(key_str), "DelayRecTime%02d", m_channel);
	m_iDelayTime = pCConfig->GetValue(RecordCfgSection, key_str, 10);

	//录像码流选择
	snprintf(key_str, sizeof(key_str), "RecStream_%02d", m_channel);
	m_iRecStreamIndex = pCConfig->GetValue(RecordCfgSection, key_str, 0);

#ifdef USE_HDAUX
    m_iRecStreamIndex = m_iRecStreamIndex > 2 ? 2 : m_iRecStreamIndex;
#else
    m_iRecStreamIndex = m_iRecStreamIndex > 1 ? 1 : m_iRecStreamIndex;
#endif

#ifdef __SUPPORT_SD_CARD__
	//每页显示的AVI个数
	snprintf(key_str, sizeof(key_str), "ShowAviNumPerPage");
	m_iShowAviNumPerPage = pCConfig->GetValue(RecordCfgSection, key_str, 50);
    pCDPManager->DR_SetCurrentAviNumPerPage(m_iShowAviNumPerPage);

	//每页显示的图片个数
	snprintf(key_str, sizeof(key_str), "ShowPicNumPerPage");
	m_iShowPicNumPerPage = pCConfig->GetValue(RecordCfgSection, key_str, 50);
    pCDPManager->DR_SetCurrentPicNumPerPage(m_iShowPicNumPerPage);

	//视频占用SD卡的百分比
	snprintf(key_str, sizeof(key_str), "VideoRate");
	m_iVideoRate = pCConfig->GetValue(RecordCfgSection, key_str, 90);
    pCDPManager->DR_SetVideoUsedSpaceRate(m_iVideoRate);
#endif
	//录像状态
    memset(key_str, 0,sizeof(key_str));
	snprintf(key_str, sizeof(key_str), "ScheduleCfgEnableV2%02d", m_channel);
	m_ScheduleCfgV2.nEnable = pCConfig->GetValue(RecordCfgSection, key_str, 1);
    //录像类型
    memset(key_str, 0,sizeof(key_str));
	snprintf(key_str, sizeof(key_str), "ScheduleRecordType%02d", m_channel);
	m_ScheduleCfgV2.nRecordType = pCConfig->GetValue(RecordCfgSection, key_str, 0);
	//录像开始时间小时
    memset(key_str, 0, sizeof(key_str));
	snprintf(key_str, sizeof(key_str), "ScheduleRecordStartTimeHour%02d", m_channel);
	m_ScheduleCfgV2.startTime.nHour = pCConfig->GetValue(RecordCfgSection, key_str, 0);	
	//录像开始时间分钟
	memset(key_str, 0, sizeof(key_str));   
	snprintf(key_str, sizeof(key_str), "ScheduleRecordStartTimeMinute%02d", m_channel);
	m_ScheduleCfgV2.startTime.nMinute = pCConfig->GetValue(RecordCfgSection, key_str, 0);	
	//录像开始时间秒数
	memset(key_str, 0, sizeof(key_str));   
	snprintf(key_str, sizeof(key_str), "ScheduleRecordStartTimeSecond%02d", m_channel);
	m_ScheduleCfgV2.startTime.nSecond = pCConfig->GetValue(RecordCfgSection, key_str, 0);	
	//录像结束时间小时	
    memset(key_str, 0, sizeof(key_str));
	snprintf(key_str, sizeof(key_str), "ScheduleRecordStopTimeHour%02d", m_channel);
	m_ScheduleCfgV2.stopTime.nHour = pCConfig->GetValue(RecordCfgSection, key_str, 24);	
    //录像结束时间分钟
	memset(key_str, 0, sizeof(key_str));
	snprintf(key_str, sizeof(key_str), "ScheduleRecordStopTimeMinute%02d", m_channel);
	m_ScheduleCfgV2.stopTime.nMinute = pCConfig->GetValue(RecordCfgSection, key_str, 0);
    //录像结束时间秒数
	memset(key_str, 0, sizeof(key_str));
	snprintf(key_str, sizeof(key_str), "ScheduleRecordStopTimeSecond%02d", m_channel);
	m_ScheduleCfgV2.stopTime.nSecond = pCConfig->GetValue(RecordCfgSection, key_str, 0);

	SetUsrRecordType(m_ScheduleCfgV2.nRecordType,m_ScheduleCfgV2.nEnable);

	return 0;
}

//判断是否支持该编码格式，现在分辨率分不用宏，而是使用实际分辨率
int CRecord::IsSupportV2(int width, int height)
{
    int nRet = 0;
    char acResolution[32] = {0};

    if (width <= 0 || height <= 0)
    {
        fprintf(stderr, "[%s][%d] ********invalid param.width=%d height=%d*****\r\n",__FUNCTION__,__LINE__,width,height);
        return -1;
    }
    snprintf(acResolution, sizeof(acResolution), "%dX%d", width, height);
	nRet = IsSupportDeviceAbility(0, 1, 0, acResolution);
    if (100 == nRet)
	    return 0;
	else
	    return -1;
}

//判断是否支持该编码格式
int CRecord::IsSupport(int EncFormat)
{
	static int nBoardID = 0;
	static DeviceDescription ddt = {0};

	if (nBoardID == 0)
	{
		ZWSys_GetBoardInfo(&nBoardID);
		EDVRS_DDT_GetDeviceDescriptionByID(&ddt, nBoardID);
	}

	int pnMode = pCConfig->GetValue("RecordCfg", "ResourceMode", 0);

	if (pnMode != 0)
	{
		//说明配置要求是实时的
		if (GetMaxEncCap() == 1)	//D1
		{
			return EncFormat;
		}
		else if (GetMaxEncCap() == 2 && (EncFormat == 0 || EncFormat == 2 || EncFormat == 5))	//2 2CIF
		{
			return EncFormat;
		}
		return GetMaxEncCap();
	}
	while (EncFormat > 0)
	{
		if (((ddt.bytMainVideoSreamResolutionCap[m_channel] & CR_4CIF) || (ddt.bytMainVideoSreamResolutionCap[m_channel] & CR_4CIF_SIM))
			&& (EncFormat == eResolutionIndex_D1))
		{
			return EncFormat;
		}
		else if ((ddt.bytMainVideoSreamResolutionCap[m_channel] & CR_HD720) && (EncFormat == eResolutionIndex_720P))//(EncFormat == 1)
		{
			return EncFormat;
		}
		else if ((ddt.bytMainVideoSreamResolutionCap[m_channel] & CR_HD1080) && (EncFormat == eResolutionIndex_1080P))//(EncFormat == 2)
		{
			return EncFormat;
		}
		else if ((ddt.bytMainVideoSreamResolutionCap[m_channel] & CR_SXGA) && (EncFormat == eResolutionIndex_SXGA))
		{
			return EncFormat;
		}
		else if ((ddt.bytMainVideoSreamResolutionCap[m_channel] & CR_QXGA) && (EncFormat == eResolutionIndex_QXGA))
		{
			return EncFormat;
		}
		else if ((ddt.bytMainVideoSreamResolutionCap[m_channel] & CR_QuadVGA) && (EncFormat == eResolutionIndex_QuadVGA))
		{
			return EncFormat;
		}
        else if ((ddt.bytMainVideoSreamResolutionCap[m_channel] & CR_UWXGA) && (EncFormat == eResolutionIndex_UWXGA))
        {
			return EncFormat;
		}
        else if ((ddt.bytMainVideoSreamResolutionCap[m_channel] & CR_USXGA) && (EncFormat == eResolutionIndex_USXGA))
        {
			return EncFormat;
		}
        else if ((ddt.bytMainVideoSreamResolutionCap[m_channel] & CR_VGA) && (EncFormat == eResolutionIndex_VGA))
        {
			return EncFormat;
		}
		else
		{
			//不支持则返回支持的最大分辨率
			if ((ddt.bytMainVideoSreamResolutionCap[m_channel] & CR_USXGA))
			{
				EncFormat = eResolutionIndex_USXGA;
			}
			else if ((ddt.bytMainVideoSreamResolutionCap[m_channel] & CR_UWXGA))
			{
				EncFormat = eResolutionIndex_UWXGA;
			}
			else if ((ddt.bytMainVideoSreamResolutionCap[m_channel] & CR_QXGA))
			{
				EncFormat = eResolutionIndex_QXGA;
			}
			else if ((ddt.bytMainVideoSreamResolutionCap[m_channel] & CR_HD1080))
			{
				EncFormat = eResolutionIndex_1080P;
			}
			else if ((ddt.bytMainVideoSreamResolutionCap[m_channel] & CR_SXGA))
			{
				EncFormat = eResolutionIndex_SXGA;
			}
			else if ((ddt.bytMainVideoSreamResolutionCap[m_channel] & CR_QuadVGA))
			{
				EncFormat = eResolutionIndex_QuadVGA;
			}
			else if ((ddt.bytMainVideoSreamResolutionCap[m_channel] & CR_HD720))
			{
				EncFormat = eResolutionIndex_720P;
			}
			else if ((ddt.bytMainVideoSreamResolutionCap[m_channel] & CR_4CIF) || (ddt.bytMainVideoSreamResolutionCap[m_channel] & CR_4CIF_SIM))
			{
				EncFormat = eResolutionIndex_D1;
			}

			return EncFormat;
		}
	}

	return 0;
}

//支持最大的实时编码格式
int CRecord::GetMaxEncCap()
{
	static int nBoardID = 0;

	static DeviceDescription ddt;

	if (nBoardID == 0)
	{
		ZWSys_GetBoardInfo(&nBoardID);
		EDVRS_DDT_GetDeviceDescriptionByID(&ddt, nBoardID);
	}

	int pnMode = pCConfig->GetValue("RecordCfg", "ResourceMode", 0);

	int pnChMask = pCConfig->GetValue("RecordCfg", "ResourceChMask", 0);

	if (pnMode == 0)
	{
/*	
		if ((ddt.bytMainVideoSreamResolutionCap[m_channel] & CR_500MP))
		{
			return 6;			//3  500MP
		}
		else
*/
		if ((ddt.bytMainVideoSreamResolutionCap[m_channel] & CR_HD576))
		{
			return 7;			//3  576P
		}
		else if ((ddt.bytMainVideoSreamResolutionCap[m_channel] & CR_HD1080))
		{
			return 6;			//3  1080P
		}
		else if ((ddt.bytMainVideoSreamResolutionCap[m_channel] & CR_UXGA))
		{
			return 5;			//3  UXGA
		}
		else if ((ddt.bytMainVideoSreamResolutionCap[m_channel] & CR_SXGA))
		{
			return 4;			//3  SXGA
		}
		else if ((ddt.bytMainVideoSreamResolutionCap[m_channel] & CR_HD720))
		{
			return 3;			//3  720P
		}
		else if ((ddt.bytMainVideoSreamResolutionCap[m_channel] & CR_4CIF))
		{
			return 1;			//3  4CIF
		}
		else if ((ddt.bytMainVideoSreamResolutionCap[m_channel] & CR_2CIF))
		{
			return 2;			//3  2CIF
		}
		else if ((ddt.bytMainVideoSreamResolutionCap[m_channel] & CR_VGA))
		{
			return 8;			//3  VGA
		}
		else if ((ddt.bytMainVideoSreamResolutionCap[m_channel] & CR_UWXGA))
		{
			return 12;			//3  UWXGA	
		}
		else if ((ddt.bytMainVideoSreamResolutionCap[m_channel] & CR_USXGA))
		{
			return 13;			//3  USXGA
		}
		else
		{
			return 0;			//3  CIF
		}
	}

	if (ZWSys_GetTotalEncChannels() == 16)//MP4Sys_GetTotalEncChannels
	{
		if (pnMode == 1)
		{
			if ((pnChMask >> m_channel) & 0x1)
			{
				return 2;		//2 2CIF
			}
		}
		else if (pnMode == 2)
		{
			if ((pnChMask >> m_channel) & 0x1)
			{
				return 1;		//3  4CIF
			}
		}
	}
	else if (ZWSys_GetTotalEncChannels() == 8)//MP4Sys_GetTotalEncChannels
	{
		if (pnMode != 0)
		{
			if ((pnChMask >> m_channel) & 0x1)
			{
				return 1;		//3  4CIF
			}
		}
	}

	AVL_ERR("pnMode: = %d pnChMask : %d is not support EncFormat return CIF \n", pnMode, pnChMask);
	return 0;
}


//##ModelId=49B714EC03AF
int CRecord::GetEncCfg(tagEncCfg * Info, int nEnctype)
{
	if (NULL == Info)
	{
		AVL_ERR("[CRecord:%s]Info==NULL\n", __FUNCTION__);
		return -1;
	}
	if (nEnctype < 0 || nEnctype > 2)
	{
		AVL_ERR("[CRecord:%s]nEnctype=%d (nEnctype<0||nEnctype>2)\n", __FUNCTION__, nEnctype);
		return -1;
	}

	memcpy(Info, &RecordCfg[nEnctype], sizeof (tagEncCfg));

    if (0xff == Info->dwVideoFrameRate)
    {
        if (m_SensorFrequency == CMOS_FREQUENCY_50HZ)
        {
            Info->dwVideoFrameRate = 25;
        }else{
            Info->dwVideoFrameRate = 30;
        }
    } 

	TRACE_AVL("\n[software] [%s-%d]===GetEncCfg====%d:%d:%d:%d:%d:%d:%d:%d======nEnctype=%d==\n",__FUNCTION__,__LINE__,
				Info->byResolution, Info->dwVideoFrameRate, Info->byStreamType, Info->byBitrateType,
				Info->byPicQuality, Info->dwQuotiety, Info->dwVideoBitrateH, Info->dwVideoBitrateL,nEnctype);
	return (int) 0;
}

//##ModelId=49A263B601BF
int CRecord::SetEncCfg(tagEncCfg * Info, int nEnctype)
{
	TRACE_AVL("\n[software] [%s-%d]===0_1====%d:%d:%d:%d:%d:%d:%d:%d====nEnctype=%d====\n",__FUNCTION__,__LINE__,
		Info->byResolution, Info->dwVideoFrameRate, Info->byStreamType, Info->byBitrateType,
		Info->byPicQuality, Info->dwQuotiety, Info->dwVideoBitrateH, Info->dwVideoBitrateL,nEnctype);

	char value[VALUE_MAX_LEN] = {0};
	char key_str[36] = {0};
	tagEncCfg tmp_info;
	int nRet = 0;
	char acCmd[64] = {0};
	
	if (NULL == Info)
	{
		AVL_ERR("[CRecord:%s]Info==NULL\n", __FUNCTION__);
		return -1;
	}
	if (Info->dwVideoFrameRate != 0xFF && (Info->dwVideoFrameRate < 1 || Info->dwVideoFrameRate > 30))
	{
		AVL_ERR("[CRecord:%s]Info->dwVideoFrameRate=%d\n", __FUNCTION__, Info->dwVideoFrameRate);
		return -1;
	}
 	if (1 == bSupportRtmp)
	{
		if(Info->dwVideoFrameRate < 15)
			Info->dwVideoFrameRate = 15;
	} 	
	//printf("===========bSupportRtmp = %d====cp.FrameRate%d===\n",bSupportRtmp,Info->dwVideoFrameRate);
	
	if (Info->dwQuotiety < 0 || Info->dwQuotiety > 5)
	{
		AVL_ERR("[CRecord:%s]Info->dwQuotiety=%d\n", __FUNCTION__, Info->dwQuotiety);
		return -1;
	}
	if (Info->byStreamType > 1 || Info->byStreamType < 0)
	{
		AVL_ERR("[CRecord:%s]Info->byStreamType=%d\n", __FUNCTION__, Info->byStreamType);
		return -1;
	}
	Info->dwVideoBitrateH = (Info->dwVideoBitrateH < 256) ? 256 : Info->dwVideoBitrateH;

    nRet = IsSupportV2(Info->dwResWidth, Info->dwResHeight);
    if (nRet != 0)
    {
		printf("EncFormat [w:%d  h:%d]not support\n", Info->dwResWidth, Info->dwResHeight);
		return -1;
	}

	DeviceDescription ddt;
	int nBoardID = 0;
	int nSmartAIWorkMode = 0;

	if (nBoardID == 0)
	{
		ZWSys_GetBoardInfo(&nBoardID);
		EDVRS_DDT_GetDeviceDescriptionByID(&ddt, nBoardID);
	}

	if (ddt.bytAudioInChannels <= m_channel)
	{
		Info->byStreamType = 0;
	}
	
	if (Info->byPicQuality > 3 || Info->byPicQuality < 0)
	{
		AVL_ERR("[CRecord:%s]Info->byPicQuality=%d\n", __FUNCTION__, Info->byPicQuality);
		return -1;
	}
	if (Info->byBitrateType > 1 || Info->byBitrateType < 0)
	{
		AVL_ERR("[CRecord:%s]Info->byBitrateType=%d\n", __FUNCTION__, Info->byBitrateType);
		return -1;
	}
#if 0
//由于没有码率下线 所以所有的码率类型都是固定码率
	if (Info->byBitrateType >= 0 && Info->byBitrateType < 2)
		Info->byBitrateType = 1;
#endif	
	memcpy(&tmp_info, Info, sizeof (tagEncCfg));

	//add by linyun
	//处理录像质量参数
	if (tmp_info.byPicQuality == 0)
	{
		snprintf(key_str, sizeof(key_str), "%s%s", HighPicQuality, m_szType[GetMaxEncCap()]);
		#if 0
		pCConfig->GetValue(RecordCfgSection, key_str, "", value, sizeof (value) - 1);

		sscanf(value, "%d:%d:%d:%d:%d:%d",
			   &tmp_info.byResolution, &tmp_info.dwVideoFrameRate, &tmp_info.byBitrateType,
			   &tmp_info.dwQuotiety, &tmp_info.dwVideoBitrateH, &tmp_info.dwVideoBitrateL);

		#else
		snprintf(value, sizeof(value), "%d:%d:%d:%d:%d:%d:%d:%d",
			tmp_info.byResolution, tmp_info.dwVideoFrameRate, tmp_info.byStreamType, tmp_info.byBitrateType,
			tmp_info.byPicQuality, tmp_info.dwQuotiety, tmp_info.dwVideoBitrateH, tmp_info.dwVideoBitrateL);
		pCConfig->SetValue(RecordCfgSection, key_str, value);
		#endif
	}
	else if (tmp_info.byPicQuality == 1)
	{
		snprintf(key_str, sizeof(key_str), "%s%s", NorPicQuality, m_szType[GetMaxEncCap()]);
		#if 0
		pCConfig->GetValue(RecordCfgSection, key_str, "", value, sizeof (value) - 1);

		sscanf(value, "%d:%d:%d:%d:%d:%d",
			   &tmp_info.byResolution, &tmp_info.dwVideoFrameRate, &tmp_info.byBitrateType,
			   &tmp_info.dwQuotiety, &tmp_info.dwVideoBitrateH, &tmp_info.dwVideoBitrateL);	

		#else
		snprintf(value, sizeof(value), "%d:%d:%d:%d:%d:%d:%d:%d",
			tmp_info.byResolution, tmp_info.dwVideoFrameRate, tmp_info.byStreamType, tmp_info.byBitrateType,
			tmp_info.byPicQuality, tmp_info.dwQuotiety, tmp_info.dwVideoBitrateH, tmp_info.dwVideoBitrateL);
		pCConfig->SetValue(RecordCfgSection, key_str, value);
		#endif

	}
	else if (tmp_info.byPicQuality == 2)
	{
		snprintf(key_str, sizeof(key_str), "%s%s", ComPicQuality, m_szType[GetMaxEncCap()]);
		#if 0
		pCConfig->GetValue(RecordCfgSection, key_str, "", value, sizeof (value) - 1);

		sscanf(value, "%d:%d:%d:%d:%d:%d",
			   &tmp_info.byResolution, &tmp_info.dwVideoFrameRate, &tmp_info.byBitrateType,
			   &tmp_info.dwQuotiety, &tmp_info.dwVideoBitrateH, &tmp_info.dwVideoBitrateL);
		

		#else
		snprintf(value, sizeof(value), "%d:%d:%d:%d:%d:%d:%d:%d",
			tmp_info.byResolution, tmp_info.dwVideoFrameRate, tmp_info.byStreamType, tmp_info.byBitrateType,
			tmp_info.byPicQuality, tmp_info.dwQuotiety, tmp_info.dwVideoBitrateH, tmp_info.dwVideoBitrateL);
		pCConfig->SetValue(RecordCfgSection, key_str, value);
		#endif
	}
	if (tmp_info.byPicQuality == 3)
	{
	#if 0
		snprintf(key_str, sizeof(key_str), "%s%s", CustomPicQuality, m_szType[GetMaxEncCap()]);
		pCConfig->GetValue(RecordCfgSection, key_str, "", value, sizeof (value) - 1);

		sscanf(value, "%d:%d:%d:%d:%d:%d",
			   &tmp_info.byResolution, &tmp_info.dwVideoFrameRate, &tmp_info.byBitrateType,
			   &tmp_info.dwQuotiety, &tmp_info.dwVideoBitrateH, &tmp_info.dwVideoBitrateL);
		
	#else	
		snprintf(key_str, sizeof(key_str), "%s%s", CustomPicQuality, m_szType[GetMaxEncCap()]);
		snprintf(value, sizeof(value), "%d:%d:%d:%d:%d:%d:%d:%d",
				tmp_info.byResolution, tmp_info.dwVideoFrameRate, tmp_info.byStreamType, tmp_info.byBitrateType,
				tmp_info.byPicQuality, tmp_info.dwQuotiety, tmp_info.dwVideoBitrateH, tmp_info.dwVideoBitrateL);
		pCConfig->SetValue(RecordCfgSection, key_str, value);
	#endif
	}

	//开始load配置[RecordCfg]
	//定制模式--RecCustomMode
	//普通配置--GeneralEncCfg[ch]--分辨率：帧率：码流类型：码率类型：质量：量化系数：码率上限：码率下限
	//手动配置--ManualEncCfg[ch]
	//事件配置--EventEncCfg[ch]
	switch (nEnctype)
	{
		case 0:				//普通配置
			{
				snprintf(key_str, sizeof(key_str), "GeneralEncCfg%02d", m_channel);

				break;
			}
		case 1:				//手动配置
			{
				snprintf(key_str, sizeof(key_str), "ManualEncCfg%02d", m_channel);
				break;
			}
		case 2:				//事件配置
			{
				snprintf(key_str, sizeof(key_str), "EventEncCfg%02d", m_channel);
				break;
			}
		default:
			{

				return -1;
			}
	}
	snprintf(value, sizeof(value), "%d:%d:%d:%d:%d:%d:%d:%d",
			tmp_info.byResolution, tmp_info.dwVideoFrameRate, tmp_info.byStreamType, tmp_info.byBitrateType,
			tmp_info.byPicQuality, tmp_info.dwQuotiety, tmp_info.dwVideoBitrateH, tmp_info.dwVideoBitrateL);

	AVL_WARN("key_str=%s, value = %s\n", key_str, value);
	nRet = pCConfig->SetValue(RecordCfgSection, key_str, value);
	if (nRet == 0 && pCConfig->m_bReadOnlyCfgFlag == 1)
	{
		pCConfig->GetValue(RecordCfgSection, key_str, "", value, sizeof(value));
		sscanf(value, "%d:%d:%d:%d:%d:%d:%d:%d",
				(int *)&tmp_info.byResolution, &tmp_info.dwVideoFrameRate, &tmp_info.byStreamType, &tmp_info.byBitrateType,
				&tmp_info.byPicQuality, &tmp_info.dwQuotiety, &tmp_info.dwVideoBitrateH, &tmp_info.dwVideoBitrateL);
	}

	int nMainVideoWidth = 0;
	int nMainVideoHeight = 0;
	
    snprintf(key_str, sizeof(key_str), "MainCoderFormatWidth%02d", m_channel);
	nMainVideoWidth = pCConfig->GetValue(RecordCfgSection, key_str, 0);

	snprintf(key_str, sizeof(key_str), "MainCoderFormatHeight%02d", m_channel);
	nMainVideoHeight = pCConfig->GetValue(RecordCfgSection, key_str, 0);

    snprintf(key_str, sizeof(key_str), "MainCoderFormatWidth%02d", m_channel);
	nRet = pCConfig->SetValue(RecordCfgSection, key_str, tmp_info.dwResWidth);
	if (nRet == 0 && pCConfig->m_bReadOnlyCfgFlag == 1)
	{
		tmp_info.dwResWidth = pCConfig->GetValue(RecordCfgSection, key_str, 0);
	}

    snprintf(key_str, sizeof(key_str), "MainCoderFormatHeight%02d", m_channel);
	nRet = pCConfig->SetValue(RecordCfgSection, key_str, tmp_info.dwResHeight);
	if (nRet == 0 && pCConfig->m_bReadOnlyCfgFlag == 1)
	{
		tmp_info.dwResHeight = pCConfig->GetValue(RecordCfgSection, key_str, 0);
	}
	
    switch (nBoardID)
    {
        case 0x2099:
        case 0x208F:
        case 0x2074:
		case 0x2080:
            if (tmp_info.dwResWidth == 2304 && tmp_info.dwResHeight == 1296)
            {
                /*设置为标准模式*/
                nSmartAIWorkMode = 0;
            }
            //else if (tmp_info.dwResWidth == 1920 && tmp_info.dwResHeight == 1080)
            else
            {
                /*设置为人脸人形模式*/
                nSmartAIWorkMode = 1;
            }
            pCConfig->SetValue("PUB", "SmartAIWorkMode", nSmartAIWorkMode);

            if (nSmartAIWorkMode == 0)  //当前是标准模式
            {
				if (nMainVideoWidth != 2304 && nMainVideoHeight != 1296 
                && tmp_info.dwResWidth == 2304 && tmp_info.dwResHeight == 1296)
                {
                    pCConfig->SetValue("PUB", "ForceAIEnable", 2);
                } 
            }
            else  //人形/人脸模式
            {
				if (nMainVideoWidth != tmp_info.dwResWidth && nMainVideoHeight != tmp_info.dwResHeight)
                {
                    pCConfig->SetValue("PUB", "ForceAIEnable", 1);
                }
            }

            /*删除原来的标记文件*/
            snprintf(acCmd, sizeof(acCmd), "rm -rf /mnt/mtd/config/.smartai:*");
            mysystem(acCmd);
            
            /*创建新的标记文件*/
            snprintf(acCmd, sizeof(acCmd), "touch /mnt/mtd/config/.smartai:%d", nSmartAIWorkMode);
            mysystem(acCmd);

            break;
        default:
            break;
    }

	//更新配置
	memcpy(&RecordCfg[nEnctype], &tmp_info, sizeof (tagEncCfg));
	//更新当前配置，使其有效
	m_bChangeCfg = 1;
	TRACE_AVL("\n[software] [%s-%d]===1====%d:%d:%d:%d:%d:%d:%d:%d====nEnctype=%d====\n",__FUNCTION__,__LINE__,
			tmp_info.byResolution, tmp_info.dwVideoFrameRate, tmp_info.byStreamType, tmp_info.byBitrateType,
			tmp_info.byPicQuality, tmp_info.dwQuotiety, tmp_info.dwVideoBitrateH, tmp_info.dwVideoBitrateL,nEnctype);

	CheckEncCfg();
	
	TRACE_AVL("\n[software] [%s-%d]===2====%d:%d:%d:%d:%d:%d:%d:%d====nEnctype=%d====\n",__FUNCTION__,__LINE__,
			tmp_info.byResolution, tmp_info.dwVideoFrameRate, tmp_info.byStreamType, tmp_info.byBitrateType,
			tmp_info.byPicQuality, tmp_info.dwQuotiety, tmp_info.dwVideoBitrateH, tmp_info.dwVideoBitrateL,nEnctype);

	return (int) 0;
}

int CRecord::HttpTriggerRecord(void)
{
    if (m_bHttpEventIsComming)
    {
        printf("------Is Dealing with trigger event! Please wait ------\n");
        return -14;
    }

    printf("http event trig record ...\n");

    m_bHttpEventIsComming = 1;

    return 0;
}

//!\brief   恢复编码参数到出厂设置
//!\param   int nEnctype        录像的编码类型
//!\return 成功为0，失败为-1
int CRecord::ResumeEncCfg(int nEnctype)
{
	char key_str[30];

	switch (nEnctype)
	{
		case 0:				//普通配置
			{
				snprintf(key_str, sizeof(key_str), "GeneralEncCfg%02d", m_channel);
				break;
			}

		case 1:				//手动配置
			{
				snprintf(key_str, sizeof(key_str), "ManualEncCfg%02d", m_channel);
				break;
			}

		case 2:				//事件配置
			{
				snprintf(key_str, sizeof(key_str), "EventEncCfg%02d", m_channel);
				break;
			}

		default:
			{
				return -1;
			}
	}

	//获取新的配置文件读取实例
	CConfig *pDefaultCfg = CConfig::NewInstance();

	//从缺省配置文件中读取
	int nRet = pDefaultCfg->LoadFileRO(IDVR_DEFAULT_CONFIG_FILE_PATH);

	if (0 == nRet)
	{
		char value[VALUE_MAX_LEN];

		tagEncCfg tEncCfg;

		tagEncCfg *Info = &tEncCfg;

		pDefaultCfg->GetValue(RecordCfgSection, key_str, DefEncCfgStr, value, VALUE_MAX_LEN);

		sscanf(value, "%d:%d:%d:%d:%d:%d:%d:%d",
			   (int *)&Info->byResolution, &Info->dwVideoFrameRate, &Info->byStreamType, &Info->byBitrateType,
			   &Info->byPicQuality, &Info->dwQuotiety, &Info->dwVideoBitrateH, &Info->dwVideoBitrateL);

        snprintf(key_str, sizeof(key_str), "MainCoderFormatWidth%02d", m_channel);
        Info->dwResWidth = pDefaultCfg->GetValue(RecordCfgSection, key_str, 1920);
        
        snprintf(key_str, sizeof(key_str), "MainCoderFormatHeight%02d", m_channel);
        Info->dwResHeight = pDefaultCfg->GetValue(RecordCfgSection, key_str, 1080);

		if (Info->dwVideoFrameRate != 0xFF && (Info->dwVideoFrameRate < 1 || Info->dwVideoFrameRate > 20))
		{
			Info->dwVideoFrameRate = 0xFF;
		}
		if (Info->dwQuotiety < 0 || Info->dwQuotiety > 5)
		{
			Info->dwQuotiety = 5;
		}
		if (Info->byStreamType > 1 || Info->byStreamType < 0)
		{
			Info->byStreamType = 1;
		}
		if (Info->byPicQuality > 3 || Info->byPicQuality < 0)
		{
			Info->byPicQuality = 1;
		}
		if (Info->byBitrateType > 3 || Info->byBitrateType < 0)
		{
			Info->byBitrateType = 1;
		}

		nRet = SetEncCfg(Info, nEnctype);
	}
	else
	{
		AVL_ERR("[CRecord:%s] LoadFile %s fail\n", __FUNCTION__, IDVR_DEFAULT_CONFIG_FILE_PATH);
	}

	CConfig::ReleaseInstance(pDefaultCfg);

	return nRet;
}

//##ModelId=49B715AC003A
int CRecord::GetPreTime()
{
	return (int) m_iPreTime;
}

//##ModelId=49B715DB02CC
int CRecord::SetPreTime(int nTime)
{
	if (nTime < 0)
	{
		AVL_ERR("[CRecord:%s] invalid param :nTime = %d\n", __FUNCTION__, nTime);
		return -1;
	}

	if ((int)m_iPreTime != nTime)
	{
    	char key_str[30] = {0};
    	snprintf(key_str, sizeof(key_str), "PrevRecTime%02d", m_channel);
    	pCConfig->SetValue(RecordCfgSection, key_str, nTime);
    	m_iPreTime = nTime;
		#ifdef __SUPPORT_SD_CARD__
        pCDPManager->DR_SetPrevTimeSec(m_channel, m_iPreTime);
		#endif
	    printf("Set prev record time is %d second(s)\n", m_iPreTime);
    }

	return (int) 0;
}

int CRecord::GetDelayTime()
{
	return (int) m_iDelayTime;
}

int CRecord::SetDelayTime(int nTime)
{
	if (nTime < 0)
	{
		AVL_ERR("[CRecord:%s] invalid param :nTime = %d\n", __FUNCTION__, nTime);
		return -1;
	}

	if (m_iDelayTime != nTime)
	{
    	char key_str[30] = {0};
    	snprintf(key_str, sizeof(key_str), "DelayRecTime%02d", m_channel);
    	pCConfig->SetValue(RecordCfgSection, key_str, nTime);
    	m_iDelayTime = nTime;    	
	    printf("Set delay record time is %d second(s)\n", m_iDelayTime);
	}
	return (int) 0;
}

int CRecord::GetSchedule(tagRecordSchedule * cfg)
{
	if (NULL == cfg)
	{
		AVL_ERR("[CRecord:%s]cfg==NULL\n", __FUNCTION__);
		return -1;
	}
	memcpy(cfg, &m_ScheduleCfg, sizeof (tagRecordSchedule));
	return (int) 0;
}

int CRecord::SetSchedule(const tagRecordSchedule * cfg)
{
	char value[VALUE_MAX_LEN] = {0};

	char key_str[30] = {0};

	int i, j;

	if (NULL == cfg)
	{
		AVL_ERR("[CRecord:%s]cfg==NULL\n", __FUNCTION__);
		return -1;
	}
	if (cfg->bytState > 2)
	{
		AVL_ERR("[CRecord:%s]cfg->bytState=%d\n", __FUNCTION__, cfg->bytState);
		return -1;
	}

	for (i = 0; i < 7; i++)
	{
		for (j = 0; j < 48; j++)
		{
			if (cfg->bytRecordTime[i][j] < 0 || cfg->bytRecordTime[i][j] > 4)
			{
				AVL_ERR("[CRecord:%s]cfg->bytRecordTime[%d][%d]=%d\n", __FUNCTION__, i, j, cfg->bytRecordTime[i][j]);
				return -1;
			}
		}
	}

	//录像计划--
	//ScheduleCfgEnable[ch]=[启用]
	//ScheduleCfg[ch]_[星期]=[48]--[启用][星期][半小时]
	snprintf(key_str, sizeof(key_str), "ScheduleCfgEnable%02d", m_channel);
	pCConfig->SetValue(RecordCfgSection, key_str, cfg->bytState);

	for (i = 0; i < 7; i++)
	{
		for (j = 0; j < 48; j++)
		{
			value[j] = cfg->bytRecordTime[i][j] + '0';
		}
		value[j] = 0;
		snprintf(key_str, sizeof(key_str), "ScheduleCfg%02d_%01d", m_channel, i);

		pCConfig->SetValue(RecordCfgSection, key_str, value);
		TRACE_AVL("[CRecord:%s]cfg->bytState=%d,Write:%s\n", __FUNCTION__, cfg->bytState, value);
	}
	//更新计划
	memcpy(&m_ScheduleCfg, cfg, sizeof (tagRecordSchedule));
	return (int) 0;
}


int CRecord::GetScheduleV2(tagRecordScheduleV2 *cfg)
{    
    if (NULL == cfg)
	{
		AVL_ERR("[CRecord:%s]cfg==NULL\n", __FUNCTION__);
		return -1;
	}
	memcpy(cfg, &m_ScheduleCfgV2, sizeof(tagRecordScheduleV2));
	return (int) 0;
}

int CRecord::SetScheduleV2(tagRecordScheduleV2 *cfg)
{
	char key_str[64] = {0};

	if (cfg->nRecordType > 1)
	{
		AVL_ERR("[CRecord:%s]cfg->nRecordType=%d\n", __FUNCTION__, cfg->nRecordType);
		return -1;
	}

	if (cfg->nEnable > 1)
	{
        AVL_ERR("[CRecord:%s]cfg->nEnable=%d\n", __FUNCTION__, cfg->nEnable);
		return -1;
	}

	//录像状态
	snprintf(key_str, sizeof(key_str), "ScheduleCfgEnableV2%02d", m_channel);
	pCConfig->SetValue(RecordCfgSection, key_str, cfg->nEnable);
	//录像类型	
    memset(key_str, 0, sizeof(key_str));   
	snprintf(key_str, sizeof(key_str), "ScheduleRecordType%02d", m_channel);
	pCConfig->SetValue(RecordCfgSection, key_str, cfg->nRecordType);
	//录像开始时间小时
    memset(key_str, 0, sizeof(key_str));   
	snprintf(key_str, sizeof(key_str), "ScheduleRecordStartTimeHour%02d", m_channel);
	pCConfig->SetValue(RecordCfgSection, key_str, cfg->startTime.nHour);
	//录像开始时间分钟
	memset(key_str, 0, sizeof(key_str));   
	snprintf(key_str, sizeof(key_str), "ScheduleRecordStartTimeMinute%02d", m_channel);
	pCConfig->SetValue(RecordCfgSection, key_str, cfg->startTime.nMinute);
	//录像开始时间秒数
	memset(key_str, 0, sizeof(key_str));   
	snprintf(key_str, sizeof(key_str), "ScheduleRecordStartTimeSecond%02d", m_channel);
	pCConfig->SetValue(RecordCfgSection, key_str, cfg->startTime.nSecond);
	//录像结束时间小时
    memset(key_str, 0, sizeof(key_str));   
	snprintf(key_str, sizeof(key_str), "ScheduleRecordStopTimeHour%02d", m_channel);
	pCConfig->SetValue(RecordCfgSection, key_str, cfg->stopTime.nHour);
	//录像结束时间分钟
	memset(key_str, 0, sizeof(key_str));   
	snprintf(key_str, sizeof(key_str), "ScheduleRecordStopTimeMinute%02d", m_channel);
	pCConfig->SetValue(RecordCfgSection, key_str, cfg->stopTime.nMinute);
	//录像结束时间秒数
	memset(key_str, 0, sizeof(key_str));   
	snprintf(key_str, sizeof(key_str), "ScheduleRecordStopTimeSecond%02d", m_channel);
	pCConfig->SetValue(RecordCfgSection, key_str, cfg->stopTime.nSecond);

	//更新计划
	memcpy(&m_ScheduleCfgV2, cfg, sizeof(tagRecordScheduleV2));

	SetUsrRecordType(m_ScheduleCfgV2.nRecordType,m_ScheduleCfgV2.nEnable);

	return (int) 0;
}


int CRecord::CheckEncCfg()
{
	int result = 0;
	CODEC_PARA_S cp;
	BoardSensorType_t Sensortype;
	char szSectionComm[36] = {0};
	int ivalue_sensor;

	//定码流设置索引
	/*
	int BitRect_chr[3][3]={	{0x400000,0x200000,0x180000} ,
							{0x600000,0x400000,0x200000},
							{0x800000,0x600000,0x400000} };*/
	if (m_iCustomMode)
	{
		if (pRecordCfg == &RecordCfg[m_iEncCfgtype])
		{		
			if (!m_bChangeCfg)	//且配置未更新
			{
				return 0;
			}

		}
		pRecordCfg = &RecordCfg[m_iEncCfgtype];
	}
	else
	{
		if (pRecordCfg == &RecordCfg[0])
		{	
			if (!m_bChangeCfg)	//且配置未更新
			{
				return 0;
			}

		}
		pRecordCfg = &RecordCfg[0];
	}
	m_bChangeCfg = 0;
	
	//1.视频流 3.音视频流
	TRACE_AVL("\n[software] [%s-%d]===CheckEncCfg==pRecordCfg->byStreamType=%d==m_iCustomMode=%d==m_iEncCfgtype=%d==\n",__FUNCTION__,__LINE__,
				pRecordCfg->byStreamType,m_iCustomMode,m_iEncCfgtype);
	result = ZWSys_EncSetStreamType(m_hEnc, (pRecordCfg->byStreamType == 0 ? 1 : 3));
	if (result)
	{
		AVL_ERR("[CRecord:%s]ZWSys_EncSetStreamType failed (%d)\n", __FUNCTION__, result);
		return result;
	}
	//cp.BitRateCtlMode = pRecordCfg->byBitrateType != 0;	//不支持限码率

	//码率设定	
	cp.BitRateCtlMode = pRecordCfg->byBitrateType;
	TRACE_AVL("\n[software] [%s-%d]===CheckEncCfg==码率设定==pRecordCfg->byBitrateType=%d==========cp.BitRateCtlMode=%d=======\n",__FUNCTION__,__LINE__,
		pRecordCfg->byBitrateType,cp.BitRateCtlMode);
	cp.PictureFormat = SwitchPicFormat(pRecordCfg->byResolution);
	cp.PictureRes.m_PicWidth = pRecordCfg->dwResWidth;
	cp.PictureRes.m_PicHeight = pRecordCfg->dwResHeight;

	ZWSys_GetSensorType(&Sensortype);
	snprintf(szSectionComm, sizeof(szSectionComm), "ISP_S%d",Sensortype);
	ivalue_sensor = pCConfig->GetValue(szSectionComm, "InputFrequency", 0);
	if (0 == ivalue_sensor)
	{
		m_SensorFrequency = CMOS_FREQUENCY_50HZ;
	}
	else
	{
		if (1 == ivalue_sensor)
		{
			m_SensorFrequency = CMOS_FREQUENCY_60HZ;
		}
		else
		{
			m_SensorFrequency = CMOS_FREQUENCY_50HZ;
		}
	}
	
	cp.FrameRate = pRecordCfg->dwVideoFrameRate;
	cp.KeyFrameInterval = pRecordCfg->dwVideoFrameRate;	//关键帧间隔等于帧率
	if (cp.KeyFrameInterval == 0xFF)
	{
		if (m_SensorFrequency == CMOS_FREQUENCY_50HZ)
		{
			cp.KeyFrameInterval = 25;
		}
		else
		{
			if (m_SensorFrequency == CMOS_FREQUENCY_60HZ)
			{
				cp.KeyFrameInterval = 30;
			}
			else
			{
				cp.KeyFrameInterval = 25;
			}
		}
		cp.FrameRate = cp.KeyFrameInterval;
	}

	cp.BitRate = pRecordCfg->dwVideoBitrateH * 1000;//<< 10;
#ifdef DO_SATA

	cp.IQuantVal = cp.PQuantVal = pRecordCfg->dwQuotiety;
	if (cp.BitRateCtlMode == 0)			//变码率
	{
		switch (pRecordCfg->byPicQuality)
		{
			case 0:			//高质量
				{
					cp.IQuantVal = cp.PQuantVal = 16;
					break;
				}
			case 1:			//标准
				{
					cp.IQuantVal = cp.PQuantVal = 28;
					break;
				}
			case 2:			//低
				{
					cp.IQuantVal = cp.PQuantVal = 36;
					break;
				}
			case 3:			//自定义
				{
					//  cp.IQuantVal=cp.PQuantVal=16
					break;
				}
		}
	}
#else

	//cp.PicQualityLever = 0;
	if (cp.BitRateCtlMode == 0)			//变码率
	{
	cp.PicQualityLever = pRecordCfg->dwQuotiety;
	/*	switch (pRecordCfg->dwQuotiety)
		{
			case 0:			//高质量
				{
					cp.BitRate = 2 << 20;	//4<<20;
					break;
				}
			case 1:			//标准
				{
					cp.BitRate = 3 << 19;	//3<<20;
					break;
				}
			case 2:			//低
				{
					cp.BitRate = 1 << 20;	//2<<20;
					break;
				}
			case 3:
				{
					cp.BitRate = 3 << 18;	//3<<19;
					break;
				}
			case 4:
				{
					cp.BitRate = 1 << 19;	////1<<20;
					break;
				}
			case 5:
				{
					cp.BitRate = 1 << 18;	//1<<19;
					break;
				}
		}
		*/
	}
	else
	{
		//录像质量 0-高质量 1-标准 2-低 3-自定义
		//分辨率0-D1, 2-720 2-1080p
		//对应的定码率大小为[0~2][0~2]={4,2,1.5,  6,4,2,  8,6,4}//单位兆码率
		cp.PicQualityLever = pRecordCfg->dwQuotiety;
/*		if ((pRecordCfg->byPicQuality >= 0)&&(pRecordCfg->byPicQuality < 3))
		{
			cp.BitRate = BitRect_chr[pRecordCfg->byPicQuality][pRecordCfg->byResolution];
		}	
		*/
	}
#endif

#ifdef DO_SATA
	TRACE_AVL("[CRecord:%s] ch=%d fs=%d,br=%d,cbr=%d,qp=%d\n", __FUNCTION__, m_channel, cp.FrameRate, cp.BitRate, cp.BitRateCtlMode, cp.IQuantVal);
	MP4Sys_EncSetIBPMode(m_hEnc, cp.KeyFrameInterval, 0, 1, cp.FrameRate);
	MP4Sys_EncSetDefaultQuant(m_hEnc, cp.IQuantVal, cp.PQuantVal, cp.PQuantVal);
	result = MP4Sys_EncSetupBitrateControl(m_hEnc, cp.BitRate, cp.BitRateCtlMode);
#else
	TRACE_AVL("[CRecord:%s] ch=%d pic=%d fs=%d,br=%d,cbr=%d\n", __FUNCTION__, m_channel, cp.PictureFormat, cp.FrameRate, cp.BitRate,
			   cp.BitRateCtlMode);
	
	result = ZWSys_EncSetCodecPara(m_hEnc, 0, &cp, (EncodeType_t)m_AdvanceEncCfg.nVideoCodecModeNormal[m_channel]);
#endif
	if (result)
	{
		AVL_ERR("[CRecord:%s]ZWSys_EncSetCodecPara failed (%d)\n", __FUNCTION__, result);
		return result;
	}
	
	return 0;
}

int CRecord::GetAdvanceEnc(int *NormalAdvEncType, int *EventAdvEncType, tagVIDEO_SHELTER *pCfg)
{
    *NormalAdvEncType = m_AdvanceEncCfg.nVideoCodecModeNormal[m_channel];
    *EventAdvEncType = m_AdvanceEncCfg.nVideoCodecModeEvent[m_channel];
    memcpy(pCfg, &(m_AdvanceEncCfg.struShelter[m_channel]), sizeof(tagVIDEO_SHELTER));

    return 0;
}

//事件编码处理未实现
int CRecord::SetAdvanceEnc(int NormalAdvEncType, int EventAdvEncType, tagVIDEO_SHELTER *pCfg)
{
    int s32Ret = 0;
	char value[VALUE_MAX_LEN];
	char key_str[36] = {0};
	CODEC_PARA_S cp;

	if (m_AdvanceEncCfg.nVideoCodecModeNormal[m_channel] != NormalAdvEncType)
	{
		cp.PictureFormat = SwitchPicFormat(RecordCfg[0].byResolution);
        cp.PictureRes.m_PicWidth = RecordCfg[0].dwResWidth;
        cp.PictureRes.m_PicHeight = RecordCfg[0].dwResHeight;
		cp.FrameRate = RecordCfg[0].dwVideoFrameRate;
		cp.KeyFrameInterval = RecordCfg[0].dwVideoFrameRate;
		cp.BitRateCtlMode = RecordCfg[0].byBitrateType;
		cp.BitRate = RecordCfg[0].dwVideoBitrateH * 1000;
		cp.PicQualityLever = RecordCfg[0].dwQuotiety;

	 	if(1 == bSupportRtmp)
		{
			if(cp.FrameRate < 15)
				cp.FrameRate = 15;
		} 				
		snprintf(key_str, sizeof(key_str), "nVideoAdvanceCodec%02d", m_channel);
	    snprintf(value, sizeof(value), "%d:%d", NormalAdvEncType, EventAdvEncType);
	    s32Ret = pCConfig->SetValue(RecordCfgSection, key_str, value);
		if (s32Ret == 0 && pCConfig->m_bReadOnlyCfgFlag == 1)
		{
			m_AdvanceEncCfg.nVideoCodecModeNormal[m_channel] = pCConfig->GetValue(RecordCfgSection, key_str, 0);
			NormalAdvEncType = m_AdvanceEncCfg.nVideoCodecModeNormal[m_channel];
		}
		s32Ret = ZWSys_EncSetCodecPara(m_hEnc, 0, &cp, (EncodeType_t)NormalAdvEncType);
		if (s32Ret)
		{
			AVL_ERR("[CRecord:%s]ZWSys_EncSetCodecPara failed (%d)\n", __FUNCTION__, s32Ret);
			return s32Ret;
		} 
	}
	else
		return 0;

    snprintf(key_str, sizeof(key_str), "nVideoShelter%02d", m_channel);
    snprintf(value, sizeof(value), "%d:%d:%d:%d", pCfg->wHideAreaTopLeftX, 
                                  pCfg->wHideAreaTopLeftY,
                                  pCfg->wHideAreaWidth,
                                  pCfg->wHideAreaHeight);
    pCConfig->SetValue(RecordCfgSection, key_str, value);

    m_AdvanceEncCfg.nVideoCodecModeNormal[m_channel] = NormalAdvEncType;
    m_AdvanceEncCfg.nVideoCodecModeEvent[m_channel]  = EventAdvEncType;
    memcpy(&(m_AdvanceEncCfg.struShelter[m_channel]), pCfg, sizeof(tagVIDEO_SHELTER));

    return s32Ret;
}

PictureFormat_t CRecord::SwitchPicFormat(int PicFormat)
{
    PictureFormat_t SdkPicFormat = ENC_HD720_FORMAT;
	
	switch (PicFormat)
    {
        case eResolutionIndex_CIF:		SdkPicFormat = ENC_CIF_FORMAT; break;
        case eResolutionIndex_D1:		SdkPicFormat = ENC_D1_FORMAT; break;
        case eResolutionIndex_640x360:	SdkPicFormat = ENC_360P_FORMAT; break;
        case eResolutionIndex_QVGA:		SdkPicFormat = ENC_QVGA_FORMAT; break;
        case eResolutionIndex_VGA:		SdkPicFormat = ENC_VGA_FORMAT; break;
        case eResolutionIndex_720P:		SdkPicFormat = ENC_HD720_FORMAT; break;
        case eResolutionIndex_QuadVGA:	SdkPicFormat = ENC_QuadVGA_FORMAT; break;
        case eResolutionIndex_SXGA:		SdkPicFormat = ENC_SXGA_FORMAT; break;
        case eResolutionIndex_UXGA:		SdkPicFormat = ENC_UXGA_FORMAT; break;
        case eResolutionIndex_1080P:	SdkPicFormat = ENC_HD1080_FORMAT; break;        
        case eResolutionIndex_QXGA:		SdkPicFormat = ENC_QXGA_FORMAT; break;        
        case eResolutionIndex_UWXGA:	SdkPicFormat = ENC_UWXGA_FORMAT; break;        
        case eResolutionIndex_USXGA:	SdkPicFormat = ENC_USXGA_FORMAT; break;        
        default :
			AVL_ERR("Not support Picture Format %d\n", PicFormat); 
			SdkPicFormat = ENC_CIF_FORMAT;
			break;
    }
    
    return SdkPicFormat;
}

//##ModelId=49B732E400CC
int CRecord::Schedule()
{
#if defined (__SUPPORT_SD_CARD__)
	int day = 0;
	int seg = 0;
	int rec_type = 0;

	time_t curr_time;

	struct tm curr_tm;

	XTRACE_MUTEX_LOCK(&m_lock);

	//获取当前时间段信息
	curr_time = time(NULL);
	localtime_r(&curr_time, &curr_tm);
	day = curr_tm.tm_wday;
	seg = (curr_tm.tm_hour << 1) + (curr_tm.tm_min >= 30);

	//获取设置的计划录像类型
	rec_type = m_ScheduleCfg.bytRecordTime[day][seg];

    if (m_bHttpEventIsComming)
    {
        m_iRecState = RECORD_TYPE_HTTP_TRIG;
        m_bHttpEventIsComming = 0;
    }
    else
    {
        if (3 == rec_type)           // 报警输入录像
        {
            if (m_bAlarmInAlarmRec)
            {
                m_iRecState = RECORD_TYPE_ALARM;
                m_bAlarmInAlarmRec = 0;
            }
            else
            {
                m_iRecState = RECORD_TYPE_NO;
            }
        }
		//fjz-notice
		else if (4 == rec_type)      // 智能侦测录像
        {
            if (m_bAlarmInAlarmRec)  // 如果当前有报警输入，则报警输入录像优先
            {
                m_iRecState = RECORD_TYPE_ALARM;
                m_bAlarmInAlarmRec = 0;
            }
			else if (m_bAiAlarmRec)
            {
                m_iRecState = RECORD_TYPE_AI;
                m_bAiAlarmRec = 0;
            }          
            else
            {
                m_iRecState = RECORD_TYPE_NO;
            }
        }
        else if (2 == rec_type)      // 移动侦测录像
        {
            if (m_bAlarmInAlarmRec)  // 如果当前有报警输入，则报警输入录像优先
            {
                m_iRecState = RECORD_TYPE_ALARM;
                m_bAlarmInAlarmRec = 0;
            }
			else if (m_bAiAlarmRec)
            {
                m_iRecState = RECORD_TYPE_AI;
                m_bAiAlarmRec = 0;
            }  
            else if (m_bMotionAlarmRec) // 其次才是移动侦测
            {
                m_iRecState = RECORD_TYPE_MOTION;
                m_bMotionAlarmRec = 0;
            }            
            else
            {
                m_iRecState = RECORD_TYPE_NO;
            }
        }
        else if (1 == rec_type)     // 定时录像
        {
            if (m_bAlarmInAlarmRec) // 如果当前有报警输入，则报警输入录像优先
            {
                m_iRecState = RECORD_TYPE_ALARM;
                m_bAlarmInAlarmRec = 0;
            }
			else if (m_bAiAlarmRec)
            {
                m_iRecState = RECORD_TYPE_AI;
                m_bAiAlarmRec = 0;
            } 
            else if (m_bMotionAlarmRec)  // 其次才是移动侦测
            {
                m_iRecState = RECORD_TYPE_MOTION;
                m_bMotionAlarmRec = 0;
            }
            else
            {
                m_iRecState = RECORD_TYPE_GENERAL;
            }
        }
        else
        {
            m_iRecState = RECORD_TYPE_NO;
        }
    }

    if (RECORD_TYPE_NO != m_iRecState)
    {
        // 根据当前录像码率用于定位
        StartAviRecord(m_iRecState);
        m_iRecState = RECORD_TYPE_NO;
    }

	XTRACE_MUTEX_UNLOCK(&m_lock);
#endif
	return (int) 0;
}

int CRecord::ScheduleV2()
{
#if defined (__SUPPORT_SD_CARD__)
	int nCurHour = 0;
	int nCurMinu = 0;
	int nRecType = 0;
	int nStarTimeHour = 0;
	int nStarTimeMin = 0;
	int nStopTimeHour = 0;
	int nStopTimeMin = 0;
	int bIsAcrossDay = 0;
	int bRecordFlag = 0;
	unsigned long nStartTimeSec = 0;
	unsigned long nStopTimeSec = 0;
	unsigned long nCurTimeSec = 0;
	time_t curr_time;
	struct tm curr_tm;
	XTRACE_MUTEX_LOCK(&m_lock);

	//获取当前时间段信息
	curr_time = time(NULL);
	localtime_r(&curr_time, &curr_tm);
    //获取当前时间段的小时和分钟和总秒数
	nCurHour = curr_tm.tm_hour;	
	nCurMinu = curr_tm.tm_min;    
    nCurTimeSec = nCurHour * 3600 + nCurMinu * 60;    
	//获取设置录像起始时间小时和分钟
	nStarTimeHour = m_ScheduleCfgV2.startTime.nHour;	    
	nStarTimeMin = m_ScheduleCfgV2.startTime.nMinute;	
	nStartTimeSec = nStarTimeHour * 3600 + nStarTimeMin * 60;	
	//获取设置录像结束时间小时和分钟和总秒数
	nStopTimeHour = m_ScheduleCfgV2.stopTime.nHour;	
	nStopTimeMin = m_ScheduleCfgV2.stopTime.nMinute;	
    nStopTimeSec = nStopTimeHour * 3600 + nStopTimeMin * 60;
	//获取当前录像类型
	if (m_ScheduleCfgV2.nEnable == 0)
	{
        //printf("[%s,%d]Forbid Record\n", __FUNCTION__, __LINE__);        
        XTRACE_MUTEX_UNLOCK(&m_lock);
        return 0;
	}

    //获取设置的录像起始时间是否跨天
    if (nStarTimeHour > nStopTimeHour)
    {
        bIsAcrossDay = 1;  
    }  
    //判断当前时间是否在录像时间范围内
    //考虑到跨天时间，需要分段来判断
    if (bIsAcrossDay == 0)
    {
        //设置的非跨天时间段
        if ((nCurTimeSec >= nStartTimeSec) && (nCurTimeSec <= nStopTimeSec))
        {
            bRecordFlag = 1; 
        }
        else
        {
            bRecordFlag = 0;
        }
    }
    else
    {        
        //设置的跨天时间段,分两个时间段来比较
        //第一个时间段为:录像开始-24:00,第二个时间段为00:00-录像结束时间
        if (((nCurTimeSec >= nStartTimeSec) && (nCurHour <= 23)) || (nCurTimeSec <= nStopTimeSec))
        {            
            bRecordFlag = 1; 
        }
        else
        {
            bRecordFlag = 0; 
        }
    }
	 
	//获取设置的计划录像类型
	nRecType = m_ScheduleCfgV2.nRecordType;	

	//全天录像+布防时间段外+事件联动录像打开+事件触发 = 录像标记为事件录像
	if(0 == nRecType)
	{
		if(m_bAlarmInAlarmRec || m_bAiAlarmRec || m_bMotionAlarmRec)
		{
			 bRecordFlag = 1; 
		}
	}
	
    if (m_bHttpEventIsComming)
    {
        m_iRecState = RECORD_TYPE_HTTP_TRIG;
        m_bHttpEventIsComming = 0;
    }
    else
    {
        if (1 == nRecType)      //事件录像
        {
            if (m_bAlarmInAlarmRec)  //如果当前有报警输入，则报警输入录像优先
            {
                m_iRecState = RECORD_TYPE_ALARM;
                m_bAlarmInAlarmRec = 0;
            }
			else if (m_bAiAlarmRec) //如果当前有只能人形检测报警录像
            {
                m_iRecState = RECORD_TYPE_AI;
                m_bAiAlarmRec = 0;
            }  
            else if (m_bMotionAlarmRec) //其次才是移动侦测
            {
                m_iRecState = RECORD_TYPE_MOTION;
                m_bMotionAlarmRec = 0;
            }
            else
            {
                m_iRecState = RECORD_TYPE_NO;
            }
        }
        else if (0 == nRecType)     //连续录像
        {
            if (m_bAlarmInAlarmRec) //如果当前有报警输入，则报警输入录像优先
            {
                m_iRecState = RECORD_TYPE_ALARM;
                m_bAlarmInAlarmRec = 0;
            }
			else if (m_bAiAlarmRec) //如果当前有人形检测报警录像
            {
                m_iRecState = RECORD_TYPE_AI;
                m_bAiAlarmRec = 0;
            }  
            else if (m_bMotionAlarmRec)  //其次才是移动侦测
            {
                m_iRecState = RECORD_TYPE_MOTION;
                m_bMotionAlarmRec = 0;
            }
            else
            {
                m_iRecState = RECORD_TYPE_GENERAL;
            }
        }
        else
        {
            m_iRecState = RECORD_TYPE_NO;
        }
    }    

    if (RECORD_TYPE_NO != m_iRecState)
    {
        if (bRecordFlag)
        {
            //根据当前录像事件类型开始录像
            StartAviRecord(m_iRecState);
        }
    }
	XTRACE_MUTEX_UNLOCK(&m_lock);
#endif
	return (int) 0;
}

//##ModelId=49B864580125
int CRecord::SetRecordCustomMode(int nCustomMode)
{
	if (nCustomMode < 0 || nCustomMode > 1)
	{
		AVL_ERR("[CRecord:%s]nCustomMode=%d\n", __FUNCTION__, nCustomMode);
		return -1;
	}
	//定制模式--RecCustomMode

	CConfig::GetInstance()->SetValue(RecordCfgSection, "RecCustomMode", nCustomMode);
	m_iCustomMode = nCustomMode;
	return (int) 0;
}

//##ModelId=49B8653002CA
int CRecord::GetRecordCustomMode(int *nCustomMode)
{
	if (nCustomMode == NULL)
	{
		AVL_ERR("[CRecord:%s]nCustomMode=NULL\n", __FUNCTION__);
		return -1;
	}

	*nCustomMode = m_iCustomMode;
	return (int) 0;
}

int CRecord::StopRecordAndUinitMemRecord()
{
#ifdef __SUPPORT_SD_CARD__	
    return pCDPManager->DR_StopRecordAndUinitMemRecord(m_channel);
#else
	return 0;
#endif
}

int CRecord::InputData(void *buff, int buflen, int FrameType, time_t tFrameTime, time_t tFrameTime_usec)
{
	if (buff == NULL)
	{
		TRACE_AVL("[%s %d] buff == NULL\n",__FILE__,__LINE__);
		return -1;
	}
#ifdef __SUPPORT_SD_CARD__
    /* 将数据塞入环形缓冲区(后面会有锁，这里不建议加锁) */
    return pCDPManager->DR_InputDataToRingBuffer(m_channel, tFrameTime, tFrameTime_usec, (const char *) buff, buflen, FrameType);
#else
	return 0;
#endif
}

