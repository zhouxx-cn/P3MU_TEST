
/*!
 *\file		MPU_BAK_API.cpp
 *\brief	MPU模块，备份相关接口实现
 *\author	林云
 *\date		2009/03/20

功能：
-#	录像文件备份（按时间段、多通道的备份方式）
-#	支持多种备份格式（H.264 raw格式、私有格式、AVI格式（可选））
-#	支持多种备份设备（CDROM、备份盘组、USB盘）
-#	提供回调告知方式的进度信息

说明：
-#	使用流存储文件系统时，CDRom刻录暂时无法实现
-#	使用备份盘组备份，需要流存储文件系统支持，暂时无法实现
 */


#include "MPU_global.h"


#ifndef DEBUG_NOT_USE_BAK_LIB	//调试时，不调用BAK库

#include "BAK/CBakManager.h"


/**************************************************************************
 *                            局部宏定义                                  *
 **************************************************************************/

//调用备份管理器执行动作
#define MPU_BAK_CALL(x)										\
do															\
{															\
	int nRet;													\
															\
	CBakManager *pMgr = (CBakManager *)hBakManager;				\
															\
	if (NULL != pMgr)											\
	{														\
		nRet = pMgr->x;										\
		if (IDVR_OK != nRet)									\
		{													\
			MPU_ERR("%s, nRet = %d\n", #x, nRet);				\
		}													\
	}														\
	else														\
	{														\
		MPU_ERR("Get CBakManager instance fail!\n");				\
		MPU_CHK(NULL != pMgr);								\
		nRet = IDVR_MPU_BAK_NO_MANAGER_INSTANCE;				\
	}														\
															\
	return nRet;												\
}															\
while (0)

//调用备份管理器执行动作
#define MPU_AUTO_BAK_CALL(x)								\
do															\
{															\
	int nRet;													\
															\
	nRet = CBakManager::x;										\
															\
	if (IDVR_OK != nRet)										\
	{														\
		MPU_ERR("call CBakManager::%s, nRet = %d\n", #x, nRet);		\
	}														\
															\
	return nRet;												\
}															\
while (0)

//调用备份管理器获取参数
#define MPU_BAK_GET(DataType, DefaultValue, Function)			\
do															\
{															\
	DataType retValue;											\
															\
	CBakManager *pMgr = (CBakManager *)hBakManager;				\
															\
	if (NULL != pMgr)											\
	{														\
		retValue = pMgr->Function;								\
	}														\
	else														\
	{														\
		MPU_ERR("Get CBakManager instance fail!\n");				\
		MPU_CHK(NULL != pMgr);								\
		retValue = DefaultValue;								\
	}														\
															\
	return retValue;											\
}															\
while (0)


#else

//调用备份管理器执行动作
#define MPU_BAK_CALL(x)										\
do															\
{															\
	return IDVR_OK;											\
}															\
while (0)

//调用备份管理器执行动作
#define MPU_AUTO_BAK_CALL(x)								\
do															\
{															\
	return IDVR_OK;											\
}															\
while (0)

//调用备份管理器获取参数
#define MPU_BAK_GET(DataType, DefaultValue, Function)			\
do															\
{															\
	return DefaultValue;										\
}															\
while (0)


#endif							/* #ifndef DEBUG_NOT_USE_AVP_LIB */


/**************************************************************************
 *                            全局函数                                    *
 **************************************************************************/

//!\brief   创建备份管理器
//!\param   nDevType    备份目的设置类型，0表示盘组备份 1表示U盘备份 2表示CDROM备份，见PUB/STREAM_TYPE.h
//!\return  成功返回管理器句柄，失败返回NULL
HANDLE MPU_BAK_CreateManager(int nDevType)
{
#ifndef DEBUG_NOT_USE_BAK_LIB	//调试时，不调用BAK库
	return (HANDLE) CBakManager::GetInstance(nDevType);
#else
	return IDVR_OK;
#endif							/* #ifndef DEBUG_NOT_USE_AVP_LIB */
}

//!\brief   设置备份输出文件类型
//!\param   hBakManager     备份管理器句柄
//!\return  无
void MPU_BAK_DestroyManager(HANDLE hBakManager)
{
#ifndef DEBUG_NOT_USE_BAK_LIB	//调试时，不调用BAK库
	CBakManager::DestroyInstance((CBakManager *) hBakManager);
#endif							/* #ifndef DEBUG_NOT_USE_AVP_LIB */
}

//!\brief   设置备份输出文件类型
//!\param   hBakManager     备份管理器句柄
//!\param   nFileType   文件类型（0表示私有格式、1表示h.264 raw格式）
//!\return  0表示成功，其它为错误码
int MPU_BAK_SetOutFileType(HANDLE hBakManager, int nFileType)
{
	//调用备份管理器接口执行动作
	MPU_BAK_CALL(SetOutFileType(nFileType));
}

//!\brief   设置要备份数据的盘组
//!\param   hBakManager     备份管理器句柄
//!\param   nChannelMask    盘组标记（按位表示）
//!\return  0表示成功，其它为错误码
int MPU_BAK_SetDiskgroupMask(HANDLE hBakManager, int nDiskGroupMask)
{
	//调用备份管理器接口执行动作
	MPU_BAK_CALL(SetDiskgroupMask(nDiskGroupMask));
}

//!\brief   设置备份通道
//!\param   hBakManager     备份管理器句柄
//!\param   nChannelMask    通道标记（按位表示）
//!\return  0表示成功，其它为错误码
int MPU_BAK_SetBackupChannel(HANDLE hBakManager, int nChannelMask)
{
	//调用备份管理器接口执行动作
	MPU_BAK_CALL(SetBackupChannel(nChannelMask));
}

//!\brief 添加录像类型
//!\param   hBakManager     备份管理器句柄
//!\param   nRecType        要添加的录像类型
//! RECORD_TYPE_ALARM 报警联动录像
//! RECORD_TYPE_GENERAL 普通类型
//! RECORD_TYPE_MANUAL 手动录像类型
//! RECORD_TYPE_MOTION 移动录像
//! RECORD_TYPE_HUMANSHAPE 人形录像
//!\return 0表示成功 其他表示错误码
int MPU_BAK_EnableRecordType(HANDLE hBakManager, int nRecType, int bEnable)
{
	//调用备份管理器接口执行动作
	MPU_BAK_CALL(EnableRecordType(nRecType, bEnable));
}

//!\brief   设置备份开始时间
//!\param   hBakManager     备份管理器句柄
//!\param   nStartTime  开始时间为绝对时间，单位为秒
//!\return  0表示成功，其它为错误码
int MPU_BAK_SetStartTime(HANDLE hBakManager, time_t tStartTime)
{
	//调用备份管理器接口执行动作
	MPU_BAK_CALL(SetStartTime(tStartTime));
}

//!\brief   设置备份通道
//!\param   hBakManager     备份管理器句柄
//!\param   tStopTime   结束时间为绝对时间，单位为秒
//!\return  0表示成功，其它为错误码
int MPU_BAK_SetStopTime(HANDLE hBakManager, time_t tStopTime)
{
	//调用备份管理器接口执行动作
	MPU_BAK_CALL(SetStopTime(tStopTime));
}

//!\brief   设置备份状态通知回调函数
//!\param   hBakManager     备份管理器句柄
//!\param   pBackupCallback     回调函数指针
//!\return  0表示成功，其它为错误码
int MPU_BAK_SetCallback(HANDLE hBakManager, HY_BACKUP_CALLBACK pBackupCallback)
{
	//调用备份管理器接口执行动作
	MPU_BAK_CALL(SetCallback(pBackupCallback));
}

//!\brief 设置USB设备的索引值,仅供USB备份时使用
//!\param   hBakManager     备份管理器句柄
//!\param tDeviceInfo 设备信息
//!\param nPart 分区编号,从0开始
//!\return 0表示设置成功 其他表示错误码
int MPU_BAK_SetDevice(HANDLE hBakManager, DeviceInfo & tDeviceInfo, int nPart)
{
	//调用备份管理器接口执行动作
	MPU_BAK_CALL(SetDevice(tDeviceInfo, nPart));
}

//!\brief   启动备份任务
//!\param   hBakManager      备份管理器句柄
//!\return  0表示成功，其它为错误码
int MPU_BAK_Start(HANDLE hBakManager)
{
	//调用备份管理器接口执行动作
	MPU_BAK_CALL(Start());
}

//!\brief   停止当前备份任务
//!\param   hBakManager      备份管理器句柄
//!\return  0表示成功，其它为错误码
int MPU_BAK_Stop(HANDLE hBakManager)
{
	//调用备份管理器接口执行动作
	MPU_BAK_CALL(Stop());
}

//!\brief   重试刻录
//!\param   hBakManager 备份管理器句柄
//!\return  0表示成功，其它为错误码
int MPU_BAK_Retry(HANDLE hBakManager)
{
	//调用备份管理器接口执行动作
	MPU_BAK_CALL(Retry());
}

//!\brief   获取当前备份状态
//!\param   hBakManager      备份管理器句柄
//!\return  状态值
int MPU_BAK_GetState(HANDLE hBakManager)
{
	//调用备份管理器接口获取参数
	MPU_BAK_GET(int, 0, State());
}

//!\brief 获取要备份的数据的容量
//!\param pdwSize 获得的容量
//!\param nBakType 备份类型 0表示盘组备份 1表示USB备份 2表示CDROM备份
//!\param nFileType 备份文件类型，盘组备份时忽略此参数 STREAM_TYPE_AVI表示AVI STREAM_TYPE_H264表示H264文件
//!\param tStartTime 备份起始时间
//!\param tEndTime 备份结束时间
//!\param nChanMask 通道掩码
//!\param nRecTypeMask 录像类型掩码
//!\param nDiskGroupMask 盘组掩码
//!\return 0表示获取成功 -1表示获取失败
int
MPU_BAK_GetSize(unsigned long long *pdwSize, int nBakType, int nFileType,
				time_t tStartTime, time_t tEndTime, int nChanMask, int nRecTypeMask, int nDiskGroupMask)
{
#ifndef DEBUG_NOT_USE_BAK_LIB	//调试时，不调用BAK库
	return CBakManager::GetSize(pdwSize, nBakType, nFileType, tStartTime, tEndTime, nChanMask, nRecTypeMask, nDiskGroupMask);
#else
	return -1;
#endif
}


/*****************************************************************************
 *						自动备份到备份盘组的处理
 ****************************************************************************/

//!\brief   设置自动备份的回调
//!\return  0表示设置成功 其他表示错误码
int MPU_BAK_Auto_SetCallback(HY_BACKUP_CALLBACK cbBackup)
{
	MPU_AUTO_BAK_CALL(Auto_SetCallback(cbBackup));
}

//!\brief   启用自动备份
//!\param   bAuto       是否启用自动备份
//!\return  0表示设置成功 其他表示错误码
int MPU_BAK_Auto_EnableAuto(int bAuto)
{
	MPU_AUTO_BAK_CALL(Auto_EnableAuto(bAuto));
}

//!\brief   设置自动备份周期
//!\param   nCycle      0表示每天 1表示每周
//!\return  0表示设置成功 其他表示错误码
int MPU_BAK_Auto_SetCycle(int nCycle)
{
	MPU_AUTO_BAK_CALL(Auto_SetCycle(nCycle));
}

//!\brief   设置启用自动备份的通道
//!\param   nChannelMask    需要备份的通道的掩码
//!\return  0表示设置成功 其他表示错误码
int MPU_BAK_Auto_SetChannelMask(int nChannelMask)
{
	MPU_AUTO_BAK_CALL(Auto_SetChannelMask(nChannelMask));
}

//!\brief   添加和删除自动备份的录像的类型
//!\param   nRecType    要备份的录像类型
//!\return  0表示设置成功 其他表示错误码
int MPU_BAK_Auto_EnableRecordType(int nRecType, int bEnable)
{
	MPU_AUTO_BAK_CALL(Auto_EnableRecordType(nRecType, bEnable));
}

//!\brief   设置自动备份开始时间
//!\param   nWeekDay    当备份周期为天时,忽略该参数
//!\param   nHour       备份时间
//!\param   nMinute     备份时间
//!\return  0表示设置成功 其他表示错误码
int MPU_BAK_Auto_SetStartTime(int nWeekDay, int nHour, int nMinute)
{
	MPU_AUTO_BAK_CALL(Auto_SetStartTime(nWeekDay, nHour, nMinute));
}

//!\brief   设置自动备份结束时间
//!\param   nWeekDay    当备份周期为天时,忽略该参数
//!\param   nHour       备份时间
//!\param   nMinute     备份时间
//!\return  0表示设置成功 其他表示错误码
int MPU_BAK_Auto_SetStopTime(int nWeekDay, int nHour, int nMinute)
{
	MPU_AUTO_BAK_CALL(Auto_SetStopTime(nWeekDay, nHour, nMinute));
}

//!\brief   设置自动备份执行时间
//!\param   nWeekDay    当备份周期为天时,忽略该参数
//!\param   nHour       备份时间
//!\param   nMinute     备份时间
//!\return  0表示设置成功 其他表示错误码
int MPU_BAK_Auto_SetExecTime(int nWeekDay, int nHour, int nMinute)
{
	MPU_AUTO_BAK_CALL(Auto_SetExecTime(nWeekDay, nHour, nMinute));
}


/**************************************************************************
 *                            注销局部宏定义                              *
 **************************************************************************/

#undef MPU_BAK_CALL				//调用备份管理器执行动作
#undef MPU_AUTO_BAK_CALL		//调用备份管理器执行动作
#undef MPU_BAK_GET				//调用备份管理器获取参数值
