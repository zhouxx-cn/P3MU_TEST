
/*!
*\file   CBaseUpdate.cpp 
*\brief  升级的基类声明
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <mntent.h>
#include <sys/vfs.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <dirent.h>
#include <sys/time.h>
#include <sys/types.h>
#include <pthread.h>
#include <errno.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mount.h>
#include <pthread.h>
#include <mtd/mtd-user.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "HANDLE.h"
#include "CBaseUpdate.h"
#include "debug.h"
#include "idvr_errno.h"
#include "zwsdk.h"
#include "CConfig.h"
#include "mysystem.h"
#include "md5Lib.h"
#include "Authority.h"
#include "PUB_BoradInfo.h"
#include "global_constant.h"
#include "global_FilePath.h"
#include "global_string.h"
extern "C" {
	#include "upgrade.h"
}

/**************************************************************************
*                           方法实现                                      *
 **************************************************************************/

/* added by liuk 20070704, for valid update packet version */

/**
 * \brief
 * verify update packet's name
 * \param IN _path
 * update packet's full path
 * \RETURN
 * 1 or 0
*/
static int check_filename(const char *_path)
{
	const char ext[] = ".bin";

	if (strlen(_path) <= strlen(ext))
		return 0;
	else
	{
		if (memcmp(_path + strlen(_path) - strlen(ext), ext, strlen(ext)) != 0)
			return 0;
		else
			return 1;
	}
}

//!\brief 将升级过程中的各个状态写到文件
//!\praram int state 状态值
//!\return  
void WriteStateToFile(int state)
{
    char fst[64];
    if (0 != access(UPDATE_STATE_FILE,F_OK))
    {
        memset(fst,0,sizeof(fst));
        snprintf(fst, sizeof (fst), "touch %s",UPDATE_STATE_FILE);
        mysystem(fst);
        
        memset(fst,0,sizeof(fst));
        snprintf(fst, sizeof (fst), "echo state=%d > %s",state,UPDATE_STATE_FILE);
        mysystem(fst);
    }else
    {
        memset(fst,0,sizeof(fst));
        snprintf(fst, sizeof (fst), "echo state=%d > %s",state,UPDATE_STATE_FILE);
        mysystem(fst);
    }

}

/**
 * \brief
 * verify update packet's validity
 * \param IN _path
 * update packet's full path
 * \RETURN
 * 1 or 0
*/

#define UPLOAD_TO_DSP_MEM

#ifdef UPLOAD_TO_DSP_MEM
static unsigned int g_dspAddr=0;
static unsigned int g_dspPhyAddr=0;
static unsigned int g_dspLen=0;
static unsigned int g_readPoint=0;
static unsigned int g_shareMemLen=0;
static unsigned int g_shareMemAddr=0;
static char g_ifUseDspMem=0;
#endif

typedef struct tageBoardT
{
	unsigned int  boardInfo;
	char ImageCode[64];
} BOARD_INFO;


BOARD_INFO boardInfoListHsab[MAX_BOARD_NUM] = {
};

BOARD_INFO boardInfoListCustom[MAX_BOARD_NUM] = {
};

enum
{
	IMAGE_TYPE_BOOT = 1,
	IMAGE_TYPE_ROMFS,
	IMAGE_TYPE_APP,
	IMAGE_TYPE_WEB,
	IMAGE_TYPE_CONFIG,
	IMAGE_TYPE_CUSTOM,
	IMAGE_TYPE_ONVIF,
};

typedef struct tageImageInfo
{
	unsigned int  ImageType;
	unsigned int  ImageOffset;
	unsigned int  ImageLen;
	unsigned char ImageCode[8];
}IMAGE_INFO;

typedef struct tagImageInfoHeader
{
	unsigned char ImageInfoMagicCode[8];
	unsigned int  ImageNum;
	IMAGE_INFO	  img_inf[IMAGE_TYPE_ONVIF];
}ImageInfoHeader;

static ImageInfoHeader g_iih;

#ifdef __UPGRADE_V2__
static int cmpVersion(unsigned int major, unsigned int minor, unsigned int rev, unsigned int date,
	unsigned int _major, unsigned int _minor, unsigned int _rev, unsigned int _date)
{
	if ((_major < major) 
		|| (_major == major && _minor < minor) 
		|| (_major == major && _minor == minor && _rev < rev)
		|| (_major == major && _minor == minor && _rev == rev && _date < date))
	{
		return 1;
	}
	
	return 0;
}

static int  getBaseVer(unsigned int *major, unsigned int *minor, unsigned int *reversion, unsigned int *date)
{
	int ret = 0;
	CConfig *cfg = CConfig::NewInstance();
	char tmp[64] = {0};
	unsigned int base_major = 0, base_minor = 0, base_reversion = 0, base_date = 0;
	unsigned int cur_major = 0, cur_minor = 0, cur_reversion = 0, cur_date = 0;

	ret = cfg->LoadFile(VERSION_FILE_NAME);
	if (0 != ret)
	{
		CSYS_ERR("cfg->LoadFile %s error\n",VERSION_FILE_NAME);
		cfg->ReleaseInstance(cfg);
		return -1;
	}

	// 设备当前版本
	cfg->GetValue("Software","Major_version","1", tmp, 32);
	cur_major = atoi(tmp);
	memset(tmp, 0, 64);
	cfg->GetValue("Software","Minor_version","0", tmp, 32);
	cur_minor = atoi(tmp);
	memset(tmp, 0, 64);
	cfg->GetValue("Software","Reversion","0", tmp, 32);
	cur_reversion = atoi(tmp);
	memset(tmp, 0, 64);
	cfg->GetValue("Software","BuildDate","20210515", tmp, 32);
	cur_date = atoi(tmp);
	memset(tmp, 0, 64);

	CSYS_INF("[UPGRADE_V2] CurVersion: %u.%u.%u-%u\n", cur_major, cur_minor, cur_reversion, cur_date);

	// 配置文件的基础版本
	cfg->GetValue("BaseSoftware","Major_version","0", tmp, 32);
	base_major = atoi(tmp);
	memset(tmp, 0, 64);
	cfg->GetValue("BaseSoftware","Minor_version","0", tmp, 32);
	base_minor = atoi(tmp);
	memset(tmp, 0, 64);
	cfg->GetValue("BaseSoftware","Reversion","0", tmp, 32);
	base_reversion = atoi(tmp);
	memset(tmp, 0, 64);
	cfg->GetValue("BaseSoftware","BuildDate","00000000", tmp, 32);
	base_date = atoi(tmp);
	memset(tmp, 0, 64);

	CSYS_INF("[UPGRADE_V2] BaseVersion: %u.%u.%u-%u\n", base_major, base_minor, base_reversion, base_date);

	cfg->ReleaseInstance(cfg);

	if (cmpVersion(base_major, base_minor, base_reversion, base_date, cur_major, cur_minor, cur_reversion, cur_date))
	{
		CSYS_INF("[UPGRADE_V2] Cur-version lower then Base-version, Use Cur-version!\n");

		*major = cur_major;
		*minor = cur_minor;
		*reversion = cur_reversion;
		*date = cur_date;
		
		return 0;
	}

	*major = base_major;
	*minor = base_minor;
	*reversion = base_reversion;
	*date = base_date;

	return 0;
}

static int checkVersion()
{
	int nRet = 0;
	unsigned int major = 0, minor = 0, reversion = 0, date = 0;
	unsigned int L_major = 0, L_minor = 0, L_reversion = 0,L_date = 0;
	unsigned int u_major = 0, u_minor = 0, u_reversion = 0, u_date = 0;
	unsigned int i = 0;
	int nBoardID = 0;
	char szDevType[64] = {0};
	UPGRADE_KEY_S stUpdateKey;
	UPGRADE_MEM_INFO_S stMemInfo;

	//获取基础版本
	getBaseVer(&major, &minor, &reversion, &date);
	
	//获取版型所对应的型号
	ZWSys_GetBoardInfo(&nBoardID); //GetDefaultBoardID 多个版型是同一个Sensor时，会默认获取到第一个版型。所以这里需要区分下

	for (i = 0; i < gstuBoardInfoSize; i++)
	{
		if((unsigned int)nBoardID == gstuBoardInfo[i].dwBoardId)
		{
			memset(szDevType, 0, sizeof(szDevType));
			snprintf(szDevType, sizeof(szDevType), "TYPE_%s", gstuBoardInfo[i].szBoardType);
			CSYS_INF("This board type is %s\n", szDevType);
			break;
		}
	}

	nRet = Upgrade_Open(NULL, szDevType);
	if (nRet < 0)
	{
		CSYS_ERR("Upgrade_Open failed, nRet=%d\n", nRet);
		return -1;
	}

	stMemInfo.Cnt = 1;
	stMemInfo.Mem[0].Addr = (void *)g_dspAddr;
	stMemInfo.Mem[0].Size = g_dspLen;
    
	nRet = Upgrade_Mem_CheckEx(0, &stMemInfo, &stUpdateKey);
	if (nRet < 0)
	{
		CSYS_ERR("Upgrade_Mem_CheckEx failed, nRet=%d\n", nRet);
		return -1;
	}

	CSYS_INF("updateInfo.Ver=%s\n", stUpdateKey.UpgradePackageVersion);

	Upgrade_Close(0);

	sscanf(stUpdateKey.UpgradePackageVersion, "V%u.%u.%u_%u", &u_major, &u_minor, &u_reversion, &u_date);

	if (cmpVersion(major, minor, reversion, date, u_major, u_minor, u_reversion, u_date))
	{
		CSYS_ERR("[UPGRADE_V2] Upgrade-version lower then Base-version!\n");
		return -1;
	}
	//增加新的sensor型号，为防止客户将新sensor板子错误升级老固件引发客诉，增加OTA升级版本限制
	
	if (cmpVersion(L_major, L_minor, L_reversion, L_date, u_major, u_minor, u_reversion, u_date))
	{
		CSYS_ERR("[UPGRADE_V2] Upgrade-version lower then Base-version!\n");
		return -1;
	}

	return 0;
}
#endif

int check_version(const char *_path)
{
	int fd = 0;

	CSYS_INF("update file name is %s !\n", _path);	
	
#ifdef UPLOAD_TO_DSP_MEM
	g_ifUseDspMem = 0;
	g_dspAddr = 0;
	g_dspPhyAddr = 0;
	g_dspLen = 0;
	g_readPoint = 0;
	g_shareMemAddr = 0;
	g_shareMemLen = 0;
	if(0 == access(DSP_ADDR_INFO, F_OK))
	{
		FILE *pfile;
		char fbuf[16] = { 0 };
		pfile = fopen(DSP_ADDR_INFO, "r");
		if(NULL == pfile)
		{
			CSYS_ERR("open %s faile!\n", DSP_ADDR_INFO);
			goto GET_UPGRADE_DATA_FROM_DSP_ERROR;
		}
		fread(fbuf, sizeof(char), sizeof(fbuf), pfile);
		sscanf(fbuf, "%d:%d", &g_dspAddr, &g_dspLen);
		// fscanf(pfile, "%d:%d", &g_dspAddr, &g_dspLen);
		fclose(pfile);
		if((0 == g_dspAddr) || (0 == g_dspLen))
		{
			CSYS_ERR("get DSP Addr error!\n");
			goto GET_UPGRADE_DATA_FROM_DSP_ERROR;
		}
		g_ifUseDspMem = 1;
		CSYS_INF("Get Dsp addr=%d, len=%d\n", g_dspAddr, g_dspLen);
	}
	else if (0 == access(DSP_ADDR_INFO_FOR_MMZ, F_OK))
	{
		FILE *pfile = NULL;
		char fbuf[128] = { 0 };
		pfile = fopen(DSP_ADDR_INFO_FOR_MMZ, "r");
		if (NULL == pfile)
		{
			CSYS_ERR("open %s faile!\n", DSP_ADDR_INFO_FOR_MMZ);
			goto GET_UPGRADE_DATA_FROM_DSP_ERROR;
		}

		fread(fbuf, sizeof(char), sizeof(fbuf), pfile);
		sscanf(fbuf, "%d:%d:%d", &g_dspAddr, &g_dspPhyAddr, &g_dspLen);
		// fscanf(pfile, "%d:%d:%d", &g_dspAddr, &g_dspPhyAddr, &g_dspLen);
		fclose(pfile);
		if ((0 == g_dspAddr) || (0 == g_dspLen))
		{
			CSYS_ERR("get DSP Addr error!\n");
			goto GET_UPGRADE_DATA_FROM_DSP_ERROR;
		}
		g_ifUseDspMem = 1;
		CSYS_INF("Get MMZ_mem addr=%#x phyAddr=%#x len=%#x\n", g_dspAddr, g_dspPhyAddr, g_dspLen);
	}
	else
	{
		if (!check_filename(_path))
		{
			CSYS_ERR("check_version <<< illegal update packet\n");

            WriteStateToFile(UPGRADE_CHECKING_FAIL);
	        sleep(5); 
            
			return -1;
		}
	}
#else
	if (!check_filename(_path))
	{
		CSYS_ERR("check_version <<< illegal update packet\n");

        WriteStateToFile(UPGRADE_CHECKING_FAIL);
        sleep(5); 

		return -1;
	}
#endif	
	memset(&g_iih, 0, sizeof(ImageInfoHeader));

#ifdef UPLOAD_TO_DSP_MEM
	if (g_ifUseDspMem)//如果使用了DSP内存
	{
		fd = g_dspAddr;
		if (0 == g_dspAddr)
		{
			goto GET_UPGRADE_DATA_FROM_DSP_ERROR;
		}
	}
	else
	{
		fd = open(_path, O_RDONLY);
		if (fd <= 0)
		{
			CSYS_ERR("open file %s fail!\n", _path);

            WriteStateToFile(UPGRADE_CHECKING_FAIL);
	        sleep(5); 
            
			return -1;
		}
	}
#else
	fd = open(_path, O_RDONLY);
	if (fd <= 0)
	{
		CSYS_ERR("open file %s fail!\n", _path);

        WriteStateToFile(UPGRADE_CHECKING_FAIL);
        sleep(5); 
        
		return -1;
	}
#endif

	if (0 != checkVersion())
	{
#ifdef UPLOAD_TO_DSP_MEM
		if(g_ifUseDspMem)
			goto GET_UPGRADE_DATA_FROM_DSP_ERROR;
#endif
		close(fd);

        WriteStateToFile(UPGRADE_CHECKING_FAIL);
        sleep(5);

		return -1;
	}

	CSYS_INF("update success!\n");
#ifdef UPLOAD_TO_DSP_MEM
	if(!g_ifUseDspMem)
		close(fd);
#else
	close(fd);
#endif

	return 1;

GET_UPGRADE_DATA_FROM_DSP_ERROR:
	CSYS_ERR("Check upgrade fail! will reboot\n");

    WriteStateToFile(UPGRADE_CHECKING_FAIL);
	sleep(5);

	mysystem("reboot");
	return -1;
}

CBaseUpdate::CBaseUpdate(char *Filter)
{
	m_funCallBack = NULL;
	bUpdate = 0;
	m_nCount = 0;	

	if (NULL != Filter)
	{
		snprintf(m_Filter, sizeof(m_Filter), "%s", Filter);
	}
	else
	{
		memset(m_Filter, 0, sizeof (m_Filter));
	}
}

CBaseUpdate::~CBaseUpdate()
{
	;
}

