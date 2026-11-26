/*!
 *\file   Resource.c
 *\brief  所有系统SDK使用的全局资源管理
 *\author fc
 *\date	  2015年7月2
 *\version 0.1
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <arpa/inet.h> 
#include <sys/sysinfo.h>
#include <dirent.h>

#include "xtrace.h"
#include "Common.h"
#include "opencore-amrnb/interf_enc.h"
#include "common_pub.h"

//-------------------------------------- 宏定义 -----------------------------------------------
/* 开主控sensor0时钟 */
#define CMD_ENABLE_I2C0_CLK "bspmd.l 0x11018440 > /dev/null; bspmm 0x11018440 0x4010 > /dev/null"

/* 开主控sensor1时钟 */
#define CMD_ENABLE_I2C2_CLK "bspmd.l 0x11018460 > /dev/null; bspmm 0x11018460 0x4010 > /dev/null"

#define CMD_I2C_READ_0X0_0X80 "i2c_read 0x0 0x80 0x0D 0x0D 1 1"
#define CMD_I2C_READ_0X0_0X84 "i2c_read 0x0 0x84 0x0D 0x0D 1 1"
#define CMD_I2C_READ_0X0_0X88 "i2c_read 0x0 0x88 0x0D 0x0D 1 1"
#define CMD_I2C_READ_0X0_0X8C "i2c_read 0x0 0x8C 0x0D 0x0D 1 1"

#define CMD_I2C_READ_0X2_0X80 "i2c_read 0x2 0x80 0x0D 0x0D 1 1"
#define CMD_I2C_READ_0X2_0X84 "i2c_read 0x2 0x84 0x0D 0x0D 1 1"
#define I2C_READ_ERROR_STRING "CMD_I2C_READ error!"

//-------------------------------------- 全局资源变量申明 -----------------------------------------------
SDK_ResInfo_t				g_SDKResInfo;
SystemInfo_t				g_SystemInfo;

static ZWSDK_PresetConfig s_sdk_presetconfig = {};

static int g_MaxSensorCnt = SUPPORT_MAX_SENSOR_COUNT; //默认为真三目

static const stuSpeechRemindTypeMap speechRemindTypeMap[] = {
    {eSpeechType_Remind_1,  SPEECH_REMINDER_SCHEDULE_1,     SPEECH_REMIND_SCHEDULE_CUSTOM_FILE1},
    {eSpeechType_Remind_2,  SPEECH_REMINDER_SCHEDULE_2,     SPEECH_REMIND_SCHEDULE_CUSTOM_FILE2},
    {eSpeechType_Remind_3,  SPEECH_REMINDER_SCHEDULE_3,     SPEECH_REMIND_SCHEDULE_CUSTOM_FILE3},
    {eSpeechType_Remind_TEST,  SPEECH_REMINDER_SCHEDULE_TEST,     ""},
};

void SpeechRemindTypeMap(SpeechType_T nType, eSpeechRemindType *nSpeechType)
{
	for (int i = 0; i < sizeof(speechRemindTypeMap) / sizeof(speechRemindTypeMap[0]); i++)
	{
		if (nType == speechRemindTypeMap[i].eAlarmType)
		{
			*nSpeechType = speechRemindTypeMap[i].nSpeechType;
		}
	}
}

void SpeechRemindNameMap(SpeechType_T nType, char *pCustomSpeechName)
{
	int i = 0;
	for (i = 0; i < sizeof(speechRemindTypeMap) / sizeof(speechRemindTypeMap[0]); i++)
	{
		if (nType == speechRemindTypeMap[i].eAlarmType)
		{
			memcpy(pCustomSpeechName, speechRemindTypeMap[i].sCustomSpeechFileName, sizeof(speechRemindTypeMap[i].sCustomSpeechFileName));
		}
	}
}

int HY_Res_GetMaxSensorCnt()
{
	return g_MaxSensorCnt;
}

int HY_Res_SysInitSensorCnt()
{
	g_MaxSensorCnt = SYSTEM_SUPPORT_CHANNEL;
	ZW_INFO("SysInitSensor End, g_MaxSensorCnt[%d]\n", g_MaxSensorCnt);
    return 0;
}

int HY_Res_GetSensorConfCorrect(int CheckCnt)
{
    int nValue = 0, nResult = 0;

    /* 判断成立说明i2c2上存在地址为0x88的i2c设备，即硬件上为真四目 */
    if (CheckCnt == 3)
    {
        if (IsStringInCommand(CMD_I2C_READ_0X0_0X88, I2C_READ_ERROR_STRING) == 0)
        {
            if (IsStringInCommand(CMD_I2C_READ_0X0_0X84, I2C_READ_ERROR_STRING) == 0)
            {
                nValue = -1;
            }
            else if (IsStringInCommand(CMD_I2C_READ_0X2_0X84, I2C_READ_ERROR_STRING) == 0)
            {
                nValue = -1;
            }
            else
            {
                nValue = IsStringInCommand(CMD_I2C_READ_0X0_0X80, I2C_READ_ERROR_STRING);
                nValue |= IsStringInCommand(CMD_I2C_READ_0X2_0X80, I2C_READ_ERROR_STRING);
            }
        }
        else
        {
            nValue = -1;
        }    
    }
    else if (CheckCnt == 4)
    {
        if (IsStringInCommand(CMD_I2C_READ_0X0_0X8C, I2C_READ_ERROR_STRING) == 0)
        {
            nValue = IsStringInCommand(CMD_I2C_READ_0X0_0X80, I2C_READ_ERROR_STRING);
            nValue |= IsStringInCommand(CMD_I2C_READ_0X2_0X84, I2C_READ_ERROR_STRING);
            nValue |= IsStringInCommand(CMD_I2C_READ_0X2_0X80, I2C_READ_ERROR_STRING);
        }
        else
        {
            nValue = -1;
        }
    }
    else
    {
        nValue = -1;
    }

    nResult = ((nValue == 0) ? 1 : 0);

    ZW_DEBUG("value[%d] result[%d] CheckCnt[%d]\n", nValue, nResult, CheckCnt);
	return nResult;
}

int HY_Res_SDK_GetTimeSinceBoot()
{
	struct sysinfo info;

	sysinfo(&info);
	return info.uptime;
}

int HY_Res_SDK_Check()
{
    if (g_SDKResInfo.m_bSDKInit != SDK_INIT_MAGIC)
        return -1;
    else
        return 0;
}

void    HY_Res_SDK_Usleep(unsigned int ntime)
{
    usleep(ntime);
}

/* 自定义读写锁初始化 */
int HY_Res_RWLock_init(HY_RWLodk_t *pLockInfo)
{
    XTRACE_MUTEX_INIT(&pLockInfo->m_Lock, NULL);
    pLockInfo->m_LockState 		= HY_UNLOCK;
    pLockInfo->m_ReadLockNum 	= 0;
    pLockInfo->m_WriteLockNum 	= 0;
    return 0;
}

int HY_Res_RWLock_destory(HY_RWLodk_t *pLockInfo)
{
    XTRACE_MUTEX_DESTORY(&pLockInfo->m_Lock);
    return 0;
}

/* 自定义读写锁读加锁 */
int HY_Res_RD_Lock(HY_RWLodk_t *pLockInfo)
{
    while (1)
    {
        XTRACE_MUTEX_LOCK(&pLockInfo->m_Lock);

        /* 有写锁需求尚未满足或者正在写锁状态，不允许加锁 */
        if ((pLockInfo->m_WriteLockNum > 0)
        || (pLockInfo->m_LockState == HY_WRLOCK))
        {
//            ZW_DEBUG("[SDK] Zview_RD_Lock wait: WLock = %d, RLock = %d, LockState = %d\n", pLockInfo->m_WriteLockNum, pLockInfo->m_ReadLockNum, pLockInfo->m_LockState);
            XTRACE_MUTEX_UNLOCK(&pLockInfo->m_Lock);
            HY_Res_SDK_Usleep(80000);
            continue;
        }

        pLockInfo->m_LockState = HY_RDLOCK;
        pLockInfo->m_ReadLockNum ++;
        XTRACE_MUTEX_UNLOCK(&pLockInfo->m_Lock);
        break;
    }
    return 0;
}

/* 自定义读写释放读锁 */
int HY_Res_RD_UnLock(HY_RWLodk_t *pLockInfo)
{
    XTRACE_MUTEX_LOCK(&pLockInfo->m_Lock);
    if (pLockInfo->m_ReadLockNum > 0)
        pLockInfo->m_ReadLockNum --;
    else
        ZW_DEBUG("[SDK] HY_RWLodk_t RDLockNum Err\n");
    if (pLockInfo->m_ReadLockNum == 0)
        pLockInfo->m_LockState = HY_UNLOCK;
    XTRACE_MUTEX_UNLOCK(&pLockInfo->m_Lock);
    return 0;
}

/* 自定义读写锁写加锁 */
int HY_Res_WR_Lock(HY_RWLodk_t *pLockInfo)
{
    /* 如果没有加锁则加锁，否则阻止之后的读加锁 */
    XTRACE_MUTEX_LOCK(&pLockInfo->m_Lock);
    if (pLockInfo->m_LockState == HY_UNLOCK)
    {
        pLockInfo->m_LockState = HY_WRLOCK;
        XTRACE_MUTEX_UNLOCK(&pLockInfo->m_Lock);
        return 0;
    }
    pLockInfo->m_WriteLockNum ++;			//阻止之后的读加锁
    XTRACE_MUTEX_UNLOCK(&pLockInfo->m_Lock);
    HY_Res_SDK_Usleep(20000);

    while (1)
    {
        XTRACE_MUTEX_LOCK(&pLockInfo->m_Lock);
        if (pLockInfo->m_LockState == HY_UNLOCK)
        {
            pLockInfo->m_LockState = HY_WRLOCK;
            pLockInfo->m_WriteLockNum --;			//允许之后的读加锁
            XTRACE_MUTEX_UNLOCK(&pLockInfo->m_Lock);
            break;
        }
//        ZW_DEBUG("[SDK] Zview_WR_Lock wait: WLock = %d, RLock = %d, LockState = %d\n", pLockInfo->m_WriteLockNum, pLockInfo->m_ReadLockNum, pLockInfo->m_LockState);
        XTRACE_MUTEX_UNLOCK(&pLockInfo->m_Lock);
        HY_Res_SDK_Usleep(40000);		
    }
    return 0;
}

/* 自定义读写释放写锁 */
int HY_Res_WR_UnLock(HY_RWLodk_t *pLockInfo)
{
    XTRACE_MUTEX_LOCK(&pLockInfo->m_Lock);
    pLockInfo->m_LockState = HY_UNLOCK;
    XTRACE_MUTEX_UNLOCK(&pLockInfo->m_Lock);
    return 0;
}

int HY_Res_SDK_Lock()
{
    if (g_SDKResInfo.m_bSDKInit != SDK_INIT_MAGIC)
    return -1;

    while (1)
    {
        XTRACE_MUTEX_LOCK(&g_SDKResInfo.m_Lock.m_Lock);

        /* 有写锁需求尚未满足或者正在写锁状态，不允许加锁 */
        if ((g_SDKResInfo.m_Lock.m_WriteLockNum > 0)
        || (g_SDKResInfo.m_Lock.m_LockState == HY_WRLOCK))
        {
            ZW_DEBUG("[SDK] Zview_RD_Lock wait: WLock = %d, RLock = %d, LockState = %d\n", g_SDKResInfo.m_Lock.m_WriteLockNum, g_SDKResInfo.m_Lock.m_ReadLockNum, g_SDKResInfo.m_Lock.m_LockState);
            XTRACE_MUTEX_UNLOCK(&g_SDKResInfo.m_Lock.m_Lock);
            HY_Res_SDK_Usleep(80000);
            continue;
        }

        g_SDKResInfo.m_Lock.m_LockState = HY_RDLOCK;
        g_SDKResInfo.m_Lock.m_ReadLockNum ++;
        XTRACE_MUTEX_UNLOCK(&g_SDKResInfo.m_Lock.m_Lock);
        break;
    }
    return 0;
}

int HY_Res_SDK_UnLock()
{
    XTRACE_MUTEX_LOCK(&g_SDKResInfo.m_Lock.m_Lock);
    if (g_SDKResInfo.m_Lock.m_ReadLockNum > 0)
        g_SDKResInfo.m_Lock.m_ReadLockNum --;
    else
        ZW_DEBUG("[SDK] HY_RWLodk_t RDLockNum Err\n");
    if (g_SDKResInfo.m_Lock.m_ReadLockNum == 0)
    g_SDKResInfo.m_Lock.m_LockState = HY_UNLOCK;
    XTRACE_MUTEX_UNLOCK(&g_SDKResInfo.m_Lock.m_Lock);
    return 0;
}

int HY_Res_InitSDKInfo()
{
	memset(&g_SDKResInfo, 0, sizeof(SDK_ResInfo_t));
	return 0;
}

int HY_Res_GetSDKInfo(SDK_ResInfo_t **pInfo)
{
    *pInfo = &g_SDKResInfo;
    return 0;
}

int HY_Res_InitSystemInfo()
{
	memset(&g_SystemInfo, 0, sizeof(SystemInfo_t));
	return 0;
}

/* 获取系统信息句柄 */
int HY_Res_GetSystemInfo(SystemInfo_t **pInfo)
{
    *pInfo = &g_SystemInfo;
    return 0;
}

int HY_Res_CreatThread(ThreadInfo_t *pThInfo, int Pri, void *(*start_routine)(void*))
{
    pthread_attr_t		attr;
    struct sched_param 	param;
    int					RetValue;
    SystemInfo_t		*pSysInfo;

    HY_Res_GetSystemInfo(&pSysInfo);

    if (Pri == 0)
    {
        pthread_attr_init(&attr);
        pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
        pthread_attr_getschedparam(&attr, &param);
        param.sched_priority = 0;
        pthread_attr_setschedparam(&attr, &param);
        RetValue = pthread_create(&pThInfo->m_ThreadId, &attr, start_routine, pThInfo);
        pthread_attr_destroy(&attr);
    }	
    else
    {
        pthread_attr_init(&attr);
        pthread_attr_setschedpolicy(&attr, SCHED_RR );
        pthread_attr_getschedparam(&attr, &param);
        param.sched_priority = Pri;
        pthread_attr_setschedparam(&attr, &param);
        RetValue = pthread_create(&pThInfo->m_ThreadId, &attr, start_routine, pThInfo);
        pthread_attr_destroy(&attr);
    }

    if (RetValue != 0)
        pThInfo->m_ThreadId = 0;
    return RetValue;
}

int HY_Res_DestroyThread(ThreadInfo_t *pThInfo)
{
    if (pThInfo->m_ThreadId != 0)
    {
        pThInfo->m_Exit = 1;
        pthread_join(pThInfo->m_ThreadId, NULL);
        pThInfo->m_ThreadId = 0;
    }
    return 0;
}

int HY_Res_GetFormatSize(int width, int height, unsigned long *puMaxRate)
{
    if (NULL != puMaxRate)
    {
        if (width < 704)
        {
			if (height == 512)
			{
				*puMaxRate = MAX_BPS_512P;
			}
			else
			{
            	*puMaxRate = MAX_BPS_360P;
			}
        }
        else if (width < 1280)
        {
            *puMaxRate = MAX_BPS_D1;
        }
        else if (width < 1920)
        {
            *puMaxRate = MAX_BPS_HD720P;
        }        
        else if (width < 2304)
        {
            *puMaxRate = MAX_BPS_HD1080P;
        }
        else
        {
        	if (height == 1440)
        	{
				*puMaxRate = MAX_BPS_4M;
        	}
        	else if (height == 1920)
        	{
				*puMaxRate = MAX_BPS_5M;
        	}
        	else
        	{
            	*puMaxRate = MAX_MAIN_BPS;
            }
        }
    }

    return 0;
}

int HY_Res_SetPresetConfig(ZWSDK_PresetConfig PresetConfig)
{
    ZW_INFO("updating sdk preset config bLangFlag=[%d] eLang=[%d] nAlgoType=[%d]\n", 
		PresetConfig.bLangFlag, 
		PresetConfig.eLang,
		PresetConfig.nAlgoType);
    memcpy(&s_sdk_presetconfig, &PresetConfig, sizeof(s_sdk_presetconfig));

    return 0;
}

int HY_Res_GetPresetConfig(ZWSDK_PresetConfig *pPresetConfig)
{
	memcpy(pPresetConfig, &s_sdk_presetconfig, sizeof(s_sdk_presetconfig));

    return 0;
}

/* 获取当前系统时间 */
int system_time(char * time_str, size_t str_len)
{
	if (str_len < 20) 
	{
		perror("time_str buffer size to low");
		return -1;
	}

	time_t now = time((time_t *)NULL);
	struct tm time_now = { 0 }; 
    localtime_r(&now, &time_now);

	memset(time_str, '\0', str_len);
	snprintf(time_str, str_len, "%4d-%02d-%02d %02d:%02d:%02d",
			time_now.tm_year + 1900,
			time_now.tm_mon + 1,
			time_now.tm_mday,
			time_now.tm_hour,
			time_now.tm_min,
			time_now.tm_sec);

	return (strlen(time_str));
}

int GetCurTimeHour()
{
    struct	tm  tnow_r;
    time_t		CurTime = 0;

    time(&CurTime);
    localtime_r (&CurTime, &tnow_r);

    return tnow_r.tm_hour;
}

unsigned long long get_ms()
{
    struct timeval time_v = { 0 };
    gettimeofday(&time_v, NULL);
    return (unsigned long long)time_v.tv_sec * 1000 + time_v.tv_usec / 1000;
}

unsigned long long get_s()
{
    struct timeval time_v = { 0 };
    gettimeofday(&time_v, NULL);
    return (unsigned long long)time_v.tv_sec;
}

int get_integer_from_file(const char *filename, int *value)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return -1;
    }

    if (fscanf(file, "%d", value) != 1) {
        fprintf(stderr, "fscanf error\n");
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}

/***
 * @description: G711U音频文件转码AMR文件格式
 * @param {char*} input_file 待转换的G711U文件
 * @param {char*} output_file 转换后的AMR文件
 * @return {SINT32} 失败-1，成功返回转换后文件大小
 */
int convert_g711u_to_amr(const char* input_file, const char* output_file)
{
    int totalsize = 0;

    FILE* in_fp = fopen(input_file, "rb");
    if (!in_fp) {
        ZW_ERROR("fopen [%s] failed!\n", input_file);
        return -1;
    }

    FILE* out_fp = fopen(output_file, "wb");
    if (!out_fp) {
        fclose(in_fp);
        ZW_ERROR("fopen [%s] failed!\n", output_file);
        return -1;
    }

    // 写入AMR文件头
    const char amr_header[] = "#!AMR\n";
    fwrite(amr_header, 1, sizeof(amr_header) - 1, out_fp);

    // 初始化AMR编码器
    void* amr_encoder = Encoder_Interface_init(0); // 0=禁用DTX
    if (!amr_encoder) {
        fclose(in_fp);
        fclose(out_fp);
        ZW_ERROR("Encoder_Interface_init failed!\n");
        return -1;
    }

    // 设置目标比特率 (MR59=5.9kbps)
    enum Mode target_mode = MR59;

    // 处理缓冲区
    unsigned char g711_buffer[G711U_FRAME_SIZE];
    short pcm_buffer[G711U_FRAME_SIZE];
    unsigned char amr_frame[AMRNB_FRAME_SIZE];

    size_t bytes_read;
    while ((bytes_read = fread(g711_buffer, 1, G711U_FRAME_SIZE, in_fp)) > 0) {
        // 转换G711U到PCM
        G711U_DECODE_PCM(pcm_buffer, g711_buffer, bytes_read);

        // 如果读取不足一帧，填充静音
        if (bytes_read < G711U_FRAME_SIZE) {
            memset(pcm_buffer + bytes_read, 0, (G711U_FRAME_SIZE - bytes_read) * sizeof(short));
        }

        // AMR编码
        int frame_size = Encoder_Interface_Encode(amr_encoder, target_mode, pcm_buffer, amr_frame, 0);
        if (frame_size <= 0) {
            ZW_ERROR("AMR Encode failed: %d\n", frame_size);
            continue; // 继续处理下一帧
        }

        // 写入AMR帧
        fwrite(amr_frame, 1, frame_size, out_fp);
        totalsize += frame_size;
    }

    // 清理资源
    Encoder_Interface_exit(amr_encoder);
    fclose(in_fp);
    fclose(out_fp);

    return totalsize;
}

int GetSpeechNamePath(char *SpeechFilePath)
{
	 DIR *dir;
	 struct dirent *entry;
	 int file_exists = 0;

	 // 初始化原始路径（可根据实际需求修改）
	 snprintf(SpeechFilePath, 128,SPEECH_FILE_PATH_OLD);

	 // 尝试打开目录
	 if ((dir = opendir(SPEECH_FILE_PATH)) != NULL) {
		 // 遍历目录内容
		 while ((entry = readdir(dir)) != NULL) {
			 // 跳过"."和".."目录
			 if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
				 file_exists = 1;
				 break;
			 }
		 }
		 closedir(dir);
	 }

	 // 根据检查结果更新路径
	 if (file_exists) {
		 snprintf(SpeechFilePath, 128, SPEECH_FILE_PATH);
		 ZW_INFO("SpeechNamePath : %s\n", SpeechFilePath);
	 } else {
		 // 保持初始值或更新为备用路径
		 ZW_INFO("SpeechNamePath: %s\n", SpeechFilePath);
	 }

	 return 0;
}

