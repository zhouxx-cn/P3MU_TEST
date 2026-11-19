//!\file DevControl.cpp
//!\brief Pelcod_PD先注云台协议实现文件
//! 参考文档 先注云台协议码值表.doc
//!\author yuk@higheasyrd.com
//!\date 2009-9-9 中国安防科技有限公司武汉分公司

#include <stdio.h>
#include "../DevControl.h"

//device type
#define	DEV_TYPE	DRIVER_PTZ

#define	PTZ_FLAGS	(PTZ_FLAG_ZOOM | PTZ_FLAG_FOCUS | PTZ_FLAG_IRIS)

#define	PTZ_AUX_FLAG	(PTZ_FLAG_AUXWIPER | PTZ_FLAG_AUXLIGHT | PTZ_FLAG_AUXAUTO | PTZ_FLAG_AUXPRESET | PTZ_FLAG_AUXSOFTWARECRUISE | PTZ_FLAG_AUXTRACK)

#define  PTZ_TIMER_FLAG	(0)
#define	TIMER_PERIOD		0	//ms

#define	BAUDRATE	(BD9600)

#define	PRESET_COUNT	256
#define	CRUISE_COUNT	16		//巡航数最多为1
#define	TRACK_COUNT		1
#define AUTO_COUNT		1

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

	const int fast = 0x3F;

	const int slow = 0;

	dwSpeed = SPEED(dwSpeed, fast, slow);

	Buffer[0] = 0xff;
	Buffer[1] = dwAddress;

	switch (dwPtzCode)
	{
		case TTY_PTZ_UP_AUTO:
		case TTY_PTZ_UP:
			Buffer[2] = 0;
			Buffer[3] = 0x08;
			Buffer[4] = 0;
			Buffer[5] = dwSpeed;
			break;
		case TTY_PTZ_DOWN_AUTO:
		case TTY_PTZ_DOWN:
			Buffer[2] = 0;
			Buffer[3] = 0x10;
			Buffer[4] = 0;
			Buffer[5] = dwSpeed;
			break;
		case TTY_PTZ_LEFT_AUTO:
		case TTY_PTZ_LEFT:
			Buffer[2] = 0;
			Buffer[3] = 0x04;
			Buffer[4] = dwSpeed;
			Buffer[5] = 0;
			break;
		case TTY_PTZ_RIGHT_AUTO:
		case TTY_PTZ_RIGHT:
			Buffer[2] = 0;
			Buffer[3] = 0x02;
			Buffer[4] = dwSpeed;
			Buffer[5] = 0;
			break;
		case TTY_PTZ_LEFTUP:
		case TTY_PTZ_RIGHTUP:
		case TTY_PTZ_LEFTDOWN:
		case TTY_PTZ_RIGHTDOWN:
		case TTY_PTZ_ZOOM_AUTO:
			return DLL_ERROR;
		case TTY_PTZ_IRIS_AUTO:
			Buffer[2] = 0;
			Buffer[3] = 0x2d;
			Buffer[4] = 0;
			Buffer[5] = 0;
			break;
		case TTY_PTZ_FOCUS_AUTO:
			Buffer[2] = 0;
			Buffer[3] = 0x2b;
			Buffer[4] = 0;
			Buffer[5] = 0;
			break;
		case TTY_PTZ_ZOOMIN:	//变倍小
			Buffer[2] = 0;
			Buffer[3] = 0x20;
			Buffer[4] = 0;
			Buffer[5] = 0;
			break;
		case TTY_PTZ_ZOOMOUT:	//变倍大
			Buffer[2] = 0;
			Buffer[3] = 0x40;
			Buffer[4] = 0;
			Buffer[5] = 0;
			break;

		case TTY_PTZ_IRIS_SMALL:	//光圈小
			Buffer[2] = 0x04;
			Buffer[3] = 0;
			Buffer[4] = 0;
			Buffer[5] = 0;
			break;
		case TTY_PTZ_IRIS_LARGE:	//光圈大
			Buffer[2] = 0x02;
			Buffer[3] = 0;
			Buffer[4] = 0;
			Buffer[5] = 0;
			break;

		case TTY_PTZ_FOCUS_NEAR:	//聚焦
			Buffer[2] = 0x01;
			Buffer[3] = 0;
			Buffer[4] = 0;
			Buffer[5] = 0;
			break;
		case TTY_PTZ_FOCUS_FAR:
			Buffer[2] = 0;
			Buffer[3] = 0x80;
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

//辅助控制
int PtzAuxCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, int bOn)
{
	ASSERT_SENDFN;
	unsigned char Buffer[7];

	Buffer[0] = 0xff;
	if (dwAddress != 0)
		Buffer[1] = dwAddress;
	else
		Buffer[1] = 0;
	switch (dwPtzCode)
	{
		case TTY_AUX_WIPER:
			if (bOn)
			{
				Buffer[2] = 0;
				Buffer[3] = 0x09;
				Buffer[4] = 0;
				Buffer[5] = 0x01;
			}
			else
			{
				Buffer[2] = 0;
				Buffer[3] = 0x0b;
				Buffer[4] = 0;
				Buffer[5] = 0x01;
			}
			break;
		case TTY_AUX_LIGHT:
			if (bOn)
			{
				Buffer[2] = 0;
				Buffer[3] = 0x09;
				Buffer[4] = 0;
				Buffer[5] = 0x02;
			}
			else
			{
				Buffer[2] = 0;
				Buffer[3] = 0x0b;
				Buffer[4] = 0;
				Buffer[5] = 0x02;
			}
			break;
		default:
			return DLL_ERROR;
	}
	Buffer[6] = (Buffer[1] + Buffer[2] + Buffer[3] + Buffer[4] + Buffer[5]) % 256;
	IFSendData(nChannel, Buffer, 7);
	return DLL_OUTPUT_OK;
}



//自动巡航开关，自动扫描操作
int PtzAutoCtrl(unsigned int nChannel,
				unsigned long dwAddress,
				unsigned long dwPtzCode,
				unsigned long dwPresetIndex1,
				unsigned long dwPresetIndex2, unsigned long dwCruiseIndex, unsigned long dwSpeed, unsigned long dwDwellTime, int bScan)
{
	ASSERT_SENDFN;
	unsigned char Buffer[7];

	Buffer[0] = 0xff;
	Buffer[1] = dwAddress;

	switch (dwPtzCode)
	{
		case TTY_PTZ_SETROUTE:
			{
				if (dwCruiseIndex >= CRUISE_COUNT)
				{
					return DLL_ERROR;
				}

				ASSERT_SOFTCRUISEFN;
				if (bScan)
				{
					int ret = IFStartCruise(nChannel, (unsigned int) dwCruiseIndex);

					if (0 == ret)
					{
						return DLL_OUTPUT_OK;
					}
					else
					{
						return DLL_INPUT_OK;
					}
				}
				else
				{
					IFStopCruise(nChannel);
				}
				return DLL_OUTPUT_OK;
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
					Buffer[2] = 0;
					Buffer[3] = 0x23;
					Buffer[4] = 0;
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
		case TTY_PTZ_AUTO:
			if (dwCruiseIndex >= AUTO_COUNT)
			{
				return DLL_ERROR;
			}
			if (bScan)
			{
				Buffer[2] = 0x98;
				Buffer[3] = 0;
				Buffer[4] = 0;
				Buffer[5] = 0;
			}
			else
			{
				Buffer[2] = 0x18;
				Buffer[3] = 0;
				Buffer[4] = 0;
				Buffer[5] = 0;
			}
			break;
		default:
			return DLL_ERROR;
	}
	Buffer[6] = (Buffer[1] + Buffer[2] + Buffer[3] + Buffer[4] + Buffer[5]) % 256;
	IFSendData(nChannel, Buffer, 7);
	return DLL_OUTPUT_OK;
}


//预置点相关操作,巡航，轨迹的设置添加等
int PtzPresetCtrl(unsigned int nChannel,
				  unsigned long dwAddress,
				  unsigned long dwPtzCode, unsigned long dwPresetIndex, unsigned long dwCruiseIndex, unsigned long dwSpeed, unsigned long dwDwellTime)
{
	ASSERT_SENDFN;
	unsigned char Buffer[7];

	Buffer[0] = 0xff;
	Buffer[1] = dwAddress;

	switch (dwPtzCode)
	{
		case TTY_PTZ_PRESETSET:
			Buffer[2] = 0;
			Buffer[3] = 0x03;
			Buffer[4] = 0;
			Buffer[5] = dwPresetIndex + 1;
			break;
		case TTY_PTZ_PRESETMOVE:
			Buffer[2] = 0;
			Buffer[3] = 0x05;
			Buffer[4] = 0;
			Buffer[5] = dwPresetIndex + 1;
			break;
		case TTY_PTZ_PRESETCLRALL:
			return DLL_INPUT_OK;
			break;
		case TTY_PTZ_PRESETCALL:
			Buffer[2] = 0;
			Buffer[3] = 0x07;
			Buffer[4] = 0;
			Buffer[5] = dwPresetIndex + 1;
			break;
		case TTY_PTZ_TRACKSET:
			Buffer[2] = 0;
			Buffer[3] = 0x1f;
			Buffer[4] = 0;
			Buffer[5] = 0;
			break;
		case TTY_PTZ_TRACKEND:
			Buffer[2] = 0;
			Buffer[3] = 0x21;
			Buffer[4] = 0;
			Buffer[5] = 0;
			break;
		case TTY_PTZ_AUTOSET:
		case TTY_PTZ_AUTOEND:
			return DLL_INPUT_OK;
		case TTY_PTZ_ROUTESET:
		case TTY_PTZ_ROUTEEND:
		case TTY_PTZ_ROUTEADDPOINT:
			return DLL_OUTPUT_OK;
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
	//printf("PelcoD SetCallback\n");
	IFSendData = FnSendData;
	IFRecvData = FnRecvData;
	IFStartCruise = FnStartCruise;
	IFStopCruise = FnStopCruise;

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

const char *GetVersion(void)
{
	return s_szVersion;
}
