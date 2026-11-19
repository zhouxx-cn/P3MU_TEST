
/*!
 *\file		MPU_AVP_API.cpp
 *\brief	MPU模块，回放相关API实现
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
	本地录像文件的查询功能由STR模块提供
 */


#include "MPU_global.h"


#ifndef DEBUG_NOT_USE_AVP_LIB	//调试时，不调用AVP库

#include "AVP/CFilePlayer.h"
#include "AVP/CStreamPlayer.h"


/**************************************************************************
 *                            常量                                        *
 **************************************************************************/

#define FILEPLAYER_HANDLE		((HANDLE)1)	//文件播放器句柄
#define STREAMPLAYER_HANDLE		((HANDLE)2)	//流播放器句柄


/**************************************************************************
 *                            宏定义                                      *
 **************************************************************************/

#define IS_FILEPLAYER(x)		(FILEPLAYER_HANDLE == (x))	//判断是否文件播放器
#define IS_STREAMPLAYER(x)		(STREAMPLAYER_HANDLE == (x))	//判断是否流播放器


//调用播放器执行动作
#define MPU_AVP_CALL(x)															\
do																				\
{																				\
	int nRet = IDVR_OK;															\
																				\
	MPU_CHECK_POINTER_PARAMETER(hPlayer);										\
																				\
	if (IS_FILEPLAYER(hPlayer))													\
	{																			\
		CFilePlayer *pPlayer = CFilePlayer::GetInstance();						\
																				\
		MPU_CHK(NULL != pPlayer);												\
		if (NULL != pPlayer)													\
		{																		\
			nRet = pPlayer->x;													\
		}																		\
		else																	\
		{																		\
			MPU_ERR ("file player not created\n");								\
			nRet = IDVR_MPU_AVP_FILEPLAYER_NOT_CREATED;							\
		}																		\
	}																			\
	else if (IS_STREAMPLAYER(hPlayer))											\
	{																			\
		CStreamPlayer *pPlayer = CStreamPlayer::GetInstance();					\
																				\
		MPU_CHK(NULL != pPlayer);												\
		if (NULL != pPlayer)													\
		{																		\
			nRet = pPlayer->x;													\
		}																		\
		else																	\
		{																		\
			MPU_ERR ("stream player not created\n");							\
			nRet = IDVR_MPU_AVP_STREAMPLAYER_NOT_CREATED;						\
		}																		\
	}																			\
	else																		\
	{																			\
		MPU_ERR ("invalid player handle, hPlayer = %d\n", (int)hPlayer);		\
		nRet = IDVR_MPU_AVP_INVALID_PLAYER_HANDLE;								\
	}																			\
																				\
	return nRet;																\
}																				\
while (0)

#else

//调用播放器执行动作
#define MPU_AVP_CALL(x)															\
do																				\
{																				\
	return IDVR_OK;																\
}																				\
while (0)

#endif							/* #ifndef DEBUG_NOT_USE_AVP_LIB */


/**************************************************************************
 *                     全局函数实现                                       *
 **************************************************************************/

//!\brief   创建文件播放器
//!\return  播放器句柄，NULL表示创建失败
HANDLE MPU_AVP_CreateFilePlayer()
{
#ifndef DEBUG_NOT_USE_AVP_LIB
	//创建单文件播放器实例
	CFilePlayer *pPlayer = CFilePlayer::NewInstance();

	//判断创建是否成功，失败则输出错误信息
	MPU_CHK(NULL != pPlayer);
	if (NULL == pPlayer)
	{
		MPU_ERR("create fail\n");
		return NULL;
	}

	return FILEPLAYER_HANDLE;
#else
	return NULL;
#endif
}

//!\brief   创建流播放器
//!\return  播放器句柄，NULL表示创建失败
HANDLE MPU_AVP_CreateStreamPlayer()
{
#ifndef DEBUG_NOT_USE_AVP_LIB
	//创建流播放器实例
	CStreamPlayer *pPlayer = CStreamPlayer::NewInstance();

	//判断创建是否成功，失败则输出错误信息
	MPU_CHK(NULL != pPlayer);
	if (NULL == pPlayer)
	{
		MPU_ERR("create fail\n");
		return NULL;
	}

	return STREAMPLAYER_HANDLE;
#else
	return NULL;
#endif
}

//!\brief   销毁播放器
//!\param   hPlayer     播放器句柄
//!\return  0表示成功，其它为错误码
int MPU_AVP_DestroyPlayer(HANDLE hPlayer)
{
#ifndef DEBUG_NOT_USE_AVP_LIB
	int nRet = IDVR_OK;

	//输入参数合法性校验
	MPU_CHECK_POINTER_PARAMETER(hPlayer);

	if (IS_FILEPLAYER(hPlayer))
	{
		//销毁文件播放器
		CFilePlayer::ReleaseInstance();
	}
	else if (IS_STREAMPLAYER(hPlayer))
	{
		//销毁流播放器
		CStreamPlayer::ReleaseInstance();
	}
	else
	{
		MPU_ERR("invalid player handle, hPlayer = %d\n", (int) hPlayer);
		nRet = IDVR_MPU_AVP_INVALID_PLAYER_HANDLE;
	}

	return nRet;
#else
	return IDVR_OK;
#endif
}

//!\brief   打开文件
//!\param   hPlayer             播放器句柄
//!\param   pszFullFileName     文件名（含路径）
//!\return  0表示成功，其它为错误码
int MPU_AVP_OpenFile(HANDLE hPlayer, const char *pszFullFileName)
{
	MPU_AVP_CALL(OpenFile(pszFullFileName));
}

//!\brief   关闭文件
//!\param   hPlayer             播放器句柄
//!\return  0表示成功，其它为错误码
int MPU_AVP_CloseFile(HANDLE hPlayer)
{
	MPU_AVP_CALL(CloseFile());
}

//!\brief   打开流
//!\param   hPlayer             播放器句柄
//!\param   nDiskGroupMask      0表示查询普通分组+报警分组、1表示冗余分组、2表示备份分组
//!\param   nEncMask            编码通道号（第0位为1表示播放编码通道1数据，依此类推）
//!\param   tStartSecond        开始时间（绝对时间，单位为秒）
//!\param   tStopSecond         结束时间（绝对时间，单位为秒）
//!\return  0表示成功，其它为错误码
int MPU_AVP_OpenStream(HANDLE hPlayer, int nDiskGroupMask, int nEncMask, time_t tStartSecond, time_t tStopSecond)
{
	MPU_AVP_CALL(OpenStream(nDiskGroupMask, nEncMask, tStartSecond, tStopSecond));
}

//!\brief   关闭流
//!\param   hPlayer             播放器句柄
//!\return  0表示成功，其它为错误码
int MPU_AVP_CloseStream(HANDLE hPlayer)
{
	MPU_AVP_CALL(CloseStream());
}

//!\brief   向弹出器添加一种录像类型
//!\param   hPlayer             播放器句柄
//!\param   RecType             录像类型（见录像模块定义）
//!\return  0表示成功，其它为错误码
int MPU_AVP_AddRecType(HANDLE hPlayer, int nRecType)
{
	MPU_AVP_CALL(AddRecType(nRecType));
}

//!\brief   删除弹出器中一种录像类型
//!\param   hPlayer             播放器句柄
//!\param   RecType             录像类型（见录像模块定义）
//!\return  0表示成功，其它为错误码
int MPU_AVP_DelRecType(HANDLE hPlayer, int nRecType)
{
	MPU_AVP_CALL(DelRecType(nRecType));
}

//!\brief   启动播放，自动取消暂停状态
//!\param   hPlayer             播放器句柄
//!\return  0表示成功，其它为错误码
int MPU_AVP_DoStart(HANDLE hPlayer)
{
	MPU_AVP_CALL(DoStart());
}

//!\brief   停止播放
//!\param   hPlayer             播放器句柄
//!\return  0表示成功，其它为错误码
int MPU_AVP_DoStop(HANDLE hPlayer)
{
	MPU_AVP_CALL(DoStop());
}

//!\brief   暂停播放，自动进入暂停状态
//!\param   hPlayer             播放器句柄
//!\return  0表示成功，其它为错误码
int MPU_AVP_DoPause(HANDLE hPlayer)
{
	MPU_AVP_CALL(DoPause());
}

//!\brief   播放下一帧（浏览模式为播放下一浏览点），自动进入暂停状态
//!\param   hPlayer             播放器句柄
//!\return  0表示成功，其它为错误码
int MPU_AVP_DoStep(HANDLE hPlayer)
{
	MPU_AVP_CALL(DoStep());
}

//!\brief   设置回调函数
//!\param   hPlayer             播放器句柄
//!\param   pCallback           回调函数指针
//!\param   pContext            回调函数上下文指针
//!\return  0表示成功，其它为错误码
int MPU_AVP_SetCallback(HANDLE hPlayer, MSG_CALLBACK pCallback, void *pContext)
{
	MPU_AVP_CALL(SetCallback(pCallback, pContext));
}

//!\brief   获取播放任务的时间总长
//!\param   hPlayer             播放器句柄
//!\param   ptCountSecond       返回当前任务时间总长，单位为秒
//!\return  0表示成功，其它为错误码
int MPU_AVP_GetCountTime(HANDLE hPlayer, time_t * ptCountSecond)
{
	MPU_AVP_CALL(GetCountTime(ptCountSecond));
}

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
int MPU_AVP_StartDisplay(HANDLE hPlayer, int nDisplayMode, int nDisplayIndex, RECT * pRect, int nVoDev)
{
	MPU_AVP_CALL(StartDisplay(nDisplayMode, nDisplayIndex, pRect, nVoDev));
}

//!\brief   关闭视频输出显示
//!\param   hPlayer             播放器句柄
//!\return  0表示成功，其它为错误码
int MPU_AVP_StopDisplay(HANDLE hPlayer)
{
	MPU_AVP_CALL(StopDisplay());
}

//!\brief   启动音频输出
//!\param   hPlayer         播放器句柄
//!\param   nDecIndex       解码器索引
//!\return  0表示成功，其它为错误码
int MPU_AVP_OpenSound(HANDLE hPlayer, int nDecIndex)
{
	MPU_AVP_CALL(OpenSound(nDecIndex));
}

//!\brief   关闭音频输出
//!\param   hPlayer             播放器句柄
//!\return  0表示成功，其它为错误码
int MPU_AVP_CloseSound(HANDLE hPlayer)
{
	MPU_AVP_CALL(CloseSound());
}

//!\brief   设置播放位置
//!\param   hPlayer             播放器句柄
//!\param   tPositionSecond     播放位置，单位为秒(至播放任务开始时间的相对时间)
//!\return  0表示成功，其它为错误码
int MPU_AVP_SetCurrentPosition(HANDLE hPlayer, time_t tPositionSecond)
{
	MPU_AVP_CALL(SetCurrentPosition(tPositionSecond));
}

//!\brief   获取当前播放位置
//!\param   hPlayer                 播放器句柄
//!\param   ptPositionSecond        返回播放位置，单位为秒；如当前没有播放任务，则返回0
//!\return  0表示成功，其它为错误码
//!\return  返回当前播放位置，单位为秒
int MPU_AVP_GetCurrentPosition(HANDLE hPlayer, time_t * ptPositionSecond)
{
	MPU_AVP_CALL(GetCurrentPosition(ptPositionSecond));
}

//!\brief   设置播放方向
//!\param   hPlayer         播放器句柄
//!\param   nDirection      0表示正向，1表示反向
//!\return  0表示成功，其它为错误码
int MPU_AVP_SetDirection(HANDLE hPlayer, int nDirection)
{
	MPU_AVP_CALL(SetDirection(nDirection));
}

//!\brief   获取播放方向
//!\param   hPlayer         播放器句柄
//!\param   pnDirection     返回方向，0表示正向，1表示反向
//!\return  0表示成功，其它为错误码
int MPU_AVP_GetDirection(HANDLE hPlayer, int *pnDirection)
{
	MPU_AVP_CALL(GetDirection(pnDirection));
}

//!\brief   设置播放速度
//!\param   hPlayer         播放器句柄
//!\param   nSpeed          速度
//!\return  0表示成功，其它为错误码
int MPU_AVP_SetSpeed(HANDLE hPlayer, int nSpeed)
{
	MPU_AVP_CALL(SetSpeed(nSpeed));
}

//!\brief   获取播放速度
//!\param   hPlayer             播放器句柄
//!\param   pnSpeed             返回速度
//!\return  0表示成功，其它为错误码
int MPU_AVP_GetSpeed(HANDLE hPlayer, int *pnSpeed)
{
	MPU_AVP_CALL(GetSpeed(pnSpeed));
}

//!\brief   设置音量
//!\param   hPlayer             播放器句柄
//!\param   nVolume             音量[0-100]
//!\return  0表示成功，其它为错误码
int MPU_AVP_SetVolume(HANDLE hPlayer, int nVolume)
{
	MPU_AVP_CALL(SetVolume(nVolume));
}

//!\brief   获取音量
//!\param   hPlayer             播放器句柄
//!\param   pnVolume            返回音量[0-100]
//!\return  0表示成功，其它为错误码
int MPU_AVP_GetVolume(HANDLE hPlayer, int *pnVolume)
{
	MPU_AVP_CALL(GetVolume(pnVolume));
}

//!\brief   设置播放模式
//!\param   hPlayer             播放器句柄
//!\param   nMode               0表示正常模式，1表示浏览模式
//!\return  0表示成功，其它为错误码
int MPU_AVP_SetMode(HANDLE hPlayer, int nMode)
{
	MPU_AVP_CALL(SetMode(nMode));
}

//!\brief   获取播放模式
//!\param   hPlayer             播放器句柄
//!\param   pnMode              0表示正常模式，1表示浏览模式
//!\return  0表示成功，其它为错误码
int MPU_AVP_GetMode(HANDLE hPlayer, int *pnMode)
{
	MPU_AVP_CALL(GetMode(pnMode));
}

//!\brief   设置浏览模式间隔时间
//!\param   hPlayer             播放器句柄
//!\param   tStepSecond         间隔时间，单位为秒
//!\return  0表示成功，其它为错误码
int MPU_AVP_SetBrowseStepTime(HANDLE hPlayer, time_t tStepSecond)
{
	MPU_AVP_CALL(SetBrowseStepTime(tStepSecond));
}

//!\brief   获取浏览模式间隔时间
//!\param   hPlayer             播放器句柄
//!\param   ptStepSecond        返回间隔时间，单位为秒
//!\return  0表示成功，其它为错误码
int MPU_AVP_GetBrowseStepTime(HANDLE hPlayer, time_t * ptStepSecond)
{
	MPU_AVP_CALL(GetBrowseStepTime(ptStepSecond));
}

//!\brief   设置浏览速度
//!\param   hPlayer         播放器句柄
//!\param   nSpeed         浏览速度
//!\return  0表示成功，其它为错误码
int MPU_AVP_SetBrowseSpeed(HANDLE hPlayer, int nBrowseSpeed)
{
	MPU_AVP_CALL(SetBrowseSpeed(nBrowseSpeed));
}

//!\brief   获取浏览速度
//!\param   hPlayer             播放器句柄
//!\param   pnSpeed           返回浏览速度
//!\return  0表示成功，其它为错误码
int MPU_AVP_GetBrowseSpeed(HANDLE hPlayer, int *pnBrowseSpeed)
{
	MPU_AVP_CALL(GetBrowseSpeed(pnBrowseSpeed));
}

//!\brief   抓拍快照，抓拍当前显示的所有通道，并自动存盘
//!\param   hPlayer             播放器句柄
//!\param   nDecID      解码器ID
//!\return  0表示成功，其它为错误码
int MPU_AVP_DoSnapshot(HANDLE hPlayer, int nDecID)
{
	MPU_AVP_CALL(DoSnapshot(nDecID));
}

//!\brief       进入拖放模式
//!\param   hPlayer 播放器句柄
//!\return  0表示成功，其它为错误码
int MPU_AVP_StartDrag(HANDLE hPlayer)
{
	MPU_AVP_CALL(StartDrag());
}

//!\brief       退出拖放模式
//!\param   hPlayer 播放器句柄
//!\return  0表示成功，其它为错误码
int MPU_AVP_StopDrag(HANDLE hPlayer)
{
	MPU_AVP_CALL(StopDrag());
}


/**************************************************************************
 *                            注销局部宏定义                              *
 **************************************************************************/
#undef	FILEPLAYER_HANDLE		//文件播放器句柄
#undef	STREAMPLAYER_HANDLE		//流播放器句柄

#undef	IS_FILEPLAYER			//判断是否文件播放器
#undef	IS_STREAMPLAYER			//判断是否流播放器
#undef	MPU_AVP_CALL			//调用播放器执行动作
