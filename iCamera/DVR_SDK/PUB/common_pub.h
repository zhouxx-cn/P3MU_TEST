#ifndef __COMMON_PUB_H__
#define __COMMON_PUB_H__

/*!
 *\brief 存放公共借口
 *
 */
#include <stdbool.h>


#ifdef __cplusplus
extern "C"
{
#endif

#ifndef MS
#define MS(ts) (unsigned int)((ts.tv_sec * 1000) + (ts.tv_nsec / 1000000))
#endif /* !MS */

//!breif 解析DevcieCap.xml中的能力_bool类型
//param pos[in] 格式 xx:xx:xx
//param df[in]  当所查路径不存在时的默认值
////return 结果
int GetCapFromDeviceCapXML_Bool(const char *pos, bool df);
int  GetCapFromDeviceCapXML_Int(const char *pos);

//!breif 解析DevcieCap.xml中的能力_bool类型(support)
//param pos[in] 格式 xx:xx:xx
//param df[in]  当所查路径不存在时的默认值
//return 结果
bool GetCapSupportFromDeviceCapXML_Bool(const char *pos, bool df);

int GetValueFromBootEnv(const char *str, char *val, unsigned int size);

int SetValueToBootEnv(const char *str, const char *val, unsigned int size);

void DeviceCfgReset(const char *func);

int GetSCodeFromFile(const char *file, char *code, int codeLen);

int SetSCodeToFile(const char *file, const char *code);

int GetDeviceUUID(char* pUuid, int pLen);

int GetRecordBufSizeByBoardID(int *szRing, int *szFrame);

int str2double(double *val, const char *str);


/**
********************************************************************************
 * @函数功能: 获取系统启动完成且图像已初始化完的状态。主要用于开启录像判断
 * @作者    : HL.LIU
 * @返回值  : 0 - 未完全启动 1 - 启动完成
 * @其他    :
********************************************************************************
*/
int GetSystemStartOverFlag();

/**
********************************************************************************
 * @函数功能: 设置系统启动完成且图像已初始化完（IRCUT已完成自检2次之后）的状态
 * @作者    : HL.LIU
 * @param[in] flag:0 - 未完全启动 1 - 启动完成
 * @返回值  :
 * @其他    :
********************************************************************************
*/
int SetSystemStartOverFlag(int flag);

/**
********************************************************************************
 * @函数功能: 获取设备初始化步骤是否已完成（main函数里的步骤已运行完）
 * @作者    : HL.LIU
 * @返回值  : 0 - 未完全启动 1 - 启动完成
 * @其他    :
********************************************************************************
*/
int GetDevInitOverFlag();

/**
********************************************************************************
 * @函数功能: 设置设备初始化完成标记（main函数里的步骤已运行完）
 * @作者    : HL.LIU
 * @param[in] flag:0 - 未完全启动 1 - 启动完成
 * @返回值  :
 * @其他    :
********************************************************************************
*/
int SetDevInitOverFlag(int flag);


/**
 * @brief 获取mjpeg buffer
 * @param  [IN]  mode    输入的模式， 0:外部输入buffer  1:内部动态malloc
 * @param  [IN]  nChn    通道号
 * @param  [OUT] pBuffer    输出的Buffer
 * @param  [OUT] pLength    输出的BUffer长度
 * @param  [IN]  maxLength    buffer的最大长度
 * @param  [OUT] pTimestampMs 输出的时间戳(毫秒)
 * @return int 0:正常，其他值为异常
 */
int GetMjpegBuffer(int mode, int nChn, char **pBuffer, int *pLength, int maxLength, long long *pTimestampMs);

/**
 * @brief 获取mjpeg buffer
 * @param  [IN]  mode    输入的模式， 0:外部输入buffer  1:内部动态malloc
 * @param  [IN]  nChn    通道号
 * @param  [OUT] pBuffer    输出的Buffer
 * @param  [OUT] pLength    输出的BUffer长度
 * @param  [IN]  maxLength    buffer的最大长度
 * @param  [OUT] pTimestampMs 输出的时间戳(毫秒)
 * @return int 0:正常，其他值为异常
 */
int GetMjpegBuffer_V2(int mode, int nChn, char **pBuffer, int *pLength, int maxLength, long long *pTimestampMs);


/*@breif GetUIDInfoFromLicensePart 从devinfo配置分区里面获取UID信息
 *@param  pTutkUID 得到的UID缓冲区
 *@param  nTutkUIDLen 缓冲区大小
 *@param  pPushId 得到的push ID缓冲区
 *@param  nPushIdLen 缓冲区大小
 *@return 成功返回0
 *@      失败返回-1
 */
int GetUIDInfoFromLicensePart(char *pTutkUID, int nTutkUIDLen, char *pPushId, int nPushIdLen);

/**
 * @brief 获取分区使用率
 * @param  [IN]  szPartitionName 分区名字
 * @param  [OUT] pnUsage 分区使用率
 * @return int 0:成功， -1:找不到分区
 */
int GetPartitionUsage(char *szPartitionName, float *pnUsage);

int IsSupportDevShareFunc();

/* 设备重启原因 */
typedef enum{
    REBOOT_WIFI_POWER_BOOST                     = 0,   // WIFI功率提升
    REBOOT_WIFI_NO_IDPRODUCT                    = 1,   // WIFI模组检测异常
    REBOOT_WIFI_STATUS_ABNORMAL                 = 2,   // WIFI状态异常
    REBOOT_P2P_STATUS_ABNORMAL                  = 3,   // P2P状态异常
    REBOOT_MAIN_STREAM_NO_DATA                  = 4,   // 主码流无流
    REBOOT_CLOUDSTORAGE_SWITCH                  = 5,   // 云存模式切换
    REBOOT_GPRSDEVICE_REST		                = 6,   // 4g模组复位重启设备
    REBOOT_GPRSDEVICE_COMERROR		            = 7,   // 4g模组通讯异常重启设备
    REBOOT_CHECKCARD_TIMEOUT	            	= 8,   // 4gSIM卡限制联网计时达到
    REBOOT_CIU_ENC_CHANGE	            		= 9,   // 设置编码参数改变
    REBOOT_UPLOAD_TIMEOUT	            		= 10,  // 远程升级超时
    REBOOT_UPLOAD			            		= 11,  // 远程升级重启
    REBOOT_RESUME			            		= 12,  // 复位重启
    REBOOT_FORMAT_SDCARD			            = 13,  // 格式化SD卡
    REBOOT_REMOTE_OPERATION			            = 14,  // 远程重启操作
}eRebootResult;
void RebootLinux(eRebootResult nResult);

/**
 * @brief 保存扫码配网的态
 *
 * @param en  0 - 禁用 1 - 开启
 * @return int
 */
int SetQRScanStatus(int en);

int GetQRScanStatus();

/**
 * @brief 获取扫码配网的有效性
 *
 * @return int
 */
int GetQRScanCloseFlags();

/**
********************************************************************************
 * @函数功能: 删除字符串最后的回车换行符(\r\n跟\n)
 * @作者    : HL.LIU
 * @param[in] *string
 * @param[in] stringMaxLen
 * @返回值  :
 * @其他    :
********************************************************************************
*/
int PubFunc_deleteNewlineInString(char *string, int stringMaxLen);

/*@ preif 执行命令并将结果存储到result字符串中
 *@ param [in] cmd 要执行的命令
 *@ param [in] result 命令输出的结果
 *@ param [in] len  result的长度
 *@ return  0 成功
 *@       -1 出错
 */
int ShellCmdOut(const char *cmd, char *result, int len);

/*@ preif 在命令command的输出中寻找字符串string
 *@ param [in] command 要执行的命令
 *@ param [in] string  要寻找的字符串(忽略大小写)
 *@ return 0 不存在该字符串
 *@       1 存在该字符串
 *@      -1 出错
 *@ note  忽略字符串的大小写，如果不想忽略，则可以用strstr代替strcasestr
 *@      使用strcasestr的时候，要使用宏_GNU_SOURCE，否则会出现警告(不影响使用)
 *@      可以拓展成命令行字符串的定位
 */
int IsStringInCommand(const char *command, const char *string);

int ISO8601TimeGet(time_t time, int nTimeZone, char *pBuf, int nBufLen);

/*-******************************************************************
在str1字符串中，找到第一个'-'的位置，拼接字符串str2，输出
--------------------------
参数 outstr[out]:输出结果字符串
参数 outstrlen[in]:输出结果字符串长度
--------------------------
返回 ：0-成功，-1-失败（缓冲区不足）
*******************************************************************-*/
int InsertStringAtFirstDash(const char* str1, const char* str2, char *outstr, unsigned int outstrlen);

#ifdef __cplusplus
}
#endif

#endif
