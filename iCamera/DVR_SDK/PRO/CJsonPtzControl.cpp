#if 1

#include <stdio.h>
#include <string.h>
#include "CJsonPtzControl.h"
#include "cJSON.h"
#include "CJsonPub.h"
#include "CConfig.h"
#include "PTZ/CPtzManager.h"
#include "motoLib.h"
#include "MPU_PTZ_API.h"
// #include "sys_banned.h"
#include "global_gather.h"


static void *PtzControlTest(void *parm)
{
#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
    PRT_TID();
	pthread_detach(pthread_self());
	JSON_INFO("PTZCONTROL TEST!!!\n");

	int nHorizontalStep = 0;
	int nVerticalStep = 0;
	int nHorizontalAngle = 0;
	int nVerticalAngle = 0;
	int iMoto_Status = 0;
	int counter = 0;
	CPtzManager *pPtz = NULL;
	
	CConfig *pConfig = CConfig::GetInstance();	
	pPtz = CPtzManager::PTZ_GetInstance();
	
	nHorizontalStep = pConfig->GetValue("PTZ_Set_0", "HorizVerticalStepontalStep", 396);
	nVerticalStep = pConfig->GetValue("PTZ_Set_0", "", 128);
	nHorizontalAngle = pConfig->GetValue("PTZ_Set_0", "HorizontalAngle", 198);
	nVerticalAngle = pConfig->GetValue("PTZ_Set_0", "VerticalAngle", 0);

	MPU_PTZ_SetSpeed(10, 0);

	MOTO_ToPos(nHorizontalStep, nVerticalStep);

	while (1)
	{
	    sleep(1);

		pPtz->PTZ_IsMotoStatus(&iMoto_Status);

		if(0 == iMoto_Status)
		{
			break;
		}
	    
	    if (++counter > 30)
	    {
	        counter = 0;
	        break;
	    }
	}


	MOTO_ToPos(0, nVerticalAngle);

	while (1)
	{
	    sleep(1);

		pPtz->PTZ_IsMotoStatus(&iMoto_Status);

		if(0 == iMoto_Status)
		{
			break;
		}
	    
	    if (++counter > 30)
	    {
	        counter = 0;
	        break;
	    }
	}

	MOTO_ToPos(0, nVerticalStep);

	while (1)
	{
	    sleep(1);

		pPtz->PTZ_IsMotoStatus(&iMoto_Status);

		if(0 == iMoto_Status)
		{
			break;
		}
	    
	    if (++counter > 30)
	    {
	        counter = 0;
	        break;
	    }
	}


	MOTO_ToPos(0, 0);

	while (1)
	{
	    sleep(1);

		pPtz->PTZ_IsMotoStatus(&iMoto_Status);

		if(0 == iMoto_Status)
		{
			break;
		}
	    
	    if (++counter > 30)
	    {
	        counter = 0;
	        break;
	    }
	}

	MOTO_ToPos(nHorizontalAngle, nVerticalAngle);

#endif
	pthread_exit(NULL);
}

int Json_PtzControlTest(char *nBuffer, int nBufferLenth, int nTestType)
{
	int nRet = 0;
	pthread_t m_PtzThread;

	if(NULL == nBuffer)
	{
		JSON_ERROR("nBuffer is NULL, pls check!\n");
		return -1;
	}


	pthread_create(&m_PtzThread, NULL, PtzControlTest, NULL);


	//MOTO_ToPosExt(nHPos, nVPos);

	nRet = CJsonPubRespond(nBuffer, nBufferLenth, 1, nTestType);

	return nRet;
}


#endif
