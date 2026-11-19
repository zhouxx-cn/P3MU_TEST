
/*!
 *\file		tagRemoteClientInfo.h
 *\brief		MPU模块，远程接入客户端信息
 *\author	林云
 *\date		2009/05/18

 */


#ifndef _INC_TAG_REMOTE_CLIENT_INFO_H_
#define _INC_TAG_REMOTE_CLIENT_INFO_H_

#include "global_macro.h"
#include "newhie/HieServer_Common.h"
typedef struct tagRemoteClientInfo
{
	//!\brief   用户名
	char szUserName[MAX_USERNAME_LEN];

	//!\brief   远程客户端IP
	char szIPAddress[16];

	//!\brief   登陆时间
	//char szLoginTime[30];
	unsigned long dwLoginTime;

	//!\brief   用户ID
	unsigned long dwUserID;

	//!\brief   登陆协议
	eProtocolID eProtocol;
}
REMOTECLIENTINFO, *PREMOTECLIENTINFO;


#endif
