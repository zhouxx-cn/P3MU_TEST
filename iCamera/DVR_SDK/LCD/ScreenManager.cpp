#if defined(__IS_SUPPORT_LCD_MANAGER__)
/* 编码声明: UTF-8 */
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

#include "ScreenManager.h"
#include "zwsdk.h"
#include "PUB_Language.h"
#include "common_pub.h"

/* 多语言字符串资源 */
typedef struct {
    const TextResource_t time;
    const TextResource_t date;
    const TextResource_t calling;
    const TextResource_t noAnswer;
    const TextResource_t welcome;
    const TextResource_t binding;
    const TextResource_t bindSuccess;
    const TextResource_t bindFailed;
    const TextResource_t bindFailedHint;
    const TextResource_t connecting;
    const TextResource_t reminding;
} StringResGroup_t;

/* 单张图片资源 */
typedef struct {
    const char*     path;       // 图片路径
    const uint16_t  x;          // 显示位置左上角X坐标
    const uint16_t  y;          // 显示位置左上角Y坐标
} ImageSingle_t;

/* 图片资源 */
typedef struct {
    const ImageSingle_t calling;       // 呼叫中
    const ImageSingle_t waitAnswer;    // 等待接听
    const ImageSingle_t callVoice;     // 语音通话
    const ImageSingle_t noAnswer;      // 未接听
    const ImageSingle_t welcome;       // 欢迎
    const ImageSingle_t binding;       // 绑定中
    const ImageSingle_t bindSuccess;   // 绑定成功
    const ImageSingle_t bindFailed;    // 绑定失败
    const ImageSingle_t connecting;    // 连接中
    const ImageSingle_t reminding;     // 定时提醒中
} ImageResGroup_t;

/* 总资源结构体（合并 字符串 + 图片，统一管理）*/
typedef struct {
    // WIFI图标
    const TextResource_t wifitext;
    // 多语言字符串表 [语言]
    const StringResGroup_t  str[LANGUAGE_MAX];
    // 状态图片表
    const ImageResGroup_t img;
} LCD_Resource_t;

/************************ 全局统一资源表(默认信息) ************************/
static const LCD_Resource_t g_lcd_res = {
.wifitext = {
    .TextStr=NULL, .x=190, .y=10, .scale=100, .align=OSD_ALIGN_LEFT
},

.str = {
    // 中文
    [LANGUAGE_CHINESE] = {
        .time =           {.TextStr="12:00",               .x=0, .y=60,  .scale=110, .align=OSD_ALIGN_CENTER},
        .date =           {.TextStr="4月9日 周四",             .x=0, .y=130, .scale=40,  .align=OSD_ALIGN_CENTER},
        .calling =        {.TextStr="呼叫中...",              .x=0, .y=230, .scale=60,  .align=OSD_ALIGN_CENTER},
        .noAnswer =       {.TextStr="对方未接听",               .x=0, .y=230, .scale=60,  .align=OSD_ALIGN_CENTER},
        .welcome =        {.TextStr="欢迎您!",                .x=0, .y=90,  .scale=60,  .align=OSD_ALIGN_CENTER},
        .binding =        {.TextStr="设备绑定中...",            .x=0, .y=90,  .scale=50,  .align=OSD_ALIGN_CENTER},
        .bindSuccess =    {.TextStr="添加成功",                .x=0, .y=210, .scale=50,  .align=OSD_ALIGN_CENTER},
        .bindFailed =     {.TextStr="添加失败",                .x=0, .y=70,  .scale=60,  .align=OSD_ALIGN_CENTER},
        .bindFailedHint = {.TextStr="请到APP查看失败原因", .x=0,         .y=120, .scale=30,  .align=OSD_ALIGN_CENTER},
        .connecting =     {.TextStr="正在连接中...",            .x=0, .y=90,  .scale=50,  .align=OSD_ALIGN_CENTER},
        .reminding =      {.TextStr="12:00",               .x=0, .y=40,  .scale=90,  .align=OSD_ALIGN_CENTER}
    },

    // 英文
    [LANGUAGE_ENGLISH] = {
        .time =           {.TextStr="12:00",                            .x=0, .y=60,  .scale=110, .align=OSD_ALIGN_CENTER},
        .date =           {.TextStr="Thu Apr 9th",                      .x=0, .y=130, .scale=40,  .align=OSD_ALIGN_CENTER},
        .calling =        {.TextStr="Calling...",                       .x=0, .y=230, .scale=60,  .align=OSD_ALIGN_CENTER},
        .noAnswer =       {.TextStr="No answer",                        .x=0, .y=230, .scale=60,  .align=OSD_ALIGN_CENTER},
        .welcome =        {.TextStr="Welcome!",                         .x=0, .y=90,  .scale=60,  .align=OSD_ALIGN_CENTER},
        .binding =        {.TextStr="Binding device...",                .x=0, .y=90,  .scale=45,  .align=OSD_ALIGN_CENTER},
        .bindSuccess =    {.TextStr="Added successfully",               .x=0, .y=210, .scale=35,  .align=OSD_ALIGN_CENTER},
        .bindFailed =     {.TextStr="Add failed",                       .x=0, .y=70,  .scale=60,  .align=OSD_ALIGN_CENTER},
        .bindFailedHint = {.TextStr="Please check the APP for details", .x=0, .y=120, .scale=22,  .align=OSD_ALIGN_CENTER},
        .connecting =     {.TextStr="Connecting...",                    .x=0, .y=90,  .scale=50,  .align=OSD_ALIGN_CENTER},
        .reminding =      {.TextStr="12:00",                            .x=0, .y=40,  .scale=90,  .align=OSD_ALIGN_CENTER}
    }
},

.img = {
    .calling        = {.path="/mnt/app/custom/img/calling.jpg",        .x=50,    .y=70},
    .waitAnswer     = {.path="/mnt/app/custom/img/waitAnswer.jpg",     .x=195,   .y=10},
    .callVoice      = {.path="/mnt/app/custom/img/callVoice.jpg",      .x=50,    .y=70},
    .noAnswer       = {.path="/mnt/app/custom/img/noAnswer.jpg",       .x=50,    .y=70},
    .welcome        = {.path="/mnt/app/custom/img/welcome.jpg",        .x=20,    .y=150},
    .binding        = {.path="/mnt/app/custom/img/binding.jpg",        .x=20,    .y=150},
    .bindSuccess    = {.path="/mnt/app/custom/img/bindSuccess.jpg",    .x=50,    .y=50},
    .bindFailed     = {.path="/mnt/app/custom/img/bindFailed.jpg",     .x=20,    .y=150},
    .connecting     = {.path="/mnt/app/custom/img/connecting.jpg",     .x=20,    .y=150},
    .reminding      = {.path="/mnt/app/custom/img/reminding.jpg",      .x=50,    .y=120}
}
};

/* ==================== 内部常量 ==================== */
/* 星期字符串表 */
static const char *WEEK_STR_CH[] = {"周日", "周一", "周二", "周三", "周四", "周五", "周六"};
static const char *WEEK_STR_EN[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

/* 月份字符串表（英文） */
static const char *MONTH_STR_EN[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

/* ==================== 工具函数 ==================== */
/**
 * @brief 获取系统单调时钟（秒）
 */
static time_t GetMonotonicTimeSec(void)
{
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return tp.tv_sec;
}

static int LCDRefreshFrame(uint16_t startX, uint16_t startY, uint16_t width, uint16_t height, uint8_t *pData, unsigned int DataSize)
{
    ZWSys_LCDRefreshFrame((char *)pData, DataSize);
    return 0;
}

/* ==================== 单例管理 ==================== */
ScreenManager* ScreenManager::theScreenManager = NULL;

ScreenManager *ScreenManager::GetInstance()
{
    if (!theScreenManager)
    {
        theScreenManager = new ScreenManager();
        if (NULL != theScreenManager)
        {
            theScreenManager->Init();
        }
    }

    return theScreenManager;
}

void ScreenManager::ReleaseInstance()
{
    if (NULL != theScreenManager)
    {
        theScreenManager->UnInit();
        delete theScreenManager;
        theScreenManager = NULL;
    }

    return;
}

ScreenManager::ScreenManager()
    : mRefreshThreadRunning(false)
    , mStateEnterTime(0)
    , mLastActivityTime(0)
    , mNeedUpdateBg(0)
    , mBGNumber(1)
    , mSleepTimeout(60)
    , mBrightness(BRIGHTNESS_MAX)
    , mLanguageType(LANGUAGE_CHINESE)
{
    pthread_mutex_init(&mMutex, NULL);
    memset(&mBackgroundImage, 0, sizeof(ImageResource_t));
    memset(mExtraImage, 0, sizeof(ImageResource_t)*LCD_SUPPORT_IMAGE_CNT);

    /* 初始化状态结构 */
    memset(&mWorkState, 0, sizeof(WorkState_t));
    memset(&mPrevWorkState, 0, sizeof(WorkState_t));
    mWorkState.mainState = MAIN_STATE_NONE;
    mPrevWorkState.mainState = MAIN_STATE_NONE;
}

ScreenManager::~ScreenManager()
{
    pthread_mutex_destroy(&mMutex);
}

bool ScreenManager::Init()
{
	pCConfig = CConfig::GetInstance();

    /* 加载初始化配置 */
    LoadParam();

    /* 分配背景缓冲区 */
    mBackgroundImage.data = (uint8_t*)malloc(LCD_SCREEN_SIZE);
    if (!mBackgroundImage.data) {
         LCD_ERR("malloc background buffer failed, size=%u\n", LCD_SCREEN_SIZE);
        return false;
    }

    /* 默认全屏黑背景 */
    memset(mBackgroundImage.data, 0, LCD_SCREEN_SIZE);
    mBackgroundImage.dataSize = LCD_SCREEN_SIZE;
    mBackgroundImage.width = LCD_SCREEN_WIDTH;
    mBackgroundImage.height = LCD_SCREEN_HEIGHT;

    /* 初始化图层管理器 */
    if (Layer_Init(&mLayerManager, LCD_SCREEN_WIDTH, LCD_SCREEN_HEIGHT) != 0) {
        LCD_ERR("Layer_Init failed\n");
        return false;
    }

    /* 初始化语言设置 */
	eSpeechLanguage eSpLan = eSpeechLanguage_Chinese;
	PUB_GetLanguageFromCfg(&eSpLan);
    mLanguageType = ((eSpLan == eSpeechLanguage_Chinese) ? LANGUAGE_CHINESE : LANGUAGE_ENGLISH);

    /* 设置LCD硬件刷新回调接口   */
    Layer_SetRefreshCallback(&mLayerManager, LCDRefreshFrame);

    /* 根据绑定状态设置初始状态 */
    if (IsDeviceBound()) {
        WorkState_t idleState = {MAIN_STATE_IDLE, {.none = SUB_STATE_NONE}};
        SetWorkState(idleState);
    } else {
        WorkState_t bindState = {MAIN_STATE_BIND, {.bind = BIND_SUB_UNBOUND}};
        SetWorkState(bindState);
    }

    /* 启动刷新线程 */
    mRefreshThreadRunning = true;
    if (pthread_create(&mRefreshThread, NULL, RefreshThreadFunc, this) != 0) {
        LCD_ERR("create refresh thread failed\n");
        mRefreshThreadRunning = false;
        return false;
    }

    mLastActivityTime = GetMonotonicTimeSec();
    LCD_INF("ScreenManager Init, mainState=%d\n", GetMainState());
    return true;
}

bool ScreenManager::UnInit()
{
    mRefreshThreadRunning = false;
    if (mRefreshThread) {
        pthread_join(mRefreshThread, NULL);
        mRefreshThread = 0;
    }

    /* 释放资源 */
    ReleaseImageResource(&mBackgroundImage);
    for (int i = 0; i < LCD_SUPPORT_IMAGE_CNT; i++) {
        ReleaseImageResource(&(mExtraImage[i]));
    }

    Layer_UnInit(&mLayerManager);

    LCD_INF("ScreenManager UnInit\n");
    return true;
}

void ScreenManager::LoadParam()
{
	// 读取背景图序号
    mBGNumber = pCConfig->GetValue(SCREEN_CFG_SECTION, "BGNumber", 1);

	// 读取屏幕休眠时间
    mSleepTimeout = pCConfig->GetValue(SCREEN_CFG_SECTION, "SleepTimeout", 60);

	// 读取屏幕亮度
    mBrightness = pCConfig->GetValue(SCREEN_CFG_SECTION, "Brightness", 80);
}

/* ==================== 内部工具方法 ==================== */
bool ScreenManager::IsDeviceBound() const
{
    char szSCode[64] = {0};
    int nSCodeLen = sizeof(szSCode);
    int rc = MPU_SYS_DeviceBind_GetSCode(szSCode, &nSCodeLen);

    return (rc == 0 && strlen(szSCode) > 0);
}

/* ==================== 状态管理 ==================== */

// 设置工作状态（内部调用，需已加锁）
void ScreenManager::SetWorkState(const WorkState_t& newState)
{
    /* 检查状态是否变化 */
    if (mWorkState.mainState == newState.mainState) {
        bool subStateChanged = false;
        switch (mWorkState.mainState) {
            case MAIN_STATE_BIND:
                subStateChanged = (mWorkState.subState.bind != newState.subState.bind);
                break;
            case MAIN_STATE_CALL:
                subStateChanged = (mWorkState.subState.call != newState.subState.call);
                break;
            default:
                return;
        }
        if (!subStateChanged) {
            return;
        }
    }

    int oldSubState = (mWorkState.mainState == MAIN_STATE_BIND) ?
                       static_cast<int>(mWorkState.subState.bind) :
                       static_cast<int>(mWorkState.subState.call);
    int newSubState = (newState.mainState == MAIN_STATE_BIND) ?
                       static_cast<int>(newState.subState.bind) :
                       static_cast<int>(newState.subState.call);
    LCD_INF("Work state change: [main=%d, sub=%d] -> [main=%d, sub=%d]\n",
            mWorkState.mainState, oldSubState,
            newState.mainState, newSubState);

    mPrevWorkState = mWorkState;
    mWorkState = newState;
    mStateEnterTime = GetMonotonicTimeSec();
}

// 获取工作状态
const WorkState_t& ScreenManager::GetWorkState() const
{
    return mWorkState;
}

// 获取主状态
MainState_t ScreenManager::GetMainState() const
{
    return mWorkState.mainState;
}

/* ==================== 刷新线程 ==================== */

// 刷新线程函数
void* ScreenManager:: RefreshThreadFunc(void* arg)
{
    PRT_TID_NAME("LCD_RefreshThread");
    ScreenManager* self = static_cast<ScreenManager*>(arg);

    while (self->mRefreshThreadRunning) {
        usleep(REFRESH_THREAD_INTERVAL_MS * 1000);

        /* 检查状态超时（自动跳转） */
        self->CheckStateTimeout();

        /* 检查休眠超时 */
        self->CheckSleepTimeout();

        /* 检查状态变化 */
        self->CheckAndApplyState();
    }

    return NULL;
}

/* ==================== 状态检查与转换（新架构） ==================== */
void ScreenManager::CheckStateTimeout()
{
    pthread_mutex_lock(&mMutex);

    time_t now = GetMonotonicTimeSec();
    time_t elapsed = now - mStateEnterTime;

    /* 使用新的状态结构 */
    MainState_t mainState = mWorkState.mainState;

    /* 主状态switch：先判断主任务 */
    switch (mainState) {
        case MAIN_STATE_BIND:
            /* 绑定模式：检查各子状态超时 */
            switch (mWorkState.subState.bind) {
                case BIND_SUB_SUCCESS:
                    /* 绑定成功：5秒后跳转到待机 */
                    if (elapsed >= TIMEOUT_BIND_SUCCESS) {
                        WorkState_t idleState = {MAIN_STATE_IDLE, {.none = SUB_STATE_NONE}};
                        SetWorkState(idleState);
                    }
                    break;

                case BIND_SUB_FAILED:
                    /* 绑定失败：5秒后返回到未配网 */
                    if (elapsed >= TIMEOUT_BIND_FAIL_RETURN) {
                        WorkState_t unboundState = {MAIN_STATE_BIND, {.bind = BIND_SUB_UNBOUND}};
                        SetWorkState(unboundState);
                    }
                    break;

                case BIND_SUB_CONNECTING:
                case BIND_SUB_BINDING:
                    /* 连接中/绑定中：4分钟超时 */
                    if (elapsed >= TIMEOUT_BINDING_CONNECTING) {
                        WorkState_t failState = {MAIN_STATE_BIND, {.bind = BIND_SUB_FAILED}};
                        SetWorkState(failState);
                    }
                    break;

                default:
                    break;
            }
            break;

        case MAIN_STATE_CALL:
            /* 通话模式：检查子状态超时 */
            switch (mWorkState.subState.call) {
                case CALL_SUB_NOANSWER:
                    /* 呼叫未接听：5秒后跳转到待机 */
                    if (elapsed >= TIMEOUT_BIND_SUCCESS) {
                        WorkState_t idleState = {MAIN_STATE_IDLE, {.none = SUB_STATE_NONE}};
                        SetWorkState(idleState);
                    }
                    break;

                default:
                    break;
            }
            break;

        case MAIN_STATE_REMIND:
            /* 提醒模式：1分钟后跳转到待机 */
            if (elapsed >= TIMEOUT_REMINDING) {
                WorkState_t idleState = {MAIN_STATE_IDLE, {.none = SUB_STATE_NONE}};
                SetWorkState(idleState);
            }
            break;

        default:
            /* 其他主状态无需超时处理 */
            break;
    }

    pthread_mutex_unlock(&mMutex);
}

void ScreenManager::CheckSleepTimeout()
{
    if (mSleepTimeout == 0) {
        return; /* 永不休眠 */
    }

    pthread_mutex_lock(&mMutex);
    /* 仅待机状态可进入休眠 */
    if (mWorkState.mainState != MAIN_STATE_IDLE) {
        pthread_mutex_unlock(&mMutex);
        return;
    }

    if ((GetMonotonicTimeSec() - mLastActivityTime) >= (time_t)mSleepTimeout) {
        WorkState_t sleepState = {MAIN_STATE_SLEEP, {.none = SUB_STATE_NONE}};
        SetWorkState(sleepState);
    }
    pthread_mutex_unlock(&mMutex);
}

/* ==================== 资源管理 ==================== */

/* 填充图片信息 */
bool ScreenManager::LoadImageResource(ImageResource_t* resource, const char* path)
{
    if (resource == NULL) {
        return false;
    }

    if (path == NULL) {
        if (resource->dataSize) {
            /* 背景直接填充黑屏0x00数据 */
            memset(resource->data, 0, resource->dataSize);
            return true;
        }
        return false;
    }

    char *outbuf = NULL;
    uint32_t width, height, outbufsize;
    if (DecodeJpegToRgb565(path, &outbuf, &outbufsize, &width, &height) != 0) {
        LCD_ERR("DecodeJpegToRgb565 failed: path[%s]\n", path);
        return false;
    }

    /* 如果不为空，先销毁 */
    if (resource->data != NULL) {
        free((void*)resource->data);
    }
    resource->data = (uint8_t *)outbuf;
    resource->dataSize = outbufsize;
    resource->width = width;
    resource->height = height;

    LCD_DBG("LoadImageResource: path[%s] data[%p] size[%u] wxh[%hu-%hu]\n", \
            path, resource->data, resource->dataSize, resource->width, resource->height);
    return true;
}

void ScreenManager::ReleaseImageResource(ImageResource_t* resource)
{
    if (resource == NULL) {
        return;
    }
    LCD_DBG("ReleaseImageResource: data[%p] size[%u]\n", resource->data, resource->dataSize);

    if (resource->data != NULL) {
        free((void*)resource->data);
        resource->data = NULL;
        resource->dataSize = 0;
    }

    memset(resource, 0, sizeof(ImageResource_t));
}

/* ==================== 状态应用与渲染 ==================== */
int ScreenManager::HandleUnboundState(int needUpdateBg)
{
    int result = 0;

    if (needUpdateBg) {
        /* 更新背景 */
        LoadImageResource(&mBackgroundImage, NULL);
        result |= Layer_UpdateBackground(&mLayerManager, &mBackgroundImage);
    }

    /* 加载欢迎您提示图片 */
    LoadImageResource(&mExtraImage[0], g_lcd_res.img.welcome.path);
    mExtraImage[0].x = g_lcd_res.img.welcome.x;
    mExtraImage[0].y = g_lcd_res.img.welcome.y;
    result |= Layer_SetImageLayer(&mLayerManager, mExtraImage, 1);

    /* 加载欢迎您提示文字 */
    result |= Layer_SetTextLayer(&mLayerManager, &(g_lcd_res.str[mLanguageType].welcome), 1);
    return result;
}

int ScreenManager::HandleConnectingState(int needUpdateBg)
{
    int result = 0;

    if (needUpdateBg) {
        /* 更新背景 */
        LoadImageResource(&mBackgroundImage, NULL);
        result |= Layer_UpdateBackground(&mLayerManager, &mBackgroundImage);
    }

    /* 加载连接中提示图片 */
    LoadImageResource(&mExtraImage[0], g_lcd_res.img.connecting.path);
    mExtraImage[0].x = g_lcd_res.img.connecting.x;
    mExtraImage[0].y = g_lcd_res.img.connecting.y;
    result |= Layer_SetImageLayer(&mLayerManager, mExtraImage, 1);

    /* 加载连接中提示文字 */
    result |= Layer_SetTextLayer(&mLayerManager, &(g_lcd_res.str[mLanguageType].connecting), 1);
    return result;
}

int ScreenManager::HandleBindingState(int needUpdateBg)
{
    int result = 0;

    if (needUpdateBg) {
        /* 更新背景 */
        LoadImageResource(&mBackgroundImage, NULL);
        result |= Layer_UpdateBackground(&mLayerManager, &mBackgroundImage);
    }

    /* 加载绑定中提示图片 */
    LoadImageResource(&mExtraImage[0], g_lcd_res.img.binding.path);
    mExtraImage[0].x = g_lcd_res.img.binding.x;
    mExtraImage[0].y = g_lcd_res.img.binding.y;
    result |= Layer_SetImageLayer(&mLayerManager, mExtraImage, 1);

    /* 加载绑定中提示文字 */
    result |= Layer_SetTextLayer(&mLayerManager, &(g_lcd_res.str[mLanguageType].binding), 1);
    return result;
}

int ScreenManager::HandleBindSuccessState(int needUpdateBg)
{
    int result = 0;

    if (needUpdateBg) {
        /* 更新背景 */
        LoadImageResource(&mBackgroundImage, NULL);
        result |= Layer_UpdateBackground(&mLayerManager, &mBackgroundImage);
    }

    /* 加载绑定成功提示图片 */
    LoadImageResource(&mExtraImage[0], g_lcd_res.img.bindSuccess.path);
    mExtraImage[0].x = g_lcd_res.img.bindSuccess.x;
    mExtraImage[0].y = g_lcd_res.img.bindSuccess.y;
    result |= Layer_SetImageLayer(&mLayerManager, mExtraImage, 1);

    /* 加载绑定成功提示文字 */
    result |= Layer_SetTextLayer(&mLayerManager, &(g_lcd_res.str[mLanguageType].bindSuccess), 1);
    return result;
}

int ScreenManager::HandleBindFailState(int needUpdateBg)
{
    int result = 0;

    if (needUpdateBg) {
        /* 更新背景 */
        LoadImageResource(&mBackgroundImage, NULL);
        result |= Layer_UpdateBackground(&mLayerManager, &mBackgroundImage);
    }

    /* 加载绑定失败提示图片 */
    LoadImageResource(&mExtraImage[0], g_lcd_res.img.bindFailed.path);
    mExtraImage[0].x = g_lcd_res.img.bindFailed.x;
    mExtraImage[0].y = g_lcd_res.img.bindFailed.y;
    result |= Layer_SetImageLayer(&mLayerManager, mExtraImage, 1);

    /* 加载绑定失败提示文字（需两行） */
    TextResource_t textgroup[2];
    memcpy(&textgroup[0], &(g_lcd_res.str[mLanguageType].bindFailed), sizeof(TextResource_t));
    memcpy(&textgroup[1], &(g_lcd_res.str[mLanguageType].bindFailedHint), sizeof(TextResource_t));
    result |= Layer_SetTextLayer(&mLayerManager, textgroup, 2);
    return result;
}

int ScreenManager::HandleIdleState(int needUpdateBg)
{
    int result = 0;

    if (needUpdateBg) {
        /* 更新背景 */
        pthread_mutex_lock(&mMutex);
        if (access(LCD_USR_BACKGROUND_IMAGE_PATH, F_OK) == 0) {
            LoadImageResource(&mBackgroundImage, LCD_USR_BACKGROUND_IMAGE_PATH);
        } else {
            LoadImageResource(&mBackgroundImage, LCD_DEF_BACKGROUND_IMAGE_PATH);
        }
        pthread_mutex_unlock(&mMutex);
        result |= Layer_UpdateBackground(&mLayerManager, &mBackgroundImage);
    }

    /* 获取当前时间 */
    struct tm currentTime;
    time_t t = time(NULL);
    localtime_r(&t, &currentTime);

    /* 格式化时间字符串 "HH:MM" */
    char timeStr[16];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d", currentTime.tm_hour, currentTime.tm_min);

    /* 格式化日期字符串 */
    char dateStr[32];
    if (mLanguageType == LANGUAGE_CHINESE) {
        /* 中文格式: "03月17日 周三" */
        snprintf(dateStr, sizeof(dateStr), "%u月%u日 %s",
                 (unsigned)currentTime.tm_mon + 1,
                 (unsigned)currentTime.tm_mday,
                 WEEK_STR_CH[currentTime.tm_wday]);
    } else {
        /* 英文格式: "Wed Mar 17th" */
        const char *suffix;
        int day = currentTime.tm_mday;

        if (day % 10 == 1 && day % 100 != 11) {
            suffix = "st";
        } else if (day % 10 == 2 && day % 100 != 12) {
            suffix = "nd";
        } else if (day % 10 == 3 && day % 100 != 13) {
            suffix = "rd";
        } else {
            suffix = "th";
        }

        snprintf(dateStr, sizeof(dateStr), "%s %s %d%s",
                 WEEK_STR_EN[currentTime.tm_wday],
                 MONTH_STR_EN[currentTime.tm_mon],
                 day, suffix);
    }

    /* 实时更新时间、日期 */
    TextResource_t textgroup[2];
    memcpy(&textgroup[0], &(g_lcd_res.str[mLanguageType].time), sizeof(TextResource_t));
    memcpy(&textgroup[1], &(g_lcd_res.str[mLanguageType].date), sizeof(TextResource_t));
    textgroup[0].TextStr = timeStr;
    textgroup[1].TextStr = dateStr;
    result |= Layer_SetTextLayer(&mLayerManager, textgroup, 2);

    int nQuality = 100;
    int wifi_status = -1;
    static int nLastQuality = 100;

    /* 此处必须等设备初始化完成才能获取网络状态 */
    if (GetDevInitOverFlag())
    {
        wifi_status = MPU_WLAN_GetStatus();
    }

    /* 此处result判断是为了避免频繁获取 */
    if (result && (wifi_status == 0)) {
        MPU_WLAN_GetAccessSignalQuality(&nQuality);
        nLastQuality = nQuality;
    } else {
        nQuality = nLastQuality;
    }

    /* wifi图标 */
    result |= Layer_SetWifiLayer(&mLayerManager, &(g_lcd_res.wifitext), !!wifi_status, nQuality);
    return result;
}

int ScreenManager::HandleVoiceCallState(int needUpdateBg)
{
    int result = 0;

    if (needUpdateBg) {
        /* 更新背景 */
        LoadImageResource(&mBackgroundImage, NULL);
        result |= Layer_UpdateBackground(&mLayerManager, &mBackgroundImage);
    }

    /* 加载语音通话中提示图片 */
    LoadImageResource(&mExtraImage[0], g_lcd_res.img.callVoice.path);
    mExtraImage[0].x = g_lcd_res.img.callVoice.x;
    mExtraImage[0].y = g_lcd_res.img.callVoice.y;
    result |= Layer_SetImageLayer(&mLayerManager, mExtraImage, 1);

    /* 加载语音通话中提示文字（暂无） */
    return result;
}

int ScreenManager::HandleVideoCallState(int needUpdateBg)
{
    /* 视频通话状态：OSD层隐藏，由外部视频流直接渲染 */
    if (needUpdateBg) {
        LCD_DBG("Video call state: external rendering active\n");
    }
    return 0; /* 无OSD变化 */
}

int ScreenManager::HandleCallingState(int needUpdateBg)
{
    int result = 0;

    if (needUpdateBg) {
        /* 更新背景 */
        LoadImageResource(&mBackgroundImage, NULL);
        result |= Layer_UpdateBackground(&mLayerManager, &mBackgroundImage);
    }

    /* 加载呼叫中提示图片 */
    LoadImageResource(&mExtraImage[0], g_lcd_res.img.calling.path);
    mExtraImage[0].x = g_lcd_res.img.calling.x;
    mExtraImage[0].y = g_lcd_res.img.calling.y;

    result |= Layer_SetImageLayer(&mLayerManager, mExtraImage, 1);

    /* 加载呼叫中提示文字 */
    result |= Layer_SetTextLayer(&mLayerManager, &(g_lcd_res.str[mLanguageType].calling), 1);
    return result;
}

int ScreenManager::HandleWaitAnswerState(int needUpdateBg)
{
    int result = 0;

    if (needUpdateBg) {
        /* 更新背景 */
        LoadImageResource(&mBackgroundImage, NULL);
        result |= Layer_UpdateBackground(&mLayerManager, &mBackgroundImage);
    }

    /* 加载等待接听中提示图片 */
    LoadImageResource(&mExtraImage[0], g_lcd_res.img.calling.path);
    mExtraImage[0].x = g_lcd_res.img.calling.x;
    mExtraImage[0].y = g_lcd_res.img.calling.y;

    LoadImageResource(&mExtraImage[1], g_lcd_res.img.waitAnswer.path);
    mExtraImage[1].x = g_lcd_res.img.waitAnswer.x;
    mExtraImage[1].y = g_lcd_res.img.waitAnswer.y;

    result |= Layer_SetImageLayer(&mLayerManager, mExtraImage, 2);

    /* 加载呼叫中提示文字 */
    result |= Layer_SetTextLayer(&mLayerManager, &(g_lcd_res.str[mLanguageType].calling), 1);
    return result;
}

int ScreenManager::HandleCallNoAnswerState(int needUpdateBg)
{
    int result = 0;

    if (needUpdateBg) {
        /* 更新背景 */
        LoadImageResource(&mBackgroundImage, NULL);
        result |= Layer_UpdateBackground(&mLayerManager, &mBackgroundImage);
    }

    /* 加载无应答动画资源 */
    LoadImageResource(&mExtraImage[0], g_lcd_res.img.noAnswer.path);
    mExtraImage[0].x = g_lcd_res.img.noAnswer.x;
    mExtraImage[0].y = g_lcd_res.img.noAnswer.y;
    result |= Layer_SetImageLayer(&mLayerManager, mExtraImage, 1);

    /* 加载无应答提示文字 */
    result |= Layer_SetTextLayer(&mLayerManager, &(g_lcd_res.str[mLanguageType].noAnswer), 1);
    return result;
}

int ScreenManager::HandleRemindingState(int needUpdateBg)
{
    int result = 0;

    if (needUpdateBg) {
        /* 更新背景 */
        LoadImageResource(&mBackgroundImage, NULL);
        result |= Layer_UpdateBackground(&mLayerManager, &mBackgroundImage);
    }

    /* 加载定时提醒图片资源 */
    LoadImageResource(&mExtraImage[0], g_lcd_res.img.reminding.path);
    mExtraImage[0].x = g_lcd_res.img.reminding.x;
    mExtraImage[0].y = g_lcd_res.img.reminding.y;
    result |= Layer_SetImageLayer(&mLayerManager, mExtraImage, 1);

    /* 获取当前时间 */
    struct tm currentTime;
    time_t t = time(NULL);
    localtime_r(&t, &currentTime);

    /* 格式化时间字符串 "HH:MM" */
    char timeStr[16];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d", currentTime.tm_hour, currentTime.tm_min);

    /* 实时显示时间 */
    TextResource_t textgroup;
    memcpy(&textgroup, &(g_lcd_res.str[mLanguageType].reminding), sizeof(TextResource_t));
    textgroup.TextStr = timeStr;
    result |= Layer_SetTextLayer(&mLayerManager, &textgroup, 1);
    return result;
}

/* ==================== 状态应用与渲染（新架构） ==================== */

// 处理绑定模式下的子状态
int ScreenManager::HandleBindMode(BindSubState_t subState, int needUpdateBg)
{
    switch (subState) {
        case BIND_SUB_UNBOUND:
            return HandleUnboundState(needUpdateBg);

        case BIND_SUB_CONNECTING:
            return HandleConnectingState(needUpdateBg);

        case BIND_SUB_BINDING:
            return HandleBindingState(needUpdateBg);

        case BIND_SUB_SUCCESS:
            return HandleBindSuccessState(needUpdateBg);

        case BIND_SUB_FAILED:
            return HandleBindFailState(needUpdateBg);

        default:
            LCD_WRAN("Unknown bind substate: %d\n", subState);
            return 0;
    }
}

// 处理通话模式下的子状态
int ScreenManager::HandleCallMode(CallSubState_t subState, int needUpdateBg)
{
    switch (subState) {
        case CALL_SUB_CALLING:
            return HandleCallingState(needUpdateBg);

        case CALL_SUB_WAITANSWER:
            return HandleWaitAnswerState(needUpdateBg);

        case CALL_SUB_VOICE:
            return HandleVoiceCallState(needUpdateBg);

        case CALL_SUB_VIDEO:
            return HandleVideoCallState(needUpdateBg);

        case CALL_SUB_NOANSWER:
            return HandleCallNoAnswerState(needUpdateBg);

        default:
            LCD_WRAN("Unknown call substate: %d\n", subState);
            return 0;
    }
}

void ScreenManager::CheckAndApplyState()
{
    int result = 0;
    int needUpdateBg = 0;

    /* 获取当前组合状态 */
    MainState_t currentMainState = mWorkState.mainState;
    WorkState_t prevState = mPrevWorkState;

    /* 状态发生变化时重置（主状态变化或子状态变化） */
    bool stateChanged = false;
    if (currentMainState != mPrevWorkState.mainState) {
        stateChanged = true;
    } else {
        /* 主状态相同，检查子状态是否变化 */
        switch (currentMainState) {
            case MAIN_STATE_BIND:
                stateChanged = (mWorkState.subState.bind != mPrevWorkState.subState.bind);
                break;
            case MAIN_STATE_CALL:
                stateChanged = (mWorkState.subState.call != mPrevWorkState.subState.call);
                break;
            default:
                /* 待机、休眠、提醒模式无子状态 */
                break;
        }
    }

    if (stateChanged) {
        /* 从休眠唤醒时打开背光 */
        if (prevState.mainState == MAIN_STATE_SLEEP || prevState.mainState == MAIN_STATE_NONE) {
            SetBacklight(true);
        }

        /* 清除所有OSD和临时图层 */
        Layer_ClearAll(&mLayerManager);

        /* 释放动态加载的图片资源 */
        for (int i = 0; i < LCD_SUPPORT_IMAGE_CNT; i++) {
            ReleaseImageResource(&mExtraImage[i]);
        }

        /* 需要更新背景图 */
        needUpdateBg = 1;
        mNeedUpdateBg = 0;

        /* 更新上一状态记录 */
        mPrevWorkState = mWorkState;
    }
    else if (currentMainState == MAIN_STATE_IDLE) {
        /* 待机状态检查是否需要更新背景，并且实时更新时间 */
        if (mNeedUpdateBg == 1) {
            needUpdateBg = 1;
            mNeedUpdateBg = 0;
        }
    }
    else {
        /* 非待机状态只需更新一次，无需实时刷新 */
        return;
    }

    /* 主状态switch：先判断主任务，再进入相应的子任务处理 */
    switch (currentMainState) {
        case MAIN_STATE_BIND:
            /* 绑定模式：根据子状态处理 */
            result = HandleBindMode(mWorkState.subState.bind, needUpdateBg);
            break;

        case MAIN_STATE_IDLE:
            /* 待机模式：直接处理 */
            result = HandleIdleState(needUpdateBg);
            break;

        case MAIN_STATE_CALL:
            /* 通话模式：根据子状态处理 */
            result = HandleCallMode(mWorkState.subState.call, needUpdateBg);
            break;

        case MAIN_STATE_REMIND:
            /* 提醒模式：直接处理 */
            result = HandleRemindingState(needUpdateBg);
            break;

        case MAIN_STATE_SLEEP:
            /* 休眠状态：关闭背光，保持最后画面 */
            SetBacklight(false);
            return;

        case MAIN_STATE_NONE:
            /* 无效状态：无需处理 */
            LCD_WRAN("Current state is NONE, skip processing\n");
            return;

        default:
            LCD_WRAN("Unknown main state: %d\n", currentMainState);
            break;
    }

    if (result) {
        /* 有变化，合成图层 */
        Layer_Composite(&mLayerManager);

        /* 刷新到LCD */
        Layer_Refresh(&mLayerManager);
    }
}

/* ==================== 对外接口 ==================== */
int ScreenManager::WakeUp()
{
    int ret = 0;

    pthread_mutex_lock(&mMutex);
    if (mWorkState.mainState == MAIN_STATE_SLEEP) {
        WorkState_t idleState = {MAIN_STATE_IDLE, {.none = SUB_STATE_NONE}};
        SetWorkState(idleState);
        ret = 1;
    }
    mLastActivityTime = GetMonotonicTimeSec();
    pthread_mutex_unlock(&mMutex);

    LCD_INF("WakeUp ret[%d]\n", ret);
    return ret;
}

int ScreenManager::GetBackgroundImageNumber(uint32_t *number)
{
    *number = mBGNumber;
    return 0;
}

int ScreenManager::SetBackgroundImageNumber(uint32_t number)
{
    if (number > BACKGROUNDNUM_MAX) {
        LCD_INF("Err: BGNumber set to %u\n", number);
        return -1;
    }

    if (number == mBGNumber) {
        return 0;
    }

    pthread_mutex_lock(&mMutex);
    mBGNumber = number;
    mLastActivityTime = GetMonotonicTimeSec();
    pthread_mutex_unlock(&mMutex);

    /* 保存配置 */
    pCConfig->SetValue(SCREEN_CFG_SECTION, "BGNumber", mBGNumber);

    LCD_INF("BGNumber set to %u\n", number);
    return 0;
}

int ScreenManager::GetSleepTimeout(uint32_t *seconds)
{
    *seconds = mSleepTimeout;
    return 0;
}

int ScreenManager::SetSleepTimeout(uint32_t seconds)
{
    if (seconds == mSleepTimeout) {
        return 0;
    }

    pthread_mutex_lock(&mMutex);
    mSleepTimeout = seconds;
    if (mWorkState.mainState == MAIN_STATE_SLEEP) {
        WorkState_t idleState = {MAIN_STATE_IDLE, {.none = SUB_STATE_NONE}};
        SetWorkState(idleState);
    }
    mLastActivityTime = GetMonotonicTimeSec();
    pthread_mutex_unlock(&mMutex);

    /* 保存配置 */
    pCConfig->SetValue(SCREEN_CFG_SECTION, "SleepTimeout", mSleepTimeout);

    LCD_INF("Sleep timeout set to %u seconds\n", seconds);
    return 0;
}

int ScreenManager::GetBrightness(uint32_t *level)
{
    *level = mBrightness;
    return 0;
}

int ScreenManager::SetBrightness(uint32_t level)
{
    if (level == mBrightness) {
        return 0;
    }

    pthread_mutex_lock(&mMutex);
    mBrightness = (level > BRIGHTNESS_MAX ? BRIGHTNESS_MAX : level);
    if (mWorkState.mainState == MAIN_STATE_SLEEP) {
        WorkState_t idleState = {MAIN_STATE_IDLE, {.none = SUB_STATE_NONE}};
        SetWorkState(idleState);
    } else {
        ZWSys_LCDSetBackLightLevel(mBrightness);
    }
    mLastActivityTime = GetMonotonicTimeSec();
    pthread_mutex_unlock(&mMutex);

    /* 保存配置 */
    pCConfig->SetValue(SCREEN_CFG_SECTION, "Brightness", mBrightness);

    LCD_INF("Brightness set to %u\n", mBrightness);
    return 0;
}

/* status: 0连接中 1绑定中 2添加成功 3添加失败 */
int ScreenManager::ExternSetDevBindConnetStatus(int status)
{
    if (status >= DEV_BIND_STATUS_MAX) {
        LCD_ERR("Invalid bind status: %d\n", status);
        return -1;
    }
    pthread_mutex_lock(&mMutex);

    WorkState_t nextState;
    bool validTransition = false;
    MainState_t curMainState = mWorkState.mainState;
    BindSubState_t curBindSubState = mWorkState.subState.bind;
    LCD_INF("Received external binding status: main=%d, sub=%d, new=%d\n",
            curMainState, curBindSubState, status);

    switch ((DevBindStatus_t)status) {
        case DEV_BIND_STATUS_CONNECTING:
            /* 未配网状态可进入连接中 */
            if (curMainState == MAIN_STATE_BIND && curBindSubState == BIND_SUB_UNBOUND) {
                nextState.mainState = MAIN_STATE_BIND;
                nextState.subState.bind = BIND_SUB_CONNECTING;
                validTransition = true;
            }
            break;

        case DEV_BIND_STATUS_BINDING:
            /* 连接中状态可进入绑定中 */
            if (curMainState == MAIN_STATE_BIND && curBindSubState == BIND_SUB_CONNECTING) {
                nextState.mainState = MAIN_STATE_BIND;
                nextState.subState.bind = BIND_SUB_BINDING;
                validTransition = true;
            }
            break;

        case DEV_BIND_STATUS_SUCCESS:
            /* 绑定中状态可进入绑定成功 */
            if (curMainState == MAIN_STATE_BIND && curBindSubState == BIND_SUB_BINDING) {
                nextState.mainState = MAIN_STATE_BIND;
                nextState.subState.bind = BIND_SUB_SUCCESS;
                validTransition = true;
            }
            break;

        case DEV_BIND_STATUS_FAILED:
            /* 失败可从任意绑定相关状态转入 */
            if (curMainState == MAIN_STATE_BIND &&
                (curBindSubState == BIND_SUB_CONNECTING ||
                 curBindSubState == BIND_SUB_BINDING ||
                 curBindSubState == BIND_SUB_UNBOUND)) {
                nextState.mainState = MAIN_STATE_BIND;
                nextState.subState.bind = BIND_SUB_FAILED;
                validTransition = true;
            }
            break;

        default:
            break;
    }

    if (validTransition) {
        mLastActivityTime = GetMonotonicTimeSec();
        SetWorkState(nextState);
        LCD_INF("Bind state transition: [main=%d,sub=%d] -> [main=%d,sub=%d]\n",
                curMainState, curBindSubState, nextState.mainState, nextState.subState.bind);
    }

    pthread_mutex_unlock(&mMutex);
    return 0;
}

/* status: 0提醒开始 1提醒结束 */
int ScreenManager::ExternSetScheduleRemindStatus(int status)
{
    if (status >= REMIND_STATUS_MAX) {
        LCD_ERR("Invalid remind status: %d\n", status);
        return -1;
    }
    pthread_mutex_lock(&mMutex);

    bool validTransition = false;
    MainState_t curMainState = mWorkState.mainState;
    LCD_INF("SetScheduleRemindStatus: current main=%d, new=%d\n", curMainState, status);

    switch (status) {
        case REMIND_STATUS_START:
            /* 待机或休眠状态可进入提醒模式 */
            if (curMainState == MAIN_STATE_IDLE || curMainState == MAIN_STATE_SLEEP) {
                WorkState_t remindState = {MAIN_STATE_REMIND, {.none = SUB_STATE_NONE}};
                SetWorkState(remindState);
                validTransition = true;
            }
            break;

        case REMIND_STATUS_END:
            /* 提醒模式可返回待机 */
            if (curMainState == MAIN_STATE_REMIND) {
                WorkState_t idleState = {MAIN_STATE_IDLE, {.none = SUB_STATE_NONE}};
                SetWorkState(idleState);
                validTransition = true;
            }
            break;

        default:
            break;
    }

    if (validTransition) {
        mLastActivityTime = GetMonotonicTimeSec();
        LCD_INF("Remind state transition: %d -> %d\n", curMainState, GetMainState());
    }

    pthread_mutex_unlock(&mMutex);
    return 0;
}

/* status: 0呼叫中 1视频通话中 2语音通话中 3呼叫未接听 4呼叫挂断 5等待接听 */
int ScreenManager::ExternSetCallStatus(int status)
{
    if (status >= CALL_STATUS_MAX) {
        LCD_ERR("Invalid call status: %d\n", status);
        return -1;
    }
    pthread_mutex_lock(&mMutex);

    WorkState_t nextState;
    bool validTransition = false;
    MainState_t curMainState = mWorkState.mainState;
    CallSubState_t curCallSubState = mWorkState.subState.call;
    LCD_INF("SetCallStatus: current main=%d, sub=%d, new=%d\n",
            curMainState, curCallSubState, status);

    switch (status) {
        case CALL_STATUS_CALLING:
            /* 待机、休眠、未接听状态可进入呼叫中 */
            if (curMainState == MAIN_STATE_IDLE || curMainState == MAIN_STATE_SLEEP ||
                (curMainState == MAIN_STATE_CALL && curCallSubState == CALL_SUB_NOANSWER)) {
                nextState.mainState = MAIN_STATE_CALL;
                nextState.subState.call = CALL_SUB_CALLING;
                validTransition = true;
            }
            break;

        case CALL_STATUS_WAITANSWER:
            /* 呼叫中可进入等待接听 */
            if (curMainState == MAIN_STATE_CALL && curCallSubState == CALL_SUB_CALLING) {
                nextState.mainState = MAIN_STATE_CALL;
                nextState.subState.call = CALL_SUB_WAITANSWER;
                validTransition = true;
            }
            break;

        case CALL_STATUS_VIDEO:
            /* 待机、休眠、呼叫中、语音通话可进入视频通话 */
            if (curMainState == MAIN_STATE_IDLE || curMainState == MAIN_STATE_SLEEP ||
                (curMainState == MAIN_STATE_CALL &&
                 (curCallSubState == CALL_SUB_CALLING || curCallSubState == CALL_SUB_WAITANSWER || curCallSubState == CALL_SUB_VOICE))) {
                nextState.mainState = MAIN_STATE_CALL;
                nextState.subState.call = CALL_SUB_VIDEO;
                validTransition = true;
            }
            break;

        case CALL_STATUS_VOICE:
            /* 待机、休眠、呼叫中、视频通话可进入语音通话 */
            if (curMainState == MAIN_STATE_IDLE || curMainState == MAIN_STATE_SLEEP ||
                (curMainState == MAIN_STATE_CALL &&
                 (curCallSubState == CALL_SUB_CALLING || curCallSubState == CALL_SUB_WAITANSWER || curCallSubState == CALL_SUB_VIDEO))) {
                nextState.mainState = MAIN_STATE_CALL;
                nextState.subState.call = CALL_SUB_VOICE;
                validTransition = true;
            }
            break;

        case CALL_STATUS_NO_ANSWER:
            /* 呼叫中可进入未接听 */
            if (curMainState == MAIN_STATE_CALL && curCallSubState == CALL_SUB_WAITANSWER) {
                nextState.mainState = MAIN_STATE_CALL;
                nextState.subState.call = CALL_SUB_NOANSWER;
                validTransition = true;
            }
            break;

        case CALL_STATUS_HANGUP:
            /* 通话中任意状态可挂断返回待机 */
            if (curMainState == MAIN_STATE_CALL) {
                nextState.mainState = MAIN_STATE_IDLE;
                nextState.subState.none = SUB_STATE_NONE;
                validTransition = true;
            }
            break;

        default:
            break;
    }

    if (validTransition) {
        mLastActivityTime = GetMonotonicTimeSec();
        SetWorkState(nextState);
        LCD_INF("Call state transition: [main=%d,sub=%d] -> [main=%d,sub=%d]\n",
                curMainState, curCallSubState, nextState.mainState, nextState.subState.call);
    }

    pthread_mutex_unlock(&mMutex);
    return 0;
}

int ScreenManager::SetVideoCallStreamData(const char *dataBuf, uint32_t bufSize)
{
    if (dataBuf == NULL || bufSize == 0) {
        LCD_ERR("Invalid video data: buf=%p, size=%u\n", dataBuf, bufSize);
        return -1;
    }

    /* TODO: 将视频流数据送入显示驱动
     * 注意：视频通话状态下，OSD层应暂停更新或仅显示半透明覆盖层
     */
    pthread_mutex_lock(&mMutex);
    if (mWorkState.mainState == MAIN_STATE_CALL && mWorkState.subState.call == CALL_SUB_VIDEO) {
        /* 直接写屏到视频层，绕过OSD合成 */
        /* Lcd_WriteVideoFrame(dataBuf, bufSize); */
    }
    pthread_mutex_unlock(&mMutex);

    return 0;
}

int ScreenManager::SetBackgroundImageData(const char* imageData, uint32_t dataLen)
{
    if (imageData == NULL || dataLen == 0) {
        LCD_ERR("Invalid background data: buf=%p, len=%u\n", imageData, dataLen);
        return -1;
    }

    /* JPEG 数据大小限制 */
    if (dataLen > JPEG_MAX_IMAGE_SIZE) {
        LCD_WRAN("Background size mismatch: expected %u, got %u\n", JPEG_MAX_IMAGE_SIZE, dataLen);
        return -1;
    }

    int ret;
    unsigned int Width, Height;

    /* 校验是不是jpeg格式, 获取宽高 */
    ret = GetJpegDimensions(imageData, dataLen, &Width, &Height);
    if (ret != 0) {
        LCD_ERR("Not in jpeg format, size:%u, ret:%d\n", dataLen, ret);
        return -1;
    }

    /* 背景图宽高匹配 */
    if (Width != LCD_SCREEN_WIDTH || Height != LCD_SCREEN_HEIGHT) {
        LCD_ERR("Jpeg resolution mismatch, dev:%ux%u, usr:%ux%u\n", LCD_SCREEN_WIDTH, LCD_SCREEN_HEIGHT, Width, Height);
        return -1;
    }

    pthread_mutex_lock(&mMutex);
    /* 写入用户背景图 */
    FILE* file = fopen(LCD_USR_BACKGROUND_IMAGE_PATH, "wb");
    if (file != NULL)
    {
        if (fwrite(imageData, 1, dataLen, file) != dataLen)
        {
            remove(LCD_USR_BACKGROUND_IMAGE_PATH); // 失败删除
            LCD_ERR("fwrite %s failed! removed!\n", LCD_USR_BACKGROUND_IMAGE_PATH);
        }
        else
        {
            mNeedUpdateBg = 1;  //成功，通知更新背景图
        }
        fclose(file);
    }

    /* 休眠状态则唤醒 */
    if (mWorkState.mainState == MAIN_STATE_SLEEP) {
        WorkState_t idleState = {MAIN_STATE_IDLE, {.none = SUB_STATE_NONE}};
        SetWorkState(idleState);
    }
    mLastActivityTime = GetMonotonicTimeSec();
    pthread_mutex_unlock(&mMutex);

    LCD_WRAN("SetImageData End! dataLen:%u\n", dataLen);
    return 0;
}

/* ==================== 硬件控制==================== */
void ScreenManager::SetBacklight(bool on)
{
    LCD_INF("SetBacklight: %s -- %u\n", on ? "ON" : "OFF", mBrightness);

    if (on) {
        ZWSys_LCDSetBackLightLevel(mBrightness);
    } else {
        ZWSys_LCDSetBackLightLevel(0);
    }

    return;
}

/* ==================== 调试接口 ==================== */
int ScreenManager::DumpInfo()
{
    LCD_INF("=== ScreenManager Dump ===\n");
    LCD_INF("MainState: %d, SubState: %d\n",
            mWorkState.mainState,
            (mWorkState.mainState == MAIN_STATE_BIND) ? mWorkState.subState.bind :
            (mWorkState.mainState == MAIN_STATE_CALL) ? mWorkState.subState.call : 0);
    LCD_INF("PrevMainState: %d, PrevSubState: %d\n",
            mPrevWorkState.mainState,
            (mPrevWorkState.mainState == MAIN_STATE_BIND) ? mPrevWorkState.subState.bind :
            (mPrevWorkState.mainState == MAIN_STATE_CALL) ? mPrevWorkState.subState.call : 0);
    LCD_INF("StateEnterTime: %lld\n", mStateEnterTime);
    LCD_INF("SleepTimeout: %u\n", mSleepTimeout);
    LCD_INF("Brightness: %u\n", mBrightness);
    LCD_INF("LastActivityTime: %lld, CurTime: %lld\n", mLastActivityTime, GetMonotonicTimeSec());
    if (mLayerManager.hasDirtyRect) {
        LCD_INF("DirtyRect: (%u,%u,%u,%u)\n",
                mLayerManager.dirtyX, mLayerManager.dirtyY,
                mLayerManager.dirtyW, mLayerManager.dirtyH);
    } else {
        LCD_INF("DirtyRect: none\n");
    }

    /* OSD信息 */
    for (int i = 0; i < LAYER_OSD_MAX; i++)
    {
        const OSD_Config_t& osd = mLayerManager.LayerOsdCfg[i];

        if (osd.enable) {
            LCD_INF("OSD[%d] text=%s xy[%hu-%hu] wh[%hu-%hu]\n", i, osd.TextStr, osd.x, osd.y, osd.osdPixelWidth, osd.osdPixelHeight);
        }
    }

    /* 图层信息 */
    for (int i = 0; i < LAYER_TYPE_MAX; i++)
    {
        const LayerInfo_t& layer = mLayerManager.layers[i];

        if (layer.state) {
            LCD_INF("Layer[%d] xy[%hu-%hu] wh[%hu-%hu]\n", i, layer.x, layer.y, layer.width, layer.height);
        }
    }

    return 0;
}

#endif
