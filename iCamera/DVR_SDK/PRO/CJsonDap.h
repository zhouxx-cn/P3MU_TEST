
#ifndef __CJSON_DAP_H__
#define __CJSON_DAP_H__

#include <stdio.h>
#include "CProManager.h"

int Json_DapRedLightTest(char *nBuffer , int nBufferLenth);

int Json_DapWhiteLightTest(char *nBuffer , int nBufferLenth);
	
int Json_DapGreenLightTest(char *nBuffer , int nBufferLenth);

int Json_DapCameraSet(char *nBuffer , int nBufferLenth);

int  Json_DapAlarmout(char *nBuffer , int nBufferLenth);

int Json_DapDoorLock(char *nBuffer , int nBufferLenth);


#endif
