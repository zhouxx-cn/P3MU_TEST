

#ifndef __CPROMANAGER_H__
#define __CPROMANAGER_H__

#include <stdio.h>

typedef enum
{
	eStartTest = 0,					//打开测试模式
    eCmdSdInfo,						//获取sd卡信息
	eRedLightTest,					//红光灯
	eWhiteLightTest,				//白光灯
	eWifiAccessPointList,			//wifi接入点获取
	eWifiTestStatue,				//wifi连接测试
	eTryListemAlarmAudio,			//声音报警，声音测试	
	ePtzControl,					//PTZ控制
	eAlarmOut,						//报警输出
	eSdFormat,						//sd卡格式化
	eFactoryDefault,				//完全恢复
	eAdcStatus,						//光敏检测
	eWifiParamTest,					//wifi配置设置检测
	eHardReset,						//硬复位测试
	e4GBulitInCard,					//内置卡
	e4GExternalCard,				//外置卡
	ePowerDetectVoltage,			//电源检测电压
	ePowerVoltage,					//电源电压
	e4GCardInfo,					//4G卡信息获取
	e4GSignalQuality,				//4G信号获取
	eNETParameters,					//设备网络使用信息
	eSensorCntCorrect,				//镜头数检测
	eKeyToCall,						//一键呼叫按键测试
	eKeyToCallHangup,				//一键呼叫挂断按键测试
	eHighPriorityTest,              //高优先级测试
	eVersion,						//版本检测
	eBlueStatus,                    //蓝牙测试	
	eOtherTest = 255,				//其它测试
}eMainCmdKey;

int SetProductToolCmd(char *pBuffer);

int GetProductToolCmd(char *pBuffer, int BufferLen);



#endif
