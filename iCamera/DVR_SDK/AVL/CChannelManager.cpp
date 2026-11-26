// Copyright (C) 1991 - 1999 Rational Software Corporation
//#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <semaphore.h>
#include <ctype.h>
#include <errno.h>

#include "CAreaDefense.h"
#include "CMotion.h"
#include "CHumanShape.h"
#include "COptics.h"
#include "CRecord.h"
#include "COsd.h"
#include "CCoder.h"
#include "CCapturePic.h"
#include "RECT.h"
#include "CChannelManager.h"
#include "pthread.h"
#include "CSysManager.h"
#include "mysystem.h"
#include "DeviceAbility.h"
#ifdef DO_SATA
#include "hisdk_sata.h"
#else
//#include "hisdk.h"
#include "zwsdk.h"
#endif
#include "ddt_so.h"
#include "AVL_global.h"
#include "common_pub.h"
//#include "NET/INet.h"
#include "STR/CDiskManager.h"
#include "STR/tagDiskInfo.h"
#include "common_pub.h"
#include "mauth.h"
//#define  MAIN_AUX_SHOW
#include "../PAR/CPubFunc.h"

int bSupportRtmp = 0;

#define SWAP16(x) (test_lite_big()?(x):(((((x)&0xff)<<8)|(((x)>>8)&0xff))))
#define SWAP32(x)  (test_lite_big()?(x):(((SWAP16((x)&0xffff)<<16)|(SWAP16((x)>>16)))))

static unsigned int GetIntSerialNum(const unsigned char *szSerialNum)
{
    if (NULL == szSerialNum)
    {
        return -1;
    }

    int i = 0;
    unsigned int result = 0;
    char buf[32] = {0};

    /* 以字母(非A~F)开头的字符串 */
    if ((0 == isdigit(szSerialNum[0]))
    && ((szSerialNum[0]>'F') || (szSerialNum[0]>'f')))
    {
        int j = 0;
        int len = strlen((const char *)szSerialNum);
        for (i=len-8; i<=len; i++, j++)
        {
            buf[j] = szSerialNum[i];
        }
    }
    else
    {
        for (i=0; i<8; i++)
        {
            buf[i] = szSerialNum[i];
        }
    }
    
    /* 转化为十六进制 */
    sscanf(buf, "%x", &result);

    return result;
}


typedef struct __AVL_FILEHEADER__
{
    unsigned int HeaderMARK0;		  //固定为0xB6010000
    unsigned int HeaderMARK1;					//固定为0x34363248,即H264
    unsigned int FileHeadLen;				//媒体文件头的长度(包含本身4Byte)
    unsigned char Version;					//媒体文件头的版本信息.初始版本为1
    /*
	2Bytes, 表示媒体数据包及媒体索引信息
	MSB B15 		  0:没有索引 1:有索引
		B1			  0:no Private Data 1:with Private Data
		B0			  0: Video Only 1:Video&Audio
	*/
    short int 	DataFlag;         
    short int   Width;			//视频媒体数据的图像宽度
    short int   Height;			//视频媒体数据的图像高度
} __attribute__ ((packed)) AVL_FILEHEADER;

#define FILEHEADERLEN		(sizeof(AVL_FILEHEADER))


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

//0 - 没有数据上来，1-网络阻塞，2-网络阻塞，3-存储阻塞，4-存储阻塞，5-不可能的
static const char avl_error_str[][20] =
	{ "sdk no data up", "net error", "net error", "store error", "store error", "impossible", "impossible", "impossible" };

//码流统计时间间隔
//static ZW_RTMP_CLIENT_CONNECT_PARAM GLClientConnectParam[5][DVR_MAX_VIDEOIN_NUM]={0};

static void *Fnx_CheckWatchThread(void *data);

static void *Fnx_CheckScheduleThreadV2(void *data);

static void *Fnx_CheckThread(void *data);

static void *Fnx_CheckThread_capture(void *data);

#if defined  __USE_RTMP__
static void *Creart_channel_thread(void *param);
#endif

CChannelManager *CChannelManager::pCChannelManger = NULL;

int CChannelManager::m_iInstanceCnt = 0;

static pthread_mutex_t InitLock;

//static time_t time_begin = 0;
static const char *s_ActiveCodeSec = "AuthorizationInfo";
static const char *s_ActiveCodeKey = "ActiveCode";

#ifdef __USE_RTMP__


static pthread_mutex_t DLock = PTHREAD_MUTEX_INITIALIZER;
static sem_t Rtmpchannelmanager;
static unsigned int Dthread_num =0;
static unsigned int Cthread_num =0;

static ZW_RTMP_CLIENT_CONNECT_PARAM gConnectParam[DVR_MAX_STREAM_NUM][DVR_MAX_VIDEOIN_NUM];
static char streamtype[][5]={"main","sub1","sub2","sub3","sub4","sub5"};

void Working_Status_callback(unsigned int handle, eZWRTMP_STATUS Status, unsigned long dwData, unsigned long dwUserData)
{
	int i=0;
	int j=0;
	CChannelManager* pInstance = (CChannelManager*)dwUserData;
	
	for(i=0;i<DVR_MAX_STREAM_NUM;i++)
	{
		for(j=0;j<MAX_CH_MANAGER_NUM;j++)
		{
			if(handle == pInstance->RtmpHandle[i][j])
			{
				pInstance->RtmpStatus[i][j] = Status;
	//			AVL_INFO("=============pInstance->RtmpStatus[%d][%d] = %d=======================\n",i,j,pInstance->RtmpStatus[i][j]);
			}
		}
	}

	return ;
}

static void *Creart_channel_thread(void *param)
{
    PRT_TID_NAME("CC_Creart_channel_thread");

	int i;
	int j;
	int nRet = 0;
	CChannelManager* pInstance = (CChannelManager*)param;
	unsigned char szSerialNum[32] = {0};
	unsigned int uptime = 0;
	int iExit = 0;

	pthread_detach(pthread_self());
	
	//获取序列号
	ZWSys_GetSerialNum(szSerialNum);

//	sem_wait(&Rtmpchannelmanager);
	if(pInstance->ActionCodeOk == 1)	//已经激活，则创建通道
	{	
		AVL_INFO("ActiveCode is right and creat channal\n");

		while(uptime<60)		//Wait for systme startup
		{
			AVL_INFO("Just for Test!\n");
			uptime++;
			sleep(1);
		}

		while (iExit == 0)
		{
			if (strlen(pInstance->szPushURLServer) > 0)
				break;

			for(i=0;i<DVR_MAX_STREAM_NUM;i++)
			{
				for(j=0;j<MAX_CH_MANAGER_NUM;j++)
				{
					if(pInstance->Veable[i][j] == 1)
					{
						if(1 == pInstance->PushAddressSupport)
						{
							if (0 == pInstance->PushAddressEnable)
							{
								if (strlen(gConnectParam[i][j].szStreamPullingAddress) > 0)
								{
									iExit = 1;
									break;
								}
							}
						}								
					}
				}
			}
			sleep(1);
			
			AVL_INFO("Just for Test zview!\n");
		}

		//批量创建或批量销毁
		for(i=0;i<DVR_MAX_STREAM_NUM;i++)
		{
			for(j=0;j<MAX_CH_MANAGER_NUM;j++)
			{
				if((pInstance->Veable[i][j] == 1)&&(pInstance->RtmpHandle[i][j] < 0))
				{
					if(1 == pInstance->PushAddressSupport)
					{
						if(1 == pInstance->PushAddressEnable)
						{			
							pInstance->ClientConnectParam[i][j].bPushStreamURLRetrieveFromServer =1;
							strncpy(pInstance->ClientConnectParam[i][j].szCustomerServer,pInstance->szPushURLServer,sizeof(pInstance->ClientConnectParam[i][j].szCustomerServer) - 1);
							strncpy(pInstance->ClientConnectParam[i][j].szDeviceSerialID,szSerialNum,sizeof(pInstance->ClientConnectParam[i][j].szDeviceSerialID) - 1);
						}
						else
						{
							pInstance->ClientConnectParam[i][j].bPushStreamURLRetrieveFromServer =0;
							strncpy(pInstance->ClientConnectParam[i][j].szStreamPullingAddress,gConnectParam[i][j].szStreamPullingAddress,sizeof(pInstance->ClientConnectParam[i][j].szStreamPullingAddress) - 1);
						}
					}		
					AVL_INFO("+++++Form+Init+++++ZW_RTMP_Client_Create [%d] [%d]++++++++++\n",i,j);
					nRet = ZW_RTMP_Client_Create((unsigned int *)&pInstance->RtmpHandle[i][j],&pInstance->ClientConnectParam[i][j],\
							Working_Status_callback, pInstance,streamtype[i]);
					
					AVL_INFO("+++++Form+Init+++++ZW_RTMP_Client_Create [%d] [%d] end++++++++++\n",i,j);
				}
			}
		}
	}

	pInstance->RtmpInit = 1;
}

static void *Recreate_channel_thread(void *param)
{
    PRT_TID_NAME("CC_Recreate_channel_thread");

	int i = 0;
	int j = 0;
	CChannelManager* pInstance = (CChannelManager*)param;
    unsigned char szSerialNum[32] = {0};
	//获取序列号
	ZWSys_GetSerialNum(&szSerialNum);

	pthread_detach(pthread_self());

	if(Dthread_num<=5)
	{
		Dthread_num++;	
		XTRACE_MUTEX_LOCK(&DLock);

		//先销毁，有参数变化的通道
		for(i=0;i<DVR_MAX_STREAM_NUM;i++)
		{
			for(j=0;j<MAX_CH_MANAGER_NUM;j++)
			{	
				//仅销毁参数有变化的
				if((pInstance->RtmpHandle[i][j] >= 0)&&(1 == pInstance->ChangeFlag[i][j]))	//只会修改参数有变化且已经创建的通道
				{
					ZW_RTMP_Client_Destory(pInstance->RtmpHandle[i][j]);					
					AVL_INFO("++++++++++++++ZW_RTMP_Client_Destory[%d][%d] end\n",i,j);
					pInstance->RtmpHandle[i][j] = -1;				//句柄销毁
					pInstance->RtmpStatus[i][j] = eZWRTMP_STATUS_NotOpen; //状态修改(由于销毁接口并不会修改状态，这里手动修改)
					pInstance->ChangeFlag[i][j] = 0;				//修改标记清零
				}					
			}
		}

		if(pInstance->ActionCodeOk == 1)	//已经激活，则创建通道	
		{	
			AVL_INFO("ActiveCode is right and creat channal\n");
			//批量创建
			for(i=0;i<DVR_MAX_STREAM_NUM;i++)
			{
				for(j=0;j<MAX_CH_MANAGER_NUM;j++)
				{
					if((pInstance->Veable[i][j] == 1)&&(pInstance->RtmpHandle[i][j] < 0))
					{
						if(1 == pInstance->PushAddressSupport)
						{
							if(1 == pInstance->PushAddressEnable)
							{		
								AVL_INFO("++++++++++++++Auto pulling[%d][%d]\n",i,j);
								pInstance->ClientConnectParam[i][j].bPushStreamURLRetrieveFromServer =1;
								strncpy(pInstance->ClientConnectParam[i][j].szCustomerServer,pInstance->szPushURLServer,sizeof(pInstance->ClientConnectParam[i][j].szCustomerServer) - 1);
								strncpy(pInstance->ClientConnectParam[i][j].szDeviceSerialID,szSerialNum,sizeof(pInstance->ClientConnectParam[i][j].szDeviceSerialID) - 1);
							}
							else
							{
								AVL_INFO("++++++++++++++manual pulling[%d][%d]\n",i,j);
								pInstance->ClientConnectParam[i][j].bPushStreamURLRetrieveFromServer =0;
								strncpy(pInstance->ClientConnectParam[i][j].szStreamPullingAddress,gConnectParam[i][j].szStreamPullingAddress,sizeof(pInstance->ClientConnectParam[i][j].szStreamPullingAddress) - 1);
							}
						}		

						ZW_RTMP_Client_Create((unsigned int *)&pInstance->RtmpHandle[i][j],&pInstance->ClientConnectParam[i][j],\
								Working_Status_callback, pInstance,streamtype[i]);
						
						AVL_INFO("+++++Form+Init+++++ZW_RTMP_Client_Create [%d] [%d] end++++++++++\n",i,j);
					}				
				}
			}
		}
		else
		{
			for(i=0;i<DVR_MAX_STREAM_NUM;i++)
			{
				for(j=0;j<MAX_CH_MANAGER_NUM;j++)
				{	
					//仅销毁参数有变化的
					if((pInstance->RtmpHandle[i][j] >= 0))	//只会修改参数有变化且已经创建的通道
					{
						AVL_INFO("Not Acticed[%d][%d]\n",i,j);
						ZW_RTMP_Client_Destory(pInstance->RtmpHandle[i][j]);
						pInstance->RtmpHandle[i][j] = -1;				//句柄销毁
						pInstance->RtmpStatus[i][j] = eZWRTMP_STATUS_NotActived; //状态修改(由于销毁接口并不会修改状态，这里手动修改)
					}					
				}
			}	
		}
		
	//	sem_post(&Rtmpchannelmanager);

		Dthread_num--;

		AVL_INFO("process rebuild channel!\n");
		XTRACE_MUTEX_UNLOCK(&DLock);
	}
	else
	{
		AVL_INFO("Please Wait for a moment!\n");
	}
}

#ifdef __cplusplus
extern "C" {
#endif 
int PI31415926(char* szActiveCode, unsigned int dwSN, unsigned int dwBoardID, unsigned int *dwCapability);

#ifdef __cplusplus
}
#endif

#endif

CChannelManager::CChannelManager()
{
	int i =0;
	int j =0;

	for (i = 0; i < MAX_CH_MANAGER_NUM; i++)
    {
        theCMotion[i] = NULL;
        theCHumanShape[i] = NULL;
        theCSoundDetect[i] = NULL;
		theCOptics[i] = NULL;
		theCRecord[i] = NULL;
		theCOsd[i]    = NULL;
		theCCoder[i]  = NULL;
		theCCapturePic[i] = NULL;
		theCoderHandle[i] = NULL;
		m_uiRealBits[i] = 0;

		m_ullRealBits_Stat[i] = 0;
		
		m_iLastTime[i] = 0;
		m_uiAuxRealBits[i] = 0;
		m_ullAuxRealBits_Stat[i] = 0;		
		m_iAuxLastTime[i] = 0;
#ifdef USE_HDAUX
		m_uiHDAuxRealBits[i] = 0;
		m_ullAuxRealBits_Stat[i] = 0;
		m_iHDAuxLastTime[i] = 0;
#endif
		m_uiFourthRealBits[i] = 0;
		m_uiFourthRealBits_Stat[i] = 0;
		m_iFourthLastTime[i] = 0;
		
		m_iAV_status[i] = 0;	//完毕

		for(j=0;j<DVR_MAX_STREAM_NUM;j++)
		{
			RtmpHandle[j][i] = -1;
			Veable[j][i] = 0;
			ChangeFlag[j][i]=0; 
		}
		RtmpInit = 0;

		PushAddressSupport = 0;
		PushAddressEnable = 0;
		memset(ClientConnectParam,0,sizeof(ClientConnectParam));
		memset(szPushURLServer,0, sizeof(szPushURLServer));
#if defined  (__USE_RTMP__)
		sem_init(&Rtmpchannelmanager,0,0);
		XTRACE_MUTEX_INIT(&DLock, NULL);
#endif
		m_uiFourthRealBits[i] = 0;
		m_uiFourthRealBits_Stat[i] = 0;
        m_iFourthLastTime[i] = 0;
    }

    m_iLength = 0;
    m_iEncChannelCnt = 0;
    m_FnxAvCallBack = NULL;

	m_hWatchThread = 0;
	m_hCheckThread = 0;
	m_hCheckScheduleThread = 0;
	m_hCheckScheduleThreadV2 = 0;
	m_hCheckThread_capture = 0;
	m_bInit = 0;
	m_cInit = 0;
    m_bDorEnable = 0;
}

CChannelManager::~CChannelManager()
{
	UnInit();
}

#ifdef __cplusplus
extern "C" {
#endif 
int PI31415926(char* szActiveCode, unsigned int dwSN, unsigned int dwBoardID, unsigned int *dwCapability);

#ifdef __cplusplus
}
#endif

static int IsDecimale(float v)
{
	int a = ((int)v)*100;
	int b = (int)(v*100);
	int rc = 0;
	if (a == b)
	{
		rc = 1;
	}

	return rc;
}

static int pnpoly(float x, float y, POLYGON_FLOAT *reg)
{
    int i, j,c = 0;
    int len = reg->nValidNum;
    for (i = 0, j = (len-1); i < len; j = i++) 
    {
        float b2 = (reg->point[j].x-reg->point[i].x) * (y-reg->point[i].y) / (reg->point[j].y-reg->point[i].y) + reg->point[i].x;
        if ( ((y < reg->point[i].y) != (y < reg->point[j].y)) && (x < b2))
        {
            c = !c;
        }
    }
    return c;
}

static int transPolygonToBlock(struct tagPolygon *pPoly, unsigned char *pBlock, int size)
{
	if ((NULL == pPoly) || (NULL == pBlock) || (size != 12*16))
	{
		return -1;
	}

	// 获取最大矩形范围
	int i, j = 0;
	POLYGON_FLOAT pg = {0};
	pg.nValidNum = pPoly->size;

	for (i = 0; i < pg.nValidNum; i++)
	{
		pg.point[i].x = (pPoly->point[i].x * 16 * (1.0)) / 10000;
		pg.point[i].y = (pPoly->point[i].y * 12 * (1.0)) / 10000;
	}

	float x_min, x_max = 0.0;
	float y_min, y_max = 0.0;

	x_min = x_max = pg.point[0].x;
	y_min = y_max = pg.point[0].y;

	for (j = 1; j < pg.nValidNum; j++) {
		if (pg.point[j].x < x_min)
		{
			x_min = pg.point[j].x;
		}
		if (pg.point[j].x > x_max)
		{
			x_max = pg.point[j].x;
		}
		if (pg.point[j].y < y_min)
		{
			y_min = pg.point[j].y;
		}
		if (pg.point[j].y > y_max)
		{
			y_max = pg.point[j].y;
		}
	}

	int left, top, right, bottom = 0;
	
	left = (int)x_min;
	top = (int)y_min;
	if (IsDecimale(x_max)) {
		right = (int)x_max; 
	} else {
		right = ((int)x_max) + 1;
	}
	if (IsDecimale(y_max)) {
		bottom = (int)y_max; 
	} else {
		bottom = ((int)y_max) + 1;
	}

	memset(pBlock, 0, size);
	float cur_x, cur_y = 0.0;
	int idx = 0;

	for (j = top; j < bottom; j++)
	{
		idx = j * 16;
		for (int k = left; k < right; k++)
		{
			// 计算该点是否在规则区域内
			cur_x = k + 0.5;
			cur_y = j + 0.5;
			if (1 == pnpoly(cur_x, cur_y, &pg))
			{
				pBlock[idx+k] = 1;
			}
		} 
	}

	return 0;
}

//##ModelId=49BF1B46026D
int CChannelManager::Init()
{
	XTRACE_MUTEX_LOCK(&InitLock);
	int i,j, ret = 0;
	int n = 0;
	int nBoardID = 0;
	int nCount = 0;
	int nIntervalTime = 0;
	int nFormat = 0;
	char nStreamType = 0;
	int nWidth = 0, nHeight = 0; 
	int nQuality = 0;
    FILE *fp = NULL;
    unsigned char szSerialNum[32] = {0};
	int ActiveStatus = 1;
	unsigned int dwCapability = 0;

	if (m_bInit)
	{
		AVL_DBG("[CChannelManager:%s]already inited\n", __FUNCTION__);
		XTRACE_MUTEX_UNLOCK(&InitLock);
		return 0;
	}

	if (m_cInit)
	{
		AVL_DBG("[CChannelManager:%s]already StreamReadCallback inited\n", __FUNCTION__);
		XTRACE_MUTEX_UNLOCK(&InitLock);
		return 0;
	}

#if 0
	//初始化系统SDK
	//if (MP4Sys_InitDSPs() < 0)
	if ( ZWSys_Init() < 0)
	{
		AVL_ERR("[CChannelManager:%s]can't initial DSP\n", __FUNCTION__);
		XTRACE_MUTEX_UNLOCK(&InitLock);
		return -1;
	}
#endif

	//获取通道数
	m_iEncChannelCnt = ZWSys_GetTotalEncChannels() ;
	if (m_iEncChannelCnt <= 0)
	{
		AVL_ERR("[CChannelManager]SDK not init\n");
		XTRACE_MUTEX_UNLOCK(&InitLock);
		UnInit();
		return -1;
	}

	if (m_iEncChannelCnt > MAX_CH_MANAGER_NUM)
	{
		AVL_ERR("[CChannelManager]m_iEncChannelCnt =%d > MAX_CH_MANAGER_NUM(%d)\n", m_iEncChannelCnt, MAX_CH_MANAGER_NUM);
		XTRACE_MUTEX_UNLOCK(&InitLock);
		UnInit();
		return -1;
	}

	DeviceDescription dds;

	//置启动流回调初始化
	m_cInit = 1;

	//设置回调函数
	ZWSys_EncRegisterStreamReadCallback(AvCallBack_FNX);

	ZWSys_GetBoardInfo(&nBoardID);
	ret = EDVRS_DDT_GetDeviceDescriptionByID(&dds, nBoardID);
	for (i = 0; i < m_iEncChannelCnt; i++)
	{
		theCoderHandle[i] = ZWSys_EncChannelOpen(i);
		AVL_ERR("[CChannelManager]theCoderHandle[%d]=%p \n", i, theCoderHandle[i]);
		theCAreaDefense[i] = new CAreaDefense(i, theCoderHandle[i]);
		theCMotion[i] = new CMotion(i, theCoderHandle[i]);
		theCHumanShape[i] = new CHumanShape(i, theCoderHandle[i]);
        theCSoundDetect[i] = new CSoundDetectManager(i, theCoderHandle[i]);
        theCOptics[i] = new COptics(i, theCoderHandle[i]);

        theCOsd[i] = new COsd(i, theCoderHandle[i]);
        theCCoder[i] = new CCoder(i, theCoderHandle[i]);
        theCCapturePic[i] = new CCapturePic(i, theCoderHandle[i]);
		theCRecord[i] = new CRecord(i, theCoderHandle[i]);

		if (NULL == theCMotion[i]
		 || NULL == theCHumanShape[i]
		 || NULL == theCSoundDetect[i]
		 || NULL == theCOptics[i] 
		 || NULL == theCRecord[i]
		 || NULL == theCOsd[i]
		 || NULL == theCCoder[i])
		{
			ret = -1;
			AVL_ERR("[CChannelManager] init failed \n");
			XTRACE_MUTEX_UNLOCK(&InitLock);
			UnInit();
			return ret;
		}

		ret |= theCRecord[i]->Init();	
		ret |= theCOsd[i]->Init();
		ret |= theCAreaDefense[i]->Init(dds.bytVideoInChannels, dds.bytAlarmOutChannels);
		ret |= theCMotion[i]->Init(dds.bytVideoInChannels, dds.bytAlarmOutChannels);
		ret |= theCHumanShape[i]->Init(dds.bytVideoInChannels, dds.bytAlarmOutChannels);
        ret |= theCSoundDetect[i]->Init(dds.bytVideoInChannels, dds.bytAlarmOutChannels);
        ret |= theCCoder[i]->Init(dds.bytVideoInChannels, dds.bytAlarmOutChannels);

        // 将区域布防配置同步到移动侦测和人形检测
		{
			// 1.区域信息
			struct tagPolygon pg = {0};
			theCAreaDefense[i]->GetPolygonRegionCfg(&pg, 1);
			theCHumanShape[i]->SetPolygonRegionCfg(&pg, 1);
			// 设置移动侦测区域
			unsigned char byMagicBlock[16 * 12] = {0};
			unsigned char byBlockBuf[12][16] = {0};
			int nCount = 0;
			// 1.多边形转换成块
			transPolygonToBlock(&pg, (unsigned char*)byBlockBuf, sizeof(byBlockBuf));
			// 2.块转换为序号数组
			nCount = 0;
			for (int y = 0; y < 12; y++)
			{
				for (int x = 0; x < 16; x++)
				{
					if (byBlockBuf[y][x]) // 如果该区域被标记了，是移动侦测区域
					{
						// 将序列号放到数组中，数组中存放的是已经标记了的宏块的序列号
						byMagicBlock[nCount] = y * 16 + x;
						nCount++;
					}
				}
			}
			theCMotion[i]->EnableMagicBlock(byMagicBlock, nCount);

            // 2.联动信息
            {
                // 屏幕提示 声音告警 上传报警中心
                // Email通知 PTZ FTP 推送
                // 快照 录像 报警输出
                int actions[] = {ACTION_SCREEN, ACTION_BEEP, ACTION_SENDTOSERVER, 
                                ACTION_EMAIL, ACTION_PTZ, ACTION_FTP, ACTION_PUSH,
                                ACTION_SNAPSHOT, ACTION_RECORD, ACTION_ALARMOUT};
                int nEn = 0;
                for (j = 0; (unsigned int)j < sizeof(actions)/sizeof(actions[0]); j++)
                {
                    nEn = theCAreaDefense[i]->IsEnableAction(actions[j]);
                    theCMotion[i]->EnableAction(actions[j], nEn);
                    theCHumanShape[i]->EnableAction(actions[j], nEn);
                }
                // PTZ动作
                int nAction = 0;
                int nIdx = 0;
                for (j = 0; j < MAX_ENC_CHANNEL_NUM; j++)
                {
                    //PTZ动作
                    theCAreaDefense[i]->GetPtzParam(j, &nAction, &nIdx);
                    theCMotion[i]->SetPtzParam(j, nAction, nIdx);
                    theCHumanShape[i]->SetPtzParam(j, nAction, nIdx);
                }
                // 快照通道标志
                int snapshot_flag = 0;
                theCAreaDefense[i]->GetSnapshotParam(&snapshot_flag);
                theCMotion[i]->SetSnapshotParam(snapshot_flag);
                theCHumanShape[i]->SetSnapshotParam(snapshot_flag);
                // 录像通道标志
                int record_flag = 0;
                theCAreaDefense[i]->GetRecordParam(&record_flag);
                theCMotion[i]->SetRecordParam(record_flag);
                theCHumanShape[i]->SetRecordParam(record_flag);
                // 报警输出配置
                int dop_flag = 0;
                theCAreaDefense[i]->GetDopParam(&dop_flag);
                theCMotion[i]->SetDopParam(dop_flag);
                theCHumanShape[i]->SetDopParam(dop_flag);
            }

			// 3.布防时间
			tagMotionSchedule ts = {0};
			theCAreaDefense[i]->GetSchedule(&ts);
			theCMotion[i]->SetSchedule(&ts);
			theCHumanShape[i]->SetSchedule(&ts);
			// 4.使能
			bool bEn = false;
			bEn = theCAreaDefense[i]->Motion_IsEnable();
			theCMotion[i]->Enable(((bEn)?1:0));
			bEn = theCAreaDefense[i]->HumanShape_IsEnable();
			theCHumanShape[i]->Enable(((bEn)?1:0));
		}

		//start the fourth stream
		theCCapturePic[i]->GetCaptureCfg(&nCount,&nIntervalTime,&nFormat, &nWidth, &nHeight, &nQuality);		
		theCCapturePic[i]->GetCaptureStreamType(&nStreamType);
		theCCapturePic[i]->SetCaptureCfgV2(nQuality, nStreamType);

		//看门狗计数清0
		for (n = 0; n < DVR_MAX_STREAM_NUM; n++)
		{
			m_iAV_FrameCnt[i][n] = 0;
		}
	}
	CMotion::SetMode(COptics::GetWorkMode());

	tagEncCfg Info;
	tagEncCfg HDAux_Info;
	theCRecord[0]->GetEncCfg(&Info, 0);
	m_mainStreamType = Info.byStreamType;
	m_auxStreamType = theCCoder[0]->AuxCoder_GetStreamType();
	theCCoder[0]->HDAuxCoder_GetEncCfg(&HDAux_Info);
	m_HDauxStreamType =  HDAux_Info.byStreamType;
//	AVL_INFO("++++++m_mainStreamType++++%d++++++++m_auxStreamType+++++++++%d+++++++++++++++\n",m_mainStreamType,m_auxStreamType);

	//置初始化状态
	m_bInit = 1;
	m_bCheck_exit = 0;
	
	//设置看门狗
	theCSysManager = CSysManager::GetInstance();
	if (theCSysManager == NULL)
	{
		AVL_ERR("[CChannelManager:%s]CSysManager::GetInstance()   failed \n", __FUNCTION__);
		XTRACE_MUTEX_UNLOCK(&InitLock);
		UnInit();
		return -1;
	}

	m_hWatch = theCSysManager->AddDogTask("AVL_AV_CallBack", 30);
	if (m_hWatch == INVALID_HANDLE)
	{
		AVL_ERR("[CChannelManager:%s]theCSysManager->AddDogTask failed\n", __FUNCTION__);
		XTRACE_MUTEX_UNLOCK(&InitLock);
		UnInit();
		return -1;
	}
	theCSysManager->WatchDog(m_hWatch);

	//创建新的录像计划检测线程
#if defined (__SUPPORT_SD_CARD__)
    ret = pthread_create(&m_hCheckScheduleThreadV2, NULL, Fnx_CheckScheduleThreadV2, this);
	if (ret != 0)
	{
		AVL_ERR("[CChannelManager:%s]pthread_create scheduleV2 failed (%d)\n", __FUNCTION__, ret);
        XTRACE_MUTEX_UNLOCK(&InitLock);
		UnInit();
		return -1;
	}
#endif
	
	//创建喂狗线程
	ret = pthread_create(&m_hWatchThread, NULL, Fnx_CheckWatchThread, this);
	if (ret != 0)
	{
		AVL_ERR("[CChannelManager:%s]pthread_create watchdog   failed (%d)\n", __FUNCTION__, ret);
		XTRACE_MUTEX_UNLOCK(&InitLock);
		UnInit();
		return -1;
	}/**/

	//创建状态检测线程
	ret = pthread_create(&m_hCheckThread, NULL, Fnx_CheckThread, this);
	if (ret != 0)
	{
		AVL_ERR("[CChannelManager:%s]pthread_create   failed (%d)\n", __FUNCTION__, ret);
		XTRACE_MUTEX_UNLOCK(&InitLock);
		UnInit();
		return -1;
	}

	//创建抓图检测线程
	ret = pthread_create(&m_hCheckThread_capture, NULL, Fnx_CheckThread_capture, this);
	if (ret != 0)
	{
		AVL_ERR("[CChannelManager:%s]pthread_create  m_hCheckThread_capture  failed (%d)\n", __FUNCTION__, ret);
		XTRACE_MUTEX_UNLOCK(&InitLock);
		UnInit();
		return -1;
	}

	for (i = 0; i < m_iEncChannelCnt; i++)
	{
		//主码率始终编码
		theCRecord[i]->StopEncCapture();
		theCRecord[i]->StartEncCapture();

              AVL_INFO(" StopEncCapture  StartEncCapture %d \n",i);
			 
		//子码率录像则启动编码
		int nIndex = theCRecord[i]->GetRecStreamIndex();

		AVL_DBG("[CChannelManager:%s]theCRecord[%d]->GetRecStreamIndex() = %d\n", __FUNCTION__, i, nIndex);

		if (1 == nIndex)
		{
			theCCoder[i]->AuxCoder_Enable(1);
		}
#ifdef USE_HDAUX
		else if (2 == nIndex)
		{
			theCCoder[i]->HDAuxCoder_Enable(1);
		}
#endif
                  AVL_INFO(" AuxCoder_Enable %d \n",i);
	}

	//获取音频输入接口
	pCConfig = CConfig::GetInstance();
	if (pCConfig == NULL)
	{
		AVL_ERR("[CRecord:%s]pCConfig==NULL\n", __FUNCTION__);
	}
	else
	{
		//使能
		m_AudioInChannel = pCConfig->GetValue("AudioIn", "channel", 1);
		ZWSys_SetAudioChannel((AudioChannelType_t)m_AudioInChannel);

		m_AudioCodec = pCConfig->GetValue("AudioIn", "AudioCodec", 2);
		ZWSys_SetAudioCodec((AudioEncodeMode_t)m_AudioCodec);    
	}

#if defined  __USE_RTMP__
	if(!access(CUSTOM_CONFIG, 0))
	{
		//获取新的配置文件读取实例
		CConfig *pCfg = CConfig::NewInstance();
		int nRet = pCfg->LoadFileRO(CUSTOM_CONFIG);
		if (0 == nRet)
		{
			bSupportRtmp = pCfg->GetValue(RTMP_CFG, "Enable",  0);
			PushAddressSupport = pCfg->GetValue(RTMP_CFG, "PushAddressSupport",  0);
			m_ncPushAudioEnable = pCfg->GetValue(RTMP_CFG, "PushAudioEnable",  1);
		}
		CConfig::ReleaseInstance(pCfg);
	}	
	AVL_INFO("[%s][%d]bSupportRtmp =%d\n",__FUNCTION__,__LINE__,bSupportRtmp);

	if(1 == bSupportRtmp)
	{
		//读取rtmp所有信息
		GetRtmpEable(); 
        ulSerialNum = ZWSys_GetSerialNum(szSerialNum);     
#ifdef SOFT_ENCRYPTION
        ActiveStatus = PI31415926(ActiveCode, GetIntSerialNum(szSerialNum), nBoardID, &dwCapability);
#else
        ActiveStatus = PI31415926(ActiveCode, ulSerialNum, nBoardID, &dwCapability);
#endif

		if(0 == dwCapability&CAPABILITY)	//不支持
		{
			for(i=0;i<DVR_MAX_STREAM_NUM;i++)
			{
				for(j=0;j<MAX_CH_MANAGER_NUM;j++)
				{
					RtmpStatus[i][j] = eZWRTMP_STATUS_NotActived;
				}
			}
			ActionCodeOk = 0;
			AVL_INFO("Don't support rtmp\n");
		}
		else
		{
			if(0 == ActiveStatus)
				ActionCodeOk = 1;	//激活
			else
				ActionCodeOk = 0;
		}

		//初始化rtmp
		ZW_RTMP_Init(); 	
		ret = pthread_create(&m_RtmpThread, NULL, Creart_channel_thread, this);
		if (ret != 0)
		{
			AVL_ERR("[CChannelManager:%s]pthread_create  Creart_channel_thread  failed (%d)\n", __FUNCTION__, ret);
			XTRACE_MUTEX_UNLOCK(&InitLock);
			UnInit();
			return -1;
		}		
	}	
#endif

#ifdef ACTIVE_CODE_SUPPORT
        if (0 != access(AUTHORIZATION_CFG, 0))  //如果文件不存在
        {
            if ((fp = fopen(AUTHORIZATION_CFG, "a")) != NULL )
            {
                fprintf(fp,"%s\r\n", "[system]");
                fprintf(fp,"%s\r\n", "dvr=idvr9000");
                fprintf(fp,"%s\r\n", "[AuthorizationInfo]");
                fprintf(fp,"%s\r\n", "ActiveCode=");
                fprintf(fp,"%s\r\n", "[end]");
                fprintf(fp,"%s\r\n", "dvr=idvr9000");
                fclose(fp);
                fp = NULL;
            }       
        }   
        //获取新的配置文件读取实例
        CConfig *pCfg = CConfig::NewInstance();
        int nRet = pCfg->LoadFileRO(AUTHORIZATION_CFG);
        if (0 == nRet)
        {
            pCfg->GetValue(s_ActiveCodeSec, s_ActiveCodeKey, "", ActiveCode, 64);
        }
        CConfig::ReleaseInstance(pCfg);
    
        ZWSys_GetSerialNum(szSerialNum);     
#ifdef SOFT_ENCRYPTION
        ActiveStatus = PI31415926(ActiveCode, GetIntSerialNum(szSerialNum), nBoardID, &dwCapability);
#endif

#ifdef AF_DEV_SUPPORT
        if (0 == (dwCapability & CAPABILITY_AF))    //不支持
        {
            ActionCodeOk &= (~CAPABILITY_AF);
            AVL_INFO("Don't support AF\n");
        }
        else
        {
            if (0 == ActiveStatus)
            {
                ActionCodeOk |= CAPABILITY_AF;  //激活
            }
            else
            {
                ActionCodeOk &= (~CAPABILITY_AF);
            }
        }
#endif
    
#ifdef DL_ACTIVE_CODE_SUPPORT
        if (0 == (dwCapability & CAPABILITY_DL))    //不支持
        {
            ActionCodeOk &= (~CAPABILITY_DL);
            AVL_INFO("Don't support DL\n");
        }
        else
        {
            if (0 == ActiveStatus)
            {
                ActionCodeOk |= CAPABILITY_DL;  //激活
            }
            else
            {
                ActionCodeOk &= (~CAPABILITY_DL);
            }
        }
#endif
    
#ifdef __USE_RTMP__
        if (!access(CUSTOM_CONFIG, 0))
        {
            //获取新的配置文件读取实例
            CConfig *pCfg = CConfig::NewInstance();
            int nRet = pCfg->LoadFileRO(CUSTOM_CONFIG);
            if (0 == nRet)
            {
                bSupportRtmp = pCfg->GetValue(RTMP_CFG, "Enable",  0);
                PushAddressSupport = pCfg->GetValue(RTMP_CFG, "PushAddressSupport",  0);
            }       
            CConfig::ReleaseInstance(pCfg);
        }   
        AVL_INFO("bSupportRtmp =%d\n", bSupportRtmp);

        if (1 == bSupportRtmp) 
        {
            //time_begin = time(NULL);
            //读取rtmp所有信息
            GetRtmpEable(); 
            if (0 == (dwCapability & CAPABILITY_RTMP))  //不支持
            {
                for (i = 0; i < DVR_MAX_STREAM_NUM; i++)
                {
                    for (j = 0; j < MAX_CH_MANAGER_NUM; j++)
                    {
                        RtmpStatus[i][j] = eZWRTMP_STATUS_NotActived;
                    }
                }
                ActionCodeOk &= ~(1 << RTMP_SUPPORT);
                AVL_INFO("Don't support rtmp\n");
            }
            else
            {
                if (0 == ActiveStatus)
                {
                    ActionCodeOk |= (1 << RTMP_SUPPORT);    //激活
                }
                else
                {
                    ActionCodeOk &= ~(1 << RTMP_SUPPORT);
                }
            }
            
            //初始化rtmp
            ZW_RTMP_Init();     
            ret = pthread_create(&m_RtmpThread, NULL, Creart_channel_thread, this);
            if (ret != 0)
            {
                AVL_ERR("[CChannelManager]pthread_create  Creart_channel_thread  failed (%d)\n", ret);
                XTRACE_MUTEX_UNLOCK(&InitLock);
                UnInit();
                return -1;
            }       
        }   
#endif
        /* 只有激活成功才能设置到SDK中 */
        if (0 == ActiveStatus)
        {
            ZWSys_SetActiveStatusCode(ActionCodeOk);
        }
#endif

	XTRACE_MUTEX_UNLOCK(&InitLock);

	return (int) ret;
}

//##ModelId=49BF1B4D0140
int CChannelManager::UnInit()
{
	int i;

	//毁狗
	theCSysManager->WatchDog(m_hWatch);
	#ifdef DEBUG_WATCH
	theCSysManager->DestroyDogTask(m_hWatch);
	#endif
	m_hWatch = NULL;
	//取消回调函数
	ZWSys_EncRegisterStreamReadCallback(NULL);

	//结束计划检测线程
	m_bCheck_exit = 1;

#if 0
	if (0 != m_hCheckScheduleThread)
	{
		pthread_cancel(m_hCheckScheduleThread);
		pthread_join(m_hCheckScheduleThread, NULL);
		m_hCheckScheduleThread = 0;
	}
#endif

	if (0 != m_hCheckScheduleThreadV2)
	{
		pthread_cancel(m_hCheckScheduleThreadV2);
		pthread_join(m_hCheckScheduleThreadV2, NULL);
		m_hCheckScheduleThreadV2 = 0;
	}

	if (0 != m_hCheckThread)
	{
		pthread_cancel(m_hCheckThread);
		pthread_join(m_hCheckThread, NULL);
		m_hCheckThread = 0;
	}
	if (0 != m_hCheckThread_capture)
	{
		pthread_cancel(m_hCheckThread_capture);
		pthread_join(m_hCheckThread_capture, NULL);
		m_hCheckThread_capture = 0;
	}

	if (0 != m_hWatchThread)
	{
		pthread_cancel(m_hWatchThread);
		pthread_join(m_hWatchThread, NULL);
		m_hWatchThread = 0;
	}

#ifdef __USE_RTMP__
	if (0 != m_RtmpThread)
	{
		pthread_cancel(m_RtmpThread);
		pthread_join(m_RtmpThread, NULL);
		m_RtmpThread = 0;
	}	
#endif

	//置状态为未初始化
	m_bInit = 0;
	m_cInit = 0;
	for (i = 0; i < MAX_CH_MANAGER_NUM; i++)
	{
		if (NULL != theCMotion[i])
			delete theCMotion[i];

		if (NULL != theCHumanShape[i])
			delete theCHumanShape[i];

		if (NULL != theCSoundDetect[i])
			delete theCSoundDetect[i];

        if (NULL != theCOptics[i])
            delete theCOptics[i];

        if (NULL != theCRecord[i])
            delete theCRecord[i];

        if (NULL != theCOsd[i])
            delete theCOsd[i];

		if (NULL != theCCoder[i])
			delete theCCoder[i];

		if (NULL != theCCapturePic[i])
			delete theCCapturePic[i];

        theCMotion[i] = NULL;
		theCHumanShape[i] = NULL;
        theCSoundDetect[i] = NULL;
        theCOptics[i] = NULL;
        theCRecord[i] = NULL;
        theCOsd[i] = NULL;
        theCCoder[i] = NULL;
        theCCapturePic[i] = NULL;
		theCoderHandle[i] = NULL;
		m_iAV_status[i] = 0;	//完毕
	}

	if (NULL != m_PDataBuf)
	{
		delete m_PDataBuf;
		m_PDataBuf = NULL;
		m_iLength = 0;
	}
	m_iEncChannelCnt = 0;
	m_FnxAvCallBack = NULL;

#if defined  (__USE_RTMP__)
	// 反初始化
	ZW_RTMP_Uninit();
#endif
	return (int) 0;
}

//!\brief 获取制式
//!\param int *pStandard,接受制式,1-NTSC,2-PAL,4-SECAM,0x80000000-none
//!\return 成功0,否则失败
//##ModelId=49B4856A0139
int CChannelManager::GetStandard(int *pStandard)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (NULL == pStandard)
	{
		AVL_ERR("NULL == pStandard\n");
		return -1;
	}
	
	return CRecord::GetStandard(pStandard);
}

//!\brief 设置制式
//!\param int Standard,1-NTSC,2-PAL,4-SECAM
//!\return 成功0,否则失败
//##ModelId=49B4856A0139
int CChannelManager::SetStandard(int Standard)
{
	int gstandard;
	int ch;
	int result = 0;

	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	GetStandard(&gstandard);
	if (gstandard == Standard)
	{
		AVL_DBG("Standard =%d no change\n", Standard);
		return 0;
	}
//!\ 应SDK要求 不需要停止编码
//!\ deng 2012-1-24
//!\ end
	//停止编码
/*
	for (ch = 0; ch < m_iEncChannelCnt; ch++)
	{
		result = theCCoder[ch]->StopAuxEncCapture();
		if (result)
		{
			AVL_ERR("theCCoder[%d]->StopAuxEncCapture", ch);
			return result;
		}
#ifdef USE_HDAUX
		result = theCCoder[ch]->StopHDAuxEncCapture();
		if (result)
		{
			AVL_ERR("theCCoder[%d]->StopHDAuxEncCapture", ch);
			return result;
		}
#endif
		result = theCRecord[ch]->StopEncCapture();
		if (result)
		{
			AVL_ERR("theCRecord[%d]->StopEncCapture", ch);
			return result;
		}
	}
*/

	result = CRecord::SetStandard(Standard);
	if (!result)
	{							//设置成功
		CCoder::SetStandard(Standard);
	}
	//重开编码
	for (ch = 0; ch < m_iEncChannelCnt; ch++)
	{
		theCRecord[ch]->ForceSwicth();
		result = theCRecord[ch]->StartEncCapture();
		if (result)
		{
			AVL_ERR("theCRecord[%d]->StartEncCapture", ch);
			return result;
		}
		result = theCCoder[ch]->StartAuxEncCapture();
		if (result)
		{
			AVL_ERR("theCCoder[%d]->StartAuxEncCapture", ch);
			return result;
		}
#ifdef USE_HDAUX
		result = theCCoder[ch]->StartHDAuxEncCapture();
		if (result)
		{
			AVL_ERR("theCCoder[%d]->StartHDAuxEncCapture", ch);
			return result;
		}
#endif
	}

	return 0;
}

#ifdef __USE_RTMP__
int CChannelManager::Reconsitution_channel(int reserver)
{
	int nRet = IDVR_OK, nSDKRet;

	pthread_t thrd_id;		//线程ID

	//创建分离线程
	nSDKRet = pthread_create(&thrd_id, NULL, Recreate_channel_thread, (void *)this);
	if (0 != nSDKRet)
	{
		//AVL_INFO("pthread_create( network_config_thread ) DETACHED return %d\n", nSDKRet);
		nRet = IDVR_CIU_CALLSDK_FAIL;
	}

	return nRet;
}

int CChannelManager:: GetRtmpEable()
{
	char value[64]={0};
	int i =0;
	int j =0;
	FILE *fp=NULL;

	if(1 == PushAddressSupport)
	{
		PushAddressEnable = pCConfig->GetValue(RTMP_CFG, "PushAddressEnable", 0);
		pCConfig->GetValue(RTMP_CFG, "szPushURLServer", "", szPushURLServer, sizeof(szPushURLServer));
		
		m_ncPushAudioEnable = pCConfig->GetValue(RTMP_CFG, "PushAudioEnable", 1);
	}
	else
	{
		m_ncPushAudioEnable = 1;
		PushAddressEnable = 0;
		memset(szPushURLServer,0,sizeof(szPushURLServer));
	}

	for(i=0;i<DVR_MAX_STREAM_NUM;i++)
	{
		for(j=0;j<MAX_CH_MANAGER_NUM;j++)
		{			
			memset(value,0,sizeof(value));
			snprintf(value,sizeof(value),"%dEable_%d",i,j);
			Veable[i][j] = pCConfig->GetValue(RTMP_CFG, value, 0);	

			memset(value,0,sizeof(value));
			snprintf(value,sizeof(value),"%dName_%d",i,j);
			pCConfig->GetValue(RTMP_CFG, value, "", ClientConnectParam[i][j].szUserName, 64);
			
			memset(value,0,sizeof(value));
			snprintf(value,sizeof(value),"%dPassword_%d",i,j);
			pCConfig->GetValue(RTMP_CFG, value, "", ClientConnectParam[i][j].szPassword, 64);	

			memset(value,0,sizeof(value));
			snprintf(value,sizeof(value),"%dAddress_%d",i,j);
			pCConfig->GetValue(RTMP_CFG, value, "", ClientConnectParam[i][j].szStreamPullingAddress, 512);
			strncpy(gConnectParam[i][j].szStreamPullingAddress,ClientConnectParam[i][j].szStreamPullingAddress,sizeof(gConnectParam[i][j].szStreamPullingAddress) - 1);
		}
	}

	if(1 == PushAddressEnable)
	{
		Veable[0][0] = 1;
		Veable[1][0] = 1;
	}
	
	if(0 != access(RTMP_CONFIG, 0))//如果文件不存在
	{
		if((fp = fopen(RTMP_CONFIG,"a")) != NULL )
		{
			fprintf(fp,"%s\r\n","[system]");
			fprintf(fp,"%s\r\n","dvr=idvr9000");
			fprintf(fp,"%s\r\n","[RTMP_cfg]");
			fprintf(fp,"%s\r\n","ActiveCode=");
			fprintf(fp,"%s\r\n","[end]");
			fprintf(fp,"%s\r\n","dvr=idvr9000");
			fclose(fp);
		}		
	}	

    //获取新的配置文件读取实例
    CConfig *pCfg = CConfig::NewInstance();
    int nRet = pCfg->LoadFileRO(RTMP_CONFIG);
	if (0 == nRet)
	{
		pCfg->GetValue(RTMP_CFG, "ActiveCode", "", ActiveCode, 64);
	}		
	CConfig::ReleaseInstance(pCfg);

	return 0;
}

int CChannelManager::Record_GetRtmpParam(int tpye, int nCh, char *szUserName ,int userNameSize,char *szPassword , int PassWordSize,
	char *szStreamPullingAddress, int StrPullAddrSize, unsigned int *eable, unsigned int *status, char *szPlayURL, int playURLSize, char *cPushAudioEnable)
{
	int ret = 0;
	char value[64]={0};
	char tmpstatus = 0;
	if((NULL == szUserName)||(NULL == szPassword)||(NULL == szStreamPullingAddress)||(NULL == eable)||(NULL == status)||(NULL == szPlayURL)||(NULL == cPushAudioEnable))
	{
		AVL_ERR("Record_GetRtmpParam error pare is NULL\n");
		return -1;
	}

    if (nCh >= m_iEncChannelCnt)
    {
        AVL_ERR("Record_GetRtmpParam error pare is NULL\n");
        return -1;
    }

	strncpy(szUserName,ClientConnectParam[tpye][nCh].szUserName, userNameSize > 0 ? userNameSize - 1: 0);
	strncpy(szPassword,ClientConnectParam[tpye][nCh].szPassword, PassWordSize > 0 ? PassWordSize - 1: 0);
	if(userNameSize > 0 && PassWordSize > 0)
	{
		szUserName[userNameSize - 1] = '\0';
		szPassword[PassWordSize - 1] = '\0';
	}
	if(1 != PushAddressEnable)
	{
		strncpy(ClientConnectParam[tpye][nCh].szStreamPullingAddress,gConnectParam[tpye][nCh].szStreamPullingAddress,sizeof(ClientConnectParam[tpye][nCh].szStreamPullingAddress) - 1);
	}

	strncpy(szStreamPullingAddress,ClientConnectParam[tpye][nCh].szStreamPullingAddress, StrPullAddrSize > 0 ? StrPullAddrSize - 1: 0);
	if(StrPullAddrSize > 0)
	{
		szStreamPullingAddress[StrPullAddrSize - 1] = '\0';
	}
	*eable = Veable[tpye][nCh];
	*status = RtmpStatus[tpye][nCh];	
	*cPushAudioEnable = m_ncPushAudioEnable;
	if((1 == Veable[tpye][nCh])&&((eZWRTMP_STATUS_Connected == RtmpStatus[tpye][nCh])||(eZWRTMP_STATUS_Reconnected == RtmpStatus[tpye][nCh])))
	{
		ret = ZW_RTMP_Client_GetConnectParam(RtmpHandle[tpye][nCh], &ClientConnectParam[tpye][nCh]);
		strncpy(szPlayURL,ClientConnectParam[tpye][nCh].szPlayURL, playURLSize > 0 ?playURLSize - 1: 0);
		AVL_INFO("+++++ClientConnectParam[%d][%d].szPlayURL=%s++++++++++\n",tpye,nCh,ClientConnectParam[tpye][nCh].szPlayURL);
	}

	if((1==PushAddressEnable)&&(eZWRTMP_STATUS_Connected != RtmpStatus[tpye][nCh])&&(eZWRTMP_STATUS_Reconnected != RtmpStatus[tpye][nCh]))
	{
		memset(szStreamPullingAddress,0,512);
	}

	return 0;
}

int CChannelManager::Record_GetPushAddressCfg(char *bsupport, char *enable, char *pushurl )
{
	if((NULL == bsupport)||(NULL == enable)||(NULL == pushurl))
	{
		AVL_ERR("Record_Record_GetPushAddressCfg error pare is NULL\n");
		return -1;
	}
	*bsupport = PushAddressSupport;
	*enable = PushAddressEnable;
	strncpy(pushurl,szPushURLServer,sizeof(szPushURLServer) - 1);
	
	return 0;
}

int CChannelManager::Record_Notif_Creat_Channel(int *ret)
{
	Reconsitution_channel(1);

	if(Dthread_num>5)
	{
		*ret = -1;
	}

	return 0;
}

int CChannelManager::Record_SetPushAddressCfg(char enable ,char *PushURL)
{
	int i;
	int j;
	int iChanged = 0, iRet = 0;
	
	if(NULL == PushURL)
	{
		AVL_ERR("PushURL is NULL error\n");
	}

	if(1 == PushAddressSupport) //支持自动注册
	{
		if(PushAddressEnable != enable)
		{
			for(i=0;i<DVR_MAX_STREAM_NUM;i++)
			{
				for(j=0;j<MAX_CH_MANAGER_NUM;j++)
				{
					ChangeFlag[i][j] = 1; //重新创建通道(所有的已创建的)					
				}
			}

			iChanged = 1;
			PushAddressEnable = enable;
			pCConfig->SetValue(RTMP_CFG, "PushAddressEnable", enable);				

			if(1 == PushAddressEnable)	//默认开启时主子的推流
			{
				Veable[0][0] = 1;
				Veable[1][0] = 1;
			}
		}
		if(0 != strcmp(szPushURLServer,PushURL))
		{
			for(i=0;i<DVR_MAX_STREAM_NUM;i++)
			{
				for(j=0;j<MAX_CH_MANAGER_NUM;j++)
				{
					ChangeFlag[i][j] = 1; //重新创建通道(所有的已创建的)					
				}
			}
			if(Dthread_num<=5)
			{
				iChanged = 1;
				strncpy(szPushURLServer,PushURL,sizeof(szPushURLServer) - 1);
				pCConfig->SetValue(RTMP_CFG, "szPushURLServer", szPushURLServer);	
			}
            //AVL_INFO("+++++++++++Dthread_num=%d++++++++\n",Dthread_num);
		}		

		if (iChanged == 1 && RtmpInit == 1)
		{		
			AVL_WARN("********param changed*****\r\n");
			Record_Notif_Creat_Channel(&iRet);
		}
	}

	return 0;
}

int CChannelManager::Record_SetRtmpParam(int tpye, int nCh, char *szUserName ,char *szPassword ,char *szStreamPullingAddress,unsigned int eable, unsigned int status, char cPushAudioEnable)
{
	char value[64]={0};
	int i=0;
	int j=0;
	int iChanged = 0, iRet = 0;
	
	if((nCh >= m_iEncChannelCnt)||(NULL == szUserName)||(NULL == szPassword)||(NULL == szStreamPullingAddress))
	{
		AVL_ERR("Record_SetRtmpParam error pare is NULL\n");
		return -1;
	}

	if (nCh >= m_iEncChannelCnt)
    {
        AVL_ERR("Record_GetRtmpParam error pare is NULL\n");
        return -1;
    }

	if ((0 != cPushAudioEnable) && (1 != cPushAudioEnable))
	{
		AVL_ERR("Record_SetRtmpParam error pare is invalid, cPushAudioEnable=[%d]\n", cPushAudioEnable);
		return -1;
	}

	if (cPushAudioEnable != m_ncPushAudioEnable)
	{	
		m_ncPushAudioEnable = cPushAudioEnable;
		pCConfig->SetValue(RTMP_CFG, "PushAudioEnable", m_ncPushAudioEnable);	
	}
	
	if(Veable[tpye][nCh] != eable)
	{	
		if (0 == PushAddressEnable)
		{
			if (ChangeFlag[tpye][nCh] == 0)
			{
				ChangeFlag[tpye][nCh] = 1;
				iChanged = 1;
			}		
		}
		
		Veable[tpye][nCh] = eable;

		if(1 == PushAddressEnable)	//自动推流开启时，需要默认开启主子的推流
		{
			if (0 == tpye || 1 == tpye)
				Veable[tpye][nCh] = 1;
		}			
		
		memset(value,0,sizeof(value));
		snprintf(value,sizeof(value),"%dEable_%d",tpye,nCh);
		pCConfig->SetValue(RTMP_CFG, value, Veable[tpye][nCh]);				
	}

	if(0 != strcmp(ClientConnectParam[tpye][nCh].szUserName,szUserName))
	{		
		if(0==PushAddressEnable)
		{
			if (ChangeFlag[tpye][nCh] == 0)
			{
				ChangeFlag[tpye][nCh] = 1;
				iChanged = 1;
			}
			strncpy(ClientConnectParam[tpye][nCh].szUserName,szUserName,sizeof(ClientConnectParam[tpye][nCh].szUserName) - 1);
			memset(value,0,sizeof(value));
			snprintf(value,sizeof(value),"%dName_%d",tpye,nCh);
			pCConfig->SetValue(RTMP_CFG, value, szUserName);	
		}
	}

	if(0 != strcmp(ClientConnectParam[tpye][nCh].szPassword,szPassword))
	{		
		if(0==PushAddressEnable)
		{
			if (ChangeFlag[tpye][nCh] == 0)
			{
				ChangeFlag[tpye][nCh] = 1;
				iChanged = 1;
			}
			strncpy(ClientConnectParam[tpye][nCh].szPassword,szPassword,sizeof(ClientConnectParam[tpye][nCh].szPassword));
			memset(value,0,sizeof(value));
			snprintf(value,sizeof(value),"%dPassword_%d",tpye,nCh);
			pCConfig->SetValue(RTMP_CFG, value, szPassword);	
		}
	}
	
	if(0 != strcmp(ClientConnectParam[tpye][nCh].szStreamPullingAddress,szStreamPullingAddress))
	{		
		if(0==PushAddressEnable)//只有自动推流关闭的时候，才能设置手动推流
		{
			if (ChangeFlag[tpye][nCh] == 0)
			{
				ChangeFlag[tpye][nCh] = 1;
				iChanged = 1;
			}
			
			for(i=0;i<DVR_MAX_STREAM_NUM;i++)
			{
				for(j=0;j<MAX_CH_MANAGER_NUM;j++)
				{
					if(0 == strcmp(ClientConnectParam[i][j].szStreamPullingAddress,szStreamPullingAddress))	//如果有一样的地址，则设置失败
					{
						AVL_INFO("Already exist address,Setup error!!!type=[%d] nCh=[%d]\n",tpye,nCh);
						//return -1;
					}
				}
			}	

			strncpy(ClientConnectParam[tpye][nCh].szStreamPullingAddress,szStreamPullingAddress,sizeof(ClientConnectParam[tpye][nCh].szStreamPullingAddress) - 1);
			strncpy(gConnectParam[tpye][nCh].szStreamPullingAddress,szStreamPullingAddress,sizeof(gConnectParam[tpye][nCh].szStreamPullingAddress) - 1);
			memset(value,0,sizeof(value));
			snprintf(value,sizeof(value),"%dAddress_%d",tpye,nCh);
			pCConfig->SetValue(RTMP_CFG, value, szStreamPullingAddress);	
		}
	}

	if (iChanged == 1 && RtmpInit == 1)
	{	
		AVL_WARN("********param changed*****\r\n");
		Record_Notif_Creat_Channel(&iRet);
	}

	return 0;
}

#endif

int CChannelManager::Record_GetActionCode(char *pCode, int codeSize, char *pCodeFlag, char *activestatus, 
		unsigned int *puSupportActivedFeatureMask, unsigned int *puAllActivedStatusMask)
{
	if (NULL == pCode)
	{
		AVL_ERR(" pCode is NULL \n");
		return -1;
	}

	*pCodeFlag = 0;
	*activestatus = 1;	//激活失败
	*puSupportActivedFeatureMask = 0;
#ifdef __GOKE__
	return 0;
#endif
#ifdef ACTIVE_CODE_SUPPORT
	*pCodeFlag = 1;
	strncpy(pCode, ActiveCode, codeSize > 0 ? codeSize - 1: 0);
	if (0 != ActionCodeOk)
	{
		*activestatus = 0;
	}
	else
	{
		*activestatus = 1;
	}

#ifdef AF_DEV_SUPPORT
	*puSupportActivedFeatureMask |= CAPABILITY_AF;
#endif

#ifdef DL_ACTIVE_CODE_SUPPORT
    *puSupportActivedFeatureMask |= CAPABILITY_DL;
#endif

#ifdef __USE_RTMP__
	*puSupportActivedFeatureMask |= (1<<RTMP_SUPPORT);
#endif

#ifdef __MODULE_4G_SUPPORT__
    int bSupport4G = 0;
    ZWSys_Get4GSupportV2(&bSupport4G);
    if (bSupport4G)
    {
        *puSupportActivedFeatureMask |= CAPABILITY_EXTERNSIMCARD;
    }
#endif

	*puAllActivedStatusMask = ActionCodeOk;
#endif

	return 0;
}

int CChannelManager::Record_SetActionCode(char *pCode,int unAllActivedFunctionMask)
{
#ifdef ACTIVE_CODE_SUPPORT
	int nRet = 0;
	int BoardInfo = 0;
	int ActiveStatus = 1;
	unsigned int dwCapability = 0;
    unsigned char szSerialNum[32] = {0};
	int j = 0;
	FILE *fp = NULL;
#ifdef __MODULE_4G_SUPPORT__    
	int bSupport4G = 0;
#endif

#if  defined(__MODULE_4G_SUPPORT__) && !defined(__SUPPORT_COMMON_DID_OPREATE__)
    ZWSys_Get4GSupportV2(&bSupport4G);
    if (bSupport4G && strlen(pCode))
    {
        char did[64] = {0};

    	ZWSys_GetSerialNum(szSerialNum);
    	ZWSys_GetBoardInfo(&BoardInfo);
        ActiveStatus = PI31415926(pCode, GetIntSerialNum(szSerialNum), BoardInfo, &dwCapability);
    	if (0 != ActiveStatus)
    	{
    		//激活码解码失败
    		AVL_INFO("[%s][%d]AtionCode is error \n", __FUNCTION__, __LINE__);
    		return -1;
    	}

    	if (dwCapability & CAPABILITY_EXTERNSIMCARD)
    	{
    		//激活运营模式，需要判断DID是不是 运营 号段
    		ZWSys_GetTUTKUID(did);
    		if (0 == ZWSys_CheckDidIsFlowOpreate(did))
    		{
    			//不是运营 号段 ，激活失败
    			AVL_ERR("AtionCode is error \n");
    			return -1;
    		}
    	}
    }
#endif     	

	if (strlen(pCode) > 64)
	{
		AVL_WARN("pCode is too large \n");
		return -1;
	}

	if (0 != access(AUTHORIZATION_CFG, 0))//如果文件不存在
	{
		if ((fp = fopen(AUTHORIZATION_CFG, "a")) != NULL )
		{
			fprintf(fp,"%s\r\n", "[system]");
			fprintf(fp,"%s\r\n", "dvr=idvr9000");
			fprintf(fp,"%s\r\n", "[RTMP_cfg]");
			fprintf(fp,"%s\r\n", "ActiveCode=");
			fprintf(fp,"%s\r\n", "[end]");
			fprintf(fp,"%s\r\n", "dvr=idvr9000");
			fclose(fp);
		}		
	}
	if (0 != strcmp(ActiveCode, pCode))
	{
		strncpy(ActiveCode, pCode, sizeof(ActiveCode) - 1);

		//获取新的配置文件读取实例
		CConfig *pCfg = CConfig::NewInstance();
		nRet = pCfg->LoadFile(AUTHORIZATION_CFG);
		if (0 == nRet)
		{
			pCfg->SetValue(s_ActiveCodeSec, s_ActiveCodeKey, pCode);
			pCfg->SaveFile();
		}		
		CConfig::ReleaseInstance(pCfg);
	}
	
	//获取序列号
	ZWSys_GetBoardInfo(&BoardInfo);

	ZWSys_GetSerialNum(szSerialNum);     
#ifdef SOFT_ENCRYPTION
	ActiveStatus = PI31415926(ActiveCode, GetIntSerialNum(szSerialNum), BoardInfo, &dwCapability);
#endif

#ifdef AF_DEV_SUPPORT
	if (0 == (dwCapability & CAPABILITY_AF))	//不支持
	{
		ActionCodeOk &= ~(1 << AF_SUPPORT);
		AVL_INFO("!!!!Don't Support AF!!!!!!!!!!\n");
	}
	else
	{
		if (0 == ActiveStatus)
		{
			ActionCodeOk |= (1 << AF_SUPPORT);
			AVL_INFO("AtionCode is right!\n");
		}
		else
		{
			ActionCodeOk &= ~(1 << AF_SUPPORT);
			AVL_ERR("AtionCode is error!\n");
		}			
	}
#endif

#ifdef __USE_RTMP__
	if (0 == (dwCapability & CAPABILITY_RTMP))	//不支持
	{
		for (i = 0; i < DVR_MAX_STREAM_NUM; i++)
		{
			for (j = 0; j < MAX_CH_MANAGER_NUM; j++)
			{
				RtmpStatus[i][j] = eZWRTMP_STATUS_NotActived;
			}
		}
		ActionCodeOk &= ~(1 << RTMP_SUPPORT);
		AVL_INFO("!!!!Don't Support Rtmp!!!!!!!!!!\n");
	}
	else
	{
		for (i = 0; i < DVR_MAX_STREAM_NUM; i++)
		{
			for (j = 0; j < MAX_CH_MANAGER_NUM; j++)
			{
				ChangeFlag[i][j] = 1; //重新创建通道(所有的已创建的)
			}
		}
		
		if (0 == ActiveStatus)
		{
			ActionCodeOk |= (1 << RTMP_SUPPORT);
			AVL_INFO("AtionCode is right!\n");
		}
		else
		{
			ActionCodeOk &= ~(1 << RTMP_SUPPORT);
			AVL_INFO("AtionCode is error!\n");
		}			
	}
	
	Record_Notif_Creat_Channel(&nRet);
	if (nRet == -1)
	{
		return -1;
	}
#endif

#ifdef __MODULE_4G_SUPPORT__
	ZWSys_Get4GSupportV2(&bSupport4G);
	if (bSupport4G)
	{
		if (0 == (dwCapability & CAPABILITY_EXTERNSIMCARD))    //不支持
		{
			ActionCodeOk &= (~CAPABILITY_EXTERNSIMCARD);
			AVL_INFO("Don't support CAPABILITY_EXTERNSIMCARD\n");
		}
		else
		{
			if (0 == ActiveStatus)
			{
				ActionCodeOk |= CAPABILITY_EXTERNSIMCARD;  //激活
			}
			else
			{
				ActionCodeOk &= (~CAPABILITY_EXTERNSIMCARD);
			}
		}
	}	
#endif


    /* 只有激活成功才能设置到SDK中 */
    if (0 == ActiveStatus)
    {
    	ZWSys_SetActiveStatusCode(ActionCodeOk);
	}

	if (0 != (ActionCodeOk & (1 << AF_SUPPORT)))	//若授权
	{
		//重设用户指定的OSD及马赛克
		//(SDK中对于未授权的球机设备会强制OSD并打满屏马赛克)
		for (j = 0; j < MAX_CH_MANAGER_NUM; j++)
		{
			theCOsd[j]->SetCharBitMap(NULL, NULL, 0, 0, 0);
		}
	}
#endif
	
	return 0;
}

//##ModelId=49B4856A0139
int CChannelManager::GetChNum()
{
	return m_iEncChannelCnt;
}

//##ModelId=49BDB39801E1
int CChannelManager::Record_Pause(int nCh, int bPause)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager]not inited\n");
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager]channel_no err, channel %d\n", nCh);
		return -1;
	}

	return (int) theCRecord[nCh]->Pause(bPause);
}

//##ModelId=49BDB3980227
int CChannelManager::Record_StartMode(int nCh, enumRecordMode model)
{
	int ret = 0;

	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager]not inited\n");
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager]channel_no err, channel %d\n", nCh);
		return -1;
	}

	ret = theCRecord[nCh]->StartMode(model);

	return (int) ret;
}

enumRecordMode CChannelManager::Record_GetRecMode(int nCh)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager]not inited\n");
		return RECORD_MODE_CLOSE;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager]channel_no err, channel %d\n", nCh);
		return RECORD_MODE_CLOSE;
	}
	return theCRecord[nCh]->GetRecMode();
}

//!\brief   设置录像码流索引
//!\param   int nCh         通道
//!\param   int nIndex  0表示主码流，1表示子码流，2表示高清子码流
//!\return 成功为0，其它为错误码
int CChannelManager::Record_SetRecStreamIndex(int nCh, int nIndex)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager]not inited\n");
		return -1;
	}

	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager]channel_no err, channel %d\n", nCh);
		return -1;
	}

	if (1 == nIndex)
	{
		theCCoder[nCh]->AuxCoder_Enable(1);
	}
#ifdef USE_HDAUX
	else if (2 == nIndex)
	{
		theCCoder[nCh]->HDAuxCoder_Enable(1);
	}
#endif

	return theCRecord[nCh]->SetRecStreamIndex(nIndex);
}

//!\brief       获取录像码率索引
//!\param   int nCh 通道
//!\return   录像码流索引(0表示主码流，1表示子码流，2表示高清子码流)
int CChannelManager::Record_GetRecStreamIndex(int nCh)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return 0;
	}

	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return 0;
	}

	return theCRecord[nCh]->GetRecStreamIndex();
}

//##ModelId=49BDB3980277
int CChannelManager::Record_GetState(int nCh)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCRecord[nCh]->GetState();
}

//##ModelId=49BDB398028B
int CChannelManager::Record_GetEncCfg(int nCh, tagEncCfg * Info, int nEnctype)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	if (NULL == Info)
	{
		AVL_ERR("[CChannelManager:%s]Info==NULL,ch=%d\n", __FUNCTION__, nCh);
		return -1;
	}
	return (int) theCRecord[nCh]->GetEncCfg(Info, nEnctype);
}

//##ModelId=49BDB3980317
int CChannelManager::Record_SetEncCfg(int nCh, tagEncCfg * Info, int nEnctype)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	if (NULL == Info)
	{
		AVL_ERR("[CChannelManager:%s]Info==NULL,ch=%d\n", __FUNCTION__, nCh);
		return -1;
	}
	if (Info->byStreamType > 1 || Info->byStreamType < 0)
	{
		AVL_ERR("[CChannelManager:%s]Info->byStreamType=%d\n", __FUNCTION__, Info->byStreamType);
		return -1;
	}
	if(0 == nEnctype)
		m_mainStreamType = Info->byStreamType;
	return (int) theCRecord[nCh]->SetEncCfg(Info, nEnctype);
}

//!\brief   恢复编码参数到出厂设置
//!\param   int nCh             通道
//!\param   int nEnctype        录像的编码类型
//!\return 成功为0，失败为-1
int CChannelManager::Record_ResumeEncCfg(int nCh, int nEnctype)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return theCRecord[nCh]->ResumeEncCfg(nEnctype);
}

//##ModelId=49BDB398039A
int CChannelManager::Record_GetPreTime(int nCh)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCRecord[nCh]->GetPreTime();
}

//##ModelId=49BDB39803B8
int CChannelManager::Record_SetPreTime(int nCh, int nTime)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCRecord[nCh]->SetPreTime(nTime);
}

//##ModelId=49BDB399002A
int CChannelManager::Record_GetDelayTime(int nCh)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCRecord[nCh]->GetDelayTime();
}

//##ModelId=49BDB3990048
int CChannelManager::Record_SetDelayTime(int nCh, int nTime)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCRecord[nCh]->SetDelayTime(nTime);
}

//##ModelId=49BDB39900A2
int CChannelManager::Record_GetSchedule(int nCh, tagRecordSchedule * cfg)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	if (NULL == cfg)
	{
		AVL_ERR("[CChannelManager:%s]cfg==NULL,ch=%d\n", __FUNCTION__, nCh);
		return -1;
	}
	return (int) theCRecord[nCh]->GetSchedule(cfg);
}

//##ModelId=49BDB39900FC
int CChannelManager::Record_SetSchedule(int nCh, tagRecordSchedule * cfg)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	if (NULL == cfg)
	{
		AVL_ERR("[CChannelManager:%s]cfg==NULL,ch=%d\n", __FUNCTION__, nCh);
		return -1;
	}
	return (int) theCRecord[nCh]->SetSchedule(cfg);
}

int CChannelManager::Record_GetNewSchedule(tagRecordScheduleV2 *cfg)
{   
    if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if (NULL == cfg)
	{
		AVL_ERR("[CChannelManager:%s]cfg==NULL\n", __FUNCTION__);
		return -1;
	}
	return (int) theCRecord[0]->GetScheduleV2(cfg);
}

int CChannelManager::Record_SetNewSchedule(int nCh, tagRecordScheduleV2 *cfg)
{    
    if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	if (NULL == cfg)
	{
		AVL_ERR("[CChannelManager:%s]cfg==NULL,ch=%d\n", __FUNCTION__, nCh);
		return -1;
	}
	return (int) theCRecord[nCh]->SetScheduleV2(cfg);
}

//##ModelId=49BDB3990160
int CChannelManager::Record_SetRecordCustomMode(int nCustomMode)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if (nCustomMode >> 1)
	{
		AVL_ERR("[CChannelManager:%s]nCustomMode=%d\n", __FUNCTION__, nCustomMode);
		return -1;
	}
	return (int) CRecord::SetRecordCustomMode(nCustomMode);
}

//##ModelId=49BDB39901C4
int CChannelManager::Record_GetRecordCustomMode(int *nCustomMode)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if (NULL == nCustomMode)
	{
		AVL_ERR("[CChannelManager:%s]nCustomMode==NULL\n", __FUNCTION__);
		return -1;
	}
	return (int) CRecord::GetRecordCustomMode(nCustomMode);
}

int CChannelManager::StopRecordAndUinitMemRecord(int nCh)
{
	if (!m_bInit)
	{
		AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	
	return (int) theCRecord[nCh]->StopRecordAndUinitMemRecord();    
}

//!\brief 移动触发报警录像
//!\return  成功为0，失败为-1
int CChannelManager::Record_SetMotionAlarmRec(int nCh)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return theCRecord[nCh]->Motion_SetAlarmRec();
}

//!\brief 智能检测报警录像
//!\return	成功为0，失败为-1
int CChannelManager::Record_SetAiAlarmRec(int nCh)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return theCRecord[nCh]->Ai_SetAlarmRec();
}

//!\brief 报警输入触发报警录像
//!\return  成功为0，失败为-1
int CChannelManager::Record_SetAlarmInAlarmRec(int nCh)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return theCRecord[nCh]->AlarmIn_SetAlarmRec();

}

//##ModelId=49BDB54F01BA
int CChannelManager::OSD_GetType(int nCh, int *nShowType)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	if (NULL == nShowType)
	{
		AVL_ERR("[CChannelManager:%s]nShowType==NULL,ch=%d\n", __FUNCTION__, nCh);
		return -1;
	}
	return (int) theCOsd[nCh]->GetType(nShowType);
}

//##ModelId=49BDB54F0264
int CChannelManager::SetAlarmCallBack(int AlarmType, int (*CallBack) (struct tagAlertInfo * pAlertInfo, struct tagActionInfo * pActionInfo))
{

	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	switch (AlarmType)
	{
		case ALERT_TYPE_MOTION:
			CMotion::SetAlarmCallBack(AlarmType, CallBack);
			break;
		case ALERT_TYPE_HUMANSHAPE:
			CHumanShape::SetAlarmCallBack(AlarmType, CallBack);
			break;
		case ALERT_TYPE_SOUNDDETECT:
			CSoundDetectManager::SetAlarmCallBack(AlarmType, CallBack);
			break;
		case ALERT_TYPE_ENCODEERROR:
			CCoder::SetAlarmCallBack(AlarmType, CallBack);
			break;
		default:
			return -1;
			break;
	}

	m_AlarmCallBack = CallBack;

	return (int) 0;
}

//!\brief 启用某个报警联动动作
//!\param ch 通道号
//!\param AlarmType //!报警输入
//    ALERT_TYPE_ALARMIN,
//   //!移动侦测
//    ALERT_TYPE_MOTION,
//!编码异常
//    ALERT_TYPE_ENCODEERROR,
//!硬盘错误
//    ALERT_TYPE_DISKERROR,
//!硬盘满
//    ALERT_TYPE_DISKFULL,
//!IP冲突
//    ALERT_TYPE_IPCONFLICT,
//!非法访问
//     ALERT_TYPE_ILLEGEACCESS,
//!网线断
//     ALERT_TYPE_RETICLEDISCONNECT,
//!视频丢失
//     ALERT_TYPE_VIDEOLOST,
//!视频异常
//     ALERT_TYPE_VIDEOEXCEPTION,
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
int CChannelManager::EnableAction(int nCh, int AlarmType, int Action, int bEnable)
{
	int result = 0;

	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	switch (AlarmType)
	{
		case ALERT_TYPE_MOTION:
			result = theCMotion[nCh]->EnableAction(Action, bEnable);
			break;
		case ALERT_TYPE_HUMANSHAPE:
			result = theCHumanShape[nCh]->EnableAction(Action, bEnable);
			break;
		case ALERT_TYPE_ENCODEERROR:
			result = theCCoder[nCh]->EnableAction(Action, bEnable);
			break;
		default:
			AVL_ERR("[CChannelManager:%s]AlarmType = %d unsupported\n", __FUNCTION__, AlarmType);
			return -1;
			break;
	}
	return result;
}

int CChannelManager::AreaDefense_EnableAction(int nCh, int Action, int bEnable)
{
	int result;

	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	
	theCMotion[nCh]->EnableAction(Action, bEnable);
	theCHumanShape[nCh]->EnableAction(Action, bEnable);
	result = theCAreaDefense[nCh]->EnableAction(Action, bEnable);

	return result;
}

//!\brief 判断某个报警联动动作是否开启
//!\param ch 通道号
//!\param AlarmType //!报警输入
//    ALERT_TYPE_ALARMIN,
//   //!移动侦测
//    ALERT_TYPE_MOTION,
//!编码异常
//    ALERT_TYPE_ENCODEERROR,
//!硬盘错误
//    ALERT_TYPE_DISKERROR,
//!硬盘满
//    ALERT_TYPE_DISKFULL,
//!IP冲突
//    ALERT_TYPE_IPCONFLICT,
//!非法访问
//     ALERT_TYPE_ILLEGEACCESS,
//!网线断
//     ALERT_TYPE_RETICLEDISCONNECT,
//!视频丢失
//     ALERT_TYPE_VIDEOLOST,
//!视频异常
//     ALERT_TYPE_VIDEOEXCEPTION,
//!\praram int Action    !      ACTION_SCREEN 屏幕显示
//  !      ACTION_BEEP 声音提示
//  !      ACTION_ALARMOUT 报警输出
//  !      ACTION_SENDTOSERVER 上传报警服务器
//  !      ACTION_EMAIL 联动Email
//  !      ACTION_SMS 联动短信
//!\return false 为禁止联动 true 为开启联动,
//对于联动屏幕显示0为不启用，1为联动主显,2为联动辅显,3为联动主显和辅显
//##ModelId=49BDEFDF0081
int CChannelManager::IsEnableAction(int nCh, int AlarmType, int Action)
{
	int result = 0;

	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt) 
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	switch (AlarmType)
	{
		case ALERT_TYPE_MOTION:
			result = theCMotion[nCh]->IsEnableAction(Action);
			break;
		case ALERT_TYPE_HUMANSHAPE:
			result = theCHumanShape[nCh]->IsEnableAction(Action);
			break;
		case ALERT_TYPE_SOUNDDETECT:
			result |= theCSoundDetect[nCh]->IsEnableAction(Action);
			break;
		case ALERT_TYPE_ENCODEERROR:
			result = theCCoder[nCh]->IsEnableAction(Action);
			break;
		default:
			AVL_ERR("[CChannelManager:%s]AlarmType = %d unsupported\n", __FUNCTION__, AlarmType);
			return -1;
			break;
	}
	return result;
}

int CChannelManager::AreaDefense_IsEnableAction(int nCh, int Action)
{
	int result;

	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	//result = theCAreaDefense[nCh]->IsEnableAction(Action);
	result = theCHumanShape[nCh]->IsEnableAction(Action);//解决布防区域设置时，导致人形检测联动被关闭的问题（CChannelManager::EnableAction中已将布防区域的联动同步到人形检测）
	return result;
}

//!\brief 获取某个报警联动录像参数
//!\param ch 通道号
//!\param AlarmType //!报警输入
//    ALERT_TYPE_ALARMIN,
//   //!移动侦测
//    ALERT_TYPE_MOTION,
//!编码异常
//    ALERT_TYPE_ENCODEERROR,
//!硬盘错误
//    ALERT_TYPE_DISKERROR,
//!硬盘满
//    ALERT_TYPE_DISKFULL,
//!IP冲突
//    ALERT_TYPE_IPCONFLICT,
//!非法访问
//     ALERT_TYPE_ILLEGEACCESS,
//!网线断
//     ALERT_TYPE_RETICLEDISCONNECT,
//!视频丢失
//     ALERT_TYPE_VIDEOLOST,
//!视频异常
//     ALERT_TYPE_VIDEOEXCEPTION,
//!\param dwChannelFlag 联动的通道标记 
//每一位表示联动一个通道 1表示联动通道1 
//2表示联动通道2录像 3表示联动通道1和2录像,以此类推
//!\param nPort 某个报警通道号
//!\return 0表示获取成功，-1表示获取失败
//##ModelId=49B7187602BF
int CChannelManager::GetRecordParam(int nCh, int AlarmType, int *dwChannelFlag)
{
	int result = 0;

	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	switch (AlarmType)
	{
		case ALERT_TYPE_MOTION:
			result = theCMotion[nCh]->GetRecordParam(dwChannelFlag);
			break;
		case ALERT_TYPE_HUMANSHAPE:
			result = theCHumanShape[nCh]->GetRecordParam(dwChannelFlag);
			break;
		case ALERT_TYPE_ENCODEERROR:
			result = theCCoder[nCh]->GetRecordParam(dwChannelFlag);
			break;
		default:
			AVL_ERR("[CChannelManager:%s]AlarmType = %d unsupported\n", __FUNCTION__, AlarmType);
			return -1;
			break;
	}
	return result;
}

int CChannelManager::AreaDefense_GetRecordParam(int nCh, int *dwChannelFlag)
{
	int result;

	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	result = theCAreaDefense[nCh]->GetRecordParam(dwChannelFlag);
	return result;
}

//!\brief 设置某个报警联动录像参数
//!\param ch 通道号
//!\param AlarmType //!报警输入
//    ALERT_TYPE_ALARMIN,
//   //!移动侦测
//    ALERT_TYPE_MOTION,
//!编码异常
//    ALERT_TYPE_ENCODEERROR,
//!硬盘错误
//    ALERT_TYPE_DISKERROR,
//!硬盘满
//    ALERT_TYPE_DISKFULL,
//!IP冲突
//    ALERT_TYPE_IPCONFLICT,
//!非法访问
//     ALERT_TYPE_ILLEGEACCESS,
//!网线断
//     ALERT_TYPE_RETICLEDISCONNECT,
//!视频丢失
//     ALERT_TYPE_VIDEOLOST,
//!视频异常
//     ALERT_TYPE_VIDEOEXCEPTION,
//!\param dwChannelFlag 联动录像通道标记
//!\param nPort 某个报警通道号
//!\return 0表示设置成功 -1表示设置失败
//##ModelId=49B718640232
int CChannelManager::SetRecordParam(int nCh, int AlarmType, int dwChannelFlag)
{
	int result = 0;

	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	switch (AlarmType)
	{
		case ALERT_TYPE_MOTION:
			result = theCMotion[nCh]->SetRecordParam(dwChannelFlag);
			break;
		case ALERT_TYPE_HUMANSHAPE:
			result = theCHumanShape[nCh]->SetRecordParam(dwChannelFlag);
			break;
		case ALERT_TYPE_ENCODEERROR:
			result = theCCoder[nCh]->SetRecordParam(dwChannelFlag);
			break;
		default:
			AVL_ERR("[CChannelManager:%s]AlarmType = %d unsupported\n", __FUNCTION__, AlarmType);
			return -1;
			break;
	}
	return result;
}

int CChannelManager::AreaDefense_SetRecordParam(int nCh, int dwChannelFlag)
{
	int result;

	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	theCMotion[nCh]->SetRecordParam(dwChannelFlag);
	theCHumanShape[nCh]->SetRecordParam(dwChannelFlag);
	result = theCAreaDefense[nCh]->SetRecordParam(dwChannelFlag);
	return result;
}

//!\brief 
//获取某个报警联动拍照参数,拍照张数和间隔由CCapturePic::GetCfg获取
//!\param ch 通道号
//!\param AlarmType //!报警输入
//    ALERT_TYPE_ALARMIN,
//   //!移动侦测
//    ALERT_TYPE_MOTION,
//!编码异常
//    ALERT_TYPE_ENCODEERROR,
//!硬盘错误
//    ALERT_TYPE_DISKERROR,
//!硬盘满
//    ALERT_TYPE_DISKFULL,
//!IP冲突
//    ALERT_TYPE_IPCONFLICT,
//!非法访问
//     ALERT_TYPE_ILLEGEACCESS,
//!网线断
//     ALERT_TYPE_RETICLEDISCONNECT,
//!视频丢失
//     ALERT_TYPE_VIDEOLOST,
//!视频异常
//     ALERT_TYPE_VIDEOEXCEPTION,
//!\param dwChannelFlag 存储联动拍照通道标记的缓冲区
//!\param nPort 报警联动输入通道号
//!\return 0表示成功 -1表示失败
//##ModelId=49B5CE9A030D
int CChannelManager::GetSnapshotParam(int nCh, int AlarmType, int *dwChannelFlag)
{
	int result = 0;

	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	switch (AlarmType)
	{
		case ALERT_TYPE_MOTION:
			result = theCMotion[nCh]->GetSnapshotParam(dwChannelFlag);
			break;
		case ALERT_TYPE_HUMANSHAPE:
			result = theCHumanShape[nCh]->GetSnapshotParam(dwChannelFlag);
			break;
		case ALERT_TYPE_ENCODEERROR:
			result = theCCoder[nCh]->GetSnapshotParam(dwChannelFlag);
			break;
		default:
			AVL_ERR("[CChannelManager:%s]AlarmType = %d unsupported\n", __FUNCTION__, AlarmType);
			return -1;
			break;
	}
	return result;
}

int CChannelManager::AreaDefense_GetSnapshotParam(int nCh, int *dwChannelFlag)
{
	int result;

	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	result = theCAreaDefense[nCh]->GetSnapshotParam(dwChannelFlag);
	return result;
}

//!\brief 
//设置某个报警联动拍照参数，拍照张数和拍照间隔由CCapturePi
//c::SetCfg设定
//!\param ch 通道号
//!\param AlarmType //!报警输入
//    ALERT_TYPE_ALARMIN,
//   //!移动侦测
//    ALERT_TYPE_MOTION,
//!编码异常
//    ALERT_TYPE_ENCODEERROR,
//!硬盘错误
//    ALERT_TYPE_DISKERROR,
//!硬盘满
//    ALERT_TYPE_DISKFULL,
//!IP冲突
//    ALERT_TYPE_IPCONFLICT,
//!非法访问
//     ALERT_TYPE_ILLEGEACCESS,
//!网线断
//     ALERT_TYPE_RETICLEDISCONNECT,
//!视频丢失
//     ALERT_TYPE_VIDEOLOST,
//!视频异常
//     ALERT_TYPE_VIDEOEXCEPTION,
//!\param dwChannelFlag 联动拍照通道标记
//!\param nPort 某个报警通道号
//!\return 0表示设置成功 -1表示失败
//##ModelId=49B5CDFE00BB
int CChannelManager::SetSnapshotParam(int nCh, int AlarmType, int dwChannelFlag)
{
	int result = 0;

	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	switch (AlarmType)
	{
		case ALERT_TYPE_MOTION:
			result = theCMotion[nCh]->SetSnapshotParam(dwChannelFlag);
			break;
		case ALERT_TYPE_HUMANSHAPE:
			result = theCHumanShape[nCh]->SetSnapshotParam(dwChannelFlag);
			break;
		case ALERT_TYPE_ENCODEERROR:
			result = theCCoder[nCh]->SetSnapshotParam(dwChannelFlag);
			break;
		default:
			AVL_ERR("[CChannelManager:%s]AlarmType = %d unsupported\n", __FUNCTION__, AlarmType);
			return -1;
			break;
	}
	return result;
}

int CChannelManager::AreaDefense_SetSnapshotParam(int nCh, int dwChannelFlag)
{
	int result;

	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	theCMotion[nCh]->SetSnapshotParam(dwChannelFlag);
	theCHumanShape[nCh]->SetSnapshotParam(dwChannelFlag);
	result = theCAreaDefense[nCh]->SetSnapshotParam(dwChannelFlag);
	return result;
}

//!\brief 获取某一某个报警通道联动某一云台的动作参数
//!\param ch 通道号
//!\param AlarmType //!报警输入
//    ALERT_TYPE_ALARMIN,
//   //!移动侦测
//    ALERT_TYPE_MOTION,
//!编码异常
//    ALERT_TYPE_ENCODEERROR,
//!硬盘错误
//    ALERT_TYPE_DISKERROR,
//!硬盘满
//    ALERT_TYPE_DISKFULL,
//!IP冲突
//    ALERT_TYPE_IPCONFLICT,
//!非法访问
//     ALERT_TYPE_ILLEGEACCESS,
//!网线断
//     ALERT_TYPE_RETICLEDISCONNECT,
//!视频丢失
//     ALERT_TYPE_VIDEOLOST,
//!视频异常
//     ALERT_TYPE_VIDEOEXCEPTION,
//!\param nAction 联动动作 
//*nAction=0表示无动作，1联动预置点，2联动巡航，3联动轨迹
//!\param nIndex 联动预置点/巡航/轨迹的编号
//!\param nChannel 云台对应的通道号
//!\param nPort 某个报警通道号
//!\return 0表示获取成功, -1表示获取失败
//##ModelId=49B718B202DE

int CChannelManager::GetPtzParam(int nCh, int AlarmType, int nChannel, int *nAction, int *nIndex)
{
	int result = 0;

	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	switch (AlarmType)
	{
		case ALERT_TYPE_MOTION:
			result = theCMotion[nCh]->GetPtzParam(nChannel, nAction, nIndex);
			break;
		case ALERT_TYPE_HUMANSHAPE:
			result = theCHumanShape[nCh]->GetPtzParam(nChannel, nAction, nIndex);
			break;
		case ALERT_TYPE_ENCODEERROR:
			result = theCCoder[nCh]->GetPtzParam(nChannel, nAction, nIndex);
			break;
		default:
			AVL_ERR("[CChannelManager:%s]AlarmType = %d unsupported\n", __FUNCTION__, AlarmType);
			return -1;
			break;
	}
	return result;
}

int CChannelManager::AreaDefense_GetPtzParam(int nCh, int nChannel, int *nAction, int *nIndex)
{
	int result;

	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	result = theCAreaDefense[nCh]->GetPtzParam(nChannel, nAction, nIndex);
	return result;
}

//!\brief 设置某个报警通道联动某一个云台的动作
//!\param ch 通道号
//!\param AlarmType //!报警输入
//    ALERT_TYPE_ALARMIN,
//   //!移动侦测
//    ALERT_TYPE_MOTION,
//!编码异常
//    ALERT_TYPE_ENCODEERROR,
//!硬盘错误
//    ALERT_TYPE_DISKERROR,
//!硬盘满
//    ALERT_TYPE_DISKFULL,
//!IP冲突
//    ALERT_TYPE_IPCONFLICT,
//!非法访问
//     ALERT_TYPE_ILLEGEACCESS,
//!网线断
//     ALERT_TYPE_RETICLEDISCONNECT,
//!视频丢失
//     ALERT_TYPE_VIDEOLOST,
//!视频异常
//     ALERT_TYPE_VIDEOEXCEPTION,
//!\param nAction 联动的动作 0表示无动作 1联动预置点 
//2联动巡航 3联动轨迹
//!\param nIndex 联动的预置点/巡航/轨迹的编号
//!\param nChannel 云台对应的通道号
//!\param nPort 某个报警通道号
//!\return 0表示成功 -1表示失败
//##ModelId=49B718BD0261
int CChannelManager::SetPtzParam(int nCh, int AlarmType, int nChannel, int nAction, int nIndex)
{
	int result = 0;

	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	switch (AlarmType)
	{
		case ALERT_TYPE_MOTION:
			result = theCMotion[nCh]->SetPtzParam(nChannel, nAction, nIndex);
			break;
		case ALERT_TYPE_HUMANSHAPE:
			result = theCHumanShape[nCh]->SetPtzParam(nChannel, nAction, nIndex);
			break;
		case ALERT_TYPE_ENCODEERROR:
			result = theCCoder[nCh]->SetPtzParam(nChannel, nAction, nIndex);
			break;
		default:
			AVL_ERR("[CChannelManager:%s]AlarmType = %d unsupported\n", __FUNCTION__, AlarmType);
			return -1;
			break;
	}
	return result;
}

int CChannelManager::AreaDefense_SetPtzParam(int nCh, int nChannel, int nAction, int nIndex)
{
	int result;

	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	theCMotion[nCh]->SetPtzParam(nChannel, nAction, nIndex);
	theCHumanShape[nCh]->SetPtzParam(nChannel, nAction, nIndex);
	result = theCAreaDefense[nCh]->SetPtzParam(nChannel, nAction, nIndex);
	return result;
}

//!\brief 获取某个报警联动报警输出的参数
//!\param ch 通道号
//!\param AlarmType //!报警输入
//    ALERT_TYPE_ALARMIN,
//   //!移动侦测
//    ALERT_TYPE_MOTION,
//!编码异常
//    ALERT_TYPE_ENCODEERROR,
//!硬盘错误
//    ALERT_TYPE_DISKERROR,
//!硬盘满
//    ALERT_TYPE_DISKFULL,
//!IP冲突
//    ALERT_TYPE_IPCONFLICT,
//!非法访问
//     ALERT_TYPE_ILLEGEACCESS,
//!网线断
//     ALERT_TYPE_RETICLEDISCONNECT,
//!视频丢失
//     ALERT_TYPE_VIDEOLOST,
//!视频异常
//     ALERT_TYPE_VIDEOEXCEPTION,
//!\param dwOutPortFlag 存储输出通道标记的缓冲区
//!\param nPort 某个报警通道号
//!\return 0表示成功 -1表示失败
//##ModelId=49B718DD030D
int CChannelManager::GetDopParam(int nCh, int AlarmType, int *dwOutPortFlag)
{
	int result = 0;

	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	switch (AlarmType)
	{
		case ALERT_TYPE_MOTION:
			result = theCMotion[nCh]->GetDopParam(dwOutPortFlag);
			break;
		case ALERT_TYPE_HUMANSHAPE:
			result = theCHumanShape[nCh]->GetDopParam(dwOutPortFlag);
			break;
		case ALERT_TYPE_ENCODEERROR:
			result = theCCoder[nCh]->GetDopParam(dwOutPortFlag);
			break;
		default:
			AVL_ERR("[CChannelManager:%s]AlarmType = %d unsupported\n", __FUNCTION__, AlarmType);
			return -1;
			break;
	}
	return result;
}

int CChannelManager::AreaDefense_GetDopParam(int nCh, int *dwOutPortFlag)
{
	int result;

	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	result = theCAreaDefense[nCh]->GetDopParam(dwOutPortFlag);
	return result;
}

//!\brief 设置某个报警联动报警输出的参数
//!\param ch 通道号
//!\param AlarmType //!报警输入
//    ALERT_TYPE_ALARMIN,
//   //!移动侦测
//    ALERT_TYPE_MOTION,
//!编码异常
//    ALERT_TYPE_ENCODEERROR,
//!硬盘错误
//    ALERT_TYPE_DISKERROR,
//!硬盘满
//    ALERT_TYPE_DISKFULL,
//!IP冲突
//    ALERT_TYPE_IPCONFLICT,
//!非法访问
//     ALERT_TYPE_ILLEGEACCESS,
//!网线断
//     ALERT_TYPE_RETICLEDISCONNECT,
//!视频丢失
//     ALERT_TYPE_VIDEOLOST,
//!视频异常
//     ALERT_TYPE_VIDEOEXCEPTION,s
//!\param dwOutPortFlag 设置输出通道标记
//!\param nPort 某个报警通道号
//!\return 0表示成功 -1表示失败
//##ModelId=49B718ED00AB
int CChannelManager::SetDopParam(int nCh, int AlarmType, int dwOutPortFlag)
{
	int result = 0;

	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	switch (AlarmType)
	{
		case ALERT_TYPE_MOTION:
			result = theCMotion[nCh]->SetDopParam(dwOutPortFlag);
			break;
		case ALERT_TYPE_HUMANSHAPE:
			result = theCHumanShape[nCh]->SetDopParam(dwOutPortFlag);
			break;
		case ALERT_TYPE_ENCODEERROR:
			result = theCCoder[nCh]->SetDopParam(dwOutPortFlag);
			break;
		default:
			AVL_ERR("[CChannelManager:%s]AlarmType = %d unsupported\n", __FUNCTION__, AlarmType);
			return -1;
			break;
	}
	return result;
}

int CChannelManager::AreaDefense_SetDopParam(int nCh, int dwOutPortFlag)
{
	int result;

	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	theCMotion[nCh]->SetDopParam(dwOutPortFlag);
	theCHumanShape[nCh]->SetDopParam(dwOutPortFlag);
	result = theCAreaDefense[nCh]->SetDopParam(dwOutPortFlag);
	return result;
}

int CChannelManager::GetPushEventIntvCnt(int nCh, int AlarmType, int *nPushEventIntv)
{
	int result;

	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	switch (AlarmType)
	{
		case ALERT_TYPE_MOTION:
			result = theCMotion[nCh]->GetPushEventIntvCnt(nPushEventIntv);
			break;
		case ALERT_TYPE_HUMANSHAPE:
			result = theCHumanShape[nCh]->GetPushEventIntvCnt(nPushEventIntv);
			break;
        case ALERT_TYPE_SOUNDDETECT:
			result = theCSoundDetect[nCh]->GetPushEventIntvCnt(nPushEventIntv);
			break;
		default:
			AVL_ERR("[CChannelManager:%s]AlarmType = %d unsupported\n", __FUNCTION__, AlarmType);
			return -1;
	}

	return result;
}

int CChannelManager::SetPushEventIntvCnt(int nCh, int AlarmType, int nPushEventIntv)
{
	int result;
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	switch (AlarmType)
	{
		case ALERT_TYPE_MOTION:
			result = theCMotion[nCh]->SetPushEventIntvCnt(nPushEventIntv);
			break;

		case ALERT_TYPE_HUMANSHAPE:
			result = theCHumanShape[nCh]->SetPushEventIntvCnt(nPushEventIntv);
			break;

        case ALERT_TYPE_SOUNDDETECT:
			result = theCSoundDetect[nCh]->SetPushEventIntvCnt(nPushEventIntv);
			break;

		default:
			AVL_ERR("[CChannelManager:%s]AlarmType = %d unsupported\n", __FUNCTION__, AlarmType);
			return -1;
	}

	return result;
}

//##ModelId=49BDB54F03AF
int CChannelManager::OSD_GetTimeFormat(int nCh, int *nShowFortMat)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	if (NULL == nShowFortMat)
	{
		AVL_ERR("[CChannelManager:%s]nShowFortMat==NULL,ch=%d\n", __FUNCTION__, nCh);
		return -1;
	}
	return (int) theCOsd[nCh]->GetTimeFormat(nShowFortMat);
}

//!\brief 获取时间点阵宽高
//!\praram int* pWidth,int *pHeight
//时间格式。根据底层的实际情况来确定可以设置哪些格式
//!\return  成功为0，失败为-1
//##ModelId=49B767A50356
int CChannelManager::OSD_GetTimeWH(int nCh, int *pWidth, int *pHeight)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	if (NULL == pWidth || NULL == pHeight)
	{
		AVL_ERR("[CChannelManager:%s]NULL == pWidth || NULL == pHeight\n", __FUNCTION__);
		return -1;
	}
	return (int) theCOsd[nCh]->GetTimeWH(pWidth, pHeight);
}

//##ModelId=49BDB550007B
int CChannelManager::OSD_GetCharBitMap(int nCh, char *CharBitMap, int buffSize)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}


	return (int) theCOsd[nCh]->GetCharBitMap(CharBitMap, buffSize);
}

//##ModelId=49BDB550012F
int CChannelManager::OSD_SetType(int nCh, int nShowType)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCOsd[nCh]->SetType(nShowType);
}

//##ModelId=49BDB55001EE
int CChannelManager::OSD_SetPosition(int nCh, int OSD_TYPE, int x, int y)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCOsd[nCh]->SetPosition(OSD_TYPE, x, y);
}

//!\brief 设置时间或者字符的位置
//!\param int OSD_TYPE OSD类型 1-文本,2-时间,3-时间+文本
//!\param int Time_x, int Time_y, int Text_x, int Text_y时间与文件的位置
//!\return  成功为0，失败为-1
int CChannelManager::OSD_SetPositionAll(int nCh, int Time_x, int Time_y, int Text_x, int Text_y, int OSD_TYPE)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCOsd[nCh]->SetPositionAll(Time_x, Time_y, Text_x, Text_y, OSD_TYPE);
}

//##ModelId=49BDB550034C
int CChannelManager::OSD_SetTimeFormat(int nCh, int nShowFortMat)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCOsd[nCh]->SetTimeFormat(nShowFortMat);
}

//##ModelId=49BDB551002C
int CChannelManager::OSD_SetCharBitMap(int nCh, char *CharBitMap, unsigned char *byLattice ,int FontSize, int Width, int Height)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	if (NULL == CharBitMap)
	{
		AVL_ERR("[CChannelManager:%s]CharBitMap==NULL,ch=%d\n", __FUNCTION__, nCh);
		return -1;
	}
	return (int) theCOsd[nCh]->SetCharBitMap(CharBitMap,byLattice,FontSize,Width,Height);
}

int CChannelManager::OSD_SetTextEx(char *OSDToken, unsigned short *strText, int CharSet)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if ((NULL == OSDToken) || (NULL == strText))
	{
		AVL_ERR("[CChannelManager:%s]OSDToken==NULL, OSDToken==NULL\n", __FUNCTION__);
		return -1;
	}
	
	return (int) theCOsd[0]->OSD_SetTextEx(OSDToken, strText, CharSet);
}

int CChannelManager::OSD_SetTextOSD_Cprocustom(char *unicde)
{
		
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if (NULL == unicde)
	{
		AVL_ERR("[CChannelManager:%s]OSDToken==NULL, OSDToken==NULL\n", __FUNCTION__);
		return -1;
	}

	return (int) theCOsd[0]->SetTextOSD_Cprocustom(unicde);
}

int CChannelManager::OSD_SetOSDV2(int index, int bEnable, char *OSDToken, unsigned short *strText, int nX, int nY)
{
	int enable = 0;
	int nStatisticalResults = 0;
		
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if ((NULL == OSDToken) || (NULL == strText))
	{
		AVL_ERR("[CChannelManager:%s]OSDToken==NULL, OSDToken==NULL\n", __FUNCTION__);
		return -1;
	}

	return (int) theCOsd[0]->OSD_SetOSDV2(index, bEnable, OSDToken, strText, nX, nY, enable & nStatisticalResults);
}

int CChannelManager::OSD_GetOSDV2TextByNum(int i, int *bEnable, char *OSDToken, unsigned short *strText, int *nX, int *nY)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if ((bEnable == NULL) || (NULL == OSDToken) || (NULL == strText) || (NULL == nX) || (NULL == nY))
	{
		AVL_ERR("[CChannelManager:%s]OSDToken==NULL, OSDToken==NULL\n", __FUNCTION__);
		return -1;
	}
	
	return (int) theCOsd[0]->OSD_GetOSDV2TextByNum(i, bEnable, OSDToken, strText, nX, nY);
}

int CChannelManager::OSD_GetTextExByToken(char *OSDToken, unsigned short *strText, int *CharSet)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if ((NULL == OSDToken) || (NULL == strText) || (NULL == CharSet))
	{
		AVL_ERR("[CChannelManager:%s]OSDToken==NULL, OSDToken==NULL\n", __FUNCTION__);
		return -1;
	}
	
	return (int) theCOsd[0]->OSD_GetTextExByToken(OSDToken, strText, CharSet);
}

int CChannelManager::OSD_GetTextExByNum(int i, char *OSDToken, int tokenSize, unsigned short *strText, int *CharSet)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if ((NULL == OSDToken) || (NULL == strText) || (NULL == CharSet))
	{
		AVL_ERR("[CChannelManager:%s]OSDToken==NULL, OSDToken==NULL\n", __FUNCTION__);
		return -1;
	}
	
	return (int) theCOsd[0]->OSD_GetTextExByNum(i, OSDToken, tokenSize, strText, CharSet);
}

int CChannelManager::SetAdvanceEncParam(int nCh, int NormalAdvEncType, int EventAdvEncType, tagVIDEO_SHELTER *pCfg)
{
	if (!m_bInit)
	{
		AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	if (pCfg == NULL)
	{
		AVL_ERR("[CChannelManager:%s]cfg == NULL, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCRecord[nCh]->SetAdvanceEnc(NormalAdvEncType, EventAdvEncType, pCfg);
}

int CChannelManager::GetAdvanceEncParam(int nCh, int *NormalAdvEncType, int *EventAdvEncType, tagVIDEO_SHELTER *pCfg)
{
	if (!m_bInit)
	{
		AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	if (pCfg == NULL)
	{
		AVL_ERR("[CChannelManager:%s]pcfg == NULL, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCRecord[nCh]->GetAdvanceEnc(NormalAdvEncType, EventAdvEncType, pCfg);
}

int CChannelManager::SetAuxAdvEncParam(int nCh, int AuxAdvEncType)
{
	if (!m_bInit)
	{
		AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCCoder[nCh]->SetAuxAdvEnc(AuxAdvEncType);
}

int CChannelManager::GetAuxAdvEncParam(int nCh, int *pAuxAdvEncType)
{
	if (!m_bInit)
	{
		AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	if (pAuxAdvEncType == NULL)
	{
		AVL_ERR("[CChannelManager:%s]pcfg == NULL, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
    
	return (int) theCCoder[nCh]->GetAuxAdvEnc(pAuxAdvEncType);    
}

int CChannelManager::SetHDAuxAdvEncParam(int nCh, int HDAuxAdvEncType)
{
	if (!m_bInit)
	{
		AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCCoder[nCh]->SetHDAuxAdvEnc(HDAuxAdvEncType);
}

int CChannelManager::GetHDAuxAdvEncParam(int nCh, int *pHDAuxAdvEncType)
{
	if (!m_bInit)
	{
		AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	if (pHDAuxAdvEncType == NULL)
	{
		AVL_ERR("[CChannelManager:%s]pcfg == NULL, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
    
	return (int) theCCoder[nCh]->GetHDAuxAdvEnc(pHDAuxAdvEncType);    
}

int CChannelManager::SetFourthStreamAdvEncParam(int nCh, int FourthStreamAdvEncType)
{
	if (!m_bInit)
	{
		AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCCoder[nCh]->SetFourthStreamAdvEnc(FourthStreamAdvEncType);
}

int CChannelManager::GetFourthStreamAdvEncParam(int nCh, int *pFourthStreamAdvEncType)
{
	if (!m_bInit)
	{
		AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	if (pFourthStreamAdvEncType == NULL)
	{
		AVL_ERR("[CChannelManager:%s]pcfg == NULL, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
    
	return (int) theCCoder[nCh]->GetFourthStreamAdvEnc(pFourthStreamAdvEncType);    
}

int CChannelManager::SetAuxHDAdvEncParam(int nCh, int AuxAdvEncType, int HDAuxAdvEncType)
{
	if (!m_bInit)
	{
		AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCCoder[nCh]->SetAuxHDAdvEnc(AuxAdvEncType, HDAuxAdvEncType);
}

int CChannelManager::GetAuxHDAdvEncParam(int nCh, int *pAuxAdvEncType, int *pHDAuxAdvEncType)
{
	if (!m_bInit)
	{
		AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	if (pAuxAdvEncType==NULL || pHDAuxAdvEncType==NULL)
	{
		AVL_ERR("[CChannelManager:%s]pcfg == NULL, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
    
	return (int) theCCoder[nCh]->GetAuxHDAdvEnc(pAuxAdvEncType, pHDAuxAdvEncType);    
}

//!\brief 设置录像写入状态 0:暂停  1:写入
int CChannelManager::SetRecordWriteStatus(int nCh, int record_write_status)
{
    if (!m_bInit)
    {
        AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
        return -1;
    }
    if (nCh < 0 || nCh >= m_iEncChannelCnt)
    {
        AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
        return -1;
    }

    return (int) theCRecord[nCh]->SetRecordWriteStatus(record_write_status);    
}

/*@breif SetCurrentAviNumPerPage 设置每页显示多少个AVI文件
 *@param [IN]  avi_num_per_page 每页显示的AVI文件个数
 *@return 成功返回0
 *@       失败返回-1
 *@note  每页显示的AVI文件个数不能超过当前有效的AVI个数g_read_curr_avi_num，
 *@      每页显示的AVI文件个数不能超过最大能显示的AVI个数MAX_AVI_NUM_ONE_PAGE
 */
int CChannelManager::SetCurrentAviNumPerPage(int nCh, int avi_num_per_page)
{
    if (!m_bInit)
    {
        AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
        return -1;
    }
    if (nCh < 0 || nCh >= m_iEncChannelCnt)
    {
        AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
        return -1;
    }

    return (int) theCRecord[nCh]->SetCurrentAviNumPerPage(avi_num_per_page);    
}

/*@breif GetCurrentAviNumPerPage 获取显示多少个AVI文件
 *@return 成功返回AVI文件个数
 *@       失败返回-1
 */
int CChannelManager::GetCurrentAviNumPerPage(int nCh)
{
	if (!m_bInit)
	{
		AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

    return (int) theCRecord[nCh]->GetCurrentAviNumPerPage();    
}

/*@breif GetCurrentAviTotalNum 获取当前AVI总个数
 *@return 成功返回当前总的AVI文件个数
 *@       失败返回-1
 */
int CChannelManager::GetCurrentAviTotalNum(int nCh)
{
	if (!m_bInit)
	{
		AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCRecord[nCh]->GetCurrentAviTotalNum();    
}

/*@breif SetSwitchPageIsChoose 设置当前选中的是哪一个页面和当前录像路径类型
 *@param [IN]  choose_page 选中的页面(从1开始)
 *@param [IN]  path_type 当前录像路径类型 0: http路径  1:绝对路径
 *@return 成功返回0
 *@      失败返回-1
 */
int CChannelManager::SetSwitchPageIsChoose(int nCh, int choose_page, int path_type)
{
    if (!m_bInit)
    {
        AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
        return -1;
    }
    if (nCh < 0 || nCh >= m_iEncChannelCnt)
    {
        AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
        return -1;
    }

    return (int) theCRecord[nCh]->SetSwitchPageIsChoose(choose_page, path_type);    
}

/*@breif GetSwitchPageIsChoose 获取当前选中的是哪一个页面
 *@return 成功返回当前选中的是哪一个页面
 *@      失败返回-1
 */
int CChannelManager::GetSwitchPageIsChoose(int nCh)
{
    if (!m_bInit)
    {
        AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
        return -1;
    }
    if (nCh < 0 || nCh >= m_iEncChannelCnt)
    {
        AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
        return -1;
    }

    return (int) theCRecord[nCh]->GetSwitchPageIsChoose();    
}


/*@breif GetCurrentAviListInfo 从给定的页面里面获取当前AVI文件列表
 *@param [OUT] p_avi_list 获取到的AVI文件列表
 *@param [OUT] p_avi_num_curr_page 当前页面有多少个AVI文件
 *@return 成功返回0
 *@      失败返回-1
 *@note  需要先调用SetSwitchPageIsChoose设置当前选中哪一页
 *@      需要先调用SetCurrentAviNumPerPage设置当前用户需要设置每页显示多少个AVI文件
 */
int CChannelManager::GetCurrentAviListInfo(int nCh, void **p_avi_list, int *p_avi_num_curr_page)
{
	if (!m_bInit)
	{
		AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	if (p_avi_list==NULL || p_avi_num_curr_page==NULL)
	{
		AVL_ERR("[CChannelManager:%s]p_avi_list == NULL, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCRecord[nCh]->GetCurrentAviListInfo(p_avi_list, p_avi_num_curr_page);    
}

/* 设置当前要寻找的AVI的时间段 */
int CChannelManager::SetCurrentAviSearchTimeSeg(int nCh, DateTime start_time, DateTime stop_time)
{
    if (!m_bInit)
    {
        AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
        return -1;
    }
    if (nCh < 0 || nCh >= m_iEncChannelCnt)
    {
        AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
        return -1;
    }

    return (int) theCRecord[nCh]->SetCurrentAviSearchTimeSeg(start_time, stop_time);    
}

/* 设置当前要寻找的PIC的时间段 */
int CChannelManager::SetCurrentPicSearchTimeSeg(int nCh, DateTime start_time, DateTime stop_time)
{
    if (!m_bInit)
    {
        AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
        return -1;
    }
    if (nCh < 0 || nCh >= m_iEncChannelCnt)
    {
        AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
        return -1;
    }

    return (int) theCRecord[nCh]->SetCurrentPicSearchTimeSeg(start_time, stop_time);    
}

/*@breif SetCurrentPicNumPerPage 设置每页显示多少个图片文件
 *@param [IN]  pic_num_per_page 每页显示的图片文件个数
 *@return 成功返回0
 *@      失败返回-1
 *@note  每页显示的图片文件个数不能超过当前有效的图片个数g_read_curr_pic_num，
 *@      每页显示的图片文件个数不能超过最大能显示的图片个数MAX_PIC_NUM_ONE_PAGE
 */
int CChannelManager::SetCurrentPicNumPerPage(int nCh, int pic_num_per_page)
{
    if (!m_bInit)
    {
        AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
        return -1;
    }
    if (nCh < 0 || nCh >= m_iEncChannelCnt)
    {
        AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
        return -1;
    }

    return (int) theCRecord[nCh]->SetCurrentPicNumPerPage(pic_num_per_page);    
}

/*@breif GetCurrentPicNumPerPage 获取显示多少个图片文件
 *@return 成功返回图片文件个数
 *@      失败返回-1
 */
int CChannelManager::GetCurrentPicNumPerPage(int nCh)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

    return (int) theCRecord[nCh]->GetCurrentPicNumPerPage();    
}

/*@breif GetCurrentPicTotalNum 获取当前图片总个数
 *@return 成功返回当前总的图片文件个数
 *@       失败返回-1
 */
int CChannelManager::GetCurrentPicTotalNum(int nCh)
{
	if (!m_bInit)
	{
		AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCRecord[nCh]->GetCurrentPicTotalNum();    
}

/*@breif SetSwitchPicPageIsChoose 设置当前选中的是哪一个页面和当前图片路径类型
 *@param [IN]  choose_page 选中的页面(从1开始)
 *@param [IN]  path_type 当前图片路径类型 0: http路径  1:绝对路径
 *@return 成功返回0
 *@       失败返回-1
 */
int CChannelManager::SetSwitchPicPageIsChoose(int nCh, int choose_page, int path_type)
{
    if (!m_bInit)
    {
        AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
        return -1;
    }
    if (nCh < 0 || nCh >= m_iEncChannelCnt)
    {
        AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
        return -1;
    }

    return (int) theCRecord[nCh]->SetSwitchPicPageIsChoose(choose_page, path_type);    
}

/*@breif GetSwitchPicPageIsChoose 获取当前选中的是哪一个页面
 *@return 成功返回当前选中的是哪一个页面
 *@       失败返回-1
 */
int CChannelManager::GetSwitchPicPageIsChoose(int nCh)
{
    if (!m_bInit)
    {
        AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
        return -1;
    }
    if (nCh < 0 || nCh >= m_iEncChannelCnt)
    {
        AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
        return -1;
    }

    return (int) theCRecord[nCh]->GetSwitchPicPageIsChoose();    
}

/*@breif GetCurrentPicListInfo 从给定的页面里面获取当前图片文件列表
 *@param [OUT] p_pic_list 获取到的图片文件列表
 *@param [OUT] p_pic_num_curr_page 当前页面有多少个图片文件
 *@return 成功返回0
 *@       失败返回-1
 *@note   需要先调用SetSwitchpicPageIsChoose设置当前选中哪一页
 *@       需要先调用SetCurrentpicNumPerPage设置当前用户需要设置每页显示多少个图片文件
 */
int CChannelManager::GetCurrentPicListInfo(int nCh, void **p_pic_list, int *p_pic_num_curr_page)
{
	if (!m_bInit)
	{
		AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	if (p_pic_list==NULL || p_pic_num_curr_page==NULL)
	{
		AVL_ERR("[CChannelManager:%s]p_pic_list == NULL, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCRecord[nCh]->GetCurrentPicListInfo(p_pic_list, p_pic_num_curr_page);    
}


/* 获取文件对应的文件信息 */
int CChannelManager::GetRecordFileInfoByFileName(int nCh, const char* pFileName, void* pFileInfo)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCRecord[nCh]->GetRecordFileInfoByFileName(pFileName, pFileInfo);    
}

/* 获取磁盘资源占用情况(单位: MB) */
int CChannelManager::GetDiskResourceSize(int nCh, int *pRecordMB, int *pPictureMB)
{
    if (!m_bInit)
    {
        AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
        return -1;
    }
    if (nCh < 0 || nCh >= m_iEncChannelCnt)
    {
        AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
        return -1;
    }

    return (int) theCRecord[nCh]->GetDiskResourceSize(pRecordMB, pPictureMB);    
}

/* 设置录像存放的比例(范围0~100) */
int CChannelManager::SetVideoUsedSpaceRate(int nCh, int nVideoSpace)
{
    if (!m_bInit)
    {
        AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
        return -1;
    }
    if (nCh < 0 || nCh >= m_iEncChannelCnt)
    {
        AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
        return -1;
    }

    return (int) theCRecord[nCh]->SetVideoUsedSpaceRate(nVideoSpace);    
}

/* 获取录像存放的比例(范围0~100) */
int CChannelManager::GetVideoUsedSpaceRate(int nCh)
{
    if (!m_bInit)
    {
        AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
        return -1;
    }
    if (nCh < 0 || nCh >= m_iEncChannelCnt)
    {
        AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
        return -1;
    }

    return (int) theCRecord[nCh]->GetVideoUsedSpaceRate();    
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
int CChannelManager::GetFileReadWriteStatus(int nCh, 
                                                   const char *file_name, 
                                                   unsigned int *read_write_status,
                                                   unsigned int *file_size)
{
    if (!m_bInit)
    {
        AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
        return -1;
    }
    if (nCh < 0 || nCh >= m_iEncChannelCnt)
    {
        AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
        return -1;
    }

    return (int) theCRecord[nCh]->GetFileReadWriteStatus(file_name, read_write_status, file_size);   
}

int CChannelManager::SetGetTimeOffsetCallBack(int nCh, 
                                                           int (*CallBack)(int *, int *, int *))
{
    if (!m_bInit)
    {
        AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
        return -1;
    }
    if (nCh < 0 || nCh >= m_iEncChannelCnt)
    {
        AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
        return -1;
    }

    return (int) theCRecord[nCh]->SetGetTimeOffsetCallBack(CallBack);
}

/* 传入的图片名字组成pic_name: 时间戳_标号，如:1532477851_01需要改为1532477851 */
int CChannelManager::UploadPictureToSdCard(int nCh, const char *pic_name, unsigned int trig_type)
{
    if (!m_bInit)
    {
        AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
        return -1;
    }
    if (nCh < 0 || nCh >= m_iEncChannelCnt)
    {
        AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
        return -1;
    }

    if (trig_type < 0 || trig_type > 0x7)
    {
        AVL_ERR("trig_type = %d out of [0, 7]\n", trig_type);
        return -1;
    }

    return (int) theCRecord[nCh]->UploadPictureToSdCard(pic_name, trig_type);
}

int CChannelManager::HttpTriggerRecord(int nCh)
{
	if (!m_bInit)
	{
		AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	
	return (int) theCRecord[nCh]->HttpTriggerRecord();    
}

int CChannelManager::SetTrigRecordOverCallBack(int nCh, int (*CallBack)(unsigned char *))
{
	if (!m_bInit)
	{
		AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	
	return (int) theCRecord[nCh]->SetTrigRecordOverCallBack(nCh, CallBack);    
}

//##ModelId=49BDC3C10352
int CChannelManager::OSD_GetPosition(int nCh, int OSD_TYPE, int *x, int *y)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	if (NULL == x || NULL == y)
	{
		AVL_ERR("[CChannelManager:%s]x==NULL||y==NULL,ch=%d\n", __FUNCTION__, nCh);
		return -1;
	}
	return (int) theCOsd[nCh]->GetPosition(OSD_TYPE, x, y);
}

//##ModelId=49BDC5120004
int CChannelManager::EncCaptureIFrame(int nCh, int nSensorIndex)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}


	return (int) theCCoder[nCh]->EncCaptureIFrame(nSensorIndex);
}

//##ModelId=49BDC52D01E3
int CChannelManager::AuxCoder_IsEnable(int nCh)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}


	return (int) theCCoder[nCh]->AuxCoder_IsEnable();
}

//##ModelId=49BDC53C00E0
int CChannelManager::AuxCoder_SetFrameRate(int nCh, int nFrameRate)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCCoder[nCh]->AuxCoder_SetFrameRate(nFrameRate);
}

//##ModelId=49BDC5900041
int CChannelManager::AuxCoder_GetFrameRate(int nCh)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}


	return (int) theCCoder[nCh]->AuxCoder_GetFrameRate();
}

//##ModelId=49BDC5BD03B7
int CChannelManager::AuxCoder_SetBitRate(int nCh, int bBitRate)
{

	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}


	return (int) theCCoder[nCh]->AuxCoder_SetBitRate(bBitRate);
}

//##ModelId=49BDC75800AB
int CChannelManager::AuxCoder_GetBitRate(int nCh)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCCoder[nCh]->AuxCoder_GetBitRate();
}

int CChannelManager::AuxCoder_SetFormat(int nCh, int byResolution, int width, int height)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCCoder[nCh]->AuxCoder_SetFormat(byResolution, width, height);
}

//!\brief 获取子码流码率
//!\praram int nCh 通道
//!\return	成功为子码流尺寸，失败为-1
//##ModelId=49BDC75800AB
int CChannelManager::AuxCoder_GetFormat(int nCh, int *width, int *height)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCCoder[nCh]->AuxCoder_GetFormat(width, height);
}

//!\brief 获取音量大小
//!\praram int nCh 通道
//!\return	成功为音量大小值，失败为-1
//##ModelId=49BDC75800AB
int CChannelManager::GetAudioInVol(int nCh)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCCoder[nCh]->GetAudioInVol();
}

int CChannelManager::SetAudioInVol(int nCh, int vol)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCCoder[nCh]->SetAudioInVol(vol);
}

//!\brief 获取音量大小
//!\praram int nCh 通道
//!\return	成功为音量大小值，失败为-1
//##ModelId=49BDC75800AB
int CChannelManager::GetAudioOutVol(int nCh)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCCoder[nCh]->GetAudioOutVol();
}

int CChannelManager::SetAudioOutVol(int nCh, int vol)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCCoder[nCh]->SetAudioOutVol(vol);
}

int CChannelManager::SetAudioMappingVol(int nCh, int vol,int Type)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCCoder[nCh]->SetAudioMappingVol(vol,Type);
}


int CChannelManager::SetAudioAlarmConfig(int nCh, int enable, int audioType, int audioDelayOutput, int audioVolume, int iAudioPlayTimes)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCCoder[nCh]->SetAudioAlarmConfig(nCh, enable, audioType, audioDelayOutput, audioVolume, iAudioPlayTimes);
}

int CChannelManager::TryListenAlarmVoice(int nCh, int enable, int audioType, int audioDelayOutput, int audioVolume, int alarmType)
{
	if (!m_bInit)
	{
		AVL_INFO("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_INFO("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCCoder[nCh]->TryListenAlarmVoice(nCh, enable, audioType, audioDelayOutput, audioVolume, alarmType);
}

int CChannelManager::SetAudioCustomVoice(int nCh, int customtype, char *pBuff, int len)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCCoder[nCh]->SetAudioCustomVoice(nCh, customtype, pBuff, len);
}

int CChannelManager::GetAudioAlarmConfig(int nCh, int *enable, int *audioType, int *audioDelayOutput, int *audioVolume, int *iAudioPlayTimes)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCCoder[nCh]->GetAudioAlarmConfig(nCh, enable, audioType, audioDelayOutput, audioVolume, iAudioPlayTimes);
}

int CChannelManager::GetGetRealKeyFrameInterval(int nCh, int stream,int *nFrameRateInterval, int *nKeyFrameInterval)
{
	if (!m_bInit)
	{
		//AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		//AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	if (stream < 0 || stream > 2)
	{
		//AVL_ERR("[CChannelManager:%s]stream_no err, stream %d\n", __FUNCTION__, stream);
		return -1;
	}

	return (int)theCCoder[nCh]->GetGetRealKeyFrameInterval(stream, nFrameRateInterval,nKeyFrameInterval);
}


int CChannelManager::GetKeyFrameInterval(int nCh, int stream, int *value)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	if (stream < 0 || stream > 2)
	{
		AVL_ERR("[CChannelManager:%s]stream_no err, stream %d\n", __FUNCTION__, stream);
		return -1;
	}

	return (int) theCCoder[nCh]->GetKeyFrameInterval(stream, value);
}

int CChannelManager::SetKeyFrameInterval(int nCh, int stream, int value)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	if (stream < 0 || stream > 2)
	{
		AVL_ERR("[CChannelManager:%s]stream_no err, stream %d\n", __FUNCTION__, stream);
		return -1;
	}

	return (int) theCCoder[nCh]->SetKeyFrameInterval(stream, value);
}

int CChannelManager::AuxCoder_GetQuality(int nCh)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCCoder[nCh]->AuxCoder_GetQuality();    
}

int CChannelManager::AuxCoder_SetQuality(int nCh, int dwQuality)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCCoder[nCh]->AuxCoder_SetQuality(dwQuality);

}

int CChannelManager::AuxCoder_GetBitrateType(int nCh)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCCoder[nCh]->AuxCoder_GetBitrateType();
}

int CChannelManager::AuxCoder_SetBitrateType(int nCh, int dwBitrateType)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCCoder[nCh]->AuxCoder_SetBitrateType(dwBitrateType);
}

int CChannelManager::AuxCoder_GetStreamType(int nCh)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCCoder[nCh]->AuxCoder_GetStreamType();
}

int CChannelManager::AuxCoder_SetStreamType(int nCh, int dwStreamType)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	
    /* 参数能力级检测 */	
    if (dwStreamType != IsSupportDeviceAbility(1, 3, dwStreamType, NULL))
    {	

		AVL_ERR("Device not support ability(StreamType)\n");
        return -1;
    }	
	m_auxStreamType = dwStreamType;

	return (int) theCCoder[nCh]->AuxCoder_SetStreamType(dwStreamType);
}

int CChannelManager::AuxCoder_Enable(int nCh, int bEnable)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	//如果是子码流录像，则始终启动子码流编码
	if (1 == theCRecord[nCh]->GetRecStreamIndex())
	{
		bEnable = 1;
	}

	return (int) theCCoder[nCh]->AuxCoder_Enable(bEnable);
}

int CChannelManager::AuxCoder_GetBitInfo(int nCh)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	return (int) m_uiAuxRealBits[nCh];
}

int CChannelManager::TUTK_GetMedia(int nCh, int *whichMedia)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCCoder[nCh]->TUTK_GetMedia(whichMedia);
}

int CChannelManager::TUTK_SetQualityCfg(int nCh, int tutkQuality)
{
	tagEncCfg Info;
	int ret = 0;
	
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	ret = theCCoder[nCh]->TUTK_SetQualityCfg(tutkQuality,&Info);
	return ret;
	
}

int CChannelManager::TUTK_GetQualityCfg(int nCh, int *tutkQuality)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCCoder[nCh]->TUTK_GetQualityCfg(tutkQuality);
}

int CChannelManager::TUTK_GetQualityboardCfg(int nCh, int count, int *dwStreamType)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCCoder[nCh]->TUTK_GetQualityboardCfg(count,dwStreamType);
}


#ifdef USE_HDAUX
//!\brief 启用/不启用高清子码流
//!\param int nCh 通道
//!\param BOOL bEnable FALSE 为不启用，TRUE为启用
//!\return  成功为0，失败为-1
int CChannelManager::HDAuxCoder_Enable(int nCh, BOOL bEnable)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	//如果是高清子码流录像，则始终启动高清子码流编码
	if (2 == theCRecord[nCh]->GetRecStreamIndex())
	{
		bEnable = 1;
	}

	return (int) theCCoder[nCh]->HDAuxCoder_Enable(bEnable);
}

int CChannelManager::HDAuxCoder_IsEnable(int nCh)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCCoder[nCh]->HDAuxCoder_IsEnable();
}

int CChannelManager::HDAuxCoder_GetBitInfo(int nCh)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	return (int) m_uiHDAuxRealBits[nCh];
}

//!\brief   获取高清子码流参数
//!\param   int nCh 通道
//!\param   tagEncCfg * pInfo 编码参数
//!\return  成功为0，失败为-1
int CChannelManager::HDAuxCoder_GetEncCfg(int nCh, tagEncCfg * pInfo)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCCoder[nCh]->HDAuxCoder_GetEncCfg(pInfo);
}

//!\brief       设置高清子码流参数
//!\param   int nCh 通道
//!\param   tagEncCfg * pInfo 编码参数
//!\return  成功为0，失败为-1
int CChannelManager::HDAuxCoder_SetEncCfg(int nCh, tagEncCfg * pInfo)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	if (pInfo->byStreamType > 1 || pInfo->byStreamType < 0)
	{
		AVL_ERR("[CChannelManager:%s]pInfo->byStreamType=%d\n", __FUNCTION__, pInfo->byStreamType);
		return -1;
	}
	m_HDauxStreamType = pInfo->byStreamType;
	return (int) theCCoder[nCh]->HDAuxCoder_SetEncCfg(pInfo);
}

#endif

#ifdef USE_FOURTH_STREAM

//!\brief   获取第四码流参数
//!\param   int nCh 通道
//!\param   tagEncCfg * pInfo 编码参数
//!\return  成功为0，失败为-1
int CChannelManager::FourthStream_GetEncCfg(int nCh, tagEncCfg * pInfo)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCCoder[nCh]->FourthStream_GetEncCfg(pInfo);
}

//!\brief   设置第四码流参数
//!\param   int nCh 通道
//!\param   tagEncCfg * pInfo 编码参数
//!\return  成功为0，失败为-1
int CChannelManager::FourthStream_SetEncCfg(int nCh, tagEncCfg * pInfo)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCCoder[nCh]->FourthStream_SetEncCfg(pInfo);
}

#endif


int CChannelManager::Coder_GetBitInfo(int nCh)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	return (int) m_uiRealBits[nCh];
}

int CChannelManager::Pic_GetSupport(int type)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}


	return (int) COptics::GetSupport(type);
}

//##ModelId=49BDCD8C02B7
int CChannelManager::Pic_GetWorkMode()
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}


	return (int) COptics::GetWorkMode();
}

//##ModelId=49BDCA430330
int CChannelManager::Pic_SetPicParam(int nCh, int TimeSec, int Type, int value)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}



	return (int) theCOptics[nCh]->SetPicParam(TimeSec, Type, value);
}

//##ModelId=49BDCA98006B
int CChannelManager::Pic_SetTimeCfg(int nCh, int timesec, int iStartTime, int iStopTime)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCOptics[nCh]->SetTimeCfg(timesec, iStartTime, iStopTime);
}

//##ModelId=49BDD24E0128
int CChannelManager::Pic_SetWorkMode(int nWorkMode)
{
	int ret = 0;

	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	ret = COptics::SetWorkMode(nWorkMode);
	CMotion::SetMode(COptics::GetWorkMode());

	return (int) ret;
}

//##ModelId=49BDD24E0330
int CChannelManager::Pic_GetPicParam(int nCh, int TimeSec, int Type)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}



	return (int) theCOptics[nCh]->GetPicParam(TimeSec, Type);
}

//##ModelId=49BDD24F01F1
int CChannelManager::Pic_GetTimeCfg(int nCh, int timesec, int *piStartTime, int *piStopTime)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	if (NULL == piStartTime || NULL == piStopTime)
	{
		AVL_ERR("[CChannelManager:%s]NULL==piStartTime||NULL==piStopTime,ch=%d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCOptics[nCh]->GetTimeCfg(timesec, piStartTime, piStopTime);
}

//!\brief 设置是否为编辑状态，图像参数
//!\praram int bEdit 1-开始编辑，暂停自动配置。0-退出编辑，开始启动自动
//!\return  成功为0，失败为-1
//##ModelId=49BDEEBD02BF
int CChannelManager::Pic_EnterEdit(int nCh, int bEdit)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	return (int) theCOptics[nCh]->EnterEdit(bEdit);

}


//##ModelId=49BDD2890141
int CChannelManager::Pic_Capture(int nCh, int bAlarm, int AlarmType, int AlarmCh)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}



	return (int) theCCapturePic[nCh]->Capture(bAlarm, AlarmType, AlarmCh);
}


//!\brief 抓图
//!\praram int nCh 通道
//!\praram char *FileName 传出文件名称
//!\return  成功为0，失败为-1
//##ModelId=49BDD2890141
int CChannelManager::Pic_Capture(int nCh, char *FileName)
{


	return 0;
}


//##ModelId=49BDD2D300CF
int CChannelManager::Pic_GetCaptureCfg(int nCh,int *nCount, int *nIntervalTime, int *nFormat, int *nWidth, int *nHeight, int *nQuality)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (NULL == nCount || NULL == nIntervalTime)
	{
		AVL_ERR("[CChannelManager:%s]NULL==nCount||NULL==nIntervalTime\n", __FUNCTION__);
		return -1;
	}

	return (int) theCCapturePic[nCh]->GetCaptureCfg(nCount, nIntervalTime, nFormat, nWidth, nHeight, nQuality);
}

//##ModelId=49BDD4D60075
int CChannelManager::Pic_SetCaptureCfg(int nCh,int nCount, int nIntervalTime, int nFormat, int nWidth, int nHeight, int nQuality)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	
	return (int) theCCapturePic[nCh]->SetCaptureCfg(nCount, nIntervalTime, nFormat, nWidth, nHeight, nQuality);
}

int CChannelManager::Pic_GetCaptureStreamType(int nCh, char *nStreamType)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (NULL == nStreamType)
	{
		AVL_ERR("[CChannelManager:%s]NULL==nStreamType\n", __FUNCTION__);
		return -1;
	}

	return (int) theCCapturePic[nCh]->GetCaptureStreamType(nStreamType);
}

int CChannelManager::Pic_SetCaptureCfgV2(int nCh, int nQualitychar, char nStreamType)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	
	return (int) theCCapturePic[nCh]->SetCaptureCfgV2(nQualitychar, nStreamType);
}

//##ModelId=49BDE6000091
int CChannelManager::Motion_SetDetectPrecision(int nCh, int iGrade)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	
	return (int) theCMotion[nCh]->SetDetectPrecision(iGrade);
}

int CChannelManager::HumanShape_SetDetectPrecision(int nCh, int iGrade)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	
	return (int) theCHumanShape[nCh]->SetDetectPrecision(iGrade);
}

//##ModelId=49BDE7850394
int CChannelManager::Motion_GetDetectPrecision(int nCh)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	
	return (int) theCMotion[nCh]->GetDetectPrecision();
}

int CChannelManager::HumanShape_GetDetectPrecision(int nCh)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	
	return (int) theCHumanShape[nCh]->GetDetectPrecision();
}

//##ModelId=49BDE672019A
int CChannelManager::Motion_EnableMagicBlock(int nCh, unsigned char *pMagicBlock, int nBlockNum)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCMotion[nCh]->EnableMagicBlock(pMagicBlock, nBlockNum);
}

//##ModelId=49BDE739029A
int CChannelManager::Motion_GetEnableMagicBlock(int nCh, unsigned char *pMagicBlock, int nMaxBlockNum)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}


	return (int) theCMotion[nCh]->GetEnableMagicBlock(pMagicBlock, nMaxBlockNum);
}

/****************************************************************************************/

int CChannelManager::Motion_EnableMagicBlock_CGI(int nCh, tagMotionRegion * pRegionMotionCfg, int nMacRegionAreas)
{
	if (!m_bInit)
	{
		AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCMotion[nCh]->EnableMagicBlock_CGI(pRegionMotionCfg, nMacRegionAreas);
}

int CChannelManager::Motion_GetEnableMagicBlock_CGI(int nCh, tagMotionRegion * pRegionMotionCfg, int nMacRegionAreas)
{
	if (!m_bInit)
	{
		AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCMotion[nCh]->GetEnableMagicBlock_CGI(pRegionMotionCfg, nMacRegionAreas);
}

int CChannelManager::Motion_SetAudioAlarmSchedule(int nCh, tagAudioAlarmSchedule *stSchedule)
{
	if (!m_bInit)
	{
		AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCMotion[nCh]->SetAudioAlarmSchedule(stSchedule);
}

int CChannelManager::Motion_GetAudioAlarmSchedule(int nCh, tagAudioAlarmSchedule *stSchedule)
{
	if (!m_bInit)
	{
		AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCMotion[nCh]->GetAudioAlarmSchedule(stSchedule);
}

int CChannelManager::Motion_CheckAudioAlarmInSchedule(int nCh)
{
	if (!m_bInit)
	{
		AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCMotion[nCh]->CheckAudioAlarmInSchedule();
}

int CChannelManager::Motion_SetAudioAlarmCfg(int nCh, int nVoiceType, int nBroadcastCount)
{
	if (!m_bInit)
	{
		AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCMotion[nCh]->SetAudioAlarmCfg(nVoiceType, nBroadcastCount);
}

int CChannelManager::Motion_GetAudioAlarmCfg(int nCh, int *nVoiceType, int *nBroadcastCount)
{
	if (!m_bInit)
	{
		AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCMotion[nCh]->GetAudioAlarmCfg(nVoiceType, nBroadcastCount);
}

int CChannelManager::HumanShape_GetRegionCfg(int nCh, tagMotionRegion *pRegionCfg, int nRegionNum)
{
	if (!m_bInit)
	{
		AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCHumanShape[nCh]->GetRegionCfg(pRegionCfg, nRegionNum);
}

int CChannelManager::GetPolygonRegionCfg(int nCh, struct tagPolygon *pRegionCfg)
{
	if (!m_bInit)
	{
		AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCAreaDefense[nCh]->GetPolygonRegionCfg(pRegionCfg, 1);
}

int CChannelManager::HumanShape_SetRegionCfg(int nCh, tagMotionRegion *pRegionCfg, int nRegionNum)
{
	#if 0
	if (!m_bInit)
	{
		AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	int rc = 0;

	rc = (int) theCHumanShape[nCh]->SetRegionCfg(pRegionCfg, nRegionNum);
	if (0 != rc)
	{
		return rc;
	}
	theCAreaDefense[nCh]->SetRegionCfg(pRegionCfg, nRegionNum);

	// 设置移动侦测区域
	struct tagPolygon polygonRegion = {0};
	unsigned char byMagicBlock[16 * 12] = {0};
	unsigned char byBlockBuf[12][16] = {0};
	int nCount = 0;

	polygonRegion.bValid = 1;
	polygonRegion.size = 4;
	polygonRegion.point[0].x = (pRegionCfg->nMotionAreaTopLeftX*10000)/704;
	polygonRegion.point[0].y = (pRegionCfg->nMotionAreaTopLeftY*10000)/576;
	polygonRegion.point[1].x = (pRegionCfg->nMotionAreaBottomRightX*10000)/704;
	polygonRegion.point[1].y = (pRegionCfg->nMotionAreaTopLeftY*10000)/576;
	polygonRegion.point[2].x = (pRegionCfg->nMotionAreaBottomRightX*10000)/704;
	polygonRegion.point[2].y = (pRegionCfg->nMotionAreaBottomRightY*10000)/576;
	polygonRegion.point[3].x = (pRegionCfg->nMotionAreaTopLeftX*10000)/704;
	polygonRegion.point[3].y = (pRegionCfg->nMotionAreaBottomRightY*10000)/576;

	// 1.多边形转换成块
	transPolygonToBlock(&polygonRegion, (unsigned char*)byBlockBuf, sizeof(byBlockBuf));

	// 2.块转换为序号数组
	nCount = 0;
	for (int y = 0; y < 12; y++)
	{
		for (int x = 0; x < 16; x++)
		{
			if (byBlockBuf[y][x]) // 如果该区域被标记了，是移动侦测区域
			{
				// 将序列号放到数组中，数组中存放的是已经标记了的宏块的序列号
				byMagicBlock[nCount] = y * 16 + x;
				nCount++;
			}
		}
	}

	rc = (int) theCMotion[nCh]->EnableMagicBlock(byMagicBlock, nCount);

	return rc;
	#endif
	if (!m_bInit)
	{
		AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCHumanShape[nCh]->SetRegionCfg(pRegionCfg, nRegionNum);
}

int CChannelManager::SetPolygonRegionCfg(int nCh, struct tagPolygon *pRegionCfg)
{
	if (!m_bInit)
	{
		AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	int rc = 0;

	// 设置人形区域
	rc = (int) theCHumanShape[nCh]->SetPolygonRegionCfg(pRegionCfg, 1);
	if (0 != rc)
	{
		AVL_ERR("[CChannelManager:%s]theCHumanShape SetPolygonRegionCfg failed\n", __FUNCTION__);
		//return rc;
	}
	// 设置移动侦测区域
	unsigned char byMagicBlock[16 * 12] = {0};
	unsigned char byBlockBuf[12][16] = {0};
	int nCount = 0;

	// 1.多边形转换成块
	transPolygonToBlock(pRegionCfg, (unsigned char*)byBlockBuf, sizeof(byBlockBuf));

	// 2.块转换为序号数组
	nCount = 0;
	for (int y = 0; y < 12; y++)
	{
		for (int x = 0; x < 16; x++)
		{
			if (byBlockBuf[y][x]) // 如果该区域被标记了，是移动侦测区域
			{
				// 将序列号放到数组中，数组中存放的是已经标记了的宏块的序列号
				byMagicBlock[nCount] = y * 16 + x;
				nCount++;
			}
		}
	}

	rc = (int) theCMotion[nCh]->EnableMagicBlock(byMagicBlock, nCount);
	if (0 != rc)
	{
		AVL_ERR("[CChannelManager:%s]theCMotion EnableMagicBlock failed\n", __FUNCTION__);
		//return rc;
	}

	rc = theCAreaDefense[nCh]->SetPolygonRegionCfg(pRegionCfg, 1);

	return rc;
}

/****************************************************************************************/

int CChannelManager::Motion_StartMotionAdjust(int nCh)
{
	if (!m_bInit)
	{
		AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCMotion[nCh]->startMotionCfgAdjust();
}

//##ModelId=49BDE7B100B2
int CChannelManager::Motion_Enable(int nCh, int nEnable)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	int rc = 0;
	
	rc |= theCMotion[nCh]->Enable(nEnable);
	rc |= theCAreaDefense[nCh]->Motion_SetEnable(nEnable);

	return rc;
}

int CChannelManager::AreaDefense_Motion_Enable(int nCh, int nEnable)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	theCMotion[nCh]->Enable(nEnable);

	bool en = false;
	if (nEnable != 0)
	{
		en = true;
	}
	
	return (int) theCAreaDefense[nCh]->Motion_SetEnable(en);
}

int CChannelManager::PedestrianFiltering_Enable(int nCh, int nEnable)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCMotion[nCh]->PedestrianFilteringEnable(nEnable);
}

int CChannelManager::SoundDetect_SetCfg(int nCh, int nEnable, int nSentive)
{
    if (!m_bInit)
    {
        AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
        return -1;
    }
    if (nCh < 0 || nCh >= m_iEncChannelCnt)
    {
        AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
        return -1;
    }
    SOUND_DETECT_PARAM_S stParam = {0};
    stParam.iSentive = nSentive;
    stParam.iSwitch = nEnable;

    return theCSoundDetect[nCh]->SetSoundDetectCfg(nCh, stParam);
}

int CChannelManager::SoundDetect_GetCfg(int nCh, int *pEnable, int *pSentive)
{
    if (!m_bInit)
    {
        AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
        return -1;
    }
    if (nCh < 0 || nCh >= m_iEncChannelCnt)
    {
        AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
        return -1;
    }
    SOUND_DETECT_PARAM_S stParam = {0};
    theCSoundDetect[nCh]->GetSoundDetectCfg(nCh, &stParam);
    *pEnable = stParam.iSwitch;
    *pSentive = stParam.iSentive;

    return 0;
}

int CChannelManager::HumanShape_Enable(int nCh, int nEnable)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	int rc = 0;
	rc |= theCHumanShape[nCh]->Enable(nEnable);
	rc |= theCAreaDefense[nCh]->HumanShape_SetEnable(nEnable);

	return rc;
}
#if defined (__IS_SUPPORT_LONGAWAYHOME__)
int CChannelManager::HumanShape_GetLongAwayEnable(int nCh, int *nEnable)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager] not inited\n");
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager] channel_no err, channel\n", nCh);
		return -1;
	}

	return theCHumanShape[nCh]->GetLongAwayEnable(nEnable);
}

int CChannelManager::HumanShape_GetLongAwayDuration(int nCh, int *AlarmDuration)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager]not inited\n");
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager]channel_no err, channel %d\n", nCh);
		return -1;
	}

	return theCHumanShape[nCh]->GetLongAwayDuration(AlarmDuration);
}

int CChannelManager::HumanShape_SetLongAwayEnable(int nCh, int nEnable)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager] not inited\n");
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager] channel_no err, channel %d\n", nCh);
		return -1;
	}

	return theCHumanShape[nCh]->SetLongAwayEnable(nEnable);
}

int CChannelManager::HumanShape_SetLongAwayDuration(int nCh, int AlarmDuration)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager] not inited\n");
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager] channel_no err, channel %d\n", nCh);
		return -1;
	}

	return theCHumanShape[nCh]->SetLongAwayDuration(AlarmDuration);
}

int CChannelManager::HumanShape_GetLongAwayPushAlarmTime(int nCh, time_t *stime, time_t *etime)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager] not inited\n");
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager] channel_no err, channel %d\n", nCh);
		return -1;
	}

	return theCHumanShape[nCh]->GetLongAwayPushAlarmTime(stime, etime);
}
#endif

int CChannelManager::AreaDefense_HumanShape_Enable(int nCh, int nEnable)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	theCHumanShape[nCh]->Enable(nEnable);

	bool en = false;
	if (nEnable != 0)
	{
		en = true;
	}
	return (int) theCAreaDefense[nCh]->HumanShape_SetEnable(en);
}

int CChannelManager::AreaDefense_FaceCap_Enable(int nCh, int nEnable)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	bool en = false;
	if (nEnable != 0)
	{
		en = true;
	}
	return (int) theCAreaDefense[nCh]->FaceCap_SetEnable(en);
}

//##ModelId=49BDE7D101E5
int CChannelManager::Motion_IsEnable(int nCh)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCMotion[nCh]->IsEnable();
}

int CChannelManager::AreaDefense_Motion_IsEnable(int nCh)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}


	return (int) theCAreaDefense[nCh]->Motion_IsEnable();
}

int CChannelManager::HumanShape_IsEnable(int nCh)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCHumanShape[nCh]->IsEnable();
}

int CChannelManager::AreaDefense_HumanShape_IsEnable(int nCh)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCAreaDefense[nCh]->HumanShape_IsEnable();
}

int CChannelManager::AreaDefense_FaceCap_IsEnable(int nCh)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCAreaDefense[nCh]->FaceCap_IsEnable();
}

int CChannelManager::PedestrianFiltering_IsEnable(int nCh)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCMotion[nCh]->IsPedestrianFilteringEnable();
}

//!\brief 获取移动的时间计划
//!\param tagSchedule* cfg 时间计划结构体
//!\return 成功为0，失败为-1
//##ModelId=49B7164C0058
int CChannelManager::Motion_GetSchedule(int nCh, tagMotionSchedule * cfg)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	if (cfg == NULL)
	{
		AVL_ERR("[CChannelManager:%s]cfg == NULL, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCMotion[nCh]->GetSchedule(cfg);
}

//!\brief 获取人形检测时间计划
//!\param tagSchedule* cfg 时间计划结构体
//!\return 成功为0，失败为-1
int CChannelManager::HumanShape_GetSchedule(int nCh, tagMotionSchedule *cfg)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	if (cfg == NULL)
	{
		AVL_ERR("[CChannelManager:%s]cfg == NULL, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCHumanShape[nCh]->GetSchedule(cfg);
}

int CChannelManager::AreaDefense_GetSchedule(int nCh, tagMotionSchedule *cfg)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	if (cfg == NULL)
	{
		AVL_ERR("[CChannelManager:%s]cfg == NULL, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCAreaDefense[nCh]->GetSchedule(cfg);
}

int CChannelManager::FaceDetect_Enable(int nCh, int nEnable)
{
	return 0;
}

int CChannelManager::FaceDetect_IsEnable(int nCh)
{
	return 0;
}


//!\brief 设置人脸检测时间计划
//!\param ttagSchedule cfg 时间计划结构体
//!\return 成功为0，失败为-1
int CChannelManager::FaceDetect_SetSchedule(int nCh, const tagMotionSchedule *cfg)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	if (cfg == NULL)
	{
		AVL_ERR("[CChannelManager:%s]cfg == NULL, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	int rc = 0;
	rc |= theCHumanShape[nCh]->SetSchedule(cfg);
	rc |= theCMotion[nCh]->SetSchedule(cfg);
	rc |= theCAreaDefense[nCh]->SetSchedule(cfg);

	return rc;
}

//!\brief 获取人脸检测时间计划
//!\param tagSchedule* cfg 时间计划结构体
//!\return 成功为0，失败为-1
int CChannelManager::FaceDetect_GetSchedule(int nCh, tagMotionSchedule *cfg)
{
	return 0;
}

int CChannelManager::FaceDetect_SetFaceOverlayInfo(int nCh, int nMainStreamOverlay, int nSubStreamOverlay)
{
	return 0;
}

int CChannelManager::FaceDetect_GetFaceOverlayInfo(int nCh, int *nMainStreamOverlay, int *nSubStreamOverlay)
{
	return 0;
}

//!\brief 设置移动的时间计划
//!\param ttagSchedule cfg 时间计划结构体
//!\return 成功为0，失败为-1
//##ModelId=49B716980242
int CChannelManager::Motion_SetSchedule(int nCh, const tagMotionSchedule * cfg)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	if (cfg == NULL)
	{
		AVL_ERR("[CChannelManager:%s]cfg == NULL, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	int rc = 0;
	rc |= theCMotion[nCh]->SetSchedule(cfg);
	rc |= theCHumanShape[nCh]->SetSchedule(cfg);
	rc |= theCAreaDefense[nCh]->SetSchedule(cfg);

	return rc;
}

//!\brief 设置人形检测时间计划
//!\param ttagSchedule cfg 时间计划结构体
//!\return 成功为0，失败为-1
int CChannelManager::HumanShape_SetSchedule(int nCh, const tagMotionSchedule *cfg)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	if (cfg == NULL)
	{
		AVL_ERR("[CChannelManager:%s]cfg == NULL, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	int rc = 0;
	rc |= theCHumanShape[nCh]->SetSchedule(cfg);
	rc |= theCMotion[nCh]->SetSchedule(cfg);
	rc |= theCAreaDefense[nCh]->SetSchedule(cfg);

	return rc;
}

int CChannelManager::AreaDefense_SetSchedule(int nCh, const tagMotionSchedule *cfg)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	if (cfg == NULL)
	{
		AVL_ERR("[CChannelManager:%s]cfg == NULL, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	theCMotion[nCh]->SetSchedule(cfg);
	theCHumanShape[nCh]->SetSchedule(cfg);	

	return (int) theCAreaDefense[nCh]->SetSchedule(cfg);
}

int CChannelManager::AudioAlarm_SetSchedule(int nCh, const tagAudioAlarmSchedule *cfg)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	if (cfg == NULL)
	{
		AVL_ERR("[CChannelManager:%s]cfg == NULL, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCCoder[nCh]->SetAudioAlarmSchedule(cfg);
}

int CChannelManager::AudioAlarm_GetSchedule(int nCh, tagAudioAlarmSchedule *cfg)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	if (cfg == NULL)
	{
		AVL_ERR("[CChannelManager:%s]cfg == NULL, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCCoder[nCh]->GetAudioAlarmSchedule(cfg);
}

int CChannelManager::AudioAlarm_CheckAudioAlarmInSchedule(int nCh)
{
	if (!m_bInit)
	{
		AVL_DBG("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCCoder[nCh]->CheckAudioAlarmInSchedule();
}


int CChannelManager::HumanShape_SetHumanOverlayInfo(int nCh, char nMainStreamOverlay, char nSubStreamOverlay)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCHumanShape[nCh]->SetHumanOverlayInfo(nMainStreamOverlay, nSubStreamOverlay);
}

int CChannelManager::HumanShape_GetHumanOverlayInfo(int nCh, char *nMainStreamOverlay, char *nSubStreamOverlay)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCHumanShape[nCh]->GetHumanOverlayInfo(nMainStreamOverlay, nSubStreamOverlay);
}

int CChannelManager::HumanShape_SetDetectAreaOverlayInfo(int nCh, int nMainStreamOverlay, int nSubStreamOverlay)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCHumanShape[nCh]->SetDetectAreaOverlayInfo(nMainStreamOverlay, nSubStreamOverlay);
}

int CChannelManager::HumanShape_GetDetectAreaOverlayInfo(int nCh, int *nMainStreamOverlay, int *nSubStreamOverlay)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCHumanShape[nCh]->GetDetectAreaOverlayInfo(nMainStreamOverlay, nSubStreamOverlay);
}

//获取人形帧数据
//注意使用完成后,第一时间释放
//否则下次获取将会阻塞(死锁)
int CChannelManager::HumanShape_GetHumanShapeFrame(int nCh, char **pBuff, int *pLen)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	if (NULL == pBuff
		|| NULL == pLen)
	{
		AVL_ERR("[CChannelManager:%s]param == NULL, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCHumanShape[nCh]->GetHumanShapeFrame(pBuff, pLen);
}

//释放人形帧数据
//与HumanShape_GetHumanShapeFrame配合使用
int CChannelManager::HumanShape_ReleaseHumanShapeFrame(int nCh, char *pBuff)
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}
	if (NULL == pBuff)
	{
		AVL_ERR("[CChannelManager:%s]param == NULL, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	return (int) theCHumanShape[nCh]->ReleaseHumanShapeFrame(pBuff);
}

#ifdef __USE_CIRCLEQUEUE__
//!\brief 获取码流类型
//!\praram int nChannel 通道
//!\return  码流类型
int CChannelManager::GetStreamType(int nChannel)
{
	if (STREAM_TYPE_MAIN == nChannel)
	{
		return m_mainStreamType;
	}
	else if (STREAM_TYPE_AUX == nChannel)
	{
		return m_auxStreamType;
	}
	else if (STREAM_TYPE_HDAUX == nChannel)
	{
		return m_HDauxStreamType;
	}

	return 0;
}


//!\brief 获取录像码流索引
//!\praram int nCh 通道
//!\return  录像码流索引
int CChannelManager::GetRecStreamIndex(int nCh)
{
	int ret = 0;
	ret = theCRecord[nCh]->GetRecStreamIndex();
	return ret;
}
#endif

/************************************ROI*******************************************/

int CChannelManager::AudioIn_SetInterface(int mode)
{
	int ret=0;
	mode = (mode == 0) ? 0 : 1;
	
	if(m_AudioInChannel != mode)
	{
		ret = pCConfig->SetValue("AudioIn", "channel", mode);
		if (ret == 0 && pCConfig->m_bReadOnlyCfgFlag == 1)
		{
			mode = pCConfig->GetValue("AudioIn", "channel", 0);
		}
		ret = ZWSys_SetAudioChannel((AudioChannelType_t)(mode));
		if(0 == ret)
		{
			m_AudioInChannel = mode;
		}
		else
			AVL_ERR("call ZWSys_SetAudioChannel fail !\n");
	}
	
	return ret;
}

int CChannelManager::AudioIn_GetInterface()
{

	return m_AudioInChannel;
}

int CChannelManager::AudioCodec_SetInterface(int ACodec)
{
	int ret=0;
	
	if(m_AudioCodec != ACodec)
	{
		ret = ZWSys_SetAudioCodec((AudioEncodeMode_t)(ACodec));
		if(0 == ret)
		{
			m_AudioCodec = ACodec;
			pCConfig->SetValue("AudioIn", "AudioCodec", ACodec);
		}
		else
			AVL_ERR("call ZWSys_SetAudioCodec fail !\n");
	}
	
	return ret;
}

int CChannelManager::AudioCodec_GetInterface()
{

	return m_AudioCodec;
}

//##ModelId=49BF41FD021E
int CChannelManager::
SetAVCallBack(int (*AvCallBack) (int nCh, char *Buff, int nBuffLen, int Type, int nStreamType, unsigned int tFrameTime, unsigned int tFrameTime_usec))
{
	if (!m_bInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}
	if (NULL == AvCallBack)
	{
		AVL_ERR("[CChannelManager:%s]AvCallBack==NULL\n", __FUNCTION__);
		return -1;
	}
	m_FnxAvCallBack = AvCallBack;

	return (int) 0;
}

CChannelManager *CChannelManager::GetInstanceHandle()
{
	if (NULL == pCChannelManger)
	{
		pCChannelManger = new CChannelManager;
		if (NULL == pCChannelManger)
		{
			AVL_ERR("[CChannelManager:%s]new CChannelManager failed\n", __FUNCTION__);
			return NULL;
		}
		else
		{
			XTRACE_MUTEX_INIT(&InitLock, NULL);
			pCChannelManger->Init();
		}
	}
	m_iInstanceCnt++;
	return pCChannelManger;

}

void CChannelManager::Release()
{
	if (NULL != pCChannelManger)
	{
		m_iInstanceCnt--;

		//if(m_iInstanceCnt<=0)
		{
			delete pCChannelManger;

			pCChannelManger = NULL;
		}
	}
}

int CChannelManager::AvCallBack_FNX(unsigned long  channelNumber)
{
	CChannelManager *pccm = CChannelManager::GetInstanceHandle();
	char CallBackType = 0;
	if (NULL != pccm)
	{
		pccm->AvReadStream((int)channelNumber,CallBackType);
	}

	return 0;
}

int CChannelManager::FaceCapSimplify_SetFilterCfg(int nCh, int MinWidth, int MinHeight, int MaxWidth, int MaxHeight)
{
	return 0;
}

int CChannelManager::FaceCapSimplify_GetFilterCfg(int nCh, int *MinWidth, int *MinHeight, int *MaxWidth, int *MaxHeight)
{
	return 0;
}

int CChannelManager::FaceCapSimplify_SetRegionCfg(int nCh, int AreaTopLeftX, int AreaTopLeftY, int AreaBottomRightX, int AreaBottomRightY)
{
	return 0;
}

int CChannelManager::FaceCapSimplify_GetRegionCfg(int nCh, int *AreaTopLeftX, int *AreaTopLeftY, int *AreaBottomRightX, int *AreaBottomRightY)
{
	return 0;
}

//获取人脸帧数据
//注意使用完成后,第一时间释放
//否则下次获取将会阻塞(死锁)
int CChannelManager::FaceCapSimplify_GetFaceEventFrame(int nCh, char **pBuff, int *pLen)
{
	return 0;
}

//释放人脸帧数据
//与CM_FaceCap_GetFaceEventFrame配合使用
int CChannelManager::FaceCapSimplify_ReleaseFaceEventFrame(int nCh, char *pBuff)
{
	return 0;
}

int CChannelManager::FaceCapSimplify_SetEventCallBack(int nCh, int (*CallBack) (struct tagAlertInfo *))
{
	return 0;
}

int CChannelManager::TriggerEvent(int nCh, unsigned long dwEventType, unsigned long dwSubEventType, unsigned long dwParam1, unsigned long dwParam2, unsigned long dwParam3, char * pBuffer, unsigned long dwBufferLen)
{
	//发送事件
	return HieServer_RefreshEventEx(dwEventType, dwSubEventType,
							   		dwParam1, dwParam2, dwParam3, pBuffer, dwBufferLen);
}

int CChannelManager::UpdateDorStatus(int bDorEnable)
{
    //单线程调用,不加锁
    m_bDorEnable = bDorEnable;		
	return 0;
}

int CChannelManager::GetDorStatus()
{
    return m_bDorEnable;
}

#ifdef MAIN_AUX_SHOW

#else
typedef enum VA_BWE_EventType {
	VA_BWE_OBJECTLOSE			= 0x00000001,	// 物品被盗
	VA_BWE_OBJECTLEFT			= 0x00000002,	// 物品遗留
	VA_BWE_FEIFATINGCHE			= 0x00000004,	// 非法停车
	VA_BWE_INTRUSION			= 0x00000008,	
	VA_BWE_ABNORMALSPEED		= 0x00000010,	// 非正常速度
	VA_BWE_REVERSE				= 0x00000020,	// 逆行
	VA_BWE_TRIPWIRE				= 0x00000040,	// 绊线
	VA_BWE_WANDER				= 0x00000080,	// 闲逛
	VA_BWE_TUMBLE				= 0x00000100,	// 倒地
	VA_BWE_WALLPROTECT			= 0x00000200,	// 院前看护
	VA_BWE_FACECAPTURE			= 0x00000400,	// 人脸抓拍
	VA_BWE_MATMPROTECT			= 0x00000800,	// 设备看护
	VA_BWE_ABNORMALGUARD		= 0x00001000,	// 岗哨异常
	VA_BWE_FIGHT					= 0x00002000,	
	VA_BWE_MULTITRIPWIRE		= 0x00004000,	
	VA_BWE_IMGABNORMAL		= 0x00008000	    // 图像异常
}VA_BWE_EventType;

/*
   回调中除了前4个字节的时间外,后面为
   VA_ANZX_TargetGroup	Struct	包含有效目标数目，目标ID，目标位置，事件，目标外接框等信息的数据结构，详细定义如下描述。
   有效目标可能为1个,2个等等.
*/

typedef struct VA_ANZX_Point2D {
    short           wx;  //目标点x坐标值
	short           hy;  //目标点Y坐标值
} VA_ANZX_Point2D;     //目标中心位置数据结构

typedef struct VA_ANZX_Rect {
    short           bottomLeftWx;//目标外接矩形左下点x坐标
	short           bottomLeftHy;//目标外接矩形左下点y坐标
	short           topRightWx;  //目标外接矩形右上点x坐标
	short           topRithtHy;   //目标外接矩形右上点y坐标
} VA_ANZX_Rect;                //目标外接矩形框数据结构

typedef struct VA_ANZX_Target {
    int                id;//目标ID
	int                status;//目标状态
	int                type;//目标类型，1：人;2：车
	int                event;//目标事件，事件定义参考附录
    VA_ANZX_Point2D   center;      //目标中心位置数据结构
	VA_ANZX_Rect      area;       //目标外接矩形框数据结构

} VA_ANZX_Target;                  //目标数据结构

typedef struct VA_ANZX_TargetGroup {
    int  num;                       //有效目标个数
    VA_ANZX_Target     target[MAXNUM_VATARGET];                 //有效目标数据结构
} VA_ANZX_TargetGroup;                             //有效目标群数据结构


VA_ANZX_TargetGroup TargetGroup;
int CChannelManager::AvReadStream(int nCh,char CallBackType)
{
	int FrameType = PktError;

	int result = 0;
    struct timeval now;
	unsigned int PTS, uPTS;
	int 	ifAudioAACFrame = 0;
    int nSensorChn = 0;
	
	if (!m_cInit)
	{
		AVL_ERR("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return -1;
	}

	if (nCh < 0 || nCh >= m_iEncChannelCnt)
	{
		AVL_ERR("[CChannelManager:%s]channel_no err, channel %d\n", __FUNCTION__, nCh);
		return -1;
	}

	m_iAV_status[nCh] = 1;		//有回调了

	BitsInfoStat(nCh);
    gettimeofday(&now,NULL);
	PTS = now.tv_sec;
	uPTS = now.tv_usec;

	//从系统SDK读取数据
	result = ZWSys_EncReadStreamData(theCoderHandle[nCh],
									&m_PDataBuf,
									&m_iLength,
									&FrameType,
									(unsigned int *)&PTS,
									(unsigned int *)&uPTS,
									&nSensorChn);
	//如果有视频帧才清零
   	if(((PktIFrames == FrameType) || (PktPFrames == FrameType)) && (nSensorChn <= 1))
   	{
        m_iAV_FrameCnt[nCh][STREAM_TYPE_MAIN+(nSensorChn*2)]++;
   	}
    
	//子码流
	if(((PktQCIFIFrames == FrameType) || (PktQCIFPFrames == FrameType)) && (nSensorChn <= 1)	)
	{
		m_iAV_FrameCnt[nCh][STREAM_TYPE_AUX+(nSensorChn*2)]++;
	}

    /* 设备睡眠中，只允许推送视频流 */
    if (m_bDorEnable)
    {
        if (PktIFrames == FrameType || PktPFrames == FrameType ||
		PktQCIFIFrames == FrameType || PktQCIFPFrames == FrameType || 
		PktIFramesForS1 == FrameType || PktPFramesForS1 == FrameType ||
		PktQCIFIFramesForS1 == FrameType || PktQCIFPFramesForS1 == FrameType ||
		PktIFramesForS2 == FrameType || PktPFramesForS2 == FrameType ||
		PktQCIFIFramesForS2 == FrameType || PktQCIFPFramesForS2 == FrameType ||
		PktIFramesForS3 == FrameType || PktPFramesForS3 == FrameType ||
		PktQCIFIFramesForS3 == FrameType || PktQCIFPFramesForS3 == FrameType )
        {}
        else
        {
            //AVL_INFO("\n---Is sleeping mode now---\n");
            return 0;
        }
    }

	if ((result != 0) || (0 == m_iLength) || (PktError == FrameType) || (NULL == m_PDataBuf))
	{
		AVL_ERR("[CChannelManager:%s]ch%d-ZWSys_EncReadStreamData failed(%d)\n", __FUNCTION__, nCh, result);
		m_iAV_status[nCh] = 0;	//完毕
		return result;
	}

	//是否是运动侦测帧 
	if (PktMotionDetection == FrameType)
	{	
#if defined (__IS_SUPPORT_LCD_MANAGER__)
        int CallState = 0;
        GetExternPreviewCallState(&CallState);
        if (CallState != 0) //通话状态下，不进行报警联动操作
        {
            m_iAV_status[nCh] = 0;  //完毕
            return 0;
        }
#endif
		AVL_DBG("\n-----CChannelManager-----检测到移动---\n");
		theCMotion[nCh]->DealMotionData(nSensorChn,m_PDataBuf, m_iLength);
		m_iAV_status[nCh] = 0;	//完毕
		ZWSys_EncSetSlowFrameMode(2);
		return 0;
	}
	//是否是人形帧 
	else if (PktIAFrames == FrameType)
	{
#if defined (__IS_SUPPORT_LCD_MANAGER__)
        int CallState = 0;
        GetExternPreviewCallState(&CallState);
        if (CallState != 0) //通话状态下，不进行报警联动操作
        {
            m_iAV_status[nCh] = 0;  //完毕
            return 0;
        }
#endif
		AVL_DBG("\n-----CChannelManager-----检测到人形---\n");
		theCHumanShape[nCh]->DealHumanShapeData(nSensorChn,m_PDataBuf, m_iLength);
		m_iAV_status[nCh] = 0;	//完毕
		ZWSys_EncSetSlowFrameMode(2);
		return 0;
	}

	//分发
	//发给录像模块
	if (PktIFrames == FrameType || PktPFrames == FrameType ||
		PktIFramesForS1 == FrameType || PktPFramesForS1 == FrameType ||
		PktQCIFIFramesForS1 == FrameType || PktQCIFPFramesForS1 == FrameType ||
		PktIFramesForS2 == FrameType || PktPFramesForS2 == FrameType ||
		PktQCIFIFramesForS2 == FrameType || PktQCIFPFramesForS2 == FrameType ||
		PktIFramesForS3 == FrameType || PktPFramesForS3 == FrameType ||
		PktQCIFIFramesForS3 == FrameType || PktQCIFPFramesForS3 == FrameType ||
		PktAudioFrames == FrameType || PktQCIFIFrames == FrameType || PktQCIFPFrames == FrameType ||
		PktFourthIFrames == FrameType || PktFourthPFrames == FrameType ||
		PktFourthIFramesForS1 == FrameType || PktFourthPFramesForS1 == FrameType ||
		PktFourthIFramesForS2 == FrameType || PktFourthPFramesForS2 == FrameType ||
		PktFourthIFramesForS3 == FrameType || PktFourthPFramesForS3 == FrameType)
	{
		//统计码流信息
		if ((PktQCIFIFrames == FrameType) || (PktQCIFPFrames == FrameType))
		{
			//辅码流统计
			m_ullAuxRealBits_Stat[nCh] += m_iLength;
		}
		else if( (PktIFrames == FrameType) || (PktPFrames == FrameType))
		{
			//主码流统计
			m_ullRealBits_Stat[nCh] += m_iLength;
		}
		
        UPGRADE_MODE eSocUpgradeStatus = NOT_UPGRADE_MODE;  //SOC升级状态
	    MPU_PUB_GetCurrentUpgradeMode(&eSocUpgradeStatus);

		//送往数据给网络
		if (NULL != m_FnxAvCallBack)
		{
			m_iAV_status[nCh] = 2;	//网络
			if (eSocUpgradeStatus == NOT_UPGRADE_MODE)
			{

				if (((PktQCIFIFrames == FrameType) || (PktQCIFPFrames == FrameType)) || ((PktQCIFIFramesForS1 == FrameType) || (PktQCIFPFramesForS1 == FrameType)) \
	                || ((PktQCIFIFramesForS2 == FrameType) || (PktQCIFPFramesForS2 == FrameType)) || ((PktQCIFIFramesForS3 == FrameType) || (PktQCIFPFramesForS3 == FrameType)))
				{	
					/* 发送子码流 */
					m_FnxAvCallBack(nCh, m_PDataBuf, m_iLength, FrameType, STREAM_TYPE_AUX, PTS, uPTS);
				}
				else if ((PktFourthIFrames == FrameType) || (PktFourthPFrames == FrameType) || (PktFourthIFramesForS1 == FrameType) || (PktFourthPFramesForS1 == FrameType) \
	                     || (PktFourthIFramesForS2 == FrameType) || (PktFourthPFramesForS2 == FrameType) || (PktFourthIFramesForS3 == FrameType) || (PktFourthPFramesForS3 == FrameType))
				{
					/* 由于Mjpeg回调往缓冲区写的时候可能会失败, 导致指针指向的内存被释放, 但是指针依然存在, 后面非法访问会崩溃, 因此函数提前 */
					//theCCapturePic[nCh]->InputData(m_PDataBuf, m_iLength, PktIFrames, PTS, uPTS);

					/* 发送MJPEG流 */
					m_FnxAvCallBack(nCh, m_PDataBuf, m_iLength, FrameType, STREAM_TYPE_FOURTH, PTS, uPTS);
				}
				else if (PktAudioFrames == FrameType)
				{
					/* 判断是否为AAC格式 */
					if (LtAacAudio == m_PDataBuf[9])
					{
						ifAudioAACFrame = 1;
					}
					/* 发送主码流音频 */
					if ((1 == m_mainStreamType) && (0 == ifAudioAACFrame))
					{
						m_FnxAvCallBack(nCh, m_PDataBuf, m_iLength, FrameType, STREAM_TYPE_MAIN, PTS, uPTS);
					}
					/* 发送子码流音频帧 */
					if ((1 == m_auxStreamType) && (0 == ifAudioAACFrame))
					{
						m_FnxAvCallBack(nCh, m_PDataBuf, m_iLength, FrameType, STREAM_TYPE_AUX, PTS, uPTS);
					}
					if ((1 == m_HDauxStreamType) && (0 == ifAudioAACFrame))
					{
						m_FnxAvCallBack(nCh, m_PDataBuf, m_iLength, FrameType, STREAM_TYPE_HDAUX, PTS, uPTS);
					}
				}
				else if ((PktIFrames == FrameType) || (PktPFrames == FrameType) || ((PktIFramesForS1 == FrameType) || (PktPFramesForS1 == FrameType)) \
	                || ((PktIFramesForS2 == FrameType) || (PktPFramesForS2 == FrameType)) || ((PktIFramesForS3 == FrameType) || (PktPFramesForS3 == FrameType)))
				{
					/* 发送主码流 */
					m_FnxAvCallBack(nCh, m_PDataBuf, m_iLength, FrameType, STREAM_TYPE_MAIN, PTS, uPTS);
				}

#if defined  __USE_RTMP__
	            int iRet = 0;
	            unsigned int i, j;

				if((1==ActionCodeOk)&&(1 == bSupportRtmp))	//是否激活且能力支持
				{
					for(i=0; i<DVR_MAX_STREAM_NUM; i++)
					{
						for(j=0;j<MAX_CH_MANAGER_NUM;j++)
						{
							if((Veable[i][j])&&((RtmpStatus[i][j] == eZWRTMP_STATUS_Connected)||(RtmpStatus[i][j] == eZWRTMP_STATUS_Reconnected)))
							{
								//AVL_INFO("+++++++++++++++++++INPUT stream %d\n",i);
								if((PktAudioFrames == FrameType) && (m_ncPushAudioEnable)&& (ifAudioAACFrame))
								{
									iRet = ZW_RTMP_Client_Inputdata(RtmpHandle[i][j], (unsigned char *)m_PDataBuf, m_iLength);								
									if (iRet == eRetUnsupportedCodec)
									{
										RtmpStatus[i][j] = eZWRTMP_STATUS_UnsupportedCodec;
									}
								}
								
								if(0==i)
								{			
									if((PktIFrames == FrameType) || (PktPFrames == FrameType))	//发送主码流
									//if((PktQCIFIFrames == FrameType) || (PktQCIFPFrames == FrameType))	//发送主码流
									{
										iRet = ZW_RTMP_Client_Inputdata(RtmpHandle[i][j],(unsigned char *)m_PDataBuf,m_iLength);
										if (iRet == eRetUnsupportedCodec)
										{
											RtmpStatus[i][j] = eZWRTMP_STATUS_UnsupportedCodec;
										}
									}
								}
								else if(1==i)
								{
									if((PktQCIFIFrames == FrameType) || (PktQCIFPFrames == FrameType))
									{
										iRet = ZW_RTMP_Client_Inputdata(RtmpHandle[i][j],(unsigned char *)m_PDataBuf,m_iLength);
										if (iRet == eRetUnsupportedCodec)
										{
											RtmpStatus[i][j] = eZWRTMP_STATUS_UnsupportedCodec;
										}
									}
								}
								else
								{
									;//暂时没有其他流
								}
							}
						}
					}
				}
#endif
			}
			m_iAV_status[nCh] = 3;	//网络完毕
		}
#ifdef __USE_CIRCLEQUEUE__		
		else
		{
			if (NULL != m_PDataBuf && PktAudioFrames != FrameType)
			{
				free(m_PDataBuf);
				m_PDataBuf = NULL;
			}
		}
#endif
		//文件头不存储
		m_iAV_status[nCh] = 4;	//存储

		//等IR-CUT切换完毕后再开始录像
		if (0 == GetSystemStartOverFlag())
		{
			return 0;
		}

        // AVL_INFO("theCRecord[nCh]->GetRecStreamIndex() = %d\n", theCRecord[nCh]->GetRecStreamIndex());
#ifndef __USE_CIRCLEQUEUE__		
		if (PktAudioFrames == FrameType)
		{
			//音频数据需要存储			
			if(0 == ifAudioAACFrame)
				theCRecord[nCh]->InputData(m_PDataBuf, m_iLength, FrameType, PTS, uPTS);
		}
		else if (0 == theCRecord[nCh]->GetRecStreamIndex())
		{
			//主码流录像
			if ((PktIFrames == FrameType) || (PktPFrames == FrameType) || (PktBPFrames == FrameType) || (PktBBPFrames == FrameType))
			{
				theCRecord[nCh]->InputData(m_PDataBuf, m_iLength, FrameType, PTS, uPTS);
			}
		}
		else if (1 == theCRecord[nCh]->GetRecStreamIndex())
		{
			//子码流录像
			if ((PktQCIFIFrames == FrameType) || (PktQCIFPFrames == FrameType))
			{
				if (PktQCIFIFrames == FrameType)
				{
					FrameType = PktIFrames;
				}
				theCRecord[nCh]->InputData(m_PDataBuf, m_iLength, FrameType, PTS, uPTS);
			}
		}
#endif
		m_iAV_status[nCh] = 5;	//存储完
	}

	m_iAV_status[nCh] = 0;		//完毕
	return 0;
}
#endif

int CChannelManager::BitsInfoStat(int nCh)
{
	int result = 0;
	int bitrateData[MAX_CH_MANAGER_NUM][DVR_MAX_STREAM_NUM] = {0};
	result = ZWSys_GetBitrateInfo(theCoderHandle[nCh], bitrateData[nCh], DVR_MAX_STREAM_NUM);
	if (result != 0)
	{
		AVL_ERR("ZWSys_GetBitrateInfo failed!\n");
		return -1;
	}
	m_uiRealBits[nCh] = bitrateData[nCh][0];
	m_uiAuxRealBits[nCh] = bitrateData[nCh][1];
	#ifdef USE_HDAUX
	m_uiHDAuxRealBits[nCh] = bitrateData[nCh][2];
	#endif
	m_uiFourthRealBits[nCh] = bitrateData[nCh][3];

	return 0;
}

int CChannelManager::CheckSchedule()
{
	int i;

	if (!m_bInit)
	{
		AVL_WARN("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return 0;
	}
	if (m_bCheck_exit)
	{
		return -1;
	}

	for (i = 0; i < m_iEncChannelCnt; i++)
	{
		theCRecord[i]->Schedule();
	}

	return 0;
}

int CChannelManager::CheckScheduleV2()
{
	if (!m_bInit)
	{
		AVL_WARN("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return 0;
	}
	if (m_bCheck_exit)
	{
		return -1;
	}

	theCRecord[0]->ScheduleV2();
	return 0;
}

int CChannelManager::CheckWatch()
{
	int i,j,k;
	static int cnt = 0;
	static int whichFrame = 0;
	int bwatchdog;
	static int framePerSecond[DVR_MAX_STREAM_NUM][MAX_SECOND_NUM] = {0};//每个码流每一秒的帧率
	int totalFrame[DVR_MAX_STREAM_NUM] = {0};//每个流的帧率总和

    int stream_check_num = 0;
	static unsigned short  noFramcnt = 0;//无帧次数。避免在产线授权时，因为无流造成重启，影响产线授权，将无流重启调整到10分钟
	if ((access(PRODUCTTEST_FILE, F_OK) == 0) ||(access(PRODUCTTEST_WIFI_FILE, F_OK) == 0))
	{
		stream_check_num = 0;
	}
	else
	{
		stream_check_num = 2;
	}
	
	if (!m_bInit)
	{
		AVL_WARN("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return 0;
	}
	if (m_bCheck_exit)
	{
		return -1;
	}

	bwatchdog = 1;
	for (i = 0; i < m_iEncChannelCnt; i++)
	{
		//统计码流信息
		BitsInfoStat(i);

		if (theCoderHandle[i] == NULL)
		{
			continue;
		}

		if (MAX_SECOND_NUM == whichFrame)
		{
			whichFrame = 0;
		}
		
		for (j = 0; j < stream_check_num; j++)
		{
			framePerSecond[j][whichFrame] = m_iAV_FrameCnt[i][j];
			m_iAV_FrameCnt[i][j] = 0;//每秒帧率清0
			
			for (k = 0; k < MAX_SECOND_NUM; k++)
			{
				totalFrame[j] += framePerSecond[j][k];//使用队列，统计最近5s的帧率总和
			}

			if (totalFrame[j] < FRAME_THRESHOLD)//最近的5s帧率都小于FRAME_THRESHOLD，则不喂狗
			{
				AVL_WARN("[Stream:%d]The frame rate in %d seconds less than %d fps\n", j, MAX_SECOND_NUM, FRAME_THRESHOLD);
				bwatchdog = 0;
			}
		}
	}

	//AVL_INFO("bwatchdog %d, cnt %d\n",bwatchdog,cnt);
	if ((bwatchdog || noFramcnt <= 570) && cnt > 5)
	{
		cnt = 0;
		// AVL_WARN("[%s]wacthdog....\n",__FUNCTION__);
		theCSysManager->WatchDog(m_hWatch);
	}

	cnt++;
	whichFrame++;
	noFramcnt = bwatchdog ? 0 : noFramcnt+1;
	AVL_DBG("noFramcnt:%d\n",noFramcnt);
	return 0;
}

int CChannelManager::CheckStaus()
{
	int i;
	int bCoderError;

	if (!m_bInit)
	{
		AVL_WARN("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return 0;
	}
	if (m_bCheck_exit)
	{
		return -1;
	}

	if (m_bDorEnable)
	{
		return 0;
	}

	bCoderError = 0;
	for (i = 0; i < m_iEncChannelCnt; i++)
	{
		//移动侦测在回调里处理
		theCMotion[i]->CheckMotion();
		theCHumanShape[i]->CheckHumanShape();
        //声音侦测 ALERT_SOUNDDETECT
        theCSoundDetect[i]->CheckSound();
        //图像参数检测
        theCOptics[i]->CheckOptics();
        theCCoder[i]->CheckCoderError((bCoderError >> i) & 1);
    }

    return 0;
}

int CChannelManager::CheckCapture()
{
	int i;

	if (!m_bInit)
	{
		AVL_WARN("[CChannelManager:%s]not inited\n", __FUNCTION__);
		return 0;
	}
	if (m_bCheck_exit)
	{
		return -1;
	}
	for (i = 0; i < m_iEncChannelCnt; i++)
	{
		theCCapturePic[i]->CheckCapturePic();
	}

	return 0;
}

static void *Fnx_CheckScheduleThreadV2(void *lParam)
{
    PRT_TID_NAME("CC_V2_CheckScheduleThread");

	int ret = 0;

	CChannelManager *pccm = (CChannelManager *) lParam;

	if (NULL == pccm)
	{
		AVL_ERR("[CChannelManager:%s] check thread failed (NULL==pccm)\n", __FUNCTION__);
		return NULL;
	}
	AVL_WARN("[%s]threadid=%d \n", __FUNCTION__, getpid());

	while (1)
	{
		ret = pccm->CheckScheduleV2();
		if (ret < 0)			    //线程退出
		{
			AVL_ERR("[CChannelManager:%s] check thread quit\n", __FUNCTION__);
			break;
		}
		usleep(500000);				//500ms检测一次
	}
	return NULL;
}

static void *Fnx_CheckWatchThread(void *lParam)
{
    PRT_TID_NAME("CC_CheckWatchThread");

	int ret = 0;
	CChannelManager *pccm = (CChannelManager *) lParam;

	if (NULL == pccm)
	{
		AVL_ERR("[CChannelManager:%s] check thread failed (NULL==pccm)\n", __FUNCTION__);
		return NULL;
	}
	AVL_WARN("[%s]threadid=%d \n", __FUNCTION__, getpid());

	while (1)
	{
		ret = pccm->CheckWatch();
		if (ret < 0)			//线程退出
		{
			AVL_ERR("[CChannelManager:%s] check thread quit\n", __FUNCTION__);
			break;
		}
		sleep(1);				//一秒检测一次
	}
	return NULL;
}

static void *Fnx_CheckThread(void *lParam)
{
    PRT_TID_NAME("CC_CheckThread");

	int ret = 0;
	CChannelManager *pccm = (CChannelManager *) lParam;

	if (NULL == pccm)
	{
		AVL_ERR("[CChannelManager:%s] check thread failed (NULL==pccm)\n", __FUNCTION__);
		return NULL;
	}
	AVL_WARN("[%s]threadid=%d \n", __FUNCTION__, getpid());

	while (1)
	{
		ret = pccm->CheckStaus();
		if (ret < 0)			//线程退出
		{
			AVL_ERR("[CChannelManager:%s] check thread quit\n", __FUNCTION__);
			break;
		}
		sleep(1);				//一秒检测一次
	}
	return NULL;
}

static void *Fnx_CheckThread_capture(void *lParam)
{
    PRT_TID_NAME("CC_CaptureCheckThread");

	int ret = 0;
	CChannelManager *pccm = (CChannelManager *) lParam;

	if (NULL == pccm)
	{
		AVL_ERR("[CChannelManager:%s] check thread failed (NULL==pccm)\n", __FUNCTION__);
		return NULL;
	}
	AVL_WARN("[%s]threadid=%d \n", __FUNCTION__, getpid());

	while (1)
	{
		ret = pccm->CheckCapture();
		if (ret < 0)			//线程退出
		{
			AVL_ERR("[CChannelManager:%s] check thread quit\n", __FUNCTION__);
			break;
		}
		sleep(1);				//一秒检测一次
	}
	return NULL;
}
