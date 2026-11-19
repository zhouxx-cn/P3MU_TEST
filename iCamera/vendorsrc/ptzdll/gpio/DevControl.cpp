#include <stdio.h>
#include "../DevControl.h"
#include "motoLib.h"

#ifdef __cplusplus
extern "C" {
#endif

#define	DEV_TYPE		DRIVER_PTZ

#define	PTZ_FLAGS		(PTZ_FLAG_PANTITLT|PTZ_FLAG_ZOOM|PTZ_FLAG_FOCUS|PTZ_FLAG_IRIS|PTZ_FLAG_RecvData)

#define	PTZ_AUX_FLAG	(PTZ_FLAG_AUXLIGHT|PTZ_FLAG_AUXWIPER|PTZ_FLAG_AUXTRACK |PTZ_FLAG_AUXPRESET | PTZ_FLAG_AUXSOFTWARECRUISE | PTZ_FLAG_AUXELETRONIC)

#define	PTZ_TIMER_FLAG	0
#define	TIMER_PERIOD	0		//ms


#define	BAUDRATE		(BD1200|BD2400)

#define	PRESET_COUNT	256
#define	TRACK_COUNT		1		//轨迹最多为16
#define	CRUISE_COUNT	16		//巡航数最多为16
#define AUTO_COUNT      0		//线扫数为0

#define PROTOCOL_VERSION "0.1"

const char s_szVersion[] = P_NAME " " PROTOCOL_VERSION "." BASEVERSION " BUILD " DATETIME;

static CbSendData IFSendData;

static CbRecvData IFRecvData;

static CbStartCruise IFStartCruise;

static CbStopCruise IFStopCruise;	//##ModelId=49A36CC30148

#if  defined (__MUSL__)
int GpioPtzNormalCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, unsigned long dwSpeed)
#else
int PtzNormalCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, unsigned long dwSpeed)
#endif
{
	//ASSERT_SENDFN;
	//printf("PtzNormalCtrl, nChannel = %lu, dwAddress = %lu, dwPtzCode = %lu, dwSpeed = %lu\n", nChannel, dwAddress, dwPtzCode, dwSpeed);
	unsigned char Buffer[7];

	// TODO: Add your specialized code here.

	Buffer[0] = 0xFF;
	Buffer[1] = dwAddress;

	Buffer[4] = 0x1F;
	Buffer[5] = 0x1F;
	const int nFastSpeed = 63;
	const int nSlowSpeed = 0;
	int nSteps = 0;

	if((TTY_PTZ_UP == dwPtzCode) || (TTY_PTZ_DOWN == dwPtzCode) 
		|| (TTY_PTZ_LEFT == dwPtzCode) || (TTY_PTZ_RIGHT == dwPtzCode))
	{	
		nSteps = SPEED(dwSpeed, nFastSpeed, nSlowSpeed);
	}
	else
	{
		nSteps = dwSpeed;
	}
	
	switch (dwPtzCode)
	{
		case TTY_PTZ_UP:
			MOTO_DropDown(nSteps,nChannel);	// 上下反向了，所以调换一下
			break;
		case TTY_PTZ_DOWN:
			MOTO_DropUp(nSteps,nChannel);	// 上下反向了，所以调换一下
			break;
		case TTY_PTZ_LEFT:
			MOTO_DropLeft(nSteps,nChannel);
			break;
		case TTY_PTZ_RIGHT:
			MOTO_DropRight(nSteps,nChannel);
			break;	
		case TTY_PTZ_LEFT_ONESTEP:
			MOTO_Left(nSteps,nChannel);
			break;
		case TTY_PTZ_RIGHT_ONESTEP:
			MOTO_Right(nSteps,nChannel);
			break;
		case TTY_PTZ_DOWN_ONESTEP:
			MOTO_Down(nSteps,nChannel);
			break;
	    case TTY_PTZ_UP_ONESTEP:
			MOTO_Up(nSteps,nChannel);
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
			Buffer[3] = 0x20; // 2016年3月9日 11:03:11 使景物放大,即将景物推近
			Buffer[4] = 0;
			Buffer[5] = 0;
			break;
		case TTY_PTZ_ZOOMOUT: // 2016年3月9日 11:03:11 使景物缩小,即将景物拉远
			Buffer[2] = 0;
			Buffer[3] = 0x40;
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
			Buffer[2] = 0x01;//0x00;
			Buffer[3] = 0;//0x80;
			Buffer[4] = 0;
			Buffer[5] = 0;
			break;
		case TTY_PTZ_FOCUS_NEAR:
			Buffer[2] = 0x00;//0x01;
			Buffer[3] = 0x80;//0;
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
			MOTO_Stop(nChannel);
			break;
		default:
			return DLL_ERROR;
	}

	Buffer[6] = (Buffer[1] + Buffer[2] + Buffer[3] + Buffer[4] + Buffer[5]) % 256;
	//IFSendData(nChannel, Buffer, 7);

	return (int) DLL_OUTPUT_OK;
}

#if  defined (__MUSL__)
int GpioPtzAuxCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, int bOn)
#else
int PtzAuxCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, int bOn)
#endif
{
	ASSERT_SENDFN;
	//printf("PtzAuxCtrl, nChannel = %lu, dwAddress = %lu, dwPtzCode = %lu, bOn = %d\n", nChannel, dwAddress, dwPtzCode, bOn);
	unsigned char Buffer[7];

	Buffer[0] = 0xFF;
	Buffer[1] = dwAddress;
	Buffer[4] = 0;
	Buffer[5] = 0;

	switch (dwPtzCode)
	{
		case TTY_AUX_HEATER:
		case TTY_AUX_COOLER:
		case TTY_AUX_POWER:
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
	//printf("Buffer=%p, 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", Buffer, Buffer[0], Buffer[1], Buffer[2], Buffer[3], Buffer[4], Buffer[5], Buffer[6]);
	//IFSendData(nChannel, Buffer, 7);

	return DLL_OUTPUT_OK;
}

#if  defined (__MUSL__)
int GpioPtzAutoCtrl(unsigned int nChannel,
				unsigned long dwAddress,
				unsigned long dwPtzCode,
				unsigned long dwPresetIndex1,
				unsigned long dwPresetIndex2, unsigned long dwCruiseIndex, unsigned long dwSpeed, unsigned long dwDwellTime, int bScan)
#else
int PtzAutoCtrl(unsigned int nChannel,
				unsigned long dwAddress,
				unsigned long dwPtzCode,
				unsigned long dwPresetIndex1,
				unsigned long dwPresetIndex2, unsigned long dwCruiseIndex, unsigned long dwSpeed, unsigned long dwDwellTime, int bScan)	
#endif
{
	ASSERT_SENDFN;
	//printf("PtzAutoCtrl, nChannel = %lu, dwAddress = %lu, dwPtzCode = %lu, dwCruiseIndex = %lu, bScan = %d\n", nChannel, dwAddress, dwPtzCode, dwCruiseIndex, bScan);
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
				ASSERT_SOFTCRUISEFN;
				if (bScan)
				{
					int ret = 0;//IFStartCruise(nChannel, (unsigned int) dwCruiseIndex);

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
					//IFStopCruise(nChannel);
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
					Buffer[3] = 0x23;
					Buffer[5] = 0;
				}
				else
				{
#if  defined (__MUSL__)
					GpioPtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_RIGHT, 1);
					GpioPtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_RIGHT_STOP, 0);
#else
					PtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_RIGHT, 1);
					PtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_RIGHT_STOP, 0);
#endif
					return DLL_OUTPUT_OK;
				}
				break;
			}
		default:
			return DLL_ERROR;
	}

	Buffer[6] = (Buffer[1] + Buffer[2] + Buffer[3] + Buffer[4] + Buffer[5]) % 256;
	//printf("Buffer=%p, 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", Buffer, Buffer[0], Buffer[1], Buffer[2], Buffer[3], Buffer[4], Buffer[5], Buffer[6]);
	//IFSendData(nChannel, Buffer, 7);

	return DLL_OUTPUT_OK;
}

#if  defined (__MUSL__)
int GpioPtzPresetCtrl(unsigned int nChannel,
				  unsigned long dwAddress,
				  unsigned long dwPtzCode, unsigned long dwPresetIndex, unsigned long dwCruiseIndex, unsigned long dwSpeed, unsigned long dwDwellTime)
#else
int PtzPresetCtrl(unsigned int nChannel,
				  unsigned long dwAddress,
				  unsigned long dwPtzCode, unsigned long dwPresetIndex, unsigned long dwCruiseIndex, unsigned long dwSpeed, unsigned long dwDwellTime)				  
#endif
{

	switch (dwPtzCode)
	{
		case TTY_PTZ_PRESETSET:
			MOTO_SetPreset(dwPresetIndex);
			break;
		case TTY_PTZ_PRESETCALL:
			MOTO_CallPreset(dwPresetIndex);
			break;
		case TTY_PTZ_PRESETMOVE:
			MOTO_RemovePreset(dwPresetIndex);
			break;
		case TTY_PTZ_TRACKSET:

			break;
		case TTY_PTZ_TRACKEND:

			break;
		case TTY_PTZ_ROUTESET:
		case TTY_PTZ_ROUTEADDPOINT:
		case TTY_PTZ_ROUTEREMOVEPOINT:
		case TTY_PTZ_ROUTEREMOVE:
		case TTY_PTZ_ROUTEEND:
			return DLL_OUTPUT_OK;
		default:
			return DLL_ERROR;
	}

	return DLL_OUTPUT_OK;
}

#if  defined (__MUSL__)
int GpioPtzMenuCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode)
#else
int PtzMenuCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode)
#endif
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

#if  defined (__MUSL__)
int GpioGetEletronicValue(unsigned int nChannel, unsigned long dwAddress, int nValueIndex, int *Value1, int *Value2)
#else
int GetEletronicValue(unsigned int nChannel, unsigned long dwAddress, int nValueIndex, int *Value1, int *Value2)
#endif
{
	int ret = 0;

	switch (nValueIndex)
	{
		case eStateType:
			
			break;
		case eStateTemperature:
			
			break;
		case eStateCoordinate:
			if (0 == MOTO_GetPos(Value1, Value2))
			{
				return DLL_OUTPUT_OK;
			}
			break;
		case eState485Com:
			
			break;

		default:
			return DLL_ERROR;
			break;
	}

	return DLL_ERROR;
}
#if  defined (__MUSL__)
int GpioSetEletronicValue(unsigned int nChannel, unsigned long dwAddress, int nValueIndex, int Value1, int Value2)
#else
int SetEletronicValue(unsigned int nChannel, unsigned long dwAddress, int nValueIndex, int Value1, int Value2)
#endif
{
	return DLL_OUTPUT_OK;
}

#if  defined (__MUSL__)
int GpioSetCallback(CbSendData FnSendData, CbRecvData FnRecvData, CbStartCruise FnStartCruise, CbStopCruise FnStopCruise)
#else
int SetCallback(CbSendData FnSendData, CbRecvData FnRecvData, CbStartCruise FnStartCruise, CbStopCruise FnStopCruise)
#endif
{
	//IFSendData = FnSendData;
	//IFRecvData = FnRecvData;
	//IFStartCruise = FnStartCruise;
	//IFStopCruise = FnStopCruise;
	return 0;
}

#if  defined (__MUSL__)
int GpioHasFunction(int nPtzFlag)
#else
int HasFunction(int nPtzFlag)
#endif
{
	return (int) ((((PTZ_FLAGS | PTZ_AUX_FLAG) & nPtzFlag) != 0) ? 1 : 0);
}

#if  defined (__MUSL__)
void GpioGetTechnicalData(int *PresetCount, int *CruiseCount, int *TrackCount, int *AutoCount, unsigned long *Baudrate)
#else
void GetTechnicalData(int *PresetCount, int *CruiseCount, int *TrackCount, int *AutoCount, unsigned long *Baudrate)
#endif
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

#if  defined (__MUSL__)
const char *GpioGetVersion()
#else
const char *GetVersion()
#endif
{
	return s_szVersion;
}

#ifdef __cplusplus
}
#endif
