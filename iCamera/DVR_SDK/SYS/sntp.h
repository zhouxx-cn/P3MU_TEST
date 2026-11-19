#ifndef __SNTP_H__
#define __SNTP_H__

#ifdef __cplusplus
extern "C" {
#endif

/*@prief     SyncNtpDate 利用sntp协议进行ntp对时
 *@param[in] host_name ntp主机名
 *@param[in] time_zone 时区
 *@return 0对是成功; -1对时失败
 *@note 时区支持整数和负数
 *对时原则: 首先是利用指定服务器对时，如果对时失败，则使用备份的配置文件里面的服务器进行对时
 *          首先逐一进行对时，成功则跳出，否则进行下一个，直至测试完毕为止
 */
int SyncNtpDate(const char *host_name, int time_zone);

#ifdef __cplusplus
}
#endif

#endif // end of __SNTP_H__