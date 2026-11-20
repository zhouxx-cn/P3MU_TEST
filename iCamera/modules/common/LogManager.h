#ifndef __LOG_MANAGER_H__
#define __LOG_MANAGER_H__

#include <string.h>
#include <unistd.h>

/* 通用日志接口 */
#define LOG_E(module, format...) DebugLog(DEBUG_LOG_LEVEL_ERROR, module, __FILE__, __LINE__, format)
#define LOG_W(module, format...) DebugLog(DEBUG_LOG_LEVEL_WARN, module, __FILE__, __LINE__, format)
#define LOG_I(module, format...) DebugLog(DEBUG_LOG_LEVEL_INFO, module, __FILE__, __LINE__, format)
#define LOG_D(module, format...) DebugLog(DEBUG_LOG_LEVEL_DEBUG, module, __FILE__, __LINE__, format)
#define LOG_H(module, desc, data, len) HexDebugLog(DEBUG_LOG_LEVEL_INFO, module, __FILE__, __LINE__, desc, data, len)

/* OPEN模块日志接口,不可关闭 */
#define OPEN_LOG_E(format...) LOG_E(LOG_MODULE_OPEN, format)
#define OPEN_LOG_W(format...) LOG_W(LOG_MODULE_OPEN, format)
#define OPEN_LOG_I(format...) LOG_I(LOG_MODULE_OPEN, format)
#define OPEN_LOG_D(format...) LOG_D(LOG_MODULE_OPEN, format)
#define OPEN_LOG_H(desc, data, len) LOG_H(LOG_MODULE_OPEN, desc, data, len)

#define ABS_DIFF(x, y) ((x > y) ? (x - y) : (y - x))

/* 条件打印 */
#define CHECK_PERR(cond, str, args...)                                                                                 \
    if (cond)                                                                                                          \
    {                                                                                                                  \
        LOG_E(LOG_MODULE_PUB, str, ##args);                                                                            \
    }
#define CHECK_RTN(cond, err)                                                                                           \
    if (cond)                                                                                                          \
    {                                                                                                                  \
        return (err);                                                                                                  \
    }
#define CHECK_RTNP(cond, err, str, args...)                                                                            \
    if (cond)                                                                                                          \
    {                                                                                                                  \
        LOG_E(LOG_MODULE_PUB, str, ##args);                                                                            \
        return (err);                                                                                                  \
    }
#define CHECK_RTNVP(cond, str, args...)                                                                                \
    if (cond)                                                                                                          \
    {                                                                                                                  \
        LOG_E(LOG_MODULE_PUB, str, ##args);                                                                            \
        return;                                                                                                        \
    }
#define CHECK_GOTOM(cond, lable, str, args...)                                                                         \
    if (cond)                                                                                                          \
    {                                                                                                                  \
        LOG_E(LOG_MODULE_PUB, str, ##args);                                                                            \
        goto lable;                                                                                                    \
    }

#ifdef __cplusplus
extern "C"
{
#endif

    typedef unsigned long long UINT64;

    typedef enum
    {
        DEBUG_LOG_LEVEL_NONE = 0,
        DEBUG_LOG_LEVEL_ERROR,
        DEBUG_LOG_LEVEL_WARN,
        DEBUG_LOG_LEVEL_INFO,
        DEBUG_LOG_LEVEL_DEBUG,
        DEBUG_LOG_LEVEL_ALL
    } DEBUG_LOG_LEVEL_E;

    typedef enum
    {
        LOG_MODULE_OPEN = 0x01 << 0,    /* OPEN常开日志*/
        LOG_MODULE_PUB = 0x01 << 1,     /* PUB公共 */
        LOG_MODULE_MAIN = 0x01 << 2,    /* MAIN */
        LOG_MODULE_AVL = 0x01 << 3,     /* AVL */
        LOG_MODULE_4G = 0x01 << 4,      /* 4G */
        LOG_MODULE_NET = 0x01 << 5,     /* NET */
        LOG_MODULE_MPU = 0x01 << 6,     /* MPU */
        LOG_MODULE_ZWSDK = 0x01 << 7,   /* ZWSDK */
        LOG_MODULE_JSONSYS = 0x01 << 8, /* JSONSYS */
        LOG_MODULE_SD = 0x01 << 9,      /* SD */
        LOG_MODULE_PTZ = 0x01 << 10,    /* PTZ */
        LOG_MODULE_AUTH = 0x01 << 11,   /* AUTH */
        LOG_MODULE_LIGHT = 0x01 << 12,  /* LIGHT */
        LOG_MODULE_SYS = 0x01 << 13,    /* SYS */
        LOG_MODULE_LCD = 0x01 << 14,    /* LCD */
        LOG_MODULE_UI = 0x01 << 15,     /* UI */
        LOG_MODULE_ALL = 0xFFFFFFFF,
    } LOG_MODULE_E;

    void DebugLog(int level, unsigned int module, const char *file, int line, const char *format, ...);
    void HexDebugLog(int level, unsigned int module, const char *file, int line, const char *pDesc, const char *data,
                     int len);

    /* log等级 */
    int init_log_level(void);
    int get_log_level(void);
    int set_log_level(int logLevel);

    /* log模块使能 */
    int SetLogEnable(int enable);
    int GetLogModuleEnable(LOG_MODULE_E module);
    int SetLogModuleEnable(LOG_MODULE_E module, int enable);

    char *GetLogModuleName(unsigned int module);

    UINT64 GetSysTickCountMs(void);
    int __SystemGetSecCount(void);

#ifdef __cplusplus
}
#endif

#endif /* __LOG_MANAGER_H__ */