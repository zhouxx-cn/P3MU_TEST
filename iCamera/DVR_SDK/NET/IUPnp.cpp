#include "IUPnp.h"
#include "MPU_Net_API.h"
#include "MPU_CIU_API.h"
#include "mysystem.h"
#include "CConfig.h"
#include <stdlib.h>
#include <sys/time.h>
// #include "sys_banned.h"
#include "global_gather.h"
#include "../PAR/CPubFunc.h"
#include "common_pub.h"

static unsigned int Upnp_state = 0;

///////////////////////////构造与初始化函数///////////////////////////////////
IUPNP* IUPNP::m_Instance = 0;

IUPNP *IUPNP::GetInstance()
{
	if (m_Instance == 0)
	{
		m_Instance = new IUPNP();
		if (m_Instance != NULL)
		{
			m_Instance->Init();
		}
	}
	
	return m_Instance;
}

void IUPNP::Release()
{
	if (m_Instance != 0)
	{
		delete m_Instance;
		m_Instance = 0;
	}
}

IUPNP::IUPNP()
{
	m_pINet = INet::GetInstance();
	if(m_pINet == NULL)
	{
		NET_ERR("get INet's  point error!!!\n");	//fan-2014-06-19
	}
}

IUPNP::~IUPNP()
{

}

int IUPNP::Init()
{
	//获取配置实例
	m_pCConfig = CConfig::GetInstance();
	if (NULL == m_pCConfig)
	{
		NET_ERR("NULL==m_pCConfig\n");
		UnInit();
		return -1;
	}
	if(m_pINet == NULL)
	{
		NET_ERR("error m_pINet is NULL\n");
		return -1;
	}
	
	return 0;
}

int IUPNP::UnInit()
{
	return 0;
}

/////////////////////////////////////功能函数///////////////////////////////////

//!\brief   查询路由器WAN IP
//!\param   ip   记录路由器wan ip
//!\return  0表示成功，其它为错误码
int IUPNP::UPNP_QueryExternalIp(char *ip, int nBufferLen)
{
	if (NULL == ip || nBufferLen < 16)
	{
		NET_ERR("Param ERROR!");
		return -1;
	}

    int  nCurrNetCardType = 1;
    char buf[128] = {0};
	
    /* 当前有效的的网卡(0:unknown 1:eth0 2:eth1 3:wlan 4:3G 5:4G 6:5G) */
    m_pINet->NET_GetCurrNetCardType(&nCurrNetCardType);
#if defined (__IS_SUPPORT_WLAN__)
	if (1 == nCurrNetCardType)       // eth0
	{
        strncpy(buf, " upnpc -m eth0 -s | grep 'ExternalIPAddress' | sed -n 's/[^0-9.]//gp' ", sizeof(buf) - 1);
	}
	else if (3 == nCurrNetCardType) // wlan0
	{
        strncpy(buf, " upnpc -m wlan0 -s | grep 'ExternalIPAddress' | sed -n 's/[^0-9.]//gp' ", sizeof(buf) - 1);
	}
	else
	{
        return -1;
	}
#else
    strncpy(buf, " upnpc -s | grep 'ExternalIPAddress' | sed -n 's/[^0-9.]//gp' ", sizeof(buf) - 1);
#endif
    printf("run cmd %s\n", buf);

    if ( ShellCmdOut(buf, ip, nBufferLen) )
    {
		NET_ERR("Fail to Query External Ip\n");
        memcpy(ip,"0.0.0.0",8);
		return 0;
    }

    ip[strlen(ip)-1] = '\0';
    
    printf("External Ip %s, nBufferLen = %d\n", ip, nBufferLen);

	return 0;
}

//!\brief   查询路由器上的UPNP 映射列表，只显示与本机MAC 相符合的
//!\param   pMapList   映射列表0-http 1-msg 2-media 3- rtsp
//!\return  0表示成功，其它为错误码
int IUPNP::UPNP_QueryMapList(WORD pMapList[])
{
	pMapList[0] =  m_pCConfig->GetValue(INet_CfgSection, INet_UPNP_HTTP_port, 0);
	pMapList[1] =  m_pCConfig->GetValue(INet_CfgSection, INet_UPNP_msg_port, 0);
	pMapList[2] =  m_pCConfig->GetValue(INet_CfgSection, INet_UPNP_media_port, 0);
	pMapList[3] =  m_pCConfig->GetValue(INet_CfgSection, INet_UPNP_rtsp_port, 0);
	
	return 0;
}

//!\brief   添加UPNP映射
//!\param   pMapList   映射列表0-http 1-msg 2-media 3- rtsp
//!\return 不小于0 为成功， 返回映射的状态，前四位表示状态
int IUPNP::UPNP_AddPortMap(WORD pMapList[], WORD desMapList[])
{
	char cIP[16] = {0};
    char iface[16] = {0};
	char buf[256] = {0};
	int ret = 0;
    int nCurrNetCardType = 1;

    /* 当前有效的的网卡(0:unknown 1:eth0 2:eth1 3:wlan 4:3G 5:4G 6:5G) */
    m_pINet->NET_GetCurrNetCardType(&nCurrNetCardType);

    if (1 == nCurrNetCardType)      // eth0
    {
        strncpy(iface, "eth0", sizeof(iface) - 1);
        MPU_NET_GetIPAddress(cIP);
    }
    else if (3 == nCurrNetCardType) // wlan0
    {
		#if defined (__IS_SUPPORT_WLAN__)
        strncpy(iface, "wlan0", sizeof(iface) - 1);
        MPU_WLAN_GetIPAddress(cIP);
		#endif
    }
    else
    {
        return -1;
    }

	isAdded(pMapList, desMapList);//删除已添加的映射
	Upnp_state = 0;
    
	for (int i = 0; i < 4; ++i)
	{
		ret = 1;
		
		if (0 == pMapList[i])
		{
			continue;
	    }
	    
		memset(buf, 0, 256);
		
        //snprintf(buf, sizeof(buf), "upnpc -m %s -a %s %d %d TCP 0|awk -F[\": \"] 'BEGIN{RESULT=0}/TCP is redirected to internal/{RESULT=$3}END{print RESULT}'", iface, cIP, pMapList[i], desMapList[i]);

		snprintf(buf, sizeof(buf), "upnpc -a %s %d %d TCP 0|awk -F[\": \"] 'BEGIN{RESULT=0}/TCP is redirected to internal/{RESULT=$3}END{print RESULT}'",
			   cIP,pMapList[i],desMapList[i]);

		char cValue[6] = {0};

        if ( ShellCmdOut(buf, cValue, sizeof(cValue)) )
        {
            NET_ERR("Fail to Query Port!\n");
            return -1;
        }

		// 0 -- 失败;非0 为external port，因为当输入的port很大时，路由器
		// 会映射到其他端口，如65535->30585
		int iresult = atoi(cValue);
		if (0 == iresult || iresult > 65535)
		{
			ret = 0;
		}

		char tmpKey[32];
		switch(i)
		{
			case 0:
				strncpy(tmpKey, INet_UPNP_HTTP_port, sizeof(tmpKey) - 1);
				break;
			case 1:
				strncpy(tmpKey, INet_UPNP_msg_port, sizeof(tmpKey) - 1);
				break;
			case 2:
				strncpy(tmpKey, INet_UPNP_media_port, sizeof(tmpKey) - 1);
				break;
			case 3:
				strncpy(tmpKey, INet_UPNP_rtsp_port, sizeof(tmpKey) - 1);
				break;
		}

		if (m_pCConfig->SetValue(INet_CfgSection, tmpKey, desMapList[i]) < 0)
		{
			NET_ERR("SetValue failed\n");
			return -1;
		}

		Upnp_state |= ret << i;
	}
	
	return Upnp_state;
}

//!\brief   删除UPNP映射
//!\param   wPort 待删除的外网端口
//!\return  0表示成功，其它为错误码
int IUPNP::UPNP_DelPortMap(WORD wPort)
{
	char buf[256] = { 0 };
    int  nCurrNetCardType = 1;
    
    bzero(buf, sizeof(buf));

    /* 当前有效的的网卡(0:unknown 1:eth0 2:eth1 3:wlan 4:3G 5:4G 6:5G) */
    m_pINet->NET_GetCurrNetCardType(&nCurrNetCardType);
#if defined (__IS_SUPPORT_WLAN__)
    if (1 == nCurrNetCardType)       // eth0
    {
    	snprintf(buf, sizeof(buf), "upnpc -m eth0 -d %d TCP | awk -F: 'BEGIN{RESULT=-4}/UPNP_DeletePortMapping()/{RESULT=$2}END{print RESULT}'", wPort);
    }
    else if (3 == nCurrNetCardType) // wlan0
    {
    	snprintf(buf, sizeof(buf), "upnpc -m wlan0 -d %d TCP | awk -F: 'BEGIN{RESULT=-4}/UPNP_DeletePortMapping()/{RESULT=$2}END{print RESULT}'", wPort);
    }
    else
    {
        return -1;
    }
#else
	snprintf(buf, sizeof(buf),"upnpc -d %d TCP | awk -F: 'BEGIN{RESULT=-4}/UPNP_DeletePortMapping()/{RESULT=$2}END{print RESULT}'",wPort);    
#endif
	
	char result[16] = {0};
    if ( ShellCmdOut(buf, result, sizeof(result)) )
    {
        NET_ERR("Fail to Del PortMap!\n");
        return -1;
    }

	return (atoi(result));
}

//!\brief   设置服务开启状态
//!\param   bEnable
//!\return  0表示成功，其它为错误码
int IUPNP::UPNP_SetEnable(bool bEnable)
{
	//写配置
	if (m_pCConfig->SetValue(INet_CfgSection, INet_UPNP_key, bEnable?1:0) < 0)
	{
		NET_ERR("SetValue failed\n");
		return -1;
	}
	
	WORD pMapList[4];

	if(!bEnable)
	{
		if(UPNP_QueryMapList(pMapList))
		{
			return -2;
		}
		else//删除映射
		{
			for(int i=0; i<4; i++)
			{
				if(UPNP_DelPortMap(pMapList[i]))
				{
					NET_ERR("Error UPNP_DelPortMap error!!\n" );
			    }
			}
		}
	}
	
	return 0;
}
	
//!\brief   查询是否启用UPNP
//!\return  TRUE 表示启用，FALSE 表示不启用
BOOL IUPNP::UPNP_GetEnable()
{
	int nRet = m_pCConfig->GetValue(INet_CfgSection, INet_UPNP_key, 0);
	
	return nRet;
}

int IUPNP::UPNP_get_mapState(WORD pMapList[], WORD desMapList[],unsigned int *state)
{
	if(state == NULL)
	{
		return -1;
	}

	char cIP[16] = {0};
    char iface[16] = {0};
	char buf[128];
	FILE *fp = NULL;

    struct timeval now;
    char tmp_file[32] = {0};
    char command[256] = {0};
    
    int nCurrNetCardType = 1;
 
    /* 当前有效的的网卡(0:unknown 1:eth0 2:eth1 3:wlan 4:3G 5:4G 6:5G) */
    m_pINet->NET_GetCurrNetCardType(&nCurrNetCardType);

    if (1 == nCurrNetCardType)      // eth0
    {
        strncpy(iface, "eth0", sizeof(iface) - 1);
        MPU_NET_GetIPAddress(cIP);
    }
    else if (3 == nCurrNetCardType) // wlan0
    {
		#if defined (__IS_SUPPORT_WLAN__)
        strncpy(iface, "wlan0", sizeof(iface) - 1);
        MPU_WLAN_GetIPAddress(cIP);
		#endif
    }
    else
    {
        return -1;
    }

    bzero(buf, sizeof(buf));
    //snprintf(buf, sizeof(buf), " upnpc -m %s -l | sed  -n \"/TCP/{s/'//g;s/->/  /g;s/:/  /p;}\"|awk '$4 ~ /%s/{print $3,$5}' ", iface, cIP);
    snprintf(buf, sizeof(buf)," upnpc -l | sed  -n \"/TCP/{s/'//g;s/->/  /g;s/:/  /p;}\"|awk '$4 ~ /%s/{print $3,$5}' ", cIP);

    gettimeofday(&now, NULL);
#if defined (__MUSL__)
    snprintf(tmp_file, sizeof(tmp_file), "/var/log/upnp_mapstatus.%lld", now.tv_sec*1000+now.tv_usec/1000);
#else
	snprintf(tmp_file, sizeof(tmp_file), "/var/log/upnp_mapstatus.%ld", now.tv_sec*1000+now.tv_usec/1000);
#endif
	snprintf(command, sizeof(command), "%s > %s", buf, tmp_file);
	mysystem(command);

	if (NULL == (fp=fopen(tmp_file, "r")))
	{
		NET_ERR("fopen error!");
		return -1;
	}

	char cValue[50] = {0};
	unsigned int  flag = 0;
	char* cTemp = cValue;

	while (!feof(fp))
	{
		cTemp = cValue;
		fgets(cTemp, 50, fp);
	
		// ip external_port internal_port
		// 192.168.1.3 80 80  为简单，内外一致
		char* pBlank =  strchr(cTemp,' ');
		if (NULL != pBlank)
		{
			WORD ExternalPort = (WORD)atoi(cTemp);
			pBlank++;
			WORD InternalPort = (WORD)atoi(pBlank);
			int i=0;
			for( i = 0; i < 4; i++)
			{
				if(InternalPort == pMapList[i])
				{
					desMapList[i] = ExternalPort;
					flag |= (0x01 << i);
				}
			}
		}
	}

	*state = flag;
    fclose(fp);
    remove(tmp_file);
    
	return 0;
}

int IUPNP::isAdded(WORD pMapList[], WORD desMapList[])
{
	for(int i = 0; i < 4; i++)
	{
		UPNP_DelPortMap(desMapList[i]);
	}
	return 0;
}
