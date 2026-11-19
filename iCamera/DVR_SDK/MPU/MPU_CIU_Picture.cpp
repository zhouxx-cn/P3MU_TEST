
/*!
 *\file		MPU_CIU_Playback.cpp
 *\brief	MPU模块，远程抓图浏览的实现
 *\author	周庆丰
 *\date		2009/08/24

功能：
-#	
-#	远程抓图浏览

说明：
-#	 
 */


#include "MPU_CIU_Picture.h"
#include "SnapShot/CSnapshotManager.h"
#include <sys/stat.h>
#include <semaphore.h>


#define ERR_PIC_BUSY      ((int)eQueryBusy)
#define ERR_PIC_OK      ((int)eQueryOK)	/*!< 查询成功                               */
#define ERR_PIC_FAILED      ((int)eQueryFailed)	/*!< 查询失败                               */
#define ERR_PIC_FINISHED      ((int)eQueryFinished)	/*!< 查询结束                               */

typedef struct _HY_CIU_SSM_CFG_
{
	HANDLE hSsm;				//快照查询句柄(真实的)
	DWORD dwHandeVerfyMark;		//用于验证当前句柄有效性
	DWORD dwChannel;			//通道
	time_t tStartTime;			//开始时间
	time_t tStopTime;			//结束时间
	int nIndex;					//当前查询
	int nTotal;					//总数
} HY_CIU_PIC_CFG;


//static int m_bInit = 0;

static HY_CIU_PIC_CFG m_hPic[MAX_PICTURESUPPORT_SEARCH];

//创建流 线程
static pthread_t m_hCreateThread=0;

static sem_t m_sem_create;

static int HY_DVR_FileQueryCreate(HANDLE * hIndex, DWORD dwUserID, const LPFileQueryInfo pQuery)
{
	return ERR_PIC_FAILED;
}

static int HY_DVR_FileQueryDestroy(HANDLE hIndex)
{
	return ERR_PIC_FAILED;
}

static int HY_DVR_FileQueryNext(HANDLE hIndex, LPFileQueryResult pNext)
{
	return ERR_PIC_FAILED;
}

static void *Fnx_CreateThread(void *lParam)
{
    PRT_TID();

	return NULL;
}

int CIU_PictureManager_init()
{
	int result;

	FileQueryInterface fqi;

	memset(m_hPic, 0, MAX_PICTURESUPPORT_SEARCH * sizeof (HY_CIU_PIC_CFG));

	sem_init(&m_sem_create, 0, 0);
	//创建注册检测线程
	if(m_hCreateThread == 0)
	{
		result = pthread_create(&m_hCreateThread, NULL, Fnx_CreateThread, NULL);
		if (result < 0)
		{
			MPU_ERR("Create thread failed\n");

			return -1;
		}
	}
	fqi.ifFileQueryCreate = HY_DVR_FileQueryCreate;
	fqi.ifFileQueryDestroy = HY_DVR_FileQueryDestroy;
	fqi.ifFileQueryNext = HY_DVR_FileQueryNext;

	HieServer_FileQueryIf(&fqi);
	return 0;
}
