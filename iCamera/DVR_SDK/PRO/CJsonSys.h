
#ifndef __CJSON_SYS_H__
#define __CJSON_SYS_H__

#include <stdio.h>


int Json_FactoryDefault(char *nBuffer , int nBufferLenth, int nTestType);

int Json_HardResetTest(char *nBuffer , int nBufferLenth, int nTestType);

int Json_SensorCntCorrect(char *nBuffer, int nBufferLenth, char *nFirParam, int nFirParamLenth);

#ifdef __ONE_CLICK_CALL__

int Json_HardKeyTocallTest(char *nBuffer , int nBufferLenth, int nTestType);

#endif

#endif

