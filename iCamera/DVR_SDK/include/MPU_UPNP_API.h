/*!
 *\file	MPU_UPNP_API.h
 *\brief	MPU模块，UPNP服务相关接口定义
 *\author	邵慰
 *\date	2011/03/01

功能：
-#	UPNP 服务开启
-#	获取UPNP 设置列表
-#	
 */


#ifndef _INC_MPU_UPNP_API_H_
#define _INC_MPU_UPNP_API_H_

//!\brief   设置是否启用UPNP
//!\param   bEnable     true表示启用，false表示禁用
//!\return  0表示成功，其它为错误码
int MPU_UPNP_SetEnable(bool bEnable);

//!\brief   查询是否启用UPNP
//!\return  TRUE 表示启用，FALSE 表示不启用
BOOL MPU_UPNP_GetEnable();

//!\brief   查询路由器WAN IP
//!\param   ip   记录路由器wan ip
//!\return  0表示成功，其它为错误码
int MPU_UPNP_QUERY_EXTERNAL_IP(char *ip, int nBufferLen);

//!\brief   查询路由器上的UPNP 映射列表，只显示与本机MAC 相符合的
//!\param   pMapList   映射列表0-http 1-msg 2-media 3- mobile
//!\return  返回值无意义，只根据返回的数组值进行判定
int MPU_UPNP_QUERY_MAPLiST(WORD pMapList[]);

int MPU_UPNP_GET_MAPSTATE(WORD pMapList[], WORD desMapList[], unsigned int *state);

//!\brief   添加UPNP映射
//!\param   pMapList   映射列表0-http 1-msg 2-media 3- mobile
//!\return  0表示成功，其它为错误码
int MPU_UPNP_ADD_PORTMAP(WORD pMapList[], WORD desMapList[]);



#endif 
