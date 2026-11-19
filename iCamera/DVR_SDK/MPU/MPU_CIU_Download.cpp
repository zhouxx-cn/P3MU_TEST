
/*!
*\file		MPU_CIU_Download.cpp
*\brief	MPU模块，下载的实现
*\author	lbj
*\date		2009/08/13

功能：
-#	
-#	下载

说明：
-#	
*/


#include "MPU_CIU_global.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "tagLog.h"

static int tempImageFile=0;

static FileUploadParaInfo  s_DUAnyFileState;

int RemoteDownloadCreate(HANDLE * hIndex, DWORD dwUserID, const LPRemoteDownloadParaInfo pRemoteDownloadParaInfo)
{
	MPU_DBG("RemoteDownloadCreate speed %d,fileType %d,fileName %s,len %lu\n", 
		pRemoteDownloadParaInfo->eSpeed,pRemoteDownloadParaInfo->eFileType,pRemoteDownloadParaInfo->cFileName, pRemoteDownloadParaInfo->dwFileTotalLen);

	//清空
	memset(&s_DUAnyFileState, 0, sizeof (s_DUAnyFileState));

    //有上传下载命令字段
    if(eTransferFileTypeBegin == pRemoteDownloadParaInfo->eFileType)
    {
		//文件存在
		if(0 == access(pRemoteDownloadParaInfo->cFileName,F_OK))
		{
			char FilePath[260] = {0};
			
			memcpy(FilePath, pRemoteDownloadParaInfo->cFileName, sizeof(FilePath));
			
			int fdFile = open(FilePath, O_RDONLY);

			if (fdFile < 0)
			{
				MPU_ERR("Open fdDownloadFile %s Filed\n", FilePath);
				return IDVR_ERROR_OPEN_FILE;
			}

			struct stat buf;

			if (stat((const char *)FilePath, &buf) != -1)
			{
				MPU_DBG("File %d size = %d\n", fdFile, (int) buf.st_size);
			}

			*((int *) hIndex) = fdFile;  

			return IDVR_OK;
		}
		else
		{
			//无此文件
			MPU_DBG("[xyx down creat] no such file\n");
			return IDVR_ERROR_FILE_NOT_EXSIT;
		}
    }

    // 打开文件
	char acFilePath[512];
	char command[256] = {0};

	if (eDeviceConfig == pRemoteDownloadParaInfo->eFileType)	// 导出配置
	{
		snprintf(acFilePath, sizeof(acFilePath), IDVR_CURRENT_CONFIG_FILE_PATH);
		if (0 == CIU_CheckRemoteUser(dwUserID, REMOTE_SETTINGS))
		{
			MPU_ERR("no config right  :REMOTE_SETTINGS\n");
			return IDVR_CIU_NO_RIGHT;
		}
	}
	else if (ePtzProtocol == pRemoteDownloadParaInfo->eFileType)	// 导出云台协议
	{
		snprintf(acFilePath, sizeof(acFilePath), "/mnt/app/lib/ExtDll/%s", pRemoteDownloadParaInfo->cFileName);
		if (0 == CIU_CheckRemoteUser(dwUserID, REMOTE_SETTINGS))
		{
			MPU_ERR("no config right  :REMOTE_SETTINGS\n");
			return IDVR_CIU_NO_RIGHT;
		}
	}
	else if (ePicture == pRemoteDownloadParaInfo->eFileType)
	{
		snprintf(acFilePath, sizeof(acFilePath), "%s", pRemoteDownloadParaInfo->cFileName);
		tempImageFile = 1;
	}
	else if (eRecordSetFile == pRemoteDownloadParaInfo->eFileType)
	{
		snprintf(acFilePath, sizeof(acFilePath), "%s", pRemoteDownloadParaInfo->cFileName);
	}	
//#if defined (__IR_CUT_CONTROL_IMPROVE__)
	else if (eTransferFileTypeCustomConfigFileAll == pRemoteDownloadParaInfo->eFileType)
	{
		memset(command, 0, sizeof(command));
		snprintf(command, sizeof(command), "cp %s %s", CUSTOM_MODE_FILE, CUSTOM_MODE_WINDOS);
		mysystem(command);
		
		memset(command, 0, sizeof(command));
		snprintf(command, sizeof(command), "unix2dos %s", CUSTOM_MODE_WINDOS);
		mysystem(command);
					
		snprintf(acFilePath, sizeof(acFilePath), "%s", CUSTOM_MODE_WINDOS);
	}
//#else
	else if (eTransferFileTypeConfigAndCap == pRemoteDownloadParaInfo->eFileType)
//#endif
	{
		memset(command, 0, sizeof(command));
		snprintf(command, sizeof(command), "cp %s %s", CUSTOM_MODE_FILE, CUSTOM_MODE_WINDOS);
		mysystem(command);
		
		memset(command, 0, sizeof(command));
		snprintf(command, sizeof(command), "unix2dos %s", CUSTOM_MODE_WINDOS);
		mysystem(command);
					
		snprintf(acFilePath, sizeof(acFilePath), "%s", CUSTOM_MODE_WINDOS);
	}
#if defined (__USE_APPLY_DID_MANUAL__)
	else if (eTransferFileTypeSetDidCfgManul == pRemoteDownloadParaInfo->eFileType)
	{

		snprintf(acFilePath, sizeof(acFilePath), APPLYDIDSERVERINFOCFGBAK);
		if (0 == CIU_CheckRemoteUser(dwUserID, REMOTE_SETTINGS))
		{
			MPU_ERR("no config right  :REMOTE_SETTINGS\n");
			return IDVR_CIU_NO_RIGHT;
		}
	}
#endif
	else
	{
		MPU_ERR("DownloadFile type err %d \n", (int) pRemoteDownloadParaInfo->eFileType);
		return 1;
	}
	int fdDownloadFile = open(acFilePath, O_RDONLY);

	if (fdDownloadFile < 0)
	{
		MPU_ERR("Open fdDownloadFile %s Filed\n", acFilePath);
		return IDVR_ERROR_OPEN_FILE;
	}

	struct stat buf;

	if (stat((const char *)acFilePath, &buf) != -1)
	{
		MPU_DBG("File %d size = %d\n", fdDownloadFile, (int) buf.st_size);
	}

	//pRemoteDownloadParaInfo->dwFileTotalLen = buf.st_size;
	*((int *) hIndex) = fdDownloadFile;
	if (eDeviceConfig == pRemoteDownloadParaInfo->eFileType)
	{
		char acUerName[64];

		char acIPAddress[20];

		char acLoginTime[30];

		HANDLE hUser = NULL;

		DWORD dwIpaddr = 0;

		MPU_CIU_GetRemoteByID(dwUserID, &hUser, acUerName, sizeof(acUerName), acIPAddress, sizeof(acIPAddress), acLoginTime, sizeof(acLoginTime));
		CIU_TransIpFromStrToDw(acIPAddress, &dwIpaddr);
		MPU_OPL_AddLog(EDVR_EVENT_SETTING, EDVR_EVENT_SETTING_DETAIL_EXPORT_SETTINGS, 0, 0, dwIpaddr, acUerName);

		memcpy(&s_DUAnyFileState, pRemoteDownloadParaInfo, sizeof (s_DUAnyFileState));
	}
	else if (ePtzProtocol == pRemoteDownloadParaInfo->eFileType)
	{
		char acUerName[64];

		char acIPAddress[20];

		char acLoginTime[30];

		HANDLE hUser = NULL;

		DWORD dwIpaddr = 0;

		MPU_CIU_GetRemoteByID(dwUserID, &hUser, acUerName, sizeof(acUerName), acIPAddress, sizeof(acIPAddress), acLoginTime, sizeof(acLoginTime));
		CIU_TransIpFromStrToDw(acIPAddress, &dwIpaddr);
		MPU_OPL_AddLog(EDVR_EVENT_SETTING, EDVR_EVENT_SETTING_DETAIL_EXPORT_PTZ, 0, 0, dwIpaddr, acUerName);

	}
	return 0;
}

int RemoteDownloadDestroy(HANDLE hIndex, const LPRemoteDownloadState pState)
{
	char command[128];

	MPU_DBG("RemoteDownloadDestroy %lu %lu\n", pState->dwFileTransferLen, pState->dwFileTotalLen);
	if (hIndex > 0)
		close((int) hIndex);
	if(tempImageFile)
	{
		memset(command,0,sizeof(command));
		snprintf(command, sizeof(command), "rm %s",REMOTE_CAPTURE_IMAGE_FILE_NAME);
		mysystem(command);
		tempImageFile = 0;
	}

	return 0;
}

int RemoteDownloadRead(HANDLE hIndex, Buffer * pBuffer)
{
	MPU_DBG("RemoteDownloadRead\n");
	int iRet = 0;
	static int s_readNum = 0;//第一次手动添加FUNCTIONTYPE

	MPU_CHK((NULL != hIndex) && (NULL != pBuffer));

	if(eDeviceConfig == s_DUAnyFileState.eFileType)//通用配置文件走的是9
	{
		if(0 == s_readNum)
		{
			memcpy(pBuffer->pBuffer, FUNCTIONTYPE, strlen(FUNCTIONTYPE));
			iRet = read((int) hIndex, pBuffer->pBuffer+strlen(FUNCTIONTYPE), BUFFER_SIZE-strlen(FUNCTIONTYPE));
			s_readNum++;
		}
		else
		{
			iRet = read((int) hIndex, pBuffer->pBuffer, BUFFER_SIZE);
		}
		
	}
	else
	{
		iRet = read((int) hIndex, pBuffer->pBuffer, BUFFER_SIZE);
	}

	if (0 > iRet)
	{
		MPU_ERR("RemoteDownloadRead  read erro %d\n", iRet);
		return -1;
	}
	else if (iRet > 0)
	{
		pBuffer->dwBufLen = (DWORD) iRet;

		if (0 == memcmp(pBuffer->pBuffer, FUNCTIONTYPE, strlen(FUNCTIONTYPE)))
		{
			pBuffer->dwBufLen += strlen(FUNCTIONTYPE);
		}

		MPU_DBG(" file %d pBuffer->dwBufLen %d ;read %d\n", (int) hIndex, (int) pBuffer->dwBufLen, iRet);
		return 0;
	}
	else
	{
		s_readNum = 0;
		pBuffer->dwBufLen = iRet;
		MPU_DBG(" file trans over  %d pBuffer->dwBufLen %d ;read %d\n", (int) hIndex, (int) pBuffer->dwBufLen, iRet);
		return 1;
	}
}

void CIU_RegisterDownload()
{
	int nSDKRet = 0;

	// 变量初始化

	// 设置远程升级
	RemoteDownloadInterface tRemoteDownloadInterface;

	tRemoteDownloadInterface.ifRemoteDownloadCreate = RemoteDownloadCreate;
	tRemoteDownloadInterface.ifRemoteDownloadDestroy = RemoteDownloadDestroy;
	tRemoteDownloadInterface.ifRemoteDownloadRead = RemoteDownloadRead;
	nSDKRet = HieServer_RemoteDownloadIf(&tRemoteDownloadInterface);
	if (0 != nSDKRet)
	{
		MPU_ERR("HieServer_RemoteDownloadIf() erro %d\n", nSDKRet);
	}

	MPU_DBG("HieServer_RemoteDownloadIf() return %d\n", nSDKRet);
}
