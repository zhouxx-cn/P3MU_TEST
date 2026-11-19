#ifndef __HTTP_SERVER_H_
#define __HTTP_SERVER_H_

/*!
*  @struct    tagHY_DVR_HTTP_EVENT_SERVER_CFG_V2
*  @brief    HTTP事件服务器信息V2
*  @illustrate
*/
typedef struct tagNET_HY_DVR_HTTP_EVENT_SERVER_CFG_V2
{
    int nValid;                             /*!< 本结构体是否有效，0表示无效 */
    int nEnable;                            /*!< 0，不使能，1，使能 */
    int nProtocol;                          /*!< 0，表示http协议，1.https（暂不支持） */
    char strHost[256];                      /*!< 服务器的地址，可以是IP地址，也可以是域名 */
    char strPath[256];                      /*!< 服务的URL路径 */
    int nPort;                              /*!< 端口，默认为80 */
    char strUserName[32];                   /*!< 服务器的用户名 */
    char strPassword[32];                   /*!< 服务器的密码 */
    int nDataTransfer2ServerTimeout;        /*!< 和服务器交互超时时间,单位是秒 */
    int iAuthMode;                          /*!< 认证方式;0-无,1-OAuth */
    char acDeviceToken[128];                /*!< 设备token */
    char acSecret[128];                     /*!< 秘钥 */

    char acReserved[256];
}NET_HY_DVR_HTTP_EVENT_SERVER_CFG_V2, *LPNET_HY_DVR_HTTP_EVENT_SERVER_CFG_V2;

#endif

