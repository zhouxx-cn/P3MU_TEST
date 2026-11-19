// Copyright (C) 1991 - 1999 Rational Software Corporation

#ifndef __CPTZMANAGER_H_
#define __CPTZMANAGER_H_

#include <pthread.h>
#include <dirent.h>
#include <semaphore.h>
#include "global_macro.h"
#include "CProtocol.h"
#include "tagSerialPort.h"
#include <dlfcn.h>

#include "zwsdk.h"

class CPtzDev;

class CPtzEletronic;

class CPtzAdjuster;

typedef struct tagProtocolLib ProtocolLib, *LPProtocolLib;

typedef int	(*PTZ_CONTROL_MODE_CALLBACK)(struct tagZW_IPC_DAY_NIGHT *pcfg);
typedef int	(*PTZ_CONTROL_LIGHTBOARD_MODE_CALLBACK)(struct tagZW_COM_ISP_EX *pcfg);

//巡航
typedef struct
{
    int speed;		             // 巡航速度
    unsigned int nRetentionTime; // 停留时间(S)
	unsigned int presetIdx;	     // 当前预置点编号
} PTZ_CRUISE_NODE;

typedef struct
{
	int idx;		             // 当前序号
    int bStop;		             // 停止命令
	int validSize;               // 预置点个数
    int cruise_enable;			 // 巡航的启动状态 0 - 开启 1 - 关闭
	int cruise_status;			 // 巡航的运行状态 0 - 运行 1 - 未运行
    unsigned int nAllTime;       // 巡航配置总时间(S)
    unsigned int nCntTime;       // 巡航已运行时间(MS)
    PTZ_CRUISE_NODE node[8];
	pthread_t tid_cruise;
} PTZ_CRUISE_INFO;

//!\brief 云台管理接口类提供本模块所需要的功能
//##ModelId=49A23BA40209
class CPtzManager
{
	friend class CPtzDev;
  public:
	//!\brief 
	//获取CPtzManager实例的指针,若是第一次调用则完成一下任务:
	//!1.读取各解码器配置，每个通道构建一个CPtzDev对象，形成CP
	//tzDev数组
	//!2.获取CPtzDev的协议名称，创建CProtocol对象数组,设置该协
	//议对象对应的CPtzDev
	//!return NULL表示获取失败，其他表示CPtzManager指针
	//##ModelId=49A4E1A00271
	static CPtzManager *PTZ_GetInstance();

	//!\brief 释放CPtzManager实例
	//!\param pInstance 指向CPtzManager实例的指针
	//!\return 无
	//##ModelId=49A4E1D502BF
	static void PTZ_DestroyInstance(CPtzManager * pInstance);

	int PTZ_Init(char *szInternalPath, char *szExternalPath);

	//!\brief 预置点控制其他功能，比如灯板、AI智能目标框、语音联动等
	//!\param dwPresetIndex 预置点号,实际值是减了1的
	int PTZ_ControlFuncEx(unsigned long dwPtzCode, unsigned long dwPresetIndex, int nChannel);

	//!\brief 获取云台协议名称
	//!\param szProtocol 协议名缓冲区
	//!\param nLength 缓冲区长度
	//!\param nChannel 通道号
	//!\return 0表示获取成功 -1表示获取失败
	//##ModelId=49A37005030D
	int PTZ_GetProtocol(char *szProtocol, int nLength, int nChannel);

	//!\brief 设置云台的协议名称
	//!\param szProtocol 云台协议名指针
	//!\param nChannel 通道号
	//!\return 0表示设置成功
	//##ModelId=49A49A110251
	int PTZ_SetProtocol(char *szProtocol, int nChannel);

	//!\brief 获取云台技术参数接口
	//!\param nChannel 云台对应的通道号
	//!\param PresetCount 预置点个数
	//!\param CruiseCount 巡航线路条数
	//!\param TrackCount 轨迹条数
	//!\param AutoCount 线扫条数
	//!\param 支持的波特率
	//!\return 无
	int PTZ_GetTechnicalData(int nChannel, int *PresetCount, int *CruiseCount, int *TrackCount, int *AutoCount, unsigned long *Baudrate);

	//!\brief 获取通道的解码器地址
	//!\param nChannel 通道号
	//!\return -1表示获取失败，其他表示解码器地址
	//##ModelId=49B71BF3038A
	unsigned long PTZ_GetAddress(int nChannel);

	//!\brief 设置云台解码器地址
	//!\param dwAddress 新的解码器地址
	//!\param nChannel 通道号
	//!\return 0表示成功，其他表示失败
	//##ModelId=49B71BFD02FD
	int PTZ_SetAddress(unsigned long dwAddress, int nChannel);

	//!\brief 获取某一通道解码器的波特率
	//!\param nChannel通道号
	//!\return -1为失败，其他为解码器波特率
	//##ModelId=49B71C070280
	unsigned long PTZ_GetBaudrate(int nChannel);

	//!\brief 设置解码器波特率
	//!\param dwBaudrate 新的波特率
	//!\param nChannel 通道号
	//!\return 0表示设置成功，-1表示失败
	//##ModelId=49B71C170186
	int PTZ_SetBaudrate(unsigned long dwBaudrate, int nChannel);

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
	int PTZ_GetEletronicProp(int nPropIndex, int nChannel);

	//!\brief 设置解码器参数
	//!\param nValue 参数值
	//!\param nPropIndex 参数序号
	//!\param nChannel 通道号
	//!\return 0表示设置成功，-1表示失败
	int PTZ_SetEletronicProp(int nValue, int nPropIndex, int nChannel);

	//!\brief 恢复解码器参数
	//!\param nChannel 通道号
	//!\return 0表示设置成功，-1表示失败
	int PTZ_ResetAllEletronicProp(int nChannel);

	int PTZ_GetType(int nChannel);

	int PTZ_GetTemperature(int nChannel, int *pnValue);

	int PTZ_GetCoordinate(int nChannel, int *pnHorizontalAngle, int *pnVerticalAngle, int *pnMultiple, int *pnOrientation);

	int PTZ_Get485Config(int nChannel, int *pnAddress, int *pnProtocol, int *pnBaudRate);

	int PTZ_OpenCom(int nChannel);

	//!\brief 返回可用（不可用的）云台协议列表
	//!\param bEnable true表示可用的协议 false表示不可用的协议
	//!\param pProtocolList 协议名数组头指针
	//!\param nLength 协议名数组长度
	//!\return 成功返回0，失败返回-1
	//##ModelId=49B46A4C0222
	int PTZ_GetProtocolList(char pProtocolList[][256], int nLength, int bEnable, int bExternal[] = NULL);

	//!\brief 返回云台协议（可用/不可用）的数目
	//!\param bEnable 
	//true表示可用的协议，false表示不可用的协议
	//!\return -1表示失败，其他值表示成功
	//##ModelId=49B47F0B0399
	int PTZ_GetProtocolCount(int bEnable);

	//!\brief 安装云台协议
	//!\param strSrcPath 协议文件路径
	//!\return 0表示安装成功，其他表示安装失败
	//##ModelId=49B46A6201B5
	int PTZ_InstallProtocol(char *strSrcPath);

	//!\brief 卸载云台协议
	//!\param szProtocol 云台协议名
	//!\param szDstPath NULL表示直接删除，其他表示备份路径
	//!\return 0表示卸载成功，其他表示卸载错误
	//##ModelId=49B46A6F003E
	int PTZ_UninstallProtocol(char *szProtocol, char *szDstPath);

#ifdef EGUI_FOR_ICAMERA_NXP8850_BASED_DVR
	//!\brief   电子PTZ控制
	//!\param   dwPtzCode       云台控制码 见TTY_MPU_PTZ_CODE
	//!\param   pdwParamList    控制参数
	//!\param   nChannel            云台对应的通道号
	//!\return  0表示成功，其他表示错误码
	//!\sa TTY_MPU_PTZ_CODE
	int PTZ_EletronicCtrl(int nChannel, unsigned long dwPtzCode, unsigned int *pdwParamList);
#endif

#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
	//!\brief	PTZ移动原始位置
	//!\param	nHPos	 相对中心水平的位置
	//!\param	nVPos	 相对中心垂直的位置
	//!\param	nChannel			云台对应的通道号
	//!\return	0表示成功，其他表示错误码
	int PTZ_NormalCtrlToPos(int nChannel);

	int PTZ_NormalCtrlToPreset(int nChannel);

	int PTZ_GetNormalCtrlPreset(int nChannel, int *nNormalCtrlPreset);

	int PTZ_SetNormalCtrlPreset(int nChannel, int nNormalCtrlPreset);

    int PTZ_SetWatchCareEnable(int nChannel, int iEnable);
    int PTZ_GetWatchCareEnable(int nChannel, int *piEnable);
    int PTZ_SetWatchCareReTime(int nChannel, int iTimes);    
    int PTZ_GetWatchCareReTime(int nChannel, int *piTime);
    int PTZ_GetWatchCareType(int nChannel, int *nType);

	int PTZ_MotoPosCalibration(int nCh);

	int PTZ_GetSelfCheckState(int *Status);

	int PTZ_SetAutoCalibrationEnable(int nCh, int nEnable);

	int PTZ_GetAutoCalibrationEnable(int nCh,  int *nEnable);

	int PTZ_IsCalibrationStart();
	
	int PTZ_SetCalibrationReset();

	int PTZ_IsAdaptProtocol(int nChannel);

	//!\brief	PTZ移动绝对位置
	//!\param	nHPos	 相对中心水平的位置
	//!\param	nVPos	 相对中心垂直的位置
	//!\param	nChannel			云台对应的通道号
	//!\return	0表示成功，其他表示错误码
	int PTZ_NormalCtrlToPos(int nChannel, int nHPos, int nVPos);

	//!\brief	PTZ移动相对位置是否移动判断
	//!\param	nHPos	 相对中心水平的位置
	//!\param	nVPos	 相对中心垂直的位置
	//!\param	nChannel			云台对应的通道号
	//!\return	0表示不需要移动，1表示需要移动
	int PTZ_PreNormalCtrlToPosExt(int nChannel, int nHPos, int nVPos);

	int PTZ_NormalCtrlToPosExt(int nChannel, int nHPos, int nVPos);
	
	int PTZ_SetTrackSpeed(int speedH, int speedV, int nChannel);

	int PTZ_ReturnToOriginalSpeed(int nChannel);
	
	void PTZ_GetIfInit(int *pIfInit);
	
	//!\brief 获取电机的状态
	//!\param iStatus 
	//!\return 0表示成功，其他表示错误码
	int PTZ_IsMotoStatus(int *iStatus);

	//!\brief 获取电机自检恢复初始位置的状态
	//!\param iStatus 
	//!\return 0表示成功，其他表示错误码
	int PTZ_GetStartStatus(int *iStatus);

	int PTZ_GetPTZCtrlStatue(int *nPTZCtrlStatue);

	int SetTrackStayTime(int nStayTimeSec);

	int GetTrackStayTime(int *nStayTimeSec);

	int PTZ_SetPTZCtrlStatue(int nPTZCtrlStatue);	

#ifdef __AGINGTEST__
	//!\brief 获取云台范围（配置值）
	int PTZ_GetMaxStep (unsigned int nChannel, int *pnHMaxStep, int *pnVMaxStep);
#endif	

	//获取手动控制云台标识
	int PTZ_GetManualMovePtzFlag();
  
	//清除手动控制云台标识
	void PTZ_ClearManualMovePtzFlag();
  
	//获取手动控制云台开始时间
	long PTZ_GetManualMoveTimeBegin();
  
	//设置手动控制云台开始时间
	void PTZ_SetManualMoveTimeBegin(long nTimes);	

	//!\brief   设置ptz 水平默认值
	//!\param   nChannel  通道号     nHorizontalAngle  水平默认值
	//!\return  -1为失败
	int PTZ_SetHorizontalAngle(int nChannel, int nHorizontalAngle);

	//!\brief   设置ptz 纵向默认值
	//!\param   nChannel  通道号     nVerticalAngle  纵向默认值
	//!\return  -1为失败
	int PTZ_SetVerticalAngle(int nChannel, int nVerticalAngle);

	int IsSuportCustomSpeed();
	
	//!\brief	设置ptz 水平反向
	//!\param   nChannel  通道号     nVerticalAngle  纵向反向
	//!\return  -1为失败	
	int PTZ_SetHorizontalReverse(int nChannel, int nHorizontalReverse);
	
	//!\brief   设置ptz 纵向反向
	//!\param   nChannel  通道号     nVerticalReverse  纵向反向
	//!\return  -1为失败
	int PTZ_SetVerticalReverse(int nChannel, int nVerticalReverse);
	
	//!\brief   设置ptz 水平步长
	//!\param   nChannel  通道号     nHorizontalStep  水平步长
	//!\return  -1为失败
	int PTZ_SetHorizontalStep(int nChannel, int nHorizontalStep);
	
	//!\brief   设置ptz 纵向步长
	//!\param   nChannel  通道号     nVerticalStep  纵向步长
	//!\return  -1为失败
	int PTZ_SetVerticalStep(int nChannel, int nVerticalStep);

	//!\brief   设置ptz 水平速度
	//!\param   nChannel  通道号     nHorizontalStep  水平速度
	//!\return  -1为失败
	int PTZ_SetMaxSpeedH(int nChannel, int nMaxSpeedH);
	
	//!\brief   设置ptz 纵向速度
	//!\param   nChannel  通道号     nVerticalStep  纵向速度
	//!\return  -1为失败
	int PTZ_SetMaxSpeedV(int nChannel, int nMaxSpeedV);


    //设置用户定制云台垂直最大步长
    int PTZ_SetUserVMaxStep(int nChannel, int nUserVMaxStep);

    //设置产测状态到云台模块
    int PTZ_SetProductTestMode(int nTestMode);

	//获取产测状态
	int PTZ_GetProductTestMode();

	//!\brief   设置ptz 支持垂直跟踪
	//!\param   nChannel  通道号     SupportVertical  垂直跟踪
	//!\return  -1为失败
	int PTZ_SetSupportVertical(int nChannel, int nSupportVertical);

	//!\brief   设置ptz 支持云台
	//!\param   nChannel  通道号     SupporMotor  支持云台
	//!\return  -1为失败
	int PTZ_SetSupporMotor(int nChannel, int nSupporMotor);
	
	int PTZ_SetPTZDevSwapLRPTZChannel(int nSupporSwap);
#endif

	//!\brief 云台一般控制
	//!\param dwPtzCode 云台控制码 见TTY_PTZ_CODE
	//!\param dwSpeed 运动速度
	//!\param nChannel 云台对应的通道号
	//!\return 0表示成功，其他表示错误码
	//!\sa TTY_PTZ_CODE
	//##ModelId=49B472B10186
	int PTZ_NormalCtrl(unsigned long dwPtzCode, unsigned long dwSpeed, int nChannel);

	//!\brief 预置点控制
	//!\param dwPtzCode 云台控制码
	//!\param dwPresetIndex 预置点号
	//!\param dwCruiseIndex 巡航点号
	//!\param dwSpeed 速度
	//!\param dwDwellTime停滞时间
	//!\param nChannel 通道号
	//!\return 0表示成功 其他值表示错误码
	//!\sa TTY_PRESET_CODE
	//##ModelId=49B472BB01A5
	int PTZ_PresetCtrl(unsigned long dwPtzCode, unsigned long dwPresetIndex, unsigned long dwCruiseIndex, unsigned long dwSpeed,
					   unsigned long dwDwellTime, int nChannel);

	//!\brief 云台自动控制
	//!\param dwPtzCode 控制码参见 TTY_AUTO_CODE
	//!\param dwPresetIndex1 预置点号
	//!\param dwPresetIndex2 预置点号
	//!\param dwSpeed 运动速度
	//!\param  bScan 开始/结束
	//!\param nChannel 通道号
	//!\return 0表示成功 -1表示失败
	//!\sa TTY_AUTO_CODE
	//##ModelId=49B472C200EA
	int PTZ_AutoCtrl(unsigned long dwPtzCode, unsigned long dwPresetIndex1, unsigned long dwPresetIndex2, unsigned long dwCruiseIndex,
					 unsigned long dwSpeed, unsigned long dwDwellTime, int bScan, int nChannel);

	//!\brief 云台辅助设备控制
	//!\param dwPtzCode 云台辅助控制码,参见TTY_AUX_CODE
	//!\param bOn 1=打开,0=关闭
	//!\param nChannel 云台解码器对应的通道号
	//!\return 0表示成功 -1表示失败
	//!\sa TTY_AUX_CODE
	//##ModelId=49B472CE036B
	int PTZ_AuxCtrl(unsigned long dwPtzCode, int bOn, int nChannel);

	//!\brief 云台球机菜单控制
	//!\param dwPtzCode 菜单控制码
	//!\param nChannel 云台对应的通道号
	//!\return 0表示执行成功，-1表示执行失败
	//!\sa TTY_MENU_CODE
	//##ModelId=49B472DC02CE
	int PTZ_MenuCtrl(unsigned long dwPtzCode, int nChannel);

	//!\brief 获取云台是否支持相应的功能
	//!\param nPtzFlag 云台支持的功能标记 参见PTZ_FLAG
	//!\param nChannel通道号
	//!\return 1表示支持 0表示不支持
	//!\sa PTZ_FLAG
	//##ModelId=49BB2D3B0251
	int PTZ_HasFunction(int nPtzFlag, int nChannel);

	int PTZ_StartMotorCruise(int nChannel);

	int PTZ_StopMotorCruise(int nChannel);

    int PTZ_GetCuiseStatue(int *nCruiseStatue);

	//!\brief 获取文件是否是合法的云台协议
	//!\param szFileName 协议路径名
	//!\param pProtocolName 协议名
	//!\param nLength 协议名缓冲区长度，包括'\0'
	//!\return 1表示合法 0表示非法, 其他表示缓冲区长度不够，返回实际协议名的长度
	int PTZ_IsValidProtocol(char *szFileName, char *pProtocolName, int nLength);

#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
	//!\brief 是否支持内置云台
	int IsSupportBuiltInMotor();

	//!\brief 是否支持垂直方向跟踪
	int IsSupportTrackVertical();
#endif	

    int PTZ_PrivacyMaskPreset(int nChannel, int AwakeMode);

    int UpdateDorStatus(int bDorEnable);
	
#if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)
	  //获取电机当前状态
	  int PTZ_GetMotorState(int *state);
#endif

  private:
	  CPtzManager();

	 ~CPtzManager();

	//!\brief 读取所有云台协议名称
	//!\param pProtocol 云台协议名数组的指针
	//!\param nLength 加载的协议个数
	//!\param szPath 加载文件夹
	//!\param nMax 最大读取个数
	//!\param nEnable 0表示已卸载的协议 1表示未卸载的协议
	//!\return 0表示成功 -1表示失败
	int GetProtocolList(char pProtocol[][MAX_PROTONAME_LEN + 2], int *nLength, char *szPath, int nMax);

	//!\brief 读取各解码器参数
	int InitDev();

	//!\brief 判断协议是内/外部协议的第几个
	//!\param szProtocol 协议名
	//!\param bExternal 是内部协议还是外部协议
	//!\return 大于等于0表示第几个 -1表示不存在
	int GetProtocolIndex(char *szProtocol, int bExternal);

	//!\brief 检查协议文件名是否合法
	//!\param szProtocolFileName 文件名完整路径
	//!\param 协议名缓冲区,文件名合法时，将协议名存入该缓冲区，例：文件名/usb/sdc1/libPelcoD.so将PelcoD存入szProtocolName
	//!\param nLength 缓冲区长度
	//!\return 1表示合法 0表示不合法
	int IsValidProtocolFileName(char *szProtocolFileName, char *szProtocolName, int nLength);

	//!\brief 判断协议是否是已有并可用的协议
	//!\return true表示可用 false表示不可用
	bool IsProtocolExist(char *szProtocol);

	//!\brief 检查协议是否合法
	//!\param szFileName 完整文件路径
	//!\param szProtocolName 协议名缓冲区
	//!\return 0表示协议合法，其他表示错误码
	int CheckProtocol(char *szFileName, char szProtocolName[MAX_PROTONAME_LEN + 20]);

	//!\brief 加载协议,根据给定的协议名加载内部/外部协议
	//!\param pProtocol 指向协议包装类的指针
	//!\param szProtocol 协议名
	//!\return 0表示加载成功，其他表示错误码
	int LoadProtocol(LPProtocolLib pProtocol, char *szProtocol);

	//!\brief 开始某个通道的软巡航
	//!\param nChannel 通道号 0~MAX_ENC_CHAN_NUM
	//!\dwCruise 巡航线路号 0~16
	//!\return 0表示成功 其他表示错误码
	static int StartSoftwareCruise(unsigned int nChannel, unsigned int dwCruise);

	//!\biref 停止某个通道的巡航
	//!\param 通道号
	//!\return 0表示成功
	static int StopSoftwareCruise(unsigned int nChannel);

	//!\brief 软件巡航主函数
	static void *SoftwareCruise(void *arg);

	//!\brief 加载所有通道的巡航设置
	int LoadCruise();

	//!\brief 保存所有通道的巡航设置
	int SaveCruise();

	void InitEletronicType(unsigned int nChannel);

	//!\brief 从文件路径中获取文件名
	//!\param 路径字符串
	//!\return 指向文件名首字母的指针
	static char *GetFileNameFromPath(char *szPath);

	//!\brief 从USB盘中拷贝文件到flash
	//!\param szSrcPath 源路径
	//!\param szDstPath 目的路径
	//!\param bRemove 拷贝完之后是否删除源文件
	//!\return 0表示拷贝成功 其他表示错误码
	static int CopyFileToUsb(char *szSrcPath, char *szDstPath, int bRemove = 0);

	//!\brief 从flash中拷贝文件到邋邋USB盘
	//!\param szSrcPath 源路径
	//!\param szDstPath 目的路径
	//!\param bRemove 拷贝完之后是否删除源文件
	//!\return 0表示拷贝成功 其他表示错误码
	static int CopyFileFromUsb(char *szSrcPath, char *szDstPath, int bRemove = 1);

	static int FnSendRawData(unsigned char *pData, unsigned int uLength);

	//!\brief 发送云台控制命令的回调
	static int FnSendData(unsigned int nChannel, void *pBuffer, int nLength);

	static int FnRecvRawData(unsigned char *pData, unsigned int uLength);

	//!\brief 接受解码器数据的回调
	static int FnRecvData(unsigned int nChannel, void *pBuffer, int nLength);

	/*! 设置巡航参数 - 仅对gpio协议有效*/
    int SetMotorCruise();

	/*! 开启巡航 - 仅对gpio协议有效*/
	int StartMotorCruise();

	/*! 关闭巡航 - 仅对gpio协议有效*/
	int StopMotorCruise();

    friend void *thread_do_cruise(void *param);

    int IsSuportPtz();
  private:
	static CPtzManager *m_pInstance;

	char m_aszProtocol[2][MAX_PROTOCOL_NUM][MAX_PROTONAME_LEN + 2];

	int m_nProtocolCount[2];

	int m_nEnableProtocol;

	int m_nDisableProtocol;
	
	int m_bSupportSwap;
	int m_bSupportMotor;
    int m_bSupportVertical;//垂直跟踪

	int m_bSupportCustomSpeed;
	int m_iCustomOneStepSpeedV;
	int m_iCustomOneStepSpeedH;
	int m_iPTZCtrlStatue;
	int m_iNormalCtrlPreset;
	int m_iWatchCareEnable;     //新版本看守位使能
	int m_iWatchCareReTime;     //回归看守位时间(暂未用到，使用m_iStayTimeSec)
	int m_iWatchCareType;       //看守位类型(0：老版本-使用m_iNormalCtrlPreset 1：新版本-使用m_iWatchCareEnable)
	int m_iCalibrationStartFlag;
	int m_iStayTimeSec;  //人形检测停留的时间(回归看守位时间)

	int m_inited;//是否初始化完成
	
	int m_bManualMovePtzFlag;
	long m_nManualMoveTimeBegin;

	//!\brief 协议数组
	//##ModelId=49B47620036B
	ProtocolLib *m_aProtocolLib[MAX_PTZ_CHANNEL_NUM];

	//!\brief 解码器数组
	//##ModelId=49B47624034B
	CPtzDev *m_aCPtzDev[MAX_PTZ_CHANNEL_NUM];

	CPtzEletronic *m_aCPtzEletronic[MAX_PTZ_CHANNEL_NUM];

#ifdef EGUI_FOR_ICAMERA_NXP8850_BASED_DVR
	//编码器句柄
	void *m_pEncHandle[MAX_PTZ_CHANNEL_NUM];
#endif

	char m_szPath[2][PROTOCOL_PATH_MAX];

	sem_t m_semCruise;

	int m_nChannelCount;

	CPtzAdjuster *m_pAdjuster;

	static unsigned short m_u16PtzStatus[MAX_PTZ_CHANNEL_NUM][2];

	static pthread_mutex_t m_mtxSoftwareCruise[MAX_PTZ_CHANNEL_NUM];

	static pthread_cond_t m_condSoftwareCruise[MAX_PTZ_CHANNEL_NUM];

	static int m_nRunningSoftwareCruise[MAX_PTZ_CHANNEL_NUM];

	static pthread_t m_tSoftwareCruise[MAX_PTZ_CHANNEL_NUM];

	//休眠状态
    int m_bDorEnable;

    /* 巡航开关 */
    pthread_mutex_t m_mtxCruise;
    PTZ_CRUISE_INFO m_CruiseCfg;
};

#endif							/* __CPTZMANAGER_H_ */
