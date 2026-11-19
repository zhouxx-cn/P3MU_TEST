#include <stdio.h>
#include "../DevControl.h"

//device type
#define	DEV_TYPE	DRIVER_PTZ

//!\brief 支持的基本功能 上下左右,变倍,变焦,光圈
#define	PTZ_FLAGS	(PTZ_FLAG_PANTITLT | PTZ_FLAG_ZOOM | PTZ_FLAG_FOCUS | PTZ_FLAG_IRIS)

//!\brief 支持的扩展功能 左上、左下、右上、右下、灯光,雨刷,预置点,自动扫,巡航
#define	PTZ_AUX_FLAG	(PTZ_FLAG_AUXPANTITLT | PTZ_FLAG_AUXLIGHT | PTZ_FLAG_AUXWIPER/* | PTZ_FLAG_AUXPOWER*/ | PTZ_FLAG_AUXPRESET | PTZ_FLAG_AUXPRESETAUTO | PTZ_FLAG_AUXCRUISE | PTZ_FLAG_AUXMENUCTRL)

#define  PTZ_TIMER_FLAG	(0)
#define	TIMER_PERIOD		0	//ms

#define	BAUDRATE	(BD9600)

#define	PRESET_COUNT	256
#define	CRUISE_COUNT	6		//巡航数最多为1
#define	TRACK_COUNT		0
#define AUTO_COUNT		1

#define PROTOCOL_VERSION "0.1"

const char s_szVersion[] = P_NAME " " PROTOCOL_VERSION "." BASEVERSION " BUILD " __DATE__ " " __TIME__;

static CbSendData IFSendData;

static CbRecvData IFRecvData;

static CbStartCruise IFStartCruise;

static CbStopCruise IFStopCruise;	//##ModelId=49A36CC30148

static void ParityByte(unsigned char Buffer[11]);

//云台普通控制
int PtzNormalCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, unsigned long dwSpeed)
{
	ASSERT_SENDFN;

	unsigned char Buffer[11];

	Buffer[0] = 0xA0;
	Buffer[1] = dwAddress;
	Buffer[2] = 0x00;

	Buffer[8] = 0xFF;
	Buffer[9] = 0xAF;

	switch (dwPtzCode)
	{
		case TTY_PTZ_UP:
			dwSpeed = SPEED(dwSpeed, 0x40, 0);
			Buffer[3] = 0;
			Buffer[4] = 0x08;
			Buffer[5] = 0x00;
			Buffer[6] = dwSpeed;
			Buffer[7] = 0;
			Buffer[8] = 0xFF;
			ParityByte(Buffer);
			IFSendData(nChannel, Buffer, 11);
			Buffer[4] = 0x08 | 0x10;
			Buffer[6] = 0;
			break;
		case TTY_PTZ_DOWN:
			dwSpeed = SPEED(dwSpeed, 0x40, 0);
			Buffer[3] = 0;
			Buffer[4] = 0x10;
			Buffer[5] = 0x00;
			Buffer[6] = dwSpeed;
			Buffer[7] = 0;
			Buffer[8] = 0xFF;
			ParityByte(Buffer);
			IFSendData(nChannel, Buffer, 11);
			Buffer[4] = 0x08 | 0x10;
			Buffer[6] = 0;
			break;
		case TTY_PTZ_LEFT:
			dwSpeed = SPEED(dwSpeed, 0x40, 0);
			Buffer[3] = 0;
			Buffer[4] = 0x04;
			Buffer[5] = dwSpeed;
			Buffer[6] = 0x00;
			Buffer[7] = 0;
			Buffer[8] = 0xFF;
			ParityByte(Buffer);
			IFSendData(nChannel, Buffer, 11);
			Buffer[4] = 0x04 | 0x02;
			Buffer[5] = 0;
			break;
		case TTY_PTZ_RIGHT:
			dwSpeed = SPEED(dwSpeed, 0x40, 0);
			Buffer[3] = 0;
			Buffer[4] = 0x02;
			Buffer[5] = dwSpeed;
			Buffer[6] = 0x00;
			Buffer[7] = 0;
			Buffer[8] = 0xFF;
			ParityByte(Buffer);
			IFSendData(nChannel, Buffer, 11);
			Buffer[4] = 0x04 | 0x02;
			Buffer[5] = 0;
			break;
		case TTY_PTZ_UP_AUTO:
		case TTY_PTZ_DOWN_AUTO:
		case TTY_PTZ_LEFT_AUTO:
		case TTY_PTZ_RIGHT_AUTO:
			return DLL_INPUT_OK;
			break;
		case TTY_PTZ_LEFTUP:
			dwSpeed = SPEED(dwSpeed, 0x40, 0);
			Buffer[3] = 0;
			Buffer[4] = 0x04 | 0x08;
			Buffer[5] = dwSpeed;
			Buffer[6] = dwSpeed;
			Buffer[7] = 0;
			Buffer[8] = 0xFF;
			ParityByte(Buffer);
			IFSendData(nChannel, Buffer, 11);
			Buffer[4] = 0x08 | 0x10 | 0x04 | 0x02;
			Buffer[5] = 0;
			Buffer[6] = 0;
			break;
		case TTY_PTZ_RIGHTUP:
			dwSpeed = SPEED(dwSpeed, 0x40, 0);
			Buffer[3] = 0;
			Buffer[4] = 0x02 | 0x08;
			Buffer[5] = dwSpeed;
			Buffer[6] = dwSpeed;
			Buffer[7] = 0;
			Buffer[8] = 0xFF;
			ParityByte(Buffer);
			IFSendData(nChannel, Buffer, 11);
			Buffer[4] = 0x08 | 0x10 | 0x04 | 0x02;
			Buffer[5] = 0;
			Buffer[6] = 0;
			break;
		case TTY_PTZ_LEFTDOWN:
			dwSpeed = SPEED(dwSpeed, 0x40, 0);
			Buffer[3] = 0;
			Buffer[4] = 0x04 | 0x10;
			Buffer[5] = dwSpeed;
			Buffer[6] = dwSpeed;
			Buffer[7] = 0;
			Buffer[8] = 0xFF;
			ParityByte(Buffer);
			IFSendData(nChannel, Buffer, 11);
			Buffer[4] = 0x08 | 0x10 | 0x04 | 0x02;
			Buffer[5] = 0;
			Buffer[6] = 0;
			break;
		case TTY_PTZ_RIGHTDOWN:
			dwSpeed = SPEED(dwSpeed, 0x40, 0);
			Buffer[3] = 0;
			Buffer[4] = 0x02 | 0x10;
			Buffer[5] = dwSpeed;
			Buffer[6] = dwSpeed;
			Buffer[7] = 0;
			Buffer[8] = 0xFF;
			ParityByte(Buffer);
			IFSendData(nChannel, Buffer, 11);
			Buffer[4] = 0x08 | 0x10 | 0x04 | 0x02;
			Buffer[5] = 0;
			Buffer[6] = 0;
			break;
		case TTY_PTZ_ZOOMIN:	//变倍大
			dwSpeed = SPEED(dwSpeed, 0x08, 0x01);
			Buffer[3] = 0;
			Buffer[4] = 0x40;
			Buffer[5] = 0;
			Buffer[6] = 0;
			Buffer[7] = dwSpeed;
			Buffer[8] = 0xFF;
			ParityByte(Buffer);
			IFSendData(nChannel, Buffer, 11);
			Buffer[4] = 0x40 | 0x20;
			Buffer[5] = 0;
			Buffer[6] = 0;
			break;
		case TTY_PTZ_ZOOMOUT:	//变倍小
			dwSpeed = SPEED(dwSpeed, 0x08, 0x01);
			Buffer[3] = 0;
			Buffer[4] = 0x20;
			Buffer[5] = 0;
			Buffer[6] = 0;
			Buffer[7] = dwSpeed;
			Buffer[8] = 0xFF;
			ParityByte(Buffer);
			IFSendData(nChannel, Buffer, 11);
			Buffer[4] = 0x40 | 0x20;
			Buffer[5] = 0;
			Buffer[6] = 0;
			break;
		case TTY_PTZ_ZOOM_AUTO:
			Buffer[3] = 0;
			Buffer[4] = 0x50;
			Buffer[5] = 0;
			Buffer[6] = 0;
			Buffer[7] = 0x08;
			Buffer[8] = 0xFF;
			break;
		case TTY_PTZ_IRIS_SMALL:	//光圈小
			Buffer[3] = 0x08;
			Buffer[4] = 0x00;
			Buffer[5] = 0;
			Buffer[6] = 0x00;
			Buffer[7] = 0;
			Buffer[8] = 0xFF;
			ParityByte(Buffer);
			IFSendData(nChannel, Buffer, 11);
			Buffer[3] = 0x08 | 0x10;
			break;
		case TTY_PTZ_IRIS_LARGE:	//光圈大
			Buffer[3] = 0x10;
			Buffer[4] = 0x00;
			Buffer[5] = 0;
			Buffer[6] = 0x00;
			Buffer[7] = 0;
			Buffer[8] = 0xFF;
			ParityByte(Buffer);
			IFSendData(nChannel, Buffer, 11);
			Buffer[3] = 0x08 | 0x10;
			break;
		case TTY_PTZ_IRIS_AUTO:
			Buffer[3] = 0x18;
			Buffer[4] = 0x00;
			Buffer[5] = 0;
			Buffer[6] = 0x00;
			Buffer[7] = 0;
			Buffer[8] = 0xFF;
			break;
		case TTY_PTZ_FOCUS_NEAR:	//聚焦
			dwSpeed = SPEED(dwSpeed, 0x08, 0x01);
			Buffer[3] = 0x02;
			Buffer[4] = 0;
			Buffer[5] = 0;
			Buffer[6] = 0x00;
			Buffer[7] = dwSpeed << 4;
			Buffer[8] = 0xFF;
			ParityByte(Buffer);
			IFSendData(nChannel, Buffer, 11);
			Buffer[3] = 0x02 | 0x01;
			break;
		case TTY_PTZ_FOCUS_FAR:
			Buffer[3] = 0x01;
			Buffer[4] = 0;
			Buffer[5] = 0;
			Buffer[6] = 0x00;
			Buffer[7] = dwSpeed << 4;
			Buffer[8] = 0xFF;
			ParityByte(Buffer);
			IFSendData(nChannel, Buffer, 11);
			Buffer[3] = 0x02 | 0x01;
			break;
		case TTY_PTZ_FOCUS_AUTO:
			Buffer[3] = 0x03;
			Buffer[4] = 0;
			Buffer[5] = 0;
			Buffer[6] = 0x00;
			Buffer[7] = 0x80;
			Buffer[8] = 0xFF;
			break;
		case TTY_PTZ_ZOOM_STOP:
		case TTY_PTZ_IRIS_STOP:
		case TTY_PTZ_FOCUS_STOP:
		case TTY_PTZ_UP_STOP:
		case TTY_PTZ_DOWN_STOP:
		case TTY_PTZ_LEFT_STOP:
		case TTY_PTZ_RIGHT_STOP:
		case TTY_PTZ_STOP:
		case TTY_PTZ_LEFTUP_STOP:
		case TTY_PTZ_LEFTDOWN_STOP:
		case TTY_PTZ_RIGHTUP_STOP:
		case TTY_PTZ_RIGHTDOWN_STOP:
			Buffer[3] = 0x00;
			Buffer[4] = 0;
			Buffer[5] = 0;
			Buffer[6] = 0x00;
			Buffer[7] = 0x00;
			Buffer[8] = 0xFF;
			break;
		default:
			return DLL_ERROR;
	}

	ParityByte(Buffer);
	IFSendData(nChannel, Buffer, 11);

	return (int) DLL_OUTPUT_OK;
}

int PtzAuxCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, int bOn)
{
	ASSERT_SENDFN;

	unsigned char Buffer[11];

	Buffer[0] = 0xA0;
	Buffer[1] = dwAddress;

	Buffer[2] = 0x00;
	Buffer[3] = 0x80;
	Buffer[4] = 0x01;

	switch (dwPtzCode)
	{
		case TTY_AUX_LIGHT:
			Buffer[5] = 0x0F;
			Buffer[6] = bOn;
			break;
		case TTY_AUX_WIPER:
			Buffer[5] = 0x0D;
			Buffer[6] = bOn;
			break;
		case TTY_AUX_POWER:
			Buffer[5] = 0x0E;
			Buffer[6] = bOn;
			break;
		default:
			return DLL_ERROR;
	}
	Buffer[7] = 0xFF;
	Buffer[8] = 0xFF;
	Buffer[9] = 0xAF;

	ParityByte(Buffer);
	IFSendData(nChannel, Buffer, 11);

	return (int) DLL_OUTPUT_OK;
}

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
	Buffer[2] = 0;
	Buffer[3] = 0;
	Buffer[9] = 0xAF;

	switch (dwPtzCode)
	{
		case TTY_PTZ_AUTO:
			if (bScan)
			{
				Buffer[4] = 0x11;
				Buffer[5] = 0x00;
				Buffer[6] = dwPresetIndex1;
				Buffer[7] = dwPresetIndex2;
				Buffer[8] = 0xFF;
				Buffer[9] = 0xAF;
			}
			else
			{
				Buffer[4] = 0x1F;
				Buffer[5] = 0xFF;
				Buffer[6] = 0xFF;
				Buffer[7] = 0xFF;
				Buffer[8] = 0xFF;
				Buffer[9] = 0xAF;
			}
			break;
		case TTY_PTZ_SETROUTE:
			if (bScan)
			{
				Buffer[4] = 0x21;
				Buffer[5] = dwCruiseIndex;
				Buffer[6] = 0;
				Buffer[7] = 0xFF;
				Buffer[8] = 0xFF;
				Buffer[9] = 0xAF;
			}
			else
			{
				Buffer[3] = 0x00;
				Buffer[4] = 0;
				Buffer[5] = 0;
				Buffer[6] = 0x00;
				Buffer[7] = 0x00;
				Buffer[8] = 0xFF;
				Buffer[9] = 0xAF;
			}
			break;
		default:
			return DLL_ERROR;
	}

	ParityByte(Buffer);
	IFSendData(nChannel, Buffer, 11);

	return DLL_OUTPUT_OK;
}

int PtzPresetCtrl(unsigned int nChannel,
				  unsigned long dwAddress,
				  unsigned long dwPtzCode, unsigned long dwPresetIndex, unsigned long dwCruiseIndex, unsigned long dwSpeed, unsigned long dwDwellTime)
{
	ASSERT_SENDFN;

	unsigned char Buffer[11];

	Buffer[0] = 0xA0;
	Buffer[1] = dwAddress;

	Buffer[2] = 0;
	Buffer[3] = 0;
	Buffer[9] = 0xAF;

	const int fast = 1;

	const int slow = 8;

	static int nPreset1 = 0;

	static int nPreset2 = 0;

	dwSpeed = SPEED(dwSpeed, 1, 8);

	switch (dwPtzCode)
	{
		case TTY_PTZ_PRESETSET:
			Buffer[4] = 0x03;
			Buffer[5] = dwPresetIndex;
			Buffer[6] = 0xFF;
			Buffer[7] = 0xFF;
			Buffer[8] = 0xFF;
			Buffer[9] = 0xAF;
			break;
		case TTY_PTZ_PRESETCALL:
			Buffer[4] = 0x07;
			Buffer[5] = dwPresetIndex;
			Buffer[6] = 0xFF;
			Buffer[7] = 0xFF;
			Buffer[8] = 0xFF;
			Buffer[9] = 0xAF;
			break;
		case TTY_PTZ_ROUTESET:
			Buffer[4] = 0x23;
			Buffer[5] = dwCruiseIndex;
			Buffer[6] = 0x80;
			Buffer[7] = 0xFF;
			Buffer[8] = 0xFF;
			Buffer[9] = 0xAF;
			break;
		case TTY_PTZ_ROUTEADDPOINT:
			Buffer[4] = 0x23;
			Buffer[5] = dwCruiseIndex;
			Buffer[6] = dwPresetIndex;
			Buffer[7] = dwSpeed;
			Buffer[8] = dwDwellTime;
			Buffer[9] = 0xAF;
			break;
		case TTY_PTZ_ROUTEEND:
			Buffer[4] = 0x23;
			Buffer[5] = dwCruiseIndex;
			Buffer[6] = 0x40;
			Buffer[7] = 0xFF;
			Buffer[8] = 0xFF;
			Buffer[9] = 0xAF;
			break;
		case TTY_PTZ_AUTOSET:
			return DLL_INPUT_OK;
			nPreset1 = dwPresetIndex;
			break;
		case TTY_PTZ_AUTOEND:
			return DLL_INPUT_OK;
			nPreset2 = dwPresetIndex;
			Buffer[4] = 0x19;
			Buffer[5] = 0x00;
			Buffer[6] = (unsigned char) nPreset1;
			Buffer[7] = (unsigned char) nPreset2;
			Buffer[8] = 0xFF;
			Buffer[9] = 0xAF;
			break;
		default:
			return DLL_ERROR;
	}

	ParityByte(Buffer);
	IFSendData(nChannel, Buffer, 11);

	return DLL_OUTPUT_OK;
}


int PtzMenuCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode)
{
	ASSERT_SENDFN;

	unsigned char Buffer[11];

	Buffer[0] = 0xA0;
	Buffer[1] = dwAddress;

	Buffer[2] = 0;
	Buffer[3] = 0;
	Buffer[9] = 0xAF;

	//本协议支持菜单操作
	switch (dwPtzCode)
	{
		case TTY_PTZ_MENU_ENTER:
			Buffer[4] = 0x43;
			Buffer[5] = 0x01;
			Buffer[6] = 0xFF;
			Buffer[7] = 0xFF;
			Buffer[8] = 0xFF;
			break;
		case TTY_PTZ_MENU_EXIT:
			Buffer[4] = 0x43;
			Buffer[5] = 0x00;
			Buffer[6] = 0xFF;
			Buffer[7] = 0xFF;
			Buffer[8] = 0xFF;
			break;
		case TTY_PTZ_MENU_UP:
			Buffer[3] = 0;
			Buffer[4] = 0x40;
			Buffer[5] = 0;
			Buffer[6] = 0;
			Buffer[7] = 0x08;
			Buffer[8] = 0xFF;
			break;
		case TTY_PTZ_MENU_DOWN:
			Buffer[3] = 0;
			Buffer[4] = 0x20;
			Buffer[5] = 0;
			Buffer[6] = 0;
			Buffer[7] = 0x08;
			Buffer[8] = 0xFF;
			break;
		case TTY_PTZ_MENU_LEFT:
			Buffer[3] = 0x01;
			Buffer[4] = 0;
			Buffer[5] = 0;
			Buffer[6] = 0x00;
			Buffer[7] = 0x08 << 4;
			Buffer[8] = 0xFF;
			break;
		case TTY_PTZ_MENU_RIGHT:
			Buffer[3] = 0x02;
			Buffer[4] = 0;
			Buffer[5] = 0;
			Buffer[6] = 0x00;
			Buffer[7] = 0x08 << 4;
			Buffer[8] = 0xFF;
			break;
		case TTY_PTZ_MENU_OK:
			Buffer[3] = 0x01;
			Buffer[4] = 0;
			Buffer[5] = 0;
			Buffer[6] = 0x00;
			Buffer[7] = 0x08 << 4;
			Buffer[8] = 0xFF;
			break;
		case TTY_PTZ_MENU_CANEL:
			Buffer[3] = 0x02;
			Buffer[4] = 0;
			Buffer[5] = 0;
			Buffer[6] = 0x00;
			Buffer[7] = 0x08 << 4;
			Buffer[8] = 0xFF;
			break;
		default:
			return DLL_ERROR;
	}

	ParityByte(Buffer);
	IFSendData(nChannel, Buffer, 11);

	return DLL_INPUT_OK;
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

void ParityByte(unsigned char Buffer[11])
{
	Buffer[10] = 0x00FF & (0xFFFF - (Buffer[1] + Buffer[2] + Buffer[3] + Buffer[4] + Buffer[5] + Buffer[6] + Buffer[7] + Buffer[8]));
}
