#include <stdio.h>
#include "CJsonNet.h"
#include "cJSON.h"
#include "string.h"
#include "newhie/HieServer.h"
#include "CJsonPub.h"

#define TUTK_LENTH_MAX                  64
#define DOMAINSUFFIX_LENTH_MAX          64

int Json_HighPrioritySupport(char *nBuffer, int nBufferLenth)
{
    /* TODO */
    JSON_ERROR("Json_HighPrioritySupport fail\n");
    return -1;
}

int Json_NETParameters(char *nBuffer, int nBufferLenth)
{
    /* TODO */
    JSON_ERROR("Json_NETParameters fail\n");
    return -1;
}

#if defined(__IS_SUPPORT_WLAN__)
int Json_WifiParamTest(char *nBuffer,int nBufferLenth, char *nFirParam, char *nSecParam)
{
    /* TODO */
    JSON_ERROR("Json_WifiParamTest fail\n");
    return -1;
}

int Json_GetWifiTestStatue(char *nBuffer, int nBufferLenth)
{
    /* TODO */
    JSON_ERROR("Json_GetWifiTestStatue fail\n");
    return -1;
}

int Json_GetWifiAccessPointList(char *nBuffer,int nBufferLenth)
{
    /* TODO */
    JSON_ERROR("Json_GetWifiAccessPointList fail\n");
    return -1;
}
#endif

int Json_BlueStatus(char *nBuffer, int nBufferLenth)
{
    /* TODO */
    JSON_ERROR("Json_GetWifiAccessPointList fail\n");
    return -1;
}
