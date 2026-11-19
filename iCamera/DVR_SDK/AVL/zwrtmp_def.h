#ifndef __ZW_RTMP_H__
#define __ZW_RTMP_H__

typedef enum eNetClientRetCode
{
	eRetSuccess = 0,						// 成功
	eRetFailed,								// 失败
	eRetFailVersion,						// 版本不匹配
	eRetFailBoardType,						// 板型不匹配，无法升级

	eRetFailSdkNotInit,						// 客户端sdk未初始化
	eRetFailUnsupported,                    // 不支持的操作
	eRetFailHasStarted,						// 已经启动
	eRetFailNotStart,						// 未启动

	eRetFailInvalidParameter,				// 参数无效
	eRetFailInvalidHandle,					// 句柄值无效
	eRetFailInvalidFile,					// 文件无效
	eRetFailNoResource,						// 资源不足

	eRetFailUserName,						// 用户名错误，预留
	eRetFailUserPwd,						// 用户密码错误，预留

	eRetFailNetworkError,					// 网络错误
	eRetFailCmdTimeout,						// 命令超时
	eRetFailDisconnect,						// 连接断开
	
	eRetFailNotAllowed,						// 服务器不允许
	eRetUnsupportedCodec,					// 不支持的编码格式
}eNetClientRetCode;


// 连接参数
typedef struct 
{
	int  CacheQSizeKB;
	char szStreamPullingAddress[512];
	char szUserName[64];
	char szPassword[64];
	int  bPushStreamURLRetrieveFromServer;
	char szCustomerServer[256];
	char szDeviceSerialID[64];
	char szPlayURL[256];
	char szReseved[444];
}ZW_RTMP_CLIENT_CONNECT_PARAM;

// 状态定义
typedef enum
{
	eZWRTMP_STATUS_Begin,
	eZWRTMP_STATUS_Connecting,			// 正在连接
	eZWRTMP_STATUS_ConnectFail,			// 正在连接
	eZWRTMP_STATUS_UserPswFail,			// 用户名或密码错误
	eZWRTMP_STATUS_Connected,			// 连接成功
	eZWRTMP_STATUS_Disconnected,		// 断开连接
	eZWRTMP_STATUS_Reconnected,			// 重连成功
	eZWRTMP_STATUS_Breaking,			// 正在断开
	eZWRTMP_STATUS_ConnectTimeout,		// 连接超时
	eZWRTMP_STATUS_NotOpen,				// 未启用
	eZWRTMP_STATUS_NotActived,			// 未激活
	eZWRTMP_STATUS_NotAllowed,			// 服务器不允许,获取推流地址时拒绝
	eZWRTMP_STATUS_UnsupportedCodec		// 不支持的编码格式
}eZWRTMP_STATUS;

//  状态回调
typedef void(*ZWRTMP_CB_Working_Status)(unsigned int handle, eZWRTMP_STATUS Status, unsigned long dwData, unsigned long dwUserData);

#endif