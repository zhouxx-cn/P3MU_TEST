#ifndef __GLOBAL_FILEPATH_H__
#define __GLOBAL_FILEPATH_H__

/* Authority.c */
#define SENSOR_NAME_PATH			   "/tmp/.SensorName"
#define TutkUID_CONF                   "/mnt/mtd/config/tutkuid.conf"
#define AUTHORIZE_LICENSE_FILE_CFG     "/mnt/mtd/config/authorization.cfg"

/* ShareMemory.h */
#define KEY_NAME   						"/tmp"

/* CCapturePic */
#define REMOTE_CAPTURE_IMAGE_FILE_NAME      "/tmp/remote_capture_image.jpeg"
#define RTMP_CONFIG                         "/mnt/mtd/config/rtmp_cfg.ini"
#define RTMP_CONFIG_DEFAULT                 "/mnt/mtd/config/rtmp_cfg.default"

/* CChannelManager */
#define AUTHORIZATION_CFG	        "/mnt/mtd/config/authorization_cfg.ini"
#define AUTHORIZATION_CFG_DEFAULT   "/mnt/mtd/config/authorization_cfg.default"

/* CCoder */
#define TUTK_CONFIG "/mnt/mtd/config/tutk_config.ini"


/* COSD -----新增，原先是直接在函数里面写的路径 */
#define  OSD_HZK_16  "/mnt/app/data/fonts/hzk16"
#define  OSD_ASC_16  "/mnt/app/data/fonts/asc16"
#define  OSD_PIXEL_S  "/usr/res/"
#define  OSD_PIXEL     "/tmp/app/res/"
#define  OSD_BAK_SAVE  "/mnt/mtd/config/.OSD"
#define  OSD_BAK_REVERT0  "/mnt/mtd/config/.OSDCfg:0"
#define  OSD_BAK_REVERT1  "/mnt/mtd/config/.OSDCfg:1"
#define  OSD_BAK_REVERT2  "/mnt/mtd/config/.OSDCfg:2"

/* CFaceCapSimplify */
#define FIRST_OPEN_FACE_FUNC_FLAG 	"/mnt/mtd/config/.fistOpenFaceFunc"

/* DeviceAbility */
#define XML_CFG_PATH              "/mnt/app/device/capacity"

/* MPU_CIU_Config_Dev -----新增，原先是直接在函数里面写的路径 */
#define BUFFER_SIZE_FILE_PATH                "/mnt/app/device/capacity/Bufszinfo.cfg"
#define CAPACITY_FILE_PATH                   "/mnt/app/device/capacity/Capacity.xml"
#define PTZ_CAP_FILE_PATH                    "/tmp/app/device/capacity/PTZCap.xml"
/* MPU_CIU_Config_Net */
#define AP_INFO_FILE                         "/mnt/mtd/config/accesspoint.info"
/* 新增，原先是直接在函数里面写的路径 */
#define NMS_CONFIG_FILE                      "/mnt/mtd/config/nms.cfg"
/* MPU_CIU_Config_Pic */
#define CUSTOM_INFO_FILE                     "/mnt/app/custom/info/custominfo.ini"
/* MPU_CIU_Config_User */
#define UserFile                             "/mnt/mtd/etc/conf.d/boa/userlist.conf"
/* MPU_CIU_Download */
#define FACERECO_FILE_PATH                   "/mnt/mtd/config/faceReco.bin"
/*  MPU_CIU_Upload  */
#define FACE_AUTHFILE_PATH                   "/mnt/mtd/AlgoAuth/af.v2c"
#define CUSTOM_MODE_FILE_TMP                 "/tmp/custommode.ini"
/* MPU_PUB_API */
#define CUSTOM_INFO                          "/mnt/app/custom/info/custominfo.ini"


/* IDDNS */
#define DDNS_SERVER_CONFIG_FILE		    "/mnt/mtd/config/DdnsServer.txt"

/* iNet */
#define P2P_LIB_NAME_TUTK               "/usr/lib/libtutkmobile.so"
#define P2P_LIB_NAME_ULK                "/usr/lib/libulk_net.so"
#define P2P_LIB_NAME_SHYUN              "/usr/lib/libzwmobile.so"
//HTTP服务器配置文件路径
#define HTTP_SERVER_CFG_NAME            "/mnt/mtd/config/httpd.conf"
#define HTTP_SERVER_CFG_NAME_BAK        "/mnt/mtd/config/httpd.conf_bak"
#define  DHCP_GW_ADDR                   "/tmp/.tmp.gw.gw.gw"
#define  WLAN_GW_ADDR                   "/tmp/.wlan.gw.gw.gw"

#define PROTOCOL_FILE                   "/home/http/Protocol.html"
#define CUSTOM_FILE                 "/mnt/app/custom/link/linkfile.txt"
#define UDHCPD_CONFIG_FILE			"/mnt/mtd/config/udhcpd/udhcpd.conf"
//attention 有无线的设备，同步时对于无线的网络也需要恢复
#define RECOVER_FILE                "/mnt/mtd/config/.recover" //升级后是否恢复网络的标记
#define SYSTEM_NETWORK_CONF         "/mnt/mtd/etc/network/interfaces"
//产测标识文件(工厂模式=产测模式)
#define PRODUCTTEST_FILE            "/tmp/app/disk/mmcblk0p1/output/ProductTest"
#define PRODUCTTEST_WIFI_FILE		"/tmp/app/disk/mmcblk0p1/SdTypeNetCfg.ini"

/*!<用户绑定后已经检查信号质量完成标记 */
#define BIND_CHECK_SIMCSQ_FLAG	"/mnt/mtd/config/.BindCheckSimCSQFinish"


//P2P发送数据失败标志文件
#define P2P_SEND_DATA_FAIL_FLAG     "/mnt/mtd/.P2PSendDataFail"
// #define PRODUCTTEST_FILE            "/mnt/mtd/ProductTest"
#define AUTH_DID_NUM_FLAG           "/tmp/.auth_did_num_flag"
#define WIFI_MODE_FLAG       		"/mnt/mtd/conig/.wifi_mode"
#define WIFI_CFG_INI                "/mnt/mtd/config/.wificfg.ini"
#define RM_WIFI_CFG_BAK             "rm /mnt/mtd/config/.wificfg.ini-bak"
#define MT7601_DRV_PATH             "/onvif/wifi"
#define WIFI_EXIST_FlAG             "/tmp/.wifi_if_exist"
#define HOSTAPD_CONF_FILE_PATH      "/mnt/mtd/config/hostapd.conf"
#define WIFI_MODE_0_BAK_FILE_PATH   "/mnt/mtd/config/.WlanWifiMode:0"
#define WIFI_MODE_1_BAK_FILE_PATH   "/mnt/mtd/config/.WlanWifiMode:1"
#define WIFI_MODE_2_BAK_FILE_PATH   "/mnt/mtd/config/.WlanWifiMode:2"
#define WIFI_MODE_BAK_FILE_PATH     "/mnt/mtd/config/.WlanWifiMode*"
#define BOA_CONF_PATH               "/mnt/mtd/etc/conf.d/boa/boa.conf"
#define BOA_CONF_BAK_PATH           "/mnt/mtd/etc/conf.d/boa/boa.conf-bak"
#define ULKUID_CONF_BAK             "/mnt/mtd/config/ulkuid.conf_bak"
#define ULKUID_CONF                 "/mnt/mtd/config/ulkuid.conf"

/* CPtzDev */
#define SD_AGING_TEST_LIST_FILE_NAME            "/tmp/aging_test.list"

//硬件af宏
#define AFCUSTOMFILE                            "/mnt/app/custom/info/AfLens.default" //定制文件
#define AFRWPARFILE                             "/mnt/mtd/config/AfInfo.default"//可读写配置文件

/* common_pub */
#define CUSTOM_DEVICE_CAP_FILE                  "/tmp/app/device/capacity/DeviceCap.xml"

/* CBaseUpdate */
#define VERIFY_FILE_NAME                        "/mnt/app/version"
#define VERSION_FILE_NAME                       "/mnt/app/device/DeviceVersion.ini"
#define DSP_ADDR_INFO                           "/tmp/app/.dspAddrInfo"
#define DSP_ADDR_INFO_FOR_MMZ                   "/tmp/app/.dspAddrInfoForMmz"
#define UPDATE_STATE_FILE                       "/tmp/uploadstate"//被写入状态的文件

//最后一次维护时间的保存文件
#define LAST_MAINTENANCETIME_FILE               "/tmp/app/.lastmaintime"
#define TIME_ZONE_CFG                           "/mnt/mtd/config/time_zone"
#define NTP_SERVER                              "/mnt/mtd/config/NtpServer.txt"

/* CSerialDev */
#define TTY_HIDEV                               "/dev/ttyS"
#define CUSTOM_DEVICE_LIGHT_CAP_FILE            "/tmp/app/device/capacity/LightCap.xml"
#define CUSTOM_DEVICE_AI_CAP_FILE               "/tmp/app/device/capacity/AICap.xml"

/* SysManager */
#define SCODE_CONF                              "/mnt/mtd/config/scode.conf"
#define SCODE_BACKUP_CONF                       "/mnt/mtd/config/scode_bak.conf"


#define s_conQrScanResultFilePth                "/tmp/.QrScanResultFile.ini";

/*global_macro.h*/
#define CFGMD5_PATH                             "/mnt/mtd/config/CfgRecordMD5.txt"
#define DEVINFO_APP                             "/mnt/app/bin/DevinfoManyFileTool"
#define CUSTOM_RESTORE_MODE_FLAG	            "/mnt/mtd/config/.custommoderestore.flag"

/*global_macro.h*/
/*!< 默认配置文件路径，一般用于恢复默认配置 */
#define IDVR_DEFAULT_CONFIG_FILE_PATH	        "/mnt/mtd/config/Edvr.default"
/*!< 当前正在使用的配置文件路径 */
#define IDVR_CURRENT_CONFIG_FILE_PATH	        "/mnt/mtd/config/Edvr.cfg"
/*!< 当前正在使用的配置文件的备份 */
#define IDVR_BACKUP_CONFIG_FILE_PATH	        "/mnt/mtd/config/Edvr.cfg-bak"
#define IDVR_RECOVER_CONTROL_CONFIG_FILE_PATH	"/mnt/mtd/config/Edvr.flag"
/*!< 版本信息文件 */
#define IDVR_VERSION_CONFIG_FILE_PATH	        "/mnt/app/device/DeviceVersion.ini"
#define IDVR_DEFAULT_CONFIG_2_FILE_PATH	        "/mnt/app/data/config/config/Edvr.default"
#define IDVR_CUSTOM_FILE_DIR	                "/mnt/app/custom"
/*!< boa 信息 */
#define BOA_FILE_PATH                           "/mnt/mtd/etc/conf.d/boa/"
/*!< boa.conf信息 */
#define BOA_CONF_FILE_PATH                      "/mnt/mtd/etc/conf.d/boa/boa.conf"
/*!< boa.conf-bak信息 */
#define BOA_CONF_BAK_FILE_PATH                  "/mnt/mtd/etc/conf.d/boa/boa.conf-bak"
/*!< boa.conf备份还原信息 */
#define BOA_APP_BAK_FILE_PATH                   "/mnt/app/data/config/etc/conf.d/boa/boa.conf"
/*!< resolv.conf信息 */
#define RESOLV_CONF_BAK_FILE_PATH               "/mnt/app/data/config/etc/resolv.conf"
/*!< resolv_wlan.conf信息 */
#define RESOLV_WLAN_CONF_BAK_FILE_PATH          "/mnt/app/data/config/etc/resolv_wlan.conf"
/*!< resolv_wlan.conf信息 */
#define RESOLV_ETH0_CONF_BAK_FILE_PATH          "/mnt/app/data/config/etc/resolv_eth0.conf"

#define UserFile_BAK       				        "/mnt/app/data/config/etc/conf.d/boa/userlist.conf"
#define PROTOCOL_HTML_PATH					    "/mnt/app/data/config/http/Protocol.html"
#define CONFIG_HTML_PATH                        "/home/http/Config.html"
/*!< tutkuid信息 */
#define TUTK_UID_FILE_PATH                      "/mnt/mtd/config/tutkuid.conf"
#define TUTK_UID_FILE_PATH_BAK                  "/mnt/mtd/config/tutkuid.conf_bak"
// 上一次的TUTU DID配置文件
#define LAST_TUTK_UID_FILE_PATH                 "/mnt/mtd/config/lastTutkUID.conf"
/*!< TUTKUID被改变的标志 */
#define TUTK_UID_CHANGE_FLAG                    "/mnt/mtd/config/DIDChange.flag"
#define DOMAIN_SUFFIX_CONFIG                    "/mnt/mtd/config/DomainSuffix.conf"

/*!< P2P默认使能信息 */
#define P2PENA_SWITCH_FILE_PATH                 "/mnt/mtd/config/P2PEnableSwitch.conf"
/*!< SubKey需要执行清除操作的标记文件 */
#define SUBKEY_NEED_CLEAR_FLAG                  "/mnt/mtd/config/.SubKeyNeedClear"
#define SPS_INFO_CFG                                "/mnt/mtd/config/SPSInfo.conf"
/*!< SubKey需要执行生成操作的标记文件 */
#define SUBKEY_NEED_GENERAL_FLAG                "/mnt/mtd/config/.SubKeyNeedGeneral"
/*!< SubKey需要操作的目录 */
#define SUBKEY_PARAM_DIR                        "/mnt/mtd/config/"
/*!< SubKey需要操作的文件 */
#define SUBKEY_PARAM_CONFIG                     "Subkey_CONF.conf"
/*!< P2PShell可上传下载任意文件标记 协议发送开启Telnet命令后创建*/
#define P2PUDAnyFile                            "/tmp/.P2PShellUpDownLoadFile"
/*!<  SD卡抓图的临时目录 */
#define CAPTURE_MEM_DIR_FOR_SD                  "/tmp/picture/pic_for_sd"
/*!<  FTP抓图的临时目录 */  
#define CAPTURE_MEM_DIR_FOR_FTP                 "/tmp/picture/pic_for_ftp"
/*!<  EMAIL抓图的临时目录 */
#define CAPTURE_MEM_DIR_FOR_EMAIL               "/tmp/picture/pic_for_email"
/*!<  PUSH抓图的临时目录 */
#define CAPTURE_MEM_DIR_FOR_PISH                "/tmp/picture/pic_for_push"
/*!<  MEM抓图的临时目录 */
#define CAPTURE_MEM_DIR_FOR_MEM                 "/tmp/picture/pic_for_mem"
/*不可上传的主目录*/
#define P2PUDAnyApp                             "/mnt/app" // app
#define P2PUDAnyWeb                             "/mnt/web" // web
#define P2PUDAnyLib                             "/usr/lib" //  /usr/lib
/*!< customcfg.ini信息 */
#define CUSTOM_CONFIG_FILE			            "/mnt/mtd/config/customcfg.ini"
/*!< resolv.conf信息 */
#define RESOLV_CONF_FILE_PATH                   "/mnt/mtd/etc/resolv.conf"

#define RESOLV_ETH0_CONF 						"/mnt/mtd/etc/resolv_eth0.conf"

/*!< resolv.conf信息 */
#define RESOLV_CONF_FILE_PATH_REAL              "/mnt/mtd/etc/resolv.conf"
/*!< resolv_wlan.conf信息 */
#define RESOLV_WLAN_CONF_FILE_PATH              "/mnt/mtd/etc/resolv_wlan.conf"
/*!< resolv_wlan.conf信息 */
#define RESOLV_ETH0_CONF_FILE_PATH              "/mnt/mtd/etc/resolv_eth0.conf"

/*!< usr.db信息 */
#define USR_DB_FILE_PATH                        "/mnt/mtd/config/user.db"
/*!< usr.db信息 */
#define USR_DB_FILE_PATH_TXT                    "/mnt/mtd/config/user.txt"
/*!< log.db信息 */
#define LOG_DB_FILE_PATH                        "/mnt/mtd/config/log.db"
#define CUSTOM_MODE_FILE			            "/mnt/mtd/config/custommode.ini"
#define CUSTOM_MODE_WINDOS			            "/tmp/.custommode_windos.ini"
#define CUSTOM_MODE_FLAG			            "/mnt/mtd/config/.custommode.flag"
#define CUSTOM_MODE_RECOVER_DEFAULT	            "/mnt/mtd/config/.custommodedef.flag"
#define CUSTOM_READONLY_CONFIG_FILE             "/mnt/mtd/config/customrocfg.flag"
#define CUSTOM_CONFIGMODE_FILE		            "/mnt/mtd/config/customcfg.ini"
#define CUSTOM_ABILITY_FILE			            "/mnt/mtd/config/customability.ini"
#define CUSTOM_BACKUP_READONLY_CONFIG_FILE 	    "/mnt/mtd/config/customrocfg.flag-bak"
#define CUSTOM_BACKUP_CONFIGMODE_FILE		    "/mnt/mtd/config/customcfg.ini-bak"
#define CUSTOM_BACKUP_ABILITY_FILE			    "/mnt/mtd/config/customability.ini-bak"
#define TEST_WORK_MODE_FLAG                     "/tmp/.TestWorkMode"
/*!< 云升级相关文件 > */
#define UPGRADE_LIST_FILE                       "/tmp/upgradeList.xml"
#define UPGRADE_PASSPORT_FILE                   "/mnt/mtd/config/.upgradePassport"
#define UPGRADE_PASSPORT_TMP_FILE               "/tmp/.upgradePassport"
#define UPDATEFILENAME_SUCC_CLOUD_UPGRADE       "/tmp/app/.update_success_cloud_upgrade"
#define UPDATEFILEPATH              "/"
#define UPDATEFILEFILE              "/tmp/app/zw_device_update.bin"
#define UPDATEFILENAME              "/tmp/app/zw_device_update.bin"
#define IMPORTFILENAME              "/tmp/app/zw_device_remote_config.xml"
#define ISPTONECURVEFILENAME        "/mnt/mtd/config/zw_isp_tone_curve.dat"
#define DESTROY_UPLOAD_OVER_FLAG    "/tmp/app/.destroy_over"
#define UPDATEFILENAME_SUCC		    "/tmp/app/.update_success"
#define PTZ_PATH                    "/tmp/app/download"
#define CUSTOM_SHILIAN              "SHILIAN"
#define STARTFLAG_NAME              "/tmp/app/startflag"
#define APPLYDIDSERVERINFOTMP       "/mnt/mtd/config/p2pSer.tmp"
#define APPLYDIDSERVERINFOCFG       "/mnt/mtd/config/p2pSer.cfg"
#define APPLYDIDSERVERINFOCFGBAK    "/mnt/mtd/config/p2pSer.cfg-bak"
#define P2P_TYPE_TUTK               "/mnt/app/lib/libtutkmobile.so"
#define P2P_TYPE_ULK                "/mnt/app/lib/libulk_net.so"
#define P2P_TYPE_SHYUN              "/mnt/app/lib/libzwmobile.so"
/*!< 视频预览通道别名*/
#define IDVR_OSD_ALIAS		"/tmp/app/res/osd_CH%02d.bmp"
#define SECTION_OSD_ALIAS	"ChannelAlias_%d"
#define DEFNAME_OSD_ALIAS	"CH%02d"
/*!< 报警输入通道别名*/
#define IDVR_DIP_ALIAS		"/tmp/app/res/AlarmIn_%02d.bmp"
#define SECTION_DIP_ALIAS	"AlarmInAlias_%d"
#define DEFNAME_DIP_ALIAS	"AI%02d"
/*!< 报警输出通道别名*/
#define IDVR_DOP_ALIAS		"/tmp/app/res/AlarmOut_%02d.bmp"
#define SECTION_DOP_ALIAS	"AlarmOutAlias_%d"
#define DEFNAME_DOP_ALIAS	"AO%02d"
/*!< 设备别名*/
#define IDVR_DEV_ALIAS		"/tmp/app/res/dev_alias.bmp"
#define SECTION_DEV_ALIAS	"iDVR_Alias"
#define DEFNAME_DEV_ALIAS	"iDVR"
/*!< UPNP配置文件的路径 */
#define UPNP_CFG_PATH       "/mnt/mtd/config/upnp"
#define CGI_BIN             "/tmp/app/bin/ipc"
/*!< 用户定制CGI控制相关文件 > */
#define CUSTOM_CONFIG       "/mnt/mtd/config/config.ini"
#define MOTO_FILE_CFG       "/mnt/mtd/config/eptzPreset.cfg"

#define MODULE_4G_BIND_FLAG		"/mnt/mtd/config/.Module4gBindFlag"
#define RESOLV_CONF				"/etc/resolv.conf"

/*!< 配置升级文件 > */
#define UPDATE_CFG_FILE			"/mnt/mtd/config/update.cfg"
#define UPDATEFLAG_CFG_FILE		"/mnt/mtd/config/update.flag"
#define UPDATE_CREATE_STATUS    "/tmp/.update_create"

#if defined (__SUPPORT_PRODUCT_TEST_CUSTOM_SPEECH_FILES__)
/*!< 设备当前语音文件 > */
#define DEV_ALL_SPEECH_REAL_PATH            "/mnt/speech"
#define CUSTOM_ALL_SPEECH_TAR_MD5_FILE      "/mnt/speech/AllSpeech.tar.md5"
#define CUSTOM_ALL_SPEECH_TAR_INFO_FILE     "/mnt/speech/CustomSpeech.ini"

/*!< 产测定制设备语音文件 > */
#define CUSTOM_ALL_SPEECH_TAR_TMP_PATH      "/tmp/AllSpeech.base64"
#define CUSTOM_ALL_SPEECH_TAR_PATH          "/tmp/AllSpeech.tar"
#define CUSTOM_ALL_SPEECH_TAR_DECODE_PATH   "/tmp/speech"
#endif

#define AUTH_USR_DATA_BACK_FILE                     "/mnt/mtd/config/authusrdata.back"

#if defined (__IS_SUPPORT_REMINDER_SCHEDULE__)
#define SPEECH_REMIND_SCHEDULE_CUSTOM_FILE1			"/mnt/mtd/config/remind_custom1.amr"
#define SPEECH_REMIND_SCHEDULE_CUSTOM_FILE2			"/mnt/mtd/config/remind_custom2.amr"
#define SPEECH_REMIND_SCHEDULE_CUSTOM_FILE3			"/mnt/mtd/config/remind_custom3.amr"
#endif

#if defined(__IS_SUPPORT_LCD_MANAGER__)
/* 默认全屏背景图路径（jpeg格式，宽高必须等于屏幕大小） */
#define LCD_DEF_BACKGROUND_IMAGE_PATH   ("/mnt/app/custom/img/background.jpg")
#define LCD_USR_BACKGROUND_IMAGE_PATH   ("/mnt/mtd/config/Usrbackground.jpg")
#endif

//本地hosts
#define HOSTS_CONFIG_ETC_FILE                       "/mnt/mtd/etc/hosts"
//YLTB号段专用hosts
#define YLTB_HOSTS_CONFIG_ETC_FILE                  "/mnt/mtd/etc/yltb_hosts"
//Normal hosts
#define NORMAL_HOSTS_CONFIG_ETC_FILE                "/mnt/mtd/etc/normal_hosts"


#endif
