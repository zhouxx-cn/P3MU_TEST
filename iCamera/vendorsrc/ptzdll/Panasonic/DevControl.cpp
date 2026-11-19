//!\file DevControl.cpp
//!\brief Panasonic协议实现文件
//! 参考文档 松下协议.pdf
//!\author yuk@higheasyrd.com
//!\date 2009-9-16 中国安防科技有限公司武汉分公司

#include <stdio.h>
#include "../DevControl.h"
#include <stdlib.h>
#include <string.h>

//device type
#define	DEV_TYPE	DRIVER_PTZ

#define	PTZ_FLAGS	(PTZ_FLAG_PANTITLT|PTZ_FLAG_ZOOM|PTZ_FLAG_FOCUS|PTZ_FLAG_IRIS)

#define	PTZ_AUX_FLAG	(PTZ_FLAG_AUXPANTITLT | PTZ_FLAG_AUXTRACK|PTZ_FLAG_AUXPRESET |PTZ_FLAG_AUXCRUISE)

#define  PTZ_TIMER_FLAG	(0)
#define	TIMER_PERIOD		0	//ms

#define	BAUDRATE	(BD9600|BD4800|BD2400)

#define	PRESET_COUNT	256
#define	TRACK_COUNT		1		//轨迹最多为4
#define	CRUISE_COUNT	1		//巡航数最多为1
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
	char strMode[16];

	char Buffer[256];

	char strCommon[64];

	const int fast = 120;

	int i;

	const int slow = 0;

	dwSpeed = SPEED(dwSpeed, fast, slow);

	switch (dwPtzCode)
	{
		case TTY_PTZ_UP:
			sprintf(strCommon, "%s%02x0\0", "202136A:2022", dwSpeed);
			sprintf(strMode, "%s\0", "GCF");
			break;
		case TTY_PTZ_DOWN:
			sprintf(strCommon, "%s%02x0\0", "202136E:2022", dwSpeed);
			sprintf(strMode, "%s\0", "GCF");
			break;
		case TTY_PTZ_LEFT:
			sprintf(strCommon, "%s%02x0\0", "2021368:2022", dwSpeed);
			sprintf(strMode, "%s\0", "GCF");
			break;
		case TTY_PTZ_RIGHT:
			sprintf(strCommon, "%s%02x0\0", "202136C:2022", dwSpeed);
			sprintf(strMode, "%s\0", "GCF");
			break;
		case TTY_PTZ_UP_AUTO:
		case TTY_PTZ_DOWN_AUTO:
		case TTY_PTZ_LEFT_AUTO:
		case TTY_PTZ_RIGHT_AUTO:
			return DLL_INPUT_OK;
			break;
		case TTY_PTZ_LEFTUP:
			sprintf(strCommon, "%s%02x0\0", "2021369:2022", dwSpeed);
			sprintf(strMode, "%s\0", "GCF");
			break;
		case TTY_PTZ_RIGHTUP:
			sprintf(strCommon, "%s%02x0\0", "202136B:2022", dwSpeed);
			sprintf(strMode, "%s\0", "GCF");
			break;
		case TTY_PTZ_LEFTDOWN:
			sprintf(strCommon, "%s%02x0\0", "202136F:2022", dwSpeed);
			sprintf(strMode, "%s\0", "GCF");
			break;
		case TTY_PTZ_RIGHTDOWN:
			sprintf(strCommon, "%s%02x0\0", "202136D:2022", dwSpeed);
			sprintf(strMode, "%s\0", "GCF");
			break;
		case TTY_PTZ_ZOOMIN:	//变倍小
			sprintf(strCommon, "%s\0", "202122C");
			sprintf(strMode, "%s\0", "GC7");
			break;
		case TTY_PTZ_ZOOMOUT:
			sprintf(strCommon, "%s\0", "2021228");
			sprintf(strMode, "%s\0", "GC7");
			break;
		case TTY_PTZ_IRIS_SMALL:	//光圈小
			sprintf(strCommon, "%s\0", "0021003:2021213");
			sprintf(strMode, "%s\0", "GCF");
			break;
		case TTY_PTZ_IRIS_LARGE:	//光圈大
			sprintf(strCommon, "%s\0", "0021002:2021212");
			sprintf(strMode, "%s\0", "GCF");
			break;
		case TTY_PTZ_FOCUS_NEAR:	//聚焦
			sprintf(strCommon, "%s\0", "202122A");
			sprintf(strMode, "%s\0", "GC7");
			break;
		case TTY_PTZ_FOCUS_FAR:
			sprintf(strCommon, "%s\0", "202122E");
			sprintf(strMode, "%s\0", "GC7");
			break;

		case TTY_PTZ_UP_STOP:
		case TTY_PTZ_DOWN_STOP:
		case TTY_PTZ_LEFT_STOP:
		case TTY_PTZ_RIGHT_STOP:
		case TTY_PTZ_LEFTUP_STOP:
		case TTY_PTZ_LEFTDOWN_STOP:
		case TTY_PTZ_RIGHTUP_STOP:
		case TTY_PTZ_RIGHTDOWN_STOP:
			sprintf(strCommon, "%s\0", "2021324");
			sprintf(strMode, "%s\0", "GC7");
			break;
		case TTY_PTZ_FOCUS_STOP:
		case TTY_PTZ_ZOOM_STOP:
			sprintf(strCommon, "%s\0", "2021224");
			sprintf(strMode, "%s\0", "GC7");
			break;
		case TTY_PTZ_IRIS_STOP:
			sprintf(strCommon, "%s\0", "0021004");
			sprintf(strMode, "%s\0", "GC7");
			break;
		case TTY_PTZ_STOP:
			Buffer[2] = 0;
			Buffer[3] = 0;
			Buffer[4] = 0;
			Buffer[5] = 0;
			return DLL_OUTPUT_OK;
			break;
		default:
			return DLL_ERROR;
	}
	memset(Buffer, 0, sizeof (Buffer));
	sprintf(Buffer, "%cAD%02d;%s:%s%c\0", 2, dwAddress, strMode, strCommon, 3);

	int dwWriteLength = strlen(Buffer);

	IFSendData(nChannel, Buffer, dwWriteLength);
	printf("字符串:%s  ;长度= %d\n", Buffer, dwWriteLength);
	for (i = 0; i < dwWriteLength; i++)
	{
		printf("%c", Buffer[i]);
	}
	printf("\n");

	return DLL_OUTPUT_OK;

}

int PtzAuxCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, int bOn)
{
	unsigned char Buffer[256];

	Buffer[0] = 0xA0;
	Buffer[1] = dwAddress;

	Buffer[4] = 0;
	Buffer[5] = 0x01;
	Buffer[6] = 0xAF;

	switch (dwPtzCode)
	{
		case TTY_AUX_LIGHT:
		case TTY_AUX_HEATER:
		case TTY_AUX_COOLER:
		case TTY_AUX_POWER:
			return DLL_ERROR;

		case TTY_AUX_WIPER:
		default:
			return DLL_ERROR;
	}

	Buffer[7] = (Buffer[0] ^ Buffer[1] ^ Buffer[2] ^ Buffer[3] ^ Buffer[4] ^ Buffer[5] ^ Buffer[6]);
	return DLL_OUTPUT_OK;
}

int PtzAutoCtrl(unsigned int nChannel,
				unsigned long dwAddress,
				unsigned long dwPtzCode,
				unsigned long dwPresetIndex1,
				unsigned long dwPresetIndex2, unsigned long dwCruiseIndex, unsigned long dwSpeed, unsigned long dwDwellTime, int bScan)
{
	char strMode[16];

	char Buffer[256];

	char strCommon[64];

	const int fast = 120;

	int i;

	const int slow = 0;

	dwSpeed = SPEED(dwSpeed, fast, slow);

	switch (dwPtzCode)
	{

		case TTY_PTZ_SETROUTE:	//
			if (bScan)
			{
				//test
				sprintf(strCommon, "%s\0", "2021400:2022330");
				sprintf(strMode, "%s\0", "GCF");

				sprintf(strCommon, "%s\0", "2021437");
				sprintf(strMode, "%s\0", "GC7");

				sprintf(strCommon, "%s\0", "2021438");
				sprintf(strMode, "%s\0", "GC7");
				//auto
				sprintf(strCommon, "%s%02x0\0", "2021300:2022", dwSpeed);
				sprintf(strMode, "%s\0", "GCF");

				//seq speed
				sprintf(strCommon, "%s%02x0\0", "2021437:2022", dwSpeed);
				sprintf(strMode, "%s\0", "GCF");

			}
			else
			{
				//test
				sprintf(strCommon, "%s\0", "2021324");
				sprintf(strMode, "%s\0", "GC7");

				sprintf(strCommon, "%s\0", "2021431");
				sprintf(strMode, "%s\0", "GC7");

				sprintf(strCommon, "%s\0", "2021431");
				sprintf(strMode, "%s\0", "GC7");
				//auto
				sprintf(strCommon, "%s\0", "2021301");
				sprintf(strMode, "%s\0", "GC7");
				//seq speed
				sprintf(strCommon, "%s\0", "2021431");
				sprintf(strMode, "%s\0", "GC7");
			}
			break;
		case TTY_PTZ_TRACKCALL:
			//test
			if (bScan)
			{
				sprintf(strCommon, "%s%x\0", "202149", 5 + dwCruiseIndex);
				sprintf(strMode, "%s\0", "GC7");

				sprintf(strCommon, "%s\0", "00219F0:0022A40");
				sprintf(strMode, "%s\0", "GCF");
				sprintf(strCommon, "%s%x0\0", "00219F0:0022C", 1 + dwCruiseIndex);
				sprintf(strMode, "%s\0", "GCF");

				int id = 4 + dwCruiseIndex;

				sprintf(strCommon, "%s\0", "2021490");
				sprintf(strMode, "%s\0", "GC7");
			}
			else
			{
				sprintf(strCommon, "%s\0", "2021491");
				sprintf(strMode, "%s\0", "GC7");
			}
			break;
		case TTY_PTZ_DWELL:
			break;
		case TTY_PTZ_SPEED:
		default:
			return DLL_ERROR;
	}

	sprintf(Buffer, "%cAD%02d;%s:%s%c\0", 2, dwAddress, strMode, strCommon, 3);

	int dwWriteLength = strlen(Buffer);	//strlen(strTemp) -1;

	IFSendData(nChannel, Buffer, dwWriteLength);

	printf("字符串:%s  ;长度= %d\n", Buffer, dwWriteLength);
	for (i = 0; i < strlen(Buffer); i++)
	{
		printf("%c", Buffer[i]);
	}
	printf("\n");

	return DLL_OUTPUT_OK;;
}

int PtzPresetCtrl(unsigned int nChannel,
				  unsigned long dwAddress,
				  unsigned long dwPtzCode, unsigned long dwPresetIndex, unsigned long dwCruiseIndex, unsigned long dwSpeed, unsigned long dwDwellTime)
{
	char strMode[16];

	char Buffer[256];

	char strCommon[64];

	const int fast = 64;

	int i;

	//printf ("libPelcop.so with dwPresetIndex = %d, dwCruiseIndex = %d\n", dwPresetIndex, dwCruiseIndex);
	switch (dwPtzCode)
	{
		case TTY_PTZ_PRESETSET:	//
			//test
			sprintf(strCommon, "%s\0", "9030066");
			sprintf(strMode, "%s\0", "GC7");

			sprintf(strCommon, "%s%x0\0", "00219F0:0022", dwPresetIndex + 100);
			sprintf(strMode, "%s\0", "GCF");
			break;
		case TTY_PTZ_PRESETCALL:	//
			sprintf(strCommon, "%s%02x0\0", "2021400:2022", dwPresetIndex);
			sprintf(strMode, "%s\0", "GCF");
			break;
		case TTY_PTZ_TRACKSET:
			//test
			sprintf(strCommon, "%s%x\0", "202149", 8 + dwCruiseIndex);
			sprintf(strMode, "%s\0", "GC7");
			sprintf(strCommon, "%s\0", "00219F0:0022A60");
			sprintf(strMode, "%s\0", "GCF");

			sprintf(strCommon, "%s%x0\0", "00219F0:0022C", 4 + dwCruiseIndex);
			sprintf(strMode, "%s\0", "GCF");
			//hk
			//int id = 4 + dwCruiseIndex;   //2021494

			sprintf(strCommon, "%s\0", "2021494");
			sprintf(strMode, "%s\0", "GC7");
			break;
		case TTY_PTZ_TRACKEND:
			//test
			sprintf(strCommon, "%s\0", "00219F0:0022A50");
			sprintf(strMode, "%s\0", "GCF");

			sprintf(strCommon, "%s\0", "2021491");
			sprintf(strMode, "%s\0", "GC7");
			break;
		case TTY_PTZ_ROUTESET:
		case TTY_PTZ_ROUTEADDPOINT:
		case TTY_PTZ_ROUTEEND:
		case TTY_PTZ_ROUTEREMOVEPOINT:
		case TTY_PTZ_ROUTEREMOVE:
		case TTY_PTZ_PRESETMOVE:
			return DLL_INPUT_OK;
			break;
		default:
			return DLL_ERROR;
	}

	sprintf(Buffer, "%cAD%02d;%s:%s%c\0", 2, dwAddress, strMode, strCommon, 3);

	int dwWriteLength = strlen(Buffer);	//strlen(strTemp) -1;

	IFSendData(nChannel, Buffer, dwWriteLength);

	printf("字符串:%s  ;长度= %d\n", Buffer, dwWriteLength);
	for (i = 0; i < strlen(Buffer); i++)
	{
		printf("%c", Buffer[i]);
	}
	printf("\n");

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
