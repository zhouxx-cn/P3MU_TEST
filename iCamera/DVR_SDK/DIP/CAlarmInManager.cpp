// Copyright (C) 1991 - 1999 Rational Software Corporation

#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <endian.h>

#include "global_macro.h"
#include "CAlarmInManager.h"
#include "CActionAlarm.h"
#include "ddt_so.h"

extern "C" int     ZWSys_AlarmIn(int *pAlarmIndata );
extern "C" int     ZWSys_GetBoardInfo(int *pBoardInfo );

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

//点阵文件路径
static const char *FileNameAlarmInPixel = OSD_PIXEL;

CAlarmInManager *CAlarmInManager::m_pInstance = NULL;

int CAlarmInManager::m_iInstance_Cnt = 0;

CBAlert CAlarmInManager::m_FxnCallBack = NULL;

int CAlarmInManager::m_AlarmType = 0;


//##ModelId=499E620500AA
CAlarmInManager::CAlarmInManager()
{
	int i;

	m_bInit = 0;
	m_henc = NULL;
	theCEmbAlarmIn = NULL;
	theCExtAlarmIn = NULL;
	m_iEmbAlarmInNum = 0;
	m_iExtAlarmInNum = 0;
	m_hCheckThread = 0;
	m_bAlarmInStatus = 0;
	m_bEnable = 1;				//布防

	for (i = 0; i < MAX_ALARMIN_NUM; i++)
	{
		m_pActionAlarm[i] = NULL;
	}
	
	memset(&m_AudioAlarmScheduleCfg, 0, sizeof(tagAudioAlarmSchedule));
}

//##ModelId=499E62070017
CAlarmInManager::~CAlarmInManager()
{
	DIP_UnInit();
}

//!\brief 初始化
//!\return 0成功，其它失败
int CAlarmInManager::DIP_Init()
{
	int ret;

	int i;

	int nBoardID = 0;

	DeviceDescription dds;

	m_henc = ZWSys_EncChannelOpen(0);

	if (m_bInit)
	{
		TRACE("[CAlarmInManager:%s]already inited\n", __FUNCTION__);
		return 0;
	}
	pCConfig = CConfig::GetInstance();
	if (pCConfig == NULL)
	{
		TRACE_ERR("[CAlarmInManager:%s]pCConfig==NULL\n", __FUNCTION__);
		return -1;
	}


	//ret = EDVRS_DDT_GetDeviceDescriptionByID(&dds, MP4Sys_GetBoardInfo());
	ZWSys_GetBoardInfo(&nBoardID);
	ret = EDVRS_DDT_GetDeviceDescriptionByID(&dds, nBoardID);
	if (ret)
	{
		TRACE_ERR("[CAlarmInManager:%s]EDVRS_DDT_GetDeviceDescriptionByID failed\n", __FUNCTION__);
		return -1;
	}



	//实例化基本alarmout
	theCEmbAlarmIn = new CEmbAlarmIn;
	if (NULL == theCEmbAlarmIn)
	{
		TRACE_ERR("[CAlarmInManager:%s]CEmbAlarmIn new Failed \n", __FUNCTION__);
		DIP_UnInit();
		return -1;
	}
	theCEmbAlarmIn->SetNum(dds.bytAlarmInChannels);
	//实例化扩展alarmout
	theCExtAlarmIn = new CExtAlarmIn;
	if (NULL == theCExtAlarmIn)
	{
		TRACE_ERR("[CAlarmInManager:%s]CExtAlarmIn  new Failed \n", __FUNCTION__);
		DIP_UnInit();
		return -1;
	}
	if (theCEmbAlarmIn->Open())
	{
		TRACE_ERR("[CAlarmInManager:%s]theCEmbAlarmIn open Failed \n", __FUNCTION__);
		DIP_UnInit();
		return -1;
	}
	if (theCExtAlarmIn->Open())
	{
		TRACE_ERR("[CAlarmInManager:%s]theCExtAlarmIn open Failed \n", __FUNCTION__);
		DIP_UnInit();
		return -1;
	}
	//获取端口数
	m_iEmbAlarmInNum = theCEmbAlarmIn->GetNum();
	m_iExtAlarmInNum = theCExtAlarmIn->GetNum();

	for (i = 0; i < m_iEmbAlarmInNum + m_iExtAlarmInNum; i++)
	{
		m_pActionAlarm[i] = new CActionAlarm;
		if (m_pActionAlarm[i] == NULL)
		{
			DIP_UnInit();
			TRACE_ERR("[CAlarmInManager:%s]new CActionAlarm[%d] failed\n", __FUNCTION__, i);
			return -1;
		}
		ret = m_pActionAlarm[i]->Init(i, (char *) AlarmInSectionStr, "AlarmInAction", dds.bytVideoInChannels, dds.bytAlarmOutChannels);
		if (ret)
		{
			DIP_UnInit();
			TRACE_ERR("[CAlarmInManager:%s]CActionAlarm[%d] ->Init failed\n", __FUNCTION__, i);
			return -1;
		}
	}


	//导入配置

	LoadParam();
	m_bInit = 1;

	m_bCheck_exit = 0;
	//创建计划检测线程
	ret = pthread_create(&m_hCheckThread, NULL, Fnx_CheckThread, this);
	if (ret != 0)
	{
		TRACE_ERR("[CAlarmInManager:%s]pthread_create   failed (%d)\n", __FUNCTION__, ret);
		DIP_UnInit();
		return -1;
	}
	return 0;

}

//!\brief 释放资源
//!\return 0成功，其它失败
int CAlarmInManager::DIP_UnInit()
{

	int i;

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


	if (NULL != theCEmbAlarmIn)
	{
		theCEmbAlarmIn->Close();
		// CEmbAlarmIn::Release();
		delete theCEmbAlarmIn;

		theCEmbAlarmIn = NULL;
	}
	if (NULL != theCExtAlarmIn)
	{
		theCExtAlarmIn->Close();
		// CExtAlarmIn::Release();
		delete theCExtAlarmIn;

		theCExtAlarmIn = NULL;
	}

	m_iEmbAlarmInNum = 0;
	m_iExtAlarmInNum = 0;

	m_bAlarmInStatus = 0;

	for (i = 0; i < MAX_ALARMIN_NUM; i++)
	{
		if (m_pActionAlarm[i] != NULL)
		{
			delete m_pActionAlarm[i];

			m_pActionAlarm[i] = NULL;
		}

	}

	memset(&m_AudioAlarmScheduleCfg, 0, sizeof(tagAudioAlarmSchedule));
	
	return 0;
}

int CAlarmInManager::LoadParam()
{
	int i, j;
	int nPort;
	int result = 0;

	char key[100] = {0};
	char value[255] = {0};

	AlarmInSchedule *cfg;
	tagAudioAlarmSchedule *audioAlarmSche = NULL;

	DIR *pdir;

	//检查文件路径是否存在
	pdir = opendir(FileNameAlarmInPixel);
	if (NULL == pdir)
	{
		mkdir(FileNameAlarmInPixel, 777);
	}
	else
	{
		closedir(pdir);
	}

	//导入模式
	//读模式配置
	m_iMode = pCConfig->GetValue(AlarmInSectionStr, AlarmInModeKeyStr, 0);

	for (nPort = 0; nPort < (m_iEmbAlarmInNum + m_iExtAlarmInNum); nPort++)
	{
		//读名字
		snprintf(key, sizeof(key), "AlarmInName_%02d", nPort);
		pCConfig->GetValue(AlarmInSectionStr, key, "", m_strName[nPort], MAX_DIP_NAME_LEN);

		//类型
		snprintf(key, sizeof(key), "AlarmInType_%02d", nPort);
		m_bType[nPort] = pCConfig->GetValue(AlarmInSectionStr, key, 0);

		// 读计划--
		//ScheduleCfgEnable[ch]=[启用]
		//ScheduleCfg[ch]_[星期]=[48]--[启用][星期][半小时]
		cfg = &m_ptAlarmInSchedule[nPort];
		snprintf(key, sizeof(key), "AIScheduleCfgEnable%02d", nPort);
		cfg->bytState = pCConfig->GetValue(AlarmInSectionStr, key, 0);
		cfg->bytReserve[0] = 0;
		cfg->bytReserve[1] = 0;
		cfg->bytReserve[2] = 0;
		for (i = 0; i < 7; i++)
		{
			snprintf(key, sizeof(key), "AIScheduleCfg%02d_%01d", nPort, i);
			pCConfig->GetValue(AlarmInSectionStr, key, "000000000000000000000000000000000000000000000000", value, 255);
			for (j = 0; j < 48; j++)
			{
				cfg->dwTimeSeg[i][j] = value[j] - '0';
			}

		}

		//导入声音报警计划
		audioAlarmSche = &m_AudioAlarmScheduleCfg[nPort];
		snprintf(key, sizeof(key), "AudioAlarmScheduleCfgEnable%02d", nPort);
		audioAlarmSche->bytState = pCConfig->GetValue(AlarmInSectionStr, key, 0);//自定义、全天
		snprintf(key, sizeof(key), "AudioAlarmScheduleCfgEnableDN%02d", nPort);
		audioAlarmSche->nDayMode = pCConfig->GetValue(AlarmInSectionStr, key, 2);//白天，夜晚
		for (i = 0; i < 7; i++)
		{
			snprintf(key, sizeof(key), "AudioAlarmScheduleCfg%02d_%01d", nPort, i);
			pCConfig->GetValue(AlarmInSectionStr, key, SCHEDULE_TIME_ONE, value, sizeof(value));
			for (j = 0; j < 48; j++)
			{
				audioAlarmSche->dwTimeSeg[i][j] = value[j] - '0';
			}
		}
	}

	m_bEnable = 1;				//开始是布防

	return 0;
}

//##ModelId=49B9FCA80157
CAlarmInManager *CAlarmInManager::DIP_GetInstance()
{
	if (NULL == m_pInstance)
	{
		m_pInstance = new CAlarmInManager;
		if (NULL == m_pInstance)
		{
			TRACE_ERR("[CAlarmInManager:%s]new CAlarmInManager failed\n", __FUNCTION__);
			return NULL;
		}
		else
		{
			m_pInstance->DIP_Init();
		}

	}
	//实例引用计数
	m_iInstance_Cnt++;

	return (CAlarmInManager *) m_pInstance;
}

//##ModelId=49B9FDEF03C8
int CAlarmInManager::DIP_Release()
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


//!\brief 设置报警输入是否使能,用于一键布防/撤防
//!\param int bEnable 1-布防,0-撤防
//!\return 0表示设置成功,-1表示设置失败
int CAlarmInManager::DIP_SetEnable(int bEnable)
{
	m_bEnable = (bEnable != 0);
	return 0;
}

//!\brief 设置报警输入是否使能,用于一键布防/撤防
//!\param int bEnable 1-布防,0-撤防
//!\return 0表示撤防,1表示布防
int CAlarmInManager::DIP_GetEnable()
{
	return m_bEnable;
}


//##ModelId=49AE1547030D
int CAlarmInManager::DIP_SetAlarmCallback(int AlarmType, CBAlert AlarmInCallback)
{
	m_AlarmType = AlarmType;
	m_FxnCallBack = AlarmInCallback;
	return (int) 0;
}

//##ModelId=49B5E6CC0128
int CAlarmInManager::DIP_SetMode(int nMode)
{
	int result;

	if (!m_bInit)
	{
		TRACE("[CAlarmInManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nMode < 0 || nMode > 1)
	{
		TRACE_ERR("[CAlarmInManager:%s]invalid argument (nMode = %d)\n", __FUNCTION__, nMode);
		return -1;
	}
	m_iMode = nMode;
	//写模式配置
	result = pCConfig->SetValue(AlarmInSectionStr, AlarmInModeKeyStr, m_iMode);
	if (result)
	{
		TRACE_ERR("[CAlarmInManager:%s]SetValue Failed\n", __FUNCTION__);
		return result;
	}


	return 0;
}

//##ModelId=49B9FA560251
int CAlarmInManager::DIP_GetMode()
{
	if (!m_bInit)
	{
		TRACE("[CAlarmInManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	return (int) m_iMode;
}

//##ModelId=49B9FA560251
int CAlarmInManager::DIP_GetPortNum()
{
	if (!m_bInit)
	{
		TRACE("[CAlarmInManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	return (int) (m_iEmbAlarmInNum + m_iExtAlarmInNum);
}


//##ModelId=49B5E0B80271
int CAlarmInManager::DIP_GetName(int nPort, char *strName, int StrNameSize, int nLength, char *strPixel, int PixelSize)
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
		TRACE("[CAlarmInManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (NULL == strName)
	{
		TRACE_ERR("[CAlarmInManager:%s]nPort = %d invalid argument (NULL == strName)\n", __FUNCTION__, nPort);
		return -1;
	}

	if (nPort < 0 || nPort >= (m_iEmbAlarmInNum + m_iExtAlarmInNum))
	{
		TRACE_ERR("[CAlarmInManager:%s]nPort = %d -->[0,%d]\n", __FUNCTION__, nPort, m_iExtAlarmInNum + m_iEmbAlarmInNum - 1);
		return -1;
	}
	len = strlen(m_strName[nPort]);

	if (len > nLength)
	{
		TRACE_ERR("[CAlarmInManager:%s]nPort = %d invalid argument (nLength =%d  >= %d)\n", __FUNCTION__, nPort, nLength, len);
		return -1;
	}


	strncpy(strName, m_strName[nPort], StrNameSize - 1);

	snprintf(fileName, sizeof(fileName), "%sAlarmIn_%02d.bmp", FileNameAlarmInPixel, nPort + 1);

#if 0
	fp = fopen(fileName, "rb");
	if (fp == NULL)
	{
		TRACE_ERR("[CAlarmInManager:%s]open file (%s)failed\n", __FUNCTION__, fileName);
		return 0;
	}
	result = fread(&hdr, 1, sizeof (BITMAPFILEHEADER), fp);
	if (result != sizeof (BITMAPFILEHEADER))
	{
		fclose(fp);
		TRACE_ERR("[CAlarmInManager:%s]read bmp header failed->ch=%d\n", __FUNCTION__, nPort);
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
		TRACE_ERR("[CAlarmInManager:%s]read bmp info failed->ch=%d\n", __FUNCTION__, nPort);
		return -1;
	}

	if (24 != SWAP16(hbi.biBitCount))
	{
		fclose(fp);
		TRACE_ERR("[CAlarmInManager:%s]not unsuppored bmp type->ch=%d\n", __FUNCTION__, nPort);
		return -1;
	}

	if (strPixel != NULL)
	{
		//开始填缓冲
		fseek(fp, 0, SEEK_SET);
		if (SWAP32(hdr.bfSize) != fread(strPixel, 1, SWAP32(hdr.bfSize), fp))
		{
			fclose(fp);
			TRACE_ERR("[CAlarmInManager:%s]read bmp data failed ->ch=%d\n", __FUNCTION__, nPort);
			return -1;

		}
	}
	fclose(fp);
	return (int) SWAP32(hdr.bfSize);
#else
	if (strPixel != NULL)
	{
		strncpy(strPixel, fileName, PixelSize - 1);
	}
	return (int) 0;
#endif

}

//##ModelId=49B5E0BF0000
int CAlarmInManager::DIP_SetName(int nPort, char *strName, char *strPixel)
{
	int result;

	char key[100];

	FILE *fp;

	char fileName[255];

//    int width,height,wBytes;
//    int pixelLen;
	char *pdata = NULL;

	struct stat statbuf;

	BITMAPFILEHEADER *hdr;

	BITMAPINFOHEADER *hbi;

	if (!m_bInit)
	{
		TRACE("[CAlarmInManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (NULL == strName)
	{
		TRACE_ERR("[CAlarmInManager:%s]nPort = %d invalid argument (NULL == strName)\n", __FUNCTION__, nPort);
		return -1;
	}
	if (strlen(strName) >= MAX_DIP_NAME_LEN)
	{
		TRACE_ERR("[CAlarmInManager:%s]nPort = %d invalid argument (strlen(strName)=%d  >= %d)\n", __FUNCTION__, nPort, strlen(strName),
				  MAX_DIP_NAME_LEN);
		return -1;
	}
	if (nPort < 0 || nPort >= (m_iEmbAlarmInNum + m_iExtAlarmInNum))
	{
		TRACE_ERR("[CAlarmInManager:%s]nPort = %d -->[0,%d]\n", __FUNCTION__, nPort, m_iExtAlarmInNum + m_iEmbAlarmInNum - 1);
		return -1;
	}
	strncpy(m_strName[nPort], strName, sizeof(m_strName[nPort]) - 1);

	if (NULL != strPixel)
	{
		result = lstat(strPixel, &statbuf);
		if (result)
		{
			TRACE_ERR("[CAlarmInManager:%s]lstat file (%s)failed\n", __FUNCTION__, strPixel);
			return -1;
		}
		if (statbuf.st_size > 1024 * 1024)
		{
			TRACE_ERR("[CAlarmInManager:%s]statbuf.st_size = %lld\n", __FUNCTION__, statbuf.st_size);
			return -1;
		}
		fp = fopen(strPixel, "rb");
		if (fp == NULL)
		{
			TRACE_ERR("[CAlarmInManager:%s]open file (%s)failed\n", __FUNCTION__, strPixel);
			return -1;
		}

		pdata = new char[statbuf.st_size];

		if (pdata == NULL)
		{
			TRACE_ERR("[CAlarmInManager:%s]new char[%lld] failed\n", __FUNCTION__, statbuf.st_size);
			fclose(fp);
			return -1;
		}
		result = fread(pdata, 1, statbuf.st_size, fp);
		if (result != statbuf.st_size)
		{
			TRACE_ERR("[CAlarmInManager:%s]fread failed(%d)\n", __FUNCTION__, result);
			fclose(fp);
			delete []pdata;

			return -1;
		}
		fclose(fp);
		hdr = (BITMAPFILEHEADER *) pdata;
		hbi = (BITMAPINFOHEADER *) (pdata + sizeof (BITMAPFILEHEADER));
		snprintf(fileName, sizeof(fileName), "%sAlarmIn_%02d.bmp", FileNameAlarmInPixel, nPort + 1);
		fp = fopen(fileName, "wb+");
		if (fp == NULL)
		{
			if (pdata)
				delete []pdata;
			pdata = 0;

			TRACE_ERR("[CAlarmInManager:%s]open file (%s)failed\n", __FUNCTION__, fileName);
			return -1;
		}
		if (24 != SWAP16(hbi->biBitCount) || SWAP16(hdr->bfType) != 0x4d42)
		{
			fclose(fp);
			TRACE_ERR("[CAlarmInManager:%s]not unsuppored bmp type(bitcount =%d,bftype=%x)->ch=%d\n", __FUNCTION__, hbi->biBitCount, hdr->bfType,
					  nPort);
			return -1;
		}
		//开始写文件

		if (SWAP32(hdr->bfSize) != fwrite(pdata, 1, SWAP32(hdr->bfSize), fp))
		{
			if (pdata)
				delete []pdata;
			pdata = 0;

			fclose(fp);
			TRACE_ERR("[CAlarmInManager:%s]write bmp data failed ->ch=%d\n", __FUNCTION__, nPort);
			return -1;

		}
		fclose(fp);
	}


	//写模式配置
	snprintf(key, sizeof(key), "AlarmInName_%02d", nPort);
	result = pCConfig->SetValue(AlarmInSectionStr, key, strName);
	if (result)
	{
		if (pdata)
			delete []pdata;
		pdata = 0;

		TRACE_ERR("[CAlarmInManager:%s]SetValue Failed\n", __FUNCTION__);
		return result;
	}

	if (pdata)
		delete []pdata;
	pdata = 0;

	return (int) 0;
}

//##ModelId=49B5CE26002E
int CAlarmInManager::DIP_GetType(int nPort)
{
	if (nPort < 0 || nPort >= (m_iEmbAlarmInNum + m_iExtAlarmInNum))
	{
		TRACE_ERR("[CAlarmInManager:%s]nPort = %d -->[0,%d]\n", __FUNCTION__, nPort, m_iExtAlarmInNum + m_iEmbAlarmInNum - 1);
		return -1;
	}
	return m_bType[nPort];
}

//##ModelId=49B5CCC2031C
int CAlarmInManager::DIP_SetType(int nPort, int nType)
{
	char key[100];

	int result;

	if (!m_bInit)
	{
		TRACE("[CAlarmInManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nPort < 0 || nPort >= (m_iEmbAlarmInNum + m_iExtAlarmInNum))
	{
		TRACE_ERR("[CAlarmInManager:%s]nPort = %d -->[0,%d]\n", __FUNCTION__, nPort, m_iExtAlarmInNum + m_iEmbAlarmInNum - 1);
		return -1;
	}
	if (nType < 0 || nType > 1)
	{
		TRACE_ERR("[CAlarmInManager:%s]invalid argument (nType = %d)\n", __FUNCTION__, nType);
		return -1;
	}
	m_bType[nPort] = nType;

	//写模式配置
	snprintf(key, sizeof(key), "AlarmInType_%02d", nPort);
	result = pCConfig->SetValue(AlarmInSectionStr, key, nType);
	if (result)
	{
		TRACE_ERR("[CAlarmInManager:%s]SetValue Failed\n", __FUNCTION__);
		return result;
	}


	return (int) 0;
}

//!\brief   获取端口输入状态
//!\param   nPort               输出通道号
//!\return  true 有输入，false无输入
int CAlarmInManager::DIP_GetState(int nPort)
{

	if (nPort < 0 || nPort >= (m_iEmbAlarmInNum + m_iExtAlarmInNum))
	{
		TRACE_ERR("[CAlarmInManager:%s]nPort = %d -->[0,%d]\n", __FUNCTION__, nPort, m_iExtAlarmInNum + m_iEmbAlarmInNum - 1);
		return 0;
	}

	return (m_bAlarmInStatus >> nPort) & 1;


}

//!\brief   获取端口输入状态
//!\return  位俺码
int CAlarmInManager::DIP_GetAllState()
{

	return m_bAlarmInStatus;
}

//##ModelId=49B7187602BF
int CAlarmInManager::DIP_GetRecordParam(int nPort, int *dwChannelFlag)
{

	int ret;

	if (!m_bInit)
	{
		TRACE("[CAlarmInManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nPort < 0 || nPort >= (m_iEmbAlarmInNum + m_iExtAlarmInNum))
	{
		TRACE_ERR("[CAlarmInManager:%s]nPort = %d -->[0,%d]\n", __FUNCTION__, nPort, m_iExtAlarmInNum + m_iEmbAlarmInNum - 1);
		return -1;
	}


	ret = m_pActionAlarm[nPort]->GetRecordParam(dwChannelFlag);
	if (ret < 0)
	{
		TRACE_ERR("[CAlarmInManager:%s]nPort = %d -->GetRecordParam return failed\n", __FUNCTION__, nPort);
		return -1;
	}

	return (int) 0;
}

//##ModelId=49B718640232
int CAlarmInManager::DIP_SetRecordParam(int nPort, int dwChannelFlag)
{
	int result;

//    int i;
//    int bEnable;
	if (!m_bInit)
	{
		TRACE("[CAlarmInManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nPort < 0 || nPort >= (m_iEmbAlarmInNum + m_iExtAlarmInNum))
	{
		TRACE_ERR("[CAlarmInManager:%s]nPort = %d -->[0,%d]\n", __FUNCTION__, nPort, m_iExtAlarmInNum + m_iEmbAlarmInNum - 1);
		return -1;
	}

	result = m_pActionAlarm[nPort]->SetRecordParam(dwChannelFlag);
	if (result)
	{
		TRACE_ERR("[CAlarmInManager:%s]nPort = %d SetRecordParam failed \n", __FUNCTION__, nPort);
		return -1;
	}

	return (int) 0;
}

//##ModelId=49B5CE9A030D
int CAlarmInManager::DIP_GetSnapshotParam(int nPort, int *dwChannelFlag)
{

	int ret;

	if (!m_bInit)
	{
		TRACE("[CAlarmInManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nPort < 0 || nPort >= (m_iEmbAlarmInNum + m_iExtAlarmInNum))
	{
		TRACE_ERR("[CAlarmInManager:%s]nPort = %d -->[0,%d]\n", __FUNCTION__, nPort, m_iExtAlarmInNum + m_iEmbAlarmInNum - 1);
		return -1;
	}


	ret = m_pActionAlarm[nPort]->GetSnapshotParam(dwChannelFlag);
	if (ret < 0)
	{
		TRACE_ERR("[CAlarmInManager:%s]nPort = %d -->GetSnapshotParam return failed\n", __FUNCTION__, nPort);
		return -1;
	}

	return (int) 0;
}

//##ModelId=49B5CDFE00BB
int CAlarmInManager::DIP_SetSnapshotParam(int nPort, int dwChannelFlag)
{
	int result;

//    int i;
//    int bEnable;
	if (!m_bInit)
	{
		TRACE("[CAlarmInManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nPort < 0 || nPort >= (m_iEmbAlarmInNum + m_iExtAlarmInNum))
	{
		TRACE_ERR("[CAlarmInManager:%s]nPort = %d -->[0,%d]\n", __FUNCTION__, nPort, m_iExtAlarmInNum + m_iEmbAlarmInNum - 1);
		return -1;
	}

	result = m_pActionAlarm[nPort]->SetSnapshotParam(dwChannelFlag);
	if (result)
	{
		TRACE_ERR("[CAlarmInManager:%s]nPort = %d SetActionParam failed \n", __FUNCTION__, nPort);
		return -1;
	}

	return (int) 0;
}



int CAlarmInManager::DIP_GetPtzParam(int nPort, int *nAction, int nChannel, int *nIndex)
{
	int result = 0;

//    int action;
//    int chFlag;
//    int i;
	if (!m_bInit)
	{
		TRACE("[CAlarmInManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nPort < 0 || nPort >= (m_iEmbAlarmInNum + m_iExtAlarmInNum))
	{
		TRACE_ERR("[CAlarmInManager:%s]nPort = %d -->[0,%d]\n", __FUNCTION__, nPort, m_iExtAlarmInNum + m_iEmbAlarmInNum - 1);
		return -1;
	}

	result = m_pActionAlarm[nPort]->GetPtzParam(nChannel, nAction, nIndex);

	return (int) result;
}

//##ModelId=49B718BD0261
int CAlarmInManager::DIP_SetPtzParam(int nPort, int nAction, int nChannel, int nIndex)
{
	int result = 0;

//    int i_pos;
//    int i;
	if (!m_bInit)
	{
		TRACE("[CAlarmInManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nPort < 0 || nPort >= (m_iEmbAlarmInNum + m_iExtAlarmInNum))
	{
		TRACE_ERR("[CAlarmInManager:%s]nPort = %d -->[0,%d]\n", __FUNCTION__, nPort, m_iExtAlarmInNum + m_iEmbAlarmInNum - 1);
		return -1;
	}

	result = m_pActionAlarm[nPort]->SetPtzParam(nChannel, nAction, nIndex);

	return result;

}



//##ModelId=49B718DD030D
int CAlarmInManager::DIP_GetDopParam(int nPort, int *dwOutPortFlag)
{
	int ret = 0;

	if (!m_bInit)
	{
		TRACE("[CAlarmInManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nPort < 0 || nPort >= (m_iEmbAlarmInNum + m_iExtAlarmInNum))
	{
		TRACE_ERR("[CAlarmInManager:%s]nPort = %d -->[0,%d]\n", __FUNCTION__, nPort, m_iExtAlarmInNum + m_iEmbAlarmInNum - 1);
		return -1;
	}



	ret = m_pActionAlarm[nPort]->GetDopParam(dwOutPortFlag);
	if (ret < 0)
	{
		TRACE_ERR("[CAlarmInManager:%s]nPort = %d -->GetDopParam return failed\n", __FUNCTION__, nPort);
		return -1;
	}



	return (int) 0;
}

//##ModelId=49B718ED00AB
int CAlarmInManager::DIP_SetDopParam(int nPort, int dwOutPortFlag)
{
	int result;

//    int i;
//    int bEnable;
	if (!m_bInit)
	{
		TRACE("[CAlarmInManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nPort < 0 || nPort >= (m_iEmbAlarmInNum + m_iExtAlarmInNum))
	{
		TRACE_ERR("[CAlarmInManager:%s]nPort = %d -->[0,%d]\n", __FUNCTION__, nPort, m_iExtAlarmInNum + m_iEmbAlarmInNum - 1);
		return -1;
	}

	result = m_pActionAlarm[nPort]->SetDopParam(dwOutPortFlag);
	if (result)
	{
		TRACE_ERR("[CAlarmInManager:%s]nPort = %d DIP_SetDopParam failed(%d)\n", __FUNCTION__, nPort, result);
		return result;
	}

	return (int) 0;
}

int CAlarmInManager::SetPushEventIntvCnt(int nPort,int nPushEventIntv)
{
	if (!m_bInit)
	{
		TRACE("[CAlarmInManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nPort < 0 || nPort >= (m_iEmbAlarmInNum + m_iExtAlarmInNum))
	{
		TRACE_ERR("[CAlarmInManager:%s]nPort = %d -->[0,%d]\n", __FUNCTION__, nPort, m_iExtAlarmInNum + m_iEmbAlarmInNum - 1);
		return -1;
	}
	
	return m_pActionAlarm[nPort]->SetPushEventIntvCnt(m_AlarmType, nPushEventIntv);
}

int CAlarmInManager::GetPushEventIntvCnt(int nPort,int *nPushEventIntv)
{
	if (!m_bInit)
	{
		TRACE("[CAlarmInManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nPort < 0 || nPort >= (m_iEmbAlarmInNum + m_iExtAlarmInNum))
	{
		TRACE_ERR("[CAlarmInManager:%s]nPort = %d -->[0,%d]\n", __FUNCTION__, nPort, m_iExtAlarmInNum + m_iEmbAlarmInNum - 1);
		return -1;
	}
	
	return m_pActionAlarm[nPort]->GetPushEventIntvCnt(m_AlarmType, nPushEventIntv);
}

//##ModelId=49B5CE7002FD
int CAlarmInManager::DIP_GetSchedule(int nPort, AlarmInSchedule * pAlarmInSchedule)
{
	if (!m_bInit)
	{
		TRACE("[CAlarmInManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (NULL == pAlarmInSchedule)
	{
		TRACE_ERR("[CAlarmInManager:%s]nPort = %d invalid argument (NULL == pAlarmInSchedule)\n", __FUNCTION__, nPort);
		return -1;
	}
	if (nPort < 0 || nPort >= (m_iEmbAlarmInNum + m_iExtAlarmInNum))
	{
		TRACE_ERR("[CAlarmInManager:%s]nPort = %d -->[0,%d]\n", __FUNCTION__, nPort, m_iExtAlarmInNum + m_iEmbAlarmInNum - 1);
		return -1;
	}
	memcpy(pAlarmInSchedule, &m_ptAlarmInSchedule[nPort], sizeof (AlarmInSchedule));
	return (int) 0;
}

//##ModelId=49B5CD6201A5
int CAlarmInManager::DIP_SetSchedule(int nPort, AlarmInSchedule * pAlarmInSchedule)
{
	char value[255];

	char key_str[30];

	int i, j;

	int result;

	if (!m_bInit)
	{
		TRACE("[CAlarmInManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (NULL == pAlarmInSchedule)
	{
		TRACE_ERR("[CAlarmInManager:%s]nPort = %d invalid argument (NULL == pAlarmInSchedule)\n", __FUNCTION__, nPort);
		return -1;
	}
	if (nPort < 0 || nPort >= (m_iEmbAlarmInNum + m_iExtAlarmInNum))
	{
		TRACE_ERR("[CAlarmInManager:%s]nPort = %d -->[0,%d]\n", __FUNCTION__, nPort, m_iExtAlarmInNum + m_iEmbAlarmInNum - 1);
		return -1;
	}
	//应用计划
	memcpy(&m_ptAlarmInSchedule[nPort], pAlarmInSchedule, sizeof (AlarmInSchedule));

	//计划--
	//ScheduleCfgEnable[ch]=[启用]
	//ScheduleCfg[ch]_[星期]=[48]--[启用][星期][半小时]
	snprintf(key_str, sizeof(key_str), "AIScheduleCfgEnable%02d", nPort);
	result = pCConfig->SetValue(AlarmInSectionStr, key_str, pAlarmInSchedule->bytState);
	if (result)
	{
		TRACE_ERR("[CAlarmInManager:%s]AIScheduleCfgEnable SetValue Failed (%d)\n", __FUNCTION__, result);
		return result;
	}
	for (i = 0; i < 7; i++)
	{
		for (j = 0; j < 48; j++)
		{
			value[j] = pAlarmInSchedule->dwTimeSeg[i][j] + '0';
		}
		value[j] = 0;
		snprintf(key_str, sizeof(key_str), "AIScheduleCfg%02d_%01d", nPort, i);

		result = pCConfig->SetValue(AlarmInSectionStr, key_str, value);
		if (result)
		{
			TRACE_ERR("[CAlarmInManager:%s]AIScheduleCfg SetValue Failed (%d)\n", __FUNCTION__, result);
			return result;
		}
		TRACE("[CAlarmInManager:%s]pAlarmInSchedule->bytState=%d,Write:%s\n", __FUNCTION__, pAlarmInSchedule->bytState, value);
	}

	return (int) 0;
}

//##ModelId=49B5CE9202AF
int CAlarmInManager::DIP_GetEnableAction(int nPort, int action)
{
	int enable;

	if (!m_bInit)
	{
		TRACE("[CAlarmInManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nPort < 0 || nPort >= (m_iEmbAlarmInNum + m_iExtAlarmInNum))
	{
		TRACE_ERR("[CAlarmInManager:%s]nPort = %d -->[0,%d]\n", __FUNCTION__, nPort, m_iExtAlarmInNum + m_iEmbAlarmInNum - 1);
		return -1;
	}
	enable = m_pActionAlarm[nPort]->IsEnableAction(action);

	return enable;

}

//##ModelId=49B5CD810222
int CAlarmInManager::DIP_SetEnableAction(int nPort, int action, int bEnable)
{
	if (!m_bInit)
	{
		TRACE("[CAlarmInManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nPort < 0 || nPort >= (m_iEmbAlarmInNum + m_iExtAlarmInNum))
	{
		TRACE_ERR("[CAlarmInManager:%s]nPort = %d -->[0,%d]\n", __FUNCTION__, nPort, m_iExtAlarmInNum + m_iEmbAlarmInNum - 1);
		return -1;
	}

	return m_pActionAlarm[nPort]->EnableAction(action, bEnable);

}

int CAlarmInManager::DIP_GetAlarmInAudioAlarmSchedule(int nPort, tagAudioAlarmSchedule *stSchedule)
{
	if (!m_bInit)
	{
		TRACE("[CAlarmInManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nPort < 0 || nPort >= (m_iEmbAlarmInNum + m_iExtAlarmInNum))
	{
		TRACE_ERR("[CAlarmInManager:%s]nPort = %d -->[0,%d]\n", __FUNCTION__, nPort, m_iExtAlarmInNum + m_iEmbAlarmInNum - 1);
		return -1;
	}

	if (NULL == stSchedule)
	{
		TRACE_ERR("[CAlarmInManager:%s]cfg==NULL\n", __FUNCTION__);
		return -1;
	}
	
	memcpy(stSchedule, &m_AudioAlarmScheduleCfg[nPort], sizeof (tagAudioAlarmSchedule));
	
	return 0;
}

int CAlarmInManager::DIP_SetAlarmInAudioAlarmSchedule(int nPort, tagAudioAlarmSchedule *stSchedule)
{
	char value[64] = {0};
	char key_str[64] = {0};
	int i, j;
	
	if (!m_bInit)
	{
		TRACE("[CAlarmInManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nPort < 0 || nPort >= (m_iEmbAlarmInNum + m_iExtAlarmInNum))
	{
		TRACE_ERR("[CAlarmInManager:%s]nPort = %d -->[0,%d]\n", __FUNCTION__, nPort, m_iExtAlarmInNum + m_iEmbAlarmInNum - 1);
		return -1;
	}

	if (NULL == stSchedule)
	{
		TRACE_ERR("[CAlarmInManager:%s]cfg==NULL\n", __FUNCTION__);
		return -1;
	}
	
	if (stSchedule->bytState > 2)
	{
		TRACE_ERR("[CAlarmInManager:%s]cfg->bytState=%d\n", __FUNCTION__, stSchedule->bytState);
		return -1;
	}

	//ScheduleCfgEnable[ch]=[启用]
	//ScheduleCfg[ch]_[星期]=[48]--[启用][星期][半小时]
	snprintf(key_str, sizeof(key_str), "AudioAlarmScheduleCfgEnable%02d", nPort);
	pCConfig->SetValue(AlarmInSectionStr, key_str, stSchedule->bytState);
	snprintf(key_str, sizeof(key_str), "AudioAlarmScheduleCfgEnableDN%02d", nPort);
	pCConfig->SetValue(AlarmInSectionStr, key_str, stSchedule->nDayMode);

	for (i = 0; i < 7; i++)
	{
		for (j = 0; j < 48; j++)
		{
			value[j] = stSchedule->dwTimeSeg[i][j] + '0';
		}
		value[j] = 0;
		snprintf(key_str, sizeof(key_str), "AudioAlarmScheduleCfg%02d_%01d", nPort, i);

		pCConfig->SetValue(AlarmInSectionStr, key_str, value);
		TRACE("[CAlarmInManager:%s]cfg->bytState=%d,Write:%s\n", __FUNCTION__, stSchedule->bytState, value);
	}
	
	//更新计划
	memcpy(&m_AudioAlarmScheduleCfg[nPort], stSchedule, sizeof (tagAudioAlarmSchedule));

	return 0;

}

int CAlarmInManager::DIP_GetAlarmInAudioAlarmCfg(int nPort, int *nVoiceType, int *nBroadcastCount)
{
	*nVoiceType = 0;
	*nBroadcastCount = 0;
	return 0;
}

int CAlarmInManager::DIP_SetAlarmInAudioAlarmCfg(int nPort, int nVoiceType, int nBroadcastCount)
{
	return 0;
}

int CAlarmInManager::DIP_CheckAudioAlarmInSchedule(int nPort)
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
		TRACE_ERR("ZWsys_EncGetDayNightStatus failed! ret=%x\n", sdkRet);
		return 0;
	}

	if (1 == m_AudioAlarmScheduleCfg[nPort].nDayMode)//白天
	{
		if (1 == dayNightStatus)
		{
			nResult = 1;
		}
	}
	else if (0 == m_AudioAlarmScheduleCfg[nPort].nDayMode)//夜晚
	{
		if (0 == dayNightStatus)
		{
			nResult = 1;
		}
	}
	else if (2 == m_AudioAlarmScheduleCfg[nPort].nDayMode && 0 == m_AudioAlarmScheduleCfg[nPort].bytState)//全天
	{
		nResult = 1;
	}
	else if (1 == m_AudioAlarmScheduleCfg[nPort].bytState)//自定义
	{
		//获取当前时间段信息
		curr_time = time(NULL);
		localtime_r(&curr_time, &curr_tm);
		day = curr_tm.tm_wday;
		seg = (curr_tm.tm_hour << 1) + (curr_tm.tm_min >= 30);
		TRACE("wday=%d hour=%d min=%d---\n", day, curr_tm.tm_hour, curr_tm.tm_min);
		
		if (m_AudioAlarmScheduleCfg[nPort].dwTimeSeg[day][seg])
		{
			nResult = 1;
		}
	}

	return nResult;
}

int CAlarmInManager::CheckSchedule(int nPort)
{
	int day;

	int seg;

	time_t curr_time;

	struct tm curr_tm;

	//   int result =0;

	if (!m_bEnable)
	{							//撤防
		return 0;
	}
	//一般模式下总是计划中
	if (!m_iMode)
	{
		return 1;
	}
	//获取当前时间段信息
	curr_time = time(NULL);
	localtime_r(&curr_time, &curr_tm);
	day = curr_tm.tm_wday;
	seg = (curr_tm.tm_hour << 1) + (curr_tm.tm_min >= 30);

	//计划检测
	if (m_ptAlarmInSchedule[nPort].bytState == 1)
	{							//计划启用且为高级模式
		if (m_ptAlarmInSchedule[nPort].dwTimeSeg[day][seg])
		{						//布防
			return 1;
		}
	}
	else
	{
		return 1;
	}

	return 0;
}

int CAlarmInManager::CheckStaus()
{
	int i;

	int bstat;

	//   int day;
//    int seg;
//    time_t curr_time;
//    struct tm curr_tm;
	int result = 0;

	int AI_status;

	if (!m_bInit)
	{
		TRACE_WARN("not inited\n");
		return 0;
	}
	if (m_bCheck_exit)
	{
		return -1;
	}

	//result = MP4Sys_AlarmIn();
	ZWSys_AlarmIn(&result);
	AI_status = 0;


	for (i = 0; i < m_iEmbAlarmInNum + m_iExtAlarmInNum; i++)
	{
		//检测AlarmIn状态
		if (i < m_iEmbAlarmInNum)
		{

			bstat = (result >> i) & 1;

		}
		else
		{
			bstat = theCExtAlarmIn->GetAlarmInState(i - m_iEmbAlarmInNum);

		}
		//检查类型 0常开 1常闭
		//   TRACE("[%s] port =%d bstat=%d m_btype =%d\n",__FUNCTION__,i,bstat,m_bType[i]);
		bstat ^= m_bType[i];



		if (!bstat)
		{						//无输入
			continue;
		}
		//检查计划
		if (!CheckSchedule(i))
		{						//不布防
			continue;

		}
		AI_status |= (bstat << i);

		m_pActionAlarm[i]->CheckAction(0,m_AlarmType, m_FxnCallBack, 0);

	}
	m_bAlarmInStatus = AI_status;
	return 0;

}

static void *Fnx_CheckThread(void *lParam)
{
    PRT_TID_NAME("AlarmInCheckThread");

	int ret = 0;

	CAlarmInManager *pcAIM = (CAlarmInManager *) lParam;
	if (NULL == pcAIM)
	{
		TRACE_ERR("[CAlarmInManager:%s]check thread failed (NULL==pcAIM)\n", __FUNCTION__);
		return NULL;
	}
	while (1)
	{
		ret = pcAIM->CheckStaus();
		if (ret < 0)			//线程退出
		{
			TRACE_ERR("[CAlarmInManager:%s] check thread quit\n", __FUNCTION__);
			break;
		}
		sleep(1);				//一秒检测一次
	}
	return NULL;
}
