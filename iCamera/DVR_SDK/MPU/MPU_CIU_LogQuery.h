#ifndef _INC_MPU_CIU_LOGQUERY_H_
#define _INC_MPU_CIU_LOGQUERY_H_

typedef struct tagRemoteQuery
{
	HANDLE hIndex;
	volatile int iLogQueryNum;
	volatile int iLogQueryTotalNum;
}
T_REMOTEQUEERY, *PT_REMOTEQUEERY;

void CIU_RegisterLogQuery();
#endif							/* #ifndef _INC_MPU_CIU_LOGQUERY_H_ */
