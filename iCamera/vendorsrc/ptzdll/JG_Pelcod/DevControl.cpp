//!\file DevControl.cpp
//!\brief JG_Pelcod协议实现文件
//! 参考文档 捷高智能恒速低速球D协议2007-8-27.doc
//!\author yuk@higheasyrd.com
//!\date 2009-9-9 中国安防科技有限公司武汉分公司

#include <stdio.h>
#include "../DevControl.h"

//device type
#define	DEV_TYPE	DRIVER_PTZ

//!\brief 云台支持的基本功能,上下左右,变倍,变焦,光圈
#define	PTZ_FLAGS	(PTZ_FLAG_PANTITLT|PTZ_FLAG_ZOOM|PTZ_FLAG_FOCUS|PTZ_FLAG_IRIS)

//!\brief 云台支持的扩展功能,电源,灯光,雨刷,轨迹,预置点,巡航
#define	PTZ_AUX_FLAG	(PTZ_FLAG_AUXPOWER|PTZ_FLAG_AUXLIGHT|PTZ_FLAG_AUXWIPER | PTZ_FLAG_AUXTRACK | PTZ_FLAG_AUXPRESET | PTZ_FLAG_AUXCRUISE)

#define  PTZ_TIMER_FLAG	(0)
#define	TIMER_PERIOD		0	//ms


#define	BAUDRATE	(BD4800)

#define	PRESET_COUNT	256		//支持的预置点数目
#define	TRACK_COUNT		1		//轨迹最多为3
#define	CRUISE_COUNT	1		//巡航数最多为1
#define AUTO_COUNT		0		//自动扫描数

#define PROTOCOL_VERSION "0.1"

const char s_szVersion[] = P_NAME " " PROTOCOL_VERSION "." BASEVERSION " BUILD " __DATE__ " " __TIME__;

static CbSendData IFSendData;

static CbRecvData IFRecvData;

static CbStartCruise IFStartCruise;

static CbStopCruise IFStopCruise;	//##ModelId=49A36CC30148

//云台普通控制
int PtzNormalCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, unsigned long dwSpeed)
{
	ASSERT_SENDFN;
	unsigned char Buffer[7];

	Buffer[0] = 0xFF;
	Buffer[1] = dwAddress;

	Buffer[4] = 0x1F;
	Buffer[5] = 0x1F;
	const int fast = 0x3F;

	const int slow = 0x00;

	dwSpeed = SPEED(dwSpeed, fast, slow);

	switch (dwPtzCode)
	{
		case TTY_PTZ_UP:
			Buffer[2] = 0;
			Buffer[3] = 1 << 3;
			Buffer[5] = dwSpeed;
			break;
		case TTY_PTZ_DOWN:
			Buffer[2] = 0;
			Buffer[3] = 1 << 4;
			Buffer[5] = dwSpeed;
			break;
		case TTY_PTZ_LEFT:
			Buffer[2] = 0;
			Buffer[3] = 1 << 2;
			Buffer[4] = dwSpeed;
			break;
		case TTY_PTZ_RIGHT:
			Buffer[2] = 0;
			Buffer[3] = 1 << 1;
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
			Buffer[3] = 0x40;
			Buffer[4] = 0;
			Buffer[5] = 0;
			break;
		case TTY_PTZ_ZOOMOUT:
			Buffer[2] = 0;
			Buffer[3] = 0x20;
			Buffer[4] = 0;
			Buffer[5] = 0;
			break;
		case TTY_PTZ_IRIS_SMALL:
			Buffer[2] = 0x04;
			Buffer[3] = 0;
			Buffer[4] = 0;
			Buffer[5] = 0;
			break;
		case TTY_PTZ_IRIS_LARGE:
			Buffer[2] = 0x02;
			Buffer[3] = 0;
			Buffer[4] = 0;
			Buffer[5] = 0;
			break;
		case TTY_PTZ_FOCUS_FAR:
			Buffer[2] = 0x00;
			Buffer[3] = 0x80;
			Buffer[4] = 0;
			Buffer[5] = 0;
			break;
		case TTY_PTZ_FOCUS_NEAR:
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
	Buffer[6] = (Buffer[1] + Buffer[2] + Buffer[3] + Buffer[4] + Buffer[5]) % 256;
	IFSendData(nChannel, Buffer, 7);

	return DLL_OUTPUT_OK;
}

int PtzAuxCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, int bOn)
{
	unsigned char Buffer[7];

	Buffer[0] = 0xFF;
	Buffer[1] = dwAddress;

	Buffer[4] = 0;
	Buffer[5] = 0;

	switch (dwPtzCode)
	{
		case TTY_AUX_HEATER:
		case TTY_AUX_COOLER:
			return DLL_ERROR;

		case TTY_AUX_WIPER:
			if (bOn)
			{
				Buffer[2] = 0;
				Buffer[3] = 0x09;
				Buffer[4] = 0;
				Buffer[5] = 1;
			}
			else
			{
				Buffer[2] = 0;
				Buffer[3] = 0x0B;
				Buffer[4] = 0;
				Buffer[5] = 1;
			}
			break;

		case TTY_AUX_LIGHT:
			if (bOn)
			{
				Buffer[2] = 0;
				Buffer[3] = 0x09;
				Buffer[4] = 0;
				Buffer[5] = 4;
			}
			else
			{
				Buffer[2] = 0;
				Buffer[3] = 0x0B;
				Buffer[4] = 0;
				Buffer[5] = 4;
			}
			break;
		case TTY_AUX_POWER:
			if (bOn)
			{
				Buffer[2] = 0;
				Buffer[3] = 0x09;
				Buffer[4] = 0;
				Buffer[5] = 2;
			}
			else
			{
				Buffer[2] = 0;
				Buffer[3] = 0x0B;
				Buffer[4] = 0;
				Buffer[5] = 2;
			}
			break;
		default:
			return DLL_ERROR;
	}
	Buffer[6] = (Buffer[1] + Buffer[2] + Buffer[3] + Buffer[4] + Buffer[5]) % 256;
	IFSendData(nChannel, Buffer, 7);

	return DLL_OUTPUT_OK;
}

int PtzAutoCtrl(unsigned int nChannel,
				unsigned long dwAddress,
				unsigned long dwPtzCode,
				unsigned long dwPresetIndex1,
				unsigned long dwPresetIndex2, unsigned long dwCruiseIndex, unsigned long dwSpeed, unsigned long dwDwellTime, int bScan)
{
	ASSERT_SENDFN;
	unsigned char Buffer[7];

	Buffer[0] = 0xFF;
	Buffer[1] = dwAddress;
	Buffer[2] = 0;
	Buffer[4] = 0;
	switch (dwPtzCode)
	{
		case TTY_PTZ_SETROUTE:
			if (dwCruiseIndex >= CRUISE_COUNT)
			{
				return DLL_ERROR;
			}
			if (bScan)
			{
				Buffer[2] = 0;
				Buffer[3] = 0x07;
				Buffer[4] = 0;
				Buffer[5] = 0x8e;
			}
			else
			{
				Buffer[2] = 0;
				Buffer[3] = 0x07;
				Buffer[4] = 0;
				Buffer[5] = 0x8a;
			}
			break;
		case TTY_PTZ_TRACKCALL:
			if (dwCruiseIndex >= TRACK_COUNT)
			{
				return DLL_ERROR;
			}

			if (bScan)
			{
				Buffer[3] = 0x23;
				Buffer[5] = 0;
			}
			else
			{
				PtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_RIGHT, 1);
				PtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_RIGHT_STOP, 0);
				return DLL_OUTPUT_OK;
			}
			break;
		default:
			return DLL_ERROR;
	}

	Buffer[6] = (Buffer[1] + Buffer[2] + Buffer[3] + Buffer[4] + Buffer[5]) % 256;
	IFSendData(nChannel, Buffer, 7);

	return DLL_OUTPUT_OK;
}

int PtzPresetCtrl(unsigned int nChannel,
				  unsigned long dwAddress,
				  unsigned long dwPtzCode, unsigned long dwPresetIndex, unsigned long dwCruiseIndex, unsigned long dwSpeed, unsigned long dwDwellTime)
{
	ASSERT_SENDFN;
	unsigned char Buffer[7];

	Buffer[0] = 0xFF;
	Buffer[1] = dwAddress;

	Buffer[2] = 0;
	Buffer[4] = 0;
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
			Buffer[2] = 0;
			Buffer[3] = 0x07;
			Buffer[4] = 0;
			Buffer[5] = 0x8c;
			break;
		case TTY_PTZ_ROUTEADDPOINT:
			Buffer[2] = 0;
			Buffer[3] = 0x07;
			Buffer[4] = 0;
			Buffer[5] = dwPresetIndex + 1;
			break;
		case TTY_PTZ_ROUTEEND:
			Buffer[2] = 0;
			Buffer[3] = 0x07;
			Buffer[4] = 0;
			Buffer[5] = 0x8d;
			break;

		case TTY_PTZ_ROUTEREMOVE:
		default:
			return DLL_ERROR;
	}

	Buffer[6] = (Buffer[1] + Buffer[2] + Buffer[3] + Buffer[4] + Buffer[5]) % 256;
	IFSendData(nChannel, Buffer, 7);

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
