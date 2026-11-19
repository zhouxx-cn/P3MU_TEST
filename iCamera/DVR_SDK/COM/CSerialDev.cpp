
/*!
 *\file		CSerialDev.cpp
 *\brief	串口设备管理类函数定义
 *\author	yuk@higheasyrd.com
 *\date		2009/05/05
 */

#include <time.h>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <errno.h>
#include "global_macro.h"
#include "CSerialDev.h"
#include "COMTrace.h"
#ifdef SATA
#include <PRW/tmsdk.h>
#else
//#include "hisdk.h"
#include "zwsdk.h"
#endif

#define TIMEOUT_SEC(datalen, baudrate) ((datalen) * 20 / (baudrate) + 1)	//接收超时


extern void rsleep(unsigned int ntime);

CSerialDev *CSerialDev::m_pDev[MAX_RS_TYPE][MAX_RS_CHANNEL] = { {NULL}, {NULL}, {NULL} };

int CSerialDev::m_bInitialized = false;

CSerialDev::CSerialDev():m_dwBaudrate(0), m_nFd(-1)
{
	pthread_mutex_init(&m_mtxComWrite, NULL);
	pthread_mutex_init(&m_mtxComRead, NULL);
}

CSerialDev::~CSerialDev()
{
	Close();
}

//##ModelId=49B8633001E4
CSerialDev *CSerialDev::GetInstance(int nType, int nPort)
{
	if ((nType < TTY232) || (nType > TTY422) || (nPort >= MAX_RS_CHANNEL) || (nPort < 0))
	{
		return NULL;
	}

	if (NULL == m_pDev[nType][nPort])
	{
		int nNode = GetTtyDevice(nType, nPort);

		if ((nNode >= MAX_RS_CHANNEL) || (nNode < 0))
		{
			return NULL;
		}

		m_pDev[nType][nPort] = new CSerialDev();
		m_pDev[nType][nPort]->m_nNode = GetTtyDevice(nType, nPort);
	}

	// TODO: Add your specialized code here.
	return (CSerialDev *) (m_pDev[nType][nPort]);
}

void CSerialDev::DestroyInstance(CSerialDev * pInstance)
{
	if (pInstance != NULL)
	{
		delete pInstance;
	}
}

//##ModelId=49A36B870157
int CSerialDev::Open()
{
	int nBoardID = 0;
	ZWSys_GetBoardInfo(&nBoardID);
	//如果模块没初始化或者已逆初始化则直接返回
	if (!m_bInitialized)
	{
		return -1;
	}

	if (-1 != m_nFd)
	{
		return m_nFd;
	}

	const char *pName = GetTtyName();

	if (2 == m_nNode)
	{
		m_nFd = open(GetTtyName(), O_RDWR | O_NOCTTY | O_NONBLOCK);
	}
	else
	{
		m_nFd = open(GetTtyName(), O_RDWR | O_NOCTTY | O_SYNC);
	}
	printf("my add: dev=%s \n", pName);
	if (-1 == m_nFd)
	{
		COM_ERROR("Open serial port %s error %d!\n", pName, errno);
		perror("open tty");
	}
	else
	{
		COM_INFO("Open serial port %s ok!\n", pName);
	}

	return m_nFd;
}

//##ModelId=49A36B8B007D
int CSerialDev::Close()
{
	if (-1 != m_nFd)
	{
		COM_INFO("close serial port %s!\n", GetTtyName());
		close(m_nFd);
		m_nFd = -1;
	}
	// TODO: Add your specialized code here.
	return (int) 0;
}

int CSerialDev::GetFd()
{
	return m_nFd;
}
#define RS485SEND_DATA 1
#define RS485RECEIVE_DATA 0
#define BYTE_PER_SEND 64
//##ModelId=49A36BBB00DA
int CSerialDev::SendData(void *pBuf, int nLength)
{
	if (!m_bInitialized)
	{
		return -1;
	}

	if (-1 == m_nFd)
	{
		return -1;
	}
	ZWSys_Set485WriteOrRead(RS485SEND_DATA);

	//还剩下多少数据没发送
	int nLeftLen = nLength;

	int nLen = 0;

	while (nLen < nLength)
	{
		//COM_INFO("nLen = %d, nLength = %d\n", nLen, nLength);
		int nWriting = 0;

		//将要发送的长度
		nWriting = (nLeftLen > BYTE_PER_SEND) ? BYTE_PER_SEND : nLeftLen;
		//剩余的长度
		nLeftLen = nLeftLen - nWriting;

//#define DPRINT_TIME 
#ifdef DPRINT_TIME
		struct timeval tv1;

		struct timeval tv2;

		gettimeofday(&tv1, NULL);
#endif
		int nWrited = write(m_nFd, (unsigned char *) pBuf + nLen, nWriting);	//实际写入的长度

#ifdef DPRINT_TIME
		gettimeofday(&tv2, NULL);
		printf("Send Data Use time %ld ms\n", (tv2.tv_sec - tv1.tv_sec) * 1000 + (tv2.tv_usec - tv1.tv_usec) / 1000);
#endif
		//rsleep(50000);
		//写入失败
		if (nWrited < nWriting)
		{
			tcflush(m_nFd, TCOFLUSH);
			break;
		}
		//总共发送的长度
		nLen += nWrited;
	}

	//COM_DBG("nLen = %d\n", nLen);
	//实际发送的数据与要求发送的数据长度不同
	if (nLen != nLength)
	{
		COM_INFO("Total Length = %d, Write Length = %d\n", nLength, nLen);
	}

	return nLen;
}

//##ModelId=49A36BD0003E
int CSerialDev::RecvData(void *pBuf, int nLength)
{
	if (!m_bInitialized)
	{
		return -1;
	}
	ZWSys_Set485WriteOrRead(RS485RECEIVE_DATA);

	int nRead = 0;

	int fs_sel;

	fd_set fs_read;

	struct timeval tv_timeout;

	if (-1 == m_nFd)
	{
		COM_ERROR("m_nFd is NULL!\n");
		return -1;
	}

	//循环读取，一次最多读取给定缓冲区大小的数据
	while (nRead < nLength)
	{
		FD_CLR(m_nFd, &fs_read);
		FD_ZERO(&fs_read);
		FD_SET(m_nFd, &fs_read);
		tv_timeout.tv_sec = 0;	//TIMEOUT_SEC(nLength, m_dwBaudrate);
		tv_timeout.tv_usec = 10000;	//TIMEOUT_USEC;

		fs_sel = select(m_nFd + 1, &fs_read, NULL, NULL, &tv_timeout);
		if ((fs_sel > 0) && FD_ISSET(m_nFd, &fs_read))
			//if(fs_sel > 0)
		{

			//COM_INFO("Before COM Read\n");
			int nReadLen = read(m_nFd, (unsigned char *) pBuf + nRead, nLength - nRead);

			//没读到数据
			//COM_INFO("After COM Read, nReadLen = %d\n", nReadLen);
			if (nReadLen <= 0)
			{
				break;
			}
			//总共读到多少数据
			nRead += nReadLen;
			//rsleep(100000);
			//}
			//return (nReadLen);
		}
		else
		{
			//没等到数据
			return (int) nRead;
		}
	}

	// TODO: Add your specialized code here.
	return (int) nRead;
}

//##ModelId=49A36C030157
int CSerialDev::SetBaudrate(unsigned long dwBaudrate)
{
	if (-1 == m_nFd)
	{
		return -1;
	}
	if (m_dwBaudrate == dwBaudrate)
	{
		return 0;
	}

	struct termios termios_com;

	int dwSpeed = B57600;

	int nDataBits = 8;

	int nStopBits = 1;

	int nParity = 0;

	int nFlowControl = 0;

	bzero(&termios_com, sizeof (termios_com));
	cfmakeraw(&termios_com);

	/*------------设置端口属性----------------*/
	m_dwBaudrate = dwBaudrate;
	dwSpeed = ConvBaudrate(m_dwBaudrate);
	cfsetispeed(&termios_com, dwSpeed);	//填入串口输入端波特率
	cfsetospeed(&termios_com, dwSpeed);	//填入串口输出端波特率
	termios_com.c_cflag |= CLOCAL;	//控制模式, 保证程序不会成为端口的占有者
	termios_com.c_cflag |= CREAD;	//控制模式, 使能端口读取输入的数据

	// 控制模式, flow control
	nFlowControl = 0;
	switch (nFlowControl)
	{
		case 0:
			termios_com.c_cflag &= ~CRTSCTS;	//无
			break;
		case 1:
			termios_com.c_cflag |= CRTSCTS;	//硬件控制
			break;
		case 2:
			termios_com.c_iflag |= IXON | IXOFF | IXANY;	//Xon/Xoff
			break;
	}

	//控制模式, 数据位
	termios_com.c_cflag &= ~CSIZE;	//控制模式, 屏蔽字符大小位
	nDataBits = 8;				//数据位8
	switch (nDataBits)
	{
		case 5:
			termios_com.c_cflag |= CS5;
			break;
		case 6:
			termios_com.c_cflag |= CS6;
			break;
		case 7:
			termios_com.c_cflag |= CS7;
			break;
		default:
			termios_com.c_cflag |= CS8;
			break;
	}

	//控制模式 nParity check
	nParity = 0;				//校验位
	switch (nParity)
	{
		case 0:
			termios_com.c_cflag &= ~PARENB;	//无
			break;
		case 1:
			termios_com.c_cflag |= PARENB;	//奇数校验
			termios_com.c_cflag &= ~PARODD;
			break;
		case 2:
			termios_com.c_cflag |= PARENB;	//偶数校验
			termios_com.c_cflag |= PARODD;
			break;
	}

	//控制模式, stop bits
	nStopBits = 1;
	switch (nStopBits)
	{
		case 2:
			termios_com.c_cflag |= CSTOPB;	//2停止位
			break;
		default:
			termios_com.c_cflag &= ~CSTOPB;	//1停止位
	}

	//其他数据为默认值
	termios_com.c_oflag &= ~OPOST;	//输出模式, 原始数据输出

	termios_com.c_cc[VMIN] = 1;	//控制字符, 所要读取字符的最小数量
	termios_com.c_cc[VTIME] = 1;	//控制字符, 读取第一个字符的等待时间,   unit: (1/10)second

	tcflush(m_nFd, TCIFLUSH);	//溢出的数据可以接收,但不读
	tcsetattr(m_nFd, TCSANOW, &termios_com);	//设置新属性, TCSANOW: 所由改变立即生效
	// TODO: Add your specialized code here.
	return (int) 0;
}

int CSerialDev::GetWriteLock()
{
	return pthread_mutex_lock(&m_mtxComWrite);
}

int CSerialDev::ReleaseWriteLock()
{
	return pthread_mutex_unlock(&m_mtxComWrite);
}

int CSerialDev::GetReadLock()
{
	return pthread_mutex_lock(&m_mtxComRead);
}

int CSerialDev::ReleaseReadLock()
{
	return pthread_mutex_unlock(&m_mtxComRead);
}

int CSerialDev::Init()
{
	int i = 0;

	int j = 0;

	for (i = 0; i < (int) MAX_RS_TYPE; i++)
	{
		for (j = 0; j < MAX_RS_CHANNEL; j++)
		{
			m_pDev[i][j] = NULL;
		}
	}

	//可以用串口了
	m_bInitialized = true;

	return 0;
}

int CSerialDev::UnInit()
{
	int i = 0;

	int j = 0;

	m_bInitialized = false;
	for (i = 0; i < (int) MAX_RS_TYPE; i++)
	{
		for (j = 0; j < MAX_RS_CHANNEL; j++)
		{
			if (NULL != m_pDev[i][j])
			{
				m_pDev[i][j]->GetWriteLock();
				m_pDev[i][j]->ReleaseWriteLock();
			}
		}
	}

	return 0;
}

int CSerialDev::ConvBaudrate(unsigned long dwBaudrate)
{
	switch (dwBaudrate)
	{
		case 1200:
			return B1200;
			break;
		case 2400:
			return B2400;
			break;
		case 4800:
			return B4800;
			break;
		case 9600:
			return B9600;
			break;
		case 19200:
			return B19200;
			break;
		case 38400:
			return B38400;
			break;
		case 57600:
			return B57600;
			break;
		case 115200:
			return B115200;
			break;
		default:
			return B57600;
			break;
	}
}

int CSerialDev::GetTtyDevice(int nType, int nPort)
{
	int nNode = MAX_RS_CHANNEL;

	DeviceDescription DevDescription;

#ifdef SATA
	//int nBoardID = MP4Sys_GetBoardID();
	int nBoardID = 0;
	ZWSys_GetBoardInfo(&nBoardID);
#else
	//int nBoardID = MP4Sys_GetBoardInfo();
	int nBoardID = 0;
	ZWSys_GetBoardInfo(&nBoardID);
#endif

	EDVRS_DDT_GetDeviceDescriptionByID(&DevDescription, nBoardID);

	COM_DBG("Type %d Devs: %d\n", nType, GetDevNum(&DevDescription, nType));

	if (nType == TTY232)
	{
		if (DevDescription.bytRS232Channels[nPort] & 0x80)
		{
			nNode = (DevDescription.bytRS232Channels[nPort] & 0x7f);
		}
	}
	else if (nType == TTY422)
	{
		if (DevDescription.bytRS422Channels[nPort] & 0x80)
		{
			nNode = (DevDescription.bytRS422Channels[nPort] & 0x7f);
		}
	}
	else if (nType == TTY485)
	{
		if (DevDescription.bytRS485Channels[nPort] & 0x80)
		{
			nNode = (DevDescription.bytRS485Channels[nPort] & 0x7f);
		}
	}
	if((0x2036 == nBoardID)||(0x2035 == nBoardID)||(0x2037 == nBoardID)||(0x2047 == nBoardID))
	{
		if(nPort)
			nNode = 0;
	}

	printf("type%d port%d node%d\n", nType, nPort, nNode);
	return nNode;
}

inline const char *CSerialDev::GetTtyName()
{
	const char *pNodeName = NULL;
	int BoardInfo = 0;
	ZWSys_GetBoardInfo(&BoardInfo);	

	switch (m_nNode)
	{
		case 0:
			pNodeName = TTY_HIDEV "0";
			break;
		case 1:
			pNodeName = TTY_HIDEV "1";
			break;
		case 2:
			pNodeName = TTY_HIDEV "2";
			break;
		case 3:
			pNodeName = TTY_HIDEV "3";
			break;
	}	

	return (pNodeName);
}

int CSerialDev::GetDevNum(LPDeviceDescription lpDevDescription, int nType)
{
	int i = 0;

	int nNum = 0;

	switch (nType)
	{
		case TTY232:
			for (i = 0; i < MAX_RS_CHANNEL; i++)
			{
				COM_DBG("lpDevDescription->bytRS232Channels[%d] = 0x%x\n", i, lpDevDescription->bytRS232Channels[i]);
				if ((lpDevDescription->bytRS232Channels[i] & 0x80) != 0)
				{
					nNum++;
				}
				else
				{
					break;
				}
			}
			break;
		case TTY422:
			for (i = 0; i < MAX_RS_CHANNEL; i++)
			{
				if (lpDevDescription->bytRS422Channels[i] & 0x80)
				{
					nNum++;
				}
				else
				{
					break;
				}
			}
			break;
		case TTY485:
			for (i = 0; i < MAX_RS_CHANNEL; i++)
			{
				if (lpDevDescription->bytRS485Channels[i] & 0x80)
				{
					nNum++;
				}
				else
				{
					break;
				}
			}
			break;
	}
	return nNum;
}
