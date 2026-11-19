#ifndef VIDEO_NET_SERVER_CONFIGURE_H

#define VIDEO_NET_SERVER_CONFIGURE_H

/*!
* @file
*  VideoNetServer_Configure.h
* @brief
*  远程配置结构定义
*  远程配置管理，系统配置结构体定义
* @author
*  ZW
* @date
*  2016年10月18日
* @version
*  1.0.0
*/


/**************************************************************************
*                           常量定义                                     *
************************************************************************* */

/** @brief 最大视屏输入数											                    		 */
#ifndef SYSTEM_SUPPORT_CHANNEL
#define DVR_MAX_VIDEOIN_NUM				(1)
#else
#define DVR_MAX_VIDEOIN_NUM				(SYSTEM_SUPPORT_CHANNEL)
#endif


/** @brief 最大PTZ数											                    			 */
#define DVR_MAX_PTZ_NUM					(DVR_MAX_VIDEOIN_NUM)

/** @brief 最大视频解码器个数										                    		 */
#define DVR_MAX_VIDEOENC_NUM			(DVR_MAX_VIDEOIN_NUM)

/** @brief 最大报警输入数											                    		 */
#define DVR_MAX_ALARMIN_NUM				(16)

/** @brief 最大报警输出数											                    		 */
#define DVR_MAX_ALARMOUT_NUM			(8)

/** @brief 最大异常个数											                    			 */
#define DVR_MAX_EXCEPTION_NUM			(8)

/** @brief 最大视屏输出个数											                    		 */
#define DVR_MAX_VIDEOOUT_NUM			(8)

/** @brief 一星期天数															          		 */
#define DVR_MAX_DAYS					(7)

/** @brief 一天最大时间段（每半小时一个时间段）						                    		 */
#define DVR_MAX_TIMESEGMENT				(24 * 2)

/** @brief 最大IP地址长度														          		 */
#define DVR_MAX_IP_STR_LEN				(16)

/** @brief 最大域名地址长度														          		 */
#define DVR_MAX_DOMAIN_STR_LEN		   (128)

/** @brief 最大MAC地址长度														          		 */
#define DVR_MAX_MAC_STR_LEN				(18)

/** @brief MAC地址数字个数															          	 */
#define DVR_MAX_MAC_NUM_LEN				(6)

/** @brief 最大支持DDNS协议个数													          		 */
#define DVR_MAX_DDNSPRO_NUM				(32)

/** @brief 最大DDNS协议名称长度													          		 */
#define DVR_MAX_DDNSPRONAME_LEN			(128)

/** @brief 最大域名长度															          		 */
#define DVR_MAX_DOMAINNAME_LEN 			(256)

/** @brief 最大Email长度														          		 */
#define DVR_MAX_EMAIL_LEN				(256)

/** @brief 设备最大用户数														          		 */
#define DVR_MAX_USER_NUM 				(16)

/** @brief 最大用户名长度														          		 */
#define DVR_MAX_USERNAME_LEN 			(64)

/** @brief 最大密码长度															          		 */
#define DVR_MAX_PASSWORD_LEN 			(64)

/** @brief 最大权限个数															          		 */
#define DVR_MAX_RIGHT_NUM				(32)

/** @brief 最大在线用户数																		*/
#define DVR_MAX_ONLINEUSER_NUM			(16)

/** @brief IP白名单数																			*/
#define DVR_MAX_WHITEIP_NUM				(10)

/** @brief IP黑名单数																			*/
#define DVR_MAX_BLACKIP_NUM				(10)

/** @brief 最大预置点个数															       		 */
#define DVR_MAX_PRESET_NUM				(128)

/** @brief 移动侦测区域个数														          		 */
#define DVR_MAX_MOTIONSCOPE_NUM			(64)

/** @brief 最大色度单元个数											          		 */
#define DVR_MAX_PICSEC_NUM				(24)

/** @brief 最大马赛克区域											          		 */
#define DVR_MAX_VIDEOSHELTER_NUM		(8)

/** @brief 字体名长度												          		 */
#define DVR_MAX_FONTNAME_LEN			(32)

/** @brief 点阵通道名最大长度,含结束符								          		 */
#define DVR_MAX_CHANNELNAME_LEN			(20 + 1)

/** @brief 安装信息通道名称							          		 */
#define DVR_MAX_COMPANYNAME_LEN			(20 + 1)

/** @brief 安装信息电话号码长度							          		 */
#define DVR_MAX_TEL_NUM_LEN			(20 + 1)

/** @brief PTZ协议名称长度															 */
#define DVR_MAX_PTZPRONAME_LEN			(48)

/** @brief PTZ协议个数(安装和未安装各256)											 */
#define DVR_MAX_PTZPRO_NUM				(64)

/** @brief 点阵宽度															    	 */
#define DVR_MAX_LATTICE_WIDTH			(24 * 10)

/** @brief 点阵高度													          		 */
#define DVR_MAX_LATTICE_HEIGHT			(24)

/** @brief 点阵总体大小												          		 */
#define DVR_MAX_LATTICE_BUFSIZE			(24 * 24 * 10 / 8)

/** @brief 设备节点名长度											          		 */
#define DVR_MAX_PARTITION_DEVNODE_LEN	(16)

/** @brief 单个硬盘中最大分区个数													 */
#define DVR_MAX_PARTITION_IN_HARDDISK	(15)

/** @brief 最大设备节点个数											          		 */
#define DVR_MAX_PARTITION_IN_DISKGROUP	(32)

/** @brief 最大硬盘个数												          		 */
#define DVR_MAX_HARDDISK_NUM 			(16)

/** @brief 最大硬盘组个数											          		 */
#define DVR_MAX_DISKGROUP_NUM 			(8)

/** @brief 最大支持分辨率个数										          		 */
#define DVR_MAX_VGARESOLVING_NUM		(16)

/** @brief 设备版本名称长度												          	 */
#define DVR_MAX_VERSION_LEN				(32)

/** @brief Hxht普通字符串长度														*/
#define DVR_COMMON_STRING_LEN			(32)

/** @brief 支持的协议库的最大个数													*/
#define DVR_MAX_SUPPORT_PROTOCOL_NUM	(32)

/** @brief WIFI热点名称最大长度														*/
#define DVR_MAX_SSID_LEN				(128)

/** @brief 可以识别的WIFI热点最大个数												 */
#define DVR_MAX_AP_NUM					(10)

/** @brief 可以识别的WIFI热点最大个数												 */
#define DVR_MAX_AP_NUM_V2				(32)

/** @brief WIFI密钥最大长度															*/
#define DVR_MAX_WIFI_KEY_LEN			(128)

/** @brief 最大源设备个数															*/
#define MAX_SOURCE_DEVICE_NUMBER		(16)

/** @brief 最大输出设备个数															*/
#define MAX_DEVICE_OUTPUT				(16)

//支持最大VGA个数
#define MAX_VGA_NUM						(8)

//TV输出
#define MAX_TV_NUM						(8) 

/** @brief ISP 参数模版个数 */
#define DVR_MAX_ISP_TEMPLATE		(6)

/** @brief ISP 参数模版个数 */
#define DVR_MAX_ISP_AEWINDOW		(6)

/** @brief ISP 参数白平衡模版个数 */
#define DVR_MAX_ISP_WHITEBALANCE	(6)

/** @brief 一个码流支持的最大ROI数目 */
#define MAX_ROI_NUM					4

/** @brief 一个码流支持的最大的移动侦测区域的数据 */
#define DVR_MAX_MOTION_REGION_NUM	16

/** @brief 针对中国移动的移动侦测区域的块数 */
#define DVR_MAX_CMDOT_MOTION_REGION_NUM_V3	25
/** @brief 针对中国移动的移动侦测区域的块数 */
#define DVR_MAX_CMDOT_MOTION_REGION_NUM_V3	25

/** @brief 一个码流支持的最大的人形侦测区域的数据 */
#define DVR_MAX_PEOPLE_DETECT_REGION_NUM	16

/** @brief 最大区域门限数量 ，包含最大门限与最小门限*/
#define DVR_MAX_REGION_THRESHOLD_NUM	2

/** @brief 一个码流支持的最大越界区域数量*/
#define DVR_MAX_TRAVERSE_PLANE_NUM	4

/** @brief 一个码流支持的最大区域入侵配置区域数量*/
#define DVR_MAX_FIELD_DETECT_NUM	4

/** @brief 默认矩形配置的点数量*/
#define DVR_MAX_RECT_POINT_NUM		4

/** @brief 设备列表一次传输的最大设备数 */
#define MAX_DEVICE_COUNT			32

/** @brief 保留字段长度 */
#define DVR_RESERVE_LEN				256

/** @brief 人脸库最大的图片条目 */
#define MAX_IMAGE_PERSON_ITEM 16

/** @brief 辅助接口最大数量 */
#define MAX_ASSISSTANT_INTERFACE_NUM 8

/** @brief 白名单数规则数												*/
#define DVR_MAX_WHITE_RULE_NUM			(32)

/** @brief 最大MD5长度												*/
#define MAX_MD5SUM_LEN						(128)


/** @brief 针对中国联通的移动侦测区域的块数 */
#define DVR_MAX_CUDOT_MOTION_REGION_NUM_V3	25

// 布防时间段最大个数
#define SCHEDULE_TIMESLOT_MAX_NUM 16


/**************************************************************************
*								协议名称定义			                  *
**************************************************************************/
/** @brief 私有协议														*/
#define	PROTOCOL_PRIVATE				0

/** @brief 手机协议														*/
#define	PROTOCOL_TIDEMOBILE				1

/** @brief 互信互通协议													*/
#define	PROTOCOL_HXHT					(1<<1)

//RTSP协议
#define	PROTOCOL_RTSP						(1<<2)

//Onvif协议
#define	PROTOCOL_ONVIF						(1<<3)

//TUTK协议
#define	PROTOCOL_TUTK						(1<<4)

//JSTP协议
#define PROTOCOL_JSTP						(1<<5)

//CGI协议
#define PROTOCOL_CGI						(1<<6)

//Goolink协议
#define PROTOCOL_GOOLINK					(1<<7)

//云存储协议
#define PROTOCOL_ZW_CLOUD_STORAGE			(1<<8)

//LongSee协议适配层
#define PROTOCOL_LONGSEE		(1<<9)

//MQTT协议适配层
#define PROTOCOL_MQTT			(1<<10)

//微信考勤机协议
#define PROTOCOL_WXGATEWAY		(1<<11)

//天翼协议
#define PROTOCOL_TIANYI			(1<<12)

//九霄协议
#define PROTOCOL_MEGLINK			(1<<13)

//金睿协议
#define PROTOCOL_FLOWCLIENT			(1<<14)

//GB28181协议
#define PROTOCOL_GB28181			(1<<15)

//CMDot协议
#define PROTOCOL_CMDOT				(1<<16)

//天翼协议
#define PROTOCOL_TIANYIV2				(1<<17)

//不启用私有协议
#define PROTOCOL_OFF_PRIVATE				(1<<18)

//LionKing协议
#define PROTOCOL_LIONKING				(1<<19)

//济工网
#define PROTOCOL_JIGONG				(1<<20)

//CMCC_OVD协议
#define PROTOCOL_CMCC_OVD				(1<<21)

//ZW_CUIOT_LTYF协议
#define PROTOCOL_CUIOT_LTYF			(1<<22)

//ZWebSocket协议
#define PROTOCOL_ZWEBSOCKET			(1<<23)

//P6SMessagePush协议
#define PROTOCOL_P6SMESSAGEPUSH			(1<<24)

//国际云存储协议
#define PROTOCOL_ZW_CLOUD_STORAGE_WORLD	(1<<25)

//CMDot协议
#define PROTOCOL_CUDOT				(1<<26)

//大匠智慧园区协议
#define PROTOCOL_DAJINAGIOT				(1<<27)

//P6SEvent协议
#define PROTOCOL_P6SEVENT				(1<<28)

//P6SCMS协议
#define PROTOCOL_P6SCMS					(1<<29)

//GA1400协议
#define PROTOCOL_GA1400					(1<<30)

//xxx协议
//#define PROTOCOL_XXX						(1<<8)

///////////////////////////////////////////////////////////////////////////////////////////////////////

/**************************************************************************
*                         探测配置信息主、次类型定义                      *
************************************************************************* */

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取网络参数												          	 */
#define HY_DVR_PROBE_GET_NETCFG 			17

/** @brief 设置网络参数												          	 */
#define HY_DVR_PROBE_SET_NETCFG 			18

//次类型
/** @brief 基本网络配置(HY_DVR_PROBE_NET_CFG)						          	 */
#define PROBE_NETCFG_BASE					1

/** @brief 无线连接热点配置(HY_DVR_PROBE_WLAN_CONNECTION)						 */
#define PROBE_NETCFG_WLAN_CONNECTION		2

/** @brief 热点列表配置(HY_DVR_PROBE_WLAN_ACCESSPOINT_CFG)						 */
#define PROBE_NETCFG_WLAN_AP_LIST			3


/**************************************************************************
*                             配置信息主、次类型定义                      *
************************************************************************* */



///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取网络参数												          	 */
#define HY_DVR_GET_NETCFG 					101

/** @brief 设置网络参数												          	 */
#define HY_DVR_SET_NETCFG 					102

//次类型
/** @brief 全部网络配置(HY_DVR_NET_CFG)								          	 */
#define NETCFG_ALL						 	0xFFFFFFFF

/** @brief Dhcp配置信息(HY_DVR_DHCP_CONF)										*/
#define NETCFG_DHCP_CONF					1

/** @brief Dhcp状态信息(HY_DVR_DHCP_STATE)							          	 */
#define NETCFG_DHCP_STATE					(1 << 1)

/** @brief Eth网络信息(HY_DVR_ETH_IF)								          	 */
#define NETCFG_ETH_IF						(1 << 2)

/** @brief PPPoE配置(HY_DVR_PPPOE_CONF)								          	 */
#define NETCFG_PPPOE_CONF					(1 << 3)

/** @brief PPPoE网络信息(HY_DVR_PPPOE_IF)							          	 */
#define NETCFG_PPPOE_IF						(1 << 4)

/** @brief DNS配置信息(HY_DVR_DNS_CONF)								          	 */
#define NETCFG_DNS_CONF						(1 << 5)

/** @brief DDNS配置信息(HY_DVR_DDNS_CONF)							          	 */
#define NETCFG_DDNS_CONF					(1 << 6)

/** @brief http配置(HY_DVR_HTTP_CONF)								          	 */
#define NETCFG_HTTP_CONF					(1 << 7)

/** @brief 服务端口配置信息(HY_DVR_LISTENPORT_CONF)						       	 */
#define NETCFG_LISTENPORT_CONF				(1 << 8)

/** @brief 手机端口配置信息(HY_DVR_MOBILE_CONF)							       	 */
#define NETCFG_MOBILE_CONF					(1 << 9)

/** @brief 自动发现端口配置信息(HY_DVR_AUTODETECT_CONF)					     	 */
#define NETCFG_AUTODETECT_CONF				(1 << 10)

/** @brief WIFI网络信息(HY_DVR_ETH_IF)											*/
#define NETCFG_WLAN_IF						(1 << 11)

/** @brief WIFI DHCP配置(HY_DVR_DHCP_CONF)										*/
#define NETCFG_WLAN_DHCP_CONF				(1 << 12)

/** @brief WIFI Dhcp状态信息(HY_DVR_DHCP_STATE)									*/
#define NETCFG_WLAN_DHCP_STATE				(1 << 13)

/** @brief WIFI DNS配置信息(HY_DVR_DNS_CONF)									*/
#define NETCFG_WLAN_DNS_CONF				(1 << 14)

/** @brief WIFI 热点列表(HY_DVR_WLAN_ACCESSPOINT_CFG)							*/
#define NETCFG_WLAN_ACCESSPOINTS			(1 << 15)

/** @brief WIFI 连接配置(HY_DVR_WLAN_CONNECTION)								*/
#define NETCFG_WLAN_CONNECTION				(1 << 16)

/** @brief WIFI 连接状态(HY_DVR_WLAN_STATE)										*/
#define NETCFG_WLAN_STATE					(1 << 17)

/** @brief UPNP 配置信息(HY_DVR_UPNP_CONF)										*/
#define NETCFG_UPNP_CONF					(1 << 18)

/** @brief 基本网络配置																	*/
#define NETCFG_BASIC_CONF					(1 << 19)

/** @brief 邮件服务器配置*/
#define NETCFG_EMAIL_CONF					(1 << 20)	

/** @brief FTP 配置信息(HY_DVR_FTP_CONF)										*/
#define NETCFG_FTP_CONF						(1 << 21)

/** @brief FTP 配置信息(HY_DVR_FTP_CONF_V2)										*/
#define NETCFG_FTP_CONF_V2					(1 << 22)

/** @brief FTP 获取中间层TUTK P2P等级对应的编码参数,内部使用，平台接入忽略此命令		*/
#define NETCFG_TUTK_CODEC_LEVEL_CONF		(1 << 23)

/** @brief 当前系统使用哪个网络接口 配置信息(HY_DVR_WHICH_NETCARD_USED)(只读),内部使用，平台接入忽略此命令	*/
#define NETCFG_WHICH_NETCARD_USED			(1 << 24)

/** @brief 当前系统使用哪个网络接口 配置信息(HY_DVR_WLAN_CONNECTION) ,内部使用，平台接入忽略此命令	*/
#define NETCFG_WIFI_CONNECT_PARAM_CHECK		(1 << 25)

/** @brief WIFI 连接状态检测（只针对WIFI连接配置的状态）,内部使用，平台接入忽略此命令				*/
#define NETCFG_WIFI_CONNECT_PARAM_CHECK_STATUS		(1 << 26)

/** @brief 通知给中间层悠洛克 P2P服务UID,内部使用，平台接入忽略此命令								*/
#define NETCFG_NOTIFY_ULUCO_P2P_UID			(0x1232)

/** @brief 通知给中间层悠洛克 P2P服务的状态,内部使用，平台接入忽略此命令							*/
#define NETCFG_NOTIFY_ULUCO_P2P_STATE		(0x1233)

/** @brief 通知给中间层TUTK P2P服务的状态,内部使用，平台接入忽略此命令								*/
#define NETCFG_NOTIFY_TUTK_STATE			(0x1234)

/** @brief 探测源通知(只读, 对应结构体,内部使用，平台接入忽略此命令)					*/
#define NETCFG_WHO_PROBE_CONF				(0x1235)
//4G 卡信息
#define NETCFG_4G_NETCARD_INFO				(0x1236)
//4G 卡配置
#define NETCFG_4G_NETCARD_CFG				(0x1237)

/** @brief WIFI 热点列表(HY_DVR_WLAN_ACCESSPOINT_CFG_V2)							*/
#define NETCFG_WLAN_ACCESSPOINTS_V2			(0x1238)

#define NETCFG_REMOTE_ALL  (0x1239)

#define NETCFG_TELNET_CONFIG				(0x1240)

//P2P连接串等信息（HY_P2P_CONNECT_INFO）
#define NETCFG_P2P_CONNECT_INFO		(4673)

//HY_DVR_WLAN_CONNECTION_V2   通过V2接口获取WIFI的信息
#define NETCFG_WLAN_CONNECTION_INFO_V2         (4674)

/** @brief WIFI 热点列表(HY_DVR_WLAN_ACCESSPOINT_CFG_SSID_INFO_V2)							*/
#define NETCFG_WLAN_ACCESSPOINTS_INFO_V2			(4675)

/** @brief IPv6地址信息(HY_DVR_IPV6_IPADDR_INFO) */
#define NETCFG_IPV6_IPADDR_INFO (4676)

/** @brief OVD协议QR模块初始化完成通知(HY_DVR_CMOVD_QR_INIT_NOTIFY) */
#define NETCFG_CMOVD_QR_INIT_NOTIFY	(4677)

/** @brief 获取WIFI的拓展配置信息(HY_DVR_NETCFG_WLAN_CONNECTION_INFO_EX) */
#define NETCFG_WLAN_CONNECTION_INFO_EX         (4678)

/** @brief 获取指定ssid的信息(HY_DVR_NETCFG_SSID_INFO) */
#define NETCFG_WLAN_SSID_INFO         (4679)

typedef enum {
	HY_DVR_PROTOCOL_TYPE_BEGIN,

	HY_DVR_PROTOCOL_TYPE_ADAPTRER,						// 自适应版本
	HY_DVR_PROTOCOL_TYPE_CLASSIC,						// 经典版本
	HY_DVR_PROTOCOL_TYPE_NEWBORN,						// 私有
	HY_DVR_PROTOCOL_TYPE_BAORM,				    	// P1
	HY_DVR_PROTOCOL_TYPE_CHIP,							// P2 
	HY_DVR_PROTOCOL_TYPE_ONVIF,							// ONVIF
	HY_DVR_PROTOCOL_TYPE_HDT,							// P3
	HY_DVR_PROTOCOL_TYPE_TST,							// P5

	HY_DVR_PROTOCOL_MAX_TYPEVALUE = HY_DVR_PROTOCOL_TYPE_TST  //协议版本中最大值
}eIPProtocolType;

/////微信项目宏定义
#define WX_MAX_USERINFO_NUMBER   500 //最多支持录入系统人数
#define WX_MAX_FACE_LIST_NUMBER  12   //人脸数据编号，支持0-11，共12个人脸数据
#define WX_MAX_FACE_DATA_SIZE    4096   //人脸特征数据大小                //此处可能要定义大一些
#define WX_MAX_RULE_LIST_NUMBER 10 // 一个成员支持的门禁规则   与微信商定为10条
#define WX_MAX_RULE_TIME_LIST_NUMBER 21    //与微信商定为  3*7 条
#define WX_MAX_CHECK_IN_PAGE_NUMBER  20//每一页的打卡记录条数
/////

/**************************************************************************
*                             配置信息主、次类型定义                      *
************************************************************************* */

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型

/** @brief 获取服务器配置参数												   	 */
#define HY_DVR_GET_NETSERVERCFG 			103

/** @brief 设置服务器配置参数												     */
#define HY_DVR_SET_NETSERVERCFG 			104

//次类型
/** @brief 全部网络配置(HY_DVR_NET_MANAGER)									     */
#define NETSERVERCFG_ALL					0xFFFFFFFF

/** @brief CMS配置信息(HY_DVR_NET_CMS)										 	 */
#define NETSERVERCFG_CMS_CONF				1

/** @brief CMS状态信息(HY_DVR_CMS_STATE)								    	 */
#define NETSERVERCFG_CMS_STATE				2

/** @brief AMS配置信息(HY_DVR_NET_AMS)										   	 */
#define NETSERVERCFG_AMS_CONF				4

/** @brief NTP配置信息(HY_DVR_NET_NTP)										  	 */
#define NETSERVERCFG_NTP_CONF				8

/** @brief EML配置信息(HY_DVR_NET_EML)										 	 */
#define NETSERVERCFG_EML_CONF				16

/** @brief EML(1.1.0版本)配置信息(HY_DVR_NET_EML_1)							 	 */
#define NETSERVERCFG_EML_1_CONF				32

/** @brief CMS协议信息														 	 */
#define NETSERVERCFG_CMS_PROTOCOL			64

/** @brief AMS配置信息(HY_DVR_NET_AMS_EX)										 */
#define NETSERVERCFG_AMS_EX_CONF			128

/** @brief 云平台配置信息(HY_DVR_NET_CLOUD_SERVER)								 */
#define NETSERVERCFG_CLOUD_SERVER_CONF		256


/** @brief 获取网络参数												          	 */
#define HY_DVR_GET_NETCFG_EX 		301

/** @brief 设置网络参数												          	 */
#define HY_DVR_SET_NETCFG_EX 		302
//次类型
// @brief 全部网络配置(HY_DVR_NET_CFG_EX)
#define NETCFGEX_ALL						 	0xFFFFFFFF

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取图像配置参数													  	 */
#define HY_DVR_GET_PICCFG 					105

/** @brief 设置图像配置参数													  	 */
#define HY_DVR_SET_PICCFG 					106

//次类型
/** @brief 全部网络配置(HY_DVR_PIC_CFG)										 	 */
#define PICCFG_ALL							0xFFFFFFFF

/** @brief 视频制式配置信息(没有实现)										 	 */
#define PICCFG_WORKMODE_CONF				1	/*工作模式                                              */

/** @brief OSD信息(没有实现)												   	 */
#define PICCFG_OSD_CONF						2

/** @brief 通道别名信息(结构体HY_DVR_PIC_CFG中的结构体HY_DVR_ALIAS_CFG)		   	 */
#define PICCFG_CHNAME_CONF					4

/** @brief 时间段信息(没有实现)												   	 */
#define PICCFG_TIMESEC_CONF					8

/** @brief 视频丢失配置信息(没有实现)										   	 */
#define PICCFG_VIDEOLOST_CONF				16

/** @brief 移动侦测配置信息(没有实现)										   	 */
#define PICCFG_MOTION_CONF					32

/** @brief 马赛克配置信息(没有实现)											   	 */
#define PICCFG_MOSAIC_CONF					64

/** @brief 通道别名信息（Unicode）(结构体HY_DVR_PIC_CFG中的结构体HY_DVR_ALIAS_CFG_1) */
#define PICCFG_CHNAME_UNICODE_CONF			128

/** @brief  NXP8850 Image Sensor Processing配置信息 */
#define PICCFG_NXP8850ISP_CONF				256

/** @brief 移动侦测联动配置信息，联动超过第32个通道的配置必须使用这个子命令配置,32路以内的设备可以忽略此子命令 */
#define PICCFG_MOTION_ACTION_CONF			512

/** @brief 视频丢失联动配置信息，联动超过第32个通道的配置必须使用这个子命令配置,32路以内的设备可以忽略此子命令 */
#define PICCFG_VLOSTS_ACTION_CONF			1024

/** @brief  视频相关高级配置配置, 编码类型(H264/H265), 走廊模式 其对应的结构体为HY_DVR_ADVANCED_CHANNEL_CFG*/
#define PICCFG_ADVANCED_CONF				2048

/** @brief 视频遮挡联动配置信息，联动超过第32个通道的配置必须使用这个子命令配置,32路以内的设备可以忽略此子命令 */
#define PICCFG_SHELTER_ACTION_CONF			4096

/** @brief  移动侦测第二个版本的配置信息(HY_DVR_MOTION_V2) */
#define PICCFG_MOTION_V2_CONF				8192

/** @brief  移动侦测第二个版本的配置信息(HY_DVR_MOTION_EX_LIST) */
#define PICCFG_MOTION_EX_CONF				16384

/** @brief  人形侦测配置信息(HY_DVR_PEOPLE_DETECT_CFG) */
#define PICCFG_PEOPLE_DETECT_CFG			16385

/** @brief  人脸检测配置信息(HY_DVR_PEOPLE_FACE_DETECT_CFG) */
#define PICCFG_PEOPLE_FACE_DETECT_CFG		16386

/** @brief  越界侦测配置信息(HY_DVR_TRAVERSE_PLANE_CFG) */
#define PICCFG_TRAVERSE_PLANE_DETECT_CFG	16387

/** @brief  区域入侵配置信息(HY_DVR_FIELD_DETECT_CFG) */
#define PICCFG_FIELD_DETECT_CFG				16388

/** @brief  红外人体检测配置信息(HY_DVR_PIR_CFG) */
#define PICCFG_PIR_CFG						16389

/** @brief  设置补光模式					 */
#define PICCFG_LIGHT_FILLING_MODE 16390

/** @brief  获取人形能力					 */
#define PICCFG_PEOPLE_DETECT_ABILITY 16391

/** @brief  获取人形能力					 */
#define PICCFG_PEOPLE_DETECT_EXTRA_INFO_SWITCH    16392

/** @brief  人形侦测配置信息(HY_DVR_PEOPLE_DETECT_CFG_V1) */
#define PICCFG_PEOPLE_DETECT_CFG_V1			16393

/** @brief  人形跟踪停留时间(HY_DVR_PEOPLE_DETECT_STAY_TIME) */
#define PICCFG_PEOPLE_DETECT_STAY_TIME			16394

/** @brief  车形检测(HY_DVR_CAR_DETECT) */
#define PICCFG_CAR_DETECT_CFG			16395

/** @brief  车形检测(HY_DVR_MOTION_TRACK_CONFIG) */
#define PICCFG_MOTION_TRACK_CFG			16396

/** @brief  基础OSD扩展(HY_DVR_OSD_CFG_EX_LIST) */
#define PICCFG_OSD_CONF_EX			16397

/** @brief  人形跟踪(HY_DVR_HUMANOID_TRACK) */
#define PICCFG_PEOPLE_TRACK            16398

//对应的结构体(HY_DVR_PICCFG_MOVE_TRACK)
#define PICCFG_MOVE_TRACK                       16399

/** @brief 动物侦测配置 (HY_DVR_ANIMAL_DETECT_CFG)                                  */
#define PICCFG_ANIMAL_DETECT_CFG                          16400     
/** @brief  移动侦测事件时间段配置(HY_DVR_PICCFG_MOTION_EX_CONF_EVENT_TIMESLOT_EX_LIST) */
#define PICCFG_MOTION_EX_CONF_EVENT_TIMESLOT_CFG                16401
/** @brief  移动侦测事件语音报警时间段配置(HY_DVR_PICCFG_MOTION_EX_CONF_VOICEALARM_TIMESLOT_EX_LIST) */
#define PICCFG_MOTION_EX_CONF_EVENT_VOICEALARM_TIMESLOT_CFG                16402

/** @brief  人形侦测事件时间段配置信息(HY_DVR_PICCFG_PEOPLE_DETECT_CFG_V1_EVENT_TIMESLOT_CFG) */ 
#define PICCFG_PEOPLE_DETECT_CFG_V1_EVENT_TIMESLOT_CFG              16403

/** @brief  雷达检测(HY_DVR_PICCFG_RADAR_DETECT_CFG) */
#define PICCFG_RADAR_DETECT_CFG                    16404
/** @brief  长久离家检测(HY_DVR_LONG_AWAY_HOME_DETECT_CFG) */
#define PICCFG_LONG_AWAY_HOME_DETECT_CFG			16405

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取编码配置参数													  	 */
#define HY_DVR_GET_COMPRESSCFG 				107

/** @brief 设置编码配置参数													  	 */
#define HY_DVR_SET_COMPRESSCFG 				108

//次类型
/** @brief 全部网络配置(HY_DVR_COMPRESSION_CFG)								  	 */
#define COMPRESSCFG_ALL						0xFFFFFFFF

/** @brief 工作模式配置信息(没有实现)										  	 */
#define COMPRESSCFG_WORKMODE_CONF			1

/** @brief 编码能力信息(没有实现)											 	 */
#define COMPRESSCFG_COMPRESS_CAP			2

/** @brief 编码配置信息(没有实现)												 */
#define COMPRESSCFG_COMPRESS_CONF			4

/** @brief 能力集																*/
#define COMPRESSCFG_CAPABILITY				8  

/** @brief  TUTK服务的视频等级,内部使用，平台接入忽略此命令(ZW_TUTK_QUALITY)						*/
#define COMPRESSCFG_TUTK_LEVEL				16

/** @brief  TUTK服务当前应该使用哪个流,内部使用，平台接入忽略此命令				*/
#define COMPRESSCFG_TUTK_STREAMINDEX		32

/** @brief 针对模拟设备的编码能力,内部使用，平台接入忽略此命令					*/
#define COMPRESSCFG_RESOLUTION_AND_FRAME_TABLE		64

/** @brief 针对模拟设备的编码能力,内部使用，平台接入忽略此命令					*/
#define COMPRESSCFG_RESOLUTION_AND_BITRATE_TABLE	128  

/*@bref 针对模拟设备视频输入的相关信息,内部使用，平台接入忽略此命令				*/
#define COMPRESSCFG_VIDEO_INPUT_INFO		256

/** @brief  编码能力的扩展,内部使用，平台接入忽略此命令	 	(HY_DVR_COMPRESSION_CFG_EX)	*/
#define COMPRESSCFG_ALL_EX					512

/** @brief  编码能力的扩展,内部使用，平台接入忽略此命令	 	(HY_DVR_COMPRESSION_CFG_EX_ALARM_SCHEDTIME)	*/
#define COMPRESSCFG_ALL_EX_ALARM_SCHEDTIME					513

/** @brief  编码能力的扩展(尚云) ,内部使用，平台接入忽略此命令 	(HY_DVR_COMPRESSION_QUALITY_AND_STREAM_TYPE_TABLE)	*/
#define COMPRESSION_QUALITY_AND_STREAM_TYPE_TABLE					1024

/** @brief 针对模拟设备的编码能力,内部使用，平台接入忽略此命令					*/
#define COMPRESSCFG_RESOLUTION_AND_FRAME_TABLE_EX		2048

/** @brief 能力集(HY_DVR_CODEC_CAP_V3)																*/
#define COMPRESSCFG_CAPABILITY_V3							4092  

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取本地录像参数														 */
#define HY_DVR_GET_RECORDCFG 				109

/** @brief 设置本地录像参数														 */
#define HY_DVR_SET_RECORDCFG 				110

//次类型
/** @brief 全部录像配置(HY_DVR_RECORD_SCHED)									 */
#define RECORDCFG_ALL						0xFFFFFFFF

/** @brief 录像控制状态配置(HY_DVR_RECORD_CTRLSTATE)							 */
#define RECORD_CTRL_STATE					1

/** @brief 录像码流选择(HY_DVR_RECORD_STREAM)									 */
#define RECORD_STREAM						2

/** @brief 获取DVR上最早录像时间和最晚录像时间												 */
#define RECODE_TIME_INFO					4

/** @brief 录像计划V2（HY_DVR_RECORD_SCHEDULE_V2）											 */
#define RECORDCFG_ALL_V2					5
/** @brief 录像布防时间计划（HY_DVR_RECORD_SCHEDULE_V2_EVENT_TIMESLOT_LIST）                                             */
#define RECORDCFG_ALL_V2_EVENT_TIMESLOT_CFG                    6

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取系统时间参数														 */
#define HY_DVR_GET_SYSTIME 					111

/** @brief 设置系统时间参数														 */
#define HY_DVR_SET_SYSTIME 					112

//次类型
/** @brief 系统时间配置(HY_DVR_TIME)											 */
#define SYSTIME_ALL							0xFFFFFFFF

//次类型
/** @brief 系统时间配置(HY_DVR_TIME_STAMP)											 */
#define SYSTIME_STAMP							0x1

/** @brief 系统时间配置(HY_DVR_UTC_TIMESTAMP)											 */
#define SYSTIME_UTC_TIMESTAMP			2

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取云台参数															 */
#define HY_DVR_GET_PTZCFG 					113

/** @brief 设置云台参数															 */
#define HY_DVR_SET_PTZCFG 					114

//次类型
/** @brief 云台参数配置(HY_DVR_PTZPRO_CFG)										 */
#define PTZCFG_ALL							0xFFFFFFFF

/** @brief 云台参数配置(HY_DVR_PTZ_LIMIT_POS)										 */
#define PTZCFG_LIMIT_POS					1

/** @brief 看护位置(HY_DVR_WATCH_CARE_PRESET)										 */
#define PTZCFG_WATCH_CARE_PRESET					2
/** @brief 云台校准配置(HY_DVR_PTZ_CALIBRATION_CFG)								*/
#define PTZ_CALIBRATION_CFG                      3
/** @brief 看守位信息(HY_DVR_HOME_POSITION_INFO)                                      */
#define HOME_POSITION_INFO                    6
/** @brief 看护位置(HY_DVR_WATCH_CARE_PRESET_ATTRIBUTE)                                        */
#define WATCH_CARE_PRESET_ATTRIBUTE           7

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取串口配置参数														 */
#define HY_DVR_GET_SERIALCFG 				115

/** @brief 设置串口配置参数														 */
#define HY_DVR_SET_SERIALCFG 				116

//次类型
/** @brief 全部串口参数配置(HY_DVR_DECODER_CFG)									 */
#define SERIALCFG_ALL						0xFFFFFFFF

/** @brief 电子PTZ配置(HY_DVR_PTZ_ELCTRONIC)									 */
#define SERIALCFG_PTZ_ELCTRONIC_CFG			1

/** @brief PTZ补充配置(HY_DVR_PTZ_REPLENISH_1)									 */
#define SERIALCFG_PTZ_REPLENISH_1_CFG		2

/* 预置点列表,内部使用，平台接入忽略此命令　对应结构体ZW_PTZ_PRESET_LIST*/
#define PRESET_LIST_CFG						4

/* 预置点列表,内部使用，平台接入忽略此命令　对应结构体ZW_DVR_PTZ_PRESET_LIST_FOR_NVR*/
#define PRESET_LIST_CFG_FOR_NVR				5

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取报警配置参数														 */
#define HY_DVR_GET_ALARMCFG 				117

/** @brief 设置报警配置参数														 */
#define HY_DVR_SET_ALARMCFG 				118

//次类型
/** @brief 全部报警参数配置(HY_DVR_ALARM_CFG)									 */
#define ALARMCFG_ALL						0xFFFFFFFF

/** @brief 报警拓展配置(HY_DVR_ALARM_CFG_EX)									*/
#define ALARMCFG_ALL_EX                   	0xFFFFFFFE

/** @brief 报警工作模式(没有实现)												 */
#define ALARMCFG_WORKMODE_CONF				1

/** @brief 快照配置(没有实现)													 */
#define ALARMCFG_IMGCAPTURE_CONF			2

/** @brief 报警输入配置(没有实现)												 */
#define ALARMCFG_ALARMIN_CONF				4

/** @brief 报警输出配置(没有实现)												 */
#define ALARMCFG_ALARMOUT_CONF				8

/** @brief 异常处理配置(没有实现)												 */
#define ALARMCFG_EXCEPTION_CONF				16

/** @brief 报警输入联动配置, 超过第32路的报警输入配置必须使用此子命令获取和设置联动配置，未超过32路报警输入的设备可以忽略此命令 */
#define ALARMCFG_ACTION_MULTICHANNEL_CONF	32
/** @brief 快照配置V2(没有实现)( HY_DVR_ALARM_CAPTURE_PIC_V2)				*/
#define ALARMCFG_IMGCAPTURE_CONF_V2	     	64

/** @brief 设置IPC受控模式													 */
#define ALARMCFG_ALARMOUT_CONTROL_MODE  128
/** @brief 前端所有报警通道配置													 */
#define ALARMCFG_ALARMOUT_CONF_REMOTE				256

/** @brief 设置报警布防配置													 */
#define ALARMCFG_ALARMOUT_PREVENTION_CONTROL		257

/** @brief 设置门径输出配置			HY_DVR_ALARMOUT_GATE		 */
#define ALARMCFG_ALARMOUT_GATE		258

/** @brief 设置门径输出配置			HY_DVR_ALARMOUT_GATE_LIST		 */
#define ALARMCFG_ALARMOUT_GATE_LIST		259

/** @brief 布防框			HY_DVR_ALARM_OVERLAY_INFO		 */
#define ALARMCFG_ALARM_OVERLAY_INFO		260

/** @brief 多边形			HY_DVR_POLYGON_CONFIG		 */
#define ALARMCFG_ALARM_POLYGON_AREA_INFO		261

/** @brief 拌线设置			HY_DVR_ALARM_ASSISSTANT_LINE_INFO		 */
#define ALARMCFG_ALARM_ASSISSTANT_LINE_INFO		262

/** @brief 拌线布防时间			HY_DVR_ALARM_ASSISSTANT_LINE_INFO		 */
#define ALARMCFG_ALARM_ASSISSTANT_LINE_SCHEDULE		263

/** @brief 多边形布防时间			HY_DVR_POLYGON_SCHEDULE		 */
#define ALARMCFG_ALARM_POLYGON_SCHEDULE		264

/** @brief 报警配置（声音，灯光等）			HY_DVR_ALARM_VOICE_LIGHT_CONFIG		 */
#define ALARMCFG_ALARM_VOICE_LIGHT_CONFIG		265

/** @brief 获取报警状态（声音，灯光等）			HY_DVR_VOICE_LIGHT_STATE		 */
#define ALARMCFG_ALARM_VOICE_LIGHT_STATE		266

/** @brief 灯光报警配置			HY_DVR_LIGHT_WARNING		 */
#define ALARMCFG_ALARM_LIGHT_CONFIG		267

/** @brief 灯光报警配置			HY_DVR_ALARM_OUT_SCHEDULE		 */
#define ALARMCFG_ALARM_OUT_SCHEDULE		268

/** @brief 远瞻客户需求（声音报警布防时间）		 */
#define ALARMCFG_ALARM_VOICE_SCHEDULE	269

/** @brief 移动侦测布防时间			HY_DVR_SCHEDTIME		 */
#define ALARMCFG_ALARM_MOTION_SCHEDULE		270

/** @brief 人形侦测布防时间			HY_DVR_SCHEDTIME		 */
#define ALARMCFG_ALARM_PEOPLE_SCHEDULE		271

/** @brief 车型侦测布防时间			HY_DVR_SCHEDTIME		 */
#define ALARMCFG_ALARM_CAR_SCHEDULE		272

/** @brief 模拟报警输入			HY_DVR_SIMULATE_DIGITAL_INPUT		 */
#define ALARMCFG_ALARM_SIMULATE_DIGITAL_INPUT		273

/** @brief 报警输出布防计划V1			HY_DVR_ALARM_OUT_SCHEDULE_V1		 */
#define ALARMCFG_ALARM_OUT_SCHEDULE_V1		274

/** @brief 灯光报警配置扩展配置            HY_DVR_LIGHT_WARNING_EX         */
#define ALARMCFG_ALARM_LIGHT_CONFIG_EX      275

/** @brief 一键报警配置（声音，灯光，报警输出等）          HY_DVR_ALARMCFG_ONE_CLICK_ALARM_CONFIG         */
#define ALARMCFG_ONEC_LICK_ALARM_CONFIG     276

/** @brief 一键报警状态（声音，灯光，报警输出等）          HY_DVR_ALARMCFG_ONE_CLICK_ALARM_STATUS         */
#define ALARMCFG_ONE_CLICK_ALARM_STATUS     277

/** @brief 一键报警控制（声音，灯光，报警输出等）          HY_DVR_ALARMCFG_ONE_CLICK_ALARM_CONTROL         */
#define ALARMCFG_ONE_CLICK_ALARM_CONTROL    278

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取显示输出配置参数													 */
#define HY_DVR_GET_VIDEOOUTCFG 				119

/** @brief 设置显示输出配置参数													 */
#define HY_DVR_SET_VIDEOOUTCFG 				120

//次类型
/** @brief 全部显示输出参数配置(HY_DVR_VO_CFG)									 */
#define VIDEOOUTCFG_ALL						0xFFFFFFFF


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取用户信息参数														 */
#define HY_DVR_GET_USERCFG 					121

/** @brief 设置用户信息参数														 */
#define HY_DVR_SET_USERCFG 					122

//次类型
/** @brief 用户信息参数(HY_DVR_USER_CFG)										 */
#define USERCFG_ALL							0xFFFFFFFF

/** @brief 单个用户信息(HY_DVR_USER)											 */
#define USERCFG_ONE							1

/** @brief 添加用户(HY_DVR_USER)												 */
#define USERCFG_CREATE						2

/** @brief 删除用户(HY_DVR_USER)												 */
#define USERCFG_REMOVE						4

/** @brief 在线用户(HY_DVR_ONLINEUSER_CFG)										*/
#define USERCFG_USER_ONLINE					8

/** @brief 强制删除单个在线用户(HY_DVR_ONLINEUSER)								*/
#define USERCFG_USER_ONLINE_ONE				16

/** @brief 是否启用用户白名单(HY_DVR_WHITEIP_ENABLE)							*/
#define USERCFG_WHITE_IP_ENABLE				32

/** @brief 用户白名单(结构体HY_DVR_USER_CFG中的HY_DVR_WHITEIP)					*/
#define USERCFG_WHITE_IP					64

/** @brief 用户黑名单列表(HY_DVR_BLACKIP_LIST)							*/
#define USERCFG_BLACK_IP_LIST				128

/** @brief 用户黑名单列表(HY_DVR_MODIFY_PASSWORD_BY_TEMP_SECRET_KEY)							*/
#define MODIFY_PASSWORD_BY_TEMP_SECRET_KEY	256

/** @brief 多个用户权限(HY_DVR_USER_CFG)							*/
#define USERCFG_PERMISSION_CONFIG_ALL		512
//
///** @brief 单个用户权限(ZW_USER_BASIC_CFG)							*/
#define USERCFG_PERMISSION_CONFIG_ONE		1024

/** @brief 多个用户权限(HY_DVR_USER_PERMISSIONS_V1)							*/
#define USERCFG_PERMISSION_CONFIG_ALL_V1		2048

/** @brief 单个用户权限(HY_DVR_USER_PERMISSION_V1)							*/
#define USERCFG_PERMISSION_CONFIG_ONE_V1		4096


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取设备信息参数														 */
#define HY_DVR_GET_DEVICEINFO  123

/** @brief 设置设备信息参数，只读												 */
#define HY_DVR_SET_DEVICEINFO  124

//次类型
/** @brief 全部设备信息参数(HY_DVR_DEVICE_INFO)									 */
#define DEVICEINFO_ALL  0xFFFFFFFF

//次类型
/** @brief 设备的序列号,内部使用，平台接入忽略此命令							 */
#define DEVICEINFO_SERIAL_NUM  1

/** @brief 自定义信息参数,内部使用，平台接入忽略此命令	(HY_DVR_CUSTOMER_INFO),(只读)								 */
#define DEVICEINFO_CUSTOMER  2

/** @brief 设备信息参数(HY_DVR_DEVICE_INFO_V2)									 */
#define DEVICEINFO_V2  0x03

/** @brief 长视设备参数配置									 */
#define LONGSEE_DEVICECONFIG  4

/** @brief 云升级设备信息									 */
#define CLOUD_UPDATE_DEVICE_INFO  5

/** @brief 获取设备硬件能力（中国移动）	HY_DVR_DEVICE_HW_CAPABILITY								 */
#define CM_DEVICE_HW_CAP  6

/** @brief 获取设备软件能力（中国移动）	HY_DVR_DEVICE_SW_CAPABILITY								 */
#define CM_DEVICE_SW_CAP  7

/** @brief 自定义信息参数,内部使用，平台接入忽略此命令	(HY_DVR_CUSTOMER_INFO_LIST),(只读)								 */
#define DEVICEINFO_CUSTOMER_LIST  8

/** @brief 设备固件安装进度(HY_DVR_UPGRADE_INFO)									 */
#define UPGRADE_INFO  9

/** @brief 获取设备硬件能力（运营商/中国联通）								 */
#define CUDOT_DEVICE_HW_CAP  10

/** @brief 获取设备软件能力（运营商/中国联通）								 */
#define CUDOT_DEVICE_SW_CAP  11

/** @brief 获取设备软件能力扩展（中国移动）	HY_DVR_DEVICE_SW_CAPABILITY_EX							 */
#define CM_DEVICE_SW_CAP_EX  12

/** @brief 获取设备软件能力扩展（中国联通）	HY_DVR_DEVICE_SW_CAPABILITY_EX							 */
#define CUDOT_DEVICE_SW_CAP_EX  13

/** @brief 获取设备信息（onvif多目功能，区分多通道是真的还是假的，临时）	HY_DVR_DEVICEINFO_FOR_ONVIF_MULTI							 */
#define DEVICEINFO_FOR_ONVIF_MULTI  14

/** @brief 获取设备运行信息							 */
#define DEVICE_RUNNING_INFO  15

/** @brief 获取设备状态信息 (HY_DVR_DEVICE_STATUS_INFO)                         */
#define DEVICE_STATUS_INFO  16

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取设备配置参数														 */
#define HY_DVR_GET_DEVICECFG 			125

/** @brief 设置设备配置参数														 */
#define HY_DVR_SET_DEVICECFG				126

//次类型
/** @brief 全部设备配置参数(HY_DVR_DEVICE_CFG)									 */
#define DEVICECFG_ALL						0xFFFFFFFF

/** @brief 锁屏时间参数(没有实现)												 */
#define DEVICECFG_LOCKSCREENTIME_CONF		1

/** @brief 语言(HY_DVR_SYSTEM_LANGUAGE_CONFIG)														 */
#define DEVICECFG_LANGUAGE_CONF				2

/** @brief 日期时间格式(没有实现)												 */
#define DEVICECFG_DATETIME_CONF				4

/** @brief 夏时制(没有实现)														 */
#define DEVICECFG_DST_CONF					8

/** @brief 设备ID，用于遥控器(没有实现)											 */
#define DEVICECFG_REMOTECONTROL_ID			16

/** @brief 时区(没有实现)														 */
#define DEVICECFG_TIMEZONE_CONF				32

/** @brief 视频制式配置信息(没有实现)											 */
#define DEVICECFG_VIDEOSTANDARD_CONF		64

/** @brief 设备安装信息															 */
#define DEVICECFG_INSTALLTION_CONF			128

/** @brief 可供升级的版本列表,对应结构体:UPGRADE_FIRMWARE_INFO_LIST					*/
#define CLOUD_UPGRADE_FIRMWARE_LIST				(1<<8)

/** @brief 开始升级, 对应结构体: UPGRADE_CONTROL_START							*/
#define CLOUD_UPGRADE_START_UPGRADE				(1<<9)

/** @brief 停止升级(UPGRADE_CONTROL_STOP)（未实现,暂不支持取消升级，因此开始升级要很谨慎）	*/
#define CLOUD_UPGRADE_STOP_UPGRADE				(1<<10)

/** @brief 升级状态(下载进度,中断,或者其它状态, 对应结构体: UPGRADE_RESULT					*/
#define CLOUD_UPGRADE_STATUS					(1<<11)

/** @brief 升级版本列表文件服务器位置,对应的结构体:UPGRADE_LIST_SERVER_CFG		*/
#define CLOUD_UPGRADE_FIRMWARE_LIST_LOCATION	(1<<12)

/** @brief 请求设备开始下载升级文件, 对应的结构体:UPGRADE_FILE_DOWNLOAD_START	*/
#define CLOUD_UPGRADE_FILE_DOWNLOAD_START				(1<<13)

/** @brief 获取设备下载升级文件的进度, 对应的结构体:UPGRADE_FILE_DOWNLOAD_STATUS	*/
#define CLOUD_UPGRADE_FILE_DOWNLOAD_STATUS		(1<<14)

/** @brief 请求设备终止下载, 对应的结构体:UPGRADE_FILE_DOWNLOAD_STOP	*/
#define CLOUD_UPGRADE_FILE_DOWNLOAD_STOP		(1<<15)

/** @brief 辅助接口开启状态	*/
#define DEVICECFG_ASSISTANT_INTERFACE_CONTROL		129

/** @brief 设备工作模式（测试模式，正常模式）	*/
#define DEVICECFG_SYSTEM_WORK_MODE_CONTROL		130

/** @brief 设备绑定信息（S码）HY_DEVICE_BIND_CONFIG	*/
#define DEVICECFG_BIND_CONFIG		131

/** @brief 删除语音定制 HY_DELETE_VOICE_CUSTOM	*/
#define DEVICECFG_DELETE_VOICE_CUSTOM		132

/** @brief 设备GUI语言 HY_DVR_SYSTEM_LANGUAGE_CONFIG	*/
#define DEVICECFG__LANGUAGE		133

/** @brief 设备安装位置 HY_FIBERHOME_LOCATION_INFO	*/
#define DEVICECFG_FIBERHOME_LOCATION_INFO		134

/** @brief 设备基本位置 HY_FIBERHOME_BASE_INFO	*/
#define DEVICECFG_FIBERHOME_BASE_INFO		135

/** @brief 中心服务器信息 HY_FIBERHOME_CTRL_CHNL_SERVER	*/
#define DEVICECFG_FIBERHOME_CTRL_CHNL_SERVER		136

/** @brief 位置编码ID HY_FIBERHOME_LOCATION_ID	*/
#define DEVICECFG_FIBERHOME_LOCATION_ID		137

/** @brief 数据服务器信息 HY_FIBERHOME_DATA_CENTER_SERVER	*/
#define DEVICECFG_FIBERHOME_DATA_CENTER_SERVER		138

/** @brief 所在区域编码 HY_FIBER_HOME_AREA_CODE	*/
#define DEVICECFG_FIBERHOME_AREA_CODE		139

/** @brief LOG路径 HY_FIBERHOME_LOG_PATH	*/
#define DEVICECFG_FIBERHOME_LOG_PATH		140

/** @brief 经纬度信息 HY_FIBERHOME_LONLAT_INFO	*/
#define DEVICECFG_FIBERHOME_LONLAT_INFO		141

/** @brief 视频服务器信息 HY_DVR_VIDEO_SERVER_URL	*/
#define DEVICECFG_FIBERHOME_VIDEO_SERVER_URL		142

/** @brief 设备智能算法工作模式 HY_DVR_AI_WORK_MODE	*/
#define DEVICECFG_AI_WORK_MODE		143

/** @brief 删除语音定制 HY_DELETE_LANGUAGE_CUSTOM	*/
#define DEVICECFG_DELETE_LANGUAGE_CUSTOM		144

/** @brief 删除语音定制 HY_SCREEN_CLOSE_TIME	*/
#define DEVICECFG_SCREEN_CLOSE_TIME		145

/** @brief S码校验功能（S码）HY_DEVICE_BIND_CONFIG	*/
#define DEVICECFG_SCODE_VALIDATE		146

/** @brief 定制语言名称 HY_DVR_CUSTOM_LANGUAGE_NAME	*/
#define DEVICECFG_CUSTOM_LANGUAGE_NAME		147

/** @brief 开始云升级 UPGRADE_FILE_INFO	*/
#define CLOUD_UPGRADE_START_UPGRADE_WITH_FILE_INFO		148

/** @brief 设备语言是否显示 HY_DVR_LANGUAGE_DISPLAY_STATUS	*/
#define DEVICE_LANGUAGE_DISPLAY_INFO		149

/** @brief 设备定制语言点阵开关 HY_DVR_CUSTOM_LANGUAGE_LATTICE_SWITCH	*/
#define DEVICE_CUSTOM_LANGUAGE_LATTICE_SWITCH		150

/** @brief 设备平台模式 HY_DVR_PLATFORM_MODE	*/
#define DEVICE_PLATFORM_MODE		151

/** @brief 低温拟合开关 HY_DVR_LOW_TEM_FIT_SWITCH	*/
#define DEVICE_LOW_TEM_FIT_SWITCH		152

/** @brief NFC开关 HY_DVR_NFC_SWITCH	*/
#define NFC_BASIC_CONFIG    153

/** @brief 双目标定 HY_DVR_DOUBLE_CAMERA_CALIBRATION	*/
#define DOUBLE_CAMERA_CALIBRATION	154

/** @brief 梯控 HY_DVR_ELEVATOR_BASIC_CONFIG	*/
#define ELEVATOR_CONFIG	155

/** @brief 推送时间 HY_DVR_PUSH_EVENT_INTERVAL	*/
#define PUSH_EVENT_INTERVAL	156

/** @brief 老化测试 HY_DVR_AGING_TEST	*/
#define AGING_TEST	157

/** @brief 每一个通道安装信息列表(ZW_MULTI_CHANNEL_INSTALLTION_INFO)					    */
#define DEVICECFG_INSTALLTION_CONF_LIST			158

/** @brief 济工网平台配置信息	*/
#define HY_DVR_JGPLATFORM_CFG		159

/** @brief 济工网平台连接状态	*/
#define HY_DVR_JGPLATFORM_CONNECT_STATUS		160

/** @brief 请求设备开始下载升级文件, 对应的结构体:UPGRADE_FILE_DOWNLOAD_START_V1	*/
#define CLOUD_UPGRADE_FILE_DOWNLOAD_START_V1				(161)

/** @brief 事件推送全局配置(HY_DVR_PUSH_EVENT_GLOBAL_CFG)  */
#define PUSH_EVENT_GLOBAL_CFG                 162

/** @brief AF机芯升级状态(HY_DVR_AF_BOARD_UPGRADE_STATUS)                                  */
#define AF_BOARD_UPGRADE_STATUS                     163

/** @brief NTP立即同步信息(HY_DVR_NTP_SYNCHRONIZE_NOW_INFO)                                    */
#define NTP_SYNCHRONIZE_NOW_INFO                        164

/** @brief 获取可读模式的夏令时信息(HY_DVR_DST_INFO)                                    */
#define HY_DVR_DEVICE_DST_INFO                        165

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取存储系统信息														 */
#define HY_DVR_GET_STORAGEINFO				127

/** @brief 设置存储系统信息，只读												 */
#define HY_DVR_SET_STORAGEINFO				128

//次类型
/** @brief 全部设备信息参数(HY_DVR_STORAGE_CFG)									 */
#define STORAGEINFO_ALL						0xFFFFFFFF

/** @brief 存储系统工作模式参数(没有实现)										 */
#define STORAGEINFO_WORKMODE_CONF			1

/** @brief 硬盘信息参数(没有实现)												 */
#define STORAGEINFO_DISK_INFO				2

/** @brief 磁盘组信息参数(没有实现)												 */
#define STORAGEINFO_DISKGROUP_INFO			4

/** @brief 磁盘效验码(没有实现)													 */
#define STORAGEINFO_DISK_MARK				8


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取设备状态信息														 */
#define HY_DVR_GET_DEVICESTATE				129

/** @brief 设置设备状态信息，空出不用											 */
#define HY_DVR_SET_DEVICESTATE				130

//次类型
/** @brief 全部设备状态信息(HY_DVR_DEVICE_STATE)								 */
#define DEVICESTATE_ALL						0xFFFFFFFF
//次命令
//获取设备连接状态（HY_DVR_CONNECT_STATE）
#define CONNECT_STATE_ALL					(1)


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取设备自动维护配置													 */
#define HY_DVR_GET_DEVICEMAINTENANCE		131

/** @brief 设置设备自动维护配置													 */
#define HY_DVR_SET_DEVICEMAINTENANCE		132

//次类型
/** @brief 全部设备自动维护配置(HY_DVR_DEVICE_MAINTENANCE)						 */
#define DEVICEMAINTENANCE_ALL				0xFFFFFFFF


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取设备功能定制配置													 */
#define HY_DVR_GET_DEVICECUSTOM				133

/** @brief 设置设备功能定制配置													 */
#define HY_DVR_SET_DEVICECUSTOM				134

//次类型
/** @brief 全部设备功能定制配置(HY_DVR_DEVICE_CUSTOM)							 */
#define DEVICECUSTOM_ALL					0xFFFFFFFF

/** @brief HC板型编解码资源分配模式信息（暂时不用）								 */
#define DEVICECUSTOM_HCRESMODE_CONF			8

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取HXHT配置(没有实现)												 */
#define HY_DVR_GET_HXHT						135

/** @brief 设置HXHT配置(没有实现)												 */
#define HY_DVR_SET_HXHT						136

//次类型
/** @brief 全部HXHT配置(没有实现)												 */
#define HXHT_ALL							0xFFFFFFFF

/** @brief NAT状态配置(没有实现)												 */
#define	HXHT_NATSTATUS						1

/** @brief 语音呼叫配置(没有实现)												 */
#define	HXHT_VOICEPARAM						2

/** @brief CMS存储注册配置信息(没有实现)										 */
#define HXHT_CMS_STORAGE					4

/** @brief 前端设备ID(没有实现)													 */
#define HXHT_DEVICE_ID						8


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取要加载的协议名称（私有协议是必须加载的，不在其中）(没有实现)		 */
#define	HY_DVR_GET_PROTOCOL					137

/** @brief 设置要加载的协议名称	(没有实现)										 */
#define	HY_DVR_SET_PROTOCOL					138

//次类型
/** @brief 全部要加载的协议名称(没有实现)										 */
#define PROTOCOL_ALL						0xFFFFFFFF


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取源设备配置														 */
#define	HY_DVR_GET_SOURCE_DEVICE			139

/** @brief 设置源设备配置														 */
#define	HY_DVR_SET_SOURCE_DEVICE			140

//次类型
/** @brief 全部源设备配置(HY_DVR_SOURCE_DEVICE_INFO_CONFIG)						 */
#define SOURCE_DEVICE_ALL					0xFFFFFFFF


///////////////////////////////////////////////////////////////////////////////////////////////////////
////主类型
///** @brief 获取设备输出														*/
//#define	HY_DVR_GET_DEVICE_OUTPUT			141
//
///** @brief 设置设备输出(无效命令，只能获取，不能设置)							*/
//#define	HY_DVR_SET_DEVICE_OUTPUT			142
//
////次类型
///** @brief 全部设备输出(HY_DVR_DEVICE_OUTPUT)									*/
//#define DEVICE_OUTPUT_ALL					0xFFFFFFFF


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取解码器显示设备信息												*/
#define	HY_DVR_GET_DISP_DEV					143

/** @brief 设置解码器显示设备信息												*/
#define	HY_DVR_SET_DISP_DEV					144

//次类型
/** @brief 所有解码器显示设备信息(HY_DVR_DISPDEV_CFG)							*/
#define DISP_DEV_ALL						0xFFFFFFFF

/** @brief VGA信息(HY_DVR_VGAPARA_CFG)											*/
#define DISP_DEV_VGA						1

/** @brief TV 信息(HY_DVR_VOOUT_CFG)											*/
#define DISP_DEV_TV							2

///////////////////////////////////////////////////////////////////////////////////////////////////////
/** @brief 获取NXP8850 ISP参数													*/
#define	HY_DVR_GET_NXP8850ISP				145
/** @brief 设置NXP8850 ISP参数													*/
#define	HY_DVR_SET_NXP8850ISP				146
//次类型
/** @brief 全部ISP参数															*/
#define NXP8850ISP_ALL						0xFFFFFFFF
///////////////////////////////////////////////////////////////////////////////////////////////////////


/**************************************************************************
*                        IPC的ISP配置信息主、次类型定义                      *
************************************************************************* */

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取ISP配置参数												          	 */
#define HY_DVR_IPC_GET_ISPCFG 			147

/** @brief 设置ISP参数												          	 */
#define HY_DVR_IPC_SET_ISPCFG 			148

//次类型
/** @brief 颜色配置(ZW_IPC_COLOR)						          	 */
#define IPC_ISPCFG_COLOR					1

/** @brief 曝光配置(ZW_IPC_EXPOSURE)					          	 */
#define IPC_ISPCFG_EXPOSURE					2

/** @brief 白平衡配置(ZW_IPC_WHITEBLANCE)				          	 */
#define IPC_ISPCFG_WHITEBLANCE				3

/** @brief 日夜切换配置(ZW_IPC_DAY_NIGHT)					         */
#define IPC_ISPCFG_DAY_NIGHT				4

/** @brief 日夜切换配置(ZW_IPC_BASIC)					          	 */
#define IPC_ISPCFG_BASIC					5

/** @brief 日夜切换配置(ZW_IPCISP_DEFAULT)					         */
#define IPC_ISPCFG_DEFAULT					6

/** @brief ISP参数默认配置V2(ZW_IPCISP_DEFAULT_V2)                   */
#define IPC_ISPCFG_DEFAULT_V2               7

/** @brief ISP所有配置(ZW_IPC_ISP_CONFIG)					         */
#define IPC_ISPCFG_ALL					(0xFFFFFFFF)

/**************************************************************************
*                        NVR的ISP配置信息主、次类型定义 （该命令仅适用于NVR设备）                     *
************************************************************************* */

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取NVR的ISP配置参数												          	 */
#define HY_DVR_NVR_GET_ISPCFG 			149

/** @brief 设置NVR的ISP参数												          	 */
#define HY_DVR_NVR_SET_ISPCFG 			150

//次类型
/** @brief 颜色配置(ZW_NVR_COLOR)						          	 */
#define NVR_ISPCFG_COLOR					1

/** @brief 曝光配置(ZW_NVR_EXPOSURE)					          	 */
#define NVR_ISPCFG_EXPOSURE					2

/** @brief 白平衡配置(ZW_NVR_WHITEBLANCE)				          	 */
#define NVR_ISPCFG_WHITEBLANCE				3

/** @brief 日夜切换配置(ZW_NVR_DAY_NIGHT)					         */
#define NVR_ISPCFG_DAY_NIGHT				4

/** @brief 日夜切换配置(ZW_NVR_BASIC)					          	 */
#define NVR_ISPCFG_BASIC					5

/** @brief 恢复默认												*/
#define NVR_ISPCFG_DEFAULT					6

/** @brief ISP所有配置(ZW_IPC_ISP_CONFIG)					         */
#define NVR_ISPCFG_ALL					(0xFFFFFFFF)

/** @brief 曝光参数												*/
#define NVR_ISPCFG_FACE_SMART_EXPOSURE					7


/**************************************************************************
*         ISP配置扩展信息主、次类型定义 （该命令适用于所有设备）        *
************************************************************************* */
//主类型
/** @brief 获取设备的ISP配置参数												          	 */
#define HY_DVR_COM_GET_ISPCFG_EX 			151

/** @brief 设置设备的ISP参数												          	 */
#define HY_DVR_COM_SET_ISPCFG_EX 			152

//次命令
/** @brief ISP扩展的所有配置(ZW_IPC_ISP_CONFIG) */
#define COM_ISPCFG_EX_ALL								(0xFFFFFFFF)

/** @brief ISP扩展的所有配置(HY_DVR_IR_STATUS) */
#define COM_ISPCFG_IR_STATUS								1

/** @brief ISP扩展的所有配置的扩展(ZW_COM_ISP_EX_EX) */  
#define COM_ISPCFG_EX_ALL_EX                                2

/**************************************************************************
*         ISP色彩相关的配置 （该命令适用于所有设备）        *
************************************************************************* */
//主类型
/** @brief 获取设备的ISP配置参数												          	 */
#define HY_DVR_GET_TONE_ISP 			153

/** @brief 设置设备的ISP参数												          	 */
#define HY_DVR_SET_TONE_ISP 			154

//次命令
/** @brief ISP扩展的所有配置(ZW_IPC_ISP_CONFIG) */
#define TONE_ISP_ALL								(0xFFFFFFFF)

/** @brief ISP设置rgb2yuv默认*/
#define TONE_ISP_RGB2YUV_DEFAULT				1

/**************************************************************************
*               ROI相关的配置 （该命令适用于所有设备）                    *
************************************************************************* */
//主类型
/** @brief 获取设备的ROI配置参数										  */
#define HY_DVR_GET_ROI_CFG 			155

/** @brief 设置设备的ROI参数											  */
#define HY_DVR_SET_ROI_CFG 			156

//次命令
#define ROI_CFG_ALL								(0xFFFFFFFF)


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取设备信息扩展参数														 */
#define HY_DVR_GET_DEVICEINFO_EX 				157

/** @brief 设置设备信息扩展参数，只读												 */
#define HY_DVR_SET_DEVICEINFO_EX				158

//次类型
/** @brief 全部设备信息扩展参数(HY_DVR_DEVICE_INFO_EX)									 */
#define DEVICEINFO_EX_ALL						0xFFFFFFFF

// 次类型
/** @brief 用户自定义数据,文本内容,协议不关注具体内容(HY_DVR_DEVICE_INFO_EX)			*/
#define USER_DEFINED_TEXT						0x01

// 次类型
/** @brief 所有onvif 客户定制的信息(HY_DVR_ONVIF_CUSTOMIZED) 
*/
#define ONVIF_CUSTOMIZED_INFO 					0x02

// 次类型
/** @brief 所有onvif 客户定制的信息(HY_DVR_ONVIF_CUSTOMIZED_EX) 
*/
#define ONVIF_CUSTOMIZED_INFO_EX 					0x03

// 次类型
/** @brief 设备运行状态(HY_DVR_DEVICE_OPERATE_STATUS)
*/
#define DEVICE_OPERATE_STATUS					0x04

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取报警配置扩展参数（内部使用，平台接入忽略此命令）														 */
#define HY_DVR_GET_ALARMCFG_EX 				161

/** @brief 设置报警配置扩展参数（内部使用，平台接入忽略此命令）														 */
#define HY_DVR_SET_ALARMCFG_EX 				162

//次类型
/** @brief 全部报警扩展参数配置（内部使用，平台接入忽略此命令）(HY_DVR_ALARM_CFG_EX)									 */
#define ALARMCFG_EX_ALL						0xFFFFFFFF


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取HTTP Event配置（内部使用，平台接入忽略此命令）															 */
#define HY_DVR_GET_HTTP_EVENT_CFG 				163

/** @brief 设置Http Event配置（内部使用，平台接入忽略此命令）															 */
#define HY_DVR_SET_HTTP_EVENT_CFG 				164

//次类型
/** @brief 全部Http Event配置（内部使用，平台接入忽略此命令）(ZW_HTTP_EVENT_CFG)									 */
#define HTTP_EVENT_ALL					0xFFFFFFFF


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取系统资源使用配置（内部使用，平台接入忽略此命令）															 */
#define HY_DVR_GET_BUFFER_SIZE 				171

/** @brief 设置系统资源使用配置（内部使用，平台接入忽略此命令）															 */
#define HY_DVR_SET_BUFFER_SIZE 				172

//次类型
/** @brief 全部系统资源使用配置（内部使用，平台接入忽略此命令）(HY_DVR_BUFFER_SIZE)									 */
#define BUFFER_SIZE_ALL					0xFFFFFFFF


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取系统资源使用配置（内部使用，平台接入忽略此命令）															 */
#define HY_DVR_GET_CONNECT_COUNT 			173

/** @brief 设置系统资源使用配置（内部使用，平台接入忽略此命令）															 */
#define HY_DVR_SET_CONNECT_COUNT 			174

//次类型
/** @brief 全部系统资源使用配置（内部使用，平台接入忽略此命令）(HY_DVR_CONNECT_COUNT)									 */
#define CONNECT_COUNT_ALL					0xFFFFFFFF

/** @brief 全部系统资源使用配置（内部使用，平台接入忽略此命令）(CLIENT_COUNT_ADD)									 */
#define CLIENT_COUNT_ADD					0x1

/** @brief 全部系统资源使用配置（内部使用，平台接入忽略此命令）(CLIENT_COUNT_SUB)									 */
#define CLIENT_COUNT_SUB					0x2

/** @brief 全部系统资源使用配置（内部使用，平台接入忽略此命令）(MAIN_REAM_COUNT_ADD)									 */
#define MAIN_REAM_COUNT_ADD					0x3

/** @brief 全部系统资源使用配置（内部使用，平台接入忽略此命令）(MAIN_REAM_COUNT_SUB)									 */
#define MAIN_REAM_COUNT_SUB					0x4

/** @brief 全部系统资源使用配置（内部使用，平台接入忽略此命令）(SUB_REAM_COUNT_ADD)									 */
#define SUB_REAM_COUNT_ADD					0x5

/** @brief 全部系统资源使用配置（内部使用，平台接入忽略此命令）(SUB_REAM_COUNT_SUB)									 */
#define SUB_REAM_COUNT_SUB					0x6

#define NOTIFY_CLIENT						0x7

#define NOTIFY_REAL_STREAM					0x8

//主类型
/** @brief 获取编码配置参数（内部使用，平台接入忽略此命令）													  	 */
#define HY_DVR_GET_COMPRESSCFG_V2 				181

/** @brief 设置编码配置参数（内部使用，平台接入忽略此命令）													  	 */
#define HY_DVR_SET_COMPRESSCFG_V2 				182

//子命令
/** @brief 全部网络配置（内部使用，平台接入忽略此命令）(HY_DVR_COMPRESSION_CFG_V2)								  	 */
#define COMPRESSCFG_ALL_V2						0xFFFFFFFF

/** @brief 全部网络配置（内部使用，平台接入忽略此命令）(HY_DVR_COMPRESSION_CHANNEL_V2)								  	 */
#define COMPRESSCFG_ONE_CHANNEL_V2						1

/** @brief 远程桌面码流															*/
#define COMPRESSCFG_REMOTE_DESKTOP_STREAM		0x02

/** @brief 获取帧率范围															*/
#define FRAME_RATE_RANGE 3

//主类型
/** @brief 获取设备能力描述文件名称（内部使用，平台接入忽略此命令）												  	 */
#define HY_DVR_GET_DEVICE_CAPACITY_FILE_NAME 				183

/** @brief 设置设备能力描述文件名称（内部使用，平台接入忽略此命令）													 */
#define HY_DVR_SET_DEVICE_CAPACITY_FILE_NAME 				184

/** @brief 	所有的设备能力描述文件名称（内部使用，平台接入忽略此命令）			  							     */
#define DEVICE_CAPACITY_ALL						0xFFFFFFFF

//主类型
/** @brief 获取推送协议描述（内部使用，平台接入忽略此命令）												  	 */
#define HY_DVR_GET_PUSH_PROTOCOL_DESC 				185

/** @brief 设置推送协议描述（内部使用，平台接入忽略此命令，此处不支持设置）													 */
#define HY_DVR_SET_PUSH_PROTOCOL_DESC 				186

/** @brief 	所有的设备能力描述文件名称（内部使用，平台接入忽略此命令）(HY_DVR_PUSH_PROTOCOL_DESC)			  				 */
#define PUSH_PROTOCOL_ALL						0xFFFFFFFF

//主类型
/** @brief 获取RTMP码流推送协议配置												  	 */
#define HY_DVR_GET_RTMP_PUSH_CONFIG 				187

/** @brief 设置RTMP码流推送协议配置													 */
#define HY_DVR_SET_RTMP_PUSH_CONFIG 				188

/** @brief 所有有关RTMP推流配置,　对应结构体为: HY_DVR_RTMP_PUSH_CONFIG			  	 */
#define RTMP_PUSH_CONFIG_ALL						0xFFFFFFFF

//主类型
/** @brief 获取设备能力描述文件名称（内部使用，平台接入忽略此命令）												  	 */
#define HY_DVR_GET_DEVICE_CAPACITY_FILE_NAME_V2 				189

/** @brief 设置设备能力描述文件名称（内部使用，平台接入忽略此命令）													 */
#define HY_DVR_SET_DEVICE_CAPACITY_FILE_NAME_V2 				190

/** @brief 	所有的设备能力描述文件名称（内部使用，平台接入忽略此命令）			  							     */
#define DEVICE_CAPACITY_V2_ALL						0xFFFFFFFF

/** @brief 	灯板能力描述文件名称（内部使用，平台接入忽略此命令）			  							     */
#define DEVICE_LIGHT_CAPACITY						1

/** @brief 	PTZ能力描述文件名称（内部使用，平台接入忽略此命令）			  							     */
#define DEVICE_PTZ_CAPACITY						2

/** @brief 	AI能力描述文件名称（内部使用，平台接入忽略此命令）			  							     */
#define DEVICE_AI_CAPACITY						3

//主类型
/** @brief 获取设备能力描述（内部使用，平台接入忽略此命令）												  	 */
#define HY_DVR_GET_DEVICE_CAPACITY_BY_BUFFER 				191

/** @brief 设置设备能力描述（内部使用，平台接入忽略此命令）													 */
#define HY_DVR_SET_DEVICE_CAPACITY_BY_BUFFER 				192

/** @brief 	所有的设备能力描述文件名称（内部使用，平台接入忽略此命令）			  							     */
#define DEVICE_CAPACITY_BY_BUFFER						0xFFFFFFFF

///////////////////////////////////////////////////////////////////////////////////////////////////////

/** @brief 获取应用层特性参数													*/
#define	HY_DVR_GET_APP_FEATURE				200
/** @brief 设置应用层特性参数													*/
#define	HY_DVR_SET_APP_FEATURE				201
//次类型
/** @brief 无效值																*/
#define APP_FEATURE_ALL						0xFFFFFFFF

/// 是否要用202，203这个命令，还要与小欢协商，保持所有产品统一，别弄冲突了，正式发布时，删掉此注释
/** @brief 获取扩展OSD配置													*/
#define	HY_DVR_GET_EXTENT_OSD				202
/** @brief 设置扩展OSD配置													*/
#define	HY_DVR_SET_EXTENT_OSD				203
//次类型
/** @brief OSD列表																*/
#define EXTENT_OSD_LIST						0xFFFFFFFF
/** @brief 添加一个OSD, 主命令为HY_DVR_GET_EXTENT_OSD有效						*/
#define EXTENT_ADD_OSD_NODE					0x01
/** @brief 删除一个OSD, 主命令为HY_DVR_GET_EXTENT_OSD有效						*/
#define EXTENT_DEL_OSD_NODE					0x02
/** @brief 设置一个OSD, 主命令为HY_DVR_SET_EXTENT_OSD有效						*/
#define EXTENT_SET_OSD_NODE					0x04
/** @brief 获取一个OSD, 主命令为HY_DVR_GET_EXTENT_OSD有效						*/
#define EXTENT_GET_OSD_NODE					0x08


/** @brief 获取扩展OSD配置														*/
#define	HY_DVR_GET_OSD_V2					204
/** @brief 设置扩展OSD配置														*/
#define	HY_DVR_SET_OSD_V2					205
//次类型
/** @brief OSD列表(HY_DVR_OSD_V2)												*/
#define OSD_INFO_V2							0xFFFFFFFF
/** @brief OSD列表(HY_DVR_OSD_V3)												*/
#define OSD_INFO_V3							1
/** @brief OSD列表(HY_DVR_OSD_V4)												*/
#define OSD_INFO_V4							2

// 查询协议交互错误详细信息

// 主类型
#define HY_DVR_GET_DEVICE_ERR_INFO								220

// 次类型, 对应结构体: HY_DVR_PROTOCOL_INTERACT_ERROR_INFO
#define PROTOCOL_INTERACT_ERR_INFO								1

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取网络参数												          	 */
#define HY_DVR_GET_NETCFG_EX 		301

/** @brief 设置网络参数												          	 */
#define HY_DVR_SET_NETCFG_EX 		302
//次类型
// @brief 全部网络配置(HY_DVR_NET_CFG_EX)
#define NETCFGEX_ALL						0xFFFFFFFF

// const int NETCFGEX_REMOTE_ALL = 1;

/**************************************************************************
*               NVR,HVR通道管理部分:　设备连接列表命令                    *
************************************************************************* */
//主类型
/** @brief 获取设备连接列表										  */
#define HY_DVR_GET_DEVICES_CONNECT_LIST 			400

/** @brief 设置设备连接列表											  */
#define HY_DVR_SET_DEVICES_CONNECT_LIST 			401

//次命令
//
#define DEVICES_CONNECT_LIST_ALL								(0xFFFFFFFF)



/**************************************************************************
*               修改NVR,HVR通道管理:　连接的前端设备IP                    *
************************************************************************* */
//主类型
/** @brief 获取NVR连接的设备的网络信息										  */
#define HY_DVR_GET_DEVICE_NETWORK 					402

/** @brief 设置NVR连接的设备的网络信息											  */
#define HY_DVR_SET_DEVICE_NETWORK 					403

//次命令
//
#define DEVICE_NETWORK_ALL								(0xFFFFFFFF)



/**************************************************************************
*				获取NVR,HVR通道管理中前端设备的连接状态                    *
************************************************************************* */
//主类型
/** @brief 获取NVR连接的设备的连接状态										  */
#define HY_DVR_GET_DEVICE_STATE 					404

/** @brief 设置NVR连接的设备的连接状态（不可用，设备状态是只读的）			 */
#define HY_DVR_SET_DEVICE_STATE 					405

//次命令
//
#define DEVICE_STATE_ALL								(0xFFFFFFFF)


/**************************************************************************
*				获取设备支持连接的客户端的协议列表
************************************************************************* */
//主类型
/** @brief 获取设备支持连接的客户端的协议列表(HY_DVR_DEVICE_SUPPORT_CLIENT_PROTOCOL_LIST)									  */
#define HY_DVR_GET_DEVICE_SUPPORT_CLIENT_PROTOCOL_LIST 					406

/** @brief 设置设备支持连接的客户端的协议列表			 */
#define HY_DVR_SET_DEVICE_SUPPORT_CLIENT_PROTOCOL_LIST 					407

//次命令
//
#define DEVICE_SUPPORT_CLIENT_PROTOCOL_ALL				(0xFFFFFFFF)




// 获取轮转列表命令(仅解码器IPD支持此命令),对应的结构体为HY_DVR_PREVIEW_LIST
#define HY_DVR_GET_PREVIEW_LIST		(500)
// 设置轮转列表命令(仅解码器IPD支持此命令),对应的结构体为HY_DVR_PREVIEW_LIST
#define HY_DVR_SET_PREVIEW_LIST		(501)

#define PREVIEW_LIST_CFG_ALL				(0xFFFFFFFF)

// 解码器显示通道放大控制命令定义
#define HY_DVR_FULLSCREEN_WINDOW		(502)

// 解码器显示通道缩小控制命令定义
#define HY_DVR_RETURN_FROM_FULLSCREEN	(503)

#define CONTROL_PREVIEW_ALL				(0xFFFFFFFF)


// 设置和获取解码器的屏幕分辨率(主命令)
#define HY_DVR_GET_MONITOR_RESOLUTION		(504)

#define HY_DVR_SET_MONITOR_RESOLUTION		(505)

// 获取当前设备支持的能力(次命令)
#define MONITOR_CAP									(0x01)			// 对应结构体HY_DVR_RESOLUTION_CAP

// 设置当前屏分辨率配置(次命令)
#define MONITOR_CONFIG								(0x02)			// 对应结构体HY_DVR_RESOLUTION_LIST


/** @brief  设备自定义功能 														*/
#define HY_DVR_GET_DEVICE_CUSTOM_FUCNTION 				600

/** @brief 	设备自定义功能	    												*/
#define HY_DVR_SET_DEVICE_CUSTOM_FUNCTION				601

/** @brief 字符串																*/
#define DEVICE_CUSTOM_ALL								0xFFFFFFFF

/** @brief HTTP消息透传																*/
#define DEVICE_HTTP_MESSAGE_TRANSMISSION				1

/** @brief 设备测试URL															*/
#define DEVICE_TEST_URL									2


/** @brief  录像文件列表属性													*/
#define HY_DVR_GET_RECORD_FILE_LIST_ATTRIBUTE 			602

/** @brief 	录像文件列表属性	    											*/
#define HY_DVR_SET_RECORD_FILE_LIST_ATTRIBUTE			603

/** @brief 字符串	( HY_DVR_RECORD_FILE_LIST_ATTRIBUTE )															*/
#define RECORD_FILE_LIST_ATTRIBUTE_ALL					0xFFFFFFFF

#ifndef FILE_LIST_ATTR_SEARCHTIME
/** @brief  搜索的时间段(HY_DVR_FILE_LIST_ATTRIBUTE_SEARCH_BY_TIME)										*/
#define FILE_LIST_ATTR_SEARCHTIME						1
#endif

/** @brief  录像文件当前页														*/
#define HY_DVR_GET_RECORD_FILE_CURRENT_PAGE 			604

/** @brief 	录像文件当前页		    											*/
#define HY_DVR_SET_RECORD_FILE_CURRENT_PAGE				605

/** @brief 字符串（HY_DVR_RECORD_FILE_CURRENT_PAGE）																*/
#define RECORD_FILE_CURRENT_PAGE_ALL					0xFFFFFFFF


/** @brief  录像文件列表														*/
#define HY_DVR_GET_RECORD_FILE_LIST 					606

/** @brief 	录像文件列表		    											*/
#define HY_DVR_SET_RECORD_FILE_LIST						607

/** @brief 字符串（HY_DVR_RECORD_FILE_LIST）																*/
#define RECORD_FILE_LIST_ALL							0xFFFFFFFF

/** @brief 字符串（HY_DVR_RECORD_FILE_LIST_EX）																*/
#define RECORD_FILE_LIST_ALL_EX							0xFFFFFFFE

/** @brief 字符串（HY_DVR_RECORD_FILE_LIST_V1）																*/
#define RECORD_FILE_LIST_ALL_V1							1

/** @brief 通过文件名获取文件信息																*/
#define RECORD_FILE_INFO_BY_FILENAME					2


/** @brief  抓图文件列表属性													*/
#define HY_DVR_GET_SNAPSHOT_FILE_LIST_ATTRIBUTE 		608

/** @brief 	抓图文件列表属性	    											*/
#define HY_DVR_SET_SNAPSHOT_FILE_LIST_ATTRIBUTE			609

#ifndef FILE_LIST_ATTR_SEARCHTIME
/** @brief  搜索的时间段(HY_DVR_FILE_LIST_ATTRIBUTE_SEARCH_BY_TIME)										*/
#define FILE_LIST_ATTR_SEARCHTIME						1
#endif

/** @brief 抓图文件列表属性信息(HY_DVR_SNAPSHOT_FILE_LIST_ATTRIBUTE)			*/
#define SNAPSHOT_FILE_LIST_ATTRIBUTE_ALL				0xFFFFFFFF


/** @brief  抓图文件当前页														*/
#define HY_DVR_GET_SNAPSHOT_FILE_CURRENT_PAGE 			610

/** @brief  抓图文件当前页		    											*/
#define HY_DVR_SET_SNAPSHOT_FILE_CURRENT_PAGE			611

/** @brief 抓图文件当前页信息 (HY_DVR_SNAPSHOT_FILE_CURRENT_PAGE)				*/
#define SNAPSHOT_FILE_CURRENT_PAGE_ALL					0xFFFFFFFF


/** @brief  抓图文件列表														*/
#define HY_DVR_GET_SNAPSHOT_FILE_LIST 					612

/** @brief 	抓图文件列表		    											*/
#define HY_DVR_SET_SNAPSHOT_FILE_LIST					613

/** @brief 设置抓图文件列表属性(HY_DVR_SNAPSHOT_FILE_LIST)						 */
#define SNAPSHOT_FILE_LIST_ALL							0xFFFFFFFF

/** @brief 设置抓图文件列表属性Ex(HY_DVR_SNAPSHOT_FILE_LIST_EX)					 */
#define SNAPSHOT_FILE_LIST_ALL_EX						0xFFFFFFFE





#define HY_DVR_GET_PUSH_PROTOCOL_INFO					700

#define HY_DVR_SET_PUSH_PROTOCOL_INFO					701

#define	PUSH_PROTOCOL_INFO_ALL							0xFFFFFFFF


#define HY_DVR_GET_TIMED_CAPTURE_CFG				702

#define HY_DVR_SET_TIMED_CAPTURE_CFG				703

#define	TIMED_CAPTURE_CFG_ALL						0xFFFFFFFF

#define TIMED_CAPTURE_CFG_EX                    1

#define HY_DVR_GET_DEVICE_CONTROL					704

#define HY_DVR_SET_DEVICE_CONTROL				705

#define DEVICE_CONTROL_EMAIL						0x1	


/** @brief 事件配置列表 														*/
#define HY_DVR_GET_EVENT_CFG_LIST						706						

/** @brief 事件配置列表 														*/
#define HY_DVR_SET_EVENT_CFG_LIST						707

/** @brief 字符串																*/
#define EVENT_CFG_LIST_ALL								0xFFFFFFFF	

/** @brief 获取一张图 															*/
#define HY_DVR_GET_PICTURE_BY_BUFFER					708

/** @brief 获取一张图（该主命令无效）											*/
#define HY_DVR_SET_PICTURE_BY_BUFFER					709

/** @brief 获取一张图（HY_DVR_PICTURE_BY_BUFFER）								*/
#define  PICTURE_BY_BUFFER_ALL							0xFFFFFFFF
//次类型
// @brief 获取图片属性（HY_DVR_PICTURE_ATTRIBUTE）
#define PICTURE_ATTRIBUTE                    2

/** @brief 获取授权信息 															*/
#define HY_DVR_GET_AUTH_INFO							710
/** @brief 设置授权信息											*/
#define HY_DVR_SET_AUTH_INFO							711
/** @brief 设置授权信息 (HY_DVR_AUTH_INFO)											*/
#define AUTH_INFO_ALL									0xFFFFFFFF
/** @brief 设置授权信息V2 (HY_DVR_AUTH_INFO_V2)											*/
#define AUTH_INFO_ALL_V2					1			
/** @brief 设置推送域名列表信息 (HY_DVR_PUSH_DOMAIN_LIST)											*/
#define AUTH_INFO_PUSH_DOMAIN_LIST			2		
/** @brief 设置透传信息 (HY_DVR_AUTH_INFO_TRANSPARENT_INFO)                                            */
#define AUTH_INFO_TRANSPARENT_INFO                      3    
/** @brief 设置保活服务器域名列表信息 (HY_DVR_KEEP_ALIVE_DOMAIN_LIST)                                            */
#define AUTH_INFO_KEEP_ALIVE_DOMAIN_LIST                      4    
/** @brief 设置WSManager服务器域名列表信息 (HY_DVR_WS_MANAGER_DOMAIN_LIST)                                            */
#define AUTH_INFO_WS_MANAGER_DOMAIN_LIST                      5   
/** @brief P2P信息 (HY_DVR_AUTH_INFO_P2P_INFO)                                            */
#define AUTH_INFO_P2P_INFO                      				6


/** @brief 获取PTZ的高级配置 （HY_PTZ_ADVANCE_CONFIG）								*/
#define HY_DVR_GET_PTZ_ADVANCE_CONFIG					712
/** @brief 设置PTZ的高级配置 （HY_PTZ_ADVANCE_CONFIG）								*/
#define HY_DVR_SET_PTZ_ADVANCE_CONFIG					713

#define PTZ_ADVANCE_CONFIG_ALL							0xFFFFFFFF

#define PTZ_ADVANCE_CONFIG_ALL_V1							1

/** @brief 获取声音报警的高级配置 （HY_AUDIO_ALARM_CONFIG）								*/
#define HY_DVR_GET_AUDIO_ALARM_CONFIG					714
/** @brief 设置声音报警的高级配置 （HY_AUDIO_ALARM_CONFIG）								*/
#define HY_DVR_SET_AUDIO_ALARM_CONFIG					715

#define AUDIO_ALARM_CONFIG_ALL							0xFFFFFFFF

#define AUDIO_ALARM_CONFIG_ALL_CHECK					1

#define AUDIO_ALARM_CONFIG_V1		2

#define AUDIO_ALARM_CONFIG_ALL_CHECK_V1					3
/** @brief 声音报警配置V1的报警布防计划/时间 （HY_DVR_AUDIO_ALARM_CONFIG_V1_ALARM_SCHEDULE）                             */
#define AUDIO_ALARM_CONFIG_V1_ALARM_SCHEDULE        4
/** @brief 声音报警的高级配置EX （HY_AUDIO_ALARM_CONFIG_EX）                               */
#define AUDIO_ALARM_CONFIG_ALL_EX                       5
/** @brief 声音报警时间段 （HY_DVR_AUDIO_ALARM_CONFIG_TIMESLOT_LIST）                               */
#define AUDIO_ALARM_CONFIG_TIMESLOT_LIST                      6
/** @brief 声音报警消除配置 （HY_DVR_AUDIO_ALARM_ELIMINATE）                              */
#define AUDIO_ALARM_ELIMINATE                           7


/** @brief 获取声音报警的高级配置 （HY_DVR_CONNECT_RESOURCE_INFO）								*/
#define HY_DVR_GET_CONNECT_RESOURCE_INFO				716
/** @brief 设置声音报警的高级配置 （HY_DVR_CONNECT_RESOURCE_INFO）								*/
#define HY_DVR_SET_CONNECT_RESOURCE_INFO				717

#define CONNECT_RESOURCE_INFO_ALL						0xFFFFFFFF


/** @brief 获取声音报警的高级配置 								*/
#define HY_DVR_GET_EVENT_EX_ACTION							718
/** @brief 设置声音报警的高级配置								*/
#define HY_DVR_SET_EVENT_EX_ACTION							719
//订阅（HY_SUBSCRIBE_INFO）
#define EVENT_EX_ACTION_SUBSCRIBE							1
//取消订阅（HY_UNSUBSCRIBE_INFO）					
#define EVENT_EX_ACTION_UNSUBSCRIBE							2

#define EVENT_EX_ACTION_ALL								0xFFFFFFFF


/** @brief 获取人脸抓拍相关信息								*/
#define HY_DVR_GET_FACE_SNAP								720
/** @brief 设置人脸抓拍相关信息								*/
#define HY_DVR_SET_FACE_SNAP								721

//人脸抓拍区域过滤（HY_DVR_FACE_FILTER_REGION）
#define FACE_SNAP_FILTER_REGION								1
//人脸抓拍配置（HY_DVR_FACE_SNAP_CONFIG）					
#define FACE_SNAP_CONFIG									2
//人脸抓拍叠加信息(HY_DVR_FACE_SNAP_OVERLAY_INFO)
#define FACE_SNAP_OVERLAY_INFO								4
//人脸检测区域(HY_DVR_FACE_DETECT_REGION)
#define FACE_SNAP_DETECT_REGION								8
//所有的人脸抓拍信息
#define FACE_SNAP_ALL								0xFFFFFFFF
//获取默认人脸探测区域和默认最大最小人脸大小
#define FACE_DEFAULT_FACE_REGION_INFO		16
//固定探测区域配置（HY_FIXED_REGION_DETECT）
#define FACE_FIXED_REGION_DETECT_INFO		17
//固定探测区域配置（HY_DVR_ORIGIN_IMAGE_CONFIG）
#define FACE_ORIGIN_IMAGE_CONFIG		18
//设置当前模式为人脸抓拍模式，不进行人脸识别，设置为此模式后Web不停的尝试获取抓拍到的人脸，获取到后进行人脸入库（HY_DVR_FACE_SNAPSHOT_MODE）
#define FACE_SNAPSHOT_MODE		19
//19命令字模式下抓拍到的人脸图片（人脸图片以二进制方式返回，Web根据Content-Length判断是否成功）
#define FACE_SNAPSHOT_IMAGE		20

/** @brief 获取客户端和流量等统计信息								*/
#define HY_DVR_GET_ALL_CLIENT_AND_FLOW_STATISTICS								722
/** @brief 设置客户端和流量等统计信息								*/
#define HY_DVR_SET_ALL_CLIENT_AND_FLOW_STATISTICS								723
//人脸抓拍区域过滤（HY_DVR_ALL_CLIENT_AND_FLOW_STATISTICS）
//所有的人脸抓拍信息
#define ALL_CLIENT_AND_FLOW_STATISTICS_ALL								0xFFFFFFFF


//主类型
/** @brief 获取报警配置扩展参数（内部使用，平台接入忽略此命令）														 */
#define HY_DVR_GET_ALARMCFG_EX_V2 				724

/** @brief 设置报警配置扩展参数（内部使用，平台接入忽略此命令）														 */
#define HY_DVR_SET_ALARMCFG_EX_V2 				725

//次类型
/** @brief 全部报警扩展参数配置（内部使用，平台接入忽略此命令）(HY_DVR_ALARM_CFG_EX_V2)									 */
#define ALARMCFG_EX_V2_ALL						0xFFFFFFFF


//主命令
#define HY_DVR_GET_FACE_GROUP  726
#define HY_DVR_SET_FACE_GROUP  727

//次命令
//对应的结构体（HY_DVR_CREATE_FACE_GROUP）
#define CREATE_FACE_GROUP			 		1
//对应的结构体（HY_DVR_MODIFY_FACE_GROUP）
#define MODIFY_FACE_GROUP			 		2
//对应的结构体（HY_DVR_DELETE_FACE_GROUP）
#define DELETE_FACE_GROUP			 		3
//对应的结构体（HY_DVR_FACE_GROUP_LIST）
#define QUERY_ALL_FACE_GROUPS        		4
//对应的结构体（HY_DVR_FACE_GROUP）
#define QUERY_FACE_GROUP_BY_GROUP_ID 		5
//设置人脸亮度
#define FACE_BRIGHTNESS	 6
//脸库通行权限（HY_DVR_FACE_GROUP_PASS_PERMISSION）
#define FACE_GROUP_PASS_PERMISSION			 		7
//对应的结构体（HY_DVR_FACE_GROUP_LIST_V1）
#define QUERY_ALL_FACE_GROUPS_V1        		8
//对应的结构体（HY_DVR_CREATE_FACE_GROUP）
#define CREATE_FACE_GROUP_V1			 		9
//对应的结构体（HY_DVR_MODIFY_FACE_GROUP_V1）
#define MODIFY_FACE_GROUP_V1			 		10
//对应的结构体（HY_DVR_FACE_GROUP_LIST_V2）
#define QUERY_ALL_FACE_GROUPS_V2        		11
//对应的结构体（HY_DVR_FACE_GROUP_V1）
#define QUERY_FACE_GROUP_BY_GROUP_ID_V1 		12
//对应的结构体（HY_DVR_MODIFY_FACE_GROUP_LIST_V1）
#define MODIFY_FACE_GROUP_LIST_V1			 		13
//对应的结构体(HY_DVR_DELETE_FACE_GROUP_V1)
#define DELETE_FACE_GROUP_V1                     14


//查询人脸识别模块的工作状态
//主命令
#define HY_DVR_GET_WORK_STATE  728
#define HY_DVR_SET_WORK_STATE  729

//次命令
//对应的结构体（HY_DVR_FACE_RECO_MODULE_WORK_STATE）
#define QUERY_WORK_STATE	   1
//对应的结构体（HY_DVR_FACE_BASE_CONFIG）
#define FACE_BASE_CONFIG	   2


//操作人员信息
//主命令
#define HY_DVR_GET_PERSON_INFO  730
#define HY_DVR_SET_PERSON_INFO  731

//次命令
//对应的结构体（HY_DVR_CHECK_PERSON_INFO）
#define CHECK_PERSON_INFO			1
//对应的结构体（HY_DVR_ADD_PERSON_INFO_AND_IMAGE_INFO）
#define ADD_PERSON_INFO_AND_IMAGE_INFO	2
//对应的结构体（HY_DVR_DELETE_PERSON_INFO_LIST）
#define DELETE_PERSON_INFO_LIST	    3
//对应的结构体（HY_DVR_MODIFY_PERSON_INFO）
#define MODIFY_PERSON_INFO			4
//对应的结构体（HY_DVR_UPDATE_IMAGE_INFO）
#define UPDATE_IMAGE_INFO			5
//对应的结构体（HY_DVR_MODIFY_PERSON_INFO_AND_IMAGE_INFO）
#define MODIFY_PERSON_INFO_AND_IMAGE_INFO			6
//对应的结构体（HY_DVR_BIND_AND_UNBIND_PERSON_INFO_LIST）
#define BIND_PERSON_INFO_LIST	    7
//对应的结构体（HY_DVR_BIND_AND_UNBIND_PERSON_INFO_LIST）
#define UNBIND_PERSON_INFO_LIST	    8
//查询满足条件的人员个数（HY_DVR_PERSON_INF_COUNT）
#define QUERY_PERSON_INFO_COUNT		9
//查询满足调的的人员信息（HY_DVR_PERSON_INFO_LIST）
#define QUERY_PERSON_INFO_LIST      10
//查询人员图片信息
#define QUERY_PERSON_IMAGE_INFO     11
//查询人员基本信息(HY_DVR_PERSON_INFO)
#define QUERY_PERSON_INFO_BY_PERSON_ID     12

//向中间层查询图片的数量
#define DEVICE_IMAGE_PERSON_MAP_NUM 13
//查询图片与对应人员的信息(HY_DVR_IMAGE_PERSION_MAP_V1)
#define IMAGE_PERSON_MAP_V1 14
//删除图片和对应的人员信息(HY_DVR_REMOVE_IMAGE_PERSON_MAP_V1)
#define REMOVE_IMAGE_PERSON_MAP_V1 15

//对应的结构体（HY_DVR_CHECK_PERSON_INFO_V1）
#define CHECK_PERSON_INFO_V1		16
//对应的结构体（HY_DVR_ADD_PERSON_INFO_AND_IMAGE_INFO_V1）
#define ADD_PERSON_INFO_AND_IMAGE_INFO_V1	17
//对应的结构体（HY_DVR_DELETE_PERSON_INFO_LIST_V1）
#define DELETE_PERSON_INFO_LIST_V1	    18
//对应的结构体（HY_DVR_BIND_AND_UNBIND_PERSON_INFO_LIST_V1）
#define UNBIND_PERSON_INFO_LIST_V1	    19
//对应的结构体（HY_DVR_BIND_AND_UNBIND_PERSON_INFO_LIST_V1）
#define BIND_PERSON_INFO_LIST_V1	    20
//对应的结构体（HY_DVR_MODIFY_PERSON_INFO_V1）
#define MODIFY_PERSON_INFO_V1			21
//对应的结构体（HY_DVR_UPDATE_IMAGE_INFO_V1）
#define UPDATE_IMAGE_INFO_V1			22
//对应的结构体（HY_DVR_MODIFY_PERSON_INFO_AND_IMAGE_INFO_V1）
#define MODIFY_PERSON_INFO_AND_IMAGE_INFO_V1			23
//查询人员图片信息（HY_DVR_IMAGE_INFO_PARAM_V1）
#define QUERY_PERSON_IMAGE_INFO_V1     24

//设置姓名位图点阵数据（HY_DVR_PRESON_NAME_BITMAP_DATA）
#define PERSON_NAME_BITMAP_DATA		25

//删除某个脸库的所有成员（HY_DVR_DELETE_SPECIFIC_GROUPID_ITEMS）
#define DELETE_SPECIFIC_GROUPID_ITEMS		26
//删除所有脸库的成员（HY_DELETE_ALL_GROUP_ITEMS）
#define DELETE_ALL_GROUP_ITEMS		27

//查询满足调的的人员信息（HY_DVR_PERSON_INFO_LIST_V1）
#define QUERY_PERSON_INFO_LIST_V1     28
//查询人员基本信息(HY_DVR_PERSON_INFO_V1)
#define QUERY_PERSON_INFO_BY_PERSON_ID_V1     29

//人员通行权限（HY_DVR_FACE_GROUP_PERSON_PASS_PERMISSION）
#define FACE_GROUP_PERSON_PASS_PERMISSION			 		30
//人员电梯配置（HY_DVR_PERSON_ELEVATOR_CONFIG）
#define PERSON_ELEVATOR_CONFIG		31

//对应的结构体（HY_DVR_ADD_PERSON_INFO_AND_IMAGE_INFO_V1）
#define ADD_PERSON_INFO_AND_IMAGE_INFO_V2	32

//查询满足条件的人员个数V1（HY_DVR_PERSON_INFO_COUNT_V1）
#define QUERY_PERSON_INFO_COUNT_V1	33

//对应的结构体（HY_DVR_ADD_PERSON_INFO_AND_IMAGE_INFO_V2），包含添加与更新操作
#define ADD_PERSON_AND_IMAGE_INFO_HEALTH_V2	34

//对应的结构体（HY_DVR_DEL_PERSON_INFO_V2）
#define DEL_PERSON_INFO_HEALTH_V2	    35

//对应的结构体（HY_DVR_SYNC_PERSON_PARAMETER），全量同步脸库开始
#define RESYNC_PERSON_BEGIN_HEALTH_V2	36

//对应的结构体（无），全量同步脸库结束
#define RESYNC_PERSON_END_HEALTH_V2		37

//对应的结构体（HY_DVR_SYNC_PERSON_PARAMETER），增量同步脸库开始
#define SYNC_PERSON_BEGIN_HEALTH_V2		38

//对应的结构体（无），增量同步脸库结束
#define SYNC_PERSON_END_HEALTH_V2		39

//对应的结构体（HY_DVR_ADD_PERSON_INFO_AND_IMAGE_INFO_V2），包含添加与更新操作（增量更新）
#define ADD_PERSON_AND_IMAGE_INFO_SYNC_HEALTH_V2	40

//人脸图片ID信息(HY_DVR_ONENET_PLATFORM_FACEID_INFO) 
#define HY_DVR_ONENET_PLATFORM_FACEID 41

//人脸图片信息(HY_DVR_ONENET_PLATFORM_FACE_INFO)
#define HY_DVR_ONENET_PLATFORM_FACE       42

//删除所有脸库的成员V1（HY_DELETE_ALL_GROUP_ITEMS_V1）
#define DELETE_ALL_GROUP_ITEMS_V1        43

///////////////////////////////////
//联动报警
//////////////////////////////////
//主命令
#define HY_DVR_GET_FACE_RECO_RULE_LIST  732
#define HY_DVR_SET_FACE_RECO_RULE_LIST  733
//次命令
////获取每个通道的人脸识别规则列表(HY_DVR_FACE_RECO_RULE_LIST)
#define FACE_RECO_RULE_LIST             1
////获取每个通道的人脸识别规则列表(HY_DVR_FACE_RECO_RULE_LIST_V1)
#define FACE_RECO_RULE_LIST_V1             2
////获取每个通道的人脸识别规则声音报警配置(HY_DVR_FACE_RECO_RULE_LIST_V1_VOICE_ALARM)
#define FACE_RECO_RULE_LIST_V1_VOICE_ALARM             3
//人脸识别事件时间段配置(HY_DVR_FACE_RECO_EVENT_TIMESLOT_LIST)
#define FACE_RECO_EVENT_TIMESLOT_CFG 4
//人脸识别事件语音报警时间段配置(HY_DVR_FACE_RECO_VOICEALARM_TIMESLOT_LIST)
#define FACE_RECO_EVENT_VOICEALARM_TIMESLOT_CFG 5


//人脸识别记录查询
//主命令
#define HY_DVR_GET_FACE_RECO_RECORD  734
#define HY_DVR_SET_FACE_RECO_RECORD  735
//次命令
//获取人脸识别记录个数（HY_DVR_FACE_RECO_RECORD_COUNT）
#define FACE_RECO_RECORD_COUNT       1
//获取人脸识别记录列表(HY_DVR_FACE_RECO_RECORD_LIST)
#define FACE_RECO_RECORD_LIST        2
//获取某一个人脸识别记录的详细信息（HY_DVR_FACE_RECO_RECORD_DETAIL）
#define FACE_RECO_RECORD_DETAIL      3
//获取某一个人脸识别记录的详细信息（HY_DVR_FACE_RECO_RECORD_SNAPSHOT）
#define FACE_RECO_RECORD_SNAPSHOT    4
//获取某一个人脸识别记录的详细信息（HY_DVR_FACE_RECO_RESULT_IMAGE）
#define FACE_RECO_RECORD_IMAGE       5
//获取某一个人脸识别记录的详细信息V1（HY_DVR_FACE_RECO_RECORD_DETAIL_V1）
#define FACE_RECO_RECORD_DETAIL_V1   6
//获取人脸识别记录列表(HY_DVR_FACE_RECO_RECORD_LIST_V1)
#define FACE_RECO_RECORD_LIST_V1     7
//获取人脸识别记录列表(HY_DVR_FACE_RECO_RECORD_LIST_V2)
#define FACE_RECO_RECORD_LIST_V2     8
//获取人脸识别记录列表(HY_DVR_FACE_DELETE_RECO_RECORD)
#define FACE_RECO_DELETE_RECORD     9
//人脸识别距离(HY_DVR_FACE_RECO_DISTANCE)
#define FACE_RECO_DISTANCE	10


//人脸识别能力
//主命令
#define HY_DVR_GET_FACE_RECO_ABILITY  736
#define HY_DVR_SET_FACE_RECO_ABILITY  737
//次命令
//获取设备人脸识别相关的所有能力（HY_DVR_FACE_RECO_ABILITY）
#define FACE_RECO_ABILITY_ALL		(0xFFFFFFFF)

//云存储
//主命令
#define HY_DVR_GET_CLOUD_STORAGE		738
#define HY_DVR_SET_CLOUD_STORAGE		739

//次命令
//设备ID
#define CLOUD_STORAGE_INFO		1

//云存储开启状态
#define CLOUD_STORAGE_BASE_CONFIG 2

//云存储开通状态
#define CLOUD_STORAGE_OPEN_STATUS 3

//云存储录像计划
#define CLOUD_STORAGE_RECORD_PLAN 4

//云存储运行状态
#define CLOUD_STORAGE_BASE_STATUS 5

//云存储订单信息
#define CLOUD_STORAGE_ORDER_INFO 6

//云存储套餐信息
#define CLOUD_STORAGE_PACKAGE_INFO 7

//云存储订单和套餐信息
#define CLOUD_STORAGE_ORDER_PACKAGE_INFO 8

//获取云存错误码列表（目前是协议内相互调用时使用的，不经过中间层）tagHY_DVR_OBS_ERROR_CODE_LIST
#define CLOUD_STORAGE_ERROR_CODE_LIST 9

//云存储录像计划布防时间计划 (HY_DVR_CLOUD_STROAGE_RECORD_PLAN_EVENT_TIMESLOT_LIST)
#define CLOUD_STORAGE_RECORD_PLAN_EVENT_TIMESLOT_CFG 10

//多通道云存储录像计划
#define CLOUD_STORAGE_RECORD_PLAN_V2 11

//多通道云存储状态(HY_DVR_CLOUD_STORAGE_BASE_STATUS_V2)
#define CLOUD_STORAGE_BASE_STATUS_V2 12

//云存储auth信息(tagHY_DVR_CLOUD_STORAGE_AUTH_INFO)
#define CLOUD_STORAGE_AUTH_INFO     13

//云存储配置(tagHY_DVR_CLOUD_STORAGE_LOW_POWER_CONFIG),这个是云存配置，并不是低功耗专属，之前名字起的有问题
#define CLOUD_STORAGE_LOW_POWER_CONFIG     14

//云存储执行状态(tagHY_DVR_CLOUD_STORAGE_EXECUTION_STATE)
#define CLOUD_STORAGE_EXECUTION_STATE    15

//获取云存错误码列表V1（目前是协议内相互调用时使用的，不经过中间层）tagHY_DVR_OBS_ERROR_CODE_LIST
#define CLOUD_STORAGE_ERROR_CODE_LIST_V1 16

/*****************************************************/
//报警动作
/*****************************************************/
//主命令
#define HY_DVR_SET_ALARM_ACTION  740

//次命令
#define ALARM_ACTION_LIGHT_WARNING  1




///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取云台状态															 */
#define HY_DVR_GET_PTZSTATUS 742

#define HY_DVR_SET_PTZSTATUS 743

//次命令
#define PTZ_STATUS 1

//PTZ自动巡航状态
#define PTZ_CURISE_STATUS 2


//日志信息(HY_DVR_LOG_INFO)
#define CLOUD_STORAGE_LOG		9



///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 消息通知															 */
#define HY_DVR_GET_MESSAGE_NOTIFY		 744

#define HY_DVR_SET_MESSAGE_NOTIFY		 745

//次命令（HY_DVR_MESSAGE_NOTIFY）
#define MESSAGE_NOTIFY_ALL				(0xFFFFFFFF)

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
#define HY_DVR_GET_FACE_LIVENESS			746
#define HY_DVR_SET_FACE_LIVENESS			747

//次命令
#define FACE_LIVENESS_STATUS		1

///////////////////////////////////////////////////////////////////////////////////////////////////////
/** @brief 系统性能状态											*/

//主命令
/** @brief 获取设备系统性能状态										*/
#define HY_DVR_GET_SYSTEM_PERFORMANCE_STATUS		748
/** @brief 设置设备系统性能状态										*/
#define HY_DVR_SET_SYSTEM_PERFORMANCE_STATUS		749

//次命令
//所有配置信息(HY_DVR_SYSTEM_PERFORMANCE_STATUS)
#define CONFIG_ALL									0xFFFFFFFF
///////////////////////////////////////////////////////////////////////////////////////////////////////

//门禁
//主命令
/** @brief 获取门禁信息										*/
#define HY_DVR_GET_ACCESS_GATE_INFO 750
/** @brief 设置门禁信息										*/
#define HY_DVR_SET_ACCESS_GATE_INFO 751

#define DVR_MAX_ACCESS_CARD_NUM 1000
#define DVR_MAX_ACCESS_PASSWORD_NUM 1000

//次命令
//添加卡
#define ADD_ACCESS_GATE_CARD 1

//删除卡
#define DELETE_ACCESS_GATE_CARD 2

//修改卡
#define MODIFY_ACCESS_GATE_CARD 3

//查询所有序列号
#define QUERY_ACCESS_GATE_CARD 4

//添加卡
#define ADD_ACCESS_GATE_PASSWORD 5

//添加
#define DELETE_ACCESS_GATE_PASSWORD 6

//修改卡
#define MODIFY_ACCESS_GATE_PASSWORD 7

//查询所有密码
#define QUERY_ACCESS_GATE_PASSWORD 8

//通过ID查询卡号
#define QUERY_ACCESS_GATE_QUERY_CARDNUMBER_BY_ID 9

//通过ID查询卡号
#define QUERY_ACCESS_GATE_QUERY_PASSWORD_BY_ID 10

//获取当前刷卡器上卡的卡号
#define QUERY_CURRENT_CARD_NUMBER 11

//读卡模式
#define CARD_READ_MODE 12

//进出方式
#define ACCESS_GATE_DIRECTION 13

//增加NFC卡信息，适配济工网平台用
#define ADD_ACCESS_GATE_NFC_CARD 14
//删除NFC卡信息，适配济工网平台用
#define DELETE_ACCESS_GATE_NFC_CARD 15
//清空NFC卡所有数据，适配济工网平台用
#define CLEAR_ALL_ACCESS_GATE_NFC_CARD 16


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取口罩检测配置										*/
#define HY_DVR_GET_FACE_DETECT_MASK_CONFIG 752
/** @brief 设置口罩检测配置										*/
#define HY_DVR_SET_FACE_DETECT_MASK_CONFIG 753

//设置口罩报警情况，有口罩报警/无口罩报警/不报警(HY_DVR_MASK_DETECT_ALARM_CONFIG)
#define FACE_MASK_ALARM_CONFIG 1

//口罩联动报警(HY_DVR_MASK_DETECT_AlARM_LINKAGE)
#define FACE_MASK_ALARM_LINKAGE 2

//口罩禁入配置(HY_DVR_MASK_PASS_CONFIG)
#define FACE_MASK_PASS_CONFIG		3

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取人脸温度检测配置										*/
#define HY_DVR_GET_FACE_TEMPERATURE_DETECT_CONFIG 754
/** @brief 设置人脸温度检测配置										*/
#define HY_DVR_SET_FACE_TEMPERATURE_DETECT_CONFIG 755

//人脸温度检测配置（HY_DVR_FACE_TEMPERATURE_DETECT_CONFIG）
#define FACE_TEMPERATURE_DETECT_CONFIG 1

//人脸温度叠加OSD设置（HY_DVR_OVERLAY_FACE_TEMPERATRUE_ON_OSD）
#define FACE_TEMPERATURE_DETECT_OSD_OVERLAY_CONFIG 2

//当前人脸温度（HY_DVR_FACE_TEMPERATURE_STATUS）
#define CURRENT_FACE_TEMPREATURE	3

//人脸温度不正常报警配置（HY_FACE_TEMPERATURE_ABNORMAL_ALARM_CONFIG）
#define FACE_TEMPERATURE_ABNORMAL_ALARM_CONFIG	4

//人脸温度探测校正（HY_DVR_DETECT_REVISE）
#define FACE_TEMPERATURE_DETECT_REVISE	5

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取安全配置												*/
#define HY_DVR_GET_SECURITY_CONFIG 					756
/** @brief 设置安全配置												*/
#define HY_DVR_SET_SECURITY_CONFIG 					757

//次命令
//白名单配置（HY_DVR_WHITE_LISTS）
#define SECURITY_WHITE_LISTS 						1

//白名单规则（HY_DVR_WHITE_RULE）
#define SECURITY_WHITE_RULE		 					2

//增加白名单规则（HY_DVR_WHITE_RULE）
#define SECURITY_WHITE_RULE_ADD 					3

//增加白名单规则（HY_DVR_WHITE_RULE）
#define SECURITY_WHITE_RULE_DEL 					4

//清空白名单规则
#define SECURITY_WHITE_RULE_CLEAR					5

//验证IP是否允许访问（HY_DVR_SECURITY_VALIDATE_IP）
#define SECURITY_VALIDATE_IP		6

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取MQTT服务器信息 */
#define HY_DVR_GET_MQTT_SERVER_CONFIG				758
/** @brief 设置MQTT服务器信息 */
#define HY_DVR_SET_MQTT_SERVER_CONFIG				759

//次命令 
/** @brief  对于结构体HY_DVR_MQTT_SERVER_CONFIG */
#define MQTT_SERVER_ALL								0xFFFFFFFF

//主类型
/** @brief 获取HTTP事件服务器信息 */
#define HY_DVR_GET_HTTP_EVENT_SERVER_CONFIG				760
/** @brief 设置HTTP事件服务器信息 */
#define HY_DVR_SET_HTTP_EVENT_SERVER_CONFIG				761

//次命令 
/** @brief  对于结构体HY_DVR_HTTP_EVENT_SERVER_CFG */
#define HTTP_EVENT_SERVER_ALL								0xFFFFFFFF

/** @brief  对于结构体HY_DVR_HTTP_EVENT_SERVER_CFG_V1 */
#define HTTP_EVENT_SERVER_ALL_V1								1
/** @brief  对于结构体HY_DVR_HTTP_EVENT_SERVER_STATUS */
#define HTTP_EVENT_SERVER_STATUS								2
/** @brief  HTTP事件服务器信息V2(HY_DVR_HTTP_EVENT_SERVER_CFG_V2)  */
#define HTTP_EVENT_SERVER_CFG_V2                        		3
/** @brief  HTTP事件服务器状态V2(HY_DVR_HTTP_EVENT_SERVER_STATUS_V2) */
#define HTTP_EVENT_SERVER_STATUS_V2                     		4
/** @brief  HTTP事件服务器状态测试(HY_DVR_HTTP_EVENT_SERVER_STATUS_TEST) */
#define HTTP_EVENT_SERVER_STATUS_TEST                   		5


///////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取历史流录像信息 */
#define HY_DVR_GET_RECORD_INFO				762
/** @brief 设置历史流录像信息 */
#define HY_DVR_SET_RECORD_INFO				763

//次命令
#define RECORD_CURRENT_RECORD_START_TIME	1


//主类型
/** @brief 获取隐私设置 */
#define HY_DVR_GET_PRIVACY_SETTING				764
/** @brief 设置隐私设置*/
#define HY_DVR_SET_PRIVACY_SETTING				765

//次命令(HY_DVR_PRIVACY_SETTING)
#define PRIVACY_SETTING							1


////////微信考勤机与中间层交互
//主类型
/** @brief 微信网络初始化配置	*/
#define HY_DVR_WX_CFG_INIT	766
//次类型
/** @brief 初台化时获取设备的固化SECRET NO	*/  //HY_DVR_WX_SEED_SECRET_NO
#define WX_INIT_SEED_SECRET_NO			1

/** @brief 写入或读取中间层的SECRET NO	*/  //HY_DVR_WX_SECRET_NO
#define WX_INIT_SECRET_NO			2

/** @brief 获取或写入密钥	*/  //HY_DVR_WX_SECRET
#define WX_INIT_SECRET			3

/** @brief 中间层找协议获取动态二维码	*/  //HY_DVR_WX_QR_CODE
#define WX_INIT_QR_CODE         4

/** @brief 找中间层要发送和接收的最大buff长度	*/ //HY_DVR_WX_SEND_RECV_BUFF_LENGTH
#define WX_INIT_SEND_RECV_BUFF_LENGTH     5

/** @brief 协议给中间层更新服务器连接状态	*/    //HY_DVR_WX_CONNECT_SERVER
#define WX_INIT_CONNECT_SERVER_STATUS     6

/** @brief 协议找中间层获取基础配置	*/    //HY_DVR_WX_DEVICE_INFO
#define WX_INIT_DEVICE_INFO        7

//主类型
/** @brief 脸库全量拉取	*/
#define HY_DVR_WX_GET_USERINFO_BY_FULLLOAD		767

//次类型
/** @brief 写入全量拉取脸库总人数	*/   //HY_DVR_WX_TOTAL_MEMBER
#define WX_FULLLOAD_USERINFO_MEMBER			1

/** @brief 写入或读取脸库版本号	*/   //HY_DVR_WX_FIRM_VERSION
#define WX_FULLLOAD_FIRM_VERSION		2

/** @brief 全量拉取时数据同步	*/  //HY_DVR_WX_ONE_USER_INFO
#define WX_FULLLOAD_USER_INFO_UPDATE			3

//主类型
/** @brief 脸库增量拉取	*/
#define HY_DVR_WX_GET_USERINFO_BY_IDS			768

//次类型
/** @brief 增量同步  添加人员	*/  //HY_DVR_WX_ONE_USER_INFO
#define WX_IDS_USERINFO_ADD			1

/** @brief 增量同步 删除人员	*/  //HY_DVR_WX_DELETE_PERSON_INFO
#define WX_IDS_USERINFO_DELETE			2

/** @brief 增量同步 更新人员	*/  //HY_DVR_WX_ONE_USER_INFO
#define WX_IDS_USERINFO_MODIFY			3

/** @brief 增量同步  批量添加人员	*/  //HY_DVR_WX_ONE_USER_INFO
#define WX_IDS_BULK_USERINFO_ADD			   4

/** @brief 增量同步 批量更新人员	*/  //HY_DVR_WX_ONE_USER_INFO
#define WX_IDS_BULK_USERINFO_MODIFY     5

/** @brief 写入或读取脸库版本号(增量)	*/   //HY_DVR_WX_FIRM_VERSION
#define WX_IDS_FIRM_VERSION             6

//主类型
/** @brief 打卡事件	*/
#define HY_DVR_WX_CHECK_IN			769

//次类型
/** @brief 单次打卡  网络传输情况反馈 */   //HY_DVR_WX_CHECK_IN_RESP
#define WX_CHECK_IN_RESP			1

/** @brief 打卡列表 网络传输情况反馈 */    //HY_DVR_WX_CHECK_IN_RESP_LIST
#define WX_CHECK_IN_LIST_RESP			2

/** @brief 打卡列表获取*/      //HY_DVR_WX_CHECK_IN_LIST
#define WX_CHECK_IN_GET_LIST			3

//主类型
/** @brief 人脸录入	*/
#define HY_DVR_WX_FCAE_RECORD			770

//次类型
/** @brief 下载图片 */   //HY_DVR_WX_RECORD_PIC
#define WX_FCAE_RECORD_DOWNLOAD_PICTURE			1

/** @brief 获取特征值 微信录入*/  //HY_DVR_WX_FACE_GROUP
#define WX_FCAE_RECORD_FEATURE			2

/** @brief 获取特征值 设备录入*/  //HY_DVR_WX_FACE_GROUP
#define WX_DEVICE_RECORD_FEATURE			3

/** @brief  录入信息*/  //HY_DVR_WX_RECORD_USERINFO
#define WX_FCAE_RECORD_START			4

/** @brief  取消录入*/  //HY_DVR_WX_CANCEL_RECORD
#define WX_FCAE_RECORD_CANCEL			5

/** @brief  录入结束*/  //HY_DVR_WX_RECORD_USERINFO
#define WX_FCAE_RECORD_END			6

/** @brief  设备录入失败*/  //HY_DVR_WX_FAILED_RECORD
#define WX_FCAE_RECORD_FAILED			7


//主类型
/** @brief UserID变更事件	*/
#define HY_DVR_WX_USER_ID_CHANGE			771

//次类型   
//HY_DVR_WX_CHANGE_USERID
#define WX_USER_ID_CHANGE			1


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
#define HY_DVR_GET_WIEGAND_SETTINGS		772
#define HY_DVR_SET_WIEGAND_SETTINGS		773

//韦根基本设置(WIEGAND_BASIC_SETTINGS)
#define WIEGAND_BASIC_SETTINGS		1
//韦根测试(HY_DVR_WIEGAND_TEST)
#define WIEGAND_TEST	2

//主类型
/** @brief 获取人脸识别事件模式*/
#define HY_DVR_GET_FACE_RECO_MODE				774
/** @brief 设置人脸识别事件模式*/
#define HY_DVR_SET_FACE_RECO_MODE				775

//次命令(HY_DVR_FACE_RECO_EVENT_MODE)
#define FACE_RECO_MODE_CFG						1


//主类型
/** @brief 通知发布状态*/
#define HY_DVR_GET_NOTIFY_PUBLISH_STATUS				776
/** @brief 通知发布状态*/
#define HY_DVR_SET_NOTIFY_PUBLISH_STATUS				777

//次命令(HY_DVR_NOTIFY_PUBLISH_STATUS)
#define NOTIFY_PUBLISH_STATUS						1
//次命令(HY_DVR_MEGLINK_NOTIFY_EVENT_UPLOAD_STATUS)
#define NOTIFY_MEGLINK_EVENT_UPLOAD_STATUS			2
//次命令(HY_DVR_NOTIFY_PUBLISH_STATUS_V1)
#define NOTIFY_PUBLISH_STATUS_V1					3


//主类型
/** @brief 获取微信基本信息*/
#define HY_DVR_GET_WXBASIC_STATUS				778
/** @brief 设置微信基本信息*/
#define HY_DVR_SET_WXBASIC_STATUS				779

//次命令
//基本状态（导入状态，激活状态之类的）(HY_DVR_WECHAT_IMPORT_STATUS)
#define BASIC_STATUS						1
//ModelID（产测ModelID）（HY_DVR_WECHAT_BASIC_INFO）
#define BASIC_INFO			2


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令	（天翼）
/** @brief 获取音频配置信息 */
#define HY_DVR_GET_AUDIO_CONFIG				780
/** @brief 设置音频配置信息 */
#define HY_DVR_SET_AUDIO_CONFIG				781

//次命令
/** @brief 获取音频编码信息 (HY_DVR_AUDIO_ENCODER_INFO)*/
#define AUDIO_ENCODER_CONFIG		1

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令	(天翼)
/** @brief 获取升级信息 */
#define HY_DVR_GET_TIANYI_UPDATE_INFO		782
/** @brief 获取升级信息 */
#define HY_DVR_SET_TIANYI_UPDATE_INFO		783

//次命令
/** @brief 升级信息 (HY_DVR_TIANYI_UPDATE_INFO)*/
#define UPDATE_INFO			1

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令	(天翼)
/** @brief 获取录像信息 */
#define HY_DVR_GET_TIANYI_STORAGE_INFO		784
/** @brief 获取录像信息 */
#define HY_DVR_SET_TIANYI_STORAGE_INFO		785

//次命令
/** @brief 升级信息 (HY_DVR_TIANYI_STORAGE_INFO)*/
#define STORAGE_INFO			1

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取录像信息 */
#define HY_DVR_GET_TIANYI_P2P_INFO				786
/** @brief 设置录像信息 */
#define HY_DVR_SET_TIANYI_P2P_INFO				787

//次命令
#define P2P_INFO	1

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取I帧加密信息 */
#define HY_DVR_GET_TIANYI_ENCKEY_INFO					788
/** @brief 设置I帧加密信息 */
#define HY_DVR_SET_TIANYI_ENCKEY_INFO					789

//次命令
#define ENCKEY_INFO		1

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取设备扩展信息 */
#define HY_DVR_GET_TIANYI_DEVICE_INFO_EX				790
/** @brief 设置设备扩展信息 */
#define HY_DVR_SET_TIANYI_DEVICE_INFO_EX				791

//次命令
//设备信息
#define DEVICE_INFO_EX		1
//报警回调
#define ALARM_INTERFACE		2
//码流套餐信息
#define STREAM_PACKAGE_INFO		3
//高清云存码类型信息
#define CLOUD_STREAM_TYPE_INFO  4
//授权配置信息(HY_DVR_TIANYI_AUTH_INFO)
#define AUTH_INFO		5
//高清云存封面类型信息（HY_DVR_TIANYI_CLOUD_ICON_TYPE_INFO）
#define CLOUD_ICON_TYPE_INFO  6

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取设备能力（天翼） */
#define HY_DVR_GET_TIANYI_DEVICE_CAP			792
/** @brief 获取设备能力（天翼） */
#define HY_DVR_SET_TIANYI_DEVICE_CAP			818
//次命令
#define TIANYI_DEVICE_CAP			1

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取远程 */
#define HY_DVR_GET_MQTT_REMOTE_OPEN_DOOR				793
/** @brief 设置设备扩展信息 */
#define HY_DVR_SET_MQTT_REMOTE_OPEN_DOOR				794

//次命令(HY_DVR_MQTT_REMOTE_OPEN_DOOR)
#define OPEN_DOOR_CONTROL_ID		1
//次命令(HY_DVR_MQTT_REMOTE_OPEN_DOOR)(MQTT是先调用配置接口设置控制码，再调用控制接口开门，如果CGI也这样搞，那两个协议同时开门时就会有问题，所以CGI开门新增一个命令字，设备端接收到此命令字就开门，结构体中会带上控制码)
#define OPEN_DOOR_CONTROL_ID_CGI		2

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取DEVICE_TOKEN（旷视九霄） */
#define HY_DVR_GET_MEGLINK_DEVICE_INFO			795
/** @brief 设置DEVICE_TOKEN（旷视九霄） */
#define HY_DVR_SET_MEGLINK_DEVICE_INFO				796
//次命令
#define MEGLINK_DEVICE_INFO		1
//MEGLINK错误信息(HY_DVR_MEGLINK_ERROR_INFO)
#define MEGLINK_ERROR_INFO		2

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取DEVICE_CONFIG（旷视九霄） */
#define HY_DVR_GET_MEGLINK_DEVICE_CONFIG			797
/** @brief 设置DEVICE_CONFIG（旷视九霄） */
#define HY_DVR_SET_MEGLINK_DEVICE_CONFIG			798
//次命令
/** @brief 设置GATEWAY_CONFIG（HY_DVR_MEGLINK_GATEWAY_CONFIG） */
#define MEGLINK_GATEWAY_CONFIG		1
/** @brief 设置PASS_CONFIG（HY_DVR_MEGLINK_PASS_CONFIG） */
#define MEGLINK_PASS_CONFIG		2
/** @brief 设置PERSTY_CONFIG（HY_DVR_MEGLINK_PERSTY_CONFIG） */
#define MEGLINK_PERSTY_CONFIG		3
/** @brief 设置SYS_CONFIG（HY_DVR_MEGLINK_SYS_CONFIG） */
#define MEGLINK_SYS_CONFIG		4
/** @brief 设置ALERT_CONFIG（HY_DVR_MEGLINK_ALERT_CONFIG） */
#define MEGLINK_ALERT_CONFIG		5
/** @brief 设置STRANGER_CONFIG（HY_DVR_MEGLINK_STRANGER_CONFIG） */
#define MEGLINK_STRANGER_CONFIG		6
/** @brief 设置TERPERATURE_CONFIG（HY_DVR_MEGLINK_TEMPERATURE_CONFIG） */
#define MEGLINK_TEMPERATURE_CONFIG		7
/** @brief 设置TIMESTAMP（HY_DVR_MEGLINK_CONFIG_UPDATE_TIMESTAMP） */
#define MEGLINK_TIMESTAMP_CONFIG		8
/** @brief 设备解绑（HY_DVR_MEGLINK_UNBIND） */
#define MEGLINK_UNBIND		9
/** @brief 2.0额外信息（HY_DVR_MEGLINK_CONFIG_EXT） */
#define MEGLINK_2DOT0_EXT		10

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取脸库配置（旷视九霄） */
#define HY_DVR_GET_MEGLINK_PERSON_PERSONGROUP_SYNC_CONFIG			801
/** @brief 设置脸库配置（旷视九霄） */
#define HY_DVR_SET_MEGLINK_PERSON_PERSONGROUP_SYNC_CONFIG			802
//次命令
/** @brief 添加人员（HY_DVR_MEGLINK_PEOPLE_PEOPLEGROUP_PERSON_INFO） */
#define MEGLINK_PERSON_PERSONGROUP_ADD_PERSON		1
/** @brief 更新人员（HY_DVR_MEGLINK_PEOPLE_PEOPLEGROUP_PERSON_INFO） */
#define MEGLINK_PERSON_PERSONGROUP_UPDATE_PERSON		2
/** @brief 删除人员（当某个人员不在属于任何组的时候，）（HY_DVR_MEGLINK_PEOPLE_PEOPLEGROUP_DELETE_PERSON_INFO） */
#define MEGLINK_PERSON_PERSONGROUP_DELETE_PERSON		3
/** @brief 清空脸库（全量更新，删除所有组，删除所有人员）（HY_DVR_MEGLINK_PEOPLE_PEOPLEGROUP_CLEAR_INFO） */
#define MEGLINK_PERSON_PERSONGROUP_CLEAR		4
/** @brief 查询人员所属的所有脸库数量（HY_DVR_MEGLINK_QUERY_PEOPLE_BELONG_TO_GROUP_NUM） */
#define MEGLINK_PERSON_PERSONGROUP_QUERY_PEOPLE_BELONG_TO_GROUP_NUM		5
/** @brief 查询人员所属的所有脸库（HY_DVR_MEGLINK_QUERY_PEOPLE_BELONG_TO_GROUP） */
#define MEGLINK_PERSON_PERSONGROUP_QUERY_PEOPLE_BELONG_TO_GROUP		6
/** @brief 同步版本（HY_DVR_MEGLINK_PERSON_PERSONGROUP_SYNC_INFO） */
#define MEGLINK_PERSON_PERSONGROUP_SYNC_VERSION_CONFIG		7

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取人员组和时间计划配置（旷视九霄） */
#define HY_DVR_GET_MEGLINK_PEOPLEGROUP_TIMESCHEDULE_CONFIG			803
/** @brief 设置人员组和时间计划配置（旷视九霄） */
#define HY_DVR_SET_MEGLINK_PEOPLEGROUP_TIMESCHEDULE_CONFIG			804
//次命令
/** @brief 添加人员组配置（HY_DVR_MEGLINK_PEOPLE_GROUP_CONFIG） */
#define MEGLINK_ADD_PEOPLE_GROUP_CONFIG		1
/** @brief 删除人员组配置（HY_DVR_MEGLINK_CLEAR_PEOPLE_GROUP_CONFIG） */
#define MEGLINK_DELETE_PEOPLE_GROUP_CONFIG		2
/** @brief 添加时间计划配置（HY_DVR_MEGLINK_TIME_SCHEDULE_CONFIG） */
#define MEGLINK_ADD_TIME_SCHEDULE_CONFIG		3
/** @brief 删除时间计划配置（HY_DVR_MEGLINK_CLEAR_TIME_SCHEDULE_CONFIG） */
#define MEGLINK_DELETE_TIME_SCHEDULE_CONFIG		4
/** @brief 添加组和时间计划绑定关系（HY_DVR_MEGLINK_GROUP_SCHEDULE_BIND_RELATION） */
#define MEGLINK_ADD_PEOPLEGROUP_TIMESCHEDULE_RELATION		5
/** @brief 删除组和时间计划绑定关系（HY_DVR_MEGLINK_CLEAR_GROUP_SCHEDULE_BIND_RELATION） */
#define MEGLINK_DELETE_PEOPLEGROUP_TIMESCHEDULE_RELATION		6
/** @brief 同步时间（HY_DVR_MEGLINK_PEOPLEGROUP_TIMESCHEDULE_UPDATETIME） */
#define MEGLINK_PERSONGROUP_TIMESCHEDULE_UPDATETIME		7
/** @brief 查询时间计划配置（HY_DVR_MEGLINK_TIME_SCHEDULE_CONFIG） */
#define MEGLINK_QUERY_TIME_SCHEDULE_CONFIG		8
/** @brief 查询脸库时间表绑定关系数量（HY_DVR_MEGLINK_QUERY_GROUP_SCHEDULE_RELATION_NUM） */
#define MEGLINK_QUERY_GROUP_SCHEDULE_NUM		9
/** @brief 根据Index查询脸库时间表绑定关系（HY_DVR_MEGLINK_QUERY_GROUP_SCHEDULE_RELATION） */
#define MEGLINK_QUERY_GROUP_SCHEDULE		10
/** @brief 查询脸库数量（HY_DVR_MEGLINK_QUERY_PEOPLE_GROUP_NUM） */
#define MEGLINK_QUERY_PEOPLE_GROUP_NUM		11
/** @brief 根据脸库Index查询脸库（HY_DVR_MEGLINK_QUERY_PEOPLE_GROUP） */
#define MEGLINK_QUERY_PEOPLE_GROUP		12
/** @brief 获取时间计划数量（HY_DVR_MEGLINK_QUERY_TIMESCHEDULE_NUM） */
#define MEGLINK_QUERY_TIMESCHEDULE_NUM		13
/** @brief 根据index获取时间计划信息（HY_DVR_MEGLINK_QUERY_TIMESCHEDULE） */
#define MEGLINK_QUERY_TIMESCHEDULE		14




///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取识别记录（旷视九霄） */
#define HY_DVR_GET_MEGLINK_RECOGNIZE_RECORD			805
/** @brief 设置识别记录（旷视九霄） */
#define HY_DVR_SET_MEGLINK_RECOGNIZE_RECORD				806
//次命令
/** @brief 识别记录数量（HY_DVR_MEGLINK_RECOGNIZE_RECORD_NUM） */
#define MEGLINK_RECOGNIZE_RECORD_NUM	1
/** @brief 识别记录（HY_DVR_MEGLINK_RECOGNIZE_RECORD_NODE） */
#define MEGLINK_RECOGNIZE_RECORD				2

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取OTA升级信息（旷视九霄） */
#define HY_DVR_GET_MEGLINK_OTA_UPDATE			807
/** @brief 设置OTA升级信息（旷视九霄） */
#define HY_DVR_SET_MEGLINK_OTA_UPDATE			808
//次命令
/** @brief 识别记录数量（HY_DVR_MEGLINK_OTA_UPDATE） */
#define MEGLINK_OTA_UPDATE		1

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取OTA升级信息（旷视九霄） */
#define HY_DVR_GET_MEGLINK_OPEN_DOOR			809
/** @brief 设置OTA升级信息（旷视九霄） */
#define HY_DVR_SET_MEGLINK_OPEN_DOOR			810
//次命令
/** @brief 识别记录数量（HY_DVR_MEGLINK_OPEN_DOOR） */
#define MEGLINK_OPEN_DOOR		1

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取第三方协议配置信息 */
#define HY_DVR_GET_THIRD_PARTY_PROTOCOL_CONFIG			811
/** @brief 设置第三方协议配置信息 */
#define HY_DVR_SET_THIRD_PARTY_PROTOCOL_CONFIG			812
//次命令
/** @brief 第三方协议配置 (HY_DVR_THIRD_PROTOCOL_MANAGE)*/
#define THIRD_PARTY_PROTOCOL_CONFIG		1

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取设备登录信息（旷视九霄） */
#define HY_DVR_GET_MEGLINK_HTTP_REQUEST_INFO			813
/** @brief 设置设备登录信息（旷视九霄） */
#define HY_DVR_SET_MEGLINK_HTTP_REQUEST_INFO			814
//次命令	(HY_DVR_MEGLINK_HTTP_REQUEST_INFO)
#define MEGLINK_HTTP_REQUEST_INFO		1

////////////////////////////////////////////////////////////////////////////////////////////////
//FlowClient
/** @brief 设备信息（金睿）主设备号 */
#define HY_DVR_FL_POST_INIT_PARAM			815

/** @brief 设备信息（金睿）appid */
#define FL_INIT_POST_APPID			1

/** @brief 设备信息（金睿）appkey */
#define FL_INIT_POST_APPKEY			2

/** @brief 设备网络信息（金睿） */
#define FL_NETWORK_INFO     3

/** @brief 设备网络信息（北纬） */
#define FL_INIT_BW_CARDTYPE     4

/** @brief 设备信息（北纬）appid */
#define FL_INIT_BW_APPID			5

/** @brief 设备信息（北纬）secretkey */
#define FL_INIT_BW_SECRETKEY			6

/** @brief 设备信息（金睿）畅享智能 URL */
#define FL_INIT_CXZN_URL			7

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取GB28181配置信息（旷视九霄） */
#define HY_DVR_GET_GB28181_BASIC_INFO		816
/** @brief 设置GB28181配置信息（旷视九霄） */
#define HY_DVR_SET_GB28181_BASIC_INFO		817
//次命令 (HY_DVR_GB28181_CFG)
#define GB28181_BASIC_INFO		1
//次命令 (HY_DVR_GB28181_REGISTER_STATE)
#define GB28181_BASIC_REGISTER_STATE		2
/////////////////////////////////////////////////////////////////////////////////////////////////
//注意下一个主命令从819开始，818被设置天翼能力占用


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取声光报警配置 */
#define HY_DVR_GET_SOUND_LIGHT_ALARM_CFG		818
/** @brief 设置声光报警配置 */
#define HY_DVR_SET_SOUND_LIGHT_ALARM_CFG		819
//次命令 (HY_DVR_SOUND_LIGHT_ALARM_START)
#define SOUND_LIGHT_ALARM_START					1
//次命令 (HY_DVR_SOUND_LIGHT_ALARM_STOP)
#define SOUND_LIGHT_ALARM_STOP					2
//
#define SOUND_LIGHT_ALARM_PLAY					3


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取声光报警配置 */
#define HY_DVR_GET_DEVICE_ON_CMDOT_SERVER_STATUS		820
/** @brief 设置声光报警配置 */
#define HY_DVR_SET_DEVICE_ON_CMDOT_SERVER_STATUS		821
//次命令 (HY_DVR_CMDOT_DEVICE_STATUS)
#define CMDOT_DEVICE_STATUS					1

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取中国移动DOT平台相关配置 */
#define HY_DVR_GET_CMDOT_CFG		822
/** @brief 设置中国移动DOT平台相关配置 */
#define HY_DVR_SET_CMDOT_CFG		823
//次命令 (HY_DVR_CMDOT_PATH)
#define CMDOT_PATH					1	
//HY_DVR_CMDOT_DEVICE_INFO
#define CMDOT_DEVICE_INFO			2
//HY_DVR_CMDOT_DEVICE_INFO_EX
#define CMDOT_DEVICE_INFO_EX		3
//HY_DVR_CMDOT_STORAGE_INFO
#define CMDOT_STORAGE_INFO			4
//HY_DVR_CMDOT_MOTION_V3_LIST
#define CMDOT_MOTION_V3_LIST		5
//恢复出厂，NULL
#define CMDOT_RESET_FACTORY			6
//设置音量，HY_DVR_CMDOT_SPEAKER_INFO
#define CMDOT_SPEAKER_INFO			7
//绑定状态，HY_DVR_CMDOT_BIND_STATUS
#define CMDOT_BIND_STATUS			8
//授权信息，HY_DVR_CMDOT_AUTH_INFO
#define CMDOT_AUTH_INFO				9
//LOGO水印显示
#define CMDOT_OSD_LOGO				10
//文字水印显示
#define CMDOT_OSD_TEXT_INFO			11
//设置MtuSize
#define CMDOT_MTU_INFO				12
//设置告警入侵区域
#define CMDOT_HACK_REGION			13
//设置告警入侵使能
#define CMDOT_HACK_STATUS			14
//人脸检测基本配置项
#define CMDOT_FACE_DETECT_BASIC_INFO	15
//人脸检测区域
#define CMDOT_FACE_DETECT_REGION		16
//人脸检测其他参数
#define CMDOT_FACE_DETECT_PARAM			17
//视频分辨率
#define CMDOT_VIDEO_RESOLUTION_LEVEL	18
//喇叭处理优先级
#define CMDOT_SPEAKER_STATUS_PRIORITY	19
//设置告警入侵灵敏度
#define CMDOT_HACK_SENSITIVITY			20
//告警音量
#define CMDOT_ALARM_VOLUME_INFO			21
//声音侦测开关
#define CMDOT_SOUND_DETECT_STATUS		22
//声音侦测灵敏度
#define CMDOT_SOUND_DETECT_SENSITIVITY	23
//人脸捕获模式
#define CMDOT_FACE_CAPTURE_MODE			24
//人脸捕获质量
#define CMDOT_FACE_CAPTURE_QUALITY		25
//人形侦测框
#define CMDOT_PEOPLE_DRAW_BOX			26
//清空SpeakerBuffer
#define CMDOT_SPEKAR_BUFFER_CLEAR		27
//中间层音频播放结束通知
#define CMDOT_AUDIO_PLAYOUT_OVER		28
//设置码流格式
#define CMDOT_VIDEO_ENCODE				29
//获取负载状态
#define CMDOT_GET_DEVICE_LOAD			30
//梯控报警使能
#define CMDOT_EBIKE_DETECT_ENABLE		31
//梯控报警灵敏度
#define CMDOT_EBIKE_DETECT_SENSITIVITY	32
//梯控报警触发
#define CMDOT_EBIKE_DETECT_TRIGGER		33
//1400协议状态
#define CMDOT_1400_PROTOCAL_STATUS		34
//梯控报警
#define CMDOT_EBIKE_ALARM_STATUS		35
//设备能力集
#define CMDOT_ALGORITHM_SUPPORT			36
//机动车检测使能
#define CMDOT_MOTOR_VEHICLE_DETETCT		37
//告警入侵算法
#define CMDOT_REGION_HACK_ALGORITHM		38
//通知中间层HY_DVR_CMDOT_NOTIFY
#define CMDOT_NOTIFY					39
//口罩检测算法
#define CMDOT_FACE_MASK_DETECT_STATUS   40
//出入口设置
#define CMDOT_CAR_EXIT_FLAG_STATUS   41
//喇叭处理优先级V2
#define CMDOT_SPEAKER_STATUS_PRIORITY_V2   42
//高空抛物开关设置
#define CMDOT_FALLING_OBJECT_DETECT_STATUS 43
//高空抛物灵敏度设置
#define CMDOT_FALLING_OBJECT_DETECT_SENSITIVITY 44
//高空抛物置信度设置
#define CMDOT_FALLING_OBJECT_CONFIDENCE_LEVEL 45
//高空抛物检测区域
#define CMDOT_FALLING_OBJECT_DETECT_REGION 46
//车道线检测区域
#define CMDOT_MOTOR_VEHICLE_OBJECT_DETECT_REGION 47
//日志打捞
#define CMDOT_GET_DEVICE_ERRLOG       48
//夜视模式设置
#define CMDOT_NIGHT_VISION_MODE         49
//出入口设置
#define CMDOT_CAR_EXIT_FLAG_EVN   50
//获取使用的码流类型
#define CMDOT_STREAM_TYPE				51
//获取移动设备产品信息
#define CMDOT_SYSTEM_PRODUCT_INFO  52
///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取中国移动DM平台相关配置 */
#define HY_DVR_GET_DM_CFG		824
/** @brief 设置中国移动DM平台相关配置 */
#define HY_DVR_SET_DM_CFG		825
//次命令 (HY_DVR_DM_INIT_PARAM_INFO)
#define DM_INIT_PARAM_INFO			1
//次命令 (HY_DVR_DM_READ_INFO)
#define DM_READ_INFO				2
//次类型(HY_DVR_DM_LAMP_INFO)
#define DM_LAMP_INFO 3
#define DM_SOUND_COLUMN_INFO 4


///////////////////////////////////////////////////////////////////////////////////////////////////////
//天翼V2初始化配置    
#define HY_DVR_TIANYIV2_CFG_INIT      826
//获取初始化设备能力参数     HY_DVR_TIANYIV2_INIT_INFO
#define TIANYIV2_INIT_INFO            1
//协议告知中间层目前的注册状态     HY_DVR_TIANYIV2_DEVICE_STATUS
#define TIANYIV2_RESIGER_STATUS       2
//协议找中间层要目前的打印信息     HY_DVR_TIANYIV2_LOG_INFO
#define TIANYIV2_LOG_FILE_INFO        3
//中间层在此值设置后，通知协议后续的网络情况    HY_DVR_TIANYIV2_NET_FLAG
#define TIANYIV2_NET_FLAG        4
//协议告诉中间层1400的开关状态        HY_DVR_TIANYIV2_1400_SWITH 
#define TIANYIV2_1400_SWITH_STATUS     5
//协议告诉中间层1400的相关配置        HY_DVR_TIANYIV2_1400_INFO
#define TIANYIV2_1400_INFO             6
//协议获取中间层的系统动态参数        HY_DVR_TIANYIV2_SYSTEM_DYNAMIC_CFG
#define TINAYIV2_SYSTEM_DYNAMIC_CFG    7
//协议获取中间层额外的网络参数        HY_DVR_TIANYIV2_SYSTEM_NET_EX_CFG
#define TINAYIV2_SYSTEM_NET_EX_CFG    8
//协议告诉中间层1400的上线状态        HY_DVR_TIANYIV2_1400_LOGIN_STATUS 
#define TIANYIV2_1400_LOGIN_STATUS     9
//协议告诉中间层ipv6开关        HY_DVR_TIANYIV2_IPV6_SWITCH 
#define TIANYIV2_IPV6_SWITCH    10
// 协议告诉中间层二维码配网信息 HY_DVR_TIANYIV2_CONFIG_NETWORK_WIFI_INFO
#define TIANYIV2_WIFI_INFO    11
// HY_DVR_TIANYIV2_PTZ_POS_INFO
#define TIANYIV2_PTZ_POS_INFO    12

//天翼V2音视频配置     
#define HY_DVR_TIANYIV2_MEDIA         827
//获取音视频的能力     HY_DVR_TIANYIV2_MEDIA_ABILITY
#define TIANYIV2_MEDIA_ABILITY        1
//检测修复SD卡      HY_DVR_TIANYIV2_SD_REPAIR 
#define TIANYIV2_SD_REPAIR            2
//获取系统语音列表      HY_DVR_TIANYIV2_SOUND_FILE_LIST
#define TIANYIV2_GET_SOUND_LIST       3
//删除系统语音     HY_DVR_TIANYIV2_SOUND_FILE_DELETE
#define TIANYIV2_DELETE_SOUND         4
//从中间层获取一帧图片
#define TIANYIV2_FRAME_DATA           5 
//从中间层获取音频配置参数
#define TIANYIV2_AUDIO_PARAM          6 
//让中间层播放警戒声    HY_DVR_TIANYIV2_ALARM_VOICE
#define TIANYIV2_ALARM_VOICE          7
//从中间层获取小喇叭的参数    HY_DVR_TIANYIV2_SMALL_SPEAK
#define TINAYIV2_SMALL_SPEAK          8
//从中间层获取警戒声播放状态  HY_DVR_TIANYIV2_ALARM_SPEAK_STATUS
#define TIANYIV2_ALARM_SPEAK_STATUS   9
//获取设备的能力新增     HY_DVR_TIANYIV2_MEDIA_ABILITY_EX
#define TIANYIV2_MEDIA_ABILITY_EX     10
//让中间层播放语音(扩展)    HY_DVR_TIANYIV2_ALARM_VOICE_EX
#define TIANYIV2_ALARM_VOICE_EX          11
//自定义语音的相关设置      HY_DVR_TIANYIV2_ALARM_CUSTOM_SOUND
#define TIANYIV2_ALARM_CUSTOM_SOUND      12 


//设置天翼V2图像信息      
#define HY_DVR_TIANYIV2_PIC_PARAM_CFG      828
//设置osd      HY_DVR_TIANYIV2_OSD_SET
#define TIANYIV2_PIC_OSD              1
//设置设备红外状态   HY_DVR_TIANYIV2_PIC_PARAM  
#define TIANYIV2_PIC_IR               2
//设置时间osd的开关  HY_DVR_TIANYIV2_OSD_FLAG
#define TIANYIV2_TIME_OSD_FLAG        3
//设置自定义OSD的开关  HY_DVR_TIANYIV2_OSD_FLAG
#define TIANYIV2_CUSTOM_OSD_FLAG        4
//向中间层要红外灯的开关状态  HY_DVR_TIANYIV2_IR_STATUS
#define TIANYIV2_IR_LED_STATUS        5

//获取设备IOT相关能力     
#define HY_DVR_TIANYIV2_IOT_PARAM       829
//获取人脸数据库人数     HY_DVR_TIANYIV2_IOT_FACE_LABEL_MEMBER
#define TIANYIV2_IOT_FACE_LABEL_MEMBER  	1
//获取人形配置能力     HY_DVR_TIANYIV2_IOT_POLICY
#define TIANYIV2_IOT_HUMAN_ABILITY  	2
//获取移动侦测配置能力    HY_DVR_TIANYIV2_IOT_POLICY
#define TIANYIV2_IOT_MOTION_ABILITY  	3
//获取和设置区域围栏
#define TIANYIV2_IOT_AlARM_FENCE        4
//设置设备灯光报警   HY_DVR_TIANYIV2_IOT_ALARM_LAMP  
#define TIANYIV2_IOT_AlARM_LAMP             5
//AI创建成员       HY_DVR_TIANYIV2_IOT_CREAT_AI_LABEL
#define TIANYIV2_IOT_CREAT_AI_LABEL         6
//AI图片入库         HY_DVR_TIANYIV2_IOT_ADD_PIC_IN_LABEL
#define TIANYIV2_IOT_ADD_PIC_IN_LABEL       7
//AI删除图片         HY_DVR_TIANYIV2_IOT_DELETE_PIC_IN_LABEL
#define TIANYIV2_IOT_DELETE_PIC_IN_LABEL    8
//AI修改图片所在组   HY_DVR_TIANYIV2_IOT_MODIFY_PIC_LABEL
#define TIANYIV2_IOT_MODIFY_PIC_LABEL       9
//AI删除成员       HY_DVR_TIANYIV2_IOT_DELETE_AI_LABEL
#define TIANYIV2_IOT_DELETE_AI_LABEL        10
//人流统计 开关与时间间隔设置   HY_DVR_TIANYIV2_IOT_HUMANCOUNT_SET_PARAM
#define TIANYIV2_IOT_HUMANCOUNT_SET_PARAM        11
//人流统计 区域设置   HY_DVR_TIANYIV2_IOT_HUMANCOUNT_SET_REGIONS
#define TIANYIV2_IOT_HUMANCOUNT_SET_REGIONS        12
//设置AI算法模式   为了解决车牌算法和人脸算法不兼容的问题 HY_DVR_TIANYIV2_IOT_AI_MODE_SET
#define TIANYIV2_IOT_AI_MODE_SET        13
//人脸抓拍或人脸识别  HY_DVR_TIANYIV2_IOT_FACE_MODE_SET
#define TIANYIV2_IOT_FACE_MODE_SET        14
//人流统计 开关与时间间隔获取   HY_DVR_TIANYIV2_IOT_HUMANCOUNT_SET_PARAM
#define TIANYIV2_IOT_HUMANCOUNT_GET_PARAM        15
//口罩检测 开关   HY_DVR_TIANYIV2_IOT_MASK
#define TIANYIV2_IOT_MASK_SET_PARAM        16
//AI开关运行互斥  HY_DVR_TIANYIV2_IOT_AI_WORK_SWITCH
#define TIANYIV2_IOT_AI_WORK_SWITCH        17
//摄像头的状态灯的控制  HY_DVR_TIANYIV2_IOT_INNER_STATE_LAMP
#define TIANYIV2_IOT_INNER_STATE_LAMP      18
//获取和设置电动车   HY_DVR_TIANYIV2_IOT_ALARM_BATTERYBIKE
#define TIANYIV2_IOT_AlARM_BATTERYBIKE        19

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取协议版本号 */
#define HY_DVR_GET_PROTOCOL_INFO		830
/** @brief 获取协议版本号 */
#define HY_DVR_SET_PROTOCOL_INFO		831

//次命令
/** @brief 协议版本号 (HY_DVR_PROTOCOL_VERSION)*/
#define PROTOCOL_VERSION	1
/** @brief 协议版本号 (HY_DVR_PROTOCOL_VERSION_V1)*/
#define PROTOCOL_VERSION_V1				2

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取中国移动VOIP平台相关配置 */
#define HY_DVR_GET_CM_VOIP_CFG			832
/** @brief 设置中国移动VOIP平台相关配置 */
#define HY_DVR_SET_CM_VOIP_CFG			833

//次命令
/** @brief 设备配置*/
#define CM_VOIP_DEVICE_CFG	1
/** @brief 设备配置扩展字段*/
#define CM_VOIP_DEVICE_CFG_EX	2


/**************************************************************************
*													健康配置									             *
************************************************************************* */
/*主命令*/
#define HY_DVR_GET_HEALTH_CONFIG		834
#define HY_DVR_SET_HEALTH_CONFIG		835

/*次命令*/
//健康检测基本配置
#define HEALTH_BASE_CONFIG		1

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取LionKing配置信息 */
#define HY_DVR_GET_LIONKING_BASIC_INFO		836
/** @brief 设置LionKing配置信息 */
#define HY_DVR_SET_LIONKING_BASIC_INFO		837
//次命令 (HY_DVR_LIONKING_CFG)
#define LIONKING_BASIC_INFO		1
//次命令 (HY_DVR_LIONKING_SERIAL_CMD_FORMAT)
#define LIONKING_SERIAL_CMD_FORMAT		2
//次命令 (HY_DVR_LIONKING_SERIAL_SETGUARD)
#define LIONKING_SERIAL_SETGUARD		3
//次命令 (HY_DVR_LIONKING_SERIAL_SETALARM)
#define LIONKING_SERIAL_SETALARM		4
//次命令 (HY_DVR_LIONKING_SERIAL_SETPGM1_PORT)
#define LIONKING_SERIAL_SETPGM1_PORT		5
//次命令 (HY_DVR_LIONKING_SERIAL_SETPGM2_PORT)
#define LIONKING_SERIAL_SETPGM2_PORT		6
/////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取手动申请DID信息 */
#define HY_DVR_GET_APPLY_DID_MANUL                         		838
/** @brief 设置手动申请DID信息 */
#define HY_DVR_SET_APPLY_DID_MANUL                           	839
/** @brief 手动申请DID信息 (HY_DVR_APPLY_DID_MANUL_CFG) */
#define HY_DVR_APPLY_DID_MANUL        							1
/////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取电子围栏信息 */
#define HY_DVR_GET_ELECTRONIC_FENCE                         		840
/** @brief 设置电子围栏信息 */
#define HY_DVR_SET_ELECTRONIC_FENCE                          		841
/** @brief 电子围栏界面设计信息 (HY_DVR_ELECTRONIC_FENCE_UI_DESIGN_CFG) */
#define HY_DVR_ELECTRONIC_FENCE_UI_DESIGN        					1
/** @brief 电子围栏界面设计信息 (HY_DVR_ELECTRONIC_FENCE_EX_CFG) */
#define ELECTRONIC_FENCE_EX_CFG                                     2
/** @brief 电子围栏事件时间段配置(HY_DVR_ELECTRONIC_FENCE_EVENT_TIMESLOT_LIST) */
#define ELECTRONIC_FENCE_EVENT_TIMESLOT_CFG                         3
/** @brief 电子围栏事件语音报警时间段配置(HY_DVR_ELECTRONIC_FENCE_VOICEALARM_TIMESLOT_LIST) */
#define ELECTRONIC_FENCE_EVENT_VOICEALARM_TIMESLOT_CFG              4
///////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取越界侦测信息	*/
#define HY_DVR_GET_CROSSBORDER_DETECT 								842
/** @brief 设置越界侦测信息	*/
#define HY_DVR_SET_CROSSBORDER_DETECT 								843
/** @brief  越界侦测配置信息(HY_DVR_CROSSBORDER_DETECT_UI_DESIGN_CFG_LIST) */
#define HY_DVR_CROSSBORDER_DETECT_UI_DESIGN_INFO					 1
/** @brief  越界侦测拓展配置信息(HY_DVR_CROSSBORDER_DETECT_CFG_EX) */
#define CROSSBORDER_DETECT_CFG_EX                                    2
/** @brief  越界侦测配置信息(HY_DVR_CROSSBORDER_DETECT_UI_DESIGN_CFG_V1_LIST) */
#define CROSSBORDER_DETECT_UI_DESIGN_INFO_V1            			 3
/** @brief  越界侦测事件时间段配置(HY_DVR_CROSSBORDER_DETECT_EVENT_TIMESLOT_LIST) */
#define CROSSBORDER_DETECT_EVENT_TIMESLOT_CFG                        4
/** @brief  越界侦测事件语音报警时间段配置(HY_DVR_CROSSBORDER_DETECT_VOICEALARM_TIMESLOT_LIST) */
#define CROSSBORDER_DETECT_EVENT_VOICEALARM_TIMESLOT_CFG             5
///////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取离岗侦测信息 */
#define HY_DVR_GET_OFFDUTY_DETECT                        			844
/** @brief 设置离岗侦测信息 */
#define HY_DVR_SET_OFFDUTY_DETECT                          			845
/** @brief 离岗侦测界面设计信息 (HY_DVR_OFFDUTY_DETECT_UI_DESIGN_CFG) */
#define HY_DVR_OFFDUTY_DETECT_UI_DESIGN        						1
/** @brief 离岗侦测界面设计信息 (HY_DVR_OFFDUTY_DETECT_CFG_EX) */
#define OFFDUTY_DETECT_CFG_EX         								2
//离岗侦测事件时间段配置(HY_DVR_OFFDUTY_DETECT_UI_DESIGN_TIMESLOT_LIST)
#define OFFDUTY_DETECT_UI_DESIGN_EVENT_TIMESLOT_CFG                 3
//离岗侦测事件语音报警时间段配置(HY_DVR_OFFDUTY_DETECT_UI_DESIGN_VOICEALARM_TIMESLOT_LIST)
#define OFFDUTY_DETECT_UI_DESIGN_EVENT_VOICEALARM_TIMESLOT_CFG      4
///////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取客流统计信息 */
#define HY_DVR_GET_PASSENGERFLOW_STATICS                       		846
/** @brief 设置客流统计信息 */
#define HY_DVR_SET_PASSENGERFLOW_STATICS                          	847
/** @brief 客流统计界面设计信息 (HY_DVR_PASSENGERFLOW_STATICS_UI_DESIGN_CFG) */
#define HY_DVR_PASSENGERFLOW_STATICS_UI_DESIGN        				1
/** @brief 客流统计界面设计信息 (HY_DVR_PASSENGERFLOW_STATICS_UI_DESIGN_CFG_V1) */
#define PASSENGERFLOW_STATICS_UI_DESIGN_V1           				2
/** @brief 客流统计拓展配置(HY_DVR_PASSENGERFLOW_STATICS_EX_CFG) */
#define PASSENGERFLOW_STATICS_EX_CFG                 				3
/** @brief 客流统计事件时间段配置(HY_DVR_OFFDUTY_DETECT_UI_DESIGN_TIMESLOT_LIST) */
#define PASSENGERFLOW_STATICS_UI_DESIGN_V1_EVENT_TIMESLOT_LIST_CFG                             4
/** @brief 客流统计事件语音报警时间段配置(HY_DVR_OFFDUTY_DETECT_UI_DESIGN_VOICEALARM_TIMESLOT_LIST) */
#define PASSENGERFLOW_STATICS_UI_DESIGN_V1_EVENT_VOICEALARM_TIMESLOT_LIST_CFG                 5
///////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////
/** @brief 每日小时数 */
#define DVR_DAILY_HOUR_NUM 24
/** @brief 每周天数 */
#define DVR_WEEK_DAY_NUM 7

//主命令
/** @brief 获取当日客流统计柱状图 */
#define HY_DVR_GET_PASSENGERFLOW_STATISTICS                     	848
/** @brief 设置当日客流统计柱状图 */
#define HY_DVR_SET_PASSENGERFLOW_STATISTICS                        	849
/** @brief 当日客流统计柱状图信息 (HY_DVR_PASSENGERFLOW_STATISTICS) */
#define HY_DVR_PASSENGERFLOW_STATISTICS_INFO       					1
///////////////////////////////////////////////////////////////////////////////////////////////////////

//主命令
/** @brief 获取批量入库状态	*/
#define HY_DVR_GET_FACEGROUP_ADDLIST_STATE 							850
/** @brief 设置批量入库状态	*/
#define HY_DVR_SET_FACEGROUP_ADDLIST_STATE 							851
/** @brief 批量入库开始状态(HY_DVR_FACEGROUP_ADDLIST_BEGIN_CFG) */
#define HY_DVR_FACEGROUP_ADDLIST_BEGIN								1
/** @brief 批量入库结束状态(HY_DVR_FACEGROUP_ADDLIST_END_CFG) */
#define HY_DVR_FACEGROUP_ADDLIST_END								2


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 中间层找协议获取CMCC_OVD设置 */
#define HY_DVR_GET_CMCC_OVD_CONFIG_FROM_PROTOCOL		852
/** @brief 中间层向协议设置CMCC_OVD设置 */
#define HY_DVR_SET_CMCC_OVD_CONFIG_TO_PROTOCOL		853

//次命令
/** @brief 中间层调用移动二维码解析接口(HY_DVR_OVD_CONFIG_NETWORK_INFO) */
#define OVD_CONFIG_NETWORK_INFO	1
/** @brief 实时流加密方式(HY_DVR_OVD_CONFIG_ENCRYPT_MODE) */
#define OVD_CONFIG_ENCRYPT_MODE	2
/** @brief 中间层调用移动视频帧加密接口(HY_DVR_OVD_CONFIG_CALL_ENCRYPT_INTERFACE)（返回0表示正常，-1表示错误） */
#define OVD_CONFIG_CALL_OVD_ENCRYPT_INTERFACE  3
/** @brief 音视频流调试模式(HY_DVR_OVD_CONFIG_AV_DEBUG_MODE) */
#define OVD_CONFIG_AV_DEBUG_MODE	4
/** @brief 中间层音频播放结束通知(NULL) */
#define OVD_CONFIG_AUDIO_PLAYOUT_OVER	5
/** @brief 中间层配置恢复默认通知(NULL) */
#define OVD_CONFIG_RESTORE_FACTORY  6


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取CMCC OVD设备信息 */
#define HY_DVR_GET_CMCC_OVD_DEVICE_INFO		854
/** @brief 设置CMCC OVD设备信息 */
#define HY_DVR_SET_CMCC_OVD_DEVICE_INFO		855

//次命令
/** @brief 设备能力(HY_DVR_CMCC_OVD_CAP) */
#define OVD_CAPABILITY		1
/** @brief 设备能力(HY_DVR_CMCC_OVD_CLIENT_PARAM) */
#define OVD_CLIENT_PARAM		2
/** @brief 设备能力(HY_DVR_CMCC_OVD_OTHER_CONFIG) */
#define OVD_OTHER_CONFIG		3
/*次命令字 设备指标HY_DVR_OVD_DEVICE_LOAD*/
#define OVD_DEVICE_LOAD 4
/** @brief 设备上一次重启原因(HY_DVR_CMCC_OVD_REBOOT_REASON) */
#define OVD_REBOOT_REASON		5

///////////////////////////////////////////////////////////////////////////////////////////////////////
/** @brief 获取CMCC OVD AI配置 */
#define HY_DVR_GET_CMCC_OVD_CHANNEL_AI_CONFIG		856
/** @brief 设置CMCC OVD AI配置 */
#define HY_DVR_SET_CMCC_OVD_CHANNEL_AI_CONFIG		857

//次命令
/** @brief 人脸配置(HY_DVR_CMCC_OVD_FACE) */
#define OVD_CHANNEL_FACE_CONFIG		1
/** @brief 厨房检测配置(HY_DVR_CMCC_OVD_KITCHEN_DETECTION) */
#define OVD_CHANNEL_KITCHEN_DETECTION_CONFIG		2
/** @brief 车辆检测配置(HY_DVR_CMCC_OVD_VEHICLE_DETECTION) */
#define OVD_CHANNEL_VEHICLE_DETECTION_CONFIG		3
/** @brief 电动车检测配置(HY_DVR_CMCC_OVD_NONMOTORVEHICLE_DETECTION) */
#define OVD_CHANNEL_NONMOTORVEHICLE_DETECTION_CONFIG		4
/** @brief 口罩检测配置(HY_DVR_CMCC_OVD_MASK_DETECTION) */
#define OVD_CHANNEL_MASK_DETECTION_CONFIG		5
/** @brief 客流统计配置(HY_DVR_CMCC_OVD_PASSENGER_FLOW) */
#define OVD_CHANNEL_PASSENGER_FLOW_CONFIG		6
/** @brief 在离岗配置(HY_DVR_CMCC_OVD_OFF_DUTY_LIST) */
#define OVD_CHANNEL_OFF_DUTY_CONFIG		7

///////////////////////////////////////////////////////////////////////////////////////////////////////
/** @brief 获取CMCC OVD ALARMS配置 */
#define HY_DVR_GET_CMCC_OVD_CHANNEL_ALARMS_CONFIG		858
/** @brief 设置CMCC OVD ALARMS配置 */
#define HY_DVR_SET_CMCC_OVD_CHANNEL_ALARMS_CONFIG		859

//次命令
/** @brief 外部报警配置(HY_DVR_CMCC_OVD_IO) */
#define OVD_CHANNEL_IO_CONFIG		1
/** @brief 哭声侦测配置(HY_DVR_CMCC_OVD_CRY) */
#define OVD_CHANNEL_CRY_CONFIG		2
/** @brief 声音侦测配置(HY_DVR_CMCC_OVD_VOICE) */
#define OVD_CHANNEL_VOICE_CONFIG		3
/** @brief 移动侦测配置(HY_DVR_CMCC_OVD_VOICE) */
#define OVD_CHANNEL_MOTION_CONFIG		4
/** @brief 拌网配置(HY_DVR_CMCC_OVD_VOICE) */
#define OVD_CHANNEL_CROSS_CONFIG		5
/** @brief 人形侦测配置(HY_DVR_CMCC_OVD_BODY) */
#define OVD_CHANNEL_BODY_CONFIG		6
/** @brief 报警区域配置(HY_DVR_CMCC_OVD_ALERT_AREA) */
#define OVD_CHANNEL_ALERT_AREA_CONFIG		7
/** @brief 红外物体移动配置(HY_DVR_CMCC_OVD_PIR) */
#define OVD_CHANNEL_PIR_CONFIG		8
/** @brief 防拆告警(HY_DVR_CMCC_OVD_LOSS_LOCK) */
#define OVD_CHANNEL_LOSS_LOCK_CONFIG		9
/** @brief 越界告警配置(HY_DVR_CMCC_OVD_TRANSGRESSION) */
#define OVD_CHANNEL_TRANSGRESSION_CONFIG 10
/** @brief 音乐播放输出配置(HY_DVR_CMCC_OVD_SPEECH) */
#define OVD_CHANNEL_SETAUDIOOUTPLAY_CONFIG 11

///////////////////////////////////////////////////////////////////////////////////////////////////////
/** @brief 获取CMCC OVD OSD配置 */
#define HY_DVR_GET_CMCC_OVD_CHANNEL_OSD_CONFIG		860
/** @brief 设置CMCC OVD OSD配置 */
#define HY_DVR_SET_CMCC_OVD_CHANNEL_OSD_CONFIG		861

//次命令
/** @brief OSD TEXT配置(HY_DVR_CMCC_OVD_OSD_TEXT) */
#define OVD_CHANNEL_OSD_TEXT_CONFIG	1
/** @brief OSD LOGO配置(HY_DVR_CMCC_OVD_OSD_LOGO) */
#define OVD_CHANNEL_OSD_LOGO_CONFIG		2

///////////////////////////////////////////////////////////////////////////////////////////////////////
/** @brief 获取CMCC OVD 通道其他配置 */
#define HY_DVR_GET_CMCC_OVD_CHANNEL_BASIC_CONFIG		862
/** @brief 设置CMCC OVD 通道其他配置 */
#define HY_DVR_SET_CMCC_OVD_CHANNEL_BASIC_CONFIG		863

//次命令
/** @brief 通道开关配置(HY_DVR_CMCC_OVD_SWITCH_SCHEDULE) */
#define OVD_CHANNEL_SWITCH_SCHEDULE		1
/** @brief 通道其他配置(HY_DVR_CMCC_OVD_CHANNEL_OTHER_CONFIG) */
#define OVD_CHANNEL_OTHER_CONFIG		2
/** @brief 通道PTZ配置(HY_DVR_CMCC_OVD_CHANNEL_PTZ_PRESET_LIST_CONFIG) */
#define OVD_CHANNEL_PTZ_PRESET_LIST_CONFIG		3

///////////////////////////////////////////////////////////////////////////////////////////////////////
/** @brief 获取CMCC OVD 设备配置 */
#define HY_DVR_GET_CMCC_OVD_DEVICE_CONFIG		864
/** @brief 设置CMCC OVD 设备配置 */
#define HY_DVR_SET_CMCC_OVD_DEVICE_CONFIG		865

//次命令
/** @brief 通除了通道外的配置(HY_DVR_CMCC_OVD_DEVICE_OTHER_CONFIG) */
#define OVD_DEVICE_CONFIG_EXPECT_CHANNEL		1
/** @brief 视频质量(HY_DVR_CMCC_OVD_VIDEO_ENCODING) */
#define OVD_CURRENT_VIDEO_QUALITY		2
/** @brief 获取日志信息(HY_DVR_CMCC_OVD_LOG_INFO) */
#define OVD_LOG_INFO		3
/** @brief 唤醒过期时间(HY_DVR_CMCC_OVD_KEEP_AWAKEN_UTIL_EXPIRED) */
#define OVD_KEEP_AWAKEN_UTIL_EXPIRED		4
/** @brief 清晰度码流对照表(HY_DVR_CMCC_OVD_QUALITY_STREAM_MAP) */
#define OVD_QUALITY_STREAM_MAP		5
/** @brief 获取日志信息(HY_DVR_CMCC_OVD_SOFTPROBE_STATUS_INFO) */
#define OVD_SOFTPROBE_STATUS_INFO		6


//主类型
/** @brief 获取第三方平台连接状态信息 */
#define HY_DVR_GET_LL_PLAT_CONNECT_STATE_INFO				866
/** @brief 设置第三方平台连接状态信息 */
#define HY_DVR_SET_LL_PLAT_CONNECT_STATE_INFO				867

/** @brief  对于结构体 HY_DVR_LL_PLAT_CONNECT_STATE_INFO_CFG */
#define HY_DVR_LL_PLAT_CONNECT_STATE_INFO					1

//主类型
/** @brief 获取语音播报 */
#define HY_DVR_GET_VOICE_BROADCAST			    868
/** @brief 设置语音播报 */
#define HY_DVR_SET_VOICE_BROADCAST				869

/** @brief  对应结构体 HY_DVR_VOICE_BROADCAST_CFG_INFO */
#define HY_DVR_VOICE_BROADCAST_CFG				1

//主类型
/** @brief 获取屏幕UI人脸事件提示使能开关 */
#define HY_DVR_GET_SCREEN_UIPERSONFACE_EVENT_PROMPT_SWITCH			    870
/** @brief 设置屏幕UI人脸事件提示使能开关 */
#define HY_DVR_SET_SCREEN_UIPERSONFACE_EVENT_PROMPT_SWITCH				871

/** @brief  对应结构体 HY_DVR_SCREEN_UIPERSONFACE_EVENT_PROMPT_SWITCH_CFG */
#define HY_DVR_SCREEN_UIPERSONFACE_EVENT_PROMPT_SWITCH				1

//主类型
/** @brief 获取客户自定义UI提示 */
#define HY_DVR_GET_CUSTOMER_DEFINE_UI_PROMPT			    872
/** @brief 设置客户自定义UI提示 */
#define HY_DVR_SET_CUSTOMER_DEFINE_UI_PROMPT				873

/** @brief  对应结构体 HY_DVR_CUSTOMER_DEFINE_UI_PROMPT_INFO */
#define HY_DVR_CUSTOMER_DEFINE_UI_PROMPT			1

//主类型
/** @brief 获取CUIOT设备鉴权信息 */
#define HY_DVR_GET_CUIOT_DEVICE_AUTH_INFO			    874
/** @brief 设置CUIOT设备鉴权信息 */
#define HY_DVR_SET_CUIOT_DEVICE_AUTH_INFO				875

/** @brief  对应结构体 HY_DVR_CUIOT_DEVICE_AUTH_INFO_CFG */
#define HY_DVR_CUIOT_DEVICE_AUTH_INFO				1

//主类型
/** @brief 获取CUIOTV服务器参数 */
#define HY_DVR_GET_CUIOTV_SERVER_CONFIG			    		876
/** @brief 设置CUIOTV服务器参数 */
#define HY_DVR_SET_CUIOTV_SERVER_CONFIG						877

/** @brief  对应结构体 HY_DVR_CUIOTV_SERVER_CONFIG_INFO */
#define HY_DVR_CUIOTV_SERVER_CONFIG						1

//主类型
/** @brief 获取ANDLINK参数 */
#define HY_DVR_GET_ANDLINK_CONFIG			    		878
/** @brief 设置ANDLINK参数 */
#define HY_DVR_SET_ANDLINK_CONFIG						879

/** @brief  对应结构体 HY_DVR_ANDLINK_DM_INFO */
#define HY_DVR_ANDLINK_DM_CONFIG						1


//主类型
/** @brief 获取报警通知 */
#define HY_DVR_GET_OVD_ALARM_CFG			    		880
/** @brief 设置报警通知 */
#define HY_DVR_SET_OVD_ALARM_CFG						881

/** @brief  对应结构体 HY_DVR_OVD_ALARM_CFG */
#define OVD_ALARM_NOTIFY						1
/** @brief  对应结构体 HY_DVR_OVD_ADL_NOTIFY_INFO */
#define OVD_ADL_NOTIFY						    2


//主类型
/** @brief 获取事件记录上传定制内容 */
#define HY_DVR_GET_EVENT_RECORD_UPLOAD_CUSTOM			    882
/** @brief 设置事件记录上传定制内容 */
#define HY_DVR_SET_EVENT_RECORD_UPLOAD_CUSTOM				883

/** @brief  对应结构体 HY_DVR_EVENT_RECORD_UPLOAD_CUSTOM_CFG */
#define HY_DVR_EVENT_RECORD_UPLOAD_CUSTOM					1


//主类型
/** @brief 获取历史事件记录上传时间段 */
#define HY_DVR_GET_HISTORY_EVENT_RECORD_TIMESLOT			    884
/** @brief 设置历史事件记录上传时间段 */
#define HY_DVR_SET_HISTORY_EVENT_RECORD_TIMESLOT				885

/** @brief  对应结构体 HY_DVR_HISTORY_EVENT_RECORD_TIMESLOT_CFG */
#define HY_DVR_HISTORY_EVENT_RECORD_TIMESLOT					1


//主类型
/** @brief 获取事件间隔和其它设置 */
#define HY_DVR_GET_EVENT_INTERVAL_AND_OTHER_SETTINGS			    886
/** @brief 设置事件间隔和其它设置 */
#define HY_DVR_SET_EVENT_INTERVAL_AND_OTHER_SETTINGS				887

/** @brief  对应结构体 HY_DVR_EVENT_INTERVAL_AND_OTHER_SETTINGS_CFG */
#define HY_DVR_EVENT_INTERVAL_AND_OTHER_SETTINGS					1


//主类型
/** @brief 获取人脸识别间隔设置 */
#define HY_DVR_GET_FACE_RECOGNITION_INTERVAL			    888
/** @brief 设置人脸识别间隔设置 */
#define HY_DVR_SET_FACE_RECOGNITION_INTERVAL				889

/** @brief  对应结构体 HY_DVR_FACE_RECOGNITION_INTERVAL_CFG */
#define HY_DVR_FACE_RECOGNITION_INTERVAL					1

///////////////////////////////////////////////////////////////////////////////////////////////////////
/** @brief 获取WebSocket配置 */
#define HY_DVR_GET_WEBSOCKET_BASE_CFG		890
/** @brief 设置WebSocket配置 */
#define HY_DVR_SET_WEBSOCKET_BASE_CFG		891

/** @brief 设置WebSocket配置(HY_DVR_WEKSOCKET_BASE_CFG) */
#define WEBSOCKET_BASE_CFG		1


//主命令
/** @brief 获取二维码配置 */
#define HY_DVR_GET_QR_CODE_CFG          					892
/** @brief 设置二维码配置 */
#define HY_DVR_SET_QR_CODE_CFG          					893

//对应的结构体（HY_DVR_QR_CODE_CFG_INFO）
#define HY_DVR_QR_CODE_CFG      							1

//主命令
/** @brief 获取远程开门配置 */
#define HY_DVR_GET_REMOTE_OPEN_DOOR_CFG  					894
/** @brief 设置远程开门配置 */
#define HY_DVR_SET_REMOTE_OPEN_DOOR_CFG  					895

//对应的结构体（HY_DVR_REMOTE_OPEN_DOOR_CFG_INFO）
#define HY_DVR_REMOTE_OPEN_DOOR_CFG	   	 					1


//主命令
/** @brief 获取远程开门配置 */
#define HY_DVR_GET_PTZ_TEST  								896
/** @brief 设置远程开门配置 */
#define HY_DVR_SET_PTZ_TEST	  								897

//对应的结构体
#define HY_DVR_PTZ_TEST	   	 								1


//主命令
/** @brief 获取光敏测试 */
#define HY_DVR_GET_PHOTO_SENSITIVE_TEST  							898
/** @brief 设置光敏测试 */
#define HY_DVR_SET_PHOTO_SENSITIVE_TEST  							899

//对应的结构体(HY_DVR_PHOTO_SENSITIVE_TEST_CFG)
#define HY_DVR_PHOTO_SENSITIVE_TEST	   	 							1

//主命令
/** @brief 获取USB导出考勤记录结果配置 */
#define HY_DVR_GET_USB_EXPORT_ATTEND_RECORD_RESULT  		900
/** @brief 设置USB导出考勤记录结果配置 */
#define HY_DVR_SET_USB_EXPORT_ATTEND_RECORD_RESULT  		901

//USB导出考勤记录结果(HY_DVR_USB_EXPORT_ATTEND_RECORD_RESULT_CFG)
#define HY_DVR_USB_EXPORT_ATTEND_RECORD_RESULT      		1


//主命令
/** @brief 获取远程升级检测结果配置 */
#define HY_DVR_GET_REMOTE_UPDATE_RESULT          			902
/** @brief 设置远程升级检测结果配置 */
#define HY_DVR_SET_REMOTE_UPDATE_RESULT          			903

//对应的结构体（HY_DVR_REMOTE_UPDATE_RESULT_INFO）
#define HY_DVR_REMOTE_UPDATE_RESULT     		 			1
////////////////////////////////////////////////////// P6SMessagePush //////////////////
//获取中间层云消息URL
#define HY_DVR_P6S_MP_GET_SEVER_PARAM           904
//设置中间层云消息URL
#define HY_DVR_P6S_MP_SET_SEVER_PARAM           905
//对应结构体(HY_DVR_MP_SERVER_URL_INFO)
#define HY_DVR_MP_SERVER_URL                     1
//获取云运维升级时的升级状态（HY_DVR_MP_UPDATE_STATUS_INFO）
#define HY_DVR_MP_UPDATE_STATUS                  2


//主命令
#define HY_DVR_GET_CLOUD_STORAGE_MODE						906
#define HY_DVR_SET_CLOUD_STORAGE_MODE 						907

//次命令
//云存储工作模式(HY_DVR_CLOUD_STORAGE_MODE)
#define CLOUD_STORAGE_MODE									1


//主类型
/** @brief 获取CUIOT事件记录上传定制内容 */
#define HY_DVR_GET_CUIOT_EVENT_RECORD_UPLOAD_CUSTOM			    908
/** @brief 设置CUIOT事件记录上传定制内容 */
#define HY_DVR_SET_CUIOT_EVENT_RECORD_UPLOAD_CUSTOM				909

/** @brief  对应结构体 HY_DVR_CUIOT_EVENT_RECORD_UPLOAD_CUSTOM_CFG */
#define HY_DVR_CUIOT_EVENT_RECORD_UPLOAD_CUSTOM					1


//主类型
/** @brief 获取CUIOT历史事件记录上传时间段 */
#define HY_DVR_GET_CUIOT_HISTORY_EVENT_RECORD_TIMESLOT			910
/** @brief 设置CUIOT历史事件记录上传时间段 */
#define HY_DVR_SET_CUIOT_HISTORY_EVENT_RECORD_TIMESLOT			911

/** @brief  对应结构体 HY_DVR_CUIOT_HISTORY_EVENT_RECORD_TIMESLOT_CFG */
#define HY_DVR_CUIOT_HISTORY_EVENT_RECORD_TIMESLOT				1


//主类型
/** @brief 获取CUIOT事件间隔和其它设置 */
#define HY_DVR_GET_CUIOT_EVENT_INTERVAL_AND_OTHER_SETTINGS			    912
/** @brief 设置CUIOT事件间隔和其它设置 */
#define HY_DVR_SET_CUIOT_EVENT_INTERVAL_AND_OTHER_SETTINGS				913

/** @brief  对应结构体 HY_DVR_CUIOT_EVENT_INTERVAL_AND_OTHER_SETTINGS_CFG */
#define HY_DVR_CUIOT_EVENT_INTERVAL_AND_OTHER_SETTINGS					1


//主命令
//获取火焰探测配置
#define HY_DVR_GET_FIRE_DETECT_CFG           					914
//设置火焰探测配置
#define HY_DVR_SET_FIRE_DETECT_CFG           					915

//火焰探测配置对应结构体(HY_DVR_FIRE_DETECT_CFG_INFO)
#define HY_DVR_FIRE_DETECT_CFG                     				1
//火焰探测拓展配置对应结构体(HY_DVR_FIRE_DETECT_EX_CFG) 
#define FIRE_DETECT_EX_CFG         								2
#define DVR_MAX_FIRE_DETECT_REGION_NUM             				16



//  以下是联通协议层的命令：由移动命令改名而来  ，有一些联通没有的命令 暂时也没有删除//
//主命令
/** @brief 获取中国联通DOT平台相关配置 */
#define HY_DVR_GET_CUDOT_CFG		916
/** @brief 设置中国联通DOT平台相关配置 */
#define HY_DVR_SET_CUDOT_CFG		917

//次命令 (HY_DVR_CUDOT_PATH)
#define CUDOT_PATH					1	
//HY_DVR_CUDOT_DEVICE_INFO
#define CUDOT_DEVICE_INFO			2
//HY_DVR_CUDOT_DEVICE_INFO_EX
#define CUDOT_DEVICE_INFO_EX		3
//HY_DVR_CUDOT_STORAGE_INFO
#define CUDOT_STORAGE_INFO			4
//HY_DVR_CUDOT_MOTION_V3_LIST
#define CUDOT_MOTION_V3_LIST		5
//恢复出厂，NULL
#define CUDOT_RESET_FACTORY			6
//设置音量，HY_DVR_CUDOT_SPEAKER_INFO
#define CUDOT_SPEAKER_INFO			7
//绑定状态，HY_DVR_CUDOT_BIND_STATUS
#define CUDOT_BIND_STATUS			8
//授权信息，HY_DVR_CUDOT_AUTH_INFO
#define CUDOT_AUTH_INFO				9
//LOGO水印显示
#define CUDOT_OSD_LOGO				10
//文字水印显示
#define CUDOT_OSD_TEXT_INFO			11
//设置MtuSize
#define CUDOT_MTU_INFO				12
//设置告警入侵区域
#define CUDOT_HACK_REGION			13
//设置告警入侵使能
#define CUDOT_HACK_STATUS			14
//人脸检测基本配置项
#define CUDOT_FACE_DETECT_BASIC_INFO	15
//人脸检测区域
#define CUDOT_FACE_DETECT_REGION		16
//人脸检测其他参数
#define CUDOT_FACE_DETECT_PARAM			17
//视频分辨率
#define CUDOT_VIDEO_RESOLUTION_LEVEL	18
//喇叭处理优先级
#define CUDOT_SPEAKER_STATUS_PRIORITY	19
//设置告警入侵灵敏度
#define CUDOT_HACK_SENSITIVITY			20
//告警音量
#define CUDOT_ALARM_VOLUME_INFO			21
//声音侦测开关
#define CUDOT_SOUND_DETECT_STATUS		22
//声音侦测灵敏度
#define CUDOT_SOUND_DETECT_SENSITIVITY	23
//人脸捕获模式
#define CUDOT_FACE_CAPTURE_MODE			24
//人脸捕获质量
#define CUDOT_FACE_CAPTURE_QUALITY		25
//人形侦测框
#define CUDOT_PEOPLE_DRAW_BOX			26
//清空SpeakerBuffer
#define CUDOT_SPEKAR_BUFFER_CLEAR		27
//中间层音频播放结束通知
#define CUDOT_AUDIO_PLAYOUT_OVER		28
//设置码流格式
#define CUDOT_VIDEO_ENCODE				29
//获取负载状态
#define CUDOT_GET_DEVICE_LOAD			30
//梯控报警使能
#define CUDOT_EBIKE_DETECT_ENABLE		31
//梯控报警灵敏度
#define CUDOT_EBIKE_DETECT_SENSITIVITY	32
//梯控报警触发
#define CUDOT_EBIKE_DETECT_TRIGGER		33
//1400协议状态
#define CUDOT_1400_PROTOCAL_STATUS		34
//梯控报警
#define CUDOT_EBIKE_ALARM_STATUS		35
//设备能力集
#define CUDOT_ALGORITHM_SUPPORT			36
//机动车检测使能
#define CUDOT_MOTOR_VEHICLE_DETETCT		37
//告警入侵算法
#define CUDOT_REGION_HACK_ALGORITHM		38
//时间osd开关
#define CUDOT_TIME_OSD_STATUS           39
//HY_DVR_CUDOT_DEVICE_INFO_ADD
#define CUDOT_DEVICE_INFO_ADD			40
//车牌识别开关
#define CUDOT_CAR_PLATE_DETETCT 		41
//出入口标识
#define CUDOT_CAR_EXIT_FLAG_EVN 		42


//主命令
/** @brief 获取设备服务状态 */
#define HY_DVR_GET_DEVICE_ON_CUDOT_SERVER_STATUS		918
/** @brief 设置设备服务状态 */
#define HY_DVR_SET_DEVICE_ON_CUDOT_SERVER_STATUS		919
//次命令 (HY_DVR_CUDOT_DEVICE_STATUS)
#define CUDOT_DEVICE_STATUS					1
// 绑定失败次数
#define CUDOT_DEVICE_BIND_FAIL_COUNT        2

//主命令
//获取健康码平台信息
#define HY_DVR_GET_JKM_PLATFORM_CFG           920
//设置健康码平台信息
#define HY_DVR_SET_JKM_PLATFORM_CFG           921

//健康码平台信息对应结构体(HY_DVR_JKM_PLATFORM_CFG_INFO)
#define HY_DVR_JKM_PLATFORM_CFG                     1


//主命令
//获取数字哨兵平台鉴权信息
#define HY_DVR_GET_SZSB_PLATFORM_AUTH           922
//设置数字哨兵平台鉴权信息
#define HY_DVR_SET_SZSB_PLATFORM_AUTH           923

//数字哨兵平台鉴权信息(HY_DVR_SZSB_PLATFORM_AUTH_INFO)
#define HY_DVR_SZSB_PLATFORM_AUTH                      1


/*主命令*/
//获取数字哨兵平台设备配置
#define HY_DVR_GET_SZSB_PLATFORM_DEVICE_CFG		924
//设置数字哨兵平台设备配置
#define HY_DVR_SET_SZSB_PLATFORM_DEVICE_CFG		925

/*次命令*/
//数字哨兵平台设备配置 (HY_DVR_SZSB_PLATFORM_DEVICE_CFG_INFO)
#define HY_DVR_SZSB_PLATFORM_DEVICE_CFG		1


//主命令
/** @brief 获取SWP平台公司名和设备场所配置 */
#define HY_DVR_GET_SWP_PLANTFORM_COMPANY_DEVICE  926
/** @brief 设置SWP平台公司名和设备场所配置 */
#define HY_DVR_SET_SWP_PLANTFORM_COMPANY_DEVICE  927

//USB导出考勤记录结果(HY_DVR_SWP_PLANTFORM_COMPANY_DEVICE_INFO)
#define HY_DVR_SWP_PLANTFORM_COMPANY_DEVICE      1


//主命令
//获取河南健康码设备激活码信息
#define HY_DVR_GET_HENAN_HEALTHCODE_DEVICE_CDKEY           928
//设置河南健康码设备激活码信息
#define HY_DVR_SET_HENAN_HEALTHCODE_DEVICE_CDKEY           929

//河南健康码设备激活码信息(HY_DVR_HENAN_HEALTHCODE_DEVICE_CDKEY_INFO)
#define HY_DVR_HENAN_HEALTHCODE_DEVICE_CDKEY                      1


//主类型
/** @brief 获取设备睡眠信息 */
#define HY_DVR_GET_DEVICE_SLEEP_INFO  930 
/** @brief 设置设备睡眠信息 */
#define HY_DVR_SET_DEVICE_SLEEP_INFO   931

//次类型(HY_DVR_DEVICE_SLEEP_INFO_CFG)
#define SLEEPCFG_ALL  0xFFFFFFFF


//主类型
/** @brief 获取设备当前环境空气质量信息 */
#define HY_DVR_GET_DEVICE_AIR_DECTECT_INFO  932 
/** @brief 设置设备当前环境空气质量信息 */
#define HY_DVR_SET_DEVICE_AIR_DECTECT_INFO   933

//次类型(HY_DVR_AIR_QUALITY_INFO_CFG)
#define AIRCFG_ALL  0xFFFFFFFF
/** 气体检测声光报警配置(HY_DVR_AIR_DECTECT_AUDIO_LIGHT_ALARM_CFG) */
#define AIR_DECTECT_AUDIO_LIGHT_ALARM_CFG        1


//主类型
/** @brief 获取设备一键夜灯信息 */
#define HY_DVR_GET_NIGHT_LED_INFO  934 
/** @brief 设置设备一键夜灯信息 */
#define HY_DVR_SET_NIGHT_LED_INFO   935

//次类型(HY_DVR_NIGHT_LED_INFO_CFG)
#define NIGHT_LED_CFG_ALL  0xFFFFFFFF


//主命令
/** @brief 获取卡选择配置*/
#define HY_DVR_GET_4G_CARD_SELECT_CFG                          936
/** @brief 设置卡选择配置 */
#define HY_DVR_SET_4G_CARD_SELECT_CFG                          937

//对应的结构体(HY_DVR_4G_CARD_SELECT_CFG_INFO)
#define HY_DVR_4G_CARD_SELECT_CFG                               1 
//对应的结构体(HY_DVR_4G_CARD_SELECT_RESULT_INFO)
#define HY_DVR_4G_CARD_SELECT_RESULT                           	2


//主命令
/** @brief 获取卡选择配置*/
#define HY_DVR_GET_4G_CARD_CFG                              	938
/** @brief 设置卡选择配置 */
#define HY_DVR_SET_4G_CARD_CFG                                  939

//对应的结构体(HY_DVR_4G_CARD_CFG_LIST)
#define HY_DVR_4G_CARD_CFG                                  	1

#define HY_DVR_MAX_4G_CARD_COUNT   4


//主命令
/** @brief 获取电源配置*/
#define HY_DVR_GET_POWER_CONFIG                              	940
/** @brief 设置电源配置 */
#define HY_DVR_SET_POWER_CONFIG                                 941

//对应的结构体(HY_DVR_POWER_CONFIG_INFO)
#define HY_DVR_POWER_CONFIG                                 	1


//主类型
/** @brief 获取设备睡眠信息 */
#define HY_DVR_GET_DEVICE_SLEEP_CONTROL  						942
/** @brief 设置设备睡眠信息 */
#define HY_DVR_SET_DEVICE_SLEEP_CONTROL   						943

//次类型(HY_DVR_DEVICE_SLEEP_CONTROL)
#define SLEEP_CONTROL  											1


//主命令
//获取OneNet平台健康码设备激活码信息
#define HY_DVR_GET_ONENET_HEALTHCODE_DEVICE_CDKEY              944
//设置OneNet平台健康码设备激活码信息
#define HY_DVR_SET_ONENET_HEALTHCODE_DEVICE_CDKEY              945

//OneNet平台健康码设备激活码信息(HY_DVR_ONENET_HEALTHCODE_DEVICE_CDKEY_INFO)
#define HY_DVR_ONENET_HEALTHCODE_DEVICE_CDKEY                      1


//主命令
//获取视频遮挡报警配置
#define HY_DVR_GET_VIDEO_SHELTER_ALARM_CFG                     	946
//设置视频遮挡报警配置
#define HY_DVR_SET_VIDEO_SHELTER_ALARM_CFG               		947

//视频遮挡报警配置(HY_DVR_VIDEO_SHELTER_ALARM_CFG)
#define VIDEO_SHELTER_ALARM_CFG                           		1


//主命令
//获取河南健康码OneNet平台
#define HY_DVR_GET_HENAN_HEALTHCODE_ONENET           			948
//设置河南健康码OneNet平台
#define HY_DVR_SET_HENAN_HEALTHCODE_ONENET           			949

//人脸图片信息(HY_DVR_ONENET_EMERGENCY_ACCESS_RULE)
#define HENAN_HEALTHCODE_ONENET_BOOT_EVENT           			1
//人脸图片信息(HY_DVR_ONENET_PASSJUDGERESULT)
#define HENAN_HEALTHCODE_ONENET_BRUSH_EVENT           			2
//人脸图片信息(HY_DVR_ONENET_ZJHM_QUERY_DATA)
#define HENAN_HEALTHCODE_ONENET_FACE_EVENT           			3


/*主命令*/
//获取设备安装信息
#define HY_DVR_GET_DEVICE_INSTALL_INFO        			950
//设置设备安装信息
#define HY_DVR_SET_DEVICE_INSTALL_INFO        			951

/*次命令*/
//设备安装信息 (HY_DVR_DEVICE_INSTALL_INFO)
#define DEVICE_INSTALL_INFO                   			1



#define HY_DVR_GET_CLOUD_STORAGE_SNAPSHOT_PIC       952
#define HY_DVR_SET_CLOUD_STORAGE_SNAPSHOT_PIC       953

//次命令
//获取云存储图片（HY_DVR_CLOUD_STORAGE_SNAPSHOT_PIC）
#define CLOUD_STORAGE_PIC       1


//主命令
/** @brief 获取SWP平台显示项配置 */
#define HY_DVR_GET_SWP_PLANTFORM_DISPLAY_ITEM_CONFIG  954
/** @brief 设置SWP平台显示项配置 */
#define HY_DVR_SET_SWP_PLANTFORM_DISPLAY_ITEM_CONFIG  955

//SWP平台显示项配置(HY_DVR_SWP_PLANTFORM_DISPLAY_ITEM_CONFIG)
#define SWP_PLANTFORM_DISPLAY_ITEM_CONFIG      1


//主命令
/** @brief 获取健康码设备和平台信息                                                               */
#define HY_DVR_GET_HEALTHCODE_DEVICE_AND_PLAT_INFO         956
/** @brief 设置健康码设备和平台信息                                                                */
#define HY_DVR_SET_HEALTHCODE_DEVICE_AND_PLAT_INFO         957

//次类型
// @brief 设备和平台信息(HY_DVR_HEALTHCODE_DEVICE_AND_PLAT_INFO)
#define HEALTHCODE_DEVICE_AND_PLAT_INFO                     1


//主命令
/** @brief 获取联通激活信息 */
#define HY_DVR_GET_UNICOM_DEVICE_INFO						958
/** @brief 设置联通激活信息 */
#define HY_DVR_SET_UNICOM_DEVICE_INFO						959

//次类型
// @brief 激活信息(HY_DVR_UNICOM_AUTH_INFO)
#define UNICOM_AUTH_INFO                                   	1


//主命令
#define HY_DVR_GET_CURRENT_FRAME_PARAM 962

#define HY_DVR_SET_CURRENT_FRAME_PARAM 963
//次命令（HY_DVR_FRAME_INFO）
#define CURRENT_FRAME_PARAM_INFO 1


//主命令
//获取智能跟踪
#define HY_DVR_GET_INTELLIGENT_TRACK_CFG           964
//设置智能跟踪
#define HY_DVR_SET_INTELLIGENT_TRACK_CFG           965

//智能跟踪(HY_DVR_INTELLIGENT_TRACK_CFG)
#define INTELLIGENT_TRACK_CFG                      1
//智能跟踪事件时间段配置(HY_DVR_INTELLIGENT_TRACK_EVENT_TIMESLOT_LIST)
#define INTELLIGENT_TRACK_EVENT_TIMESLOT_CFG       2
//智能跟踪事件语音报警时间段配置(HY_DVR_INTELLIGENT_TRACK_VOICEALARM_TIMESLOT_LIST)
#define INTELLIGENT_TRACK_EVENT_VOICEALARM_TIMESLOT_CFG       3

//主命令
//获取流传输方式
#define HY_DVR_GET_STREAM_TRANSPORT_TYPE           966
//设置流传输方式
#define HY_DVR_SET_STREAM_TRANSPORT_TYPE           967

//次命令
#define STREAM_TRANSPORT_TYPE                     1

//主命令
//获取RMS平台信息
#define HY_DVR_GET_RMS_SERVER_INFO           968
//设置RMS平台信息
#define HY_DVR_SET_RMS_SERVER_INFO           969

//次命令
#define RMS_SERVER_INFO                     1

//主命令
/** @brief 获取离岗查询 */
#define HY_DVR_GET_AI_OFFDUTY_QUERY                  970
/** @brief 设置离岗查询(set无效) */
#define HY_DVR_SET_AI_OFFDUTY_QUERY                  971

/** @brief 离岗查询信息 (HY_DVR_AI_OFFDUTY_QUERY) */
#define AI_OFFDUTY_QUERY                              1


//主命令
//获取哭声叫声检测配置
#define HY_DVR_GET_CRY_SCREAM_DETECT 			972
//设置哭声叫声检测配置
#define HY_DVR_SET_CRY_SCREAM_DETECT 			973

//哭声叫声检测配置对应结构体(HY_DVR_CRY_SCREAM_DETECT_CFG)
#define CRY_SCREAM_DETECT_CFG 1

//主命令
//获取高抛配置
#define HY_DVR_GET_OVDSDK_CFG				974
//设置高抛配置
#define HY_DVR_SET_OVDSDK_CFG				975

//次命令
#define OVDSDK_FALLING_OBJECT_DETECT_ENABLE                     1
#define OVDSDK_FALLING_OBJECT_DETECT_CONFIG                     2
#define OVDSDK_FALLING_OBJECT_FLOOR_LINE_INFO                   3

//主命令 结构体tagHY_DVR_CLOUD_STORAGE_BASE_STATUS
//获取错误码信息
#define HY_DVR_GET_ERROR_CODE_INFO           976
//设置错误码信息
#define HY_DVR_SET_ERROR_CODE_INFO           977

//次命令，该字段表示获取哪个协议的错误码信息,具体数值定义见HiServer_Cfg.h协议名称定义，直接填协议名称即可
//例如：填PROTOCOL_ZW_CLOUD_STORAGE表示获取云存的错误码；
// #define ERROR_CODE_INFO                   PROTOCOL_ZW_CLOUD_STORAGE // 该值表示云存协议

//主命令 结构体tagHY_DVR_MP_POST_PARAM
//获取服务器必选参数
#define HY_DVR_GET_ERROR_CODE_SERVER_INFO           978
//设置服务器必选参数
#define HY_DVR_SET_ERROR_CODE_SERVER_INFO           979

//次命令
#define ERROR_CODE_SERVER_INFO                   1


//主命令
//获取人脸抓拍配置
#define HY_DVR_GET_AI_FACE_SNAPSHOT_CFG           980
//设置人脸抓拍配置
#define HY_DVR_SET_AI_FACE_SNAPSHOT_CFG           981

//人脸抓拍(HY_DVR_AI_FACE_SNAPSHOT_CFG)
#define AI_FACE_SNAPSHOT_CFG                      1
//人脸抓拍事件时间段配置(HY_DVR_AI_FACE_SNAPSHOT_EVENT_TIMESLOT_LIST)
#define AI_FACE_SNAPSHOT_EVENT_TIMESLOT_CFG 2
//人脸抓拍事件语音报警时间段配置(HY_DVR_AI_FACE_SNAPSHOT_VOICEALARM_TIMESLOT_LIST)
#define AI_FACE_SNAPSHOT_EVENT_VOICEALARM_TIMESLOT_CFG 3



//主命令
//获取口罩检测配置
#define HY_DVR_GET_AI_BREATHMASK_DETECT_CFG          982
//设置口罩检测配置
#define HY_DVR_SET_AI_BREATHMASK_DETECT_CFG          983

//口罩检测(HY_DVR_AI_BREATHMASK_DETECT_CFG)
#define AI_BREATHMASK_DETECT_CFG                      1


//主命令
/** @brief 获取人脸算法版本 */
#define HY_DVR_GET_AI_FACE_ALGORITHM_VERSION          984
/** @brief 设置人脸算法版本(未实现) */
#define HY_DVR_SET_AI_FACE_ALGORITHM_VERSION          985

//人脸算法版本(HY_DVR_AI_FACE_ALGORITHM_VERSION)
#define AI_FACE_ALGORITHM_VERSION                      1


//主类型
/** @brief 获取前端脸库绑定与解绑信息 */
#define HY_DVR_GET_FRONT_DEVICE_OWNNER_INFO  		986
/** @brief 设置前端脸库绑定与解绑信息 */
#define HY_DVR_SET_FRONT_DEVICE_OWNNER_INFO  		987

//对应的结构体(HY_DVR_FRONT_DEVICE_OWNNER_INFO)
#define FRONT_DEVICE_OWNNER_INFO           			1


//主类型
/* 获取人脸识别基准脸库 */
#define HY_DVR_GET_FACE_RECOGNIZE_STANDARD_GROUP     989
/* 设置人脸识别基准脸库 */
#define HY_DVR_SET_FACE_RECOGNIZE_STANDARD_GROUP     990
/* 最大人脸识别基准脸库数 */
#define MAX_FACE_RECOGNIZE_STANDARD_GROUPS           16

//对应的结构体(HY_DVR_FACE_RECOGNIZE_STANDARD_GROUP_LIST)
#define FACE_RECOGNIZE_STANDARD_GROUP            	  1


//主类型
/** @brief 获取设备对讲模式                */
#define HY_DVR_GET_DEVICE_INTERCOM_MODE           	991
/** @brief 设置设备对讲模式                */
#define HY_DVR_SET_DEVICE_INTERCOM_MODE				992

//对应的结构体(HY_DVR_DEVICE_INTERCOM_MODE)
#define DEVICE_INTERCOM_MODE						1


//主类型
/** @brief 获取设备一键呼叫配置(暂未实现)                */
#define HY_DVR_GET_DEVICE_ONE_BUTTON_CALL_CFG           993
/** @brief 设置设备一键呼叫配置                */
#define HY_DVR_SET_DEVICE_ONE_BUTTON_CALL_CFG           994

//对应的结构体(HY_DVR_DEVICE_ONE_BUTTON_CALL_CFG,结构体及命令后缀应该为notify,需求模糊导致命名上与实际功能不相符)
#define DEVICE_ONE_BUTTON_CALL_CFG                      1
//对应的结构体(HY_DVR_DEVICE_ONE_BUTTON_CALL_FUNC_CFG)
#define DEVICE_ONE_BUTTON_CALL_FUNC_CFG                 2


//主类型
/* 获取组织架构树 */
#define HY_DVR_GET_FACE_GROUP_ORGANIZATION_STRUCTURE_TREE  		995
/* 设置组织架构树 */
#define HY_DVR_SET_FACE_GROUP_ORGANIZATION_STRUCTURE_TREE  		996

//对应的结构体(HY_DVR_FACE_GROUP_ORGANIZATION_STRUCTURE_TREE)
#define FACE_GROUP_ORGANIZATION_STRUCTURE_TREE            		1
//对应的结构体(HY_DVR_FACE_GROUP_ORGANIZATION_MEMBER_ADD)
#define FACE_GROUP_ORGANIZATION_MEMBER_ADD           			2
//对应的结构体(HY_DVR_FACE_GROUP_ORGANIZATION_MEMBER_MODIFY)
#define FACE_GROUP_ORGANIZATION_MEMBER_MODIFY            		3
//对应的结构体(HY_DVR_FACE_GROUP_ORGANIZATION_MEMBER_DELETE)
#define FACE_GROUP_ORGANIZATION_MEMBER_DELETE            4
//对应的结构体(HY_DVR_FACE_GROUP_ORGANIZATION_MEMBER_SELECT)
#define FACE_GROUP_ORGANIZATION_MEMBER_SELECT            5


//主命令
//获取大匠智慧社区配置参数
#define HY_DVR_GET_DAJIANG_IOT_CFG_PARAM           				997
//设置大匠智慧社区配置参数
#define HY_DVR_SET_DAJIANG_IOT_CFG_PARAM           				998

//大匠智慧社区配置参数(HY_DVR_DAJIANG_IOT_CFG_PARAM)
#define DAJIANG_IOT_CFG_PARAM                      				1

//主命令
//大匠智慧社区服务器配置(定制)
#define HY_DVR_GET_DAJIANG_IOT_SERVER_CFG_CUSTOM          		999
//大匠智慧社区服务器配置(定制)
#define HY_DVR_SET_DAJIANG_IOT_SERVER_CFG_CUSTOM           		1000

//大匠智慧社区服务器配置(定制)(HY_DVR_DAJIANG_IOT_SERVER_CFG_CUSTOM)
#define DAJIANG_IOT_SERVER_CFG_CUSTOM                      1
//大匠智慧社区服务器连接状态(定制)(HY_DVR_DAJIANG_IOT_SERVER_CONNECT_STATUS)
#define DAJIANG_IOT_SERVER_CONNECT_STATUS                   2

//主命令
//获取老化测试信息
#define HY_DVR_GET_AGINGTEST_MODE_INFO      1001
//设置老化测试信息
#define HY_DVR_SET_AGINGTEST_MODE_INFO      1002

//次命令
#define AGINGTEST_MODE_INFO               1


//主类型
/** @brief 获取产测工具台灯配置	*/
#define HY_DVR_GET_PRODUCT_TOOL_LAMP_CFG                 	1003
/** @brief 设置产测工具台灯配置	*/
#define HY_DVR_SET_PRODUCT_TOOL_LAMP_CFG                 	1004

//次类型
/** @brief 产测工具台灯配置(HY_DVR_PRODUCT_TOOL_LAMP_CFG)	*/
#define PRODUCT_TOOL_LAMP_CFG                             	1


//主类型
/** @brief 获取区域人数统计配置 */
#define HY_DVR_GET_REGIONAL_PEOPLE_STAT						1005
/** @brief 设置区域人数统计配置 */
#define HY_DVR_SET_REGIONAL_PEOPLE_STAT         			1006

//对应的结构体(HY_DVR_CMCC_OVD_REGIONAL_PEOPLE_STATLIST)
#define REGIONAL_PEOPLE_STAT 1


//主类型
/** @brief 通知voip语音播放类型(暂不支持) */
#define HY_DVR_GET_VOIP_VOICE_TYPE            				1007
/** @brief 通知voip语音播放类型 */
#define HY_DVR_SET_VOIP_VOICE_TYPE                 			1008

//对应的结构体(HY_DVR_VOIP_VOICE_TYPE)
#define VOIP_VOICE_TYPE 1


//主类型
/** @brief 消除指定类型告警 */
#define HY_DVR_SET_STOP_ALARM_STATUS                  		1009

//对应的结构体(HY_DVR_OVDSDK_STOP_ALARM)
#define STOP_ALARM_STATUS 1


//主类型
/** @brief 获取人脸识别配置 */
#define HY_DVR_GET_FACE_RECO_CFG							1011
/** @brief 设置人脸识别配置 */
#define HY_DVR_SET_FACE_RECO_CFG							1012

/** @brief 人脸识别使能开关(HY_DVR_CUDOT_FACERECO_STATUS)	*/
#define CUDOT_FACE_RECO_STATUS								1
/** @brief 人脸识别参数(HY_DVR_CUDOT_FACERECO_PARAMS)	*/
#define CUDOT_FACE_RECO_PARAMS								2


//主类型
/** @brief 获取人脸识别信息 */
#define HY_DVR_GET_FACE_RECO_INFO							1013
/** @brief 设置人脸识别信息 */
#define HY_DVR_SET_FACE_RECO_INFO							1014

/** @brief 人脸新增(HY_DVR_CUDOT_FACERECO_INFO)	*/
#define CUDOT_FACE_RECO_ADD									1
/** @brief 人脸删除(HY_DVR_CUDOT_FACERECO_INFO)	*/
#define CUDOT_FACE_RECO_DEL									2
/** @brief 人脸更新(HY_DVR_CUDOT_FACERECO_INFO)	*/
#define CUDOT_FACE_RECO_UPDATE								3


//主类型
/** @brief 获取算法仓库配置 */
#define HY_DVR_GET_ALGORITHM_STORE_CFG   					1015
/** @brief 设置算法仓库配置 */
#define HY_DVR_SET_ALGORITHM_STORE_CFG   					1016

//次类型
/** 算法仓库配置(HY_DVR_ALGORITHM_STORE_CFG) */
#define ALGORITHM_STORE_CFG               					1


//获取流超限处理模式
/** 流超限处理模式(HY_DVR_STREAM_OUT_CFG) */
#define HY_DVR_GET_STREAM_OUT_OF_RANGE                         1017
#define HY_DVR_GET_STREAM_OUT_MODE                             1018

//主类型
/** @brief macHash,防克隆校验信息 */
#define HY_DVR_GET_CMCC_OVD_CHANNEL_MACHASH 					1019
#define HY_DVR_SET_CMCC_OVD_CHANNEL_MACHASH 					1020
// 次类型
/** 防克隆校验信息(HY_DVR_CMCC_OVD_MACHASH) */
#define OVD_DEVICE_CONFIG_MACHASH 								1


//主类型
/** @brief 获取ipc key(捷顺定制) */
#define HY_DVR_GET_IPC_KEY_JIESHUN_CUSTOM   					1021
/** @brief 设置ipc key(捷顺定制)(暂不支持) */
#define HY_DVR_SET_IPC_KEY_JIESHUN_CUSTOM   					1022
//次类型
/** ipc key(捷顺定制 HY_DVR_IPC_KEY_JIESHUN_CUSTOM) */
#define IPC_KEY_JIESHUN_CUSTOM               					1


//主类型
/** @brief 获取算法商场透传配置                                                       */
#define HY_DVR_GET_ALGO_MALL_TRANSPARENT_TRANSMISSION_CFG       1023
/** @brief 设置算法商场透传配置                                                           */
#define HY_DVR_SET_ALGO_MALL_TRANSPARENT_TRANSMISSION_CFG       1024

/** @brief 结构体(HY_DVR_ALGO_MALL_TRANSPARENT_TRANSMISSION_CFG)    */
#define ALGO_MALL_TRANSPARENT_TRANSMISSION_CFG                  1


//主类型
/** @brief 获取算法商场透传(暂不支持)                                                   */
#define HY_DVR_GET_ALGO_CUSTOM            						1025
/** @brief 设置算法商场透传                                                           */
#define HY_DVR_SET_ALGO_CUSTOM            						1026

/** @brief 结构体(HY_DVR_ALGO_CUSTOM)    */
#define ALGO_CUSTOM                        						1


//主命令
/** @brief 获取PTZ联动配置*/
#define HY_DVR_GET_PTZ_LINKAGE_GOTO_CFG           				1029
/** @brief 设置PTZ联动配置 */
#define HY_DVR_SET_PTZ_LINKAGE_GOTO_CFG           				1030

//对应的结构体(HY_DVR_PTZ_LINKAGE_GOTO_CFG)
#define PTZ_LINKAGE_GOTO_CFG                       				1


//主命令
//获取1400配置
#define HY_DVR_GET_CMCC_CHANNEL_1400_CFG           				1031
//设置1400配置
#define HY_DVR_SET_CMCC_CHANNEL_1400_CFG           				1032

//次命令字 HY_DVR_CMCC_1400_CFG
#define HY_DVR_CMCC_CHANNEL_1400_CFG                            1


//主命令
/** @brief 中间层找协议获取TianYiV2设置 */
#define HY_DVR_GET_TIANYIV2_CONFIG_FROM_PROTOCOL				1033
/** @brief 中间层向协议设置TianYiV2设置 */
#define HY_DVR_SET_TIANYIV2_CONFIG_TO_PROTOCOL					1034
//次命令
/** @brief 中间层调用移动二维码解析接口(HY_DVR_TIANYIV2_CONFIG_NETWORK_INFO) */
#define TIANYIV2_CONFIG_NETWORK_QR_INFO	1


//主命令
/** @brief 中间层透传ZWHOST的接口调用 */
#define HY_DVR_GET_ZWHOST_API				1035
/** @brief 中间层透传ZWHOST的接口调用 */
#define HY_DVR_SET_ZWHOST_API				1036
//次命令
/** @brief 为了获取别名添加域名(HY_DVR_ZWHOST_API_ADD_DOMAIN_FOR_GET_ALIAS) */
#define ZWHOST_API_ADD_DOMAIN_FOR_GET_ALIAS		1
/** @brief 添加域名列表到本地(HY_DVR_ZWHOST_API_ADD_DOMAIN_2_LOCAL) */
#define ZWHOST_API_ADD_DOMAIN_LIST_2_LOCAL	    2


//主命令
/** @brief 获取布防时间段 */
#define HY_DVR_GET_DEFENCE_TIME_SCHEDULE_LIST          1037
/** @brief 设置布防时间段 */
#define HY_DVR_SET_DEFENCE_TIME_SCHEDULE_LIST          1038
//次命令
/** @brief 人员通行权限(HY_DVR_DEFENCE_TIME_SCHEDULE_LIST) */
#define DEFENCE_TIME_SCHEDULE_LIST                     1


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取车牌抓拍配置 */
#define HY_DVR_GET_CAR_PLATE_SNAP                         		1039
/** @brief 设置车牌抓拍配置 */
#define HY_DVR_SET_CAR_PLATE_SNAP                          		1040
/** @brief 车牌抓拍配置信息 (HY_DVR_CAR_PLATE_SNAP_CFG) */
#define CAR_PLATE_SNAP_CONFIG        					        1
//车牌抓拍事件时间段配置(HY_DVR_CAR_PLATE_SNAP_EVENT_TIMESLOT_LIST)
#define CAR_PLATE_SNAP_EVENT_TIMESLOT_CFG                       2
//车牌抓拍事件语音报警时间段配置(HY_DVR_CAR_PLATE_SNAP_VOICEALARM_TIMESLOT_LIST)
#define CAR_PLATE_SNAP_EVENT_VOICEALARM_TIMESLOT_CFG            3
///////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取手动回充(暂不支持)                    */
#define HY_DVR_GET_MANUAL_RECHARGE                     1043
/** @brief 设置手动回充                             */
#define HY_DVR_SET_MANUAL_RECHARGE                     1044

//次命令
/** @brief 手动回充(结构体:无)    */
#define MANUAL_RECHARGE                                 1
///////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取灯带控制配置 */
#define HY_DVR_GET_LIGHT_STRIP_CONTROL				1045
/** @brief 设置灯带控制配置*/
#define HY_DVR_SET_LIGHT_STRIP_CONTROL			    1046

//次命令(HY_DVR_LIGHT_STRIP_CONTROL_CONFIG)
#define LIGHT_STRIP_CONTROL							1
///////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取逗宠配置 */
#define HY_DVR_GET_TEASE_PET				1047
/** @brief 设置逗宠配置*/
#define HY_DVR_SET_TEASE_PET			    1048

//次命令
/** @brief 逗宠动作(HY_DVR_TEASE_PET_ACTION_CONFIG) */
#define TEASE_PET_MOTION							1
/** @brief 逗宠声音(HY_DVR_TEASE_PET_ACTION_CONFIG) */
#define TEASE_PET_VOICE							    2
/** @brief 逗宠激光(HY_DVR_TEASE_PET_LASER_CONFIG) */
#define TEASE_PET_LASER							    3
///////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取宠物检测配置 */
#define HY_DVR_GET_PET_DETECT				1049
/** @brief 设置宠物检测配置*/
#define HY_DVR_SET_PET_DETECT			    1050

//次命令(HY_DVR_PET_DETECT_CONFIG)
#define PET_DETECT							1
///////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取云消息推送策略 */
#define HY_DVR_GET_P6SMSG_POLICY				1051
/** @brief 设置宠物检测配置*/
#define HY_DVR_SET_P6SMSG_POLICY			    1052

//次命令(HY_DVR_P6SMSG_POLICY_CONFIG)
#define P6SMSG_POLICY							1
///////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取相关信息（P2P 协议） */
#define HY_DVR_GET_P2P_COMMAND				    1053
/** @brief 设置相关信息（P2P 协议）*/
#define HY_DVR_SET_P2P_COMMAND			        1054

//次命令
#define PPCS_CLOSE_ALL_SESSION                      1
#define PPCS_LISTEN_BREAK							2
#define TRY_DEVICE_SLEEP							3 //尝试设备进入休眠模式
#define PPCS_NEW_LISTEN							4 //重新listenSERVER
#define P2P_SERVER_INFO							5 //设置服务器信息 (HY_DVR_P2P_SERVERINFO)
#define P2P_IS_CONNECTED								6 //是否有P2P连接
#define P2P_UPDATA_WAKEUP_SERVER						7  //p2p更新唤醒服务器
#define P2P_BUFFER_FULL                             8 // 底层缓冲区满
///////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取NVR主动注册配置 */
#define HY_DVR_GET_REGISTER_NVR				    1055
/** @brief 设置NVR主动注册配置*/
#define HY_DVR_SET_REGISTER_NVR			        1056

//次命令(HY_DVR_REGISTER_NVR_INFO)
#define SERVER_INFO                             1
//次命令(HY_DVR_REGISTER_STATE)
#define SERVER_STA_REGISTER_STATE				2
///////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取透传的P6SEvent心跳响应信息(暂未使用) */
#define HY_DVR_GET_P6SEVENT_HEARTBEAT_RESPONSE_TRANSPARENT                       1057
/** @brief 设置透传的P6SEvent心跳响应信息 */
#define HY_DVR_SET_P6SEVENT_HEARTBEAT_RESPONSE_TRANSPARENT                       1058

/** @brief 透传的P6SEvent心跳响应信息 */
#define P6SEVENT_HEARTBEAT_RESPONSE_TRANSPARENT                                   1
///////////////////////////////////////////////////////////////////////////////////////////////////////


//主命令
/** @brief 获取某个功能使能打开后设备是否会重启 */
#define HY_DVR_GET_DEVICE_NEED_REBOOT                                               1059
/** @brief 设置某个功能使能打开后设备是否会重启 */
#define HY_DVR_SET_DEVICE_NEED_REBOOT                                               1060

/** @brief 设备功能是能是否打开(HY_DVR_DEVICE_CAPABILITY) */
#define DEVICE_NEED_REBOOT                                                          1


//主命令
/** @brief 获取设备数字倍数 */
#define HY_DVR_GET_DEVICE_DIGITAL_MULTIPLE                                              1061
/** @brief 设置设备数字倍数 */
#define HY_DVR_SET_DEVICE_DIGITAL_MULTIPLE                                              1062

/** @brief 设备数字倍数(HY_DVR_DEVICE_DIGITAL_MULTIPLE) */
#define DEVICE_DIGITAL_MULTIPLE                                                          1


//主命令
/** @brief 获取 P6SEvent MQTT 配置 */
#define HY_DVR_GET_P6SEVENT_MQTT_CONFIG                                              1063
/** @brief 设置 P6SEvent MQTT 配置 */
#define HY_DVR_SET_P6SEVENT_MQTT_CONFIG                                              1064

/** @brief P6SEvent MQTT 配置(HY_DVR_P6SEVENT_MQTT_CONFIG) */
#define P6SEVENT_MQTT_CONFIG                                                          1


//主命令
/** @brief 获取白光灯使能配置 */
#define HY_DVR_GET_WHITE_LIGHT_ENABLE                                              1065
/** @brief 设置白光灯使能配置 */
#define HY_DVR_SET_WHITE_LIGHT_ENABLE                                              1066

/** @brief 白光灯使能配置(HY_DVR_WHITE_LIGHT_ENABLE) */
#define WHITE_LIGHT_ENABLE                                                          1


//主命令
/** @brief 获取语音播报语言类型配置 */
#define HY_DVR_GET_VOICE_BROADCAST_LANGUAGE                                        1067
/** @brief 设置语音播报语言类型配置 */
#define HY_DVR_SET_VOICE_BROADCAST_LANGUAGE                                        1068

/** @brief 语音播报语言类型配置(HY_DVR_VOICE_BROADCAST_LANGUAGE) */
#define VOICE_BROADCAST_LANGUAGE                                                   1


//主命令
/** @brief 获取是否是 AOV 模式 */
#define HY_DVR_GET_IS_AOV_MODE                                        1069
/** @brief 设置是否是 AOV 模式 */
#define HY_DVR_SET_IS_AOV_MODE                                        1070

/** @brief 语音播报语言类型配置(HY_DVR_IS_AOV_MODE) */
#define AOV_MODE                                                   1


//主命令
/** @brief 获取指示灯配置 */
#define HY_DVR_GET_INDICATOR_LIGHT_CFG                                        1071
/** @brief 设置指示灯配置 */
#define HY_DVR_SET_INDICATOR_LIGHT_CFG                                        1072

/** @brief 指示灯配置(HY_DVR_INDICATOR_LIGHT_CFG) */
#define INDICATOR_LIGHT_CFG                                                   1


//主命令
/** @brief 获取灵敏度配置 */
#define HY_DVR_GET_SENSITIVITY_CFG                                        1073
/** @brief 设置灵敏度配置 */
#define HY_DVR_SET_SENSITIVITY_CFG                                        1074

/** @brief 灵敏度配置(HY_DVR_SENSITIVITY_CFG_LIST) */
#define SENSITIVITY_CFG                                                   1


//主类型
/** @brief 获取布防框配置（禹尊定制-GB28181协议-布防框）                                                            */
#define HY_DVR_GET_DEFENCE_FRAME_GB28181_YUZUN              				1075
/** @brief 设置布防框配置（禹尊定制-GB28181协议-布防框）                                                            */
#define HY_DVR_SET_DEFENCE_FRAME_GB28181_YUZUN              				1076

//次类型
/** @brief 布防框配置（禹尊定制-GB28181协议-布防框）(HY_DVR_DEFENCE_FRAME_GB28181_YUZUN)                                    */
#define DEFENCE_FRAME_GB28181_YUZUN                 						1


//主命令
/** @brief 获取设备激活状态（HoloLink 协议） */
#define HY_DVR_GET_HOLOLINK_ACTIVE_STATUS                                        1077
/** @brief 设置设备激活状态（HoloLink 协议） */
#define HY_DVR_SET_HOLOLINK_ACTIVE_STATUS                                        1078

/** @brief 设备激活状态(HY_DVR_HOLOLINK_ACTIVE_STATUS) */
#define HOLOLINK_ACTIVE_STATUS                                                   1


//主命令
/** @brief 获取蓝牙地锁配置 */
#define HY_DVR_GET_BT_GROUND_LOCK_CFG                                        1079
/** @brief 设置蓝牙地锁配置 */
#define HY_DVR_SET_BT_GROUND_LOCK_CFG                                        1080

/** @brief 蓝牙地锁配置(HY_DVR_BT_BASIC_CFG) */
#define BT_BASIC_CFG                                                     1
/** @brief 蓝牙设备列表(HY_DVR_BT_DEVICE_LIST) */
#define BT_DEVICE_LIST                                                   2
/** @brief 蓝牙地锁控制结果(HY_DVR_BT_GROUND_LOCK_CTRL_STATUS) */
#define BT_GROUND_LOCK_CTRL_STATUS                                       3
/** @brief 蓝牙地锁信息(HY_DVR_BT_DEVICE_INFO) */
#define BT_DEVICE_INFO                                                   4
/** @brief 蓝牙地锁车位信息(HY_DVR_BT_PARKING_SPACE_LIST) */
#define BT_PARKING_SPACE_LIST                                            5


//主命令
/** @brief 获取设备工作模式 */
#define HY_DVR_GET_POWER_MANAGE_CFG                                        1081
/** @brief 设置设备工作模式 */
#define HY_DVR_SET_POWER_MANAGE_CFG                                        1082

/** @brief 设备工作模式(HY_DVR_POWER_MANAGE_CFG) */
#define POWER_MANAGE_CFG                                                   1
/** @brief 设备工作模式V2(HY_DVR_DEVICE_POWER_MANAGE_CFG_V2) */
#define DEVICE_POWER_MANAGE_CFG_V2											2


//主命令
/** @brief 获取设备 SIM 卡信息 */
#define HY_DVR_GET_SIM_CARD_INFO                                        1083
/** @brief 设置设备 SIM 卡信息 */
#define HY_DVR_SET_SIM_CARD_INFO                                        1084

/** @brief 设备 SIM 卡信息(HY_DVR_SIM_CARD_INFO) */
#define SIM_CARD_INFO                                                   1


//主命令
/** @brief 获取设备夜视设置 */
#define HY_DVR_GET_NIGHT_VISION_CFG                                        1085
/** @brief 设置设备夜视设置 */
#define HY_DVR_SET_NIGHT_VISION_CFG                                        1086

/** @brief 设备夜视设置(HY_DVR_NIGHT_VISION_CFG) */
#define NIGHT_VISION_CFG                                                   1


//主命令
/** @brief 获取车位配置 */
#define HY_DVR_GET_PARKING_SPACE_CFG                                        1087
/** @brief 设置车位配置 */
#define HY_DVR_SET_PARKING_SPACE_CFG                                        1088

/** @brief 车位配置(HY_DVR_PARKING_SPACE_CFG) */
#define PARKING_SPACE_CFG                                                   1


//主命令
/** @brief 获取地锁控制设置 */
#define HY_DVR_GET_GROUND_LOCK_CTRL_STATUS                                        1089
/** @brief 设置地锁控制设置 */
#define HY_DVR_SET_GROUND_LOCK_CTRL_STATUS                                        1090

/** @brief 地锁控制配置(HY_DVR_GROUND_LOCK_CTRL_STATUS) */
#define GROUND_LOCK_CTRL_STATUS                                                   1
/** @brief 地锁控制V2操作结果(HY_DVR_GROUND_LOCK_CTRL_V2_OPT_RESULT) */
#define GROUND_LOCK_CTRL_V2_OPT_RESULT                                      	  2


//主命令
/** @brief 获取网络信息 */
#define HY_DVR_GET_NETWORK_INFO                                        1091
/** @brief 设置网络信息 */
#define HY_DVR_SET_NETWORK_INFO                                        1092

/** @brief 网络信息配置(HY_DVR_NETWORK_INFO) */
#define NETWORK_INFO_ALL                                                   1

//主命令
/** @brief 通过p6sMSGPush协议获取配置 */
#define HY_P6SMSGPUSH_GET_CONFIG                                        1093
/** @brief 通过p6sMSGPush协议设置配置 */
#define HY_P6SMSGPUSH_SET_CONFIG                                        1094
/** @brief 通过p6sMSGPush协议上传设备信息到平台（和setconfig接口配套使用，主命令也是set） */
#define HY_SEND_DEVICE_INFO													1


//主命令
/** @brief 获取地锁升降策略配置 */
#define HY_DVR_GET_GROUND_LOCK_LIFT_STRATEGY                                        1095
/** @brief 设置地锁升降策略配置 */
#define HY_DVR_SET_GROUND_LOCK_LIFT_STRATEGY                                        1096

/** @brief 地锁升降策略配置(HY_DVR_GROUND_LOCK_LIFT_STRATEGY) */
#define GROUND_LOCK_LIFT_STRATEGY                                                   1


//主命令
/** @brief 获取勿扰模式配置 */
#define HY_DVR_GET_DO_NOT_DISTURB_MODE_CFG                                        1097
/** @brief 设置勿扰模式配置 */
#define HY_DVR_SET_DO_NOT_DISTURB_MODE_CFG                                        1098

/** @brief 勿扰模式配置(HY_DVR_DO_NOT_DISTURB_MODE_CFG) */
#define DO_NOT_DISTURB_MODE_CFG                                                   1


//主命令
/** @brief 获取设备错误码 */
#define HY_DVR_GET_DEVICE_ERROR_CODE                                        	  1099
/** @brief 设置设备错误码 */
#define HY_DVR_SET_DEVICE_ERROR_CODE                                              1100

/** @brief 4G错误码列表(tagHY_4G_ERROR_CODE_LIST) */
#define HY_4G_ERROR_CODE_LIST_INFO                                                   1


//主命令
/** @brief 获取巡航时间段 */
#define HY_DVR_GET_PTZ_CRUISE_TIME_SLOT                                        	        1101
/** @brief 设置巡航时间段 */
#define HY_DVR_SET_PTZ_CRUISE_TIME_SLOT                                                 1102

/** @brief 巡航时间段配置(HY_DVR_PTZ_CRUISE_TIME_SLOT) */
#define PTZ_CRUISE_TIME_SLOT                                                   1


//主命令
/** @brief 获取准入车辆配置 */
#define HY_DVR_GET_DDLOCK_ACCESS_CAR_CFG                                        1103
/** @brief 设置准入车辆配置 */
#define HY_DVR_SET_DDLOCK_ACCESS_CAR_CFG                                        1104

/** @brief 准入车辆配置(HY_DVR_DDLOCK_ACCESS_CAR_CFG) */
#define DDLOCK_ACCESS_CAR_CFG                                                   1


//主命令
/** @brief 获取鉴权模式配置（丁丁地锁） */
#define HY_DVR_GET_DDLOCK_AUTH_MODE_CFG                                        1105
/** @brief 设置鉴权模式配置（丁丁地锁） */
#define HY_DVR_SET_DDLOCK_AUTH_MODE_CFG                                        1106

/** @brief 准入车辆配置(HY_DVR_DDLOCK_AUTH_MODE_CFG) */
#define DDLOCK_AUTH_MODE_CFG                                                  1


//主命令
/** @brief 查询地锁状态获取配置（丁丁地锁） */
#define HY_DVR_GET_DDLOCK_QUERY_STATUS                                        1107
/** @brief 查询地锁状态设置配置（丁丁地锁） */
#define HY_DVR_SET_DDLOCK_QUERY_STATUS                                        1108

/** @brief 地锁状态配置(HY_DVR_DDLOCK_QUERY_STATUS) */
#define DDLOCK_QUERY_STATUS                                                  1


//主命令
/** @brief 主动抓拍配置（丁丁地锁） */
#define HY_DVR_GET_DDLOCK_ACTIVE_SNAPSHOT_CFG                                 1109
/** @brief 主动抓拍配置（丁丁地锁） */
#define HY_DVR_SET_DDLOCK_ACTIVE_SNAPSHOT_CFG                                 1110

/** @brief 主动抓拍配置(HY_DVR_DDLOCK_ACTIVE_SNAPSHOT_CFG) */
#define DDLOCK_ACTIVE_SNAPSHOT_CFG                                            1


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取车流统计信息 */
#define HY_DVR_GET_TRAFFIC_STATICS                              			1111
/** @brief 设置车流统计信息 */
#define HY_DVR_SET_TRAFFIC_STATICS                              			1112
 
//次命令
/** @brief 车流统计界面设计信息 (HY_DVR_TRAFFIC_STATICS_UI_DESIGN_CFG) */
#define TRAFFIC_STATICS_UI_DESIGN_CFG                        				1
/** @brief 车流统计事件时间段配置(HY_DVR_TRAFFIC_STATICS_UI_DESIGN_EVENT_TIMESLOT_LIST_CFG) */
#define TRAFFIC_STATICS_UI_DESIGN_EVENT_TIMESLOT_LIST_CFG                   2
/** @brief 车流统计事件语音报警时间段配置(HY_DVR_TRAFFIC_STATICS_UI_DESIGN_VOICEALARM_TIMESLOT_LIST_CFG) */
#define TRAFFIC_STATICS_UI_DESIGN_VOICEALARM_TIMESLOT_LIST_CFG              3
///////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////// 
//主命令
/** @brief 获取当日车流统计柱状图 */
#define HY_DVR_GET_TRAFFIC_STATISTICS                           			1113
/** @brief 设置当日车流统计柱状图 */
#define HY_DVR_SET_TRAFFIC_STATISTICS                           			1114
/** @brief 当日车流统计柱状图信息 (HY_DVR_TRAFFIC_STATISTICS) */
#define TRAFFIC_STATISTICS                              					1
///////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////// 
//主命令
/** @brief 获取跟踪时间段 */
#define HY_DVR_GET_PTZ_TRACK_TIME_SLOT                                        1115
/** @brief 设置跟踪时间段 */
#define HY_DVR_SET_PTZ_TRACK_TIME_SLOT                                        1116
 
/** @brief 跟踪时间段配置(HY_DVR_PTZ_TRACK_TIME_SLOT) */
#define PTZ_TRACK_TIME_SLOT                                                   1
///////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////// 
//主命令
/** @brief 获取设备模式 */
#define HY_DVR_GET_DEVICE_MODE                                        1117
/** @brief 设置设备模式 */
#define HY_DVR_SET_DEVICE_MODE                                        1118
 
/** @brief 设备运行模式配置(HY_DVR_DEVICE_RUN_MODE) */
#define RUN_MODE                                                   1
///////////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////////////////// 
//主命令
/** @brief 获取云台状态 */
#define HY_DVR_GET_ONVIF_PTZ_STATUS                                        1119
/** @brief 设置设备模式 */
#define HY_DVR_SET_ONVIF_PTZ_STATUS                                        1120
 
/** @brief 设备运行模式配置(HY_DVR_ONVIF_PTZ_STATUS) */
#define ONVIF_PTZ_STATUS                                                   1
///////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
/** @brief 获取车牌布控 */
#define HY_DVR_GET_LICENSE_PLATE_SURVEILLANCE                               1121
/** @brief 设置车牌布控 */
#define HY_DVR_SET_LICENSE_PLATE_SURVEILLANCE                      			1122
 
/** @brief 车牌布控界面设计信息 (HY_DVR_LICENSE_PLATE_SURVEILLANCE_UI_DESIGN_CFG) */
#define LICENSE_PLATE_SURVEILLANCE_UI_DESIGN_CFG                                1
///////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取车牌库绑定与解绑信息 */
#define HY_DVR_GET_LICENSE_PLATE_OWNNER_INFO  								1123
/** @brief 设置车牌库绑定与解绑信息 */
#define HY_DVR_SET_LICENSE_PLATE_OWNNER_INFO  								1124

//对应的结构体(HY_DVR_LICENSE_PLATE_OWNNER_INFO)
#define LICENSE_PLATE_OWNNER_INFO           								1
///////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 车牌库 */
#define HY_DVR_GET_LICENSE_PLATE_GROUP										1125
/** @brief 车牌库 */
#define HY_DVR_SET_LICENSE_PLATE_GROUP										1126

//对应的结构体(HY_DVR_CREATE_LICENSE_PLATE_GROUP)
#define CREATE_LICENSE_PLATE_GROUP											1
//对应的结构体(HY_DVR_DELETE_LICENSE_PLATE_GROUP)
#define DELETE_LICENSE_PLATE_GROUP											2
//对应的结构体(HY_DVR_MODIFY_LICENSE_PLATE_GROUP)
#define MODIFY_LICENSE_PLATE_GROUP											3
//对应的结构体(HY_DVR_LICENSE_PLATE_GROUP_LIST)
#define QUERYALL_LICENSE_PLATE_GROUP										4
//对应的结构体(HY_DVR_UPDATE_LICENSE_PLATE_INFO_LIST)
#define UPDATE_LICENSE_PLATE_INFO_LIST										5
//对应的结构体(HY_DVR_DELETE_LICENSE_PLATE_INFO_LIST)
#define DELETE_LICENSE_PLATE_INFO_LIST										6
//对应的结构体(HY_DVR_QUERY_LICENSE_PLATE_INFO_LIST)
#define QUERY_LICENSE_PLATE_INFO_LIST										7
//对应的结构体(HY_DVR_ONE_CLICK_DELETE_ALL_LICENSE_PLATE_GROUP_CFG)
#define ONE_CLICK_DELETE_ALL_LICENSE_PLATE_GROUP          					8
///////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
//获取电动车探测配置
#define HY_DVR_GET_EBIKE_DETECT_CFG											1127
//设置电动车探测配置
#define HY_DVR_SET_EBIKE_DETECT_CFG											1128
 
//电动车探测配置对应结构体(HY_DVR_EBIKE_DETECT_CFG)
#define EBIKE_DETECT_CFG													1
///////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
//获取煤气罐探测配置
#define HY_DVR_GET_GAS_TANK_DETECT_CFG										1129
//设置煤气罐探测配置
#define HY_DVR_SET_GAS_TANK_DETECT_CFG										1130
 
//煤气罐探测配置对应结构体(HY_DVR_GAS_TANK_DETECT_CFG)
#define GAS_TANK_DETECT_CFG													1
///////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取设备模块恢复默认配置(暂不支持) */
#define HY_DVR_GET_DEVICE_MODULE_RESET_DEFAULT              				1131
/** @brief 设置设备模块恢复默认配置*/
#define HY_DVR_SET_DEVICE_MODULE_RESET_DEFAULT          					1132
 
//次命令
/** @brief 设备模块恢复默认配置(HY_DVR_DEVICE_MODULE_RESET_DEFAULT) */
#define DEVICE_MODULE_RESET_DEFAULT                  						1
///////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
//获取激活信息
#define HY_DVR_GET_ACTIVE_CFG                                        		1133
//设置激活信息
#define HY_DVR_SET_ACTIVE_CFG                                        		1134
 
//32以上所有功能的激活信息结构体(HY_DVR_ACTIVE_CFG)
#define ACTIVE_CFG                                                          1
///////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
//获取32以上内部激活信息
#define HY_DVR_INTERNAL_GET_ACTIVE_CFG                                        1135
//设置32以上内部激活信息
#define HY_DVR_INTERNAL_SET_ACTIVE_CFG                                        1136
 
//32以上内部功能的激活信息对应结构体(HY_DVR_INTERNAL_ACTIVE_CFG)
#define  INTERNAL_ACTIVE_CFG                                                  1
///////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取RTMPRegister的信息 */
#define HY_DVR_GET_RTMP_REGISTER_INFO           								1137
/** @brief 设置RTMPRegister的信息 */
#define HY_DVR_SET_RTMP_REGISTER_INFO         									1138
 
//次类型
/** @brief RTMPRegister的服务器信息( HY_DVR_RTMP_REGISTER_SERVER_INFO) */
#define  RTMP_REGISTER_SERVER_INFO         										1
/** @brief RTMPRegister的设备状态(HY_DVR_RTMP_REGISTER_DEVICE_STATUS)                                    */
#define  RTMP_REGISTER_DEVICE_STATUS         									2
///////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取Onvif配置信息  */
#define HY_DVR_GET_ONVIF_CFG_INFO												1139
/** @brief 设置Onvif配置信息  */
#define HY_DVR_SET_ONVIF_CFG_INFO												1140
 
//次类型
//次命令 (HY_DVR_ONVIF_CFG)
#define ONVIF_CFG_INFO															1
///////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取外置云台配置信息  */
#define HY_DVR_GET_EXTERNAL_PTZ_CONFIG											1141
/** @brief 设置外置云台配置信息  */
#define HY_DVR_SET_EXTERNAL_PTZ_CONFIG											1142
 
//次类型
//次命令 外置AF机芯云台跟踪配置(HY_DVR_EXTERNAL_PTZ_AF_BOARD_TRACK_CONFIG)
#define EXTERNAL_PTZ_AF_BOARD_TRACK_CONFIG                                      1
///////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取设备特殊模式  */
#define HY_DVR_GET_DEVICE_DIY_MODE_CFG                                          1143
/** @brief 设置设备特殊模式  */
#define HY_DVR_SET_DEVICE_DIY_MODE_CFG                              			1144
  
//次类型
//次命令 设备特殊模式配置(HY_DVR_DEVICE_DIY_MODE_CFG)
#define DEVICE_DIY_MODE_CFG                                      				1
///////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////

//主类型
/** @brief 获取设备3D定位配置(暂不支持)  */
#define HY_DVR_GET_DEVICE_3D_POSITIONING_CFG									1145
/** @brief 设置设备3D定位配置  */
#define HY_DVR_SET_DEVICE_3D_POSITIONING_CFG									1146
   
//次类型
//次命令 设备3D定位配置(HY_DVR_DEVICE_3D_POSITIONING_CFG)
#define DEVICE_3D_POSITIONING_CFG												1
///////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取GB定制信息  */
#define HY_DVR_GET_GB28181_CUSTOM_INFO											1147
/** @brief 设置GB定制信息(暂不支持)  */
#define HY_DVR_SET_GB28181_CUSTOM_INFO											1148
   
//次类型
//次命令 GB定制信息(HY_DVR_GB28181_CUSTOM_INFO)
#define GB28181_CUSTOM_INFO												1
///////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取氛围灯配置 ?*/
#define	HY_DVR_GET_ATMOSPHERE_LIGHT_CFG											1149
/** @brief 设置氛围灯配置 ?*/
#define	HY_DVR_SET_ATMOSPHERE_LIGHT_CFG											1150

//次类型
//次命令 氛围灯配置(HY_DVR_ATMOSPHERE_LIGHT_CFG)
#define	ATMOSPHERE_LIGHT_CFG													1
///////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取贝福佳定制配置                                                               */
#define HY_DVR_GET_BFJ_CUSTOM_CFG												1151
/** @brief 设置贝福佳定制配置                                                               */
#define HY_DVR_SET_BFJ_CUSTOM_CFG												1152

/** @brief 贝福佳密码认证配置信息(HY_DVR_BFJ_PASSWORD_AUTH_CFG) */
#define BFJ_PASSWORD_AUTH_CFG													1
/** @brief 贝福佳安全密码配置信息(HY_DVR_BFJ_SECURE_PASSWORD_CFG) */
#define BFJ_SECURE_PASSWORD_CFG													2
/** @brief 贝福佳WIFI密码配置信息(HY_DVR_BFJ_WIFI_PASSWORD_CFG) */
#define BFJ_WIFI_PASSWORD_CFG         											3
///////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取人数统计信息 */
#define HY_DVR_GET_PEOPLE_STATICS												1153
/** @brief 设置人数统计信息 */
#define HY_DVR_SET_PEOPLE_STATICS												1154
  
//次类型
/** @brief 人数统计界面设计信息 (HY_DVR_PEOPLE_STATICS_UI_DESIGN_CFG) */
#define PEOPLE_STATICS_UI_DESIGN_CFG											1
///////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取当日人数统计柱状图 */
#define HY_DVR_GET_PEOPLE_STATISTICS                           					1155
/** @brief 设置当日人数统计柱状图 */
#define HY_DVR_SET_PEOPLE_STATISTICS                           					1156

//次类型
/** @brief 当日人数统计柱状图信息 (HY_DVR_PEOPLE_STATISTICS) */
#define PEOPLE_STATISTICS                              							1
///////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取P6SCMS平台的配置 */
#define HY_DVR_GET_P6SCMS_CONFIG												1157
/** @brief 设置P6SCMS平台的配置 */
#define HY_DVR_SET_P6SCMS_CONFIG												1158

//次类型
/** @brief P6SCMS平台的配置 (HY_DVR_P6SCMS_CONFIG) */
#define P6SCMS_CONFIG															1
/** @brief P6SCMS平台连接测试 (HY_DVR_P6SCMS_SERVER_CONNECT_TEST) */
#define P6SCMS_SERVER_CONNECT_TEST												2
/** @brief P6SCMS平台的连接状态(只修改状态) (HY_DVR_P6SCMS_CONFIG) */
#define P6SCMS_CONNECT_STATUS													3
///////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取灯具控制配置                                                           */
#define HY_DVR_GET_LIGHT_CONTROL_CFG                 							1159
/** @brief 设置灯具控制配置                                                           */
#define HY_DVR_SET_LIGHT_CONTROL_CFG                 							1160

//次类型
/** @brief 灯具控制配置(HY_DVR_LIGHT_CONTROL_CFG)                                     */
#define LIGHT_CONTROL_CFG                            							1
///////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取HTTPS认证 */
#define HY_DVR_GET_HTTPS_CFG													1161
/** @brief 设置HTTPS认证 */
#define HY_DVR_SET_HTTPS_CFG													1162

// HTTPS配置(HY_DVR_HTTPS_CFG)
#define HTTPS_CFG																1
// HTTPS证书信息(HY_DVR_HTTPS_CERTIFICATION_INFO)
#define HTTPS_CERTIFICATION_INFO												2
// HTTPS认证配置(HY_DVR_HTTPS_CERTIFICATE_CREATE_CFG)
#define HTTPS_CERTIFICATE_CREATE												3
// HTTPS认证证书删除(HY_DVR_HTTPS_CERTIFICATE_DELETE_CFG)
#define HTTPS_CERTIFICATE_DELETE												4
// RTSPs配置(HY_DVR_RTSPS_CFG)
#define RTSPS_CFG                                            					5
// 证书信息(HY_DVR_CERTIFICATION_CFG)
#define CERTIFICATION_CFG                                   					6
///////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
/** @brief 获取人形居中配置 */
#define HY_DVR_GET_PERSON_CENTER_CFG 											1163
/** @brief 设置人形居中配置 */
#define HY_DVR_SET_PERSON_CENTER_CFG											1164

// 人形居中配置(HY_DVR_PERSON_CENTER_CFG)
#define PERSON_CENTER_CFG														1
///////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主类型
/** @brief 获取屏幕管理配置参数                                                           */
#define HY_DVR_GET_SCREEN_MANAGEMENT_CFG										1165
/** @brief 设置屏幕管理配置参数                                                           */
#define HY_DVR_SET_SCREEN_MANAGEMENT_CFG										1166

//次类型
/** @brief  屏幕管理配置(HY_DVR_SCREEN_MANAGEMENT_CFG) */
#define SCREEN_MANAGEMENT_CFG													1
///////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取定时提醒参数 */
#define HY_DVR_GET_TIME_REMIND                           						1167
/** @brief 设置定时提醒参数 */
#define HY_DVR_SET_TIME_REMIND                           						1168

//次类型
/** @brief  查询定时提醒配置(HY_DVR_TIME_REMIND_CFG) */
#define QUERY_TIME_REMIND_CFG													1
/** @brief  添加定时提醒配置(HY_DVR_TIME_REMIND_CFG) */
#define ADD_TIME_REMIND_CFG														2
/** @brief  删除定时提醒配置(HY_DVR_TIME_REMIND_CFG) */
#define DELETE_TIME_REMIND_CFG													3
/** @brief  修改定时提醒配置(HY_DVR_TIME_REMIND_CFG) */
#define MODIFY_TIME_REMIND_CFG													4
/** @brief  添加定时提醒语音信息(HY_DVR_TIME_REMIND_AUDIO_INFO_LIST) */
#define ADD_TIME_REMIND_AUDIO_INFO												5
/** @brief  删除定时提醒语音信息(HY_DVR_TIME_REMIND_AUDIO_INFO_LIST) */
#define DELETE_TIME_REMIND_AUDIO_INFO											6
/** @brief  查询定时提醒语音信息(HY_DVR_TIME_REMIND_AUDIO_INFO_LIST) */
#define QUERY_TIME_REMIND_AUDIO_INFO											7
/** @brief  播放定时提醒语音(HY_DVR_TIME_REMIND_AUDIO_PLAY_INFO) */
#define PLAY_TIME_REMIND_AUDIO													8
///////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////
//主命令
/** @brief 获取tcprelay信息 */
#define HY_DVR_GET_TCPRELAY_INFO                           						1169
/** @brief 设置tcprelay信息 */
#define HY_DVR_SET_TCPRELAY_INFO                           						1170

//次类型
/** @brief  TCPRELAY使能(HY_DVR_TCPRELAY_ENABLE) */
#define TCPRELAY_ENABLE													1
///////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct tagHY_DVR_STREAM_OUT_CFG
{
    int nValid;
    int mode;
    char reserve[128];
}HY_DVR_STREAM_OUT_CFG;

typedef struct tagHY_DVR_CMDOT_PATH
{
	int nValid;
	char cConfigPath[256];
	char cLogoPath[256];
	char cLogPath[256];
	int nMaxLogLine;	//日志最大行数（0的话默认10000行，最小1000行）
	int nLogLevel;		//日志级别，参考DoT_LogLevel_en
}HY_DVR_CMDOT_PATH;

#define CM_SN_LEN (16)
#define CM_BINDCODE_LEN (8)
#define CM_PASSWORD_LEN (8)
typedef struct tagHY_DVR_CMDOT_DEVICE_INFO
{
	int bValid;
	// 移动设备序列号,16个字节,ex:7943690000000001 
	char szSn[CM_SN_LEN + 1];
	// 平台接入码,8字节 ex:1C94piOs
	char szBindCode[CM_BINDCODE_LEN + 1];
	// 密码,8字节 ex:1C94piOs
	char szPassword[CM_PASSWORD_LEN + 1];
	//设备名称
	char szModel[64];
}HY_DVR_CMDOT_DEVICE_INFO;

typedef struct tagHY_DVR_CMDOT_DEVICE_INFO_EX
{
	int bValid;
	//每一个通道支持的码流个数
	int nStreamsPerChannel;
	//每一个码流最大buffer,最大不能唱过5M
	int nBufferSizePerStream;
	//设备类型，参考DoT_DeviceType_en枚举
	int nDeviceType;
	//支持的设备分辨率，参考枚举DoT_Resolution_en，使用掩码的方式支持多种
	int nSupportResolution;
	//最大关键帧大小，不能超过800 * 1024
	int nMaxIFrameSize;
	//绑定模式，参考枚举DoT_BindMode_en，使用掩码的方式支持多种
	int nBindMode;
	//事件触发停止的超时时间
	int m_nEventStopTimeout;
	//事件模式，目前固定为0
	int nEventMode;
	//流数据缓存目录路径，确保一直可写，不设置则默认为tmp路径, 不能取外部存储(如TFCard)设备路径，因为无法保证一直可写，可能会造成云存异常，建议采用/tmp路径
	char szStreamDataPath[256];
	//流数据缓存目录路径预留空间大小（字节），最小4MByte，小于4M默认取4M，为了提高云录像性能，建议在空间允许的情况下取更大值
	int nStreamPathSpace;
	int nEventMaxLastTime;
	char cReserve[256];
}HY_DVR_CMDOT_DEVICE_INFO_EX;

typedef struct tagHY_DVR_CMDOT_SYSTEM_PRODUCT_INFO
{
	int bValid;
	char strHjzhSdkVersion[128];    /**< 和家智话SDK版本号（可为空串）*/
	char strAndlinkSdkVersion[128]; /**< Andlink SDK版本号（可为空串）*/
	int nMemorySize;                              /**< 设备运行内存大小 单位KB */
	int nFlashSize;                               /**< 闪存大小 单位KB */
	char strChipName[128];             /**< 芯片名称 */
	char strProductor[128];                /**< 厂商名称 */	
	char cReserve[1024];
}HY_DVR_CMDOT_SYSTEM_PRODUCT_INFO;

typedef struct tagHY_DVR_CMDOT_STORAGE_INFO
{
	int bValid;
	//设备存储类型，参考枚举DoT_StorageType_en
	int nStorageType;
	//存储设备状态，参考枚举DoT_StorageStatus_en
	int nStorageStatus;
	//存储路径
	char cStoragePath[256];
}HY_DVR_CMDOT_STORAGE_INFO;

typedef struct tagHY_DVR_CMDOT_SPEAKER_INFO
{
	int bValid;
	int nVolume;		//0-100
	char cReserve[64];
}HY_DVR_CMDOT_SPEAKER_INFO, *LPHY_DVR_CMDOT_SPEAKER_INFO;

typedef struct tagHY_DVR_CMDOT_BIND_STATUS
{
	int bValid;
	int nBindStatus;		//0,未绑定，1，绑定
	char cReserve[64];
}HY_DVR_CMDOT_BIND_STATUS, *LPHY_DVR_CMDOT_BIND_STATUS;

typedef struct tagHY_DVR_CMDOT_AUTH_INFO
{
	int nValid;

	int nAuthStatus;		//授权状态

	//旷世
	char szSerialNumber[32];	//序列号

	//CMDOT平台
	char szCMDotSn[CM_SN_LEN + 1];// 移动设备序列号,16个字节,ex:7943690000000001 
	char szBindCode[CM_BINDCODE_LEN + 1];// 平台接入码,8字节 ex:1C94piOs
	char szPassword[CM_PASSWORD_LEN + 1];// 密码,8字节 ex:1C94piOs

	//DM平台
	char szCMDotCMEI_IMEI[64]; 	//CMEI/IMEI,和IMEI2选一个填,参考规范文档;设备没有的话向移动申请； ex:111041730000001

	char szReserved[256];
}HY_DVR_CMDOT_AUTH_INFO, *LPHY_DVR_CMDOT_AUTH_INFO;

typedef struct tagHY_DVR_CMDOT_OSD_LOGO_ENABLE
{
	int bValid;
	int nEnable;
}HY_DVR_CMDOT_OSD_LOGO_ENABLE, *LPHY_DVR_CMDOT_OSD_LOGO_ENABLE;


typedef struct tagHY_DVR_CMDOT_OSD_LOCATION
{
	int imageanchor_x;            ///< 店铺名称水印的位置锚点x坐标（x取值0-10000），此参数暂时不需要处理
	int imageanchor_y;            ///< 店铺名称水印的位置锚点y坐标（y取值0-10000），此参数暂时不需要处理
	int location_x;               ///< 店铺名称水印的位置左上角x坐标 (x取值0-10000）
	int location_y;               ///< 店铺名称水印的位置左上角y坐标 (x取值0-10000）
}HY_DVR_CMDOT_OSD_LOCATION;

typedef struct tagHY_DVR_CMDOT_COLOR
{
	int red;                      ///< RGB 红色，范围0~255
	int green;                    ///< RGB 绿色，范围0~255
	int blue;                     ///< RGB 蓝色，范围0~255
}HY_DVR_CMDOT_COLOR;

typedef struct tagHY_DVR_CMDOT_OSD_NAME_INFO
{
	char name[200];               //水印文字字符串，GBK编码
	int font_size;                //字体大小，范围 0~10000  【10000表示字符高度等于视频分辨率高度】
	int display;                  //是否显示，0：不显示  1：显示
	int id;                       //编号
	HY_DVR_CMDOT_COLOR color;     //水印颜色
	HY_DVR_CMDOT_OSD_LOCATION pos; //水印位置
}HY_DVR_CMDOT_OSD_NAME_INFO;

typedef struct tagHY_DVR_CMDOT_OSD_NAME_LIST
{
	int bValid;
	HY_DVR_CMDOT_OSD_NAME_INFO* osd_list;  //水印列表
	int count;                           // 水印列表条目个数
}HY_DVR_CMDOT_OSD_NAME_LIST, *LPHY_DVR_CMDOT_OSD_NAME_LIST;

typedef struct tagHY_DVR_CMDOT_MTU_INFO
{
	int bValid;
	int nMtuSize; // Mtu大小
}HY_DVR_CMDOT_MTU_INFO, *LPHY_DVR_CMDOT_MTU_INFO;

/** 区域坐标信息，坐标取值范围统一为0~10000，设备做归一化处理。摄像头视频区域左上角坐标为（0,0），右下角坐标为（10000,10000）*/
typedef struct tagHY_DVR_CMDOT_REGION_ID
{
	int id;                       /**< 编号 */
	int x;                        /**< x坐标 */
	int y;                        /**< y坐标 */
}HY_DVR_CMDOT_REGION_ID;

/** 告警入侵区域以归一化的基准区域为参考，所选择的区域坐标信息列表 */
typedef struct tagHY_DVR_CMDOT_HACK_REGION_ID
{
	int num;                            /**< 区域坐标个数 */
	HY_DVR_CMDOT_REGION_ID *region_id;  /**< 坐标值 */
}HY_DVR_CMDOT_HACK_REGION_ID, *LPHY_DVR_CMDOT_HACK_REGION_ID;

typedef struct tagHY_DVR_CMDOT_HACK_STAUTS
{
	int bValid;
	int nEnable;      // 告警入侵开关
}HY_DVR_CMDOT_HACK_STAUTS, *LPHY_DVR_CMDOT_HACK_STAUTS;

typedef struct tagHY_DVR_CMDOT_HACK_SENSITIVITY
{
	int bValid;
	int nSensitivity; // 告警入侵灵敏度
}HY_DVR_CMDOT_HACK_SENSITIVITY, *LPHY_DVR_CMDOT_HACK_SENSITIVITY;



typedef struct tagHY_DVR_CMDOT_FACE_DETECT_BASIC_INFO
{
	int bValid;
	int nDetectEnable;      // 人脸检测开关
	int nDetectDistance;    // 人脸检测瞳距
	int nExposureEnable;    // 人脸检测曝光
	int nLuminance;         // 人脸检测亮度

}HY_DVR_CMDOT_FACE_DETECT_BASIC_INFO, *LPHY_DVR_CMDOT_FACE_DETECT_BASIC_INFO;

/** 矩形区域坐标信息，坐标取值范围统一为0~10000，设备做归一化处理。摄像头视频区域左上角坐标为（0,0），右下角坐标为（10000,10000）*/
typedef struct tagHY_DVR_CMDOT_FACE_DETECT_REGION
{
	int nUpperLeftX;                       /**< 矩形左上角x坐标 */
	int nUpperLeftY;                       /**< 矩形左上角y坐标 */
	int nLowerRightX;                      /**< 矩形右下角x坐标 */
	int nLowerRightY;                      /**< 矩形右下角y坐标 */
}HY_DVR_CMDOT_FACE_DETECT_REGION;

typedef struct tagHY_DVR_CMDOT_FACE_CAPTURE_MODE
{
	int bValid;
	int nFaceCaptureMode; //0:离开后抓拍(距离选优),1:快速抓拍,2:间隔抓拍(以秒为单位),3:间隔抓拍(以帧为单位),4:单人模式,5:离开后抓拍(质量选优),6:快速+离开后抓拍
}HY_DVR_CMDOT_FACE_CAPTURE_MODE, *LPHY_DVR_CMDOT_FACE_CAPTURE_MODE;

typedef struct tagHY_DVR_CMDOT_FACE_CAPTURE_QUALITY
{
	int bValid;
	int nFaceCaptureQuality; //人脸抓拍质量 范围：0~100
}HY_DVR_CMDOT_FACE_CAPTURE_QUALITY, *LPHY_DVR_CMDOT_FACE_CAPTURE_QUALITY;


/** 以归一化的基准区域为参考，所选择的矩形区域坐标信息列表 */
typedef struct tagHY_DVR_CMDOT_FACE_DETECT_REGION_LIST
{
	int nRegionsNums;                              /**< 侦测区域个数 */
	HY_DVR_CMDOT_FACE_DETECT_REGION *pRegionList;  /**< 侦测区域坐标信息列表，包含每个检测区域的坐标 */
}HY_DVR_CMDOT_FACE_DETECT_REGION_LIST, *LPHY_DVR_CMDOT_FACE_DETECT_REGION_LIST;

typedef struct tagHY_DVR_CMDOT_FACE_DETECT_PARAM
{
	int nSensitivity;                   /**< 人脸检测灵敏度，默认3，范围1~5 */
	int nMinWidth;                      /**< 人脸检测最小像素宽，默认为-1，表示无限制 */
	int nMinHeight;                     /**< 人脸检测最小像素高，默认为-1，表示无限制 */
	int nMaxWidth;                      /**< 人脸检测最大像素宽，默认为-1，表示无限制 */
	int nMaxHeight;                     /**< 人脸检测最大像素高，默认为-1，表示无限制 */
}HY_DVR_CMDOT_FACE_DETECT_PARAM, *LPHY_DVR_CMDOT_FACE_DETECT_PARAM;

typedef struct tagHY_DVR_CMDOT_VIDEO_RESOLUTION_LEVEL
{
	int bValid;
	int nResolutionLevel; //清晰度级别，0-标清 1-高清 2-超高清
}HY_DVR_CMDOT_VIDEO_RESOLUTION_LEVEL, *LPHY_DVR_CMDOT_VIDEO_RESOLUTION_LEVEL;

typedef struct tagHY_DVR_CMDOT_SPEAKER_PART
{
	int nStatus;                       // 状态，0-离线 1-在线
	int nNeedStop;                     // 是否需要停止 0-不关闭，1-关闭
	char szStopReason[32];             // 停止原因
}HY_DVR_CMDOT_SPEAKER_PART;

typedef struct tagHY_DVR_CMDOT_SPEAKER_STATUS_PRIORITY
{
	int bValid;
	int nPlayKindStatus;                          // 播放种类 0-无播放 1-播放VOIP 2-播放语音对讲 3-播放声光报警 4-播放梯控报警 5-播放机动车报警
	int nPlayRequestType;                         // 播放请求类型 0-无播放 1-播放VOIPCALL 2-播放VOIP广播 3-播放语音对讲 4-播放声光报警 5-播放梯控报警 6-播放机动车报警
	HY_DVR_CMDOT_SPEAKER_PART stVoipCall;         // VOIP电话
	HY_DVR_CMDOT_SPEAKER_PART stVoipBroadCast;    // VOIP云广播
	HY_DVR_CMDOT_SPEAKER_PART stSpeech;           // 语音对讲
	HY_DVR_CMDOT_SPEAKER_PART stSoundLight;       // 声光报警/警戒
	HY_DVR_CMDOT_SPEAKER_PART stEbike;            // 梯控报警/电动车
	HY_DVR_CMDOT_SPEAKER_PART stMotorVehicle;     // 机动车报警
}HY_DVR_CMDOT_SPEAKER_STATUS_PRIORITY, *LPHY_DVR_CMDOT_SPEAKER_STATUS_PRIORITY;

typedef struct tagHY_DVR_CMDOT_SPEAKER_STATUS_PRIORITY_V2
{
	int bValid;
	int nPlayKindStatus;                          // 播放种类 0-无播放 1-播放VOIP 2-播放语音对讲 3-播放声光报警 4-播放梯控报警 5-播放机动车报警 6-播报未佩戴口罩报警 7-播报区域人数越界报警 8-播报越界侦测报警
	int nPlayRequestType;                         // 播放请求类型 0-无播放 1-播放VOIPCALL 2-播放VOIP广播 3-播放语音对讲 4-播放声光报警 5-播放梯控报警 6-播放机动车报警 7-播报未佩戴口罩报警 8-播报区域人数越界报警 9-播报越界侦测报警
	HY_DVR_CMDOT_SPEAKER_PART stVoipCall;         // VOIP电话
	HY_DVR_CMDOT_SPEAKER_PART stVoipBroadCast;    // VOIP云广播
	HY_DVR_CMDOT_SPEAKER_PART stSpeech;           // 语音对讲
	HY_DVR_CMDOT_SPEAKER_PART stSoundLight;       // 声光报警/警戒
	HY_DVR_CMDOT_SPEAKER_PART stEbike;            // 梯控报警/电动车
	HY_DVR_CMDOT_SPEAKER_PART stMotorVehicle;     // 机动车报警
	HY_DVR_CMDOT_SPEAKER_PART stUnMask;           // 未佩戴口罩报警
	HY_DVR_CMDOT_SPEAKER_PART stRegPeople;        //区域人数越界报警
	HY_DVR_CMDOT_SPEAKER_PART stTransgress;       //越界侦测报警
	HY_DVR_CMDOT_SPEAKER_PART stReserve[14];
	char cReserve[256];				
}HY_DVR_CMDOT_SPEAKER_STATUS_PRIORITY_V2, *LPHY_DVR_CMDOT_SPEAKER_STATUS_PRIORITY_V2;


typedef struct tagHY_DVR_CMDOT_SOUND_DETECT_STAUTS
{
	int bValid;
	int nEnable;      // 声音侦测开关
}HY_DVR_CMDOT_SOUND_DETECT_STAUTS, *LPHY_DVR_CMDOT_SOUND_DETECT_STAUTS;

typedef struct tagHY_DVR_CMDOT_SOUND_DETECT_SENSITIVITY
{
	int bValid;
	int nSensitivity; // 声音侦测灵敏度
}HY_DVR_CMDOT_SOUND_DETECT_SENSITIVITY, *LPHY_DVR_CMDOT_SOUND_DETECT_SENSITIVITY;

typedef struct tagHY_DVR_CMDOT_PEOPLE_DRAW_BOX
{
	int bValid;
	int nPepoleDrawBox; //画人形侦测框使能
}HY_DVR_CMDOT_PEOPLE_DRAW_BOX, *LPHY_DVR_CMDOT_PEOPLE_DRAW_BOX;

typedef struct tagHY_DVR_CMDOT_SPEAKER_BUFFER_CLEAR
{
	int bValid;
	int nSpeakerBufferClear; //清空喇叭缓冲区
}HY_DVR_CMDOT_SPEAKER_BUFFER_CLEAR, *LPHY_DVR_CMDOT_SPEAKER_BUFFER_CLEAR;

typedef struct tagHY_DVR_CMDOT_VIDEO_ENCODE
{
	int bValid;
	int nVideoEncode; //码流编码参数 1-H264 2-H265
}HY_DVR_CMDOT_VIDEO_ENCODE, *LPHY_DVR_CMDOT_VIDEO_ENCODE;

typedef struct tagHY_DVR_CMDOT_DEVICE_LOAD
{
	int bValid;
	int nWifiSignal; //WIFI信号强度 db值
	int nCpuUsage;   //CPU负载百分比
	int nMemUsage;   //内存使用率百分比
	int nMemLeft;    //剩余内存（KB）
	int nPacketLostRate; //wan口丢包率  (rx_dropped+tx_dropped)/(rx+tx)
}HY_DVR_CMDOT_DEVICE_LOAD, *LPHY_DVR_CMDOT_DEVICE_LOAD;

typedef struct tagHY_DVR_CMDOT_EBIKE_DETECT_ENABLE
{
	int bValid;
	int nEbikeDetectEnable; // 梯控报警检测开关， 0：关闭 1：打开
}HY_DVR_CMDOT_EBIKE_DETECT_ENABLE, *LPHY_DVR_CMDOT_EBIKE_DETECT_ENABLE;

typedef struct tagHY_DVR_CMDOT_EBIKE_DETECT_SENSITIVITY
{
	int bValid;
	int nEbikeDetectSensitivity; // 设置梯控报警灵敏度 （范围0~100）
}HY_DVR_CMDOT_EBIKE_DETECT_SENSITIVITY, *LPHY_DVR_CMDOT_EBIKE_DETECT_SENSITIVITY;

typedef struct tagHY_DVR_CMDOT_EBIKE_DETECT_TRIGGER
{
	int bValid;
	int nEbikeDetectTrigger; // 设置梯控报警触发条件，取值范围：1：电动车  2：自行车 ，支持按位或的操作（3表示两者都可以触发报警）
}HY_DVR_CMDOT_EBIKE_DETECT_TRIGGER, *LPHY_DVR_CMDOT_EBIKE_DETECT_TRIGGER;

typedef struct tagHY_DVR_CMDOT_1400_PROTOCOL_STATUS
{
	int bValid;
	int n1400ProtocolStatus; // 设置1400协议开关 1-开，非1-关
}HY_DVR_CMDOT_1400_PROTOCOL_STATUS, *LPHY_DVR_CMDOT_1400_PROTOCOL_STATUS;

typedef struct tagHY_DVR_CMDOT_FACE_MASK_PROTOCOL_STATUS
{
	int bValid;
	int nFaceMaskStatus; // 设置口罩检测开关 1-开，非1-关
}HY_DVR_CMDOT_FACE_MASK_PROTOCOL_STATUS, *LPHY_DVR_CMDOT_FACE_MASK_PROTOCOL_STATUS;

typedef struct tagHY_DVR_CMDOT_EXIT_FLAG_PROTOCOL_STATUS
{
	int bValid;
	int nExitFlagStatus; // 出入口 0-入口，非0-出口
}HY_DVR_CMDOT_EXIT_FLAG_PROTOCOL_STATUS, *LPHY_DVR_CMDOT_EXIT_FLAG_PROTOCOL_STATUS;

typedef struct tagHY_DVR_CMDOT_EXIT_FLAG_PROTOCOL_EVN
{
	int bValid;
	int nExitFlagStatus; // 0：关闭出口标记 1：打开出口标记
	int nEnv;        // 0:道路  1：岗亭 （nEnv为0的时候，nExitFlagStatus必须设置为0）,  -1表示无效值
	char szReserved[64];
}HY_DVR_CMDOT_EXIT_FLAG_PROTOCOL_EVN, *LPHY_DVR_CMDOT_EXIT_FLAG_PROTOCOL_EVN;

typedef struct tagHY_DVR_CMDOT_FALLING_OBJECT_DETECT_STATUS
{
	int bValid;
	int nFallingStatus; //设置高空抛物的开关 1-开，非1-关
}HY_DVR_CMDOT_FALLING_OBJECT_DETECT_STATUS, *LPHY_DVR_CMDOT_FALLING_OBJECT_DETECT_STATUS;

typedef struct tagHY_DVR_CMDOT_FALLING_OBJECT_DETECT_SENSITIVITY
{
	int bValid;
	int nFallingSensitivity; //设置高空抛物的灵敏度 1-5
}HY_DVR_CMDOT_FALLING_OBJECT_DETECT_SENSITIVITY, *LPHY_DVR_CMDOT_FALLING_OBJECT_DETECT_SENSITIVITY;

typedef struct tagHY_DVR_CMDOT_FALLING_OBJECT_CONFIDENCE_LEVEL
{
	int bValid;
	int nFallingConfidenceLevel; //设置高空抛物的置信度 1 - 100
}HY_DVR_CMDOT_FALLING_OBJECT_CONFIDENCE_LEVEL, *LPHY_DVR_CMDOT_FALLING_OBJECT_CONFIDENCE_LEVEL;

//!\brief 人脸扣图数据
typedef struct tagHY_DVR_CMDOT_SNAP_FACE_JPG_INFO
{
	int nValid;
	char* pPicBuff;   
	int nPicLen;
	int nWidth;							/*!< 图片宽度								*/
	int nHeight;						/*!< 图片高度								*/
	int nUpperLeftX;                    //左上X坐标
	int nUpperLeftY;                    //左上Y坐标
	int nLowerRightX;                  //右下X坐标
	int nLowerRightY;                   //右下Y坐标
	char szReserved[128];
}HY_DVR_CMDOT_SNAP_FACE_JPG_INFO;

//!\brief 人脸背景图数据
typedef struct tagHY_DVR_CMDOT_FACE_BACK_JPG_INFO
{
	int nValid;
	char* pPicBuff;    
	int nPicLen;
	int nWidth;							/*!< 图片宽度								*/
	int nHeight;						/*!< 图片高度								*/
	char szReserved[128];
}HY_DVR_CMDOT_FACE_BACK_JPG_INFO;

typedef struct tagHY_DVR_CMDOT_FACE_ATTR_INFO
{
	float				cYaw;							/*!< 侧向角度								*/
	float				cRoll;							/*!< 俯仰角度								*/
	float				cPitch;							/*!< 斜脸角度								*/
	unsigned char		cAge;							/*!< 年龄：0-未知，其它表示年龄				*/
	unsigned char		cSex;							/*!< 性别：0-未知，1-男性，2-女性			*/
	unsigned char		cHat;							/*!< 帽子：0-未知，1-戴帽子，2-无帽子		*/
	unsigned char		cHair;							/*!< 头发长度：0-未知，1-短发，2-普通，3-长发*/
	unsigned char		cHairColor;						/*!< 头发颜色：0-未知，1-黑色，2-白色，3-金色，4-其它*/
	unsigned char		cEye;							/*!< 眼睛：0-未知，1-正常，2-闭眼，3-遮挡	*/
	unsigned char		cGlasses;						/*!< 眼镜：0-未知，1-戴眼镜，2-无眼镜		*/
	unsigned char		cBeard;							/*!< 胡须：0-未知，1-有胡须，2-无胡须		*/
	unsigned char		cMask;							/*!< 口罩：0-未知，1-有口罩，2-无口罩		*/
	unsigned char		cSkin;							/*!< 肤色：0-未知，1-黄色，2-白色，3-黑色，4-棕色*/
	unsigned char		cSmile;							/*!< 表情：0-未知，1-普通，2-微笑			*/
	unsigned char		cHuman;							/*!< 活体：0-未知，1-活体，2-假体			*/
	unsigned char		cQuality;						/*!< 清晰度：值域范围（0-100）				*/
	char szReserved[128-1];
}HY_DVR_CMDOT_FACE_ATTR_INFO;

typedef struct tagHY_DVR_CMDOT_FACE_RECO_INFO
{
	int nValid;
	unsigned int nTimeSec;						/*!< 绝对时间戳（单位：秒）					*/
	unsigned int nTimeUSec;						/*!< 绝对时间戳（单位：微秒）				*/
	HY_DVR_CMDOT_FACE_ATTR_INFO stFaceAttr;//人脸信息属性
	HY_DVR_CMDOT_SNAP_FACE_JPG_INFO stFaceSnap;//扣图信息
	HY_DVR_CMDOT_FACE_BACK_JPG_INFO stFaceBack;//背景图信息
	char szReserved[256];
}HY_DVR_CMDOT_FACE_RECO_INFO;

#define HY_DVR_CMDOT_MAX_FALLING_OBJECT_DETECT_REGION  (1)
#define HY_DVR_CMDOT_MAX_FALLING_OBJECT_IGNORE_REGION  (10)
#define HY_DVR_CMDOT_MAX_FALLING_OBJECT_REGION_POINT   (6)
#define HY_DVR_CMDOT_MAX_MOTOR_VEHICLE_OBJECT_DETECT_REGION  (4)
#define HY_DVR_CMDOT_MAX_MOTOR_VEHICLE_OBJECT_REGION_POINT   (8)

typedef struct tagHY_DVR_CMDOT_SET_AI_REGION_POINT
{
	int id;                       /**< 编号 */
	int x;                        /**< x坐标 */
	int y;                        /**< y坐标 */
	char szReserved[16];
}HY_DVR_CMDOT_SET_AI_REGION_POINT;

typedef struct tagHY_DVR_CMDOT_FALLING_OBJECT_REGION
{
	int nVaild;
	int nPointCount;
	HY_DVR_CMDOT_SET_AI_REGION_POINT struRegionPoint[HY_DVR_CMDOT_MAX_FALLING_OBJECT_REGION_POINT];
	char szReserved[32];	
}HY_DVR_CMDOT_FALLING_OBJECT_REGION, *LPHY_DVR_CMDOT_FALLING_OBJECT_REGION;

typedef struct tagHY_DVR_CMDOT_FALLING_OBJECT_DETECT_LINE
{
	int nVaild;
	HY_DVR_CMDOT_SET_AI_REGION_POINT struLeft;
	HY_DVR_CMDOT_SET_AI_REGION_POINT struRight;
	char szReserved[128];
}HY_DVR_CMDOT_FALLING_OBJECT_DETECT_LINE, *LPHY_DVR_CMDOT_FALLING_OBJECT_DETECT_LINE;

typedef struct tagHY_DVR_CMDOT_FALLING_OBJECT_REGION_LIST
{
	int nValid;
	int nDetectCount;
	int nIgnoreCount;
	HY_DVR_CMDOT_FALLING_OBJECT_REGION struHYDetectRegion[HY_DVR_CMDOT_MAX_FALLING_OBJECT_DETECT_REGION];
	HY_DVR_CMDOT_FALLING_OBJECT_REGION struHYIgnoreRegion[HY_DVR_CMDOT_MAX_FALLING_OBJECT_IGNORE_REGION];
	HY_DVR_CMDOT_FALLING_OBJECT_DETECT_LINE struHYDetectLine;
	char szReserved[128];
}HY_DVR_CMDOT_FALLING_OBJECT_REGION_LIST, *LPHY_DVR_CMDOT_FALLING_OBJECT_REGION_LIST;


typedef struct tagHY_DVR_CMDOT_MOTOR_VEHICLE_OBJECT_REGION
{
	int nVaild;
	int nPointCount;
	int nId;
	HY_DVR_CMDOT_SET_AI_REGION_POINT struRegionPoint[HY_DVR_CMDOT_MAX_MOTOR_VEHICLE_OBJECT_REGION_POINT];
	int nDirection;     ///< 检测方向（仅针对车辆检测区域有效），1：上行,   2:下行,   0或者未定义表示无方向定义
	char szReserved[32 - 4];
}HY_DVR_CMDOT_MOTOR_VEHICLE_OBJECT_REGION, *LPHY_DVR_CMDOT_MOTOR_VEHICLE_OBJECT_REGION;

typedef struct tagHY_DVR_CMDOT_MOTOR_VEHICLE_OBJECT_REGION_LIST
{
	int nValid;
	int nDetectCount;
	HY_DVR_CMDOT_MOTOR_VEHICLE_OBJECT_REGION struHYDetectRegion[HY_DVR_CMDOT_MAX_MOTOR_VEHICLE_OBJECT_DETECT_REGION];
	char szReserved[128];
}HY_DVR_CMDOT_MOTOR_VEHICLE_OBJECT_REGION_LIST, *LPHY_DVR_CMDOT_MOTOR_VEHICLE_OBJECT_REGION_LIST;

typedef struct tagHY_DVR_CMDOT_EBIKE_ALARM_STATUS
{
	int bValid;
	int nEbikeAlarmStatus; // 梯控报警状态 0-关 非0-开
	int nEbikeAlarmExcute; // 梯控报警执行 0-可以暂缓执行 非0-立即执行
}HY_DVR_CMDOT_EBIKE_ALARM_STATUS, *LPHY_DVR_CMDOT_EBIKE_ALARM_STATUS;


typedef struct tagHY_DVR_CMDOT_ALGORITHM_SUPPORT
{
	// 0x0001 支持人形检测算法
	// 0x0002 支持人脸检测算法
	// 0x0004 支持车牌检测算法
	// 0x0008 支持人脸比对算法
	// 0x0010 支持机动车检测算法
	// 0x0020 支持电瓶车检测算法
	int bValid;
	int nAlgorithmSet[32];    // 算法能力集合，从序号0开始。例如设备支持【方式1，人形检测和人脸检测】、【方式2 电瓶车检测】、【方式3 支持机动车和车牌检测】
								// nAlgorithmSet[0] = 0x01+ 0x02;
								// nAlgorithmSet[1] = 0x20;
								// nAlgorithmSet[2] = 0x04 + 0x10;
								// nAlgorithmSetNumber = 3;
	int nAlgorithmSetNumber;  // 算法能力集合个数

	int nRegionHackAlgorithm; // 算法支持告警入侵的方式，按位或方式，例如设备支持【1.人形】、【2.机动车】、【3.电瓶车】触发警戒
								// nRegionHackAlgorithm = 0x01 + 0x10 + 0x20;

}HY_DVR_CMDOT_ALGORITHM_SUPPORT, *LPHY_DVR_CMDOT_ALGORITHM_SUPPORT;

typedef struct tagHY_DVR_CMDOT_REGION_HACK_ALGORITHM
{
	int bValid;
	int nRegionhackAlgorithm; // 告警入侵算法，可能是组合参数，参数意义Refer HY_DVR_CMDOT_ALGORITHM_SUPPORT
}HY_DVR_CMDOT_REGION_HACK_ALGORITHM, *LPHY_DVR_CMDOT_REGION_HACK_ALGORITHM;


typedef struct tagHY_DVR_CMDOT_VEHICLE_DETECT_STATUS
{
	int bValid;
	int nVehicleDetectEnable; // 机动车检测开关
}HY_DVR_CMDOT_VEHICLE_DETECT_STATUS, *LPHY_DVR_CMDOT_VEHICLE_DETECT_STATUS;

typedef struct tagHY_DVR_CMDOT_STREAM_TYPE
{
	int bValid;
	int nStreamType; // 码流类型，0，子码流，1，主码流
}HY_DVR_CMDOT_STREAM_TYPE, *LPHY_DVR_CMDOT_STREAM_TYPE;
typedef struct tagHY_DVR_CMDOT_NIGHT_VISION_MODE
{
	int bValid;
	int nNightMode; // 夜视模式 0自动   1 关闭  2打开
	char cReserve[128];
}HY_DVR_CMDOT_NIGHT_VISION_MODE, *LPHY_DVR_CMDOT_NIGHT_VISION_MODE;

/** 报警事件类型定义 */
typedef enum {
    CMDOT_ALARM_BEGIN = 0,
    CMDOT_ALARM_MOTION_DETECT,               /**< 移动侦测，需要上报开始和结束 */
    CMDOT_ALARM_SOUND_DETECT,                /**< 声音侦测，需要上报开始和结束 */
    CMDOT_ALARM_PIR_DETECT,                  /**< PIR侦测，脉冲事件，只需要上报开始 */
    CMDOT_ALARM_FACE_DETECT,                 /**< 人脸侦测，需要上报开始和结束 */
    CMDOT_ALARM_PEOPLE_DETECT,               /**< 人形检测，需要上报开始和结束 */
    CMDOT_ALARM_RING,                        /**< 门铃按键，脉冲事件，只需要上报开始 */
    CMDOT_ALARM_TAMPER_ALARM,                /**< 防拆报警，脉冲事件，只需要上报开始 */
	CMDOT_ALARM_REGION_HACK,			       /**< 区域入侵检测，需要上报开始和结束 */

    CMDOT_ALARM_FALLING_OBJECT_ALARM,             /**< 高空抛物事件，需要上报开始和结束 */



    CMDOT_ALARM_EBIKE_DETECT,                     /**<  电动车入侵事件 */


    CMDOT_ALARM_MOTOR_VEHICLE_DETECT,             /**< 机动车入侵事件 */


    CMDOT_ALARM_NO_MASK,                          /**< 未戴口罩 */
    CMDOT_ALARM_NO_HAT,                           /**< 未戴工帽 */
    CMDOT_ALARM_NO_WORKING_CLOTHES,                      /**< 未穿工装 */


    CMDOT_ALARM_MAX = 0xFFFF,
} CMDOT_EVENT_TYPE_EN;

typedef struct tagHY_DVR_CMDOT_NOTIFY
{
	int bValid;
	int nMessageType;//消息类型：0,事件
	int nParam1;	//当nMessageType为事件消息类型时，该参数表示事件类型,参考CMDOT_EVENT_TYPE_EN类型
	int nParam2;
	int nParam3;
	int nParam4;
	char cReserve[64];
}HY_DVR_CMDOT_NOTIFY;

typedef struct tagHY_DVR_DM_INIT_PARAM_INFO
{
	char szCMEI_IMEI[64]; 	//CMEI/IMEI,和IMEI2选一个填,参考规范文档;设备没有的话向移动申请； ex:111041730000001
	char szCMEI_IMEI2[64]; 	//CMEI/IMEI2,和IMEI选一个填，参考规范文档
	char szIMSI[64];		//IMSI,没有可设为空
	char szDMv[16]; 		//DM版本号,固定v2.0
	char szAppKey[64]; 		//appkey,需申请
	char szPwd[64];			//pwd秘钥,需申请

	int  nAddressFamily; 	//4或6,表示ipv4，ipv6; 固定设为4;
	char szSrvIP[32];		//服务器host or ip : shipei.fxltsbl.com(适配)/m.fxltsbl.com(商用)
	int  nSrvPort;   		//服务器端口：5683
	int  nLifetime;    		//更新间隔，心跳，生命周期：单位s,测试设置300s,商用设为一天86400s
	int  nLocalPort;        //本地端口绑定,默认为56830
	int  nBootstrap; 		//是否booststrap 0,not use; other n,use;	
}HY_DVR_DM_INIT_PARAM_INFO;

//字段赋值参看 多形态终端上报接口规范V2.0.3(多形态二期).docx
//不支持:unSupport
//没有值:unKnown
typedef struct tagHY_DVR_DM_READ_INFO
{
	char szDeviceInfo[64]; 	//devinfo
	char szAppInfo[64]; 	//appinfo,ex:app_name1|pak_name1|1234000|3\r\napp_name2|pak_name2|2234000|5\r\n
	char szMac[64];			//mac地址，ex:00:50:56:C0:00:01
	char szRom[16]; 		//rom，ex:4GB
	char szRam[64]; 		//ram,ex:2GB
	char szCPU[64];			//CPU,ex:ARM
	char szSystemVersion[64];	//linux系统版本，ex：LINUX 2.6.18
	char szFirmwareVersion[64];	//固件版本，ex:1.20.1
	char szFirmwareName[64];	//固件名，ex：X-A10
	char szVolte[64];			//volte，ex：0
	char szNetType[64];			//网络类型，ex:4G
	char szNetAccount[64];		//网络账号，ex：Account
	char szNetPhoneNumber[64];	//电话，ex：13812345678
	char szLocation[64];		//地理位置，ex：25.77701556036132,123.52958679200002,1
}HY_DVR_DM_READ_INFO;


typedef struct tagHY_DVR_DM_LAMP_INFO
{
 int iValid; /*!< 本结构体是否有效，0表示无效 */
 int powerStatus; /*!<关-0，开-1> */
 int lightMode; /*!<模式,护眼学习-0，自动调光-1，高效工作-2，屏幕阅读-3> */
 int delaySwith; /*!<延时30S关灯开关，关-0，开-1> */
 int countDown; /*!<定时开-0，定时关-1> */ 
int brightness; /*!<0-100单位空缺> */
 int AISwitch; /*!<AI开关，关-0，开-1> */
 int strongLightProtect; /*!<强光保护开关，关-0，开-1>*/
 char cReserved[1024];
}HY_DVR_DM_LAMP_INFO, *LPHY_DVR_DM_LAMP_INFO;

typedef struct tagHY_DVR_DM_SOUND_COLUMN_INFO
{
 int iValid; /*!< 本结构体是否有效，0表示无效 */
 int deviceAudio; /*!<IP音柱声音开关, 关-0，开-1> */
 int deviceVolume; /*!<IP音柱音量控制, 0-100> */
 char cReserved[256];
}HY_DVR_DM_SOUND_COLUMN_INFO, *LPHY_DVR_DM_SOUND_COLUMN_INFO;

typedef struct CGIRect
{
	int x;
	int y;
	int width;
	int height;
}CGIRect;

typedef struct CGISize
{
	int width;
	int height;
}CGISize;

///////////////////////////////////////////////////////////////////////////////////////////////
/*!
*  @struct	tagHY_DVR_TIME
*  @brief	配置系统时间参数
 */
typedef struct tagHY_DVR_TIME
{
	int nYear;																/*!< 年 */
	int nMonth;																/*!< 月 */
	int nDay;																/*!< 日 */
	int nHour;																/*!< 时 */
	int nMinute;															/*!< 分 */
	int nSecond;															/*!< 秒 */

} HY_DVR_TIME, *LPHY_DVR_TIME;

/** @brief 配置系统时间结构体长度																 */
#define TIME_CFG_LEN sizeof(HY_DVR_TIME)

/*!
*  @struct	tagHY_DVR_TIME_STAMP
*  @brief	配置系统时间参数
 */
typedef struct tagHY_DVR_TIME_STAMP
{
	int nValid;
	unsigned long long timestamp;
} HY_DVR_TIME_STAMP, *LPHY_DVR_TIME_STAMP;

/** @brief 配置系统时间结构体长度																 */
#define TIME_STAMP_CFG_LEN sizeof(HY_DVR_TIME_STAMP)

typedef struct tagHY_DVR_UTC_TIMESTAMP
{
	int nValid;

	int sec;
	int m_sec;
	int u_sec;		//精确到微秒，计算方式：此刻的时间戳(us)=sec*1000*1000+m_sec*1000+u_sec;

	char szReserved[32];
}HY_DVR_UTC_TIMESTAMP, *LPHY_DVR_UTC_TIMESTAMP;
/////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************
*                        配置信息结构体定义                              *
************************************************************************* */

////////////////////////////////////////////////////////////////////////////
//该结构体仅适用于IPC设备
/*
*  @struct	tagZW_IPC_COLOR
*  @brief	IPC颜色配置
 */
typedef struct tagZW_IPC_COLOR
{
	int bValid;			/*!< 本结构体是否有效，0表示无效 */

	BYTE nAutoChangeBright;		/*!< 是否自适应亮度,0表示不进行自适应,1表示自适应亮度0xff表示不支持 */   //当是具有车牌相关功能的IPC时，该功能被屏蔽（使用结构体ZW_SYSCOMMON_NORMAL_CFG的nEnableFlag字段用来标记是否具有车牌标记的功能）

	BYTE nBright;		/*!< 亮度,范围[0,100] */   //当是具有车牌相关功能的IPC时，该功能表示强光抑制（使用结构体ZW_SYSCOMMON_NORMAL_CFG的nEnableFlag字段用来标记是否具有车牌标记的功能）
	BYTE nContrast;		/*!< 对比度,范围[0,100] */
	BYTE nSaturation;	/*!< 饱和度,范围[0,100] */
	BYTE nChroma;		/*!< 色度,范围[0,100] */
	BYTE nSharpness;		/*!< 锐度,范围[0,100] */
}ZW_IPC_COLOR;

/*
*  @struct	tagZW_IPC_BASIC
*  @brief	IPC基本配置
 */
typedef struct tagZW_IPC_BASIC
{
	int bValid;			/*!< 本结构体是否有效，0表示无效 */

	BYTE nAntiFlickMode;		/*!< 抗抖动模式,0表示50HZ,1表示60HZ ,2,关闭，0xff表示不支持*/
	BYTE n3DDeNoise;			/*!< 3D降噪, 0表示关闭3D降燥, 1表示弱,2表示一般,3表示强 */
	BYTE nBackLight;			/*!< 背光补偿,0表示关闭,1表示打开 0xff表示不支持*/
	BYTE nWideDynamicRange;	/*!< 宽动态,0表示关闭, 1表示弱,2表示一般,3表示强,4表示自动 0xff表示不支持*/
	BYTE nMirrorMode;		/*!< 镜像模式,0表示关闭,1表示左右,2表示上下,3表示中心（180°翻转） 0xff表示不支持*/
}ZW_IPC_BASIC;

/*!
*  @struct	tagZW_IPC_EXPOSURE
*  @brief	IPC曝光配置
 */
typedef struct tagZW_IPC_EXPOSURE
{
	int bValid;			/*!< 本结构体是否有效，0表示无效 */

	BYTE nIrisType;			/*!< 光圈类型, 0,表示手动光圈,1表示自动光圈 0xff表示不支持*/
	int nExposureDelay;		/*!< 曝光时间,0,自动;25,表示1/25;  50,表示1/50;  100,表示1/100; ...*///当是具有车牌相关功能的IPC时，该功能表示车速  /*25表示0-30km/h,100表示30-60km/h,1000表示60-90km/h,2000表示>90km/h*/（使用结构体ZW_SYSCOMMON_NORMAL_CFG的nEnableFlag字段用来标记是否具有车牌标记的功能）
	int nGain;				/*!< 增益, 0表示自动; 其它表示相应增益值,如6表示6dB */
}ZW_IPC_EXPOSURE;

/*!
*  @struct	tagZW_IPC_WHITE_BLANCE
*  @brief	IPC白平衡配置
 */
typedef struct tagZW_IPC_WHITE_BLANCE
{
	int bValid;			/*!< 本结构体是否有效，0表示无效 */

	BYTE nWhiteBlanceMode;	/*!< 白平衡模式,0表示白炽灯,1暖光灯,2日光灯,3自然光,4手动白平衡,5锁定白平衡,6自动白平衡，7室内模式，8.室外模式 ，9.白平衡1，10.白平衡2，0xff表示不支持*/
	BYTE nGainRed;			/*!< 当 nWhiteBlanceMode等于4时 有效，范围[0,100] */
	BYTE nGainBlue;			/*!< 当 nWhiteBlanceMode等于4时 有效，范围[0,100] */
}ZW_IPC_WHITE_BLANCE;

/*!
*  @struct	tagZW_IPC_DAY_NIGHT
*  @brief	IPC日夜转换
 */
typedef struct tagZW_IPC_DAY_NIGHT
{
	int bValid;			/*!< 本结构体是否有效，0表示无效 */

	BYTE nMode;			/*!< 0表示自动（外部红外自动控制，被动模式）, 1白天，2夜晚，3定时，4根据亮度自动控制（主动模式），5夜视全彩（全彩白）
											6智能全彩（智能双光），7智能红外（强光抑制）无极红，8智能白光PWM（无极白），9普通白光, 10星光夜视*/

	BYTE nStartHour;		/*!< 当nMode等于3时有效, 夜晚开始小时,范围[0,23] */
	BYTE nStartMin;		/*!< 当nMode等于3时有效, 夜晚开始分钟,范围[0,59] */
	BYTE nStartSec;		/*!< 当nMode等于3时有效, 夜晚开始秒,  范围[0,59] */

	BYTE nStopHour;		/*!< 当nMode等于3时有效, 夜晚结束小时,范围[0,23] */
	BYTE nStopMin;		/*!< 当nMode等于3时有效, 夜晚结束分钟,范围[0,59] */
	BYTE nStopSec;		/*!< 当nMode等于3时有效, 夜晚结束秒,  范围[0,59] */
}ZW_IPC_DAY_NIGHT;


/*!
*  @struct	tagZW_ISP
*  @brief	IPC的ISP配置
 */
typedef struct tagZW_ISP
{
	int bValid;
	ZW_IPC_BASIC		struBasic;
	ZW_IPC_COLOR		struColor;
	ZW_IPC_EXPOSURE		struExposure;
	ZW_IPC_WHITE_BLANCE struWhiteBlance;
	ZW_IPC_DAY_NIGHT	struDayAndNight;
}ZW_IPC_ISP_CONFIG;



/*!
*  @struct	tagZW_IPC_ISP_DEFAULT
*  @brief	IPC的默认ISP配置结构
 */
typedef struct tagZW_IPC_ISP_DEFAULT
{
	int bValid;
}ZW_IPCISP_DEFAULT, *LP_ZW_IPCISP_DEFAULT;

/*!
*  @struct  tagZW_IPC_ISP_DEFAULT_V2
*  @brief   IPC参数默认配置V2结构
 */
typedef struct tagZW_IPC_ISP_DEFAULT_V2
{
	int iValid;
	
	int iRestoreType;     //  1，恢复ISP参数  2 恢复灯光参数
	int iChannel;         // 通道号，从0开始

	char acReserved[128 - 4];
}ZW_IPCISP_DEFAULT_V2, *LP_ZW_IPCISP_DEFAULT_V2;



//该结构体仅适用于NVR设备
/*!
*  @struct	tagZW_NVR_COLOR
*  @brief	NVR颜色配置
 */
typedef struct tagZW_NVR_COLOR
{
	int bValid;
	ZW_IPC_COLOR      struColor[DVR_MAX_VIDEOIN_NUM];
}ZW_NVR_COLOR;

/*!
*  @struct	tagZW_NVR_BASIC
*  @brief	NVR基本配置
 */
typedef struct tagZW_NVR_BASIC
{
	int bValid;
	ZW_IPC_BASIC		struBasic[DVR_MAX_VIDEOIN_NUM];
}ZW_NVR_BASIC;

/*!
*  @struct	tagZW_NVR_EXPOSURE
*  @brief	NVR曝光配置
 */
typedef struct tagZW_NVR_EXPOSURE
{
	int bValid;
	ZW_IPC_EXPOSURE		struExposure[DVR_MAX_VIDEOIN_NUM];
}ZW_NVR_EXPOSURE;

/*!
*  @struct	tagZW_NVR_WHITE_BLANCE
*  @brief	NVR白平衡配置
 */
typedef struct tagZW_NVR_WHITE_BLANCE
{
	int bValid;
	ZW_IPC_WHITE_BLANCE struWhiteBlance[DVR_MAX_VIDEOIN_NUM];
}ZW_NVR_WHITE_BLANCE;

/*!
*  @struct	tagZW_NVR_DAY_NIGHT
*  @brief	NVR日夜转换
 */
typedef struct tagZW_NVR_DAY_NIGHT
{
	int bValid;
	ZW_IPC_DAY_NIGHT	struDayAndNight[DVR_MAX_VIDEOIN_NUM];
}ZW_NVR_DAY_NIGHT;

/*!
*  @struct	tagZW_NVR_ISP
*  @brief	NVR的ISP配置
 */
typedef struct tagZW_NVR_ISP
{
	int bValid;
	ZW_IPC_ISP_CONFIG IpcIspConfig[DVR_MAX_VIDEOIN_NUM];
}ZW_NVR_ISP_CONFIG;



typedef struct tagZW_IPC_DAY_NIGHT_VALUE
{
	int	nDay2Night;	   //0--100
	int nNight2Day;	   //0--100
}ZW_IPC_DAY_NIGHT_VALUE;

/*!
*  @struct	tagZW_IPC_ISP_EX
*  @brief	IPC的ISP扩展配置
 */
typedef struct tagZW_IPC_ISP_EX
{
	int bValid;
	ZW_IPC_DAY_NIGHT_VALUE struDayNightValue;
	int nDayNightSensitivity;	    //日夜转化的灵敏度
	unsigned int nDevIspCab;		//表示ISP扩展功能的能力集,参见上面的宏
	int nIRCutModeEnable;		//IRCut模式的使能，1，表示该设备具有IRcut相关的功能，0，表示该设备不具有该功能，该功能包括的字段nPassivitySwitchTime ,nInitactiveSwitchTime	,nPassivityDayNightSensitivity,nInitactiveDayNightSensitivity
	int nPassivitySwitchTime;			//被动模式下的切换时间
	int nInitactiveSwitchTime;			//主动模式下的切换时间
	int nPassivityDayNightSensitivity;  //被动模式下日夜转化的灵敏度
	int nInitactiveDayNightSensitivity;  //主动模式下的灵敏度
	int nSceneEnable;					//是否支持场景模式，0，表示不支持，1，表示支持
	int nSceneMode;						//场景模式选项为“标准”（值为0）、“柔和”（值为1）、“鲜艳”（值为2）、“自定义”（值为3）
	int nSupportDeFrog;						// 去雾. 0, 表示不支持, 1, 表示支持
	int nDeFrog;							// 当前是启用去雾0,表示关闭，1表示启用
	int nSupportCorridor;				// 是否支持走廊模式, 0, 表示不支持, 1, 表示支持
	int nEnableCorridor;				// 是否开启走廊模式，0, 关闭，　1, 开启
	unsigned short nSupportIRcutReverse;	// 是否支持IRCUT反序
	unsigned short nEnableIRcutReverse;		// 是否启用IRCUT反序
	int nSupportShutterMode;				//是否支持快门模式，0，表示不支持，1，表示支持
	int nEnableShutterMode;					//使能快门模式，0，表示自动模式，1，手动模式
	int nSupportLDC;						//是否支持LDC处理,0，表示不支持LDC，1，表示支持LDC。
	int nEnableLDC;							//是否使能LDC处理.0，表示关闭LDC，1，表示打开LDC。
	int nGammaValue;						//0-100
	int nDeFrogStrengthValue;				//去雾模式开启时可以设置去雾等级（0-100）
	int nColorNightLastTime;				//智能全彩持续时间
	int nVariableInfraredThreshold;			//可变的红外阈值（0-100）
	int nVariableWhiteThreshold;			//可变的白光阈值（0-100）
	int nImageMode;							//0,标准模式，1,人脸无曝光模式，2,车牌模式
	int nIsSupportColorOtherConfig;			//是否支持智能全彩功能的其他功能的配置，1，支持，0，不支持
	int nColorWorkMode;						//智能全彩的工作模式，0，表示自定义，1表示自动		智能全彩=全彩白光
	HY_DVR_TIME struColorWorkModeCustomStartTime; //智能全彩自定义工作模式下的开始时间
	HY_DVR_TIME struColorWorkModeCustomStopTime;  //智能全彩自定义工作模式下的结束时间
	int nColorWorkModeCustomBrigthness;		//智能全彩自定义工作模式下的亮度调节   0--100
	int nIsSupportintelligentOtherConfig;   //是否支持智能夜视的其他功能的配置，1.支持，0，不支持
	int nintelligentWorkmode;				//智能夜视的工作模式，0，常亮，1，闪烁			智能夜视=智能双光
	int nintelligentWorkmodeFlickerTime;		//智能夜视的闪烁工作模式的持续时间  5-180S
	int nintelligentWorkmodeFlickerFrequency;	//智能夜视的闪烁工作模式的闪烁频率  0-100
	int nintelligentWorkmodeFlickerBrigthness;	//智能夜视闪烁工作模式下的亮度调节		0-100
	int nIsSupportEnableLight;				//是否支持灯板开关
	int nEnableLight;						//灯的开关
	int nSupportLDCAdvance;					//是否支持LDC增强,0，表示不支持，1，表示支持。
	int nLDCAdvance;						//增加LDC强度.值域范围0-100。
	int nIsSupportVarWhiteColorOtherConfig;
	int nVarWhiteControlMode;		//无极白光模式字段		0:custom	1:auto
	int nVarWhiteBrightness;			//无极白光手动模式亮度控制
	int nVarWhiteWorkMode;			//无极白光工作模式		//0:定时	1:手动阈值
	HY_DVR_TIME struVarWhiteModeStartTime;		//无极白光起始时间
	HY_DVR_TIME struVarWhiteModeStopTime;		//无极白光终止时间
	int nVarWhiteLightOnThreshold;			//无极白光开灯阈值
	int nVarWhiteLightOffThreshold;			//无极白光关灯阈值
	int nNormalWhiteLightOffLimit;				//普通白光关灯阈值
	int nNormalWhiteLightOnLimit;				//普通白光开灯阈值
	//无极红外工作模式
	int nVarInfraredWorkMode;		//0:custom	1:auto
	int nVarInfraredBrightness;		//亮度		0-100
	//无极红外亮度值
	int nColorNightThreshold;			//智能全彩=全彩白光阈值
	int nIntelligentThreshold;			//智能夜视=智能双光阈值
	int nWDRSwitch;						//WDR开关，0：关闭，1：开启
	int nWDRLevel;							//WDR等级，0-100
	int nPWMValue;						//PWM值，用来控制白光灯亮度		50：半亮		100：全亮
	int iAntiFlashStrength;				//抗闪强度
	HY_DVR_TIME struVarRedModeStartTime;    //无极红光起始时间
    HY_DVR_TIME struVarRedModeStopTime;        //无极红光终止时间
	char cReserved2[168];
}ZW_IPC_ISP_EX;

/*!
*  @struct	tagZW_COM_ISP_EX
*  @brief	NVR的扩展配置
 */
typedef struct tagZW_COM_ISP_EX
{
	int bValid;
	ZW_IPC_ISP_EX IpcIspExConfig[DVR_MAX_VIDEOIN_NUM];
}ZW_COM_ISP_EX;



/*!
*  @enum    eLightSceneMode
*  @brief    场景模式
*/
typedef enum
{
    eLightSceneMode_Unknown = 0,
    eLightSceneMode_StarlightFullColor = 1,         // 星光全彩
    eLightSceneMode_IntelligentFullColor = 2,       // 智能全彩
    eLightSceneMode_TimerMode = 3,                  // 定时模式
    eLightSceneMode_FullColorNightVision = 4,       // 全彩夜视
    eLightSceneMode_BlackWhiteNightVision = 5,      // 黑白夜视
    eLightSceneMode_IntelligentDualLight = 6        // 智能双光
}eLightSceneMode;
/*!
*  @struct  tagZW_IPC_ISP_EX_EX
*  @brief   IPC的ISP扩展配置
 */
typedef struct tagZW_IPC_ISP_EX_EX
{
    int bValid;
 
    int iLightSceneControlMode;             //灯光场景控制模式：0--场景配置模式 1--专家模式
    eLightSceneMode eSceneMode;             //场景模式（灯光场景控制模式为0时生效）：星光全彩 智能全彩 定时模式 全彩夜视 黑白夜视 智能双光
     
    char cReserved2[1024];
}ZW_IPC_ISP_EX_EX;
 
/*!
*  @struct  tagZW_COM_ISP_EX_EX
*  @brief   NVR的扩展配置的扩展
 */
typedef struct tagZW_COM_ISP_EX_EX
{
    int bValid;
    ZW_IPC_ISP_EX_EX IpcIspExConfigEx[DVR_MAX_VIDEOIN_NUM];
}ZW_COM_ISP_EX_EX;

/*!
*  @struct	tagZW_IPC_TONE_RGB2YUV
*  @brief	RGB转YUV调节
*/
typedef struct tagZW_IPC_TONE_RGB2YUV
{
	int bValid;
	int matrix_values[9];// 范围[-1024-1023]
	int y_offset;// 范围［-128-255］
	int u_offset;// 范围［-128-255］
	int v_offset;// 范围［-128-255］
}ZW_IPC_TONE_RGB2YUV, *LP_ZW_IPC_TONE_RGB2YUV;

/*!
*  @struct	tagZW_IPC_TONE_ISP
*  @brief	用户定制图像风格调节
*/
typedef struct tagZW_IPC_TONE_ISP
{
	int bValid;
	ZW_IPC_TONE_RGB2YUV struRGB2YUV;
	char acReserved[1024];
}ZW_IPC_TONE_ISP, *LP_ZW_IPC_TONE_ISP;


/* 区域信息(左上点坐标(ntopx, ntopy)和右下点坐标(nbottomx, nbottomy)，范围均是0--9999 */
typedef struct tagRect
{
	int ntopx;			/* <==>left   <==> x1 */
	int ntopy;			/* <==>top    <==> y1 */
	int nbottomx;       /* <==>right  <==> x2 */
	int nbottomy;       /* <==>bottom <==> y2 */
}RECT_T, *LPRECT_T;

/** @brief 表示ROI配置信息							 */
typedef struct tagROIChnanel
{
	int         nEnable;
	int 	    nLevel;
	RECT_T      struArea;
}HY_DVR_ROI_CHANNEL, *LPHY_DVR_ROI_CHANNEL;

/** @brief 表示每一个码流支持的ROI的情况		      */
typedef struct tagROICfg
{
	int bValid;
	int nEnable;				    //ROI区域
	HY_DVR_ROI_CHANNEL struROIInfo[MAX_ROI_NUM];
}HY_DVR_ROI_CFG, *LPHY_DVR_ROI_CFG;

/** @brief 表示每一个通道支持的么一个码流的ROI的情况												          	 */
typedef struct tagStreamROIChannel
{
	int bValid;
	int nAreaMin;                           //表示每一个通道支持的最小ROI的区域值
	int nAreaMax;                           //表示每一个通道支持的最大ROI的区域
	HY_DVR_ROI_CFG	struMainStream;	//主码流的ROI属性
	HY_DVR_ROI_CFG  struSubStream;	//子码流的ROI属性
	HY_DVR_ROI_CFG  struReserve[2];	//保留
}HY_DVR_STREAM_ROI_CHANNEL, *LPHY_DVR_STREAM_ROI_CHANNEL;


/** @brief 表示对于一个设备支持总的ROI情况	 */
typedef struct tagStreamROICfg
{
	int bValid;
	HY_DVR_STREAM_ROI_CHANNEL  struStreamRoiChannel[DVR_MAX_VIDEOIN_NUM];
}HY_DVR_STREAM_ROI_CFG, *LPHY_DVR_STREAM_ROI_CFG;

typedef struct tagHY_DVR_SCHEDULE_TIMESLOT
{
    int iValid;                    /*!< 本结构体是否有效，0表示无效 */

    int iEnable;                   /*!< 时间段是否有效，0-无效,1-有效 */
    char acStartTime[32];          /*!< 开始时间，格式hh:mm,24小时制 */        
    char acEndTime[32];            /*!< 结束时间，格式hh:mm,24小时制 */
    char acWeekday[8];             /*!< 每周7天，下标0表示周日,下标1表示周一,以此类推;元素取值1表示选定对应星期,取值0表示没有选择对应星期, 第8位保留 */

    char szReserved[128];
}HY_DVR_SCHEDULE_TIMESLOT;

/*!
*  @struct	tagHY_DVR_SCHEDTIME
*  @brief	时间计划, 24小时等分成48个时间段
 */
typedef struct tagHY_DVR_SCHEDTIME
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */

	unsigned char bySetType[DVR_MAX_DAYS][DVR_MAX_TIMESEGMENT];	/*!< 0为不布防，1为布防 */
	unsigned char byState;										/*!< 状态,0为全天布防,1为启用,2为不支持 */
	unsigned char byReserve[3];									/*!< 保留位 */

} HY_DVR_SCHEDTIME, *LPHY_DVR_SCHEDTIME;

/*!
*  @struct	tagHY_DVR_WORKMODE
*  @brief	工作模式
 */
typedef struct tagHY_DVR_WORKMODE
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */

	int nWorkMode;												/*!< 高级模式，0为普通模式，1为高级模式 */

} HY_DVR_WORKMODE, *LPHY_DVR_WORKMODE;

/*!
*  @struct	tagHY_DVR_FONT_INFO
*  @brief	字体信息
 */
typedef struct tagHY_DVR_FONT_INFO
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */

	char szFontName[DVR_MAX_FONTNAME_LEN];						/*!< 字体名 */
	int nFontStyle;												/*!< 字体风格 */
	int nFontSize;												/*!< 字体大小 */

} HY_DVR_FONT_INFO, *LPHY_DVR_FONT_INFO;

/*!
*  @struct	tagHY_DVR_ALIAS_CFG_1
*  @brief	别名配置(Unicode)
*/
typedef struct tagHY_DVR_ALIAS_CFG_1
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */

	unsigned short strName[DVR_MAX_CHANNELNAME_LEN];			/*!< 通道名称 */
	int nCharSet;												/*!< 通道名字符集: 0-GB, 1-unicode */
	int nWidth;													/*!< 通道名称完整字符串宽度(以像素为单位)，最大宽度 DVR_MAX_LATTICE_WIDTH */
	int nHeight;												/*!< 通道名称完整字符串高度(以像素为单位)，最大高度 DVR_MAX_LATTICE_HEIGHT */
	unsigned char byLattice[DVR_MAX_LATTICE_BUFSIZE];			/*!< 点阵，按行扫描，宽为(nWidth+7)/8，高为nHeight，目前最大支持10个24*24的点阵 */
	HY_DVR_FONT_INFO struFontInfo;								/*!< 通道名字体信息 */

} HY_DVR_ALIAS_CFG_1, *LPHY_DVR_ALIAS_CFG_1;


#define DVR_MAX_EXTENT_OSD_CNT		(8)
#define DVR_MAX_EXTENT_OSD_TEXT_LENGTH  (256)
#define DVR_MAX_EXTENT_OSD_TOKEN_LENGTH  (32)

/*!
*  @struct	tagHY_DVR_OSD_EXTENT
*  @brief	扩展的OSD配置
*/
typedef struct tagHY_DVR_OSD_EXTENT
{
	int bValid;

	char  szOSDToken[DVR_MAX_EXTENT_OSD_TOKEN_LENGTH];		/*!< OSD的文字描述 */
	unsigned short  strText[DVR_MAX_EXTENT_OSD_TEXT_LENGTH];/*!< OSD的文字内容 */
	int nCharSet;												/*!< 通道名字符集: 0-GB, 1-unicode */
}HY_DVR_OSD_EXTENT;

/*!
*  @struct	tagHY_DVR_OSD_EX
*  @brief	扩展的OSD配置, 用于获取，设置一堆OSD
*/
typedef struct tagHY_DVR_OSD_EX
{
	int bValid;
	int               nChannel;									// 通道号
	HY_DVR_OSD_EXTENT struOSD[DVR_MAX_EXTENT_OSD_CNT];			// 最多支持8个OSD
}HY_DVR_OSD_EX;

/*!
*  @struct	tagHY_DVR_OSD_NODE_EX
*  @brief	扩展的OSD配置结点, 用于添加，修改，删除一个OSD结点
*/
typedef struct tagHY_DVR_OSD_NODE_EX
{
	int bValid;
	int nChannel;
	HY_DVR_OSD_EXTENT struOSD;
}HY_DVR_OSD_NODE_EX;


#define DVR_MAX_OSD_V2_CNT		(4)
#define DVR_MAX_OSD_V2_TEXT_LENGTH  (512)
#define DVR_MAX_OSD_V2_TOKEN_LENGTH  (32)

/*!
*  @struct	tagHY_DVR_OSD_CHANNEL_V2
*  @brief	扩展的OSD配置
*/
typedef struct tagHY_DVR_OSD_CHANNEL_V2
{
	int bValid;
	int bEnable;										/*!< OSD是否启用 */
	int nX;												/*!< OSD位置x坐标 */
	int nY; 											/*!< OSD位置y坐标 */
	char  szToken[DVR_MAX_OSD_V2_TOKEN_LENGTH];      	/*!< OSD的文字描述 */
	unsigned short strText[DVR_MAX_OSD_V2_TEXT_LENGTH]; /*!< OSD的文字内容,编码必须为unicode */
	// OSD文字内容说明 
	// ^表示换行
	// [FPS]表示帧率
	// [BPS]表示码率
	// [RES] 表示分辨率
} HY_DVR_OSD_CHANNEL_V2;

typedef struct tagHY_DVR_OSD_TIME_CHANNEL_V2
{
	int bValid;
	int bEnable;										/*!< OSD 时间是否启用 */
	int nX;												/*!< OSD 时间位置x坐标 */
	int nY; 											/*!< OSD 时间位置y坐标 */
} HY_DVR_OSD_TIME_CHANNEL_V2;

/*!
*  @struct	tagHY_DVR_OSD_V2
*  @brief	V2的OSD配置, 用于获取，设置一堆OSD
*/
typedef struct tagHY_DVR_OSD_V2
{
	int bValid;
	int nChannel;												/*!< 通道号 */
	HY_DVR_OSD_TIME_CHANNEL_V2 struOSDTime;						/*!< OSD时间 */
	HY_DVR_OSD_CHANNEL_V2 struOSD[DVR_MAX_OSD_V2_CNT];			/*!< 最多支持4个OSD */
}HY_DVR_OSD_V2;


#define DVR_MAX_OSD_V4_CNT		(8)
#define DVR_MAX_OSD_V4_TEXT_LENGTH  (512)
#define DVR_MAX_OSD_V4_TOKEN_LENGTH  (32)

/*!
*  @struct	tagHY_DVR_OSD_CHANNEL_V4
*  @brief	扩展的OSD配置
*/
typedef struct tagHY_DVR_OSD_CHANNEL_V4
{
	int bValid;
	int bEnable;										/*!< OSD是否启用 */
	int nX;												/*!< OSD位置x坐标 */
	int nY; 											/*!< OSD位置y坐标 */
	char  szToken[DVR_MAX_OSD_V4_TOKEN_LENGTH];      	/*!< OSD的文字描述 */
	unsigned short strText[DVR_MAX_OSD_V4_TEXT_LENGTH]; /*!< OSD的文字内容,编码必须为unicode */
	// OSD文字内容说明 
	// ^表示换行
	// [FPS]表示帧率
	// [BPS]表示码率
	// [RES] 表示分辨率
	char szReserved[128];
} HY_DVR_OSD_CHANNEL_V4;

typedef struct tagHY_DVR_OSD_TIME_CHANNEL_V4
{
	int bValid;
	int bEnable;										/*!< OSD 时间是否启用 */
	int nX;												/*!< OSD 时间位置x坐标 */
	int nY; 											/*!< OSD 时间位置y坐标 */
	char szReserved[256];
} HY_DVR_OSD_TIME_CHANNEL_V4;

typedef struct tagHY_DVR_OSD_BMP_V4
{
	char cValid;

	unsigned char ucFontSize;                            /*!< OSD字体大小，只对客户端有用 */
	unsigned int uiBmpW;								/*!< OSD的点阵宽度 */
    unsigned int uiBmpH;								/*!< OSD的点阵高度 */
	unsigned int uiOSDBmpLen;							/*!< OSD点阵内容的长度 */
	char *pcOSDBmp;										/*!< OSD点阵内容的数据 */
}HY_DVR_OSD_BMP_V4, *LPHY_DVR_OSD_BMP_V4;

/*!
*  @struct	tagHY_DVR_OSD_V4
*  @brief	V4的OSD配置, 用于获取，设置一堆OSD,在V2上扩充支持的OSD数量
*/
typedef struct tagHY_DVR_OSD_V4
{
	int bValid;
	int nChannel;												/*!< 通道号 */
	HY_DVR_OSD_TIME_CHANNEL_V4 struOSDTime;						/*!< OSD时间 */
	HY_DVR_OSD_CHANNEL_V4 struOSD[DVR_MAX_OSD_V4_CNT];			/*!< 最多支持8个OSD */
	HY_DVR_OSD_CHANNEL_V4 struOSDName[DVR_MAX_OSD_V4_CNT];		/*!< 最多支持8个OSD名称 */
	HY_DVR_OSD_BMP_V4 stOSDBMP[DVR_MAX_OSD_V4_CNT];				/*!< 最多支持8个OSDBmp */
	char cOSDKind;                                               /*!< OSD种类，0：文字OSD，1：图片OSD */

	char szReserved[92];
}HY_DVR_OSD_V4, *LPHY_DVR_OSD_V4;



/*!
*  @struct	tagHY_DVR_DDNS
*  @brief	DDNS协议信息结构
 */
typedef struct tagHY_DVR_DDNS
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */

	int bDdnsEnable;											/*!< 0-不启用自动注册, 1-启用自动注册  */
	int nDdnsState;												/*!< 目前DDNS只读，0表示已经停止，1表示正在启动，2表示已经启动，3表示正在停止 */
	char szProtocolName[DVR_MAX_DDNSPRONAME_LEN];				/*!< 协议名称(只读)  */
	char szUserName[DVR_MAX_USERNAME_LEN];						/*!< 注册用户名  */
	char szUserPassword[DVR_MAX_PASSWORD_LEN];					/*!< 注册用户密码  */
	char szDomainName[DVR_MAX_DOMAINNAME_LEN];					/*!< 注册用户域名  */
	unsigned short wRegisterPort;								/*!< 服务端口  */
	int nAutoRegIntervals;										/*!< 自动注册间隔时间，单位为秒  */

} HY_DVR_DDNS, *LPHY_DVR_DDNS;

/*!
*  @struct	tagHY_DVR_DDNS_CONF
*  @brief	DDNS功能配置结构
 */
typedef struct tagHY_DVR_DDNS_CONF
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */

	int nDdnsProtocolCount;										/*!< 支持的DDNS协议总数(只读) */
	HY_DVR_DDNS struDdnsProtocol[DVR_MAX_DDNSPRO_NUM];			/*!< DDNS协议信息 */
	int bEnable;												// 是否启用DDNS
} HY_DVR_DDNS_CONF, *LPHY_DVR_DDNS_CONF;

/*!
*  @struct	tagHY_DVR_PPPOE_CONF
*  @brief	PPPoE功能配置信息
 */
typedef struct tagHY_DVR_PPPOE_CONF
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */

	int bPPPoEEnable;											/*!< 0-不启用PPPoE，1-启用 PPPoE  */
	char szPPPoEUser[DVR_MAX_USERNAME_LEN];						/*!< PPPoE用户名  */
	char szPPPoEPassword[DVR_MAX_PASSWORD_LEN];					/*!< PPPoE密码  */

} HY_DVR_PPPOE_CONF, *LPHY_DVR_PPPOE_CONF;

/*!
*  @struct	tagHY_DVR_PPPOE_IF
*  @brief	PPPoE网络信息
 */
typedef struct tagHY_DVR_PPPOE_IF
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */

	int nPPPoEState;											/*!< PPPoE 连接状态(只读),0表示没有连接，1表示正在连接，2表示连接成功，3表示正在挂断 */
	char szPPPoEIP[DVR_MAX_IP_STR_LEN];							/*!< PPPoE IP地址(只读)  */
	char szPPPoESubnetMask[DVR_MAX_IP_STR_LEN];					/*!< PPPoE 子网掩码(只读)  */
	char szPPPoEGateway[DVR_MAX_IP_STR_LEN];					/*!< PPPoE 网关(只读)  */

} HY_DVR_PPPOE_IF, *LPHY_DVR_PPPOE_IF;

/*!
*  @struct	tagHY_DVR_DHCP_CONF
*  @brief	DHCP配置信息
 */
typedef struct tagHY_DVR_DHCP_CONF
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */

	int bDhcpEnable;											/*!< 0-不启用Dhcp,1-启用Dhcp  */

} HY_DVR_DHCP_CONF, *LPHY_DVR_DHCP_CONF;

/*!
*  @struct	tagHY_DVR_DHCP_STATE
*  @brief	DHCP状态信息
 */
typedef struct tagHY_DVR_DHCP_STATE
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */

	int nDhcpState;												/*!< Dhcp状态 (只读)，目前不支持 */

} HY_DVR_DHCP_STATE, *LPHY_DVR_DHCP_STATE;

/*!
*  @struct	tagHY_DVR_DNS_CONF
*  @brief	DNS配置信息
 */
typedef struct tagHY_DVR_DNS_CONF
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */

	char szPrimaryDNS[DVR_MAX_IP_STR_LEN];						/*!< 主DNS服务器  */
	char szSecondaryDNS[DVR_MAX_IP_STR_LEN];					/*!< 备用DNS  */

} HY_DVR_DNS_CONF, *LPHY_DVR_DNS_CONF;

/*!
*  @struct	tagHY_DVR_ETH_IF
*  @brief	ETH0配置结构
 */
typedef struct tagHY_DVR_ETH_IF
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */

	char szIPAddress[DVR_MAX_IP_STR_LEN];						/*!< DVR IP地址  */
	char szSubnetMask[DVR_MAX_IP_STR_LEN];						/*!< DVR 子网掩码  */
	char szGateway[DVR_MAX_IP_STR_LEN];							/*!< 网关地址  */
	char szMacAddress[DVR_MAX_MAC_STR_LEN];						/*!< 只读：服务器的物理地址  */

} HY_DVR_ETH_IF, *LPHY_DVR_ETH_IF;

/*!
*  @struct	tagHY_DVR_WLAN_STATE
*  @brief	WIFI状态
*/
typedef struct tagHY_DVR_WLAN_STATE
{
	int bValid;												/*!< 本结构体是否有效，0表示无效 */

	int bEnable;											/*!< WIFI状态 0 不启用 1 启用, 2 网卡未找到*/

} HY_DVR_WLAN_STATE, *LPHY_DVR_WLAN_STATE;

/*!
*  @enum	eWhichNetcardUsed
*  @brief	网卡类型
*/
typedef enum
{
	eNETCARD_WIRED_Unknown = 0,
	eNETCARD_WIRED_1 = 1,		// 有线网卡1
	eNETCARD_WIRED_2 = 2,			// 有线网卡2
	eNETCARD_WIFI = 3,			// 无线网卡
	eNETCARD_3G = 4,			// 3G网络
	eNETCARD_4G = 5,			// 4G网络
	eNETCARD_5G = 6			// 5G网络
}eWhichNetcardUsed;

typedef enum
{
	eAPMODE_Unknown,				//未定义模式
	eAPMODE_STA,					//STA模式
	eAPMODE_AP,						//AP模式
}eWhichAPMode;

/*!
*  @struct	tagHY_DVR_WLAN_STATE
*  @brief	当前系统中使用哪个网络接口进行通讯
*           暂仅用ONVIF协议探测时给那个接口的IP地址
*/
typedef struct tagHY_DVR_WHICH_NETCARD_USED
{
	int bValid;												/*!< 本结构体是否有效，0表示无效 */

	int dwCardIndex;										/*!< 参见eWhichNetcardUsed */

	int dwAPMode;											/*!< 参见eWhichAPMode */

	char cResvered[252];									// 保留
} HY_DVR_WHICH_NETCARD_USED, *LPHY_DVR_WHICH_NETCARD_USED;


/*!
*  @struct	tagHY_DVR_WLAN_ACCESSPOINT
*  @brief	WIFI热点
*/
typedef struct tagHY_DVR_WLAN_ACCESSPOINT
{
	int bValid;														/*!< 本结构体是否有效，0表示无效 */

	char szSSID[DVR_MAX_SSID_LEN];									/*!< 热点名称 */
	char szMacAddress[DVR_MAX_MAC_STR_LEN];							/*!< 热点MAC地址 */
	int nSignal;													/*!< 热点信号强度 */
	int nAuthentication;											/*!< 认证模式 0-Open,1-Shared,2-WPAPSK, 3-WPA2PSK, 4-WPANONE(ad-hoc模式),5-WPA,6-WPA2,7,No Password*/
	int nEncrypType;												/*!< 加密模式  0表示NONE 1表示WEP 2表示TKIP 3表示AES */
	int nWirelessMode;												/*!< 1: legacy 11B only 2: legacy 11A only 3: legacy 11a/b/g mixed
																		 4: legacy 11G only 5: 11ABGN mixed    6: 11N only
																		 7: 11GN mixed      8: 11AN mixed      9: 11BGN mixed */

} HY_DVR_WLAN_ACCESSPOINT, *LP_HY_DVR_WLAN_ACCESSPOINT;


/*!
*  @struct	tagHY_DVR_WLAN_ACCESSPOINT
*  @brief	WIFI热点
*/
typedef struct tagHY_DVR_WLAN_ACCESSPOINT_SSID_INFO_V2
{
	int bValid;														/*!< 本结构体是否有效，0表示无效 */

	char szSSID[DVR_MAX_SSID_LEN];									/*!< 热点名称 */
	char szMacAddress[DVR_MAX_MAC_STR_LEN];							/*!< 热点MAC地址 */
	int nSignal;													/*!< 热点信号强度 */
	int nAuthentication;											/*!< 认证模式 0-Open,1-Shared,2-WPAPSK, 3-WPA2PSK, 4-WPANONE(ad-hoc模式),5-WPA,6-WPA2,7,No Password*/
	int nEncrypType;												/*!< 加密模式  0表示NONE 1表示WEP 2表示TKIP 3表示AES */
	int nWirelessMode;												/*!< 1: legacy 11B only 2: legacy 11A only 3: legacy 11a/b/g mixed
																		 4: legacy 11G only 5: 11ABGN mixed    6: 11N only
																		 7: 11GN mixed      8: 11AN mixed      9: 11BGN mixed */
	char cResvered[128];											// 保留
} HY_DVR_WLAN_ACCESSPOINT_SSID_INFO_V2, *LP_HY_DVR_WLAN_ACCESSPOINT_SSID_INFO_V2;


/*!
*  @struct	tagHY_DVR_WLAN_ACCESSPOINT_CFG
*  @brief	WIFI热点配置
*/
typedef struct tagHY_DVR_WLAN_ACCESSPOINT_CFG
{
	int bValid;														/*!< 本结构体是否有效，0表示无效 */

	HY_DVR_WLAN_ACCESSPOINT struAccessPoints[DVR_MAX_AP_NUM];		/*!< WIFI热点 */

} HY_DVR_WLAN_ACCESSPOINT_CFG, *LP_HY_DVR_WLAN_ACCESSPOINT_CFG;

/*!
*  @struct	tagHY_DVR_WLAN_ACCESSPOINT_CFG_V2
*  @brief	WIFI热点配置V2
*/
typedef struct tagHY_DVR_WLAN_ACCESSPOINT_CFG_V2
{
	int bValid;														/*!< 本结构体是否有效，0表示无效 */

	HY_DVR_WLAN_ACCESSPOINT struAccessPoints[DVR_MAX_AP_NUM_V2];	/*!< WIFI热点 */

} HY_DVR_WLAN_ACCESSPOINT_CFG_V2, *LP_HY_DVR_WLAN_ACCESSPOINT_CFG_V2;


/*!
*  @struct	tagHY_DVR_WLAN_ACCESSPOINT_CFG
*  @brief	WIFI热点配置
*/
typedef struct tagHY_DVR_WLAN_ACCESSPOINT_CFG_SSID_INFO_V2
{
	int bValid;														/*!< 本结构体是否有效，0表示无效 */

	HY_DVR_WLAN_ACCESSPOINT_SSID_INFO_V2 struAccessPoints[DVR_MAX_AP_NUM_V2];		/*!< WIFI热点 */
	char cResvered[256];									// 保留	
} HY_DVR_WLAN_ACCESSPOINT_CFG_SSID_INFO_V2, *LP_HY_DVR_WLAN_ACCESSPOINT_CFG_SSID_INFO_V2;


/*!
*  @struct	tagHY_DVR_WLAN_KEY
*  @brief	WIFI密钥
*/
typedef struct tagHY_DVR_WLAN_KEY
{
	int bValid;														/*!< 本结构体是否有效，0表示无效 */

	// 密码
	char szKey[DVR_MAX_WIFI_KEY_LEN];

	// 密钥类型 0-表示16进制 1表示字符串类型
	int nKeyType;
} HY_DVR_WLAN_KEY, *LPHY_DVR_WLAN_KEY;

/*!
*  @struct	tagHY_DVR_WLAN_KEY_CFG
*  @brief	WIFI密钥配置
*/
typedef struct tagHY_DVR_WLAN_KEY_CFG
{
	int bValid;														/*!< 本结构体是否有效，0表示无效 */

	int nDefKeyID;													/*!< WEP认证当前用的KEY的索引,1~4 */
	HY_DVR_WLAN_KEY Key1;											/*!< WEP KEY1 */
	HY_DVR_WLAN_KEY Key2;											/*!< WEP KEY2 */
	HY_DVR_WLAN_KEY Key3;											/*!< WEP KEY3 */
	HY_DVR_WLAN_KEY Key4;											/*!< WEP KEY4 */
	HY_DVR_WLAN_KEY WPAKey;											/*!< WPA KEY */
} HY_DVR_WLAN_KEY_CFG, *LPHY_DVR_WLAN_KEY_CFG;

/*!
*  @struct	tagHY_DVR_WLAN_CONNECTION
*  @brief	WIFI连接配置
*/
typedef struct tagHY_DVR_WLAN_CONNECTION
{
	int bValid;														/*!< 本结构体是否有效，0表示无效 */

	int bConnect;													/*!< 是否连接上：0表示未连接上 1表示连接上(只读) */
	char szSSID[DVR_MAX_SSID_LEN];									/*!< 热点名称 */
	char szMacAddress[DVR_MAX_MAC_STR_LEN];							/*!< 热点MAC地址 */
	int nNetworkType;												/*!< 网络类型: 0表示Managed, 1表示Ad-hoc */
	int nAuthMode;													/*!< 认证模式: 0-OPEN,1-SHARED,2-WEPAUTO,3-WPAPSK,4-WPA2PSK,5-WPANONE,6-WPA,7-WPA2*/
	int nEncrypType;												/*!< 加密算法 0-NONE,1-WEP,2-TKIP,3-AES */
	HY_DVR_WLAN_KEY_CFG struKeyCfg;									/*!< 密码配置 */

} HY_DVR_WLAN_CONNECTION, *LP_HY_DVR_WLAN_CONNECTION;

/*!
*  @struct	tagHY_DVR_WLAN_CONNECTION
*  @brief	WIFI连接配置
*/
typedef struct tagHY_DVR_WLAN_CONNECTION_V2
{
	int bValid;														/*!< 本结构体是否有效，0表示无效 */

	int bConnect;													/*!< 是否连接上：0表示未连接上 1表示连接上(只读) */
	char szSSID[DVR_MAX_SSID_LEN];									/*!< 热点名称 */
	char szMacAddress[DVR_MAX_MAC_STR_LEN];							/*!< 热点MAC地址 */
	int nNetworkType;												/*!< 网络类型: 0表示Managed, 1表示Ad-hoc */
	int nAuthMode;													/*!< 认证模式: 0-OPEN,1-SHARED,2-WEPAUTO,3-WPAPSK,4-WPA2PSK,5-WPANONE,6-WPA,7-WPA2*/
	int nEncrypType;												/*!< 加密算法 0-NONE,1-WEP,2-TKIP,3-AES */
	HY_DVR_WLAN_KEY_CFG struKeyCfg;									/*!< 密码配置 */
    int nSignal;													/*!< 热点信号强度 */
	char nReserve[252];
} HY_DVR_WLAN_CONNECTION_V2, *LP_HY_DVR_WLAN_CONNECTION_V2;


/*!
*  @struct  tagHY_DVR_NETCFG_WLAN_CONNECTION_INFO_EX
*  @brief   WIFI连接拓展配置信息
*  @explain  app在预览时需要获取连接的wifi的名称和信号强度，若使用原先HY_DVR_WLAN_CONNECTION_V2结构体，
*           有部分字段不需要且在获取时比较耗时，影响预览效果，显示有卡顿，为此单独新增接口和结构体用来获取/设置连接的wifi的名称和信号强度
*/
typedef struct tagHY_DVR_NETCFG_WLAN_CONNECTION_INFO_EX
{
    int iValid;                                                     /*!< 本结构体是否有效，0表示无效 */
 
    int iWifiSignalStrength;                                        /*!< Wifi热点信号强度;取值0-100，默认值100 */
    char acWifiName[DVR_MAX_SSID_LEN];                              /*!< 热点名称 */
	char acBFJAPWiFiPassword[16];                                   /*!< ap热点密码，贝福佳定制字段 */
     
    char acReserve[256 - 16];
} HY_DVR_NETCFG_WLAN_CONNECTION_INFO_EX, *LPHY_DVR_NETCFG_WLAN_CONNECTION_INFO_EX;

/*!
*  @struct  tagHY_DVR_NETCFG_SSID_INFO
*  @brief   指定ssid的信息
*  @explain  之前通过wifi列表获取信号强度，但是探测会一直刷新wifi列表，会导致设备cpu过高，使用的时候需要传入wifiname
*/
typedef struct tagHY_DVR_NETCFG_SSID_INFO
{
    int iValid;                                                     /*!< 本结构体是否有效，0表示无效 */
 
    int iWifiSignalStrength;                                        /*!< Wifi热点信号强度;20，40，60，80，100  */
    char acWifiName[DVR_MAX_SSID_LEN];                              /*!< 热点名称 */
     
    char acReserve[256];
} HY_DVR_NETCFG_SSID_INFO, *LPHY_DVR_NETCFG_SSID_INFO;

/*!
*  @struct	tagHY_DVR_UPNP_CONF
*  @brief	UPNP配置
*/
typedef struct
{
	int bValid;														/*!< 本结构体是否有效，0表示无效	*/

	int bEnable;													/*!< UPNP控制 0 不启用 1 启用		*/
	char szWanAddress[DVR_MAX_IP_STR_LEN];							/*!< 出口地址(只读)					*/
	int nLocalHttpPort;												/*!< 本地HTTP端口					*/
	int nExportHttpPort;											/*!< 出口HTTP端口					*/
	int nHttpPortState;												/*!< HTTP状态 1 成功 0 失败 2 映射中*/
	int nLocalMobilePort;											/*!< 本地手机端口					*/
	int nExportMobilePort;											/*!< 出口手机端口					*/
	int nMobilePortState;											/*!< 手机状态 1 成功 0 失败 2 映射中*/
	int nLocalCommandPort;											/*!< 本地信令端口					*/
	int nExportCommandPort;											/*!< 出口信令端口					*/
	int nCommandPortState;											/*!< 信令状态 1 成功 0 失败 2 映射中*/
	int nLocalMediaPort;											/*!< 本地媒体端口					*/
	int nExportMediaPort;											/*!< 出口媒体端口					*/
	int nMediaPortState;											/*!< 媒体状态 1 成功 0 失败 2 映射中*/
	int nReserve[8];												/*!< 保留							*/
} HY_DVR_UPNP_CONF, *LPHY_DVR_UPNP_CONF;

////////////////////////////////////////////////////////////////////////////

/*!
*  @struct	tagHY_DVR_FTP_CONF
*  @brief	FTP配置
*/
typedef struct
{
	int bValid;														/*!< 本结构体是否有效，0表示无效	*/

	int bEnable;													/*!< FTP控制 0 不启用 1 启用		*/
	char szUserName[DVR_MAX_USERNAME_LEN];						/*!< FTP用户名  */
	char szUserPassword[DVR_MAX_PASSWORD_LEN];					/*!< FTP用户密码  */
	char szIPAddress[DVR_MAX_IP_STR_LEN];					/*!< FTP服务器IP  */
	unsigned short wFtpPort;								/*!< FTP服务端口  */
	char cReserve[2];										/*!< 保留字段*/
	int iFtpState;											/*!< FTP 状态  0表示没有连接，1表示正在连接，2表示连接成功*/
	char dwReserve[256];										/*!< 保留字段*/

}HY_DVR_FTP_CONF, *LPHY_DVR_FTP_CONF;


#define HY_DVR_FTP_TRANSFER_CONTENT_TYPE_IMAGE	0x01
#define HY_DVR_FTP_TRANSFER_CONTENT_TYPE_VIDEO	0x02
/*!
*  @struct	tagHY_DVR_FTP_CONF
*  @brief	FTP配置
*/
typedef struct
{
	int bValid;														/*!< 本结构体是否有效，0表示无效	*/

	int bEnable;													/*!< FTP控制 0 不启用 1 启用		*/
	char szUserName[DVR_MAX_USERNAME_LEN];						/*!< FTP用户名  */
	char szUserPassword[DVR_MAX_PASSWORD_LEN];					/*!< FTP用户密码  */
	char szIPAddress[DVR_MAX_DOMAIN_STR_LEN];					/*!< FTP服务器IP  */
	unsigned short wFtpPort;								/*!< FTP服务端口  */
	int iFtpState;											/*!< FTP 状态  0表示没有连接，1表示正在连接，2表示连接成功*/
	int nTransferContentTypeMask;							//传输内容的类型掩码,该值是HY_DVR_FTP_TRANSFER_CONTENT_TYPE_IMAGE 和HY_DVR_FTP_TRANSFER_CONTENT_TYPE_VIDEO相或或者相与的结果
	int nFtpMode;											//0,表示主动模式，1表示被动模式
	char cFtpRemoteServerStoragePath[256];					//FTP的录像文件远程服务器存储路径
	char cReserve[764];
}HY_DVR_FTP_CONF_V2, *LPHY_DVR_FTP_CONF_V2;


// 系统配置——网络配置——基本配置
// 虽然将IP地址换成数字表示的地址，节省传输带宽并不明显
typedef enum
{
	eNetworkCardType_10M,
	eNetworkCardType_100M,
	eNetworkCardType_1000M,
}eZWNetworkCardType;

// P2P服务的状态
typedef enum
{
	eP2PStauts_Offline = 0x00,		// 未联机,
	eP2PStauts_LanReady = 0x01,		// 内网就绪,可以使用手机或PC客户端从内网访问设备
	eP2PStauts_InternetReady = 0x02,	// 互联网就绪，可以使用手机或PC客户端从互联网访问设备
	eP2PStauts_NetworkUnreachable = 0x04,	// 网络不通
	eP2PStauts_InvalidUUID = 0x08,		// 非法ID
	eP2PStauts_NotFoundServer = 0x10,	// 找不到服务器
}eZWP2PStatus;

#define DVR_MAX_TUTK_LEVEL_CNT				(10)
typedef struct
{
	unsigned bVaild;

	unsigned dwTutkLevel;				// 对应ENUM_QUALITY_LEVEL

	unsigned dwResolution;				// 对应ResolutionIndex
	unsigned dwFPS;						// 0XFF表示全帧率
	unsigned dwBitrate;					// 码率
}ZW_TUTK_CODEC_MAP_NODE;

typedef struct
{
	unsigned bValid;
	unsigned dwValidLevelCnt;								// 现在应该是5
	ZW_TUTK_CODEC_MAP_NODE list[DVR_MAX_TUTK_LEVEL_CNT];	// 目前一个通道最大支持10个TUTK的等级
}ZW_TUTK_CODEC_MAP_LIST;

typedef struct
{
	unsigned bValid;

	unsigned dwMaxVideoInCnt;			// 具体看什么设备
	ZW_TUTK_CODEC_MAP_LIST map[64];		// 支持64路的配置
}ZW_TUTK_CODEC_MAP;

// AVIOCTRL Quality Type
typedef enum
{
	zwAVIOCTRL_QUALITY_UNKNOWN = 0x00,
	zwAVIOCTRL_QUALITY_MAX = 0x01,	// ex. 640*480, 15fps, 320kbps (or 1280x720, 5fps, 320kbps)
	zwAVIOCTRL_QUALITY_HIGH = 0x02,	// ex. 640*480, 10fps, 256kbps
	zwAVIOCTRL_QUALITY_MIDDLE = 0x03,	// ex. 320*240, 15fps, 256kbps
	zwAVIOCTRL_QUALITY_LOW = 0x04, // ex. 320*240, 10fps, 128kbps
	zwAVIOCTRL_QUALITY_MIN = 0x05,	// ex. 160*120, 10fps, 64kbps
}eVideoQualtityLevel;

typedef struct
{
	unsigned bValid;

	unsigned dwVideoQulity;						//参见eVideoQualtityLevel值
	char cResvered[256];
}ZW_TUTK_QUALITY_NODE;

typedef struct
{
	unsigned bValid;
	unsigned dwMaxVieoInCnt;
	ZW_TUTK_QUALITY_NODE quality[64];
}ZW_TUTK_QUALITY, *LPZW_TUTK_QUALITY;

typedef struct
{
	unsigned bValid;
	unsigned dwWhichStream;
	char cReserved[256];
}ZW_TUTK_STREAMINDEX_NODE;

typedef struct
{
	unsigned bValid;
	unsigned dwMaxVideoInCnt;
	ZW_TUTK_STREAMINDEX_NODE stream[64];
}ZW_TUTK_STREAMINDEX, *LPZW_TUTK_STREAMINDEX;

typedef struct tagZW_DEFAULT_ROUTE_CFG
{
	int bValid;					// 是否有效
	char  szDevice[8];			// 默认路由
}ZW_SYS_NET_DEFAULT_ROUTE_CFG;

typedef struct tagZW_NETCARD_CFG
{
	int	  bValid;				// 是否有效

	DWORD dwEnableDHCP;			// 是否启用DHCP
	DWORD dwDHCPStatus;			// DHCP状态[只读]

	DWORD dwIPAddress;			// IP地址
	DWORD dwGateway;			// 网关
	DWORD dwSubNetMask;			// 子网掩码
	char  bytMAC[DVR_MAX_MAC_STR_LEN];			// MAC地址[只读]
	char  bytReserved_padding[2];				// 保留,字符对齐

	char  bytReserved[16];		// 保留
}ZW_SYS_NET_NETCARD_CFG;

typedef struct tagZW_SYS_NET_CFG
{
	int bValid;																		/*!< 本结构体是否有效，0表示无效  */

	eZWNetworkCardType dwNetworkCardType;		// 网络类型
	DWORD dwEnableDHCP;						// 是否启用DHCP

	DWORD dwIPAddress;						/*!< DVR IP地址  */
	DWORD dwSubnetMask;						/*!< DVR 子网掩码  */
	DWORD dwGateway;							/*!< 网关地址  */
	char szMacAddress[DVR_MAX_MAC_STR_LEN];			/*!< 只读：服务器的物理地址  */

	DWORD bAutoObtainDNS;					// 是否自动获取DNS
	DWORD dwPrimaryDNS;						// 主DNS
	DWORD dwSecondaryDNS;					// 次DNS

	unsigned short dwHTTPPort;											// HTTP服务器端口
	unsigned short dwCommandPort;											// 私有协议命令端口
	unsigned short dwMediaPort;											// 私有协议媒体端口
	unsigned short dwMobilePort;											// 私有协议手机端口
	unsigned short dwRtpPort;											//RTP端口
	unsigned short wAutoDetectPort;                            //自动探测端口

	DWORD  dwDHCPStatus;
	//要注意，下面的提供的2种P2P服务，设备同时刻仅支持下面的一种P2P服务
	//TUTK P2P
	char acTutkUID[32];											// 当前设备的TUTK UID
	char bTutkEnable;										// 是否启用TUTK服务
	char acReserved_1[3];
	DWORD dwTutkStatus;								 // 0x0，未联机，0x01，内网就绪，0x02, 外网就绪        // add by hdd 20140226
	//悠落客P2P
	char bUlucoP2PEnable;									//是否支持悠落客P2P	
	char acReserved_2[3];
	char acUlucoP2PDID[32];									//当前设备的悠落客P2P的UID
	int dwUlucoP2PStatus;									//设备上线状态，0,offline,1,Device is Online,-1,Register failed

	char bEnableAutoGenIP;									// 是否开启自动生成IP地址机制．ONVIF协议/私有协议探测时会将自已修改成与探测者一样网段的IP.0,不启用. 1,启用.
	char acReserved_padding[3];								// 字符对齐

	ZW_SYS_NET_NETCARD_CFG			cfgSecondNetcard;			// 第二网卡配置
	ZW_SYS_NET_DEFAULT_ROUTE_CFG	cfgDefaultRoute;
	char	strPushNum[8];
	DWORD   dwDeviceRelyEnable;								// 设备转发使能
	unsigned short HKCommandPortFieldValid;					// HK协议命令端口是否有效
	unsigned short HKCommandPort;							// HK协议命讼端口
	int		nIsEnablePushSubKey;							// 是否使能
	char	strShangyunPushSubKey[20];						// 尚云推送使用的SubKey
	char cIsUseWifiInfo;										//探测包中的信息是否使用wifi配置（IP，MAC等），0：不支持，1：使用有线，2：无线
	char cIsWifiSignalBoostMode;						//Wifi增益模式
	char cWanMtuValid;
	char cWanPPPOEMtuValid;
	char cWanMruValid;
	char cWanAutoDns;										//0：不支持		1：手动		2：自动
	char cNotReplyProbe;										//是否回应探测包
	char reserved[1];
	DWORD nWanMtu;												//WAN MTU
	DWORD nWanPPPOEMtu;										//PPPOEMTU
	DWORD nWanMru;												//WAN MRU
	int iErrCode;
	int iDidGetStatus;
	char	acReserved[32];									// 保留(方便日后扩展)
}ZW_SYS_NET_CFG, *LPZW_SYS_NET_CFG;

#define ZW_NET_CFG_LEN  sizeof(ZW_SYS_NET_CFG)

/*!
*  @struct	tagHY_DVR_NET_EML_1
*  @brief	Email服务器配置信息(1.1.0版本使用)
*/
typedef struct	tagHY_DVR_NET_EML_1
{
	int bValid;													/*!< 本结构体是否有效，0表示无效		*/
	int bEmlEnable;												/*!< 0-不启用, 1-启用					*/

	char szSmtpServer[DVR_MAX_DOMAINNAME_LEN];					/*!< smtp服务器地址						*/
	unsigned short wSmtpPort;									/*!< 端口								*/
	int nSmtpAuth;												/*!< 验证方式(0为NONE, 1为LOGIN)		*/
	int nSmtpTLS;												/*!< 是否使用安全登陆(0为off, 1为on(SSL),2,TLS,3.STARTTLS)	*/

	char szSenderEmail[DVR_MAX_EMAIL_LEN];						/*!< 发件人邮箱地址						*/
	char szSenderPassword[DVR_MAX_PASSWORD_LEN];				/*!< 发件人邮箱密码						*/

	char szReceiverAddress_1[DVR_MAX_EMAIL_LEN];
	char szReceiverAddress_2[DVR_MAX_EMAIL_LEN];
	char szReceiverAddress_3[DVR_MAX_EMAIL_LEN];
	char szReceiverAddress_4[DVR_MAX_EMAIL_LEN];
	char szReceiverAddress_5[DVR_MAX_EMAIL_LEN];

	int  nWithSnapshot;											// 是否附带抓图

	//以下参数暂不使用
	int nProtocol;												/*!< (目前为smtp)						*/
	int nTimeout;												/*!< 超时配置							*/
	char nConfigMode;											/*!< 配置模式(0为自定义, 1为简化自动模式)*/
	char nReserve[3];

	//以下为TLS相关高级参数，暂不使用
	int nTls_starttls;
	int nTls_certcheck;
	int nTls_force_sslv3;
	int nTls_min_dh_prime_bits;
	int nTls_priorities;

} HY_DVR_NET_EML_1, *LPHY_DVR_NET_EML_1;

/*!
*  @brief	谁探测的．
*/

typedef struct
{
	int   bValid;
	char  szIPAddress[DVR_MAX_IP_STR_LEN];		/*!<ipv4地址                                   */
	char  cReserved[256];
} HY_DVR_WHOPROBE_CONF, *LPHY_DVR_WHOPROBE_CONF;

/*!
*  @struct	tagHY_DVR_NET_CFG
*  @brief	本地网络适配器配置结构
 */
typedef struct tagHY_DVR_NET_CFG
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */


	HY_DVR_PPPOE_CONF struPPPoE_Conf;							/*!< PPPoE配置 */
	HY_DVR_PPPOE_IF struPPPoE_IF;								/*!< PPPoE网络信息 */
	HY_DVR_DDNS_CONF struDDNS;									/*!< DDNS配置信息 */
	HY_DVR_DHCP_CONF struWIFIDhcp_Conf;							/*!< WIFI DHCP配置 */
	HY_DVR_DHCP_STATE struWIFIDhcp_State;						/*!< WIFI Dhcp状态信息 */
	HY_DVR_ETH_IF struWIFICommon;								/*!< WIFI 网络信息 */
	HY_DVR_DNS_CONF struWIFIDNS;								/*!< WIFI DNS配置信息 */
	HY_DVR_WLAN_ACCESSPOINT_CFG struWIFIAccessPoint;			/*!< WIFI热点配置 */
	HY_DVR_WLAN_CONNECTION struWIFIConnectCfg;					/*!< WIFI连接配置 */
	HY_DVR_WLAN_STATE struWIFIState;							/*!< WIFI 状态 */
	HY_DVR_UPNP_CONF	struUPNPConf;							/*!< UPNP配置信息				*/

	ZW_SYS_NET_CFG struBasicNetCfg;										/*!< 基本网络配置结构体,设备端应新响应这个配置　*/
	HY_DVR_NET_EML_1 struEmlCfg;
} HY_DVR_NET_CFG, *LPHY_DVR_NET_CFG;


/** @brief 本地网络适配器配置结构长度																		 */
#define NET_CFG_LEN   sizeof(HY_DVR_NET_CFG)
////////////////////////////////////////////////////////////////////////////

/*!
*  @struct	tagHY_DVR_WLAN_STATE_FOR_TEST
*  @brief	WIFI状态
*/
typedef struct tagHY_DVR_WLAN_STATE_FOR_TEST
{
	int bValid;												/*!< 本结构体是否有效，0表示无效 */

	int bWifiStatus;										/*!< WIFI状态 0 表示没有连接，1,表示连接成功,2.用户名或者密码错误*/

} HY_DVR_WLAN_STATE_FOR_TEST, *LPHY_DVR_WLAN_STATE_FOR_TEST;


////////////////////////////////////////////////////////////////////////////
//设备支持的网络协议宏
//
#define HY_DVR_NETWORK_PROTOCOL_HYPRIVATE	0x01	//TUTK
#define HY_DVR_NETWORK_PROTOCOL_TIDEMOBILE	0x02	//悠落客
#define HY_DVR_NETWORK_PROTOCOL_MEGAEYES	0x04
#define HY_DVR_NETWORK_PROTOCOL_RTSP	0x08
#define HY_DVR_NETWORK_PROTOCOL_ONVIF	0x10
#define HY_DVR_NETWORK_PROTOCOL_TUTK	0x20
#define HY_DVR_NETWORK_PROTOCOL_JSTP	0x40
#define HY_DVR_NETWORK_PROTOCOL_CGI		0x80
#define HY_DVR_NETWORK_PROTOCOL_WXGATEWAY	0x100
#define HY_DVR_NETWORK_PROTOCOL_TIANYI		0x200

#define HY_DVR_NETWORK_PROTOCOL_MEGLINK		0x400

#define HY_DVR_NETWORK_PROTOCOL_FLOWCLIENT	0x800

#define HY_DVR_NETWORK_PROTOCOL_GB28181			0x1000

#define HY_DVR_NETWORK_PROTOCOL_CMDOT			0x2000
#define HY_DVR_NETWORK_PROTOCOL_TIANYIV2		0x4000
#define HY_DVR_NETWORK_PROTOCOL_LIONKING		0x8000
#define HY_DVR_NETWORK_PROTOCOL_JGPLATFORM		0x10000
#define HY_DVR_NETWORK_PROTOCOL_CMCCOVD		0x20000
#define HY_DVR_NETWORK_PROTOCOL_CUIOT_LTYF	0x40000
#define HY_DVR_NETWORK_PROTOCOL_ZWEBSOCKET		0x80000
#define HY_DVR_NETWORK_PROTOCOL_P6SMESSAGEPUSH		0x100000
#define HY_DVR_NETWORK_PROTOCOL_DAJIANGIOT		0x200000

/*!
*  @struct	tagVIDEONETCLIENT_NET_EML_1_EX
*  @brief	EMAIL扩展配置
 */
typedef struct tagHY_DVR_NET_EML_1_EX
{
	int bValid;
	HY_DVR_SCHEDTIME struEmlSechedTime;
	unsigned int  nEMLTimeInterval;
	char strEmailTopic[32];
	char strEmailMessage[128];
	char cReserve[92];
}HY_DVR_NET_EML_1_EX, *LPHY_DVR_NET_EML_1_EX;
/*!
*  @struct	tagHY_DVR_NET_CFG_EX
*  @brief	本地网络适配器扩展配置结构
 */
typedef struct tagHY_DVR_NET_CFG_EX
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */
	HY_DVR_NET_EML_1_EX struEmlCfgEx;
	DWORD dwIsSupportWifi;										/*!<0，表示不支持wifi，1，表示支持wifi */
	DWORD dwNetworkCardSelect;									/*!<0，表示选择了有线网卡，1，表示选择wifi */
	DWORD dwNetworkWifiDNSEnable;								/*!<0，表示不使能wifi，1，表示使能wifi */
	DWORD dwNetworkProtocolSupport;								/*!<设备是否支持网络协议掩码，0，表示不支持，1，表示支持 */
	DWORD dwNetworkProtocolMask;								/*!<设备支持的网络协议 */
	char cAppDownloadURL[256];									/*!<APP下载URL地址 */
	DWORD dwIsSupportAutoIP;
	DWORD dwWifiMode;											/*!<0,none,1,sta,2,AP模式 */
	DWORD dwIsSupportNetCardName;								//是否支持网卡名字
	char strNetworkCardName[32];								/*!< 网卡名字 */
	DWORD dwIsWifiEnable;                                       /*!<0，表示不使能wifi，1，表示使能wifi */
	DWORD dwWifiSignalEnhanceEnable;							/*!<无线信号增强开关；0，表示关，1，表示开 */
	char cReserve[1724 - 4];										/*!< 保留*/
} HY_DVR_NET_CFG_EX, *LPHY_DVR_NET_CFG_EX;

/** @brief 本地网络适配器扩展配置结构长度																		 */
#define NET_CFG_EX_LEN   sizeof(HY_DVR_NET_CFG_EX)


////////////////////////////////////////////////////////////////////////////

/*!
*  @struct	tagHY_DVR_NET_CMS
*  @brief	中心管理服务器配置信息
 */
typedef struct tagHY_DVR_NET_CMS
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */

	int bCmsEnable;												/*!< 0-不启用管理服务器, 1-启用  */
	char szManageHostAddress[DVR_MAX_DOMAINNAME_LEN];			/*!< 远程管理主机地址，长度考虑域名  */
	unsigned short wManageHostPort;								/*!< 远程管理主机端口  */
	int nAutoRegIntervals;										/*!< 注册间隔时间，单位为秒  */

} HY_DVR_NET_CMS, *LPHY_DVR_NET_CMS;

/*!
*  @struct	tagHY_DVR_CMS_STATE
*  @brief	中心管理服务器连接状态
 */
typedef struct tagHY_DVR_CMS_STATE
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */

	int nCmsState;												/*!< CMS注册状态,目前系统不支持  */

} HY_DVR_CMS_STATE, *LPHY_DVR_CMS_STATE;

/*!
*  @struct	tagHY_DVR_NET_AMS
*  @brief	报警管理服务器配置信息
 */
typedef struct tagHY_DVR_NET_AMS
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */

	int bAmsEnable;												/*!< 0-不启用报警服务器, 1-启用  */
	char szAlarmHostAddress[DVR_MAX_DOMAINNAME_LEN];			/*!< 报警服务器 */
	unsigned short wAlarmHostPort;								/*!< 报警服务器端口  */

} HY_DVR_NET_AMS, *LPHY_DVR_NET_AMS;

/*!
*  @struct	tagHY_DVR_NET_NTP
*  @brief	Ntp服务器配置信息
 */
typedef struct tagHY_DVR_NET_NTP
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */

	int bNtpEnable;												/*!< 0-不启用, 1-启用  */
	char szNtpHostAddress[DVR_MAX_DOMAINNAME_LEN];				/*!< Ntp服务器，长度考虑域名  */
	unsigned short wNtpHostPort;								/*!< Ntp服务器，端口  */
	int nAutoRegIntervals;										/*!< Ntp服务器同步间隔时间，单位为秒  */

} HY_DVR_NET_NTP, *LPHY_DVR_NET_NTP;

/*!
*  @struct	tagVIDEONETCLIENT_CMS_FOR_PROTOCOL
*  @brief	CMS对应的协议名称
*/
typedef struct tagHY_DVR_CMS_FOR_PROTOCOL
{
	int bValid;											//本结构体是否有效，=0表示无效

	DWORD dwCMSProtocol;								/*!< 协议名称：在CMS中需要填写IP地址，需要告诉设备端，这个
														CMS中的IP地址是为哪一个协议准备的，如果是私有协议，那么设备
														将注册到NVMS中；如果是Hxht协议，则设备注册到Hxht的接入服务器中。
														该参数只能表示一种协议。*/
}HY_DVR_CMS_FOR_PROTOCOL, *LPHY_DVR_CMS_FOR_PROTOCOL;


/*!
*  @struct	tagHY_DVR_NET_CLOUD_SERVER
*  @brief	云平台服务器配置信息
 */
typedef struct tagHY_DVR_NET_CLOUD_SERVER
{
	int bValid;													/*!< 本结构体是否有效，0表示无效		*/

	int bEnable;												/*!< 0-不启用, 1-启用					*/
	char szCloudServerAddress[DVR_MAX_DOMAINNAME_LEN];			/*!< 云平台服务器地址，长度考虑域名		*/
	int dwCloudServerPort;										/*!< 云平台服务器端口					*/
	char szUserName[DVR_MAX_USERNAME_LEN];						/*!< 云平台服务器用户名					*/
	char szPassword[DVR_MAX_PASSWORD_LEN];						/*!< 云平台服务器密码					*/
	char cReserve[DVR_RESERVE_LEN];								/*!< 保留字段							*/
} HY_DVR_NET_CLOUD_SERVER, *LPHY_DVR_NET_CLOUD_SERVER;

////////////////////////////////////////////////////////////////////////////
/*!
*  @struct	tagHY_DVR_NET_MANAGER
*  @brief	网络管理结构
 */
typedef struct tagHY_DVR_NET_MANAGER
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */

	HY_DVR_NET_CMS struCMS;										/*!< 中心管理服务器信息 */
	HY_DVR_NET_AMS struAMS;										/*!< 报警服务器信息 */
	HY_DVR_CMS_FOR_PROTOCOL struCMSForProtocol;					/*!< 中心管理服务器信息对应的协议（和CMS是绑定的）*/
	HY_DVR_CMS_STATE struCmsState;								/*!< 中心管理服务器连接状态 */
} HY_DVR_NET_MANAGER, *LPHY_DVR_NET_MANAGER;

/** @brief 网络管理结构长度																				 */
#define NET_MANAGER_LEN   sizeof(HY_DVR_NET_MANAGER)
////////////////////////////////////////////////////////////////////////////




typedef struct tagHY_MOTION_EVENT_CFG
{
	int bValid;
	int nPostRecordTime;
	int nPostAlarmInterval;
	char szHttpServerMessage[256];
}HY_MOTION_EVENT_CFG;


typedef struct tagHY_HTTP_EVENT_CFG
{
	int bValid;
	int nPostRecordTime;
	int nPostAlarmInterval;
	char szHttpServerMessage[256];
}HY_HTTP_EVENT_CFG;


typedef struct tagHY_EVENT_CFG_LIST
{
	int bValid;
	HY_MOTION_EVENT_CFG struMotionEventCfg;
	HY_HTTP_EVENT_CFG struHttpEventCfg;
	int nProtocolType;
	char cReserve1[1020];
	char cReserve2[1024];
}HY_EVENT_CFG_LIST;



#define MAX_SUPPORT_SUBSCRIBE_EVENT_COUNT	 10
#define MAX_SUPPORT_UNSUBSCRIBE_EVENT_COUNT  10

typedef struct tagHY_EVENT_CFG_EX
{
	int bValid;
	int nEventType;			//事件类型，其值的含义暂定
	int nChannelMask1;		//通道0-31
	int nChannelMask2;		//通道32-63
	char cReserve[32];
}HY_EVENT_CFG_EX, *LPHY_EVENT_CFG_EX;

typedef struct tagHY_EVENT_CFG_EX_LIST
{
	int bValid;
	HY_EVENT_CFG_EX struEventCfgExList[MAX_SUPPORT_SUBSCRIBE_EVENT_COUNT];
}HY_EVENT_CFG_EX_LIST, *LPHY_EVENT_CFG_EX_LIST;

//订阅（服务器名字和URL共同确定一个订阅事件,在订阅的时候，如果遇到相同的服务器名字和URL，自动覆盖）
typedef struct tagHY_EVENT_OBJECT_INFO
{
	char szServerName[64];
	char szServerHostURL[256];
	char szServerUserName[64];
	char szServerPassword[64];
	HY_EVENT_CFG_EX_LIST struEventCfgExList;
}HY_EVENT_OBJECT_INFO, *LPHY_EVENT_OBJECT_INFO;



/*!
*  @struct	tagHY_DVR_NET_AMS
*  @brief	报警管理服务器配置信息
 */
typedef struct tagHY_DVR_NET_AMS_EX
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */

	int bAmsEnable;												/*!< 0-不启用报警服务器, 1-启用  */
	char szServerName[64];
	char szServerHostURL[256];
	char szServerUserName[64];
	char szServerPassword[64];
	int nServerPort;
	int nServerType;											//服务器类型，在GET和SET的时候，该值都作为输入有效
	char cReserve[248];
} HY_DVR_NET_AMS_EX, *LPHY_DVR_NET_AMS_EX;

////////////////////////////////////////////////////////////////////////////
/*!
*  @struct	tagHY_DVR_NET_MANAGER
*  @brief	网络管理结构
 */
typedef struct tagHY_DVR_NET_MANAGER_EX
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */
	HY_DVR_NET_AMS_EX struAMSEx;								/*!< 报警扩展服务器信息 */
	int nProtocolType;
	char cReserver1[1020];
	char cReserver2[1024];
} HY_DVR_NET_MANAGER_EX, *LPHY_DVR_NET_MANAGER_EX;

/** @brief 网络管理结构长度																				 */
#define NET_MANAGER_EX_LEN   sizeof(HY_DVR_NET_MANAGER_EX)

////////////////////////////////////////////////////////////////////////////
//#define RESERVED_PIC_OLD
/*!
*  @struct	tagHY_DVR_CHROMA_SEC
*  @brief	DVR色度单元配置结构
 */
typedef struct tagHY_DVR_CHROMA_SEC
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */

	int bTimeSegEnable;											/*!< 启用时间片  */
	int nStartTime;												/*!< 开始时间 相对00:00时的秒数   */
	int nStopTime;												/*!< 结束时间 相对00:00时的秒数   */
	BYTE nBrightness;											/*!< 亮度,0-255  */
	BYTE cReserve1[3];
	BYTE nContrast;												/*!< 对比度,0-127  */
	BYTE cReserve2[3];
	BYTE nSaturation;											/*!< 饱和度,0-127  */
	BYTE cReserve3[3];
	BYTE nHue;													/*!< 色调,0-255  */
	BYTE cHSharpness;											/*!< 水平锐度,0-15  */
	BYTE cVSharpness;											/*!< 垂直锐度,0-15  */
	BYTE cReserve4[1];

} HY_DVR_CHROMA_SEC, *LPHY_DVR_CHROMA_SEC;

/*!
*  @struct	tagHY_DVR_CHROMA_SEC
*  @brief	色度配置结构(每通道支持24个时间段)
 */
typedef struct tagHY_DVR_CHROMA
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */

	HY_DVR_CHROMA_SEC struChromaSec[DVR_MAX_PICSEC_NUM];		/*!< 色度单元 */

} HY_DVR_CHROMA, *LPHY_DVR_CHROMA;


/*异常处理方式 */

/** @brief 无响应																				 */
#define DVR_ALARM_TRIGGER_NONE			0

/** @brief 触发屏幕显示，通道参数由录像参数决定													 */
#define DVR_ALARM_TRIGGER_PREVIEW		(0x1)

/** @brief 声音警告																				 */
#define DVR_ALARM_TRIGGER_AUDIOOUT		(0x1 << 1)

/** @brief 触发录像																				 */
#define DVR_ALARM_TRIGGER_RECORD		(0x1 << 2)

/** @brief 触发拍照																				 */
#define DVR_ALARM_TRIGGER_SNAPSHOT		(0x1 << 3)

/** @brief 触发报警输出																			 */
#define DVR_ALARM_TRIGGER_ALARMOUT		(0x1 << 4)

/** @brief 上传报警中心																			 */
#define DVR_ALARM_TRIGGER_NOTIFYAMS		(0x1 << 5)

/** @brief 触发PTZ																				 */
#define DVR_ALARM_TRIGGER_PTZ			(0x1 << 6)

/** @brief 发生EMail通知																		 */
#define DVR_ALARM_TRIGGER_EMAIL			(0x1 << 7)

/** @brief 屏幕提示																				 */
#define DVR_ALARM_TRIGGER_NOTIFYGUI		(0x1 << 8)

/** @brief 联动BNC																		 */
#define DVR_ALARM_TRIGGER_BNC			(0x1 << 9)

/** @brief 联动FTP																		 */
#define DVR_ALARM_TRIGGER_FTP			(0x1 << 10)

/** @brief 联动推送																		 */
#define DVR_ALARM_TRIGGER_PUSH			(0x1 << 11)

/** @brief 联动口罩事件																		 */
#define DVR_ALARM_TRIGGER_FACE_MASK		(0x1 << 12)

/** @brief 联动灯光事件																		 */
#define DVR_ALARM_TRIGGER_LIGHTALARM	(0x1 << 13)

/*!
*  @struct	tagHY_DVR_HANDLEEXCEPTION
*  @brief	联动处理结构体
 */
typedef struct tagHY_DVR_HANDLEEXCEPTION
{
	int bValid;													/*!< 本结构体是否有效，0表示无效  */

	DWORD dwHandleType;								/*!< 处理方式,处理方式的"或"结果; 二进制的每一位对应一种异常处理方式,为1表示支持该异常处理方式,反正为0, 对应关系如下(详见DVR_ALARM_TRIGGER_*):
																   无响应: DVR_ALARM_TRIGGER_NONE			0
																   触发屏幕显示: DVR_ALARM_TRIGGER_PREVIEW		(0x1)
																   声音警告: DVR_ALARM_TRIGGER_AUDIOOUT		(0x1 << 1)
																   触发录像: DVR_ALARM_TRIGGER_RECORD		(0x1 << 2)
																   触发拍照: DVR_ALARM_TRIGGER_SNAPSHOT		(0x1 << 3)
																   触发报警输出: DVR_ALARM_TRIGGER_ALARMOUT		(0x1 << 4)
																   上传报警中心: DVR_ALARM_TRIGGER_NOTIFYAMS		(0x1 << 5)
																   触发PTZ: DVR_ALARM_TRIGGER_PTZ			(0x1 << 6)
																   发生EMail通知: DVR_ALARM_TRIGGER_EMAIL			(0x1 << 7)
																   屏幕提示: DVR_ALARM_TRIGGER_NOTIFYGUI		(0x1 << 8)
																   联动BNC: DVR_ALARM_TRIGGER_BNC			(0x1 << 9)
																   联动FTP: DVR_ALARM_TRIGGER_FTP			(0x1 << 10)
																   联动推送: DVR_ALARM_TRIGGER_PUSH			(0x1 << 11)
																   联动口罩事件: DVR_ALARM_TRIGGER_FACE_MASK		(0x1 << 12)
																   联动灯光事件: DVR_ALARM_TRIGGER_LIGHTALARM	(0x1 << 13) */
	int nRecordChannelMask;										/*!< 联动录像通道标识，按位表示，若位0为1，表示通道1被选中，以此类推  */
	int nSnapshotChannelMask;									/*!< 联动快照通道标识，按位表示，若位0为1，表示通道1被选中，以此类推  */
	int nAlarmOutChannelMask;									/*!< 联动报警输出通道标识，按位表示，若位0为1，表示通道1被选中，以此类推  */

	// HDD 以下两个成员改成字符形式，以便于使用单字符串传输，可以大量减少数据量，针对此应用而言，256的范围也足够了。
	unsigned char szPtzAction[DVR_MAX_PTZ_NUM];							/*!< 联动的动作,  0表示无动作 , 1联动预置点, 2联动巡航 3联动轨迹  */
	unsigned char szPtzIndex[DVR_MAX_PTZ_NUM];								/*!< 联动的预置点/巡航/轨迹的编号  */
	int nMonitorMask;											/*!< 0-不在显示器上显示 1-主显 2-SPOT1 4-SPOT2 8-SPOT3 16-SPOT4，若需要多种组合，将这些值相加即可 */

} HY_DVR_HANDLEEXCEPTION, *LPHY_DVR_HANDLEEXCEPTION;

typedef struct tagHY_DVR_PTZ_HANDLE_CHANNEL
{
	int nValid;
	unsigned int nPTZChannelID;		/*!<表示联动处理的通道号，同时最多支持32路的联动*/
	unsigned int nPtzAction;							/*!< 联动的动作,  0表示无动作 , 1联动预置点, 2联动巡航 3联动轨迹  */
	unsigned int nPtzIndex;								/*!< 联动的预置点/巡航/轨迹的编号  */
}HY_DVR_PTZ_HANDLE_CHANNEL;
/*!
*  @struct	tagHY_DVR_HANDLEEXCEPTION_MULTICHANNEL
*  @brief	联动处理结构体,支持设置超过32个通道以上的配置
 */
typedef struct tagHY_DVR_HANDLEEXCEPTION_MULTICHANNEL
{
	int bValid;							/*!< 本结构体是否有效，0表示无效  */

	DWORD dwHandleType;			/*!< 处理方式,处理方式的"或"结果  */
	int nRecordChannelMask[32];			/*!< 联动录像通道标识，按位表示，每个数组成员表示1个32通道，若位0为1，表示通道1被选中，以此类推  */
	int nSnapshotChannelMask[32];		/*!< 联动快照通道标识，按位表示，每个数组成员表示1个32通道，若位0为1，表示通道1被选中，以此类推  */
	int nAlarmOutChannelMask[32];		/*!< 联动报警输出通道标识，每个数组成员表示1个32通道，按位表示，若位0为1，表示通道1被选中，以此类推  */

	HY_DVR_PTZ_HANDLE_CHANNEL struPtzHandle[DVR_MAX_PTZ_NUM];
	int nMonitorMask[32];				/*!< 0-不在显示器上显示 1-主显 2-SPOT1 4-SPOT2 8-SPOT3 16-SPOT4，若需要多种组合，将这些值相加即可 */

} HY_DVR_HANDLEEXCEPTION_MULTICHANNEL, *LPHY_DVR_HANDLEEXCEPTION_MULTICHANNEL;

/*!
*  @struct	tagHY_DVR_MOTION
*  @brief	移动侦测
 */
typedef struct tagHY_DVR_MOTION
{
	int bValid;																		/*!< 本结构体是否有效，0表示无效  */

	int bEnable;																	/*!< 是否处理移动侦测  */
	int nSenstive;																	/*!< 移动侦测灵敏度, 0 - 5,越低越灵敏,0xff关闭  0,highest,1,higher,2,high,3,middle,4,low,5,lower,6.lowest*/
	int nXMaxScope;// HDD,由设备端告诉客户端支持多大的范围，就不用去猜了。
	int nYMaxScope;// HDD,同上
	int nXScope;																	/*!< X方向宏块范围：0-32（目前9000设备支持0-16）  */
	int nYScope;																	/*!< Y方向宏块范围：0-32（目前9000设备支持0-12）*/
	// HDD 数据组大小有改动，注意在序列化时可以改为位压缩的字符串传输
	unsigned char byMotionScope[DVR_MAX_MOTIONSCOPE_NUM][DVR_MAX_MOTIONSCOPE_NUM];	/*!< 侦测区域,1表示改宏块是移动侦测区域,0表示不是  */
	HY_DVR_HANDLEEXCEPTION struHandleType;											/*!< 处理方式  */
	HY_DVR_SCHEDTIME struSchedTime;													/*!< 布防计划  */

} HY_DVR_MOTION, *LPHY_DVR_MOTION;

typedef struct tagHY_DVR_REGION
{
	int bValid;
	int nMotionAreaTopLeftX;														/*!< 马赛克区域的左上点x相对坐标（0-703）  */
	int nMotionAreaTopLeftY;														/*!< 马赛克区域的左上点y相对坐标（0-575）  */
	int nMotionAreaBottomRightX;													/*!< 马赛克区域的右下点x相对坐标（0-703）  */
	int nMotionAreaBottomRightY;													/*!< 马赛克区域的右下点y相对坐标（0-575）  */
}HY_DVR_REGION, *LPHY_DVR_REGION;
/*!
*  @struct	tagHY_DVR_MOTION_V2
*  @brief	移动侦测(坐标表示区域)
 */
typedef struct tagHY_DVR_MOTION_V2
{
	int bValid;																		/*!< 本结构体是否有效，0表示无效  */

	int bEnable;																	/*!< 是否处理移动侦测  */
	int nSenstive;																	/*!< 移动侦测灵敏度, 0 - 5,越低越灵敏,0xff关闭  */
	HY_DVR_REGION struMotionRegion[DVR_MAX_MOTION_REGION_NUM];

	HY_DVR_HANDLEEXCEPTION struHandleType;											/*!< 处理方式  */
	HY_DVR_SCHEDTIME struSchedTime;													/*!< 布防计划  */

} HY_DVR_MOTION_V2, *LPHY_DVR_MOTION_V2;

/*!
*  @struct	tagHY_DVR_MOTION_V2_LIST
*  @brief	移动侦测(坐标表示区域)
 */
typedef struct tagHY_DVR_MOTION_V2_LIST
{
	int bValid;																		/*!< 本结构体是否有效，0表示无效  */

	HY_DVR_MOTION_V2 struMotionV2[DVR_MAX_VIDEOIN_NUM];
} HY_DVR_MOTION_V2_LIST, *LPHY_DVR_MOTION_V2_LIST;

/*!
*  @struct	tagHY_DVR_CMDOT_MOTION_V3
*  @brief	移动侦测(坐标表示区域)
 */
typedef struct tagHY_DVR_CMDOT_MOTION_V3
{
	int bValid;																		/*!< 本结构体是否有效，0表示无效  */

	int bEnable;																	/*!< 是否处理移动侦测  */
	int nSenstive;																	/*!< 移动侦测灵敏度, 0 - 4,越高越灵敏，默认为3*/
	HY_DVR_REGION struMotionRegion[DVR_MAX_CMDOT_MOTION_REGION_NUM_V3];

} HY_DVR_CMDOT_MOTION_V3, *LPHY_DVR_CMDOT_MOTION_V3;

/*!
*  @struct	tagHY_DVR_MOTION_V3_LIST
*  @brief	移动侦测(坐标表示区域)
 */
typedef struct tagHY_DVR_CMDOT_MOTION_V3_LIST
{
	int bValid;																		/*!< 本结构体是否有效，0表示无效  */

	HY_DVR_CMDOT_MOTION_V3 struMotionV3[DVR_MAX_VIDEOIN_NUM];
} HY_DVR_CMDOT_MOTION_V3_LIST, *LPHY_DVR_CMDOT_MOTION_V3_LIST;

/*!
*  @struct	tagHY_DVR_PEOPLE_DETECT
*  @brief	人形侦测(坐标表示区域)
 */
typedef struct tagHY_DVR_PEOPLE_DETECT
{
	int bValid;																		/*!< 本结构体是否有效，0表示无效  */

	int bEnable;																	/*!< 是否处理人形侦测  */
	int nSenstive;																	/*!< 人形侦测灵敏度, 0 - 5,越低越灵敏,0xff关闭  */
	int nValidTrack;																/*!< 跟踪使能参数是否有效  */
	int nEnableTrack;																/*!< 是否使能跟踪功能	   */
	bool nValidRegionMode;															/*!< 人形区域模式参数是否有效  */
	char nRegionMode;																/*!< 人形区域模式		   */
	char nAIH265PlusEnable;
	char nAIFaceEnable;
	char cMainStreamOverlay;													/*!< 主码流人形叠加框		   */
	char cSubStreamOverlay;														/*!< 子码流人形叠加框		   */
	char cIsPushPeopleImage;													/*!< 人形事件是否推图		   */
	char cAlarmInterval;																/*!< 天翼报警间隔		   */
	char cTrackZoom;																	/*!< 人形跟踪放大		   */
	char cReserve[15];															/*!< 保留		*/
	HY_DVR_REGION struPeopDetectRegion[DVR_MAX_PEOPLE_DETECT_REGION_NUM];

	HY_DVR_HANDLEEXCEPTION struHandleType;											/*!< 处理方式  */
	HY_DVR_SCHEDTIME struSchedTime;													/*!< 布防计划  */

} HY_DVR_PEOPLE_DETECT, *LPHY_DVR_PEOPLE_DETECT;

/*!
*  @struct	tagHY_DVR_PEOPLE_DETECT_CFG
*  @brief	人形侦测(坐标表示区域)
 */
typedef struct tagHY_DVR_PEOPLE_DETECT_CFG
{
	int bValid;																		/*!< 本结构体是否有效，0表示无效  */

	HY_DVR_PEOPLE_DETECT struPeopleDetect[DVR_MAX_VIDEOIN_NUM];
} HY_DVR_PEOPLE_DETECT_CFG, *LPHY_DVR_PEOPLE_DETECT_CFG;

/*!
*  @struct	tagHY_DVR_PEOPLE_DETECT_V1
*  @brief	人形侦测(坐标表示区域)
 */
typedef struct tagHY_DVR_PEOPLE_DETECT_V1
{
	int bValid;																		/*!< 本结构体是否有效，0表示无效  */

	int bEnable;																	/*!< 是否处理人形侦测  */
	int nSenstive;																	/*!< 人形侦测灵敏度, 0 - 5,越低越灵敏,0xff关闭  */
	int nValidTrack;																/*!< 跟踪使能参数是否有效  */
	int nEnableTrack;																/*!< 是否使能跟踪功能	   */
	bool nValidRegionMode;															/*!< 人形区域模式参数是否有效  */
	char nRegionMode;																/*!< 人形区域模式		   */
	char nAIH265PlusEnable;
	char nAIFaceEnable;
	char cMainStreamOverlay;													/*!< 主码流人形叠加框		   */
	char cSubStreamOverlay;														/*!< 子码流人形叠加框		   */
	char cIsPushPeopleImage;													/*!< 人形事件是否推图		   */
	int nSenstiveV1;
	char cTrackZoom;																/*!< 人形跟踪放大		   */
	char cReserve[127];															/*!< 保留  -2为添加nAIH265PlusEnable和nAiFaceEnable  -2：nMainStreamOverlay nSubStreamOverlay*/
	HY_DVR_REGION struPeopDetectRegion[DVR_MAX_PEOPLE_DETECT_REGION_NUM];

	HY_DVR_HANDLEEXCEPTION struHandleType;											/*!< 处理方式  */
	HY_DVR_SCHEDTIME struSchedTime;													/*!< 布防计划  */

	char szReserved[128];
} HY_DVR_PEOPLE_DETECT_V1, *LPHY_DVR_PEOPLE_DETECT_V1;

/*!
*  @struct	tagHY_DVR_PEOPLE_DETECT_CFG_V1
*  @brief	人形侦测(坐标表示区域)
 */
typedef struct tagHY_DVR_PEOPLE_DETECT_CFG_V1
{
	int bValid;																		/*!< 本结构体是否有效，0表示无效  */

	HY_DVR_PEOPLE_DETECT_V1 struPeopleDetectV1[DVR_MAX_VIDEOIN_NUM];
} HY_DVR_PEOPLE_DETECT_CFG_V1, *LPHY_DVR_PEOPLE_DETECT_CFG_V1;

/*! 
*  @struct    tagHY_DVR_PICCFG_PEOPLE_DETECT_CFG_V1_EVENT_TIMESLOT_LIST 
*  @brief    人形侦测事件布防时间段  
*/ 
typedef struct tagHY_DVR_PICCFG_PEOPLE_DETECT_CFG_V1_EVENT_TIMESLOT_LIST
{
    int iValid;
    int iEnable;                                                         /*!< 是否有效，0-无效,1-有效 */
    HY_DVR_SCHEDULE_TIMESLOT stSchedTimeSlot[SCHEDULE_TIMESLOT_MAX_NUM]; /*!< 布防时间段计划;说明:XT5(T40N + 2*2083)4G低功耗设备可以支持跨天时间段设置,但不支持多个时间段设置,只支持一个时间段设置 */
    char szReserved[1024];
} HY_DVR_PICCFG_PEOPLE_DETECT_CFG_V1_EVENT_TIMESLOT_LIST, *LPHY_DVR_PICCFG_PEOPLE_DETECT_CFG_V1_EVENT_TIMESLOT_LIST;

/*! 
*  @struct    tagHY_DVR_PICCFG_PEOPLE_DETECT_CFG_V1_EVENT_TIMESLOT_CFG 
*  @brief    人形侦测事件布防时间段配置信息  
*/ 
typedef struct tagHY_DVR_PICCFG_PEOPLE_DETECT_CFG_V1_EVENT_TIMESLOT_CFG
{
    int bValid; /*!< 本结构体是否有效，0表示无效  */
    HY_DVR_PICCFG_PEOPLE_DETECT_CFG_V1_EVENT_TIMESLOT_LIST stTimeSlot[DVR_MAX_VIDEOIN_NUM];
} HY_DVR_PICCFG_PEOPLE_DETECT_CFG_V1_EVENT_TIMESLOT_CFG, *LPHY_DVR_PICCFG_PEOPLE_DETECT_CFG_V1_EVENT_TIMESLOT_CFG;


typedef struct tagHY_DVR_PICCFG_RADAR_DETECT_CFG
{
    int iValid;

    int iEnable;                    /* 是否使能雷达检测配置;0-否,1-是 */
    int iSenstive;                    /* 雷达检测侦测灵敏度;0-30为低,30-60为中,60-100为高 */
    int iDistanceDisplayEnable;        /* 是否使能测距显示;0-否,1-是 */

    char acReserved[512];
}HY_DVR_PICCFG_RADAR_DETECT_CFG, *LPHY_DVR_PICCFG_RADAR_DETECT_CFG;


typedef struct tagHY_DVR_LONG_AWAY_HOME_DETECT_CFG
{
    int iValid;

    int iEnable;                /* 是否使能长久离家检测;0-否,1-是 */
    int iAwayHomeAlarmDuration; /* 长久离家时间,单位:小时;4-4小时,6-6小时,8-8小时,10-10小时,12-12小时 */

    char acReserved[2048];        //保留字段给大，以后可能添加联动方式、布防计划等
}HY_DVR_LONG_AWAY_HOME_DETECT_CFG, *LPHY_DVR_LONG_AWAY_HOME_DETECT_CFG;


/*!
*  @struct	tagHY_DVR_FACE_DETECT
*  @brief	人脸检测配置
 */
typedef struct tagHY_DVR_FACE_DETECT
{
	int bValid;																		/*!< 本结构体是否有效，0表示无效  */

	int bEnable;																	/*!< 是否启用人脸检测  */
	int bOverlay;																	/*!< 是否叠加区域*/
	int nSenstive;																	/*!< 侦测灵敏度, 0 - 99,越高越灵敏,0xff关闭  */
	HY_DVR_REGION struRegionThreshold[DVR_MAX_REGION_THRESHOLD_NUM];				/*!< 侦测目标的区域门限，最大及最小区域 */
	HY_DVR_REGION struFocus;														/*!< 有效检测区域信息 */
	HY_DVR_HANDLEEXCEPTION struHandleType;											/*!< 处理方式  */
	HY_DVR_SCHEDTIME struSchedTime;													/*!< 布防计划  */

} HY_DVR_FACE_DETECT, *LPHY_DVR_FACE_DETECT;

/*!
*  @struct	tagHY_DVR_PEOPLE_FACE_DETECT_CFG
*  @brief	人脸检测配置信息
 */
typedef struct tagHY_DVR_PEOPLE_FACE_DETECT_CFG
{
	int bValid;																		/*!< 本结构体是否有效，0表示无效  */

	HY_DVR_FACE_DETECT struFace[DVR_MAX_VIDEOIN_NUM];
} HY_DVR_FACE_DETECT_CFG, *LPHY_DVR_FACE_DETECT_CFG;

/*!
*  @struct	tagHY_DVR_TRAVERSE_LINE
*  @brief	越界侦测线条配置
 */
typedef struct tagHY_DVR_TRAVERSE_LINE
{
	int bValid;
	int nBeginX;																	/*!< 起点x相对坐标（0-703）  */
	int nBeginY;																	/*!< 起点y相对坐标（0-575）  */
	int nEndX;																		/*!< 终点x相对坐标（0-703）  */
	int nEndY;																		/*!< 终点y相对坐标（0-575）  */
	int nOrientation;																/*!< 目标方向，0表示A->B，1表示B->A，2表示两个方向A<->B */
	int nSenstive;																	/*!< 侦测灵敏度，0 - 99，越高越灵敏*/
}HY_DVR_TRAVERSE_LINE, *LPHY_DVR_TRAVERSE_LINE;

/*!
*  @struct	tagHY_DVR_TRAVERSE_PLANE
*  @brief	越界侦测配置
 */
typedef struct tagHY_DVR_TRAVERSE_PLANE
{
	int bValid;																		/*!< 本结构体是否有效，0表示无效  */

	int bEnable;																	/*!< 是否启用越界侦测 */
	int bOverlay;																	/*!< 是否叠加区域*/
	HY_DVR_TRAVERSE_LINE struLine[DVR_MAX_TRAVERSE_PLANE_NUM];						/*!< 越界侦测线条配置 */

	HY_DVR_HANDLEEXCEPTION struHandleType;											/*!< 处理方式  */
	HY_DVR_SCHEDTIME struSchedTime;													/*!< 布防计划  */

} HY_DVR_TRAVERSE_PLANE, *LPHY_DVR_TRAVERSE_PLANE;

/*!
*  @struct	tagHY_DVR_TRAVERSE_PLANE_CFG
*  @brief	越界侦测配置信息
 */
typedef struct tagHY_DVR_TRAVERSE_PLANE_CFG
{
	int bValid;																		/*!< 本结构体是否有效，0表示无效  */

	HY_DVR_TRAVERSE_PLANE struPlane[DVR_MAX_VIDEOIN_NUM];
} HY_DVR_TRAVERSE_PLANE_CFG, *LPHY_DVR_TRAVERSE_PLANE_CFG;

/*!
*  @struct	tagHY_DVR_POINT
*  @brief	点配置信息
 */
typedef struct tagHY_DVR_POINT
{
	int bValid;
	int nX;																			/*!< x相对坐标（0-703）  			 */
	int nY;																			/*!< y相对坐标（0-575）  			 */
}HY_DVR_POINT, *LPHY_DVR_POINT;

/*!
*  @struct	tagHY_DVR_FIELD_REGION
*  @brief	入侵区域配置信息
 */
typedef struct tagHY_DVR_FIELD_REGION
{
	int bValid;
	int nSenstive;																	/*!< 侦测灵敏度，0 - 99，越高越灵敏	*/
	HY_DVR_POINT struRect[DVR_MAX_RECT_POINT_NUM];									/*!< 区域矩形，由四个点顺序连接组成	*/
}HY_DVR_FIELD_REGION, *LPHY_DVR_FIELD_REGION;

/*!
*  @struct	tagHY_DVR_FIELD_DETECT
*  @brief	区域入侵配置
 */
typedef struct tagHY_DVR_FIELD_DETECT
{
	int bValid;																		/*!< 本结构体是否有效，0表示无效  */

	int bEnable;																	/*!< 是否启用越界侦测 */
	int bOverlay;																	/*!< 是否叠加区域*/
	HY_DVR_FIELD_REGION struRegion[DVR_MAX_FIELD_DETECT_NUM];						/*!< 区域入侵配置区域信息 */

	HY_DVR_HANDLEEXCEPTION struHandleType;											/*!< 处理方式  */
	HY_DVR_SCHEDTIME struSchedTime;													/*!< 布防计划  */

} HY_DVR_FIELD_DETECT, *LPHY_DVR_FIELD_DETECT;

/*!
*  @struct	tagHY_DVR_FIELD_DETECT_CFG
*  @brief	区域入侵配置信息
 */
typedef struct tagHY_DVR_FIELD_DETECT_CFG
{
	int bValid;																		/*!< 本结构体是否有效，0表示无效  */

	HY_DVR_FIELD_DETECT struPlane[DVR_MAX_VIDEOIN_NUM];
} HY_DVR_FIELD_DETECT_CFG, *LPHY_DVR_FIELD_DETECT_CFG;

/*!
*  @struct	tagHY_DVR_PIR
*  @brief	红外人体检测配置
 */
typedef struct tagHY_DVR_PIR
{
	int bValid;																		/*!< 本结构体是否有效，0表示无效  	*/

	int bEnable;																	/*!< 是否启用PIR 					*/
	int nSenstive;																	/*!< 侦测灵敏度，0 - 99，越高越灵敏 */
	HY_DVR_REGION struRegion;														/*!< 有效检测区域信息 				*/

	HY_DVR_HANDLEEXCEPTION struHandleType;											/*!< 处理方式  						*/
	HY_DVR_SCHEDTIME struSchedTime;													/*!< 布防计划 						*/
} HY_DVR_PIR, *LPHY_DVR_PIR;

/*!
*  @struct	tagHY_DVR_PIR_CFG
*  @brief	红外人体检测配置信息
 */
typedef struct tagHY_DVR_PIR_CFG
{
	int bValid;																		/*!< 本结构体是否有效，0表示无效  */

	HY_DVR_PIR struPIR[DVR_MAX_VIDEOIN_NUM];
} HY_DVR_PIR_CFG, *LPHY_DVR_PIR_CFG;

#define HY_DVR_MOTION_SENSTIVE_HIGHEST  0x1
#define HY_DVR_MOTION_SENSTIVE_HIGHER	0x2
#define HY_DVR_MOTION_SENSTIVE_HIGH		0x4
#define HY_DVR_MOTION_SENSTIVE_MIDDLE	0x8
#define HY_DVR_MOTION_SENSTIVE_LOW		0x16
#define HY_DVR_MOTION_SENSTIVE_LOWER	0x32
#define HY_DVR_MOTION_SENSTIVE_LOWEST	0x64

/*!
*  @struct	tagHY_DVR_MOTION_V2
*  @brief	移动侦测
 */
typedef struct tagHY_DVR_MOTION_EX
{
	int bValid;																		/*!< 本结构体是否有效，0表示无效  */
	int nMotionSenstiveCap;															/*!< 移动侦测的灵敏度的能力  */
	int nPedestrianfilteringValid;													/*!< 表示字段nEnablePedestrianfiltering是否有效  */
	int nEnablePedestrianfiltering;													/*!< 表示人形过滤功能是否使能  */
	int iVoiceType;/*!< 0:自定义,1:警报,2:犬吠,3:警戒区域,4:监控区域,5:欢迎光临,6:深水警告,7:私人领地,8:危险区域,9:贵重物品,10:高处危险,
                        11:私家价位,12:叮咚,13:有口罩,14:无口罩,15:注意私人物品,16:抓小偷,17:救命,18:分类,19:可回收垃圾,20:家庭食物垃圾
                        21:危险废物,22:残余垃圾,23:全部分类,24:车形自定义,25:禁止停车 */
    int iBroadcastCount;                                            /*!< 播报次数 */
    int iDayMode;/*!< 0:夜晚模式,1:白天模式,2:不启用此模式（AllDay字段有效，当字段不为disable时，AllDay字段无效）当设置为此模式时，IPC依赖自身传感器判断当前环境是白天还是夜晚*/
    HY_DVR_SCHEDTIME stVoiceAlarmSchedTime;                         /*!< 声音报警布防时间 */

	char cReserve[666];
} HY_DVR_MOTION_EX, *LPHY_DVR_MOTION_EX;

/*!
*  @struct	tagHY_DVR_MOTION_V2_LIST
*  @brief	移动侦测
 */
typedef struct tagHY_DVR_MOTION_EX_LIST
{
	int bValid;																		/*!< 本结构体是否有效，0表示无效  */

	HY_DVR_MOTION_EX struMotionEx[DVR_MAX_VIDEOIN_NUM];
} HY_DVR_MOTION_EX_LIST, *LPHY_DVR_MOTION_EX_LIST;

typedef struct tagHY_DVR_PICCFG_MOTION_EX_CONF_EVENT_TIMESLOT_LIST
{
    int iValid;
    HY_DVR_SCHEDULE_TIMESLOT stSchedTimeSlot[SCHEDULE_TIMESLOT_MAX_NUM]; /*!< 布防时间计划 */
    char szReserved[1024];
}HY_DVR_PICCFG_MOTION_EX_CONF_EVENT_TIMESLOT_LIST, *LPHY_DVR_PICCFG_MOTION_EX_CONF_EVENT_TIMESLOT_LIST;

typedef struct tagHY_DVR_PICCFG_MOTION_EX_CONF_EVENT_TIMESLOT_EX_LIST
{
    int iValid;
    HY_DVR_PICCFG_MOTION_EX_CONF_EVENT_TIMESLOT_LIST stSchedTimeSlotEx[DVR_MAX_VIDEOIN_NUM]; /*!< 布防时间计划 */
    char szReserved[1024];
}HY_DVR_PICCFG_MOTION_EX_CONF_EVENT_TIMESLOT_EX_LIST, *LPHY_DVR_PICCFG_MOTION_EX_CONF_EVENT_TIMESLOT_EX_LIST;


typedef struct tagHY_DVR_PICCFG_MOTION_EX_CONF_VOICEALARM_TIMESLOT_LIST
{
    int iValid;
    HY_DVR_SCHEDULE_TIMESLOT stSchedTimeSlot[SCHEDULE_TIMESLOT_MAX_NUM]; /*!< 布防时间计划 */
    char szReserved[1024];
}HY_DVR_PICCFG_MOTION_EX_CONF_VOICEALARM_TIMESLOT_LIST, *LPHY_DVR_PICCFG_MOTION_EX_CONF_VOICEALARM_TIMESLOT_LIST;

typedef struct tagHY_DVR_PICCFG_MOTION_EX_CONF_VOICEALARM_TIMESLOT_EX_LIST
{
    int iValid;
    HY_DVR_PICCFG_MOTION_EX_CONF_VOICEALARM_TIMESLOT_LIST stSchedTimeSlotEx[DVR_MAX_VIDEOIN_NUM]; /*!< 布防时间计划 */
    char szReserved[1024];
}HY_DVR_PICCFG_MOTION_EX_CONF_VOICEALARM_TIMESLOT_EX_LIST, *LPHY_DVR_PICCFG_MOTION_EX_CONF_VOICEALARM_TIMESLOT_EX_LIST;

/*!
*  @struct	tagHY_DVR_VILOST
*  @brief	信号丢失报警
 */
typedef struct tagHY_DVR_VILOST
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */

	int bEnable;												/*!< 是否处理信号丢失报警  */
	HY_DVR_HANDLEEXCEPTION struHandleType;						/*!< 处理方式  */
	HY_DVR_SCHEDTIME struSchedTime;								/*!< 布防计划  */

} HY_DVR_VILOST, *LPHY_DVR_VILOST;

/*!
*  @struct	tagHY_DVR_MOSAIC_INFO
*  @brief	马赛克区域信息
 */
typedef struct tagHY_DVR_MOSAIC_INFO
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */

	int bEnable;												/*!< 是否启用  */
	int nHideAreaTopLeftX;										/*!< 马赛克区域的左上点x相对坐标（0-703）  */
	int nHideAreaTopLeftY;										/*!< 马赛克区域的左上点y相对坐标（0-575）  */
	int nHideAreaBottomRightX;									/*!< 马赛克区域的右下点x相对坐标（0-703）  */
	int nHideAreaBottomRightY;									/*!< 马赛克区域的右下点y相对坐标（0-575）  */

} HY_DVR_MOSAIC_INFO, *LPHY_DVR_MOSAIC_INFO;

/*!
*  @struct	tagHY_DVR_MOSAIC
*  @brief	马赛克配置信息
 */
typedef struct tagHY_DVR_MOSAIC
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */

	int bEnableMosaic;											/*!< 是否启动遮挡 ,0-否,1-是 区域为704*576  */
	DWORD dwVirtualWidth;										// 比如19200 HDD,新增
	DWORD dwVirtualHeight;										// 比如10800 HDD,新增
	HY_DVR_MOSAIC_INFO struMosaicInfo[DVR_MAX_VIDEOSHELTER_NUM];/*!< 马赛克区域 */

} HY_DVR_MOSAIC, *LPHY_DVR_MOSAIC;

/*!
*  @struct	tagHY_DVR_OSD_CFG
*  @brief	OSD配置信息
 */
typedef struct tagHY_DVR_OSD_CFG
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */

	int nShowOsd;												/*!< 0为通道时间均不显示、1为显示通道，2、为显示时间、3为两者均显示  */
	int nTimeTopLeftX;											/*!< 时间的x坐标（0-703）  */
	int nTimeTopLeftY;											/*!< 时间的y坐标（0-575）  */
	int nNameTopLeftX;											/*!< 名称的x坐标（0-703）  */
	int nNameTopLeftY;											/*!< 名称的y坐标（0-575）  */

	// HDD 新增，动态表明坐标的范围
	DWORD dwVirtualWidth;										// 比如19200 HDD,新增
	DWORD dwVirtualHeight;										// 比如10800 HDD,新增
} HY_DVR_OSD_CFG, *LPHY_DVR_OSD_CFG;

/*!
*  @struct	tagHY_DVR_OSD_CFG
*  @brief	图像参数结构
 */
typedef struct tagHY_DVR_PIC_CFG
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */

	HY_DVR_WORKMODE struWorkMode;								/*!< 工作模式(只读) */
	HY_DVR_OSD_CFG struOsd[DVR_MAX_VIDEOIN_NUM];				/*!< OSD配置 */
	HY_DVR_CHROMA struChroma[DVR_MAX_VIDEOIN_NUM];				/*!< 色度配置 */
	HY_DVR_VILOST struViLost[DVR_MAX_VIDEOIN_NUM];				/*!< 信号丢失报警 */
	HY_DVR_MOTION struMotion[DVR_MAX_VIDEOIN_NUM];				/*!< 移动侦测 */
	HY_DVR_MOSAIC struMosaic[DVR_MAX_VIDEOIN_NUM];				/*!< 马赛克 */
	HY_DVR_ALIAS_CFG_1 struChName_1[DVR_MAX_VIDEOIN_NUM];		/*!< 通道别名配置(Unicode) */

} HY_DVR_PIC_CFG, *LPHY_DVR_PIC_CFG;


/*!
*  @struct	tagHY_DVR_OSD_CFG_EX
*  @brief	OSD配置信息
 */
typedef struct tagHY_DVR_OSD_CFG_EX
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */

	int nShowBadAirOSD;											/*!< 是否显示毒气OSD，0，不显示，1显示  */
	int nTimeTopLeftX;											/*!< 时间的x坐标（0-703）  */
	int nTimeTopLeftY;											/*!< 时间的y坐标（0-575）  */

    int nShowBatteryInfo;                                       /*!< 是否显示电量信息，0，不显示，1显示  */
    int nBatteryInfoTopLeftX;									/*!< 电量信息的x坐标（0-703）  */
	int nBatteryInfoTopLeftY;									/*!< 电量信息的y坐标（0-575）  */
	
	int iShowSignalStrengthInfo;								/*!< 是否显示信号强度信息，0，不显示，1显示  */
	int iSignalStrengthInfoTopLeftX;							/*!< 信号强度信息的x坐标（0-703）  */
	int iSignalStrengthInfoTopLeftY;							/*!< 信号强度信息的y坐标（0-575）  */ 
	
	char cReserve[1000];
} HY_DVR_OSD_CFG_EX, *LPHY_DVR_OSD_CFG_EX;

/*!
*  @struct	tagHY_DVR_OSD_CFG_EX_LIST
*  @brief	OSD配置信息
 */
typedef struct tagHY_DVR_OSD_CFG_EX_LIST
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */

	HY_DVR_OSD_CFG_EX struOsd[DVR_MAX_VIDEOIN_NUM];				/*!< OSD配置 */
} HY_DVR_OSD_CFG_EX_LIST, *LPHY_DVR_OSD_CFG_EX_LIST;


/*!
*  @struct	tagHY_DVR_EVENT_ACTION_CFG
*  @brief	事件联动配置(包括移动侦测，视频丢失，报警输入等事件都将用到本结构体)
*  @ref     ALARMCFG_ACTION_MULTICHANNEL_CONF(报警输入事件联动配置子命令)
*  @ref     PICCFG_MOTION_ACTION_CONF(移动侦测事件联动配置子命令)
*  @ref     PICCFG_VLOSTS_ACTION_CONF(视频丢失事件联动配置子命令)
 */
typedef struct tagHY_DVR_EVENT_ACTION_CFG
{
	int bValid;
	HY_DVR_HANDLEEXCEPTION_MULTICHANNEL struAction[DVR_MAX_VIDEOIN_NUM];
}HY_DVR_EVENT_ACTION;

typedef struct tagHY_DVR_SHELTER_CHANNEL
{
	int bValid;

	int nEnable;															/*!< 是否启用遮挡检测, 0表示不启用，1表示启用 */
	HY_DVR_MOSAIC_INFO struShelterArea[DVR_MAX_VIDEOSHELTER_NUM];	/*!< 遮挡区域设置 */
	HY_DVR_SCHEDTIME struSchedTime;									/*!< 布防计划  */
}HY_DVR_SHELTER_CHANNEL;

// 有关视频通道的高级参数
// 本结构体使用说明:
// 主命令: HY_DVR_GET_PICCFG和HY_DVR_SET_PICCFG
// 子命令: PICCFG_ADVANCED_CONF.
typedef struct tagHY_DVR_ADVANCED_CHANNEL_CFG
{
	int bValid;

	int nVideoCodecModeNormal[DVR_MAX_VIDEOIN_NUM];				/*!< 当前普通录像编码类型, 数组成员值: 0,表示H264, 1表示H265 2表示MJPEG 3表示H265+ 4表示H264+*/
	int nVideoCodecModeEvent[DVR_MAX_VIDEOIN_NUM];				/*!< 当前事件录像编码类型, 数组成员值: 0,表示H264, 1表示H265 2表示MJPEG 3表示H265+ 4表示H264+ */
	int nVideoCodecModeVGANet[DVR_MAX_VIDEOIN_NUM];				/*!< 当前子码流编码类型, 数组成员值: 0,表示H264, 1表示H265 2表示MJPEG 3表示H265+ 4表示H264+ */
	int nVideoCodecModeNet[DVR_MAX_VIDEOIN_NUM];				/*!< 当前第三子码流编码类型, 数组成员值: 0,表示H264, 1表示H265 2表示MJPEG 3表示H265+ 4表示H264+ */

	//int nEnableCorridorMode[DVR_MAX_VIDEOIN_NUM];			/*!< 是否启用走廊模式, 数组成员值: 0表示未启用,1表示已启用 */

	HY_DVR_SHELTER_CHANNEL struShelter[DVR_MAX_VIDEOIN_NUM];		/*!< 所有通道的视频遮挡配置  */

	int acReserved1[DVR_MAX_VIDEOIN_NUM];
	int acReserved2[DVR_MAX_VIDEOIN_NUM];
	int acReserved3[DVR_MAX_VIDEOIN_NUM];
	int acReserved4[DVR_MAX_VIDEOIN_NUM];
}HY_DVR_ADVANCED_CHANNEL_CFG;

/** @brief 图像配置信息长度																	 */
#define PIC_CFG_LEN   sizeof (HY_DVR_PIC_CFG)
/////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////

/* NXP8850 Image Sensor Processing配置信息: 数字降噪 （De-Noise）*/
typedef struct tagHY_DVR_ISP_COMMON
{
	int bValid;					//! 本结构体是否有效，=0表示无效

	/*
	   参数模板配置 :
	   意义： 通过环境条件的不同进行不同参数脚本的调节
	   0    普通
	   1    室内
	   2 室内低亮度
	   3 室外
	   4 出入口
	   5 自定义
	   6，7 待定
	 */
	int nWorkMode;

	/*
	   图像颜色 （Video Color）
	   0    黑白
	   1    彩色
	 */
	int nVideoColor;

	/*
	   图像方向 (Video orientation)
	   0    正常
	   1    翻转
	   2    镜像
	   3    翻转+ 镜像
	 */
	int nVideoOrientation;

	/*
	前端输入频率
	0    50Hz
	1    60Hz
	*/
	int nInputFrequency;

} HY_DVR_ISP_COMMON, *LPHY_DVR_ISP_COMMON;

/* NXP8850 Image Sensor Processing配置信息: AE Mode 设置*/
typedef struct tagHY_DVR_AE_MODE
{
	int bValid;					//! 本结构体是否有效，=0表示无效

	int nMode;					//AE模式，0-自动 1-背光 2-手动

}HY_DVR_AE_MODE, *LPHY_DVR_AE_MODE;

/* NXP8850 Image Sensor Processing配置信息: AE Window 设置*/
typedef struct tagHY_DVR_AEWINDOW_CFG
{
	int bValid;					//! 本结构体是否有效，=0表示无效

	//开关(on /off)
	int bOpen;

	//X (0-4095) 需要识别到最大的宽度，如果是最大720p,就是1280
	int nX;

	//Y (0-4095) 需要识别最大有效像素高度
	int nY;

	//width (1-4095) 需要识别到最大的宽度
	int nWidth;

	//height(1-4095) 需要识别最大有效像素高度
	int nHeight;

	//Weighting (1X 到 8X可选) 参数0-7
	int nWeighting;

} HY_DVR_AEWINDOW_CFG, *LPHY_DVR_AEWINDOW_CFG;

/* NXP8850 Image Sensor Processing配置信息: 曝光参数*/
typedef struct tagHY_DVR_EXPOSAL_CFG
{
	int bValid;					//! 本结构体是否有效，=0表示无效

	/*
		镜头光圈：(Lens Type)
		关闭自动自动  开启启动光圈   手动光圈
	*/
	int bLensType;

	/*
		目标亮度  (Target Lumiance)	0-255
	*/
	int nTargetLumiance;

	/*
		偏移 (Offset)	0-255
	*/
	int nOffset;

	/*
		快门速度  (Shutter Speed)
		最小速度，0～1/2
		最大速度，1/120～1
		速度可选
		0,1/120,1/60,1/30,1/15,1/8,1/4,1/2,1
	*/
	int nMinShutterSpeed;
	int nMaxShutterSpeed;

	/*
		增益系数 (Gain)
		最小增益 1x ~ 16x
		最大增益 2x ~ 32x
	*/
	int nMinAutoGain;
	int nMaxAutoGain;

} HY_DVR_EXPOSAL_CFG, *LPHY_DVR_EXPOSAL_CFG;

/* NXP8850 Image Sensor Processing配置信息: 白平衡 (White Balance)*/
typedef struct tagHY_DVR_WHITEBALANCE_CFG
{
	int bValid;					//! 本结构体是否有效，=0表示无效

	/*
		1 自动
		2 手动： 手动 需要修改 RED Gain (0-255),Blue Gain(0-255)
		3 多云
		4 太阳
		5 阴天
		6 冷白荧光灯
	*/
	int bWorkMode;

	int nRedGain[DVR_MAX_ISP_WHITEBALANCE];

	int nBlueGain[DVR_MAX_ISP_WHITEBALANCE];

} HY_DVR_WHITEBALANCE_CFG, *LPHY_DVR_WHITEBALANCE_CFG;

/* NXP8850 Image Sensor Processing配置信息: 数字降噪 （De-Noise）*/
typedef struct tagHY_DVR_DENOISE_CFG
{
	int bValid;					//! 本结构体是否有效，=0表示无效

	/*
		关闭
		开启： Y  Strength （0-255）
				   Cb strength (0-255)
				   Cr strength (0-255)
	*/
	int bWorkMode;

	int nYStrength;

	int nCbStrength;

	int nCrStrength;

} HY_DVR_DENOISE_CFG, *LPHY_DVR_DENOISE_CFG;

/* NXP8850 Image Sensor Processing配置信息*/
typedef struct tagHY_DVR_ISP_TEMPLATE
{
	int bValid;					//! 本结构体是否有效，=0表示无效

	/*
	AE 模式，为手动模式时，AE Window设置配置才是有效的
	*/
	HY_DVR_AE_MODE struAEMode;

	/*
		AE Window 设置,分成6个Window,分别是 1 -6
	*/
	HY_DVR_AEWINDOW_CFG struAEWindow[DVR_MAX_ISP_AEWINDOW];

	/*
		曝光参数
	*/
	HY_DVR_EXPOSAL_CFG struExposal;

	/*
		白平衡 (White Balance)
	*/
	HY_DVR_WHITEBALANCE_CFG struWhiteBalance;

	/*
		数字降噪 （De-Noise）
	*/
	HY_DVR_DENOISE_CFG struDeNoise;
}HY_DVR_ISP_TEMPLATE, *LPHY_DVR_ISP_TEMPLATE;

/* NXP8850 Image Sensor Processing配置信息*/
typedef struct tagHY_DVR_NXP8850ISP_CFG
{
	int bValid;					//! 本结构体是否有效，=0表示无效

	//ISP基本配置参数
	HY_DVR_ISP_COMMON struCommon;

	//ISP参数模版
	HY_DVR_ISP_TEMPLATE struTemplate[DVR_MAX_ISP_TEMPLATE];

} HY_DVR_NXP8850ISP_CFG, *LPHY_DVR_NXP8850ISP_CFG;

#define NXP8850ISP_CFG_LEN		(sizeof (HY_DVR_NXP8850ISP_CFG))
/////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////
//编码类型
#define DVR_MAX_STREAM_TYPE		2
#define DVR_CAP_NORMAL_H264    				0x01
#define DVR_CAP_ADVANCED_H264				0x02

#define   DVR_CAP_RESOLUTION_QCIF   0x01		//176*144
#define   DVR_CAP_RESOLUTION_CIF    0x02		//352*288
#define   DVR_CAP_RESOLUTION_2CIF  	0x04		//实时2CIF,704*288
#define   DVR_CAP_RESOLUTION_D1    	0x08		//实时D1,704*576

#define DVR_CAP_RESOLUTION_4CIF_SIM   		0x10		//非实时D1
#define DVR_CAP_RESOLUTION_2CIF_SIM   		0x20		//非实时2CIF

#define DVR_CAP_RESOLUTION_QVGA				0x40		//320*240
#define DVR_CAP_RESOLUTION_VGA				0x80		//640*480
#define DVR_CAP_RESOLUTION_SVGA				0x100		//800*600
#define DVR_CAP_RESOLUTION_XVGA				0x200		//1024*768
#define DVR_CAP_RESOLUTION_HD720			0x400		//1280*720
#define DVR_CAP_RESOLUTION_QuadVGA			0x800		//1280*960
#define DVR_CAP_RESOLUTION_SXGA				0x1000		//1280*1024
#define DVR_CAP_RESOLUTION_UXGA				0x2000		//1600*1200
#define DVR_CAP_RESOLUTION_HD1080			0x4000		//1920*1080
#define DVR_CAP_RESOLUTION_SD1				0x8000		//960*576
#define DVR_CAP_RESOLUTION_QXGA				0x10000     // 2048x1536
#define DVR_CAP_RESOLUTION_CR_640x360		0x20000     //640*360
#define DVR_CAP_RESOLUTION_CR_UWXGA			0x40000		//2592*1520
#define DVR_CAP_RESOLUTION_CR_USXGA			0x80000		//2592*1944
#define DVR_CAP_RESOLUTION_CR_480X256		0x100000	//480*256
#define DVR_CAP_RESOLUTION_CR_320X176		0x200000	//320*176
#define DVR_CAP_RESOLUTION_WQXGA			0x400000	//2304*1728
#define DVR_CAP_RESOLUTION_QSXGA			0x800000	//2592*1920
#define DVR_CAP_RESOLUTION_960H				0x1000000	//960*576
#define DVR_CAP_RESOLUTION_CR_1024x1536		0x10000000  //1024*1536
#define DVR_CAP_RESOLUTION_CR_1080N			0x20000000  //960*1080
#define DVR_CAP_RESOLUTION_CR_4M_N			0x40000000  // 4M_N
#define DVR_CAP_RESOLUTION_CR_5M_N			0x80000000  // 5M N
#define DVR_CAP_RESOLUTION_CR_448P			0x100000000  // 800*448
#define DVR_CAP_RESOLUTION_CR_4K			0x200000000  // 3840 * 2160
#define DVR_CAP_RESOLUTION_CR_2560x1440		0x400000000  // 2560 * 1440
//////////////////////////////////////////////////////////////////////////////////////////////////
/*
	结构体设计说明:每一个IPC设备的子码流支持的分辨率依赖于主码流对应的分辨率，也就是说每一种主码流可以支持多种分辨率
	而每一种分辨率下，又对应支持多种子码流（子码流和高清子码流）的多种分辨率，每一种分辨率对应了支持的最大帧率

	该结构体主要是用于从设备端获取每一种分辨率支持对应的最大帧率
*/

/*!
*  @struct	tagHY_DVR_FRAMERATE_MAP
*  @brief	每一种分辨率对应的最大帧率
 */

 // 编码能力集，主要用实现动态帧率范围
#define DVR_MAX_RESOLUTION_CNT (8) // 最大的分辨率数目
#define DVR_MAX_SUBSTREAM_CNT (2) // 子码流的最大个数
typedef struct tagHY_DVR_FRAMERATE_MAP
{
	int bValid;
	DWORD resolution;
	int max_framerate; // 如果此为0，表示动态帧率，依赖于主码流的分辨率
}HY_DVR_FRAMERATE_MAP;


/*!
*  @struct	tagHY_DVR_STREAM_FRAMERATE
*  @brief	主码流和子码流支持的分辨率能力
*/
typedef struct tagHY_DVR_STREAM_FRAMERATE
{
	int bValid;
	HY_DVR_FRAMERATE_MAP cur_cap; // 当前码流的支持情况

	int nValidFramerateCnt[DVR_MAX_SUBSTREAM_CNT]; // 子码流支持的分辨率的个数, [0]表示子码流，[1]表示高清子码流
	HY_DVR_FRAMERATE_MAP cap[DVR_MAX_SUBSTREAM_CNT][DVR_MAX_RESOLUTION_CNT]; // 当前码流在分辨率下对其它子码流的影响[0][DVR_MAX_RESOLUTION_CNT]表示子码流， [1][DVR_MAX_RESOLUTION_CNT]表示高清子码流， 
}HY_DVR_STREAM_FRAMERATE;

/*!
*  @struct	tagHY_DVR_CODEC_CHANNEL_CAP
*  @brief	每个通道的设备的编码能力结构
 */
typedef struct tagHY_DVR_CODEC_CHANNEL_CAP
{
	int bValid;
	int resolution_cnt; // 主码流所支持的分辨率的数目
	HY_DVR_STREAM_FRAMERATE framerate_cap[DVR_MAX_RESOLUTION_CNT]; // 主码流最多支持的分辨率帧率能力情况 

	int nSupportCorridorMode;			// 是否支持走廊模式, 0表示不支持, 1表示支持
	unsigned nSupportCodecMask;			// 无论此值如何，默认是支持H264．1表示支持H265编码 2表示MJPEG 4表示H265+ 8表示H264+
	unsigned nSupportShelterCheck;		// 是否支持遮挡检测, 0表示不支持，1表示支持

	unsigned char bValidAudioCap;			// 音频能力字段是否有效
	unsigned char nSupportAudioCodecMask;	// 音频编码方式: 0x00表示不支持音频, 0x01, G711, 0x04, G726, 0x08,AAC

    unsigned char bEnableH264;              //是否禁用默认 H264， 0 - 不禁用，1 - 禁用
	unsigned char cReserve;					//保留字段
	unsigned nIsSupportCodecMaskV2;			//0,表示V2版本的编码类型参数不支持，1.表示V2版本的编码类型参数支持
	unsigned nSupportMainStreamCodecMaskV2; //V2版本的编码类型，从低位到高位，第一位表示H264（1），第二位表示H265（2），第三位表示MJPEG（4），第四位表示H265+（8），第五位表示H264+（16），第六位表示smart265（32）
	unsigned nSupportSubStreamCodecMaskV2;  //V2版本的编码类型，从低位到高位，第一位表示H264（1），第二位表示H265（2），第三位表示MJPEG（4），第四位表示H265+（8），第五位表示H264+（16），第六位表示smart265（32）
	unsigned nSupportThirdStreamCodecMaskV2;//V2版本的第三码流编码类型，从低位到高位，第一位表示H264（1），第二位表示H265（2），第三位表示MJPEG（4），第四位表示H265+（8），第五位表示H264+（16），第六位表示smart265（32）
	char resvered[224]; // 保留，以便扩展
}HY_DVR_CODEC_CHANNEL_CAP;

/*!
*  @struct	tagHY_DVR_CODEC_CAP
*  @brief	设备的总编码能力
 */
typedef struct tagHY_DVR_CODEC_CAP
{
	int bValid;
	HY_DVR_CODEC_CHANNEL_CAP codec_cap[DVR_MAX_VIDEOIN_NUM]; // 设备的总编码能力
}HY_DVR_CODEC_CAP;
#define COMPRESS_NEW_CFG_LEN   sizeof (HY_DVR_CODEC_CAP)















//////////////////////////////////////////////////////////////////////////////////////////////////
/*
	结构体设计说明:每一个IPC设备的子码流支持的分辨率依赖于主码流对应的分辨率，也就是说每一种主码流可以支持多种分辨率
	而每一种分辨率下，又对应支持多种子码流（子码流和高清子码流）的多种分辨率，每一种分辨率对应了支持的最大帧率

	该结构体主要是用于从设备端获取每一种分辨率支持对应的最大帧率
*/

/*!
*  @struct	tagHY_DVR_FRAMERATE_MAP
*  @brief	每一种分辨率对应的最大帧率
 */

typedef struct tagHY_DVR_RESOLUTION_AND_FRAMERATE_INFO
{
	int nResolutionWidth;
	int nResolutionHeight;
	int nMaxFrameRate; // 如果此为0，表示动态帧率，依赖于主码流的分辨率
}HY_DVR_RESOLUTION_AND_FRAMERATE_INFO;

// 编码能力集，主要用实现动态帧率范围
typedef struct tagHY_DVR_SUB_STREAM_RESOLUTION_V3
{
	int bValid;
	HY_DVR_RESOLUTION_AND_FRAMERATE_INFO struResolutionAndFrameInfo;

	struct tagHY_DVR_SUB_STREAM_RESOLUTION_V3 * pNext;
}HY_DVR_SUB_STREAM_RESOLUTION_V3;


/*!
*  @struct	tagHY_DVR_STREAM_FRAMERATE
*  @brief	主码流和子码流支持的分辨率能力
 */
typedef struct tagHY_DVR_SUB_STREAM
{
	int bValid;
	HY_DVR_SUB_STREAM_RESOLUTION_V3 * pSubStreamResolutionHead; // 某一个子码流支持的分辨率和帧率的列表的头指针

	struct tagHY_DVR_SUB_STREAM * pNext;
}HY_DVR_SUB_STREAM;


/*!
*  @struct	tagHY_DVR_MAIN_STREAM_RESOLUTION_FRAMERATE
*  @brief	主码流和子码流支持的分辨率能力
 */
typedef struct tagHY_DVR_MAIN_STREAM_RESOLUTION_V3
{
	int bValid;
	HY_DVR_RESOLUTION_AND_FRAMERATE_INFO struMainStreamResolutionAndFramerate; // 当前主码流的支持的分辨率和帧率

	HY_DVR_SUB_STREAM * pSubStreamHead; // 当前主码流的分辨率下支持的子码流的头指针（子码流为对象）
	struct tagHY_DVR_MAIN_STREAM_RESOLUTION_V3 * pNext;
}HY_DVR_MAIN_STREAM_RESOLUTION_V3;

/*!
*  @struct	tagHY_DVR_CODEC_CHANNEL_CAP_V3
*  @brief	每个通道的设备的编码能力结构
 */
typedef struct tagHY_DVR_CODEC_CHANNEL_CAP_V3
{
	int bValid;
	HY_DVR_MAIN_STREAM_RESOLUTION_V3 * pMainStreamResolutionHead; // 主码流支持的分辨率的列表的头指针

	char resvered[64]; // 保留，以便扩展
}HY_DVR_CODEC_CHANNEL_CAP_V3;

/*!
*  @struct	tagHY_DVR_CODEC_CAP_V3
*  @brief	设备的总编码能力
 */
typedef struct tagHY_DVR_CODEC_CAP_V3
{
	int bValid;
	HY_DVR_CODEC_CHANNEL_CAP_V3 codec_cap[DVR_MAX_VIDEOIN_NUM]; // 设备的总编码能力
}HY_DVR_CODEC_CAP_V3;
#define COMPRESS_NEW_CFG_LEN_V3   sizeof (HY_DVR_CODEC_CAP_V3)


//////////////////////////////////////////////////////////////////////////////////////////////////
/*
	结构体设计说明:每一个IPC设备的子码流支持的分辨率依赖于主码流对应的分辨率，也就是说每一种主码流可以支持多种分辨率
	而每一种分辨率下，又对应支持多种子码流（子码流和高清子码流）的多种分辨率，每一种分辨率对应了支持的最大帧率

	该结构体主要是用于从设备端获取每一种分辨率支持对应的最大帧率
*/

/*!
*  @struct	tagHY_DVR_FRAMERATE_MAP
*  @brief	每一种分辨率对应的最大帧率
 */

 // 编码能力集，主要用实现动态帧率范围
#define DVR_MAX_RESOLUTION_CNT_V2 (10) // 最大的分辨率数目
#define DVR_MAX_SUBSTREAM_CNT_V2 (10) // 子码流的最大个数
#define DVR_MAX_FRAMERATE_CNT_V2 (120) //码流最大支持的帧率数
#define DVR_MAX_BITRATE_CNT_V2 (50)	//支持的最大固定码率数

//帧率和码率对应表
typedef struct tagHY_DVR_FRAMERATE_BITRATE_MAP
{
	int bValid;
	int nFrameRate;							//对应分辨率下的当前帧率
	int nMaxBitrate[DVR_MAX_BITRATE_CNT_V2];
	int nMinBitrate[DVR_MAX_BITRATE_CNT_V2];
}HY_DVR_FRAMERATE_BITRATE_MAP;


//分辨率和帧率对应表
typedef struct tagHY_DVR_RESOLUTION_FRAMERATE_BITRATE_MAP
{
	int bValid;
	int nResolution;
	int nMaxFramerate; // 如果此为0，表示动态帧率，依赖于主码流的分辨率
	HY_DVR_FRAMERATE_BITRATE_MAP struFramerateBitrateMap[DVR_MAX_FRAMERATE_CNT_V2];
}HY_DVR_RESOLUTION_FRAMERATE_BITRATE_MAP;


typedef struct tagHY_DVR_SUB_STREAM_RESOLUTION
{
	int bValid;
	HY_DVR_RESOLUTION_FRAMERATE_BITRATE_MAP struSubResolutionFramerateBitrate; // 子码流的分辨率和对应的帧率以及码率的关系
}HY_DVR_SUB_STREAM_RESOLUTION;


/*!
*  @struct	tagHY_DVR_SUB_STREAM_CFG
*  @brief	子码流的情况
 */
typedef struct tagHY_DVR_SUB_STREAM_CFG
{
	int bValid;
	HY_DVR_SUB_STREAM_RESOLUTION struSubStreamResolution[DVR_MAX_RESOLUTION_CNT_V2]; //子码流支持的分辨率情况
}HY_DVR_SUB_STREAM_CFG;


/*!
*  @struct	tagHY_DVR_MAIN_STREAM_RESOLUTION
*  @brief	主码流支持的分辨率数组
 */
typedef struct tagHY_DVR_MAIN_STREAM_RESOLUTION
{
	int bValid;
	HY_DVR_RESOLUTION_FRAMERATE_BITRATE_MAP struMainResolutionFramerateBitrate; // 主码流的分辨率和对应的帧率以及码率的关系
	HY_DVR_SUB_STREAM_CFG struSubStreamCfg[DVR_MAX_SUBSTREAM_CNT_V2]; // 主码流下支持的子码流的情况，bValid值控制是否有效
}HY_DVR_MAIN_STREAM_RESOLUTION;

/*!
*  @struct	tagHY_DVR_CODEC_CAP_CHANNEL_V2
*  @brief	每个通道的设备的编码能力结构
 */
typedef struct tagHY_DVR_CODEC_CAP_CHANNEL_V2
{
	int bValid;
	HY_DVR_MAIN_STREAM_RESOLUTION struMainStreamResolution[DVR_MAX_RESOLUTION_CNT_V2]; // 主码流最多支持的分辨率帧率能力情况 
	char resvered[256]; // 保留，以便扩展
}HY_DVR_CODEC_CAP_CHANNEL_V2;

/*!
*  @struct	tagHY_DVR_CODEC_CAP_V2
*  @brief	设备的总编码能力
 */
typedef struct tagHY_DVR_CODEC_CAP_V2
{
	int bValid;
	HY_DVR_CODEC_CAP_CHANNEL_V2 struCodecCapChannelV2[DVR_MAX_VIDEOIN_NUM]; // 设备的总编码能力
}HY_DVR_CODEC_CAP_V2;
#define COMPRESS_NEW_CFG_LEN_V2   sizeof (HY_DVR_CODEC_CAP_V2)


//////////////////////////////////////////////////////////////////////////

/*!
*  @struct	tagHY_DVR_COMPRESS_CAP
*  @brief	编码能力结构
 */
typedef struct tagHY_DVR_COMPRESS_CAP
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */
	int nStreamFormatCap;										/*!< 支持的码流格式， 所有支持的格式相或的结果  */
	unsigned long dwResolutionCap[DVR_MAX_STREAM_TYPE];			/*!< 每种编码格式下支持的分辨率格式,支持所有的格式相或的结果  */

}HY_DVR_COMPRESS_CAP, *LPHY_DVR_COMPRESS_CAP;

/*!
*  @struct	tagHY_DVR_COMPRESSION_INFO
*  @brief	编码配置结构
 */
typedef struct tagHY_DVR_COMPRESSION_INFO
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */
	int nVideoFrameRate;										/*!< 帧率 1-20，ff为全帧率 */
	int nVideoBitrate;											/*!< 码率上限,单位Kbit/s */

	int nQuotiety;												/*!< 量化系数:0-6 */
	int nResolution;											/*!< 分辨率0-CIF, 1-D1 */
	int nStreamType;											/*!< 码流类型 0-视频流 1-复合流 */

	int nBitrateType;											/*!< 码率类型 0-变码率率 1-定码 2-限定码率 3.smartAVC*/

}HY_DVR_COMPRESSION_INFO, *LPHY_DVR_COMPRESSION_INFO;

/*!
*  @struct	tagHY_DVR_AUX_COMPRESSION_INFO
*  @brief	子码流编码参数结构
 */
typedef struct tagHY_DVR_AUX_COMPRESSION_INFO
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */
	int bEnalbe;												/*!< 是否启用子码流  */
	int nResolution;											/*!< 分辨率0-CIF, 1-D1 */
	int nVideoFrameRate;										/*!< 帧率 1-20，ff为全帧率 */
	int nVideoBitrate;											/*!< 码率:32Kbin/s,48Kbit/s,64Kbit/s,128Kbit/s,256Kbit/s,512Kbit/s */

}HY_DVR_AUX_COMPRESSION_INFO, *LPHY_DVR_AUX_COMPRESSION_INFO;

/*!
*  @struct	tagHY_DVR_COMPRESSION_CHANNEL
*  @brief	通道编码参数结构
 */
typedef struct tagHY_DVR_COMPRESSION_CHANNEL
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */
	int nDelayRecordTime;										/*!< 录象延迟时间:10s、30s、自定义（10s-300s） */
	int nPreRecordTime;											/*!< 预录时间:0表示最大预录时间、5秒、10秒 */
	HY_DVR_COMPRESS_CAP struCompressCap;						/*!< 压缩能力（只读）  */
	HY_DVR_AUX_COMPRESSION_INFO struNetPara;					/*!< 子码流  */
	HY_DVR_COMPRESSION_INFO struNormalRecordPara;				// HDD 新增
	HY_DVR_COMPRESSION_INFO struEventRecordPara;					// HDD 新增
	HY_DVR_COMPRESSION_INFO struVGANetPara;					// HDD 新增
} HY_DVR_COMPRESSION_CHANNEL, *LPHY_DVR_COMPRESSION_CHANNEL;

/*!
*  @struct	tagHY_DVR_HC_RESMODE
*  @brief	HC板型编解码资源分配模式
*/
typedef struct tagHY_DVR_HC_RESMODE
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */

	//该结构体只在HC或HS板型中有效

	/*  工作模式取值范围为(0 - 2)
	16路板型:
	模式0时，系统支持16路实时CIF或非实时2CIF、D1格式编解码
	模式1时，系统支持8路实时2CIF 加8路实时CIF 编解，4路2CIF实时解码
	模式2时，系统支持4路实时D1 加12路实时CIF编码，1路D1实时解码
	8路板型:
	模式0时，系统支持8路实时CIF或非实时2CIF、D1格式编解码
	模式1时，系统支持2路实时D1 加6路实时CIF 编解，4路D1实时解码
	模式2时，系统支持6路实时D1 加2路实时CIF编码，1路D1实时解码
	*/
	int nMode;

	DWORD dwChMask;												/*!< 通道掩码，第0位为1表示通道1支持2CIF或D1格式编码，依次类推 */

} HY_DVR_HC_RESMODE, *LPHY_DVR_HC_RESMODE;

/////////////////////////////////////////////////////////////////////////////////////////////////
/*!
*  @struct	tagHY_DVR_VGA_NET_CFG
*  @brief	VGA子码流结构
*/
typedef struct tagHY_DVR_HD_NET_CFG
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */
	HY_DVR_COMPRESSION_INFO struVGANetPara[DVR_MAX_VIDEOIN_NUM];				/*!< VGA子码流 */

}HY_DVR_VGA_NET_CFG, *LPHY_DVR_VGA_NET_CFG;

/*!
*  @struct	tagHY_DVR_COMPRESSION_CFG
*  @brief	编码参数结构
*/
typedef struct tagHY_DVR_COMPRESSION_CFG
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */

	HY_DVR_COMPRESSION_CHANNEL struCompressChannel[DVR_MAX_VIDEOIN_NUM];	/*!< 各通道编码参数 */
} HY_DVR_COMPRESSION_CFG, *LPHY_DVR_COMPRESSION_CFG;

/** @brief 编码配置结构体长度																	 */
#define COMPRESS_CFG_LEN   sizeof (HY_DVR_COMPRESSION_CFG)
/////////////////////////////////////////////////////////////////////////////////////////////////

/*!
*  @struct	tagHY_DVR_COMPRESSION_INFO
*  @brief	编码配置结构
 */
typedef struct tagHY_DVR_COMPRESSION_INFO_EX
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */
	int nVideoFrameRateEx;										/*!< 按实际帧率给出，没有全帧率的概念 */
	int nKeyFrameInterval;										/*!< 关键帧帧间隔 */
	char cRevered[1020];										/*!< 保留字段 */
} HY_DVR_COMPRESSION_INFO_EX, *LPHY_DVR_COMPRESSION_INFO_EX;

/*!
*  @struct	tagHY_DVR_COMPRESSION_CHANNEL
*  @brief	通道编码参数结构
 */
typedef struct tagHY_DVR_COMPRESSION_CHANNEL_EX
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */
	HY_DVR_COMPRESSION_INFO_EX struNetPara;					/*!< 子码流  */
	HY_DVR_COMPRESSION_INFO_EX struNormalRecordPara;
	HY_DVR_COMPRESSION_INFO_EX struEventRecordPara;
	HY_DVR_COMPRESSION_INFO_EX struVGANetPara;
	char cVolume;											/*!<设备音频输入的声音，音量值 0-100   */
	char cEnableAudioOutVolume;								/*!<设备音频输出的声音使能			   */
	char cAudioOutVolume;									/*!<设备音频输出的声音，音量值 0-100   */
	char cIsMute;									/*!<是否静音			   */
	char cIsMaxVolume;							/*!<最大音量			   */

	char cEnableAlarmAudio;    /*!< 是否使能报警铃声;0-否,1-是  */
    char cEnableAlarmFlashLight; /*是否打开报警闪光灯;0-否,1-是*/
    char cAlarmVolume;        /*!< 报警音量  */
    int iVoiceType;     /*!< 0:自定义,1:警报,2:犬吠,3:警戒区域,4:监控区域,5:欢迎光临,6:深水警告,7:私人领地,8:危险区域,9:贵重物品,10:高处危险,
                        11:私家价位,12:叮咚,13:有口罩,14:无口罩,15:注意私人物品,16:抓小偷,17:救命,18:分类,19:可回收垃圾,20:家庭食物垃圾
                        21:危险废物,22:残余垃圾,23:全部分类,24:车形自定义,25:禁止停车 */
    int iAlarmDuration;      /*!< 报警时长,单位:秒;5,10,15  */
	int iAlarmPlayTimes;                /* 报警播报次数,为0时表示自定义, 1/3/5/10/20 */

	char cRevered[1004];
} HY_DVR_COMPRESSION_CHANNEL_EX, *LPHY_DVR_COMPRESSION_CHANNEL_EX;

/*!
*  @struct	tagHY_DVR_COMPRESSION_CFG
*  @brief	编码参数结构
*/
typedef struct tagHY_DVR_COMPRESSION_CFG_EX
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */
	HY_DVR_COMPRESSION_CHANNEL_EX struCompressChannelEx[DVR_MAX_VIDEOIN_NUM];	/*!< 各通道编码参数 */
	char cRevered[1024];
} HY_DVR_COMPRESSION_CFG_EX, *LPHY_DVR_COMPRESSION_CFG_EX;
/** @brief 编码配置结构体长度																	 */
#define COMPRESS_CFG_LEN_EX   sizeof (HY_DVR_COMPRESSION_CFG_EX)



/*!
*  @struct	tagHY_DVR_COMPRESSION_CHANNEL_EX_ALARM_SCHEDTIME
*  @brief	通道编码参数结构
*/
typedef struct tagHY_DVR_COMPRESSION_CHANNEL_EX_ALARM_SCHEDTIME
{
	int iValid;																/*!< 本结构体是否有效，0表示无效 */
	
	HY_DVR_SCHEDTIME struSchedTime;     /* 语音报警布防计划 */
	HY_DVR_SCHEDULE_TIMESLOT stSchedTimeSlot[SCHEDULE_TIMESLOT_MAX_NUM];     /*!< 布防时间计划 */
	
	char cRevered[1024];
} HY_DVR_COMPRESSION_CHANNEL_EX_ALARM_SCHEDTIME, *LPHY_DVR_COMPRESSION_CHANNEL_EX_ALARM_SCHEDTIME;

/*!
*  @struct	tagHY_DVR_COMPRESSION_CFG_EX_ALARM_SCHEDTIME
*  @brief	编码参数结构
*/
typedef struct tagHY_DVR_COMPRESSION_CFG_EX_ALARM_SCHEDTIME
{
	int iValid;																/*!< 本结构体是否有效，0表示无效 */

	HY_DVR_COMPRESSION_CHANNEL_EX_ALARM_SCHEDTIME stCompressChannelExAlarmSchedTime[DVR_MAX_VIDEOIN_NUM];	/*!< 各通道编码参数报警布防时间 */

	char acRevered[1024];
} HY_DVR_COMPRESSION_CFG_EX_ALARM_SCHEDTIME, *LPHY_DVR_COMPRESSION_CFG_EX_ALARM_SCHEDTIME;



#define  MAX_STREAM_COUNT			64 * 6
/////////////////////////////////////////////////////////////////////////////////////////////////
/*!
*  @struct	tagHY_DVR_COMPRESSION_INFO
*  @brief	编码配置结构
 */
typedef struct tagHY_DVR_COMPRESSION_INFO_V2
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */
	int nVideoCodecID;											/*!< 0,H264,1,H265,2,MJPEG,3,H.265+,4,H.264+,5,smart265 */
	int nVideoFrameRate;										/*!< 按实际帧率给出，不存在全帧率的概念*/
	int nVideoBitrate;											/*!< 码率上限,单位Kbit/s */
	int nQuotiety;												/*!< 量化系数:0-6 */
	int nResolutionWidth;										/*!< 分辨率的宽 */
	int nResolutionHeight;										/*!< 分辨率的高 */
	int nStreamType;											/*!< 码流类型 0-视频流 1-复合流 */
	int nBitrateType;											/*!< 码率类型 0-变码率率 1-定码 2-限定码率 3.smartAVC*/
	int nKeyframeinterval;										/*!< 帧间隔*/
	int nGop;														//码流gop，单位帧
	int nEncrypt;													//是否对视频加密，移动OVD专用
	char reserved1[500];									//添加了 帧间隔 

	int nDeinterlace;											/*!< 反交错	0:NotSupport		1:Close		2:Open */
	int nStreamEnable;										/*!< 启用流	0:NotSupport		1:Disable	2:Enable */
	int nAudioCodecID;											/*!< 0,G711U,1,G711A */
	int nMaxResolutionWidth;									/*!< 最大分辨率的宽 */
	int nMaxResolutionHeight;									/*!< 最大分辨率的高 */
	char cRevered[500 - 8];										/*!< 保留字段 */
} HY_DVR_COMPRESSION_INFO_V2, *LPHY_DVR_COMPRESSION_INFO_V2;
/*!
*  @struct	tagHY_DVR_COMPRESSION_CHANNEL_V2
*  @brief	通道编码参数结构V2
 */
typedef struct tagHY_DVR_COMPRESSION_CHANNEL_V2
{
	int bValid;
	int nChannelID;												/*!< 通道						 */
	int nStreamID;												/*!< 1,表示主码流，2表示第一子码流，3表示第二子码流，4表示第三子码流....依次类推 *//*!< 本结构体是否有效，0表示无效 */
	HY_DVR_COMPRESSION_INFO_V2 struStreamTypeV2;				/*!< 码流类型 */
} HY_DVR_COMPRESSION_CHANNEL_V2, *LPHY_DVR_COMPRESSION_CHANNEL_V2;
/*!
*  @struct	tagHY_DVR_COMPRESSION_CFG_V2
*  @brief	通道编码参数结构V2
 */
typedef struct tagHY_DVR_COMPRESSION_CFG_V2
{
	int bValid;
	HY_DVR_COMPRESSION_CHANNEL_V2 struCompressChannelV2[MAX_STREAM_COUNT];
} HY_DVR_COMPRESSION_CFG_V2, *LPHY_DVR_COMPRESSION_CFG_V2;


/*!
*  @struct	tagHY_DVR_COMPRESSION_CFG
*  @brief	远程桌面码流配置
*/
typedef struct tagHY_DVR_REMOTE_DESKTOP_STREAM_CFG
{
	HY_DVR_COMPRESSION_INFO_V2 struRemoteDesktopStream[DVR_MAX_VIDEOIN_NUM];
}HY_DVR_REMOTE_DESKTOP_STREAM_CFG;



#define MAX_QUALITY_STREAM_MAP_COUNT   (10)

typedef struct tagHY_DVR_COMPRESSION_QUALITY_AND_STREAM_TYPE
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */
	int dwQuality;															/*!< 视频质量 */
	int dwStreamType;														/*!< 码流类型 */
	char cRevered[1024];
}HY_DVR_COMPRESSION_QUALITY_AND_STREAM_TYPE;

/*!
*  @struct	tagHY_DVR_COMPRESSION_CFG
*  @brief	某一通道视频质量和码流类型映射表
*/
typedef struct tagHY_DVR_COMPRESSION_QUALITY_AND_STREAM_TYPE_TABLE_CHANNEL
{
	int nValid;					/*!< 本结构体是否有效，0表示无效 */
	int dwRealCount;			//记录实际映射表的个数，注意不能大于MAX_QUALITY_STREAM_MAP_COUNT
	HY_DVR_COMPRESSION_QUALITY_AND_STREAM_TYPE struQualityAndStreamType[MAX_QUALITY_STREAM_MAP_COUNT];
	char cRevered[1024];		/*!< 本结构体是否有效，0表示无效 */
} HY_DVR_COMPRESSION_QUALITY_AND_STREAM_TYPE_TABLE_CHANNEL, *LPHY_DVR_COMPRESSION_QUALITY_AND_STREAM_TYPE_TABLE_CHANNEL;


/*!
*  @struct	tagHY_DVR_COMPRESSION_CFG
*  @brief	所有通道视频质量和码流类型映射表
*/
typedef struct tagHY_DVR_COMPRESSION_QUALITY_AND_STREAM_TYPE_TABLE
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */
	HY_DVR_COMPRESSION_QUALITY_AND_STREAM_TYPE_TABLE_CHANNEL struQualityAndStreamTypeTableChannel[DVR_MAX_VIDEOIN_NUM];	/*!< 各通道编码参数 */
	char cRevered[1024];
} HY_DVR_COMPRESSION_QUALITY_AND_STREAM_TYPE_TABLE, *LPHY_DVR_COMPRESSION_QUALITY_AND_STREAM_TYPE_TABLE;
/** @brief 编码配置结构体长度																	 */
#define COMPRESS_QUALITY_AND_STREAM_TABLE_LEN_EX   sizeof (HY_DVR_COMPRESSION_QUALITY_AND_STREAM_TYPE_TABLE)

/////////////////////////////////////////////////////////////////////////////////////////////////
/*!
*  @struct	tagHY_DVR_RECORD_CTRLSTATE
*  @brief	录像控制状态
*/
typedef struct tagHY_DVR_RECORD_CTRLSTATE
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */

	unsigned int uRecordMode[DVR_MAX_VIDEOIN_NUM];							/*!< 0表示手动录像,1表示自动录像,2表示关闭录像 */

} HY_DVR_RECORD_CTRLSTATE, *LPHY_DVR_RECORD_CTRLSTATE;

/*!
*  @struct	tagHY_DVR_RECORD_STREAM
*  @brief	录像码流选择
*/
typedef struct tagHY_DVR_RECORD_STREAM
{
	int bValid;																/*!< 本结构体是否有效，=0表示无效 */

	unsigned int uStreamIndex[DVR_MAX_VIDEOIN_NUM];							/*!< 0表示主码流,1表示子码流(CIF),2表示高清子码流(VGA) */

} HY_DVR_RECORD_STREAM, *LPHY_DVR_RECORD_STREAM;

/*!
*  @struct	tagHY_DVR_RECORD_SCHED
*  @brief	录像计划结构
 */
typedef struct tagHY_DVR_RECORD_SCHED
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */
	HY_DVR_SCHEDTIME struRecordTime[DVR_MAX_VIDEOIN_NUM];					/*!< 录像时间计划 */
	HY_DVR_RECORD_CTRLSTATE struRecordCtlState;								/*!< 录像控制状态 */
	HY_DVR_RECORD_STREAM struRecordStream;									/*!< 录像码流 */

} HY_DVR_RECORD_SCHED, *LPHY_DVR_RECORD_SCHED;

/** @brief 录像计划结构体长度																	 */
#define RECORD_SCHED_LEN sizeof(HY_DVR_RECORD_SCHED)

/////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////////////

/*!
*  @struct	tagHY_DVR_PTZPRO
*  @brief	云台协议信息
 */
typedef struct tagHY_DVR_PTZPRO
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */

	char szPTZName[DVR_MAX_PTZPRONAME_LEN];									/*!< PTZ协议名称  */

} HY_DVR_PTZPRO, *LPHY_DVR_PTZPRO;

/*!
*  @struct	tagHY_DVR_PTZPRO_CFG
*  @brief	云台协议管理信息
 */
typedef struct tagHY_DVR_PTZPRO_CFG
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */
	int nPtzCount;															/*!< 系统支持PTZ协议的个数  */
	HY_DVR_PTZPRO struPTZProtocol[DVR_MAX_PTZPRO_NUM];						/*!< 系统PTZ  */

} HY_DVR_PTZPRO_CFG, *LPHY_DVR_PTZPRO_CFG;

/** @brief 云台协议管理结构体长度																 */
#define PTZPRO_CFG_LEN sizeof(HY_DVR_PTZPRO_CFG)
/////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////

/*!
*  @struct	tagHY_DVR_PTZ
*  @brief	云台信息
 */
typedef struct tagHY_DVR_PTZ
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */
	unsigned char bySetPreset[DVR_MAX_PRESET_NUM];							/*!< 预置点是否设置,0-没有设置,1-设置  */
	unsigned char bySetCruise[DVR_MAX_PRESET_NUM];							/*!< 巡航是否设置: 0-没有设置,1-设置  */
	unsigned char bySetTrack[DVR_MAX_PRESET_NUM];							/*!< 轨迹是否设置,0-没有设置,1-设置  */

} HY_DVR_PTZ, *LPHY_DVR_PTZ;


// 摄像机配置——云台配置
typedef enum
{
	eZWPTZActionType_Preset,
	eZWPTZActionType_Cruise,
	eZWPTZActionType_Track,
	eZWPTZActionType_AutoScan,
}eZWPTZActionType;
typedef struct tagZW_CAMERA_PTZ_CFG
{
	int bValid;																		/*!< 本结构体是否有效，0表示无效  */

	int dwEnable;
	DWORD dwAddress;
	DWORD dwBandrate;
	DWORD dwProtocol_index;

	int  bWatch;
	DWORD dwWatchDelayBySecond;									// 多长时间之后开始守望,单位：秒
	eZWPTZActionType dwPTZActionType;							// 守望动作类型
	DWORD dwPTZActionID;										// 守望动作ID

	int     nSpeedValid;										// 	
	int     nSpeed;												// 速度
	int	nPortValid;
	int	nPort;													//天翼-PTZ服务器端口
	char	acReserved[240];									// 保留(方便日后扩展)
}ZW_CAMERA_PTZ_CFG;


// 云台预置点列表
typedef struct
{
	int                                 bValid;						// 当前预置点是否有效
	int                                 nPresetIndex;				// 预置点编号　
	char                                strPresetToken[32];			// 预置点TOKEN
	char                                strPresetName[32];			// 预置点NAME

	char                                cResvered[64];				// 保留字段
}ZW_PRESET_INFO;

typedef struct
{
	int									bValid;						// 有效性
	ZW_PRESET_INFO                   struPresetList[256];			// 预置点列表
}ZW_CAMERA_PRESET_LIST;

typedef struct
{
	int                                 bValid;
	ZW_CAMERA_PRESET_LIST              struPTZPresetList[1];
}ZW_DVR_PTZ_PRESET_LIST;

// 云台预置点列表
typedef struct
{
	int                                 bValid;						// 当前预置点是否有效
	int                                 nPresetIndex;				// 预置点编号　
	char                                strPresetToken[32];			// 预置点TOKEN
	char                                strPresetName[32];			// 预置点NAME
}ZW_PRESET_INFO_FOR_NVR;

typedef struct
{
	int									bValid;						// 有效性
	ZW_PRESET_INFO_FOR_NVR                   struPresetList[256];			// 预置点列表
}ZW_CAMERA_PRESET_LIST_FOR_NVR;

typedef struct
{
	int                                 bValid;
	ZW_CAMERA_PRESET_LIST_FOR_NVR              struPTZPresetList[DVR_MAX_PTZ_NUM];
}ZW_DVR_PTZ_PRESET_LIST_FOR_NVR;

/*!
*  @struct	tagHY_DVR_DECODER_CHANNEL
*  @brief	解码通道配置
 */
typedef struct tagHY_DVR_DECODER_CHANNEL
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */
	char szPTZName[DVR_MAX_PTZPRONAME_LEN];									/*!< PTZ协议名称  */

	ZW_CAMERA_PTZ_CFG   struPTZcfg;			// HDD 新增，用于替代struSerial

} HY_DVR_DECODER_CHANNEL, *LPHY_DVR_DECODER_CHANNEL;

/*!
*  @struct	tagHY_DVR_PTZ_ELCTRONIC_SONY
*  @brief	sony的电子ptz机芯配置
*/
typedef struct  tagHY_DVR_PTZ_ELCTRONIC_SONY
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */

	int nDigitZoom;															/*!< 数字变倍 (0-关 1-开)			                        */
	int nLowLum;															/*!< 低照度 (0-关 1-开)										*/
	int nBackLight;															/*!< 背光补偿 (0-关 1-开)									*/
	int nAutoShutter;														/*!< 自动快门 (0-手动 1-自动)			                    */
	int nWhiteBalance;														/*!< 白平衡 (0-手动 1-自动 2-室内 3-室外 4-ATW 5-ONE PUSH)	*/
	int nRedGain;															/*!< 红增益 (0~255)											*/
	int nBlueGain;															/*!< 蓝增益 (0~255)											*/
	int nAcutance;															/*!< 锐度 (0~15)											*/
	int nExpMode;															/*!< 曝光模式 (0-自动 1-手动 2-快门优先 3-光圈优先)			*/
	int nShutterSpeed;														/*!< 快门速度 (0~15)				                        */
	int nIrisSpeed;															/*!< 光圈 (0-手动 1-自动）									*/
	int nWideDyn;															/*!< 宽动态 (0-关 1-开)										*/
	int n3DFilter;															/*!< 3D降噪	(0-关 1-开)										*/
	int nMirror;															/*!< 镜像  (0-关 1-开)										*/
	int nReverse;															/*!< 翻转  (0-关 1-开)										*/
	int nInputFrequency;													/*!< 输入频率  (0-50Hz 1-60Hz)                              */
	int nLongRestrict;														/*!< 长焦限速（1-开，0-关）									*/
	int nNorthSet;															/*!< 北位设定（1-设定）										*/

} HY_DVR_PTZ_ELCTRONIC_SONY, *LPHY_DVR_PTZ_ELCTRONIC_SONY;

/*!
*  @struct	tagHY_DVR_PTZ_ELCTRONIC_CHANNEL
*  @brief	单个通道的电子ptz配置
*/
typedef struct  tagHY_DVR_PTZ_ELCTRONIC_CHANNEL
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */

	HY_DVR_PTZ_ELCTRONIC_SONY struPtzElcSony;								/*!< SONY电子PTZ机芯配置 */

} HY_DVR_PTZ_ELCTRONIC_CHANNEL, *LPHY_DVR_PTZ_ELCTRONIC_CHANNEL;

/*!
*  @struct	HY_DVR_PTZ_TEMPERATURE_DEVICE
*  @brief	ptz温度设备配置
*/
typedef struct  tagHY_DVR_PTZ_TEMPERATURE_DEVICE
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */

	int	iWorkMode;															/*!< 操作模式: 0自动，1手动									*/
	int	iManualSwitch;														/*!< 手动开关: 0手动关，1手动开								*/
	int	iOpenThreshold;														/*!< 开启温度，高于此温度时开启								*/
	int	iCloseThreshold;													/*!< 关闭温度，低于此温度时开启								*/
} HY_DVR_PTZ_TEMPERATURE_DEVICE, *LPHY_DVR_PTZ_TEMPERATURE_DEVICE;

/*!
*  @struct	HY_DVR_PTZ_ELCTRONIC_CH_TEMPERATURE
*  @brief	单个通道的电子ptz温度设备配置
*/
typedef struct  tagHY_DVR_PTZ_ELCTRONIC_CH_TEMPERATURE
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */

	HY_DVR_PTZ_TEMPERATURE_DEVICE struPtzFanDevice;							/*!< 风扇设备配置 */
	HY_DVR_PTZ_TEMPERATURE_DEVICE struPtzHeaterDevice;						/*!< 加热设备配置 */
	HY_DVR_PTZ_TEMPERATURE_DEVICE struPtzReserveDevice[14];					/*!< 温度设备配置(增加设备时减一) */

} HY_DVR_PTZ_ELCTRONIC_CH_TEMPERATURE, *LPHY_DVR_PTZ_ELCTRONIC_CH_TEMPERATURE;

/*!
*  @struct	tagHY_DVR_PTZ_ELCTRONIC
*  @brief	所有通道的电子ptz配置
*/
typedef struct  tagHY_DVR_PTZ_ELCTRONIC
{
	int bValid;					//! 本结构体是否有效，=0表示无效

	HY_DVR_PTZ_ELCTRONIC_CHANNEL   struPtzElcChannel[DVR_MAX_PTZ_NUM];
	HY_DVR_PTZ_ELCTRONIC_CH_TEMPERATURE   struPtzElcChTemperature[DVR_MAX_PTZ_NUM];

} HY_DVR_PTZ_ELCTRONIC, *LPHY_DVR_PTZ_ELCTRONIC;

/*!
*  @struct	HY_DVR_PTZ_REPLENISH_CONTENT_1
*  @brief	ptz设备补充配置内容
*/
typedef struct  tagHY_DVR_PTZ_REPLENISH_CONTENT_1
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */

	int	iLowPowerConsumption;												/*!< 低功耗: 0关，1开										*/
	int	iScreenWiper;														/*!< 雨刷器: 0关，1单刮，2连刮								*/
	int	iNightVision;														/*!< 夜视：0关，1开，2自动									*/
	int	iDemist;															/*!< 除雾：0关，1开											*/
} HY_DVR_PTZ_REPLENISH_CONTENT_1, *LPHY_DVR_PTZ_REPLENISH_CONTENT_1;

/*!
*  @struct	HY_DVR_PTZ_REPLENISH_1
*  @brief	ptz设备补充配置
*/
typedef struct tagHY_DVR_PTZ_REPLENISH_1
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */
	HY_DVR_PTZ_REPLENISH_CONTENT_1 struContent[DVR_MAX_PTZ_NUM];			/*!< 配置内容 */

} HY_DVR_PTZ_REPLENISH_1, *LPHY_DVR_PTZ_REPLENISH_1;



/*!
*  @struct	tagHY_DVR_DECODER_CFG
*  @brief	设备解码配置
 */
typedef struct tagHY_DVR_DECODER_CFG
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */
	HY_DVR_DECODER_CHANNEL struDecoder[DVR_MAX_PTZ_NUM];					/*!< 各解码通道配置 */
	HY_DVR_PTZ_ELCTRONIC	struPtzElc;										/*!< 电子ptz配置 */
	HY_DVR_PTZ_REPLENISH_1	struReplenish1;									/*!< 补充配置1		*/

} HY_DVR_DECODER_CFG, *LPHY_DVR_DECODER_CFG;

/** @brief 设备解码配置结构体长度																 */
#define  DECODER_CFG_LEN  sizeof(HY_DVR_DECODER_CFG)
///////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////

/*!
*  @struct	tagHY_DVR_ALARMIN_CHANNEL
*  @brief	各通道报警输入配置
 */
typedef struct tagHY_DVR_ALARMIN_CHANNEL
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */

	int nAlarmInType;														/*!< 报警器类型,0：常开,1：常闭  */

	HY_DVR_HANDLEEXCEPTION struHandleType;									/*!< 处理方式  */
	HY_DVR_SCHEDTIME struSchedTime;											/*<! 报警输入布防时间 */

} HY_DVR_ALARMIN_CHANNEL, *LPHY_DVR_ALARMIN_CHANNEL;

/*!
*  @struct	tagHY_DVR_ALARMOUT_CHANNEL
*  @brief	各通道报警输出配置
 */
typedef struct tagHY_DVR_ALARMOUT_CHANNEL
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */
	int nAlarmOutDelay;														/*!< 输出保持时间(0为无限，需要手动关闭)  */
	HY_DVR_SCHEDTIME struSchedTime;											/*!< 报警输出布防时间 */



} HY_DVR_ALARMOUT_CHANNEL, *LPHY_DVR_ALARMOUT_CHANNEL;

/*!
*  @struct	tagHY_DVR_ALARM_CAPTURE_PIC
*  @brief	报警联动抓图配置
 */
typedef struct tagHY_DVR_ALARM_CAPTURE_PIC
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */
	int nCapPicCount;														/*!< 联动快照张数 */
	int nCapPicIntervalTime;												/*!< 联动快照间隔时间 */

	DWORD dwResolution;
	DWORD dwPictureQuality;

} HY_DVR_ALARM_CAPTURE_PIC, *LPHY_DVR_ALARM_CAPTURE_PIC;

/*!
*  @struct	tagHY_DVR_SYSTEM_EXCEPTION
*  @brief	系统异常检测
 */
typedef struct tagHY_DVR_SYSTEM_EXCEPTION
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */
	int bEnable;															/*!< 是否启动检测 */
	int nCheckTime;															/*!< 检测时间间隔，单位为秒（目前不支持） */
	HY_DVR_HANDLEEXCEPTION struHandleType;									/*!< 联动处理方式  */

} HY_DVR_SYSTEM_EXCEPTION, *LPHY_DVR_SYSTEM_EXCEPTION;

/*!
*  @struct	tagHY_DVR_ALARM_CFG
*  @brief	报警设置
 */
typedef struct tagHY_DVR_ALARM_CFG
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */
	HY_DVR_WORKMODE struWorkMode;											/*!< 工作模式(只读) */
	HY_DVR_ALARM_CAPTURE_PIC struCapPic;									/*!< 联动快照 */
	HY_DVR_ALARMIN_CHANNEL struAlarmIn[DVR_MAX_ALARMIN_NUM];				/*!< 报警输入配置 */
	HY_DVR_ALARMOUT_CHANNEL struAlarmOut[DVR_MAX_ALARMOUT_NUM];				/*!< 报警输出配置 */
	HY_DVR_SYSTEM_EXCEPTION struSystemException[DVR_MAX_EXCEPTION_NUM];		/*!< 系统异常处理：0-硬盘故障,1-IP冲突,2-非法访问,3-网线断,4-盘组满,5-盘组异常 */
	HY_DVR_ALIAS_CFG_1 struAlarmInAlias_1[DVR_MAX_ALARMIN_NUM];				/*!< 报警输入通道别名(Unicode) */
	HY_DVR_ALIAS_CFG_1 struAlarmOutAlias_1[DVR_MAX_ALARMOUT_NUM];			/*!< 报警输出通道别名(Unicode) */


} HY_DVR_ALARM_CFG, *LPHY_DVR_ALARM_CFG;

/*!
*  @struct    tagHY_DVR_ALARMIN_EX_CHANNEL
*  @brief    各通道报警输入拓展配置
 */
typedef struct tagHY_DVR_ALARMIN_EX_CHANNEL
{
    int iValid;                                                                /*!< 本结构体是否有效，0表示无效 */

    int iVoiceType; /*!< 0:自定义,1:警报,2:犬吠,3:警戒区域,4:监控区域,5:欢迎光临,6:深水警告,7:私人领地,8:危险区域,9:贵重物品,10:高处危险,
                        11:私家价位,12:叮咚,13:有口罩,14:无口罩,15:注意私人物品,16:抓小偷,17:救命,18:分类,19:可回收垃圾,20:家庭食物垃圾
                        21:危险废物,22:残余垃圾,23:全部分类,24:车形自定义,25:禁止停车 */
    int iBroadCastTime;                                                        /*!< 播报次数  */
    int iDayMode;/*!< 0:夜晚模式,1:白天模式,2:不启用此模式（AllDay字段有效，当字段不为disable时，AllDay字段无效）当设置为此模式时，IPC依赖自身传感器判断当前环境是白天还是夜晚*/
    HY_DVR_SCHEDTIME stSchedTime;                                            /*<! 报警声音布防时间 */

    char acReserved[128];
} HY_DVR_ALARMIN_EX_CHANNEL, *LPHY_DVR_ALARMIN_EX_CHANNEL;

/*!
*  @struct    tagHY_DVR_ALARM_IN_CFG_EX
*  @brief    报警拓展配置
 */
typedef struct tagHY_DVR_ALARM_IN_CFG_EX
{
    int iValid;                                                                /*!< 本结构体是否有效，0表示无效 */

    HY_DVR_ALARMIN_EX_CHANNEL stAlarmIn[DVR_MAX_ALARMIN_NUM];                /*!< 报警输入配置 */

    char acReserved[1024];
} HY_DVR_ALARM_IN_CFG_EX, *LPHY_DVR_ALARM_IN_CFG_EX;

/** @brief 报警自动抓图设置结构体长度																 */
#define ALARM_CFG_LEN   sizeof (HY_DVR_ALARM_CFG)
//////////////////////////////////////////////////////////////////////////////

typedef struct tagHY_DVR_SNAPSHOT_PIC_CAPABILITY
{
	int bValid;
	unsigned int nResolutionCap;								//该参数参考分辨率的能力表
	unsigned short	bSupportDynReso;								// 是否支持动态分辨率.即抓图的分辨率与实际的能力保持一致,不固定.
	unsigned short	cReserve[3];
	unsigned int nResolutionStreamType;							//0，表示此种方式无效，1，主码流，2，第一子码流，3，第二子码流
	char cReserved[244];
}HY_DVR_SNAPSHOT_PIC_CAPABILITY, *LPHY_DVR_SNAPSHOT_PIC_CAPABILITY;

typedef struct tagHY_DVR_ALARM_CFG_EX
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */
	HY_DVR_SNAPSHOT_PIC_CAPABILITY struSnapshotPicCapability;				/*!< 抓图能力(只读) */
	char cReserved[2048];
} HY_DVR_ALARM_CFG_EX, *LPHY_DVR_ALARM_CFG_EX;


/*!
*  @struct	tagHY_DVR_ALARM_CAPTURE_PIC_V2
*  @brief	报警联动抓图配置
 */
typedef struct tagHY_DVR_ALARM_CAPTURE_PIC_V2
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */
	int nCapPicCount;														/*!< 联动快照张数 */
	int nCapPicIntervalTime;												/*!< 联动快照间隔时间 */

	DWORD dwResolutionWidth;
	DWORD dwResolutionHeight;
	DWORD dwPictureQuality;
	char nStreamTypeValid;													/*码流类型是否有效，0表示无效，1表示有效*/
	char nStreamType;														/*码流类型，0表示主码流，1表示第一子码流，2表示第二子码流*/
    int nSupportStreamType;                                                 /*支持的码流类型，按位获取，第0位表示主码流，第一位表示子码流，依此类推*/
    int nChannel;
	char cReserve[22];

} HY_DVR_ALARM_CAPTURE_PIC_V2, *LPHY_DVR_ALARM_CAPTURE_PIC_V2;


/*!
*  @struct	tagHY_DVR_FRAMERATE_MAP
*  @brief	每一种分辨率对应的最大帧率
 */

typedef struct tagHY_DVR_RESOLUTION_INFO
{
	int nResolutionWidth;
	int nResolutionHeight;
    int nChannel;
	char cReserve[32 - 4];
}HY_DVR_RESOLUTION_INFO;

typedef struct tagHY_DVR_SNAPSHOT_PIC_CAPABILITY_V2
{
	int bValid;
	HY_DVR_RESOLUTION_INFO struResolutionInfo;
	struct tagHY_DVR_SNAPSHOT_PIC_CAPABILITY_V2 * pNext;
}HY_DVR_SNAPSHOT_PIC_CAPABILITY_V2, *LPHY_DVR_SNAPSHOT_PIC_CAPABILITY_V2;

typedef struct tagHY_DVR_ALARM_CFG_EX_V2
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */
	HY_DVR_SNAPSHOT_PIC_CAPABILITY_V2 struSnapshotPicCapabilityV2;				/*!< 抓图能力(只读) */
} HY_DVR_ALARM_CFG_EX_V2, *LPHY_DVR_ALARM_CFG_EX_V2;



/////////////////////////////////////////////////////////////

/*!
*  @struct	tagHY_DVR_VO_CFG
*  @brief	显示参数
 */
typedef struct tagHY_DVR_VO_CFG
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */
	int nScreenSaveTime;													/*!< 屏幕保护时间：0表示从不启动屏幕保护，60s-3600s表示屏幕保护时间）  */
	int nMenuAlphaValue;													/*!< 菜单与背景图象对比度：0表示不透明，4表示4:1，8表示3:1  */

} HY_DVR_VO_CFG, *LPHY_DVR_VO_CFG;

/** @brief 显示参数结构体长度																 */
#define VO_CFG_LEN   sizeof (HY_DVR_VO_CFG)
/////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////

/*!
*  @struct	tagHY_DVR_USER_ENABLE
*  @brief	用户使能信息
 */
typedef struct tagHY_DVR_USER_ENABLE
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */
	int bEnable;															/*!< 是否启用:0表示禁用，1表示启用 */

} HY_DVR_USER_ENABLE, *LPHY_DVR_USER_ENABLE;

/*!
*  @struct	tagHY_DVR_USER_INFO
*  @brief	用户基本信息
 */
typedef struct tagHY_DVR_USER_INFO
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */
	char szPassword[DVR_MAX_PASSWORD_LEN];									/*!< 密码  */
	unsigned char byLimitMac[DVR_MAX_MAC_NUM_LEN];							/*!< 限制访问物理地址  */
	char szLimitIP[DVR_MAX_IP_STR_LEN];										/*!< 限制访问IP地址  */
} HY_DVR_USER_INFO, *LPHY_DVR_USER_INFO;

/*!
*  @struct	tagHY_DVR_USER_RIGHT
*  @brief	用户权限信息
 */
typedef struct tagHY_DVR_USER_RIGHT
{
	int bValid;								/*!< 本结构体是否有效，0表示无效 */
	int nLocalRight[DVR_MAX_RIGHT_NUM];		/*!< 本地权限：数组0-本地系统设置；1-文件管理；2-录像备份；3-系统关闭；4-邮件通知 */
	int nRemoteRight[DVR_MAX_RIGHT_NUM];	/*!< 远程权限：数组0-系统设置；1-语音对讲；2-串口输出；3-报警输出；4-远程升级；5-关闭系统 */
	int nChannelRight[DVR_MAX_RIGHT_NUM];	/*!< 通道权限：数组0-本地回放；1-本地预览；2-本地云台控制；3-本地手动录像；4-远程回放；5-远程预览；6-远程云台控制；7-远程手动录像 */

} HY_DVR_USER_RIGHT, *LPHY_DVR_USER_RIGHT;

/*!
*  @struct	tagHY_DVR_ONLINEUSER
*  @brief	在线用户信息
*/
typedef struct tagHY_DVR_ONLINEUSER
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */

	int bRemote;															/*!< 是否为远程用户	*/
	int bConnect;															/*!< 是否处于连接状态 */
	DWORD dwUserID;															/*!< 用户句柄 */
	DWORD dwProtocolID;														/*!< 登陆协议 */
	char szUserName[DVR_MAX_USERNAME_LEN];									/*!< 用户名 */
	DWORD dwIPAddress;														/*!< 登陆IP */
	HY_DVR_TIME struLoginTime;												/*!< 登陆时间 */
	DWORD dwBitrate;														/*!< 网络流量 b/s */

}HY_DVR_ONLINEUSER, *LPHY_DVR_ONLINEUSER;

/*!
*  @struct	tagHY_DVR_ONLINEUSER_CFG
*  @brief	在线用户配置
*/
typedef struct tagHY_DVR_ONLINEUSER_CFG
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */

	HY_DVR_ONLINEUSER struOnlineUser[DVR_MAX_ONLINEUSER_NUM];				/*!< 在线用户列表 */

} HY_DVR_ONLINEUSER_CFG, *LPHY_DVR_ONLINEUSER_CFG;

/*!
*  @struct	tagHY_DVR_WHITEIP_ENABLE
*  @brief	白名单使能
*/
typedef struct tagHY_DVR_WHITEIP_ENABLE
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */

	int bEnable;															/*!< 是否启用IP白名单*/
} HY_DVR_WHITEIP_ENABLE, *LPHY_DVR_WHITEIP_ENABLE;

/*!
*  @struct	tagHY_DVR_WHITEIP
*  @brief	白名单用户信息
*/
typedef struct
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */

	DWORD dwWhiteIP;														/*!< 白名单IP地址 */

} HY_DVR_WHITEIP, *LPHY_DVR_WHITEIP;

/*!
*  @struct	tagHY_DVR_BLACKIP_ENABLE
*  @brief	黑名单使能
*/
typedef struct
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */

	int bEnable;															/*!< 是否启用IP黑名单*/
} HY_DVR_BLACKIP_ENABLE, *LPHY_DVR_BLACKIP_ENABLE;

/*!
*  @struct	tagHY_DVR_BLACKIP
*  @brief	黑名单用户信息
*/
typedef struct
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */

	DWORD dwBlackIP;														/*!< 黑名单IP地址 */

} HY_DVR_BLACKIP, *LPHY_DVR_BLACKIP;

/*!
*  @struct	tagHY_DVR_BLACKIP
*  @brief	黑名单用户列表
*/
typedef struct
{
	HY_DVR_BLACKIP struBlackIP[DVR_MAX_BLACKIP_NUM];						/*!< 黑名单信息 */
	HY_DVR_BLACKIP_ENABLE struBlackIPEnable;								/*!< 黑名单使能信息 */
}HY_DVR_BLACKIP_LIST, *LPHY_DVR_BLACKIP_LIST;

// 系统配置——用户配置
typedef enum
{
	eZWUserLevel_Administrator,
	eZWUserLevel_DefaultUser,
	eZWUserLevel_Normal,
	eZWUserLevel_OnvifUser,
	eZWUserLevel_OnvifOpreator,
}eZWUserLevel;

typedef enum
{
	eZWLocalAuthority_Configure = 0x01,
	eZWLocalAuthority_Advanced = 0x02,
	eZWLocalAuthority_PoweroffOrReboot = 0x04,
	eZWLocalAuthority_Backup = 0x08,
	eZWLocalAuthority_ClearAlarm = 0x10,
}eZWLocalAuthority;

typedef enum
{
	eZWRemoteAuthority_Configure = 1,
	eZWRemoteAuthority_Advanced = 2,
	eZWRemoteAuthority_PoweroffOrReboot = 4,
	eZWRemoteAuthority_SerialPortOutput = 8,
	eZWRemoteAuthority_RealTalk = 16,
	eZWRemoteAuthority_Alarmout = 32,
	eZWRemoteAuthority_Backup = 64,
	eZWRemoteAuthority_ClearAlarm = 128,
	eZWRemoteAuthority_FaceGroupManage = 256,			//脸库管理权限
	eZWRemoteAuthority_TransparnetChannel = 512,			//透明通道权限
	eZWRemoteAuthority_Update = 1024,					//远程升级
	// 在本行上面添加新扩展的权限
}eZWRemoteAuthority;

typedef struct tagZW_AUTHORITY
{
	int bValid;

	DWORD dwLocalAuthorityMask;						// eZWLocalAuthority枚举的组合值
	DWORD dwRemoteAuthorityMask;					// eZWRemoteAuthority枚举的组合值
	DWORD dwLocalPreviewMask;						// 每一位表示对应的通道是否拥有此项权限,比如0x02表示有第二个通道的本地通览权限
	DWORD dwLocalPlaybackMask;						// ...............................................................本地回放权限
	DWORD dwLocalPTZMask;							// ...............................................................本地PTZr控制权限
	DWORD dwLocalManualControlMask;					// ...............................................................本地手动控制权限
	DWORD dwRemotePreviewMask;						// ...............................................................远程预览权限
	DWORD dwRemotePlaybackMask;						// ...............................................................远程回放权限
	DWORD dwRemotePTZMask;							// ...............................................................远程PTZ控制权限
	DWORD dwRemoteManualControlMask;				// ...............................................................远程手动控制权限
}ZW_AUTHORITY;

//typedef struct tagZW_AUTHORITYEX
//{
//	int nValid;
//
//	DWORD dwVoiceTalkPermission;
//	char cRemoteConfigPermission;
//
//	char cReserved[3];
//	DWORD dwReserved[5];
//}ZW_AUTHORITYEX;

typedef enum ZW_USER_BASIC_CFG_USAGE
{
	ZW_USER_UNKNOWN,

	ZW_USER_UPDATE_PASSWORD,
	ZW_USER_CREATE,
	ZW_USER_PERMISSION,
}ZW_USER_BASIC_CFG_USAGE;

typedef struct tagZW_USER_BASIC_CFG
{
	int bValid;																		/*!< 本结构体是否有效，0表示无效  */

	char szUserName[DVR_MAX_USERNAME_LEN];
	char szUserPassword[DVR_MAX_PASSWORD_LEN];
	eZWUserLevel level;
	DWORD bBindMAC;
	char szBindMAC[DVR_MAX_MAC_STR_LEN];

	ZW_AUTHORITY struAuthority;
	//ZW_AUTHORITYEX struAuthorityEx;
	ZW_USER_BASIC_CFG_USAGE eUsage;
	DWORD iIsPasswordEncrypted;     												/*!< 用户密码是否base64加密;0-否，1-是，默认值为0 */

	DWORD dwReserved[14];
}ZW_USER_BASIC_CFG;

typedef enum
{
	Device_Status_Begin = 0,	// 起始值

	Device_Status_Inactive = 1, // 未激活
	Device_Status_Active = 2,	// 激活
	Device_Status_Locked = 3,	// 锁定

	Device_Status_End, // 结束值
} eDeviceStatusInfo;

typedef struct tagHY_DVR_DEVICE_STATUS_INFO
{
	int iValid; /*!< 本结构体是否有效，0表示无效 */

	eDeviceStatusInfo eStatus; /*!< 设备状态 */
	int iRemainLockDuration;   /*!< 剩余锁定时长 */
	int iPwdErrorCount;	   /*!< 密码连续错误次数 */

	char szReserved[64];
} HY_DVR_DEVICE_STATUS_INFO, *LPHY_DVR_DEVICE_STATUS_INFO;

/*!
*  @struct	tagHY_DVR_USER_CFG
*  @brief	用户配置信息
 */
typedef struct tagHY_DVR_USER_CFG
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */

	ZW_USER_BASIC_CFG struUserBasic[DVR_MAX_USER_NUM];						// 新增
	HY_DVR_ONLINEUSER_CFG struUserOnline;									/*!< 在线用户信息 */
	HY_DVR_WHITEIP struWhiteIP[DVR_MAX_WHITEIP_NUM];						/*!< 白名单信息 */
	HY_DVR_WHITEIP_ENABLE struWhiteIPEnable;								/*!< 白名单使能信息 */
} HY_DVR_USER_CFG, *LPHY_DVR_USER_CFG;

/** @brief 用户配置信息结构体长度														 */
#define USER_CFG_LEN	sizeof(HY_DVR_USER_CFG)
/////////////////////////////////////////////////////////////

/*!
*  @struct	tagHY_DVR_WHITE_RULE
*  @brief	白名单规则
*/
typedef struct
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */

	DWORD dwID;																/*!< 白名单ID 					 */
	DWORD dwMode;															/*!< 白名单规则，0-不支持，1-指定IP，2-指定IP段 */
	DWORD dwWhiteIP;														/*!< 白名单IP地址 				 */
	DWORD dwWhiteIPEnd;														/*!< 白名单IP结束地址（IP段模式）*/

	char  cReserve[32];
} HY_DVR_WHITE_RULE, *LPHY_DVR_WHITE_RULE;

/*!
*  @struct	tagHY_DVR_WHITE_LISTS
*  @brief	白名单记录
*/
typedef struct
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */

	DWORD dwEnable;															/*!< 白名单使能，0-不支持，1-启用，2-禁用 */
	HY_DVR_WHITE_RULE cRules[DVR_MAX_WHITE_RULE_NUM];						/*!< 白名单规则列表  				 */

	char  cReserve[128];
} HY_DVR_WHITE_LISTS, *LPHY_DVR_WHITE_LISTS;

/////////////////////////////////////////////////////////////

/*!
*  @struct	tagHY_DVR_DEVICE_INFO
*  @brief	设备信息结构(只读)
 */
#define DVR_MAX_SN_LEN			(32)
typedef struct tagHY_DVR_DEVICE_INFO
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */
	DWORD dwBoardType;														/*!< 板型编号 */
	char szTypeName[DVR_MAX_VERSION_LEN];									/*!< 设备型谱名称，如ZW-2016HC */
	char szSerialNum[DVR_MAX_SN_LEN];												/*!< 序列号  */
	char szHardwareVersion[DVR_MAX_VERSION_LEN];							/*!< 硬件版本 */
	char szSoftwareVersion[DVR_MAX_VERSION_LEN];							/*!< 软件版本 */
	char szDspVersion[DVR_MAX_VERSION_LEN];									/*!< 编码版本 */

	unsigned char byVideoInChannels;										/*!< 视频输入路数  */
	unsigned char byAudioInChannels;										/*!< 音频输入路数  */
	unsigned char byAlarmInChannels;										/*!< 报警输入路数  */
	unsigned char byAlarmOutChannels;										/*!< 报警输出路数  */

	unsigned char byVGAChannels;											/*!< VGA接口个数  */

	unsigned char bySpotOutChannles;										/*!< SPOT输出路数  */
	unsigned char byVoiceInChannels;										/*!< 语音对讲输入路数  */

} HY_DVR_DEVICE_INFO, *LPHY_DVR_DEVICE_INFO;

/** @brief 设备信息结构体长度														 */
#define	DEVICE_INFO_LEN	sizeof(HY_DVR_DEVICE_INFO)

typedef struct tagHY_DVR_DEVICE_INFO_SERIAL_NUM
{
	int nValid;
	int  nSerialCustom;														/*!< 序列定制：远程只读	*/
	char szSerialNum[DVR_MAX_SN_LEN];										/*!< 序列号		*/
	char cReserve[32];
}HY_DVR_DEVICE_INFO_SERIAL_NUM;

typedef struct tagHY_DVR_DEVICE_INFO_V2
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */
	HY_DVR_DEVICE_INFO_SERIAL_NUM struSerial;								/*!< 序列号  */

	char cReserve[1024];
} HY_DVR_DEVICE_INFO_V2, *LPHY_DVR_DEVICE_INFO_V2;

typedef struct tagHY_DVR_DEVICEINFO_FOR_ONVIF_MULTI
{
	int iValid;											/*!< 本结构体是否有效，0表示无效 */
	int iIsMultiChannel;								/*!< 是否是真的多通道,还是假的多通道;0-假的多通道,1-真的多通道  */

	char acReserve[128];
} HY_DVR_DEVICEINFO_FOR_ONVIF_MULTI, *LPHY_DVR_DEVICEINFO_FOR_ONVIF_MULTI;

/** @brief 设备信息结构体长度														 */
#define	DEVICE_INFO_V2_LEN	sizeof(HY_DVR_DEVICE_INFO_V2)

#define MAX_DEVICE_CAP_FILE_NAME	256
typedef struct tagHY_DVR_DEVICE_CAP_FILE_NAME
{
	int nValid;
	char strDeviceCapFileName[MAX_DEVICE_CAP_FILE_NAME];
}HY_DVR_DEVICE_CAP_FILE_NAME;


typedef struct tagHY_DVR_DEVICE_CAP_FILE_NAME_V2
{
	int nValid;
	char strDeviceCapFileName[MAX_DEVICE_CAP_FILE_NAME];
}HY_DVR_DEVICE_CAP_FILE_NAME_V2;
//////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////

/*!
*  @struct	tagHY_DVR_DEVICE_INFO_EX
*  @brief	设备信息扩展结构(只读)
 */
#ifdef _WIN32
#pragma pack(1)
#endif
typedef struct tagHY_DVR_DEVICE_INFO_EX
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */
	int nEnableSmartAVC[DVR_MAX_VIDEOIN_NUM];								/*!< 表示设备的通道是否支持smartAVC，0，表示不支持，1，表示支持*/
	char cHdd;
	char cSD;
	char cAudio;
	char cPTZ;
	char cAuthState;
	char cRejectEmptyPassword;												/*!< 表示是否拒绝空密码登录，0表示支持，1表示拒绝*/
	//扩展字段起始
	char bValidAlgorithmVersion;											/*!< 算法版本是否有效 */
	char szAlgorithmVersion[DVR_MAX_VERSION_LEN];							/*!< 算法版本 */
	char bValidSessionID;													/*!< 会话ID是否有效 */
	char bytSessionID[8];													/*!< 设备会话ID，设备动态生成，短时有效	*/
	char bDeviceShared;
	char cCustomSubTypeName[16];
	char bDeviceAutoUnbind;													/* 设备自动解绑能力 */
	char cTokenSupportStatus;												/* Token支持状态	0：不支持Token	1：支持但不启用		2：支持并启用 */
	char cIsHttpRetCodeInXml;												/* 非/System/UserAuth时HTTP错误码是否放到XML中，不放在HTTP头中，直接在HTTP头中给401的话，浏览器会弹出用户登录认证的提示框 0：不放在XML中	1：放在XML中，目前旷视设备给1*/
	char cSaveTwoSecondsRealStream;											/*!< 私有协议服务端发送实时流的时候是否只缓存两秒的数据	*/
	char cUseSliceToSend;													/*!< 使用切片发送模式，0：不使用，1：使用	*/
	int  iP6SSocketSendBuffer;												/*!< P6S协议发送缓冲区大小（单位字节） */
	int  iP6SSocketReceiveBuffer;											/*!< P6S协议接收缓冲区大小（单位字节） */
	char szChipID[64];														/*芯片实际ID*/
	char cPositionCapability;												//定位功能支持情况，0，不支持，1，支持
	char cDownloadSpeed;													//下载倍速范围，0,1/2/4,1,1/2/4/8,3,1/2/4/8/16
	char cEncode;															//是否编码，0，没有编码，1，有编码			
	char cReason;															//不正常工作的原因，0,正常，1，网络问题，2，密码错误
	AbsoluteTime struLastUpgradeTime;							//最后一次升级时间
	int nIsSupportStrongPassword;								//是否支持强密码
	int nIsSupportProductTestToolV2;							//是否支持V2产测工具
	int nIsSupportTheSecondDeviceVersion;						//是否支持第二个版本号，用于P6SMessage协议，0，表示不支持，1，表示支持
	char cTheSecondDeviceVersion[32];							//设备的第二个版本号管理，格式是"主版本.子版本.修订版本-日期" 例如：1.0.0-20230412
	int nIsSupportMainAndSubCaptureMode;							//是否支持同主子码流抓图模式
	int nIsCustom;												//是否客户自定义版本 1:支持，0：不支持
	char cCustomVersion[32];										//客户自定义版本信息
    char cMCUVersion[32];                                           // MCU 版本
    char cDeviceName[32];                                           // 设备名称
    char cDeviceOriginalMode[32];                                   // 设备原始型号
	int iOnvifReviewEnable;											// 是否使能onvif评审;0-去使能,1-使能
	int nIsSupportBOA;                                              // 是否支持 boa，0 - 默认值，支持；1 - 不支持。用于 G4W AOV 模式
	int nIsGetCapFileSize;                                          // 是否获取能力文件大小。0 - 否； 1 - 是
    int nIsSupportReleativeMove;                                    // 是否支持云台以坐标形式进行相对移动， 0 - 默认值，不支持； 1 - 支持
    int nIsNotVerifyToken;                                          // 是否不校验 token（onvif）， 0 - 否（即保持以前逻辑，仍然校验 token）； 1 - 是（不校验 token）
	char acAFBoardVersion[64];                                      // AF机芯版本号(只读,机芯的程序也会需要更新,通过机芯版本号区分机芯的程序版本,机芯程序更新升级后会更新AF机芯版本号)
    int nIsCacheOnvifConfig;                                        // 是否缓存 onvif 配置。 0 - 默认值，使用缓存机制；1 - 不适用缓存机制
	int nisSupportV2ActiveCode;										// 是否支持V2的激活码, 0,无效值，1，支持V2激活码，2，不支持V2激活码

	char cReserve[594 - 4];
#ifdef _WIN32
}HY_DVR_DEVICE_INFO_EX, *LPHY_DVR_DEVICE_INFO_EX;
#pragma pack()
#endif
#ifdef __linux__
}__attribute__((packed)) HY_DVR_DEVICE_INFO_EX, *LPHY_DVR_DEVICE_INFO_EX;
#endif

/** @brief 设备信息结构体长度														 */
#define	DEVICE_INFO_EX_LEN	sizeof(HY_DVR_DEVICE_INFO_EX)

typedef struct tagHY_DVR_ONVIF_CUSTOMIZED
{
int nValid;
int isSupportMedia2; 
/* 0:不支持 media2 , 1:支持 media2 */ 
int isSupportNoAuthSetSystemDateAndTime;/* 0:不支持 没有鉴权设置时间 , 1:支持 没有鉴权设置时间 */ 
int nOnvifToolTestMode;										//onvif官方工具测试模式，0.表示正常模式，1。表示测试模式
int nOnvifOSDPosCap;                                            // onvif 协议 OSD 位置能力配置。0 - 默认值，方位和坐标都支持；1 - 只支持坐标；2 - 只支持方位
char Resevered[116];
} HY_DVR_ONVIF_CUSTOMIZED, *LPHY_DVR_ONVIF_CUSTOMIZED;

typedef struct tagHY_DVR_ONVIF_CUSTOMIZED_EX
{
    int nValid;
    int isSupportCustomURL;     //是否支持自定义URL
    char MainStreamURL[128];		//主码流的URL
    char SubStreamURL[128];			//子码流的URL
    int isSupportNoUserAuthorization;  //是否支持不用用户认证得方式获取流（0表示需要用户认证，1表示不需要用户认证）
    int isSupportNoAuth;            // 是否支持取消所有鉴权
    int isSupportCustomSnapshotURL;     //是否支持抓图自定义URL
    char strSnapshotURL[128];		//抓图URL
    int nManufacturerType;          // 厂家类型。 1 - 赛特美
    char Resevered[112];
} HY_DVR_ONVIF_CUSTOMIZED_EX, *LPHY_DVR_ONVIF_CUSTOMIZED_EX;

/*!
*  @struct	tagVIDEONETCLIENT_DEVICE_INFO_EX
*  @brief	用户自定义数据,协议不关注具体内容
 */
typedef struct tagVIDEONETCLIENT_USER_DEFINED_TEXT
{
	int  nValid;
	char acData[10 * 1024];													/*!< 可以是任何文本内容,由中间层,应用层自行定义 */
} HY_DVR_USER_DEFINED_TEXT, *LPHY_DVR_USER_DEFINED_TEXT;


//////////////////////////////////////////////////////////////////////////////
typedef struct tagHY_DVR_CUSTOMER_INFO
{
	char szCountry[32];				// 国家
	char szCity[32];				// 城市
	char Manaufacture[32];			// 生产商
	char Model[32];					// 型号
	char DeviceName[32];			// 设备名称

	char bytDiscoveryMode;			// 是否允许探测
	char Resevered_0[3];			// 保留(字节对齐)

	void* pUnicode2GB;				// Unicode转GB的函数指针(只读)
	void* pGB2Unicode;				// GB转Unicode的函数指针(只读)
	char cGBCmdSafeMode;			// 信令安全模式，缺省为0;		0:不采用;	2:S/MIME 签名方式;		3:S/MIME加密签名同时采用方式;		4:数字摘要方式
	char cGBRegisterWay;			// 注册方式,缺省为1;		1:符合IETFRFC3261标准的认证注册模式;		2:基于口令的双向认证注册模式		3:基于数字证书的双向认证注册模式
	char cSecrecy;					// 保密属性，缺省为0;	0:不涉密,	1:涉密
	char cOwner[32];				//设备归属

	int nValid;

	char cCivilCode[32];			//行政区域，比如：430000

	char HostName[32];				// 主机名称

    
    int iOnvifReviewEnable;
	char Resevered[1932 - 4];
}HY_DVR_CUSTOMER_INFO, *LPHY_DVR_CUSTOMER_INFO;

typedef struct
{
	int bValid;

	HY_DVR_CUSTOMER_INFO stDvrCustomerInfo[DVR_MAX_VIDEOENC_NUM];
}HY_DVR_CUSTOMER_INFO_LIST, *LPHY_DVR_CUSTOMER_INFO_LIST;

//////////////////////////////////////////////////////////////////////////////

// 夏令时时间点周表示方法，常用
typedef struct tagZW_DSTWEEKMODE_TIMEPOINT_CFG
{
	int bValid;																		/*!< 本结构体是否有效，0表示无效  */

	unsigned char bytStartMonth;								// 月份  (范围:0-11)
	unsigned char bytWhichWeek;								// 第几周(范围:0-6,6表示最后一周)
	unsigned char bytWeekDay;								// 星期几(范围:0-6)

	unsigned char bytHour;									// 小时[范围:0-23]							
	unsigned char bytMinute;									// 分钟[范围:0-59]
	unsigned char bytSecond;									// 秒[范围:0-59]

	unsigned char bytReserved[2];
}ZW_DSTWEEKMODE_TIMEPOINT_CFG;

typedef enum
{
	eDSTTimeFormat_Week,
	eDSTTimeFormat_Date,
}eDSTTimeFormat;

/*!
*  @struct	tagHY_DVR_DST
*  @brief	夏时制信息结构(系统配置——通用配置——夏令时)
 */
typedef struct tagHY_DVR_DST
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */
	int bEnableDST;															/*!< 是否启用夏时制:0表示禁用，1表示启用 */

	HY_DVR_TIME struStart;													/*!< 月模式开始时间，年无效 */
	HY_DVR_TIME struEnd;													/*!< 月模式结束时间，年无效 */
	int nDSTBias;															/*!< 调整时间，以秒为单位，正数表示延时，负数表示提前，取值范围60-3600 */

	eDSTTimeFormat   dwMode;							// 0,周模式，1,日模式
	ZW_DSTWEEKMODE_TIMEPOINT_CFG struWeekModeStart;		// 周模式开始时间，dwMode为eDSTTimeFormat_Week时间有效
	ZW_DSTWEEKMODE_TIMEPOINT_CFG struWeekModeStop;		// 周模式结束时间，dwMode为eDSTTimeFormat_Week时间有效
} HY_DVR_DST, *LPHY_DVR_DST;

//对应激活功能的掩码位
#define HY_DVR_ACTIVE_FUNCTION_RTMP					0x1			//推流摄像机
#define HY_DVR_ACTIVE_FUNCTION_AF					0x2			//汇讯视通AF
#define HY_DVR_ACTIVE_FUNCTION_CAY_AF				0x4			//菲特AF
#define HY_DVR_ACTIVE_FUNCTION_DL					0x8			//双光源
#define HY_DVR_ACTIVE_FUNCTION_FACE_SNAP			0x10		//人脸抓拍
#define HY_DVR_ACTIVE_FUNCTION_FACE_RECOGNITION		0x20		//人脸对比
#define HY_DVR_ACTIVE_FUNCTION_PEOPLE_TRACK			0x40		//人形坐标推送-跟踪
#define HY_DVR_ACTIVE_FUNCTION_AI_STICK				0x80		//智能棒激活
#define HY_DVR_ACTIVE_FUNCTION_MEGVII_FACE_SNAP		0x100		//旷视人脸抓拍
#define HY_DVR_ACTIVE_FUNCTION_ADVANCED_PEOPLE_TRACK			0x200		//高级人形跟踪
#define HY_DVR_ACTIVE_FUNCTION_FACE_SNAPSHOT	0x400			//高级人形跟踪
#define HY_DVR_ACTIVE_FUNCTION_FACE_DETECTION  0x1000			//海思人脸检测
#define HY_DVR_ACTIVE_FUNCTION_CONSUM_UVC  0x2000				//消费类UVC（USB视频）
#define HY_DVR_ACTIVE_FUNCTION_VEHICLE_REGISTRATION_PLATE  0x4000	//车牌
#define HY_DVR_ACTIVE_FUNCTION_TRAFFIC_OPERATION  	0x8000		//流量运营
#define HY_DVR_ACTIVE_FUNCTION_GAS_DETECTION  		0x10000		//气体检测
#define HY_DVR_ACTIVE_FUNCTION_VM_PLATE				0x20000		//越南车牌
#define HY_DVR_ACTIVE_FUNCTION_FIREWORKS_DETECTION	0x40000		//烟火检测
#define HY_DVR_ACTIVE_FUNCTION_EBIKE				0x80000		//电动车检测(包含煤气罐检测)
#define HY_DVR_ACTIVE_FUNCTION_LPR_TURKEY			0x100000	//土耳其车牌识别
#define HY_DVR_ACTIVE_FUNCTION_RTMP_REGISTER		0x200000	//巴西客户主动注册
#define HY_DVR_ACTIVE_FUNCTION_XRS_AF				0x400000	//XRS_AF
#define HY_DVR_ACTIVE_FUNCTION_LPRIRN           	0x800000	//YL车牌识别
#define HY_DVR_ACTIVE_FUNCTION_LPRJOR           	0x1000000	//约旦车牌识别
#define HY_DVR_ACTIVE_FUNCTION_LPRVEN           	0x2000000	//委内瑞拉车牌识别
#define HY_DVR_ACTIVE_FUNCTION_LPRIND           	0x4000000	//印度车牌识别
#define HY_DVR_ACTIVE_FUNCTION_LPREUP           	0x8000000	//欧盟车牌识别
#define HY_DVR_ACTIVE_FUNCTION_ARRAY_1_LPRMCS		0x1			//南共车牌激活码（南方共同市场）
#define HY_DVR_ACTIVE_FUNCTION_ARRAY_1_LPRRUS		0x2			//俄罗斯车牌激活码
#define HY_DVR_ACTIVE_FUNCTION_ARRAY_1_LPRUCM		0x4			//美加墨车牌激活码


#define TEST_EASY_ACTIVE_V_1_5_0 // 测试时, 将V2版本所有的激活信息返回, 后续实际增加功能时, 参照以前的逻辑/约定，单独新增命名节点, 并删除这里的测试代码
#ifdef TEST_EASY_ACTIVE_V_1_5_0

#define ACTIVE_CODE_SUPPERT_33		(33)		//支持33激活功能
#define ACTIVE_CODE_SUPPERT_66		(66)		//支持66激活功能
#define ACTIVE_CODE_SUPPERT_99		(99)		//支持99激活功能
#define ACTIVE_CODE_SUPPERT_132		(132)		//支持132激活功能
#define ACTIVE_CODE_SUPPERT_255		(255)		//支持255激活功能

#define ACTIVE_CODE_SUPPERT_35		(35)		//支持35激活功能
#define ACTIVE_CODE_SUPPERT_69		(69)		//支持69激活功能
#define ACTIVE_CODE_SUPPERT_254		(254)		//支持254激活功能

#define ACTIVE_CODE_SUPPERT_1_1		(0x1)		//支持33激活功能
#define ACTIVE_CODE_SUPPERT_2_2		(0x2)		//支持66激活功能
#define ACTIVE_CODE_SUPPERT_3_3		(0x3)		//支持99激活功能
#define ACTIVE_CODE_SUPPERT_4_4		(0x4)		//支持132激活功能
#define ACTIVE_CODE_SUPPERT_7_31	(0x40000000)		//支持255激活功能

#define ACTIVE_CODE_SUPPERT_1_3		(0x4)		//支持35激活功能
#define ACTIVE_CODE_SUPPERT_2_5		(0x10)		//支持69激活功能
#define ACTIVE_CODE_SUPPERT_7_30	(0x20000000)		//支持254激活功能

#endif

// 系统配置——通用配置——一般页面
typedef struct tagZW_SYS_COMMON_NORMAL_CFG
{
	int bValid;																		/*!< 本结构体是否有效，0表示无效  */

	HY_DVR_ALIAS_CFG_1 strDeviceName;
	DWORD dwDeviceID;							// 设备ID
	DWORD dwVideoOutputStandard;				// 视频输出制式
	DWORD dwLanguage;							// 系统语言
	DWORD dwAutoLogoutDelayByMinute;			// 自动注销延迟时间(分钟)
	DWORD dwScreenSaverDelayByMinute;			// 屏保延迟时间(分钟)
	DWORD dwMenuBackgroundContrast;				// 菜单背景对比度

	char  nEnableFlag;							//是否是具有车牌标记功能的IPC	
	char  bSupportActiveCodeFlag;				//是否支持激活码,默认表示RTMP激活
	char  bActiveStatus;						//激活状态，默认表示
	char  cReserverd[1];						//保留位
	char  szActiveCode[64];
	char  acDeviceTypeDesc[8];					// "IPC","NVR","DVR","PVR","IPD"
	unsigned int uSupportActivedFeatureMask;    //当前设备支持特性掩码(RTMP-0/AF-1/...)
	unsigned int uAllActivedStatusMask;         //当前设备各特性激活状态掩码(RTMP-0/AF-1/...)
	unsigned int unAllActivedFunctionMask;		//设置激活码操作，使用掩码的方式，如果对应的掩码位为0，表示该功能不进行设置，如果为1，表示激活该功能	
	int nWSDiscoveryEnable;					//0：不支持		1：关闭			2：开启
	int nWebEnable;								//0：不支持		1：关闭			2：开启
	char  acReserved[160];						// 保留(方便日后扩展)
}ZW_SYSCOMMON_NORMAL_CFG;

// 系统配置——通用配置——日期和时间
typedef enum {
	eZWDateFormat_YYYYMMDDWhhmmss,
	eZWDateFormat_YYYYMMDDhhmmss,
	eZWDateFormat_MMDDYYYYWhhmmss,
	eZWDateFormat_MMDDYYYYhhmmss,
	eZWDateFormat_DDMMYYYYWhhmmss,
	eZWDateFormat_DDMMYYYYhhmmss,
	eZWDateFormat_DDMMYYYYWhhmmssV2,
	eZWDateFormat_DDMMYYYYhhmmssV2,
}eZWDateFormat;
typedef enum
{
	eZWTimeFormat_12Hour,
	eZWTimeFormat_24Hour,
}eZWTimeFormat;

/*!
*  @struct	tagHY_DVR_NET_NTP
*  @brief	Ntp服务器配置信息
 */
typedef struct tagHY_DVR_NTP_CFG
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */

	int bNtpEnable;												/*!< 0-不启用, 1-启用  */
	char szNtpHostAddress[DVR_MAX_DOMAINNAME_LEN];				/*!< Ntp服务器，长度考虑域名  */
} HY_DVR_NTP_CFG, *LPHY_DVR_NTP_CFG;

typedef struct tagZW_SYS_COMMON_DATETIME_CFG
{
	int bValid;																		/*!< 本结构体是否有效，0表示无效  */

	eZWDateFormat	dwDateFormat;						// 日期显示格式
	eZWTimeFormat	dwTimeFormat;						// 时间显示格式

	DWORD dwTimeZone;
	HY_DVR_TIME struSysTime;					// 系统时间
	HY_DVR_NTP_CFG struNtp;

	int		dwEnableTimeZoneV2;						//对第二种方案
	int		dwTimeZoneV2;							//时区，以分钟为单位
	unsigned int nTimeSyncInterval;					//时间同步间隔
	int     dwSyncNTPFlag;							//是否同步NTP(默认同步NTP),1:同步,0:不同步;xml对应节点值为0表示不同步,其它情况表示同步
    int dwChannel;                                  //通道号
	int dwSyncAllTimeFlag;                          //是否同步所有时间标识; 0表示不同步，1表示同步

	char	acReserved[236 - 4];						//保留(方便日后扩展)
}ZW_SYSCOMMON_DATETIME_CFG;


typedef enum
{
	eESTATWorkMode_RecordOrBackup,
	eESTATWorkMode_Snapshot,
}eESTATWorkMode;
typedef enum
{
	eAudioInInterface_Microphone,
	eAduioInInterface_LineIn,
}eAudioInInterface;
typedef struct tagZW_RECORD_ADVANCED_CFG
{
	int bValid;																		/*!< 本结构体是否有效，0表示无效  */

	int bOverwrite;											//0,不覆盖，1，覆盖
	eESTATWorkMode dwESATAWorkMode;

	int dwAudioInInterface;					// 对讲的音频输入使用何种方式,参见eAudioInInterface
	int eAudioCodec;
	int nRecordDelete;						//0,不删除，1,24小时，表示循环写入的时候，只覆盖超过24小时的录像，其他来的录像丢弃。2,48小时，表示循环写入的时候，只覆盖超过48小时的录像，其他来的录像丢弃.
	int nRecordStreamType;						//0,表示主码流.1,表示子码流.

	//URL
	int nProtocol;							//0，表示http协议
	int nPort;								//URL的端口
	char strHost[64];
	char strPath[64];
	char strDescription[32];
	char strUserName[32];
	char strPassword[32];
	char cRecordMode;						//0,停止录像，1手动录像，2，自动录像
	char cReserved[3];
	char  acReserved[4];					// 保留(方便日后扩展)
}ZW_RECORD_ADVANCED_CFG;


typedef struct tagZW_HTTP_EVENT_CFG
{
	int nValid;
	int nEnableHttpEvent;		//http事件使能,0,表示不使能，1表示使能
	char acReserved[1024];
}ZW_HTTP_EVENT_CFG;

/*!
*  @struct	tagHY_DVR_DEVICE_CFG
*  @brief	设备基本配置结构
 */
typedef struct tagHY_DVR_DEVICE_CFG
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */

	// 有改动，添加进了月模式 2.1.1.5
	HY_DVR_DST struDST;														/*!< 夏时制配置 */
	ZW_SYSCOMMON_DATETIME_CFG strDateTime;//2.1.1.3 新添加日期配置
	ZW_SYSCOMMON_NORMAL_CFG struNormal;	// 2.1.1.1 新添加系统普通配置
	ZW_RECORD_ADVANCED_CFG struRecord;  // 2.1.1.30 录像控制
} HY_DVR_DEVICE_CFG, *LPHY_DVR_DEVICE_CFG;

/** @brief 设备基本配置结构体长度														 */
#define	DEVICE_CFG_LEN	sizeof(HY_DVR_DEVICE_CFG)
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
//#define RESERVED_STORAGE_OLD
/*!
*  @struct	tagHY_DVR_DISK_PARTITION
*  @brief	分区信息结构（只读）
 */
typedef struct tagHY_DVR_DISK_PARTITION
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */

	char szDevNode[DVR_MAX_PARTITION_DEVNODE_LEN];							/*!< 设备节点名, 如 /dev/sda1 */
	DWORD dwCapacity;												/*!< 总容量, M为单位 */
	DWORD dwSpare;													/*!< 剩余容量 */
	DWORD dwStartCHS;												/*!< 分区起始扇区 */
	DWORD dwEndtCHS;												/*!< 分区结束扇区 */

} HY_DVR_DISK_PARTITION, *LPHY_DVR_DISK_PARTITION;

/*!
*  @struct	tagHY_DVR_DISK_MARK
*  @brief	硬盘标记（只读）
 */
typedef struct tagHY_DVR_DISK_MARK
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */

	int nVerifyMark;														/*!< 验证码(配置改变时，验证码会同步改变) */

} HY_DVR_DISK_MARK, *LPHY_DVR_DISK_MARK;

/** @brief 硬盘标记结构体长度														 */
#define	DISK_MARK_LEN	sizeof(HY_DVR_DISK_MARK)



/*!
*  @struct	tagHY_DVR_DISK
*  @brief	硬盘信息结构（只读）
 */
typedef struct tagHY_DVR_DISK
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */

	int bEnable;															/*!< 是否安装有此硬盘:0表示没有，1表示有 */

	HY_DVR_DISK_MARK struVerifyMark;										/*!< 验证码(配置改变时，验证码会同步改变) */

	int nRestState;															/*!< 硬盘状态:0-正常，1-正在写入，2-异常，3-警告 */
	int nWriteState;														/*!< 硬盘状态:0-没有写，非0-正在写入 */

	char szSerialNum[DVR_MAX_VERSION_LEN];									/*!< 序列号 */
	char szTypeName[DVR_MAX_VERSION_LEN];									/*!< 硬盘类型 */
	int nBusNum;															/*!< 物理位置,如SATA1 */

	DWORD dwCapacity;												/*!< 容量,M为单位 */
	int nPartitionCount;													/*!< 分区数量 */

	HY_DVR_DISK_PARTITION struPartitionInfo[DVR_MAX_PARTITION_IN_HARDDISK];	/*!< 分区信息 */

} HY_DVR_DISK, *LPHY_DVR_DISK;

/*!
*  @struct	tagHY_DVR_DISK_GROUP
*  @brief	盘组信息结构（只读）
 */
typedef struct tagHY_DVR_DISK_GROUP
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */

	int bEnable;															/*!< 是否启用本盘组 */
	int nGroupType;															/*!< 盘组类型:0-普通盘组，1-报警盘组，2-冗余盘组，3-备份盘组 */
	int nBindChannelMask;													/*!< 绑定的通道:按位标记，若第0位为1，表示通道1被选中，以此类推 */
	int nDataKeepTime;														/*!< 数据保持时间:0x80000000-一直保持，24 * 3600-保持1天，2 * 24 * 3600-保持2天，7 * 24 * 3600-保持7天，其他值-最大保持 */
	int nPartitionCount;													/*!< 已添加的分区数量 */
	char szPartitionList[DVR_MAX_PARTITION_IN_DISKGROUP][DVR_MAX_PARTITION_DEVNODE_LEN];/*!< 设备节点列表,支持32个分区 */

} HY_DVR_DISK_GROUP, *LPHY_DVR_DISK_GROUP;

// 磁盘管理——基本配置
typedef enum
{
	eZWDiskStatus_Idle,
	eZWDiskStatus_Writting,
	eZWDiskStatus_Reading,
	eZWDiskStatus_Exception,
	eZWDiskStatus_Error,
	eZWDiskStatus_Readonly,
	eZWDiskStatus_Formatting,
	eZWDiskStatus_Warn,
	eZWDiskStatus_ReadWrite,
	eZWDiskStatus_NoDisk, // 无磁盘
	eZWDiskStatus_ExpNeedFormat, // 磁盘异常需要格式化
	eZWDiskStatus_ExpNeedChangeDisk, // 磁盘异常需要换卡
	eZWDiskStatus_FormatFailNeedReboot // 磁盘格式化失败需要重启
}eZWDiskStatus;

// 磁盘管理——格式化状态
typedef enum
{
	eZWDiskFormatStatus_UnFormatted = 0,  // 未格式化
	eZWDiskFormatStatus_Formatting = 1,  // 格式化中
	eZWDiskFormatStatus_FormatOk = 2,  // 格式化成功
	eZWDiskFormatStatus_FormatErr = 3,  // 格式化失败
	eZWDiskFormatStatus_UmountFail = 4,  // 卸载失败
	eZWDiskFormatStatus_DeviceIsBusy = 5,  // 设备忙
}eZWDiskFormatStatus;

typedef enum
{
	eZWDiskType_Local,
	eZWDiskType_ESATA,
	eZWDiskType_Samba,
}eZWDiskType;

typedef struct tagZW_DISK_COMMON_CFG
{
	int bValid;																		/*!< 本结构体是否有效，0表示无效  */

	DWORD dwBusID;
	char  szProductType[DVR_MAX_CHANNELNAME_LEN];
	eZWDiskStatus dwStatus;								// 正常为0，正在写入为1，正在读为2，异常为3，错误为4，只读为5，格式化为6， 警告为7 ，参见枚举变量。
	DWORD dwTotalCapacity;						// 单位：MB
	DWORD dwAvailableCapacity;					// 单位：MB
	eZWDiskType dwType;								// 0 本地/ 1 ESATA /2 samba
	int nVerifyMark;														/*!< 验证码(配置改变时，验证码会同步改变) */
	int nPictureCapacityPercent;				//图片占存储空间的百分比
	int nRecordCapacityPercent;					//录像栈存储空间的百分比
	int nFormatStatus;							//格式化状态0,未格式化，1，正在格式化，2，格式化成功，3，格式化失败
	char  acReserved[244];						// 保留(方便日后扩展)
}ZW_DISK_COMMON_CFG;

/*!
*  @struct	tagHY_DVR_STORAGE_CFG
*  @brief	存储管理结构
 */
typedef struct tagHY_DVR_STORAGE_CFG
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */

	HY_DVR_DISK struDisk[DVR_MAX_HARDDISK_NUM];								/*!< 硬盘信息（只读） */	//后续将删除
	ZW_DISK_COMMON_CFG struDiskCommon[DVR_MAX_HARDDISK_NUM];				//新增 替代struDisk
	HY_DVR_DISK_GROUP struDiskGroup[DVR_MAX_DISKGROUP_NUM];					/*!< 盘组信息（只读） */
} HY_DVR_STORAGE_CFG, *LPHY_DVR_STORAGE_CFG;

/** @brief 设备存储管理结构体长度														 */
#define	STORAGE_CFG_LEN	sizeof(HY_DVR_STORAGE_CFG)
//////////////////////////////////////////////////////////////////////////////


/*!
*  @struct	tagHY_DVR_CHANNEL_STATE
*  @brief	设备通道状态
 */
typedef struct tagHY_DVR_CHANNEL_STATE
{
	int bValid;																/*本结构体是否有效，表示无效(即系统没有该通道) */

	int nVideoBitrate;														/*!< 主码流码率（128kbit/s-5120kbit/s） */
	int nChildVideoBitrate;													/*!< CIF子码流码率（32 48 64 128 256 512kbit/s） */
	int nVGAVideoBitrate;													/*!< VGA子码流码率（32 48 64 128 256 512kbit/s） */
	int bUseAuxVideoStream;													/*!< 是否启动子码流：0-不启用，1-启用 */
	int bUseVGAVideoStream;													/*!< 是否启动子码流：0-不启用，1-启用 */

	int nRecordType;														/*!< 录像状态：0-无录像，1-报警录像类型，2-普通类型，3-手动录像类型，4-移动录像 */
	int bViLost;															/*!< 是否有无视频信号报警(0表示无信号，1表示有信号) */
	int bMotion;															/*!< 是否有移动报警(尚不支持) */

} HY_DVR_CHANNEL_STATE, *LPHY_DVR_CHANNEL_STATE;

/*!
*  @struct	tagHY_DVR_DEVICE_STATE
*  @brief	设备状态信息
 */
typedef struct tagHY_DVR_DEVICE_STATE
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */

	HY_DVR_CHANNEL_STATE struChannelState[DVR_MAX_VIDEOIN_NUM];				/*!< 通道状态 */
} HY_DVR_DEVICE_STATE, *LPHY_DVR_DEVICE_STATE;

/** @brief 设备状态结构体长度														 */
#define	DEVICE_STATE_LEN	sizeof(HY_DVR_DEVICE_STATE)
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

/*!
*  @struct	tagHY_DVR_MAINTENANCE_COMMON
*  @brief	设备自动维护基本配置
 */
typedef struct tagHY_DVR_MAINTENANCE_COMMON
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */

	int bEnable;															/*!< 是否启用 */
	int nMode;																/*!< 维护模式:0-每天,1-每周,2-单次 */
	int nWeekDayMask;														/*!< 星期模式时，第0位表示星期天,第1位表示星期1,对应位为1表示当天有效 */
	HY_DVR_TIME struTime;													/*!< 单次模式，则年月日时分有效；其它模式，时分有效 */

} HY_DVR_MAINTENANCE_COMMON, *LPHY_DVR_MAINTENANCE_COMMON;

/*!
*  @struct	tagHY_DVR_DEVICE_MAINTENANCE
*  @brief	设备自动维护
 */
typedef struct tagHY_DVR_DEVICE_MAINTENANCE
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */

	HY_DVR_MAINTENANCE_COMMON struCommon;									/*!< 设备自动维护基本配置 */

} HY_DVR_DEVICE_MAINTENANCE, *LPHY_DVR_DEVICE_MAINTENANCE;

/** @brief 设备自动维护结构体长度														 */
#define	DEVICE_MAINTENANCE_LEN	sizeof(HY_DVR_DEVICE_MAINTENANCE)
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

/*!
*  @struct	tagHY_DVR_THRESHOLD
*  @brief	告警阀值（暂不实现）
*/
typedef struct tagHY_DVR_THRESHOLD
{
	int bValid;														/*!< 本结构体是否有效，0表示无效 */

	float fCPUUseRatio;												/*!< CPU利用率 */
	float fMemoryUseRatio;											/*!< 内存利用率 */
	float fDiskSpaceRatio;											/*!< 硬盘利用率 */
	DWORD dwDiskSpaceBalance;										/*!< 硬盘剩余空间 */

}HY_DVR_THRESHOLD, *LPHY_DVR_THRESHOLD;

/*!
*  @struct	tagHY_DVR_DOWNLOAD_MODE
*  @brief	下载方式配置（可以依据NAT的配置来区分，暂不实现）
*/
typedef struct tagHY_DVR_DOWNLOAD_MODE
{
	int bValid;														/*!< 本结构体是否有效，0表示无效 */
	DWORD dwDownloadMode;											/*!< 下载方式， 0-hxht, 1-STREAM_TCP */

}HY_DVR_DOWNLOAD_MODE, *LPHY_DVR_DOWNLOADMODE;

//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////

/*!
*  @struct	tagHY_DVR_NATSTATUS
*  @brief	NAT配置
*/
typedef struct tagHY_DVR_NATSTATUS
{
	int bValid;														/*!< 本结构体是否有效，0表示无效 */

	DWORD dwIsNAT;													/*!< 是否在NAT后面，0表示不在后面，1表示在NAT后面 */

}HY_DVR_NATSTATUS, *LPHY_DVR_NATSTATUS;


//////////////////////////////////////////////////////////////////////////////
/*
压缩格式 语音通道数 采样深度   采样速率
G711U.HX      1        16        8000
G711A.HX      1        16        8000
G722.HX       1        16    8000或16000
*/

/*!
*  @struct	tagHY_DVR_VOICEPARAM
*  @brief	语音参数配置
*/
typedef struct tagHY_DVR_VOICEPARAM
{
	int bValid;														/*!< 本结构体是否有效，0表示无效 */

	char szAudioEncoder[DVR_COMMON_STRING_LEN];						/*!< 支持的语音编码格式，可以有多个，以逗号隔开；G711U.HX G711A.HX G722.HX */
	DWORD dwChannel;												/*!< 通道数 */
	DWORD dwBitsPerSample;											/*!< 采样深度 */
	DWORD dwSamplesPerSec;											/*!< 采样率 */

}HY_DVR_VOICEPARAM, *LPHY_DVR_VOICEPARAM;


//////////////////////////////////////////////////////////////////////////////

/*!
*  @struct	tagHY_DVR_NET_CMS_STOREAGE
*  @brief	中心管理服务器存储注册配置信息
*/
typedef struct tagHY_DVR_NET_CMS_STOREAGE
{
	int bValid;														/*!< 本结构体是否有效，0表示无效 */

	char szManageHostAddress[DVR_MAX_DOMAINNAME_LEN];				/*!< 远程管理主机地址，长度考虑域名 */

	unsigned short wManageHostPort;									/*!< 远程管理主机端口 */

} HY_DVR_NET_CMS_STOREAGE, *LPHY_DVR_NET_CMS_STOREAGE;

//////////////////////////////////////////////////////////////////////////////

/*!
*  @struct	tagHY_DVR_DEVICE_ID
*  @brief	前端设备ID（该ID是由Hxht接入服务器分配的）
*/
typedef struct tagHY_DVR_DEVICE_ID
{
	int bValid;														/*!< 本结构体是否有效，0表示无效 */

	char szDeviceID[DVR_COMMON_STRING_LEN];							/*!< 前端设备ID */

} HY_DVR_DEVICE_ID, *LPHY_DVR_DEVICE_ID;

//////////////////////////////////////////////////////////////////////////////

typedef struct
{
	int bValid;														/*!< 本结构体是否有效，0表示无效 */

	HY_DVR_NET_CMS_STOREAGE struCmsStorage;							/*!< 中心管理服务器存储注册信息 */
	HY_DVR_DEVICE_ID		struDeviceID;							/*!< 前端设备ID */
	HY_DVR_NATSTATUS		struNATStatus;							/*!< NAT状态 */
	HY_DVR_VOICEPARAM		struVoiceParam;							/*!< 语音呼叫参数 */

}HY_DVR_HXHT_CFG, *LPHY_DVR_HXHT_CFG;

typedef struct
{
	int bValid;					//本结构体是否有效，=0表示无效

	unsigned int dwFileMap; //0~30位表示1号~31号的文件分布情况,0表示无文件,1表示有文件
} HY_DVR_FILEMAP_CFG, *LPHY_DVR_FILEMAP_CFG;

//////////////////////////////////////////////////////////////////////////////

/*!
*  @struct	tagHY_DVR_PROTOCOL_NAME
*  @brief	协议名称配置
*/
typedef struct tagHY_DVR_PROTOCOL_NAME
{
	int bValid;														/*!< 本结构体是否有效，0表示无效 */

	DWORD dwProtocolName;											/*!< 协议名称：协议是按位设置和读取的，例如只有手机协议时，该值为1；
																	只有互信互通协议时，该值为2；两个协议都有时，该值为3。
																	（私有协议是必须加载的，不在其中） */
}HY_DVR_PROTOCOL_NAME, *LPHY_DVR_PROTOCOL_NAME;

//////////////////////////////////////////////////////////////////////////////
/** @brief 地址长度(含结束符)															*/
#define HY_DVR_ADDRESS_LEN				(256)

/** @brief 用户名长度																	*/
#define HY_DVR_USERNAME_LEN				(32)

/** @brief 用户密码长度																	*/
#define HY_DVR_USERPASS_LEN				(32)

/** @brief 默认保留字段个数																*/
#define HY_DVR_DEF_RESERVE_NUM			(3)

/*!
*  @struct	tagHY_DVR_USERLOGIN_INFO
*  @brief	用户登录参数
*/
typedef struct tagHY_DVR_USERLOGIN_INFO
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */

	char	sServerIP[HY_DVR_ADDRESS_LEN];						/*!< 服务器端IP地址							*/
	DWORD	dwCommandPort;								/*!< 登录连接的信令端口						*/
	char	sUName[HY_DVR_USERNAME_LEN];						/*!< 登录用户的用户名称						*/
	char	sUPass[HY_DVR_USERPASS_LEN];						/*!< 登录用户的用户密码						*/
	DWORD	dwReserve[HY_DVR_DEF_RESERVE_NUM];			/*!< 保留									*/
}HY_DVR_USERLOGIN_INFO, *LPHY_DVR_USERLOGIN_INFO;

/*!
*  @struct	tagHY_DVR_SOURCE_DEVICE_INFO
*  @brief	源设备信息
*/
typedef struct tagHY_DVR_SOURCE_DEVICE_INFO
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */

	int						nEnable;							/*!< 0表示不启用，1表示启用			*/
	HY_DVR_USERLOGIN_INFO	cUserPara;							/*!< 用户参数						*/
	DWORD			dwVideoInNumber;					/*!< 视频输入数						*/
	DWORD			dwReserve[HY_DVR_DEF_RESERVE_NUM];	/*!< 保留							*/
}HY_DVR_SOURCE_DEVICE_INFO, *LPHY_DVR_SOURCE_DEVICE_INFO;

/*!
*  @struct	tagHY_DVR_SOURCE_DEVICE_INFO_CONFIG
*  @brief	源设备配置
*/
typedef struct tagHY_DVR_SOURCE_DEVICE_INFO_CONFIG
{
	int bValid;																/*!< 本结构体是否有效，0表示无效 */

	HY_DVR_SOURCE_DEVICE_INFO	cSourceDevice[MAX_SOURCE_DEVICE_NUMBER];	/*!< 源设备信息					*/
	DWORD				dwReserve[HY_DVR_DEF_RESERVE_NUM];			/*!< 保留				*/
}HY_DVR_SOURCE_DEVICE_INFO_CONFIG, *LPHY_DVR_SOURCE_DEVICE_INFO_CONFIG;


//////////////////////////////////////////////////////////////////////////////
//显示设备属性
//获取显示设备信息

/*!
*  @struct	tagHY_DVR_VGAPARA
*  @brief	VGA参数
*/
typedef struct tagHY_DVR_VGAPARA
{
	int bValid;									/*!< 本结构体是否有效，0表示无效 */

	int nResolution;							/*!< 分辨率 ,比特位表示 */
	int nResolutionCap;							/*!< 分屏能力，只读。与分辨率有关,按位表示，例：第九位为1表示
												支持9分屏 */
	DWORD	dwDeviceIndex;				/*!< 设备编号 */

}HY_DVR_VGAPARA, *LPHY_DVR_VGAPARA;

/*!
*  @struct	tagHY_DVR_VGAPARA_CFG
*  @brief	VGA参数配置
*/
typedef struct tagHY_DVR_VGAPARA_CFG
{
	int bValid;									/*!< 0表示无效 1表示有效 */

	int nVGACount;								/*!< VGA Out实际个数 */
	HY_DVR_VGAPARA struVga[MAX_VGA_NUM];

}HY_DVR_VGAPARA_CFG, *LPHY_DVR_VGAPARA_CFG;

/*!
*  @struct	tagHY_DVR_VOOUT
*  @brief	视频输出
*/
typedef struct tagHY_DVR_VOOUT
{
	int bValid; 								/*!< 0表示无效 1表示有效 */

	int byVideoFormat;							/*!< 输出制式,0-PAL,1-NTSC */
	int wLeftffset;								/*!< 视频输出左偏移 */
	int wRightffset;							/*!< 视频输出右偏移 */
	int wTopffset;								/*!< 视频输出上偏移 */
	int wBottomffset;							/*!< 视频输出下偏移 */
	int dwResolutionCap;						/*!< 分屏能力，只读。*/
	DWORD	dwDeviceIndex;						/*!< 设备编号 */

}HY_DVR_VOOUT, *LPHY_DVR_VOOUT;

/*!
*  @struct	tagHY_DVR_VOOUT_CFG
*  @brief	视频输出配置
*/
typedef struct tagHY_DVR_VOOUT_CFG
{
	int bValid;									/*!< 0表示无效 1表示有效 */
	int nTVCount;								/*!< TV Out 个数*/
	HY_DVR_VOOUT struTv[MAX_TV_NUM];

}HY_DVR_VOOUT_CFG, *LPHY_DVR_VOOUT_CFG;

/*!
*  @struct	tagHY_DVR_DISPDEV_CFG
*  @brief	解码器显示设备信息
*/
typedef struct tagHY_DVR_DISPDEV_CFG
{
	int bValid; 								/*!< 0表示无效 1表示有效 */

	HY_DVR_VGAPARA_CFG struVGA;					/*!< VGA参数 */
	HY_DVR_VOOUT_CFG struTV;					/*!< TV参数 */

} HY_DVR_DISPDEV_CFG, *LPHY_DVR_DISPDEV_CFG;

//////////////////////////////////////////////////////////////////////////////
/*!
*  @enum	eDeviceCapability
*  @brief	设备能力定义
*/
typedef enum eDeviceCapability
{
	//能力字段0
	eCapabilityLocalNetwork = 0x00,							/*!< 本地网络								*/
	eCapabilityWirelessNetwork = 0x01,							/*!< 无线网络								*/
	eCapabilityDHCP = 0x02,							/*!< DHCP									*/
}eDeviceCapability;
//////////////////////////////////////////////////////////////////////////////
/*!
*  @struct	tagHY_DVR_PROBE_NET_INTERFACE
*  @brief	网络接口配置
*/
typedef struct tagHY_DVR_PROBE_NET_INTERFACE
{
	unsigned char	m_bytEnable;									/*!< 是否启用：0-不启用，1-启用，2-未找到	*/
	unsigned char	m_bytGetIPMode;									/*!< 获取地址：0-手动设定，1-DHCP获取		*/
	unsigned char	m_bytDevMac[DVR_MAX_MAC_NUM_LEN];				/*!< 设备MAC地址(Read)						*/
	unsigned char	m_bytDevIP[DVR_MAX_IP_STR_LEN];					/*!< 设备当前IP地址							*/
	unsigned char	m_bytDevMask[DVR_MAX_IP_STR_LEN];				/*!< 设备IP地址掩码							*/
	unsigned char	m_bytDevGateway[DVR_MAX_IP_STR_LEN];			/*!< 设备网关地址							*/
	DWORD			m_dwNetworkState;								/*!< 网络状态:
																			0-连接成功,
																			1-未启用,
																			2-正在连接网络,
																			3-连接网络失败,继续连接,
																			4-正在获取IP,
																			5-获取IP失败,继续获取				*/
	DWORD			m_dwReserve;									/*!< 保留数据								*/
}HY_DVR_PROBE_NET_INTERFACE, *LPHY_DVR_PROBE_NET_INTERFACE;
/*!
*  @struct	tagHY_DVR_PROBE_NET_CFG
*  @brief	探测网络配置
*/
typedef struct tagHY_DVR_PROBE_NET_CFG
{
	DWORD			m_bytDevType;									/*!< 服务器类型(Read)						*/
	unsigned char	m_bytDevChan;									/*!< 设备通道个数(Read)						*/
	unsigned char	m_bytDevAutoReg;								/*!< 是否启用自动注册						*/
	unsigned char	m_bytDevRegInterval;							/*!< 设备注册间隔时间						*/
	HY_DVR_PROBE_NET_INTERFACE	m_cInterface[3];					/*!< 网络接口配置							*/
	unsigned char	m_bytDevManHost[DVR_MAX_IP_STR_LEN];			/*!< 设备管理主机地址						*/
	unsigned short	m_bytDevDetectPort;								/*!< 设备自动发现监听端口					*/
	unsigned short	m_wManHostPort;									/*!< 设备管理主机端口						*/
	unsigned short	m_wCmdPort;										/*!< 设备命令端口							*/
	unsigned short	m_wMediaPort;									/*!< 设备媒体端口							*/
	DWORD	m_dwDeviceCapability[4];								/*!< 设备能力，位域详见eDeviceCapability定义*/
	DWORD	m_dwEnableDhcp;											/*!< 是否使能DHCP							*/
	unsigned char	m_cEnableAutoGenIP;								/*!< 是否使能自适应IP						*/
	unsigned short  m_usHttpPort;									/*!< http 端口								*/
	unsigned char	m_cReceiver[1];									/*!< 保留数据								*/
	DWORD	m_dwReserve[1];											/*!< 保留数据								*/
}HY_DVR_PROBE_NET_CFG, *LPHY_DVR_PROBE_NET_CFG;
//////////////////////////////////////////////////////////////////////////////
/*!
*  @struct	tagHY_DVR_WLAN_CONNECTION
*  @brief	WIFI连接配置
*/
typedef struct tagHY_DVR_PROBE_WLAN_CONNECTION
{
	int		m_bEnable;												/*!< WIFI状态 0 不启用 1 启用, 2 网卡未找到*/
	int		m_bConnect;												/*!< 是否连接上：0表示未连接上 1表示连接上(只读) */
	int		m_nKeyIndex;											/*!< 密码索引: 1 - 4						*/
	char	m_szSSID[DVR_MAX_SSID_LEN];								/*!< 热点名称								*/
	char	m_szKey[DVR_MAX_WIFI_KEY_LEN];							/*!< 密码									*/
	char	m_szMacAddress[DVR_MAX_MAC_STR_LEN];					/*!< 热点MAC地址							*/
	int		m_nNetworkType;											/*!< 网络类型: 0表示Managed, 1表示Ad-hoc	*/
	int		m_nAuthMode;											/*!< 认证模式: 0-OPEN,1-SHARED,2-WEPAUTO,3-WPAPSK,4-WPA2PSK,5-WPANONE,6-WPA,7-WPA2*/
	int		m_nEncrypType;											/*!< 加密算法 0-NONE,1-WEP,2-TKIP,3-AES		*/
	DWORD	m_dwReserve[3];									/*!< 保留数据								*/
} HY_DVR_PROBE_WLAN_CONNECTION, *LPHY_DVR_PROBE_WLAN_CONNECTION;
//////////////////////////////////////////////////////////////////////////////
/*!
*  @struct	tagHY_DVR_PROBE_WLAN_ACCESSPOINT
*  @brief	WIFI热点
*/
typedef struct tagHY_DVR_PROBE_WLAN_ACCESSPOINT
{
	char szSSID[DVR_MAX_SSID_LEN];									/*!< 热点名称 */
	char szMacAddress[DVR_MAX_MAC_STR_LEN];							/*!< 热点MAC地址 */
	int nSignal;													/*!< 热点信号强度 */
	int nAuthentication;											/*!< 认证模式 0-Open,1-Shared,2-WPAPSK, 3-WPA2PSK, 4-WPANONE(ad-hoc模式),5-WPA,6-WPA2 */
	int nEncrypType;												/*!< 加密模式  0表示NONE 1表示WEP 2表示TKIP 3表示AES */
	int nWirelessMode;												/*!< 1: legacy 11B only 2: legacy 11A only 3: legacy 11a/b/g mixed
																		 4: legacy 11G only 5: 11ABGN mixed    6: 11N only
																		 7: 11GN mixed      8: 11AN mixed      9: 11BGN mixed */
	DWORD	m_dwReserve[3];									/*!< 保留数据								*/
} HY_DVR_PROBE_WLAN_ACCESSPOINT, *LPHY_DVR_PROBE_WLAN_ACCESSPOINT;

/*!
*  @struct	tagHY_DVR_PROBE_WLAN_ACCESSPOINT_CFG
*  @brief	WIFI热点配置
*/
typedef struct tagHY_DVR_PROBE_WLAN_ACCESSPOINT_CFG
{
	int		m_nNumber;												/*!< 有效热点个数							*/
	HY_DVR_PROBE_WLAN_ACCESSPOINT m_struAccessPoints[DVR_MAX_AP_NUM];/*!< WIFI热点								*/
	DWORD	m_dwReserve[3];									/*!< 保留数据								*/
} HY_DVR_PROBE_WLAN_ACCESSPOINT_CFG, *LPHY_DVR_PROBE_WLAN_ACCESSPOINT_CFG;
//////////////////////////////////////////////////////////////////////////////

/*!
*  @struct	tagZW_DVR_INSTALLTION_INFO
*  @brief	安装信息配置参数
 */
typedef struct tagZW_DVR_INSTALLTION_INFO
{
	int bValid;			/*!< 本结构体是否有效，0表示无效 */

	char strCompanyName[DVR_MAX_COMPANYNAME_LEN]; /*!< 公司名称 */
	char cReserve1[3];						/*!< 保留字段*/
	char strTelNum[DVR_MAX_TEL_NUM_LEN];	/*!< 公司电话号码 */
	char cReserve2[3];						/*!< 保留字段*/
	HY_DVR_TIME struInstallTime;			/*!< 安装日期*/
	HY_DVR_TIME struMaintainTime;			/*!< 最后一次维护日期 */
	char cInstallAddress[128];				/*!< 安装地址 */
	char	dwReserve[128];					/*!<保留字段*/
}ZW_DVR_INSTALLTION_INFO;

/*!
*  @struct	tagZW_MULTI_CHANNEL_INSTALLTION_INFO
*  @brief	安装信息配置参数
 */
typedef struct tagZW_MULTI_CHANNEL_INSTALLTION_INFO
{
	int bValid;			/*!< 本结构体是否有效，0表示无效 */

	ZW_DVR_INSTALLTION_INFO stInstallInfo[DVR_MAX_VIDEOENC_NUM];
}ZW_MULTI_CHANNEL_INSTALLTION_INFO;

//探测相关
#define DVR_MAX_DEVICE_NAME_LENGTH		(32)

/*!
*  @struct	tagHY_DVR_RECODE_TIME_INFO
*  @brief	DVR录像配置参数（录像开始的最早时间，最晚时间）
 */
typedef struct tagHY_DVR_RECODE_TIME_INFO
{
	int bVaild;						/*!< 本结构体是否有效，0表示无效 */

	HY_DVR_TIME StartTime;			//最早时间
	HY_DVR_TIME EndTime;			//最晚时间
	char dwReserver[256];
}HY_DVR_RECODE_TIME_INFO;


/*!
*  @struct	tagHY_DVR_BUFFER_SIZE
*  @brief	缓冲区大小
 */
typedef struct tagHY_DVR_BUFFER_SIZE
{
	int bVaild;						/*!< 本结构体是否有效，0表示无效 */

	int nCommandBufferLen;			/*!< 命令接收缓冲区大小，私有协议，默认是8KB，建议128KB，单位是Byte */
	int nMediaBufferLen;			/*!< 媒体缓冲区大小，私有协议，单位是Byte */
	int nRealStreamBufferLen;		/*!< 实时流(主)缓冲区大小，私有协议议，建议512KB，以主码流最大I帧的大小来设置，单位是Byte */
	int nSubRealStreamBufferLen;	/*!< 实时流(子)缓冲区大小，私有协议，建议256KB，以子码流最大I帧的大小来设置，单位是Byte*/
	int nSubMediaBufferLen;         /*!< 媒体(子)缓冲区大小，私有协议，单位是Byte */
	int nPictureBufferLen;			/*!< 缩略图缓冲区大小，CGI协议，建议不要超过64KB，单位是Byte*/
	int nEventNoticeBufferLen;		/*!< 事件推送缓冲区大小，私有协议，建议不要超过256KB，单位是Byte */
	int nCloudStorageSendMediaBufferLen;	/*!< 云存储发送媒体缓冲区大小，该大小设置确保要大于I帧，默认是256，单位是Byte*/
	int nCloudStorageAnnulusBufferLen;	/*!< 云存储环形缓冲区大小，默认是768，建议环形缓冲区的大小至少是nCloudStorageSendMediaBufferLen的1.5倍，单位是Byte*/
	int nCloudStorageSendDataRetryCount;  /*!< 云存储媒体发送数据重试次数，默认为2次*/
	int nCloudStorageSendMediaTimeout;  /*!< 云存储媒体发送超时时间，默认10秒，建议发送超时按每128KB/秒 + 1秒（抖动时间）进行设置，同时减少重试次数*/
	int nCloudStorageSendIndexTimeout;  /*!< 云存储索引发送超时时间 ，默认10秒，因为索引文件相对大小比较固定，所以使用默认值即可*/
	int nCloudStorageOtherMyServerTimeout;  /*!< 云存储服务与我们自己的服务器交互超时时间 ，默认10秒*/
	int nMqttRecvMessageBufferLen;			/*!< mqtt协议支持的最大接收缓冲区大小 ，默认8MB，单位是Byte*/
	int nMqttMessageQueueCount;				/*!< mqtt协议支持的最大的消息队列的个数 ，默认100个*/
	int nMqttMessageQueueMemory;			/*!< mqtt协议支持的最大的消息队列缓冲区大小 ，默认8MB*/
	int nWanHuEventQueueSize;				/*!< 万户主动注册事件发送缓冲区队列，私有协议，默认10, 可以不设置，单位是Byte */
	int nCGIBufferSize;								/*!< CGI请求缓冲区的大小，可以不设置，设置为0，使用默认值，单位是Byte */
	int nOVDRealStreamBufferSize;			/*!< OVD实时流环形缓冲区的大小，可以不设置，设置为0，使用默认值，单位是Byte  */
	int nCloudStorageGopMaxBufferSize;		/*!< 国际云存储协议一个I帧组最大的大小，建议768KB，根据传输给国际云存储一个I帧组最大大小来决定，这个需要计算，单位是Byte */
	int nEventMaxNum;						/*!< 事件缓冲区的大小,默认为1 */
	int nCommandSendBufferLen;				/*!< 命令发送缓冲区大小，私有协议，默认是8KB,建议128KB，单位是Byte */
	int nCloudStorageSubSendMediaBufferLen; /*!< 多通道时子码流云存储发送媒体缓冲区大小 ，默认是128，单位是Byte*/
    int nCloudStorageSubAnnulusBufferLen;  /*!< 多通道时子码流云存储环形缓冲区大小，默认是256，建议环形缓冲区的大小至少是发送缓冲区大小的1.5倍，单位是Byte*/
	int nP2PHttpSendBufferLen;             	/*P2P http 请求最大发送buffer长度 默认值384k*/
	int nP2PHttpRecvBufferLen;				/*P2P http 请求最大接收buffer长度 默认值384k*/
    int nCarPlateWhiteListBufferLen;		/*车牌白名单最大接收buffer长度 默认值256k*/
	int nCloudStorageConnectTimeout;  		/*!< 云存储服务与服务器连接超时时间 ，默认3秒*/
	int nP2PMaxSenderBufferLens;  			/*!< P2P底层尚云发送buffer最大值,默认是128k*/
	int nActiveInfoCount;                    /*!< 激活信息列表的个数，对应的是结构体HY_DVR_ACTIVE_CFG的pActiveInfoList申请的个数，如果该值为小于等于0的数字，就用默认值，默认为8*/
	int nRtmpRegisterEventMaxNum;			/*!< 巴西主动注册协议事件缓冲区的大小,默认为3;当设置为0时，底层的缓冲区大小为3 */
    int nMaxVideoCallFrameBufferLen;        /*!< 视频通话帧最大长度,默认为512K;当设置为0时，长度最大为512K */

	char dwReserver[180 - 8 - 8 - 4 - 4 - 4 - 4 - 4 - 4];
}HY_DVR_BUFFER_SIZE;


/*!
*  @struct	tagHY_DVR_CONNECT_COUNT
*  @brief	缓冲区大小
 */
typedef struct tagHY_DVR_CONNECT_COUNT
{
	int bVaild;						/*!< 本结构体是否有效，0表示无效 */
	int nClientCount;
	int nMainStreamCount;
	int nSubStreamCount;
	char dwReserver[256];
}HY_DVR_CONNECT_COUNT;

//以下结构体中，无论是设备信息结构体数组，还是设备状态结构体数组，其中的每一个元素都依次和设备前端的通道相对应

/*!
*  @struct	tagHY_DVR_FRONT_DEVICE_INFO
*  @brief	NVR前端设备列表参数
 */

 //NVR前端设备连接列表信息
typedef struct tagHY_DVR_FRONT_DEVICE_INFO
{
	int nValid;										 //当nVailed为1时表示该通道有设备，当nvailed为0的时候表示该通道没有设备
	char strDeviceName[DVR_MAX_DEVICE_NAME_LENGTH];  //用来存储设备的名字（主要是设备的生产厂商）
	char strUserName[USERNAME_LEN];					//设备的登录用户名
	char strPassword[USERPASS_LEN];					//设备的登录密码
	char strDevIp[DVR_MAX_IP_STR_LEN];			//设备的IP
	char strDevMac[DVR_MAX_MAC_STR_LEN];		//设备mac地址
	char cReserve1[2];							//保留字段，保证四字节对齐
	DWORD dwDevType;							//设备的类型（需要NVR端提供，数字对应的字符串）
	DWORD dwDevProtocolType;					//设备的协议类型（需要NVR端提供，数字对应的字符串）
	DWORD dwFrontDeviceChannel;					//NVR端连接的多通道的设备的通道
	DWORD dwDevPort;							//设备访问的端口号（私有的是指命令端口号，不同的协议端口好不一样）

	DWORD dwActionState;						//该值主要用来添加，删除，设备的时候用于区分这些操作，客户端在获取该配置的时候，
												//该值可以为0，对于服务端，当收到该值为0，的时候表示添加设备或者修改网络配置重新登录，
												//当为1的时候，表示删除，该设备
												//0,表示不做任何处理，1，删除，2，表示添加和修改，3表示手动添加
	char strDevUID[64];							//获取设备的UID
	char strDevType[32];						//设备版型
	char strSoftVersion[32];					//软件版本
	int nTutkEnable;								//是否启用
	DWORD dwTutkStatus;								 // 0x0，未联机，0x01，内网就绪，0x02, 外网就绪
	char szSubNetMask[16];			// 子网掩码
	char szGateway[16];					//网关	
	char cPositionCapability;			//定位功能支持情况，0，不支持，1，支持
	char cDownloadSpeed;				//下载倍速范围，0,1/2/4,1,1/2/4/8,3,1/2/4/8/16
	char cEncode;						//是否编码，0，没有编码，1，有编码			
	char cReason;						//不正常工作的原因，0,正常，1，网络问题，2，密码错误	
	char cReserve[84];
}HY_DVR_FRONT_DEVICE_INFO;


typedef struct tagHY_DVR_FRONT_DEVICE_LIST
{
	int nValid;
	HY_DVR_FRONT_DEVICE_INFO struFrontDeviceInfo[MAX_DEVICE_COUNT];
	HY_DVR_FRONT_DEVICE_INFO struReserve[MAX_DEVICE_COUNT * 3];     //保留位
	char dwReserve[1024];											 //保留位
}HY_DVR_FRONT_DEVICE_LIST;


//NVR前端设备连接列表的状态信息
typedef struct tagHY_DVR_FRONT_DEVICE_STATE
{
	int nValid;			//当nVailed为1时表示该通道有设备，当nvailed为0的时候表示该通道没有设备
	int nLoginState;  //登录状态，用来反映设备登录的情况，是一个只读的信息，用于在客户端获取NVR前端设备连接的状态，
	char cReserve[64];
}HY_DVR_FRONT_DEVICE_STATE;

typedef struct tagHY_DVR_FRONT_DEVICE_STATE_LIST
{
	int nValid;
	HY_DVR_FRONT_DEVICE_STATE struDevState[MAX_DEVICE_COUNT];  //标记设备的状态
	HY_DVR_FRONT_DEVICE_STATE struReserve[MAX_DEVICE_COUNT * 3];     //保留位
	char dwReserve[1024];											 //保留位
}HY_DVR_FRONT_DEVICE_STATE_LIST;


//修改NVR前端设备的网络信息
typedef struct tagHY_DVR_FRONT_DEVICE_NETWORK
{
	int nValid;

	char strUserName[USERNAME_LEN];		//设备的登录用户名
	char strPassword[USERPASS_LEN];		//设备的登录密码
	char strDevIp[DVR_MAX_IP_STR_LEN];			//设备的IP
	char strDevSubmask[DVR_MAX_IP_STR_LEN];		//设备的子网掩码
	char strDevGateway[DVR_MAX_IP_STR_LEN];		//设备的网关
	char strDevMac[DVR_MAX_MAC_STR_LEN];		//设备mac地址
	char cReserve1[2];							//保留字段，保证四字节对齐
	char cDevUUid[64];							//设备唯一标识符
	DWORD dwDevPort;							//设备访问的端口号（私有的是指命令端口号，不同的协议端口好不一样）

	char dwReserve[1024];
}HY_DVR_FRONT_DEVICE_NETWORK;


// 轮巡列表配置(新)

//!\brief 支持的最大的轮巡节点数
#define DEV_MAX_POLL_NODE_NUM		(64)
#define DEV_MAX_SUPPORT_CHANNEL		(64)
//所有支持的分屏模式，在底层所有分辨率均可
//上层可以根据实际效果，选择不同的分辨率支持不同的分屏模式

typedef enum
{
	SINGLE = 1,
	THREE = 3,
	FOUR = 4,
	SIX = 6,
	EIGHT = 8,
	NINE = 9,
	TEN = 10,
	TWELVE = 12,
	THIRTEEN = 13,
	FIFTEEN = 15,
	SIXTEEN = 16,
	TWENTYFIVE = 25,
	THIRTYSIX = 36,
	SIXTYFOURTH = 64
}enumScreenType;

//!\brief 在自定义序列上，该节点的分屏模式及涉及的通道。
//! 例如：三分屏 则channel数组的前三个填响应的通道号。后面的填写0xff
typedef struct
{
	int bValid;

	//!\brief 是否启用了该节点
	int bEnable;

	//!\brief 分屏模式
	int enScreenType;							// enumScreenType
	int Page;									// 用于扩展,如果某一屏支持的通道数大于64时使用,默认填零即可
	int dwDuration;								// 停留时间
	int dwMonitorIndex;							// 哪个屏幕上显示出来(弃用)．
	//!\brief 记录的通道
	unsigned int channel[DEV_MAX_SUPPORT_CHANNEL];
} HY_DVR_PREVIEW_NODE, *LP_HY_DVR_PREVIEW_NODE;

// 对应的命令是HY_DVR_GET_PREVIEW_LIST和HY_DVR_SET_PREVIEW_LIST
typedef struct
{
	int bValid;

	unsigned short BeginNodeIndex;				// 便于以后扩充更多的节点的配置
	unsigned short MonitorIndex;				// 这里描述一下这个轮巡列表在哪个屏幕上
	//!\brief 节点数组
	HY_DVR_PREVIEW_NODE Node[DEV_MAX_POLL_NODE_NUM];
} HY_DVR_PREVIEW_LIST, *LP_HY_DVR_PREVIEW_LIST;



// 启动停止轮转的控制使用远程控制命令
// 具体请参考远程控制命令的定义eRemotePreviewCruiseStart和eRemotePreviewCruiseStop

// 放大某个通道的显示(对应命令HY_DVR_FULLSCREEN_WINDOW)
typedef struct
{
	int bValid;

	int nChannel;								// 要放大的是个哪个通道
	int dwMonitorIndex;							// 哪个屏幕上显示出来．
	char cResvered[255];
}HY_DVR_FULLSCREEN_CHANNEL;



#define DVR_MAX_RESOLUTION_STRING_LEN				(16)			// 每个分辨率的描述字符串的最大长充
#define DVR_MAX_SUPPORT_RESOLUTION_NUM				(32)			// 屏幕最多支持多少分辨率

// 描述一个分辨率
typedef struct
{
	int bValid;
	char acResolutionDesc[DVR_MAX_RESOLUTION_STRING_LEN];			// 一个分辨率的描述,比如"1024x768", 最大16个字符应该够
}HY_DVR_RESOLUTION, *LPHY_DVR_RESOLUTION;

// 描述一个屏幕所支持分辨率能力
typedef struct
{
	int bValid;
	char acType[16];												// "HDMI", "VGA"
	char acDescription[32];											// 用于描述屏幕的一些信息，比如品牌型号之类的
	HY_DVR_RESOLUTION acResolution[DVR_MAX_SUPPORT_RESOLUTION_NUM];		// 屏幕最多支持32个分辨率，这应该也够了吧
}HY_DVR_MONITOR_RESOLUTION_CAP, *LPHY_DVR_MONITOR_RESOLUTION_CAP;

// 描述设备内所支持的所有屏幕的能力
typedef struct
{
	int bValid;
	HY_DVR_MONITOR_RESOLUTION_CAP acMonitorCap[DVR_MAX_VGARESOLVING_NUM];		// 最多支持16个屏幕(全部只读)
}HY_DVR_RESOLUTION_CAP;

// 设置和获取当前屏幕分辨率
typedef struct
{
	int bValid;
	HY_DVR_RESOLUTION acResolutionList[DVR_MAX_VGARESOLVING_NUM];
}HY_DVR_RESOLUTION_LIST;

//主码流和子码流的分辨率和帧率的关系，
#define MAX_CAP_ITEM (32)
//编码能力项，每项有4个成员
typedef struct tagHY_DVR_RESOLUTION_AND_FRAME_CAP
{
	int nValid;					/*!< 本结构体是否有效，0表示无效 */
	int enMainStreamResolution; /*!< 主码流分辨率，参见eResolutionIndex枚举 */
	int nMainStreamMaxFrame;	/*!< 主码流最大帧率 */
	int enAuxStreamResolution; /*!< 子码流分辨率，参见eResolutionIndex枚举*/
	int nAuxStreamMaxFrame;		/*!< 子码流最大帧率 */
}HY_RESOLUTION_AND_FRAME_CAP;

//编码能力表，在确定了当前主码流和子码流的分辨率之后，主码流的最大帧率和子码流的最大帧率互相影响，
//通过下面的结构体来从前端获取设备的能力表，然后根据当前的制式，主码流的分辨率，子码流的分辨率，以及任意码流的码率，
//来获取另外一个码流的码率
typedef struct tagHY_RESOLUTION_AND_FRAME_CAP_TABLE
{
	int nValid;
	HY_RESOLUTION_AND_FRAME_CAP struResolutionAndFrameCap[MAX_CAP_ITEM];
} HY_RESOLUTION_AND_FRAME_CAP_TABLE, *LPHY_RESOLUTION_AND_FRAME_CAP_TABLE;

//码流的分辨率与码率之间的关系结构体，即不同分辨率的码流对支持的码率的大小是不一样的
//主码流和子码流的分辨率和帧率的关系，
#define MAX_CAP_ITEM_EX (512)
//编码能力表，在确定了当前主码流和子码流的分辨率之后，主码流的最大帧率和子码流的最大帧率互相影响，
//通过下面的结构体来从前端获取设备的能力表，然后根据当前的制式，主码流的分辨率，子码流的分辨率，以及任意码流的码率，
//来获取另外一个码流的码率
typedef struct tagHY_RESOLUTION_AND_FRAME_CAP_TABLE_EX
{
	int nValid;
	HY_RESOLUTION_AND_FRAME_CAP struResolutionAndFrameCap[MAX_CAP_ITEM_EX];
} HY_RESOLUTION_AND_FRAME_CAP_TABLE_EX, *LPHY_RESOLUTION_AND_FRAME_CAP_TABLE_EX;

//码流的分辨率与码率之间的关系结构体，即不同分辨率的码流对支持的码率的大小是不一样的
#define MAX_RESOLUTION_COUNT	(32)
#define MAX_BITRATE_GRADE_COUNT	(16)
//位率动态计算因子
typedef struct tagHY__BITRATE_INFO
{
	int nValid;
	int nBaseBitrate;	    /*!< 帧率为1帧时的比特率*/
	int nMaxBitrate;		/*!< 最大帧率时的比特率*/
	int nMaxFrame;			/*!< 最大帧率*/
}HY_BITRATE_INFO;

typedef struct tagHY_RESOLUTION_AND_BITRATE_CAP
{
	int nValid;						/*!< 本结构体是否有效，0表示无效 */
	int enResolutionFormat;			/*!< 分辨率，参见eResolutionIndex枚举*/
	HY_BITRATE_INFO struBitrateInfo[MAX_BITRATE_GRADE_COUNT];
}HY_RESOLUTION_AND_BITRATE_CAP;

typedef struct tagHY_RESOLUTION_AND_BITRATE_CAP_TABLE
{
	int nValid;
	HY_RESOLUTION_AND_BITRATE_CAP struResolutionAndBitrateCap[MAX_RESOLUTION_COUNT];
}HY_RESOLUTION_AND_BITRATE_CAP_TABLE, *LPHY_RESOLUTION_AND_BITRATE_CAP_TABLE;


typedef struct tagHY_DVR_VI_INFO
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */
	int nResolution;								            /*!< 输入视频的分辨率, 值与HY_DVR_COMPRESSION_INFO结构体中nResolution含义相同*/
	int nFrameRate;                                             /*!< 输入视频的帧率*/
	char acReserved[32];
} HY_DVR_VI_INFO, *LPHY_DVR_VI_INFO;

typedef struct tagHY_DVR_VI_CFG
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */
	HY_DVR_VI_INFO struViInfo[DVR_MAX_VIDEOIN_NUM];				/*!< 各通道视频输入信息 */
}HY_DVR_VI_CFG, *LPHY_DVR_VI_CFG;

#define MAX_SUPPORT_FIRMWARE_VERSION_CNT	(15)

#define MAX_FILECOUNT_PER_VERSION			(5)
#define MAX_FIRMWARE_NAME_LEN				(128)
#define MAX_FIRMWARE_VERSION_LEN			(64)

// 升级版本列表所在的服务器配置
typedef struct
{
	int bValid;
	char		 szHostIP[64];									/*!< 服务器IP地址, 必须是IP地址 */
	int 		 nPort;											/*!< 服务器端口					*/
	char		 szPath[128];									/*!< 服务器存储目录，手动下载此字段没有用到，自动下载此字段为目录				*/
	char         szProtocol[16];								/*!< 服务器访问协议，范围:[http,ftp] */
	char         szUserName[256];								/*!< 服务器登录用户名称, 可以为空    */
	char         szPassword[256];								/*!< 服务器登录用户密码, 可以为空    */
	char 		 nMqttFlag;										/*!< MQTT云升级标记，0，表示是CGI云升级，1表示MQTT云升级,2表示巴西客户RTMP主动注册云升级   */

	char		 szResvered[512 - 1];
}UPGRADE_LIST_SERVER_CFG;

// 升级包版本描述信息
typedef struct
{
	int			bVaild;
	char		szFirmwareName[MAX_FIRMWARE_NAME_LEN];			/*!< 升级包版本唯一标识								*/

	/// 有关此升级文件的简要描述(支持多国语言,utf8格式)
	char		szDescription[4096];							/*!< 数据为XML格式: 如<zh-cn>描述<zh-cn>\r\n<en>desc</en> */

	char        szFirmwareVersion[MAX_FIRMWARE_VERSION_LEN];
	int			nTotalSize;										/*!< 文件总大小 */
	char		szResvered[512 - MAX_FIRMWARE_VERSION_LEN - 4];
}UPGRADE_FIRMWARE_INFO;

// 升级版本列表
typedef struct
{
	int						bValid;
	int						nVersionType;									/*!<  0,表示默认，默认传递最新的版本，1，表示传递所有的版本*/
	UPGRADE_FIRMWARE_INFO	struFileList[MAX_SUPPORT_FIRMWARE_VERSION_CNT];	/*!< 最大支持显示15个版本					*/
}UPGRADE_FIRMWARE_INFO_LIST;

/*
  指定将要升级的版本(并传送升级过程凭证)(指定后，设备端将会启动下载，
  这里没有定义用于中止/终止下载的命令)
*/
#define MAX_UPGRADE_PASSPORT_LEN (64)
// 开始下载升级文件
typedef struct
{
	int					bValid;
	char				szFirmwareName[MAX_FIRMWARE_NAME_LEN];							// 升级包版本名称标识
	char				szUpgradePassport[MAX_UPGRADE_PASSPORT_LEN];	// 升级过程凭证(由APP产生一段随机字符，后续所有操作都将使用它作为凭证)
}UPGRADE_FILE_DOWNLOAD_START;

typedef struct
{
	int		  bValid;
	char	  szFirmwareName[MAX_FIRMWARE_NAME_LEN];	   // 升级包版本名称标识
	char	  szUpgradePassport[MAX_UPGRADE_PASSPORT_LEN]; // 升级过程凭证(由APP产生一段随机字符，后续所有操作都将使用它作为凭证)
    int		  iAutoUpgrade;                                // 自动升级标识;0-手动,1-自动
	int		  iUpgradeFileSize;                            // 升级包大小,单位:byte
	char	  acMD5[128];                                  // 升级包MD5值

    char      strReserved[128];
}UPGRADE_FILE_DOWNLOAD_START_V1;

// 获取升级包下载状态
typedef struct
{
	int					bValid;
	char				szUpgradePassport[MAX_UPGRADE_PASSPORT_LEN];
	int					nStatus;										// 0,空闲 1,正在下载 2,下载完成 3,下载失败
	int					nTotalSize;										// 总大小
	int					nAlreadyTransSize;								// 已下载大小
}UPGRADE_FILE_DOWNLOAD_STATUS;

// t停止下载升级文件
typedef struct
{
	int					bValid;
	char				szUpgradePassport[MAX_UPGRADE_PASSPORT_LEN];	// 升级过程凭证(由APP产生一段随机字符，后续所有操作都将使用它作为凭证)
}UPGRADE_FILE_DOWNLOAD_STOP;

// 开始升级
typedef struct
{
	int					bValid;
	char				szUpgradePassport[MAX_UPGRADE_PASSPORT_LEN];	// 升级凭证
}UPGRADE_CONTROL_START;

// 终止升级
typedef struct
{
	int					bValid;
	char				szUpgradePassport[MAX_UPGRADE_PASSPORT_LEN];	// 升级凭证
}UPGRADE_CONTROL_STOP;

// 获取当前升级是否成功
typedef struct
{
	int					bValid;

	int					nStatus;
	char				szUpgradePassport[MAX_UPGRADE_PASSPORT_LEN];	// 升级凭证
	char				szFirmwareName[MAX_FIRMWARE_NAME_LEN];			// 设备的版本
}UPGRADE_RESULT;

// 错误码与错误码描述信息对
typedef struct
{
	int					nErrCode;										// 错误码
	char				szErrInfo[128];									// 错误描述
}HY_DVR_ERROR_INFO_MAP;

typedef struct
{
	int					bValid;
	HY_DVR_ERROR_INFO_MAP stErrInfo;									// 错误码信息对
}HY_DVR_PROTOCOL_INTERACT_ERROR_INFO;

typedef struct
{
	int					bValid;
	char				szFirmwareName[MAX_FIRMWARE_NAME_LEN];			// 升级包版本名称标识
	char				szMd5sum[MAX_MD5SUM_LEN];						// 升级过程凭证(由APP产生一段随机字符，后续所有操作都将使用它作为凭证)
	int 				nTotalSize;										// 文件总大小
}UPGRADE_FILE_INFO;



typedef struct
{
	int bValid;
	int dwProtocolType;
	int dwPort;
	char cReserve[256];
}HY_DVR_DEVICE_SUPPORT_CLIENT_PROTOCOL;

typedef struct
{
	int bValid;
	HY_DVR_DEVICE_SUPPORT_CLIENT_PROTOCOL cDeviceSupportClientProtocol[32];
}HY_DVR_DEVICE_SUPPORT_CLIENT_PROTOCOL_LIST;


/*!
*  @struct	tagHY_DVR_PUSH_PROTOCOL_DESC
*  @brief	推送协议接口
 */
typedef struct
{
	int bValid;
	int nPushCh;
	char cReserve[256];
}HY_DVR_PUSH_CH;


typedef struct tagHY_DVR_PUSH_PROTOCOL_DESC
{
	int bValid;
	HY_DVR_PUSH_CH AIPushCh[DVR_MAX_ALARMIN_NUM];
	HY_DVR_PUSH_CH MotionPushCh[DVR_MAX_VIDEOIN_NUM];
	HY_DVR_PUSH_CH cReserve[1024];
}HY_DVR_PUSH_PROTOCOL_DESC;


/*!
*  @struct	tagHY_DVR_RTMP_CLIENT_CONNECT_PARAM
*  @brief	RTMP推流参数配置
 */
typedef struct tagHY_DVR_RTMP_CLIENT_CONNECT_PARAM
{
	int bValid;								// 是否有效

	unsigned int dwEnable;					// 是否使能

	char szStreamPullingAddress[512];		// 推流地址
	char szUserName[64];					// 用户名
	char szPassword[64];					// 密码

	unsigned int dwStatus;					// 推流状态

	char szPlayURL[256];					// 播流地址(被动模式有效,只读)

	char cIsPushAudioEnable;				// 是否推音频
	char cReserve[3];
	unsigned int nPort;						//TIANYI RTMP端口
	char szReseved[760];					// 保留字段
}HY_DVR_RTMP_CLIENT_CONNECT_PARAM;

/*!
*  @struct	tagHY_DVR_PUSH_CONFIG
*  @brief	各通道码流的RTMP推流参数配置
*           对应主命令和次命令为　
　　　　　　HY_DVR_GET_RTMP_PUSH_CONFIG
			HY_DVR_SET_RTMP_PUSH_CONFIG
 */
typedef struct tagHY_DVR_PUSH_CONFIG
{
	int bValid;																	// 是否有效

	char szActiveCode[64];														 //激活码，废弃，不使用

	HY_DVR_RTMP_CLIENT_CONNECT_PARAM struMainRTMPConfig[DVR_MAX_VIDEOIN_NUM];	// 主码流
	HY_DVR_RTMP_CLIENT_CONNECT_PARAM struSub1RTMPConfig[DVR_MAX_VIDEOIN_NUM];	// 第一子码流
	HY_DVR_RTMP_CLIENT_CONNECT_PARAM struSub2RTMPConfig[DVR_MAX_VIDEOIN_NUM];	// 第二子码流
	HY_DVR_RTMP_CLIENT_CONNECT_PARAM struSub3RTMPConfig[DVR_MAX_VIDEOIN_NUM];	// 第三子码流
	HY_DVR_RTMP_CLIENT_CONNECT_PARAM struSub4RTMPConfig[DVR_MAX_VIDEOIN_NUM];	// 第四子码流
	HY_DVR_RTMP_CLIENT_CONNECT_PARAM struSub5RTMPConfig[DVR_MAX_VIDEOIN_NUM];	// 第五子码流

	HY_DVR_RTMP_CLIENT_CONNECT_PARAM struResevedConfig[DVR_MAX_VIDEOIN_NUM];	// 保留一路子码流配置

	char bSupportDynRetrievePushAddress;		// 是否支持从服务器获取推流地址
	char bRetrievePushAddressEnable;			// 当前是否从服务器获取推流地址
	char szReserved_1[2];
	char szPushURLServer[128];					// 生成推流地址的服务器URL
	int iAudioFormat;                           // 音频格式; 取值详见 eVoiceMode
    int iSampleRate;                            // 采样率; 0表示0kps采样率, 16表示16kps采样率

	char szReserved[892 - 8];
}HY_DVR_RTMP_PUSH_CONFIG;




//获取和设置，总共有多少条记录，每一页多少个
typedef struct tagHY_DVR_RECORD_FILE_LIST_ATTRIBUTE
{
	int bValid;
	int nSumRecordFileCount;				//录像文件总数
	int nRecordFileCountPerPage;			//每一页的录像文件数
}HY_DVR_RECORD_FILE_LIST_ATTRIBUTE;

//哪一页,只支持设置操作
typedef struct tagHY_DVR_RECORD_FILE_CURRENT_PAGE
{
	int bValid;
	int nIndexPage;							//当前页号
	int nPathType;							//0表示相对于http的路径，1表示绝对路径
}HY_DVR_RECORD_FILE_CURRENT_PAGE;

//录像文件
typedef struct tagHY_DVR_RECORD_FILE
{
	int bValid;
	char strPathName[64];
	char strFileName[32];						//文件名
	HY_DVR_TIME struStartTime;					//开始时间
	HY_DVR_TIME struStopTime;					//结束时间
}HY_DVR_RECORD_FILE;

//录像文件列表
typedef struct tagHY_DVR_RECORD_FILE_LIST
{
	int bValid;
	HY_DVR_RECORD_FILE struRecordFile[100];		//录像文件
}HY_DVR_RECORD_FILE_LIST;

//录像文件（扩展）
typedef struct tagHY_DVR_RECORD_FILE_EX
{
	int bValid;
	char strPathName[64];
	char strFileName[32];						//文件名
	HY_DVR_TIME struStartTime;					//开始时间
	HY_DVR_TIME struStopTime;					//结束时间
	int nFileSize;								//文件大小
	char cReserve[32];
}HY_DVR_RECORD_FILE_EX;

//录像文件列表（扩展）
typedef struct tagHY_DVR_RECORD_FILE_LIST_EX
{
	int bValid;
	HY_DVR_RECORD_FILE_EX struRecordFile[100];		//录像文件
	char cReserve[128];
}HY_DVR_RECORD_FILE_LIST_EX;

//录像文件（V1）
typedef struct tagHY_DVR_RECORD_FILE_V1
{
	int bValid;
	char strPathName[64];
	char strFileName[128];						//文件名
	HY_DVR_TIME struStartTime;					//开始时间
	HY_DVR_TIME struStopTime;					//结束时间
	int nFileSize;								//文件大小
	int nFileType;							//文件类型，0 视频文件， 1 告警文件
	int nDuration;							//录像时长(秒)
	int nChannel;							//媒体通道，对于IPC是0

	char cReserve[20];
}HY_DVR_RECORD_FILE_V1;

//录像文件列表（V1）
typedef struct tagHY_DVR_RECORD_FILE_LIST_V1
{
	int bValid;
	HY_DVR_RECORD_FILE_V1 struRecordFile[100];		//录像文件
	char cReserve[128];
}HY_DVR_RECORD_FILE_LIST_V1;


//抓图文件
//获取和设置，总共有多少条记录，每一页多少个
typedef struct tagHY_DVR_SNAPSHOT_FILE_LIST_ATTRIBUTE
{
	int bValid;
	int nSumSnapshotFileCount;				//录像文件总数
	int nSnapshotFileCountPerPage;			//每一页的录像文件数
}HY_DVR_SNAPSHOT_FILE_LIST_ATTRIBUTE;

// 用于描述要搜索文件的时间段
typedef struct tagHY_DVR_FILE_LIST_ATTRIBUTE_SEARCH_BY_TIME
{
	int bValid;
	int         nTimeZone_Min;				// 时区，与UTC的相差的秒数，如中国就是8*60, 可以实现真正的国际化
	HY_DVR_TIME struStartTime;              // 开始时间
	HY_DVR_TIME struStopTime;               // 结束时间
	unsigned char reservered[512];
}HY_DVR_FILE_LIST_ATTRIBUTE_SEARCH_BY_TIME;

/* CGI需要先调用HY_DVR_RECORD_FILE_LIST_ATTRIBUTE设置每一页的录像文件数nRecordFileCountPerPage
 * 然后调用HY_DVR_RECORD_FILE_CURRENT_PAGE设置当前页号nIndexPage和路径nPathType
 * 最后调用HY_DVR_FILE_LIST_ATTRIBUTE_SEARCH_BY_TIME设置时间段
 *
 * CGI调用HY_DVR_RECORD_FILE_LIST_ATTRIBUTE获取录像文件总数，然后调用HY_DVR_RECORD_FILE_LIST获取文件列表
 */

 //哪一页,只支持设置操作
typedef struct tagHY_DVR_SNAPSHOT_FILE_CURRENT_PAGE
{
	int bValid;
	int nIndexPage;							//当前页号
	int nPathType;
}HY_DVR_SNAPSHOT_FILE_CURRENT_PAGE;

//抓图文件
typedef struct tagHY_DVR_SNAPSHOT_FILE
{
	int bValid;
	char strPathName[64];
	char strFileName[32];						//文件名
	HY_DVR_TIME struSnapshotTime;				//抓图时间
}HY_DVR_SNAPSHOT_FILE;

//抓图文件列表
typedef struct tagHY_DVR_SNAPSHOT_FILE_LIST
{
	int bValid;
	HY_DVR_SNAPSHOT_FILE struSnapshotFile[100];		//录像文件
}HY_DVR_SNAPSHOT_FILE_LIST;


//抓图文件(扩展)
typedef struct tagHY_DVR_SNAPSHOT_FILE_EX
{
	int bValid;
	char strPathName[64];
	char strFileName[32];						//文件名
	HY_DVR_TIME struSnapshotTime;				//抓图时间
	int nFileSize;
	char cReserve[32];
}HY_DVR_SNAPSHOT_FILE_EX;

//抓图文件列表（扩展）
typedef struct tagHY_DVR_SNAPSHOT_FILE_LIST_EX
{
	int bValid;
	HY_DVR_SNAPSHOT_FILE_EX struSnapshotFile[100];		//录像文件
	char cReserver[128];
}HY_DVR_SNAPSHOT_FILE_LIST_EX;


#define HY_DVR_MAX_TITLE_LENGTH						32

#define HY_DVR_MAX_DEVICE_NAME_LENGTH				40

#define HY_DVR_MAX_CHANNEL_NAME_LENGTH				40

#define HY_DVR_MAX_EVENT_CREATE_TIME_LENGTH	 		20

#define HY_DVR_MAX_CUSTOM_LENGTH					80

#define HY_DVR_MAX_APP_CUSTOM_PARAM_LENGTH			80

#define HY_DVR_MAX_DID_LENGTH  						64

typedef struct tagHY_DVR_EVENT_INFO
{
	char  strPushTitle[HY_DVR_MAX_TITLE_LENGTH];					//推送标题
	char  strDeviceName[HY_DVR_MAX_DEVICE_NAME_LENGTH];				//设备名称（UTF8编码），可以为空，如果与strChannelName同时为空的话，那么strCustomContent字段有效.
	char strChannelName[HY_DVR_MAX_CHANNEL_NAME_LENGTH];			//通道名称（UTF8编码）可以为空，如果与strDeviceName同时为空的话，那么strCustomContent字段有效.
	char strEventCreateTime[HY_DVR_MAX_EVENT_CREATE_TIME_LENGTH];	//事件创建时间(ISO 8601标准格式的时间,例如：20040503T173008+08)
	char strCustomContent[HY_DVR_MAX_CUSTOM_LENGTH];				//自定义消息，供特殊消息使用
	char strAPPCustomParam[HY_DVR_MAX_APP_CUSTOM_PARAM_LENGTH];		//自定义参数，透传给APP使用
} HY_DVR_EVENT_INFO;



typedef struct tagHY_DVR_PUSH_PROTOCOL_INFO
{
	int bValid;
	char szDevieDID[HY_DVR_MAX_DID_LENGTH];			//设备DID
	unsigned int nPushInterval;						//推送间隔
	unsigned int nMaxEventCount;					//最大事件缓冲区个数
}HY_DVR_PUSH_PROTOCOL_INFO;


typedef struct tagHY_TIMED_CAPTURE_INFO
{
	int bValid;
	int bEnable;
	int uTimeInterval;
    int nCapturePictureCount;
	char cReserve[64 - 4];
}HY_TIMED_CAPTURE_INFO;

typedef struct tagHY_TIMED_CAPTURE_CFG
{
	int bValid;
	HY_TIMED_CAPTURE_INFO struFtpTimedCaptureInfo;		//FTP抓图
	HY_TIMED_CAPTURE_INFO struSDTimedCaptureInfo;		//SD抓图
    HY_TIMED_CAPTURE_INFO struMQTTTimedCaptureInfo;		//P6SEvent-MQTT抓图
	HY_TIMED_CAPTURE_INFO cReserve[3 - 1];
}HY_TIMED_CAPTURE_CFG;


typedef struct tagHY_TIMED_CAPTURE_CFG_EX
{
	int bValid;
	int bEnable;                        //是否开启定时抓拍；0 - 关闭， 1 - 开启
	char cReserve[128];
}HY_TIMED_CAPTURE_CFG_EX;

typedef struct tagHY_DVR_4G_NETCARD_INFO
{
	int bValid;
	char szIMSI[16];
	char szIMEI[16];
	char szACT[64];
	char szOperator[64];			//运营商
	char szBand[64];				//频段信息
	char szICCID[32];				//ICCID
    int bIsExternalCard;            //是否是金睿外部卡，1 为外部卡，0 为内部卡, 2 为北纬卡
    int bIsHaveSurplusFlow;         //是否有可用流量,1 为有可用流量，0 为没有可用流量
	char ac4GModuleVersion[128];	//4G模组版本号
	int i4GSignalStrength;			//4G信号强度
	int iCardWorkMode;              /*!< 卡工作模式，0：实体卡模式 1：云卡模式 */

	char cReserve[1872];
}HY_DVR_4G_NETCARD_INFO;

typedef struct tagHY_DVR_4G_NETCARD_CONFIG
{
	int bValid;
	int nAPNMode;					//访问点模式，0,自动，1，手动
	char szAPNServer[64];
	char szAPNUserName[64];
	char szAPNPassword[64];
	int nRegisterStatus;			//注册状态0,表示成功，1，表示锁住，2，表示失败
	int nSignalVal;					//信号强度，0-100
	int nEnableModWander;			//模块漫游使能0,关闭，1，开启
	char strIPAddress[16];			//4G模块IP
	char strGateway[16];			//4G模块网关
	int iEnable;                    //使能,0:false,1:true
	char cReserve[2010];
}HY_DVR_4G_NETCARD_CONFIG;


typedef struct tagHY_DVR_TELNET_CONFIG
{
	int bValid;
	int nEnableTelnet;		//0,表示关闭，1，表示开启
	char cReserve[64];
}HY_DVR_TELNET_CONFIG;


typedef struct tagHY_DVR_DEVICE_CAP
{
	int bValid;
	int nChannel;
	char cBuffer[128 * 1024];
	int nBufferLen;
}HY_DVR_DEVICE_CAP;

typedef struct tagHY_DVR_DEVICE_CONTROL_ERROR_CODE
{
	int bValid;
	int nReturn;
	char strReturnDescription[256];
	char cReserve[1024];
}HY_DVR_DEVICE_CONTROL_ERROR_CODE;



#define MAX_PICTURE_LENGTH   350 *1024  //该大小保持和CGI缓冲区的大小保持一致

typedef struct tagHY_DVR_PICTURE_BY_BUFFER
{
	int bValid;
	int nChannelID;							/*输入参数，通道号，从0开始*/
	int nStreamID;							/*输入参数，码流类型，0，主码流，1，第一子码流...*/
	int nPictureFormat;						/*输入参数，图片格式，0，bmp，1，jpg*/
	char cReserve1[256];
	char* pBuffer;							/*输出参数，图片数据,该参数虽然是指针，但是该指针对应的buffer的空间由调用者申请，大小有应用层指定，同时也受xmlbuffer最大大小限制*/
	int nBufferLen;							/*输出参数，图片真实大小		*/
	int nWidth;								/*输出参数，图片真实宽		*/
	int nHeight;							/*输出参数，图片真实高		*/
	char cReserve2[1024 - 8];
}HY_DVR_PICTURE_BY_BUFFER;

typedef struct tagHY_DVR_PICTURE_ATTRIBUTE
{
    int iValid;

    int iHorizontalPixel;                      /* 图片水平像素        */
    int iVerticalPixel;                        /* 图片垂直像素        */

    char acReserve[256];
}HY_DVR_PICTURE_ATTRIBUTE;

typedef struct tagHY_DVR_CLOUD_STORAGE_SNAPSHOT_PIC
{
    int nValid;
    int nEventMainType;     //输入参数,事件主类型
    int nEventAssistType;   //输入参数，事件子类型
    char cPicBuffer[256 * 1024];    //输出参数
    int nPicLen;                    //输出参数
    char cReserve[128];             //保留字段
}HY_DVR_CLOUD_STORAGE_SNAPSHOT_PIC,*LPHY_DVR_CLOUD_STORAGE_SNAPSHOT_PIC;

typedef struct tagHY_DVR_SWP_PLANTFORM_DISPLAY_ITEM_CONFIG
{
    int iValid;

    int iSnapshotDynamicDisplay;                //抓拍内容动态显示设定项,1:显示抓拍图片(对应xml中snapshot),2:显示脸库图片(对应xml中face),3:不显示任何内容(对应xml中no)

    char acReserved[256];
}HY_DVR_SWP_PLANTFORM_DISPLAY_ITEM_CONFIG, *LPHY_DVR_SWP_PLANTFORM_DISPLAY_ITEM_CONFIG;

/*!
*  @struct    tagHY_DVR_HEALTHCODE_DEVICE_AND_PLAT_INFO
*  @brief    设备和平台信息
 */
typedef struct tagHY_DVR_HEALTHCODE_DEVICE_AND_PLAT_INFO
{
    int iValid;                                               /*!< 本结构体是否有效，0表示无效 */

    char acRegionID[64];                                      /*!< 封闭区域 ID */
    char acGuardID[64];                                       /*!< 门禁 ID */
    char acVillageCode[64];                                   /*!< 小区编码 */
    char acToken[64];                                         /*!< token */
    char acServerAddress[256];                                /*!< appkey */
    char acAppKey[64];                                        /*!< appkey */
    char acIV[64];                                            /*!< 秘钥偏移量 */

    char cReserve[1024];                                        /*!< 保留*/
} HY_DVR_HEALTHCODE_DEVICE_AND_PLAT_INFO, *LPHY_DVR_HEALTHCODE_DEVICE_AND_PLAT_INFO;

/*!
*  @struct    tagHY_DVR_UNICOM_AUTH_INFO
*  @brief       联通激活信息结构
 */
typedef struct tagHY_DVR_UNICOM_AUTH_INFO
{
	int nValid;

	int nAuthStatus;		//授权状态
	char szSerialNumber[32];	//序列号
	char szUnicomEI[16];			//15byte十进制数，前8byte是TUI码，不同型号产品不同（如天翼3MP室外枪机TUI为 18483912），后7位从0000001开始递增，每台设备唯一（示例：184839120000011）
	char szDeviceCode[16];		//设备码
	char szDevicePassword[16];	//设备密码
	char szBindVerifyCode[16];	//绑定验证码
	char szBindCode[64];		//设备绑定标识二维码

	char szReserved[256];
} HY_DVR_UNICOM_AUTH_INFO, *LPHY_DVR_UNICOM_AUTH_INFO;

typedef struct tagHY_DVR_AUTH_INFO
{
	int bValid;
	char strAuthInfo[512];		//授权信息，目前仅仅只是授权码，后面如果有其他需要传递，只需要客户端和中间层自定义格式自行解析
	char cReserve[256];
}HY_DVR_AUTH_INFO;

typedef struct tagHY_DOMAIN_INFO
{
	int nValid;

	char szDomain[64];
	char szIP[16];

	char szReserved[256];
}HY_DOMAIN_INFO, *LPHY_DOMAIN_INFO;

typedef struct tagHY_P2P_CONNECT_INFO
{
	int nValid;

	char szPlatformName[16];
	char szConnectString[256];
	char szAuthWord[32];
	HY_DOMAIN_INFO struDomain[8];

	char szReserved[256];
}HY_P2P_CONNECT_INFO, *LPHY_P2P_CONNECT_INFO;

typedef struct tagHY_P2P_CONNECT_INFO_V2
{
	int nValid;

	char szPlatformName[16];
	char szConnectString[256];
	char szAuthWord[32];
	HY_DOMAIN_INFO struDomain[8];
	char acTransConfig[1024];

	char szReserved[1024];
}HY_P2P_CONNECT_INFO_V2, *LPHY_P2P_CONNECT_INFO_V2;

typedef struct tagHY_DVR_AUTH_INFO_V2
{
	int nValid;

	char szAuthInfo[256];
	HY_P2P_CONNECT_INFO struP2PConnectInfo;

	char szReserved[32];
}HY_DVR_AUTH_INFO_V2, *LPHY_DVR_AUTH_INFO_V2;

typedef struct tagHY_DVR_TRANSPARENT_INFO_PUSHDOMAIN_PARAM
{
    int nValid;

    char acDomainSuffix[64];                       /* 域名后缀*/

    char szReserved[256];
}HY_DVR_TRANSPARENT_INFO_PUSHDOMAIN_PARAM, *LPHY_DVR_TRANSPARENT_INFO_PUSHDOMAIN_PARAM;

typedef struct tagHY_DVR_DOMAINLIST_MANAGERSERVER_PARAM
{
    int nValid;

    char acDomainListManagerServerAdd[128];        /* 域名列表管理服务器地址 */
	int iDomainListManagerServerPort;              /* 域名列表管理服务器端口 */

    char szReserved[252];
}HY_DVR_DOMAINLIST_MANAGERSERVER_PARAM, *LPHY_DVR_DOMAINLIST_MANAGERSERVER_PARAM;

typedef struct tagHY_DVR_AUTH_INFO_TRANSPARENT_INFO
{
    int nValid;

    HY_DVR_TRANSPARENT_INFO_PUSHDOMAIN_PARAM stPushDomainParam;        /* 推送域名参数*/
    HY_DVR_DOMAINLIST_MANAGERSERVER_PARAM stManagerServerParam;        /* 域名列表管理服务器参数 */

    char szReserved[1024];
}HY_DVR_AUTH_INFO_TRANSPARENT_INFO, *LPHY_DVR_AUTH_INFO_TRANSPARENT_INFO;

typedef struct tagHY_DVR_PUSH_DOMAIN
{
	int nValid;
	char szDomain[64];
	char szReserved[32];
}HY_DVR_PUSH_DOMAIN, *LPHY_DVR_PUSH_DOMAIN;

typedef struct tagHY_DVR_PUSH_DOMAIN_LIST
{
	int nValid;
	HY_DVR_PUSH_DOMAIN stDomainList[32];
}HY_DVR_PUSH_DOMAIN_LIST, *LPHY_DVR_PUSH_DOMAIN_LIST;


typedef struct tagHY_DVR_KEEP_ALIVE_DOMAIN
{
	int nValid;

	char szDomain[128];
    int nPort;

	char szReserved[128];
}HY_DVR_KEEP_ALIVE_DOMAIN, *LPHY_DVR_KEEP_ALIVE_DOMAIN;

typedef struct tagHY_DVR_KEEP_ALIVE_DOMAIN_LIST
{
	int nValid;
	HY_DVR_KEEP_ALIVE_DOMAIN stDomainList[8];
	int wakeUpServerType;												/*唤醒服务器类型，2:使用xrs唤醒服务器；其他为使用尚云唤醒服务器*/

    char szReserved[256 - 4];
}HY_DVR_KEEP_ALIVE_DOMAIN_LIST, *LPHY_DVR_KEEP_ALIVE_DOMAIN_LIST;

typedef struct tagHY_DVR_WS_MANAGER_DOMAIN_LIST      // 新增的RSL服务器列表，原保活服务器列表保留字段不够，所以新增
{
	int nValid;
	HY_DVR_KEEP_ALIVE_DOMAIN stWSDomainList[8];

    char szReserved[256];
}HY_DVR_WS_MANAGER_DOMAIN_LIST, *LPHY_DVR_WS_MANAGER_DOMAIN_LIST;

typedef struct tagHY_DVR_AUTH_INFO_P2P_INFO
{
	int nValid;

	int iP2PHDR;                                                /*是否支持P2PHDR，0-不支持，1-支持*/

	char szReserved[128];
}HY_DVR_AUTH_INFO_P2P_INFO, *LPHY_DVR_AUTH_INFO_P2P_INFO;

typedef struct tagHY_PTZ_ADVANCE_CONFIG
{
	int bValid;
	int nChannel;
	char nCruiseEnable;
	int nBeginPresetNumForCruise;
	int nBeginPresetActionForCruise;			//0,表示none,1,表示set,2,表示call,3,表示clear
	int nEndPresetNumForCruise;
	int nEndPresetActionForCruise;				//0,表示none,1,表示set,2,表示call,3,表示clear
	char nWatchEnable;
	int nBeginPresetNumForWatch;
	int nBeginPresetActionForWatch;				//0,表示none,1,表示set,2,表示call,3,表示clear
	int nEndPresetNumForWatch;
	int nEndPresetActionForWatch;				//0,表示none,1,表示set,2,表示call,3,表示clear
}HY_PTZ_ADVANCE_CONFIG;

typedef struct tagHY_PTZ_ADVANCE_CONFIG_V1
{
	int bValid;
	int nChannel;
	char nCruiseEnable;
	int nBeginPresetNumForCruise;
	int nBeginPresetActionForCruise;			//0,表示none,1,表示set,2,表示call,3,表示clear
	int nEndPresetNumForCruise;
	int nEndPresetActionForCruise;				//0,表示none,1,表示set,2,表示call,3,表示clear
	char nWatchEnable;
	int nBeginPresetNumForWatch;
	int nBeginPresetActionForWatch;				//0,表示none,1,表示set,2,表示call,3,表示clear
	int nEndPresetNumForWatch;
	int nEndPresetActionForWatch;				//0,表示none,1,表示set,2,表示call,3,表示clear
	char nTrackEnable;
	int nBeginPresetNumForTrack;
	int nBeginPresetActionForTrack;
	int nEndPresetNumForTrack;
	int nEndPresetActionForTrack;
}HY_PTZ_ADVANCE_CONFIG_V1;

typedef struct tagHY_AUDIO_ALARM_CONFIG
{
	int bValid;
	int nEnable;			//0,false,1,true	
	int nAudioType;			//0：自定义，1：警报，2：犬吠，3：警戒区域，4：监控区域，5：欢迎光临，6：深水警告，7：私人领地，8：危险区域，9：贵重物品，10：高处危险，
										//11：私家价位，12：叮咚,		13:有口罩,		14:无口罩    15：注意私人物品     16：抓小偷     17：救命		18：分类		19：可回收垃圾		20：家庭食物垃圾		
										//21：危险废物		22：残余垃圾		23：全部分类	24：车形自定义		25：禁止停车
	int nAudioDelayOutput;	//单位为S
	int nAudioVolume;		//音频音量1~100，如果音量为0，那么音量设为默认值50
}HY_AUDIO_ALARM_CONFIG;

typedef struct tagHY_AUDIO_ALARM_CONFIG_EX
{
    int bValid;
    int nEnable;            //0,false,1,true   
    int nAudioType;         //0：自定义，1：警报，2：犬吠，3：警戒区域，4：监控区域，5：欢迎光临，6：深水警告，7：私人领地，8：危险区域，9：贵重物品，10：高处危险，
                                        //11：私家价位，12：叮咚,        13:有口罩,     14:无口罩    15：注意私人物品     16：抓小偷     17：救命        18：分类       19：可回收垃圾        20：家庭食物垃圾      
                                        //21：危险废物       22：残余垃圾     23：全部分类 24：车形自定义        25：禁止停车
    int nAudioDelayOutput;  //单位为S
    int nAudioVolume;       //音频音量1~100，如果音量为0，那么音量设为默认值50
 
    int iAudioPlayTimes;                /* 语音报警语音播报默认次数,为0时表示自定义, 1/3/5/10/20 */
	int iDayMode;              /*!< 0:夜晚模式,1:白天模式,2:不启用此模式（AllDay字段有效，当字段不为disable时，AllDay字段无效）当设置为此模式时，IPC依赖自身传感器判断当前环境是白天还是夜晚*/
    HY_DVR_SCHEDTIME struSchedTime;     /* 语音报警布防计划 */
 
   char acReserved[512];
}HY_AUDIO_ALARM_CONFIG_EX;

typedef struct tagHY_DVR_AUDIO_ALARM_CONFIG_TIMESLOT_LIST
{
    int iValid;

	int nChannel;                                                   /*!< 通道号 */
    HY_DVR_SCHEDULE_TIMESLOT stSchedTimeSlot[SCHEDULE_TIMESLOT_MAX_NUM];     /*!< 布防时间计划 */
    
    char szReserved[1024];
}HY_DVR_AUDIO_ALARM_CONFIG_TIMESLOT_LIST, *LPHY_DVR_AUDIO_ALARM_CONFIG_TIMESLOT_LIST;

typedef struct tagHY_DVR_CONNECT_RESOURCE_INFO
{
	int bValid;
	int nMaxClientCount;
	int nMaxRealStreamCount;
	int nMaxPlaybackStreamCount;
	char cReserve[256];
}HY_DVR_CONNECT_RESOURCE_INFO;


//人脸抓拍功能结构体

//区域过滤配置

typedef struct tagHY_DVR_REGION_SIZE
{
	int nHeight;   //范围是(0-10000)
	int nWidth;	   //范围是(0-10000)
}HY_DVR_REGION_SIZE;

typedef struct tagHY_DVR_FACE_SNAP_FILTER_REGION
{
	int nValid;
	int nChannelID;								//设置和获取，该参数都有用
	HY_DVR_REGION_SIZE struMaxFaceRegionSize;	//区域的分辨率是10000，
	HY_DVR_REGION_SIZE struMinFaceRegionSize;	//区域的分辨率是10000，
	char cReserve[64];
}HY_DVR_FACE_FILTER_REGION;


//区域检测配置
typedef struct tagHY_DVR_FACE_SNAP_DETECT_REGION
{
	int nValid;
	int nChannelID;							//设置和获取，该参数都有用
	HY_DVR_REGION struFaceDetectRegion;		//区域的分辨率是10000*10000，
	char cReserve[64];
}HY_DVR_FACE_DETECT_REGION;


typedef struct tagHY_DVR_FACE_SNAP_MODE_AUTO
{
	int nValid;
	int nSnapshotCount;				//当前抓拍张数，目前暂定最大张数是3
	int nSnapShotInterval;			//抓拍间隔，时间是ms
	int nMaxSnapshotCount;		//最大抓拍张数
	int nSensitivity;						//灵敏度
	char cReserve[24];				//保留字段
}HY_DVR_FACE_SNAP_MODE_AUTO;

typedef struct tagHY_DVR_FACE_SNAP_MODE_FAST
{
	int nValid;
	int nSnapshotCount;			//抓拍张数，目前暂定最大张数是3
	int nIsSupportSnapshotInterval;  //是否支持抓图间隔
	int nSnapShotInterval;		//抓拍间隔，时间是ms
	int bHighQuality;				//高质量过滤的开关使能
	int nMaxSnapshotCount;		//最大抓拍张数
	int nSensitivity;						//灵敏度
	char cReserve[12];			//保留字段
}HY_DVR_FACE_SNAP_MODE_FAST;


typedef struct tagHY_DVR_FACE_SNAP_MODE_OPTIMAL
{
	int nValid;
	int nSnapshotCount;			//抓拍张数，目前暂定最大张数是3
	int nIsSupportSnapshotInterval;  //是否支持抓拍间隔
	int nSnapShotInterval;		//抓拍间隔，时间是ms
	int nIsSupportSnaphotDelay;     //是否支持抓拍延时
	int nSnapShotDelay;		    //抓拍延时，时间是ms
	int nMaxSnapshotCount;		//最大抓拍张数
	int nSensitivity;						//灵敏度
	char cReserve[8];			//保留字段
}HY_DVR_FACE_SNAP_MODE_OPTIMAL;



typedef struct tagHY_DVR_SECTION_THRESHOLD
{
	int nMaxValue;
	int nMinValue;
}HY_DVR_SECTION_THRESHOLD;

typedef struct tagHY_DVR_FACE_SNAP_MODE_CUSTOM
{
	int nValid;
	int nSnapshotCount;			//抓拍张数，目前暂定最大张数是3
	int nPicQualityThreshold;	//画质阀值 0-100
	HY_DVR_SECTION_THRESHOLD struFacePixel;		//人脸像素区间 0-~
	HY_DVR_SECTION_THRESHOLD struObliqueAngle;	//斜脸角区间 0-180
	HY_DVR_SECTION_THRESHOLD struPitchAngle;	//俯仰角区间 0-180
	HY_DVR_SECTION_THRESHOLD struSideAngle;		//侧脸角区间 0-180
	HY_DVR_SECTION_THRESHOLD struFuzzyDegree;	//模糊程度区间 0-100 值越大，越模糊
	HY_DVR_SECTION_THRESHOLD struReserve[10];	//保留字段
	int nMaxSnapshotCount;		//最大抓拍张数
	int nSensitivity;						//灵敏度
	char cReserve[24];			//保留字段
}HY_DVR_FACE_SNAP_MODE_CUSTOM;

typedef struct tagHY_DVR_FACE_SNAP_MODE_PASS
{
	short int nValid;
	unsigned short int nSnapShotInterval;		//单位：毫秒	200-5000
	char nSensitivity;						//灵敏度
	char szReserved[3];
	//char reserved[4];
}HY_DVR_FACE_SNAP_MODE_PASS;

//抓图配置
typedef struct tagHY_DVR_FACE_SNAP_CONFIG
{
	int nValid;
	int nChannelID;				//设置和获取，该参数都有用
	int nFaceSnapMode;			//抓图模式，0，自定义，1，自动模式，2，最优模式，3，快速模式，4，通行抓拍模式
	int nCutoutMode;			//抠图方式，0，人脸抠图，1，半身抠图，2，全身抠图
	HY_DVR_FACE_SNAP_MODE_AUTO struFaceSnapModeAuto;
	HY_DVR_FACE_SNAP_MODE_FAST struFaceSnapModeFast;
	HY_DVR_FACE_SNAP_MODE_OPTIMAL struFaceSnapModeOptimal;
	HY_DVR_FACE_SNAP_MODE_CUSTOM struFaceSnapModeCustom;
	HY_DVR_FACE_SNAP_MODE_PASS struFaceSnapModePass;
	int nSaveSnapImage;			//是否保存抓拍图片，0：不支持，1：保存，2：不保存
	char cReserve[64 - 8 - 4];							//保留字段	-8:HY_DVR_FACE_SNAP_MODE_PASS结构体	-4：nSaveSnapImage
}HY_DVR_FACE_SNAP_CONFIG;

//人脸叠加配置
typedef struct tagHY_DVR_FACE_TRACK_FRAME
{
	int nEnableFaceTrackFrame;			//人脸跟踪框 ，0，不叠加人脸跟踪信息，1叠加人脸跟踪信息
	int nEnableBackEndSuperposition;	//后端叠加使能
}HY_DVR_FACE_TRACK_FRAME;

typedef struct tagHY_DVR_FACE_SNAP_OVERLAY_INFO
{
	int nValid;
	int nChannelID;						//设置和获取，该参数都有用
	HY_DVR_FACE_TRACK_FRAME struFaceTrackFrame;  //人脸跟踪叠加配置
	int nEnableFaceStatistcs;			//人脸统计信息 0，不叠加人脸统计信息，1，叠加人脸统计信息
	HY_DVR_FACE_TRACK_FRAME struSubStreamFaceTrackFrame;		//子码流人脸跟踪叠加配置
	HY_DVR_FACE_TRACK_FRAME struSecondSubStreamFaceTrackFrame;		//第二子码流人脸跟踪叠加配置
	char cReserve[64 - 16];					//保留信息(-16：增加了两个HY_DVR_FACE_TRACK_FRAME结构体)
}HY_DVR_FACE_SNAP_OVERLAY_INFO;


//私有协议
typedef struct tagHY_DVR_SINGLE_PROTOCOL_STATISTICS
{
	int nValid;									//0,表示该协议不支持，1，表示该协议支持
	int nClientCount;							//客户端个数
	int nRealStreamCount;						//实时流个数
	int nHistoryStreamCount;					//历史流个数
	int nUplinkBandwidth;						//上线带宽 单位是Byte/s
	int nDownLinkBandwidth;						//下行带宽 单位是Byte/s
	int cReserve[32];
}HY_DVR_SINGLE_PROTOCOL_STATISTICS;


typedef struct tagHY_DVR_ALL_CLIENT_AND_FLOW_STATISTICS
{
	HY_DVR_SINGLE_PROTOCOL_STATISTICS struSingleProtocolStatistics[15]; //数字元素下标的值对应的协议参考枚举eProtocolID
}HY_DVR_ALL_CLIENT_AND_FLOW_STATISTICS;




//////////////////////////////////////////////////////////////////////////////
//人脸识别功能结构体
//////////////////////////////////////////////////////////////////////////////

//人脸库操作
typedef struct tagHY_DVR_CREATE_FACE_GROUP
{
	int nValid;
	char strGroupName[32];		//作为输入参数,脸库名字
	long long nGroupID;			//作为输出参数,脸库ID
}HY_DVR_CREATE_FACE_GROUP;

typedef struct tagHY_DVR_CREATE_FACE_GROUP_V1
{
	int nValid;
	char strGroupName[32];		//作为输入参数,脸库名字
	long long nGroupID;			//作为输出参数,脸库ID
	char szVersion[32];			//脸库版本号
	char acGroupID2[64];        //脸库ID(人脸识别前抓前比功能使用)
    char acOwnner[64];          //安全码(人脸识别前抓前比功能使用)
    char acVersionID[64];       //版本ID(人脸识别前抓前比功能使用)
	char acSystemTime[32];        //系统时间(仅人脸识别前抓前比功能使用)
    float fGroupThresholdValue;   //脸库阈值(仅人脸识别前抓前比功能使用)

    char szReserved[796];
}HY_DVR_CREATE_FACE_GROUP_V1;

typedef struct tagHY_DVR_MODIFY_FACE_GROUP
{
	int nValid;
	char strGroupName[32];		//作为输入参数,脸库名字
	long long nGroupID;			//作为输入参数,脸库ID
}HY_DVR_MODIFY_FACE_GROUP;

typedef struct HY_DVR_MODIFY_FACE_GROUP_V1
{
	int nValid;
	char strGroupName[32];		//作为输入参数,脸库名字
	long long nGroupID;			//作为输入参数,脸库ID
	char szVersion[32];			//脸库版本号
	char acGroupID2[64];        //脸库ID(人脸识别前抓前比功能使用)
    char acOwnner[64];          //安全码(人脸识别前抓前比功能使用)
    char acVersionID[64];       //版本ID(人脸识别前抓前比功能使用)
	char acSystemTime[32];      //系统时间(仅人脸识别前抓前比功能使用)
    float fGroupThresholdValue; //脸库阈值(仅人脸识别前抓前比功能使用)

    char szReserved[796];
}HY_DVR_MODIFY_FACE_GROUP_V1;

typedef struct HY_DVR_MODIFY_FACE_GROUP_LIST_V1
{
	int nValid;
	HY_DVR_MODIFY_FACE_GROUP_V1 *pStruModifyFaceGroupListV1;

	char szReserved[512];
}HY_DVR_MODIFY_FACE_GROUP_LIST_V1;

typedef struct tagHY_DVR_DELETE_FACE_GROUP
{
	int nValid;
	long long nGroupID;			//作为输入参数,脸库ID
	int nIsDelBindFace;			//作为输入参数,是否删除绑定的人脸图片信息0,不删除，1，删除
}HY_DVR_DELETE_FACE_GROUP;

typedef struct tagHY_DVR_DELETE_FACE_GROUP_V1
{
    int nValid;

    long long nGroupID;           //作为输入参数,脸库ID
    int nIsDelBindFace;           //作为输入参数,是否删除绑定的人脸图片信息0,不删除，1，删除
    char acGroupID2[64];          //脸库ID(人脸识别前抓前比功能使用)
    char acOwnner[64];            //安全码(人脸识别前抓前比功能使用)
	char acSystemTime[32];        //系统时间(仅人脸识别前抓前比功能使用)

    char szReserved[992];
}HY_DVR_DELETE_FACE_GROUP_V1;

#define HY_DVR_MAX_GROUP_FACE_COUNT   16
#define HY_DVR_MAX_GROUP_FACE_COUNT_V1   100

typedef struct tagHY_DVR_FACE_GROUP
{
	int nValid;
	long long nGroupID;			//作为输出参数,脸库ID
	char strGroupName[32];		//作为输出参数,脸库名字
	int nAvailableStatus;		//作为输出参数,脸库可用状态,0,不可用，1，可用
	int nPersonInfoCount;		//作为输出参数,脸库人员信息个数
	HY_DVR_TIME struCreateTime;	//作为输出参数，脸库创建的时间
}HY_DVR_FACE_GROUP;

typedef struct tagHY_DVR_FACE_GROUP_V1
{
	int nValid;
	long long nGroupID;			//作为输出参数,脸库ID
	char strGroupName[32];		//作为输出参数,脸库名字
	int nAvailableStatus;		//作为输出参数,脸库可用状态,0,不可用，1，可用
	int nPersonInfoCount;		//作为输出参数,脸库人员信息个数
	HY_DVR_TIME struCreateTime;	//作为输出参数，脸库创建的时间
	char szVersion[32];
	int	iValidPersonCount;		//有效人数
	int iInvalidPersonCount;	//无效人数
	char acGroupID2[64];			//作为输出参数,脸库ID
	float fGroupThresholdValue; //作为输出参数,脸库阈值(仅人脸识别前抓前比功能使用)
    int bSelected;           //该节点在某条规则中是否选中，按位取值，bit0 为 1 则表示该节点在规则一中被选中；bot0 为 0 则表示该节点在规则一中未被选中

	char szReserved[944];
}HY_DVR_FACE_GROUP_V1;

typedef struct tagHY_DVR_FACE_GROUP_LIST
{
	int nValid;
	HY_DVR_FACE_GROUP struFaceGroup[HY_DVR_MAX_GROUP_FACE_COUNT];
}HY_DVR_FACE_GROUP_LIST;

typedef struct tagHY_DVR_FACE_GROUP_LIST_V1
{
	int nValid;
	HY_DVR_FACE_GROUP *pStruFaceGroup;
}HY_DVR_FACE_GROUP_LIST_V1;

typedef struct tagHY_DVR_FACE_GROUP_LIST_V2
{
	int nValid;
	HY_DVR_FACE_GROUP_V1 *pStruFaceGroupV1;

	char szReserved[512];
}HY_DVR_FACE_GROUP_LIST_V2;

//作为输出参数
typedef struct tagHY_DVR_FACE_RECO_MODULE_WORK_STATE
{
	int nValid;
	int nWorkState;			//0,正在初始化，1，正常工作，2，数据恢复中，3，建模过程中
	int nErrorCode;			//返回当前旷世的错误码
}HY_DVR_FACE_RECO_MODULE_WORK_STATE;

typedef struct tagHY_DVR_CHECK_PERSON_INFO
{
	int nValid;
	long long nPersonID;					//作为输入参数，PersonID，对于添加人员信息时，PersonID默认为0，对于修改人员信息时,使用对应的人员信息PersonID
	char strName[32];				//作为输入参数,姓名
	int nSex;						//作为输入参数,性别,0,male,1,female
	int nCertificateType;			//作为输入参数,身份证类型 0，passport,1,身份证，0xff，其他
	char strCertificateID[32];		//作为输入参数,ID
	char strCountry[32];            //作为输入参数,国家
	char strAddress[64];			//作为输入参数,地址
	HY_DVR_TIME struBirthday;		//作为输入参数,生日
}HY_DVR_CHECK_PERSON_INFO;

typedef struct tagHY_DVR_CHECK_PERSON_INFO_V1
{
	int nValid;
	long long nPersonID;					//作为输入参数，PersonID，对于添加人员信息时，PersonID默认为0，对于修改人员信息时,使用对应的人员信息PersonID（>
	char strName[32];				//作为输入参数,姓名
	int nSex;						//作为输入参数,性别,0,male,1,female
	int nCertificateType;			//作为输入参数,身份证类型 0，passport,1,身份证，0xff，其他
	char strCertificateID[32];		//作为输入参数,ID
	char strCountry[32];            //作为输入参数,国家
	char strAddress[64];			//作为输入参数,地址
	HY_DVR_TIME struBirthday;		//作为输入参数,生日
	char strUniqueID[32];
	char strOrganizationInfo[92];			//作为输入参数，组织信息
	char strNoteInfo[100];				//作为输入参数，备注信息
	char cEnable;							//作为输入参数，使能
	char strCardNum[32];			//作为输入参数,卡号
	char reserved[3];
	char szReserved[28];
}HY_DVR_CHECK_PERSON_INFO_V1;

//添加人员信息
typedef struct tagHY_DVR_ADD_PERSON_INFO_AND_IMAGE_INFO
{
	int nValid;
	long long nGroupID;				//作为输入参数，表示人脸库ID
	int nIgnoreError;				//作为输入参数,是否忽略错误
	char strName[32];				//作为输入参数,姓名
	int nSex;						//作为输入参数,性别,0,male,1,female
	int nCertificateType;			//作为输入参数,身份证类型 0，passport,1,身份证，0xff，其他
	char strCertificateID[32];		//作为输入参数,ID
	char strCountry[32];			//作为输入参数,国家
	char strAddress[64];			//作为输入参数,地址
	HY_DVR_TIME struBirthday;		//作为输入参数,生日
	int nFaceImageID;				//作为输入参数，图片ID
	int  nPicLength;				//作为输入参数,图片的长度
	char * pstrPicBuffer;			//作为输入参数,图片的缓冲区,中间层必须得复制一份，协议层在传递完了之后，会释放内存
}HY_DVR_ADD_PERSON_INFO_AND_IMAGE_INFO;

typedef struct tagHY_DVR_PERSON_V1_ENTERN
{
	int nValid;

	char szPassword[32];
	char szCardID[32];
	int iFileType;                  //文件类型字段(默认为0);0-传图片,1-传特征值(人脸识别前抓前比功能使用)
	int iFaceFeatureValueLen;       //特征值数据长度(人脸识别前抓前比功能使用)
	char *pcFaceFeatureValue;       //特征值;iFileType=0时为null,iFileType=1时传特征值(人脸识别前抓前比功能使用)
	char acFaceGroupSyncVersion[32];//脸库同步版本号,随人脸,脸库的添加,更新,删除自增(人脸识别前抓前比功能使用)
    char acOwnner[64];              //安全码(人脸识别前抓前比功能使用)
    char acFaceGroupID[64];         //脸库ID(人脸识别前抓前比功能使用)
    char acFaceUUID[64];            //FaceUUID(人脸识别前抓前比功能使用)
	char acSystemTime[32];          //系统时间(仅人脸识别前抓前比功能使用)
    char acOrganizationID[64];      //组织ID(仅人脸识别前抓前比功能使用)
	
    char szReserved[148];
}HY_DVR_PERSON_V1_ENTERN;

typedef struct tagHY_DVR_ADD_PERSON_INFO_AND_IMAGE_INFO_V1
{
	int nValid;
	long long nGroupID;				//作为输入参数，表示人脸库ID
	int nIgnoreError;				//作为输入参数,是否忽略错误
	char strName[32];				//作为输入参数,姓名
	int nSex;						//作为输入参数,性别,0,male,1,female
	int nCertificateType;			//作为输入参数,身份证类型 0，passport,1,身份证，0xff，其他
	char strCertificateID[32];		//作为输入参数,ID
	char strCountry[32];			//作为输入参数,国家
	char strAddress[64];			//作为输入参数,地址
	HY_DVR_TIME struBirthday;		//作为输入参数,生日
	int nFaceImageID;				//作为输入参数，图片ID
	int  nPicLength;				//作为输入参数,图片的长度
	char * pstrPicBuffer;			//作为输入参数,图片的缓冲区,中间层必须得复制一份，协议层在传递完了之后，会释放内存
	char strUniqueID[32];			//作为输入参数
	long long nPersonID;			//作为输入参数
	char strPictureMd5[48];			//作为输入参数，图片的md5值
	char strOrganizationInfo[92];			//作为输入参数，组织信息
	char strNoteInfo[100];				//作为输入参数，备注信息
	char cPassPermissionEnable;						//作为输入参数，通行权限使能
	char szReserved[3];
	HY_DVR_PERSON_V1_ENTERN *pExtern;			//保留字段不够了，指向一个新的扩展结构体
}HY_DVR_ADD_PERSON_INFO_AND_IMAGE_INFO_V1;

typedef struct tagHY_DVR_ADD_PERSON_INFO_AND_IMAGE_INFO_V2
{
	int nValid;
	long nID;							//作为输入参数，表示人员ID，必填项
	char strName[32];					//作为输入参数,姓名，必填项
	char strSecretID[32];				//作为输入参数,脱敏证件号（为空表示无数据）（长度不足，废弃）
	char strEncryptID[32];				//作为输入参数,加密证件号（为空表示无数据）（长度不足，废弃）
	
	int iHasNucleicResult;				//是否有72小时内核酸结果，-1：无数据 0：没有 1：有。
	char acNucleicResult[16];			//核酸检测结果，如“阴性”。缺省为空
	int iInocTime;						//接种总次数,缺省值给-1（-1：未获取到疫苗信息；0：未接种；1-3：接种次数）
	unsigned long long ullInocDate;		//最后一次接种时间,时间戳(毫秒)
	unsigned long long ullDetectionTime;//最近一次核酸检测时间，0：无数据
	
	int  nPicLength;					//作为输入参数,图片的长度
	char * pstrPicBuffer;				//作为输入参数,图片的缓冲区,中间层必须得复制一份，协议层在传递完了之后，会释放内存

	char strSecretIDv1[64];				//作为输入参数,脱敏证件号（为空表示无数据）
	char strEncryptIDv1[64];			//作为输入参数,加密证件号（为空表示无数据）

	int iCodeStatus;					//健康码状态，-1：无数据 0：绿码 1：黄码 2：红码。
	
	char szReserved[892];
}HY_DVR_ADD_PERSON_INFO_AND_IMAGE_INFO_V2;

typedef struct tagHY_DVR_ONENET_PLATFORM_FACEID_INFO
{
    int iValid;

    unsigned long long ullFacePicId;              /*!< 人脸图片ID */ 
    char acMD5[128];                              /*!< 图片MD5值 */

    char strReserved[1024];
}HY_DVR_ONENET_PLATFORM_FACEID_INFO, *LPHY_DVR_ONENET_PLATFORM_FACEID_INFO;

typedef struct tagHY_DVR_ONENET_PLATFORM_FACE_INFO
{
    int iValid;

    unsigned long long ullFacePicId;                     /*!< 人脸图片ID */ 
	char acMD5[128];                              /*!< 图片MD5值 */
    int  iPicLength;                    /*!< 作为输入参数,图片的长度 */ 
    char * pstrPicBuffer;                /*!< 作为输入参数,图片的缓冲区,中间层必须得复制一份，协议层在传递完了之后，会释放内存 */ 

    char strReserved[1024];
}HY_DVR_ONENET_PLATFORM_FACE_INFO, *LPHY_DVR_ONENET_PLATFORM_FACE_INFO;

typedef struct tagHY_DVR_DEL_PERSON_INFO_V2
{
	int nValid;
	long nID;							//作为输入参数，表示人员ID，必填项	
	char strEncryptID[64];				//作为输入参数,加密证件号（为空表示无数据）（长度不足，废弃）
	char szReserved[960];
}HY_DVR_DEL_PERSON_INFO_V2;

//同步人员信息
typedef struct tagHY_DVR_SYNC_PERSON_PARAMETER
{
	int nValid;
	int	nTotalNumber;					//人员总数量
	char szReserved[1024];
}HY_DVR_SYNC_PERSON_PARAMETER;

#define  HY_DVR_MAX_DELETE_PERSON_INFO_COUNT  100
//删除人员信息
typedef struct tagHY_DVR_DELETE_PERSON_INFO
{
	int nValid;
	long long nGroupID;					//作为输入参数,脸库ID
	long long nPersonID;				//作为输入参数,人员ID
	int nDelPicture;					//作为输入参数，是否彻底删除图片0,不删除，1.删除
}HY_DVR_DELETE_PERSON_INFO;

typedef struct tagHY_DVR_DELETE_PERSON_INFO_LIST
{
	int nValid;
	HY_DVR_DELETE_PERSON_INFO struDeletePersonInfo[HY_DVR_MAX_DELETE_PERSON_INFO_COUNT];
}HY_DVR_DELETE_PERSON_INFO_LIST;

//删除人员信息
typedef struct tagHY_DVR_DELETE_PERSON_INFO_V1
{
	int nValid;
	long long nGroupID;					//作为输入参数,脸库ID
	long long nPersonID;				//作为输入参数,人员ID
	int nDelPicture;					//作为输入参数，因为现在一个人员可以存在于多个脸库里面，如果这个为1，则将此人员从所有脸库中删除，并移到未绑定脸库，如果为0，就仅仅将此人员从指定脸库删除(人员只存在于指定脸库的话就移到未绑定脸库)
	char strUniqueID[32];				//作为输入参数
	char cForceDel;						//作为输入参数，强制删除，true or false ,如果为true，表示删除的人员信息不进未绑定的脸库，否则表示会进入未绑定脸库
	long long llFaceGroupSyncVersion;//脸库同步版本号,随人脸,脸库的添加,更新,删除自增(人脸识别前抓前比功能使用)
    char acOwnner[64];              //安全码(人脸识别前抓前比功能使用)
    char acFaceGroupID[64];         //脸库ID(人脸识别前抓前比功能使用)
    char acFaceUUID[64];            //FaceUUID(人脸识别前抓前比功能使用)
	char acSystemTime[32];          //系统时间(仅人脸识别前抓前比功能使用)

	char szReserved[32 * 8 - 233];
}HY_DVR_DELETE_PERSON_INFO_V1;

typedef struct tagHY_DVR_DELETE_PERSON_INFO_LIST_V1
{
	int nValid;
	HY_DVR_DELETE_PERSON_INFO_V1 struDeletePersonInfoV1[HY_DVR_MAX_DELETE_PERSON_INFO_COUNT];
}HY_DVR_DELETE_PERSON_INFO_LIST_V1;


#define  HY_DVR_MAX_BIND_AND_UNBIND_PERSON_INFO_COUNT  100


//绑定和解绑人员信息
typedef struct tagHY_DVR_BIND_AND_UNBIND_PERSON_INFO
{
	int nValid;
	long long nSrcGroupID;				    //作为输入参数，当时解绑操作时，该值无意义，但是绑定操作时，该值表示源脸库ID	
	long long nDstGroupID;					//作为输入参数,目标脸库ID
	long long nPersonID;					//作为输入参数,人员ID
}HY_DVR_BIND_AND_UNBIND_PERSON_INFO;

typedef struct tagHY_DVR_BIND_AND_UNBIND_PERSON_INFO_LIST
{
	int nValid;
	HY_DVR_BIND_AND_UNBIND_PERSON_INFO struBindAndUnBindPersonInfo[HY_DVR_MAX_BIND_AND_UNBIND_PERSON_INFO_COUNT];
}HY_DVR_BIND_AND_UNBIND_PERSON_INFO_LIST;

//绑定和解绑人员信息
typedef struct tagHY_DVR_BIND_AND_UNBIND_PERSON_INFO_V1
{
	int nValid;
	long long nSrcGroupID;				    //作为输入参数，当时解绑操作时，该值无意义，但是绑定操作时，该值表示源脸库ID	
	long long nDstGroupID;					//作为输入参数,目标脸库ID
	long long nPersonID;					//作为输入参数,人员ID
	char strUniqueID[32];					//作为输入参数
	char szReserved[32 * 8];
}HY_DVR_BIND_AND_UNBIND_PERSON_INFO_V1;

typedef struct tagHY_DVR_BIND_AND_UNBIND_PERSON_INFO_LIST_V1
{
	int nValid;
	HY_DVR_BIND_AND_UNBIND_PERSON_INFO_V1 struBindAndUnBindPersonInfoV1[HY_DVR_MAX_BIND_AND_UNBIND_PERSON_INFO_COUNT];
}HY_DVR_BIND_AND_UNBIND_PERSON_INFO_LIST_V1;

//修改人员信息
typedef struct tagHY_DVR_MODIFY_PERSON_INFO
{
	int nValid;
	long long nPersonID;			//作为输入参数,人员ID
	char strName[32];				//作为输入参数,姓名
	int nSex;						//作为输入参数,性别,0,male,1,female
	int nCertificateType;			//作为输入参数,身份证类型 0，passport,1,身份证，0xff，其他
	char strCertificateID[32];		//作为输入参数,ID
	char strCountry[32];			//作为输入参数,国家
	char strAddress[64];			//作为输入参数,地址
	HY_DVR_TIME struBirthday;			//作为输入参数,生日
}HY_DVR_MODIFY_PERSON_INFO;

//修改人员信息
typedef struct tagHY_DVR_MODIFY_PERSON_INFO_V1
{
	int nValid;
	long long nPersonID;			//作为输入参数,人员ID
	char strName[32];				//作为输入参数,姓名
	int nSex;						//作为输入参数,性别,0,male,1,female
	int nCertificateType;			//作为输入参数,身份证类型 0，passport,1,身份证，0xff，其他
	char strCertificateID[32];		//作为输入参数,ID
	char strCountry[32];			//作为输入参数,国家
	char strAddress[64];			//作为输入参数,地址
	HY_DVR_TIME struBirthday;			//作为输入参数,生日
	char strUniqueID[32];		//作为输入参数
	char strOrganizationInfo[92];		//作为输入参数，组织结构信息
	char strNoteInfo[100];					//作为输入参数，备注信息
	char cPassPermissionEnable;							//作为输入参数，通行权限使能
	char reserved1[3];
	char strPassword[32];
	HY_DVR_PERSON_V1_ENTERN *pExtern;			//保留字段不够了，指向一个新的扩展结构体
	char strReserved[24];		//作为输入参数
}HY_DVR_MODIFY_PERSON_INFO_V1;

//修改人员图片信息
typedef struct tagHY_DVR_UPDATE_IMAGE_INFO
{
	int nValid;
	long long nPersonID;					//作为输入参数,人员ID
	int nPictureID;					//作为输入参数，图片，1,2,3
	int nPicLenght;					//作为输入参数,表示图片的实际长度
	char * pPicBuffer;				//作为输入参数,表示图片本身
}HY_DVR_UPDATE_IMAGE_INFO;

//修改人员图片信息
typedef struct tagHY_DVR_UPDATE_IMAGE_INFO_V1
{
	int nValid;
	long long nPersonID;					//作为输入参数,人员ID
	int nPictureID;					//作为输入参数，图片，1,2,3
	int nPicLenght;					//作为输入参数,表示图片的实际长度
	char * pPicBuffer;				//作为输入参数,表示图片本身
	char strUniqueID[32];				//作为输入参数
	char strPictureMd5[48];			//作为输入参数，图片的md5值
	char szReserved[32 * 8 - 48];
}HY_DVR_UPDATE_IMAGE_INFO_V1;

//修改人员信息
typedef struct tagHY_DVR_MODIFY_PERSON_INFO_AND_IMAGE_INFO
{
	int nValid;
	long long nPersonID;			//作为输入参数,人员ID
	char strName[32];				//作为输入参数,姓名
	int nSex;						//作为输入参数,性别,0,male,1,female
	int nCertificateType;			//作为输入参数,身份证类型 0，passport,1,身份证，0xff，其他
	char strCertificateID[32];		//作为输入参数,ID
	char strCountry[32];			//作为输入参数,国家
	char strAddress[64];			//作为输入参数,地址
	HY_DVR_TIME struBirthday;		//作为输入参数,生日
	int nFaceImageID;				//作为输入参数,图片ID，1,2,3
	int nPicLenght;					//作为输入参数,表示图片的实际长度
	char * pPicBuffer;				//作为输入参数,表示图片本身
}HY_DVR_MODIFY_PERSON_INFO_AND_IMAGE_INFO;

typedef struct tagHY_DVR_MODIFY_PERSON_INFO_AND_IMAGE_INFO_V1
{
	int nValid;
	long long nPersonID;			//作为输入参数,人员ID
	char strName[32];				//作为输入参数,姓名
	int nSex;						//作为输入参数,性别,0,male,1,female
	int nCertificateType;			//作为输入参数,身份证类型 0，passport,1,身份证，0xff，其他
	char strCertificateID[32];		//作为输入参数,ID
	char strCountry[32];			//作为输入参数,国家
	char strAddress[64];			//作为输入参数,地址
	HY_DVR_TIME struBirthday;		//作为输入参数,生日
	int nFaceImageID;				//作为输入参数,图片ID，1,2,3
	int nPicLenght;					//作为输入参数,表示图片的实际长度
	char * pPicBuffer;				//作为输入参数,表示图片本身
	char strUniqueID[32];				//作为输入参数
	char strPictureMd5[48];			//作为输入参数，图片的md5值
	char strOrganizationInfo[92];		//作为输入参数，组织结构信息
	char strNoteInfo[100];				//作为输入参数，备注信息
	char cPassPermissionEnable;							//作为输入参数，通行权限使能
	char reserved1[3];
	HY_DVR_PERSON_V1_ENTERN *pExtern;			//保留字段不够了，指向一个新的扩展结构体
	char szReserved[8];
}HY_DVR_MODIFY_PERSON_INFO_AND_IMAGE_INFO_V1;


//
typedef struct tagHY_DVR_PERSON_INFO_COUNT
{
	int nValid;
	long long nGroupID;					//作为输入参数,人脸库，-1，表示对所有
	char strName[32];				//作为输入参数,姓名
	int nSex;						//作为输入参数,性别,0,male,1,female
	int nCertificateType;			 //作为输入参数,身份证类型 0，passport,1,身份证，0xff，其他
	char strCertificateID[32];		//作为输入参数,ID
	char strCountry[32];			//作为输入参数,国家
	char strAddress[64];			//作为输入参数,地址
	HY_DVR_TIME struStartBirthday;	//作为输入参数
	HY_DVR_TIME struStopBirthday;   //作为输入参数
	int nPersonInfoCount;			//作为输出参数，人员信息个数
}HY_DVR_PERSON_INF_COUNT;

typedef struct tagHY_DVR_PERSON_INFO_COUNT_V1
{
	int nValid;
	long long nGroupID;					//作为输入参数,人脸库，-1，表示对所有
	char strName[32];				//作为输入参数,姓名
	int nSex;						//作为输入参数,性别,0,male,1,female
	int nCertificateType;			 //作为输入参数,身份证类型 0，passport,1,身份证，0xff，其他
	char strCertificateID[32];		//作为输入参数,ID
	char strCountry[32];			//作为输入参数,国家
	char strAddress[64];			//作为输入参数,地址
	HY_DVR_TIME struStartBirthday;	//作为输入参数
	HY_DVR_TIME struStopBirthday;   //作为输入参数
	int nPersonInfoCount;			//作为输出参数，人员信息个数
	int iModelState;				//作为输入参数,建模状态
	char acOrginationID[64];        //作为输入参数，组织ID(仅人脸识别前抓前比功能使用)

	char szReserved[192];			//保留字段
}HY_DVR_PERSON_INFO_COUNT_V1;


//数组的第一个成员作为输入参数,由于输出的人员信息是一个列表，所以正常逻辑是先获取人员信息个数，然后动态申请空间供中间层这边填充
typedef struct tagHY_DVR_PERSON_INFO
{
	int nValid;
	long long nGroupID;				//作为输入参数和输出参数
	long long nPersonID;			//作为输入参数和输出参数
	char strName[32];				//作为输入参数,姓名
	int nSex;						//作为输入参数,性别,0,male,1,female,0xff,unknown
	int nCertificateType;			//作为输入参数,身份证类型 0，身份证,1,passport，2,custom，0xff，未知
	char strCertificateID[32];		//作为输入参数,ID
	char strCountry[32];			//作为输入参数,国家
	char strAddress[64];			//作为输入参数,地址
	HY_DVR_TIME struBirthday;		//作为输入参数,生日
	int nFaceImageID;				//作为输出参数,值为1,2,3
	int nModelState;				//作为输出参数,模型状态,0,建模失败,1,建模成功
	char strUniqueID[32];		//作为输出参数
}HY_DVR_PERSON_INFO;

typedef struct tagHY_DVR_PERSON_INFO_LIST
{
	int nValid;
	long long nGroupID;						//作为输入参数,人脸库，-1，表示对所有
	char strName[32];					//作为输入参数,姓名
	int nSex;							//作为输入参数,性别,0,male,1,female
	int nCertificateType;		        //作为输入参数,身份证类型 0，passport,1,身份证，0xff，其他
	char strCertificateID[32];			//作为输入参数,ID
	char strCountry[32];				//作为输入参数,国家
	char strAddress[64];				//作为输入参数,地址
	HY_DVR_TIME struStartBirthday;		//作为输入参数
	HY_DVR_TIME struStopBirthday;		//作为输入参数
	int nPageSize;						//作为输入参数，表示每一页最大的记录条数,作为输出参数，表示实际的一页的记录个数
	int nPageNum;						//作为输入参数，表示第几页
	HY_DVR_PERSON_INFO * pPersonInfoList;   //人员信息列表
}HY_DVR_PERSON_INFO_LIST;

//数组的第一个成员作为输入参数,由于输出的人员信息是一个列表，所以正常逻辑是先获取人员信息个数，然后动态申请空间供中间层这边填充
typedef struct tagHY_DVR_PERSON_INFO_V1
{
	int nValid;
	long long nGroupID;				//作为输入参数和输出参数
	long long nPersonID;			//作为输入参数和输出参数
	char strName[32];				//作为输入参数,姓名
	int nSex;						//作为输入参数,性别,0,male,1,female,0xff,unknown
	int nCertificateType;			//作为输入参数,身份证类型 0，身份证,1,passport，2,custom，0xff，未知
	char strCertificateID[32];		//作为输入参数,ID
	char strCountry[32];			//作为输入参数,国家
	char strAddress[64];			//作为输入参数,地址
	HY_DVR_TIME struBirthday;		//作为输入参数,生日
	int nFaceImageID;				//作为输出参数,值为1,2,3
	int nModelState;				//作为输出参数,模型状态,0,建模失败,1,建模成功
	char strUniqueID[32];		//作为输出参数
	char strOrganizationInfo[92];		//作为输入参数，组织结构信息
	char strNoteInfo[100];				//作为输入参数，备注信息
	char cPassPermissionEnable;			//作为输入参数，通行权限使能
	char reserved1[3];
	char szPassword[32];
	char szCardID[32];			//作为输出参数，卡号
	char acValidPeriod[32];				//作为输出参数，有效期
	char acOrganizationID[64];   //作为输入参数，组织ID(仅人脸识别前抓前比功能使用)
    int iFileType;                  //文件类型字段(默认为0);0-传图片,1-传特征值(人脸识别前抓前比功能使用)
    int iFaceFeatureValueLen;       //特征值数据长度(人脸识别前抓前比功能使用)
	char *pcFaceFeatureValue;       //特征值;iFileType=0时为null,iFileType=1时传特征值(人脸识别前抓前比功能使用)
    char acFaceUUID[64];            //FaceUUID(人脸识别前抓前比功能使用)
    int nFaceVersion;               // 人脸版本号
    char acGroup[64];               // groupID

    char szReserved[204];
}HY_DVR_PERSON_INFO_V1;

typedef struct tagHY_DVR_PERSON_INFO_LIST_V1
{
	int nValid;
	long long nGroupID;						//作为输入参数,人脸库，-1，表示对所有
	char strName[32];					//作为输入参数,姓名
	int nSex;							//作为输入参数,性别,0,male,1,female
	int nCertificateType;		        //作为输入参数,身份证类型 0，passport,1,身份证，0xff，其他
	char strCertificateID[32];			//作为输入参数,ID
	char strCountry[32];				//作为输入参数,国家
	char strAddress[64];				//作为输入参数,地址
	HY_DVR_TIME struStartBirthday;		//作为输入参数
	HY_DVR_TIME struStopBirthday;		//作为输入参数
	int nPageSize;						//作为输入参数，表示每一页最大的记录条数,作为输出参数，表示实际的一页的记录个数
	int nPageNum;						//作为输入参数，表示第几页
	HY_DVR_PERSON_INFO_V1 * pPersonInfoListV1;   //人员信息列表
	char strOrganizationInfo[92];		//作为输入参数，组织结构信息
	char strNoteInfo[100];				//作为输入参数，备注信息
	int iModelState;					//作为输入参数，建模状态
	char acOrginationID[64];            //作为输入参数，组织ID(仅人脸识别前抓前比功能使用)

    char szReserved[444];
}HY_DVR_PERSON_INFO_LIST_V1;

typedef struct tagHY_DVR_DEVICE_IMAGE_PERSON_MAP_NUM
{
	int nValid;
	int nImagePersonMapNum;
	char reserved[1024];
}HY_DVR_DEVICE_IMAGE_PERSON_MAP_NUM;

typedef struct tagHY_DVR_IMAGE_PERSON_MAP_V1
{
	int nValid;
	int nItemIndex;
	char strPersonName[32];
	int nRemark;
	char strImageName[256];
	char reserved[1024];
} HY_DVR_IMAGE_PERSON_MAP_V1;

typedef struct tagHY_DVR_REMOVE_IMAGE_PERSON_MAP_V1
{
	int nValid;
	int nItemIndex;
	char reserved[1024];
}HY_DVR_REMOVE_IMAGE_PERSON_MAP_V1;


//数组的第一个成员作为输入参数,由于输出的人员信息是一个列表，所以正常逻辑是先获取人员信息个数，然后动态申请空间供中间层这边填充
typedef struct tagHY_DVR_IMAGE_INFO_PARAM
{
	int nValid;
	long long nPersonID;					//人员ID
	int nPictureID;					//图片ID
	int nPicLength;					//作为输入参数时，是图片数据最大长度，作为输出参数时，是图片数据的实际长度
	char * pstrPicBuffer;			//图片数据缓冲区
}HY_DVR_IMAGE_INFO_PARAM;

typedef struct tagHY_DVR_IMAGE_INFO_PARAM_V1
{
	int nValid;
	long long nPersonID;					//人员ID
	int nPictureID;					//图片ID
	int nPicLength;					//作为输入参数时，是图片数据最大长度，作为输出参数时，是图片数据的实际长度
	char * pstrPicBuffer;			//图片数据缓冲区
	char strUniqueID[32];
	char cImageFormat;				//0-JPG;1-PNG

	char szReserved[32 * 8 - 1];
}HY_DVR_IMAGE_INFO_PARAM_V1;

#define MAX_CHANNEL_RULE_COUNT  4

typedef struct tagHY_DVR_RULE_FACE_GROUP
{
	int nValid;
	int nEnable;
	long long nGroupID;
	char strGroupName[32];
}HY_DVR_RULE_FACE_GROUP;

typedef struct tagHY_DVR_RULE_FACE_GROUP_LIST
{
	int nValid;										//该字段控制该规则下支持的脸库，为1，表示支持该脸库，0，表示不支持该脸库，没有顺序
	HY_DVR_RULE_FACE_GROUP struRuleFaceGroup[HY_DVR_MAX_GROUP_FACE_COUNT];
}HY_DVR_RULE_FACE_GROUP_LIST;

#define HY_DVR_MAX_GROUP_FACE_COUNT_V1	100

// 布控人员类型
typedef enum
{
    eZWControlPersonnelType_None = 0,                    // 起始值
    eZWControlPersonnelType_Stranger,                    // 陌生人
    eZWControlPersonnelType_EveryOne,                    // 所有人
    eZWControlPersonnelType_OrganizationMember,                    // 组织架构成员
}eZWControlPersonnelType;

typedef struct tagHY_DVR_RULE_FACE_GROUP_V1
{
	int nValid;
	int nEnable;
	long long nGroupID;
	char strGroupName[32];

	char szReserved[64];
}HY_DVR_RULE_FACE_GROUP_V1;

typedef struct tagHY_DVR_RULE_FACE_GROUP_LIST_V1
{
	int nValid;										//该字段控制该规则下支持的脸库，为1，表示支持该脸库，0，表示不支持该脸库，没有顺序
	HY_DVR_RULE_FACE_GROUP_V1 *pStruRuleFaceGroupV1;		//大小：HY_DVR_MAX_GROUP_FACE_COUNT_V1
    eZWControlPersonnelType personnelType;                  // 布控人员类型                                与上面的字段互斥

	char szReserved[60];
}HY_DVR_RULE_FACE_GROUP_LIST_V1;


#define HY_DVR_RECO_THRESHOLD_SCHEDULE_COUNT 4

typedef struct tagHY_DVR_TIME_V2
{
	int nHour;																/*!< 时 */
	int nMinute;															/*!< 分 */
	int nSecond;															/*!< 秒 */

} HY_DVR_TIME_V2, *LPHY_DVR_TIME_V2;

typedef struct tagHY_DVR_SCHEDULE_RECO_THRESHOLD
{
	int nValid;
	HY_DVR_TIME_V2 struStartTime;
	HY_DVR_TIME_V2 struStopTime;
	int  nRecoThreshold;								//人脸识别阀值，0-100，默认为80
}HY_DVR_SCHEDULE_RECO_THRESHOLD;


typedef struct tagHY_DVR_SCHEDULE_RECO_THRESHOLD_LIST
{
	int nValid;
	HY_DVR_SCHEDULE_RECO_THRESHOLD struScheduleRecoThreshold[HY_DVR_RECO_THRESHOLD_SCHEDULE_COUNT];
}HY_DVR_SCHEDULE_RECO_THRESHOLD_LIST;

typedef struct tagHY_DVR_FACE_RECO_RULE
{
	int nValid;										//该字段一直为1，因为对于规则的支持，始终支持4个，是否启用，由nEnable控制
	int nEnable;									//是否启用
	int nRecognitionRule;							//人脸识别规则，0，表示比对通过，1，表示比对不通过
	int nCompareLimit;								//比对上限，0-5，默认为1
	HY_DVR_RULE_FACE_GROUP_LIST struFaceGroupList;  //规则关联的人脸库配置
	HY_DVR_HANDLEEXCEPTION struHandleType;			//处理方式
	HY_DVR_SCHEDTIME struSchedule;
	HY_DVR_SCHEDULE_RECO_THRESHOLD_LIST struScheduleRecoThresholdList;
}HY_DVR_FACE_RECO_RULE;

typedef struct tagHY_DVR_FACE_RECO_RULE_LIST
{
	int nValid;														//作为输入参数的时候，该值置为1
	int nChannelID;													//作为输入参数，表示视频通道，从0开始
	HY_DVR_FACE_RECO_RULE struFaceRecoRule[MAX_CHANNEL_RULE_COUNT];
}HY_DVR_FACE_RECO_RULE_LIST;

#define MAX_CHANNEL_RULE_COUNT_V1		8

typedef struct tagHY_DVR_FACE_RECO_RULE_V1
{
	int nValid;										//该字段一直为1，因为对于规则的支持，始终支持4个，是否启用，由nEnable控制
	int nEnable;									//是否启用
	int nRecognitionRule;							//人脸识别规则，0，表示比对通过，1，表示比对不通过
	int nCompareLimit;								//比对上限，0-5，默认为1
	char szReserved1[64];
	HY_DVR_RULE_FACE_GROUP_LIST_V1 struFaceGroupList;  //规则关联的人脸库配置
	HY_DVR_HANDLEEXCEPTION struHandleType;			//处理方式
	HY_DVR_SCHEDTIME struSchedule;
	HY_DVR_SCHEDULE_RECO_THRESHOLD_LIST struScheduleRecoThresholdList;
	char szReserved2[64];
}HY_DVR_FACE_RECO_RULE_V1;

typedef struct tagHY_DVR_FACE_RECO_RULE_LIST_V1
{
	int nValid;														//作为输入参数的时候，该值置为1
	int nChannelID;													//作为输入参数，表示视频通道，从0开始
	HY_DVR_FACE_RECO_RULE_V1 *pStruFaceRecoRuleV1;	//大小：MAX_CHANNEL_RULE_COUNT_V1

	char szReserved[128];
}HY_DVR_FACE_RECO_RULE_LIST_V1;

typedef struct tagHY_DVR_FACE_RECO_RULE_LIST_V1_VOICE_ALARM
{
    int iValid;

    int iEnable;          /*!< 使能,0:关闭,1:启用 */
    int iVoiceType;       /*!< 声音类型,0:自定义,1:警报,2:犬吠,3:警戒区域,4:监控区域,
                          5:欢迎光临,6:深水警告,7:私人领地,8:危险区域,9:贵重物品,
                          10:高处危险,11:私家价位,12:叮咚,13:有口罩,14:无口罩,
                          15:注意私人物品,16:抓小偷,17:救命,18:分类,19:可回收垃圾,
                          20:家庭食物垃圾21:危险废物,22:残余垃圾,23:全部分类,24:车形自定义,
                          25:禁止停车，26:人脸识别 */
    int iBroadcastCount;  /*!< 播报次数,[1-10] */
    int iBroadcastVolume; /*!< 播报音量,[0-100] */
    int iDayMode;/*!< 0:夜晚模式,1:白天模式,2:不启用此模式（AllDay字段有效，当字段不为disable时，AllDay字段无效）
                        当设置为此模式时，IPC依赖自身传感器判断当前环境是白天还是夜晚 */
    HY_DVR_SCHEDTIME stSchedTime;   /*!< 语音播报布防时间 */

    char szReserved[256];
} HY_DVR_FACE_RECO_RULE_LIST_V1_VOICE_ALARM, *LPHY_DVR_FACE_RECO_RULE_LIST_V1_VOICE_ALARM;


typedef struct tagHY_DVR_FACE_ReECO_EVENT_TIMESLOT_LIST
{
    int iValid;
    HY_DVR_SCHEDULE_TIMESLOT stSchedTimeSlot[SCHEDULE_TIMESLOT_MAX_NUM]; /*!< 布防时间计划 */
    char szReserved[1024];
}HY_DVR_FACE_RECO_EVENT_TIMESLOT_LIST,*LPHY_DVR_FACE_RECO_EVENT_TIMESLOT_LIST;


typedef struct tagHY_DVR_FACE_RECO_VOICEALARM_TIMESLOT_LIST
{
    int iValid;
    HY_DVR_SCHEDULE_TIMESLOT stSchedTimeSlot[SCHEDULE_TIMESLOT_MAX_NUM]; /*!< 布防时间计划 */
    char szReserved[1024];
}HY_DVR_FACE_RECO_VOICEALARM_TIMESLOT_LIST,*LPHY_DVR_FACE_RECO_VOICEALARM_TIMESLOT_LIST;


typedef struct tagHY_DVR_FACE_RECO_RECORD_COUNT
{
	int nValid;
	long long nGroupID;				//作为输入参数,脸库ID，-1表示搜索所有的人脸库
	char strName[32];				//作为输入参数,人员人名信息
	HY_DVR_TIME struStartTime;		//作为输入参数,开始时间
	HY_DVR_TIME struStopTime;		//作为输入参数,结束时间
	int nFaceRecoResultType;		//作为输入参数,0,Comparison pass，1，Comparison fail，2，Failure of biopsy，0xff,all
	int nRuleID;					//规则ID,从1开始
	int nFaceRecoRecordCount;		//作为输出参数，人脸识别记录个数
}HY_DVR_FACE_RECO_RECORD_COUNT;


typedef struct tagHY_DVR_FACE_RECO_RECORD
{
	int nValid;
	long long nLogID;
	long long nPersonID;
	char strGroupName[32];
	char strName[32];				//作为输出参数,人员人名信息
	HY_DVR_TIME struTime;
	int nIsPass;				//0,pass,1,no pass
	char strSnapshotPath[80];  //抓拍图片的路径
	float nSimilar;				//0-100
	int nRuleID;				//规则ID,从1开始
}HY_DVR_FACE_RECO_RECORD;

typedef struct tagHY_DVR_FACE_RECO_RECORD_LIST
{
	int nValid;
	long long nGroupID;								//作为输入参数,脸库ID，-1表示搜索所有的人脸库
	char strName[32];								//作为输入参数,人员人名信息
	HY_DVR_TIME struStartTime;						//作为输入参数,开始时间
	HY_DVR_TIME struStopTime;						//作为输入参数,结束时间
	int nFaceRecoResultType;						//作为输入参数,0,Comparison pass，1，Comparison fail，2，Failure of biopsy，0xff,all
	int nPagesize;									//作为输入参数,每一页的最大记录数，作为输出，表示每一页实际记录数
	int nPageNum;                                   //作为输入参数,第几页，从1开始
	int nIsHasPath;                                 //作为输入参数,0，表示没有路径，1，表示有路径
	int nRuleID;									//规则ID,从1开始
	HY_DVR_FACE_RECO_RECORD * pFaceRecoRecordList;	//作为输出参数
}HY_DVR_FACE_RECO_RECORD_LIST;

typedef struct tagHY_DVR_FACE_RECO_RECORD_EXTERN
{
	int nValid;

	int nHealthCodeStatus;		//0：不支持，1：绿码，2：黄码，3：红码
	char strCardID[32];				//身份证号
	char strName[32];				//姓名
	int iTrafficResults;					//通行结果,0-已通行,1-未通行
    char acNucleicData[128];				//核酸数据
    char acTravelData[192];					//行程数据

	char szReserved[188];
}HY_DVR_FACE_RECO_RECORD_EXTERN, LPHY_DVR_FACE_RECO_RECORD_EXTERN;

typedef struct tagHY_DVR_FACE_RECO_RECORD_V1
{
	int nValid;
	long long nLogID;
	long long nPersonID;
	char strGroupName[32];
	char strName[32];								//作为输出参数,人员人名信息
	HY_DVR_TIME struTime;
	int nIsPass;									//0,pass,1,no pass		复用定义：当识别规则为102密码时：0-密码正确；1-密码错误
	char strSnapshotPath[80];						//抓拍图片的路径
	float nSimilar;									//0-100
	int nRuleID;									//规则ID,1-白名单；2-黑名单；3-未注册；		拓展定义：100-刷卡有效；101-刷卡无效；102-密码
	float fFaceTemperature;							//作为输出参数，人脸温度
	char cTemperatureUnit;							//作为输出参数，人脸温度单位（0-不支持，1-摄氏度，2-华氏度）
	char cFaceMask;									//作为输出参数，口罩（0-不支持，1-有口罩，2-无口罩，3-未知）
	char cFever;									//作为输出参数，发热（0-不支持，1-正常，2-发热，3-未知）
	char cRecoValid;								//识别结果是否有效，0-不支持，1-有效，2-无效
	char strUniqueID[32];							//人员唯一编码
	long long nGroupID;
	HY_DVR_FACE_RECO_RECORD_EXTERN *pExtern;
	int iReserve[5];							//保留字段
}HY_DVR_FACE_RECO_RECORD_V1;

typedef struct tagHY_DVR_FACE_RECO_RECORD_LIST_V1
{
	int nValid;
	long long nGroupID;								//作为输入参数,脸库ID，-1表示搜索所有的人脸库
	char strName[32];								//作为输入参数,人员人名信息
	HY_DVR_TIME struStartTime;						//作为输入参数,开始时间
	HY_DVR_TIME struStopTime;						//作为输入参数,结束时间
	int nFaceRecoResultType;						//作为输入参数,0,Comparison pass，1，Comparison fail，2，Failure of biopsy，0xff,all
	int nPagesize;									//作为输入参数,每一页的最大记录数，作为输出，表示每一页实际记录数
	int nPageNum;                                   //作为输入参数,第几页，从1开始
	int nIsHasPath;                                 //作为输入参数,0，表示没有路径，1，表示有路径
	int nRuleID;									//规则ID,从1开始
	HY_DVR_FACE_RECO_RECORD_V1 * pFaceRecoRecordList;	//作为输出参数
}HY_DVR_FACE_RECO_RECORD_LIST_V1;

typedef struct tagHY_DVR_FACE_RECO_RECORD_V2
{
	int nValid;
	long long nLogID;
	long long nPersonID;
	char strGroupName[32];
	char strName[32];								//作为输出参数,人员人名信息
	HY_DVR_TIME struTime;
	int nIsPass;									//0,pass,1,no pass			复用定义：当识别规则为102密码时：0-密码正确；1-密码错误
	char strSnapshotPath[80];						//抓拍图片的路径
	float nSimilar;									//0-100
	int nRuleID;									//规则ID,1-白名单；2-黑名单；3-未注册		拓展定义：100-刷卡有效；101-刷卡无效；102-密码
	float fFaceTemperature;							//作为输出参数，人脸温度
	char cTemperatureUnit;							//作为输出参数，人脸温度单位（0-不支持，1-摄氏度，2-华氏度）
	char cFaceMask;									//作为输出参数，口罩（0-不支持，1-有口罩，2-无口罩，3-未知）
	char cFever;									//作为输出参数，发热（0-不支持，1-正常，2-发热，3-未知）
	char cRecoValid;								//识别结果是否有效，0-不支持，1-有效，2-无效
	char strUniqueID[32];							//人员唯一编码
	int nEventId;									//事件Id
	int iReserve[64];								//保留字段
}HY_DVR_FACE_RECO_RECORD_V2;

typedef struct tagHY_DVR_FACE_RECO_RECORD_LIST_V2
{
	int nValid;
	long long nGroupID;								//作为输入参数,脸库ID，-1表示搜索所有的人脸库
	char strName[32];								//作为输入参数,人员人名信息
	HY_DVR_TIME struStartTime;						//作为输入参数,开始时间
	HY_DVR_TIME struStopTime;						//作为输入参数,结束时间
	int nFaceRecoResultType;						//作为输入参数,0,Comparison pass，1，Comparison fail，2，Failure of biopsy，0xff,all
	int nPagesize;									//作为输入参数,每一页的最大记录数，作为输出，表示每一页实际记录数
	int nPageNum;                                   //作为输入参数,第几页，从1开始
	int nIsHasPath;                                 //作为输入参数,0，表示没有路径，1，表示有路径
	int nRuleID;									//规则ID,从1开始
	int nEventId;									//事件Id
	char cReserve[64];
	HY_DVR_FACE_RECO_RECORD_V2 * pFaceRecoRecordList;	//作为输出参数
}HY_DVR_FACE_RECO_RECORD_LIST_V2;

typedef struct tagHY_DVR_FACE_RECO_RECORD_DETAIL
{
	int nValid;
	long long nLogID;                               //作为输入参数,标记日志记录的ID
	int nIsPicture;									//作为输入参数,是否带图片，0，表示不带，1，表示带
	long long nPersonID;							//作为输出参数，标记日志记录的人员ID
	char strName[32];								//作为输出参数,姓名
	int nSex;										//作为输出参数,性别,0,male,1,female
	int nCertificateType;							//作为输出参数,身份证类型 0，passport,1,身份证，0xff，其他
	char strCertificateID[32];						//作为输出参数,ID
	char strCountry[32];							//作为输出参数,国家
	char strAddress[64];							//作为输出参数,地址
	HY_DVR_TIME struBirthday;							//作为输出参数,生日
	int nFaceRecoResult;							//作为输出参数，识别结果
}HY_DVR_FACE_RECO_RECORD_DETAIL;

typedef struct tagHY_DVR_FACE_RECO_RECORD_DETAIL_V1
{
	int nValid;
	long long nLogID;                               //作为输入参数,标记日志记录的ID
	int nIsPicture;									//作为输入参数,是否带图片，0，表示不带，1，表示带
	long long nPersonID;							//作为输出参数，标记日志记录的人员ID
	char strName[32];								//作为输出参数,姓名
	int nSex;										//作为输出参数,性别,0,male,1,female
	int nCertificateType;							//作为输出参数,身份证类型 0，passport,1,身份证，0xff，其他
	char strCertificateID[32];						//作为输出参数,ID
	char strCountry[32];							//作为输出参数,国家
	char strAddress[64];							//作为输出参数,地址
	HY_DVR_TIME struBirthday;						//作为输出参数,生日
	int nFaceRecoResult;							//作为输出参数，识别结果
	float fFaceTemperature;							//作为输出参数，人脸温度
	char cTemperatureUnit;							//作为输出参数，人脸温度单位（0-不支持，1-摄氏度，2-华氏度）
	char cFaceMask;									//作为输出参数，口罩（0-不支持，1-有口罩，2-无口罩，3-未知）
	char cFever;									//作为输出参数，发热（0-不支持，1-正常，2-发热，3-未知）
	char cRecoValid;								//识别结果是否有效，0-不支持，1-有效，2-无效
	char strUniqueID[32];							//人员唯一编码
	int iReserve[16 - 8];							//保留字段
}HY_DVR_FACE_RECO_RECORD_DETAIL_V1;

typedef struct tagHY_DVR_FACE_RECO_RECORD_SNAPSHOT
{
	int nValid;
	char strSnapshotPath[256];								//作为输入参数,抓图路径
	int nPicLength;											//作为输入参数，表示最大的buffer大小，作为输出参数，表示图片实际的大小
	char * pPicBuffer;								        //图片buffer
}HY_DVR_FACE_RECO_RECORD_SNAPSHOT;


typedef struct tagHY_DVR_FACE_RECO_RESULT_IMAGE
{
	int nValid;
	long long nLogID;												//作为输入参数,日志ID
	int nPicLength;											//作为输入参数，表示最大的buffer大小，作为输出参数，表示图片实际的大小
	char * pPicBuffer;								        //图片buffer
}HY_DVR_FACE_RECO_RESULT_IMAGE;


typedef struct tagHY_DVR_FACE_RECO_ABILITY
{
	int nValid;
	int nBufferLen;											//作为输入参数，表示可以支持的最大长度，作为输出参数，表示buffer存储的数据实际的大小
	char * pBuffer;											//存储数据的buffer
}HY_DVR_FACE_RECO_ABILITY;

typedef struct tagHY_DVR_DAY_SCHEDULE
{
	int nValid;

	int nEnable;
	char szSchedule[8][16];

	char szReserved[32];
}HY_DVR_DAY_SCHEDULE;

typedef struct tagHY_DVR_FACE_GROUP_PASS_PERMISSION
{
	int nValid;

	int nChannelID;
	long long nGroupID;			//作为输出参数,脸库ID
	int nPassPermissionEnable;		//通行权限			0：禁用，1：启用
	int nEffectiveTimeMode;			//有效时间模式		0：永久，1：指定时间段
	char szEffectiveTime[32];			//有效时间模式为1的时候时间段的值		yyyyMMdd-yyyyMMdd
	int nWeekScheduleMode;			//周计划模式		0：每周，1：单双周，2：全周
	HY_DVR_DAY_SCHEDULE struEveryWeekTime[7];			//每周计划的值		7天，每天8个时间段，每个时间段的值为hh:mm-hh:mm		0：星期天，1：星期一......
	HY_DVR_DAY_SCHEDULE struOddWeekTime[7];			//单数周计划的值		7天，每天8个时间段，每个时间段的值为hh:mm-hh:mm		0：星期天，1：星期一......
	HY_DVR_DAY_SCHEDULE struEvenWeekTime[7];			//双数周计划的值		7天，每天8个时间段，每个时间段的值为hh:mm-hh:mm		0：星期天，1：星期一......

	char szReserved[1024];
}HY_DVR_FACE_GROUP_PASS_PERMISSION, *LPHY_DVR_FACE_GROUP_PASS_PERMISSION;

typedef struct tagHY_DVR_FACE_GROUP_PERSON_PASS_PERMISSION
{
	int nValid;

	int nChannelID;
	long long nPersonID;
	char szUniqueID[32];
	int nPassMode;							//通行模式			0：继承所属脸库通行方式，1：自定义;2:通行次数(PassCount)
	int nPassPermissionEnable;		//通行权限			0：禁用，1：启用
	int nEffectiveTimeMode;			//有效时间模式		0：永久，1：指定时间段
	char szEffectiveTime[32];			//有效时间模式为1的时候时间段的值		yyyyMMdd-yyyyMMdd
	int nWeekScheduleMode;			//周计划模式		0：每周，1：单双周，2：全周
	HY_DVR_DAY_SCHEDULE struEveryWeekTime[7];			//每周计划的值		7天，每天8个时间段，每个时间段的值为hh:mm-hh:mm
	HY_DVR_DAY_SCHEDULE struOddWeekTime[7];			//单数周计划的值		7天，每天8个时间段，每个时间段的值为hh:mm-hh:mm
	HY_DVR_DAY_SCHEDULE struEvenWeekTime[7];			//双数周计划的值		7天，每天8个时间段，每个时间段的值为hh:mm-hh:mm
	int iPassCount;                  //人员通行次数,通行模式取值为2(PassCount/通行次数)时有效.有效时,默认值为4;默认选项值有1,2,4,8,自定义(1-100);

	char szReserved[1020];
}HY_DVR_FACE_GROUP_PERSON_PASS_PERMISSION, *LPHY_DVR_FACE_GROUP_PERSON_PASS_PERMISSION;


typedef struct tagHY_DVR_CONNECT_STATE
{
	int nValid;
	int	m_MaxRealStreamMain;						//最大实时主码流连接数量
	int m_RealStreamMain;							//实时主码流连接数量
	int	m_MaxRealStreamSub;							//最大实时子码流连接数量
	int m_RealStreamSub;							//实时子码流连接数量
	char m_Reserve[256];							//保留字段
}HY_DVR_CONNECT_STATE;

typedef struct tagHY_DVR_MODIFY_PASSWORD_BY_TEMP_SECRET_KEY
{
	int nValid;
	char strSuperUserName[32];
	char strSuperPassword[32];
	char strModifyUserName[32];
	char strModifyPassword[32];
}HY_DVR_MODIFY_PASSWORD_BY_TEMP_SECRET_KEY;

//云存储结构体
typedef struct tagHY_DVR_CLOUD_STORAGE
{
	int nValid;
	char strDeviceID[64];		//设备ID
	char strDomainName[256];		//域名
	char reserved[1024 - 256];
	//保留字段
	//-256：域名(strDomainName)
}HY_DVR_CLOUD_STORAGE;

//是否开启云存储
typedef struct tagHY_DVR_CLOUD_STROAGE_BASE_CONFIG
{
	int nValid;		//本结构体是否有效
	int nEnableStatus;		//开启/关闭云存储设备功能		1：开启		0：关闭
	int cloudStorageRegion; //云存区域  0:国内云存，1：国际云存				
	char reserved[1024 - 4];
}HY_DVR_CLOUD_STROAGE_BASE_CONFIG;

//云存储是否已经开通
typedef struct tagHY_DVR_CLOUD_STROAGE_OPEN_STATUS
{
	int nValid;					//本结构体是否有效
	int nSubscribeStatus;		//是否开通了云存储业务		1：开通		0：未开通
	char strDealerID[256];		//经销商ID
	char reserved[1024];
}HY_DVR_CLOUD_STROAGE_OPEN_STATUS;

//云存储录像计划
typedef struct tagHY_DVR_CLOUD_STROAGE_RECORD_PLAN
{
	int nValid;				//本结构体是否有效
	int nStreamMode;		//码流类型：0：主码流		1：子码流		3：第二子码流等
	int nRecordMode;		//0,手动录像，1，自动录像，2，关闭录像（该参数弃用）
	unsigned char recordPlan[DVR_MAX_DAYS][DVR_MAX_TIMESEGMENT];//录像计划  0,表示不录像，1表示定时录像，2，表示移动侦测录像，3，表报警输入，4.智能侦测事件
	int nChannel;	//通道号
	char reserved[1024 - 4];
}HY_DVR_CLOUD_STROAGE_RECORD_PLAN;

typedef struct tagHY_DVR_CLOUD_STROAGE_RECORD_PLAN_EVENT_TIMESLOT_LIST
{
    int iValid;
    HY_DVR_SCHEDULE_TIMESLOT stSchedTimeSlot[SCHEDULE_TIMESLOT_MAX_NUM]; /*!< 布防时间计划 */
    char szReserved[1024];
}HY_DVR_CLOUD_STROAGE_RECORD_PLAN_EVENT_TIMESLOT_LIST, *LPHY_DVR_CLOUD_STROAGE_RECORD_PLAN_EVENT_TIMESLOT_LIST;

//云存储状态
#pragma  pack (4)
typedef struct tagHY_DVR_CLOUD_STORAGE_BASE_STATUS
{
  int nValid;    //本结构体是否有效
  int nNetworkStatus;      //正常：0    异常：大于0（错误代码）1.网络超时，2.
  int nStorageStatus;      //空闲：0    存储中：1    存储异常：大于2（错误代码）
  int nErrorCode;        //该错误码由模块错误码基址+接口错误码基址+错误码组成
  long long lTime;
  int line;
  char fileName[128];
  char reserved[880];
}HY_DVR_CLOUD_STORAGE_BASE_STATUS;
#pragma  pack ()

//错误码
#pragma pack (4)
typedef struct tagHY_DVR_CLOUD_STORAGE_ERROR_CODE
{
 int nValid; //本结构体是否有效
 int nErrorCode; //该错误码由模块错误码基址+接口错误码基址+错误码组成
 long long lTime;
 int line;
 char fileName[128];
 char reserved[880];
}HY_DVR_CLOUD_STORAGE_ERROR_CODE;
#pragma pack ()

//多通道云存储状态
#pragma pack (4)
typedef struct tagHY_DVR_CLOUD_STORAGE_BASE_STATUS_V2
{
 int nValid; //本结构体是否有效
 int nNetworkStatus; //正常：0 异常：大于0（错误代码）1.网络超时，2.
 int nStorageStatus; //空闲：0 存储中：1 存储异常：大于2（错误代码）
 HY_DVR_CLOUD_STORAGE_ERROR_CODE baseStatus[DVR_MAX_VIDEOIN_NUM]; //从0开始每个通道的错误码
}HY_DVR_CLOUD_STORAGE_BASE_STATUS_V2;
#pragma pack ()


//云存储auth信息
#pragma pack (4)
typedef struct tagHY_DVR_CLOUD_STORAGE_AUTH_INFO
{
 int nValid; //本结构体是否有效
 char jsonAuthInfo[8*1024]; //Auth请求返回的json数据
 char reserved[1024];
}HY_DVR_CLOUD_STORAGE_AUTH_INFO;
#pragma pack ()

//云存储配置
#pragma pack (4)
typedef struct tagHY_DVR_CLOUD_STORAGE_LOW_POWER_CONFIG
{
 int nValid; //本结构体是否有效
 int  deviceMode; //当前设备模式，0：普通模式（常电），1:低功耗模式,2:AOV模式
 int  recordDuration; //录像时长(s)
 int isSingleChannelLogic; //特殊需求的时候再设置（比如双目设备走单通道逻辑）0：该参数无效，走正常逻辑（根据实际通道数执行），1：强制走单目逻辑
 int  prerecordedInterval;//预录时长，不设置默认6s，设置的话需要大于一个gop值，建议gop+2,这个预录值设置之后，实际预录时长取决I帧出现的位置，并不是固定预录时长
 char reserved[256 - 4];
}HY_DVR_CLOUD_STORAGE_LOW_POWER_CONFIG;
#pragma pack ()

//云存储执行状态
#pragma pack (4)
typedef struct tagHY_DVR_CLOUD_STORAGE_EXECUTION_STATE
{
 int nValid; //本结构体是否有效
 int  executionState; //当前执行状态，1:开始执行，2：执行结束
 int channel;
 char reserved[256 - 4];
}HY_DVR_CLOUD_STORAGE_EXECUTION_STATE;
#pragma pack ()

//4g错误码
typedef struct tagHY_4G_ERROR_CODE
{
 int nValid; //本结构体是否有效
 int  errorCode;
 long long lTime;
}HY_4G_ERROR_CODE;

//云存错误码，在HY_DVR_CLOUD_STORAGE_BASE_STATUS的基础上去掉保留字段，协议内部使用，中间层不使用
typedef struct tagHY_DVR_CLOUD_STORAGE_BASE_STATUS_V1
{
  int nValid;    //本结构体是否有效
  int nNetworkStatus;      //正常：0    异常：大于0（错误代码）1.网络超时，2.
  int nStorageStatus;      //空闲：0    存储中：1    存储异常：大于2（错误代码）
  int nErrorCode;        //该错误码由模块错误码基址+接口错误码基址+错误码组成
  long long lTime;
  int line;
  char fileName[128];
}HY_DVR_CLOUD_STORAGE_BASE_STATUS_V1,*LPHY_DVR_CLOUD_STORAGE_BASE_STATUS_V1;

#define MAX_ERROR_CODE_LIST 256 //错误码列表最大个数

//云存错误码列表，协议内部使用，将错误码由obs传给p6smessage
typedef struct tagHY_DVR_OBS_ERROR_CODE_LIST
{
	int nValid;
	int listCount; //当前list个数
	HY_DVR_CLOUD_STORAGE_BASE_STATUS_V1 errorCodeList[MAX_ERROR_CODE_LIST]; //错误码列表
}HY_DVR_OBS_ERROR_CODE_LIST,*LPHY_DVR_OBS_ERROR_CODE_LIST;

//4g错误码列表
typedef struct tagHY_4G_ERROR_CODE_LIST
{
 int nValid; //本结构体是否有效
 int listCount; //当前list个数
 HY_4G_ERROR_CODE errorCodeList[MAX_ERROR_CODE_LIST];
}HY_4G_ERROR_CODE_LIST;

typedef struct tagHY_DVR_CLOUD_STORAGE_ORDER_INFO
{
	int nValid;		//本结构体是否有效
	int nStatus;		//是否绑定，		0：未绑定		1:绑定		2：已过期
	char strOrderID[256];		//订单ID
	char reserved[1024];
}HY_DVR_CLOUD_STORAGE_ORDER_INFO;

typedef struct tagHY_DVR_CLOUD_STORAGE_PACKAGE_INFO
{
	int nValid;		//本结构体是否有效
	char strPackageName[256];		//套餐名称
	char strBeginTime[32];		//开始时间
	char strEndTime[32];			//结束时间
	int nCycleDays;		//最大存储时间(单位：天)
	int nRemainDays;		//有效期（单位：天）
	char reserved[1024];
}HY_DVR_CLOUD_STORAGE_PACKAGE_INFO;

typedef struct tagHY_DVR_CLOUD_STORAGE_BUSINESS_INFO
{
	int nValid;

	HY_DVR_CLOUD_STORAGE_ORDER_INFO struOrderInfo;
	HY_DVR_CLOUD_STORAGE_PACKAGE_INFO struPackageInfo;

	char reserved[1024];
}HY_DVR_CLOUD_STORAGE_BUSINESS_INFO;

typedef struct tagHY_DVR_CLOUD_STORAGE_MODE
{
	int nValid;
	int nMode; //0,国内，1，海外,2-未知模式

	char cReserve[256];
} HY_DVR_CLOUD_STORAGE_MODE, *LPHY_DVR_CLOUD_STORAGE_MODE;

//IPC与NVR连接时受控模式
typedef struct tagHY_DVR_ALARM_OUT_CONTROL_MODE
{
	int nValid;
	int nChannelID;
	int nControlMode;

	char reserved[32];
}HY_DVR_ALARM_OUT_CONTROL_MODE;

//补光模式
typedef struct tagHY_DVR_LIGHT_FILLING_MODE
{
	int nValid;
	int nChannelID;
	int nLightFillingMode;
	HY_DVR_TIME_V2 strDayStartTime;
	HY_DVR_TIME_V2 strDayEndTime;

	char reserved[32];
}HY_DVR_LIGHT_FILLING_MODE;

//人脸亮度
typedef struct tagHY_DVR_FACE_BRIGHTNESS
{
	int nValid;
	int nChannelID;
	int nFaceBrightness;

	char reserved[32];
}HY_DVR_FACE_BRIGHTNESS;

//灯光报警
typedef struct tagHY_DVR_ALARMACTION_LIGHT_WARNING
{
	int nValid;
	int nChannelID;
	int nLightWarningMode;
	int nTwinkleTime;

	char reserved[32];
}HY_DVR_ALARMACTION_LIGHT_WARNING;

//获取设备帧率信息
typedef struct tagHY_DVR_FRAME_RATE_RANGE
{
	int nValid;
	int nChannelID;
	int nStreamID;
	int nMinFrameRate;
	int nMaxFrameRate;

	char reserved[32];
}HY_DVR_FRAME_RATE_RANGE;

//日志
typedef struct tagHY_DVR_LOG_INFO
{
	char cCreateTime[64];  //日志创建的时间
	DWORD dwChannel;
	DWORD dwMainLogType;
	DWORD dwAssistLogType;
	char cDescription[256];
	char cReserve[32];
}HY_DVR_LOG_INFO, *LPHY_DVR_LOG_INFO;

//PTZ状态
typedef struct tagHY_DVR_PTZ_STATUS
{
	int nValid;

	int nChannelID;
	int nStatus;		//0：Running		1：ArriveBottomLimit		2:ArriveTopLimit       3:Stop
	char reserved[32];
}HY_DVR_PTZ_STATUS, *LPHY_DVR_PTZ_STATUS;
#define HY_DVR_PTZ_STATUS_SIZE sizeof(HY_DVR_PTZ_STATUS)



//消息通知MESSAGE_NOTIFY_ALL
typedef struct tagHY_DVR_MESSAGE_NOTIFY
{
	int nValid;
	int nMessageType;				//消息推送类型，0，订阅通知
	char cReserved[32];
}HY_DVR_MESSAGE_NOTIFY;

//获取默认人脸探测区域和最大最小人脸大小
typedef struct tagHY_DVR_DEFAULT_FACE_REGION_INFO
{
	int nValid;

	int nChannel;

	int nDetectRegionTopLeftX;
	int nDetectRegionTopLeftY;
	int nDetectRegionBottomRightX;
	int nDetectRegionBottomRightY;

	int nMaxFaceWidth;
	int nMaxFaceHeight;

	int nMinFaceWidth;
	int nMinFaceHeight;

	char reserved[32];
}HY_DVR_DEFAULT_FACE_REGION_INFO;

typedef struct tagHY_DVR_PEOPLE_DETECT_ABILITY
{
	int nValid;

	char *buffer;
	int nBufferLength;
}HY_DVR_PEOPLE_DETECT_ABILITY;

typedef struct tagHY_DVR_FACE_LIVENESS_STATUS
{
	int nValid;

	int nEnable;
	float score;

	char reserved[32];
}HY_DVR_FACE_LIVENESS_STATUS;

typedef struct tagHY_DVR_FACE_BASE_CONFIG
{
	int nValid;

	int nChannelID;								//通道标识
	int nEnableRecognition;						//识别功能开关，0-不支持，1-启用，2-禁用
	int nOpenDoorMode;							//开门模式，0-不支持，1-人脸识别，2-人脸识别+刷卡，3-人脸识别+密码，4-人脸识别或刷卡，5-人脸识别或密码，6-人脸识别或刷卡或密码，7-快速测温,8-数字哨兵
	int nOverlayHumanBox;						//叠加人形框，0-不支持，1-启用，2-禁用
	int nSenstive;								//灵敏度，0-不支持
	int iEnableForceOutCardNumber;				//使能强制输出卡号,0-去使能;1-使能
	int iSyncPictureOpt;						//同步拍照选项,0:关闭;1:一直拍照;2:开门时拍照;3:不开门时拍照
	int iOpenDoorDelay;                         //开门延时,单位ms

    char reserved[112];
}HY_DVR_FACE_BASE_CONFIG;

//系统性能状态
typedef struct tagHY_DVR_SYSTEM_PERFORMANCE_STATUS
{
	//通用参数
	int nValid;									//0,表示该协议不支持，1，表示该协议支持
	double m_CPUPercent;						//!<CPU利用率
	double m_MemoryPercent;						//!<内存利用率
	double m_StoragePercent;					//!<存储利用率
	DWORD m_Uptime;								//!<启动时间（秒）
//定制参数
	DWORD m_OpenDoorCount;						//!<开门次数
	DWORD m_dwBootCount;
	DWORD m_dwMemorySize;				//!<内存大小

	int cReserve[248];
}HY_DVR_SYSTEM_PERFORMANCE_STATUS, *PHY_DVR_SYSTEM_PERFORMANCE_STATUS;

#ifdef ENABLE_LONGSEE

typedef struct tagHY_DVR_LONGSEE_DEVICE_CONFIG
{
	int nValid;

	char szMacAddress[DVR_MAX_MAC_STR_LEN];
	char szSerialNum[DVR_MAX_MAC_STR_LEN];

	char reserved[32];
}HY_DVR_LONGSEE_DEVICE_CONFIG, *LPHY_DVR_LONGSEE_DEVICE_CONFIG;

#endif

#ifndef DISABLE_GATE

typedef struct tagHY_DVR_ACCESS_CARD_INFO
{
	int nValid;

	char szID[32];
	char szCardNumber[32];

	char reserved[64];
}HY_DVR_ACCESS_CARD_INFO, *LPHY_DVR_ACCESS_CARD_INFO;

typedef struct tagHY_DVR_ACCESS_PASSWORD_INFO
{
	int nValid;

	char szID[32];
	char szPassword[32];

	char reserved[64];
}HY_DVR_ACCESS_PASSWORD_INFO, *LPHY_DVR_ACCESS_PASSWORD_INFO;

#endif

#ifndef DISABLE_FACEMASK

typedef struct tagHY_DVR_MASK_DETECT_ALARM_CONFIG
{
	int nValid;

	int nAlarmConfig;		//0:不报警		1:有口罩报警		2:无口罩报警

	//以下三项系历史遗留问题，其实不需要用到
	int nValidRegionMode;		//模块区域参数是否有效
	int nRegionMode;		//模块区域参数
	HY_DVR_REGION struDetectRegion;

	char reserved[32];
}HY_DVR_MASK_DETECT_ALARM_CONFIG, *LPHY_DVR_MASK_DETECT_ALARM_CONFIG;

typedef struct tagHY_DVR_MASK_DETECT_ALARM_LINKAGE
{
	int nValid;

	int nVoiceAlert;
	int nAlarmOut1;
	int nEmailAlert;

	char szReserved[28];
}HY_DVR_MASK_DETECT_ALARM_LINKAGE, *LPHY_DVR_MASK_DETECT_ALARM_LINKAGE;

typedef struct tagHY_DVR_MASK_PASS_CONFIG
{
	int nValid;

	int nChannelID;
	int nPassType;

	char szReserved[32];
}HY_DVR_MASK_PASS_CONFIG, *LPHY_DVR_MASK_PASS_CONFIG;

#endif

//辅助接口枚举
typedef enum eAssistantInterfaceType
{
	eWhiteLight,		//白光灯
	eBrustLight,		//爆闪灯
	eInfraredLight,		//红外灯
	ePilotLight,		//指示灯
	eRedLight,		//红灯
	eGreenLight,		//绿灯

	eErrorAssistantInterfaceType,
}eAssistantInterfaceType;

//辅助功能接口
typedef struct tagHY_DVR_ASSISTANT_INTERFACE
{
	int nValid;
	int nChannelID;
	eAssistantInterfaceType nAssisstantID;
	int nAssistantInterfaceStatus;	//1：开启	0：关闭
	char reserved[32];
}HY_DVR_ASSISTANT_INTERFACE;

typedef struct tagHY_DVR_ALARM_PREVENTION
{
	int nValid;
	int nAlarmStatus;	//0:不布防	1:布防
	char reserved[32];
}HY_DVR_ALARM_PREVENTION;

typedef enum eSystemWorkMode
{
	eSystemTestMode,
	eSystemNormalMode,
}eSystemWorkMode;

typedef struct tagHY_DVR_SYSTEM_WORK_MODE
{
	int nValid;
	eSystemWorkMode nSystemWorkMode;
	char reserved[32];
}HY_DVR_SYSTEM_WORK_MODE;

typedef enum eTemperatureType
{
	eTemperature_Fahrenheit,		//华氏度
	eTemperature_Centigrade,	//摄氏度

	eTemperature_Unknown,
}eTemperatureType;

//人脸体温探测配置
typedef struct tagHY_DVR_FACE_TEMPERATURE_DETECT_CONFIG
{
	int nValid;

	int nChannelID;		//通道ID
	int nEnable;		//是否开启体温检测
	eTemperatureType temperatureType;	//体温类型 
	float normalTemperatureDownLimit;	//正常体温下限
	float normalTemperatureUpLimit;	//正常体温上限
	int nTemperatureReviseEnable;		//温度修正开启关闭		0：关闭	1：启用手动补偿,2:启用户外自动补偿
	float temperatureReviseValue;	//温度值修正（-2.0-2.0）	
	int nHighTemperatureNoPassEnable;		//0:禁用，1：启用
	int nWorkMode;		//0：室内	1：室外

	char reserved[24];
}HY_DVR_FACE_TEMPERATURE_DETECT_CONFIG, *LPHY_DVR_FACE_TEMPERATURE_DETECT_CONFIG;

//人脸温度信息是否叠加到OSD上配置
typedef struct tagHY_DVR_OVERLAY_FACE_TEMPERATRUE_ON_OSD
{
	int nValid;

	int nChannelID;		//通道ID
	int nOverlay;		//是否叠加温度信息在OSD上
	char reserved[32];
}HY_DVR_OVERLAY_FACE_TEMPERATRUE_ON_OSD, *LPDVR_OVERLAY_FACE_TEMPERATRUE_ON_OSD;

//获取当前人脸温度
typedef struct tagHY_DVR_FACE_TEMPERATURE_STATUS
{
	int nValid;

	int nChannelID;		//通道ID
	float nFaceTemperature;
	char reserved[32];
}HY_DVR_FACE_TEMPERATURE_STATUS, *LPHY_DVR_FACE_TEMPERATURE_STATUS;

typedef struct tagHY_FACE_TEMPERATURE_ABNORMAL_ALARM_CONFIG
{
	int nValid;

	int nChannelID;
	int nVoiceAlert;
	int nDisableWhiteListLinkage;
	int nPushToPhone;
	int nAlarmOut1;
	int nAlarmOut2;
	int nEmail;
	char reserved[28];
}HY_FACE_TEMPERATURE_ABNORMAL_ALARM_CONFIG, *LPHY_FACE_TEMPERATURE_ABNORMAL_ALARM_CONFIG;


typedef struct tagHY_DEVICE_BIND_CONFIG
{
	int nValid;

	int nSCodeValid;
	char strSCode[64];

	char reserved[32];
}HY_DEVICE_BIND_CONFIG, *LPHY_DEVICE_BIND_CONFIG;

//删除定制语音
typedef struct tagHY_DELETE_VOICE_CUSTOM
{
	int nValid;

	char strParameter[64];

	char cReserved[32];
}HY_DELETE_VOICE_CUSTOM, *LPHY_DELETE_VOICE_CUSTOM;

typedef struct tagHY_FIXED_REGION_DETECT
{
	int nValid;

	int nValidRegionMode;		//模块区域参数是否有效
	int nRegionMode;		//模块区域参数
	HY_DVR_REGION struDetectRegion;	//区域设置

	char cReserved[32];
}HY_FIXED_REGION_DETECT, *LPHY_FIXED_REGION_DETECT;

typedef struct tagHY_DVR_PTZ_LIMIT_POS
{
	int nValid;

	int nPosX;
	int nPosY;

	char cReserved[32];
}HY_DVR_PTZ_LIMIT_POS, *LPHY_DVR_PTZ_LIMIT_POS;

typedef struct tagHY_DVR_SYSTEM_LANGUAGE_CONFIG
{
	int nValid;

	eSystemLanguage nLanguage;

	char cReserved[32];
}HY_DVR_SYSTEM_LANGUAGE_CONFIG, *LPHY_DVR_SYSTEM_LANGUAGE_CONFIG;

//烽火：摄像头基本信息结构体
typedef struct tagHY_FIBERHOME_BASE_INFO
{
	int nValid;

	char szManufactureID[32];		//厂商信息
	char szDevMod[32];					//摄像头型号
	char szDetailID[32];					//详细编号		格式：固定编号(7位)-自定义编号(2位) eg:13001SN-01

	char reserved[32 * 8];
}HY_FIBERHOME_BASE_INFO, *LPHY_FIBERHOME_BASE_INFO;

//烽火：摄像头安装位置信息结构体
typedef struct tagHY_FIBERHOME_LOCATION_INFO
{
	int nValid;

	char szSiteName[256];
	char szSiteAddr[256];
	char szDevName[256];
	char szDevAddr[256];

	char reserved[256 * 8];
}HY_FIBERHOME_LOCATION_INFO, *LPHY_FIBERHOME_LOCATION_INFO;

//烽火：中心控制服务器地址
typedef struct tagHY_FIBERHOME_CTRL_CHNL_SERVER
{
	int nValid;

	char szCtrlChnlServer[64];

	char reserved[64 * 8];
}HY_FIBERHOME_CTRL_CHNL_SERVER, *LPHY_FIBERHOME_CTRL_CHNL_SERVER;

//烽火：所在场所编码信息
typedef struct tagHY_FIBERHOME_LOCATION_ID
{
	int nValid;

	char szLocationID[32];

	char reserved[32 * 8];
}HY_FIBERHOME_LOCATION_ID, *LPHY_FIBERHOME_LOCATION_ID;

//烽火：数据中心服务器信息
typedef struct tagHY_FIBERHOME_DATA_CENTER_SERVER
{
	int nValid;

	char szDataCenterServer[64];

	char reserved[64 * 8];
}HY_FIBERHOME_DATA_CENTER_SERVER, *LPHY_FIBERHOME_DATA_CENTER_SERVER;

//最大的MQTT ID的长度
#define HY_DVR_MAX_MQTT_ID_LENGHT				64
//最大的主topic的长度
#define HY_DVR_MAX_MQTT_MAIN_TOPIC_LENGTH		64
//最大的topic的长度
#define HY_DVR_MAX_MQTT_TOPIC_LENGTH			128
//
typedef struct tagHY_DVR_MQTT_SERVER_CONFIG
{
	int nValid;
	int nEnable;											/* 是否启用mqtt服务器，0，不启用，1，启用			*/
	char struDevMqttID[HY_DVR_MAX_MQTT_ID_LENGHT];			/* Mqtt协议传输中，设备的唯一标记					*/
	char strMqttServerAddress[DVR_MAX_DOMAIN_STR_LEN];		/* Mqtt服务器地址，可以是IP，也可以是域名			*/
	int nMqttServerPort;									/* Mqtt协议服务端口								  */
	int nDirection;											/* 出入口方向，0.无方向，1.进入，2，退出			*/
	char strMqttServerUserName[DVR_MAX_USERNAME_LEN];		/* Mqtt服务器用户名								  */
	char strMqttServerPassword[DVR_MAX_PASSWORD_LEN];		/* Mqtt服务器密码  								  */
	int  nConnectServerStatus;
	/* 连接服务器状态，0，初始化状态 1.成功，2，用户名或密码错误，3，ID冲突，4.网络异常，0xff.其他未知错误*/
	int nEventSendTimeOut;									/* 事件发送超时时间  								  */
	char nIsForceSetValid;									/* 设置是否强制生效 ，0，正常判断逻辑，1强制设置 								  */
	char cReserve1[64 - 4 - 1];										/* 保留字段	  								     */
	char strMqttMainTopic[HY_DVR_MAX_MQTT_TOPIC_LENGTH];	/* 主的topic，可以用来获取和设置  		 		  */
	char strMqttOperateDeviceTopic[HY_DVR_MAX_MQTT_TOPIC_LENGTH];/* topic，只读，用来显示  					  */
	char strMqttPublishEventTopic[HY_DVR_MAX_MQTT_TOPIC_LENGTH];/* topic，只读，用来显示  		     		  */
	char cReserve2[HY_DVR_MAX_MQTT_TOPIC_LENGTH * 4];       	/* 保留字段  		     		  */
}HY_DVR_MQTT_SERVER_CONFIG, *LPHY_DVR_MQTT_SERVER_CONFIG;

typedef struct tagHY_DVR_CUIOT_DEVICE_AUTH_INFO_CFG
{
	int iValid;
	char acProductKey[64];						/*!< 产品Key   */
	char acDeviceKey[64];						/*!< 设备Key   */
	char acDeviceSecret[64];					/*!< 设备秘钥   */

	char szReserved[512];
}HY_DVR_CUIOT_DEVICE_AUTH_INFO_CFG, *LPHY_DVR_CUIOT_DEVICE_AUTH_INFO_CFG;

typedef struct tagHY_DVR_CUIOTV_SERVER_CONFIG_INFO
{
	int iValid;
	int iEnable;							/*!< 是否使能   */
	char acMainServerAddress[128];          /*!< 主服务器地址或域名   */
	char acFileServerAddress[128];          /*!< 文件服务器地址或域名   */
	char acUpdateServerAddress[128];       	/*!< 升级服务器地址或域名   */
	char acMainServerUser[32];       		/*!< 主服务器登录名   */
	char acMainServerPassword[32];       	/*!< 主服务器登录密码   */
	char acFileServerUser[32];       		/*!< 文件服务器登录名   */
	char acFileServerPassword[32];       	/*!< 文件服务器登录密码   */
	char acUpdateServerUser[32];       		/*!< 升级服务器登录名   */
	char acUpdateServerPassword[32];        /*!< 升级服务器登录密码   */
	int iMainServerPort;					/*!< 主服务器端口   */
	int iFileServerPort;					/*!< 文件服务器端口   */
	int iUpdateServerPort;					/*!< 升级服务器端口   */
	int iServerConnectStatus;            	/*!<连接服务器状态:0.初始化状态;1.成功;2.用户名或密码错误;3.ID冲突;4.网络异常;0xff.其他未知错误*/

	char szReserved[1024];
}HY_DVR_CUIOTV_SERVER_CONFIG_INFO, *LPHY_DVR_CUIOTV_SERVER_CONFIG_INFO;

typedef struct tagHY_DVR_EVENT_RECORD_UPLOAD_CUSTOM_CFG
{
	int iValid;
    int iStrangerUpload;									/*!< 是否陌生人上传,1-上传,0-不上传   */
    int iIdentifyRecordPicture;								/*!< 是否识别记录带图片,1-上传,0-不上传   */
    int iIDCardUpload;										/*!< 是否身份证上传,1-上传,0-不上传   */
    int iCardNumberUpload;									/*!< 是否卡号上传,1-上传,0-不上传   */
    
	char szReserved[256];
}HY_DVR_EVENT_RECORD_UPLOAD_CUSTOM_CFG, *LPHY_DVR_EVENT_RECORD_UPLOAD_CUSTOM_CFG;

typedef enum
{
	eHistoryEventUploadType_No,								/*!< 不上传   */
	eHistoryEventUploadType_Now,							/*!< 立即上传   */
	eHistoryEventUploadType_Stagger,						/*!< 错峰上传   */
}eHistoryEventUploadType;

typedef struct tagHY_DVR_HISTORY_EVENT_RECORD_TIMESLOT_CFG
{
	int iValid;
	eHistoryEventUploadType eUploadType;					/*!< 历史事件记录上传方式;不上传,立即上传,错峰上传,选择错峰上传时自动显示开始时间和结束时间选项，且支持跨天设置，例如 23:00-02:00 代表从晚上的23点到次日的凌晨2点可上传   */
	HY_DVR_TIME_V2 stStart;									/*!< 错峰上传方式下该字段有效   */
	HY_DVR_TIME_V2 stEnd;									/*!< 错峰上传方式下该字段有效   */

	char szReserved[256];
}HY_DVR_HISTORY_EVENT_RECORD_TIMESLOT_CFG, *LPHY_DVR_HISTORY_EVENT_RECORD_TIMESLOT_CFG;

typedef struct tagHY_DVR_EVENT_INTERVAL_AND_OTHER_SETTINGS_CFG
{
	int iValid;
    int iMQTTHeartBeatInterval;								/*!< MQTT协议心跳时间间隔，10s-300s   */
    int iIdentifyEventsRespReply;							/*!< 识别事件上报是否需要应答,1-应答,0-不应答   */
    int iSameEventUploadInterval;							/*!< 同一事件上传间隔，识别事件上报应答模式下该字段才有效,10s-1800s   */
    int iDifferentEventUploadInterval;						/*!< 不同事件上传间隔，1s-300s   */
    
	char szReserved[256];
}HY_DVR_EVENT_INTERVAL_AND_OTHER_SETTINGS_CFG, *LPHY_DVR_EVENT_INTERVAL_AND_OTHER_SETTINGS_CFG;

typedef struct tagHY_DVR_FACE_RECOGNITION_INTERVAL_CFG
{
	int iValid;
	int iFaceRecognitionInterval;							/*!< 人脸识别间隔，2s-15s   */

	char szReserved[128];
}HY_DVR_FACE_RECOGNITION_INTERVAL_CFG, *LPHY_DVR_FACE_RECOGNITION_INTERVAL_CFG;

typedef struct tagHY_DVR_CUIOT_EVENT_RECORD_UPLOAD_CUSTOM_CFG
{
	int iValid;
    int iStrangerUpload;									/*!< 是否陌生人上传   */
    int iIdentifyRecordPicture;								/*!< 是否识别记录带图片   */
    int iIDCardUpload;										/*!< 是否身份证上传   */
    int iCardNumberUpload;									/*!< 是否卡号上传   */
    
	char szReserved[256];
}HY_DVR_CUIOT_EVENT_RECORD_UPLOAD_CUSTOM_CFG, *LPHY_DVR_CUIOT_EVENT_RECORD_UPLOAD_CUSTOM_CFG;

typedef struct tagHY_DVR_CUIOT_HISTORY_EVENT_RECORD_TIMESLOT_CFG
{   
    int iValid;
    eHistoryEventUploadType eUploadType;					/*!< 历史事件记录上传方式;不上传,立即上传,错峰上传,选择错峰上传时自动显示开始时间和结束时间选项，且支持跨天设置，例如 23:00-02:00 代表从晚上的23点到次日的凌晨2点可上传   */
    HY_DVR_TIME_V2 stStart;									/*!< 错峰上传方式下该字段有效   */
    HY_DVR_TIME_V2 stEnd;									/*!< 错峰上传方式下该字段有效   */
    
	char szReserved[256];
}HY_DVR_CUIOT_HISTORY_EVENT_RECORD_TIMESLOT_CFG, *LPHY_DVR_CUIOT_HISTORY_EVENT_RECORD_TIMESLOT_CFG;

typedef struct tagHY_DVR_CUIOT_EVENT_INTERVAL_AND_OTHER_SETTINGS_CFG
{   
    int iValid;
    int iCUIOTHeartBeatInterval;							/*!< CUIOT协议心跳时间间隔，10s-300s   */
    int iCUIOTEventTimeOut;									/*!< CUIOT事件发送超时时间，10s-300s   */
    int iIdentifyEventsRespReply;							/*!< 识别事件上报是否需要应答,应答/不应答   */
    int iSameEventUploadInterval;							/*!< 同一事件上传间隔，识别事件上报应答模式下该字段才有效,10s-1800s   */
    int iDifferentEventUploadInterval;						/*!< 不同事件上传间隔，1s-300s   */
    
	char szReserved[256];
}HY_DVR_CUIOT_EVENT_INTERVAL_AND_OTHER_SETTINGS_CFG, *LPHY_DVR_CUIOT_EVENT_INTERVAL_AND_OTHER_SETTINGS_CFG;

typedef struct tagHY_DVR_QR_CODE_CFG_INFO
{
	int iValid;
    int iQRCodeIdentySwitch;					//二维码识别开关,1-开,0-关
	int iQRCodeUnrepeatInterval;				//二维码去重间隔时间,2~30秒，默认10秒

	char szReserved[1024];
}HY_DVR_QR_CODE_CFG_INFO;

typedef struct tagHY_DVR_REMOTE_OPEN_DOOR_CFG_INFO
{
	int iValid;
    int iRemoteDoorOpenSwitch;					//远程开门开关,0-禁用;1-启用
    int iAssociateQRCode;						//是否关联二维码,0-否;1-是
    int iAssociatedVisitorBodyTemperature;		//是否关联访客体温,0-否;1-是
    
	char reserved[512];
}HY_DVR_REMOTE_OPEN_DOOR_CFG_INFO;

typedef struct tagHY_DVR_PHOTO_SENSITIVE_TEST_CFG
{
	int iValid;

	int iLightSensitiveStatus;					//光敏状态(1-亮,0-暗)

	char szReserved[256];
}HY_DVR_PHOTO_SENSITIVE_TEST_CFG, LPHY_DVR_PHOTO_SENSITIVE_TEST_CFG;

typedef struct tagHY_DVR_USB_EXPORT_ATTEND_RECORD_RESULT_CFG
{
	int iValid;
	int iUSBExportAttendRecordResult;				//USB导出考勤记录结果,0-成功,1-失败

	char acReserved[256];
}HY_DVR_USB_EXPORT_ATTEND_RECORD_RESULT_CFG, *LPHY_DVR_USB_EXPORT_ATTEND_RECORD_RESULT_CFG;

typedef struct tagHY_DVR_REMOTE_UPDATE_RESULT_INFO
{
	int iValid;
    int iRemoteUpdateResult;					//远程升级检测结果,0-成功,1-当前已是最新版本,2-无可用升级包

	char szReserved[256];
}HY_DVR_REMOTE_UPDATE_RESULT_INFO, *LPHY_DVR_REMOTE_UPDATE_RESULT_INFO;

typedef struct tagHY_FIBERHOME_AREA_CODE
{
	int nValid;

	char szAreaCode[128];

	char reserved[128 * 8];
}HY_FIBERHOME_AREA_CODE, *LPHY_FIBERHOME_AREA_CODE;

typedef struct tagHY_FIBERHOME_LOG_PATH
{
	int nValid;

	char szLogPath[256];

	char reserved[256 * 8];
}HY_FIBERHOME_LOG_PATH, *LPHY_FIBERHOME_LOG_PATH;

typedef struct tagHY_DVR_FIBERHOME_LONLAT_INFO
{
	int nValid;

	double dLon;
	double dLat;

	char reserved[32];
}HY_DVR_FIBERHOME_LONLAT_INFO, *LPHY_DVR_FIBERHOME_LONLAT_INFO;

typedef struct tagHY_DVR_VIDEO_SERVER_URL
{
	int nValid;

	char szVideoServerURL[64];

	char szReserved[64 * 8];
}HY_DVR_VIDEO_SERVER_URL, *LPHY_DVR_VIDEO_SERVER_URL;

typedef struct tagHY_DVR_ALARMOUT_GATE
{
	int nValid;

	int nGateOutState;

	char reserved[32];
}HY_DVR_ALARMOUT_GATE, *LPHY_DVR_ALARMOUT_GATE;

typedef struct tagHY_DVR_ALARMOUT_GATE_LIST
{
	int nValid;

	HY_DVR_ALARMOUT_GATE struAlarmOutGate[DVR_MAX_ALARMOUT_NUM];

	DWORD szReserved[32];
}HY_DVR_ALARMOUT_GATE_LIST, *LPHY_DVR_ALARMOUT_GATE_LIST;

typedef struct tagHY_DVR_SECURITY_VALIDATE_IP
{
	int nValid;

	DWORD dwIPAddress;		//网络字节序IP地址
	int nIsAllowAccess;			//0：不支持白名单		1：允许访问		2：不允许访问

	char szReserved[32];
}HY_DVR_SECURITY_VALIDATE_IP, *LPHY_DVR_SECURITY_VALIDATE_IP;

typedef struct tagHY_DVR_AI_WORK_MODE
{
	int nValid;

	int nWorkMode;	//0：人脸模式		1：人形模式

	char szReserved[32];
}HY_DVR_AI_WORK_MODE, *LPHY_DVR_AI_WORK_MODE;

typedef struct tagHY_DVR_ALARM_OVERLAY_INFO
{
	int nValid;

	int nMainStreamOverlay;
	int nSubStreamOverlay;

	char cMainStreamPeopleOverlay;
	char cMainStreamFaceOverlay;
	char cMainStreamAreaOverlay;
	char cMainStreamAssisstantLineOverlay;
	char cSubStreamPeopleOverlay;
	char cSubStreamFaceOverlay;
	char cSubStreamAreaOverlay;
	char cSubStreamAssisstantLineOverlay;

	char cMainStreamCarOverlay;
	char cSubStreamCarOverlay;

	char cMainStreamPFStatisticsOverlay;
	char cSubStreamPFStatisticsOverlay;
	char cMainStreamFireOverlay;
    char cSubStreamFireOverlay;

	char szReserved[18];
}HY_DVR_ALARM_OVERLAY_INFO, *LPHY_DVR_ALARM_OVERLAY_INFO;

typedef struct tagHY_DVR_HTTP_EVENT_SERVER_CFG
{
	int nValid;								/*!< 本结构体是否有效，0表示无效 */
	int nEnable;							//0，不使能，1，使能
	int nProtocol;							//0，表示http协议，1.https（暂不支持）
	char strHost[256];						//服务器的地址，可以是IP地址，也可以是域名
	char strPath[256];						//服务的URL路径
	int nPort;								//端口，默认为80
	char strUserName[32];					//服务器的用户名
	char strPassword[32];					//服务器的密码
	int nDataTransfer2ServerTimeout;		//和服务器交互超时时间,单位是秒
}HY_DVR_HTTP_EVENT_SERVER_CFG, *LPHY_DVR_HTTP_EVENT_SERVER_CFG;

typedef struct tagHY_DVR_HTTP_EVENT_SERVER_STATUS
{
	int nValid;								/*!< 本结构体是否有效，0表示无效 */
	int nStatus;							/*!< 在线状态，0，不在线，1，在线 */
}HY_DVR_HTTP_EVENT_SERVER_STATUS, *LPHY_DVR_HTTP_EVENT_SERVER_STATUS;

/*!
*  @struct    tagHY_DVR_HTTP_EVENT_SERVER_CFG_V2
*  @brief    HTTP事件服务器信息V2
*  @illustrate
*/
typedef struct tagHY_DVR_HTTP_EVENT_SERVER_CFG_V2
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
	char cCacheEventEnable;                 /*!< 是否缓存事件开关 ，0-否，1-是 */

    char acReserved[255];
}HY_DVR_HTTP_EVENT_SERVER_CFG_V2, *LPHY_DVR_HTTP_EVENT_SERVER_CFG_V2;

/*!
*  @struct    tagHY_DVR_HTTP_EVENT_SERVER_STATUS_V2
*  @brief    HTTP事件服务器状态V2
*  @illustrate
*/
typedef struct tagHY_DVR_HTTP_EVENT_SERVER_STATUS_V2
{
    int nValid;                             /*!< 本结构体是否有效，0表示无效 */

    int nStatus;                            /*!< 在线状态，0，不在线，1，在线 */

    char acReserved[256];
}HY_DVR_HTTP_EVENT_SERVER_STATUS_V2, *LPHY_DVR_HTTP_EVENT_SERVER_STATUS_V2;

/*!
*  @struct    tagHY_DVR_HTTP_EVENT_SERVER_STATUS_TEST
*  @brief    HTTP事件服务器状态测试
*  @illustrate
*/
typedef struct tagHY_DVR_HTTP_EVENT_SERVER_STATUS_TEST
{
    int iValid;                             /*!< 本结构体是否有效，0表示无效 */
    char acText[256];                       /*!< 输入参数,是否必须:否 */
    char acInputReserved[256];              /*!< 输入参数保留字段 */

    int iEventID;                           /*!< 输出参数,事件ID */
    char acStatusCode[32];                  /*!< 输出参数,服务器返回状态码 */
    char acTime[32];                        /*!< 输出参数,服务器响应时间 */
    char acInfo[1024];                      /*!< 输出参数,服务器返回的信息(使用base64编码) */

    char acOutputReserved[256];             /*!< 输出参数保留字段 */
}HY_DVR_HTTP_EVENT_SERVER_STATUS_TEST, *LPHY_DVR_HTTP_EVENT_SERVER_STATUS_TEST;

typedef struct tagHY_DVR_CURRENT_RECORD_INFO
{
	int nValid;

	int nCurrentRecordStartTime;

	char szReserved[1024];
}HY_DVR_CURRENT_RECORD_INFO, *LPHY_DVR_CURRENT_RECORD_INFO;

#define MAX_POLYGON_LINE_NUM 8

typedef struct tagHY_DVR_POLYGON_CONFIG
{
	int nValid;

	int nChannel;
	HY_DVR_POINT struPolygon[MAX_POLYGON_LINE_NUM];			//8个点表示8边形
	int nPolygonLineDirection;		//0：正向	1：反向		2：双向
	int nPolygonLineNum;

	//用途
	char cUseForMotionDetect;
	char cUseForPeopleDetect;
	char cUseForFaceDetect;
	char cUseForCarDetect;
	char szUseForReserved[16];

	//报警联动
	HY_DVR_HANDLEEXCEPTION struHandleException;

	int nTrackZoom;

	char cReserved[224];
}HY_DVR_POLYGON_CONFIG, *LPHY_DVR_POLYGON_CONFIG;

typedef struct tagHY_DVR_POLYGON_SCHEDULE
{
	int nValid;

	int nEnable; //0,表示全天，szSchedule，nDayMode不启用，1.表示szSchedule，nDayMode启用
	int nChannel;
	char szSchedule[DVR_MAX_DAYS][DVR_MAX_TIMESEGMENT];
	int nDayMode;		//0:夜晚，szSchedule不启用	1:白天，szSchedule不启用		2:当前模式不启用，szSchedule启用

	char szReserved[28];
}HY_DVR_POLYGON_SCHEDULE, *LPHY_DVR_POLYGON_SCHEDULE;

typedef struct tagHY_DVR_ASSISSTANT_LINE_CONFIG
{
	int nValid;

	int nEnable;
	int nChannel;
	HY_DVR_TRAVERSE_LINE struAssisstantLine;

	//用途
	char cUseForPeopleDetect;
	char cReserved[19];

	//报警联动
	HY_DVR_HANDLEEXCEPTION struHandleException;

	int nSensitivity;

	char szReserved[252];
}HY_DVR_ASSISSTANT_LINE_CONFIG, *LPHY_DVR_ASSISSTANT_LINE_CONFIG;

typedef struct tagHY_DVR_ASSISSTANT_LINE_SCHEDULE
{
	int nValid;

	int nEnable;
	int nChannel;
	char szSchedule[DVR_MAX_DAYS][DVR_MAX_TIMESEGMENT];

	char szReserved[32];
}HY_DVR_ASSISSTANT_LINE_SCHEDULE, *LPHY_DVR_ASSISSTANT_LINE_SCHEDULE;

typedef struct tagHY_DVR_DELETE_LANGUAGE_CUSTOM
{
	int nValid;

	int nChannelID;
	char strParameter[64];

	char szReserved[32];
}HY_DVR_DELETE_LANGUAGE_CUSTOM, *LPHY_DVR_DELETE_LANGUAGE_CUSTOM;

typedef struct tagHY_DVR_DETECT_REVISE
{
	int nValid;

	int nChannelID;
	int nReviseResult;		//0:成功		1:失败		2:校正中

	char szReserved[32];
}HY_DVR_DETECT_REVISE, *LPHY_DVR_DETECT_REVISE;

typedef struct tagHY_DVR_SCREEN_CLOSE_TIME
{
	int nValid;

	int nChannelID;
	int nScreenCloseTime;		//时间：秒
	int nRValue;		//息屏R值
	int nGValue;		//息屏G值
	int nBValue;		//息屏B值
	char cEnable;	//息屏是否启用
	char cSwitchTime;		//屏保图片切换时间(3-30s)
	char cSNorMACEnable;	//显示序列号或MAC地址.0-无效,1-SN，2-MAC
	char cTrafficPageStayTime;  //通行结果页面停留时间.2秒/5秒/10秒/15秒/20秒/25秒/30秒/0-自定义,注:此设置会引起屏保功能失效
    int iTrafficCustomStayTime; //通行页面自定义停留时间.范围5s-300s,cTrafficPageStayTime为0时有效

	char szReserved[12];
}HY_DVR_SCREEN_CLOSE_TIME, *LPHY_DVR_SCREEN_CLOSE_TIME;

typedef struct tagHY_DVR_PRESON_NAME_BITMAP_DATA
{
	int nValid;

	long long nPersonID;
	char strUniqueID[32];

	char *pLatticeBuffer;			//点阵缓冲
	int nLatticeBufferLength;		//点阵缓冲大小
	int nLatticeSIze;		//点阵大小
	int nWidth;				//点阵宽度
	int nHeight;				//点阵高度
	int nTextType;		//0:点阵中的数据是姓名

	char szReserved[32];
}HY_DVR_PRESON_NAME_BITMAP_DATA, *LPHY_DVR_PRESON_NAME_BITMAP_DATA;

typedef struct tagHY_DVR_CUSTOM_LANGUAGE_NAME
{
	int nValid;

	int nChannelID;
	char strCustomLanguageName[64];
	int nIsExistCustomLanguage;

	char szReserved[508];
}HY_DVR_CUSTOM_LANGUAGE_NAME, *LPHY_DVR_CUSTOM_LANGUAGE_NAME;

typedef struct tagHY_DVR_PRIVACY_SETTING_PUSH
{
	int nValid;
	int nIncludeFace;
	int nIncludeTemperature;

	char szReserved[32];
}HY_DVR_PRIVACY_SETTING_PUSH, *LPHY_DVR_PRIVACY_SETTING_PUSH;

typedef struct tagHY_DVR_PRIVACY_SETTING_LOG
{
	int nValid;
	int nIncludeFace;
	int nIncludeTemperature;

	char szReserved[32];
}HY_DVR_PRIVACY_SETTING_LOG, *LPHY_DVR_PRIVACY_SETTING_LOG;

typedef struct tagHY_DVR_PRIVACY_SETTING_DISPLAY
{
	int nValid;
	int nIncludeFace;
	int nIncludeTemperature;

	char szReserved[32];
}HY_DVR_PRIVACY_SETTING_DISPLAY, *LPHY_DVR_PRIVACY_SETTING_DISPLAY;

typedef struct tagHY_DVR_PRIVACY_SETTING
{
	int nValid;
	HY_DVR_PRIVACY_SETTING_PUSH stPush;
	HY_DVR_PRIVACY_SETTING_LOG stLog;
	HY_DVR_PRIVACY_SETTING_DISPLAY stDisplay;
	char cReserve[256];
}HY_DVR_PRIVACY_SETTING, *LPHY_DVR_PRIVACY_SETTING;

typedef struct tagHY_DVR_LANGUAGE_DISPLAY_STATUS
{
	int nValid;

	int nIndex;
	char szLanguageKey[32];
	char szLanguageName[32];
	int nIsDisplay;

	char szReserved[32];
}HY_DVR_LANGUAGE_DISPLAY_STATUS, *LPHY_DVR_LANGUAGE_DISPLAY_STATUS;

typedef struct tagHY_DVR_LANGUAGE_DISPLAY_STATUS_LIST
{
	int nValid;

	HY_DVR_LANGUAGE_DISPLAY_STATUS struLanguageDisplayStatus[64];

	char szReserved[1024];
}HY_DVR_LANGUAGE_DISPLAY_STATUS_LIST, *LPHY_DVR_LANGUAGE_DISPLAY_STATUS_LIST;

typedef struct tagHY_DVR_ORIGIN_IMAGE_CONFIG
{
	int nValid;

	int nChannelID;
	int nEnableOriginImage;

	char szReserved[512];
}HY_DVR_ORIGIN_IMAGE_CONFIG, *LPHY_DVR_ORIGIN_IMAGE_CONFIG;

#if 1
////////微信考勤机与中间层交互
//获取固化的SecretNo
typedef struct tagHY_DVR_WX_SEED_SECRET_NO
{
	int nValid;
	char strSeedSecretNo[256];		//固件初始的SecretNo
	char szReserved[128];			//保留
}HY_DVR_WX_SEED_SECRET_NO;

//获取或写入SecretNo
typedef struct tagHY_DVR_WX_SECRET_NO
{
	int nValid;
	char strSecretNo[256];		//当前设备的SecretNo：要存储
	char szReserved[128];		//保留
}HY_DVR_WX_SECRET_NO;

//中间层找协议获取动态码
typedef struct tagHY_DVR_WX_QR_CODE
{
	int nValid;
	char strQrCode[512];		//当前设备的SecretNo：要存储
	char szReserved[128];		//保留
}HY_DVR_WX_QR_CODE;

//获取或写入登录密钥
typedef struct tagHY_DVR_WX_SECRET
{
	int nValid;
	char strSecret[256];		//密钥  要存储
	char szReserved[128];		//保留
}HY_DVR_WX_SECRET;


//获取或写入设备的人脸库版本
typedef struct tagHY_DVR_WX_FIRM_VERSION
{
	int nValid;
	int nFirmVersion;    //当前人脸库版本号  
	char szReserved[128];		//保留
}HY_DVR_WX_FIRM_VERSION;

//连接服务器的状态
typedef struct tagHY_DVR_WX_CONNECT_SERVER
{
	int nValid;
	int nConnectServerStatus;       //0:表示未连接,1:表示已连接
	char szReserved[32];		//保留	
}HY_DVR_WX_CONNECT_SERVER;

//获取设备关于微信的配置
typedef struct tagHY_DVR_WX_DEVICE_INFO
{
	int nValid;
	int nDevType;       //0:考勤机,1:门禁机
	char szReserved[128];		//保留	
}HY_DVR_WX_DEVICE_INFO;

//人脸特征值
typedef struct tagHY_DVR_WX_FACE_FEATURE
{
	int nValid;
	int nId;				//人脸编号
	int nDataLen;			//特征值长度
	char strData[WX_MAX_FACE_DATA_SIZE];
	char szReserved[128];		//保留
}HY_DVR_WX_FACE_FEATURE;

//模式1 -> 门禁:年月日 时分 -- 年月日 时分 
typedef struct tagHY_DVR_WX_MODE_MONTH_AND_DAY_RULE
{
	int nValid;

	int nStartYear;				//开始年份 表示生效时间为nStartYear至nEndYear的区间  
	int nEndYear;				//结束年份		
	unsigned char nStartMonth;          //开始月份
	unsigned char nEndMonth;
	unsigned char nStartDay;            //开始号数
	unsigned char nEndDay;

	unsigned char nStartHour;
	unsigned char nEndHour;
	unsigned char nStartMin;
	unsigned char nEndMin;

	char szReserved[16];		//保留
}HY_DVR_WX_MODE_MONTH_AND_DAY_RULE;

//模式 -> 门禁:年 周几 时分 -- 年周几 时分
typedef struct tagHY_DVR_WX_MODE_WEEKDAY_RULE
{
	int nValid;

	int nStartYear;				//开始年份 表示生效时间为nStartYear至nEndYear的区间  
	int nEndYear;				//结束年份
	char cReserved[2];
	unsigned char nStartWeekDay;          //开始周几
	unsigned char nEndWeekDay;			//结束周几

	unsigned char nStartHour;
	unsigned char nEndHour;
	unsigned char nStartMin;
	unsigned char nEndMin;

	char szReserved[16];		//保留
}HY_DVR_WX_MODE_WEEKDAY_RULE;

//门禁规则
typedef struct tagHY_DVR_WX_PASS_RULE
{
	int nValid;
	int nId;					//门禁规则编号
	int nMode;					//规则模式 模式1 -> 门禁:年月日 时分 -- 年月日 时分    模式 -> 门禁:年 周几 时分 -- 年周几 时分
	int nRuleCount;				//此条门禁规则所对应时间段的条数

	HY_DVR_WX_MODE_MONTH_AND_DAY_RULE struMonthAndDay[WX_MAX_RULE_TIME_LIST_NUMBER];
	HY_DVR_WX_MODE_WEEKDAY_RULE struWeekDay[WX_MAX_RULE_TIME_LIST_NUMBER];

	int nSpaceDay;				//隔多少天生效一次
	int nSpaceWeek;				//隔多少周生效一次
	unsigned int nEffectTime;	//规则生效时间
	char szReserved[128];		//保留
}HY_DVR_WX_PASS_RULE;

//给中间层全量更新总人数
typedef struct tagHY_DVR_WX_TOTAL_MEMBER
{
	int nValid;
	int nTotalPeople;				//当前同步的总人数
	char szReserved[128];		//保留
}HY_DVR_WX_TOTAL_MEMBER;

//注意：目前一个人只有一张图片，只保存一个特征值，但是后期若企业微信有升级，一个人要保存多个特征值时，内存可能存在过大情况。
//单个成员信息  --用于全量拉取  
//用于增量同步的   增  改
typedef struct tagHY_DVR_WX_ONE_USER_INFO
{
	int nValid;
	char strUserid[64];				//成员ID
	int nUserType;					//人员类型
	int nFaVer;						//人脸版本号
	char strName[64];				//人员姓名
	int nOperate;					//1:人员变更   2:设备录入人脸  3:微信录入人脸
	int nCountFace;					//一个成员所对应的人脸图片数量，表示pstruFacelist的有效人脸个数
	int nCountPassRule;
	HY_DVR_WX_PASS_RULE struPassRule[WX_MAX_RULE_LIST_NUMBER];
	HY_DVR_WX_FACE_FEATURE	*pstruFaceFeature;	//作为输入参数,人脸特征值的缓冲区,中间层必须得复制一份，协议层在传递完了之后，会释放内存	
	char szReserved[128];		//保留
}HY_DVR_WX_ONE_USER_INFO;

//删除人员信息
typedef struct tagHY_DVR_WX_DELETE_PERSON_INFO
{
	int nValid;
	char strUserid[64];				//成员ID
	char szReserved[32];		//保留
}HY_DVR_WX_DELETE_PERSON_INFO;

typedef struct tagHY_DVR_WX_DELETE_PERSON_INFO_LIST
{
	int nValid;
	int nDeleteCount;
	HY_DVR_WX_DELETE_PERSON_INFO struDeletePersonInfo[WX_MAX_USERINFO_NUMBER];
	char szReserved[128];		//保留
}HY_DVR_WX_DELETE_PERSON_INFO_LIST;

//成员UserId变更事件
typedef struct tagHY_DVR_WX_CHANGE_USERID
{
	int nValid;
	unsigned int nTimeStamp;	//成员UserId变更的时间
	char strOldUserid[64];		//旧成员ID
	char strNewUserid[64];		//新成员ID
	int nFirmVersion;			//当前人脸库版本号 
	char szReserved[128];		//保留	
}HY_DVR_WX_CHANGE_USERID;

//通过userid查人脸特征
typedef struct tagHY_DVR_WX_FACE_GROUP
{
	int nValid;
	char strUserid[64];				//成员ID
	int nUserType;					//人员类型
	int nCountFace;
	HY_DVR_WX_FACE_FEATURE	*pstruFaceFeature;
	char strReserved[128];
}HY_DVR_WX_FACE_GROUP;

/////注意此处内存消耗可能会有些大，目前测试时发现一张图片约40K至50K，但后续若企业微信升级，图片可能会变大
//微信录入图片
typedef struct tagHY_DVR_WX_RECORD_PIC
{
	int nValid;
	char strUserid[64];				//成员ID
	int nUserType;					//人员类型
	//char strName[64];				//人员姓名   ------去掉:录入图片时获取不到
	int nId;						//作为输入参数，图片ID
	char strMediaType[32];			//图片格式
	int  nPicLength;				//作为输入参数,图片的长度
	char * pstrPicBuffer;			//作为输入参数,图片的缓冲区,中间层必须得复制一份，协议层在传递完了之后，会释放内存
	char strReserved[128];
}HY_DVR_WX_RECORD_PIC;

//成员打卡
typedef struct tagHY_DVR_WX_ONE_CHECK_IN
{
	int nValid;
	int nEventId;					//打卡记录事件ID
	char strUserid[64];		//成员ID
	int nUserType;				//人员类型
	unsigned int nTimeStamp;			//打卡时间
	int nPassType;			//门禁需要填写通行类型：1：进门 2：出门
	int nPassMethod;		//门禁需要填写通行方法：1：人脸 2：二维码
	char szReserved[128 - 8];		//保留
}HY_DVR_WX_ONE_CHECK_IN;


//成员打卡状态列表
typedef struct tagHY_DVR_WX_CHECK_IN_LIST
{
	int nValid;
	int nListCount;    //此次同步的打卡记录条数
	HY_DVR_WX_ONE_CHECK_IN struCheckIn[WX_MAX_CHECK_IN_PAGE_NUMBER];
	char szReserved[128];		//保留	
}HY_DVR_WX_CHECK_IN_LIST;

//成员打卡记录上传结果反馈
typedef struct tagHY_DVR_WX_CHECK_IN_RESP
{
	int nValid;
	int nStatus;                 //1:上传成功 ,2：表示打卡记录传输失败
	int nEventId;				//事件ID
	int nErrcode;				//错误码
	char szReserved[128];		//保留	
}HY_DVR_WX_CHECK_IN_RESP;

//成员打卡状态列表反馈
typedef struct tagHY_DVR_WX_CHECK_IN_RESP_LIST
{
	int nValid;
	int nListCount;    //此次同步的打卡记录反馈条数
	HY_DVR_WX_CHECK_IN_RESP struCheckInResp[WX_MAX_CHECK_IN_PAGE_NUMBER];
	char szReserved[128];		//保留	
}HY_DVR_WX_CHECK_IN_RESP_LIST;

//取消录入信息操作
typedef struct tagHY_DVR_WX_CANCEL_RECORD
{
	int nValid;
	char strUserid[64];				//成员ID
	int nUserType;					//人员类型			
	char szReserved[128];		//保留	
}HY_DVR_WX_CANCEL_RECORD;

//设备录入操作失败
typedef struct tagHY_DVR_WX_FAILED_RECORD
{
	int nValid;
	char strUserid[64];				//成员ID
	int nUserType;					//人员类型			
	char szReserved[128];		//保留	
}HY_DVR_WX_FAILED_RECORD;

//结束设备录入信息操作
typedef struct tagHY_DVR_WX_END_DEVICE_RECORD
{
	int nValid;
	char strUserid[64];				//成员ID
	int nUserType;					//人员类型			
	char szReserved[128];		//保留	
}HY_DVR_WX_END_DEVICE_RECORD;

//录入信息操作
typedef struct tagHY_DVR_WX_RECORD_USERINFO
{
	int nValid;
	char strUserid[64];				//成员ID
	int nUserType;					//人员类型			
	char szReserved[128];		//保留	
}HY_DVR_WX_RECORD_USERINFO;

//最大的发送与接受buff的长度
typedef struct tagHY_DVR_WX_SEND_RECV_BUFF_LENGTH
{
	int nValid;
	int nMaxSendRecvBuffLen;       //中间层允许协议的发送和接受buff
	char szReserved[32];		//保留	
}HY_DVR_WX_SEND_RECV_BUFF_LENGTH;

#endif

//金睿相关配置信息
#if 1
//获取APPID
typedef struct tagHY_DVR_FL_APPID
{
	int nValid;
	char strAppid[64];		//Appid  要存储
	char szReserved[128];		//保留
}HY_DVR_FL_APPID, *LPHY_DVR_FL_APPID;

//获取APPKEY
typedef struct tagHY_DVR_FL_APPKEY
{
	int nValid;
	char strAppkey[256];		//Appkey  要存储
	char szReserved[128];		//保留
}HY_DVR_FL_APPKEY, *LPHY_DVR_FL_APPKEY;

//获取
typedef struct tagHY_DVR_FL_NETWORK_INFO
{
	int nValid;
	int nFlMode;                //设备是否是使用的金睿的sim卡建立连接  0:其它    1:金睿sim卡
	int nConnectInterVal;       //使用金睿sim卡时允许传流时长   秒
	char szReserved[128];		//保留
}HY_DVR_FL_NETWORK_INFO, *LPHY_DVR_FL_NETWORK_INFO;

typedef struct tagHY_DVR_FL_CXZN_URL
{
	int nValid;
	char strURL[64];		//畅享智能 URL  要存储
	char szReserved[128];		//保留
}HY_DVR_FL_CXZN_URL, *LPHY_DVR_FL_CXZN_URL;

#endif

//北纬相关配置信息
#if 1
//获取APPID
typedef struct tagHY_DVR_FL_BW_APPID
{
	int nValid;
	char strAppid[64];		//Appid  要存储
	char szReserved[128];		//保留
}HY_DVR_FL_BW_APPID, *LPHY_DVR_FL_BW_APPID;

//获取SECRETKEY
typedef struct tagHY_DVR_FL_BW_SECRETKEY
{
	int nValid;
	char strSecretkey[256];		//secretkey  要存储
	char szReserved[128];		//保留
}HY_DVR_FL_BW_SECRETKEY, *LPHY_DVR_FL_BW_SECRETKEY;

//获取CARDTYPE
typedef struct tagHY_DVR_FL_BW_CARDTYPE
{
	int nValid;
	char strCardtype[256];		//cardtype  要存储
	char szReserved[128];		//保留
}HY_DVR_FL_BW_CARDTYPE, *LPHY_DVR_FL_BW_CARDTYPE;

#endif

typedef struct tagHY_DVR_FACE_RECO_EVENT_MODE
{
	int nValid;
	int nMode;					//人脸识别事件方式，0，原始方式，1，表示合并之后的方式
}HY_DVR_FACE_RECO_EVENT_MODE, *LPHY_DVR_FACE_RECO_EVENT_MODE;

typedef struct tagHY_DVR_NOTIFY_PUBLISH_STATUS
{
	int nValid;
	int nEventID;				//事件ID
	int nPublishStatus;			//发布状态，0,位置状态,1，开始发布，2发布成功，3。发布失败
}HY_DVR_NOTIFY_PUBLISH_STATUS, *LPHY_DVR_NOTIFY_PUBLISH_STATUS;

typedef struct tagHY_DVR_NOTIFY_PUBLISH_STATUS_V1
{
	int nValid;
	int nEventID;				//事件ID
	int nPublishStatus;			//发布状态，0,位置状态,1，开始发布，2发布成功，3。发布失败
	int nEventType;				//事件主类型,对应sendEvent/sendEventEx接口传入的dwEventType
	int nSubEventType;			//事件次类型,对应sendEvent/sendEventEx接口传入的dwSubEventType
	int nP6SEventPublishStatus; //P6SEvent事件发布状态;1-中间层已发送状态,2-协议层在发送之前向中间层发送正在发送状态,3-发送成功,4-发送失败,5-发送使能关闭,6-不支持的事件

	char szReserved[28];
}HY_DVR_NOTIFY_PUBLISH_STATUS_V1, *LPHY_DVR_NOTIFY_PUBLISH_STATUS_V1;

typedef struct tagHY_DVR_MEGLINK_NOTIFY_EVENT_UPLOAD_STATUS
{
	int nValid;

	int nEventType;
	int nEventID;				//事件ID
	int nPublishStatus;			//发布状态，0,位置状态,1，开始发布，2发布成功，3。发布失败

	char szReserved[32];
}HY_DVR_MEGLINK_NOTIFY_EVENT_UPLOAD_STATUS, *LPHY_DVR_MEGLINK_NOTIFY_EVENT_UPLOAD_STATUS;

typedef struct tagHY_DVR_DELETE_SPECIFIC_GROUPID_ITEMS
{
	int nValid;

	int nGroupID;
	int nIsAbsoluteDelete;

	char szReserved[32];
}HY_DVR_DELETE_SPECIFIC_GROUPID_ITEMS, *LPHY_DVR_DELETE_SPECIFIC_GROUPID_ITEMS;

typedef struct tagHY_DELETE_ALL_GROUP_ITEMS
{
	int nValid;

	char szReserved[32];
}HY_DELETE_ALL_GROUP_ITEMS, *LPHY_DELETE_ALL_GROUP_ITEMS;

typedef struct tagHY_DELETE_ALL_GROUP_ITEMS_V1
{
    int iValid;

    char acOwnner[64];            //安全码(仅人脸识别前抓前比功能使用)
    char acSystemTime[32];        //系统时间(仅人脸识别前抓前比功能使用)

    char szReserved[512];
}HY_DELETE_ALL_GROUP_ITEMS_V1;

typedef struct tagHY_DVR_WIEGAND_SETTINGS
{
	int nValid;

	int nChannelID;		//媒体通道，NVR用的
	int nEnable;
	int nDirection;		//0-输出；1-输入
	int nMode;
	int nCardSwitch;
	int nPasswordSwitch;
	int nMeglinkOutput;				//旷视九霄：韦根输出内容		0：人员卡号；1：人员编号
	int nWiegandID;		//韦根通道
	int iOutputQRCode;  //输出二维码;0:禁用,1:启用

	int szReserved[21];
}HY_DVR_WIEGAND_SETTINGS, *LPHY_DVR_WIEGAND_SETTINGS;

typedef struct tagHY_DVR_WIEGAND_TEST
{
	int nValid;

	int nChannelID;
	char szTestData[64];
	int nTimeout;

	char szReserved[256];
}HY_DVR_WIEGAND_TEST, *LPHY_DVR_WIEGAND_TEST;


typedef struct tagHY_DVR_IR_STATUS
{
	int nValid;

	int nChannelID;
	int nADCValue;

	char szReserved[32];
}HY_DVR_IR_STATUS, *LPHY_DVR_IR_STATUS;

typedef struct tagHY_DVR_PEOPLE_DETECT_EXTRA_INFO_SWITCH
{
	int nValid;

	int nChannelID;
	int nExtraInfoEnable;

	char szReserved[32];
}HY_DVR_PEOPLE_DETECT_EXTRA_INFO_SWITCH, *LPHY_DVR_PEOPLE_DETECT_EXTRA_INFO_SWITCH;

typedef struct tagHY_DVR_WECHAT_BASIC_STATUS
{
	int nValid;

	int nImportStatus;
	int nActiveStatus;

	char szReserved[32];
}HY_DVR_WECHAT_BASIC_STATUS, *LPHY_DVR_WECHAT_BASIC_STATUS;

typedef struct tagHY_DVR_CLOUD_UPDATE_BASIC_INFO
{
	int nValid;

	int nBoardID;
	char szCustomerID[32];
	char szCustomerVersionID[32];
	char szLanguage[32];
	int nDeviceType;
	char szType[32];
	char szCurrentVersion[32];
	char szRequestUrl[256];
	int nIs4g;		//是否是4g设备

	char szReserved[512 - 4];
}HY_DVR_CLOUD_UPDATE_BASIC_INFO, *LPHY_DVR_CLOUD_UPDATE_BASIC_INFO;


typedef struct tagHY_DVR_AUDIO_ENCODER_INFO
{
	int nValid;

	int nEnable;
	int nSampleRate;		//采样率
	int nSoundMod;		//1-momo	2:stereo
	int nChannel;			//声道数
	int nBitPerSample;	//位宽
	int nBitRate;				//码流比特率
	int nSamplePerFrame;	//每一帧中包含的sample数，AAC算法标准固定为1024
	int nEncoder;			//0:G711U	1:G711A	2:ACC
	int nChannelID;			//通道，对于IPC是0

	char szReserved[16];
}HY_DVR_AUDIO_ENCODER_INFO, *LPHY_DVR_AUDIO_ENCODER_INFO;

typedef struct tagHY_DVR_TIANYI_UPDATE_INFO
{
	int nValid;

	int nUpdateAuto;			//是否自动更新固件：0：禁用		1：启用
	char szUpdateServer[64];
	int updateInt;
	int nManualUpdateStatus;		//0：不能升级		1：可以升级
	char szUpdateReleaseData[32];		//固件升级日期
}HY_DVR_TIANYI_UPDATE_INFO, *LPHY_DVR_TIANYI_UPDATE_INFO;

typedef struct tagHY_DVR_TIANYI_STORAGE_INFO
{
	int nValid;

	char szStorageDev[32];
	char szStorageWay[32];
	char szStorageISCSIServer[32];
	char szStorageISCSIIQN[32];
	int nStorageStartTimeStamp;
	int nStorageEndTimeStamp;
	int nStorageRepeat;
	int nStorageMaxSize;

	char szReserved[512];
}HY_DVR_TIANYI_STORAGE_INFO, *LPHY_DVR_TIANYI_STORAGE_INFO;

/*!
*  @struct	tagHY_DVR_OSD_V3
*  @brief	V3的OSD配置, 用于获取，设置一堆OSD
*/
typedef struct tagHY_DVR_OSD_V3
{
	int bValid;
	int nChannel;												/*!< 通道号 */
	HY_DVR_OSD_TIME_CHANNEL_V2 struOSDTime;						/*!< OSD时间 */
	HY_DVR_OSD_CHANNEL_V2 struOSD[DVR_MAX_OSD_V2_CNT];			/*!< 最多支持4个OSD */

	int nOSDAlpha;
	int nOSDColor1;
	int nOSDColor2;
	int nOSDHeight1;
	int nOSDHeight2;

	char szReserved[256];
}HY_DVR_OSD_V3, *LPHY_DVR_OSD_V3;

typedef struct tagHY_DVR_TIANYI_P2P_INFO
{
	int nValid;

	char szServer[64];
	unsigned int nServerPort;
	char szUID[64];
	int nCircle;
	char szAlarmServer[64];

	char szReserved[256];
}HY_DVR_TIANYI_P2P_INFO, *LPHY_DVR_TIANYI_P2P_INFO;

typedef struct tagHY_DVR_TIANYI_ENCKEY_INFO
{
	int nValid;

	char szSecretKey[32];
	int nEnckeyType;			//视频I帧加密，0：不加密		1：加密
	char szEnckeyMgrServer[32];		//密钥管理服务器地址

	char szReserved[256];
}HY_DVR_TIANYI_ENCKEY_INFO, *LPHY_DVR_TIANYI_ENCKEY_INFO;

typedef enum eTeleLogLevelTianYi
{
	eTeleLog_Abnornal = -1,  // 特殊异常处理日志
	eTeleLog_Disable = 0,    // 不发送日志
	eTeleLog_Error = 1,		 // 错误日志
	eTeleLog_Prime = 2,      // 重要日志  ，包括核心功能日志和错误日志
	eTeleLog_Basic = 3       // 基本功能日志，包括基本操作日志、核心功能日志、错误日志
}eTeleLogLevelTianYi;


typedef struct tagHY_DVR_TIANYI_DEVICE_INFO_EX
{
	int nValid;

	int nSecurityLogin;		//0：非密码加固设备		1：密码加固设备
	char szEhomeMgrServer[32];			//0：翼家管理平台服务器地址
	int nDormancyStatus;		//0：休眠状态		1：唤醒状态
	char szImageAlarmServerAddress[32];		//报警图片服务器地址
	char szLogReportServer[32];			//日志上报服务器
	char szEhomeLogEnckey[32];		//日志上报加密密钥
	char szPublicIP[32];						//公网IP
	eTeleLogLevelTianYi eLogLevel;		//日志上报级别
	int nIFrameMaxSize;						//I帧上限
	char szDeviceSN[32];
	char szPassword[32];
	char szBindCode[16];
	char szAppKey[16];
	char szSecret[64];
	int  ColorLightSwitch;      //全彩模式：0-不支持，1-强制开启，2-强制关闭，3-自动（全彩智能模式，此时有移动侦测或者有人形检测时开启30秒，30秒有动作则继续延期，30内无动作则自动关闭）
	char szDeviceBoardInfo[32];
	int ScheduledDormFlag; //周期定时休眠标识， 2： 关闭， 1-开启， 0-不支持
	char DormancyTime[16]; //定时休眠的时间段， HHmm:HHmm(时分)
	char DormancyPeriod[16]; //定时休眠的周期， 1-7分别表示周一至周日， 多个日期用英文‘ ,’ 分隔， 例： 1,2,3,4,5
	int  startScheduledDorm; //周期定时休眠是否开始，用于告诉设备层周期定时休眠是否开始

	char szReserved[92];
}HY_DVR_TIANYI_DEVICE_INFO_EX, *LPHY_DVR_TIANYI_DEVICE_INFO_EX;

typedef struct tagHY_DVR_TIANYI_DEVICE_ABILITY
{
	char uid[64];	//设备uid		
	unsigned int ChannelNum; //视频通道数 ipcam = 1 
	unsigned int OSDSetFlag; //OSD 设置标志，1-支持，0-不支持
	unsigned int PTZFlag;    //云台标志，1-支持，0-不支持
	unsigned int LocalStoreFlag;  //本地存储状态 0-没有存储介质，1-有存储介质且可录像，2-有存储介质但没有格式化，3-存储介质异常，4-存储介质满（非循环录像时）
	unsigned int MotionDetectionFlag; //移动侦测，2：关闭，1：开启，0：不支持
	char MotionDetection[16];	//当MotionDetectionFlag=1时，该字段有值：0表示全天侦测，HHmm:HHmm表示侦测的时间段(时分)
	unsigned int InfraredDetectionFlag;	//人体红外探测，2：关闭，1：开启，0：不支持
	char FrameSize[64]; //设备支持的分辨率，以","分隔，例如： 1080P,VGA 代表设备支持 1080P 和 VGA 两种分辨率可选范围有：720P，D1，CIF，VGA ，QVGA, 1080P主码流和子码流都需要上报
	unsigned int AlarmRecordFlag;  //告警录像联动标志，1-支持，0-不支持
	unsigned int AlarmPictureFlag; //告警图片联动标志，1-支持，0-不支持
	unsigned int AlarmPolicyFlag;  //告警策略标志，1-支持，0-不支持
	unsigned int AlarmVoiceFlag; //告警声音联动标志，1-支持，0-不支持
	unsigned int AlarmLightFlag; //告警灯光联动标志，1-支持，0-不支持
	unsigned int VoicePlayFlag;	//语音对讲标志，2-双工 1-单工，0-不支持
	unsigned int FocusFlag; 	//镜头变焦标志，1-支持，0-不支持
	unsigned int PresetPointFlag; //预置点设置标志，1-支持，0-不支持
	char PresetPoint[128];		  //如果设备支持预置位设置并且已有设置，则返回设置的预置位序号，多个‘,’分割
	unsigned int CruiseFlag; 	  //巡航预置点标志，1-支持，0-不支持
	unsigned int WatchPresetFlag; //看守位设置标志，1-支持，0-不支持
	unsigned int PtzSpeedFlag;    //云台速度标志，1-支持，0-不支持
	unsigned int ImageInversionFlag; //图像倒置标志，2-关闭，1-开启，0-不支持
	unsigned int FullHDFlag;      //1080P 超清支持标志,1-支持，0-不支持
	unsigned int StatusLampFlag;  //状态灯设置支持标志, 1-支持，0-不支持
	unsigned int MicroPhoneFlag;  //麦克风设置支持标志，1-支持，0-不支持
	unsigned int VoiceAlarmDetectionFlag;  //声音侦测告警支持标志，2-关闭，1开启，0-不支持
	unsigned int AiCameraFlag;    //AI摄像机标志，1-支持，0-不支持
	char FeatureId[1024];         //如果设备是AI摄像机，并且设备上保存有特征信息，则返回已保存的特征信息Id，多个用‘，’分割
	unsigned int MoveTrackFlag;   //移动追踪标志，2：关闭，1-开启，0-不支持
	unsigned int VoiceTrackFlag;  //声音追踪标志，2：关闭，1-开启，0-不支持
	unsigned int QRCodeFlag;      //二维码配网标志，1-支持，0-不支持
	unsigned int TimeDormancyFlag; //定时休眠唤醒标识，2-非休眠状态，1-休眠状态，0-不支持

	unsigned int ScheduledDormFlag; //周期定时休眠标识，2：关闭，1-开启，0-不支持
	char DormancyTime[16]; //定时休眠的时间段，HHmm:HHmm(时分)
	char DormancyPeriod[16]; //定时休眠的周期，1-7分别表示周一至周日，多个日期用英文‘,’分隔，例：1,2,3,4,5

	unsigned int RebootFlag;       //重启，1-支持，0-不支持
	char ConnectSSIDCable[64];     //设备连接无线使用 "Wireless"  ，有线用"Cable"
	char IPAddress[32];            //设备本地IP地址
	char MACAddress[64];           //设备MAC地址
	char DeviceVersion[64];        //设备固件版本
	char DeviceModel[64];          //设备型号
	char CTEI[64];                 //设备CTEI码，必须与终端包装盒上的CTEI码保持一致
	int  StreamType;			   //码流，代表设备有几路流，1-代表1路，2-代表2路，以此类推 
	int  H265Support;              //是否支持H.265视频编码，1-支持，0-不支持
	char MainStreamCode[10];		//主码流当前编码 "H264"-H.264编码  ,"H265"-H.265编码（当能力值发生变化时，立即上报）
	char SubStreamCode[10];			//子码流当前编码 "H264"-H.264编码,  "H265"-H.265编码（当能力值发生变化时，立即上报）
	int  HotConfigId;				//平台下发的业务配置ID，设备出厂或重置后或者平台未下发，上报0，该值设置了以后需要断电保存，恢复出厂时恢复成0
	int  MotionSensitivityFlag;		//移动侦测灵敏度设置标志，1-支持，0-不支持 
	int  MotionSensitivity;			//当前灵敏度设置值，5档，具体用数字表示，1-5数字表 示，具体数据值：1-小 2-较小 3-中等 4-较大 5-大
	int  ColorLightSwitch;			//全彩模式：0-不支持，1-强制开启，2-强制关闭，3-自动（全彩智能模式，此时有移动侦测或者有人形检测时开启30秒，30秒有动作则继续延期，30内无动作则自动关闭）
	int  BodyDetect;				//人形识别开关2：关闭，1：开启，0：不支持
	int  AlarmInterval;				//告警消息间隔（1，5（默认），10， 30）分钟，对所有告警。包括移动侦 测告警、声音侦测告警等
	int  BodyTrack;					//人形追踪开关， 0-不支持， 1-开启，2关闭。 打开的情况下，检测到人形， 摄像机跟随人形转动。
	int  CryDetect;					//哭声侦测告警开关，0-不支持，1-开 启， 2-关闭。检测到哭声时上报告警 消 息 ， 告 警 时 间 间 隔 按 AlarmInterval。
	int  FaceDetect;				//人脸识别能力上报，0-不支持，1-开 启， 2-关闭。
	int  AlarmFence; 				//自定义告警区域(电子围栏)能力上报，0-不支持，1-开启， 2-关闭。 
	char AlarmFencePoints[1024]; 	//自定义告警区域(电子围栏)坐标上报，坐标：最少3个，最多10个；区域：最多3个,具体参看《IPC公共接口及云存储接口开发文档》
	int  AlarmFenceWarning;         //声音警戒 0-不支持，1-开启，2-关闭 
	int  FaceUpload; 				//人脸检测抓拍上传能力，0-不支持，1-开启， 2-关闭
	char ChipModelCode[10]; 		//芯片型号编码，下面对应“芯片字典表”里面的芯片型号编 码，举例0010010 代表realtek瑞昱 realtek3903 
	char ChipModelDesc[128]; 		//芯片型号描述，格式为“厂家 型号”比如"realtek realtek3903" 英文表示 
	int  GAT1400; 					// 摄像机是否支持GA/T1400协议，0-不支持，1-开启， 2-关闭

	char szReserved[1024];
}HY_DVR_TIANYI_DEVICE_ABILITY, *LPHY_DVR_TIANYI_DEVICE_ABILITY;

typedef struct tagHY_DVR_CUSTOM_LANGUAGE_LATTICE_SWITCH
{
	int nValid;

	int nChannelID;
	int nCustomLanguageLatticeSwitch;

	char szReserved[32];
}HY_DVR_CUSTOM_LANGUAGE_LATTICE_SWITCH, *LPHY_DVR_CUSTOM_LANGUAGE_LATTICE_SWITCH;

typedef struct tagHY_DVR_PERMISSION_DETAIL
{
	int nValid;

	char cReadPermission;
	char cWritePermission;
	char reserved1[2];

	char szReserved[8];
}HY_DVR_PERMISSION_DETAIL, *LPHY_DVR_PERMISSION_DETAIL;

typedef struct tagHY_DVR_PERMISSION_SET
{
	int nValid;

	int nVoiceTalk;
	int nTransparentChannel;

	//智能配置
	HY_DVR_PERMISSION_DETAIL struFace;
	HY_DVR_PERMISSION_DETAIL struTemperature;
	HY_DVR_PERMISSION_DETAIL struFaceMask;
	HY_DVR_PERMISSION_DETAIL struSecretInfo;
	HY_DVR_PERMISSION_DETAIL struOpenDoor;
	HY_DVR_PERMISSION_DETAIL reserved1[4];

	HY_DVR_PERMISSION_DETAIL struImage;

	HY_DVR_PERMISSION_DETAIL struBasicNetworkConfig;
	HY_DVR_PERMISSION_DETAIL struAdvanceNetworkConfig;
	HY_DVR_PERMISSION_DETAIL reserved2[4];

	HY_DVR_PERMISSION_DETAIL struCodec;
	HY_DVR_PERMISSION_DETAIL struAlarm;
	HY_DVR_PERMISSION_DETAIL struBasicSystemConfig;
	HY_DVR_PERMISSION_DETAIL struAdvanceSystemConfig;
	HY_DVR_PERMISSION_DETAIL reserved3[8];

	int nSystemUpdate;

	char reserved0[512];
}HY_DVR_PERMISSION_SET, *LPHY_DVR_PERMISSION_SET;

typedef struct tagHY_DVR_USER_PERMISSION_V1
{
	int nValid;

	int nChannelID;
	int nUserID;
	char szUsername[DVR_MAX_USERNAME_LEN];
	eZWUserLevel level;

	HY_DVR_PERMISSION_SET struLocalPermission;
	HY_DVR_PERMISSION_SET struRemotePermission;

	char szLocalPlayback[128];
	char szLocalPreview[128];
	char szLocalPTZ[128];
	char szLocalMannalRecord[128];
	char szRemotePlayback[128];
	char szRemotePreview[128];
	char szRemotePTZ[128];
	char szRemoteMannalRecord[128];

	char reserved[1024];

}HY_DVR_USER_PERMISSION_V1, *LPHY_DVR_USER_PERMISSION_V1;

typedef struct tagHY_DVR_MQTT_REMOTE_OPEN_DOOR
{
	int nValid;
	char strControlId[32];
	char cReserve[64];
}HY_DVR_MQTT_REMOTE_OPEN_DOOR, *LPHY_DVR_MQTT_REMOTE_OPEN_DOOR;


typedef struct tagHY_DVR_ALARM_VOICE_LIGHT_CONFIG
{
	int nValid;

	int nIsVoiceOpen;
	int nIsLightOpen;
	int nAlarmTime;

	char szReserved[32];
}HY_DVR_ALARM_VOICE_LIGHT_CONFIG, *LPHY_DVR_ALARM_VOICE_LIGHT_CONFIG;

typedef struct tagHY_DVR_VOICE_LIGHT_STATE
{
	int nValid;

	int nAlarmState;
	int nRemainTime;

	char szReserved[32];
}HY_DVR_VOICE_LIGHT_STATE, *LPHY_DVR_VOICE_LIGHT_STATE;

typedef struct tagHY_DVR_MEGLINK_HTTP_REQUEST_INFO
{
	int nValid;

	char szServerAddress[64];
	char szSecret[64];
	char szDeviceToken[64];
	char szHTTPSCandidate[64];
	long long nRegisterTime;

	char szReserved[256];
}HY_DVR_MEGLINK_HTTP_REQUEST_INFO, *LPHY_DVR_MEGLINK_HTTP_REQUEST_INFO;


typedef struct tagHY_DVR_MEGLINK_DEVICE_INFO
{
	int nValid;

	char szUsername[32];		//非必须		用户名，邮箱或者手机，Koala 模式必填
	char szPassword[32];		//非必须		密码 md5 校验值，Koala 模式必填
	char szSN[32];				//面板机序列号
	int nIsFactorySetting;		//设备是否为出厂状态
	char szDeviceChannel[32];	//设备类型
	char szAppChannel[32];		//app类型
	char szRomVersion[32];		//rom版本号
	char szAppVersion[32];		//app版本号

	char szReserved[512];
}HY_DVR_MEGLINK_DEVICE_INFO, *LPHY_DVR_MEGLINK_DEVICE_INFO;

typedef struct tagHY_DVR_MEGLINK_GATEWAY_CONFIG
{
	int nValid;

	int nSensorType;					//门磁类型 0：不使用；1：常开；2：常闭
	char szRelayAddress[DVR_MAX_IP_STR_LEN];
	int nLockSwitch;				//门锁开关 0：关闭	1：开启
	int nOpenTimeout;		//开门超时时长 单位：秒	最大值: 255 最小值: 1
	float nOpenDuration;		//开门驱动持续时长	 单位：秒	最大值: 60 最小值: 0.1
	float nOpenDelay;		//开门延时	 单位：秒	最大值: 60 最小值: 0

	char szReserved[1024];
}HY_DVR_MEGLINK_GATEWAY_CONFIG, *LPHY_DVR_MEGLINK_GATEWAY_CONFIG;

typedef struct tagHY_DVR_MEGLINK_PASS_CONFIG
{
	int nValid;

	int nVerficationMode;	//通行认证方式 0：人脸；1：人脸或刷卡；2：人脸及刷卡；3：人脸及密码
	int nFaceCardVerificationMode;		//人证混合认证模式	0：不启用 1：人脸及证件混合认证
	char szFaceCardNotificationUrl[256];		//人证混合认证回流 url
	int nRecognitionMode;	//通行识别模式	0：单人；1：多人
	int nMinFace;			//最小脸像素值
	float nPitchThreshold;	//人脸垂直角度阈值，单位：度	最大值: 90   最小值: 0
	float nRollThreshold;		//人脸旋转角度阈值，单位：度	最大值: 90   最小值: 0
	float nYawThreshold;		//人脸水平角度阈值，单位：度	最大值: 90   最小值: 0
	float nSharpnessThreshold;	//人脸清晰度阈值
	float nMaxBrightness;	//人脸亮度最大值	最大值: 100	最小值: 0
	float nMinBrightness;	//人脸亮度最小值	最大值: 100	最小值: 0
	float nBrightnessStdDeviation;		//人脸亮度标准差阈值
	float nRecognitionThreshold;		//通行识别阈值		最大值: 100	最小值: 0
	float nStrangerThreshold;		//通行陌生人阈值
	int nLivenessMode;	//通行活体模式 0 关 1 开单目 2开双目
	float nRGBLivenessThreshold;		//通行 RGB 活体阈值		最大值: 100	最小值: 0
	int nFillLightThreshold;		//通行补光阈值		最大值: 100	最小值: 0
	int nFillLightness;		//通行补光亮度		最大值: 100	最小值: 0
	int nCameraWDRMode; //摄像头宽动态开关
	int nCameraWDR;		//摄像头宽动态等级
	int nMaxDistance;		//最大识别距离等级		1：1 米；2：1.5 米；3：2 米
	float nRecognitionInterval;	//重新识别间隔，单位：秒	最大值: 9		最小值: 0
	char szNotificationAddress[64];	//识别消息通知的第三方地址，{ip}:{port}
	int nAllowBlackList;	//是否允许黑名单人员通行
	int nIsPublicGate;		//是否为公共门
	int nIsMaskMode;		//口罩识别模式，true开启，false关闭
	float nMaskThreshold;	//口罩识别阈值	最大值: 100	最小值: 0

	char szReserved[1024];
}HY_DVR_MEGLINK_PASS_CONFIG, *LPHY_DVR_MEGLINK_PASS_CONFIG;

typedef struct tagHY_DVR_MEGLINK_PERSTY_CONFIG
{
	int nValid;

	char szCompanyName[32 * 4];		//公司名称
	char szLocation[256];		//设备地点
	int nShowPersonName;		//弹窗是否显示人员名字
	int nPictureType;		//弹窗显示的图片类型	0：底库照片；1：抓拍照片 2：不显示图片
	int nDuressEnable;		//是否开启胁迫警示功能

	char szReserved[1024];
}HY_DVR_MEGLINK_PERSTY_CONFIG, *LPHY_DVR_MEGLINK_PERSTY_CONFIG;

typedef struct tagHY_DVR_MEGLINK_SYS_CONFIG
{
	int nValid;

	int nVoiceVolume;		//语音音量调节	最大值: 100	最小值: 0
	int nTouchVolume;		//触控提示音		最大值: 100	最小值: 0
	int nStandByTime;		//待机时间		最大值: 30	最小值: 0
	int nKeepAliveInterval;		//心跳间隔
	int nGroupPersonSyncInterval;	//人员、人员组关系数据同步间隔，单位：秒
	int nGroupScheduleSyncInterval;	//人员组、时间计划数据同步间隔，单位：秒
	int nConfigSyncInterval;		//配置数据同步间隔，单位：秒
	int nRelayEnable;				//是否使能继电器
	int nPersonInfoMaxBatchNum;	//获取人员信息最大批量数
	int nFeatureUploadMaxBatchNum;	//特征上传最大批量数
	int nRecordUploadMaxBatchNum;	//记录上传最大批量数
	int nEventUploadMaxBatchNum;		//事件上传最大批量数
	char szStandByWPUrl[64];			//自定义待机界面地址,为半角逗号分隔的 uri 列表
	int nStandByWPCarouselInterval;		//待机图片的轮播时间，单位为秒
	int nFaceAEMode;					// FACE AE 开关

	char szReserved[1024];
}HY_DVR_MEGLINK_SYS_CONFIG, *LPHY_DVR_MEGLINK_SYS_CONFIG;

typedef struct tagHY_DVR_MEGLINK_ALERT_CONFIG
{
	int nValid;

	int nAlertInput1;	//报警输入1		0：不启用；1：消防信号；2：报警信号
	int nAlertInput2;	//报警输入2		0：不启用；1：消防信号；2：报警信号
	int nAlertOutput[8];		//报警输出，如果无效给-1

	char szReserved[512];
}HY_DVR_MEGLINK_ALERT_CONFIG, *LPHY_DVR_MEGLINK_ALERT_CONFIG;

typedef struct tagHY_DVR_MEGLINK_STRANGER_CONFIG
{
	int nValid;

	int nRelayEnable;		//陌生人联动继电器报警模式 （开启或关闭）
	char szAddress[DVR_MAX_IP_STR_LEN];		//继电器IP
	int nRelayType;			//继电器类型， 目前只支持一种TCP-KP-C2 (值是2)
	int nRelayDuration;			//继电器持续时间， 0~10s

	char szReserved[512];
}HY_DVR_MEGLINK_STRANGER_CONFIG, *LPHY_DVR_MEGLINK_STRANGER_CONFIG;

typedef struct tagHY_DVR_MEGLINK_TEMPERATURE_CONFIG
{
	int nValid;

	int nMode;					//是否开启测温
	int nMaskPrompt;				//口罩佩戴提醒
	int nMaskNoEntryWithoutMask;	//未佩戴口罩禁入
	int nModeNonrecognition;		//快速测温
	int nHighNoEntry;			//高温禁入
	float nHighThreshold;			//高温阈值
	int nSensitivity;				//测温灵敏度，

	char szReserved[512];
}HY_DVR_MEGLINK_TEMPERATURE_CONFIG, *LPHY_DVR_MEGLINK_TEMPERATURE_CONFIG;

typedef struct tagHY_DVR_MEGLINK_CONFIG_UPDATE_TIMESTAMP
{
	int nValid;

	long long nTimeStamp;		//客户端更新的时间，服务器需要判断哪边更新来确定更新规则，上述所有结构体中的配置改变后，中间层都要更新此时间

	char szReserved[32];
}HY_DVR_MEGLINK_CONFIG_UPDATE_TIMESTAMP, *LPHY_DVR_MEGLINK_CONFIG_UPDATE_TIMESTAMP;

/*
*****人员，人员组关系同步*****
*/
//人员，人员组关系同步版本
typedef struct tagHY_DVR_MEGLINK_PERSON_PERSONGROUP_SYNC_INFO
{
	int nValid;

	char szVersion[32];

	char szReserved[256];
}HY_DVR_MEGLINK_PERSON_PERSONGROUP_SYNC_INFO, *LPHY_DVR_MEGLINK_PERSON_PERSONGROUP_SYNC_INFO;

//人员信息添加
typedef struct tagHY_DVR_MEGLINK_PEOPLE_PEOPLEGROUP_PERSON_INFO
{
	int nValid;

	char szGroupID[64];			//脸库ID
	char szPeopleID[64];			//人员id
	char szPeopleName[128];			//人员姓名
	char *imageData;		//底库图片，中间层仅使用，不要操作内存，比如释放
	int nImageSize;		//图片大小
	char szImageUrl[256];		//图片URL，推送抽取特征事件要用到
	char szImageFeature[2048];		//底库图片对应的特征值
	char szPeopleCode[32];			//人员编号
	char szCardNumber[32];		//卡号
	char szPasswordMD5[32];		//密码 md5 哈希值
	int nIsAdmin;			//boolean	
	int nPeopleType;				//人员类型		1：员工；2：访客；3：黑名单
	HY_DVR_TIME struStartTime;	//访客允许通行时间，仅访客类型
	HY_DVR_TIME struEndTime;
	char szVisitTarget[32];	//受访人，仅访客类型
	long long nUpdateTime;			//更新时间

	//扩展保留字段
	char szIdNumber[32];			//身份证号

	char szReserved[512 - 32];
}HY_DVR_MEGLINK_PEOPLE_PEOPLEGROUP_PERSON_INFO, *LPHY_DVR_MEGLINK_PEOPLE_PEOPLEGROUP_PERSON_INFO;

//从某个组里面删除人员（当某个人不在属于任何一个组的时候，删除人员信息）
typedef struct tagHY_DVR_MEGLINK_PEOPLE_PEOPLEGROUP_DELETE_PERSON_INFO
{
	int nValid;

	char szGroupID[64];
	char szPersonID[64];

	char szReserved[512];
}HY_DVR_MEGLINK_PEOPLE_PEOPLEGROUP_DELETE_PERSON_INFO, *LPHY_DVR_MEGLINK_PEOPLE_PEOPLEGROUP_DELETE_PERSON_INFO;

//全量更新（删除所有人员信息，删除所有人员和组绑定关系，即所有组都不存在人了，但是组还在）
typedef struct tagHY_DVR_MEGLINK_PEOPLE_PEOPLEGROUP_CLEAR_INFO
{
	int nValid;

	char szReserved[512];
}HY_DVR_MEGLINK_PEOPLE_PEOPLEGROUP_CLEAR_INFO, *LPHY_DVR_MEGLINK_PEOPLE_PEOPLEGROUP_CLEAR_INFO;

//人员组和时间计划
//员工组
typedef struct tagHY_DVR_MEGLINK_PEOPLE_GROUP_CONFIG
{
	int nValid;

	char szGroupID[64];
	char szGroupName[128];
	int nGroupType;			//组类型		1：员工组；2：访客组；3：黑名单组
	int nIsAll;					//是否为员工组

	char szReserved[256];
}HY_DVR_MEGLINK_PEOPLE_GROUP_CONFIG, *LPHY_DVR_MEGLINK_PEOPLE_GROUP_CONFIG;

//删除员工组（保留人员）
typedef struct tagHY_DVR_MEGLINK_CLEAR_PEOPLE_GROUP_CONFIG
{
	int nValid;

	int nClearType;				//清除类型：0：删除所有的组		1：指定组ID清除
	char szGroupID[64];

	char szReserved[256];
}HY_DVR_MEGLINK_CLEAR_PEOPLE_GROUP_CONFIG, *LPHY_DVR_MEGLINK_CLEAR_PEOPLE_GROUP_CONFIG;

//时间计划
typedef struct tagHY_DVR_MEGLINK_TIME_NODE
{
	int nValid;

	int nStartHour;
	int nStartMinute;
	int nStopHour;
	int nStopMinute;

	char szReserved[32];
}HY_DVR_MEGLINK_TIME_NODE, *LPHY_DVR_MEGLINK_TIME_NODE;

typedef struct tagHY_DVR_MEGLINK_WEEK_SECHULE_NODE
{
	int nValid;

	int nDay;
	HY_DVR_MEGLINK_TIME_NODE struTimeNode[8];

	char szReserved[256];
}HY_DVR_MEGLINK_WEEK_SECHULE_NODE, *LPHY_DVR_MEGLINK_WEEK_SECHULE_NODE;

typedef struct tagHY_DVR_MEGLINK_HOLIDAY_SCHEDULE_NODE
{
	int nValid;

	int nYear;
	int nMonth;
	int nDay;
	HY_DVR_MEGLINK_TIME_NODE struTimeNode[8];

	char szReserved[256];
}HY_DVR_MEGLINK_HOLIDAY_SCHEDULE_NODE, *LPHY_DVR_MEGLINK_HOLIDAY_SCHEDULE_NODE;

typedef struct tagHY_DVR_MEGLINK_TIME_SCHEDULE_CONFIG
{
	int nValid;

	char szScheduleID[64];
	char szScheduleName[128];
	HY_DVR_MEGLINK_WEEK_SECHULE_NODE struWeekScheduleNode[7];
	HY_DVR_MEGLINK_HOLIDAY_SCHEDULE_NODE struHolidayScheduleNode[16];
	int nAction;

	char szReserved[256];
}HY_DVR_MEGLINK_TIME_SCHEDULE_CONFIG, *LPHY_DVR_MEGLINK_TIME_SCHEDULE_CONFIG;

typedef struct tagHY_DVR_MEGLINK_CLEAR_TIME_SCHEDULE_CONFIG
{
	int nValid;

	int nClearType;					//0：清空时间计划表		1：指定时间计划表清空
	char szScheduleID[64];

	char szReserved[256];
}HY_DVR_MEGLINK_CLEAR_TIME_SCHEDULE_CONFIG, *LPHY_DVR_MEGLINK_CLEAR_TIME_SCHEDULE_CONFIG;

//查询设备上的时间计划数量
typedef struct tagHY_DVR_MEGLINK_QUERY_TIMESCHEDULE_NUM
{
	int nValid;

	int nTimeScheduleNum;

	char szReserved[32];
}HY_DVR_MEGLINK_QUERY_TIMESCHEDULE_NUM, *LPHY_DVR_MEGLINK_QUERY_TIMESCHEDULE_NUM;

//根据Index获取时间计划信息
typedef struct tagHY_DVR_MEGLINK_QUERY_TIMESCHEDULE
{
	int nValid;

	int nIndex;
	char szTimeScheduleID[64];

	char szReserved[256];
}HY_DVR_MEGLINK_QUERY_TIMESCHEDULE, *LPHY_DVR_MEGLINK_QUERY_TIMESCHEDULE;

//组和时间计划绑定关系
typedef struct tagHY_DVR_MEGLINK_GROUP_SCHEDULE_BIND_RELATION
{
	int nValid;

	char szGroupID[64];
	char szScheduleID[64];

	char szReserved[256];
}HY_DVR_MEGLINK_GROUP_SCHEDULE_BIND_RELATION, *LPHY_DVR_MEGLINK_GROUP_SCHEDULE_BIND_RELATION;

typedef struct tagHY_DVR_MEGLINK_CLEAR_GROUP_SCHEDULE_BIND_RELATION
{
	int nValid;

	int nType;					//0：全部清除		1：按照给定的GroupID和ScheduleID删除
	char szGroupID[64];
	char szScheduleID[64];

	char szReserved[256];
}HY_DVR_MEGLINK_CLEAR_GROUP_SCHEDULE_BIND_RELATION, *LPHY_DVR_MEGLINK_CLEAR_GROUP_SCHEDULE_BIND_RELATION;

//员工组和时间计划表同步时间
typedef struct tagHY_DVR_MEGLINK_PEOPLEGROUP_TIMESCHEDULE_UPDATETIME
{
	int nValid;

	long long nUpdateTime;

	char szReserved[32];
}HY_DVR_MEGLINK_PEOPLEGROUP_TIMESCHEDULE_UPDATETIME, *LPHY_DVR_MEGLINK_PEOPLEGROUP_TIMESCHEDULE_UPDATETIME;

typedef struct tagHY_DVR_MEGLINK_RECOGNIZE_RECORD_NUM
{
	int nValid;

	char szPersonID[64];

	int nItemNum;

	char szReserved[128];
}HY_DVR_MEGLINK_RECOGNIZE_RECORD_NUM, *LPHY_DVR_MEGLINK_RECOGNIZE_RECORD_NUM;

//防疫健康信息
typedef struct tagHY_DVR_MEGLINK_RECOGNIZE_RECORD_HEALTH_INFO
{
	int nValid;								/*!< 本结构体是否有效，0表示无效 */
	int nHealthyState;						//健康码状态枚举 : 0, 1, 2, 3, 4，枚举备注 : 0：绿色；1：黄色；2：红色；3未知、4超时
	int HealthkitPasstype;					//健康码通行结果 : 0、未通过 1 通过 2 未使用
	int nNatTestState;						//核酸状态枚举 : 0, 1, 2枚举备注 : 0：无，1：有并且为阴性，2：有并且为阳性
	unsigned long long ullNatTestLasttime;	//核酸检测时间，最近一次核酸时间, 单位时间戳，精确到毫秒
	char szIdNumber[64];					//身份证号
	int nVaccineState;						//接种疫苗状态，枚举 : 0, 1, 2枚举备注 : 0：未打疫苗，1：打了一针，2：打了两针，3：打了三针
	char szReserved[128];
}HY_DVR_MEGLINK_RECOGNIZE_RECORD_HEALTH_INFO, *LPHY_DVR_MEGLINK_RECOGNIZE_RECORD_HEALTH_INFO;

//识别记录
typedef struct tagHY_DVR_MEGLINK_RECOGNIZE_RECORD_NODE
{
	int nValid;

	long long nLogID;
	int nItemIndex;					//本字段由协议赋值，意义是识别记录的index，由中间层回写其他字段的值
	char szPersonID[64];
	char szPersonName[128];
	char *imageData;			//中间层表示此指针未填值,buffer：HY_DVR_MEGLINK_RECOGNIZE_RECORD_NODE+图片二进制
	int nImageDataLength;		//图片长度
	int nRecognitionType;		//识别人员类型	1：员工；2：访客；3：黑名单；4：陌生人；5：未识别
	int nLivenessType;		//活体结果	0：非活体；1：活体；2：未检测
	int nVerificationMode;	//验证类型	0：人脸；1：人脸或刷卡；2：人脸及刷卡；3：人脸及密码; 4:远程开门 5：二维码扫码开门
	int nPassType;			//通行类型	0：未通行；1：通行 2：刷脸通过密码错误（复核认证失败）
	float nRecognitionScore;	//识别分数
	float nLivenessScore;	//活体分数
	float nTemperature;	//人体温度
	int nTemperatureType;	//人体高温状态		0：未知，1：正常，2：高温
	int nMaskType;		//口罩状态，0：未知，1：戴口罩，2：未戴口罩
	int nTimeStamp;		//抓住时间戳
	char szImageMD5[48];		//图片MD5值
	char szImagePath[64];		//图片路径

	//扩展保留字段
	HY_DVR_MEGLINK_RECOGNIZE_RECORD_HEALTH_INFO	struHealthInfo;		//防疫健康信息

	char szReserved[512 - sizeof(HY_DVR_MEGLINK_RECOGNIZE_RECORD_HEALTH_INFO)];
}HY_DVR_MEGLINK_RECOGNIZE_RECORD_NODE, *LPHY_DVR_MEGLINK_RECOGNIZE_RECORD_NODE;

//旷视OTA升级检查
typedef struct tagHY_DVR_MEGLINK_OTA_UPDATE
{
	int nValid;

	char szAppVersion[64];
	char szRomVersion[64];
	char szAppChannel[64];
	char szDeviceChannel[64];

	char szReserved[256];
}HY_DVR_MEGLINK_OTA_UPDATE, *LPHY_DVR_MEGLINK_OTA_UPDATE;

typedef struct tagHY_DVR_THIRD_PROTOCOL_MANAGE
{
	int nValid;

	int nJstpProtocol;

	char szReserved[32];
}HY_DVR_THIRD_PROTOCOL_MANAGE, *LPHY_DVR_THIRD_PROTOCOL_MANAGE;

typedef struct tagHY_DVR_MEGLINK_FACE_FEATURE_ANALYSIS_FAIL_EVENT
{
	int nValid;

	char szFeatureVersion[32];
	char szPersonID[64];
	char szImageUrl[256];
	int nExtractFeatureStatus;

	char szReserved[512];
}HY_DVR_MEGLINK_FACE_FEATURE_ANALYSIS_FAIL_EVENT, *LPHY_DVR_MEGLINK_FACE_FEATURE_ANALYSIS_FAIL_EVENT;

typedef struct tagHY_DVR_MEGLINK_FEATURE_UPLOAD_EVENT
{
	int nValid;

	char szFeature[2048];
	char szPersonID[64];
	char szImageUrl[256];

	char szReserved[256];
}HY_DVR_MEGLINK_FEATURE_UPLOAD_EVENT, *LPHY_DVR_MEGLINK_FEATURE_UPLOAD_EVENT;

typedef struct tagHY_DVR_PLATFORM_MODE
{
	int nValid;

	int nPlatformMode;		//0：MQTT		1：Meglink    2：JiGong		3：HTTP

	char szReserved[32];
}HY_DVR_PLATFORM_MODE, *LPHY_DVR_PLATFORM_MODE;

typedef struct tagHY_DVR_OTA_UPDATE_INFO
{
	int nValid;

	int nTotalSize;
	int nDownloadSize;

	char szReserved[32];
}HY_DVR_OTA_UPDATE_INFO, *LPHY_DVR_OTA_UPDATE_INFO;

typedef struct tagHY_DVR_MEGLINK_SYNC_INFO
{
	int nValid;

	int nType;						//0：配置同步		1：人员同步
	int nProgressRate;			//-1：同步失败		0-100：同步进度

	char szReserved[32];
}HY_DVR_MEGLINK_SYNC_INFO, *LPHY_DVR_MEGLINK_SYNC_INFO;

typedef struct tagHY_DVR_MEGLINK_QUERY_PEOPLE_BELONG_TO_GROUP_NUM
{
	int nValid;

	char szPersonID[64];
	int nBelongToGroupNum;

	char szReserved[1024];
}HY_DVR_MEGLINK_QUERY_PEOPLE_BELONG_TO_GROUP_NUM, *LPHY_DVR_MEGLINK_QUERY_PEOPLE_BELONG_TO_GROUP_NUM;

typedef struct tagHY_DVR_MEGLINK_QUERY_PEOPLE_BELONG_TO_GROUP
{
	int nValid;

	int nItemIndex;
	char szPersonID[64];
	char szGroupID[64];

	char szReserved[1024];
}HY_DVR_MEGLINK_QUERY_PEOPLE_BELONG_TO_GROUP, *LPHY_DVR_MEGLINK_QUERY_PEOPLE_BELONG_TO_GROUP;

typedef struct tagHY_DVR_MEGLINK_QUERY_GROUP_SCHEDULE_RELATION_NUM
{
	int nValid;

	int nScheduleIDRelationNum;

	char szReserved[32];
}HY_DVR_MEGLINK_QUERY_GROUP_SCHEDULE_RELATION_NUM, *LPHY_DVR_MEGLINK_QUERY_GROUP_SCHEDULE_RELATION_NUM;

typedef struct tagHY_DVR_MEGLINK_QUERY_GROUP_SCHEDULE_RELATION
{
	int nValid;

	int nItemIndex;
	char szGroupID[64];
	char szScheduleID[64];

	char szReserved[256];
}HY_DVR_MEGLINK_QUERY_GROUP_SCHEDULE_RELATION, *LPHY_DVR_MEGLINK_QUERY_GROUP_SCHEDULE_RELATION;

typedef struct tagHY_DVR_FACE_DELETE_RECO_RECORD
{
	int nValid;

	long long nLogID;

	char szReserved[32];
}HY_DVR_FACE_DELETE_RECO_RECORD, *LPHY_DVR_FACE_DELETE_RECO_RECORD;

typedef struct tagHY_DVR_MEGLINK_SERVER_CONNECTION_STATE
{
	int nValid;

	int nConnectionState;		//0：服务器连接中		1：服务器连接成功	2：服务器连接失败	3：服务器连接断开	

	char szReserved[32];
}HY_DVR_MEGLINK_SERVER_CONNECTION_STATE, *LPHY_DVR_MEGLINK_SERVER_CONNECTION_STATE;

typedef struct tagHY_DVR_MEGLINK_QUERY_PEOPLE_GROUP_NUM
{
	int nValid;

	int nGroupNum;

	char szReserved[32];
}HY_DVR_MEGLINK_QUERY_PEOPLE_GROUP_NUM, *LPHY_DVR_MEGLINK_QUERY_PEOPLE_GROUP_NUM;

typedef struct tagHY_DVR_MEGLINK_QUERY_PEOPLE_GROUP
{
	int nValid;

	int nGroupIndex;
	char szGroupID[64];

	char szReserved[256];
}HY_DVR_MEGLINK_QUERY_PEOPLE_GROUP, *LPHY_DVR_MEGLINK_QUERY_PEOPLE_GROUP;

typedef struct tagHY_DVR_MEGLINK_UNBIND
{
	int nValid;

	char szReserved[32];
}HY_DVR_MEGLINK_UNBIND, *LPHY_DVR_MEGLINK_UNBIND;

typedef struct tagHY_DVR_MEGLINK_ERROR_INFO
{
	int nValid;

	int nErrorCode;
	char szErrorMessage[256];

	char szReserved[2048];
}HY_DVR_MEGLINK_ERROR_INFO, *LPHY_DVR_MEGLINK_ERROR_INFO;

typedef struct tagHY_DVR_TIANYI_STREAM_PACKAGE_INFO
{
	int nValid;

	int nPackageType;
	int nStreamID;

	char szReserved[32];
}HY_DVR_TIANYI_STREAM_PACKAGE_INFO, *LPHY_DVR_TIANYI_STREAM_PACKAGE_INFO;

typedef struct tagHY_DVR_GB28181_UP_SERVER_CFG
{
	char cSIPServerID[32]; //SIP服务器ID，长度必须是20
	char cSIPServerRegion[16]; //SIP服务器域,长度必须是10
	char cSIPServerIPAddress[256]; //SIP服务器IP地址或者域名
	int nSIPServerPort; //SIP服务器端口
	char cSIPUserName[32];
	char cSIPUserAuthID[32];
	char cSIPPassword[32];
	int nTransportLayType; //传输层协议类型，0,tcp,1,udp
	char cReserve[256]; //保留字段
}HY_DVR_GB28181_UP_SERVER_CFG, *LPHY_DVR_GB28181_UP_SERVER_CFG;

typedef struct tagHY_DVR_GB28181_ALARMIN_ID
{
	int nID;
	char cSIPAlarmInID[32]; //SIP报警输入ID，长度必须是20
}HY_DVR_GB28181_ALARMIN_ID, *LPHY_DVR_GB28181_ALARMIN_ID;

typedef struct tagHY_DVR_GB28181_VIDEO_OUT_ID
{
	int nID;
	char cSIPVideoOutID[32]; //SIP视频输出ID，长度必须是20
}HY_DVR_GB28181_VIDEO_OUT_ID, *LPHY_DVR_GB28181_VIDEO_OUT_ID;

typedef struct tagHY_DVR_GB28181_AUDIOOUT_ID
{
	int nID;
	char cSIPAudioOutID[32]; //SIP音频输出ID，长度必须是20
}HY_DVR_GB28181_AUDIOOUT_ID, *LPHY_DVR_GB28181_AUDIOOUT_ID;

/** @brief 最大报警输入数											                    		 */
#define MAX_ALARM_IN_CHANNEL_COUNT				(DVR_MAX_ALARMIN_NUM)

/** @brief 最大报警输出数											                    		 */
#define MAX_VIDEO_OUT_CHANNEL_COUNT			(DVR_MAX_VIDEOIN_NUM)

/** @brief 最大音频输出数											                    		 */
#define MAX_AUDIO_OUT_CHANNEL_COUNT	(DVR_MAX_VIDEOIN_NUM + 1)

typedef struct tagHY_DVR_GB28181_CFG
{
	int nValid; //结构体有效性，0表示无效，1表示有效
	int nChannelID;
	int nEnable; //0,未启用，1启用
	int nProtocolVersion; //0,GB28181-2011,1,GB28181-2016
	HY_DVR_GB28181_UP_SERVER_CFG struGB28181UpServerCfg; //上级服务器配置
	int nHeartbeatInterval; //心跳间隔，单位是秒，范围是5~255
	int nMaxHeartbeatTimeoutCount; //最大心跳超时次数，范围是3~255
	int nRegisterExpire; //注册有效期，单位是秒，范围3600~100000
	int nRegisterInterval; //注册间隔，单位是秒，范围是60~600
	int nRegisterState; //0，不在线，1在线
	int nStreamType; //使用码流类型，0，主码流，1.子码流，2。第二子码流
	HY_DVR_GB28181_ALARMIN_ID struAlarm[MAX_ALARM_IN_CHANNEL_COUNT];		//报警输入编号列表,与设备支持的最大报警输入个数相同XML定义WEB界面
	HY_DVR_GB28181_VIDEO_OUT_ID struSIPVideoOutID[MAX_VIDEO_OUT_CHANNEL_COUNT];		//视频编码ID列表，与设备支持的最大视频通道个数相同
	HY_DVR_GB28181_AUDIOOUT_ID struAudioOutID[MAX_AUDIO_OUT_CHANNEL_COUNT];	//语音输出编码ID列表，与设备支持的最大视频通道个数相同
	int voiceTalkTransportType;												//对讲传输方式 0:udp, 1:tcp主动, 2:tcp被动

	int GBUpgradeFlag;    // GB升级标志，当GB下发升级的时候此状态由协议层设置 0:未下发GB升级，1:GB下发升级
	int GBUpgradeResult;	  // 设备启动时候先获取升级标志，如果设置了升级标志，再获取升级状态，升级状态由中间层设置，0:升级失败，1:升级成功，-1:无效值
	int UpgradeFaileReason; // 升级失败原因，1:软件下载超时；2:升级包损坏；3:系统异常；99:其他
	char UpgradeSessionId[128];    // sessionID,升级成功后不能使用当前的sessionID，要使用下发升级时的sessionID
	int iDevieceSIPPort;	// 设备SIP端口
	char cReserve[252 - 12 - 128 - 4]; //保留字段
}HY_DVR_GB28181_CFG, *LPHY_DVR_GB28181_CFG;

typedef struct tagHY_DVR_GB28181_REGISTER_STATE
{
	int nRegisterState; //0，不在线，1在线
}HY_DVR_GB28181_REGISTER_STATE, *LPHY_DVR_GB28181_REGISTER_STATE;

typedef struct tagHY_DVR_LIONKING_UP_SERVER_CFG
{
	int nValid; 														//结构体有效性，0表示无效，1表示有效
	char cServerIPAddress[256]; 										//服务器IP地址或者域名
	int nServerPort; 													//服务器端口

	char cReserve[256]; 												//保留字段
}HY_DVR_LIONKING_UP_SERVER_CFG, *LPHY_DVR_LIONKING_UP_SERVER_CFG;

typedef struct tagHY_DVR_LIONKING_CFG
{
	int nValid; 														//结构体有效性，0表示无效，1表示有效
	HY_DVR_LIONKING_UP_SERVER_CFG stLionKingUpServerCfg; 				//服务器配置

	char cReserve[256]; 												//保留字段
}HY_DVR_LIONKING_CFG, *LPHY_DVR_LIONKING_CFG;

typedef struct tagHY_DVR_LIONKING_SERIAL_CMD_FORMAT
{
	int nValid; 														//结构体有效性，0表示无效，1表示有效
	unsigned char cCombineCode1; 										//组合码1  以主机发送数据给摄像机为例,对应撤防( 0xFF 0x01 length 0x0E 0x00 Xor )中的0xFF
	unsigned char cCombineCode2; 										//组合码2  以主机发送数据给摄像机为例,对应撤防( 0xFF 0x01 length 0x0E 0x00 Xor )中的0x01
	unsigned char cLength;												//长度     以主机发送数据给摄像机为例,对应撤防( 0xFF 0x01 length 0x0E 0x00 Xor )中的length
	unsigned char cCmdCode1;											//指令码2  以主机发送数据给摄像机为例,对应撤防( 0xFF 0x01 length 0x0E 0x00 Xor )中的0x0E
	unsigned char cCmdCode2;											//指令码2  以主机发送数据给摄像机为例,对应撤防( 0xFF 0x01 length 0x0E 0x00 Xor )中的0x00
	unsigned char cCheckCode;											//校验码   以主机发送数据给摄像机为例,对应撤防( 0xFF 0x01 length 0x0E 0x00 Xor )中的Xor

	char cReserve[64]; 													//保留字段
}HY_DVR_LIONKING_SERIAL_CMD_FORMAT, *LPHY_DVR_LIONKING_SERIAL_CMD_FORMAT;

typedef struct tagHY_DVR_LIONKING_SERIAL_SETGUARD
{
	int nValid; 														//结构体有效性，0表示无效，1表示有效
	int iSetGuard; 														//0表示撤防，1表示布防
	char cReserve[64]; 													//保留字段
}HY_DVR_LIONKING_SERIAL_SETGUARD, *LPHY_DVR_LIONKING_SERIAL_SETGUARD;

typedef struct tagHY_DVR_LIONKING_SERIAL_SETALARM
{
	int nValid; 														//结构体有效性，0表示无效，1表示有效
	int iSetAlarm; 														//0表示打开主机警号声音，1表示关闭主机警号声音
	char cReserve[64]; 													//保留字段
}HY_DVR_LIONKING_SERIAL_SETALARM, *LPHY_DVR_LIONKING_SERIAL_SETALARM;

typedef struct tagHY_DVR_LIONKING_SERIAL_SETPGM1_PORT
{
	int iValid; 														//结构体有效性，0表示无效，1表示有效
	int iSetPGM1Port; 													//0表示关闭PGM1口，1表示打开PGM1口
    
	char cReserve[64]; 													//保留字段
}HY_DVR_LIONKING_SERIAL_SETPGM1_PORT, *LPHY_DVR_LIONKING_SERIAL_SETPGM1_PORT;

typedef struct tagHY_DVR_LIONKING_SERIAL_SETPGM2_PORT
{
	int iValid; 														//结构体有效性，0表示无效，1表示有效
	int iSetPGM2Port; 													//0表示关闭PGM2口，1表示打开PGM2口
    
	char cReserve[64]; 													//保留字段
}HY_DVR_LIONKING_SERIAL_SETPGM2_PORT, *LPHY_DVR_LIONKING_SERIAL_SETPGM2_PORT;

typedef struct tagHY_DVR_APPLY_DID_MANUL_CFG
{
	int nValid;
	char acRequestUrl[256];				  								//请求url(协议+服务器地址/域名+服务器端口+资源路径+参数等,例：http://192.168.1.102/system/zzz,注:可以为空)
	char acReserved[256];						  						//保留(方便日后扩展)
} HY_DVR_APPLY_DID_MANUL_CFG, *LPHY_DVR_APPLY_DID_MANUL_CFG;

/*!
*  @struct	tagHY_DVR_POINT_V2
*  @brief	点配置信息
 */
typedef struct tagHY_DVR_POINT_V2
{
	int bValid;
	int iX;																			/*!< x相对坐标（0-10000）*/
	int iY;																			/*!< y相对坐标（0-10000）*/
}HY_DVR_POINT_V2, *LPHY_DVR_POINT_V2;

/*!
*  @struct	tagHY_DVR_REGION_V2
*  @brief	区域配置信息
 */
typedef struct tagHY_DVR_REGION_V2
{
	int bValid;
	int iAreaTopLeftX;														/*!< 区域的左上点x相对坐标（0-10000）*/
	int iAreaTopLeftY;														/*!< 区域的左上点y相对坐标（0-10000）*/
	int iAreaBottomRightX;													/*!< 区域的右下点x相对坐标（0-10000）*/
	int iAreaBottomRightY;													/*!< 区域的右下点y相对坐标（0-10000）*/
}HY_DVR_REGION_V2, *LPHY_DVR_REGION_V2;

#define ELECTRONIC_FENCE_UI_DESIGN_V2_MAX_TARGET_COUNT	8
typedef struct tagHY_DVR_ELECTRONIC_FENCE_UI_DESIGN_CFG
{
	int nValid;
	int iEnable;													/*!< 是否使能 */
	int iSensitivity; 												/*!< 灵敏度。取值范围1~100：其中30为低，60为中，90为高 */
	int iAlarmMode;													/*!< 报警模式。0-按方向，1-按位置 */
	int iDetectDirection;											/*!< 检测方向。0-Invalid;1-OutAndIn;2-In;3-Out */
	int iIntrusionDuration; 										/*!< 入侵时长/滞留时间(V2,取值0-600)。0:入侵了就触发，非0:跟踪阈值，跟踪目标达到阈值后触发 */
	int iDetectTarget;												/*!< 检测目标。同时只检查人形或者车形：0:人形;1:车形;2:人形+车形 */
	HY_DVR_POINT_V2 stPolygon[MAX_POLYGON_LINE_NUM];				/*!< n个点表示n边形。例:8个点表示8边形 */
	HY_DVR_HANDLEEXCEPTION stHandleType;							/*!< 处理方式 */
	HY_DVR_SCHEDTIME stSchedTime;									/*!< 布防计划 */
	int iWarningFrame;           									/*!< 显示警戒框.0-不显示，1-显示 */
    int iTargetFrame;            									/*!< 显示目标框.0-不显示，1-显示 */
    int iIsPushBackgroundPicture;                                   /*!< 是否推送背景图/推送告警图(V2).0-不推送，1-推送 */
    int nChannel;                                                   /*!< 通道号 */
	int iAlarmTargetValid;											/*!< 检测目标是否有效(兼容旧版本).0-否，1-是 */
	char acAlarmTarget[ELECTRONIC_FENCE_UI_DESIGN_V2_MAX_TARGET_COUNT];	/*!< 告警目标;数组元素0表示告警目标为人,元素值bit0=1表示检测人的进入,元素值bit1=1表示检测人的离开,元素值bit2=1表示检测人的滞留,支持同时检测多种状态,依此类推;
																			数组元素1表示告警目标为机动车,元素值bit0=1表示检测机动车的进入,元素值bit1=1表示检测机动车的离开,元素值bit2=1表示检测机动车的滞留,支持同时检测多种状态,依此类推;
																			数组元素2表示告警目标为非机动车,元素值bit0=1表示检测非机动车的进入,元素值bit1=1表示检测非机动车的离开,元素值bit2=1表示检测非机动车的滞留,支持同时检测多种状态,依此类推;
																			依此类推; */
	int cVersion;                                                  /*!< 电子围栏版本; 0-V1,1-V2 */

	char acReserved[32];
} HY_DVR_ELECTRONIC_FENCE_UI_DESIGN_CFG, *LPHY_DVR_ELECTRONIC_FENCE_UI_DESIGN_CFG;

typedef struct tagHY_DVR_ELECTRONIC_FENCE_EVENT_TIMESLOT_LIST
{
    int iValid;
    HY_DVR_SCHEDULE_TIMESLOT stSchedTimeSlot[SCHEDULE_TIMESLOT_MAX_NUM];     /*!< 布防时间计划 */
    int nChannel;                                                   /*!< 通道号 */

    char szReserved[1020];
}HY_DVR_ELECTRONIC_FENCE_EVENT_TIMESLOT_LIST, *LPHY_DVR_ELECTRONIC_FENCE_EVENT_TIMESLOT_LIST;


typedef struct tagHY_DVR_CAR_PLATE_SNAP_VOICEALARM_CFG
{
    int iValid;

    int iEnable;                                                    /*!< 是否使能 */
    int iVoiceType; /*!< 0:自定义,1:警报,2:犬吠,3:警戒区域,4:监控区域,5:欢迎光临,6:深水警告,7:私人领地,8:危险区域,9:贵重物品,10:高处危险,
                        11:私家价位,12:叮咚,13:有口罩,14:无口罩,15:注意私人物品,16:抓小偷,17:救命,18:分类,19:可回收垃圾,20:家庭食物垃圾
                        21:危险废物,22:残余垃圾,23:全部分类,24:车形自定义,25:禁止停车 */
    int iBroadcastCount;                                            /*!< 播报次数 */
    int iDayMode;/*!< 0:夜晚模式,1:白天模式,2:不启用此模式（AllDay字段有效，当字段不为disable时，AllDay字段无效）当设置为此模式时，IPC依赖自身传感器判断当前环境是白天还是夜晚*/
    int iBroadcastVolume; /*!< 播报音量 */ 
    HY_DVR_SCHEDTIME stSchedTime;                                    /*!< 声音报警布防时间 */
	int iIsLicensePlateBroadcast;									/*!< 是否车牌播报；0-否，1-是 */
	int iLicensePlateBroadcastMode;									/*!< 车牌播报模式；0-仅车牌播报，1-车牌播报+自定义语音 */ 

    char acReserved[256 - 8];
} HY_DVR_CAR_PLATE_SNAP_VOICEALARM_CFG, *LPHY_DVR_CAR_PLATE_SNAP_VOICEALARM_CFG;

typedef struct tagHY_DVR_CAR_PLATE_SNAP_CFG
{
    int iValid;
	int iEnable;													/*!< 是否使能 */
    int iShowSchedFrame;           									/*!< 显示布防框.0-不显示，1-显示 */
    int iCarFrame;            									    /*!< 显示车型框.0-不显示，1-显示 */
    int iCarInfo;            									    /*!< 显示车辆信息.0-不显示，1-显示 */
    int iIsPushBackgroundPicture;                                   /*!< 是否推送背景图.0-不推送，1-推送 */ 
    int iSnapMode;                                                  /*!< 抓拍模式.0-自动，1-手动 */ 
    int iSnapSpeed;                                                 /*!< 抓拍速度，仅在抓拍模式为手动时有效.单位：km/h */ 
    int iIncomingDirection;                                         /*!< 驶入方向.0-下行为驶入，1-上行为驶入 */ 
    int iAlarmRules;                                                /*!< 报警规则.0-驶入，1-驶出, 2-驶入&驶出 */ 
    int iIsRepeatPush;                                              /*!< 是否重复推送.0-否，1-是 */ 
    int iPushInterval;                                              /*!< 推送时间间隔，单位：秒 */ 
	HY_DVR_POINT_V2 stPolygon[MAX_POLYGON_LINE_NUM];				/*!< n个点表示n边形。例:8个点表示8边形 */
	HY_DVR_HANDLEEXCEPTION stHandleType;							/*!< 处理方式 */
	HY_DVR_SCHEDTIME stSchedTime;									/*!< 布防计划 */
    HY_DVR_CAR_PLATE_SNAP_VOICEALARM_CFG stVoiceAlarm;              /*!< 声音报警 */
    int iCarPlateFrame;            									/*!< 显示车牌框.0-不显示，1-显示 */
    int bIsSupportSecondPolygon;                                    /*!< 是否支持第二个检测区域 */
    HY_DVR_POINT_V2 stSecondPolygon[MAX_POLYGON_LINE_NUM];          /*!< 第二个检测区域，n个点表示n边形。例:8个点表示8边形 */
    int nNoMoveNoInspect;                                           /*!< 是否开启不动不检； 0 - 否， 1 - 是 */
	int iIsIdentifyLicensePlateNum;                                 /*!< 是否识别车牌号码； 0 - 否， 1 - 是 */
	int iCutoutImageType;                                           /*!< 抠图类型；0-车形图，1-车牌图 */
	int iSnapshotMode;                                              /*!< 抓拍方式；0-最优，1-快速 */
	int iIdentifyCountry;											/*!< 识别国家，默认不识别；0-不识别，1-识别 */
	int iIdentifyArea;												/*!< 识别区域，默认不识别；0-不识别，1-识别 */

    char acReserved[904 - 8];
}HY_DVR_CAR_PLATE_SNAP_CFG, *LPHY_DVR_CAR_PLATE_SNAP_CFG;


typedef struct tagHY_DVR_CAR_PLATE_SNAP_EVENT_TIMESLOT_LIST
{
    int iValid;
    HY_DVR_SCHEDULE_TIMESLOT stSchedTimeSlot[SCHEDULE_TIMESLOT_MAX_NUM]; /*!< 布防时间计划 */
    char szReserved[1024];
}HY_DVR_CAR_PLATE_SNAP_EVENT_TIMESLOT_LIST,*LPHY_DVR_CAR_PLATE_SNAP_EVENT_TIMESLOT_LIST;


typedef struct tagHY_DVR_CAR_PLATE_SNAP_VOICEALARM_TIMESLOT_LIST
{
    int iValid;
    HY_DVR_SCHEDULE_TIMESLOT stSchedTimeSlot[SCHEDULE_TIMESLOT_MAX_NUM]; /*!< 布防时间计划 */
    char szReserved[1024];
}HY_DVR_CAR_PLATE_SNAP_VOICEALARM_TIMESLOT_LIST,*LPHY_DVR_CAR_PLATE_SNAP_VOICEALARM_TIMESLOT_LIST;


typedef struct tagHY_DVR_CROSSBORDER_LINE
{
	int bValid;
	HY_DVR_POINT_V2 stBeginPoint;											/*!< 起点坐标  */
	HY_DVR_POINT_V2 stEndPoint;												/*!< 终点坐标  */
}HY_DVR_CROSSBORDER_LINE, *LPHY_DVR_CROSSBORDER_LINE;

typedef struct tagHY_DVR_CROSSBORDER_DETECT_UI_DESIGN_CFG
{
	int bValid;																/*!< 本结构体是否有效，0表示无效  */

	int bEnable;															/*!< 是否启用越界侦测 */
	int iDetectTarget;														/*!< 检测目标。0:人形;1:车形 */
	int iOrientation;														/*!< 检测方向，0：A->B，1：B->A，2：A<->B */
	int iSenstive;															/*!< 灵敏度。取值范围1~100：其中30为低，60为中，90为高*/
	HY_DVR_CROSSBORDER_LINE stCrossBorderLine;								/*!< 越界侦测线条配置 */
	HY_DVR_HANDLEEXCEPTION stHandleType;									/*!< 处理方式  */
	HY_DVR_SCHEDTIME stSchedTime;											/*!< 布防计划  */
} HY_DVR_CROSSBORDER_DETECT_UI_DESIGN_CFG, *LPHY_DVR_CROSSBORDER_DETECT_UI_DESIGN_CFG;

typedef struct tagHY_DVR_CROSSBORDER_DETECT_UI_DESIGN_CFG_LIST
{
	int bValid;																	/*!< 本结构体是否有效，0表示无效  */

	HY_DVR_CROSSBORDER_DETECT_UI_DESIGN_CFG stCrossBorderDetectUIDesign[DVR_MAX_VIDEOIN_NUM];
} HY_DVR_CROSSBORDER_DETECT_UI_DESIGN_CFG_LIST, *LPHY_DVR_CROSSBORDER_DETECT_UI_DESIGN_CFG_LIST;

typedef struct tagHY_DVR_CROSSBORDER_DETECT_UI_DESIGN_CFG_V1
{
    int bValid;       /*!< 本结构体是否有效，0表示无效  */

    int bEnable;      /*!< 是否启用越界侦测 */
    int iDetectTarget;/*!< 检测目标。0:人形;1:车形;2:人形+车形 */
    int iOrientation; /*!< 检测方向，0：A->B，1：B->A，2：A<->B */
    int iSenstive;    /*!< 灵敏度。取值范围1~100：其中30为低，60为中，90为高*/
    HY_DVR_CROSSBORDER_LINE stCrossBorderLine;/*!< 越界侦测线条配置 */
    HY_DVR_HANDLEEXCEPTION struHandleType; /*!< 处理方式  */
    HY_DVR_SCHEDTIME struSchedTime;/*!< 布防计划  */
    int iWarningFrame;           /*!< 显示警戒框.0-不显示，1-显示 */
    int iTargetFrame;            /*!< 显示目标框.0-不显示，1-显示 */
    int iIsPushBackgroundPicture; /*!< 是否推送背景图.0-不推送，1-推送 */

    char cReserve[252];
} HY_DVR_CROSSBORDER_DETECT_UI_DESIGN_CFG_V1, *LPHY_DVR_CROSSBORDER_DETECT_UI_DESIGN_CFG_V1;

typedef struct tagHY_DVR_CROSSBORDER_DETECT_UI_DESIGN_CFG_V1_LIST
{
    int bValid;                                                                    /*!< 本结构体是否有效，0表示无效  */

    HY_DVR_CROSSBORDER_DETECT_UI_DESIGN_CFG_V1 stCrossBorderDetectV1[DVR_MAX_VIDEOIN_NUM];
    int nChannel;                                   /*!< 通道号 */

	char acReserve[1020];
} HY_DVR_CROSSBORDER_DETECT_UI_DESIGN_CFG_V1_LIST, *LPHY_DVR_CROSSBORDER_DETECT_UI_DESIGN_CFG_V1_LIST;

typedef struct tagHY_DVR_CROSSBORDER_DETECT_EVENT_TIMESLOT_LIST
{
    int iValid;
    HY_DVR_SCHEDULE_TIMESLOT stSchedTimeSlot[SCHEDULE_TIMESLOT_MAX_NUM]; /*!< 布防时间计划 */
    char szReserved[1024];
}HY_DVR_CROSSBORDER_DETECT_EVENT_TIMESLOT_LIST, *LPHY_DVR_CROSSBORDER_DETECT_EVENT_TIMESLOT_LIST;

typedef struct tagHY_DVR_CROSSBORDER_DETECT_EVENT_TIMESLOT_V1_LIST
{
    int iValid;
    HY_DVR_CROSSBORDER_DETECT_EVENT_TIMESLOT_LIST stSchedTimeSlotV1[DVR_MAX_VIDEOIN_NUM]; /*!< 布防时间计划 */
    int nChannel;                                   /*!< 通道号 */
    char szReserved[1020];
}HY_DVR_CROSSBORDER_DETECT_EVENT_TIMESLOT_V1_LIST, *LPHY_DVR_CROSSBORDER_DETECT_EVENT_TIMESLOT_V1_LIST;


typedef struct tagHY_DVR_CROSSBORDER_DETECT_VOICEALARM_TIMESLOT_LIST
{
    int iValid;
    HY_DVR_SCHEDULE_TIMESLOT stSchedTimeSlot[SCHEDULE_TIMESLOT_MAX_NUM]; /*!< 布防时间计划 */
    int nChannel;                                   /*!< 通道号 */
    char szReserved[1020];
}HY_DVR_CROSSBORDER_DETECT_VOICEALARM_TIMESLOT_LIST, *LPHY_DVR_CROSSBORDER_DETECT_VOICEALARM_TIMESLOT_LIST;

typedef struct tagHY_DVR_ELECTRONIC_FENCE_EX_CFG
{
    int iValid;

    int iEnable;                                                    /*!< 是否使能 */
    int iVoiceType; /*!< 0:自定义,1:警报,2:犬吠,3:警戒区域,4:监控区域,5:欢迎光临,6:深水警告,7:私人领地,8:危险区域,9:贵重物品,10:高处危险,
                        11:私家价位,12:叮咚,13:有口罩,14:无口罩,15:注意私人物品,16:抓小偷,17:救命,18:分类,19:可回收垃圾,20:家庭食物垃圾
                        21:危险废物,22:残余垃圾,23:全部分类,24:车形自定义,25:禁止停车 */
    int iBroadcastCount;                                            /*!< 播报次数 */
    int iDayMode;/*!< 0:夜晚模式,1:白天模式,2:不启用此模式（AllDay字段有效，当字段不为disable时，AllDay字段无效）当设置为此模式时，IPC依赖自身传感器判断当前环境是白天还是夜晚*/
    HY_DVR_SCHEDTIME stSchedTime;                                    /*!< 声音报警布防时间 */
	int iBroadcastVolume; /*!< 播报音量 */ 
    int nChannel;                                                   /*!< 通道号 */
	char cOpenDoorMode;                                             /*!< 开门模式；0-自动，1-上锁，2-常开 */
    char cPersonAlarmTriggerMode;                                   /*!< 人形报警触发方式；0-人形中心点触发，1-人形边缘触发 */
	char cIsOpenDoorModeInvalid;                                    /*!< cOpenDoorMode字段是否无效(xml中无cIsOpenDoorModeInvalid对应的节点);0-否(有效,xml中包含OpenDoorMode),1-是(无效);w3/安智定制版本/fullhanv3-fh865x */
	char cPersonTrackSwitch;                                        /*!< 人形跟踪开关；0-关，1-开；w3/安智定制版本/fullhanv3-fh865x */
	char cTwowayPassSwitch;                                         /*!< 双向通行开关；0-关，1-开；w3/安智定制版本/fullhanv3-fh865x */
	char cHorizontalNotOpenDoorSwitch;                              /*!< 水平不开门开关；0-关，1-开；w3/安智定制版本/fullhanv3-fh865x */
	int cVersion;                                                  /*!< 电子围栏版本; 0-V1,1-V2 */

    char acReserved[236];
} HY_DVR_ELECTRONIC_FENCE_EX_CFG, *LPHY_DVR_ELECTRONIC_FENCE_EX_CFG;

typedef struct tagHY_DVR_ELECTRONIC_FENCE_VOICEALARM_TIMESLOT_LIST
{
    int iValid;
    HY_DVR_SCHEDULE_TIMESLOT stSchedTimeSlot[SCHEDULE_TIMESLOT_MAX_NUM];     /*!< 布防时间计划 */
    int nChannel;                                                   /*!< 通道号 */

    char szReserved[1020];
}HY_DVR_ELECTRONIC_FENCE_VOICEALARM_TIMESLOT_LIST, *LPHY_DVR_ELECTRONIC_FENCE_VOICEALARM_TIMESLOT_LIST;

/*!
*  @struct    tagHY_DVR_CROSSBORDER_DETECT_CFG_EX
*  @brief    越界侦测拓展配置信息
 */
typedef struct tagHY_DVR_CROSSBORDER_DETECT_CFG_EX
{
    int iValid;                                                                /*!< 本结构体是否有效，0表示无效 */

    int iEnable;                                                    /*!< 是否使能 */
    int iVoiceType; /*!< 0:自定义,1:警报,2:犬吠,3:警戒区域,4:监控区域,5:欢迎光临,6:深水警告,7:私人领地,8:危险区域,9:贵重物品,10:高处危险,
                        11:私家价位,12:叮咚,13:有口罩,14:无口罩,15:注意私人物品,16:抓小偷,17:救命,18:分类,19:可回收垃圾,20:家庭食物垃圾
                        21:危险废物,22:残余垃圾,23:全部分类,24:车形自定义,25:禁止停车 */
    int iBroadcastCount;                                            /*!< 播报次数 */
    int iDayMode;/*!< 0:夜晚模式,1:白天模式,2:不启用此模式（AllDay字段有效，当字段不为disable时，AllDay字段无效）当设置为此模式时，IPC依赖自身传感器判断当前环境是白天还是夜晚*/
    HY_DVR_SCHEDTIME stSchedTime;                                   /*!< 声音报警布防时间 */
	int iBroadcastVolume; /*!< 播报音量 */ 
    int nChannel;                                                            /*!< 通道号 */

    char cReserve[120];
} HY_DVR_CROSSBORDER_DETECT_CFG_EX, *LPHY_DVR_CROSSBORDER_DETECT_CFG_EX;

typedef struct tagHY_DVR_OFFDUTY_DETECT_UI_DESIGN_CFG
{
	int nValid;
	int iEnable;															/*!< 使能 */
	int iSensitivity; 														/*!< 灵敏度，取值范围1~100：其中30为低，60为中，90为高 */
	int iOffDutyTime;														/*!< 离岗时长,时间范围1~600秒 */
	HY_DVR_POINT_V2 stPolygon[MAX_POLYGON_LINE_NUM];						/*!< n个点表示n边形,例:8个点表示8边形 */
	HY_DVR_HANDLEEXCEPTION stHandleType;									/*!< 处理方式 */
	HY_DVR_SCHEDTIME stSchedTime;											/*!< 布防计划 */
	int iWarningFrame;                                                      /*!< 显示警戒框.0-不显示，1-显示 */
    int iTargetFrame;                                                       /*!< 显示目标框.0-不显示，1-显示 */
    int iIsPushBackgroundPicture;                                           /*!< 是否推送背景图.0-不推送，1-推送 */

	char acReserved[52];
} HY_DVR_OFFDUTY_DETECT_UI_DESIGN_CFG, *LPHY_DVR_OFFDUTY_DETECT_UI_DESIGN_CFG;

/*!
*  @struct    tagHY_DVR_OFFDUTY_DETECT_CFG_EX
*  @brief    离岗侦测拓展信息
 */
typedef struct tagHY_DVR_OFFDUTY_DETECT_CFG_EX
{
    int iValid;                                                                /*!< 本结构体是否有效，0表示无效 */

    int iEnable;                                                    /*!< 是否使能 */
    int iVoiceType; /*!< 0:自定义,1:警报,2:犬吠,3:警戒区域,4:监控区域,5:欢迎光临,6:深水警告,7:私人领地,8:危险区域,9:贵重物品,10:高处危险,
                        11:私家价位,12:叮咚,13:有口罩,14:无口罩,15:注意私人物品,16:抓小偷,17:救命,18:分类,19:可回收垃圾,20:家庭食物垃圾
                        21:危险废物,22:残余垃圾,23:全部分类,24:车形自定义,25:禁止停车 */
    int iBroadcastCount;                                            /*!< 播报次数 */
    int iDayMode;/*!< 0:夜晚模式,1:白天模式,2:不启用此模式（AllDay字段有效，当字段不为disable时，AllDay字段无效）当设置为此模式时，IPC依赖自身传感器判断当前环境是白天还是夜晚*/
    HY_DVR_SCHEDTIME stSchedTime;                                    /*!< 声音报警布防时间 */
	int iBroadcastVolume; /*!< 播报音量 */ 

    char cReserve[252];
} HY_DVR_OFFDUTY_DETECT_CFG_EX, *LPHY_DVR_OFFDUTY_DETECT_CFG_EX;

typedef struct tagHY_DVR_OFFDUTY_DETECT_EVENT_TIMESLOT_LIST
{
    int iValid;
    HY_DVR_SCHEDULE_TIMESLOT stSchedTimeSlot[SCHEDULE_TIMESLOT_MAX_NUM]; /*!< 布防时间计划 */
    char szReserved[1024];
}HY_DVR_OFFDUTY_DETECT_EVENT_TIMESLOT_LIST, *LPHY_DVR_OFFDUTY_DETECT_EVENT_TIMESLOT_LIST;

typedef struct tagHY_DVR_OFFDUTY_DETECT_EVENT_VOICEALARM_TIMESLOT_LIST
{
    int iValid;
    HY_DVR_SCHEDULE_TIMESLOT stSchedTimeSlot[SCHEDULE_TIMESLOT_MAX_NUM]; /*!< 布防时间计划 */
    char szReserved[1024];
}HY_DVR_OFFDUTY_DETECT_EVENT_VOICEALARM_TIMESLOT_LIST, *LPHY_DVR_OFFDUTY_DETECT_EVENT_VOICEALARM_TIMESLOT_LIST;    

typedef struct tagHY_DVR_PASSENGERFLOW_STATICS_UI_DESIGN_CFG
{
	int nValid;
	int iEnable;														/*!< 使能 */
	int iSensitivity; 													/*!< 灵敏度，取值范围1~100：其中30为低，60为中，90为高 */
	HY_DVR_POINT_V2 stPolygon[MAX_POLYGON_LINE_NUM];					/*!< 布防 区域，n个点表示n边形。例:8个点表示8边形 */
} HY_DVR_PASSENGERFLOW_STATICS_UI_DESIGN_CFG, *LPHY_DVR_PASSENGERFLOW_STATICS_UI_DESIGN_CFG;

typedef struct tagHY_DVR_PASSENGERFLOW_STATICS_ALARM
{
    int iValid;

    int iBroadcastCount;  /*!< 播放次数 */
    int iBroadcastVolume;  /*!< 播放音量,1-100 */
    int iEnterVoiceEnable; /*!< 进入语音使能 */
    int iEnterVoiceType; /*!< 0:自定义,1:警报,2:犬吠,3:警戒区域,4:监控区域,5:欢迎光临,
                        6:深水警告,7:私人领地,8:危险区域,9:贵重物品,10:高处危险,
                        11:私家价位,12:叮咚,13:有口罩,14:无口罩,15:注意私人物品,
                        16:抓小偷,17:救命,18:分类,19:可回收垃圾,20:家庭食物垃圾
                        21:危险废物,22:残余垃圾,23:全部分类,24:车形自定义,25:禁止停车 */
    int iLeaveVoiceEnable;  /*!< 离开语音使能 */
    int iLeaveVoiceType; /*!< 0:自定义,1:警报,2:犬吠,3:警戒区域,4:监控区域,5:欢迎光临,
                        6:深水警告,7:私人领地,8:危险区域,9:贵重物品,10:高处危险,
                        11:私家价位,12:叮咚,13:有口罩,14:无口罩,15:注意私人物品,
                        16:抓小偷,17:救命,18:分类,19:可回收垃圾,20:家庭食物垃圾
                        21:危险废物,22:残余垃圾,23:全部分类,24:车形自定义,25:禁止停车 */
    HY_DVR_SCHEDTIME struSchedTime;  /*!< 语音报警布防计划 */
    int iDayMode;/*!< 0:夜晚模式,1:白天模式,2:不启用此模式（AllDay字段有效，当字段不为disable时，AllDay字段无效）当设置为此模式时，IPC依赖自身传感器判断当前环境是白天还是夜晚*/

    char acReserved[252];
} HY_DVR_PASSENGERFLOW_STATICS_ALARM, *LPHY_DVR_PASSENGERFLOW_STATICS_ALARM;

typedef struct tagHY_DVR_PASSENGERFLOW_STATICS_LINE
{
	int bValid;
	HY_DVR_POINT_V2 stBeginPoint;											/*!< 起点坐标  */
	HY_DVR_POINT_V2 stEndPoint;												/*!< 终点坐标  */
}HY_DVR_PASSENGERFLOW_STATICS_LINE, *LPHY_DVR_PASSENGERFLOW_STATICS_LINE;

typedef struct tagHY_DVR_PASSENGERFLOW_STATICS_UI_DESIGN_CFG_V1
{
    int iValid;

    int iEnable;       /*!< 使能 */
    int iSensitivity;  /*!< 灵敏度，取值范围1~100：其中30为低，60为中，90为高 */
    int iDetectArea;         /*!< 显示检测区.0-不显示，1-显示 */
    int iTargetFrame;        /*!< 显示目标框.0-不显示，1-显示 */
    int iStatisticalResults; /*!< 显示统计结果.0-不显示，1-显示 */
    HY_DVR_PASSENGERFLOW_STATICS_ALARM stVoiceAlarm; /*!< 语音报警 */
    HY_DVR_POINT_V2 stPolygon[MAX_POLYGON_LINE_NUM];/*!< 布防区域，n个点表示n边形。例:8个点表示8边形 */
	HY_DVR_SCHEDTIME struSchedTime;  /*!< 布防计划 */
    int iIsPushBackgroundPicture; /*!< 是否推送背景图.0-不推送，1-推送 */
	int iWarningLineOrientation;  /*!< 客流统计警戒线检测方向, 0：A->B, 1：B->A; A->B是垂直于线段,若是从上到下或从左到右.B->A应该相反的 */
    HY_DVR_PASSENGERFLOW_STATICS_LINE stWarningLine;/*!< 客流统计警戒线配置 */
	int iDetectMode;		/* 检测模式，默认值0；0-头肩模式，1-人形模式  */

    char acReserved[984];
} HY_DVR_PASSENGERFLOW_STATICS_UI_DESIGN_CFG_V1, *LPHY_DVR_PASSENGERFLOW_STATICS_UI_DESIGN_CFG_V1;

typedef struct tagHY_DVR_WELCOME_GUEST_VOICE
{
    int iValid;

    int iEnable;                              /*!< 使能 */
    int iPlayTime;                            /*!< 播放次数 */
    int iPlayVolume;                          /*!< 播放音量,1-100 */
    char acEnterVoice[128];                   /*!< 客人进店提示音 */
    char acOutVoice[128];                     /*!< 客人出店提示音 */
    HY_DVR_SCHEDTIME struSchedTime;           /*!< 迎宾语音联动输出-布防计划 */

    char szReserved[256];
} HY_DVR_WELCOME_GUEST_VOICE, *LPHY_DVR_WELCOME_GUEST_VOICE;

typedef struct tagHY_DVR_PASSENGERFLOW_STATICS_EX_CFG
{
    int iValid;

    int iEnable;                              /*!< 使能 */
    HY_DVR_WELCOME_GUEST_VOICE stWelcomeVoice;/*!< 迎宾语音 */
    HY_DVR_HANDLEEXCEPTION struHandleType;      /*!< 处理方式  */
    HY_DVR_SCHEDTIME struSchedTime;           /*!< 客流统计时间设置-布防计划 */

    char szReserved[1024];
} HY_DVR_PASSENGERFLOW_STATICS_EX_CFG, *LPHY_DVR_PASSENGERFLOW_STATICS_EX_CFG;

typedef struct tagHY_DVR_PASSENGERFLOW_STATICS_UI_DESIGN_V1_EVENT_TIMESLOT_LIST
{
    int iValid;
    HY_DVR_SCHEDULE_TIMESLOT stSchedTimeSlot[SCHEDULE_TIMESLOT_MAX_NUM]; /*!< 布防时间计划 */
    char szReserved[1024];
}HY_DVR_PASSENGERFLOW_STATICS_UI_DESIGN_V1_EVENT_TIMESLOT_LIST, *LPHY_DVR_PASSENGERFLOW_STATICS_UI_DESIGN_V1_EVENT_TIMESLOT_LIST;


typedef struct tagHY_DVR_PASSENGERFLOW_STATICS_UI_DESIGN_V1_VOICEALARM_TIMESLOT_LIST
{
    int iValid;
    HY_DVR_SCHEDULE_TIMESLOT stSchedTimeSlot[SCHEDULE_TIMESLOT_MAX_NUM]; /*!< 布防时间计划 */
    char szReserved[1024];
}HY_DVR_PASSENGERFLOW_STATICS_UI_DESIGN_V1_VOICEALARM_TIMESLOT_LIST, *LPHY_DVR_PASSENGERFLOW_STATICS_UI_DESIGN_V1_VOICEALARM_TIMESLOT_LIST;

typedef struct tagHY_DVR_PASSENGERFLOW_STATISTICS_HOUR
{
	int nValid;
	int iDepartureNumber; 							/*!< 离开人数 */
	int iEntrantNumber;								/*!< 进入人数 */
} HY_DVR_PASSENGERFLOW_STATISTICS_HOUR, *LPHY_DVR_PASSENGERFLOW_STATISTICS_HOUR;

typedef struct tagHY_DVR_DAILYPASSENGERFLOW_STATISTIC_DAY
{
	int bValid;																			/*!< 本结构体是否有效，0表示无效  */
	HY_DVR_TIME stStatisticDate;														/*!< 统计日期 */
	HY_DVR_PASSENGERFLOW_STATISTICS_HOUR stStatisticHourList[DVR_DAILY_HOUR_NUM];		/*!< 统计日期的客流统计数据 */
} HY_DVR_PASSENGERFLOW_STATISTICS_DAY, *LPHY_DVR_PASSENGERFLOW_STATISTICS_DAY;

typedef struct tagHY_DVR_PASSENGERFLOW_STATISTICS
{
	int bValid;																				/*!< 本结构体是否有效，0表示无效  */
	HY_DVR_TIME stStatisticStartDate;														/*!< 输入参数：统计起始日期 */
	HY_DVR_TIME stStatisticEndDate;															/*!< 输入参数：统计截止日期 */
	HY_DVR_PASSENGERFLOW_STATISTICS_DAY stStatisticDayList[DVR_WEEK_DAY_NUM];				/*!< 输出参数：7天的客流统计数据 */
} HY_DVR_PASSENGERFLOW_STATISTICS, *LPHY_DVR_PASSENGERFLOW_STATISTICS;

typedef struct tagHY_DVR_INTELLIGENT_TRACK_ALARM
{
    int iValid;

    int iVoiceType; /*!< 0:自定义,1:警报,2:犬吠,3:警戒区域,4:监控区域,5:欢迎光临,
                        6:深水警告,7:私人领地,8:危险区域,9:贵重物品,10:高处危险,
                        11:私家价位,12:叮咚,13:有口罩,14:无口罩,15:注意私人物品,
                        16:抓小偷,17:救命,18:分类,19:可回收垃圾,20:家庭食物垃圾
                        21:危险废物,22:残余垃圾,23:全部分类,24:车形自定义,25:禁止停车 */
    int iBroadcastCount; /*!< 播报次数 */
    int iBroadcastVolume;/*!< 播报音量 */
    HY_DVR_SCHEDTIME struSchedTime; /*!< 声音报警布防时间 */

    char szReserved[128];
} HY_DVR_INTELLIGENT_TRACK_ALARM, *LPHY_DVR_INTELLIGENT_TRACK_ALARM;

typedef struct tagHY_DVR_INTELLIGENT_TRACK_CFG
{
    int iValid;

    int iEnable;                                      
    int iTrackTarget;                                  
    HY_DVR_POINT_V2 stPolygon[MAX_POLYGON_LINE_NUM];
	int iSensitivity;      /*!< 灵敏度，取值范围1~100 */
    HY_DVR_INTELLIGENT_TRACK_ALARM stVoiceAlarm;/*!< 语音报警 */
    HY_DVR_SCHEDTIME struSchedTime;       /*!< 布防时间 */
	HY_DVR_HANDLEEXCEPTION struHandleType;  /*!< 处理方式  */
    int nChannel;                           /*!< 通道号 */
	int iTrackAmplifySwitch;                /*!< 跟踪放大开关；1-开，0-关 */

    char strReserved[152];
}HY_DVR_INTELLIGENT_TRACK_CFG, *LPHY_DVR_INTELLIGENT_TRACK_CFG;

typedef struct tagHY_DVR_INTELLIGENT_TRACK_EVENT_TIMESLOT_LIST
{
    int iValid;
    HY_DVR_SCHEDULE_TIMESLOT stSchedTimeSlot[SCHEDULE_TIMESLOT_MAX_NUM]; /*!< 布防时间计划 */
    int nChannel;                           /*!< 通道号 */
    char szReserved[1020];
} HY_DVR_INTELLIGENT_TRACK_EVENT_TIMESLOT_LIST, *LPHY_DVR_INTELLIGENT_TRACK_EVENT_TIMESLOT_LIST;

typedef struct tagHY_DVR_INTELLIGENT_TRACK_VOICEALARM_TIMESLOT_LIST
{
    int iValid;
    HY_DVR_SCHEDULE_TIMESLOT stSchedTimeSlot[SCHEDULE_TIMESLOT_MAX_NUM]; /*!< 布防时间计划 */
    int nChannel;                           /*!< 通道号 */
    char szReserved[1020];
} HY_DVR_INTELLIGENT_TRACK_VOICEALARM_TIMESLOT_LIST, *LPHY_DVR_INTELLIGENT_TRACK_VOICEALARM_TIMESLOT_LIST;

/*!
*  @struct    tagHY_DVR_CRY_SCREAM_ALARM_CFG
*  @brief    哭声叫声声音报警配置信息
 */
typedef struct tagHY_DVR_CRY_SCREAM_ALARM_CFG
{
    int iValid;

    int iEnable;                                                    /*!< 是否使能 */
    int iVoiceType; /*!< 0:自定义,1:警报,2:犬吠,3:警戒区域,4:监控区域,5:欢迎光临,6:深水警告,7:私人领地,8:危险区域,9:贵重物品,10:高处危险,
                        11:私家价位,12:叮咚,13:有口罩,14:无口罩,15:注意私人物品,16:抓小偷,17:救命,18:分类,19:可回收垃圾,20:家庭食物垃圾
                        21:危险废物,22:残余垃圾,23:全部分类,24:车形自定义,25:禁止停车 */
    int iBroadcastCount;                                            /*!< 播报次数,范围1~5次 */
    int iDayMode;/*!< 0:夜晚模式,1:白天模式,2:不启用此模式（AllDay字段有效，当字段不为disable时，AllDay字段无效）当设置为此模式时，IPC依赖自身传感器判断当前环境是白天还是夜晚*/
    HY_DVR_SCHEDTIME struSchedTime;        /*!< 声音报警布防时间 */

    char acReserved[256];
} HY_DVR_CRY_SCREAM_ALARM_CFG, *LPHY_DVR_CRY_SCREAM_ALARM_CFG;

/*!
*  @struct    tagHY_DVR_CRY_SCREAM_DETECT_CFG
*  @brief    哭声叫声配置信息
 */
typedef struct tagHY_DVR_CRY_SCREAM_DETECT_CFG
{
     int iValid;

     int iEnable;                           /*!< 是否处理哭声叫声检测,0-否,1-是 */
     int iSenstive;                         /*!< 哭声叫声检测灵敏度, 1-低,2-中,3-高 */
     HY_DVR_POINT_V2 stPolygon[MAX_POLYGON_LINE_NUM]; /*!< 哭声叫声检测区域 */
     HY_DVR_CRY_SCREAM_ALARM_CFG stVoiceAlarm;/*!< 哭声叫声声音报警配置信息 */
     HY_DVR_HANDLEEXCEPTION struHandleType;   /*!< 哭声叫声检测处理方式 */
     HY_DVR_SCHEDTIME struSchedTime;          /*!< 布防计划 */

     char acReserved[512];
}HY_DVR_CRY_SCREAM_DETECT_CFG, *LPHY_DVR_CRY_SCREAM_DETECT_CFG;

typedef struct tagHY_DVR_AI_FACE_SNAPSHOT_ALARM
{
    int iValid;

    int iEnable;          /*!< 使能,0:关闭,1:启用 */
    int iVoiceType;       /*!< 声音类型,0:自定义,1:警报,2:犬吠,3:警戒区域,4:监控区域,
                          5:欢迎光临,6:深水警告,7:私人领地,8:危险区域,9:贵重物品,
                          10:高处危险,11:私家价位,12:叮咚,13:有口罩,14:无口罩,
                          15:注意私人物品,16:抓小偷,17:救命,18:分类,19:可回收垃圾,
                          20:家庭食物垃圾21:危险废物,22:残余垃圾,23:全部分类,24:车形自定义,
                          25:禁止停车 */
    int iBroadcastCount;  /*!< 播报次数,[1-10] */
    int iBroadcastVolume; /*!< 播报音量,[0-100] */
    int iDayMode;/*!< 0:夜晚模式,1:白天模式,2:不启用此模式（AllDay字段有效，当字段不为disable时，AllDay字段无效）
                        当设置为此模式时，IPC依赖自身传感器判断当前环境是白天还是夜晚 */
    HY_DVR_SCHEDTIME struSchedTime;   /*!< 语音播报布防时间 */

    char szReserved[128];
} HY_DVR_AI_FACE_SNAPSHOT_ALARM, *LPHY_DVR_AI_FACE_SNAPSHOT_ALARM;

typedef struct tagHY_DVR_AI_FACE_SNAPSHOT_CFG
{
    int iValid;

    int iEnable;          /*!< 人脸抓拍使能,0:关闭,1:启用 */
	int iBreathMaskDetectEnable;/*!< 口罩检测使能,0:关闭,1:启用 */
    int iSnapshotMode;    /*!< 抓拍模式,0:质量抓拍模式,1:快速抓拍模式,2:自定义抓拍模式 */
    int iFaceQuality;     /*!< 人脸质量(五个挡位),[20,40,60,80,100] */
    int iShowFaceFrame;   /*!< 显示人脸框,0:关闭,1:启用 */
    int iShowSchedFrame;  /*!< 显示布防框,0:关闭,1:启用 */
    int iIsCaptureBackground;  /*!< 是否抓拍背景图,0:关闭,1:启用 */
    HY_DVR_AI_FACE_SNAPSHOT_ALARM stVoiceAlarm; /*!< 语音报警 */
    HY_DVR_SCHEDTIME struSchedTime;       /*!< 布防时间 */
    HY_DVR_HANDLEEXCEPTION struHandleType;/*!< 联动选项  */
    HY_DVR_POINT_V2 stPolygon[MAX_POLYGON_LINE_NUM];/*!< n个点表示n边形。例:8个点表示8边形 */
	int iMaxPitchAngle;					        /* ?最大俯仰角,范围[0-90]，自定义抓拍模式下有效 */
	int iMaxYawAngle;					        /* ?最大偏航角,范围[0-90]，自定义抓拍模式下有效 */ 
	int iMaxRollAngle;					        /* ?最大滚转角,范围[0-90]，自定义抓拍模式下有效 */ 
	int iMaxAmbiguity;					        /* ?最大模糊度,范围[0-100]，自定义抓拍模式下有效 */ 
	int iMaxFilterDuration;					    /* ?最大筛选时长;范围[0-30000],单位:毫秒(ms)，自定义抓拍模式下有效 */ 
	int iMinFaceSize;					        /* ?最小人脸尺寸,范围[20-500]，自定义抓拍模式下有效 */   
    int iIsPushFaceFeatures;                    /*!< 是否推送人脸特征,0:关闭,1:启用 */
	int iIsRepeatPush;							/*!< 是否重复推送,0:关闭,1:启用 */
	int iRepeatPushInterval;					/*!< 重复推送间隔,1-600s */

    char szReserved[484 - 8];
} HY_DVR_AI_FACE_SNAPSHOT_CFG, *LPHY_DVR_AI_FACE_SNAPSHOT_CFG;


typedef struct tagHY_DVR_AI_FACE_SNAPSHOT_EVENT_TIMESLOT_LIST
{
    int iValid;
    HY_DVR_SCHEDULE_TIMESLOT stSchedTimeSlot[SCHEDULE_TIMESLOT_MAX_NUM]; /*!< 布防时间计划 */
    char szReserved[1024];
}HY_DVR_AI_FACE_SNAPSHOT_EVENT_TIMESLOT_LIST,* LPHY_DVR_AI_FACE_SNAPSHOT_EVENT_TIMESLOT_LIST;

typedef struct tagHY_DVR_AI_FACE_SNAPSHOT_VOICEALARM_TIMESLOT_LIST
{
    int iValid;
    HY_DVR_SCHEDULE_TIMESLOT stSchedTimeSlot[SCHEDULE_TIMESLOT_MAX_NUM]; /*!< 布防时间计划 */
    char szReserved[1024];
}HY_DVR_AI_FACE_SNAPSHOT_VOICEALARM_TIMESLOT_LIST, *LPHY_DVR_AI_FACE_SNAPSHOT_VOICEALARM_TIMESLOT_LIST;


typedef struct tagHY_DVR_AI_BREATHMASK_DETECT_ALARM
{
    int iValid;

    int iEnable;        /*!< 使能,0:关闭,1:启用 */
    int iVoiceType;     /*!< 声音类型,0:自定义,1:警报,2:犬吠,3:警戒区域,4:监控区域,5:欢迎光临,
                        6:深水警告,7:私人领地,8:危险区域,9:贵重物品,10:高处危险,
                        11:私家价位,12:叮咚,13:有口罩,14:无口罩,15:注意私人物品,
                        16:抓小偷,17:救命,18:分类,19:可回收垃圾,20:家庭食物垃圾
                        21:危险废物,22:残余垃圾,23:全部分类,24:车形自定义,25:禁止停车 */
    int iBroadcastCount; /*!< 播报次数,[1-10] */
    int iBroadcastVolume;/*!< 播报音量,[0-100] */
	int iDayMode;/*!< 0:夜晚模式,1:白天模式,2:不启用此模式（AllDay字段有效，当字段不为disable时，AllDay字段无效）
                        当设置为此模式时，IPC依赖自身传感器判断当前环境是白天还是夜晚 */
    HY_DVR_SCHEDTIME struSchedTime; /*!< 语音播报布防时间 */

    char strReserved[128];
}HY_DVR_AI_BREATHMASK_DETECT_ALARM, *LPHY_DVR_AI_BREATHMASK_DETECT_ALARM;

typedef struct tagHY_DVR_AI_BREATHMASK_DETECT_CFG
{
    int iValid;

    int iEnable;        /*!< 口罩检测,0:关闭,1:启用 */
    int iMaskDetectAlarmType;  /*!< 口罩检测报警类型,0:无口罩报警,1:有口罩报警 */
    HY_DVR_AI_BREATHMASK_DETECT_ALARM stVoiceAlarm; /*!< 语音报警 */
    HY_DVR_SCHEDTIME struSchedTime; /*!< 布防时间 */
    HY_DVR_HANDLEEXCEPTION struHandleType;    /*!< 联动选项  */

    char strReserved[256];
}HY_DVR_AI_BREATHMASK_DETECT_CFG, *LPHY_DVR_AI_BREATHMASK_DETECT_CFG;


typedef struct tagHY_DVR_LIGHT_STRIP_CONTROL_CONFIG
{
    char cValid;

    char cEnable;        /*!< 灯带控制,0:关闭,1:启用 */

    char acReserved[16];
}HY_DVR_LIGHT_STRIP_CONTROL_CONFIG, *LPHY_DVR_LIGHT_STRIP_CONTROL_CONFIG;


typedef struct tagHY_DVR_PET_DETECT_CONFIG
{
    int iValid;

    int iEnable;                            /*!< 宠物检测,0:关闭,1:启用 */
    int iSDCardRecord;                      /*!< SD 卡录像使能,0:关闭,1:启用 */
    int iRecordingDuration;                 /*!< 录像时长,单位：秒 */
    int iTracking;                          /*!< 联动跟踪使能,0:关闭,1:启用 */
    int iSnapshot;                          /*!< 联动抓拍使能,0:关闭,1:启用 */
	int iIsRecordSupportDurationCtl;        /*!< 事件录像是否支持时长控制,0:否,1:是 */

    char szReserved[256];
}HY_DVR_PET_DETECT_CONFIG, *LPHY_DVR_PET_DETECT_CONFIG;


typedef struct tagHY_DVR_TEASE_PET_LASER_CONFIG
{
    int iValid;

    int iEnable;                     /*!< 逗宠激光,0:关闭,1:启用 */

    char acReserved[128];
}HY_DVR_TEASE_PET_LASER_CONFIG, *LPHY_DVR_TEASE_PET_LASER_CONFIG;



typedef struct tagHY_DVR_P6SMSG_POLICY_CONFIG
{
    int iValid;

    int iPushMode;                          /*!< 推送模式,0:不带图,1:带图 */
    int iPushStatus;                        /*!< 推送状态,0:关闭,1:启用 */

    char acReserved[256];
}HY_DVR_P6SMSG_POLICY_CONFIG, *LPHY_DVR_P6SMSG_POLICY_CONFIG;


typedef struct tagHY_DVR_PUSH_EVENT_GLOBAL_CFG
{
    char cValid;

    int iChannelID;
    int iEventGlobalInterval;        //事件推送间隔(对所有事件有效),单位:s;其中0-表示关闭推送

    char acReserved[256];
}HY_DVR_PUSH_EVENT_GLOBAL_CFG, *LPHY_DVR_PUSH_EVENT_GLOBAL_CFG;

typedef struct tagHY_DVR_AF_BOARD_UPGRADE_STATUS
{
    int iValid;
 
    char  cStatus;                                      // 0-空闲、1-升级中、2-升级失败、3-升级成功、4-升级超时
 
    char  acReserved[256];
}HY_DVR_AF_BOARD_UPGRADE_STATUS, *LPHY_DVR_AF_BOARD_UPGRADE_STATUS;

typedef struct tagHY_DVR_NTP_SYNCHRONIZE_NOW_INFO
{
    int iValid;                                                                     /*!< 本结构体是否有效，0表示无效  */
 
    char acLastSuccessSyncNTPTime[32];              //ntp上次成功同步时间，只读；格式："YYYY/MM/DD hh:mm:ss"
    char cIsSynchronizeNTPNow;                      //是否立即同步NTP,只写；1:同步,0:不同步
    char cSynchronizeNTPNowStatus;                  //立即同步NTP状态,1:成功,0:失败
 
   char    acReserved[128];                        //保留(方便日后扩展)
}HY_DVR_NTP_SYNCHRONIZE_NOW_INFO, *LPHY_DVR_NTP_SYNCHRONIZE_NOW_INFO;

// 逗宠运动
typedef enum eTeasePetAction
{
    TeasePetMotion_BigRing = 1,                     // 大圆环
    TeasePetMotion_Straight = 2,                    // 直线式
    TeasePetMotion_RandomWalk = 3,                  // 随机漫步
    TeasePetMotion_DuckStroll = 4,                  // 鸭子漫步
    TeasePetMotion_RockingInPlace = 5,              // 原地摇摆
    TeasePetMotion_StepByStepPremise = 6,           // 步步前提
    TeasePetMotion_FigureEight = 7,                 // 8字形
    TeasePetMotion_Wandering = 8,                   // 徘徊式
    TeasePetMotion_SnakeWanders = 9,                // 小蛇游走
    TeasePetMotion_ZShapedTravel = 10,              // Z字行进
    TeasePetMotion_WalkAndPlay = 11,                // 边走边玩
    TeasePetMotion_AttackFromAllSides = 12,         // 四面出击

    TeasePetVoice_EatFood = 1000,                   // 大口吃粮
    TeasePetVoice_DuckToy,                          // 鸭子玩具
    TeasePetVoice_OpenCan,                          // 开罐头啦
    TeasePetVoice_CallingCat,                       // 呼唤喵宝
    TeasePetVoice_KittenMeow,                       // 小猫叫声
    TeasePetVoice_Snoring,                          // 呼噜呼噜
    TeasePetVoice_ComeForDinner,                    // 来开饭啦
    TeasePetVoice_CuteCatMeows,                     // 萌猫萌鸣
}eTeasePetAction;

typedef struct tagHY_DVR_TEASE_PET_ACTION_CONFIG
{
    int iValid;

    int iAction;                  /*!< 逗宠动作,参考TeasePetAction枚举定义 */

    char szReserved[128];
}HY_DVR_TEASE_PET_ACTION_CONFIG, *LPHY_DVR_TEASE_PET_ACTION_CONFIG;

typedef struct tagHY_DVR_AI_FACE_ALGORITHM_VERSION
{
    int iValid;

    char acFeatureVersion[32];  /*!< 特征值版本  */
    char acManufacturer[16];    /*!< 厂商  */
    char acPlatform[16];        /*!< 平台  */

    char szReserved[128];
}HY_DVR_AI_FACE_ALGORITHM_VERSION, *LPHY_DVR_AI_FACE_ALGORITHM_VERSION;


typedef struct tagHY_DVR_REGISTER_NVR_INFO
{
    int bValid;                                       /*!< 本结构体是否有效，0表示无效 */
    int bCmsEnable;                                   /*!< 0-不启用管理服务器, 1-启用 */
    char szManageHostAddress[DVR_MAX_DOMAINNAME_LEN]; /*!< 远程管理主机地址，长度考虑域名 */
    unsigned short wManageHostPort;                   /*!< 远程管理主机端口 */
    int nAutoRegIntervals;                            /*!< 注册间隔时间，单位为秒 */

    char szReserved[128];
}HY_DVR_REGISTER_NVR_INFO,*LPHY_DVR_REGISTER_NVR_INFO;

typedef struct tagHY_DVR_P2P_SERVERINFO
{
    int bValid;                                       	// 本结构体是否有效，0表示无效 
	char serverIP[18]; 									//服务器IP
	int keepAliveInterval;								// 唤醒服务器保活时间默认180s,当使用xrs唤醒服务器时才会传此项，单位s
	int nport;											// 唤醒服务器端口，当使用xrs唤醒服务器需要使用此端口，使用尚云服务时不需要此值
    char cDomain[64];
	char szReserved[128 - 4 - 4 - 64];					
}HY_DVR_P2P_SERVERINFO,*LPHY_DVR_P2P_SERVERINFO;

typedef struct tagHY_DVR_REGISTER_STATE
{
    int bValid;    /*!< 本结构体是否有效，0表示无效 */
    int nCmsState; /*!< 注册状态 */

    char szReserved[128];
} HY_DVR_REGISTER_STATE, *LPHY_DVR_REGISTER_STATE;


typedef enum
{
	DEVICE_CAP_BEGIN 						= 0,		//起始值
	DEVICE_CAP_FENCE_DETECT 		        = 1,		//电子围栏 FenceDetect
	DEVICE_CAP_CROSS_BORDER 	            = 2,		//越界检测 CrossBorder
	DEVICE_CAP_OFF_DUTY 		            = 3,		//离岗检测 OffDuty
	DEVICE_CAP_PASSENGER_FLOW 		        = 4,		//客流统计(人形模型) PassengerFlow
	DEVICE_CAP_INTELLIGENCE_TRACK 			= 5,		//目标跟踪 IntelligenceTrack
	DEVICE_CAP_FACE_SNAPSHOT 			    = 6,		//人脸抓拍 FaceSnapshot
	DEVICE_CAP_FACE_RECO 				    = 7,		//人脸识别 FaceReco
	DEVICE_CAP_CAR_LICENSE_SNAPSHOT			= 8,		//车牌抓拍 CarLicenseSnapshot
    DEVICE_CAP_PARKING_SPACE                = 9,        //车位事件 ParkingSpace
	DEVICE_CAP_TRAFFIC_STATISTICS           = 10,       //车流统计 TrafficStatistics
	DEVICE_CAP_FIRE_DETECT           		= 11,       //火焰检测 FireDetect
	DEVICE_CAP_EBIKE_DETECT           		= 12,       //电瓶车检测 EBikeDetect
	DEVICE_CAP_GAS_CONTAINER_DETECT         = 13,   	//煤气罐检测 GasContainerDetect
	DEVICE_CAP_LICENSE_PLATE_SURVEILLANCE   = 14,       //车牌布控 LicensePlateSurveillance
	DEVICE_CAP_FENCE_DETECT_V2				= 15,       //电子围栏V2 (机动车/非机动车目标) FenceDetectV2
	DEVICE_CAP_PEOPLE_STATISTICS			= 16,       //人数统计 PeopleStatistics
	DEVICE_CAP_PASSENGER_FLOW_FOR_HEAD_AND_SHOULDER				= 17,       //客流统计(头肩模型) PassengerFlowForHeadAndShoulder
	DEVICE_CAP_FENCE_DETECT_V2_NO_MOTOR_VEHICLE					= 18,       //电子围栏V2非机动车目标

	DEVICE_CAP_720P           				= 51,       //720P
	DEVICE_CAP_1080P           				= 52,       //1080P
	DEVICE_CAP_3MP           				= 53,       //3MP
	DEVICE_CAP_4MP           				= 54,       //4MP
	DEVICE_CAP_5MP           				= 55,       //5MP
	DEVICE_CAP_WDR_OPEN           			= 56,       //WDR开
	DEVICE_CAP_WDR_CLOSE           			= 57,       //WDR关
	DEVICE_CAP_END,								//结束值
} DEVICE_CAPABILITY_INFO;


typedef struct tagHY_DVR_DEVICE_CAPABILITY
{
    int bValid;                                 /*!< 本结构体是否有效，0表示无效 */
    DEVICE_CAPABILITY_INFO eDeviceCap;          /*!< 设备功能   电子围栏 : FenceDetect; 越界侦测 : CrossBorder; 离岗侦测 : OffDuty; 
                                                    客流统计 : PassengerFlow; 目标跟踪 : IntelligenceTrack; 人脸抓拍 : FaceSnapshot; 
                                                    人脸识别 : FaceReco; 车牌抓拍 : CarLicenseSnapshot; 车位事件 ParkingSpace;
													车流统计: TrafficStatistics */
    int bIsNeedReboot;                          /*!< 打开此功能后是否会重启 */
	int iSupplementary;							/*!< 输出参数，补充说明字段(最初始的是用bIsNeedReboot里面是true或者false，为true提示),对新增的提示语，
													用该字段的值对应不同的提示语：
													0-表示该字段不生效
													1-"开启此功能将自动关闭人脸抓拍、人脸识别功能，摄像机将自动重启", T5AI_V2.5.0火焰支持 以及功能优化[IPC2400093]项目新增以下场景：
													  a.如果当前能力里面支持  [人脸抓拍/识别] [火焰检测],提示  "开启此功能将自动关闭人脸抓拍、人脸识别、火焰检测功能，摄像机将自动重启"
													  b.如果当前能力里面支持  [人脸抓拍/识别],提示  "开启此功能将自动关闭人脸抓拍、人脸识别功能，摄像机将自动重启"
													  c.如果当前能力里面支持  [人脸抓拍/识别],提示  "开启此功能将自动关闭人脸抓拍、人脸识别功能，摄像机将自动重启"
													2-"切换此功能，摄像机将自动重启"
													3-"不支持开启此功能" */

    char szReserved[124];
} HY_DVR_DEVICE_CAPABILITY, *LPHY_DVR_DEVICE_CAPABILITY;



typedef struct tagHY_DVR_DEVICE_DIGITAL_MULTIPLE
{
    int bValid;                                 /*!< 本结构体是否有效，0表示无效 */
    int nDeviceDigitalMultiple;                 /*!< 配置内置云台的数字变倍倍数（整形值） */
	double dDeviceDigitalMultiple;              /*!< 配置内置云台的数字变倍倍数(浮点) */
    int iDeviceDigitalMultipleType;             /*!< 数字变倍倍数类型,当前数字变倍倍数是使用小数还是是整数; 0-整数,1-小数(浮点) */
	float fIPCSupportedBoardDigitalMaxMultiple;	/*!< IPC支持的机芯数字变倍最大倍数；范围是1~10 */
 	float fCustomBoardDigitalMaxMultiple;		/*!< 定制的机芯数字变倍最大倍数（小于等于cIPCSupportedBoardDigitalMaxMultiple） */

    char szReserved[108];
} HY_DVR_DEVICE_DIGITAL_MULTIPLE, *LPHY_DVR_DEVICE_DIGITAL_MULTIPLE;



typedef struct tagHY_DVR_P6SEVENT_MQTT_CONFIG
{
    int bValid;                                                         /*!< 本结构体是否有效，0表示无效 */
    int bEnable;                                                        /*!< MQTT 是否启用，0 - 不启用; 1 - 启用 */
    char strDeviceID[HY_DVR_MAX_MQTT_ID_LENGHT];                        /*!< 设备 ID 号 */
    char strAddress[DVR_MAX_IP_STR_LEN];                                /*!< 服务器地址 */
    int nPort;                                                          /*!< 服务器端口号 */
    char strUserName[DVR_MAX_USERNAME_LEN];                             /*!< 用户名 */
    char strPassword[DVR_MAX_PASSWORD_LEN];                             /*!< 密码 */
    int nServerConnectState;                                            /*!< 服务器连接状态; 0，初始化状态 1.成功，2，用户名或密码错误，3，ID冲突，4.网络异常，5.服务器拒绝，6.连接服务器超时，7.域名解析失败 0xff.其他未知错误 */
    char strMqttMainTopic[HY_DVR_MAX_MQTT_TOPIC_LENGTH];	            /*!< 主 topic，只读，可以用来获取和设置  		 		  */
	char strMqttOperateDeviceTopic[HY_DVR_MAX_MQTT_TOPIC_LENGTH];       /*!< 设备操作 topic，只读，用来显示  					  */
	char strMqttPublishEventTopic[HY_DVR_MAX_MQTT_TOPIC_LENGTH];        /*!< 事件发布 topic，只读，用来显示  		     		  */
    int nIsSupportAddressV2;                                            /*!< 是否支持 V2 版本服务器地址， 0 - 不支持； 1 - 支持    */
    char strAddressV2[DVR_MAX_DOMAIN_STR_LEN];                          /*!< 服务器地址 */
	char cIsConfigReadOnly;                                             /*!< 配置是否只读(丁丁地锁定制版本使用); 0-否，1-是 */
	char cCacheEventEnable;                                             /*!< 是否缓存事件开关; 0-否，1-是 */

    char szReserved[256 - 4 - 128 - 1 - 1];
} HY_DVR_P6SEVENT_MQTT_CONFIG, *LPHY_DVR_P6SEVENT_MQTT_CONFIG;



typedef struct tagHY_DVR_WHITE_LIGHT_ENABLE
{
    int bValid;                                                         /*!< 本结构体是否有效，0表示无效 */
    int nEnable;                                                        /*!< 白光灯是否开启。0 - 关闭； 1 - 开启 */

    char szReserved[32];
} HY_DVR_WHITE_LIGHT_ENABLE, *LPHY_DVR_WHITE_LIGHT_ENABLE;



typedef struct tagHY_DVR_VOICE_BROADCAST_LANGUAGE
{
    int bValid;

	char strLanguage[64];

	char cReserved[32];
} HY_DVR_VOICE_BROADCAST_LANGUAGE, *LPHY_DVR_VOICE_BROADCAST_LANGUAGE;


typedef struct tagHY_DVR_IS_AOV_MODE
{
    int bValid;                                                         /*!< 本结构体是否有效，0表示无效 */
    int bIsAOVMode;                                                     /*!< 是否是 AOV 模式。0 - 否； 1 - 是 */

    char szReserved[32];
} HY_DVR_IS_AOV_MODE, *LPHY_DVR_IS_AOV_MODE;


typedef struct tagHY_DVR_INDICATOR_LIGHT_CFG
{
    int bValid;                                                         /*!< 本结构体是否有效，0表示无效 */
    int bEnable;                                                        /*!< 是否开启指示灯。0 - 关闭； 1 - 开启 */

    char szReserved[128];
} HY_DVR_INDICATOR_LIGHT_CFG, *LPHY_DVR_INDICATOR_LIGHT_CFG;


typedef struct tagHY_DVR_SENSITIVITY_CFG
{
    int bValid;                                                         /*!< 本结构体是否有效，0表示无效 */
    int nSensitivity;                                                   /*!< 灵敏度数值， 0 - 100 */
    char strSensitivity[16];                                            /*!< 灵敏度等级，lowest, low, middle, high, highest */

    char szReserved[128];
} HY_DVR_SENSITIVITY_CFG, *LPHY_DVR_SENSITIVITY_CFG;


typedef struct tagHY_DVR_SENSITIVITY_CFG_LIST
{
    int bValid;                                                         /*!< 本结构体是否有效，0表示无效 */
    HY_DVR_SENSITIVITY_CFG stSensitivity[10];

    char szReserved[128];
} HY_DVR_SENSITIVITY_CFG_LIST, *LPHY_DVR_SENSITIVITY_CFG_LIST;

/*!
*  @struct  tagHY_DVR_DEFENCE_FRAME_GB28181_YUZUN
*  @brief   布防框配置（禹尊定制-GB28181协议-布防框）
 */
typedef struct tagHY_DVR_DEFENCE_FRAME_GB28181_YUZUN
{
    int iVaild;                     /*!< 本结构体是否有效，0表示无效 */

    int iEanble;                      /*!< 是否使能;0-去使能,1-使能 */
	int iIsShowWaringFrame;           /*!< 是否显示警戒框;0-不显示,1-显示 */
    HY_DVR_POINT_V2 stPolygon[MAX_POLYGON_LINE_NUM];      /*!< n个点表示n边形。例:8个点表示8边形,GB28181禹尊定制默认只用4个点 */

    char acReserver[128];
}HY_DVR_DEFENCE_FRAME_GB28181_YUZUN;


typedef struct tagHY_DVR_HOLOLINK_ACTIVE_STATUS
{
    int bValid;                                                         /*!< 本结构体是否有效，0表示无效 */
    int nActiveStatus;                                                  /*!< 激活状态，0 - 未激活， 1 - 已激活 */

    char szReserved[128];
} HY_DVR_HOLOLINK_ACTIVE_STATUS, *LPHY_DVR_HOLOLINK_ACTIVE_STATUS;


// 蓝牙地锁配置相关结构体
typedef struct tagHY_DVR_BT_BASIC_CFG
{
    int bValid;                                                         /*!< 本结构体是否有效，0表示无效 */
    char strChargingStationNum[32];                                     /*!< 充电场站编号 */
    char strDeviceName[32];                                             /*!< 设备名称 */
    int nParkingSpaceMode;                                              /*!< 车位模式  0 - 垂直； 1 - 水平 */

    int nEnable;                                                        /*!< 车位模式使能 */

    char szReserved[128 - 4];
} HY_DVR_BT_BASIC_CFG, *LPHY_DVR_BT_BASIC_CFG;


typedef struct tagHY_DVR_BT_DEVICE_INFO
{
    int bValid;                                                         /*!< 本结构体是否有效，0表示无效 */
    char strDeviceName[32];                                             /*!< 设备名称 */
    int nDeviceStatus;                                                  /*!< 设备状态，0 - 落下； 1 - 升起 */
    char strMAC[32];                                                    /*!< 设备 MAC 地址 */
    char strUUID[64];                                                   /*!< 设备 UUID */

    char szReserved[256];
} HY_DVR_BT_DEVICE_INFO, *LPHY_DVR_BT_DEVICE_INFO;


typedef struct tagHY_DVR_BT_DEVICE_LIST
{
    int bValid;                                                         /*!< 本结构体是否有效，0表示无效 */
    HY_DVR_BT_DEVICE_INFO stDeviceList[3];                              /*!< 设备信息 */

    char szReserved[128];
} HY_DVR_BT_DEVICE_LIST, *LPHY_DVR_BT_DEVICE_LIST;



typedef struct tagHY_DVR_BT_GROUND_LOCK_CTRL_STATUS
{
    int bValid;                                                         /*!< 本结构体是否有效，0表示无效 */

    int nGroundLockCmd;                                                 /*!< 地锁控制命令，0 - 落下； 1 - 升起； 2 - 获取地锁状态 */
    char strGroundLockID[32];                                           /*!< 地锁 ID */
    int nGroundLockStatus;                                              /*!< 地锁状态，0 - 落下； 1 - 升起 */

    char szReserved[128];
} HY_DVR_BT_GROUND_LOCK_CTRL_STATUS, *LPHY_DVR_BT_GROUND_LOCK_CTRL_STATUS;


typedef struct tagHY_DVR_BT_PARKING_SPACE_INFO
{
    int bValid;                                                         /*!< 本结构体是否有效，0表示无效 */
    int nEnable;                                                        /*!< 是否启用，0 - 未启用， 1 - 启用 */
    char strParkingSpaceName[32];                                       /*!< 车位名字 */
    char strGroundLockID[32];                                           /*!< 地锁 ID */
    char strGroundLockKey[32];                                          /*!< 地锁 Key */
    char strMAC[32];                                                    /*!< 设备 MAC 地址 */

    char szReserved[256];
} HY_DVR_BT_PARKING_SPACE_INFO, *LPHY_DVR_BT_PARKING_SPACE_INFO;


typedef struct tagHY_DVR_BT_PARKING_SPACE_LIST
{
    int bValid;                                                         /*!< 本结构体是否有效，0表示无效 */
    HY_DVR_BT_PARKING_SPACE_INFO stLeftParkingSpaceInfo;                /*!< 左车位信息 */
    HY_DVR_BT_PARKING_SPACE_INFO stRightParkingSpaceInfo;               /*!< 右车位信息 */

    char szReserved[1024];
} HY_DVR_BT_PARKING_SPACE_LIST, *LPHY_DVR_BT_PARKING_SPACE_LIST;



typedef struct tagHY_DVR_DEVICE_POWER_MANAGE_CFG
{
    int bValid;                                                         /*!< 本结构体是否有效，0表示无效 */
    int nWorkMode;                                                      /*!< 是否启用，0 - 节能模式， 1 - 性能模式， 2 - 自定义模式，3-智能模式 */
    int nEnergyConserThre;                                              /*!< 切换全时录像模式电量阈值， 10 ~ 50 */
    int nDeviceWorkMode;                                                /*!< 设备工作模式， 0 - 混电模式； 1 - 低功耗模式； 2 - 省电模式； 3 - 常电模式； 4 - AOV 模式 */
    int nAovSleepCycle;                                                 /*!< AOV下休眠周期（单位：秒，nWorkMode为自定义模式下生效）*/
	int nSuperPowerSaveModeEnable;										/*!< 超省电模式使能；0-去使能，1-使能 */

    char szReserved[120 - 4];
} HY_DVR_DEVICE_POWER_MANAGE_CFG, *LPHY_DVR_DEVICE_POWER_MANAGE_CFG;

/*!
*  @struct    tagHY_DVR_DEVICE_POWER_MANAGE_CFG_V2
*  @brief		设备工作模式V2结构体
*  @brief		首次对接的设备型号: E3-52PGM-S/W/BT
 */
typedef struct tagHY_DVR_DEVICE_POWER_MANAGE_CFG_V2
{
    int bValid;															/*!< 本结构体是否有效，0表示无效 */

    int nWorkMode;														/*!< 工作模式，0 – 省电模式，1 – 低功耗模式，2 – AOV模式 3 – 智能模式 （后续需要其他模式直接向后扩展，不要增加新字段） */
    int nEnergyConserThre;                                              /*!< 电量阈值，用于控制设备端依赖电量的功能（云台、喇叭等外设），根据阈值判断是否工作，取值范围[0-100]，
																			这里E3项目暂时未用到，E3当前阈值固定为20，此处保留为后续若需要可修改准备。
																			电量阈值是否使用只与设备有关，与工作模式无关。*/
    int nAovSleepCycle;                                                 /*!< AOV下休眠周期（单位：秒，WorkMode为AOV模式下生效）*/

    char szReserved[256];
} HY_DVR_DEVICE_POWER_MANAGE_CFG_V2, *LPHY_DVR_DEVICE_POWER_MANAGE_CFG_V2;

typedef struct tagHY_DVR_DEVICE_RUNNING_INFO
{
    int iValid;                                                         /*!< 本结构体是否有效，0表示无效 */
 
    char iIntradayWeekBatteryPower[DVR_WEEK_DAY_NUM + 1][DVR_DAILY_HOUR_NUM];        /*!< 记录当天和最近一周的电池电量；取值[0-100],一维数组索引[0-6]表示最近一周从周一到周日的电池电量可循环覆盖，索引7表示当天的电池电量；电池电量每小时记录一次，无记录的小时点用0填充，记录以天方式循环覆盖； */
    char iIntradayWeekSignalIntensity[DVR_WEEK_DAY_NUM + 1][DVR_DAILY_HOUR_NUM];     /*!< 记录当天和最近一周的设备信号强度；取值[0-100],一维数组索引[0-6]表示最近一周从周一到周日的信号强度可循环覆盖，索引7表示当天的信号强度； 信号强度每小时记录一次，无记录的小时点用前面的信号强度填充，记录以天方式循环覆盖 */
    int iIntradayWeekPreviewDuration[DVR_WEEK_DAY_NUM + 1];                        /*!< 记录当天和最近一周的预览时间(总时长)，从0开始递增，单位：秒；数组索引[0-6]表示最近一周从周一到周日的预览时间，预览时间每天记录一次，记录以天方式循环覆盖 */
    int iIntradayWeekAwakenDuration[DVR_WEEK_DAY_NUM + 1];                         /*!< 记录当天和最近一周的唤醒时间(总时长)，从0开始递增，单位：秒，数组索引[0-6]表示最近一周从周一到周日的唤醒时间，唤醒时间每天记录一次，记录以天方式循环覆盖 */
    int iIntradayWeekAlarmCount[DVR_WEEK_DAY_NUM + 1];                             /*!< 记录当天和最近一周的报警次数(事件上报次数)，从0开始递增，数组索引[0-6]表示最近一周从周一到周日的报警次数，报警次数每天记录一次，记录以天方式循环覆盖 */
    int iIntradayWeekPeopleDetectCount[DVR_WEEK_DAY_NUM + 1];                    /*!< 记录当天和最近一周的人形检测次数(事件上报次数)，从0开始递增，数组索引[0-6]表示最近一周从周一到周日的报警次数，报警次数每天记录一次，记录以天方式循环覆盖 */

    char szReserved[1024 - 32];
} HY_DVR_DEVICE_RUNNING_INFO, *LPHY_DVR_DEVICE_RUNNING_INFO;


typedef struct tagHY_DVR_SIM_CARD_INFO
{
    int bValid;                                                         /*!< 本结构体是否有效，0表示无效 */
    int nIsInUse;                                                       /*!< 是否在使用中，0 - 否， 1 - 是 */
    int nCardType;                                                      /*!< 卡类型，0，内置卡，1，外置卡 */
    char szOperator[64];			                                    /*!< 运营商 */
    int nSignalStrength;                                                /*!< 信号强度 */
    char szICCID[32];				                                    /*!< ICCID */
    char szIMEI[16];
	char sz4GModuleVersion[128];	                                    /*!< 4G模组版本号 */
	int iCardWorkMode;                                                  /*!< 卡工作模式，0：实体卡模式 1：云卡模式 */
	int isDirect;														// 0,该字段无效，适配以前的设备；1,定向流量卡；2，非定向流量卡
	int nRegisterStatus;                                                //注册状态0,表示成功，1，表示锁住，2，表示失败
	char szIMSI[16];
	char szACT[64];
   
    char szReserved[508 - 4 - 84];
} HY_DVR_SIM_CARD_INFO, *LPHY_DVR_SIM_CARD_INFO;

typedef struct tagHY_DVR_SIM_CARD_INFO_LIST
{
    int bValid;                                                         /*!< 本结构体是否有效，0表示无效 */
    HY_DVR_SIM_CARD_INFO stSIMCardInfo[2];

    char szReserved[512];
} HY_DVR_SIM_CARD_INFO_LIST, *LPHY_DVR_SIM_CARD_INFO_LIST;



typedef struct tagHY_DVR_NIGHT_VISION_CFG
{
    int bValid;                                                         /*!< 本结构体是否有效，0表示无效 */
    int nNightVisionMode;                                               /*!< 夜视设置， 0 - 自动模式； 1 - 定时模式； 2 - 关闭补光 */
    char szStartTime[32];                                               /*!< 开灯开始时间，仅在定时模式下生效; 格式hh:mm,24小时制 */
    char szEndTime[32];                                                 /*!< 开灯结束时间，仅在定时模式下生效; 格式hh:mm,24小时制 */
	int iFillLightBrightness;                                           /*!< 补光灯亮度，设置范围10%~50%，默认20% */

    char szReserved[124];
} HY_DVR_NIGHT_VISION_CFG, *LPHY_DVR_NIGHT_VISION_CFG;



typedef struct tagHY_DVR_PARKING_SPACE_CFG
{
    int bValid;                                                         /*!< 本结构体是否有效，0表示无效 */
    int nEnable;                                                        /*!< 是否启用，0 - 未启用， 1 - 启用 */
    char strParkingSpaceName[32];                                       /*!< 车位名字 */
    int nParkingSpaceNum;                                               /*!< 车位序号 */
    HY_DVR_POINT_V2 stPolygon[MAX_POLYGON_LINE_NUM];				    /*!< n个点表示n边形。例:8个点表示8边形 */
    char strGroundLockID[32];                                           /*!< 地锁 ID */

    char szReserved[512 - 32];
} HY_DVR_PARKING_SPACE_CFG, *LPHY_DVR_PARKING_SPACE_CFG;

typedef struct tagHY_DVR_PARKING_SPACE_LIST
{
    int bValid;                                                         /*!< 本结构体是否有效，0表示无效 */
    int nEnable;                                                        /*!< 车位功能使能 */
    HY_DVR_PARKING_SPACE_CFG struParkingSpaceList[2];
    int iShowSchedFrame;           									/*!< 显示布防框.0-不显示，1-显示 */
    int iCarFrame;            									    /*!< 显示车型框.0-不显示，1-显示 */
    int iCarPlateFrame;            									/*!< 显示车牌框.0-不显示，1-显示 */
    int iDebugInfo;            									    /*!< 显示调试信息.0-不显示，1-显示 */

    char szReserved[2048 - 16];
} HY_DVR_PARKING_SPACE_LIST, *LPHY_DVR_PARKING_SPACE_LIST;


typedef struct tagHY_DVR_GROUND_LOCK_CTRL_STATUS
{
    int bValid;                                                         /*!< 本结构体是否有效，0表示无效 */

    int nGroundLockCmd;                                                 /*!< 地锁控制命令，0 - 落下； 1 - 升起； 2 - 获取地锁状态 */
    char strParkingSpaceName[32];                                       /*!< 车位名字 */
    int nParkingSpaceNum;                                               /*!< 车位序号 */
    int nForce;                                                         /*!< 是否强制操作（升降锁） */

    char szReserved[124];
} HY_DVR_GROUND_LOCK_CTRL_STATUS, *LPHY_DVR_GROUND_LOCK_CTRL_STATUS;


typedef struct tagHY_DVR_GROUND_LOCK_CTRL_V2_OPT_RESULT
{
	int iValid;															/*!< 本结构体是否有效，0表示无效 */

	int iOperationResult;												/*!< 地锁控制操作结果；0-初始化状态，1-操作中，2-操作异常，3-操作完成 */

    char acReserved[128];
} HY_DVR_GROUND_LOCK_CTRL_V2_OPT_RESULT, *LPHY_DVR_GROUND_LOCK_CTRL_V2_OPT_RESULT;


typedef struct tagHY_DVR_LOCAL_CAR_PLATE_WHITE_LIST
{
    char strCarPlate[16];                       /*!< 车牌号 */    
    int nOption;                                /*!< 处理方式 0 表示不做处理，1表示增加，2表示删除 */

    char szReserved[16];

} HY_DVR_LOCAL_CAR_PLATE_WHITE_LIST, *LPHY_DVR_LOCAL_CAR_PLATE_WHITE_LIST;


typedef struct tagHY_DVR_GROUND_LOCK_LIFT_STRATEGY
{
    int bValid;                                                         /*!< 本结构体是否有效，0表示无效 */

    int nHeartbeatInterval;                                            /*!< 心跳上报频率间隔（秒），默认 15s，范围：15-180   */
    int nBearOffLiftLockDuration;                                      /*!< 停车离开后升锁时长（秒）,车辆驶离后，设定多少秒执行升锁，默认 20，范围0-60   */
    int nLockEnabledDate0;                                             /*!< 地锁启用时间段(0 点-8点) 。地锁启用时间段，每半个小时为 1 个单元，每 8 小时为一组；启用：升起地锁，停用：降下地锁示例：65535 转 2 进制为1111111111111111，则表示0 点-8 点为全开启?   */
    int nLockEnabledDate1;                                             /*!< 同上   */
    int nLockEnabledDate2;                                             /*!< 同上   */
    HY_DVR_LOCAL_CAR_PLATE_WHITE_LIST* stLocalWhiteList;               /*!< 本地车牌白名单   */
    int nLocalWhiteNum;                                                /*!< 下发携带的车牌数量   */
    int nAuthMode;                                                     /*!< 鉴权模式: 0 - 自动模式（默认）（摄像头判断）;1 - 手动模式（除了本地的白名单车牌之外，每个都要向平台鉴权）; 2 - 特殊规则（除了本地的白名单车牌之外，只鉴权蓝牌）?   */
    int nTaskPhotosDuration;                                           /*!< 定时抓拍间隔（单位：秒，最小值：10）?   */
    int nBuzzer;                                                       /*!< 蜂鸣器（0关闭，1开启）?   */

    char szReserved[512];
} HY_DVR_GROUND_LOCK_LIFT_STRATEGY, *LPHY_DVR_GROUND_LOCK_LIFT_STRATEGY;


typedef struct tagHY_DVR_DO_NOT_DISTURB_MODE_CFG
{
    int bValid;                                                         /*!< 本结构体是否有效，0表示无效 */

    int nEnable;                                                        /*!< 是否开启勿扰模式。0 - 否， 1 - 是 */
    int nDuration;                                                      /*!< 勿扰模式持续时间（单位：分钟） */

    char szReserved[64];

} HY_DVR_DO_NOT_DISTURB_MODE_CFG, *LPHY_DVR_DO_NOT_DISTURB_MODE_CFG;



typedef struct tagHY_DVR_PTZ_CRUISE_TIME_SLOT
{
    int iValid;
    HY_DVR_SCHEDULE_TIMESLOT stSchedTimeSlot[SCHEDULE_TIMESLOT_MAX_NUM]; /*!< 布防时间计划 */
    char szReserved[2048];                      // 中间层建议保留字段给大一点，后面可能需要扩展
} HY_DVR_PTZ_CRUISE_TIME_SLOT, *LPHY_DVR_PTZ_CRUISE_TIME_SLOT;


typedef struct tagHY_DVR_PTZ_TRACK_TIME_SLOT
{
    int iValid;
    HY_DVR_SCHEDULE_TIMESLOT stSchedTimeSlot[SCHEDULE_TIMESLOT_MAX_NUM]; /*!< 布防时间计划 */

    char szReserved[2048];                      // 中间层建议保留字段给大一点，后面可能需要扩展
} HY_DVR_PTZ_TRACK_TIME_SLOT, *LPHY_DVR_PTZ_TRACK_TIME_SLOT;



typedef struct tagHY_DVR_DDLOCK_ACCESS_CAR_CFG
{
    int bValid;                                                         /*!< 本结构体是否有效，0表示无效 */

    int nCarType[32];                                                       /*!< 1 - 蓝牌；2 - 黄牌；3 - 黑牌；4 - 新能源；
                                                                             5 - 教练车；6 - 警车；7 - 武警车；8 - 军车；
                                                                             9 - 港澳；10 - 领馆车；11 - 使馆车；12 - 民航；
                                                                             13 - 应急；14 - 无牌车；  */

    char szReserved[64];

} HY_DVR_DDLOCK_ACCESS_CAR_CFG, *LPHY_DVR_DDLOCK_ACCESS_CAR_CFG;



typedef struct tagHY_DVR_DDLOCK_AUTH_MODE_CFG
{
    int bValid;                                                         /*!< 本结构体是否有效，0表示无效 */

    int nAuthMode;                                                      /*!< 鉴权模式。1 - 自动模式；2 - 平台鉴权模式*/

    char szReserved[64];

} HY_DVR_DDLOCK_AUTH_MODE_CFG, *LPHY_DVR_DDLOCK_AUTH_MODE_CFG;



typedef struct tagHY_DVR_DDLOCK_QUERY_STATUS
{
    int bValid;                                                         /*!< 本结构体是否有效，0表示无效 */
   
    int nParkingSpaceNum;                                               /*!< 车位序号 */
    int nGroundLockStatus;                                              /*!< 车位锁状态。0 - 落下状态；1 - 升起状态 */
    int nParkingStatus;                                                 /*!< 停车状态。0–无车；1–有车 */

    char szReserved[128];

} HY_DVR_DDLOCK_QUERY_STATUS, *LPHY_DVR_DDLOCK_QUERY_STATUS;



typedef struct tagHY_DVR_DDLOCK_ACTIVE_SNAPSHOT_CFG
{
    int bValid;                                                         /*!< 本结构体是否有效，0表示无效 */
   
    int nIsSnapshot;                                                    /*!< 是否抓拍。0 - 否；1 - 是 */
    int nInterval;                                                      /*!< 时间间隔（多少秒之后进行抓拍，单位：秒） */
    int nParkingSpaceNum;                                               /*!< 车位序号 */

    char szReserved[124];

} HY_DVR_DDLOCK_ACTIVE_SNAPSHOT_CFG, *LPHY_DVR_DDLOCK_ACTIVE_SNAPSHOT_CFG;



typedef struct tagHY_DVR_TRAFFIC_STATICS_ALARM
{
    int iValid;

    int iBroadcastCount;  /*!< 播放次数 */
    int iBroadcastVolume;  /*!< 播放音量,1-100 */
    int iEnterVoiceEnable; /*!< 进入语音使能 */
    int iEnterVoiceType; /*!< 0:自定义,1:警报,2:犬吠,3:警戒区域,4:监控区域,5:欢迎光临,
                        6:深水警告,7:私人领地,8:危险区域,9:贵重物品,10:高处危险,
                        11:私家价位,12:叮咚,13:有口罩,14:无口罩,15:注意私人物品,
                        16:抓小偷,17:救命,18:分类,19:可回收垃圾,20:家庭食物垃圾
                        21:危险废物,22:残余垃圾,23:全部分类,24:车形自定义,25:禁止停车 */
    int iLeaveVoiceEnable;  /*!< 离开语音使能 */
    int iLeaveVoiceType; /*!< 0:自定义,1:警报,2:犬吠,3:警戒区域,4:监控区域,5:欢迎光临,
                        6:深水警告,7:私人领地,8:危险区域,9:贵重物品,10:高处危险,
                        11:私家价位,12:叮咚,13:有口罩,14:无口罩,15:注意私人物品,
                        16:抓小偷,17:救命,18:分类,19:可回收垃圾,20:家庭食物垃圾
                        21:危险废物,22:残余垃圾,23:全部分类,24:车形自定义,25:禁止停车 */
	int iDayMode;/*!< 0:夜晚模式,1:白天模式,2:不启用此模式（AllDay字段有效，当字段不为disable时，AllDay字段无效）当设置为此模式时，IPC依赖自身传感器判断当前环境是白天还是夜晚*/
    HY_DVR_SCHEDTIME struSchedTime;  /*!< 语音报警布防计划 */

    char acReserved[256];
} HY_DVR_TRAFFIC_STATICS_ALARM, *LPHY_DVR_TRAFFIC_STATICS_ALARM;

typedef struct tagHY_DVR_TRAFFIC_STATICS_UI_DESIGN_CFG
{
    int iValid;
 
    int iEnable;       /*!< 使能 */
    int iSensitivity;  /*!< 灵敏度，取值范围1~100：其中30为低，60为中，90为高 */
    int iDetectArea;         /*!< 显示检测区.0-不显示，1-显示 */
    int iTargetFrame;        /*!< 显示目标框.0-不显示，1-显示 */
    int iStatisticalResults; /*!< 显示统计结果.0-不显示，1-显示 */
    int iIsPushBackgroundPicture; /*!< 是否推送背景图.0-不推送，1-推送 */
    int iOrientation;             /*!< 检测方向，0：A->B，1：B->A */
    HY_DVR_TRAFFIC_STATICS_ALARM stVoiceAlarm; /*!< 语音报警 */
    HY_DVR_POINT_V2 stPolygon[MAX_POLYGON_LINE_NUM];/*!< 布防区域，n个点表示n边形。例:8个点表示8边形 */
    HY_DVR_SCHEDTIME struSchedTime;  /*!< 车流统计事件布防计划 */
    HY_DVR_HANDLEEXCEPTION struHandleType;      /*!< 处理方式  */
 
    char acReserved[1024];
} HY_DVR_TRAFFIC_STATICS_UI_DESIGN_CFG, *LPHY_DVR_TRAFFIC_STATICS_UI_DESIGN_CFG;
 
typedef struct tagHY_DVR_TRAFFIC_STATICS_UI_DESIGN_EVENT_TIMESLOT_LIST_CFG
{
    int iValid;
    HY_DVR_SCHEDULE_TIMESLOT stSchedTimeSlot[SCHEDULE_TIMESLOT_MAX_NUM]; /*!< 布防时间计划 */
    char szReserved[1024];
}HY_DVR_TRAFFIC_STATICS_UI_DESIGN_EVENT_TIMESLOT_LIST_CFG, *LPHY_DVR_TRAFFIC_STATICS_UI_DESIGN_EVENT_TIMESLOT_LIST_CFG;
 
typedef struct tagHY_DVR_TRAFFIC_STATICS_UI_DESIGN_VOICEALARM_TIMESLOT_LIST_CFG
{
    int iValid;
    HY_DVR_SCHEDULE_TIMESLOT stSchedTimeSlot[SCHEDULE_TIMESLOT_MAX_NUM]; /*!< 布防时间计划 */
    char szReserved[1024];
}HY_DVR_TRAFFIC_STATICS_UI_DESIGN_VOICEALARM_TIMESLOT_LIST_CFG, *LPHY_DVR_TRAFFIC_STATICS_UI_DESIGN_VOICEALARM_TIMESLOT_LIST_CFG;


typedef struct tagHY_DVR_TRAFFIC_STATISTICS_HOUR
{
    int nValid;
    int iDepartureNumber;                           /*!< 离开人数 */
    int iEntrantNumber;                             /*!< 进入人数 */
} HY_DVR_TRAFFIC_STATISTICS_HOUR, *LPHY_DVR_TRAFFIC_STATISTICS_HOUR;
 
typedef struct tagHY_DVR_TRAFFIC_STATISTIC_DAY
{
    int bValid;                                                                         /*!< 本结构体是否有效，0表示无效  */
    HY_DVR_TIME stStatisticDate;                                                        /*!< 统计日期 */
    HY_DVR_TRAFFIC_STATISTICS_HOUR stStatisticHourList[DVR_DAILY_HOUR_NUM];     /*!< 统计日期的客流统计数据 */
} HY_DVR_TRAFFIC_STATISTICS_DAY, *LPHY_DVR_TRAFFIC_STATISTICS_DAY;
 
typedef struct tagHY_DVR_TRAFFIC_STATISTICS
{
    int bValid;                                                                             /*!< 本结构体是否有效，0表示无效  */
    HY_DVR_TIME stStatisticStartDate;                                                       /*!< 输入参数：统计起始日期 */
    HY_DVR_TIME stStatisticEndDate;                                                         /*!< 输入参数：统计截止日期 */
    HY_DVR_TRAFFIC_STATISTICS_DAY stStatisticDayList[DVR_WEEK_DAY_NUM];                     /*!< 输出参数：7天的客流统计数据 */
} HY_DVR_TRAFFIC_STATISTICS, *LPHY_DVR_TRAFFIC_STATISTICS;



typedef struct tagHY_DVR_FRONT_DEVICE_OWNNER_INFO
{
    int iValid;

    int iUpdateFlag;           //更新标记;0-全量更新,1-增量更新(人脸识别前抓前比功能使用)
    char acOwnner[32];         //脸库拥有者(人脸识别前抓前比功能使用)
    char acWholeVersion[32];   //整体版本号,用来做增量更新(人脸识别前抓前比功能使用)
    char acWholeGroupUUID[64]; //整体脸库UUID,用来做全量量更新(人脸识别前抓前比功能使用)

    char strReserved[256];
}HY_DVR_FRONT_DEVICE_OWNNER_INFO;

//人脸识别基准脸库信息
typedef struct tagHY_DVR_FACE_RECOGNIZE_STANDARD_GROUP
{
    int iValid;

    int iFaceGroupPriority;   /*基准脸库优先级,取值0-最大通道数;0-表示当前脸库无效,
                                1-最大通道数 表示当前脸库作为人脸识别对比脸库的优先级,数值越大优先级越高*/
    char acFaceGroupID[64];   //脸库ID(人脸识别前抓前比功能使用)

    char szReserved[256];
}HY_DVR_FACE_RECOGNIZE_STANDARD_GROUP,*LPHY_DVR_FACE_RECOGNIZE_STANDARD_GROUP;

typedef struct tagHY_DVR_FACE_RECOGNIZE_STANDARD_GROUP_LIST
{
    int iValid;

    HY_DVR_FACE_RECOGNIZE_STANDARD_GROUP stFaceRecognizeStardandGroups[MAX_FACE_RECOGNIZE_STANDARD_GROUPS];
}HY_DVR_FACE_RECOGNIZE_STANDARD_GROUP_LIST;

typedef struct tagHY_DVR_DEVICE_INTERCOM_MODE
{
    int iValid;

    int iEnable;           /*!< 使能;0-去使能,1-使能;暂未使用 */
    int iIntercomMode;     /*!< 对讲模式;0-全双工对讲,1-半双工对讲 */

    char szReserved[128];
}HY_DVR_DEVICE_INTERCOM_MODE, *LPHY_DVR_DEVICE_INTERCOM_MODE;

typedef struct tagHY_DVR_DEVICE_ONE_BUTTON_CALL_CFG
{
    int iValid;

    int iEnable;           /*!< 使能;0-去使能,1-使能;暂未使用 */
    int iOneButtonCall;    /*!< 一键呼叫;0-挂断一键呼叫,1-接听一键呼叫 */

    char szReserved[128];
}HY_DVR_DEVICE_ONE_BUTTON_CALL_CFG, *LPHY_DVR_DEVICE_ONE_BUTTON_CALL_CFG;

typedef struct tagHY_DVR_DEVICE_ONE_BUTTON_CALL_FUNC_CFG
{
    int iValid;

    int iEnable;           /*!< 使能一键呼叫功能;0-去使能,1-使能 */

    char szReserved[128];
}HY_DVR_DEVICE_ONE_BUTTON_CALL_FUNC_CFG, *LPHY_DVR_DEVICE_ONE_BUTTON_CALL_FUNC_CFG;

typedef struct tagHY_DVR_FACE_GROUP_ORGANIZATION_STRUCTURE_NODE
{
    int iValid;

    char acOrganizationID[64];//当前组织ID(仅人脸识别前抓前比功能使用)
    char acLayer1ID[64];   //最顶层组织的ID(仅人脸识别前抓前比功能使用，同时表示黑白名单ID)
    char acLayer2ID[64];   //第二级组织的ID,若无第二组织,此值为0(仅人脸识别前抓前比功能使用)
    char acLayer3ID[64];   //第三级组织的ID,若无第三组织,此值为0(仅人脸识别前抓前比功能使用)
    char acLayer4ID[64];   //第四级组织的ID,若无第四组织,此值为0(仅人脸识别前抓前比功能使用)
    char acLayer5ID[64];   //第五级组织的ID,若无第五组织,此值为0(仅人脸识别前抓前比功能使用)
    char acLayer6ID[64];   //第六级组织的ID,若无第六组织,此值为0(仅人脸识别前抓前比功能使用)
    char acLayer7ID[64];   //第七级组织的ID,若无第七组织,此值为0(仅人脸识别前抓前比功能使用)
    char acOrganizationName[64];   //组织名称(仅人脸识别前抓前比功能使用)
    char acOrganizationMemo[128];  //有关组织的备注信息(仅人脸识别前抓前比功能使用)
    long long llReversion; //全局唯一(包括脸库表,人脸表),只增不减(仅人脸识别前抓前比功能使用)
    int bSelected;           //该节点在某条规则中是否选中，按位取值，bit0 为 1 则表示该节点在规则一中被选中；bot0 为 0 则表示该节点在规则一中未被选中

    char szReserved[252];
}HY_DVR_FACE_GROUP_ORGANIZATION_STRUCTURE_NODE,*LPHY_DVR_FACE_GROUP_ORGANIZATION_STRUCTURE_NODE;

typedef struct tagHY_DVR_FACE_GROUP_ORGANIZATION_STRUCTURE_TREE
{
    int iValid;

    int iNodeNum;
    HY_DVR_FACE_GROUP_ORGANIZATION_STRUCTURE_NODE *pstNode;
}HY_DVR_FACE_GROUP_ORGANIZATION_STRUCTURE_TREE, *LPHY_DVR_FACE_GROUP_ORGANIZATION_STRUCTURE_TREE;

//组织架构成员信息
typedef struct tagHY_DVR_FACE_GROUP_ORGANIZATION_MEMBER_ADD
{
    int iValid;

	long long llReversion; //全局唯一(包括脸库表,人脸表),只增不减(仅人脸识别前抓前比功能使用)
	char acOrganizationID[64];//当前组织ID(仅人脸识别前抓前比功能使用)
	char acLayer1ID[64];   //最顶层组织的ID(仅人脸识别前抓前比功能使用，同时表示黑白名单ID)
    char acLayer2ID[64];   //第二级组织的ID,若无第二组织,此值为0(仅人脸识别前抓前比功能使用)
    char acLayer3ID[64];   //第三级组织的ID,若无第三组织,此值为0(仅人脸识别前抓前比功能使用)
    char acLayer4ID[64];   //第四级组织的ID,若无第四组织,此值为0(仅人脸识别前抓前比功能使用)
    char acLayer5ID[64];   //第五级组织的ID,若无第五组织,此值为0(仅人脸识别前抓前比功能使用)
    char acLayer6ID[64];   //第六级组织的ID,若无第六组织,此值为0(仅人脸识别前抓前比功能使用)
    char acLayer7ID[64];   //第七级组织的ID,若无第七组织,此值为0(仅人脸识别前抓前比功能使用)
    char acOrganizationName[64];   //组织名称(仅人脸识别前抓前比功能使用)
    char acOrganizationMemo[128];  //有关组织的备注信息(仅人脸识别前抓前比功能使用)
    char acOwnner[64];             //安全码(仅人脸识别前抓前比功能使用)
    char acSystemTime[32];         //系统时间(仅人脸识别前抓前比功能使用)
    int bSelected;           //该节点在某条规则中是否选中，按位取值，bit0 为 1 则表示该节点在规则一中被选中；bot0 为 0 则表示该节点在规则一中未被选中

    char szReserved[252];
}HY_DVR_FACE_GROUP_ORGANIZATION_MEMBER_ADD,*LPHY_DVR_FACE_GROUP_ORGANIZATION_MEMBER_ADD;

//组织架构成员信息
typedef struct tagHY_DVR_FACE_GROUP_ORGANIZATION_MEMBER_MODIFY
{
    int iValid;

	long long llReversion; //全局唯一(包括脸库表,人脸表),只增不减(仅人脸识别前抓前比功能使用)
	char acOrganizationID[64];//当前组织ID(仅人脸识别前抓前比功能使用)
	char acLayer1ID[64];   //最顶层组织的ID(仅人脸识别前抓前比功能使用，同时表示黑白名单ID)
    char acLayer2ID[64];   //第二级组织的ID,若无第二组织,此值为0(仅人脸识别前抓前比功能使用)
    char acLayer3ID[64];   //第三级组织的ID,若无第三组织,此值为0(仅人脸识别前抓前比功能使用)
    char acLayer4ID[64];   //第四级组织的ID,若无第四组织,此值为0(仅人脸识别前抓前比功能使用)
    char acLayer5ID[64];   //第五级组织的ID,若无第五组织,此值为0(仅人脸识别前抓前比功能使用)
    char acLayer6ID[64];   //第六级组织的ID,若无第六组织,此值为0(仅人脸识别前抓前比功能使用)
    char acLayer7ID[64];   //第七级组织的ID,若无第七组织,此值为0(仅人脸识别前抓前比功能使用)
    char acOrganizationName[64];   //组织名称(仅人脸识别前抓前比功能使用)
    char acOrganizationMemo[128];  //有关组织的备注信息(仅人脸识别前抓前比功能使用)
    char acOwnner[64];             //安全码(仅人脸识别前抓前比功能使用)
    char acSystemTime[32];         //系统时间(仅人脸识别前抓前比功能使用)
    int bSelected;           //该节点在某条规则中是否选中，按位取值，bit0 为 1 则表示该节点在规则一中被选中；bot0 为 0 则表示该节点在规则一中未被选中

    char szReserved[252];
}HY_DVR_FACE_GROUP_ORGANIZATION_MEMBER_MODIFY,*LPHY_DVR_FACE_GROUP_ORGANIZATION_MEMBER_MODIFY;

//组织架构成员信息
typedef struct tagHY_DVR_FACE_GROUP_ORGANIZATION_MEMBER_DELETE
{
    int iValid;

	long long llReversion; //全局唯一(包括脸库表,人脸表),只增不减(仅人脸识别前抓前比功能使用)
	char acOrganizationID[64];//当前组织ID(仅人脸识别前抓前比功能使用)
	char acLayer1ID[64];   //最顶层组织的ID(仅人脸识别前抓前比功能使用，同时表示黑白名单ID)
    char acLayer2ID[64];   //第二级组织的ID,若无第二组织,此值为0(仅人脸识别前抓前比功能使用)
    char acLayer3ID[64];   //第三级组织的ID,若无第三组织,此值为0(仅人脸识别前抓前比功能使用)
    char acLayer4ID[64];   //第四级组织的ID,若无第四组织,此值为0(仅人脸识别前抓前比功能使用)
    char acLayer5ID[64];   //第五级组织的ID,若无第五组织,此值为0(仅人脸识别前抓前比功能使用)
    char acLayer6ID[64];   //第六级组织的ID,若无第六组织,此值为0(仅人脸识别前抓前比功能使用)
    char acLayer7ID[64];   //第七级组织的ID,若无第七组织,此值为0(仅人脸识别前抓前比功能使用)
    char acOrganizationName[64];   //组织名称(仅人脸识别前抓前比功能使用)
    char acOrganizationMemo[128];  //有关组织的备注信息(仅人脸识别前抓前比功能使用)
    char acOwnner[64];             //安全码(仅人脸识别前抓前比功能使用)
    char acSystemTime[32];         //系统时间(仅人脸识别前抓前比功能使用)
    int bSelected;           //该节点在某条规则中是否选中，按位取值，bit0 为 1 则表示该节点在规则一中被选中；bot0 为 0 则表示该节点在规则一中未被选中

    char szReserved[252];
}HY_DVR_FACE_GROUP_ORGANIZATION_MEMBER_DELETE,*LPHY_DVR_FACE_GROUP_ORGANIZATION_MEMBER_DELETE;

//组织架构成员信息
typedef struct tagHY_DVR_FACE_GROUP_ORGANIZATION_MEMBER_SELECT
{
    int iValid;

    long long llReversion; //全局唯一(包括脸库表,人脸表),只增不减(仅人脸识别前抓前比功能使用)
    char acOrganizationID[64]; //当前组织ID(仅人脸识别前抓前比功能使用,对应xml的GroupID节点)
    char acLayer1ID[64];   //最顶层组织的ID(仅人脸识别前抓前比功能使用)
    char acLayer2ID[64];   //第二级组织的ID,若无第二组织,此值为0(仅人脸识别前抓前比功能使用)
    char acLayer3ID[64];   //第三级组织的ID,若无第三组织,此值为0(仅人脸识别前抓前比功能使用)
    char acLayer4ID[64];   //第四级组织的ID,若无第四组织,此值为0(仅人脸识别前抓前比功能使用)
    char acLayer5ID[64];   //第五级组织的ID,若无第五组织,此值为0(仅人脸识别前抓前比功能使用)
    char acLayer6ID[64];   //第六级组织的ID,若无第六组织,此值为0(仅人脸识别前抓前比功能使用)
    char acLayer7ID[64];   //第七级组织的ID,若无第七组织,此值为0(仅人脸识别前抓前比功能使用)
    char acOrganizationName[64];   //组织名称(仅人脸识别前抓前比功能使用)
    char acOrganizationMemo[128];  //有关组织的备注信息(仅人脸识别前抓前比功能使用)
    char acOwnner[64];             //安全码(仅人脸识别前抓前比功能使用)
    char acSystemTime[32];         //系统时间(仅人脸识别前抓前比功能使用)
    int bSelected;           //该节点在某条规则中是否选中，按位取值，bit0 为 1 则表示该节点在规则一中被选中；bot0 为 0 则表示该节点在规则一中未被选中

    char szReserved[252];
}HY_DVR_FACE_GROUP_ORGANIZATION_MEMBER_SELECT,*LPHY_DVR_FACE_GROUP_ORGANIZATION_MEMBER_SELECT;

typedef struct tagHY_DVR_DAJIANG_IOT_CFG_PARAM
{
    int iValid;

    char acUpdateDate[32];         /*!< eg:"2018-12-24 10:59:31" */
    int iIsFaceRecgEnable;         /*!< 是否开启人脸识别,0:否,1:是 */
    int iIsLiveDetectEnable;       /*!< 是否启用活体检测,0:否,1:是 */
    int iIsUploadUnlockRec;        /*!< 是否上传开锁记录,0:否,1:是 */
    int iIsUploadFaceMatchFailRec; /*!< 是否上传人脸比对失败记录,0:否,1:是 */
    int iLockDelayLong;            /*!< 关锁延时时长,单位毫秒 */
    double dSimThreshold;          /*!< 相似度阈值 */

    char strReserved[256];
}HY_DVR_DAJIANG_IOT_CFG_PARAM, *LPHY_DVR_DAJIANG_IOT_CFG_PARAM;

typedef struct tagHY_DVR_DAJIANG_IOT_SERVER_CFG_CUSTOM
{
    int iValid;

    int iEnable;                       /*!< 是否使能;0:否,1:是 */
    char acServerAddress[128];         /*!< 事件服务器地址;eg:http://auth.sfbchina.cn:9868 */
    char acHBServerAddress[128];       /*!< 心跳服务器地址;eg:http://auth.sfbchina.cn:9603*/
    char acFaceInfoDownloadUrl[128];   /*!< 下载门禁人脸信息url;eg:/ivs/api/v2.0/acFaceInfo/download */
    char acFaceSyncRsltUploadUrl[128]; /*!< 人脸同步结果上传url;eg:/ivs/api/v2.0/acFaceInfo/syncRslt/upload */
    char acUnlockRecordUploadUrl[128]; /*!< 开锁记录上传url;eg:/ivs/api/v2.0/acUnlockRec/upload */
    char acDeviceSettingUrl[128];      /*!< 下载配置参数url;eg:/ivs/api/v2.0/acDevice/setting/get */
    char acHeartBeatUrl[128];          /*!< 心跳url;eg:/dms/api/device/heartBeat */
    int iHeartBeatInterval;            /*!< 心跳间隔时间(秒)：10-3600（默认1800） */
    int iFaceOnceDownloadCount;        /*!< 人员信息一次下载数量：1-100 （默认为20） */
    int iFaceDownloadInterval;         /*!< 人员信息下载间隔时间(秒):5-60（默认为10） */
    int iIsEventRetransmission;        /*!< 事件是否重传;0:否,1:是 */
    int iSameEventRetransInterval;     /*!< 同一事件重传间隔(秒);10-1800 （默认为900） */
    int iDiffEventRetransInterval;     /*!< 不同事件重传间隔(秒);1-300 （默认为10） */
    int iDeviceSettingInterval;        /*!< 从平台下载配置参数间隔(秒);1-100 （默认为10） */

    char strReserved[256];
}HY_DVR_DAJIANG_IOT_SERVER_CFG_CUSTOM, *LPHY_DVR_DAJIANG_IOT_SERVER_CFG_CUSTOM;

typedef struct tagHY_DVR_DAJIANG_IOT_SERVER_CONNECT_STATUS
{
    int iValid;

    int  iConnectServerStatus; /* 连接服务器状态,0-初始化状态;1-成功;2-用户名或密码错误,3-ID冲突,4-网络异常,
                                  0xff-其他未知错误;大匠定制版本适配原则:除成功状态外的其他状态都适配成失败状态*/

    char strReserved[128];
}HY_DVR_DAJIANG_IOT_SERVER_CONNECT_STATUS, *LPHY_DVR_DAJIANG_IOT_SERVER_CONNECT_STATUS;

/*!
*  @struct    tagHY_DVR_PRODUCT_TOOL_LAMP_CFG
*  @brief    产测工具台灯配置
 */
typedef struct tagHY_DVR_PRODUCT_TOOL_LAMP_CFG
{
    int iValid;                     /*!< 本结构体是否有效，0表示无效 */

    int iLampSwitch;                /*!< 台灯开关;0-关,1-开 */
    int iLampBrightness;            /*!< 台灯亮度;0-100 */

    char acReserver[128];
}HY_DVR_PRODUCT_TOOL_LAMP_CFG, *LPHY_DVR_PRODUCT_TOOL_LAMP_CFG;

typedef struct tagHY_DVR_OFFDUTY_QUERY_DAY
{
    int iValid;                             /*!< 本结构体是否有效，0表示无效  */

    HY_DVR_TIME stDate;            /*!< 统计日期 */
    int iOffDutyNumber;       /*!< 离岗次数 */
    int iOffDutyDuration;     /*!< 离岗总时长 */
} HY_DVR_OFFDUTY_QUERY_DAY, *LPHY_DVR_OFFDUTY_QUERY_DAY;

typedef struct tagHY_DVR_AI_OFFDUTY_QUERY
{
    int iValid;                                         /*!< 本结构体是否有效，0表示无效  */

    HY_DVR_TIME stStartDate;                   /*!< 输入参数：统计起始日期 */
    HY_DVR_TIME stEndDate;                     /*!< 输入参数：统计截止日期 */
    HY_DVR_OFFDUTY_QUERY_DAY stDayList[DVR_WEEK_DAY_NUM]; /*!< 输出参数：7天的客流统计数据 */
} HY_DVR_AI_OFFDUTY_QUERY, *LPHY_DVR_AI_OFFDUTY_QUERY;

typedef struct tagHY_DVR_FIRE_DETECT_CFG_INFO
{
	int iValid;
    
	int iEnable;														/*!< 是否处理火焰侦测,0-否,1-是  */
	int iSenstive;														/*!< 火焰侦测灵敏度, 1-低,2-中,3-高  */
	HY_DVR_POINT_V2 stFireDetectRegion[DVR_MAX_FIRE_DETECT_REGION_NUM];	/*!< 火焰探测区域  */
	HY_DVR_HANDLEEXCEPTION stHandleType;								/*!< 火焰探测处理方式  */
	HY_DVR_SCHEDTIME stSchedTime;										/*!< 布防计划  */
	int iNightDetectEnable;                          					/*!< 是否使能夜间检测,0-否,1-是 */ 
    int iWhiteLightBrightness;                							/*!< 白光灯亮度;取值1-100，默认50 */
	int iIsPushBackgroundPicture;                    					/*!< 是否推送背景图.0-不推送，1-推送 */

	char strReserved[244];
}HY_DVR_FIRE_DETECT_CFG_INFO, *LPHY_DVR_FIRE_DETECT_CFG_INFO;

typedef struct tagHY_DVR_FIRE_DETECT_EX_CFG
{
    int iValid;

    int iEnable;                                                    /*!< 是否使能 */
    int iVoiceType; /*!< 0:自定义,1:警报,2:犬吠,3:警戒区域,4:监控区域,5:欢迎光临,6:深水警告,7:私人领地,8:危险区域,9:贵重物品,10:高处危险,
                        11:私家价位,12:叮咚,13:有口罩,14:无口罩,15:注意私人物品,16:抓小偷,17:救命,18:分类,19:可回收垃圾,20:家庭食物垃圾
                        21:危险废物,22:残余垃圾,23:全部分类,24:车形自定义,25:禁止停车 */
    int iBroadcastCount;                                            /*!< 播报次数,范围1~5次 */
    int iDayMode;/*!< 0:夜晚模式,1:白天模式,2:不启用此模式（AllDay字段有效，当字段不为disable时，AllDay字段无效）当设置为此模式时，IPC依赖自身传感器判断当前环境是白天还是夜晚*/
    HY_DVR_SCHEDTIME struSchedTime;                                    /*!< 声音报警布防时间 */
	int iBroadcastCountVolume;                                         /*!< 播报音量；取值1-100，默认90 */

    char acReserved[1020];
} HY_DVR_FIRE_DETECT_EX_CFG, *LPHY_DVR_FIRE_DETECT_EX_CFG;

typedef struct tagHY_DVR_JKM_PLATFORM_CFG_INFO
{
	int iValid;
    
	char acServerIPAddress[256];							/*!< 服务器IP地址或者域名  */
	int iServerPort;										/*!< 服务器端口  */

	char strReserved[512];
}HY_DVR_JKM_PLATFORM_CFG_INFO, *LPHY_DVR_JKM_PLATFORM_CFG_INFO;

typedef struct tagHY_DVR_SZSB_PLATFORM_AUTH_INFO
{
	int iValid;
    
    char acUrl[256];							/*!< 接口地址 */
    char acAppID[64];							/*!< AppId  */
	char acAppKey[16];							/*!< AppKey */
    char acIfVersion[32];						/*!< 接口版本(例https://ip:port/xx/v1/register中的/v1),缺省为 v1 */
	char acToken[128];               			/*!< token  */
    char acSecretKey[128];           			/*!< SecretKey  */

    char strReserved[768];
}HY_DVR_SZSB_PLATFORM_AUTH_INFO, *LPHY_DVR_SZSB_PLATFORM_AUTH_INFO;

typedef struct tagHY_DVR_SZSB_PLATFORM_DEVICE_CFG_INFO
{
	int iValid;

	char acSceneAccount[64];		//场所编码，例:123
	char acSceneCategory[8];		//场所一级分类，例:1002,详见《SZSB平台接入规范V1.1-达式》附录中 场景分类
	char acSceneType[8];			//场所二级分类，例:1018,支持终端侧可配置，详见《SZSB平台接入规范V1.1-达式》附录中 场景分类

	char szReserved[1024];
}HY_DVR_SZSB_PLATFORM_DEVICE_CFG_INFO, *LPHY_DVR_SZSB_PLATFORM_DEVICE_CFG_INFO;

typedef struct tagHY_DVR_SWP_PLANTFORM_COMPANY_DEVICE_INFO
{
	int iValid;
    
	char acCompany[128];				//公司名
	char acDeviceAddress[256];			//设备场所
    
	char acReserved[1024];
}HY_DVR_SWP_PLANTFORM_COMPANY_DEVICE_INFO, *LPHY_DVR_SWP_PLANTFORM_COMPANY_DEVICE_INFO;

typedef struct tagHY_DVR_HENAN_HEALTHCODE_DEVICE_CDKEY_INFO
{
    int iValid;
    
    char acCDKEY[128];                            /*!< 激活码 */

    char strReserved[1024];
}HY_DVR_HENAN_HEALTHCODE_DEVICE_CDKEY_INFO, *LPHY_DVR_HENAN_HEALTHCODE_DEVICE_CDKEY_INFO;

typedef struct tagHY_DVR_ONENET_HEALTHCODE_DEVICE_CDKEY_INFO
{
    int iValid;
    
    char acCDKEY[128];                            /*!< 激活码 */

    char strReserved[1024];
}HY_DVR_ONENET_HEALTHCODE_DEVICE_CDKEY_INFO, *LPHY_DVR_ONENET_HEALTHCODE_DEVICE_CDKEY_INFO;

/*!
*  @struct    tagHY_DVR_VIDEO_SHELTER_ALARM_CFG
*  @brief    视频遮挡报警配置
 */
typedef struct tagHY_DVR_VIDEO_SHELTER_ALARM_CFG
{
    int bValid;                                                     /*!< 本结构体是否有效，0表示无效 */

    int iEnable;                                                    /*!< 是否使能 */
    int iSensitivity;                                               /*!< 灵敏度。取值范围1~100：其中30为低，60为中，90为高 */
    int iVoiceType; /*!< 0:自定义,1:警报,2:犬吠,3:警戒区域,4:监控区域,5:欢迎光临,6:深水警告,7:私人领地,8:危险区域,9:贵重物品,10:高处危险,
                        11:私家价位,12:叮咚,13:有口罩,14:无口罩,15:注意私人物品,16:抓小偷,17:救命,18:分类,19:可回收垃圾,20:家庭食物垃圾
                        21:危险废物,22:残余垃圾,23:全部分类,24:车形自定义,25:禁止停车 */
    int iBroadcastCount;                                            /*!< 播报次数 */
    int iDayMode;/*!< 0:夜晚模式,1:白天模式,2:不启用此模式（AllDay字段有效，当字段不为disable时，AllDay字段无效）当设置为此模式时，IPC依赖自身传感器判断当前环境是白天还是夜晚*/
    HY_DVR_HANDLEEXCEPTION stHandleType;                            /*!< 处理方式 */
    HY_DVR_SCHEDTIME stSchedTime;                                   /*!< 布防计划 */
    HY_DVR_SCHEDTIME stVoiceAlarmSchedTime;                         /*!< 声音报警布防计划 */
	int iShelterTimeThreshold;                                      /*!< 遮挡时间阈值,[1-60]; */
	int iMinShelterInterval;                                        /*!< 最小遮挡时间间隔,[1-60] */

    char acReserved[256 - 8];
} HY_DVR_VIDEO_SHELTER_ALARM_CFG, *LPHY_DVR_VIDEO_SHELTER_ALARM_CFG;

typedef struct tagHY_DVR_ONENET_EMERGENCY_ACCESS_RULE
{
	int iValid;

	int iForbidRules_jkmColor;				//健康码颜色判断规则,0:不判断健康码颜色;1:黄码(包含红码);2:红码,缺省为-1
	double dForbidRules_temper;				//体温判断规则,体温>=阈值时提示禁止通行;默认值为0,则表示不判断体温
	char acAllowRules_hsStatus[32];			//核酸结果判断规则,0:表示未定义;24h:表示持24小时核酸阴性可进入;48:表示持48小时核酸阴性可进入,72h:表示持72小时核酸阴性可进入;
											//7d:表示持7天核酸阴性可进入,14d:表示持14天核酸阴性可进入,30d:表示持30天核酸阴性可进入
	int iTimeout;							//等待平台响应的超时时间,默认为6秒,用于后续切换为终端判断模式用,单位：秒

	char reserved[256];
}HY_DVR_ONENET_EMERGENCY_ACCESS_RULE, *LPHY_DVR_ONENET_EMERGENCY_ACCESS_RULE;

//OneNet平台通行判断结果
typedef struct tagHY_DVR_ONENET_PASSJUDGERESULT
{
	int iValid;

	unsigned long long ullId;				//事件ID
	char acNotifyId[32];				//注意，这里和刷码数据上报时携带的id一致,用于对比请求和响应匹配,缺省为空
	int iPass;							//通行结果,0:禁止通行(体温、健康码、核酸、疫苗等状态有异常,不符合当前区域放行规则,禁止通行);
												//1:正常通行(体温、健康码、核酸、疫苗等状态无异常,符合当前区域放行规则,正常通行),缺省为-1
	int iShowColor;						//通行结果颜色展示.1:绿色(当通行结果为1(正常通行)时存在);2:黄色(当通行结果为0(禁止通行)时存在);3:红色(当通行结果为0(禁止通行)时存在),缺省为-1
	int iUserType;						//人员类型.1:居住人员;2:工作人员;3:访客;999:其他,缺省为-1
	int iUserBelong;					//人员归属范围.1:本围合点内白名单;2:同社区白名单;3:同街道白名单;4:同区白名单;5:非本区白名单;6:未知;
													//7:非本围合白名单;8:非本围合白名单;9:非本围合白名单;缺省为-1
	int iNucleinResult;					//核酸结果.0:阴性;1:阳性;-1:无记录
	int iNucleinPeriod;					//核酸检测有效期.0:近30天无核酸检查结果;24:处于核酸24小时内;48:处于核酸48小时内;72:处于核酸72小时内;
														//168:处于核酸7天内;336:处于核酸14天内;720:处于核酸30天内,缺省为-1
	int iNucleinRecord;					//核酸检测记录。0:未知;3:3连阴;4:4连阴;7:7连阴;32:3天2检查,且结果为阴性;53:5天3检查,且结果为阴性;75:7天5检查,且结果为阴性,缺省为-1
	int iStatus;						//健康码状态.0:绿码;1:黄码,2:红码,999:未知,缺省为-1
	int iIsWhite;						//是否开启白名单.0:未开启,1:已开启,缺省为-1
	char acNucleinTime[32];				//核酸检测时间,缺省为空
	char acName[32];					//人员姓名,缺省为空
	char acPointName[32];				//围合点名称,缺省为空
	char acYmRecord[128];				//疫苗接种情况记录,字符串结构为：完成情况&日期,缺省为空
	char acMessage[256];				//通行提示，多个提示原因用‘&’字符分隔,缺省为空

	char reserved[256];
}HY_DVR_ONENET_PASSJUDGERESULT, *LPHY_DVR_ONENET_PASSJUDGERESULT;

//OneNet平台用户证件号码查询结果下发
typedef struct tagHY_DVR_ONENET_ZJHM_QUERY_DATA
{
	int iValid;

	unsigned long long ullId;				//事件ID
	int iCode;
	char acNotifyId[32];				//注意,这里和用户证件号码查询事件上报的ID一致,缺省为空
	char acCardId[32];					//身份ID,缺省为空
	char acMsge[256];					//通行提示，多个提示原因用‘&’字符分隔,缺省为空

	char reserved[512];
}HY_DVR_ONENET_ZJHM_QUERY_DATA, *LPHY_DVR_ONENET_ZJHM_QUERY_DATA;

typedef struct tagHY_DVR_DEVICE_INSTALL_INFO
{
    int iValid;

    char acDeviceName[128];                  //设备名称
    char acInstallAddress[256];              //安装地址
    char cIsDeviceNameShow;                  //是否显示设备名称;0:不显示;1:显示
    char cIsInstallAddressShow;              //是否显示安装地址;0:不显示;1:显示

    char szReserved[1026];
}HY_DVR_DEVICE_INSTALL_INFO, *LPHY_DVR_DEVICE_INSTALL_INFO;

typedef struct tagHY_DVR_LL_PLAT_CONNECT_STATE_INFO_CFG
{
	int iValid;								/*!< 本结构体是否有效，0表示无效 */
	int iEnable;							/*!< 使能 */
	int iStatus;							/*!< 在线状态，0，不在线，1，在线 */
}HY_DVR_LL_PLAT_CONNECT_STATE_INFO_CFG, *LPHY_DVR_LL_PLAT_CONNECT_STATE_INFO_CFG;

typedef struct tagHY_DVR_VOICE_BROADCAST_CFG_INFO
{
	int iValid;								/*!< 本结构体是否有效，0表示无效 */
	char acPath[128];						/*!< 语音播报路径 */
}HY_DVR_VOICE_BROADCAST_CFG_INFO, *LPHY_DVR_VOICE_BROADCAST_CFG_INFO;

typedef struct tagHY_DVR_SCREEN_UIPERSONFACE_EVENT_PROMPT_SWITCH_CFG
{
	int iValid;								/*!< 本结构体是否有效，0表示无效 */
	int iEnable;							/*!< 屏幕UI人脸事件提示使能开关，1:使能;0:去使能 */
	char acReserved[64];
}HY_DVR_SCREEN_UIPERSONFACE_EVENT_PROMPT_SWITCH_CFG, *LPHY_DVR_SCREEN_UIPERSONFACE_EVENT_PROMPT_SWITCH_CFG;

typedef struct tagHY_DVR_CUSTOMER_DEFINE_UI_PROMPT_INFO
{
	int iValid;								/*!< 本结构体是否有效，0表示无效 */
	int iDisplayDuration;					/*!< 显示时长 */
	char acPromptContext[128];				/*!< 自定义提示内容 */
	char acReserved[256];
}HY_DVR_CUSTOMER_DEFINE_UI_PROMPT_INFO, *LPHY_DVR_CUSTOMER_DEFINE_UI_PROMPT_INFO;

typedef struct tagHY_DVR_FACEGROUP_ADDLIST_BEGIN_CFG
{
	int bValid;
	int iNumber;															/*!< 入库数据量  */
	char szReserved[128];
}HY_DVR_FACEGROUP_ADDLIST_BEGIN_CFG, *LPHY_DVR_FACEGROUP_ADDLIST_BEGIN_CFG;

typedef struct tagHY_DVR_FACEGROUP_ADDLIST_END_CFG
{
	int bValid;
	char szReserved[128];
}HY_DVR_FACEGROUP_ADDLIST_END_CFG, *LPHY_DVR_FACEGROUP_ADDLIST_END_CFG;

typedef struct tagHY_DVR_TIANYI_CLOUD_STREAM_TYPE_INFO
{
	int nValid;

	int nCloudStreamTypeID;
	int nHotConfigID;        //平台下发的业务配置ID，设备出厂或重置后或者平台未下发，上报0，该值设置了以后需要断电保存，恢复出厂时恢复成0

	char szReserved[32];
}HY_DVR_TIANYI_CLOUD_STREAM_TYPE_INFO, *LPHY_DVR_TIANYI_CLOUD_STREAM_TYPE_INFO;

typedef struct tagHY_DVR_TIANYI_ALARM_INFO
{
	int nValid;

	int nAlarmType;			/*
											图片告警类型：1.移动侦测，2.外部输入，3.人体红外，4.烟感，5.门磁，6.异响告警，7.人形侦测，8.哭声告警（0-10摄像机使用）
																11.按铃，12.经过，13.逗留，14.强拆 ,15.门铃上线 ，16.门铃下线， 17.电量低 （10-20门铃使用）
										*/
	int nPicSize;					//报警抓图图片大小
	int nCameraID;				//通道号    IPC设备默认通道 0
	char szFileName[64];		//对上传图片文件的重新命名   ，IPC设备文件名使用 触发时的设备本地时间（如GMT+8北京时间） 格式为yyyyMMddHHmmss.jpg 。
										//门铃设备：指定文件名为报警云存录像文件名开始时间 格式为yyyyMMddHHmmss.jpg 。
	char szFaceID[256];		//触发人脸识别报警时，传入识别的人脸ID，可以有多个使用逗号隔开
	long long nUtcTime;		//支持毫秒级时间戳

	char szReserved[256];
}HY_DVR_TIANYI_ALARM_INFO, *LPHY_DVR_TIANYI_ALARM_INFO;
//抓图图片二进制紧跟结构体
//HieServer_RefreshEventEx倒数第二个参数为结构体二进制+图片二进制，最后一个参数为sizeof(HY_DVR_TIANYI_ALARM_INFO)+nPicSize

typedef struct tagHY_DVR_WECHAT_BASIC_INFO
{
	int nValid;

	char szModelID[64];

	char szReserved[256];
}HY_DVR_WECHAT_BASIC_INFO, *LPHY_DVR_WECHAT_BASIC_INFO;

typedef struct tagHY_DVR_TIANYI_AUTH_INFO
{
	int nValid;

	int nAuthStatus;		//授权状态
	char szSerialNumber[32];	//序列号
	char szCTEI[16];			//15byte十进制数，前8byte是TUI码，不同型号产品不同（如天翼3MP室外枪机TUI为 18483912），后7位从0000001开始递增，每台设备唯一（示例：184839120000011）
	char szDeviceCode[16];		//设备码
	char szDevicePassword[16];	//设备密码
	char szBindVerifyCode[16];	//绑定验证码
	char szBindCode[64];		//设备绑定标识二维码

	char szReserved[256];
}HY_DVR_TIANYI_AUTH_INFO, *LPHY_DVR_TIANYI_AUTH_INFO;

typedef struct tagHY_DVR_TIANYI_CLOUD_ICON_TYPE_INFO
{
	int nValid;
	int nCloudIconType; //云存封面开关，0-开启，需要上传封面(默认) 1-关闭，不上传封面
	char szReserved[32];
}HY_DVR_TIANYI_CLOUD_ICON_TYPE_INFO, *LPHY_DVR_TIANYI_CLOUD_ICON_TYPE_INFO;

typedef struct tagHY_DVR_SOUND_LIGHT_ALARM_START
{
	int nValid;
	int nEnable;
	int type;          /**< 动作类型： 1：打开声音警报   2：打开白光灯   4：打开蜂鸣器，支持按位或的操作 */
	int spkdur;        /**< 扬声器警报时长，单位：秒 */
	int litmode;       /**< 白光灯亮灯模式，0：常亮 1：闪烁  */
	int litdur;        /**< 白光灯亮灯时长，单位：秒 */
	int buzdur;        /**< 蜂鸣器的报警时长， 单位：秒 */
	char cReserve[64];
}HY_DVR_SOUND_LIGHT_ALARM_START, *LPHY_DVR_SOUND_LIGHT_ALARM_START;

typedef struct tagHY_DVR_SOUND_LIGHT_ALARM_STOP
{
	int nValid;
	char cReserve[64];
}HY_DVR_SOUND_LIGHT_ALARM_STOP, *LPHY_DVR_SOUND_LIGHT_ALARM_STOP;

typedef struct tagHY_DVR_GET_CMDOT_DEVICE_LOG
{
	int nValid;
	char strLogPath[256];         //获取中间层的日志文件  eg:/tmp/device.zip
	char cReserve[512];
}HY_DVR_GET_CMDOT_DEVICE_LOG, *LPHY_DVR_GET_CMDOT_DEVICE_LOG;


/** 设备硬件能力集属性(中国移动) */
typedef struct {
	bool hw_support_wifi;             /**< 硬件支持WIFI */
	bool hw_support_tfcard;           /**< 硬件支持TF卡 */
	bool hw_support_speaker;          /**< 硬件支持扬声器 */
	bool hw_support_pir;              /**< 硬件支持PIR侦测 */
	bool hw_support_ptz_pan;          /**< 硬件支持PTZ左右转动 */
	bool hw_support_ptz_tilt;         /**< 硬件支持PTZ上下转动 */
	bool hw_support_microphone;       /**< 硬件支持麦克风采集声音 */
	bool hw_support_battery;          /**< 硬件支持电池供电 */
	bool hw_support_buzzer;           /**< 硬件支持蜂鸣器 */
	bool hw_support_led_indicate;     /**< 硬件支持状态指示灯 */
	bool hw_support_night_vision;     /**< 硬件支持夜视模式 */
	bool hw_support_tamer_alarm;      /**< 硬件支持防拆报警 */
	bool hw_support_light;            /**< 硬件支持白光灯控制 */
	bool hw_support_ring;             /**< 硬件支持门铃按键 */
	bool hw_support_binocular;        /**< 双目摄像头 */
	bool hw_support_manual_focus;     /**< 手动变焦 */
	bool hw_support_manual_zoom;      /**< 手动变倍 */
	bool hw_cap_reserve[60];          /**< 预留字段 */
}HY_DVR_DEVICE_HW_CAPABILITY;

/** 设备软件能力集属性(中国移动) */
typedef struct {
	bool sw_support_rotate;                     /**< 支持画面180度翻转  */
	bool sw_support_bi_directional_audiotalk;   /**< 支持双向语音对讲  */
	bool sw_support_motion_detect;              /**< 支持运动检测报警 */
	bool sw_support_motion_detect_region;       /**< 支持运动检测区域 */
	bool sw_support_motion_track;               /**< 支持运动跟踪 */
	bool sw_support_motion_detect_sesitivity;   /**< 支持运动检测灵敏度设置 */
	bool sw_support_sound_detect;               /**< 支持声音检测报警 */
	bool sw_support_sound_detect_sensitivity;   /**< 支持声音检测灵敏度设置 */
	bool sw_support_people_detect;              /**< 支持人形检测报警 */
	bool sw_support_people_detect_sensitivity;  /**< 支持人形检测灵敏度设置 */
	bool sw_support_people_draw_box;            /**< 支持人形画框功能 */
	bool sw_support_speaker_volume;             /**< 支持扬声器音量调节 */
	bool sw_support_doorbell_volume;            /**< 支持门铃音量调节 */
	bool sw_support_video_resolution_modify;    /**< 支持视频分辨率修改 */
	bool sw_support_pir_sensitivity;            /**< 支持PIR灵敏度设置 */
	bool sw_support_antiflicker;                /**< 支持防闪烁设置（50HZ/60HZ） */
	bool sw_support_face_detect;                /**< 支持人脸侦测报警 */
	bool sw_support_face_detect_distance;       /**< 支持人脸检测瞳距设置 */
	bool sw_support_face_detect_region;         /**< 支持人脸检测区域设置 */
	bool sw_support_face_detect_exposure;       /**< 支持人脸曝光开关 */
	bool sw_support_face_detect_luminance;      /**< 支持人脸亮度设置 */
	bool sw_support_face_detect_param;          /**< 支持人脸检测参数 */
	bool sw_support_face_capture_mode;          /**< 支持人脸抓拍模式设置，和回调类型 DOT_CMD_SET_FACE_CAPTURE_MODE 对应 */
	bool sw_support_face_capture_quality;       /**< 支持人脸抓拍质量设置，和回调类型 DOT_CMD_SET_FACE_CAPTURE_QUALITY 对应 */
	bool sw_support_ptz_preset_position;        /**< 支持PTZ预置点设置 */
	bool sw_support_defense_alarm;              /**< 支持防御报警设置（声光报警） */
	bool sw_support_close_defense_alarm;        /**< 支持关闭防御报警 */
	bool sw_support_alarm_volume;               /**< 支持报警音量调节 */
	bool sw_support_full_color;                 /**< 支持全彩设置 */
	bool sw_support_remote_wakeup_switch;       /**< 支持远程唤醒 */
	bool sw_support_people_detect_region;
	bool sw_support_super_hd_video;             /**< 支持超清视频（超清、高清、标清切换）， 如果支持此能力则将sw_support_video_resolution_modify 能力置为false*/
	bool sw_support_event_thumbnail;            /**< 支持事件缩略图 */
	bool sw_support_osd_logo;                   /**< 支持logo水印和水印开关 */
	bool sw_support_region_hack;                /**< 支持区域入侵，与人形检测能力互斥，即设置区域入侵能力集，需要将sw_support_people_detect_region，sw_support_people_detect和sw_support_people_detect_sensitivity这3个相关的人形检测能力集均设置为false */
	bool sw_support_generate_osd_name;          /**< 支持根据文字生成OSD水印，并叠加到视频中，设备需要包含中文字库 */
	bool sw_support_pir_stay_time;              /**< 支持pir逗留时间配置 */
	bool sw_support_falling_object_detect;      /**< 支持高空抛物检测 */
	bool sw_support_falling_object_region;      /**< 支持高空抛物区域设置 */
	bool sw_support_falling_object_confidence_level;   /**< 支持高空抛物置信度设置 */
	bool sw_support_andhome_voip;               /** < 支持和家固话功能 */
	bool sw_support_soft_probe;                 /**< 支持软探针功能 */
	bool sw_support_ebike_detect;               /**< 支持梯控检测（电动车入侵） */
	bool sw_support_ebike_detect_sensitivity;   /**< 支持电动车入侵检测 灵敏度设置*/
	bool sw_support_bicycle_detect;             /**< 支持自行车入侵检测 */
	bool sw_support_video_encode_modify;        /**< 支持修改视频编码格式[h264/h265]，需要处理回调 DOT_CMD_SET_VIDEO_ENCODE  */
	bool sw_support_1400_protocol;              /**< 支持1400协议 */
	bool sw_support_wifi_signal;                /**< 支持获取wifi信号强度，需响应回调 DOT_CMD_GET_WIFI_SIGNAL_LEVEL*/
	bool sw_support_motor_vehicle_detect;       /**<   支持机动车入侵检测 */
	bool sw_support_algorithm_control;          /**<  支持算法切换 */
} HY_DVR_DEVICE_SW_CAPABILITY;

/** 设备软件能力集属性扩展(中国移动) */
typedef struct {
	int nValid;
	bool sw_support_mask_detect;          /**<  口罩支持 */
	bool sw_support_exit_flag;			   /**<  支持车辆出口标记 */
	bool sw_support_motor_vehicle_detect_region;   /**<  支持机动车检测区域 */
	bool sw_support_falling_object_sensitivity;   /**<  高空抛物灵敏度设置 */
	char cReserve[128 - 1];	
} HY_DVR_DEVICE_SW_CAPABILITY_EX;

typedef struct tagHY_DVR_CMDOT_DEVICE_STATUS
{
	int nValid;
	int nStatus;							//0,离线，1，在线，2，设备解绑,3.绑定成功，4，绑定失败
}HY_DVR_CMDOT_DEVICE_STATUS;

//天翼V2

#define TIANYIV2_PIC_MAX_SIZE   384*1024   	//抓图一张图片大小   最多允许384K  用堆空间分配   超过此大小时建议将图片进行缩放再给协议
#define TIANYIV2_DEVICE_LENS_COUNT_MAX   1  //设备支持的最大镜头数
#define TIANYIV2_DEVICE_FENCE_PIONT_COUNT_MAX  10  //一个电子围栏最多允许的坐标点个数
#define TIANYIV2_DEVICE_FENCE_REGION_COUNT_MAX   4  //电子围栏最多允许的区域个数
#define TIANYIV2_ZIP_PIC_NUM       500     //一次最多允许传的背景图

#define TIANYIV2_1400_MAX_FACE_NUM (500)
#define TIANYIV2_1400_MAX_PLATE_NO_NUM (500)
#define TIANYIV2_ALARM_SOUND_MAX_NUM (50)

// 图片发送模式
typedef enum eTIANYIV2_PIC_SEND_MODE {
	TIANYIV2_ONE_PIC_SEND = 0,     //普通模式，传单张图片
	TIANYIV2_ZIP_PIC_SEND = 1,     //zip模式
	TIANYIV2_FOLDER_PIC_SEND = 2,  //zip对应的文件夹模式
	TIANYIV2_1400_PIC_SEND = 3,    //1400传图方式
}eTIANYIV2_PIC_SEND_MODE;


// AI模式
typedef enum eTIANYIV2_AI_MODE {
	TIANYIV2_FACE_MODE = 0,  //人脸模式
	TIANYIV2_CARNUM_MODE = 1,  //车牌模式
	TIANYIV2_CARNUM_FACE_OFF = 2,  //人脸与车牌全关
	TIANYIV2_AI_SET_LOST = 1000,  //异常模式，协议端丢弃
}eTIANYIV2_AI_MODE;

//设备初始化参数:中间层提供
typedef struct tagHY_DVR_TIANYIV2_INIT_INFO
{
	int nValid;
	char strSystemPath[256];			//系统目录，用于放设备参数
	char strUserConfigPath[256];		//用户配置目录
	char strDeviceName[64];				//设备名称
	char strDeviceModel[64];			//设备型号
	int nDeviceType;					//设备类型 1.摄像机；2.分体门铃；3.单体门铃；4.NVR
	char strPucDevSn[64];				//设备UID
	char strpucDevKey[64];				//设备密码
	char strCTEI[64];					//CTEI固定设备前缀   CTEI=CTEI前缀+CTEI序列号  中间层组好后给协议
	int nSysMemLevel;					// 中间层提供此参数:能够给协议使用的内存  0:不足(存一帧)，1:适中(存一个GOP)  2:足量(3个以上GOP)
	int nLogFileSize;                   // 根据设备的flash大小决定，一个日志文件可以写多大空间 一般设备64*1024  128*1024等  最大1024*1024
	int nLogFileNum;					// 根据设备的flash大小决定，最多可以写12个日志文件
	char strReserved[128-8];
}HY_DVR_TIANYIV2_INIT_INFO;


//设备状态  告诉中间层目前设备所处状态
typedef struct tagHY_DVR_TIANYIV2_DEVICE_STATUS
{
	int nValid;
	int nDeviceStatus;   //0:开始注册   1:获取网络地址  2:网络连接完成  3:设备注册  4:设备等待绑定  5:设备已上线    6:设备解绑
	char strReserved[128];
}HY_DVR_TIANYIV2_DEVICE_STATUS;


typedef struct tagHY_DVR_TIANYIV2_LENS_INF
{
	int nValid;
	int nLedId;  			 //镜头ID
	char strPucLenName[64];  //镜头名
	int nLensType;  		 //镜头类型 0:EN_ZJ_CAMERA_LENS_TYPE_NORMAL  1:EN_ZJ_CAMERA_LENS_TYPE_360 2:EN_ZJ_CAMERA_LENS_TYPE_720
	double nFocalLenth;  	 //焦距
	char strReserved[128];
}HY_DVR_TIANYIV2_LENS_INF;

//设备有关的能力
typedef struct tagHY_DVR_TIANYIV2_MEDIA_ABILITY
{
	int nValid;
	int nChannelNum; 		//视频通道数 ipcam = 1 
	int nVoicePlayFlag;		//语音对讲标志，2-双工 1-单工，0-不支持
	int nPtzFlag;          	//0x00：不支持；0x01: 支持P；0x02: 支持T；0x04: 支持Z；按位与
	int nPtzSpeedFlag;    	//云台速度标志，1-支持，0-不支持
	int nAttachPTZFlag;		//外挂云台PTZ支持
	int nPresetPointFlag; 	//预置点设置标志，1-支持，0-不支持
	int nOSDSetFlag; 		//OSD 设置标志，1-支持，0-不支持
	int nMicroPhoneFlag;  	//麦克风设置支持标志，1-支持，0-不支持
	int nHumCountFlag;     //1-支持客流统计，0-不支持客流统计
	int nAiFaceFlag;       //1-设备支持人脸识别功能，0-设备不支持人脸识别功能

	int nSDCardFlag;  		//本地SD存储状态 0-没有存储介质，1-有存储介质且可录像
	char strMediaStoragePath[256];  //媒体存储路径，路径后不带'/'或'\'符号，为全路径；若设置路径为空表示没有SD卡；
	char strDevSoudFilePath[256];	//声音文件存储路径，路径后不带'/'或'\'符号，为全路径
	int nImageInversion;    		//图像翻转能力支持 0.不支持； 1.支持

	int nLensCount;					//镜头数量
	HY_DVR_TIANYIV2_LENS_INF lens_info[TIANYIV2_DEVICE_LENS_COUNT_MAX];
	int nMaxFacePic;              //人脸库最多允许的图片数
	int n1400Flag;                //1-支持1400协议，0-不支持1400协议
	int nCloudSpeak;              //1-支持云广播，0-不支持云广播
	int nExternalSpeakerAbility;  //外接音频设备(音柱)的能力 1--支持   0--不支持
	int nMaskFlag;				//口罩识别是否支持   1-- 支持  0-- 不支持
	int nCustomizeAlarmSoundAbility;   //支持自定义语音   默认写1
	int nInnerLampSwitch;              //是否支持SDK控制开关亮灭  1--支持  0--不支持
	int nReturnErrAbility;             //AI互斥能力   异常返回支持   写1
	int nFenceMotorVehicle;               //区域入侵扩展检测:机动车 1--支持  0--不支持
	int nFenceNonMotorVehicle;               //区域入侵扩展检测:非机动车	1--支持  0--不支持 
	int nOsdCodeFormat;                 //OSD编码下发格式  0.unicode  1.utf8
	int nDeviceNotHasRTC;               //设备是否有RTC  0:有RTC   1:无RTC
	int nIpV6Support;                   //设备是否支持iPV6  0:不支持   1:支持
	int nBatteryBikeAbility;            //电动车能力   0:不支持 1:支持
	int nSupportAwakeAbility;           //休眠与唤醒能力 0: 不支持 1: 支持
	int nSupportWired;                  //有线配网 0: 不支持 1: 支持
	int nSupportWifi;                   //WIFI配网(二维码配网) 0: 不支持 1: 支持

	char strReserved[128 - 56 - 12];
}HY_DVR_TIANYIV2_MEDIA_ABILITY;

//设备有关的能力--新增
typedef struct tagHY_DVR_TIANYIV2_MEDIA_ABILITY_EX
{
	int nValid;
	char strAIFaceCachePath[128];  //人脸布控的缓存入库图片路径
	char strReserved[1024];
}HY_DVR_TIANYIV2_MEDIA_ABILITY_EX;

//设备音频有关的参数配置
typedef struct tagHY_DVR_TIANYIV2_AUDIO_PARAM
{
	eVoiceMode nEncodeType;    //编码格式
	unsigned int nSampleRate;    //采样率
	unsigned int nChannel;       //声道数
	unsigned int nDepth;         //深度
	char strReserved[128];
}HY_DVR_TIANYIV2_AUDIO_PARAM;


//小喇叭的参数
typedef struct tagHY_DVR_TIANYIV2_SMALL_SPEAK
{
	int nValid;
	char strStartSmallSpeakPath[256];   //小喇叭初始音全路径 例:/speech/chinese/small_speaker.g711a  
	int nSpeakBuffFrameCount;//解码的缓存
	int nSpeakBuffAoCount;//AO的缓存
	int nSpeakBuffCustomFlag;  // 0:表示解码缓存频率协议控制，协议让中间层一直有25帧数据缓存   1:表示中间层自己控制多少帧缓存
	int nSpeakBuffFrameAllow;  //nSpeakBuffCustomFlag为1时，有效。  中间层允许的缓存帧数
	char strReserved[512 - 4 - 4 - 4 - 4];
}HY_DVR_TIANYIV2_SMALL_SPEAK;

//OSD开关设置: 协议通过此开发将对应的osd功能开与关
typedef struct tagHY_DVR_TIANYIV2_OSD_FLAG
{
	int nValid;
	int nOsdFlag;   //代码osd的设置开关。 0:表示关闭对应OSD    1:表示打开对应的OSD功能
	char strReserved[32];
}HY_DVR_TIANYIV2_OSD_FLAG;

//OSD设置  协议将对应位置和显示内容给中间层，中间层再显示
typedef struct tagHY_DVR_TIANYIV2_OSD_SET
{
	int nValid;
	int nPos;   //1.左上；2.左下；3.右上；4.右下；0.默认
	char strOSDName[256];
	char strReserved[128];
}HY_DVR_TIANYIV2_OSD_SET;

typedef struct tagHY_DVR_TIANYIV2_PIC_PARAM
{
	int nValid;
	int nIRLedFlag;         //0:不支持灯板红外模式切换   1:支持灯板红外模式切换
	int nSetIRLedMode;		//0:自动模式  ,  1:红外模式  2:全彩模式
	char strReserved[128];
}HY_DVR_TIANYIV2_PIC_PARAM;

//向中间层要红外的开和关状态
typedef struct tagHY_DVR_TIANYIV2_IR_STATUS
{
	int nValid;
	int nIRLedStatus;         //0:白天   1:晚上
	char strReserved[128];
}HY_DVR_TIANYIV2_IR_STATUS;

//电信工程师反馈此接口暂时没使用，可以先不管
typedef struct tagHY_DVR_TIANYIV2_SD_REPAIR
{
	int nValid;
	int nCheckSdCardError;	//SD卡检测修复   0:无异常
	char strReserved[128];
}HY_DVR_TIANYIV2_SD_REPAIR;

//删除声音文件
typedef struct tagHY_DVR_TIANYIV2_SOUND_FILE_DELETE
{
	int nValid;
	char strName[256];     //通知中间层要删除的音频文件  文件名称
	char strReserved[128];
}HY_DVR_TIANYIV2_SOUND_FILE_DELETE;

typedef struct tagHY_DVR_TIANYIV2_SOUND_FILE_INFO
{
	int nValid;
	unsigned char strFileName[256];		//文件名称
	unsigned int nFileSize;				//文件大小 单位:B
	unsigned int nFileFormat;           //EN_ZJ_FILE_FORMAT  1:wav格式
	unsigned char strFileContent[256];    //中文描述  目前不用写，置空即可
	unsigned int nFileCategory;         //EN_ZJ_RING_TYPE   //  铃声类型： 1 门铃提示音  2 门铃 4 警戒音,0X07  三种声音
	char strReserved[128];
}HY_DVR_TIANYIV2_SOUND_FILE_INFO;

//获取声音列表
typedef struct tagHY_DVR_TIANYIV2_SOUND_FILE_LIST
{
	int nValid;
	unsigned int nTotalSize;         //目录总大小   单位:B
	unsigned int nFreeSize;			//目录使用的大小 单位:B
	unsigned int nSoundNum;			//文件数
	HY_DVR_TIANYIV2_SOUND_FILE_INFO struSoundInfo[TIANYIV2_ALARM_SOUND_MAX_NUM];
	char strReserved[128+256];
}HY_DVR_TIANYIV2_SOUND_FILE_LIST;

//图片获取 协议找中间层获取一幅实时画面图片
typedef struct tagHY_DVR_TIANYIV2_PHOTO
{
	int nValid;
	char nPicType; 			//1:普通图片，2:中等大小图片，3:小图，4:ICON图片；
	unsigned char strPhoto[TIANYIV2_PIC_MAX_SIZE];
	char strReserved[128];
}HY_DVR_TIANYIV2_PHOTO;


typedef struct tagHY_DVR_TIANYIV2_SNAP_PARAM
{
	int nValid;
	unsigned int nLoopFlag;   //循环抓图标志
	unsigned int nFullFlag;   //完整抓图标志
	unsigned int nInterval;   //抓图间隔
	unsigned int nPicType;    //抓图类型
	char strReserved[128];
}HY_DVR_TIANYIV2_SNAP_PARAM;

typedef struct tagHY_DVR_TIANYIV2_RECORD_PARAM
{
	int nValid;
	unsigned int uiLoopFlag; //循环录制标志
	unsigned int uiFullFlag; //录制策略，1全天录制，0按策略录制
	unsigned int uiStreamId; //录制码流选择。0.主码流；1.次码流
	char strReserved[128];
}HY_DVR_TIANYIV2_RECORD_PARAM;

typedef struct tagHY_DVR_TIANYIV2_AI_EVENT
{
	int nValid;
	unsigned int nBuzzerFlag;				//蜂鸣器开关 1 开启，0关闭
	unsigned int nSenstive; 				//灵敏度
	unsigned int nEventInterval; 			//触发报警时，时间间隔
	unsigned int nStatus;					//开启状态
	unsigned int nTrace;					//开启自动跟随 

	unsigned int nRecordFlag;				//触发报警时，是否开启录像
	unsigned int nSnapFlag;					//触发报警时，是否开启抓图
	unsigned int nEventFlag;				//触发报警时，是否事件上报

	unsigned int nDuration;					//触发报警时，录像持续时间
	unsigned int nActiveTime;				//侦测到物体持续时间超过设置值才触发报警
	unsigned int nSoundType;				//外机门铃的铃声类型（配置蜂鸣器iot作为输出的时候使用，选择蜂鸣器的播放声音类型：警戒音、自定义声音）
	unsigned char strSoundFile[64];			//声音文件名

	HY_DVR_TIANYIV2_RECORD_PARAM struRecordParam;    //录像设置
	HY_DVR_TIANYIV2_SNAP_PARAM struSnapParam;        //抓图设置
	char strReserved[128];
}HY_DVR_TIANYIV2_AI_EVENT;

typedef struct tagHY_DVR_TIANYIV2_IOT_POLICY
{
	int nValid;
	unsigned int nAiIoType;		//与EN_ZJ_AIIOT_TYPE定义保持一致
	unsigned long long nAIIoTID; //摄像机ID（一般为0）
	unsigned int nOpenFlag;		//报警总开关  1:开启   0:关闭
	unsigned int nEventId;		//开启移动侦测时置为0， 开启人形时置为1   开启人脸置2
	unsigned int nTraceFlag;    //是否开启镜头追踪 1开启，0关闭
	HY_DVR_TIANYIV2_AI_EVENT struAiEvent;

	char strReserved[128];
}HY_DVR_TIANYIV2_IOT_POLICY;

typedef struct tagHY_DVR_TIANYIV2_FENCE_POINT
{
	int nValid;
	int nX;
	int nY;
	char strReserved[128];
}HY_DVR_TIANYIV2_FENCE_POINT;

typedef struct tagHY_DVR_TIANYIV2_FENCE_REGION
{
	int nValid;
	int nFencePointNum;   //该区域多少个点有效:最少3个，最多10个
	int nFenceDir;      //区域报警方向  0:向里(进入) 1:向外(离开)  2:向里(进入)，向外(离开)均支持
	HY_DVR_TIANYIV2_FENCE_POINT struFencePoint[TIANYIV2_DEVICE_FENCE_PIONT_COUNT_MAX];
	int nStayTime;			//电子围栏允许逗留时间  为0，表示无需确发逗留事件
	int nPicCapture;        //0:不推图  1:推1080P图片
	char strReserved[128 - 8];
}HY_DVR_TIANYIV2_FENCE_REGION;

typedef struct tagHY_DVR_TIANYIV2_IOT_ALARM_FENCE
{
	int nValid;
	int nFenceRegionNum;//多少个区域有效
	HY_DVR_TIANYIV2_FENCE_REGION struFenceRegion[TIANYIV2_DEVICE_FENCE_REGION_COUNT_MAX];//此数组结构体的索引，对应电子围栏的索引
	char strReserved[128];
}HY_DVR_TIANYIV2_IOT_ALARM_FENCE;

typedef struct tagHY_DVR_TIANYIV2_BATTERYBIKE_REGION
{
	int nValid;
	int nPointNum;   //该区域多少个点有效
	int nPicCapture;        //0:不推图  1:推1080P图片
	HY_DVR_TIANYIV2_FENCE_POINT struPoint[TIANYIV2_DEVICE_FENCE_PIONT_COUNT_MAX];
	char strReserved[128];
}HY_DVR_TIANYIV2_BATTERYBIKE_REGION;

typedef struct tagHY_DVR_TIANYIV2_IOT_ALARM_BATTERYBIKE
{
	int nValid;
	int nRegionNum;//多少个区域有效
	int nInterval;//上报的间隔
	HY_DVR_TIANYIV2_BATTERYBIKE_REGION struRegion[TIANYIV2_DEVICE_FENCE_REGION_COUNT_MAX];//此数组结构体的索引，对应电子围栏的索引
	char strReserved[128];
}HY_DVR_TIANYIV2_IOT_ALARM_BATTERYBIKE;

typedef struct tagHY_DVR_TIANYIV2_IOT_AI_MODE_SET
{
	int nValid;
	eTIANYIV2_AI_MODE nAiModeSet;
	char strReserved[128];
}HY_DVR_TIANYIV2_IOT_AI_MODE_SET;

typedef struct tagHY_DVR_TIANYIV2_IOT_FACE_MODE_SET
{
	int nValid;
	int nFaceModeSet;     //0:关人脸抓拍和人脸识别  1:人脸抓拍    2:人脸识别
	char strReserved[128];
}HY_DVR_TIANYIV2_IOT_FACE_MODE_SET;

typedef struct tagHY_DVR_TIANYIV2_IOT_ALARM_LAMP
{
	int nValid;
	int nCtrlType;         //0:关   1:开
	int nLampWorkTime;    //声光模式，白光灯亮的持续时间
	int nFlicker;         //是否闪烁
	char strReserved[128];
}HY_DVR_TIANYIV2_IOT_ALARM_LAMP;

typedef struct tagHY_DVR_TIANYIV2_ALARM_VOICE
{
	int nValid;
	int nCtrlType;         //0:关   1:开
	int nAlarmVoice;     //声音序列
	char strReserved[128];
}HY_DVR_TIANYIV2_ALARM_VOICE;

typedef struct tagHY_DVR_TIANYIV2_ALARM_VOICE_EX
{
	int nValid;
	int nCtrlType;         //0:关   1:开
	char strSoundName[256];      //语音名称
	int nLoopCnt;         //播放次数
	int nSoundType;     //2:自定义的警戒音  4:默认语音
	int nStartTime;     //告警开始时间
	int nEndTime;       //告警结束时间
	int nAlarmType;     //0:区域入侵触发告警    1:口罩触发告警  2:电动车
	char strReserved[512 - 12];
}HY_DVR_TIANYIV2_ALARM_VOICE_EX;


typedef struct tagHY_DVR_TIANYIV2_ALARM_CUSTOM_SOUND
{
	int nValid;
	int nFenceSoundExist;   //自定义警戒语音是否存在  0:不存在   1:存在
	int nFenceStartTime;    //开始告警的时间
	int nFenceEndTime;      //结束告警的时间
	char strReserved[512];
}HY_DVR_TIANYIV2_ALARM_CUSTOM_SOUND;

//此处注意：控制指示灯开关，只在设备正常的情况下有效，设备需要指示状态时（如启动、断网等），
//仍受厂家控制。厂家需要记录SDK回调函数设置的开关值，待设备正常时，在根据此开关值控制指示灯。
typedef struct tagHY_DVR_TIANYIV2_IOT_INNER_STATE_LAMP
{
	int nValid;
	int nCtrlType;         //0:关   1:开     
	char strReserved[512];
}HY_DVR_TIANYIV2_IOT_INNER_STATE_LAMP;

typedef struct tagHY_DVR_TIANYIV2_ALARM_SPEAK_STATUS
{
	int nValid;
	int nStatus;         //0:没有播放警报音   1:正在播放警报音
	char strReserved[128];
}HY_DVR_TIANYIV2_ALARM_SPEAK_STATUS;

//人脸库的人数
typedef struct tagHY_DVR_TIANYIV2_IOT_FACE_LABEL_MEMBER
{
	int nValid;
	int nCurMember;   //已录入人脸个数
	int nTotalMember; //数据库支持的人脸特征值个数
	char strReserved[128];
}HY_DVR_TIANYIV2_IOT_FACE_LABEL_MEMBER;

//人脸特征
typedef struct tagHY_DVR_TIANYIV2_IOT_FACE_ATTR_INFO
{
	int nValid;
	unsigned char		cAge;							/*!< 年龄：0-未知，其它表示年龄 			*/
	unsigned char		cSex;							/*!< 性别：0-未知，1-男性，2-女性			*/
	unsigned char		cSkin;							/*!< 肤色：0-未知，1-黄色，2-白色，3-黑色，4-棕色*/
	unsigned char		cBeard; 						/*!< 胡须：0-未知，1-有胡须，2-无胡须		*/
	unsigned char		cGlasses;						/*!< 眼镜：0-未知，1-戴眼镜，2-无眼镜		*/
	unsigned char		cMask;							/*!< 口罩：0-未知，1-有口罩，2-无口罩		*/
	char szReserved[64];
}HY_DVR_TIANYIV2_IOT_FACE_ATTR_INFO;

//识别记录
typedef struct tagHY_DVR_TIANYIV2_IOT_PIC_SNAP_INFO
{
	int nValid;
	char  nPicId[128];    //扣图所匹配的目标数据库的ID
	int nPicLen;	//扣图大小
	int uiSimilarity;     //相似度
	char* ptrPicBuff; //图片buff
	int nX;
	int nY;
	int nWidth;
	int nHeight;
	HY_DVR_TIANYIV2_IOT_FACE_ATTR_INFO struFaceAttrInFo;
	char szReserved[512 - 6 - 4 - 64];
}HY_DVR_TIANYIV2_IOT_PIC_SNAP_INFO;

//识别记录
typedef struct tagHY_DVR_TIANYIV2_IOT_FACE_RECOGNIZE
{
	int nValid;
	int nBgJpgLen;    //背景图片长度
	char* pBgJpgBuff; //背景图片地址
	int nPicNum;	//扣图个数
	HY_DVR_TIANYIV2_IOT_PIC_SNAP_INFO struPicSnapInFo;
	unsigned long long lluTimeStamp;		     //背景图时间  时间毫秒
	char szReserved[512 - 8];
}HY_DVR_TIANYIV2_IOT_FACE_RECOGNIZE;

//zip
typedef struct tagHY_DVR_TIANYIV2_IOT_AI_ZIP_EVENT
{
	int nValid;
	int nBgFileCount;								//背景图的数量
	unsigned char strFilePath[128];                  //绝对路径,压缩包sdk删除,压缩包文件名:UID_yyyyMMddHHmmss.zip  里面的背景图和扣图用 001.jpg   002.jpg依次命名下去   如:001.jpg(背景图)  002.jpg(扣图)  003.jpg(背景图)  004.jpg(扣图)
	unsigned long long lluTimeStamp[TIANYIV2_ZIP_PIC_NUM];		     //背景图时间
	unsigned char strCarNum[TIANYIV2_ZIP_PIC_NUM][16];                //车牌号
	char szReserved[128];
}HY_DVR_TIANYIV2_IOT_AI_ZIP_EVENT;


typedef struct tagHY_DVR_TIANYIV2_LOG_INFO
{
	int nValid;
	int nStatus;				//收集状态  0:表示log收集成功   -1:表示收集log失败
	char strLogPath[512];        //log的全路径:路径+log包名称  log包命名例示:DEV_6.2.19_xxx_2020-10-26-09-52-55.zip
								//打包文件包括:tyconfig中内容+自己的log
	char strReserved[128];
}HY_DVR_TIANYIV2_LOG_INFO;

typedef struct tagHY_DVR_TIANYIV2_NET_FLAG
{
	int nValid;
	int nNetFlag;     //网络状态 1:表示协议初始化完成，后面时间需要中间层通知协议网络变化情况
	char strReserved[128];
}HY_DVR_TIANYIV2_NET_FLAG;

typedef struct tagHY_DVR_TIANYIV2_1400_SWITH
{
	int nValid;
	int nStatus;     //1400开关
	char strReserved[128];
}HY_DVR_TIANYIV2_1400_SWITH;


typedef struct tagHY_DVR_TIANYIV2_1400_LOGIN_STATUS
{
	int nValid;
	int nStatus;     //1400开关状态  0:1400未上线   1:1400上线
	char strReserved[128];
}HY_DVR_TIANYIV2_1400_LOGIN_STATUS;

typedef struct tagHY_DVR_TIANYIV2_IPV6_SWITCH
{
	int nValid;
	int nSwitch;     //ipv6开关状态  0:关   1:开
	char strReserved[128];
}HY_DVR_TIANYIV2_IPV6_SWITCH;

typedef struct tagHY_DVR_TIANYIV2_1400_INFO
{
	int nValid;
	char str1400Id[128];    //1400设备Id
	char strDomain[256];    //1400服务器的URL
	char strReserved[128];
}HY_DVR_TIANYIV2_1400_INFO;


typedef struct tagHY_DVR_TIANYIV2_SYSTEM_DYNAMIC_CFG
{
	int nValid;
	int nCpuUsedRatio;    //CPU的利用率
	int nRamUsedRatio;    //内存的利用率
	int nWifiSignal;      //wifi信号强度(百分比)，范围 0-100
	char strReserved[128 -4];
}HY_DVR_TIANYIV2_SYSTEM_DYNAMIC_CFG;

typedef struct tagHY_DVR_TIANYIV2_CONFIG_NETWORK_QR_INFO
{
	int nValid;

	char szQRInfo[128];		//二维码扫描出来的字符串，中间层传入协议

	char szReserved[256];
}HY_DVR_TIANYIV2_CONFIG_NETWORK_QR_INFO, *LPHY_DVR_TIANYIV2_QR_CONFIG_NETWORK_QR_INFO;

typedef struct tagHY_DVR_TIANYIV2_CONFIG_NETWORK_WIFI_INFO
{
	int nValid;

	char szSSID[DVR_MAX_SSID_LEN];			//协议将解析到的SSID回填到结构体中
	char szPassword[DVR_MAX_WIFI_KEY_LEN];	//协议将解析到的密码回填到结构体中
	int iEncrytType;						//协议将解析到的加密类型回填到结构体中

	char szReserved[256];
}HY_DVR_TIANYIV2_CONFIG_NETWORK_WIFI_INFO, *LPHY_DVR_TIANYIV2_QR_CONFIG_NETWORK_WIFI_INFO;

typedef struct tagHY_DVR_TIANYIV2_PTZ_POS_INFO
{
	int nValid;

	int nPosition_x;	//当前云台水平方向上的坐标
	int nPosition_y;	//当前云台垂直方向上的坐标

	char szReserved[32];
}HY_DVR_TIANYIV2_PTZ_POS_INFO, *LPHY_DVR_TIANYIV2_PTZ_POS_INFO;

typedef struct tagHY_DVR_TIANYIV2_SYSTEM_NET_EX_CFG
{
	int nValid;
	char strUpMacAddr[DVR_MAX_MAC_STR_LEN];    
	char strReserved[128];
}HY_DVR_TIANYIV2_SYSTEM_NET_EX_CFG;

typedef struct tagHY_DVR_TIANYIV2_IOT_CREAT_AI_LABEL
{
	int nValid;
	char strLabelId[128];  //用户id号
	int nLableType;	       //1人脸  2 牌照
	char strReserved[512];
}HY_DVR_TIANYIV2_IOT_CREAT_AI_LABEL;

typedef struct tagHY_DVR_TIANYIV2_IOT_ADD_PIC_IN_LABEL
{
	int nValid;
	char strLabelId[128];  //用户id号
	char strPicId[128];		//图片id
	char* ptrPicBuff;		//图片指针
	int nPicLen;			//图片长度
	char strReserved[512];
}HY_DVR_TIANYIV2_IOT_ADD_PIC_IN_LABEL;

typedef struct tagHY_DVR_TIANYIV2_IOT_DELETE_PIC_IN_LABEL
{
	int nValid;
	char strLabelId[128];  //用户id号
	char strPicId[128];		//图片id
	char strReserved[512];
}HY_DVR_TIANYIV2_IOT_DELETE_PIC_IN_LABEL;

typedef struct tagHY_DVR_TIANYIV2_IOT_MODIFY_PIC_LABEL
{
	int nValid;
	char strPicId[128];				//图片id
	char strNewLabelId[128];  //新的用户id号
	char strReserved[512];
}HY_DVR_TIANYIV2_IOT_MODIFY_PIC_LABEL;

typedef struct tagHY_DVR_TIANYIV2_IOT_DELETE_AI_LABEL
{
	int nValid;
	char strLabelId[128];  //用户id号
	char strReserved[512];
}HY_DVR_TIANYIV2_IOT_DELETE_AI_LABEL;

typedef struct tagHY_DVR_TIANYIV2_IOT_HUMANCOUNT_SET_PARAM
{
	int nValid;
	int nOpenFlag; //1:打开   0:关闭
	int nInterval; //间隔   min
	char strReserved[512];
}HY_DVR_TIANYIV2_IOT_HUMANCOUNT_SET_PARAM;

typedef struct tagHY_DVR_TIANYIV2_IOT_MASK
{
	int nValid;
	int nOpenFlag; //1:打开口罩检测功能 0:关闭口罩检测功能
	int nMaskCapture; //1:开图    0:关图
	char strReserved[512 - 4];
}HY_DVR_TIANYIV2_IOT_MASK;

//return :0 成功，1 算法不支持，2 算法互斥，3 内存不足，4 算力不足，5 其他错误
//此接口 只是为了通知中间层目前要开启的算法情况,中间层只要确认这几种是否能够同时运行即可
typedef struct tagHY_DVR_TIANYIV2_IOT_AI_WORK_SWITCH
{
	int nValid;
	int nFenceSwitch;           //电子围栏开关  0:关  非0:开
	int nHumCountSwitch;        //客流统计开关  0:关  非0:开
	int nFaceDetectSWitch;     //人脸抓拍开关  0:关  非0:开
	int nFaceRecognitionSwitch; //人脸识别开关  0:关  非0:开
	int nCarNumSwitch;          //车牌开关  0:关  非0:开
	int nMaskSwitch;            //口罩开关  0:关  非0:开
	int nBatterybike;           //电动车  0:关  非0:开
	char strReserved[512 -4];
}HY_DVR_TIANYIV2_IOT_AI_WORK_SWITCH;

typedef struct tagHY_DVR_TIANYIV2_HUMANCOUNT_REGION
{
	int nValid;
	int nPointNum;   //该区域多少个点有效
	HY_DVR_TIANYIV2_FENCE_POINT struFencePoint[TIANYIV2_DEVICE_FENCE_PIONT_COUNT_MAX];
	char strReserved[128];
}HY_DVR_TIANYIV2_HUMANCOUNT_REGION;

typedef struct tagHY_DVR_TIANYIV2_IOT_HUMANCOUNT_SET_REGIONS
{
	int nValid;
	int nRegionNum;//多少个区域有效
	HY_DVR_TIANYIV2_HUMANCOUNT_REGION struRegion[TIANYIV2_DEVICE_FENCE_REGION_COUNT_MAX];//此数组结构体的索引，对应电子围栏的索引
	char strReserved[128];
}HY_DVR_TIANYIV2_IOT_HUMANCOUNT_SET_REGIONS;

//!\brief 人脸扣图数据
typedef struct tagHY_DVR_TIANYIV2_SNAP_FACE_JPG_INFO
{
	int nValid;
	char szFileName[32];  //人脸扣图名称  文件夹传图方式时填名称，1400单图传图方式时置空
	int nEventSort;
	char szImageSource[16];
	char szType[16];
	char szFileFormat[16];
	char szShotTime[32];
	int nWidth;
	int nHeight;

	char* pPicBuff;    //文件夹传图方式时填空，1400单图传图方式
	int nPicLen;
	char szReserved[128 - 8];
}HY_DVR_TIANYIV2_SNAP_FACE_JPG_INFO;

//!\brief 人脸背景图数据
typedef struct tagHY_DVR_TIANYIV2_FACE_BACK_JPG_INFO
{
	int nValid;
	char szFileName[32]; //人脸背景图名称  文件夹传图方式时填名称，1400单图传图方式时置空
	int nEventSort;  //10:人脸检测 11:人脸比对  12:车辆检测 13:车辆比对  16:车牌识别
	char szImageSource[16];
	char szType[16];
	char szFileFormat[16];
	char szShotTime[32];
	int nWidth;
	int nHeight;

	char* pPicBuff;    //文件夹传图方式时填空，1400单图传图方式
	int nPicLen;

	char szReserved[128 - 8];
}HY_DVR_TIANYIV2_FACE_BACK_JPG_INFO;

typedef struct tagHY_DVR_TIANYIV2_FACE_JPG_INFO
{
	int nValid;
	int nInfoKind;//0:其它  1:自动采集  2:人工采集
	int nLeftTopX;
	int nLeftTopY;
	int nRightBottomX;
	int nRightBottomY;
	int nIsSuspectedTerrorist;//是否涉恐人员 0：否；1：是；2：不确定
	int nIsCriminalInvolved;//是否涉案人员 0：否；1：是；2：不确定 
	int nIsDetainees;//是否在押人员 0：否；1：是；2：不确定，人工采集必填
	int nIsVictim;//是否被害人  0：否；1：是；2：不确定
	int nIsSuspiciousPerson;//是否可疑人  0：否；1：是；2：不确定

	HY_DVR_TIANYIV2_SNAP_FACE_JPG_INFO stFaceSnapInfo;//扣图信息
	HY_DVR_TIANYIV2_FACE_BACK_JPG_INFO stFaceBackInfo;//背景图信息
	unsigned long long lluTimeStamp;		     //背景图时间
	char szReserved[256 - 8];
}HY_DVR_TIANYIV2_FACE_JPG_INFO;

//\brief 人脸列表
typedef struct tagHY_DVR_TIANYIV2_FACE_INFO_LIST
{
	int nValid;
	int nFaceNum;                   //人脸扣图数量
	char szFaceInfoPath[128];         //人脸文件夹所在路径
	HY_DVR_TIANYIV2_FACE_JPG_INFO stFaceInfo[TIANYIV2_1400_MAX_FACE_NUM]; //人脸信息
	char szReserved[128];
}HY_DVR_TIANYIV2_FACE_INFO_LIST;

//!\brief 车牌扣图数据
typedef struct tagHY_DVR_TIANYIV2_SNAP_PLATENO_JPG_INFO
{
	int nValid;
	char szFileName[32];  //车牌扣图名称    文件夹传图方式时填名称，1400单图传图方式时置空
	int nEventSort;
	char szType[16];
	char szFileFormat[16];
	char szShotTime[32];
	int nWidth;
	int nHeight;

	char* pPicBuff;    //文件夹传图方式时填空，1400单图传图方式
	int nPicLen;

	char szReserved[128 - 8];
}HY_DVR_TIANYIV2_SNAP_PLATENO_JPG_INFO;

//!\brief 车牌背景图数据
typedef struct tagHY_DVR_TIANYIV2_PLATENO_BACK_JPG_INFO
{
	int nValid;
	char szFileName[32]; //车牌背景图名称    文件夹传图方式时填名称，1400单图传图方式时置空
	int nEventSort;//10:人脸检测 11:人脸比对  12:车辆检测 13:车辆比对  16:车牌识别
	char szType[16];
	char szFileFormat[16];//"jpeg"
	char szShotTime[32];//例:"20210914134438"
	int nWidth;
	int nHeight;

	char* pPicBuff;    //文件夹传图方式时填空，1400单图传图方式
	int nPicLen;
	char szReserved[128 - 8];
}HY_DVR_TIANYIV2_PLATENO_BACK_JPG_INFO;

typedef struct tagHY_DVR_TIANYIV2_PLATENO_JPG_INFO
{
	int nValid;
	int nInfoKind;   //0:其它  1:自动采集  2:人工采集
	int nLeftTopX;
	int nLeftTopY;
	int nRightBottomX;
	int nRightBottomY;
	int nHasPlate;   //有无车牌  1或0
	char szMotorVehicleID[64]; //车辆id
	char szPlateColor[32];  //车牌颜色
	char szPlateNo[32];  //车牌号
	char szDeviceID[32];
	char szPlateClass[32];  //class

	HY_DVR_TIANYIV2_SNAP_PLATENO_JPG_INFO stPlateSnapInfo;//扣图信息
	HY_DVR_TIANYIV2_PLATENO_BACK_JPG_INFO stPlateBackInfo;//背景图信息
	unsigned long long lluTimeStamp;		     //背景图时间
	char szReserved[256 - 8];
}HY_DVR_TIANYIV2_PLATENO_JPG_INFO;

//\brief 车牌列表
typedef struct tagHY_DVR_TIANYIV2_PLATENO_INFO_LIST
{
	int nValid;
	int nPlateNum;                   //车牌扣图数量
	char szPlateInfoPath[128];         //车牌文件夹所在路径
	HY_DVR_TIANYIV2_PLATENO_JPG_INFO stPlateInfo[TIANYIV2_1400_MAX_PLATE_NO_NUM]; //车牌信息
	char szReserved[128];
}HY_DVR_TIANYIV2_PLATENO_INFO_LIST;

typedef struct tagHY_DVR_LIGHT_WARNING
{
	int nValid;

	int nEnable;
	int nLightMode;
	int nDelayTime;

	HY_DVR_SCHEDTIME struSchedule;

    int nChannel;           // 通道号
	int iRedBlueLightFlashEnable; // 是否使能红蓝灯闪烁；0-否，1-是

	char szReserved[248];
}HY_DVR_LIGHT_WARNING, *LPHY_DVR_LIGHT_WARNING;

typedef struct tagHY_DVR_MEGLINK_CONFIG_EXT
{
	int nValid;

	int nMaskForBid;		//0  口罩禁入不启用 1 佩戴口罩禁入 2 未佩戴口罩禁入
	int nMaskTip;			//0  口罩提醒不启用 1 佩戴口罩提醒 2 未佩戴口罩提醒;

	char szReserved[256];
}HY_DVR_MEGLINK_CONFIG_EXT, *LPHY_DVR_MEGLINK_CONFIG_EXT;

typedef struct tagHY_DVR_RECORD_SCHEDULE_V2
{
	int nValid;

	int nChannelID;
	int nEnable;
	int nRecordType;		//0：连续录像	1：事件录像
	HY_DVR_TIME_V2 struStartTime[6];
	HY_DVR_TIME_V2 struStopTime[6];
	int iStreamType;	/*!< 0表示主码流,1表示子码流(CIF),2表示高清子码流(VGA) */

	char szReserved[252];
}HY_DVR_RECORD_SCHEDULE_V2, *LPHY_DVR_RECORD_SCHEDULE_V2;

typedef struct tagHY_DVR_RECORD_SCHEDULE_V2_EVENT_TIMESLOT_LIST
{
    int iValid;
    HY_DVR_SCHEDULE_TIMESLOT stSchedTimeSlot[SCHEDULE_TIMESLOT_MAX_NUM]; /*!< 布防时间计划 */
    char szReserved[1024];
}HY_DVR_RECORD_SCHEDULE_V2_EVENT_TIMESLOT_LIST, *LPHY_DVR_RECORD_SCHEDULE_V2_EVENT_TIMESLOT_LIST;

typedef struct tagHY_DVR_LOW_TEM_FIT_SWITCH
{
	int nValid;

	int nEnable;	//0:禁用	1：启用

	char szReserved[32];
}HY_DVR_LOW_TEM_FIT_SWITCH, *LPHY_DVR_LOW_TEM_FIT_SWITCH;

typedef struct tagHY_DVR_PROTOCOL_VERSION
{
	int nValid;

	int nProtocolID;
	char szVersion[64];
	char szVersionDate[16];

	char szReserved[256];
}HY_DVR_PROTOCOL_VERSION, *LPHY_DVR_PROTOCOL_VERSION;

typedef struct tagHY_DVR_PROTOCOL_VERSION_V1
{
	int iValid;

	int iProtocolID;
	char acBaseVersion[128];
	char acCurrentVersion[128];
	char acBaseVersionSVN[16];
	char acCurrentVersionSVN[16];
	char acBaseVersionDate[16];
	char acCurrentVersionDate[16];

	char szReserved[256];
}HY_DVR_PROTOCOL_VERSION_V1, *LPHY_DVR_PROTOCOL_VERSION_V1;

typedef struct tagHY_DVR_PTZ_CURISE_STATUS
{
	int nValid;

	int nChannelID;
	int nStatus;

	char szReserved[32];
}HY_DVR_PTZ_CURISE_STATUS, *LPHY_DVR_PTZ_CURISE_STATUS;

typedef struct tagHY_DVR_ALARM_OUT_SCHEDULE
{
	int nValid;

	int nChannelID;
	int nEnable;
	HY_DVR_TIME_V2 struStartTime[6];
	HY_DVR_TIME_V2 struStopTime[6];

	char szReserved[256];
}HY_DVR_ALARM_OUT_SCHEDULE, *LP_HY_DVR_ALARM_OUT_SCHEDULE;

typedef struct tagHY_DVR_ALARM_OUT_SCHEDULE_V1
{
    int nValid;

    int nChannelID;
    int nEnable;
    HY_DVR_TIME_V2 struStartTime[6];
    HY_DVR_TIME_V2 struStopTime[6];
    HY_DVR_SCHEDTIME struSchedTime;                      /*!< 布防计划 */

    char szReserved[1024];
}HY_DVR_ALARM_OUT_SCHEDULE_V1, *LP_HY_DVR_ALARM_OUT_SCHEDULE_V1;
 
typedef struct tagHY_DVR_LIGHT_WARNING_EX
{
    int iValid;
 
    int nChannel;                                                   /*!< 通道号 */
    HY_DVR_SCHEDULE_TIMESLOT stSchedTimeSlot[SCHEDULE_TIMESLOT_MAX_NUM];     /*!< 布防时间段计划 */
 
    char szReserved[1024];
}HY_DVR_LIGHT_WARNING_EX, *LPHY_DVR_LIGHT_WARNING_EX;

typedef struct tagHY_DVR_ALARMCFG_ONE_CLICK_ALARM_CONFIG
{
	int nValid;
 
	int nChannel;                            /*!< 通道号 */
	int nIsVoiceOpen;                        //是否开启声音报警；0-否，1-是
	int nIsLightOpen;                       //是否开启灯光报警；0-否，1-是
	int nIsAlarmOutOpen;                    //是否开启报警输出；0-否，1-是
	int nAlarmTime;                         //报警持续时长；单位秒，取值范围[0-60]
 
	char szReserved[128];
}HY_DVR_ALARMCFG_ONE_CLICK_ALARM_CONFIG, *LPHY_DVR_ALARMCFG_ONE_CLICK_ALARM_CONFIG;

typedef struct tagHY_DVR_ALARMCFG_ONE_CLICK_ALARM_STATUS
{
	int nValid;

	int nChannel;                            /*!< 通道号 */
	int nAlarmStatus;                       //报警状态；0-报警结束，1-报警中
	int nAlarmTime;                         //报警持续时长；单位秒，取值范围[0-60]

	char szReserved[64];
}HY_DVR_ALARMCFG_ONE_CLICK_ALARM_STATUS, *LPHY_DVR_ALARMCFG_ONE_CLICK_ALARM_STATUS;

typedef struct tagHY_DVR_ALARMCFG_ONE_CLICK_ALARM_CONTROL
{
    int nValid;
 
	int nChannel;                            /*!< 通道号 */
    int nAlarmControl;                        //报警控制；0-关闭一键报警，1-打开一键报警
 
    char szReserved[32];
}HY_DVR_ALARMCFG_ONE_CLICK_ALARM_CONTROL, *LPHY_DVR_ALARMCFG_ONE_CLICK_ALARM_CONTROL;

typedef struct tagHY_DVR_CURRENT_CARD_NUMBER
{
	int nValid;

	char szCardNumber[32];

	char szReserved[128];
}HY_DVR_CURRENT_CARD_NUMBER, *LPHY_DVR_CURRENT_CARD_NUMBER;

typedef struct tagHY_DVR_CARD_READ_MODE
{
	int nValid;

	int nCardReadMode;

	char szReserved[32];
}HY_DVR_CARD_READ_MODE, *LPHY_DVR_CARD_READ_MODE;

typedef struct tagHY_DVR_FACE_SNAPSHOT_MODE
{
	int nValid;

	int nMode;		//0：正常模式（人脸识别模式）	1：人脸抓拍模式（此模式下web不停的尝试获取面板机抓取到的人脸，获取到后进行入库操作）

	char szReserved[128];
}HY_DVR_FACE_SNAPSHOT_MODE, *LPHY_DVR_FACE_SNAPSHOT_MODE;

typedef struct tagHY_DVR_FACE_SNAPSHOT_IMAGE
{
	int nValid;

	char *pBuffer;
	int nBufferLength;

	char szReserved[64];
}HY_DVR_FACE_SNAPSHOT_IMAGE, *LPHY_DVR_FACE_SNAPSHOT_IMAGE;

typedef enum
{
	CMCC_HJGH_TYPE = 0,//家业模式
	CMCC_PBX_TYPE,
	CMCC_GE_TYPE,//政企模式
}HY_DVR_CM_VOIP_BUSINESS_TYPE;

typedef struct tagHY_DVR_CM_VOIP_DEVICE_CFG
{
	int nValid;
	char szOsVersion[32];
	char szImageVersion[32];
	char szHardwareVersion[32];
	char szDeviceModel[32];
	char szSerialNumber[32];
	char szCMEI[32];
	char szMacAddress[32];
	char szChipModel[32];
	char szChipUUID[128];
	char szLogPath[128];   /*/tmp/call.txt*/
	int  nLogSize;
	int  nBusinessType;  // 中间层需写0,家业模式
	char szAppKey[32];
	char szAppSecret[32];
	char szDeviceId[64];
}HY_DVR_CM_VOIP_DEVICE_CFG, *LPHY_DVR_CM_VOIP_DEVICE_CFG;


typedef struct tagHY_DVR_CM_VOIP_DEVICE_CFG_EX
{
	int nValid;
	int nSupportTalkAudioFormat; //0,PCM,1,G711U,2,G711A
	char cReserve[1024];
}HY_DVR_CM_VOIP_DEVICE_CFG_EX, *LPHY_DVR_CM_VOIP_DEVICE_CFG_EX;


typedef struct tagHY_DVR_NFC_BASIC_CFG
{
	int nValid;

	int nEnable;

	char szReserved[32];
}HY_DVR_NFC_BASIC_CFG, *LPHY_DVR_NFC_BASIC_CFG;

typedef struct tagHY_DVR_FACE_SMART_EXPOSURE
{
	int nValid;

	int nChannelID;
	int nEnable;
	int nWorkThreshold;
	int nFaceBrightness;

	char szReserved[32];
}HY_DVR_FACE_SMART_EXPOSURE, *LPHY_DVR_FACE_SMART_EXPOSURE;

typedef struct tagHY_DVR_DOUBLE_CAMERA_CALIBRATION
{
	int nValid;

	int nChannelID;
	int nDistance;    //单位：cm
	int nStatus; 		//0：未标定	1：已标定

	char szReserved[64];
}HY_DVR_DOUBLE_CAMERA_CALIBRATION, *LPHY_DVR_DOUBLE_CAMERA_CALIBRATION;

typedef struct tagHY_DVR_RECOGNIZE_DISTANCE
{
	int nValid;

	int nChannelID;
	int nDistance;    //单位：cm

	char szReserved[32];
}HY_DVR_RECOGNIZE_DISTANCE, *LPHY_DVR_RECOGNIZE_DISTANCE;

typedef struct tagHY_DVR_PERSON_ELEVATOR_CONFIG
{
	int nValid;

	int nChannelID;
	long long nPersonID;
	char szUniqueID[32];
	char szElevatorFloor[512];    //以128为地上地下分界，szElevatorFloor[128]为1层，szElevatorFloor[129]为2层；szElevatorFloor[127]为-1层，szElevatorFloor[126]为-2层

	char szReserved[1024];
}HY_DVR_PERSON_ELEVATOR_CONFIG, *LPHY_DVR_PERSON_ELEVATOR_CONFIG;

typedef struct tagHY_DVR_ELEVATOR_BASIC_CONFIG
{
	int nValid;

	int nChannelID;
	int nEnable;    //0：禁用	1：启用	
	int nMode;	//0：梯控模式	1：消防模式
	short nDownLimit;	//楼层范围下限
	short nUpLimit;			//楼层范围上限

	char szReserved[32];
}HY_DVR_ELEVATOR_BASIC_CONFIG, *LPHY_DVR_ELEVATOR_BASIC_CONFIG;

typedef struct tagHY_DVR_WATCH_CARE_PRESET
{
	int nValid;

	int nChannelID;
	int nPreset;
	char cType;    /* 点类型(旧版本选择已设置的预置点中的某一个为看守位,新版本要求独立分开预置点和看守位,通过该字段区别);0-表示以nPreset对应的预置点为看守位(旧版本逻辑)，1-独立的看守位(新版本逻辑，此时nPreset表示看守位编号，目前仅有一个看守位可忽略) */
    char cWatchCareEnable;  /* 是否使能看守位(新版本cType=1时有效)；0-去使能，1-使能 */

	char szReserved[28];
}HY_DVR_WATCH_CARE_PRESET, *LPHY_DVR_WATCH_CARE_PRESET;

typedef struct tagHY_DVR_WATCH_CARE_PRESET_ATTRIBUTE
{
    int iValid;
 
    int iChannelID;/* 通道编号，从0开始递增 */
    int iWatchCareIndex;   /* 看守位编号，[0-255] */
    int iReturnTime;   /* 回归看守位时间；单位秒，默认15s，范围[10-30] */
 
   char acReserved[128];
}HY_DVR_WATCH_CARE_PRESET_ATTRIBUTE, *LPHY_DVR_WATCH_CARE_PRESET_ATTRIBUTE;

typedef struct tagHY_DVR_PTZ_PRESET_INFO
{
    int iValid;
 
    int iEnable;				/* 看守位开关(必选);0-关闭,1-打开 */
    int nChannelID;				/* 通道号 */
    int iPresetIndex;			/* 调用预置位编号 */
    int iResetTime;				/* 自动归位时间间隔，单位：秒 */
    int iStayTime;				/* 预置点停留时间,单位:秒 */
    int iSpeed;					/* 云台速度:1-15 */
 
   char szReserved[128];
}HY_DVR_PTZ_PRESET_INFO, *LPHY_DVR_PTZ_PRESET_INFO;
 
#define MAX_PTZ_PRESETS 256
typedef struct tagHY_DVR_HOME_POSITION_INFO
{
    int iValid;
 
    int iChannelID;
    HY_DVR_PTZ_PRESET_INFO stPresetList[MAX_PTZ_PRESETS]; /* 预置点列表 */
 
    char szReserved[256];
}HY_DVR_HOME_POSITION_INFO, *LPHY_DVR_HOME_POSITION_INFO;

typedef struct tagHY_DVR_PTZ_CALIBRATION_CFG
{
    int iValid;

    int iCalibrationInterval;  /*!< 云台校准间隔;0:立即校准,4:4小时,12:12小时,24:24小时 */

    char acReserved[256];
} HY_DVR_PTZ_CALIBRATION_CFG, *LPHY_DVR_PTZ_CALIBRATION_CFG;

typedef struct tagHY_DVR_PUSH_EVENT_INTERVAL
{
	int nValid;

	int nChannelID;
	int nMotionInterval;		//移动侦测事件推送间隔，单位:s
	int nPeopleInterval;		//人形事件推送间隔，单位:s
	int nAssistLineInterval;		//拌线时事件推送间隔，单位:s
	int nAlarmInInterval;		//报警输入推送间隔，单位:s
	int nCarInterval;	//车形事件推送间隔，单位:s
	int iAreaDetectInterval;        //区域侦测事件推送间隔，单位:s
    int iCrossBorderDetectInterval; //越界侦测事件推送间隔，单位:s
    int iOffDutyDetectInterval;     //离岗检测事件推送间隔，单位:s
    int iPassgerStaticsInterval;    //客流统计事件推送间隔，单位:s
    int iFireDetectInterval;        //火焰检测事件推送间隔，单位:s
    int iCryScreamInterval;         //哭声叫声检测事件推送间隔，单位:s
	int iTrafficStatisticsInterval; //车流统计事件推送间隔，单位:s
}HY_DVR_PUSH_EVENT_INTERVAL, *LPHY_DVR_PUSH_EVENT_INTERVAL;

typedef struct tagHY_DVR_PEOPLE_DETECT_STAY_TIME
{
	int nValid;

	int nStayTime;		//单位:s

	char szReserved[32];
}HY_DVR_PEOPLE_DETECT_STAY_TIME, *LPHY_DVR_PEOPLE_DETECT_STAY_TIME;

typedef struct tagHY_DVR_AGING_TEST
{
	int nValid;

	int nChannelID;
	int nEnable;
	int nInterval;		//单位：ms

	char szReserved[32];
}HY_DVR_AGING_TEST, *LPHY_DVR_AGING_TEST;

typedef struct tagHY_DVR_CAR_DETECT
{
	int nValid;		/*!< 本结构体是否有效，0表示无效  */

	int nChannelID;
	int nEnable;			/*!< 是否开启车形侦测  */
	int nEnableTrack;		/*!< 是否开启车型跟踪  */
	int nSenstive;			/*!< 车形侦测灵敏度, 0 - 5,越低越灵敏,0xff关闭  */
	int nEnableRegionMode;	/*!< 是否启用区域模式		   */
	int nEnableTrackZoom;		/*!< 是否启用跟踪放大		   */
	int nAlarmMode;			/*!< 报警模式：0：自定义		1：持续		   */
	int nAlarmDuration;		/*!< 报警持续时间，单位：秒	*/
	int nIsPushCarImage;		/*!< 车形事件是否推图			*/
	int iEnableCarFrame;        /*!< 是否显示车形框;0-否,1-是  */
	char cReserve[60];			/*!< 保留 	*/

	HY_DVR_HANDLEEXCEPTION struHandleType;	/*!< 处理方式  */
	HY_DVR_SCHEDTIME struSchedTime;													/*!< 布防计划  */

	char szReserved[1024];
}HY_DVR_CAR_DETECT, *LPHY_DVR_CAR_DETECT;

typedef struct tagHY_DVR_ANIMAL_DETECT
{
    int iValid;

    int iEnable;                                                        /*!< 是否使能动物检测;0-否,1-是  */
    int iEnableAnimalFrame;                                             /*!< 是否显示动物框;0-否,1-是  */
    int iIsPushAnimalImage;                                             /*!< 动物检测事件是否推图;0-否,1-是  */

    char szReserved2[508];
}HY_DVR_ANIMAL_DETECT, *LPHY_DVR_ANIMAL_DETECT;

typedef struct tagHY_DVR_ANIMAL_DETECT_CFG
{
    int iValid;

    HY_DVR_ANIMAL_DETECT stAnimalDetect[DVR_MAX_VIDEOIN_NUM];
    char cRevered[1024];
}HY_DVR_ANIMAL_DETECT_CFG, *LPHY_DVR_ANIMAL_DETECT_CFG;

typedef struct tagHY_DVR_PICCFG_PEOPLE_TRACK_SCHEDULE
{
    int iValid;

    HY_DVR_SCHEDTIME struSchedTime;                      /*!< 布防计划 */

    char szReserved[1024];
}HY_DVR_PICCFG_PEOPLE_TRACK, *LPHY_DVR_PICCFG_PEOPLE_TRACK;

typedef struct tagHY_DVR_PICCFG_MOVE_TRACK
{
    int iValid;

    int iEnable;                        /*!< 使能;0-去使能,1-使能 */
    HY_DVR_SCHEDTIME struSchedTime;     /*!< 布防计划 */

    char szReserved[1024];
}HY_DVR_PICCFG_MOVE_TRACK, *LPHY_DVR_PICCFG_MOVE_TRACK;

typedef struct tagHY_AUDIO_ALARM_CONFIG_V1
{
	int nValid;

	int nChannelID;
	int nEnable;			//0,false,1,true	
	int nPeopleAudioType;			//0：自定义，1：警报，2：犬吠，3：警戒区域，4：监控区域，5：欢迎光临，6：深水警告，7：私人领地，8：危险区域，9：贵重物品，10：高处危险，
										//11：私家价位，12：叮咚,		13:有口罩,		14:无口罩    15：注意私人物品     16：抓小偷     17：救命		18：分类		19：可回收垃圾		20：家庭食物垃圾		
										//21：危险废物		22：残余垃圾		23：全部分类	24：车形自定义		25：禁止停车
	int nCarAudioType;			//0：自定义，1：警报，2：犬吠，3：警戒区域，4：监控区域，5：欢迎光临，6：深水警告，7：私人领地，8：危险区域，9：贵重物品，10：高处危险，
										//11：私家价位，12：叮咚,		13:有口罩,		14:无口罩    15：注意私人物品     16：抓小偷     17：救命		18：分类		19：可回收垃圾		20：家庭食物垃圾		
										//21：危险废物		22：残余垃圾		23：全部分类	24：车形自定义		25：禁止停车
	int nAudioDelayOutput;	//单位为S
	int nAudioVolume;		//音频音量1~100，如果音量为0，那么音量设为默认值50
	int nTryListenType;		/* 0:人形	1：车形,2:火焰,3:报警输入(AlarmIn),4:视频遮挡(VideoShelter),5:移动侦测(Motion),6:电子围栏(ElectronicDence),7:越界侦测(CrossBorderDetect),
							   8:离岗侦测(OffDutyDetect),9:哭声叫声(CryScreamAudio),10:客流进入(PassengerIn),11:客流离开(PassengerOut),12:智能跟踪(IntelligenceTrack),
							   13:人脸抓拍(FaceCapture),14:口罩检测(MaskDetection),15:车牌抓拍(CarPlateSnap),16:人脸识别(FaceReco),17:车流统计进入(TrafficStatisticsIn),
							   18:车流统计离开(TrafficStatisticsOut),19:关注车牌(AttentionLicensePlateAudioType),20:陌生车牌(StrangeLicensePlateAudioType),
							   21:非绿色车牌(NonGreenLicensePlateAudioType),22:黄色车牌(YellowLicensePlateAudioType),23:电动车检测(EBikeDetectAudioType),
							   24:煤气罐检测(GasContainerDetectAudioType),25:车牌布控无车牌(NoLicensePlateAudioType),26:人数统计超员报警试听音(PeopleStatisticsOverNumAudioType),
							   27:人数统计少员报警试听音(PeopleStatisticsLessNumAudioType) */

	int nFireAudioType;			//0：自定义，1：警报，2：犬吠，3：警戒区域，4：监控区域，5：欢迎光临，6：深水警告，7：私人领地，8：危险区域，9：贵重物品，10：高处危险，
										//11：私家价位，12：叮咚,		13:有口罩,		14:无口罩    15：注意私人物品     16：抓小偷     17：救命		18：分类		19：可回收垃圾		20：家庭食物垃圾		
										//21：危险废物		22：残余垃圾		23：全部分类	24：车形自定义		25：禁止停车      26-火焰自定义(用户可以在app上自定义语音播报的内容);?    27-谢谢;
										//28-您已离岗；     29-森林防火；       30-火焰报警(火警声?)
	int iAlarmInExAudioType;				//报警输入试听，0：自定义，1：警报，2：犬吠，3：警戒区域，4：监控区域，5：欢迎光临，6：深水警告，7：私人领地，8：危险区域，9：贵重物品，10：高处危险，
										//11：私家价位，12：叮咚,		13:有口罩,		14:无口罩    15：注意私人物品     16：抓小偷     17：救命		18：分类		19：可回收垃圾		20：家庭食物垃圾		
										//21：危险废物		22：残余垃圾		23：全部分类	
    		//0:人形	1：车形
	int iVideoShelterAudioType;			//视频遮挡试听，0：自定义，1：警报，2：犬吠，3：警戒区域，4：监控区域，5：欢迎光临，6：深水警告，7：私人领地，8：危险区域，9：贵重物品，10：高处危险，
										//11：私家价位，12：叮咚,		13:有口罩,		14:无口罩    15：注意私人物品     16：抓小偷     17：救命		18：分类		19：可回收垃圾		20：家庭食物垃圾		
										//21：危险废物		22：残余垃圾		23：全部分类	
    		//0:人形	1：车形
	int iMotionAudioType;			    //移动侦测试听，0：自定义，1：警报，2：犬吠，3：警戒区域，4：监控区域，5：欢迎光临，6：深水警告，7：私人领地，8：危险区域，9：贵重物品，10：高处危险，
										//11：私家价位，12：叮咚,		13:有口罩,		14:无口罩    15：注意私人物品     16：抓小偷     17：救命		18：分类		19：可回收垃圾		20：家庭食物垃圾		
										//21：危险废物		22：残余垃圾		23：全部分类	
    		//0:人形	1：车形
	int iElectronicDenceAudioType;		//电子围栏试听，0：自定义，1：警报，2：犬吠，3：警戒区域，4：监控区域，5：欢迎光临，6：深水警告，7：私人领地，8：危险区域，9：贵重物品，10：高处危险，
										//11：私家价位，12：叮咚,		13:有口罩,		14:无口罩    15：注意私人物品     16：抓小偷     17：救命		18：分类		19：可回收垃圾		20：家庭食物垃圾		
										//21：危险废物		22：残余垃圾		23：全部分类	
    		//0:人形	1：车形
	int iCrossBorderDetectAudioType;	//越界侦测试听，0：自定义，1：警报，2：犬吠，3：警戒区域，4：监控区域，5：欢迎光临，6：深水警告，7：私人领地，8：危险区域，9：贵重物品，10：高处危险，
										//11：私家价位，12：叮咚,		13:有口罩,		14:无口罩    15：注意私人物品     16：抓小偷     17：救命		18：分类		19：可回收垃圾		20：家庭食物垃圾		
										//21：危险废物		22：残余垃圾		23：全部分类	
    		//0:人形	1：车形
	int iOffDutyDetectAudioType;	    //离岗侦测试听，0：自定义，1：警报，2：犬吠，3：警戒区域，4：监控区域，5：欢迎光临，6：深水警告，7：私人领地，8：危险区域，9：贵重物品，10：高处危险，
										//11：私家价位，12：叮咚,		13:有口罩,		14:无口罩    15：注意私人物品     16：抓小偷     17：救命		18：分类		19：可回收垃圾		20：家庭食物垃圾		
										//21：危险废物		22：残余垃圾		23：全部分类	
	int iCryScreamAudioType;            /* 哭声叫声试听音，0：自定义，1：警报，2：犬吠，3：警戒区域，4：监控区域，5：欢迎光临，6：深水警告，7：私人领地，8：危险区域，9：贵重物品，10：高处危险，
                                        11：私家价位，12：叮咚,        13:有口罩,        14:无口罩    15：注意私人物品     16：抓小偷     17：救命        18：分类        19：可回收垃圾        20：家庭食物垃圾        
                                        21：危险废物        22：残余垃圾        23：全部分类 */

	int iPassengerInAudioType;          /* 客流进入试听音，0：自定义，1：警报，2：犬吠，3：警戒区域，4：监控区域，5：欢迎光临，6：深水警告，7：私人领地，8：危险区域，9：贵重物品，10：高处危险，
                                        11：私家价位，12：叮咚,        13:有口罩,        14:无口罩    15：注意私人物品     16：抓小偷     17：救命        18：分类        19：可回收垃圾        20：家庭食物垃圾        
                                        21：危险废物        22：残余垃圾        23：全部分类    */

	int iPassengerOutAudioType;         /* 客流离开试听音，0：自定义，1：警报，2：犬吠，3：警戒区域，4：监控区域，5：欢迎光临，6：深水警告，7：私人领地，8：危险区域，9：贵重物品，10：高处危险，
                                        11：私家价位，12：叮咚,        13:有口罩,        14:无口罩    15：注意私人物品     16：抓小偷     17：救命        18：分类        19：可回收垃圾        20：家庭食物垃圾        
                                        21：危险废物        22：残余垃圾        23：全部分类    */
	
	int iIntelligenceTrackAudioType;    /* 智能跟踪试听音，0：自定义，1：警报，2：犬吠，3：警戒区域，4：监控区域，5：欢迎光临，6：深水警告，7：私人领地，8：危险区域，9：贵重物品，10：高处危险，
                                        11：私家价位，12：叮咚,        13:有口罩,        14:无口罩    15：注意私人物品     16：抓小偷     17：救命        18：分类        19：可回收垃圾        20：家庭食物垃圾        
                                        21：危险废物        22：残余垃圾        23：全部分类    */

	int iAudioPlayTimes;                /* 语音播报默认次数,为0时表示自定义, 1/3/5/10/20 */
    int iAudioPlayCustomTimes;          /* 语音播报自定义次数,1-60 */

	int iFaceCaptureAudioType;    		/* 人脸抓拍试听音，0：自定义，1：警报，2：犬吠，3：警戒区域，4：监控区域，5：欢迎光临，6：深水警告，7：私人领地，8：危险区域，9：贵重物品，10：高处危险，
                                        11：私家价位，12：叮咚,        13:有口罩,        14:无口罩    15：注意私人物品     16：抓小偷     17：救命        18：分类        19：可回收垃圾        20：家庭食物垃圾        
                                        21：危险废物        22：残余垃圾        23：全部分类    */

	int iMaskDetectionAudioType;    	/* 口罩检测试听音，0：自定义，1：警报，2：犬吠，3：警戒区域，4：监控区域，5：欢迎光临，6：深水警告，7：私人领地，8：危险区域，9：贵重物品，10：高处危险，
                                        11：私家价位，12：叮咚,        13:有口罩,        14:无口罩    15：注意私人物品     16：抓小偷     17：救命        18：分类        19：可回收垃圾        20：家庭食物垃圾        
                                        21：危险废物        22：残余垃圾        23：全部分类    */

    int iCarPlateSnapAudioType;    	    /* 车牌抓拍试听音，0：自定义，1：警报，2：犬吠，3：警戒区域，4：监控区域，5：欢迎光临，6：深水警告，7：私人领地，8：危险区域，9：贵重物品，10：高处危险，
                                        11：私家价位，12：叮咚,        13:有口罩,        14:无口罩    15：注意私人物品     16：抓小偷     17：救命        18：分类        19：可回收垃圾        20：家庭食物垃圾        
                                        21：危险废物        22：残余垃圾        23：全部分类    */

    int iFaceRecoAudioType;    	        /* 人脸识别试听音，0：自定义，1：警报，2：犬吠，3：警戒区域，4：监控区域，5：欢迎光临，6：深水警告，7：私人领地，8：危险区域，9：贵重物品，10：高处危险，
                                        11：私家价位，12：叮咚,        13:有口罩,        14:无口罩    15：注意私人物品     16：抓小偷     17：救命        18：分类        19：可回收垃圾        20：家庭食物垃圾        
                                        21：危险废物        22：残余垃圾        23：全部分类    */
	
	int iTrafficStatisticsInAudioType;  /* 车流统计进入试听音，0：自定义，1：警报，2：犬吠，3：警戒区域，4：监控区域，5：欢迎光临，6：深水警告，7：私人领地，8：危险区域，9：贵重物品，10：高处危险，
                                        11：私家价位，12：叮咚,        13:有口罩,        14:无口罩    15：注意私人物品     16：抓小偷     17：救命        18：分类        19：可回收垃圾        20：家庭食物垃圾       
                                        21：危险废物        22：残余垃圾        23：全部分类    */
    
	int iTrafficStatisticsOutAudioType; /* 车流统计离开试听音，0：自定义，1：警报，2：犬吠，3：警戒区域，4：监控区域，5：欢迎光临，6：深水警告，7：私人领地，8：危险区域，9：贵重物品，10：高处危险，
                                        11：私家价位，12：叮咚,        13:有口罩,        14:无口罩    15：注意私人物品     16：抓小偷     17：救命        18：分类        19：可回收垃圾        20：家庭食物垃圾       
                                        21：危险废物        22：残余垃圾        23：全部分类    */
	int iAttentionLicensePlateAudioType; /* 关注车牌试听音，0：自定义，1：警报，2：犬吠，3：警戒区域，4：监控区域，5：欢迎光临，6：深水警告，7：私人领地，8：危险区域，9：贵重物品，10：高处危险，
                                        11：私家价位，12：叮咚,        13:有口罩,        14:无口罩    15：注意私人物品     16：抓小偷     17：救命        18：分类        19：可回收垃圾        20：家庭食物垃圾       
                                        21：危险废物        22：残余垃圾        23：全部分类    */
	int iStrangeLicensePlateAudioType; /* 陌生车牌试听音，0：自定义，1：警报，2：犬吠，3：警戒区域，4：监控区域，5：欢迎光临，6：深水警告，7：私人领地，8：危险区域，9：贵重物品，10：高处危险，
                                        11：私家价位，12：叮咚,        13:有口罩,        14:无口罩    15：注意私人物品     16：抓小偷     17：救命        18：分类        19：可回收垃圾        20：家庭食物垃圾       
                                        21：危险废物        22：残余垃圾        23：全部分类    */
	int iNonGreenLicensePlateAudioType; /* 非绿色车牌试听音，0：自定义，1：警报，2：犬吠，3：警戒区域，4：监控区域，5：欢迎光临，6：深水警告，7：私人领地，8：危险区域，9：贵重物品，10：高处危险，
                                        11：私家价位，12：叮咚,        13:有口罩,        14:无口罩    15：注意私人物品     16：抓小偷     17：救命        18：分类        19：可回收垃圾        20：家庭食物垃圾       
                                        21：危险废物        22：残余垃圾        23：全部分类    */
	int iYellowLicensePlateAudioType; /* 黄色车牌试听音，0：自定义，1：警报，2：犬吠，3：警戒区域，4：监控区域，5：欢迎光临，6：深水警告，7：私人领地，8：危险区域，9：贵重物品，10：高处危险，
                                        11：私家价位，12：叮咚,        13:有口罩,        14:无口罩    15：注意私人物品     16：抓小偷     17：救命        18：分类        19：可回收垃圾        20：家庭食物垃圾       
                                        21：危险废物        22：残余垃圾        23：全部分类    */
	int iEBikeDetectAudioType; 		/* 电动车检测试听音，0：自定义，1：警报，2：犬吠，3：警戒区域，4：监控区域，5：欢迎光临，6：深水警告，7：私人领地，8：危险区域，9：贵重物品，10：高处危险，
                                        11：私家价位，12：叮咚,        13:有口罩,        14:无口罩    15：注意私人物品     16：抓小偷     17：救命        18：分类        19：可回收垃圾        20：家庭食物垃圾       
                                        21：危险废物        22：残余垃圾        23：全部分类    */
	int iGasContainerDetectAudioType; /* 煤气罐检测试听音，0：自定义，1：警报，2：犬吠，3：警戒区域，4：监控区域，5：欢迎光临，6：深水警告，7：私人领地，8：危险区域，9：贵重物品，10：高处危险，
                                        11：私家价位，12：叮咚,        13:有口罩,        14:无口罩    15：注意私人物品     16：抓小偷     17：救命        18：分类        19：可回收垃圾        20：家庭食物垃圾       
                                        21：危险废物        22：残余垃圾        23：全部分类    */
	int iNoLicensePlateAudioType; 		/* 车牌布控无车牌试听音，0：自定义，1：警报，2：犬吠，3：警戒区域，4：监控区域，5：欢迎光临，6：深水警告，7：私人领地，8：危险区域，9：贵重物品，10：高处危险，
                                        11：私家价位，12：叮咚,        13:有口罩,        14:无口罩    15：注意私人物品     16：抓小偷     17：救命        18：分类        19：可回收垃圾        20：家庭食物垃圾       
                                        21：危险废物        22：残余垃圾        23：全部分类    */
	int iPeopleStatisticsOverNumAudioType;     /* 人数统计超员报警试听音，0：自定义，1：警报，2：犬吠，3：警戒区域，4：监控区域，5：欢迎光临，6：深水警告，7：私人领地，8：危险区域，9：贵重物品，10：高处危险，
                                        11：私家价位，12：叮咚,        13:有口罩,        14:无口罩    15：注意私人物品     16：抓小偷     17：救命        18：分类        19：可回收垃圾        20：家庭食物垃圾      
                                        21：危险废物        22：残余垃圾        23：全部分类    */
	int iPeopleStatisticsLessNumAudioType;     /* 人数统计少员报警试听音，0：自定义，1：警报，2：犬吠，3：警戒区域，4：监控区域，5：欢迎光临，6：深水警告，7：私人领地，8：危险区域，9：贵重物品，10：高处危险，
                                        11：私家价位，12：叮咚,        13:有口罩,        14:无口罩    15：注意私人物品     16：抓小偷     17：救命        18：分类        19：可回收垃圾        20：家庭食物垃圾      
                                        21：危险废物        22：残余垃圾        23：全部分类    */
	char szReserved[144];
}HY_AUDIO_ALARM_CONFIG_V1, *LPHY_AUDIO_ALARM_CONFIG_V1;

typedef struct tagHY_DVR_AUDIO_ALARM_CONFIG_V1_ALARM_SCHEDULE
{
    int iValid;
 
    HY_DVR_SCHEDTIME struSchedTime;
 
    char acReserved[512];
}HY_DVR_AUDIO_ALARM_CONFIG_V1_ALARM_SCHEDULE, *LPHY_DVR_AUDIO_ALARM_CONFIG_V1_ALARM_SCHEDULE;

typedef struct tagHY_DVR_AUDIO_ALARM_ELIMINATE
{
	int iValid;

    int iChannelID;
	int iEnable;          /* 是否使能声音报警消除配置 */  
	int iAlarmEliminateAudioType;   /* 报警消除的音频类型(取值与HY_AUDIO_ALARM_CONFIG_V1的nTryListenType保持一致)；0:人形 1：车形,2:火焰,3:报警输入(AlarmIn),
								4:视频遮挡(VideoShelter),5:移动侦测(Motion),6:电子围栏(ElectronicDence),7:越界侦测(CrossBorderDetect),8:离岗侦测(OffDutyDetect),
                               9:哭声叫声(CryScreamAudio),10:客流进入(PassengerIn),11:客流离开(PassengerOut),12:智能跟踪(IntelligenceTrack),
                               13:人脸抓拍(FaceCapture),14:口罩检测(MaskDetection),15:车牌抓拍(CarPlateSnap),16:人脸识别(FaceReco),17:车流统计进入(TrafficStatisticsIn),
                               18:车流统计离开(TrafficStatisticsOut),19:关注车牌(AttentionLicensePlateAudioType),20:陌生车牌(StrangeLicensePlateAudioType),
                               21:非绿色车牌(NonGreenLicensePlateAudioType),22:黄色车牌(YellowLicensePlateAudioType),23:电动车检测(EBikeDetectAudioType)
							   24:煤气罐检测(GasContainerDetectAudioType) */
    int iEBikeDetectEliminateDuration;			/* 电动车检测音频报警消除时长，单位：秒，最大时长600s */
    int iGasContainerDetectEliminateDuration;	/* 煤气罐检测音频报警消除时长，单位：秒，最大时长600s */
	
	char acReserved[512];
}HY_DVR_AUDIO_ALARM_ELIMINATE, *LPHY_DVR_AUDIO_ALARM_ELIMINATE;

typedef struct tagHY_DVR_GATE_ACCESS_DIRECTION
{
	int nValid;
	int nDirection;				// 进出方向。0-无效；1-出入；2-只入；3-只出
}HY_DVR_GATE_ACCESS_DIRECTION, *LPHY_DVR_GATE_ACCESS_DIRECTION;

typedef struct tagHY_DVR_JG_PLATFORM_CFG
{
	int nValid;
	int nEnable;				// 使能状态
	char szServerPrefix[64];	// 服务器前缀
	char szAppId[64];			// AppId
	char szSecret[64];			// Secret
	char szReserved[32];
}HY_DVR_JG_PLATFORM_CFG, *LPHY_DVR_JG_PLATFORM_CFG;

typedef struct tagHY_DVR_NFC_CARD_INFO
{
	int nValid;
	char szCardId[64];		// 卡ID号
	char szReserved[32];
}HY_DVR_NFC_CARD_INFO, *LPHY_DVR_NFC_CARD_INFO;

typedef struct tagHY_DVR_JG_PLATFORM_CONNECT_STATUTS
{
	int nValid;
	int nServerStatus;		// 服务器状态，0-Offline 1-Online
	char szReserved[32];
}HY_DVR_JG_PLATFORM_CONNECT_STATUTS, *LPHY_DVR_JG_PLATFORM_CONNECT_STATUTS;

typedef enum eAddress
{
	eAddrBegin,
	eAddrBeiJing,						//"北京市"
	eAddrTianJing,						//"天津市"
	eAddrHeBei,						//"河北省"
	eAddrShanXi,						//"山西省"
	eAddrNeiMengGu,				//"内蒙古"
	eAddrLiaoNing,					//"辽宁省"
	eAddrJiLin,							//"吉林省"
	eAddrHeiLongJiang,			//"黑龙江省"
	eAddrShangHai,					//"上海市"
	eAddrJiangSu,					//"江苏省"
	eAddrZheJiang,					//"浙江省"
	eAddrAnHui,						//"安徽省"
	eAddrFuJian,						//"福建省"
	eAddrJiangXi,						//"江西省"
	eAddrShanDong,				//"山东省"
	eAddrHeNan,						//"河南省"
	eAddrHuBei,						//"湖北省"
	eAddrHuNan,						//"湖南省"
	eAddrGuangDong,				//"广东省"
	eAddrGuangXi,					//"广西自治区"
	eAddrHaiNan,						//"海南省"
	eAddrChongQing,				//"重庆市"
	eAddrSiChuan,					//"四川省"
	eAddrGuiZhou,					//"贵州省"
	eAddrYunNan,					//"云南省"
	eAddrXiZang,						//"西藏自治区"
	eAddrShan3Xi,					//"陕西省"
	eAddrGanSu,						//"甘肃省"
	eAddrQingHai,					//"青海省"
	eAddrNingXia,					//"宁夏回族自治区"
	eAddrXinJiang,					//"新疆维吾尔自治区"
	eAddrXiangGang,				//"香港特别行政区"
	eAddrAoMen,						//"澳门特别行政区"
	eAddrTaiWan,						//"台湾省"
	eAddrDiaoYuDao,				//"钓鱼岛"
	eAddrOtherIsland,				//"其他附属岛屿"
}eAddress;

typedef struct tagHY_DVR_HEALTH_CONFIG
{
	int nValid;

	int nChannelID;
	int nEnable;
	eAddress eArea;
	int nIDCardRecoEnable;
	int nHealthCodeRecoEnable;
	char cJKMType;										/*!< 健康码类型：0-国康码,1-地方健康码 */
    char cJKMAbnormalForbid;							/*!< 健康码异常禁入：0-禁用,1-启用 */
    char cNuclainAudioBroadcast;						/*!< 核酸语音播报：0-不播报,1-普通播报,2-超期播报	*/
    char cNuclainExtendBan;								/*!< 核酸超期禁入：0-禁用,1-启用 */
    int	 iNuclainExtendTime;							/*!< 核酸超期时间：0-12小时,1-24小时,2-48小时,3-72小时,7-7天,14-14天,30-30天,31-自定义 */
    char cPersonCertiCompSwitch;        				/*!< 人证比对开关：0-禁用,1-启用 */
    char cHealthCodeAbnormalBroadcast;  				/*!< 健康码异常播报：0-不播报,1-播报 */
    char cNucleicAbnormalDebar;         				/*!< 核酸异常禁入：0-禁用,1-启用 */
    char cNucleicAbnormalBroadcast;     				/*!< 核酸异常播报：0-不播报,1-播报 */
    char cAntigenAbnormalDebar;         				/*!< 抗原异常禁入：0-禁用,1-启用 */
    char cAntigenAbnormalBroadcast;     				/*!< 抗原异常播报：0-不播报,1-播报 */
    char cAntigenExtendDebar;           				/*!< 抗原超期禁入：0-禁用,1-启用 */
    char cAntigenExtendBroadcast;       				/*!< 抗原超期播报：0-禁用,1-启用 */
	int iAntigenEffectiveTime;							/*!< 抗原有效时间：0-12小时,1-24小时,2-48小时,3-72小时,7-7天,14-14天,30-30天,31-自定义 */
    char cNucCustomExtendDay;           				/*!< 核酸超期时间自定义天数,范围1-30, iNuclainExtendTime 为31时有效*/
    char cAntCustomEffectiveDay;        				/*!< 抗原有效时间自定义天数,范围1-30, iAntigenEffectiveTime 为31时有效*/
	int iPersonCertiCompThreshold;      				/*!< 人证比对阈值设置，30 ~ 100，默认40 */

    char szReserved[228];
}HY_DVR_HEALTH_CONFIG, *LPHY_DVR_HEALTH_CONFIG;

typedef struct tagHY_DVR_SIMULATE_DIGITAL_INPUT
{
	int nValid;

	int nChannelID;
	int nAlarmInID;
	int nState;

	char szReserved[32];
}HY_DVR_SIMULATE_DIGITAL_INPUT, *LPHY_DVR_SIMULATE_DIGITAL_INPUT;

typedef struct tagHY_DVR_OVD_CONFIG_NETWORK_INFO
{
	int nValid;

	char szQRInfo[128];		//二维码扫描出来的字符串，中间层传入协议
	char szSSID[DVR_MAX_SSID_LEN];		//协议将解析到的SSID回填到结构体中
	char szPassword[DVR_MAX_WIFI_KEY_LEN];			//协议将解析到的密码回填到结构体中

	char szReserved[256];
}HY_DVR_OVD_CONFIG_NETWORK_INFO, *LPHY_DVR_OVD_CONFIG_NETWORK_INFO;

typedef struct tagHY_DVR_OVD_CONFIG_ENCRYPT_MODE
{
	int nValid;

	int nEncryptMode;		//0：半加密（I帧都加密，P帧半加密）		1：全加密		2：仅加密I帧（效果不好，移动不建议）

	char szReserved[32];
}HY_DVR_OVD_CONFIG_ENCRYPT_MODE, *LPHY_DVR_OVD_CONFIG_ENCRYPT_MODE;

typedef struct tagHY_DVR_OVD_CONFIG_CALL_OVD_ENCRYPT_INTERFACE
{
	int nValid;

	int nChannelID;				//通道号，IPC为０
	int nIsIFrame;			//是否是I帧
	char *pOriginFrameData;		//原始帧数据
	int nOriginFrameLen;		//原始帧数据长度
	char *pEncBuffer;		//加密缓冲区(大小为原始帧数据长度＋128)
	int nEncBufferLen;		//加密缓冲区长度
	int nEncFrameLen;		//加密后的帧实际数据长度
	int nCodecID;		//帧编码类型，0:H264，1:H265

	char szReserved[64];
}HY_DVR_OVD_CONFIG_CALL_OVD_ENCRYPT_INTERFACE, LPHY_DVR_OVD_CONFIG_CALL_OVD_ENCRYPT_INTERFACE;

typedef struct tagHY_DVR_OVD_CONFIG_AV_DEBUG_MODE
{
	int nValid;

	int nEnable;
	char szFilePath[128];		//eg: /tmp
	char szFileName[128];		//最终生成的视频文件：{szFilePath}/{szFileName}_live.video，音频文件：{szFilePath}/{szFileName}_liveaudio.adts

	char szReserved[32];
}HY_DVR_OVD_CONFIG_AV_DEBUG_MODE, *LPHY_DVR_OVD_CONFIG_AV_DEBUG_MODE;

typedef struct tagHY_DVR_CMCC_OVD_SERVER_CONNECT_STATUS
{
	int nValid;

	int nStatus;	//0:连接成功		- 1 : 连接失败1		 : 重连中

	char szReserved[32];
}HY_DVR_CMCC_OVD_SERVER_CONNECT_STATUS, *LPHY_DVR_CMCC_OVD_SERVER_CONNECT_STATUS;

typedef struct tagHY_DVR_CMCC_OVD_KEY_TO_CALL_STATUS
{
	int nValid;

	int nStatus;	//-1:异常      0:发起通话        1:正在发起通话         2:请求接听       3: 主动挂断        4：被动挂断
}HY_DVR_CMCC_OVD_KEY_TO_CALL_STATUS, *LPHY_DVR_CMCC_OVD_KEY_TO_CALL_STATUS;

typedef struct tagHY_DVR_CMCC_OVD_DM_BIND_STATUS
{
	int nValid;

	int nStatus;	// 设备的绑定状态,1,绑定;非1,为非绑定;

	char szReserved[32];
} HY_DVR_CMCC_OVD_DM_BIND_STATUS, *LPHY_DVR_CMCC_OVD_DM_BIND_STATUS;

typedef struct tagHY_DVR_CMCC_OVD_CAP
{
	int nValid;

	int have_ptz;			//<布尔型：是否支持云台控制能力,默认值为false>
	int support_ptz_preset; //<布尔型：是否支持云台预置位,默认值为false>
	int have_battery;		//<布尔型：是否电池供电,主要用于门铃设备,默认值为false>,

	int have_audio_out; //<布尔型：是否支持音乐输出,默认值为false>
	int have_voice_out; //<布尔型：是否支持对讲输出,默认值为false>
	int have_ivrs;		//<布尔型：是否支持ivrs协议，默认值为false，表示云存只支持ivr协议>,
	int have_trace;		//<布尔型：是否支持移动跟踪,默认值为false>

	// OVDAIFaceCapInfo
	int aiface_face;				  // 布尔型，设备是否支持上报人脸AI,默认值为false
	int aiface_faceExposureAdjust;	  // 布尔型，设备是否支持人脸曝光度调节,默认值为false
	char aiface_CaptureModeList[256]; // 字符串,抓拍模式调节列表选项，若没有，则设备不支持模式切换,默认“quality”:代表质量抓拍，“quick”:代表快速抓拍,用逗号相隔
	int aiface_alarm_zone;

	// OVDAIKitchenCapInfo
	int aikitchen_kitchen_detection;
	char aikitchen_CaptureModeList[256];
	char aikitchen_ClothesColorList[256];
	int aikitchen_alarm_zone;

	// OVDAIVehicleCapInfo
	int aivehicle_vehicle_detection;
	char aivehicle_CaptureModeList[256];
	int aivehicle_alarm_zone;

	// OVDAlarmsInfo
	int have_alarms_io;				   //<布尔型：是否支持外包报警,默认值为false>,
	int have_alarms_face;			   //<布尔型：是否支持人脸侦测,默认值为false>,
	int support_alarms_face_zone;	   //<整型:0不支持，1：只支持矩形，n(n>=3):支持不规则n边型>, 注：目前app限制，只需要设备支持矩形即可
	int have_alarms_cry;			   //<布尔型：是否支持哭声侦测,默认值为false>,
	int have_alarms_voice;			   //<布尔型：是否支持声音侦测,默认值为false>,
	int have_alarms_motion;			   //<布尔型：是否支持移动侦测,默认值为false>,
	int support_alarms_motion_zone;	   //<整型:0不支持，1：只支持矩形，n(n>=3):支持不规则n边型>, 注：目前app限制，只需要设备支持矩形即可
	int have_alarms_body;			   //<布尔型：是否支持人形侦测,默认值为false>,
	int support_alarms_body_zone;	   //<整型:0不支持，1：只支持矩形，n(n>=3):支持不规则n边型>, 注：目前app限制，只需要设备支持矩形即可
	int have_alarms_cross;			   //<布尔型：是否支持拌网侦测>,
	int have_alarms_pir;			   //<<布尔型，是否支持pir侦测>,
	int have_alarms_pir_staymode;	   //<<布尔型，是否支持pir逗留模式,门铃>
	int have_alarms_lossLock;		   //<<布尔型，是否支持撬锁侦测>
	int have_alarms_alertarea;		   //<<布尔型，是否支持警戒功能>
	int have_alertarea_expel;		   //<布尔型，是否支持智能驱离功能（针对带云台的警戒设备
	int support_alarms_alertarea_zone; //<整型:0不支持，1：只支持矩形，n(n>=3):支持不规则n边型>, 注：目前app限制，只需要设备支持矩形即可

	// OVDlinkage_mode
	char alertarea[128];
    char linkage_mode_mask[128];   //字符串，口罩检测告警所支持的联动策略，目前只支持”speech“（语音输出)，默认值为空字符串
	char vehicle_detection[64];
    char linkage_regional_people[64];	// 字符串, 人数统计所支持的联动策略列表，选项之间以逗号相隔，可选项有"speech"（语音输出), "light"(联动警灯) ，默认值为空字符串
	char linkage_transgression[128];//字符串，越界侦测告警所支持的联动策略列表，选项之间以逗号相隔，可选项有"speech"（语音输出), "light"(联动警灯) , "buz"(蜂鸣器），默认值为空字符串

	int log_upload_https;		 //<布尔型：日志文件上传是否支持HTTPS，若不支持，则OVC下发的日志上传指令中的http URL为http协议，默认值为false>,
	int firmware_download_https; //<布尔型：固件下载是否支持HTTPS，若不支持，则OVC下发的升级指令中的http URL为http协议，默认值为false>,
	int video_encrypt;			 // 是否支持视频加密，默认值为false, 视频加密分两个值与，一个是厂商是否支持,主要考虑厂商的性能等因素是否支撑的了视频加密，第2个是设备sdk是否支持视频加密实现
	int have_switch;			 // 是否支持通道使能开关，默认值为false>,
	int have_sd;				 // 是否支持SD卡/TF卡，默认值为true>,
	int have_led;				 // 是否支持led灯开关，默认值为false>,
	int have_support_softprobe;	 // 是否支软探针，默认值为false>,
	int have_auto_reboot;
	// 支持的视频清晰度选项列表，列表元素可选值为：ld/sd/hd/fhd，分别代表低清/标清/高清/全高清，默认支持sd和hd>
	char video_quality[256];
	//<字符串列表(中间用,隔开)：支持的视频编码格式列表，列表元素可选值为：h264/h265,默认支持h264>
	char video_formats_supportlists[256];

	//<布尔型，是否支持平台切换视频h264,h265格式>
	int support_set_video_formats;

	/*
        OVD_MEDIA_NO_ENCRYPT  该设备不支持码流加密功能
        OVD_MEDIA_BASE_ENCRYPT  只支持基础加密方案，适用于所有设备
        OVD_MEDIA_ADVANCED_ENCRYPT 用于设备性能富裕的设备，说明设备能够支持aes_cb_256加密及基础加密方案
    */
	int stream_encryption_mode;

	int have_voiceout_volume; //<布尔型： 设备是否支持对讲音量调节，默认值为false>
	int have_doorbell_volume;
	int support_set_normal_nightvision_mode; //<布尔型: 普通摄像头是否支持设备切换夜视模式，夜视模式分为开启/关闭/自动>
	int support_set_color_nightvision_mode;	 //<布尔型：全彩摄像头是否支持设备切换夜视模式，夜视模式分为黑白夜视/全彩夜视/智能夜视>

	// osd相关
	int osd_text;	 // 布尔型，是否支持持文本水印
	int osd_logo;	 // 布尔型，是否支持水印图片
	int enable_hjkh; // 布尔型，设备是否集成了和家固话，默认值为false

	// OVDHJKH_mode
	int preference_call;	// 布尔型，是否支持呼叫偏好功能；呼叫偏好：可设置在发起呼叫时，普通电话优先或者APP通话优先
	int set_calling_number; // 布尔型，是否支持呼叫号码设置的功能；呼叫号码：可设置点击呼叫按键后，摄像头呼叫的号码（被叫号码）
	int not_disturb_mode;	// 布尔型，是否支持电话勿扰模式；电话勿扰：可设置设备勿扰开关是否打开，如果打开，可设置勿扰时间
	int auto_hangup;		// 布尔型，是否支持自动挂断功能；自动挂断：可设置检测到访客离开/进门后自行停止呼叫

	// nonmotorvehicle
	int nonmotorvehicle_detection;				// 布尔型，设备是否支持电瓶车侦测
	char nonmotorvehicle_detect_mode_list[128]; // 枚举字符串列表，检测类型选项列表，选项之间用逗号相隔，目前端侧类型选项有"electromobile"（电瓶车）,"bike"（自行车）
	int nonmotorvehicle_alarm_zone;				// 整型，0不支持，1只支持矩形，n(n>=3)，支持不规则n边形>
	int not_disturb_device_mode;				// 布尔型，是否支持设备勿扰模式，设备勿扰是控制手机端主叫设备端
	int mask_detection;							// 布尔型，口罩检测,默认值为false
	int vehicle_detect_site;					// 布尔型，是否支持车辆场景设置

	// zoom, focus
	int zoomcontrol;
	int focuscontrol;

	int parabolicAerial; // 设备是否支持上报高空抛物，默认值为false
	int detectZoneNum;	 // 检测区域最大个数
	int detectZone;		 // 检测区域绘制多边形最大边数，0不支持，1：只支持矩形，n(n>=3):支持不规则n边形
	int shieldZoneNum;	 // 屏蔽区域最大个数
	int shieldZone;		 // 屏蔽区域绘制多边形最大边数，0不支持，1：只支持矩形，n(n>=3):支持不规则n边形
	int floorConfNum;	 // 楼层设置最大个数

	// 客流统计
	int passenger;						  // 布尔型，设备是否支持上报客流统计,默认值为false
	int passenger_exposure_adjust;		  // 布尔型，设备是否支持人脸曝光度调节,默认值为false
	char passenger_capture_modelist[128]; // 字符串,抓拍模式调节列表选项，若没有，则设备不支持模式切换,默认“quality”:代表质量抓拍，“quick”:代表快速抓拍,用逗号相隔
	int passenger_alarmzone;			  // 整型，0 不支持，1：只支持矩形，n(n>=3):支持不规则 n 边形

	// 区域人数统计
	int regional_people_stat;			// 布尔型，设备是否支持上报区域人数统计,默认值为false
	int regional_people_stat_alarmzone; // 整型，0 不支持，1：只支持矩形，n(n>=3):支持不规则 n 边形

	int stopalarm; //<布尔型，是否支持消控能力，默认值为false>
	
	// 区域人数统计
	int support_alert_time;               //是否支持设置检测时间， 0表示不支持，1表示支持，默认不支持
	int support_osd_status;               //布尔型，是否支持设置OSD叠加人数， 0表示不支持，1表示支持，默认不支持
	int support_regional_people_count;    //整型，个数，0表示不支持，其他值表示对应配置项“触发预警的人数”的最大值限制，默认不支持
	int support_alarm_report_duration;    //整型，分钟，0表示不支持，其他值表示对应配置项“告警上报间隔时间”的最大值限制，默认不支持
	int support_detect_result_report_duration;//整型，分钟，0表示不支持，其他值表示对应配置项“区域人数定时上传”的最大值限制，默认不支持
	int detect_plans_num;                 //整型，检测计划的个数，0不支持，最大值10个，默认不支持
	int alarm_area_num;                   //整型，检测区域的个数，0不支持，最大值10个，默认不支持

	//区域入侵参数
	int alertarea_set_target;    //<整型，位运算0x0F。设备是否支持设置检测目标。最低位表示"人形" 0x01，第二位表示"机动车" 0x02，第三位表示"非机动车" 0x04，第四位表示"其他" 0x08，默认0表示不支持>
	int alertarea_support_staymode_time;     //<布尔型，是否支持区域入侵后设置停留时长。0表示不支持，1表示支持，默认不支持>
	int alertarea_support_set_alerttime;     //<布尔型，设备是否支持设置检测时间段，0表示不支持，1表示支持，默认不支持>
	int alertarea_support_alarms_zone;		//<整型:0不支持，1：只支持矩形，n(n>=3):支持不规则n边型>, 注：目前app限制，只需要设备支持矩形即可
	
	//越界侦测功能
	int have_alarms_transgression;    //布尔型：是否支持越界侦测功能，默认值为false>
	int transgression_set_target;    //<整型，位运算0x0F。设备是否支持设置检测目标。最低位表示"人形" 0x01，第二位表示"机动车" 0x02，第三位表示"非机动车" 0x04，第四位表示"其他" 0x08，默认0表示不支持>
	int transgression_support_set_alerttime;     //<布尔型，设备是否支持设置检测时间段，0表示不支持，1表示支持，默认不支持>
	int transgression_set_statistics_line; //<布尔型，设备是否支持设置越界规则线，0表示不支持，1表示支持，默认不支持>

	// 客流统计
	int passenger_support_osd_status;  //布尔型，设备是否支持osd显示客流统计信息
	
	//离岗检测
	int offduty_detection;     //布尔型，设备是否支持离岗检测,默认值为false
	int support_on_duty_count;  //<整型，0表示不支持，其他值表示对应配置项“在岗人数”的最大值限制，默认值为0>
	int offduty_support_durtion;   //<整型，分钟，0表示不支持，其他值表示对应配置项“设置离岗时长”的最大值限制，默认值为0>
	int offduty_support_alert_time;   //<布尔型，是否支持设置检测时间，默认值为false>
	int offduty_detect_plans_num;  //<整型，检测计划的个数，0不支持，最大值5个，默认值为0>
	int offduty_alarm_area_num;    //<整型，检测区域的个数，0不支持，最大值10个，默认值为0>
	int offduty_alarm_zone;   //<整型，0不支持，1：只支持矩形，n(n>=3):支持不规则n边形，目前只支持4边形。最大支持10边形，默认值为0>

	char szReserved[592];
} HY_DVR_CMCC_OVD_CAP, *LPHY_DVR_CMCC_OVD_CAP;

typedef enum eVoiceType
{
	VoiceType_None = 0,
	VoiceType_VoipCall = 1,//对讲
	VoiceType_VoipBroadCast = 2, //云广播
}eVoiceType;

typedef struct tagHY_DVR_VOIP_VOICE_TYPE
{
	int nValid;

	int nChannelID;
	eVoiceType eVoipVoiceType; //1:对讲 2：云广播...

	char szReserved[256];
}HY_DVR_VOIP_VOICE_TYPE, *LPHY_DVR_VOIP_VOICE_TYPE;

typedef struct tagHY_DVR_OVDSDK_STOP_ALARM
{
	int nValid;
	int eAlarmType; // 报警类型,参考hieserver_common.h中的eAlertType类型
	char szReserved[128];
} HY_DVR_OVDSDK_STOP_ALARM;

typedef struct tagHY_DVR_CUDOT_FACERECO_STATUS
{
	int bValid;
	int bEnable;					// 使能开关，0关闭，1使能
}HY_DVR_CUDOT_FACERECO_STATUS, *LPHY_DVR_CUDOT_FACERECO_STATUS;

typedef struct tagHY_DVR_CUDOT_FACERECO_PARAMS
{
	int bValid;
	int nSensitivity;				// 人脸检测灵敏度，默认3，范围1~5
	int nMinWidth;					// 人脸检测最小像素宽，默认为-1，表示无限制
	int nMinHeight;					// 人脸检测最小像素高，默认为-1，表示无限制
	int nMaxWidth;					// 人脸检测最大像素宽，默认为-1，表示无限制
	int nMaxHeight;					// 人脸检测最大像素高，默认为-1，表示无限制
}HY_DVR_CUDOT_FACERECO_PARAMS, *LPHY_DVR_CUDOT_FACERECO_PARAMS;

#define DVR_MAX_CUDOT_FACERECO_FACE_NUM				(10)			// 最多支持下发的人脸个数

typedef struct tagHY_DVR_CUDOT_FACE_INFO
{
	char *faceid;		///<人脸id，唯一	
	char* personid;           ///<人员id，唯一
	char* extraInfo;          ///<人脸额外信息
	char* md5sum;             ///<人脸图片MD5
	void *picbuf;			///<人脸图片数据
	int piclen;				///<人脸图片长度
}HY_DVR_CUDOT_FACE_INFO;

typedef struct tagHY_DVR_CUDOT_FACERECO_INFO
{
	int bValid;
	int nAction;			// 下发的数据需执行的动作，0 表示删除，1 表示增加，2 表示更新
	int nCount;				// 实际下发人脸个数
	HY_DVR_CUDOT_FACE_INFO struFaces[DVR_MAX_CUDOT_FACERECO_FACE_NUM];
}HY_DVR_CUDOT_FACERECO_INFO, *LPHY_DVR_CUDOT_FACERECO_INFO;

//人脸特征
typedef struct tagHY_DVR_CUDOT_FACE_ATTR_INFO
{
	int nValid;
	unsigned char		cAge;							/*!< 年龄：0-未知，其它表示年龄 			*/
	unsigned char		cSex;							/*!< 性别：0-未知，1-男性，2-女性			*/
	unsigned char		cSkin;							/*!< 肤色：0-未知，1-黄色，2-白色，3-黑色，4-棕色*/
	unsigned char		cBeard; 						/*!< 胡须：0-未知，1-有胡须，2-无胡须		*/
	unsigned char		cGlasses;						/*!< 眼镜：0-未知，1-戴眼镜，2-无眼镜		*/
	unsigned char		cMask;							/*!< 口罩：0-未知，1-有口罩，2-无口罩		*/
	char szReserved[64];
}HY_DVR_CUDOT_FACE_ATTR_INFO;

//识别记录
typedef struct tagHY_DVR_CUDOT_PIC_SNAP_INFO
{
	int nValid;
	char  nPicId[128];    //扣图所匹配的目标数据库的ID
	int nPicLen;	//扣图大小
	int uiSimilarity;     //相似度
	char* ptrPicBuff; //图片buff
	int nX;
	int nY;
	int nWidth;
	int nHeight;
	HY_DVR_CUDOT_FACE_ATTR_INFO struFaceAttrInFo;
	char nPersonId[128];	//人员ID
	int nPersonType;		//人员类型：0.注册人脸的人员 1.陌生人(personType为1时：faceid，personid由设备生成)
	char szReserved[512 - 6 - 4 - 64 - 128 - 4];
}HY_DVR_CUDOT_PIC_SNAP_INFO;

//识别结果
typedef struct tagHY_DVR_CUDOT_FACE_RECOGNIZE
{
	int nValid;
	int nBgJpgLen;    //背景图片长度
	char* pBgJpgBuff; //背景图片地址
	int nPicNum;	//扣图个数
	HY_DVR_CUDOT_PIC_SNAP_INFO struPicSnapInFo;
	unsigned long long lluTimeStamp;		     //背景图时间  时间毫秒
	char szReserved[512 - 8];
}HY_DVR_CUDOT_FACE_RECOGNIZE;

/*!
*  @enum    eAlgorithmStoreType
*  @brief      算法仓库算法类型
*/
typedef enum
{
    eAlgorithmType_Unknown,                    //未定义模式
    eAlgorithmType_IntelligentAlert,           //智能警戒(IntelligentAlert)
    eAlgorithmType_FaceCapture,                //人脸抓拍(FaceCapture)
    eAlgorithmType_FaceRecognition,            //人脸识别(FaceRecognition)
}eAlgorithmStoreType;

/*!
*  @struct    tagHY_DVR_ALGORITHM_STORE_CFG
*  @brief      算法仓库配置
*/
typedef struct tagHY_DVR_ALGORITHM_STORE_CFG
{
    int iValid;

    eAlgorithmStoreType eAlgorithmType;      /* 算法类型：见eAlgorithmStoreType */

    char szReserved[128];
}HY_DVR_ALGORITHM_STORE_CFG, *LPHY_DVR_ALGORITHM_STORE_CFG;

typedef struct tagHY_DVR_CMCC_OVD_MACHASH
{
	int nValid;

	int nChannelID;
	char acMacHash[64];
	
	char reserved1[256];
}HY_DVR_CMCC_OVD_MACHASH, *LPHY_DVR_CMCC_OVD_MACHASH;

typedef struct tagHY_DVR_CMCC_OVD_CLIENT_PARAM
{
	int nValid;

	char OVDDeviceID[32];           //OVD设备ID，必填
	char OVDLoginPassword[64];           //OVD接入密码，必填
	char OVDMediaEncPassword[128];    //OVD视频加密密码，必填
	char OVDHardWareModel[32];      //OVD的硬件型号，必填
	char OVDSystemVersion[32];      //OVD的固件版本号，必填
	char OVDModelId[32];            //modelid,用于区分同一设备型号不同芯片方案的情况
	char OVDmacaddress[32];         //OVD的mac地址,大写，中间以分号相隔

	//OVDNetParam
	char passDomain[64];        //<云服务器的域名>，必填
	int  passPort;       //<云服务器的端口>，必填
	int  Secure_PassPort;  //<云服务器的加密端口>,必填
	char p2p_passDomain[64];    // <P2P pass 的域名>，必填
	int  p2p_passPort;   //<杭研p2p pass的端口>，必填
	char turnDomain[64];        //<P2P turn的域名>，没有置为空
	int  turnPort;       //<p2p turn 的端口>，没有置为-1
	char probeDomain[64];        //<软探针服务器的域名>，必填
	int  probePort;       //<软探针服务器的端口>，必填
	int  probeSecurePort;  //<软探针服务器的加密端口>,必填
	char hibernationDomain[64];        //<休眠服务地址域名>，没有置为空
	int  hibernationPort;       //<休眠服务地址端口>，没有置为-1
	int  hibernationHBInterval;       //<休眠心跳骤起>，没有置为值10s，单位为秒
	int  maxP2PSession;       //设备支持的最多的P2P流的个数，设备固有参数，服务器不能设置

	int enable_servicschedule;      //是否启用服务调度
	char servicescheduleurl[1024];  //服务调度的URL 
	char cafilepath[256];   //客户端ca文件路径
	int tz;                        //设备时区，默认东8区
	char cServerForGet1400Config[256]; //获取1400配置的的服务器
	char cMacHash[64];//macHash,防克隆校验信息，需要烧录固化到设备里，且恢复出产设置时不能擦除。
	char cHttpsDnsPath[128];//域名解析保存ip路径，重启或重置不清除
	char cOvdDataPath[128];//数据文件保存路径，保存flash，不设置会失败
	char cOvdLogPath[128];//本地日志保存路径，保存内存，需可读写，eg:/tmp,如不设置，默认写在/tmp目录下
	char szReserved[320];
}HY_DVR_CMCC_OVD_CLIENT_PARAM, *LPHY_DVR_CMCC_OVD_CLIENT_PARAM;

typedef struct tagHY_DVR_OVD_DEVICE_LOAD
{
  int bValid;
  int nWifiSignal; //WIFI信号强度 db值
  int nCpuUsage;   //CPU负载百分比 0-100
  int nMemUsage;   //内存使用率百分比 0-100
  int nMemLeft;    //剩余内存（KB）
  int nPacketLostRate; //wan口丢包率 0-100  (rx_dropped+tx_dropped)/(rx+tx)
  int nUpBandwidth;   //上行最大带宽，单位bps
  int nDownBandwidth; //下行最大带宽，单位bps
  int nBattery; //可选，整数:当前设备电池电量, 0-100

  int nRunTime; //设备运行时长，单位s
  int nMemTotal; //设备运行总内存，单位KB
  int nFlashTotal;//设备运行总闪存，单位KB
  char acPingAddr[64];//ping地址
  int nPingDelay;//ping 5次的平均延时ms（要求3s内返回回调结果）
  int nPingResult;//成功率（丢包率）（要求3s内返回回调结果）
  char acTraceRouteAddr[64];//TraceRoute地址
  int nTraceRouteDelay;//Traceroute延时，跳到目的地用了多少时间（ms）（要求3s内返回回调结果）
  int nTraceRouteHops;//Traceroute跳数,单位ms,traceroute跳到目的地跳了多少次（要求3s内返回回调结果）
  int nPowerMode;//供电模式，1：POE，2：5V，3：Battery
  char szReserved[1024-160];
}HY_DVR_OVD_DEVICE_LOAD, *LPHY_DVR_OVD_DEVICE_LOAD;

typedef enum eRebootReason
{
	RebootReasonDutCommon=1000, //设备侧通用原因重启
	RebootReasonDutPowerDown=1001, //设备侧断电重启
	RebootReasonDutCrash=1002, //设备侧崩溃触发的重启
	RebootReasonDutHwatchdog=1003, //设备侧硬件看门狗触发的重启
	RebootReasonDutFactoryReset=1004, //设备侧本地恢复出厂设置引发的重启
	RebootReasonDutAbnormalReboot=1005, //设备侧异常重启，可能规避一些异常逻辑而导致的重启
	
	RebootReasonSdkCommon=2000, //sdk逻辑触发的通用原因重启
	RebootReasonSdkScheduledMaintenance=2001,//sdk自动维护触发的重启
	RebootReasonSdkSwatchdog=2002, //sdk软看门狗触发的重启
	
	RebootReasonCloudCommon=3000, //平台触发的通用重启原因
	RebootReasonCloudReboot=3001, //平台触发的远程重启
	RebootReasonCloudUpgrade=3002, //平台触发的升级重启
	RebootReasonCloudFactoryReset=3003, //平台触发的恢复出厂设置
}eDeviceLastRebootReason;

typedef struct tagHY_DVR_CMCC_OVD_REBOOT_REASON
{
	int nValid;
	
	eDeviceLastRebootReason eRebootReason;
	unsigned long long ullRebootTs;
	char szMessage[256];
}HY_DVR_CMCC_OVD_REBOOT_REASON, *LPHY_DVR_CMCC_OVD_REBOOT_REASON;

typedef struct tagHY_DVR_CMCC_OVD_INIT_OTHER_CONFIG
{
	int nValid;

	int nSnapshotSize;		//设备默认清晰度下的截图大小。若不设置，则SDK中默认设置为500k
	int nBuffDuration;			//设备从上电到连接到平台时，需要SDK缓存的音视频内容的时长，单位为秒。若不设置，则SDK中默认设置为0s，建议低功耗设备设置此值，一般设置为一个数据分片的大小（约10s）
	int nDebugTS;				//是否开启媒体数据输出ts开关

	int nSegMaxCount;		//指定云存模块队列缓存最大切片数量，默认值为3个，默认值由平台指令下发
	int nSndDropDelay;  	//单位为毫秒，指定底层流媒体srt连接最大缓存时延，默认值为10000 毫秒
	char szReserved[64 - 8];
}HY_DVR_CMCC_OVD_INIT_OTHER_CONFIG, *LPHY_DVR_CMCC_OVD_INIT_OTHER_CONFIG;

typedef struct tagHY_DVR_MOTION_TRACK_CONFIG
{
	int nValid;

	int nEnable;

	char szReserved[2048];		//保留字段给大，以后可能添加移动跟踪区域设置
}HY_DVR_MOTION_TRACK_CONFIG, *LPHY_DVR_MOTION_TRACK_CONFIG;

typedef struct tagHY_DVR_UPGRADE_INFO
{
	int nValid;

	int nUpgradeStatus;		//0：空闲，1：下载中，2：安装中，3：升级失败，4：升级成功
	int nInstallPercent;		//安装百分比

	char szReserved[32];
}HY_DVR_UPGRADE_INFO, *LPHY_DVR_UPGRADE_INFO;

//ai
typedef struct tagHY_DVR_CMCC_OVD_POINT
{
	int nValid;

	int nPointID;		//<整型，顶点id，从0 开始>
	int nX;		//<整型，x坐标>
	int nY;		//<整型，y坐标>

	char szReserved[32];
}HY_DVR_CMCC_OVD_POINT, *LPHY_DVR_CMCC_OVD_POINT;

typedef struct tagHY_DVR_CMCC_OVD_ALARM_ZONE		//若支持区域配置，则有改配置，将图形10000等分,每个区域由各个顶点组成，顶点需要按照图像的顺时针或逆时针排序
{
	int nValid;

	int nAreaID;		//<整型，areaId 从0 开始>
	HY_DVR_CMCC_OVD_POINT struPoints[64];

	char szReserved[256];
}HY_DVR_CMCC_OVD_ALARM_ZONE, *LPHY_DVR_CMCC_OVD_ALARM_ZONE;

typedef struct tagHY_DVR_CMCC_OVD_ALARM_ZONES
{
	int nValid;

	HY_DVR_CMCC_OVD_ALARM_ZONE struZones[32];
}HY_DVR_CMCC_OVD_ALARM_ZONES, *LPHY_DVR_CMCC_OVD_ALARM_ZONES;

//AI->face
typedef struct tagHY_DVR_CMCC_OVD_FACE
{
	int nValid;

	int nChannelID;
	int nOn;							//<必填，布尔型，是否上报AI平台>
	int nExposureOn;			//<必填，可读可写，布尔型，人脸曝光度调节功能开关
	int nExposureAdjust;			//<必填，可读可写，整型，人脸曝光度调节，0~100>
	char szMode[32];				//<可选，可读可写，字符串，人脸抓拍模式，0：质量抓拍，1: 代表快速抓拍
	int nQualityValue;				//<可选，可读写，整形，选择质量抓拍时，人脸质量抓拍的阈值设置，0~100>
	int nAlarmSensitivity;			//<必填，可读可写，整型，人脸探测灵敏度，0~100>
	HY_DVR_CMCC_OVD_ALARM_ZONES struZones;

	char szReserved[256];
}HY_DVR_CMCC_OVD_FACE, *LPHY_DVR_CMCC_OVD_FACE;

//AI->kitchen_detection
typedef struct tagHY_DVR_CMCC_OVD_KITCHEN_DETECTION
{
	int nValid;

	int nChannelID;
	int nOn;								//布尔型，使能开关
	int nMaskOn;						//布尔型，是否开启检测口罩
	int nCapOn;							//布尔型，是否检测帽子
	int nClothesOn;					//布尔型，是否检测工服
	char szClothesColor[32];		//字符串，默认白色，目前端侧可选配置为：white,black,red
	int nAlarmSensitivity;				//<必填，可读可写, 整型：探测灵敏度， 0 - 100, 目前PC侧设置1~10；可按比例下发>
	HY_DVR_CMCC_OVD_ALARM_ZONES struZones;

	char szReserved[256];
}HY_DVR_CMCC_OVD_KITCHEN_DETECTION, *LPHY_DVR_CMCC_OVD_KITCHEN_DETECTION;

//AI->vehicle_detection
typedef struct tagHY_DVR_CMCC_OVD_SPEECH
{
	int nValid;

	int nOn;				//<可选，可读可写，布尔型，是否联动语音播报>
	int nVol;				//<可选，可读可写，整形，语音播放音量>
	int nRepeat;			//<可选，可读可写，整形，语音播放次数>
	char szUrl[256];	
	char *pAudioData;
	int nAudioLength;

	char szReserved[512];
}HY_DVR_CMCC_OVD_SPEECH, *LPHY_DVR_CMCC_OVD_SPEECH;

typedef struct tagHY_DVR_CMCC_OVD_LIGHT
{
	int nValid;

	int nOn;			//<可选，可读可写, 布尔型，是否联动警灯>
	int nMode;			//<可选，可读可写，整形，告警灯亮灯模式，0：常亮  1：闪烁>
	int nDur;				//<可选，可短可写，整形，告警灯单次亮灯时长，单位：秒>

	char szReserved[32];
}HY_DVR_CMCC_OVD_LIGHT, *LPHY_DVR_CMCC_OVD_LIGHT;

typedef struct tagHY_DVR_CMCC_OVD_BUZ
{
	int nValid;

	int nOn;			//<可选，可读可写，布尔型，是否联动蜂鸣器>
	int nDur;			//<可选，蜂鸣器报警时长，单位为秒>

	char szReserved[32];
}HY_DVR_CMCC_OVD_BUZ, *LPHY_DVR_CMCC_OVD_BUZ;

typedef struct tagHY_DVR_CMCC_OVD_LINKAGE_STRATEGY
{
	int nValid;

	HY_DVR_CMCC_OVD_SPEECH struSpeech;
	HY_DVR_CMCC_OVD_LIGHT struLight;
	HY_DVR_CMCC_OVD_BUZ struBuz;

	char szReserved[2048];
}HY_DVR_CMCC_OVD_LINKAGE_STRATEGY, *LPHY_DVR_CMCC_OVD_LINKAGE_STRATEGY;

typedef struct tagHY_DVR_CMCC_OVD_VEHICLE_DETECTION
{
	int nValid;

	int nChannelID;
	int nOn;								//<必填，布尔型，使能开关>
	int nMotorbikeOn;				//<必填，布尔型，是否检测电瓶车>
	int nBikeOn;						//<必填，布尔型，是否检测自行车>
	HY_DVR_CMCC_OVD_LINKAGE_STRATEGY struLinkageStrategy;		//告警联动策略，若不支持则无此选项
	int nAlarmSensitivity;				//<必填，可读可写, 整型：探测灵敏度， 0 - 100, 目前PC侧设置1~10；可按比例下发>
	HY_DVR_CMCC_OVD_ALARM_ZONES struAlarmZones;
	int nDetectSite; //可读可写，整型，检测地点，1：道路，2：岗亭（默认）>
  	int nExitMode;//可读可写，整型，出口标识，检测地点只为岗亭时有效，1：出口，车头离开，车尾进入 2：入口，车头进入，车尾离开>

	char szReserved[2040];
}HY_DVR_CMCC_OVD_VEHICLE_DETECTION, *LPHY_DVR_CMCC_OVD_VEHICLE_DETECTION;


typedef struct tagHY_DVR_CMCC_OVD_NONMOTORVEHICLE_DETECTION
{
	int nValid;

	int nChannelID;
	int nOn;							//<必填，布尔型，使能开关>
	char detect_mode_list[128];			//<可读写,枚举字符串，可选值有“electromobile”,"bike",默认电瓶车>
	int nAlarmSensitivity;				//<必填，可读可写, 整型：探测灵敏度， 0 - 100, 目前PC侧设置1~10；可按比例下发>
	int quality;						//<可读写，整型，质量评分（0~100）>
	int capture_mode;					//<可读写，枚举整型，0：最优推图，1最快推图，2定时推图>
	int scheduled_capture_time;			//<可读写，整型，定时推图模式下的间隔时间>
	HY_DVR_CMCC_OVD_LINKAGE_STRATEGY struLinkageStrategy;		//告警联动策略，若不支持则无此选项
	HY_DVR_CMCC_OVD_ALARM_ZONES struAlarmZones;
	char szReserved[2048];
}HY_DVR_CMCC_OVD_NONMOTORVEHICLE_DETECTION, *LPHY_DVR_CMCC_OVD_NONMOTORVEHICLE_DETECTION;
//alarms

//alarms->io
typedef struct tagHY_DVR_CMCC_OVD_IO		//外部报警配置，若OVD不具备该能力，该字段不存在
{
	int nValid;

	int nChannelID;
	int nOn;				//<必填，可读可写, 布尔型：使能开关>
	int nSensitivity;		//<必填，可读可写, 整型：探测灵敏度， 0 - 100>

	char szReserved[32];
}HY_DVR_CMCC_OVD_IO, *LPHY_DVR_CMCC_OVD_IO;

//alarms->cry
typedef struct tagHY_DVR_CMCC_OVD_CRY		//哭声侦测配置，若OVD不具备该能力，该字段不存在
{
	int nValid;

	int nChannelID;
	int nOn;				//<必填，可读可写, 布尔型：使能开关>
	int nSensitivity;		//<必填，可读可写, 整型：探测灵敏度， 0 - 100>

	char szReserved[32];
}HY_DVR_CMCC_OVD_CRY, *LPHY_DVR_CMCC_OVD_CRY;

//alarms->voice
typedef struct tagHY_DVR_CMCC_OVD_VOICE		//声音侦测配置，若OVD不具备该能力，该字段不存在
{
	int nValid;

	int nChannelID;
	int nOn;				//<必填，可读可写, 布尔型：使能开关>
	int nSensitivity;		//<必填，可读可写, 整型：探测灵敏度， 0 - 100>

	char szReserved[32];
}HY_DVR_CMCC_OVD_VOICE, *LPHY_DVR_CMCC_OVD_VOICE;

//alarms->motion
typedef struct tagHY_DVR_CMCC_OVD_MOTION		//移动侦测配置，若OVD不具备该能力，该字段不存在
{
	int nValid;

	int nChannelID;
	int nOn;				//<必填，可读可写, 布尔型：使能开关>
	int nSensitivity;		//<必填，可读可写, 整型：探测灵敏度， 0 - 100>
	HY_DVR_CMCC_OVD_ALARM_ZONES struZones;

	char reserved1[256];
}HY_DVR_CMCC_OVD_MOTION, *LPHY_DVR_CMCC_OVD_MOTION;

//alarms->cross
typedef struct tagHY_DVR_CMCC_OVD_CROSS		//拌网配置，若OVD不具备该能力，该字段不存在
{
	int nValid;

	int nChannelID;
	int nOn;				//<必填，可读可写, 布尔型：使能开关>
	int nSensitivity;		//<必填，可读可写, 整型：探测灵敏度， 0 - 100>
	HY_DVR_CMCC_OVD_ALARM_ZONES struZones;

	char reserved1[256];
}HY_DVR_CMCC_OVD_CROSS, *LPHY_DVR_CMCC_OVD_CROSS;

//alarms->alertarea->body
typedef struct tagHY_DVR_CMCC_OVD_BODY		//人形侦测配置，若OVD不具备该能力，该字段不存在
{
	int nValid;

	int nChannelID;
	int nOn;				//<必填，可读可写, 布尔型：使能开关>
	int nSensitivity;		//<必填，可读可写, 整型：探测灵敏度， 0 - 100>
	HY_DVR_CMCC_OVD_ALARM_ZONES struZones;		//若支持区域配置则有该配置，将图形10000等分
	HY_DVR_CMCC_OVD_LINKAGE_STRATEGY struLinkageStrategy;		//告警联动策略，若不支持则无此选项

	char reserved1[256];
}HY_DVR_CMCC_OVD_BODY, *LPHY_DVR_CMCC_OVD_BODY;

//alarms->alertarea->alerttime
typedef struct tagHY_DVR_CMCC_OVD_ALERT_TIME		//<可选，警戒时间段，可读可写>
{
	int nValid;

	int nOn;						//<可选，可读可写，布尔型，是否开启警戒时间段的配置>
	char szStartTime[16];		//<可选，可读可写，字符串，“hh : mm: ss” 格式>
	char szEndTime[16];		//<可选，可读可写，字符串，“hh : mm: ss” 格式> //当endtime早于starttime，就是跨天
	char szRepeats[8][16];			//<必填，字符串数组 : 计划重复的星期号，数组内的的字符串枚举值可选为Mon / Tue / Wed / Thu / Fri / Sat / Sun>

	char reserved1[128];
}HY_DVR_CMCC_OVD_ALERT_TIME, *LPHY_DVR_CMCC_OVD_ALERT_TIME;

//alarms->alertarea
typedef struct tagHY_DVR_CMCC_OVD_ALERT_AREA
{
	int nValid;

	int nChannelID;
	int nOn;				//<必填，可读可写, 布尔型：使能开关>
	int nSensitivity;		//<必填，可读可写, 整型：探测灵敏度， 0 - 100>
	HY_DVR_CMCC_OVD_ALERT_TIME struAlertTime;
	HY_DVR_CMCC_OVD_ALARM_ZONES struAlarmZone;
	HY_DVR_CMCC_OVD_LINKAGE_STRATEGY struLinkageStrategy;

	int nTargetType;//<可读可写,整型：检测目标, 位运算表示如：0x0F，最低位表示"人形"0x01，第二位表示"机动车"0x02，第三位表示"非机动车"0x04，第四位表示"其他"0x08，默认0表示不支持>
	int nStaymodeTime;//<可读可写,整型，单位为秒，区域被入侵后且目标在区域中停留时长满足要求后再告警，范围0-240秒，默认为0秒表示入侵后立刻报警>

	char reserved1[248];
}HY_DVR_CMCC_OVD_ALERT_AREA, *LPHY_DVR_CMCC_OVD_ALERT_AREA;

//alarms->transgression
typedef struct tagHY_DVR_CMCC_OVD_TRANSGRESSION
{
	int nValid;

	int nChannelID;
	int nOn;				//<必填，可读可写, 布尔型：使能开关>
	int nTargetType;//<可读可写,整型：检测目标, 位运算表示如：0x0F，最低位表示"人形"0x01，第二位表示"机动车"0x02，第三位表示"非机动车"0x04，第四位表示"其他"0x08，默认0表示不支持>
	HY_DVR_CMCC_OVD_ALERT_TIME struAlertTime;
	HY_DVR_CMCC_OVD_LINKAGE_STRATEGY struLinkageStrategy;
	//客流规则线,将图形 10000 等分，A 的 x 小于等于 B 的 x，当 A 的 x等于 B 的 x 时，A 的 y 小于 B 的 y
	HY_DVR_CMCC_OVD_POINT struAStatisticsLine;//默认值（2500,5000）
	HY_DVR_CMCC_OVD_POINT struBStatisticsLine;//默认值（7500,5000）
	int nStatisticsModeStyle;//<可读写,整型,0表示单向越界方式,1表示双向，默认值>
	int nStatisticsModeDir;//<可读写,整型,越界方式,0代表上出下进,1代表上进下出，默认值为0> 
	
	char reserved1[256];
}HY_DVR_CMCC_OVD_TRANSGRESSION, *LPHY_DVR_CMCC_OVD_TRANSGRESSION;

//未戴口罩检测配置
typedef struct tagHY_DVR_CMCC_OVD_MASK_DETECTION
{
	int nValid;

	int nChannelID;
	int nOn;								//<必填，布尔型，使能开关>
	HY_DVR_CMCC_OVD_ALERT_TIME struAlertTime; 
	HY_DVR_CMCC_OVD_LINKAGE_STRATEGY struLinkageStrategy;		//告警联动策略，若不支持则无此选项

	char szReserved[2048];
}HY_DVR_CMCC_OVD_MASK_DETECTION, *LPHY_DVR_CMCC_OVD_MASK_DETECTION;

//客流统计配置
typedef struct tagHY_DVR_CMCC_OVD_PASSENGER_FLOW
{
	int nValid;

	int nChannelID;
	int nOn;							//<必填，布尔型，是否上报AI平台>
	int nExposureOn;			//<必填，可读可写，布尔型，人脸曝光度调节功能开关
	int nExposureAdjust;			//<必填，可读可写，整型，人脸曝光度调节，0~100>
	char szMode[32];				//<可选，可读可写，字符串，人脸抓拍模式，0：质量抓拍，1: 代表快速抓拍
	int nQualityValue;				//<可选，可读写，整形，选择质量抓拍时，人脸质量抓拍的阈值设置，0~100>
	int nAlarmSensitivity;			//<必填，可读可写，整型，人脸探测灵敏度，0~100>
	HY_DVR_CMCC_OVD_POINT struAStatisticsLine;//客流规则线,将图形 10000 等分，A 的 x 小于等于 B 的 x，当 A 的 x等于 B 的 x 时，A 的 y 小于 B 的 y
	HY_DVR_CMCC_OVD_POINT struBStatisticsLine;
	int nStatisticsMode;//可读写，整型，统计方式,0 代表上出下进，1 代表上进下出, A 朝向B 方向左侧为上，A 朝向 B 方向右侧为下
	HY_DVR_CMCC_OVD_ALARM_ZONES struZones;
	int nOsdStatus;                //<可读可写，布尔型, OSD 叠加人数：默认为支持>
	
	char szReserved[252];
}HY_DVR_CMCC_OVD_PASSENGER_FLOW, *LPHY_DVR_CMCC_OVD_PASSENGER_FLOW;

typedef struct tagHY_DVR_CMCC_1400_CFG
{
	int nValid;

	int nChannelID;
	int nTrafficRepInterval;    //客流统计上报间隔，默认1800，单位s
	
	int nFaceScene;             //int类型，人脸背景图是否携带,0不携带，1携带
	int nKitchenScene;          //int类型，明厨亮灶背景图是否携带,0不携带，1携带
	int nMotorVehicleScene;     //int类型，机动车背景图是否携带,0不携带，1携带
	int nNonMotorVehicleScene;  //int类型，非机动车背景图是否携带,0不携带，1携带
	int nPersonScene;           //int类型，人形背景图是否携带,0不携带，1携带
	
	int nHumanBorder;           //int类型，人形框图是否携带
	
	char szReserved[1024];
}HY_DVR_CMCC_1400_CFG;

typedef struct tagHY_DVR_CMCC_OVD_ALARM_ZONES_V2
{
	int nValid;

	HY_DVR_CMCC_OVD_ALARM_ZONE struZones[4];
}HY_DVR_CMCC_OVD_ALARM_ZONES_V2, *LPHY_DVR_CMCC_OVD_ALARM_ZONES_V2;

//在离岗配置定义
#define OVD_OFF_DUTY_MAX 8
typedef struct tagHY_DVR_CMCC_OVD_OFF_DUTY
{
	int nValid;

	int nOn;											  //<可读写，布尔型，是否上报 AI 平台>
	int nRegionalId;									  //< 可读写，整型，区域 id 号，从 0 开始>
	char szRegionalName[128];							  //<可读写,字符串,区域信息描述,最大支持 127 个字符,不支持特殊字符，默认值为空字符串>
	int nOnDutyCount;                                     //<可读写,整型,范围 1-能力集支持的最大值，默认值为1>
	int nOffDutyDuration;                                 //<可读可写，整型，离岗时长：分钟，1-对应能力集配置的值，默认值为10>
	HY_DVR_CMCC_OVD_ALERT_TIME struAlertTime;			  //<检测时间段，可读可写>
	HY_DVR_CMCC_OVD_ALARM_ZONES_V2 struAlarmZone;		  // 若支持区域配置，则有改配置，将图形 10000 等分
	HY_DVR_CMCC_OVD_LINKAGE_STRATEGY struLinkageStrategy; // 告警联动策略，暂不支持

	char reserved1[256];
} HY_DVR_CMCC_OVD_OFF_DUTY, *LHY_DVR_CMCC_OVD_OFF_DUTY;

typedef struct tagHY_DVR_CMCC_OVD_OFF_DUTY_LIST
{
	int nValid;

	int nChannelID;

    int nDetectOn; //<可读写,布尔型,默认 false 关闭,离岗检测总开关,是否上报 AI 平台，默认false>
	//目前要求只支持一个，为方便扩展，配置使用数组形式
	HY_DVR_CMCC_OVD_OFF_DUTY struOffDuty[OVD_OFF_DUTY_MAX];
	
	char reserved1[1024];
}HY_DVR_CMCC_OVD_OFF_DUTY_LIST,*LHY_DVR_CMCC_OVD_OFF_DUTY_LIST;

#define OVD_REGIONAL_PEOPLE_MAX 8
typedef struct tagHY_DVR_CMCC_OVD_REGIONAL_PEOPLE_STAT
{
	int nValid;

	int nOn;											  //<可读写，布尔型，是否上报 AI 平台>
	int nRegionalId;									  ///< 可读写，整型，区域 id 号，从 0 开始>
	char szRegionalName[128];							  //<可读写，字符串，区域信息描述，最大支持 8 个汉字，16 个字符>
	int nRegionalPeopleCount;							  //<可读写，整型，默认 30,范围 1-100；一旦超过预警人数触发告警，告警上报走 SDK>
	int nAlarmReportDuration;							  //<可读可写，整型，单位min，告警上报间隔时间：默认 5 分钟，1-30 分钟>
	int nAlarmSpeechStatus;								  //<可读可写，布尔型, 语音告警状态，默认不启用>
	HY_DVR_CMCC_OVD_ALERT_TIME struAlertTime;			  //<检测时间段，可读可写>
	HY_DVR_CMCC_OVD_ALARM_ZONES_V2 struAlarmZone;		  // 若支持区域配置，则有改配置，将图形 10000 等分
	HY_DVR_CMCC_OVD_LINKAGE_STRATEGY struLinkageStrategy; // 告警联动策略，若不支持则无此选项
	int nDetectResultReportDur;							  //<可读可写，整型，区域人数定时上传：默认 5 分钟（取 5 分钟内平均值），范围 1-30 分钟。区域人数通过 1400 协议上传 >
	int nOsdStatus;										  //<可读可写，布尔型, OSD 叠加人数：默认为支持>

	char reserved1[256];
} HY_DVR_CMCC_OVD_REGIONAL_PEOPLE_STAT, *LHY_DVR_CMCC_OVD_REGIONAL_PEOPLE_STAT;

typedef struct tagHY_DVR_CMCC_OVD_REGIONAL_PEOPLE_STATLIST
{
	int nValid;

	int nChannelID;

    int nDetectOn; //<可读写,布尔型,默认 false 关闭,区域人数统计检测总开关,是否上报 AI 平台，默认false>
	//区域人数统计配置, 目前要求只支持一个，为方便扩展，配置使用数组形式
	HY_DVR_CMCC_OVD_REGIONAL_PEOPLE_STAT struRegPeoStat[OVD_REGIONAL_PEOPLE_MAX];
	
	char reserved1[1024];
}HY_DVR_CMCC_OVD_REGIONAL_PEOPLE_STATLIST,*LHY_DVR_CMCC_OVD_REGIONAL_PEOPLE_STATLIST;


typedef struct tagHY_DVR_CMCC_OVD_PIR		//红外物体移动侦测，若OVD不具备该能力，该字段不存在
{
	int nValid;

	int nChannelID;
	int nOn;						//<必填，可读可写, 布尔型：使能开关>
	int nSensitivity;				//<必填，可读可写, 整型：探测灵敏度， 0 - 100>
	int nStaymodeTime;		//<必填，可读可写，整形，单位为秒，若ovd不具备逗留模式设置，则该字段不存在>

	char reserved1[32];
}HY_DVR_CMCC_OVD_PIR, *LPHY_DVR_CMCC_OVD_PIR;

typedef struct tagHY_DVR_CMCC_OVD_LOSS_LOCK		//防拆告警
{
	int nValid;

	int nChannelID;
	int nOn;		//<必填，可读可写，布尔型，使能开关>

	char reserved1[32];
}HY_DVR_CMCC_OVD_LOSS_LOCK, *LPHY_DVR_CMCC_OVD_LOSS_LOCK;

//osd_text
typedef struct tagHY_DVR_CMCC_OVD_LOSS_COLOR
{
	int nValid;

	int nRed;			//int,范围0~255
	int nGreen;		//int,范围0~255
	int nBlue;		//int，范围0~255

	char szReserved[32];
}HY_DVR_CMCC_OVD_LOSS_COLOR, *LPHY_DVR_CMCC_OVD_LOSS_COLOR;

typedef struct tagHY_DVR_CMCC_OVD_LOSS_POS
{
	int nValid;

	int nX;
	int nY;

	char szReserved[32];
}HY_DVR_CMCC_OVD_LOSS_POS, *LPHY_DVR_CMCC_OVD_LOSS_POS;

typedef struct tagHY_DVR_CMCC_OVD_OSD_TEXT
{
	int nValid;

	char szText[64];		//必填，字符串类型，osd文本内容
	HY_DVR_CMCC_OVD_LOSS_POS struPos;				//该osd左上角坐标信息，将图形10000等分
	int nOn;				//布尔型，是否显示
	int nFontSize;			//字体大小，枚举值，0~3; 0代表厂商默认形式，1、2、3分别代码，小、中、大，三个等级
	HY_DVR_CMCC_OVD_LOSS_COLOR struColor;				//字体颜色默认下发RGB（255，255，255），厂家可处理为白色或者为黑白反色或者白色加黑色描边，以最佳用户体验呈现
	
	int nbigOSDTextFlag;
	char szBigText[128];
	char szReserved[1024 - 128 - 4];
}HY_DVR_CMCC_OVD_OSD_TEXT, *LPHY_DVR_CMCC_OVD_OSD_TEXT;

typedef struct tagHY_DVR_CMCC_OVD_OSD_TEXTS
{
	int nValid;

	int nChannelID;
	HY_DVR_CMCC_OVD_OSD_TEXT struOSDText[8];

	char szReserved[1024];
}HY_DVR_CMCC_OVD_OSD_TEXTS, *LPHY_DVR_CMCC_OVD_OSD_TEXTS;

typedef struct tagHY_DVR_CMCC_OVD_OSD_LOGO
{
	int nValid;

	int nChannelID;
	int nOn;		// logo水印是否显示

	char szReserved[64];
}HY_DVR_CMCC_OVD_OSD_LOGO, *LPHY_DVR_CMCC_OVD_OSD_LOGO;

typedef struct tagHY_DVR_CMCC_OVD_SWITCH_SCHEDULE
{
	int nValid;

	int nChannelID;
	int nOn;
	char szStartTime[32];
	char szShutdownTime[32];
	char repeat[8][16];

	char szReserved[256];
}HY_DVR_CMCC_OVD_SWITCH_SCHEDULE, *LPHY_DVR_CMCC_OVD_SWITCH_SCHEDULE;

typedef struct tagHY_DVR_CMCC_OVD_CHANNEL_OTHER_CONFIG
{
	int nValid;

	int nChannelID;
	int nOn;		//0: 离线		1：在线		2：异常		3：不使能
	int nAudioOutVolume;
	int nDoorBellVolume;
	int nTrace;
	int nNormalNightVisionMode;
	int nColorNightVisionMode;

	char szExtension[256];
}HY_DVR_CMCC_OVD_CHANNEL_OTHER_CONFIG, *LPHY_DVR_CMCC_OVD_CHANNEL_OTHER_CONFIG;

typedef struct tagHY_DVR_CMCC_OVD_CHANNEL_PTZ_PRESET_LIST_CONFIG
{
	int nValid;

	int nChannelID;
	char szPresetList[256];

	char szReserved[256];
}HY_DVR_CMCC_OVD_CHANNEL_PTZ_PRESET_LIST_CONFIG, *LPHY_DVR_CMCC_OVD_CHANNEL_PTZ_PRESET_LIST_CONFIG;

typedef struct tagHY_DVR_PTZ_LINKAGE_GOTO_CFG
{
    int iValid;

    int iChannel;                                       /*!< 通道号;从0开始 */
    int iEnable;                                        /*!< 是否启用;0-否,1-是 */
    int iX;                                             /*!< 位置x坐标 */
    int iY;                                             /*!< 位置y坐标 */

    char acReserved[256];
} HY_DVR_PTZ_LINKAGE_GOTO_CFG, *LPHY_DVR_PTZ_LINKAGE_GOTO_CFG;

typedef struct tagHY_DVR_CMCC_OVD_AUTO_REBOOT		//自动维护（重启）能力，若OVD不具备该能力，则该字段不存在
{
	int nValid;

	int nOn;							//<必填，可读可写, 布尔型：使能开关>
	int nCycle;							//<必填，可读可写, 整型：自动维护（重启）的最短周期，单位秒，例如7天自动重启，可以设置为604800>
	char szStart[16];					//<必填，可读可写, 字符串：自动维护（重启）开始时间，格式“HH : MM : SS”>
	char szEnd[16];					//<必填，可读可写, 字符串：自动维护（重启）结束时间，格式“HH : MM : SS”>
	char szLastReboot[32];		//<必填，只读, 字符串型：上次重启时间，格式”YY - MM - DDTHH : MM : SS”>

	char szExtension[256];
}HY_DVR_CMCC_OVD_AUTO_REBOOT, *LPHY_DVR_CMCC_OVD_AUTO_REBOOT;

typedef struct tagHY_DVR_CMCC_OVD_DEVICE_OTHER_CONFIG
{
	int nValid;

	int nLogLevel;
	int nTimeZone;
	HY_DVR_CMCC_OVD_AUTO_REBOOT struAutoReboot;
	int nLed;
	int nSoftProbeOn;
	int nDutMTU;
	int nNormalNightVisionMode;
	int nColorNightVisionMode;
	int nLogContrl;

	char szExtension[256 - 4];
}HY_DVR_CMCC_OVD_DEVICE_OTHER_CONFIG, *LPHY_DVR_CMCC_OVD_DEVICE_OTHER_CONFIG;

typedef struct tagHY_DVR_CMCC_OVD_KEEP_AWAKEN_UTIL_EXPIRED
{
	int nValid;

	int nChannel;
	int nNotAllowHibernate;
	int nExpired;
	int nReason;		//0:OVC信令交互，需要保持一段时间不断电		1:P2P开始NAT穿透打洞		2:正在P2P通话

	char szReserved[32];
}HY_DVR_CMCC_OVD_KEEP_AWAKEN_UTIL_EXPIRED, *LPHY_DVR_CMCC_OVD_KEEP_AWAKEN_UTIL_EXPIRED;

typedef struct tagHY_DVR_CMCC_OVD_QUALITY_STREAM_MAP
{
	int nValid;

	int nMapNum;
	char nQuality[16];
	eMediaCodeType eCodeType[16];

	char szReserved[256];
}HY_DVR_CMCC_OVD_QUALITY_STREAM_MAP,*LPHY_DVR_CMCC_OVD_QUALITY_STREAM_MAP;

typedef struct tagHY_DVR_CMCC_OVD_SOFTPROBE_STATUS_INFO
{
	int bValid;
	int nWifiSignal; //WIFI信号强度 db值
	int nCpuUsage;   //CPU负载百分比
	int nMemUsage;   //内存使用率百分比
	int nMemLeft;    //剩余内存（KB）
	int nPacketLostRate; //wan口丢包率  (rx_dropped+tx_dropped)/(rx+tx)
	int nMaxRTT;    //最大RTT时延(ms)
    int nMinRTT;    //最小RTT时延(ms)
    int nAvgRTT;    //平均RTT时延(ms)

	char szReserved[1024];
}HY_DVR_CMCC_OVD_SOFTPROBE_STATUS_INFO, *LPHY_DVR_CMCC_OVD_SOFTPROBE_STATUS_INFO;

typedef struct tagHY_DVR_CMCC_OVD_VIDEO_ENCODING
{
	int nValid;

	int nCurrentQuality;			//0: ld		1:sd		2:hd		3:fhd
	int nChannel;

	char szReserved[252];
}HY_DVR_CMCC_OVD_VIDEO_ENCODING, *LPHY_DVR_CMCC_OVD_VIDEO_ENCODING;

typedef struct tagHY_DVR_CMCC_OVD_LOG_INFO
{
	int nValid;

	AbsoluteTime struStartTime;
	AbsoluteTime struEndTime;
	char szLogFilePath[256];

	char szReserved[1024];
}HY_DVR_CMCC_OVD_LOG_INFO, *LPHY_DVR_CMCC_OVD_LOG_INFO;

typedef struct tagHY_DVR_CMCC_OVD_DEVICE_REBOOT_V1
{
	int nValid;

	int nChannel;

	char szReserved[32];
}HY_DVR_CMCC_OVD_DEVICE_REBOOT_V1, *LPHY_DVR_CMCC_OVD_DEVICE_REBOOT_V1;

// 配网模式
typedef enum
{
	HY_NETWOKR_MODE_WIRED = 0x01,  // 表示有线配网设备
	HY_NETWOKR_MODE_WIFI = 0x02,  // 表示WIFI配网设备,sdk自主选择采用qlink配网还是soft配网方式
	HY_NETWOKR_MODE_4G, 			// 表示4G配网设备
	HY_NETWOKR_MODE_BT, 			// 表示蓝牙配网设备
	HY_NETWOKR_MODE_OTHERS, 		// 表示其他配网设备

	HY_NETWOKR_MODE_WIFI_NOTNEED_ADLCFGNET = 0x12, // 表示无需使用andlink配网功能的wifi设备
	HY_NETWOKR_MODE_MAX
}HY_NET_MODE_e;

typedef struct tagHY_DVR_ANDLINK_DM_INFO
{
	int nValid;
	char cCMEI[32];					// 必填,设备唯一标识
	char cDeviceType[32];			// 设备类型ID,即门户上的 产品ID;
	char cProductToken[128];		// 设备预置秘钥,即门户上的 产品验证码;
	char cAndlinkToken[128];		// 杭研云平台秘钥,即门户上的 平台验证码;
	char cDeviceMac[16];            // 设备MAC地址，格式为将MAC地址的冒号去掉，比如F0C814295666，是由MAC地址为F0:C8:14:29:56:66生成
	char cAndlinkDMCfgPath[128];   // Andlink SDK配置文件存储位置
	char cOSVersion[64];        // 必填,操作系统(包含版本号),比如arm-linux32 2.6.18
	int nAuthMode;				//必填, 0 一型一密;1 一机一密
	char cDevManuDate[32];		// 必填,设备生产日期,格式为:年-月
	char cAHSVersion[64]; 		//双模版本填写AOS-IPC-Linux V1.0.0
	char cDeviceVendor[64];     // 必填,设备制造商
	char cDeviceBrand[64];      // 必填,设备品牌
	char cDeviceModel[64];      // 必填,设备型号
	char cPowerSupplyMode[64]; //  必填,供电方式，电池供电:battery;   POE供电:POE;   市电:220V(110V);   USB供电:USB;   其他方式:other
	char cCPUModel[64];		//若使用必填,处理器型号, 比如Allwinner-A40i
	char cRomStorageSize[32];	//若使用必填,设备的存储总容量（ROM）大小，与工信部登记终端ROM信息一致
	char cRamStorageSize[32];	//若使用必填,设备的内存总容量（RAM）大小，与工信部登记争端RAM信息一致
	char cNetworkType[32];	//必填,网络类型分RJ45（有线）,wifi, 5G, 4G, 3G, NB, ZigBee等
	HY_NET_MODE_e nAndlinkDMcfgNetMode;//
	char cWlanMac[32];			//wifi mac地址，格式是112233445566
	char cwifiRssi[16];			//wifi信号场强，格式是-60
	char candimsVersion[32];	// 和家固话 SDK,若使用必填;
	char candnlpVersion[16];	// 和家语音交互 SDK,若使用必填;	   	
	char canddotVersion[32];	// 和家安防 SDK,若使用必填;	   	     	  
	char candfacerecVersion[16];	// 和家人脸识别 SDK,若使用必填;	
	char cstbId[32];				// 机顶盒ID，机顶盒设备必填，11110
	char clocationInfo[64];			//可选,位置，123.52958679200002,25.77701556036132,1
	char cbluetoothMacAddress[32];  // 蓝牙设备必填,蓝牙设备的 MAC地址,665544332211
	char cmoduleType[32];			//模组类型, NB、WiFi、Zigbee、Bluetooth、Thread、lora、ZWAVE等,WiFi
	char cmoduleVendor[32];			// 模组厂商名,11110
	char cmoduleBrand[32];			// 模组品牌名,11110
	char cmoduleModel[32];			// 模组型号,11110
	char cctaNetworkModel[32];		// CTA入网许可证型号,支持蜂窝入网的模组必填,11110
	char cmoduleFirmwareVersion[32];// 模组固件版本,11110
	char cmoduleSystemVersion[32];// 模组系统版本,11110
	char cmoduleImei[32];			// 模组IMEI，11110
	char cmoduleImsi[32];			// 模组IMSI，11110
	char cmoduleIccid[32];			// 模组ICCID号，11110
	char cmoduleCellid[32];			// 模组Cellid，11110
	char cmoduleLac[32];			// 模组Lac，11110
	char cmoduleBleMacAddress[32];	// 模组蓝牙MAC地址，11110
	char cmoduleMacAddress[32];		// 模组MAC地址，11110
	char cmoduleWlanMac[32];		// 模组采集的设备Wlan MAC地址，11110
	char cmoduleSn[32];				// 模组SN，11110
	char ctype[32];
	char cfactory[32];
	char cmodel[32];
    int  IsSupportLampCfg;
	char chipSupplier[64];			//芯片厂家
	char candAndlinkVersion[16];	// andlink SDK版本,若使用必填;	
	char cReserve[1024 - 64 - 640 - 96 - 4 - 64 - 16];		
}HY_DVR_ANDLINK_DM_INFO, *LPHY_DVR_ANDLINK_DM_INFO;

typedef struct tagHY_DVR_OVD_ALARM_CFG
{
	int nValid;
	int nAlarmType;	//0,声音警戒，1，灯光警戒 ,2.电动车 3 未戴口罩声音报警，4 未戴口罩光报警，5 区域人数声音报警，6 区域人数光报警 7越界侦测声音报警 8越界侦测光报警
	int nStatus;	//0,开始，1，立即停止，2，延迟停止
	char cReserve[16];
}HY_DVR_OVD_ALARM_CFG, *LPHY_DVR_OVD_ALARM_CFG;

typedef struct tagHY_DVR_OVD_ADL_NOTIFY_INFO
{
	int nValid;
	//0-无感配网语音播报 1-统一认证语音播报
	int nNotifyType; 
	// 0 = nNotifyType时, 
	//     	0 = nNotifySubType,  		// 开始配网认证中，请打开和家亲等待
	//     	1 = nNotifySubType,  		// 扫描引导热点失败，请扫码配网
	//     	2 = nNotifySubType, 		// 周围引导热点信号较差，请靠近路由器或有线连接路由求后，并重启设备 not use
	//     	3 = nNotifySubType, 	    // 连接引导热点失败,请扫码配网
	//     	4 = nNotifySubType,  	    // 连接工作热点失败,请扫码配网
	//     	5 = nNotifySubType,         // 连接引导热点成功
	//     	6 = nNotifySubType,         // 设备已成功连接网络,请注意查收短信或和家亲消息进行确认
	// 1 = nNotifyType时,
	//     	0 = nNotifySubType,         //连接网关失败，请扫码配网，注：本语音由调用方播放
    //     	1 = nNotifySubType,	        //获取用户身份信息失败，请扫码配网
    //     	2 = nNotifySubType,         //请扫绿码进行家宽账号预绑定
    //     	3 = nNotifySubType,         //请注意查收短信或和家亲消息进行绑定确认
    //     	4 = nNotifySubType,	        //设备即将与尾号为****的用户绑定,号码存在cData中
    //     	5 = nNotifySubType,         //设备已与手机尾号后四位****用户绑定,号码存在cData中
	//     	6 = nNotifySubType,	        //网络不通，请检查摄像头与路由器间的网络连接状况
	int nNotifySubType;
	char cData[64];  // 附加数据，如电话号码等，参考nNotifySubType
} HY_DVR_OVD_ADL_NOTIFY_INFO, *LPHY_DVR_OVD_ADL_NOTIFY_INFO;

//区域人数告警 图片数据紧接在后面
typedef struct tagHY_DVR_OVDSDK_REGIONAL_PEOPLE_ALARM
{
    int nValid;
    int nPeopleConut;//当前人数，暂时无用
    int nRegionalId;//区域ID

	char szReserved[256];
}HY_DVR_OVDSDK_REGIONAL_PEOPLE_ALARM;

//越界侦测事件 图片数据紧接在后面
typedef struct tagHY_DVR_OVDSDK_TRANSGRESS_ALARM
{
    int nValid;
    int nStatisticsModeDir;//越界方式,0代表上出下进,1代表上进下出,暂时无用

	char szReserved[1024];
}HY_DVR_OVDSDK_TRANSGRESS_ALARM;

//离岗检测事件 图片数据紧接在后面
typedef struct tagHY_DVR_OVDSDK_OFF_DUTY_ALARM
{
    int nValid;
	
	int nRegionalId;//区域ID
	int nOffDutyTime;//离岗时长，单位分钟

	char szReserved[1024];
}HY_DVR_OVDSDK_OFF_DUTY_ALARM;

typedef struct tagHY_DVR_WEKSOCKET_BASE_CFG
{
	int nValid;

	int nListenPort;

	char szReserved[128];
}HY_DVR_WEKSOCKET_BASE_CFG, *LPHY_DVR_WEKSOCKET_BASE_CFG;

////////// P6SMessagePush ///////
//设备初始化参数:中间层提供
typedef struct tagHY_DVR_MP_SERVER_URL_INFO
{
	int nValid;
	char strServerIpAndPort[128];     //云消息服务器地址,eg:http://124.71.223.236:8089
	char strUrlPathTimeCheck[128];    //eg:/message-server/api/v1/msg/now
	char strUrlPathEventPolicy[128];  //eg:/message-server/api/v1/msg/tactics 
	char strUrlPathEventPost[128];    //eg:/message-server/api/v1/msg/push-event
	char strUrlPathSetXmlStatus[128]; //eg:/message-server/api/v1/msg/set-xml
	char strSrvMaintenance[128];      //云运维服务器地址,eg:http://124.71.223.236:8099
}HY_DVR_MP_SERVER_URL_INFO, *LPHY_DVR_MP_SERVER_URL_INFO;

typedef struct tagHY_DVR_MP_UPDATE_STATUS_INFO
{
	int nValid;
	int updateStatus;   // 设备是否处于升级状态中，1-处于升级状态中（这个状态是从协议回调升级包路径给中层开始，到设备升级结束(包括升级成功和失败)）；0-未处于升级状态中
	char szReserved[128];
}HY_DVR_MP_UPDATE_STATUS_INFO,*LPHY_DVR_MP_UPDATE_STATUS_INFO;


////     以下是联通协议层的结构体，由拷贝移动的结构体改名得到:CM -> CU        ////

typedef struct tagHY_DVR_CUDOT_DEVICE_LOAD
{
	int bValid;
	int nWifiSignal;        //WIFI信号强度 db值
	int nCpuUsage;          //CPU负载百分比
	int nMemUsage;          //内存使用率百分比
	int nMemLeft;           //剩余内存（KB）
	int nPacketLostRate;    //wan口丢包率  (rx_dropped+tx_dropped)/(rx+tx)
}HY_DVR_CUDOT_DEVICE_LOAD, *LPHY_DVR_CUDOT_DEVICE_LOAD;

typedef struct tagHY_DVR_CUDOT_SPEAKER_PART
{
	int nStatus;                       // 状态，0-离线 1-在线
	int nNeedStop;                     // 是否需要停止 0-不关闭，1-关闭
	char szStopReason[32];             // 停止原因
}HY_DVR_CUDOT_SPEAKER_PART;

typedef struct tagHY_DVR_CUDOT_SPEAKER_STATUS_PRIORITY
{
	int bValid;
	int nPlayKindStatus;                          // 播放种类 0-无播放 1-播放VOIP 2-播放语音对讲 3-播放声光报警 4-播放梯控报警 5-播放机动车报警
	int nPlayRequestType;                         // 播放请求类型 0-无播放 1-播放VOIPCALL 2-播放VOIP广播 3-播放语音对讲 4-播放声光报警 5-播放梯控报警 6-播放机动车报警
	HY_DVR_CUDOT_SPEAKER_PART stVoipCall;         // VOIP电话
	HY_DVR_CUDOT_SPEAKER_PART stVoipBroadCast;    // VOIP云广播
	HY_DVR_CUDOT_SPEAKER_PART stSpeech;           // 语音对讲
	HY_DVR_CUDOT_SPEAKER_PART stSoundLight;       // 声光报警
	HY_DVR_CUDOT_SPEAKER_PART stEbike;            // 梯控报警
	HY_DVR_CUDOT_SPEAKER_PART stMotorVehicle;     // 机动车报警
}HY_DVR_CUDOT_SPEAKER_STATUS_PRIORITY, *LPHY_DVR_CUDOT_SPEAKER_STATUS_PRIORITY;

typedef struct tagHY_DVR_CUDOT_PATH
{
	int nValid;
	char cConfigPath[256];
	//char cLogoPath[256];
	char cLogPath[256];
	int nMaxLogLine;	//日志最大行数（0的话默认10000行，最小1000行）
	int nLogLevel;		//日志级别，参考DoT_LogLevel_en
}HY_DVR_CUDOT_PATH;

#define CU_SN_LEN (15)
#define CU_BINDCODE_LEN (8)
#define CU_PASSWORD_LEN (8)
typedef struct tagHY_DVR_CUDOT_DEVICE_INFO
{
    //联通暂时只需要SN、ModelID
	int bValid;
	// 设备序列号,15个字节,ex:2000xxxx0000001
	char szSn[CU_SN_LEN + 1];
	// 平台接入码,8字节 ex:1C94piOs
	char szBindCode[CU_BINDCODE_LEN + 1];
	// 密码,8字节 ex:1C94piOs
	char szPassword[CU_PASSWORD_LEN + 1];
	//设备名称
	char szModel[64];
}HY_DVR_CUDOT_DEVICE_INFO;

typedef struct tagHY_DVR_CUDOT_AUTH_INFO
{
	int nValid;

	int nAuthStatus;		//授权状态

	//旷世
	char szSerialNumber[32];	//序列号

	//CUDOT平台
	char szDotSn[CU_SN_LEN + 1];// 移动设备序列号,15个字节,ex:2000xxxx0000001
	char szBindCode[CU_BINDCODE_LEN + 1];// 平台接入码,8字节 ex:1C94piOs
	char szPassword[CU_PASSWORD_LEN + 1];// 密码,8字节 ex:1C94piOs

	//DM平台
	char szCMDotCMEI_IMEI[64]; 	//CMEI/IMEI,和IMEI2选一个填,参考规范文档;设备没有的话向移动申请； ex:111041730000001

	char szReserved[256];
}HY_DVR_CUDOT_AUTH_INFO, *LPHY_DVR_CUDOT_AUTH_INFO;

typedef struct tagHY_DVR_CUDOT_DEVICE_INFO_EX
{
	int bValid;
	//每一个通道支持的码流个数
	int nStreamsPerChannel;
	//每一个码流最大buffer,最大不能唱过5M
	int nBufferSizePerStream;
	//设备类型，参考DoT_DeviceType_en枚举
	int nDeviceType;
	//支持的设备分辨率，参考枚举DoT_Resolution_en，使用掩码的方式支持多种
	int nSupportResolution;
	//最大关键帧大小，不能超过800 * 1024
	int nMaxIFrameSize;
	//绑定模式，参考枚举DoT_BindMode_en，使用掩码的方式支持多种
	int nBindMode;
	//事件触发停止的超时时间
	int m_nEventStopTimeout;
	//事件模式，目前固定为0
	int nEventMode;
	//流数据缓存目录路径，确保一直可写，不设置则默认为tmp路径, 不能取外部存储(如TFCard)设备路径，因为无法保证一直可写，可能会造成云存异常，建议采用/tmp路径
	char szStreamDataPath[256];
	//流数据缓存目录路径预留空间大小（字节），最小4MByte，小于4M默认取4M，为了提高云录像性能，建议在空间允许的情况下取更大值
	int nStreamPathSpace;
}HY_DVR_CUDOT_DEVICE_INFO_EX;

typedef struct tagHY_DVR_CUDOT_DEVICE_INFO_ADD
{
    int bValid;
	// 水印图片、logo保存路径，需保证至少有150KB的空间
	char szLogoPath[256];    

	//支持的设备分辨率集合，用于logo水印下载，参考枚举DoT_Resolution_en，使用掩码的方式支持多种
	int nSupportResolutionLogo;
	
	// 流数据是否高优先级处理
	int nStreamHighPrio;

	char cReserve[64];
} HY_DVR_CUDOT_DEVICE_INFO_ADD;

typedef struct tagHY_DVR_CUDOT_ALGORITHM_SUPPORT
{
	// 0x0001 支持人形检测算法
	// 0x0002 支持人脸检测算法
	// 0x0004 支持车牌检测算法
	// 0x0008 支持人脸比对算法
	// 0x0010 支持机动车检测算法
	// 0x0020 支持电瓶车检测算法
	int bValid;
	int nAlgorithmSet[32]; // 算法能力集合，从序号0开始。例如设备支持【方式1，人形检测和人脸检测】、【方式2 电瓶车检测】、【方式3 支持机动车和车牌检测】
	// nAlgorithmSet[0] = 0x01+ 0x02;
	// nAlgorithmSet[1] = 0x20;
	// nAlgorithmSet[2] = 0x04 + 0x10;
	// nAlgorithmSetNumber = 3;
	int nAlgorithmSetNumber; // 算法能力集合个数

	int nRegionHackAlgorithm; // 算法支持告警入侵的方式，按位或方式，例如设备支持【1.人形】、【2.机动车】、【3.电瓶车】触发警戒
	// nRegionHackAlgorithm = 0x01 + 0x10 + 0x20;

} HY_DVR_CUDOT_ALGORITHM_SUPPORT, *LPHY_DVR_CUDOT_ALGORITHM_SUPPORT;

typedef struct tagHY_DVR_CUDOT_CAR_PLATE_DETECT_STATUS
{
	int bValid;
	int nCarPlateDetectEnable; // 车牌识别开关
} HY_DVR_CUDOT_CAR_PLATE_DETECT_STATUS, *LPHY_DVR_CUDOT_CAR_PLATE_DETECT_STATUS;

typedef struct tagHY_DVR_CUDOT_REGION_HACK_ALGORITHM
{
	int bValid;
	int nRegionhackAlgorithm; // 告警入侵算法，可能是组合参数，参数意义Refer HY_DVR_CUDOT_ALGORITHM_SUPPORT
} HY_DVR_CUDOT_REGION_HACK_ALGORITHM, *LPHY_DVR_CUDOT_REGION_HACK_ALGORITHM;

typedef struct tagHY_DVR_CUDOT_EXIT_FLAG_PROTOCOL_EVN
{
	int bValid;
	int nExitFlagStatus; // 0：关闭出口标记 1：打开出口标记
	int nEnv;			 // 0:道路  1：岗亭 （nEnv为0的时候，nExitFlagStatus必须设置为0）,  -1表示无效值
	char szReserved[64];
} HY_DVR_CUDOT_EXIT_FLAG_PROTOCOL_EVN, *LPHY_DVR_CUDOT_EXIT_FLAG_PROTOCOL_EVN;

typedef struct tagHY_DVR_CUDOT_VIDEO_RESOLUTION_LEVEL
{
	int bValid;
	int nResolutionLevel; //清晰度级别，0-标清 1-高清 2-超高清
}HY_DVR_CUDOT_VIDEO_RESOLUTION_LEVEL, *LPHY_DVR_CUDOT_VIDEO_RESOLUTION_LEVEL;

typedef struct tagHY_DVR_CUDOT_OSD_LOGO_ENABLE
{
	int bValid;
	int nEnable;
}HY_DVR_CUDOT_OSD_LOGO_ENABLE, *LPHY_DVR_CUDOT_OSD_LOGO_ENABLE;


/*!
*  @struct	tagHY_DVR_CMDOT_MOTION_V3
*  @brief	移动侦测(坐标表示区域)
 */
typedef struct tagHY_DVR_CUDOT_MOTION_V3
{
	int bValid;																		/*!< 本结构体是否有效，0表示无效  */

	int bEnable;																	/*!< 是否处理移动侦测  */
	int nSenstive;																	/*!< 移动侦测灵敏度, 0 - 4,越高越灵敏，默认为3*/
	HY_DVR_REGION struMotionRegion[DVR_MAX_CUDOT_MOTION_REGION_NUM_V3];

} HY_DVR_CUDOT_MOTION_V3, *LPHY_DVR_CUDOT_MOTION_V3;

/*!
*  @struct	tagHY_DVR_MOTION_V3_LIST
*  @brief	移动侦测(坐标表示区域)
 */
typedef struct tagHY_DVR_CUDOT_MOTION_V3_LIST
{
	int bValid;																		/*!< 本结构体是否有效，0表示无效  */

	HY_DVR_CUDOT_MOTION_V3 struMotionV3[DVR_MAX_VIDEOIN_NUM];
} HY_DVR_CUDOT_MOTION_V3_LIST, *LPHY_DVR_CUDOT_MOTION_V3_LIST;

typedef struct tagHY_DVR_CUDOT_HACK_STAUTS
{
	int bValid;
	int nEnable;      // 告警入侵开关
}HY_DVR_CUDOT_HACK_STAUTS, *LPHY_DVR_CUDOT_HACK_STAUTS;

typedef struct tagHY_DVR_CUDOT_HACK_SENSITIVITY
{
	int bValid;
	int nSensitivity; // 告警入侵灵敏度
}HY_DVR_CUDOT_HACK_SENSITIVITY, *LPHY_DVR_CUDOT_HACK_SENSITIVITY;

/** 区域坐标信息，坐标取值范围统一为0~10000，设备做归一化处理。摄像头视频区域左上角坐标为（0,0），右下角坐标为（10000,10000）*/
typedef struct tagHY_DVR_CUDOT_REGION_ID
{
	int id;                       /**< 编号 */
	int x;                        /**< x坐标 */
	int y;                        /**< y坐标 */
}HY_DVR_CUDOT_REGION_ID;

/** 告警入侵区域以归一化的基准区域为参考，所选择的区域坐标信息列表 */
typedef struct tagHY_DVR_CUDOT_HACK_REGION_ID
{
	int num;                            /**< 区域坐标个数 */
	HY_DVR_CUDOT_REGION_ID *region_id;  /**< 坐标值 */
}HY_DVR_CUDOT_HACK_REGION_ID, *LPHY_DVR_CUDOT_HACK_REGION_ID;

typedef struct tagHY_DVR_CUDOT_FACE_DETECT_BASIC_INFO
{
	int bValid;
	int nDetectEnable;      // 人脸检测开关
	int nDetectDistance;    // 人脸检测瞳距
	int nExposureEnable;    // 人脸检测曝光
	int nLuminance;         // 人脸检测亮度

}HY_DVR_CUDOT_FACE_DETECT_BASIC_INFO, *LPHY_DVR_CUDOT_FACE_DETECT_BASIC_INFO;


/** 矩形区域坐标信息，坐标取值范围统一为0~10000，设备做归一化处理。摄像头视频区域左上角坐标为（0,0），右下角坐标为（10000,10000）*/
typedef struct tagHY_DVR_CUDOT_FACE_DETECT_REGION
{
	int nUpperLeftX;                       /**< 矩形左上角x坐标 */
	int nUpperLeftY;                       /**< 矩形左上角y坐标 */
	int nLowerRightX;                      /**< 矩形右下角x坐标 */
	int nLowerRightY;                      /**< 矩形右下角y坐标 */
}HY_DVR_CUDOT_FACE_DETECT_REGION;

/** 以归一化的基准区域为参考，所选择的矩形区域坐标信息列表 */
typedef struct tagHY_DVR_CUDOT_FACE_DETECT_REGION_LIST
{
	int nRegionsNums;                              /**< 人脸侦测区域个数 */
	HY_DVR_CUDOT_FACE_DETECT_REGION *pRegionList;  /**< 人脸侦测区域坐标信息列表，包含每个检测区域的坐标 */
}HY_DVR_CUDOT_FACE_DETECT_REGION_LIST, *LPHY_DVR_CUDOT_FACE_DETECT_REGION_LIST;

typedef struct tagHY_DVR_CUDOT_FACE_DETECT_PARAM
{
	int nSensitivity;                   /**< 人脸检测灵敏度，默认3，范围1~5 */
	int nMinWidth;                      /**< 人脸检测最小像素宽，默认为-1，表示无限制 */
	int nMinHeight;                     /**< 人脸检测最小像素高，默认为-1，表示无限制 */
	int nMaxWidth;                      /**< 人脸检测最大像素宽，默认为-1，表示无限制 */
	int nMaxHeight;                     /**< 人脸检测最大像素高，默认为-1，表示无限制 */
}HY_DVR_CUDOT_FACE_DETECT_PARAM, *LPHY_DVR_CUDOT_FACE_DETECT_PARAM;

typedef struct tagHY_DVR_CUDOT_SOUND_DETECT_STAUTS
{
	int bValid;
	int nEnable;      // 声音侦测开关
}HY_DVR_CUDOT_SOUND_DETECT_STAUTS, *LPHY_DVR_CUDOT_SOUND_DETECT_STAUTS;

typedef struct tagHY_DVR_CUDOT_SOUND_DETECT_SENSITIVITY
{
	int bValid;
	int nSensitivity; // 声音侦测灵敏度
}HY_DVR_CUDOT_SOUND_DETECT_SENSITIVITY, *LPHY_DVR_CUDOT_SOUND_DETECT_SENSITIVITY;

typedef struct tagHY_DVR_CUDOT_SPEAKER_INFO
{
	int bValid;
	int nVolume;		//0-100
	char cReserve[64];
}HY_DVR_CUDOT_SPEAKER_INFO, *LPHY_DVR_CUDOT_SPEAKER_INFO;


typedef struct tagHY_DVR_CUDOT_OSD_LOCATION
{
	int imageanchor_x;            ///< 店铺名称水印的位置锚点x坐标（x取值0-10000），此参数暂时不需要处理
	int imageanchor_y;            ///< 店铺名称水印的位置锚点y坐标（y取值0-10000），此参数暂时不需要处理
	int location_x;               ///< 店铺名称水印的位置左上角x坐标 (x取值0-10000）
	int location_y;               ///< 店铺名称水印的位置左上角y坐标 (x取值0-10000）
}HY_DVR_CUDOT_OSD_LOCATION;

typedef struct tagHY_DVR_CUDOT_COLOR
{
	int red;                      ///< RGB 红色，范围0~255
	int green;                    ///< RGB 绿色，范围0~255
	int blue;                     ///< RGB 蓝色，范围0~255
}HY_DVR_CUDOT_COLOR;

typedef struct tagHY_DVR_CUDOT_OSD_NAME_INFO
{
	char name[200];               //水印文字字符串，GBK编码
	int font_size;                //字体大小，范围 0~10000  【10000表示字符高度等于视频分辨率高度】
	int display;                  //是否显示，0：不显示  1：显示
	int id;                       //编号
	HY_DVR_CUDOT_COLOR color;     //水印颜色
	HY_DVR_CUDOT_OSD_LOCATION pos; //水印位置
}HY_DVR_CUDOT_OSD_NAME_INFO;

typedef struct tagHY_DVR_CUDOT_OSD_NAME_LIST
{
	int bValid;
	HY_DVR_CUDOT_OSD_NAME_INFO* osd_list;  //水印列表
	int count;                           // 水印列表条目个数
}HY_DVR_CUDOT_OSD_NAME_LIST, *LPHY_DVR_CUDOT_OSD_NAME_LIST;

typedef struct tagHY_DVR_CUDOT_MTU_INFO
{
	int bValid;
	int nMtuSize; // Mtu大小
}HY_DVR_CUDOT_MTU_INFO, *LPHY_DVR_CUDOT_MTU_INFO;

typedef struct tagHY_DVR_CUDOT_FACE_CAPTURE_MODE
{
	int bValid;
	int nFaceCaptureMode; //0:离开后抓拍(距离选优),1:快速抓拍,2:间隔抓拍(以秒为单位),3:间隔抓拍(以帧为单位),4:单人模式,5:离开后抓拍(质量选优),6:快速+离开后抓拍
}HY_DVR_CUDOT_FACE_CAPTURE_MODE, *LPHY_DVR_CUDOT_FACE_CAPTURE_MODE;

typedef struct tagHY_DVR_CUDOT_FACE_CAPTURE_QUALITY
{
	int bValid;
	int nFaceCaptureQuality; //人脸抓拍质量 范围：0~100
}HY_DVR_CUDOT_FACE_CAPTURE_QUALITY, *LPHY_DVR_CUDOT_FACE_CAPTURE_QUALITY;

typedef struct tagHY_DVR_CUDOT_PEOPLE_DRAW_BOX
{
	int bValid;
	int nPepoleDrawBox; //画人形侦测框使能
}HY_DVR_CUDOT_PEOPLE_DRAW_BOX, *LPHY_DVR_CUDOT_PEOPLE_DRAW_BOX;

typedef struct tagHY_DVR_CUDOT_VIDEO_ENCODE
{
	int bValid;
	int nVideoEncode; //码流编码参数 1-H264 2-H265
}HY_DVR_CUDOT_VIDEO_ENCODE, *LPHY_DVR_CUDOT_VIDEO_ENCODE;

typedef struct tagHY_DVR_CUDOT_TIMEOSD_STATUS
{
	int bValid;
	int nchannel;
	int nTimeOsdEnable; //时间osd开关 0-关 1-开
	char cReserve[64];
}HY_DVR_CUDOT_TIMEOSD_STATUS, *LPHY_DVR_CUDOT_TIMEOSD_STATUS;


typedef struct tagHY_DVR_CUDOT_SPEAKER_BUFFER_CLEAR
{
	int bValid;
	int nSpeakerBufferClear; //清空喇叭缓冲区
}HY_DVR_CUDOT_SPEAKER_BUFFER_CLEAR, *LPHY_DVR_CUDOT_SPEAKER_BUFFER_CLEAR;

typedef struct tagHY_DVR_CUDOT_DEVICE_STATUS
{
	int nValid;
	int nStatus;							//0,离线，1，在线，2，设备解绑,3.绑定成功，4，绑定失败
}HY_DVR_CUDOT_DEVICE_STATUS;

typedef struct tagHY_DVR_CUDOT_BIND_FAIL_INFO
{
	int nValid;
	int nCount;							// 绑定失败的次数
}HY_DVR_CUDOT_BIND_FAIL_INFO;

typedef struct tagHY_DVR_CUDOT_STORAGE_INFO
{
	int bValid;
	//设备存储类型，参考枚举DoT_StorageType_en
	int nStorageType;
	//存储设备状态，参考枚举DoT_StorageStatus_en
	int nStorageStatus;
	//存储路径
	char cStoragePath[256];
}HY_DVR_CUDOT_STORAGE_INFO;

typedef struct tagHY_DVR_CUDOT_BIND_STATUS
{
	int bValid;
	int nBindStatus;		//0,未绑定，1，绑定
	char cReserve[64];
}HY_DVR_CUDOT_BIND_STATUS, *LPHY_DVR_CUDOT_BIND_STATUS;

/** 设备绑定类型 */
typedef enum {
    CUDOT_BIND_MODE_AP      = 1,   /**< AP热点绑定模式 */
    CUDOT_BIND_MODE_WIRED   = 2,   /**< 有线绑定方式 */
    CUDOT_BIND_MODE_QRCODE  = 4,   /**< 二维码绑定方式 */
} HY_DVR_CUDOT_BIND_MODE_E;

/** 设备类型枚举定义 */
typedef enum {
    CUDOT_DEVICE_TYPE_IPC,                /**< 普通长上电IPC设备 */
    CUDOT_DEVICE_TYPE_NVR,                /**< NVR设备，暂不支持 */
    CUDOT_DEVICE_TYPE_LOWPOWER_DOORBELL,  /**< 低功耗门铃设备  */
    CUDOT_DEVICE_TYPE_LOWPOWER_IPC,       /**< 低功耗IPC设备 */
    CUDOT_DEVICE_TYPE_REPEATER_SINGLE,    /**< 中继，目前只支持一拖一 */
} HY_DVR_CUDOT_DEVICE_TYPE_E;

/** 日志级别 */
typedef enum {
    CUDOT_LOG_LEVEL_DEBUG = 1,			
    CUDOT_LOG_LEVEL_INFO,	
    CUDOT_LOG_LEVEL_WARNNING,				
    CUDOT_LOG_LEVEL_ERROR,
    CUDOT_LOG_LEVEL_FATAL,
    CUDOT_LOG_LEVEL_NONE = 0xff,
} HY_DVR_CUDOT_LOG_LEVEL_E;

typedef enum {
    CUDOT_RESOLUTION_NONE = 0,
    CUDOT_RESOLUTION_360P = 0x00000001,
    CUDOT_RESOLUTION_720P = 0x00000002,
    CUDOT_RESOLUTION_1080P = 0x00000004,
    CUDOT_RESOLUTION_300W  = 0x00000008,  //300w 以上分辨率不定，统一采用像素方式表示
    CUDOT_RESOLUTION_400W  = 0x00000010,
    CUDOT_RESOLUTION_500W  = 0x00000020,
} HY_DVR_CUDOT_RESOLUTION_E;

/** 存储设备状态 */
typedef enum {
    CUDOT_STORAGE_STATUS_READY,                 /**< 设备外部存储(如TFCard)插入，且状态正常 */
    CUDOT_STORAGE_STATUS_NOT_READY,             /**< 设备无外部存储   */
    CUDOT_STORAGE_STATUS_ABNORMAL,              /**< 设备外部存储异常 */
    CUDOT_STORAGE_FILE_SYSTEM_NOT_SUPPORT,      /**< 不支持的文件系统 */
} HY_DVR_CUDOT_STORAGE_STATUS_E;

/** 存储设备类型 */
typedef enum {
    CUDOT_STORAGE_TYPE_TFCARD,
} HY_DVR_CUDOT_STORAGE_TYPE_E;

/** 设备硬件能力集属性(中国联通) */
typedef struct {
    bool hw_support_wifi;             /**< 硬件支持WIFI */
    bool hw_support_tfcard;           /**< 硬件支持TF卡 */
    bool hw_support_speaker;          /**< 硬件支持扬声器 */
    bool hw_support_pir;              /**< 硬件支持PIR侦测 */
    bool hw_support_ptz_pan;          /**< 硬件支持PTZ左右转动 */
    bool hw_support_ptz_tilt;         /**< 硬件支持PTZ上下转动 */
    bool hw_support_microphone;       /**< 硬件支持麦克风采集声音 */
    bool hw_support_battery;          /**< 硬件支持电池供电 */
    bool hw_support_buzzer;           /**< 硬件支持蜂鸣器 */
    bool hw_support_led_indicate;     /**< 硬件支持状态指示灯 */
    bool hw_support_night_vision;     /**< 硬件支持夜视模式 */
    bool hw_support_tamer_alarm;      /**< 硬件支持防拆报警 */
    bool hw_support_light;            /**< 硬件支持白光灯控制 */
    bool hw_support_ring;             /**< 硬件支持门铃按键 */
    bool hw_support_binocular;        /**< 双目摄像头 */
    bool hw_support_manual_focus;     /**< 手动变焦 */
    bool hw_support_manual_zoom;      /**< 手动变倍 */
    bool hw_support_focus_value;      /**< 支持设置变焦值 ，对应回调 DOT_CMD_SET_OPTICAL_FOCUS*/
    bool hw_support_zoom_value;       /**< 支持设置变倍值，对应回调 DOT_CMD_SET_OPTICAL_ZOOM ,  */
    bool hw_support_call_event;       /**< 支持一键呼叫 */
    bool hw_cap_reserve[57];          /**< 预留字段 */
}HY_DVR_DEVICE_CUDOT_HW_CAPABILITY;

/** 设备软件能力集属性(中国联通) */
typedef struct {
    bool sw_support_rotate;                     /**< 支持画面180度翻转  */
    bool sw_support_bi_directional_audiotalk;   /**< 支持双向语音对讲  */
    bool sw_support_motion_detect;              /**< 支持运动检测报警 */
    bool sw_support_motion_detect_region;       /**< 支持运动检测区域 */
    bool sw_support_motion_track;               /**< 支持运动跟踪 */
    bool sw_support_motion_detect_sesitivity;   /**< 支持运动检测灵敏度设置 */
    bool sw_support_sound_detect;               /**< 支持声音检测报警 */
    bool sw_support_sound_detect_sensitivity;   /**< 支持声音检测灵敏度设置 */
    bool sw_support_sound_track;                /**< 支持声音跟踪 */
    bool sw_support_people_detect;              /**< 支持人形检测报警 */
    bool sw_support_people_detect_sensitivity;  /**< 支持人形检测灵敏度设置 */
    bool sw_support_people_draw_box;            /**< 支持人形画框功能 */
    bool sw_support_speaker_volume;             /**< 支持扬声器音量调节 */
    bool sw_support_doorbell_volume;            /**< 支持门铃音量调节 */
    bool sw_support_video_resolution_modify;    /**< 支持视频分辨率修改 */
    bool sw_support_pir_sensitivity;            /**< 支持PIR灵敏度设置 */
    bool sw_support_antiflicker;                /**< 支持防闪烁设置（50HZ/60HZ） */
    bool sw_support_face_detect;                /**< 支持人脸侦测报警 */
    bool sw_support_face_detect_distance;       /**< 支持人脸检测瞳距设置 */
    bool sw_support_face_detect_region;         /**< 支持人脸检测区域设置 */
    bool sw_support_face_detect_exposure;       /**< 支持人脸曝光开关 */
    bool sw_support_face_detect_luminance;      /**< 支持人脸亮度设置 */
    bool sw_support_face_detect_param;          /**< 支持人脸检测参数 */
    bool sw_support_face_capture_mode;          /**< 支持人脸抓拍模式设置，和回调类型 DOT_CMD_SET_FACE_CAPTURE_MODE 对应 */
    bool sw_support_face_capture_quality;       /**< 支持人脸抓拍质量设置，和回调类型 DOT_CMD_SET_FACE_CAPTURE_QUALITY 对应 */
    bool sw_support_babycry_detect;             /**< 支持哭声检测 */
    bool sw_support_ptz_preset_position;        /**< 支持PTZ预置点设置 */
    bool sw_support_defense_alarm;              /**< 支持防御报警设置（声光报警） */
    bool sw_support_close_defense_alarm;        /**< 支持关闭防御报警 */
    bool sw_support_alarm_volume;               /**< 支持报警音量调节 */
    bool sw_support_full_color;                 /**< 支持全彩设置 */
    bool sw_support_remote_wakeup_switch;       /**< 支持远程唤醒 */
    bool sw_support_super_hd_video;             /**< 支持超清视频（超清、高清、标清切换）， 如果支持此能力则将sw_support_video_resolution_modify 能力置为false*/
    bool sw_support_event_thumbnail;            /**< 支持事件缩略图 */
    bool sw_support_pir_stay_time;              /**< 支持pir逗留时间配置 */

    bool sw_support_image_brightness;           /**< 支持图像亮度设置 */
    bool sw_support_image_contrast;             /**< 支持图像对比度设置 */
    bool sw_support_image_scenario;             /**< 支持图像场景设置 */
    bool sw_support_face_anglefilter;           /**< 支持人脸角度过滤设置 */
    bool sw_support_face_frame;                 /**< 支持人脸画框 */
    bool sw_support_thirdparty_message;        /**<  支持第三方平台透传的信令消息 */
    bool sw_support_audio_broadcast;           /**< 支持云广播功能 */

	bool sw_support_snapshot;					/**< 支持获取截图 */
    bool sw_support_ai_detect_region;			/**< 支持AI 动检区域，对应回调 DOT_ALARM_AI_DETECT_REGION，区域内检测到motion需上报 DOT_ALARM_AI_DETECT_ALARM */
    bool sw_support_babycry_detect_sensitivity;	/**< 支持哭声检测灵敏度，和回调类型 DOT_CMD_SET_BABYCRY_DETECT_SENSITIVITY对应 */
    bool sw_support_face_recognize;				/**< 支持人脸识别功能 */
	bool sw_support_license_plate_recognition;	/**< 支持车牌识别功能 */
	bool sw_support_motor_vehicle_detect;		/**< 支持车辆检测功能 */
	bool sw_support_exit_flag;					/**< 支持出口标识功能 */
	bool sw_support_algorithm_control;			/**< 支持算法切换功能 */
} HY_DVR_DEVICE_CUDOT_SW_CAPABILITY;

/** 设备软件能力集属性(中国联通) */
typedef struct {
    bool sw_support_time_osd;                   /**<支持时间水印能力*/
    bool sw_support_video_encode_modify;        /**< 支持修改视频编码格式[h264/h265] */
    bool sw_support_wifi_signal;                 /**< 支持获取wifi信号强度*/
    bool sw_support_osd_name_bitmap;             /**< 支持OSD 文字水印点阵叠加方式 */
    bool sw_support_rotate_diff_angle;          /**< 支持画面90/180/270度翻转，支持此能力集则不需要上报sw_support_rotate能力集，目前仅用于特殊场景设备，一般设备不需要上报此能力*/
    bool sw_support_face_similarity;            /**<  支持人脸相似度设置， 和回调类型DOT_CMD_SET_DEVICE_FACE_PARAM 对应 */
    bool sw_support_face_vague;                 /**<  支持人脸模糊度设置， 和回调类型DOT_CMD_SET_DEVICE_FACE_PARAM 对应 */    
    bool sw_cap_reserve[58];                    /**< 预留字段 */
}HY_DVR_DEVICE_CUDOT_SW_CAPABILITY_EX;


//OVDSDK,AI
#define HY_DVR_OVDSDK_MAX_FACE_NUM (4)

//!\brief 图像数据
typedef struct tagHY_DVR_OVDSDK_1400_IMAGE_INFO
{
	int nValid;

	char szImageID[64];		//R/O,图像标识
	int nEventSort;			//R/O,事件分类，10，人脸检测，11，人脸比对，12，车辆检测，13，车辆比对，16，车牌识别, 101:高空抛物
	char szDeviceID[32];	//O,设备编码
	char szStoragePath[256];//O,图像文件的存储路径，才有URI命名规则
	char szType[16];		//R，1，车辆大图，2.车牌彩色小图，11，人脸图，12，非机动车图,14.背景图, 101:高空抛物
	char szFileFormat[16];  //R,图片格式，Bmp,Gif,jpeg,Png
	char szShotTime[32];	//R.拍摄时间，时间格式yyyyMMddHHmmss
	char *pData;			//R.图片二进制数据
	int nDataLength;		//R.图片二进制数据长度
	int nWidth;				//R.宽度，单位是像素
	int nHeight;			//R.高度，单位是像素

	char szReserved[256];
}HY_DVR_OVDSDK_1400_IMAGE_INFO;

//OVDSDK时间段
typedef struct tagHY_DVR_OVDSDK_ALERTTIME
{
	int bValid; //1为有效，0为无效
	int nStartTime;	//必需
	int nEndTime;	//必需
} HY_DVR_CMCC_OVD_ALERTTIME;

//离岗检测事件 图片数据紧接在后面
typedef struct tagHY_DVR_OVDSDK_OFF_DUTY_ALARM_V2
{
    int nValid;

	char szOnLeaveID[64];		//事件ID
	int nInfoKind;				//R,信息分类，0，其他，1.自动采集，2，人工采集
	char szSourceID[64];		//R.来源图像信息标志
	char szDeviceID[32];		//R/O,设备编码
	char szAppearTime[32]; 		//离岗事件发生时间，时间格式yyyyMMddHHmmss
	int nEventType;				//事件类型
	int bOnDutyCountValid;		//在岗人数是否有效
	int nOnDutyCount;			//在岗人数 ------(可选,参照bOnDutyCountValid)
	int bOffDutyDurationValid;	//离岗时长是否有效
	int nOffDutyDuration;		//离岗时长 ------(可选,参照bOffDutyDurationValid)

	HY_DVR_CMCC_OVD_ALERTTIME stAlertTime;	//检测时间段 ------(可选)
	HY_DVR_CMCC_OVD_ALARM_ZONE stAlarmZone;	//检测区域 ------(可选)
	HY_DVR_OVDSDK_1400_IMAGE_INFO stImageInfo;	//图片信息

	char szReserved[1024];
} HY_DVR_OVDSDK_OFF_DUTY_ALARM_V2;

typedef struct tagHY_DVR_OVDSDK_OFF_DUTY_ALERT_TIME
{
	char szStartTime[32];
	char szEndTime[32];
}HY_DVR_OVDSDK_OFF_DUTY_ALERT_TIME;

typedef struct tagHY_DVR_OVDSDK_OFF_DUTY_ALERM_ZONE_POINT
{
	int nValid;

	int nPointId;
	int nX;
	int nY;
}HY_DVR_OVDSDK_OFF_DUTY_ALERM_ZONE_POINT;

#define MAX_OFF_DUTY_ALERM_ZONE_POINT	(10)
typedef struct tagHY_DVR_OVDSDK_OFF_DUTY_ALERM_ZONE
{
	int nValid;
	
	int nAreaId;
	int nPoints;
	HY_DVR_OVDSDK_OFF_DUTY_ALERM_ZONE_POINT stAreaPoints[MAX_OFF_DUTY_ALERM_ZONE_POINT];
}HY_DVR_OVDSDK_OFF_DUTY_ALERM_ZONE;

#define HY_DVR_OVDSDK_MAX_IMAGE_NUM 4
#define MAX_OFF_DUTY_ALERM_ZONE			(8)
typedef struct tagHY_DVR_OVDSDK_OFF_DUTY_INFO
{
	int nValid;

	char szOnLeaveID[64];			//R,在离岗标记
	int nInfoKind;				//R,信息分类，0，其他，1.自动采集，2，人工采集
	char szSourceID[64];		//R.来源图像信息标志
	char szDeviceID[32];		//R/O,设备编码
	char szAppearTime[32];   //R,事件产生时间，时间格式yyyyMMddHHmmss
	int nEventType;				//R/O,事件类型
	int nOnDutyCount;				//R/O,在岗人数
	int nOffDutyDuration;			//R/O,离岗时间
	HY_DVR_OVDSDK_OFF_DUTY_ALERT_TIME stAlertTime;			//R/O,检测时间段
	HY_DVR_OVDSDK_OFF_DUTY_ALERM_ZONE stAlermZone[MAX_OFF_DUTY_ALERM_ZONE];
	char szReserved[256];

	HY_DVR_OVDSDK_1400_IMAGE_INFO stImageInfo[HY_DVR_OVDSDK_MAX_IMAGE_NUM];//图片信息
}HY_DVR_OVDSDK_OFF_DUTY_INFO;

#define HY_DVR_OVDSDK_MAX_OFF_DUTY_NUM 8
typedef struct tagHY_DVR_OVDSDK_OFF_DUTY_INFO_LIST
{
	int nValid;
	int nStatisticsNum; //统计次数
	HY_DVR_OVDSDK_OFF_DUTY_INFO stOffDutyInfo[HY_DVR_OVDSDK_MAX_OFF_DUTY_NUM];
	char szReserved[128];
}HY_DVR_OVDSDK_OFF_DUTY_INFO_LIST;

//\brief 人脸列表
typedef struct tagHY_DVR_OVDSDK_FACE_INFO_LIST
{
	int nValid;

	char szFaceID[64];			//R,人脸标记
	int nInfoKind;				//R,信息分类，0，其他，1.自动采集，2，人工采集
	char szSourceID[64];		//R.来源图像信息标志
	char szDeviceID[32];		//R/O,设备编码
	int nLeftTopX;				//R/O,左上角X坐标
	int nLeftTopY;				//R/O,左上角Y坐标
	int nRightBottomX;			//R/O,右下角X坐标
	int nRightBottomY;			//R/O,右下角Y坐标
	char szLocationMarkTime[32]; //O,位置标记时间，时间格式yyyyMMddHHmmss
	char szFaceAppearTime[32];   //O,人脸出现时间，时间格式yyyyMMddHHmmss
	char szFaceDisAppearTime[32];   //O,人脸消失时间，时间格式yyyyMMddHHmmss
	int nIsSuspectedTerrorist;	 //R,是否涉恐人员,0,否，1，是，2，不确定
	int nIsCriminalInvolved;	//R,是否涉案人员，0,否，1，是，2，不确定
	int nIsDetainees;//R,是否在押人员，0,否，1，是，2，不确定
	int nIsVictim;//R,是否被害人员，0,否，1，是，2，不确定
	int nIsSuspiciousPerson;//R,是否可疑人员，0,否，1，是，2，不确定
	char acMaskColor[32];//口罩颜色，空值表示未做检测，0表示没有口罩,1，黑，2，白，3，灰，4，红，5，蓝，6，黄，9，绿等，详细请参考1400文档ColorType类型定义
	char szReserved[224];

	HY_DVR_OVDSDK_1400_IMAGE_INFO stFaceInfo[HY_DVR_OVDSDK_MAX_FACE_NUM];//人脸信息
}HY_DVR_OVDSDK_FACE_INFO_LIST;


#define HY_DVR_OVDSDK_1400_MAX_PLATE_PICTURE_NUM  10
typedef struct  tagHY_DVR_OVDSDK_VEHICLE_INFO
{
	int nValid;
	char szMotorVehicleID[64]; //R,车辆标识
	int nInfoKind;   //R,0:其它  1:自动采集  2:人工采集
	char szSourceID[64];//R,来源图像标识
	char szTollgateID[32];//O,关联卡口编号
	char szDeviceID[32];  //R/O,设备编码
	char StorageUrl1[256];//R,近景照片，卡口相机所拍照片，自动采集必选，图像访问路径，采用URI命名规则
	char StorageUrl3[256];//O,远景照片
	char StorageUrl5[256];//O,缩略图
	int nLeftTopX;//R/O,左上角X坐标
	int nLeftTopY;//R/O,左上角Y坐标
	int nRightBottomX;//R/O,右下角X坐标
	int nRightBottomY;//R/O,右下角Y坐标
	char szMarkTime[32];//O,位置标记时间，时间格式yyyyMMddHHmmss
	char szAppearTime[32];//O,车辆出现时间，时间格式yyyyMMddHHmmss
	int nLaneNo;	  //O,车道号，车辆形式方向最左车道为1，由左向右顺序编号
	char szHasPlate[32];   //R/O,有无车牌  0,没有车牌，1，有车牌
	char szPlateClass[32];  //R/O,号牌种类，01,大型汽车号牌，02，小型汽车号牌等，请参考1400文档PlateClassType类型定义
	char szPlateColor[32];  //R/O,车牌颜色，1，黑，2，白，3，灰，4，红，5，蓝，6，黄，9，绿等，详细请参考1400文档ColorType类型定义
	char szPlateNo[32];  //R/O,车牌号，各类机动车号牌编号车牌全部无法识别的以“无车牌”标识，部分未识别的每个字符以半角“-”代替
	char szPlateNoAttach[32];//O,挂车牌号
	char szPlateDescribe[64];//O,车牌描述，车牌框广告信息，包括车行名称，联系电话等
	char szIsDecked[16];	//O,是否套牌,0,不是，1，是
	char szIsAltered[16]; //O,是否涂改,0,不是，1，是
	char nIsCovered[16]; //O,是否遮挡,0,不是，1，是
	int nSpeed; //O,行驶速度，单位千米每小时（km/h）
	char szDirection[16];//O,1,东，2，西，3，南，4，北，5，东北，6，西南，7，东南，8，西北，9，其他
	char szDrivingStatusCode[32];//O,行驶状态代码，参考1400文档的DrivingStatusCodeType字段
	char szUsingPropertiesCode[32];//O,车辆使用性质代码，参考1400文档的UsingPropertiesCodeType字段
	char szVehicleClass[32];//O,车辆类型，参考1400文档的VehicleClassType定义
	char szVehicleBrand[32]; //O,车辆品牌，0，其他，1，大众，2，别克，3，宝马等，参考1400文档的VehicleBrandType的定义
	char szVehicleModel[32]; //O,车辆型号，参考1400文档的VehicleModelType定义
	char szVehicleStyles[16];//O,车辆年款
	int nVehicleLength;		//O,车辆长度，单位是毫米
	int nVehicleWidth;		//O,车辆官渡，单位是毫米
	int nVehicleHeight;		//O,车辆高度，单位是毫米
	char szVehicleColor[32];//R,车身颜色
	char szVehicleColorDepth[32];//O,颜色深浅，0，深，1，浅
	char szVehicleHood[64]; //O,对车前盖的描述
	char szVehicleTrunk[64]; //O,对车后盖的描述
	char szVehicleWheel[64]; //O,对车轮的描述
	char szWheelPrintedPattern[64]; //O,车轮印华为，参考GA240.43文档定义
	char szVehicleWindow[64];//O,对车窗的描述
	char szVehicleRoof[64];//O,对车顶的描述
	char szVehicleDoor[64];//O,对车门的描述
	char szSideOfVehicle[64];//O,对车侧面的描述，不包括门
	char szCarOfVehicle[64];//O,对车厢的描述
	char szRearviewMirror[64];//O,对后视镜的描述
	char szVehicleChassis[64];//O,对底盘的描述
	char szVehicleShielding[64];//O,对车遮挡物的描述
    char szFilmColor[32];//O,贴膜颜色，1,深色，2，浅色，3，无
	char szIsModified[32];//O,改装标志，0，改装，1，不改装
	char szHitMarkInfo[32];//O,撞痕信息，0，没有撞痕，1，少量撞痕，2，大量撞痕
	char szVehicleBodyDesc[128];//O,描述车身上的文字信息，或者车上载物信息
	char szVehicleFrontItem[32];//O,车前部物品，当有多个时，可用英文半角逗号分隔
	char szDescOfFrontItem[256];//O,对车前部物品数量，颜色，种类等信息的描述
	char szVehicleRearItem[32];//O,车后不物品，当有多个时，可用英文半角逗号分隔
	char szDescOfRearItem[256];//O,对车后部物品数量，颜色，种类等信息的描述
	int nNumOfPassenger;	//O,车辆内人员数量
	char szPassTime[32];    //O,经过时刻，时间格式yyyyMMddHHmmss
	char szNameOfPassedRoad[64];    //O,经过道路名称
	char szIsSuspicious[32];	//O,是否可以车
	char szPlateReliability[4];//O,整个号牌号码的识别可信度，以0-100数值表示百分比，数值越大可信度越高
	char szPlateCharReliability[64];//O,每位号牌号码可信度，号牌号码的识别可信度，以 0～100 数值表示百分比，数值越大可信度越高。按“字符 1-可信度 1，字符 2-可信度 2”方式排列，中间为英文半角连接线、逗号；例如识别号牌号码为：苏 B12345， 则取值为： ”苏-80， B-90，1-90， 2-88， 3-90， 4-67， 5-87”
	char szBrandReliability[4];//O,品牌标志识别可信度，车辆品牌标志可信度； 以 0～100 之间数值表示百分比，数值越大可信度越高
	HY_DVR_OVDSDK_1400_IMAGE_INFO stPlateSnapInfoList[HY_DVR_OVDSDK_1400_MAX_PLATE_PICTURE_NUM];//车辆图片信息列表
	char szReserved[256];
}HY_DVR_OVDSDK_VEHICLE_INFO;


//\brief 车辆列表
#define HY_DVR_OVDSDK_1400_MAX_VEHICLE_PICTURE_NUM  16
typedef struct tagHY_DVR_OVDSDK_VEHICLE_INFO_LIST
{
	int nValid;
	int nVehicleNum;                   	//车辆实际数量
	HY_DVR_OVDSDK_VEHICLE_INFO stVehicleInfoList[HY_DVR_OVDSDK_1400_MAX_VEHICLE_PICTURE_NUM]; //车牌信息
	char szReserved[128];
}HY_DVR_OVDSDK_VEHICLE_INFO_LIST;


typedef struct  tagHY_DVR_OVDSDK_NONMOTOR_VEHICLE_INFO
{
	int nValid;
	char szNonMotorVehicleID[64]; //R,车辆标识
	int nInfoKind;   //R,0:其它  1:自动采集  2:人工采集
	char szSourceID[64];//R,来源图像标识
	char szDeviceID[32];  //R/O,设备编码
	int nLeftTopX;//R/O,左上角X坐标
	int nLeftTopY;//R/O,左上角Y坐标
	int nRightBottomX;//R/O,右下角X坐标
	int nRightBottomY;//R/O,右下角Y坐标
	char szMarkTime[32];//O,位置标记时间，时间格式yyyyMMddHHmmss
	char szAppearTime[32];//O,车辆出现时间，时间格式yyyyMMddHHmmss
	char szHasPlate[32];   //R/O,有无车牌  0,没有车牌，1，有车牌
	char szPlateClass[32];  //R/O,号牌种类，01,大型汽车号牌，02，小型汽车号牌等，请参考1400文档PlateClassType类型定义
	char szPlateColor[32];  //R/O,车牌颜色，1，黑，2，白，3，灰，4，红，5，蓝，6，黄，9，绿等，详细请参考1400文档ColorType类型定义
	char szPlateNo[32];  //R/O,车牌号，各类机动车号牌编号车牌全部无法识别的以“无车牌”标识，部分未识别的每个字符以半角“-”代替
	char szVehicleColor[32];//R,车身颜色
	HY_DVR_OVDSDK_1400_IMAGE_INFO stPlateSnapInfoList[HY_DVR_OVDSDK_1400_MAX_PLATE_PICTURE_NUM];//车辆图片信息列表
	char szReserved[256];
}HY_DVR_OVDSDK_NONMOTOR_VEHICLE_INFO;

//\brief 车辆列表
#define HY_DVR_OVDSDK_1400_MAX_NONMOTOR_VEHICLE_PICTURE_NUM  64
typedef struct tagHY_DVR_OVDSDK_NONMOTOR_VEHICLE_INFO_LIST
{
	int nValid;
	int nVehicleNum;                   	//车辆实际数量
	HY_DVR_OVDSDK_NONMOTOR_VEHICLE_INFO stNonMotorVehicleInfoList[HY_DVR_OVDSDK_1400_MAX_NONMOTOR_VEHICLE_PICTURE_NUM]; //车牌信息
	char szReserved[128];
}HY_DVR_OVDSDK_NONMOTOR_VEHICLE_INFO_LIST;

typedef struct tagHY_DVR_OVDSDK_PASSENGER_FLOW_INFO
{
	int nValid;

	char szTrafficID[64];			//R,客流标记
	int nInfoKind;				//R,信息分类，0，其他，1.自动采集，2，人工采集
	char szSourceID[64];		//R.来源图像信息标志
	char szDeviceID[32];		//R/O,设备编码
	int nLeftTopX;				//R/O,左上角X坐标
	int nLeftTopY;				//R/O,左上角Y坐标
	int nRightBottomX;			//R/O,右下角X坐标
	int nRightBottomY;			//R/O,右下角Y坐标
	char szBeginTime[32]; //R,客流统计开始时间，时间格式yyyyMMddHHmmss
	char szEndTime[32];   //R,客流统计结束时间，时间格式yyyyMMddHHmmss
	char szAppearTime[32];   //R,统计产生时间，时间格式yyyyMMddHHmmss
	int nOutCount; //进出统计人数
	int nInCount;  //进入统计人数
	char szReserved[256];

	HY_DVR_OVDSDK_1400_IMAGE_INFO stImageInfo[HY_DVR_OVDSDK_MAX_IMAGE_NUM];//图片信息
}HY_DVR_OVDSDK_PASSENGER_FLOW_INFO;

#define HY_DVR_OVDSDK_MAX_PASSENGER_FLOW_NUM 8
typedef struct tagHY_DVR_OVDSDK_PASSENGER_FLOW_INFO_LIST
{
	int nValid;
	int nStatisticsNum; //统计次数
	HY_DVR_OVDSDK_PASSENGER_FLOW_INFO stPassengerFlowInfo[HY_DVR_OVDSDK_MAX_PASSENGER_FLOW_NUM];
	char szReserved[128];
}HY_DVR_OVDSDK_PASSENGER_FLOW_INFO_LIST;

//区域人数统计上传
#define HY_DVR_OVDSDK_MAX_REGIONAL_PEOPLE_NUM 8

typedef struct tagHY_DVR_OVDSDK_REGIONAL_PEOPLE_INFO
{
	int nValid;

	char szRegionID[64];			//R,客流标记
	int nInfoKind;				//R,信息分类，0，其他，1.自动采集，2，人工采集
	char szSourceID[64];		//R.来源图像信息标志
	char szDeviceID[32];		//R/O,设备编码
	int nLeftTopX;				//R/O,左上角X坐标
	int nLeftTopY;				//R/O,左上角Y坐标
	int nRightBottomX;			//R/O,右下角X坐标
	int nRightBottomY;			//R/O,右下角Y坐标
	char szAppearTime[32];   //R,统计上传时间，时间格式yyyyMMddHHmmss
	int nPeopleCount; //人数
	int nCordon;  //警戒值
	char szReserved[256];

	HY_DVR_OVDSDK_1400_IMAGE_INFO stImageInfo[HY_DVR_OVDSDK_MAX_IMAGE_NUM];//图片信息
}HY_DVR_OVDSDK_REGIONAL_PEOPLE_INFO;

typedef struct tagHY_DVR_OVDSDK_REGIONAL_PEOPLE_INFO_LIST
{
	int nValid;
	int nStatisticsNum; //统计次数
	HY_DVR_OVDSDK_REGIONAL_PEOPLE_INFO stRegionalPeopleInfo[HY_DVR_OVDSDK_MAX_REGIONAL_PEOPLE_NUM];
	char szReserved[128];
}HY_DVR_OVDSDK_REGIONAL_PEOPLE_INFO_LIST;

typedef struct tagHY_DVR_SLEEP_END_TIME_CFG
{
    int iValid;								/*!< 本结构体是否有效，0表示无效  */
    int iLhour;								/*!< 距离睡眠结束的时间 0-23代表具体时间点  */
    int iLweekday;							/*!< 距离睡眠结束的时间 0-6代表星期一到星期日  */

	char szReserved[64];
}HY_DVR_SLEEP_END_TIME_CFG, *LPHY_DVR_SLEEP_END_TIME_CFG;

typedef struct tagHY_DVR_DEVICE_SLEEP_INFO_CFG
{
    int iValid;								/*!< 本结构体是否有效，0表示无效  */
    int nSleepStatus;						/*!< 0，休眠，1，唤醒  */
    int bEnable;							/*!< 定时睡眠开关  */
	HY_DVR_SLEEP_END_TIME_CFG stSleepEndTime;	/*!< 下次睡眠结束时间  */
    HY_DVR_SCHEDTIME struSchedTime;			/*!< 布防计划  */

	char szReserved[128];
}HY_DVR_DEVICE_SLEEP_INFO_CFG, *LPHY_DVR_DEVICE_SLEEP_INFO_CFG;


typedef struct tagHY_DVR_DEVICE_SLEEP_CONTROL
{
    int iValid;                                /*!< 本结构体是否有效，0表示无效  */
    int nSleepControl;                        /*!< 0，立即休眠，1，立即唤醒  */

	char szReserved[64];
}HY_DVR_DEVICE_SLEEP_CONTROL, *LPHY_DVR_DEVICE_SLEEP_CONTROL;


#define DVR_MAX_AIR_VALID_DAYS    (2)
#define DVR_MAX_AIR_TIMESEGMENT    (24)

typedef enum {
    Air_QUALITY_CLEAN  = 0,		/*!< 洁净     */
    Air_QUALITY_MILD   = 1,		/*!< 轻度污染  */
    Air_QUALITY_MIDDLE = 2,		/*!< 中度污染  */
    Air_QUALITY_HIGH  = 3,		/*!< 重度污染  */
}eAirQualityLevel;

typedef struct tagHY_DVR_BAD_AIR_QUERY
{
	int bValid;													/*!< 本结构体是否有效，0表示无效 */
	unsigned char byGetType[DVR_MAX_AIR_VALID_DAYS][DVR_MAX_AIR_TIMESEGMENT];	/*!< 0为洁净，1为轻度污染, 2为中度污染，3为重度污染 */

	char szReserve[128];     //保留字段
} HY_DVR_BAD_AIR_QUERY, *LPHY_DVR_BAD_AIR_QUERY;

typedef struct tagHY_DVR_AIR_QUALITY_INFO_CFG
{
	int bValid;								/*!< 本结构体是否有效，0表示无效  */
	int bEnable;							/*!< 气体检测开关  */
	int bPushInterval;						/*!< 推送时间间隔，单位：小时  */
	eAirQualityLevel currentAirQualityLevel;/*!< 当前空气质量  */
	eAirQualityLevel alarmLevel;			/*!< 报警阈值等级  */
	HY_DVR_HANDLEEXCEPTION struHandleType;	/*!< 处理方式  */
	HY_DVR_SCHEDTIME struSchedTime;			/*!< 检测时间段设置  */
	HY_DVR_BAD_AIR_QUERY struGetAirValue;	/*!< 毒害气体浓度查询  */

	char szReserve[256];     //保留字段
}HY_DVR_AIR_QUALITY_INFO_CFG, *LPHY_DVR_AIR_QUALITY_INFO_CFG;

/*!
*  @struct    tagHY_DVR_AIR_DECTECT_AUDIO_LIGHT_ALARM_CFG
*  @brief      气体检测声光报警配置
*/
typedef struct tagHY_DVR_AIR_DECTECT_AUDIO_LIGHT_ALARM_CFG
{
    int iValid;

    int iAudioEnable;        /* 声音报警使能：0,false, 1,true */
    int iAudioType;          /* 声音报警类型：0:自定义,1:警报,2:犬吠,3:警戒区域,4:监控区域,5:欢迎光临,6:深水警告,7:私人领地,
                                        8:危险区域,9:贵重物品,10:高处危险,11:私家价位,12:叮咚,13:有口罩,14:无口罩,
                                        15:注意私人物品,16:抓小偷,17:救命,18:分类,19:可回收垃圾,20:家庭食物垃圾        
                                        21:危险废物,22:残余垃圾,23:全部分类,24:车形自定义,25:禁止停车*/
    int iLightEnable;        /* 声光报警开关：0,false, 1,true */
    int iAlarmTime;          /* 报警时长：10，30，60，120;单位秒 */

    char szReserved[256];
}HY_DVR_AIR_DECTECT_AUDIO_LIGHT_ALARM_CFG, *LPHY_DVR_AIR_DECTECT_AUDIO_LIGHT_ALARM_CFG;

typedef struct tagHY_DVR_NIGHT_LED_INFO_CFG
{
	int iValid;								 /*!< 本结构体是否有效，0表示无效  */
	int bDuration;							/*!< 亮灯持续时长  */
	int bEnable;							      /*!< 0表示只设置配置，1表示用户在预览界面开启夜灯  */

	char szReserve[128];     //保留字段
}HY_DVR_NIGHT_LED_INFO_CFG, *LPHY_DVR_NIGHT_LED_INFO_CFG;

typedef struct tagHY_DVR_4G_CARD_SELECT_CFG_INFO
{
    int nValid;
    char cICCID[64];        //
    int nAutoSwitchCard;    //0,不自动切换，1，自动切换
	int nCardID;			//卡序号，nCardID=0表示卡0，nCardID=1表示卡1, nCardID=2表示卡2，依此类推；同时为兼容/Network/4GCardCfgList接口CardType字段，约定 卡0表示内置卡，卡1表示外置卡

    char cReserve[256 - 4];     //保留字段
}HY_DVR_4G_CARD_SELECT_CFG_INFO, *LPHY_DVR_4G_CARD_SELECT_CFG_INFO;

typedef struct tagHY_DVR_4G_CARD_SELECT_RESULT_INFO
{
    int nValid;

	char cICCID[64];        // ICCID
	int nCardID;          	// 卡序号，nCardID=0表示卡0，nCardID=1表示卡1, nCardID=2表示卡2，依此类推；同时为兼容/Network/4GCardCfgList接口CardType字段，约定 卡0表示内置卡，卡1表示外置卡
	int iSelectErrCode;     // 4G卡切换结果；0-成功，非0值表示切换失败
 
    char cReserve[128];     //保留字段
}HY_DVR_4G_CARD_SELECT_RESULT_INFO, *LPHY_DVR_4G_CARD_SELECT_RESULT_INFO;

typedef struct tagHY_DVR_4G_CARD_CFG_INFO
{
    int nValid;
    int nCardType;          //卡类型，0，内置卡，1，外置卡
    char cICCID[64];        //ICCID
    int nCardStatus;        //0,没有卡，1，有卡
	char szOperator[64];	//运营商
	int isDirect;           // 0,该字段无效，适配以前的设备；1,定向流量卡；2，非定向流量卡
    char cReserve[192 - 4];     //保留字段
}HY_DVR_4G_CARD_CFG_INFO, *LPHY_DVR_4G_CARD_CFG_INFO;

typedef struct tagHY_DVR_4G_CARD_CFG_LIST
{
    int nValid;
    HY_DVR_4G_CARD_CFG_INFO st4GCardCfg[HY_DVR_MAX_4G_CARD_COUNT];
}HY_DVR_4G_CARD_CFG_LIST, *LPHY_DVR_4G_CARD_CFG_LIST;

typedef struct tagHY_DVR_POWER_CONFIG_INFO
{
    int nValid;
    int nPowerVoltage; //电源电压0,低，1，中，2.高
    int nPowerStatus;  //供电状态，0，无状态，1，放电中，2，充电中，3，已充满
    int nAutoPowerSavingMode;//自动进入省电模式开关，0，关，1，开
    int iCurrentPowerPercentage; //当前电量百分比
    int iSetPowerSavingPercentage; //设置省电模式电量百分比
	int iIsPsmState;				//当前设备是否处于省电模式状态;0-否,1-是
    int iLowBatteryAlarmSwitch; /*!< 低电量报警开关;0-关,1-开 */
    int iWakeUpDuration; /*!< 唤醒持续时间 */
    int iWakeUpIntervalTimeSwitch; /*!< 唤醒间隔时间开关;0-关,1-开 */
    int iWakeUpIntervalTime; /*!< 唤醒间隔时间 */
    int iPowerSupplyType; /*!< 唤醒间隔时间; 0. 无状态  1. 电源线供电  2. 太阳能供电  3. 仅电池供电 */
    int iShutdownAlarmSwitch; /*!< 关机报警开关;0-关,1-开 */
	int iIsSupportAOVPopWindow; /*!< 是否支持 AOV 弹窗;0：不支持，5 分钟弹窗,1：支持，2 分钟弹窗 */

    char cReserve[216];
}HY_DVR_POWER_CONFIG_INFO, *LPHY_DVR_POWER_CONFIG_INFO;


typedef struct tagHY_DVR_IPV6_IPADDR_INFO
{
	int bValid; /*!< 本结构体是否有效，0表示无效 */
	int bUseIpv6;               /*是否使用IPv6地址信息,如果此值为1，则使用Ipv6地址，为0，则使用IPv4地址，默认为0*/
	int bDhcpEnable;
	char cIpv6Addr[128];
	char cIpv6Gateway[128];
	char cIpv6Dns1[128];
	char cIpv6Dns2[128];
	char cResvered[1024]; // 保留
} HY_DVR_IPV6_IPADDR_INFO, *LP_HY_DVR_IPV6_IPADDR_INFO;

typedef struct tagHY_DVR_CMOVD_QR_INIT_NOTIFY
{
	int bValid; /*!< 本结构体是否有效，0表示无效 */
} HY_DVR_CMOVD_QR_INIT_NOTIFY, *LP_HY_DVR_CMOVD_QR_INIT_NOTIFY;

typedef struct tagHY_DVR_FRAME_INFO
{
	int bValid; //本结构体是否有效，0表示无效
	int nStreamID; //流ID, 1,表示主码流，2表示第一子码流，3表示第二子码流，4表示第三子码流....依次类推
	int nChannelID; //通道ID,通道从0开始
	int currentVideoFrameRate;  //当前视频帧率
	int currentKeyFrameInterval; //当前关键帧间隔
	char cReserve[256]; //保留
}HY_DVR_FRAME_INFO, *LP_HY_DVR_FRAME_INFO;

typedef struct tagHY_DVR_STREAM_TRANSPORT_TYPE
{
	int bValid;				//本结构体是否有效，0表示无效
	bool isPullStream;		// true表示为以协议拉流的方式获取，false表示以中间层发流的方式获取，默认为false
    int nIsSupportAddAllUser;//拉流缓冲区的 AddUser 接口是否支持 0xff 参数。0 - 不支持； 1 - 支持
	char cReserve[60];		//保留
}HY_DVR_STREAM_TRANSPORT_TYPE, *LPHY_DVR_STREAM_TRANSPORT_TYPE;

typedef struct tagHY_DVR_RMS_SERVER_INFO
{
	int bValid;				//本结构体是否有效，0表示无效
	char RMSIpAndPort[128]; // https://hdict.sd.chinamobile.com:9008/
	char userKey[12];
	char dmToken[12];
	int timeType;           //0:表示每12小时发送一次设备信息, 其他每24小时发送
	int timeInterval;       //获取时间戳的间隔s
	char deviceId[64];      //设备Id
	int isSupportRMS;		//是否支持RMS平台,0不支持，其他支持
	char cReserve[60];		//保留
}HY_DVR_RMS_SERVER_INFO, *LPHY_DVR_RMS_SERVER_INFO;

#define HY_DVR_OVDSDK_MAX_FALLING_OBJECT_DETECT_REGION  (6)
#define HY_DVR_OVDSDK_MAX_FALLING_OBJECT_IGNORE_REGION  (10)
#define HY_DVR_OVDSDK_MAX_FALLING_OBJECT_REGION_POINT   (10)

typedef struct tagHY_DVR_OVDSDK_FALLING_OBJECT_DETECT_ENABLE
{
	int bValid;
	int nFallingEnable; //设置高空抛物的开关 1-开，非1-关
}HY_DVR_OVDSDK_FALLING_OBJECT_DETECT_ENABLE, *LPHY_DVR_OVDSDK_FALLING_OBJECT_DETECT_ENABLE;

typedef struct tagHY_DVR_OVDSDK_FALLING_POINT
{
	int id;                       /**< 编号 */
	int x;                        /**< x坐标 */
	int y;                        /**< y坐标 */
	char szReserved[16];
}HY_DVR_OVDSDK_FALLING_POINT;

typedef struct tagHY_DVR_OVDSDK_FALLING_OBJECT_REGION
{
	int nVaild;
	int nPointCount;
	int areaId; //区域ID
	HY_DVR_OVDSDK_FALLING_POINT struRegionPoint[HY_DVR_OVDSDK_MAX_FALLING_OBJECT_REGION_POINT];
	HY_DVR_CMCC_OVD_OSD_TEXT struRegionText;				// 区域文本信息（当前仅检测区域支持设置区域信息，如：4号楼西南侧）
	char szReserved[32];
}HY_DVR_OVDSDK_FALLING_OBJECT_REGION, *LPHY_DVR_OVDSDK_FALLING_OBJECT_REGION;

typedef struct tagHY_DVR_OVDSDK_FALLING_OBJECT_DETECT_LINE
{
	int nVaild;
	HY_DVR_OVDSDK_FALLING_POINT struLeft;
	HY_DVR_OVDSDK_FALLING_POINT struRight;
	char szReserved[128];
}HY_DVR_OVDSDK_FALLING_OBJECT_DETECT_LINE, *LPHY_DVR_OVDSDK_FALLING_OBJECT_DETECT_LINE;

typedef struct tagHY_DVR_OVDSDK_FALLING_OBJECT_DETECT_CONFIG
{
	int nValid;
	
	int nFallingSensitivity; //设置高空抛物的灵敏度 1-5
	int nFallingConfidenceLevel; //设置高空抛物的置信度 1 - 100
	
	int nDetectCount;
	int nIgnoreCount;
	HY_DVR_OVDSDK_FALLING_OBJECT_REGION struHYDetectRegion[HY_DVR_OVDSDK_MAX_FALLING_OBJECT_DETECT_REGION];
	HY_DVR_OVDSDK_FALLING_OBJECT_REGION struHYIgnoreRegion[HY_DVR_OVDSDK_MAX_FALLING_OBJECT_IGNORE_REGION];
	HY_DVR_OVDSDK_FALLING_OBJECT_DETECT_LINE struHYDetectLine;
	
	char szReserved[128];
}HY_DVR_OVDSDK_FALLING_OBJECT_DETECT_CONFIG, *LPHY_DVR_OVDSDK_FALLING_OBJECT_DETECT_CONFIG;

#define HY_DVR_OVDSDK_MAX_FALLING_OBJECT_FLOOR_LINE  	(18)

typedef struct tagHY_DVR_OVDSDK_FALLING_OBJECT_FLOOR_LINE
{
	int nVaild;
	HY_DVR_OVDSDK_FALLING_POINT struLeft;
	HY_DVR_OVDSDK_FALLING_POINT struRight;
	int floorId;
	char szFloorName[64];		//楼层信息，如：5楼
	char szReserved[64];
}HY_DVR_OVDSDK_FALLING_OBJECT_FLOOR_LINE, *LPHY_DVR_OVDSDK_FALLING_OBJECT_FLOOR_LINE;

typedef struct tagHY_DVR_OVDSDK_FALLING_OBJECT_FLOOR_INFO
{
	int nVaild;
	int nLineCount;
	HY_DVR_OVDSDK_FALLING_OBJECT_FLOOR_LINE struFloorLine[HY_DVR_OVDSDK_MAX_FALLING_OBJECT_FLOOR_LINE];
	char szReserved[32];
}HY_DVR_OVDSDK_FALLING_OBJECT_FLOOR_INFO, *LPHY_DVR_OVDSDK_FALLING_OBJECT_FLOOR_INFO;

#define HY_DVR_OVDSDK_1400_MAX_PARABOLIC_PICTURE_NUM  4
typedef struct  tagHY_DVR_OVDSDK_PARABOLIC_INFO
{
	int nVaild;
	char FallingID[64]; //R,抛物标识
	int nInfoKind;   //R,0:其它  1:自动采集  2:人工采集
	char szSourceID[64];//R,来源图像标识
	char szDeviceID[32];  //R/O,设备编码
	int nLeftTopX;//R/O,左上角X坐标
	int nLeftTopY;//R/O,左上角Y坐标
	int nRightBottomX;//R/O,右下角X坐标
	int nRightBottomY;//R/O,右下角Y坐标
	char szAppearTime[32];//O,抛物出现时间，时间格式yyyyMMddHHmmss
	HY_DVR_OVDSDK_1400_IMAGE_INFO stParabolicSnapInfoList[HY_DVR_OVDSDK_1400_MAX_PARABOLIC_PICTURE_NUM];//抛物图片信息列表
	char szReserved[256];
}HY_DVR_OVDSDK_PARABOLIC_INFO;

#define HY_DVR_OVDSDK_1400_MAX_PARABOLIC_LIST_NUM  16
typedef struct tagHY_DVR_OVDSDK_PARABOLIC_INFO_LIST
{
	int nValid;
	int nParabolicNum;                    //抛物实际数量
	HY_DVR_OVDSDK_PARABOLIC_INFO stNonMotorVehicleInfoList[HY_DVR_OVDSDK_1400_MAX_PARABOLIC_LIST_NUM]; //抛物信息
	char *pEventJpgData;
	int nEventJpgLen;
	char szReserved[128];
}HY_DVR_OVDSDK_PARABOLIC_INFO_LIST;

typedef struct tagHY_DVR_OVDSDK_AERA_ALERT_ALARM
{
	DWORD dwID;
    int bNewTrigger; //1表示触发新的报警，0表示之前的报警持续
	char szReserved[1024];
}HY_DVR_OVDSDK_AERA_ALERT_ALARM, *LPHY_DVR_OVDSDK_AERA_ALERT_ALARM;

typedef struct tagHY_DVR_MP_POST_PARAM
{
    long long deviceOpenTime;    // 设备开启启动时长
    char boardId[64];            // boardId
    int  customer;               // 客户id
    char versionId[64];          // 客户版本id
    char reserved[128];          // 保留
}HY_DVR_MP_POST_PARAM;

typedef struct tagHY_DVR_AGINGTEST_MODE_INFO
{
	int bValid;				//本结构体是否有效，0表示无效
	int AgingTest_Enable;   //老化测试模式开关            0表示关，1表示开
    int Whiteled_Enable;    //白光灯测试开关             0表示关，1表示开
    int Redled_Enable;      //红外灯测试开关             0表示关，1表示开
    int Ircut_Enable;       //IRCUT测试开关           0表示关，1表示开
    int Audio_Enable;       //音频测试开关              0表示关，1表示开
    int Indicator_Time;     //两种颜色指示灯交替闪烁的时间间隔 单位:秒
    int Ircut_Time;         //IRCUT切换的时间间隔 单位:秒
    int Audio_Time;         //音频播放的时间间隔 单位:秒
    int Whiteled_Time;      //白光灯亮灭时间间隔 单位:秒
    int Redled_Time;        //红外灯亮灭时间间隔 单位:秒
    char cReserve[128];     //保留
}HY_DVR_AGINGTEST_MODE_INFO, *LPHY_DVR_AGINGTEST_MODE_INFO;

typedef struct tagHY_DVR_IPC_KEY_JIESHUN_CUSTOM
{
	int iValid;					//本结构体是否有效，0表示无效

	unsigned long ulRandom;   	//随机整数，32位，无符号整数。用于对32字节的key计算异或值,是否必选:是
    unsigned long ulSN;			//请求序号．标志控制机发送的请求次数，在一定时间内要保证唯一，控制机请求序号在１到99999999之间循环增加,是否必选:是
    char acInputReserve[128];	//保留

	char acMFR[128];     		//厂商名称
	char acMFD[32];     		//生产日期, 格式yyyy-mm-dd
	char acMFRCode[32];     	//
	char acHardwareVer[64];     //硬件版本号或唯一标识（厂家可自行定义便于问题反馈）
	char acSoftwareVer[64];     //软件版本号或唯一标识（厂家可自行定义便于问题反馈）
	char acMac[32];     		//摄像头的MAC,注意如果里面有字母,要求字母是大写.例如"00:18:99:11:AA:BB".注意定制摄像头,这个MAC应该是捷顺提供一批MAC给摄像头厂家.
	char ackey[128];     		//Key是请求消息中的random（32位无符号整数）和id异或后，进行BASE64编码的结果

	char acOutputReserve[128];	//保留
}HY_DVR_IPC_KEY_JIESHUN_CUSTOM, *LPHY_DVR_IPC_KEY_JIESHUN_CUSTOM;

typedef struct tagHY_DVR_ALGO_MALL_TRANSPARENT_TRANSMISSION_CFG
{
    int iValid;

    int iIsElectronicDenceTransparent;            //电子围栏配置是否透传给算法进程;0-否,1-是
    int iIsElectronicDenceExTransparent;          //电子围栏拓展配置是否透传给算法进程;0-否,1-是
    int iIsCrossBorderDetectTransparent;          //越界侦测配置是否透传给算法进程;0-否,1-是
    int iIsCrossBorderDetectExTransparent;        //越界侦测拓展配置是否透传给算法进程;0-否,1-是
    int iIsOffDutyDetectTransparent;              //离岗侦测配置是否透传给算法进程;0-否,1-是
    int iIsOffDutyDetectExTransparent;            //离岗侦测拓展配置是否透传给算法进程;0-否,1-是
    int iIsPassengerFlowStaticsTransparent;       //客流统计配置是否透传给算法进程;0-否,1-是
    int iIsPassengerFlowStaticsExTransparent;     //客流统计拓展配置是否透传给算法进程;0-否,1-是
    int iIsIntelligentTrackTransparent;           //智能跟踪配置是否透传给算法进程;0-否,1-是
    int iIsOffDutyQueryTransparent;               //离岗查询配置是否透传给算法进程;0-否,1-是
    int iIsCryScreamDetectTransparent;            //哭声叫声配置是否透传给算法进程;0-否,1-是
    int iIsFaceManagerTransparent;                //脸库管理相关配置是否透传给算法进程;0-否,1-是
    int iIsTryListenSelectedAlarmAudioV1Transparent;//试听配置是否透传给算法进程;0-否,1-是
    int iIsPeopleDetectTransparent;                 //人形检测是否透传给算法进程;0-否,1-是;对应url:/Pictures/1/PeopleDetect
    int iIsMoveDetectTransparent;                   //移动侦测是否透传给算法进程;0-否,1-是;对应url:/Pictures/1/Motion
    int iIsAudioAlarmConfigTransparent;             //声音报警是否透传给算法进程;0-否,1-是;对应url:/System/AudioAlarmConfig
    int iIsPolygonConfigTransparent;                //多边形侦测是否透传给算法进程;0-否,1-是;对应url:/Alarm/1/PolygonConfig
    int iIsPolygonScheduleTransparent;              //多边形布防配置是否透传给算法进程;0-否,1-是;对应url:/Alarm/1/PolygonSchedule
    int iIsAlarmOverlayInfoTransparent;             //布防叠加框配置是否透传给算法进程;0-否,1-是;对应url:/Alarm/OverlayInfo
    int iIsPushEventIntervalTransparent;            //事件推送间隔配置是否透传给算法进程;0-否,1-是;对应url:/System/1/PushEventInterval
	int iIsAlarmLightWarningTransparent;            //灯光报警配置是否透传给算法进程;0-否,1-是;对应url:/Alarm/ChannelID/LightWarning

    char acReserved[508];              //保留字段
}HY_DVR_ALGO_MALL_TRANSPARENT_TRANSMISSION_CFG, *LPHY_DVR_ALGO_MALL_TRANSPARENT_TRANSMISSION_CFG;

typedef enum eAlgoStoreTransType
{
	eElectronicDenceTrans,
	eElectronicDenceExTrans,
	eCrossBorderDetectTrans,
	eCrossBorderDetectExTrans,
	eOffDutyDetectTrans,
	eOffDutyDetectExTrans,
	ePassengerFlowStaticsTrans,
	ePassengerFlowStaticsExTrans,
	eIntelligentTrackTrans,
	eOffDutyQueryTrans,
	eCryScreamDetectTrans,
	eFaceManagerTrans,
	eTryListenSelectedAlarmAudioV1Trans,
	ePeopleDetectTrans,
	eMoveDetectTrans,
	eAudioAlarmConfigTrans,
	ePolygonConfigTrans,
	ePolygonScheduleTrans,
	eAlarmOverlayInfoTrans,
	ePushEventIntervalTrans,
	eAlarmLightWarningTrans,
} eAlgoStoreTransType;

//AIMAN 状态
typedef enum
{
    //AI进程返回
    AI_Status_Uninit = 0,       //未初始化
    AI_Status_Inited,           //已初始化，尚未运行
    AI_Status_Runing,           //正常运行中
    AI_Status_Stopping,         //正在停止中
    //主进程返回
    AI_Status_Offline,          //离线
}E_AI_STATUS;
//返给网络状态码： NET_AI_STATUS_CODE_BASE + E_AI_STATUS 
#define NET_AI_STATUS_CODE_BASE (0x00010100)

//AI功能状态
typedef enum
{
    AiFunc_Status_UnStart = 0, //未启动
    AiFunc_Status_Installing,  //安装中
    AiFunc_Status_Starting,    //启动中
    AiFunc_Status_NoAuth,      //当前系统时间授权无效
    AiFunc_Status_Runing,      //正常运行中
    AiFunc_Status_RunErrorAuthDue, //运行异常：授权过期
}E_AI_FUNC_STATUS;
//返给网络状态码： NET_AI_FUNC_STATUS_CODE_BASE + E_AI_FUNC_STATUS 
#define NET_AI_FUNC_STATUS_CODE_BASE (0x00010200)

typedef struct tagHY_DVR_ALGO_CUSTOM
{
    int iValid;

    char *pcBuffer;        //透传的信息内容;包含http请求的方法,url,若请求为PUT方法还必须包含url对应的配置xml信息
    int iBufferLen;        //透传的信息长度
    int iMaxBufLen;        //支持的最大长度

    char acReserved[4096];              //保留字段
}HY_DVR_ALGO_CUSTOM,*LPHY_DVR_ALGO_CUSTOM;


typedef struct tagHY_DVR_ZWHOST_API_ADD_DOMAIN_FOR_GET_ALIAS
{
	int nValid;
	char cDomain[384];
}HY_DVR_ZWHOST_API_ADD_DOMAIN_FOR_GET_ALIAS,*LPHY_DVR_ZWHOST_API_ADD_DOMAIN_FOR_GET_ALIAS;

typedef struct tagHY_DVR_DOMAIN_INFO
{
	int nValid;
	char cDomain[128];
	char cIP[32];
	char cReserve[256];
}HY_DVR_DOMAIN_INFO,*LPHY_DVR_DOMAIN_INFO;


typedef struct tagHY_DVR_ZWHOST_API_ADD_DOMAIN_LIST_2_LOCAL
{
	int nValid;
	HY_DVR_DOMAIN_INFO stDomainList[32];
	char cReserve[256];
}HY_DVR_ZWHOST_API_ADD_DOMAIN_2_LOCAL,*LPHY_DVR_ZWHOST_API_ADD_DOMAIN_2_LOCAL;

typedef struct tagHY_DVR_DEFENCE_TIME_SCHEDULE
{
    int iValid;                    /*!< 本结构体是否有效，0表示无效 */

    int iEnable;                   /*!< 时间段是否有效，0-无效,1-有效 */
    char acStartTime[32];          /*!< 开始时间，格式hh:mm,24小时制 */        
    char acEndTime[32];            /*!< 结束时间，格式hh:mm,24小时制 */
    char acWeekday[8];             /*!< 每周7天，下标0表示周日,下标1表示周一,以此类推;元素取值1表示选定对应星期,取值0表示没有选择对应星期, 第8位保留 */

    char szReserved[128];
}HY_DVR_DEFENCE_TIME_SCHEDULE;

#define DEFENCE_TIME_SCHEDULE_NUM                    16
typedef struct tagHY_DVR_DEFENCE_TIME_SCHEDULE_LIST
{
    int iValid;

    int iEnable;                    /*!< 是否有效，0-无效,1-有效;保留 */
    HY_DVR_DEFENCE_TIME_SCHEDULE stDefenceSchedule[DEFENCE_TIME_SCHEDULE_NUM];     /*!< 布防时间计划 */

    char szReserved[1024];
}HY_DVR_DEFENCE_TIME_SCHEDULE_LIST, *LPHY_DVR_DEFENCE_TIME_SCHEDULE_LIST;


//事件类型ALERT_DEBUG_FILE专用
typedef struct tagHY_DVR_FILE_GUIDE{
	char cName[256];				//文件名
	int  nSize;						//文件大小
	unsigned int nOffset;			//文件起始偏移
}HY_DVR_FILE_GUIDE;


//网络信息
typedef struct tagHY_DVR_NETWORK_INFO
{
	int iValid;
	int nNetworkStatus;  //0,断网状态，1.联网状态
	char cReserve[256];
}HY_DVR_NETWORK_INFO,*LPHY_DVR_NETWORK_INFO;

typedef struct tagHY_DVR_DEVICE_OPERATE_STATUS
{
	int packetLossRate;  //设备丢包率，单位%
	int cpuUsage;		// cpu占用率，单位%
	int deviceMemoryUsage;  // 设备内存占用率，单位%
	int availableMemory;  // 设备剩余内存，单位KB
	int deviceRunningTotalMemory; // 设备运行总内存，单位KB
	int configPartitionAvailable; // 设备配置分区剩余容量，单位KB
	char cReserve[256];
}HY_DVR_DEVICE_OPERATE_STATUS,*LPHY_DVR_DEVICE_OPERATE_STATUS;



typedef struct tagHY_DVR_DEVICE_RUN_MODE
{
	int nValid;
	int nRunMode;		//0.常电模式，1.低功耗，2.省电，3.AOV
	char cReserve[32];
}HY_DVR_DEVICE_RUN_MODE,*LPHY_DVR_DEVICE_RUN_MODE;


typedef struct tagHY_DVR_ONVIF_PTZ_STATUS
{
    int nValid;

    float fX;
    float fY;
    float fZoom;
    int nPanTiltStatus;         // 移动状态；0 - IDLE， 1 - Moving， 2 - Unkonwn
    int nZoomStatus;            // 移动状态；0 - IDLE， 1 - Moving， 2 - Unkonwn

    char cReserve[128];
}HY_DVR_ONVIF_PTZ_STATUS, *LPHY_DVR_ONVIF_PTZ_STATUS;


typedef struct tagHY_DVR_LICENSE_PLATE_SUVEILLANCE_VOICEALARM_CFG
{
    int iValid;

    int iEnable;                                                    /*!< 是否使能 */
    int iVoiceType; 	/*!< 语音类型；0:自定义,1:警报,2:犬吠,3:警戒区域,4:监控区域,5:欢迎光临,6:深水警告,7:私人领地,8:危险区域,9:贵重物品,10:高处危险,
                        11:私家价位,12:叮咚,13:有口罩,14:无口罩,15:注意私人物品,16:抓小偷,17:救命,18:分类,19:可回收垃圾,20:家庭食物垃圾
                        21:危险废物,22:残余垃圾,23:全部分类,24:车形自定义,25:禁止停车 */
    int iBroadcastCount;                                            /*!< 播报次数 */
    int iBroadcastVolume; /*!< 播报音量 */ 
    int iDayMode;	/*!< 天模式，与stSchedTime组合使用；0:夜晚模式,1:白天模式,2:不启用此模式（AllDay字段有效，当字段不为disable时，AllDay字段无效）当设置为此模式时，IPC依赖自身传感器判断当前环境是白天还是夜晚*/ 
    HY_DVR_SCHEDTIME stSchedTime;                                    /*!< 声音报警布防时间(web用) */
	HY_DVR_SCHEDULE_TIMESLOT stSchedTimeSlot[SCHEDULE_TIMESLOT_MAX_NUM];     /*!< 布防时间计划(app用) */
	int iIsLicensePlateBroadcast;									/*!< 是否车牌播报；0-否，1-是 */
	int iLicensePlateBroadcastMode;									/*!< 车牌播报模式；0-仅车牌播报，1-车牌播报+自定义语音 */

	char acReserved[128 - 8];
} HY_DVR_LICENSE_PLATE_SUVEILLANCE_VOICEALARM_CFG, *LPHY_DVR_LICENSE_PLATE_SUVEILLANCE_VOICEALARM_CFG;

#define MAX_LICENSE_PLATE_SUVEILLANCE_RULE_CFG_COUNT  8
typedef struct tagHY_DVR_LICENSE_PLATE_SUVEILLANCE_RULE_CFG
{
    int iValid;

 	int iDayMode;/*!< 天模式，与stSchedTime组合使用；0:夜晚模式,1:白天模式,2:不启用此模式（AllDay字段有效，当字段不为disable时，AllDay字段无效）当设置为此模式时，IPC依赖自身传感器判断当前环境是白天还是夜晚*/
    HY_DVR_SCHEDTIME stSchedTime;                                 /*!< 车牌布控 布防时间(web用) */
	HY_DVR_SCHEDULE_TIMESLOT stSchedTimeSlot[SCHEDULE_TIMESLOT_MAX_NUM];     /*!< 车牌布控 布防时间计划(app用) */
	HY_DVR_HANDLEEXCEPTION stHandleType;						  /*!< 处理方式 */ 
	HY_DVR_LICENSE_PLATE_SUVEILLANCE_VOICEALARM_CFG stVoiceAlarm; /*!< 声音报警 */
	char cSensitivity;												/*!< 灵敏度,[0,100],目前仅无车牌布控类型有效*/

    char acReserved[124];
} HY_DVR_LICENSE_PLATE_SUVEILLANCE_RULE_CFG, *LPHY_DVR_LICENSE_PLATE_SUVEILLANCE_RULE_CFG;

typedef struct tagHY_DVR_LICENSE_PLATE_SURVEILLANCE_UI_DESIGN_CFG
{
    int iValid;

	int iEnable;													/*!< 是否使能 */
	int iLicensePlateSuveillanceType;                               /*!< 车牌布控类型；bit0位为1表示关注车牌，bit1位为1表示陌生车牌，bit2位为1表示非绿色车牌， bit3位为1表示黄色车牌， bit4位为1表示无牌车，依此类推 */
	HY_DVR_LICENSE_PLATE_SUVEILLANCE_RULE_CFG stRuleCfgList[MAX_LICENSE_PLATE_SUVEILLANCE_RULE_CFG_COUNT];	//车牌布控类型及对应的声音报警、灯光报警、布防时间、联动方式;下标为0的元素表示关注车牌,下标为1的元素表示陌生车牌,下标为2的元素表示非绿色车牌,下标为3的元素表示黄色车牌,下标为4的元素表示无牌车，依此类推

	char acReserved[256];
}HY_DVR_LICENSE_PLATE_SURVEILLANCE_UI_DESIGN_CFG, *LPHY_DVR_LICENSE_PLATE_SURVEILLANCE_UI_DESIGN_CFG;


typedef struct tagHY_DVR_LICENSE_PLATE_OWNNER_INFO
{
    int iValid;

    char acOwnner[64];         //车牌库拥有者，由想操作设备脸库的客户端生成的唯一标志

    char acReserved[256];
}HY_DVR_LICENSE_PLATE_OWNNER_INFO, *LPHY_DVR_LICENSE_PLATE_OWNNER_INFO;


typedef struct tagHY_DVR_CREATE_LICENSE_PLATE_GROUP
{
	int iValid;

 	char acSystemTime[32];        			//系统时间，格式:YYYY-MM-DDThh:mm:ss
 	char acLicensePlateGroupName[64];		//车牌库名字
 	char acLicensePlateGroupID[64];          //车牌库ID
	char acSecurityCode[64];          		//安全码
	char acVersion[64];       				//版本
	
	char acInReserved[512];

	char acLicensePlateGroupIDOut[64];			//车牌库ID

	char acOutReserved[128];
}HY_DVR_CREATE_LICENSE_PLATE_GROUP, *LPHY_DVR_CREATE_LICENSE_PLATE_GROUP;


typedef struct tagHY_DVR_DELETE_LICENSE_PLATE_GROUP
{
	int iValid;

	char acSystemTime[32];        			//系统时间，格式:YYYY-MM-DDThh:mm:ss
	char acLicensePlateGroupID[64];          //车牌库ID
	char acSecurityCode[64];          		//安全码

	char acReserved[128];
}HY_DVR_DELETE_LICENSE_PLATE_GROUP, *LPHY_DVR_DELETE_LICENSE_PLATE_GROUP;


typedef struct tagHY_DVR_MODIFY_LICENSE_PLATE_GROUP
{
	int iValid;

	char acSystemTime[32];        			//系统时间，格式:YYYY-MM-DDThh:mm:ss
	char acLicensePlateGroupName[64];		//车牌库名字
	char acLicensePlateGroupID[64];          //车牌库ID
	char acSecurityCode[64];          		//安全码

	char acReserved[256];
}HY_DVR_MODIFY_LICENSE_PLATE_GROUP, *LPHY_DVR_MODIFY_LICENSE_PLATE_GROUP;


//最大支持查询的车牌库个数(协议与固件约定好的，每次查询时申请的内存大小)
#define MAX_LICENSE_PLATE_GROUP_COUNT   16

typedef struct tagHY_DVR_LICENSE_PLATE_GROUP
{
	int iValid;

	int iLicensePlateNumber;					//车牌数量
	char acCreateTime[32];        			//车牌库创建时间，格式:YYYYMMDDThhmmss
	char acLicensePlateGroupName[64];		//车牌库名字
	char acLicensePlateGroupID[64];          //车牌库ID

	char acReserved[256];
}HY_DVR_LICENSE_PLATE_GROUP, *LPHY_DVR_LICENSE_PLATE_GROUP;

typedef struct tagHY_DVR_LICENSE_PLATE_GROUP_LIST
{
	int iValid;

	HY_DVR_LICENSE_PLATE_GROUP *pstLicensePlateGroup;

	char acReserved[512];
}HY_DVR_LICENSE_PLATE_GROUP_LIST, *LPHY_DVR_LICENSE_PLATE_GROUP_LIST;

typedef struct tagHY_DVR_UPDATE_LICENSE_PLATE_INFO
{
	int iValid;

	int iLicensePlateColor;        			//车牌颜色(首版固件暂不使用该字段);0-起始无效值,1-蓝牌,2-黄牌,3-黑牌,4-白牌,5-绿牌,6-渐变绿,7-黄绿,8-缺省
	int iCarColor;							//车辆颜色(首版固件暂不使用该字段);0-起始无效值,1-黑色,2-白色,3-银灰,4-深灰,5-棕色,6-红色,7-橙色,8-黄色,9-绿色,10-青色,11-蓝色,12-粉色,13-紫色,14-香槟色
	char acCarOwnnerName[64];          		//车主姓名(首版固件暂不使用该字段) 
	char acLicensePlate[64];					//车牌
	char acLicensePlateGroupID[64];         //车牌库ID
	char acLicensePlateUUID[64];			//车牌唯一标识ID，需保证唯一性

	char acReserved[512];
}HY_DVR_UPDATE_LICENSE_PLATE_INFO, *LPHY_DVR_UPDATE_LICENSE_PLATE_INFO;

#define  MAX_LICENSE_PLATE_INFO_UPDATE_COUNT  50
typedef struct tagHY_DVR_UPDATE_LICENSE_PLATE_INFO_LIST
{
	int iValid;

	char acSystemTime[32];        			//系统时间，格式:YYYY-MM-DDThh:mm:ss
	char acSecurityCode[64];          		//安全码 
	HY_DVR_UPDATE_LICENSE_PLATE_INFO stLicensePlateList[MAX_LICENSE_PLATE_INFO_UPDATE_COUNT]; //待更新的车牌信息，单次请求最大支持同时更新50个车牌信息

	char acReserved[512];
}HY_DVR_UPDATE_LICENSE_PLATE_INFO_LIST, *LPHY_DVR_UPDATE_LICENSE_PLATE_INFO_LIST;

typedef struct tagHY_DVR_DELETE_LICENSE_PLATE_INFO
{
	int iValid;

	char acLicensePlate[64];					//车牌
	char acLicensePlateGroupID[64];         //车牌库ID
	char acLicensePlateUUID[64];			//车牌唯一标识ID，需保证唯一性

	char acReserved[512];
}HY_DVR_DELETE_LICENSE_PLATE_INFO, *LPHY_DVR_DELETE_LICENSE_PLATE_INFO;

#define	MAX_LICENSE_PLATE_INFO_DELETE_LIST	50
typedef struct tagHY_DVR_DELETE_LICENSE_PLATE_INFO_LIST
{
	int iValid;

	char acSystemTime[32];        			//系统时间，格式:YYYY-MM-DDThh:mm:ss
	char acSecurityCode[64];          		//安全码 
	HY_DVR_DELETE_LICENSE_PLATE_INFO stLicensePlateList[MAX_LICENSE_PLATE_INFO_DELETE_LIST]; //待删除的车牌信息，单次请求最大支持同时删除50个车牌信息

	char acReserved[512];
}HY_DVR_DELETE_LICENSE_PLATE_INFO_LIST, *LPHY_DVR_DELETE_LICENSE_PLATE_INFO_LIST;

typedef struct tagHY_DVR_QUERY_LICENSE_PLATE_INFO
{
    int iValid;
 
	int iLicensePlateColor;                  //车牌颜色(首版固件暂不使用该字段);0-起始无效值,1-蓝牌,2-黄牌,3-黑牌,4-白牌,5-绿牌,6-渐变绿,7-黄绿,8-缺省
	int iCarColor;                          //车辆颜色(首版固件暂不使用该字段);0-起始无效值,1-黑色,2-白色,3-银灰,4-深灰,5-棕色,6-红色,7-橙色,8-黄色,9-绿色,10-青色,11-蓝色,12-粉色,13-紫色,14-香槟色
	char acCarOwnnerName[64];               //车主姓名(首版固件暂不使用该字段)
	char acLicensePlate[64];                 //车牌
	char acLicensePlateGroupID[64];         //车牌库ID
	char acLicensePlateUUID[64];            //车牌唯一标识ID，需保证唯一性

	char acReserved[512];
}HY_DVR_QUERY_LICENSE_PLATE_INFO, *LPHY_DVR_QUERY_LICENSE_PLATE_INFO;

typedef struct tagHY_DVR_QUERY_LICENSE_PLATE_INFO_LIST
{
	int iValid;

	int iLicensePlateColor;        		//作为输入参数,车牌颜色(首版固件暂不使用该字段);0-起始无效值,1-蓝牌,2-黄牌,3-黑牌,4-白牌,5-绿牌,6-渐变绿,7-黄绿,8-缺省
	int iCarColor;						//作为输入参数,车辆颜色(首版固件暂不使用该字段);0-起始无效值,1-黑色,2-白色,3-银灰,4-深灰,5-棕色,6-红色,7-橙色,8-黄色,9-绿色,10-青色,11-蓝色,12-粉色,13-紫色,14-香槟色
	char acCarOwnnerName[64];			//作为输入参数,车主姓名(首版固件暂不使用该字段)  
	char acLicensePlate[64];			//作为输入参数,车牌
	char acLicensePlateGroupID[64];     //作为输入参数,车牌库ID
	char acLicensePlateUUID[64];		//作为输入参数,车牌唯一标识ID，需保证唯一性
	int iPageSize;						//作为输入参数,表示每一页最大的记录条数,作为输出参数，表示实际的一页的记录个数
	int iPageNum;						//作为输入参数,表示第几页
	char acInReserved[512];
 
    int iLicenseNumber;                //作为输出参数,符合查询条件的车牌总个数
	HY_DVR_QUERY_LICENSE_PLATE_INFO * pstLicensePlateList; //车牌信息

	char acOutReserved[512];
}HY_DVR_QUERY_LICENSE_PLATE_INFO_LIST, *LPHY_DVR_QUERY_LICENSE_PLATE_INFO_LIST;


typedef struct tagHY_DVR_ONE_CLICK_DELETE_ALL_LICENSE_PLATE_GROUP_CFG
{
    int iValid;
 
   char acSystemTime[32];                  //系统时间，格式:YYYY-MM-DDThh:mm:ss
   char acSecurityCode[64];                //安全码 
 
   char acReserved[128];
}HY_DVR_ONE_CLICK_DELETE_ALL_LICENSE_PLATE_GROUP_CFG, *LPHY_DVR_ONE_CLICK_DELETE_ALL_LICENSE_PLATE_GROUP_CFG;


typedef struct tagHY_DVR_EBIKE_DETECT_VOICE_ALARM
{
    int iValid;
 
    int iEnable;                                                    /*!< 是否使能 */
	int iVoiceType; /*!< 0:自定义,1:警报,2:犬吠,3:警戒区域,4:监控区域,5:欢迎光临,
                        6:深水警告,7:私人领地,8:危险区域,9:贵重物品,10:高处危险,
                        11:私家价位,12:叮咚,13:有口罩,14:无口罩,15:注意私人物品,
                        16:抓小偷,17:救命,18:分类,19:可回收垃圾,20:家庭食物垃圾
                        21:危险废物,22:残余垃圾,23:全部分类,24:车形自定义,25:禁止停车 */
	int iBroadcastCount;                                            /*!< 播报次数,范围1~5次 */
	int iBroadcastVolume;                                         	/*!< 播报音量；取值1-100，默认90 */
    int iDayMode;/*!< 声音报警 布防计划的天模式(web用);0:夜晚模式,1:白天模式,2:不启用此模式（AllDay字段有效，当字段不为disable时，AllDay字段无效）
                        当设置为此模式时，IPC依赖自身传感器判断当前环境是白天还是夜晚*/
    HY_DVR_SCHEDTIME stSchedTime;                                    /*!< 声音报警布防时间(web用) */
	HY_DVR_SCHEDULE_TIMESLOT stSchedTimeSlot[SCHEDULE_TIMESLOT_MAX_NUM];     /*!< 布防时间计划(app用) */
    
	char acReserved[256];
} HY_DVR_EBIKE_DETECT_VOICE_ALARM, *LPHY_DVR_EBIKE_DETECT_VOICE_ALARM;
 
#define MAX_EBIKE_DETECT_REGION_POINT_NUM 8
typedef struct tagHY_DVR_EBIKE_DETECT_CFG
{
    int iValid;
 
    int iEnable;                                   /*!< 是否使能电动车侦测,0-否,1-是 */
    int iSensitivity;                              /*!< 电动车侦测灵敏度;取值[0-100] */
	int iIsShowDefenceArea;                        /*!< 是否显示布防区域,0-否,1-是 */
    int iIsShowTargetBox;                          /*!< 是否显示目标框,0-否,1-是 */
	int iEBikeDetectEliminateDuration;             /* 电动车检测音频报警消除时长，单位：秒，最大时长600s */
	int iDayMode;		/*!< 电动车探测 布防计划的天模式(web用);0:夜晚模式,1:白天模式,2:不启用此模式（AllDay字段有效，当字段不为disable时，AllDay字段无效）
                        	当设置为此模式时，IPC依赖自身传感器判断当前环境是白天还是夜晚*/
	HY_DVR_EBIKE_DETECT_VOICE_ALARM stVoiceAlarm;  /*!< 语音报警 */
	HY_DVR_POINT_V2 stEBikeDetectRegion[MAX_EBIKE_DETECT_REGION_POINT_NUM]; /*!< 电动车探测区域 */
    HY_DVR_SCHEDTIME stSchedTime;                  /*!< 布防计划(web用) */
    HY_DVR_SCHEDULE_TIMESLOT stSchedTimeSlot[SCHEDULE_TIMESLOT_MAX_NUM];  /*!< 布防时间计划(app用) */ 
	HY_DVR_HANDLEEXCEPTION stHandleType;           /*!< 电动车探测处理方式 */ 
 
	char acReserved[256];
}HY_DVR_EBIKE_DETECT_CFG, *LPHY_DVR_EBIKE_DETECT_CFG;


typedef struct tagHY_DVR_GAS_TANK_DETECT_VOICE_ALARM
{
    int iValid;
 
    int iEnable;                                                    /*!< 是否使能 */
    int iVoiceType; /*!< 0:自定义,1:警报,2:犬吠,3:警戒区域,4:监控区域,5:欢迎光临,
                        6:深水警告,7:私人领地,8:危险区域,9:贵重物品,10:高处危险,
                        11:私家价位,12:叮咚,13:有口罩,14:无口罩,15:注意私人物品,
                        16:抓小偷,17:救命,18:分类,19:可回收垃圾,20:家庭食物垃圾
                        21:危险废物,22:残余垃圾,23:全部分类,24:车形自定义,25:禁止停车 */
    int iBroadcastCount;                                            /*!< 播报次数,范围1~5次 */
	int iBroadcastVolume;                                         	/*!< 播报音量；取值1-100，默认90 */
    int iDayMode;/*!< 声音报警 布防计划的天模式(web用);0:夜晚模式,1:白天模式,2:不启用此模式（AllDay字段有效，当字段不为disable时，AllDay字段无效）
                        当设置为此模式时，IPC依赖自身传感器判断当前环境是白天还是夜晚*/
	HY_DVR_SCHEDTIME stSchedTime;                                    /*!< 声音报警布防时间(web用) */
	HY_DVR_SCHEDULE_TIMESLOT stSchedTimeSlot[SCHEDULE_TIMESLOT_MAX_NUM];     /*!< 布防时间计划(app用) */
    
	char acReserved[256];
} HY_DVR_GAS_TANK_DETECT_VOICE_ALARM, *LPHY_DVR_GAS_TANK_DETECT_VOICE_ALARM;
 
#define MAX_GAS_TANK_DETECT_REGION_POINT_NUM 8
typedef struct tagHY_DVR_GAS_TANK_DETECT_CFG
{
    int iValid;
 
    int iEnable;                                   /*!< 是否使能煤气罐侦测,0-否,1-是 */
    int iSensitivity;                                 /*!< 煤气罐侦测灵敏度;取值[0-100] */
	int iIsShowDefenceArea;                        /*!< 是否显示布防区域,0-否,1-是 */
    int iIsShowTargetBox;                          /*!< 是否显示目标框,0-否,1-是 */
	int iDayMode;		/*!< 煤气罐探测 布防计划的天模式(web用);0:夜晚模式,1:白天模式,2:不启用此模式（AllDay字段有效，当字段不为disable时，AllDay字段无效）
                        	当设置为此模式时，IPC依赖自身传感器判断当前环境是白天还是夜晚*/
    HY_DVR_GAS_TANK_DETECT_VOICE_ALARM stVoiceAlarm; /*!< 语音报警 */
	HY_DVR_POINT_V2 stGasTankDetectRegion[MAX_GAS_TANK_DETECT_REGION_POINT_NUM]; /*!< 煤气罐探测区域 */
    HY_DVR_SCHEDTIME stSchedTime;                  /*!< 布防计划(web用) */
    HY_DVR_SCHEDULE_TIMESLOT stSchedTimeSlot[SCHEDULE_TIMESLOT_MAX_NUM];  /*!< 布防时间计划(app用) */ 
	HY_DVR_HANDLEEXCEPTION stHandleType;           /*!< 煤气罐探测处理方式 */ 
 
	char acReserved[256];
}HY_DVR_GAS_TANK_DETECT_CFG, *LPHY_DVR_GAS_TANK_DETECT_CFG;



typedef struct tagHY_DVR_PTZ_POS_INFO
{
	int nValid;

	float fX;	        // X坐标
	float fY;	        // Y坐标
    float fZoom;      // 变焦
    float fXSpeed;
    float fYSpeed;
    float fZoomSpeed;

	char szReserved[32];
}HY_DVR_PTZ_POS_INFO, *LPHY_DVR_PTZ_POS_INFO;


typedef struct tagHY_DVR_DEVICE_MODULE_RESET_DEFAULT
{
	int iValid;
 
	int iIsResetVolume;			/**< 是否恢复 输入与输出音量 模块默认配置； 0-否，1-是 */
	int iIsResetAudio;			/**< 是否恢复 音频 模块默认配置； 0-否，1-是 */
	int iIsResetVideo;			/**< 是否恢复 视频 模块默认配置； 0-否，1-是 */
	int iIsResetImage;			/**< 是否恢复 图像 模块默认配置； 0-否，1-是 */
	int iIsResetNetwork;		/**< 是否恢复 网络 模块默认配置； 0-否，1-是 */

	char reserve[256];
}HY_DVR_DEVICE_MODULE_RESET_DEFAULT;


typedef struct tagHY_DVR_ACTIVE_INFO
{
    int bValid;                                                           /*!< 本结构体是否有效，0表示无效  */
    unsigned int uSupportActivedFeatureMask;    //当前设备支持特性掩码，此字段的每一位表示设备是否支持此功能，取值参考激活掩码宏定义
    unsigned int uAllActivedStatusMask;                //当前设备各特性激活状态掩码，此字段的每一位表示设备是否激活了此功能，取值参考激活码宏定义
    unsigned int unAllActivedFunctionMask;        //设置激活码操作，使用掩码的方式，如果对应的掩码位为0，表示该功能不进行设置，如果为1，表示激活该功能  ，取值参考激活码宏定义
    char cReserve[32];
}HY_DVR_ACTIVE_INFO;

typedef struct tagHY_DVR_ACTIVE_CFG
{
    int bValid;                                                         /*!< 本结构体是否有效，0表示无效  */
	int nMaxActiveInfoCount;						/*!< 最大激活信息个数，其值是从中间层获取（HY_DVR_BUFFER_SIZE的nActiveInfoCount字段），此处作为输入参数告知中间层pActiveInfoList指针指向的内存支持的最大的信息列表个数，避免中间层赋值时越界*/ 
    char cActiveCode[132];                                 /*!< 激活码的最大长度之所以设置为132的原因是为了避免当激活码未128个字节时，后面\0没有位置放，同时也为了四字节对齐，所以专门增加了4个字节  */
	HY_DVR_ACTIVE_INFO * pActiveInfoList;			/*!< 激活信息列表，该指针由CGI申请和释放，其值由HY_DVR_BUFFER_SIZE的nActiveInfoCount字段确定*/
    char cReserve[32];
}HY_DVR_ACTIVE_CFG;

typedef struct tagHY_DVR_INTERNAL_ACTIVE_INFO
{
    int bValid;                                                          /*!< 本结构体是否有效，0表示无效  */
    unsigned int unAllActivedFunctionMask;        //设置激活码操作，使用掩码的方式，如果对应的掩码位为0，表示该功能不进行设置，如果为1，表示激活该功能    
    char cReserve[32];
}HY_DVR_INTERNAL_ACTIVE_INFO;

typedef struct tagHY_DVR_INTERNAL_ACTIVE_CFG
{
    int bValid;                                                         /*!< 本结构体是否有效，0表示无效  */
	int nMaxActiveInfoCount;								/*!< 最大激活信息个数，其值是从中间层获取（HY_DVR_BUFFER_SIZE的nActiveInfoCount字段），此处作为输入参数告知中间层pActiveInfoList指针指向的内存支持的最大的信息列表个数，避免中间层赋值时越界*/ 
    HY_DVR_INTERNAL_ACTIVE_INFO  * pActiveInfoList;			/*!< 激活信息列表，该指针由CGI申请和释放，其值由HY_DVR_BUFFER_SIZE的nActiveInfoCount字段确定 */
    char cActiveCode[132];									/*!< 激活码的最大长度之所以设置未132的原因是为了避免当激活码未128个字节时，后面\0没有位置放，同时也为了四字节对齐，所以专门增加了4个字节  */
    char cReserve[32];
}HY_DVR_INTERNAL_ACTIVE_CFG;

/*!
*  @struct  tagHY_DVR_RTMP_REGISTER_SERVER_INFO
*  @brief    RTMPRegister的服务器信息
 */
typedef struct
{
    int iValid;
 
    int	nPort;                                          /*!< 服务器端口                   */
    char acServerAddress[256];                           /*!< 服务器域名/IP地址 */
    char acProtocol[32];                                /*!< 服务器访问协议，范围:[http,ftp] */
    char acAccessToken[256];                         	/*!< 访问令牌    */
 
    char acResvered[1024];
}HY_DVR_RTMP_REGISTER_SERVER_INFO;

/*!
*  @struct  tagHY_DVR_RTMP_REGISTER_DEVICE_STATUS
*  @brief    RTMPRegister的设备状态
 */
typedef struct
{
    int iValid;
 
    int          iBlocked;                                      /*!< 设备状态;0-解锁状态，1-锁定状态                  */
 
    char         szResvered[128];
}HY_DVR_RTMP_REGISTER_DEVICE_STATUS;


typedef struct tagHY_DVR_ONVIF_CFG
{
    int nValid; //结构体有效性，0表示无效，1表示有效
 
    int nChannelID;
    int nEnable;            //是否启用onvif配置；0,未启用，1启用
    int nOnvifAuthEnable;   //是否启用onvif鉴权，onvif鉴权开启、禁用同时联动控制RTSP的鉴权为开启、禁用；0,未启用，1启用
 
    char cReserve[1024]; //保留字段
}HY_DVR_ONVIF_CFG, *LPHY_DVR_ONVIF_CFG;


/*!
*  @struct  tagHY_DVR_EXTERNAL_PTZ_AF_BOARD_TRACK_CONFIG
*  @brief    外置AF机芯云台跟踪配置结构
 */
typedef struct tagHY_DVR_EXTERNAL_PTZ_AF_BOARD_TRACK_CONFIG
{
	int iValid;
	
	int iEnable;     //  是否开启跟踪；0-否，1-是
		
	char acReserved[128];
}HY_DVR_EXTERNAL_PTZ_AF_BOARD_TRACK_CONFIG, *LPHY_DVR_EXTERNAL_PTZ_AF_BOARD_TRACK_CONFIG;


/*!
*  @struct  tagHY_DVR_DEVICE_DIY_MODE_CFG
*  @brief    设备特殊模式结构体
 */
typedef struct tagHY_DVR_DEVICE_DIY_MODE_CFG
{
    int iValid;
     
    int iStreamBandWidth;     // 流带宽；0-默认分辨率和码率，1-超清低带宽模式
         
    char acReserved[256];
}HY_DVR_DEVICE_DIY_MODE_CFG, *LPHY_DVR_DEVICE_DIY_MODE_CFG;


/*!
*  @struct  tagHY_DVR_DEVICE_3D_POSITIONING_COORD
*  @brief    3D定位区域(矩形)坐标信息
 */
typedef struct tagHY_DVR_DEVICE_3D_POSITIONING_COORD
{
	int bValid;
	
	HY_DVR_POINT stPointCenter;										/*!< 3D定位区域中心点坐标    */
	HY_DVR_POINT stPointLeftTop;									/*!< 3D定位区域(矩形)左上点坐标 */
	HY_DVR_POINT stPointRightTop;									/*!< 3D定位区域(矩形)右上点坐标 */
	HY_DVR_POINT stPointRightBottom;								/*!< 3D定位区域(矩形)右下点坐标 */
	HY_DVR_POINT stPointLeftBottom;									/*!< 3D定位区域(矩形)左下点坐标 */
}HY_DVR_DEVICE_3D_POSITIONING_COORD, *LPHY_DVR_DEVICE_3D_POSITIONING_COORD;

typedef struct tagHY_DVR_DEVICE_3D_POSITIONING_CFG
{
	int bValid;										/*!< 本结构体是否有效，0表示无效 */
	
	int iOperatorType;													/*!< 3D定位操作类型；0-点控、1-放大、2-缩小 */
	HY_DVR_DEVICE_3D_POSITIONING_COORD st3DPositionCoord;				/*!< 3D定位坐标信息 */
	int nChannel;														/*!< 通道号 */
 
    char szReserved[252];
} HY_DVR_DEVICE_3D_POSITIONING_CFG, *LPHY_DVR_DEVICE_3D_POSITIONING_CFG;


typedef struct tagHY_DVR_GB28181_CUSTOM_INFO
{
	int bValid;										/*!< 本结构体是否有效，0表示无效 */
	
	int iFactoryType;								/*!< 厂家类型; 0-通用、1-新疆定制 */
 
    char szReserved[128];
} HY_DVR_GB28181_CUSTOM_INFO, *LPHY_DVR_GB28181_CUSTOM_INFO;


//氛围灯配置
typedef struct tagHY_DVR_ATMOSPHERE_LIGHT_CFG
{
	int bValid;

	int iEnable;								/* 是否使能氛围灯；0-否，1-是 */
	int iModeType;								/* 氛围灯配置模式；0-红灯，1-蓝灯，2-绿灯，3-循环 */

	char cReserve[128];
}HY_DVR_ATMOSPHERE_LIGHT_CFG, *LPHY_DVR_ATMOSPHERE_LIGHT_CFG;


/*!
*  @struct  tagHY_DVR_BFJ_PASSWORD_AUTH_CFG
*  @brief   贝福佳密码认证配置信息
*  @explain 
*/
typedef struct tagHY_DVR_BFJ_PASSWORD_AUTH_CFG
{
	int nValid;												/*!< 本结构体是否有效，0表示无效 */

	char acAuthPassword[32];								/*!< 认证密码, 输入参数，贝福佳定制, 暂不考虑密码长度、特殊字符等其他情况 */

	char outReserve[64];
} HY_DVR_BFJ_PASSWORD_AUTH_CFG, *LPHY_DVR_BFJ_PASSWORD_AUTH_CFG;


/*!
*  @struct  tagHY_DVR_BFJ_SECURE_PASSWORD_CFG
*  @brief   贝福佳安全密码配置信息
*  @explain 
*/
typedef struct tagHY_DVR_BFJ_SECURE_PASSWORD_CFG
{
	int nValid;												/*!< 本结构体是否有效，0表示无效 */

	char acSecurePassword[32];								/*!< 安全密码，输入参数，贝福佳定制, 暂不考虑密码长度、特殊字符等其他情况 */
	char acOldPassword[32];									/*!< 旧密码，输出参数，贝福佳定制, 暂不考虑密码长度、特殊字符等其他情况 */

	char szReserve[64];
} HY_DVR_BFJ_SECURE_PASSWORD_CFG, *LPHY_DVR_BFJ_SECURE_PASSWORD_CFG;


/*!
*  @struct  tagHY_DVR_BFJ_WIFI_PASSWORD_CFG
*  @brief   贝福佳WIFI密码配置信息
*  @explain 
*/
typedef struct tagHY_DVR_BFJ_WIFI_PASSWORD_CFG
{
	int nValid;												/*!< 本结构体是否有效，0表示无效 */

	char acWIFIPassword[32];								/*!< WIFI密码，贝福佳定制, 暂不考虑密码长度、特殊字符等其他情况 */
	char acOldPassword[32];									/*!< 旧密码，贝福佳定制, 暂不考虑密码长度、特殊字符等其他情况 */

	char szReserve[64];
} HY_DVR_BFJ_WIFI_PASSWORD_CFG, *LPHY_DVR_BFJ_WIFI_PASSWORD_CFG;


typedef struct tagHY_DVR_PEOPLE_STATICS_AREA_RULE
{
	int iValid;

	int iEnable;										/*!< 使能; 0-去使能，1-使能 */
	int iMoreThenNumAlarmEnable;						/*!< 报警人数下限使能；0-去使能，1-使能 */
	int iMoreThenNumAlarm;								/*!< 报警人数下限，0-50 */
	int iLessThenNumAlarmEnable;						/*!< 报警人数上限使能；0-去使能，1-使能 */
	int iLessThenNumAlarm;								/*!< 报警人数上限，50-100 */
	int iAlarmStatisticsPeriod;							/*!< 报警统计周期，0-600 */
	HY_DVR_POINT_V2 stPolygon[MAX_POLYGON_LINE_NUM];	/*!< 布防区域，n个点表示n边形。例:8个点表示8边形 */

	char szReserved[128];
}HY_DVR_PEOPLE_STATICS_AREA_RULE, *LPHY_DVR_PEOPLE_STATICS_AREA_RULE;

typedef struct tagHY_DVR_PEOPLE_STATICS_ALARM
{
	int iValid;

	int iBroadcastCount;				/*!< 播放次数 */
	int iBroadcastVolume;				/*!< 播放音量,1-100 */
	int iOverNumAlarmVoiceEnable;					/*!< 超员报警语音使能 */
	int iOverNumAlarmVoiceType; 	/*!< 超员报警语音类型; 0:自定义,1:警报,2:犬吠,3:警戒区域,4:监控区域,5:欢迎光临,
                        6:深水警告,7:私人领地,8:危险区域,9:贵重物品,10:高处危险,
                        11:私家价位,12:叮咚,13:有口罩,14:无口罩,15:注意私人物品,
                        16:抓小偷,17:救命,18:分类,19:可回收垃圾,20:家庭食物垃圾
                        21:危险废物,22:残余垃圾,23:全部分类,24:车形自定义,25:禁止停车 */
	int iLessNumAlarmVoiceEnable;					/*!< 少员报警语音使能 */
	int iLessNumAlarmVoiceType; 	/*!< 少员报警语音类型; 0:自定义,1:警报,2:犬吠,3:警戒区域,4:监控区域,5:欢迎光临,
                        6:深水警告,7:私人领地,8:危险区域,9:贵重物品,10:高处危险,
                        11:私家价位,12:叮咚,13:有口罩,14:无口罩,15:注意私人物品,
                        16:抓小偷,17:救命,18:分类,19:可回收垃圾,20:家庭食物垃圾
                        21:危险废物,22:残余垃圾,23:全部分类,24:车形自定义,25:禁止停车 */
	int iDayMode;				/*!< 0:夜晚模式,1:白天模式,2:不启用此模式（AllDay字段有效，当字段不为disable时，AllDay字段无效）当设置为此模式时，IPC依赖自身传感器判断当前环境是白天还是夜晚*/
	HY_DVR_SCHEDTIME struSchedTime;				/*!< 语音报警布防计划(web用) */
	HY_DVR_SCHEDULE_TIMESLOT stSchedTimeSlot[SCHEDULE_TIMESLOT_MAX_NUM];	/*!< 布防时间计划(app用) */

	char acReserved[256];
} HY_DVR_PEOPLE_STATICS_ALARM, *LPHY_DVR_PEOPLE_STATICS_ALARM;

#define MAX_AREA_NUM 6
typedef struct tagHY_DVR_PEOPLE_STATICS_UI_DESIGN_CFG
{
	int iValid;

	int iEnable;					/*!< 使能 */
	int iSensitivity;				/*!< 灵敏度，取值范围1~100：其中30为低，60为中，90为高 */
	int iDetectArea;				/*!< 显示区域区.0-不显示，1-显示 */
	int iTargetFrame;				/*!< 显示目标框.0-不显示，1-显示 */
	int iStatisticalResults;		/*!< 显示统计人数.0-不显示，1-显示 */
	int iIsPushAlarmPicture;		/*!< 是否推送告警图.0-不推送，1-推送 */
	int iStaticsInterval;			/*!< 统计间隔，1-600s */
	HY_DVR_PEOPLE_STATICS_AREA_RULE stAreaRules[MAX_AREA_NUM];	/*!< 区域规则 */
	HY_DVR_HANDLEEXCEPTION struHandleType;		/*!< 处理方式  */
	HY_DVR_SCHEDTIME struSchedTime;		/*!< 事件布防计划(web用) */
	HY_DVR_SCHEDULE_TIMESLOT stSchedTimeSlot[SCHEDULE_TIMESLOT_MAX_NUM];	/*!< 布防时间计划(app用) */
	HY_DVR_PEOPLE_STATICS_ALARM stVoiceAlarm;	/*!< 语音报警 */

	char acReserved[1024];
} HY_DVR_PEOPLE_STATICS_UI_DESIGN_CFG, *LPHY_DVR_PEOPLE_STATICS_UI_DESIGN_CFG;


typedef struct tagHY_DVR_SECURITY_SAVE_QUESTION_CFG
{
	int nValid;

	int  ID;							//ID，从1开始
	char acQuestion[2048];				//问题
	char acAnswer[2048];				//答案

	char szReserved[128];
}HY_DVR_SECURITY_SAVE_QUESTION_CFG, *LPHY_DVR_SECURITY_SAVE_QUESTION_CFG;

#define SAVE_QUESTION_ANSWER_MAX_COUNT 6
typedef struct tagHY_DVR_SECURITY_SAVE_QUESTION_LIST
{
	int nValid;

	HY_DVR_SECURITY_SAVE_QUESTION_CFG stSaveQuestionAnswer[SAVE_QUESTION_ANSWER_MAX_COUNT];			//问题-答案集
	char acModifyUsername[32];                            //修改的用户名
	char acModifyPassword[32];                            //修改的密码

	char szReserved[128];
}HY_DVR_SECURITY_SAVE_QUESTION_LIST, *LPHY_DVR_SECURITY_SAVE_QUESTION_LIST;


typedef struct tagHY_DVR_PEOPLE_STATISTICS_MINUTE
{
	int nValid;
	int iArea1Number;						/*!< 区域1人数 */
	int iArea2Number;						/*!< 区域2人数 */
	int iArea3Number;						/*!< 区域3人数 */

	char azReserved[64];
} HY_DVR_PEOPLE_STATISTICS_MINUTE, *LPHY_DVR_PEOPLE_STATISTICS_MINUTE;

/** @brief 每小时分钟数 */
#define DVR_DAILY_MINUTE_NUM 60
typedef struct tagHY_DVR_PEOPLE_STATISTICS_HOUR
{
	int nValid;

	int iArea1Number;						/*!< 区域1人数 */
	int iArea2Number;						/*!< 区域2人数 */
	int iArea3Number;						/*!< 区域3人数 */
	HY_DVR_PEOPLE_STATISTICS_MINUTE stStatisticMinuteList[DVR_DAILY_MINUTE_NUM]; /*!< 人数统计数据 */
} HY_DVR_PEOPLE_STATISTICS_HOUR, *LPHY_DVR_PEOPLE_STATISTICS_HOUR;

typedef struct tagHY_DVR_PEOPLE_STATISTIC_DAY
{
	int bValid;															   /*!< 本结构体是否有效，0表示无效  */
	HY_DVR_TIME stStatisticDate;										   /*!< 统计日期 */
	HY_DVR_PEOPLE_STATISTICS_HOUR stStatisticHourList[DVR_DAILY_HOUR_NUM]; /*!< 统计日期的人数统计数据 */
} HY_DVR_PEOPLE_STATISTICS_DAY, *LPHY_DVR_PEOPLE_STATISTICS_DAY;

typedef struct tagHY_DVR_PEOPLE_STATISTICS
{
	int bValid;														   /*!< 本结构体是否有效，0表示无效  */
	HY_DVR_TIME stStatisticStartDate;								   /*!< 输入参数：统计起始日期 */
	HY_DVR_TIME stStatisticEndDate;									   /*!< 输入参数：统计截止日期 */
	HY_DVR_PEOPLE_STATISTICS_DAY stStatisticDayList[DVR_WEEK_DAY_NUM]; /*!< 输出参数：7天的人数统计数据 */
} HY_DVR_PEOPLE_STATISTICS, *LPHY_DVR_PEOPLE_STATISTICS;


#define HY_DVR_P6SCMS_FEATURES_MASK_GB 0x1
#define HY_DVR_P6SCMS_FEATURES_MASK_P6SEVENT_MQTT 0x2
#define HY_DVR_P6SCMS_FEATURES_MASK_RTMP 0x4
#define HY_DVR_P6SCMS_FEATURES_MASK_P6SEVENT_HTTP 0x8
typedef struct tagHY_DVR_P6SCMS_CONFIG
{
	int bValid;

	int nEnable;				// 0.不启用，1.启用
	int nProtocolType; // 0,http,1.https
	char cServerDomain[256]; //CMS平台服务器地址（可以为IP或者域名）
	int nServerPort; //CMS平台服务器端口
	char cPath[128]; //服务器请求路径
	char cUserName[64];
	char cPassword[64];
	int nSupportedFeaturesMask;//掩码，每一位代表一个功能，为0表示该功能不开启，为1表示该功能开启，掩码值参考前缀为HY_DVR_P6SCMS的宏
	int nConnectionStatus; //连接状态，该参数由设备赋值，对web来说是只读参数，0.未知，1.成功，2.失败
	int nErrorCode; //P6SCMS库里面返回的错误码
	int nLastReqTimestamp; //上次请求时间，时间戳，单位是秒

	char cReserve[256];		// 保留字段
}HY_DVR_P6SCMS_CONFIG, *LPHY_DVR_P6SCMS_CONFIG;

typedef struct tagHY_DVR_P6SCMS_SERVER_CONNECT_TEST
{
    int bValid;

    char acInputReserved[256];              /*!< 输入参数保留字段 */

    char acStatusCode[32];                  /*!< 输出参数,服务器返回状态码 */
    char acTime[32];                            /*!< 输出参数,服务器响应时间 */
    char acInfo[1024];                          /*!< 输出参数,服务器返回的信息(使用base64编码) */

    char acOutputReserved[256];        /*!< 输出参数保留字段 */
}HY_DVR_P6SCMS_SERVER_CONNECT_TEST, *LPHY_DVR_P6SCMS_SERVER_CONNECT_TEST;


typedef struct tagHY_DVR_DST_INFO
{
	int bValid;
	int isInDST;		/*!< 是否处于夏令时中，0-否，1-是 */
	int dstOffset;		/*!< 夏令时偏移，单位分钟*/
	char cReserve[64];
}HY_DVR_DST_INFO, *LPHY_DVR_DST_INFO;

typedef struct tagHY_DVR_LIGHT_CONTROL_TIMESLOT_CFG
{   
    int iValid;
    int iEnable;                    /*!< 时间段是否有效; 0-否,1-是   */
    HY_DVR_TIME_V2 stStart;            /*!< 起始时间   */
    HY_DVR_TIME_V2 stEnd;            /*!< 结束时间   */
    
    char szReserved[128];
}HY_DVR_LIGHT_CONTROL_TIMESLOT_CFG, *LPHY_DVR_LIGHT_CONTROL_TIMESLOT_CFG;


typedef struct tagHY_DVR_LIGHT_CONTROL_CFG
{
	int iValid;

	int iEnable;					/*!< 灯具开关;0-关，-开 */
	int iHasPersonLightBright;		/*!< 有人灯具亮度,[60-100] */
	int iNoPersonLightBright;		/*!< 无人灯具亮度, [0-40] */
	int iRadarSensorEnable;			/*!< 雷达感应开关;0-关，-开 */
	int iIntellectOnLightEnable;	/*!< 智能开灯开关;0-关，1-开；开启后白天灯具不亮，夜间没人时灯具低亮，有人时灯具高亮 */
	int iTimeOnLightEnable;			/*!< 定时开灯开关;0-关，1-开; 开启后按stAllowLightTimeSlot设置的时间段灯具开灯常亮 */
	HY_DVR_LIGHT_CONTROL_TIMESLOT_CFG stDayTimeSlot;	/*!< 允许开灯时间段 的白天时间段，iTimeOnLightEnable=1时有效 */
    HY_DVR_LIGHT_CONTROL_TIMESLOT_CFG stNightTimeSlot;	/*!< 允许开灯时间段 的夜晚时间段，iTimeOnLightEnable=1时有效 */
	HY_DVR_SCHEDULE_TIMESLOT stAllowLightTimeSlotList[SCHEDULE_TIMESLOT_MAX_NUM]; /*!< 允许开灯时间段，iTimeOnLightEnable=1时有效 */

	char szReserved[1024];
}HY_DVR_LIGHT_CONTROL_CFG, *LPHY_DVR_LIGHT_CONTROL_CFG;


/*!
*  @struct    tagHY_DVR_HTTPS_CFG
*  @brief    HTTPS配置
*  @illustrate
*/
typedef struct tagHY_DVR_HTTPS_CFG
{
    int iValid;								/*!< 本结构体是否有效，0表示无效 */

    int iEnable;							/*!< 是否使能HTTPS认证配置；0，否，1，是 */
    int iHTTPSPort;							/*!< https端口 */
    int iTLSVersion;						/*!< TLSV版本；0表示TLSV1.1，1表示TLSV1.2  */

    char acReserved[256];
}HY_DVR_HTTPS_CFG, *LPHY_DVR_HTTPS_CFG;


/*!
*  @struct    tagHY_DVR_HTTPS_CERTIFICATION_INFO
*  @brief    HTTPS证书信息
*  @illustrate
*/

typedef struct tagHY_DVR_HTTPS_CERTIFICATION_INFO
{
    int iValid;								/*!< 本结构体是否有效，0表示无效 */

    int iCertificationStatus;	/*!< 证书状态；1.无证书:完全没有证书
                                2.未安装:证书已经上传（上传目录或者创建目录），但是没有安装（安装目录），有可能存在在上传或者生成过程中突然断电，导致证书在上传目录或者创建目录，但是没有移动到或者链接到安装目录），
                                3.已安装:证书已经正确移动到或者链接到了安装目录，且证书是有效
                                4.证书即将过期:证书已正确安装，证书过期时间在0-30天内
                                5.证书已过期，证书正确安装，证书已经过期
                                6.证书无效（安装的证书是一个无效的证书文件）， */
    char acCertificateHolderInfo[2048];		/*!< 证书持有者信息 */
    char acCertificateDetailContent[4096];	/*!< 证书详细内容 */

    char acReserved[256];
}HY_DVR_HTTPS_CERTIFICATION_INFO, *LPHY_DVR_HTTPS_CERTIFICATION_INFO;


/*!
*  @struct    tagHY_DVR_HTTPS_CERTIFICATE_CREATE_CFG
*  @brief    HTTPS认证创建配置
*  @illustrate
*/
typedef struct tagHY_DVR_HTTPS_CERTIFICATE_CREATE_CFG
{
    int iValid;									/*!< 本结构体是否有效，0表示无效 */

    char acRegion[64];							/*!< 设备所在区域 */
    char acAddress[256];						/*!< 设备IP地址或域名 */
    int iValidityPeriod;						/*!< 证书有效期；单位：天，范围[1-5000] */
    char acProvince[64];						/*!< 设备所在省份 */
    char acLocation[128];						/*!< 设备所在地址 */
    char acOrganization[64];					/*!< 设备组织 */
    char acOrganizationUnit[128];				/*!< 设备组织单位 */

    char acReserved[256];
}HY_DVR_HTTPS_CERTIFICATE_CREATE_CFG, *LPHY_DVR_HTTPS_CERTIFICATE_CREATE_CFG;


/*!
*  @struct    tagHY_DVR_HTTPS_CERTIFICATE_DELETE_CFG
*  @brief    HTTPS认证删除配置
*  @illustrate
*/
typedef struct tagHY_DVR_HTTPS_CERTIFICATE_DELETE_CFG
{
    int iValid;                                 /*!< 本结构体是否有效，0表示无效 */

    char acReserved[256];
}HY_DVR_HTTPS_CERTIFICATE_DELETE_CFG, *LPHY_DVR_HTTPS_CERTIFICATE_DELETE_CFG;



/*!
*  @struct    tagHY_DVR_RTSPS_CFG
*  @brief    RTSPs配置 (同https使用tls加密,放在HY_DVR_GET_HTTPS_CFG主命令下)
*  @illustrate
*/
typedef struct tagHY_DVR_RTSPS_CFG
{
    int iValid;                             /*!< 本结构体是否有效，0表示无效 */

    int iEnable;                            /*!< 是否使能RTSPs配置认证配置；0，否，1，是 */
    int iRTSPSPort;                         /*!< rtsps端口 */

    char acReserved[256];
}HY_DVR_RTSPS_CFG, *LPHY_DVR_RTSPS_CFG;


/*!
*  @struct    tagHY_DVR_CERTIFICATION_CFG
*  @brief    证书配置
*  @illustrate
*/
typedef struct tagHY_DVR_CERTIFICATION_CFG
{
    int iValid;                             /*!< 本结构体是否有效，0表示无效 */

    char acCertificationFilePath[256];         /*!< 证书文件路径 */
    char acCertificationKeyFilePath[256];      /*!< 秘钥文件路径 */

    char acReserved[1024];
}HY_DVR_CERTIFICATION_CFG, *LPHY_DVR_CERTIFICATION_CFG;



typedef struct tagHY_DVR_PERSON_CENTER
{
    int iValid;

    int iEnable;                                                        /*!< 是否使能人形居中;0-否,1-是  */

    char szReserved[128];
}HY_DVR_PERSON_CENTER, *LPHY_DVR_PERSON_CENTER;

typedef struct tagHY_DVR_PERSON_CENTER_CFG
{
    int iValid;

    HY_DVR_PERSON_CENTER stPersonCenter[DVR_MAX_VIDEOIN_NUM];

    char cRevered[1024];
}HY_DVR_PERSON_CENTER_CFG, *LPHY_DVR_PERSON_CENTER_CFG;


typedef struct tagHY_DVR_TIME_REMIND_PLAN
{
    int iValid;					/* 是否有效;0-否,1-是 */

    int iEnable;				/* 是否使能;0-否,1-是 */
    int iID;					/* 序号ID,从1开始 */
    int iVoiceType;				/*!<  语音类型; 0:自定义, 1:卡农, 2:爱丽丝, 3:电话铃声, 4:闹铃, 5:鸟鸣  */
    int iBroadcastCount;		/* 播报次数 */
    int iBroadcastVolume;		/*!< 播报音量；取值1-100，默认90 */
    char acPeriod[8];			/* 周期,下标从0-7分别表示周日、周一、周二、周三、周四、周五、周六;数组元素取值1表示当天设置定时计划,取值0表示不设置 */
    char acRemindTime[32];		/* 提醒时间,格式:YYYY-MM-DDThh:mm:ss */
    char acRemindName[64];		/* 提醒名称 */
	char acVoiceTypeName[128];	/* 语音类型名称 */

    char acReserved[128];		//保留字段
}HY_DVR_TIME_REMIND_PLAN, *LPHY_DVR_TIME_REMIND_PLAN;

#define MAX_TIME_REMIND_NUM 8
typedef struct tagHY_DVR_TIME_REMIND_CFG
{
    int iValid;

	int iCurrentCustomVoiceValidCount;  /* 当前自定义语音有效个数[0-3] */
    HY_DVR_TIME_REMIND_PLAN stTimeRemindPlanList[MAX_TIME_REMIND_NUM];    /* 定时提醒计划列表 */

    char acReserved[128];        //保留字段
}HY_DVR_TIME_REMIND_CFG, *LPHY_DVR_TIME_REMIND_CFG;


typedef struct tagHY_DVR_TIME_REMIND_AUDIO_INFO
{
    int iValid;                /* 是否有效;0-否,1-是 */

    int iID;                   /* 语音序号ID,从1开始 */
    char acName[128];          /* 语音名称 */

    char acReserved[128];        //保留字段
}HY_DVR_TIME_REMIND_AUDIO_INFO, *LPHY_DVR_TIME_REMIND_AUDIO_INFO;

#define MAX_TIME_REMIND_AUDIO_NUM 8
/*!
*  @struct  tagHY_DVR_TIME_REMIND_AUDIO_INFO_LIST
*  @brief   定时提醒语音信息(P3MU项目  支持了自定义语音的添加、删除、查询)
*/
typedef struct tagHY_DVR_TIME_REMIND_AUDIO_INFO_LIST
{
    int iValid;

    int iCurrentCustomVoiceValidCount;  /* 当前自定义语音有效个数[0-3] */
    HY_DVR_TIME_REMIND_AUDIO_INFO stTimeRemindAudioInfoList[MAX_TIME_REMIND_AUDIO_NUM];    /* 定时提醒语音信息列表 */

    char acReserved[128];        //保留字段
}HY_DVR_TIME_REMIND_AUDIO_INFO_LIST, *LPHY_DVR_TIME_REMIND_AUDIO_INFO_LIST;

typedef struct tagHY_DVR_TIME_REMIND_AUDIO_PLAY_INFO
{
    int iValid;                /* 是否有效;0-否,1-是 */

    int iVoiceType;            /* 语音类型; 0:自定义语音1, 1:卡农, 2:爱丽丝, 3:电话铃声, 4:闹铃, 5:鸟鸣; 6:自定义语音2; 7:自定义语音3  */

    char acReserved[64];        //保留字段
}HY_DVR_TIME_REMIND_AUDIO_PLAY_INFO, *LPHY_DVR_TIME_REMIND_AUDIO_PLAY_INFO;


typedef struct tagHY_DVR_SCREEN_MANAGEMENT_CFG
{
    int iValid;

    int iEnable;            /* 是否使能屏幕管理;0-否,1-是 */
    int iScreenCloseTime;    /* 息屏时间,单位:秒; 0表示永不息屏 */
    int iScreenLight;        /* 屏幕亮度,[0-100] */
    int iCurrentScreenThemeType;        /* 屏幕亮度,[0-9] */

    char acReserved[256];        //保留字段给大
}HY_DVR_SCREEN_MANAGEMENT_CFG, *LPHY_DVR_SCREEN_MANAGEMENT_CFG;


typedef struct tagHY_DVR_TCPRELAY_ENABLE
{
    int iValid;
	int iEnable;	// 0, 关闭tctrelay,1，开启tcprelay
}HY_DVR_TCPRELAY_ENABLE, *LPHY_DVR_TCPRELAY_ENABLE;


#endif

/*
URL，命令字对应

/Security/UserAuth
GET
HY_DVR_GET_DEVICEINFO_EX, DEVICEINFO_EX_ALL
HY_DVR_GET_USERCFG, USERCFG_ALL

/Security/UserAuthV2
PUT
None

/Security/ModifyPasswordByTempSecretkey
PUT
HY_DVR_GET_USERCFG, USERCFG_ALL HY_DVR_USER_CFG
HY_DVR_SET_USERCFG, USERCFG_ALL HY_DVR_USER_CFG

/Security/BaseConfig
GET
HY_DVR_GET_SECURITY_CONFIG, SECURITY_WHITE_LISTS HY_DVR_WHITE_LISTS
PUT
HY_DVR_GET_SECURITY_CONFIG, SECURITY_WHITE_LISTS HY_DVR_WHITE_LISTS

/Security/WhiteLists
GET
HY_DVR_GET_SECURITY_CONFIG, SECURITY_WHITE_LISTS HY_DVR_WHITE_LISTS

/Security/WhiteList
GET
HY_DVR_GET_SECURITY_CONFIG, SECURITY_WHITE_RULE HY_DVR_WHITE_RULE
PUT
HY_DVR_SET_SECURITY_CONFIG, SECURITY_WHITE_RULE HY_DVR_WHITE_RULE
POST
HY_DVR_SET_SECURITY_CONFIG, SECURITY_WHITE_RULE_ADD HY_DVR_WHITE_RULE
DEL
//清空
HY_DVR_SET_SECURITY_CONFIG, SECURITY_WHITE_RULE_CLEAR HY_DVR_WHITE_RULE
//删除
HY_DVR_SET_SECURITY_CONFIG, SECURITY_WHITE_RULE_DEL HY_DVR_WHITE_RULE

/Network/PPPOE
GET
HY_DVR_GET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG
PUT
HY_DVR_GET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG
HY_DVR_SET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG

/Network/PPPOE/Config
GET
HY_DVR_GET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG
PUT
HY_DVR_GET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG
HY_DVR_SET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG

/Network/PPPOE/Status
GET
HY_DVR_GET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG

/Network/DDNS/ServerList
GET
HY_DVR_GET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG

/Network/DDNS/Config
GET
HY_DVR_GET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG
PUT
HY_DVR_SET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG

/Network/DDNS/Status
GET
HY_DVR_GET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG

/Network/DDNS
GET
HY_DVR_GET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG
PUT
HY_DVR_SET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG

/Network/UPNP/Config
GET
HY_DVR_GET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG
PUT
HY_DVR_SET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG

/Network/UPNP/Status
GET
HY_DVR_GET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG

/Network/UPNP
GET
HY_DVR_GET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG
PUT
HY_DVR_GET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG
HY_DVR_SET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG

/Network/EMail
GET
HY_DVR_GET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG
HY_DVR_GET_NETCFG_EX, NETCFGEX_ALL HY_DVR_NET_CFG_EX

/Network/Port
GET
HY_DVR_GET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG
PUT
HY_DVR_GET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG
HY_DVR_SET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG

/Network/P2P
GET
HY_DVR_GET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG
PUT
HY_DVR_GET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG
HY_DVR_SET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG

/Network/P2PV2
GET
HY_DVR_GET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG
HY_DVR_GET_NETCFG_EX, NETCFGEX_ALL HY_DVR_NET_CFG_EX
PUT
HY_DVR_GET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG
HY_DVR_SET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG

/Network/FTP
GET
HY_DVR_GET_NETCFG, NETCFG_FTP_CONF_V2 HY_DVR_FTP_CONF_V2
PUT
HY_DVR_GET_NETCFG, NETCFG_FTP_CONF_V2 HY_DVR_FTP_CONF_V2
HY_DVR_SET_NETCFG, NETCFG_FTP_CONF_V2 HY_DVR_FTP_CONF_V2

/Network/Interfaces/1/IPAddress
GET
HY_DVR_GET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG
PUT
HY_DVR_GET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG
HY_DVR_SET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG

/Network/Interfaces/1/IPAddress/DHCP
GET
HY_DVR_GET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG
PUT
HY_DVR_GET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG
HY_DVR_SET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG

/Network/Interfaces/1/IPAddress/DNS
GET
HY_DVR_GET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG
PUT
HY_DVR_GET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG
HY_DVR_SET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG

/Network/Interfaces/2/IPAddress
GET
HY_DVR_GET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG
PUT
HY_DVR_GET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG
HY_DVR_SET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG

/Network/Interfaces/2/WIFIAccessPointList
GET
HY_DVR_GET_NETCFG, NETCFG_WLAN_ACCESSPOINTS_V2 HY_DVR_WLAN_ACCESSPOINT_CFG_V2
if v2 fail
HY_DVR_GET_NETCFG, NETCFG_WLAN_ACCESSPOINTS HY_DVR_WLAN_ACCESSPOINT_CFG

/Network/Interfaces/2/Wireless
GET
HY_DVR_GET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG

/Network/Interfaces/2/WirelessEx
GET
HY_DVR_GET_NETCFG, NETCFG_WLAN_CONNECTION_INFO_EX
PUT
HY_DVR_GET_NETCFG, NETCFG_WLAN_CONNECTION_INFO_EX
HY_DVR_SET_NETCFG, NETCFG_WLAN_CONNECTION_INFO_EX

/Network/Interfaces/2/WIFIStatus
GET
HY_DVR_GET_NETCFG, NETCFG_ALL

/Network/Interfaces/1
/Network/Interfaces/2
GET
HY_DVR_GET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG
HY_DVR_GET_NETCFG_EX, NETCFGEX_ALL HY_DVR_NET_CFG_EX

/Network/Interfaces/3/4GCardCfg
GET
HY_DVR_GET_NETCFG, NETCFG_4G_NETCARD_CFG HY_DVR_4G_NETCARD_CONFIG
PUT
HY_DVR_GET_NETCFG, NETCFG_4G_NETCARD_CFG HY_DVR_4G_NETCARD_CONFIG
HY_DVR_SET_NETCFG, NETCFG_4G_NETCARD_CFG HY_DVR_4G_NETCARD_CONFIG

/Network/Interfaces/3/4GCardInfo
GET
HY_DVR_GET_NETCFG, NETCFG_4G_NETCARD_INFO HY_DVR_4G_NETCARD_INFO

/Network/Interfaces/3/4GCardTest
typedef int (*IF_RemoteDeviceControl) (DWORD dwUserID, eRemoteDeviceControl eOperationCode);    (DWORD)-1,eDevice4GCardTest

/Network/Interfaces
GET
HY_DVR_GET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG
HY_DVR_GET_NETCFG_EX, NETCFGEX_ALL HY_DVR_NET_CFG_EX

/Network
GET
HY_DVR_GET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG
HY_DVR_GET_NETCFG_EX, NETCFGEX_ALL HY_DVR_NET_CFG_EX

/Network/WifiParamTest
PUT
HY_DVR_SET_NETCFG, NETCFG_WIFI_CONNECT_PARAM_CHECK HY_DVR_WLAN_CONNECTION

/Network/WifiParamTestStatus
GET
HY_DVR_GET_NETCFG, NETCFG_WIFI_CONNECT_PARAM_CHECK_STATUS HY_DVR_WLAN_STATE_FOR_TEST

/Network/WhichNetworkCard
GET
HY_DVR_GET_NETCFG, NETCFG_WHICH_NETCARD_USED HY_DVR_WHICH_NETCARD_USED

/Network/CloudStorage
GET
HY_DVR_GET_CLOUD_STORAGE, CLOUD_STORAGE_INFO HY_DVR_CLOUD_STORAGE
PUT
HY_DVR_GET_CLOUD_STORAGE, CLOUD_STORAGE_INFO HY_DVR_CLOUD_STORAGE
HY_DVR_SET_CLOUD_STORAGE, CLOUD_STORAGE_INFO HY_DVR_CLOUD_STORAGE

/Network/CloudStorage/BaseConfig
GET
HY_DVR_GET_CLOUD_STORAGE, CLOUD_STORAGE_BASE_CONFIG HY_DVR_CLOUD_STROAGE_BASE_CONFIG
PUT
HY_DVR_SET_CLOUD_STORAGE, CLOUD_STORAGE_BASE_CONFIG HY_DVR_CLOUD_STROAGE_BASE_CONFIG

/Network/CloudStorage/OpenStatus
GET
HY_DVR_GET_CLOUD_STORAGE, CLOUD_STORAGE_OPEN_STATUS HY_DVR_CLOUD_STROAGE_OPEN_STATUS
PUT
HY_DVR_SET_CLOUD_STORAGE, CLOUD_STORAGE_OPEN_STATUS HY_DVR_CLOUD_STROAGE_OPEN_STATUS

/Network/CloudStorage/RecordPlan
GET
HY_DVR_GET_CLOUD_STORAGE, CLOUD_STORAGE_RECORD_PLAN HY_DVR_CLOUD_STROAGE_RECORD_PLAN
PUT
HY_DVR_SET_CLOUD_STORAGE, CLOUD_STORAGE_RECORD_PLAN HY_DVR_CLOUD_STROAGE_RECORD_PLAN

/Network/CloudStorage/BaseStatus
GET
HY_DVR_GET_CLOUD_STORAGE, CLOUD_STORAGE_BASE_STATUS HY_DVR_CLOUD_STORAGE_BASE_STATUS

/Network/CloudStorage/BaseStatusV2
GET
HY_DVR_GET_CLOUD_STORAGE, CLOUD_STORAGE_BASE_STATUS_V2

/Network/CloudStorage/BusinessInfo
GET
HY_DVR_GET_CLOUD_STORAGE, CLOUD_STORAGE_ORDER_INFO HY_DVR_CLOUD_STORAGE_ORDER_INFO
HY_DVR_GET_CLOUD_STORAGE, CLOUD_STORAGE_PACKAGE_INFO HY_DVR_CLOUD_STORAGE_PACKAGE_INFO
PUT
HY_DVR_SET_CLOUD_STORAGE, CLOUD_STORAGE_ORDER_INFO HY_DVR_CLOUD_STORAGE_ORDER_INFO
HY_DVR_SET_CLOUD_STORAGE, CLOUD_STORAGE_PACKAGE_INFO HY_DVR_CLOUD_STORAGE_PACKAGE_INFO

/Network/CloudStorage/WorkMode
GET
HY_DVR_GET_CLOUD_STORAGE_MODE, CLOUD_STORAGE_MODE
PUT
HY_DVR_SET_CLOUD_STORAGE_MODE, CLOUD_STORAGE_MODE

/Network/4GCardSelectCfg
GET
HY_DVR_GET_4G_CARD_SELECT_CFG, HY_DVR_4G_CARD_SELECT_CFG
SET
HY_DVR_SET_4G_CARD_SELECT_CFG, HY_DVR_4G_CARD_SELECT_CFG

/Network/4GCardCfgList
GET
HY_DVR_GET_4G_CARD_CFG, HY_DVR_4G_CARD_CFG
SET
HY_DVR_SET_4G_CARD_CFG, HY_DVR_4G_CARD_CFG

/System/Time
GET
HY_DVR_GET_DEVICECFG, DEVICECFG_ALL HY_DVR_DEVICE_CFG
HY_DVR_GET_SYSTIME, SYSTIME_ALL HY_DVR_TIME
PUT
HY_DVR_GET_DEVICECFG, DEVICECFG_ALL HY_DVR_DEVICE_CFG
HY_DVR_GET_SYSTIME, SYSTIME_ALL HY_DVR_TIME
HY_DVR_SET_DEVICECFG, DEVICECFG_ALL HY_DVR_DEVICE_CFG
HY_DVR_SET_SYSTIME, SYSTIME_ALL HY_DVR_TIME

/System/NTP
GET
HY_DVR_GET_DEVICECFG, DEVICECFG_ALL HY_DVR_DEVICE_CFG
PUT
HY_DVR_GET_DEVICECFG, DEVICECFG_ALL HY_DVR_DEVICE_CFG
HY_DVR_SET_DEVICECFG, DEVICECFG_ALL HY_DVR_DEVICE_CFG

/System/DeviceInfo
GET
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL HY_DVR_DEVICE_INFO
HY_DVR_GET_DEVICEINFO_EX, DEVICEINFO_EX_ALL HY_DVR_DEVICE_INFO_EX
PUT
HY_DVR_SET_DEVICEINFO, DEVICEINFO_ALL HY_DVR_DEVICE_INFO HY_DVR_DEVICE_CFG

/System/DeviceInfoV2
GET
HY_DVR_GET_DEVICEINFO, DEVICEINFO_V2 HY_DVR_DEVICE_INFO_V2
PUT
HY_DVR_SET_DEVICEINFO, DEVICEINFO_V2 HY_DVR_DEVICE_INFO_V2

/System/ActiveStatusInfoList
GET
HY_DVR_GET_DEVICECFG, DEVICECFG_ALL
HY_DVR_GET_DEVICEINFO_EX, DEVICEINFO_EX_ALL
HY_DVR_GET_BUFFER_SIZE, BUFFER_SIZE_ALL
HY_DVR_GET_ACTIVE_CFG, ACTIVE_CFG

/System/DeviceRunningInfo
GET
HY_DVR_GET_DEVICEINFO, DEVICE_RUNNING_INFO

/System/DeviceStatus
GET
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL HY_DVR_DEVICE_INFO
HY_DVR_GET_DEVICESTATE, DEVICESTATE_ALL HY_DVR_DEVICE_STATE

/System/Reboot
PUT
typedef int (*IF_RemoteDeviceControl) (DWORD dwUserID, eRemoteDeviceControl eOperationCode); dwUserID eDeviceReboot

/System/FactoryDefault
PUT
typedef int (*IF_RemoteDeviceControl) (DWORD dwUserID, eRemoteDeviceControl eOperationCode); dwUserID eDeviceSetDefault

/System/FactoryDefaultV2
PUT
typedef int (*IF_RemoteDeviceControl) (DWORD dwUserID, eRemoteDeviceControl eOperationCode); dwUserID eDeviceRestoreFactory

/System/SendTestEmail
PUT
typedef int (*IF_RemoteDeviceControl) (DWORD dwUserID, eRemoteDeviceControl eOperationCode); dwUserID eDeviceSendTestEmail

/System/SendTestFTP
PUT
typedef int (*IF_RemoteDeviceControl) (DWORD dwUserID, eRemoteDeviceControl eOperationCode); dwUserID eDeviceSendTestFTP

/System/RecoveryAdminPassword
PUT
typedef int (*IF_RemoteDeviceControl) (DWORD dwUserID, eRemoteDeviceControl eOperationCode); -3 eDeviceAdminDefaultPassword

/System/TryListenCurrentAlarmAudio
PUT
typedef int (*IF_RemoteDeviceControl) (DWORD dwUserID, eRemoteDeviceControl eOperationCode); -3 eDeviceTryListenAlarmAudio

/System/TryListenSelectedAlarmAudio
PUT
HY_DVR_SET_AUDIO_ALARM_CONFIG, AUDIO_ALARM_CONFIG_ALL_CHECK HY_AUDIO_ALARM_CONFIG

/System/DeviceReversePacket
PUT
typedef int (*IF_RemoteDeviceControl) (DWORD dwUserID, eRemoteDeviceControl eOperationCode); -3 eDeviceReversePacket

/System/AFBoardUpgradeStatus
GET
HY_DVR_GET_DEVICECFG, AF_BOARD_UPGRADE_STATUS

/System/BufferSize
GET
HY_DVR_GET_BUFFER_SIZE, BUFFER_SIZE_ALL
PUT
HY_DVR_GET_BUFFER_SIZE, BUFFER_SIZE_ALL
HY_DVR_SET_BUFFER_SIZE, BUFFER_SIZE_ALL

/System/DeviceWifiSTA2AP
PUT
typedef int (*IF_RemoteDeviceControl) (DWORD dwUserID, eRemoteDeviceControl eOperationCode); -1 eDeviceEnterAPMode

/System/DeviceWifiAP2STA
PUT
typedef int (*IF_RemoteDeviceControl) (DWORD dwUserID, eRemoteDeviceControl eOperationCode); -1 eDeviceEnterSTAMode

/System/Logging
GET
HY_DVR_GET_USERCFG, USERCFG_ALL HY_DVR_USER_CFG
typedef int (*IF_UserLogin) (eProtocolID eProtocol, DWORD * pUserID, const LPRemoteUserInfo pRemoteUserInfo);
typedef int (*IF_LogQueryCreate) (HANDLE * hIndex, DWORD dwUserID, const LPLogQueryInfo pQuery);
typedef int (*IF_LogQueryNext) (HANDLE hIndex, LPLogQueryResult pNext);
typedef int (*IF_LogQueryDestroy) (HANDLE hIndex);
typedef int (*IF_UserLogout) (DWORD dwUserID);

/System/Users
GET
HY_DVR_GET_USERCFG, USERCFG_ALL HY_DVR_USER_CFG
HY_DVR_GET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG

/System/DST
GET
HY_DVR_GET_DEVICECFG, DEVICECFG_ALL HY_DVR_DEVICE_CFG

/System/AlarmInState
GET
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL HY_DVR_DEVICE_INFO
typedef int (*IF_RemoteAlarminStateGet) (DWORD dwUserID, DWORD* dwChanNum, DWORD* dwStateBits);

/System/AlarmOutState
GET
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL HY_DVR_DEVICE_INFO
typedef int (*IF_RemoteAlarmoutStateGet) (DWORD dwUserID, DWORD* dwChanNum, DWORD* dwStateBits);

/Common/DeviceFileDownload
GET
typedef int (*IF_RemoteDownloadCreate) (HANDLE * hIndex, DWORD dwUserID, const LPRemoteDownloadParaInfo pRemoteDownloadParaInfo);
typedef int (*IF_RemoteDownloadDestroy) (HANDLE hIndex, const LPRemoteDownloadState pState);
typedef int (*IF_RemoteDownloadRead) (HANDLE hIndex, Buffer * pBuffer);

/Streams
GET
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL HY_DVR_DEVICE_INFO
HY_DVR_GET_COMPRESSCFG, COMPRESSCFG_ALL HY_DVR_COMPRESSION_CFG
HY_DVR_GET_COMPRESSCFG, COMPRESSCFG_CAPABILITY HY_DVR_CODEC_CAP
HY_DVR_GET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG
HY_DVR_GET_COMPRESSCFG, COMPRESSCFG_ALL_EX HY_DVR_COMPRESSION_CFG_EX
HY_DVR_GET_PICCFG, PICCFG_ADVANCED_CONF HY_DVR_ADVANCED_CHANNEL_CFG
HY_DVR_GET_COMPRESSCFG, COMPRESSCFG_CAPABILITY_V3 HY_DVR_CODEC_CAP_V3
PUT
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL HY_DVR_DEVICE_INFO
HY_DVR_GET_COMPRESSCFG, COMPRESSCFG_ALL HY_DVR_COMPRESSION_CFG
HY_DVR_GET_COMPRESSCFG, COMPRESSCFG_ALL_EX HY_DVR_COMPRESSION_CFG_EX
HY_DVR_GET_PICCFG, PICCFG_ADVANCED_CONF HY_DVR_ADVANCED_CHANNEL_CFG
HY_DVR_SET_COMPRESSCFG, COMPRESSCFG_ALL HY_DVR_COMPRESSION_CFG
HY_DVR_SET_COMPRESSCFG, COMPRESSCFG_ALL_EX HY_DVR_COMPRESSION_CFG_EX
HY_DVR_SET_PICCFG, PICCFG_ADVANCED_CONF HY_DVR_ADVANCED_CHANNEL_CFG

/Images
GET
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL HY_DVR_DEVICE_INFO
HY_DVR_NVR_GET_ISPCFG,NVR_ISPCFG_ALL ZW_NVR_ISP_CONFIG
HY_DVR_COM_GET_ISPCFG_EX,COM_ISPCFG_EX_ALL ZW_COM_ISP_EX
PUT
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL HY_DVR_DEVICE_INFO
HY_DVR_NVR_GET_ISPCFG, NVR_ISPCFG_ALL ZW_NVR_ISP_CONFIG
HY_DVR_COM_GET_ISPCFG_EX,COM_ISPCFG_EX_ALL ZW_COM_ISP_EX
HY_DVR_NVR_SET_ISPCFG, NVR_ISPCFG_ALL ZW_NVR_ISP_CONFIG
HY_DVR_COM_SET_ISPCFG_EX,COM_ISPCFG_EX_ALL ZW_COM_ISP_EX

/Pictures
GET
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL HY_DVR_DEVICE_INFO
HY_DVR_GET_PICCFG,PICCFG_ALL HY_DVR_PIC_CFG
HY_DVR_GET_PICCFG,PICCFG_MOTION_V2_CONF HY_DVR_MOTION_V2_LIST
HY_DVR_GET_PICCFG,PICCFG_MOTION_EX_CONF HY_DVR_MOTION_EX_LIST
PUT
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL HY_DVR_DEVICE_INFO
HY_DVR_GET_PICCFG,PICCFG_ALL HY_DVR_PIC_CFG
HY_DVR_GET_PICCFG,PICCFG_MOTION_V2_CONF HY_DVR_MOTION_V2_LIST
HY_DVR_GET_PICCFG,PICCFG_MOTION_EX_CONF HY_DVR_MOTION_EX_LIST
HY_DVR_SET_PICCFG, PICCFG_ALL HY_DVR_PIC_CFG
HY_DVR_SET_PICCFG, PICCFG_MOTION_V2_CONF HY_DVR_MOTION_V2_LIST
HY_DVR_SET_PICCFG,PICCFG_MOTION_EX_CONF HY_DVR_MOTION_EX_LIST

/PTZ/Config
GET
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL HY_DVR_DEVICE_INFO
HY_DVR_GET_SERIALCFG, SERIALCFG_ALL HY_DVR_DECODER_CFG
HY_DVR_GET_PTZCFG, PTZCFG_ALL HY_DVR_PTZPRO_CFG
PUT
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL HY_DVR_DEVICE_INFO
HY_DVR_GET_SERIALCFG, SERIALCFG_ALL HY_DVR_DECODER_CFG
HY_DVR_GET_PTZCFG, PTZCFG_ALL HY_DVR_PTZPRO_CFG
HY_DVR_SET_SERIALCFG, SERIALCFG_ALL HY_DVR_DECODER_CFG

/System/DeviceAutoMaintenance
GET
HY_DVR_GET_DEVICEMAINTENANCE, DEVICEMAINTENANCE_ALL HY_DVR_DEVICE_MAINTENANCE
PUT
HY_DVR_SET_DEVICEMAINTENANCE, DEVICEMAINTENANCE_ALL HY_DVR_DEVICE_MAINTENANCE

/System/TransparentChannel/WriteOnce
PUT
typedef int (*IF_TransparentChannelCreate) (HANDLE * hIndex, DWORD dwUserID, const LPTransparentParaInfo pTransparentParaInfo);
typedef int (*IF_TransparentChannelRead) (HANDLE hIndex, const LPTransparentParaInfo pTransparentParaInfo);
typedef int (*IF_TransparentChannelDestroy) (HANDLE hIndex, const LPTransparentParaInfo pTransparentParaInfo);

/System/DeviceConfigFile
GET
typedef int (*IF_RemoteDownloadCreate) (HANDLE * hIndex, DWORD dwUserID, const LPRemoteDownloadParaInfo pRemoteDownloadParaInfo);
typedef int (*IF_RemoteDownloadRead) (HANDLE hIndex, Buffer * pBuffer);
typedef int (*IF_RemoteDownloadDestroy) (HANDLE hIndex, const LPRemoteDownloadState pState);
PUT
HY_DVR_GET_NETCFG, NETCFG_ALL HY_DVR_NET_CFG
typedef int (*IF_RemoteUploadCreate) (HANDLE * hIndex, DWORD dwUserID, const LPFileUploadParaInfo pFileUploadParaInfo);
typedef int (*IF_RemoteUploadWrite) (HANDLE hIndex, Buffer * pBuffer);
typedef int (*IF_RemoteUploadDestroy) (HANDLE hIndex, const LPFileUploadState pState);

/System/DeviceCap
GET
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL			HY_DVR_DEVICE_INFO
HY_DVR_GET_DEVICE_CAPACITY_FILE_NAME_V2, DEVICE_CAPACITY_V2_ALL			HY_DVR_DEVICE_CAP_FILE_NAME_V2

/System/LightCap
GET
HY_DVR_GET_DEVICE_CAPACITY_FILE_NAME_V2, DEVICE_LIGHT_CAPACITY		HY_DVR_DEVICE_CAP_FILE_NAME_V2

/System/PTZCap
GET
HY_DVR_GET_DEVICE_CAPACITY_FILE_NAME_V2, DEVICE_PTZ_CAPACITY

/System/AICap
GET
HY_DVR_GET_DEVICE_CAPACITY_FILE_NAME_V2, DEVICE_AI_CAPACITY

/System/CloudUpgradeServerInfo
PUT
HY_DVR_SET_DEVICECFG, CLOUD_UPGRADE_FIRMWARE_LIST_LOCATION			UPGRADE_LIST_SERVER_CFG

/System/CloudUpgradeFirmwareList
GET
HY_DVR_GET_DEVICECFG, CLOUD_UPGRADE_FIRMWARE_LIST		UPGRADE_FIRMWARE_INFO_LIST

/System/CloudUpgradeDownloadStart
PUT
HY_DVR_SET_DEVICECFG, CLOUD_UPGRADE_FILE_DOWNLOAD_START			UPGRADE_FILE_DOWNLOAD_START

/System/CloudUpgradeDownloadStartV1
PUT
HY_DVR_SET_DEVICECFG, CLOUD_UPGRADE_FILE_DOWNLOAD_START_V1

/System/CloudUpgradeDownloadStatus
GET
GET HY_DVR_GET_DEVICECFG, CLOUD_UPGRADE_FILE_DOWNLOAD_STATUS		UPGRADE_FILE_DOWNLOAD_STATUS

/System/CloudUpgradeDownloadTerminate
PUT
HY_DVR_SET_DEVICECFG, CLOUD_UPGRADE_FILE_DOWNLOAD_STOP		UPGRADE_FILE_DOWNLOAD_STOP

/System/CloudUpgradeStartUpgrade
PUT
PUT HY_DVR_SET_DEVICECFG, CLOUD_UPGRADE_START_UPGRADE		UPGRADE_CONTROL_START

/System/CloudUpgradeResult
GET
HY_DVR_GET_DEVICECFG, CLOUD_UPGRADE_STATUS		UPGRADE_RESULT

/System/ErrorInfoMap
GET
HY_DVR_GET_DEVICE_ERR_INFO, PROTOCOL_INTERACT_ERR_INFO		HY_DVR_PROTOCOL_INTERACT_ERROR_INFO

/System/AudioAlarmConfig
GET
HY_DVR_GET_AUDIO_ALARM_CONFIG, AUDIO_ALARM_CONFIG_ALL			HY_AUDIO_ALARM_CONFIG
PUT
HY_DVR_SET_AUDIO_ALARM_CONFIG, AUDIO_ALARM_CONFIG_ALL			HY_AUDIO_ALARM_CONFIG

/System/ApplyDIDManul
PUT POST
HY_DVR_SET_APPLY_DID_MANUL, HY_DVR_APPLY_DID_MANUL

/System/ElectronicDenceUIDesignCfg
GET
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_ELECTRONIC_FENCE, HY_DVR_ELECTRONIC_FENCE_UI_DESIGN
PUT
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_SET_ELECTRONIC_FENCE, HY_DVR_ELECTRONIC_FENCE_UI_DESIGN

/System/ElectronicDenceUIDesignCfgV2
GET
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_ELECTRONIC_FENCE, HY_DVR_ELECTRONIC_FENCE_UI_DESIGN
PUT
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_SET_ELECTRONIC_FENCE, HY_DVR_ELECTRONIC_FENCE_UI_DESIGN

/System/CrossBorderDetectUIDesignInfo
GET
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_CROSSBORDER_DETECT, HY_DVR_CROSSBORDER_DETECT_UI_DESIGN_INFO
PUT
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_SET_CROSSBORDER_DETECT, HY_DVR_CROSSBORDER_DETECT_UI_DESIGN_INFO

/System/OffDutyDetectUIDesignCfg
GET
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_OFFDUTY_DETECT, HY_DVR_OFFDUTY_DETECT_UI_DESIGN
PUT
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_SET_OFFDUTY_DETECT, HY_DVR_OFFDUTY_DETECT_UI_DESIGN

/System/PassengerFlowStatisticsUIDesignCfg
GET
HY_DVR_GET_PASSENGERFLOW_STATICS, HY_DVR_PASSENGERFLOW_STATICS_UI_DESIGN
PUT
HY_DVR_SET_PASSENGERFLOW_STATICS, HY_DVR_PASSENGERFLOW_STATICS_UI_DESIGN

/System/PassengerFlowStatistics
POST
HY_DVR_GET_PASSENGERFLOW_STATISTICS, HY_DVR_PASSENGERFLOW_STATISTICS_INFO

/System/LicensePlateSurveillanceUIDesignCfg
GET
HY_DVR_GET_LICENSE_PLATE_SURVEILLANCE, LICENSE_PLATE_SURVEILLANCE_UI_DESIGN_CFG
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
PUT
HY_DVR_GET_LICENSE_PLATE_SURVEILLANCE, LICENSE_PLATE_SURVEILLANCE_UI_DESIGN_CFG
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_SET_LICENSE_PLATE_SURVEILLANCE, LICENSE_PLATE_SURVEILLANCE_UI_DESIGN_CFG

/System/TrafficStatisticsUIDesignCfg
GET
HY_DVR_GET_TRAFFIC_STATICS, TRAFFIC_STATICS_UI_DESIGN_CFG
HY_DVR_GET_TRAFFIC_STATICS, TRAFFIC_STATICS_UI_DESIGN_VOICEALARM_TIMESLOT_LIST_CFG
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_TRAFFIC_STATICS, TRAFFIC_STATICS_UI_DESIGN_EVENT_TIMESLOT_LIST_CFG
PUT
HY_DVR_GET_TRAFFIC_STATICS, TRAFFIC_STATICS_UI_DESIGN_CFG
HY_DVR_GET_TRAFFIC_STATICS, TRAFFIC_STATICS_UI_DESIGN_VOICEALARM_TIMESLOT_LIST_CFG
HY_DVR_SET_TRAFFIC_STATICS, TRAFFIC_STATICS_UI_DESIGN_VOICEALARM_TIMESLOT_LIST_CFG
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_TRAFFIC_STATICS, TRAFFIC_STATICS_UI_DESIGN_EVENT_TIMESLOT_LIST_CFG
HY_DVR_SET_TRAFFIC_STATICS, TRAFFIC_STATICS_UI_DESIGN_EVENT_TIMESLOT_LIST_CFG
HY_DVR_SET_TRAFFIC_STATICS, TRAFFIC_STATICS_UI_DESIGN_CFG

/System/TrafficStatistics
POST
HY_DVR_GET_TRAFFIC_STATISTICS, TRAFFIC_STATISTICS

/System/FireDetectCfgInfo
GET
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_FIRE_DETECT_CFG, HY_DVR_FIRE_DETECT_CFG
PUT
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_SET_FIRE_DETECT_CFG, HY_DVR_FIRE_DETECT_CFG

/System/ElectronicDenceCfgEx
GET
HY_DVR_GET_ELECTRONIC_FENCE, ELECTRONIC_FENCE_EX_CFG
PUT
HY_DVR_GET_ELECTRONIC_FENCE, ELECTRONIC_FENCE_EX_CFG
HY_DVR_SET_ELECTRONIC_FENCE, ELECTRONIC_FENCE_EX_CFG

/System/ElectronicDenceCfgExV2
GET
HY_DVR_GET_ELECTRONIC_FENCE, ELECTRONIC_FENCE_EX_CFG
HY_DVR_GET_ELECTRONIC_FENCE, ELECTRONIC_FENCE_EVENT_VOICEALARM_TIMESLOT_CFG
PUT
HY_DVR_GET_ELECTRONIC_FENCE, ELECTRONIC_FENCE_EX_CFG
HY_DVR_GET_ELECTRONIC_FENCE, ELECTRONIC_FENCE_EVENT_VOICEALARM_TIMESLOT_CFG
HY_DVR_SET_ELECTRONIC_FENCE, ELECTRONIC_FENCE_EVENT_VOICEALARM_TIMESLOT_CFG
HY_DVR_SET_ELECTRONIC_FENCE, ELECTRONIC_FENCE_EX_CFG

/System/CrossBorderDetectCfgEx
GET
HY_DVR_GET_CROSSBORDER_DETECT, CROSSBORDER_DETECT_CFG_EX
PUT
HY_DVR_GET_CROSSBORDER_DETECT, CROSSBORDER_DETECT_CFG_EX
HY_DVR_SET_CROSSBORDER_DETECT, CROSSBORDER_DETECT_CFG_EX

/System/OffDutyDetectCfgEx
GET
HY_DVR_GET_OFFDUTY_DETECT, OFFDUTY_DETECT_CFG_EX
PUT
HY_DVR_GET_OFFDUTY_DETECT, OFFDUTY_DETECT_CFG_EX
HY_DVR_SET_OFFDUTY_DETECT, OFFDUTY_DETECT_CFG_EX

/System/PassengerFlowStaticsExCfg
GET
HY_DVR_GET_PASSENGERFLOW_STATICS, PASSENGERFLOW_STATICS_EX_CFG
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
PUT
HY_DVR_GET_PASSENGERFLOW_STATICS, PASSENGERFLOW_STATICS_EX_CFG
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_SET_PASSENGERFLOW_STATICS, PASSENGERFLOW_STATICS_EX_CFG

/System/FireDetectExCfg
GET
HY_DVR_GET_FIRE_DETECT_CFG, FIRE_DETECT_EX_CFG
PUT
HY_DVR_GET_FIRE_DETECT_CFG, FIRE_DETECT_EX_CFG
HY_DVR_SET_FIRE_DETECT_CFG, FIRE_DETECT_EX_CFG

/System/EBikeDetectCfg
GET
HY_DVR_GET_EBIKE_DETECT_CFG, EBIKE_DETECT_CFG
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
PUT
HY_DVR_GET_EBIKE_DETECT_CFG, EBIKE_DETECT_CFG
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_SET_EBIKE_DETECT_CFG, EBIKE_DETECT_CFG

/System/GasTankDetectCfg
GET
HY_DVR_GET_GAS_TANK_DETECT_CFG, GAS_TANK_DETECT_CFG
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
PUT
HY_DVR_GET_GAS_TANK_DETECT_CFG, GAS_TANK_DETECT_CFG
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_SET_GAS_TANK_DETECT_CFG, GAS_TANK_DETECT_CFG

/System/CompilePlatformInfo
None

/EventsNotification/SubscribedEventTypeList
GET

PUT


/EventsNotification/EventList
GET
HY_DVR_GET_NETCFG, NETCFG_ALL
HY_DVR_GET_SYSTIME, SYSTIME_ALL

/TwoWayAudio/Open
typedef int(*IF_VoiceStreamCreate) (DWORD dwUserId, DWORD dwChannel, eVoiceMode eMode);

/TwoWayAudio/Close
typedef int(*IF_VoiceStreamDestroy) (DWORD dwUserId, DWORD dwChannel);

/TwoWayAudio/SendData
PUT


/TwoWayAudio/ReceiveData
GET


/Snapshot/Capability
Get
HY_DVR_GET_ALARMCFG_EX, ALARMCFG_EX_ALL
HY_DVR_GET_ALARMCFG_EX_V2, ALARMCFG_EX_V2_ALL

/Snapshot/Config
Get
HY_DVR_GET_ALARMCFG, ALARMCFG_ALL
HY_DVR_GET_ALARMCFG, ALARMCFG_IMGCAPTURE_CONF_V2
Put
HY_DVR_GET_ALARMCFG, ALARMCFG_ALL
HY_DVR_GET_ALARMCFG, ALARMCFG_IMGCAPTURE_CONF_V2
HY_DVR_SET_ALARMCFG, ALARMCFG_IMGCAPTURE_CONF_V2
HY_DVR_SET_ALARMCFG, ALARMCFG_ALL

/Snapshot/TimedCaptureCfg
Get
HY_DVR_GET_TIMED_CAPTURE_CFG, TIMED_CAPTURE_CFG_ALL
Put
HY_DVR_GET_TIMED_CAPTURE_CFG, TIMED_CAPTURE_CFG_ALL

/Snapshot/ChannelID/RemoteImageCapture
Put
typedef int(*IF_RemoteImageCapture) (DWORD dwUserID, DWORD dwChannel, eImageFormat eFormat, LPImageFileInfo pImageFileInfo);

/Snapshot/ChannelID/1,2,3,4/RemoteImageCaptureV2
Get
HY_DVR_GET_BUFFER_SIZE, BUFFER_SIZE_ALL
HY_DVR_GET_PICTURE_BY_BUFFER, PICTURE_BY_BUFFER_ALL

/Snapshot/ChannelID/Capability
Get
HY_DVR_GET_ALARMCFG_EX, ALARMCFG_EX_ALL
HY_DVR_GET_ALARMCFG_EX_V2, ALARMCFG_EX_V2_ALL

/Snapshot/ChannelID/Config
Get
HY_DVR_GET_ALARMCFG, ALARMCFG_ALL
HY_DVR_GET_ALARMCFG, ALARMCFG_IMGCAPTURE_CONF_V2
Put
HY_DVR_GET_ALARMCFG, ALARMCFG_ALL
HY_DVR_GET_ALARMCFG, ALARMCFG_IMGCAPTURE_CONF_V2
HY_DVR_SET_ALARMCFG, ALARMCFG_IMGCAPTURE_CONF_V2
HY_DVR_SET_ALARMCFG, ALARMCFG_ALL

/Streams/AudioStream
Get
HY_DVR_GET_DEVICECFG, DEVICECFG_ALL
HY_DVR_GET_COMPRESSCFG, COMPRESSCFG_ALL_EX

/System/DeviceStatus/ChannelID
Get
HY_DVR_GET_DEVICESTATE, DEVICESTATE_ALL

/System/ChannelID/RemoteForceIFrame
Put
typedef int(*IF_RemoteForceIFrame) (DWORD dwUserID, DWORD dwChannel);

/System/ChannelID/RemoteAlarmOutClear/0,1
Put
typedef int(*IF_RemoteAlarmClear) (DWORD dwUserID, DWORD dwChannel, DWORD dwAlarmType);

/Streams/ChannelID/CapabilityV2
Get
HY_DVR_GET_COMPRESSCFG, COMPRESSCFG_CAPABILITY
HY_DVR_GET_COMPRESSCFG, COMPRESSCFG_CAPABILITY_V3

/Streams/ChannelID/1,2,3,4
Get
HY_DVR_GET_COMPRESSCFG, COMPRESSCFG_ALL
HY_DVR_GET_NETCFG, NETCFG_ALL
HY_DVR_GET_COMPRESSCFG, COMPRESSCFG_ALL_EX
HY_DVR_GET_PICCFG, PICCFG_ADVANCED_CONF
HY_DVR_GET_COMPRESSCFG_V2, COMPRESSCFG_ONE_CHANNEL_V2
Put
HY_DVR_GET_COMPRESSCFG, COMPRESSCFG_ALL
HY_DVR_GET_COMPRESSCFG, COMPRESSCFG_ALL_EX
HY_DVR_GET_PICCFG, PICCFG_ADVANCED_CONF
HY_DVR_SET_COMPRESSCFG, COMPRESSCFG_ALL
HY_DVR_SET_COMPRESSCFG, COMPRESSCFG_ALL_EX
HY_DVR_SET_PICCFG, PICCFG_ADVANCED_CONF
HY_DVR_GET_COMPRESSCFG_V2, COMPRESSCFG_ONE_CHANNEL_V2
HY_DVR_SET_COMPRESSCFG_V2, COMPRESSCFG_ONE_CHANNEL_V2

/Streams/ChannelID/1,2,3,4/Transport
Get
HY_DVR_GET_COMPRESSCFG, COMPRESSCFG_ALL
HY_DVR_GET_NETCFG, NETCFG_ALL

/Streams/ChannelID
Get
HY_DVR_GET_COMPRESSCFG, COMPRESSCFG_ALL
HY_DVR_GET_COMPRESSCFG, COMPRESSCFG_CAPABILITY
HY_DVR_GET_NETCFG, NETCFG_ALL
HY_DVR_GET_COMPRESSCFG, COMPRESSCFG_ALL_EX
HY_DVR_GET_PICCFG, PICCFG_ADVANCED_CONF
HY_DVR_GET_COMPRESSCFG, COMPRESSCFG_CAPABILITY_V3
Put
HY_DVR_GET_COMPRESSCFG, COMPRESSCFG_ALL
HY_DVR_GET_COMPRESSCFG, COMPRESSCFG_ALL_EX
HY_DVR_GET_PICCFG, PICCFG_ADVANCED_CONF
HY_DVR_SET_COMPRESSCFG, COMPRESSCFG_ALL
HY_DVR_SET_COMPRESSCFG, COMPRESSCFG_ALL_EX
HY_DVR_SET_PICCFG, PICCFG_ADVANCED_CONF

/Streams/ChannelID(/1,/2,/3,/4)/ReceiveData
GET


/Images/ChannelID/Scene
Get
HY_DVR_NVR_GET_ISPCFG, NVR_ISPCFG_ALL
HY_DVR_COM_GET_ISPCFG_EX, COM_ISPCFG_EX_ALL
Put
HY_DVR_NVR_GET_ISPCFG, NVR_ISPCFG_ALL
HY_DVR_COM_GET_ISPCFG_EX, COM_ISPCFG_EX_ALL
HY_DVR_NVR_SET_ISPCFG, NVR_ISPCFG_ALL
HY_DVR_COM_SET_ISPCFG_EX, COM_ISPCFG_EX_ALL

/Images/ChannelID/Basic
Get
HY_DVR_NVR_GET_ISPCFG, NVR_ISPCFG_ALL
Put
HY_DVR_NVR_GET_ISPCFG, NVR_ISPCFG_ALL
HY_DVR_NVR_SET_ISPCFG, NVR_ISPCFG_ALL

/Images/ChannelID/Color
Get
HY_DVR_NVR_GET_ISPCFG, NVR_ISPCFG_ALL
Put
HY_DVR_NVR_GET_ISPCFG, NVR_ISPCFG_ALL
HY_DVR_NVR_SET_ISPCFG, NVR_ISPCFG_ALL

/Images/ChannelID/Exposure
Get
HY_DVR_NVR_GET_ISPCFG, NVR_ISPCFG_ALL
HY_DVR_COM_GET_ISPCFG_EX, COM_ISPCFG_EX_ALL
Put
HY_DVR_NVR_GET_ISPCFG, NVR_ISPCFG_ALL
HY_DVR_COM_GET_ISPCFG_EX, COM_ISPCFG_EX_ALL
HY_DVR_NVR_SET_ISPCFG, NVR_ISPCFG_ALL
HY_DVR_COM_SET_ISPCFG_EX, COM_ISPCFG_EX_ALL

/Images/ChannelID/WhiteBalance
Get
HY_DVR_NVR_GET_ISPCFG, NVR_ISPCFG_ALL
Put
HY_DVR_NVR_GET_ISPCFG, NVR_ISPCFG_ALL
HY_DVR_NVR_SET_ISPCFG, NVR_ISPCFG_ALL

/Images/ChannelID/IrCutFilter
Get
HY_DVR_NVR_GET_ISPCFG, NVR_ISPCFG_ALL
HY_DVR_COM_GET_ISPCFG_EX, COM_ISPCFG_EX_ALL
Put
HY_DVR_NVR_GET_ISPCFG, NVR_ISPCFG_ALL
HY_DVR_COM_GET_ISPCFG_EX, COM_ISPCFG_EX_ALL
HY_DVR_NVR_SET_ISPCFG, NVR_ISPCFG_ALL
HY_DVR_COM_SET_ISPCFG_EX, COM_ISPCFG_EX_ALL

/Images/ChannelID/IrCutFilterEx
Get
HY_DVR_COM_GET_ISPCFG_EX, COM_ISPCFG_EX_ALL_EX
PUT
HY_DVR_COM_GET_ISPCFG_EX, COM_ISPCFG_EX_ALL_EX
HY_DVR_COM_SET_ISPCFG_EX, COM_ISPCFG_EX_ALL_EX

/Images/ChannelID
Get
HY_DVR_NVR_GET_ISPCFG, NVR_ISPCFG_ALL
HY_DVR_COM_GET_ISPCFG_EX, COM_ISPCFG_EX_ALL
Put
HY_DVR_NVR_GET_ISPCFG, NVR_ISPCFG_ALL
HY_DVR_COM_GET_ISPCFG_EX, COM_ISPCFG_EX_ALL
HY_DVR_NVR_SET_ISPCFG, NVR_ISPCFG_ALL
HY_DVR_COM_SET_ISPCFG_EX, COM_ISPCFG_EX_ALL

/Images/ChannelID/Restore
Put
HY_DVR_IPC_SET_ISPCFG, IPC_ISPCFG_DEFAULT

/RTMP/BasicConfig
Get
HY_DVR_GET_RTMP_PUSH_CONFIG, RTMP_PUSH_CONFIG_ALL
Put
HY_DVR_GET_RTMP_PUSH_CONFIG, RTMP_PUSH_CONFIG_ALL
HY_DVR_SET_RTMP_PUSH_CONFIG, RTMP_PUSH_CONFIG_ALL

/RTMP/ChannelID/1,2/Config
Get
HY_DVR_GET_RTMP_PUSH_CONFIG, RTMP_PUSH_CONFIG_ALL
Put
HY_DVR_GET_RTMP_PUSH_CONFIG, RTMP_PUSH_CONFIG_ALL
HY_DVR_SET_RTMP_PUSH_CONFIG, RTMP_PUSH_CONFIG_ALL

/Pictures/ChannelID/OSD
Get
HY_DVR_GET_PICCFG, PICCFG_ALL
Put
HY_DVR_GET_PICCFG, PICCFG_ALL
HY_DVR_SET_PICCFG, PICCFG_ALL

/Pictures/ChannelID/MultiOSD
Get
HY_DVR_GET_EXTENT_OSD, EXTENT_OSD_LIST
PUT
HY_DVR_GET_EXTENT_OSD, EXTENT_OSD_LIST
HY_DVR_SET_EXTENT_OSD, EXTENT_OSD_LIST

/Pictures/ChannelID/MultiOSDV2
Get
HY_DVR_GET_OSD_V2, OSD_INFO_V4
HY_DVR_GET_OSD_V2, OSD_INFO_V2
Put
HY_DVR_GET_OSD_V2, OSD_INFO_V4
HY_DVR_SET_OSD_V2, OSD_INFO_V4
HY_DVR_GET_OSD_V2, OSD_INFO_V2
HY_DVR_SET_OSD_V2, OSD_INFO_V2

/Pictures/ChannelID/VideoLost/Trigger
Get
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_PICCFG, PICCFG_ALL
Put
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_PICCFG, PICCFG_ALL
HY_DVR_SET_PICCFG, PICCFG_ALL

/Pictures/ChannelID/VideoLost/Schedule
Get
HY_DVR_GET_PICCFG, PICCFG_ALL
Put
HY_DVR_GET_PICCFG, PICCFG_ALL
HY_DVR_SET_PICCFG, PICCFG_ALL

/Pictures/ChannelID/VideoLost
Get
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_PICCFG, PICCFG_ALL
Put
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_PICCFG, PICCFG_ALL
HY_DVR_SET_PICCFG, PICCFG_ALL

/Pictures/ChannelID/Motion/Trigger
Get
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_PICCFG, PICCFG_ALL
Put
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_PICCFG, PICCFG_ALL
HY_DVR_GET_PICCFG, PICCFG_MOTION_V2_CONF
HY_DVR_SET_PICCFG, PICCFG_ALL
HY_DVR_SET_PICCFG, PICCFG_MOTION_V2_CONF

/Pictures/ChannelID/Motion/Schedule
Get
HY_DVR_GET_PICCFG, PICCFG_ALL
Put
HY_DVR_GET_PICCFG, PICCFG_ALL
HY_DVR_SET_PICCFG, PICCFG_ALL
HY_DVR_GET_PICCFG, PICCFG_MOTION_V2_CONF
HY_DVR_SET_PICCFG, PICCFG_MOTION_V2_CONF

/Pictures/ChannelID/Motion/Regions
Get
HY_DVR_GET_PICCFG, PICCFG_MOTION_V2_CONF
Put
HY_DVR_GET_PICCFG, PICCFG_MOTION_V2_CONF
HY_DVR_SET_PICCFG, PICCFG_MOTION_V2_CONF

/Pictures/ChannelID/Motion/RegionsV2
Get
HY_DVR_GET_PICCFG, PICCFG_ALL
Put
HY_DVR_GET_PICCFG, PICCFG_ALL
HY_DVR_SET_PICCFG, PICCFG_ALL

/Pictures/ChannelID/Motion
Get
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_PICCFG, PICCFG_ALL
HY_DVR_GET_PICCFG, PICCFG_MOTION_V2_CONF
HY_DVR_GET_PICCFG, PICCFG_MOTION_EX_CONF
Put
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_PICCFG, PICCFG_ALL
HY_DVR_GET_PICCFG, PICCFG_MOTION_V2_CONF
HY_DVR_GET_PICCFG, PICCFG_MOTION_EX_CONF
HY_DVR_SET_PICCFG, PICCFG_ALL
HY_DVR_SET_PICCFG, PICCFG_MOTION_V2_CONF
HY_DVR_SET_PICCFG, PICCFG_MOTION_EX_CONF

/Pictures/ChannelID/PeopleDetect
Get
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_PICCFG, PICCFG_PEOPLE_DETECT_CFG
Put
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_PICCFG, PICCFG_PEOPLE_DETECT_CFG
HY_DVR_SET_PICCFG, PICCFG_PEOPLE_DETECT_CFG

/Pictures/ChannelID/PeopleDetectV1
Get
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_PICCFG, PICCFG_PEOPLE_DETECT_CFG_V1
Put
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_PICCFG, PICCFG_PEOPLE_DETECT_CFG_V1
HY_DVR_SET_PICCFG, PICCFG_PEOPLE_DETECT_CFG_V1

/Pictures/ChannelID/PeopleDetect/Ability
Get
HY_DVR_GET_PICCFG, PICCFG_PEOPLE_DETECT_ABILITY

/Pictures/ChannelID/PeopleDetectStayTime
Get
HY_DVR_GET_PICCFG, PICCFG_PEOPLE_DETECT_STAY_TIME
Put
HY_DVR_GET_PICCFG, PICCFG_PEOPLE_DETECT_STAY_TIME
HY_DVR_SET_PICCFG, PICCFG_PEOPLE_DETECT_STAY_TIME

/Pictures/ChannelID/Mosaic
Get
HY_DVR_GET_PICCFG, PICCFG_ALL
Put
HY_DVR_GET_PICCFG, PICCFG_ALL
HY_DVR_SET_PICCFG, PICCFG_ALL

/Pictures/ChannelID
Get
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_PICCFG, PICCFG_ALL
HY_DVR_GET_PICCFG, PICCFG_MOTION_V2_CONF
HY_DVR_GET_PICCFG, PICCFG_MOTION_EX_CONF
Put
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_PICCFG, PICCFG_ALL
HY_DVR_GET_PICCFG, PICCFG_MOTION_V2_CONF
HY_DVR_GET_PICCFG, PICCFG_MOTION_EX_CONF
HY_DVR_SET_PICCFG, PICCFG_ALL
HY_DVR_SET_PICCFG, PICCFG_MOTION_V2_CONF
HY_DVR_SET_PICCFG, PICCFG_MOTION_EX_CONF

/Pictures/ChannelID/Motion/Notify/URL
GET
None
PUT
None

/Pictures/ChannelID/FaceDetect
Get
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_PICCFG, PICCFG_PEOPLE_FACE_DETECT_CFG
Put
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_PICCFG, PICCFG_PEOPLE_FACE_DETECT_CFG
HY_DVR_SET_PICCFG, PICCFG_PEOPLE_FACE_DETECT_CFG

/Pictures/ChannelID/TraversePlaneDetect
Get
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_PICCFG, PICCFG_TRAVERSE_PLANE_DETECT_CFG
Put
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_PICCFG, PICCFG_TRAVERSE_PLANE_DETECT_CFG
HY_DVR_SET_PICCFG, PICCFG_TRAVERSE_PLANE_DETECT_CFG

/Pictures/ChannelID/FieldDetect
Get
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_PICCFG, PICCFG_FIELD_DETECT_CFG
Put
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_PICCFG, PICCFG_FIELD_DETECT_CFG
HY_DVR_SET_PICCFG, PICCFG_FIELD_DETECT_CFG

/Pictures/ChannelID/FieldDetect
Get
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_PICCFG, PICCFG_FIELD_DETECT_CFG
Put
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_PICCFG, PICCFG_FIELD_DETECT_CFG
HY_DVR_SET_PICCFG, PICCFG_FIELD_DETECT_CFG

/Pictures/ChannelID/PIRDetect
Get
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_PICCFG, PICCFG_PIR_CFG
Put
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_PICCFG, PICCFG_PIR_CFG
HY_DVR_SET_PICCFG, PICCFG_PIR_CFG

/Pictures/ChannelID/LightFillingMode
Get
HY_DVR_GET_PICCFG, PICCFG_LIGHT_FILLING_MODE
Put
HY_DVR_SET_PICCFG, PICCFG_LIGHT_FILLING_MODE

/Pictures/ChannelID/PeopleDetectExtraInfoSwitch
Get
HY_DVR_GET_PICCFG, PICCFG_PEOPLE_DETECT_EXTRA_INFO_SWITCH
Put
HY_DVR_SET_PICCFG, PICCFG_PEOPLE_DETECT_EXTRA_INFO_SWITCH

/Pictures/ChannelID/FaceSmartExposure
Get
HY_DVR_NVR_GET_ISPCFG, NVR_ISPCFG_FACE_SMART_EXPOSURE
Put
HY_DVR_NVR_SET_ISPCFG, NVR_ISPCFG_FACE_SMART_EXPOSURE

/Pictures/ChannelID/CarDetect
Get
HY_DVR_GET_PICCFG, PICCFG_CAR_DETECT_CFG
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
Put
HY_DVR_GET_PICCFG, PICCFG_CAR_DETECT_CFG
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_SET_PICCFG, PICCFG_CAR_DETECT_CFG

/Pictures/ChannelID/PeopleTrack
GET
HY_DVR_GET_PICCFG, PICCFG_PEOPLE_TRACK
PUT
HY_DVR_GET_PICCFG, PICCFG_PEOPLE_TRACK
HY_DVR_SET_PICCFG, PICCFG_PEOPLE_TRACK

/Pictures/ChannelID/MoveTrack
GET
HY_DVR_GET_PICCFG, PICCFG_MOVE_TRACK
PUT
HY_DVR_SET_PICCFG, PICCFG_MOVE_TRACK

/Pictures/ChannelID/AnimalDetect
GET
HY_DVR_GET_PICCFG, PICCFG_ANIMAL_DETECT_CFG
PUT
HY_DVR_GET_PICCFG, PICCFG_ANIMAL_DETECT_CFG
HY_DVR_SET_PICCFG, PICCFG_ANIMAL_DETECT_CFG

/Pictures/ChannelID/LongAwayHomeDetect
GET
HY_DVR_GET_PICCFG, PICCFG_LONG_AWAY_HOME_DETECT_CFG
PUT
HY_DVR_GET_PICCFG, PICCFG_LONG_AWAY_HOME_DETECT_CFG
HY_DVR_SET_PICCFG, PICCFG_LONG_AWAY_HOME_DETECT_CFG

/PTZ/Config
Get
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_SERIALCFG, SERIALCFG_ALL
HY_DVR_GET_PTZCFG, PTZCFG_ALL
Put
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_SERIALCFG, SERIALCFG_ALL
HY_DVR_GET_PTZCFG, PTZCFG_ALL
HY_DVR_SET_SERIALCFG, SERIALCFG_ALL

/PTZ/ChannelID/xxxx
typedef int(*IF_PtzControl) (DWORD dwUserID, DWORD dwChannel, ePTZControlCmdCode eCommandCode, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3,DWORD dwParam4);

/PTZ/ChannelID/Config
Get
HY_DVR_GET_SERIALCFG, SERIALCFG_ALL
HY_DVR_GET_PTZCFG, PTZCFG_ALL
Put
HY_DVR_GET_SERIALCFG, SERIALCFG_ALL
HY_DVR_GET_PTZCFG, PTZCFG_ALL
HY_DVR_SET_SERIALCFG, SERIALCFG_ALL

/PTZ/ChannelID/AdvanceConfig
GET
HY_DVR_GET_PTZ_ADVANCE_CONFIG, PTZ_ADVANCE_CONFIG_ALL_V1
HY_DVR_GET_PTZ_ADVANCE_CONFIG, PTZ_ADVANCE_CONFIG_ALL
PUT
HY_DVR_GET_PTZ_ADVANCE_CONFIG, PTZ_ADVANCE_CONFIG_ALL_V1
HY_DVR_SET_PTZ_ADVANCE_CONFIG, PTZ_ADVANCE_CONFIG_ALL_V1
HY_DVR_SET_PTZ_ADVANCE_CONFIG, PTZ_ADVANCE_CONFIG_ALL

/PTZ/ChannelID/Status
GET
HY_DVR_GET_PTZSTATUS, PTZ_STATUS

/PTZ/ChannelID/LimitPos
Get
HY_DVR_GET_PTZCFG, PTZCFG_LIMIT_POS
Put
HY_DVR_SET_PTZCFG, PTZCFG_LIMIT_POS

/PTZ/ChannelID/CuriseStatus
Get
HY_DVR_GET_PTZSTATUS, PTZ_CURISE_STATUS

/PTZ/ChannelID/WatchCarePreset
Get
HY_DVR_GET_PTZCFG, PTZCFG_WATCH_CARE_PRESET
Put
HY_DVR_GET_PTZCFG, PTZCFG_WATCH_CARE_PRESET
HY_DVR_SET_PTZCFG, PTZCFG_WATCH_CARE_PRESET

/PTZ/ChannelID/WatchCarePresetAttribute
GET
HY_DVR_GET_PTZCFG, WATCH_CARE_PRESET_ATTRIBUTE
PUT
HY_DVR_GET_PTZCFG, WATCH_CARE_PRESET_ATTRIBUTE
HY_DVR_SET_PTZCFG, WATCH_CARE_PRESET_ATTRIBUTE

/PTZ/ChannelID/CalibrationCfg
GET
HY_DVR_GET_PTZCFG, PTZ_CALIBRATION_CFG
PUT
HY_DVR_GET_PTZCFG, PTZ_CALIBRATION_CFG
HY_DVR_SET_PTZCFG, PTZ_CALIBRATION_CFG

/PTZ/ChannelID/CruiseTimeSlot
GET
HY_DVR_GET_PTZ_CRUISE_TIME_SLOT, PTZ_CRUISE_TIME_SLOT
PUT
HY_DVR_GET_PTZ_CRUISE_TIME_SLOT, PTZ_CRUISE_TIME_SLOT
HY_DVR_SET_PTZ_CRUISE_TIME_SLOT, PTZ_CRUISE_TIME_SLOT

/PTZ/ChannelID/TrackTimeSlot
GET
HY_DVR_GET_PTZ_TRACK_TIME_SLOT, PTZ_TRACK_TIME_SLOT
PUT
HY_DVR_GET_PTZ_TRACK_TIME_SLOT, PTZ_TRACK_TIME_SLOT
HY_DVR_SET_PTZ_TRACK_TIME_SLOT, PTZ_TRACK_TIME_SLOT

/PTZ/ChannelID/Linkage/Goto
GET
None
PUT
HY_DVR_SET_PTZ_LINKAGE_GOTO_CFG, PTZ_LINKAGE_GOTO_CFG

/ExternalPTZ/ChannelID/TrackConfig
GET
HY_DVR_GET_EXTERNAL_PTZ_CONFIG, EXTERNAL_PTZ_AF_BOARD_TRACK_CONFIG
PUT
HY_DVR_GET_EXTERNAL_PTZ_CONFIG, EXTERNAL_PTZ_AF_BOARD_TRACK_CONFIG
HY_DVR_SET_EXTERNAL_PTZ_CONFIG, EXTERNAL_PTZ_AF_BOARD_TRACK_CONFIG

/Face/ChannelID/FilterRegionSize
Get
HY_DVR_GET_FACE_SNAP, FACE_SNAP_FILTER_REGION
Put
HY_DVR_GET_FACE_SNAP, FACE_SNAP_FILTER_REGION
HY_DVR_SET_FACE_SNAP, FACE_SNAP_FILTER_REGION

/Face/ChannelID/DetectRegion
Get
HY_DVR_GET_FACE_SNAP, FACE_SNAP_DETECT_REGION
Put
HY_DVR_GET_FACE_SNAP, FACE_SNAP_DETECT_REGION
HY_DVR_SET_FACE_SNAP, FACE_SNAP_DETECT_REGION

/Face/ChannelID/Config
Get
HY_DVR_GET_FACE_SNAP, FACE_SNAP_CONFIG
Put
HY_DVR_GET_FACE_SNAP, FACE_SNAP_CONFIG
HY_DVR_SET_FACE_SNAP, FACE_SNAP_CONFIG

/Face/ChannelID/OverlayInfo
Get
HY_DVR_GET_FACE_SNAP, FACE_SNAP_OVERLAY_INFO
Put
HY_DVR_GET_FACE_SNAP, FACE_SNAP_OVERLAY_INFO
HY_DVR_SET_FACE_SNAP, FACE_SNAP_OVERLAY_INFO

/Face/ChannelID/ClearFaceOverlayInfo
Get
typedef int(*IF_RemoteDeviceControl) (DWORD dwUserID, eRemoteDeviceControl eOperationCode);		eDeviceClearFaceOverlayInfo

/Face/ChannelID/DefaultFaceRegionInfo
Get
HY_DVR_GET_FACE_SNAP, FACE_DEFAULT_FACE_REGION_INFO

/Face/ChannelID/FixedRegionDetect
Get
HY_DVR_GET_FACE_SNAP, FACE_FIXED_REGION_DETECT_INFO
Put
HY_DVR_SET_FACE_SNAP, FACE_FIXED_REGION_DETECT_INFO

/Face/ChannelID/OriginImageConfig
Get
HY_DVR_GET_FACE_SNAP, FACE_ORIGIN_IMAGE_CONFIG
Put
HY_DVR_SET_FACE_SNAP, FACE_ORIGIN_IMAGE_CONFIG

/Face/ChannelID/FaceSnapshotMode
Get
HY_DVR_GET_FACE_SNAP, FACE_SNAPSHOT_MODE
Put
HY_DVR_SET_FACE_SNAP, FACE_SNAPSHOT_MODE

/Face/ChannelID/FaceSnapshotImage
Get
HY_DVR_GET_FACE_SNAP, FACE_SNAPSHOT_IMAGE

/Alarm/AlarmIn/ChannelID
Get
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_ALARMCFG, ALARMCFG_ALL
Put
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_ALARMCFG, ALARMCFG_ALL
HY_DVR_SET_ALARMCFG, ALARMCFG_ALL

/Alarm/AlarmInEx/ChannelID
Get
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_ALARMCFG, ALARMCFG_ALL_EX
Put
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_ALARMCFG, ALARMCFG_ALL_EX
HY_DVR_SET_ALARMCFG, ALARMCFG_ALL_EX

/Alarm/AlarmOut/ChannelID
Get
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_ALARMCFG, ALARMCFG_ALL
HY_DVR_GET_ALARMCFG, ALARMCFG_ALARMOUT_GATE
Put
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_ALARMCFG, ALARMCFG_ALL
HY_DVR_SET_ALARMCFG, ALARMCFG_ALL
HY_DVR_SET_ALARMCFG, ALARMCFG_ALARMOUT_GATE

/Alarm/AlarmOut/ChannelID/ControlMode
Get
HY_DVR_GET_ALARMCFG, ALARMCFG_ALARMOUT_CONTROL_MODE
Put
HY_DVR_SET_ALARMCFG, ALARMCFG_ALARMOUT_CONTROL_MODE

/Alarm/AlarmOut/ChannelID/Schedule
Get
HY_DVR_GET_ALARMCFG, ALARMCFG_ALARM_OUT_SCHEDULE
Put
HY_DVR_GET_ALARMCFG, ALARMCFG_ALARM_OUT_SCHEDULE
HY_DVR_SET_ALARMCFG, ALARMCFG_ALARM_OUT_SCHEDULE

/System/ChannelID/RemoteAlarmoutControl/on
Put
typedef int(*IF_RemoteAlarmoutControl) (DWORD dwUserID, DWORD dwChannel, DWORD dwSwitch);

/System/ChannelID/RemoteAlarmoutControl/off
Put
typedef int(*IF_RemoteAlarmoutControl) (DWORD dwUserID, DWORD dwChannel, DWORD dwSwitch);

/System/ChannelID/RemoteAlarmoutChannelControl
PUT
typedef int(*IF_RemoteAlarmoutChannelControl) (DWORD dwUserID, DWORD dwChannel, DWORD dwStateBits);

/Alarm/AlarmIn
Get
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_ALARMCFG, ALARMCFG_ALL
Put
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_ALARMCFG, ALARMCFG_ALL
HY_DVR_SET_ALARMCFG, ALARMCFG_ALL

/Alarm/AlarmInEx
Get
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_ALARMCFG, ALARMCFG_ALL_EX
Put
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_ALARMCFG, ALARMCFG_ALL_EX
HY_DVR_SET_ALARMCFG, ALARMCFG_ALL_EX

/Alarm/AlarmOut
Get
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_ALARMCFG, ALARMCFG_ALL
HY_DVR_GET_ALARMCFG, ALARMCFG_ALARMOUT_GATE_LIST
Put
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_ALARMCFG, ALARMCFG_ALL
HY_DVR_SET_ALARMCFG, ALARMCFG_ALL
HY_DVR_SET_ALARMCFG, ALARMCFG_ALARMOUT_GATE_LIST

/Alarm/AlarmAction/ChannelID/LightWarning
Put
HY_DVR_SET_ALARM_ACTION, ALARM_ACTION_LIGHT_WARNING

/Alarm/AlarmAction/ChannelID/LightWarningByCurrentConfig
Put
typedef int(*IF_RemoteDeviceControl) (DWORD dwUserID, eRemoteDeviceControl eOperationCode);		eDeviceLightWarningByCurrentConfig

/System/ChannelID/RemoteAlarmoutControl/on
Put
typedef int(*IF_RemoteAlarmoutControl) (DWORD dwUserID, DWORD dwChannel, DWORD dwSwitch);

/System/ChannelID/RemoteAlarmoutControl/off
Put
typedef int(*IF_RemoteAlarmoutControl) (DWORD dwUserID, DWORD dwChannel, DWORD dwSwitch);

/Alarm/ChannelID/VoiceLightConfig
Get
HY_DVR_GET_ALARMCFG, ALARMCFG_ALARM_VOICE_LIGHT_CONFIG
Put
HY_DVR_GET_ALARMCFG, ALARMCFG_ALARM_VOICE_LIGHT_CONFIG
HY_DVR_SET_ALARMCFG, ALARMCFG_ALARM_VOICE_LIGHT_CONFIG

/Alarm/ChannelID/VoiceLightState
Get
HY_DVR_GET_ALARMCFG, ALARMCFG_ALARM_VOICE_LIGHT_STATE

/Alarm/ChannelID/LightWarning
Get
HY_DVR_GET_ALARMCFG, ALARMCFG_ALARM_LIGHT_CONFIG
Put
HY_DVR_SET_ALARMCFG, ALARMCFG_ALARM_LIGHT_CONFIG

/Alarm/ChannelID/AlarmSchedule
Get
HY_DVR_GET_ALARMCFG, ALARMCFG_ALARM_VOICE_SCHEDULE
Put
HY_DVR_GET_ALARMCFG, ALARMCFG_ALARM_VOICE_SCHEDULE
HY_DVR_SET_ALARMCFG, ALARMCFG_ALARM_VOICE_SCHEDULE

/Alarm/ChannelID/1/SimulateDigitalInput
PUT
HY_DVR_SET_ALARMCFG, ALARMCFG_ALARM_SIMULATE_DIGITAL_INPUT

/Alarm/ChannelID/OneClickAlarmCfg
GET
HY_DVR_GET_ALARMCFG, ALARMCFG_ONEC_LICK_ALARM_CONFIG
PUT
HY_DVR_GET_ALARMCFG, ALARMCFG_ONEC_LICK_ALARM_CONFIG
HY_DVR_SET_ALARMCFG, ALARMCFG_ONEC_LICK_ALARM_CONFIG

/Alarm/ChannelID/OneClickAlarmStatus
GET
HY_DVR_GET_ALARMCFG, ALARMCFG_ONE_CLICK_ALARM_STATUS
PUT
None

/Alarm/ChannelID/OneClickAlarmControl
POST
HY_DVR_SET_ALARMCFG, ALARMCFG_ONE_CLICK_ALARM_CONTROL

/System/DeviceCustomFunction
Put
HY_DVR_SET_DEVICE_CUSTOM_FUNCTION, DEVICE_CUSTOM_ALL
//nvr-ipc只有1，2，3；H695AI新增4-11
Main-Command=1时：
Assist-Command=1;//移动侦测语音自定义
Assist-Command=2;//车形报警语音自定义
Assist-Command=3;//火焰报警语音自定义
Assist-Command=4;//电子围栏语音自定义
Assist-Command=5;//越界语音自定义
Assist-Command=6;//离岗语音自定义
Assist-Command=7;//客流统计进入语音自定义
Assist-Command=8;//客流统计离开语音自定义
Assist-Command=9;//目标跟踪语音自定义
Assist-Command=10;//视频遮挡语音自定义
Assist-Command=11;//报警输入语音自定义
Assist-Command=12;//人脸抓拍语音自定义
Assist-Command=13;//口罩检测语音自定义
Assist-Command=14;//车牌语音自定义
Assist-Command=15;//人脸识别语音自定义
Assist-Command=16;//车流统计进入自定义
Assist-Command=17;//车流统计离开自定义

/System/DIYMode
GET
HY_DVR_GET_DEVICE_DIY_MODE_CFG, DEVICE_DIY_MODE_CFG
PUT
HY_DVR_GET_DEVICE_DIY_MODE_CFG, DEVICE_DIY_MODE_CFG
HY_DVR_SET_DEVICE_DIY_MODE_CFG, DEVICE_DIY_MODE_CFG

/System/Device3DPositioningCfg
GET
HY_DVR_GET_DEVICE_3D_POSITIONING_CFG, DEVICE_3D_POSITIONING_CFG
PUT
HY_DVR_SET_DEVICE_3D_POSITIONING_CFG, DEVICE_3D_POSITIONING_CFG

/System/DeviceCMS
Get
HY_DVR_GET_NETSERVERCFG, NETSERVERCFG_ALL
Put
HY_DVR_GET_NETSERVERCFG, NETSERVERCFG_ALL
HY_DVR_SET_NETSERVERCFG, NETSERVERCFG_ALL

/System/Exception
Get
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_ALARMCFG, ALARMCFG_ALL
Put
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_ALARMCFG, ALARMCFG_ALL
HY_DVR_SET_ALARMCFG, ALARMCFG_ALL

/System/HTTPMessageTransmission
Get
HY_DVR_GET_DEVICE_CUSTOM_FUCNTION, DEVICE_HTTP_MESSAGE_TRANSMISSION
Put
HY_DVR_SET_DEVICE_CUSTOM_FUNCTION, DEVICE_HTTP_MESSAGE_TRANSMISSION

/System/LLPlatConnectStateInfo
Get
HY_DVR_GET_LL_PLAT_CONNECT_STATE_INFO, HY_DVR_LL_PLAT_CONNECT_STATE_INFO
Put
HY_DVR_GET_LL_PLAT_CONNECT_STATE_INFO, HY_DVR_LL_PLAT_CONNECT_STATE_INFO
HY_DVR_SET_LL_PLAT_CONNECT_STATE_INFO, HY_DVR_LL_PLAT_CONNECT_STATE_INFO

/System/DevTest
Post
HY_DVR_SET_DEVICE_CUSTOM_FUNCTION, DEVICE_TEST_URL
HY_DVR_GET_DEVICE_CUSTOM_FUCNTION, DEVICE_TEST_URL

/System/VoiceBroadcast
Get
HY_DVR_GET_VOICE_BROADCAST, HY_DVR_VOICE_BROADCAST_CFG
Put
HY_DVR_SET_VOICE_BROADCAST, HY_DVR_VOICE_BROADCAST_CFG

/System/UIPersonFaceEventPromptSwitchCfg
Get
HY_DVR_GET_SCREEN_UIPERSONFACE_EVENT_PROMPT_SWITCH, HY_DVR_SCREEN_UIPERSONFACE_EVENT_PROMPT_SWITCH
Put
HY_DVR_GET_SCREEN_UIPERSONFACE_EVENT_PROMPT_SWITCH, HY_DVR_SCREEN_UIPERSONFACE_EVENT_PROMPT_SWITCH
HY_DVR_SET_SCREEN_UIPERSONFACE_EVENT_PROMPT_SWITCH, HY_DVR_SCREEN_UIPERSONFACE_EVENT_PROMPT_SWITCH

/System/CustomerDefineUIPromptInfo
Get
HY_DVR_GET_CUSTOMER_DEFINE_UI_PROMPT, HY_DVR_CUSTOMER_DEFINE_UI_PROMPT
Put
HY_DVR_SET_CUSTOMER_DEFINE_UI_PROMPT, HY_DVR_CUSTOMER_DEFINE_UI_PROMPT

/System/CUIOTVDeviceAuthInfoCfg
Get
HY_DVR_GET_CUIOT_DEVICE_AUTH_INFO, HY_DVR_CUIOT_DEVICE_AUTH_INFO
Put
HY_DVR_SET_CUIOT_DEVICE_AUTH_INFO, HY_DVR_CUIOT_DEVICE_AUTH_INFO

/System/CUIOTVServerCfg
Get
HY_DVR_GET_CUIOTV_SERVER_CONFIG, HY_DVR_CUIOTV_SERVER_CONFIG
Put
HY_DVR_SET_CUIOTV_SERVER_CONFIG, HY_DVR_CUIOTV_SERVER_CONFIG

/System/EventRecordUploadCustomCfg
Get
HY_DVR_GET_EVENT_RECORD_UPLOAD_CUSTOM, HY_DVR_EVENT_RECORD_UPLOAD_CUSTOM
Put
HY_DVR_SET_EVENT_RECORD_UPLOAD_CUSTOM, HY_DVR_EVENT_RECORD_UPLOAD_CUSTOM

/System/HistoryEventRecordTimeSlotCfg
Get
HY_DVR_GET_HISTORY_EVENT_RECORD_TIMESLOT, HY_DVR_HISTORY_EVENT_RECORD_TIMESLOT
Put
HY_DVR_SET_HISTORY_EVENT_RECORD_TIMESLOT, HY_DVR_HISTORY_EVENT_RECORD_TIMESLOT

/System/EventIntervalAndOtherSettingsCfg
Get
HY_DVR_GET_EVENT_INTERVAL_AND_OTHER_SETTINGS, HY_DVR_EVENT_INTERVAL_AND_OTHER_SETTINGS
Put
HY_DVR_SET_EVENT_INTERVAL_AND_OTHER_SETTINGS, HY_DVR_EVENT_INTERVAL_AND_OTHER_SETTINGS

/System/FaceRecognitionIntervalCfg
Get
HY_DVR_GET_FACE_RECOGNITION_INTERVAL, HY_DVR_FACE_RECOGNITION_INTERVAL
Put
HY_DVR_SET_FACE_RECOGNITION_INTERVAL, HY_DVR_FACE_RECOGNITION_INTERVAL

/System/QRCodeConfigure
Get
HY_DVR_GET_QR_CODE_CFG, HY_DVR_QR_CODE_CFG
Put
HY_DVR_SET_QR_CODE_CFG, HY_DVR_QR_CODE_CFG

/System/ProductTestTool
Post
HY_DVR_SET_PTZ_TEST, HY_DVR_PTZ_TEST

/System/RemoteControlCmd
POST
HY_DVR_SET_MANUAL_RECHARGE, MANUAL_RECHARGE
HY_DVR_SET_TEASE_PET, TEASE_PET_MOTION
HY_DVR_SET_TEASE_PET, TEASE_PET_VOICE

/System/USBExportAttendRecordResultCfg
Get
HY_DVR_GET_USB_EXPORT_ATTEND_RECORD_RESULT, HY_DVR_USB_EXPORT_ATTEND_RECORD_RESULT
Put
HY_DVR_SET_USB_EXPORT_ATTEND_RECORD_RESULT, HY_DVR_USB_EXPORT_ATTEND_RECORD_RESULT

/System/RemoteUpdateResultInfo
Get
HY_DVR_GET_REMOTE_UPDATE_RESULT, HY_DVR_REMOTE_UPDATE_RESULT
Put
HY_DVR_SET_REMOTE_UPDATE_RESULT, HY_DVR_REMOTE_UPDATE_RESULT

/System/P6SMPServerParam
Get
HY_DVR_P6S_MP_GET_SEVER_PARAM, HY_DVR_MP_SERVER_URL
Put
HY_DVR_P6S_MP_GET_SEVER_PARAM, HY_DVR_MP_SERVER_URL
HY_DVR_P6S_MP_SET_SEVER_PARAM, HY_DVR_MP_SERVER_URL

/System/CUIOTEventRecordUploadCustomCfg
Get
HY_DVR_GET_CUIOT_EVENT_RECORD_UPLOAD_CUSTOM, HY_DVR_CUIOT_EVENT_RECORD_UPLOAD_CUSTOM
Put
HY_DVR_SET_CUIOT_EVENT_RECORD_UPLOAD_CUSTOM, HY_DVR_CUIOT_EVENT_RECORD_UPLOAD_CUSTOM

/System/CUIOTHistoryEventRecordTimeSlotCfg
Get
HY_DVR_GET_CUIOT_HISTORY_EVENT_RECORD_TIMESLOT, HY_DVR_CUIOT_HISTORY_EVENT_RECORD_TIMESLOT
Put
HY_DVR_SET_CUIOT_HISTORY_EVENT_RECORD_TIMESLOT, HY_DVR_CUIOT_HISTORY_EVENT_RECORD_TIMESLOT

/System/CUIOTEventIntervalAndOtherSettingsCfg
Get
HY_DVR_GET_CUIOT_EVENT_INTERVAL_AND_OTHER_SETTINGS, HY_DVR_CUIOT_EVENT_INTERVAL_AND_OTHER_SETTINGS
Put
HY_DVR_SET_CUIOT_EVENT_INTERVAL_AND_OTHER_SETTINGS, HY_DVR_CUIOT_EVENT_INTERVAL_AND_OTHER_SETTINGS

/System/JKMPlatformCfgInfo
Get
HY_DVR_GET_JKM_PLATFORM_CFG, HY_DVR_JKM_PLATFORM_CFG
Put
HY_DVR_SET_JKM_PLATFORM_CFG, HY_DVR_JKM_PLATFORM_CFG

/System/SZSBPlatformAuthInfo
Get
HY_DVR_GET_SZSB_PLATFORM_AUTH, HY_DVR_SZSB_PLATFORM_AUTH
Put
HY_DVR_SET_SZSB_PLATFORM_AUTH, HY_DVR_SZSB_PLATFORM_AUTH

/System/SZSBPlatformDeviceCfgInfo
Get
HY_DVR_GET_SZSB_PLATFORM_DEVICE_CFG, HY_DVR_SZSB_PLATFORM_DEVICE_CFG
Put
HY_DVR_SET_SZSB_PLATFORM_DEVICE_CFG, HY_DVR_SZSB_PLATFORM_DEVICE_CFG

/System/SWPPlatformCompanyDeviceInfo
Get
HY_DVR_GET_SWP_PLANTFORM_COMPANY_DEVICE, HY_DVR_SWP_PLANTFORM_COMPANY_DEVICE
Put
HY_DVR_SET_SWP_PLANTFORM_COMPANY_DEVICE, HY_DVR_SWP_PLANTFORM_COMPANY_DEVICE

/System/HeNanHealthCodeDeviceCDKEYInfo
Get
HY_DVR_GET_HENAN_HEALTHCODE_DEVICE_CDKEY, HY_DVR_HENAN_HEALTHCODE_DEVICE_CDKEY
Put
HY_DVR_SET_HENAN_HEALTHCODE_DEVICE_CDKEY, HY_DVR_HENAN_HEALTHCODE_DEVICE_CDKEY

/System/DeviceSleepInfo
Get
HY_DVR_GET_DEVICE_SLEEP_INFO, SLEEPCFG_ALL
Put
HY_DVR_GET_DEVICE_SLEEP_INFO, SLEEPCFG_ALL
HY_DVR_SET_DEVICE_SLEEP_INFO, SLEEPCFG_ALL

/System/AirQualityInfo
Get
HY_DVR_GET_DEVICE_AIR_DECTECT_INFO, AIRCFG_ALL
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
Put
HY_DVR_GET_DEVICE_AIR_DECTECT_INFO, AIRCFG_ALL
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_SET_DEVICE_AIR_DECTECT_INFO, AIRCFG_ALL

/System/NightLedInfo
Get
HY_DVR_GET_NIGHT_LED_INFO, NIGHT_LED_CFG_ALL
Put
HY_DVR_GET_NIGHT_LED_INFO, NIGHT_LED_CFG_ALL
HY_DVR_SET_NIGHT_LED_INFO, NIGHT_LED_CFG_ALL

/System/PowerConfig
Get
HY_DVR_GET_POWER_CONFIG, HY_DVR_POWER_CONFIG
Put
HY_DVR_GET_POWER_CONFIG, HY_DVR_POWER_CONFIG
HY_DVR_SET_POWER_CONFIG, HY_DVR_POWER_CONFIG

/System/DeviceSleepControl
Get
HY_DVR_GET_DEVICE_SLEEP_CONTROL, SLEEP_CONTROL
Put
HY_DVR_GET_DEVICE_SLEEP_CONTROL, SLEEP_CONTROL
HY_DVR_SET_DEVICE_SLEEP_CONTROL, SLEEP_CONTROL

/System/DeviceInstallInfo
Get
HY_DVR_GET_DEVICE_INSTALL_INFO, DEVICE_INSTALL_INFO
Put
HY_DVR_GET_DEVICE_INSTALL_INFO, DEVICE_INSTALL_INFO
HY_DVR_SET_DEVICE_INSTALL_INFO, DEVICE_INSTALL_INFO

/System/VideoShelterAlarmCfg
Get
HY_DVR_GET_VIDEO_SHELTER_ALARM_CFG, VIDEO_SHELTER_ALARM_CFG
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
Put
HY_DVR_GET_VIDEO_SHELTER_ALARM_CFG, VIDEO_SHELTER_ALARM_CFG
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_SET_VIDEO_SHELTER_ALARM_CFG, VIDEO_SHELTER_ALARM_CFG

/System/SWPPlatformDisplayItemCfg
Get
HY_DVR_GET_SWP_PLANTFORM_DISPLAY_ITEM_CONFIG, SWP_PLANTFORM_DISPLAY_ITEM_CONFIG
Put
HY_DVR_GET_SWP_PLANTFORM_DISPLAY_ITEM_CONFIG, SWP_PLANTFORM_DISPLAY_ITEM_CONFIG
HY_DVR_SET_SWP_PLANTFORM_DISPLAY_ITEM_CONFIG, SWP_PLANTFORM_DISPLAY_ITEM_CONFIG

/System/HealthCodeDeviceAndPlatInfo
Get
HY_DVR_GET_HEALTHCODE_DEVICE_AND_PLAT_INFO, HEALTHCODE_DEVICE_AND_PLAT_INFO
Put
HY_DVR_GET_HEALTHCODE_DEVICE_AND_PLAT_INFO, HEALTHCODE_DEVICE_AND_PLAT_INFO
HY_DVR_SET_HEALTHCODE_DEVICE_AND_PLAT_INFO, HEALTHCODE_DEVICE_AND_PLAT_INFO

/System/FrontDeviceDetectList
GET
typedef int(*IF_RemoteDownloadCreate) (HANDLE * hIndex, DWORD dwUserID, const LPRemoteDownloadParaInfo pRemoteDownloadParaInfo);
typedef int(*IF_RemoteDownloadRead) (HANDLE hIndex, Buffer * pBuffer);
typedef int(*IF_RemoteDownloadDestroy) (HANDLE hIndex, const LPRemoteDownloadState pState);

/System/FrontConnectDevicetList
GET
HY_DVR_GET_DEVICES_CONNECT_LIST, DEVICES_CONNECT_LIST_ALL
PUT
HY_DVR_SET_DEVICES_CONNECT_LIST, DEVICES_CONNECT_LIST_ALL

/System/FrontConnectDeviceStatusList
GET
HY_DVR_GET_DEVICE_STATE, DEVICE_STATE_ALL

/System/ModifyDeviceNetwork
PUT
HY_DVR_SET_DEVICE_NETWORK, DEVICE_NETWORK_ALL

/Record/HttpEvent/Config
Get
HY_DVR_GET_HTTP_EVENT_CFG, HTTP_EVENT_ALL
Put
HY_DVR_GET_HTTP_EVENT_CFG, HTTP_EVENT_ALL
HY_DVR_SET_HTTP_EVENT_CFG, HTTP_EVENT_ALL

/Record/Notify/URL
Get
HY_DVR_GET_DEVICECFG, DEVICECFG_ALL
Put
HY_DVR_GET_DEVICECFG, DEVICECFG_ALL
HY_DVR_SET_DEVICECFG, DEVICECFG_ALL

/Record/HttpEvent/Call
Put
typedef int(*IF_TriggerRecordEvent) (DWORD dwUserID, DWORD dwChannel, DWORD dwRecordType, DWORD dwUserData);

/Record/Format/Call
Put
typedef int(*IF_DiskGroupManage) (DWORD dwUserID, eDiskGroupOperation eOperationCode, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3);

/Record
Get
HY_DVR_GET_COMPRESSCFG, COMPRESSCFG_ALL
HY_DVR_GET_DEVICECFG, DEVICECFG_ALL
HY_DVR_GET_STORAGEINFO, STORAGEINFO_ALL
Put
HY_DVR_GET_COMPRESSCFG, COMPRESSCFG_ALL
HY_DVR_GET_DEVICECFG, DEVICECFG_ALL
HY_DVR_SET_COMPRESSCFG, COMPRESSCFG_ALL
HY_DVR_SET_DEVICECFG, DEVICECFG_ALL

/Disk
Get
HY_DVR_GET_STORAGEINFO, STORAGEINFO_ALL
Put
HY_DVR_GET_STORAGEINFO, STORAGEINFO_ALL
HY_DVR_SET_STORAGEINFO, STORAGEINFO_ALL

/Playback/GetRecordInfoByMonth

/Playback/GetRecordInfoByDay

/RecordFileList/Pages/1,2,3,4.......
Get
HY_DVR_SET_RECORD_FILE_CURRENT_PAGE, RECORD_FILE_CURRENT_PAGE_ALL
HY_DVR_GET_RECORD_FILE_LIST, RECORD_FILE_LIST_ALL_V1
HY_DVR_GET_RECORD_FILE_LIST, RECORD_FILE_LIST_ALL_EX
HY_DVR_GET_RECORD_FILE_LIST, RECORD_FILE_LIST_ALL

/RecordFileList/Attribute
Get
HY_DVR_SET_RECORD_FILE_CURRENT_PAGE, RECORD_FILE_CURRENT_PAGE_ALL
HY_DVR_GET_RECORD_FILE_LIST, RECORD_FILE_LIST_ALL_V1
HY_DVR_GET_RECORD_FILE_LIST, RECORD_FILE_LIST_ALL

/SnapshotFileList/Pages/1,2,3,4......
Get
HY_DVR_SET_SNAPSHOT_FILE_CURRENT_PAGE, SNAPSHOT_FILE_CURRENT_PAGE_ALL
HY_DVR_GET_SNAPSHOT_FILE_LIST, SNAPSHOT_FILE_LIST_ALL_EX
HY_DVR_GET_SNAPSHOT_FILE_LIST, SNAPSHOT_FILE_LIST_ALL

/SnapshotFileList/Attribute
Get
HY_DVR_GET_SNAPSHOT_FILE_LIST_ATTRIBUTE, SNAPSHOT_FILE_LIST_ATTRIBUTE_ALL
Put
HY_DVR_SET_SNAPSHOT_FILE_LIST_ATTRIBUTE, FILE_LIST_ATTR_SEARCHTIME
HY_DVR_GET_SNAPSHOT_FILE_LIST_ATTRIBUTE, SNAPSHOT_FILE_LIST_ATTRIBUTE_ALL
HY_DVR_SET_SNAPSHOT_FILE_LIST_ATTRIBUTE, SNAPSHOT_FILE_LIST_ATTRIBUTE_ALL

/Record/ChannelID/RecordSchedule
Get
HY_DVR_GET_RECORDCFG, RECORDCFG_ALL
Put
HY_DVR_GET_RECORDCFG, RECORDCFG_ALL
HY_DVR_SET_RECORDCFG, RECORDCFG_ALL

/Record/ChannelID/RecordScheduleV2
Get
HY_DVR_GET_RECORDCFG, RECORDCFG_ALL_V2
Put
HY_DVR_SET_RECORDCFG, RECORDCFG_ALL_V2

/System/DeviceCMS
Get
HY_DVR_GET_NETSERVERCFG, NETSERVERCFG_ALL
Put
HY_DVR_GET_NETSERVERCFG, NETSERVERCFG_ALL
HY_DVR_SET_NETSERVERCFG, NETSERVERCFG_ALL

/System/Exception
Get
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_ALARMCFG, ALARMCFG_ALL
Put
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_ALARMCFG, ALARMCFG_ALL
HY_DVR_SET_ALARMCFG, ALARMCFG_ALL

/System/AuthInfo
Get
HY_DVR_GET_AUTH_INFO, AUTH_INFO_ALL
Put
HY_DVR_SET_AUTH_INFO, AUTH_INFO_ALL

/System/AuthInfoV2
Put
HY_DVR_SET_AUTH_INFO, AUTH_INFO_ALL_V2
HY_DVR_SET_AUTH_INFO, AUTH_INFO_PUSH_DOMAIN_LIST

/System/SaveHardwareConfig
Put
typedef int(*IF_RemoteDeviceControl) (DWORD dwUserID, eRemoteDeviceControl eOperationCode);			eDeviceSaveHardwareConfig

/System/CancelHardwareConfig
Put
typedef int(*IF_RemoteDeviceControl) (DWORD dwUserID, eRemoteDeviceControl eOperationCode);			eDeviceCancelHardwareConfig

/Alarm/AMS
/Alarm/AMS/1,2
Get
HY_DVR_GET_NETSERVERCFG, NETSERVERCFG_AMS_EX_CONF
Put
HY_DVR_GET_NETSERVERCFG, NETSERVERCFG_AMS_EX_CONF
HY_DVR_SET_NETSERVERCFG, NETSERVERCFG_AMS_EX_CONF

/Alarm/EventCfgList
/Alarm/EventCfgList/1,2
Get
HY_DVR_GET_EVENT_CFG_LIST, EVENT_CFG_LIST_ALL
Put
HY_DVR_GET_EVENT_CFG_LIST, EVENT_CFG_LIST_ALL
HY_DVR_SET_EVENT_CFG_LIST, EVENT_CFG_LIST_ALL

/System/Users/1,2,3,4....
Get
HY_DVR_GET_USERCFG, USERCFG_ALL
HY_DVR_GET_NETCFG, NETCFG_ALL
Put
HY_DVR_GET_USERCFG, USERCFG_ALL
HY_DVR_SET_USERCFG, USERCFG_ONE
Post
HY_DVR_GET_USERCFG, USERCFG_ALL
HY_DVR_SET_USERCFG, USERCFG_CREATE
HY_DVR_SET_USERCFG, USERCFG_ONE
Del
HY_DVR_GET_USERCFG, USERCFG_ALL
HY_DVR_SET_USERCFG, USERCFG_REMOVE

/FaceGroups/Create
Put
HY_DVR_SET_FACE_GROUP, CREATE_FACE_GROUP

/FaceGroups/Modify
Put
HY_DVR_SET_FACE_GROUP, MODIFY_FACE_GROUP

/FaceGroups/Delete
Put
HY_DVR_SET_FACE_GROUP, DELETE_FACE_GROUP
HY_DVR_SET_FACE_GROUP, DELETE_FACE_GROUP_V1

/FaceGroups/QueryAll
Put
HY_DVR_GET_FACE_GROUP, QUERY_ALL_FACE_GROUPS_V1
HY_DVR_GET_FACE_GROUP, QUERY_ALL_FACE_GROUPS

/FaceGroups/QueryByGroupID
Put
HY_DVR_GET_FACE_GROUP, QUERY_FACE_GROUP_BY_GROUP_ID

/FaceGroups/QueryWorkState
Put
HY_DVR_GET_WORK_STATE, QUERY_WORK_STATE

/FaceGroup/CheckPersonInfo
Put
HY_DVR_SET_PERSON_INFO, CHECK_PERSON_INFO_V1
V1 Failed:
HY_DVR_SET_PERSON_INFO, CHECK_PERSON_INFO

/FaceGroup/AddPersonInfoAndFaceImage
Post
HY_DVR_SET_PERSON_INFO, ADD_PERSON_INFO_AND_IMAGE_INFO_V1
HY_DVR_SET_PERSON_INFO, PERSON_NAME_BITMAP_DATA
V1 Failed:
HY_DVR_SET_PERSON_INFO, ADD_PERSON_INFO_AND_IMAGE_INFO

/FaceGroup/DeletePersonList
Put
HY_DVR_SET_PERSON_INFO, DELETE_PERSON_INFO_LIST_V1
V1 Failed:
HY_DVR_SET_PERSON_INFO, DELETE_PERSON_INFO_LIST

/FaceGroup/UnBindPersonInfo
Put
HY_DVR_SET_PERSON_INFO, UNBIND_PERSON_INFO_LIST_V1
V1 Failed:
HY_DVR_SET_PERSON_INFO, UNBIND_PERSON_INFO_LIST

/FaceGroup/BindPersonInfo
Put
HY_DVR_SET_PERSON_INFO, BIND_PERSON_INFO_LIST_V1
V1 Failed:
HY_DVR_SET_PERSON_INFO, BIND_PERSON_INFO_LIST

/FaceGroup/ModifyPersonInfo
Put
HY_DVR_GET_PERSON_INFO, QUERY_PERSON_INFO_BY_PERSON_ID_V1	(Failed: 2)
HY_DVR_SET_PERSON_INFO, MODIFY_PERSON_INFO_V1
HY_DVR_SET_PERSON_INFO, PERSON_NAME_BITMAP_DATA
2:
HY_DVR_SET_PERSON_INFO, MODIFY_PERSON_INFO

/FaceGroup/UpdateFaceImageInfoByPerson
Put
HY_DVR_SET_PERSON_INFO, UPDATE_IMAGE_INFO_V1
V1 Failed:
HY_DVR_SET_PERSON_INFO, UPDATE_IMAGE_INFO

/FaceGroup/UpdatePersonInfoAndFaceImage
Put
HY_DVR_GET_PERSON_INFO, QUERY_PERSON_INFO_BY_PERSON_ID_V1	(Failed:2)
HY_DVR_SET_PERSON_INFO, MODIFY_PERSON_INFO_AND_IMAGE_INFO_V1
HY_DVR_SET_PERSON_INFO, PERSON_NAME_BITMAP_DATA
2:
HY_DVR_SET_PERSON_INFO, MODIFY_PERSON_INFO_AND_IMAGE_INFO

/FaceGroup/QueryPersonInfoList
Put
HY_DVR_GET_PERSON_INFO, QUERY_PERSON_INFO_LIST_V1
V1 Failed:
HY_DVR_GET_PERSON_INFO, QUERY_PERSON_INFO_LIST

/FaceGroup/QueryPersonInfoByPersonID
Put
HY_DVR_GET_PERSON_INFO, QUERY_PERSON_INFO_BY_PERSON_ID_V1
Failed:
HY_DVR_GET_PERSON_INFO, QUERY_PERSON_INFO_BY_PERSON_ID

/FaceGroup/QueryPersonInfoListCount
Put
HY_DVR_GET_PERSON_INFO, QUERY_PERSON_INFO_COUNT

/FaceGroup/QueryPersonImageInfo
Post
HY_DVR_GET_PERSON_INFO, QUERY_PERSON_IMAGE_INFO_V1
HY_DVR_GET_PERSON_INFO, QUERY_PERSON_IMAGE_INFO

/FaceGroup/ImagePersonMap
Get
HY_DVR_GET_PERSON_INFO, DEVICE_IMAGE_PERSON_MAP_NUM
HY_DVR_GET_PERSON_INFO, IMAGE_PERSON_MAP_V1
Put
HY_DVR_SET_PERSON_INFO, IMAGE_PERSON_MAP_V1

/FaceGroup/RemoveImagePersonMap
Put
HY_DVR_SET_PERSON_INFO, REMOVE_IMAGE_PERSON_MAP_V1

/FaceGroup/ChannelID/FaceBrightness
Get
HY_DVR_GET_FACE_GROUP, FACE_BRIGHTNESS
Put
HY_DVR_SET_FACE_GROUP, FACE_BRIGHTNESS

/FaceGroup/ChannelID/PersonNameBitmapData
Post
HY_DVR_SET_PERSON_INFO, PERSON_NAME_BITMAP_DATA

/FaceGroup/ChannelID/DeleteSpecificGroupIDItems
Put
HY_DVR_SET_PERSON_INFO, DELETE_SPECIFIC_GROUPID_ITEMS

/FaceGroup/ChannelID/DeleteAllGroupItems
Put
HY_DVR_SET_PERSON_INFO, DELETE_ALL_GROUP_ITEMS_V1
HY_DVR_SET_PERSON_INFO, DELETE_ALL_GROUP_ITEMS

/FaceGroup/ChannelID/FaceGroupPassPermission
Get
HY_DVR_GET_FACE_GROUP, FACE_GROUP_PASS_PERMISSION
Put
HY_DVR_GET_FACE_GROUP, FACE_GROUP_PASS_PERMISSION
HY_DVR_SET_FACE_GROUP, FACE_GROUP_PASS_PERMISSION

/FaceGroup/ChannelID/PersonPassPermission
Get
HY_DVR_GET_PERSON_INFO, FACE_GROUP_PERSON_PASS_PERMISSION
Put
HY_DVR_GET_PERSON_INFO, FACE_GROUP_PERSON_PASS_PERMISSION
HY_DVR_SET_PERSON_INFO, FACE_GROUP_PERSON_PASS_PERMISSION

/FaceGroup/ChannelID/PersonElevatorConfig
Get
HY_DVR_GET_PERSON_INFO, PERSON_ELEVATOR_CONFIG
Put
HY_DVR_GET_PERSON_INFO, PERSON_ELEVATOR_CONFIG
HY_DVR_SET_PERSON_INFO, PERSON_ELEVATOR_CONFIG

/FaceGroup/ChannelID/AddPersonInfoAndFaceImageV1
Post
HY_DVR_SET_PERSON_INFO, ADD_PERSON_INFO_AND_IMAGE_INFO_V2
HY_DVR_SET_PERSON_INFO, PERSON_NAME_BITMAP_DATA

/FaceGroup/ChannelID/ModifyList
Put
HY_DVR_GET_FACE_GROUP, MODIFY_FACE_GROUP_LIST_V1
HY_DVR_SET_FACE_GROUP, MODIFY_FACE_GROUP_LIST_V1

/FaceGroup/FaceRecognizeStardandGroupList
GET
HY_DVR_GET_FACE_RECOGNIZE_STANDARD_GROUP, FACE_RECOGNIZE_STANDARD_GROUP
PUT
HY_DVR_SET_FACE_RECOGNIZE_STANDARD_GROUP, FACE_RECOGNIZE_STANDARD_GROUP

/FaceGroup/OrganizationStructureTree
GET
HY_DVR_GET_FACE_GROUP_ORGANIZATION_STRUCTURE_TREE, FACE_GROUP_ORGANIZATION_STRUCTURE_TREE
HY_DVR_GET_FACE_GROUP, QUERY_ALL_FACE_GROUPS_V2
PUT


/FaceGroup/OrganizationMemberAdd
GET

PUT
HY_DVR_SET_FACE_GROUP_ORGANIZATION_STRUCTURE_TREE, FACE_GROUP_ORGANIZATION_MEMBER_ADD

/FaceGroup/OrganizationMemberModify
GET

PUT
HY_DVR_SET_FACE_GROUP_ORGANIZATION_STRUCTURE_TREE, FACE_GROUP_ORGANIZATION_MEMBER_MODIFY


/FaceGroup/OrganizationMemberDelete
GET
none
PUT
HY_DVR_SET_FACE_GROUP_ORGANIZATION_STRUCTURE_TREE, FACE_GROUP_ORGANIZATION_MEMBER_DELETE

/FaceGroup/OrganizationMemberSelect
GET
None
PUT
HY_DVR_SET_FACE_GROUP_ORGANIZATION_STRUCTURE_TREE, FACE_GROUP_ORGANIZATION_MEMBER_SELECT


/FaceReco/ChannelID/RecoRuleList
Get
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_FACE_RECO_RULE_LIST, FACE_RECO_RULE_LIST
Put
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_FACE_RECO_RULE_LIST, FACE_RECO_RULE_LIST_V1	(Failed:2)
HY_DVR_SET_FACE_RECO_RULE_LIST, FACE_RECO_RULE_LIST_V1
2:
HY_DVR_GET_FACE_RECO_RULE_LIST, FACE_RECO_RULE_LIST
HY_DVR_SET_FACE_RECO_RULE_LIST, FACE_RECO_RULE_LIST

/FaceReco/ChannelID/RecoRuleListVoiceAlarm
GET
HY_DVR_GET_FACE_RECO_RULE_LIST, FACE_RECO_RULE_LIST_V1_VOICE_ALARM
HY_DVR_GET_FACE_RECO_RULE_LIST, FACE_RECO_EVENT_VOICEALARM_TIMESLOT_CFG
PUT
HY_DVR_GET_FACE_RECO_RULE_LIST, FACE_RECO_RULE_LIST_V1_VOICE_ALARM
HY_DVR_GET_FACE_RECO_RULE_LIST, FACE_RECO_EVENT_VOICEALARM_TIMESLOT_CFG
HY_DVR_SET_FACE_RECO_RULE_LIST, FACE_RECO_EVENT_VOICEALARM_TIMESLOT_CFG
HY_DVR_SET_FACE_RECO_RULE_LIST, FACE_RECO_RULE_LIST_V1_VOICE_ALARM

/FaceReco/ChannelID/BaseConfig
Get
HY_DVR_GET_WORK_STATE, FACE_BASE_CONFIG
Put
HY_DVR_SET_WORK_STATE, FACE_BASE_CONFIG

/FaceReco/1/RemoteOpenDoorCfg
Get
HY_DVR_GET_REMOTE_OPEN_DOOR_CFG, HY_DVR_REMOTE_OPEN_DOOR_CFG
Put
HY_DVR_SET_REMOTE_OPEN_DOOR_CFG, HY_DVR_REMOTE_OPEN_DOOR_CFG

/FaceRecognition/QueryRecordCount
Put
HY_DVR_GET_FACE_RECO_RECORD, FACE_RECO_RECORD_COUNT

/FaceRecognition/QueryRecordList
Put
HY_DVR_GET_FACE_RECO_RECORD, FACE_RECO_RECORD_LIST_V1
V1 Failed:
HY_DVR_GET_FACE_RECO_RECORD, FACE_RECO_RECORD_LIST

/FaceRecognition/DetailByRecognitionRecord
Put
HY_DVR_GET_FACE_RECO_RECORD, FACE_RECO_RECORD_DETAIL_V1
V1 Failed:
HY_DVR_GET_FACE_RECO_RECORD, FACE_RECO_RECORD_DETAIL

/FaceRecognition/SnapshotByRecognitionRecord
Post
HY_DVR_GET_FACE_RECO_RECORD, FACE_RECO_RECORD_SNAPSHOT

/FaceRecognition/RecoResultImageByRecognitionRecord
Post
HY_DVR_GET_FACE_RECO_RECORD, FACE_RECO_RECORD_IMAGE

/FaceRecognition/ChannelID/DeleteRecord
Put
HY_DVR_SET_FACE_RECO_RECORD, FACE_RECO_DELETE_RECORD

/FaceRecognition/ChannelID/RecoDistance
Get
HY_DVR_GET_FACE_RECO_RECORD, FACE_RECO_DISTANCE
Put
HY_DVR_SET_FACE_RECO_RECORD, FACE_RECO_DISTANCE

/FaceRecognition/Ability
Get
HY_DVR_GET_FACE_RECO_ABILITY, FACE_RECO_ABILITY_ALL

/FaceRecognition/FaceLivenessStatus
Get
HY_DVR_GET_FACE_LIVENESS, FACE_LIVENESS_STATUS
Put
HY_DVR_SET_FACE_LIVENESS, FACE_LIVENESS_STATUS

/FaceGroup/AddPersonListBegin
Get
HY_DVR_GET_FACEGROUP_ADDLIST_STATE, HY_DVR_FACEGROUP_ADDLIST_BEGIN
Put
HY_DVR_SET_FACEGROUP_ADDLIST_STATE, HY_DVR_FACEGROUP_ADDLIST_BEGIN

/FaceGroup/AddPersonListEnd
Put
HY_DVR_SET_FACEGROUP_ADDLIST_STATE, HY_DVR_FACEGROUP_ADDLIST_END

/System/LongSeeDeviceConfig
Get
HY_DVR_GET_DEVICEINFO, LONGSEE_DEVICECONFIG
Put
HY_DVR_SET_DEVICEINFO, LONGSEE_DEVICECONFIG

/AccessGate/AddCard
Put
HY_DVR_SET_ACCESS_GATE_INFO, ADD_ACCESS_GATE_CARD

/AccessGate/DeleteCard
Put
HY_DVR_SET_ACCESS_GATE_INFO, DELETE_ACCESS_GATE_CARD

/AccessGate/ModifyCard
Put
HY_DVR_SET_ACCESS_GATE_INFO, MODIFY_ACCESS_GATE_CARD

/AccessGate/QueryAllCard
Get
HY_DVR_GET_ACCESS_GATE_INFO, QUERY_ACCESS_GATE_CARD

/AccessGate/AddPassword
Put
HY_DVR_SET_ACCESS_GATE_INFO, ADD_ACCESS_GATE_PASSWORD

/AccessGate/DeletePassword
Put
HY_DVR_SET_ACCESS_GATE_INFO, DELETE_ACCESS_GATE_PASSWORD

/AccessGate/ModifyPassword
Put
HY_DVR_SET_ACCESS_GATE_INFO, MODIFY_ACCESS_GATE_PASSWORD

/AccessGate/QueryAllPassword
Get
HY_DVR_GET_ACCESS_GATE_INFO, QUERY_ACCESS_GATE_PASSWORD

/AccessGate/QueryCardNumberByID
Get
HY_DVR_GET_ACCESS_GATE_INFO, QUERY_ACCESS_GATE_QUERY_CARDNUMBER_BY_ID

/AccessGate/QueryPasswordByID
Get
HY_DVR_GET_ACCESS_GATE_INFO, QUERY_ACCESS_GATE_QUERY_PASSWORD_BY_ID

/AccessGate/QueryCurrentCardNumber
Get
HY_DVR_GET_ACCESS_GATE_INFO, QUERY_CURRENT_CARD_NUMBER

/AccessGate/CardReadMode
Get
HY_DVR_GET_ACCESS_GATE_INFO, CARD_READ_MODE
Put
HY_DVR_SET_ACCESS_GATE_INFO, CARD_READ_MODE

/AccessGate/ChannelID/RemoteOpenDoor
Put
HY_DVR_SET_MQTT_REMOTE_OPEN_DOOR, OPEN_DOOR_CONTROL_ID_CGI

/AccessGate/Direction
Get
HY_DVR_GET_ACCESS_GATE_INFO, ACCESS_GATE_DIRECTION
Put
HY_DVR_GET_ACCESS_GATE_INFO, ACCESS_GATE_DIRECTION
HY_DVR_SET_ACCESS_GATE_INFO, ACCESS_GATE_DIRECTION

/FaceMaskDetect/AlarmConfig
Get
HY_DVR_GET_FACE_DETECT_MASK_CONFIG, FACE_MASK_ALARM_CONFIG
Put
HY_DVR_SET_FACE_DETECT_MASK_CONFIG, FACE_MASK_ALARM_CONFIG

/FaceMaskDetect/AlarmLinkage
Get
HY_DVR_GET_FACE_DETECT_MASK_CONFIG, FACE_MASK_ALARM_LINKAGE
Put
HY_DVR_SET_FACE_DETECT_MASK_CONFIG, FACE_MASK_ALARM_LINKAGE

/FaceMaskDetect/ChannelID/PassConfig
Get
HY_DVR_GET_FACE_DETECT_MASK_CONFIG, FACE_MASK_PASS_CONFIG
Put
HY_DVR_SET_FACE_DETECT_MASK_CONFIG, FACE_MASK_PASS_CONFIG

/System/ChannelID/AssistantInterfaceControl
Get
HY_DVR_GET_DEVICECFG, DEVICECFG_ASSISTANT_INTERFACE_CONTROL
Put
HY_DVR_SET_DEVICECFG, DEVICECFG_ASSISTANT_INTERFACE_CONTROL

/Alarm/PreventionControl
Get
HY_DVR_GET_ALARMCFG, ALARMCFG_ALARMOUT_PREVENTION_CONTROL
Put
HY_DVR_SET_ALARMCFG, ALARMCFG_ALARMOUT_PREVENTION_CONTROL

/System/WorkModeControl
Get
HY_DVR_GET_DEVICECFG, DEVICECFG_SYSTEM_WORK_MODE_CONTROL
Put
HY_DVR_SET_DEVICECFG, DEVICECFG_SYSTEM_WORK_MODE_CONTROL

/FaceTemperatureDetect/ChannelID/FaceTemperatureDetectConfig
Get
HY_DVR_GET_FACE_TEMPERATURE_DETECT_CONFIG, FACE_TEMPERATURE_DETECT_CONFIG
Put
HY_DVR_SET_FACE_TEMPERATURE_DETECT_CONFIG, FACE_TEMPERATURE_DETECT_CONFIG

/FaceTemperatureDetect/ChannelID/OverlayFaceTemperatureOnOSD
Get
HY_DVR_GET_FACE_TEMPERATURE_DETECT_CONFIG, FACE_TEMPERATURE_DETECT_OSD_OVERLAY_CONFIG
Put
HY_DVR_SET_FACE_TEMPERATURE_DETECT_CONFIG, FACE_TEMPERATURE_DETECT_OSD_OVERLAY_CONFIG

/FaceTemperatureDetect/ChannelID/FaceTemperatureStatus
Get
HY_DVR_GET_FACE_TEMPERATURE_DETECT_CONFIG, CURRENT_FACE_TEMPREATURE

/FaceTemperatureDetect/ChannelID/FaceTemperatureAbnormalAlarmConfig
Get
HY_DVR_GET_FACE_TEMPERATURE_DETECT_CONFIG, FACE_TEMPERATURE_ABNORMAL_ALARM_CONFIG
Put
HY_DVR_SET_FACE_TEMPERATURE_DETECT_CONFIG, FACE_TEMPERATURE_ABNORMAL_ALARM_CONFIG

/FaceTemperatureDetect/ChannelID/DetectRevise
Get
HY_DVR_GET_FACE_TEMPERATURE_DETECT_CONFIG, FACE_TEMPERATURE_DETECT_REVISE
Put
HY_DVR_SET_FACE_TEMPERATURE_DETECT_CONFIG, FACE_TEMPERATURE_DETECT_REVISE

/System/ChannelID/LowTemFitSwitch
Get
HY_DVR_GET_DEVICECFG, DEVICE_LOW_TEM_FIT_SWITCH
Put
HY_DVR_SET_DEVICECFG, DEVICE_LOW_TEM_FIT_SWITCH

/System/DeviceBindConfig
Get
HY_DVR_GET_DEVICECFG, DEVICECFG_BIND_CONFIG
Put
HY_DVR_SET_DEVICECFG, DEVICECFG_BIND_CONFIG

/Security/UserPermissionConfigs
Get
HY_DVR_GET_USERCFG, USERCFG_PERMISSION_CONFIG_ALL
Put
HY_DVR_GET_USERCFG, USERCFG_PERMISSION_CONFIG_ALL
HY_DVR_SET_USERCFG, USERCFG_PERMISSION_CONFIG_ALL

/Security/UserPermissionConfig/1,2,3,4......
Get
HY_DVR_GET_USERCFG, USERCFG_PERMISSION_CONFIG_ALL
Put
HY_DVR_GET_USERCFG, USERCFG_PERMISSION_CONFIG_ALL
HY_DVR_SET_USERCFG, USERCFG_PERMISSION_CONFIG_ALL

/Security/UserPermissionConfigByUsername
Get
HY_DVR_GET_USERCFG, USERCFG_PERMISSION_CONFIG_ALL

/System/UserWhiteIPList
Get
HY_DVR_GET_USERCFG, USERCFG_ALL
Put
HY_DVR_GET_USERCFG, USERCFG_ALL
HY_DVR_SET_USERCFG, USERCFG_ALL

/System/UserBlackIPList
Get
HY_DVR_GET_USERCFG, USERCFG_BLACK_IP_LIST
Put
HY_DVR_GET_USERCFG, USERCFG_BLACK_IP_LIST
HY_DVR_SET_USERCFG, USERCFG_BLACK_IP_LIST

/System/DeleteVoiceCustom
Put
HY_DVR_SET_DEVICECFG, DEVICECFG_DELETE_VOICE_CUSTOM

/System/Language
Get
HY_DVR_GET_DEVICECFG, DEVICECFG__LANGUAGE
Put
HY_DVR_SET_DEVICECFG, DEVICECFG__LANGUAGE

/System/PerformanceStatus
Get
HY_DVR_GET_SYSTEM_PERFORMANCE_STATUS, CONFIG_ALL

/System/FiberHomeBaseInfo
Get
HY_DVR_GET_DEVICECFG, DEVICECFG_FIBERHOME_BASE_INFO
Put
HY_DVR_SET_DEVICECFG, DEVICECFG_FIBERHOME_BASE_INFO

/System/FiberHomeLocationInfo
Get
HY_DVR_GET_DEVICECFG, DEVICECFG_FIBERHOME_LOCATION_INFO
Put
HY_DVR_SET_DEVICECFG, DEVICECFG_FIBERHOME_LOCATION_INFO

/System/FiberHomeCtrlChnlServer
Get
HY_DVR_GET_DEVICECFG, DEVICECFG_FIBERHOME_CTRL_CHNL_SERVER
Put
HY_DVR_SET_DEVICECFG, DEVICECFG_FIBERHOME_CTRL_CHNL_SERVER

/System/FiberHomeLocationID
Get
HY_DVR_GET_DEVICECFG, DEVICECFG_FIBERHOME_LOCATION_ID
Put
HY_DVR_SET_DEVICECFG, DEVICECFG_FIBERHOME_LOCATION_ID

/System/FiberHomeDataCenterServer
Get
HY_DVR_GET_DEVICECFG, DEVICECFG_FIBERHOME_DATA_CENTER_SERVER
Put
HY_DVR_SET_DEVICECFG, DEVICECFG_FIBERHOME_DATA_CENTER_SERVER

/System/FiberHomeAreaCodeInfo
Get
HY_DVR_GET_DEVICECFG, DEVICECFG_FIBERHOME_AREA_CODE
Put
HY_DVR_SET_DEVICECFG, DEVICECFG_FIBERHOME_AREA_CODE

/System/FiberHomeLonLatInfo
Get
HY_DVR_GET_DEVICECFG, DEVICECFG_FIBERHOME_LONLAT_INFO
Put
HY_DVR_SET_DEVICECFG, DEVICECFG_FIBERHOME_LONLAT_INFO

/System/FiberHomeLogPath
Get
HY_DVR_GET_DEVICECFG, DEVICECFG_FIBERHOME_LOG_PATH

/System/FiberHomeVideoServerUrl
Get
HY_DVR_GET_DEVICECFG, DEVICECFG_FIBERHOME_VIDEO_SERVER_URL
Put
HY_DVR_SET_DEVICECFG, DEVICECFG_FIBERHOME_VIDEO_SERVER_URL

/Record/CurrentRecordInfo
Get
HY_DVR_GET_RECORD_INFO, RECORD_CURRENT_RECORD_START_TIME

/System/MQTTServerConfig
Get
HY_DVR_GET_MQTT_SERVER_CONFIG, MQTT_SERVER_ALL
Put
HY_DVR_SET_MQTT_SERVER_CONFIG, MQTT_SERVER_ALL

/System/ChannelID/MQTTPrivateSettings
Get
HY_DVR_GET_PRIVACY_SETTING, PRIVACY_SETTING
Put
HY_DVR_GET_PRIVACY_SETTING, PRIVACY_SETTING
HY_DVR_SET_PRIVACY_SETTING, PRIVACY_SETTING

/System/JGPlatformServerConfig
Get
HY_DVR_GET_DEVICECFG, HY_DVR_JGPLATFORM_CFG
Put
HY_DVR_SET_DEVICECFG, HY_DVR_JGPLATFORM_CFG

/System/AIWorkMode
Get
HY_DVR_GET_DEVICECFG, DEVICECFG_AI_WORK_MODE
Put
HY_DVR_SET_DEVICECFG, DEVICECFG_AI_WORK_MODE

/Alarm/OverlayInfo
Get
HY_DVR_GET_ALARMCFG, ALARMCFG_ALARM_OVERLAY_INFO
Put
HY_DVR_SET_ALARMCFG, ALARMCFG_ALARM_OVERLAY_INFO

/System/UserTokenLogout
PUT
None

/System/HTTPEventServerConfig
Get
HY_DVR_GET_HTTP_EVENT_SERVER_CONFIG, HTTP_EVENT_SERVER_ALL
Put
HY_DVR_SET_HTTP_EVENT_SERVER_CONFIG, HTTP_EVENT_SERVER_ALL

/System/HTTPEventServerStatus
Get
HY_DVR_GET_HTTP_EVENT_SERVER_CONFIG, HTTP_EVENT_SERVER_STATUS
Put
HY_DVR_GET_HTTP_EVENT_SERVER_CONFIG, HTTP_EVENT_SERVER_STATUS
HY_DVR_SET_HTTP_EVENT_SERVER_CONFIG, HTTP_EVENT_SERVER_STATUS

/System/HTTPEventPushMode
Get
HY_DVR_GET_FACE_RECO_MODE, FACE_RECO_MODE_CFG
Put
HY_DVR_SET_FACE_RECO_MODE, FACE_RECO_MODE_CFG

/System/HTTPEventServerConfigV2
GET
HY_DVR_GET_HTTP_EVENT_SERVER_CONFIG, HTTP_EVENT_SERVER_CFG_V2
PUT
HY_DVR_GET_HTTP_EVENT_SERVER_CONFIG, HTTP_EVENT_SERVER_CFG_V2
HY_DVR_SET_HTTP_EVENT_SERVER_CONFIG, HTTP_EVENT_SERVER_CFG_V2

/System/HTTPEventServerStatusV2
GET
HY_DVR_GET_HTTP_EVENT_SERVER_CONFIG, HTTP_EVENT_SERVER_STATUS_V2
PUT
HY_DVR_GET_HTTP_EVENT_SERVER_CONFIG, HTTP_EVENT_SERVER_STATUS_V2
HY_DVR_SET_HTTP_EVENT_SERVER_CONFIG, HTTP_EVENT_SERVER_STATUS_V2

/System/HTTPEventServerTest
POST
HY_DVR_GET_HTTP_EVENT_SERVER_CONFIG, HTTP_EVENT_SERVER_STATUS_TEST

/System/EventPushMode
Get
HY_DVR_GET_FACE_RECO_MODE, FACE_RECO_MODE_CFG
Put
HY_DVR_SET_FACE_RECO_MODE, FACE_RECO_MODE_CFG

/System/ChannelID/DeleteCustomLanguage
Put
HY_DVR_SET_DEVICECFG, DEVICECFG_DELETE_LANGUAGE_CUSTOM

/System/ChannelID/CustomLanguageName
Get
HY_DVR_GET_DEVICECFG, DEVICECFG_CUSTOM_LANGUAGE_NAME
Put
HY_DVR_SET_DEVICECFG, DEVICECFG_CUSTOM_LANGUAGE_NAME

/System/ChannelID/LanguageDisplayStatus
Get
HY_DVR_GET_DEVICECFG, DEVICE_LANGUAGE_DISPLAY_INFO

/System/ChannelID/CustomLanguageLatticeSwitch
Get
HY_DVR_GET_DEVICECFG, DEVICE_CUSTOM_LANGUAGE_LATTICE_SWITCH
Put
HY_DVR_SET_DEVICECFG, DEVICE_CUSTOM_LANGUAGE_LATTICE_SWITCH

/System/ChannelID/ScreenCloseTime
Get
HY_DVR_GET_DEVICECFG, DEVICECFG_SCREEN_CLOSE_TIME
Put
HY_DVR_GET_DEVICECFG, DEVICECFG_SCREEN_CLOSE_TIME
HY_DVR_SET_DEVICECFG, DEVICECFG_SCREEN_CLOSE_TIME

/System/ChannelID/IRStatus
Get
HY_DVR_COM_GET_ISPCFG_EX, COM_ISPCFG_IR_STATUS

/System/ChannelID/ThirdProtocolManage
Get
HY_DVR_GET_THIRD_PARTY_PROTOCOL_CONFIG, THIRD_PARTY_PROTOCOL_CONFIG
Put
HY_DVR_SET_THIRD_PARTY_PROTOCOL_CONFIG, THIRD_PARTY_PROTOCOL_CONFIG

/System/ChannelID/PlatformMode
Get
HY_DVR_GET_DEVICECFG, DEVICE_PLATFORM_MODE
Put
HY_DVR_SET_DEVICECFG, DEVICE_PLATFORM_MODE

/System/ChannelID/NFCBasicConfig
Get
HY_DVR_GET_DEVICECFG, NFC_BASIC_CONFIG
Put
HY_DVR_SET_DEVICECFG, NFC_BASIC_CONFIG

/System/ChannelID/DoubleCameraCalibration
Get
HY_DVR_GET_DEVICECFG, DOUBLE_CAMERA_CALIBRATION
Put
HY_DVR_SET_DEVICECFG, DOUBLE_CAMERA_CALIBRATION

/System/ChannelID/ElevatorBasicConfig
Get
HY_DVR_GET_DEVICECFG, ELEVATOR_CONFIG
Put
HY_DVR_GET_DEVICECFG, ELEVATOR_CONFIG
HY_DVR_SET_DEVICECFG, ELEVATOR_CONFIG

/System/ChannelID/AgingTest
Get
HY_DVR_GET_DEVICECFG, AGING_TEST
Put
HY_DVR_GET_DEVICECFG, AGING_TEST
HY_DVR_SET_DEVICECFG, AGING_TEST

/System/ChannelID/PushEventInterval
Get
HY_DVR_GET_DEVICECFG, PUSH_EVENT_INTERVAL
Put
HY_DVR_GET_DEVICECFG, PUSH_EVENT_INTERVAL
HY_DVR_SET_DEVICECFG, PUSH_EVENT_INTERVAL

/System/ChannelID/PushEventGlobalCfg
GET
HY_DVR_GET_DEVICECFG, PUSH_EVENT_GLOBAL_CFG
PUT
HY_DVR_GET_DEVICECFG, PUSH_EVENT_GLOBAL_CFG
HY_DVR_SET_DEVICECFG, PUSH_EVENT_GLOBAL_CFG

/System/ChannelID/TryListenSelectedAlarmAudioV1
Put
HY_DVR_SET_AUDIO_ALARM_CONFIG, AUDIO_ALARM_CONFIG_ALL_CHECK_V1

/System/ChannelID/AudioAlarmConfigV1
Get
HY_DVR_GET_AUDIO_ALARM_CONFIG, AUDIO_ALARM_CONFIG_V1
Put
HY_DVR_GET_AUDIO_ALARM_CONFIG, AUDIO_ALARM_CONFIG_V1
HY_DVR_SET_AUDIO_ALARM_CONFIG, AUDIO_ALARM_CONFIG_V1

/System/ChannelID/AudioAlarmEliminate
GET
HY_DVR_GET_AUDIO_ALARM_CONFIG, AUDIO_ALARM_ELIMINATE
PUT
HY_DVR_SET_AUDIO_ALARM_CONFIG, AUDIO_ALARM_ELIMINATE

/System/ChannelID/GB28181Config
Get
HY_DVR_GET_GB28181_BASIC_INFO, GB28181_BASIC_INFO
Put
HY_DVR_SET_GB28181_BASIC_INFO, GB28181_BASIC_INFO

/System/ChannelID/GB28181DeviceInfo
Get
HY_DVR_GET_DEVICEINFO, DEVICEINFO_CUSTOMER
Put
HY_DVR_GET_DEVICEINFO, DEVICEINFO_CUSTOMER
HY_DVR_SET_DEVICEINFO, DEVICEINFO_CUSTOMER

/System/ChannelID/GB28181DeviceInstallInfo
Get
HY_DVR_GET_DEVICECFG, DEVICECFG_INSTALLTION_CONF_LIST
Put
HY_DVR_GET_DEVICECFG, DEVICECFG_INSTALLTION_CONF_LIST
HY_DVR_SET_DEVICECFG, DEVICECFG_INSTALLTION_CONF_LIST

/System/ChannelID/OnvifConfig
GET
HY_DVR_GET_ONVIF_CFG_INFO, ONVIF_CFG_INFO
PUT
HY_DVR_GET_ONVIF_CFG_INFO, ONVIF_CFG_INFO
HY_DVR_SET_ONVIF_CFG_INFO, ONVIF_CFG_INFO

/System/LicensePlateOwnnerInfo
GET
HY_DVR_GET_LICENSE_PLATE_OWNNER_INFO, LICENSE_PLATE_OWNNER_INFO
PUT
HY_DVR_GET_LICENSE_PLATE_OWNNER_INFO, LICENSE_PLATE_OWNNER_INFO
HY_DVR_SET_LICENSE_PLATE_OWNNER_INFO, LICENSE_PLATE_OWNNER_INFO

/Alarm/ChannelID/PolygonConfig
Get
HY_DVR_GET_ALARMCFG, ALARMCFG_ALARM_POLYGON_AREA_INFO
Put
HY_DVR_GET_ALARMCFG, ALARMCFG_ALARM_POLYGON_AREA_INFO
HY_DVR_SET_ALARMCFG, ALARMCFG_ALARM_POLYGON_AREA_INFO

/Alarm/ChannelID/PolygonSchedule
Get
HY_DVR_GET_ALARMCFG, ALARMCFG_ALARM_POLYGON_SCHEDULE
Put
HY_DVR_SET_ALARMCFG, ALARMCFG_ALARM_POLYGON_SCHEDULE

/Alarm/ChannelID/AssisstantLineConfig
Get
HY_DVR_GET_ALARMCFG, ALARMCFG_ALARM_ASSISSTANT_LINE_INFO
Put
HY_DVR_SET_ALARMCFG, ALARMCFG_ALARM_ASSISSTANT_LINE_INFO

/Alarm/ChannelID/AssisstantLineSchedule
Get
HY_DVR_GET_ALARMCFG, ALARMCFG_ALARM_ASSISSTANT_LINE_INFO
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
Put
HY_DVR_SET_ALARMCFG, ALARMCFG_ALARM_ASSISSTANT_LINE_INFO

/Wiegand/ChannelID/BasicSettings
Get
HY_DVR_GET_WIEGAND_SETTINGS, WIEGAND_BASIC_SETTINGS
Put
HY_DVR_GET_WIEGAND_SETTINGS, WIEGAND_BASIC_SETTINGS
HY_DVR_SET_WIEGAND_SETTINGS, WIEGAND_BASIC_SETTINGS

/Wiegand/ChannelID/Test
Put
HY_DVR_SET_WIEGAND_SETTINGS, WIEGAND_TEST
HY_DVR_GET_WIEGAND_SETTINGS, WIEGAND_TEST

/Security/ChannelID/UserPermissionV1ByUsername
Get
HY_DVR_GET_USERCFG, USERCFG_PERMISSION_CONFIG_ONE_V1
Put
HY_DVR_GET_USERCFG, USERCFG_PERMISSION_CONFIG_ONE_V1
HY_DVR_SET_USERCFG, USERCFG_PERMISSION_CONFIG_ONE_V1

/MegLink/ChannelID/ServerAddress
Get
HY_DVR_GET_MEGLINK_HTTP_REQUEST_INFO, MEGLINK_HTTP_REQUEST_INFO
Put
HY_DVR_GET_MEGLINK_HTTP_REQUEST_INFO, MEGLINK_HTTP_REQUEST_INFO
HY_DVR_SET_MEGLINK_HTTP_REQUEST_INFO, MEGLINK_HTTP_REQUEST_INFO

/MegLink/ChannelID/AuthInfo
Get
HY_DVR_GET_MEGLINK_HTTP_REQUEST_INFO, MEGLINK_HTTP_REQUEST_INFO
Put
HY_DVR_GET_MEGLINK_HTTP_REQUEST_INFO, MEGLINK_HTTP_REQUEST_INFO
HY_DVR_SET_MEGLINK_HTTP_REQUEST_INFO, MEGLINK_HTTP_REQUEST_INFO

/MegLink/ChannelID/DeviceInfo
Get
HY_DVR_GET_MEGLINK_DEVICE_INFO, MEGLINK_DEVICE_INFO

/MegLink/ChannelID/ErrorInfo
Get
HY_DVR_GET_MEGLINK_DEVICE_INFO, MEGLINK_ERROR_INFO

/Health/ChannelID/BaseConfig
Get
HY_DVR_GET_HEALTH_CONFIG, HEALTH_BASE_CONFIG
Put
HY_DVR_GET_HEALTH_CONFIG, HEALTH_BASE_CONFIG
HY_DVR_SET_HEALTH_CONFIG, HEALTH_BASE_CONFIG

/TianYi/ChannelID/AuthInfo
Get
HY_DVR_GET_TIANYI_DEVICE_INFO_EX, AUTH_INFO
Put
HY_DVR_SET_TIANYI_DEVICE_INFO_EX, AUTH_INFO

/Unicom/ChannelID/AuthInfo
Get
HY_DVR_GET_UNICOM_DEVICE_INFO, UNICOM_AUTH_INFO
Put
HY_DVR_SET_UNICOM_DEVICE_INFO, UNICOM_AUTH_INFO

/CM/ChannelID/AuthInfo
Get
HY_DVR_GET_CMDOT_CFG, CMDOT_AUTH_INFO
Put
HY_DVR_SET_CMDOT_CFG, CMDOT_AUTH_INFO

/WeChat/ChannelID/BasicStatus
Get
HY_DVR_GET_WXBASIC_STATUS, BASIC_STATUS
Put
HY_DVR_SET_WXBASIC_STATUS, BASIC_STATUS

/WeChat/ChannelID/BasicInfo
Get
HY_DVR_GET_WXBASIC_STATUS, BASIC_INFO

/CloudUpdate/ChannelID/BasicInfo
Get
HY_DVR_GET_DEVICEINFO, CLOUD_UPDATE_DEVICE_INFO

/AI/ChannelID/IntelligentTrack
GET
HY_DVR_GET_INTELLIGENT_TRACK_CFG, INTELLIGENT_TRACK_CFG
SET
HY_DVR_SET_INTELLIGENT_TRACK_CFG, INTELLIGENT_TRACK_CFG

/AI/IntelligentTrack
GET
HY_DVR_GET_INTELLIGENT_TRACK_CFG, INTELLIGENT_TRACK_CFG
SET
HY_DVR_SET_INTELLIGENT_TRACK_CFG, INTELLIGENT_TRACK_CFG

/AI/OffDutyQuery
GET
HY_DVR_GET_AI_OFFDUTY_QUERY, AI_OFFDUTY_QUERY

/AI/CryScreamDetect
GET
HY_DVR_GET_CRY_SCREAM_DETECT, CRY_SCREAM_DETECT_CFG
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
SET
HY_DVR_SET_CRY_SCREAM_DETECT, CRY_SCREAM_DETECT_CFG
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL

/AI/BreathMaskDetectCfg
GET
HY_DVR_GET_AI_BREATHMASK_DETECT_CFG, AI_BREATHMASK_DETECT_CFG
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
SET
HY_DVR_SET_AI_BREATHMASK_DETECT_CFG, AI_BREATHMASK_DETECT_CFG
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL

/AI/FaceSnapshotCfg
GET
HY_DVR_GET_AI_FACE_SNAPSHOT_CFG, AI_FACE_SNAPSHOT_CFG
HY_DVR_GET_AI_FACE_SNAPSHOT_CFG, AI_FACE_SNAPSHOT_EVENT_VOICEALARM_TIMESLOT_CFG
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_AI_FACE_SNAPSHOT_CFG, AI_FACE_SNAPSHOT_EVENT_TIMESLOT_CFG
PUT
HY_DVR_GET_AI_FACE_SNAPSHOT_CFG, AI_FACE_SNAPSHOT_EVENT_VOICEALARM_TIMESLOT_CFG
HY_DVR_SET_AI_FACE_SNAPSHOT_CFG, AI_FACE_SNAPSHOT_EVENT_VOICEALARM_TIMESLOT_CFG
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_AI_FACE_SNAPSHOT_CFG, AI_FACE_SNAPSHOT_EVENT_TIMESLOT_CFG
HY_DVR_SET_AI_FACE_SNAPSHOT_CFG, AI_FACE_SNAPSHOT_EVENT_TIMESLOT_CFG
HY_DVR_SET_AI_FACE_SNAPSHOT_CFG, AI_FACE_SNAPSHOT_CFG

/AI/FaceAlgorithmVersion
GET
HY_DVR_GET_AI_FACE_ALGORITHM_VERSION, AI_FACE_ALGORITHM_VERSION
PUT
HY_DVR_SET_AI_FACE_ALGORITHM_VERSION, AI_FACE_ALGORITHM_VERSION

/AI/CarPlateSnap
GET
HY_DVR_GET_CAR_PLATE_SNAP, CAR_PLATE_SNAP_CONFIG
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_CAR_PLATE_SNAP, CAR_PLATE_SNAP_EVENT_TIMESLOT_CFG
HY_DVR_GET_CAR_PLATE_SNAP, CAR_PLATE_SNAP_EVENT_VOICEALARM_TIMESLOT_CFG
PUT
HY_DVR_GET_CAR_PLATE_SNAP, CAR_PLATE_SNAP_CONFIG
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_CAR_PLATE_SNAP, CAR_PLATE_SNAP_EVENT_TIMESLOT_CFG
HY_DVR_SET_CAR_PLATE_SNAP, CAR_PLATE_SNAP_EVENT_TIMESLOT_CFG
HY_DVR_GET_CAR_PLATE_SNAP, CAR_PLATE_SNAP_EVENT_VOICEALARM_TIMESLOT_CFG
HY_DVR_SET_CAR_PLATE_SNAP, CAR_PLATE_SNAP_EVENT_VOICEALARM_TIMESLOT_CFG
HY_DVR_SET_CAR_PLATE_SNAP, CAR_PLATE_SNAP_CONFIG

/AI/DeviceNeedReboot
POST
HY_DVR_GET_DEVICE_NEED_REBOOT, DEVICE_NEED_REBOOT

/System/LogFileDownloadXTCustom
GET

/System/FrontDeviceOwnnerInfo
GET
HY_DVR_GET_FRONT_DEVICE_OWNNER_INFO, FRONT_DEVICE_OWNNER_INFO
PUT
HY_DVR_SET_FRONT_DEVICE_OWNNER_INFO, FRONT_DEVICE_OWNNER_INFO

/System/DeviceIntercomMode
GET
HY_DVR_GET_DEVICE_INTERCOM_MODE, DEVICE_INTERCOM_MODE
PUT
HY_DVR_SET_DEVICE_INTERCOM_MODE, DEVICE_INTERCOM_MODE

/System/DeviceOneButtonCall
GET
PUT
HY_DVR_SET_DEVICE_ONE_BUTTON_CALL_CFG, DEVICE_ONE_BUTTON_CALL_CFG

/System/DetectDeviceNetworkStatus
POST


/System/DaJiangIOTServerCfgCustom
GET
HY_DVR_GET_DAJIANG_IOT_SERVER_CFG_CUSTOM,DAJIANG_IOT_SERVER_CFG_CUSTOM
PUT
HY_DVR_SET_DAJIANG_IOT_SERVER_CFG_CUSTOM,DAJIANG_IOT_SERVER_CFG_CUSTOM

/System/DaJiangIOTServerConnectStatus
GET
HY_DVR_GET_DAJIANG_IOT_SERVER_CFG_CUSTOM,DAJIANG_IOT_SERVER_CONNECT_STATUS
PUT
HY_DVR_SET_DAJIANG_IOT_SERVER_CFG_CUSTOM,DAJIANG_IOT_SERVER_CONNECT_STATUS

/System/AlgorithmStoreCfg
GET
HY_DVR_GET_ALGORITHM_STORE_CFG, ALGORITHM_STORE_CFG
PUT
HY_DVR_GET_ALGORITHM_STORE_CFG, ALGORITHM_STORE_CFG
HY_DVR_SET_ALGORITHM_STORE_CFG, ALGORITHM_STORE_CFG

/System/AlgoMallTransparentTransmissCfg
GET
HY_DVR_GET_ALGO_MALL_TRANSPARENT_TRANSMISSION_CFG, ALGO_MALL_TRANSPARENT_TRANSMISSION_CFG
PUT
HY_DVR_GET_ALGO_MALL_TRANSPARENT_TRANSMISSION_CFG, ALGO_MALL_TRANSPARENT_TRANSMISSION_CFG
HY_DVR_SET_ALGO_MALL_TRANSPARENT_TRANSMISSION_CFG, ALGO_MALL_TRANSPARENT_TRANSMISSION_CFG

/System/AlgoCustom
PUT
HY_DVR_SET_ALGO_CUSTOM, ALGO_CUSTOM

/System/LightStripControl
GET
HY_DVR_GET_LIGHT_STRIP_CONTROL, LIGHT_STRIP_CONTROL
PUT
HY_DVR_GET_LIGHT_STRIP_CONTROL, LIGHT_STRIP_CONTROL
HY_DVR_SET_LIGHT_STRIP_CONTROL, LIGHT_STRIP_CONTROL

/System/PetDetect
GET
HY_DVR_GET_PET_DETECT, PET_DETECT
PUT
HY_DVR_GET_PET_DETECT, PET_DETECT
HY_DVR_SET_PET_DETECT, PET_DETECT

/System/TeasePetLaser
GET
HY_DVR_GET_TEASE_PET, TEASE_PET_LASER
PUT
HY_DVR_GET_TEASE_PET, TEASE_PET_LASER
HY_DVR_SET_TEASE_PET, TEASE_PET_LASER

/System/AriDectAudioLightAlarm
GET
HY_DVR_GET_DEVICE_AIR_DECTECT_INFO, AIR_DECTECT_AUDIO_LIGHT_ALARM_CFG
PUT
HY_DVR_SET_DEVICE_AIR_DECTECT_INFO, AIR_DECTECT_AUDIO_LIGHT_ALARM_CFG

/Network/CloudStorageParamConfig
GET
HY_DVR_GET_BUFFER_SIZE, BUFFER_SIZE_ALL
PUT
HY_DVR_SET_BUFFER_SIZE, BUFFER_SIZE_ALL

/Network/4GCardSelectResult
Get
HY_DVR_GET_4G_CARD_SELECT_CFG, HY_DVR_4G_CARD_SELECT_RESULT
Put
NotSupport

/ProductTool/LampCfg
GET
HY_DVR_GET_PRODUCT_TOOL_LAMP_CFG, PRODUCT_TOOL_LAMP_CFG
PUT
HY_DVR_SET_PRODUCT_TOOL_LAMP_CFG, PRODUCT_TOOL_LAMP_CFG

/ipc/getkey
POST
HY_DVR_GET_IPC_KEY_JIESHUN_CUSTOM, IPC_KEY_JIESHUN_CUSTOM

/LicensePlateGroup/Create
GET
None
PUT
HY_DVR_SET_LICENSE_PLATE_GROUP, CREATE_LICENSE_PLATE_GROUP

/LicensePlateGroup/Delete
GET
None
PUT
HY_DVR_SET_LICENSE_PLATE_GROUP, DELETE_LICENSE_PLATE_GROUP

/LicensePlateGroup/Modify
GET
None
PUT
HY_DVR_SET_LICENSE_PLATE_GROUP, MODIFY_LICENSE_PLATE_GROUP

/LicensePlateGroup/QueryALL
GET
None
PUT
HY_DVR_GET_LICENSE_PLATE_GROUP, QUERYALL_LICENSE_PLATE_GROUP

/LicensePlateGroup/OneClickDeleteALL
GET
None
PUT
HY_DVR_SET_LICENSE_PLATE_GROUP, ONE_CLICK_DELETE_ALL_LICENSE_PLATE_GROUP

/LicensePlateGroup/UpdateLicensePlateInfoList
GET
None
PUT
HY_DVR_SET_LICENSE_PLATE_GROUP, UPDATE_LICENSE_PLATE_INFO_LIST

/LicensePlateGroup/DeleteLicensePlateInfoList
GET
None
PUT
HY_DVR_SET_LICENSE_PLATE_GROUP, DELETE_LICENSE_PLATE_INFO_LIST

/LicensePlateGroup/QueryLicensePlateInfoList
GET
None
PUT
HY_DVR_GET_LICENSE_PLATE_GROUP, QUERY_LICENSE_PLATE_INFO_LIST


/System/DevOneBtnCallFuncCfg
GET
HY_DVR_GET_DEVICE_ONE_BUTTON_CALL_CFG,DEVICE_ONE_BUTTON_CALL_FUNC_CFG
PUT
HY_DVR_SET_DEVICE_ONE_BUTTON_CALL_CFG,DEVICE_ONE_BUTTON_CALL_FUNC_CFG

/System/NVRRegisterCfg
GET
HY_DVR_GET_REGISTER_NVR, SERVER_INFO
PUT
HY_DVR_SET_REGISTER_NVR, SERVER_INFO

/System/NVRRegisterState
GET
HY_DVR_GET_REGISTER_NVR, SERVER_STA_REGISTER_STATE
PUT
HY_DVR_SET_REGISTER_NVR, SERVER_STA_REGISTER_STATE

/System/DeviceDigitalMultiple
GET
HY_DVR_GET_DEVICE_DIGITAL_MULTIPLE, DEVICE_DIGITAL_MULTIPLE
PUT
HY_DVR_SET_DEVICE_DIGITAL_MULTIPLE, DEVICE_DIGITAL_MULTIPLE

/System/P6SEventMQTTConfig
GET
HY_DVR_GET_P6SEVENT_MQTT_CONFIG, P6SEVENT_MQTT_CONFIG
PUT
HY_DVR_SET_P6SEVENT_MQTT_CONFIG, P6SEVENT_MQTT_CONFIG

/System/WhiteLightEnable
GET
HY_DVR_GET_WHITE_LIGHT_ENABLE, WHITE_LIGHT_ENABLE
PUT
HY_DVR_SET_WHITE_LIGHT_ENABLE, WHITE_LIGHT_ENABLE

/System/VoiceBroadcastLanguage
GET
HY_DVR_GET_VOICE_BROADCAST_LANGUAGE, VOICE_BROADCAST_LANGUAGE
PUT
HY_DVR_SET_VOICE_BROADCAST_LANGUAGE, VOICE_BROADCAST_LANGUAGE

/System/IndicatorLightCfg
GET
HY_DVR_GET_INDICATOR_LIGHT_CFG, INDICATOR_LIGHT_CFG
PUT
HY_DVR_SET_INDICATOR_LIGHT_CFG, INDICATOR_LIGHT_CFG

/System/AtmosphereLightCfg
GET
HY_DVR_GET_ATMOSPHERE_LIGHT_CFG, ATMOSPHERE_LIGHT_CFG
PUT
HY_DVR_GET_ATMOSPHERE_LIGHT_CFG, ATMOSPHERE_LIGHT_CFG
HY_DVR_SET_ATMOSPHERE_LIGHT_CFG, ATMOSPHERE_LIGHT_CFG

/System/BTParkingSpaceBasicCfg
GET
HY_DVR_GET_BT_GROUND_LOCK_CFG, BT_BASIC_CFG
PUT
HY_DVR_SET_BT_GROUND_LOCK_CFG, BT_BASIC_CFG

/System/BTDeviceList
GET
HY_DVR_GET_BT_GROUND_LOCK_CFG, BT_DEVICE_LIST

/System/ParkingSpaceCfg
GET
HY_DVR_GET_BT_GROUND_LOCK_CFG, BT_PARKING_SPACE_LIST
PUT
HY_DVR_SET_BT_GROUND_LOCK_CFG, BT_PARKING_SPACE_LIST

/System/GroundoLockControl
POST
HY_DVR_GET_BT_GROUND_LOCK_CFG, BT_GROUND_LOCK_CTRL_STATUS

/System/GroundLockStatus
GET
HY_DVR_GET_BT_GROUND_LOCK_CFG, BT_GROUND_LOCK_CTRL_STATUS

/System/DevicePowerManageCfg
GET
HY_DVR_GET_POWER_MANAGE_CFG, POWER_MANAGE_CFG
PUT
HY_DVR_SET_POWER_MANAGE_CFG, POWER_MANAGE_CFG

/System/DeviceSIMCardInfo
GET
HY_DVR_GET_SIM_CARD_INFO, SIM_CARD_INFO

/System/DeviceNightVisionCfg
GET
HY_DVR_GET_NIGHT_VISION_CFG, NIGHT_VISION_CFG
PUT
HY_DVR_SET_NIGHT_VISION_CFG, NIGHT_VISION_CFG

/System/ParkingSpaceBasicCfg
GET
HY_DVR_GET_PARKING_SPACE_CFG, PARKING_SPACE_CFG
PUT
HY_DVR_SET_PARKING_SPACE_CFG, PARKING_SPACE_CFG

/System/DeviceCustomConfig
GET
HY_DVR_GET_DEVICEINFO, DEVICEINFO_CUSTOMER
PUT
HY_DVR_GET_DEVICEINFO, DEVICEINFO_CUSTOMER
HY_DVR_SET_DEVICEINFO, DEVICEINFO_CUSTOMER

/System/DoNotDisturbMode
GET
HY_DVR_GET_DO_NOT_DISTURB_MODE_CFG, DO_NOT_DISTURB_MODE_CFG
PUT
HY_DVR_GET_DO_NOT_DISTURB_MODE_CFG, DO_NOT_DISTURB_MODE_CFG
HY_DVR_SET_DO_NOT_DISTURB_MODE_CFG, DO_NOT_DISTURB_MODE_CFG

/System/GroundLockControlV2
PUT
HY_DVR_SET_GROUND_LOCK_CTRL_STATUS, GROUND_LOCK_CTRL_STATUS

/System/GroundLockControlV2OptResult
GET
HY_DVR_GET_GROUND_LOCK_CTRL_STATUS, GROUND_LOCK_CTRL_V2_OPT_RESULT

/System/ChannelID/ElectronicDenceUIDesignCfg
GET
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_ELECTRONIC_FENCE, HY_DVR_ELECTRONIC_FENCE_UI_DESIGN
PUT
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_SET_ELECTRONIC_FENCE, HY_DVR_ELECTRONIC_FENCE_UI_DESIGN

/System/ChannelID/ElectronicDenceCfgEx
GET
HY_DVR_GET_ELECTRONIC_FENCE, ELECTRONIC_FENCE_EX_CFG
PUT
HY_DVR_GET_ELECTRONIC_FENCE, ELECTRONIC_FENCE_EX_CFG
HY_DVR_SET_ELECTRONIC_FENCE, ELECTRONIC_FENCE_EX_CFG

/System/ChannelID/CrossBorderDetectUIDesignInfo
GET
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_CROSSBORDER_DETECT, HY_DVR_CROSSBORDER_DETECT_UI_DESIGN_INFO
PUT
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_SET_CROSSBORDER_DETECT, HY_DVR_CROSSBORDER_DETECT_UI_DESIGN_INFO

/System/ChannelID/CrossBorderDetectCfgEx
GET
HY_DVR_GET_CROSSBORDER_DETECT, CROSSBORDER_DETECT_CFG_EX
PUT
HY_DVR_GET_CROSSBORDER_DETECT, CROSSBORDER_DETECT_CFG_EX
HY_DVR_SET_CROSSBORDER_DETECT, CROSSBORDER_DETECT_CFG_EX

/System/ChannelID/Time
GET
HY_DVR_GET_DEVICECFG, DEVICECFG_ALL HY_DVR_DEVICE_CFG
HY_DVR_GET_SYSTIME, SYSTIME_ALL HY_DVR_TIME
PUT
HY_DVR_GET_DEVICECFG, DEVICECFG_ALL HY_DVR_DEVICE_CFG
HY_DVR_GET_SYSTIME, SYSTIME_ALL HY_DVR_TIME
HY_DVR_SET_DEVICECFG, DEVICECFG_ALL HY_DVR_DEVICE_CFG
HY_DVR_SET_SYSTIME, SYSTIME_ALL HY_DVR_TIME

/System/ChannelID/IntelligentTrack
GET
HY_DVR_GET_INTELLIGENT_TRACK_CFG, INTELLIGENT_TRACK_CFG
HY_DVR_GET_INTELLIGENT_TRACK_CFG, INTELLIGENT_TRACK_EVENT_VOICEALARM_TIMESLOT_CFG
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_INTELLIGENT_TRACK_CFG, INTELLIGENT_TRACK_EVENT_TIMESLOT_CFG
PUT
HY_DVR_GET_INTELLIGENT_TRACK_CFG, INTELLIGENT_TRACK_CFG
HY_DVR_GET_INTELLIGENT_TRACK_CFG, INTELLIGENT_TRACK_EVENT_VOICEALARM_TIMESLOT_CFG
HY_DVR_SET_INTELLIGENT_TRACK_CFG, INTELLIGENT_TRACK_EVENT_VOICEALARM_TIMESLOT_CFG
HY_DVR_GET_DEVICEINFO, DEVICEINFO_ALL
HY_DVR_GET_INTELLIGENT_TRACK_CFG, INTELLIGENT_TRACK_EVENT_TIMESLOT_CFG
HY_DVR_SET_INTELLIGENT_TRACK_CFG, INTELLIGENT_TRACK_EVENT_TIMESLOT_CFG
HY_DVR_SET_INTELLIGENT_TRACK_CFG, INTELLIGENT_TRACK_CFG

/System/DeviceModuleResetDefault
PUT
HY_DVR_SET_DEVICE_MODULE_RESET_DEFAULT, DEVICE_MODULE_RESET_DEFAULT


/System/P6SCMSConfig
GET
HY_DVR_GET_P6SCMS_CONFIG, P6SCMS_CONFIG
PUT
HY_DVR_GET_P6SCMS_CONFIG, P6SCMS_CONFIG
HY_DVR_SET_P6SCMS_CONFIG, P6SCMS_CONFIG

/System/P6SCMSServerTest
POST
HY_DVR_GET_P6SCMS_CONFIG, P6SCMS_SERVER_CONNECT_TEST(HieServerConfig)



/System/CloudStorageErrorCodeList
GET
PROTOCOL_ZW_CLOUD_STORAGE, HY_DVR_GET_CLOUD_STORAGE, CLOUD_STORAGE_ERROR_CODE_LIST_V1(HieServerConfig)


/System/DevicePowerManageCfgV2
GET
HY_DVR_GET_POWER_MANAGE_CFG, DEVICE_POWER_MANAGE_CFG_V2
PUT
HY_DVR_GET_POWER_MANAGE_CFG, DEVICE_POWER_MANAGE_CFG_V2
HY_DVR_SET_POWER_MANAGE_CFG, DEVICE_POWER_MANAGE_CFG_V2


/System/LightControlCfg
GET
HY_DVR_GET_LIGHT_CONTROL_CFG, LIGHT_CONTROL_CFG
PUT
HY_DVR_GET_LIGHT_CONTROL_CFG, LIGHT_CONTROL_CFG
HY_DVR_SET_LIGHT_CONTROL_CFG, LIGHT_CONTROL_CFG


/System/HTTPSCfg
GET
HY_DVR_GET_HTTPS_CFG, HTTPS_CFG
PUT
HY_DVR_GET_HTTPS_CFG, HTTPS_CFG
HY_DVR_SET_HTTPS_CFG, HTTPS_CFG


/System/HTTPSCertificateCreate
POST
HY_DVR_SET_HTTPS_CFG, HTTPS_CERTIFICATE_CREATE


/System/HTTPSCertificationInfo
GET
HY_DVR_GET_HTTPS_CFG, HTTPS_CERTIFICATION_INFO


/System/HTTPSCertificateDelete
POST
HY_DVR_SET_HTTPS_CFG, HTTPS_CERTIFICATE_DELETE


/System/ChannelID/PersonCenterCfg
GET
HY_DVR_GET_PERSON_CENTER_CFG, PERSON_CENTER_CFG
PUT
HY_DVR_SET_PERSON_CENTER_CFG, PERSON_CENTER_CFG


/System/ScreenManagementCfg
GET
HY_DVR_GET_SCREEN_MANAGEMENT_CFG, SCREEN_MANAGEMENT_CFG
PUT
HY_DVR_GET_SCREEN_MANAGEMENT_CFG, SCREEN_MANAGEMENT_CFG
HY_DVR_SET_SCREEN_MANAGEMENT_CFG, SCREEN_MANAGEMENT_CFG


/System/QueryTimeRemindPlan
GET
HY_DVR_GET_TIME_REMIND, QUERY_TIME_REMIND_CFG


/System/AddTimeRemindPlan
PUT
HY_DVR_SET_TIME_REMIND, ADD_TIME_REMIND_CFG


/System/ModifyTimeRemindPlan
PUT
HY_DVR_SET_TIME_REMIND, MODIFY_TIME_REMIND_CFG


/System/DeleteTimeRemindPlan
PUT
HY_DVR_SET_TIME_REMIND, DELETE_TIME_REMIND_CFG


/System/TimeRemindAudioInfoAdd
GET
PUT
HY_DVR_SET_TIME_REMIND, ADD_TIME_REMIND_AUDIO_INFO


/System/TimeRemindAudioInfoDelete
GET
PUT
HY_DVR_SET_TIME_REMIND, DELETE_TIME_REMIND_AUDIO_INFO


/System/TimeRemindAudioInfoQuery
GET
HY_DVR_GET_TIME_REMIND, QUERY_TIME_REMIND_AUDIO_INFO
PUT


/System/TimeRemindAudioInfoPlay
GET
PUT
HY_DVR_SET_TIME_REMIND, PLAY_TIME_REMIND_AUDIO


/System/RTSPSCfg
GET
HY_DVR_GET_HTTPS_CFG, RTSPS_CFG
PUT
HY_DVR_SET_HTTPS_CFG, RTSPS_CFG


*/
