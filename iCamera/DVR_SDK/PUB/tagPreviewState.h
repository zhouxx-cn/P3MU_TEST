
/*!
*\file   tagPreviewState.h
*\brief  预览状态枚举定义
*\author yuk@higheasyrd.com
*\date	  2009/04/08
*\version 0.1
*/

#ifndef __TAG_PREVIEWSTATE_H_
#define __TAG_PREVIEWSTATE_H_

//!\预览状态
typedef enum tagPreviewState
{
	//!\brief 无轮巡,无报警,静止画面
	PREVIEW_STATIC = 0,

	//!\brief 有自定义轮巡
	//PREVIEW_CUSTOMPOLL,
	//!\brief 有自动轮巡
	PREVIEW_POLL = 1,

	//!\brief 报警画面
	PREVIEW_ALERT = 2,

	PREVIEW_STOP = 3
} PREVIEW_STATE;

#endif
