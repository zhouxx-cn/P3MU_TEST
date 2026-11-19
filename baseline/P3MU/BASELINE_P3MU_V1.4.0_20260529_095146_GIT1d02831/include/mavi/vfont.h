#ifndef _FONT_H_
#define _FONT_H_

#include "xrsstd.h"

#define FONT_FILE_PATH_LEN  128

typedef enum
{
    FONT_ERROR_FOPEN = COMP_ERROR_START, /* 打开字库错误 */
} FONT_ERROR_E;

#define UCS224_FILE_HEADER          16   /* 字库文件头16 bytes */
#define UCS224_FILE_INFO            8    /* 字库文件信息4bytes + 字符集索引表个数4bytes */
#define UCS224_CHASET_INDEX_SIZE    16   /* 字符集索引表16 bytes */
#define UCS224_UNICODE_CHARSET_NUM  146  /* UNICODE编码总分区数 */

typedef enum
{
    FONT_CMD_GETDEVTYPE = CMDNO(0, COMP_VIDEOCFG), /* 获取机器型号,param(SINT32 *),Chn(此处无效) */
    FONT_CMD_GETVIDEONUM,   /* 获取视频输入通道数,param(SINT32 *),Chn(此处无效) */
} FONT_CMD_E;

/* 字库枚举类型 */
typedef enum
{
    FONT_GB231216,  /* 16点阵GB2312字库 */
    FONT_UCS224,    /* 24点阵Unicode(UCS-2)字库 */
    FONT_BMP,		/*直接给bitmap*/
    FONT_BUTT
} FONT_TYPE_E;

typedef enum
{
    FONT_STAT_NONE = 0, /* 未使用字库功能 */
    FONT_STAT_BUTT
} FONT_STAT_E;

typedef struct 
{
    UINT8 *pChinaAddr;
    UINT8 *pAsciAddr;
} GB231216_INFO_S, *PGB231216_INFO_S;
#define GB231216_INFO_S_LEN sizeof(GB231216_INFO_S)

typedef struct 
{
    SINT32 UniBgn;      /* 对应unicode编码开始值2byte */
    SINT32 UniEnd;      /* 对应unicode编码结束值2byte */
    SINT32 FontSizeX;   /* 点阵字形大小2byte */
    SINT32 FontSizeY;   /* 点阵字形大小2byte */
    SLNG32 FontDataBgn; /* 某个字符集点阵数据在字库文件中的起始地址4byte */
    SLNG32 FontDataEnd; /* 某个字符集点阵数据在字库文件中的结束地址4byte */
} UCS224_CHARACTER_SET_INDEX_S, *PUCS224_CHARACTER_SET_INDEX_S;
#define UCS224_CHARACTER_SET_INDEX_S_LEN    sizeof(UCS224_CHARACTER_SET_INDEX_S)

typedef struct 
{
    const char *pFontAddr;
    SLNG32 FontFileSize;    /* 字库文件总大小4byte */
    SLNG32 CharSetNum;      /* 字库文件中支持的字符集个数4byte */
    UCS224_CHARACTER_SET_INDEX_S CharSetIdx[UCS224_UNICODE_CHARSET_NUM];
} UCS224_INFO_S, *PUCS224_INFO_S;
#define UCS224_INFO_S_LEN   sizeof(UCS224_INFO_S)

typedef union
{
    GB231216_INFO_S Gb231216;
    UCS224_INFO_S Ucs224;
} FONT_INFO_U, *PFONT_INFO_U;
#define FONT_INFO_U_LEN sizeof(FONT_INFO_U)

/* 设备信息结构体 */
typedef struct 
{
    UINT8 FontType; /* (FONT_TYPE_E) */
    UINT8 Resvd[3]; /* 保留位 */
    FONT_INFO_U FontInfo;
} FONT_INFO_S, *PFONT_INFO_S;
#define FONT_INFO_S_LEN sizeof(FONT_INFO_S)

typedef struct 
{
    UINT8 FontStat; /* 字库状态(FONT_STAT_E) */
    UINT8 FontType; /* 字库枚举类型(FONT_TYPE_E) */
    UINT8 Resvd[6]; /* 保留位 */
    char *pFontAddr;
    char FontPath[FONT_FILE_PATH_LEN];
} FONT_CONF_S, *PFONT_CONF_S;
#define FONT_CONF_S_LEN sizeof(FONT_CONF_S)
#ifdef POINTER_SIZE_8
ASSERT_SIZEOF_TYPEDEF_STRUCT(FONT_CONF_S, 16 + FONT_FILE_PATH_LEN);
#else
ASSERT_SIZEOF_TYPEDEF_STRUCT(FONT_CONF_S, 12 + FONT_FILE_PATH_LEN);
#endif /* POINTER_SIZE_8 */
/******************************************************************************
 * 函数功能: 打开FONT模块,分配系统资源,模块初始化
 * 输入参数: FontConf: 字体配置信息结构体
 * 输出参数: 无
 * 返回值  : >0-模块句柄,<0-错误代码
 *****************************************************************************/
SINT32 Font_Open(FONT_CONF_S FontConf);

/******************************************************************************
 * 函数介绍: 获取字库信息
 * 输入参数: handle: 模块句柄,此处无效
 *           pFontInfo: 字库信息结构体指针
 * 输出参数: pFontInfo: 返回字库信息
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Font_Get_Info(SINT32 handle, PFONT_INFO_S pFontInfo);

/******************************************************************************
 * 函数功能: 模块控制
 * 输入参数: handle: 模块句柄,此处无效
 *           cmd: 具体的控制参数命令
 *           Chn: 视频输出通道号(从0开始)
 *           param: 对应命令的参数
 *           param_len: param长度,特别对于GET命令时,输出参数应先判断缓冲区是否足够
 * 输出参数: param: 输出参数
 * 返回值  : >=0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Font_IoCtrl(SINT32 handle, SINT32 cmd, SINT32 Chn, SINT32 *param, SINT32 param_len);

/******************************************************************************
 * 函数介绍: 关闭FONT模块,释放系统资源
 * 输入参数: handle: 模块句柄,此处无效
 * 输出参数: 无
 * 返回值  : 0-成功,<0-错误代码
 *****************************************************************************/
SINT32 Font_Close(SINT32 handle);

#endif /* _FONT_H_ */
