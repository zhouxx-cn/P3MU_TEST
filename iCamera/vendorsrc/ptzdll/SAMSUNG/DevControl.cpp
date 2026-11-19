//!\file DevControl.cpp
//!\brief SAMSUNG协议实现文件
//! 参考文档 samsung protocol.doc
//!\author yuk@higheasyrd.com
//!\date 2009-9-11 中国安防科技有限公司武汉分公司

#include <stdio.h>
#include "../DevControl.h"

//device type
#define	DEV_TYPE	DRIVER_PTZ

#define	PTZ_FLAGS	(PTZ_FLAG_PANTITLT|PTZ_FLAG_ZOOM|PTZ_FLAG_FOCUS|PTZ_FLAG_IRIS)

#define	PTZ_AUX_FLAG	(PTZ_FLAG_AUXPRESET|PTZ_FLAG_AUXTRACK|PTZ_FLAG_AUXAUTO|PTZ_FLAG_AUXSOFTWARECRUISE)

#define  PTZ_TIMER_FLAG	(0)
#define	TIMER_PERIOD		0	//ms

#define	BAUDRATE	(BD4800|BD9600|BD19200|BD38400)

#define	PRESET_COUNT	256
#define	CRUISE_COUNT	16		//巡航数最多为1
#define	TRACK_COUNT		3
#define AUTO_COUNT		1		//线扫数为0

#define PROTOCOL_VERSION "0.1"

const char s_szVersion[] = P_NAME " " PROTOCOL_VERSION "." BASEVERSION " BUILD " __DATE__ " " __TIME__;

static CbSendData IFSendData;

static CbRecvData IFRecvData;

static CbStartCruise IFStartCruise;

static CbStopCruise IFStopCruise;

void ParityByte(unsigned char Buffer[9]);

//云台普通控制
int PtzNormalCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, unsigned long dwSpeed)
{
	ASSERT_SENDFN;
	unsigned char Buffer[9];

	Buffer[0] = 0xA0;
	Buffer[1] = dwAddress;
	Buffer[2] = dwAddress;

	Buffer[3] = 0x01;
	const int fast = 64;

	const int slow = 0;

	dwSpeed = SPEED(dwSpeed, fast, slow);

	switch (dwPtzCode)
	{
		case TTY_PTZ_UP:
			Buffer[4] = 0;
			Buffer[5] = 0x04;
			Buffer[6] = 0;
			Buffer[7] = dwSpeed;
			ParityByte(Buffer);
			IFSendData(nChannel, Buffer, 9);
			Buffer[5] = 0x04 | 0x08;
			break;
		case TTY_PTZ_UP_AUTO:
			Buffer[4] = 0;
			Buffer[5] = 0x0C;
			Buffer[6] = 0;
			Buffer[7] = dwSpeed;
			break;
		case TTY_PTZ_DOWN:
			Buffer[4] = 0;
			Buffer[5] = 0x08;
			Buffer[6] = 0;
			Buffer[7] = dwSpeed;
			ParityByte(Buffer);
			IFSendData(nChannel, Buffer, 9);
			Buffer[5] = 0x04 | 0x08;
			break;
		case TTY_PTZ_DOWN_AUTO:
			Buffer[4] = 0;
			Buffer[5] = 0x0C;
			Buffer[6] = 0;
			Buffer[7] = dwSpeed;
			break;
		case TTY_PTZ_LEFT:
			Buffer[4] = 0;
			Buffer[5] = 0x01;
			Buffer[6] = dwSpeed;
			Buffer[7] = 0;
			ParityByte(Buffer);
			IFSendData(nChannel, Buffer, 9);
			Buffer[5] = 0x01 | 0x02;
			break;
		case TTY_PTZ_LEFT_AUTO:
			Buffer[4] = 0;
			Buffer[5] = 0x03;
			Buffer[6] = dwSpeed;
			Buffer[7] = 0;
			break;
		case TTY_PTZ_RIGHT:
			Buffer[4] = 0;
			Buffer[5] = 0x02;
			Buffer[6] = dwSpeed;
			Buffer[7] = 0;
			ParityByte(Buffer);
			IFSendData(nChannel, Buffer, 9);
			Buffer[5] = 0x01 | 0x02;
			break;
		case TTY_PTZ_RIGHT_AUTO:
			Buffer[4] = 0;
			Buffer[5] = 0x03;
			Buffer[6] = dwSpeed;
			Buffer[7] = 0;
			break;
		case TTY_PTZ_LEFTUP:
		case TTY_PTZ_RIGHTUP:
		case TTY_PTZ_LEFTDOWN:
		case TTY_PTZ_RIGHTDOWN:
			return DLL_INPUT_OK;
		case TTY_PTZ_ZOOMIN:	//变倍小
			Buffer[4] = 0x40;
			Buffer[5] = 0;
			Buffer[6] = 0;
			Buffer[7] = 0;
			ParityByte(Buffer);
			IFSendData(nChannel, Buffer, 9);
			Buffer[4] = 0x20 | 0x40;
			break;
		case TTY_PTZ_ZOOMOUT:	//变倍大
			Buffer[4] = 0x20;
			Buffer[5] = 0;
			Buffer[6] = 0;
			Buffer[7] = 0;
			ParityByte(Buffer);
			IFSendData(nChannel, Buffer, 9);
			Buffer[4] = 0x20 | 0x40;
			break;
		case TTY_PTZ_ZOOM_AUTO:
			Buffer[4] = 0x60;
			Buffer[5] = 0;
			Buffer[6] = 0;
			Buffer[7] = 0;
			break;

		case TTY_PTZ_IRIS_SMALL:	//光圈小
			Buffer[4] = 0x10;
			Buffer[5] = 0;
			Buffer[6] = 0;
			Buffer[7] = 0;
			break;
		case TTY_PTZ_IRIS_LARGE:	//光圈大
			Buffer[4] = 0x08;
			Buffer[5] = 0;
			Buffer[6] = 0;
			Buffer[7] = 0;
			break;
		case TTY_PTZ_IRIS_AUTO:
			return DLL_INPUT_OK;
		case TTY_PTZ_FOCUS_NEAR:	//聚焦
			Buffer[4] = 0x02;
			Buffer[5] = 0;
			Buffer[6] = 0;
			Buffer[7] = 0;
			ParityByte(Buffer);
			IFSendData(nChannel, Buffer, 9);
			Buffer[4] = 0x02 | 0x01;
			break;
		case TTY_PTZ_FOCUS_FAR:
			Buffer[4] = 0x01;
			Buffer[5] = 0;
			Buffer[6] = 0;
			Buffer[7] = 0;
			ParityByte(Buffer);
			IFSendData(nChannel, Buffer, 9);
			Buffer[4] = 0x02 | 0x01;
			break;
		case TTY_PTZ_FOCUS_AUTO:
			Buffer[4] = 0x03;
			Buffer[5] = 0;
			Buffer[6] = 0;
			Buffer[7] = 0;
			break;
		case TTY_PTZ_ZOOM_STOP:
		case TTY_PTZ_IRIS_STOP:
		case TTY_PTZ_FOCUS_STOP:
		case TTY_PTZ_UP_STOP:
		case TTY_PTZ_DOWN_STOP:
		case TTY_PTZ_LEFT_STOP:
		case TTY_PTZ_RIGHT_STOP:
		case TTY_PTZ_STOP:
			Buffer[4] = 0;
			Buffer[5] = 0;
			Buffer[6] = 0;
			Buffer[7] = 0;
			break;
		default:
			return DLL_ERROR;
	}
	ParityByte(Buffer);
	IFSendData(nChannel, Buffer, 9);

	return DLL_OUTPUT_OK;
}

int PtzAuxCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, int bOn)
{
	switch (dwPtzCode)
	{
		case TTY_AUX_HEATER:
		case TTY_AUX_COOLER:
		case TTY_AUX_POWER:
		case TTY_AUX_WIPER:
		case TTY_AUX_LIGHT:
			return DLL_INPUT_OK;
			break;
		default:
			return DLL_ERROR;
	}

	return DLL_ERROR;
}

int PtzAutoCtrl(unsigned int nChannel,
				unsigned long dwAddress,
				unsigned long dwPtzCode,
				unsigned long dwPresetIndex1,
				unsigned long dwPresetIndex2, unsigned long dwCruiseIndex, unsigned long dwSpeed, unsigned long dwDwellTime, int bScan)
{
	ASSERT_SENDFN;
	unsigned char Buffer[9];

	Buffer[0] = 0xA0;
	Buffer[1] = dwAddress;
	Buffer[2] = dwAddress;
	Buffer[3] = 0x03;
	Buffer[4] = 0x13;
	Buffer[6] = 0xFF;
	Buffer[7] = 0xFF;
	const int fast = 64;

	const int slow = 0;

	dwSpeed = SPEED(dwSpeed, fast, slow);

	switch (dwPtzCode)
	{
		case TTY_PTZ_AUTO:
			Buffer[4] = 0x13;
			Buffer[5] = bScan;
			break;
		case TTY_PTZ_TRACKCALL:
			if (bScan)
			{
				Buffer[4] = 0;
				Buffer[5] = 0x02;
				Buffer[6] = dwCruiseIndex + 1;
				Buffer[7] = 0xFF;
			}
			else
			{
				PtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_RIGHT, 0);
				PtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_RIGHT_STOP, 0);
				return DLL_OUTPUT_OK;
			}
			break;
		case TTY_PTZ_SETROUTE:
			ASSERT_SOFTCRUISEFN;
			if (dwCruiseIndex >= CRUISE_COUNT)
			{
				return DLL_ERROR;
			}
			if (bScan)
			{
				int ret = IFStartCruise(nChannel, (unsigned int) dwCruiseIndex);

				if (0 == ret)
				{
					return DLL_OUTPUT_OK;
				}
				else
				{
					return DLL_INPUT_OK;
				}
			}
			else
			{
				IFStopCruise(nChannel);
				return DLL_OUTPUT_OK;
			}
			return DLL_OUTPUT_OK;
			break;
		default:
			return DLL_ERROR;
	}
	ParityByte(Buffer);
	IFSendData(nChannel, Buffer, 9);

	return DLL_OUTPUT_OK;
}

int PtzPresetCtrl(unsigned int nChannel,
				  unsigned long dwAddress,
				  unsigned long dwPtzCode, unsigned long dwPresetIndex, unsigned long dwCruiseIndex, unsigned long dwSpeed, unsigned long dwDwellTime)
{
	ASSERT_SENDFN;
	unsigned char Buffer[9];

	Buffer[0] = 0xA0;
	Buffer[1] = dwAddress;
	Buffer[2] = dwAddress;
	Buffer[3] = 0x03;
	const int fast = 64;

	const int slow = 0;

	dwSpeed = SPEED(dwSpeed, fast, slow);

	switch (dwPtzCode)
	{
		case TTY_PTZ_PRESETSET:
			Buffer[4] = 0x50;
			Buffer[5] = dwPresetIndex;
			Buffer[6] = 0xFF;
			Buffer[7] = 0xFF;
			break;
		case TTY_PTZ_PRESETCALL:
			Buffer[4] = 0x19;
			Buffer[5] = dwPresetIndex;
			Buffer[6] = 0xFF;
			Buffer[7] = 0xFF;
			break;
		case TTY_PTZ_PRESETCLRALL:
			Buffer[4] = 0x51;
			Buffer[5] = dwPresetIndex;
			Buffer[6] = 0xFF;
			Buffer[7] = 0xFF;
			break;
		case TTY_PTZ_TRACKSET:
			Buffer[4] = 0x1B;
			Buffer[5] = dwCruiseIndex + 1;
			Buffer[6] = 0x01;
			Buffer[7] = 0xFF;
			break;
		case TTY_PTZ_TRACKEND:
			Buffer[4] = 0x1B;
			Buffer[5] = dwCruiseIndex + 1;
			Buffer[6] = 0;
			Buffer[7] = 0xFF;
			break;
		case TTY_PTZ_ROUTESET:
		case TTY_PTZ_ROUTEEND:
		case TTY_PTZ_ROUTEADDPOINT:
			return DLL_OUTPUT_OK;
		case TTY_PTZ_AUTOSET:
		case TTY_PTZ_AUTOEND:
			return DLL_INPUT_OK;
			break;
		default:
			return DLL_ERROR;
	}
	ParityByte(Buffer);
	IFSendData(nChannel, Buffer, 9);

	return DLL_OUTPUT_OK;
}


int PtzMenuCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode)
{
	//本协议不支持菜单操作
	switch (dwPtzCode)
	{
		case TTY_PTZ_AUTO:
		case TTY_PTZ_MANUAL:
		case TTY_PTZ_SETROUTE:
		case TTY_PTZ_TRACKCALL:
		case TTY_PTZ_SPEED:
		case TTY_PTZ_UPDOWN_SPEED:
		case TTY_PTZ_LEFTRIGHT_SPEED:
		case TTY_PTZ_DWELL:
			return DLL_INPUT_OK;
			break;
		default:
			break;
	}

	return DLL_ERROR;
}

int SetCallback(CbSendData FnSendData, CbRecvData FnRecvData, CbStartCruise FnStartCruise, CbStopCruise FnStopCruise)
{
	//printf("SetCallback\n");
	IFSendData = FnSendData;
	IFRecvData = FnRecvData;
	IFStartCruise = FnStartCruise;
	IFStopCruise = FnStopCruise;

	return 0;
}

int HasFunction(int nPtzFlag)
{
	//printf("nPtzFlag=%d\n", nPtzFlag);
	return (int) ((((PTZ_FLAGS | PTZ_AUX_FLAG) & nPtzFlag) != 0) ? 1 : 0);
}

void GetTechnicalData(int *PresetCount, int *CruiseCount, int *TrackCount, int *AutoCount, unsigned long *Baudrate)
{
	//printf("GetTechnicalData\n");
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

const char *GetVersion()
{
	return s_szVersion;
}

void ParityByte(unsigned char Buffer[9])
{
	Buffer[8] = 0x00FF & (0xFFFF - (Buffer[1] + Buffer[2] + Buffer[3] + Buffer[4] + Buffer[5] + Buffer[6] + Buffer[7]));
}
