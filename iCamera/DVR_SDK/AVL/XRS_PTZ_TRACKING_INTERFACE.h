/*******************************************************************************
*
* 版权信息：版权所有 (c) 202X, 武汉芯睿视科技有限公司, 保留所有权利
*
* 文件名称：XRS_PTZ_TRACKING_INTERFACE.h
* 摘    要：芯睿视 球机跟踪接口头文件
*
* 当前版本：0.0.0.1
* 作    者：吴胜斌
* 日    期：20210901
* 备    注：
*			
********************************************************************************/
#ifndef _XRS_PTZ_TRACKING_INTERFACE_H_
#define _XRS_PTZ_TRACKING_INTERFACE_H_
#include "XRS_PTZ_TRACKING_STRUCT.h"

#ifdef __cplusplus
extern "C"
{
#endif
	/****************************************************************
	* 函数名：XRS_PTZ_Tracking_Init
	* 函数功能：球机跟踪算法初始化
	* 参数列表：pContext算法库句柄
	* 返回值：XRS_PTZ_TRACKING_ERROR_SUCESS ---- 成功
	* 备  注：
	*****************************************************************/
	int XRS_PTZ_Tracking_Init(void** pContext);

	/****************************************************************
	* 函数名：XRS_PTZ_Tracking_GetAlgVesion
	* 函数功能：获取当前算法库的版本信息,选择性调用
	* 参数列表：pContext算法库句柄，AlgVersion字符串（算法库类型和版本号）
	* 返回值：XRS_PTZ_TRACKING_ERROR_SUCESS			---- 成功
	* 备  注：获取当前算法库类型和版本号信息
	*****************************************************************/
	int XRS_PTZ_Tracking_GetAlgVersion(void* pContext, char* chAlgVersion);

	/****************************************************************
	* 函数名：XRS_PTZ_Tracking_UnInit
	* 函数功能：球机跟踪算法库反初始化
	* 参数列表：pContext算法库句柄
	* 返回值：XRS_PTZ_TRACKING_ERROR_SUCESS ---- 成功
	* 备  注：释放占用的资源
	*****************************************************************/
	int XRS_PTZ_Tracking_UnInit(void* pContext);

	/****************************************************************
	* 函数名：XRS_PTZ_Tracking_ForceStop
	* 函数功能：球机跟踪停止算法处理过程
	* 参数列表：pContext算法库句柄, bStop输入参数，int类型，非0时执行，0时不执行
	* 返回值：XRS_PTZ_TRACKING_ERROR_SUCESS ---- 成功
	* 备  注：该方法不能立即停止算法过程,需要在当前算法处理结束后停止之后的进度.
	*****************************************************************/
	int XRS_PTZ_Tracking_ForceStop(void* pContext, int bStop);

	/****************************************************************
	* 函数名：XRS_PTZ_Tracking_SetParam
	* 函数功能：球机跟踪算法库传入外部值
	* 参数列表：pContext算法库句柄，param需要设置的参数；
	* 返回值：XRS_PTZ_TRACKING_ERROR_SUCESS ---- 成功
	* 备  注：设置算法库参数
	*****************************************************************/
	int XRS_PTZ_Tracking_SetParam(void* pContext, PTZ_TRACK_INPUT_PARAM_ST param);

	/****************************************************************
	* 函数名：XRS_PTZ_Tracking_Stop
	* 函数功能：临时停止球机追踪模块
	* 参数列表:pContext算法库句柄；
	* 返回值：XRS_PTZ_TRACKING_ERROR_SUCESS				  ---- 成功
	* 备  注：临时停止球机追踪模块
	*****************************************************************/
	int XRS_PTZ_Tracking_Stop(void* pContext);

	/****************************************************************
	* 函数名：XRS_PTZ_Tracking_Process
	* 函数功能：球机追踪主处理模块
	* 参数列表:pContext算法库句柄, humanInfo为传入的人形坐标，outPut存放着结果地址；
	* 返回值：XRS_PTZ_TRACKING_ERROR_SUCESS				  ---- 成功
	* 备  注：主处理模块，执行目标跟踪及预测
	*****************************************************************/
	int XRS_PTZ_Tracking_Process(void* pContext, PTZ_TRACK_TASK_S humanInfo, void** outPut);

#ifdef __cplusplus
}
#endif 

#endif
