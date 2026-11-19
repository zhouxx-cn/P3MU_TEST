
/*!
*\file   CConfig.cpp
*\brief  配置管理类
*\author yuk@higheasyrd.com
*\date	  2009/03/23
*\version 0.1
*/
#include <errno.h>
#include <ctype.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

//#include "libxml/parser.h"
//#include "libxml/tree.h"
#include "global_macro.h"
#include "CConfig.h"
#include "RNL/idvr_rnl.h"
#include "Authority.h"
#ifndef NDEBUG
#include <sys/time.h>
#include <time.h>
#endif

#define PAR_LOG(level, x...) \
	do {\
	char buf[512];\
	snprintf(buf, sizeof(buf), x);\
	idvr_rnl_log(level, "PAR", "[%s line %d] %s", __FILE__, __LINE__, buf);\
	} while (0)

#define PAR_DBG(x...) //IDVR_RNL_DEBUG("PAR", "[%s:%d]", __FILE__, __LINE__);IDVR_RNL_DEBUG("PAR", x);
#define PAR_INFO(x...) PAR_LOG(LOG_LEVEL_INFO, x)
#define PAR_ERROR(x...) PAR_LOG(LOG_LEVEL_ERROR, x)

#ifndef PATH_MAX
#include <dirent.h>
#endif

struct CConfig::tagSection
{
  public:
	struct tagKey
	{
	  public:
		char *m_strName;
		char *m_strValue;
		  tagKey():m_strName(NULL), m_strValue(NULL)
		{
		};

		 ~tagKey()
		{
			//释放字符串所占内存
			if (m_strName != NULL)
			{
				free(m_strName);
				m_strName = NULL;
			}
			if (m_strValue != NULL)
			{
				free(m_strValue);
				m_strValue = NULL;
			}
		}

		void Copy(tagKey &newKey)
		{
			newKey.~tagKey();
			if (m_strName != NULL)
			{
				newKey.m_strName = strdup(m_strName);
			}
			if (m_strValue != NULL)
			{
				newKey.m_strValue = strdup(m_strValue);
			}
		}
	};

	tagSection():m_strName(NULL), m_pKeyHead(NULL), m_nKeyCount(0), m_nKeyAlloced(0)
	{
	};

	tagKey *SearchKey(const char *pszKey)
	{
		int i = 0;
		tagKey *pKey = NULL;

		for (i = 0; i < m_nKeyCount; i++)
		{
			//找到key
			if (0 == strcmp(m_pKeyHead[i].m_strName, pszKey))
			{
				pKey = m_pKeyHead + i;
				break;
			}
		}
		return pKey;
	};

	tagKey *AddKey()
	{
		int max_key_nr = DEF_KEY_NUM;

		if (m_nKeyCount != 0)
		{
			if ((m_nKeyCount+1) > m_nKeyAlloced)
			{
				max_key_nr = m_nKeyAlloced + ADD_KEY_NUM;

				//需要重新分配内存
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
				m_pKeyHead = (tagSection::tagKey *) realloc(m_pKeyHead, sizeof (tagKey) * max_key_nr);
#pragma GCC diagnostic pop
				m_nKeyAlloced = max_key_nr;
			}
		}
		else
		{
			//分配初始内存
			m_pKeyHead = (tagSection::tagKey *) malloc(sizeof (tagKey) * max_key_nr);
			m_nKeyAlloced = max_key_nr;
		}

		//内存分配失败
		if (NULL == m_pKeyHead)
		{
			return NULL;
		}

		//获取新添加的key指针
		tagKey *pKey = m_pKeyHead + m_nKeyCount;

		//初始化
		pKey->m_strName = NULL;
		pKey->m_strValue = NULL;

		//增加key数目
		m_nKeyCount++;

		return pKey;
	}

	void Copy(tagSection &newSection)
	{
		newSection.~tagSection();
		if (m_nKeyCount && m_pKeyHead)
		{
			newSection.m_pKeyHead = (tagKey *)malloc(m_nKeyAlloced * sizeof(tagKey));
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
			memset(newSection.m_pKeyHead, 0, m_nKeyAlloced * sizeof(tagKey));
#pragma GCC diagnostic pop
			for (int i = 0; i < m_nKeyCount; i++)
			{
				m_pKeyHead[i].Copy(newSection.m_pKeyHead[i]);
			}
			newSection.m_nKeyCount = m_nKeyCount;
			newSection.m_nKeyAlloced = m_nKeyAlloced;
		}
		if (m_strName != NULL)
		{
			newSection.m_strName = strdup(m_strName);
		}
	}

	~tagSection()
	{
		//删除所有key,释放内存
		if (m_pKeyHead != NULL)
		{
			int i = 0;
			tagKey *pKey = m_pKeyHead;

			for (i = 0; i < m_nKeyCount; i++)
			{
				//析构
				pKey->~tagKey();
				pKey++;
			}
			//释放数组所占内存
			free(m_pKeyHead);
			m_nKeyCount = 0;
			m_nKeyAlloced = 0;
			m_pKeyHead = NULL;
		}
		if (m_strName)
		{
			free(m_strName);
			m_strName = NULL;
		}
	};

	char *m_strName;
	tagKey *m_pKeyHead;
	int m_nKeyCount;
	int m_nKeyAlloced;
};

/* MUSL: http://www.openwall.com/lists/musl/2017/02/20/5 */
static pthread_mutex_t g_mtxCfg;
__attribute__((constructor))
static void init_tmutex(void) 
{
	pthread_mutexattr_t mutattr = {0}; 
	pthread_mutexattr_init(&mutattr); 
	pthread_mutexattr_settype(&mutattr, PTHREAD_MUTEX_RECURSIVE); 
	pthread_mutex_init(&g_mtxCfg, &mutattr); 
	pthread_mutexattr_destroy(&mutattr);
}
pthread_mutex_t CConfig::m_mtxConfig = g_mtxCfg;

pthread_mutex_t CConfig::m_mtxSaveFile =  PTHREAD_MUTEX_INITIALIZER;
pthread_t 		CConfig::m_SaveFileThread = (pthread_t)NULL;
unsigned int CConfig::m_NewCount = 0;

CConfig *CConfig::m_pEdvrCfg = NULL;
CConfig *CConfig::m_pCustomRoCfg = NULL;

static void *Fnx_SaveFileThread(void *lParam)
{
	CConfig *pNTP = (CConfig *) lParam;

	if (NULL == pNTP)
	{
		PAR_ERROR("[INTP:%s]NULL==pnet\n", __FUNCTION__);
		return NULL;
	}
	printf("%s:%d, Fnx_SaveFileThread -----!\n", __FUNCTION__, __LINE__);

	while (1)
	{
		pNTP->CheckIfNeedSaveFile();

		sleep(1);				//一秒检测一次
	}
	
	return NULL;
}

int CConfig::CheckIfNeedSaveFile()
{
	int ret = 0;
	int ifSaveFile = 0;

	LockSave();
	ifSaveFile = m_nNeedSaveFileFlag;
	m_nNeedSaveFileFlag = 0;
	UnlockSave();
	
	if(ifSaveFile)
	{
		ret = SaveFileReal();
		if(ret)
		{
			printf("%s:%d, SaveFileReal fail ret=%d!\n", __FUNCTION__, 
			__LINE__, ret);
		}
	}
	return 0;
}


CConfig *CConfig::GetInstance()
{
	if (NULL == m_pEdvrCfg)
	{
		m_pEdvrCfg = new CConfig();
		m_pEdvrCfg->m_ifSaveSoon = 0;
		m_pEdvrCfg->m_bForbidSaveFileFlag = 0;
	}

	if(!m_SaveFileThread)
	{
		int ret = 0;
		ret = pthread_create(&m_SaveFileThread, NULL, Fnx_SaveFileThread, (void *)m_pEdvrCfg);
		if (ret < 0)
		{
			PAR_ERROR("pthread_create<Fnx_SaveFileThread> failed(%d)\n", ret);
		}
	}
	
	return m_pEdvrCfg;
}

int CConfig::GetValue(const char *pszSection, const char *pszKey, int nDefault)
{
	// TODO: Add your specialized code here.
	char *ret;
	char szBuff[51];
	char szDefault[51];
	int nResult = nDefault;

	snprintf(szDefault, sizeof(szDefault), "%d", nDefault);

	//获取key的字符串值
	ret = GetValue(pszSection, pszKey, szDefault, szBuff, 50);
	if (ret == NULL)
	{
		return nResult;			//nDefault
	}

	//转换成整型值
	nResult = strtol(szBuff, NULL, 0);

	//检查是否有错误
	//但是如果字符串为"abcd" nResult = 0检查不出错误
	if (((LONG_MIN == nResult) || (LONG_MAX == nResult)) && (ERANGE == errno))
	{
		nResult = nDefault;
	}

	return (int) nResult;
}

long long CConfig::GetValueLong(const char *pszSection, const char *pszKey, long long nDefault)
{
	// TODO: Add your specialized code here.
	char *ret;
	char szBuff[51];
	char szDefault[51];
	long long nResult = nDefault;

	snprintf(szDefault, sizeof(szDefault), "%lld", nDefault);

	//获取key的字符串值
	ret = GetValue(pszSection, pszKey, szDefault, szBuff, 50);
	if (ret == NULL)
	{
		return nResult;			//nDefault
	}

	//转换成长整型值
	nResult = strtoll(szBuff, NULL, 0);

	//检查是否有错误
	//但是如果字符串为"abcd" nResult = 0检查不出错误
	if (((LONG_MIN == nResult) || (LONG_MAX == nResult)) && (ERANGE == errno))
	{
		nResult = nDefault;
	}

	return (long long) nResult;
}


int CConfig::SetValue(const char *pszSection, const char *pszKey, int nValue)
{
	// TODO: Add your specialized code here.
	char szBuf[100];

	snprintf(szBuf,sizeof(szBuf), "%d", nValue);
	return SetValue(pszSection, pszKey, szBuf);
}

int CConfig::SetValueLong(const char *pszSection, const char *pszKey, long long nValue)
{
	// TODO: Add your specialized code here.
	char szBuf[100];

	snprintf(szBuf,sizeof(szBuf), "%lld", nValue);
	return SetValue(pszSection, pszKey, szBuf);
}


int CConfig::UnloadFile()
{
	Lock();
	// TODO: Add your specialized code here.
	int nResult = -1;

	//配置未绑定到文件
	if (NULL == m_strFile)
	{
		Unlock();
		return -1;
	}

	free(m_strFile);
	m_strFile = NULL;

	nResult = FreeMemory();

	Unlock();
	return (int) nResult;
}

int CConfig::LockBakFile()
{

	m_uninitStartFlag=1;
	return 0;
}

/* 禁止保存参数的操作，一般用在完全恢复默认的时候，使用该操作之后，系统将不能将内存参数写入配置文件 */
int CConfig::ForbiddenSaveFile()
{
	Lock();
    m_bForbidSaveFileFlag = 1;
	Unlock();

    return 0;
}

/* 解除禁止保存参数的操作，暂时未用到，使用该操作之后，解除ForbiddenSaveFile的锁定 */
int CConfig::UnForbiddenSaveFile()
{
	Lock();
    m_bForbidSaveFileFlag = 0;
	Unlock();

    return 0;
}

//仅用于原配置文件和默认配置文件均正常时的恢复
int CConfig::ResumeDefCfg(const char *pszDefFileName, const char *pszFlagFile)
{
	if (NULL == pszDefFileName)
	{
		return IDVR_ERROR_PARAMETER;
	}

	//配置未绑定到文件则不能恢复默认配置
	Lock();
	if (NULL == m_strFile)
	{
		Unlock();
		return -1;
	}

	//检查文件是否存在
	//不存在则直接返回
	if (!IsFileExsit(pszDefFileName))
	{
		Unlock();
		return IDVR_ERROR_FILE_NOT_EXSIT;
	}

	char *strFileName = strdup(m_strFile);

	CConfig objDefConfig;
	CConfig objConfigFlag;

	//默认配置文件
	PAR_DBG("Begin Load DefaultFile %s\n", pszDefFileName);
	int ret = objDefConfig.LoadFile(pszDefFileName);
	PAR_DBG("End Load DefaultFile, ret = %d\n", ret);

	if (IDVR_PAR_ERROR_EMPTY_FILE == ret)
	{
		free(strFileName);
		strFileName = NULL;
		objDefConfig.UnloadFile();
		Unlock();
		return IDVR_PAR_ERROR_EMPTY_FILE;
	}
	else if (ret != 0)
	{
		free(strFileName);
		strFileName = NULL;
		PAR_ERROR("[%s]ERR:Load Default Config File %s failed", __FUNCTION__, pszDefFileName);
		Unlock();
		return ret;
	}

	if (NULL == pszFlagFile)
	{
		pszFlagFile = "/mnt/mtd/config/Edvr.flag";
	}
	
	//有标记文件,则除了标记为不恢复默认的key其他都恢复默认
	if (pszFlagFile != NULL)
	{
		int i = 0;
		int j = 0;
		char szValue[256];

		//加载标记文件
		ret = objConfigFlag.LoadFile(pszFlagFile);
		if (IDVR_PAR_ERROR_EMPTY_FILE == ret)
		{
			//直接覆盖
		}
		else if (ret != 0)
		{
			PAR_ERROR("[%s]ERR:Load Flag Config File %s failed", __FUNCTION__, pszFlagFile);
			free(strFileName);
			strFileName = NULL;
			objDefConfig.UnloadFile();
			Unlock();
			return ret;
		}
		else
		{
			for (i = 0; i < objConfigFlag.m_nSectionCount; i++)
			{
				for (j = 0; j < objConfigFlag.m_pSection[i].m_nKeyCount; j++)
				{
					//如果该key不恢复默认值
					if (1 == strtol(objConfigFlag.m_pSection[i].m_pKeyHead[j].m_strValue, NULL, 0))
					{
						PAR_DBG("No Resume Default Value %s\n", objConfigFlag.m_pSection[i].m_pKeyHead[j].m_strName);
						char *pValue = InternalGetValue(objConfigFlag.m_pSection[i].m_strName,
												objConfigFlag.m_pSection[i].m_pKeyHead[j].m_strName, NULL, szValue, 256);
						PAR_DBG("Cur Value = %s\n", szValue);

						if (NULL != pValue)
						{
							objDefConfig.InternalSetValue(objConfigFlag.m_pSection[i].m_strName, objConfigFlag.m_pSection[i].m_pKeyHead[j].m_strName, szValue);
						}
					}
				}
			}
		}
	}

	//备份当前绑定的文件
	BackupFile(m_strFile);

	int nResult = UnloadFile();

	if (nResult != 0)
	{
		if (strFileName)
			free(strFileName);
		strFileName = 0;
	    Unlock();
		return -1;
	}

	PAR_DBG("SaveAsFile %s\n", strFileName);
	objDefConfig.SaveAsFile(strFileName);
	LoadFile(strFileName);

	free(strFileName);
	strFileName = NULL;
	Unlock();
	return (int) 0;
}

int CConfig::UpdateCfg(const char *pszNewCfgFileName, const char *pszFlagFile)
{
	if (NULL == pszNewCfgFileName)
	{
		return IDVR_ERROR_PARAMETER;
	}

	Lock();
	//配置未绑定到文件则不能恢复默认配置
	if (NULL == m_strFile)
	{
		Unlock();
		return -1;
	}

	//检查文件是否存在
	//不存在则直接返回
	if (!IsFileExsit(pszNewCfgFileName))
	{
		Unlock();
		return IDVR_ERROR_FILE_NOT_EXSIT;
	}

	char *strFileName = strdup(m_strFile);

	CConfig objUpdateConfig;
	CConfig objUpdateFlag;

	//读取新的配置文件
	PAR_DBG("Begin Load DefaultFile %s\n", pszNewCfgFileName);
	int ret = objUpdateConfig.LoadFile(pszNewCfgFileName);
	PAR_DBG("End Load DefaultFile, ret = %d\n", ret);

	//新的配置文件为空，不更新
	if (IDVR_PAR_ERROR_EMPTY_FILE == ret)
	{
		free(strFileName);
		strFileName = NULL;
		objUpdateConfig.UnloadFile();
		Unlock();
		return IDVR_PAR_ERROR_EMPTY_FILE;
	}
	//加载出错
	else if (ret != 0)
	{
		free(strFileName);
		strFileName = NULL;
		PAR_ERROR("[%s]ERR:Load Update Config File %s failed", __FUNCTION__, pszNewCfgFileName);
		Unlock();
		return ret;
	}

	bool bDirectCover = false;
	//没有标记文件
	if (NULL == pszFlagFile)
	{
		bDirectCover = true;
	}

	int i = 0;
	int j = 0;
	char szValue[256];
	//有标记文件,则除了标记为更新的key其他都不更新
	if (pszFlagFile != NULL)
	{
		//加载标记文件
		ret = objUpdateFlag.LoadFile(pszFlagFile);

		//加载标记文件出错
		if ((ret != 0) && (ret != IDVR_PAR_ERROR_EMPTY_FILE))
		{
			PAR_ERROR("[%s]ERR:Load Flag Config File %s failed", __FUNCTION__, pszFlagFile);
			free(strFileName);
			strFileName = NULL;
			objUpdateConfig.UnloadFile();
			Unlock();
			return ret;
		}
	}

	//备份当前绑定的文件
	BackupFile(m_strFile);

	int bCovered = 0;
	if (IDVR_PAR_ERROR_EMPTY_FILE == ret)
	{
		//直接覆盖
		bDirectCover = true;
	}

	//更新新配置文件中的配置到原配置中
	char szCovered[INI_MAX_KEY_VALUE_LENGTH + 20];
	for (i = 0; i < objUpdateConfig.m_nSectionCount; i++)
	{
		for (j = 0; j < objUpdateConfig.m_pSection[i].m_nKeyCount; j++)
		{
			if (bDirectCover)
			{
				bCovered = 1;
			}
			else//获取是否覆盖原配置
			{
				objUpdateFlag.InternalGetValue(objUpdateConfig.m_pSection[i].m_strName, objUpdateConfig.m_pSection[i].m_pKeyHead[j].m_strName, "0", szCovered, INI_MAX_KEY_VALUE_LENGTH + 10);
				if ('\0' == szCovered[0])
				{
					bCovered = 0;
				}
				else if ('1' != szCovered[0])
				{
					bCovered = 0;
				}
				else if ('\0' != szCovered[1])
				{
					bCovered = 0;
				}
				else
				{
					bCovered = 1;
				}

				//bCovered = objUpdateFlag.GetValue(objUpdateConfig.m_pSection[i].m_strName, objUpdateConfig.m_pSection[i].m_pKeyHead[j].m_strName, 1);
			}

			//如果该key值被更新
			if (1 == bCovered)
			{
				PAR_DBG("Update Value %s\n", objUpdateConfig.m_pSection[i].m_pKeyHead[j].m_strName);

				//获取更新的值
				char *pValue = objUpdateConfig.InternalGetValue(objUpdateConfig.m_pSection[i].m_strName,
					objUpdateConfig.m_pSection[i].m_pKeyHead[j].m_strName, NULL, szValue, 256);
				PAR_DBG("Cur Value = %s\n", szValue);

				if (NULL != pValue)//更新值到配置文件
				{
					InternalSetValue(objUpdateConfig.m_pSection[i].m_strName, objUpdateConfig.m_pSection[i].m_pKeyHead[j].m_strName, szValue);
				}
			}
		}
	}

	PAR_DBG("SaveAsFile %s\n", strFileName);
	SaveAsFile(strFileName);

	objUpdateConfig.UnloadFile();
	objUpdateFlag.UnloadFile();
	free(strFileName);
	strFileName = NULL;
	Unlock();
	return (int) 0;
}


CConfig *CConfig::NewInstance()
{
	CConfig *pObject = new CConfig();
	pObject->m_ifSaveSoon = 1;
	pObject->m_bForbidSaveFileFlag = 0;

	m_NewCount++;
	if(m_NewCount > CONFIG_INSTANCE_WAINING_THRESHOLD)
	{
		printf("[WARN] config instance count %d > %d !\n", m_NewCount, CONFIG_INSTANCE_WAINING_THRESHOLD);
	}
	return pObject;
}


void CConfig::ReleaseInstance(CConfig * pInstance)
{
	if (pInstance != NULL)
	{
		pInstance->UnloadFile();
		delete pInstance;
		pInstance = NULL;
		m_NewCount--;
	}
}


int CConfig::LoadFile(const char *pszFileName)
{
	if (NULL == pszFileName)
	{
		return IDVR_ERROR_PARAMETER;
	}
	m_uninitStartFlag = 0;

	Lock();
	if (NULL != m_strFile)		//如果配置已绑定到一个文件,则先释放先前的绑定和内存空间
	{
		UnloadFile();
	}

	int nResult = GetFileType(pszFileName);
	m_nType = nResult;

	switch (nResult)
	{
		case 0:
			nResult = LoadIniFile(pszFileName);
			break;
		case 1:
			nResult = LoadXmlFile(pszFileName);
			break;
		default:
			break;
	}

	PAR_DBG("nResult = %d\n", nResult);
	if ((0 != nResult) && (IDVR_PAR_ERROR_EMPTY_FILE != nResult))
	{
		FreeMemory();
	}
	
	//打印验证正确性
#if 0
	if (0 == nResult)
	{
		Print();
	}
#endif

	Unlock();
	return nResult;
}

int CConfig::LoadFileRO(const char *pszFileName)
{
	if (NULL == pszFileName)
	{
		return IDVR_ERROR_PARAMETER;
	}
	m_uninitStartFlag = 0;

	Lock();
	if (NULL != m_strFile)		//如果配置已绑定到一个文件,则先释放先前的绑定和内存空间
	{
		UnloadFile();
	}

	int nResult = GetFileTypeRO(pszFileName);
	m_nType = nResult;

	switch (nResult)
	{
		case 0:
			nResult = LoadIniFileRO(pszFileName);
			break;
		case 1:
			nResult = LoadXmlFile(pszFileName);
			break;
		default:
			break;
	}

	PAR_DBG("nResult = %d\n", nResult);
	if ((0 != nResult) && (IDVR_PAR_ERROR_EMPTY_FILE != nResult))
	{
		FreeMemory();
	}
	
	//打印验证正确性
#if 0
	if (0 == nResult)
	{
		Print();
	}
#endif

	Unlock();
	return nResult;
}

int CConfig::SaveFileReal(int bNotNeedSaveBak)
{
	// TODO: Add your specialized code here.
	int nResult = -1;
	CConfig cfgTemp;
	char szStrTmp[256] = {0};

    if (1 == m_bForbidSaveFileFlag)
	{
		printf("Current forbidden save files!\n");
		return 0;
	}
	
	//if(0 == m_uninitStartFlag)
		//修改前保存Edvr.cfg为Edvr.cfg-bak(20141208)
	if(1 == m_uninitStartFlag)
		return 0;
	
	if (0 == bNotNeedSaveBak)
	{
		//配置改变或配置不存在
		//则执行保存至文件
		memset(szStrTmp, 0, sizeof(szStrTmp));
		snprintf(szStrTmp, sizeof(szStrTmp), "%s-bak", m_strFile);
		if (m_iCfgDataChanged
			|| 0 != access(szStrTmp, F_OK))
		{
	    	BackupFile(m_strFile);	
		}
	}

	//配置改变或配置不存在
	//则执行保存至文件
	if (m_iCfgDataChanged
		|| 0 != access(m_strFile, F_OK))
	{
		Copy(cfgTemp);
		LockSave();
		nResult = cfgTemp.SaveIniFile(m_strFile);
		if (0 == nResult)
		{
			m_iCfgDataChanged = 0;
		}
		UnlockSave();
	}

	return (int) nResult;
}

int CConfig::SaveFile()
{
	int nResult = -1;

	if(m_ifSaveSoon)
	{
		nResult = SaveFileReal();
	}
	else
	{
		if (1 == m_bForbidSaveFileFlag)
		{
			printf("Current forbidden save files!\n");
			return 0;
		}

		LockSave();
		m_nNeedSaveFileFlag = 1;
		UnlockSave();
		nResult = 0;
	}

	return nResult;
}

char *CConfig::GetValue(const char *pszSection, const char *pszKey, const char *pszDefault, char *pValue, int nSize)
{
	char *pResult = NULL;
	Lock();
	pResult = InternalGetValue(pszSection, pszKey, pszDefault, pValue, nSize);
	Unlock();
	return pResult;
}


char *CConfig::InternalGetValue(const char *pszSection, const char *pszKey, const char *pszDefault, char *pValue, int nSize)
{
	if ((NULL == pValue) || (nSize <= 0) || (NULL == pszSection) || (0 == strlen(pszSection)) || (NULL == pszKey) || (0 == strlen(pszKey)))
	{
		return NULL;
	}

	char *pResult = NULL;

	CConfig::tagSection::tagKey * pKey = NULL;
	tagSection *pSection = SearchSection(pszSection);

	//没找到section,添加一个section
	if (pSection == NULL)
	{		
		pSection = InsertSection();
		if (NULL == pSection)
		{
			return NULL;
		}
		pSection->m_strName = strdup(pszSection);
		m_nSectionCount++;
		m_iCfgDataChanged = 1;        
	}
	
	pKey = pSection->SearchKey(pszKey);

	//没找到key添加一个key
	char *pKeyName = NULL;
	char *pKeyValue = NULL;

	if (NULL == pKey)
	{
		pKeyName = strdup(pszKey);
		if (NULL == pKeyName)
		{
			return NULL;	//内存不足
		}

		pKey = pSection->AddKey();
		if (NULL == pKey)			//添加key失败
		{
			free(pKeyName);
			pKeyName = NULL;
			return NULL;
		}
		pKey->m_strName = pKeyName;

		if (pszDefault != NULL)
		{
			pKeyValue = strdup(pszDefault);
		}
		else
		{
			pKeyValue = strdup("");
		}
		
		if (NULL == pKeyValue)
		{
			if (pKeyName != NULL)
			{
				free(pKeyName);
				pKeyName = NULL;
			}
			return NULL;
		}

		pKey->m_strValue = pKeyValue;
		m_iCfgDataChanged = 1;
	}

	if (nSize > 0)
	{
		//PAR_DBG("Get the Key %s\n", pKey->m_strName);
		strncpy(pValue, pKey->m_strValue, nSize - 1);
		pValue[nSize - 1] = '\0';
		pResult = pValue;
	}
	//PAR_DBG("Get pResult:%s\n", pResult);

	return pResult;
}

int CConfig::SetValue(const char *pszSection, const char *pszKey, const char *pValue)
{
	int nResult = 0;
	Lock();
	nResult = InternalSetValue(pszSection, pszKey, pValue);
	Unlock();
	return nResult;
}

int CConfig::InternalSetValue(const char *pszSection, const char *pszKey, const char *pValue)
{
	if ((NULL == pValue) || (NULL == pszSection) || (0 == strlen(pszSection)) || (NULL == pszKey) || (0 == strlen(pszKey)))
	{
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	CConfig::tagSection::tagKey * pKey = NULL;

	static int nStatus = IDVR_PAR_ERROR_EMPTY_FILE;

#if 1
	if (NULL == m_pCustomRoCfg)
	{
		m_pCustomRoCfg = new CConfig();
	}

	if (m_pCustomRoCfg != NULL && IDVR_PAR_ERROR_EMPTY_FILE == nStatus)//第一次默认customrocfg.flag不存在
	{
		nStatus = m_pCustomRoCfg->LoadFileRO(CUSTOM_READONLY_CONFIG_FILE);
	}
	
	if (m_pCustomRoCfg != NULL && 0 == nStatus)
	{
		//查找section
		tagSection *pROSection = m_pCustomRoCfg->SearchSection(pszSection);

		if (pROSection != NULL)
		{
			//查找key
			if (pROSection->SearchKey(pszKey) != NULL)
			{
				PAR_DBG("find key\n");
				m_bReadOnlyCfgFlag = 1;
				return 0;
			}
			else
			{
				PAR_DBG("not find key\n");
				m_bReadOnlyCfgFlag = 0;
			}
		}
		else
		{
			PAR_DBG("not find section\n");
			m_bReadOnlyCfgFlag = 0;
		}
	}
	else
	{
		PAR_DBG("not file %s\n", CUSTOM_READONLY_CONFIG_FILE);
		m_bReadOnlyCfgFlag = 0;
	}
#endif

	//查找section
	tagSection *pSection = SearchSection(pszSection);

	//没找到section,添加一个section
	if (pSection == NULL)
	{
		pSection = InsertSection();
		if (NULL == pSection)
		{
			return IDVR_ERROR_ALLOC;
		}
		pSection->m_strName = strdup(pszSection);
		m_nSectionCount++;
		m_iCfgDataChanged = 1;	//添加 section 已成功        
	}

	pKey = pSection->SearchKey(pszKey);
	//没找到key添加一个key
	char *pKeyName = NULL;
	char *pKeyValue = NULL;

	if (NULL == pKey)
	{
		pKeyName = strdup(pszKey);
		if (NULL == pKeyName)
		{
			return IDVR_ERROR_ALLOC;	//内存不足
		}

		pKey = pSection->AddKey();
		
		if (NULL == pKey)			//添加key失败
		{
			free(pKeyName);
			pKeyName = NULL;
			return IDVR_ERROR_ALLOC;
		}
		pKey->m_strName = pKeyName;

	}
	
	pKeyValue = strdup(pValue);
	
	if (NULL == pKeyValue)
	{
		if (pKeyName != NULL)
		{
			free(pKeyName);
			pKeyName = NULL;
		}
		return IDVR_ERROR_ALLOC;
	}
	
	if (pKey->m_strValue != NULL)
	{
		if ( (strlen(pKey->m_strValue) == strlen(pKeyValue)) 
			&& (0 == memcmp(pKey->m_strValue, pKeyValue, strlen(pKeyValue))) ) //内容一致
		{
			free(pKeyValue);
			pKeyValue = NULL;
			return 0;
		}	
		//释放key值原占有的内存
		free(pKey->m_strValue);
		pKey->m_strValue = NULL;
	}
	pKey->m_strValue = pKeyValue;
    m_iCfgDataChanged = 1;

	return (int) 0;
}


int CConfig::SaveAsFile(const char *pszFileName)
{
	// TODO: Add your specialized code here.
	PAR_DBG("Begin Save\n");
	CConfig cfgTemp;

	Lock();
	if (NULL == m_pSection)
	{
		Unlock();
		return IDVR_PAR_ERROR_NOSECTION;
	}

	if ((NULL == pszFileName) || (0 == strlen(pszFileName)))
	{
		Unlock();
		return IDVR_ERROR_PARAMETER;
	}
	Unlock();
	Copy(cfgTemp);
	LockSave();
	PAR_DBG("here\n");
	int nType = 0;
	int nlen = strlen(pszFileName);
	if ((nlen > 3) && (0 == strcmp(".xml", pszFileName + nlen - 4)))
	{
		nType = 1;
	}

	int nResult = -1;
	switch (nType)
	{
		case 0:
			nResult = cfgTemp.SaveIniFile(pszFileName);
			break;
		case 1:
			nResult = cfgTemp.SaveXmlFile(pszFileName);
			break;
		default:
			break;
	}
	UnlockSave();
	PAR_DBG("End Save\n");
	return (int) nResult;
}


CConfig::CConfig():hCfg(NULL), m_strFile(NULL), m_pSection(NULL), m_nSectionAlloced(0)
{
    m_nSectionCount = 0;
    m_iCfgDataChanged = 0;

}

CConfig::CConfig(CConfig & cfg)
{
	//不实现该函数,外部不能调用此函数
	cfg.Print();
}

void CConfig::Copy(CConfig & cfg)
{
	cfg.Lock();
	if (cfg.m_pSection && cfg.m_nSectionCount)
	{
		for (int i = 0; i < cfg.m_nSectionCount; i++)
		{
			//释放为每个section的名称和key分配的内存
			cfg.m_pSection[i].~tagSection();
		}
		//删除section数组
		free(cfg.m_pSection);
		cfg.m_pSection = NULL;
		cfg.m_nSectionCount = 0;
		cfg.m_nSectionAlloced = 0;
	}
	Lock();
	if (m_nSectionCount && m_pSection)
	{
		cfg.m_pSection = (tagSection *)malloc(sizeof(tagSection)*m_nSectionAlloced);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
		memset(cfg.m_pSection, 0, sizeof(tagSection)*m_nSectionAlloced);
#pragma GCC diagnostic pop
		for (int i = 0; i < m_nSectionCount; i++)
		{
			m_pSection[i].Copy(cfg.m_pSection[i]);
		}
		cfg.m_nSectionCount = m_nSectionCount;
		cfg.m_nSectionAlloced = m_nSectionAlloced;
	}
	Unlock();
	cfg.Unlock();
}

CConfig::~CConfig()
{
	if (m_pSection != NULL)
	{
		int i = 0;
		tagSection *pSection = m_pSection;

		for (i = 0; i < m_nSectionCount; i++)
		{
			//释放为每个section的名称和key分配的内存
			pSection->~tagSection();
			pSection++;
		}
		//删除section数组
		free(m_pSection);
		m_pSection = NULL;
		m_nSectionCount = 0;
		m_nSectionAlloced = 0;
	}
	//释放绑定的文件名
	if (m_strFile != NULL)
	{
		free(m_strFile);
		m_strFile = NULL;
	}
}

int CConfig::GetFileTypeRO(const char *pszFileName)
{
	if ((NULL == pszFileName) || (strlen(pszFileName) > (PATH_MAX - 1)))
	{
		return -1;
	}

	FILE *fp = fopen(pszFileName, "r");

	if (NULL == fp)
	{
		return 0;
	}

	char *current = NULL;
	char szBuff[INI_LINE_LENGTH + 1 + 1];
	while (1)
	{
		const char *ret = fgets(szBuff, INI_LINE_LENGTH, fp);
		if (NULL == ret)
		{
			if (feof(fp))
			{
				fclose(fp);
				return 0;		//读取文件结束
			}
			else				//文件IO错误
			{
				fclose(fp);
				return -1;
			}
		}

		current = szBuff;

		while (*current == ' ' || *current == '\t')
		{
			current++;
		}

		if ((*current == ';') || (*current == '#') || (*current == '\n'))
		{
			continue;
		}

		if (*current == '[')
		{
			fclose(fp);
			return 0;
		}
		else if (*current == '<')
		{
			fclose(fp);
			return 1;
		}
		else
		{
			fclose(fp);
			return -1;
		}
	}

	fclose(fp);
	return -1;
}

int CConfig::GetFileType(const char *pszFileName)
{
	if ((NULL == pszFileName) || (strlen(pszFileName) > (PATH_MAX - 1)))
	{
		return -1;
	}

	FILE *fp = fopen(pszFileName, "r");

	if (NULL == fp)
	{
		return 0;
	}

	char *current = NULL;
	char szBuff[INI_LINE_LENGTH + 1 + 1];
	while (1)
	{
		const char *ret = fgets(szBuff, INI_LINE_LENGTH, fp);
		if (NULL == ret)
		{
			if (feof(fp))
			{
				fclose(fp);
				return 0;		//读取文件结束
			}
			else				//文件IO错误
			{
				fclose(fp);
				return -1;
			}
		}

		current = szBuff;

		while (*current == ' ' || *current == '\t')
		{
			current++;
		}

		if ((*current == ';') || (*current == '#') || (*current == '\n'))
		{
			continue;
		}

		if (*current == '[')
		{
			fclose(fp);
			return 0;
		}
		else if (*current == '<')
		{
			fclose(fp);
			return 1;
		}
		else
		{
			fclose(fp);
			return -1;
		}
	}

	fclose(fp);
	return -1;
}

int CConfig::LoadIniFileRO(const char *pszFileName)
{
	FILE *fp;

	//int max_sect_nr = DEF_SEC_NUM;
	//tagSection *pSect;

	if (NULL == pszFileName)
	{
		//写debug日志,错误的参数
		PAR_ERROR("[%s]ERR:Bad Parameters", __FUNCTION__);
		return IDVR_ERROR_PARAMETER;
	}

	m_nCurrentLine = 0;
	fp = fopen(pszFileName, "r");
	if (NULL == fp)
	{
		fp = fopen(pszFileName, "w+");
		if (NULL == fp)
		{
			//写系统日志,配置文件不存在并且创建失败
			PAR_ERROR("[%s]ERR:No Cfg File %s", __FUNCTION__, pszFileName);
			return IDVR_ERROR_FILE_NOT_EXSIT;
		}
	}

	m_nSectionCount = 0;
	m_pSection = NULL;
	/* we allocate 15 sections first */
	//m_pSection = new tagSection[max_sect_nr];
	tagSection tSect;

	PAR_DBG("Begin Read\n");
	int ret = 0;
	while (0 == (ret = ReadIniSection(fp, &tSect)))
	{
		if (tSect.m_strName != NULL)
		{
			if (0 == m_nSectionCount)
			{
				if (!(((0 == strcmp(tSect.m_strName, "system"))
					&& (tSect.m_pKeyHead != NULL)
					&& (tSect.m_pKeyHead->m_strName != NULL)
					&& (0 == strcmp(tSect.m_pKeyHead->m_strName, "dvr"))
					&& (0 == strcmp(tSect.m_pKeyHead->m_strValue, "idvr9000")))
					|| ((0 == strcmp(tSect.m_strName, ParityStart))
						&& (tSect.m_pKeyHead != NULL)
						&& (tSect.m_pKeyHead->m_strName != NULL)
						&& (0 == strcmp(tSect.m_pKeyHead->m_strName, ParityKey)) 
						&& (0 == strcmp(tSect.m_pKeyHead->m_strValue, ParityVal)))
					))
				{
					fclose(fp);
					// 之前如果此处KeyHead = NULL,会导致Segment fault
					if (tSect.m_pKeyHead)
					{
						PAR_ERROR("Section = %s, tSect.m_pKeyHead->m_strName = %s, tSect.m_pKeyHead->m_strValue = %s\n", tSect.m_strName, tSect.m_pKeyHead->m_strName, tSect.m_pKeyHead->m_strValue);
					}
					else
					{
						PAR_ERROR("Section = %s, tSect.m_pKeyHead=NULL\n", tSect.m_strName);
					}
					PAR_ERROR("Invalid File Format, [system] Section Error, At or Before Line = %d\n", m_nCurrentLine);
					return IDVR_PAR_ERROR_INVALID_FORMAT;
				}
			}
			
			//添加新的section
			tagSection *pCurSection = AddSection();

			if (pCurSection == NULL)
			{
				if (m_pSection != NULL)
				{
					free(m_pSection);
					m_pSection = NULL;
				}
				PAR_ERROR("Add Section Failed, Can not Allocate Memory\n");
				fclose(fp);
				return IDVR_ERROR_ALLOC;
			}
			m_nSectionCount++;
			pCurSection->m_strName = tSect.m_strName;
			pCurSection->m_pKeyHead = tSect.m_pKeyHead;
			pCurSection->m_nKeyCount = tSect.m_nKeyCount;
			pCurSection->m_nKeyAlloced = tSect.m_nKeyAlloced;
			tSect.m_strName = NULL;
			tSect.m_pKeyHead = NULL;
			tSect.m_nKeyCount = 0;
			tSect.m_nKeyAlloced = 0;
		}
		if (feof(fp))
		{
			if (0 != m_nSectionCount)
			{
				if((NULL == m_pSection) || (NULL == m_pSection[m_nSectionCount - 1].m_strName)
					|| (NULL == m_pSection[m_nSectionCount - 1].m_pKeyHead)
					|| (NULL == m_pSection[m_nSectionCount - 1].m_pKeyHead->m_strName)
					|| (NULL == m_pSection[m_nSectionCount - 1].m_pKeyHead->m_strValue))
				{
					fclose(fp);
					PAR_ERROR("Invalid File Format, [end] Section Error, At or Before Line = %d\n", m_nCurrentLine);
					return IDVR_PAR_ERROR_INVALID_FORMAT;
				}
				else
					{
						if (!(((0 == strcmp("end", m_pSection[m_nSectionCount - 1].m_strName))
							&& (m_pSection != NULL)
							&& (m_pSection[m_nSectionCount - 1].m_pKeyHead->m_strName != NULL)
							&& (0 == strcmp(m_pSection[m_nSectionCount - 1].m_pKeyHead->m_strName, "dvr")) 
							&& (0 == strcmp(m_pSection[m_nSectionCount - 1].m_pKeyHead->m_strValue, "idvr9000")))
							|| ((0 == strcmp(ParityEnd, m_pSection[m_nSectionCount - 1].m_strName))
							&& (m_pSection != NULL)
							&& (m_pSection[m_nSectionCount - 1].m_pKeyHead->m_strName != NULL)
							&& (0 == strcmp(m_pSection[m_nSectionCount - 1].m_pKeyHead->m_strName, ParityKey)) 
							&& (0 == strcmp(m_pSection[m_nSectionCount - 1].m_pKeyHead->m_strValue, ParityVal)))
							))
						{
							fclose(fp);
							PAR_ERROR("Invalid File Format, [end] Section Error, At or Before Line = %d\n", m_nCurrentLine);
							return IDVR_PAR_ERROR_INVALID_FORMAT;
						}
					}
			}
			else
			{
				//空文件,添加开始段
				tSect.m_strName = strdup(ParityStart);
				
				//添加新的key
				tagSection::tagKey *pKey = tSect.AddKey();
				if (NULL == pKey)
				{
					//写系统日志,分配内存失败
					PAR_ERROR("Add Key Failed, Can not Allocate Memory\n");
					return IDVR_ERROR_ALLOC;
				}
				pKey->m_strName = strdup(ParityKey);
				pKey->m_strValue = strdup(ParityVal);

				//添加新的section
				tagSection *pCurSection = AddSection();

				if (pCurSection == NULL)
				{
					if (m_pSection != NULL)
					{
						free(m_pSection);
						m_pSection = NULL;
					}
					PAR_ERROR("Add Section Failed, Can not Allocate Memory\n");
					fclose(fp);
					return IDVR_ERROR_ALLOC;
				}
				m_nSectionCount++;
				pCurSection->m_strName = tSect.m_strName;
				pCurSection->m_pKeyHead = tSect.m_pKeyHead;
				pCurSection->m_nKeyCount = tSect.m_nKeyCount;
				pCurSection->m_nKeyAlloced = tSect.m_nKeyAlloced;
				tSect.m_strName = NULL;
				tSect.m_pKeyHead = NULL;
				tSect.m_nKeyCount = 0;

				//添加结束段
				tSect.m_strName = strdup(ParityEnd);
				
				//添加新的key
				pKey = tSect.AddKey();
				if (NULL == pKey)
				{
					//写系统日志,分配内存失败
					PAR_ERROR("Add Key Failed, Can not Allocate Memory\n");
					fclose(fp);
					return IDVR_ERROR_ALLOC;
				}
				pKey->m_strName = strdup(ParityKey);
				pKey->m_strValue = strdup(ParityVal);

				//添加新的section
				pCurSection = AddSection();

				if (pCurSection == NULL)
				{
					if (m_pSection != NULL)
					{
						free(m_pSection);
						m_pSection = NULL;
					}
					PAR_ERROR("Add Section Failed, Can not Allocate Memory\n");
					fclose(fp);
					return IDVR_ERROR_ALLOC;
				}
				m_nSectionCount++;
				pCurSection->m_strName = tSect.m_strName;
				pCurSection->m_pKeyHead = tSect.m_pKeyHead;
				pCurSection->m_nKeyCount = tSect.m_nKeyCount;
				pCurSection->m_nKeyAlloced = tSect.m_nKeyAlloced;
				tSect.m_strName = NULL;
				tSect.m_pKeyHead = NULL;
				tSect.m_nKeyCount = 0;
				ret = IDVR_PAR_ERROR_EMPTY_FILE;
			}

			//读取文件结束
			PAR_DBG("End of File\n");
			break;
		}
	}

	if (m_strFile != NULL)
	{
		free(m_strFile);
		m_strFile = NULL;
	}

	if ((0 == ret) || (IDVR_PAR_ERROR_EMPTY_FILE == ret))
	{
		m_strFile = strdup(pszFileName);
	}
	
	fclose(fp);

	PAR_DBG("Load File, return %d\n", ret);
	return ret;
}

int CConfig::LoadIniFile(const char *pszFileName)
{
	FILE *fp;

	//int max_sect_nr = DEF_SEC_NUM;
	//tagSection *pSect;

	if (NULL == pszFileName)
	{
		//写debug日志,错误的参数
		PAR_ERROR("[%s]ERR:Bad Parameters", __FUNCTION__);
		return IDVR_ERROR_PARAMETER;
	}

	m_nCurrentLine = 0;
	fp = fopen(pszFileName, "r");
	if (NULL == fp)
	{
		fp = fopen(pszFileName, "w");
		if (NULL == fp)
		{
			fp = fopen(pszFileName, "r");
			if (NULL == fp)
			{
				//写系统日志,配置文件不存在并且创建失败
				PAR_ERROR("[%s]ERR:No Cfg File %s", __FUNCTION__, pszFileName);
				return IDVR_ERROR_FILE_NOT_EXSIT;
			}
		}
	}

	m_nSectionCount = 0;
	m_pSection = NULL;
	/* we allocate 15 sections first */
	//m_pSection = new tagSection[max_sect_nr];
	tagSection tSect;

	PAR_DBG("Begin Read\n");
	int ret = 0;
	while (0 == (ret = ReadIniSection(fp, &tSect)))
	{
		if (tSect.m_strName != NULL)
		{
			if (0 == m_nSectionCount)
			{
				if (!(((0 == strcmp(tSect.m_strName, "system"))
					&& (tSect.m_pKeyHead != NULL)
					&& (tSect.m_pKeyHead->m_strName != NULL)
					&& (0 == strcmp(tSect.m_pKeyHead->m_strName, "dvr"))
					&& (0 == strcmp(tSect.m_pKeyHead->m_strValue, "idvr9000")))
					|| ((0 == strcmp(tSect.m_strName, ParityStart))
						&& (tSect.m_pKeyHead != NULL)
						&& (tSect.m_pKeyHead->m_strName != NULL)
						&& (0 == strcmp(tSect.m_pKeyHead->m_strName, ParityKey)) 
						&& (0 == strcmp(tSect.m_pKeyHead->m_strValue, ParityVal)))
					))
				{
					fclose(fp);
					// 之前如果此处KeyHead = NULL,会导致Segment fault
					if (tSect.m_pKeyHead)
					{
						PAR_ERROR("Section = %s, tSect.m_pKeyHead->m_strName = %s, tSect.m_pKeyHead->m_strValue = %s\n", tSect.m_strName, tSect.m_pKeyHead->m_strName, tSect.m_pKeyHead->m_strValue);
					}
					else
					{
						PAR_ERROR("Section = %s, tSect.m_pKeyHead=NULL\n", tSect.m_strName);
					}
					PAR_ERROR("Invalid File Format, [system] Section Error, At or Before Line = %d\n", m_nCurrentLine);
					return IDVR_PAR_ERROR_INVALID_FORMAT;
				}
			}
			
			//添加新的section
			tagSection *pCurSection = AddSection();

			if (pCurSection == NULL)
			{
				if (m_pSection != NULL)
				{
					free(m_pSection);
					m_pSection = NULL;
				}
				PAR_ERROR("Add Section Failed, Can not Allocate Memory\n");
				fclose(fp);
				return IDVR_ERROR_ALLOC;
			}
			m_nSectionCount++;
			pCurSection->m_strName = tSect.m_strName;
			pCurSection->m_pKeyHead = tSect.m_pKeyHead;
			pCurSection->m_nKeyCount = tSect.m_nKeyCount;
			pCurSection->m_nKeyAlloced = tSect.m_nKeyAlloced;
			tSect.m_strName = NULL;
			tSect.m_pKeyHead = NULL;
			tSect.m_nKeyCount = 0;
			tSect.m_nKeyAlloced = 0;
		}
		if (feof(fp))
		{
			if (0 != m_nSectionCount)
			{
				if((NULL == m_pSection) || (NULL == m_pSection[m_nSectionCount - 1].m_strName)
					|| (NULL == m_pSection[m_nSectionCount - 1].m_pKeyHead)
					|| (NULL == m_pSection[m_nSectionCount - 1].m_pKeyHead->m_strName)
					|| (NULL == m_pSection[m_nSectionCount - 1].m_pKeyHead->m_strValue))
				{
					fclose(fp);
					PAR_ERROR("Invalid File Format, [end] Section Error, At or Before Line = %d\n", m_nCurrentLine);
					return IDVR_PAR_ERROR_INVALID_FORMAT;
				}
				else
					{
						if (!(((0 == strcmp("end", m_pSection[m_nSectionCount - 1].m_strName))
							&& (m_pSection != NULL)
							&& (m_pSection[m_nSectionCount - 1].m_pKeyHead->m_strName != NULL)
							&& (0 == strcmp(m_pSection[m_nSectionCount - 1].m_pKeyHead->m_strName, "dvr")) 
							&& (0 == strcmp(m_pSection[m_nSectionCount - 1].m_pKeyHead->m_strValue, "idvr9000")))
							|| ((0 == strcmp(ParityEnd, m_pSection[m_nSectionCount - 1].m_strName))
							&& (m_pSection != NULL)
							&& (m_pSection[m_nSectionCount - 1].m_pKeyHead->m_strName != NULL)
							&& (0 == strcmp(m_pSection[m_nSectionCount - 1].m_pKeyHead->m_strName, ParityKey)) 
							&& (0 == strcmp(m_pSection[m_nSectionCount - 1].m_pKeyHead->m_strValue, ParityVal)))
							))
						{
							fclose(fp);
							PAR_ERROR("Invalid File Format, [end] Section Error, At or Before Line = %d\n", m_nCurrentLine);
							return IDVR_PAR_ERROR_INVALID_FORMAT;
						}
					}
			}
			else
			{
				//空文件,添加开始段
				tSect.m_strName = strdup(ParityStart);
				
				//添加新的key
				tagSection::tagKey *pKey = tSect.AddKey();
				if (NULL == pKey)
				{
					//写系统日志,分配内存失败
					PAR_ERROR("Add Key Failed, Can not Allocate Memory\n");
					return IDVR_ERROR_ALLOC;
				}
				pKey->m_strName = strdup(ParityKey);
				pKey->m_strValue = strdup(ParityVal);

				//添加新的section
				tagSection *pCurSection = AddSection();

				if (pCurSection == NULL)
				{
					if (m_pSection != NULL)
					{
						free(m_pSection);
						m_pSection = NULL;
					}
					PAR_ERROR("Add Section Failed, Can not Allocate Memory\n");
					fclose(fp);
					return IDVR_ERROR_ALLOC;
				}
				m_nSectionCount++;
				pCurSection->m_strName = tSect.m_strName;
				pCurSection->m_pKeyHead = tSect.m_pKeyHead;
				pCurSection->m_nKeyCount = tSect.m_nKeyCount;
				pCurSection->m_nKeyAlloced = tSect.m_nKeyAlloced;
				tSect.m_strName = NULL;
				tSect.m_pKeyHead = NULL;
				tSect.m_nKeyCount = 0;

				//添加结束段
				tSect.m_strName = strdup(ParityEnd);
				
				//添加新的key
				pKey = tSect.AddKey();
				if (NULL == pKey)
				{
					//写系统日志,分配内存失败
					PAR_ERROR("Add Key Failed, Can not Allocate Memory\n");
					fclose(fp);
					return IDVR_ERROR_ALLOC;
				}
				pKey->m_strName = strdup(ParityKey);
				pKey->m_strValue = strdup(ParityVal);

				//添加新的section
				pCurSection = AddSection();

				if (pCurSection == NULL)
				{
					if (m_pSection != NULL)
					{
						free(m_pSection);
						m_pSection = NULL;
					}
					PAR_ERROR("Add Section Failed, Can not Allocate Memory\n");
					fclose(fp);
					return IDVR_ERROR_ALLOC;
				}
				m_nSectionCount++;
				pCurSection->m_strName = tSect.m_strName;
				pCurSection->m_pKeyHead = tSect.m_pKeyHead;
				pCurSection->m_nKeyCount = tSect.m_nKeyCount;
				pCurSection->m_nKeyAlloced = tSect.m_nKeyAlloced;
				tSect.m_strName = NULL;
				tSect.m_pKeyHead = NULL;
				tSect.m_nKeyCount = 0;
				ret = IDVR_PAR_ERROR_EMPTY_FILE;
			}

			//读取文件结束
			PAR_DBG("End of File\n");
			break;
		}
	}

	if (m_strFile != NULL)
	{
		free(m_strFile);
		m_strFile = NULL;
	}

	if ((0 == ret) || (IDVR_PAR_ERROR_EMPTY_FILE == ret))
	{
		m_strFile = strdup(pszFileName);
	}
	
	fclose(fp);

	PAR_DBG("Load File, return %d\n", ret);
	return ret;
}

//暂未实现
int CConfig::LoadXmlFile(const char *pszFileName)
{
	return LoadIniFile(pszFileName);
/*
	int bInvalidFormat = false;
	bool bBadAlloc = false;

	if (NULL == pszFileName)
	{
		return IDVR_ERROR_PARAMETER;
	}

	tagSection *pCurSection = NULL;
	tagSection::tagKey *pCurKey = NULL;

#ifndef NDEBUG
	struct timeval tvNow;
	gettimeofday(&tvNow, NULL);
	printf("tBegin = %ld, %ld\n", tvNow.tv_sec, tvNow.tv_usec);
#endif
	xmlDocPtr pXmlDoc = xmlReadFile(pszFileName, NULL, 0);
#ifndef NDEBUG
	gettimeofday(&tvNow, NULL);
	printf("tEnd = %ld, %ld\n", tvNow.tv_sec, tvNow.tv_usec);
#endif

	//格式错误
	if (NULL == pXmlDoc)
	{
		return IDVR_PAR_ERROR_INVALID_FORMAT;
	}

	xmlNodePtr pXmlNode = xmlDocGetRootElement(pXmlDoc);
	//检查根节点
	if (0 != strcmp("appSettings", (const char *)pXmlNode->name))
	{
		xmlFreeDoc(pXmlDoc);
		return IDVR_PAR_ERROR_INVALID_FORMAT;
	}

	xmlNodePtr pSectionNode = pXmlNode->children;
	xmlAttrPtr pKeyNode = NULL;

	pSectionNode = pXmlNode->children;
	while (pSectionNode != NULL)
	{
		if (XML_ELEMENT_NODE != pSectionNode->type)
		{
			pSectionNode = pSectionNode->next;
			continue;
		}

		//添加新的section
		pCurSection = AddSection();
		if (NULL == pCurSection)
		{
			bBadAlloc = true;
			break;
		}

		//获取section名称
		pCurSection->m_strName = strdup((const char *)pSectionNode->name);
		PAR_DBG("[%s]\n", pCurSection->m_strName);
		m_nSectionCount++;

		pKeyNode = pSectionNode->properties;
		//转换获取所有的key
		while(pKeyNode != NULL)
		{
			//增加一个key
			pCurKey = pCurSection->AddKey();
			if (NULL == pCurKey)
			{
				bBadAlloc = true;
				break;
			}

			//读取key名称
			pCurKey->m_strName = strdup((const char *)pKeyNode->name);
			if (NULL == pCurKey->m_strName)
			{
				bBadAlloc = true;
				break;
			}
			
			//读取key值
			const char *pValue = (const char *)pKeyNode->children->content;
			pCurKey->m_strValue = strdup(pValue);
			PAR_DBG("%s = %s\n", pCurKey->m_strName, pCurKey->m_strValue);
			if (NULL == pCurKey->m_strValue)
			{
				bBadAlloc = true;
				break;
			}

			pKeyNode = pKeyNode->next;
		}

		if (bBadAlloc || bInvalidFormat)
		{
			break;
		}

		//读取下一个section
		pSectionNode = pSectionNode->next;
	}

	//出现内存分配错误
	if (bBadAlloc)
	{
		xmlFreeDoc(pXmlDoc);
		return IDVR_ERROR_ALLOC;
	}

	//检查配置是否合法
	if ((NULL == m_pSection) 
		|| (0 != strcmp("system", m_pSection[0].m_strName))//第一个section为"system"
		|| (NULL == m_pSection[0].m_pKeyHead) 
		|| (0 != strcmp("dvr", m_pSection[0].m_pKeyHead->m_strName)) //idvr=idvr9000
		|| (0 != strcmp("idvr9000", m_pSection[0].m_pKeyHead->m_strValue))
		|| (0 != strcmp("end", m_pSection[m_nSectionCount - 1].m_strName))//最后一个section为"end"
		|| (0 != strcmp("dvr", m_pSection[m_nSectionCount - 1].m_pKeyHead->m_strName))//idvr=idvr9000
		|| (0 != strcmp("idvr9000", m_pSection[m_nSectionCount - 1].m_pKeyHead->m_strValue)))
	{
		printf("0 = {%s, %s, %s}, $={%s, %s, %s}\n"
			, m_pSection[0].m_strName, m_pSection[0].m_pKeyHead->m_strName, m_pSection[0].m_pKeyHead->m_strValue
			, m_pSection[m_nSectionCount - 1].m_strName, m_pSection[m_nSectionCount - 1].m_pKeyHead->m_strName
			, m_pSection[m_nSectionCount - 1].m_pKeyHead->m_strValue);
		bInvalidFormat = true;
	}
	
	if (bInvalidFormat)
	{
		xmlFreeDoc(pXmlDoc);
		return IDVR_PAR_ERROR_INVALID_FORMAT;
	}
	
	m_strFile = strdup(pszFileName);
	xmlFreeDoc(pXmlDoc);
*/
	return (int)0;
}

int CConfig::SaveIniFile(const char *pszFileName)
{
	if ((NULL == pszFileName) || (0 == strlen(pszFileName)))
	{
		//写调试日志,错误的参数
		PAR_ERROR("Invalid Parameter, pszFileName = NULL");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	bool bUsbFile = false;
	if (strstr(pszFileName, "/usb/sd") == pszFileName)
	{
		bUsbFile = true;
	}
	
	int i;
	FILE *fp;
	int ret = 0;

	fp = fopen(pszFileName, "w+");
	if (NULL == fp)
	{
		//写系统日志,文件打开失败
		PAR_ERROR("Open File %s Failed\n", pszFileName);
		return bUsbFile ? IDVR_ERROR_USB_OPEN_FILE : IDVR_ERROR_OPEN_FILE;
	}

	for (i = 0; i < m_nSectionCount; i++)
	{
		tagSection *pSect = m_pSection + i;
		int j;

		ret = fprintf(fp, "\n[%s]\n", pSect->m_strName);
		if (ret < 0)
		{
			fclose(fp);
			unlink(pszFileName);
			return bUsbFile ? IDVR_ERROR_USB_WRITE_FILE : IDVR_ERROR_WRITE_FILE;
		}

		for (j = 0; j < pSect->m_nKeyCount; j++)
		{
			ret = fprintf(fp, "%s=%s\n", pSect->m_pKeyHead[j].m_strName, pSect->m_pKeyHead[j].m_strValue);
			if (ret < 0)
			{
				fclose(fp);
				unlink(pszFileName);
				return bUsbFile ? IDVR_ERROR_USB_WRITE_FILE : IDVR_ERROR_WRITE_FILE;
			}
		}
	}

	fflush(fp);
	fclose(fp);
	return (int) 0;
}

//暂未实现
int CConfig::SaveXmlFile(const char *pszFileName)
{
	return SaveIniFile(pszFileName);
/*
	if ((NULL == pszFileName) || (0 == strlen(pszFileName)))
	{
		//写调试日志,错误的参数
		PAR_ERROR("Invalid Parameter, pszFileName = NULL");
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}

	bool bUsbFile = false;
	if (strstr(pszFileName, "/usb/sd") == pszFileName)
	{
		bUsbFile = true;
	}

	int i;
	FILE *fp;
	int ret = 0;

	fp = fopen(pszFileName, "w+");
	if (NULL == fp)
	{
		//写系统日志,文件打开失败
		PAR_ERROR("Open File %s Failed\n", pszFileName);
		return bUsbFile ? IDVR_ERROR_USB_OPEN_FILE : IDVR_ERROR_OPEN_FILE;
	}

	ret = fprintf(fp, "<?xml version=\"1.0\"?>\n<appSettings>\n");
	if (ret < 0)
	{
		fclose(fp);
		unlink(pszFileName);
		return bUsbFile ? IDVR_ERROR_USB_WRITE_FILE : IDVR_ERROR_WRITE_FILE;
	}

	for (i = 0; i < m_nSectionCount; i++)
	{
		tagSection *pSect = m_pSection + i;
		int j;

		ret = fprintf(fp, "<%s\n", pSect->m_strName);
		if (ret < 0)
		{
			fclose(fp);
			unlink(pszFileName);
			return bUsbFile ? IDVR_ERROR_USB_WRITE_FILE : IDVR_ERROR_WRITE_FILE;
		}

		for (j = 0; j < pSect->m_nKeyCount; j++)
		{
			ret = fprintf(fp, "\t%s=\"%s\"\n", pSect->m_pKeyHead[j].m_strName, pSect->m_pKeyHead[j].m_strValue, pSect->m_pKeyHead[j].m_strName);
			if (ret < 0)
			{
				fclose(fp);
				unlink(pszFileName);
				return bUsbFile ? IDVR_ERROR_USB_WRITE_FILE : IDVR_ERROR_WRITE_FILE;
			}
		}

		ret = fprintf(fp, "\t/>\n");
		if (ret < 0)
		{
			fclose(fp);
			unlink(pszFileName);
			return bUsbFile ? IDVR_ERROR_USB_WRITE_FILE : IDVR_ERROR_WRITE_FILE;
		}
	}

	ret = fprintf(fp, "</appSettings>");
	if (ret < 0)
	{
		fclose(fp);
		unlink(pszFileName);
		return bUsbFile ? IDVR_ERROR_USB_WRITE_FILE : IDVR_ERROR_WRITE_FILE;
	}
	fflush(fp);
	fclose(fp);
	return (int) 0;
*/
}

int CConfig::ReadIniSection(FILE * fp, tagSection * pSect)
{
	if ((NULL == fp) || (NULL == pSect))
	{
		return IDVR_ERROR_PARAMETER;	//参数错误
	}
	//cout << "Read Section" << endl;
	char szBuff[INI_LINE_LENGTH + 1 + 1];

	while (1)
	{
		int bufflen;

		if (!fgets(szBuff, INI_LINE_LENGTH, fp))
		{
			if (feof(fp))
			{
				PAR_DBG("End of File\n");
				return 0;		//读取文件结束
			}
			else				//文件IO错误
			{
				PAR_ERROR("ERR: IO Fault\n");	//文件没有结束,但读不到数据
				return IDVR_ERROR_CONFIGFILE;
			}
		}
		m_nCurrentLine++;
		bufflen = strlen(szBuff);

		if (NULL == pSect->m_strName)	//读取section
		{
			int ret = GetIniSectionName(&(pSect->m_strName), szBuff);

			if (ret != 0)
			{
				return ret;
			}

			if (NULL == pSect->m_strName)	//空行或者注释行
			{
				continue;
			}
			//cout << "[" << pSect->m_strName << "]" << endl;
		}
		else					//读取key
		{
			int ret;
			char *key, *value;

			ret = GetIniKey(szBuff, &key, &value);
			if (ret < 0)		//没有key
			{
				continue;
			}
			else if (1 == ret)	//发现新的section
			{
				m_nCurrentLine--;
				fseek(fp, -bufflen, SEEK_CUR);
				break;
			}
			else if (0 != ret)
			{
				return ret;
			}

			char *pKeyName = strdup(key);

			if (NULL == pKeyName)
			{
				PAR_ERROR("Add Key Failed, Can not Allocate Memory\n");
				return IDVR_ERROR_ALLOC;	//分配内存失败
			}
			char *pKeyValue = strdup(value);

			if (NULL == pKeyValue)
			{
				free(pKeyName);
				pKeyName = NULL;
				PAR_ERROR("Add Key Failed, Can not Allocate Memory\n");
				return IDVR_ERROR_ALLOC;	//分配内存失败
			}
			//添加新的key
			CConfig::tagSection::tagKey * pKey = pSect->AddKey();
			if (NULL == pKey)
			{
				//写系统日志,分配内存失败
				free(pKeyName);
				free(pKeyValue);
				pKeyName = NULL;
				pKeyValue = NULL;
				PAR_ERROR("Add Key Failed, Can not Allocate Memory\n");
				return IDVR_ERROR_ALLOC;
			}
			pKey->m_strName = pKeyName;
			pKey->m_strValue = pKeyValue;
			//cout << pKey->m_strName << "=" << pKey->m_strValue << endl;
		}
	}
	return 0;
}

int CConfig::GetIniSectionName(char **strSectionName, char *szBuf)
{
	char *current = NULL;
	char *tail = NULL;
	char *name = NULL;

	if (!szBuf)
		return 0;

	current = szBuf;

	//忽略空白字符
	while (*current == ' ' || *current == '\t')
	{
		current++;
	}

	//忽略注释
	if (*current == ';' || *current == '#')
	{
		return 0;
	}

	//碰到section名称开始符
	if (*current++ == '[')
	{
		while (*current == ' ' || *current == '\t')
		{
			current++;
		}
	}
	//检测是否是空白行
	else if (('\0' == *current) || ('\n' == *current) || ('\r' == *current))
	{
		return 0;
	}
	else
	{
		PAR_ERROR("Invalid File Format, \"[\" Not Found In Section Line, Error Line = %d\n", m_nCurrentLine);
		return IDVR_PAR_ERROR_INVALID_FORMAT;	//文件格式不对
	}

	//获取section名称
	name = tail = current;
	while (*tail != ']' && *tail != '\n' && *tail != ';' && *tail != '#' && *tail != '\0')
	{
		tail++;
	}
	//section名称没有以]结尾
	if (*tail != ']')
	{
		PAR_ERROR("Invalid File Format, No \"]\" Follow SectionName, Error Line = %d\n", m_nCurrentLine);
		return IDVR_PAR_ERROR_INVALID_FORMAT;
	}

	*tail = '\0';

	char *pValid = (++tail);
	while (*pValid != '\0' && *pValid != ';' && *pValid != '#')
	{		
		if (*pValid != ' ' && *pValid != '\t' && *pValid != '\n' && *pValid != '\r')
		{
			PAR_ERROR("Invalid File Format, Invalid Character 0x%x Follow \"]\", Error Line = %d\n", *pValid, m_nCurrentLine);
			return IDVR_PAR_ERROR_INVALID_FORMAT;
		}
		pValid++;
	}	
	
	//获取section名成功
	*tail = '\0';
	tail--;
	while (*tail == ' ' || *tail == '\t')
	{
		*tail = '\0';
		tail--;
	}

	//section名称为空
	int len = (tail - name + 1);
	if ((len <= 0) || (len > INI_MAX_SECTION_NAME_LENGTH))
	{
		PAR_ERROR("Invalid File Format, SectionName Length > %d or = 0, Error Line = %d\n", INI_MAX_SECTION_NAME_LENGTH, m_nCurrentLine);
		return IDVR_PAR_ERROR_INVALID_FORMAT;
	}
	
	*strSectionName = strdup(name);
	if (NULL == *strSectionName)
	{
		return IDVR_ERROR_ALLOC;
	}
	
	return 0;
}

int CConfig::GetIniKey(char *szBuf, char **pszKeyName, char **pszKeyValue)
{
	char *current = NULL;
	char *tail = NULL;
	char *value = NULL;

	if (!szBuf)
		return -1;

	current = szBuf;

	//忽略空白字符
	while (*current == ' ' || *current == '\t')
	{
		current++;
	}

	//注释
	if (*current == ';' || *current == '#')
	{
		return -1;
	}

	//遇到新的section
	if (*current == '[')
	{
		return 1;
	}

	//空白行
	if (*current == '\n' || *current == '\r' || *current == '\0')
	{
		return -1;
	}

	//key名称为空
	if (*current == '=')
	{
		PAR_ERROR("Invalid File Format, KeyName Is NULL, Error Line = %d\n", m_nCurrentLine);
		return IDVR_PAR_ERROR_INVALID_FORMAT;
	}

	tail = current;
	while (*tail != '=' && *tail != '\r' && *tail != '\n' && *tail != ';' && *tail != '#' && *tail != '\0')
	{
		tail++;
	}

	if (*tail != '=')
	{
		PAR_ERROR("Invalid File Format, No \"=\" operator Follow KeyName, Error Line = %d\n", m_nCurrentLine);
		return IDVR_PAR_ERROR_INVALID_FORMAT;
	}
	
	value = tail + 1;

	//去掉key名称后面的空白字符
	*tail-- = '\0';
	while (*tail == ' ' || *tail == '\t')
	{
		*tail = '\0';
		tail--;
	}

	int len = (tail - current + 1);
	if ((len > INI_MAX_KEY_NAME_LENGTH) || (len <= 0))
	{
		PAR_ERROR("Invalid File Format, KeyName Lenth > %d, Error Line = %d\n", INI_MAX_KEY_NAME_LENGTH, m_nCurrentLine);
		return IDVR_PAR_ERROR_INVALID_FORMAT;
	}

	//获取key值
	tail = value;
	while (*tail != '\r' && *tail != '\n' && *tail != '\0')
	{
		tail++;
	}
	*tail = '\0';

	len = (tail - value);
	if ((len > INI_MAX_KEY_VALUE_LENGTH) || (len < 0))
	{
		PAR_ERROR("Invalid File Format, KeyValue Lenth > %d, Error Line = %d\n", INI_MAX_KEY_VALUE_LENGTH, m_nCurrentLine);
		return IDVR_PAR_ERROR_INVALID_FORMAT;
	}

	if (pszKeyName)
	{
		*pszKeyName = current;
	}
	if (pszKeyValue)
	{
		*pszKeyValue = value;
	}

	return 0;
}

int CConfig::FreeMemory()
{
	int nResult = 0;

	//释放每个section所占的资源
	if (m_pSection != NULL)
	{
		int i = 0;
		tagSection *pSection = m_pSection;

		for (i = 0; i < m_nSectionCount; i++)
		{
			pSection->~tagSection();
			pSection++;
		}
		free(m_pSection);
		m_pSection = NULL;
		nResult = 0;
	}

	m_nSectionCount = 0;
	return nResult;
}

CConfig::tagSection * CConfig::SearchSection(const char *pszSection)
{
	tagSection *pSect = NULL;
	int i = 0;

	if (NULL == pszSection)
	{
		return NULL;
	}
	for (i = 0; i < m_nSectionCount; i++)
	{
		pSect = m_pSection + i;
		if (0 == strcmp(pSect->m_strName, pszSection))
		{
			//cout << pSect->m_strName << endl;
			break;
		}
	}
	if (i >= m_nSectionCount)
	{
		pSect = NULL;
	}
	return pSect;
}

int CConfig::BackupFile(const char *pszFileName)
{
	if (NULL == pszFileName)
	{
		return IDVR_ERROR_PARAMETER_INVALID_POINTER;
	}
	char strBakFileName[PATH_MAX + 1] = { 0 };

	strncpy(strBakFileName, pszFileName, sizeof(strBakFileName) - 1);
	strncat(strBakFileName, "-bak", sizeof(strBakFileName) - strlen(strBakFileName) - 1);

	int nRetry = 0;
	while(0 != (rename(pszFileName, strBakFileName)) && nRetry++ < 20)
	{
		usleep(1000);
	}

	return (int) 0;
}

CConfig::tagSection * CConfig::AddSection()
{
	int max_sect_nr = DEF_SEC_NUM;

	if (m_nSectionCount != 0)
	{
		if ((m_nSectionCount+1) > m_nSectionAlloced)
		{
			max_sect_nr = m_nSectionAlloced + ADD_SEC_NUM;	//需要重新分配内存
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
			m_pSection = (tagSection *) realloc(m_pSection, sizeof (tagSection) * max_sect_nr);
#pragma GCC diagnostic pop
			m_nSectionAlloced = max_sect_nr;
		}
		else
		{
			// 无需分配内存
		}
	}
	else						//空section
	{
		//一般应为NULL
		if (m_pSection != NULL)
		{
			free(m_pSection);
			m_pSection = NULL;
		}
		//初始分配的DEF_SEC_NUM个section的空间
		m_pSection = (tagSection *) malloc(sizeof (tagSection) * max_sect_nr /*DEF_SEC_NUM */ );
		m_nSectionAlloced = max_sect_nr;
	}
	if (NULL == m_pSection)//分配空间失败
	{
		return NULL;
	}
	tagSection *pSection = m_pSection + m_nSectionCount;

	//设置初始值,避免错误的读写内存
	pSection->m_strName = NULL;
	pSection->m_nKeyCount = 0;
	pSection->m_pKeyHead = NULL;
	//cout << "Section Count " << m_nSectionCount << endl;
	return pSection;
}

CConfig::tagSection * CConfig::InsertSection()
{
	int max_sect_nr = DEF_SEC_NUM;

	if (m_nSectionCount != 0)
	{
		//计算是否需要增加内存空间
		if ((m_nSectionCount+1) > m_nSectionAlloced)
		{
			max_sect_nr = m_nSectionAlloced + ADD_SEC_NUM;	//需要重新分配内存
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
			m_pSection = (tagSection *) realloc(m_pSection, sizeof (tagSection) * max_sect_nr);
#pragma GCC diagnostic pop
			m_nSectionAlloced = max_sect_nr;
		}
		else
		{
			// 无需分配内存
		}
	}
	else//没有起始节点，出错
	{
		return NULL;
	}

	tagSection *pEndSection = m_pSection + m_nSectionCount;
	tagSection *pSection = m_pSection + m_nSectionCount - 1;

	//设置初始值,避免错误的读写内存
	pEndSection->m_strName = pSection->m_strName;
	pEndSection->m_nKeyCount = pSection->m_nKeyCount;
	pEndSection->m_pKeyHead = pSection->m_pKeyHead;
	pEndSection->m_nKeyAlloced = pSection->m_nKeyAlloced;

	pSection->m_strName = NULL;
	pSection->m_nKeyCount = 0;
	pSection->m_pKeyHead = NULL;
	pSection->m_nKeyAlloced = 0;
	//cout << "Section Count " << m_nSectionCount << endl;
	return pSection;
}

int CConfig::IsFileExsit(const char *pszFileName)
{
	//检查文件是否存在
	return (0 == access(pszFileName, F_OK));
}

void CConfig::Print()
{
	int i;

	for (i = 0; i < m_nSectionCount; i++)
	{
		tagSection *pSect = m_pSection + i;
		int j;

		printf ("\n[%s]\n", pSect->m_strName);
		for (j = 0; j < pSect->m_nKeyCount; j++)
		{
			  printf ("%s=%s\n", 
			      pSect->m_pKeyHead[j].m_strName, 
			      pSect->m_pKeyHead[j].m_strValue);
		}
	}
	PAR_DBG("nSectionCount = %d\n", m_nSectionCount);
	
}

int CConfig::ReplaceFileCfg(const char *pach)
{
	int i = 0;
	int j = 0;
	Lock();
    
	char boardIDSection[24] = {0};
	int nBoardID = 0;
	stuShmAuthInfo stShmAuthInfo = {0};

	CConfig *pCfg = CConfig::NewInstance();
    
	int Ret =  pCfg->LoadFile(pach);
	if (0 != Ret)
	{
		printf("[%s-%d][ERR] Load File  Fail\n", __FILE__, __LINE__);
		pCfg->ReleaseInstance(pCfg); 
		return -1;
	}

	/*
	1.这里由于SDK未初始化完成，不能调用ZWSys_GetBoardInfo来获取版型， 需调用GetDefaultBoardID获取版型;
	2.多个版型是同一个Sensor时，会默认获取到第一个版型，由于需要根据授权来判断IPG-8730PHS与IPG-8730PHS-AI版型，需要从授权码里面获取版型；
	*/
    Authorize_GetAuthDevInfo(&stShmAuthInfo);
	if (AUTH_SUCCESS == stShmAuthInfo.dwAuthVerifyErrno)
	{
		snprintf(boardIDSection, sizeof(boardIDSection), "BoardID-%lx", stShmAuthInfo.dwBoardID);
	}
	else
	{
		nBoardID = GetDefaultBoardID(); 
    	snprintf(boardIDSection, sizeof(boardIDSection), "BoardID-%x", nBoardID);
	}

	for (i = 0; i < m_nSectionCount; i++)
	{
		tagSection *pSect = m_pSection + i;	
		for (j = 0; j < pSect->m_nKeyCount; j++)
		{
			if (((0 != strncmp(pSect->m_pKeyHead[j].m_strName ,"dvr", 3)) && 
				(0 != strncmp(pSect->m_pKeyHead[j].m_strValue ,"idvr9000", 8))))
			{
				if (0 == strncmp(pSect->m_pKeyHead[j].m_strName ,"UserPassword", 12))
				{
					continue;
				}
				//printf("ljx111 pSect->m_pKeyHead[j].m_strName =%s, pSect->m_pKeyHead[j].m_strValue =%s \n",pSect->m_pKeyHead[j].m_strName,pSect->m_pKeyHead[j].m_strValue);
                if (0 == strcmp(pSect->m_strName, "IR_CUT_cfg"))
                {
                	if (0 != strcmp(pSect->m_pKeyHead[j].m_strName, "nVarWhiteLightOnThreshold") 
						&& 0 != strcmp(pSect->m_pKeyHead[j].m_strName, "nVarWhiteLightOffThreshold")
						&& 0 != strcmp(pSect->m_pKeyHead[j].m_strName, "nVarWhiteBrightness")
						&& 0 != strcmp(pSect->m_pKeyHead[j].m_strName, "InfraredBrightness"))
                	{
                    	pCfg->SetValue(boardIDSection, pSect->m_pKeyHead[j].m_strName, pSect->m_pKeyHead[j].m_strValue);
					}
                }
                pCfg->SetValue(pSect->m_strName, pSect->m_pKeyHead[j].m_strName, pSect->m_pKeyHead[j].m_strValue);
			}
		}
	}
	pCfg->SaveFileReal(1);
	pCfg->ReleaseInstance(pCfg); 
	Unlock();
	return 0;
}

bool CConfig::CheckSectionExist(const char *sec)
{
    bool bRet = false;

    if ((NULL != sec) && (NULL != SearchSection(sec)))
    {
        bRet = true;
    }
    
    return bRet;
}

bool CConfig::CheckKeyExist(const char *sec, const char *key)
{
    bool bRet = false;

    if ((NULL != sec) && (NULL != key))
    {
        tagSection *pSect = SearchSection(sec);
        if (NULL != pSect)
        {
            tagSection::tagKey *pKey = pSect->SearchKey(key);
            if (NULL != pKey)
            {
                bRet = true;
            }
        }
    }

    return bRet;
}

inline int CConfig::Lock()
{
	return pthread_mutex_lock(&m_mtxConfig);
}

inline int CConfig::Unlock()
{
	return pthread_mutex_unlock(&m_mtxConfig);
}

inline int CConfig::LockSave()
{
	return pthread_mutex_lock(&m_mtxSaveFile);
}

inline int CConfig::UnlockSave()
{
	return pthread_mutex_unlock(&m_mtxSaveFile);
}

