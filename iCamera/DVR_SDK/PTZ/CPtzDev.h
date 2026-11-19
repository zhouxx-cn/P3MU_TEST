// Copyright (C) 1991 - 1999 Rational Software Corporation

#ifndef __CPTZDEV_H_
#define __CPTZDEV_H_


#include "COM/CSerialDev.h"

#define MAX_CHANNEL_CNT SYSTEM_SUPPORT_CHANNEL 
//!巡航点结构体
//##ModelId=49B48F3D032C
typedef struct tagCruisePoint
{
  public:
	//!预置点号
	//##ModelId=49B48F9301A5
	int dwPresetId;

	//!预置点速度
	//##ModelId=49B48F9F008C
	unsigned long dwSpeed;

	//!停滞时间
	//##ModelId=49B48FA600BB
	unsigned long dwDwellTime;

	  tagCruisePoint():dwPresetId(0), dwSpeed(0), dwDwellTime(0)
	{
	};
} CruisePoint;

//!\brief 巡航线路结构体
//##ModelId=49B48FD4000F
typedef struct tagCruise
{
  public:
	//!预置点数组
	//##ModelId=49BB2CD10119
	CruisePoint aCruisePoint[MAX_CRUISE_POINT];

	//!预置点数
	//##ModelId=49BB423703C8
	int nPointCount;

	  tagCruise()
	{
		nPointCount = 0;
	};
} Cruise;

typedef struct CruiseTemporary
{
	Cruise tCruise;
	int nCruiseIndex;

	  CruiseTemporary()
	{
		nCruiseIndex = -1;
	};
} CruiseTemporary;

//!\brief 管理云台解码器参数
//##ModelId=49A36E710167
class CPtzDev
{
  public:
	CPtzDev(int nChannel);

	 ~CPtzDev();

	//!\brief 加载参数
	int Init(int nIndex);

	//!\brief 设置解码器参数
	//!\param strProtocol 协议名缓冲区
	//!\param nLength 缓冲区长度
	//!\return 0表示成功 -1表示成功
	//##ModelId=49B720CA0399
	int GetProtocol(char *strProtocol, int nLength);

	//!\brief 设置云台参数
	//!\param strProtocol 协议名
	//!\return 0表示成功，其他值表示失败或者参数不合法
	//##ModelId=49A3A4060232
	int SetProtocol(char *strProtocol);

	//!\brief 获取云台是否使用该协议
	//!\param szProtocol 协议名
	//!\return true表示使用 false表示不试用
	bool UseProtocol(char *sszProtocol);

	//!\brief 获取解码器地址
	//!\return -1表示获取失败 其他表示解码器地址
	//##ModelId=49B7202C01B5
	unsigned long GetAddress();

	//!\brief 设置解码器地址
	//!\param dwAddress 解码器地址
	//!\return -1设置失败 0表示设置成功
	//##ModelId=49B72011004E
	int SetAddress(unsigned long dwAddress);

	//!\brief 获取解码器波特率
	//!\return -1表示获取失败 其他表示解码器波特率
	//##ModelId=49B72032031C
	unsigned long GetBaudRate();

	//!\brief 设置解码器波特率
	//!\param dwBaudrate 解码器波特率
	//!\return -1设置失败 0表示设置成功
	//##ModelId=49B720180399
	int SetBaudRate(unsigned long dwBaudRate);

	int GetSerialDev(int *nSerialType, int *nPort = NULL);

	int SetSerialDev(int nSerialType, int nPort = 0);

	int Open();

	//!\brief 发送数据
	//!\param pBuf 数据缓冲区
	//!\param nLength缓冲区长度
	//!\return 已发送的数据长度
	//##ModelId=49A3B6AF035B
	int SendData(void *pBuf, int nLength);

	//!\brief 接受解码器数据
	//!\param pBuf 接受数据的缓冲区
	//!\param nLength 缓冲区长度
	//!\return 接受到的数据长度
	//##ModelId=49A3C326034B
	int RecvData(void *pBuf, int nLength);

	Cruise *GetCruise(unsigned long dwCruiseIndex);

	CruiseTemporary *GetTempCruise();

	void ClearTempCruise();

	int SetCruise(unsigned long dwCruiseIndex);

	int AddCruisePoint(unsigned long CruiseIndex, unsigned long PresetPointId, unsigned long nSpeed, unsigned long nDwellTime);

	int SaveCruise(unsigned long CruiseIndex);

	int ClearCruise(unsigned long dwCruiseIndex);

	//!\brief 当前要调用的预置点的滞留时间（软巡航）
	int GetDwellTime();

	//!\brief 转到软巡航下一个要调用的预置点
	int NextPoint();

	//!\brief 获取要调用的预置点号（软巡航）
	int GetPresetId();

	//!\brief 获取要调用的预置点的速度（软巡航）
	unsigned long GetSpeed();

	//!\brief 设置要软巡航的巡航线路
	int SetCurCruise(int nCruise);

	//!\brief 获取当前正在巡航的线路（软巡航）
	int GetCurCruise();

#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
	//!\brief 设置水平电机位置
	int SetHorizontalAngle(int nHorizontalAngle);

	//!\brief 设置水平电机位置
	int SetVerticalAngle(int nVerticalAngle);

	//!\brief 获取水平电机是否反转
	//!\brief 设置水平电机位置
	int SetHorizontalReverse(int nReverse,int Chn);

	int GetHorizontalReverse(int Chn);

	//!\brief 设置水平电机位置
	int SetVerticalReverse(int nReverse,int Chn);
	//!\brief 获取垂直电机是否反转

	int GetVerticalReverse(int Chn);
	//将预置点位置作为新的位置记录点
	//保存至配置文件
	int SavePresetAsNewAngle(unsigned long dwPresetIndex);
	
	int SetHorizontalStep(int nHorizontalStep,int Chn);

	int SetVerticalStep(int nVerticalStep,int Chn);

    // 用户定制云台垂直最大步长
    int SetUserVMaxStep(int nUserVMaxStep);

	//设置云台垂直最大速度
	int SetMaxSpeedV(int nMaxSpeedV,int Chn);
	//设置云台水平最大速度
	int SetMaxSpeedH(int nMaxSpeedH,int Chn);

#ifdef __AGINGTEST__
	//!\brief 获取最大位置
	int GetMaxStep (int *nHMaxStop, int *nVMaxStep,int Chn);
#endif
    //!\brief 进入隐私遮挡位置
    int PrivacyMaskPreset(int AwakeMode);
#endif

  private:
	//##ModelId=49BB2CDC01B5
	  Cruise m_tCruise[MAX_CRUISE_COUNT];

	CruiseTemporary m_tTempCruise;

	//!\brief 波特率
	unsigned long m_dwBaudRate;

	//!\brief 解码器地址
	unsigned long m_dwAddress;

	//!\brief 协议名称
	char m_szProtocol[256];

	//!\brief 关联的通道号
	int m_nChannel;

	//!\brief 串口类型
	int m_nSerialType;

	//!\brief 串口端口号
	int m_nSerialPort;

	int m_nCurCruise;

	int m_nCurPoint;

#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
	// 水平位置
	int m_nHorizontalAngle;
	// 垂直位置
	int m_nVerticalAngle;
	// 水平最大步长
	int m_nHorizontalStep[MAX_CHANNEL_CNT];
	// 垂直最大步长
	int m_nVerticalStep[MAX_CHANNEL_CNT];
    // 用户定制云台垂直最大步长
    int m_nUserVMaxStep;
	// 水平马达速度
	int m_nMotoSpeedH[MAX_CHANNEL_CNT];
	// 垂直马达速度
	int m_nMotoSpeedV[MAX_CHANNEL_CNT];
	// 水平马达反转
	int m_nHorizontalReverse[MAX_CHANNEL_CNT];
	// 垂直马达反转
	int m_nVerticalReverse[MAX_CHANNEL_CNT];

    //当前水平位置
	int m_nCurHorizontal;
	//当前垂直位置
	int m_nCurVertical;
#endif
};

#endif							/* __CPTZDEV_H_ */
