#ifndef __TAG_TRANSPARENT_H_
#define __TAG_TRANSPARENT_H_

#include "newhie/HieServer.h"
#include "tagSerialPort.h"

typedef struct tagCreateTransparentParam
{
	int hSerial;
	int nSerialType;
	int nSerialPort;
} CreateTransparentParam, *LPCreateTransparentParam;

typedef struct tagReadTransparentParam
{
	int hSerial;
	char *pBuffer;
	int nLength;
} ReadTransparentParam, LPReadTransparentParam;

typedef struct tagTransparentStopPara
{
	int hSerial;
} TransparentStopParam, LPTransparentStopParam;

typedef int (*IF_TransparentChannelWrite) (HANDLE hIndex, const LPTransparentParaInfo pTransparentParaInfo);

#endif /*__TAG_TRANSPARENT_H_*/
