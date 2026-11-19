#include <iostream>
#include <sys/statvfs.h>
#include <fstream>
#include <string.h>
#include <sys/time.h>
#include <stdarg.h>
#include <unistd.h>
#include "mxml.h"
#include "mysystem.h"
#include "global_macro.h"		//全工程宏定义
#include "newhie/HieServer.h"	//网络SDK
#include "MPU_AVL_API.h"
#include "MPU_PAR_API.h"
#include "MPU/MPU_CIU_global.h"
#include "common_pub.h"
#include "CConfig.h"
#include "zwsdk.h"
#include "mauth.h"
#include "../PAR/CPubFunc.h"
#include "Authority.h"

static char s_DeviceUUID[64] = {0};
static const char *g_UUIDInValid= "FFFFFFFFFFFFFFFF";

static pthread_mutex_t m_MjpegLock = PTHREAD_MUTEX_INITIALIZER;

struct QRRunInfo {
  int qrscan_enable;
  int start_tm;
};

#ifdef __MODULE_4G_SUPPORT__
#define QR_SCAN_CLOSE_TIME (8*60*1000)
#else
#define QR_SCAN_CLOSE_TIME (5*60*1000)
#endif
static struct QRRunInfo g_qr_run_info = {0};
static pthread_mutex_t g_mtx_qr_run_info = PTHREAD_MUTEX_INITIALIZER;

int SetQRScanStatus(int en)
{
    struct timespec ts;
    pthread_mutex_lock(&g_mtx_qr_run_info);
    ZWSys_EnableQrScan(en);
    if (0 != en)
    {
        clock_gettime(CLOCK_MONOTONIC, &ts);
        g_qr_run_info.start_tm = MS(ts);
    }
    g_qr_run_info.qrscan_enable = en;
    pthread_mutex_unlock(&g_mtx_qr_run_info);
    return 0;
}

int GetQRScanStatus()
{
    /* 人形追踪时会频繁获取，此处不加锁 */
    return g_qr_run_info.qrscan_enable;
}

int GetQRScanCloseFlags()
{
    int ret = 0;
    struct timespec ts;
    int cur_tm = 0;

    pthread_mutex_lock(&g_mtx_qr_run_info);
    if (0 != g_qr_run_info.qrscan_enable)
    {
        clock_gettime(CLOCK_MONOTONIC, &ts);
        cur_tm = MS(ts);
        if (g_qr_run_info.start_tm > 0)
        {
            if ((cur_tm - g_qr_run_info.start_tm) > QR_SCAN_CLOSE_TIME)
            {
                ret = 1;
            }
        }
    }
    pthread_mutex_unlock(&g_mtx_qr_run_info);
    return ret;
}

int GetCapFromDeviceCapXML_Bool(const char *pos, bool df)
{
    int ret = df ? 1:0;
    char *p = NULL;
    const char *value = NULL;
    char tmp[256] = {0};
    char *tmpIn = NULL;
    FILE *pFile = NULL;
    mxml_node_t *tree = NULL;
    mxml_node_t *node = NULL;

    if (pos)
    {
        pFile = fopen(CUSTOM_DEVICE_CAP_FILE, "r");
        if (pFile)
        {
            tree = mxmlLoadFile(NULL, pFile, MXML_TEXT_CALLBACK);
            if (tree)
            {
                node = tree;
                snprintf(tmp, sizeof(tmp), "%s", pos);
                p = strtok_r(tmp, ":", &tmpIn);
                while (p)
                {
                    node = mxmlFindElement(node, tree, p, NULL, NULL, MXML_DESCEND);
                    if (NULL == node)
                    {
                        OPEN_LOG_E("(ERROR) XML Find Node failed\n");
                        break;
                    }
                    p = strtok_r(NULL, ":", &tmpIn);
                    if (NULL == p)
                    {
                        value = mxmlGetText(node, NULL);
                        if (value)
                        {
                            //printf("Value[%s]\n", value);
                            if (strcasecmp(value, "true") == 0)
                            {
                                ret = 1;
                            }
                            else
                            {
                                ret = 0;
                            }
                        }
                    }
                }
                mxmlDelete(tree);
            }
            else
            {
                OPEN_LOG_E("(ERROR) XML Load File failed\n");
                ret = -1;
            }
            fclose(pFile);
        }
		else
		{
			OPEN_LOG_E("(ERROR) open [%s] failed\n", CUSTOM_DEVICE_CAP_FILE);
			ret = -1;
		}
    }

    return ret;
}
int  GetCapFromDeviceCapXML_Int(const char *pos)
{
	int ret = 0;
    char *p = NULL;
    const char *value = NULL;
    char tmp[256] = {0};
    char *tmpIn = NULL;
    FILE *pFile = NULL;
    mxml_node_t *tree = NULL;
    mxml_node_t *node = NULL;

    if (pos)
    {
        pFile = fopen(CUSTOM_DEVICE_CAP_FILE, "r");
        if (pFile)
        {
            tree = mxmlLoadFile(NULL, pFile, MXML_TEXT_CALLBACK);
            if (tree)
            {
                node = tree;
                snprintf(tmp, sizeof(tmp), "%s", pos);
                p = strtok_r(tmp, ":", &tmpIn);
                while (p)
                {
                    node = mxmlFindElement(node, tree, p, NULL, NULL, MXML_DESCEND);
                    if (NULL == node)
                    {
                        break;
                    }
                    p = strtok_r(NULL, ":", &tmpIn);
                    if (NULL == p)
                    {
                        value = mxmlGetText(node, NULL);
                        if (value)
                        {
                            ret = atoi(value);
                        }
						else
						{
							ret = -1;
						}
                    }
                }
                mxmlDelete(tree);
            }
            fclose(pFile);
        }
		else
		{
			printf("No DeviceCap file,open failed\n");
			ret = -1;
		}
    }

    return ret;
}

bool GetCapSupportFromDeviceCapXML_Bool(const char *pos, bool df)
{
    bool ret = df;
    char *p = NULL;
    const char *value = NULL;
    char tmp[256] = {0};
    char *tmpIn = NULL;
    FILE *pFile = NULL;
    mxml_node_t *tree = NULL;
    mxml_node_t *node = NULL;

    if (pos)
    {
        pFile = fopen(CUSTOM_DEVICE_CAP_FILE, "r");
        if (pFile)
        {
            tree = mxmlLoadFile(NULL, pFile, MXML_TEXT_CALLBACK);
            if (tree)
            {
                node = tree;
                snprintf(tmp, sizeof(tmp), "%s", pos);
                p = strtok_r(tmp, ":", &tmpIn);
                while (p)
                {
                    node = mxmlFindElement(node, tree, p, NULL, NULL, MXML_DESCEND);
                    if (NULL == node)
                    {
                        break;
                    }
                    p = strtok_r(NULL, ":", &tmpIn);
                    if (NULL == p)
                    {
                        mxml_node_t* node_support = mxmlFindElement (node, tree, "Support", NULL, NULL, MXML_DESCEND);
                        if (NULL == node_support)
                        {
                            printf ("can not find element Support\n");
                        }
                        else
                        {
                            value = mxmlGetText(node_support, NULL);
                            if (value)
                            {
                                //printf("Value[%s]\n", value);
                                if (strcasecmp(value, "true") == 0)
                                {
                                    ret = true;
                                }
                                else
                                {
                                    ret = false;
                                }
                            }
                        }
                    }
                }
                mxmlDelete(tree);
            }
            fclose(pFile);
        }
    }

    return ret;
}

int GetValueFromBootEnv(const char *str, char *val, unsigned int size)
{
    if (NULL == str || NULL == val)
    {
        printf("[%s %d] str[%p] val[%p]\n", __FUNCTION__, __LINE__, str, val);
        return -1;
    }

    char command[512] = {0};
    snprintf(command, sizeof(command), "fw_printenv -n %s", str);

    if (ShellCmdOut(command, val, size))
    {
        printf("[%s %d]Fail to get %s from boot env\n", __FUNCTION__, __LINE__, str);
        return -1;
    }

    /* 去掉结尾的回车换行 */
    if (strlen(val))
    {
        val[strlen(val)-1] = '\0';
    }
    else
    {
        return -1;
    }

    return 0;
}

int SetValueToBootEnv(const char *str, const char *val, unsigned int size)
{
    if (NULL == str || NULL == val)
    {
        printf("[%s %d] str[%p] val[%p]\n", __FUNCTION__, __LINE__, str, val);
        return -1;
    }

    char command[512] = {0};
    snprintf(command, sizeof(command), "fw_setenv %s %s", str, val);
    mysystem(command);

    return 0;
}

void DeviceCfgReset(const char *func)
{
    char command[128] = {0};
	char subkey_conf[64] = {0};

    /* 需要恢复的文件: Edvr.cfg、Edvr.cfg-bak、boa.conf、boa.conf-bak */
	snprintf(command,sizeof(command), "cp -v %s %s", IDVR_DEFAULT_CONFIG_FILE_PATH, IDVR_CURRENT_CONFIG_FILE_PATH);
	mysystem(command); // Edvr.cfg

	memset(command, 0, sizeof(command));
	snprintf(command,sizeof(command), "cp -v %s %s", IDVR_DEFAULT_CONFIG_FILE_PATH, IDVR_BACKUP_CONFIG_FILE_PATH);
	mysystem(command); // Edvr.cfg-bak

	memset(command, 0, sizeof(command));
	snprintf(command,sizeof(command), "cp -v %s %s", BOA_APP_BAK_FILE_PATH, BOA_CONF_FILE_PATH);
	mysystem(command); // boa.conf

	memset(command, 0, sizeof(command));
	snprintf(command,sizeof(command), "cp -v %s %s", BOA_APP_BAK_FILE_PATH, BOA_CONF_BAK_FILE_PATH);
	mysystem(command); // boa.conf-bak

    /* 需要删除的文件: tutkuid.conf、resolv.conf、resolv_wlan.conf、usr.db、log.db、userlist.conf */
#if 0//恢复出厂时不去删除DID的配置文件
    memset(command, 0, sizeof(command));
	snprintf(command,sizeof(command), "rm -v %s", TUTK_UID_FILE_PATH);
	mysystem(command); // tutkuid.conf
#endif

	memset(command, 0, sizeof(command));
	snprintf(command,sizeof(command), "cp -v %s %s", RESOLV_CONF_BAK_FILE_PATH, RESOLV_CONF_FILE_PATH_REAL);
	mysystem(command); // resolv.conf(需要清空)

	memset(command, 0, sizeof(command));
	snprintf(command,sizeof(command), "cp -v %s %s", RESOLV_WLAN_CONF_BAK_FILE_PATH, RESOLV_WLAN_CONF_FILE_PATH);
	mysystem(command); // resolv_wlan.conf(需要清空)

	memset(command, 0, sizeof(command));
	snprintf(command,sizeof(command), "cp -v %s %s", RESOLV_ETH0_CONF_BAK_FILE_PATH, RESOLV_ETH0_CONF_FILE_PATH);
	mysystem(command); // resolv_eth0.conf(需要清空)

	memset(command, 0, sizeof(command));
	snprintf(command,sizeof(command), "rm -v %s", USR_DB_FILE_PATH);
	mysystem(command); // usr.db

	memset(command, 0, sizeof(command));
	snprintf(command,sizeof(command), "rm -v %s", USR_DB_FILE_PATH_TXT);
	mysystem(command); // usr.txt

	memset(command, 0, sizeof(command));
	snprintf(command,sizeof(command), "rm -v %s", LOG_DB_FILE_PATH);
	mysystem(command); // log.db

	memset(command, 0, sizeof(command));
	snprintf(command,sizeof(command), "cp -v %s %s", UserFile_BAK, UserFile);
	mysystem(command); // userlist.conf

	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), "rm -v  %s %s",CLOUD_STORAGE_MODE_DOMESTIC_FLAG,CLOUD_STORAGE_MODE_FOREIGN_FLAG);
	mysystem(command); // CloudMode0,CloudMode1

    if (0 == access(SPEECH_MOTION_ALARM_CUSTOM_FILE, 0))
	{
		memset(command, 0, sizeof(command));
		snprintf(command, sizeof(command), "rm -v %s", SPEECH_MOTION_ALARM_CUSTOM_FILE);
		mysystem(command);
	}

	mysystem("rm /mnt/mtd/config/.Ip*");
	mysystem("rm /mnt/mtd/config/.GATEWAY*");
	mysystem("rm /mnt/mtd/config/.MASK*");
	mysystem("rm /mnt/mtd/config/.DHCP*");
	mysystem("rm /mnt/mtd/config/.IPADJUST*");
	mysystem("rm /mnt/mtd/config/.OSD");
#if defined (__IS_SUPPORT_WLAN__)
    mysystem("rm /mnt/mtd/config/.GateWay*");
	mysystem("rm /mnt/mtd/config/.WlanIp* ");
	mysystem("rm /mnt/mtd/config/.WlanMASK* ");
	mysystem("rm /mnt/mtd/config/.WlanGateWay* ");
	mysystem("rm /mnt/mtd/config/.WlanDHCP* ");
	mysystem("rm /mnt/mtd/config/.wificfg.ini ");
	mysystem("rm /mnt/mtd/config/.WlanWifiMode* ");
    mysystem("rm /mnt/mtd/config/.wifiConSuc* ");
    mysystem("rm /mnt/mtd/config/.webCfgWifi* ");
#endif

	/* 删除推送的Subkey信息 */
    snprintf(subkey_conf, sizeof(subkey_conf), "%s%s", SUBKEY_PARAM_DIR, SUBKEY_PARAM_CONFIG);
	if(0 == access(subkey_conf, F_OK))
	{
	    printf("%s is exist! need to remove it for subkey!\n", subkey_conf);
		remove(subkey_conf);
	}

	mysystem(RESTART_TIMES);	
	mysystem(SAVE_ENV);
	OPEN_LOG_I("reset clean RESTART_TIMES\n");

	mysystem("sync");
	printf("--------- Restore DeviceCfgReset default factory over -%s- --------\n", func);
#ifdef __MODULE_4G_SUPPORT__
    ZWSys_SetSpeechContent(SPEECH_4G_SYS_RESET);
#else
    ZWSys_SetSpeechContent(SPEECH_SYS_RESET);
#endif

    return;
}


int GetSCodeFromFile(const char *file, char *code, int codeLen)
{
    if ((NULL == file) || (NULL == code) || (codeLen <= 0))
    {
        printf("[%s %d]Param Err! file[%p] code[%p] codeLen[%d]",
                __FUNCTION__, __LINE__, file, code, codeLen);
        return -1;
    }

    int nRet = -1;

    if (0 == access(file, F_OK))
    {
        CConfig *pConf = CConfig::NewInstance();
        int rc = pConf->LoadFileRO(file);
        if (0 == rc)
        {
            pConf->GetValue("ERP", "SCode", "", code, codeLen);
            // S码不可能为空
            if (strlen(code) > 0)
            {
                nRet = 0;
            }
        }

        CConfig::ReleaseInstance(pConf);
    }

    return nRet;
}

int SetSCodeToFile(const char *file, const char *code)
{
    if ((NULL == file) || (NULL == code) || strlen(code) > 256)
    {
        printf("[%s %d]Param Err! file[%p] code[%p] codeLen[%d]",
                __FUNCTION__, __LINE__, file, code, strlen(code));
        return -1;
    }

    int nRet = -1;
    int rc = 0;
    CConfig *pConf = CConfig::NewInstance();

    // 1.file格式检查
    if (0 == access(file, F_OK))
    {
        rc = pConf->LoadFile(file);
        if (0 != rc)
        {
            char cmd[256] = {0};
            snprintf(cmd, sizeof(cmd), "rm -rf %s", file);
            mysystem(cmd);
        }
    }

    // 2.文件不存在生成ini格式文件
    if (0 != access(file, F_OK))
    {
        char buf[256] = {0};
        snprintf(buf, sizeof(buf), "[system]\ndvr=idvr9000\n\n[ERP]\nSCode=\n\n[end]\ndvr=idvr9000");
        FILE *fp = fopen(file, "w");
        if (fp)
        {
            fwrite(buf, sizeof(char), strlen(buf), fp);
            fflush(fp);
            fclose(fp);
            nRet = 0;
        }
    }
	else
	{
		nRet = 0;
	}

    if (0 == nRet)
    {
        nRet = -1;
        rc = pConf->LoadFile(file);
        if (0 == rc)
        {
            pConf->SetValue("ERP", "SCode", code);
            pConf->SaveFileReal(1);
            nRet = 0;
        }
    }

    CConfig::ReleaseInstance(pConf);

    return nRet;
}

int checkValidWifiConnection(WLAN_CONNECTION *con)
{
	if(0 == strlen(con->szSSID))
	{
        printf("wifi SSID is NUll!\n");
		return -1;
	}

	WLAN_AUTHMODE nMode = (WLAN_AUTHMODE)con->nAuthMode;
	WLAN_ENCRYPTYPE nEncrypType = (WLAN_ENCRYPTYPE)con->nEncrypType;
	LPWIFI_KEY_CFG pKeyCfg = &con->struKeyCfg;

	if ((AM_OPEN == nMode) || (AM_SHARED == nMode))
	{
		if (ET_WEP == nEncrypType)
		{
			WIFI_KEY *pKey = &pKeyCfg->Key1+pKeyCfg->nDefKeyID-1;
			if ((0 == pKeyCfg->bValid) || (0 >= pKeyCfg->nDefKeyID) || (4 < pKeyCfg->nDefKeyID))
			{
				printf("KeyCfg Is Valid, Mode=%d, EncrypType=%d\n", nMode, nEncrypType);
				return -1;
			}
			else if (!pKey->bValid)
			{
				printf("KeyCfg Is Valid, DefKeyID=%d, Mode=%d, EncrypType=%d\n", pKeyCfg->nDefKeyID, nMode, nEncrypType);
				return -1;
			}
			else
			{
				int nKeyLen = strlen(pKey->szKey);
				if ((5 != nKeyLen) && (10 != nKeyLen) && (13 != nKeyLen) && (26 != nKeyLen))
				{
					printf("Key %d is Valid, Value=%s\n", pKeyCfg->nDefKeyID, pKey->szKey);
					return -1;
				}
			}
		}
	}
	else if ((AM_WPAPSK == nMode) || (AM_WPA2PSK == nMode))
	{
		if ((ET_TKIP == nEncrypType) || (ET_AES == nEncrypType))
		{
			if ((0 == pKeyCfg->bValid) || (0 == pKeyCfg->WPAKey.bValid))
			{
				printf("WPAKeyCfg Is invalid, Mode=%d, EncrypType=%d\n", nMode, nEncrypType);
				return -1;
			}
			else
			{
				int nKeyLen = strlen(pKeyCfg->WPAKey.szKey);
				if ((nKeyLen < 8) || (nKeyLen > 63))
				{
					printf("WPAKey is Valid, Value=%s\n", pKeyCfg->WPAKey.szKey);
					return -1;
				}
			}
		}
		else
		{
			printf("EncrypType Is Valid, Mode=%d, EncrypType=%d\n", nMode, nEncrypType);
			return -1;
		}
	}
	else if (AM_NONE == nMode)
	{
		;
	}
	else
	{
		printf("Mode Is Valid, Mode=%d\n", nMode);
		return -1;
	}

    return 0;
}

/*  固定长度 24
byte 0-1   厂商        RK
byte 2-5   cpu型号     1109
byte 6-7   cpu版本     00
byte 8-23  chipID      xxxx唯一码
*/
int GetDeviceUUID(char* pUuid, int pLen)
{
	int nRet = 0;
	int device_chipid_len = 0;
    unsigned int i = 0;
    int chip_head_len = 0;
    char chip_head[16] = {0};
    int chip_product_id_len = 0;
    char chip_product_id[32] = {0};

    if ((NULL == pUuid)|| (pLen < 24))
    {
        printf("[%s %d] Param Err! pUuid[%p] pLen[%d]\n", __FUNCTION__, __LINE__, pUuid, pLen);
        return -1;
    }

    if (0 == strlen(s_DeviceUUID))
    {
        //cpu型号
        if (0 == access("/mnt/app/custom/cpu_type", F_OK|R_OK))
        {
            memset(chip_head, 0, sizeof(chip_head));
            ShellCmdOut("cat /mnt/app/custom/cpu_type", chip_head, 9);
            chip_head_len = strlen(chip_head);
            //chipID
            //ShellCmdOut("cat /sys/devices/platform/fh_spi.0/spi_master/spi0/spi0.0/factory_id | awk -F'-' '{print $1$2$3$4$5$6$7$8}'", chip_product_id, 16);
			if (ZWSys_GetUniqueID(chip_product_id, UNIQUEID_BUF_LEN))
			{
				// 万一出现失败，则为全F
				memcpy(chip_product_id, g_UUIDInValid, 16);
			}

            chip_product_id_len = strlen(chip_product_id);
            if (((chip_head_len != 0) && (chip_head_len <= 8)) && ((chip_product_id_len != 0) && (chip_product_id_len <= 16)))
            {
                memset(s_DeviceUUID, '0', 24);
                memcpy(s_DeviceUUID, chip_head, chip_head_len);
                memcpy(&s_DeviceUUID[8], chip_product_id, chip_product_id_len);
                //小写->大写
                for (i = 0; i < strlen(s_DeviceUUID); i++)
                {
                    if (s_DeviceUUID[i] >= 'a' && s_DeviceUUID[i] <= 'z')
                        s_DeviceUUID[i] -= 32;
                }
            }
        }
    }

    device_chipid_len = strlen(s_DeviceUUID);
    if (24 == device_chipid_len)
    {
        memcpy(pUuid, s_DeviceUUID, sizeof(s_DeviceUUID));
        nRet = 0;
    }
    else
    {
        nRet = 0; //直接返回成功，无CHIPID
    }

	return nRet;
}

/*@breif GetUIDInfoFromLicensePart 从devinfo分区里面获取UID信息
 *@param  pTutkUID 得到的UID缓冲区
 *@param  nTutkUIDLen 缓冲区大小
 *@param  pPushId 得到的push ID缓冲区
 *@param  nPushIdLen 缓冲区大小
 *@return 成功返回0
 *@      失败返回-1
 */
int GetUIDInfoFromLicensePart(char *pTutkUID, int nTutkUIDLen, char *pPushId, int nPushIdLen)
{
    stuShmAuthInfo nAuthInfo = {0};
    char TutkUIDAndPush[MAX_DID_LENGTH] = {0};
    char *pPushIdStartPos = NULL;
    int nRet = 0;

    if((NULL == pTutkUID) || (NULL == pPushId) || (nTutkUIDLen < MAX_DID_LENGTH) || (nPushIdLen < MAX_PUSH_ID_LENGTH))
    {
        printf("[%s - %d]NULL POINTER EXCEPTION! or size too short!\n", __FUNCTION__, __LINE__);
        return -1;
    }

    memset(pTutkUID, 0, nTutkUIDLen);
    memset(pPushId, 0, nPushIdLen);

    Authorize_GetAuthDevInfo(&nAuthInfo);

	printf("[%s:%d] szDid: %s\n", __FUNCTION__, __LINE__, nAuthInfo.struCloudInfo.szDid);

	if (0 != strlen((const char*)nAuthInfo.struCloudInfo.szDid))
	{
		memcpy(TutkUIDAndPush, nAuthInfo.struCloudInfo.szDid, strlen((const char*)nAuthInfo.struCloudInfo.szDid));
		pPushIdStartPos = strchr(TutkUIDAndPush, MAX_PUSH_ID_SFD);
        if (NULL != pPushIdStartPos)
        {
            memcpy(pTutkUID, TutkUIDAndPush, (int)(pPushIdStartPos-TutkUIDAndPush));
            strncpy(pPushId, pPushIdStartPos+1, MAX_PUSH_ID_LENGTH - 1);
        }
        else
        {
            //不支持推送号
            strncpy(pTutkUID, TutkUIDAndPush, sizeof(TutkUIDAndPush) - 1);
        }
    }
    else
    {
		nRet = -1;
    }

    return nRet;
}

int GetRecordBufSizeByBoardID(int *szRing, int *szFrame)
{
    if ((NULL != szRing) && (NULL != szFrame))
    {
        // 获取当前设备版型
        int nBoardID = 0;
        ZWSys_GetBoardInfo(&nBoardID);
        switch (nBoardID)
        {
        case 0x216D:
            {
                // 200w
                *szRing = 1*1024*1024;
                *szFrame = 400*1024;
            }
            break;
		case 0x2175:
			{
                // 200w
                *szRing = 1*1024*1024;
                *szFrame = 400*1024;
            }
			break;
        default:
            {
                *szRing = 1*1024*1024;
                *szFrame = 512*1024;
            }
            break;
        }
        printf("[%s %d] Ring: %d Frame: %d\n", __FUNCTION__, __LINE__, *szRing, *szFrame);
    }
    return 0;
}

int str2double(double *val, const char *str)
{
  if(str) {
    char *endptr;
    double num = strtod(str, &endptr);
    if((endptr != str) && (endptr == str + strlen(str))) {
      *val = num;
      return 0;  /* Ok */
    }
  }
  return -1; /* badness */
}

static int g_SystemStartOverFlag = 0;
static int g_SetDevInitOverFlag = 0;
/**
********************************************************************************
 * @函数功能: 获取系统启动完成且图像已初始化完的状态。主要用于开启录像判断
 * @作者    : HL.LIU
 * @返回值  : 0 - 未完全启动 1 - 启动完成
 * @其他    :
********************************************************************************
*/
int GetSystemStartOverFlag()
{
    return g_SystemStartOverFlag;
}

/**
********************************************************************************
 * @函数功能: 设置系统启动完成且图像已初始化完（IRCUT已完成自检2次之后）的状态
 * @作者    : HL.LIU
 * @param[in] flag:0 - 未完全启动 1 - 启动完成
 * @返回值  :
 * @其他    :
********************************************************************************
*/
int SetSystemStartOverFlag(int flag)
{
    g_SystemStartOverFlag = flag;
    return 0;
}

/**
********************************************************************************
 * @函数功能: 获取设备初始化步骤是否已完成（main函数里的步骤已运行完）
 * @作者    : HL.LIU
 * @返回值  : 0 - 未完全启动 1 - 启动完成
 * @其他    :
********************************************************************************
*/
int GetDevInitOverFlag()
{
    return g_SetDevInitOverFlag;
}

/**
********************************************************************************
 * @函数功能: 设置设备初始化完成标记（main函数里的步骤已运行完）
 * @作者    : HL.LIU
 * @param[in] flag:0 - 未完全启动 1 - 启动完成
 * @返回值  :
 * @其他    :
********************************************************************************
*/
int SetDevInitOverFlag(int flag)
{
    g_SetDevInitOverFlag = flag;
    return 0;
}

/**
 * @brief 获取mjpeg buffer
 * @param  [IN]  mode	输入的模式， 0:外部输入buffer  1:内部动态malloc
 * @param  [IN]  nChn	通道号
 * @param  [OUT] pBuffer	输出的Buffer
 * @param  [OUT] pLength	输出的BUffer长度
 * @param  [IN]  maxLength	buffer的最大长度
 * @param  [OUT] pTimestampMs 输出的时间戳(毫秒)
 * @return int 0:正常，其他值为异常
 */
int GetMjpegBuffer(int mode, int nChn, char **pBuffer, int *pLength, int maxLength, long long *pTimestampMs)
{
#ifdef __USE_CIRCLEQUEUE__
	int nRet = 0;
	PullRealStreamInterface MjpegStreamInterface = {0};
	DWORD ConnectionId = 0;
	RealStreamMedia realStreamMedia;

    if (nChn >= MAX_CH_NUM)
    {
        printf("[%s][%d] GetMjpegBuffer fail! Chn=[%d]\n", __FUNCTION__, __LINE__, nChn);
        return -1;
    }

	XTRACE_MUTEX_LOCK(&m_MjpegLock);

	memset(&realStreamMedia, 0, sizeof(RealStreamMedia));
	if(CIU_GetMjpegStreamCircleQueueInterface(&MjpegStreamInterface) < 0)
	{
		return -1;
	}
	nRet = MjpegStreamInterface.ifRealStreamConnectionAdd(eProtocolBegin, 0, nChn, eMediaCodefourthStream, NULL, &ConnectionId);
	if (!nRet)
	{
		nRet = MjpegStreamInterface.ifRealStreamConnectionGetData(ConnectionId, &realStreamMedia, NULL);
		if (!nRet)
		{
			if (realStreamMedia.cBuffer.dwBufLen > 28 && realStreamMedia.cBuffer.dwBufLen - 28 <= (unsigned int)maxLength)
			{
				if (1 == mode)
				{
					*pBuffer = (char *)malloc(realStreamMedia.cBuffer.dwBufLen - 28);
					if (NULL == *pBuffer)
					{
						printf("[%s][%d] buffer malloc size (%lu) failed!\n", __FUNCTION__, __LINE__, realStreamMedia.cBuffer.dwBufLen - 28);
						goto ReleaseData;
					}
				}

				if (*pBuffer)
				{
					memcpy(*pBuffer, realStreamMedia.cBuffer.pBuffer + 28, realStreamMedia.cBuffer.dwBufLen - 28);
					*pLength = realStreamMedia.cBuffer.dwBufLen - 28;
				}
				else
				{
					printf("[%s][%d] buffer is null\n", __FUNCTION__, __LINE__);
					goto ReleaseData;
				}
			}
			else
			{
				printf("[%s][%d]get mjpeg frame length=%lu error! max length is %d!\n", __FUNCTION__, __LINE__, realStreamMedia.cBuffer.dwBufLen, maxLength);
				goto ReleaseData;
			}

			if (pTimestampMs)
			{
				// 时间戳赋值
				struct tm datetime;
				datetime.tm_year = realStreamMedia.cTime.wYear - 1900;
				datetime.tm_mon = realStreamMedia.cTime.wMonth - 1;
				datetime.tm_mday = realStreamMedia.cTime.wDay;
				datetime.tm_hour = realStreamMedia.cTime.wHour;
				datetime.tm_min = realStreamMedia.cTime.wMinute;
				datetime.tm_sec = realStreamMedia.cTime.wSecond;

				// printf("%d-%d-%d %d:%d:%d\n", datetime.tm_year, datetime.tm_mon, datetime.tm_mday, datetime.tm_hour, datetime.tm_min, datetime.tm_sec);
				// 将时间转换为时间戳(毫秒)
				*pTimestampMs = (long long)((long long)mktime(&datetime) * 1000LL + realStreamMedia.cTime.wMillisecond);
				// printf("timestamps=%lld\n", (long long)*pTimestampMs);
			}

			nRet = MjpegStreamInterface.ifRealStreamConnectionReleaseData(ConnectionId, &realStreamMedia, NULL);
			if(nRet)
			{
				printf("[%s][%d]ifRealStreamConnectionReleaseData fail!\n", __FUNCTION__, __LINE__);
				goto ConnectionDelete;
			}
		}
		else
		{
			printf("[%s][%d]ifRealStreamConnectionGetData fail!\n", __FUNCTION__, __LINE__);
			goto ConnectionDelete;
		}

		nRet = MjpegStreamInterface.ifRealStreamConnectionDelete(ConnectionId, NULL);
		if (nRet)
		{
			printf("[%s][%d]ifRealStreamConnectionDelete fail!\n", __FUNCTION__, __LINE__);
			XTRACE_MUTEX_UNLOCK(&m_MjpegLock);
			return -1;
		}
	}
	else
	{
		printf("[%s][%d]ifRealStreamConnectionAdd fail!\n", __FUNCTION__, __LINE__);
		XTRACE_MUTEX_UNLOCK(&m_MjpegLock);
		return -2;
	}
	XTRACE_MUTEX_UNLOCK(&m_MjpegLock);

	return 0;

ReleaseData:
	nRet = MjpegStreamInterface.ifRealStreamConnectionReleaseData(ConnectionId, &realStreamMedia, NULL);
	if (nRet)
	{
		printf("[%s][%d]ifRealStreamConnectionReleaseData fail!\n", __FUNCTION__, __LINE__);
	}

ConnectionDelete:
	nRet = MjpegStreamInterface.ifRealStreamConnectionDelete(ConnectionId, NULL);
	if (nRet)
	{
		printf("[%s][%d]ifRealStreamConnectionDelete fail!\n", __FUNCTION__, __LINE__);
	}
	XTRACE_MUTEX_UNLOCK(&m_MjpegLock);

	return -3;
#endif

	return 0;
}


/**
 * @brief 获取mjpeg buffer
 * @param  [IN]  mode	输入的模式， 0:外部输入buffer  1:内部动态malloc
 * @param  [IN]  nChn	通道号
 * @param  [OUT] pBuffer	输出的Buffer
 * @param  [OUT] pLength	输出的BUffer长度
 * @param  [IN]  maxLength	buffer的最大长度
 * @param  [OUT] pTimestampMs 输出的时间戳(毫秒)
 * @return int 0:正常，其他值为异常
 */
int GetMjpegBuffer_V2(int mode, int nChn, char **pBuffer, int *pLength, int maxLength, long long *pTimestampMs)
{
#ifdef __USE_CIRCLEQUEUE__
	int nRet = 0;
	PullRealStreamInterface MjpegStreamInterface = {0};
	static DWORD ConnectionId[SYSTEM_SUPPORT_CHANNEL] = {0};
	RealStreamMedia realStreamMedia;

    if (nChn >= MAX_CH_NUM)
    {
        printf("[%s][%d] GetMjpegBuffer fail! Chn=[%d]\n", __FUNCTION__, __LINE__, nChn);
        return -1;
    }

	XTRACE_MUTEX_LOCK(&m_MjpegLock);

	memset(&realStreamMedia, 0, sizeof(RealStreamMedia));
	if(CIU_GetMjpegStreamCircleQueueInterface(&MjpegStreamInterface) < 0)
	{
		return -1;
	}
	if(ConnectionId[nChn] == 0)
	nRet = MjpegStreamInterface.ifRealStreamConnectionAdd(eProtocolBegin, 0, nChn, eMediaCodefourthStream, NULL, &ConnectionId[nChn]);

	if(nRet != 0)
	{
		printf("[%s][%d]ifRealStreamConnectionAdd fail!\n", __FUNCTION__, __LINE__);
		XTRACE_MUTEX_UNLOCK(&m_MjpegLock);
		return -2;
	}
	
	nRet = MjpegStreamInterface.ifRealStreamConnectionGetData(ConnectionId[nChn], &realStreamMedia, NULL);
	if (!nRet)
	{
		if (realStreamMedia.cBuffer.dwBufLen > 28 && realStreamMedia.cBuffer.dwBufLen - 28 <= (unsigned int)maxLength)
		{
			if (1 == mode)
			{
				*pBuffer = (char *)malloc(realStreamMedia.cBuffer.dwBufLen - 28);
				if (NULL == *pBuffer)
				{
					printf("[%s][%d] buffer malloc size (%lu) failed!\n", __FUNCTION__, __LINE__, realStreamMedia.cBuffer.dwBufLen - 28);
					goto ReleaseData;
				}
			}

			if (*pBuffer)
			{
				memcpy(*pBuffer, realStreamMedia.cBuffer.pBuffer + 28, realStreamMedia.cBuffer.dwBufLen - 28);
				*pLength = realStreamMedia.cBuffer.dwBufLen - 28;
			}
			else
			{
				printf("[%s][%d] buffer is null\n", __FUNCTION__, __LINE__);
				goto ReleaseData;
			}
		}
		else
		{
			printf("[%s][%d]get mjpeg frame length=%lu error! max length is %d!\n", __FUNCTION__, __LINE__, realStreamMedia.cBuffer.dwBufLen, maxLength);
			goto ReleaseData;
		}

		if (pTimestampMs)
		{
			// 时间戳赋值
			struct tm datetime;
			datetime.tm_year = realStreamMedia.cTime.wYear - 1900;
			datetime.tm_mon = realStreamMedia.cTime.wMonth - 1;
			datetime.tm_mday = realStreamMedia.cTime.wDay;
			datetime.tm_hour = realStreamMedia.cTime.wHour;
			datetime.tm_min = realStreamMedia.cTime.wMinute;
			datetime.tm_sec = realStreamMedia.cTime.wSecond;

			// printf("%d-%d-%d %d:%d:%d\n", datetime.tm_year, datetime.tm_mon, datetime.tm_mday, datetime.tm_hour, datetime.tm_min, datetime.tm_sec);
			// 将时间转换为时间戳(毫秒)
			*pTimestampMs = (long long)((long long)mktime(&datetime) * 1000LL + realStreamMedia.cTime.wMillisecond);
			// printf("timestamps=%lld\n", (long long)*pTimestampMs);
		}

		nRet = MjpegStreamInterface.ifRealStreamConnectionReleaseData(ConnectionId[nChn], &realStreamMedia, NULL);
	}
	else
	{
		printf("[%s][%d]ifRealStreamConnectionReleaseData fail!\n", __FUNCTION__, __LINE__);
	}
	
	XTRACE_MUTEX_UNLOCK(&m_MjpegLock);
	return 0;

ReleaseData:
	nRet = MjpegStreamInterface.ifRealStreamConnectionReleaseData(ConnectionId[nChn], &realStreamMedia, NULL);
	if (nRet)
	{
		printf("[%s][%d]ifRealStreamConnectionReleaseData fail!\n", __FUNCTION__, __LINE__);
	}
#endif

	XTRACE_MUTEX_UNLOCK(&m_MjpegLock);
	return 0;
}


/**
 * @brief 获取分区使用率
 * @param  [IN]  szPartitionName 分区名字
 * @param  [OUT] pfUsage 分区使用率
 * @return int 0:成功， -1:找不到分区
 */
int GetPartitionUsage(char *szPartitionName, float *pfUsage)
{
	struct statvfs vfs = {0};
	unsigned long long totalSpace = 0;
	unsigned long long availableSpace = 0;
	unsigned long long usedSpace = 0;

	if (statvfs(szPartitionName, &vfs) == 0)
	{
		totalSpace = vfs.f_blocks * vfs.f_frsize;
		availableSpace = vfs.f_bavail * vfs.f_frsize;
		usedSpace = totalSpace - availableSpace;

		*pfUsage = (float)usedSpace * 100 / totalSpace;
		// printf("Partition Usage: %f, usedSpace=%llu, totalSpace=%llu\n", *pnUsage, usedSpace, totalSpace);
	}
	else
	{
		fprintf(stderr, "Failed to get partition information.\n");
		return -1;
	}

	return 0;
}

/**
 * @brief  是否支持设备分享功能
 * @return int 1:支持  0:不支持
 */
int IsSupportDevShareFunc()
{
	int nRet = 0;
	int bCfgSupportShare = 0;

	CConfig *pCfg = CConfig::NewInstance();
	// 分享
	nRet = pCfg->LoadFileRO(CUSTOM_ABILITY_FILE);
	if (0 == nRet)
	{
		bCfgSupportShare = pCfg->GetValue("FunctionSupport", "IsSupportDevShared", 0);
	}
	CConfig::ReleaseInstance(pCfg);

	return bCfgSupportShare;
}

MY_DECRIBE_DECLARE_BEGIN(eRebootResult)  // 宏定义开始
    MY_DECRIBE_ADD_DESC(REBOOT_WIFI_POWER_BOOST, [Wifi status chg] Set WifiPower Boost Enable\n)  // 注意这里的字符串描述不用加引号
    MY_DECRIBE_ADD_DESC(REBOOT_WIFI_NO_IDPRODUCT, [Wifi module err] Not find idProduct or idVendor\n)
    MY_DECRIBE_ADD_DESC(REBOOT_WIFI_STATUS_ABNORMAL, [Wifi status err] All networking repair method used\n)
    MY_DECRIBE_ADD_DESC(REBOOT_P2P_STATUS_ABNORMAL, [P2P status Err] Dev has network but P2P down\n)
    MY_DECRIBE_ADD_DESC(REBOOT_MAIN_STREAM_NO_DATA, [MainStream no Data]\n)
	MY_DECRIBE_ADD_DESC(REBOOT_CLOUDSTORAGE_SWITCH, [Cloud storage switch] \n)
	MY_DECRIBE_ADD_DESC(REBOOT_GPRSDEVICE_REST, [4g Mode Rest]\n)
	MY_DECRIBE_ADD_DESC(REBOOT_GPRSDEVICE_COMERROR, [4g Mode CMD Fail]\n)
	MY_DECRIBE_ADD_DESC(REBOOT_CHECKCARD_TIMEOUT, [Check card fail] check 12 times card failed\n)
	MY_DECRIBE_ADD_DESC(REBOOT_CIU_ENC_CHANGE, [MPU Change Encoder]\n)
	MY_DECRIBE_ADD_DESC(REBOOT_UPLOAD_TIMEOUT, [Up Load Timeout]\n)
	MY_DECRIBE_ADD_DESC(REBOOT_UPLOAD, [Up Load Over]\n)
	MY_DECRIBE_ADD_DESC(REBOOT_RESUME, [The system Resume]\n)
	MY_DECRIBE_ADD_DESC(REBOOT_FORMAT_SDCARD, [Format SD Card]\n)
	MY_DECRIBE_ADD_DESC(REBOOT_REMOTE_OPERATION, [Remote Operation Reboot]\n)
MY_DECRIBE_DECLARE_END                            // 宏定义结束

static int GetFileLines(const char* pFile)
{
    if (NULL == pFile)
    {
        fprintf(stderr, "NULL POINTER EXCEPTION\n");
        return -1;
    }

    FILE *fp = NULL;
    char buf_line[512] = {0};
    int lines = 0;

    fp = fopen(pFile, "r");
    if(NULL == fp)
    {
        fprintf(stderr, "Fail to fopen %s: %s\n", pFile, strerror(errno));
        return 0;
    }

    while(!feof(fp))
    {
        fgets(buf_line, sizeof(buf_line), fp);
        lines ++;
    }

    fclose(fp);
    fp = NULL;

    return lines;
}

#define MAX_LOG_LINE   (64)
static int WriteLog(const char* pFile, const char *format, ...)
{
    if (!pFile)
    {
        printf("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    int lines = 0;
    FILE *fp = NULL;

    lines = GetFileLines(pFile);
    if (lines >= MAX_LOG_LINE)
    {
        fp = fopen(pFile, "r+");
    }
    else
    {
        fp = fopen(pFile, "a+");
    }

    if (!fp)
    {
        fprintf(stderr, "Fail to fopen %s: %s\n", pFile, strerror(errno));
        return -1;
    }

    va_list arg;
    int done = 0;

    va_start (arg, format);

    //获取当前时间段信息
    struct tm tm_log;
    time_t time_log = time(NULL);
    localtime_r(&time_log, &tm_log);

    fprintf(fp, "[%04d-%02d-%02d %02d:%02d:%02d] ",
            tm_log.tm_year + 1900,
            tm_log.tm_mon + 1,
            tm_log.tm_mday,
            tm_log.tm_hour,
            tm_log.tm_min,
            tm_log.tm_sec);

    done = vfprintf (fp, format, arg);
    va_end (arg);

    fflush(fp);
    fclose(fp);
    fp = NULL;

    return done;
}

#define DEV_REBOOT_LOG "/mnt/mtd/config/.RebootResult"
void RebootLinux(eRebootResult nResult)
{
    printf("[%s %d] Will RebootLinux! nResult[%d]\n", __FUNCTION__, __LINE__, nResult);

    WriteLog(DEV_REBOOT_LOG, MY_DESCRIBE(eRebootResult, nResult));

    mysystem("dmesg");
	mysystem("logcat &");

    /* 重启之前，需要停止录像 */
    MPU_AVI_StopRecordAndUinitMemRecord(0);
    CConfig *pConfig = CConfig::GetInstance();
    if (NULL != pConfig)
    {
        pConfig->ForbiddenSaveFile();
    }
    mysystem("sync");
	mysystem("sync");//第二次调用，确保第一次中可能遗漏的、较晚到达的元数据也被写入
    sleep(1);
    mysystem("/mnt/app/bin/savetime");
    mysystem("reboot -f");
}

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
int PubFunc_deleteNewlineInString(char *string, int stringMaxLen)
{
	if(NULL == string)
	{
		printf("ERR:string is NULL\n");
		return -1;
	}

	size_t tail = strlen(string);

	if(0 == tail)
	{
		return 0;
	}

	if(tail > (unsigned int)stringMaxLen)
	{
		printf("ERR:%s length(%d) > %d\n", string, tail, stringMaxLen);
		return -1;
	}

	// 检查最后一个字符是否为换行符
	if ((tail >= 1) && ('\n' == string[tail - 1]))
	{
		string[tail - 1] = '\0';
        if((tail >= 2) && ('\r' == string[tail - 2]))
        {
            string[tail - 2] = '\0';
        }
	}

	return 0;
}

/*@ preif 执行命令并将结果存储到result字符串中
 *@ param [in] cmd 要执行的命令
 *@ param [in] result 命令输出的结果
 *@ param [in] len  result的长度
 *@ return  0 成功
 *@       -1 出错
 */
int ShellCmdOut(const char *cmd, char *result, int len)
{
    if(!cmd || !strlen(cmd) || len<=0)
    {
        return -1;
    }
    memset(result, 0, len);

    char fileName[64] = {0};
    char command[256] = {0};
    struct timeval now = {0, 0};
    int ret = 0;
    FILE *fp = NULL;

    gettimeofday(&now, NULL);

    snprintf(fileName, sizeof(fileName), "/tmp/.ShellCmdOut%lld", now.tv_usec);
    snprintf(command, sizeof(command), "%s > %s", cmd, fileName);
    mysystem(command);

    if ((fp = fopen(fileName, "r")) == NULL)
    {
        fprintf(stderr, "Fail to fopen %s\n", fileName);
        ret = -1;
    }
    else
    {
        fread(result, sizeof(char), len-1, fp);
        if(fclose(fp))
        {
            perror("fclose");
            ret = -1;
        }
    }

    unlink(fileName);

    return ret;
}

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
int IsStringInCommand(const char *command, const char *string)
{
    int iRet = 0;
    FILE *fp = NULL;
    char buf[1024] = {0};
    char fileName[64] = {0};
    char command2[256] = {0};
    struct timeval now = {0, 0};

    gettimeofday(&now, NULL);

    snprintf(fileName, sizeof(fileName), "/tmp/.command%lld", now.tv_usec);
    snprintf(command2, sizeof(command2), "%s > %s", command, fileName);
    mysystem(command2);

    /* 将获取到的信息放到缓冲区buf中 */
    if ((fp = fopen(fileName, "r")) == NULL)
    {
        fprintf(stderr, "Fail to fopen %s\n", fileName);
        unlink(fileName);
        return -1;
    }

    while(fgets(buf, sizeof(buf), fp) != NULL)
    {
        if (buf[strlen(buf)-1] == '\n')
        {
            buf[strlen(buf)-1] =  '\0';
        }

        char *point = strcasestr(buf, string);
        if (point == NULL)
        {
            iRet = 0;
            continue;
        }
        else
        {
            iRet = 1;
            break;
        }
    }
    if (fclose(fp) == -1)
    {
        perror("fclose");
        iRet =    -1;
    }
    unlink(fileName);

    return iRet;
}

int ISO8601TimeGet(time_t time, int nTimeZone, char *pBuf, int nBufLen)
{
	if (NULL == pBuf || nBufLen <= 0)
	{
		return -1;
	}

	struct tm now_time = {0};
	char cSigne = 0;

    localtime_r(&time, &now_time);

	if (nTimeZone >= 0)
	{
		cSigne = '+';
	}
	else
	{
		cSigne = '-';
		nTimeZone *= -1;
	}
	snprintf(pBuf, nBufLen, "%04d%02d%02dT%02d%02d%02d%c%02d",
		now_time.tm_year + 1900,
		now_time.tm_mon + 1,
		now_time.tm_mday,
		now_time.tm_hour,
		now_time.tm_min,
		now_time.tm_sec,
		cSigne,
		nTimeZone);

	return 0;
}

/*-******************************************************************
在str1字符串中，找到第一个'-'的位置，拼接字符串str2，输出
--------------------------
参数 outstr[out]:输出结果字符串
参数 outstrlen[in]:输出结果字符串长度
--------------------------
返回 ：0-成功，-1-失败（缓冲区不足）
*******************************************************************-*/
int InsertStringAtFirstDash(const char* str1, const char* str2, char *outstr, unsigned int outstrlen)
{
    // 参数检查
    if (str1 == NULL || str2 == NULL || outstr == NULL || outstrlen == 0)
    {
        return -1;
    }

    const char* dashPos = strchr(str1, '-');

    if (dashPos == NULL)
    {
        // 直接拼接
        int result = snprintf(outstr, outstrlen, "%s%s", str1, str2);
        return (result < 0 || (unsigned int)result >= outstrlen) ? -1 : 0;
    }
    else
    {
        // 计算插入位置
        size_t prefixLen = dashPos - str1;

        // 分步构建，确保安全
        int written = 0;

        // 复制前缀
        written = snprintf(outstr, outstrlen, "%.*s", (int)prefixLen, str1);
        if (written < 0 || (unsigned int)written >= outstrlen) return -1;

        // 拼接str2
        unsigned int remaining = outstrlen - written;
        written += snprintf(outstr + written, remaining, "%s", str2);
        if (written < 0 || (unsigned int)written >= outstrlen) return -1;

        // 拼接剩余部分
        remaining = outstrlen - written;
        snprintf(outstr + written, remaining, "%s", dashPos);
    }

    return 0;
}

