#ifndef _INC_IUPNP_H
#define _INC_IUPNP_H

#include <pthread.h>
#include <sys/socket.h>
#include "global_macro.h"
#include "NET_global.h"
#include 	"INet.h"

typedef unsigned short WORD;
typedef unsigned char BYTE;

class CConfig;

class IUPNP
{
public:

	//!\brief   获取唯一实例
	//!\return  实例指针
	static IUPNP *GetInstance();

	//!\brief   销毁实例
	static void Release();

	//!\brief   查询路由器WAN IP
	//!\param   ip   记录路由器wan ip
	//!\param   nBufferLen  缓冲区长度
	//!\return  0表示成功，其它为错误码
	int UPNP_QueryExternalIp(char *ip, int nBufferLen);

	//!\brief   查询路由器上的UPNP 映射列表，只显示与本机MAC 相符合的
	//!\param   pMapList   映射列表0-http 1-msg 2-media 3- mobile
	//!\return  0表示成功，其它为错误码
	int UPNP_QueryMapList(WORD pMapList[]);

	//!\brief   添加UPNP映射
	//!\param   pMapList   映射列表0-http 1-msg 2-media 3- mobile
	//!\return  0表示成功，其它为错误码
	int UPNP_AddPortMap(WORD pMapList[], WORD desMapList[]);

	//!\brief   删除UPNP映射
	//!\param   wPort 待删除的外网端口
	//!\return  0表示成功，其它为错误码
	int UPNP_DelPortMap(WORD wPort);

	//!\brief   设置服务开启状态
	//!\param   bEnable
	//!\return  0表示成功，其它为错误码
	int UPNP_SetEnable(bool bEnable);

	//!\brief   查询是否启用UPNP
	//!\return  TRUE 表示启用，FALSE 表示不启用
	BOOL UPNP_GetEnable();

	//获取映射的状态，
	int UPNP_get_mapState(WORD pMapList[], WORD desMapList[],unsigned int *state);

	//判断该端口是否已经映射，如果已经映射则删除映射
	int isAdded(WORD pMapList[], WORD desMapList[]);

protected:
	
	  IUPNP();

	~IUPNP();

	//!\brief 初始化
	int Init();

	//!\brief 释放资源
	int UnInit();

private:
	
	static IUPNP * m_Instance;

	//配置文件
	CConfig *m_pCConfig;
	INet *m_pINet;
};


#endif

