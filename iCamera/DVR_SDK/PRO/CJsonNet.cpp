#include <stdio.h>
#include "CJsonNet.h"
#include "cJSON.h"
#include "string.h"
#include "newhie/HieServer.h"
#include "CJsonPub.h"
#include "MPU_Net_API.h"

#define TUTK_LENTH_MAX                  64
#define DOMAINSUFFIX_LENTH_MAX          64

static int NETParametersToJson(char *nBuffer, int nBufferLenth, NetCardInfo NetworkInfo)
{
	int nRet = 0;

	if(NULL == nBuffer)
	{
		JSON_ERROR("nBuffer is NULL, Pls check it\n");
		return -1;
	}
	
	cJSON * root =  cJSON_CreateObject(); 
	if(!root)
	{
		JSON_ERROR("create Json error, please try again\n");
		return -1;
	}

	cJSON_AddItemToObject(root, "code", cJSON_CreateNumber(JSON_SUCCESS_CODE));
	cJSON * nData =  cJSON_CreateObject();
	if(!nData)
	{
		cJSON_Delete(root);
		JSON_ERROR("create root Json error, please try again\n");
		return -1;
	}
	

	cJSON_AddItemToObject(root, "data", nData);

	//data下创建 isPass节点
	cJSON_AddItemToObject(nData, "isPass", cJSON_CreateString("true"));

    //data下创建 Data节点
	cJSON_AddItemToObject(nData, "Bluetooth", cJSON_CreateNumber(NetworkInfo.nBluetoothIsExist));
	cJSON_AddItemToObject(nData, "Sparklink", cJSON_CreateNumber(NetworkInfo.nSparklinkIsExist));
	cJSON_AddItemToObject(nData, "NetworkType", cJSON_CreateNumber(NetworkInfo.nNetworkType));
	cJSON_AddItemToObject(nData, "WIFIband", cJSON_CreateNumber(NetworkInfo.nWIFIband));
	cJSON_AddItemToObject(nData, "WIFIspeed1", cJSON_CreateString(NetworkInfo.WIFIspeed1));
	cJSON_AddItemToObject(nData, "WIFIspeed2", cJSON_CreateString(NetworkInfo.WIFIspeed2));

	cJSON_AddItemToObject(root, "message", cJSON_CreateString(SUCCESS_MESSAGE));
	char *nBuffString =  cJSON_Print(root);
	JSON_INFO("INFO: %s\n", nBuffString);
	memset(nBuffer, 0, nBufferLenth);
	strncpy(nBuffer, nBuffString, nBufferLenth-1);

	cJSON_Delete(root);
	root = NULL;

	return nRet;
}

static int HighPiorityToJson(char *nBuffer, int nBufferLenth , int status , char *DomainSuffix)
{
	int nRet = 0;

	if(NULL == nBuffer || NULL == DomainSuffix)
	{
		JSON_ERROR("nBuffer or DomainSuffix is NULL, Pls check it\n");
		return -1;
	}
	
	cJSON * root =  cJSON_CreateObject(); 
	if(!root)
	{
		JSON_ERROR("create Json error, please try again\n");
		return -1;
	}

	cJSON_AddItemToObject(root, "code", cJSON_CreateNumber(JSON_SUCCESS_CODE));
	cJSON * nData =  cJSON_CreateObject();
	if(!nData)
	{
		cJSON_Delete(root);
		JSON_ERROR("create root Json error, please try again\n");
		return -1;
	}
	

	cJSON_AddItemToObject(root, "data", nData);

	//data下创建 isPass节点
	if(1 == status)
	{
		cJSON_AddItemToObject(nData, "isPass", cJSON_CreateString("true"));
	}
	else
	{
		cJSON_AddItemToObject(nData, "isPass", cJSON_CreateString("false"));
	}

    //data下创建 info节点
	cJSON_AddItemToObject(nData, "info", cJSON_CreateString(DomainSuffix));

	cJSON_AddItemToObject(root, "message", cJSON_CreateString(SUCCESS_MESSAGE));
	
	char *nBuffString =  cJSON_Print(root);
	JSON_DEBUG("DBG: %s\n", nBuffString);
	
	memset(nBuffer, 0, nBufferLenth);
	strncpy(nBuffer, nBuffString, nBufferLenth);

	cJSON_Delete(root);
	root = NULL;

	return nRet;
}

int Json_HighPrioritySupport(char *nBuffer, int nBufferLenth)
{
    int nRet = 0;
    int status = 0;
    char tutk[TUTK_LENTH_MAX] = {0};
    char domainsuffix[DOMAINSUFFIX_LENTH_MAX] = {0};

    if(NULL == nBuffer)
    {
        JSON_ERROR("nBuffer is NULL, GetWifiTestStatue fail\n");
        return -1;
    }

    nRet = MPU_Net_GetAuthDomainSuffixExist_Test(&status);

    nRet = MPU_Net_GetTutkAndDomainSuffix_Test(tutk, TUTK_LENTH_MAX, domainsuffix, DOMAINSUFFIX_LENTH_MAX);

    nRet = HighPiorityToJson(nBuffer, nBufferLenth, status, domainsuffix);

    return nRet;
    
}

int Json_NETParameters(char *nBuffer, int nBufferLenth)
{
    int nRet = 0;

    NetCardInfo NetworkInfo = {0};

    if(NULL == nBuffer)
    {
        JSON_ERROR("nBuffer is NULL, GetWifiTestStatue fail\n");
        return -1;
    }

    nRet = MPU_Net_GetNetCardInfo(&NetworkInfo);
    if (0 != nRet)
    {
        JSON_ERROR("get wifi module info failed!\n");
        return -1;
    }

    nRet = NETParametersToJson(nBuffer,nBufferLenth, NetworkInfo);

    return nRet;
}

#if defined (__IS_SUPPORT_WLAN__)
static int WifiAccessPointToJson(char *nBuffer, int nBufferLenth ,WLAN_ACCESSPOINT *nWifiStatue, int nCount)
{
	int nRet = 0;
	int  i  = 0;
	cJSON * nlist = NULL;
	
	if(NULL == nBuffer || NULL == nWifiStatue)
	{
		JSON_ERROR("nBuffer or nWifiStatue is NULL, Pls check it\n");
		return -1;
	}
	
	cJSON * root =  cJSON_CreateObject(); 
	if(!root)
	{
		JSON_ERROR("create Json error, please try again\n");
		return -1;
	}

	cJSON_AddItemToObject(root, "code", cJSON_CreateNumber(JSON_SUCCESS_CODE));
	cJSON * nData =  cJSON_CreateObject();
	if(!nData)
	{
		cJSON_Delete(root);
		JSON_ERROR("create root Json error, please try again\n");
		return -1;
	}
	

	cJSON_AddItemToObject(root, "data", nData);
	cJSON_AddItemToObject(nData, "isPass", cJSON_CreateString("true"));

	
    cJSON * nAccessPoint =  cJSON_CreateArray();
	if(!nAccessPoint)
	{
		cJSON_Delete(root);
		JSON_ERROR("create nDisklist Json error, please try again\n");
		return -1;
	}
    cJSON_AddItemToObject(nData, "AccessPoint", nAccessPoint);//root节点下添加semantic节点

	for (i = 0; i < nCount; i++)
	{
		nlist =  cJSON_CreateObject();
		if(!nlist)
		{
			cJSON_Delete(root);
			JSON_ERROR("create nDisklist Json error, please try again\n");
			return -1;
		}
		cJSON_AddItemToArray(nAccessPoint, nlist);

		cJSON_AddItemToObject(nlist, "ID", cJSON_CreateNumber(i+1));//添加name节点
		cJSON_AddItemToObject(nlist, "SSID", cJSON_CreateString(nWifiStatue[i].szSSID));//添加name节点	
		cJSON_AddItemToObject(nlist, "MACAddress", cJSON_CreateString(nWifiStatue[i].szMacAddress));//添加name节点
		cJSON_AddItemToObject(nlist, "SignalValue", cJSON_CreateNumber(nWifiStatue[i].nQuality));//添加name节点
		cJSON_AddItemToObject(nlist, "Authentication", cJSON_CreateNumber(nWifiStatue[i].nAuthentication));//添加name节点
		cJSON_AddItemToObject(nlist, "EncryptType", cJSON_CreateNumber(nWifiStatue[i].nEncrypType));//添加name节点
		cJSON_AddItemToObject(nlist, "WirelessMode", cJSON_CreateNumber(nWifiStatue[i].nWirelessMode));//添加name节点
	}

	cJSON_AddItemToObject(root, "message", cJSON_CreateString(SUCCESS_MESSAGE));
	
	char *nBuffString =  cJSON_Print(root);
	JSON_INFO("INFO: %s\n", nBuffString);

	memset(nBuffer, 0, nBufferLenth);
	strncpy(nBuffer, nBuffString, nBufferLenth);

	cJSON_Delete(root);
	root = NULL;

	free(nBuffString);
	nBuffString = NULL;

	return nRet;
}

int Json_WifiParamTest(char *nBuffer,int nBufferLenth, char *nFirParam, char *nSecParam)
{
	int nRet = 0;
	WLAN_CONNECTION wifiConnection;
	
	if(NULL == nBuffer || NULL == nFirParam || NULL == nSecParam)
	{
		JSON_ERROR("nBuffer is NULL, WifiParamTest fail\n");
		return -1;
	}

	memset(&wifiConnection, 0, sizeof(WLAN_CONNECTION));

	wifiConnection.struKeyCfg.WPAKey.bValid = 1;
    wifiConnection.struKeyCfg.bValid = 1;

	strncpy(wifiConnection.szSSID, nFirParam, sizeof(wifiConnection.szSSID)-1);

	strncpy(wifiConnection.struKeyCfg.WPAKey.szKey, nSecParam, sizeof(wifiConnection.struKeyCfg.WPAKey.szKey)-1);

	wifiConnection.nAuthMode = 2;

	wifiConnection.nEncrypType =3;

	nRet = MPU_WLAN_SetConnection(&wifiConnection);

	if(-1 == nRet)
	{
		JSON_ERROR("wifi Connect error, please try again!\n");
		return -1;
	}

	nRet = CJsonPubRespond(nBuffer, nBufferLenth, 1, eWifiParamTest);

	return nRet;
}

int Json_GetWifiTestStatue(char *nBuffer, int nBufferLenth)
{
	int nRet = 0;

	if(NULL == nBuffer)
	{
		JSON_ERROR("nBuffer is NULL, GetWifiTestStatue fail\n");
		return -1;
	}


	WLAN_CONNECTION conn;

	nRet = MPU_WLAN_GetConnection(&conn);
	if (0 != nRet)
	{
		return nRet;
	}

	nRet = CJsonPubRespond(nBuffer, nBufferLenth, conn.bConnect, eWifiTestStatue);

	return nRet; 
}

int Json_GetWifiAccessPointList(char *nBuffer,int nBufferLenth)
{
	int nRet = 0;
	WLAN_ACCESSPOINT AccessPoints[WLAN_MAX_AP_NUM];

	int count = 0;

	if(NULL == nBuffer)
	{
		JSON_ERROR("nBuffer is NULL, GetWifiAccessPointList fail\n");
		return -1;
	}

	MPU_WLAN_GetAccessPoints(AccessPoints, &count);

	count = count > DVR_MAX_AP_NUM ? DVR_MAX_AP_NUM : count;

	nRet = WifiAccessPointToJson(nBuffer,nBufferLenth, AccessPoints,count);
	
	return nRet; 
}
#endif

int Json_BlueStatus(char *nBuffer, int nBufferLenth)
{
    int nRet = 0;
	
#ifdef __IS_SUPPORT_AIC_BLUETOOTH__	
	int nBlueStatus = 0;
	if(NULL == nBuffer)
	{
		JSON_ERROR("nBuffer is NULL, pls check!\n");
		return -1;
	}

    cJSON * root =  cJSON_CreateObject(); 
	if(!root)
	{
		JSON_ERROR("create Json error, please try again\n");
		return -1;
	}
    cJSON * nData =  cJSON_CreateObject();
	if(!nData)
	{
		cJSON_Delete(root);
		JSON_ERROR("create root Json error, please try again\n");
		return -1;
	}
	
	cJSON_AddItemToObject(root, "code", cJSON_CreateNumber(JSON_SUCCESS_CODE));
	cJSON_AddItemToObject(root, "data", nData);
    MPU_WLAN_BluetoothProTestStatus(&nBlueStatus);
    if(nBlueStatus)
    {
        cJSON_AddItemToObject(nData, "isPass", cJSON_CreateString("false"));
    }
    else
    {
        cJSON_AddItemToObject(nData, "isPass", cJSON_CreateString("true"));
    }
    cJSON_AddItemToObject(root, "message", cJSON_CreateString("Successful!"));
	char *nBuffString =  cJSON_Print(root);
	JSON_DEBUG("DBG: %s\n", nBuffString);
	
	memset(nBuffer, 0, nBufferLenth);
	strncpy(nBuffer, nBuffString, nBufferLenth);

	cJSON_Delete(root);
	root = NULL;
#endif
    return nRet;
}

