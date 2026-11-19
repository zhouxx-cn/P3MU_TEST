#ifndef __GLOBAL_STRING_H__
#define __GLOBAL_STRING_H__



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////* 字符�?*////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//!\brief 管理员用户名
#define ADMIN_USRNAME "admin"
//!\brief 默认登陆用户�?
#define DEFAULT_USRNAME "default"
//!\brief 默认超级管理员用�?
#define SUPER_ADMIN_USRNAME "super_admin"
//!\brief 管理员和默认用户的初始密�?普通版�?
#define DEFAULT_PASSWORD ""

#define GET_LICENSE_FROM_ENV_CMD      "fw_printenv -n auth_license"
#define SET_LICENSE_TO_ENV_CMD        "fw_setenv auth_license"

#define GET_SENSOR_NAME_FROM_ENV_CMD  "fw_printenv -n sensor_name"
#define SET_SENSOR_NAME_TO_ENV_CMD    "fw_setenv sensor_name"

#define GET_BOARDVER_FROM_ENV_CMD    "fw_printenv -n boardver"
#define SET_BOARDVER_TO_ENV_CMD      "fw_setenv boardver"

#define GET_MAJOR_FROM_FILE_CMD		"readini /mnt/app/device/DeviceVersion.ini Software Major_version"
#define GET_MINOR_FROM_FILE_CMD		"readini /mnt/app/device/DeviceVersion.ini Software Minor_version"
#define GET_REVER_FROM_FILE_CMD		"readini /mnt/app/device/DeviceVersion.ini Software Reversion"
#define GET_DATE_FROM_FILE_CMD		"readini /mnt/app/device/DeviceVersion.ini Software BuildDate"


/* __AUTH_V2__ 未使用了 */
#if defined (__AUTH_V2__)
#define GET_CONNECT_STRING_TO_ENV_CMD    "fw_printenv -n connect_string"
#define SET_CONNECT_STRING_TO_ENV_CMD    "fw_setenv connect_string"

#define GET_PLATFORM_TO_ENV_CMD		"fw_printenv -n platform"
#define SET_PLATFORM_TO_ENV_CMD		"fw_setenv platform"
#define GET_PUSH_KEY_TO_ENV_CMD		"fw_printenv -n push_key"
#define SET_PUSH_KEY_TO_ENV_CMD		"fw_setenv push_key"
#define GET_CRC_TO_ENV_CMD			"fw_printenv -n crc"
#define SET_CRC_TO_ENV_CMD			"fw_setenv crc"
#endif
#define SET_TUTKUID_TO_ENV_CMD			"fw_setenv tutk_uid"
#define SET_BOARD_ID_TO_ENV_CMD         "fw_setenv board_id"
#define GET_TUTKUID_TO_ENV_CMD			"fw_printenv -n tutk_uid"
#define VEHICLE_ALGO_BOOTARGS	        "fw_setenv bootargs 'console=ttyS1,115200n8 mem=64704K@0x0 rmem=41792K@0x3F30000 nmem=24M@0x6800000 init=/linuxrc rootfstype=squashfs root=/dev/mtdblock2 mtdparts=sfc0_nand:2M(uboot),1M(env),5M(rootfs),5M(rootfsb),68M(data),26M(datb),4M(back),15M(storage),2M(devinfo) lpj=11968512'"
#define FACE_ALGO_BOOTARGS		        "fw_setenv bootargs 'console=ttyS1,115200n8 mem=73920K@0x0 rmem=41792K@0x4830000 nmem=15M@0x7100000 init=/linuxrc rootfstype=squashfs root=/dev/mtdblock2 mtdparts=sfc0_nand:2M(uboot),1M(env),5M(rootfs),5M(rootfsb),68M(data),26M(datb),4M(back),15M(storage),2M(devinfo) lpj=11968512'"
#define NMEM_SIZE				        "fw_printenv | grep bootargs | awk -F'[=@ ]' '{print $11}' | rev | cut -b 2-$n | rev"
#define RESTART_TIMES					"fw_setenv restart_times 0"
#define SAVE_ENV					    "fw_setenv saveenv"

#define PARTITION_USED				    "df | grep storage | awk -F'[ %]+' '{print $5}'"

#define OSD_TEXT_EX_TOKEN	            "osd_token_1"
#define OSD_TEXT_CONTENT_START	        "t="
#define OSD_TEXT_X_START	            "&x="
#define OSD_TEXT_Y_START	            "&y="
#define OSDV2_TEXT_NEW_LINE_CHAR		'^'
#define OSDV2_TEXT_KEYWORD_START_CHAR	'['

#define CLOUD_UPGRADE_CUSTOM_SECTION_NAME			"CloudUpgrade"
#define CLOUD_UPGRADE_CUSTOM_CUSTOM_NO_KEY_NAME		"CustomNo"
#define CLOUD_UPGRADE_CUSTOM_CUSTOM_NO_VALUE_DEF	"V001"

#define SendMSGToBoa                "killall -1 boa"
#define LOGIN_DEFAULT_IP            "255.255.255.255"

#define CLOUD_UPGRADE_CUSTOM_CLIENT_ID_KEY_NAME		"ClientId"
#define CLOUD_UPGRADE_CUSTOM_CLIENT_ID_VALUE_DEF	"C000"
#define CLOUD_UPGRADE_USRINFO_START_FLAG			":"
#define CLOUD_UPGRADE_DOWNLOAD_PROTOCOL_HTTP		"http"
#define CLOUD_UPGRADE_DOWNLOAD_PROTOCOL_FTP			"ftp"
#define CLOUD_UPGRADE_DOWNLOAD_PROTOCOL_SFTP		"sftp"

#define TRANSPARENT_DATA_MAIN_COMM 			"Main-Command:"
#define TRANSPARENT_DATA_MINOR_COMM 		"Assist-Command:"
#define TRANSPARENT_DATA_LEN 				"Payload-Length:"
#define TRANSPARENT_DATA_CONTENET_HEAD 		"Content:"
#define AMR_FORMAT_HEAD						"#!AMR"
#define DIYDDNSNAME                         "diyddns"
#define	NET_EMAIL_CFG			            "Email_Cfg"

#define WIFI_STA_KO_NAME     "nl80211Usta"
#define WIFI_AP_KO_NAME      "nl80211Uap"

#define RUN_WPA_SUPPLICANT   "wpa_supplicant -B -iwlan0 -Dnl80211 -c /mnt/mtd/etc/wifi/wpa.conf -f /tmp/wpa.log"
#define DEFAULT_AP_IP        "223.223.100.2"
#define DEFAULT_AP_NETMASK   "255.255.255.0"
#define DEFAULT_AP_GATEWAY   "223.223.100.1"
#define WLAN_DEFAULT_DNS     "114.114.114.114"
#define DEFAULT_AP_PREFIX    "ZWAP_"
#if defined (__T4P_W__)
#define DEFAULT_SPFTAP_PWD   "88888888"  /* 华视安邦定制密码 */
#else
#define DEFAULT_SPFTAP_PWD   "01234567"
#endif

#define ADD_ONVIF
#define PORT_KEY                    "Port "

#define DEVINFO_write              "write"
#define DEVINFO_read               "read"

#define BLUE_CfgSection             "Blue_Test"
#define BLUE_DEV_TYPE               "BlueServer"
#define BLUE_TEST_MAC               "BlueServerMac"
#define BLUE_DEV_NAME               "BlueName"
#define BLUE_TEST_AUDIO             "BlueTestStartAudio"
#define HTTP_EVENT_SERVER_PARAM     "HttpEvent"
#define P6S_SERVER_PARAM            "P6SMessage"

#define ParityStart         "zview"
#define ParityEnd           "tail"
#define ParityKey           "device"
#define ParityVal	        "zviewtech"
#define DEFAULT_PROTOCOL    "PelcoD"
#define PROTOCOL_PREFIX     "lib"
#define PROTOCOL_POSTFIX    ".so"
#define UNINSTALL_FLAG      '_'

//!\brief 云台复位配置section name
#define PTZRESET_SECTION "PTZReset"
//!\brief 云台复位配置动作key前缀
#define PTZRESET_ENABLE_PREFIX "Enable"
//!\brief 云台启用key前缀
#define PTZRESET_ENABLE_PTZ "Enable_PTZ"
//!\brief 云台复位配置动作key前缀
#define PTZRESET_CYCLE_PREFIX "Cycle"
//!\brief 云台复位配置动作key前缀
#define PTZRESET_ACTION_PREFIX "Action"
#define PTZRESET_ADVANCE_CFG "PTZ_Advance"
#define PTZ_CAP "PTZCap_0"

#define GUI_KEY			        "bgui"
#define RECORD_KEY              "brecord"
#define ALARMOUT_KEY            "balarmout"
#define SCREEN_KEY              "bscreen"
#define BEEP_KEY                "bbeep"
#define TOSERVER_KEY            "bserver"
#define EMAIL_KEY               "bmail"
#define SMS_KEY                 "bsms"
#define PTZ_KEY                 "ptz"
#define SNAPSHOT_KEY            "bsnapshot"
#define FTP_KEY			        "bftp"
#define PUSH_KEY		        "bpush"
#define MASK_KEY		        "bmask"
#define LIGHTALARM_KEY	        "blightalarm"

#define CH_KEY                  "bch"
#define _DPM_VERSION__          "1.0.0 svn:1234"
#define VIDEODIR                "videoout"
#define BIN_MAGIC_CODE			"BINMAGIC"
#define BOARD_INFO_MAGIC_CODE   "INFMAGIC"
#define IMAGE_INFO_MAGIC_CODE   "IMGMAGIC"

#define  SECTION	            "summer"
#define  BENABLE                "benable"
#define  TIME		            "num"
#define	 N_STATUS               "status"
#define  BEGIN                  "begin"           
#define  END                    "end"
#define  MODE		            "mode"
#define  BEGIN_WEEK	            "begin_week"
#define  END_WEEK	            "end_week"

#define ILLEGEACCESS_SECTION    "IllegeAccessCfg"
#define AGING_TEST_MODULE_NAME  "[aging_test]"
#define CFG_FILE_HEAD 	        "dvr=idvr9000"
#define CFG_FILE_END 	        "[end]"
#define CFG_FILE_FLAG	        ";******;"
#define CFG_FILE_HEAD_FLAG 	    "[system]\ndvr=idvr9000\n"
#define CFG_FILE_END_FLAG 	    "\n[end]\ndvr=idvr9000\n"
#define IMAGECFG                "ImageCfg"


#define PUSH_EVENT_INTV                     "PushEventIntv"
#define CROSS_BOARD_DETECT_CFG_SECTION      "CrossBoardDetectCfg"
#define VIDEO_LOSE_CFG_SECTION              "VideoLostCfg"
#define DEFENCE_AREA_CFG_SECTION            "DefenseAreaCfg"
#define CAPTURE_PIC_CFG_SECTION             "CapturePicCfg"
#define KEY_CAPTURE_PIC_COUNT               "CapturePicCount"
#define KEY_CAPTURE_PIC_INTERVAL            "CapturePicInterval"
#define KEY_CAPTURE_PIC_FORMAT              "CapturePicFormat"
#define KEY_CAPTURE_PIC_WIDTH               "CapturePicWidth"
#define KEY_CAPTURE_PIC_HEIGHT              "CapturePicHeight"
#define KEY_CAPTURE_PIC_QUALITY             "CapturePicQuality"
#define KEY_CAPTURE_PIC_STREAMTYPE          "CapturePicStreamType"
#define CAR_DETECT_CFG_SECTION              "CarDetectCfg"
#define SCHEDULE_TIME_ONE                   "111111111111111111111111111111111111111111111111"
#define SCHEDULE_TIME_ZERO                  "000000000000000000000000000000000000000000000000"
#define ALARM_DEFAULT_VALUE                 "803::::::::::::::::::::::::::::::::::::::"
#define CoderErrorCfgSection                    "CoderErrorCfg"
#define MainCoderCfgSection                     "RecordCfg"
#define AuxCoderCfgSection                      "AuxCoderCfg"
#define AudioAlarmCfgSection                    "AudioAlarmCfg"
#define HDAuxCoderCfgSection                    "HDAuxCoderCfg"
#define DefHDAuxEncCfgStr                       "0:255:1:0:0:0:512:0"
#define HDFourthStreamCfgSection                "FourthStreamCfg"
#define DefFourthStreamCfgStr                   "0:255:1:0:0:0:512:0"
#define FaceGroupCfgSection                     "FaceGroupCfg"
#define FaceRecoCfgSection                      "FaceRecoCfg"
#define FaceCapSimplifyCfgSection               "FaceCapSimplifyCfg"
#define FaceCapSimplify_MinFaceW_key            "MinFaceW"
#define FaceCapSimplify_MinFaceH_key            "MinFaceH"
#define FaceCapSimplify_MaxFaceW_key            "MaxFaceW"
#define FaceCapSimplify_MaxFaceH_key            "MaxFaceH"
#define FaceCapSimplify_CropMode_key            "CropMode"
#define FaceCapSimplify_Region                  "DetectRegion"
#define LightAlarmSectionStr                    "LightAlarmCfg"
#define HumanShapeCfgSection                    "HumanShapeCfg"
#define LicensePlateCapCfgSection               "LicensePlateCapCfg"
#define MaskDetectCfgSection                    "MaskDetectCfg"
#define MosaicCfgSection                        "MosaicCfg"
#define MotionCfgSection                        "MotionCfg"
#define VideoExceptionCfgSection                "VideoExceptionCfg"
#define AlarmInSectionStr                       "AlarmInCfg"
#define AlarmInModeKeyStr                       "AlarmInMode"
#define LongAwayCfgSection                    	"LongAwayCfg"



#define s_AccessPointInfo                       "AccessPointInfo"
#define gszCgiRemoteUpgradeIpAddr               "192.168.1.11"
#define INet_CfgSection                         "INet_Cfg"
#define INet_IP_key                             "LocalIP"
#define INet_Gateway_key                        "LocalGateway"
#define DEVICE_INFO                             "DeviceInfo"
#define GB28181_InstallSec                      "GB28181_Install"

#define TcpRelay                     	"TcpRelay"
#define TcpEnable               		"TcpEnable"

#define GB28181_CfgSection                      "GB28181_Cfg"
#define IAlarmServer_CfgSection                 "AlarmServer_Cfg"
#define DDNS_CfgSection                         "DDNS_Cfg"
#define DDNS_Enable_key                         "DDNS_Enable"
#define NMS_CfgSection                          "NMSCfg"
#define HXHT_CfgSection                         "HXHTCfg"
#define NMS_Mode_key                            "Mode"
#define NMS_Enable_key                          "NMS_Enable"
#define NMS_IP_key                              "NMS_IP"
#define NMS_IP_default                          "192.168.0.30"
#define NMS_Port_key                            "NMS_Port"
#define NMS_RegisterTime_key                    "NMS_RegisterTime"
#define NMS_Protocol_key                        "NMS_Protocol"



#define IPPPOE_CfgSection                           "IPPPOE_Cfg"
#define IFTP_CfgSection                           "FTP_Cfg"
#define INet_IP_Default_s                           "192.168.1.11"
#define INet_SubnetMask_key                         "LocalSubnetMask"
#define INet_SubnetMask_Default_s                   "255.255.255.0"
#define INet_Gateway_Default_s                     "192.168.1.1"
#define INet_WLANIP_Default_s                      "192.168.2.11"
#define INet_HttpPort_key                           "HttpPort"
#define INet_DHCP_key                               "DHCP_Enable"
#define INet_IPAdjust_key                           "IPAdjust_Enable"
#define IFNAME                                      "eth0"
#define GOOGLE_DOMAIN                               "8.8.8.8"
#define GOOGLE_DOMAIN_YL_1                          "217.218.155.155"
#define GOOGLE_DOMAIN_YL_2                          "217.217.127.127"
#define INet_PortCfgSection                           "CIU"
#define INet_RegNVR                                 "RegisterNVR"
#define IFTP_Enable_key                           "FTP_Enable"
#define IFTP_Port_key                             "FTP_Port"
#define IFTP_Type_key                             "FTP_Type"
#define IFTP_Host_key                             "FTP_Host"
#define IFTP_User_key                             "FTP_User"
#define IFTP_Pass_key                             "FTP_Pass"
#define IFTP_Mode_key                             "FTP_Mode"
#define IFTP_RmPath_key                           "FTP_RmPath"
#define IMemrecord_LinkageType                     "MEM_LinkageType"
#define IFTP_Host_Default_s                           "192.168.1.109"
#define IFTP_User_Default_s                           "zhuowei"
#define IFTP_Pass_Default_s                           "zhuowei"
#define IFTP_RmPath_Default_s                          "ftp"
#define HttpSeverProcRestart                           "/etc/init.d/HttpStart start"
#define BoaSeverProcStart                           "/mnt/app/bin/boa -c /home/http -f /mnt/mtd/etc/conf.d/boa/boa.conf &"
#define g_ApplyDidDefaultURL                           "https://auth.p6sai.com:8089/auth-server/api/v2/devicePool/request-did-open"
#define IPUSH_AI_CfgSection                           "PUSH_AI_Cfg"
#define IPUSH_AISubKey_Key                            "PushAiSubKey"
#define IPUSH_AISubKey_Default_s                     "1234567ABCDEFG"
#define INet_WLANDHCP_key                           "Wlan_DHCP_Enable"
#define INet_WLANDNS0_key                           "WLAN_PrimaryDNS"
#define INet_WLANDNS1_key                           "WLAN_SecondaryDNS"
#define INet_WLANNetworkType_key                    "Wlan_NetworkType"
#define INet_WLANAuthMode_key                       "Wlan_AuthMode"
#define INet_WLANEncType_key                        "Wlan_EncType"
#define INet_WLANDefKeyID_key                       "Wlan_DefKeyID"
#define INet_WLANKey1_key                           "Wlan_Key1"
#define INet_WLANKey2_key                           "Wlan_Key2"
#define INet_WLANKey3_key                           "Wlan_Key3"
#define INet_WLANKey4_key                           "Wlan_Key4"
#define INet_WLANWPAKey_key                         "Wlan_WPAKey"
#define INet_WLANSSID_key                           "Wlan_SSID"
#define INET_WLANEnable_key                         "Wlan_Enable"
#define INet_WLAN_AP_TO_STA_FLAG_key                "Wlan_TOSTA_FLAG"
#define INet_WLAN_KEY_ENCODE_ENABLE                 "Wlan_KeyEncodeEnable"
#define WLAN_PrimaryDNS_Default_s                   "1.0.0.0"
#define WLAN_SecondaryDNS_Default_s                 "1.0.0.0"
#define INet_Default_Head_s                         "192.168.254."
#define ClearARPCache                               "arp -n|awk '/^[1-9]/{print \"arp -d  \" $1}'"
#define IHOST_PARSER_CACHE_CONF                     "/mnt/mtd/etc/hosts"
#define IHOST_CONF                                  "/mnt/mtd/config/host.conf"
#define INet_WLANIP_key                             "WlanIP"
#define INet_WLANSubnetMask_key                     "WlanSubnetMask"
#define INet_WLANGateway_key                        "WlanGateway"
#define INet_WLANWifiMode                           "wifiMode"
#define INet_WLANFakeSTAMode                        "wifiFakeSTAModeEnable"

#define USBDEV_IDVENDOR         "/sys/bus/usb/devices/1-1/idVendor"
#define USBDEV_IDPRODUCT        "/sys/bus/usb/devices/1-1/idProduct"

#define INTP_CfgSection                     "INTP_Cfg"
#define IFNAME_PPPOE                        "ppp0"
//配置区名
#define IPPPOE_CfgSection                     "IPPPOE_Cfg"
#define IPPPOE_Enable_key                     "IPPPOE_Enable"
#define IPPPOE_UserName_key                   "IPPPOE_UserName"
#define IPPPOE_UserName_Default_s             "username"
#define IPPPOE_Password_key                   "IPPPOE_Password"
#define IPPPOE_Password_Default_s             ""
#define IPPPOE_PrimaryDNS_key                 "IPPPOE_PrimaryDNS"
#define IPPPOE_PrimaryDNS_Default_s           "114.114.114.114"
#define IPPPOE_SecondaryDNS_key               "IPPPOE_SecondaryDNS"
#define IPPPOE_SecondaryDNS_Default_s         "114.114.114.114"
//配置区名
#define INet_UPNP_key                           "UPNP_Enable"
#define INet_UPNP_HTTP_port                     "UPNP_HTTP_port"
#define INet_UPNP_msg_port                      "UPNP_msg_port"
#define INet_UPNP_media_port                    "UPNP_media_port"
#define INet_UPNP_rtsp_port                     "UPNP_rtsp_port"

#define DEFAULT_RESOLV_CONF  "8.8.8.8"		//默认dns  ip
#define DEFAULT_RESOLV_CONF_LOCAL  "114.114.114.114"		//国内默认dns  ip

#define XRS_DOMAIN_NAME_MANAGER             "zwerp.zwcloud.wang@8860"
#define ZWHOST_DOMAIN_RESOLUTION            "device-ddns-speed.p6sai.com"
#define ZWHOST_RESOLUTION_TIMEOUT           (3 * 1000)  //超时时间 毫秒


#define SPEECH_MOTION_ALARM_CUSTOM_FILE			"/mnt/mtd/config/motionCustomAlarm.g711u" //自定义语音路�?

#endif
