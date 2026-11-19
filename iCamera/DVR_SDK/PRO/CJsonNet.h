#ifndef __CJSON_NET_H__
#define __CJSON_NET_H__

#include <stdio.h>

int Json_NETParameters(char *nBuffer, int nBufferLenth);

int Json_WifiParamTest(char *nBuffer,int nBufferLenth, char *nFirParam, char *nSecParam);

int Json_GetWifiTestStatue(char *nBuffer,int nBufferLenth);

int Json_GetWifiAccessPointList(char *nBuffer,int nBufferLenth);

int Json_HighPrioritySupport(char *nBuffer, int nBufferLenth);

int Json_BlueStatus(char *nBuffer, int nBufferLenth);


#endif
