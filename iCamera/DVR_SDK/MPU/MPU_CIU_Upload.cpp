
/*!
*\file		MPU_CIU_Upload.cpp
*\brief	MPU模块，远程升级的实现
*\author	lbj
*\date		2009/08/13

功能：
-#	
-#	远程升级

说明：
-#	
*/
#include <sys/statfs.h>
#include "mysystem.h"
#include "MPU_CIU_global.h"
#include "MPU_CIU_Upload.h"
#include "ftp_update.h"
#include <fcntl.h>
#include <pthread.h>
#include <linux/kernel.h>
#include <linux/unistd.h>
#include <stdio.h>
#include <sys/mount.h>
#include "SYS/CBaseUpdate.h"
#include <sys/shm.h>
#include "common_pub.h"

#define UPLOAD_TO_DSP_MEM

#define CFG_FILE_FLAG	";******;"

static FileUploadParaInfo           s_UpdateState;

static int s_UpdateIsOver = 0;		//升级完成标记

static int s_iUpdateFileSize = 0;	// 已传输的大小

static int s_iUserCancel = 0;	// 取消升级标志

static Upload_Callback cbUploadCallBack = NULL;

//P2PShell Down(Up)load
static FileUploadParaInfo  s_DUAnyFileState;
static int s_iDUAnyFileSize = 0;	// 已传输的大小
static int P2PShellFlag = 0;

#if defined (__Hi3516CV500__) || defined (__Hi3516AV300__) 
static int g_UpgradeModeReturnStatus = 0;//0:进入升级模式成功,-1:进入升级模式失败,1:此时不能升级，需稍等再试
#endif

static int s_iFtpFileLen = 0;
#ifdef UPLOAD_TO_DSP_MEM
static unsigned int g_dspAddr=0;
static unsigned int g_dspPhyAddr=0;
static unsigned int g_dspLen=0;
static unsigned int g_writePoint=0;
static char g_ifUseDspMem=0;
#endif

pthread_mutex_t lock_remote_upload = PTHREAD_MUTEX_INITIALIZER;

/*!
*  @struct	tagSystemUpdateParam
*  @brief	系统升级参数
*/
int FtpCallback(int iBytes)
{
	MPU_DBG("FtpCallback %d, s_iFtpFileLen %d\n", iBytes, s_iFtpFileLen);
	(*cbUploadCallBack) (0, (int) eFileSystemUpdate, iBytes, s_iFtpFileLen, 0);
	return 0;
}

int umountPartition(char *mountPath)
{
	char command[128];
	int ret=0,n=0;
	
	if(NULL == mountPath)
	{
		printf("param error!\n");
		return -1;
	}

	mysystem("sync");

	memset(command, 0, sizeof(command));
	snprintf(command, sizeof(command), "fuser -k -m %s", mountPath);
	do
	{
		mysystem(command);
		sleep(1);
		ret = umount(mountPath);
		n++;
	}while(ret!=0 && n<=3);

	return ret;
}

SystemUpdateParam s_tFtpUpdateParam;

// 远程通知开启ftp升级
int CIU_FtpUpdateStart(LPSystemUpdateParam ptFtpUpdateParam)
{
	return 0;
}

void ftpTest()
{
	SystemUpdateParam tFtpUpdateParam;

	snprintf(tFtpUpdateParam.szFTPIP, sizeof(tFtpUpdateParam.szFTPIP), "192.168.1.109");
	tFtpUpdateParam.dwFTPPort = 21;
	snprintf(tFtpUpdateParam.szFTPUserName, sizeof(tFtpUpdateParam.szFTPUserName), "administrator");
	snprintf(tFtpUpdateParam.szFTPPassword, sizeof(tFtpUpdateParam.szFTPPassword), "zhuowei123ZHUOWEI");
	snprintf(tFtpUpdateParam.szFTPFilePath, sizeof(tFtpUpdateParam.szFTPFilePath), "/work");
	snprintf(tFtpUpdateParam.szFTPFileName, sizeof(tFtpUpdateParam.szFTPFileName), "mount_nfs.sh");

	CIU_FtpUpdateStart(&tFtpUpdateParam);

}

static int Upload_flag = 0;				//检测升级是否调用函数标志

/*
关于升级超时检测，当检测到升级调用creat函数但是
超过 UPLOAD_TIMEOUT_DETECTION 时间没有调用write
或者destory函数时，为了解决设备升级故障导致死机
直接重启设备
*/
static void* checkCallUploadWriteORDestroyFunctionThread(void* arg)
{
    PRT_TID();

	int TimeOut_Count = 0;
	int TimeOut_Limit = 0;
	int CurrNetCardType = 0;

	MPU_NET_GetCurrNetCardType(&CurrNetCardType);
	
	if (3 == CurrNetCardType)
	{
		//使用WIFI时，超时时间限制为10 min
		TimeOut_Limit = UPLOAD_TIMEOUT_DETECTION_WIFI;
	}
	else
	{
		TimeOut_Limit = UPLOAD_TIMEOUT_DETECTION;
	}
	
	pthread_detach(pthread_self());
	while(1)
	{
		TimeOut_Count++;
		sleep(1);
		if(1 == Upload_flag)
		{
			Upload_flag = 0;
			break;
		}
		if(TimeOut_Limit == TimeOut_Count)
		{
			MPU_PAR_SaveConfigReal();
			PAR_ForbiddenSaveFile();		
			MPU_ERR("checkCallUploadWriteORDestroyFunctionThread timeout!\n");
			RebootLinux(REBOOT_UPLOAD_TIMEOUT);
		}
	}
	return (void *)0;
}

int RemoteUploadCreate(HANDLE * hIndex, DWORD dwUserID, const LPFileUploadParaInfo pFileUploadParaInfo)
{
    //有上传下载命令字段
    if(eTransferFileTypeBegin == pFileUploadParaInfo->eFileType)
    {
        //开启了telnet标记、允许上传下载
        if(0 == access(P2PUDAnyFile,F_OK))
        {  
            //检查文件大小
    		if (0 >= pFileUploadParaInfo->dwFileTotalLen)
    		{
    			MPU_ERR("this file size is erro\n");

    			return -1;
    		}   
    		// 打开文件
    		char FilePath[200];
    		memcpy(FilePath, pFileUploadParaInfo->cFileName, sizeof(FilePath));

            //不可操作的目录
            if((NULL != strstr(FilePath, P2PUDAnyApp)) || (NULL != strstr(FilePath, P2PUDAnyWeb)) || (NULL != strstr(FilePath, P2PUDAnyLib)))
            {
                MPU_ERR("[xyx up creat] Warning! This path does not support uploading\n");
                return -1;
            }
            
    		int fdFile = open(FilePath, O_RDWR | O_CREAT | O_TRUNC);

    		if (fdFile < 0)
    		{
    			MPU_ERR("[xyx creat] Open fdFile %s Filed\n", FilePath);
    			return IDVR_ERROR_OPEN_FILE;
    		}

    		s_iDUAnyFileSize = -1;
            P2PShellFlag = 1;
    		*((int *) hIndex) = fdFile;

    		memcpy(&s_DUAnyFileState, pFileUploadParaInfo, sizeof (s_DUAnyFileState));
    		return 0;            
        }
        else
        {   
            //未开启Telnet
            MPU_ERR("[xyx up creat] not openTelnet\n");
            return -1;
        }        
    }

    WriteStateToFile(UPGRADE_UNSTARTED);
    
//	MPU_DBG("RemoteUploadCreate fileName %s dwFileTotalLen %lu type %d\n", pFileUploadParaInfo->cFileName, pFileUploadParaInfo->dwFileTotalLen, pFileUploadParaInfo->eFileType);
	// 检查权限
	const char *pcUserName = NULL;
	const char *pcTime = NULL;
	const char *pcIpAddr = NULL;
	char szCmd[256] = {0};

	pthread_t Upload_thrdID = 0;			//检测升级超时线程ID
	int nRet = 0;
	UPGRADE_MODE enUpgradeMode = NOT_UPGRADE_MODE;
	char acFilePath[260] = {0};
	char command[128] = {0};

	HANDLE hUser = NULL;

	if (NULL == cbUploadCallBack)
	{
		MPU_ERR("cbUploadCallBack is NULL\n");
		return -1;
	}

    /* 删除之前的标记文件 */
    if (!access(DESTROY_UPLOAD_OVER_FLAG, F_OK))
    {
        remove(DESTROY_UPLOAD_OVER_FLAG);
    }

	if (eFileSystemUpdate == pFileUploadParaInfo->eFileType)	// 升级包
	{
        memset(command, 0,sizeof(command));
        snprintf(command, sizeof(command), "touch %s", UPDATE_CREATE_STATUS);
        mysystem(command);
		//获取当前升级模式
		MPU_PUB_GetCurrentUpgradeMode(&enUpgradeMode);
		//若当前不是空闲模式或云升级模式，返回错误
		if (NOT_UPGRADE_MODE != enUpgradeMode
			&& LOCAL_UPGRADE_MODE != enUpgradeMode)
		{
			MPU_ERR("There are other ways of upgrading\n");
            remove(UPDATE_CREATE_STATUS);
    		return -1;
		}
		#ifdef __SUPPORT_SD_CARD__
			MPU_AVI_StopRecordAndUinitMemRecord(0);
		#endif
		//标志进入本地升级模式
		MPU_PUB_SetCurrentUpgradeMode(LOCAL_UPGRADE_MODE);

        if (dwUserID != VALUE_INVALUE) // 如果不是后台用户-1，则需要检查是否有升级的权限
        {
    		if (0 == CIU_GetUserInfo(dwUserID, &hUser, &pcUserName, &pcTime, &pcIpAddr))
    		{
    			if (1 != MPU_USR_GetRemotePrivilege(hUser, REMOTE_UPDATE))	// 无权限
    			{
    				MPU_ERR("MPU_USR_GetRemotePrivilege no right\n");
					MPU_PUB_SetCurrentUpgradeMode(NOT_UPGRADE_MODE);
                    remove(UPDATE_CREATE_STATUS);
    				return -1;
    			}
    		}
    		else
    		{
    			MPU_ERR("CIU_GetUserInfo erro\n");
				//标志退出本地升级模式
				MPU_PUB_SetCurrentUpgradeMode(NOT_UPGRADE_MODE);
                remove(UPDATE_CREATE_STATUS);

    			return -1;
    		}
		}
		else
		{
		    /* CGI升级需要模拟一个IP地址 */
            pcIpAddr = gszCgiRemoteUpgradeIpAddr;
		}        

		//检查文件大小
		if (0 >= pFileUploadParaInfo->dwFileTotalLen)
		{
			MPU_ERR("this file size is erro\n");
			//标志退出本地升级模式
			MPU_PUB_SetCurrentUpgradeMode(NOT_UPGRADE_MODE);
            remove(UPDATE_CREATE_STATUS);
			return -1;
		}

		// 检查是否已有升级
		if (0 == (*cbUploadCallBack) (dwUserID, (int) eFileSystemUpdate, 0, pFileUploadParaInfo->dwFileTotalLen, pcIpAddr))
		{
			MPU_ERR("There is another update!\n");
            remove(UPDATE_CREATE_STATUS);
			return -1;
		}

		// 打开文件
		if(0 == access(UPDATEFILENAME, F_OK))
		{
			memset(command, 0,sizeof(command));
			snprintf(command, sizeof(command), "rm %s", UPDATEFILENAME);
			mysystem(command);
		}
	#ifdef UPLOAD_TO_DSP_MEM
		if (0 == access(DSP_ADDR_INFO, F_OK))
		{
			memset(command, 0,sizeof(command));
			snprintf(command, sizeof(command), "rm %s", DSP_ADDR_INFO);
			mysystem(command);
		}
		
		if (0 == access(DSP_ADDR_INFO_FOR_MMZ, F_OK))
		{
			memset(command, 0,sizeof(command));
			snprintf(command, sizeof(command), "rm %s", DSP_ADDR_INFO_FOR_MMZ);
			mysystem(command);
		}
	#endif
		snprintf(acFilePath, sizeof(acFilePath), "%s", UPDATEFILENAME);
		int fdUpdateFile = open(acFilePath, O_RDWR | O_CREAT | O_TRUNC);

		if (fdUpdateFile < 0)
		{
			MPU_ERR("Open fdUpdateFile %s Filed\n", acFilePath);

			(*cbUploadCallBack) (dwUserID, (int) eFileSystemUpdate, 0xffffffff, pFileUploadParaInfo->dwFileTotalLen, 0);
	
			//标志退出本地升级模式
			MPU_PUB_SetCurrentUpgradeMode(NOT_UPGRADE_MODE);
            remove(UPDATE_CREATE_STATUS);
			return IDVR_ERROR_OPEN_FILE;
		}

		s_iUpdateFileSize = 0;
		s_iUserCancel = 0;
		*((int *) hIndex) = fdUpdateFile;
		memcpy(&s_UpdateState, pFileUploadParaInfo, sizeof (s_UpdateState));

		snprintf(szCmd, sizeof(szCmd), "rm  %s %s", UPDATEFILENAME_SUCC, UPDATEFILENAME_SUCC_CLOUD_UPGRADE);
		nRet = mysystem(szCmd);
		if (nRet)
		{
			MPU_ERR("rm Error. szCmd = [%s]. code = %d.\n", szCmd, nRet);
		}

		g_writePoint = 0;
		g_ifUseDspMem = 0;

	#ifdef UPLOAD_TO_DSP_MEM
		/*采用MMZ内存升级*/
		//将缓存写入磁盘
		mysystem("sync");
		//释放缓存
		//mysystem("echo 3 > /proc/sys/vm/drop_caches");
		
		//进入升级模式，并获得MMZ内存块
		unsigned int u32PhyAddr = 0;
		void *pVirtAddr = NULL;
		unsigned int uMmzAllocSize = 0;
        int counter = 0;
		
		ZWSys_SetSpeechContent(SPEECH_UPGRADE_START);
        //循环查询，等待播放结束
        while(1)
        {
            sleep(1);
            if (0 == ZWSys_GetSpeechPlayingStatus())
            {
                break;
            }
            if (++counter > SPEECH_PLAY_TIME_OUT_SEC)
            {
                break;
            }
        }

		//usleep(2500000);//延时等待播报结束
		nRet = ZWSys_SwitchToUpgradeMode(1, &u32PhyAddr, &pVirtAddr, &uMmzAllocSize);
		if (nRet)
		{
			MPU_ERR("ZWSys_SwitchToUpgradeMode fail %d!.\n", nRet);
			(*cbUploadCallBack) (0, (int) s_UpdateState.eFileType, 0xffffffff, s_UpdateState.dwFileTotalLen, 0);
			//标志退出本地升级模式
			MPU_PUB_SetCurrentUpgradeMode(NOT_UPGRADE_MODE);
			ZWSys_SwitchToUpgradeMode(0, NULL, NULL, NULL);
            remove(UPDATE_CREATE_STATUS);
			return -1;
		}
		else
		{
			g_dspPhyAddr = (unsigned int)u32PhyAddr;
			g_dspAddr = (unsigned int)pVirtAddr;
			g_dspLen = (unsigned int)uMmzAllocSize;
			MPU_INFO("MMZ_mem addr=%#x phyAddr=%#x len=%#x\n", g_dspAddr, g_dspPhyAddr, g_dspLen);
		}
		
		usleep(100000);
		
		if (g_dspLen >= pFileUploadParaInfo->dwFileTotalLen)
		{
			char buff[64] = {0};
			FILE *file_new = NULL;
			
			file_new = fopen(DSP_ADDR_INFO_FOR_MMZ, "w");
			if (file_new == NULL)
			{
				MPU_ERR("open file failed [%s]\n", DSP_ADDR_INFO_FOR_MMZ);
				(*cbUploadCallBack) (0, (int) s_UpdateState.eFileType, 0xffffffff, s_UpdateState.dwFileTotalLen, 0);
				//标志退出本地升级模式
				MPU_PUB_SetCurrentUpgradeMode(NOT_UPGRADE_MODE);
				ZWSys_SwitchToUpgradeMode(0, NULL, NULL, NULL);
                remove(UPDATE_CREATE_STATUS);
				return -1;
			}
			
			memset(buff, 0, sizeof(buff));
			snprintf(buff, sizeof(buff), "%d:%d:%d", g_dspAddr, g_dspPhyAddr, g_dspLen);
			fprintf(file_new, "%s", buff);
			fclose(file_new);
			file_new = NULL;
			g_ifUseDspMem = 1;
	
			nRet = pthread_create(&Upload_thrdID, NULL, checkCallUploadWriteORDestroyFunctionThread, NULL);
			if (0 != nRet)
			{
				MPU_ERR("create checkCallUploadWriteORDestroyFunctionThread\n");
			}

            WriteStateToFile(UPGRADE_CHECKING);
            
			return 0;
		}
		else
		{
			MPU_ERR("MMZ mem is not enough to upgrade!.\n");
			(*cbUploadCallBack) (0, (int) s_UpdateState.eFileType, 0xffffffff, s_UpdateState.dwFileTotalLen, 0);
			//标志退出本地升级模式
			MPU_PUB_SetCurrentUpgradeMode(NOT_UPGRADE_MODE);
			ZWSys_SwitchToUpgradeMode(0, NULL, NULL, NULL);
            remove(UPDATE_CREATE_STATUS);
			return -1;
		}
        remove(UPDATE_CREATE_STATUS);

        WriteStateToFile(UPGRADE_CHECKING);

		return 0;
	#endif	
	}
	else if (eDeviceConfig == pFileUploadParaInfo->eFileType)	// 升级配置文件
	{
        if (dwUserID != VALUE_INVALUE) // 如果不是后台用户-1，则需要检查是否有升级的权限
        {
    		if (0 == CIU_GetUserInfo(dwUserID, &hUser, &pcUserName, &pcTime, &pcIpAddr))
    		{
    			if (1 != MPU_USR_GetRemotePrivilege(hUser, REMOTE_SETTINGS))	// 无权限
    			{
    				MPU_ERR("MPU_USR_GetRemotePrivilege no right\n");
    				return -1;
    			}
    		}
    		else
    		{
    			MPU_ERR("CIU_GetUserInfo erro\n");
    			return -1;
    		}
        }
		else
		{
		    /* CGI升级需要模拟一个IP地址 */
            pcIpAddr = gszCgiRemoteUpgradeIpAddr;
		}   

		//检查文件大小
		if (0 >= pFileUploadParaInfo->dwFileTotalLen)
		{
			MPU_ERR("this file size is erro\n");
			return -1;
		}

		// 打开文件
		char acFilePath[200];

		snprintf(acFilePath, sizeof(acFilePath), "%s", IMPORTFILENAME);
		int fdUpdateFile = open(acFilePath, O_RDWR | O_CREAT | O_TRUNC);

		if (fdUpdateFile < 0)
		{
			MPU_ERR("Open fdUpdateFile %s Filed\n", acFilePath);
			return IDVR_ERROR_OPEN_FILE;
		}

		// 检查是否已有升级
		if (0 == (*cbUploadCallBack) (dwUserID, (int) eDeviceConfig, 0, pFileUploadParaInfo->dwFileTotalLen, pcIpAddr))
		{
			MPU_ERR("There is another update!\n");
			close(fdUpdateFile);
			return -1;
		}

		s_iUpdateFileSize = 0;
		s_iUserCancel = 0;
		*((int *) hIndex) = fdUpdateFile;
		memcpy(&s_UpdateState, pFileUploadParaInfo, sizeof (s_UpdateState));
		return 0;
	}
	else if (ePtzProtocol == pFileUploadParaInfo->eFileType)	// 升级云台协议 
	{
        if (dwUserID != VALUE_INVALUE) // 如果不是后台用户-1，则需要检查是否有升级的权限
		{
	    
    		if (0 == CIU_GetUserInfo(dwUserID, &hUser, &pcUserName, &pcTime, &pcIpAddr))
    		{
    			if (1 != MPU_USR_GetRemotePrivilege(hUser, REMOTE_SETTINGS))	// 无权限
    			{
    				MPU_ERR("MPU_USR_GetRemotePrivilege no right\n");
    				return -1;
    			}
    		}
    		else
    		{
    			MPU_ERR("CIU_GetUserInfo erro\n");
    			return -1;
    		}
        }

		//检查文件大小
		if (0 >= pFileUploadParaInfo->dwFileTotalLen)
		{
			MPU_ERR("this file size is erro\n");
			return -1;
		}

		// 打开文件
		char acFilePath[512];

		snprintf(acFilePath, sizeof(acFilePath), "%s/%s", PTZ_PATH, pFileUploadParaInfo->cFileName);
		int fdUpdateFile = open(acFilePath, O_RDWR | O_CREAT | O_TRUNC);

		if (fdUpdateFile < 0)
		{
			MPU_ERR("Open fdUpdateFile %s Filed\n", acFilePath);
			return IDVR_ERROR_OPEN_FILE;
		}

		// 检查是否已有升级
		if (0 == (*cbUploadCallBack) (dwUserID, (int) ePtzProtocol, 0, pFileUploadParaInfo->dwFileTotalLen, acFilePath))
		{
			MPU_ERR("There is another update!\n");
			close(fdUpdateFile);
			return -1;
		}

		s_iUpdateFileSize = 0;
		s_iUserCancel = 0;
		*((int *) hIndex) = fdUpdateFile;
		memcpy(&s_UpdateState, pFileUploadParaInfo, sizeof (s_UpdateState));
		return 0;
	}
	else if(eIspColorConfig == pFileUploadParaInfo->eFileType)
	{
		if (0 == CIU_GetUserInfo(dwUserID, &hUser, &pcUserName, &pcTime, &pcIpAddr))
		{
			if (1 != MPU_USR_GetRemotePrivilege(hUser, REMOTE_UPDATE))	// 与远程升级同等权限
			{
				MPU_ERR("MPU_USR_GetRemotePrivilege no right\n");
				return -1;
			}
		}
		else
		{
			MPU_ERR("CIU_GetUserInfo erro\n");
			return -1;
		}


		//检查文件大小
		if (512 != pFileUploadParaInfo->dwFileTotalLen)//文件大小固定为512
		{
			MPU_ERR("this file size is erro\n");
			return -1;
		}

		// 打开文件
		char acFilePath[64];

		snprintf(acFilePath, sizeof(acFilePath), "%s", ISPTONECURVEFILENAME);
		int fdUpdateFile = open(acFilePath, O_RDWR | O_CREAT | O_TRUNC);

		if (fdUpdateFile < 0)
		{
			MPU_ERR("Open fdUpdateFile %s Filed\n", acFilePath);
			return IDVR_ERROR_OPEN_FILE;
		}

		// 检查是否已有升级
		if (0 == (*cbUploadCallBack) (dwUserID, (int) eIspColorConfig, 0, pFileUploadParaInfo->dwFileTotalLen, pcIpAddr))
		{
			MPU_ERR("There is another update!\n");
			close(fdUpdateFile);
			return -1;
		}

		s_iUpdateFileSize = 0;
		s_iUserCancel = 0;
		*((int *) hIndex) = fdUpdateFile;
		memcpy(&s_UpdateState, pFileUploadParaInfo, sizeof (s_UpdateState));
		return 0;
	}
#if defined (__IR_CUT_CONTROL_IMPROVE__)
	else if (eTransferFileTypeCustomConfigFileAll == pFileUploadParaInfo->eFileType)
#else
	else if (eTransferFileTypeConfigAndCap == pFileUploadParaInfo->eFileType)
#endif
	{
		if (dwUserID != VALUE_INVALUE) // 如果不是后台用户-1，则需要检查是否有升级的权限
		{
			if (0 == CIU_GetUserInfo(dwUserID, &hUser, &pcUserName, &pcTime, &pcIpAddr))
			{
				if (1 != MPU_USR_GetRemotePrivilege(hUser, REMOTE_SETTINGS))	// 无权限
				{
					MPU_ERR("MPU_USR_GetRemotePrivilege no right\n");
					return -1;
				}
			}
			else
			{
				MPU_ERR("CIU_GetUserInfo erro\n");
				return -1;
			}
		}
		//检查文件大小
		if (0 >= pFileUploadParaInfo->dwFileTotalLen)
		{
			MPU_ERR("this file size is erro\n");
			return -1;
		}
		// 打开文件
		char acFilePath[200] = "";
		snprintf(acFilePath, sizeof(acFilePath), "%s", CUSTOM_MODE_FILE);
		int fdUpdateFile = open(acFilePath, O_RDWR | O_CREAT | O_TRUNC);

		if (fdUpdateFile < 0)
		{
			MPU_ERR("Open fdUpdateFile %s Filed\n", acFilePath);
			return IDVR_ERROR_OPEN_FILE;
		}
		// 检查是否已有升级
		if (eTransferFileTypeConfigAndCap == pFileUploadParaInfo->eFileType)
        {      
    		if (0 == (*cbUploadCallBack) (dwUserID, (int) eTransferFileTypeConfigAndCap, 0, pFileUploadParaInfo->dwFileTotalLen, acFilePath))
    		{
    			MPU_ERR("There is another update!\n");
    			close(fdUpdateFile);
    			return -1;
    		}
        }
        
		if (eTransferFileTypeCustomConfigFileAll == pFileUploadParaInfo->eFileType)
        {      
    		if (0 == (*cbUploadCallBack) (dwUserID, (int) eTransferFileTypeCustomConfigFileAll, 0, pFileUploadParaInfo->dwFileTotalLen, acFilePath))
    		{
    			MPU_ERR("There is another update!\n");
    			close(fdUpdateFile);
    			return -1;
    		}
        }
        
		s_iUpdateFileSize = 0;
		s_iUserCancel = 0;
		*((int *) hIndex) = fdUpdateFile;
		memcpy(&s_UpdateState, pFileUploadParaInfo, sizeof (s_UpdateState));
		return 0;
		
	}
#if defined (__USE_APPLY_DID_MANUAL__)
    else if(eTransferFileTypeSetDidCfgManul == pFileUploadParaInfo->eFileType)
    {
        //检查文件大小
        if (0 >= pFileUploadParaInfo->dwFileTotalLen)
        {
            MPU_ERR("this file size is erro\n");

            return -1;
        }  
        
        // 打开文件
        char FilePath[64];
        snprintf(FilePath, sizeof(FilePath), "%s", APPLYDIDSERVERINFOTMP);
        
        int fdFile = open(FilePath, O_RDWR | O_CREAT | O_TRUNC);

        if (fdFile < 0)
        {
            MPU_ERR("Open fdFile %s Filed\n", FilePath);
            return IDVR_ERROR_OPEN_FILE;
        }

        s_iDUAnyFileSize = -1;
        *((int *) hIndex) = fdFile;

        memcpy(&s_DUAnyFileState, pFileUploadParaInfo, sizeof (s_DUAnyFileState));
        return 0;            
                
    }
#endif
	else if(eTransferFileTypeCustomFileUpload == pFileUploadParaInfo->eFileType)
	{
		if (dwUserID != VALUE_INVALUE) // 如果不是后台用户-1，则需要检查是否有升级的权限
		{
			if (0 == CIU_GetUserInfo(dwUserID, &hUser, &pcUserName, &pcTime, &pcIpAddr))
			{
				if (1 != MPU_USR_GetRemotePrivilege(hUser, REMOTE_SETTINGS))	// 无权限
				{
					MPU_ERR("MPU_USR_GetRemotePrivilege no right\n");
					return -1;
				}
			}
			else
			{
				MPU_ERR("CIU_GetUserInfo erro\n");
				return -1;
			}
		}
		//检查文件大小
		if (0 >= pFileUploadParaInfo->dwFileTotalLen)
		{
			MPU_ERR("this file size is erro\n");
			return -1;
		}
		// 打开文件
		char acFilePath[200] = "";
		snprintf(acFilePath, sizeof(acFilePath), "%s", CUSTOM_MODE_FILE);

		//清空之前把数据存储起来，详细参数配置时需要用到
        memset(command, 0, sizeof(command));
		snprintf(command, sizeof(command), "cp %s %s", CUSTOM_MODE_FILE, CUSTOM_MODE_FILE_TMP);
		mysystem(command);
		
		int fdUpdateFile = open(acFilePath, O_RDWR | O_CREAT | O_TRUNC);

		if (fdUpdateFile < 0)
		{
			MPU_ERR("Open fdUpdateFile %s Filed\n", acFilePath);
			return IDVR_ERROR_OPEN_FILE;
		}

		// 检查是否已有升级
		if (0 == (*cbUploadCallBack) (dwUserID, (int) eTransferFileTypeCustomFileUpload, 0, pFileUploadParaInfo->dwFileTotalLen, acFilePath))
		{
			MPU_ERR("There is another update!\n");
			close(fdUpdateFile);
			return -1;
		}
		s_iUpdateFileSize = -1;
		s_iUserCancel = 0;
		*((int *) hIndex) = fdUpdateFile;
		memcpy(&s_UpdateState, pFileUploadParaInfo, sizeof (s_UpdateState));
		return 0;

	}
	else if(eTransferFileTypeAudioData == pFileUploadParaInfo->eFileType)
	{
		if (dwUserID != VALUE_INVALUE) // 如果不是后台用户-1，则需要检查是否有升级的权限
		{
			if (0 == CIU_GetUserInfo(dwUserID, &hUser, &pcUserName, &pcTime, &pcIpAddr))
			{
				if (1 != MPU_USR_GetRemotePrivilege(hUser, REMOTE_SETTINGS))	// 无权限
				{
					MPU_ERR("MPU_USR_GetRemotePrivilege no right\n");
					return -1;
				}
			}
			else
			{
				MPU_ERR("CIU_GetUserInfo erro\n");
				return -1;
			}
		}
		//检查文件大小
		if (0 >= pFileUploadParaInfo->dwFileTotalLen)
		{
			MPU_ERR("this file size is erro\n");
			return -1;
		}
		// 打开文件
		char acFilePath[200] = "";
		snprintf(acFilePath, sizeof(acFilePath), "%s", "/mnt/mtd/config/motionCustomAlarm.g711u");
		int fdUpdateFile = open(acFilePath, O_RDWR | O_CREAT | O_TRUNC);
		if (fdUpdateFile < 0)
		{
			MPU_ERR("Open fdUpdateFile %s Filed\n", acFilePath);
			return IDVR_ERROR_OPEN_FILE;
		}

		s_iUpdateFileSize = -1;
		s_iUserCancel = 0;
		*((int *) hIndex) = fdUpdateFile;
		memcpy(&s_UpdateState, pFileUploadParaInfo, sizeof (s_UpdateState));
		return 0;

	}
#if defined (__SUPPORT_PRODUCT_TEST_CUSTOM_SPEECH_FILES__)
	else if(eTransferFileTypeAllAudioData == pFileUploadParaInfo->eFileType)
	{
		if (dwUserID != VALUE_INVALUE) // 如果不是后台用户-1，则需要检查是否有升级的权限
		{
			if (0 == CIU_GetUserInfo(dwUserID, &hUser, &pcUserName, &pcTime, &pcIpAddr))
			{
				if (1 != MPU_USR_GetRemotePrivilege(hUser, REMOTE_SETTINGS))	// 无权限
				{
					MPU_ERR("MPU_USR_GetRemotePrivilege no right\n");
					return -1;
				}
			}
			else
			{
				MPU_ERR("CIU_GetUserInfo erro\n");
				return -1;
			}
		}

		DWORD fileSize = 134 * 1024 * 1.3 ;//base64文件比原文件大约大0.3,数值为大约数值比实际可能大一点。
		//检查文件大小
		if (0 >= pFileUploadParaInfo->dwFileTotalLen || pFileUploadParaInfo->dwFileTotalLen >= fileSize)
		{
			MPU_ERR("this file size is erro\n");
			return -1;
		}
		// 打开文件
		char acFilePath[200] = "";
		snprintf(acFilePath, sizeof(acFilePath), "%s", CUSTOM_ALL_SPEECH_TAR_TMP_PATH);
		int fdUpdateFile = open(acFilePath, O_RDWR | O_CREAT | O_TRUNC);
		if (fdUpdateFile < 0)
		{
			MPU_ERR("Open fdUpdateFile %s Filed\n", acFilePath);
			return IDVR_ERROR_OPEN_FILE;
		}

		// 检查是否已有升级
		if (0 == (*cbUploadCallBack) (dwUserID, (int) eTransferFileTypeAllAudioData, 0, pFileUploadParaInfo->dwFileTotalLen, acFilePath))
		{
			MPU_ERR("There is another update!\n");
			close(fdUpdateFile);
			return -1;
		}

		s_iUpdateFileSize = 0;
		s_iUserCancel = 0;
		*((int *) hIndex) = fdUpdateFile;
		memcpy(&s_UpdateState, pFileUploadParaInfo, sizeof (s_UpdateState));

		return 0;
	}
#endif

	return -1;
}

int RemoteUploadDestroy(HANDLE hIndex, const LPFileUploadState pState)
{
	char command[64] = {0};

	pthread_mutex_lock(&lock_remote_upload);
//	MPU_DBG("RemoteUploadDestroy s_iUpdateFileSize %d, dwFileTotalLen %lu\n", s_iUpdateFileSize, s_UpdateState.dwFileTotalLen);
    if(1 == P2PShellFlag)
    {
    	if (hIndex > 0)
    		close((int) hIndex);
    	if ((s_iDUAnyFileSize == (int) s_DUAnyFileState.dwFileTotalLen) && (s_iDUAnyFileSize > 0))	// 成功
    	{
    		MPU_INFO("translate success and destroy\n");
    	}
    	else
    	{
    		;
    	}

        P2PShellFlag = 0;
    	pthread_mutex_unlock(&lock_remote_upload);

        return 0;
    }	
	if (hIndex > 0)
		close((int) hIndex);

#ifdef UPLOAD_TO_DSP_MEM
	int iRet=0;

	if(g_ifUseDspMem)
	{
		if((DWORD)s_iUpdateFileSize != s_UpdateState.dwFileTotalLen && 0 == s_UpdateIsOver)//第二次多传的一个字节会导致重启，这里要判断是否升级过了
		{
			/*iRet = ZWSys_EnableDspMemToUpgrade(ZWSys_EncChannelOpen(0), &g_dspAddr, &g_dspLen, 0);
			if (iRet)
			{
				MPU_ERR("ZWSys_EnableDspMemToUpgrade fail %d!.\n", iRet);
			}*/
#if defined (__Hi3516CV500__) || defined (__Hi3516AV300__)
            if(1 == g_UpgradeModeReturnStatus)
            {
                ;
            }
            else
#endif
            {
    			MPU_INFO("Need reboot IPC when DSP mem used and user cancel!\n", iRet);
    			MPU_PAR_SaveConfigReal();
    			PAR_ForbiddenSaveFile();
                
    			mysystem("reboot");
    			mysystem("killall myWatchDogProcess");

    			//标志退出本地升级模式
    			MPU_PUB_SetCurrentUpgradeMode(NOT_UPGRADE_MODE);
			}
		}
		g_ifUseDspMem = 0;
		g_dspAddr = 0;
		g_dspPhyAddr = 0;
		g_dspLen = 0;
	}

#endif
	if ((s_iUpdateFileSize == (int) s_UpdateState.dwFileTotalLen) && (s_iUpdateFileSize > 0))	// 成功
	{
		
	}
	else if (1 == s_UpdateIsOver)//升级过一次了也表示成功
	{
		s_UpdateIsOver = 0;
	}
	else
	{
		//失败
		(*cbUploadCallBack) (0, (int) eFileSystemUpdate, 0xffffffff, s_UpdateState.dwFileTotalLen, 0);

		//标志退出本地升级模式
		MPU_PUB_SetCurrentUpgradeMode(NOT_UPGRADE_MODE);
	}

	if (s_UpdateState.eFileType == eIspColorConfig)
	{
		memset(command, 0,sizeof(command));
		snprintf(command, sizeof(command), "rm %s", ISPTONECURVEFILENAME);
		//mysystem(command);
	}
	else if (s_UpdateState.eFileType == eFileSystemUpdate)
	{
		memset(command, 0,sizeof(command));
		snprintf(command, sizeof(command), "rm %s", UPDATEFILENAME);
		//mysystem(command);
	}

    /* 如若网络层调用Destroy函数，表明升级文件下载完成且文件头校验成功 */
    memset(command, 0,sizeof(command));
    snprintf(command, sizeof(command), "touch %s", DESTROY_UPLOAD_OVER_FLAG);
    mysystem(command);

    printf("Called %s over!\n", __func__);

	pthread_mutex_unlock(&lock_remote_upload);
	return 0;
}

// 返回0表示成功, 1表示写失败,2表示取消,3表示flash已经满,4表示文件版本不对
int RemoteUploadWrite(HANDLE hIndex, Buffer * pBuffer)
{
	int iRet = 0;
	static unsigned long sRevivedLen = 0;

	int iRetCallBack = 0;
	static int s_IfCustomDetailPara = 0;//判断是不是使用产测工具定制详细参数
	static int s_GeneralCfgWriteNum = 0;//1：导入通用配置第一次传输数据时，去掉头部因产测工具定制详细参数加的FUNCTIONTYPE
	char command[128] = {0};

	char buff[1024] = {0};
	char head_buff[sizeof(FUNCTIONTYPE) + 1] = {0};
	static int s_SectionNum = 0;//找到INI文件中的分隔符
	static int s_SerialNum = 0;//传输次数,上传数据分为多次传输,第一次传输时去掉头部加的FUNCTIONTYPE
	static int index = 0;//原始INI文件的位置
	static int s_WriteSize = 0;//上层传递详细参数文件数据的大小
	FILE *fd = NULL;

	// 使用Web升级操作时协议最后多传了一个无用字节，导致升级崩溃，这里做规避，最后一个字节直接丢掉
	if (eFileSystemUpdate == s_UpdateState.eFileType)
	{
		if (sRevivedLen == s_UpdateState.dwFileTotalLen && 1 == pBuffer->dwBufLen)
		{
			return 0;
		}
		
		sRevivedLen += pBuffer->dwBufLen;
	}

	pthread_mutex_lock(&lock_remote_upload);
	MPU_DBG(" pBuffer->dwBufLen %d ;write %d; total %lu\n", (int) pBuffer->dwBufLen, iRet, s_UpdateState.dwFileTotalLen);

	MPU_CHK((NULL != hIndex) && (NULL != pBuffer));

	if(1 == P2PShellFlag)
    {
		if (0 == pBuffer->dwBufLen)
		{
			s_iDUAnyFileSize++;
			MPU_DBG("transover s_iDUAnyFileSize %d, dwFileTotalLen %lu\n",
					s_iDUAnyFileSize, s_DUAnyFileState.dwFileTotalLen);

			pthread_mutex_unlock(&lock_remote_upload);
			return 0;
		}

		int retV = write((int) hIndex, pBuffer->pBuffer, pBuffer->dwBufLen);
		if (0 > retV)
		{
			MPU_ERR("[xyx up write] RemoteUploadWrite  write erro %d\n", retV);

            if (hIndex > 0)
    		    close((int) hIndex);
			pthread_mutex_unlock(&lock_remote_upload);

			return 1;
		}
		else
		{
			s_iDUAnyFileSize += retV;
			if (retV != 0)
				MPU_DBG("[xyx up upWrite] retV = %d    %d  %d\n", retV, s_iDUAnyFileSize, s_DUAnyFileState.dwFileTotalLen);
			pthread_mutex_unlock(&lock_remote_upload);
			return 0;
		}	        
    }

	if(0 == s_IfCustomDetailPara && eTransferFileTypeCustomFileUpload == s_UpdateState.eFileType)
	{
		memcpy(head_buff, (char *)pBuffer->pBuffer, strlen(FUNCTIONTYPE));
	}
	
	if(pBuffer->dwBufLen >= strlen(FUNCTIONTYPE))
	{
		if(0 == strncmp(head_buff, FUNCTIONTYPE, sizeof(FUNCTIONTYPE)) && eTransferFileTypeCustomFileUpload == s_UpdateState.eFileType)
		{
			s_IfCustomDetailPara = 1;
			s_UpdateState.dwFileTotalLen -= strlen(FUNCTIONTYPE);//上传总长度去掉头部的FUNCTIONTYPE
		}
	}

	if (0 != s_iUserCancel && eFileSystemUpdate == s_UpdateState.eFileType)	// 用户取消升级
	{
		MPU_DBG("UserCancel\n");
		if(hIndex)
			close((int) hIndex);

		#ifdef UPLOAD_TO_DSP_MEM
		if(g_ifUseDspMem)
		{
			/*iRet = ZWSys_EnableDspMemToUpgrade(ZWSys_EncChannelOpen(0), &g_dspAddr, &g_dspLen, 0);
			if (iRet)
			{
				MPU_ERR("ZWSys_EnableDspMemToUpgrade fail %d!.\n", iRet);
			}*/
			MPU_INFO("Need reboot IPC when DSP mem used and user cancel!\n", iRet);
			MPU_PAR_SaveConfigReal();
			PAR_ForbiddenSaveFile();            
			mysystem("reboot");
			mysystem("killall myWatchDogProcess");
			g_ifUseDspMem = 0;
			g_dspAddr = 0;
			g_dspPhyAddr = 0;
			g_dspLen = 0;
		}
		#endif
		pthread_mutex_unlock(&lock_remote_upload);

		return 2;
	}
	else if(1 == s_IfCustomDetailPara)//如果是产测工具定制并且定制详细参数
	{
        /*只读模式下是无法使用 fseek 函数的*/
		fd = fopen(CUSTOM_MODE_FILE_TMP, "r+"); 
        if(NULL == fd)
        {
            MPU_ERR("open %s faild!\n", CUSTOM_MODE_FILE_TMP);
            pthread_mutex_unlock(&lock_remote_upload);
		    return 1;
        }
        
        fflush(fd);
		if(0 == s_SectionNum)//第一段内容
		{
			fseek(fd, 0, SEEK_SET);
            memset(buff, 0,sizeof(buff));
			while(fgets(buff, sizeof(buff), fd) != NULL)
			{
				index += strlen(buff);

				write((int) hIndex, buff, strlen(buff));

				if(0 == strncmp(buff, CFG_FILE_FLAG, strlen(CFG_FILE_FLAG)))//第一个分割符
				{
					s_SectionNum++;
					break;
				}
                memset(buff, 0,sizeof(buff));
			}
		}

		if(1 == s_SectionNum)//第二段内容
		{
			if(0 == s_SerialNum)//第一次传输去掉头部的FUNCTIONTYPE
			{
				iRet = write((int) hIndex, pBuffer->pBuffer + strlen(FUNCTIONTYPE), pBuffer->dwBufLen - strlen(FUNCTIONTYPE));//将详细参数写到第二段 实际下发数据的长度(跳过功能类型)
				s_SerialNum++;
				s_WriteSize += iRet;
			}
			else
			{
				iRet = write((int) hIndex, pBuffer->pBuffer, pBuffer->dwBufLen);//将详细参数写到第二段 实际下发数据的长度(跳过功能类型)
				s_WriteSize += iRet;
			}
		}

		if(1 == s_SerialNum && 1 == s_SectionNum &&  s_UpdateState.dwFileTotalLen == (DWORD)s_WriteSize)//第二段内容write完后，找到原始文件第二段内容结束的位置
		{
			fseek(fd, index, SEEK_SET);
			while(fgets(buff, sizeof(buff), fd) != NULL)
			{
				index += strlen(buff);

				if(0 == strncmp(buff, CFG_FILE_FLAG, strlen(CFG_FILE_FLAG)))//找到第二个分割符并写到INI文件中
				{
					s_SectionNum++;
					write((int) hIndex, "\n", 1);//加一个换行，避免INI文件分成四个文件时后面两个文件没有内容
					write((int) hIndex, buff, strlen(buff));//补一个分隔符
					break;
				}
			}
		}

		//写第三、四段的内容
		if(2 == s_SectionNum)
		{
			fseek(fd, index, SEEK_SET);
			while(fgets(buff, sizeof(buff), fd) != NULL)
			{
				write((int) hIndex, buff, strlen(buff));
			}
			s_SectionNum++;//保证三、四段内容只写一次
		}
			
		//处理产测工具两个详细参数配置的差异
		//http的数据是一次传输，传完之后，再传一个长度为1的数据，表示传输结束
		//SDK的数据是分多次传输，最后再传一个数据长度为0的数据
		if(3 == s_SectionNum && (1 == pBuffer->dwBufLen || 0 == pBuffer->dwBufLen))
		{
			if(1 == pBuffer->dwBufLen)
			{
				iRet = write((int) hIndex, pBuffer->pBuffer, pBuffer->dwBufLen);
			}
			else if(0 == pBuffer->dwBufLen)
			{
				iRet = 1;//SDK时，传输长度+1，避免因传输总长度不够，导致后面文件INI不分
			}

			//清空
			s_SectionNum = 0;
			s_IfCustomDetailPara = 0;
			s_SerialNum = 0;
			s_WriteSize = 0;
			index = 0;

			snprintf(command, sizeof(command), "rm %s", CUSTOM_MODE_FILE_TMP);
			mysystem(command);
			close((int) hIndex);
		}

		fclose(fd);

		s_iUpdateFileSize += iRet;
		iRetCallBack = (*cbUploadCallBack) (0, (int) s_UpdateState.eFileType, s_iUpdateFileSize, s_UpdateState.dwFileTotalLen, 0);
		pthread_mutex_unlock(&lock_remote_upload);
		return iRetCallBack;
	}
	else
	{
		if(eDeviceConfig == s_UpdateState.eFileType && 0 == pBuffer->dwBufLen)
		{
			s_GeneralCfgWriteNum = 0;//最后一次传输长度为0的数据时还原
		}

		if (0 == pBuffer->dwBufLen)
		{
			int iRetCallBack = 0;
			s_iUpdateFileSize++;
			iRetCallBack = (*cbUploadCallBack) (0, (int) s_UpdateState.eFileType, s_iUpdateFileSize, s_UpdateState.dwFileTotalLen, 0);
			if (9 == iRetCallBack)//这里9表示多传的一个字节，这里建一个标记给destory用，避免重启
			{
				s_UpdateIsOver = 1;
				iRetCallBack = 0;//给协议的返回值依然是0
			}
			MPU_DBG("transover  return %d  s_iUpdateFileSize %d, dwFileTotalLen %lu\n",
					iRetCallBack, s_iUpdateFileSize, s_UpdateState.dwFileTotalLen);
			pthread_mutex_unlock(&lock_remote_upload);
			return iRetCallBack;
		}
#ifdef UPLOAD_TO_DSP_MEM
		if(g_ifUseDspMem)
		{
			if (g_writePoint >= g_dspLen)
			{
			}
			else
			{
				memcpy((char *)(g_dspAddr + g_writePoint), pBuffer->pBuffer, pBuffer->dwBufLen);
			}
			iRet = pBuffer->dwBufLen;
			g_writePoint += iRet;
		}
		else
		{
			if(eDeviceConfig == s_UpdateState.eFileType)
			{
				if(0 == s_GeneralCfgWriteNum)
				{
					s_UpdateState.dwFileTotalLen -= sizeof(FUNCTIONTYPE);//上传总长度去掉头部的FUNCTIONTYPE,多减一个换行符的长度
					iRet = write((int) hIndex, pBuffer->pBuffer+sizeof(FUNCTIONTYPE), pBuffer->dwBufLen-sizeof(FUNCTIONTYPE));
					s_GeneralCfgWriteNum = 1;
				}
				else
				{
					iRet = write((int) hIndex, pBuffer->pBuffer, pBuffer->dwBufLen);	
				}
			}
			else
			{
				iRet = write((int) hIndex, pBuffer->pBuffer, pBuffer->dwBufLen);
			}
		}
#else
		iRet = write((int) hIndex, pBuffer->pBuffer, pBuffer->dwBufLen);
#endif

		if (0 > iRet)
		{
			char command[64];
			MPU_ERR("RemoteUploadWrite  write erro %d\n", iRet);
			pthread_mutex_unlock(&lock_remote_upload);
			if (s_UpdateState.eFileType == eFileSystemUpdate)
			{
				memset(command, 0,sizeof(command));
				snprintf(command, sizeof(command), "rm %s", UPDATEFILENAME);
				mysystem(command);
			}
			return 1;
		}
		else
		{
			int iRetCallBack = 0;
			//MPU_DBG(" pBuffer->dwBufLen %d ;write %d; total %lu\n", (int) pBuffer->dwBufLen, iRet, s_UpdateState.dwFileTotalLen);

			s_iUpdateFileSize += iRet;
			if (iRet != 0)
			{
				iRetCallBack = (*cbUploadCallBack) (0, (int) s_UpdateState.eFileType, s_iUpdateFileSize, s_UpdateState.dwFileTotalLen, 0);
			}

			if (9 == iRetCallBack)//这里9表示多传的一个字节，这里建一个标记给destory用，避免重启
			{
				s_UpdateIsOver = 1;
				iRetCallBack = 0;//给协议的返回值依然是0
			}
			//MPU_DBG("iRetCallBack = %d    %d  %d\n", iRetCallBack, s_iUpdateFileSize, s_UpdateState.dwFileTotalLen);
			pthread_mutex_unlock(&lock_remote_upload);
			return iRetCallBack;
		}
	}

}

int CIU_StopRemoteUpdate()
{
	MPU_DBG("enter\n");
	int nBoardID;
	ZWSys_GetBoardInfo(&nBoardID);		

	s_iUserCancel = 1;
	(*cbUploadCallBack) (0, (int) eFileSystemUpdate, 0xffffffff, s_UpdateState.dwFileTotalLen, 0);
    MPU_PAR_SaveConfigReal();
    PAR_ForbiddenSaveFile();

	mysystem("sync");
	sleep(1);
	mysystem("reboot");

	//标志退出本地升级模式
	MPU_PUB_SetCurrentUpgradeMode(NOT_UPGRADE_MODE);

	return 0;
}

void CIU_SetRemoteUploadCallBack(Upload_Callback pRemoteUploadCallBack)
{
	cbUploadCallBack = pRemoteUploadCallBack;

	if (!pRemoteUploadCallBack)
	{
		MPU_ERR("pRemoteUploadCallBack is NULL\n");
	}
}

void CIU_RegisterUpload()
{
	int nSDKRet = 0;

	// 变量初始化
	memset(&s_UpdateState, 0, sizeof (s_UpdateState));

	// 设置远程升级
	RemoteUploadInterface tRemoteUploadInterface;

	tRemoteUploadInterface.ifRemoteUploadCreate = RemoteUploadCreate;
	tRemoteUploadInterface.ifRemoteUploadDestroy = RemoteUploadDestroy;
	tRemoteUploadInterface.ifRemoteUploadWrite = RemoteUploadWrite;
	nSDKRet = HieServer_RemoteUploadIf(&tRemoteUploadInterface);
	if (0 != nSDKRet)
	{
		MPU_ERR("HieServer_RemoteUploadIf() erro %d\n", nSDKRet);
	}

	MPU_DBG("HieServer_RemoteUploadIf() return %d\n", nSDKRet);
}

