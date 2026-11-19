
/*!
*\file		MPU_CIU_LogQuery.cpp
*\brief	MPU模块，远程日志查询的实现
*\author	lbj
*\date		2009/08/24

功能：
-#	
-#	日志查询

说明：
-#	
*/

#include "MPU_CIU_global.h"
#include "tagLog.h"
#include "MPU_CIU_LogQuery.h"
#include "MPU_CIU_Login.h"

/* 消除本文件中的snprintf截断警告 */
#pragma GCC diagnostic ignored "-Wformat-truncation"

int LogQueryCallback(void *pParam, int iTotalNum)
{
	MPU_DBG("hIndex %d, iTotalNum %d\n", (int) pParam, iTotalNum);
	PT_REMOTEQUEERY ptLogquery = NULL;

	for (int i = 0; i < LOGINUSER_COUNT; i++)
	{
		ptLogquery = CIU_GetLogQueryParam(i);

		if (NULL != ptLogquery)
		{
			MPU_DBG("local hIndex %d\n", (int) ptLogquery->hIndex);

			if ((HANDLE) pParam == ptLogquery->hIndex)
			{
				ptLogquery->iLogQueryNum = 0;
				ptLogquery->iLogQueryTotalNum = iTotalNum;

				MPU_DBG("set  iLogQueryTotalNum = %d ok\n", iTotalNum);
				break;
			}
		}
	}
	return 0;
}

eHistoryLogMajorType MajorType_LocToNet(int iMajorType)
{
	eHistoryLogMajorType eNetType = eHistoryLogSysOperation;

	if (iMajorType >= EDVR_EVENT_OPT && iMajorType <= EDVR_EVENT_ALL)
	{
		eNetType = (eHistoryLogMajorType) (eHistoryLogMajorTypeBegin + 1 + (iMajorType - EDVR_EVENT_OPT));
		if (eNetType <= eHistoryLogMajorTypeBegin || eNetType >= eHistoryLogMajorTypeEnd)
		{
			MPU_ERR("eNetType %d erro\n", eNetType);
			eNetType = eHistoryLogMajorAll;
		}
	}
	else
		MPU_ERR("iMajorType %d erro\n", iMajorType);

	return eNetType;

}

int MajorType_NetToLoc(eHistoryLogMajorType eMajorType)
{
	int iLocType = EDVR_EVENT_OPT;

	if (eMajorType >= eHistoryLogMajorTypeBegin && eMajorType <= eHistoryLogMajorTypeEnd)
	{
		iLocType = EDVR_EVENT_OPT + (eMajorType - eHistoryLogMajorTypeBegin) - 1;
		MPU_DBG("MajorType (iLocType)  %d\n", iLocType);
		if (iLocType < EDVR_EVENT_OPT || iLocType > EDVR_EVENT_ALL)
		{
			MPU_ERR("iLocType %d erro\n", iLocType);
			iLocType = EDVR_EVENT_OPT;
		}
	}
	else
		MPU_ERR("eMajorType %d erro\n", eMajorType);

	return iLocType;

}


eHistoryLogMinorType MinorType_LocToNet(int iMajorType, int iMinorType)
{
	return (eHistoryLogMinorType) (iMajorType * 100 + iMinorType + 1);
}

int MinorType_NetToLoc(eHistoryLogMinorType eMajorType)
{
	return 0;
}

int LogQueryCreate(HANDLE * hIndex, DWORD dwUserID, const LPLogQueryInfo pQuery)
{
	MPU_DBG("LogQueryCreate \n");
	PT_REMOTEQUEERY ptLogquery = 0;


	ptLogquery = CIU_GetLogQueryParam(dwUserID);
	if (ptLogquery->hIndex != NULL)	// 正在查询
	{
		MPU_ERR("LogQueryCreate : there  is a query already\n");
		return -1;
	}
	else
	{
		MPU_DBG("time %s %s %d %d\n", pQuery->m_sStartTime, pQuery->m_sStopTime, pQuery->m_eMajorType, pQuery->m_eMinorType);
		char acTemp[10];

		struct tm tTime;

		struct tm *ptTime = &tTime;

		time_t timep_start, timep_stop;

		memset(acTemp, 0, sizeof (acTemp));
		memcpy(acTemp, pQuery->m_sStartTime, 4);
		tTime.tm_year = atoi(acTemp) - 1900;
		MPU_DBG("year %d\n", tTime.tm_year);

		memset(acTemp, 0, sizeof (acTemp));
		memcpy(acTemp, pQuery->m_sStartTime + 4, 2);
		tTime.tm_mon = atoi(acTemp) - 1;
		MPU_DBG("mon %d\n", tTime.tm_mon);

		memset(acTemp, 0, sizeof (acTemp));
		memcpy(acTemp, pQuery->m_sStartTime + 6, 2);
		tTime.tm_mday = atoi(acTemp);
		MPU_DBG("mday %d\n", tTime.tm_mday);

		memset(acTemp, 0, sizeof (acTemp));
		memcpy(acTemp, pQuery->m_sStartTime + 8, 2);
		tTime.tm_hour = atoi(acTemp);
		MPU_DBG("hour %d\n", tTime.tm_hour);

		memset(acTemp, 0, sizeof (acTemp));
		memcpy(acTemp, pQuery->m_sStartTime + 10, 2);
		tTime.tm_min = atoi(acTemp);
		MPU_DBG("min %d\n", tTime.tm_min);

		memset(acTemp, 0, sizeof (acTemp));
		memcpy(acTemp, pQuery->m_sStartTime + 12, 2);
		tTime.tm_sec = atoi(acTemp);
		MPU_DBG("sec %d\n", tTime.tm_sec);

		timep_start = mktime(ptTime);

		memset(acTemp, 0, sizeof (acTemp));
		memcpy(acTemp, pQuery->m_sStopTime, 4);
		tTime.tm_year = atoi(acTemp) - 1900;
		MPU_DBG("year %d\n", tTime.tm_year);

		memset(acTemp, 0, sizeof (acTemp));
		memcpy(acTemp, pQuery->m_sStopTime + 4, 2);
		tTime.tm_mon = atoi(acTemp) - 1;
		MPU_DBG("mon %d\n", tTime.tm_mon);

		memset(acTemp, 0, sizeof (acTemp));
		memcpy(acTemp, pQuery->m_sStopTime + 6, 2);
		tTime.tm_mday = atoi(acTemp);
		MPU_DBG("mday %d\n", tTime.tm_mday);

		memset(acTemp, 0, sizeof (acTemp));
		memcpy(acTemp, pQuery->m_sStopTime + 8, 2);
		tTime.tm_hour = atoi(acTemp);
		MPU_DBG("hour %d\n", tTime.tm_hour);

		memset(acTemp, 0, sizeof (acTemp));
		memcpy(acTemp, pQuery->m_sStopTime + 10, 2);
		tTime.tm_min = atoi(acTemp);
		MPU_DBG("min %d\n", tTime.tm_min);

		memset(acTemp, 0, sizeof (acTemp));
		memcpy(acTemp, pQuery->m_sStopTime + 12, 2);
		tTime.tm_sec = atoi(acTemp);
		MPU_DBG("sec %d\n", tTime.tm_sec);

		timep_stop = mktime(ptTime);

		MPU_DBG("timep_start %lu, timep_stop %lu\n", (DWORD) timep_start, (DWORD) timep_stop);

		ptLogquery->iLogQueryNum = 0;
		ptLogquery->iLogQueryTotalNum = -1;

		if (0 == MPU_OPL_QueryStart(MajorType_NetToLoc(pQuery->m_eMajorType),
									(unsigned int) timep_start, (unsigned int) timep_stop, 1, LogQueryCallback, &ptLogquery->hIndex))
		{
			*hIndex = ptLogquery->hIndex;
		}
		else
		{
			ptLogquery->hIndex = NULL;
			MPU_ERR("MPU_OPL_QueryStart failed\n");
			return eQueryFailed;
		}
		return eQueryOK;
	}
}

int LogQueryDestroy(HANDLE hIndex)
{
	MPU_DBG("LogQueryDestroy \n");
	MPU_OPL_QueryStop(hIndex);
	PT_REMOTEQUEERY ptLogquery = NULL;

	for (int i = 0; i < LOGINUSER_COUNT; i++)
	{
		ptLogquery = CIU_GetLogQueryParam(i);
		if ((NULL != ptLogquery) && (hIndex == ptLogquery->hIndex))
		{
			MPU_DBG("LogQueryDestroy get handle ok %d\n", i);
			break;
		}
		ptLogquery = NULL;
	}

	if (NULL == ptLogquery)
	{
		MPU_ERR("LogQueryDestroy not find hIndex %d\n", (int) hIndex);
		return -1;
	}

	ptLogquery->hIndex = NULL;
	ptLogquery->iLogQueryNum = 0;
	ptLogquery->iLogQueryTotalNum = 0;
	return 0;
}

int LogQueryNext(HANDLE hIndex, LPLogQueryResult pNext)
{
	//MPU_DBG("LogQueryNext \n");
	int iLogTotalItem = 0;

	LOG tLogBuf;

	PT_REMOTEQUEERY ptLogquery = NULL;

	for (int i = 0; i < LOGINUSER_COUNT; i++)
	{
		ptLogquery = CIU_GetLogQueryParam(i);
		if ((NULL != ptLogquery) && (hIndex == ptLogquery->hIndex) && (ptLogquery->iLogQueryTotalNum >= 0))
		{
			//MPU_DBG("LogQueryNext get handle ok %d\n", i);
			if (ptLogquery->iLogQueryTotalNum == 0)
			{
				MPU_DBG("ptLogquery->iLogQueryNum == 0\n");
				return eQueryFailed;
			}
			break;
		}
		ptLogquery = NULL;
	}

	if (NULL == ptLogquery)
	{
		MPU_ERR("LogQueryNext not find hIndex %d or query is not over\n", (int) hIndex);
		return eQueryBusy;
	}

	if (0 == MPU_OPL_QueryPage(hIndex, &tLogBuf, 1, ptLogquery->iLogQueryNum, &iLogTotalItem))
	{
		if (1 == iLogTotalItem)
		{
			ptLogquery->iLogQueryNum++;
			pNext->m_eMajorType = MajorType_LocToNet(tLogBuf.dwEventType);
			pNext->m_eMinorType = MinorType_LocToNet(tLogBuf.dwEventType, tLogBuf.dwEventDetail);
			pNext->m_dwDetailInfo = tLogBuf.dwChannel;
			snprintf(pNext->m_sUserName, sizeof(pNext->m_sUserName), tLogBuf.strUsername);

			DWORD dwIpaddr = tLogBuf.dwRemote;

			if (0 != dwIpaddr)
			{
				snprintf(pNext->m_sUserIP, sizeof(pNext->m_sUserIP), "%lu.%lu.%lu.%lu", (dwIpaddr & 0xff000000) >> 24,
						(dwIpaddr & 0xff0000) >> 16, (dwIpaddr & 0xff00) >> 8, (dwIpaddr & 0xff));
			}
			else
			{
				pNext->m_sUserIP[0] = 0;
			}


			// 时间
			struct tm tNt;
			struct tm *pnt;
			time_t timeTemp = tLogBuf.dwTime;

			pnt = localtime_r(& timeTemp, &tNt);	/* Convert to local time. */
			snprintf(pNext->m_sLogTime, sizeof(pNext->m_sLogTime), "%04d%02d%02d%02d%02d%02d",
					pnt->tm_year + 1900, pnt->tm_mon + 1, pnt->tm_mday, pnt->tm_hour, pnt->tm_min, pnt->tm_sec);

			MPU_DBG("loginfo[%d] :%d %d %s \n", ptLogquery->iLogQueryNum, (int) pNext->m_eMajorType, (int) pNext->m_eMinorType, pNext->m_sLogTime);
			return eQueryOK;
		}
		else
		{
			MPU_DBG("over\n");
			return eQueryFinished;
		}
	}
	else
	{
		MPU_ERR("over\n");
		return eQueryFailed;
	}
}

void CIU_RegisterLogQuery()
{
	int nSDKRet = 0;

	// 变量初始化

	// 设置
	LogQueryInterface tLogQueryInterface;

	tLogQueryInterface.ifLogQueryCreate = LogQueryCreate;
	tLogQueryInterface.ifLogQueryDestroy = LogQueryDestroy;
	tLogQueryInterface.ifLogQueryNext = LogQueryNext;

	nSDKRet = HieServer_LogQueryIf(&tLogQueryInterface);
	if (0 != nSDKRet)
	{
		MPU_ERR("HieServer_LogQueryIf() erro %d\n", nSDKRet);
	}

	MPU_DBG("HieServer_LogQueryIf() return %d\n", nSDKRet);
}
