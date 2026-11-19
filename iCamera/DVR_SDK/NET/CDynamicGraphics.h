#ifndef _CDYNAMIC_GRAPHICS_H_
#define _CDYNAMIC_GRAPHICS_H_
#include "global_macro.h"
#include "newhie/ZWP2PPUSHVNClient.h"
#include "newhie/ZWP2PPUSHVNClient_Common.h"

/**************************************************************************
 * 处理动态图推送，包括初始化、设置参数、推送事件、回调等 *
 * 1. 初始化：初始化动态图推送，包括设置参数、注册回调等
 * 2. 设置参数：设置动态图推送的参数，包括重传模式、是否支持动图、是否支持预录动图等
 * 3. 推送事件：推送动态图事件，包括事件类型、通道号、事件信息等
 * 4. 回调：处理动态图推送的回调，包括事件状态回调、图片捕获回调、包状态回调等
 **************************************************************************/
#if defined(__IS_SUPPORT_DYNAMIC_GRAPHICS__)
class CDynamicGraphics
{
public:
    ~CDynamicGraphics();
    void Init();
    unsigned int GetRetransmissionMode(); //获取重传模式
    unsigned int GetIsSupportDynamicGraphics(); //获取是否支持动图
    unsigned int GetIsSupportPrerecorded(); //获取是否支持预录动图
    bool IsInit(); //是否初始化
    static CDynamicGraphics* GetInstance()
    {
        static CDynamicGraphics instance;
        return &instance;
    }
    int PushEvent(int nEventType,int nChannel,LPZWP2P_PUSH_EVENT_INFO pEventInfo);
    int GetDID(char *did, int didlen);
private:
    CDynamicGraphics();
private:
    unsigned int m_RetransmissionMode; //重传模式 0 协议控制重传，1 中间层控制重传。中间层控制重传时，由中间层保存数据，协议控制重传时，由协议保存数据，缓冲区满了之后正常淘汰，没满之前一直重试上报事件
    unsigned int m_isSupportDynamicGraphics; //是否支持动图（不启用中间层重传才生效），0:不支持，1：支持
    unsigned int m_isSupportPrerecorded; //是否支持预录动图（支持动图才生效），0:不支持，1：支持
    unsigned int m_ImageLength; //抓图图片 buffer 的最大长度（默认大小为 64K）
    unsigned long long m_EventID; //事件ID
    bool m_bInit; //是否初始化
};
#endif // __IS_SUPPORT_DYNAMIC_GRAPHICS__

#endif // _CDYNAMIC_GRAPHICS_H_