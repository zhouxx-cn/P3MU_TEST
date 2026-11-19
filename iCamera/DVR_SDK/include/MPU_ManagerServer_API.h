
/*!
 *\file		MPU_ManagerServer_API.h
 *\brief		MPU模块，中心管理服务器相关接口定义
 *\author	林云
 *\date		2009/03/20

功能：
-#	中心服务器参数管理（是否启用、IP地址、端口、自动注册时间间隔）
 */


#ifndef _INC_MPU_MANAGERSERVER_API_H_
#define _INC_MPU_MANAGERSERVER_API_H_


//!\brief      获取中心管理服务器的工作模式
//!\return   0为普通工作模式，1为高级工作模式，失败为-1
int MPU_NMS_GetWorkMode();

//!\brief       设置中心管理服务器
//!\param   nWorkMode   0为普通工作模式，1为高级工作模式，失败为-1
//!\return  成功为0，失败为-1
int MPU_NMS_SetWorkMode(int nWorkMode);

//!\brief   获取是否启用中心管理服务器
//!\return  TRUE表示启用，FALSE表示不启用
BOOL MPU_NMS_GetEnable();

//!\brief   设置是否启用中心管理服务器
//!\param   bEnable     TRUE表示启用，FALSE表示不启用
//!\return  0表示成功，其它为错误码
int MPU_NMS_SetEnable(BOOL bEnable);

//!\brief   获取中心管理服务器IP地址
//!\param   pszServerIP 返回中心管理服务器IP地址
//!\return  本接口拷贝中心管理服务器IP地址到pszServerIP指向的buf，并返回pszServerIP
char *MPU_NMS_GetServerIP(char *pszServerIP, int serverIpSize);

//!\brief   设置中心管理服务器IP地址
//!\param   pszServerIP 中心管理服务器IP地址
//!\return  0表示成功，其它为错误码
int MPU_NMS_SetServerIP(const char *pszServerIP);

//!\brief   获取中心管理服务器端口
//!\return  端口号(0-65535)
unsigned short MPU_NMS_GetServerPort();

//!\brief   设置中心管理服务器端口
//!\param   usPort  端口号(0-65535)
//!\return  0表示成功，其它为错误码
int MPU_NMS_SetServerPort(unsigned short usPort);

//!\brief   获取自动注册中心服务器间隔时间
//!\return  间隔时间，单位为秒
int MPU_NMS_GetAutoRegisterTime();

//!\brief   设置自动注册中心服务器间隔时间
//!\param   nIntervalSecond     间隔时间，单位为秒
//!\return  0表示成功，其它为错误码
int MPU_NMS_SetAutoRegisterTime(int nIntervalSecond);

//!\brief   获取通讯协议
//!\return  0表示禁用，1表示互信互通
int MPU_NMS_GetProtocol();

//!\brief   设置通讯协议
//!\param   nProtocol       0表示禁用，1表示互信互通
//!\return  0表示不NAT背后，1表示在NAT背后
int MPU_NMS_SetProtocol(int nProtocol);

//!\brief   获取互信互通是否有NAT服务
//!\return  0表示禁用，1表示互信互通
int MPU_HXHT_GetNAT();

//!\brief   设置互信互通是否有NAT服务
//!\param   bEnable     0表示不NAT背后，1表示在NAT背后
//!\return  0表示成功，其它为错误码
int MPU_HXHT_SetNAT(int bEnable);

//!\brief   获取互信互通存储IP地址
//!\param   pszServerIP 返回中心管理服务器IP地址
//!\param   nBufSize    pszServerIP指向的buf大小
//!\return  本接口拷贝中心管理服务器IP地址到pszServerIP指向的buf，并返回pszServerIP
char *MPU_HXHT_GetStorageServerIP(char *pszServerIP, int nBufSize);

//!\brief   设置互信互通存储IP地址
//!\param   pszServerIP 中心管理服务器IP地址
//!\return  0表示成功，其它为错误码
int MPU_HXHT_SetStorageServerIP(const char *pszServerIP);

//!\brief   获取互信互通存储端口
//!\return  端口号(0-65535)
unsigned short MPU_HXHT_GetStorageServerPort();

//!\brief   设置互信互通存储服务器端口
//!\param   usPort  端口号(0-65535)
//!\return  0表示成功，其它为错误码
int MPU_HXHT_SetStorageServerPort(unsigned short usPort);

//!\brief   获取互信互通前端设备ID
//!\param   pszDeviceID 返回前端设备ID
//!\param   nBufSize    pszDeviceID指向的buf大小
//!\return  本接口拷贝前端设备ID到pszDeviceID指向的buf，并返回pszDeviceID
char *MPU_HXHT_GetDeviceID(char *pszDeviceID, int nBufSize);

//!\brief   设置互信互通前端设备ID
//!\param   pszDeviceID 前端设备ID
//!\return  0表示成功，其它为错误码
int MPU_HXHT_SetDeviceID(const char *pszDeviceID);

#endif							/* _INC_MPU_MANAGERSERVER_API_H_ */
