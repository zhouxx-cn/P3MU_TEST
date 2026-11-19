
/*!
 *\file		MPU_DOP_API.h
 *\brief	MPU模块，报警输出相关接口定义
 *\author	yuk@higheasyrd.com
 *\date		2009/03/17

功能：
-#	提供报警通道属性获取和设置（路数、时间表、输出延时等）
-#	提供获取和设置报警输出通道状态的接口
 */

#ifndef _INC_MPU_DOP_API_H_
#define _INC_MPU_DOP_API_H_

#include "tagAlarmOutSchedule.h"
#include "tagActionInfo.h"


//!\brief   获取布防时间表
//!\param   nPort                           报警输出通道号
//!\param   pAlarmOutSchedule               布防时间表缓冲区
//!\param   pAlarmOutSchedule->bytState     是否启用时间表，=0表示不启用，则为全天有效
//!\return  0表示获取成功，时间表存入pAlarmSchedule缓冲区中, -1表示失败
int MPU_DOP_GetSchedule(int nPort, AlarmOutSchedule * pAlarmOutSchedule);

//!\brief   设置布防时间表
//!\param   pAlarmOutSchedule               时间表缓冲区
//!\param   pAlarmOutSchedule->bytState     是否启用时间表，=0表示不启用，则为全天有效
//!\param   nPort               报警输出通道号
//!\return  0表示设置成功，-1表示设置失败
int MPU_DOP_SetSchedule(int nPort, AlarmOutSchedule * pAlarmOutSchedule);

//!\brief   设置模式，高级/一般模式
//!\param   nMode               0表示一般模式，1表示高级模式
//!\return  0表示设置成功，-1表示设置失败
int MPU_DOP_SetMode(int nMode);

//!\brief   获取模式, 高级/一般模式
//!\return  0表示一般模式，1表示高级模式，-1表示失败
int MPU_DOP_GetMode();

//!\brief   设置开关
//!\param   nEn
//!\return  0表示设置成功，-1表示设置失败
int MPU_DOP_SetEnable(int nEn);

//!\brief   获取开关
//!\return  0表示关，1表示开
int MPU_DOP_GetEnable();

//!\brief   获取延时时间
//!\param   nPort               报警输出通道号
//!\return  -1为获取失败；其他为获取成功
int MPU_DOP_GetDelayTime(int nPort);

//!\brief   设置延时时间
//!\param   nSecond             延时的秒数
//!\param   nPort               报警输出通道号
//!\return  0表示设置成功，-1表示失败
int MPU_DOP_SetDelayTime(int nPort, int nSecond);

//!\brief   获取输出端口数
//!\return  -1表示获取失败，其他表示获取的通道数
int MPU_DOP_GetPortNum();

//!\brief   获取端口输出状态
//!\param   nPort               输出通道号
//!\return  true表示正在输出;false表示已关闭输出
int MPU_DOP_GetAlarmOutState(int nPort);

//\brief    设置端口输出状态
//\param    bOn                 true表示打开输出;false表示关闭输出
//\param    nPort               输出通道号
//\return   0表示设置成功，-1表示设置失败
int MPU_DOP_SetAlarmOutState(int nPort, int bOn);

//\brief    设置声光报警使能
//\param    nEnable                 1表示打开输出;0表示关闭输出
//\param    nStayTime               报警持续时间
//\return   0表示设置成功，-1表示设置失败
int MPU_DOP_SetAlarmLightEnable(int nEnable,  int nStayTime);

//\brief    获取声光报警使能
//\param    nEnable                 1表示打开输出;0表示关闭输出
//\param    nStayTime               报警持续时间
//\return   0表示设置成功，-1表示设置失败
int MPU_DOP_GetAlarmLightEnable(int *nEnable,  int *nStayTime);

/* 设置报警输出事件计划配置 */
int MPU_DOP_SetAlarmOutScheduleCfg(const pAlarmOutTimeSchedule pAlarmOutSchedule);

/* 获取报警输出事件计划配置 */
int MPU_DOP_GetAlarmOutScheduleCfg(pAlarmOutTimeSchedule pAlarmOutSchedule);

#endif							/* _INC_MPU_DOP_API_H_ */
