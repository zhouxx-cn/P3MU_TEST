
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <sys/times.h>
#include <fcntl.h>
#include <unistd.h>

#include "LogManager.h"
#include "mysystem.h"
#include "common_pub.h"

#define gettid()  syscall(SYS_gettid)






typedef struct tagLogModule
{
	unsigned int module;
	char* name;
}stLogModule;


const static stLogModule g_LogModuleArray[] = 
{
	{LOG_MODULE_OPEN,	"[OPEN]"},
	{LOG_MODULE_PUB,	"[PUB]"},
	{LOG_MODULE_MAIN,	"[MAIN]"},
    {LOG_MODULE_AVL,   	"[AVL]"},
	{LOG_MODULE_4G,		"[4G]"},
	{LOG_MODULE_NET,	"[NET]"},	
	{LOG_MODULE_MPU,	"[MPU]"},	
	{LOG_MODULE_ZWSDK,	"[ZWSDK]"},
	{LOG_MODULE_JSONSYS,"[JSONSYS]"},
	{LOG_MODULE_SD,     "[SD]"},
    {LOG_MODULE_PTZ,    "[PTZ]"},
    {LOG_MODULE_AUTH,   "[AUTH]"},
    {LOG_MODULE_LIGHT,  "[LIGHT]"},
    {LOG_MODULE_SYS,    "[SYS]"},
    {LOG_MODULE_LCD,    "[LCD]"},
};



static unsigned int g_log_level     = DEBUG_LOG_LEVEL_INFO;
static unsigned int g_module_status = LOG_MODULE_ALL;



int get_log_level(void)
{
	return g_log_level;
}

int set_log_level(int logLevel)
{
	if((logLevel < DEBUG_LOG_LEVEL_NONE) || (logLevel > DEBUG_LOG_LEVEL_ALL))
	{
		printf("%s =>log_level(%d) input err, Usage: level(0~5)\n", __FUNCTION__, logLevel);
		return -1;
	}

	g_log_level = logLevel;

	char command[128] = {0};
	snprintf(command, sizeof(command), "echo %d > %s", g_log_level, "/mnt/mtd/log_level");
	mysystem(command);

	return 0;
}

int init_log_level(void)
{
	unsigned int log_level     = DEBUG_LOG_LEVEL_INFO;
	unsigned int module_status = LOG_MODULE_OPEN;
	char buf[128] = {0};
	char fileVal[16] = {0};

	do
	{
		if(0 == access("/mnt/mtd/log_level", 0))
		{
			/* 1. log_level */
			snprintf(buf, sizeof(buf)-1, "%s", "cat /mnt/mtd/log_level");
			memset(fileVal, 0, sizeof(fileVal));

			if(0 == ShellCmdOut(buf, fileVal, sizeof(fileVal) - 1))
			{
				log_level = atoi(fileVal);
			}

			g_log_level = log_level;
		}
		else
		{
			set_log_level(log_level);
		}

		if(0 == access("/mnt/mtd/log_module", 0))
		{
			/* 2. log_module */
			snprintf(buf, sizeof(buf)-1, "%s", "cat /mnt/mtd/log_module");
			memset(fileVal, 0, sizeof(fileVal));

			if(0 == ShellCmdOut(buf, fileVal, sizeof(fileVal) - 1))
			{
				module_status = atoi(fileVal);
			}

			g_module_status = module_status;
		}
		else
		{
			//SetLogModuleEnable(LOG_MODULE_ALL, 0);
            //SetLogModuleEnable(LOG_MODULE_AUTH, 1);
		}
	}
	while(0);

	LOG_W(LOG_MODULE_OPEN, "%s => g_log_level: %d, g_module_status: %#x\n", __FUNCTION__, g_log_level, g_module_status);

	//SetLogEnable(log_enable);
	return 0;
}


int GetLogModuleEnable(LOG_MODULE_E module)
{
	int i = 0;
	for (i = 0; i < sizeof(g_LogModuleArray)/sizeof(g_LogModuleArray[0]); i++ )
	{
		unsigned int modulebit = g_LogModuleArray[i].module;
		if (module == modulebit)
		{
			return ((g_module_status & module) == module) ? 1 : 0;
		}
	}

	return -1;
}

int SetLogEnable(int enable)
{
	if(enable)
	{
		/* 恢复console */
		freopen("/dev/ttyAMA0", "w", stdout);
		freopen("/dev/ttyAMA0", "w", stderr);

		OPEN_LOG_W("open printf log!!!\n");

		if(0 != access("/mnt/mtd/en_app_prn", 0))
		{
			mysystem("touch /mnt/mtd/en_app_prn");
		}
	}
	else
	{
		OPEN_LOG_W("close printf log!!!\n");

		/* 重定向console到/dev/null */
		freopen("/dev/null", "w", stdout);
		freopen("/dev/null", "w", stderr);

		if(0 == access("/mnt/mtd/en_app_prn", 0))
		{
			mysystem("rm /mnt/mtd/en_app_prn");
		}
	}

	return 0;
}

int SetLogModuleEnable(LOG_MODULE_E module, int enable)
{
	int i = 0;
	for (i = 0; i < sizeof(g_LogModuleArray)/sizeof(g_LogModuleArray[0]); i++ )
	{
		unsigned int modulebit = g_LogModuleArray[i].module;
		if (module == modulebit || module == LOG_MODULE_ALL)
		{
			if (enable)
			{
				g_module_status |= modulebit;
			}
			else
			{
				g_module_status &= ~modulebit;	   
			} 	
		}
	}

	char command[128] = {0};
	snprintf(command, sizeof(command), "echo %d > %s", g_module_status, "/mnt/mtd/log_module");
	mysystem(command);

	return 0;
}

#define LOG_LINE_MAX_SIZE    512
#define FORMAT_ALL_SIZE      256
#define DISPLAY_SIZE         32
#define TIMESTAMP_SIZE       32
#define FILE_INFO_SIZE       64
#define TID_SIZE            16

void DebugLog(int logLevel, unsigned int module, const char *file, int line, const char * format, ...)
{
    // 1.模块和日志级别过滤(合并条件)
    if (module != LOG_MODULE_OPEN && module != LOG_MODULE_PUB && module != LOG_MODULE_MAIN)
    {
        if ((g_module_status & module) != module)
        {
            return;
        }

        if( logLevel > g_log_level)
        {
            return;
        }
    }

    // 2.使用合理的缓冲区大小
    char outPutString[LOG_LINE_MAX_SIZE] = {0};
    char formatAll[FORMAT_ALL_SIZE] = {0};
    char display[DISPLAY_SIZE] = {0};

    #if 0 /* 显示日期 */
    UINT64 currTimeInMSec = getSystemTimeInMSec();
    time_t curSysTime = currTimeInMSec/1000;
    struct tm *tm_CurSysTime = gmtime(&curSysTime);

    char strCurrTimeStamp[32] = {0};
    snprintf(strCurrTimeStamp, 32, "%04d-%02d-%02d %02d:%02d:%02d.%03d ", 
        tm_CurSysTime->tm_year+1900, tm_CurSysTime->tm_mon+1, tm_CurSysTime->tm_mday,
        tm_CurSysTime->tm_hour, tm_CurSysTime->tm_min, tm_CurSysTime->tm_sec, (unsigned int)currTimeInMSec%1000);

    strncat(formatAll, strCurrTimeStamp, FORMAT_ALL_SIZE);
    #else /* 显示启动到当前时间 */
    if (logLevel == DEBUG_LOG_LEVEL_ERROR)
    {
        snprintf(formatAll, FORMAT_ALL_SIZE, "[%llu] ", GetSysTickCountMs());
    }
    #endif

    // 3.日志级别标记(使用查表法提高效率)
    static const char* levelTags[] = {"", "[E] ", "[W] ", "[I] ", "[D] "};
    static const char* colorCodes[] = {"", "\033[1;31m", "\033[1;33m", "\033[1;32m", "\033[1;36m"};

    int levelIndex = (logLevel < DEBUG_LOG_LEVEL_NONE || logLevel >= DEBUG_LOG_LEVEL_ALL) ? 0 : logLevel;
    strncat(formatAll, levelTags[levelIndex], FORMAT_ALL_SIZE - strlen(formatAll) - 1);
    strncat(display, colorCodes[levelIndex], DISPLAY_SIZE - strlen(display) - 1);

    // 4.模块名称
    const char* name = GetLogModuleName(module);
    if (name)
    {
        strncat(formatAll, name, FORMAT_ALL_SIZE - strlen(formatAll) - 1);
        strncat(formatAll, " ", FORMAT_ALL_SIZE - strlen(formatAll) - 1);
    }

    // 5.线程ID
    char strTid[TID_SIZE] = {0};
    snprintf(strTid, TID_SIZE, "[T:%03d] ", (int)gettid());
    strncat(formatAll, strTid, FORMAT_ALL_SIZE - strlen(formatAll) - 1);

    // 6.文件信息
    if (NULL != file)
    {
        const char *p = strrchr(file, '/');
        (NULL == p) ? p = file : p++;

        char fileInfo[FILE_INFO_SIZE] = {0};
        snprintf(fileInfo, sizeof(fileInfo), "[%s:%d] ", p, line);
        strncat(formatAll, fileInfo, FORMAT_ALL_SIZE - strlen(formatAll) - 1);
    }

    // 7.主日志内容（防止溢出）
    size_t remaining = FORMAT_ALL_SIZE - strlen(formatAll) - 1;
    if (remaining >= 0) {
        strncat(formatAll, format, remaining);
    }
    else
    {
        printf("[ERR] Log Debug Over 256 size\n");
        printf("%s\n", format);
        return;
    }

    // 8.可变参数处理
    va_list args;
    va_start(args, format);
    vsnprintf(outPutString, LOG_LINE_MAX_SIZE, formatAll, args);
    va_end(args);

    if (!strrchr(outPutString, '\n'))
    {
        printf("%s%s%s\n", display, outPutString, "\033[0m");
    }
    else
    {
        printf("%s%s%s", display, outPutString, "\033[0m");
    }
}

static int HexToStr(char *dst, int size, const char *src, int len)
{
	if (NULL == src || 0 >= len  
		|| NULL == dst || 0 >= size)
	{
		return -1;
	}

	int i = 0;
	int length = 0;
	for (i = 0; i < len; i++)
	{
		if (length > 0)  
		{
            if (length < size - 1) 
			{
                dst[length] = ' ';
                dst[length + 1] = '\0';
                length++;
            } else 
			{
                break;  
            }
        }
	
        int written = snprintf(dst + length, size - length, "%02X", (unsigned char)src[i]);
        if (written < 0 || written >= size - length) 
		{
            break;  
        }
		length += written;

		if (length > size-3)
		{
			break;
		}
	}


    if (length < size) 
	{
        dst[length] = '\0';
    } else if (size > 0) 
	{
        dst[size - 1] = '\0';
    }

	return length;
}


void HexDebugLog(int level, unsigned int module, const char *file, int line, const char *pDesc, const char *data, int len)
{
	if (module == LOG_MODULE_OPEN || g_log_level >= DEBUG_LOG_LEVEL_INFO)
	{
		char buf[1024] = {0};
		int oLen = HexToStr(buf, sizeof(buf),data,len);
		DebugLog(level, module, file, line, "[%s][LEN=%d][HexLen=%d][%s]", pDesc, len, oLen, buf);
	}
	return;
}


char* GetLogModuleName(unsigned int module)
{
	int i = 0;
	for (i = 0; i < sizeof(g_LogModuleArray)/sizeof(g_LogModuleArray[0]); i++ )
	{
		unsigned int modulebit = g_LogModuleArray[i].module;
		if (module == modulebit)
		{
			return g_LogModuleArray[i].name;
		}
	}

	return NULL;
}



UINT64 GetSysTickCountMs(void)
{
	UINT64 tickMs = 0;
	struct timespec abstime;
    /*CLOCK_BOOTTIME可以在SOC休眠时计时, 比CLOCK_MONOTONIC计时更精准*/
	clock_gettime(CLOCK_BOOTTIME, &abstime);
	tickMs = abstime.tv_sec;
	tickMs *= 1000;
	tickMs += abstime.tv_nsec/1000000;
	return (tickMs);
}

int __SystemGetSecCount(void)
{
	int iSecCount = 0;
	int iTck = 0;
	clock_t tTimes = 0;
	
	iTck = sysconf(_SC_CLK_TCK);         /*获取系统时钟(1秒里有多少个)*/
	tTimes = times(NULL);            /*进程运行到此时的系统时钟数(总的)*/ 
	iSecCount = (int)(tTimes/(double)iTck * 1000/1000);

	return iSecCount;
}






