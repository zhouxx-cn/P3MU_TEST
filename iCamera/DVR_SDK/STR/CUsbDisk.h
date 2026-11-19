
/*!
*\file   CUsbDisk.h 
*\brief  移动硬盘操作声明
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CUSBDISK_49A7459F019B_INCLUDED
#define _INC_CUSBDISK_49A7459F019B_INCLUDED

#include "CDisk.h"

//!\brief  
//可以选择性支持，如不支持则只有硬盘一类。目前可以按照不支持来
//做，但需要保留此架构，便于日后扩展
//##ModelId=49A7459F019B
class CUsbDisk:public CDisk
{
};

#endif							/* _INC_CUSBDISK_49A7459F019B_INCLUDED */
