#ifndef __SCREEN_MANAGER_H__
#define __SCREEN_MANAGER_H__

#if defined(__IS_SUPPORT_LCD_MANAGER__)

#include <pthread.h>
#include <time.h>
#include <cstdint>

extern "C" {
#include "lcd_layer.h"
#include "lcd_common.h"
#include "font_scale.h"
}
#include "PAR/CConfig.h"

/* ==================== 常量定义 ==================== */
/* 屏幕管理配置字段 */
#define SCREEN_CFG_SECTION      "ScreenManagerCfg"

/* LCD屏幕尺寸 */
#define LCD_SCREEN_WIDTH            (240)
#define LCD_SCREEN_HEIGHT           (320)
/* 屏幕尺寸计算RGB565 */
#define LCD_SCREEN_SIZE             (LCD_SCREEN_WIDTH * LCD_SCREEN_HEIGHT * 2)

/* 状态超时时间（秒） */
#define TIMEOUT_BIND_SUCCESS        (5)         /* 绑定成功显示时间 */
#define TIMEOUT_BIND_FAIL_RETURN    (5)         /* 绑定失败返回时间 */
#define TIMEOUT_BINDING_CONNECTING  (4 * 60)    /* 绑定/连接超时时间 */
#define TIMEOUT_REMINDING           (1 * 60)    /* 定时提醒超时时间 */

/* 刷新线程间隔 */
#define REFRESH_THREAD_INTERVAL_MS  (100)       /* 100毫秒 */

/* 亮度范围 */
#define BRIGHTNESS_MAX              (100)

/* 支持最大的背景图序号 */
#define BACKGROUNDNUM_MAX           (9)

/* 各状态支持最大的额外背景图数量 */
#define LCD_SUPPORT_IMAGE_CNT   (2)

/* 背景图最大大小限制 */
#define JPEG_MAX_IMAGE_SIZE   (40*1024)

/* ==================== 枚举定义 ==================== */
/* 语言类型 */
typedef enum {
    LANGUAGE_CHINESE = 0,
    LANGUAGE_ENGLISH = 1,
    LANGUAGE_MAX
} LCD_LanguageType_t;

/**
 * @brief 主状态枚举（状态层级架构 - 第一层）
 * @note 主状态代表设备的主要工作模式
 */
typedef enum {
    MAIN_STATE_NONE = 0,        /* 无效状态（初始值） */
    MAIN_STATE_BIND,            /* 绑定模式 */
    MAIN_STATE_IDLE,            /* 待机模式 */
    MAIN_STATE_CALL,            /* 通话模式 */
    MAIN_STATE_REMIND,          /* 提醒模式 */
    MAIN_STATE_SLEEP,           /* 休眠模式 */
    MAIN_STATE_MAX
} MainState_t;

/**
 * @brief 无效子状态（用于无子任务的主状态）
 * @note 待机、休眠、提醒等主状态无子任务，使用此类型
 */
typedef enum {
    SUB_STATE_NONE = 0          /* 无效子状态 */
} SubStateNone_t;

/**
 * @brief 绑定模式子状态（状态层级架构 - 第二层）
 * @note 绑定模式下的具体子任务状态
 */
typedef enum {
    BIND_SUB_UNBOUND = 0,       /* 未配网（显示"欢迎您"） */
    BIND_SUB_CONNECTING,        /* 连接中（4分钟超时） */
    BIND_SUB_BINDING,           /* 绑定中（4分钟超时） */
    BIND_SUB_SUCCESS,           /* 绑定成功（5秒后跳转待机） */
    BIND_SUB_FAILED,            /* 绑定失败（5秒后返回未配网） */
    BIND_SUB_MAX
} BindSubState_t;

/**
 * @brief 通话模式子状态（状态层级架构 - 第二层）
 * @note 通话模式下的具体子任务状态
 */
typedef enum {
    CALL_SUB_CALLING = 0,       /* 呼叫中 */
    CALL_SUB_WAITANSWER,        /* 等待接听 */
    CALL_SUB_VOICE,             /* 语音通话中 */
    CALL_SUB_VIDEO,             /* 视频通话中 */
    CALL_SUB_NOANSWER,          /* 呼叫未接听（5秒后跳转待机） */
    CALL_SUB_MAX
} CallSubState_t;

/**
 * @brief 子状态联合（根据主状态选择对应的子状态类型）
 * @note 使用联合节省内存，同一时间只有一个子状态类型有效
 */
typedef union {
    SubStateNone_t none;        /* 无效子状态（待机、休眠、提醒模式使用） */
    BindSubState_t bind;        /* 绑定模式子状态 */
    CallSubState_t call;        /* 通话模式子状态 */
} SubStateUnion_t;

/**
 * @brief 组合状态结构体（主状态+子状态）
 * @note 完整描述设备当前工作状态
 */
typedef struct {
    MainState_t       mainState;    /* 当前主状态 */
    SubStateUnion_t   subState;     /* 当前子状态（联合类型） */
} WorkState_t;

/**
 * @brief 设备绑定状态（供外部模块调用）
 */
typedef enum {
    DEV_BIND_STATUS_CONNECTING = 0,     /* 0:连接中 */
    DEV_BIND_STATUS_BINDING,            /* 1:绑定中 */
    DEV_BIND_STATUS_SUCCESS,            /* 2:添加成功 */
    DEV_BIND_STATUS_FAILED,             /* 3:添加失败 */
    DEV_BIND_STATUS_MAX
} DevBindStatus_t;

/**
 * @brief 通话状态（供外部模块调用）
 */
typedef enum {
    CALL_STATUS_CALLING = 0,            /* 0:呼叫中 */
    CALL_STATUS_VIDEO,                  /* 1:视频通话中 */
    CALL_STATUS_VOICE,                  /* 2:语音通话中 */
    CALL_STATUS_NO_ANSWER,              /* 3:呼叫未接听 */
    CALL_STATUS_HANGUP,                 /* 4:呼叫挂断 */
    CALL_STATUS_WAITANSWER,             /* 5:等待接听 */
    CALL_STATUS_MAX
} CallStatus_t;

/**
 * @brief 提醒状态（供外部模块调用）
 */
typedef enum {
    REMIND_STATUS_START = 0,            /* 0:定时提醒开始 */
    REMIND_STATUS_END,                  /* 1:定时提醒结束 */
    REMIND_STATUS_MAX
} RemindStatus_t;

/* ==================== 类定义 ==================== */
class ScreenManager {
public:
    /* 获取单例实例 */
    static ScreenManager* GetInstance();

    /* 释放单例实例 */
    static void ReleaseInstance();

    /* 禁止拷贝和赋值 */
    ScreenManager(const ScreenManager&) = delete;
    ScreenManager& operator=(const ScreenManager&) = delete;

    /**
     * @brief 设置背景图片（主题）数据
     * @param imageData RGB565格式图片数据
     * @param dataLen 数据长度（字节）
     * @return 0成功，-1失败
     */
    int SetBackgroundImageData(const char* imageData, uint32_t dataLen);

    /**
     * @brief 获取背景图片（主题）序号
     * @param number 输出指针
     * @return 0成功，-1失败
     */
    int GetBackgroundImageNumber(uint32_t *number);

    /**
     * @brief 设置背景图片（主题）序号
     * @param number 序号（0-9）
     * @return 0成功，-1失败
     */
    int SetBackgroundImageNumber(uint32_t number);

    /**
     * @brief 获取熄屏超时时间
     * @param seconds 超时秒数（0表示永不休眠）
     */
    int GetSleepTimeout(uint32_t *seconds);

    /**
     * @brief 设置熄屏超时时间
     * @param seconds 超时秒数（0表示永不休眠）
     */
    int SetSleepTimeout(uint32_t seconds);

    /**
     * @brief 获取屏幕亮度
     * @param level 亮度等级（BRIGHTNESS_MIN ~ BRIGHTNESS_MAX）
     */
    int GetBrightness(uint32_t *level);

    /**
     * @brief 设置屏幕亮度
     * @param level 亮度等级（BRIGHTNESS_MIN ~ BRIGHTNESS_MAX）
     */
    int SetBrightness(uint32_t level);

    /**
     * @brief 设置设备绑定连接状态（供网络模块调用）
     * @param status 绑定状态枚举
     * @return 0成功
     */
    int ExternSetDevBindConnetStatus(int status);

    /**
     * @brief 设置通话状态（供呼叫模块调用）
     * @param status 通话状态枚举
     * @return 0成功
     */
    int ExternSetCallStatus(int status);

    /**
     * @brief 设置定时提醒状态（供定时提醒模块调用）
     * @param status 提醒状态
     * @return 0成功
     */
    int ExternSetScheduleRemindStatus(int status);

    /**
     * @brief 视频通话喂流接口
     * @param dataBuf 视频数据缓冲区
     * @param bufSize 数据大小
     * @return 0成功，-1失败
     */
    int SetVideoCallStreamData(const char *dataBuf, uint32_t bufSize);

    /**
     * @brief 打印当前状态、图层信息（调试使用）
     * @return 0成功
     */
    int DumpInfo();

    /**
     * @brief 唤醒屏幕（重置活动时间）
     */
    int WakeUp();

private:
    /* 私有构造/析构（单例模式） */
    ScreenManager();
    ~ScreenManager();

    /* 单例指针 */
    static ScreenManager* theScreenManager;

    /* ---------------- 内部管理 ---------------- */
    bool Init();
    bool UnInit();

    // 加载初始配置参数
    void LoadParam();

    /* 检查设备是否已绑定 */
    bool IsDeviceBound() const;

    /* 刷新线程（静态入口） */
    static void* RefreshThreadFunc(void* arg);

    /* 状态管理 */
    void SetWorkState(const WorkState_t& state);
    const WorkState_t& GetWorkState() const;
    MainState_t GetMainState() const;

    /* 状态应用与渲染 */
    int HandleUnboundState(int needUpdateBg);
    int HandleConnectingState(int needUpdateBg);
    int HandleBindingState(int needUpdateBg);
    int HandleBindSuccessState(int needUpdateBg);
    int HandleBindFailState(int needUpdateBg);
    int HandleIdleState(int needUpdateBg);
    int HandleVoiceCallState(int needUpdateBg);
    int HandleVideoCallState(int needUpdateBg);
    int HandleCallingState(int needUpdateBg);
    int HandleWaitAnswerState(int needUpdateBg);
    int HandleCallNoAnswerState(int needUpdateBg);
    int HandleRemindingState(int needUpdateBg);

    /* 子状态处理入口 */
    int HandleBindMode(BindSubState_t subState, int needUpdateBg);
    int HandleCallMode(CallSubState_t subState, int needUpdateBg);

    /* 状态检查与转换 */
    void CheckAndApplyState();
    void CheckStateTimeout();
    void CheckSleepTimeout();

    /* 硬件控制 */
    void SetBacklight(bool on);

    /* 加载图片资源（从外部解码器） */
    bool LoadImageResource(ImageResource_t* resource, const char* path);
    void ReleaseImageResource(ImageResource_t* resource);

    /* ---------------- 成员变量 ---------------- */
    LayerManager_t      mLayerManager;          /* 图层管理器 */
    pthread_mutex_t     mMutex;                 /* 主锁（保护状态） */
    pthread_t           mRefreshThread;         /* 刷新线程ID */
    volatile bool       mRefreshThreadRunning;  /* 线程运行标志 */
	CConfig *           pCConfig;

    /* 状态管理（新架构 - 主状态+子状态） */
    WorkState_t         mWorkState;             /* 当前组合状态 */
    WorkState_t         mPrevWorkState;         /* 上一组合状态 */
    time_t              mStateEnterTime;        /* 状态进入时间戳 */
    time_t              mLastActivityTime;      /* 最后活动时间戳 */
    int                 mNeedUpdateBg;          /* 需要更新背景 */

    /* 配置 */
    uint32_t            mBGNumber;              /* 背景图片序号（0-9） */
    uint32_t            mSleepTimeout;          /* 休眠超时（秒，0=不休眠） */
    uint32_t            mBrightness;            /* 当前亮度（0-100） */
    LCD_LanguageType_t  mLanguageType;          /* 语言类型 */

    /* 资源缓冲  */
    ImageResource_t     mBackgroundImage;
    ImageResource_t     mExtraImage[LCD_SUPPORT_IMAGE_CNT];
};

#endif /* __IS_SUPPORT_LCD_MANAGER__ */

#endif /* __SCREEN_MANAGER_H__ */

