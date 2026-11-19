//!\file DevControl.cpp
//!\brief Pelcod_HTZ协议实现文件
//! 参考文档 宏天智PELCO_D_protocol code.doc
//!\author yuk@higheasyrd.com
//!\date 2009-9-9 中国安防科技有限公司武汉分公司

#include <stdio.h>
#include "../DevControl.h"

//device type
#define    DEV_TYPE    DRIVER_PTZ

#define    PTZ_FLAGS	(PTZ_FLAG_PANTITLT|PTZ_FLAG_ZOOM|PTZ_FLAG_FOCUS|PTZ_FLAG_IRIS)

#define    PTZ_AUX_FLAG	(PTZ_FLAG_AUXPRESET |PTZ_FLAG_AUXAUTO |PTZ_FLAG_AUXCRUISE|PTZ_FLAG_AUXTRACK)

#define    PTZ_TIMER_FLAG    (0)
#define    TIMER_PERIOD        0	//ms


#define    BAUDRATE    (BD2400 | BD4800 | BD9600 | BD19200)

#define    PRESET_COUNT		256
#define    TRACK_COUNT		1	//轨迹最多为0
#define    CRUISE_COUNT		1	//巡航数最多为1
#define    AUTO_COUNT		1

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
	unsigned char Buffer[7];

	Buffer[0] = 0xFF;
	Buffer[1] = dwAddress;

	const int fast = 0x3F;

	const int slow = 0;

	dwSpeed = SPEED(dwSpeed, fast, slow);

	switch (dwPtzCode)
	{
		case TTY_PTZ_UP:
			Buffer[2] = 0;
			Buffer[3] = 1 << 3;
			Buffer[4] = 0;
			Buffer[5] = dwSpeed;
			break;
		case TTY_PTZ_DOWN:
			Buffer[2] = 0;
			Buffer[3] = 1 << 4;
			Buffer[4] = 0;
			Buffer[5] = dwSpeed;
			break;
		case TTY_PTZ_LEFT:
			Buffer[2] = 0;
			Buffer[3] = 1 << 2;
			Buffer[4] = dwSpeed;
			Buffer[5] = 0;
			break;
		case TTY_PTZ_RIGHT:
			Buffer[2] = 0;
			Buffer[3] = 1 << 1;
			Buffer[4] = dwSpeed;
			Buffer[5] = 0;
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

		case TTY_PTZ_ZOOMIN:	//[3] = 0x40
			Buffer[2] = 0;
			Buffer[3] = 0x40;
			Buffer[4] = 0;
			Buffer[5] = 0;
			break;
		case TTY_PTZ_ZOOMOUT:	//[3] = 0x20
			Buffer[2] = 0;
			Buffer[3] = 0x20;
			Buffer[4] = 0;
			Buffer[5] = 0;
			break;
		case TTY_PTZ_IRIS_SMALL:	//[2] = 0x04
			Buffer[2] = 0x04;
			Buffer[3] = 0;
			Buffer[4] = 0;
			Buffer[5] = 0;
			break;
		case TTY_PTZ_IRIS_LARGE:	//[2] = 0x02
			Buffer[2] = 0x02;
			Buffer[3] = 0;
			Buffer[4] = 0;
			Buffer[5] = 0;
			break;
		case TTY_PTZ_FOCUS_FAR:	//[3] = 0x80
			Buffer[2] = 0x00;
			Buffer[3] = 0x80;
			Buffer[4] = 0;
			Buffer[5] = 0;
			break;
		case TTY_PTZ_FOCUS_NEAR:	//[2] = 0x01
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

int PtzAuxCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, int bOn)	//辅助控制
{
	switch (dwPtzCode)
	{
		case TTY_AUX_HEATER:
		case TTY_AUX_COOLER:
		case TTY_AUX_POWER:
		case TTY_AUX_WIPER:
		case TTY_AUX_LIGHT:
			return DLL_INPUT_OK;
		default:
			break;
	}

	return DLL_ERROR;
}

int PtzAutoCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, unsigned long dwPresetIndex1, unsigned long dwPresetIndex2, unsigned long dwCruiseIndex, unsigned long dwSpeed, unsigned long dwDwellTime, int bScan)	//自动控制
{
	ASSERT_SENDFN;
	unsigned char Buffer[7];

	Buffer[0] = 0xFF;
	Buffer[1] = dwAddress;

	Buffer[4] = 0;
	Buffer[5] = 0;

	const int fast = 63;

	const int slow = 0;

	dwSpeed = SPEED(dwSpeed, fast, slow);

	switch (dwPtzCode)
	{
			//51# 52#预置点间扫描
		case TTY_PTZ_AUTO:
			{
				Buffer[3] = 0;
				Buffer[4] = 0;
				Buffer[5] = 0;
				if (bScan)
				{
					Buffer[2] = 0x90;
				}
				else
				{
					Buffer[2] = 0x10;
				}
				break;
			}
			//在预置点1~16之间巡航
		case TTY_PTZ_SETROUTE:	//<! 开始自动巡航
			{
				if (bScan)
				{
					Buffer[2] = 0;
					Buffer[3] = 0x07;
					Buffer[4] = 0;
					Buffer[5] = 0x35;
				}
				else
				{
					Buffer[2] = 0;
					Buffer[3] = 0;
					Buffer[4] = 0;
					Buffer[5] = 0;
				}
				break;
			}
			//轨迹有六条
		case TTY_PTZ_TRACKCALL:
			if (bScan)
			{
				Buffer[2] = 0;
				Buffer[3] = 0x07;
				Buffer[4] = 0;
				Buffer[5] = 0x33;
			}
			else
			{
				PtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_RIGHT, 0);
				PtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_RIGHT_STOP, 0);
			}
			break;
		default:
			return DLL_ERROR;
	}
	Buffer[6] = (Buffer[1] + Buffer[2] + Buffer[3] + Buffer[4] + Buffer[5]) % 256;
	IFSendData(nChannel, Buffer, 7);

	return DLL_OUTPUT_OK;
}

int PtzPresetCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, unsigned long dwPresetIndex, unsigned long dwCruiseIndex, unsigned long dwSpeed, unsigned long dwDwellTime)	//预置点控制
{
	ASSERT_SENDFN;
	unsigned char Buffer[7];

	Buffer[0] = 0xFF;
	Buffer[1] = dwAddress;

	Buffer[2] = 0;
	Buffer[4] = 0;

	switch (dwPtzCode)
	{
		case TTY_PTZ_PRESETSET:	//设置预置点
			Buffer[3] = 0x03;
			Buffer[5] = dwPresetIndex + 1;
			break;
		case TTY_PTZ_PRESETCALL:	//调用预置点
			Buffer[3] = 0x07;
			Buffer[5] = dwPresetIndex + 1;
			break;
		case TTY_PTZ_PRESETMOVE:	//删除预置点
			Buffer[3] = 0x05;
			Buffer[5] = dwPresetIndex + 1;
			break;
		case TTY_PTZ_TRACKSET:
			Buffer[3] = 0x03;
			Buffer[5] = 0x33;
			break;
		case TTY_PTZ_TRACKEND:
			Buffer[3] = 0x03;
			Buffer[5] = 0x34;
			break;
			//线扫也不能设置
		case TTY_PTZ_AUTOSET:
		case TTY_PTZ_AUTOEND:
			return DLL_INPUT_OK;
			break;

/*
	//巡航线路不能编辑
    case TTY_PTZ_ROUTESET://开始设置巡航
	{
		Buffer[3] = 0x07;
		Buffer[5] = 0x8C;
		break;
	}
    case TTY_PTZ_ROUTEADDPOINT: //添加巡航点
	{
		Buffer[3] = 0x07;
		Buffer[5] = dwPresetIndex + 1;
		break;
	}
    case TTY_PTZ_ROUTEEND:  //结束巡航设置
	{
		Buffer[3] = 0x07;
		Buffer[5] = 0x8D;
		break;
	}
*/
		case TTY_PTZ_ROUTEREMOVEPOINT:
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
