
/*!
 *\file			CBDisk.h
 *\brief			硬盘状态通知回调函数类型定义
 *\author		林云
 *\date			2009/05/21
 *\version		1.0.0

 */


#ifndef _INC_CBDISK_H_
#define _INC_CBDISK_H_


//!\brief       发现新硬盘回调通知
//!\param   nBusNo  新硬盘物理编号
//!\return  返回0表示可以采用默认初始化，其他表示不允许初始化
typedef int (*NEWDISKCALLBACK) (int nBusNo);


//!\brief       硬盘热插拔时间回调通知
//!\param   pszDev      硬盘名称
//!\param   bAdd        true表示添加，false表示删除
//!\return  返回0表示可以采用默认初始化，其他表示不允许初始化
typedef void (*DISKSTATUSCALLBACK) (char *pszDev, bool bAdd);


#endif							/* #ifndef _INC_CBDISK_H_ */
