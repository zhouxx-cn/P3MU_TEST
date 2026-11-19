
/*!
 *\file		CTransparentManager.h
 *\brief	透明通道管理类定义
 *\author	yuk@higheasyrd.com
 *\date		2009/05/05

功能：
-#	透明通道回调函数（设备个数、分区信息）

说明：
-#	最大支持两个透明通道 232, 485
 */

#include "idvr_errno.h"
#include "PUB/DVR_LIST.h"
#include "COM/CSerialDev.h"
#include "CTransparentManager.h"
#include "PTZTrace.h"
#include "COM/CSerialDev.h"
#include <time.h>
#include <string.h>
#include <PTZ/CPtzManager.h>
// #include "sys_banned.h"
#include "global_gather.h"


extern int rsleep(unsigned int ntime);

sem_t s_semChannel;

typedef struct tagTransparentChannel
{
	dvr_list_head tHead;
	CSerialDev *pDev;
	int nDataLen;
	unsigned long dwBaudRate;
	HANDLE hIndex;
	unsigned char byteData[TRANSPARENT_BUFF_SIZE];
	pthread_t tThread;
	int bRunning;
} TransparentChannel, *LPTransparentChannel;

static unsigned int s_nChannelIndex = 0;

CTransparentManager *CTransparentManager::m_pInstance = NULL;

dvr_list_head CTransparentManager::m_listChannel = { &CTransparentManager::m_listChannel, &CTransparentManager::m_listChannel };

pthread_mutex_t CTransparentManager::m_mtxChannel;

CTransparentManager::CTransparentManager()
{
	m_tNetInterface.ifTransparentChannelStart = ChannelCreateCallback;
	m_tNetInterface.ifTransparentChannelRead = ChannelReadCallback;
	m_tNetInterface.ifTransparentChannelDestroy = ChannelDestroyCallback;
}

CTransparentManager::~CTransparentManager()
{
}

CTransparentManager *CTransparentManager::GetInstance()
{
	if (NULL == m_pInstance)
	{
		m_pInstance = new CTransparentManager();
		if (NULL != m_pInstance)
		{
			INIT_DVR_LIST_HEAD(&m_listChannel);
			pthread_mutex_init(&m_mtxChannel, NULL);
			m_pInstance->m_ifTransparentChannelWrite = NULL;
		}
	}

	return m_pInstance;
}

void CTransparentManager::DestroyInstance(CTransparentManager * pInstance)
{
	if (pInstance != NULL)
	{
		dvr_list_head *pos = NULL;

		dvr_list_head *next = NULL;

		pthread_mutex_lock(&m_mtxChannel);
		dvr_list_for_each_safe(pos, next, &m_listChannel)
		{
			dvr_list_del(pos);
		}
		pthread_mutex_unlock(&m_mtxChannel);

		pthread_mutex_destroy(&m_mtxChannel);

		delete pInstance;
	}
}

const LPTransparentChanInterface CTransparentManager::GetNetInterface()
{
	return &m_tNetInterface;
}

int CTransparentManager::SetIFTransparentChannelWrite(IF_TransparentChannelWrite ifTransparentChannelWrite)
{
	m_ifTransparentChannelWrite = ifTransparentChannelWrite;
	return IDVR_OK;
}

static HANDLE *s_hIndex = NULL;

int CTransparentManager::Start()
{
	CSerialDev *pDev = CSerialDev::GetInstance(TTY422, 0);

	if (NULL == pDev)
	{
		PTZ_INFO("Get Serial Dev %d, Port 0 Instance Failed\n", TTY422);
		return IDVR_ERROR_ALLOC;
	}

	pDev->Open();
	LPTransparentChannel pChannel = new TransparentChannel();

	if (!pChannel)
	{
		PTZ_INFO("Create Transparent Channel Failed, Alloc Failed\n");
		return IDVR_ERROR_ALLOC;
	}

	pChannel->pDev = pDev;
	pChannel->dwBaudRate = 115200;
	pChannel->bRunning = 1;
	*s_hIndex = (HANDLE) pChannel;

	int ret = pthread_create(&(pChannel->tThread), NULL, ComRead, (void *) pChannel);

	if (ret != 0)
	{
		pChannel->bRunning = 0;
		delete pChannel;

		return -1;
	}

	dvr_list_add((dvr_list_head *) pChannel, &m_listChannel);

	// TODO: Add your specialized code here.
	return (int) 0;

}

int CTransparentManager::Stop()
{
	LPTransparentChannel pChannel = (LPTransparentChannel) s_hIndex;

	if (pChannel != NULL)
	{
		pChannel->bRunning = 0;
		pthread_join(pChannel->tThread, NULL);
		pChannel->pDev->Close();
		dvr_list_del((dvr_list_head *) pChannel);
		delete pChannel;

		pChannel = NULL;

		return 0;
	}

	// TODO: Add your specialized code here.
	return (int) -1;
}

//##ModelId=49B5DB500261
#ifdef SATA
int CTransparentManager::ChannelCreateCallback(LONG lUserID, HANDLE hIndex, HY_DVR_SERIALPORT_TYPE nType, int nPort)
#else
int CTransparentManager::ChannelCreateCallback(HANDLE * hIndex, unsigned long dwUserID, const LPTransparentParaInfo pTransparentParaInfo)
#endif
{
#ifdef SATA
	PTZ_DBG("lUserID = %ld, hIndex = %d, nType = %d, nPort = %d\n", lUserID, (int) hIndex, nType, nPort);
	CSerialDev *pDev = CSerialDev::GetInstance(nType, nPort);
#else
	PTZ_INFO("Create New Transparent Channel\n");
	int ret = -1;

	if (NULL == pTransparentParaInfo)
	{
		PTZ_ERROR("透明通道参数为空\n");
		return -1;
	}

	CreateTransparentParam tCreateParam;

	LPCreateTransparentParam p = NULL;

	//if (NULL != pTransparentParaInfo->cBuffer.pBuffer)
	//{
	//  p = (LPCreateTransparentParam)pTransparentParaInfo->cBuffer.pBuffer;
	//}
	//else
	//{
	tCreateParam.nSerialType = (int) pTransparentParaInfo->dwMajorType;
	tCreateParam.nSerialPort = (int) pTransparentParaInfo->dwMinorType;
	p = &tCreateParam;
	//}

	if ((-1 == p->nSerialType) || (-1 == p->nSerialPort))
	{
		PTZ_ERROR("错误的端口类型, SerialType = %d, Port = %d\n", p->nSerialType, p->nSerialPort);
		return -1;
	}

	CSerialDev *pDev = CSerialDev::GetInstance(p->nSerialType, 0);

	if (NULL == pDev)
	{
		PTZ_ERROR("获取对应的串口句柄失败, Type = %d, Port = %d\n", p->nSerialType, p->nSerialPort);
		return -1;
	}

	CPtzManager *pPTZManager = CPtzManager::PTZ_GetInstance();

	if (NULL == pPTZManager)
	{
		PTZ_ERROR("Can not get PTZManager\n");
		return -1;
	}
	unsigned long dwBaudRate = pPTZManager->PTZ_GetBaudrate(p->nSerialPort);

	if ((unsigned long) -1 == dwBaudRate)
	{
		PTZ_ERROR("Can not Get Baudrate, Port = %d\n", p->nSerialPort);
		return -1;
	}
#endif
	pthread_mutex_lock(&m_mtxChannel);
	LPTransparentChannel pChannel = IsValidDev(pDev);

	if (NULL != pChannel)
	{
		//有通道在使用该串口设备
		if (pChannel->bRunning)
		{
			pthread_mutex_unlock(&m_mtxChannel);
			PTZ_ERROR("This Dev is used by another transparent channel\n");
			return -1;
		}
	}
	else
	{
		pDev->Open();
		pChannel = new TransparentChannel();
		pChannel->pDev = pDev;
		sem_init(&s_semChannel, 0, 0);
		pthread_t tThread;

		int ret = pthread_create(&tThread, NULL, ComRead, (void *) pChannel);

		if (0 != ret)
		{
			delete pChannel;

			pChannel = NULL;
			PTZ_INFO("Transparent Channel Create Thread Failed\n");
		}
		else
		{
			sem_wait(&s_semChannel);
			dvr_list_add((dvr_list_head *) pChannel, &m_listChannel);
		}
		sem_destroy(&s_semChannel);
	}
	if (NULL != pChannel)
	{
#if 1
		int nDataLen;
		char Buffer[TRANSPARENT_BUFF_SIZE];

		do
			nDataLen = pChannel->pDev->RecvData(Buffer, TRANSPARENT_BUFF_SIZE);
		while(nDataLen == TRANSPARENT_BUFF_SIZE);
#endif
		pChannel->bRunning = 1;
		pChannel->dwBaudRate = dwBaudRate;
		pDev->GetWriteLock();
		pDev->SetBaudrate(pChannel->dwBaudRate);
		pDev->ReleaseWriteLock();
#ifdef SATA
		pChannel->hIndex = hIndex;
#else
		pChannel->hIndex = (HANDLE) GenerateIndex();
#endif
		ret = 0;
	}

	pthread_mutex_unlock(&m_mtxChannel);
#ifdef SATA
	return (int) 0;
#else
	*hIndex = pChannel->hIndex;
	// TODO: Add your specialized code here.
	return (int) ret;
#endif
}

//##ModelId=49B5DB6300CB
#ifdef SATA
int CTransparentManager::ChannelReadCallback(HANDLE hIndex, char *pBuffer, int nLength)
#else
int CTransparentManager::ChannelReadCallback(HANDLE hIndex, const LPTransparentParaInfo pTransparentParaInfo)
#endif
{
	pthread_mutex_lock(&m_mtxChannel);

	LPTransparentChannel pChannel = (LPTransparentChannel) IsValidIndex(hIndex);

	if ((NULL == pChannel) || (!pChannel->bRunning))
	{
		pthread_mutex_unlock(&m_mtxChannel);
		return -1;
	}
	pthread_mutex_unlock(&m_mtxChannel);
	pChannel->pDev->GetWriteLock();
	pChannel->pDev->Open();
	pChannel->pDev->SetBaudrate(pChannel->dwBaudRate);
#ifdef SATA
	PTZ_DBG("hIndex = %p, pBuffer=%p, nLength=%d\n", hIndex, pBuffer, nLength);
	int ret = pChannel->pDev->SendData(pBuffer, nLength);
#else
	int ret = pChannel->pDev->SendData(pTransparentParaInfo->cBuffer.pBuffer, pTransparentParaInfo->cBuffer.dwBufLen);
#endif
	pChannel->pDev->ReleaseWriteLock();

	// TODO: Add your specialized code here.

	PTZ_INFO("Read Data %lu Bytes, Write Data To Com, %d Bytes\n", pTransparentParaInfo->cBuffer.dwBufLen, ret);
	return (int) 0;
}

//##ModelId=49B5DB6B0128
#ifdef SATA
int CTransparentManager::ChannelDestroyCallback(HANDLE hIndex)
#else
int CTransparentManager::ChannelDestroyCallback(HANDLE hIndex, const LPTransparentParaInfo pTransparentParaInfo)
#endif
{
	PTZ_INFO("Destroy Channel hIndex = %p\n", hIndex);
	pthread_mutex_lock(&m_mtxChannel);

	LPTransparentChannel pChannel = (LPTransparentChannel) IsValidIndex(hIndex);

	if (pChannel != NULL)
	{
		//pChannel->pDev->Close();
		//pthread_rwlock_wrlock(&m_mtxChannel);
		if (IsValidDev(pChannel->pDev) == pChannel)
		{
			pChannel->bRunning = 0;
		}
		pthread_mutex_unlock(&m_mtxChannel);

		return 0;
	}

	pthread_mutex_unlock(&m_mtxChannel);
	// TODO: Add your specialized code here.
	return (int) -1;
}

void *CTransparentManager::ComRead(void *pArg)
{
	LPTransparentChannel pChannel = (LPTransparentChannel) pArg;

	sem_post(&s_semChannel);

	int nDataLen = 0;

	char Buffer[TRANSPARENT_BUFF_SIZE];
	pthread_detach(pthread_self());
	//int fs_sel;
	//fd_set    fs_read;
	//struct timeval tv_timeout;

	while (1)
	{
		//如果有几个透明通道，分别读取
		pChannel->pDev->GetWriteLock();
		pChannel->pDev->Open();
		//int nFd = pChannel->pDev->GetFd();
		pChannel->pDev->ReleaseWriteLock();
		if (!pChannel->bRunning)
		{
			rsleep(100000);
			continue;
		}
#if 0
		FD_ZERO(&fs_read);
		FD_SET(nFd, &fs_read);
		tv_timeout.tv_sec = TIMEOUT_SEC(TRANSPARENT_BUFF_SIZE, pChannel->dwBaudRate);
		tv_timeout.tv_usec = TIMEOUT_USEC;

		fs_sel = select(nFd + 1, &fs_read, NULL, NULL, &tv_timeout);
		pthread_mutex_lock(&m_mtxChannel);
		FD_CLR(nFd, &fs_read);
		if (!pChannel->bRunning)
		{
			pthread_mutex_unlock(&m_mtxChannel);
			rsleep(100000);
			continue;
		}

		pChannel->pDev->GetWriteLock();
		if (fs_sel > 0)
#endif
		{
			//pChannel->pDev->SetBaudrate(pChannel->dwBaudRate);
			//do
			//{
			//PTZ_INFO("Before Read Data From Com\n");
			nDataLen = pChannel->pDev->RecvData(Buffer, TRANSPARENT_BUFF_SIZE);
			if (nDataLen < 0)
			{
				PTZ_ERROR("Read Data From Com, nLength = %d\n", nDataLen);
			}
			else if (nDataLen > 0)
			{
				PTZ_INFO("Read Data From Com, nLength = %d\n", nDataLen);
				if (NULL != m_pInstance->m_ifTransparentChannelWrite)
				{
#if 0
					pChannel->pDev->ReleaseWriteLock();
#endif
#ifdef SATA
					m_pInstance->m_ifTransparentChannelWrite(pChannel->hIndex, Buffer, nDataLen);
#else
					TransparentParaInfo tWriteParam;

					tWriteParam.cBuffer.dwBufLen = nDataLen;
					tWriteParam.cBuffer.pBuffer = (unsigned char *) Buffer;
					PTZ_INFO("Write To Net, Leng = %lu, hIndex = %d\n", tWriteParam.cBuffer.dwBufLen, (int) (pChannel->hIndex));
					m_pInstance->m_ifTransparentChannelWrite(pChannel->hIndex, &tWriteParam);
#endif
#if 0
					pChannel->pDev->GetWriteLock();
#endif
				}
			}
			//} while (nDataLen > 0);
		}
#if 0
		pChannel->pDev->ReleaseWriteLock();
#endif
		rsleep(50000);
	}

	return NULL;
}

LPTransparentChannel CTransparentManager::IsValidDev(CSerialDev * pDev)
{
	dvr_list_head *pos = NULL;

	dvr_list_head *next = NULL;

	//检查是否已经被其他透明通道使用
	dvr_list_for_each_safe(pos, next, &m_listChannel)
	{
		LPTransparentChannel pChannel = (LPTransparentChannel) pos;

		if (pDev == pChannel->pDev)
		{
			return pChannel;
		}
	}

	return NULL;
}

LPTransparentChannel CTransparentManager::IsValidIndex(HANDLE hIndex)
{
	dvr_list_head *pos = NULL;

	dvr_list_head *next = NULL;

	//检查是否已经被其他透明通道使用
	dvr_list_for_each_safe(pos, next, &m_listChannel)
	{
		LPTransparentChannel pChannel = (LPTransparentChannel) pos;

		if (hIndex == pChannel->hIndex)
		{
			return pChannel;
		}
	}

	return NULL;
}

//void *CTransparentManager::GetChannel(HANDLE hIndex)
//{
//  dvr_list_head *pos = NULL;
//  dvr_list_head *next = NULL;
//  dvr_list_for_each_safe(pos, next, &m_listChannel)
//  {
//      LPTransparentChannel pChannel = (LPTransparentChannel)pos;
//      if (hIndex == pChannel->hIndex)
//      {
//          return (void *)pChannel;
//      }
//  }
//
//  return NULL;
//}

int CTransparentManager::GenerateIndex()
{
	while (1)
	{
		while ((0 == s_nChannelIndex) || ((unsigned int) -1 == s_nChannelIndex))
		{
			s_nChannelIndex = 1;
		}
		if (NULL != IsValidIndex((HANDLE) s_nChannelIndex))
		{
			s_nChannelIndex++;
			continue;
		}
		else
		{
			break;
		}
	}

	return s_nChannelIndex;
}
