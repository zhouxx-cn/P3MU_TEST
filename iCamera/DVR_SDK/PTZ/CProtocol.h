#ifndef __CPROTOCOL_H_
#define __CPROTOCOL_H_

#include "tagPtzFlag.h"
#include "tagPtzCode.h"


typedef int (*DllPtzDevNormalCtrl) (unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, unsigned long dwSpeed);

typedef int (*DllPtzDevAuxCtrl) (unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzAuxCode, int bOn);

typedef int (*DllPtzDevAutoCtrl) (unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, unsigned long dwPresetIndex1,
								  unsigned long dwPresetIndex2, unsigned long dwCruiseIndex, unsigned long dwSpeed, unsigned long dwDwellTime,
								  int bScan);
typedef int (*DllPtzDevPresetCtrl) (unsigned int nChannel, unsigned long dwAddress, unsigned long dwCruiseIndex, unsigned long dwPresetIndex,
									unsigned long dwPtzCode, unsigned long dwSpeed, unsigned long dwDwellTime);
typedef int (*DllPtzDevMenuCtrl) (unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode);

typedef int (*DllPtzDevHasFunction) (unsigned int nPtzFlag);

typedef int (*DllPtzDevSetCallback) (CbSendData FnSendData, CbRecvData FnRecvData, CbStartCruise FnStartCruise, CbStopCruise FnStopCruise);

typedef void (*DLLPtzDevGetTechnicalData) (int *PresetCount, int *CruiseCount, int *TrackCount, int *AutoCount, unsigned long *Baudrate);

typedef int (*DLLPtzDevGetEletronicValue) (unsigned int nChannel, unsigned long dwAddress, int nValueIndex, int *Value1, int *Value2);

typedef int (*DLLPtzDevSetEletronicValue) (unsigned int nChannel, unsigned long dwAddress, int nValueIndex, int Value1, int Value2);

typedef const char *(*DLLPtzDevGetVersion) (void);

//!\brief 云台协议包装类
//##ModelId=49A36CA4008C
struct CProtocol
{
	CProtocol():PtzNormalCtrl(NULL), PtzAutoCtrl(NULL), PtzPresetCtrl(NULL), PtzAuxCtrl(NULL), PtzMenuCtrl(NULL), HasFunction(NULL),
		SetCallback(NULL), GetTechnicalData(NULL), GetEletronicValue(NULL), SetEletronicValue(NULL), GetVersion(NULL)
	{
	};

	DllPtzDevNormalCtrl PtzNormalCtrl;

	DllPtzDevAutoCtrl PtzAutoCtrl;

	DllPtzDevPresetCtrl PtzPresetCtrl;

	DllPtzDevAuxCtrl PtzAuxCtrl;

	DllPtzDevMenuCtrl PtzMenuCtrl;

	DllPtzDevHasFunction HasFunction;

	DllPtzDevSetCallback SetCallback;

	DLLPtzDevGetTechnicalData GetTechnicalData;

	DLLPtzDevGetEletronicValue GetEletronicValue;

	DLLPtzDevSetEletronicValue SetEletronicValue;

	DLLPtzDevGetVersion GetVersion;
};


#endif							/* __CPROTOCOL_H_ */
