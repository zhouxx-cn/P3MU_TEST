
#ifndef __CBVIDEOPRIVIEW_H_
#define __CBVIDEOPRIVIEW_H_

#include "RECT.h"

typedef struct tagPreviewLayout
{
	int is_alert;
	int is_poll;
	int count;
	int *ch;
	RECT *rect;
} PREVIEW_LAYOUT, LP_PREVIEW_LAYOUT;

//!\brief 某个通道需要在屏幕上显示的回调
//!\param bAuxMinor 主显/辅显
//!\param nCh 通道名
//!\param rect 显示区域坐标
//!\param nAction 0表示打开预览, 1表示关闭预览, 2表示异常, 3表示预览状态发生变化（报警联动,轮巡）
//!\ nAction = 3; nCh = (LP_PREVIEW_LAYOUT)pLayout
//!\return 0表示正常,-1表示出现错误
typedef int (*VP_Callback) (const int bAuxMinor, const int nCh, const RECT & rect, const int nAction);

#endif
