
/*!
 *\file		MPU_PTZ_API.h
 *\brief	MPU模块，云台控制相关接口定义
 *\author	林云
 *\date		2009/04/10

功能：
-	支持云台参数的获取和设置,参数包括协议、解码器地址、波特率
-	支持云台控制
	-#	一般控制；上、下、左右、光圈、焦距、变倍
	-#	自动控制；自动扫描、巡航扫描
	-#	预置点控制；预置点设置、调用、删除，巡航点设置开始、添加、结束，轨迹设置
	-#	辅助设备控制；灯光、雨刷、加热、制冷、电源
	-#	菜单控制；进入菜单、焦点上移、焦点下移、退出菜单
-	支持云台协议的安装与卸载
	-#	固定协议可卸载但不删除，需要时可随时安装
	-#	非固定协议卸载后立即删除
	-#	可从外部设备导入新的协议

 */

#ifndef __MPU_PTZ_H_
#define __MPU_PTZ_H_

#include "tagProtocolName.h"
#include "tagPtzFlag.h"
#include "tagTransparent.h"

//!\brief 获取是否支持云台
//!\return 0表示不启用,1表示启用
int MPU_PTZ_GetIsSupportMoto();

//!\brief 设置是否启用云台功能
//!\param bEnable 0表示不启用，其他表示启用
//!\param nChannel 通道号
//!\return 0表示设置成功，其他表示错误码
int MPU_PTZ_Enable(int bEnable, int nChannel);

//!\brief 获取云台是否启用云台功能
//!\param nChannel 通道号
//!\return 0表示不启用,其他表示启用
int MPU_PTZ_IsEnable(int nChannel);

//!\brief   获取云台协议名称
//!\param   szProtocol              协议名缓冲区
//!\param   nLength                 缓冲区长度
//!\param   nChannel                通道号
//!\return  0表示获取成功 -1表示获取失败
int MPU_PTZ_GetProtocol(char *pszProtocol, int nLength, int nChannel);

//!\brief   设置云台的协议名称
//!\param   szProtocol              云台协议名指针
//!\param   nChannel                通道号
//!\return  0表示获取成功 -1表示获取失败
int MPU_PTZ_SetProtocol(char *pszProtocol, int nChannel);

//!\brief   设置云台先进配置
//!\param                 
//!\param                   
//!\return  0表示获取成功 -1表示获取失败
int MPU_PTZ_SetAdvanceCfg(int nChannel, char nCruiseEnable, 
									int nBeginPresetNumForCruise,
									int nBeginPresetActionForCruise,
									int nEndPresetNumForCruise,
									int nEndPresetActionForCruise,
									char nWatchEnable,
									int nBeginPresetNumForWatch,
									int nBeginPresetActionForWatch,
									int nEndPresetNumForWatch,
									int nEndPresetActionForWatch);

//!\brief	设置云台先进配置
//!\param				  
//!\param					
//!\return	0表示获取成功 -1表示获取失败
int MPU_PTZ_SetAdvanceCfgV1(int nChannel, char nCruiseEnable, 
									int nBeginPresetNumForCruise,
									int nBeginPresetActionForCruise,
									int nEndPresetNumForCruise,
									int nEndPresetActionForCruise,
									char nWatchEnable,
									int nBeginPresetNumForWatch,
									int nBeginPresetActionForWatch,
									int nEndPresetNumForWatch,
									int nEndPresetActionForWatch,
									char nTrackEnable,
									int nBeginPresetNumForTrack,
									int nBeginPresetActionForTrack,
									int nEndPresetNumForTrack,
									int nEndPresetActionForTrack);

//!\brief   获取云台先进配置
//!\param                 
//!\param                   
//!\return  0表示获取成功 -1表示获取失败
int MPU_PTZ_GetAdvanceCfg(int nChannel, char *nCruiseEnable, 
									int *nBeginPresetNumForCruise,
									int *nBeginPresetActionForCruise,
									int *nEndPresetNumForCruise,
									int *nEndPresetActionForCruise,
									char *nWatchEnable,
									int *nBeginPresetNumForWatch,
									int *nBeginPresetActionForWatch,
									int *nEndPresetNumForWatch,
									int *nEndPresetActionForWatch);

//!\brief	获取云台先进配置
//!\param				  
//!\param					
//!\return	0表示获取成功 -1表示获取失败
int MPU_PTZ_GetAdvanceCfgV1(int nChannel, char *nCruiseEnable, 
									int *nBeginPresetNumForCruise,
									int *nBeginPresetActionForCruise,
									int *nEndPresetNumForCruise,
									int *nEndPresetActionForCruise,
									char *nWatchEnable,
									int *nBeginPresetNumForWatch,
									int *nBeginPresetActionForWatch,
									int *nEndPresetNumForWatch,
									int *nEndPresetActionForWatch,
									char *nTrackEnable,
									int *nBeginPresetNumForTrack,
									int *nBeginPresetActionForTrack,
									int *nEndPresetNumForTrack,
									int *nEndPresetActionForTrack);

//!\brief   设置云台速度
//!\param   nSpeed               速度等级
//!\param   nChannel                通道号
//!\return  0表示成功，其他表示失败
int MPU_PTZ_SetSpeed(int nSpeed, int nChannel);

//!\brief   设置ptzV速度
//!\param   nChannel  通道号     nSpeedV  速度
//!\return  -1为失败
int MPU_PTZ_SetSpeedV(int nSpeedV, int nChannel);

//!\brief   设置ptzH速度
//!\param   nChannel  通道号     nSpeedH  速度
//!\return  -1为失败
int MPU_PTZ_SetSpeedH(int nSpeedH, int nChannel);

int MPU_PTZ_GetSpeed(int nChannel);

//!\brief   获取通道的解码器地址
//!\param   nChannel                通道号
//!\return  -1表示获取失败，其他表示解码器地址
unsigned long MPU_PTZ_GetAddress(int nChannel);

//!\brief   设置云台解码器地址
//!\param   dwAddress               新的解码器地址
//!\param   nChannel                通道号
//!\return  0表示成功，其他表示失败
int MPU_PTZ_SetAddress(unsigned long dwAddress, int nChannel);

//!\brief   获取某一通道解码器的波特率
//!\param   nChannel                通道号
//!\return  -1为失败，其他为解码器波特率
unsigned long MPU_PTZ_GetBaudrate(int nChannel);

//!\brief   设置解码器波特率
//!\param   dwBaudrate 新的波特率
//!\param   nChannel 通道号
//!\return  0表示设置成功，-1表示失败
int MPU_PTZ_SetBaudrate(unsigned long dwBaudrate, int nChannel);

int MPU_PTZ_GetNormalCtrlPreset(int nChannel, int *nNormalCtrlPreset);

int MPU_PTZ_SetNormalCtrlPreset(int nChannel, int nNormalCtrlPreset);

int MPU_PTZ_SetWatchCareEnable(int nCh, int nEnable);

int MPU_PTZ_GetWatchCareEnable(int nCh,  int *nEnable);

int MPU_PTZ_SetWatchCareReTime(int nCh, int nTime);

int MPU_PTZ_GetWatchCareReTime(int nCh,  int *nTime);

int MPU_PTZ_GetWatchCareType(int nCh,  int *nType);

//!\brief 获取解码器参数
//!\param nPropIndex 参数序号
//!0-"DigitZoom",
//!1-"LowLum",
//!2-"BackLight",
//!3-"AutoShutter",
//!4-"WhiteBalance",
//!5-"RedGain",
//!6-"BlueGain",
//!7-"Acutance",
//!8-"ExpMode",
//!9-"ShutterSpeed",
//!10-"IrisSpeed",
//!11-"WideDyn",
//!12-"3DFilter",
//!13-"Mirror",
//!14-"Reverse",
//!\param nChannel 通道号
//!\return 参数值,-1表示失败
//##ModelId=49B71C170186
int MPU_PTZ_GetEletronicProp(int nPropIndex, int nChannel);

//!\brief 设置解码器参数
//!\param nValue 参数值
//!\param nPropIndex 参数序号
//!\param nChannel 通道号
//!\return 0表示设置成功，-1表示失败
int MPU_PTZ_SetEletronicProp(int nValue, int nPropIndex, int nChannel);

//!\brief 恢复解码器参数
//!\param nChannel 通道号
//!\return 0表示设置成功，-1表示失败
int MPU_PTZ_ResetAllEletronicProp(int nChannel);

//!\brief 获取类型
int MPU_PTZ_GetType(int nChannel);

//!\brief 获取当前机芯温度
int MPU_PTZ_GetTemperature(int nChannel, int *pnValue);

//!\brief 获取当前坐标
//!\param nChannel 通道
//!\param pnHorizontalAngle水平角度0~360
//!\param pnVerticalAngle垂直角度-180~+180
//!\param pnMultiple 倍数 1~9999
int MPU_PTZ_GetCoordinate(int nChannel, int *pnHorizontalAngle, int *pnVerticalAngle, int *pnMultiple, int *pnOrientation);

//!\brief 获取球机485串口配置
//!\param nChannel 通道
//!\param pnAddress	485控制地址
//!\param pnProtocol	485控制协议编号
//!\param pnBaudRate	485波特率
int MPU_PTZ_Get485Config(int nChannel, int *pnAddress, int *pnProtocol, int *pnBaudRate);

//!\brief   返回可用（不可用的）云台协议列表
//!\param   bEnable                 true表示可用的协议 false表示不可用的协议
//!\param   pProtocolList           协议名数组头指针
//!\param   nLength                 协议名数组长度
//!\param   bExternal               协议是否外部协议的数组头指针,TRUE表示是外部协议, FALSE表示是内部协议
//!\return  返回协议个数
int MPU_PTZ_GetProtocolList(char pProtocolList[][256], int nLength, int bEnable, int bExternal[] = NULL);

//!\brief   返回云台协议（可用/不可用）的数目
//!\param   bEnable                 true表示可用的协议，false表示不可用的协议
//!\return  -1表示失败，其他值表示成功
int MPU_PTZ_GetProtocolCount(int bEnable);

//!\brief   安装云台协议
//!\param   pszSrcPath              协议文件路径
//!\return  0表示安装成功，其他表示安装失败
int MPU_PTZ_InstallProtocol(char *pszSrcPath);

//!\brief   卸载云台协议
//!\param   pszProtocolName         云台协议名
//!\param   pszDstPath              NULL表示直接删除，其他表示备份路径
//!\return  0表示卸载成功，其他表示卸载错误
int MPU_PTZ_UninstallProtocol(char *pszProtocolName, char *pszDstPath);

//!\brief 获取文件是否是合法的云台协议
//!\param szFileName 协议路径名
//!\param pProtocolName 协议名
//!\param nLength 协议名缓冲区长度
//!\return 1表示合法 0表示非法
int MPU_PTZ_IsValidProtocol(char *szFileName, char *pProtocolName, int nLength);

#ifdef EGUI_FOR_ICAMERA_NXP8850_BASED_DVR
//!\brief   电子PTZ控制
//!\param   dwPtzCode       云台控制码 见TTY_MPU_PTZ_CODE
//!\param   pdwParamList    控制参数
//!\param   nChannel            云台对应的通道号
//!\return  0表示成功，其他表示错误码
//!\sa TTY_MPU_PTZ_CODE
int MPU_PTZ_EletronicCtrl(int nChannel, unsigned long dwPtzCode, unsigned long *pdwParamList);
#endif

//!\brief   云台一般控制
//!\param   dwPtzCode               云台控制码 见TTY_MPU_PTZ_CODE
//!\param   dwSpeed                 运动速度
//!\param   nChannel                云台对应的通道号
//!\return  0表示成功，其他表示错误码
//!\sa TTY_MPU_PTZ_CODE
int MPU_PTZ_NormalCtrl(unsigned long dwPtzCode, unsigned long dwSpeed, int nChannel);

//!\brief   预置点控制
//!\param   dwPtzCode               云台控制码
//!\param   dwPresetIndex           预置点号
//!\param   dwCruiseIndex           巡航点号
//!\param   dwSpeed                 速度
//!\param   dwDwellTime             停滞时间
//!\param   nChannel                通道号
//!\return  0表示成功 其他值表示错误码
//!\sa TTY_PRESET_CODE
int MPU_PTZ_PresetCtrl(unsigned long dwPtzCode, unsigned long dwPresetIndex, unsigned long dwCruiseIndex, unsigned long dwSpeed,
					   unsigned long dwDwellTime, int nChannel);

//!\brief   云台自动控制
//!\param   dwPtzCode 控制码参见 TTY_AUTO_CODE
//!\param   dwPresetIndex1 预置点号
//!\param   dwPresetIndex2 预置点号
//!\param   dwSpeed 运动速度
//!\param   bScan 开始/结束
//!\param   nChannel 通道号
//!\return  0表示成功 -1表示失败
//!\sa TTY_AUTO_CODE
int MPU_PTZ_AutoCtrl(unsigned long dwPtzCode, unsigned long dwPresetIndex1, unsigned long dwPresetIndex2, unsigned long dwCruiseIndex,
					 unsigned long dwSpeed, unsigned long dwDwellTime, int bScan, int nChannel);

//!\brief   云台辅助设备控制
//!\param   dwPtzCode               云台辅助控制码,参见TTY_AUX_CODE
//!\param   bOn                     1=打开,0=关闭
//!\param   nChannel                云台解码器对应的通道号
//!\return  0表示成功 -1表示失败
//!\sa TTY_AUX_CODE
int MPU_PTZ_AuxCtrl(unsigned long dwPtzCode, int bOn, int nChannel);

//!\brief   云台球机菜单控制
//!\param   dwPtzCode               菜单控制码
//!\param   nChannel                云台对应的通道号
//!\return  0表示执行成功，-1表示执行失败
//!\sa TTY_MENU_CODE
int MPU_PTZ_MenuCtrl(unsigned long dwPtzCode, int nChannel);

//!\brief   获取云台是否支持相应的功能
//!\param   nPtzFlag                云台支持的功能标记 参见PTZ_FLAG
//!\param   nChannel                通道号
//!\return  0表示支持 -1表示不支持
//!\sa PTZ_FLAG
int MPU_PTZ_HasFunction(PTZ_FLAG nPtzFlag, int nChannel);

//!\brief       获取云台技术参数接口
//!\param   nChannel 云台对应的通道号
//!\param   pnPresetCount       预置点个数
//!\param   pnCruiseCount       巡航线路条数
//!\param   pnTrackCount        轨迹条数
//!\param   pnAutoCount       线扫条数
//!\param   pdwBaudrate     支持的波特率
//!\return  0表示执行成功，-1表示执行失败
int MPU_PTZ_GetTechnicalData(int nChannel, int *pnPresetCount, int *pnCruiseCount, int *pnTrackCount, int *pnAutoCount, unsigned long *pdwBaudrate);

//!\brief 获取透明通道接口参数
//!\param 无
//!\return NULL表示获取失败, 其他表示获取成功
LPTransparentChanInterface MPU_TPC_GetNetInterface();

//!\brief 设置向透明通道写数据的回调
//!\param 回调函数，在该函数中处理从透明通道读到的数据
//!\return 0表示设置成功 -1表示设置失败
int MPU_TPC_SetWriteCallback(IF_TransparentChannelWrite cbWrite);

//!\brief 开始透明通道测试
int MPU_TPC_StartTest();

//!\brief 停止透明通道测试
int MPU_TPC_StopTest();


//!\brief 获取云台控制键是调出界面还是进入键盘控制模式
int MPU_PTZ_GetPtzKeyWorkMode();


//!\brief 设置云台控制键是调出界面还是进入键盘控制模式
int MPU_PTZ_SetPtzKeyWorkMode(int Mode);


/************************************************************************/

/*                     云台守望功能接口                                      */

/************************************************************************/

//!\brief 设置是否启用云台守望功能
//!\param bEnable 0表示不启用，其他表示启用
//!\param nChannel 通道号
//!\return 0表示设置成功，其他表示错误码
int MPU_PTZ_EnableKeeper(int bEnable, int nChannel);

//!\brief 获取云台是否启用守望功能
//!\param nChannel 通道号
//!\return 0表示不启用,其他表示启用
int MPU_PTZ_IsEnableKeeper(int nChannel);

//!\brief 获取云台复位周期
//!\param nChannel 云台通道号
//!\return 复位周期，以秒为单位
int MPU_PTZ_GetResetCycle(int nChannel);

//!\brief 设置云台复位周期
//!\param nSecond 设置云台复位需要空闲的秒数
//!\param nChannel 云台通道号
//!\return 0表示设置成功 其他表示错误码
int MPU_PTZ_SetResetCycle(int nSecond, int nChannel);

//!\brief 获取云台默认动作的接口
//!\param pdwActionType 默认动作类型
//!\param pdwIndex 动作相关参数，（预置点号，巡航线路号，轨迹号，扫描线路号）
//!\param nChannel 云台通道号
int MPU_PTZ_GetDefaultAction(unsigned long *pdwActionType, unsigned long *pdwIndex, int nChannel);

//!\brief 设置云台默认动作的接口
//!\param dwActionType 默认动作类型
//!\param dwIndex 动作相关参数，（预置点号，巡航线路号，轨迹号，扫描线路号）
//!\param nChannel 云台通道号
//!\return 0表示设置正确，其他表示错误码
int MPU_PTZ_SetDefaultAction(unsigned long dwActionType, unsigned long dwIndex, int nChannel);

//!\brief 通知云台进入报警联动状态
//!\param dwActionType 联动动作类型
//!\param dwIndex 动作相关参数，（预置点号，巡航线路号，轨迹号，扫描线路号）
//!\param nChannel 云台通道号
//!\return 0
int MPU_PTZ_Alarm(unsigned long dwActionType, unsigned long dwIndex, int nChannel);

//!\brief 通知云台退出报警联动状态
//!\param nChannel 云台通道号
//!\return 0
int MPU_PTZ_ClearAlarm(int nChannel);

//!\brief   设置ptz 水平默认值
//!\param   nChannel  通道号     nHorizontalAngle  水平默认值
//!\return  -1为失败
int MPU_PTZ_SetHorizontalAngle(int nChannel, int nHorizontalAngle);

//!\brief   设置ptz 纵向默认值
//!\param   nChannel  通道号     nVerticalAngle  纵向默认值
//!\return  -1为失败
int MPU_PTZ_SetVerticalAngle(int nChannel, int nVerticalAngle);

//!\brief	设置ptz 水平反向
//!\param	nChannel  通道号 	nHorizontalReverse  水平反向
//!\return	-1为失败
int MPU_PTZ_SetHorizontalReverse(int nChannel, int nHorizontalReverse);

//!\brief   设置ptz 纵向反向
//!\param   nChannel  通道号     nVerticalReverse  纵向反向
//!\return  -1为失败
int MPU_PTZ_SetVerticalReverse(int nChannel, int nVerticalReverse);

//!\brief   设置ptz 水平步长
//!\param   nChannel  通道号     nHorizontalStep  水平步长
//!\return  -1为失败
int MPU_PTZ_SetHorizontalStep(int nChannel, int nHorizontalStep);

//!\brief   设置ptz 纵向步长
//!\param   nChannel  通道号     nVerticalStep  纵向步长
//!\return  -1为失败
int MPU_PTZ_SetVerticalStep(int nChannel, int nVerticalStep);

//!\brief   设置ptz 水平速度
//!\param   nChannel  通道号     nHorizontalStep  水平速度
//!\return  -1为失败
int MPU_PTZ_SetMaxSpeedH(int nChannel, int nMaxSpeedH);

//!\brief   设置ptz 纵向速度
//!\param   nChannel  通道号     nVerticalStep  纵向速度
//!\return  -1为失败
int MPU_PTZ_SetMaxSpeedV(int nChannel, int nMaxSpeedV);


//设置用户定制云台垂直最大步长
int MPU_PTZ_SetUserVMaxStep(int nChannel, int nUserVMaxStep);

//设置产测状态到云台模块
int MPU_PTZ_SetProductTestMode(int nTestMode);

//!\brief   设置ptz 支持垂直跟踪
//!\param   nChannel  通道号     SupportVertical  垂直跟踪
//!\return  -1为失败
int MPU_PTZ_SetSupportVertical(int nChannel, int nSupportVertical);

//!\brief   设置ptz 支持云台
//!\param   nChannel  通道号     nSupporMotor  支持云台
//!\return  -1为失败
int MPU_PTZ_SetSupportMotor(int nChannel, int nSupporMotor);

int MPU_PTZ_SetPTZDevSwapLRPTZChannel(int nSupporSwap);


//!\brief   更新设备睡眠状态
int MPU_PTZ_UpdateDorStatus(int bDorEnable);

#endif
