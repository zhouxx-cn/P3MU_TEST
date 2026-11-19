#ifndef HIE_SERVER_H

//HieServer接口定义文件
#define HIE_SERVER_H

//HieServer通用信息定义文件
#include "./HieServer_Common.h"

#include "./HieServer_Cfg.h"

/*
#ifdef HIESERVER_EXPORTS
#define HIESERVER_API __declspec(dllexport)
#else
#define HIESERVER_API __declspec(dllimport)
#endif
*/
#if defined( __cplusplus)
#define HIESERVER_API  extern "C"
#else
#define HIESERVER_API
#endif

/*!
* @brief
*	设置配置信息操作接口
* @param pInterface
*	配置信息操作接口
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
HIESERVER_API int HieServer_ConfigureIf(const LPConfigureInterface pInterface);

/*!
* @brief
*	设置配置信息操作接口
* @param pInterface
*	配置信息操作接口
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
HIESERVER_API int HieServer_ConfigureIfV1(const LPConfigureInterfaceV1 pInterface);

/*!
* @brief
*	设置用户操作回调接口
* @param pInterface
*	用户操作接口
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
HIESERVER_API int HieServer_UserActionIf(const LPUserActionInterface pInterface);

/*!
* @brief
*	设置更新事件操作回调接口
* @param pInterface
*	更新事件操作接口
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
HIESERVER_API int HieServer_RefreshEventIf(const LPRefreshEventInterface pInterface);

/*!
* @brief
*	设置实时流捕获回调接口
* @param pInterface
*	实时流操作接口
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
HIESERVER_API int HieServer_StreamCaptureIf(const LPStreamCaptureInterface pInterface);

HIESERVER_API int HieServer_StreamAccessIf(const LPRealStreamAccessInterface pInterface);

/*!
* @brief
*	设置拉流方式来获取流数据
* @param pInterface
*	拉流方式接口信息
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
HIESERVER_API int HieServer_PullRealStreamIf(const LPPullRealStreamInterface pInterface);

/*!
* @brief
*	设置语音流操作回调接口
* @param pInterface
*	语音流操作接口
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
HIESERVER_API int HieServer_VoiceStreamIf(const LPVoiceStreamInterface pInterface);

/*!
* @brief
*	设置历史查询操作回调接口
* @param pInterface
*	历史查询操作接口
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
HIESERVER_API int HieServer_HistoryQueryIf(const LPHistoryQueryInterface pInterface);

/*!
* @brief
*	设置历史查询操作回调接口
* @param pInterface
*	历史查询操作接口
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
HIESERVER_API int HieServer_HistoryQueryMultiTypeIf(const LPHistoryQueryInterfaceMultiType pInterface);

/*!
* @brief
*	设置历史查询操作回调接口(将查询结果生成文件)
* @param pInterface
*	历史查询操作接口
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
HIESERVER_API int HieServer_HistoryQueryMultiTypeRSFileIf(const LPHistoryQueryInterfaceMultiTypeEx pInterface);

/*!
* @brief
*	设置文件查询操作回调接口
* @param pInterface
*	文件查询操作接口
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
HIESERVER_API int HieServer_FileQueryIf(const LPFileQueryInterface pInterface);

/*!
* @brief
*	设置日志查询操作回调接口
* @param pInterface
*	日志查询操作接口
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
HIESERVER_API int HieServer_LogQueryIf(const LPLogQueryInterface pInterface);

/*!
* @brief
*	设置日志查询操作V1回调接口
* @param pInterface
*	日志查询操作接口
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
HIESERVER_API int HieServer_LogQueryV1If(const LPLogQueryInterfaceV1 pInterface);

/*!
* @brief
*	设置历史通道操作回调接口
* @param pInterface
*	历史通道操作接口
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
HIESERVER_API int HieServer_HistoryChannelIf(const LPHistoryChannelInterface pInterface);

/*!
* @brief
*	设置历史通道操作回调接口
* @param pInterface
*	历史通道操作接口
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
HIESERVER_API int HieServer_HistoryChannelMultiTypeIf(const LPHistoryChannelInterfaceMultiType pInterface);


/*!
* @brief
*	设置历史通道操作回调接口
* @param pInterface
*	历史通道操作接口
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
HIESERVER_API int HieServer_HistoryChannelMultiTypeIfV1(const LPHistoryChannelInterfaceMultiTypeV1 pInterface);

/*!
* @brief
*	设置历史通道操作回调接口
* @param pInterface
*	历史通道操作接口
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
HIESERVER_API int HieServer_HistoryChannelMultiTypeIfV2(const LPHistoryChannelInterfaceMultiTypeV2 pInterface);

/*!
* @brief
*	设置透明通道回调接口
* @param pInterface
*	透明通道操作接口
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
HIESERVER_API int HieServer_TransparentChannelIf(const LPTransparentChanInterface pInterface);

/*!
* @brief
*	设置远程下载回调接口
* @param pInterface
*	远程下载接口信息
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
HIESERVER_API int HieServer_RemoteDownloadIf(const LPRemoteDownloadInterface pInterface);

/*!
* @brief
*	获取Buffer回调接口
* @param pInterface
*	获取Buffer接口信息
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
HIESERVER_API int HieServer_RemoteGetBufferIf(const LPRemoteGetBufferInterface pInterface);

/*!
* @brief
*	设置远程文件上传回调接口
* @param pInterface
*	远程文件上传接口信息
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
HIESERVER_API int HieServer_RemoteUploadIf(const LPRemoteUploadInterface pInterface);

/*!
* @brief
*	设置写log回调接口
* @param pInterface
*	写log回调接口
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
HIESERVER_API int HieServer_WriteLogFileIf(const LPWriteLogFileInterface pInterface);

/*!
* @brief
*	设置其它功能回调接口
* @param pInterface
*	其它功能接口信息
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
HIESERVER_API int HieServer_OtherFunctionIf(const LPOtherFunctionInterface pInterface);

/*!
* @brief
*	设置远程命令回调接口
* @param pInterface
*	远程命令接口信息
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
HIESERVER_API int HieServer_RemoteCommandIf(const LPRemoteCommandInterface pInterface);

/*!
* @brief
*	设置远程命令回调接口V1
* @param pInterface
*	远程命令接口信息
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
HIESERVER_API int HieServer_RemoteCommandIfV1(const LPRemoteCommandInterfaceV1 pInterface);

/*!
* @brief
*	设置远程硬盘分组管理接口
* @param pInterface
*	远程命令接口信息
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
HIESERVER_API int HieServer_DiskGroupManageIf(const LPDiskGroupManageInterface pInterface);

/*!
* @brief
*	设置远程硬盘分组管理接口
* @param pInterface
*	远程命令接口信息
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
HIESERVER_API int HieServer_DiskGroupManageMultiTypeIf(const LPDiskGroupManageInterfaceMultiType pInterface);

/*!
* @brief
*	设置视频通话流操作回调接口
* @param pInterface
*	视频通话流操作接口
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
HIESERVER_API int HieServer_VideoStreamIf(const LPVideoStreamInterface pInterface);

/*!
* @brief
*	启动服务
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	启动服务后将会读取配置信息,
*	所以启动前需要先 设置配置信息读写接口
* @note
*	接口类型: 阻塞式
*/
HIESERVER_API int HieServer_Start();

/*!
* @brief
*	启动服务(不加载私有协议)
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	启动服务后将会读取配置信息,
*	所以启动前需要先 设置配置信息读写接口
* @note
*	接口类型: 阻塞式
*/
HIESERVER_API int HieServer_Start_No_HY();

/*!
* @brief
*	停止服务,断开所有协议及远程连接
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
HIESERVER_API int HieServer_Stop();

/*!
* @brief
*	启用除私有协议之外的所有协议
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
HIESERVER_API int HieServer_AttachProtocol();

/*!
* @brief
*	启用指定的协议
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
HIESERVER_API int HieServer_LoadProtocol(int dwProtocol);

/*!
* @brief
*	停止服务,停用某个协议
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
HIESERVER_API int HieServer_DetachProtocol(int dwProtocol);

/*!
* @brief
*	更新事件通知
* @param dwEventType
*	事件类型
* @param dwParam1 -- dwParam3
*	报警事件：
*	参数1  报警类型
*	参数2  报警通道
*	参数3  报警状态(视频丢失时,0表示无信号,1表示有信号)，2表示开始事件，3表示持续事件，4表示结束事件
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 非阻塞式
*/
HIESERVER_API int HieServer_RefreshEvent(DWORD dwEventType, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3);

/*!
* @brief
*	更新事件通知
* @param dwEventType
*	事件类型
* @param dwParam1 -- dwParam3
*	报警事件：
*	参数1  报警类型
*	参数2  报警通道
*	参数3  报警状态(视频丢失时,0表示无信号,1表示有信号)，2表示开始事件，3表示持续事件，4表示结束事件
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 非阻塞式
*/
HIESERVER_API int HieServer_RefreshEventEx(DWORD dwEventType, DWORD dwSubEventType, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3, char * pBuffer, DWORD dwBufferLen);

/*!
* @brief
*	更新事件通知
* @param dwEventType
*	事件类型
* @param dwParam1 -- dwParam3
*	报警事件：
*	参数1  报警类型
*	参数2  报警通道
*	参数3  报警状态(视频丢失时,0表示无信号,1表示有信号)，2表示开始事件，3表示持续事件，4表示结束事件
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 非阻塞式
*/
HIESERVER_API int HieServer_RefreshEventExV2(DWORD dwProtocol, DWORD dwEventType, DWORD dwSubEventType, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3, char * pBuffer, DWORD dwBufferLen);

/*!
* @brief
*	获取状态
* @param eType
*	获取状态类型
* @param pBuffer
*	状态信息
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
HIESERVER_API int HieServer_GetState(eGetServerStateType eType, LPBuffer pBuffer);

/*!
* @brief
*	实时流媒体数据
* @param dwChannel
*	通道号
* @param pRealStreamMedia
*	实时流媒体
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 非阻塞式
*/
HIESERVER_API int HieServer_RealStreamMedia(DWORD dwChannel, eMediaCodeType eCodeType, const LPRealStreamMedia pRealStreamMedia);

/*!
* @brief
*	语音流媒体数据
* @param dwChannel
*	通道号
* @param pRealStreamMedia
*	实时流媒体
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 非阻塞式
*/
HIESERVER_API int HieServer_VoiceStreamMedia(DWORD dwChannel, const LPRealStreamMedia pVoiceStreamMedia);

/*!
* @brief
*	历史通道流媒体数据
* @param hIndex
*	历史通道句柄
* @param pHistoryChannelStream
*	历史通道流媒体
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式,阻塞时间可能较长
*/
HIESERVER_API int HieServer_HistoryChannelStream(HANDLE hIndex, const LPHistoryChannelStream pHistoryChannelStream);

/*!
* @brief
*	透明通道数据发送接口
* @param hIndex
*	透明通道句柄
* @param pTransparentParaInfo
*	透明通道数据信息
* @note
*	接口类型: 非阻塞
*/
HIESERVER_API int HieServer_TransparentChannelWrite(HANDLE hIndex, const LPTransparentParaInfo pTransparentParaInfo);

/*!
* @brief
*	设备注册启动
* @param pDeviceInfo
*	设备信息
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞
*/
HIESERVER_API int HieServer_DeviceRegisterStart();

/*!
* @brief
*	设备注册停止
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞
*/
HIESERVER_API int HieServer_DeviceRegisterStop();


/*!
* @brief
*	强制删除已登陆用户
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞
*/
HIESERVER_API int HieServer_DeleteUserForce(DWORD dwUserID);


/*!
* @brief
*	获取用户码率
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞
*/
#ifndef USE_LOAD_C_COMPILER
HIESERVER_API int HieServer_GetUserBitRate(DWORD dwUserId, DWORD & dwUserBitRate);
#else
HIESERVER_API int HieServer_GetUserBitRate(DWORD dwUserId, DWORD * pdwUserBitRate);
#endif


/*!
* @brief
*	触发报警时，通知接入服务器
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞
*/
HIESERVER_API int HieServer_AlarmOUTNotify(LPDevAlarmInfo pAlarmoutNotify);


/*!
* @brief
*	发生故障时，通知接入服务器
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞
*/
HIESERVER_API int HieServer_DeviceFaultNotify(LPDevFaultInfo pDevFaultInfo);

/*!
* @brief
*	设备探测启动
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞
*/
HIESERVER_API int HieServer_DeviceProbeStart();

/*!
* @brief
*	设备探测停止
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞
*/
HIESERVER_API int HieServer_DeviceProbeStop();

/*!
* @brief
*	设备面板控制
* @param pInterface
*	面板控制接口信息
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞
*/
HIESERVER_API int HieServer_PanelControl(const LPPanelControlInterface pInterface);

/*!
* @brief
*	设置流转发回调接口
* @param pInterface
*	流转发接口
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
//HIESERVER_API int HieServer_StreamTransferIF(const LPStreamTransferInterface pInterface);

/*!
* @brief
*	设置转发历史流查询回调接口
* @param pInterface
*	转发历史流查询接口
* @return
*	返回0表示成功, 否则表示错误码
* @note
*	接口类型: 阻塞式
*/
//HIESERVER_API int HieServer_TransmitHistoryStreamQueryIF(const LPTransmitHistoryStreamQueryInterface pInterface);

/*!
* @brief
*	获取协议加载的情况
* @param dwProtocolState
*	每一位都指定一个协议，0，表示协议不支持，1，表示协议支持（每一位指定的协议参见）
* @note
*	接口类型: 阻塞式
*/
#ifndef USE_LOAD_C_COMPILER
HIESERVER_API void HieServer_GetProtocolLoadState(DWORD& dwProtocolState);
#else
HIESERVER_API void HieServer_GetProtocolLoadState(DWORD * pdwProtocolState);
#endif

/*!
* @brief 	向协议获取配置信息
* @param dwProtocolID				[in]协议类型
* @param dwMajorCommand		[in]主命令字
* @param dwMinorCommand		[in]次命令字
* @param buffer							[in]数据缓冲区指针
* @param dwBufferLen				[in]数据缓冲区大小
*/
HIESERVER_API int HieServer_GetConfig(DWORD dwProtocolID, DWORD dwMajorCommand, DWORD dwMinorCommand, char *buffer, DWORD dwBufferLen);

/*!
* @brief 	向协议设置信息
* @param dwProtocolID				[in]协议类型
* @param dwMajorCommand		[in]主命令字
* @param dwMinorCommand		[in]次命令字
* @param buffer							[in]数据缓冲区指针
* @param dwBufferLen				[in]数据缓冲区大小
*/
HIESERVER_API int HieServer_SetConfig(DWORD dwProtocolID, DWORD dwMajorCommand, DWORD dwMinorCommand, char *buffer, DWORD dwBufferLen);

#endif
