
/*!
 *\file		CSerialDev.h
 *\brief	串口设备管理类定义
 *\author	yuk@higheasyrd.com
 *\date		2009/05/05

功能：
-#	串口设备的获取,收发数据

说明：
-#	实现串口设备的控制,线程安全
 */

#ifndef __CSERIALDEV_H_
#define __CSERIALDEV_H_

#include <pthread.h>
#include "ddt_so.h"
#include "tagSerialPort.h"

//!串口设备基础类
//##ModelId=49A36B4E0251
class CSerialDev
{
  public:
	//!\brief 获取串口设备指针
	//!\param nType串口类型 参见 SERIAL_TYPE
	//!\param nPort 串口控制设备号
	//!\return  NULL表示获取失败, 其他表示CSerialDev指针
	//##ModelId=49B8633001E4
	static CSerialDev *GetInstance(int nType, int nPort = 0);

	static void DestroyInstance(CSerialDev * pInstance);

	//!\brief 打开串口
	//!\return 0表示打开成功 -1表示失败
	//##ModelId=49A36B870157
	int Open();

	//!\brief 关闭串口
	//!\return 0表示关闭成功，-1表示失败
	//##ModelId=49A36B8B007D
	int Close();

	int GetFd();

	//!\brief 发送数据
	//!\param pBuf 数据缓冲区
	//!\param nLength缓冲区长度
	//!\return 已发送的数据长度
	//##ModelId=49A36BBB00DA
	int SendData(void *pBuf, int nLength);

	//!\brief 接受串口数据
	//!\param pBuf 接受数据的缓冲区
	//!\param nLength 缓冲区长度
	//!\return 接受到的数据长度
	//##ModelId=49A36BD0003E
	int RecvData(void *pBuf, int nLength);

	//!\brief 设置串口的波特率
	//!\param dwBaudrate 波特率
	//!\return 0表示设置成功，-1表示设置失败
	//##ModelId=49A36C030157
	int SetBaudrate(unsigned long dwBaudrate);

	//!\brief 获取写控制权
	int GetWriteLock();

	//!\brief 释放写控制权
	int ReleaseWriteLock();

	//!\brief 获取读控制权
	int GetReadLock();

	//!\brief 释放读控制权
	int ReleaseReadLock();

	//!\brief 初始化串口
	static int Init();

	static int UnInit();
  private:
	  CSerialDev();

	 ~CSerialDev();

	static int GetTtyDevice(int nType, int nPort = 0);

	const char *GetTtyName();

	//!\brief 波特率转化转换函数
	//!\param dwBaudrate 波特率整数值
	//!\return 返回speed B1200~B115200
	int ConvBaudrate(unsigned long dwBaudrate);

	static int GetDevNum(LPDeviceDescription lpDevDescription, int nType);

  private:
	//!\brief 串口设备实例
	static CSerialDev *m_pDev[MAX_RS_TYPE][MAX_RS_CHANNEL];

	//!\brief 波特率
	unsigned long m_dwBaudrate;

	//!\brief ttyS节点号
	int m_nNode;

	//!\brief 文件描述符
	int m_nFd;

	//!\brief 写互斥锁
	pthread_mutex_t m_mtxComWrite;

	//!\brief 读互斥锁
	pthread_mutex_t m_mtxComRead;

	static int m_bInitialized;
};

#endif							/* __CSERIALDEV_H_ */
