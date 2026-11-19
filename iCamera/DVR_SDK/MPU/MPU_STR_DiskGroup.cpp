
/*!
 *\file		MPU_STR_DiskGroup.cpp
 *\brief	MPU模块，盘组管理相关接口实现
 *\author	林云
 *\date		2009/04/07

功能：
-#	提供磁盘分组的管理（建立、删除、更改、设置组的属性）
-#	提供磁盘属性（容量、剩余容量、类型、状态、物理位置）的获取
-#	盘组、数据的绑定，数据的查询、存储、读取等
 */


#include "MPU_global.h"

#include "STR/CDiskGroupManager.h"

/**************************************************************************
 *                     声明局部宏定义                                     *
 **************************************************************************/

#ifndef DEBUG_NOT_USE_STR_LIB	//调试时，不使用STR库

//调用盘组管理器执行动作
#define MPU_STR_DISKGROUP_CALL(x)								\
do																\
{																\
	int nRet;														\
																\
	CDiskGroupManager *pMgr = CDiskGroupManager::GetInstance();		\
																\
	if (NULL != pMgr)												\
	{															\
		nRet = pMgr->x;											\
																\
		if (IDVR_OK != nRet)										\
		{														\
			MPU_ERR("%s, nRet = %d\n", #x, nRet);					\
		}														\
	}															\
	else															\
	{															\
		MPU_ERR("Get CDiskGroupManager instance fail!\n");				\
		MPU_CHK(NULL != pMgr);									\
		nRet = IDVR_MPU_STR_NO_DISKGROUP_MANAGER_INSTANCE;		\
	}															\
																\
	return nRet;													\
}																\
while (0)

//调用快照管理器获取参数
#define MPU_STR_DISKGROUP_GET(DataType, DefaultValue, Function)		\
do																\
{																\
	DataType retValue;												\
																\
	CDiskGroupManager *pMgr = CDiskGroupManager::GetInstance();		\
																\
	if (NULL != pMgr)												\
	{															\
		retValue = pMgr->Function;									\
	}															\
	else															\
	{															\
		MPU_ERR("Get CDiskGroupManager instance fail!\n");				\
		MPU_CHK(NULL != pMgr);									\
		retValue = DefaultValue;									\
	}															\
																\
	return retValue;												\
}																\
while (0)

#else

//调用盘组管理器执行动作
#define MPU_STR_DISKGROUP_CALL(x)								\
do																\
{																\
	return IDVR_OK;												\
}																\
while (0)

//调用快照管理器获取参数
#define MPU_STR_DISKGROUP_GET(DataType, DefaultValue, Function)		\
do																\
{																\
	return DefaultValue;											\
}																\
while (0)

#endif							/* #ifndef DEBUG_NOT_USE_STR_LIB */


/**************************************************************************
 *                     盘组管理                                           *
 **************************************************************************/

//!\brief   获取工作模式
//!\return  成功返回工作模式，失败为-1
int MPU_STR_DiskGroup_GetWorkMode()
{
	//MPU_DBG("enter\n");

	MPU_STR_DISKGROUP_GET(int, 0, GetWorkMode());
}

//!\brief   设置工作模式
//!\param   int nWorkMode 工作模式
//!\return  成功返回0，其它为错误码
static int STR_DiskGroup_SetWorkMode(int nWorkMode)
{
	//MPU_DBG("enter\n");

	MPU_STR_DISKGROUP_CALL(SetWorkMode(nWorkMode));
}

//!\brief       设置工作模式
//!\param   int nWorkMode 工作模式
//!\return  成功返回0，其它为错误码
int MPU_STR_DiskGroup_SetWorkMode(int nWorkMode)
{
	int nRet, nEncID;

	//模式改变则设置
	if (MPU_STR_DiskGroup_GetWorkMode() != nWorkMode)
	{
		//由高级模式切到普通模式，需要先停止录像
		if (0 == nWorkMode)
		{
			//需暂停录像
			for (nEncID = 0; nEncID < MAX_ENC_CHANNEL_NUM; ++nEncID)
			{
				MPU_AVL_Record_Pause(nEncID, TRUE);
			}
		}

		nRet = STR_DiskGroup_SetWorkMode(nWorkMode);

		if (0 == nWorkMode)
		{
			//恢复录像
			for (nEncID = 0; nEncID < MAX_ENC_CHANNEL_NUM; ++nEncID)
			{
				MPU_AVL_Record_Pause(nEncID, FALSE);
			}
		}
	}
	else
	{
		nRet = IDVR_OK;
	}

	return nRet;
}

//!\brief   提供操作盘组的接口函数
//!\brief   创建硬盘分组，主要是注册该新硬盘分组的基本信息。
//!\param   int nDiskGroupType  盘组类型。0为普通盘组，1为报警盘组，2为冗余盘组，3为备份盘组
//!\param   int nCycleMode 盘组满时是覆盖还是停止
//!\return  成功返回0，失败为-1
int MPU_STR_DiskGroup_Create(int nDiskGroupType, int nCycleMode)
{
	//MPU_DBG("enter\n");

	//调用盘组管理器实现
	MPU_STR_DISKGROUP_CALL(Create(nDiskGroupType, nCycleMode));
}

//!\brief   销毁硬盘分组。
//!\param   int nDiskGroupType  盘组类型。0为普通盘组，1为报警盘组，2为冗余盘组，3为备份盘组
//!\return  成功返回0，失败为-1
int MPU_STR_DiskGroup_Destroy(int nDiskGroupType)
{
	//MPU_DBG("enter\n");

	//调用盘组管理器实现
	MPU_STR_DISKGROUP_CALL(Destroy(nDiskGroupType));
}

//!\brief   获取磁盘组信息
//!\param   int nDiskGroupType  盘组类型。0为普通盘组，1为报警盘组，2为冗余盘组，3为备份盘组
//!\param   tagDiskGroupInfo* Info 磁盘组信息结构体指针
//!\return  成功返回0，失败为-1
int MPU_STR_DiskGroup_GetInfo(int nDiskGroupType, tagDiskGroupInfo * Info)
{
//  MPU_DBG("nDiskGroupType = %d\n", nDiskGroupType);

	//调用盘组管理器实现
	MPU_STR_DISKGROUP_CALL(GetInfo(nDiskGroupType, Info));
}

//!\brief       获取指定分区所含有的录像数据的开始时间和结束时间
//!\param   pcDevPart 分区节点号
//!\param   tBeginTime 存放开始时间
//!\param   tEndTime 存放结束时间
//!\return  成功返回0，失败返回小于0的错误码。
int MPU_STR_DiskGroup_GetDiskPartDataInfoEx(char *pcDevPart, time_t * tBeginTime, time_t * tEndTime)
{
	//MPU_DBG("enter\n");

	MPU_STR_DISKGROUP_CALL(GetDiskPartDataInfoEx(pcDevPart, tBeginTime, tEndTime));
}

//!\brief   解除绑定某通道数据至该盘组
//!\param   int nDiskGroupType  盘组类型。0为普通盘组，1为报警盘组，2为冗余盘组，3为备份盘组
//!\param   int nCh 通道
//!\return  成功返回0，失败为-1
int MPU_STR_DiskGroup_UnBindDataCH(int nDiskGroupType, int nCh, int RecType)
{
	//MPU_DBG("enter\n");

	//调用盘组管理器实现
	MPU_STR_DISKGROUP_CALL(UnBindDataCH(nDiskGroupType, nCh, RecType));
}

//!\brief   绑定某通道数据至该盘组
//!\param   int nDiskGroupType  盘组类型。0为普通盘组，1为报警盘组，2为冗余盘组，3为备份盘组
//!\param   int nCh 通道
//!\param   int RecType录像类型
//!\return  成功返回0，失败为-1
int MPU_STR_DiskGroup_BindDataCH(int nDiskGroupType, int nCh, int RecType)
{
	//MPU_DBG("enter\n");

	//调用盘组管理器实现
	MPU_STR_DISKGROUP_CALL(BindDataCH(nDiskGroupType, nCh, RecType));
}

//!\brief   添加分区至该盘组
//!\param   int nDiskGroupType  盘组类型。0为普通盘组，1为报警盘组，2为冗余盘组，3为备份盘组
//!\param char *Partition 分区
//!\return  成功返回0，失败为-1
int MPU_STR_DiskGroup_AddPartition(int nDiskGroupType, const char *Partition)
{
	//MPU_DBG("enter\n");

	//调用盘组管理器实现
	MPU_STR_DISKGROUP_CALL(AddPartition(nDiskGroupType, Partition));
}

//!\brief 删除分区至该盘组
//!\param int nDiskGroupType    盘组类型。0为普通盘组，1为报警盘组，2为冗余盘组，3为备份盘组
//!\param char *Partition 分区
//!\return  成功返回0，失败为-1
int MPU_STR_DiskGroup_DelPartition(int nDiskGroupType, const char *Partition)
{
	//MPU_DBG("enter\n");

	//调用盘组管理器实现
	MPU_STR_DISKGROUP_CALL(DelPartition(nDiskGroupType, Partition));
}

//!\brief   获取指定硬盘组中块坏分布信息，含硬盘位置，分区位置。盘组属性及数据保持时间
//!\param   int nDiskGroupType  盘组类型。0为普通盘组，1为报警盘组，2为冗余盘组，3为备份盘组
//!\param PBADBLOCKINFO pBadBlockInfo   坏块链表表头指针。
//!\return  成功返回0，失败为-1
int MPU_STR_DiskGroup_GetBadBlockInfo(int nDiskGroupType, void *pBadBlockInfo)
{
	//当前不需要实现该接口
	MPU_ERR("***** unsupported, nDiskGroupType= %d\n", nDiskGroupType);
	return (int) IDVR_ERROR_UNSUPPORT;
}

//!\brief   设置数据的保持时间
//!\param   int nDiskGroupType  盘组类型。0为普通盘组，1为报警盘组，2为冗余盘组，3为备份盘组
//!\param   int nDataKeepTime 数据保持时间
//!\return  成功返回0，失败返回小于0的错误码。
int MPU_STR_DiskGroup_DataKeepTime(int nDiskGroupType, int nDataKeepTime)
{
	//MPU_DBG("enter\n");

	//调用盘组管理器实现
	MPU_STR_DISKGROUP_CALL(Set_DataKeepTime(nDiskGroupType, nDataKeepTime));
}

//!\brief       设置盘组满回调函数
//!\param   int nType                   盘组满返回的类型
//!\param   CBAlert pCallback       回调函数句柄
//!\return  成功返回0，失败为-1
int MPU_STR_DiskGroup_SetFullCallBack(int nAlarmType, CBAlert pCallback)
{
	//MPU_DBG("enter\n");

	if (NULL == g_pMpuInfo)
	{
		return IDVR_ERROR_INVALID_POINTER;
	}

	//报警类型必须为ALERT_TYPE_DISKFULL
	if (ALERT_TYPE_DISKFULL == nAlarmType)
	{
		g_pMpuInfo->m_pCallback_DiskFull = pCallback;
		MPU_STR_DISKGROUP_CALL(SetDiskGroupFullCallBack(nAlarmType, callback_AlarmAction));
	}
	else
	{
		MPU_ERR("ALERT_TYPE_DISKFULL != nAlarmType\n");
		MPU_CHK(ALERT_TYPE_DISKFULL == nAlarmType);
		return IDVR_ERROR_PARAMETER;
	}
}

//!\brief 设置盘组异常回调函数
//!\param int nAlarmType 盘组满返回的类型
//!\param CBAlert CallBack 回调函数句柄
//!\return  成功返回0，失败为-1
int MPU_STR_DiskGroup_SetErrorCallBack(int nAlarmType, CBAlert pCallback)
{
	//MPU_DBG("enter\n");

	if (NULL == g_pMpuInfo)
	{
		return IDVR_ERROR_INVALID_POINTER;
	}

	//报警类型必须为ALERT_TYPE_DISKFULL
	if (ALERT_TYPE_DISKGROUP_ERROR == nAlarmType)
	{
		g_pMpuInfo->m_pCallback_DiskGroupError = pCallback;
		MPU_STR_DISKGROUP_CALL(SetDiskGroupErrorCallBack(nAlarmType, callback_AlarmAction));
	}
	else
	{
		MPU_ERR("ALERT_TYPE_DISKGROUP_ERROR != nAlarmType\n");
		MPU_CHK(ALERT_TYPE_DISKGROUP_ERROR == nAlarmType);
		return IDVR_ERROR_PARAMETER;
	}
}

//!\brief 开启/禁止某个报警联动动作
//!\praram int nAlarmType 报警类型。VIDEO_LOST 
//!\praram int nActionType
//!      ACTION_SCREEN 屏幕显示
//!      ACTION_BEEP 声音提示
//!      ACTION_ALARMOUT 报警输出
//!      ACTION_SENDTOSERVER 上传报警服务器
//!      ACTION_EMAIL 联动Email
//!      ACTION_SMS 联动短信
//!\bool bEnahle true为开启，false为禁止
//!\return 成功为0，失败为错误码
int MPU_STR_DiskGroup_EnableAction(int nAlarmType, tagActionType nActionType, bool bEnahle)
{
	//MPU_DBG("enter\n");

	MPU_STR_DISKGROUP_CALL(EnableAction(nAlarmType, nActionType, bEnahle));
}

//!\brief 判断某个报警联动动作是否开启
//!\praram int nAlarmType 报警类型。
//!\praram int nActionType
//!      ACTION_SCREEN 屏幕显示
//!      ACTION_BEEP 声音提示
//!      ACTION_ALARMOUT 报警输出
//!      ACTION_SENDTOSERVER 上传报警服务器
//!      ACTION_EMAIL 联动Email
//!      ACTION_SMS 联动短信
//!\return false 为禁止联动 true 为开启联动
bool MPU_STR_DiskGroup_IsEnableAction(int nAlarmType, tagActionType nActionType)
{
	//MPU_DBG("enter\n");
	//特别注意，添加调试信息后，系统启动失败

	MPU_STR_DISKGROUP_GET(bool, false, IsEnableAction(nAlarmType, nActionType));
}

//!\brief 设置某个联动动作涉及的通道。无通道概念的不需要设置
//!\param int nCh 通道
//!\praram int nAlarmType 报警类型
//!\praram tagActionType nActionType   动作类型
//!\praram int nAlarmCh 联动动作涉及到通道。
//!\praram bool bEnable 启用/不启用该通道的配置
//!\praram int dwReserved 保留字。对于预置点、轨迹、巡航线路输入上述编号
//!\return  成功为0，失败为-1
int MPU_STR_DiskGroup_SetActionChParam(int nAlarmType, tagActionType nActionType, int nAlarmCh, bool bEnable, int nReserved)
{
	//MPU_DBG("enter\n");

	MPU_STR_DISKGROUP_CALL(SetActionChParam(nAlarmType, nActionType, nAlarmCh, bEnable, nReserved));
}

//!\brief 获取某个联动动作涉及的通道或者编号
//!\praram int AlarmType 报警类型
//!\praram tagActionType Action 动作类型
//!\praram bool *szCh 通道数组，true为联动了该通道，false为没有联动该通道
//!\praram int *szReserved 对应通道涉及到的子类型数据。目前仅相对于预置点编号等
//!\return  成功为0，失败为-1
int MPU_STR_DiskGroup_GetActionChParam(int nAlarmType, tagActionType nActionType, bool * szCh, int *szReserved)
{
	//MPU_DBG("enter\n");

	MPU_STR_DISKGROUP_CALL(GetActionChParam(nAlarmType, nActionType, szCh, szReserved));
}

//!\brief 获取指定盘组的剩余容量(单位：MB)
//!\param int nDiskGroupType 盘组类型。0为普通盘组，1为报警盘组，2为冗余盘组，3为备份盘组
//!\param hDiskGroup 磁盘分组句柄
//!\param pnSize 总容量
//!\return 成功返回0，失败返回小于0的错误码。
int MPU_STR_DiskGroup_GetRemainSize(const unsigned int nDiskGroupType, int *pnSize)
{
	//MPU_DBG("enter\n");

	MPU_STR_DISKGROUP_CALL(GetDiskGroupRemainSize(nDiskGroupType, pnSize));
}

#undef MPU_STR_DISKGROUP_CALL	//调用盘组管理器执行动作
