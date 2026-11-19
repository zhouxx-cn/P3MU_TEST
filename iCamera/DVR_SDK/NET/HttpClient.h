#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include "curl/curl.h"

#define HTTP_RESPONSE_LENGTH 2048
#define MAX_HTTP_RESPONSE_LENGTH (16 * 1024)

class CHttpClientV2
{
public:
	CHttpClientV2();
	~CHttpClientV2();
public:
	int Initialize();
	int UnInitialize();
	//设置代理
	int SetProxy(const char * pIP,const int nPort,const char * pUserName,const char * pPassword);
	//增加头文件
	int AddHeader(const char * pHeaderName,const char * pHeaderVaule);
	//设置超时时间(秒)
	int SetConnectTimeout(int nSecond);
	//设置超时时间（连接成功之后，等待数据的时间）(秒)
	int SetTimeout(int nSecond);

	//设置超时时间(毫秒)
	int SetConnectTimeoutMS(int nMilliSecond);
	//设置超时时间（连接成功之后，等待数据的时间）(毫秒)
	int SetTimeoutMS(int nMilliSecond);

	int HttpHead(const char * pURL,int nPort,const char * pGetQueryParam);
	int HttpGet(const char * pURL,int nPort,const char * pGetQueryParam);
	int HttpPost(const char * pURL,int nPort,const char * pPostQueryParam,char * pBuffer,int nBufferLen);

	//
	void GetHttpResponse(char ** pstrBuffer,int nMaxBufferLen);

	friend size_t cbGetdata(void *buffer, size_t size, size_t nmemb, void *userp) ;
	friend size_t cbPostdata(void *buffer, size_t size, size_t nmemb, void *userp) ;
	friend size_t cbHeaddata(void *buffer, size_t size, size_t nmemb, void *userp) ;
protected:
	void PrintInfo();

public:
    char * m_pBuffer;
    int m_pBufferLen;
	int m_nRealDateSize;

private:
	CURL * m_pCurl;
	struct curl_slist * m_plheaders;
	unsigned long m_dwT1;
	unsigned long m_dwT2;
	unsigned long m_dwT3;

};


#endif 
