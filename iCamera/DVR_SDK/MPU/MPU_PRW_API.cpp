
/*!
 *\file		MPU_PRW_API.cpp
 *\brief	MPU模块，视频预览相关接口实现
 *\author	林云
 *\date		2009/03/20

功能：
-#	提供预览参数的配置
-#	提供任意分屏模式的切换
-#	支持自定义轮询序列
-#	提供提供分辨能力的设置与获取
-#	提供预览改变时通知回调
-#	提供任意轮询模式的切换
-#	轮询时支持暂停/与继续功能
-#	支持在分辨率能力下，任意的显示位置的设置
-#	支持分屏模式的获取与当前分屏模式下对应的通道、位置及是否显示的信息。
-#	支持报警联动在主通道或辅助通道的预览及自动恢复
-#	屏幕保护。屏幕保护的启动参数获取与配置，上层根据这个配置来决定合适启用屏幕保护

注意：
-#	关于UI层需要对对应通道的刷新有UI层自行控制
 */


#include "MPU_global.h"

/**************************************************************************
 *                            局部宏定义                                  *
 **************************************************************************/

#ifndef DEBUG_NOT_USE_PRW_LIB	//调试时，不使用PRW库

#include "PRW/CPreviewManager.h"

//调用预览管理器执行动作
#define MPU_PRW_CALL(x)															\
{																				\
	int nRet;																	\
																				\
	CPreviewManager *pMgr = CPreviewManager::GetInstance();						\
																				\
	if (NULL != pMgr)															\
	{																			\
		nRet = pMgr->x;															\
																				\
		if (IDVR_OK != nRet)													\
		{																		\
			MPU_ERR("%s, return %d\n", #x, nRet);								\
		}																		\
	}																			\
	else																		\
	{																			\
		MPU_ERR("Get CPreviewManager instance fail!\n");						\
		MPU_CHK(NULL != pMgr);													\
		nRet = IDVR_MPU_PRW_NO_PREVIEW_MANAGER_INSTANCE;						\
	}																			\
																				\
	return nRet;																\
}

//调用预览管理器获取参数
#define MPU_PRW_GET(DataType, DefaultValue, Function)							\
{																				\
	DataType retValue;															\
																				\
	CPreviewManager *pMgr = CPreviewManager::GetInstance();						\
																				\
	if (NULL != pMgr)															\
	{																			\
		retValue = pMgr->Function;												\
	}																			\
	else																		\
	{																			\
		MPU_ERR("Get CPreviewManager instance fail!\n");						\
		MPU_CHK(NULL != pMgr);													\
		retValue = DefaultValue;												\
	}																			\
																				\
	return retValue;															\
}

#else

//调用预览管理器执行动作
#define MPU_PRW_CALL(x)															\
{																				\
	return IDVR_OK;																\
}

//调用预览管理器获取参数
#define MPU_PRW_GET(DataType, DefaultValue, Function)							\
{																				\
	return DefaultValue;														\
}

#endif							/* #ifndef DEBUG_NOT_USE_PRW_LIB */


/**************************************************************************
 *                            模块内部接口	                              *
 **************************************************************************/

//!\brief 初始化
//!\return  成功为0，失败为-1
int PRW_Init()
{
	int nRet = IDVR_OK;
	int ch=0;
	int total_channel=ZWSys_GetTotalEncChannels();

	//设置制式
	CConfig *pCfg = CConfig::GetInstance();

	int iStandard = pCfg->GetValue("RecordCfg", "VideoStandard", StandardPAL);

//	MP4Sys_SetSystemStandard((VideoOutStandard_t) iStandard);
	for (ch=0;ch<total_channel;ch++)
	{
	ZWSys_SetCVBSOutStandard(ZWSys_EncChannelOpen(ch),(VideoOutStandard_t)iStandard);
	}
#ifndef DEBUG_NOT_USE_PRW_LIB	//调试时，不使用PRW库
	//获取预览管理器实例指针
	CPreviewManager *pMgr = CPreviewManager::GetInstance();

	if (NULL == pMgr)
	{
		MPU_ERR("Get CPreviewManager instance fail!\n");
		MPU_CHK(NULL != pMgr);
		return IDVR_MPU_PRW_NO_PREVIEW_MANAGER_INSTANCE;
	}

	//调用初始化接口
	nRet = pMgr->Init();
	if (IDVR_OK != nRet)
	{
		MPU_ERR("call Init() fail, return %d\n", nRet);
		return nRet;
	}

	//获取default用户
	HANDLE hUser = USR_GetDefault();

	//系统必须有缺省用户
	if (INVALID_HANDLE == hUser)
	{
		MPU_ERR("get default user handle fail\n");
		//MPU_CHK(INVALID_HANDLE != hUser);
		return nRet;
	}

	//获取通道总数
	for (int nChannel = 0; nChannel < MAX_ENC_CHANNEL_NUM; ++nChannel)
	{
		//获取缺省用户的本地预览权限
		int bEnable = MPU_USR_GetChannelPrivilege(hUser, CHANNEL_PREVIEW, nChannel, FALSE);

		//设置通道预览
		pMgr->EnableChannel(0, nChannel, bEnable);	//主显
		pMgr->EnableChannel(1, nChannel, bEnable);	//辅显
	}
#endif
	return nRet;
}

//!\brief 释放资源
//!\return  成功为0，失败为-1
int PRW_Uninit()
{
	//调用预览管理器接口执行
	MPU_PRW_CALL(UnInit());
}


/**************************************************************************
 *                                辅显                                    *
 **************************************************************************/

//!\brief   判读是否定制了辅显
//!\return  定制为1，无定制为0
bool MPU_PRW_IsCustomAuxMinorDisPlay()
{
	//调用预览管理器获取参数
	MPU_PRW_GET(bool, false, IsCustomAuxMinorDisPlay());
}

//!\brief   定制辅显
//!\param   bool bCustom    定制为1，无定制为0
//!\return  成功为0，失败为-1
int MPU_PRW_CustomMinorDisPlay(bool bCustom)
{
	//调用预览管理器执行动作
	MPU_PRW_CALL(CustomMinorDisPlay(bCustom));
}

//!\brief   判读是否启用了辅显
//!\return  启用为1，无启用为0
bool MPU_PRW_IsEnableMinorDisplay()
{
	//调用预览管理器获取参数
	MPU_PRW_GET(bool, false, IsEnableMinorDisplay());
}

//!\brief   启用辅显
//!\param   bool bEnable 启用为1，无启用为0
//!\return  成功为0，失败为-1
int MPU_PRW_EnableMinorDisPlay(bool bEnable)
{
	//调用预览管理器执行动作
	MPU_PRW_CALL(EnableMinorDisPlay(bEnable));
}


/**************************************************************************
 *                                屏保                                    *
 **************************************************************************/

//!\brief   获取屏保时间
//!\return  屏保时间，没有启用为0，失败为-1
int MPU_PRW_GetScreenSaverTime()
{
	//调用预览管理器获取参数
	MPU_PRW_GET(int, 0, GetScreenSaverTime());
}

//!\brief   设置屏保时间
//!\param   int nScreenSaverTime    屏保时间。0为不启用屏保
//!\return  成功为0，失败为-1
int MPU_PRW_SetScreenSaverTime(int nScreenSaverTime)
{
	//调用预览管理器执行动作
	MPU_PRW_CALL(SetScreenSaverTime(nScreenSaverTime));
}

//!\brief   开始屏保
//!\param   bool bStart             0为结束屏保，1为开始屏保
//!\return  成功为0，失败为-1
int MPU_PRW_ScreenSaving(bool bStart)
{
	//调用预览管理器执行动作
	MPU_PRW_CALL(ScreenSaving(bStart));
}


/**************************************************************************
 *                                菜单                                    *
 **************************************************************************/

//!\brief   设置EGUI的叠加深度
//!\param   int nAlpha          叠加值。上层根据实际分档次
//!\return  成功为0，失败为-1
int MPU_PRW_SetEguiMixAlpha(int nAlpha)
{
	//调用预览管理器执行动作
	MPU_PRW_CALL(SetEguiMixAlpha(nAlpha));
}

//!\brief   获取EGUI的叠加深度
//!\param   int *pnAlpha        叠加值
//!\return  成功为0，失败为-1
int MPU_PRW_GetEguiMixAlpha(int *pnAlpha)
{
	//调用预览管理器执行动作
	MPU_PRW_CALL(GetEguiMixAlpha(pnAlpha));
}


/**************************************************************************
 *                                预览                                    *
 **************************************************************************/

#ifndef DEBUG_NOT_USE_PRW_LIB
//!\brief   获取预览轮巡参数
//!\param   int bAuxMinor 获取主显为0，辅显为1
//!\param   LP_PREVIEW_LIST pInfo    自定义序列结构体
//!\return  成功为0，失败为-1
int MPU_PRW_GetPollList(int bAuxMinor, LP_PREVIEW_LIST pInfo)
{
	//调用预览管理器执行动作
	MPU_PRW_CALL(GetPollList(bAuxMinor, pInfo));
}

//!\brief   设置预览轮巡参数
//!\param   int bAuxMinor 获取主显为0，辅显为1
//!\param   PREVIEW_LIST *pInfo     自定义序列结构体
//!\return  成功为0，失败为-1
int MPU_PRW_SetPollList(int bAuxMinor, LP_PREVIEW_LIST pInfo)
{
	//调用预览管理器执行动作
	MPU_PRW_CALL(SetPollList(bAuxMinor, pInfo));
}
#endif

//!\brief   获取轮巡间隔时间
//!\param   int bAuxMinor   获取主显为0，辅显为1
//!\return  间隔的秒数
int MPU_PRW_GetPollInterval(int bAuxMinor)
{
	//调用预览管理器获取参数
	MPU_PRW_GET(int, 0, GetPollInterval(bAuxMinor));
}

//!\brief   设置轮巡间隔时间
//!\param   int bAuxMinor   获取主显为0，辅显为1
//!\param   nSecond     延迟的秒数
//!\return  0表示设置成功 其他表示设置失败
int MPU_PRW_SetPollInterval(int bAuxMinor, int nSecond)
{
	//调用预览管理器执行动作
	MPU_PRW_CALL(SetPollInterval(bAuxMinor, nSecond));
}

#ifndef DEBUG_NOT_USE_PRW_LIB
//!\brief   转为某种分屏模式。为某一分屏的默认分屏模式的第一个节点。该操作不会停止正在进行的轮询
//!\param   int bAuxMinor   获取主显为0，辅显为1
//!\param   enumScreenType eType 分屏模式
//!\param   nIndex  某一分屏的默认分屏模式的第nIndex个节点
//!\return  成功为0，失败为-1
int MPU_PRW_SwitchTo(int bAuxMinor, enumScreenType eType, int nIndex)
{
	//调用预览管理器执行动作
	MPU_PRW_CALL(SwitchTo(bAuxMinor, eType, nIndex));
}
#endif

//!\brief   下一屏幕
//!\param   int bAuxMinor 获取主显为0，辅显为1
//!\param   int bFron 0为上一瓶，1为下一屏
//!\return  成功为0，失败为-1
int MPU_PRW_NextScreen(int bAuxMinor, int bFron)
{
	//调用预览管理器执行动作
	MPU_PRW_CALL(NextScreen(bAuxMinor, bFron));
}

//!\brief   屏幕转到某单一通道。该操作不会停止正在进行的轮询
//!\param   int bAuxMinor 获取主显为0，辅显为1
//!\param   int nCh 通道
//!\return  成功为0，失败为-1
int MPU_PRW_SwitchToSingle(int bAuxMinor, int nCh)
{
	//调用预览管理器执行动作
	MPU_PRW_CALL(SwitchToSingle(bAuxMinor, nCh));
}

//!\brief   获取支持的分辨能力。支持的分辨能力字符串。例如：1024*768*60。对辅显来说，VGA固定为800*600
//!\param   int bAuxMinor               获取主显为0，辅显为1
//!\param   int *pnNum                  分辨率个数
//!\param   int (*pnResolvingPower)[3]  分辨率数组
//!\return  成功为0，失败为-1
int MPU_PRW_GetResolvingPower(int bAuxMinor, int *pnNum, int (*pnResolvingPower)[3])
{
	//调用预览管理器执行动作
	MPU_PRW_CALL(GetResolvingPower(bAuxMinor, pnNum, pnResolvingPower));
}

//!\brief 设置视频显示区域。底层会根据实际区域计算出每个通道实际占用矩形。对辅显为无效函数
//!\param   int bAuxMinor 获取主显为0，辅显为1
//!\param   RECT Rect 视频区域
//!\return  成功为0，失败为-1
int MPU_PRW_SetVideoScreenRect(int bAuxMinor, RECT Rect)
{
	//调用预览管理器执行动作
	MPU_PRW_CALL(SetVideoScreenRect(bAuxMinor, Rect));
}

//!\brief   获取当前正在预览的分屏模式
//!\param   int bAuxMinor 获取主显为0，辅显为1
//!\param   enumScreenType *pType   分屏模式
//!\return  成功为0，失败为-1
//int MPU_PRW_GetEnumScreenType(int bAuxMinor, enumScreenType * pType)
//{
//  //调用预览管理器执行动作
//  MPU_PRW_CALL(GetEnumScreenType(bAuxMinor, pType));
//}

//!\brief   根据屏幕索引获取通道的信息、矩形信息及显示状态
//!\param   int bAuxMinor 获取主显为0，辅显为1
//!\param   int nScreenIndex 当前分屏模式下屏幕的索引值
//!\param   int* pnCh 获取的通道
//!\param   RECT* pRect 获取的矩形
//!\return  成功为0，失败为-1
//int MPU_PRW_GetPreviewInfo(int bAuxMinor, int nScreenIndex, int *pnCh, RECT * pRect)
//{
//  //调用预览管理器执行动作
//  MPU_PRW_CALL(GetPreviewInfo(bAuxMinor, nScreenIndex, pnCh, pRect));
//}

//!\brief   判断指定坐标点属于哪一个通道
//!\param   int bAuxMinor           获取主显为0，辅显为1
//!\param   int nX                  X坐标
//!\param   int nY                  Y坐标
//!\param   int *nCh                 通道号
//!\param   RECT *pRect             通道显示的矩形区域
//!\return  -1表示不属于任何通道, 0表示获取成功
int MPU_PRW_GetChannelInfo(int bAuxMinor, int nX, int nY, int *nCh, RECT * pRect)
{
	//调用预览管理器获取参数
	MPU_PRW_GET(int, -1, GetChannelInfo(bAuxMinor, nX, nY, nCh, pRect));
}

//!\brief   获取当前预览状态
//!\param   int bAuxMinor   获取主显为0，辅显为1
//!\param   PREVIEW_STATE *pState 预览状态 PREVIEW_STATIC 静态画面，PREVIEW_POLL 轮巡状态，PREVIEW_ALERT报警提示状态 
//!\return  0表示获取成功 其他表示错误码
int MPU_PRW_GetPreviewState(int bAuxMinor, PREVIEW_STATE * pState)
{
	//调用预览管理器获取参数
	MPU_PRW_GET(int, 0, GetPreviewState(bAuxMinor, pState));
}

//!\brief   设置当前正在预览的分辨率
//!\param   int bAuxMinor       获取主显为0，辅显为1
//!\param   int nResolvingPower[3]  分辨率，宽，高，刷新率
//!\param   rectScreen 改完分辨率后的预览区域
//!\note    改变分辨率后，轮询自动停止，上层需要根据显示分辨率对分屏的限制，决定是否重新设置轮询序列。
//!\note    如果上层没有重新设置轮询序列，可能在分屏的效果上存在问题
//!\return  成功为0，失败为-1
int MPU_PRW_SetCurrentResolvingPower(int bAuxMinor, int nResolvingPower[3], RECT & rcScreen)
{
	//调用预览管理器执行动作
	MPU_PRW_CALL(SetCurrentResolvingPower(bAuxMinor, nResolvingPower, rcScreen));
}

//!\brief   设置通道显示的顺序
//!\param   bAuxMinor       主显/辅显
//!\param   pnChannelOrder  通道顺序的数组的指针
//!\param   nLength         数组长度
//!\return  0表示获取成功 其他表示参数错误
int MPU_PRW_SetChannelOrder(int bAuxMinor, int *pnChannelOrder, int nLength)
{
	//调用预览管理器执行动作
	MPU_PRW_CALL(SetChannelOrder(bAuxMinor, pnChannelOrder, nLength));
}

//!\brief   获取通道显示的顺序
//!\param   bAuxMinor       主显/辅显
//!\param   pnChannelOrder  通道顺序的数组的指针
//!\param   nLength         数组长度
//!\return  0表示获取成功 其他表示参数错误
int MPU_PRW_GetChannelOrder(int bAuxMinor, int *pnChannelOrder, int nLength)
{
	//调用预览管理器执行动作
	MPU_PRW_CALL(GetChannelOrder(bAuxMinor, pnChannelOrder, nLength));
}

//!\brief   设置某个通道允许预览
//!\param   bAuxMinor   主显/辅显
//!\param   nChannel    通道号
//!\param   bEnable     是否允许该通道预览
//!\return  0表示成功 其他表示错误码
int MPU_PRW_EnableChannel(int bAuxMinor, int nChannel, int bEnable)
{
	//调用预览管理器执行动作
	MPU_PRW_CALL(EnableChannel(bAuxMinor, nChannel, bEnable));
}

//!\brief   设置报警的通道。该函数需要重复设置，如果在延迟的时间内重复调用，则该通道自动恢复到无报警状态。
//!\param   int bAuxMinor   获取主显为0，辅显为1
//!\param   int nCh         通道
//!\return  成功为0，失败为-1
int MPU_PRW_SetAlarmCh(int bAuxMinor, int nCh)
{
	//调用预览管理器执行动作
	MPU_PRW_CALL(SetAlarmCh(bAuxMinor, nCh));
}

//!\brief   进入自动轮询模式
//!\param   int bAuxMinor       获取主显为0，辅显为1
//!\param   bool bStart         true表示开始 false表示结束
//!\param   nType 0表示用户自定义序列，其他表示自动轮巡的分屏模式
//!\return  成功为0，失败为-1
int MPU_PRW_AutoPoll(int bAuxMinor, bool bStart, int nType)
{
	//调用预览管理器执行动作
	MPU_PRW_CALL(AutoPoll(bAuxMinor, bStart, nType));
}

//!\brief 暂停或者继续轮询。开始轮询后，必需先暂停
//!\param   int bAuxMinor   获取主显为0，辅显为1
//!\param   int bPause      暂停/继续标志
//!\return  成功为0，失败为-1
int MPU_PRW_PausePoll(int bAuxMinor, int bPause)
{
	//调用预览管理器执行动作
	MPU_PRW_CALL(PausePoll(bAuxMinor, bPause));
}

//!\brief   设置显示设备类型,默认为VGA。对主显来说，此项目前为VGA
//!\param   int bAuxMinor       获取主显为0，辅显为1
//!\param   int nDev            0为VGA，1为TV
//!\return  成功为0，失败为-1
int MPU_PRW_SetDisplayDev(int bAuxMinor, int nDev)
{
	//调用预览管理器执行动作
	MPU_PRW_CALL(SetDisplayDev(bAuxMinor, nDev));
}

//!\brief   设置显示设备类型
//!\param   int bAuxMinor       获取主显为0，辅显为1
//!\param   int *nDev            0为VGA，1为TV
//!\return  成功为0，失败为-1
int MPU_PRW_GetDisplayDev(int bAuxMinor, int *nDev)
{
	//调用预览管理器执行动作
	MPU_PRW_CALL(GetDisplayDev(bAuxMinor, nDev));
}


//!\brief   开启预览。
//!\param   bAuxMinor           0表示主显 1表示辅显
//!\return  成功为0，失败为-1
int MPU_PRW_StartPreview(int bAuxMinor)
{
	//调用预览管理器执行动作
	MPU_PRW_CALL(StartPreview(bAuxMinor));
}

//!\brief   停止预览
//!\param   bAuxMinor           0表示主显 1表示辅显
//!\return  成功为0，失败为-1
int MPU_PRW_StopPreview(int bAuxMinor)
{
	//调用预览管理器执行动作
	MPU_PRW_CALL(StopPreview(bAuxMinor));
}

//!\brief   设置视频预览的回调
//!\param   VP_Callback pCallback       预览回调
//!\return 0表示设置成功 其他表示错误码
int MPU_PRW_SetVPCallBack(VP_Callback pCallback)
{
	//调用预览管理器执行动作
	MPU_PRW_CALL(SetVPCallback(pCallback));
}

//!\brief   获取联动屏幕提示的延迟时间
//!\param   bAuxMinor           0表示主显 1表示辅显
//!\return  0表示永久延迟 其他表示延迟的秒数
int MPU_PRW_GetAlertLasttime(int bAuxMinor)
{
	//调用预览管理器执行动作
	MPU_PRW_GET(int, 0, GetAlertLastTime(bAuxMinor));
}

//!\brief   设置联动屏幕提示的时间
//!\param   bAuxMinor 0表示主显 1表示辅显
//!\param   nSecond 0表示永久延迟 其他表示延迟的秒数
//!\return  0表示设置成功 其他表示失败
int MPU_PRW_SetAlertLasttime(int bAuxMinor, int nSecond)
{
	//调用预览管理器执行动作
	MPU_PRW_CALL(SetAlertLastTime(bAuxMinor, nSecond));
}

//!\brief   获取当前的屏幕布局,及各矩形对应的通道号
//!\param   bAuxMinor       0为主显 1为辅显
//!\param   pRectList       存储矩形的缓冲区
//!\param   pChannelList    存储通道号的缓冲区
//!\param   nLength          表示缓冲区长度
//!\return  -1表示失败 其他表示屏幕矩形数 
//int MPU_PRW_GetCurrentScreen(int bAuxMinor, RECT * pRectList, int *pnChannelList, int nLength)
//{
//  //调用预览管理器执行动作
//  MPU_PRW_CALL(GetCurrentScreen(bAuxMinor, pRectList, pnChannelList, nLength));
//}

//!\brief   清除预览通道
//!\param   bAuxMinor       0为主显 1为辅显
//!\return  0表示成功 -1表示失败
int MPU_PRW_ClearAlarm(int bAuxMinor)
{
	//调用预览管理器执行动作
	MPU_PRW_CALL(ClearAlarm(bAuxMinor));
}

//!\brief   刷新通道
//!\param   bAuxMinor       0为主显 1为辅显
//!\return  0表示成功 -1表示失败
int MPU_PRW_RefreshChannel(int bAuxMinor, int nChannel)
{
	//调用预览管理器执行动作
	MPU_PRW_CALL(RefreshChannel(bAuxMinor, nChannel));
}

int MPU_PRW_LockScreen(int bAuxMinor, int bLock)
{
	MPU_PRW_CALL(LockScreen(bAuxMinor, bLock));
}

int MPU_PRW_Revert(int bAuxMinor)
{
	MPU_PRW_CALL(Revert(bAuxMinor));
}

//!\brief       启动音频预览
//!\param   nChannel        通道号
//!\return  0表示成功 -1表示失败
int MPU_PRW_StartAudioPreview(int nChannel)
{
	MPU_PRW_CALL(StartAudioPreview(nChannel));
}

//!\brief       停止音频预览
//!\return  0表示成功 -1表示失败
int MPU_PRW_StopAudioPreview()
{
	MPU_PRW_CALL(StopAudioPreview());
}

int MPU_PRW_SwapChannel(int bAuxMinor, int nCh1, int nCh2)
{
	MPU_PRW_CALL(SwapChannel(bAuxMinor, nCh1, nCh2));
}

int MPU_PRW_ShowOSD(int bAuxMinor, int bShow)
{
	MPU_PRW_CALL(ShowOSD(bAuxMinor, bShow));
}

int MPU_PRW_SetSpotMode(int nMode)
{
	MPU_PRW_CALL(SetSpotMode(nMode));
}

int MPU_PRW_GetSpotMode()
{
	MPU_PRW_GET(int, 0, GetSpotMode());
}

/**************************************************************************
 *                            注销局部宏定义                              *
 **************************************************************************/
#undef	MPU_PRW_CALL			//文件播放器句柄
#undef	MPU_PRW_GET				//流播放器句柄
