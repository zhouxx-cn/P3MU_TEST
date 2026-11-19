
/*!
 *\file		MPU_AVP_API.h
 *\brief	MPU模块，回放相关API定义
 *\author	林云
 *\date		2009/03/20

功能：
-# 	支持流方式回放（用于本地录像回放），及文件方式回放（用于U盘上文件回放）
-# 	支持多通道同步回放模式
-# 	流方式回放支持流存储文件系统、jfs文件系统
-# 	回放控制（开始、停止、暂停、快放、慢放、倒放、浏览模式）
-# 	支持多种分屏模式的显示
-# 	回放中快照。
-# 	回放音频

说明：
-# 	本地录像文件的查询功能由STR模块提供
-# 	编码时保证每秒有1个I帧
-# 	播放器创建时，缺省为正常模式，暂停状态，正放，速度为0，浏览时间间隔为1分钟（60秒）
-# 	open文件或流时，不重新初始化模式、音量、浏览时间间隔
-# 	open文件或流时，初始化为正放、暂停状态、速度为0、播放位置为0
-# 	close时，声音被自动关闭，显示输出不控制，由上层处理

暂停状态：
-# 	点播放器暂停按钮，即进入暂停状态
-# 	正常模式下，单步播放，正放解码所有帧并显示下一帧，倒放只解码关键帧并显示上一帧
-# 	浏览模式下，单步播放只解码关键帧，正放显示下一浏览点的关键帧，倒放显示下一浏览点的关键帧

正常模式：
-# 	正放速度 <= 0时，解码所有帧
-# 	正放速度 > 0时，只解码关键帧
-# 	倒放，只解码关键帧
-# 	速度0为1倍速，解所有帧时，每帧显示时间由编码帧率确定；只解关键帧时，每帧显示时间为1s
-# 	速度1为2倍速，每帧显示时间/2
-# 	速度2为4倍速，每帧显示时间/4
-# 	速度3为8倍速，每帧显示时间/8
-# 	速度4为16倍速，每帧显示时间/16
-# 	速度-1为1/2倍速，每帧显示时间*2
-# 	速度-2为1/4倍速，每帧显示时间*4
-# 	速度-3为1/8倍速，每帧显示时间*8
-# 	速度-4为1/16倍速，每帧显示时间*16

浏览模式：
-# 	无论正放/倒放，只解码浏览点的关键帧
-# 	缺省速度为0
-# 	速度0，每帧驻留5s
-# 	速度1，每帧驻留3s
-# 	速度2，每帧驻留1s
-# 	速度-1，每帧驻留10s
-# 	速度-2，每帧驻留15s
-# 	速度-3，每帧驻留20s

拖动模式：

 */

#ifndef _INC_MPU_AVP_API_H_
#define _INC_MPU_AVP_API_H_

#pragma once


#include <time.h>
#include "HANDLE.h"
#include "RECT.h"
#include "MSG.h"



//!\brief   创建文件播放器
//!\return  播放器句柄，NULL表示创建失败
HANDLE MPU_AVP_CreateFilePlayer();

//!\brief   创建流播放器
//!\return  播放器句柄，NULL表示创建失败
HANDLE MPU_AVP_CreateStreamPlayer();

//!\brief   销毁播放器
//!\param   hPlayer     播放器句柄
//!\return  0表示成功，其它为错误码
int MPU_AVP_DestroyPlayer(HANDLE hPlayer);

//!\brief   打开文件
//!\param   hPlayer             播放器句柄
//!\param   pszFullFileName      文件名（含路径）
//!\return  0表示成功，其它为错误码
int MPU_AVP_OpenFile(HANDLE hPlayer, const char *pszFullFileName);

//!\brief   关闭文件
//!\param   hPlayer             播放器句柄
//!\return  0表示成功，其它为错误码
int MPU_AVP_CloseFile(HANDLE hPlayer);

//!\brief   打开流
//!\param   hPlayer             播放器句柄
//!\param   nDiskGroupMask      0表示查询普通分组+报警分组、1表示冗余分组、2表示备份分组
//!\param   nEncMask            编码通道号（第0位为1表示播放编码通道1数据，依此类推）
//!\param   tStartSecond        开始时间（绝对时间，单位为秒）
//!\param   tStopSecond         结束时间（绝对时间，单位为秒）
//!\return  0表示成功，其它为错误码
int MPU_AVP_OpenStream(HANDLE hPlayer, int nDiskGroupMask, int nEncMask, time_t tStartSecond, time_t tStopSecond);

//!\brief   关闭流
//!\param   hPlayer             播放器句柄
//!\return  0表示成功，其它为错误码
int MPU_AVP_CloseStream(HANDLE hPlayer);

//!\brief   向弹出器添加一种录像类型
//!\param   hPlayer             播放器句柄
//!\param   RecType             录像类型（见录像模块定义）
//!\return  0表示成功，其它为错误码
int MPU_AVP_AddRecType(HANDLE hPlayer, int nRecType);

//!\brief   删除弹出器中一种录像类型
//!\param   hPlayer             播放器句柄
//!\param   RecType             录像类型（见录像模块定义）
//!\return  0表示成功，其它为错误码
int MPU_AVP_DelRecType(HANDLE hPlayer, int nRecType);

//!\brief   启动播放，自动取消暂停状态
//!\param   hPlayer             播放器句柄
//!\return  0表示成功，其它为错误码
int MPU_AVP_DoStart(HANDLE hPlayer);

//!\brief   停止播放
//!\param   hPlayer             播放器句柄
//!\return  0表示成功，其它为错误码
int MPU_AVP_DoStop(HANDLE hPlayer);

//!\brief   暂停播放，自动进入暂停状态
//!\param   hPlayer             播放器句柄
//!\return  0表示成功，其它为错误码
int MPU_AVP_DoPause(HANDLE hPlayer);

//!\brief   播放下一帧（浏览模式为播放下一浏览点），自动进入暂停状态
//!\param   hPlayer             播放器句柄
//!\return  0表示成功，其它为错误码
int MPU_AVP_DoStep(HANDLE hPlayer);

//!\brief   设置回调函数
//!\param   hPlayer             播放器句柄
//!\param   pCallback           回调函数指针
//!\param   pContext            回调函数上下文指针
//!\return  0表示成功，其它为错误码
int MPU_AVP_SetCallback(HANDLE hPlayer, MSG_CALLBACK pCallback, void *pContext);

//!\brief   获取播放任务的时间总长
//!\param   hPlayer             播放器句柄
//!\param   ptCountSecond       返回当前任务时间总长，单位为秒
//!\return  0表示成功，其它为错误码
int MPU_AVP_GetCountTime(HANDLE hPlayer, time_t * ptCountSecond);

//!\brief   启动视频输出显示
//!\param   hPlayer         播放器句柄
//!\param   nDisplayMode    显示模式，范围（0～3）分别代表1分屏、4分屏、9分屏和16分屏。
//!\param   nDisplayIndex   窗口分组索引（根据模式范围不同）
//!\param   pRect           各显示窗口坐标
//!\param   nVoDev  0为主显,1为辅显
//!\return  0表示成功，其它为错误码
//
//Mode=0时DispIndex为（0～15），分别代表0～15路；
//Mode=1时DispIndex为（0～3），分别代表0～3路、4～7路、8～11路和12～15路；
//Mode=2时DispIndex为（0～1），分别代表0～7路和8～15路；
//Mode=3时DispIndex只能为0，显示全部的16路。
int MPU_AVP_StartDisplay(HANDLE hPlayer, int nDisplayMode, int nDisplayIndex, RECT * pRect, int nVoDev = 0);

//!\brief   关闭视频输出显示
//!\param   hPlayer             播放器句柄
//!\return  0表示成功，其它为错误码
int MPU_AVP_StopDisplay(HANDLE hPlayer);

//!\brief   启动音频输出
//!\param   hPlayer         播放器句柄
//!\param   nDecID          解码器索引
//!\return  0表示成功，其它为错误码
int MPU_AVP_OpenSound(HANDLE hPlayer, int nDecID);

//!\brief   关闭音频输出
//!\param   hPlayer             播放器句柄
//!\return  0表示成功，其它为错误码
int MPU_AVP_CloseSound(HANDLE hPlayer);

//!\brief   设置播放位置
//!\param   hPlayer             播放器句柄
//!\param   tPositionSecond     播放位置，单位为秒(至播放任务开始时间的相对时间)
//!\return  0表示成功，其它为错误码
int MPU_AVP_SetCurrentPosition(HANDLE hPlayer, time_t tPositionSecond);

//!\brief   获取当前播放位置
//!\param   hPlayer             播放器句柄
//!\param   ptPositionSecond    返回播放位置，单位为秒；如当前没有播放任务，则返回0
//!\return  0表示成功，其它为错误码
int MPU_AVP_GetCurrentPosition(HANDLE hPlayer, time_t * ptPositionSecond);

//!\brief   设置播放方向
//!\param   hPlayer         播放器句柄
//!\param   nDirection      0表示正向，1表示反向
//!\return  0表示成功，其它为错误码
int MPU_AVP_SetDirection(HANDLE hPlayer, int nDirection);

//!\brief   获取播放方向
//!\param   hPlayer         播放器句柄
//!\param   pnDirection     返回方向，0表示正向，1表示反向
//!\return  0表示成功，其它为错误码
int MPU_AVP_GetDirection(HANDLE hPlayer, int *pnDirection);

//!\brief   设置播放速度
//!\param   hPlayer         播放器句柄
//!\param   nSpeed          速度
//!\return  0表示成功，其它为错误码
int MPU_AVP_SetSpeed(HANDLE hPlayer, int nSpeed);

//!\brief   获取播放速度
//!\param   hPlayer             播放器句柄
//!\param   pnSpeed             返回速度
//!\return  0表示成功，其它为错误码
int MPU_AVP_GetSpeed(HANDLE hPlayer, int *pnSpeed);

//!\brief   设置音量
//!\param   hPlayer             播放器句柄
//!\param   nVolume             音量[0-100]
//!\return  0表示成功，其它为错误码
int MPU_AVP_SetVolume(HANDLE hPlayer, int nVolume);

//!\brief   获取音量
//!\param   hPlayer             播放器句柄
//!\param   pnVolume            返回音量[0-100]
//!\return  0表示成功，其它为错误码
int MPU_AVP_GetVolume(HANDLE hPlayer, int *pnVolume);

//!\brief   设置播放模式
//!\param   hPlayer             播放器句柄
//!\param   nMode               0表示正常模式，1表示浏览模式
//!\return  0表示成功，其它为错误码
int MPU_AVP_SetMode(HANDLE hPlayer, int nMode);

//!\brief   获取播放模式
//!\param   hPlayer             播放器句柄
//!\param   pnMode              0表示正常模式，1表示浏览模式
//!\return  0表示成功，其它为错误码
int MPU_AVP_GetMode(HANDLE hPlayer, int *pnMode);

//!\brief   设置浏览模式间隔时间
//!\param   hPlayer             播放器句柄
//!\param   tStepSecond         间隔时间，单位为秒
//!\return  0表示成功，其它为错误码
int MPU_AVP_SetBrowseStepTime(HANDLE hPlayer, time_t tStepSecond);

//!\brief   获取浏览模式间隔时间
//!\param   hPlayer             播放器句柄
//!\param   ptStepSecond        返回间隔时间，单位为秒
//!\return  0表示成功，其它为错误码
int MPU_AVP_GetBrowseStepTime(HANDLE hPlayer, time_t * ptStepSecond);

//!\brief   设置浏览速度
//!\param   hPlayer         播放器句柄
//!\param   nSpeed         浏览速度
//!\return  0表示成功，其它为错误码
int MPU_AVP_SetBrowseSpeed(HANDLE hPlayer, int nBrowseSpeed);

//!\brief   获取浏览速度
//!\param   hPlayer             播放器句柄
//!\param   pnSpeed           返回浏览速度
//!\return  0表示成功，其它为错误码
int MPU_AVP_GetBrowseSpeed(HANDLE hPlayer, int *pnBrowseSpeed);

//!\brief   抓拍快照，抓拍当前显示的所有通道，并自动存盘
//!\param   hPlayer 播放器句柄
//!\param   nDecID      解码器ID
//!\return  0表示成功，其它为错误码
int MPU_AVP_DoSnapshot(HANDLE hPlayer, int nDecID);

//!\brief       进入拖放模式
//!\param   hPlayer 播放器句柄
//!\return  0表示成功，其它为错误码
int MPU_AVP_StartDrag(HANDLE hPlayer);

//!\brief       退出拖放模式
//!\param   hPlayer 播放器句柄
//!\return  0表示成功，其它为错误码
int MPU_AVP_StopDrag(HANDLE hPlayer);

#endif							//_INC_MPU_AVP_API_H_
