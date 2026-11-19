// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_RECT_49AB72A10039_INCLUDED
#define _INC_RECT_49AB72A10039_INCLUDED

//typedef struct tag_Rect RECT;
//##ModelId=49AB72A10039
typedef struct
{

	//!\brief 矩形左上角x坐标
	int left;
	//!\brief 矩形左上角y坐标
	int top;
	//!\brief 矩形右下角x坐标
	int right;
	//!\brief 矩形右下角y坐标
	int bottom;

} RECT, *PRECT;

/**
 * \var typedef RECT* PRECT
 * \brief Data type of the pointer to a RECT.
 *
 * \sa RECT
 */
typedef RECT *PRECT;

#endif							/* _INC_RECT_49AB72A10039_INCLUDED */
