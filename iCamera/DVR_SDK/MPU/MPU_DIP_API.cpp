
/*!
 *\file		MPU_DIP_API.cpp
 *\brief	MPU模块，报警输入相关接口实现
 *\author	yuk@higheasyrd.com
 *\date		2009/03/17

功能：
-#	提供自身属性（路数、类型、联动方式、时间表等）
-#	提供对自身状态的查询接口及当自身状态改变时可以主动上报
-#	提供设置各种联动处理的回调接口
 */

#include "MPU_global.h"


#ifndef DEBUG_NOT_USE_DIP_LIB	//调试时，不调用DIP库

#include "DIP/CAlarmInManager.h"	//报警输入管理接口


/**************************************************************************
 *                            局部宏定义                                  *
 **************************************************************************/


//调用报警输入管理器执行动作
#define MPU_DIP_CALL(x)											\
do																\
{																\
	int nRet;														\
																\
	CAlarmInManager *pMgr = CAlarmInManager::DIP_GetInstance();		\
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
		MPU_ERR("Get CAlarmInManager instance fail!\n");				\
		MPU_CHK(NULL != pMgr);									\
		nRet = IDVR_MPU_DIP_NO_MANAGER_INSTANCE;					\
	}															\
																\
	return nRet;													\
}																\
while (0)


//调用报警输入管理器获取参数
#define MPU_DIP_GET(DataType, DefaultValue, Function)					\
do																\
{																\
	DataType retValue;												\
																\
	CAlarmInManager *pMgr = CAlarmInManager::DIP_GetInstance();		\
																\
	if (NULL != pMgr)												\
	{															\
		retValue = pMgr->Function;									\
	}															\
	else															\
	{															\
		MPU_ERR("Get CAlarmInManager instance fail!\n");				\
		MPU_CHK(NULL != pMgr);									\
		retValue = DefaultValue;									\
	}															\
																\
	return retValue;												\
}																\
while (0)


#else							/* #ifndef DEBUG_NOT_USE_DIP_LIB */


//调用报警输入管理器执行动作
#define MPU_DIP_CALL(x)								\
do													\
{													\
	return IDVR_OK;									\
}													\
while (0)

//调用报警输入管理器获取参数
#define MPU_DIP_GET(DataType, DefaultValue, Function)		\
do													\
{													\
	return DefaultValue;								\
}													\
while (0)


#endif							/* #ifndef DEBUG_NOT_USE_DIP_LIB */



/**************************************************************************
 *                            模块内部接口                                *
 **************************************************************************/

//!\brief   初始化报警输入
//!\return  0表示成功，其它为错误码
int DIP_Init()
{
	int nRet = IDVR_OK;

#ifndef DEBUG_NOT_USE_DIP_LIB	//调试时，不调用DIP库

	CAlarmInManager *pMgr = CAlarmInManager::DIP_GetInstance();

	if (NULL == pMgr)
	{
		MPU_ERR("Get CAlarmInManager instance fail!\n");
		MPU_CHK(NULL != pMgr);
		nRet = IDVR_MPU_DIP_NO_MANAGER_INSTANCE;
	}
	else
	{
		MPU_INFO("out, return %d\n", nRet);
	}

#endif							/* #ifndef DEBUG_NOT_USE_DIP_LIB */

	return nRet;
}

//!\brief   逆初始化报警输入
//!\return  0表示成功，其它为错误码
int DIP_Uninit()
{
#ifndef DEBUG_NOT_USE_DIP_LIB	//调试时，不调用DIP库

	CAlarmInManager::DIP_Release();

#endif							/* #ifndef DEBUG_NOT_USE_DIP_LIB */

	return IDVR_OK;
}


/**************************************************************************
 *                            全局接口                                    *
 **************************************************************************/

//!\brief   获取报警输入通道布防时间表
//!\param   nPort               通道号
//!\param   pAlarmInSchedule    布防时间表缓冲区
//!\return  0表示设置成功,-1表示设置失败
int MPU_DIP_GetSchedule(int nPort, AlarmInSchedule * pAlarmInSchedule)
{
	MPU_DIP_CALL(DIP_GetSchedule(nPort, pAlarmInSchedule));
}

//!\brief   设置报警输入通道布防时间表
//!\param   nPort               通道号
//!\param   pAlarmInSchedule    布防时间表指针
//!\return  0表示设置成功,-1表示设置失败
int MPU_DIP_SetSchedule(int nPort, AlarmInSchedule * pAlarmInSchedule)
{
	MPU_DIP_CALL(DIP_SetSchedule(nPort, pAlarmInSchedule));
}

//!\brief   设置报警输入设置模式 一般/高级
//!\param   nMode               0表示一般模式,1表示高级模式
//!\return  0表示设置成功,-1表示失败
int MPU_DIP_SetMode(int nMode)
{
	MPU_DIP_CALL(DIP_SetMode(nMode));
}

//!\brief   获取报警输入设置模式 
//!\return  0表示一般模式，1表示高级模式,-1表示失败
int MPU_DIP_GetMode()
{
	MPU_DIP_GET(int, 0, DIP_GetMode());
}

//!\brief   获取报警输入通道数
//!\return  -1表示获取失败，其他表示报警输入通道数
int MPU_DIP_GetPortNum()
{
	MPU_DIP_GET(int, 0, DIP_GetPortNum());
}

//!\brief   获取报警输入通道类型
//!\param   nPort               通道号
//!\return  -1表示失败,0表示常开,1表示常闭
int MPU_DIP_GetType(int nPort)
{
	MPU_DIP_GET(int, 0, DIP_GetType(nPort));
}

//!\brief   设置通道类型
//!\param   nPort               通道号
//!\param   nType               0为常开,1为常闭
//!\return  0表示设置成功,-1表示设置失败
int MPU_DIP_SetType(int nPort, int nType)
{
	MPU_DIP_CALL(DIP_SetType(nPort, nType));
}

//!\brief       获取端口输入状态
//!\param   nPort               输出通道号
//!\return  TRUE 有输入，FALSE无输入
BOOL MPU_DIP_GetState(int nPort)
{
	MPU_DIP_GET(int, FALSE, DIP_GetState(nPort));
}

//!\brief       获取端口输入状态
//!\return  位俺码
int MPU_DIP_GetAllState()
{
	MPU_DIP_GET(int, 0, DIP_GetAllState());
}

//!\brief   获取报警输入联动录像参数
//!\param   nPort               报警输入通道号
//!\param   pnChannelFlag       联动的通道标记 
//!     每一位表示联动一个通道, 1表示联动通道1, 2表示联动通道2录像,
//!     3表示联动通道1和2录像, 以此类推
//!\return  0表示获取成功，-1表示获取失败
int MPU_DIP_GetRecordParam(int nPort, int *pnChannelFlag)
{
	MPU_DIP_CALL(DIP_GetRecordParam(nPort, pnChannelFlag));
}

//!\brief   设置报警输入联动录像参数
//!\param   nPort               报警输入通道号
//!\param   nChannelFlag        联动录像通道标记
//!\return  0表示设置成功 -1表示设置失败
int MPU_DIP_SetRecordParam(int nPort, int nChannelFlag)
{
	MPU_DIP_CALL(DIP_SetRecordParam(nPort, nChannelFlag));
}

//!\brief   获取报警输入联动拍照参数,拍照张数和间隔由CCapturePic::GetCfg获取
//!\param   nPort               报警联动输入通道号
//!\param   pnChannelFlag       存储联动拍照通道标记的缓冲区
//!\return  0表示成功 -1表示失败
int MPU_DIP_GetSnapshotParam(int nPort, int *pnChannelFlag)
{
	MPU_DIP_CALL(DIP_GetSnapshotParam(nPort, pnChannelFlag));
}

//!\brief   设置报警输入联动拍照参数，拍照张数和拍照间隔由CCapturePic::SetCfg设定
//!\param   nPort               报警输入通道号
//!\param   nChannelFlag        联动拍照通道标记
//!\return  0表示设置成功 -1表示失败
int MPU_DIP_SetSnapshotParam(int nPort, int nChannelFlag)
{
	MPU_DIP_CALL(DIP_SetSnapshotParam(nPort, nChannelFlag));
}

//!\brief       获取某一报警输入通道联动某一云台的动作参数
//!\param   nAction 联动动作, *nAction=0表示无动作，1联动预置点，2联动巡航，3联动轨迹
//!\param   nIndex 联动预置点/巡航/轨迹的编号
//!\param   nChannel 云台对应的通道号
//!\param   nPort 报警输入通道号
//!\return  0表示获取成功, -1表示获取失败
int MPU_DIP_GetPtzParam(int nPort, int *nAction, int nChannel, int *nIndex)
{
	MPU_DIP_CALL(DIP_GetPtzParam(nPort, nAction, nChannel, nIndex));
}

//!\brief       设置报警输入通道联动某一个云台的动作
//!\param   nAction 联动的动作,  0表示无动作 , 1联动预置点, 2联动巡航 3联动轨迹
//!\param   nIndex  联动的预置点/巡航/轨迹的编号
//!\param   nChannel    云台对应的通道号
//!\param   nPort   报警输入通道号
//!\return  0表示成功 -1表示失败
int MPU_DIP_SetPtzParam(int nPort, int nAction, int nChannel, int nIndex)
{
	MPU_DIP_CALL(DIP_SetPtzParam(nPort, nAction, nChannel, nIndex));
}

//!\brief   获取报警输入联动报警输出的参数
//!\param   nPort               报警输入通道号
//!\param   pnOutPortFlag       存储输出通道标记的缓冲区
//!\return  0表示成功 -1表示失败
int MPU_DIP_GetDopParam(int nPort, int *pnOutPortFlag)
{
	MPU_DIP_CALL(DIP_GetDopParam(nPort, pnOutPortFlag));
}

//!\brief   设置报警输入联动报警输出的参数
//!\param   nPort               报警输入通道号
//!\param   nOutPortFlag        设置输出通道标记
//!\return  0表示成功 -1表示失败
int MPU_DIP_SetDopParam(int nPort, int nOutPortFlag)
{
	MPU_DIP_CALL(DIP_SetDopParam(nPort, nOutPortFlag));
}

int MPU_DIP_SetPushEventIntvCnt(int nPort,int nPushEventIntv)
{
	MPU_DIP_CALL(SetPushEventIntvCnt(nPort, nPushEventIntv));
}

int MPU_DIP_GetPushEventIntvCnt(int nPort,int *nPushEventIntv)
{
	MPU_DIP_CALL(GetPushEventIntvCnt(nPort, nPushEventIntv));
}

//!\brief   获取报警输入的联动的某一项是否启用
//!\param   nPort               报警输入通道号
//!\param   nAction             联动的动作
//!     ACTION_SCREEN 屏幕显示
//!     ACTION_BEEP 声音提示
//!     ACTION_RECORD 通道录像
//!     ACTION_SNAPSHOT 通道拍照
//!     ACTION_ALARMOUT 报警输出
//!     ACTION_SENDTOSERVER 上传报警服务器
//!     ACTION_PTZ 联动云台
//!     ACTION_EMAIL 联动Email
//!     ACTION_SMS 联动短信
//!\return  -1为失败, 0为不启用，1为启用；
//!         对于联动屏幕提示，0为不启用，1为联动主显, 2为联动辅显, 3为联动主显和辅显;
int MPU_DIP_GetEnableAction(int nPort, int nAction)
{
	MPU_DIP_GET(int, 0, DIP_GetEnableAction(nPort, nAction));
}

//!\brief   设置是否启用某项联动
//!\param   nPort               报警输入端口号
//!\param   bEnable             是否启用该项联动,0为不启用,1为启用
//!     对于联动屏幕显示0为不启用，1为联动主显, 2为联动辅显, 3为联动主显和辅显
//!\param   nAction            联动动作
//!     ACTION_SCREEN 屏幕显示
//!     ACTION_BEEP 声音提示
//!     ACTION_RECORD 通道录像
//!     ACTION_SNAPSHOT 通道拍照
//!     ACTION_ALARMOUT 报警输出
//!     ACTION_SENDTOSERVER 上传报警服务器
//!     ACTION_PTZ 联动云台
//!     ACTION_EMAIL 联动Email
//!     ACTION_SMS 联动短信
//!\return  0为设置成功，-1为设置失败
int MPU_DIP_SetEnableAction(int nPort, int nAction, int bEnable)
{
	MPU_DIP_CALL(DIP_SetEnableAction(nPort, nAction, bEnable));
}

//!\brief   设置报警输入的回调
//!\param   nAlarmType          报警类型。
//!     VIDEO_LOST 为视频丢失报警，VIDEO_MOTION为移动报警、CODE_ERROR编码错
//!\param   pAlarmInCallback    报警输入联动其他模块的回调函数
//!     typedef int (*CB_Alert)(struct tagAlertInfo *pAlertInfo, struct tActionInfo *pActionInfo) 
//!             pAlertInfo      存储报警信息
//!             pActionInfo     存储联动信息
//!\return  0表示设置成功，其他表示设置失败
//!\sa  tagAlertInfo, tagActionInfo
int MPU_DIP_SetAlarmCallback(int nAlarmType, CBAlert pAlarmInCallback)
{
	if (NULL == g_pMpuInfo)
	{
		return IDVR_ERROR_INVALID_POINTER;
	}

	if (ALERT_TYPE_ALARMIN == nAlarmType)
	{
		g_pMpuInfo->m_pCallback_AlarmIn = pAlarmInCallback;
		MPU_DIP_CALL(DIP_SetAlarmCallback(nAlarmType, callback_AlarmAction));
	}
	else
	{
		MPU_ERR("ALERT_TYPE_ALARMIN != nAlarmType\n");
		MPU_CHK(ALERT_TYPE_ALARMIN == nAlarmType);
		return IDVR_ERROR_PARAMETER;
	}
}

//!\brief 设置报警输入是否使能,用于一键布防/撤防
//!\param int bEnable 1-布防,0-撤防
//!\return 0表示设置成功,-1表示设置失败
int MPU_DIP_SetEnable(int bEnable)
{
	MPU_DIP_CALL(DIP_SetEnable(bEnable));
}

//!\brief 设置报警输入是否使能,用于一键布防/撤防
//!\param int bEnable 1-布防,0-撤防
//!\return 0表示撤防,1表示布防
int MPU_DIP_GetEnable()
{
	MPU_DIP_GET(int, 1, DIP_GetEnable());
}

int MPU_DIP_GetAlarmInAudioAlarmSchedule(int nPort, tagAudioAlarmSchedule *stSchedule)
{
	MPU_DIP_GET(int, 0, DIP_GetAlarmInAudioAlarmSchedule(nPort, stSchedule));
}

int MPU_DIP_SetAlarmInAudioAlarmSchedule(int nPort, tagAudioAlarmSchedule *stSchedule)
{
	MPU_DIP_CALL(DIP_SetAlarmInAudioAlarmSchedule(nPort, stSchedule));
}

int MPU_DIP_GetAlarmInAudioAlarmCfg(int nPort, int *nVoiceType, int *nBroadcastCount)
{
	MPU_DIP_GET(int, 0, DIP_GetAlarmInAudioAlarmCfg(nPort, nVoiceType, nBroadcastCount));
}

int MPU_DIP_SetAlarmInAudioAlarmCfg(int nPort, int nVoiceType, int nBroadcastCount)
{
	MPU_DIP_CALL(DIP_SetAlarmInAudioAlarmCfg(nPort,  nVoiceType, nBroadcastCount));
}

int MPU_DIP_CheckAudioAlarmInSchedule(int nPort)
{
	MPU_DIP_GET(int, 0, DIP_CheckAudioAlarmInSchedule(nPort));
}

/**************************************************************************
 *                            注销局部宏定义                              *
 **************************************************************************/

#undef MPU_DIP_CALL				//调用报警输入管理器执行动作
#undef MPU_DIP_GET				//调用报警输入管理器获取参数值
