#ifndef __ZW_RTMP_H__

#include "zwrtmp_def.h"

#ifdef __cplusplus
extern "C" {
#endif 
// 初始化
int ZW_RTMP_Init();

// 反初始化
int ZW_RTMP_Uninit();

//　创建推流
int ZW_RTMP_Client_Create(unsigned int* handle,ZW_RTMP_CLIENT_CONNECT_PARAM* param, ZWRTMP_CB_Working_Status cb, void* dwUserData, char* szStreamIdentity=0);

//　推流
int ZW_RTMP_Client_Inputdata(unsigned int handle, unsigned char* buffer, int bufLen);

//  播流地址
int ZW_RTMP_Client_GetConnectParam(unsigned int handle, ZW_RTMP_CLIENT_CONNECT_PARAM* param);

//　销毁
int ZW_RTMP_Client_Destory(unsigned int handle);

#ifdef __cplusplus
}
#endif

#endif