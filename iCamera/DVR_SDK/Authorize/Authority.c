/*****************************************************************************
 * Copyright (C) 2018 All rights reserved.
 * 文件名称：Authority.c
 * 创 建 者：樊聪
 * 邮    箱：fancong20@163.com
 * 创建日期：2018年08月16日 星期四
 * 描    述：
 *  
 * 更新日志：2018年08月22日 星期三         初版完成
 *  
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#if defined (__MUSL__)
#include <sys/time.h>
#include <time.h>
#endif
#include "zwdes_api.h"
#include "Authority.h"
#include "mauth.h"
#include "PUB_BoradInfo.h"
#include "LogManager.h"
#include "inifile.h"
#include "global_macro.h"

/* 消除本文件中的snprintf截断警告 */
#pragma GCC diagnostic ignored "-Wformat-truncation"
/* 消除本文件中的类型转换警告 */
#pragma GCC diagnostic ignored "-Wcast-qual"

#define AUTH_ERR(format...)          LOG_E(LOG_MODULE_AUTH,format)
#define AUTH_DBG(format...)          LOG_D(LOG_MODULE_AUTH,format)
#define AUTH_INF(format...)          LOG_I(LOG_MODULE_AUTH,format)

#define DEVICE_INFO_FILE_CFG          "/mnt/app/device/DeviceType.txt"

#define MAUTH_DEFAULT_HANDLE (0)

static char g_szoldPlatformName[3][5] = {"NGLS", "IOTB", "BOTC"};//这几个平台不需要连接串
static stuShmAuthInfo gAuthInfo = {0};

enum LineNum {
    LICENSE_LINENUM			= 1,
    CONNECT_STRING_LINENUM	= 2,
    PLATFORM_LINENUM		= 3,
    PUSH_KEY_LINENUM		= 4,
    GG_CLOUD_LINENUM        = 5,
    CRC_LINENUM				= 6,
    LINENUM_MAX
};

#define AUTH_SECTION	"AuthBak"

static char *pAuthField[] = {
	"AuthoriCode",
	"ConnectString",
	"Platform",
	"PushKey",
	"4GCloud",
	"CRC"
};

static char gBoardTypeName[MAX_BOARD_TYPE_NAME_LEN] = {0};  /*设备版型名称*/

static const unsigned int crc32tab[] = {
    0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL,
    0x076dc419L, 0x706af48fL, 0xe963a535L, 0x9e6495a3L,
    0x0edb8832L, 0x79dcb8a4L, 0xe0d5e91eL, 0x97d2d988L,
    0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L, 0x90bf1d91L,
    0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
    0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L,
    0x136c9856L, 0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL,
    0x14015c4fL, 0x63066cd9L, 0xfa0f3d63L, 0x8d080df5L,
    0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L, 0xa2677172L,
    0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
    0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L,
    0x32d86ce3L, 0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L,
    0x26d930acL, 0x51de003aL, 0xc8d75180L, 0xbfd06116L,
    0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L, 0xb8bda50fL,
    0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
    0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL,
    0x76dc4190L, 0x01db7106L, 0x98d220bcL, 0xefd5102aL,
    0x71b18589L, 0x06b6b51fL, 0x9fbfe4a5L, 0xe8b8d433L,
    0x7807c9a2L, 0x0f00f934L, 0x9609a88eL, 0xe10e9818L,
    0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
    0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL,
    0x6c0695edL, 0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L,
    0x65b0d9c6L, 0x12b7e950L, 0x8bbeb8eaL, 0xfcb9887cL,
    0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L, 0xfbd44c65L,
    0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
    0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL,
    0x4369e96aL, 0x346ed9fcL, 0xad678846L, 0xda60b8d0L,
    0x44042d73L, 0x33031de5L, 0xaa0a4c5fL, 0xdd0d7cc9L,
    0x5005713cL, 0x270241aaL, 0xbe0b1010L, 0xc90c2086L,
    0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
    0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L,
    0x59b33d17L, 0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL,
    0xedb88320L, 0x9abfb3b6L, 0x03b6e20cL, 0x74b1d29aL,
    0xead54739L, 0x9dd277afL, 0x04db2615L, 0x73dc1683L,
    0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
    0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L,
    0xf00f9344L, 0x8708a3d2L, 0x1e01f268L, 0x6906c2feL,
    0xf762575dL, 0x806567cbL, 0x196c3671L, 0x6e6b06e7L,
    0xfed41b76L, 0x89d32be0L, 0x10da7a5aL, 0x67dd4accL,
    0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
    0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L,
    0xd1bb67f1L, 0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL,
    0xd80d2bdaL, 0xaf0a1b4cL, 0x36034af6L, 0x41047a60L,
    0xdf60efc3L, 0xa867df55L, 0x316e8eefL, 0x4669be79L,
    0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
    0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL,
    0xc5ba3bbeL, 0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L,
    0xc2d7ffa7L, 0xb5d0cf31L, 0x2cd99e8bL, 0x5bdeae1dL,
    0x9b64c2b0L, 0xec63f226L, 0x756aa39cL, 0x026d930aL,
    0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
    0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L,
    0x92d28e9bL, 0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L,
    0x86d3d2d4L, 0xf1d4e242L, 0x68ddb3f8L, 0x1fda836eL,
    0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L, 0x18b74777L,
    0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
    0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L,
    0xa00ae278L, 0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L,
    0xa7672661L, 0xd06016f7L, 0x4969474dL, 0x3e6e77dbL,
    0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L, 0x37d83bf0L,
    0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
    0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L,
    0xbad03605L, 0xcdd70693L, 0x54de5729L, 0x23d967bfL,
    0xb3667a2eL, 0xc4614ab8L, 0x5d681b02L, 0x2a6f2b94L,
    0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL, 0x2d02ef8dL
};

/*
 * 支持内存分块计算crc
 * param:
 *      crc: crc起始值
 *      buf：需计算crc内存的起始值
 *      size：内存大小
 */
static unsigned int crc32(unsigned int crc, const unsigned char *buf, unsigned int size)
{
     unsigned int i;
     crc ^= 0xFFFFFFFF;
 
     for (i = 0; i < size; i++)
      crc = crc32tab[(crc ^ buf[i]) & 0xff] ^ (crc >> 8);
 
     return crc^0xFFFFFFFF;
}


/*@breif GetBoardTypeFromCfg 从配置文件里面获取版型信息
 *@param [OUT] buf 得到的缓冲区
 *@param [IN] bufsize 缓冲区大小
 *@return 成功返回0
 *@      失败返回-1 文件不存在或内容为空
 */
static int GetBoardTypeFromCfg(unsigned char *buf, unsigned int bufsize)
{
    if (NULL == buf)
    {
        AUTH_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }
    memset(buf, 0, bufsize);

    struct stat fileStat;

    if (stat(DEVICE_INFO_FILE_CFG, &fileStat))
    {
        return -1;
    }

    FILE *fp = NULL;
    char tmp[128] = {0};

    fp = fopen(DEVICE_INFO_FILE_CFG, "r");
    if (NULL == fp)
    {
        AUTH_ERR("Fail to fopen %s\n", DEVICE_INFO_FILE_CFG);
        return -1;
    }
    fread(tmp, sizeof(char), sizeof(tmp), fp);
    fclose(fp);
    fp = NULL;

    if (strlen(tmp))
    {
        sscanf(tmp, "TYPE_%s", buf);
    }
    else
    {
        return -1;
    }

    return 0;
}

/*@breif CheckBoardIdAndSensorIsMatch 检测当前的BoardID是否和sensor匹配
 *@param [IN]  board_id 需要匹配的BoardID
 *@param [IN]  sensor_name 需要匹配的SensorName
 *@return 匹配返回1
 *@      不匹配返回0
 */
static int CheckBoardIdAndSensorIsMatch(unsigned long board_id, const unsigned char *sensor_name)
{
    if (NULL == sensor_name)
    {
        AUTH_ERR("NULL POINTER EXCEPTION!\n");
        return 0;
    }

    int i = 0;
    unsigned char szBoardType[64] = {0};

    // 存在版型信息, 获取指定版型boardID
    int rc = GetBoardTypeFromCfg(szBoardType, sizeof(szBoardType));
    if (0 == rc)
    {
        for (i=0; i<gstuBoardInfoSize; i++)
        {
            if (board_id == gstuBoardInfo[i].dwBoardId)
            {
                if ((0 == strncasecmp((const char*)szBoardType, (const char*)gstuBoardInfo[i].szBoardType, sizeof(gstuBoardInfo[i].szBoardType))) &&
                    (0 == strncasecmp((const char*)sensor_name, (const char*)gstuBoardInfo[i].szSensorName, sizeof(gstuBoardInfo[i].szSensorName))))
                {
                    return 1;
                }
            }
        }
    }

    return 0;
}


/*@breif GetDefaultBoardIDBySensor 通过sensor选取一个默认支持该sensor的BoardID
 *@param [IN]  sensor_name 需要匹配的SensorName
 *@return 如果找到了BoardID，返回该BoardID
 *@      没有找到返回0
 *@note  默认版型用于在授权出错的时候，返回一个默认支持的作为系统的BoardId
 */
static unsigned long GetDefaultBoardIDBySensor(const unsigned char *sensor_name)
{
    if (NULL == sensor_name)
    {
        AUTH_ERR("NULL POINTER EXCEPTION!\n");
        return 0;
    }

    int i = 0;

    for (i=0; i<gstuBoardInfoSize; i++)
    {
        if (0 == strncmp((const char*)sensor_name, (const char*)gstuBoardInfo[i].szSensorName, sizeof(gstuBoardInfo[i].szSensorName)))
        {
            return gstuBoardInfo[i].dwBoardId;
        }
    }    

    return 0;
}

/*@breif IsValidMacAddr MAC地址合法性判断
 *@param [IN] szMac 六个字节的MAC地址 
 *@return 合法返回1
 *@      不合法返回0
 *@note  验证规则:
 *@      1. 每一位必须在0到255之间
 *@      2. 第一位必须是偶数
 */
static int IsValidMacAddr(unsigned char szMac[MAC_ADDR_LEN])
{
    int i = 0;
    unsigned int ucMac[MAC_ADDR_LEN] = {0};

    for (i=0; i<MAC_ADDR_LEN; i++)
    {
        ucMac[i] = szMac[i];
        if (ucMac[i] < 0 || ucMac[i] > 255)
        {
            AUTH_ERR("mac bit %d is %d out of [0, 255]\n", i, ucMac[i]);
            return 0;
        }
    }

    if (0 == DATA_IS_EVEN(ucMac[0]))
    {
        AUTH_ERR("mac bit 0 must be even but is %d\n", ucMac[0]);
        return 0;
    }

    return 1;
}

/*@breif IsValidDidNum DID合法性判断
 *@param [IN] did_num DID号 
 *@return 合法返回1
 *@      不合法返回0
 *@note  验证规则:
 *@      1. 总长度不能超过MAX_DID_NUM_LEN；
 *@      2. 如果是带推送的DID(带#字符)，需要判断后面推送DID的长度；
 */
static int IsValidDidNum(const char *did_num)
{
    if (NULL == did_num)
    {
        AUTH_ERR("NULL POINTER EXCEPTION!\n");
        return 0;
    }

    if (strlen(did_num) > MAX_DID_NUM_LEN)
    {
        AUTH_ERR("DID num count over %d\n", MAX_DID_NUM_LEN);
        return 0;
    }

    char *pPushIdStartPos = NULL;
    pPushIdStartPos = strchr(did_num, MAX_PUSH_ID_SFD);
    if (pPushIdStartPos)
    {
        pPushIdStartPos ++;
        if ((strlen(pPushIdStartPos) <= 0)
         || (strlen(pPushIdStartPos) > MAX_PUSH_ID_LENGTH))
        {
            AUTH_ERR("PUSH ID len invalid!\n");
            return 0;
        }
    }

    return 1;
}

static int GetStringFromCfg(int lineNum, unsigned char * string, unsigned int string_size)
{
	int nRet = 0;
	
    if (NULL == string)
    {
        AUTH_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if (lineNum > (sizeof(pAuthField)/sizeof(char *)))
    {
		AUTH_ERR("lineNum d% big!\n",lineNum);
        return -1;
    }
	
	memset(string, 0, string_size);
	nRet = read_profile_string(AUTH_SECTION,pAuthField[lineNum-1],(char*)string,string_size,NULL,AUTHORIZE_LICENSE_FILE_CFG);
	if (0 == nRet)
	{
		AUTH_ERR("read_profile_string Fail\n");
		return -1;
	}

    return 0;
}

static int SetStringToCfg(int lineNum, const unsigned char *string, unsigned int string_size)
{
    if (NULL == string)
    {
        AUTH_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if (lineNum > (sizeof(pAuthField)/sizeof(char *)))
    {
		AUTH_ERR("lineNum d% big!\n",lineNum);
        return -1;
    }

    if (0 == write_profile_string(AUTH_SECTION,pAuthField[lineNum-1],(const char*)string,AUTHORIZE_LICENSE_FILE_CFG))
	{
		AUTH_ERR("Fail to called SetAuthLicenseToLicense to bak!\n");
		return -1;
	}

    return 0;
}

static int GetSensorNameFromCfg(unsigned char *sensor_name, unsigned int sensor_name_size)
{
	FILE *fp = NULL;
	
	if (NULL == sensor_name)
	{
		AUTH_ERR("NULL POINTER EXCEPTION!\n");
		return -1;
	}

	fp = fopen(SENSOR_NAME_PATH, "r");
	if (NULL == fp)
	{
		AUTH_ERR("Fail to fopen %s\n", SENSOR_NAME_PATH);
		return -1;
	}
	
	fread(sensor_name, sizeof(char), sensor_name_size, fp);
	
	fclose(fp);

	/* 去掉结尾的回车换行 */
	if (strlen((const char*)sensor_name))
	{
		//sensor_name[strlen(sensor_name)-1] = '\0';
	}
	else
	{
		return -1;
	}

	return 0;
}

static int GetStringFromPartition(int lineNum, unsigned char *string, unsigned int string_size)
{
    int nRet = -1;
    char P2PLinkCode[LICENSE_CODE_MAX_LEN] = {0};
    char *pPlatfrom = NULL;
    char *pPushKey = NULL;
    char szP2PLinkCode[256] = {0};
    char szP2PLinkCodeTmp[256] = {0};
    char szPlatfrom[16] = {0};
    char szPushKey[32] = {0};
    
    char *pDomain = NULL;
    char DomainSuffix[64] = {0};
    char szCloud4GHost[128] = {0};
    char szCloud4GStr[128] = {0};
    int szCloud4GPort = 0;

    if (NULL == string)
    {
        AUTH_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    stuShmAuthInfo auth_info = {0};
    nRet = Authorize_GetAuthDevInfo(&auth_info);
    if (nRet < 0)
    {
        AUTH_ERR("Authorize GetAuthDevInfo\n");
    }
    else
    {
        if ((pDomain = strstr((const char*)auth_info.szConnectString, ".com")) 
                || (strstr((const char*)auth_info.szConnectString, "#"))
                )//判断licenseInfo是否存在域名后缀
        {
            //从licenseInfo数组中找到授权信息		（当用新授权工具会下发域名后缀此时连接串格式是：szP2PLinkCode@DomainSuffix@szPlatfrom@szPushKey）
            memcpy(P2PLinkCode, auth_info.szConnectString, LICENSE_CODE_MAX_LEN);
            // 初始化解析起始指针
            char *start = P2PLinkCode;
            char *end;

            // 解析 szP2PLinkCodeTmp
            end = strchr(start, '@');
            if (end) {
                strncpy(szP2PLinkCodeTmp, start, end - start);
                szP2PLinkCodeTmp[end - start] = '\0';
                start = end + 1;
            }

            // 判断并解析 optionalField
            end = strchr(start, '@');
            if (end && strchr(end + 1, '@')) { // 确保后续还有多余的 `@`
                strncpy(DomainSuffix, start, end - start);
                DomainSuffix[end - start] = '\0';
                start = end + 1;
            }

            // 解析 szPlatfrom
            end = strchr(start, '@');
            if (end) {
                strncpy(szPlatfrom, start, end - start);
                szPlatfrom[end - start] = '\0';
                start = end + 1;
            }

            // 解析 szPushKey
            end = strchr(start, '#');
            if (end) {
                strncpy(szPushKey, start, end - start);
                szPushKey[end - start] = '\0';
                start = end + 1;
            } else {
                strncpy(szPushKey, start, strlen(start));
                szPushKey[strlen(start)] = '\0';
                start = NULL; // 终止后续解析
            }

            // 解析 Cloud4GHost（可选）
            if (start && (end = strchr(start, '#'))) {
                strncpy(szCloud4GHost, start, end - start);
                szCloud4GHost[end - start] = '\0';
                start = end + 1;
            } else if (start) {
                // 如果只有一个字段（没有第二个 `#`），直接解析为 Cloud4GHost
                strncpy(szCloud4GHost, start, strlen(start));
                szCloud4GHost[strlen(start)] = '\0';
                start = NULL; // 终止后续解析
            }

            // 解析 Cloud4GPort（可选）
            if (start) {
                szCloud4GPort = atoi(start);
            }
            if (DomainSuffix[0])
            {
                snprintf(szP2PLinkCode, sizeof(szP2PLinkCode), "%s@%s", szP2PLinkCodeTmp, DomainSuffix);
            }
            else
            {
                snprintf(szP2PLinkCode, sizeof(szP2PLinkCode), "%s", szP2PLinkCodeTmp);
            }
            if(szCloud4GHost[0])
            {
                snprintf(szCloud4GStr, sizeof(szCloud4GStr), "%s#%d", szCloud4GHost, szCloud4GPort);
            }
            else
            {
                memset(szCloud4GStr, 0, sizeof(szCloud4GStr));
            }
        }
        else
        {
            //从licenseInfo数组中找到授权信息		（当用旧授权工具没有下发域名后缀此时连接串格式是：szP2PLinkCode@szPlatfrom@szPushKey）
            memcpy(P2PLinkCode, auth_info.szConnectString, LICENSE_CODE_MAX_LEN);
            pPlatfrom = strchr(P2PLinkCode, '@');
            pPushKey = strrchr(P2PLinkCode, '@');
            if (NULL != pPlatfrom && NULL != pPlatfrom)
            {
                memcpy(szP2PLinkCode, P2PLinkCode, pPlatfrom-P2PLinkCode);
                memcpy(szPlatfrom, pPlatfrom+1, pPushKey-pPlatfrom-1);
                memcpy(szPushKey, pPushKey+1, P2PLinkCode+strlen(P2PLinkCode)-pPushKey-1);
            }
        }
    }

	memset(string, 0, string_size);

    switch(lineNum)
    {
        case CONNECT_STRING_LINENUM:
            memcpy(string, szP2PLinkCode, sizeof(szP2PLinkCode));
            break;
        case PLATFORM_LINENUM:
            memcpy(string, szPlatfrom, sizeof(szPlatfrom));
            break;
        case PUSH_KEY_LINENUM:
            memcpy(string, szPushKey, sizeof(szPushKey));
            break;
        case GG_CLOUD_LINENUM:
            memcpy(string, szCloud4GStr, sizeof(szCloud4GStr));
            break;
    }

    return 0;
}

static int Authorize_InitAuthDevInfo(unsigned char *pAuthCode, unsigned char *pP2PLinkCode)
{
    int ret = 0;
    MAUTH_DEV_ATTR_S stMauthDevAttr = {0};

    if (NULL == pAuthCode || NULL == pP2PLinkCode)
    {
        AUTH_ERR("pAuthCode or pP2PLinkCode IS NULL\n");
        return -1;
    }

	ret = MAuth_Dev_Open();
	if (ret < 0)
	{
		AUTH_ERR("MAuth Dev Open failed![ret=%#x]\n", ret);
        return -1;
	}
    else
    {
        ret = MAuth_Dev_GetDevAttr(0, &stMauthDevAttr);
        if (ret < 0)
        {
            AUTH_ERR("MAuth Dev GetDevAttr failed![ret=%#x]\n", ret);
        }
        else
        {
            /*授权码*/
            memcpy(gAuthInfo.szAuthCode, stMauthDevAttr.AuthCode, sizeof(gAuthInfo.szAuthCode));
            /*MAC 地址*/
            memcpy(gAuthInfo.szMac, stMauthDevAttr.MacAddr, sizeof(gAuthInfo.szMac));
            /*连接串*/
            memcpy(gAuthInfo.szConnectString, stMauthDevAttr.ConnectString, sizeof(gAuthInfo.szConnectString));
            /*序列号*/
            memcpy(gAuthInfo.szSerialNum, stMauthDevAttr.SerialNum, sizeof(gAuthInfo.szSerialNum));
            /*DID信息*/
            memcpy(gAuthInfo.struCloudInfo.szDid, stMauthDevAttr.DIDInfo.szDid, sizeof(gAuthInfo.struCloudInfo.szDid));
            gAuthInfo.struCloudInfo.szLen = stMauthDevAttr.DIDInfo.szLen;
            /*版型ID*/
            gAuthInfo.dwBoardID = stMauthDevAttr.BoardID;
            /*激活码*/
            gAuthInfo.dwFuncion = stMauthDevAttr.Function;
			/*版型名称*/
        	memcpy(gBoardTypeName, stMauthDevAttr.BoardTypeName, sizeof(gBoardTypeName));
            AUTH_INF("Get Auth Dev Attr Success\n");
        }

        ret = MAuth_Dev_Close(0);
        if (ret < 0)
        {
            AUTH_ERR("MAuth Dev Close failed![ret=%#x]\n", ret);
        }
    }

    memcpy(pAuthCode, &gAuthInfo.szAuthCode, sizeof(gAuthInfo.szAuthCode));
    memcpy(pP2PLinkCode, &gAuthInfo.szConnectString, sizeof(gAuthInfo.szConnectString));

    return 0;
}

static int Authorize_CheckAuthCode(unsigned char *pAuthCode, int AuthCodeSize, unsigned char *pP2PLinkString, int P2PLinkStringSize)
{
    int ret = 0;
    int ret_conf = -1;
    stuShmAuthInfo auth_info = {0};
    unsigned char license_conf[LICENSE_CODE_MAX_LEN] = {0};

	//从配置文件中获取授权码
	ret_conf = GetStringFromCfg(LICENSE_LINENUM, license_conf, sizeof(license_conf));
    if (ret_conf < 0)
    {
        AUTH_ERR("Get AuthLicenseFromCfg Failed\n");
    }

	//如果两个地方都没有获取到授权码，则判断授权失败
	if (!AuthCodeSize && ret_conf)
	{
        AUTH_ERR("NO AuthCode\n");
        //Authorize_SetDefaultInfo();
		return -1;
	}

    memset(&auth_info, 0, sizeof(stuShmAuthInfo));

	//恢复授权码，并写到共享内存
    if (AuthCodeSize && ret_conf) // 分区 OK; CFG ERR
    {
        if (AUTH_SUCCESS == DeEncryptLicense(pAuthCode, &auth_info))
        {
            SetStringToCfg(LICENSE_LINENUM, pAuthCode, AuthCodeSize);
            if (P2PLinkStringSize)
            {
                memcpy(auth_info.szConnectString, pP2PLinkString, sizeof(auth_info.szConnectString));
            }
            ret = Authorize_SetAuthDevInfo(&auth_info);
            if (ret < 0)
            {
                AUTH_ERR("Authorize SetAuthDevInfo failed!\n");
            }
            AUTH_INF("Check Licnese recover from partition Success\n");
        }
    }
    else if (!AuthCodeSize && (0 == ret_conf)) // 分区 ERR; CFG OK
    {
        if (AUTH_SUCCESS == DeEncryptLicense(license_conf, &auth_info))
        {
            if (P2PLinkStringSize)
            {
                memcpy(auth_info.szConnectString, pP2PLinkString, sizeof(auth_info.szConnectString));
            }
			memcpy(auth_info.szAuthCode, license_conf, LICENSE_CODE_MAX_LEN);
            ret = Authorize_SetAuthDevInfo(&auth_info);
            if (ret < 0)
            {
                AUTH_ERR("Authorize SetAuthDevInfo failed!\n");
            }
            AUTH_INF("Check Licnese recover from cfg Success\n");
        }
    }
    else // 分区 OK; CFG OK
    {
        /* 分区和CFG里面的License相等 */
        if (0 == strncmp((char *)pAuthCode, (char *)license_conf, AuthCodeSize))
        {
            if (AUTH_SUCCESS == DeEncryptLicense(pAuthCode, &auth_info))
            {
                if (P2PLinkStringSize)
                {
                    memcpy(auth_info.szConnectString, pP2PLinkString, sizeof(auth_info.szConnectString));
                }
                ret = Authorize_SetAuthDevInfo(&auth_info);
                if (ret < 0)
                {
                    AUTH_ERR("Authorize SetAuthDevInfo failed!\n");
                }
                AUTH_INF("Check Licnese from partition and cfg same Success!!!\n");
            }
        }
        else
        {
            /* 分区里面的license检测成功，则重新写入配置 */
            if (AUTH_SUCCESS == DeEncryptLicense(pAuthCode, &auth_info))
            {
                SetStringToCfg(LICENSE_LINENUM, pAuthCode, AuthCodeSize);
                if (P2PLinkStringSize)
                {
                    memcpy(auth_info.szConnectString, pP2PLinkString, sizeof(auth_info.szConnectString));
                }
                ret = Authorize_SetAuthDevInfo(&auth_info);
                if (ret < 0)
                {
                    AUTH_ERR("Authorize SetAuthDevInfo failed!\n");
                }
                AUTH_INF("Check Licnese No The Same, Get From Partition\n");
            }
            /* 配置里面的license检测成功，则重新写入分区 */
            else if (AUTH_SUCCESS == DeEncryptLicense(license_conf, &auth_info))
            {
                if (P2PLinkStringSize)
                {
                    memcpy(auth_info.szConnectString, pP2PLinkString, sizeof(auth_info.szConnectString));
                }
                memcpy(auth_info.szAuthCode, license_conf, LICENSE_CODE_MAX_LEN);
                ret = Authorize_SetAuthDevInfo(&auth_info);
                if (ret < 0)
                {
                    AUTH_ERR("Authorize SetAuthDevInfo failed!\n");
                }
            }
			AUTH_INF("Check Licnese No The Same, Get From Cfg\n");
        }
    }

    return 0;
}

static int Authorize_CheckConnectString(unsigned char *pP2PLinkString, int P2PLinkStringSize)
{
    int ret = 0;
    stuShmAuthInfo auth_info = {0};
    unsigned char crcBuf[512] = {0};
    unsigned int crcLen = 0;
    unsigned int crc = 0;
    char crcString[64] = {0};
    int partition_crc_correct = 0;
    int cfg_crc_correct = 0;
    unsigned char szP2PLinkCode[LICENSE_CODE_MAX_LEN] = {0};
    unsigned char P2PLinkCode[LICENSE_CODE_MAX_LEN] = {0};
    unsigned char license_conf[LICENSE_CODE_MAX_LEN] = {0};
    char szPlatfrom[16] = {0};
    char szPushKey[32] = {0};
    char sz4GCloud[128] = {0};
	int i = 0;

	for (i = 0; i < sizeof(g_szoldPlatformName)/sizeof(g_szoldPlatformName[0]) ; i++)
	{
		if (!strncmp((char *)auth_info.struCloudInfo.szDid, g_szoldPlatformName[i], 4))
		{
			//在支持表示是老的DID，不需要进行连接串的设置，直接返回0
			return 0;
		}
	}

    if (!P2PLinkStringSize)
    {
        partition_crc_correct = -1;
    }

	//对比配置里的CRC
    memset(crcBuf, 0, sizeof(crcBuf));
    memset(crcString, 0, sizeof(crcString));
    crcLen = 0;
    GetStringFromCfg(CONNECT_STRING_LINENUM, license_conf, sizeof(license_conf));
    memcpy(crcBuf, license_conf, strlen((char *)license_conf));
    crcLen += strlen((char *)license_conf);
    
    GetStringFromCfg(PLATFORM_LINENUM, license_conf, sizeof(license_conf));
    memcpy(crcBuf+crcLen, license_conf, strlen((char *)license_conf));
    crcLen += strlen((char *)license_conf);
    
    GetStringFromCfg(PUSH_KEY_LINENUM, license_conf, sizeof(license_conf));
    memcpy(crcBuf+crcLen, license_conf, strlen((char *)license_conf));
    crcLen += strlen((char *)license_conf);

    GetStringFromCfg(GG_CLOUD_LINENUM, license_conf, sizeof(license_conf));
    memcpy(crcBuf+crcLen, license_conf, strlen((char *)license_conf));
    crcLen += strlen((char *)license_conf);

    crcBuf[crcLen] = '\0';
    crc = crc32(0, crcBuf, crcLen);
    snprintf(crcString, sizeof(crcString), "%u", crc);
    GetStringFromCfg(CRC_LINENUM, license_conf, sizeof(license_conf));

    if (!strncmp(crcString, (char *)license_conf, sizeof(crcString)))
    {
        cfg_crc_correct = 1;
    }

	//对连接串进行恢复
	if ((0 != partition_crc_correct) && (0 == cfg_crc_correct)) // 分区 ERR; CFG ERR
	{
        AUTH_ERR("Get Connect string No Access!!!\n");
		return -1;
	}
	else if ((0 == partition_crc_correct) && (0 == cfg_crc_correct)) // 分区 OK; CFG ERR
	{
		memset(crcBuf, 0, sizeof(crcBuf));
		memset(crcString, 0, sizeof(crcString));

		GetStringFromPartition(CONNECT_STRING_LINENUM, P2PLinkCode, sizeof(P2PLinkCode));
		SetStringToCfg(CONNECT_STRING_LINENUM, P2PLinkCode, sizeof(P2PLinkCode));
		memcpy(crcBuf, P2PLinkCode, strlen((char *)P2PLinkCode));
		crcLen += strlen((char *)P2PLinkCode);

		GetStringFromPartition(PLATFORM_LINENUM, P2PLinkCode, sizeof(P2PLinkCode));
		SetStringToCfg(PLATFORM_LINENUM, P2PLinkCode, sizeof(P2PLinkCode));
		memcpy(crcBuf+crcLen, P2PLinkCode, strlen((char *)P2PLinkCode));
		crcLen += strlen((char *)P2PLinkCode);

		GetStringFromPartition(PUSH_KEY_LINENUM, P2PLinkCode, sizeof(P2PLinkCode));
		SetStringToCfg(PUSH_KEY_LINENUM, P2PLinkCode, sizeof(P2PLinkCode));
		memcpy(crcBuf+crcLen, P2PLinkCode, strlen((char *)P2PLinkCode));
		crcLen += strlen((char *)P2PLinkCode);

        GetStringFromPartition(GG_CLOUD_LINENUM, P2PLinkCode, sizeof(P2PLinkCode));
		SetStringToCfg(GG_CLOUD_LINENUM, P2PLinkCode, sizeof(P2PLinkCode));
		memcpy(crcBuf+crcLen, P2PLinkCode, strlen((char *)P2PLinkCode));
		crcLen += strlen((char *)P2PLinkCode);

		crcBuf[crcLen] = '\0';
		crc = crc32(0, crcBuf, crcLen);
		snprintf(crcString, sizeof(crcString), "%u", crc);
		SetStringToCfg(CRC_LINENUM, (unsigned char *)crcString, sizeof((char *)crcString));

		AUTH_INF("Connect string recover from partition\n");
	}
	else if ((0 != partition_crc_correct) && (1 == cfg_crc_correct)) // 分区 ERR; CFG OK
	{
		GetStringFromCfg(CONNECT_STRING_LINENUM, szP2PLinkCode, sizeof(szP2PLinkCode));
		GetStringFromCfg(PLATFORM_LINENUM, (unsigned char *)szPlatfrom, sizeof(szPlatfrom));
		GetStringFromCfg(PUSH_KEY_LINENUM, (unsigned char *)szPushKey, sizeof(szPushKey));
        GetStringFromCfg(GG_CLOUD_LINENUM, (unsigned char *)sz4GCloud, sizeof(sz4GCloud));
		snprintf((char *)P2PLinkCode, sizeof(P2PLinkCode), "%s@%s@%s#%s", szP2PLinkCode, szPlatfrom, szPushKey, sz4GCloud);
        ret = Authorize_GetAuthDevInfo(&auth_info);
        if (ret < 0)
        {
            AUTH_ERR("Authorize GetAuthDevInfo failed!\n");
            return -1;
        }
        memcpy(auth_info.szConnectString, P2PLinkCode, CONNECT_STRING_MAX_LEN);
        ret = Authorize_SetAuthDevInfo(&auth_info);
		if (ret < 0)
		{
			AUTH_ERR("Authorize SetAuthDevInfo failed!\n");
		}

		AUTH_INF("Connect string recover from cfg\n");
	}
	else // U-boot OK; CFG OK
	{
        AUTH_INF("Connect string Get Success!!!\n");
	}

    return 0;
}


/*@breif GetDefaultBoardIDBySensor 获取取一个默认支持该sensor的BoardID
 *@return 如果找到了BoardID，返回该BoardID
 *@      没有找到返回0
 *@note  默认版型用于在授权出错的时候，返回一个默认支持的作为系统的BoardId
 */
unsigned long GetDefaultBoardID(void)
{
    int rc = 0;
    unsigned long boardID = 0;
    unsigned char szSensorName[32] = {0};
    unsigned char szBoardType[64] = {0};

    // 1. 根据sensor获取默认ID
	rc = GetSensorNameFromCfg(szSensorName, sizeof(szSensorName));
    if (0 == rc)
    {
        boardID = GetDefaultBoardIDBySensor(szSensorName);
    }

    // 2. 存在版型信息, 获取指定版型boardID
    rc = GetBoardTypeFromCfg(szBoardType, sizeof(szBoardType));
    if (0 == rc)
    {
        for (unsigned int i = 0; i < gstuBoardInfoSize; i++)
        {
            if (0 == strncasecmp((const char *)szBoardType, (const char *)gstuBoardInfo[i].szBoardType, sizeof(gstuBoardInfo[i].szBoardType)))
            {
                boardID = gstuBoardInfo[i].dwBoardId;
                break;
            }
        }
    }

    return boardID;
}

/*@breif DeEncryptLicense 解密授权码
 *@param [IN] encrypt_license 需要创建的共享内存的首地址 
 *@param [OUT] p_auth_info 解密出的信息
 *@return 返回错误码
 */
int DeEncryptLicense(const unsigned char *encrypt_license, pShmAuthInfo p_auth_info)
{
    if (NULL==encrypt_license || NULL==p_auth_info)
    {
        AUTH_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    int ret = -1;
    unsigned char sensor_name[32] = {0};
    license lic;
    memset(&lic, 0, sizeof(license));

    ret = testProbe((char *)encrypt_license, &lic);

    /* 解码授权码 */
    if (0 != ret)
    {
        switch(ret)
        {
            case ERROR_INVALID_LICENSECODE:
            {
                p_auth_info->dwAuthVerifyErrno = AUTH_LIC_CODE_INVALID;
                break;
            }
            case ERROR_INVLIAD_ARGRUMENT:
            {
                p_auth_info->dwAuthVerifyErrno = AUTH_LIC_CODE_ARG_ERR;
                break;
            }
            case ERROR_ALLOC_MEM:
            {
                p_auth_info->dwAuthVerifyErrno = AUTH_LIC_CODE_ALLOC_MEM_ERR;
                break;
            }
            case ERROR_INVALID_LICENSELEN:
            {
                p_auth_info->dwAuthVerifyErrno = AUTH_LIC_CODE_LEN_INVALID;
                break;
            }
            case ERROR_INVALID_CUSTOMCODE:
            {
                p_auth_info->dwAuthVerifyErrno = AUTH_CUSTOM_CODE_INVALID;
                break;
            }
            case ERROR_INVALID_CLOUDCODE:
            {
                p_auth_info->dwAuthVerifyErrno = AUTH_DID_INVALID;
                break;
            }
            case ERROR_INVALID_SN:
            {
                p_auth_info->dwAuthVerifyErrno = AUTH_SERIAL_NUM_INVALID;
                break;
            }
        }

        AUTH_ERR("Auth code erified fail in low level, errno: %d!\n", ret);
        return p_auth_info->dwAuthVerifyErrno;
    }

    /* 进行相关参数的检验 */
    /* MAC地址合法性验证 */
    if (0 == IsValidMacAddr(lic.mac))
    {
        AUTH_ERR("mac addr check fail!\n");
        p_auth_info->dwAuthVerifyErrno = AUTH_MAC_INVALID;
        return p_auth_info->dwAuthVerifyErrno;
    }

    /* DID号验证 */
    if (0 == IsValidDidNum(lic.cloudID.did))
    {
        AUTH_ERR("did num check fail!\n");
        p_auth_info->dwAuthVerifyErrno = AUTH_DID_INVALID;
        return p_auth_info->dwAuthVerifyErrno;
    }

    /* 序列号长度合法性 */
    if (strlen(lic.sn) > MAX_SERIAL_NUM_LEN)
    {
        AUTH_ERR("serial num check invaild!\n");
        p_auth_info->dwAuthVerifyErrno = AUTH_SERIAL_NUM_INVALID;
        return p_auth_info->dwAuthVerifyErrno;
    }

	/* 版型是否合法 */
	if (GetSensorNameFromCfg(sensor_name, sizeof(sensor_name)))
	{
		AUTH_ERR("board id check invaild!\n");
		p_auth_info->dwAuthVerifyErrno = AUTH_BOARD_ID_INVALID;
		return p_auth_info->dwAuthVerifyErrno;
	}

    /* 版型是否匹配 */
    if (0 == CheckBoardIdAndSensorIsMatch(lic.boardID, sensor_name))
    {
        AUTH_ERR("board id check not match!\n");
        p_auth_info->dwAuthVerifyErrno = AUTH_BOARD_ID_NOT_MATCH;
        return p_auth_info->dwAuthVerifyErrno;
    }

    /* 参数填充 */
    p_auth_info->dwAuthVerifyErrno = AUTH_SUCCESS;
    p_auth_info->dCustomID = lic.customID; /* 客户编号    */
    p_auth_info->dwBoardID = lic.boardID;  /* 版型ID    */
    p_auth_info->dwAtime = lic.atime;      /* 时间戳     */
    p_auth_info->dwFuncion = lic.funcion;  /* 激活码     */
    strncpy((char *)p_auth_info->szAuthCode, (const char*)encrypt_license, LICENSE_CODE_MAX_LEN - 1);
    memcpy(p_auth_info->szMac, lic.mac, MAC_ADDR_LEN);
    strncpy((char *)p_auth_info->szSerialNum, (const char*)lic.sn, MAX_SERIAL_NUM_LEN - 1);
    memcpy(&p_auth_info->struCloudInfo, &(lic.cloudID), sizeof(StuCloudInfo));

    /* 所有的校验均成功，授权码校验成功 */
    p_auth_info->dwAuthVerifyErrno = AUTH_SUCCESS;
    AUTH_INF("OK! Auth Code verified success!\n");

    return AUTH_SUCCESS;
}

int InitLicenseAuthorize(void)
{
	int ret = -1;
	unsigned char AuthCode[LICENSE_CODE_MAX_LEN] = {0};
	unsigned char P2PLinkString[LICENSE_CODE_MAX_LEN] = {0};
	stuShmAuthInfo auth_info = {0};

    ret = Authorize_InitAuthDevInfo(AuthCode, P2PLinkString);
    if (ret < 0)
    {
        AUTH_ERR("Authorize InitAuthDevInfo failed!\n");
    }

    AUTH_INF("License: %s\n", AuthCode);
    ret = Authorize_CheckAuthCode(AuthCode, strlen((char *)AuthCode), P2PLinkString, strlen((char *)P2PLinkString));
    if (ret < 0)
    {
        goto ERR_NOT_AUTHORIZE;
    }

    AUTH_INF("P2PLinkString: %s\n", P2PLinkString);

    ret = Authorize_CheckConnectString(P2PLinkString, strlen((char *)P2PLinkString));
    if (ret < 0)
    {
        goto ERR_NOT_CONNECT_STRING;
    }

	return 0;

ERR_NOT_CONNECT_STRING:
    AUTH_ERR("Fail to set connect string!\n");
    return 0;

ERR_NOT_AUTHORIZE:
    AUTH_ERR("Fail to auth verify!\n");
    memset(&auth_info, 0, sizeof(stuShmAuthInfo));
    auth_info.dwAuthVerifyErrno = AUTH_NOT_AUTHORIZE;
    ret = Authorize_SetAuthDevInfo(&auth_info);
    if (ret < 0)
    {
        AUTH_ERR("Authorize SetAuthDevInfo failed!\n");
    }

    return 0;
}

/*@breif SetLicenseAuthorize 设置授权码
 *@return 成功返回0
 *@      失败返回-1
 *@note  设置授权码，需要操作以下两步：
 *@      1. 修改DEVINFO分区;
 *@      2. 修改AUTHORIZE_LICENSE_FILE_CFG配置文件;
 */
int SetLicenseAuthorize(const pShmAuthInfo p_auth_info)
{
	int nRet = 0;
	
    if (NULL == p_auth_info)
    {
        AUTH_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

	/* Step 1: 把授权码保存在设备中 */
    nRet = Authorize_SetAuthDevInfo(p_auth_info);
    if (nRet < 0)
    {
        AUTH_ERR("Authorize SetAuthDevInfo! failed\n");
        return -1;
    }
    AUTH_INF("Check Licnese recover from partition Success\n");

    /* Step 2: 设置(修改)AUTHORIZE_LICENSE_FILE_CFG配置文件 */
    if (SetStringToCfg(LICENSE_LINENUM, p_auth_info->szAuthCode, LICENSE_CODE_MAX_LEN))
    {
        AUTH_ERR("Fail to called SetStringToCfg LICENSE_LINENUM!\n");
        return -1;
    }

    return 0;
}

/*@breif RemoveLicenseAuthorize 强制删除授权
 *@return 无
 *@note  删除(取消)授权，需要操作以下两步：
 *@      1. 清除DEVINFO分区;
 *@      2. 删除AUTHORIZE_LICENSE_FILE_CFG配置文件;
 */
void RemoveLicenseAuthorize(void)
{
	int nRet = 0;
	char szBuffer[MAX_DID_LEN] = {0};
    stuShmAuthInfo auth_info;

    memset(&auth_info, 0, sizeof(auth_info));

    nRet = Authorize_SetAuthDevInfo(&auth_info);
    if (nRet < 0)
    {
        AUTH_ERR("Authorize GetAuthDevInfo! Failed\n");
    }

	// 删除配置中的DID号
	if (!access(TutkUID_CONF, F_OK))
	{
		//读之前的文件
		FILE *fp = fopen(TutkUID_CONF, "r");
		if (NULL == fp)
		{
			AUTH_ERR("open file failed [%s]\n", TutkUID_CONF);
		}
		else
		{
			fread(szBuffer, sizeof(szBuffer), 1, fp);
			fclose(fp);
		}

		//当有DID时才进行操作
		if (strlen(szBuffer+2))
		{
			//删除DID
			fp = fopen(TutkUID_CONF, "w");
			if (NULL == fp)
			{
				AUTH_ERR("open file failed [%s]\n", TutkUID_CONF);
			}
			else
			{
				fprintf(fp, "%d:", szBuffer[0] - '0');
				fclose(fp);
			}
		}
	}

    /* Step 2: 删除AUTHORIZE_LICENSE_FILE_CFG配置文件 */
    unlink(AUTHORIZE_LICENSE_FILE_CFG);
    /* Step 3: 删除SPS文件(避免后续授权其他号段后，无法订阅推送问题) */
    unlink(SPS_INFO_CFG);

    AUTH_INF("Warning: authorize license has remove!\n");
    return;
}

void PrintAuthorizeInfo(void)
{
	int nRet = 0;
    int i = 0;
	MAUTH_DEV_ATTR_S stMauthDevAttr = {0};

	nRet = MAuth_Dev_Open();
	if (nRet < 0)
	{
		AUTH_ERR("MAuth Dev Open failed![nRet=%d]\n", nRet);
	}

	nRet = MAuth_Dev_GetDevAttr(0, &stMauthDevAttr);
	if (nRet < 0)
	{
        AUTH_ERR("MAuth Dev GetDevAttr Failed![nRet=%d]\n", nRet);
	}

    printf("----------------------------------------\n");
    printf("AuthCode=%s\n",      stMauthDevAttr.AuthCode);
    printf("DIDInfo=%s\n",       stMauthDevAttr.DIDInfo.szDid);
    printf("MacAddr=");
    for (i = 0; i < 6; i++)
    {
        printf("%#x ", stMauthDevAttr.MacAddr[i]);
    }
    printf("\n");
    printf("ConnectString=%s\n", stMauthDevAttr.ConnectString);
    printf("BoardTypeName=%s\n", stMauthDevAttr.BoardTypeName);
    printf("BoardID=%#x\n",     stMauthDevAttr.BoardID);
    printf("----------------------------------------\n");

	nRet = MAuth_Dev_Close(0);
	if (nRet < 0)
	{
		AUTH_ERR("MAuth Dev Close failed![nRet=%d]\n", nRet);
	}

    printf("+++++++++++++++++++++++++++++++++++++\n");
    printf("gAuthCode=%s\n",      gAuthInfo.szAuthCode);
    printf("gDIDInfo=%s\n",       gAuthInfo.struCloudInfo.szDid);
    printf("gMacAddr=");
    for (i = 0; i < 6; i++)
    {
        printf("%#x ", gAuthInfo.szMac[i]);
    }
    printf("\n");
    printf("gConnectString=%s\n", gAuthInfo.szConnectString);
    printf("gBoardID=%#lx\n",     gAuthInfo.dwBoardID);
    printf("+++++++++++++++++++++++++++++++++++++\n");

    return;
}

/*@breif Authorize_SetConnectInfo 获取连接串信息
 *@return 成功返回0
 *@       失败返回-1
 *@note  获取连接串信息，2种用法：
 *@      1. lineNum = LINENUM_MAX，获取全部信息;
 *@      2. lineNum = 其他，获取某一项信息;
 */
int Authorize_GetConnectInfo(int lineNum, stuConnectInfo* pconnect_info)
{
    if (lineNum < 0 || lineNum > LINENUM_MAX || pconnect_info == NULL)
    {
        AUTH_ERR("Parameter error!! lineNum[%d]\n", lineNum);
        return -1;
    }

    int ret = 0;

    if (lineNum == LINENUM_MAX)
    {
		ret = GetStringFromCfg(CONNECT_STRING_LINENUM, (unsigned char *)pconnect_info->szConnectString, sizeof(pconnect_info->szConnectString));
		ret |= GetStringFromCfg(PLATFORM_LINENUM, (unsigned char *)pconnect_info->szPlatformName, sizeof(pconnect_info->szPlatformName));
		ret |= GetStringFromCfg(PUSH_KEY_LINENUM, (unsigned char *)pconnect_info->szAuthWord, sizeof(pconnect_info->szAuthWord));
		ret |= GetStringFromCfg(GG_CLOUD_LINENUM, (unsigned char *)pconnect_info->sz4GCloud, sizeof(pconnect_info->sz4GCloud));
    }
    else
    {
        if (lineNum == CONNECT_STRING_LINENUM)
        {
    		ret = GetStringFromCfg(CONNECT_STRING_LINENUM, (unsigned char *)pconnect_info->szConnectString, sizeof(pconnect_info->szConnectString));
        }
        else if (lineNum == PLATFORM_LINENUM)
        {
            ret = GetStringFromCfg(PLATFORM_LINENUM, (unsigned char *)pconnect_info->szPlatformName, sizeof(pconnect_info->szPlatformName));
        }
        else if (lineNum == PUSH_KEY_LINENUM)
        {
            ret = GetStringFromCfg(PUSH_KEY_LINENUM, (unsigned char *)pconnect_info->szAuthWord, sizeof(pconnect_info->szAuthWord));
        }
        else if (lineNum == GG_CLOUD_LINENUM)
        {
            ret = GetStringFromCfg(GG_CLOUD_LINENUM, (unsigned char *)pconnect_info->sz4GCloud, sizeof(pconnect_info->sz4GCloud));
        }
    }

    return ret;
}

/*@breif Authorize_SetConnectInfo 设置连接串信息
 *@return 成功返回0
 *@       失败返回-1
 *@note  设置连接串信息，2种用法：
 *@      1. lineNum = LINENUM_MAX，设置全部信息;
 *@      2. lineNum = 其他，设置某一项信息;
 *@      先保存至配置，同样也保存连接串至DEVINFO分区;
 */
int Authorize_SetConnectInfo(int lineNum, const stuConnectInfo* pconnect_info)
{
    if (lineNum < 0 || lineNum > LINENUM_MAX || pconnect_info == NULL)
    {
        AUTH_ERR("Parameter error!! lineNum[%d]\n", lineNum);
        return -1;
    }

    int ret = 0;
	int crcLen = 0;
	unsigned int crc = 0;
	char crcString[64] = {0};
	char P2PLinkCode[256] = {0};
	unsigned char crcBuf[512] = {0};
    stuShmAuthInfo authinfo = {0};
    stuConnectInfo connect_info;
    memset(&connect_info, 0, sizeof(connect_info));

    if (lineNum == LINENUM_MAX)
    {
    	//把连接串写入文件
    	SetStringToCfg(CONNECT_STRING_LINENUM, (const unsigned char *)pconnect_info->szConnectString, sizeof(pconnect_info->szConnectString));
    	memcpy(crcBuf, pconnect_info->szConnectString, strlen(pconnect_info->szConnectString));
    	crcLen += strlen(pconnect_info->szConnectString);

    	//把平台名字写入文件
    	SetStringToCfg(PLATFORM_LINENUM, (const unsigned char *)pconnect_info->szPlatformName, sizeof(pconnect_info->szPlatformName));
    	memcpy(crcBuf+crcLen, pconnect_info->szPlatformName, strlen(pconnect_info->szPlatformName));
    	crcLen += strlen(pconnect_info->szPlatformName);

    	//把PUSH KEY写入文件
    	SetStringToCfg(PUSH_KEY_LINENUM, (const unsigned char *)pconnect_info->szAuthWord, sizeof(pconnect_info->szAuthWord));
    	memcpy(crcBuf+crcLen, pconnect_info->szAuthWord, strlen(pconnect_info->szAuthWord));
    	crcLen += strlen(pconnect_info->szAuthWord);

    	SetStringToCfg(GG_CLOUD_LINENUM, (const unsigned char *)pconnect_info->sz4GCloud, sizeof(pconnect_info->sz4GCloud));
        memcpy(crcBuf+crcLen, pconnect_info->sz4GCloud, strlen(pconnect_info->sz4GCloud));
        crcLen += strlen(pconnect_info->sz4GCloud);

        memcpy(&connect_info, pconnect_info, sizeof(stuConnectInfo));
    }
    else
    {
        if (lineNum == CONNECT_STRING_LINENUM)
        {
        	SetStringToCfg(CONNECT_STRING_LINENUM, (const unsigned char *)pconnect_info->szConnectString, sizeof(pconnect_info->szConnectString));
        	memcpy(crcBuf, pconnect_info->szConnectString, strlen(pconnect_info->szConnectString));
        	crcLen += strlen(pconnect_info->szConnectString);

            memcpy(connect_info.szConnectString, pconnect_info->szConnectString, sizeof(pconnect_info->szConnectString));
        }
        else
        {
            GetStringFromCfg(CONNECT_STRING_LINENUM, (unsigned char *)connect_info.szConnectString, sizeof(connect_info.szConnectString));
            memcpy(crcBuf, connect_info.szConnectString, strlen(connect_info.szConnectString));
            crcLen += strlen(connect_info.szConnectString);
        }

        if (lineNum == PLATFORM_LINENUM)
        {
        	SetStringToCfg(PLATFORM_LINENUM, (const unsigned char *)pconnect_info->szPlatformName, sizeof(pconnect_info->szPlatformName));
        	memcpy(crcBuf+crcLen, pconnect_info->szPlatformName, strlen(pconnect_info->szPlatformName));
        	crcLen += strlen(pconnect_info->szPlatformName);

            memcpy(connect_info.szPlatformName, pconnect_info->szPlatformName, sizeof(pconnect_info->szPlatformName));
        }
        else
        {
            GetStringFromCfg(PLATFORM_LINENUM, (unsigned char *)connect_info.szPlatformName, sizeof(connect_info.szPlatformName));
            memcpy(crcBuf+crcLen, connect_info.szPlatformName, strlen(connect_info.szPlatformName));
            crcLen += strlen(connect_info.szPlatformName);
        }

        if (lineNum == PUSH_KEY_LINENUM)
        {
        	SetStringToCfg(PUSH_KEY_LINENUM, (const unsigned char *)pconnect_info->szAuthWord, sizeof(pconnect_info->szAuthWord));
        	memcpy(crcBuf+crcLen, pconnect_info->szAuthWord, strlen(pconnect_info->szAuthWord));
        	crcLen += strlen(pconnect_info->szAuthWord);

            memcpy(connect_info.szAuthWord, pconnect_info->szAuthWord, sizeof(pconnect_info->szAuthWord));
        }
        else
        {
            GetStringFromCfg(PUSH_KEY_LINENUM, (unsigned char *)connect_info.szAuthWord, sizeof(connect_info.szAuthWord));
            memcpy(crcBuf+crcLen, connect_info.szAuthWord, strlen(connect_info.szAuthWord));
            crcLen += strlen(connect_info.szAuthWord);
        }

        if (lineNum == GG_CLOUD_LINENUM)
        {
        	SetStringToCfg(GG_CLOUD_LINENUM, (const unsigned char *)pconnect_info->sz4GCloud, sizeof(pconnect_info->sz4GCloud));
            memcpy(crcBuf+crcLen, pconnect_info->sz4GCloud, strlen(pconnect_info->sz4GCloud));
            crcLen += strlen(pconnect_info->sz4GCloud);

            memcpy(connect_info.sz4GCloud, pconnect_info->sz4GCloud, sizeof(pconnect_info->sz4GCloud));
        }
        else
        {
            GetStringFromCfg(GG_CLOUD_LINENUM, (unsigned char *)connect_info.sz4GCloud, sizeof(connect_info.sz4GCloud));
            memcpy(crcBuf+crcLen, connect_info.sz4GCloud, strlen(connect_info.sz4GCloud));
            crcLen += strlen(connect_info.sz4GCloud);
        }
    }

    //计算CRC并写入配置中
    crcBuf[crcLen] = '\0';
    crc = crc32(0, crcBuf, crcLen);
    snprintf(crcString,sizeof(crcString), "%u", crc);
    SetStringToCfg(CRC_LINENUM, (const unsigned char *)crcString, sizeof(crcString));

    //把连接串、平台名、PUSH KEY组合起来并保存到分区中
    snprintf(P2PLinkCode, sizeof(P2PLinkCode), "%s@%s@%s#%s", connect_info.szConnectString, connect_info.szPlatformName, connect_info.szAuthWord, connect_info.sz4GCloud);

    Authorize_GetAuthDevInfo(&authinfo);

    memcpy(authinfo.szConnectString, P2PLinkCode, CONNECT_STRING_MAX_LEN);

    ret = Authorize_SetAuthDevInfo(&authinfo);

    AUTH_INF("Set P2PLinkCode [%s] End!\n", P2PLinkCode);

    return ret;
}

/*@breif Authorize_GetAuthDevInfo 获取授权信息
 *@return 成功返回0
 *@       失败返回-1
 *@note  获取授权信息，仅操作全局变量
 */
int Authorize_GetAuthDevInfo(stuShmAuthInfo *pAuthInfo)
{
    if (NULL == pAuthInfo)
    {
        AUTH_ERR("pAuthInfo Not NULL!!!\n");
        return -1;
    }

    memcpy(pAuthInfo, &gAuthInfo, sizeof(stuShmAuthInfo));
    return 0;
}

/*@breif Authorize_SetAuthDevInfo 设置授权信息
 *@return 成功返回0
 *@       失败返回-1
 *@note  设置(修改)授权，需要操作以下两步：
 *@      1. 修改全局变量gAuthInfo;
 *@      2. 修改DEVINFO分区;
 */
int Authorize_SetAuthDevInfo(const stuShmAuthInfo *pAuthInfo)
{
    int ret = 0;
    MAUTH_DEV_ATTR_S stMauthDevAttr = {0};

    if (NULL == pAuthInfo)
    {
        AUTH_ERR("pAuthInfo Not NULL!!!\n");
        return -1;
    }

	ret = MAuth_Dev_Open();
	if (ret < 0)
	{
		AUTH_ERR("MAuth Dev Open failed![ret=%#x]\n", ret);
        return -1;
	}
    else
    {
        /*更新到全局变量中*/
        memcpy(&gAuthInfo, pAuthInfo, sizeof(gAuthInfo));

        /*授权码*/
        memcpy(stMauthDevAttr.AuthCode, gAuthInfo.szAuthCode, sizeof(stMauthDevAttr.AuthCode));
        /*MAC 地址*/
        memcpy(stMauthDevAttr.MacAddr, gAuthInfo.szMac, sizeof(stMauthDevAttr.MacAddr));
        /*连接串*/
        memcpy(stMauthDevAttr.ConnectString, gAuthInfo.szConnectString, sizeof(stMauthDevAttr.ConnectString));
        /*序列号*/
        memcpy(stMauthDevAttr.SerialNum, gAuthInfo.szSerialNum, sizeof(stMauthDevAttr.SerialNum));
        /*DID信息*/
        memcpy(stMauthDevAttr.DIDInfo.szDid, gAuthInfo.struCloudInfo.szDid, sizeof(stMauthDevAttr.DIDInfo.szDid));
        stMauthDevAttr.DIDInfo.szLen = gAuthInfo.struCloudInfo.szLen;
        /*版型ID*/
        stMauthDevAttr.BoardID = gAuthInfo.dwBoardID;
        /*激活码*/
        stMauthDevAttr.Function = gAuthInfo.dwFuncion;


		/*版型名称*/
	    if (strlen(gBoardTypeName) == 0)
	    {
	        ret = GetBoardTypeFromCfg((unsigned char*)gBoardTypeName, sizeof(gBoardTypeName));
	        if (ret == 0)
	        {
	            memcpy(stMauthDevAttr.BoardTypeName, gBoardTypeName, sizeof(stMauthDevAttr.BoardTypeName));
	            AUTH_ERR("Auth BoardTypeName = %s\n", gBoardTypeName);
	        }
	    }
	    else
	    {
	        memcpy(stMauthDevAttr.BoardTypeName, gBoardTypeName, sizeof(stMauthDevAttr.BoardTypeName));
	    }
	    

        ret = MAuth_Dev_SetDevAttr(0, &stMauthDevAttr); 
        if (ret < 0)
        {
            AUTH_ERR("MAuth Dev SetDevAttr failed![ret=%#x]\n", ret);
        }

        ret = MAuth_Dev_Close(0);
        if (ret < 0)
        {
            AUTH_ERR("MAuth Dev Close failed![ret=%#x]\n", ret);
        }
    }

    return 0;
}


static int SetUsrDataInfoToCfg(const void *DataBuf, unsigned int DataSize)
{
    FILE *pfile = NULL;
    unsigned int bytes_written = 0;

    if (DataBuf == NULL || DataSize == 0)
    {
        return -1;
    }

    pfile = fopen(AUTH_USR_DATA_BACK_FILE, "wb");
    if (pfile == NULL)
    {
        AUTH_ERR("[Set] fopen [%s] failed!\n", AUTH_USR_DATA_BACK_FILE);
        return -2;
    }

    bytes_written = fwrite(DataBuf, 1, DataSize, pfile);
    if (bytes_written != DataSize)
    {
        SAFE_FCLOSE(pfile);
        return -3;
    }

    SAFE_FCLOSE(pfile);
    return 0;
}

/*@breif Auth_SetAuthP2pInfo 
 *@return 成功返回0
 *@      失败返回错误码
 */
int Auth_SetAuthP2pInfo(const StuAuthP2pInfo *pstP2pInfo)
{
    if ((NULL == pstP2pInfo))
    {
        AUTH_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    int nRet = 0;
    MAUTH_USR_DATA_S *pstMauthUsrData = NULL;
    MauthUsrData * pMauthUsrData = NULL;

	nRet = MAuth_Dev_Open();
	if (nRet < 0)
	{
		AUTH_ERR("MAuth Dev Open failed![ret=%#x]\n", nRet);
        return -1;
	}

    pstMauthUsrData = (MAUTH_USR_DATA_S *)malloc(sizeof(MAUTH_USR_DATA_S));
    if (NULL == pstMauthUsrData)
    {
        AUTH_ERR("malloc(sizeof(MAUTH_USR_DATA_S) failed!\n");
		nRet = MAuth_Dev_Close(0);
        if (nRet < 0)
        {
            AUTH_ERR("MAuth Dev Close failed![ret=%#x]\n", nRet);
        }
        return -2;
    }

    memset(pstMauthUsrData, 0, sizeof(MAUTH_USR_DATA_S));
	nRet = MAuth_Dev_GetUsrDat(MAUTH_DEFAULT_HANDLE, pstMauthUsrData);
	if (nRet < 0)
	{
		AUTH_ERR("MAuth_Dev_GetUsrDat failed!\n");
        SAFE_FREE(pstMauthUsrData);
		
		nRet = MAuth_Dev_Close(0);
        if (nRet < 0)
        {
            AUTH_ERR("MAuth Dev Close failed![ret=%#x]\n", nRet);
        }
		return -4;
	}

	pMauthUsrData = (MauthUsrData *)pstMauthUsrData->Data;
    if (memcmp(&(pMauthUsrData->AuthP2pInfo), pstP2pInfo, sizeof(StuAuthP2pInfo)) == 0)
    {
        AUTH_INF("[%s:%d] The Same KeepAliveData, Not Set!\n", __FUNCTION__, __LINE__);
        SAFE_FREE(pstMauthUsrData);
				
		nRet = MAuth_Dev_Close(0);
        if (nRet < 0)
        {
            AUTH_ERR("MAuth Dev Close failed![ret=%#x]\n", nRet);
        }
		
        return 0;
    }

	memcpy(&(pMauthUsrData->AuthP2pInfo), pstP2pInfo, sizeof(pMauthUsrData->AuthP2pInfo));
    pstMauthUsrData->Size = sizeof(MauthUsrData);

	nRet = MAuth_Dev_SetUsrDat(MAUTH_DEFAULT_HANDLE, pstMauthUsrData);
	if (nRet < 0)
	{
		AUTH_ERR("MAuth_Dev_SetUsrDat failed!\n");
        SAFE_FREE(pstMauthUsrData);
				
		nRet = MAuth_Dev_Close(0);
        if (nRet < 0)
        {
            AUTH_ERR("MAuth Dev Close failed![ret=%#x]\n", nRet);
        }
		return -5;
	}

    /* 备份至文件 */
    MauthUsrDataCfg stMauthUsrDataCfg;
    memset(&stMauthUsrDataCfg, 0, sizeof(MauthUsrDataCfg));

	memcpy(&(stMauthUsrDataCfg.nMauthUsrData), pMauthUsrData, sizeof(MauthUsrData));
    stMauthUsrDataCfg.Crc32 = MAuth_Crc32(0, (char *)&stMauthUsrDataCfg + sizeof(UINT32), MAUTH_USR_DATA_CFG_LEN - sizeof(UINT32));
    SetUsrDataInfoToCfg((void *)&stMauthUsrDataCfg, MAUTH_USR_DATA_CFG_LEN);
    AUTH_INF("SetUsrDataInfoToCfg Crc32[%u]\n", stMauthUsrDataCfg.Crc32);

    SAFE_FREE(pstMauthUsrData);
			
	nRet = MAuth_Dev_Close(0);
    if (nRet < 0)
    {
        AUTH_ERR("MAuth Dev Close failed![ret=%#x]\n", nRet);
    }
	
    return 0;
}



/*@breif Auth_GetAuthP2pInfo 
 *@return 成功返回0
 *@      失败返回-1
 */
int Auth_GetAuthP2pInfo(StuAuthP2pInfo *pstP2pInfo)
{
    if ((NULL == pstP2pInfo))
    {
        AUTH_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    int nRet = 0;

    MAUTH_USR_DATA_S * pstMauthUsrData = NULL;
    MauthUsrData * pMauthUsrData = NULL;

	nRet = MAuth_Dev_Open();
	if (nRet < 0)
	{
		AUTH_ERR("MAuth Dev Open failed![ret=%#x]\n", nRet);
        return -1;
	}

    pstMauthUsrData = (MAUTH_USR_DATA_S *)malloc(sizeof(MAUTH_USR_DATA_S));
    if (NULL == pstMauthUsrData)
    {
        AUTH_ERR("malloc(sizeof(MAUTH_USR_DATA_S) failed!\n");
		nRet = MAuth_Dev_Close(0);
        if (nRet < 0)
        {
            AUTH_ERR("MAuth Dev Close failed![ret=%#x]\n", nRet);
        }
		
        return -2;
    }

    memset(pstMauthUsrData, 0, sizeof(MAUTH_USR_DATA_S));
	nRet = MAuth_Dev_GetUsrDat(MAUTH_DEFAULT_HANDLE, pstMauthUsrData);
	if (nRet < 0)
	{
		AUTH_ERR("MAuth_Dev_GetUsrDat failed!\n");
        SAFE_FREE(pstMauthUsrData);
		
		nRet = MAuth_Dev_Close(0);
        if (nRet < 0)
        {
            AUTH_ERR("MAuth Dev Close failed![ret=%#x]\n", nRet);
        }
		return -4;
	}

	pMauthUsrData = (MauthUsrData *)pstMauthUsrData->Data;
    memcpy(pstP2pInfo, &(pMauthUsrData->AuthP2pInfo), sizeof(StuAuthP2pInfo));	

    SAFE_FREE(pstMauthUsrData);

	nRet = MAuth_Dev_Close(0);
    if (nRet < 0)
    {
        AUTH_ERR("MAuth Dev Close failed![ret=%#x]\n", nRet);
    }
	
    return 0;
}


