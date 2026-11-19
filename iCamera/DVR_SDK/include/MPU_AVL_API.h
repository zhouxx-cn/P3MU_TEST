
/*!
 *\file		MPU_AVL_API.h
 *\brief	MPU模块，通道管理相关接口定义
 *\author	林云
 *\date		2009/03/20

功能：
-#	支持各种录像参数（压缩参数）、录像计划（各种录像类型之间的计划）的配置及执行。
-#	录像模式之间优先级的管理
-#	支持高级模式与普通模式的切换
-#	关于通道编码参数、移动侦测参数、马赛克参数及OSD的设置、支持光学参数的普通及高级设置
-#	支持手动录像
-#	支持录像状态的查询或者回调通知
-#	支持移动侦测报警回调
-#	支持预览抓图
-#	支持视频信号异常、编码异常等报警
 */


#ifndef _INC_MPU_AVL_API_H_
#define _INC_MPU_AVL_API_H_

#include "RECT.h"
#include "AVL/enumRecordMode.h"
#include "AVL/tagEncCfg.h"
#include "AVL/tagRecordSchedule.h"
#include "AVL/tagMotionSchedule.h"
#include "AVL/tagAdvancEncCfg.h"
#include "CBAlert.h"
#include "zwsdk.h"
#include "AVL/tagCharBitMap.h"
#include "newhie/HieServer_Cfg.h"


/**************************************************************************
 *                            录像                                        *
 **************************************************************************/

//!\brief   获取通道个数
//!\param   void
//!\return  返回编码通道个数
int MPU_AVL_GetChNum();

//!\brief   获取制式
//!\param   int *pStandard  接受制式,1-NTSC,2-PAL,4-SECAM,0x80000000-none
//!\return  成功0,否则失败
int MPU_AVL_GetStandard(int *pnStandard);

//!\brief   设置制式
//!\param   int Standard    1-NTSC,2-PAL,4-SECAM
//!\return  成功0,否则失败
int MPU_AVL_SetStandard(int nStandard);


int MPU_AVL_SetActiveCode(char *pcode,int unAllActivedFunctionMask);
int MPU_AVL_GetActiveCode(char *pcode, int codeSize, char *pcodeFlag, char *activestatus,
	unsigned int *puSupportActivedFeatureMask, unsigned int *puAllActivedStatusMask);

//!\brief   启动录像模式：手动、自动、停止三种模式
//!\param   int nCh                 通道
//!\param   enumRecordeMode model   录像模式的一种
//!\return 成功为0，其它为错误码
int MPU_AVL_Record_StartMode(int nCh, enumRecordMode model);

//!\brief       获取录像模式：手动、自动、停止三种模式
//!\param   int nCh 通道
//!\return   录像模式enumRecordeMode model 录像模式的一种
enumRecordMode MPU_AVL_Record_GetRecMode(int nCh);

//!\brief   设置录像码流索引
//!\param   int nCh         通道
//!\param   int nIndex	0表示主码流，1表示子码流，2表示高清子码流
//!\return 成功为0，其它为错误码
int MPU_AVL_Record_SetRecStreamIndex(int nCh, int nIndex);

//!\brief       获取录像码率索引
//!\param   int nCh 通道
//!\return   录像码流索引(0表示主码流，1表示子码流，2表示高清子码流)
int MPU_AVL_Record_GetRecStreamIndex(int nCh);

int MPU_AVL_Record_SetActionCode(char *pCode,int unAllActivedFunctionMask);
int MPU_AVL_Record_GetActionCode(char *pCode);
int MPU_AVL_Record_Notif_Creat_Channel(int *ret);


int MPU_AVL_Record_SetRtmpParam(int tpye, int nCh, char *szUserName ,char *szPassword ,\
char *szStreamPullingAddress,unsigned int eable, unsigned int status, char cPushAudioEnable);

int  MPU_AVL_Record_GetRtmpParam(int tpye, int nCh, char *szUserName ,int UserNameSize, char *szPassword , int PwdSize,
char *szStreamPullingAddress, int strPullAddrSize, unsigned int *eable, unsigned int *status, char *szPlayURL, int playURLSize, char *cPushAudioEnable);

int MPU_AVL_Record_SetPushAddressCfg(char enable ,char *PushURL);
int MPU_AVL_Record_GetPushAddressCfg(char *bsupport, char *enable, char *pushurl );

//!\brief   暂停/继续录像
//!\param   int bPause  暂停或者继续的标志。
//!\param   int nCh     通道
//!\return  成功为0，失败为-1
int MPU_AVL_Record_Pause(int nCh, int bPause);

//!\brief   获取当前录像的状态
//!\param   int nCh                 通道号
//!\return  返回当前录像的状态，失败为-1
int MPU_AVL_Record_GetState(int nCh);

//!\brief   获取编码参数
//!\param   int nCh 通道
//!\param   tagEncCfg *pInfo    编码参数结构体指针
//!\param   int nEnctype        录像的编码类型
//!\return  成功为0，失败为-1
int MPU_AVL_Record_GetEncCfg(int nCh, tagEncCfg * pInfo, int nEnctype);

//!\brief   设置编码参数
//!\param   int nCh             通道
//!\param   tagEncCfg *pInfo    编码参数结构体
//!\param   int nEnctype        录像的编码类型
//!\return 成功为0，失败为-1
int MPU_AVL_Record_SetEncCfg(int nCh, tagEncCfg * pInfo, int nEnctype);

//!\brief   获取预录时间
//!\param   int nCh 通道
//!\return  预录时间
int MPU_AVL_Record_GetPreTime(int nCh);

//!\brief   设置预录时间
//!\param   int nCh 通道
//!\param   int nTime 预录时间
//!\return  成功为0，失败为-1
int MPU_AVL_Record_SetPreTime(int nCh, int nTime);

//!\brief   获取延迟时间
//!\param   int nCh 通道
//!\return  延迟时间
int MPU_AVL_Record_GetDelayTime(int nCh);

//!\brief   设置延迟时间
//!\param   int nCh 通道
//!\param   int nTime 延迟时间
//!\return  成功为0，失败为-1
int MPU_AVL_Record_SetDelayTime(int nCh, int nTime);

//!\brief   获取录像的时间计划
//!\param   int nCh             通道
//!\param   tagRecordSchedule *pCfg   时间计划结构体
//!\return  成功为0，失败为-1
int MPU_AVL_Record_GetSchedule(int nCh, tagRecordSchedule * pCfg);

//!\brief   设置录像的时间计划
//!\param   int nCh 通道
//!\param   tagRecordSchedule *pCfg   时间计划结构体
//!\return  成功为0，失败为-1
int MPU_AVL_Record_SetSchedule(int nCh, tagRecordSchedule * pCfg);

//!\brief   获取新的录像的时间计划
//!\param   int nCh             通道
//!\param   tagRecordScheduleV2 *pCfg   时间计划结构体
//!\return  成功为0，失败为-1
int MPU_AVL_Record_GetNewSchedule(tagRecordScheduleV2 *pCfg);

//!\brief   设置新的录像的时间计划
//!\param   int nCh 通道
//!\param   tagRecordSchedule *pCfg   时间计划结构体
//!\return  成功为0，失败为-1
int MPU_AVL_Record_SetNewSchedule(int nCh, tagRecordScheduleV2 *pCfg);

//!\brief   定制录像模式
//!\param   int nCustomMode     0为一般模式，1为高级模式
//!\return  成功为0，失败为-1
int MPU_AVL_Record_SetWorkMode(int nCustomMode);

//!\brief   获取定制的信息
//!\param   int *pnCustomMode   0为一般模式，1为高级模式
//!\return  成功为0，失败为-1
int MPU_AVL_Record_GetWorkMode(int *pnCustomMode);

//!\brief   获取主码流码率
//!\param  int nCh 通道
//!\return  成功为主码流信息，单位（Kb）,失败为-1
int MPU_AVL_Coder_GetBitInfo(int nCh);

//!\brief 强制一个I帧
//!\param int nCh 通道
//!\return  成功为0，失败为-1
int MPU_AVL_Record_EncCaptureIFrame(int nCh, int nSensorIndex);

//!\brief   恢复编码参数到出厂设置
//!\param   int nCh             通道
//!\param   int nEnctype        录像的编码类型
//!\return 成功为0，失败为-1
int MPU_AVL_Record_ResumeEncCfg(int nCh, int nEnctype);

//!\brief   设置HC板型资源分配模式
//!\param   int nMode       0-默认值表示同编同解，1-仅支持4路解码，2-仅支持1路解码
//!\param   int *pnChMask 支持实时D1编码通道掩码
//!\return  成功为0，其它为错误码
int MPU_AVL_SetHCWorkMode(int nMode, int nChMask);

//!\brief   获取HC板型资源分配模式
//!\param   int *pnMode 0-默认值表示同编同解，1-仅支持4路解码，2-仅支持1路解码
//!\param   int *pnChMask 支持实时D1编码通道掩码
//!\return  成功为0，失败为-1
int MPU_AVL_GetHCWorkMode(int *pnMode, int *pnChMask);


/**************************************************************************
 *                            OSD                                         *
 **************************************************************************/

//!\brief   获取OSD显示模式
//!\param   int nCh             通道
//!\param   int *pnShowType     0为通道时间均不显示、1为显示通道，2、为显示时间、3为两者均显示
//!\return  成功为0，失败为-1
int MPU_AVL_OSD_GetType(int nCh, int *pnShowType);

//!\brief   设置OSD参数配置(需要实现通道别名+位置，时间格式+位置)
//!\param   int nCh 通道
//!\param   int nShowType OSD类型
//!\return  成功为0，失败为-1
int MPU_AVL_OSD_SetType(int nCh, int nShowType);

//!\brief   获取时间格式
//!\param   int nCh             通道
//!\param   int *pnShowFortMat  时间格式。根据底层的实际情况来确定可以设置哪些格式
//!\return  成功为0，失败为-1
int MPU_AVL_OSD_GetTimeFormat(int nCh, int *pnShowFortMat);

//!\brief   设置时间格式
//!\param   int nCh 通道
//!\param   int nShowFortMat    时间格式。根据底层的实际情况来确定可以设置哪些格式
//!\return  成功为0，失败为-1
int MPU_AVL_OSD_SetTimeFormat(int nCh, int nShowFortMat);

//!\brief       获取通道字符点阵
//!\param   int nCh                     通道号
//!\param   char *pCharBitMap   位图文件名
//!\param   int nBuffSize       缓冲大小
//!\return  成功为0，失败为-1
int MPU_AVL_OSD_GetCharBitMap(int nCh, char *pCharBitMap, int nBuffSize);

//!\brief       设置通道字符点阵
//!\param   int nCh         通道
//!\param   char *pCharBitMap   位图文件名
//!\return  成功为0，失败为-1
int MPU_AVL_OSD_SetCharBitMap(int nCh, char *pCharBitMap, unsigned char *byLattice ,int FontSize, int Width, int Height);

//!\brief   获取时间或者字符的位置
//!\param   int nCh                 通道
//!\param   int nOsdType            OSD类型 
//!\param   int *pnX                X坐标
//!\param   int *pnY                Y坐标
//!\return  成功为0，失败为-1
int MPU_AVL_OSD_GetPosition(int nCh, int nOsdType, int *pnX, int *pnY);

//!\brief   设置时间或者字符的位置
//!\param   int nCh                 通道
//!\param   int nOsdType            OSD类型 
//!\param   int nX                  X坐标
//!\param   int nY                  Y坐标
//!\return  成功为0，失败为-1
int MPU_AVL_OSD_SetPosition(int nCh, int nOsdType, int nX, int nY);

//!\brief   设置时间或者字符的位置
//!\param   int OSD_TYPE OSD类型 1-文本,2-时间,3-时间+文本
//!\param   int nTimeX, int nTimeY, int nTextX, int nTextY 时间与文件的位置
//!\return  成功为0，失败为-1
int MPU_AVL_OSD_SetPositionAll(int nCh, int nTimeX, int nTimeY, int nTextX, int nTextY, int nOsdType = 3);

//!\brief 获取时间点阵宽高
//!\param   int* pnWidth, int *pnHeight 时间格式。根据底层的实际情况来确定可以设置哪些格式
//!\return   成功为0，失败为-1
int MPU_AVL_OSD_GetTimeWH(int nCh, int *pnWidth, int *pnHeight);

int MPU_AVL_OSD_SetTextEx(char *OSDToken, unsigned short *strText, int CharSet);

int MPU_AVL_OSD_GetTextExByToken(char *OSDToken, unsigned short *strText, int *CharSet);

int MPU_AVL_OSD_GetTextExByNum(int i, char *OSDToken, int tokenSize, unsigned short *strText, int *CharSet);

int MPU_AVL_SetTextOSD_Cprocustom(char *unicde);

int MPU_AVL_OSD_SetOSDV2(int index, int bEnable, char *OSDToken, unsigned short *strText, int nX, int nY);

int MPU_AVL_OSD_GetOSDV2TextByNum(int i, int *bEnable, char *OSDToken, unsigned short *strText, int *nX, int *nY);


/**************************************************************************
 *                            报警                                        *
 **************************************************************************/

//!\brief 设置报警联动回调函数
//!\param AlarmType //!报警输入
//    ALERT_TYPE_ALARMIN,
//   //!移动侦测
//    ALERT_TYPE_MOTION,
//!编码异常
//    ALERT_TYPE_ENCODEERROR,
//!硬盘错误
//    ALERT_TYPE_DISKERROR,
//!硬盘满
//    ALERT_TYPE_DISKFULL,
//!IP冲突
//    ALERT_TYPE_IPCONFLICT,
//!非法访问
//     ALERT_TYPE_ILLEGEACCESS,
//!网线断
//     ALERT_TYPE_RETICLEDISCONNECT,
//!视频丢失
//     ALERT_TYPE_VIDEOLOST,
//!视频异常
//     ALERT_TYPE_VIDEOEXCEPTION,
//!\praram int(*CallBack)(struct tAlertInfo *pAlertInfo,  
//struct tActionInfo *pActionInfo)) 报警回调指针
//!\return  成功为0，失败为-1
//##ModelId=49BDB54F0264
int MPU_AVL_SetAlarmCallBack(int AlarmType, int (*CallBack) (struct tagAlertInfo * pAlertInfo, struct tagActionInfo * pActionInfo));

//!\brief 启用某个报警联动动作
//!\param ch 通道号
//!\param AlarmType //!报警输入
//    ALERT_TYPE_ALARMIN,
//   //!移动侦测
//    ALERT_TYPE_MOTION,
//!编码异常
//    ALERT_TYPE_ENCODEERROR,
//!硬盘错误
//    ALERT_TYPE_DISKERROR,
//!硬盘满
//    ALERT_TYPE_DISKFULL,
//!IP冲突
//    ALERT_TYPE_IPCONFLICT,
//!非法访问
//     ALERT_TYPE_ILLEGEACCESS,
//!网线断
//     ALERT_TYPE_RETICLEDISCONNECT,
//!视频丢失
//     ALERT_TYPE_VIDEOLOST,
//!视频异常
//     ALERT_TYPE_VIDEOEXCEPTION,
//!\praram int Action    !      ACTION_SCREEN 屏幕显示
//  !      ACTION_BEEP 声音提示
//  !      ACTION_ALARMOUT 报警输出
//  !      ACTION_SENDTOSERVER 上传报警服务器
//  !      ACTION_EMAIL 联动Email
//  !      ACTION_SMS 联动短信
//!\praram int bEnable false 为禁止联动 true 为开启联动
//对于联动屏幕显示0为不启用，1为联动主显,2为联动辅显,3为联动主显和辅显
//!\return  成功为0，失败为-1
//##ModelId=49BDEFDF0009
int MPU_AVL_EnableAction(int nCh, int AlarmType, int Action, int bEnable);

// 区域布防联动信息设置
int MPU_AVL_AreaDefense_EnableAction(int nCh, int Action, int bEnable);

//!\brief 判断某个报警联动动作是否开启
//!\param ch 通道号
//!\param AlarmType //!报警输入
//    ALERT_TYPE_ALARMIN,
//   //!移动侦测
//    ALERT_TYPE_MOTION,
//!编码异常
//    ALERT_TYPE_ENCODEERROR,
//!硬盘错误
//    ALERT_TYPE_DISKERROR,
//!硬盘满
//    ALERT_TYPE_DISKFULL,
//!IP冲突
//    ALERT_TYPE_IPCONFLICT,
//!非法访问
//     ALERT_TYPE_ILLEGEACCESS,
//!网线断
//     ALERT_TYPE_RETICLEDISCONNECT,
//!视频丢失
//     ALERT_TYPE_VIDEOLOST,
//!视频异常
//     ALERT_TYPE_VIDEOEXCEPTION,
//!\praram int Action    !      ACTION_SCREEN 屏幕显示
//  !      ACTION_BEEP 声音提示
//  !      ACTION_ALARMOUT 报警输出
//  !      ACTION_SENDTOSERVER 上传报警服务器
//  !      ACTION_EMAIL 联动Email
//  !      ACTION_SMS 联动短信
//!\return false 为禁止联动 true 为开启联动,
//对于联动屏幕显示0为不启用，1为联动主显,2为联动辅显,3为联动主显和辅显
//##ModelId=49BDEFDF0081
int MPU_AVL_IsEnableAction(int nCh, int AlarmType, int Action);

// 区域布防联动信息获取
int MPU_AVL_AreaDefense_IsEnableAction(int nCh, int Action);

//!\brief 获取某个报警联动录像参数
//!\param ch 通道号
//!\param AlarmType //!报警输入
//    ALERT_TYPE_ALARMIN,
//   //!移动侦测
//    ALERT_TYPE_MOTION,
//!编码异常
//    ALERT_TYPE_ENCODEERROR,
//!硬盘错误
//    ALERT_TYPE_DISKERROR,
//!硬盘满
//    ALERT_TYPE_DISKFULL,
//!IP冲突
//    ALERT_TYPE_IPCONFLICT,
//!非法访问
//     ALERT_TYPE_ILLEGEACCESS,
//!网线断
//     ALERT_TYPE_RETICLEDISCONNECT,
//!视频丢失
//     ALERT_TYPE_VIDEOLOST,
//!视频异常
//     ALERT_TYPE_VIDEOEXCEPTION,
//!\param dwChannelFlag 联动的通道标记 
//每一位表示联动一个通道 1表示联动通道1 
//2表示联动通道2录像 3表示联动通道1和2录像,以此类推
//!\param nPort 某个报警通道号
//!\return 0表示获取成功，-1表示获取失败
//##ModelId=49B7187602BF
int MPU_AVL_GetRecordParam(int nCh, int AlarmType, int *dwChannelFlag);

int MPU_AVL_AreaDefense_GetRecordParam(int nCh, int *dwChannelFlag);

//!\brief 设置某个报警联动录像参数
//!\param ch 通道号
//!\param AlarmType //!报警输入
//    ALERT_TYPE_ALARMIN,
//   //!移动侦测
//    ALERT_TYPE_MOTION,
//!编码异常
//    ALERT_TYPE_ENCODEERROR,
//!硬盘错误
//    ALERT_TYPE_DISKERROR,
//!硬盘满
//    ALERT_TYPE_DISKFULL,
//!IP冲突
//    ALERT_TYPE_IPCONFLICT,
//!非法访问
//     ALERT_TYPE_ILLEGEACCESS,
//!网线断
//     ALERT_TYPE_RETICLEDISCONNECT,
//!视频丢失
//     ALERT_TYPE_VIDEOLOST,
//!视频异常
//     ALERT_TYPE_VIDEOEXCEPTION,
//!\param dwChannelFlag 联动录像通道标记
//!\param nPort 某个报警通道号
//!\return 0表示设置成功 -1表示设置失败
//##ModelId=49B718640232
int MPU_AVL_SetRecordParam(int nCh, int AlarmType, int dwChannelFlag);

int MPU_AVL_AreaDefense_SetRecordParam(int nCh, int dwChannelFlag);

//!\brief 
//获取某个报警联动拍照参数,拍照张数和间隔由CCapturePic::GetCfg获取
//!\param ch 通道号
//!\param AlarmType //!报警输入
//    ALERT_TYPE_ALARMIN,
//   //!移动侦测
//    ALERT_TYPE_MOTION,
//!编码异常
//    ALERT_TYPE_ENCODEERROR,
//!硬盘错误
//    ALERT_TYPE_DISKERROR,
//!硬盘满
//    ALERT_TYPE_DISKFULL,
//!IP冲突
//    ALERT_TYPE_IPCONFLICT,
//!非法访问
//     ALERT_TYPE_ILLEGEACCESS,
//!网线断
//     ALERT_TYPE_RETICLEDISCONNECT,
//!视频丢失
//     ALERT_TYPE_VIDEOLOST,
//!视频异常
//     ALERT_TYPE_VIDEOEXCEPTION,
//!\param dwChannelFlag 存储联动拍照通道标记的缓冲区
//!\param nPort 报警联动输入通道号
//!\return 0表示成功 -1表示失败
//##ModelId=49B5CE9A030D
int MPU_AVL_GetSnapshotParam(int nCh, int AlarmType, int *dwChannelFlag);

int MPU_AVL_AreaDefense_GetSnapshotParam(int nCh, int *dwChannelFlag);

//!\brief 
//设置某个报警联动拍照参数，拍照张数和拍照间隔由CCapturePi
//c::SetCfg设定
//!\param ch 通道号
//!\param AlarmType //!报警输入
//    ALERT_TYPE_ALARMIN,
//   //!移动侦测
//    ALERT_TYPE_MOTION,
//!编码异常
//    ALERT_TYPE_ENCODEERROR,
//!硬盘错误
//    ALERT_TYPE_DISKERROR,
//!硬盘满
//    ALERT_TYPE_DISKFULL,
//!IP冲突
//    ALERT_TYPE_IPCONFLICT,
//!非法访问
//     ALERT_TYPE_ILLEGEACCESS,
//!网线断
//     ALERT_TYPE_RETICLEDISCONNECT,
//!视频丢失
//     ALERT_TYPE_VIDEOLOST,
//!视频异常
//     ALERT_TYPE_VIDEOEXCEPTION,
//!\param dwChannelFlag 联动拍照通道标记
//!\param nPort 某个报警通道号
//!\return 0表示设置成功 -1表示失败
//##ModelId=49B5CDFE00BB
int MPU_AVL_SetSnapshotParam(int nCh, int AlarmType, int dwChannelFlag);

int MPU_AVL_AreaDefense_SetSnapshotParam(int nCh, int dwChannelFlag);

//!\brief 获取某一某个报警通道联动某一云台的动作参数
//!\param ch 通道号
//!\param AlarmType //!报警输入
//    ALERT_TYPE_ALARMIN,
//   //!移动侦测
//    ALERT_TYPE_MOTION,
//!编码异常
//    ALERT_TYPE_ENCODEERROR,
//!硬盘错误
//    ALERT_TYPE_DISKERROR,
//!硬盘满
//    ALERT_TYPE_DISKFULL,
//!IP冲突
//    ALERT_TYPE_IPCONFLICT,
//!非法访问
//     ALERT_TYPE_ILLEGEACCESS,
//!网线断
//     ALERT_TYPE_RETICLEDISCONNECT,
//!视频丢失
//     ALERT_TYPE_VIDEOLOST,
//!视频异常
//     ALERT_TYPE_VIDEOEXCEPTION,
//!\param nAction 联动动作 
//*nAction=0表示无动作，1联动预置点，2联动巡航，3联动轨迹
//!\param nIndex 联动预置点/巡航/轨迹的编号
//!\param nChannel 云台对应的通道号
//!\param nPort 某个报警通道号
//!\return 0表示获取成功, -1表示获取失败
//##ModelId=49B718B202DE

int MPU_AVL_GetPtzParam(int nCh, int AlarmType, int nChannel, int *nAction, int *nIndex);

int MPU_AVL_AreaDefense_GetPtzParam(int nCh, int nChannel, int *nAction, int *nIndex);

//!\brief 设置某个报警通道联动某一个云台的动作
//!\param ch 通道号
//!\param AlarmType //!报警输入
//    ALERT_TYPE_ALARMIN,
//   //!移动侦测
//    ALERT_TYPE_MOTION,
//!编码异常
//    ALERT_TYPE_ENCODEERROR,
//!硬盘错误
//    ALERT_TYPE_DISKERROR,
//!硬盘满
//    ALERT_TYPE_DISKFULL,
//!IP冲突
//    ALERT_TYPE_IPCONFLICT,
//!非法访问
//     ALERT_TYPE_ILLEGEACCESS,
//!网线断
//     ALERT_TYPE_RETICLEDISCONNECT,
//!视频丢失
//     ALERT_TYPE_VIDEOLOST,
//!视频异常
//     ALERT_TYPE_VIDEOEXCEPTION,
//!\param nAction 联动的动作 0表示无动作 1联动预置点 
//2联动巡航 3联动轨迹
//!\param nIndex 联动的预置点/巡航/轨迹的编号
//!\param nChannel 云台对应的通道号
//!\param nPort 某个报警通道号
//!\return 0表示成功 -1表示失败
//##ModelId=49B718BD0261
int MPU_AVL_SetPtzParam(int nCh, int AlarmType, int nChannel, int nAction, int nIndex);

int MPU_AVL_AreaDefense_SetPtzParam(int nCh, int nChannel, int nAction, int nIndex);

//!\brief 获取某个报警联动报警输出的参数
//!\param ch 通道号
//!\param AlarmType //!报警输入
//    ALERT_TYPE_ALARMIN,
//   //!移动侦测
//    ALERT_TYPE_MOTION,
//!编码异常
//    ALERT_TYPE_ENCODEERROR,
//!硬盘错误
//    ALERT_TYPE_DISKERROR,
//!硬盘满
//    ALERT_TYPE_DISKFULL,
//!IP冲突
//    ALERT_TYPE_IPCONFLICT,
//!非法访问
//     ALERT_TYPE_ILLEGEACCESS,
//!网线断
//     ALERT_TYPE_RETICLEDISCONNECT,
//!视频丢失
//     ALERT_TYPE_VIDEOLOST,
//!视频异常
//     ALERT_TYPE_VIDEOEXCEPTION,
//!\param dwOutPortFlag 存储输出通道标记的缓冲区
//!\param nPort 某个报警通道号
//!\return 0表示成功 -1表示失败
//##ModelId=49B718DD030D
int MPU_AVL_GetDopParam(int nCh, int AlarmType, int *dwOutPortFlag);

int MPU_AVL_GetPushEventIntvCnt(int nCh, int AlarmType, int *nPushEventIntv);

int MPU_AVL_SetPushEventIntvCnt(int nCh, int AlarmType, int nPushEventIntv);

int MPU_AVL_AreaDefense_GetDopParam(int nCh, int *dwOutPortFlag);

//!\brief 设置某个报警联动报警输出的参数
//!\param ch 通道号
//!\param AlarmType //!报警输入
//    ALERT_TYPE_ALARMIN,
//   //!移动侦测
//    ALERT_TYPE_MOTION,
//!编码异常
//    ALERT_TYPE_ENCODEERROR,
//!硬盘错误
//    ALERT_TYPE_DISKERROR,
//!硬盘满
//    ALERT_TYPE_DISKFULL,
//!IP冲突
//    ALERT_TYPE_IPCONFLICT,
//!非法访问
//     ALERT_TYPE_ILLEGEACCESS,
//!网线断
//     ALERT_TYPE_RETICLEDISCONNECT,
//!视频丢失
//     ALERT_TYPE_VIDEOLOST,
//!视频异常
//     ALERT_TYPE_VIDEOEXCEPTION,s
//!\param dwOutPortFlag 设置输出通道标记
//!\param nPort 某个报警通道号
//!\return 0表示成功 -1表示失败
//##ModelId=49B718ED00AB
int MPU_AVL_SetDopParam(int nCh, int AlarmType, int dwOutPortFlag);

int MPU_AVL_AreaDefense_SetDopParam(int nCh, int dwOutPortFlag);

//!\brief   获取音量大小
//!\param   int nCh             通道
//!\return  音量大小值，失败为-1
int MPU_AVL_GetAudioInVol(int nCh);
int MPU_AVL_SetAudioInVol(int nCh, int vol);

int MPU_AVL_GetAudioOutVol(int nCh);
int MPU_AVL_SetAudioOutVol(int nCh, int vol);
int MPU_AVL_SetAudioMappingVol(int nCh, int vol, int Type);

int MPU_AVL_SetAudioAlarmCfg(int nCh, int enable, int audioType, int audioDelayOutput, int audioVolume, int iAudioPlayTimes);

int MPU_AVL_GetAudioAlarmCfg(int nCh, int *enable, int *audioType, int *audioDelayOutput, int *audioVolume, int *iAudioPlayTimes);

int MPU_AVL_SetAudioCustomVoice(int nCh, int customtype, char *pBuff, int len);

int MPU_AVL_GetGetRealKeyFrameInterval(int nCh, int stream, int *nFrameRateInterval, int *nKeyFrameInterval);

int MPU_AVL_GetKeyFrameInterval(int nCh, int stream, int *value);

int MPU_AVL_SetKeyFrameInterval(int nCh, int stream, int value);

int MPU_AVL_TryListenAlarmAudio(int nCh, int enable, int audioType, int audioDelayOutput, int audioVolume, int alarmType);

/**************************************************************************
 *                            子码流                                      *
 **************************************************************************/

//!\brief   判断是否启用码流
//!\param   int nCh 通道
//!\return  FALSE为不启用，TRUE为启用
BOOL MPU_AVL_AuxCoder_IsEnable(int nCh);

//!\brief 启用/不启用子码流
//!\param int nCh 通道
//!\param BOOL bEnable FALSE 为不启用，TRUE为启用
//!\return  成功为0，失败为-1
int MPU_AVL_AuxCoder_Enable(int nCh, BOOL bEnable);

//!\brief   设置子码流帧率
//!\param   int nCh             通道
//!\param   int nFrameRate      帧率
//!\return  成功为0，失败为-1
int MPU_AVL_AuxCoder_SetFrameRate(int nCh, int nFrameRate);

//!\brief   获取子码流帧率
//!\param   int nCh             通道
//!\return  子码流帧率，失败为-1
int MPU_AVL_AuxCoder_GetFrameRate(int nCh);

//!\brief   设置子码流码率
//!\param   int nCh             通道
//!\param   int nBitRate        码率
//!\return  成功为0，失败为-1
int MPU_AVL_AuxCoder_SetBitRate(int nCh, int nBitRate);

//!\brief   获取子码流码率
//!\param   int nCh 通道
//!\return  成功为子码流码率，失败为-1
int MPU_AVL_AuxCoder_GetBitRate(int nCh);

//!\brief       获取子码流码率
//!\param   int nCh 通道
//!\return   成功为子码流信息，单位（Kb）,失败为-1
int MPU_AVL_AuxCoder_GetBitInfo(int nCh);

//!\brief 获取子码流码率质量
//!\praram int nCh 通道
//!\return  成功为0，失败为-1
int MPU_AVL_AuxCoder_GetQuality(int nCh);

//!\brief 设置子码流码率质量
//!\praram int nCh 通道
//!\return  成功为0，失败为-1
int MPU_AVL_AuxCoder_SetQuality(int nCh, int dwQuality);

//!\brief 获取子码流码率类型
//!\praram int nCh 通道
//!\return  成功为0，失败为-1
int MPU_AVL_AuxCoder_GetBitrateType(int nCh);

//!\brief 设置子码流码率类型
//!\praram int nCh 通道
//!\return  成功为0，失败为-1
int MPU_AVL_AuxCoder_SetBitrateType(int nCh, int dwBitrateType);

//!\brief 获取子码流码流类型
//!\praram int nCh 通道
//!\return  成功为0，失败为-1
int MPU_AVL_AuxCoder_GetStreamType(int nCh);

//!\brief 设置子码流流类型
//!\praram int nCh 通道
//!\return  成功为0，失败为-1
int MPU_AVL_AuxCoder_SetStreamType(int nCh, int dwStreamType);

int MPU_AVL_TUTK_GetMedia(int nCh, int *whichMedia);

int MPU_AVL_TUTK_SetQualityCfg(int nCh, int tutkQuality);

int MPU_AVL_TUTK_GetQualityCfg(int nCh, int *tutkQuality);

int MPU_AVL_TUTK_GetQualityboardCfg(int nCh, int count, int *dwStreamType);


//!\brief 启用/不启用高清子码流
//!\param int nCh 通道
//!\param BOOL bEnable FALSE 为不启用，TRUE为启用
//!\return  成功为0，失败为-1
int MPU_AVL_HDAuxCoder_Enable(int nCh, BOOL bEnable);

BOOL MPU_AVL_HDAuxCoder_IsEnable(int nCh);

//!\brief       获取子码流码率
//!\param   int nCh 通道
//!\return   成功为子码流信息，单位（Kb）,失败为-1
int MPU_AVL_HDAuxCoder_GetBitInfo(int nCh);
//!\brief   获取高清子码流参数
//!\param   int nCh 通道
//!\param   tagEncCfg * pInfo 编码参数
//!\return  成功为0，失败为-1
int MPU_AVL_HDAuxCoder_GetEncCfg(int nCh, tagEncCfg * pInfo);

//!\brief       设置高清子码流参数
//!\param   int nCh 通道
//!\param   tagEncCfg * pInfo 编码参数
//!\return  成功为0，失败为-1
int MPU_AVL_HDAuxCoder_SetEncCfg(int nCh, tagEncCfg * pInfo);

//!\brief   设置子码流尺寸
//!\param   int nCh             通道
//!\param   int byResolution 子码流尺寸        
//!\return  成功为0，失败为-1
int MPU_AVL_AuxCoder_SetPictureFormat(int nCh, int byResolution, int width, int height);

//!\brief   获取子码流尺寸
//!\param   int nCh 通道
//!\return  成功为子码流码率，失败为-1
int MPU_AVL_AuxCoder_GetPictureFormat(int nCh, int *width, int *height);

#ifdef USE_FOURTH_STREAM

//!\brief   获取第四码流参数
//!\param   int nCh 通道
//!\param   tagEncCfg * pInfo 编码参数
//!\return  成功为0，失败为-1
int MPU_AVL_FourthStream_GetEncCfg(int nCh, tagEncCfg * pInfo);

//!\brief   设置第四码流参数
//!\param   int nCh 通道
//!\param   tagEncCfg * pInfo 编码参数
//!\return  成功为0，失败为-1
int MPU_AVL_FourthStream_SetEncCfg(int nCh, tagEncCfg * pInfo);

#endif


/**************************************************************************
 *                            图像                                        *
 **************************************************************************/

int MPU_AVL_Pic_GetSupport(int type);

//!\brief   获取图像参数配置的工作模式
//!\return  0为普通工作模式，1为高级工作模式，失败为-1
int MPU_AVL_Pic_GetWorkMode();

//!\brief 设置图像参数配置的工作模式
//!\param int nWorkMode 
//0为普通工作模式，1为高级工作模式，失败为-1
//!\return  成功为0，失败为-1
int MPU_AVL_Pic_SetWorkMode(int nWorkMode);

//!\brief   设置图像参数
//!\param   int nCh         通道
//!\param   int nTimeSec    时间段。0为缺省参数（普通工作模式下只需要设置缺省参数即可），1为时间段1参数...  
//!\param   int nType       参数类型。0为亮度，1为色度，2为对比度，3为饱和度
//!\param   int nValue      数值。0-255
//!\return  成功为0，失败为-1
int MPU_AVL_Pic_SetPicParam(int nCh, int nTimeSec, int nType, int nValue);

//!\brief   获取图像参数
//!\param   int nCh         通道
//!\param   int nTimeSec    时间段。0为缺省参数（普通工作模式下只需要设置缺省参数即可），1为时间段1参数...  
//!\param   int nType       参数类型。0为亮度，1为色度，2为对比度，3为饱和度
//!\return  成功为对应参数，失败为-1
int MPU_AVL_Pic_GetPicParam(int nCh, int nTimeSec, int nType);

//!\brief   设置图像时间段参数
//!\param   int nCh 通道
//!\param   int nTimeSec 时间段。1为时间段1参数...  
//!\param   int nStartTime  开始时间 相对00:00时的秒数  
//!\param   int nStopTime       结束时间 相对00:00时的秒数  
//!\return  成功为0，失败为-1
int MPU_AVL_Pic_SetTimeCfg(int nCh, int nTimeSec, int nStartTime, int nStopTime);

//!\brief   获取图像时间段参数
//!\param   int nCh 通道
//!\param   int nTimeSec 时间段。1为时间段1参数...  
//!\param   int *pnStartTime    开始时间 相对00:00时的秒数  
//!\param   int *pnStopTime     结束时间 相对00:00时的秒数  
//!\return  1为启用了该时间段，0为没有启用该时间段，-1获取失败
int MPU_AVL_Pic_GetTimeCfg(int nCh, int nTimeSec, int *pnStartTime, int *pnStopTime);

//!\brief 设置是否为编辑状态，图像参数
//!\praram int bEdit 1-开始编辑，暂停自动配置。0-退出编辑，开始启动自动
//!\return  成功为0，失败为-1
int MPU_AVL_Pic_EnterEdit(int nCh, int bEdit);

//!\brief   抓图
//!\param   int nCh 通道
//!\param   BOOL bAlarm     是否为联动抓图。FALSE为手动抓图，TRUE为联动抓图
//!\param int AlarmType 触发源类型
//!\param int AlarmCh 触发源通道
//!\return  成功为0，失败为-1
int MPU_AVL_Pic_Capture(int nCh, BOOL bAlarm, int nAlarmType = 0, int nAlarmCh = 0);

//!\brief       抓图
//!\praram  int nCh         通道号
//!\praram  char *pszFileName   传出文件名称(>=100 bytes)
//!\return  成功为0，失败为-1
int MPU_AVL_Pic_Capture(int nCh, char *pszFileName);

//!\brief   获取抓拍参数。手动抓图不受该参数影响
//!\param   int nCount          连续抓拍多少张
//!\param   int nIntervalTime   间隔时间
//!\return  成功为0，失败为-1
int MPU_AVL_Pic_GetCaptureCfg(int nCh, int *pnCount, int *pnIntervalTime, int *pnFormat, int *pnWidth, int *pnHeight, int *pnQuality);

//!\brief   设置抓拍参数。手动抓图不受该参数影响
//!\param   int nCount 连续抓拍多少张
//!\param   int nIntervalTime 间隔时间
//!\return  成功为0，失败为-1
int MPU_AVL_Pic_SetCaptureCfg(int nCh, int nCount, int nIntervalTime, int nFormat, int nWidth, int nHeight, int nQuality);

//!\brief   获取抓码流类型
//!\param   char nStreamType 0表示主码流，1表示第一子码流，2表示第二子码流
//!\return  成功为0，失败为-1

int MPU_AVL_Pic_GetCaptureStreamType(int nCh, char *pnStreamType);

//!\brief   设置抓码流类型
//!\param   char nStreamType 0表示主码流，1表示第一子码流，2表示第二子码流
//!\return  成功为0，失败为-1
int MPU_AVL_Pic_SetCaptureCfgV2(int nCh, int nQualitychar, char nStreamType);

/**************************************************************************
 *                            移动侦测                                    *
 **************************************************************************/

//!\brief   设置移动侦测灵敏度
//!\param   int nCh 通道
//!\param   int nGrade 灵敏度
//!\return  成功为0，其它为错误码
int MPU_AVL_Motion_SetDetectPrecision(int nCh, int nGrade);

//!\brief	设置人形检测灵敏度
//!\param	int nCh 通道
//!\param	int nGrade 灵敏度
//!\return	成功为0，其它为错误码
int MPU_AVL_HumanShape_SetDetectPrecision(int nCh, int nGrade);

//!\brief   获取灵敏度
//!\param   int nCh 通道
//!\return  返回灵敏度
int MPU_AVL_Motion_GetDetectPrecision(int nCh);

//!\brief   获取灵敏度
//!\param   int nCh 通道
//!\return  返回灵敏度
int MPU_AVL_HumanShape_GetDetectPrecision(int nCh);

//!\brief   设置多个移动块的移动侦测
//!\param   int nCh                     通道
//!\param   unsigned char *pMagicBlock  启用块的索引序列数组
//!\param   int nBlockNum               使能的块数
//!\return  成功为0，失败为-1
int MPU_AVL_Motion_EnableMagicBlock(int nCh, unsigned char *pMagicBlock, int nBlockNum);

//!\brief   获得启用了移动侦测的块
//!\param   int nCh                     通道
//!\param   unsigned char *pMagicBlock  启用块的索引序列数组
//!\param   int nMaxBlockNum            缓冲最大可接受的块数(一般 16x12块)
//!\return  >=0 块个数,否则错误;pMagicBlock = NULL 时返回块个数,缓冲大小不够时返回错误
int MPU_AVL_Motion_GetEnableMagicBlock(int nCh, unsigned char *pMagicBlock, int nMaxBlockNum);

int MPU_AVL_Motion_EnableMagicBlock_CGI(int nCh, tagMotionRegion * pRegionMotionCfg, int nMaxRegionMotionNum);

//!\brief 人形检测区域设置
//!\praram int nCh 通道
//!\praram  tagMotionRegion * pRegionCfg 区域结构体
//!\praram  int nRegionNum  区域块数
//!\return  成功为0，失败为-1
int MPU_AVL_HumanShape_SetRegionCfg(int nCh, tagMotionRegion *pRegionCfg, int nRegionNum);

//!\brief   设置多个移动块的移动侦测
//!\param   int nCh                     通道
//!\param   unsigned char *pMagicBlock  启用块的索引序列数组
//!\param   int nBlockNum               使能的块数
//!\return  成功为0，失败为-1
int MPU_AVL_Motion_GetEnableMagicBlock_CGI(int nCh, tagMotionRegion *pRegionMotionCfg, int nMaxRegionMotionNum);

int MPU_AVL_Motion_SetAudioAlarmSchedule(int nCh, tagAudioAlarmSchedule *stSchedule);

int MPU_AVL_Motion_GetAudioAlarmSchedule(int nCh, tagAudioAlarmSchedule *stSchedule);

int MPU_AVL_Motion_CheckAudioAlarmInSchedule(int nCh);

int MPU_AVL_Motion_SetAudioAlarmCfg(int nCh, int nVoiceType, int nBroadcastCount);

int MPU_AVL_Motion_GetAudioAlarmCfg(int nCh, int *nVoiceType, int *nBroadcastCount);


//!\brief 人形检测区域获取
//!\praram int nCh 通道
//!\praram tagMotionRegion * cfg 区域结构体
//!\praram  int nRegionAreas  区域块数
//!\return  >=0 块个数,否则错误
int MPU_AVL_HumanShape_GetRegionCfg(int nCh, tagMotionRegion *pRegionCfg, int nRegionNum);

//!\brief 布防区域获取
//!\praram int nCh 通道
//!\praram struct tagPolygon *cfg 区域结构体
//!\return  >=0 块个数,否则错误
int MPU_AVL_GetPolygonRegionCfg(int nCh, struct tagPolygon *pRegionCfg);

//!\brief 布防区域设置
//!\praram int nCh 通道
//!\praram struct tagPolygon *cfg 区域结构体
//!\return  >=0 块个数,否则错误
int MPU_AVL_SetPolygonRegionCfg(int nCh, struct tagPolygon *pRegionCfg);

//!\brief   如果移动侦测没有使能则使能，如果没有选择区域则设置为全屏
//!\param   int nCh                     通道
//!\return  成功为0，失败为-1
int MPU_AVL_Motion_StartMotionAdjust(int nCh);

//!\brief   启动/不启动移动侦测
//!\param   int nCh         通道
//!\param   BOOL bEnable    是否启用移动侦测。FALSE为不启动，TRUE为启动
//!\return  成功为0，其它为错误码
int MPU_AVL_Motion_Enable(int nCh, BOOL bEnable);

//!\brief 当前是否开启区域功能
//!\praram int nCh 通道
//!\praram int *nFunc 开启的区域功能类型
//!\return  成功为0，失败为-1
int MPU_AVL_AreaDefense_IsEnableFunction(int nCh, int *nFunc);

//!\brief 设置区域功能的使能
//!\nFunc 开启的区域功能
//!\nEnable 区域功能是否使能
//!\exclusion 功能是否互斥
//!\return  成功为0，其它为错误码
int MPU_AVL_AIDetect_SetAIAllFunc(int nCh, int nFunc, int nEnable, int exclusion);

//!\brief   启动/不启动区域布防移动侦测
//!\param   int nCh         通道
//!\param   BOOL bEnable    是否启用移动侦测。FALSE为不启动，TRUE为启动
//!\return  成功为0，其它为错误码
int MPU_AVL_AreaDefense_Motion_Enable(int nCh, BOOL bEnable);

int MPU_AVL_PedestrianFiltering_Enable(int nCh, BOOL bEnable);

//!\brief   启动/不启动人形检测
//!\param   int nCh         通道
//!\param   BOOL bEnable    是否启用移动侦测。FALSE为不启动，TRUE为启动
//!\return  成功为0，其它为错误码
int MPU_AVL_HumanShape_Enable(int nCh, BOOL bEnable);

//!\brief   启动/不启动区域布防人形检测
//!\param   int nCh         通道
//!\param   BOOL bEnable    是否启用移动侦测。FALSE为不启动，TRUE为启动
//!\return  成功为0，其它为错误码
int MPU_AVL_AreaDefense_HumanShape_Enable(int nCh, BOOL bEnable);

int MPU_AVL_AreaDefense_FaceCap_Enable(int nCh, BOOL bEnable);

//!\brief   判断一个通道是否启动了移动侦测
//!\param   int nCh 通道
//!\return  FALSE为不启动，TRUE为启动
int MPU_AVL_Motion_IsEnable(int nCh);

//!\brief   判断一个通道区域布防是否启动了移动侦测
//!\param   int nCh 通道
//!\return  FALSE为不启动，TRUE为启动
int MPU_AVL_AreaDefense_Motion_IsEnable(int nCh);

//!\brief   判断一个通道区域布防是否启动了人形检测
//!\param   int nCh 通道
//!\return  FALSE为不启动，TRUE为启动
int MPU_AVL_AreaDefense_HumanShape_IsEnable(int nCh);

int MPU_AVL_AreaDefense_FaceCap_IsEnable(int nCh);

//!\brief   判断一个通道是否启动了人形过滤
//!\param   int nCh 通道
//!\return  FALSE为不启动，TRUE为启动
int MPU_AVL_PedestrianFiltering_IsEnable(int nCh);

//!\brief   判断一个通道是否启动了人形检测
//!\param   int nCh 通道
//!\return  FALSE为不启动，TRUE为启动
int MPU_AVL_HumanShape_IsEnable(int nCh);

//!\brief       获取移动的时间计划
//!\param       int nCh 通道
//!\param   tagSchedule* cfg 时间计划结构体
//!\return  成功为0，失败为-1
int MPU_AVL_Motion_GetSchedule(int nCh, tagMotionSchedule * pCfg);

//!\brief 获取人形检测时间计划
//!\param tagSchedule* cfg 时间计划结构体
//!\return 成功为0，失败为-1
int MPU_AVL_HumanShape_GetSchedule(int nCh, tagMotionSchedule *pCfg);

//!\brief 获取区域布防时间计划
//!\param tagSchedule* cfg 时间计划结构体
//!\return 成功为0，失败为-1
int MPU_AVL_AreaDefense_GetSchedule(int nCh, tagMotionSchedule *pCfg);

//!\brief       设置移动的时间计划
//!\param       int nCh 通道
//!\param   ttagSchedule cfg 时间计划结构体
//!\return  成功为0，失败为-1
int MPU_AVL_Motion_SetSchedule(int nCh, const tagMotionSchedule * pCfg);

//!\brief       设置人形检测时间计划
//!\param	int nCh 通道
//!\param	ttagSchedule cfg 时间计划结构体
//!\return  成功为0，失败为-1
int MPU_AVL_HumanShape_SetSchedule(int nCh, const tagMotionSchedule *pCfg);

//!\brief       设置区域布防时间计划
//!\param	int nCh 通道
//!\param	ttagSchedule cfg 时间计划结构体
//!\return  成功为0，失败为-1
int MPU_AVL_AreaDefense_SetSchedule(int nCh, const tagMotionSchedule *pCfg);

//!\brief 获取声音报警布防时间计划
//!\param tagSchedule* cfg 时间计划结构体
//!\return 成功为0，失败为-1
int MPU_AVL_AudioAlarm_SetSchedule(int nCh, const tagAudioAlarmSchedule *pCfg);

//!\brief 获取声音报警布防时间计划
//!\param tagSchedule* cfg 时间计划结构体
//!\return 成功为0，失败为-1
int MPU_AVL_AudioAlarm_GetSchedule(int nCh, tagAudioAlarmSchedule *pCfg);

//检查音频报警时间表
int MPU_AVL_AudioAlarm_CheckAudioAlarmInSchedule(int nCh);

//获取人形帧数据
//注意使用完成后,第一时间释放
//否则下次获取将会阻塞(死锁)
int MPU_AVL_HumanShape_GetHumanShapeFrame(int nCh, char **pBuff, int *pLen);

//释放人形帧数据
//与MPU_AVL_HumanShape_GetHumanShapeFrame配合使用
int MPU_AVL_HumanShape_ReleaseHumanShapeFrame(int nCh, char *pBuff);

//获取叠加人形框信息
int MPU_AVL_GetHumanOverlayInfo(int nCh, char *nMainStreamOverlay, char *nSubStreamOverlay);

//设置叠加人形框信息
int MPU_AVL_SetHumanOverlayInfo(int nCh, char nMainStreamOverlay, char nSubStreamOverlay);

//获取叠加布防框信息
int MPU_AVL_GetDetectAreaOverlayInfo(int nCh, int *nMainStreamOverlay, int *nSubStreamOverlay);

//设置叠加布防框信息
int MPU_AVL_SetDetectAreaOverlayInfo(int nCh, int nMainStreamOverlay, int nSubStreamOverlay);

int MPU_AVL_SetHumanformTrackEnable(int nChn,int iEable);
int MPU_AVL_GetHumanformTrackEnable(int nChn);

int MPU_AVL_GetCarOverlayInfo(int nCh, char *nMainStreamCarOverlay, char *nSubStreamCarOverlay);
int MPU_AVL_SetCarOverlayInfo(int nCh, char nMainStreamCarOverlay, char nSubStreamCarOverlay);
#if defined (__IS_SUPPORT_PTZ_BUILTIN__) && defined (__IS_SUPPORT_PTZ_BUILTIN_HUMAN_TRACK__)
/* 跟踪 */
int MPU_AVL_AIDetect_SetSmartTrackCfg(int nCh, int enable, int nTrackTarget, int nSensitive, int ptzFlg);

int MPU_AVL_AIDetect_GetSmartTrackCfg(int nCh, int *pEnable, int *nTrackTarget, int *nSensitive);
#endif

int MPU_AVL_AudioIn_SetInterface(int mode);

int MPU_AVL_AudioIn_GetInterface();

int MPU_AVL_AudioCodec_SetInterface(int ACodec);

int MPU_AVL_AudioCodec_GetInterface();

int MPU_AVL_GetAdvanceEncParam(int nCh, int *NormalAdvEncType, int *EventAdvEncType, tagVIDEO_SHELTER * pCfg);

int MPU_AVL_SetAdvanceEncParam(int nCh, int NormalAdvEncType, int EventAdvEncType, tagVIDEO_SHELTER * pCfg);


int MPU_AVL_GetAuxAdvEncParam(int nCh, int *AuxAdvEncType);

int MPU_AVL_SetAuxAdvEncParam(int nCh, int AuxAdvEncType);

int MPU_AVL_GetHDAuxAdvEncParam(int nCh, int *HDAuxAdvEncType);

int MPU_AVL_SetHDAuxAdvEncParam(int nCh, int HDAuxAdvEncType);

int MPU_AVL_GetFourthStreamAdvEncParam(int nCh, int *pFourthStreamAdvEncType);

int MPU_AVL_SetFourthStreamAdvEncParam(int nCh, int FourthStreamAdvEncType);

int MPU_AVL_GetAuxHDAdvEncParam(int nCh, int *AuxAdvEncType, int *HDAuxAdvEncType);

int MPU_AVL_SetAuxHDAdvEncParam(int nCh, int AuxAdvEncType, int HDAuxAdvEncType);

/* 人脸抓拍 */
int MPU_AVL_SetFaceCapFilterCfg(int nCh, int MinWidth, int MinHeight, int MaxWidth, int MaxHeight);
int MPU_AVL_GetFaceCapFilterCfg(int nCh, int *MinWidth, int *MinHeight, int *MaxWidth, int *MaxHeight);
int MPU_AVL_SetFaceCapRegionCfg(int nCh, int AreaTopLeftX, int AreaTopLeftY, int AreaBottomRightX, int AreaBottomRightY);
int MPU_AVL_GetFaceCapRegionCfg(int nCh, int *AreaTopLeftX, int *AreaTopLeftY, int *AreaBottomRightX, int *AreaBottomRightY);
int MPU_AVL_SetFaceOverlayInfo(int nCh, int nMainStreamOverlay, int nSubStreamOverlay);
int MPU_AVL_GetFaceOverlayInfo(int nCh, int *nMainStreamOverlay, int *nSubStreamOverlay);

int MPU_AVL_AdvFace_GetFaceEventFrame(int nCh, char **pBuff, int *pLen);
int MPU_AVL_AdvFace_ReleaseFaceEventFrame(int nCh, char *pBuff);
int MPU_AVL_SetFaceEventCallBack(int nCh, int (*CallBack) (struct tagAlertInfo *));

int MPU_AVL_FaceDetect_Enable(int nCh, BOOL bEnable);
int MPU_AVL_FaceDetect_IsEnable(int nCh);
int MPU_AVL_FaceDetect_GetSchedule(int nCh, tagMotionSchedule *pCfg);
int MPU_AVL_FaceDetect_SetSchedule(int nCh, const tagMotionSchedule *pCfg);

/* 触发录像完毕之后，发送消息的回调函数 */
int MPU_AVL_SetTrigRecordOverCallBack(int (*CallBack)(unsigned char *));

int MPU_AVI_StopRecordAndUinitMemRecord(int nCh);
//!\brief 设置录像写入状态 0:暂停  1:写入
int MPU_AVL_SetRecordWriteStatus(int nCh, int record_write_status);

/*@breif MPU_AVL_SetCurrentAviNumPerPage 设置每页显示多少个AVI文件
 *@param [IN]  avi_num_per_page 每页显示的AVI文件个数
 *@return 成功返回0
 *@       失败返回-1
 *@note  每页显示的AVI文件个数不能超过当前有效的AVI个数g_read_curr_avi_num，
 *@      每页显示的AVI文件个数不能超过最大能显示的AVI个数MAX_AVI_NUM_ONE_PAGE
 */
int MPU_AVL_SetCurrentAviNumPerPage(int nCh, int avi_num_per_page);

/*@breif MPU_AVL_GetSwitchPageIsChoose 获取当前选中的是哪一个页面
 *@return 成功返回当前选中的是哪一个页面
 *@       失败返回-1
 */
int MPU_AVL_GetSwitchPageIsChoose(int nCh);

/*@breif MPU_AVL_GetCurrentAviNumPerPage 获取显示多少个AVI文件
 *@return 成功返回AVI文件个数
 *@       失败返回-1
 */
int MPU_AVL_GetCurrentAviNumPerPage(int nCh);

/*@breif MPU_AVL_GetCurrentAviTotalNum 获取当前AVI总个数
 *@return 成功返回当前总的AVI文件个数
 *@       失败返回-1
 */
int MPU_AVL_GetCurrentAviTotalNum(int nCh);

/*@breif MPU_AVL_SetSwitchPageIsChoose 设置当前选中的是哪一个页面和当前录像路径类型
 *@param [IN]  choose_page 选中的页面(从1开始)
 *@param [IN]  path_type 当前录像路径类型 0: http路径  1:绝对路径
 *@return 成功返回0
 *@       失败返回-1
 */
int MPU_AVL_SetSwitchPageIsChoose(int nCh, int choose_page, int path_type);

/*@breif MPU_AVL_GetCurrentAviListInfo 从给定的页面里面获取当前AVI文件列表
 *@param [OUT] p_avi_list 获取到的AVI文件列表
 *@param [OUT] p_avi_num_curr_page 当前页面有多少个AVI文件
 *@return 成功返回0
 *@       失败返回-1
 *@note   需要先调用MPU_AVL_SetSwitchPageIsChoose设置当前选中哪一页
 *@       需要先调用MPU_AVL_SetCurrentAviNumPerPage设置当前用户需要设置每页显示多少个AVI文件
 */
int MPU_AVL_GetCurrentAviListInfo(int nCh, void **p_avi_list, int *p_avi_num_curr_page);

/* 设置当前要寻找的AVI的时间段 */
int MPU_AVL_SetCurrentAviSearchTimeSeg(int nCh, DateTime start_time, DateTime stop_time);

/* 设置当前要寻找的PIC的时间段 */
int MPU_AVL_SetCurrentPicSearchTimeSeg(int nCh, DateTime start_time, DateTime stop_time);

/*@breif MPU_AVL_SetCurrentPicNumPerPage 设置每页显示多少个图片文件
 *@param [IN]  pic_num_per_page 每页显示的图片文件个数
 *@return 成功返回0
 *@       失败返回-1
 *@note  每页显示的图片文件个数不能超过当前有效的图片个数g_read_curr_pic_num，
 *@      每页显示的图片文件个数不能超过最大能显示的图片个数MAX_PIC_NUM_ONE_PAGE
 */
int MPU_AVL_SetCurrentPicNumPerPage(int nCh, int pic_num_per_page);

/*@breif MPU_AVL_GetCurrentPicNumPerPage 获取显示多少个图片文件
 *@return 成功返回图片文件个数
 *@       失败返回-1
 */
int MPU_AVL_GetCurrentPicNumPerPage(int nCh);

/*@breif MPU_AVL_GetCurrentPicTotalNum 获取当前图片总个数
 *@return 成功返回当前总的图片文件个数
 *@       失败返回-1
 */
int MPU_AVL_GetCurrentPicTotalNum(int nCh);

/*@breif MPU_AVL_GetRecordFileInfoByFileName 通过文件名获取当前文件的信息
 *@param [IN]  pszFileName 文件名(不包含路径) 
 *@param [OUT]  pRecordFileInfo 获取到的文件信息
 *@return 成功返回0
 *@       失败返回-1
 */
int MPU_AVL_GetRecordFileInfoByFileName(int nCh, const char *pszFileName, void *pRecordFileInfo);


/*@breif MPU_AVL_SetSwitchPicPageIsChoose 设置当前选中的是哪一个页面和当前图片路径类型
 *@param [IN]  choose_page 选中的页面(从1开始)
 *@param [IN]  path_type 当前图片路径类型 0: http路径  1:绝对路径
 *@return 成功返回0
 *@       失败返回-1
 */
int MPU_AVL_SetSwitchPicPageIsChoose(int nCh, int choose_page, int path_type);

/*@breif MPU_AVL_GetSwitchPicPageIsChoose 获取当前选中的是哪一个页面
 *@return 成功返回当前选中的是哪一个页面
 *@       失败返回-1
 */
int MPU_AVL_GetSwitchPicPageIsChoose(int nCh);

/*@breif MPU_AVL_GetCurrentPicListInfo 从给定的页面里面获取当前图片文件列表
 *@param [OUT] p_pic_list 获取到的图片文件列表
 *@param [OUT] p_pic_num_curr_page 当前页面有多少个图片文件
 *@return 成功返回0
 *@       失败返回-1
 *@note   需要先调用SetSwitchpicPageIsChoose设置当前选中哪一页
 *@       需要先调用SetCurrentpicNumPerPage设置当前用户需要设置每页显示多少个图片文件
 */
int MPU_AVL_GetCurrentPicListInfo(int nCh, void **p_pic_list, int *p_pic_num_curr_page);

/* 获取磁盘资源占用情况(单位: MB) */
int MPU_AVL_GetDiskResourceSize(int nCh, int *pRecordMB, int *pPictureMB);

/* 设置录像存放的比例(范围0~100) */
int MPU_AVL_SetVideoUsedSpaceRate(int nCh, int nVideoSpace);

/* 获取录像存放的比例(范围0~100) */
int MPU_AVL_GetVideoUsedSpaceRate(int nCh);

/*@breif MPU_AVL_GetFileReadWriteStatus 获取当前文件的读写状态
 *@param [IN]  file_name 需要操作的文件名(绝对路径)
 *@param [OUT] read_write_status 
 *@            0:  当前文件空闲
 *@            1:  当前文件正在读
 *@            2:  当前文件正在写
 *@param [OUT] file_size 文件大小 (单位: 字节)
 *@return 返回0表示成功
 *@      返回-1表示失败
 */
int MPU_AVL_GetFileReadWriteStatus(int nCh, 
                                             const char *file_name, 
                                             unsigned int *read_write_status,
                                             unsigned int *file_size);


/* 设置获取时间偏移回调 */
int MPU_AVL_SetGetTimeOffsetCallBack(int nCh, 
                                                      int (*CallBack)(int *, int *, int *));

int MPU_AVL_UploadPictureToSdCard(int nChn, const char *pic_name, unsigned int trig_type);

int MPU_AVL_GetDeviceNeedRebootInfo(int nChn, int func_type);

int MPU_AVL_GetTrackStayTime(int nCh, int *nStayTimeSec);

int MPU_AVL_SetTrackStayTime(int nCh, int nStayTimeSec);

int MPU_AVL_GetTrackStatCfg(int nCh, int *trackEnable, int *linkAction);

int MPU_AVL_SetTrackLinkStart(int nCh, int linkStart);

int MPU_AVL_TriggerEvent(int nCh, unsigned long dwEventType, unsigned long dwSubEventType, unsigned long dwParam1, unsigned long dwParam2, unsigned long dwParam3, char * pBuffer, unsigned long dwBufferLen);

int MPU_AVL_UpdateDorStatus(int bDorEnable);

//声音侦测
int MPU_AVL_SoundDetect_SetCfg(int nCh, int nEnable, int nSentive);
int MPU_AVL_SoundDetect_GetCfg(int nCh, int *pEnable, int *pSentive);

#if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)
int MPU_AVL_GetTrackZoomEnable(int nCh, int *nEnable);

int MPU_AVL_SetTrackZoomEnable(int nCh, int nEnable);
#endif

#if defined (__IS_SUPPORT_LONGAWAYHOME__) 
int MPU_AVL_HumanShape_GetLongAwayEnable(int nCh, int *nEnable);
int MPU_AVL_HumanShape_GetLongAwayDuration(int nCh, int *AlarmDuration);
int MPU_AVL_HumanShape_SetLongAwayEnable(int nCh, int nEnable);
int MPU_AVL_HumanShape_SetLongAwayDuration(int nCh, int AlarmDuration);
int MPU_AVL_HumanShape_GetLongAwayPushAlarmTime(int nCh, time_t *stime, time_t *etime);
#endif


#endif							/* _INC_MPU_AVL_API_H_ */
