#if defined (__IS_SUPPORT_REMINDER_SCHEDULE__)
#ifndef __CREMINDER_SCHEDULE_H__
#define __CREMINDER_SCHEDULE_H__

#include "zwsdk.h"
#include "PAR/CConfig.h"
#include "pthreadmacro.h"

#define MAX_REMINDER_COUNT (3)                  // 最多3条提醒
#define DEFAULT_BROADCAST_VOL (95)              // 默认提醒播报音量
#define MAX_REMINDER_CUSTOM_SPEECH_COUNT (3)    // 最多3条自定义提醒音频

// 单个提醒项的数据结构
typedef struct tagReminderItem
{
    int  id;                        // 身份标识，用于和客户端交互，从1开始，0表示该数据无效
    int  enable;                    // 是否启用（闹铃开关）
    int  weekMask;                  // 星期掩码，位0=周一，位6=周日
    int  hour;                      // 小时 0-23
    int  minute;                    // 分钟 0-59
    char name[64];                  // 提醒名称
	char voicename[128];	        // 语音类型名称
    SpeechRemindInfo_t audioCfg;    // 音频配置参数
} tagReminderItem;

// 定时提醒语音播报自定义数据结构
typedef struct tagCusSpeechInfo
{
    int iID;            /* 语音序号ID, 从1开始(表示自定义语音1...), 0无效 */
    char iName[128];    /* 语音名称 */
} tagCusSpeechInfo;

class CReminderSchedule
{
public:
    // 获取单例实例
    static CReminderSchedule* GetInstance();
    // 释放实例
    static int ReleaseInstance();

    // 添加定时提醒自定义语音信息
    int AddCustomSpeech(const tagCusSpeechInfo* item);

    // 删除定时提醒自定义语音信息
    int DeleteCustomSpeech(int nID);

    // 获取定时提醒自定义语音信息
    int GetCustomSpeech(int nID, tagCusSpeechInfo* item);

    // 获取最大定时提醒自定义语音数量
    int GetMaxCustomSpeechCount() { return MAX_REMINDER_CUSTOM_SPEECH_COUNT; }

    // 测试播放定时提醒语音
    int TestPlaySpeech(int nType);

    // 启用/禁用指定索引的提醒
    int EnableReminder(int nID, int enable);

    // 新增提醒
    int AddReminder(const tagReminderItem* item);

    // 删除指定索引的提醒，item->id决定
    int DeleteReminder(int nID);

    // 修改指定索引的提醒项，item->id决定
    int ModifyReminder(const tagReminderItem* item);

    // 获取指定索引的提醒项，item->id决定
    int GetReminder(int nID, tagReminderItem* item);

    // 外部中断当前提醒（按键）
    int InterruptReminder();

    // 获取最大提醒数量
    int GetMaxReminderCount() { return MAX_REMINDER_COUNT; }

    // 获取当前已有提醒数量
    int GetCurReminderCount() { return m_curVaildReminders; }

    // 设置外部提醒触发回调（可选，不设置则内部调用执行函数）
    typedef void (*ReminderCallback)(const tagReminderItem* item);
    int SetReminderCallback(ReminderCallback cb);

private:
    CReminderSchedule();
    ~CReminderSchedule();

    // 禁止拷贝
    CReminderSchedule(const CReminderSchedule&);
    CReminderSchedule& operator=(const CReminderSchedule&);

    // 初始化（加载配置）
    int Init();

    // 从配置文件加载所有提醒
    int LoadParam();

    // 保存单个自定义提醒语音信息到配置文件
    int SaveCusSpeechInfoToCfg(int index, const tagCusSpeechInfo* item);
    
    // 保存指定提醒项到配置文件
    int SaveReminderToCfg(int index, const tagReminderItem* item);

    // 内部执行函数
    void InternalCallback(int index, const tagReminderItem* item);

    // 工作线程函数
    static void* WorkThread(void* arg);
    int StartWorkThread();
    void StopWorkThread();

    // 检查当前时间是否匹配某个提醒项，若匹配则触发播放
    void CheckAndTrigger();

private:
    static CReminderSchedule* m_pInstance;

	//配置文件
	CConfig *pCConfig;

    tagCusSpeechInfo m_CusSpeechInfos[MAX_REMINDER_CUSTOM_SPEECH_COUNT];  //自定义提醒语音数组
    tagReminderItem m_reminders[MAX_REMINDER_COUNT];    // 提醒数组
    int m_lastTriggerDate[MAX_REMINDER_COUNT];          // 上次触发日期（YYYYMMDD）
    int m_bInited;                                      // 初始化标志
    int m_curVaildReminders;                            // 当前已有提醒数量
    int m_bWorkThreadRun;                               // 工作线程运行标志
    pthread_t m_workThread;                             // 工作线程ID
    pthread_mutex_t mutex;

    ReminderCallback m_externcallback;                  // 外部回调（可选）
};

#endif // __CREMINDER_SCHEDULE_H__
#endif // __IS_SUPPORT_REMINDER_SCHEDULE__
