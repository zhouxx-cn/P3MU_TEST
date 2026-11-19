
/*!
 *\file		MPU_BAK_API.h
 *\brief	MPU模块，备份相关接口定义
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

#ifndef _INC_MPU_BAK_API_H_
#define _INC_MPU_BAK_API_H_


#include <time.h>
#include "usb.h"
#include "CBBackup.h"


/*****************************************************************************
 *                                   备份                                    *
 *****************************************************************************/

//!\brief   创建备份管理器
//!\param   nDevType    备份目的设置类型，备份目的设置类型，0表示盘组备份 1表示U盘备份 2表示CDROM备份，见PUB/STREAM_TYPE.h
//!\return  成功返回管理器句柄，失败返回NULL
HANDLE MPU_BAK_CreateManager(int nDevType);

//!\brief   设置备份输出文件类型
//!\param   hBakManager     备份管理器句柄
//!\return  无
void MPU_BAK_DestroyManager(HANDLE hBakManager);

//!\brief   设置备份输出文件类型
//!\param   hBakManager     备份管理器句柄
//!\param   nFileType   文件类型（0表示私有格式、1表示h.264 raw格式）
//!\return  0表示成功，其它为错误码
int MPU_BAK_SetOutFileType(HANDLE hBakManager, int nFileType);

//!\brief   设置要备份数据的盘组
//!\param   hBakManager     备份管理器句柄
//!\param   nChannelMask    盘组标记（按位表示）
//!\return  0表示成功，其它为错误码
int MPU_BAK_SetDiskgroupMask(HANDLE hBakManager, int nDiskGroupMask);

//!\brief   设置备份通道
//!\param   hBakManager     备份管理器句柄
//!\param   nChannelMask    通道标记（按位表示）
//!\return  0表示成功，其它为错误码
int MPU_BAK_SetBackupChannel(HANDLE hBakManager, int nChannelMask);

//!\brief 添加录像类型
//!\param   hBakManager     备份管理器句柄
//!\param   nRecType        要添加的录像类型
//! RECORD_TYPE_ALARM 报警联动录像
//! RECORD_TYPE_GENERAL 普通类型
//! RECORD_TYPE_MANUAL 手动录像类型
//! RECORD_TYPE_MOTION 移动录像
//! RECORD_TYPE_HUMANSHAPE 人形录像
//!\return 0表示成功 其他表示错误码
int MPU_BAK_EnableRecordType(HANDLE hBakManager, int nRecType, int bEnable);

//!\brief   设置备份开始时间
//!\param   hBakManager     备份管理器句柄
//!\param   nStartTime  开始时间为绝对时间，单位为秒
//!\return  0表示成功，其它为错误码
int MPU_BAK_SetStartTime(HANDLE hBakManager, time_t tStartTime);

//!\brief   设置备份通道
//!\param   hBakManager     备份管理器句柄
//!\param   tStopTime   结束时间为绝对时间，单位为秒
//!\return  0表示成功，其它为错误码
int MPU_BAK_SetStopTime(HANDLE hBakManager, time_t tStopTime);

//!\brief   设置备份状态通知回调函数
//!\param   hBakManager     备份管理器句柄
//!\param   pBackupCallback     回调函数指针
//!\return  0表示成功，其它为错误码
int MPU_BAK_SetCallback(HANDLE hBakManager, HY_BACKUP_CALLBACK pBackupCallback);

//!\brief 设置USB设备的索引值,仅供USB备份时使用
//!\param   hBakManager     备份管理器句柄
//!\param tDeviceInfo 设备信息
//!\param nPart 分区编号,从0开始
//!\return 0表示设置成功 其他表示错误码
int MPU_BAK_SetDevice(HANDLE hBakManager, DeviceInfo & tDeviceInfo, int nPart);

//!\brief   启动备份任务
//!\param   hBakManager      备份管理器句柄
//!\return  0表示成功，其它为错误码
int MPU_BAK_Start(HANDLE hBakManager);

//!\brief   停止当前备份任务
//!\param   hBakManager      备份管理器句柄
//!\return  0表示成功，其它为错误码

int MPU_BAK_Stop(HANDLE hBakManager);

//!\brief   重试刻录
//!\param   hBakManager 备份管理器句柄
//!\return  0表示成功，其它为错误码
int MPU_BAK_Retry(HANDLE hBakManager);

//!\brief   获取当前备份状态
//!\param   hBakManager      备份管理器句柄
//!\return  状态值
int MPU_BAK_GetState(HANDLE hBakManager);

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
int MPU_BAK_GetSize(unsigned long long *pdwSize, int nBakType, int nFileType, time_t tStartTime, time_t tEndTime, int nChanMask, int nRecTypeMask,
					int nDiskGroupMask);


/*****************************************************************************
 *						自动备份到备份盘组的处理
 ****************************************************************************/

//!\brief   设置自动备份的回调
//!\return  0表示设置成功 其他表示错误码
int MPU_BAK_Auto_SetCallback(HY_BACKUP_CALLBACK cbBackup);

//!\brief   启用自动备份
//!\param   bAuto       是否启用自动备份
//!\return  0表示设置成功 其他表示错误码
int MPU_BAK_Auto_EnableAuto(int bAuto);

//!\brief   设置自动备份周期
//!\param   nCycle      0表示每天 1表示每周
//!\return  0表示设置成功 其他表示错误码
int MPU_BAK_Auto_SetCycle(int nCycle);

//!\brief   设置启用自动备份的通道
//!\param   nChannelMask    需要备份的通道的掩码
//!\return  0表示设置成功 其他表示错误码
int MPU_BAK_Auto_SetChannelMask(int nChannelMask);

//!\brief   添加和删除自动备份的录像的类型
//!\param   nRecType    要备份的录像类型
//!\return  0表示设置成功 其他表示错误码
int MPU_BAK_Auto_EnableRecordType(int nRecType, int bEnable);

//!\brief   设置自动备份开始时间
//!\param   nWeekDay    当备份周期为天时,忽略该参数
//!\param   nHour       备份时间
//!\param   nMinute     备份时间
//!\return  0表示设置成功 其他表示错误码
int MPU_BAK_Auto_SetStartTime(int nWeekDay, int nHour, int nMinute);

//!\brief   设置自动备份结束时间
//!\param   nWeekDay    当备份周期为天时,忽略该参数
//!\param   nHour       备份时间
//!\param   nMinute     备份时间
//!\return  0表示设置成功 其他表示错误码
int MPU_BAK_Auto_SetStopTime(int nWeekDay, int nHour, int nMinute);

//!\brief   设置自动备份执行时间
//!\param   nWeekDay    当备份周期为天时,忽略该参数
//!\param   nHour       备份时间
//!\param   nMinute     备份时间
//!\return  0表示设置成功 其他表示错误码
int MPU_BAK_Auto_SetExecTime(int nWeekDay, int nHour, int nMinute);


#endif							/* #ifndef _INC_MPU_BAK_API_H_ */
