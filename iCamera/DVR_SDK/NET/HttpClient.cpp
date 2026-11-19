#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "HttpClient.h"
#include "NET_global.h"
//#include "../inc/Property.h"
//#include "../inc/Utility.h"

CHttpClientV2::CHttpClientV2()
{

}
CHttpClientV2::~CHttpClientV2()
{

}

int CHttpClientV2::Initialize()
{
	curl_global_init(CURL_GLOBAL_ALL);
	m_pCurl = curl_easy_init();
	m_plheaders = NULL;
	m_nRealDateSize = 0;
	m_dwT1 = 0;
	m_dwT2 = 0;
	m_dwT3 = 0;
	m_pBuffer = NULL;
	m_pBufferLen = 0;
	return 0;
}
int CHttpClientV2::UnInitialize()
{
	if(NULL != m_plheaders)
	{
		curl_slist_free_all(m_plheaders);
		m_plheaders = NULL;
	}
	curl_easy_cleanup(m_pCurl);
	curl_global_cleanup();
	if(NULL != m_pBuffer)
	{
		free(m_pBuffer);
		m_pBuffer = NULL;
	}
	return 0;
}

int CHttpClientV2::AddHeader(const char * pHeaderName,const char * pHeaderVaule)
{
	char cHeader[128]="\0";
	snprintf(cHeader, sizeof(cHeader), "%s:%s",pHeaderName,pHeaderVaule);
	m_plheaders = curl_slist_append(m_plheaders,cHeader);
	return 0;
}

//设置代理
int CHttpClientV2::SetProxy(const char * pIP,int nPort,const char * pUserName,const char * pPassword)
{
	curl_easy_setopt(m_pCurl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP); //
	
	curl_easy_setopt(m_pCurl, CURLOPT_PROXY, pIP);
	curl_easy_setopt(m_pCurl, CURLOPT_PROXYPORT, nPort);
	if(NULL != pUserName && NULL != pPassword)
	{
		curl_easy_setopt(m_pCurl, CURLOPT_PROXYAUTH, CURLAUTH_BASIC);
		curl_easy_setopt(m_pCurl, CURLOPT_PROXYUSERNAME, pUserName);
		curl_easy_setopt(m_pCurl, CURLOPT_PROXYUSERPWD, pPassword);
	}
	else
	{
		curl_easy_setopt(m_pCurl, CURLOPT_PROXYAUTH, CURLAUTH_NONE);
	}
	return 0;
}

//设置连接超时时间
int CHttpClientV2::SetConnectTimeout(int nSecond)
{
	curl_easy_setopt(m_pCurl, CURLOPT_CONNECTTIMEOUT, nSecond); 
	return 0;
}

//设置超时时间（连接成功之后，等待数据的时间）
int CHttpClientV2::SetTimeout(int nSecond)
{
	curl_easy_setopt(m_pCurl, CURLOPT_TIMEOUT, nSecond); 
	return 0;
}

//设置超时时间(毫秒)
int CHttpClientV2::SetConnectTimeoutMS(int nMilliSecond)
{
	curl_easy_setopt(m_pCurl, CURLOPT_CONNECTTIMEOUT_MS, nMilliSecond);
	return 0;
}
//设置超时时间（连接成功之后，等待数据的时间）(毫秒)
int CHttpClientV2::SetTimeoutMS(int nMilliSecond)
{
	curl_easy_setopt(m_pCurl, CURLOPT_TIMEOUT_MS, nMilliSecond);
	return 0;
}

size_t cbHeaddata(void *buffer, size_t size, size_t nmemb, void *userp) 
{
	CHttpClientV2 * pThis = (CHttpClientV2 *)userp;

    if(nmemb > MAX_HTTP_RESPONSE_LENGTH)
	{
		//OUTC_TRACK("data size = [%lu],buffer size = [%lu],buffer size is small\n",nmemb,MAX_HTTP_RESPONSE_LENGTH);
		return -1;
	}
	strncat(pThis->m_pBuffer + pThis->m_nRealDateSize,(char*)buffer,MAX_HTTP_RESPONSE_LENGTH);
	pThis->m_nRealDateSize += nmemb;

	return nmemb;
}

int CHttpClientV2::HttpHead(const char * pURL,int nPort,const char * pGetQueryParam)
{
	if(NULL == m_pBuffer)
	{
		m_pBuffer = (char *)malloc(MAX_HTTP_RESPONSE_LENGTH * sizeof(char));
		if(NULL == m_pBuffer)
		{
			return 699;
		}
	}
	CURLcode res;
	memset(m_pBuffer,0,MAX_HTTP_RESPONSE_LENGTH);
	m_nRealDateSize = 0;
	//全局控制
	curl_easy_setopt(m_pCurl, CURLOPT_HEADER, 1);			//如果为true，那么返回信息里面会有头部信息
	//curl_easy_setopt(m_pCurl, CURLOPT_VERBOSE, 1);		//输出调试信息
	curl_easy_setopt(m_pCurl, CURLOPT_NOSIGNAL, 1);			//忽略SIGPIPE信号导致崩溃的问题
	//curl_easy_setopt(m_pCurl, CURLOPT_CONNECT_ONLY, 1L);

	//请求头部控制
#if 1
	curl_easy_setopt(m_pCurl, CURLOPT_URL, pURL); //URL
	if(nPort != 0)
	{
		curl_easy_setopt(m_pCurl, CURLOPT_PORT, nPort); //端口
	}
	curl_easy_setopt(m_pCurl, CURLOPT_HTTPHEADER, m_plheaders); //请求消息头
	curl_easy_setopt(m_pCurl,CURLOPT_NOBODY, 1);

	//返回
	curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, cbHeaddata);
	curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, this);

	res = curl_easy_perform(m_pCurl);
	if(NULL != m_plheaders)
	{
		curl_slist_free_all(m_plheaders);
		m_plheaders = NULL;
	}

	if(res != CURLE_OK)
	{
		fprintf(stderr, "curl_easy_perform() failed: %s[%d]\n",curl_easy_strerror(res),res);
		if(res == 28) //超时
		{
			return -1;
		}
		else
		{
			return -1;
		}	
	}
	long retcode = 0;
	CURLcode return_code = curl_easy_getinfo(m_pCurl, CURLINFO_RESPONSE_CODE , &retcode); 
	if((CURLE_OK==return_code) && retcode)
	{
		return retcode;
	}
#endif
	return 0;
}

size_t cbGetdata(void *buffer, size_t size, size_t nmemb, void *userp) 
{
	CHttpClientV2 * pThis = (CHttpClientV2 *)userp;

	if(nmemb > MAX_HTTP_RESPONSE_LENGTH)
	{
		return nmemb;
	}
	memcpy((void *)pThis->m_pBuffer,buffer,nmemb);
	pThis->m_nRealDateSize = nmemb;
	return nmemb;
}


int CHttpClientV2::HttpGet(const char * pURL,int nPort,const char * pGetQueryParam)
{
	if(NULL == m_pBuffer)
	{
		m_pBuffer = (char *)malloc(MAX_HTTP_RESPONSE_LENGTH * sizeof(char));
		if(NULL == m_pBuffer)
		{
			return 699;
		}
	}
	CURLcode res;
	memset(m_pBuffer,0,MAX_HTTP_RESPONSE_LENGTH);
	m_nRealDateSize = 0;
	//全局控制
	curl_easy_setopt(m_pCurl, CURLOPT_NOSIGNAL, 1);			//忽略SIGPIPE信号导致崩溃的问题

	//请求头部控制
	curl_easy_setopt(m_pCurl, CURLOPT_URL, pURL); //URL
	if(nPort != 0)
	{
		curl_easy_setopt(m_pCurl, CURLOPT_PORT, nPort); //端口
	}
	curl_easy_setopt(m_pCurl, CURLOPT_HTTPHEADER, m_plheaders); //请求消息头


	//返回
	curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, cbGetdata);
	curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, this);

	res = curl_easy_perform(m_pCurl);
	if(NULL != m_plheaders)
	{
		curl_slist_free_all(m_plheaders);
		m_plheaders = NULL;
	}

	if(res != CURLE_OK)
	{
		fprintf(stderr, "curl_easy_perform() failed: %s[%d]\n",curl_easy_strerror(res),res);
		if(res == 28) //超时
		{

			return -1;
		}
		else
		{

			return -1;
		}	
	}
	long retcode = 0;
	CURLcode return_code = curl_easy_getinfo(m_pCurl, CURLINFO_RESPONSE_CODE , &retcode); 
	if((CURLE_OK==return_code) && retcode)
	{
		return retcode;
	}
	
	return 0;
}

size_t cbPostdataV2(void *buffer, size_t size, size_t nmemb, void *userp) 
{
	CHttpClientV2 * pThis = (CHttpClientV2 *)userp;
    char *ptr = NULL;

	if((nmemb > MAX_HTTP_RESPONSE_LENGTH) || ((pThis->m_nRealDateSize + nmemb) > MAX_HTTP_RESPONSE_LENGTH))
	{
		NET_ERR("Data size exceeds 16K\n");
		return -1;
	}

    if((pThis->m_pBufferLen - 1) < (pThis->m_nRealDateSize + (int)nmemb))
    {
		//缓存初始申请2K，接受的数据大于2K后，开始随数据大小申请，但最大申请不能超过16K
        ptr = (char *)realloc(pThis->m_pBuffer, pThis->m_nRealDateSize + nmemb + 1);
        if(NULL == ptr)
        {
			NET_ERR("out of memory!\n");
            return -1;
        }

        pThis->m_pBuffer = ptr;
        pThis->m_pBufferLen = pThis->m_nRealDateSize + nmemb + 1;
    }
    
	memcpy((void *)(pThis->m_pBuffer + pThis->m_nRealDateSize), buffer, nmemb);
	pThis->m_nRealDateSize += nmemb;

	return nmemb;
}

int CHttpClientV2::HttpPost(const char * pURL,int nPort,const char * pPostQueryParam,char * pBuffer,int nBufferLen)
{
	if(NULL == m_pBuffer)
	{
		m_pBuffer = (char *)malloc(HTTP_RESPONSE_LENGTH * sizeof(char));
		if(NULL == m_pBuffer)
		{
			return -1;
		}
	}
	CURLcode res;
	memset(m_pBuffer,0,HTTP_RESPONSE_LENGTH);
    m_pBufferLen = HTTP_RESPONSE_LENGTH;
	m_nRealDateSize = 0;
	//全局控制
	curl_easy_setopt(m_pCurl, CURLOPT_NOSIGNAL, 1);		//忽略SIGPIPE信号导致崩溃的问题


	//请求头部控制
	curl_easy_setopt(m_pCurl, CURLOPT_POST, 1); //方法
	curl_easy_setopt(m_pCurl, CURLOPT_URL, pURL); //URL
	if(nPort != 0)
	{
		curl_easy_setopt(m_pCurl, CURLOPT_PORT, nPort); //端口
	}
    curl_easy_setopt(m_pCurl, CURLOPT_CONNECTTIMEOUT, 3);  
	curl_easy_setopt(m_pCurl, CURLOPT_TIMEOUT, 5);   
    
    curl_easy_setopt(m_pCurl, CURLOPT_LOW_SPEED_TIME, 1L);
    curl_easy_setopt(m_pCurl, CURLOPT_LOW_SPEED_LIMIT, 6L);
    
	if(NULL != pPostQueryParam)
	{
		curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDS, pPostQueryParam);//Post的请求消息参数
	}

    AddHeader("Content-Type", "application/json");
	curl_easy_setopt(m_pCurl, CURLOPT_HTTPHEADER, m_plheaders); //请求消息头


	//body
	curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDS, pBuffer);
	curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDSIZE, nBufferLen);


	//返回
	curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, cbPostdataV2);
	curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYHOST, 0L);
    
	res = curl_easy_perform(m_pCurl);
	if(NULL != m_plheaders)
	{
		curl_slist_free_all(m_plheaders);
		m_plheaders = NULL;
	}

	if (res != CURLE_OK)
	{
		fprintf(stderr, "curl_easy_perform() failed: %s[%d]\n", curl_easy_strerror(res), res);
		if (res == 28) //超时
		{
			return -1;
		}
		else
		{
			return res;
		}
	}
	long retcode = 0;
	CURLcode return_code = curl_easy_getinfo(m_pCurl, CURLINFO_RESPONSE_CODE, &retcode);

	if ((CURLE_OK == return_code) && retcode)
	{
		if(retcode != 200)
		{
		    ;
		}
		else
		{
		    ;
		}
		return retcode;
	}
	return 0;
}


void CHttpClientV2::GetHttpResponse(char ** pstrBuffer,int nMaxBufferLen)
{
	if(nMaxBufferLen < m_nRealDateSize)
	{
		return;
	}
	*pstrBuffer = m_pBuffer;
}


static void print_cookies(CURL *curl)
{
	CURLcode res;
	struct curl_slist *cookies;
	struct curl_slist *nc;
	int i;

	res = curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies);
	if (res != CURLE_OK) {
		fprintf(stderr, "Curl curl_easy_getinfo failed: %s\n", curl_easy_strerror(res));
		exit(1);
	}
	nc = cookies, i = 1;
	while (nc) {
		nc = nc->next;
		i++;
	}
	if (i == 1) {
        ;
	}
	curl_slist_free_all(cookies);
}

void CHttpClientV2::PrintInfo()
{
	CURLcode return_code;

	char *ipAddress={0};
	return_code = curl_easy_getinfo(m_pCurl, CURLINFO_PRIMARY_IP,&ipAddress);
	if((CURLE_OK==return_code) && ipAddress)
		;

	long retcode = 0;
	return_code = curl_easy_getinfo(m_pCurl, CURLINFO_RESPONSE_CODE , &retcode); 
	if((CURLE_OK==return_code) && retcode)
		;

	char *contentType={0};
	return_code = curl_easy_getinfo(m_pCurl, CURLINFO_CONTENT_TYPE , &contentType); 
	if((CURLE_OK==return_code) && contentType)
		;	

	long requestSize = 0;
	return_code = curl_easy_getinfo(m_pCurl, CURLINFO_REQUEST_SIZE , &requestSize);
	if((CURLE_OK==return_code) && requestSize)
		;

	long headerSize = 0;
	return_code = curl_easy_getinfo(m_pCurl, CURLINFO_HEADER_SIZE , &headerSize);
	if((CURLE_OK==return_code) && headerSize)
		;

	print_cookies(m_pCurl);
}
