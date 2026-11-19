/*** 
 * @Author: Sun Yuhui
 * @Date: 2022-05-13 16:22:17
 * @LastEditTime: 2022-05-24 15:06:17
 * @LastEditors: Sun Yuhui
 * @Description: 
 * @FilePath: /sdk/include/xrs/defs.h
 * @deepelves.com
 */
#ifndef __DEFS_H__
#define __DEFS_H__

#ifndef NULL
#define NULL                0
#endif

/* 数据类型定义 */
#ifndef SINT8
typedef signed char         SINT8;
#endif
#ifndef SINT16
typedef signed short        SINT16;
#endif
#ifndef SINT32
typedef signed int          SINT32;
#endif
#ifndef SLNG32
typedef signed long         SLNG32;
#endif
#ifndef SINT64
typedef signed long long    SINT64;
#endif
#ifndef UINT8
typedef unsigned char       UINT8;
#endif
#ifndef UINT16
typedef unsigned short      UINT16;
#endif
#ifndef UINT32
typedef unsigned int        UINT32;
#endif
#ifndef ULNG32
typedef unsigned long       ULNG32;
#endif
#ifndef UINT64
typedef unsigned long long  UINT64;
#endif
#ifndef FLOAT32
typedef float               FLOAT32;
#endif
#ifndef FLOAT64
typedef double              FLOAT64;
#endif

#ifndef BOOL
typedef enum
{
    FALSE = 0,
    TRUE  = 1
} BOOL;
#endif

#ifndef HANDLE_T
#define HANDLE_T
#define HANDLE  void *
#endif

#ifndef WORD
typedef short int WORD;
#endif

#ifndef DWORD
typedef unsigned int DWORD;
#endif

#ifndef BYTE
typedef unsigned char  BYTE;
#endif

// 定义按钮状态
typedef enum key_status
{
    KEY_PUSH = 0,   // 按键按下
    KEY_RELE        // 按键松开
} KEY_STATUS_E;

// 定义IRCut状态
typedef enum ircut_mode
{
    IRCUT_MODE_DAY = 0,	// 白天
    IRCUT_MODE_NIGHT    // 夜晚
} IRCUT_MODE_E;

#endif // __DEFS_H__