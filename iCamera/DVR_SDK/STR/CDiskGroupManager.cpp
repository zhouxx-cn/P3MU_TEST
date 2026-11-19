
/*!
*\file   CDiskGroupManager.cpp
*\brief  提供操作盘组的接口函数
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/
//#include <iostream.h>
#ifdef __SUPPORT_SD_CARD__
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include "CDiskGroup.h"
#include "tagDiskGroupInfo.h"
#include "CDiskGroupManager.h"
#include "PAR/CConfig.h"
#include "pthreadmacro.h"
#include "AlarmCfg.h"
#include "idvr_errno.h"
#include "CDPManager.h"
#include "str_fs.h"
#include "macro_pthread.h"
#include "STR_global.h"

#define ISDISKGROUP_SUPPORT( nDiskGroupType )								\
	{																		\
		if ( nDiskGroupType >= MAX_DISK_GROUP_NUM)							\
		{																	\
			DEBUG_MSG("盘组类型%d不是支持的盘组类型\n",nDiskGroupType);		\
			return IDVR_STR_ERROR_NOTSUPPORT;								\
		}																	\
	}							//end

const static char *section = "diskgroup";

const static char *key = "workmode";


/**************************************************************************
*                            全局变量初始化                               *
 **************************************************************************/

CDiskGroupManager *CDiskGroupManager::theDiskGroupManger = NULL;

sem_t CDiskGroupManager::cond;

/**************************************************************************
*                            全局函数声明                                 *
 **************************************************************************/
static int StartWorkThread();


/**************************************************************************
*                               方法实现                                   *
 **************************************************************************/


//!\brief 创建硬盘分组，主要是注册该新硬盘分组的基本信息。
//!\param int nDiskGroupType 盘组类型。0为普通盘组，1为报警盘组，2为冗余盘组，3为备份
//!\param int nCycleMode 盘组满时是覆盖还是停止
//!\return  成功返回0，失败为-1
int CDiskGroupManager::Create(const unsigned int nDiskGroupType, const unsigned int nCycleMode)
{
	// TODO: Add your specialized code here.
	int ret;

	ISDISKGROUP_SUPPORT(nDiskGroupType);

	DEBUG_MSG("创建盘组%d\n", nDiskGroupType);
	ret = theCDiskGroup[nDiskGroupType]->Create();
	WARN_MSG(ret == 0);
	DEBUG_MSG("创建盘组%d结束\n", nDiskGroupType);
	LOCK(mutex);

	if (ret != 0)
	{
		ret = -1;
	}

	if (0 == ret)
	{
		GetAllDiskGroupInfo();
	}

	UNLOCK(mutex);
	return ret;
}



//!\brief 获取指定盘组的剩余容量(单位：MB)
//!\param int nDiskGroupType 盘组类型。0为普通盘组，1为报警盘组，2为冗余盘组，3为备份盘组
//!\param hDiskGroup 磁盘分组句柄
//!\param pSize 总容量
//!\return 成功返回0，失败返回小于0的错误码。
int CDiskGroupManager::GetDiskGroupRemainSize(const unsigned int nDiskGroupType, int *pSize)
{
	int ret = -1;

	ISDISKGROUP_SUPPORT(nDiskGroupType);
	ret = theCDiskGroup[nDiskGroupType]->GetDiskGroupRemainSize(pSize);
	return ret;

}


//!\brief 检测该录像类型在DiskGroup盘组内是否可以录像
//!\param int type 录像类型
//!\param int DiskGroup 盘组
//!\return  可以为0，失败为false
bool CDiskGroupManager::IsRecordRun(const unsigned int nDiskGroupType, int type, int nCh)
{
	bool ret = false;

	int value, i;

	tagDiskGroupInfo Info;

	int nPartitionSize = 0;

	ISDISKGROUP_SUPPORT(nDiskGroupType);

	if (NULL != theCDiskGroup[nDiskGroupType])
	{
		value = theCDiskGroup[nDiskGroupType]->GetInfo(&Info);

		if (Info.bCh[nCh] == 0)
		{
			return ret;
		}

		for (i = 0; i < Info.Partitions; i++)
		{
			//更换成剩余容量大小
			nPartitionSize += Info.tPartitions[i].dwPartSize;
		}

		if ((nPartitionSize > 0) && (value == 0))
		{
			for (i = 0; i < MAX_BIND_CH_NUM; i++)
			{
				if (Info.bCh[i] != 0)
				{
					ret = true;
				}
			}
		}
	}

	return ret;
}

//##ModelId=49AF739B0274
int CDiskGroupManager::Destroy(const unsigned int nDiskGroupType)
{
	// TODO: Add your specialized code here.
	int ret;

	int type;

	int nch;

	DEBUG_MSG("销毁盘组%d\n", nDiskGroupType);

	ISDISKGROUP_SUPPORT(nDiskGroupType);

	LOCK(mutex);

	for (nch = 0; nch < MAX_BIND_CH_NUM; nch++)
	{
		for (type = GROUP_RECORDE_TYPE_ALARM; type <= GROUP_RECORDE_TYPE_MOTION; type++)
		{
			ret = theCDiskGroup[nDiskGroupType]->UnBindDataCH(nch, type);
			WARN_MSG(0 == ret);
		}
	}

	ret = theCDiskGroup[nDiskGroupType]->destroy();
	WARN_MSG(0 == ret);
	GetAllDiskGroupInfo();
	UNLOCK(mutex);

	DEBUG_MSG("结束销毁盘组%d\n", nDiskGroupType);
	return ret;
}

void CDiskGroupManager::Update()
{
	LOCK(mutex);
	GetAllDiskGroupInfo();
	UNLOCK(mutex);
}

//##ModelId=49AF74090308
int CDiskGroupManager::GetInfo(const unsigned int nDiskGroupType, tagDiskGroupInfo * Info)
{
	// TODO: Add your specialized code here.
	int ret;

	ISDISKGROUP_SUPPORT(nDiskGroupType);

	LOCK(mutex);
	ret = theCDiskGroup[nDiskGroupType]->GetInfo(Info);

	if (Info->Partitions)
	{
		detect_status(nDiskGroupType);
	}

	WARN_MSG(0 == ret);
	UNLOCK(mutex);
	return ret;
}

//##ModelId=49AF745D0377
int CDiskGroupManager::BindDataCH(const unsigned int nDiskGroupType, const unsigned int nCh, const unsigned int RecType)
{
	// TODO: Add your specialized code here.
	int ret = 0;


	ISDISKGROUP_SUPPORT(nDiskGroupType);


	ret = theCDiskGroup[nDiskGroupType]->BindDataCH(nCh, RecType);

	if (nDiskGroupType == 1 && RecType == GROUP_RECORDE_TYPE_ALARM && IsRecordRun(GROUP_RECORDE_TYPE_ALARM, RecType, nCh))
	{							//解除普通盘组上的报警录像
		ret = theCDiskGroup[0]->UnBindDataCH(nCh, RecType);
	}

	WARN_MSG(0 == ret);

	return ret;
}

//##ModelId=49AF745D0377
int CDiskGroupManager::UnBindDataCH(const unsigned int nDiskGroupType, const unsigned int nCh, const unsigned int RecType)
{
	// TODO: Add your specialized code here.
	int ret = 0;

	ISDISKGROUP_SUPPORT(nDiskGroupType);


	if ((0 == nDiskGroupType) && (RecType != GROUP_RECORDE_TYPE_ALARM))
	{
		//普通盘组无法解除绑定普通录像
		DEBUG_MSG("普通盘组无法解除%d录像类型\n", RecType);
		return IDVR_ERROR_PARAMETER;
	}

	ret = theCDiskGroup[nDiskGroupType]->UnBindDataCH(nCh, RecType);

	if (nDiskGroupType == 1 && RecType == GROUP_RECORDE_TYPE_ALARM && ret == 0)
	{							//将报警录像绑定至普通盘组
		ret = theCDiskGroup[0]->BindDataCH(nCh, RecType);
	}

	WARN_MSG(0 == ret);

	return ret;
}


//!\brief 磁盘满回调函数
//!\param HANDLE handle 盘组的句柄
//!\return  成功返回0，失败为-1
int DiskGroupFullCallBack(HANDLE handle)
{
	CDiskGroupManager *theCDiskGroupManager = CDiskGroupManager::GetInstance();

	unsigned int i = 0;

	int nDiskGroupType = -1;

	for (i = 0; i < MAX_DISK_GROUP_NUM; i++)
	{
		if (theCDiskGroupManager->theCDiskGroup[i]->dev == handle)
		{
			nDiskGroupType = i;
		}
	}

	if (-1 == nDiskGroupType)
	{
		TRACE("磁盘回调出现错误\n");

		TRACE("回调句柄为:%p\n", handle);

		for (i = 0; i < MAX_DISK_GROUP_NUM; i++)
		{
			TRACE("磁盘%d句柄为:%p\n", i, theCDiskGroupManager->theCDiskGroup[i]->dev);
		}
		return -1;
	}


	if (NULL != theCDiskGroupManager)
	{
		//if (theCDiskGroupManager->m_Full[nDiskGroupType] != 1)
		{
			struct tagAlertInfo Info;

			Info.nChannel = nDiskGroupType;
			Info.dwReserved = 0;

			theCDiskGroupManager->PushAlarm(theCDiskGroupManager->m_obj[CDiskGroupManager::FULL].AlarmType, (void *) 0, &Info);
			theCDiskGroupManager->m_Full[nDiskGroupType] = 1;
		}
	}

	return 0;
}

//!\brief 设置盘组满回调函数
//!\param int nType 盘组满返回的类型
//!\param CBAlert CallBack 回调函数句柄
//!\return  成功返回0，失败为-1
int CDiskGroupManager::SetDiskGroupFullCallBack(int AlarmType, CBAlert callabck)
{
	int ret;

	if (NULL == m_theGroupFullAlarm)
	{
		DEBUG_MSG("报警参数出现错误\n");
		return IDVR_ERROR_ALLOC;
	}

	m_obj[CDiskGroupManager::FULL].AlarmType = AlarmType;

	ret = m_theGroupFullAlarm->SetCallBack(AlarmType, callabck);
	Macro_SetDiskGroupFullCallback(DiskGroupFullCallBack);

	return ret;
}

//!\brief 设置盘组异常回调函数
//!\param int nType 盘组满返回的类型
//!\param CBAlert CallBack 回调函数句柄
//!\return  成功返回0，失败为-1
int CDiskGroupManager::SetDiskGroupErrorCallBack(int AlarmType, CBAlert callabck)
{
	int ret;

	if (NULL == m_theGroupErrorAlarm)
	{
		DEBUG_MSG("报警参数出现错误\n");
		return IDVR_ERROR_ALLOC;
	}

	m_obj[CDiskGroupManager::ERROR].AlarmType = AlarmType;

	ret = m_theGroupErrorAlarm->SetCallBack(AlarmType, callabck);

	return ret;
}

#define ALARM_CALL(RETVALTYPE,DEFRETVAL,AlarmType,x) {\
											\
	RETVALTYPE ret = DEFRETVAL;				\
											\
	CAlarm *Obj = NULL;							\
	if (AlarmType == m_obj[CDiskGroupManager::FULL].AlarmType)	\
	{															\
		Obj = m_obj[CDiskGroupManager::FULL].obj;				\
	}															\
	if (AlarmType == m_obj[CDiskGroupManager::ERROR].AlarmType)	\
	{															\
		Obj = m_obj[CDiskGroupManager::ERROR].obj;				\
	}															\
											\
											\
	if ( NULL == Obj )						\
	{										\
		TRACE("报警参数出现错误:%s,AlarmType = %d [%p %p]\n",#x,AlarmType,m_obj[CDiskGroupManager::FULL].obj,m_obj[CDiskGroupManager::ERROR].obj);	\
		return ret;			\
	}										\
											\
											\
	ret = Obj->x;							\
											\
	return ret;								\
}

//!\brief 判断某个报警联动动作是否开启
//!\praram int AlarmType 报警类型。VIDEO_LOST 
//!\praram int Action    !      ACTION_SCREEN 屏幕显示
//  !      ACTION_BEEP 声音提示
//  !      ACTION_ALARMOUT 报警输出
//  !      ACTION_SENDTOSERVER 上传报警服务器
//  !      ACTION_EMAIL 联动Email
//  !      ACTION_SMS 联动短信
//!\return 成功为0，失败为错误码
//##ModelId=49BDBA5C02A0
int CDiskGroupManager::EnableAction(int AlarmType, tagActionType Action, bool bEnable)
{
	ALARM_CALL(int, -1, AlarmType, EnableAction(AlarmType, Action, bEnable));
}

//!\brief 判断某个报警联动动作是否开启
//!\praram int AlarmType 报警类型。
//!\praram int Action   动作类型
//!\return false 为禁止联动 true 为开启联动
//##ModelId=49BDBA5C02A0
bool CDiskGroupManager::IsEnableAction(int AlarmType, tagActionType Action)
{
	ALARM_CALL(bool, false, AlarmType, IsEnableAction(AlarmType, Action));
}

//!\brief 设置某个联动动作涉及的通道。无通道概念的不需要设置
//!\param int nCh 通道
//!\praram int AlarmType 报警类型
//!\praram tagActionType Action   动作类型
//!\praram int AlarmCh 联动动作涉及到通道。
//!\praram bool bEnable 启用/不启用该通道的配置
//!\praram int dwReserved 保留字。对于预置点、轨迹、巡航线路输入上述编号
//!\return  成功为0，失败为-1
int CDiskGroupManager::SetActionChParam(int AlarmType, tagActionType Action, int AlarmCh, bool bEnable, int nReserved)
{
	ALARM_CALL(int, -1, AlarmType, SetActionChParam(AlarmType, Action, AlarmCh, bEnable, nReserved));
}

//!\brief 获取盘组工作模式
//!\param 无
//!\return  0为一般模式，1为高级模式
int CDiskGroupManager::GetWorkMode()
{
	return m_nWorkMode;
}

//!\brief 设置盘组工作模式
//!\param int nWorkMode 0为一般模式，1为高级模式
//!\return  成功为0，失败为-1
int CDiskGroupManager::SetWorkMode(int nWorkMode)
{
	CConfig *theCConfig = CConfig::GetInstance();

	int ret = 0;

	int ch, RecType;



	if (m_nWorkMode != nWorkMode)
	{
		if (nWorkMode == 1)
		{
			m_nWorkMode = nWorkMode;
			Update();
		}

	}
	else
	{
		return 0;
	}

	if (NULL != theCConfig)
	{
		ret = theCConfig->SetValue(section, key, nWorkMode);
	}


	return 0;


	if (0 == nWorkMode)
	{
		DEBUG_MSG("销毁其他盘组\n");

		for (ch = 0; ch < MAX_BIND_CH_NUM; ch++)
		{
			for (RecType = GROUP_RECORDE_TYPE_ALARM; RecType <= GROUP_RECORDE_TYPE_MOTION; RecType++)
			{
				ret = theCDiskGroup[1]->TmpUnBindDataCH(ch, RecType);
				ret = theCDiskGroup[2]->TmpUnBindDataCH(ch, RecType);
				ret = theCDiskGroup[3]->TmpUnBindDataCH(ch, RecType);
			}
		}

		ret = theCDiskGroup[1]->destroy();
		if (0 != ret)
		{
			Update();
			return ret;
		}
		ret = theCDiskGroup[2]->destroy();
		if (0 != ret)
		{
			Update();
			return ret;
		}
		ret = theCDiskGroup[3]->destroy();
		if (0 != ret)
		{
			Update();
			return ret;
		}
		detect_status(1);
		detect_status(2);
		detect_status(3);
	}

	if (NULL != theCConfig)
	{
		ret = theCConfig->SetValue(section, key, nWorkMode);
	}

	if (m_nWorkMode != nWorkMode)
	{
		m_nWorkMode = nWorkMode;
		Update();
	}

	return ret;
}


//!\brief 获取某个联动动作涉及的通道或者编号
//!\praram int AlarmType 报警类型
//!\praram tagActionType Action 动作类型
//!\praram bool *szCh 通道数组，true为联动了该通道，false为没有联动该通道
//!\praram int *szReserved 对应通道涉及到的子类型数据。目前仅相对于预置点编号等
//!\return  成功为0，失败为-1
int CDiskGroupManager::GetActionChParam(int AlarmType, tagActionType Action, bool * szCh, int *szReserved = NULL)
{
	ALARM_CALL(int, -1, AlarmType, GetActionChParam(AlarmType, Action, szCh, szReserved));
}

//!\brief 向上报警
//!\praram 磁盘满报警
//!\return  成功为0，失败为-1
int CDiskGroupManager::PushAlarm(int AlarmType, void *org, struct tagAlertInfo *Info)
{
	ALARM_CALL(int, -1, AlarmType, PushAlarm(org, Info));
}

//##ModelId=49AF749B0236
int CDiskGroupManager::AddPartition(const unsigned int nDiskGroupType, const char *PartitionName)
{
	// TODO: Add your specialized code here.    
	int ret = 0;


	ISDISKGROUP_SUPPORT(nDiskGroupType);


	ret = theCDiskGroup[nDiskGroupType]->AddPartition(PartitionName);

	if (0 == ret)
	{
		Update();
	}

	WARN_MSG(ret == 0);
	return ret;
}

//##ModelId=49AF74ED0125
int CDiskGroupManager::DelPartition(const unsigned int nDiskGroupType, const char *PartitionName)
{
	// TODO: Add your specialized code here.    
	int ret = 0;

	if (nDiskGroupType >= MAX_DISK_GROUP_NUM)
	{
		DEBUG_MSG("%d超过最大允许的盘组类型\n", nDiskGroupType);
		return -1;
	}

	LOCK(mutex);
	ret = theCDiskGroup[nDiskGroupType]->DelPartition(PartitionName);
	UNLOCK(mutex);

	if (0 == ret)
	{
		Update();
	}

	return ret;
}

//!\param int nDataKeepTime 数据保持时间
//盘组属性。包好读写属性及数据的保持时间。
//!\return  成功返回0，失败返回小于0的错误码。
int CDiskGroupManager::Set_DataKeepTime(const unsigned int nDiskGroupType, const unsigned int nDataKeepTime)
{
	// TODO: Add your specialized code here.

	ISDISKGROUP_SUPPORT(nDiskGroupType);



	theCDiskGroup[nDiskGroupType]->Set_DataKeepTime(nDataKeepTime);
	return (int) 0;
}

#ifdef __PKGSTREAM_STORTAGE__
static int Pkg_Syserror_Callback(PKGSTREAMERR nPkgErr, char *sFileName, char *Function, int nLine)
{
	printf("------------------------存储系统出现错误-------------------------\n");
	printf("%s\n", __FS_VERSION__);
	printf("文件：%s\n", sFileName);
	printf("函数：%s\n", Function);
	printf("行数：%d\n", nLine);
	printf("------------------------存储系统出现错误--------------------------\n");
	return 0;
}
#endif

extern "C"
{
	//int MP4Sys_GetSerialNum();
	int ZWSys_GetSerialNum(unsigned char * pSerialNum);
};

//##ModelId=49BF195E0008
int CDiskGroupManager::Init()
{
	// TODO: Add your specialized code here.
	int ret = 0;
	unsigned int i = 0;
    unsigned char szSerial[32] = {0};

	LOCK(mutex);

	if (1 == bWorkThreadRun)
	{
		UNLOCK(mutex);
		return 0;
	}

	printf("%s\n", __FS_VERSION__);

#ifndef TEST_MEM_INFO
	theCDiskGroup = new class CDiskGroup *[MAX_DISK_GROUP_NUM];
#else
	theCDiskGroup = CommonKit::TCreateObject<CDiskGroup *>::CreateObject(MAX_DISK_GROUP_NUM, __FILE__, __FUNCTION__, __LINE__);
#endif

#ifndef TEST_MEM_INFO
	m_theGroupFullAlarm = new CAlarm();
#else
	m_theGroupFullAlarm = CommonKit::TCreateObject<CAlarm>::CreateObject(__FILE__, __FUNCTION__, __LINE__);
#endif

	if (NULL != m_theGroupFullAlarm)
	{
		DEBUG_MSG("初始化报警配置参数成功\n");
		m_theGroupFullAlarm->Init("disk_group_full");
		m_theGroupFullAlarm->ReadCfg();
	}
	else
	{
		DEBUG_MSG("初始化报警配置参数失败\n");
	}

#ifndef TEST_MEM_INFO
	m_theGroupErrorAlarm = new CAlarm();
#else
	m_theGroupErrorAlarm = CommonKit::TCreateObject<CAlarm>::CreateObject(__FILE__, __FUNCTION__, __LINE__);
#endif

	if (NULL != m_theGroupErrorAlarm)
	{
		DEBUG_MSG("初始化报警配置参数成功\n");
		m_theGroupErrorAlarm->Init("disk_group_error");
		m_theGroupErrorAlarm->ReadCfg();
	}
	else
	{
		DEBUG_MSG("初始化报警配置参数失败\n");
	}

	m_obj[CDiskGroupManager::FULL].obj = m_theGroupFullAlarm;
	m_obj[CDiskGroupManager::ERROR].obj = m_theGroupErrorAlarm;

	//初始化各种参数
	for (i = 0; i < MAX_DISK_GROUP_NUM; i++)
	{

		theCDiskGroup[i] = CDiskGroup::GetInstance(i);
		m_Status[i] = 0;
		m_Full[i] = 0;
		if (NULL == theCDiskGroup[i])
		{
			DEBUG_MSG("初始化盘组%d失败\n", i);
			UNLOCK(mutex);
			return -1;
		}
	}

	DEBUG_MSG("初始化存储系统\n");

	CConfig *theCConfig = CConfig::GetInstance();

	if (NULL != theCConfig)
	{
		m_nWorkMode = theCConfig->GetValue(section, key, 0);
	}

	ZWSys_GetSerialNum(szSerial);
	ret = Macro_Init((char *)szSerial);

	if (ret != 0)
	{
		DEBUG_MSG("初始化存储系统失败\n");
		UNLOCK(mutex);
		return ret;
	}

	DEBUG_MSG("初始化存储系统成功\n");
	Macro_SysErrorCallbackRegister(Pkg_Syserror_Callback);
	GetAllDiskGroupInfo();

	if (0 == bWorkThreadRun)
	{
		sem_init(&cond, 0, 0);
		bWorkThreadRun = 1;
		StartWorkThread();
	}

	UNLOCK(mutex);
	TRACE("init over..............\n");
	return (int) ret;
}

//##ModelId=49BF19610052
int CDiskGroupManager::UnInit()
{
	// TODO: Add your specialized code here.
	if (bWorkThreadRun == 0)
	{
		return -1;
	}
DEBUG_MSG("my add: --CDiskGroupManager--UnInit--a\n");
	bWorkThreadRun = 0;
	sem_wait(&cond);
	sem_destroy(&cond);
	if (NULL != theCDiskGroup)
	{
#ifndef TEST_MEM_INFO
		delete[]theCDiskGroup;
#else
		CommonKit::TDestroyObject<CDiskGroup *>::DestroyObject(theCDiskGroup);
#endif
		theCDiskGroup = NULL;
	}
	DEBUG_MSG("my add: --CDiskGroupManager--UnInit--b\n");

	if (NULL != m_theGroupFullAlarm)
	{
#ifndef TEST_MEM_INFO
		delete m_theGroupFullAlarm;
#else
		CommonKit::TDestroyObject<CAlarm>::DestroyObject(m_theGroupFullAlarm);
#endif

		m_theGroupFullAlarm = NULL;
	}
	DEBUG_MSG("my add: --CDiskGroupManager--UnInit--c\n");

	if (NULL != m_theGroupErrorAlarm)
	{
#ifndef TEST_MEM_INFO
		delete m_theGroupErrorAlarm;
#else
		CommonKit::TDestroyObject<CAlarm>::DestroyObject(m_theGroupErrorAlarm);
#endif

		m_theGroupErrorAlarm = NULL;
	}
	DEBUG_MSG("my add: --CDiskGroupManager--UnInit--d\n");

	UNINIT_MUTEX(mutex);
	return (int) 0;
}

//##ModelId=49BDA6850206
CDiskGroupManager *CDiskGroupManager::GetInstance()
{
	// TODO: Add your specialized code here.
	if (NULL == theDiskGroupManger)
	{
#ifndef TEST_MEM_INFO
		theDiskGroupManger = new CDiskGroupManager();
#else
		theDiskGroupManger = CommonKit::TCreateObject<CDiskGroupManager>::CreateObject(__FILE__, __FUNCTION__, __LINE__);
#endif

		if (NULL == theDiskGroupManger)
		{
			DEBUG_MSG("获取管理器实例失败\n");
			return NULL;
		}

		theDiskGroupManger->Init();
	}
	return (CDiskGroupManager *) theDiskGroupManger;
}

//##ModelId=49BDA69B001D
int CDiskGroupManager::ReleaseInstance()
{
	// TODO: Add your specialized code here.
	if (NULL != theDiskGroupManger)
	{
		theDiskGroupManger->UnInit();
#ifndef TEST_MEM_INFO
		delete theDiskGroupManger;
#else
		CommonKit::TDestroyObject<CDiskGroupManager>::DestroyObject(theDiskGroupManger);
#endif

		theDiskGroupManger = NULL;
	}

	return (int) 0;
}

//##ModelId=49BE1EFF00EC
CDiskGroupManager::~CDiskGroupManager()
{
	// TODO: Add your specialized code here.
}

//##ModelId=49BF01BF02AC
CDiskGroupManager::CDiskGroupManager()
{
	// TODO: Add your specialized code here.
	bWorkThreadRun = 0;
	m_nWorkMode = 0;

	INIT_MUTEX(mutex);
}

//!\brief 获取指定分区所含有的录像数据的开始时间和结束时间
//!\param hDiskGroup 硬盘分组句柄
//!\param pcDevPart 分区节点号
//!\param tBeginTime 存放开始时间
//!\param tEndTime 存放结束时间
//!\return 成功返回0，失败返回小于0的错误码。
int CDiskGroupManager::GetDiskPartDataInfo(const unsigned int nDiskGroupType, char *pcDevPart, time_t * tBeginTime, time_t * tEndTime)
{
	int ret = -1;

	ret = Macro_GetDiskPartDataInfo(theCDiskGroup[nDiskGroupType]->dev, pcDevPart, tBeginTime, tEndTime);
	WARN_MSG(0 == ret);
	return ret;
}

//!\brief 获取指定分区所含有的录像数据的开始时间和结束时间
 //!\param pcDevPart 分区节点号
 //!\param tBeginTime 存放开始时间
 //!\param tEndTime 存放结束时间
 //!\return 成功返回0，失败返回小于0的错误码。
int CDiskGroupManager::GetDiskPartDataInfoEx(char *pcDevPart, time_t * tBeginTime, time_t * tEndTime)
{
	int ret = -1;

	ret = Macro_GetDiskPartDataInfoEx(pcDevPart, tBeginTime, tEndTime);
	WARN_MSG(0 == ret);
	return ret;
}

//!\brief 获取磁盘信息, 存入ptDiskInfo.
//!\param iDiskIndex  磁盘编号(取值范围在0——(硬盘个数-1)).
//!\param ptDiskInfo 磁盘信息结构体指针.
//!\return  成功返回0, 失败返回-1.
int CDiskGroupManager::GetDiskGroupInfo(int iDiskIndex)
{
	unsigned int j;

	int bFind;

	PSS_DISKGROUPINFO ptDiskGroupInfo;

	HANDLE handle;

	handle = Macro_GetDiskGroupInfo(iDiskIndex, &ptDiskGroupInfo);

	//更新盘组信息
	for (j = 0; j < MAX_DISK_GROUP_NUM; j++)
	{
		bFind = 0;
		if (NULL != handle)
		{
			DEBUG_MSG("对比%s -%s \n", ptDiskGroupInfo.acGroupName, theCDiskGroup[j]->GetName());
			if (0 == strcmp(ptDiskGroupInfo.acGroupName, theCDiskGroup[j]->GetName()))
			{
				theCDiskGroup[j]->Update(iDiskIndex);
				bFind = 1;
				break;
			}
		}
		else
		{
			TRACE("获取%d失败\n", iDiskIndex);
		}

	}

	if (0 == bFind && NULL != handle)
	{
		//不认识的盘组
		DEBUG_MSG("发现了不认识的盘组%s，类型%d，创建时间%lu\n",
				  ptDiskGroupInfo.acGroupName, ptDiskGroupInfo.iGroupType, ptDiskGroupInfo.tCreateTime);
		Macro_DiskGroupDestroy(handle);
		return -1;
	}

	return 0;
}

//!\brief 获取磁盘信息, 存入ptDiskInfo.
//!\param iDiskIndex  磁盘编号(取值范围在0——(硬盘个数-1)).
//!\param ptDiskInfo 磁盘信息结构体指针.
//!\return  成功返回0, 失败返回-1.
int CDiskGroupManager::GetAllDiskGroupInfo()
{
	unsigned int i = 0;

	int ret;

  FINDOTHERGROUP:


	//重置盘组信息
	for (i = 0; i < MAX_DISK_GROUP_NUM; i++)
	{
		if (0 == m_nWorkMode && (i > 0))
		{
			continue;
		}

		theCDiskGroup[i]->Create();
	}


	m_nDiskGroupNum = Macro_GetDiskGroupCount();

	DEBUG_MSG("盘组个数%d\n", m_nDiskGroupNum);

	//重置盘组信息
	for (i = 0; i < MAX_DISK_GROUP_NUM; i++)
	{
		theCDiskGroup[i]->Update(ERR_DISKGROUP_INDEX);
	}

	DEBUG_MSG("刷新盘组信息\n");
	//刷新盘组信息
	for (i = 0; i < m_nDiskGroupNum; i++)
	{
		DEBUG_MSG("刷新盘组信息%d\n", i);
		ret = GetDiskGroupInfo(i);

		if (0 != ret)
		{
			DEBUG_MSG("刷新盘组信息%d失败\n", i);
			goto FINDOTHERGROUP;
		}
	}

	DEBUG_MSG("工作模式%d\n", m_nWorkMode);
#if (MAX_DISK_GROUP_NUM == 4)
	if (0 == m_nWorkMode)
	{
		DEBUG_MSG("销毁其他盘组\n");

		for (ch = 0; ch < MAX_BIND_CH_NUM; ch++)
		{
			for (RecType = GROUP_RECORDE_TYPE_ALARM; RecType <= GROUP_RECORDE_TYPE_MOTION; RecType++)
			{
				ret = theCDiskGroup[1]->UnBindDataCH(ch, RecType);
				ret = theCDiskGroup[2]->UnBindDataCH(ch, RecType);
				ret = theCDiskGroup[3]->UnBindDataCH(ch, RecType);
			}
		}

		theCDiskGroup[1]->destroy();
		theCDiskGroup[2]->destroy();
		theCDiskGroup[3]->destroy();
		detect_status(1);
		detect_status(2);
		detect_status(3);
	}
#endif

	m_nDiskGroupNum = Macro_GetDiskGroupCount();

	DEBUG_MSG("盘组个数%d\n", m_nDiskGroupNum);

#if (MAX_DISK_GROUP_NUM == 4)

	//重置盘组信息
	for (i = 0; i < MAX_DISK_GROUP_NUM; i++)
	{
		theCDiskGroup[i]->Update(ERR_DISKGROUP_INDEX);
	}
#endif
	//刷新盘组信息
	for (i = 0; i < m_nDiskGroupNum; i++)
	{
		ret = GetDiskGroupInfo(i);

		if (0 != ret)
		{
			goto FINDOTHERGROUP;
		}
	}

#if (MAX_DISK_GROUP_NUM == 4)

	//更新盘组绑定关系
	for (ch = 0; ch < MAX_BIND_CH_NUM; ch++)
	{
		for (RecType = GROUP_RECORDE_TYPE_ALARM; RecType <= GROUP_RECORDE_TYPE_ALARM; RecType++)
		{
			//检测报警盘组是否可以录报警录像
			if (!IsRecordRun(1, RecType, ch))
			{
				//报警盘组无法继续录像
				DEBUG_MSG("报警盘组无法继续录像，解除该盘组的报警通道\n");
				CDPManager::GetInstance()->PauseRecord(ch, 1);
				ret = theCDiskGroup[1]->TmpUnBindDataCH(ch, RecType);
				ret = theCDiskGroup[0]->BindDataCH(ch, RecType);
				CDPManager::GetInstance()->PauseRecord(ch, 0);
			}
			else
			{
				//报警盘组可以继续录像
				DEBUG_MSG("报警盘组可以继续录像，解除普通盘组的报警通道\n");
				CDPManager::GetInstance()->PauseRecord(ch, 1);
				ret = theCDiskGroup[0]->UnBindDataCH(ch, RecType);
				theCDiskGroup[1]->UpdateBindCh();
				CDPManager::GetInstance()->PauseRecord(ch, 0);
			}
		}
	}
#endif
	//更新盘组信息
	for (i = 0; i < MAX_DISK_GROUP_NUM; i++)
	{
		theCDiskGroup[i]->UpdateBindCh();
	}

	return 0;
}

//!\brief 检测盘组的数据是否已经超过设定的时间
//!\param 无
	//!\return  返回编号
int CDiskGroupManager::detect_data()
{
	unsigned int i;

	for (i = 0; i < MAX_DISK_GROUP_NUM; i++)
	{
		theCDiskGroup[i]->detect_data();
	}
	return 0;
}


//检测盘组
int CDiskGroupManager::detect_status(int DiskGroup)
{
	int i = DiskGroup, status;

#if (MAX_DISK_GROUP_NUM == 4)
	static int sum[MAX_DISK_GROUP_NUM] = { 0, 0, 0, 0 };
#elif (MAX_DISK_GROUP_NUM == 1)
	static int sum[MAX_DISK_GROUP_NUM] = { 0 };
#else
#error MAX_DISK_GROUP_NUM is error
#endif
	struct tagAlertInfo Info;;

	Info.nChannel = i;
	Info.dwReserved = 0;

	if (m_nWorkMode == 0 && i > 0 && i < MAX_DISK_GROUP_NUM)
	{
		m_Status[i] = 1;
		sum[i] = 0;
		if (m_Full[i] != 0)
		{
			TRACE("here\n");
			PushAlarm(m_obj[CDiskGroupManager::FULL].AlarmType, (void *) 1, &Info);
			m_Full[i] = 0;
		}
		return 0;
	}

	status = theCDiskGroup[i]->GetStatus();


	if (status == 3)
	{
		//状态改为满
		//if (m_Full[i] != 1)
		{
			TRACE("here\n");
			PushAlarm(m_obj[CDiskGroupManager::FULL].AlarmType, (void *) 0, &Info);
			m_Full[i] = 1;
		}

		status = 0;
	}
	else
	{
		//状态改为未满
		if (m_Full[i] != 0)
		{
			TRACE("here\n");
			PushAlarm(m_obj[CDiskGroupManager::FULL].AlarmType, (void *) 1, &Info);
			m_Full[i] = 0;
		}
	}

	if (m_Status[i] != status)
	{
		Info.nChannel = i;
		Info.dwReserved = 0;
		if (m_Status[i] == 0)
		{
			if (++sum[i] >= 2)
			{
			TRACE("here\n");
				DEBUG_MSG("盘组%d异常\n", i);
				PushAlarm(m_obj[CDiskGroupManager::ERROR].AlarmType, (void *) 0, &Info);
				m_Status[i] = status;
				sum[i] = 10;
			}
		}
		else if (status == 0)
		{
			sum[i] = 0;
			TRACE("here\n");
			PushAlarm(m_obj[CDiskGroupManager::ERROR].AlarmType, (void *) 1, &Info);
			m_Status[i] = status;
		}
	}
	return 0;
}

//检测盘组
int CDiskGroupManager::detect_status()
{
	int i;

	for (i = 0; i < MAX_DISK_GROUP_NUM; i++)
	{
		if (m_nWorkMode == 0 && i > 0)
		{
			continue;
		}
		detect_status(i);
	}
	return 0;
}

//!\brief 工作线程
//!\param 无
	//!\return  返回编号
void *work_thread_data(void *org)
{
	prctl(PR_SET_NAME,"work_thread_data");
    PRT_TID();

	struct timespec ts;

	int nSleepSum;

	ts.tv_sec = 1;
	ts.tv_nsec = 0;
	CDiskGroupManager *handle = CDiskGroupManager::GetInstance();

	BEGINRUN_THREAD;

	while (handle->bWorkThreadRun)
	{

		nSleepSum = 0;

		//下面的睡眠方式，防止线程退出过慢的问题
		while ((nSleepSum++) < 60 /*需要扩大 */ )
		{
			if (handle->bWorkThreadRun)
			{
				nanosleep(&ts, NULL);
			}
			else
			{
				nSleepSum = 0;
				break;
			}
		}

		if (NULL != handle)
		{

			//检测盘组中的数据是否超过了设定的数据
			handle->detect_data();
			handle->detect_status();
		}

		DEBUG_MSG("盘组检测线程正在运行\n");
	}
	sem_post(&CDiskGroupManager::cond);
	ENDRUN_THREAD;
	DEBUG_MSG("淘汰线程退出\n");
	return NULL;
}

//!\brief 创建工作线程
//!\param 无
	//!\return  返回编号
static int StartWorkThread()
{
	// TODO: Add your specialized code here.
	int status;

	pthread_attr_t new_attr;

	pthread_t thread_t;

	pthread_attr_init(&new_attr);
	pthread_attr_setdetachstate(&new_attr, PTHREAD_CREATE_DETACHED);
	status = pthread_create(&thread_t, &new_attr, work_thread_data, NULL);
	if (status != 0)
	{
		perror("创建线程出错\n");
		ERROR_MSG("创建同步线程出错:%s\n", strerror(errno));
		return -1;
	}

	return 0;
}
#endif