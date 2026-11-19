//!\file DevControl.cpp
//!\brief VIDO协议实现文件
//! 参考文档 REQ_86_VTS控制协议.doc
//!\author yuk@higheasyrd.com
//!\date 2009-9-9 中国安防科技有限公司武汉分公司

#include <stdio.h>
#include "../DevControl.h"

//device type
#define	DEV_TYPE	DRIVER_PTZ

#define	PTZ_FLAGS	(PTZ_FLAG_PANTITLT|PTZ_FLAG_ZOOM|PTZ_FLAG_FOCUS|PTZ_FLAG_IRIS)

#define	PTZ_AUX_FLAG	(PTZ_FLAG_AUXPRESET | PTZ_FLAG_AUXPANTITLT |PTZ_FLAG_AUXTRACK | PTZ_FLAG_AUXPRESETAUTO | PTZ_FLAG_AUXCRUISE)

#define  PTZ_TIMER_FLAG	(0)
#define	TIMER_PERIOD		0	//ms


#define	BAUDRATE	( BD9600)

#define	PRESET_COUNT	256
#define	TRACK_COUNT		1		//轨迹最多为0
#define	CRUISE_COUNT	1		//巡航数最多为1,调用53(0x34)号预置点
#define AUTO_COUNT		1

#define PROTOCOL_VERSION "0.1"

const char s_szVersion[] = P_NAME " " PROTOCOL_VERSION "." BASEVERSION " BUILD " __DATE__ " " __TIME__;

static CbSendData IFSendData;

static CbRecvData IFRecvData;

static CbStartCruise IFStartCruise;

static CbStopCruise IFStopCruise;

static void ParityByte(unsigned char Buffer[11]);

//云台普通控制
int PtzNormalCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, unsigned long dwSpeed)
{
	ASSERT_SENDFN;
	unsigned char Buffer[11];

	Buffer[0] = 0xA0;
	Buffer[1] = dwAddress;
	Buffer[2] = dwAddress;
	Buffer[3] = 0;
	Buffer[4] = 0;
	Buffer[5] = 0;
	Buffer[6] = 0;
	Buffer[7] = 0;
	Buffer[8] = 0xFF;
	Buffer[9] = 0xAF;

	const int fast = 0x40;

	const int slow = 0x01;

	dwSpeed = SPEED(dwSpeed, fast, slow);

	switch (dwPtzCode)
	{
		case TTY_PTZ_UP:
			Buffer[4] = 0x8;
			Buffer[6] = dwSpeed;
			ParityByte(Buffer);
			IFSendData(nChannel, Buffer, 11);
			Buffer[4] = 0x08 | 0x10;
			break;
		case TTY_PTZ_DOWN:
			Buffer[4] = 0x10;
			Buffer[6] = dwSpeed;
			ParityByte(Buffer);
			IFSendData(nChannel, Buffer, 11);
			Buffer[4] = 0x08 | 0x10;
			break;
		case TTY_PTZ_LEFT:
			Buffer[4] = 0x4;
			Buffer[5] = dwSpeed;
			ParityByte(Buffer);
			IFSendData(nChannel, Buffer, 11);
			Buffer[4] = 0x04 | 0x02;
			break;
		case TTY_PTZ_RIGHT:
			Buffer[4] = 0x2;
			Buffer[5] = dwSpeed;
			ParityByte(Buffer);
			IFSendData(nChannel, Buffer, 11);
			Buffer[4] = 0x04 | 0x02;
			break;
		case TTY_PTZ_LEFTUP:
			Buffer[4] = 0x4 | 0x8;
			Buffer[5] = dwSpeed;
			Buffer[6] = dwSpeed;
			ParityByte(Buffer);
			IFSendData(nChannel, Buffer, 11);
			Buffer[4] = 0x04 | 0x02 | 0x8 | 0x10;
			break;
		case TTY_PTZ_LEFTDOWN:
			Buffer[4] = 0x4 | 0x10;
			Buffer[5] = dwSpeed;
			Buffer[6] = dwSpeed;
			ParityByte(Buffer);
			IFSendData(nChannel, Buffer, 11);
			Buffer[4] = 0x04 | 0x02 | 0x8 | 0x10;
			break;
		case TTY_PTZ_RIGHTUP:
			Buffer[4] = 0x2 | 0x8;
			Buffer[5] = dwSpeed;
			Buffer[6] = dwSpeed;
			ParityByte(Buffer);
			IFSendData(nChannel, Buffer, 11);
			Buffer[4] = 0x04 | 0x02 | 0x8 | 0x10;
			break;
		case TTY_PTZ_RIGHTDOWN:
			Buffer[4] = 0x2 | 0x10;
			Buffer[5] = dwSpeed;
			Buffer[6] = dwSpeed;
			ParityByte(Buffer);
			IFSendData(nChannel, Buffer, 11);
			Buffer[4] = 0x04 | 0x02 | 0x8 | 0x10;
			break;
		case TTY_PTZ_ZOOMIN:
			Buffer[4] = 0x20;
			Buffer[7] = dwSpeed;
			ParityByte(Buffer);
			IFSendData(nChannel, Buffer, 11);
			Buffer[4] = 0x20 | 0x40;
			break;
		case TTY_PTZ_ZOOMOUT:
			Buffer[4] = 0x40;
			Buffer[7] = dwSpeed;
			ParityByte(Buffer);
			IFSendData(nChannel, Buffer, 11);
			Buffer[4] = 0x20 | 0x40;
			break;
		case TTY_PTZ_IRIS_LARGE:
			Buffer[3] = 0x10;
			Buffer[7] = dwSpeed;
			ParityByte(Buffer);
			IFSendData(nChannel, Buffer, 11);
			Buffer[3] = 0x8 | 0x10;
			break;
		case TTY_PTZ_IRIS_SMALL:
			Buffer[3] = 0x8;
			Buffer[7] = dwSpeed;
			ParityByte(Buffer);
			IFSendData(nChannel, Buffer, 11);
			Buffer[3] = 0x8 | 0x10;
			break;
		case TTY_PTZ_FOCUS_NEAR:
			Buffer[3] = 0x2;
			Buffer[7] = dwSpeed;
			ParityByte(Buffer);
			IFSendData(nChannel, Buffer, 11);
			Buffer[3] = 0x1 | 0x2;
			break;
		case TTY_PTZ_FOCUS_FAR:
			Buffer[3] = 0x1;
			Buffer[7] = dwSpeed;
			ParityByte(Buffer);
			IFSendData(nChannel, Buffer, 11);
			Buffer[3] = 0x1 | 0x2;
			break;
		case TTY_PTZ_IRIS_AUTO:
		case TTY_PTZ_ZOOM_AUTO:
		case TTY_PTZ_FOCUS_AUTO:
		case TTY_PTZ_LEFT_AUTO:
		case TTY_PTZ_RIGHT_AUTO:
		case TTY_PTZ_UP_AUTO:
		case TTY_PTZ_DOWN_AUTO:
			return DLL_INPUT_OK;
			break;
		case TTY_PTZ_IRIS_STOP:
		case TTY_PTZ_ZOOM_STOP:
		case TTY_PTZ_FOCUS_STOP:
		case TTY_PTZ_LEFT_STOP:
		case TTY_PTZ_RIGHT_STOP:
		case TTY_PTZ_UP_STOP:
		case TTY_PTZ_DOWN_STOP:
		case TTY_PTZ_STOP:
		case TTY_PTZ_LEFTUP_STOP:
		case TTY_PTZ_LEFTDOWN_STOP:
		case TTY_PTZ_RIGHTUP_STOP:
		case TTY_PTZ_RIGHTDOWN_STOP:
			Buffer[7] = 0x44;
			break;
		default:
			break;
	}

	ParityByte(Buffer);
	IFSendData(nChannel, Buffer, 11);

	return DLL_OUTPUT_OK;
}

//辅助控制
int PtzAuxCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, int bOn)
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



//自动巡航开关，自动扫描操作
int PtzAutoCtrl(unsigned int nChannel,
				unsigned long dwAddress,
				unsigned long dwPtzCode,
				unsigned long dwPresetIndex1,
				unsigned long dwPresetIndex2, unsigned long dwCruiseIndex, unsigned long dwSpeed, unsigned long dwDwellTime, int bScan)
{
	ASSERT_SENDFN;
	unsigned char Buffer[11];

	Buffer[0] = 0xA0;
	Buffer[1] = dwAddress;
	Buffer[2] = dwAddress;
	Buffer[3] = 0;
	Buffer[4] = 0;
	Buffer[5] = 0;
	Buffer[6] = 0;
	Buffer[7] = 0;
	Buffer[8] = 0xFF;
	Buffer[9] = 0xAF;

	switch (dwPtzCode)
	{
		case TTY_PTZ_SETROUTE:	// 开始自动巡航
			{
				if (dwCruiseIndex >= CRUISE_COUNT)
				{
					return DLL_ERROR;
				}

				if (bScan)
				{
					Buffer[3] = 0;
					Buffer[4] = 0x7;
					Buffer[5] = 0x34;
					Buffer[6] = 0xFF;
					Buffer[7] = 0xFF;
				}
				else
				{
					Buffer[3] = 0;
					Buffer[4] = 0;
					Buffer[5] = 0;
					Buffer[6] = 0;
					Buffer[7] = 0x44;
				}
				break;
			}
		case TTY_PTZ_AUTO:		// 自动
			{
				if (dwCruiseIndex >= AUTO_COUNT)
				{
					return DLL_ERROR;
				}

				if (bScan)
				{
					Buffer[3] = 0;
					Buffer[4] = 0x1B;
					Buffer[5] = 0;
					Buffer[6] = 0xFF;
					Buffer[7] = 0xFF;
				}
				else
				{
					Buffer[3] = 0;
					Buffer[4] = 0x1f;
					Buffer[5] = 0xFF;
					Buffer[6] = 0xFF;
					Buffer[7] = 0xFF;
				}
				break;
			}
		case TTY_PTZ_TRACKCALL:
			if (dwCruiseIndex >= TRACK_COUNT)
			{
				return DLL_ERROR;
			}

			if (bScan)
			{
				Buffer[3] = 0;
				Buffer[4] = 0x7;
				Buffer[5] = 0x32;
				Buffer[6] = 0xFF;
				Buffer[7] = 0xFF;
			}
			else
			{
				Buffer[3] = 0;
				Buffer[4] = 0x2F;
				Buffer[5] = 0xFF;
				Buffer[6] = 0xFF;
				Buffer[7] = 0xFF;
			}
			break;
		default:
			break;
	}

	ParityByte(Buffer);
	IFSendData(nChannel, Buffer, 11);

	return DLL_OUTPUT_OK;
}


//预置点相关操作,巡航，轨迹的设置添加等
int PtzPresetCtrl(unsigned int nChannel,
				  unsigned long dwAddress,
				  unsigned long dwPtzCode, unsigned long dwPresetIndex, unsigned long dwCruiseIndex, unsigned long dwSpeed, unsigned long dwDwellTime)
{
	ASSERT_SENDFN;
	unsigned char Buffer[11];

	static int nPreset1 = 0;

	static int nPreset2 = 0;

	const int fast = 1;

	const int slow = 8;

	dwSpeed = SPEED(dwSpeed, fast, slow);

	Buffer[0] = 0xA0;
	Buffer[1] = dwAddress;
	Buffer[2] = dwAddress;
	Buffer[3] = 0;
	Buffer[4] = 0;
	Buffer[5] = 0;
	Buffer[6] = 0;
	Buffer[7] = 0;
	Buffer[8] = 0xFF;
	Buffer[9] = 0xAF;

	switch (dwPtzCode)
	{
		case TTY_PTZ_PRESETSET:	//设置预置点
			Buffer[4] = 0x03;
			Buffer[5] = 0;
			Buffer[6] = 0xFF;
			Buffer[7] = 0xFF;
			Buffer[8] = 0xFF;
			break;
		case TTY_PTZ_PRESETCALL:	//调用预置点
			Buffer[4] = 0x07;
			Buffer[5] = dwPresetIndex;
			Buffer[6] = 0xFF;
			Buffer[7] = 0xFF;
			Buffer[8] = 0xFF;
			break;
		case TTY_PTZ_PRESETMOVE:	//删除预置点
			{
				return DLL_INPUT_OK;
				break;
			}
		case TTY_PTZ_ROUTESET:	//开始设置巡航
			return DLL_INPUT_OK;
			if (dwCruiseIndex >= CRUISE_COUNT)
			{
				return DLL_ERROR;
			}
			Buffer[4] = 0x23;
			Buffer[5] = dwCruiseIndex;
			Buffer[6] = 0x80;
			Buffer[7] = 0xFF;
			Buffer[8] = 0xFF;
			break;
		case TTY_PTZ_ROUTEEND:	//结束巡航设置
			{
				return DLL_INPUT_OK;
				if (dwCruiseIndex >= CRUISE_COUNT)
				{
					return DLL_ERROR;
				}
				Buffer[4] = 0x23;
				Buffer[5] = dwCruiseIndex;
				Buffer[6] = 0x40;
				Buffer[7] = 0xFF;
				Buffer[8] = 0xFF;
				break;
			}
		case TTY_PTZ_ROUTEADDPOINT:	//添加巡航点
			{
				return DLL_INPUT_OK;
				if (dwCruiseIndex >= CRUISE_COUNT)
				{
					return DLL_ERROR;
				}
				Buffer[4] = 0x23;
				Buffer[5] = dwCruiseIndex;
				Buffer[6] = dwPresetIndex;
				Buffer[7] = dwSpeed;
				Buffer[8] = dwDwellTime;
				break;
			}
		case TTY_PTZ_TRACKSET:
			Buffer[4] = 0x03;
			Buffer[5] = 0x32;
			Buffer[6] = 0xFF;
			Buffer[7] = 0xFF;
			Buffer[8] = 0xFF;
			break;
		case TTY_PTZ_TRACKEND:
			Buffer[4] = 0x03;
			Buffer[5] = 0x33;
			Buffer[6] = 0xFF;
			Buffer[7] = 0xFF;
			Buffer[8] = 0xFF;
			break;
		case TTY_PTZ_AUTOSET:
			nPreset1 = dwPresetIndex;
			return DLL_OUTPUT_OK;
			break;
		case TTY_PTZ_AUTOEND:
			nPreset2 = dwPresetIndex;
			Buffer[4] = 0x19;
			Buffer[5] = 0;
			Buffer[6] = nPreset1;
			Buffer[7] = nPreset2;
			Buffer[8] = 0xFF;
			break;
		default:
			{
				return DLL_ERROR;
				break;
			}
	}

	ParityByte(Buffer);
	IFSendData(nChannel, Buffer, 11);

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

void ParityByte(unsigned char Buffer[11])
{
	Buffer[10] = (0xFFFF - (Buffer[1] + Buffer[2] + Buffer[3] + Buffer[4] + Buffer[5] + Buffer[6] + Buffer[7] + Buffer[8])) % 0x100;
}
