
/*!
 *\file		MPU_AlarmServer_API.h
 *\brief	MPU模块，报警服务器相关接口定义
 *\author	林云
 *\date		2009/03/20

功能：
-#	报警服务器参数配置（使能、IP地址、端口）
 */

#ifndef _INC_MPU_ALARMSERVER_API_H_
#define _INC_MPU_ALARMSERVER_API_H_

//!\brief      获取报警服务器的工作模式
//!\return   0为普通工作模式，1为高级工作模式，失败为-1
int MPU_AMS_GetWorkMode();

//!\brief       设置报警服务器
//!\param   nWorkMode   0为普通工作模式，1为高级工作模式，失败为-1
//!\return  成功为0，失败为-1
int MPU_AMS_SetWorkMode(int nWorkMode);

//!\fn      BOOL MPU_AMS_GetEnable()
//!\brief   获取是否启用报警管理服务器
//!\return  TRUE表示启用，FALSE表示不启用
BOOL MPU_AMS_GetEnable();

//!\fn      int MPU_AMS_SetEnable(BOOL bEnable)
//!\brief   设置是否启用报警管理服务器
//!\param   bEnable     TRUE表示启用，FALSE表示不启用
//!\return  0表示成功，其它为错误码
int MPU_AMS_SetEnable(BOOL bEnable);

//!\fn      char *MPU_AMS_GetServerIP(char *pszServerIP)
//!\brief   获取报警管理服务器IP地址
//!\param   pszServerIP 返回报警管理服务器IP地址，buf大小不得小于16字节
//!\return  本接口拷贝报警管理服务器IP地址到pszServerIP指向的buf，并返回pszServerIP
char *MPU_AMS_GetServerIP(char *pszServerIP);

//!\fn      int MPU_AMS_SetServerIP(const char *pszServerIP)
//!\brief   设置报警管理服务器IP地址
//!\param   pszServerIP 报警管理服务器IP地址
//!\return  0表示成功，其它为错误码
int MPU_AMS_SetServerIP(const char *pszServerIP);

//!\fn      unsigned short MPU_AMS_GetServerPort()
//!\brief   获取报警管理服务器端口
//!\return  端口号(0-65535)
unsigned short MPU_AMS_GetServerPort();

//!\fn      int MPU_AMS_SetServerPort(unsigned short usPort)
//!\brief   设置报警管理服务器端口
//!\param   usPort  端口号(0-65535)
//!\return  0表示成功，其它为错误码
int MPU_AMS_SetServerPort(unsigned short usPort);


#endif							/* _INC_MPU_ALARMSERVER_API_H_ */
