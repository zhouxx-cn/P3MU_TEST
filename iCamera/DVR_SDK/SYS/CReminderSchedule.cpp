#if defined (__IS_SUPPORT_REMINDER_SCHEDULE__)
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#include "debug.h"
#include "CReminderSchedule.h"
#include "MPU_LCD_API.h"

// 静态成员初始化
CReminderSchedule* CReminderSchedule::m_pInstance = NULL;
static const char* REMIND_CFG_SECTION = "ReminderCfg";

// 将星期掩码转换为英文缩写字符串（如 "Mon Tue Wed"）
static void WeekMaskToString(int weekMask, char* buf, int bufSize)
{
    const char* weeks[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
    int first = 1;
    buf[0] = '\0';

    for (int i = 0; i < 7; i++) {
        if (weekMask & (1 << i)) {
            if (!first) {
                strncat(buf, " ", bufSize - strlen(buf) - 1);
            }
            strncat(buf, weeks[i], bufSize - strlen(buf) - 1);
            first = 0;
        }
    }
    if (first) {
        strncpy(buf, "None", bufSize);
    }
}

// 将音频类型转换为英文描述
static const char* SpeechTypeToString(unsigned char type)
{
    switch (type) {
        case 0:  return "Custom";
        case 1:  return "Canon";
        case 2:  return "Alice";
        case 3:  return "Ringtone";
        case 4:  return "Alarm";
        case 5:  return "Bird";
        default: return "Unknown";
    }
}

// 打印单个提醒项
void PrintReminderItem(int index, const tagReminderItem* item)
{
    if (!item) {
        return;
    }

    char weekBuf[64] = {0};
    WeekMaskToString(item->weekMask, weekBuf, sizeof(weekBuf));

    printf("===== Reminder Info [%d] =====\n", index);
    printf("Name        : %s\n", item->name);    
    printf("Id          : %d\n", item->id);
    printf("Enabled     : %s\n", item->enable ? "Yes" : "No");
    printf("Weekdays    : %s\n", weekBuf);
    printf("Time        : %02d:%02d\n", item->hour, item->minute);
    printf("Audio Config:\n");
    printf("  Type      : %s (%d)\n", SpeechTypeToString(item->audioCfg.remindSpeechType), item->audioCfg.remindSpeechType);
    printf("  Enable    : %s\n", item->audioCfg.enable ? "Yes" : "No");
    printf("  Times     : %d\n", item->audioCfg.broadCastTimes);
    printf("  Volume    : %d\n", item->audioCfg.broadCastVolume);
    printf("=============================\n");
}

// 单例获取
CReminderSchedule* CReminderSchedule::GetInstance()
{
    if (NULL == m_pInstance)
    {
        m_pInstance = new CReminderSchedule();
        if (m_pInstance)
        {
            m_pInstance->Init();
            m_pInstance->StartWorkThread();
        }
    }
    return m_pInstance;
}

// 释放实例
int CReminderSchedule::ReleaseInstance()
{
    if (m_pInstance)
    {
        m_pInstance->StopWorkThread();
        delete m_pInstance;
        m_pInstance = NULL;
    }
    return 0;
}

// 构造函数
CReminderSchedule::CReminderSchedule()
    : m_bInited(0)
    , m_curVaildReminders(0)
    , m_bWorkThreadRun(0)
    , m_workThread(0)
    , m_externcallback(NULL)
{
    memset(m_CusSpeechInfos, 0, sizeof(m_CusSpeechInfos));
    memset(m_reminders, 0, sizeof(m_reminders));
    for (int i = 0; i < MAX_REMINDER_COUNT; ++i)
    {
        m_lastTriggerDate[i] = 0;
    }
    INIT_MUTEX(mutex);
}

// 析构
CReminderSchedule::~CReminderSchedule()
{
    StopWorkThread();
    UNINIT_MUTEX(mutex);
}

// 初始化：加载配置
int CReminderSchedule::Init()
{
	pCConfig = CConfig::GetInstance();

    LoadParam();

    m_bInited = 1;
    return 0;
}

// 从配置文件加载所有提醒
int CReminderSchedule::LoadParam()
{
    char key[128];

    for (int i = 0; i < MAX_REMINDER_CUSTOM_SPEECH_COUNT; i++)
    {
        // 读取Speech id
        memset(key, 0, sizeof(key));
        snprintf(key, sizeof(key), "CustomSpeech_%d_Id", i);
        m_CusSpeechInfos[i].iID= pCConfig->GetValue(REMIND_CFG_SECTION, key, 0);

        // 读取名称
        memset(key, 0, sizeof(key));
        snprintf(key, sizeof(key), "CustomSpeech_%d_Name", i);
        pCConfig->GetValue(REMIND_CFG_SECTION, key, "", m_CusSpeechInfos[i].iName, sizeof(m_CusSpeechInfos[i].iName));
    }

    for (int i = 0; i < MAX_REMINDER_COUNT; i++)
    {
		// 读取id
        memset(key, 0, sizeof(key));
        snprintf(key, sizeof(key), "Reminder_%d_Id", i);
        m_reminders[i].id = pCConfig->GetValue(REMIND_CFG_SECTION, key, 0);
        if (m_reminders[i].id <= 0 || m_reminders[i].id > MAX_REMINDER_COUNT) {
            m_reminders[i].id = 0;
        } else {
            m_curVaildReminders++;
        }

        // 读取名称
        memset(key, 0, sizeof(key));
        snprintf(key, sizeof(key), "Reminder_%d_Name", i);
		pCConfig->GetValue(REMIND_CFG_SECTION, key, "", m_reminders[i].name, sizeof(m_reminders[i].name));
        // 读取启用
        memset(key, 0, sizeof(key));
        snprintf(key, sizeof(key), "Reminder_%d_Enable", i);
        m_reminders[i].enable = pCConfig->GetValue(REMIND_CFG_SECTION, key, 0); 
        // 读取星期掩码
        memset(key, 0, sizeof(key));
        snprintf(key, sizeof(key), "Reminder_%d_WeekMask", i);
        m_reminders[i].weekMask = pCConfig->GetValue(REMIND_CFG_SECTION, key, 0);
        // 读取小时
        memset(key, 0, sizeof(key));
        snprintf(key, sizeof(key), "Reminder_%d_Hour", i);
        m_reminders[i].hour = pCConfig->GetValue(REMIND_CFG_SECTION, key, 0);
        // 读取分钟
        memset(key, 0, sizeof(key));
        snprintf(key, sizeof(key), "Reminder_%d_Minute", i);
        m_reminders[i].minute = pCConfig->GetValue(REMIND_CFG_SECTION, key, 0);
        // 读取音频配置
       	m_reminders[i].audioCfg.enable = m_reminders[i].enable;        
        memset(key, 0, sizeof(key));        
        snprintf(key, sizeof(key), "Reminder_%d_AudioVolume", i);
    	m_reminders[i].audioCfg.broadCastVolume = pCConfig->GetValue(REMIND_CFG_SECTION, key, 95);
        memset(key, 0, sizeof(key));
        snprintf(key, sizeof(key), "Reminder_%d_AudioTimes", i);
    	m_reminders[i].audioCfg.broadCastTimes = pCConfig->GetValue(REMIND_CFG_SECTION, key, 1);
        memset(key, 0, sizeof(key));
        snprintf(key, sizeof(key), "Reminder_%d_AudioType", i);
    	m_reminders[i].audioCfg.remindSpeechType = pCConfig->GetValue(REMIND_CFG_SECTION, key, 1);
        // 设置音频配置
    	ZWSys_SetRemindSpeechCfg(ZWSys_EncChannelOpen(0), i, &(m_reminders[i].audioCfg));
    }

    return 0;
}

// 保存单个自定义提醒语音信息到配置文件
int CReminderSchedule::SaveCusSpeechInfoToCfg(int index, const tagCusSpeechInfo* item)
{
    if (index < 0 || index >= MAX_REMINDER_CUSTOM_SPEECH_COUNT || !item)
        return -1;

    char key[64];

	memset(key, 0, sizeof(key));
    snprintf(key, sizeof(key), "CustomSpeech_%d_Id", index);
    pCConfig->SetValue(REMIND_CFG_SECTION, key, item->iID);

    memset(key, 0, sizeof(key));
    snprintf(key, sizeof(key), "CustomSpeech_%d_Name", index);
    pCConfig->SetValue(REMIND_CFG_SECTION, key, item->iName);

    return 0;
}

// 保存单个提醒到配置文件
int CReminderSchedule::SaveReminderToCfg(int index, const tagReminderItem* item)
{
    if (index < 0 || index >= MAX_REMINDER_COUNT || !item)
        return -1;

    char key[64];

	memset(key, 0, sizeof(key));
    snprintf(key, sizeof(key), "Reminder_%d_Id", index);
    pCConfig->SetValue(REMIND_CFG_SECTION, key, item->id);

	memset(key, 0, sizeof(key));
    snprintf(key, sizeof(key), "Reminder_%d_Enable", index);
    pCConfig->SetValue(REMIND_CFG_SECTION, key, item->enable);

    memset(key, 0, sizeof(key));
    snprintf(key, sizeof(key), "Reminder_%d_WeekMask", index);    
    pCConfig->SetValue(REMIND_CFG_SECTION, key, item->weekMask);

	memset(key, 0, sizeof(key));
    snprintf(key, sizeof(key), "Reminder_%d_Hour", index);    
	pCConfig->SetValue(REMIND_CFG_SECTION, key, item->hour);

    memset(key, 0, sizeof(key));
    snprintf(key, sizeof(key), "Reminder_%d_Minute", index);
    pCConfig->SetValue(REMIND_CFG_SECTION, key, item->minute);

    memset(key, 0, sizeof(key));
    snprintf(key, sizeof(key), "Reminder_%d_Name", index);
    pCConfig->SetValue(REMIND_CFG_SECTION, key, item->name);

	memset(key, 0, sizeof(key));
    snprintf(key, sizeof(key), "Reminder_%d_AudioType", index);
    pCConfig->SetValue(REMIND_CFG_SECTION, key, item->audioCfg.remindSpeechType);

	memset(key, 0, sizeof(key));
    snprintf(key, sizeof(key), "Reminder_%d_AudioTimes", index);
    pCConfig->SetValue(REMIND_CFG_SECTION, key, item->audioCfg.broadCastTimes);

    memset(key, 0, sizeof(key));
    snprintf(key, sizeof(key), "Reminder_%d_AudioVolume", index);
    pCConfig->SetValue(REMIND_CFG_SECTION, key, item->audioCfg.broadCastVolume);

    //pCConfig->SaveFileReal();
    return 0;
}

// 添加定时提醒自定义语音信息
int CReminderSchedule::AddCustomSpeech(const tagCusSpeechInfo* item)
{
    if (!item) 
	{
        CSYS_ERR("[AddCustomSpeech] input item is NULL\n");
        return -1;
    }

    if (item->iID <= 0 || item->iID > MAX_REMINDER_CUSTOM_SPEECH_COUNT)
    {
        CSYS_ERR("[AddCustomSpeech] failed, id out of range [%d]\n", item->iID);
        return -1;
    }

    int index = item->iID-1;

	LOCK(mutex);

    // 复制到内存
    memcpy(&m_CusSpeechInfos[index], item, sizeof(tagCusSpeechInfo));

    // 保存到配置文件
    SaveCusSpeechInfoToCfg(index, &m_CusSpeechInfos[index]);

	UNLOCK(mutex);
    return 0;
}

// 删除定时提醒自定义语音信息
int CReminderSchedule::DeleteCustomSpeech(int nID)
{
    if (nID <= 0 || nID > MAX_REMINDER_CUSTOM_SPEECH_COUNT)
    {
        CSYS_ERR("[DeleteCustomSpeech] failed, id out of range [%d]\n", nID);
        return -1;
    }

    int index = nID-1;

	LOCK(mutex);

    // 清空内存结构
    memset(&m_CusSpeechInfos[index], 0, sizeof(tagCusSpeechInfo));

    // 保存到配置文件
    SaveCusSpeechInfoToCfg(index, &m_CusSpeechInfos[index]);

	UNLOCK(mutex);	
    return 0;
}

// 获取定时提醒自定义语音信息
int CReminderSchedule::GetCustomSpeech(int nID, tagCusSpeechInfo* item)
{
    if (!item) 
	{
        CSYS_ERR("[GetCustomSpeech] input item is NULL\n");
        return -1;
    }

	if (nID <= 0 || nID > MAX_REMINDER_CUSTOM_SPEECH_COUNT)
	{
		CSYS_ERR("[GetCustomSpeech] id out of range [%d]\n", nID);
		return -1;
	}

    int index = nID-1;

	LOCK(mutex);

    // 复制信息
    memcpy(item, &m_CusSpeechInfos[index], sizeof(tagCusSpeechInfo));

	UNLOCK(mutex);
    return 0;
}

// 测试播放定时提醒语音
int CReminderSchedule::TestPlaySpeech(int nType)
{
	if (nType < 0)
	{
		CSYS_ERR("[TestPlaySpeech] Failed!! nType [%d]\n", nType);
		return -1;
	}

	LOCK(mutex);

    /* 打断当前定时提醒音频 */
    ZWSys_SetRemindHangupState(1);

    /* 设置测试音频参数 */
    SpeechRemindInfo_t audioCfg;
    audioCfg.broadCastTimes = 1;
    audioCfg.enable = 1;
    audioCfg.broadCastVolume = DEFAULT_BROADCAST_VOL;
    audioCfg.remindSpeechType = nType;
    ZWSys_SetRemindSpeechCfg(ZWSys_EncChannelOpen(0), eSpeechType_Remind_TEST, &audioCfg);

    /* 播放测试音频 */
    ZWSys_SetSpeechContent(SPEECH_REMINDER_SCHEDULE_TEST);

	UNLOCK(mutex);

    CSYS_WRAN("[TestPlaySpeech] nType[%d]\n", nType);
    return 0;
}

// 设置提醒
int CReminderSchedule::AddReminder(const tagReminderItem* item)
{
    if (!item) 
	{
        CSYS_ERR("[AddReminder] input item is NULL\n");
        return -1;
    }

	LOCK(mutex);
    // 查找空闲槽位（id == 0）
    int index = -1;
    for (int i = 0; i < MAX_REMINDER_COUNT; i++)
    {
        if (m_reminders[i].id == 0) 
		{
            index = i;
            break;
        }
    }
    if (index == -1) 
	{	
        UNLOCK(mutex);  
        CSYS_ERR("[AddReminder] no free slot, cur-max[%d-%d]\n", m_curVaildReminders, MAX_REMINDER_COUNT);
        return -1;
    }

    // 复制到内存
    memcpy(&m_reminders[index], item, sizeof(tagReminderItem));
    m_curVaildReminders++;
    m_reminders[index].id = m_curVaildReminders;

    //设置音频参数（音频enable同步提醒enable）
    m_reminders[index].audioCfg.enable = m_reminders[index].enable;
    m_reminders[index].audioCfg.broadCastVolume = DEFAULT_BROADCAST_VOL;
    ZWSys_SetRemindSpeechCfg(ZWSys_EncChannelOpen(0), index, &(m_reminders[index].audioCfg));

    // 保存到配置文件
    SaveReminderToCfg(index, &m_reminders[index]);

    // 重置该提醒的触发日期（下次会重新检查）
    m_lastTriggerDate[index] = 0;

	UNLOCK(mutex);	
    return 0;
}

// 删除指定索引的提醒，item->id决定
int CReminderSchedule::DeleteReminder(int nID)
{
	int i;
    int index = -1;

    if (nID <= 0 || nID > MAX_REMINDER_COUNT)
    {
        CSYS_ERR("[DeleteReminder] Error ID [%d]", nID);
        return -1;
    }

	LOCK(mutex);
    for (i = 0; i < MAX_REMINDER_COUNT; i++)
    {
        if (m_reminders[i].id == nID) 
		{
            index = i;
			break;
        }
        continue;
    }
    if (index == -1) 
	{	
        UNLOCK(mutex);
        CSYS_ERR("[DelReminder] No Reminder with ID was found! [%d]", nID);
        return -1;
    }

    // 清空内存结构
    memset(&m_reminders[index], 0, sizeof(tagReminderItem));
    m_curVaildReminders--;

    //设置音频参数
    ZWSys_SetRemindSpeechCfg(ZWSys_EncChannelOpen(0), index, &(m_reminders[index].audioCfg));

    // 保存空配置到文件
    SaveReminderToCfg(index, &m_reminders[index]);

	// 将元素m_reminders[i].id修改
    for (i = 0; i < MAX_REMINDER_COUNT; i++)
	{
	    if (m_reminders[i].id > nID)
        {
            m_reminders[i].id--;

            // 保存空配置到文件
            SaveReminderToCfg(i, &m_reminders[i]);
        }
    }

	UNLOCK(mutex);
    return 0;
}

// 修改指定索引的提醒项，item->id决定
int CReminderSchedule::ModifyReminder(const tagReminderItem* item)
{
    if (!item) {
        CSYS_ERR("[ModifyReminder] input item is NULL\n");
        return -1;
    }

    if (item->id <= 0 || item->id > MAX_REMINDER_COUNT)
    {
        CSYS_ERR("[ModifyReminder] Error ID [%d]", item->id);
        return -1;
    }

	LOCK(mutex);
    // 查找修改槽位（id == 0）
    int index = -1;
    for (int i = 0; i < MAX_REMINDER_COUNT; i++)
    {
        if (m_reminders[i].id == item->id) 
		{
            index = i;
            break;
        }
    }
    if (index == -1) 
	{	
        UNLOCK(mutex);
        CSYS_ERR("[ModifyReminder] no find slot id[%d], cur-max[%d-%d]\n", item->id, m_curVaildReminders, MAX_REMINDER_COUNT);
        return -1;
    }

    // 复制到内存
    memcpy(&m_reminders[index], item, sizeof(tagReminderItem));

    //设置音频参数（音频enable同步提醒enable）
    m_reminders[index].audioCfg.enable = m_reminders[index].enable;
    m_reminders[index].audioCfg.broadCastVolume = DEFAULT_BROADCAST_VOL;
    ZWSys_SetRemindSpeechCfg(ZWSys_EncChannelOpen(0), index, &(m_reminders[index].audioCfg));

    // 保存到配置文件
    SaveReminderToCfg(index, item);

    // 重置该提醒的触发日期（下次会重新检查）
    m_lastTriggerDate[index] = 0;

	UNLOCK(mutex);
    return 0;
}

// 获取指定索引的提醒项，item->id决定
int CReminderSchedule::GetReminder(int nID, tagReminderItem* item)
{
	if (nID <= 0 || nID > MAX_REMINDER_COUNT)
	{
		CSYS_ERR("[GetReminder] id = %d error\n", nID);
		return -1;
	}

	LOCK(mutex);
    int index = -1;
    for (int i = 0; i < MAX_REMINDER_COUNT; i++)
    {
        if (m_reminders[i].id == nID)
        {
            index = i;
        }
        continue;
    }

    if (index == -1) {
        UNLOCK(mutex);
        return -1;
    }

    memcpy(item, &m_reminders[index], sizeof(tagReminderItem));

    if (m_reminders[index].audioCfg.remindSpeechType == 0) {
        memcpy(item->voicename, m_CusSpeechInfos[0].iName, sizeof(item->voicename));
    }
    else if (m_reminders[index].audioCfg.remindSpeechType == 6) {
        memcpy(item->voicename, m_CusSpeechInfos[1].iName, sizeof(item->voicename));
    }
    else if (m_reminders[index].audioCfg.remindSpeechType == 7) {
        memcpy(item->voicename, m_CusSpeechInfos[2].iName, sizeof(item->voicename));
    }

	UNLOCK(mutex);
    return 0;
}

// 启用/禁用提醒
int CReminderSchedule::EnableReminder(int nID, int enable)
{
    int index = -1;

    if (nID <= 0 || nID > MAX_REMINDER_COUNT)
    {
        CSYS_ERR("[EnableReminder] Error ID [%d]", nID);
        return -1;
    }

	LOCK(mutex);
    for (int i = 0; i < MAX_REMINDER_COUNT; i++)
    {
        if (m_reminders[i].id == nID)
        {
            index = i;
        }
        continue;
    }
    if (index == -1) 
	{	
        UNLOCK(mutex);
        CSYS_ERR("[EnableReminder] No Reminder with ID was found! [%d]", nID);
        return -1;
    }

    if (m_reminders[index].enable == enable)
    {
    	UNLOCK(mutex);
        return 0;
    }

    m_reminders[index].enable = enable ? 1 : 0;

    //设置音频参数    
    m_reminders[index].audioCfg.enable = m_reminders[index].enable;
    m_reminders[index].audioCfg.broadCastVolume = DEFAULT_BROADCAST_VOL;
    ZWSys_SetRemindSpeechCfg(ZWSys_EncChannelOpen(0), index, &(m_reminders[index].audioCfg));

    // 更新配置文件
    char key[64] = {0};
    snprintf(key, sizeof(key), "Reminder_%d_Enable", index);
    pCConfig->SetValue(REMIND_CFG_SECTION, key, m_reminders[index].enable);
    //pCConfig->SaveFileReal();

    UNLOCK(mutex);
    return 0;
}

// 外部中断当前提醒（按键）
int CReminderSchedule::InterruptReminder()
{
	int nRet = 0;

    if (m_externcallback) {
        /* TODO */
    }
    else {
        /* 内部提醒直接中断语音报警 */
        nRet = ZWSys_SetRemindHangupState(1);
    }

    return nRet;
}

// 设置提醒触发回调
int CReminderSchedule::SetReminderCallback(ReminderCallback cb)
{
    m_externcallback = cb;
    return 0;
}

// 内部执行函数
void CReminderSchedule::InternalCallback(int index, const tagReminderItem* item)
{
	int nRet = -1;
	int nCounter = 0;

    switch (index)
    {
        case 0:
            nRet = ZWSys_SetSpeechContent(SPEECH_REMINDER_SCHEDULE_1);
            break;
        case 1:
            nRet = ZWSys_SetSpeechContent(SPEECH_REMINDER_SCHEDULE_2);
            break;
        case 2:
            nRet = ZWSys_SetSpeechContent(SPEECH_REMINDER_SCHEDULE_3);
            break;
        default:
            break;
    }

    if (nRet == 0)
    {
        /* 定时提醒开始->LCD */
        MPU_LCD_SetScheduleRemindStatus(0);
        while (1)
        {
            sleep(1);
            if (eSpeechPlayIdle == ZWSys_GetSpeechPlayingStatus()) {
                break;
            }

            if (++nCounter > 60) {
                nCounter = 0;
                break;
            }
        }
        /* 定时提醒结束->LCD */
        MPU_LCD_SetScheduleRemindStatus(1);
    }

    CSYS_INF("[CReminderSchedule] PlayRing  index:%d\n", index);
}

// 工作线程函数（静态）
void* CReminderSchedule::WorkThread(void* arg)
{
    CReminderSchedule* pThis = (CReminderSchedule*)arg;
    CSYS_INF("[CReminderSchedule] Work thread started\n");

    while (pThis->m_bWorkThreadRun)
    {
        sleep(1);   // 每秒检查一次，时间匹配时检查日期避免重复触发

        if (!pThis->m_bInited)
            continue;

        // 执行检查
        pThis->CheckAndTrigger();
    }

    CSYS_INF("[CReminderSchedule] Work thread exit\n");
    return NULL;
}

// 启动工作线程
int CReminderSchedule::StartWorkThread()
{
    if (m_bWorkThreadRun)
        return 0;

    m_bWorkThreadRun = 1;
    int ret = pthread_create(&m_workThread, NULL, WorkThread, this);
    if (ret != 0)
    {
        CSYS_ERR("[CReminderSchedule] Failed to create work thread\n");
        m_bWorkThreadRun = 0;
        return -1;
    }
    // 设置为分离线程，无需join
    pthread_detach(m_workThread);
    return 0;
}

// 停止工作线程
void CReminderSchedule::StopWorkThread()
{
    m_bWorkThreadRun = 0;
    // 等待线程退出（如果已分离则不需要join，但可以加一点延时确保线程退出）
    usleep(200*1000);
}

// 检查当前时间并触发提醒
void CReminderSchedule::CheckAndTrigger()
{
    time_t now = time(NULL);
    struct tm tm_now;
    localtime_r(&now, &tm_now);

    int curHour = tm_now.tm_hour;
    int curMin = tm_now.tm_min;
    int curWday = tm_now.tm_wday;   // 0=Sunday, 1=Monday, ... 6=Saturday
    // 转换为我们的掩码：位0=周一，位6=周日
    int wdayMaskBit = (curWday == 0) ? 6 : (curWday - 1); // 周日是6
    int curDate = (tm_now.tm_year + 1900) * 10000 + (tm_now.tm_mon + 1) * 100 + tm_now.tm_mday;

    for (int i = 0; i < MAX_REMINDER_COUNT; ++i)
    {
        if (!m_reminders[i].enable)
            continue;   // 未启用

        // 检查星期掩码
        if (!(m_reminders[i].weekMask & (1 << wdayMaskBit)))
            continue;

        // 检查时间（精确到分钟）
        if (m_reminders[i].hour != curHour || m_reminders[i].minute != curMin)
            continue;

        // 检查当天是否已经触发过
        if (m_lastTriggerDate[i] == curDate)
            continue;

        // 触发提醒
        CSYS_INF("[CReminderSchedule] Trigger reminder[%d]: %s\n", i, m_reminders[i].name);

        // 如果设置了外部回调，调用它；否则使用内部播放函数
        if (m_externcallback) {
            m_externcallback(&m_reminders[i]);
        }
        else {
            InternalCallback(i, &m_reminders[i]);
        }

        // 记录本次触发日期
        m_lastTriggerDate[i] = curDate;
    }
}
#endif
