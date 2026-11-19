
/**
  * filename : update.c
  *
  * used for update edvr program.
  *
  * motificatin :
  *                 20071016 - add updating progress. by qzy
  *					20070701 - verify device version, release v2.1
  *					20070625 - fix bugs, write file incompletely. release v2.0
  *                 20070608 - assemble tar and gunzip source code to resolve the
  * the problem of that i can't get the unpacked file name.
  *                 20070530 - fix bugs, release v1.0.
  *                 20070529 - create.
 */
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "update.h"
 
#include "PAR/CConfig.h"
//#define UPDATE_BY_NANDWRITE

void rsleep (unsigned int ntime)
{
   fd_set rfds;  struct timeval tv;
   int retval;  /* Watch stdin  (fd 0) to see when it has input. */
  FD_ZERO (&rfds);  FD_SET (0, &rfds);  /* Wait up to five seconds. */
  tv.tv_sec = ntime / 1000000;
  tv.tv_usec = ntime % 1000000;
   retval = select (0, &rfds, NULL, NULL, &tv);  /* Don't rely on the value of tv now! */
}

/* 升级程序版本 */
char szVersion_update[] = "1.0.0.3123 Build 20100812";

/**
 * \brief
 * background thread of update
 */
int update()
{
	char buf[BUFLEN + 1],strVal[20];		/* buffer for reading */
	int retval,reboot_after_update = 0;
	CConfig *cfg = CConfig::GetInstance();

	if ( NULL == cfg )
	{
		PRNT("NULL == cfg \n");
		return -1;
	}

	PRNT("----updating thread is running\n");
#ifndef UPDATE_BY_NANDWRITE
	#if defined (__T41N__)
	;
	#else
	snprintf(buf, sizeof(buf), "tar -zxvf %s -C /mnt/mtd/", UPDATE_FILE_PATH);
	system(buf);
	#endif
#if 0
		retval = cfg->LoadFile(VERIFY_FILE_NAME);		
		
		if ( 0 != retval )
		{
				PRNT("MPU_PAR_LoadConfig %s error\n",VERIFY_FILE_NAME);
				return -1;
		}
		cfg->GetValue("Check","EnableReboot","NO",strVal,20);
				
		if (strcmp(strVal,"YES") == 0)
		{
			PRNT("reboot_after_update == 1\n");
			reboot_after_update = 1;
		}
		else
			PRNT("reboot_after_update == 0\n");
#endif
	/*默认每次升级后都重启一次*/
	#if defined (__T41N__)
	reboot_after_update = 0;
	#else
	reboot_after_update = 1;
	#endif
#endif

	//加载设备配置
	retval = cfg->LoadFile(NORMAL_CFG_FILE);
	if (0 != retval)
	{
		printf("[update] Load File Failed\n");
	}
	
	//update_status = STATUS_UPDATING_SUCCESS;
	//callback_update_completely(g_hdlg, STATUS_UPDATING_SUCCESS, 0);
	PRNT("[Update]......reset_update_progress (update_end)........\n");
	
	//升级配置
	retval = cfg->UpdateCfg(UPDATE_CFG_FILE, UPDATEFLAG_CFG_FILE);
	if (0 != retval)
	{
		printf("[update] UpdateCfg Failed\n");
	}
	
	retval = cfg->SaveFileReal();
	if (0 != retval)
	{
		printf("[update] SaveFileReal Failed\n");
	}
	
	cfg->ForbiddenSaveFile();
	
#ifndef UPDATE_BY_NANDWRITE
	#if defined (__T41N__)
	;
	#else
	remove (UPDATE_FILE_PATH);	
	remove (FLAG_FILE_NAME);
	#endif
	remove (UPDATE_CFG_FILE);
	remove (UPDATEFLAG_CFG_FILE);
	remove (VERIFY_FILE_NAME);
	if ( 1 == reboot_after_update )
	{
		system("reboot");
	}
#else
	remove (UPDATE_CFG_FILE);
	remove (UPDATEFLAG_CFG_FILE);	
#endif
	return 0;
}
/**
 * \brief
 * check whether update file specialed by _fpath is illegal
 */
int check_update (char *_fpath)
{
	struct stat st;

	//PRNT ("%s\n", _fpath);

	if (access ((char *) (_fpath), R_OK | F_OK) < 0)
	{
		return -1;
	}
	else
	{
		stat ((char *) _fpath, &st);
		if (0 == st.st_size)
		{
			return -1;
		}
		else
		{
			return 0;
		}
	}
}

/**
 *\brief
 *get udpate file path from disk or flash
 *\param
 *NONE
 *RETURN
 *0 - no update file or update file is illeage
 *1 - from disk
 *2 - from flash
*/
#if defined (__T41N__)
int get_update_path (void)
{
	int retval;					/* default */
	char mount_path[128];
	int fd = 0;
	char buf[32] = {0};

	chdir ("/");

	/* update file is illegal */
	if (access ((char *) (FLAG_FILE_NAME), R_OK | F_OK) < 0)
	{
		PRNT ("no upgrade tag file\n");
		return 0;
	}

	fd = open(FLAG_FILE_NAME, O_RDONLY);
	if (-1 == fd)
	{
		printf("can not open the file[%s]\n", FLAG_FILE_NAME);
		return 0;
	}
	
	// 读取文件内容
	read(fd, buf, sizeof(buf));

	// 读取升级标记是否为成功
	if (!strncmp(buf, "success", sizeof("success")))
	{
		PRNT ("upgrade tag is success, ready update file from flash\n");
		retval = 1;
	}
	else
	{
		PRNT ("upgrade tag is failed!\n");
	}
	
	close(fd);

	return retval;
}
#else
int get_update_path (void)
{
 
	int retval;					/* default */
	char mount_path[128];

	chdir ("/");

	/* update file is illegal */
	if (access ((char *) (FLAG_FILE_NAME), R_OK | F_OK) < 0)
	{
		PRNT ("no flag file\n");
		return 0;
	}
	
	if (!check_update (UPDATE_FILE_PATH))
	{
		PRNT ("find update file from flash\n");
		retval = 2;
	}
	else
	{
		PRNT ("not find update file from flash\n");
		retval = 0;
	}
	return retval;
}
#endif

/**
 * \brief
 * process main entry
 */
extern "C"
int main (int argc, char *argv[])
{
	int retval = -1, i;

	PRNT ("[UPDATE] VERSION: %s\n", szVersion_update);
#ifdef UPDATE_BY_NANDWRITE
	update();
#else
	/* check if need update program */
	if (0 != (retval = get_update_path ()))
	{
		update();
	}
	else
	{
    	PRNT("update exit retval = %d\n",retval);
	}
#endif
	return 0;
}
