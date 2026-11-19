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

#define	PTZ_AUX_FLAG	(PTZ_FLAG_AUXPANTITLT | PTZ_FLAG_AUXPRESET | PTZ_FLAG_AUXCRUISE | PTZ_FLAG_AUXAUTO)

#define  PTZ_TIMER_FLAG	(0)
#define	TIMER_PERIOD		0	//ms


#define	BAUDRATE	(BD4800)

#define	PRESET_COUNT	256
#define	CRUISE_COUNT	1		//巡航数最多为1
#define	TRACK_COUNT		0
#define AUTO_COUNT      1		//线扫数为0

#define PROTOCOL_VERSION "0.1"

const char s_szVersion[] = P_NAME " " PROTOCOL_VERSION "." BASEVERSION " BUILD " __DATE__ " " __TIME__;

static CbSendData IFSendData;

static CbRecvData IFRecvData;

static CbStartCruise IFStartCruise;

static CbStopCruise IFStopCruise;

static void ParityByte(unsigned char Buffer[], int nLength);

//云台普通控制
int PtzNormalCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, unsigned long dwSpeed)
{
	ASSERT_SENDFN;
	unsigned char Buffer[6];

	Buffer[0] = 0x02;
	Buffer[1] = dwAddress;
	Buffer[2] = 0x01;
	const int fast = 0xFF;

	const int slow = 0;

	dwSpeed = SPEED(dwSpeed, fast, slow);

	switch (dwPtzCode)
	{
		case TTY_PTZ_UP:
			Buffer[3] = 0x04;
			Buffer[4] = 0;
			ParityByte(Buffer, 6);
			IFSendData(nChannel, Buffer, 6);
			Buffer[2] = 0x0E;
			Buffer[3] = 0;
			Buffer[4] = dwSpeed;
			break;
		case TTY_PTZ_DOWN:
			Buffer[3] = 0x08;
			Buffer[4] = 0x00;
			ParityByte(Buffer, 6);
			IFSendData(nChannel, Buffer, 6);
			Buffer[2] = 0x0E;
			Buffer[3] = 0;
			Buffer[4] = dwSpeed;
			break;
		case TTY_PTZ_LEFT:
			Buffer[3] = 0x02;
			Buffer[4] = 0x00;
			ParityByte(Buffer, 6);
			IFSendData(nChannel, Buffer, 6);
			Buffer[2] = 0x0E;
			Buffer[3] = dwSpeed;
			Buffer[4] = 0;
			break;
		case TTY_PTZ_RIGHT:
			Buffer[3] = 0x01;
			Buffer[4] = 0x00;
			ParityByte(Buffer, 6);
			IFSendData(nChannel, Buffer, 6);
			Buffer[2] = 0x0E;
			Buffer[3] = dwSpeed;
			Buffer[4] = 0;
			break;
		case TTY_PTZ_UP_STOP:
		case TTY_PTZ_DOWN_STOP:
		case TTY_PTZ_LEFT_STOP:
		case TTY_PTZ_RIGHT_STOP:
		case TTY_PTZ_LEFTUP_STOP:
		case TTY_PTZ_LEFTDOWN_STOP:
		case TTY_PTZ_RIGHTUP_STOP:
		case TTY_PTZ_RIGHTDOWN_STOP:
		case TTY_PTZ_STOP:
			Buffer[3] = 0x00;
			Buffer[4] = 0x00;
			ParityByte(Buffer, 6);
			IFSendData(nChannel, Buffer, 6);
			Buffer[2] = 0x0E;
			break;
		case TTY_PTZ_LEFTUP:
			Buffer[3] = 0x04 | 0x02;
			Buffer[4] = 0;
			ParityByte(Buffer, 6);
			IFSendData(nChannel, Buffer, 6);
			Buffer[2] = 0x0E;
			Buffer[3] = dwSpeed;
			Buffer[4] = dwSpeed;
			break;
		case TTY_PTZ_RIGHTUP:
			Buffer[3] = 0x04 | 0x01;
			Buffer[4] = 0;
			ParityByte(Buffer, 6);
			IFSendData(nChannel, Buffer, 6);
			Buffer[2] = 0x0E;
			Buffer[3] = dwSpeed;
			Buffer[4] = dwSpeed;
			break;
		case TTY_PTZ_LEFTDOWN:
			Buffer[3] = 0x08 | 0x02;
			Buffer[4] = 0;
			ParityByte(Buffer, 6);
			IFSendData(nChannel, Buffer, 6);
			Buffer[2] = 0x0E;
			Buffer[3] = dwSpeed;
			Buffer[4] = dwSpeed;
			break;
		case TTY_PTZ_RIGHTDOWN:
			Buffer[3] = 0x08 | 0x01;
			Buffer[4] = 0;
			ParityByte(Buffer, 6);
			IFSendData(nChannel, Buffer, 6);
			Buffer[2] = 0x0E;
			Buffer[3] = dwSpeed;
			Buffer[4] = dwSpeed;
			break;
		case TTY_PTZ_ZOOMIN:	//变倍小
			Buffer[3] = 0x10;
			Buffer[4] = 0x00;
			break;
		case TTY_PTZ_ZOOMOUT:	//变倍大
			Buffer[3] = 0x20;
			Buffer[4] = 0;
			break;
		case TTY_PTZ_ZOOM_STOP:
			Buffer[3] = 0;
			Buffer[4] = 0x00;
			break;
		case TTY_PTZ_IRIS_SMALL:	//光圈小
			Buffer[3] = 0x00;
			Buffer[4] = 0x02;
			break;
		case TTY_PTZ_IRIS_LARGE:	//光圈大
			Buffer[3] = 0x00;
			Buffer[4] = 0x01;
			break;
		case TTY_PTZ_IRIS_STOP:
			Buffer[3] = 0x00;
			Buffer[4] = 0x00;
			break;
		case TTY_PTZ_FOCUS_NEAR:	//聚焦
			Buffer[3] = 0x40;
			Buffer[4] = 0;
			break;
		case TTY_PTZ_FOCUS_FAR:
			Buffer[3] = 0x80;
			Buffer[4] = 0;
			break;
		case TTY_PTZ_FOCUS_STOP:
			Buffer[3] = 0x00;
			Buffer[4] = 0;
			break;
		default:
			return DLL_ERROR;
	}
	ParityByte(Buffer, 6);
	IFSendData(nChannel, Buffer, 6);

	return DLL_OUTPUT_OK;
}

int PtzAuxCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, int bOn)
{
	switch (dwPtzCode)
	{
		case TTY_AUX_HEATER:	//
		case TTY_AUX_COOLER:
		case TTY_AUX_POWER:
		case TTY_AUX_WIPER:
		case TTY_AUX_LIGHT:
			return DLL_INPUT_OK;
			break;
		default:
			return DLL_ERROR;
	}

	return DLL_OUTPUT_OK;
}

int PtzAutoCtrl(unsigned int nChannel,
				unsigned long dwAddress,
				unsigned long dwPtzCode,
				unsigned long dwPresetIndex1,
				unsigned long dwPresetIndex2, unsigned long dwCruiseIndex, unsigned long dwSpeed, unsigned long dwDwellTime, int bScan)
{
	int nCmdLen;

	int nSum;

	unsigned char Buffer[6];

	Buffer[0] = 0x02;
	Buffer[1] = dwAddress;
	if (bScan)
	{
		switch (dwPtzCode)
		{
			case TTY_PTZ_AUTO:
				Buffer[2] = 0x0F;
				Buffer[3] = 0x03;
				Buffer[4] = 0x00;
				ParityByte(Buffer, 6);
				IFSendData(nChannel, Buffer, 6);
				return DLL_OUTPUT_OK;
				break;
			case TTY_PTZ_DWELL:
				Buffer[2] = 0x0B;
				Buffer[3] = dwDwellTime;
				ParityByte(Buffer, 5);
				IFSendData(nChannel, Buffer, 5);
				return DLL_OUTPUT_OK;
				break;
			case TTY_PTZ_SETROUTE:
				Buffer[2] = 0x03;
				Buffer[3] = 0;
				ParityByte(Buffer, 5);
				IFSendData(nChannel, Buffer, 5);
				return DLL_OUTPUT_OK;
				break;
			case TTY_PTZ_UPDOWN_SPEED:
				Buffer[2] = 0x0E;
				Buffer[3] = 0x00;
				Buffer[4] = dwSpeed;
				ParityByte(Buffer, 6);
				IFSendData(nChannel, Buffer, 6);
				return DLL_OUTPUT_OK;
				break;
			case TTY_PTZ_LEFTRIGHT_SPEED:
				Buffer[2] = 0x0E;
				Buffer[3] = dwSpeed;
				Buffer[4] = 0;
				ParityByte(Buffer, 6);
				IFSendData(nChannel, Buffer, 6);
				return DLL_OUTPUT_OK;
				break;
			default:
				return DLL_ERROR;
		}
	}
	else
	{
		Buffer[0] = 0x02;
		Buffer[1] = dwAddress;
		Buffer[2] = 0x01;
		Buffer[3] = 0x00;
		Buffer[4] = 0x00;
		ParityByte(Buffer, 6);
		IFSendData(nChannel, Buffer, 6);
		return DLL_OUTPUT_OK;
	}

	return DLL_ERROR;
}

int PtzPresetCtrl(unsigned int nChannel,
				  unsigned long dwAddress,
				  unsigned long dwPtzCode, unsigned long dwPresetIndex, unsigned long dwCruiseIndex, unsigned long dwSpeed, unsigned long dwDwellTime)
{
	ASSERT_SENDFN;
	unsigned char Buffer[6];

	Buffer[0] = 0x02;
	Buffer[1] = dwAddress;
	Buffer[4] = 0;

	switch (dwPtzCode)
	{
		case TTY_PTZ_PRESETSET:	//
			Buffer[2] = 0x05;
			Buffer[3] = dwPresetIndex + 1;
			ParityByte(Buffer, 5);
			IFSendData(nChannel, Buffer, 5);
			break;
		case TTY_PTZ_PRESETCALL:	//
			Buffer[2] = 0x02;
			Buffer[3] = dwPresetIndex + 1;
			ParityByte(Buffer, 5);
			IFSendData(nChannel, Buffer, 5);
			break;
		case TTY_PTZ_ROUTEADDPOINT:
			Buffer[2] = 0x06;
			Buffer[3] = dwPresetIndex + 1;
			ParityByte(Buffer, 5);
			IFSendData(nChannel, Buffer, 5);
			break;
		case TTY_PTZ_PRESETCLRALL:
		case TTY_PTZ_ROUTEREMOVE:
			Buffer[2] = 0x08;
			Buffer[3] = 0x0;
			ParityByte(Buffer, 5);
			IFSendData(nChannel, Buffer, 5);
			break;
		case TTY_PTZ_AUTOSET:
			Buffer[2] = 0x0F;
			Buffer[3] = 0x02;
			Buffer[4] = 0x01;
			ParityByte(Buffer, 6);
			IFSendData(nChannel, Buffer, 6);
			break;
		case TTY_PTZ_AUTOEND:
			Buffer[2] = 0x0F;
			Buffer[3] = 0x02;
			Buffer[4] = 0x02;
			ParityByte(Buffer, 6);
			IFSendData(nChannel, Buffer, 6);
			break;
		case TTY_PTZ_TRACKSET:
		case TTY_PTZ_TRACKEND:
			return DLL_INPUT_OK;
			break;
		default:
			return DLL_ERROR;
	}

	return DLL_OUTPUT_OK;
}

int PtzMenuCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode)
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

int SetCallback(CbSendData FnSendData, CbRecvData FnRecvData, CbStartCruise FnStartCruise, CbStopCruise FnStopCruise)
{
	IFSendData = FnSendData;
	IFRecvData = FnRecvData;
	return 0;
}

int HasFunction(int nPtzFlag)
{
	return (int) ((((PTZ_FLAGS | PTZ_AUX_FLAG) & nPtzFlag) != 0) ? 1 : 0);
}

void GetTechnicalData(int *PresetCount, int *CruiseCount, int *TrackCount, int *AutoCount, unsigned long *Baudrate)
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

const char *GetVersion()
{
	return s_szVersion;
}

void ParityByte(unsigned char Buffer[], int nLength)
{
	int i = 0;

	Buffer[nLength - 1] = 0;
	for (i = 0; i < nLength - 1; i++)
	{
		Buffer[nLength - 1] += Buffer[i];
	}
	Buffer[nLength - 1] = Buffer[nLength - 1] & 0xFF;
}
