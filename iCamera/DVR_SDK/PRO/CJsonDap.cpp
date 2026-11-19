

#include <stdio.h>
#include "CJsonDap.h"
#include "CJsonPub.h"
// #include "sys_banned.h"
#include "global_gather.h"

int Json_DapRedLightTest(char *nBuffer , int nBufferLenth)
{
	int nRet = 0;
	nRet = CJsonPubRespond(nBuffer, nBufferLenth, 1, eOtherTest);//当前没有该测试类型，先根据CProManager中定义的其它类型的值给，后面再改
	return nRet;
}

int Json_DapWhiteLightTest(char *nBuffer , int nBufferLenth)
{
	int nRet = 0;
	nRet = CJsonPubRespond(nBuffer, nBufferLenth, 1, eOtherTest);
	return nRet;
}
	
int Json_DapGreenLightTest(char *nBuffer , int nBufferLenth)
{
	int nRet = 0;
	nRet = CJsonPubRespond(nBuffer, nBufferLenth, 1, eOtherTest);
	return nRet;
}

int Json_DapCameraSet(char *nBuffer , int nBufferLenth)
{
	int nRet = 0;
	nRet = CJsonPubRespond(nBuffer, nBufferLenth, 1, eOtherTest);
	return nRet;
}

int  Json_DapAlarmout(char *nBuffer , int nBufferLenth)
{
	int nRet = 0;
	nRet = CJsonPubRespond(nBuffer, nBufferLenth, 1, eOtherTest);
	return nRet;
}

int Json_DapDoorLock(char *nBuffer , int nBufferLenth)
{
	int nRet = 0;
	nRet = CJsonPubRespond(nBuffer, nBufferLenth, 1, eOtherTest);
	return nRet;
}




