#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "upgrade.h"

#define UPGRADE_BIN_FILE    "restore.bin"
#define SINGLE_BUFFER_SIZE  (8*1024*1024)

#define Hi3516CV610_W85T_BOARD_ID	    0x2159 //Hi3516CV610 W85T
#define Hi3516CV610_G85T_BOARD_ID	    0x216D //Hi3516CV608 G85T

#define MAX_BOARD_NUM 50

typedef struct tageBoardT
{
	unsigned int  boardInfo;
	char ImageCode[64];
} BOARD_INFO;


#if defined(BOARD_G85T)
	static int m_BoardInfo = Hi3516CV610_G85T_BOARD_ID;
#else if defined(BOARD_W85T)
	static int m_BoardInfo = Hi3516CV610_W85T_BOARD_ID;
#endif



BOARD_INFO boardInfoList[MAX_BOARD_NUM] = {
    {0x216D, "TYPE_G85T-50PHM-S/G"},        //Hi3516CV610+JXF38P
    {0x2159, "TYPE_W85T-50PHM-S/W"},        //Hi3516CV610+JXF38P
};

void Upgrade_Callback(SINT32 wParam, SINT32* plParam)
{
    printf("Process = %d\n", wParam);
}

static int mysystem(const char *cmdstring)
{
	pid_t pid;

	int status;

	if (cmdstring == NULL)
		return 1;
	if ((pid = vfork()) < 0)
		status = -1;
	else if (pid == 0)
	{
		execl("/bin/sh", "sh", "-c", cmdstring, (char *) 0);
		_exit(127);
	}
	else
	{
		while (waitpid(pid, &status, 0) < 0)
			if (errno != EINTR)
			{
				status = -1;
				break;
			}
	}
	return status;
}

// 描述:  执行命令并将结果存储到result字符串中 
// 参数:  cmd表示要执行的命令
// 返回值:成功返回0，失败返回-1
static int ShellCmdOut(const char *cmd, char *result, int len) 
{  
	struct timeval now;
	char fileName[64] = {0};
	char command2[256] = {0};
	int ret = 0;
	FILE *fp = NULL;

	if((!cmd) || (!strlen(cmd)) || (len <= 0))
	{
		return -1;
	}

	gettimeofday(&now, NULL);

	snprintf(fileName, sizeof(fileName), "/tmp/.ShellCmdOut%d", now.tv_usec);

	memset(result, 0, len); 
	snprintf(command2, sizeof(command2), "%s > %s", cmd, fileName);

	mysystem(command2);
	if ((fp = fopen(fileName, "r")) == NULL)
	{
		perror("fopen");
		ret = -1;
	}
	else
	{
		fread(result, sizeof(char), len, fp);
		
		if(fclose(fp))
		{
			perror("fclose");
			ret = -1;
		}
	}
	memset(command2, 0, sizeof(command2));
	snprintf(command2, sizeof(command2), "rm -rf %s", fileName);
	mysystem(command2);
	
	return ret; 							   
}

static void _Test_Get_Cmd(char* pCmdStr)
{
    SINT32 i = 0;

    while ('\n' != (pCmdStr[i] = getchar()))
    {
        i++;
    }
}

static SINT32 _ReadBinToMemInfo(char *filename, PUPGRADE_MEM_INFO_S pMemInfo)
{
    FILE* fp = NULL;
    long filesize = 0;
    int i = 0;
    if (NULL == filename)
    {
        return -1; 
    }

    fp = fopen(filename, "rb");
    if (NULL == fp)
    {
        UPGRADE_TRACE_ERR("fopen file(%s) failed!\n", filename);
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    pMemInfo->Cnt = (filesize % SINGLE_BUFFER_SIZE) ? (filesize / SINGLE_BUFFER_SIZE + 1) : (filesize / SINGLE_BUFFER_SIZE);

    for (i = 0; i < pMemInfo->Cnt; i++)
    {
        pMemInfo->Mem[i].Size = (filesize - i * SINGLE_BUFFER_SIZE) > SINGLE_BUFFER_SIZE ? SINGLE_BUFFER_SIZE : (filesize - i * SINGLE_BUFFER_SIZE);
        pMemInfo->Mem[i].Addr = malloc(pMemInfo->Mem[i].Size);
        if (pMemInfo->Mem[i].Addr == NULL)
        {
            UPGRADE_TRACE_ERR("Malloc mem(%d) bytes failed!\n", pMemInfo->Mem[i].Size);
            goto err_out;
        }

        memset(pMemInfo->Mem[i].Addr, 0, pMemInfo->Mem[i].Size);
        
        if (fread(pMemInfo->Mem[i].Addr, pMemInfo->Mem[i].Size, 1, fp) != 1)
        {
            UPGRADE_TRACE_ERR("read file(%s) failed!\n", filename);
            goto err_out;
        }

        UPGRADE_TRACE_INFO("[MEM%d] Addr = %p, Size = %d.\n", i, pMemInfo->Mem[i].Addr, pMemInfo->Mem[i].Size);
    }

    UPGRADE_TRACE_INFO("Package  Magic(%s).\n", ((PUPGRADE_FILE_INFO_HEADER_S)(pMemInfo->Mem[0].Addr))->Magic);

    fclose(fp);

    return 0;

err_out:
    if (fp) fclose(fp);

    for (i = 0; i < pMemInfo->Cnt; i++)
    {
        if (pMemInfo->Mem[i].Addr)
        {
            free(pMemInfo->Mem[i].Addr);
            pMemInfo->Mem[i].Addr = NULL;
        }
    }

    return 0;
}

SINT32 main(SINT32 argc, char** argv)
{
    char CmdStr[8] = { 0 };
	char szDevType[64] = {0};
    char filename[256] = { 0 };
    char command[512];
    char getVersion[256];
    char updatefilename[256] = { 0 };
	int i = 0;
    SINT32 RtVal = 0, nCmd = 0;
    UPGRADE_MEM_INFO_S MemInfo = {0};
    UPGRADE_KEY_S key = { 0 };
    UPGRADE_TRACE_ERR("===== upg.txt Upgrade method =====\n");

	FILE *fp=NULL;
	fp = fopen("/tmp/app/disk/mmcblk0p1/upg.txt", "r");
	if (NULL == fp)
	{
		UPGRADE_TRACE_ERR("Usage: fopen upg.txt failed\n");
        return -1;
	}

	if (fgets(filename, sizeof(filename), fp) != NULL)
	{
	    // 去除末尾的换行符（如果存在）
	    int len = strlen(filename);
	    if (len > 0 && filename[len - 1] == '\n') {
	        filename[len - 1] = '\0';  // 替换为字符串结束符，防止upg.txt文件中多余换行符导致识别文件失败
	    }
	}
	else
	{
		UPGRADE_TRACE_ERR("Usage: fgets failed\n");
		return -1;
	}

	fclose(fp);
	fp = NULL;

    snprintf(updatefilename, sizeof(updatefilename), "/tmp/app/disk/mmcblk0p1/%s",filename);
	
	if (access(updatefilename, F_OK) != 0)
	{
		UPGRADE_TRACE_INFO("Usage: filename[%s] not exit\n", updatefilename);
		return 0;
	}
	UPGRADE_TRACE_INFO("[%s-%d]UPG.TXT [%s] \n", __FUNCTION__, __LINE__,filename);
	UPGRADE_TRACE_INFO("[%s-%d]updatefilename [%s] \n", __FUNCTION__, __LINE__,updatefilename);
    RtVal = _ReadBinToMemInfo(updatefilename, &MemInfo);
    if (RtVal < 0)
    {
        UPGRADE_TRACE_ERR("_ReadBinToMemInfo failed with %d!\n", RtVal);
        return -1;
    }

	UPGRADE_TRACE_INFO("m_BoardInfo = %d\n",m_BoardInfo);
	
	for (i = 0; i < MAX_BOARD_NUM; i++)
	{
		if(m_BoardInfo == boardInfoList[i].boardInfo)
		{
			memset(szDevType, 0, sizeof(szDevType));
			memcpy(szDevType, boardInfoList[i].ImageCode, strlen(boardInfoList[i].ImageCode));
			
			UPGRADE_TRACE_INFO("[%s-%d]This board type is %s\n", __FUNCTION__, __LINE__, szDevType);
			break;
		}
	}

    RtVal = Upgrade_Open(Upgrade_Callback, szDevType);
    if (RtVal < 0)
    {
        UPGRADE_TRACE_ERR("Upgrade_Open failed with %d!\n", RtVal);
        return -1;
    }

    RtVal = Upgrade_Mem_CheckEx(0, &MemInfo, &key);
    if (RtVal < 0)
    {
        UPGRADE_TRACE_ERR("Upgrade_Mem_CheckEx failed with %d!\n", RtVal);
        goto out;
    }

    UPGRADE_TRACE_INFO("Upgrade_Mem_CheckEx Success, HWCode(%#x) Ver(%s) BaseVer(%s)\n", key.HwIdCode, key.UpgradeBaseVersion, key.UpgradePackageVersion);
    
    snprintf(command, sizeof(command), 
        "awk -F= '/\\[Software\\]/{f=1;next} /^\\[/{f=0} "
        "f && /Major_version/{maj=$2} "
        "f && /Minor_version/{min=$2} "
        "f && /Reversion/{rev=$2} "
        "f && /BuildDate/{bd=$2} "
        "END{printf \"V%%s.%%s.%%s_%%s\", maj, min, rev, bd}' /mnt/app/device/DeviceVersion.ini");
    ShellCmdOut(command, getVersion, sizeof(getVersion)-1);
    UPGRADE_TRACE_INFO("Get DeviceVersion.ini is %s\n",getVersion);

    RtVal = memcmp(key.UpgradePackageVersion, getVersion, strlen(key.UpgradePackageVersion));
    if (0 == RtVal)
    {
        UPGRADE_TRACE_ERR("Upgrade_Mem_CheckEx failed with %d!\n", RtVal);
        goto out;
    }
    else
    {
        mysystem("mcu_test 1");
    }

    RtVal = Upgrade_Mem_UpdateEx(0, &MemInfo, NULL);
    if (RtVal < 0)
    {
        UPGRADE_TRACE_ERR("Upgrade_Mem_UpdateEx failed with %d!\n", RtVal);
    }
    UPGRADE_TRACE_INFO("Upgrade Success!\n");
    Upgrade_Close(0);
    mysystem("mcu_test 5");
    mysystem("reboot -f");
    
    return 0;
    
out:
    Upgrade_Close(0);

    return 0;
}
