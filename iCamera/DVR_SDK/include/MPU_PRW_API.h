
/*!
 *\file		MPU_PRW_API.h
 *\brief	MPU模块，视频预览相关接口定义
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

#ifndef _INC_MPU_PRW_API_H_
#define _INC_MPU_PRW_API_H_

#if defined(EGUI_FOR_ICAMERA_NXP8850_BASED_DVR)
#else
#include "PRW/tagPreviewList.h"
#include "PRW/enumScreenType.h"
#endif

#include "RECT.h"
#include "CBVideoPreview.h"
#include "tagPreviewState.h"

/**************************************************************************
 *                                辅显                                    *
 **************************************************************************/

//!\brief   判读是否定制了辅显
//!\return  定制为1，无定制为0
bool MPU_PRW_IsCustomAuxMinorDisPlay();

//!\brief   定制辅显
//!\param   bool bCustom 定制为1，无定制为0
//!\return  成功为0，失败为-1
int MPU_PRW_CustomMinorDisPlay(bool bCustom);

//!\brief   判读是否启用了辅显
//!\return  启用为1，无启用为0
bool MPU_PRW_IsEnableMinorDisplay();

//!\brief   启用辅显
//!\param   bool bEnable 启用为1，无启用为0
//!\return  成功为0，失败为-1
int MPU_PRW_EnableMinorDisPlay(bool bEnable);


/**************************************************************************
 *                                屏保                                    *
 **************************************************************************/

//!\brief   获取屏保时间
//!\return  屏保时间，没有启用为0，失败为-1
int MPU_PRW_GetScreenSaverTime();

//!\brief   设置屏保时间
//!\param   int nScreenSaverTime    屏保时间。0为不启用屏保
//!\return  成功为0，失败为-1
int MPU_PRW_SetScreenSaverTime(int nScreenSaverTime);

//!\brief   开始屏保
//!\param   bool bStart             0为结束屏保，1为开始屏保
//!\return  成功为0，失败为-1
int MPU_PRW_ScreenSaving(bool bStart);


/**************************************************************************
 *                                菜单                                    *
 **************************************************************************/

//!\brief   设置EGUI的叠加深度
//!\param   int nAlpha          叠加值。上层根据实际分档次
//!\return  成功为0，失败为-1
int MPU_PRW_SetEguiMixAlpha(int nAlpha);

//!\brief   获取EGUI的叠加深度
//!\param   int *pnAlpha        叠加值
//!\return  成功为0，失败为-1
int MPU_PRW_GetEguiMixAlpha(int *pnAlpha);


/**************************************************************************
 *                                预览                                    *
 **************************************************************************/

#if defined(EGUI_FOR_ICAMERA_NXP8850_BASED_DVR)
#else
//!\brief   获取预览参数
//!\param   int bAuxMinor 获取主显为0，辅显为1
//!\param   LP_PREVIEW_LIST pInfo       自定义序列结构体
//!\return  成功为0，失败为-1
int MPU_PRW_GetPollList(int bAuxMinor, LP_PREVIEW_LIST pInfo);

//!\brief   设置预览参数
//!\param   int bAuxMinor 获取主显为0，辅显为1
//!\param   LP_PREVIEW_LIST pInfo       自定义序列结构体
//!\return  成功为0，失败为-1
int MPU_PRW_SetPollList(int bAuxMinor, LP_PREVIEW_LIST pInfo);
#endif

//!\brief   获取轮巡间隔时间
//!\param   int bAuxMinor   获取主显为0，辅显为1
//!\return  间隔的秒数
int MPU_PRW_GetPollInterval(int bAuxMinor);

//!\brief   设置轮巡间隔时间
//!\param   int bAuxMinor   获取主显为0，辅显为1
//!\param   nSecond     延迟的秒数
//!\return  0表示设置成功 其他表示设置失败
int MPU_PRW_SetPollInterval(int bAuxMinor, int nSecond);

#if defined(EGUI_FOR_ICAMERA_NXP8850_BASED_DVR)
#else
//!\brief   转为某种分屏模式。为某一分屏的默认分屏模式的第一个节点。该操作不会停止正在进行的轮询
//!\param   int bAuxMinor   获取主显为0，辅显为1
//!\param   enumScreenType eType 分屏模式
//!\param   nIndex  某一分屏的默认分屏模式的第nIndex个节点
//!\return  成功为0，失败为-1
int MPU_PRW_SwitchTo(int bAuxMinor, enumScreenType eType, int nIndex);
#endif

//!\brief   下一屏幕
//!\param   int bAuxMinor 获取主显为0，辅显为1
//!\param   int bFron 0为上一瓶，1为下一屏
//!\return  成功为0，失败为-1
int MPU_PRW_NextScreen(int bAuxMinor, int bFron);

//!\brief   屏幕转到某单一通道。该操作不会停止正在进行的轮询
//!\param   int bAuxMinor 获取主显为0，辅显为1
//!\param   int nCh 通道
//!\return  成功为0，失败为-1
int MPU_PRW_SwitchToSingle(int bAuxMinor, int nCh);

//!\brief   获取支持的分辨能力。支持的分辨能力字符串。例如：1024*768*60。对辅显来说，VGA固定为800*600
//!\param   int bAuxMinor               获取主显为0，辅显为1
//!\param   int *pnNum                  分辨率个数
//!\param   int (*pnResolvingPower)[3]  分辨率数组
//!\return  成功为0，失败为-1
int MPU_PRW_GetResolvingPower(int bAuxMinor, int *pnNum, int (*pnResolvingPower)[3]);

//!\brief 设置视频显示区域。底层会根据实际区域计算出每个通道实际占用矩形。对辅显为无效函数
//!\param   int bAuxMinor 获取主显为0，辅显为1
//!\param   RECT Rect 视频区域
//!\return  成功为0，失败为-1
int MPU_PRW_SetVideoScreenRect(int bAuxMinor, RECT Rect);

//!\brief   获取当前正在预览的分屏模式
//!\param   int bAuxMinor 获取主显为0，辅显为1
//!\param   enumScreenType *pType 分屏模式
//!\return  成功为0，失败为-1
//int MPU_PRW_GetEnumScreenType(int bAuxMinor, enumScreenType * pType);

//!\brief   根据屏幕索引获取通道的信息、矩形信息及显示状态
//!\param   int bAuxMinor 获取主显为0，辅显为1
//!\param   int nScreenIndex 当前分屏模式下屏幕的索引值
//!\param   int* pnCh 获取的通道
//!\param   RECT* pRect 获取的矩形
//!\return  成功为0，失败为-1
//int MPU_PRW_GetPreviewInfo(int bAuxMinor, int nScreenIndex, int *pnCh, RECT * pRect);

//!\brief   判断指定坐标点属于哪一个通道
//!\param   int bAuxMinor           获取主显为0，辅显为1
//!\param   int nX                  X坐标
//!\param   int nY                  Y坐标
//!\param   int *nCh                 通道号
//!\param   RECT *pRect             通道显示的矩形区域
//!\return  -1表示不属于任何通道 0表示获取成功
int MPU_PRW_GetChannelInfo(int bAuxMinor, int nX, int nY, int *nCh, RECT * pRect);

//!\brief   获取当前预览状态
//!\param   int bAuxMinor   获取主显为0，辅显为1
//!\param   PREVIEW_STATE *pState 预览状态 PREVIEW_STATIC 静态画面，PREVIEW_POLL 轮巡状态，PREVIEW_ALERT报警提示状态 
//!\return  0表示获取成功 其他表示错误码
int MPU_PRW_GetPreviewState(int bAuxMinor, PREVIEW_STATE * pState);

//!\brief   设置当前正在预览的分辨率
//!\param   int bAuxMinor           获取主显为0，辅显为1
//!\param   int nResolvingPower[3]  分辨率，宽，高，刷新率
//!\param   rcScreen                改完分辨率后的预览区域
//!\note    改变分辨率后，轮询自动停止，上层需要根据显示分辨率对分屏的限制，决定是否重新设置轮询序列。
//!\note    如果上层没有重新设置轮询序列，可能在分屏的效果上存在问题
//!\return  成功为0，失败为-1
int MPU_PRW_SetCurrentResolvingPower(int bAuxMinor, int nResolvingPower[3], RECT & rcScreen);

//!\brief   设置通道显示的顺序
//!\param   bAuxMinor       主显/辅显
//!\param   pnChannelOrder  通道顺序的数组的指针
//!\param   nLength         数组长度
//!\return  0表示获取成功 其他表示参数错误
int MPU_PRW_SetChannelOrder(int bAuxMinor, int *pnChannelOrder, int nLength);

//!\brief   获取通道显示的顺序
//!\param   bAuxMinor       主显/辅显
//!\param   pnChannelOrder  通道顺序的数组的指针
//!\param   nLength         数组长度
//!\return  0表示获取成功 其他表示参数错误
int MPU_PRW_GetChannelOrder(int bAuxMinor, int *pnChannelOrder, int nLength);

//!\brief   设置某个通道允许预览
//!\param   bAuxMinor   主显/辅显
//!\param   nChannel    通道号
//!\param   bEnable     是否允许该通道预览
//!\return  0表示成功 其他表示错误码
int MPU_PRW_EnableChannel(int bAuxMinor, int nChannel, int bEnable);

//!\brief   设置报警的通道。该函数需要重复设置，如果在延迟的时间内重复调用，则该通道自动恢复到无报警状态。
//!\param   int bAuxMinor   获取主显为0，辅显为1
//!\param   int nCh         通道
//!\return  成功为0，失败为-1
int MPU_PRW_SetAlarmCh(int bAuxMinor, int nCh);

//!\brief   进入自动轮询模式
//!\param   int bAuxMinor       获取主显为0，辅显为1
//!\param   bool bStart         true表示开始 false表示结束
//!\param   nType 0表示用户自定义序列，其他表示自动轮巡的分屏模式
//!\return  成功为0，失败为-1
int MPU_PRW_AutoPoll(int bAuxMinor, bool bStart, int nType);

//!\brief 暂停或者继续轮询。开始轮询后，必需先暂停
//!\param   int bAuxMinor   获取主显为0，辅显为1
//!\param   int bPause      暂停/继续标志
//!\return  成功为0，失败为-1
int MPU_PRW_PausePoll(int bAuxMinor, int bPause);

//!\brief   设置显示设备类型,默认为VGA。对主显来说，此项目前为VGA
//!\param   int bAuxMinor       获取主显为0，辅显为1
//!\param   int nDev            0为VGA，1为TV
//!\return  成功为0，失败为-1
int MPU_PRW_SetDisplayDev(int bAuxMinor, int nDev);

//!\brief   获取显示设备类型
//!\param   int bAuxMinor       获取主显为0，辅显为1
//!\param   int *nDev            0为VGA，1为TV
//!\return  成功为0，失败为-1
int MPU_PRW_GetDisplayDev(int bAuxMinor, int *nDev);

//!\brief   打开预览。
//!\param   int bAuxMinor       获取主显为0，辅显为1
//!\return  成功为0，失败为-1
int MPU_PRW_StartPreview(int bAuxMinor);

//!\brief   停止预览
//!\param   int bAuxMinor       获取主显为0，辅显为1
//!\return  成功为0，失败为-1
int MPU_PRW_StopPreview(int bAuxMinor);

//!\brief   设置视频预览的回调
//!\param   VP_Callback pCallback       预览回调
//!\return 0表示设置成功 其他表示错误码
int MPU_PRW_SetVPCallBack(VP_Callback pCallback);

//!\brief   获取联动屏幕提示的延迟时间
//!\param   bAuxMinor           0表示主显 1表示辅显
//!\return  0表示永久延迟 其他表示延迟的秒数
int MPU_PRW_GetAlertLasttime(int bAuxMinor);

//!\brief   设置联动屏幕提示的时间
//!\param   bAuxMinor 0表示主显 1表示辅显
//!\param   nSecond 0表示永久延迟 其他表示延迟的秒数
//!\return  0表示设置成功 其他表示失败
int MPU_PRW_SetAlertLasttime(int bAuxMinor, int nSecond);

//!\brief   获取当前的屏幕布局,及各矩形对应的通道号
//!\param   bAuxMinor       0为主显 1为辅显
//!\param   pRectList       存储矩形的缓冲区
//!\param   pChannelList    存储通道号的缓冲区
//!\param   nLength         缓冲区长度
//!\return  -1表示失败 其他表示屏幕矩形数
//int MPU_PRW_GetCurrentScreen(int bAuxMinor, RECT * pRectList, int *pnChannelList, int nLength);

//!\brief   清除预览通道
//!\param   bAuxMinor       0为主显 1为辅显
//!\return  0表示成功 -1表示失败
int MPU_PRW_ClearAlarm(int bAuxMinor);

//!\brief   刷新通道
//!\param   bAuxMinor       0为主显 1为辅显
//!\return  0表示成功 -1表示失败
int MPU_PRW_RefreshChannel(int bAuxMinor, int nChannel);

int MPU_PRW_LockScreen(int bAuxMinor, int bLock);

int MPU_PRW_Revert(int bAuxMinor);

//!\brief       启动音频预览
//!\param   nChannel        通道号
//!\return  0表示成功 -1表示失败
int MPU_PRW_StartAudioPreview(int nChannel);

//!\brief       停止音频预览
//!\return  0表示成功 -1表示失败
int MPU_PRW_StopAudioPreview();

//!\brief 交换两个通道的显示顺序
//!\param bAuxMinor 目前只有主显支持，应为0
//!\param nCh1 第一个选中的通道
//!\param nCh2 第二个选中的通道
//!\return 0表示交换成功，其他表示错误码
int MPU_PRW_SwapChannel(int bAuxMinor, int nCh1, int nCh2);

//!\brief 是否显示与该显示器相关联的OSD
//!\param bAuxMinor  显示器编号 0表示主显 1~4表示SPOT 5表示辅显
//!\param bShow 1表示显示OSD 0表示不显示
//!\return 0
int MPU_PRW_ShowOSD(int bAuxMinor, int bShow);

//!\brief 设置SPOT模式 
//!\param nMode 0表示一般模式 1表示复制模式
//!\return 0 总是成功
int MPU_PRW_SetSpotMode(int nMode);

//!\brief 获取SPOT模式
//!\return 0表示一般模式 1表示复制模式
int MPU_PRW_GetSpotMode();

#endif							/* _INC_MPU_PRW_API_H_ */
