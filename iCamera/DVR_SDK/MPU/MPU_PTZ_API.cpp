
/*!
 *\file		MPU_PTZ_API.cpp
 *\brief	MPU模块，云台控制相关接口实现
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

 *\history 添加云台守望功能 2009-11-20
 *\author 喻坤
功能:
-   支持云台在用户指定的时间内复位
 */


#include "MPU_global.h"

#include "PTZ/CPtzManager.h"
#include "PTZ/CTransparentManager.h"

/**************************************************************************
 *                            声明局部宏定义                              *
 **************************************************************************/

#ifndef DEBUG_NOT_USE_PTZ_LIB	//调试时，不调用PTZ库

//调用云台管理器执行动作
#define MPU_PTZ_CALL(x)								\
do													\
{													\
	int nRet;											\
													\
	CPtzManager *pMgr = CPtzManager::PTZ_GetInstance();	\
													\
	if (NULL != pMgr)									\
	{												\
		nRet = pMgr->x;								\
		if (IDVR_OK != nRet)							\
		{											\
			MPU_ERR("%s, nRet = %d\n", #x, nRet);		\
		}											\
	}												\
	else												\
	{												\
		MPU_ERR("Get CPtzManager instance fail!\n");		\
		MPU_CHK(NULL != pMgr);						\
		nRet = IDVR_MPU_PTZ_NO_MANAGER_INSTANCE;		\
	}												\
													\
	return nRet;										\
}													\
while (0)

//调用云台管理器获取参数
#define MPU_PTZ_GET(DataType, DefaultValue, Function)		\
do													\
{													\
	DataType retValue;									\
													\
	CPtzManager *pMgr = CPtzManager::PTZ_GetInstance();	\
													\
	if (NULL != pMgr)									\
	{												\
		retValue = pMgr->Function;						\
	}												\
	else												\
	{												\
		MPU_ERR("Get CPtzManager instance fail!\n");		\
		MPU_CHK(NULL != pMgr);						\
		retValue = DefaultValue;				\
	}												\
													\
	return retValue;									\
}													\
while (0)

//调用云台管理器执行动作
#define MPU_TPC_CALL(x)								\
do													\
{													\
	int nRet;										\
													\
	CTransparentManager *pMgr = CTransparentManager::GetInstance();	\
													\
	if (NULL != pMgr)								\
	{												\
		nRet = pMgr->x;								\
		if (IDVR_OK != nRet)						\
		{											\
			MPU_ERR("%s, nRet = %d\n", #x, nRet);	\
		}											\
	}												\
	else											\
	{												\
		MPU_ERR("Get CTransparentManager instance fail!\n");\
		MPU_CHK(NULL != pMgr);						\
		nRet = IDVR_MPU_TPC_NO_MANAGER_INSTANCE;	\
	}												\
													\
	return nRet;										\
}													\
while (0)

//调用云台管理器获取参数
#define MPU_TPC_GET(DataType, DefaultValue, Function)		\
do													\
{													\
	DataType retValue;									\
														\
	CTransparentManager *pMgr = CTransparentManager::GetInstance();	\
														\
	if (NULL != pMgr)									\
	{												\
		retValue = pMgr->Function;						\
	}												\
	else												\
	{												\
		MPU_ERR("Get CTransparentManager instance fail!\n");		\
		MPU_CHK(NULL != pMgr);						\
		retValue = DefaultValue;						\
	}												\
													\
	return retValue;									\
}													\
while (0)

#else

//调用云台管理器执行动作
#define MPU_PTZ_CALL(x)								\
do													\
{													\
	return IDVR_OK;									\
}													\
while (0)

//调用云台管理器获取参数
#define MPU_PTZ_GET(DataType, DefaultValue, Function)		\
do													\
{													\
	return DefaultValue;								\
}													\
while (0)

//调用透明通道执行动作
#define MPU_TPC_CALL(x)								\
do													\
{													\
	return IDVR_OK;									\
}													\
while (0)

//调用透明通道获取参数
#define MPU_TPC_GET(DataType, DefaultValue, Function)		\
do													\
{													\
	return DefaultValue;								\
}													\
while (0)

#endif							/* #ifndef DEBUG_NOT_USE_PTZ_LIB */


/**************************************************************************
 *                            模块内部接口                                *
 **************************************************************************/


//!\brief   云台模块初始化
//!\return  0表示初始化成功 -1表示初始化失败
int PTZ_Init()
{
#ifndef DEBUG_NOT_USE_PTZ_LIB	//调试时，不调用PTZ库
	CPtzManager *pManager = CPtzManager::PTZ_GetInstance();

	if (pManager != NULL)
	{
		pManager->PTZ_Init((char *)"/mnt/app/lib/IntDll", (char *)"/mnt/app/lib/ExtDll");
	}
#endif

	return IDVR_OK;
}

int MPU_PTZ_GetPtzKeyWorkMode()
{
	CConfig *Par = CConfig::GetInstance();

	if (NULL == Par)
	{
		MPU_ERR("CConfig::GetInstance() == NULL\n");
		return 0;
	}

	return Par->GetValue("PtzKeyWorkMode", "WorkMode", 0);
}


int MPU_PTZ_SetPtzKeyWorkMode(int Mode)
{
	CConfig *Par = CConfig::GetInstance();

	if (NULL == Par)
	{
		MPU_ERR("CConfig::GetInstance() == NULL\n");
		return 0;
	}

	return Par->SetValue("PtzKeyWorkMode", "WorkMode", Mode);
}

//!\brief   云台模块资源清除
//!\return  0表示成功 其他表示错误码
int PTZ_Uninit()
{
#ifndef DEBUG_NOT_USE_PTZ_LIB	//调试时，不调用PTZ库

	CPtzManager *pManager = CPtzManager::PTZ_GetInstance();

	if (pManager != NULL)
	{
		CPtzManager::PTZ_DestroyInstance(pManager);
	}

#endif

	return IDVR_OK;
}


/**************************************************************************
 *                            云台协议管理                                *
 **************************************************************************/

//!\brief   获取云台协议名称
//!\param   szProtocol              协议名缓冲区
//!\param   nLength                 缓冲区长度
//!\param   nChannel                通道号
//!\return  0表示获取成功 -1表示获取失败
int MPU_PTZ_GetProtocol(char *pszProtocol, int nLength, int nChannel)
{
	//调用云台管理器执行动作
	MPU_PTZ_CALL(PTZ_GetProtocol(pszProtocol, nLength, nChannel));
}

//!\brief   设置云台的协议名称
//!\param   szProtocol              云台协议名指针
//!\param   nChannel                通道号
//!\return  0表示获取成功 -1表示获取失败
int MPU_PTZ_SetProtocol(char *pszProtocol, int nChannel)
{
	//调用云台管理器执行动作
	MPU_PTZ_CALL(PTZ_SetProtocol(pszProtocol, nChannel));
}

//!\brief   获取通道的解码器地址
//!\param   nChannel                通道号
//!\return  -1表示获取失败，其他表示解码器地址
unsigned long MPU_PTZ_GetAddress(int nChannel)
{
	//调用云台管理器获取参数
	MPU_PTZ_GET(unsigned long, ((unsigned long) -1), PTZ_GetAddress(nChannel));
}

//!\brief   设置云台解码器地址
//!\param   dwAddress               新的解码器地址
//!\param   nChannel                通道号
//!\return  0表示成功，其他表示失败
int MPU_PTZ_SetAddress(unsigned long dwAddress, int nChannel)
{
	//调用云台管理器执行动作
	MPU_PTZ_CALL(PTZ_SetAddress(dwAddress, nChannel));
}

//!\brief   获取某一通道解码器的波特率
//!\param   nChannel                通道号
//!\return  -1为失败，其他为解码器波特率
unsigned long MPU_PTZ_GetBaudrate(int nChannel)
{
	//调用云台管理器获取参数
	MPU_PTZ_GET(unsigned long, ((unsigned long) -1), PTZ_GetBaudrate(nChannel));
}

//!\brief   设置解码器波特率
//!\param   dwBaudrate 新的波特率
//!\param   nChannel 通道号
//!\return  0表示设置成功，-1表示失败
int MPU_PTZ_SetBaudrate(unsigned long dwBaudrate, int nChannel)
{
	//调用云台管理器执行动作
	MPU_PTZ_CALL(PTZ_SetBaudrate(dwBaudrate, nChannel));
}

int MPU_PTZ_GetNormalCtrlPreset(int nChannel, int *nNormalCtrlPreset)
{
	MPU_PTZ_CALL(PTZ_GetNormalCtrlPreset(nChannel, nNormalCtrlPreset));
}

int MPU_PTZ_SetNormalCtrlPreset(int nChannel, int nNormalCtrlPreset)
{
	MPU_PTZ_CALL(PTZ_SetNormalCtrlPreset(nChannel, nNormalCtrlPreset));
}

int MPU_PTZ_SetWatchCareEnable(int nCh, int nEnable)
{
	MPU_PTZ_CALL(PTZ_SetWatchCareEnable(nCh, nEnable));
}

int MPU_PTZ_GetWatchCareEnable(int nCh,  int *nEnable)
{
	MPU_PTZ_CALL(PTZ_GetWatchCareEnable(nCh, nEnable));
}

int MPU_PTZ_SetWatchCareReTime(int nCh, int nTime)
{
	MPU_PTZ_CALL(PTZ_SetWatchCareReTime(nCh, nTime));
}

int MPU_PTZ_GetWatchCareReTime(int nCh,  int *nTime)
{
	MPU_PTZ_CALL(PTZ_GetWatchCareReTime(nCh, nTime));
}

int MPU_PTZ_GetWatchCareType(int nCh,  int *nType)
{
	MPU_PTZ_CALL(PTZ_GetWatchCareType(nCh, nType));
}

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
int MPU_PTZ_GetEletronicProp(int nPropIndex, int nChannel)
{
	//调用云台管理器获取参数
	MPU_PTZ_GET(int, 0, PTZ_GetEletronicProp(nPropIndex, nChannel));
}

//!\brief 设置解码器参数
//!\param nValue 参数值
//!\param nPropIndex 参数序号
//!\param nChannel 通道号
//!\return 0表示设置成功，-1表示失败
int MPU_PTZ_SetEletronicProp(int nValue, int nPropIndex, int nChannel)
{
	//调用云台管理器执行动作
	MPU_PTZ_CALL(PTZ_SetEletronicProp(nValue, nPropIndex, nChannel));
}

//!\brief 恢复解码器参数
//!\param nChannel 通道号
//!\return 0表示设置成功，-1表示失败
int MPU_PTZ_ResetAllEletronicProp(int nChannel)
{
	//调用云台管理器执行动作
	MPU_PTZ_CALL(PTZ_ResetAllEletronicProp(nChannel));
}

//!\brief 获取类型
int MPU_PTZ_GetType(int nChannel)
{
	//调用云台管理器获取参数
	MPU_PTZ_GET(int, ((unsigned long) 0), PTZ_GetType(nChannel));
}

//!\brief 获取当前机芯温度
int MPU_PTZ_GetTemperature(int nChannel, int *pnValue)
{
	//调用云台管理器执行动作
	MPU_PTZ_CALL(PTZ_GetTemperature(nChannel, pnValue));
}

//!\brief 获取当前坐标
//!\param nChannel 通道
//!\param pnHorizontalAngle水平角度0~360
//!\param pnVerticalAngle垂直角度-180~+180
//!\param pnMultiple 倍数 1~9999
int MPU_PTZ_GetCoordinate(int nChannel, int *pnHorizontalAngle, int *pnVerticalAngle, int *pnMultiple, int *pnOrientation)
{
	//调用云台管理器执行动作
	MPU_PTZ_CALL(PTZ_GetCoordinate(nChannel, pnHorizontalAngle, pnVerticalAngle, pnMultiple, pnOrientation));
}

//!\brief 获取球机485串口配置
//!\param nChannel 通道
//!\param pnAddress 485控制地址
//!\param pnProtocol    485控制协议编号
//!\param pnBaudRate    485波特率
int MPU_PTZ_Get485Config(int nChannel, int *pnAddress, int *pnProtocol, int *pnBaudRate)
{
	//调用云台管理器执行动作
	MPU_PTZ_CALL(PTZ_Get485Config(nChannel, pnAddress, pnProtocol, pnBaudRate));
}

//!\brief   返回可用（不可用的）云台协议列表
//!\param   bEnable                 true表示可用的协议 false表示不可用的协议
//!\param   pProtocolList           协议名数组头指针
//!\param   nLength                 协议名数组长度
//!\param   bExternal               协议是否外部协议的数组头指针,TRUE表示是外部协议, FALSE表示是内部协议
//!\return  返回协议个数
int MPU_PTZ_GetProtocolList(char pProtocolList[][256], int nLength, int bEnable, int bExternal[])
{
	//调用云台管理器执行动作
	MPU_PTZ_GET(int, -1, PTZ_GetProtocolList(pProtocolList, nLength, bEnable, bExternal));
}

//!\brief   返回云台协议（可用/不可用）的数目
//!\param   bEnable                 true表示可用的协议，false表示不可用的协议
//!\return  -1表示失败，其他值表示成功
int MPU_PTZ_GetProtocolCount(int bEnable)
{
	//调用云台管理器获取参数
	MPU_PTZ_GET(int, -1, PTZ_GetProtocolCount(bEnable));
}

//!\brief   安装云台协议
//!\param   pszSrcPath              协议文件路径
//!\return  0表示安装成功，其他表示安装失败
int MPU_PTZ_InstallProtocol(char *pszSrcPath)
{
	//调用云台管理器执行动作
	MPU_PTZ_CALL(PTZ_InstallProtocol(pszSrcPath));
}

//!\brief   卸载云台协议
//!\param   pszProtocolName         云台协议名
//!\param   pszDstPath              NULL表示直接删除，其他表示备份路径
//!\return  0表示卸载成功，其他表示卸载错误
int MPU_PTZ_UninstallProtocol(char *pszProtocolName, char *pszDstPath)
{
	//调用云台管理器执行动作
	MPU_PTZ_CALL(PTZ_UninstallProtocol(pszProtocolName, pszDstPath));
}

//!\brief   获取文件是否是合法的云台协议
//!\param   pszFileName     协议路径名
//!\param   pszProtocolName 协议名
//!\param   nLength         协议名缓冲区长度
//!\return  1表示合法 0表示非法
int MPU_PTZ_IsValidProtocol(char *pszFileName, char *pszProtocolName, int nLength)
{
	//调用云台管理器获取参数
	MPU_PTZ_GET(int, 0, PTZ_IsValidProtocol(pszFileName, pszProtocolName, nLength));
}


/**************************************************************************
 *                            云台操作                                    *
 **************************************************************************/

#ifdef EGUI_FOR_ICAMERA_NXP8850_BASED_DVR
//!\brief   电子PTZ控制
//!\param   dwPtzCode       云台控制码 见TTY_MPU_PTZ_CODE
//!\param   pdwParamList    控制参数
//!\param   nChannel            云台对应的通道号
//!\return  0表示成功，其他表示错误码
//!\sa TTY_MPU_PTZ_CODE
int MPU_PTZ_EletronicCtrl(int nChannel, unsigned long dwPtzCode, unsigned long *pdwParamList)
{
	//调用云台管理器执行动作
	MPU_PTZ_CALL(PTZ_EletronicCtrl(nChannel, dwPtzCode,(unsigned int *) pdwParamList));
}
#endif

//!\brief   云台一般控制
//!\param   dwPtzCode               云台控制码 见TTY_MPU_PTZ_CODE
//!\param   dwSpeed                 运动速度
//!\param   nChannel                云台对应的通道号
//!\return  0表示成功，其他表示错误码
//!\sa TTY_MPU_PTZ_CODE
int MPU_PTZ_NormalCtrl(unsigned long dwPtzCode, unsigned long dwSpeed, int nChannel)
{
	//调用云台管理器执行动作
	MPU_PTZ_CALL(PTZ_NormalCtrl(dwPtzCode, dwSpeed, nChannel));
}

//!\brief   预置点控制
//!\param   dwPtzCode               云台控制码
//!\param   dwPresetIndex           预置点号
//!\param   dwCruiseIndex           巡航点号
//!\param   dwSpeed                 速度
//!\param   dwDwellTime             停滞时间
//!\param   nChannel                通道号
//!\return  0表示成功 其他值表示错误码
//!\sa TTY_PRESET_CODE
int
MPU_PTZ_PresetCtrl(unsigned long dwPtzCode, unsigned long dwPresetIndex,
				   unsigned long dwCruiseIndex, unsigned long dwSpeed, unsigned long dwDwellTime, int nChannel)
{
	//调用云台管理器执行动作
	MPU_PTZ_CALL(PTZ_PresetCtrl(dwPtzCode, dwPresetIndex, dwCruiseIndex, dwSpeed, dwDwellTime, nChannel));
}

//!\brief   云台自动控制
//!\param   dwPtzCode 控制码参见 TTY_AUTO_CODE
//!\param   dwPresetIndex1 预置点号
//!\param   dwPresetIndex2 预置点号
//!\param   dwSpeed 运动速度
//!\param   bScan 开始/结束
//!\param   nChannel 通道号
//!\return  0表示成功 -1表示失败
//!\sa TTY_AUTO_CODE
int
MPU_PTZ_AutoCtrl(unsigned long dwPtzCode, unsigned long dwPresetIndex1,
				 unsigned long dwPresetIndex2, unsigned long dwCruiseIndex, unsigned long dwSpeed, unsigned long dwDwellTime, int bScan, int nChannel)
{
	MPU_PTZ_CALL(PTZ_AutoCtrl(dwPtzCode, dwPresetIndex1, dwPresetIndex2, dwCruiseIndex, dwSpeed, dwDwellTime, bScan, nChannel));
}

//!\brief   云台辅助设备控制
//!\param   dwPtzCode               云台辅助控制码,参见TTY_AUX_CODE
//!\param   bOn                     1=打开,0=关闭
//!\param   nChannel                云台解码器对应的通道号
//!\return  0表示成功 -1表示失败
//!\sa TTY_AUX_CODE
int MPU_PTZ_AuxCtrl(unsigned long dwPtzCode, int bOn, int nChannel)
{
	//调用云台管理器执行动作
	MPU_PTZ_CALL(PTZ_AuxCtrl(dwPtzCode, bOn, nChannel));
}

//!\brief   云台球机菜单控制
//!\param   dwPtzCode               菜单控制码
//!\param   nChannel                云台对应的通道号
//!\return  0表示执行成功，-1表示执行失败
//!\sa TTY_MENU_CODE
int MPU_PTZ_MenuCtrl(unsigned long dwPtzCode, int nChannel)
{
	//调用云台管理器执行动作
	MPU_PTZ_CALL(PTZ_MenuCtrl(dwPtzCode, nChannel));
}

//!\brief   获取云台是否支持相应的功能
//!\param   nPtzFlag                云台支持的功能标记 参见PTZ_FLAG
//!\param   nChannel                通道号
//!\return  0表示支持 -1表示不支持
//!\sa PTZ_FLAG
int MPU_PTZ_HasFunction(PTZ_FLAG nPtzFlag, int nChannel)
{
	//调用云台管理器获取参数
	MPU_PTZ_GET(int, -1, PTZ_HasFunction(nPtzFlag, nChannel));
}

//!\brief       获取云台技术参数接口
//!\param   nChannel 云台对应的通道号
//!\param   pnPresetCount       预置点个数
//!\param   pnCruiseCount       巡航线路条数
//!\param   pnTrackCount        轨迹条数
//!\param   pnAutoCount       线扫条数
//!\param   pdwBaudrate     支持的波特率
//!\return  0表示执行成功，-1表示执行失败
int MPU_PTZ_GetTechnicalData(int nChannel, int *pnPresetCount, int *pnCruiseCount, int *pnTrackCount, int *pnAutoCount, unsigned long *pdwBaudrate)
{
	MPU_PTZ_CALL(PTZ_GetTechnicalData(nChannel, pnPresetCount, pnCruiseCount, pnTrackCount, pnAutoCount, pdwBaudrate));
}

/**************************************************************************
 *                            透明通道功能
 *************************************************************************/
LPTransparentChanInterface MPU_TPC_GetNetInterface()
{
	MPU_TPC_GET(LPTransparentChanInterface, NULL, GetNetInterface());
}

int MPU_TPC_SetWriteCallback(IF_TransparentChannelWrite cbWrite)
{
	MPU_TPC_CALL(SetIFTransparentChannelWrite(cbWrite));
}

int MPU_TPC_StartTest()
{
	MPU_TPC_CALL(Start());
}

int MPU_TPC_StopTest()
{
	MPU_TPC_CALL(Stop());
}

/************************************************************************/

/*                     云台守望功能接口                                      */

/************************************************************************/
#include "PTZ/CPtzAdjuster.h"

#ifndef DEBUG_NOT_USE_PTZ_LIB	//调试时，不调用PTZ库

//调用云台守望者执行动作
#define MPU_PTZADJ_CALL(x)								\
do														\
{														\
	int nRet;											\
														\
	CPtzAdjuster *pAdj = CPtzAdjuster::GetInstance();	\
														\
	if (NULL != pAdj)									\
	{													\
		nRet = pAdj->x;									\
		if (IDVR_OK != nRet)							\
		{												\
			MPU_ERR("%s, nRet = %d\n", #x, nRet);		\
		}												\
	}													\
	else												\
	{													\
		MPU_ERR("Get CPtzAdjuster instance fail!\n");	\
		MPU_CHK(NULL != pAdj);							\
		nRet = IDVR_MPU_PTZ_NO_MANAGER_INSTANCE;		\
	}													\
														\
	return nRet;										\
}														\
while (0)

//调用云台守望者获取参数
#define MPU_PTZADJ_GET(DataType, DefaultValue, Function)	\
do															\
{															\
	DataType retValue;										\
															\
	CPtzAdjuster *pAdj = CPtzAdjuster::GetInstance();		\
															\
	if (NULL != pAdj)										\
	{														\
		retValue = pAdj->Function;							\
	}														\
	else													\
	{														\
		MPU_ERR("Get CPtzManager instance fail!\n");		\
		MPU_CHK(NULL != pAdj);								\
		retValue = DefaultValue;							\
	}														\
															\
	return retValue;										\
}															\
while (0)

#else

//调用云台守望者执行动作
#define MPU_PTZADJ_CALL(x)								\
do														\
{														\
	return IDVR_OK;										\
}														\
while (0)

//调用云台守望者获取参数
#define MPU_PTZADJ_GET(DataType, DefaultValue, Function)	\
do															\
{															\
	return DefaultValue;									\
}															\
while (0)

#endif

#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
int MPU_PTZ_GetIsSupportMoto()
{
	MPU_PTZ_CALL(IsSupportBuiltInMotor());
}
#endif

int MPU_PTZ_Enable(int bEnable, int nChannel)
{
	MPU_PTZADJ_CALL(EnablePTZ(bEnable, nChannel));
}

int MPU_PTZ_IsEnable(int nChannel)
{
	MPU_PTZADJ_GET(int, 0, IsEnablePTZ(nChannel));
}

int MPU_PTZ_IsEnableKeeper(int nChannel)
{
	MPU_PTZADJ_GET(int, 0, IsEnableKeeper(nChannel));
}

int MPU_PTZ_EnableKeeper(int bEnable, int nChannel)
{
	MPU_PTZADJ_CALL(EnableKeeper(bEnable, nChannel));
}

int MPU_PTZ_GetResetCycle(int nChannel)
{
	MPU_PTZADJ_GET(int, 0, GetResetCycle(nChannel));
}

int MPU_PTZ_SetResetCycle(int nSecond, int nChannel)
{
	MPU_PTZADJ_CALL(SetResetCycle(nSecond, nChannel));
}

int MPU_PTZ_GetDefaultAction(unsigned long *pdwActionType, unsigned long *pdwIndex, int nChannel)
{
	MPU_PTZADJ_CALL(GetDefaultAction(pdwActionType, pdwIndex, nChannel));
}

int MPU_PTZ_SetDefaultAction(unsigned long dwActionType, unsigned long dwIndex, int nChannel)
{
	MPU_PTZADJ_CALL(SetDefaultAction(dwActionType, dwIndex, nChannel));
}

int MPU_PTZ_Alarm(unsigned long dwActionType, unsigned long dwIndex, int nChannel)
{
	MPU_PTZADJ_CALL(Alarm(dwActionType, dwIndex, nChannel));
}

int MPU_PTZ_ClearAlarm(int nChannel)
{
	MPU_PTZADJ_CALL(ClearAlarm(nChannel));
}

#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
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
									int nEndPresetActionForWatch)
{
	//调用云台管理器执行动作
	MPU_PTZADJ_CALL(SetAdvanceCfg(nChannel, nCruiseEnable, 
									nBeginPresetNumForCruise,
									nBeginPresetActionForCruise,
									nEndPresetNumForCruise,
									nEndPresetActionForCruise,
									nWatchEnable,
									nBeginPresetNumForWatch,
									nBeginPresetActionForWatch,
									nEndPresetNumForWatch,
									nEndPresetActionForWatch));
}

//!\brief   设置云台先进配置
//!\param                 
//!\param                   
//!\return  0表示获取成功 -1表示获取失败
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
									int nEndPresetActionForTrack)
{
	//调用云台管理器执行动作
	MPU_PTZADJ_CALL(SetAdvanceCfgV1(nChannel, nCruiseEnable, 
									nBeginPresetNumForCruise,
									nBeginPresetActionForCruise,
									nEndPresetNumForCruise,
									nEndPresetActionForCruise,
									nWatchEnable,
									nBeginPresetNumForWatch,
									nBeginPresetActionForWatch,
									nEndPresetNumForWatch,
									nEndPresetActionForWatch,
									nTrackEnable,
									nBeginPresetNumForTrack,
									nBeginPresetActionForTrack,
									nEndPresetNumForTrack,
									nEndPresetActionForTrack));
}

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
									int *nEndPresetActionForWatch)
{
	//调用云台管理器执行动作
	MPU_PTZADJ_GET(int , 0, GetAdvanceCfg(nChannel, nCruiseEnable, 
									nBeginPresetNumForCruise,
									nBeginPresetActionForCruise,
									nEndPresetNumForCruise,
									nEndPresetActionForCruise,
									nWatchEnable,
									nBeginPresetNumForWatch,
									nBeginPresetActionForWatch,
									nEndPresetNumForWatch,
									nEndPresetActionForWatch));
}
									
//!\brief   获取云台先进配置
//!\param                 
//!\param                   
//!\return  0表示获取成功 -1表示获取失败
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
									int *nEndPresetActionForTrack)
{
	//调用云台管理器执行动作
	MPU_PTZADJ_GET(int , 0, GetAdvanceCfgV1(nChannel, nCruiseEnable, 
									nBeginPresetNumForCruise,
									nBeginPresetActionForCruise,
									nEndPresetNumForCruise,
									nEndPresetActionForCruise,
									nWatchEnable,
									nBeginPresetNumForWatch,
									nBeginPresetActionForWatch,
									nEndPresetNumForWatch,
									nEndPresetActionForWatch,
									nTrackEnable,
									nBeginPresetNumForTrack,
									nBeginPresetActionForTrack,
									nEndPresetNumForTrack,
									nEndPresetActionForTrack));
}
#endif

//!\brief   设置云台速度
//!\param   nSpeed               速度等级
//!\param   nChannel                通道号
//!\return  0表示成功，其他表示失败
int MPU_PTZ_SetSpeed(int nSpeed, int nChannel)
{
	MPU_PTZADJ_CALL(SetSpeed(nSpeed, nChannel));
}

//!\brief   设置ptzV速度
//!\param   nChannel  通道号     nSpeedV  速度
//!\return  -1为失败
int MPU_PTZ_SetSpeedV(int nSpeedV, int nChannel)
{
	MPU_PTZADJ_CALL(SetSpeedV(nSpeedV, nChannel));
}

//!\brief   设置ptzH速度
//!\param   nChannel  通道号     nSpeedH  速度
//!\return  -1为失败
int MPU_PTZ_SetSpeedH(int nSpeedH, int nChannel)
{
	MPU_PTZADJ_CALL(SetSpeedH(nSpeedH, nChannel));
}

int MPU_PTZ_GetSpeed(int nChannel)
{
	//调用云台管理器获取参数
	MPU_PTZADJ_GET(int, 1, GetSpeed(nChannel));
}

//!\brief   设置ptz 水平默认值
//!\param   nChannel  通道号     nHorizontalAngle  水平默认值
//!\return  -1为失败
int MPU_PTZ_SetHorizontalAngle(int nChannel, int nHorizontalAngle)
{
	MPU_PTZ_CALL(PTZ_SetHorizontalAngle(nChannel, nHorizontalAngle));
}

//!\brief   设置ptz 纵向默认值
//!\param   nChannel  通道号     nVerticalAngle  纵向默认值
//!\return  -1为失败
int MPU_PTZ_SetVerticalAngle(int nChannel, int nVerticalAngle)
{
	MPU_PTZ_CALL(PTZ_SetVerticalAngle(nChannel, nVerticalAngle));
}

//!\brief	设置ptz 水平反向
//!\param	nChannel  通道号 	nHorizontalReverse  水平反向
//!\return	-1为失败
int MPU_PTZ_SetHorizontalReverse(int nChannel, int nHorizontalReverse)
{
	MPU_PTZ_CALL(PTZ_SetHorizontalReverse(nChannel, nHorizontalReverse));
}

//!\brief   设置ptz 纵向反向
//!\param   nChannel  通道号     nVerticalReverse  纵向反向
//!\return  -1为失败
int MPU_PTZ_SetVerticalReverse(int nChannel, int nVerticalReverse)
{
	MPU_PTZ_CALL(PTZ_SetVerticalReverse(nChannel, nVerticalReverse));
}

//!\brief   设置ptz 水平步长
//!\param   nChannel  通道号     nHorizontalStep  水平步长
//!\return  -1为失败
int MPU_PTZ_SetHorizontalStep(int nChannel, int nHorizontalStep)
{
	MPU_PTZ_CALL(PTZ_SetHorizontalStep(nChannel, nHorizontalStep));
}

//!\brief   设置ptz 纵向步长
//!\param   nChannel  通道号     nVerticalStep  纵向步长
//!\return  -1为失败
int MPU_PTZ_SetVerticalStep(int nChannel, int nVerticalStep)
{
	MPU_PTZ_CALL(PTZ_SetVerticalStep(nChannel, nVerticalStep));
}

//!\brief   设置ptz 水平速度
//!\param   nChannel  通道号     nHorizontalStep  水平速度
//!\return  -1为失败
int MPU_PTZ_SetMaxSpeedH(int nChannel, int nMaxSpeedH)
{
	MPU_PTZ_CALL(PTZ_SetMaxSpeedH(nChannel, nMaxSpeedH));
}

//!\brief   设置ptz 纵向速度
//!\param   nChannel  通道号     nVerticalStep  纵向速度
//!\return  -1为失败
int MPU_PTZ_SetMaxSpeedV(int nChannel, int nMaxSpeedV)
{
	MPU_PTZ_CALL(PTZ_SetMaxSpeedV(nChannel, nMaxSpeedV));
}


//设置用户定制云台垂直最大步长
int MPU_PTZ_SetUserVMaxStep(int nChannel, int nUserVMaxStep)
{
	MPU_PTZ_CALL(PTZ_SetUserVMaxStep(nChannel, nUserVMaxStep));
}

//设置产测状态到云台模块
int MPU_PTZ_SetProductTestMode(int nTestMode)
{
	MPU_PTZ_CALL(PTZ_SetProductTestMode(nTestMode));
}

//!\brief   设置ptz 支持垂直跟踪
//!\param   nChannel  通道号     nSupportVertical  垂直跟踪
//!\return  -1为失败
int MPU_PTZ_SetSupportVertical(int nChannel, int nSupportVertical)
{
	MPU_PTZ_CALL(PTZ_SetSupportVertical(nChannel, nSupportVertical));
}

//!\brief   设置ptz 支持云台
//!\param   nChannel  通道号     nSupporMotor  支持云台
//!\return  -1为失败
int MPU_PTZ_SetSupportMotor(int nChannel, int nSupporMotor)
{
	MPU_PTZ_CALL(PTZ_SetSupporMotor(nChannel, nSupporMotor));
}

//!\brief   设置ptz 支持电机交换
//!\param   nSupporSwap  支持电机交换
//!\return  -1为失败
int MPU_PTZ_SetPTZDevSwapLRPTZChannel(int nSupporSwap)
{
	MPU_PTZ_CALL(PTZ_SetPTZDevSwapLRPTZChannel(nSupporSwap));
}


//更新设备休眠状态
int MPU_PTZ_UpdateDorStatus(int bDorEnable)
{
	MPU_PTZ_CALL(UpdateDorStatus(bDorEnable));
}

#undef  MPU_PTZADJ_CALL
#undef	MPU_PTZADJ_GET

/**************************************************************************
 *                            注销局部宏定义                              *
 **************************************************************************/

#undef MPU_PTZ_CALL				//调用云台管理器执行动作
#undef MPU_PTZ_GET				//调用云台管理器获取参数值
