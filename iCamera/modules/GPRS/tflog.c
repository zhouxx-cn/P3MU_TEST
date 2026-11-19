/*******************************************************************************
     Copyright (c) 2020-2031, XXXX
--------------------------------------------------------------------------------
                              sdk_log.cpp
  Project Code: log
   Module Name: log.c
  Date Created: 2020.08.17
        Author: zhujiajia
   Description: 提供打印日志记录接口，可选打印到串口或指定的日志文件

--------------------------------------------------------------------------------
  Modification History
  DATE        NAME             DESCRIPTION
--------------------------------------------------------------------------------
  YYYY-MM-DD

*******************************************************************************/
#ifdef __SUPPORT_SAVE_TF_LOG__

#include "tflog.h"
#include "mysystem.h"
#include <unistd.h>

#define SDK_LOG_FILE          /* 日志文件记录宏 */


LOG_CB_S gstLogCb;

static int nlogFlag = 0;
static int nlogLoop = 0;
static int nInit = 0;
static int nAllow = 1;


/* 文件路径与枚举值对应，见LOG_F_UVC */
static const char * const szLogPath[LOG_F_BUTT] = {
    "1.log",
    "2.log",
    "3.log",
    "4.log",
    "5.log"
};

/* FIX:对应枚举顺序，见LEVEL_ERROR */
static const char * const szLevelPrefix[LEVEL_BUTT] = {
    "[ERR]",
    "[WARN]",
    "[LOG]",
    "[INFO]",
    "[DEBUG]", /* 调试用 */
};

/* @Description：初始化日志目录 */
static int LogDirInit(void)
{
	int iRet = 0;
	char cmd[128] = {0};
	
	//存在sd卡结点
	if (0 == access(TFCARD_NODE,F_OK))
	{
		//存在挂载目录
		if (0 == access(TFCARD_MOUNT,F_OK))
		{
			if (0 != access(LOG_PATH,F_OK))
			{
				memset(cmd,0,sizeof(cmd));
				snprintf(cmd,sizeof(cmd)-1,"mkdir -p %s",LOG_PATH);
				mysystem(cmd);
				
				nlogFlag = 0;
				nlogLoop = 0;
				memset(gstLogCb.stLogDescTbl,0,sizeof(gstLogCb.stLogDescTbl));
			}
		}
		else
		{
			iRet = -1;
		}
	}
	else
	{
		iRet = -1;
	}
	
	return iRet;
}

/* @Description：检测日志目录 */
static int LogDirCheck(void)
{
	int iRet = 0;
	
	if (0 != access(LOG_PATH,F_OK))
	{
		iRet = -1;
	}
	
	return iRet;
}


/* @Description：文件描述信息填充到内存 */
int SDK_LOG_UpdateFileDescrp(FILE* pFile)
{
    int iRet = 0;

    if (NULL == pFile)
    {
        return -1;
    }
    
    //if (0 == gstLogCb.stLogDescTbl[nlogFlag].iLined % 40)
    {
        /* 更新文件头部 */
        fseek(pFile, 0, SEEK_SET);
        fprintf(pFile,"magic=%x;version:%d;lined=%d;recover=%d\n", SDK_LOG_MAGIC, SDK_LOG_VER, 
                              gstLogCb.stLogDescTbl[nlogFlag].iLined, gstLogCb.stLogDescTbl[nlogFlag].u8RcovrCnt);
        fseek(pFile, 0, SEEK_END); /* 偏移到末尾 */
    }
    
    gstLogCb.stLogDescTbl[nlogFlag].iLined ++;
    
    return iRet;
}

int String2int(char *str)//字符串转数字 
{
    char flag = '+';//指示结果是否带符号 
    long res = 0;
    
    if(*str=='-')//字符串带负号 
    {
        ++str;//指向下一个字符 
        flag = '-';//将标志设为负号 
    } 
    //逐个字符转换，并累加到结果res 
    while(('\0' != *str) && *str>=48 && *str<=57)//如果是数字才进行转换，数字0~9的ASCII码：48~57 
    {
        res = 10*res+  *str++-48;//字符'0'的ASCII码为48,48-48=0刚好转化为数字0 
        //printf("[mytest]res = %d\r\n", res);
    } 
 
    if(flag == '-')//处理是负数的情况
    {
        res = -res;
    }
 
    return (int)res;
}

int SDK_LOG_GetLined(char *szBuff, int iLen)
{
    char LinedBuff[10];
    char *pcPtr = NULL;
    int j = 0;
    int iLined = 0;

    if (NULL == szBuff)
    {
        return 0;
    }
    
    pcPtr = szBuff;
    memset(LinedBuff, 0, sizeof(LinedBuff));
    
    if ('\0' == *pcPtr)
    {
        return 0;
    }
    while ('\n' != *pcPtr && '\0' != *pcPtr && (0 < iLen))
    {
        if ('l' == *pcPtr && 'i' == *(pcPtr+1) && 
             'n' == *(pcPtr+2) && 'e' == *(pcPtr+3))
         {
            break;
         }
         pcPtr++;
         iLen--;
    }

    if (20 > iLen) /* 未找到 */
    {
        return 0;
    }
    
    pcPtr += 6; /**/    
    while (';' != *(pcPtr + j) && (j < 6))
    {
        *(LinedBuff + j) = *(pcPtr + j);
        j++;
    }
    *(LinedBuff + j + 1) = '\0';
    
    j = 0;
    /* 取整数 */
    iLined = String2int(LinedBuff);
    return iLined;
}

/* @Description：读取日志文件描述 */
int SDK_LOG_ReadLogsFileDescrp()
{
    char szLogPathName[LOG_PATH_MAX_SZ] = {0};
    char szBuff[250];
    int iRet = 0;
    int i = 0;
    FILE* pFile = NULL;
    int bNewFile = 1;
    pthread_mutex_lock(&gstLogCb.stMutex);

    for (i = 0; i < LOG_F_BUTT; i++)
    {
        memset(szLogPathName, 0, LOG_PATH_MAX_SZ);
        memset(szBuff, 0, 250);
        bNewFile = 1;
        
        sprintf(szLogPathName,"%s/%s", LOG_PATH, szLogPath[i]);
        pFile = fopen(szLogPathName, "r+");
        if (NULL == pFile)
        {
            pFile = fopen(szLogPathName, "w+");
            if (NULL == pFile)
            {
                printf("open w+ %s failed\r\n",  szLogPathName);
                continue;
            }
        }
        else
        {
            bNewFile = 0;
        }
        
        if (bNewFile)
        {
            gstLogCb.stLogDescTbl[i].iLined = 0;
            fseek(pFile, 0, SEEK_SET);
            fprintf(pFile,"magic=%x;version:%d;lined=%d;recover=%d\n", SDK_LOG_MAGIC, SDK_LOG_VER, 
                                  gstLogCb.stLogDescTbl[i].iLined, gstLogCb.stLogDescTbl[i].u8RcovrCnt);
            fseek(pFile, 0, SEEK_END); /* 偏移到末尾 */
        }
        else
        {
            fgets(szBuff, 200, pFile);
            gstLogCb.stLogDescTbl[i].iLined = SDK_LOG_GetLined(szBuff, 200);
        }

        fclose(pFile);
        pFile = NULL;
        /* 解析Buff */
    }

    for (i = 0; i < LOG_F_BUTT; i++ )
    {
		if (gstLogCb.stLogDescTbl[i].iLined < LOG_FILE_LINE_MAX)
		{
			break;
		}

		nlogFlag++;
		if (nlogFlag >= 5)
		{
			nlogFlag = 0;
		}
    }
    
    pthread_mutex_unlock(&gstLogCb.stMutex);
    return iRet;
}


/* @brief：写入调试日志
   @param：u8Target      输出目标，文件或STDERR
           u8LogFile    指定输出到哪个文件
           u8PrintLevel 输出打印等级，由宏控制屏蔽等级，后续增加命令行设置
   @return ： NONE
**/
void write_tflog(unsigned char u8Target, unsigned char u8PrintLevel, const char *format, ...) 
{
#ifdef SDK_LOG_FILE
    va_list arg;
    //int nRet = 0;
    char szLogPathName[LOG_PATH_MAX_SZ] = {0};

	if (0 == nInit)
	{
		return;
	}
    
    if ( gstLogCb.iPrintLevel < u8PrintLevel )
    {
        return; /* 不在打印等级内 */
    }

    struct tm tm_log;
    time_t time_log = time(NULL);
    localtime_r(&time_log, &tm_log);
    
    if ( !(LOG_FILE & u8Target) )
    {
        return;
    }
    
    pthread_mutex_lock(&gstLogCb.stMutex);

	if (0 == nAllow)
	{
		pthread_mutex_unlock(&gstLogCb.stMutex);
		return;
	}

	//检测日志目录是否存在，可能中途识别不到卡
	if (0 != LogDirCheck())
	{
		if (0 != LogDirInit())
		{
			pthread_mutex_unlock(&gstLogCb.stMutex);
			return;
		}
	}    

    if ( LOG_FILE_LINE_MAX < gstLogCb.stLogDescTbl[nlogFlag].iLined )
    {
        nlogFlag++;
        if(5 == nlogFlag)
        {
            nlogLoop = 1;
            nlogFlag = 0; 
        }
		if(nlogLoop)
        {
            memset(szLogPathName, 0, LOG_PATH_MAX_SZ);
            snprintf(szLogPathName, sizeof(szLogPathName),"%s/%s", LOG_PATH, szLogPath[nlogFlag]);
            FILE* pFile = fopen(szLogPathName, "w+");
			if(pFile==NULL)
			{
				pthread_mutex_unlock(&gstLogCb.stMutex);
                printf("[fix]create file[%s] failed\r\n", szLogPathName);
				return;
			}
            gstLogCb.stLogDescTbl[nlogFlag].iLined = 0;
            fprintf(pFile,"magic=%x;version:%d;lined=%d;recover=%d\n", SDK_LOG_MAGIC, SDK_LOG_VER, 
                                  gstLogCb.stLogDescTbl[nlogFlag].iLined, gstLogCb.stLogDescTbl[nlogFlag].u8RcovrCnt);
            fclose(pFile);
            pFile = NULL;
        }
    }

    /*
       FIX:若写入flash，需要增加行数控制，单个日志文件最大行数
       写入FLSAH有利于出厂后的维护，可直接导出日志定位
       文件头部用于日志文件维护表，生成时间和日志的行数
    */
    memset(szLogPathName, 0, LOG_PATH_MAX_SZ);

    if ( LOG_FILE & u8Target )
    {
        sprintf(szLogPathName,"%s/%s", LOG_PATH, szLogPath[nlogFlag]);
        FILE* pFile = fopen(szLogPathName, "r+");
        if (NULL == pFile)
        {
            pFile = fopen(szLogPathName, "w+");
            if (NULL == pFile)
            {
            	pthread_mutex_unlock(&gstLogCb.stMutex);
                printf("[fix]create file[%s] failed\r\n", szLogPathName);
                return;
            }
        }
        
        fseek(pFile, 0, SEEK_END); /* 偏移到末尾,不覆盖原记录 */
        va_start (arg, format);
        
        SDK_LOG_UpdateFileDescrp(pFile);
        fprintf(pFile, "[%04d-%02d-%02d %02d:%02d:%02d]%s", 
                  tm_log.tm_year + 1900, tm_log.tm_mon + 1, tm_log.tm_mday, tm_log.tm_hour, tm_log.tm_min, tm_log.tm_sec, szLevelPrefix[u8PrintLevel]);
        vfprintf (pFile, format, arg);
        fprintf(pFile, "\n");
        va_end (arg);

        fflush(pFile);
        fclose(pFile);
    }
    
    pthread_mutex_unlock(&gstLogCb.stMutex);
#endif
	
    return;
}

/* 怎么设置？用线程获取屏幕输入 */
void SDK_LOG_SetPrintLevel(int iLevel)
{
    pthread_mutex_lock(&gstLogCb.stMutex);
    gstLogCb.iPrintLevel = iLevel;
    pthread_mutex_unlock(&gstLogCb.stMutex);

    return;
}

/* 设置允许标识 */
void TFLOG_SetAllowFlag(int allowflag)
{
	pthread_mutex_lock(&gstLogCb.stMutex);
    nAllow = allowflag;
    pthread_mutex_unlock(&gstLogCb.stMutex);

    printf("TFLOG_SetAllowFlag %d\n",nAllow);
}

/* @Description：LOG模块初始化 */
void TFLOG_Init()
{
#ifdef SDK_LOG_FILE
    memset(&gstLogCb,0,sizeof(gstLogCb));
    pthread_mutex_init(&gstLogCb.stMutex, NULL);
    gstLogCb.iPrintLevel = LOG_PRINT_LEVEL;
	LogDirInit();
    /* 读取各个日志文件头部信息 */
    SDK_LOG_ReadLogsFileDescrp();

    nInit = 1;
    nAllow = 1;

#endif

    return;

}

#endif