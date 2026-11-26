
#include <stdio.h>
#include <string.h>
#include "cJSON.h"
#include "zwsdk.h"
#include "CJsonPub.h"
#include "global_gather.h"

int CJsonPubRespond (char *nBuffer, int nBufferLenth, int nStatus, int nTestType)
{
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

	//默认是白天状态为1，避免还没有遮光敏，工具已经提示产测成功
	if (eAdcStatus == nTestType)
	{
		if(nStatus)
		{
			cJSON_AddItemToObject(nData, "isPass", cJSON_CreateString("false"));
		}
		else
		{
			cJSON_AddItemToObject(nData, "isPass", cJSON_CreateString("true"));
		}
	}
	else
	{
		if(nStatus)
		{
			cJSON_AddItemToObject(nData, "isPass", cJSON_CreateString("true"));
		}
		else
		{
			cJSON_AddItemToObject(nData, "isPass", cJSON_CreateString("false"));
		}
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

//重新获取一遍版本信息，定制隐藏日期后，通过这个接口来获取新的版本信息
int Json_Version(char *nBuffer, int nBufferLenth)
{
    /* TODO */
    JSON_ERROR("Json_Version fail\n");
    return -1;
}

