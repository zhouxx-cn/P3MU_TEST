/*!
 *\file		MPU_UPNP_API.cpp
 *\brief		MPU 模块，UPNP 服务相关接口实现
 *\author		邵慰
 *\date		2010/03/01
 */


#include "MPU_global.h"

#include "NET/IUPnp.h"			//UPNP 配置接口

/**************************************************************************
 *                            局部宏定义                                  *
 **************************************************************************/


//调用UPNP 管理器执行动作
#define MPU_UPNP_CALL(x)								\
do														\
{														\
	int nRet;												\
														\
	IUPNP *pMgr = IUPNP::GetInstance();						\
														\
	if (NULL != pMgr)										\
	{													\
		nRet = pMgr->x;									\
														\
		if (IDVR_OK != nRet)								\
		{												\
			MPU_ERR("%s, nRet = %d\n", #x, nRet);			\
		}												\
	}													\
	else													\
	{													\
		MPU_ERR("Get IUPNP instance fail!\n");				\
		MPU_CHK(NULL != pMgr);							\
		nRet = IDVR_MPU_UPNP_NO_MANAGER_INSTANCE;		\
	}													\
														\
	return nRet;											\
}														\
while (0)

/**************************************************************************
 *                            全局接口                                    *
 **************************************************************************/

//!\brief   设置是否启用UPNP
//!\param   bEnable     true表示启用，false表示禁用
//!\return  0表示成功，其它为错误码
int MPU_UPNP_SetEnable(bool bEnable)
{
	MPU_UPNP_CALL(UPNP_SetEnable(bEnable));
}

//!\brief   查询是否启用UPNP
//!\return  TRUE 表示启用，FALSE 表示不启用
BOOL MPU_UPNP_GetEnable()
{
	MPU_UPNP_CALL(UPNP_GetEnable());
}

//!\brief   查询路由器WAN IP
//!\param   ip   记录路由器wan ip
//!\return  0表示成功，其它为错误码
int MPU_UPNP_QUERY_EXTERNAL_IP(char *ip, int nBufferLen)
{
	MPU_UPNP_CALL(UPNP_QueryExternalIp(ip,nBufferLen));
}

//!\brief   查询路由器上的UPNP 映射列表，只显示与本机MAC 相符合的
//!\param   pMapList   映射列表0-http 1-msg 2-media 3- mobile
//!\return  
int MPU_UPNP_QUERY_MAPLiST(WORD pMapList[])
{
	MPU_UPNP_CALL(UPNP_QueryMapList(pMapList));
}

int MPU_UPNP_GET_MAPSTATE(WORD pMapList[], WORD desMapList[], unsigned int *state)
{
	MPU_UPNP_CALL(UPNP_get_mapState(pMapList, desMapList, state));
}

//!\brief   添加UPNP映射
//!\param   pMapList   映射列表0-http 1-msg 2-media 3- mobile
//!\return  0表示成功，其它为错误码

int MPU_UPNP_ADD_PORTMAP(WORD pMapList[], WORD desMapList[])
{
	//樊聪2016年6月28日
	IUPNP *pMgr = IUPNP::GetInstance();
	return pMgr->UPNP_AddPortMap(pMapList, desMapList);
}



