
/*!
 *\file		MPU_DOP_API.cpp
 *\brief	MPU模块，报警输出相关接口实现
 *\author	yuk@higheasyrd.com
 *\date		2009/03/17

功能：
-#	提供报警通道属性获取和设置（路数、时间表、输出延时等）
-#	提供获取和设置报警输出通道状态的接口
 */


#include "MPU_global.h"


#ifndef DEBUG_NOT_USE_DOP_LIB	//调试时，不调用DOP库

#include "DOP/CAlarmOutManager.h"	//报警输出管理接口


/**************************************************************************
 *                            局部宏定义                                  *
 **************************************************************************/


//调用报警输出管理器执行动作
#define MPU_DOP_CALL(x)											\
do																\
{																\
	int nRet;														\
																\
	CAlarmOutManager *pMgr = CAlarmOutManager::DOP_GetInstance();		\
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
		MPU_ERR("Get CAlarmOutManager instance fail!\n");				\
		MPU_CHK(NULL != pMgr);									\
		nRet = IDVR_MPU_DOP_NO_MANAGER_INSTANCE;					\
	}															\
																\
	return nRet;													\
}																\
while (0)


//调用报警输出管理器获取参数
#define MPU_DOP_GET(DataType, DefaultValue, Function)				\
do																\
{																\
	DataType retValue;												\
																\
	CAlarmOutManager *pMgr = CAlarmOutManager::DOP_GetInstance();		\
																\
	if (NULL != pMgr)												\
	{															\
		retValue = pMgr->Function;									\
	}															\
	else															\
	{															\
		MPU_ERR("Get CAlarmOutManager instance fail!\n");				\
		MPU_CHK(NULL != pMgr);									\
		retValue = DefaultValue;									\
	}															\
																\
	return retValue;												\
}																\
while (0)


#else							/* #ifndef DEBUG_NOT_USE_DOP_LIB */


//调用报警输出管理器执行动作
#define MPU_DOP_CALL(x)											\
do																\
{																\
	return IDVR_OK;												\
}																\
while (0)

//调用报警输出管理器获取参数
#define MPU_DOP_GET(DataType, DefaultValue, Function)				\
do																\
{																\
	return DefaultValue;											\
}																\
while (0)


#endif							/* #ifndef DEBUG_NOT_USE_DOP_LIB */



/**************************************************************************
 *                            模块内部接口                                *
 **************************************************************************/

//!\brief   初始化报警输出
//!\return  0表示成功，其它为错误码
int DOP_Init()
{
	int nRet = IDVR_OK;

#ifndef DEBUG_NOT_USE_DOP_LIB	//调试时，不调用DOP库

	CAlarmOutManager *pMgr = CAlarmOutManager::DOP_GetInstance();

	if (NULL == pMgr)
	{
		MPU_ERR("Get CAlarmOutManager instance fail!\n");
		MPU_CHK(NULL != pMgr);
		nRet = IDVR_MPU_DOP_NO_MANAGER_INSTANCE;
	}

#endif							/* #ifndef DEBUG_NOT_USE_DOP_LIB */

	return nRet;
}

//!\brief   逆初始化报警输出
//!\return  0表示成功，其它为错误码
int DOP_Uninit()
{
#ifndef DEBUG_NOT_USE_DOP_LIB	//调试时，不调用DOP库

	CAlarmOutManager::DOP_Release();

#endif							/* #ifndef DEBUG_NOT_USE_DOP_LIB */

	return IDVR_OK;
}


/**************************************************************************
 *                            全局接口                                    *
 **************************************************************************/

//!\brief   获取布防时间表
//!\param   nPort                           报警输出通道号
//!\param   pAlarmOutSchedule               布防时间表缓冲区
//!\param   pAlarmOutSchedule->bytState     是否启用时间表，=0表示不启用，则为全天有效
//!\return  0表示获取成功，时间表存入pAlarmSchedule缓冲区中, -1表示失败
int MPU_DOP_GetSchedule(int nPort, AlarmOutSchedule * pAlarmOutSchedule)
{
	MPU_DOP_CALL(DOP_GetSchedule(nPort, pAlarmOutSchedule));
}

//!\brief   设置布防时间表
//!\param   pAlarmOutSchedule               时间表缓冲区
//!\param   pAlarmOutSchedule->bytState     是否启用时间表，=0表示不启用，则为全天有效
//!\param   nPort               报警输出通道号
//!\return  0表示设置成功，-1表示设置失败
int MPU_DOP_SetSchedule(int nPort, AlarmOutSchedule * pAlarmOutSchedule)
{
	MPU_DOP_CALL(DOP_SetSchedule(nPort, pAlarmOutSchedule));
}

//!\brief   设置模式，高级/一般模式
//!\param   nMode               0表示一般模式，1表示高级模式
//!\return  0表示设置成功，-1表示设置失败
int MPU_DOP_SetMode(int nMode)
{
	MPU_DOP_CALL(DOP_SetMode(nMode));
}

//!\brief   获取模式, 高级/一般模式
//!\return  0表示一般模式，1表示高级模式，-1表示失败
int MPU_DOP_GetMode()
{
	MPU_DOP_GET(int, 0, DOP_GetMode());
}

//!\brief   设置开关
//!\param   nEn
//!\return  0表示设置成功，-1表示设置失败
int MPU_DOP_SetEnable(int nEn)
{
	MPU_DOP_CALL(DOP_SetEnable(nEn));
}

//!\brief   获取开关
//!\return  0表示关，1表示开
int MPU_DOP_GetEnable()
{
	MPU_DOP_GET(int, 0, DOP_GetEnable());
}

//!\brief   获取延时时间
//!\param   nPort               报警输出通道号
//!\return  -1为获取失败；其他为获取成功
int MPU_DOP_GetDelayTime(int nPort)
{
	MPU_DOP_GET(int, 0, DOP_GetLastTime(nPort));
}

//!\brief   设置延时时间
//!\param   nSecond             延时的秒数
//!\param   nPort               报警输出通道号
//!\return  0表示设置成功，-1表示失败
int MPU_DOP_SetDelayTime(int nPort, int nSecond)
{
	MPU_DOP_CALL(DOP_SetLastTime(nPort, nSecond));
}

//!\brief   获取输出端口数
//!\return  -1表示获取失败，其他表示获取的通道数
int MPU_DOP_GetPortNum()
{
	MPU_DOP_GET(int, 0, DOP_GetPortNum());
}

//!\brief   获取端口输出状态
//!\param   nPort               输出通道号
//!\return  true表示正在输出;false表示已关闭输出
int MPU_DOP_GetAlarmOutState(int nPort)
{
	MPU_DOP_GET(int, 0, DOP_GetAlarmOutState(nPort));
}

//\brief    设置端口输出状态
//\param    bOn                 true表示打开输出;false表示关闭输出
//\param    nPort               输出通道号
//\return   0表示设置成功，-1表示设置失败
int MPU_DOP_SetAlarmOutState(int nPort, int bOn)
{
	MPU_DOP_CALL(DOP_SetAlarmOutState(nPort, bOn));
}

int MPU_DOP_GetAlarmLightEnable(int *nEnable,  int *nStayTime)
{
	MPU_DOP_CALL(DOP_GetAlarmLightEnable(nEnable, nStayTime));
}

int MPU_DOP_SetAlarmLightEnable(int nEnable,  int nStayTime)
{
	MPU_DOP_CALL(DOP_SetAlarmLightEnable(nEnable, nStayTime));
}

/**************************************************************************
 *                            注销局部宏定义                              *
 **************************************************************************/

#undef MPU_DOP_CALL				//调用报警输出管理器执行动作
#undef MPU_DOP_GET				//调用报警输出管理器获取参数值
