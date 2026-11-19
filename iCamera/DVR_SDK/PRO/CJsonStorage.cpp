

#include <stdio.h>
#include "cJSON.h"
#include "string.h"
#include "CJsonStorage.h"
#include "STR/tagDiskInfo.h"
#include "newhie/HieServer.h"
#include "MPU_STR_API.h"
#include "MPU_AVL_API.h"
#include "CJsonPub.h"
#include "MPU_Net_API.h"
// #include "sys_banned.h"
#include "global_gather.h"


typedef struct tagJsonDiskInfo
{
	unsigned int  nDiskID;				   //第几块盘组
	char nDiskTypeName[32];        		   //名称
	unsigned long nTotalCapacity;
	unsigned long nAvailableCapacity;
	int nDiskStorageType;
	int nDiskStorageAttribute;			   //状态
	int nPictureCapacityPercent;
	int nRecordCapacityPercent;
	int nDiskFormatStatus;
}JsonDiskInfo;

typedef struct tag4GCardInfo
{
	char szIMSI[16];
	char szIMEI[16];
	char szACT[64];
	char szOperator[64];			//运营商
	char szBand[64];				//频段信息
	char szICCID[32];				//ICCID
	char Version[128];
}Json4GCardInfo;

static int SdInfoToJson(char *nBuffer, JsonDiskInfo *nDiskInfo, int nBufferLenth)
{
	if(NULL == nBuffer || NULL == nDiskInfo)
	{
		JSON_ERROR("nBuffer or nDiskInfo is NULL, Pls check it\n");
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
	cJSON_AddItemToObject(nData, "isPass", cJSON_CreateString("true"));

	
    cJSON * nDisklist =  cJSON_CreateArray();
	if(!nDisklist)
	{
		cJSON_Delete(root);
		JSON_ERROR("create nDisklist Json error, please try again\n");
		return -1;
	}
    cJSON_AddItemToObject(nData, "Disklist", nDisklist);//root节点下添加semantic节点


	
	cJSON * nlist =  cJSON_CreateObject();
	if(!nlist)
	{
		cJSON_Delete(root);
		JSON_ERROR("create nDisklist Json error, please try again\n");
		return -1;
	}
	cJSON_AddItemToArray(nDisklist, nlist);
	if(nDiskInfo->nPictureCapacityPercent != 0 || nDiskInfo->nRecordCapacityPercent != 0)
	{
	    cJSON_AddItemToObject(nlist, "DiskID", cJSON_CreateNumber(nDiskInfo->nDiskID));//添加name节点
		cJSON_AddItemToObject(nlist, "DiskTypeName", cJSON_CreateString(nDiskInfo->nDiskTypeName));//添加name节点	
		cJSON_AddItemToObject(nlist, "TotalCapacity", cJSON_CreateNumber(nDiskInfo->nTotalCapacity));//添加name节点
		cJSON_AddItemToObject(nlist, "AvailableCapacity", cJSON_CreateNumber(nDiskInfo->nAvailableCapacity));//添加name节点
		cJSON_AddItemToObject(nlist, "DiskStorageType", cJSON_CreateNumber(nDiskInfo->nDiskFormatStatus));//添加name节点
		cJSON_AddItemToObject(nlist, "DiskStorageAttribute", cJSON_CreateNumber(nDiskInfo->nDiskStorageAttribute));//添加name节点
		cJSON_AddItemToObject(nlist, "PictureCapacityPercent", cJSON_CreateNumber(nDiskInfo->nPictureCapacityPercent));//添加name节点
		cJSON_AddItemToObject(nlist, "RecordCapacityPercent", cJSON_CreateNumber(nDiskInfo->nRecordCapacityPercent));//添加name节点	
		cJSON_AddItemToObject(nlist, "DiskFormatStatus", cJSON_CreateNumber(nDiskInfo->nDiskFormatStatus));//添加name节点
	}
	cJSON_AddItemToObject(root, "message", cJSON_CreateString(SUCCESS_MESSAGE));
	
	char *nBuffString =  cJSON_Print(root);
	JSON_INFO("INFO: %s\n", nBuffString);
	
	memset(nBuffer, 0, nBufferLenth);
	strncpy(nBuffer, nBuffString, nBufferLenth - 1);

	cJSON_Delete(root);
	root = NULL;

	return 0;
}


static int GetDevDiskInfo(JsonDiskInfo *nDiskInfo)
{
	tagDiskInfo Info;
	
	nDiskInfo->nDiskID = 0;
	strncpy(nDiskInfo->nDiskTypeName, "SD", sizeof (nDiskInfo->nDiskTypeName) - 1);

	memset(&Info, 0, sizeof(tagDiskInfo));	
	
	MPU_STR_Disk_GetInfo(0, &Info);

	nDiskInfo->nTotalCapacity = Info.Total_space;
	nDiskInfo->nAvailableCapacity = Info.tPartitions[0].Available_space;
	
	nDiskInfo->nDiskFormatStatus = MPU_STR_Disk_GetDiskFormatStatus(0);
	nDiskInfo->nDiskFormatStatus = nDiskInfo->nDiskFormatStatus > 5 ? 3 : nDiskInfo->nDiskFormatStatus;

    if (eZWDiskFormatStatus_Formatting == nDiskInfo->nDiskFormatStatus)
    {
        nDiskInfo->nDiskStorageAttribute = eZWDiskStatus_Formatting;
    }
    else if (eZWDiskFormatStatus_UmountFail == nDiskInfo->nDiskFormatStatus)
    {
        nDiskInfo->nDiskStorageAttribute = eZWDiskStatus_FormatFailNeedReboot;
    }
    else if (eZWDiskFormatStatus_FormatErr == nDiskInfo->nDiskFormatStatus)
    {
        if (eZWDiskStatus_NoDisk != nDiskInfo->nDiskStorageAttribute)
        {
            nDiskInfo->nDiskStorageAttribute = eZWDiskStatus_ExpNeedChangeDisk;
        }
    }
	
	MPU_AVL_GetDiskResourceSize(0, &nDiskInfo->nRecordCapacityPercent, &nDiskInfo->nPictureCapacityPercent);

	return 0;
}

int Json_GetSdInfo(char *nBuffer,int nBufferLenth, int nTestType)
{
	int nRet = 0;
	JsonDiskInfo nJsonDiskInfo = {0};

	if(NULL == nBuffer)
	{
		JSON_ERROR("nBuffer is NULL, GetSdInfo fail\n");
		return -1;
	}

	GetDevDiskInfo(&nJsonDiskInfo);


	nRet = SdInfoToJson(nBuffer, &nJsonDiskInfo, nBufferLenth);

	return nRet; 
}



int Json_SdFormat(char *nBuffer, int nBufferLenth, int nTestType)
{
	int nRet = 0;

	if(NULL == nBuffer)
	{
		JSON_ERROR("nBuffer is NULL, SdFormat fail\n");
		return -1;
	}
	
	nRet = MPU_STR_Disk_FormatPartition(0, 0);

	if (nRet)
	{
		JSON_ERROR("FormatPartition  failed<%d>\n", nRet);
		return -1;
	}

	nRet = CJsonPubRespond(nBuffer, nBufferLenth, 1, nTestType);


	return nRet; 
}

#ifdef __MODULE_4G_SUPPORT__

/*@breif ProductTest4GDataToJson 4G产测数据创建json字符串
 *@param sim_card 当前测试的卡槽号，0：外置卡  1：内置卡
 *@return 成功返回 0
 *@       失败返回-1
 */
static int ProductTest4GDataToJson(char *nBuffer, Module4G_ProductTest_Data_t *nProductTestData, int nBufferLenth,int nResult,int sim_card)
{
	char csq[10] = {0};

	if(NULL == nBuffer || NULL == nProductTestData)
	{
		JSON_ERROR("nBuffer or nProductTestData is NULL, Pls check it\n");
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

	if (0 == nResult)
	{
		cJSON_AddItemToObject(nData, "isPass", cJSON_CreateString("true"));
	}
	else
	{
		cJSON_AddItemToObject(nData, "isPass", cJSON_CreateString("false"));
	}

    cJSON * nProductData =  cJSON_CreateObject();
	if(!nProductData)
	{
		cJSON_Delete(root);
		JSON_ERROR("create nDisklist Json error, please try again\n");
		return -1;
	}
    cJSON_AddItemToObject(nData, "AccessPoint", nProductData);

    //这里后续需支持多卡的信息传递（设备层、产测工具及相关json需修改）
	if (sim_card >= Get4GRealSimCardCount() || 0 == nProductTestData->uSimCardStatus[sim_card])
	{
		JSON_ERROR("simcard num:%d or simcard status:%d error!\n", sim_card, nProductTestData->uSimCardStatus[sim_card]);
		return -1;
	}

    cJSON_AddItemToObject(nProductData, "IMSI", cJSON_CreateString(nProductTestData->sImsi[sim_card]));
	cJSON_AddItemToObject(nProductData, "IMEI", cJSON_CreateString(nProductTestData->sImei));
	cJSON_AddItemToObject(nProductData, "ACT", cJSON_CreateString(nProductTestData->sACT[sim_card]));
	cJSON_AddItemToObject(nProductData, "Operator", cJSON_CreateString(nProductTestData->sOper[sim_card]));
	cJSON_AddItemToObject(nProductData, "Band", cJSON_CreateString(nProductTestData->sBand[sim_card]));
	cJSON_AddItemToObject(nProductData, "ICCID", cJSON_CreateString(nProductTestData->sIccid[sim_card]));
	cJSON_AddItemToObject(nProductData, "_4GModuleVersion", cJSON_CreateString(nProductTestData->sVersion));

	/* 和产品确认，只有双卡设备，产测才需要校验SIM卡状态 */
	if (Get4GRealSimCardCount() == 2)
	{
		cJSON_AddItemToObject(nProductData, "GetStatus", cJSON_CreateString("true"));
	}

	if (nProductTestData->uSimCardStatus[sim_card])
	{
		cJSON_AddItemToObject(nProductData, "CardStatus", cJSON_CreateString("true"));
	}
	else
	{
		cJSON_AddItemToObject(nProductData, "CardStatus", cJSON_CreateString("false"));
	}

	if (nProductTestData->uCregStatus[sim_card])
	{
		cJSON_AddItemToObject(nProductData, "NetworkStatus", cJSON_CreateString("true"));
	}
	else
	{
		cJSON_AddItemToObject(nProductData, "NetworkStatus", cJSON_CreateString("false"));
	}

	if (nProductTestData->uInternetConnStatus)
	{
		cJSON_AddItemToObject(nProductData, "NetworkConnection", cJSON_CreateString("true"));
	}
	else
	{
		cJSON_AddItemToObject(nProductData, "NetworkConnection", cJSON_CreateString("false"));
	}

	snprintf(csq,sizeof(csq),"%d",nProductTestData->iCsq[sim_card]);
	cJSON_AddItemToObject(nProductData, "NetworkSignal", cJSON_CreateString(csq));

	cJSON_AddItemToObject(root, "message", cJSON_CreateString(SUCCESS_MESSAGE));
	
	char *nBuffString =  cJSON_Print(root);
	JSON_INFO("INFO: %s\n", nBuffString);
	
	memset(nBuffer, 0, nBufferLenth);
	strncpy(nBuffer, nBuffString, nBufferLenth);

	cJSON_Delete(root);
	root = NULL;

	return 0;
}

static int SimCardInfoToJson(char *nBuffer, int nBufferLenth, Json4GCardInfo *pcfg)
{
	int nRet = 0;

	if(NULL == nBuffer || NULL == pcfg)
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
	cJSON_AddItemToObject(nData, "isPass", cJSON_CreateString("true"));

	cJSON * nAccessPoint =  cJSON_CreateObject();
	if(!nAccessPoint)
	{
		cJSON_Delete(root);
		JSON_ERROR("create root Json error, please try again\n");
		return -1;
	}
	cJSON_AddItemToObject(nData, "AccessPoint", nAccessPoint);

	cJSON_AddItemToObject(nAccessPoint, "IMSI", cJSON_CreateString(pcfg->szIMSI));
	cJSON_AddItemToObject(nAccessPoint, "IMEI", cJSON_CreateString(pcfg->szIMEI));
	cJSON_AddItemToObject(nAccessPoint, "ACT", cJSON_CreateString(pcfg->szACT));
	cJSON_AddItemToObject(nAccessPoint, "Operator", cJSON_CreateString(pcfg->szOperator));
	cJSON_AddItemToObject(nAccessPoint, "Band", cJSON_CreateString(pcfg->szBand));
	cJSON_AddItemToObject(nAccessPoint, "ICCID", cJSON_CreateString(pcfg->szICCID));
    cJSON_AddItemToObject(nAccessPoint, "_4GModuleVersion", cJSON_CreateString(pcfg->Version));

	cJSON_AddItemToObject(root, "message", cJSON_CreateString(SUCCESS_MESSAGE));
	
	char *nBuffString =  cJSON_Print(root);
	JSON_INFO("INFO: %s\n", nBuffString);
	
	memset(nBuffer, 0, nBufferLenth);
	strncpy(nBuffer, nBuffString, nBufferLenth);

	cJSON_Delete(root);
	root = NULL;
	

	return nRet;
}


static int GPRSSignalQualityToJson(char *nBuffer, int nBufferLenth, int nSignalQuality)
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

	if(nSignalQuality > 0)
	{
		cJSON_AddItemToObject(nData, "isPass", cJSON_CreateString("true"));
	}
	else
	{
		cJSON_AddItemToObject(nData, "isPass", cJSON_CreateString("false"));
	}
	

	cJSON_AddItemToObject(root, "message", cJSON_CreateString(SUCCESS_MESSAGE));
	
	char *nBuffString =  cJSON_Print(root);
	JSON_INFO("INFO: %s\n", nBuffString);
	
	memset(nBuffer, 0, nBufferLenth);
	strncpy(nBuffer, nBuffString, nBufferLenth);

	cJSON_Delete(root);
	root = NULL;
	

	return nRet;
}

/*@breif Json_4GExternalCardTest 获取内置卡产测数据
 *@param 
 *@return 成功返回 0
 *@       失败返回-1
 */
int Json_4GBuiltCardTest(char *nBuffer,int nBufferLenth)
{
	int nRet = 0;
    int iUcWorkMode = 0;
	Module4G_ProductTest_Data_t ProductTestData = {0};

	if(NULL == nBuffer)
	{
		JSON_ERROR("nBuffer is NULL, GetSdInfo fail\n");
		return -1;
	}

	nRet = MPU_Net_Get4GProductTestData(&ProductTestData);

    MPU_Net_GetUcWorkMode(&iUcWorkMode);
    if(1 == iUcWorkMode)
    {
        nRet = ProductTest4GDataToJson(nBuffer, &ProductTestData, nBufferLenth,nRet,0);
        JSON_ERROR("cloud card mode,not exist InternalCard!!\n");
    }
	else
	{
		nRet = ProductTest4GDataToJson(nBuffer, &ProductTestData, nBufferLenth,nRet,1);
	}
	
	return nRet; 
}

/*@breif Json_4GExternalCardTest 获取外置卡产测数据
 *@param 
 *@return 成功返回 0
 *@       失败返回-1
 */
int Json_4GExternalCardTest(char *nBuffer,int nBufferLenth)
{
	int nRet = 0;
    int iUcWorkMode = 0;
	Module4G_ProductTest_Data_t ProductTestData = {0};

	if(NULL == nBuffer)
	{
		JSON_ERROR("nBuffer is NULL, GetSdInfo fail\n");
		return -1;
	}

	nRet = MPU_Net_Get4GProductTestData(&ProductTestData);
    MPU_Net_GetUcWorkMode(&iUcWorkMode);
    if(1 == iUcWorkMode)
    {
        JSON_ERROR("cloud card mode,not exist ExternalCard!!\n");
        return -1;
    }
	nRet = ProductTest4GDataToJson(nBuffer, &ProductTestData, nBufferLenth,nRet,0);

	return nRet; 
}

int Json_4GCardInfo(char *nBuffer,int nBufferLenth, int nTestType)
{
	int nRet = 0;
	Json4GCardInfo pcfg;

	if(NULL == nBuffer)
	{
		JSON_ERROR("nBuffer is NULL, 4GCardInfo fail\n");
		return -1;
	}
	memset(&pcfg, 0, sizeof(Json4GCardInfo));

	nRet = MPU_NET_GetSimCardInfo(pcfg.szIMSI, 
                                    pcfg.szIMEI, 
                                    pcfg.szACT, 
                                    pcfg.szOperator, 
                                    pcfg.szBand,
                                    pcfg.szICCID,
                                    pcfg.Version);
    if (-1 == nRet)
    {    
		JSON_ERROR("Fail to called MPU_NET_GetSimCardInfo\n");
		return -1;
    }

	nRet = SimCardInfoToJson(nBuffer, nBufferLenth, &pcfg);

	return nRet; 
}



int Json_4GSignalQuality(char *nBuffer,int nBufferLenth, int nTestType)
{
	int nRet = 0;
	int nSignalQuality = 0;

	if(NULL == nBuffer)
	{
		JSON_ERROR("nBuffer is NULL, 4GSignalQuality fail\n");
		return -1;
	}

	nRet = MPU_Net_GetGPRSModuleSignalQuality(&nSignalQuality);
	if (-1 == nRet)
	{    
		JSON_ERROR("Fail to called MPU_Net_GetGPRSModuleSignalQuality\n");
		return nRet;
	}


	nRet = GPRSSignalQualityToJson(nBuffer, nBufferLenth,nSignalQuality);

	return nRet;
}


#endif



