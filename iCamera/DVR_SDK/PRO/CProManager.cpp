#include "CProManager.h"
#include "cJSON.h"
#include "string.h"
#include "CJsonStorage.h"
#include "CJsonNet.h"
#include "CJsonAlarm.h"
#include "CJsonLightTest.h"
#include "CJsonSys.h"
#include "CJsonDap.h"
#include "CJsonPtzControl.h"
#include "./MPU/MPU_global.h"
#include "CJsonPub.h"
// #include "sys_banned.h"
#include "global_gather.h"


#define CHECK_POINT_NULL_SPECIAL_RETURN_VALUE_WITH_STR(x,return_value,str)\
{\
  if(NULL == x)\
  {\
    JSON_ERROR(str);  \
    return return_value;\
  }\
}

#define STR_SAFE_CPY(dst,src) strncpy(dst,src,sizeof(dst) - 1)

typedef enum tagJsonRespondError
{
	eFormatErr = 0,
	eCmdInfoErr,
	eRespondJsonErr,
}eJsonRespondErrorType;

typedef struct tagMainCmdInfo
{
	eMainCmdKey   nMainCmdKey;
	char nMainCmdString[16];
}MainCmdInfo;


typedef struct tagDevCmdParam
{
	int nOperateMethod;
	int nMainCmd;
	int nAssistCmd;	
}DevCmdParam;



static MainCmdInfo s_nMainCmdInfo[MAXMAINCMD] =
	{
		{eStartTest,           "00000000"},
		{eCmdSdInfo,           "00000005"},
	    {eWifiAccessPointList, "00000006"},
		{eWifiTestStatue,      "00000007"},
		{eRedLightTest,        "00000009"},
		{eWhiteLightTest,      "00000010"},
		{eTryListemAlarmAudio, "00000012"},
		{ePtzControl,          "00000013"},
		{eAlarmOut,            "00000015"},
		{eTryListemAlarmAudio, "00000016"},
		{eSdFormat,            "00000021"},
		{eFactoryDefault,      "00000023"},
		{eAdcStatus,           "00000024"},
		{eWifiParamTest,       "00000026"},
		{eHardReset,           "00000022"},
		{e4GBulitInCard,       "00000018"},
		{e4GExternalCard,      "00000019"},
		{ePowerDetectVoltage,  "00000030"},
	    {ePowerVoltage,        "00000033"},
	    {e4GCardInfo,          "00000017"},
	    {e4GSignalQuality,     "00000020"},
		{eNETParameters,       "00020001"},
		{eSensorCntCorrect,    "00000056"},
		{eHighPriorityTest,    "00000042"},
		{eKeyToCall,    	   "00000032"},
		{eKeyToCallHangup,     "00000071"},
		{eVersion,			   "00000003"},
		{eBlueStatus,		   "00000040"},
    };

//static const char *s_content = "Content";
//static const char *s_head  = "Head";
static char s_RespondBuff[RESPONDMAXLENTH] = {0};
static char s_szFirstParam[FIRSTPARAMLENTH] ={0};
static char s_szSecParam[SECPARAMLENTH] = {0};


//!\brief 开始测试命令
//!\return 成功为0,否则为-1
//!\param nBuffer 命令buff  nBufferLenth buff长度, nStatus 测试命令状态
static int Json_StartTest(char *nBuffer,int nBufferLenth, int nStatus)
{
	if(NULL == nBuffer)
	{
		JSON_ERROR("nBuffer is NULL, pls check!");
		return -1;
	}

	cJSON * root =  cJSON_CreateObject(); 
	if(!root)
	{
		JSON_ERROR("create Json error, please try again\n");
		return -1;
	}

	if(0 == nStatus)
	{
		cJSON_AddItemToObject(root, "code", cJSON_CreateNumber(JSON_SUCCESS_CODE));
	}
	else
	{
		cJSON_AddItemToObject(root, "code", cJSON_CreateNumber(JSON_SUCCESS_CODE));
	}
	

	cJSON * nData =  cJSON_CreateObject();
	if(!nData)
	{
		cJSON_Delete(root);
		JSON_ERROR("create root Json error, please try again\n");
		return -1;
	}
	cJSON_AddItemToObject(root, "data", nData);

	if(0 == nStatus)
	{
		cJSON_AddItemToObject(root, "message", cJSON_CreateString(SUCCESS_MESSAGE));
	}
	else
	{
		cJSON_AddItemToObject(root, "message", cJSON_CreateString(TEST_MODE_FAIL));
	}
	

	char *nBuffString =  cJSON_Print(root);
	JSON_INFO("INFO: %s\n", nBuffString);
	
	memset(nBuffer, 0, nBufferLenth);
	strncpy(nBuffer, nBuffString, nBufferLenth - 1);

	cJSON_Delete(root);
	root = NULL;

	free(nBuffString);
	nBuffString = NULL;

	return 0;
}


//!\brief 错误JSON回复
//!\return 成功为0,否则为-1
//!\param nErrNum 错误编号 nErrBuff 错误内容  nBuffer 回复buff nBufferLenth 回复buff长度
static int CreateErrRespond(int nErrNum, const char *nErrBuff,char *nBuffer, int nBufferLenth)
{
	if(NULL == nBuffer)
	{
		JSON_ERROR("nBuffer is NULL, pls check!");
		return -1;
	}
	cJSON * root =  cJSON_CreateObject(); 
	if(!root)
	{
		JSON_ERROR("create Json error, please try again\n");
		return -1;
	}

	cJSON_AddItemToObject(root, "code", cJSON_CreateNumber(nErrNum));
	cJSON_AddItemToObject(root, "message", cJSON_CreateString(nErrBuff));
	
	char *nBuffString =  cJSON_Print(root);
	JSON_INFO("INFO: %s\n", nBuffString);
	
	memset(nBuffer, 0, nBufferLenth);
	strncpy(nBuffer, nBuffString, nBufferLenth - 1);

	cJSON_Delete(root);
	root = NULL;

	free(nBuffString);
	nBuffString = NULL;

	return 0;
}


//!\brief 解析命令
//!\return 成功为0,否则为-1
//!\param json_root  json句柄， nDevCmdParam，解析出来的数据
static int PraseCmdToDev(cJSON * json_root, DevCmdParam *nDevCmdParam)
{
	 int i = 0;
	 int nOperateMethod = -1;
	 int nMainCmdKey    = -1;
	 int nAssistCmdKey  = -1;
/*	 
	 cJSON * JsonContent = cJSON_GetObjectItem(json_root,s_content);

    CHECK_POINT_NULL_SPECIAL_RETURN_VALUE_WITH_STR(JsonContent, -1,"Content is NULL\n");

	 cJSON * JsonHead = cJSON_GetObjectItem(JsonContent,s_head); 
	 CHECK_POINT_NULL_SPECIAL_RETURN_VALUE_WITH_STR(JsonHead, -1,"Head is NULL\n");
*/	 
	 cJSON * JsonOperate = cJSON_GetObjectItem(json_root,"Operate");

	 CHECK_POINT_NULL_SPECIAL_RETURN_VALUE_WITH_STR(JsonOperate,-1,"JsonOperate is NULL");

	 cJSON_CheckObjectTypeWithReturn(JsonOperate,cJSON_String,-2);

	 if (0 == strcmp(JsonOperate->valuestring, "GET"))
	 {
		nOperateMethod = 0;
	 }
	 else if(0 == strcmp(JsonOperate->valuestring, "SET"))
	 {
        nOperateMethod = 1;
	 }
	 

     cJSON * JsonMainCmd = cJSON_GetObjectItem(json_root,"MainCmd");
	 CHECK_POINT_NULL_SPECIAL_RETURN_VALUE_WITH_STR(JsonMainCmd,-1,"JsonMainCmd is NULL");
	
	 cJSON_CheckObjectTypeWithReturn(JsonMainCmd,cJSON_String,-2);
	 for(i = 0; i < MAXMAINCMD; i++)
	 {
		if(0 == strcmp(JsonMainCmd->valuestring, s_nMainCmdInfo[i].nMainCmdString))
		{
			nMainCmdKey = s_nMainCmdInfo[i].nMainCmdKey;
			break;
		}
	 }


	 cJSON * JsonAssistCmd = cJSON_GetObjectItem(json_root,"AssistCmd");

	 if(NULL != JsonAssistCmd)
	 {
		 cJSON_CheckObjectTypeWithReturn(JsonAssistCmd,cJSON_String,-2);
		 
     }

	 cJSON *JsonFirstParam = cJSON_GetObjectItem(json_root, "Param1");
	 if(NULL != JsonFirstParam)
	 {
	 	 if (e4GExternalCard == nMainCmdKey || e4GBulitInCard == nMainCmdKey || ePowerDetectVoltage == nMainCmdKey || eSensorCntCorrect == nMainCmdKey)
	 	 {
			 memset(s_szFirstParam, 0, sizeof(s_szFirstParam));
			 snprintf(s_szFirstParam,sizeof(s_szFirstParam),"%d",JsonFirstParam->valueint);
	 	 }
		 else if(ePowerVoltage == nMainCmdKey)
		 {
			 snprintf(s_szFirstParam,sizeof(s_szFirstParam),"%f",JsonFirstParam->valuedouble);
		 }
	 	 else
	 	 {
			 cJSON_CheckObjectTypeWithReturn(JsonFirstParam,cJSON_String,-2);
			 strncpy(s_szFirstParam, JsonFirstParam->valuestring, sizeof(s_szFirstParam));
		 }
	 }

	 cJSON *JsonSecParam = cJSON_GetObjectItem(json_root, "Param2");
	 if(NULL != JsonSecParam)
	 {
		if (ePowerVoltage == nMainCmdKey)
		{
			snprintf(s_szSecParam,sizeof(s_szSecParam),"%f",JsonSecParam ->valuedouble);
		}
		else
		{
			cJSON_CheckObjectTypeWithReturn(JsonSecParam ,cJSON_String,-2);
			strncpy(s_szSecParam, JsonSecParam ->valuestring, sizeof(s_szSecParam));
		}
	 }

	if((-1 == nOperateMethod) || (-1 == nMainCmdKey))
	{
		JSON_ERROR("nOperateMethod [%d] or nMainCmdKey[%d] is error, please check it\n", nOperateMethod, nMainCmdKey);
		return -1;	
	}

	nDevCmdParam->nOperateMethod = nOperateMethod;
	nDevCmdParam->nMainCmd = nMainCmdKey;
	nDevCmdParam->nAssistCmd = nAssistCmdKey;
	
	JSON_INFO("INFO: OperateMethod = %d,nMaincmd = %d, nAssistCmd = %d\n",  \
		  nDevCmdParam->nOperateMethod,      \
		  nDevCmdParam->nMainCmd,            \
		  nDevCmdParam->nAssistCmd);

	return 0;
}

//!\brief 应答错误回复
//!\return 成功为0,否则为-1
//!\param type 应答错误类型
static int JsonErrorRespond(int type)
{
	int nRet = -1;
	switch (type)
	{
		case eFormatErr:
		{
			nRet = CreateErrRespond(JSON_FORMAT_ERROR, FORMAT_ERR_MESSAGE, s_RespondBuff, sizeof(s_RespondBuff));
		}
		break;
		case eCmdInfoErr:
		{
			nRet = CreateErrRespond(JSON_CMDINFO_ERROR, CMD_INFO_ERR_MESSAGE, s_RespondBuff,sizeof(s_RespondBuff));
		}
		break;
		case eRespondJsonErr:
		{
			nRet = CreateErrRespond(JSON_RESPOND_ERROR, RESPOND_JSON_ERR, s_RespondBuff,sizeof(s_RespondBuff));
		}
		break;
		default:
			JSON_ERROR("nMaincmd error nMaincmd = %d\n", type);
			break;
	}
	return nRet;
}


static int ProductToolSetConfig(int nMaincmd, int nAssistCmd)
{
	int nRet = 0;
	switch (nMaincmd)
	{
    #if defined (__IS_SUPPORT_WLAN__)
		case eWifiParamTest:
		{
			nRet = Json_WifiParamTest(s_RespondBuff, sizeof(s_RespondBuff), s_szFirstParam, s_szSecParam);
		}
		break;
    #endif
    
	#ifdef __MODULE_4G_SUPPORT__
    	case e4GBulitInCard:
		{
			//获取内置卡产测数据
			nRet = Json_4GBuiltCardTest(s_RespondBuff, sizeof(s_RespondBuff));
		}
		break;
		case e4GExternalCard:
		{
			//获取外置卡产测数据
			nRet = Json_4GExternalCardTest(s_RespondBuff, sizeof(s_RespondBuff));
		}
		break;
	#endif
		default:
			JSON_ERROR("nMaincmd error nMaincmd = %d\n", nMaincmd);
			break;
	}
	
	return nRet;
}


//!\brief 应带重组命令
//!\return 成功为0,否则为-1
//!\param nMaincmd 主命令  nAssistCmd    次命令
static int ProductToolGetConfig(int nMaincmd, int nAssistCmd)
{
	int nRet = 0;
	switch (nMaincmd)
	{
		case eStartTest:
		{
			int  nStatus = 0;
			nStatus = MPU_SYS_SetProductTestMode(1);
            nStatus |= MPU_PTZ_SetProductTestMode(1);
            ZWSys_SetProductMode(1);
			if(access("/data/config/config/noSupportShare",F_OK) == 0)  //产测时会下发该命令，用于删除产测拉流时误增加的文件
			{
				printf("[%s:%d] ProductTest need detlet 'noSupportShare' \n",__FILE__, __LINE__);
				int RmRet = remove("/data/config/config/noSupportShare");
				if(RmRet != 0)
				printf("[%s:%d] ProductTest delet 'noSupportShare' fail [%d]!\n",__FILE__, __LINE__,RmRet);
			}	
			nRet = Json_StartTest(s_RespondBuff, sizeof(s_RespondBuff), nStatus);
		}
		break;
		case eHardReset:
		{
			nRet = Json_HardResetTest(s_RespondBuff, sizeof(s_RespondBuff), eHardReset);
		}
		break;
		case eCmdSdInfo:
		{
			nRet = Json_GetSdInfo(s_RespondBuff, sizeof(s_RespondBuff), eCmdSdInfo);
		}
		break;
        #if defined (__IS_SUPPORT_WLAN__)
        case eWifiTestStatue:
		{
			nRet = Json_GetWifiTestStatue(s_RespondBuff, sizeof(s_RespondBuff));
		}
		break;
		case eWifiAccessPointList:
		{
			nRet = Json_GetWifiAccessPointList(s_RespondBuff, sizeof(s_RespondBuff));
		}
		break;
        #endif
		case eTryListemAlarmAudio:
		{
			nRet = Json_TryListenAlaramAudio(s_RespondBuff, sizeof(s_RespondBuff), eTryListemAlarmAudio);
		}
		break;
		case ePtzControl:
		{
			nRet = Json_PtzControlTest(s_RespondBuff, sizeof(s_RespondBuff), ePtzControl);
		}
		break;
		case eRedLightTest:
		{
			nRet = Json_RedLightTest(s_RespondBuff, sizeof(s_RespondBuff), eRedLightTest);
		}
		break;
		case eWhiteLightTest:
		{
			nRet = Json_WhiteLightTest(s_RespondBuff, sizeof(s_RespondBuff), eWhiteLightTest);
		}
		break;
		case eAlarmOut:
		{
			nRet = Json_AlarmOut(s_RespondBuff, sizeof(s_RespondBuff), eAlarmOut);
		}
		break;
		case eSdFormat:
		{
			nRet = Json_SdFormat(s_RespondBuff, sizeof(s_RespondBuff), eSdFormat); 
		}
		break;
		case eFactoryDefault:
		{
			nRet = Json_FactoryDefault(s_RespondBuff, sizeof(s_RespondBuff), eFactoryDefault);
		}
		break;
		case eAdcStatus:
		{
			nRet = Json_AdcStatus(s_RespondBuff, sizeof(s_RespondBuff), eAdcStatus);
		}
		break; 
    #ifdef __MODULE_4G_SUPPORT__     
        case e4GCardInfo:
		{
			nRet = Json_4GCardInfo(s_RespondBuff, sizeof(s_RespondBuff), e4GCardInfo);
		}
		break;
		case e4GSignalQuality:
		{
			nRet = Json_4GSignalQuality(s_RespondBuff, sizeof(s_RespondBuff), e4GSignalQuality);
		}
		break;
    #endif
		case eNETParameters:
		{
			nRet = Json_NETParameters(s_RespondBuff, sizeof(s_RespondBuff));
		}
		break;
        case eSensorCntCorrect:
        {
            nRet = Json_SensorCntCorrect(s_RespondBuff, sizeof(s_RespondBuff), s_szFirstParam, sizeof(s_szFirstParam));
        }
		break;
		case eHighPriorityTest:
		{
			nRet = Json_HighPrioritySupport(s_RespondBuff, sizeof(s_RespondBuff));
		}
		break;
#ifdef __ONE_CLICK_CALL__
		case eKeyToCall:
		case eKeyToCallHangup:
		{
			nRet = Json_HardKeyTocallTest(s_RespondBuff, sizeof(s_RespondBuff), nMaincmd);
		}
		break;
#endif

		case eVersion:
		{
			nRet = Json_Version(s_RespondBuff, sizeof(s_RespondBuff));
		}
		break;
		case eBlueStatus:
		{
			nRet = Json_BlueStatus(s_RespondBuff, sizeof(s_RespondBuff));
		}
		break;

		default:
			JSON_ERROR("nMaincmd error nMaincmd = %d\n", nMaincmd);
			break;
	}
	return nRet;
}


static int OperateCmd(DevCmdParam *nDevCmdParam)
{
    int nRet  = 0;
	
	if(nDevCmdParam->nOperateMethod)
	{
		nRet = ProductToolSetConfig(nDevCmdParam->nMainCmd, nDevCmdParam->nAssistCmd);
	}
	else
	{
		nRet = ProductToolGetConfig(nDevCmdParam->nMainCmd, nDevCmdParam->nAssistCmd);
	}

	return nRet;
}



//!\brief 下发命令解析
//!\return 成功为0,否则为-1
//!\param pBuffer为透传下的数据
int SetProductToolCmd(char *pBuffer)
{
	int nRet  = 0;
	cJSON * root = NULL;
	DevCmdParam nDevCmdParam = {0};

	JSON_INFO("INFO: pBuffer: %s\n", pBuffer);

// 第一步， 解析命令
	JSON_INFO("======ProductTool test first step=====\n");
	root = cJSON_Parse(pBuffer);
	if(!root)
	{
		JSON_ERROR("Error before: [%s]\n",cJSON_GetErrorPtr());
		nRet = JsonErrorRespond(eFormatErr);
		return nRet;
	}
	JSON_INFO("======ProductTool test second step=====\n");

//第二步，解析命令，获取需要执行的指令
	nRet = PraseCmdToDev(root, &nDevCmdParam);
	cJSON_Delete(root);
	if(0 != nRet)
	{
		JSON_ERROR("Prase cmd error\n");
		nRet = JsonErrorRespond(eCmdInfoErr);
		return nRet;
	}
	JSON_INFO("======ProductTool test third step=====\n");
	
//执行命令，形成应答字符串
    nRet = OperateCmd(&nDevCmdParam);
	if(nRet)
	{
		JSON_ERROR("Operate cmd error\n");
		nRet = JsonErrorRespond(eRespondJsonErr);
		return nRet;
	}

	JSON_INFO("======ProductTool test over, success!=====\n");
	return 0;
}

//!\brief 回复命令应答
//!\return 成功为0,否则为-1
//!\param pBuffer为透传应答的数据
int GetProductToolCmd(char * pBuffer, int BufferLen)
{
	if(NULL == pBuffer)
	{
		return -1;
	}
	strncpy(pBuffer, s_RespondBuff, BufferLen - 1);

	return 0;
}
