// Copyright (C) 1991 - 1999 Rational Software Corporation

#include <string.h>
#include "PAR/CConfig.h"
#include "CPtzManager.h"
#include "CPtzDev.h"
#include "PTZTrace.h"
#include "zwsdk.h"
#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
#include "motoLib.h"
#include "mysystem.h"
#endif
// #include "sys_banned.h"
#include "global_gather.h"

CPtzDev::CPtzDev(int nChannel):m_nChannel(nChannel), m_nSerialType(TTY485), m_nSerialPort(0)
{
	memset(m_szProtocol, 0, sizeof (m_szProtocol) / sizeof (char));
}

CPtzDev::~CPtzDev()
{
}

int CPtzDev::Init(int nIndex)
{
	CConfig *pConfig = CConfig::GetInstance();

	CPtzManager *pPtzManager = CPtzManager::PTZ_GetInstance();

	char szSectionName[20];

	snprintf(szSectionName,sizeof(szSectionName), "PTZ_Set_%d", m_nChannel);
	m_dwAddress = pConfig->GetValue(szSectionName, "Address", nIndex);
	m_dwBaudRate = pConfig->GetValue(szSectionName, "Baudrate", 9600);
	m_nSerialType = pConfig->GetValue(szSectionName, "SerialType", TTY422);
	m_nSerialPort = pConfig->GetValue(szSectionName, "SerialPort", 0);
	
#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
	m_nHorizontalAngle = pConfig->GetValue(szSectionName, "HorizontalAngle", 198);
	m_nVerticalAngle = pConfig->GetValue(szSectionName, "VerticalAngle", 0);
    m_nHorizontalStep[PTZ_MAIN] = pConfig->GetValue(szSectionName, "HorizontalStep", 512);
    m_nVerticalStep[PTZ_MAIN] = pConfig->GetValue(szSectionName, "VerticalStep", 168);
	m_nHorizontalReverse[PTZ_MAIN] = pConfig->GetValue(szSectionName, "HorizontalReverse", 0);
	m_nVerticalReverse[PTZ_MAIN] = pConfig->GetValue(szSectionName, "VerticalReverse", 0);
	m_nMotoSpeedH[PTZ_MAIN] = pConfig->GetValue(szSectionName, "MotorSpeedH", 5);
    m_nUserVMaxStep = pConfig->GetValue(szSectionName, "UserVMaxStep", 168);

#if (DEVICE_SUPPORT_PTZ_NUM) >= 3
	m_nHorizontalStep[PTZ_LERF] = pConfig->GetValue(PTZ_MULTIDEV_CFG_SECTION, PTZ_LR_HORZ_MAXSTEP, 512);
	m_nVerticalStep[PTZ_LERF] = pConfig->GetValue(PTZ_MULTIDEV_CFG_SECTION, PTZ_LR_VERT_MAXSTEP, 168);
	m_nHorizontalReverse[PTZ_LERF] = pConfig->GetValue(PTZ_MULTIDEV_CFG_SECTION, PTZ_L_HORZ_REVERSE, 0);
	m_nVerticalReverse[PTZ_LERF] = pConfig->GetValue(PTZ_MULTIDEV_CFG_SECTION, PTZ_L_VERT_REVERSE, 0);
	m_nMotoSpeedH[PTZ_LERF] = pConfig->GetValue(PTZ_MULTIDEV_CFG_SECTION, PTZ_LR_HORZ_MAXSPEED, 5);
	m_nHorizontalStep[PTZ_RIGHT] = pConfig->GetValue(PTZ_MULTIDEV_CFG_SECTION, PTZ_LR_HORZ_MAXSTEP, 512);
	m_nVerticalStep[PTZ_RIGHT] = pConfig->GetValue(PTZ_MULTIDEV_CFG_SECTION, PTZ_LR_VERT_MAXSTEP, 168);
	m_nHorizontalReverse[PTZ_RIGHT] = pConfig->GetValue(PTZ_MULTIDEV_CFG_SECTION, PTZ_R_HORZ_REVERSE, 0);
	m_nVerticalReverse[PTZ_RIGHT] = pConfig->GetValue(PTZ_MULTIDEV_CFG_SECTION, PTZ_R_VERT_REVERSE, 0);
	m_nMotoSpeedH[PTZ_RIGHT] = pConfig->GetValue(PTZ_MULTIDEV_CFG_SECTION, PTZ_LR_HORZ_MAXSPEED, 5);
#endif

	for(int i = 0; i<MAX_CHANNEL_CNT;i++)
	{
		m_nMotoSpeedH[i] = m_nMotoSpeedH[i] > 9 ? 9 : (m_nMotoSpeedH[i] < 0 ? 0 : m_nMotoSpeedH[i]);
	}

	m_nMotoSpeedV[PTZ_MAIN] = pConfig->GetValue(szSectionName, "MotorSpeedV", 5);
	
#if (DEVICE_SUPPORT_PTZ_NUM) >= 3
	m_nMotoSpeedV[PTZ_LERF] = pConfig->GetValue(PTZ_MULTIDEV_CFG_SECTION, PTZ_LR_VERT_MAXSPEED, 5);
	m_nMotoSpeedV[PTZ_RIGHT] = pConfig->GetValue(PTZ_MULTIDEV_CFG_SECTION, PTZ_LR_VERT_MAXSPEED, 5);
#endif
	for(int i = 0; i<MAX_CHANNEL_CNT;i++)
	{
		m_nMotoSpeedV[i] = m_nMotoSpeedV[i] > 9 ? 9 : (m_nMotoSpeedV[i] < 0 ? 0 : m_nMotoSpeedV[i]);
	}


    // 用于记录当期模式和进入睡眠模式前电机位置
	m_nCurHorizontal = m_nHorizontalAngle;
	m_nCurVertical = m_nVerticalAngle;

	if (0 != pPtzManager->IsSupportBuiltInMotor())
	{		
		MOTO_Init();

        //从custom定制获取云台速度上限
        int nSpeedHmax = 0;
		CConfig *pCustomCfg = CConfig::NewInstance();
        int s32Ret = pCustomCfg->LoadFile(CUSTOM_CONFIG_FILE);
        if (0 == s32Ret)
        {
            nSpeedHmax = pCustomCfg->GetValue("PTZ_Set_0", "MaxMotorSpeedH", MOTO_SPEED_H_MAX);
        }
        else
        {
            s32Ret = pCustomCfg->LoadFile(CUSTOM_BACKUP_CONFIGMODE_FILE);
            if (0 == s32Ret)
            {
                nSpeedHmax = pCustomCfg->GetValue("PTZ_Set_0", "MaxMotorSpeedH", MOTO_SPEED_H_MAX);
            }
        }
        CConfig::ReleaseInstance(pCustomCfg);
        pCustomCfg = NULL;
        //设置云台速度上限
		MOTO_SetSpeedMax(nSpeedHmax, MOTO_SPEED_V_MAX);
        MOTO_SetSpeedType(m_nHorizontalStep[PTZ_MAIN], m_nVerticalStep[PTZ_MAIN]);
		for(int i = 0;i < MAX_CHANNEL_CNT;i++)
		{
			MOTO_SetVerticalReverse(m_nVerticalReverse[i],i);
			MOTO_SetSpeed(m_nMotoSpeedH[i], m_nMotoSpeedV[i],i);
			MOTO_SetMaxPos(m_nHorizontalStep[i], m_nVerticalStep[i],i);
#if (DEVICE_SUPPORT_PTZ_NUM) >= 3
			if(i == PTZ_LERF || i == PTZ_RIGHT)
			{
				MOTO_SetPos((m_nHorizontalStep[i]/2), (m_nVerticalStep[i]/2),i);
				PTZ_DBG(" MOTO_SetPos [%d] [%d] \n",(m_nHorizontalStep[i]/2), (m_nVerticalStep[i]/2));
			}
#endif
		}

		MOTO_SetUserVMaxPos(m_nUserVMaxStep);
#ifdef __AGINGTEST__
		if (access(SD_AGING_TEST_LIST_FILE_NAME, F_OK) != 0)
#endif
		{
			if ((m_nHorizontalAngle > m_nHorizontalStep[PTZ_MAIN]) || (m_nVerticalAngle > m_nVerticalStep[PTZ_MAIN]))
			{
				m_nHorizontalAngle = m_nHorizontalStep[PTZ_MAIN] / 2;
				m_nVerticalAngle = m_nVerticalStep[PTZ_MAIN] / 2;
			}
			MOTO_SelfTest(PTZ_MAIN);
			MOTO_ToStartPos(m_nHorizontalAngle, m_nVerticalAngle);

		}
	}
#endif

	bool bSearched = false;

	//有内部协议
	if (0 != pPtzManager->m_nProtocolCount[0])
	{
		pConfig->GetValue(szSectionName, "Protocol", DEFAULT_PROTOCOL, m_szProtocol, 256);
		if (!(pPtzManager->IsProtocolExist(m_szProtocol)))
		{
			int i = 0;

			while (i < pPtzManager->m_nProtocolCount[0])
			{
				if (pPtzManager->m_aszProtocol[0][i][0] != '_')
				{
					bSearched = true;
					strncpy(m_szProtocol, pPtzManager->m_aszProtocol[0][i], sizeof(m_szProtocol) - 1);
					break;
				}
				i++;
			}
		}
		else
		{
			bSearched = true;
		}
	}
	//没有内部协议，有外部协议
	if ((!bSearched) && (0 != pPtzManager->m_nProtocolCount[1]))
	{
		pConfig->GetValue(szSectionName, "Protocol", DEFAULT_PROTOCOL, m_szProtocol, 256);
		if (!(pPtzManager->IsProtocolExist(m_szProtocol)))
		{
			bSearched = true;
			strncpy(m_szProtocol, pPtzManager->m_aszProtocol[1][0], sizeof(m_szProtocol) - 1);
		}
	}

	if (!bSearched)
	{
		memset(m_szProtocol, 0, 256);
	}

	PTZ_DBG("Channel = %d, m_szProtocol = %s, m_dwAddress = %lu, m_dwBaudRate = %lu\n", m_nChannel, m_szProtocol, m_dwAddress, m_dwBaudRate);
	return (int) 0;
}

//##ModelId=49B720CA0399
int CPtzDev::GetProtocol(char *strProtocol, int nLength)
{
	// TODO: Add your specialized code here.
	strncpy(strProtocol, m_szProtocol, nLength - 1);
	strProtocol[nLength - 1] = '\0';
	return (int) 0;
}

//##ModelId=49A3A4060232
int CPtzDev::SetProtocol(char *strProtocol)
{
	// TODO: Add your specialized code here.
	char szSectionName[20];

	snprintf(szSectionName,sizeof(szSectionName), "PTZ_Set_%d", m_nChannel);
	CConfig *pConfig = CConfig::GetInstance();

	pConfig->SetValue(szSectionName, "Protocol", strProtocol);
	strncpy(m_szProtocol, strProtocol, sizeof(m_szProtocol) - 1);
	return (int) 0;
}

bool CPtzDev::UseProtocol(char *szProtocol)
{
	return (0 == strcmp(szProtocol, m_szProtocol));
}

//##ModelId=49B7202C01B5
unsigned long CPtzDev::GetAddress()
{
	// TODO: Add your specialized code here.
	return (int) m_dwAddress;
}

//##ModelId=49B72011004E
int CPtzDev::SetAddress(unsigned long dwAddress)
{
	// TODO: Add your specialized code here.
	char szSectionName[20];

	snprintf(szSectionName,sizeof(szSectionName), "PTZ_Set_%d", m_nChannel);
	CConfig *pConfig = CConfig::GetInstance();

	pConfig->SetValue(szSectionName, "Address", (int) dwAddress);
	m_dwAddress = dwAddress;
	return (int) 0;
}

//##ModelId=49B72032031C
unsigned long CPtzDev::GetBaudRate()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return m_dwBaudRate;
}

//##ModelId=49B720180399
int CPtzDev::SetBaudRate(unsigned long dwBaudRate)
{
	// TODO: Add your specialized code here.
	m_dwBaudRate = dwBaudRate;
	char szSectionName[20];

	snprintf(szSectionName,sizeof(szSectionName), "PTZ_Set_%d", m_nChannel);
	CConfig *pConfig = CConfig::GetInstance();

	pConfig->SetValue(szSectionName, "Baudrate", (int) dwBaudRate);
	return (int) 0;
}

int CPtzDev::SetSerialDev(int nSerialType, int nPort)
{
	char szSectionName[20];

	snprintf(szSectionName,sizeof(szSectionName), "PTZ_Set_%d", m_nChannel);
	CConfig *pConfig = CConfig::GetInstance();

	pConfig->SetValue(szSectionName, "SerialType", (int) nSerialType);
	pConfig->SetValue(szSectionName, "SerialPort", (int) nPort);
	m_nSerialType = nSerialType;
	m_nSerialPort = nPort;
	return 0;
}

int CPtzDev::GetSerialDev(int *nSerialType, int *nPort)
{
	if (nSerialType != NULL)
	{
		*nSerialType = m_nSerialType;
	}

	if (nPort != NULL)
	{
		*nPort = m_nSerialPort;
	}

	return 0;
}

#if defined(EGUI_FOR_ICAMERA_NXP8850_BASED_DVR)
//!\brief 外连rsleep 定义在STR中:(
extern void rsleep(unsigned int utime);

static void *thread_open_com(void *data)
{
	PRT_TID();
	CSerialDev *pDev = (CSerialDev *) data;

	while (1)
	{
		pDev->GetWriteLock();

		int nRet = pDev->Open();

		pDev->ReleaseWriteLock();

		if (-1 != nRet)
		{
			break;
		}

		rsleep(1 * 1000);
	}

	return NULL;
}
#endif

int CPtzDev::Open()
{
	int nBoardID = 0;
	ZWSys_GetBoardInfo(&nBoardID);
	
	int port = 0;
	if(1 == m_nSerialPort)
		port = 0;
	else
		port = 1;
		
	CSerialDev *pDev = CSerialDev::GetInstance(m_nSerialType, m_nSerialPort);

	if (NULL == pDev)
	{
		return -1;
	}

	//创建一个独立线程去打开/dev/ttyS0
	pthread_attr_t new_attr;

	pthread_t new_thread;

	pthread_attr_init(&new_attr);
	pthread_attr_setdetachstate(&new_attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&new_thread, &new_attr, thread_open_com, (void *) pDev);
	pthread_attr_destroy(&new_attr);


	if((0x2036 == nBoardID)||(0x2035 == nBoardID)||(0x2037 == nBoardID)||(0x2047 == nBoardID))
	{
		CSerialDev *pDev1 = CSerialDev::GetInstance(m_nSerialType, port);

		if (NULL == pDev1)
		{
			return -1;
		}	
		//创建一个独立线程去打开/dev/ttyS1
		pthread_attr_t new_attr1;

		pthread_t new_thread1;

		pthread_attr_init(&new_attr1);
		pthread_attr_setdetachstate(&new_attr1, PTHREAD_CREATE_DETACHED);
		pthread_create(&new_thread1, &new_attr1, thread_open_com, (void *) pDev1);
		pthread_attr_destroy(&new_attr1);
	}
	return 0;
}

//##ModelId=49A3B6AF035B
int CPtzDev::SendData(void *pBuf, int nLength)
{
	int nBoardID = 0;
	ZWSys_GetBoardInfo(&nBoardID);
	int port = 0;
	if(1 == m_nSerialPort)
		port = 0;
	else
		port = 1;

	CSerialDev *pDev = CSerialDev::GetInstance(m_nSerialType, m_nSerialPort);
	int ret;
	if (NULL == pDev)
	{
		return -1;
	}

	pDev->GetWriteLock();
	ret = pDev->Open();
	ret = pDev->SetBaudrate(m_dwBaudRate);
	ret = pDev->SendData(pBuf, nLength);
    if (ret < 0)
    {
        PTZ_ERROR("Ptz Send Data Err\n");
    }
#if defined(EGUI_FOR_ICAMERA_NXP8850_BASED_DVR)
	// sleep 40ms
	//rsleep(40000);
#endif
	pDev->ReleaseWriteLock();

	if((0x2036 == nBoardID)||(0x2035 == nBoardID)||(0x2037 == nBoardID)||(0x2047 == nBoardID))
	{
		CSerialDev *pDev1 = CSerialDev::GetInstance(m_nSerialType, port);
		if (NULL == pDev1)
		{
			PTZ_ERROR("Instance error\n");
			return -1;
		}

		pDev1->GetWriteLock();
		ret = pDev1->Open();

		ret = pDev1->SetBaudrate(m_dwBaudRate);
		ret = pDev1->SendData(pBuf, nLength);
#if defined(EGUI_FOR_ICAMERA_NXP8850_BASED_DVR)
		// sleep 40ms
		rsleep(40000);
#endif
		pDev1->ReleaseWriteLock();	
	}
	return (int) 0;
}

//##ModelId=49A3C326034B
int CPtzDev::RecvData(void *pBuf, int nLength)
{
	// TODO: Add your specialized code here.

	int nBoardID = 0;
	ZWSys_GetBoardInfo(&nBoardID);	
	int ret = 0;
	int port = 0;
	if(1 == m_nSerialPort)
		port = 0;
	else
		port = 1;
		
	CSerialDev *pDev = CSerialDev::GetInstance(m_nSerialType, m_nSerialPort);

	if (NULL == pDev)
	{
		return -1;
	}

	pDev->GetReadLock();
	pDev->Open();
	pDev->SetBaudrate(m_dwBaudRate);
	ret = pDev->RecvData(pBuf, nLength);
	pDev->ReleaseReadLock();

	if((0x2036 == nBoardID)||(0x2035 == nBoardID)||(0x2037 == nBoardID)||(0x2047 == nBoardID))
	{
		CSerialDev *pDev1 = CSerialDev::GetInstance(m_nSerialType, port);

		if (NULL == pDev1)
		{
			return -1;
		}

		pDev1->GetWriteLock();
		pDev1->Open();
		pDev1->SetBaudrate(m_dwBaudRate);
		ret = pDev->RecvData(pBuf, nLength);
		pDev1->ReleaseWriteLock();	
	}
	return (int) ret;
}

Cruise *CPtzDev::GetCruise(unsigned long dwCruiseIndex)
{
	return &(m_tCruise[dwCruiseIndex]);
}

CruiseTemporary *CPtzDev::GetTempCruise()
{
	return &m_tTempCruise;
}

void CPtzDev::ClearTempCruise()
{
	m_tTempCruise.nCruiseIndex = -1;
	m_tTempCruise.tCruise.nPointCount = 0;
}

int CPtzDev::SetCruise(unsigned long nCruiseIndex)
{
	ClearTempCruise();
	PTZ_DBG("Begin Set Cruise %lu\n", nCruiseIndex);
	m_tTempCruise.nCruiseIndex = nCruiseIndex;
	return 0;
}

int CPtzDev::AddCruisePoint(unsigned long CruiseIndex, unsigned long PresetPointId, unsigned long dwSpeed, unsigned long dwDwellTime)
{
	PTZ_DBG("m_tTempCruise.nCruiseIndex = %d\n", m_tTempCruise.nCruiseIndex);
	if (-1 == m_tTempCruise.nCruiseIndex)
	{
		return -1;
	}

	PTZ_DBG("Add Cruise Point, index = %d, id = %lu, speed = %lu, time = %lu", m_tTempCruise.nCruiseIndex, PresetPointId, dwSpeed, dwDwellTime);

	if (m_tTempCruise.nCruiseIndex == (int) CruiseIndex)
	{
		m_tTempCruise.tCruise.aCruisePoint[m_tTempCruise.tCruise.nPointCount].dwPresetId = PresetPointId;
		m_tTempCruise.tCruise.aCruisePoint[m_tTempCruise.tCruise.nPointCount].dwSpeed = dwSpeed;
		m_tTempCruise.tCruise.aCruisePoint[m_tTempCruise.tCruise.nPointCount].dwDwellTime = dwDwellTime;
		m_tTempCruise.tCruise.nPointCount++;
		return 0;
	}

	return -1;
}

int CPtzDev::SaveCruise(unsigned long dwCruiseIndex)
{
	int ret = 0;

	PTZ_DBG("m_tTempCruise.nCruiseIndex = %d\n", m_tTempCruise.nCruiseIndex);
	if (m_tTempCruise.nCruiseIndex == (int) dwCruiseIndex)
	{
		int i = 0;

		memcpy(&(m_tCruise[m_tTempCruise.nCruiseIndex]), &(m_tTempCruise.tCruise), sizeof (Cruise));
		PTZ_DBG("Save Cruise %lu", dwCruiseIndex);
		for (i = 0; i < MAX_CRUISE_POINT; i++)
		{
			PTZ_DBG("%d ", m_tCruise[dwCruiseIndex].aCruisePoint[i].dwPresetId);
		}
		PTZ_DBG("\n");
	}
	else
	{
		ret = -1;
	}

	ClearTempCruise();
	return ret;
}

int CPtzDev::ClearCruise(unsigned long dwCruiseIndex)
{
	ClearTempCruise();
	m_tCruise[dwCruiseIndex].nPointCount = 0;
    for (size_t i = 0; i < MAX_CRUISE_POINT; ++i) {
        m_tCruise[dwCruiseIndex].aCruisePoint[i] = CruisePoint{};
    }
	return 0;
}

int CPtzDev::GetDwellTime()
{
	int nTime = 0;

	int nNextPoint = m_nCurPoint;

	int nPresetId = 0;

	do
	{
		m_nCurPoint = nNextPoint;
		//PTZ_DBG("nPresetId = %d\n", nPresetId);
		nTime += m_tCruise[m_nCurCruise].aCruisePoint[m_nCurPoint].dwDwellTime;
		nPresetId = m_tCruise[m_nCurCruise].aCruisePoint[m_nCurPoint].dwPresetId;
		nNextPoint = (m_nCurPoint + 1) % m_tCruise[m_nCurCruise].nPointCount;
	}
	while ((nTime <= 6) && (nPresetId == m_tCruise[m_nCurCruise].aCruisePoint[nNextPoint].dwPresetId));
	//如果该预置点滞留时间大于6秒钟，或者该预置点号与下个预置点号不同，退出循环

	return nTime;
}

int CPtzDev::NextPoint()
{
	m_nCurPoint = (m_nCurPoint + 1) % m_tCruise[m_nCurCruise].nPointCount;
	return m_nCurPoint;
}

int CPtzDev::GetPresetId()
{
	return m_tCruise[m_nCurCruise].aCruisePoint[m_nCurPoint].dwPresetId;
}

unsigned long CPtzDev::GetSpeed()
{
	return m_tCruise[m_nCurCruise].aCruisePoint[m_nCurPoint].dwSpeed;
}

int CPtzDev::SetCurCruise(int nCruise)
{
	m_nCurCruise = (int) nCruise;
	m_nCurPoint = 0;
	return 0;
}

int CPtzDev::GetCurCruise()
{
	return m_nCurCruise;
}

#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
int CPtzDev::SetHorizontalAngle(int nHorizontalAngle)
{
	// TODO: Add your specialized code here.
	char szSectionName[20];

	snprintf(szSectionName,sizeof(szSectionName), "PTZ_Set_%d", m_nChannel);
	CConfig *pConfig = CConfig::GetInstance();

	pConfig->SetValue(szSectionName, "HorizontalAngle", (int) nHorizontalAngle);
	pConfig->SaveFile();

	m_nHorizontalAngle = nHorizontalAngle;
	return 0;
}

int CPtzDev::SetVerticalAngle(int nVerticalAngle)
{
	// TODO: Add your specialized code here.
	char szSectionName[20];

	snprintf(szSectionName,sizeof(szSectionName), "PTZ_Set_%d", m_nChannel);
	CConfig *pConfig = CConfig::GetInstance();

	pConfig->SetValue(szSectionName, "VerticalAngle", (int) nVerticalAngle);
	pConfig->SaveFile();
	m_nVerticalAngle = nVerticalAngle;
	return 0;

}

int CPtzDev::SetHorizontalReverse(int nReverse,int Chn)
{
    if ( m_nHorizontalReverse[Chn] == nReverse )
    {
        return 0;
    }

	m_nHorizontalReverse[Chn] = nReverse;
	CConfig *pConfig = CConfig::GetInstance();

	switch(Chn)
	{
		case PTZ_MAIN:
		{
			char szSectionName[20];
			snprintf(szSectionName, sizeof(szSectionName), "PTZ_Set_%d", m_nChannel);
			pConfig->SetValue(szSectionName, "HorizontalReverse", (int) nReverse);
			break;
		}
#if (DEVICE_SUPPORT_PTZ_NUM) >= 3
		case PTZ_LERF:
		{
			pConfig->SetValue(PTZ_MULTIDEV_CFG_SECTION, PTZ_L_HORZ_REVERSE, (int) nReverse);
			break;
		}

		case PTZ_RIGHT:
		{
			pConfig->SetValue(PTZ_MULTIDEV_CFG_SECTION, PTZ_R_HORZ_REVERSE, (int) nReverse);
			break;
		}
#endif
	}
	pConfig->SaveFile();
	return 0;
}
//!\brief 获取水平电机是否反转
int CPtzDev::GetHorizontalReverse(int Chn)
{
	return m_nHorizontalReverse[Chn];
}

int CPtzDev::SetVerticalReverse(int nReverse,int Chn)
{
    if (m_nVerticalReverse[Chn] == nReverse)
    {
        return 0;
    }

	m_nVerticalReverse[Chn] = nReverse;
	CConfig *pConfig = CConfig::GetInstance();

	switch(Chn)
	{
		case PTZ_MAIN:
		{
			char szSectionName[20];
			snprintf(szSectionName, sizeof(szSectionName), "PTZ_Set_%d", m_nChannel);
			pConfig->SetValue(szSectionName, "VerticalReverse", (int) nReverse);
			break;
		}
#if (DEVICE_SUPPORT_PTZ_NUM) >= 3
		case PTZ_LERF:
		{
			pConfig->SetValue(PTZ_MULTIDEV_CFG_SECTION, PTZ_L_VERT_REVERSE, (int) nReverse);
			break;
		}
		case PTZ_RIGHT:
		{
			pConfig->SetValue(PTZ_MULTIDEV_CFG_SECTION, PTZ_R_VERT_REVERSE, (int) nReverse);
			break;
		}
#endif
	}

	pConfig->SaveFile();

    MOTO_SetVerticalReverse(m_nVerticalReverse[Chn],Chn);
	return 0;
}

//!\brief 获取垂直电机是否反转
int CPtzDev::GetVerticalReverse(int Chn)
{
	return m_nVerticalReverse[Chn];
}

int CPtzDev::SetHorizontalStep(int nHorizontalStep,int Chn)
{
    if (m_nHorizontalStep[Chn] == nHorizontalStep)
    {
        return 0;
    }
	CConfig *pConfig = CConfig::GetInstance();

	switch(Chn)
	{
		case PTZ_MAIN:
		{
			char szSectionName[20];
			snprintf(szSectionName, sizeof(szSectionName), "PTZ_Set_%d", m_nChannel);
			pConfig->SetValue(szSectionName, "HorizontalStep", (int) nHorizontalStep);
			pConfig->SaveFile();
			m_nHorizontalStep[PTZ_MAIN] = nHorizontalStep;
			MOTO_SetMaxPos(m_nHorizontalStep[PTZ_MAIN], m_nVerticalStep[PTZ_MAIN],PTZ_MAIN);
			break;
		}
#if (DEVICE_SUPPORT_PTZ_NUM) >= 3
		case PTZ_LERF:
		case PTZ_RIGHT:
		{
			pConfig->SetValue(PTZ_MULTIDEV_CFG_SECTION, PTZ_LR_HORZ_MAXSTEP, (int) nHorizontalStep);
			pConfig->SaveFile();
			m_nHorizontalStep[PTZ_LERF] = nHorizontalStep;
			m_nHorizontalStep[PTZ_RIGHT] = nHorizontalStep;
			MOTO_SetMaxPos(m_nHorizontalStep[PTZ_LERF], m_nVerticalStep[PTZ_LERF],PTZ_LERF);
			MOTO_SetMaxPos(m_nHorizontalStep[PTZ_RIGHT], m_nVerticalStep[PTZ_RIGHT],PTZ_RIGHT);
			break;
		}
#endif
	}
	return 0;
}

int CPtzDev::SetVerticalStep(int nVerticalStep,int Chn)
{
    if (m_nVerticalStep[Chn] == nVerticalStep)
    {
        return 0;
    }
	CConfig *pConfig = CConfig::GetInstance();
	switch(Chn)
	{
		case PTZ_MAIN:
		{
			char szSectionName[20];
			snprintf(szSectionName, sizeof(szSectionName), "PTZ_Set_%d", m_nChannel);
			pConfig->SetValue(szSectionName, "HorizontalStep", (int) nVerticalStep);
			pConfig->SaveFile();
			m_nVerticalStep[PTZ_MAIN] = nVerticalStep;
			MOTO_SetMaxPos(m_nHorizontalStep[PTZ_MAIN], m_nVerticalStep[PTZ_MAIN],PTZ_MAIN);
			break;
		}
#if (DEVICE_SUPPORT_PTZ_NUM) >= 3
		case PTZ_LERF:
		case PTZ_RIGHT:
		{
			pConfig->SetValue(PTZ_MULTIDEV_CFG_SECTION, PTZ_LR_VERT_MAXSTEP, (int) nVerticalStep);
			pConfig->SaveFile();
			m_nVerticalStep[PTZ_LERF] = nVerticalStep;
			m_nVerticalStep[PTZ_RIGHT] = nVerticalStep;
			MOTO_SetMaxPos(m_nHorizontalStep[PTZ_LERF], m_nVerticalStep[PTZ_LERF],PTZ_LERF);
			MOTO_SetMaxPos(m_nHorizontalStep[PTZ_RIGHT], m_nVerticalStep[PTZ_RIGHT],PTZ_RIGHT);
			break;
		}
#endif

	}
	return 0;
}

int CPtzDev::SetUserVMaxStep(int nUserVMaxStep)
{
    if(m_nUserVMaxStep == nUserVMaxStep)
    {
        return 0;
    }

	char szSectionName[20];
	snprintf(szSectionName, sizeof(szSectionName), "PTZ_Set_%d", m_nChannel);
	CConfig *pConfig = CConfig::GetInstance();
	pConfig->SetValue(szSectionName, "UserVMaxStep", (int) nUserVMaxStep);
	pConfig->SaveFile();

	m_nUserVMaxStep = nUserVMaxStep;
    
    MOTO_SetUserVMaxPos(m_nUserVMaxStep);
    
	return 0;
}

int CPtzDev::SetMaxSpeedV(int nMaxSpeedV,int Chn)
{
    if(m_nMotoSpeedV[Chn] == nMaxSpeedV)
    {
        return 0;
    }
	CConfig *pConfig = CConfig::GetInstance();
	switch(Chn)
	{
		case PTZ_MAIN:
		{
			char szSectionName[20];
			snprintf(szSectionName, sizeof(szSectionName), "PTZ_Set_%d", m_nChannel);
			pConfig->SetValue(szSectionName, "MotorSpeedH", (int) nMaxSpeedV);
			pConfig->SaveFile();
			m_nMotoSpeedV[PTZ_MAIN] = nMaxSpeedV;
			MOTO_SetSpeed(m_nMotoSpeedH[PTZ_MAIN], m_nMotoSpeedV[PTZ_MAIN],PTZ_MAIN);
			break;
		}
#if (DEVICE_SUPPORT_PTZ_NUM) >= 3
		case PTZ_LERF:
		case PTZ_RIGHT:
		{
			pConfig->SetValue(PTZ_MULTIDEV_CFG_SECTION, PTZ_LR_VERT_MAXSPEED, (int) nMaxSpeedV);
			pConfig->SaveFile();
			m_nMotoSpeedV[PTZ_LERF] = nMaxSpeedV;
			m_nMotoSpeedV[PTZ_RIGHT] = nMaxSpeedV;
			MOTO_SetSpeed(m_nMotoSpeedH[PTZ_LERF], m_nMotoSpeedV[PTZ_LERF],PTZ_LERF);
			MOTO_SetSpeed(m_nMotoSpeedH[PTZ_RIGHT], m_nMotoSpeedV[PTZ_RIGHT],PTZ_RIGHT);
			break;
		}
#endif
	}
	return 0;
}


int CPtzDev::SetMaxSpeedH(int nMaxSpeedH,int Chn)
{
    if(m_nMotoSpeedH[Chn] == nMaxSpeedH)
    {
        return 0;
    }
	CConfig *pConfig = CConfig::GetInstance();
	switch(Chn)
	{
		case PTZ_MAIN:
		{
			char szSectionName[20];
			snprintf(szSectionName, sizeof(szSectionName), "PTZ_Set_%d", m_nChannel);
			pConfig->SetValue(szSectionName, "MotorSpeedH", (int) nMaxSpeedH);
			pConfig->SaveFile();
			m_nMotoSpeedV[PTZ_MAIN] = nMaxSpeedH;
			MOTO_SetSpeed(m_nMotoSpeedH[PTZ_MAIN], m_nMotoSpeedV[PTZ_MAIN],PTZ_MAIN);
			break;
		}
#if (DEVICE_SUPPORT_PTZ_NUM) >= 3
		case PTZ_LERF:
		case PTZ_RIGHT:
		{
			pConfig->SetValue(PTZ_MULTIDEV_CFG_SECTION, PTZ_LR_HORZ_MAXSPEED, (int) nMaxSpeedH);
			pConfig->SaveFile();
			m_nMotoSpeedH[PTZ_LERF] = nMaxSpeedH;
			m_nMotoSpeedH[PTZ_RIGHT] = nMaxSpeedH;
			MOTO_SetSpeed(m_nMotoSpeedH[PTZ_LERF], m_nMotoSpeedV[PTZ_LERF],PTZ_LERF);
			MOTO_SetSpeed(m_nMotoSpeedH[PTZ_RIGHT], m_nMotoSpeedV[PTZ_RIGHT],PTZ_RIGHT);
			break;
		}
#endif
	}
	return 0;
}


//将预置点位置作为新的位置记录点
//保存至配置文件
int CPtzDev::SavePresetAsNewAngle(unsigned long dwPresetIndex)
{
	int nRet = 0;
	int nHorizontalAngle = 0;
	int nVerticalAngle = 0;
	char szSectionName[256] = {0};
	CConfig *pConfig = CConfig::GetInstance();
	CPtzManager *pPtzManager = CPtzManager::PTZ_GetInstance();

	if (NULL == pConfig
		|| NULL == pPtzManager)
	{
		return -1;
	}
	
	if (0 != pPtzManager->IsSupportBuiltInMotor())
	{
		//获取当前预置点坐标
		nRet = MOTO_GetPresetPos(dwPresetIndex, &nHorizontalAngle, &nVerticalAngle);
		if (0 == nRet)
		{
			//同步至电机位置配置
			m_nHorizontalAngle = nHorizontalAngle;
			m_nVerticalAngle = nVerticalAngle;
			snprintf(szSectionName,sizeof(szSectionName), "PTZ_Set_%d", m_nChannel);
			pConfig->SetValue(szSectionName, "HorizontalAngle", m_nHorizontalAngle);
			pConfig->SetValue(szSectionName, "VerticalAngle", m_nVerticalAngle);
			pConfig->SaveFile();
		}
		else
		{
			return -1;
		}
	}
	
	return 0;
}

#ifdef __AGINGTEST__
int CPtzDev::GetMaxStep (int *pnHMaxStop, int *pnVMaxStep,int Chn)
{
	if (NULL == pnHMaxStop || NULL == pnVMaxStep)
	{
		PTZ_ERROR ("GetMaxStep error\n");
		return -1;
	}

	*pnHMaxStop = m_nHorizontalStep[Chn];
	*pnVMaxStep = m_nVerticalStep[Chn];

	return 0;
}
#endif
int CPtzDev::PrivacyMaskPreset(int AwakeMode)
{
    int iState = 0;
	int iCount = 0;

	// 可在云台停止之后，再执行
	while(1)
	{
		MOTO_GetState(&iState);
		if (0 == iState)
		{
			PTZ_INFO("the ptz has stopped\n");
			break;
		}
		else
		{
			iCount++;
			if(iCount >= 200)
			{
				break;
			}
		}
		usleep(100000);
	}

	if (AwakeMode)
	{
		// 执行唤醒
		MOTO_ToPos(m_nCurHorizontal, m_nCurVertical);
	}
	else
	{
		// 执行休眠,记录当前位置。
		MOTO_GetPos(&m_nCurHorizontal, &m_nCurVertical);

        if (1 == m_nVerticalReverse[PTZ_MAIN])
        {
            MOTO_ToPos(m_nCurHorizontal, m_nVerticalStep[PTZ_MAIN]);
        }
        else
        {
            MOTO_ToPos(m_nCurHorizontal, 0);
        }
	}

	return 0;
}

#endif
