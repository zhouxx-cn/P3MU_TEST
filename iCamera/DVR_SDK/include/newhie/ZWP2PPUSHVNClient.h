/*!
* @file
*  ZWP2PVNClient.h
* @brief
*  客户端SDK头文件, 定义SDK所有接口, 并附简要接口说明
*  客户端SDK是与服务器交互的最佳途径
* @author
*  zw
* @date
*  2017年7月29日
* @version
*  1.2.0
*/

#ifndef __ZWP2P_PUSH_VNCLIENT_H__

#define __ZWP2P_PUSH_VNCLIENT_H__

#ifdef ZWP2P_PUSH_VNCLIENT_EXPORTS

#define ZWP2P_PUSH_VNCLIENT_API	extern "C"__declspec(dllexport)
#define CALLSTACK		__stdcall

#elif defined(__cplusplus)

#ifdef SYSTEM_WIN32

#define ZWP2P_PUSH_VNCLIENT_API	extern "C"__declspec(dllimport)
#ifndef CALLSTACK
#define CALLSTACK		__stdcall
#endif

#else

#define ZWP2P_PUSH_VNCLIENT_API	extern "C"
#ifndef CALLSTACK
#define CALLSTACK
#endif

#endif

#else

#define ZWP2P_PUSH_VNCLIENT_API
#define CALLSTACK

#endif

//通用信息定义文件
#include "./ZWP2PPUSHVNClient_Common.h"

/*!
* @brief
*	启动服务
* @param pInitPushParameters
*	初始化信息
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	启动服务后,不再需要服务时请调用停止服务接口
* @note
*	接口类型: 阻塞式
*/
ZWP2P_PUSH_VNCLIENT_API ZWP2P_UINT32 CALLSTACK ZWP2PPUSHVNClient_Initialize(LPZWP2P_PUSH_INIT_PUSH_PARAMETERS pInitPushParameters);

/*!
* @brief
*	停止服务
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
ZWP2P_PUSH_VNCLIENT_API ZWP2P_UINT32 CALLSTACK ZWP2PPUSHVNClient_Deinitialize();

/*!
* @brief
*	设置自定义的AuthWord
* @param pInitPushParameters
*	具体设置的字符串
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	
* @note
*	接口类型: 阻塞式
*/
ZWP2P_PUSH_VNCLIENT_API ZWP2P_UINT32 CALLSTACK ZWP2PPUSHVNClient_SetCustomAuthWord(IN char * pCustomAuthWord);

/*!
* @brief
*	通过DID来获取服务器支持的推送功能情况，该接口一定要保证在ZWP2PPUSHVNClient_Initialize之后调用，且一定要判断返回值，不然得到的值是会有问题的
* @param pDeviceDID
*	设备DeviceID
* @return
*	返回0表示成功, 
* @note
*	接口类型: 阻塞式
*/
ZWP2P_PUSH_VNCLIENT_API ZWP2P_UINT32 CALLSTACK ZWP2PPUSHVNClient_IsSupportSetSubkeyByDID(IN char * pDeviceDID);
/*!
* @brief
*	推送事件
* @param nEventType
*	参考枚举eDeviceEventType
* @param nChannel
*	通道(流通道或者报警通道),当推送事件的类型与通道没有关系时，该参数无效，当推送事件的类型与通道有关系时才有效
* @param pEventInfo
*	推送协议需要的具体参数
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
ZWP2P_PUSH_VNCLIENT_API ZWP2P_UINT32 CALLSTACK ZWP2PPUSHVNClient_PushEvent(ZWP2P_UINT32 nEventType,ZWP2P_UINT32 nChannel,LPZWP2P_PUSH_EVENT_INFO pEventInfo);

/*!
* @brief
*	推送事件
* @param nEventType
*	参考枚举eDeviceEventType
* @param nChannel
*	通道(流通道或者报警通道),当推送事件的类型与通道没有关系时，该参数无效，当推送事件的类型与通道有关系时才有效
* @param pEventInfo
*	推送协议需要的具体参数
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
ZWP2P_PUSH_VNCLIENT_API ZWP2P_UINT32 CALLSTACK ZWP2PPUSHVNClient_PushEvent_V2(ZWP2P_UINT32 nEventType,ZWP2P_UINT32 nChannel,LPZWP2P_PUSH_EVENT_INFO pEventInfo);

/*!
* @brief
*	推送事件(V2结构体大小不够了，加的V3接口，使用方法跟V2一样)
* @param nEventType
*	参考枚举eDeviceEventType
* @param nChannel
*	通道(流通道或者报警通道),当推送事件的类型与通道没有关系时，该参数无效，当推送事件的类型与通道有关系时才有效
* @param pEventInfo
*	推送协议需要的具体参数
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
ZWP2P_PUSH_VNCLIENT_API ZWP2P_UINT32 CALLSTACK ZWP2PPUSHVNClient_PushEvent_V3(ZWP2P_UINT32 nEventType,ZWP2P_UINT32 nChannel,LPZWP2P_PUSH_EVENT_INFO_EX pEventInfo);


/*!
* @brief
*	触发推送使能
* @param bEnable
*	false,表示关闭推送，true，表示打开推送
* @param nChannel
*  当订阅只与设备有关系时，那么该参数无效，与设备的通道有关系是，那么该参数有效，目前订阅只与设备有关系
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
ZWP2P_PUSH_VNCLIENT_API ZWP2P_UINT32 CALLSTACK ZWP2PPUSHVNClient_TriggerPushEnable(ZWP2P_UINT32 bEnable,ZWP2P_UINT32 nChannel);


/*!
* @brief
*	初始化subkey的初始化参数，如果设置了subkey的相关参数，那么该库会按照有subkey的逻辑来处理，如果没有那么按照没有subkey处理，都可以正常推送
*   该接口保证在ZWP2PPUSHVNClient_Initialize接口之前调用
* @param pSubkeyParams
*	参考结构体LPZWP2P_PUSH_SUBKEY_INPUT_PARAMS
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
ZWP2P_PUSH_VNCLIENT_API ZWP2P_UINT32 CALLSTACK ZWP2PPUSHVNClient_InitSubkeyParam(LPZWP2P_PUSH_SUBKEY_INPUT_PARAMS pSubkeyParams);


/*!
* @brief
*	获取subkey信息
*   该接口保证在ZWP2PPUSHVNClient_Initialize接口之后调用才有意义
* @param cSubkey
*	存储subkey的存储空间
* @param nSubkeyLen
*	上层提供的存储subkey的长度
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
ZWP2P_PUSH_VNCLIENT_API ZWP2P_UINT32 CALLSTACK ZWP2PPUSHVNClient_GetSubkey(char * cSubkey,int nSubkeyLen);

/*!
* @brief
*   该接口设置事件类别的属性
* @param eEventType
*	事件类别
* @param struEventAttribute
*	事件类型的属性
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
ZWP2P_PUSH_VNCLIENT_API ZWP2P_UINT32 CALLSTACK ZWP2PPUSHVNClient_SetCategoryAttribute(eDeviceEventCategory eEventType,ZWP2P_EVENT_ATTRIBUTE struEventAttribute);

/*!
* @brief
*   该接口设置推送库配置
* @param pushEventParams
*	推送库的配置
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
ZWP2P_PUSH_VNCLIENT_API ZWP2P_UINT32 CALLSTACK ZWP2PPUSHVNClient_SetPushEventParam(LPZWP2P_PUSH_EVENT_PARAMS pushEventParams);


/*!
* @brief
*   该接口设置事件类型的属性
* @param eEventType
*	事件类型
* @param struEventAttribute
*	事件类型的属性
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
ZWP2P_PUSH_VNCLIENT_API ZWP2P_UINT32 CALLSTACK ZWP2PPUSHVNClient_SetAttribute(eDeviceEventType eEventType,ZWP2P_EVENT_ATTRIBUTE struEventAttribute);

#if !defined(USE_C_LANGUAGE)
/*!
* @brief
*   该接口获取事件类型的属性
* @param eEventType
*	事件类型
* @param struEventAttribute
*	事件类型的属性
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
ZWP2P_PUSH_VNCLIENT_API ZWP2P_UINT32 CALLSTACK ZWP2PPUSHVNClient_GetAttribute(eDeviceEventType eEventType,ZWP2P_EVENT_ATTRIBUTE& struEventAttribute);
#else
/*!
* @brief
*   该接口获取事件类型的属性
* @param eEventType
*	事件类型
* @param struEventAttribute
*	事件类型的属性
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
ZWP2P_PUSH_VNCLIENT_API ZWP2P_UINT32 CALLSTACK ZWP2PPUSHVNClient_GetAttribute(eDeviceEventType eEventType,ZWP2P_EVENT_ATTRIBUTE * pstruEventAttribute);
#endif


/*!
* @brief
*   设置域名列表
* @param *ppDomainList
*	域名列表
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
ZWP2P_PUSH_VNCLIENT_API ZWP2P_UINT32 CALLSTACK ZWP2PPUSHVNClient_SetDomainList(ZWPush_DOMAIN_INFO_LIST * pDomainList);

/*!
* @brief
*   设置推送服务器
* @param *pDomainParams
*	服务器地址
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
    1.	该接口和 ZWP2PPUSHVNClient_SetDomainList 及内部向服务器获取域名逻辑互斥，
    2.	该接口设置之后，推送服务器就是自己按域名规则自动生成（在该接口中完成），
    3.	如果ZWP2PPUSHVNClient_SetDomainList这个接口调用的话，表示域名是有外部设置，
    4.	两个接口都不调用，则内部向服务器获取，
    5.  该接口和 ZWP2PPUSHVNClient_SetDomainList 接口谁后调用则谁生效。
*/
//除了支持高优先级推送需要设置域名外，BOTE,BOTC，YLTB(除了YLTBAA外)都需要调用该接口
ZWP2P_PUSH_VNCLIENT_API ZWP2P_UINT32 CALLSTACK ZWP2PPUSHVNClient_SetDomainParam(ZWP2P_PUSH_DOMAIN_PARAMS* pDomainParams);

/*!
* @brief
*   清空配置文件内容
* @param *pDomainParams
*	服务器地址
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
    1. 清空之前 ZWP2PPUSHVNClient_SetDomainParam 接口设置的服务器域名相关信息
*/
ZWP2P_PUSH_VNCLIENT_API ZWP2P_UINT32 CALLSTACK ZWP2PPUSHVNClient_CleanConfigFile();

//打断尚云接口
ZWP2P_PUSH_VNCLIENT_API ZWP2P_UINT32 CALLSTACK ZWP2PPUSHVNClient_SPS_Break();

/*!
* @brief
*   设置 P6SMSG 服务器参数
* @param *pServerInfo
*	服务器参数
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
ZWP2P_PUSH_VNCLIENT_API ZWP2P_UINT32 CALLSTACK ZWP2PPUSHVNClient_SetP6SMSGServerInfo(ZWPush_P6SMSG_SERVER_INFO ServerInfo);

/*!
* @brief
*   设置 P6SMSG 服务器地址
* @param *url
*	服务器地址
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
ZWP2P_PUSH_VNCLIENT_API ZWP2P_UINT32 CALLSTACK ZWP2PPUSHVNClient_SetP6SMSGServerURL(char* pServerURL);

/*!
* @brief
*   设置 P6SMSG 消息推送策略参数
* @param *pServerInfo
*	服务器参数
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
ZWP2P_PUSH_VNCLIENT_API ZWP2P_UINT32 CALLSTACK ZWP2PPUSHVNClient_SetP6SMSGPushPolicy(ZWP2P_PUSH_P6SMSG_POLICY_PARAMS policy);

/*!
* @brief
*   设置 P6SMSG 消息推送策略参数
* @param *pServerInfo
*	服务器参数
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
ZWP2P_PUSH_VNCLIENT_API ZWP2P_UINT32 CALLSTACK ZWP2PPUSHVNClient_SetP6SMSGPushPolicy_V2(ZWP2P_PUSH_P6SMSG_POLICY_PARAMS_V2 policy);

/*!
* @brief
*   设置 P6SMSG 向服务器请求获取到的策略
* @param statics
*	策略相关参数
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
ZWP2P_PUSH_VNCLIENT_API ZWP2P_UINT32 CALLSTACK ZWP2PPUSHVNClient_SetP6SMSGServerStatics(ZWP2P_PUSH_P6SMSG_SERVER_STATICS statics);


/*!
* @brief
*   设置获取网络状态回调
* @param IF_GetNetworkStatus
*	回调函数
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
#if !defined(USE_C_LANGUAGE)
ZWP2P_PUSH_VNCLIENT_API ZWP2P_UINT32 CALLSTACK ZWP2PPUSHVNClient_SetNetworkStatusCallback(IF_GetNetworkStatus getNetworkStatus);
#else
ZWP2P_PUSH_VNCLIENT_API ZWP2P_UINT32 CALLSTACK ZWP2PPUSHVNClient_SetNetworkStatusCallback(IF_GetNetworkStatus getNetworkStatus);
#endif


/*!
* @brief
* 该接口设置云消息配置，需要在初始化接口之前设置（相当于云消息的初始化接口）
* @param P6SMSGParams
* 云消息的配置
* @return
* 返回0表示成功, 否则表示错误码
* @note
* 接口类型: 阻塞式
*/
ZWP2P_PUSH_VNCLIENT_API ZWP2P_UINT32 CALLSTACK ZWP2PPUSHVNClient_SetP6SMSGParam(ZWP2P_PUSH_P6SMSG_PARAM P6SMSGParams);


/*!
* @brief
*   发送云消息（调用此接口时需要将原推送事件接口参数中的 nSendMode 置为 2 ，表示只发送尚云推送）
* @param pEventInfo
*	推送协议需要的具体参数
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
ZWP2P_PUSH_VNCLIENT_API ZWP2P_UINT32 CALLSTACK ZWP2PPUSHVNClient_P6SMsgPushEvent(LPZWP2P_PUSH_P6SMSG_EVENT_INFO pEventInfo);


/*!
* @brief
*   设置事件状态回调
* @param pCallback
*	回调函数
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
ZWP2P_PUSH_VNCLIENT_API ZWP2P_UINT32 CALLSTACK ZWP2PPUSHVNClient_SetP6SMsgCallback(ZWP2P_PUSH_P6SMSG_CALLBACK pCallback);

#endif
