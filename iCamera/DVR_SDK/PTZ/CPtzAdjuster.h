
/*!
*\file		CPtzAdjuster.h
*\brief		PTZ模块，云台守望者接口定义
*\author	喻坤
*\date		2009/11/20
*/

#ifndef __CPTZADJUSTER_H_
#define __CPTZADJUSTER_H_
#include "global_macro.h"

class CPtzManager;

class CPtzDev;

class CPtzAdjuster
{
  public:
	CPtzAdjuster();

	~CPtzAdjuster();

	//!\brief 获取复位器实例
	static CPtzAdjuster *GetInstance();

	//!\brief 初始化复位器
	//!\param PTZManager
	//!\param 传入通道数
	//!\return 0表示失败,其他表示错误码
	int Init(CPtzManager * pManager, int nChNum);

	//!\brief 设置是否启用云台功能
	//!\param bEnable 0表示不启用，其他表示启用
	//!\param nChannel 通道号
	//!\return 0表示设置成功，其他表示错误码
	int EnablePTZ(int bEnable, int nChannel);

	//!\brief 获取云台是否启用功能
	//!\param nChannel 通道号
	//!\return 0表示不启用,其他表示启用
	int IsEnablePTZ(int nChannel);

	//!\brief 设置是否启用云台守望功能
	//!\param bEnable 0表示不启用，其他表示启用
	//!\param nChannel 通道号
	//!\return 0表示设置成功，其他表示错误码
	int EnableKeeper(int bEnable, int nChannel);

	//!\brief 获取云台是否启用守望功能
	//!\param nChannel 通道号
	//!\return 0表示不启用,其他表示启用
	int IsEnableKeeper(int nChannel);

	//!\brief 获取云台复位周期
	//!\param nChannel 云台通道号
	//!\return 复位周期，以秒为单位
	time_t GetResetCycle(int nChannel);

	//!\brief 设置云台复位周期
	//!\param nSecond 设置云台复位需要空闲的秒数
	//!\param nChannel 云台通道号
	//!\return 0表示设置成功 其他表示错误码
	int SetResetCycle(time_t nSecond, int nChannel);

	//!\brief 获取云台默认动作的接口
	//!\param pdwActionType 默认动作类型
	//!\param pdwIndex 动作相关参数，（预置点号，巡航线路号，轨迹号，扫描线路号）
	//!\param nChannel 云台通道号
	int GetDefaultAction(unsigned long *pdwActionType, unsigned long *pdwIndex, int nChannel);

	//!\brief 设置云台默认动作的接口
	//!\param dwActionType 默认动作类型
	//!\param dwIndex 动作相关参数，（预置点号，巡航线路号，轨迹号，扫描线路号）
	//!\param nChannel 云台通道号
	//!\return 0表示设置正确，其他表示错误码
	int SetDefaultAction(unsigned long dwActionType, unsigned long dwIndex, int nChannel);

	//!\brief 通知云台进入报警联动状态
	//!\param dwActionType 联动动作类型
	//!\param dwIndex 动作相关参数，（预置点号，巡航线路号，轨迹号，扫描线路号）
	//!\param nChannel 云台通道号
	//!\return 0
	int Alarm(unsigned long dwActionType, unsigned long dwIndex, int nChannel);

	//!\brief 通知云台退出报警联动状态
	//!\param nChannel 云台通道号
	//!\return 0
	int ClearAlarm(int nChannel);

	//!\brief 获取云台设备的默认动作和报警动作
	//!\param dwDefAction 联动动作类型
	//!\param dwDefIndex 联动动作关联序号
	//!\param nChannel 云台通道号
	//!\return 无
	void GetAlarmAction(unsigned long *dwActionType, unsigned long *dwIndex, int nChannel);

	//!\brief 重置计时器
	//!\param nChannel 指定通道号
	//!\return 无
	void ResetTimer(int nChannel);

#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
	int SetAdvanceCfg(int nChannel, char nCruiseEnable, 
									int nBeginPresetNumForCruise,
									int nBeginPresetActionForCruise,
									int nEndPresetNumForCruise,
									int nEndPresetActionForCruise,
									char nWatchEnable,
									int nBeginPresetNumForWatch,
									int nBeginPresetActionForWatch,
									int nEndPresetNumForWatch,
									int nEndPresetActionForWatch);

	int SetAdvanceCfgV1(int nChannel, char nCruiseEnable, 
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

	int GetAdvanceCfg(int nChannel, char *nCruiseEnable, 
									int *nBeginPresetNumForCruise,
									int *nBeginPresetActionForCruise,
									int *nEndPresetNumForCruise,
									int *nEndPresetActionForCruise,
									char *nWatchEnable,
									int *nBeginPresetNumForWatch,
									int *nBeginPresetActionForWatch,
									int *nEndPresetNumForWatch,
									int *nEndPresetActionForWatch);

	int GetAdvanceCfgV1(int nChannel, char *nCruiseEnable, 
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
	
	int EnablePTZCruise(int enable);

	int EnablePTZWatch(int enable);

	int EnablePTZTrack(int enable);

	int SetTrackSpeed(int speedH, int speedV, int nChannel);
	int ReturnToOriginalSpeed(int nChannel);
#endif
    int SetSpeed(int nSpeed, int nChannel);

	//!\brief   设置ptzV速度
	//!\param   nChannel  通道号     nSpeedV  速度
	//!\return  -1为失败	
	int SetSpeedV(int nSpeedV, int nChannel);

	//!\brief   设置ptzH速度
	//!\param   nChannel  通道号     nSpeedH  速度
	//!\return  -1为失败
	int SetSpeedH(int nSpeedV, int nChannel);
	
    int GetSpeed(int nChannel);

  private:
	//!\brief 根据配置和状态使云台复位
	//!\param nChannel 通道号
	//!\return 0表示复位成功后, 其他表示错误码
	int Reset(int nChannel);

	//!\brief 检查指定的通道是否超时
	//!\param nTime 当前时间
	//!\param nChannel 通道号
	//!\return 0表示未超时 1表示已超时
	int IsTimeOut(int nTime, int nChannel);

	//!\brief 定时器检查线程
	static void *CheckTimer(void *arg);

  private:
	  unsigned char m_bEnable[MAX_PTZ_CHANNEL_NUM];
	  unsigned char m_bEnablePTZ[MAX_PTZ_CHANNEL_NUM];

	//!\brief 存储各个通道的空闲时间计数
	time_t m_lIdleTime[MAX_PTZ_CHANNEL_NUM];

	//!\brief 各通道复位周期
	time_t m_lResetCycle[MAX_PTZ_CHANNEL_NUM];

	//!\brief 各通道默认动作
	unsigned int m_uDefaultAction[MAX_PTZ_CHANNEL_NUM];	//dwIndex << 24 & dwActionType

	//!\brief 各通道联动动作
	unsigned int m_uAlarmAction[MAX_PTZ_CHANNEL_NUM];	//dwIndex << 24 & dwActionType

	//!\brief 关联的PTZManager
	CPtzManager *m_pManager;

	//!\brief 监视的云台设备数组
	CPtzDev **m_pDevs;

	//!\brief 唯一实例
	static CPtzAdjuster *m_pInstance;

	//!\brief 通道数
	int m_nChannelCount;
 
	//PTZ Advance cfg（用预置点代替软巡航，通过调特殊预置点来达到球机自己巡航或守望的目的）
	char m_nCruiseEnable;//巡航开始或结束配置使能
	
	int m_nBeginPresetNumForCruise;//巡航开始时调用的预置点
	
	int m_nBeginPresetActionForCruise;//巡航开始时调用的预置点动作
	
	int m_nEndPresetNumForCruise;//巡航结束时调用的预置点

	int m_nEndPresetActionForCruise;//巡航结束时调用的预置点动作
	
	char m_nWatchEnable;//守望开始或结束配置使能
	
	int m_nBeginPresetNumForWatch;//守望开始时调用的预置点

	int m_nBeginPresetActionForWatch;//守望开始时调用的预置点动作
	
	int m_nEndPresetNumForWatch;//守望结束时调用的预置点

	int m_nEndPresetActionForWatch;//守望结束时调用的预置点动作

	char m_nTrackEnable;//跟踪开始或结束配置使能
	
	int m_nBeginPresetNumForTrack;//跟踪开始时调用的预置点

	int m_nBeginPresetActionForTrack;//跟踪开始时调用的预置点动作
	
	int m_nEndPresetNumForTrack;//跟踪结束时调用的预置点

	int m_nEndPresetActionForTrack;//跟踪结束时调用的预置点动作
	   
	int m_nSpeed[MAX_PTZ_CHANNEL_NUM];//云台速度

	int m_nSpeedV[MAX_PTZ_CHANNEL_NUM]; //垂直速度

	int m_nSpeedH[MAX_PTZ_CHANNEL_NUM]; //水平速度
	
	int m_bSupportMotor;//是否支持电机
};

#endif
