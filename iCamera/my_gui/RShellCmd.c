/*
 * ShellCmd.c
 *
 *  Created on: Dec 1, 2020
 *      Author: ad
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/prctl.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <netinet/in.h>
#include <pthread.h>


#include "RShellCmd.h"
#include "mysystem.h"
#include "LogManager.h"
#include "zwsdk.h"
#include "DEVICEVERSION.h"
#include "MPU_PUB_API.h"
#include "Authority.h"
#include "MPU_LCD_API.h"

typedef void (*SHELL_CMD_FUNC)(int argc,char * argv[]);

typedef struct tagShellCmdTblInfo
{
	const char* cmd;
	SHELL_CMD_FUNC pShellCmdFunc;
}ShellCmdTblInfo;

void shell_set_log_enable(int argc,char * argv[])
{
	if( argc < 1 )
	{
		OPEN_LOG_W("Usage:\n");
		OPEN_LOG_W("log_enable 1 => open printf log!!! redirect to /dev/ttyAMA0\n");
		OPEN_LOG_W("log_enable 0 => close printf log!!! redirect to /dev/null\n");
		return;
	}

	OPEN_LOG_W("%s => set log enable: %d\n", __FUNCTION__, atoi(argv[0]));
	SetLogEnable(atoi(argv[0]));
}


void shell_set_log_level(int argc,char * argv[])
{
	if( argc < 1 )
	{
		/* 打印当前的日志级别 */
		OPEN_LOG_E("cur log level:%d\n", get_log_level());
		OPEN_LOG_E("Usage: log_level level(0~5) \n");
		return;
	}

	int level = atoi(argv[0]);

	if((level < DEBUG_LOG_LEVEL_NONE) || (level > DEBUG_LOG_LEVEL_ALL))
	{
		OPEN_LOG_E("Usage: log_level level(0~5) \n");
		return;
	}

	OPEN_LOG_I("set log level is from %d to:%d \n", get_log_level(), level);
	set_log_level(level);
}


void shell_set_log_module(int argc,char * argv[])
{
	if( argc < 2 )
	{
		/* 打印当前的日志级别 */
		OPEN_LOG_E("***************************************************\n");
		OPEN_LOG_E("examle:                                            \n");
		OPEN_LOG_E("	 log_module module(0~31,>=256) enable(0~1)	   \n");
		OPEN_LOG_E("     log_module 1  1                               \n");
		OPEN_LOG_E("module state: > 256 ALL                            \n");

		int i = 0;
		for (i = 0; i < 32; i++)
		{
			char* name = GetLogModuleName((LOG_MODULE_E)(0x0001 << i));
			if (name)
			{
				OPEN_LOG_W("  %-2d    %-10s   [%d] \n", i, name, GetLogModuleEnable((LOG_MODULE_E)(0x0001 << i)));
			}
		}

		return;
	}

	LOG_MODULE_E moduleBit = LOG_MODULE_OPEN;
	unsigned int index  = (unsigned int)atoi(argv[0]);
	unsigned int enable = (unsigned int)atoi(argv[1]);

	if (index >= 0 && index < 32)
	{
		moduleBit = (LOG_MODULE_E)(0x0001 << index);
	}
	else
	{
		if (index < 256)
		{
			OPEN_LOG_E("***************************************************\n");
			OPEN_LOG_E("examle: 										   \n");
			OPEN_LOG_E("	 log_module module(0~31,>=256) enable(0~1)	   \n");
			OPEN_LOG_E("	 log_module 1  1							   \n");
			return;
		}

		moduleBit = LOG_MODULE_ALL;
	}

	OPEN_LOG_I("***************************************************\n");
	OPEN_LOG_I("Set log module, module=[%d] enable=[%d] moduleBit=[0x%08X]\n", 
		index, enable, (unsigned int)moduleBit);
	SetLogModuleEnable(moduleBit, enable);
	
	OPEN_LOG_I("module state: \n");
	int i = 0;
	for (i = 0; i < 32; i++)
	{
		char* name = GetLogModuleName((LOG_MODULE_E)(0x0001 << i));
		if (name)
		{
			OPEN_LOG_W("  %-2d    %-10s   [%d] \n", i, name, GetLogModuleEnable((LOG_MODULE_E)(0x0001 << i)));
		}
	}
}


void shell_cmd_get_version(int argc, char * argv[])
{
    //获取设备版本信息
    DEVICEVERSION dv = {0};
    MPU_PUB_GetDeviceVersion(&dv,0);
	OPEN_LOG_I("**********************************\n");
	OPEN_LOG_I("FW compile date =[%s %s]\n", __DATE__, __TIME__);
	OPEN_LOG_I("  m_szHardwareVersion = [%s]\n", dv.m_szHardwareVersion);
	OPEN_LOG_I("  m_szSoftwareVersion = [%s]\n", dv.m_szSoftwareVersion);
	OPEN_LOG_I("  m_szDspVersion      = [%s]\n", dv.m_szDspVersion);
	OPEN_LOG_I("**********************************\n");
	return;
}

void shell_cmd_print_authinfo(int argc,char * argv[])
{
    PrintAuthorizeInfo();
	OPEN_LOG_I("show authinfo\n");
}

void shell_cmd_print_lcdinfo(int argc,char * argv[])
{
    MPU_LCD_DumpInfo();
}

const ShellCmdTblInfo sShellCmdTbl[] = 
{
	{"log_enable",				shell_set_log_enable},
	{"log_level",				shell_set_log_level},
	{"log_module",				shell_set_log_module},
	{"getVersion",				shell_cmd_get_version},
    {"authinfo",				shell_cmd_print_authinfo},
    {"lcdinfo",				    shell_cmd_print_lcdinfo},
};

void createShellCmdSymLink(void)
{
	if(0 != access("/tmp/usr/bin", 0))
	{
		mysystem("mkdir -p /tmp/usr/bin");
	}

	unsigned int loop  = 0;
	for( loop = 0; loop < sizeof(sShellCmdTbl)/sizeof(sShellCmdTbl[0]); loop++ )
	{
		char softlink[128] = {0};
		snprintf( softlink, 128, "/tmp/usr/bin/%s", sShellCmdTbl[loop].cmd);
		symlink("/mnt/app/bin/RShell", softlink);
	}
}

void execShellCmd(const char* shell, int argc,char * argv[])
{
	unsigned int loop = 0;
	for( loop = 0; loop < sizeof(sShellCmdTbl)/sizeof(sShellCmdTbl[0]); loop++ )
	{
		if( !strcmp(sShellCmdTbl[loop].cmd, shell) )
		{
			sShellCmdTbl[loop].pShellCmdFunc(argc, argv);
			break;
		}
	}
}

int StartShellCmdLoop(void)
{
	/* 创建Shell命令软链接 */
	createShellCmdSymLink();

	/* 主循环，处理Shell命令 */
	int shellSockFd = -1;
	struct sockaddr_in shellClientaddr;
	socklen_t addrlen = sizeof(shellClientaddr);

	shellSockFd = socket(AF_INET, SOCK_DGRAM, 0);
	if (shellSockFd == -1)
	{
		OPEN_LOG_E("Create socket fail, error:%s\n", strerror(errno));
		return -1;
	}

	int reuse = 1;
	setsockopt(shellSockFd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

	char shellCmdBuf[1024];
	ssize_t recvbytes = 0;
	struct sockaddr_in shellServaddr;
	shellServaddr.sin_family = AF_INET;
	shellServaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	shellServaddr.sin_port = htons(8822);

	if (bind(shellSockFd, (const struct sockaddr *)&shellServaddr, sizeof(shellServaddr)) == -1)
	{
		OPEN_LOG_E("Bind shell socket fail, error:%s\n", strerror(errno));
		close( shellSockFd );
		return -1;
	}

	while(1)
	{
		memset( shellCmdBuf, 0x00, sizeof(shellCmdBuf) );
		memset( &shellClientaddr, 0, sizeof(struct sockaddr_in) );

		recvbytes = recvfrom(shellSockFd, shellCmdBuf, sizeof(shellCmdBuf), 0, (struct sockaddr *)&shellClientaddr, &addrlen);
		if (recvbytes < 0)
		{
			if (errno == EINTR)
			{
				usleep(10);
				continue;
			}
			else
			{
				OPEN_LOG_E("recvfrom shell socket failed\n");
				break;
			}
		}

		OPEN_LOG_I("Recv RShell : [%s]\n", shellCmdBuf);

		/* 解析Shell命令 */
		int shellArgc = 0;
		char * shellArgv[32]; /* 最多支持32个参数 */
		char delim[] = " ";
		char *token;
		for(token = strtok(shellCmdBuf, delim); token != NULL; token = strtok(NULL, delim))
		{
			shellArgv[shellArgc++] = token;
			if( shellArgc >= 31 )
			{
				OPEN_LOG_W("Too many shell args\n");
				break;
			}
		}

		if( shellArgc >= 1 )
		{
			/* 执行Shell命令 */
			execShellCmd(shellArgv[0], (shellArgc-1), (shellArgv+1) );
		}
	}

	close( shellSockFd );

	return 0;
}

void *RshellCmdBody(void *arg)
{
	prctl(PR_SET_NAME, "RshellCmdBody");
	pthread_detach(pthread_self());
	(void)arg;

	StartShellCmdLoop();
	return NULL;
}

int RShellCmdInit(void)
{
/*
	#ifndef __USE_RSHELL__
	OPEN_LOG_W("is not support Rshell!!!\n");
	return 0;
	#endif
*/
	pthread_t Rshell_tid;
	if(pthread_create(&Rshell_tid, NULL, RshellCmdBody, NULL) != 0)
	{
		OPEN_LOG_W("Failed to create RshellCmdBody thread\n");
		return -1;
	}

	return 0;
}


