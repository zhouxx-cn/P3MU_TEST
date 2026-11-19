
/*!
 *\file		MPU_CIU_API.cpp
 *\brief	MPU模块，管理远程访问相关接口实现
 *\author	林云
 *\date		2009/03/20

功能：
-#	远程服务端口配置
-#	获取在线网络客户端数量、信息（用户、IP地址、物理地址等）
-#	管理网络客户连接（断开连接、禁止IP登录
-#	远程升级（显示进度、取消升级）
-#	响应远程重启、关机命令

说明：
-#	本文件只定义设备本地GUI需要干预的相关部分。
-#	对应网络客户端访问设备，主要功能如远程预览、远程回放、远程参数配置等，由底层自动完成。
 */


#include "MPU_global.h"
#include "MPU_CIU_global.h"
/**************************************************************************
 *                            全局接口                                    *
 **************************************************************************/

void CIU_UpdatePortConfig()
{
	FILE *fp = fopen(CONFIG_HTML_PATH, "w");

	if (NULL != fp)
	{
		unsigned short wCommandPort = MPU_CIU_GetCommandPort();

		unsigned short wMobilePort = MPU_CIU_GetMobilePort();

		fprintf(fp, "[Network]\n");
		fprintf(fp, "CommandPort=%u\n", wCommandPort);
		fprintf(fp, "MobilePort=%u\n", wMobilePort);

		fclose(fp);
	}
	else
	{
		MPU_ERR("open %s fail\n", CONFIG_HTML_PATH);
	}
}


//!\brief       获取数据端口
//!\return  端口号
unsigned short MPU_CIU_GetMediaPort()
{
	unsigned short nPort = 6066;

	nPort = MPU_NET_GetMediaPort();

	return nPort;
}

//!\brief       设置数据端口
//!\param   nPort       端口号
//!\return  0表示成功，其它为错误码
int MPU_CIU_SetMediaPort(unsigned short nPort)
{
	int nRet;

	nRet = MPU_NET_SetMediaPort(nPort);
	if(1 == nRet)//返回1表示参数跟之前一致
		nRet = 0;

	return nRet;
}

//!\brief       获取命令端口
//!\return  端口号
unsigned short MPU_CIU_GetCommandPort()
{
	unsigned short nPort = 6060;

	nPort = MPU_NET_GetCommandPort();

	return nPort;
}

//!\brief       设置命令端口
//!\param   nPort       端口号
//!\return  0表示成功，其它为错误码
int MPU_CIU_SetCommandPort(unsigned short nPort)
{
	int nRet;

	nRet = MPU_NET_SetCommandPort(nPort);

	if(0 == nRet)
		CIU_UpdatePortConfig();
	
	if(1 == nRet)//返回1表示参数跟之前一致
		nRet = 0;
	
	return nRet;
}

//!\brief   获取rtp端口
//!\return  端口号
unsigned short MPU_CIU_GetRtpPort()
{
	unsigned short nPort = 6060;

	nPort = MPU_NET_GetRtspPort();

	return nPort;
}

//!\brief       设置rtp端口
//!\param   nPort       端口号
//!\return  0表示成功，其它为错误码
int MPU_CIU_SetRtpPort(unsigned short nPort, int *needReboot)
{
	int nRet;

	nRet = MPU_NET_SetRtspPort(nPort);

	if(0 == nRet)//修改成功
		*needReboot = 1;
	else
		if(1 == nRet)//端口跟之前一致不用修改
			nRet = 0;

	return nRet;
}

//!\brief   获取mobile端口
//!\return  端口号
unsigned short MPU_CIU_GetMobilePort()
{
	unsigned short nPort = 10000;

	nPort = MPU_NET_GetMobilePort();

	return nPort;
}

//!\brief       设置Mobile端口
//!\param   nPort       端口号
//!\return  0表示成功，其它为错误码
int MPU_CIU_SetMobilePort(unsigned short nPort)
{
	int nRet;

	nRet = MPU_NET_SetMobilePort(nPort);
	
	if(0 == nRet)
		CIU_UpdatePortConfig();

	if(1 == nRet)//返回1表示参数跟之前一致
		nRet = 0;

	return nRet;
}

//!\brief   获取AutoDetect端口
//!\return  端口号
unsigned short MPU_CIU_GetAutoDetectPort()
{
	unsigned short nPort = 5555;

	nPort = MPU_NET_GetAutoDetectPort();

	return nPort;
}

//!\brief       设置AutoDetect端口
//!\param   nPort       端口号
//!\return  0表示成功，其它为错误码
int MPU_CIU_SetAutoDetectPort(unsigned short nPort)
{
	int nRet;

	nRet = MPU_NET_SetAutoDetectPort(nPort);

	if(0 == nRet)
	{
		HieServer_DeviceProbeStop();
		HieServer_DeviceProbeStart();
	}
	
	if(1 == nRet)//返回1表示参数跟之前一致
		nRet = 0;

	return nRet;
}

//!\brief   获取HK Private端口
//!\return  端口号
unsigned short MPU_CIU_GetHKPrivatePort()
{
	unsigned short nPort = 8000;

	nPort = MPU_NET_GetHKPrivatePort();

	return nPort;
}

//!\brief       设置HK Private端口
//!\param   nPort       端口号
//!\return  0表示成功，其它为错误码
int MPU_CIU_SetHKPrivatePort(unsigned short nPort)
{
	int nRet;

	nRet = MPU_NET_SetHKPrivatePort(nPort);

	return nRet;
}

int MPU_CIU_GetClientNum()
{
	int i = 0;

	int iClientNum = 0;

	const char *pcUserName, *pcTime, *pcIpAddr;

	HANDLE hUser = NULL;

	for (i = 0; i < LOGINUSER_COUNT; i++)
	{
		if (0 == CIU_GetUserInfo(i, &hUser, &pcUserName, &pcTime, &pcIpAddr))
		{
			iClientNum++;
		}
	}
	return iClientNum;
}

//!\brief   获取指定的用户是否在线
//!\return  0 不在线，1在线
int MPU_CIU_IsOnline(HANDLE hUser)
{
	return CIU_IsOnline(hUser);
}

//!\brief       获取连接的客户端信息
//!\param  dwUserID   用户ID
//!\param  HANDLE *hUser 用户句柄地址
//!\param pcUerName   用户名
//!\param pcIPAddress   ip 地址
//!\param pcLoginTime  登陆时间
//!\return  0表示成功，其它为错误码
int MPU_CIU_GetRemoteByID(unsigned long dwUserID, HANDLE * phUser, char *pcUerName, int userNameSize, char *pcIPAddress,  int ipAddrSize, char *pcLoginTime, int loginTimeSize)
{
	const char *pcUserName, *pcTime, *pcIpAddr;

	HANDLE hUser;

	if (0 == CIU_GetUserInfo(dwUserID, &hUser, &pcUserName, &pcTime, &pcIpAddr))
	{
		if (phUser)
			*phUser = hUser;

		if (pcUerName)
			snprintf(pcUerName, userNameSize, "%s", pcUserName);

		if (pcIPAddress)
			snprintf(pcIPAddress, ipAddrSize, "%s", pcIpAddr);

		if (pcLoginTime)
			snprintf(pcLoginTime, loginTimeSize, "%s", pcTime);
		return 0;
	}
	else
	{
		if (phUser)
			*phUser = 0;

		if (pcUerName)
			pcUerName[0] = 0;

		if (pcIPAddress)
			pcIPAddress[0] = 0;

		if (pcLoginTime)
			pcLoginTime[0] = 0;
		return -1;
	}

}


//!\brief       判断 远程是否具有权限
//!\param dwUserID   远程用户ID
//!\param eAction   操作
//!\return  0表示无权限，1表示有权限
int MPU_CIU_CheckRemoteUser(DWORD dwUserID, REMOTE_ACTION eAction)
{
	return CIU_CheckRemoteUser(dwUserID, eAction);
}

//!\brief       判断 远程是否具有权限
//!\param dwUserID   远程用户ID
//!\param eAction   操作
//!\param nCh   通道
//!\return  0表示无权限，1表示有权限
int MPU_CIU_CheckRemoteUser(DWORD dwUserID, CHANNEL_ACTION eAction, int nCh)
{
	return CIU_CheckRemoteUser(dwUserID, eAction, nCh);
}

//!\brief   获取连接的客户端信息
//!\param   pClientInfo   客户端信息
//!\return  客户端个数
int MPU_CIU_GetClientInfo(PREMOTECLIENTINFO pClientInfo)
{
	int i = 0;

	int iClientNum = 0;

	const char *pcUserName, *pcIpAddr;

	DWORD dwTime;

	HANDLE hUser = NULL;

	eProtocolID eProtocol;

	for (i = 0; i < LOGINUSER_COUNT; i++)
	{
		if (0 == CIU_GetUserInfo(i, &hUser, &pcUserName, &dwTime, &pcIpAddr, &eProtocol))
		{
			snprintf(pClientInfo[iClientNum].szUserName, sizeof(pClientInfo[iClientNum].szUserName), "%s", pcUserName);
			snprintf(pClientInfo[iClientNum].szIPAddress, sizeof(pClientInfo[iClientNum].szIPAddress), "%s", pcIpAddr);
			pClientInfo[iClientNum].dwLoginTime = dwTime;
			pClientInfo[iClientNum].dwUserID = i;
			pClientInfo[iClientNum].eProtocol = eProtocol;
			//MPU_DBG("%s %s %s\n", pcUserName, pcIpAddr, pcTime);
			iClientNum++;
		}
	}
	if (iClientNum < LOGINUSER_COUNT)
	{
		for (i = iClientNum; i < LOGINUSER_COUNT; i++)
		{
			memset(&(pClientInfo[i]), 0, sizeof (REMOTECLIENTINFO));
		}
	}
	return iClientNum;
}

int MPU_CIU_DisconnectClient(int nClientID)
{
	MPU_INFO("nClientID  %d\n", nClientID);
	return HieServer_DeleteUserForce(nClientID);
}

//!\brief  获取是否启用IP 权限管理
//!\param   
//!\return  0 不启用，1启用
int MPU_CIU_GetIPRightEnable()
{
	return CIU_GetIsIPRightEnable();
}

//!\brief  设置是否启用IP 权限管理
//!\param    0 不启用，1启用
//!\return 
int MPU_CIU_SetIPRightEnable(int iIpRightEnable)



{
	return CIU_SetIsIPRightEnable(iIpRightEnable);
}

int MPU_CIU_GetIPEnableList(DWORD * pdwIPEnableList)
{
	return CIU_GetIPEnableList(pdwIPEnableList);
}

int MPU_CIU_ChangeIPEnable(DWORD dwIpAddr, int iEnable)
{
	return CIU_ChangeIPEnable(dwIpAddr, iEnable);
}

//!\brief  清除所有IP
//!\return  0成功
int MPU_CIU_ClearIPEnableList()
{
	return CIU_ClearIPEnableList();
}

int MPU_CIU_CancelUpdate()
{
	return CIU_StopRemoteUpdate();
}

int MPU_CIU_SetUpdateNotifyCallback(Upload_Callback pRemoteUploadCallBack)
{
	CIU_SetRemoteUploadCallBack(pRemoteUploadCallBack);
	return 0;
}

int MPU_CIU_SetTalkCallback(Talk_Callback pStopTalkCallBack)
{
	CIU_SetTalkCallBack(pStopTalkCallBack);
	return 0;
}

//!\brief       设置远程控制回调函数
//!\param   RemoteControl_Callback 回调函数
//!\param   unsigned long dwUserID 用户ID
//!\param   unsigned long dwOperation 1-重启,2-关机,3-待机
//!\return  0表示设置成功
int MPU_CIU_SetRemoteControlCallback(RemoteControl_Callback pRemoteControlCallBack)
{
	return CIU_SetRemoteControlCallback(pRemoteControlCallBack);
}

//!\brief       重启网络服务,为异步处理
//!\param   nSecond 延迟时间
//!\return  0表示设置成功
int MPU_CIU_ResetService(int nSecond)
{
	return CIU_ResetServer(nSecond);
}

//!\brief    工厂测试用 开启语音对讲
void MPU_CIU_StartLocalVoice(DWORD dwChannel)
{
	CIU_StartLocalVoice(dwChannel);
}


//!\brief   工厂 测试用 停止语音对讲
void MPU_CIU_StopLocalVoice(DWORD dwChannel)
{
	CIU_StopLocalVoice(dwChannel);
}


LPHY_DVR_PTZ_ELCTRONIC GetPtzEletronicConfig();
//获取云台配置信息，需外部将pcfg清零
LPHY_DVR_PTZ_ELCTRONIC MPU_CIU_GetPtzEletronicConfig()
{
	return GetPtzEletronicConfig();
}

void MPU_CIU_SetStandardChangeCallback(int (*pStandardChangeCallBack) (int newStandard))
{
	CIU_SetStandardChangeCallback(pStandardChangeCallBack);
}


