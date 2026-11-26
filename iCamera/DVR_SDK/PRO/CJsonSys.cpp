
#include <stdio.h>
#include "CJsonSys.h"
#include "CJsonPub.h"
#include "common_pub.h"
// #include "sys_banned.h"
#include "global_gather.h"
#include "global_macro.h"

static void *RestoreFactoryTest(void *parm)
{
    PRT_TID();

    /* TODO */
    JSON_ERROR("Json RestoreFactoryTest fail\n");
    return NULL;
}

int Json_FactoryDefault(char *nBuffer , int nBufferLenth, int nTestType)
{
	int nRet = 0;
	pthread_t m_Thread;
	
	if(NULL == nBuffer)
	{
		JSON_ERROR("nBuffer is NULL, FactoryDefault fail\n");
		return -1;
    }

    //异步处理，让网络回应
    pthread_create(&m_Thread, NULL, RestoreFactoryTest, NULL);

    nRet = CJsonPubRespond(nBuffer, nBufferLenth, 1, nTestType);

    return nRet;
}


int Json_HardResetTest(char *nBuffer , int nBufferLenth, int nTestType)
{
    /* TODO */
    JSON_ERROR("Json_HardResetTest fail\n");
    return -1;
}
