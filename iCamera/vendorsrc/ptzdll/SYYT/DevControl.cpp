//!\file DevControl.cpp
//!\brief SYYT协议实现文件
//! 参考文档 SYYT报警协议.doc
//!\author yuk@higheasyrd.com
//!\date 2009-9-9 中国安防科技有限公司武汉分公司

#include <stdio.h>
#include "../DevControl.h"

//device type
#define	DEV_TYPE	DRIVER_PTZ

#define	PTZ_FLAGS	(PTZ_FLAG_PANTITLT|PTZ_FLAG_ZOOM|PTZ_FLAG_FOCUS|PTZ_FLAG_IRIS)

#define	PTZ_AUX_FLAG	(PTZ_FLAG_AUXLIGHT|PTZ_FLAG_AUXWIPER)

#define  PTZ_TIMER_FLAG	(0)
#define	TIMER_PERIOD		0	//ms

#define	BAUDRATE	(BD2400|BD4800|BD9600|BD19200)

#define	PRESET_COUNT	0
#define	CRUISE_COUNT	0
#define	TRACK_COUNT		0
#define AUTO_COUNT		0

#define PROTOCOL_VERSION "0.1"

const char s_szVersion[] = P_NAME " " PROTOCOL_VERSION "." BASEVERSION " BUILD " __DATE__ " " __TIME__;

static CbSendData IFSendData;

static CbRecvData IFRecvData;

static CbStartCruise IFStartCruise;

static CbStopCruise IFStopCruise;

//云台普通控制
int PtzNormalCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, unsigned long dwSpeed)
{
	ASSERT_SENDFN;
	unsigned char Buffer[16];

	Buffer[0] = 0xFF;
	Buffer[1] = 0xFF;
	Buffer[2] = 0xFF;
	Buffer[3] = 0xFF;

	Buffer[4] = 0;
	Buffer[5] = 0;
	Buffer[6] = 0x01;
	Buffer[7] = 0;
	Buffer[8] = 0;
	Buffer[9] = 0;

	switch (dwPtzCode)
	{
		case TTY_PTZ_UP:
			Buffer[10] = dwAddress;
			Buffer[11] = 0x92;
			break;
		case TTY_PTZ_DOWN:
			Buffer[10] = dwAddress;
			Buffer[11] = 0x94;
			break;
		case TTY_PTZ_LEFT:
			Buffer[10] = dwAddress;
			Buffer[11] = 0x96;
			break;
		case TTY_PTZ_RIGHT:
			Buffer[10] = dwAddress;
			Buffer[11] = 0x98;
			break;
		case TTY_PTZ_DOWN_AUTO:
		case TTY_PTZ_LEFT_AUTO:
		case TTY_PTZ_RIGHT_AUTO:
		case TTY_PTZ_UP_AUTO:
			return DLL_INPUT_OK;

		case TTY_PTZ_LEFTUP:
		case TTY_PTZ_RIGHTUP:
		case TTY_PTZ_LEFTDOWN:
		case TTY_PTZ_RIGHTDOWN:
			return DLL_INPUT_OK;
		case TTY_PTZ_ZOOMIN:	//变倍小
			Buffer[10] = dwAddress;
			Buffer[11] = 0x90;
			break;
		case TTY_PTZ_ZOOMOUT:	//变倍大
			Buffer[10] = dwAddress;
			Buffer[11] = 0x8E;
			break;
		case TTY_PTZ_IRIS_SMALL:	//光圈小
			Buffer[10] = dwAddress;
			Buffer[11] = 0x88;
			break;
		case TTY_PTZ_IRIS_LARGE:	//光圈大
			Buffer[10] = dwAddress;
			Buffer[11] = 0x86;
			break;
		case TTY_PTZ_FOCUS_NEAR:	//聚焦
			Buffer[10] = dwAddress;
			Buffer[11] = 0x8C;
			break;
		case TTY_PTZ_FOCUS_FAR:
			Buffer[10] = dwAddress;
			Buffer[11] = 0x8A;
			break;
		case TTY_PTZ_FOCUS_AUTO:
		case TTY_PTZ_ZOOM_AUTO:
		case TTY_PTZ_IRIS_AUTO:
			return DLL_INPUT_OK;

		case TTY_PTZ_ZOOM_STOP:
			Buffer[10] = dwAddress;
			Buffer[11] = 0x8F;
			break;
		case TTY_PTZ_IRIS_STOP:
			Buffer[10] = dwAddress;
			Buffer[11] = 0x87;
			break;
		case TTY_PTZ_FOCUS_STOP:
			Buffer[10] = dwAddress;
			Buffer[11] = 0x8B;
			break;
		case TTY_PTZ_UP_STOP:
		case TTY_PTZ_DOWN_STOP:
			Buffer[10] = dwAddress;
			Buffer[11] = 0x93;
			break;
		case TTY_PTZ_LEFT_STOP:
		case TTY_PTZ_RIGHT_STOP:
			Buffer[10] = dwAddress;
			Buffer[11] = 0x97;
			break;
		default:
			return DLL_ERROR;
	}
	Buffer[12] = (Buffer[4] ^ Buffer[5] ^ Buffer[6] ^ Buffer[7] ^ Buffer[8] ^ Buffer[9] ^ Buffer[10] ^ Buffer[11]);
	Buffer[13] = 0xFE;
	Buffer[14] = 0xEE;
	Buffer[15] = 0xEE;
	IFSendData(nChannel, Buffer, 16);

	return DLL_OUTPUT_OK;
}

int PtzAuxCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, int bOn)
{
	ASSERT_SENDFN;
	unsigned char Buffer[16];

	Buffer[0] = 0xFF;
	Buffer[1] = 0xFF;
	Buffer[2] = 0xFF;
	Buffer[3] = 0xFF;

	Buffer[4] = 0;
	Buffer[5] = 0;
	Buffer[6] = 0x01;
	Buffer[7] = 0;
	Buffer[8] = 0;
	Buffer[9] = 0;
	switch (dwPtzCode)
	{
		case TTY_AUX_LIGHT:
			Buffer[10] = dwAddress;
			if (bOn)
			{
				Buffer[11] = 0xA2;
			}
			else
			{
				Buffer[11] = 0xA3;
			}
			break;
		case TTY_AUX_WIPER:
			Buffer[10] = dwAddress;
			if (bOn)
			{
				Buffer[11] = 0xA0;
			}
			else
			{
				Buffer[11] = 0xA1;
			}
			break;
		case TTY_AUX_POWER:
		case TTY_AUX_HEATER:
		case TTY_AUX_COOLER:
			return DLL_INPUT_OK;
			break;
		default:
			return DLL_ERROR;
	}
	Buffer[12] = (Buffer[4] ^ Buffer[5] ^ Buffer[6] ^ Buffer[16] ^ Buffer[8] ^ Buffer[9] ^ Buffer[10] ^ Buffer[11]);
	Buffer[13] = 0xFE;
	Buffer[14] = 0xEE;
	Buffer[15] = 0xEE;
	IFSendData(nChannel, Buffer, 16);

	return DLL_OUTPUT_OK;
}

int PtzAutoCtrl(unsigned int nChannel,
				unsigned long dwAddress,
				unsigned long dwPtzCode,
				unsigned long dwPresetIndex1,
				unsigned long dwPresetIndex2, unsigned long dwCruiseIndex, unsigned long dwSpeed, unsigned long dwDwellTime, int bScan)
{
	switch (dwPtzCode)
	{
		case TTY_PTZ_SETROUTE:
		case TTY_PTZ_AUTO:
		case TTY_PTZ_TRACKCALL:
			return DLL_INPUT_OK;
			break;
		default:
			break;
	}

	return DLL_ERROR;			//本协议不支持自动操作
}

int PtzPresetCtrl(unsigned int nChannel,
				  unsigned long dwAddress,
				  unsigned long dwPtzCode, unsigned long dwPresetIndex, unsigned long dwCruiseIndex, unsigned long dwSpeed, unsigned long dwDwellTime)
{
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


int PtzMenuCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode)
{
	//本协议不支持菜单操作
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
	IFStartCruise = FnStartCruise;
	IFStopCruise = FnStopCruise;
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
