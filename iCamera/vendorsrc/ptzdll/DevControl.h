#ifndef __DEVCONTROL_H_
#define __DEVCONTROL_H_

#include <time.h>
#include "tagPtzFlag.h"
#include "tagPtzCode.h"

#ifndef __cplusplus
#define DLL_API
#else
#define DLL_API extern "C"
#endif

//定义返回值
#define	DLL_ERROR		(-1)	//参数或者命令错误
#define	DLL_INPUT_OK	(0)		//参数或者命令正确，但是执行错误
#define	DLL_OUTPUT_OK	(1)		//返回正确

//速度级别数
#define SPEED_LEVELS (10)

#define SPEED(dwSpeed, fast, slow) ((0 == dwSpeed) ? 0 : ((SPEED_LEVELS == dwSpeed) ? fast : (slow + (int)((int)dwSpeed*((int)fast-(int)slow) / (int)SPEED_LEVELS))))

#define ASSERT_SENDFN \
	if (NULL == IFSendData)\
{\
	return DLL_ERROR;\
}

#define ASSERT_RECVFN \
if (NULL == IFRecvData)\
{\
	return DLL_ERROR;\
}

#define ASSERT_SOFTCRUISEFN \
if ((NULL == IFStartCruise) || (NULL == IFStopCruise))\
{\
	return DLL_ERROR;\
}

//!\brief 一般云台控制
//!\param dwPtzCode 云台控制码 见TTY_PTZ_CODE
//!\param dwSpeed 运动速度
//!\return 0表示成功，其他表示错误码
//##ModelId=49A36CC30148
DLL_API int PtzNormalCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, unsigned long dwSpeed);

//!\brief 自动控制
//!\param dwPtzCode 自动控制码
//!\param dwPresetIndex1 预置点号
//!\param dwPresetIndex2 预置点号
//!\param dwSpeed 运动速度
//!\param  bScan 开始/结束
//!\return 0表示成功 -1表示失败
//##ModelId=49A36CCF000F
DLL_API int PtzAutoCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, unsigned long dwPresetIndex1,
						unsigned long dwPresetIndex2, unsigned long dwCruiseIndex, unsigned long dwSpeed, unsigned long dwDwellTime, int bScan);

//!\brief 预置点控制
//!\param dwPtzCode 预置点控制码
//!\param dwPresetIndex 预置点号
//!\param dwCruiseIndex 巡航点号
//!\param dwSpeed 速度
//!\param dwDwellTime停滞时间
//!\return 0表示成功 其他值表示错误码
//##ModelId=49A36CDD00FA
DLL_API int PtzPresetCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, unsigned long dwPresetIndex,
						  unsigned long dwCruiseIndex, unsigned long dwSpeed, unsigned long dwDwellTime);

//!\brief 辅助设备控制
//!\param dwPtzCode 云台辅助控制码,参见TTY_AUX_CODE
//!\param bOn 1=打开,0=关闭
//!\return 0表示成功 -1表示失败
//##ModelId=49A36CE8035B
DLL_API int PtzAuxCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode, int bOn);

//!\brief 菜单控制
//!\param dwPtzCode 菜单控制码
//!\return 0表示执行成功，-1表示执行失败
//!\sa TTY_MENU_CODE
//##ModelId=49A49DAE0138
DLL_API int PtzMenuCtrl(unsigned int nChannel, unsigned long dwAddress, unsigned long dwPtzCode);

DLL_API int GetEletronicValue(unsigned int nChannel, unsigned long dwAddress, int nValueIndex, int *Value1, int *Value2);

DLL_API int SetEletronicValue(unsigned int nChannel, unsigned long dwAddress, int nValueIndex, int Value1, int Value2);

DLL_API int SetCallback(CbSendData FnSendData, CbRecvData FnRecvData, CbStartCruise FnStartCruise, CbStopCruise FnStopCruise);

//!\brief 检查协议是否有相关功能
//!\param nPtzFlag PTZ_FLAG 相关功能标志
//!\return 1表示有该功能 0表示没有该功能
DLL_API int HasFunction(int nPtzFlag);

//!\brief 获取云台技术参数
//!\param Presetcount 预置点数
//!\param CruiseCount 巡航数
//!\param TrackCount 轨迹数
//!\param Baudrate 支持的波特率
//!\return 无
DLL_API void GetTechnicalData(int *PresetCount, int *CruiseCount, int *TrackCount, int *AutoCount, unsigned long *Baudrate);

//!\brief 获取协议的版本号
//! 协议版本号组成 协议名 主版本号.次版本号.SVN号 BUILD 编译日期 编译时间
//! 例 PelcoD 0.1.2913 BUILD Jul 15 2009 00:31:23
//!\param 无
//!\return 协议版本号常量字符串
DLL_API const char *GetVersion(void);

//!\brief 延时的毫秒数
static inline void PtzDelay(int usec)
{
	struct timespec ts;

	ts.tv_sec = 0;
	ts.tv_nsec = usec * 1000 * 1000;
	nanosleep(&ts, NULL);
}

#endif							/* __DEVCONTROL_H_ */
