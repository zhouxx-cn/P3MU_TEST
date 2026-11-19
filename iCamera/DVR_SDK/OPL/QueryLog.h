#ifndef __QUERYLOG_H_
#define __QUERYLOG_H_

#include <semaphore.h>
#include "PUB/HANDLE.h"
#include "CBQueryLogCallback.h"
#include "DVR_LIST.h"

enum QUERY_MODE
{
	QUERY_BY_TYPE,
	QUERY_BY_TYPE_AND_TIME,
};

struct QueryParam
{
	QUERY_MODE nQueryMode;
	int nLogType;
	int nPageItem;
	time_t tStartTime;
	time_t tStopTime;
	void *pData;
	int nCount;
	void *pLogManager;
	CBQueryLogCallback cbQuery;
	sem_t semQuery1;
	sem_t semQuery2;
	int nRetValue;

	  QueryParam()
	{
		sem_init(&semQuery1, 0, 0);
		sem_init(&semQuery2, 0, 0);
	};

	~QueryParam()
	{
	};
};

struct QueryNode
{
	dvr_list_head m_tHead;
	HANDLE hQuery;
};

#endif
