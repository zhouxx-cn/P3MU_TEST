
/*!
*\file   CLogManager.h
*\brief  日志管理类定义
*\author yuk@higheasyrd.com
*\date	  2009/04/28
*\version 0.1
*/

#include "CLogManager.h"
#include "OPL/CLogDao.h"
#include "idvr_errno.h"
#include "OPLTrace.h"
#include "QueryLog.h"
#include "mysystem.h"
#include <unistd.h>
#include "../PAR/CPubFunc.h"
#include "common_pub.h"

CLogManager *CLogManager::m_pInstance = NULL;


/*@ preif 因为配置分区满了后，sqlite加载了配置分区的数据库后到
 *@让程序崩溃,为了让配置分区满了后程序依然能跑，所以加载
 *@ 前需要判断分区是否满，如果满了就将数据库拷贝到内存，
 *@然后再加载
 *@ return 0 分区未满
 *@ return 1 分区已满
 */
static int ISCfgPartitionFull()
{
	int ret = 0;
	char command[64] = {0};
	char testFile[] = "/mnt/mtd/config/.checkFull";

	//检查配置分区挂载点
	ret = IsStringInCommand("mount", "config");
	if(1 == ret)
	{
		//检查分区是否满
		snprintf(command,sizeof(command), "touch %s", testFile);
		mysystem(command);

		if(0 == access(testFile, 0))
		{
			//分区未满
			memset(command, 0, sizeof(command));
			snprintf(command,sizeof(command), "rm %s", testFile);
			mysystem(command);	
			
			return 0;
		}
		else
			{
				return 1;
			}
	}
	else
		{
			//配置分区未挂载或者挂载点不是config，按默认处理
			return 0;
		}
}

//##ModelId=499E6049005C
CLogManager::CLogManager()
{
	INIT_DVR_LIST_HEAD(&m_listQuery);
	// TODO: Add your specialized code here.
}

//##ModelId=499E604B039E
CLogManager::~CLogManager()
{
	dvr_list_head *pos = NULL;

	dvr_list_head *next = NULL;

	dvr_list_for_each_safe(pos, next, &m_listQuery)
	{
		dvr_list_del(pos);
		OPL_DBG("Delete Query\n");
		CLogDao *pDao = CLogDao::GetInstance();

		pDao->DestroyQuery(((QueryNode *) pos)->hQuery);
		delete(QueryParam *) (((QueryNode *) pos)->hQuery);
		delete(QueryNode *) pos;
	}

	CLogDao *pDao = CLogDao::GetInstance();

	CLogDao::DestroyInstance(pDao);
	pDao = NULL;
	// TODO: Add your specialized code here.
}

//##ModelId=49BA0AEC002E
void CLogManager::OPL_DestroyInstance(CLogManager * pInstance)
{
	if (pInstance != NULL)
	{
		delete pInstance;
	}
	// TODO: Add your specialized code here.
}

//##ModelId=49BA0A6600BB
CLogManager *CLogManager::OPL_GetInstance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new CLogManager();

		int ret = 0;

		if (m_pInstance != NULL)
		{
			CLogDao *pDao = CLogDao::GetInstance();

			ret = ISCfgPartitionFull();
			if(0 == ret)
				ret = pDao->BindDb("/mnt/mtd/config/log.db");
			else
			{
				char command[64] = {0};
				//如果分区满则将数据库拷贝到内存
				memset(command, 0, sizeof(command));
				snprintf(command,sizeof(command), "cp /mnt/mtd/config/log.db /tmp/app/log.db");
				mysystem(command);
				ret = pDao->BindDb("/tmp/app/log.db");
			}

			if (ret != 0)
			{
				CLogDao::DestroyInstance(pDao);
				delete m_pInstance;

				m_pInstance = NULL;
			}
		}
	}

	// TODO: Add your specialized code here.
	return (CLogManager *) m_pInstance;
}

//##ModelId=49B4B2D402AF
int CLogManager::OPL_AddLog(int nLogType, int nDetail, int nAlertChannel, unsigned long long u64ActionChannelFlag, unsigned long dwLocalOrRemote,
							char *strUsername)
{
	CLogDao *pDao = CLogDao::GetInstance();

	tagLog objLog;

	objLog.dwEventType = nLogType;
	objLog.dwEventDetail = nDetail;
	objLog.dwChannel = nAlertChannel;
	objLog.u64RecordChannelFlag = u64ActionChannelFlag;
	objLog.dwRemote = dwLocalOrRemote;
	if (NULL != strUsername)
	{
		strncpy(objLog.strUsername, strUsername, sizeof(objLog.strUsername) - 1);
	}
	else
	{
		memset(objLog.strUsername, 0, MAX_USERNAME_LEN + 4);
	}

	pDao->InsertLog(objLog);
	return 0;
}

//##ModelId=49B4B74A01D4
int CLogManager::OPL_QueryPage(HANDLE hQueryLog, struct tagLog *pLogBuf, int nLogBufLen, int nPageNum, int *nLogItem)
{
	if ((NULL == hQueryLog) || (pLogBuf == NULL) || (nLogBufLen <= 0) || (nPageNum < 0) || (NULL == nLogItem))
	{
		OPL_ERROR("param error, hQueryLog=%p, pLogBuf=%p, nLogBufLen = %d, nPageNum = %d, nLogItem = %p\n",
				  hQueryLog, pLogBuf, nLogBufLen, nPageNum, nLogItem);
		return IDVR_ERROR_PARAMETER;
	}

	dvr_list_head *pos = NULL;

	dvr_list_head *next = NULL;

	dvr_list_for_each_safe(pos, next, &m_listQuery)
	{
		if (((QueryNode *) pos)->hQuery == hQueryLog)
		{
			OPL_DBG("Search Query Node\n");
			break;
		}
	}

	if ((&m_listQuery) == pos)
	{
		OPL_ERROR("Invalid Query or Query has alread been stoped\n");
		return IDVR_ERROR_PARAMETER;
	}
	CLogDao *pDao = CLogDao::GetInstance();

	OPL_DBG("nPageNum = %d\n", nPageNum);
	*nLogItem = pDao->GetLogs(hQueryLog, pLogBuf, nLogBufLen, nPageNum);
	OPL_DBG("*nLogItem=%d\n", *nLogItem);
	return 0;
}

//##ModelId=49B4B739000F
int CLogManager::OPL_QueryStop(HANDLE hQueryLog)
{
	// TODO: Add your specialized code here.
	CLogDao *pDao = CLogDao::GetInstance();

	if (hQueryLog != NULL)
	{
		dvr_list_head *pos = NULL;

		dvr_list_head *next = NULL;

		dvr_list_for_each_safe(pos, next, &m_listQuery)
		{
			OPL_DBG("Search Query Node\n");
			if (((QueryNode *) pos)->hQuery == hQueryLog)
			{
				dvr_list_del(pos);
				pDao->DestroyQuery(hQueryLog);
				delete(QueryParam *) hQueryLog;
				delete(QueryNode *) pos;
				OPL_DBG("Stop Query\n");
				break;
			}
		}

		if ((&m_listQuery) == pos)
		{
			OPL_INFO("Invalid Query Handle or Query has alread been stoped\n");
		}
	}

	return (int) 0;
}

//##ModelId=49B4B4A2001F
int CLogManager::OPL_QueryStart(int nType, time_t tStartTime, time_t tStopTime, int nPageItem, CBQueryLogCallback pQueryCallback, HANDLE * pHandle)
{
	OPL_DBG("%s line %d\n", __FILE__, __LINE__);
	if ((NULL == pQueryCallback) || (tStopTime < tStartTime) || (NULL == pHandle))
	{
		return IDVR_ERROR_PARAMETER;
	}

	QueryParam *hQuery = new QueryParam();

	if (NULL == hQuery)
	{
		return IDVR_ERROR_ALLOC;
	}

	hQuery->nQueryMode = QUERY_BY_TYPE_AND_TIME;
	hQuery->nLogType = nType;
	hQuery->tStartTime = tStartTime;
	hQuery->tStopTime = tStopTime;
	hQuery->nPageItem = nPageItem;
	hQuery->pData = NULL;
	hQuery->cbQuery = pQueryCallback;
	hQuery->pLogManager = this;
	*pHandle = (HANDLE) hQuery;

	pthread_t tPthread;

	int ret = pthread_create(&tPthread, NULL, QueryLog, (void *) hQuery);

	if (0 != ret)
	{
		sem_destroy(&hQuery->semQuery1);
		sem_destroy(&hQuery->semQuery2);
		OPL_INFO("Create Query Log Thread Failed\n");
		delete hQuery;

		hQuery = NULL;
		ret = IDVR_ERROR_CREATETHREAD;
		return ret;
	}
	else
	{
		pthread_detach(tPthread);
	}

	sem_wait(&hQuery->semQuery1);
	ret = hQuery->nRetValue;
	if (0 != ret)
	{
		*pHandle = NULL;
	}

	sem_post(&hQuery->semQuery2);

	// TODO: Add your specialized code here.
	return (int) ret;
}


void *CLogManager::QueryLog(void *arg)
{
	QueryParam *hQuery = (QueryParam *) arg;

	CLogDao *pDao = CLogDao::GetInstance();

	CLogManager *pManager = (CLogManager *) hQuery->pLogManager;

	QueryNode *pNode = new QueryNode();

	if ((NULL == pNode) || (NULL == pDao))
	{
		hQuery->nRetValue = IDVR_ERROR_ALLOC;
		sem_post(&hQuery->semQuery1);
		sem_wait(&hQuery->semQuery2);
		sem_destroy(&hQuery->semQuery1);
		sem_destroy(&hQuery->semQuery2);
		delete hQuery;

		if (NULL == pNode)
		{
			OPL_ERROR("Create Query Node failed, More Memory Needed\n");
		}
		else
		{
			OPL_ERROR("Get pDao Instance failed\n");
		}

		return NULL;
	}

	hQuery->nRetValue = 0;
	sem_post(&hQuery->semQuery1);
	sem_wait(&hQuery->semQuery2);
	sem_destroy(&hQuery->semQuery1);
	sem_destroy(&hQuery->semQuery2);
	pNode->hQuery = (HANDLE) hQuery;
	dvr_list_add((dvr_list_head *) pNode, &pManager->m_listQuery);
	int ret = pDao->CreateQuery(hQuery);

	if (ret < 0)
	{
		hQuery->cbQuery(hQuery, -1);
		OPL_ERROR("pDao Create Query failed, ret = %d\n", ret);
	}
	else
	{
		hQuery->cbQuery(hQuery, ret);
	}

	return NULL;
}
