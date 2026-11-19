
/*!
 *\file		MPU_STR_Disk.cpp
 *\brief	MPU模块，磁盘管理相关接口实现
 *\author	林云
 *\date		2009/04/07

功能：
-#	获取系统磁盘信息（安装磁盘个数、是否新硬盘）
-#	盘组、数据的绑定，数据的查询、存储、读取等
 */


#include "MPU_global.h"

#include "STR/CDiskManager.h"

/**************************************************************************
 *                     声明局部宏定义                                     *
 **************************************************************************/

#ifndef DEBUG_NOT_USE_STR_LIB	//调试时，不使用STR库

//调用磁盘管理器执行动作(更新verify mark)
#define MPU_STR_DISK_CALL_VERIFY(index,x)				\
do														\
{														\
	int nRet;												\
														\
	CDiskManager *pMgr = CDiskManager::GetInstance();			\
														\
	if (NULL != pMgr)										\
	{													\
		nRet = pMgr->x;									\
		if (IDVR_OK == nRet)								\
		{												\
			mpu_DiskVerifyRefresh(index);					\
		}												\
		else												\
		{												\
			MPU_ERR("%s, return %d\n", #x, nRet);			\
		}												\
	}													\
	else													\
	{													\
		MPU_ERR("Get CDiskManager instance fail!\n");			\
		MPU_CHK(NULL != pMgr);							\
		nRet = IDVR_MPU_STR_NO_DISK_MANAGER_INSTANCE;		\
	}													\
														\
	return nRet;											\
}														\
while (0)

//调用磁盘管理器执行动作
#define MPU_STR_DISK_CALL(x)													\
do																			\
{																			\
	int nRet;																	\
																			\
	CDiskManager *pMgr = CDiskManager::GetInstance();								\
																			\
	if (NULL != pMgr)															\
	{																		\
		nRet = pMgr->x;														\
		if (IDVR_OK != nRet)													\
		{																	\
			MPU_ERR("%s, return %d\n", #x, nRet);								\
		}																	\
	}																		\
	else																		\
	{																		\
		MPU_ERR("Get CDiskManager instance fail!\n");								\
		MPU_CHK(NULL != pMgr);												\
		nRet = IDVR_MPU_STR_NO_DISK_MANAGER_INSTANCE;							\
	}																		\
																			\
	return nRet;																\
}																			\
while (0)

//调用快照管理器获取参数
#define MPU_STR_DISK_GET(DataType, DefaultValue, Function)						\
do																			\
{																			\
	DataType retValue;															\
																			\
	CDiskManager *pMgr = CDiskManager::GetInstance();								\
																			\
	if (NULL != pMgr)															\
	{																		\
		retValue = pMgr->Function;												\
	}																		\
	else																		\
	{																		\
		MPU_ERR("Get CSnapshotManager instance fail!\n");							\
		MPU_CHK(NULL != pMgr);												\
		retValue = DefaultValue;												\
	}																		\
																			\
	return retValue;															\
}																			\
while (0)

#else

//调用磁盘管理器执行动作(更新verify mark)
#define MPU_STR_DISK_CALL_VERIFY(index,x)									\
do																			\
{																			\
	return IDVR_OK;															\
}																			\
while (0)

//调用磁盘管理器执行动作
#define MPU_STR_DISK_CALL(x)													\
do																			\
{																			\
	return IDVR_OK;															\
}																			\
while (0)

#define MPU_STR_DISK_GET(DataType, DefaultValue, Function)						\
do																			\
{																			\
	return DefaultValue;														\
}																			\
while (0)

#endif							/* #ifndef DEBUG_NOT_USE_STR_LIB */


//定义verify mark相关变量
static unsigned int m_MPU_DiskVerifyMark[MPU_MAX_SUPPORT_DISKNUM];	//初始化时随机值即可

static int m_MPU_DiskVerifyMarkCnt = 0;

static int mpu_DiskVerifyRefresh(int nDiskIndex)
{
	time_t currTime;

	unsigned int mark;

	if (nDiskIndex < 0 || nDiskIndex >= MPU_MAX_SUPPORT_DISKNUM)
	{
		return -1;
	}

	currTime = time(NULL);
	mark = (unsigned int) (currTime & 0xFFFF);
	m_MPU_DiskVerifyMark[nDiskIndex] = ((mark << 8) | nDiskIndex | (m_MPU_DiskVerifyMarkCnt << 24));

	m_MPU_DiskVerifyMarkCnt++;
	if (m_MPU_DiskVerifyMarkCnt > 127)
	{
		m_MPU_DiskVerifyMarkCnt = 0;
	}

	return 0;
}

//!\brief 获取某硬盘当前的改动验证码
//!\param int nDiskIndex 硬盘索引号
//!\return 返回验证码
unsigned int MPU_STR_Disk_GetVerifyMark(int nDiskIndex)
{
	if (nDiskIndex < 0 || nDiskIndex >= MPU_MAX_SUPPORT_DISKNUM)
	{
		return 0x7FFFFFFF;
	}

	if (m_MPU_DiskVerifyMark[nDiskIndex] == 0)
	{
		mpu_DiskVerifyRefresh(nDiskIndex);
	}

	return m_MPU_DiskVerifyMark[nDiskIndex];
}

/**************************************************************************
 *                     磁盘管理                                           *
 **************************************************************************/

//!\brief   获取硬盘个数
//!\param   int *pnNum  返回磁盘个数
//!\return  成功返回0，其它为错误码
int MPU_STR_Disk_GetNum(int *pnNum)
{
	//MPU_DBG("enter\n");

	//调用CDiskManager接口实现
	MPU_STR_DISK_CALL(GetNum(pnNum));
}

//!\brief   获取磁盘物理位置编号
//!\param   int nIndex      硬盘索引号
//!\param   int *pBusNo     硬盘物理位置编号
//!\return  成功返回0，其它为错误码
int MPU_STR_Disk_GetBusNumber(const unsigned int nIndex, int *pBusNo)
{
	//MPU_DBG("enter\n");

	//调用CDiskManager接口实现
	MPU_STR_DISK_CALL(GetBusNumber(nIndex, pBusNo));
}

//!\brief   判断硬盘是否为新硬盘
//!\param   int nIndex  硬盘索引号
//!\param   bool *pbNew 返回是否为新硬盘
//!\return  成功返回0，其它为错误码
int MPU_STR_Disk_IsNew(const unsigned int nIndex, bool * pbNew)
{
	//MPU_DBG("enter\n");

	//调用CDiskManager接口实现
	MPU_STR_DISK_CALL(IsNew(nIndex, pbNew));
}

//!\brief   获取磁盘信息
//!\param   tagDiskInfo *Info 硬盘信息结构体指针    
//!\param   int nIndex 硬盘索引号
//!\return  成功返回0，其它为错误码
int MPU_STR_Disk_GetInfo(int nIndex, tagDiskInfo * Info)
{
	//MPU_DBG("enter\n");

	//调用CDiskManager接口实现
	MPU_STR_DISK_CALL(GetInfo(nIndex, Info));
}

//!\brief   获取对应磁盘对应分区的信息
//!\param   tagPartition *Info 硬盘信息结构体指针   
//!\param   int nPartitionIndex 分区在硬盘中的索引值
//!\param   int nDiskIndex 硬盘索引号
//!\return  成功返回0，其它为错误码
int MPU_STR_Disk_GetPartitionInfo(int nDiskIndex, int nPartitionIndex, tagPartition * Info)
{
	//MPU_DBG("enter\n");

	//调用CDiskManager接口实现
	MPU_STR_DISK_CALL(GetPartitionInfo(nDiskIndex, nPartitionIndex, Info));
}

//!\brief   格式化一个分区
//!\param   int nPartitionIndex 分区在硬盘中的索引值
//!\param   int nDiskIndex 硬盘索引号
//!\return  成功返回0，其它为错误码
int MPU_STR_Disk_FormatPartition(int nDiskIndex, int nPartitionIndex)
{
	//MPU_DBG("enter\n");

	//调用CDiskManager接口实现
	MPU_STR_DISK_CALL_VERIFY(nDiskIndex, FormatPartition(nDiskIndex, nPartitionIndex));
}

//!\brief   卸载该硬盘
//!\param   int nPartitionIndex 分区在硬盘中的索引值
//!\param   int nDiskIndex 硬盘索引号
//!\return  成功返回0，其它为错误码
int MPU_STR_Disk_UnMount(int nIndex)
{
	//MPU_DBG("enter\n");

	//调用CDiskManager接口实现
	MPU_STR_DISK_CALL_VERIFY(nIndex, UnMount(nIndex));
}

//!\brief   创建一个分区
//!\param   int nDiskIndex 硬盘索引号
//!\param   unsigned long nSize 分区大小
//!\return  成功返回0，其它为错误码
int MPU_STR_Disk_CreatePartition(int nDiskIndex, unsigned long nSize)
{
	//MPU_DBG("enter\n");

	//调用CDiskManager接口实现
	MPU_STR_DISK_CALL_VERIFY(nDiskIndex, CreatePartition(nDiskIndex, nSize));
}

//!\brief   创建一个分区
//!\param   int nDiskIndex 硬盘索引号
//!\param   int nPartitionIndex 分区在硬盘中的索引值
//!\return  成功返回0，其它为错误码
int MPU_STR_Disk_DestroyPartition(int nDiskIndex, int nIndexPartition)
{
	//MPU_DBG("enter\n");

	//调用CDiskManager接口实现
	MPU_STR_DISK_CALL_VERIFY(nDiskIndex, DestroyPartition(nDiskIndex, nIndexPartition));
}

//!\brief   设置硬盘异常通知回调函数
//!\param   int nAlarmType      硬盘错误类型
//!\param   CBAlert pCallback   回调指针
//!\return  成功返回0，其它为错误码
int MPU_STR_Disk_SetErrorNotifyCallback(int nAlarmType, CBAlert pCallback)
{
	//MPU_DBG("enter\n");

	if (NULL == g_pMpuInfo)
	{
		return IDVR_ERROR_INVALID_POINTER;
	}

	//报警类型必须为ALERT_TYPE_DISKERROR
	if (ALERT_TYPE_DISKERROR == nAlarmType)
	{
		g_pMpuInfo->m_pCallback_DiskError = pCallback;
		MPU_STR_DISK_CALL(SetDiskErrorCallBack(nAlarmType, callback_AlarmAction));
	}
	else
	{
		MPU_ERR("ALERT_TYPE_DISKERROR != nAlarmType\n");
		MPU_CHK(ALERT_TYPE_DISKERROR == nAlarmType);
		return IDVR_ERROR_PARAMETER;
	}
}

//!\brief       开启/禁止某个报警联动动作
//!\param   int AlarmType   报警类型。
//!\param   int Action
//!      ACTION_SCREEN 屏幕显示
//!     ACTION_BEEP 声音提示
//!     ACTION_ALARMOUT 报警输出
//!     ACTION_SENDTOSERVER 上传报警服务器
//!     ACTION_EMAIL 联动Email
//!     ACTION_SMS 联动短信
//!\param   bool bEnahle    true为开启，false为禁止
//!\return  成功为0，失败为错误码
int MPU_STR_Disk_EnableAction(int AlarmType, tagActionType Type, bool bEnahle)
{
	//MPU_DBG("enter\n");

	MPU_STR_DISK_CALL(EnableAction(AlarmType, Type, bEnahle));
}

//!\brief       判断某个报警联动动作是否开启
//!\param   int AlarmType   报警类型。
//!\param int Action
//!     ACTION_SCREEN                       屏幕显示
//!     ACTION_BEEP                         声音提示
//!     ACTION_ALARMOUT                 报警输出
//!     ACTION_SENDTOSERVER         上传报警服务器
//!     ACTION_EMAIL                            联动Email
//!     ACTION_SMS                          联动短信
//!\return  false 为禁止联动 true 为开启联动
bool MPU_STR_Disk_IsEnableAction(int AlarmType, tagActionType Type)
{
//  MPU_DBG("AlarmType = %d, ActionType = 0x%X\n", AlarmType, Type);

	MPU_STR_DISK_GET(bool, false, IsEnableAction(AlarmType, Type));
}

//!\brief       设置某个联动动作涉及的通道。无通道概念的不需要设置
//!\param   int nAlarmType              报警类型
//!\param   tagActionType Action        动作类型
//!\param   int nAlarmCh                联动动作涉及到通道。
//!\param   bool bEnable                启用/不启用该通道的配置
//!\param   int dwReserved          保留字。对于预置点、轨迹、巡航线路输入上述编号
//!\return   成功为0，失败为-1
int MPU_STR_Disk_SetActionChParam(int nAlarmType, tagActionType Action, int nAlarmCh, bool bEnable, int nReserved)
{
	//MPU_DBG("enter\n");

	MPU_STR_DISK_CALL(SetActionChParam(nAlarmType, Action, nAlarmCh, bEnable, nReserved));
}

//!\brief       获取某个联动动作涉及的通道或者编号
//!\param   int nAlarmType              报警类型
//!\param   tagActionType Action        动作类型
//!\param   bool *pbChEnable            通道数组，true为联动了该通道，false为没有联动该通道
//!\param   int *pnReserved         对应通道涉及到的子类型数据。目前仅相对于预置点编号等
//!\return  成功为0，失败为-1
int MPU_STR_Disk_GetActionChParam(int nAlarmType, tagActionType Action, bool * pbChEnable, int *pnReserved)
{
	//MPU_DBG("enter\n");

	MPU_STR_DISK_CALL(GetActionChParam(nAlarmType, Action, pbChEnable, pnReserved));
}

/*@brief MPU_STR_Disk_GetDiskStatus 获取磁盘状态
 *@return 见下表
  eZWDiskStatus_Idle,
  eZWDiskStatus_Writting,
  eZWDiskStatus_Reading,
  eZWDiskStatus_Exception,
  eZWDiskStatus_Error,
  eZWDiskStatus_Readonly,
  eZWDiskStatus_Formatting,
  eZWDiskStatus_Warn,
  eZWDiskStatus_ReadWrite,
  eZWDiskStatus_NoDisk, // 无磁盘
  eZWDiskStatus_ExpNeedFormat, // 磁盘异常需要格式化
  eZWDiskStatus_ExpNeedChangeDisk, // 磁盘异常需要换卡
  eZWDiskStatus_FormatFailNeedReboot // 磁盘格式化失败需要重启
 */
int MPU_STR_Disk_GetDiskStatus(const unsigned int nIndex)
{
	MPU_STR_DISK_GET(int, 9, GetDiskStatus(nIndex));
}

//!\brief       初始化硬盘
//!\param   int nBusNo  硬盘物理位置编号
//!\return  成功为0，失败为-1
int MPU_STR_Disk_InitDisk(const unsigned int nBusNo)
{
	//MPU_DBG("enter\n");

	//调用CDiskManager接口实现
	MPU_STR_DISK_CALL(InitDisk(nBusNo));
}

/*@ brief 获取硬盘当前格式化的状态
 *@ param int nIndex 硬盘索引号
 *@ return  格式化状态:
 *@                  0: 未格式化;
 *@                  1: 格式化中
 *@                  2: 格式化成功
 *@                  3: 格式化失败
 *@                  4: 卸载失败
 *@                  5: 设备忙
 *@                  6: 无SD卡
 */
int MPU_STR_Disk_GetDiskFormatStatus(const unsigned int nIndex)
{
	//MPU_DBG("enter\n");

	MPU_STR_DISK_GET(int, 0, GetDiskFormatStatus(nIndex));
}

//!\brief 获取当前硬盘写的状态
//!\param int nIndex 硬盘索引号
//!\return  当前硬盘写状态: 0.SD卡空闲, 1.SD卡正在写, 2.SD卡写完成, 3.SD卡写失败
int MPU_STR_Disk_GetDiskWriteStatus(const unsigned int nIndex)
{
	//MPU_DBG("enter\n");

	MPU_STR_DISK_GET(int, 0, GetDiskWriteStatus(nIndex));
}

int MPU_STR_Disk_PauseSdRecord()
{
    MPU_STR_DISK_CALL(PauseSDRecord());
}

int MPU_STR_Disk_ResumeSdRecord()
{
    MPU_STR_DISK_CALL(ResumeSDRecord());
}

/**************************************************************************
 *                     注销局部宏定义                                     *
 **************************************************************************/
#undef MPU_STR_DISK_CALL		//调用磁盘管理器执行动作
