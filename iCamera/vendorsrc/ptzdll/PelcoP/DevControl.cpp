#include <stdio.h>
#include "../DevControl.h"

#ifdef __cplusplus
extern "C" {
#endif

//device type
#define	DEV_TYPE	DRIVER_PTZ

//!\brief Pelcop协议基本功能
//! 上下左右,变倍,焦距,光圈
#define	PTZ_FLAGS	(PTZ_FLAG_PANTITLT | PTZ_FLAG_ZOOM | PTZ_FLAG_FOCUS | PTZ_FLAG_IRIS)

//!\brief Pelcop协议扩展功能
//! 雨刷,轨迹,预置点,巡航
#define	PTZ_AUX_FLAG	(PTZ_FLAG_AUXWIPER | PTZ_FLAG_AUXTRACK | PTZ_FLAG_AUXPRESET | PTZ_FLAG_AUXCRUISE /*| PTZ_FLAG_AUXAUTO*/)

#define  PTZ_TIMER_FLAG	(0)
#define	TIMER_PERIOD		0	//ms

#define	BAUDRATE	(BD9600|BD4800|BD2400)

#define	PRESET_COUNT	256
#define	TRACK_COUNT		1		//轨迹最多为3
#define	CRUISE_COUNT	1		//巡航数最多为1
#define AUTO_COUNT      0		//线扫数为0

#define PROTOCOL_VERSION "0.1"

const char s_szVersion[] = P_NAME " " PROTOCOL_VERSION "." BASEVERSION " BUILD " __DATE__ " " __TIME__;

static CbSendData IFSendData;

static CbRecvData IFRecvData;

static CbStartCruise IFStartCruise;

static CbStopCruise IFStopCruise;	//##ModelId=49A36CC30148

//云台普通控制
#if  defined (__MUSL__)
int PelcopPtzNormalCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, unsigned long dwSpeed)
#else
int PtzNormalCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, unsigned long dwSpeed)
#endif
{
	ASSERT_SENDFN;
	char Buffer[8];

	Buffer[0] = 0xA0;
	Buffer[1] = dwAddress;

	Buffer[4] = 0x1F;
	Buffer[5] = 0x1F;
	Buffer[6] = 0xAF;

	const int fast = 63;

	const int slow = 0;

	dwSpeed = SPEED(dwSpeed, fast, slow);

	switch (dwPtzCode)
	{
		case TTY_PTZ_UP:
			Buffer[2] = 0;
			Buffer[3] = 0x08;
			Buffer[5] = dwSpeed;
			break;
		case TTY_PTZ_DOWN:
			Buffer[2] = 0;
			Buffer[3] = 0x10;
			Buffer[5] = dwSpeed;
			break;
		case TTY_PTZ_LEFT:
			Buffer[2] = 0;
			Buffer[3] = 0x04;
			Buffer[4] = dwSpeed;
			break;
		case TTY_PTZ_RIGHT:
			Buffer[2] = 0;
			Buffer[3] = 0x02;
			Buffer[4] = dwSpeed;
			break;
		case TTY_PTZ_UP_AUTO:
		case TTY_PTZ_DOWN_AUTO:
		case TTY_PTZ_LEFT_AUTO:
		case TTY_PTZ_RIGHT_AUTO:
		case TTY_PTZ_LEFTUP:
		case TTY_PTZ_RIGHTUP:
		case TTY_PTZ_LEFTDOWN:
		case TTY_PTZ_RIGHTDOWN:
			return DLL_INPUT_OK;
		case TTY_PTZ_ZOOMIN:
			Buffer[2] = 0;
			Buffer[3] = 0x20; //  2016年3月9日 11:03:11 使景物放大,即将景物推近
			Buffer[4] = 0;
			Buffer[5] = 0;
			break;
		case TTY_PTZ_ZOOMOUT: //  2016年3月9日 11:03:11 使景物缩小,即将景物拉远
			Buffer[2] = 0;
			Buffer[3] = 0x40;
			Buffer[4] = 0;
			Buffer[5] = 0;
			break;
		case TTY_PTZ_IRIS_SMALL:
			Buffer[2] = 0x08;
			Buffer[3] = 0;
			Buffer[4] = 0;
			Buffer[5] = 0;
			break;
		case TTY_PTZ_IRIS_LARGE:	//光圈大
			Buffer[2] = 0x04;
			Buffer[3] = 0;
			Buffer[4] = 0;
			Buffer[5] = 0;
			break;
		case TTY_PTZ_FOCUS_NEAR:	//聚焦
			Buffer[2] = 0x02;
			Buffer[3] = 0;
			Buffer[4] = 0;
			Buffer[5] = 0;
			break;
		case TTY_PTZ_FOCUS_FAR:
			Buffer[2] = 0x01;
			Buffer[3] = 0;
			Buffer[4] = 0;
			Buffer[5] = 0;
			break;
		case TTY_PTZ_ZOOM_STOP:
		case TTY_PTZ_IRIS_STOP:
		case TTY_PTZ_FOCUS_STOP:
		case TTY_PTZ_UP_STOP:
		case TTY_PTZ_DOWN_STOP:
		case TTY_PTZ_LEFT_STOP:
		case TTY_PTZ_RIGHT_STOP:
		case TTY_PTZ_STOP:
			Buffer[2] = 0;
			Buffer[3] = 0;
			Buffer[4] = 0;
			Buffer[5] = 0;
			break;
		default:
			return DLL_ERROR;
	}

	Buffer[7] = (Buffer[0] ^ Buffer[1] ^ Buffer[2] ^ Buffer[3] ^ Buffer[4] ^ Buffer[5] ^ Buffer[6]);
	IFSendData(nChannel, Buffer, 8);

	return DLL_OUTPUT_OK;
}

#if  defined (__MUSL__)
int PelcopPtzAuxCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, int bOn)
#else
int PtzAuxCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, int bOn)
#endif
{
	ASSERT_SENDFN;
	char Buffer[8];

	Buffer[0] = 0xA0;
	Buffer[1] = dwAddress;

	Buffer[4] = 0;
	Buffer[5] = 0x01;
	Buffer[6] = 0xAF;

	switch (dwPtzCode)
	{
		case TTY_AUX_LIGHT:
		case TTY_AUX_HEATER:
		case TTY_AUX_COOLER:
		case TTY_AUX_POWER:
			return DLL_ERROR;

		case TTY_AUX_WIPER:
			if (bOn)
			{
				Buffer[2] = 0;
				Buffer[3] = 0x09;
			}
			else
			{
				Buffer[2] = 0;
				Buffer[3] = 0x0B;
			}
			break;
		default:
			return DLL_ERROR;
	}

	Buffer[7] = (Buffer[0] ^ Buffer[1] ^ Buffer[2] ^ Buffer[3] ^ Buffer[4] ^ Buffer[5] ^ Buffer[6]);
	IFSendData(nChannel, Buffer, 8);

	return DLL_OUTPUT_OK;
}
#if  defined (__MUSL__)
int PelcopPtzAutoCtrl(unsigned int nChannel,
				unsigned long dwAddress,
				unsigned long dwPtzCode,
				unsigned long dwPresetIndex1,
				unsigned long dwPresetIndex2, unsigned long dwCruiseIndex, unsigned long dwSpeed, unsigned long dwDwellTime, int bScan)

#else
int PtzAutoCtrl(unsigned int nChannel,
				unsigned long dwAddress,
				unsigned long dwPtzCode,
				unsigned long dwPresetIndex1,
				unsigned long dwPresetIndex2, unsigned long dwCruiseIndex, unsigned long dwSpeed, unsigned long dwDwellTime, int bScan)
#endif
{
	ASSERT_SENDFN;
	char Buffer[8];

	Buffer[0] = 0xA0;
	Buffer[1] = dwAddress;
	Buffer[2] = 0;

	Buffer[4] = 0;
	Buffer[6] = 0xAF;

	switch (dwPtzCode)
	{
		case TTY_PTZ_SETROUTE:
			if (bScan)
			{
				Buffer[2] = 0x85;
				Buffer[3] = 0x81;
				Buffer[5] = 0;
			}
			else
			{
				Buffer[2] = 0;
				Buffer[3] = 0;
				Buffer[4] = 0;
				Buffer[5] = 0;
			}
			break;
		case TTY_PTZ_TRACKCALL:
			{
				if (dwCruiseIndex >= TRACK_COUNT)
				{
					return DLL_ERROR;
				}

				if (bScan)
				{
					Buffer[2] = 0;
					Buffer[3] = 0x23;
					Buffer[5] = 0;
				}
				else
				{
#if  defined (__MUSL__)
					PelcopPtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_RIGHT, 1);
					PelcopPtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_RIGHT_STOP, 0);
#else
					PtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_RIGHT, 1);
					PtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_RIGHT_STOP, 0);
#endif
					return DLL_OUTPUT_OK;
				}
				break;
			}
		case TTY_PTZ_AUTO:
			if (dwCruiseIndex >= AUTO_COUNT)
			{
				return DLL_ERROR;
			}

			if (bScan)
			{
				Buffer[2] = 0x88;
				Buffer[3] = 0x81;
				Buffer[4] = 1;
				Buffer[5] = SPEED(dwSpeed, 255, 0);
				IFSendData(nChannel, Buffer, 8);
				Buffer[2] = 0x88;
				Buffer[3] = 0x81;
				Buffer[4] = 3;
				Buffer[5] = 0;
			}
			else
			{
#if  defined (__MUSL__)
				PelcopPtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_RIGHT, 1);
				PelcopPtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_RIGHT_STOP, 0);
#else
				PtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_RIGHT, 1);
				PtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_RIGHT_STOP, 0);
#endif
				return DLL_OUTPUT_OK;
			}
			break;
		case TTY_PTZ_DWELL:
			Buffer[2] = 0x86;
			Buffer[3] = 0x81;
			Buffer[5] = dwDwellTime;
			break;
		case TTY_PTZ_SPEED:
		default:
			return DLL_ERROR;
	}

	Buffer[7] = (Buffer[0] ^ Buffer[1] ^ Buffer[2] ^ Buffer[3] ^ Buffer[4] ^ Buffer[5] ^ Buffer[6]);
	IFSendData(nChannel, Buffer, 8);

	return DLL_OUTPUT_OK;;
}
#if  defined (__MUSL__)
int PelcopPtzPresetCtrl(unsigned int nChannel,
				  unsigned long dwAddress,
				  unsigned long dwPtzCode, unsigned long dwPresetIndex, unsigned long dwCruiseIndex, unsigned long dwSpeed, unsigned long dwDwellTime)
#else
int PtzPresetCtrl(unsigned int nChannel,
				  unsigned long dwAddress,
				  unsigned long dwPtzCode, unsigned long dwPresetIndex, unsigned long dwCruiseIndex, unsigned long dwSpeed, unsigned long dwDwellTime)
#endif
{
	ASSERT_SENDFN;
	char Buffer[8];

	Buffer[0] = 0xA0;
	Buffer[1] = dwAddress;

	Buffer[2] = 0;
	Buffer[4] = 0;
	Buffer[6] = 0xAF;

	switch (dwPtzCode)
	{
		case TTY_PTZ_PRESETSET:
			Buffer[3] = 0x03;
			Buffer[5] = dwPresetIndex + 1;
			break;
		case TTY_PTZ_PRESETCALL:
			Buffer[3] = 0x07;
			Buffer[5] = dwPresetIndex + 1;
			break;
		case TTY_PTZ_PRESETMOVE:
			Buffer[3] = 0x05;
			Buffer[5] = dwPresetIndex + 1;
			break;
		case TTY_PTZ_TRACKSET:
			Buffer[3] = 0x1F;
			Buffer[5] = 0;
			break;
		case TTY_PTZ_TRACKEND:
			Buffer[3] = 0x21;
			Buffer[5] = 0;
			break;
		case TTY_PTZ_ROUTESET:
			Buffer[2] = 0x81;
			Buffer[3] = 0x81;
			Buffer[5] = 0;
			break;
		case TTY_PTZ_ROUTEADDPOINT:
			Buffer[2] = 0x82;
			Buffer[3] = 0x81;
			Buffer[5] = dwPresetIndex + 1;
			break;
		case TTY_PTZ_ROUTEREMOVEPOINT:
			Buffer[2] = 0x83;
			Buffer[3] = 0x81;
			Buffer[5] = dwPresetIndex + 1;
			break;
		case TTY_PTZ_ROUTEREMOVE:
			Buffer[2] = 0x84;
			Buffer[3] = 0x81;
			Buffer[5] = 0;
			break;
		case TTY_PTZ_AUTOSET:
			Buffer[2] = 0x88;
			Buffer[3] = 0x81;
			Buffer[4] = 2;
			Buffer[5] = 1;
			break;
		case TTY_PTZ_AUTOEND:
			Buffer[2] = 0x88;
			Buffer[3] = 0x81;
			Buffer[4] = 2;
			Buffer[5] = 2;
			break;
		default:
			return DLL_ERROR;
	}

	Buffer[7] = (Buffer[0] ^ Buffer[1] ^ Buffer[2] ^ Buffer[3] ^ Buffer[4] ^ Buffer[5] ^ Buffer[6]);
	IFSendData(nChannel, Buffer, 8);

	return DLL_OUTPUT_OK;
}


#if  defined (__MUSL__)
int PelcopPtzMenuCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode)
#else
int PtzMenuCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode)
#endif
{
	//本协议不支持菜单操作
	switch (dwPtzCode)
	{
		case TTY_PTZ_MENU_ENTER:
		case TTY_PTZ_MENU_EXIT:
		case TTY_PTZ_MENU_UP:
		case TTY_PTZ_MENU_DOWN:
		case TTY_PTZ_MENU_LEFT:
		case TTY_PTZ_MENU_RIGHT:
		case TTY_PTZ_MENU_OK:
		case TTY_PTZ_MENU_CANEL:
			return DLL_INPUT_OK;
			break;
		default:
			break;
	}

	return DLL_ERROR;
}

#if  defined (__MUSL__)
int PelcopSetCallback(CbSendData FnSendData, CbRecvData FnRecvData, CbStartCruise FnStartCruise, CbStopCruise FnStopCruise)
#else
int SetCallback(CbSendData FnSendData, CbRecvData FnRecvData, CbStartCruise FnStartCruise, CbStopCruise FnStopCruise)
#endif
{
	IFSendData = FnSendData;
	IFRecvData = FnRecvData;
	IFStartCruise = FnStartCruise;
	IFStopCruise = FnStopCruise;

	return 0;
}

#if  defined (__MUSL__)
int PelcopHasFunction(int nPtzFlag)
#else
int HasFunction(int nPtzFlag)
#endif
{
	return (int) ((((PTZ_FLAGS | PTZ_AUX_FLAG) & nPtzFlag) != 0) ? 1 : 0);
}

#if  defined (__MUSL__)
void PelcopGetTechnicalData(int *PresetCount, int *CruiseCount, int *TrackCount, int *AutoCount, unsigned long *Baudrate)
#else
void GetTechnicalData(int *PresetCount, int *CruiseCount, int *TrackCount, int *AutoCount, unsigned long *Baudrate)
#endif
{
	if (PresetCount != NULL)
	{
		*PresetCount = PRESET_COUNT;
	}

	if (CruiseCount != NULL)
	{
		*CruiseCount = CRUISE_COUNT;
	}

	if (TrackCount != NULL)
	{
		*TrackCount = TRACK_COUNT;
	}

	if (AutoCount != NULL)
	{
		*AutoCount = AUTO_COUNT;
	}

	if (Baudrate != NULL)
	{
		*Baudrate = BAUDRATE;
	}
}

#if  defined (__MUSL__)
const char *PelcopGetVersion()
#else
const char *GetVersion()
#endif
{
	return s_szVersion;
}

#ifdef __cplusplus
}
#endif
