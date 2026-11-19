/*!
 *\file		MPU_CloudUpgrade_API.h
 *\brief		MPU模块，云升级相关接口定义
 *\author	樊景柱
 *\date		2017/12/29

功能：
-#	设备升级列表服务器配置管理
-#	设备升级包服务器配置管理
-#   设备升级列表获取
-#   云端升级包下载及状态查询
-#   云升级启动及状态返回
-#   云升级结果查询
*/

#include <sys/mount.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/shm.h>

#include "MPU_global.h"
#include "mxml.h"
#include "mysystem.h"
#include "md5Lib.h"  
#include "stdDes.h"
#include "base64Lib.h"
#include "curl/curl.h"
#include "Authority.h"
#include "common_pub.h"
#include "PUB_Language.h"
#include "DEVICEVERSION.h"
#include "MPU_PUB_API.h"
#include "../PAR/CPubFunc.h"
#include "PUB_BoradInfo.h"

/* 消除本文件中的snprintf截断警告 */
#pragma GCC diagnostic ignored "-Wformat-truncation"

#define UPLOAD_TO_DSP_MEM

#define PRTCOLOR

/* 打印宏 */
#define MSG_DEBUG   0x01
#define MSG_INFO    0x02
#define MSG_ERROR   0x04

static int LogLevel = MSG_DEBUG | MSG_INFO | MSG_ERROR;

#ifdef PRTCOLOR
#define NONE         "\033[m"  
#define RED          "\033[0;32;31m"
#define LIGHT_RED    "\033[1;31m"
#define GREEN        "\033[0;32;32m"
#define LIGHT_GREEN  "\033[1;32m"
#define BLUE         "\033[0;32;34m"
#define LIGHT_BLUE   "\033[1;34m"
#define DARY_GRAY    "\033[1;30m"
#define CYAN         "\033[0;36m"
#define LIGHT_CYAN   "\033[1;36m"
#define PURPLE       "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN        "\033[0;33m"
#define YELLOW       "\033[1;33m"
#define LIGHT_GRAY   "\033[0;37m"
#define WHITE        "\033[1;37m"
#else
#define NONE
#define RED
#define LIGHT_RED
#define GREEN
#define LIGHT_GREEN
#define BLUE
#define LIGHT_BLUE
#define DARY_GRAY
#define CYAN
#define LIGHT_CYAN
#define PURPLE
#define LIGHT_PURPLE
#define BROWN
#define YELLOW
#define LIGHT_GRAY
#define WHITE
#endif

#undef printf
#define	LPRINT(mylog, LOG_LEVEL, format, args...) do {	\
	if (mylog & LOG_LEVEL)	{	\
		printf(format, ##args);	\
	}	\
} while (0)

#define CLUP_ERROR(format, args...)		LPRINT(LogLevel, MSG_ERROR, LIGHT_RED "[%s][ERR][%s:%d]" NONE format, CLOUD_UPGRADE_CUSTOM_SECTION_NAME, __FILE__, __LINE__, ##args)
#define CLUP_INFO(format, args...)		LPRINT(LogLevel, MSG_INFO, LIGHT_PURPLE "[%s][INFO][%s:%d]" NONE format, CLOUD_UPGRADE_CUSTOM_SECTION_NAME, __FILE__, __LINE__, ##args)
#define CLUP_DEBUG(format, args...)		LPRINT(LogLevel, MSG_DEBUG, LIGHT_GREEN "[%s][DBG][%s:%d]" NONE format, CLOUD_UPGRADE_CUSTOM_SECTION_NAME, __FILE__, __LINE__, ##args)


typedef struct tageBoardIdProductModelMap
{
	int nBoardId;
	int nDeviceType;
	char szProductModel[128];
} BOARDID_PRODUCT_MODEL_MAP;

typedef struct tagSpeechLanguageMap
{
	eSpeechLanguage eType;
	char szDescription[32];
} SPEECH_LANGUAGE_MAP;

// 升级版本列表所在的服务器配置
typedef struct
{
	char		szHostIP[64];									/*!< 服务器IP地址, 必须是IP地址 */
	int			nPort;											/*!< 服务器端口					*/
	char		szPath[128];									/*!< 服务器存储目录				*/
	char		szProtocol[16];									/*!< 服务器访问协议，范围:[http,ftp] */
	char		szUserName[256];								/*!< 服务器登录用户名称, 可以为空    */
	char		szPassword[256];								/*!< 服务器登录用户密码, 可以为空    */
}UPGRADE_LIST_SERVER_CFG_LOCAL;

// 升级包版本描述信息
typedef struct
{
	char		szFirmwareName[MAX_FIRMWARE_NAME_LEN];			/*!< 升级包版本唯一标识								*/
	char		szHostIP[24];									/*!< 服务器IP地址, 必须是IP地址 */
	int			nPort;											/*!< 服务器端口					*/
	char		szProtocol[16];									/*!< 服务器访问协议，范围:[http,ftp] */
	char		szUserName[256];								/*!< 服务器登录用户名称, 可以为空    */
	char		szPassword[256];								/*!< 服务器登录用户密码, 可以为空    */
	char		szMd5[128];										/*!< 升级包文件md5 */
	char		szURL[128];										/*!< 升级包文件URL*/
	int			nFileSize;										/*!< 升级包文件大小*/
}UPGRADE_FIRMWARE_INFO_LOCAL;

// 升级包内存信息
typedef struct UpgradeMem
{
    void *Addr;
    unsigned int Size;
} UPGRADE_MEM_S, * PUPGRADE_MEM_S;

// 升级缓存空间信息
typedef struct
{
	unsigned int nCnt;
	UPGRADE_MEM_S Mem[UPGRADE_MAX_MEM_NUM];
	FILE *pFile;
	unsigned int uMaxSize;
	unsigned int uDataSize;
}UPGRADE_BUF_INFO;

// 升级包下载线程数据
typedef struct
{
	pthread_t	thrdId;
	int			nDownloadStatus;	// 0,空闲 1,正在下载 2,下载完成 3,下载失败
}UPGRADE_FILE_DOWNLOAD_THREAD_DATA;

//云升级当前阶段
typedef enum
{
	CLOUD_UPGRADE_STATGE_INIT = 0,	//初始阶段
	CLOUD_UPGRADE_STATGE_SET_UPGRADE_LIST_SERVER_CFG,	//设置升级列表服务器配置阶段
	CLOUD_UPGRADE_STATGE_GET_UPGRADE_LIST,	//获取升级列表阶段
	CLOUD_UPGRADE_STATGE_DOWNLOAD_UPGRADE_FILE,	//下载升级文件阶段
	CLOUD_UPGRADE_STATGE_UPGRADE,	//升级阶段
}CLOUD_UPGRADE_STATGE_EN;

typedef struct
{
	char		szProtocol[16];
	char		szUserName[64];
	char		szPassword[64];
	char		szURL[256];
	int			nConnectTimeoutSec;
	int			nTimeoutSec;
	int			nTriesCnt;
	void *		pUserData;
}DOWNLOAD_CURL_PARAM;

static const char *s_ProtocolSupSet[] = {
	CLOUD_UPGRADE_DOWNLOAD_PROTOCOL_HTTP,
	CLOUD_UPGRADE_DOWNLOAD_PROTOCOL_FTP,
	CLOUD_UPGRADE_DOWNLOAD_PROTOCOL_SFTP,
};

typedef enum{
	ManualUpgrade   = 0, // 手动升级
	AutoUpgrade     = 1, // 自动升级
}eUpgradeTye;

static int g_nUpgradeType = ManualUpgrade;//默认手动升级

/** function declaration **/
static int MPU_CloudU_VersionComp (const char *szVersionStringA, const char *szVersionStringB, int *pnResult);



static sem_t s_semReboot;
static pthread_t s_thrdIdReboot = 0;
static pthread_t s_thrdIdMonitor = 0;
static UPGRADE_LIST_SERVER_CFG_LOCAL s_stUpgradeListServerCfg = {0};
#if defined (__IS_SUPPORT_P6SMESSAGE__)
static UPGRADE_LIST_SERVER_CFG_LOCAL s_stUpgradeListServerCfgV1 = {0};
#endif
static char s_szUpgradePassport[MAX_UPGRADE_PASSPORT_LEN] = {0};	// 升级过程凭证(由APP产生一段随机字符，后续所有操作都将使用它作为凭证)
static UPGRADE_FIRMWARE_INFO_LOCAL s_stUpgradeFileDownloadInfo = {0};
static UPGRADE_BUF_INFO s_stUpgradeBufInfo = {0};
static UPGRADE_FILE_DOWNLOAD_THREAD_DATA s_stUpgradeFileDownloadThreadData = {0};
static CLOUD_UPGRADE_STATGE_EN enCloudUpgradeStageCur = CLOUD_UPGRADE_STATGE_INIT;
static int s_nRebootTimer = CLOUD_UPGRADE_REBOOT_MONITOR_DELAY;
// static char s_szClientId[24] = {0};
// static char s_szCustomNo[24] = {0};

/*接口调用互斥锁*/
static pthread_mutex_t s_mutexInterface = PTHREAD_MUTEX_INITIALIZER;
/*重启监控定时器互斥锁*/
static pthread_mutex_t s_mutexMonitorTimer = PTHREAD_MUTEX_INITIALIZER;
/**************************************************************************
 *                                                      模块内部接口                                              *
 **************************************************************************/

static int CreateShm(int nSize, int nFlags)
{
    int nKey = -1;
    int nShmid = -1;
    
    nKey = ftok(KEY_NAME, KEY_ID_MEM_UPGRADE);
    if (nKey < 0)
    {
        printf("Fail to ftok %s, id=%#x\n", KEY_NAME, KEY_ID_MEM_UPGRADE);
        return -1;
    }
    nShmid = shmget(nKey, nSize, nFlags);
    if (nShmid < 0)
    {
        printf("Fail to shmget[%d:%s]\n", errno, strerror(errno));
        return -1;
    }

    return nShmid;
}

static void *GetShm(int nShmID, const void* pShmAddr, int nFlags)
{
    void* pAddr = NULL;
    pAddr = shmat(nShmID, pShmAddr, nFlags);
    if (-1 == (int)pAddr)
    {
        printf("Fail to shmat\n");
        return NULL;
    }

	return pAddr;
}

/*字符串大写转小写*/
static int StrCapitals2SmallLetter(char *pDst, const char *pSrc, int nDataLen)
{
	if (NULL == pSrc
		|| NULL == pDst
		|| nDataLen <= 0)
	{
		return -1;
	}
	
	int i = 0;

	for (i = 0; i < nDataLen; i++)
	{
		if (pSrc[i] >= 'A' && pSrc[i] <= 'Z')
		{
			pDst[i] = pSrc[i] + 32;
		}
		else
		{
			pDst[i] = pSrc[i];
		}
	}
	
	return 0;
}

//重启线程
static void *RebootThreadForCloudUpgrade(void *pData)
{
    PRT_TID();
	
	while (1)
	{
		sem_wait(&s_semReboot);
	    //确保云升级状态返回后再重启
	    sleep(CLOUD_UPGRADE_RET_DELAY);
		MPU_PAR_SaveConfigReal();
		PAR_ForbiddenSaveFile();
		RebootLinux(REBOOT_UPLOAD);
	}

	sem_destroy(&s_semReboot);
	
	return NULL;
}

/*延时重启(非阻塞)*/
static int RebootForCloudUpgrade()
{
	int nRet = 0;
	
	//创建延时重启线程
	if (0 == s_thrdIdReboot)
	{
		pthread_attr_t thrdAttr = {0};
		
		sem_init(&s_semReboot, 0, 0);
		nRet = pthread_attr_init(&thrdAttr);
		if (0 != nRet)
		{
			CLUP_ERROR("pthread_attr_init() return %d\n", nRet);
		}
		nRet = pthread_attr_setdetachstate(&thrdAttr, PTHREAD_CREATE_DETACHED);
		if (0 != nRet)
		{
			CLUP_ERROR("pthread_attr_setdetachstate( PTHREAD_CREATE_DETACHED ) return %d\n", nRet);
		}
		nRet = pthread_create(&s_thrdIdReboot, &thrdAttr, RebootThreadForCloudUpgrade, NULL);
		if (0 != nRet)
		{
			CLUP_ERROR("pthread_create( RebootThreadForCloudUpgrade ) DETACHED return %d\n", nRet);
			pthread_attr_destroy(&thrdAttr);
		}
		pthread_attr_destroy(&thrdAttr);
	}

	sem_post(&s_semReboot);

	return 0;
}

static int ResetMonitorTimer()
{
	pthread_mutex_lock(&s_mutexMonitorTimer);
	s_nRebootTimer = CLOUD_UPGRADE_REBOOT_MONITOR_DELAY;
	pthread_mutex_unlock(&s_mutexMonitorTimer);
		
	return 0;
}

//重启监控线程
//启动下载升级包后
//一段时间内未下载完成或下载完成后未执行升级都将重启设备
static void *RebootTimerThreadForCloudUpgrade(void *pData)
{
    PRT_TID();

	ResetMonitorTimer();
	while (1)
	{
		pthread_mutex_lock(&s_mutexMonitorTimer);
		if (s_nRebootTimer <= 0)
		{
			MPU_PAR_SaveConfigReal();
			PAR_ForbiddenSaveFile();
			RebootLinux(REBOOT_UPLOAD_TIMEOUT);
		}
		s_nRebootTimer--;
		pthread_mutex_unlock(&s_mutexMonitorTimer);
		
	    usleep(1000000);
	}
	
	return NULL;
}

/*重启监控(避免设备永远处于升级状态)*/
static int RebootMonitorForCloudUpgrade()
{
	int nRet = 0;
	
	//创建重启监控线程
	if (0 == s_thrdIdMonitor)
	{
		pthread_attr_t thrdAttr = {0};
		nRet = pthread_attr_init(&thrdAttr);
		if (0 != nRet)
		{
			CLUP_ERROR("pthread_attr_init() return %d\n", nRet);
		}
		nRet = pthread_attr_setdetachstate(&thrdAttr, PTHREAD_CREATE_DETACHED);
		if (0 != nRet)
		{
			CLUP_ERROR("pthread_attr_setdetachstate( PTHREAD_CREATE_DETACHED ) return %d\n", nRet);
		}
		nRet = pthread_create(&s_thrdIdMonitor, &thrdAttr, RebootTimerThreadForCloudUpgrade, NULL);
		if (0 != nRet)
		{
			CLUP_ERROR("pthread_create( RebootTimerThreadForCloudUpgrade ) DETACHED return %d\n", nRet);
			pthread_attr_destroy(&thrdAttr);
		}
		pthread_attr_destroy(&thrdAttr);
	}

	return 0;
}

/*判断IP合法性*/
static int IsValidIpAddr(const char * ipaddr)
{
	unsigned int uIp = (unsigned int)inet_addr(ipaddr);
	if ((unsigned int)(-1) == uIp || 0 == uIp)
	{
		return 0;
	}

    unsigned int ip1 = 0;
	unsigned int ip2 = 0;
	unsigned int ip3 = 0;
	unsigned int ip4 = 0;
	
    sscanf(ipaddr, "%d.%d.%d.%d", &ip1, &ip2, &ip3, &ip4);
    if (ip1 < 0
		|| ip1 > 255
		|| ip2 < 0
		|| ip2 > 255
		|| ip3 < 0
		|| ip3 > 255
		|| ip4 <= 0 
		|| ip4 >= 255)
    {
        return 0;
    }
	
    return 1;
}

/*判断Port合法性*/
static int IsValidPort(int nPort)
{
	if (nPort < 0
		|| nPort > 65535)
    {
        return 0;
    }
	
    return 1;
}

/*判断下载协议是否支持*/
static int IsSupportProtocol(const char *pProtocolName)
{
	if (NULL == pProtocolName)
    {
        return 0;
    }

	int i = 0;
	int nSzie = sizeof(s_ProtocolSupSet)/sizeof(s_ProtocolSupSet[0]);
	for (i = 0; i < nSzie; i++)
	{
		if (0 == strcmp(pProtocolName, s_ProtocolSupSet[i]))
		{
			return 1;
		}
	}
	
    return 0;
}

/* parse headers for Content-Length */  
static size_t GetContentLengthFunc(void *ptr, size_t size, size_t nmemb, void *stream)   
{  
    int r;  
    long len = 0;  
    /* _snscanf() is Win32 specific */  
    //r = _snscanf(ptr, size * nmemb, "Content-Length: %ld\n", &len);  
    r = sscanf((const char*)ptr, "Content-Length: %ld\n", &len);  
    if (r) /* Microsoft: we don't read the specs */  
        *((long *) stream) = len;  
    return size * nmemb;  
}  

/*数据传输回调*/
size_t WriteFunc(void *ptr, size_t size, size_t nmemb, void *pUsrData)  
{  
	if (NULL == ptr 
		|| NULL == pUsrData
		|| (size * nmemb) <= 0)
	{
		return -1;
	}

	int nRet = 0;
	static unsigned int sTransShmLen = 0;
	UPGRADE_BUF_INFO *pUpgradeBufInfo = (UPGRADE_BUF_INFO *)pUsrData;
	
	if (NULL == pUpgradeBufInfo->pFile)
	{
		// CLUP_INFO("[%s %d] size=%u, nmemb=%u\n", __FUNCTION__, __LINE__, size, nmemb);
		// CLUP_INFO("[%s %d] dspLen=%u, shmLen=%u, alreadyTransLen=%u\n", __FUNCTION__, __LINE__, pUpgradeBufInfo->Mem[0].Size, pUpgradeBufInfo->Mem[1].Size, pUpgradeBufInfo->uDataSize);
		if (pUpgradeBufInfo->uDataSize + size * nmemb > pUpgradeBufInfo->Mem[0].Size
			&& pUpgradeBufInfo->Mem[0].Size > pUpgradeBufInfo->uDataSize)
		{
			// 往内存里面写
			memcpy((char *)pUpgradeBufInfo->Mem[0].Addr + pUpgradeBufInfo->uDataSize, 
				ptr, pUpgradeBufInfo->Mem[0].Size - pUpgradeBufInfo->uDataSize);

			// 往共享内存里面写
			memcpy((char *)pUpgradeBufInfo->Mem[1].Addr, 
				(char *)ptr + pUpgradeBufInfo->Mem[0].Size - pUpgradeBufInfo->uDataSize,
				size * nmemb - (pUpgradeBufInfo->Mem[0].Size - pUpgradeBufInfo->uDataSize));
			sTransShmLen = size * nmemb - (pUpgradeBufInfo->Mem[0].Size - pUpgradeBufInfo->uDataSize);
		}
		else if (pUpgradeBufInfo->uDataSize >= pUpgradeBufInfo->Mem[0].Size
			&& pUpgradeBufInfo->uDataSize <= pUpgradeBufInfo->Mem[0].Size + pUpgradeBufInfo->Mem[1].Size)
		{
			// 往共享内存里面写
			memcpy((char *)pUpgradeBufInfo->Mem[1].Addr + sTransShmLen, 
				ptr, size * nmemb);

			sTransShmLen += size * nmemb;
		}
		else
		{
			// 往内存里面写
			memcpy((char *)pUpgradeBufInfo->Mem[0].Addr + pUpgradeBufInfo->uDataSize, 
				ptr, size * nmemb);
		}
		nRet = size * nmemb;
	}
	else
	{
		nRet = fwrite(ptr, 1, size * nmemb, pUpgradeBufInfo->pFile);
	}
	pUpgradeBufInfo->uDataSize += size * nmemb;
	
    return nRet;  
}

static int HttpDownLoadCurl(DOWNLOAD_CURL_PARAM *pParam)
{
	if (NULL == pParam)
	{
		return -1;
	}
	
	static CURL *curlHandle = NULL;
	CURLcode curlRet = CURLE_GOT_NOTHING;
	long filesize = 0;
	char szTmp[256] = {0};
	int nTriesCnt = pParam->nTriesCnt;
	UPGRADE_BUF_INFO *pUpgradeBufInfo = (UPGRADE_BUF_INFO *)pParam->pUserData;

	if (NULL == pUpgradeBufInfo)
	{
		return -1;
	}
	
	if (NULL == curlHandle)
	{
		curlHandle = curl_easy_init(); 
	}
	if (NULL == curlHandle)
	{
		return -1;
	}

	//设置URL
	curl_easy_setopt(curlHandle, CURLOPT_URL, pParam->szURL);
	//设置用户名密码
	memset(szTmp, 0, sizeof(szTmp));
	snprintf(szTmp, sizeof(szTmp), "%s:%s", 
		pParam->szUserName, pParam->szPassword);
    curl_easy_setopt(curlHandle, CURLOPT_USERPWD, szTmp);
	//屏蔽cur处理异常时生成SIGNAL信号
	//否则该信号会造成主进程被kill
	curl_easy_setopt(curlHandle, CURLOPT_NOSIGNAL, 1L);
    //连接超时设置 
    curl_easy_setopt(curlHandle, CURLOPT_CONNECTTIMEOUT, pParam->nConnectTimeoutSec);
	curl_easy_setopt(curlHandle, CURLOPT_TIMEOUT, pParam->nTimeoutSec);
    //设置头处理函数  
    curl_easy_setopt(curlHandle, CURLOPT_HEADERFUNCTION, GetContentLengthFunc);
    curl_easy_setopt(curlHandle, CURLOPT_HEADERDATA, &filesize);
	curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, WriteFunc);
    curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, pParam->pUserData);
    curl_easy_setopt(curlHandle, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curlHandle, CURLOPT_VERBOSE, 1L);
	//设置最低速度为CLOUD_UPGRADE_DOWNLOAD_LOW_SPEED_LIMIT
	curl_easy_setopt(curlHandle, CURLOPT_LOW_SPEED_LIMIT, CLOUD_UPGRADE_DOWNLOAD_LOW_SPEED_LIMIT);
	//设置最低速度允许持续的最长时间为CLOUD_UPGRADE_DOWNLOAD_LOW_SPEED_TIME
	curl_easy_setopt(curlHandle, CURLOPT_LOW_SPEED_TIME, CLOUD_UPGRADE_DOWNLOAD_LOW_SPEED_TIME);
	while (1)
	{
		#if 0
		// 设置断点续传  
		if (nTriesCnt == pParam->nTriesCnt)
		{
    		curl_easy_setopt(curlHandle, CURLOPT_RESUME_FROM_LARGE, 0);
		}
		else
		{
    		curl_easy_setopt(curlHandle, CURLOPT_RESUME_FROM_LARGE, pUpgradeBufInfo->uDataSize);
		}
		#else
		if (NULL == pUpgradeBufInfo->pFile)
		{
			pUpgradeBufInfo->uDataSize = 0;
		}
		else
		{
			fseek(pUpgradeBufInfo->pFile, 0L, SEEK_SET);
		}
		#endif

		//开始传输
		curlRet = curl_easy_perform(curlHandle);
	    if (CURLE_OK == curlRet)
	    {
	        break;
	    }
	    else 
	    {
			CLUP_DEBUG("curl_easy_strerror=%s\n", curl_easy_strerror(curlRet));
	    }

		nTriesCnt--;
		if (0 == nTriesCnt)
		{
			break;
		}
	}

	if (CURLE_OK != curlRet)
	{
		return 1;
	}

	return 0;
}

static int FtpDownLoadCurl(DOWNLOAD_CURL_PARAM *pParam)
{
	if (NULL == pParam)
	{
		return -1;
	}
	
	static CURL *curlHandle = NULL;
	CURLcode curlRet = CURLE_GOT_NOTHING;
	char szTmp[256] = {0};
	int nTriesCnt = pParam->nTriesCnt;
	UPGRADE_BUF_INFO *pUpgradeBufInfo = (UPGRADE_BUF_INFO *)pParam->pUserData;

	if (NULL == pUpgradeBufInfo)
	{
		return -1;
	}
	
	if (NULL == curlHandle)
	{
		curlHandle = curl_easy_init(); 
	}
	if (NULL == curlHandle)
	{
		return -1;
	}

	//设置URL
	curl_easy_setopt(curlHandle, CURLOPT_URL, pParam->szURL);
	//设置用户名密码
	memset(szTmp, 0, sizeof(szTmp));
	CLUP_DEBUG("\nusrname=%s len=%d passwd=%s len=%d\n",
		pParam->szUserName, strlen(pParam->szUserName),
		pParam->szPassword, strlen(pParam->szPassword));
	#if 1
	snprintf(szTmp, sizeof(szTmp), "%s:%s", 
		pParam->szUserName, pParam->szPassword);
	#else
	snprintf(szTmp, sizeof(szTmp), "ftpuser:echo123");
	#endif
	curl_easy_setopt(curlHandle, CURLOPT_USERPWD, szTmp);
	//屏蔽cur处理异常时生成SIGNAL信号
	//否则该信号会造成主进程被kill
	curl_easy_setopt(curlHandle, CURLOPT_NOSIGNAL, 1L);
	//连接超时设置 
	curl_easy_setopt(curlHandle, CURLOPT_CONNECTTIMEOUT, pParam->nConnectTimeoutSec);
	curl_easy_setopt(curlHandle, CURLOPT_TIMEOUT, pParam->nTimeoutSec);
	//设置头处理函数  
	curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, WriteFunc);
	curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, pParam->pUserData);
	curl_easy_setopt(curlHandle, CURLOPT_NOPROGRESS, 1L);
	curl_easy_setopt(curlHandle, CURLOPT_VERBOSE, 1L);
	//设置最低速度为CLOUD_UPGRADE_DOWNLOAD_LOW_SPEED_LIMIT
	curl_easy_setopt(curlHandle, CURLOPT_LOW_SPEED_LIMIT, CLOUD_UPGRADE_DOWNLOAD_LOW_SPEED_LIMIT);
	//设置最低速度允许持续的最长时间为CLOUD_UPGRADE_DOWNLOAD_LOW_SPEED_TIME
	curl_easy_setopt(curlHandle, CURLOPT_LOW_SPEED_TIME, CLOUD_UPGRADE_DOWNLOAD_LOW_SPEED_TIME);
	while (1)
	{
		#if 0
		// 设置断点续传  
		if (nTriesCnt == pParam->nTriesCnt)
		{
			curl_easy_setopt(curlHandle, CURLOPT_RESUME_FROM_LARGE, 0);
		}
		else
		{
			curl_easy_setopt(curlHandle, CURLOPT_RESUME_FROM_LARGE, pUpgradeBufInfo->uDataSize);
		}
		#else
		if (NULL == pUpgradeBufInfo->pFile)
		{
			pUpgradeBufInfo->uDataSize = 0;
		}
		else
		{
			fseek(pUpgradeBufInfo->pFile, 0L, SEEK_SET);
		}
		#endif

		//开始传输
		curlRet = curl_easy_perform(curlHandle);
		if (CURLE_OK == curlRet)
		{
			break;
		}
		else 
		{
			CLUP_DEBUG("curl_easy_strerror=%s\n", curl_easy_strerror(curlRet));
		}

		nTriesCnt--;
		if (0 == nTriesCnt)
		{
			break;
		}
	}

	if (CURLE_OK != curlRet)
	{
		return 1;
	}

	return 0;
}

static int SftpDownLoadCurl(DOWNLOAD_CURL_PARAM *pParam)
{
	if (NULL == pParam)
	{
		return -1;
	}
	
	static CURL *curlHandle = NULL;
	CURLcode curlRet = CURLE_GOT_NOTHING;
	char szTmp[256] = {0};
	int nTriesCnt = pParam->nTriesCnt;
	UPGRADE_BUF_INFO *pUpgradeBufInfo = (UPGRADE_BUF_INFO *)pParam->pUserData;

	if (NULL == pUpgradeBufInfo)
	{
		return -1;
	}

	if (NULL == curlHandle)
	{
		curlHandle = curl_easy_init(); 
	}
	if (NULL == curlHandle)
	{
		return -1;
	}

	//设置URL
	curl_easy_setopt(curlHandle, CURLOPT_URL, pParam->szURL);
	
	//设置用户名密码
	memset(szTmp, 0, sizeof(szTmp));
	CLUP_DEBUG("\nusrname=%s passwd=%s\n",
		pParam->szUserName, pParam->szPassword);
	snprintf(szTmp, sizeof(szTmp), "%s:%s", 
		pParam->szUserName, pParam->szPassword);
	
	curl_easy_setopt(curlHandle, CURLOPT_USERPWD, szTmp);
	//屏蔽cur处理异常时生成SIGNAL信号
	//否则该信号会造成主进程被kill
	curl_easy_setopt(curlHandle, CURLOPT_NOSIGNAL, 1L);
	//连接超时设置 
	curl_easy_setopt(curlHandle, CURLOPT_CONNECTTIMEOUT, pParam->nConnectTimeoutSec);
	curl_easy_setopt(curlHandle, CURLOPT_TIMEOUT, pParam->nTimeoutSec);
	//设置头处理函数  
	curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, WriteFunc);
	curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, pParam->pUserData);
	curl_easy_setopt(curlHandle, CURLOPT_NOPROGRESS, 1L);
	curl_easy_setopt(curlHandle, CURLOPT_VERBOSE, 1L);
	//采用密码方式(非交互式)
	curl_easy_setopt(curlHandle, CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_PASSWORD);
	//设置最低速度为CLOUD_UPGRADE_DOWNLOAD_LOW_SPEED_LIMIT
	curl_easy_setopt(curlHandle, CURLOPT_LOW_SPEED_LIMIT, CLOUD_UPGRADE_DOWNLOAD_LOW_SPEED_LIMIT);
	//设置最低速度允许持续的最长时间为CLOUD_UPGRADE_DOWNLOAD_LOW_SPEED_TIME
	curl_easy_setopt(curlHandle, CURLOPT_LOW_SPEED_TIME, CLOUD_UPGRADE_DOWNLOAD_LOW_SPEED_TIME);

	while (1)
	{
		#if 0
		// 设置断点续传  
		if (nTriesCnt == pParam->nTriesCnt)
		{
			curl_easy_setopt(curlHandle, CURLOPT_RESUME_FROM_LARGE, 0);
		}
		else
		{
			curl_easy_setopt(curlHandle, CURLOPT_RESUME_FROM_LARGE, pUpgradeBufInfo->uDataSize);
		}
		#else
		if (NULL == pUpgradeBufInfo->pFile)
		{
			pUpgradeBufInfo->uDataSize = 0;
		}
		else
		{
			fseek(pUpgradeBufInfo->pFile, 0L, SEEK_SET);
		}
		#endif
		
		//开始传输
		curlRet = curl_easy_perform(curlHandle);
		if (CURLE_OK == curlRet)
		{
			break;
		}
		else 
		{
			CLUP_DEBUG("curl_easy_strerror=%s\n", curl_easy_strerror(curlRet));
		}

		nTriesCnt--;
		if (0 == nTriesCnt)
		{
			break;
		}
	}

	if (CURLE_OK != curlRet)
	{
		return 1;
	}

	return 0;
}

/*下载接口*/
static int DownLoadCurl(DOWNLOAD_CURL_PARAM *pParam)
{
	if (NULL == pParam)
	{
		return -1;
	}

	int nRet = 0;
	CURLcode curlRet = CURLE_GOT_NOTHING;
    curlRet = curl_global_init(CURL_GLOBAL_ALL);	//重复调用返回成功
	if (CURLE_OK != curlRet)
	{
		return -1;
	}
	
	if (0 == strcmp(pParam->szProtocol, (char *)CLOUD_UPGRADE_DOWNLOAD_PROTOCOL_HTTP))
	{
		nRet = HttpDownLoadCurl(pParam);
	}
	else if (0 == strcmp(pParam->szProtocol, (char *)CLOUD_UPGRADE_DOWNLOAD_PROTOCOL_FTP))
	{
		nRet = FtpDownLoadCurl(pParam);
	}
	else if (0 == strcmp(pParam->szProtocol, (char *)CLOUD_UPGRADE_DOWNLOAD_PROTOCOL_SFTP))
	{
		nRet = SftpDownLoadCurl(pParam);
	}

	return nRet;
}

//!\brief      设置设备升级列表服务器配置
int MPU_CloudUpgrade_SetUpgradeListServerCfg(UPGRADE_LIST_SERVER_CFG *pCfg)
{
	pthread_mutex_lock(&s_mutexInterface);
	
	int nRet = 0;
	char strKey[32] = {0};
	char strTmp1[256] = {0};
	char strTmp2[256] = {0};
	char *pTmp = NULL;
	int nBinDataLen = 0;
	
	if (NULL == pCfg)
	{
		nRet = IDVR_CIU_STRUCT_UNVALID;
		goto RET;
	}

	//判断协议是否支持(目前HTTP/FTP)
	StrCapitals2SmallLetter(strTmp1, pCfg->szProtocol, strlen(pCfg->szProtocol));
	if (0 == IsSupportProtocol(strTmp1))
	{
	    pthread_mutex_unlock(&s_mutexInterface);
		return IDVR_CIU_CLOUD_UPGRADE_UPGRADE_LIST_DOWNLOAD_PROTCL_UNSUPPORT;
	}
	memcpy(pCfg->szProtocol, strTmp1, sizeof(pCfg->szProtocol));

	//判断IP合法性 http采用域名的方式
	if(!strcmp(pCfg->szProtocol, "ftp"))
	{
		if (0 == IsValidIpAddr(pCfg->szHostIP))
		{
			nRet = IDVR_CIU_CLOUD_UPGRADE_UPGRADE_LIST_SERVER_HOSTIP_INVALID;
			goto RET;
		}
	}
	
	//判断端口合法性
	if (0 == IsValidPort(pCfg->nPort))
	{
		nRet = IDVR_CIU_CLOUD_UPGRADE_UPGRADE_LIST_SERVER_PORT_INVALID;
		goto RET;
	}
	
	//解密服务器账户，并检查是否可正常解密
	#if 0
	///////////////////////////test//////////////////////////
	memset(strKey, 0, sizeof(strKey));
	memset(strTmp1, 0, sizeof(strTmp1));
	memset(strTmp2, 0, sizeof(strTmp2));
	snprintf(strTmp2, sizeof(strTmp2), "qweasdzxc:%s", pCfg->szUserName);
	CLUP_DEBUG("\nstrTmp2=%s\n", strTmp2);
	memcpy(strKey, pCfg->szHostIP, 8);
	nDataLen = ((strlen(strTmp2)+1)+7)&0xfffffff8;
	StdDesEncryption((unsigned char*)strTmp2, (unsigned char*)strTmp1, nDataLen, 
		(unsigned char*)strKey, ENCRYPTION_MODE);
	memset(strTmp2, 0, sizeof(strTmp2));
    base64Encode((unsigned char*)strTmp1, strTmp2, nDataLen);
	snprintf(strTmp2, sizeof(strTmp2), "AZ4IhtYAEtjDoX8jnnrTzA==");
    CLUP_DEBUG("\nstrTmp2=%s\n", strTmp2);
    memset(strTmp1, 0, sizeof(strTmp1));
    base64Decode(strTmp2, (unsigned char*)strTmp1);
	memset(strTmp2, 0, sizeof(strTmp2));
	StdDesEncryption((unsigned char*)strTmp1, (unsigned char*)strTmp2, nDataLen, 
		(unsigned char*)strKey, DECRYPTION_MODE);
	CLUP_DEBUG("\nstrTmp2=%s\n", strTmp2);
	pTmp = strstr(strTmp2, CLOUD_UPGRADE_USRINFO_START_FLAG);
	if (NULL == pTmp)
	{
		memset(pCfg->szUserName, 0, sizeof(pCfg->szUserName));
		memcpy(pCfg->szUserName, strTmp2, sizeof(pCfg->szUserName)-1);
	}
	else
	{
		memset(pCfg->szUserName, 0, sizeof(pCfg->szUserName));
		memcpy(pCfg->szUserName, pTmp+1, sizeof(pCfg->szUserName)-1);
	}

	memset(strKey, 0, sizeof(strKey));
	memset(strTmp1, 0, sizeof(strTmp1));
	memset(strTmp2, 0, sizeof(strTmp2));
	snprintf(strTmp2, sizeof(strTmp2), "qweasdzxc:%s", pCfg->szPassword);
	CLUP_DEBUG("\nstrTmp2=%s\n", strTmp2);
	memcpy(strKey, pCfg->szHostIP, 8);
	nDataLen = ((strlen(strTmp2)+1)+7)&0xfffffff8;
	StdDesEncryption((unsigned char*)strTmp2, (unsigned char*)strTmp1, nDataLen, 
		(unsigned char*)strKey, ENCRYPTION_MODE);
	memset(strTmp2, 0, sizeof(strTmp2));
    base64Encode((const unsigned char*)strTmp1, strTmp2, nDataLen);
	snprintf(strTmp2, sizeof(strTmp2), "dRdIKCGV6OR2KTKi/XGTAg==");
    CLUP_DEBUG("\nstrTmp2=%s\n", strTmp2);
    memset(strTmp1, 0, sizeof(strTmp1));
    base64Decode(strTmp2, (unsigned char*)strTmp1);
	StdDesEncryption((unsigned char*)strTmp1, (unsigned char*)strTmp2, nDataLen, 
		(unsigned char*)strKey, DECRYPTION_MODE);
	CLUP_DEBUG("\nstrTmp2=%s\n", strTmp2);
	pTmp = strstr(strTmp2, CLOUD_UPGRADE_USRINFO_START_FLAG);
	if (NULL == pTmp)
	{
		memset(pCfg->szPassword, 0, sizeof(pCfg->szPassword));
		memcpy(pCfg->szPassword, strTmp2, sizeof(pCfg->szPassword)-1);
	}
	else
	{
		memset(pCfg->szPassword, 0, sizeof(pCfg->szPassword));
		memcpy(pCfg->szPassword, pTmp+1, sizeof(pCfg->szPassword)-1);
	}
	////////////////////////////////////////////////////////
	#else

#if defined (__IS_SUPPORT_P6SMESSAGE__)
	//将数据复制一份，用于p6sMessage升级(用户名和密码用解密之前的，后面再根据文件名的前八位解析)
	if (strlen(pCfg->szUserName) > 0)
	{
		memcpy(s_stUpgradeListServerCfgV1.szUserName, pCfg->szUserName, sizeof(s_stUpgradeListServerCfgV1.szUserName)-1);
	}
	if (strlen(pCfg->szPassword) > 0)
	{
		memcpy(s_stUpgradeListServerCfgV1.szPassword, pCfg->szPassword, sizeof(s_stUpgradeListServerCfgV1.szPassword)-1);
	}

	//将数据复制一份，用于p6sMessage升级
	memcpy(s_stUpgradeListServerCfgV1.szHostIP, pCfg->szHostIP, sizeof(s_stUpgradeListServerCfgV1.szHostIP)-1);
	CLUP_DEBUG("\nszHostIP=%s\n", pCfg->szHostIP);
	s_stUpgradeListServerCfgV1.nPort = pCfg->nPort;
	CLUP_DEBUG("\nnPort=%d\n", pCfg->nPort);
	
	memcpy(s_stUpgradeListServerCfgV1.szPath, pCfg->szPath, sizeof(s_stUpgradeListServerCfgV1.szPath)-1);
	CLUP_DEBUG("\nszPath=%s\n", pCfg->szPath);
	
	memcpy(s_stUpgradeListServerCfgV1.szProtocol, pCfg->szProtocol, sizeof(s_stUpgradeListServerCfgV1.szProtocol)-1);
	CLUP_DEBUG("\nszProtocol=%s\n", pCfg->szProtocol);
#endif

	if (strlen(pCfg->szUserName) > 0)
	{
		memset(strTmp1, 0, sizeof(strTmp1));
		nBinDataLen = base64Decode(pCfg->szUserName, (unsigned char*)strTmp1);
		
		memset(strTmp2, 0, sizeof(strTmp2));
		memset(strKey, 0, sizeof(strKey));
		memcpy(strKey, pCfg->szHostIP, 8);
		nRet = StdDesEncryption((unsigned char*)strTmp1, (unsigned char*)strTmp2, 
			((nBinDataLen+7)&0xfffffff8), (unsigned char*)strKey, DECRYPTION_MODE);
		if (0 != nRet)
		{
			nRet = IDVR_CIU_CLOUD_UPGRADE_UPGRADE_LIST_SERVER_USRNAME_DECRYPT_FAILD;
			goto RET;
		}
		CLUP_DEBUG("\nstrTmpIn=%s strTmpOut=%s\n", pCfg->szUserName, strTmp2);
		
		pTmp = strstr(strTmp2, CLOUD_UPGRADE_USRINFO_START_FLAG);
		if (NULL == pTmp)
		{
			memset(pCfg->szUserName, 0, sizeof(pCfg->szUserName));
			memcpy(pCfg->szUserName, strTmp2, sizeof(pCfg->szUserName)-1);
		}
		else
		{
			memset(pCfg->szUserName, 0, sizeof(pCfg->szUserName));
			memcpy(pCfg->szUserName, pTmp+1, sizeof(pCfg->szUserName)-1);
		}
	}
	
	if (strlen(pCfg->szPassword) > 0)
	{
		memset(strTmp1, 0, sizeof(strTmp1));
		nBinDataLen = base64Decode(pCfg->szPassword, (unsigned char*)strTmp1);
		
		memset(strTmp2, 0, sizeof(strTmp2));
		memset(strKey, 0, sizeof(strKey));
		memcpy(strKey, pCfg->szHostIP, 8);
		
		nRet = StdDesEncryption((unsigned char*)strTmp1, (unsigned char*)strTmp2, 
			((nBinDataLen+7)&0xfffffff8), (unsigned char*)strKey, DECRYPTION_MODE);
		if (0 != nRet)
		{
			nRet = IDVR_CIU_CLOUD_UPGRADE_UPGRADE_LIST_SERVER_PASSWD_DECRYPT_FAILD;
			goto RET;
		}
		CLUP_DEBUG("\nstrTmpIn=%s strTmpOut=%s\n", pCfg->szPassword, strTmp2);
		pTmp = strstr(strTmp2, CLOUD_UPGRADE_USRINFO_START_FLAG);
		if (NULL == pTmp)
		{
			memset(pCfg->szPassword, 0, sizeof(pCfg->szPassword));
			memcpy(pCfg->szPassword, strTmp2, sizeof(pCfg->szPassword)-1);
		}
		else
		{
			memset(pCfg->szPassword, 0, sizeof(pCfg->szPassword));
			memcpy(pCfg->szPassword, pTmp+1, sizeof(pCfg->szPassword)-1);
		}
	}
	nRet = 0;
	#endif
	
	memset(s_stUpgradeListServerCfg.szHostIP, 0, sizeof(s_stUpgradeListServerCfg.szHostIP));
	memcpy(s_stUpgradeListServerCfg.szHostIP, pCfg->szHostIP, sizeof(s_stUpgradeListServerCfg.szHostIP)-1);
	CLUP_DEBUG("\nszHostIP=%s\n", pCfg->szHostIP);
	s_stUpgradeListServerCfg.nPort = pCfg->nPort;
	CLUP_DEBUG("\nnPort=%d\n", pCfg->nPort);
	memset(s_stUpgradeListServerCfg.szPath, 0, sizeof(s_stUpgradeListServerCfg.szPath));
	memcpy(s_stUpgradeListServerCfg.szPath, pCfg->szPath, sizeof(s_stUpgradeListServerCfg.szPath)-1);
	CLUP_DEBUG("\nszPath=%s\n", pCfg->szPath);
	memset(s_stUpgradeListServerCfg.szProtocol, 0, sizeof(s_stUpgradeListServerCfg.szProtocol));
	memcpy(s_stUpgradeListServerCfg.szProtocol, pCfg->szProtocol, sizeof(s_stUpgradeListServerCfg.szProtocol)-1);
	CLUP_DEBUG("\nszProtocol=%s\n", pCfg->szProtocol);
	memset(s_stUpgradeListServerCfg.szUserName, 0, sizeof(s_stUpgradeListServerCfg.szUserName));
	if (strlen(pCfg->szUserName) > 0)
	{
		memcpy(s_stUpgradeListServerCfg.szUserName, pCfg->szUserName, sizeof(s_stUpgradeListServerCfg.szUserName)-1);
	}
	CLUP_DEBUG("\nszUserName=%s\n", pCfg->szUserName);
	memset(s_stUpgradeListServerCfg.szPassword, 0, sizeof(s_stUpgradeListServerCfg.szPassword));
	if (strlen(pCfg->szPassword) > 0)
	{
		memcpy(s_stUpgradeListServerCfg.szPassword, pCfg->szPassword, sizeof(s_stUpgradeListServerCfg.szPassword)-1);
	}
	CLUP_DEBUG("\nszPassword=%s\n", pCfg->szPassword);
	
RET:
	if (0 == nRet)
	{
		if (enCloudUpgradeStageCur < CLOUD_UPGRADE_STATGE_SET_UPGRADE_LIST_SERVER_CFG)
		{
			enCloudUpgradeStageCur = CLOUD_UPGRADE_STATGE_SET_UPGRADE_LIST_SERVER_CFG;
		}
	}
	pthread_mutex_unlock(&s_mutexInterface);
	return nRet;
}

/**
 * @brief 获取云升级URL
 * @version 202005 在1.1.4 版本之前，没有使用curVersion字段，本版本之后，使用curVersion字段
 * 当前服务器逻辑：使用了curVersion才能搜索得到升级包
 * 
 * 旧的URL示例：
 * http://58.221.67.93:8080/UtilERP/device/CloudUpgradeFirmwareList?boardID=207A&customer=C000&
 *                          versionID=V001&lang=SimpChinese&deviceType=507F&type=all 
 * 新的URL示例：
 * http://139.9.6.140:8090/UtilERP/device/CloudUpgradeFirmwareList?boardID=207F&customer=C007&
 *                          versionID=V001&lang=SimpChinese&deviceType=5086&type=all&curVersion=1.1.4-20200507
 * 
 */
static int MPU_CU_API_GetUrl (char *szUrl, int nUrlSize, const UPGRADE_LIST_SERVER_CFG_LOCAL *psServer, const BOARDID_PRODUCT_MODEL_MAP *psDev)
{
    int nRet = 0;

    char szClientId[24] = {};
    char szCustomNo[24] = {};
     //szCustomNo 读取定制信息
    CConfig *pCfg = CConfig::GetInstance();
    if (pCfg->CheckKeyExist(CLOUD_UPGRADE_CUSTOM_SECTION_NAME, CLOUD_UPGRADE_CUSTOM_CUSTOM_NO_KEY_NAME))
    {
        //使用config的定制
        pCfg->GetValue(CLOUD_UPGRADE_CUSTOM_SECTION_NAME, 
            CLOUD_UPGRADE_CUSTOM_CUSTOM_NO_KEY_NAME, 
            CLOUD_UPGRADE_CUSTOM_CUSTOM_NO_VALUE_DEF, 
            szCustomNo, sizeof(szCustomNo));
    }
    else
    {
        //使用custom的定制
        pCfg = CConfig::NewInstance();
        nRet = pCfg->LoadFile(CUSTOM_INFO);
        if (0 == nRet)
        {
            pCfg->GetValue(CLOUD_UPGRADE_CUSTOM_SECTION_NAME, 
                CLOUD_UPGRADE_CUSTOM_CUSTOM_NO_KEY_NAME, 
                CLOUD_UPGRADE_CUSTOM_CUSTOM_NO_VALUE_DEF, 
                szCustomNo, sizeof(szCustomNo));
        }
        CConfig::ReleaseInstance(pCfg);
        pCfg = NULL;
    }

    // szClientId 优先使用授权码内部的customID
	stuShmAuthInfo stShmAuthInfo = {0};
    Authorize_GetAuthDevInfo(&stShmAuthInfo);
    if (AUTH_SUCCESS == stShmAuthInfo.dwAuthVerifyErrno)
    {
        snprintf(szClientId, sizeof(szClientId), "%X", stShmAuthInfo.dCustomID);
    }
    printf("[xxxx] Custom info !ClientId=%s CustomNo=%s\n", szClientId, szCustomNo);

    if (0 == strlen(szClientId)
        || 0 == strlen(szCustomNo))
    {
        CLUP_ERROR("Custom info error!ClientId=%s CustomNo=%s\n", 
            szClientId, szCustomNo);
        return IDVR_CIU_CLOUD_UPGRADE_UPGRADE_LIST_CUSTOM_NO_UNSUPPORT;
    }

    // 获取软件版本
    DEVICEVERSION sDevVer = {};
    MPU_PUB_GetDeviceVersion (& sDevVer,0);
	

#ifdef __MODULE_4G_SUPPORT__
	int is4g = MPU_Net_Module4G_GetCardBindMode();
    snprintf(szUrl, nUrlSize, "%s://%s:%d/UtilERP/device/CloudUpgradeFirmwareList?"
                                "boardID=%X&customer=%s&versionID=%s&lang=%s&deviceType=%X&type=%s&"
                                "curVersion==%s&is4g=%d",
        psServer->szProtocol,
        psServer->szHostIP,
        psServer->nPort,
        psDev->nBoardId,
        szClientId, 
        szCustomNo,
        PUB_SpeechLanguage_enum2str(),
        psDev->nDeviceType, 
        "all",
        sDevVer.m_szSoftwareVersion,
        is4g);

 #endif
 
     snprintf(szUrl, nUrlSize, "%s://%s:%d/UtilERP/device/CloudUpgradeFirmwareList?"
                                "boardID=%X&customer=%s&versionID=%s&lang=%s&deviceType=%X&type=%s&"
                                "curVersion==%s",
        psServer->szProtocol,
        psServer->szHostIP,
        psServer->nPort,
        psDev->nBoardId,
        szClientId, 
        szCustomNo,
        PUB_SpeechLanguage_enum2str(),
        psDev->nDeviceType, 
        "all",
        sDevVer.m_szSoftwareVersion);
	      
        // 待服务器支持获取最新版本信息后再使用default
        // ((0 == pNetCfg->nVersionType) ? "default" : "all"));
    
    return nRet;
}

/*下载升级列表*/
static int DownloadUpgradeList(UPGRADE_FIRMWARE_INFO_LIST *pNetCfg)
{
	int i = 0;
	int nRet = 0;
	int nIndexMax = 0;
	int nBoardID = 0;
	char strBuffer[256] = {0};
	DOWNLOAD_CURL_PARAM stDownloadCurlParam = {0};
	BOARDID_PRODUCT_MODEL_MAP s_ProductModelMapSet = {0};
	// stuShmAuthInfo stShmAuthInfo = {0};

	/*填充协议*/
	memset(stDownloadCurlParam.szProtocol, 0, sizeof(stDownloadCurlParam.szProtocol));
	memcpy(stDownloadCurlParam.szProtocol, s_stUpgradeListServerCfg.szProtocol, 
		sizeof(stDownloadCurlParam.szProtocol));

	/*填充用户名*/
	memset(stDownloadCurlParam.szUserName, 0, sizeof(stDownloadCurlParam.szUserName));
	memcpy(stDownloadCurlParam.szUserName, s_stUpgradeListServerCfg.szUserName, 
		sizeof(stDownloadCurlParam.szUserName));

	/*填充密码*/
	memset(stDownloadCurlParam.szPassword, 0, sizeof(stDownloadCurlParam.szPassword));
	memcpy(stDownloadCurlParam.szPassword, s_stUpgradeListServerCfg.szPassword, 
		sizeof(stDownloadCurlParam.szPassword));
	
	/*填充URL*/
	ZWSys_GetBoardInfo(&nBoardID);

	/* http://58.221.67.93:8080/UtilERP/device/CloudUpgradeFirmwareList?boardID=207A&customer=C000&versionID=V001&lang=SimpChinese&deviceType=507F&type=all */
	nIndexMax = gstuBoardInfoSize;
	for (i = 0; i < nIndexMax; i++)
	{
		if ((unsigned int)nBoardID == gstuBoardInfo[i].dwBoardId)
		{
			s_ProductModelMapSet.nBoardId = gstuBoardInfo[i].dwBoardId;
			s_ProductModelMapSet.nDeviceType = gstuBoardInfo[i].nDeviceType;			
			memcpy(s_ProductModelMapSet.szProductModel, gstuBoardInfo[i].szBoardType, sizeof(s_ProductModelMapSet.szProductModel));
			
			MPU_CU_API_GetUrl (strBuffer, sizeof(strBuffer), &s_stUpgradeListServerCfg, &s_ProductModelMapSet);
            // memset(s_szClientId, 0, sizeof(s_szClientId));
            // memset(s_szCustomNo, 0, sizeof(s_szCustomNo));
			// //读取定制信息
			// CConfig *pCfg = CConfig::GetInstance();
            // if (pCfg->CheckKeyExist(CLOUD_UPGRADE_CUSTOM_SECTION_NAME, CLOUD_UPGRADE_CUSTOM_CUSTOM_NO_KEY_NAME))
            // {
            //     //使用config的定制
            //     pCfg->GetValue(CLOUD_UPGRADE_CUSTOM_SECTION_NAME, 
            //         CLOUD_UPGRADE_CUSTOM_CUSTOM_NO_KEY_NAME, 
            //         CLOUD_UPGRADE_CUSTOM_CUSTOM_NO_VALUE_DEF, 
            //         s_szCustomNo, sizeof(s_szCustomNo));
            // }
            // else
            // {
            //     //使用custom的定制
            //     pCfg = CConfig::NewInstance();
            //     nRet = pCfg->LoadFile(CUSTOM_INFO);
            //     if (0 == nRet)
            //     {
            //         pCfg->GetValue(CLOUD_UPGRADE_CUSTOM_SECTION_NAME, 
            //             CLOUD_UPGRADE_CUSTOM_CUSTOM_NO_KEY_NAME, 
            //             CLOUD_UPGRADE_CUSTOM_CUSTOM_NO_VALUE_DEF, 
            //             s_szCustomNo, sizeof(s_szCustomNo));
            //     }
            //     CConfig::ReleaseInstance(pCfg);
            //     pCfg = NULL;
            // }

			// //优先使用授权码内部的customID
			// Authorize_GetAuthDevInfo(&stShmAuthInfo);
			// if (AUTH_SUCCESS == stShmAuthInfo.dwAuthVerifyErrno)
			// {
			// 	memset(s_szClientId, 0, sizeof(s_szClientId));
			// 	snprintf(s_szClientId, sizeof(s_szClientId), "%X", stShmAuthInfo.dCustomID);
			// }

			// if (0 == strlen(s_szClientId)
			// 	|| 0 == strlen(s_szCustomNo))
			// {
			// 	CLUP_ERROR("Custom info error!ClientId=%s CustomNo=%s\n", 
			// 		s_szClientId, s_szCustomNo);
			// 	return IDVR_CIU_CLOUD_UPGRADE_UPGRADE_LIST_CUSTOM_NO_UNSUPPORT;
			// }

			// //获取当前语音语言
			// CConfig *pcfg = CConfig::GetInstance();
			// eLanguageType languageType = (eLanguageType)pcfg->GetValue("PUB", "SpeechLanguage", (int)PUB_eLanguage_Chinese);

			// snprintf(strBuffer, sizeof(strBuffer), "%s://%s:%d/UtilERP/device/CloudUpgradeFirmwareList?boardID=%X&customer=%s&versionID=%s&lang=%s&deviceType=%X&type=%s",
			// 	s_stUpgradeListServerCfg.szProtocol,
			// 	s_stUpgradeListServerCfg.szHostIP,
			// 	s_stUpgradeListServerCfg.nPort,
			// 	s_ProductModelMapSet[i].nBoardId,
			// 	s_szClientId, s_szCustomNo,SpeechLanguage_enum2str(languageType),
			// 	s_ProductModelMapSet[i].nDeviceType, "all");
			// 	// 待服务器支持获取最新版本信息后再使用default
			// 	// ((0 == pNetCfg->nVersionType) ? "default" : "all"));
			break;
		}
	}
	if (i == nIndexMax)
	{
		CLUP_ERROR("Unsupport BoardId:%#x\n", nBoardID);
		return IDVR_CIU_CLOUD_UPGRADE_UPGRADE_LIST_BOARD_ID_UNSUPPORT;
	}
    
	CLUP_DEBUG("\nstrBuffer=%s\n", strBuffer);
	memset(stDownloadCurlParam.szURL, 0, sizeof(stDownloadCurlParam.szURL));
	memcpy(stDownloadCurlParam.szURL, strBuffer, sizeof(stDownloadCurlParam.szURL));

	/*填充其他下载参数*/
	UPGRADE_BUF_INFO stUpgradeBufInfo = {0};
	stDownloadCurlParam.nConnectTimeoutSec = CLOUD_UPGRADE_LIST_DOWNLOAD_CONNECT_TIMEOUT;
	stDownloadCurlParam.nTimeoutSec = CLOUD_UPGRADE_LIST_DOWNLOAD_TIMEOUT;
	stDownloadCurlParam.nTriesCnt = CLOUD_UPGRADE_LIST_DOWNLOAD_TRIES_CNT;
	stDownloadCurlParam.pUserData = (void *)(&stUpgradeBufInfo);

	/*启动下载*/
	memset(&stUpgradeBufInfo, 0, sizeof(UPGRADE_BUF_INFO));
	stUpgradeBufInfo.pFile = fopen(UPGRADE_LIST_FILE, "w+");
    if (NULL == stUpgradeBufInfo.pFile)
    {
        CLUP_ERROR("open file failed [%s]\n", UPGRADE_LIST_FILE);
		return IDVR_CIU_CLOUD_UPGRADE_UPGRADE_LIST_SAVE_FAILD;
    }
	nRet = DownLoadCurl(&stDownloadCurlParam);
	fclose(stUpgradeBufInfo.pFile);
	stUpgradeBufInfo.pFile = NULL;
	if (0 != nRet)
	{
		return IDVR_CIU_CLOUD_UPGRADE_UPGRADE_LIST_DOWNLOAD_FAILD;
	}

	return nRet;
}

static void UpgradeListSortByVersion(UPGRADE_FIRMWARE_INFO *list, int size)
{
	if (NULL == list || size <= 0)
	{
		CLUP_ERROR("param err list[%p] size[%d]\n", list, size);
		return;
	}

	int len = 0;
	int i,j;
	UPGRADE_FIRMWARE_INFO tmp;

	// 获取有效长度
	for (i = 0; i<size; i++)
	{
		if (0 == list[i].bVaild)
		{
			break;
		}
	}
	len = i;

	for (i = 1; i < len; i++)
	{
		for (j = 0; j < len - 1; j++)
		{
			CLUP_DEBUG("\ni=%d j=%d name s1=%s s2=%s\n", i, j, list[j].szFirmwareVersion, list[j+1].szFirmwareVersion);
            int bComp = 0;
            MPU_CloudU_VersionComp(list[j].szFirmwareVersion, list[j+1].szFirmwareVersion, &bComp);
            if (bComp < 0) /* 不考虑解析失败的情况 */
			// if (strncmp(list[j].szFirmwareVersion, list[j+1].szFirmwareVersion, MAX_FIRMWARE_VERSION_LEN) < 0)
			{
				tmp = list[j];
				list[j] = list[j+1];
				list[j+1] = tmp;
			}
		}
	}

	return;
}

/**
 * @brief 解析版本号，获取版本及日期
 * 目前考虑的可能性
 * V1.1.4-20200519Z1S
 * 1.1.4-20200519Z1S
 * V1.1.4_20200519Z1S
 * 1.1.4-20200519Z1S
 * @return 成功返回0，否则返回-1
 */
static int MPU_CloudU_ParseVersionString (const char *szVersionString, char *szVersion, int nVersionSize, char *szDate, int nDateSize)
{
	const unsigned int nLen = strlen (szVersionString);

    const char szVersionDefault[] = "0.0.0";
    const char szDateDefault[] = "00000000";
    const char szMin[] = "0.0.0-20200520";

    snprintf (szVersion, nVersionSize, "%s", szVersionDefault);
    snprintf (szDate, nDateSize, "%s", szDateDefault);

    if (nLen < sizeof(szMin) - 1)
    {
        CLUP_ERROR ("nLenError\n");
        return -1;
    }

	int nVersionHead = -1;
	int nDateHead = -1;

	/* 找第一个数字 */
    unsigned int i = 0;
    for (i = 0; i < nLen; ++i)
    {
        char cIt = szVersionString[i];
        if (cIt >= '0' && cIt <= '9')
        {
            nVersionHead = i;
            break;
        }
    }

    /* 接着找 '-' 或 '_' */
    for (; i < nLen; ++i) // i 接着用
    {
        char cIt = szVersionString[i];
        if (cIt == '-' || cIt == '_')
        {
            nDateHead = i + 1;
            break;
        }
    }

    if (nVersionHead < 0 || nDateHead < 0)
    {
        CLUP_ERROR ("string error\n");
        return -1;
    }

    const int nRealVersionSize = (nDateHead - nVersionHead - 1);
    const int nRealDateSize = sizeof ("20200520") - 1;

    int nCopy = nRealVersionSize < nVersionSize ? nRealVersionSize : nVersionSize;
	strncpy (szVersion, & szVersionString[nVersionHead], nCopy);

    nCopy = nRealDateSize < nDateSize ? nRealDateSize : nDateSize;
	strncpy (szDate, & szVersionString[nDateHead], nCopy);

    szVersion[nVersionSize - 1] = '\0';
    szDate[nDateSize - 1] = '\0';

    return 0;
}

static int GetVersionInfo(const char *version1, const int sizev1, const char *Date1, unsigned int *base_major, unsigned int *base_minor, unsigned int *base_reversion, unsigned int *base_date)
{
	if (!version1 || !sizev1 || !Date1 || !base_major || !base_minor || !base_reversion || !base_date)
	{
		return -1;
	}

	char *SaveVersion1[3] = {NULL}; 	// 存储服务器版本号分割后的数字

	char versiontmp[64]= {0};
	MPU_ERR("version1 :%s sizev1 :%d \n, ", version1, sizev1);
	memcpy(versiontmp, version1, sizev1);
	char *pversion = &versiontmp[0];
	
	// 获取起始地址
	SaveVersion1[0] = versiontmp;
	int i = 1;
	while(1)
	{
		if('\0' != *pversion)
		{
			if('.' == *pversion)
			{
				SaveVersion1[i] = pversion+1;
				*pversion = '\0';
				i++;
				if (3 == i)
				{
					break;
				}
			}
			pversion++;
		}
		else
		{
			break;
		}
		
	}
	for (int x = 0;  x < 3; ++ x)
	{
		if (SaveVersion1[x])
		{
			MPU_ERR("SaveVersion1[%d] :%s \n", x, SaveVersion1[x]);
		}
		else
		{
			MPU_ERR("invalid pointer \n");
			return 0;
		}
	}

	/*--------Step 3 字符型转整型---------------*/
	*base_major	   =  atoi(SaveVersion1[0]);
	*base_minor	   =  atoi(SaveVersion1[1]);
	*base_reversion = atoi(SaveVersion1[2]);
	*base_date	   =  atoi(Date1);

	return 0;
}

static int MPU_CloudU_CompareVersion(const char *version1, const int sizev1, const char *version2, int sizev2, const char *Date1, const char *Date2)
{
	//szVersionA=2.0.10,szDateA=20230407,szVersionB=2.0.3,szDateB=20230407

	if(!version1 ||  !version2 ||  !Date1 || !Date2)		
	{
		MPU_ERR("invalid pointer \n");
		return 0;
	}

	unsigned int base_major = 0, base_minor = 0, base_reversion = 0, base_date = 0;//服务器版本号信息
	unsigned int cur_major = 0, cur_minor = 0, cur_reversion = 0, cur_date = 0;    //本地版本号信息

    GetVersionInfo(version1, sizev1, Date1, &base_major, &base_minor, &base_reversion,  &base_date);
    GetVersionInfo(version2, sizev2, Date2, &cur_major,  &cur_minor,  &cur_reversion,   &cur_date);

   /*--------比较版本 1：服务器版本大于本地版本 0：本地版本最新---------------*/
   MPU_ERR("version info --%u-%u-%u_%u-----%u-%u-%u_%u-----\n",base_major,base_minor,base_reversion,base_date,cur_major,cur_minor,cur_reversion,cur_date);
   if ((cur_major < base_major)
	   || (cur_major == base_major && cur_minor < base_minor)
	   || (cur_major == base_major && cur_minor == base_minor && cur_reversion < base_reversion)
	   || (cur_major == base_major && cur_minor == base_minor && cur_reversion == base_reversion && cur_date < base_date))
   {
	   return 1;
   }
   else
   {
	   return 0;
   }   
}

/**
 * @brief 比较两个版本号
 * 目前考虑的可能性
 * V1.1.4-20200519Z1S
 * 1.1.4-20200519Z1S
 * V1.1.4_20200519Z1S
 * 1.1.4-20200519Z1S
 */
static int MPU_CloudU_VersionComp (const char *szVersionStringA, const char *szVersionStringB, int *pnResult)
{
	char szVersionA[64] = {};
	char szDateA[64] = {};

	char szVersionB[64] = {};
	char szDateB[64] = {};

    MPU_CloudU_ParseVersionString (szVersionStringA, szVersionA, sizeof (szVersionA), szDateA, sizeof(szDateA));
    MPU_CloudU_ParseVersionString (szVersionStringB, szVersionB, sizeof(szVersionB), szDateB, sizeof(szDateB));

    printf ("szVersionA=%s,szDateA=%s,szVersionB=%s,szDateB=%s\n",
            szVersionA,szDateA,
            szVersionB,szDateB);


	int bComp = MPU_CloudU_CompareVersion(szVersionA, sizeof(szVersionA), szVersionB, sizeof(szVersionB), szDateA,szDateB);
    if (bComp)
    {
        *pnResult = bComp;
    }
    else 
    {

        *pnResult = bComp;
    }

    return 0;
}

/**
 * @brief 判断某个版本号是否高于当前版本
 * @return 成功返回 0-否 1-是 
 */
static int MPU_CloudU_IsNewestVersion (const char *szVersionString)
{
    //获取当前设备版本信息
    DEVICEVERSION dv = {0};
    MPU_PUB_GetDeviceVersion(&dv,0);
    
    int bComp = 0;
    MPU_CloudU_VersionComp (szVersionString, dv.m_szSoftwareVersion, &bComp);

    return (bComp > 0 ? 1 : 0);
}

/**
 * @brief 判断某个版本号是否大于等于1.1.4
 * @return 成功返回0-否 1-是 
 */
static int MPU_CloudU_IsVersionNotBeforeV1_1_4(const char *szVersionString)
{
    int bComp = 0;
    MPU_CloudU_VersionComp (szVersionString, "1.0.2-00000000", &bComp);

    return (bComp > 0 ? 1 : 0);
}

/*解析升级列表文件，获取升级列表信息*/
static int ParseUpgradeList(UPGRADE_FIRMWARE_INFO_LIST *pCfg)
{
	if (NULL == pCfg)
	{
		return IDVR_CIU_STRUCT_UNVALID;
	}

	int i = 0;
	char strBuffer[4096] = {0};
    mxml_node_t *root_node = NULL;
    mxml_node_t *firmware_node = NULL;
	mxml_node_t *desc_node = NULL;
    mxml_node_t *node = NULL;
    const char *value = NULL;
	const char *attr = NULL;
	
	FILE *file_new  = fopen(UPGRADE_LIST_FILE, "rb");
    if (NULL == file_new)
    {
        CLUP_ERROR("open file failed [%s]\n", UPGRADE_LIST_FILE);
		return IDVR_CIU_CLOUD_UPGRADE_UPGRADE_LIST_OPEN_FAILD;
    }
	// #if 0
	// 导入一个xml文件，MXML定义了如下几种回调函数: 
	// 	MXML_INTEGER_CALLBACK - 所有的数据节点包含以空格分割的整数。
	// 	MXML_OPAQUE_CALLBACK - 所有的数据节点包含"不透明"字符串（CDATA）。
	// 	MXML_REAL_CALLBACK - 所有的数据节点包含以空格分割的浮点数。
	// 	MXML_TEXT_CALLBACK - 所有的数据节点包含以空格分割的文本字符串。
	// #endif
	mxml_node_t *xml = mxmlLoadFile(NULL, file_new, MXML_OPAQUE_CALLBACK);
	fclose(file_new);
	file_new = NULL;
	if (NULL == xml)
    {
        CLUP_ERROR("Parse Upgrade List failed [%s]\n", UPGRADE_LIST_FILE);
		return IDVR_CIU_CLOUD_UPGRADE_UPGRADE_LIST_PARSE_FAILD;
    }
	
	#if 0
	// MXML_DESCEND 含义是一直向下直到树的根部
	MXML_DESCEND_FIRST 含义是向下搜索到一个节点的第一个匹配子节点，但不再继续向下搜索
	MXML_NO_DESCEND 含义是不查看任何的子节点在XML元素层次中，仅查看同级的伙伴节点或者父节点直到到达顶级节点或者给出的树的顶级节点.
	#endif
	// 存储升级列表
	UPGRADE_FIRMWARE_INFO upgradeFileList[MAX_SUPPORT_FIRMWARE_VERSION_CNT];
	memset(upgradeFileList, 0, sizeof(upgradeFileList));

    for (root_node = mxmlFindElement(xml, xml, "root", NULL, NULL, MXML_DESCEND);
		root_node != NULL;
		root_node = mxmlFindElement(root_node, xml, "root", NULL, NULL, MXML_DESCEND))
    {
        /* 升级文件信息*/
		i = 0;
        for (firmware_node = mxmlFindElement(root_node, xml, "firmware", NULL, NULL, MXML_DESCEND);
			firmware_node != NULL;
			firmware_node = mxmlFindElement(firmware_node, xml, "firmware", NULL, NULL, MXML_DESCEND))
        {
			// 升级包版本 2.1.4-20190305
			attr = mxmlElementGetAttr(firmware_node, "version");
	        if (attr)
	        {
	            CLUP_DEBUG("\nFirmwareVersion attr=%s\n", attr);
				strncpy(upgradeFileList[i].szFirmwareVersion, attr, 
					sizeof(upgradeFileList[i].szFirmwareVersion) - 1);
	        }
			
			// 升级包大小 单位:字节
			node = mxmlFindElement(firmware_node, xml, "file", NULL, NULL, MXML_DESCEND);
            if (node)
            {
				attr = mxmlElementGetAttr(node, "filesize");
		        if (attr)
		        {
		            CLUP_DEBUG("\nfilesize attr=%s\n", attr);
					upgradeFileList[i].nTotalSize = atoi(attr);
		        }
            }
			
			#if 0
			如果选择了MXML_OPAQUE_CALLBACK作为回调函数，可以解析带空格的字符串，MXML默认解析字符串
			是以空格分隔的，在获取的时候，不能直接按照mxmlGetText获取，否则会被空格给隔断，会忽略
			空格后面的字符串，需要按照node->child->value.opaque的方式获取
			#endif
			// 升级包名称  nvr.NBD-27xxNEA_V2.1.4_20190305.bin
            node = mxmlFindElement(firmware_node, xml, "name", NULL, NULL, MXML_DESCEND);
            if (node && node->child)
            {
                value = node->child->value.opaque;
                if (strlen(value))
                {
                    strncpy(upgradeFileList[i].szFirmwareName, value, sizeof(upgradeFileList[i].szFirmwareName) - 1);
					CLUP_DEBUG("\ni=%d name value=%s\n", i, upgradeFileList[i].szFirmwareName);
				}
				else
				{
	        		continue;
	        	}
            }
			else
			{
    			continue;
    		}

			//只要存在升级包名,就认为此版本有效
			upgradeFileList[i].bVaild = 1;

            desc_node = mxmlFindElement(firmware_node, xml, "desc", NULL, NULL, MXML_DESCEND);
            if (desc_node)
            {
				for (node = mxmlGetFirstChild(desc_node);
					node != NULL;
					node = mxmlGetNextSibling(node))
	        	{
	        		const char *pElement = mxmlGetElement(node);
					const char *pOpaque = mxmlGetOpaque(node);
	        		if (NULL == pElement
						|| strlen(pElement) <= 0
						|| NULL == pOpaque
						|| strlen(pOpaque) <= 0)
	        		{
	        			continue;
	        		}
					
					memset(strBuffer, 0, sizeof(strBuffer));
					snprintf(strBuffer, sizeof(strBuffer), "<%s>%s</%s>", pElement, pOpaque, pElement);
					strncat(upgradeFileList[i].szDescription, strBuffer, sizeof(strBuffer)-1);
					CLUP_DEBUG("\ni=%d %s\n", i, upgradeFileList[i].szDescription);
				}
            }
			i++;
			
			//最大支持MAX_SUPPORT_FIRMWARE_VERSION_CNT个版本
			if (i >= MAX_SUPPORT_FIRMWARE_VERSION_CNT)
			{
				break;
			}
		}
	}

    /* debug use */
#if 0
    for (i = 0; i < sizeof(upgradeFileList)/sizeof(UPGRADE_FIRMWARE_INFO); i++)
    {
        if (upgradeFileList[i].bVaild)
        {
            CLUP_DEBUG ("upgradeFileList[%d].szFirmwareVersion=%s\n", i, upgradeFileList[i].szFirmwareVersion);
        }
        else 
        {
            CLUP_DEBUG ("upgradeFileList[%d].szFirmwareVersion=%s\n", i, "not valid");
        }
    }
#endif 

	// 对升级列表按版本进行排序
	UpgradeListSortByVersion(upgradeFileList, sizeof(upgradeFileList)/sizeof(UPGRADE_FIRMWARE_INFO));

    /* debug use */
#if 0
    for (i = 0; i < sizeof(upgradeFileList)/sizeof(UPGRADE_FIRMWARE_INFO); i++)
    {
        if (upgradeFileList[i].bVaild)
        {
            CLUP_DEBUG ("upgradeFileList[%d].szFirmwareVersion=%s\n", i, upgradeFileList[i].szFirmwareVersion);
        }
        else 
        {
            CLUP_DEBUG ("upgradeFileList[%d].szFirmwareVersion=%s\n", i, "not valid");
        }
    }
#endif 

    /**
     * 云升级版本策略：
     * 1.获取最新版本：只显示大于当前版本的最新版本
     * 2.获取所有版本：显示大于等于1.1.4版本的所有版本
     */

	// pCfg->nVersionType 0 - 获取最新版本 1 - 获取所有版本
	if (0 == pCfg->nVersionType)
	{
        int ret = MPU_CloudU_IsNewestVersion (upgradeFileList[0].szFirmwareVersion);
        
        if (1 == ret)
        {
            memcpy(&pCfg->struFileList[0], &upgradeFileList[0], sizeof(UPGRADE_FIRMWARE_INFO));
        }
	}
	else
	{
		memcpy(pCfg->struFileList, upgradeFileList, sizeof(upgradeFileList));
        for (int i = 0; i < MAX_SUPPORT_FIRMWARE_VERSION_CNT; ++i)
        {
            int ret = MPU_CloudU_IsVersionNotBeforeV1_1_4 (pCfg->struFileList[i].szFirmwareVersion);
            if (1 != ret)
            {
                memset (& pCfg->struFileList[i], 0, sizeof(pCfg->struFileList[i]));
            }
        }
	}

#if 0
    /* debug use */
    for (i = 0; i < sizeof(pCfg->struFileList)/sizeof(UPGRADE_FIRMWARE_INFO); i++)
    {
        if (pCfg->struFileList[i].bVaild)
        {
            CLUP_DEBUG ("pCfg->struFileList[%d].szFirmwareVersion=%s\n", i, pCfg->struFileList[i].szFirmwareVersion);
        }
        else 
        {
            CLUP_DEBUG ("pCfg->struFileList[%d].szFirmwareVersion=%s\n", i, "not valid");
        }
    }
#endif 

	/*释放xml*/
	mxmlDelete(xml);
    xml = NULL;

	return 0;
}

//!\brief      获取设备升级列表(阻塞式)
int MPU_CloudUpgrade_GetUpgradeList(UPGRADE_FIRMWARE_INFO_LIST *pCfg)
{
	pthread_mutex_lock(&s_mutexInterface);

	int nRet = 0;
	
	if (NULL == pCfg)
	{
		nRet = IDVR_CIU_STRUCT_UNVALID;
		goto RET;
	}

	//检测是否已经配置过升级列表服务器地址
	if (enCloudUpgradeStageCur < CLOUD_UPGRADE_STATGE_SET_UPGRADE_LIST_SERVER_CFG)
	{
		nRet = IDVR_CIU_CLOUD_UPGRADE_UPGRADE_LIST_SERVER_CFG_NULL;
		goto RET;
	}
	
	nRet = DownloadUpgradeList(pCfg);
	if (0 != nRet)
	{
		goto RET;
	}

	nRet = ParseUpgradeList(pCfg);
	if (0 != nRet)
	{
		goto RET;
	}
	
RET:
	if (0 == nRet)
	{
		if (enCloudUpgradeStageCur < CLOUD_UPGRADE_STATGE_GET_UPGRADE_LIST)
		{
			enCloudUpgradeStageCur = CLOUD_UPGRADE_STATGE_GET_UPGRADE_LIST;
		}
	}
	pthread_mutex_unlock(&s_mutexInterface);
	return nRet;
}

/*升级包下载*/
static int DownloadUpgradeFile()
{
	int nRet = 0;
	char strBuffer[256] = {0};
#if defined (__IS_SUPPORT_P6SMESSAGE__)
	char strKey[32] = {0};
	char strTmp1[256] = {0};
	char strTmp2[256] = {0};
	char *pTmp = NULL;
	int nBinDataLen = 0;
#endif

	DOWNLOAD_CURL_PARAM stDownloadCurlParam = {0};

	if(ManualUpgrade == g_nUpgradeType)
	{
		/*填充协议*/
		memset(stDownloadCurlParam.szProtocol, 0, sizeof(stDownloadCurlParam.szProtocol));
		memcpy(stDownloadCurlParam.szProtocol, s_stUpgradeFileDownloadInfo.szProtocol, 
			sizeof(stDownloadCurlParam.szProtocol));

		/*填充用户名*/
		memset(stDownloadCurlParam.szUserName, 0, sizeof(stDownloadCurlParam.szUserName));
		memcpy(stDownloadCurlParam.szUserName, s_stUpgradeFileDownloadInfo.szUserName, 
			sizeof(stDownloadCurlParam.szUserName));

		/*填充密码*/
		memset(stDownloadCurlParam.szPassword, 0, sizeof(stDownloadCurlParam.szPassword));
		memcpy(stDownloadCurlParam.szPassword, s_stUpgradeFileDownloadInfo.szPassword, 
			sizeof(stDownloadCurlParam.szPassword));

		/*填充URL*/
		//"http://192.168.3.165:80/downloadpath/IPG-7420PCS-S-W/normal/IPG-7420PCS-S-W_V1.0.0_20171228Z1S.bin"
		//"http://192.168.3.165:80/downloadpath/7420PCS_2058_5055/C000V001/IPG-7420PCS-S-W_V1.0.0_20171228Z1S.bin"
		snprintf(strBuffer, sizeof(strBuffer), "%s://%s:%d/%s", 
			s_stUpgradeFileDownloadInfo.szProtocol,
			s_stUpgradeFileDownloadInfo.szHostIP, 
			s_stUpgradeFileDownloadInfo.nPort,
			s_stUpgradeFileDownloadInfo.szURL);
	}
#if defined (__IS_SUPPORT_P6SMESSAGE__)
	else if(AutoUpgrade == g_nUpgradeType)
	{
		/*填充协议*/
		memset(stDownloadCurlParam.szProtocol, 0, sizeof(stDownloadCurlParam.szProtocol));
		memcpy(stDownloadCurlParam.szProtocol, s_stUpgradeListServerCfgV1.szProtocol, 
			sizeof(stDownloadCurlParam.szProtocol));

		//http没有用户名和密码
		if(!strcmp(stDownloadCurlParam.szProtocol, "http"))
		{
			memset(stDownloadCurlParam.szUserName, 0, sizeof(stDownloadCurlParam.szUserName));
			memset(stDownloadCurlParam.szPassword, 0, sizeof(stDownloadCurlParam.szPassword));
		}
		else
		{
			memset(strTmp1, 0, sizeof(strTmp1));
			nBinDataLen = base64Decode(s_stUpgradeListServerCfgV1.szUserName, (unsigned char*)strTmp1);
				
			memset(strTmp2, 0, sizeof(strTmp2));
			memset(strKey, 0, sizeof(strKey));
			memcpy(strKey, s_stUpgradeFileDownloadInfo.szFirmwareName, 8); 
			nRet = StdDesEncryption((unsigned char*)strTmp1, (unsigned char*)strTmp2, 
			((nBinDataLen+7)&0xfffffff8), (unsigned char*)strKey, DECRYPTION_MODE);
			if (0 != nRet)
			{
				nRet = IDVR_CIU_CLOUD_UPGRADE_UPGRADE_LIST_SERVER_USRNAME_DECRYPT_FAILD;
			}

			pTmp = strstr(strTmp2, CLOUD_UPGRADE_USRINFO_START_FLAG);
			if (NULL == pTmp)
			{
				memset(stDownloadCurlParam.szUserName, 0, sizeof(stDownloadCurlParam.szUserName));
				memcpy(stDownloadCurlParam.szUserName, strTmp2, sizeof(stDownloadCurlParam.szUserName)-1);
			}
			else
			{
				memset(stDownloadCurlParam.szUserName, 0, sizeof(stDownloadCurlParam.szUserName));
				memcpy(stDownloadCurlParam.szUserName, pTmp+1, sizeof(stDownloadCurlParam.szUserName)-1);
			}
			
			memset(strTmp1, 0, sizeof(strTmp1));
			nBinDataLen = base64Decode(s_stUpgradeListServerCfgV1.szPassword, (unsigned char*)strTmp1);
				
			memset(strTmp2, 0, sizeof(strTmp2));
			memset(strKey, 0, sizeof(strKey));
			memcpy(strKey, s_stUpgradeFileDownloadInfo.szFirmwareName, 8); 
				
			nRet = StdDesEncryption((unsigned char*)strTmp1, (unsigned char*)strTmp2, 
					((nBinDataLen+7)&0xfffffff8), (unsigned char*)strKey, DECRYPTION_MODE);
			if (0 != nRet)
			{
				nRet = IDVR_CIU_CLOUD_UPGRADE_UPGRADE_LIST_SERVER_PASSWD_DECRYPT_FAILD;
			}
				
			pTmp = strstr(strTmp2, CLOUD_UPGRADE_USRINFO_START_FLAG);
			if (NULL == pTmp)
			{
				memset(stDownloadCurlParam.szPassword, 0, sizeof(stDownloadCurlParam.szPassword));
				memcpy(stDownloadCurlParam.szPassword, strTmp2, sizeof(stDownloadCurlParam.szPassword)-1);
			}
			else
			{
				memset(stDownloadCurlParam.szPassword, 0, sizeof(stDownloadCurlParam.szPassword));
				memcpy(stDownloadCurlParam.szPassword, pTmp+1, sizeof(stDownloadCurlParam.szPassword)-1);
			}

		}
		
		/*填充URL*/
		//"http://192.168.3.165:80/downloadpath/IPG-7420PCS-S-W/normal/IPG-7420PCS-S-W_V1.0.0_20171228Z1S.bin"
		//"http://192.168.3.165:80/downloadpath/7420PCS_2058_5055/C000V001/IPG-7420PCS-S-W_V1.0.0_20171228Z1S.bin"
		if(!strcmp(stDownloadCurlParam.szProtocol, "http"))
		{
			snprintf(strBuffer, sizeof(strBuffer), "%s://%s%s", 
			s_stUpgradeListServerCfg.szProtocol,
			s_stUpgradeListServerCfg.szHostIP, 
			s_stUpgradeListServerCfg.szPath);
		}
		else 
		{
			snprintf(strBuffer, sizeof(strBuffer), "%s://%s:%d%s", 
			s_stUpgradeListServerCfg.szProtocol,
			s_stUpgradeListServerCfg.szHostIP, 
			s_stUpgradeListServerCfg.nPort,
			s_stUpgradeListServerCfg.szPath);
		}
	}
#endif
	CLUP_DEBUG("\nstrBuffer=%s\n", strBuffer);
	memset(stDownloadCurlParam.szURL, 0, sizeof(stDownloadCurlParam.szURL));
	memcpy(stDownloadCurlParam.szURL, strBuffer, sizeof(stDownloadCurlParam.szURL));

	/*填充其他下载参数*/
	stDownloadCurlParam.nConnectTimeoutSec = CLOUD_UPGRADE_UPGRADE_FILE_DOWNLOAD_CONNECT_TIMEOUT;
	stDownloadCurlParam.nTimeoutSec = CLOUD_UPGRADE_UPGRADE_FILE_DOWNLOAD_TIMEOUT;
	stDownloadCurlParam.nTriesCnt = CLOUD_UPGRADE_UPGRADE_FILE_DOWNLOAD_TRIES_CNT;
	stDownloadCurlParam.pUserData = (void *)(&s_stUpgradeBufInfo);

	/*启动下载*/
	#ifdef UPLOAD_TO_DSP_MEM
	CLUP_DEBUG("\nmyDataStart:\n");
	mysystem("date");
	s_stUpgradeBufInfo.pFile = NULL;
	nRet = DownLoadCurl(&stDownloadCurlParam);
	CLUP_DEBUG("\nmyDataEnd:\n");
	mysystem("date");
	#else
	s_stUpgradeBufInfo.pFile = fopen(UPDATEFILENAME, "wb");
    if (NULL == s_stUpgradeBufInfo.pFile)
    {
        CLUP_ERROR("open file failed [%s]\n", UPDATEFILENAME);
		return -1;
    }
	nRet = DownLoadCurl(&stDownloadCurlParam);
	fclose(s_stUpgradeBufInfo.pFile);
	s_stUpgradeBufInfo.pFile = NULL;
	#endif

	#ifdef __GOKE__
	#ifdef UPLOAD_TO_DSP_MEM
	CLUP_DEBUG("\nmyDataStart:\n");
	mysystem("date");
	s_stUpgradeBufInfo.pFile = NULL;
	nRet = DownLoadCurl(&stDownloadCurlParam);
	CLUP_DEBUG("\nmyDataEnd:\n");
	mysystem("date");
	#else
	CLUP_DEBUG("\nenCloudUpgradeStageCur=%d nDownloadStatus=%d\n",
		enCloudUpgradeStageCur, s_stUpgradeFileDownloadThreadData.nDownloadStatus);
	return -1;
	#endif
	#endif	
	
	if (0 != nRet)
	{
		CLUP_ERROR("\nnRet=%d\n", nRet);
		return -1;
	}

	CLUP_DEBUG("\nuDataSize=%d nFileSize=%d\n", 
		s_stUpgradeBufInfo.uDataSize, s_stUpgradeFileDownloadInfo.nFileSize);
	if (s_stUpgradeBufInfo.uDataSize != (unsigned int)s_stUpgradeFileDownloadInfo.nFileSize)
	{
		CLUP_ERROR("\nuDataSize=%d nFileSize=%d\n", 
			s_stUpgradeBufInfo.uDataSize, s_stUpgradeFileDownloadInfo.nFileSize);
		return -1;
	}
	else
	{
		//计算md5
		memset(strBuffer, 0, sizeof(strBuffer));
		#ifdef UPLOAD_TO_DSP_MEM
		if (1 == s_stUpgradeBufInfo.nCnt)
		{
			ComputeDataMd5sum((unsigned char *)(s_stUpgradeBufInfo.Mem[0].Addr), s_stUpgradeBufInfo.uDataSize, strBuffer);
		}
		else if (2 == s_stUpgradeBufInfo.nCnt)
		{
			ComputeMultiDataMd5sum((unsigned char *)(s_stUpgradeBufInfo.Mem[0].Addr), s_stUpgradeBufInfo.Mem[0].Size, 
								   (unsigned char *)(s_stUpgradeBufInfo.Mem[1].Addr), s_stUpgradeBufInfo.uDataSize - s_stUpgradeBufInfo.Mem[0].Size, 
								    strBuffer);
		}
		#else
		ComputeFileMd5sum(UPDATEFILENAME, strBuffer);
		#endif
		//校验md5
		CLUP_DEBUG("\nUpgrade file md5 check:TargetMd5=%s RealMd5=%s\n",
			s_stUpgradeFileDownloadInfo.szMd5, strBuffer);
		if (0 != strncmp(s_stUpgradeFileDownloadInfo.szMd5, strBuffer, sizeof(s_stUpgradeFileDownloadInfo.szMd5) - 1))
		{
			CLUP_ERROR("\nUpgrade file md5 check faild!TargetMd5=%s RealMd5=%s\n",
				s_stUpgradeFileDownloadInfo.szMd5, strBuffer);
			return -1;
		}
	}
	
	return 0;
}

/*升级包下载线程*/
static void * ThreadForUpgradeFileDownload(void *pData)
{
    PRT_TID();

	if (NULL == pData)
	{
		//标志退出云升级模式
		MPU_PUB_SetCurrentUpgradeMode(NOT_UPGRADE_MODE);
		return (void *)-1;
	}

	int nRet = 0;
	UPGRADE_FILE_DOWNLOAD_THREAD_DATA *pThrdData = (UPGRADE_FILE_DOWNLOAD_THREAD_DATA *)pData;
	
	/*下载升级包*/
	pThrdData->nDownloadStatus = 1;
	s_stUpgradeBufInfo.uDataSize = 0;
	
	CLUP_DEBUG("\nenCloudUpgradeStageCur=%d nDownloadStatus=%d\n",
		enCloudUpgradeStageCur, s_stUpgradeFileDownloadThreadData.nDownloadStatus);
	
	nRet = DownloadUpgradeFile();
	if (0 != nRet)
	{
		pThrdData->nDownloadStatus = 3;
		
		//标志退出云升级模式
		MPU_PUB_SetCurrentUpgradeMode(NOT_UPGRADE_MODE);
		
		//为重新下载等后续操作预留足够的时间
		ResetMonitorTimer();
		
		return (void *)-1;
	}
	else
	{
		pThrdData->nDownloadStatus = 2;
	}
	
	CLUP_DEBUG("\nenCloudUpgradeStageCur=%d nDownloadStatus=%d\n",
		enCloudUpgradeStageCur, s_stUpgradeFileDownloadThreadData.nDownloadStatus);
	
	//下载完成后，重置倒计时
	//为升级预留足够的时间
	ResetMonitorTimer();
	
	return NULL;
}

/*解析升级列表文件，获取升级包地址信息*/
static int ParseUpgradeFileDownloadInfo(UPGRADE_FILE_DOWNLOAD_START *pCfg)
{
	if (NULL == pCfg)
	{
		return IDVR_CIU_STRUCT_UNVALID;
	}
	
	int nRet = 0;
	char strKey[32] = {0};
	char strBuffer[256] = {0};
	char strBuffer2[256] = {0};
	char *pTmp = NULL;
	mxml_node_t *xml = NULL;
    mxml_node_t *root_node = NULL;
    mxml_node_t *firmware_node = NULL;
    mxml_node_t *node = NULL;
    const char *value = NULL;
	const char *attr = NULL;
	int nBinDataLen = 0;
	UPGRADE_FIRMWARE_INFO_LOCAL stUpgradeFileDownloadInfo = {0};
	
	FILE *file_new = fopen(UPGRADE_LIST_FILE, "rb");
    if (NULL == file_new)
    {
        CLUP_ERROR("open file failed [%s]\n", UPGRADE_LIST_FILE);
		return IDVR_CIU_CLOUD_UPGRADE_UPGRADE_FILE_ADDR_INFO_FILE_OPEN_FAILD;
    }
	// #if 0
	// 导入一个xml文件，MXML定义了如下几种回调函数: 
	// MXML_INTEGER_CALLBACK - 所有的数据节点包含以空格分割的整数。
	// MXML_OPAQUE_CALLBACK - 所有的数据节点包含"不透明"字符串（CDATA）。
	// MXML_REAL_CALLBACK - 所有的数据节点包含以空格分割的浮点数。
	// MXML_TEXT_CALLBACK - 所有的数据节点包含以空格分割的文本字符串。
	// #endif
	xml = mxmlLoadFile(NULL, file_new, MXML_OPAQUE_CALLBACK);
	fclose(file_new);
	file_new = NULL;
	if (NULL == xml)
    {
    	return IDVR_CIU_CLOUD_UPGRADE_UPGRADE_FILE_ADDR_INFO_FILE_PARSE_FAILD;
	}
	
	#if 0
	MXML_DESCEND 含义是一直向下直到树的根部
	MXML_DESCEND_FIRST 含义是向下搜索到一个节点的第一个匹配子节点，但不再继续向下搜索
	MXML_NO_DESCEND 含义是不查看任何的子节点在XML元素层次中，仅查看同级的伙伴节点或者父节点直到到达顶级节点或者给出的树的顶级节点.
	#endif
    for (root_node = mxmlFindElement(xml, xml, "root", NULL, NULL, MXML_DESCEND);
		root_node != NULL;
		root_node = mxmlFindElement(root_node, xml, "root", NULL, NULL, MXML_DESCEND))
    {
        /* 升级文件信息*/
        for (firmware_node = mxmlFindElement(root_node, xml, "firmware", NULL, NULL, MXML_DESCEND);
			firmware_node != NULL;
			firmware_node = mxmlFindElement(firmware_node, xml, "firmware", NULL, NULL, MXML_DESCEND))
        {
        	memset(&stUpgradeFileDownloadInfo, 0, sizeof(stUpgradeFileDownloadInfo));
			#if 0
			如果选择了MXML_OPAQUE_CALLBACK作为回调函数，可以解析带空格的字符串，MXML默认解析字符串
			是以空格分隔的，在获取的时候，不能直接按照mxmlGetText获取，否则会被空格给隔断，会忽略
			空格后面的字符串，需要按照node->child->value.opaque的方式获取
			#endif
            node = mxmlFindElement(firmware_node, xml, "name", NULL, NULL, MXML_DESCEND);
            if (node && node->child)
            {
                value = node->child->value.opaque;
                if (0 == strlen(value))
                {
                	continue;
                }
				else
				{
					//根据指定文件名找到对应升级版本信息节点
					if (0 != strncmp(value, pCfg->szFirmwareName, sizeof(pCfg->szFirmwareName)))
					{
						continue;
					}
					else
					{
						CLUP_DEBUG("\nname value=%s\n", value);
                    	strncpy(stUpgradeFileDownloadInfo.szFirmwareName, value, sizeof(stUpgradeFileDownloadInfo.szFirmwareName) - 1);
					}
				}
            }
			else
			{
				continue;
			}
			
            node = mxmlFindElement(firmware_node, xml, "proto", NULL, NULL, MXML_DESCEND);
            if (node && node->child)
            {
                value = NULL;
                value = node->child->value.opaque;
                if (strlen(value))
                {
                	CLUP_DEBUG("\nproto value=%s\n", value);
                    strncpy(stUpgradeFileDownloadInfo.szProtocol, value, sizeof(stUpgradeFileDownloadInfo.szProtocol) - 1);
				}
            }
			
            node = mxmlFindElement(firmware_node, xml, "hostip", NULL, NULL, MXML_DESCEND);
            if (node && node->child)
            {
                value = node->child->value.opaque;
                if (strlen(value))
                {
                	CLUP_DEBUG("\nhostip value=%s\n", value);
                    strncpy(stUpgradeFileDownloadInfo.szHostIP, value, sizeof(stUpgradeFileDownloadInfo.szHostIP) - 1);
				}
            }
			
            node = mxmlFindElement(firmware_node, xml, "port", NULL, NULL, MXML_DESCEND);
            if (node && node->child)
            {
                value = node->child->value.opaque;
                if (strlen(value))
                {
                	CLUP_DEBUG("\nport value=%s\n", value);
                    stUpgradeFileDownloadInfo.nPort = atoi(value);
                }
            }
			
            node = mxmlFindElement(firmware_node, xml, "username", NULL, NULL, MXML_DESCEND);
            if (node && node->child)
            {
                value = node->child->value.opaque;
                if (strlen(value))
                {
                	CLUP_DEBUG("\nusername value=%s\n", value);
                    strncpy(stUpgradeFileDownloadInfo.szUserName, value, sizeof(stUpgradeFileDownloadInfo.szUserName) - 1);
                }
            }
			
            node = mxmlFindElement(firmware_node, xml, "password", NULL, NULL, MXML_DESCEND);
            if (node && node->child)
            {
                value = node->child->value.opaque;
                if (strlen(value))
                {
                	CLUP_DEBUG("\npassword value=%s\n", value);
                    strncpy(stUpgradeFileDownloadInfo.szPassword, value, sizeof(stUpgradeFileDownloadInfo.szPassword) - 1);
                }
            }
			
            node = mxmlFindElement(firmware_node, xml, "file", NULL, NULL, MXML_DESCEND);
            if (node)
            {
            	attr = mxmlElementGetAttr(node, "md5");
		        if (attr)
		        {
		            CLUP_DEBUG("\nmd5 attr=%s\n", attr);
					strncpy(stUpgradeFileDownloadInfo.szMd5, attr, sizeof(stUpgradeFileDownloadInfo.szMd5) - 1);
		        }
				
		        attr = mxmlElementGetAttr(node, "URI");
		        if (attr)
		        {
		            CLUP_DEBUG("\nURI attr=%s\n", attr);
					strncpy(stUpgradeFileDownloadInfo.szURL, attr, sizeof(stUpgradeFileDownloadInfo.szURL) - 1);
		        }

				attr = mxmlElementGetAttr(node, "filesize");
		        if (attr)
		        {
		            CLUP_DEBUG("\nfilesize attr=%s\n", attr);
					stUpgradeFileDownloadInfo.nFileSize = atoi(attr);
		        }
            }

			//能到这里，说明已经找到指定版本
			break;
		}
	}

	/*释放xml*/
	mxmlDelete(xml);
    xml = NULL;

	/*检查下载信息合法性*/
	if (0 == strlen(stUpgradeFileDownloadInfo.szFirmwareName))
	{
		return IDVR_CIU_CLOUD_UPGRADE_DOWNLOAD_VERSION_NOT_FOUND;
	}
	
	//if (0 == IsValidIpAddr(stUpgradeFileDownloadInfo.szHostIP))
	//{
	//	return IDVR_CIU_CLOUD_UPGRADE_UPGRADE_FILE_ADDR_HOSTIP_INVALID;
	//}
	//
	//if (0 == IsValidPort(stUpgradeFileDownloadInfo.nPort))
	//{
	//	return IDVR_CIU_CLOUD_UPGRADE_UPGRADE_FILE_ADDR_PORT_INVALID;
	//}
	
	if (0 == strlen(stUpgradeFileDownloadInfo.szURL))
	{
		return IDVR_CIU_CLOUD_UPGRADE_UPGRADE_FILE_ADDR_URL_INVALID;
	}
	
	if (stUpgradeFileDownloadInfo.nFileSize <= 0)
	{
		return IDVR_CIU_CLOUD_UPGRADE_UPGRADE_FILE_SIZE_EXCEPTIONAL;
	}
	//判断协议是否支持(目前HTTP/FTP)
	memset(strBuffer, 0, sizeof(strBuffer));
	StrCapitals2SmallLetter(strBuffer, stUpgradeFileDownloadInfo.szProtocol, 
		strlen(stUpgradeFileDownloadInfo.szProtocol));
	if (0 == IsSupportProtocol(strBuffer))
	{
		return IDVR_CIU_CLOUD_UPGRADE_UPGRADE_FILE_DOWNLOAD_PROTOCOL_UNSUPPORT;
	}
	memcpy(stUpgradeFileDownloadInfo.szProtocol, strBuffer, 
		sizeof(stUpgradeFileDownloadInfo.szProtocol));

	/*解密服务器账户*/
	#if 0
	///////////////////////////test//////////////////////////
	memset(strKey, 0, sizeof(strKey));
	memset(strBuffer, 0, sizeof(strBuffer));
	memset(strBuffer2, 0, sizeof(strBuffer2));
	snprintf(strBuffer2, sizeof(strBuffer2), "qweasdzxc:%s", stUpgradeFileDownloadInfo.szUserName);
	CLUP_DEBUG("\nstrBuffer2=%s\n", strBuffer2);
	nDataLen = ((strlen(strBuffer2)+1)+7)&0xfffffff8;
	memcpy(strKey, stUpgradeFileDownloadInfo.szFirmwareName, 8);
	StdDesEncryption((unsigned char*)strBuffer2, (unsigned char*)strBuffer, nDataLen, 
		(unsigned char*)strKey, ENCRYPTION_MODE);
	memset(strBuffer2, 0, sizeof(strBuffer2));
	base64Encode((const unsigned char*)strBuffer, strBuffer2, nDataLen);
	snprintf(strBuffer2, sizeof(strBuffer2), "Ws3fe1LSXtKYpfcfvCQEfe2YlIxUV/Pw");
    CLUP_DEBUG("\nstrBuffer2=%s\n", strBuffer2);
    memset(strBuffer, 0, sizeof(strBuffer));
    base64Decode(strBuffer2, (unsigned char*)strBuffer);
	StdDesEncryption((unsigned char*)strBuffer, (unsigned char*)strBuffer2, nDataLen, 
		(unsigned char*)strKey, DECRYPTION_MODE);
	CLUP_DEBUG("\nstrBuffer2=%s\n", strBuffer2); 
	pTmp = strstr(strBuffer2, CLOUD_UPGRADE_USRINFO_START_FLAG);
	if (NULL == pTmp)
	{
		memset(stUpgradeFileDownloadInfo.szUserName, 0, 
			sizeof(stUpgradeFileDownloadInfo.szUserName));
		memcpy(stUpgradeFileDownloadInfo.szUserName, strBuffer2, 
			sizeof(stUpgradeFileDownloadInfo.szUserName)-1);
	}
	else
	{
		memset(stUpgradeFileDownloadInfo.szUserName, 0, 
			sizeof(stUpgradeFileDownloadInfo.szUserName));
		memcpy(stUpgradeFileDownloadInfo.szUserName, pTmp+1, 
			sizeof(stUpgradeFileDownloadInfo.szUserName)-1);
	}

	memset(strKey, 0, sizeof(strKey));
	memset(strBuffer, 0, sizeof(strBuffer));
	memset(strBuffer2, 0, sizeof(strBuffer2));
	snprintf(strBuffer2, sizeof(strBuffer2), "qweasdzxc:%s", stUpgradeFileDownloadInfo.szPassword);
	CLUP_DEBUG("\nstrBuffer2=%s\n", strBuffer2);
	nDataLen = ((strlen(strBuffer2)+1)+7)&0xfffffff8;
	memcpy(strKey, stUpgradeFileDownloadInfo.szFirmwareName, 8);
	StdDesEncryption((unsigned char*)strBuffer2, (unsigned char*)strBuffer, nDataLen, 
		(unsigned char*)strKey, ENCRYPTION_MODE);
	memset(strBuffer2, 0, sizeof(strBuffer2));
	base64Encode((const unsigned char*)strBuffer, strBuffer2, nDataLen);
	snprintf(strBuffer2, sizeof(strBuffer2), "zXchgcgW580p68T5ReE/+Mj0dFlBn54OMgooQNIIr4s=");
    CLUP_DEBUG("\nstrBuffer2=%s\n", strBuffer2);
    memset(strBuffer, 0, sizeof(strBuffer));
    base64Decode(strBuffer2, (unsigned char*)strBuffer);
	StdDesEncryption((unsigned char*)strBuffer, (unsigned char*)strBuffer2, nDataLen, 
		(unsigned char*)strKey, DECRYPTION_MODE);
	CLUP_DEBUG("\nstrBuffer2=%s\n", strBuffer2);
	pTmp = strstr(strBuffer2, CLOUD_UPGRADE_USRINFO_START_FLAG);
	if (NULL == pTmp)
	{
		memset(stUpgradeFileDownloadInfo.szPassword, 0, 
			sizeof(stUpgradeFileDownloadInfo.szPassword));
		memcpy(stUpgradeFileDownloadInfo.szPassword, strBuffer2, 
			sizeof(stUpgradeFileDownloadInfo.szPassword)-1);
	}
	else
	{
		memset(stUpgradeFileDownloadInfo.szPassword, 0, 
			sizeof(stUpgradeFileDownloadInfo.szPassword));
		memcpy(stUpgradeFileDownloadInfo.szPassword, pTmp+1, 
			sizeof(stUpgradeFileDownloadInfo.szPassword)-1);
	}
	////////////////////////////////////////////////////////
	#else
	if (strlen(stUpgradeFileDownloadInfo.szUserName) > 0)
	{
		memset(strBuffer, 0, sizeof(strBuffer));
		nBinDataLen = base64Decode(stUpgradeFileDownloadInfo.szUserName, (unsigned char*)strBuffer);
		memset(strKey, 0, sizeof(strKey));
		memcpy(strKey, stUpgradeFileDownloadInfo.szFirmwareName, 8);
		memset(strBuffer2, 0, sizeof(strBuffer2));
		nRet = StdDesEncryption((unsigned char*)strBuffer, (unsigned char*)strBuffer2, 
			((nBinDataLen+7)&0xfffffff8), (unsigned char*)strKey, DECRYPTION_MODE);
		if (0 != nRet)
		{
			return IDVR_CIU_CLOUD_UPGRADE_UPGRADE_FILE_SERVER_USRNAME_DECRYPT_FAILD;
		}
		CLUP_DEBUG("\nstrTmpIn=%s len=%d strTmpOut=%s len=%d\n", 
			stUpgradeFileDownloadInfo.szUserName, strlen(stUpgradeFileDownloadInfo.szUserName),
			strBuffer2, strlen(strBuffer2));
		pTmp = strstr(strBuffer2, CLOUD_UPGRADE_USRINFO_START_FLAG);
		if (NULL == pTmp)
		{
			memset(stUpgradeFileDownloadInfo.szUserName, 
				0, sizeof(stUpgradeFileDownloadInfo.szUserName));
			memcpy(stUpgradeFileDownloadInfo.szUserName, strBuffer2, 
				sizeof(stUpgradeFileDownloadInfo.szUserName)-1);
		}
		else
		{
			memset(stUpgradeFileDownloadInfo.szUserName, 0, 
				sizeof(stUpgradeFileDownloadInfo.szUserName));
			memcpy(stUpgradeFileDownloadInfo.szUserName, pTmp+1, 
				sizeof(stUpgradeFileDownloadInfo.szUserName)-1);
		}
	}
	
	if (strlen(stUpgradeFileDownloadInfo.szPassword) > 0)
	{
		memset(strBuffer, 0, sizeof(strBuffer));
		nBinDataLen = base64Decode(stUpgradeFileDownloadInfo.szPassword, (unsigned char*)strBuffer);

		memset(strKey, 0, sizeof(strKey));
		memcpy(strKey, stUpgradeFileDownloadInfo.szFirmwareName, 8);
		memset(strBuffer2, 0, sizeof(strBuffer2));
		nRet = StdDesEncryption((unsigned char*)strBuffer, (unsigned char*)strBuffer2, 
			((nBinDataLen+7)&0xfffffff8), (unsigned char*)strKey, DECRYPTION_MODE);
		if (0 != nRet)
		{
			return IDVR_CIU_CLOUD_UPGRADE_UPGRADE_FILE_SERVER_PASSWD_DECRYPT_FAILD;
		}
		CLUP_DEBUG("\nstrTmpIn=%s len=%d strTmpOut=%s len=%d\n", 
			stUpgradeFileDownloadInfo.szPassword, strlen(stUpgradeFileDownloadInfo.szPassword),
			strBuffer2, strlen(strBuffer2));
		pTmp = strstr(strBuffer2, CLOUD_UPGRADE_USRINFO_START_FLAG);
		if (NULL == pTmp)
		{
			memset(stUpgradeFileDownloadInfo.szPassword, 
				0, sizeof(stUpgradeFileDownloadInfo.szPassword));
			memcpy(stUpgradeFileDownloadInfo.szPassword, strBuffer2, 
				sizeof(stUpgradeFileDownloadInfo.szPassword)-1);
		}
		else
		{
			memset(stUpgradeFileDownloadInfo.szPassword, 0, 
				sizeof(stUpgradeFileDownloadInfo.szPassword));
			memcpy(stUpgradeFileDownloadInfo.szPassword, pTmp+1, 
				sizeof(stUpgradeFileDownloadInfo.szPassword)-1);
		}
	}
	#endif
	
	memcpy(&s_stUpgradeFileDownloadInfo, &stUpgradeFileDownloadInfo, 
		sizeof(s_stUpgradeFileDownloadInfo));

	return 0;
}

/*初始化升级环境*/
static int InitUpgradeContext()
{
	int nRet = 0;
	char strBuffer[128] = {0};
	char command[64] = {0};
	FILE *file_new = NULL;
	int counter = 0;
	
	if (0 == access(UPDATEFILENAME, F_OK))
	{
		memset(command, 0,sizeof(command));
		snprintf(command, sizeof(command), "rm %s", UPDATEFILENAME);
		mysystem(command);
	}
	#ifdef UPLOAD_TO_DSP_MEM
	if (0 == access(DSP_ADDR_INFO, F_OK))
	{
		memset(command, 0,sizeof(command));
		snprintf(command, sizeof(command), "rm %s", DSP_ADDR_INFO);
		mysystem(command);
	}
	if (0 == access(DSP_ADDR_INFO_FOR_MMZ, F_OK))
	{
		memset(command, 0,sizeof(command));
		snprintf(command, sizeof(command), "rm %s", DSP_ADDR_INFO_FOR_MMZ);
		mysystem(command);
	}
	#endif
	snprintf(command, sizeof(command), "rm %s %s", UPDATEFILENAME_SUCC, UPDATEFILENAME_SUCC_CLOUD_UPGRADE);
	mysystem(command);

	CLUP_DEBUG("\n");
	/*采用MMZ内存升级*/
	//将缓存写入磁盘
	mysystem("sync");
	//释放缓存
	mysystem("echo 3 > /proc/sys/vm/drop_caches");

	#ifndef UPLOAD_TO_DSP_MEM	//如果没有使用mmz内存升级，则需要
	char szManufacturer[256] = {0};
	if (!access(CUSTOM_INFO, 0))
	{
		//获取新的配置文件读取实例
		CConfig *pCfg = CConfig::NewInstance();
		//从版本信息中获取
		nRet = pCfg->LoadFile(CUSTOM_INFO);
		if (0 == nRet)
		{
			pCfg->GetValue("CustomInfo", "Manufacturer", "Zview", szManufacturer, sizeof(szManufacturer));
		}
		CConfig::ReleaseInstance(pCfg);
		pCfg = NULL;
	}
	CLUP_DEBUG("\n");
	if (0 != strcmp(szManufacturer, CUSTOM_SHILIAN))
	{
		//3518EV200需要节省内存，所以卸载部分分区	
		if (IsStringInCommand("mount", "web") != 0)
		{
			//fuser这个命令一定是要在挂载点存在的情况下使用，否则会立即重启
			nRet = umountPartition("/web");
			if (0 != nRet)
			{
				CLUP_INFO("/web part umount fail! exit!\n");
			}
		}
	}
	CLUP_DEBUG("\n");
	if (IsStringInCommand("mount", "custom") != 0)
	{
		nRet = umountPartition(IDVR_CUSTOM_FILE_DIR);
		if (0 != nRet)
		{
			CLUP_INFO("/mnt/app/custom part umount fail! exit!\n");
		}
	}
	CLUP_DEBUG("\n");
	#endif

    ZWSys_SetSpeechContent(SPEECH_UPGRADE_START);
    //循环查询，等待播放结束
    while (1)
    {
        sleep(1);
        if (0 == ZWSys_GetSpeechPlayingStatus())
        {
            break;
        }
        if (++counter > SPEECH_PLAY_TIME_OUT_SEC)
        {
            counter = 0;
            break;
        }
    }

	#ifdef UPLOAD_TO_DSP_MEM
	//进入升级模式，并获得MMZ内存块
	unsigned int u32PhyAddr = 0;
	void *pVirtAddr = NULL;
	unsigned int uMmzAllocSize = 0;
	int nShmid = -1;
	unsigned int uShareMemSize = 0;
	unsigned int uShareMemAddr = 0;
	nRet = ZWSys_SwitchToUpgradeMode(1, &u32PhyAddr, &pVirtAddr, &uMmzAllocSize);
	CLUP_DEBUG("\n");
	if (nRet)
	{
		CLUP_ERROR("ZWSys_SwitchToUpgradeMode fail %d!.\n", nRet);
		return IDVR_CIU_CLOUD_UPGRADE_DOWNLOAD_BUFFER_PREPARE_FAILD;
	}
	else
	{
		CLUP_DEBUG("\n");
		usleep(100000);
		if (0 == u32PhyAddr
			|| NULL == pVirtAddr
			|| uMmzAllocSize <= 0)
		{
			CLUP_ERROR("MMZ_mem addr=%#x phyAddr=%p len=%#x\n", 
				u32PhyAddr, pVirtAddr, uMmzAllocSize);
			return IDVR_CIU_CLOUD_UPGRADE_DOWNLOAD_BUFFER_PREPARE_FAILD;
		}

		CLUP_DEBUG("\n");

		if ((unsigned int)s_stUpgradeFileDownloadInfo.nFileSize > uMmzAllocSize)
		{
			mysystem("echo 3 > /proc/sys/vm/drop_caches");
			nShmid = CreateShm(s_stUpgradeFileDownloadInfo.nFileSize - uMmzAllocSize, IPC_CREAT|IPC_EXCL|0666);
			if (nShmid != -1)
			{
				if (GetShm(nShmid, NULL, IPC_CREAT|IPC_EXCL|0666) != NULL)
				{
					uShareMemAddr = (unsigned int)GetShm(nShmid, NULL, IPC_CREAT|IPC_EXCL|0666);
					uShareMemSize = s_stUpgradeFileDownloadInfo.nFileSize - uMmzAllocSize;
					CLUP_INFO("share memory addr=%#x len=%#x\n", uShareMemAddr, uShareMemSize);
				}
				else
				{
					CLUP_INFO("share memory malloc addr fail!\n");
					return IDVR_CIU_CLOUD_UPGRADE_DOWNLOAD_BUFFER_PREPARE_FAILD;
				}
			}
			else
			{
				CLUP_INFO("share memory malloc fail!\n");
				return IDVR_CIU_CLOUD_UPGRADE_DOWNLOAD_BUFFER_PREPARE_FAILD;
			}
		}
		else
		{
			uShareMemSize = 0;
			uShareMemAddr = 0;
		}
		
		if (uMmzAllocSize + uShareMemSize < (unsigned int)s_stUpgradeFileDownloadInfo.nFileSize)
		{
			CLUP_ERROR("upgrade buf mmzlen=%u, shmlen=%u, upgrade file size=%d, buffer is too small!\n", 
				uMmzAllocSize, uShareMemAddr, s_stUpgradeFileDownloadInfo.nFileSize);
			return IDVR_CIU_CLOUD_UPGRADE_UPGRADE_FILE_TOO_LARGE;
		}
		else
		{
			CLUP_DEBUG("\n");
			file_new = fopen(DSP_ADDR_INFO_FOR_MMZ, "w");
			if (NULL == file_new)
			{
				CLUP_ERROR("open file failed [%s]\n", DSP_ADDR_INFO_FOR_MMZ);
				return IDVR_CIU_CLOUD_UPGRADE_DOWNLOAD_BUFFER_PREPARE_FAILD;
			}
			
			memset(strBuffer, 0, sizeof(strBuffer));
			snprintf(strBuffer, sizeof(strBuffer), "%u:%u:%u:%u:%u", (unsigned int)pVirtAddr, u32PhyAddr, uMmzAllocSize, uShareMemAddr, uShareMemSize);
			fprintf(file_new, "%s", strBuffer);
			fclose(file_new);
			file_new = NULL;
		}

		CLUP_DEBUG("\n");
		if (uShareMemSize > 0)
		{
			s_stUpgradeBufInfo.nCnt = 2;
			s_stUpgradeBufInfo.Mem[0].Addr = (void *)pVirtAddr;
			s_stUpgradeBufInfo.Mem[0].Size = uMmzAllocSize;
			s_stUpgradeBufInfo.Mem[1].Addr = (void *)uShareMemAddr;
			s_stUpgradeBufInfo.Mem[1].Size = uShareMemSize;
		}
		else
		{
			s_stUpgradeBufInfo.nCnt = 1;
			s_stUpgradeBufInfo.Mem[0].Addr = (void *)pVirtAddr;
			s_stUpgradeBufInfo.Mem[0].Size = uMmzAllocSize;
		}
		s_stUpgradeBufInfo.uMaxSize = uMmzAllocSize + uShareMemSize;
		CLUP_INFO("MMZ_mem addr=%p phyAddr=%#x len=%#x, shmAddr=%#x, shmLen=%#x, totolLen=%#x\n", 
			pVirtAddr, u32PhyAddr, uMmzAllocSize, uShareMemAddr, uShareMemSize, s_stUpgradeBufInfo.uMaxSize);
	}
    #else
	memIsEnoughForUpdate(s_stUpgradeFileDownloadInfo.nFileSize);
    #endif
	
	#ifdef __GOKE__
	//if (0 != memIsEnoughForUpdate(pFileUploadParaInfo->dwFileTotalLen))
	{
		#ifdef UPLOAD_TO_DSP_MEM
		CLUP_INFO("Mem is not enough to upgrade, upload to DSP mem!\n");
		
		int dspAddr = 0;
		int dspLen = 0;
		
		nRet = ZWSys_EnableDspMemToUpgrade(ZWSys_EncChannelOpen(0), (int *)(&dspAddr), (int *)(&dspLen), 1);
		if (nRet)
		{
			CLUP_ERROR("ZWSys_EnableDspMemToUpgrade fail %d!.\n", nRet);
			return IDVR_CIU_CLOUD_UPGRADE_DOWNLOAD_BUFFER_PREPARE_FAILD;
		}
		usleep(100000);
		if (0 == dspAddr
			|| dspLen <= 0)
		{
			CLUP_ERROR("MMZ_mem dspAddr=%#x dspLen=%#x\n", dspAddr, dspLen);
			return IDVR_CIU_CLOUD_UPGRADE_DOWNLOAD_BUFFER_PREPARE_FAILD;
		}

		if (dspLen < s_stUpgradeFileDownloadInfo.nFileSize)
		{
			CLUP_ERROR("upgrade buf len=%d, upgrade file size=%d, buffer is too small!\n", 
				dspLen, s_stUpgradeFileDownloadInfo.nFileSize);
			return IDVR_CIU_CLOUD_UPGRADE_UPGRADE_FILE_TOO_LARGE;
		}
		else
		{
			file_new = fopen(DSP_ADDR_INFO, "w");
			if (file_new == NULL)
			{
				CLUP_ERROR("open file failed [%s]\n", DSP_ADDR_INFO);
				return IDVR_CIU_CLOUD_UPGRADE_DOWNLOAD_BUFFER_PREPARE_FAILD;
			}
			memset(strBuffer, 0, sizeof (strBuffer));
			CLUP_INFO("DSP addr=%d, len=%d\n", dspAddr, dspLen);
			if (dspLen > 16 * 1024 * 1024)
			{
				dspAddr = dspAddr + dspLen - 16 * 1024 * 1024;
				dspLen = 16 * 1024 * 1024;
			}
			snprintf(strBuffer, sizeof(strBuffer), "%d:%d", dspAddr, dspLen);
			fprintf(file_new, "%s", strBuffer);
			fclose(file_new);
			CLUP_INFO("DSP addr=%d, len=%d\n", dspAddr, dspLen);
		}
		
		s_stUpgradeBufInfo.u32PhyAddr = dspAddr;
		s_stUpgradeBufInfo.pVirtAddr = (void *)dspAddr;
		s_stUpgradeBufInfo.uMaxSize = dspLen;
		CLUP_INFO("MMZ_mem addr=%#x phyAddr=%#x len=%#x\n", 
			s_stUpgradeBufInfo.u32PhyAddr, s_stUpgradeBufInfo.pVirtAddr, s_stUpgradeBufInfo.uMaxSize);
		#else
		CLUP_ERROR("update failed cause of insufficient in RAM !!\n");
		return IDVR_CIU_CLOUD_UPGRADE_DOWNLOAD_BUFFER_PREPARE_FAILD;
		#endif
	}
	#endif	
	
	return 0;
}

//!\brief      指定并启动升级包下载(非阻塞式)
int MPU_CloudUpgrade_UpgradeFileDownloadStart(UPGRADE_FILE_DOWNLOAD_START *pCfg)
{
	pthread_mutex_lock(&s_mutexInterface);

	int nRet = 0;
	pthread_attr_t thrdAttr = {0};
	UPGRADE_MODE enUpgradeMode = NOT_UPGRADE_MODE;
	
	if (NULL == pCfg)
	{
		nRet = IDVR_CIU_STRUCT_UNVALID;
		goto RET;
	}

	//获取当前升级模式
	MPU_PUB_GetCurrentUpgradeMode(&enUpgradeMode);
	//若当前不是空闲模式或云升级模式，返回错误
	if (NOT_UPGRADE_MODE != enUpgradeMode
		&& CLOUD_UPGRADE_MODE != enUpgradeMode)
	{
		s_stUpgradeFileDownloadThreadData.nDownloadStatus = 3;
		nRet = IDVR_CIU_CLOUD_UPGRADE_UPGRADE_TYPE_CONFLICT;
		goto RET;
	}
	//标志进入云升级模式
	MPU_PUB_SetCurrentUpgradeMode(CLOUD_UPGRADE_MODE);

	//从这一步开始，拒绝其他用户升级操作
	//并禁用其他升级方式
	if (0 == strlen(s_szUpgradePassport))
	{
		//记录升级凭证
		memset(s_szUpgradePassport, 0, sizeof(s_szUpgradePassport));
		memcpy(s_szUpgradePassport, pCfg->szUpgradePassport, sizeof(s_szUpgradePassport)-1);
	}
	else
	{
		if (0 != strncmp(pCfg->szUpgradePassport, s_szUpgradePassport, sizeof(pCfg->szUpgradePassport)))
		{
			nRet = IDVR_CIU_CLOUD_UPGRADE_USER_UPGRADE_CONFLICT;
			//还原老的升级模式
			MPU_PUB_SetCurrentUpgradeMode(enUpgradeMode);
			goto RET;
		}
	}

	//检测是否已经下载过升级列表
	if (enCloudUpgradeStageCur < CLOUD_UPGRADE_STATGE_GET_UPGRADE_LIST)
	{
		s_stUpgradeFileDownloadThreadData.nDownloadStatus = 3;
		nRet = IDVR_CIU_CLOUD_UPGRADE_UPGRADE_LIST_NOT_FOUND;
		//标志退出云升级模式
		MPU_PUB_SetCurrentUpgradeMode(NOT_UPGRADE_MODE);
		goto RET;
	}

	//检测是否正在下载升级文件
	CLUP_DEBUG("\nenCloudUpgradeStageCur=%d nDownloadStatus=%d\n",
		enCloudUpgradeStageCur, s_stUpgradeFileDownloadThreadData.nDownloadStatus);
	if (CLOUD_UPGRADE_STATGE_DOWNLOAD_UPGRADE_FILE == enCloudUpgradeStageCur
		&& 1 == s_stUpgradeFileDownloadThreadData.nDownloadStatus)
	{
		nRet = IDVR_CIU_CLOUD_UPGRADE_DOWLOAD_UPGRADE_FILE_ING;
		//还原老的升级模式
		MPU_PUB_SetCurrentUpgradeMode(enUpgradeMode);
		goto RET;
	}

	//检测当前是否正在升级
	if (CLOUD_UPGRADE_STATGE_UPGRADE == enCloudUpgradeStageCur)
	{
		nRet = IDVR_CIU_CLOUD_UPGRADE_UPGRADE_ING;
		//还原老的升级模式
		MPU_PUB_SetCurrentUpgradeMode(enUpgradeMode);
		goto RET;
	}
	
	if(ManualUpgrade == g_nUpgradeType) // 手动升级
	{
		/*解析升级列表文件，获取升级包地址信息*/
		nRet = ParseUpgradeFileDownloadInfo(pCfg);
		if (nRet != 0)
		{
			s_stUpgradeFileDownloadThreadData.nDownloadStatus = 3;
			//标志退出云升级模式
			MPU_PUB_SetCurrentUpgradeMode(NOT_UPGRADE_MODE);
			goto RET;
		}
	}
	
	//启动重启监控
	RebootMonitorForCloudUpgrade();

#ifdef __SUPPORT_SD_CARD__
	MPU_AVI_StopRecordAndUinitMemRecord(0);
#endif

	nRet = InitUpgradeContext();
	if (nRet != 0)
	{
		s_stUpgradeFileDownloadThreadData.nDownloadStatus = 3;
		//标志退出云升级模式
		MPU_PUB_SetCurrentUpgradeMode(NOT_UPGRADE_MODE);
		goto RET;
	}
	
	memset(&s_stUpgradeFileDownloadThreadData, 0, sizeof(s_stUpgradeFileDownloadThreadData));
	s_stUpgradeFileDownloadThreadData.nDownloadStatus = 1;
	CLUP_DEBUG("\nenCloudUpgradeStageCur=%d nDownloadStatus=%d\n",
		enCloudUpgradeStageCur, s_stUpgradeFileDownloadThreadData.nDownloadStatus);

	if(ManualUpgrade == g_nUpgradeType) // 手动升级
	{
		nRet = pthread_attr_init(&thrdAttr);
		if (0 != nRet)
		{
			CLUP_ERROR("pthread_attr_init() return %d\n", nRet);
		}
		nRet = pthread_attr_setdetachstate(&thrdAttr, PTHREAD_CREATE_DETACHED);
		if (0 != nRet)
		{
			CLUP_ERROR("pthread_attr_setdetachstate( PTHREAD_CREATE_DETACHED ) return %d\n", nRet);
		}
		nRet = pthread_create(&s_stUpgradeFileDownloadThreadData.thrdId, &thrdAttr, 
			ThreadForUpgradeFileDownload, (void *)(&s_stUpgradeFileDownloadThreadData));
		if (0 != nRet)
		{
			s_stUpgradeFileDownloadThreadData.nDownloadStatus = 3;
			CLUP_ERROR("pthread_create( ThreadForUpgradeFileDownload ) DETACHED return %d\n", nRet);
			pthread_attr_destroy(&thrdAttr);
			nRet = IDVR_CIU_CLOUD_UPGRADE_DOWNLOAD_THREAD_CREATE_FAILD;
			//标志退出云升级模式
			MPU_PUB_SetCurrentUpgradeMode(NOT_UPGRADE_MODE);
			goto RET;
		}
		pthread_attr_destroy(&thrdAttr);
	}
#if defined (__IS_SUPPORT_P6SMESSAGE__)
	else if(AutoUpgrade == g_nUpgradeType) // 自动升级  在下载过程中P6SMessage不需要进行其它操作
	{
		nRet = pthread_create(&s_stUpgradeFileDownloadThreadData.thrdId, NULL, 
		ThreadForUpgradeFileDownload, (void *)(&s_stUpgradeFileDownloadThreadData));
		if (0 != nRet)
		{
			s_stUpgradeFileDownloadThreadData.nDownloadStatus = 3;
			CLUP_ERROR("pthread_create( ThreadForUpgradeFileDownload ) DETACHED return %d\n", nRet);
			nRet = IDVR_CIU_CLOUD_UPGRADE_DOWNLOAD_THREAD_CREATE_FAILD;
			//标志退出云升级模式
			MPU_PUB_SetCurrentUpgradeMode(NOT_UPGRADE_MODE);
			goto RET;
		}
		pthread_join(s_stUpgradeFileDownloadThreadData.thrdId, NULL);
	}
#endif

	//有任何操作，重置重启监控计时器
	ResetMonitorTimer();
	
RET:
	if (0 == nRet)
	{
		if (enCloudUpgradeStageCur < CLOUD_UPGRADE_STATGE_DOWNLOAD_UPGRADE_FILE)
		{
			enCloudUpgradeStageCur = CLOUD_UPGRADE_STATGE_DOWNLOAD_UPGRADE_FILE;
		}
	}
	pthread_mutex_unlock(&s_mutexInterface);
	return nRet;
}

#if defined (__IS_SUPPORT_P6SMESSAGE__)
//!\brief      指定并启动升级包下载(非阻塞式)
int MPU_CloudUpgrade_UpgradeFileDownloadStart_V1(UPGRADE_FILE_DOWNLOAD_START_V1 *pCfg)
{
	int nRet = 0;
	
	UPGRADE_FILE_DOWNLOAD_START FileDownload = {0};
	UPGRADE_CONTROL_START StartUpgrade = {0};
	UPGRADE_RESULT UpgradeResult = {0};

	if (NULL == pCfg)
	{
		nRet = IDVR_CIU_STRUCT_UNVALID;
		return nRet;
	}

	if(AutoUpgrade == pCfg->iAutoUpgrade)
	{
		g_nUpgradeType = AutoUpgrade;

		//用于升级包下载成功后的校验
		s_stUpgradeFileDownloadInfo.nFileSize = pCfg->iUpgradeFileSize;
		memcpy(s_stUpgradeFileDownloadInfo.szMd5, pCfg->acMD5, sizeof(s_stUpgradeFileDownloadInfo.szMd5));
		memcpy(s_stUpgradeFileDownloadInfo.szFirmwareName, pCfg->szFirmwareName, sizeof(s_stUpgradeFileDownloadInfo.szFirmwareName));
		
		//协议已给出路径以及用户名等信息，直接进入下载升级包阶段即可
		enCloudUpgradeStageCur = CLOUD_UPGRADE_STATGE_GET_UPGRADE_LIST;

		//指定并下载升级包(阻塞式) 升级包下载成功后才执行下列操作
		memcpy(FileDownload.szFirmwareName, pCfg->szFirmwareName, sizeof(FileDownload.szFirmwareName));
		memcpy(FileDownload.szUpgradePassport, pCfg->szUpgradePassport, sizeof(FileDownload.szUpgradePassport));
		MPU_CloudUpgrade_UpgradeFileDownloadStart(&FileDownload);
		
		//开始升级(非阻塞式)
		memcpy(StartUpgrade.szUpgradePassport, pCfg->szUpgradePassport, sizeof(StartUpgrade.szUpgradePassport));
		MPU_CloudUpgrade_UpgradeStart(&StartUpgrade);
		
		//获取升级结果
		memcpy(UpgradeResult.szFirmwareName, pCfg->szFirmwareName, sizeof(UpgradeResult.szFirmwareName));
		memcpy(UpgradeResult.szUpgradePassport, pCfg->szUpgradePassport, sizeof(UpgradeResult.szUpgradePassport));
		MPU_CloudUpgrade_GetUpgradeResult(&UpgradeResult);
		
	}
	else
	{
		g_nUpgradeType = ManualUpgrade;
		return -1;
	}
	
	return nRet;
}
#endif

//!\brief      获取升级包下载状态
int MPU_CloudUpgrade_GetUpgradeFileDownloadStatus(UPGRADE_FILE_DOWNLOAD_STATUS *pCfg)
{
	pthread_mutex_lock(&s_mutexInterface);

	int nRet = 0;
	
	if (NULL == pCfg)
	{
		nRet = IDVR_CIU_STRUCT_UNVALID;
		goto RET;
	}

	CLUP_DEBUG("\nszUpgradePassport=%s s_szUpgradePassport=%s\n",
		pCfg->szUpgradePassport, s_szUpgradePassport);
	
	if (0 != strncmp(pCfg->szUpgradePassport, s_szUpgradePassport, sizeof(pCfg->szUpgradePassport)))
	{
		nRet = IDVR_CIU_CLOUD_UPGRADE_PASSPORT_CHECK_FAILD;
		goto RET;
	}

	pCfg->nStatus = s_stUpgradeFileDownloadThreadData.nDownloadStatus;
	pCfg->nTotalSize = s_stUpgradeFileDownloadInfo.nFileSize;
	pCfg->nAlreadyTransSize = s_stUpgradeBufInfo.uDataSize;

	//有任何操作，重置重启监控计时器
	ResetMonitorTimer();

RET:
	pthread_mutex_unlock(&s_mutexInterface);
	return nRet;
}

//!\brief      取消下载(阻塞式)
int MPU_CloudUpgrade_UpgradeFileDownloadStop(UPGRADE_FILE_DOWNLOAD_STOP *pCfg)
{
	pthread_mutex_lock(&s_mutexInterface);

	int nRet = 0;
	UPGRADE_MODE enUpgradeMode = NOT_UPGRADE_MODE;
	int nBoardID = 0;
	
	if (NULL == pCfg)
	{
		nRet = IDVR_CIU_STRUCT_UNVALID;
		goto RET;
	}

	//获取当前升级模式
	MPU_PUB_GetCurrentUpgradeMode(&enUpgradeMode);
	//若当前不是云升级模式，返回错误
	if (CLOUD_UPGRADE_MODE != enUpgradeMode)
	{
		s_stUpgradeFileDownloadThreadData.nDownloadStatus = 3;
		nRet = IDVR_CIU_CLOUD_UPGRADE_UPGRADE_TYPE_CONFLICT;
		goto RET;
	}

	if (0 != strncmp(pCfg->szUpgradePassport, s_szUpgradePassport, sizeof(pCfg->szUpgradePassport)))
	{
		nRet = IDVR_CIU_CLOUD_UPGRADE_PASSPORT_CHECK_FAILD;
		goto RET;
	}


	MPU_SYS_Buzzer(BUZZER500);
	ZWSys_GetBoardInfo(&nBoardID);
	//海思平台下载升级包前卸载掉了分区
	if ((0x2022 == nBoardID) 
	    || (0x2048 == nBoardID) 
	    || (0x2052 == nBoardID)
	    || (0x2053 == nBoardID)
	    || (0x2057 == nBoardID)
	    || (0x2058 == nBoardID)
	    || (0x205d == nBoardID)
	    || (0x2061 == nBoardID)
	    || (0x2062 == nBoardID))
	{
		//延迟重启，确保状态返回至客户端
		CLUP_INFO("System will reboot!\n");
		RebootForCloudUpgrade();
	}

	//标志退出云升级模式
	MPU_PUB_SetCurrentUpgradeMode(NOT_UPGRADE_MODE);
	
RET:
	pthread_mutex_unlock(&s_mutexInterface);
	
	return nRet;
}

//!\brief      启动升级(非阻塞式)
int MPU_CloudUpgrade_UpgradeStart(UPGRADE_CONTROL_START *pCfg)
{
	pthread_mutex_lock(&s_mutexInterface);

	int nRet = 0;
	char szCmd[128] = {0};
	int nBoardID = 0;
	FILE *file_new = NULL;
	UPGRADE_MODE enUpgradeMode = NOT_UPGRADE_MODE;
	
	if (NULL == pCfg)
	{
		nRet = IDVR_CIU_STRUCT_UNVALID;
		goto RET;
	}

	//获取当前升级模式
	MPU_PUB_GetCurrentUpgradeMode(&enUpgradeMode);
	//若当前不是空闲模式或云升级模式，返回错误
	if (NOT_UPGRADE_MODE != enUpgradeMode
		&& CLOUD_UPGRADE_MODE != enUpgradeMode)
	{
		nRet = IDVR_CIU_CLOUD_UPGRADE_UPGRADE_TYPE_CONFLICT;
		goto RET;
	}
	//标志进入云升级模式
	MPU_PUB_SetCurrentUpgradeMode(CLOUD_UPGRADE_MODE);

	CLUP_DEBUG("\nszUpgradePassport=%s s_szUpgradePassport=%s\n",
		pCfg->szUpgradePassport, s_szUpgradePassport);
	
	if (0 != strncmp(pCfg->szUpgradePassport, s_szUpgradePassport, sizeof(pCfg->szUpgradePassport)))
	{
		nRet = IDVR_CIU_CLOUD_UPGRADE_PASSPORT_CHECK_FAILD;
		//还原老的升级模式
		MPU_PUB_SetCurrentUpgradeMode(enUpgradeMode);
		goto RET;
	}
	
	//检测升级文件是否已经下载及完成
	if (enCloudUpgradeStageCur < CLOUD_UPGRADE_STATGE_DOWNLOAD_UPGRADE_FILE)
	{
		nRet = IDVR_CIU_CLOUD_UPGRADE_UPGRADE_FILE_NOT_FOUND;
		//标志退出云升级模式
		MPU_PUB_SetCurrentUpgradeMode(NOT_UPGRADE_MODE);
		goto RET;
	}
	else// if (CLOUD_UPGRADE_STATGE_DOWNLOAD_UPGRADE_FILE == enCloudUpgradeStageCur)
	{
		if (1 == s_stUpgradeFileDownloadThreadData.nDownloadStatus)
		{
			nRet = IDVR_CIU_CLOUD_UPGRADE_UPGRADE_FILE_DOWNLOAD_UNFINISHED;
			//还原老的升级模式
			MPU_PUB_SetCurrentUpgradeMode(enUpgradeMode);
			goto RET;
		}
		else if (3 == s_stUpgradeFileDownloadThreadData.nDownloadStatus)
		{
			nRet = IDVR_CIU_CLOUD_UPGRADE_UPGRADE_FILE_NOT_FOUND;
			//标志退出云升级模式
			MPU_PUB_SetCurrentUpgradeMode(NOT_UPGRADE_MODE);
			goto RET;
		}
	}

	//检测当前是否正在升级
	if (CLOUD_UPGRADE_STATGE_UPGRADE == enCloudUpgradeStageCur)
	{
		nRet = IDVR_CIU_CLOUD_UPGRADE_UPGRADE_ING;
		goto RET;
	}

	//确保删除上一次保存的升级凭证
	snprintf(szCmd, sizeof(szCmd), "rm %s %s", UPGRADE_PASSPORT_FILE, UPGRADE_PASSPORT_TMP_FILE);
	mysystem(szCmd);
	
	//保存升级凭证至临时文件
	//升级成功后将会拷贝其至
	//可写分区，供升级重启后
	//验证升级结果使用
	file_new = fopen(UPGRADE_PASSPORT_TMP_FILE, "w");
    if (NULL == file_new)
    {
        CLUP_ERROR("open file failed [%s]\n", UPGRADE_PASSPORT_TMP_FILE);
		RebootForCloudUpgrade();
		nRet = IDVR_CIU_CLOUD_UPGRADE_UPGRADE_START_PASSPORT_SAVE_FAILD;
		//标志退出云升级模式
		MPU_PUB_SetCurrentUpgradeMode(NOT_UPGRADE_MODE);
		goto RET;
    }
	fprintf(file_new, "%s", s_szUpgradePassport);
	fclose(file_new);
	
	if (2 == s_stUpgradeFileDownloadThreadData.nDownloadStatus
		&& (unsigned int)s_stUpgradeFileDownloadInfo.nFileSize == s_stUpgradeBufInfo.uDataSize)	//下载已完成
	{  
		nRet = MPU_SYS_Update_CheckVersion(UPDATEFILENAME);//UPDATEFILENAME
		if (1 == nRet)	// 合法
		{
            // 测试模式-升级后要恢复出厂设置
            if (0 == access(TEST_WORK_MODE_FLAG, F_OK))
            {
                USR_ResumeAdminPassword();
                DeviceCfgReset(__FUNCTION__);
            }
			//延迟启动升级，确保状态返回至客户端
			snprintf(szCmd, sizeof(szCmd), "touch %s", UPDATEFILENAME_SUCC_CLOUD_UPGRADE);
			mysystem(szCmd);
			MPU_SYS_Buzzer(BUZZER500);
			CLUP_INFO(">>>>>SendNotifyMessage<<<<<< start reboot\n");

			nRet = 0;
			goto RET;
		}
		else			// 版本不对
		{
			MPU_SYS_Buzzer(BUZZER500);
			ZWSys_GetBoardInfo(&nBoardID);
			//海思平台下载升级包前卸载掉了分区
			if ((0x2022 == nBoardID) 
			    || (0x2048 == nBoardID) 
			    || (0x2052 == nBoardID)
			    || (0x2053 == nBoardID)
			    || (0x2057 == nBoardID)
			    || (0x2058 == nBoardID)
			    || (0x205d == nBoardID)
			    || (0x2061 == nBoardID)
			    || (0x2062 == nBoardID))
			{
				//延迟重启，确保状态返回至客户端
				CLUP_INFO("System will reboot!\n");
				RebootForCloudUpgrade();
			}
			
			nRet = IDVR_CIU_CLOUD_UPGRADE_UPGRADE_FILE_CHECK_FAILD;
			//标志退出云升级模式
			MPU_PUB_SetCurrentUpgradeMode(NOT_UPGRADE_MODE);
			goto RET;
		}
	}
	else
	{
		MPU_SYS_Buzzer(BUZZER500);
		ZWSys_GetBoardInfo(&nBoardID);
		//海思平台下载升级包前卸载掉了分区
		if ((0x2022 == nBoardID) 
		    || (0x2048 == nBoardID) 
		    || (0x2052 == nBoardID)
		    || (0x2053 == nBoardID)
		    || (0x2057 == nBoardID)
		    || (0x2058 == nBoardID)
		    || (0x205d == nBoardID)
		    || (0x2061 == nBoardID)
		    || (0x2062 == nBoardID))
		{
			//延迟重启，确保状态返回至客户端
			CLUP_INFO("System will reboot!\n");
			RebootForCloudUpgrade();
		}

		//标志退出云升级模式
		MPU_PUB_SetCurrentUpgradeMode(NOT_UPGRADE_MODE);

		if (2 == s_stUpgradeFileDownloadThreadData.nDownloadStatus)
		{
			nRet = IDVR_CIU_CLOUD_UPGRADE_UPGRADE_FILE_CHECK_FAILD;
			goto RET;
		}
		else
		{
			nRet = IDVR_CIU_CLOUD_UPGRADE_UPGRADE_FILE_DOWNLOAD_UNFINISHED;
			goto RET;
		}
	}

	//有任何操作，重置重启监控计时器
	ResetMonitorTimer();

RET:
	if (0 == nRet)
	{
		if (enCloudUpgradeStageCur < CLOUD_UPGRADE_STATGE_UPGRADE)
		{
			enCloudUpgradeStageCur = CLOUD_UPGRADE_STATGE_UPGRADE;
		}
	}
	pthread_mutex_unlock(&s_mutexInterface);
	return nRet;
}

//!\brief      获取升级结果
int MPU_CloudUpgrade_GetUpgradeResult(UPGRADE_RESULT *pCfg)
{
	pthread_mutex_lock(&s_mutexInterface);

	int nRet = 0;
	char strTmp[128] = {0};
	FILE *pFile = NULL;
	DEVICEVERSION stDevInfo = {0};
	
	if (NULL == pCfg)
	{
		nRet = IDVR_CIU_STRUCT_UNVALID;
		goto RET;
	}

	#if 1
	//从文件中读取升级凭证
    pFile = fopen(UPGRADE_PASSPORT_FILE, "r");
    if (pFile == NULL)
    {
        CLUP_ERROR("open file failed [%s]\n", UPGRADE_PASSPORT_FILE);
        pCfg->nStatus = 1;	//升级失败
		nRet = 0;
		goto RET;
    }
    else
    {
    	memset(strTmp, 0, sizeof(strTmp));
        fgets(strTmp, sizeof(strTmp), pFile);
        fclose(pFile);

		CLUP_DEBUG("\nszUpgradePassport=%s strTmp=%s\n",
			pCfg->szUpgradePassport, strTmp);
		
        if (0 != strncmp(pCfg->szUpgradePassport, strTmp, sizeof(pCfg->szUpgradePassport)))
		{
			pCfg->nStatus = 1;	//升级失败
			nRet = 0;
			goto RET;
		}

		pCfg->nStatus = 0;	//升级成功
    }
	#else
	if (!access(UPGRADE_PASSPORT_FILE, F_OK))	//升级凭证存在
	{
		pCfg->nStatus = 0;	//升级成功
	}
	else
	{
		pCfg->nStatus = 1;	//升级失败
	}
	#endif
	
    //获取设备版本信息
    MPU_PUB_GetDeviceVersion(&stDevInfo,0);
	strncpy(pCfg->szFirmwareName, stDevInfo.m_szSoftwareVersion, sizeof(stDevInfo.m_szSoftwareVersion) - 1);

	#if 0
	//获取升级结果后，删除上一次保存的升级凭证
	memset(strTmp, 0, sizeof(strTmp));
	snprintf(strTmp, sizeof(strTmp), "rm %s", UPGRADE_PASSPORT_FILE);
	mysystem(strTmp);
	#endif

	//有任何操作，重置重启监控计时器
	ResetMonitorTimer();

RET:
	pthread_mutex_unlock(&s_mutexInterface);
	return nRet;
}

//!\brief      获取云升级设备信息
int MPU_CloudUpgrade_GetUpgradeInfo(HY_DVR_CLOUD_UPDATE_BASIC_INFO *psDev)
{
	int nRet = 0;

	unsigned int i = 0;

	//szCustomNo 读取定制信息
	CConfig *pCfg = CConfig::GetInstance();
	if (pCfg->CheckKeyExist(CLOUD_UPGRADE_CUSTOM_SECTION_NAME, CLOUD_UPGRADE_CUSTOM_CUSTOM_NO_KEY_NAME))
	{
		//使用config的定制
		pCfg->GetValue(CLOUD_UPGRADE_CUSTOM_SECTION_NAME, 
			CLOUD_UPGRADE_CUSTOM_CUSTOM_NO_KEY_NAME, 
			CLOUD_UPGRADE_CUSTOM_CUSTOM_NO_VALUE_DEF, 
			psDev->szCustomerVersionID, sizeof(psDev->szCustomerVersionID));
	}
	else
	{
		//使用custom的定制
		pCfg = CConfig::NewInstance();
		nRet = pCfg->LoadFile(CUSTOM_INFO);
		if (0 == nRet)
		{
			pCfg->GetValue(CLOUD_UPGRADE_CUSTOM_SECTION_NAME, 
				CLOUD_UPGRADE_CUSTOM_CUSTOM_NO_KEY_NAME, 
				CLOUD_UPGRADE_CUSTOM_CUSTOM_NO_VALUE_DEF, 
				psDev->szCustomerVersionID, sizeof(psDev->szCustomerVersionID));
		}
		CConfig::ReleaseInstance(pCfg);
		pCfg = NULL;
	}

	// szClientId 优先使用授权码内部的customID
	stuShmAuthInfo stShmAuthInfo = {0};
	Authorize_GetAuthDevInfo(&stShmAuthInfo);
	if (AUTH_SUCCESS == stShmAuthInfo.dwAuthVerifyErrno)
	{
		snprintf(psDev->szCustomerID, sizeof(psDev->szCustomerID), "%X", stShmAuthInfo.dCustomID);
	}
	printf("[xxxx] Custom info !ClientId=%s CustomNo=%s\n", psDev->szCustomerID, psDev->szCustomerVersionID);

	if (0 == strlen(psDev->szCustomerID)
		|| 0 == strlen(psDev->szCustomerVersionID))
	{
		CLUP_ERROR("Custom info error!ClientId=%s CustomNo=%s\n", 
			psDev->szCustomerID, psDev->szCustomerVersionID);
		return IDVR_CIU_CLOUD_UPGRADE_UPGRADE_LIST_CUSTOM_NO_UNSUPPORT;
	}
	
	memcpy(psDev->szLanguage, PUB_SpeechLanguage_enum2str(), sizeof(psDev->szLanguage));

	// 获取软件版本
	DEVICEVERSION sDevVer = {};
	MPU_PUB_GetDeviceVersion (&sDevVer,0);
	memcpy(psDev->szCurrentVersion, sDevVer.m_szSoftwareVersion, sizeof(sDevVer.m_szSoftwareVersion));

	//获取版型及设备类型
	ZWSys_GetBoardInfo(&psDev->nBoardID);
	for (i = 0; i < gstuBoardInfoSize; i++)
	{
		if (gstuBoardInfo[i].dwBoardId == (unsigned int)psDev->nBoardID)
		{
			psDev->nDeviceType = gstuBoardInfo[i].nDeviceType;
		}
	}

	//获取类型
	snprintf(psDev->szType, sizeof("all"), "all");

	snprintf(psDev->szRequestUrl, sizeof(psDev->szRequestUrl), "/device/CloudUpgradeFirmwareListV1?boardID=%X&customer=%s&versionID=%s&lang=%s&deviceType=%X&type=%s&curVersion=%s", 
		psDev->nBoardID,
		psDev->szCustomerID,
		psDev->szCustomerVersionID,
		psDev->szLanguage,
		psDev->nDeviceType,
		psDev->szType,
		psDev->szCurrentVersion);

	printf("boardID=%X&customer=%s&versionID=%s&lang=%s&deviceType=%X&type=%s&curVersion=%s\n",
		psDev->nBoardID,
		psDev->szCustomerID,
		psDev->szCustomerVersionID,
		psDev->szLanguage,
		psDev->nDeviceType,
		psDev->szType,
		psDev->szCurrentVersion);

	printf("*** szRequestUrl=%s ***\n", psDev->szRequestUrl);
	
	return nRet;
}

