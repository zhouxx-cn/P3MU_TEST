#ifndef _MDBG_H_
#define _MDBG_H_

#include "xrsdef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef SINT32 (*PMDBG_FUNC)(char *);

typedef struct MDBG_T
{
    char *pCmdName;     /* 命令名称,静态字串 */
    PMDBG_FUNC pFunc;
    char *pDoc;         /* 帮助信息,可不填,静态字串 */
    SINT32 Key;
} MDBG_S, *PMDBG_S;
#define MDBG_S_LEN  sizeof(MDBG_S)

/* 模块命令列表 */
typedef struct
{
    SINT32 Id;          /* COMPID */
    char *pName;        /* COMP NAME 静态字串 */
    PMDBG_S pMenu;      /* MDBG menu */
    PRN_LVL_E PrnLvl;   /* 默认打印级别 */
    PRN_SHOW_E PrnShow; /* 默认打印级别比较方式 */
    SINT32 PrnTime;     /* 打印到syslog时,超过默认级别的打印的时间,超时后恢复默认级别 */
} MODULE_LIST_S, *PMODULE_LIST_S;
#define MODULE_LIST_S_LEN   sizeof(MODULE_LIST_S)

/* MDBG命令帮助 */
#define MDBG_COMMON_CMD_DOC  "option list:\n"                       \
    "\t\t\t-a or -A,\t Show all the module\n"                       \
    "\t\t\t-m [module id] -n [module name]\n"                       \
    "\t\t\t-P [1 term, 2 syslog] -p [0 ~ 8, print level]\n"         \
    "\t\t\t-s [sub mod idx] -S [pring show flag, default 0]\n"      \
    "\t\t\texample: mdbg -m 64, or ll -n mvssmgr, "                 \
        "change current to mfsfs module,\n"                         \
    "\t\t\t\tU also can use the module name 'mvssmgr' directly.\n"  \
    "\t\t\t\tmdbg -P 1 -p 1, set current module print level to 1, " \
        "and the print info show on this terminal"

/* 获取打印输出方式 */
SINT32 MDbg_Get_PrnMannerFlag(void);

/* 获取模块打印级别 */
SINT32 MDbg_GetTraceLvlById(SINT32 ModId);

SINT32 MDbg_SetTraceLvlByName(char *Name, SINT32 Level);

SINT32 MDbg_GetTraceTimeById(SINT32 ModId);
SINT32 MDbg_SetTraceTimeByName(char *Name, SINT32 TimeInterval);

SINT32 MDbg_DelModTermFd(void *pFile);

/* 按模块分级控制打印函数 */
SINT32 MDbg_ModLvl_Prn(SINT32 ModId, SINT32 Lvl, const char *pFmt, ...) __attribute__((format(printf, 3, 4)));

/*
 * 初始化CLI
 * PrnMannerInit: 打印方式初始化值,默认PRN_MANNER_SYSLOG
 * pPrnLvlConf  : 模块打印级别配置文件,每一行格式:模块名+打印级别值
 * pModNameList : 模块名列表,NULL结尾
 */
SINT32 MDbg_Init(PRN_MANNER_E PrnMannerInit, char *pPrnLvlConf, const char *const pModNameList[]);

/******************************************************************************
 * 根据选项列表定义,获取命令参数选项: 如: -l 123中的选项a参数123,sample:
SINT32 XXX_MDBG_FUNC(char *arg)
{
    SINT32 l = 0;
    char *pOptParam = NULL;

    MDBG_CMD_ARGS_PARSE_BGN(arg, "hl");
    switch (MDBG_CMD_GET_OPTION())
    {
    case 'h':   // do something
        pOptParam = MDBG_CMD_GET_OPT_PARAM();
        printf("%s\n", pOptParam);
        break;
    case 'l':
        MDBG_CMD_OPT_PARAM_ATOI(&l);    // get option param value
        break;
    ......
    }
    MDBG_CMD_ARGS_PARSE_END();

    return 0;
}
******************************************************************************/
char *MDbg_Cmd_GetOpt(char *pLine, char *pOpt, char *pOptParam, const char pOptList[]);

/* parse options begin */
#define MDBG_CMD_ARGS_PARSE_BGN(pArgs, pOptList) do {       \
    char *pMDbgArgs = pArgs;                                \
    char OptMDbg = 0, OptParamMDbg[128] = {0};              \
    while (NULL != (pMDbgArgs = MDbg_Cmd_GetOpt(pMDbgArgs,  \
        &OptMDbg, OptParamMDbg, pOptList))) {

/* get option */
#define MDBG_CMD_GET_OPTION()       OptMDbg

/* get option parameter string */
#define MDBG_CMD_GET_OPT_PARAM()    OptParamMDbg

/* convert option parameter to integer value */
#define MDBG_CMD_OPT_PARAM_ATOI(pVal) do {  \
    if (('0' == OptParamMDbg[0]) &&         \
        (('x' == OptParamMDbg[1]) ||        \
        ('X' == OptParamMDbg[1]))) {        \
        char pFmtSz[] = "0x%x";             \
        pFmtSz[1] = OptParamMDbg[1];        \
        sscanf(OptParamMDbg, pFmtSz, pVal); \
    }                                       \
    else {                                  \
        *pVal = atoi(OptParamMDbg);         \
    } } while (0)

/* parse options end */
#define MDBG_CMD_ARGS_PARSE_END()   \
    memset(OptParamMDbg, 0x00, sizeof(OptParamMDbg)); }} while (0)

/* 统一用以下两个宏,简化使用;默认增加帮助选项h */
#define MDBG_CMD_ARGS_SWITCH_BGN(pArgs, pOptList)   \
    MDBG_CMD_ARGS_PARSE_BGN(pArgs, pOptList"h")     \
    switch (OptMDbg) {                              \
    case 'h': MDbg_Show_Usage(__FUNCTION__); return 0;

#define MDBG_CMD_ARGS_SWITCH_END()  default:        \
    MDbg_Print("invalid option %c\n", OptMDbg);     \
    return 0; } MDBG_CMD_ARGS_PARSE_END()

/******************************************************************************
 * 注册某个模块的CLI命令列表,sample:
// 定义
MDBG_S XXX_menu[] =
{
    {"XXX_prn", XXX_MDBG_FUNC, "XXX CLI COMMAND HELP, option : -l [val] -m [id] -n [name], -h", 0},
    {NULL, NULL, NULL, 0} // 必须加,END FLAG
};
MODULE_LIST_S XXX_mdbg = {COMP_XXX, "XXX_MODULE_NAME", XXX_menu, PRN_LVL_ERR};

MDbg_Reg(&XXX_mdbg); // 注册
 *****************************************************************************/
SINT32 MDbg_Reg(MODULE_LIST_S *pModList);

/* 打印函数使用帮助usage text */
SINT32 MDbg_Show_Usage(const char *pMDbgFuncName);

/* 打印当前调用栈 */
SINT32 MDbg_Call_Stack(void);

void MDbg_Hex_Dump(UINT8 *pData, SINT32 Size);

SINT32 Mdbg_Chk_Mutex(pthread_mutex_t *pMtx);

/* 注册的命令列表使用的打印函数,类似printf */
SINT32 MDbg_Print(const char *pFmt, ...) __attribute__((format(printf, 1, 2)));

/* 获取当前MDBG打印句柄 */
FILE *MDbg_Get_PrnFPtr(void);
SINT32 MDbg_FPrint(FILE *fp, const char *pFmt, ...) __attribute__((format(printf, 2, 3)));

/******************************************************************************
 * 以下三个函数用于表格形式对齐打印,以表头宽度为基准
SINT32 MDbg_XXX(char *pArgs)
{
    char column[12] = "dev";
    ....

    MDbg_Align_Print_Bgn(' ', "%s type size", column);  // 格式化打印表头
    MDbg_Align_Print("%d%d%d", dev, type, size);        // 格式化打印表项值行
    MDbg_Align_Print_End();                             // 结束
    MDbg_Print("hello mdbg XXX\n");

    return 0;
}
*****************************************************************************/

/*
 * 对齐打印开始,初始化表头
 * Separator   : 分隔符
 * pColmnHdrFmt: 格式化字符串
 */
SINT32 MDbg_Align_Print_Bgn(char Separator, const char *pTblHdr,
    const char *pColmnHdrFmt, ...) __attribute__((format(printf, 3, 4)));

/* 对齐打印表项 */
SINT32 MDbg_Align_Print(const char *pFmt, ...) __attribute__((format(printf, 1, 2)));

/* 打印结束 */
SINT32 MDbg_Align_Print_End(void);

/* 也可以用以下表格打印宏
SINT32 MDbg_XXX(char *pArgs)
{
    MDBG_PRN_TBL_BGN("Dev Info", 1);        // 格式化打印表头
    MDBG_PRN_TBL_COLUMN("Dev", "%d", 1);    // 格式化打印表项值列
    MDBG_PRN_TBL_END();                     // 结束

    return 0;
}
*/

/* 表打印开始 */
#define MDBG_PRN_TBL_BGN(pTblNameFmt, LineNum, pArgs...)   do {         \
    char *pTblNameTmp = (char *)pTblNameFmt;                            \
    SINT32 _IdxLine = 0, _IdxCo = 0, _enCoLine = 1, _CoNum = 0;         \
    SINT32 CoHdrLineEnd = 0, ColumnWidthTbl[256] = {0}, TblEndFlag = 0; \
    if (pTblNameTmp) {                                                  \
        MDbg_Print("----- " #pTblNameFmt " -----\n", ##pArgs);          \
        TblEndFlag = 1;                                                 \
    }                                                                   \
    for (_IdxLine = 0; _IdxLine < (LineNum + 1); _IdxLine++) {

/* 不打印表头 */
#define MDBG_PRN_TBL_NULL_CLMN_HDR()    do { _enCoLine = 0; } while (0)

/* 打印表头及每行值 */
/* 参数说明: ColumnName - 列名
 *           expr - 本列值是否打印
 *           pFmt - 格式化参数;比如: %d
 *           ColumnVal - 参数值变量) */
#define MDBG_PRN_TBL_COLUMN(ColumnName, expr, pFmt, ColumnVal...)    do {       \
    if (!CoHdrLineEnd) {                                                        \
        if (_enCoLine) {                                                        \
            ColumnWidthTbl[_CoNum++] = MDbg_Print("%s ", ColumnName);           \
        }                                                                       \
        else {                                                                  \
            ColumnWidthTbl[_CoNum++] = strlen(ColumnName) + 1;                  \
        }                                                                       \
    }                                                                           \
    else {                                                                      \
        if (expr) {                                                             \
            SINT32 w = ColumnWidthTbl[_IdxCo++] - MDbg_Print(pFmt, ##ColumnVal);\
            if (w > 0) { MDbg_Print("%*c",  w, ' '); }                          \
        }                                                                       \
        else {                                                                  \
            if (_IdxCo) { MDbg_Print("%*c",  ColumnWidthTbl[_IdxCo++], ' '); }  \
        }                                                                       \
        if (_IdxCo == _CoNum) {                                                 \
            _IdxCo = 0; MDbg_Print("\n");                                       \
        }                                                                       \
    }} while (0)

/* 获取当前打印行号,-1为表头 */
#define MDBG_PRN_TBL_LINE_IDX() (_IdxLine - 1)

#define MDBG_PRN_TBL_END()      if (!CoHdrLineEnd) {            \
    if (_enCoLine) { MDbg_Print("\n"); } CoHdrLineEnd = 1; } }  \
    if (TblEndFlag) { MDbg_Print("----- END -----\n\n"); } } while (0)

/* CLI去初始化 */
SINT32 MDbg_Fini(void);

#define MDBG_PRI_DBG(pFmt, args...) MDbg_Print(pFmt, ## args)
#define MDBG_PRI_REL(pFmt, args...)

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _MDBG_H_ */
