
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

#ifndef __CTRANSPARENTMANAGER_H_
#define __CTRANSPARENTMANAGER_H_

#include "global_macro.h"
#include "PUB/HANDLE.h"
#include "PUB/DVR_LIST.h"
#include "newhie/HieServer.h"
#include "tagTransparent.h"
class CSerialDev;

#ifdef SATA

#define LONG long

#ifndef DWORD_MACRO
#define DWORD_MACRO
typedef unsigned long DWORD;
#endif

#ifndef BYTE_MACRO
#define BYTE_MACRO
typedef unsigned char BYTE;
#endif

#ifndef WORD_MACRO
#define WORD_MACRO
typedef unsigned short WORD;
#endif

#include "../../../modules/include/hieserver_api.h"
#endif

#if 0
//!透明传输参数信息
struct tagTransparentParaInfo
{
	//!主类型信息
	unsigned long dwMajorType;
	//!子类型信息
	unsigned long dwMinorType;
	//!参数信息
	Buffer cBuffer;
	//!保留
	unsigned long dwReserve[DEF_RESERVE_NUM];
};

typedef tagTransparentParaInfo TransparentParaInfo, *LPTransparentParaInfo;

#endif


#ifdef SATA

//!\brief 透明通道创建接口
//!\param hIndex 透明通道句柄
//!\param dwUserID  用户ID
//!\param iSerialType   串口类型
//!\param nPort 端口序号
//!\return 返回0表示成功, 否则表示错误码
typedef int (*IF_TransparentChannelCreate) (LONG lUserID, HANDLE hSerial, HY_DVR_SERIALPORT_TYPE iSerialType, int nPort);

//!\brief 透明通道销毁接口
//!\param hIndex 透明通道句柄
//!\return 返回0表示成功, 否则表示错误码
typedef int (*IF_TransparentChannelDestroy) (HANDLE hSerial);

//!\brief 透明通道数据读取接口
//!\param hIndex 透明通道句柄
//!\param pBuf 要写的数据
//!\param nLength 数据长度
//!\return  0表示成功 否则表示错误码
typedef int (*IF_TransparentChannelRead) (HANDLE hSerial, char *buf, int iLen);

typedef int (*IF_TransparentChannelWrite) (HANDLE hIndex, char *pBuf, int iLen);
#elif 0

//!\brief 透明通道创建接口
//!\param hIndex 透明通道句柄
//!\param dwUserID  用户ID
//!\param pTransparentParaInfo  透明通道启动参数
//!\return 返回0表示成功, 否则表示错误码
typedef int (*IF_TransparentChannelCreate) (HANDLE * hIndex, unsigned long dwUserID, const LPTransparentParaInfo pTransparentParaInfo);

//!\brief 透明通道销毁接口
//!\param hIndex 透明通道句柄
//!\param pTransparentParaInfo 透明通道停止参数
//!\return 返回0表示成功, 否则表示错误码
typedef int (*IF_TransparentChannelDestroy) (HANDLE hIndex, const LPTransparentParaInfo pTransparentParaInfo);

//!\brief 透明通道数据读取接口
//!\param hIndex 透明通道句柄
//!\param pTransparentParaInfo 透明通道数据信息
//!\return  0表示成功 否则表示错误码
typedef int (*IF_TransparentChannelRead) (HANDLE hIndex, const LPTransparentParaInfo pTransparentParaInfo);

#endif

#if 0
typedef struct tagTransparentChanInterface
{
	//!透明通道创建接口
	IF_TransparentChannelCreate ifTransparentChannelStart;
	//!透明通道销毁接口
	IF_TransparentChannelDestroy ifTransparentChannelDestroy;
	//!透明通道数据读取接口
	IF_TransparentChannelRead ifTransparentChannelRead;
} TransparentChanInterface, *LPTransparentChanInterface;

int HieServer_TransparentChannelIf(const LPTransparentChanInterface pInterface);

#endif

typedef struct tagTransparentChannel TransparentChannel, *LPTransparentChannel;

//!\brief 透明通道模块，提供网络库所需的回调
//!1.创建透明通道，监听串口是否有数据，如有数据，调用int 
//HieServer_TransparentChannelWrite(HANDLE hIndex, const 
//LPTransparentParaInfo pTransparentParaInfo)发送数据到客户端
//!2.客户端数据回调，将数据写入串口
//!3.销毁透明通道回调，关闭串口
//##ModelId=49B5DB2D0242
class CTransparentManager
{
  public:
	static CTransparentManager *GetInstance();

	static void DestroyInstance(CTransparentManager * pInstance);

	const LPTransparentChanInterface GetNetInterface();

	int SetIFTransparentChannelWrite(IF_TransparentChannelWrite ifTransparentChannelWrite);

	int Start();

	int Stop();
  private:
	  CTransparentManager();

	 ~CTransparentManager();

#ifdef SATA

	static int ChannelCreateCallback(LONG lUserID, HANDLE hSerial, HY_DVR_SERIALPORT_TYPE nType, int nPort);
	static int ChannelDestroyCallback(HANDLE hSerial);
	static int ChannelReadCallback(HANDLE hSerial, char *pBuffer, int nLength);

#else

	//!\brief 收到客户端关闭透明通道命令时的回调函数
	//!\param hIndex 透明通道句柄
	//!\param pTransparentParaInfo 透明通道停止参数
	//!\return 返回0表示成功, 否则表示错误码
	//##ModelId=49B5DB6B0128
	static int ChannelDestroyCallback(HANDLE hIndex, const LPTransparentParaInfo pTransparentParaInfo);

	//!\brief 客户端要求打开透明通道时的回调函数
	//\param hIndex 透明通道句柄
	//\param dwUserID   用户ID
	//\param pTransparentParaInfo       透明通道启动参数
	//return 返回0表示成功, 否则表示错误码
	//##ModelId=49B5DB500261
	static int ChannelCreateCallback(HANDLE * hIndex, unsigned long dwUserID, const LPTransparentParaInfo pTransparentParaInfo);

	//!\brief 收到客户端数据时的透明通道回调函数
	//!\param hIndex 透明通道句柄
	//!\param pTransparentParaInfo 透明通道数据信息
	//!\return 返回0表示成功, 否则表示错误码
	//##ModelId=49B5DB6300CB
	static int ChannelReadCallback(HANDLE hIndex, const LPTransparentParaInfo pTransparentParaInfo);

#endif

	static void *ComRead(void *pArg);

	static LPTransparentChannel IsValidDev(CSerialDev * pDev);

	static LPTransparentChannel IsValidIndex(HANDLE hIndex);

	static int GenerateIndex();

	//static void *GetChannel(HANDLE hIndex);
  private:
	static CTransparentManager *m_pInstance;

	static dvr_list_head m_listChannel;

	static pthread_mutex_t m_mtxChannel;

	IF_TransparentChannelWrite m_ifTransparentChannelWrite;

	TransparentChanInterface m_tNetInterface;

};

#endif /*__CTRANSPARENTMANAGER_H_*/
