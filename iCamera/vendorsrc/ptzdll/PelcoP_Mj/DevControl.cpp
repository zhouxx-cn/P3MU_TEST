#include <stdio.h>
#include "../DevControl.h"

//device type
#define	DEV_TYPE	DRIVER_PTZ

#define	PTZ_FLAGS	(PTZ_FLAG_PANTITLT | PTZ_FLAG_ZOOM | PTZ_FLAG_FOCUS | PTZ_FLAG_IRIS)

#define	PTZ_AUX_FLAG	(PTZ_FLAG_AUXPOWER | PTZ_FLAG_AUXTRACK | PTZ_FLAG_AUXPRESET \
						|PTZ_FLAG_AUXCRUISE | PTZ_FLAG_AUXTIMECRUISE | PTZ_FLAG_AUXMENUCTRL)

#define  PTZ_TIMER_FLAG	(0)
#define	TIMER_PERIOD		0	//ms


#define	BAUDRATE	(BD9600|BD4800|BD2400)

#define	PRESET_COUNT	256
#define	TRACK_COUNT		1		//轨迹最多为3
#define	CRUISE_COUNT	6		//巡航数最多为6
#define AUTO_COUNT      0

#define PROTOCOL_VERSION "0.1"

const char s_szVersion[] = P_NAME " " PROTOCOL_VERSION "." BASEVERSION " BUILD " __DATE__ " " __TIME__;

static CbSendData IFSendData;

static CbRecvData IFRecvData;

//云台普通控制
int PtzNormalCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, unsigned long dwSpeed)
{
	ASSERT_SENDFN;
	char pBuf[8];

	pBuf[0] = 0xA0;
	pBuf[1] = dwAddress;

	pBuf[4] = 0x1F;
	pBuf[5] = 0x1F;
	pBuf[6] = 0xAF;
	const int fast = 63;

	const int slow = 0;

	dwSpeed = SPEED(dwSpeed, fast, slow);

	switch (dwPtzCode)
	{
		case TTY_PTZ_UP:
			pBuf[2] = 0;
			pBuf[3] = 0x08;
			pBuf[5] = dwSpeed;
			break;
		case TTY_PTZ_DOWN:
			pBuf[2] = 0;
			pBuf[3] = 0x10;
			pBuf[5] = dwSpeed;
			break;
		case TTY_PTZ_LEFT:
			pBuf[2] = 0;
			pBuf[3] = 0x04;
			pBuf[4] = dwSpeed;
			break;
		case TTY_PTZ_RIGHT:
			pBuf[2] = 0;
			pBuf[3] = 0x02;
			pBuf[4] = dwSpeed;
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
		case TTY_PTZ_ZOOMIN:	//变倍小
			pBuf[2] = 0;
			pBuf[3] = 0x40;
			pBuf[4] = 0;
			pBuf[5] = 0;
			break;
		case TTY_PTZ_ZOOMOUT:
			pBuf[2] = 0;
			pBuf[3] = 0x20;
			pBuf[4] = 0;
			pBuf[5] = 0;
			break;
		case TTY_PTZ_IRIS_SMALL:	//光圈小
			pBuf[2] = 0x08;
			pBuf[3] = 0;
			pBuf[4] = 0;
			pBuf[5] = 0;
			break;
		case TTY_PTZ_IRIS_LARGE:	//光圈大
			pBuf[2] = 0x04;
			pBuf[3] = 0;
			pBuf[4] = 0;
			pBuf[5] = 0;
			break;
		case TTY_PTZ_FOCUS_NEAR:	//聚焦
			pBuf[2] = 0x02;
			pBuf[3] = 0;
			pBuf[4] = 0;
			pBuf[5] = 0;
			break;
		case TTY_PTZ_FOCUS_FAR:
			pBuf[2] = 0x01;
			pBuf[3] = 0;
			pBuf[4] = 0;
			pBuf[5] = 0;
			break;
		case TTY_PTZ_ZOOM_STOP:
		case TTY_PTZ_IRIS_STOP:
		case TTY_PTZ_FOCUS_STOP:
		case TTY_PTZ_UP_STOP:
		case TTY_PTZ_DOWN_STOP:
		case TTY_PTZ_LEFT_STOP:
		case TTY_PTZ_RIGHT_STOP:
		case TTY_PTZ_STOP:
			pBuf[2] = 0;
			pBuf[3] = 0;
			pBuf[4] = 0;
			pBuf[5] = 0;
			break;
		default:
			return DLL_ERROR;
	}
	pBuf[7] = (pBuf[0] ^ pBuf[1] ^ pBuf[2] ^ pBuf[3] ^ pBuf[4] ^ pBuf[5] ^ pBuf[6]);

	//printf("Buffer=%p, 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", pBuf, pBuf[0], pBuf[1], pBuf[2], pBuf[3], pBuf[4], pBuf[5], pBuf[6]);
	IFSendData(nChannel, pBuf, 8);

	return DLL_OUTPUT_OK;
}

int PtzAuxCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, int bOn)
{
	ASSERT_SENDFN;
	char pBuf[8];

	pBuf[0] = 0xA0;
	pBuf[1] = dwAddress;

	pBuf[4] = 0;
	pBuf[5] = 0x01;
	pBuf[6] = 0xAF;

	switch (dwPtzCode)
	{
		case TTY_AUX_LIGHT:	//
		case TTY_AUX_HEATER:	//
		case TTY_AUX_COOLER:
		case TTY_AUX_WIPER:
			return DLL_ERROR;

		case TTY_AUX_POWER:
			if (bOn)
			{
				pBuf[2] = 0;
				pBuf[3] = 0x53;
				pBuf[4] = 0xe;
				pBuf[5] = 0x1;
			}
			else
			{
				pBuf[2] = 0;
				pBuf[3] = 0x53;
				pBuf[4] = 0xe;
				pBuf[5] = 0x0;
			}
			break;
		default:
			return DLL_ERROR;
	}

	pBuf[7] = (pBuf[0] ^ pBuf[1] ^ pBuf[2] ^ pBuf[3] ^ pBuf[4] ^ pBuf[5] ^ pBuf[6]);

	//printf("Buffer=%p, 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", pBuf, pBuf[0], pBuf[1], pBuf[2], pBuf[3], pBuf[4], pBuf[5], pBuf[6]);
	IFSendData(nChannel, pBuf, 8);

	return DLL_OUTPUT_OK;
}

int PtzAutoCtrl(unsigned int nChannel,
				unsigned long dwAddress,
				unsigned long dwPtzCode,
				unsigned long dwPresetIndex1,
				unsigned long dwPresetIndex2, unsigned long dwCruiseIndex, unsigned long dwSpeed, unsigned long dwDwellTime, int bScan)
{
	ASSERT_SENDFN;
	char pBuf[8];

	pBuf[0] = 0xA0;
	pBuf[1] = dwAddress;
	pBuf[2] = 0;
	pBuf[4] = 0;
	pBuf[6] = 0xAF;

	switch (dwPtzCode)
	{
		case TTY_PTZ_SETROUTE:	//
			if (dwCruiseIndex >= CRUISE_COUNT)
			{
				return DLL_ERROR;
			}
			if (bScan)
			{
				pBuf[2] = 0x0;
				pBuf[3] = 0x4f;
				pBuf[4] = 1;
				pBuf[5] = dwCruiseIndex;
			}
			else
			{
				pBuf[2] = 0x0;
				pBuf[3] = 0x51;
				pBuf[4] = 0;
				pBuf[5] = 0;
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
					pBuf[3] = 0x23;
					pBuf[5] = 0;
				}
				else
				{
					PtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_RIGHT, 1);
					PtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_RIGHT_STOP, 0);
					return DLL_OUTPUT_OK;
				}
				break;
			}
		case TTY_PTZ_DWELL:
			pBuf[2] = 0x86;
			pBuf[3] = 0x81;
			pBuf[5] = dwDwellTime;
			break;
		case TTY_PTZ_SPEED:
		default:
			return DLL_ERROR;
	}

	pBuf[7] = (pBuf[0] ^ pBuf[1] ^ pBuf[2] ^ pBuf[3] ^ pBuf[4] ^ pBuf[5] ^ pBuf[6]);

	//printf("Buffer=%p, 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", pBuf, pBuf[0], pBuf[1], pBuf[2], pBuf[3], pBuf[4], pBuf[5], pBuf[6]);
	IFSendData(nChannel, pBuf, 8);

	return DLL_OUTPUT_OK;
}

int PtzPresetCtrl(unsigned int nChannel,
				  unsigned long dwAddress,
				  unsigned long dwPtzCode, unsigned long dwPresetIndex, unsigned long dwCruiseIndex, unsigned long dwSpeed, unsigned long dwDwellTime)
{
	ASSERT_SENDFN;
	char pBuf[8];

	pBuf[0] = 0xA0;
	pBuf[1] = dwAddress;

	pBuf[2] = 0;
	pBuf[4] = 0;
	pBuf[6] = 0xAF;

	const int fast = 0;

	const int slow = 7;

	dwSpeed = SPEED(dwSpeed, fast, slow);

	switch (dwPtzCode)
	{
		case TTY_PTZ_PRESETSET:
			pBuf[3] = 0x03;
			pBuf[5] = dwPresetIndex + 1;
			break;
		case TTY_PTZ_PRESETCALL:
			pBuf[3] = 0x07;
			pBuf[5] = dwPresetIndex + 1;
			break;
		case TTY_PTZ_PRESETMOVE:
			pBuf[3] = 0x05;
			pBuf[5] = dwPresetIndex + 1;
			break;
		case TTY_PTZ_TRACKSET:
			pBuf[3] = 0x1F;
			pBuf[5] = 0;
			break;
		case TTY_PTZ_TRACKEND:
			pBuf[3] = 0x21;
			pBuf[5] = 0;
			break;
		case TTY_PTZ_ROUTESET:
			pBuf[2] = 0x0;
			pBuf[3] = 0x47;
			pBuf[5] = dwCruiseIndex;
			break;
		case TTY_PTZ_ROUTEADDPOINT:
			pBuf[2] = dwPresetIndex + 1;
			pBuf[3] = 0x49;
			pBuf[4] = dwSpeed;
			pBuf[5] = dwDwellTime;
			break;
		case TTY_PTZ_ROUTEEND:
			pBuf[2] = 0x0;
			pBuf[3] = 0x4b;
			pBuf[5] = 0;
			break;
			break;
		case TTY_PTZ_ROUTEREMOVEPOINT:
			pBuf[2] = 0x83;
			pBuf[3] = 0x81;
			pBuf[5] = dwPresetIndex + 1;
			break;
		case TTY_PTZ_ROUTEREMOVE:
			pBuf[2] = 0x0;
			pBuf[3] = 0x4d;
			pBuf[5] = dwCruiseIndex;
			break;
		default:
			return DLL_ERROR;
	}

	pBuf[7] = (pBuf[0] ^ pBuf[1] ^ pBuf[2] ^ pBuf[3] ^ pBuf[4] ^ pBuf[5] ^ pBuf[6]);

	//printf("Buffer=%p, 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", pBuf, pBuf[0], pBuf[1], pBuf[2], pBuf[3], pBuf[4], pBuf[5], pBuf[6]);
	IFSendData(nChannel, pBuf, 8);

	return DLL_OUTPUT_OK;
}

int PtzMenuCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode)
{
	//printf("PtzMenuCtrl dwPtzCode = %lu, nChannel = %ld\n", dwPtzCode, nChannel);
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
			PtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_LEFT_STOP, 1);
			break;
		case TTY_PTZ_MENU_RIGHT:
			PtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_RIGHT, 1);
			PtzDelay(50);
			PtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_RIGHT_STOP, 1);
			break;
		case TTY_PTZ_MENU_OK:
			PtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_LEFT, 1);
			PtzDelay(50);
			PtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_LEFT_STOP, 1);
			break;
		case TTY_PTZ_MENU_CANEL:
			//PtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_RIGHT, 1); 
			//PtzDelay(50);
			//PtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_RIGHT_STOP, 1);
			return DLL_INPUT_OK;
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
		default:
			return DLL_ERROR;
	}

	//本协议不支持菜单操作
	return DLL_OUTPUT_OK;
}

int SetCallback(CbSendData FnSendData, CbRecvData FnRecvData, CbStartCruise FnStartCruise, CbStopCruise FnStopCruise)
{
	//printf("PelcoD SetCallback\n");
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
