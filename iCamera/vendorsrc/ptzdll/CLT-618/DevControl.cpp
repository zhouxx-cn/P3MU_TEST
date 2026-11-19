//!\file DevControl.cpp
//!\brief CLT-618协议实现文件
//!\author yuk@higheasyrd.com
//!\date 2009-7-14 中国安防科技有限公司武汉分公司
//!\history 
//!-1.增加左上、左下、右上、右下及电源指令码

#include <stdio.h>
#include "../DevControl.h"

#define DEV_TYPE	DRIVER_PTZ

//!\brief 支持的基本功能
//! 上下左右、变倍、变焦、光圈
#define PTZ_FLAGS	(PTZ_FLAG_PANTITLT | PTZ_FLAG_ZOOM | PTZ_FLAG_FOCUS | PTZ_FLAG_IRIS)

//!\brief 辅助功能
//! 灯光、雨刮、电源
#define PTZ_AUX_FLAG	(PTZ_FLAG_AUXPANTITLT | PTZ_FLAG_AUXLIGHT | PTZ_FLAG_AUXWIPER /*| PTZ_FLAG_AUXPOWER*/)

#define PTZ_TIMER_FLAG	(0)
#define	TIMER_PERIOD	(0)		//ms

//!\brief 支持的波特率，说明书上只支持9600
#define	BAUDRATE	(BD2400 | BD4800 | BD9600 | BD19200)

//!\brief 预置点数目
#define	PRESET_COUNT	(0)

//!\brief 巡航线路数
#define	CRUISE_COUNT	(0)

//!\brief 轨迹数
#define	TRACK_COUNT		(0)

//!\brief 线扫线路数
#define AUTO_COUNT		(0)

//!\brief 命令字长度
#define COMMAND_LEN		(6)

//!\brief 协议版本
#define PROTOCOL_VERSION "0.1"

const char s_szVersion[] = P_NAME " " PROTOCOL_VERSION "." BASEVERSION " BUILD " __DATE__ " " __TIME__;

static CbSendData IFSendData;

static CbRecvData IFRecvData;

int PtzNormalCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, unsigned long dwSpeed)
{
	ASSERT_SENDFN;
	unsigned char Buffer[6];

	Buffer[0] = 0xEB;
	Buffer[1] = 0x20;
	Buffer[2] = dwAddress;
	Buffer[3] = 0xAA;
	Buffer[5] = 0x55;

	switch (dwPtzCode)
	{
		case TTY_PTZ_UP:
			Buffer[4] = 0x00;
			break;
		case TTY_PTZ_DOWN:
			Buffer[4] = 0x01;
			break;
		case TTY_PTZ_LEFT:
			Buffer[4] = 0x02;
			break;
		case TTY_PTZ_RIGHT:
			Buffer[4] = 0x03;
			break;
		case TTY_PTZ_IRIS_LARGE:
			Buffer[4] = 0x06;
			break;
		case TTY_PTZ_IRIS_SMALL:
			Buffer[4] = 0x07;
			break;
		case TTY_PTZ_FOCUS_FAR:
			Buffer[4] = 0x05;
			break;
		case TTY_PTZ_FOCUS_NEAR:
			Buffer[4] = 0x04;
			break;
		case TTY_PTZ_ZOOMIN:
			Buffer[4] = 0x08;
			break;
		case TTY_PTZ_ZOOMOUT:
			Buffer[4] = 0x09;
			break;
		case TTY_PTZ_LEFTUP:
			Buffer[4] = 0x15;
			break;
		case TTY_PTZ_LEFTDOWN:
			Buffer[4] = 0x23;
			break;
		case TTY_PTZ_RIGHTUP:
			Buffer[4] = 0x1A;
			break;
		case TTY_PTZ_RIGHTDOWN:
			Buffer[4] = 0x2A;
			break;
		case TTY_PTZ_STOP:
		case TTY_PTZ_UP_STOP:
		case TTY_PTZ_DOWN_STOP:
		case TTY_PTZ_LEFT_STOP:
		case TTY_PTZ_RIGHT_STOP:
		case TTY_PTZ_IRIS_STOP:
		case TTY_PTZ_FOCUS_STOP:
		case TTY_PTZ_ZOOM_STOP:
		case TTY_PTZ_LEFTUP_STOP:
		case TTY_PTZ_LEFTDOWN_STOP:
		case TTY_PTZ_RIGHTUP_STOP:
		case TTY_PTZ_RIGHTDOWN_STOP:
			Buffer[4] = 0xFF;
			break;

		case TTY_PTZ_UP_AUTO:
		case TTY_PTZ_DOWN_AUTO:
		case TTY_PTZ_LEFT_AUTO:
		case TTY_PTZ_RIGHT_AUTO:
		case TTY_PTZ_IRIS_AUTO:
		case TTY_PTZ_FOCUS_AUTO:
		case TTY_PTZ_ZOOM_AUTO:
			return DLL_INPUT_OK;

		default:
			return DLL_ERROR;
	}

	IFSendData(nChannel, Buffer, COMMAND_LEN);

	return DLL_OUTPUT_OK;
}

int PtzAuxCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, int bOn)
{
	ASSERT_SENDFN;
	unsigned char Buffer[COMMAND_LEN];

	Buffer[0] = 0xEB;
	Buffer[1] = 0x20;
	Buffer[2] = dwAddress;
	Buffer[3] = 0xAA;
	Buffer[5] = 0x55;
	switch (dwPtzCode)
	{
		case TTY_AUX_LIGHT:
			if (bOn)
			{
				Buffer[4] = 0x31;
			}
			else
			{
				Buffer[4] = 0x32;
			}
			break;
		case TTY_AUX_WIPER:
			if (bOn)
			{
				Buffer[4] = 0x2B;
			}
			else
			{
				Buffer[4] = 0x0B;
			}
			break;
		case TTY_AUX_POWER:
			if (bOn)
			{
				Buffer[4] = 0x0D;
			}
			else
			{
				Buffer[4] = 0x0C;
			}
			break;
		case TTY_AUX_COOLER:
		case TTY_AUX_HEATER:
			break;
		default:
			return DLL_ERROR;
	}

	IFSendData(nChannel, Buffer, COMMAND_LEN);

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
		case TTY_PTZ_AUTO:
		case TTY_PTZ_SETROUTE:
		case TTY_PTZ_TRACKCALL:
			return DLL_INPUT_OK;
			break;
	}

	return DLL_ERROR;
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
	//printf("SetCallback\n");
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
