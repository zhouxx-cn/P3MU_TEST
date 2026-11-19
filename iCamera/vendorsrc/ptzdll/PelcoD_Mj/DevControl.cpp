#include <stdio.h>
#include "../DevControl.h"

//device type
#define	DEV_TYPE	DRIVER_PTZ

#define	PTZ_FLAGS	(PTZ_FLAG_PANTITLT | PTZ_FLAG_ZOOM | PTZ_FLAG_FOCUS | PTZ_FLAG_IRIS)

#define	PTZ_AUX_FLAG	(PTZ_FLAG_AUXPOWER | PTZ_FLAG_AUXTRACK | PTZ_FLAG_AUXPRESET \
						| PTZ_FLAG_AUXCRUISE | PTZ_FLAG_AUXTIMECRUISE | PTZ_FLAG_AUXMENUCTRL)

#define  PTZ_TIMER_FLAG	(0)
#define	TIMER_PERIOD		0	//ms


#define	BAUDRATE	(BD1200|BD2400)

#define	PRESET_COUNT	256
#define	TRACK_COUNT		1		//轨迹最多为1
#define	CRUISE_COUNT	6		//巡航数最多为1
#define AUTO_COUNT      0		//线扫数

#define PROTOCOL_VERSION "0.1"

const char s_szVersion[] = P_NAME " " PROTOCOL_VERSION "." BASEVERSION " BUILD " __DATE__ " " __TIME__;

static CbSendData IFSendData;

static CbRecvData IFRecvData;


//云台普通控制
int PtzNormalCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, unsigned long dwSpeed)
{
	ASSERT_SENDFN;
	char Buffer[7];

	Buffer[0] = 0xFF;
	Buffer[1] = dwAddress;

	Buffer[4] = 0x1F;
	Buffer[5] = 0x1F;
	const int fast = 63;

	const int slow = 0;

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
			break;

		case TTY_PTZ_ZOOMIN:	//\x{53D8}\x{500D}\x{5C0F}
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
		case TTY_PTZ_IRIS_SMALL:	//\x{5149}\x{5708}\x{5C0F}
			Buffer[2] = 0x04;
			Buffer[3] = 0;
			Buffer[4] = 0;
			Buffer[5] = 0;
			break;
		case TTY_PTZ_IRIS_LARGE:	//\x{5149}\x{5708}\x{5927}
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
		case TTY_PTZ_FOCUS_NEAR:	//\x{805A}\x{7126}
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

	//printf("Buffer=%p, 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", Buffer, Buffer[0], Buffer[1], Buffer[2], Buffer[3], Buffer[4], Buffer[5], Buffer[6]);
	IFSendData(nChannel, Buffer, 7);

	return (int) DLL_OUTPUT_OK;
}

int PtzAuxCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, int bOn)
{
	ASSERT_SENDFN;
	char Buffer[7];

	if (!Buffer)
		return DLL_ERROR;
	Buffer[0] = 0xFF;
	Buffer[1] = dwAddress;

	Buffer[4] = 0;
	Buffer[5] = 0;

	switch (dwPtzCode)
	{
		case TTY_AUX_LIGHT:	//
		case TTY_AUX_HEATER:	//
		case TTY_AUX_COOLER:
		case TTY_AUX_WIPER:
			return DLL_ERROR;

		case TTY_AUX_POWER:
			{
				if (bOn)
				{
					Buffer[2] = 0;
					Buffer[3] = 0x53;
					Buffer[4] = 0xe;
					Buffer[5] = 0x1;
				}
				else
				{
					Buffer[2] = 0;
					Buffer[3] = 0x53;
					Buffer[4] = 0xe;
					Buffer[5] = 0x0;
				}
				break;
			}
		default:
			return DLL_ERROR;
	}
	Buffer[6] = (Buffer[1] + Buffer[2] + Buffer[3] + Buffer[4] + Buffer[5]) % 256;

	//printf("Buffer=%p, 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", Buffer, Buffer[0], Buffer[1], Buffer[2], Buffer[3], Buffer[4], Buffer[5], Buffer[6]);
	IFSendData(nChannel, Buffer, 7);

	return DLL_OUTPUT_OK;
}

int PtzAutoCtrl(unsigned int nChannel,
				unsigned long dwAddress,
				unsigned long dwPtzCode,
				unsigned long dwPresetIndex1,
				unsigned long dwPresetIndex2, unsigned long dwCruiseIndex, unsigned long dwSpeed, unsigned long dwDwellTime, int bScan)
{
	char Buffer[7];

	Buffer[0] = 0xFF;
	Buffer[1] = dwAddress;
	Buffer[2] = 0;
	Buffer[4] = 0;

	switch (dwPtzCode)
	{
		case TTY_PTZ_SETROUTE:
			{
				if (dwCruiseIndex >= CRUISE_COUNT)
				{
					return DLL_ERROR;
				}
				if (bScan)
				{
					Buffer[2] = 0x0;
					Buffer[3] = 0x4f;
					Buffer[4] = 1;
					Buffer[5] = dwCruiseIndex;
				}
				else
				{
					Buffer[2] = 0x0;
					Buffer[3] = 0x51;
					Buffer[4] = 0;
					Buffer[5] = 0;
				}
				break;
			}
		case TTY_PTZ_TRACKCALL:
			{
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
			}
		case TTY_PTZ_DWELL:	//
			Buffer[2] = 0x86;
			Buffer[3] = 0x81;
			Buffer[5] = dwDwellTime;
			break;
		case TTY_PTZ_SPEED:
		default:
			return DLL_ERROR;
	}
	Buffer[6] = (Buffer[1] + Buffer[2] + Buffer[3] + Buffer[4] + Buffer[5]) % 256;

	//printf("Buffer=%p, 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", Buffer, Buffer[0], Buffer[1], Buffer[2], Buffer[3], Buffer[4], Buffer[5], Buffer[6]);
	IFSendData(nChannel, Buffer, 7);

	return DLL_OUTPUT_OK;
}

int PtzPresetCtrl(unsigned int nChannel,
				  unsigned long dwAddress,
				  unsigned long dwPtzCode, unsigned long dwPresetIndex, unsigned long dwCruiseIndex, unsigned long dwSpeed, unsigned long dwDwellTime)
{
	ASSERT_SENDFN;
	char Buffer[7];

	Buffer[0] = 0xFF;
	Buffer[1] = dwAddress;

	Buffer[2] = 0;
	Buffer[4] = 0;

	const int fast = 0;

	const int slow = 7;

	dwSpeed = SPEED(dwSpeed, fast, slow);

	//printf ("libPelcop.so with dwPresetIndex = %d, dwCruiseIndex = %d\n", dwPresetIndex, dwCruiseIndex);
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
			Buffer[2] = 0x0;
			Buffer[3] = 0x47;
			Buffer[5] = dwCruiseIndex;	// + 1;
			break;
		case TTY_PTZ_ROUTEADDPOINT:
			Buffer[2] = dwPresetIndex + 1;
			Buffer[3] = 0x49;
			Buffer[4] = dwSpeed;
//cwf 20071228 由巡航线路号改为预置点号
			Buffer[5] = dwDwellTime;
			break;
		case TTY_PTZ_ROUTEEND:
			Buffer[2] = 0x0;
			Buffer[3] = 0x4b;
			Buffer[5] = 0;
			break;
			break;
		case TTY_PTZ_ROUTEREMOVEPOINT:
			Buffer[2] = 0x83;
			Buffer[3] = 0x81;
			Buffer[5] = dwPresetIndex + 1;
			break;
		case TTY_PTZ_ROUTEREMOVE:
			Buffer[2] = 0x0;
			Buffer[3] = 0x4d;
			Buffer[5] = dwCruiseIndex + 1;
			break;
		default:
			return DLL_ERROR;
	}

	Buffer[6] = (Buffer[1] + Buffer[2] + Buffer[3] + Buffer[4] + Buffer[5]) % 256;

	//printf("Buffer=%p, 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", Buffer, Buffer[0], Buffer[1], Buffer[2], Buffer[3], Buffer[4], Buffer[5], Buffer[6]);
	IFSendData(nChannel, Buffer, 7);

	return DLL_OUTPUT_OK;
}

int PtzMenuCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode)
{
	//printf("PtzMenuCtrl, nChannel = %u, dwAddress = %lu, dwPtzCode = %lu\n", nChannel, dwAddress, dwPtzCode);
	switch (dwPtzCode)
	{
		case TTY_PTZ_MENU_ENTER:
			PtzPresetCtrl(nChannel, dwAddress, TTY_PTZ_PRESETCALL, 0, 0, 0, 0);
			PtzPresetCtrl(nChannel, dwAddress, TTY_PTZ_PRESETCALL, 0, 0, 0, 0);
			break;
		case TTY_PTZ_MENU_EXIT:
			return DLL_INPUT_OK;
			break;
		case TTY_PTZ_MENU_LEFT:
			PtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_LEFT, 1);
			PtzDelay(50);
			PtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_LEFT_STOP, 0);
			break;
		case TTY_PTZ_MENU_RIGHT:
			PtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_RIGHT, 1);
			PtzDelay(50);
			PtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_RIGHT_STOP, 0);
			break;
		case TTY_PTZ_MENU_UP:
			PtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_UP, 1);
			PtzDelay(50);
			PtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_UP_STOP, 1);
			break;
		case TTY_PTZ_MENU_DOWN:
			PtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_DOWN, 1);
			PtzDelay(50);
			PtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_DOWN_STOP, 1);
			break;
		case TTY_PTZ_MENU_OK:
			PtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_LEFT, 1);
			PtzDelay(50);
			PtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_LEFT_STOP, 0);
			break;
		case TTY_PTZ_MENU_CANEL:
			//PtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_RIGHT, 1);
			//PtzDelay(50);
			//PtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_RIGHT_STOP, 0);
			return DLL_INPUT_OK;
			break;
		default:
			return DLL_ERROR;
	}

	//本协议不支持菜单操作
	return DLL_OUTPUT_OK;
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

DLL_API const char *GetVersion(void)
{
	return s_szVersion;
}
