

#include <stdio.h>
#include "cJSON.h"
#include "string.h"
#include "CJsonStorage.h"
#include "newhie/HieServer.h"
#include "CJsonPub.h"
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

    /* 格式化SD卡 */
    if (nRet)
    {
        JSON_ERROR("FormatPartition  failed<%d>\n", nRet);
        return -1;
    }

    nRet = CJsonPubRespond(nBuffer, nBufferLenth, 1, nTestType);
    return nRet;
}
