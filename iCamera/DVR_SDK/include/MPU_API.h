
/*!
 *\file		MPU_API.h
 *\brief	MPU模块，接口定义
 *\author	林云
 *\date		2009/03/20

说明：
-#	MPU为上层GUI访问SDK的唯一入口。
-#	MPU只向上层GUI提供必须的接口，其它不需要上层GUI干预的功能由MPU内部自己调度。
 */


#ifndef _INC_MPU_API_H_
#define _INC_MPU_API_H_


//!\brief   MPU模块初始化
//!\return  成功为0，其它为错误码
int MPU_Init();

//!\brief   MPU模块初始化，第二阶段
//!\return  成功为0，其它为错误码
int MPU_Init2();


//!\brief   MPU模块逆初始化
//!\return  成功为0，其它为错误码
int MPU_Uninit();

//!\brief   MPU模块清除报警
//!\return  成功为0，其它为错误码
int MPU_ClearAlarm();

//!\brief       设置异常处理是否显示桌面图标
//!\param   nExceptionType  异常类型
//!\param   bEnable         是否处理
//!\return  0表示成功，其它为错误码
int MPU_Exception_EnableDesktopIcon(int nExceptionType, int bEnable);

//!\brief       获取异常处理是否显示桌面图标
//!\param   nExceptionType
//!\return  返回是否处理
int MPU_Exception_IsEnableDesktopIcon(int nExceptionType);


//公共基础相关API定义
#include "MPU_PUB_API.h"

//网络管理相关API定义
#include "MPU_CIU_API.h"

//报警输出相关API定义
#include "MPU_DOP_API.h"

//报警输入相关API定义
#include "MPU_DIP_API.h"

//录像备份相关API定义
#include "MPU_BAK_API.h"

//参数管理相关API定义
#include "MPU_PAR_API.h"

//操作日志相关API定义
#include "MPU_OPL_API.h"

//磁盘管理相关API定义
#include "MPU_STR_API.h"

//回放相关API定义
#include "MPU_AVP_API.h"

//预览相关API定义
#include "MPU_PRW_API.h"

//通道管理相关API定义
#include "MPU_AVL_API.h"

//系统设备管理相关API定义
#include "MPU_SYS_API.h"

//用户管理相关API定义
#include "MPU_USR_API.h"

//云台控制相关API定义
#include "MPU_PTZ_API.h"

//本地网络参数管理相关API定义
#include "MPU_Net_API.h"

//DDNS服务相关API定义
#include "MPU_DDNS_API.h"

//PPPoE服务相关API定义
#include "MPU_PPPoE_API.h"

//管理服务器相关API定义
#include "MPU_ManagerServer_API.h"

//报警服务器相关API定义
#include "MPU_AlarmServer_API.h"

//NTP服务器相关API定义
#include "MPU_NTP_API.h"

//Email服务器相关API定义
#include "MPU_Email_API.h"

//云升级相关API定义
#include "MPU_CloudUpgrade_API.h"

#if defined(__DIGITAL_ZOOM_DRAG_SUPPORT__)
//变倍相关API定义
#include "MPU_ZOOM_API.h"
#endif


#endif							/* _INC_MPU_API_H_ */
