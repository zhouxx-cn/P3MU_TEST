#include <stdio.h>
#include <unistd.h>
#include "../DevControl.h"

#define	DEV_TYPE		DRIVER_PTZ

#define	PTZ_FLAGS		(PTZ_FLAG_PANTITLT|PTZ_FLAG_ZOOM|PTZ_FLAG_FOCUS|PTZ_FLAG_IRIS)

#define	PTZ_AUX_FLAG	(PTZ_FLAG_AUXLIGHT|PTZ_FLAG_AUXWIPER|PTZ_FLAG_AUXTRACK |PTZ_FLAG_AUXPRESET | PTZ_FLAG_AUXCRUISE | PTZ_FLAG_AUXAUTO | PTZ_FLAG_AUXELETRONIC)

#define	PTZ_TIMER_FLAG	0
#define	TIMER_PERIOD	0		//ms


#define	BAUDRATE		(BD9600)

#define	PRESET_COUNT	250
#define	TRACK_COUNT		4		//轨迹最多为16
#define	CRUISE_COUNT	6		//巡航数最多为16
#define AUTO_COUNT      4		//线扫数为0

#define PROTOCOL_VERSION "0.1"

const char s_szVersion[] = P_NAME " " PROTOCOL_VERSION "." BASEVERSION " BUILD " __DATE__ " " __TIME__;

static CbSendData IFSendData;

static CbRecvData IFRecvData;

static CbStartCruise IFStartCruise;

static CbStopCruise IFStopCruise;	//##ModelId=49A36CC30148

int RecvMsg(unsigned int nChannel, char *pBuf, int nLen)
{
	int nMsgLen = 0;

	int nRecvTimes = 0;

	int bFindHead = 0;

	int nNeed = 0;

	int i;

	while (nRecvTimes++ < 10)
	{
		//读取消息
		int nRecv = IFRecvData(nChannel, pBuf + nMsgLen, nLen);

		if (nRecv < 0)
		{
			//读取失败，直接退出
			break;
		}
		else if (nRecv == 0)
		{
			continue;
		}

		nMsgLen += nRecv;
		nLen -= nRecv;

		//检查0xEA
		if ((!bFindHead) && (nMsgLen >= 5))
		{
			//查找消息头
			int i;

			//空消息EA D5 00 FF CS
			for (i = 0; i < (nMsgLen - 3); ++i)
			{
				if ((pBuf[i] == 0xEA) && (pBuf[i + 1] == 0xD5) && (pBuf[i + 2] == (~pBuf[i + 3])))
				{
					bFindHead = 1;
					nNeed = pBuf[i + 2];

					//移动消息
					if (i > 0)
					{
						int j = 0, move = i;

						printf("PTZ lose %d bytes\n", move);

						while (i < nMsgLen)
						{
							pBuf[j++] = pBuf[i++];
						}

						nMsgLen -= move;
					}

					if (nLen > (nNeed + 5 - nMsgLen))
					{
						nLen = nNeed + 5 - nMsgLen;
					}
				}
			}
		}

		if (bFindHead && (nMsgLen >= (nNeed + 5)))
		{
			//消息已经完整，退出
			break;
		}
	}

	return nMsgLen;
}

int PtzNormalCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, unsigned long dwSpeed)
{
	ASSERT_SENDFN;
	//printf("PtzNormalCtrl, nChannel = %lu, dwAddress = %lu, dwPtzCode = %lu, dwSpeed = %lu\n", nChannel, dwAddress, dwPtzCode, dwSpeed);
	unsigned char Buffer[11];

	// TODO: Add your specialized code here.

	//协议头
	Buffer[0] = 0xEA;
	Buffer[1] = 0xD5;
	// data 长度为6
	Buffer[2] = 0x06;
	Buffer[3] = ~Buffer[2];
	//固定为1
	Buffer[4] = 0x01;
	// 5,6表示动作
	Buffer[5] = 0x00;
	Buffer[6] = 0x00;
	// 7,8,9表示速度
	Buffer[7] = 0x00;
	Buffer[8] = 0x00;
	Buffer[9] = 0x00;
	Buffer[10] = 0x00;
	const int nFastSpeed = 63;

	const int nSlowSpeed = 0;

	dwSpeed = SPEED(dwSpeed, nFastSpeed, nSlowSpeed);

	switch (dwPtzCode)
	{
		case TTY_PTZ_UP:
			Buffer[6] = 1 << 3;
			Buffer[8] = dwSpeed;
			break;
		case TTY_PTZ_DOWN:
			Buffer[6] = 1 << 4;
			Buffer[8] = dwSpeed;
			break;
		case TTY_PTZ_LEFT:
			Buffer[6] = 1 << 2;
			Buffer[7] = dwSpeed;
			break;
		case TTY_PTZ_RIGHT:
			Buffer[6] = 1 << 1;
			Buffer[7] = dwSpeed;
			break;
		case TTY_PTZ_LEFTUP:
			Buffer[6] = (1 << 3) | (1 << 2);
			Buffer[7] = dwSpeed;
			Buffer[8] = dwSpeed;
			break;
		case TTY_PTZ_RIGHTUP:
			Buffer[6] = (1 << 3) | (1 << 1);
			Buffer[7] = dwSpeed;
			Buffer[8] = dwSpeed;
			break;
		case TTY_PTZ_LEFTDOWN:
			Buffer[6] = (1 << 4) | (1 << 2);
			Buffer[7] = dwSpeed;
			Buffer[8] = dwSpeed;
			break;
		case TTY_PTZ_RIGHTDOWN:
			Buffer[6] = (1 << 4) | (1 << 1);
			Buffer[7] = dwSpeed;
			Buffer[8] = dwSpeed;
			break;
		case TTY_PTZ_UP_AUTO:
		case TTY_PTZ_DOWN_AUTO:
		case TTY_PTZ_LEFT_AUTO:
		case TTY_PTZ_RIGHT_AUTO:
			return DLL_INPUT_OK;

		case TTY_PTZ_ZOOMIN:
			dwSpeed = SPEED(dwSpeed, 7, 0);
			Buffer[6] = 0x40;
			Buffer[9] = dwSpeed;
			break;
		case TTY_PTZ_ZOOMOUT:
			dwSpeed = SPEED(dwSpeed, 7, 0);
			Buffer[6] = 0x20;
			Buffer[9] = dwSpeed;
			break;
		case TTY_PTZ_IRIS_SMALL:
			Buffer[5] = 1 << 3;
			break;
		case TTY_PTZ_IRIS_LARGE:
			Buffer[5] = 1 << 2;
			break;
		case TTY_PTZ_FOCUS_FAR:
			Buffer[5] = 1 << 0;
			break;
		case TTY_PTZ_FOCUS_NEAR:
			Buffer[5] = 1 << 1;
			break;
		case TTY_PTZ_ZOOM_STOP:
		case TTY_PTZ_IRIS_STOP:
		case TTY_PTZ_FOCUS_STOP:
		case TTY_PTZ_UP_STOP:
		case TTY_PTZ_DOWN_STOP:
		case TTY_PTZ_LEFT_STOP:
		case TTY_PTZ_RIGHT_STOP:
		case TTY_PTZ_STOP:
			Buffer[5] = 0;
			break;
		default:
			return DLL_ERROR;
	}

	Buffer[10] = (Buffer[4] ^ Buffer[5] ^ Buffer[6] ^ Buffer[7] ^ Buffer[8] ^ Buffer[9]);
	IFSendData(nChannel, Buffer, 11);


	return (int) DLL_OUTPUT_OK;
}

int PtzAuxCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, int bOn)
{
	ASSERT_SENDFN;
	//printf("PtzAuxCtrl, nChannel = %lu, dwAddress = %lu, dwPtzCode = %lu, bOn = %d\n", nChannel, dwAddress, dwPtzCode, bOn);
	char Buffer[10];

	//协议头
	Buffer[0] = 0xEA;
	Buffer[1] = 0xD5;
	// data 长度为5
	Buffer[2] = 0x05;
	Buffer[3] = ~Buffer[2];
	// 固定为1
	Buffer[4] = 0x01;
	// 固定为0
	Buffer[5] = 0x00;
	// 表示动作
	Buffer[6] = 0x71;
	// 固定为0
	Buffer[7] = 0x00;
	// 预置点号
	Buffer[8] = 0x00;
	// 校验码
	Buffer[9] = 0x00;

	switch (dwPtzCode)
	{
		case TTY_AUX_HEATER:
		case TTY_AUX_COOLER:
		case TTY_AUX_POWER:
			return DLL_ERROR;

		case TTY_AUX_WIPER:
			Buffer[7] = 61;
			Buffer[8] = bOn ? 2 : 3;
			break;

		case TTY_AUX_LIGHT:
			Buffer[7] = 62;
			Buffer[8] = bOn ? 3 : 4;
			break;

		default:
			return DLL_ERROR;
			break;
	}

	Buffer[9] = (Buffer[4] ^ Buffer[5] ^ Buffer[6] ^ Buffer[7] ^ Buffer[8]);
	//printf("Buffer=%p, 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", Buffer, Buffer[0], Buffer[1], Buffer[2], Buffer[3], Buffer[4], Buffer[5], Buffer[6]);
	IFSendData(nChannel, Buffer, 10);

	return DLL_OUTPUT_OK;
}

int PtzAutoCtrl(unsigned int nChannel,
				unsigned long dwAddress,
				unsigned long dwPtzCode,
				unsigned long dwPresetIndex1,
				unsigned long dwPresetIndex2, unsigned long dwCruiseIndex, unsigned long dwSpeed, unsigned long dwDwellTime, int bScan)
{
	ASSERT_SENDFN;
	//printf("PtzAutoCtrl, nChannel = %lu, dwAddress = %lu, dwPtzCode = %lu, dwCruiseIndex = %lu, bScan = %d\n", nChannel, dwAddress, dwPtzCode, dwCruiseIndex, bScan);
	char Buffer[10];

	//协议头
	Buffer[0] = 0xEA;
	Buffer[1] = 0xD5;
	// data 长度为5
	Buffer[2] = 0x05;
	Buffer[3] = ~Buffer[2];
	//固定为1
	Buffer[4] = 0x01;
	// 固定为0
	Buffer[5] = 0x00;
	// 动作编码
	Buffer[6] = 0x00;
	// 固定为0
	Buffer[7] = 0x00;
	// 序列号
	Buffer[8] = 0x00;
	// 校验码
	Buffer[9] = 0x00;

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
					Buffer[6] = 0x29;
				}
				else
				{
					Buffer[6] = 0x2B;
				}
				Buffer[8] = dwCruiseIndex + 1;
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
					Buffer[6] = 0x15;
				}
				else
				{
					PtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_RIGHT, 1);
					PtzNormalCtrl(nChannel, dwAddress, TTY_PTZ_RIGHT_STOP, 0);
					return DLL_OUTPUT_OK;
				}
				Buffer[8] = dwCruiseIndex + 1;
				break;
			}
		case TTY_PTZ_AUTO:
			if (dwCruiseIndex >= AUTO_COUNT)
			{
				return DLL_ERROR;
			}

			if (bScan)
			{
				Buffer[6] = 0x35;
			}
			else
			{
				Buffer[6] = 0x37;
			}
			Buffer[8] = dwCruiseIndex + 1;
			break;
		default:
			return DLL_ERROR;
	}

	Buffer[9] = (Buffer[4] ^ Buffer[5] ^ Buffer[6] ^ Buffer[7] ^ Buffer[8]);
	//printf("Buffer=%p, 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", Buffer, Buffer[0], Buffer[1], Buffer[2], Buffer[3], Buffer[4], Buffer[5], Buffer[6]);
	IFSendData(nChannel, Buffer, 10);

	return DLL_OUTPUT_OK;
}

int PtzPresetCtrl(unsigned int nChannel,
				  unsigned long dwAddress,
				  unsigned long dwPtzCode, unsigned long dwPresetIndex, unsigned long dwCruiseIndex, unsigned long dwSpeed, unsigned long dwDwellTime)
{
	ASSERT_SENDFN;
	//printf("PtzPresetCtrl, nChannel = %lu, dwAddress = %lu, dwPtzCode = %lu, dwPresetIndex = %lu, dwCruiseIndex = %lu\n", nChannel, dwAddress, dwPtzCode, dwPresetIndex, dwCruiseIndex);
	char Buffer[10];

	//协议头
	Buffer[0] = 0xEA;
	Buffer[1] = 0xD5;
	// data 长度为5
	Buffer[2] = 0x05;
	Buffer[3] = ~Buffer[2];
	// 固定为1
	Buffer[4] = 0x01;
	// 固定为0
	Buffer[5] = 0x00;
	// 表示动作
	Buffer[6] = 0x00;
	// 固定为0
	Buffer[7] = 0x00;
	// 预置点号
	Buffer[8] = 0x00;
	// 校验码
	Buffer[9] = 0x00;

	switch (dwPtzCode)
	{
		case TTY_PTZ_PRESETSET:
			Buffer[6] = 0x03;
			Buffer[8] = dwPresetIndex + 1;
			break;
		case TTY_PTZ_PRESETCALL:
			Buffer[6] = 0x07;
			Buffer[8] = dwPresetIndex + 1;
			break;
		case TTY_PTZ_PRESETMOVE:
			Buffer[6] = 0x05;
			Buffer[8] = dwPresetIndex + 1;
			break;
		case TTY_PTZ_TRACKSET:
			Buffer[6] = 0x11;
			Buffer[8] = dwCruiseIndex + 1;
			break;
		case TTY_PTZ_TRACKEND:
			Buffer[6] = 0x13;
			Buffer[8] = dwCruiseIndex + 1;
			break;
		case TTY_PTZ_ROUTESET:
			Buffer[6] = 0x21;
			Buffer[8] = dwCruiseIndex + 1;
			break;
		case TTY_PTZ_ROUTEADDPOINT:
			Buffer[6] = 0x23;
			Buffer[7] = dwPresetIndex + 1;
			Buffer[8] = dwDwellTime;
			break;
		case TTY_PTZ_ROUTEREMOVEPOINT:
			return DLL_OUTPUT_OK;
			break;
		case TTY_PTZ_ROUTEREMOVE:
			Buffer[6] = 0x27;
			Buffer[7] = dwPresetIndex + 1;
			break;
		case TTY_PTZ_ROUTEEND:
			Buffer[6] = 0x25;
			Buffer[8] = dwCruiseIndex + 1;
			break;
		case TTY_PTZ_AUTOSET:
			Buffer[6] = 0x31;
			Buffer[8] = dwCruiseIndex + 1;
			break;
		case TTY_PTZ_AUTOEND:
			Buffer[6] = 0x33;
			Buffer[8] = dwCruiseIndex + 1;
			break;
		default:
			return DLL_ERROR;
	}

	Buffer[9] = (Buffer[4] ^ Buffer[5] ^ Buffer[6] ^ Buffer[7] ^ Buffer[8]);
	//printf("Buffer=%p, 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", Buffer, Buffer[0], Buffer[1], Buffer[2], Buffer[3], Buffer[4], Buffer[5], Buffer[6]);
	IFSendData(nChannel, Buffer, 10);

	return DLL_OUTPUT_OK;
}

int PtzMenuCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode)
{
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

#if 0
int GetCruiseData(unsigned int nChannel,
				  unsigned long dwAddress,
				  unsigned long dwCruiseIndex, unsigned long *PresetCount, unsigned long *PresetIndex, unsigned long *DwellTime)
{
	ASSERT_SENDFN;
	ASSERT_RECVFN;

	int ret = 0;

	//EA D5 04 FB 02 00 05 mm CS

	char Buffer[64];

	//协议头
	Buffer[0] = 0xEA;
	Buffer[1] = 0xD5;
	// data 长度为5
	Buffer[2] = 0x04;
	Buffer[3] = ~Buffer[2];
	// 固定为1
	Buffer[4] = 0x02;
	// 固定为0
	Buffer[5] = 0x00;
	// 表示动作
	Buffer[6] = 0x05;
	// 线路号
	Buffer[7] = dwCruiseIndex;
	// 校验码
	Buffer[8] = 0x00;
	IFSendData(nChannel, Buffer, 8);
	rsleep(10000);
	ret = IFRecvData(nChannel, Buffer, 64);
	if (ret <= 0)
	{
		return DLL_ERROR;
	}
	else
	{
		//EA D5 24 DB 02 00 05 mm P1 T1 P2 T2 … CS
		if ((0xEA == Buffer[0]) && (0xD5 == Buffer[0])
			&& (0x06 == Buffer[2]) && (0xF9 == Buffer[3])
			&& (0x02 == Buffer[4]) && (0x00 == Buffer[5])
			&& (0x03 == Buffer[6]) && (0x00 == Buffer[8])
			&& (0x00 == Buffer[9]) && (Buffer[10] == (Buffer[4] ^ Buffer[5] ^ Buffer[6] ^ Buffer[7] ^ Buffer[8] ^ Buffer[9])))
		{
			*Value1 = (int) Buffer[7];
			return DLL_OUTPUT_OK;
		}
	}

	return 0;
}
#endif

int GetEletronicValue(unsigned int nChannel, unsigned long dwAddress, int nValueIndex, int *Value1, int *Value2)
{
	ASSERT_SENDFN;
	ASSERT_RECVFN;
	int ret = 0;

	char Buffer[32];

	//协议头
	Buffer[0] = 0xEA;
	Buffer[1] = 0xD5;
	// data 长度为3
	Buffer[2] = 0x03;
	Buffer[3] = ~Buffer[2];
	// 固定为1
	Buffer[4] = 0x02;
	// 固定为0
	Buffer[5] = 0x00;
	// 表示动作
	Buffer[6] = 0x00;
	// 校验码
	Buffer[7] = 0x00;

	switch (nValueIndex)
	{
		case eStateType:
			*Value1 = 1;
			*Value2 = 0;
			return DLL_OUTPUT_OK;
			Buffer[6] = 0x04;
			Buffer[7] = (Buffer[4] ^ Buffer[5] ^ Buffer[6]);
			IFSendData(nChannel, Buffer, 8);
			ret = RecvMsg(nChannel, Buffer, sizeof (Buffer));
			if (ret <= 0)
			{
				return DLL_ERROR;
			}
			else if (ret >= 10)
			{
				//EA D5 05 FA 02 00 04 MM SS CS
				if ((0xEA == Buffer[0]) && (0xD5 == Buffer[1])
					&& (0x05 == Buffer[2]) && (0xFA == Buffer[3])
					&& (0x02 == Buffer[4]) && (0x00 == Buffer[5])
					&& (0x04 == Buffer[6]) && (Buffer[9] == (Buffer[4] ^ Buffer[5] ^ Buffer[6] ^ Buffer[7] ^ Buffer[8])))
				{
					*Value1 = (int) Buffer[7];
					*Value2 = (int) Buffer[8];
					return DLL_OUTPUT_OK;
				}
			}
			break;
		case eStateTemperature:
			Buffer[6] = 0x03;
			Buffer[7] = (Buffer[4] ^ Buffer[5] ^ Buffer[6]);
			IFSendData(nChannel, Buffer, 8);
			ret = RecvMsg(nChannel, Buffer, sizeof (Buffer));
			if (ret >= 11)
			{
				//EA D5 06 F9 02 00 03 TT 00 00 CS
				if ((0xEA == Buffer[0]) && (0xD5 == Buffer[1])
					&& (0x06 == Buffer[2]) && (0xF9 == Buffer[3])
					&& (0x02 == Buffer[4]) && (0x00 == Buffer[5])
					&& (0x03 == Buffer[6]) && (0x00 == Buffer[8])
					&& (0x00 == Buffer[9]) && (Buffer[10] == (Buffer[4] ^ Buffer[5] ^ Buffer[6] ^ Buffer[7] ^ Buffer[8] ^ Buffer[9])))
				{
					
					 if (Buffer[7]> 127 )    
					 {    
						*Value1 = (int) Buffer[7] - 256;
					 }    
					 else    
					 {    
						*Value1 = (int) Buffer[7];
					}
					return DLL_OUTPUT_OK;
				}
			}
			break;
		case eStateCoordinate:
			Buffer[6] = 0x02;
			Buffer[7] = (Buffer[4] ^ Buffer[5] ^ Buffer[6]);
			IFSendData(nChannel, Buffer, 8);
			ret = RecvMsg(nChannel, Buffer, sizeof (Buffer));
			if (ret >= 15)
			{
				//EA D5 0A F5 02 00 02 PH PL TH TL ZH ZL DD CS
				if ((0xEA == Buffer[0]) && (0xD5 == Buffer[1])
					&& (0x0A == Buffer[2]) && (0xF5 == Buffer[3])
					&& (0x02 == Buffer[4]) && (0x00 == Buffer[5])
					&& (0x02 == Buffer[6])
					&& (Buffer[14] ==
						(Buffer[4] ^ Buffer[5] ^ Buffer[6] ^ Buffer[7] ^ Buffer[8] ^ Buffer[9] ^ Buffer[10] ^ Buffer[11] ^ Buffer[12] ^ Buffer[13])))
				{
					*Value1 = (int) ((((Buffer[7] << 8) | Buffer[8]) << 16) | ((Buffer[9] << 8) | Buffer[10]));
					*Value2 = (int) ((((Buffer[11] << 8) | Buffer[12]) << 16) | Buffer[13]);

					//printf("PTZ Recv: 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X\n", Buffer[7], Buffer[8], Buffer[9], Buffer[10], Buffer[11], Buffer[12], Buffer[13]);
					return DLL_OUTPUT_OK;
				}
			}
			break;

		case eState485Com:
			//EA D5 03 FC 02 00 06 CS
			Buffer[6] = 0x06;
			Buffer[7] = (Buffer[4] ^ Buffer[5] ^ Buffer[6]);
			IFSendData(nChannel, Buffer, 8);
			ret = RecvMsg(nChannel, Buffer, sizeof (Buffer));
			if (ret >= 11)
			{
				//EA D5 06 F9 02 00 06 rr pp qq CS
				if ((0xEA == Buffer[0]) && (0xD5 == Buffer[1])
					&& (0x06 == Buffer[2]) && (0xF9 == Buffer[3])
					&& (0x02 == Buffer[4]) && (0x00 == Buffer[5]) && (0x06 == Buffer[6])
					&& (Buffer[10] == (Buffer[4] ^ Buffer[5] ^ Buffer[6] ^ Buffer[7] ^ Buffer[8] ^ Buffer[9])))
				{
					*Value1 = (int) (Buffer[6] | (Buffer[7] << 8) | (Buffer[8] << 16));
					return DLL_OUTPUT_OK;
				}
			}
			break;

		default:
			return DLL_ERROR;
			break;
	}

	int i = 0;

	printf("Recv Data: %02X", Buffer[0]);
	for (i = 1; i < ret; i++)
	{
		printf(" %02X", Buffer[i]);
	}
	printf("\n");
	if (0 == ret)
	{
		printf("Recv Data Error\n");
	}
	return DLL_ERROR;
}

int SetEletronicValue(unsigned int nChannel, unsigned long dwAddress, int nValueIndex, int Value1, int Value2)
{
	ASSERT_SENDFN;
	//printf("PtzAuxCtrl, nChannel = %lu, dwAddress = %lu, dwPtzCode = %lu, bOn = %d\n", nChannel, dwAddress, dwPtzCode, bOn);
	char Buffer[10];

	//协议头
	Buffer[0] = 0xEA;
	Buffer[1] = 0xD5;
	// data 长度为5
	Buffer[2] = 0x05;
	Buffer[3] = ~Buffer[2];
	// 固定为1
	Buffer[4] = 0x01;
	// 固定为0
	Buffer[5] = 0x00;
	// 表示动作
	Buffer[6] = 0x71;
	// 固定为0
	Buffer[7] = 0x00;
	// 预置点号
	Buffer[8] = 0x00;
	// 校验码
	Buffer[9] = 0x00;

	switch (nValueIndex)
	{
			//!数字变倍
		case ePropDigitZoom:
			Buffer[7] = 21;
			Buffer[8] = Value1 ? 2 : 3;
			break;
			//!低照度
		case ePropLowLum:
			Buffer[7] = 24;
			Buffer[8] = Value1 ? 2 : 3;
			break;
			//!背光补偿
		case ePropBackLight:
			Buffer[7] = 25;
			Buffer[8] = Value1 ? 2 : 3;
			break;
			//!自动快门
		case ePropAutoShutter:
			Buffer[7] = 26;
			Buffer[8] = Value1 ? 2 : 3;
			break;
			//!白平衡
		case ePropWhiteBalance:
			Buffer[7] = 30;
			Buffer[8] = Value1;
			break;
			//!红增益
		case ePropRedGain:
			Buffer[7] = 31;
			Buffer[8] = Value1;
			break;
			//!蓝增益
		case ePropBlueGain:
			Buffer[7] = 32;
			Buffer[8] = Value1;
			break;
			//!锐度
		case ePropAcutance:
			Buffer[7] = 39;
			Buffer[8] = Value1;
			break;
			//!曝光模式
		case ePropExpMode:
			Buffer[7] = 37;
			Buffer[8] = Value1;
			break;
			//!快门速度
		case ePropShutterSpeed:
			Buffer[7] = 29;
			Buffer[8] = Value1;
			break;
			//!光圈
		case ePropIrisSpeed:
			Buffer[7] = 38;
			Buffer[8] = Value1;
			break;
			//!宽动态
		case ePropWideDyn:
			Buffer[7] = 40;
			Buffer[8] = Value1 ? 2 : 3;
			break;
			//!3D降噪
		case eProp3DFilter:
			Buffer[7] = 41;
			Buffer[8] = Value1 ? 2 : 3;
			break;
			//!镜像
		case ePropMirror:
			break;
			//!翻转
		case ePropReverse:
			Buffer[7] = 1;
			Buffer[8] = Value1 ? 2 : 3;
			break;
		case eProplimitS:
			Buffer[7] = 2;
			Buffer[8] = Value1 ? 2 : 3;
			break;
		case ePropnorth:
			Buffer[7] = 4;
			break;			
		case ePropHDVideoMode:
			//!EA D5 05 FA 04 03 rr pp qq CS
			Buffer[2] = 0x03;
			//分辨率
			Buffer[3] = 0xfc;
			//制式频率
			Buffer[4] = 0x04;
			//高宽比
			Buffer[5] = 0x10;
			Buffer[6] = (Value1 );
			
			if (Buffer[6]  == 0)
			{
				Buffer[6] = 2 ;
			}
			else
			{
				Buffer[6] = 1;
			}
			Buffer[7] = (Buffer[4] ^ Buffer[5] ^ Buffer[6]);
			IFSendData(nChannel, Buffer, 8);
			return DLL_OUTPUT_OK;
			
			break;
			//低功耗
		case eLowPowerConsumption:	
			Buffer[7] = 6;
			Buffer[8] = Value1 ? 2 : 3;
			break;
		
			//雨刷		
		case eScreenWiper:
			Buffer[7] = 61;
			
			if (Value1 == 1)
			Buffer[8] = 4;
			
			else if (Value1 == 0)
			Buffer[8] = 3;
			
			else if (Value1 == 2)
			Buffer[8] = 2;
			break;
		
			//夜视设备
		case eNightVision:
			Buffer[7] = 62;
						
			if (Value1 == 1)
			Buffer[8] = 3;
			
			else if (Value1 == 2)
			Buffer[8] = 2;
			
			else if (Value1 == 0)
			Buffer[8] = 4;
			break;
		
			//除雾
		case eDemist:
			Buffer[7] = 63;
			Buffer[8] = Value1 ? 2 : 3;
			break;
		
		case ePropreboot:
			Buffer[7] = 0x07;
			break;
		default:
			return DLL_ERROR;
			break;
	}

	Buffer[9] = (Buffer[4] ^ Buffer[5] ^ Buffer[6] ^ Buffer[7] ^ Buffer[8]);
	//printf("Buffer=%p, 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", Buffer, Buffer[0], Buffer[1], Buffer[2], Buffer[3], Buffer[4], Buffer[5], Buffer[6]);
	IFSendData(nChannel, Buffer, 10);

	return DLL_OUTPUT_OK;
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
