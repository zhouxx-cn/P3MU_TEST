#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include "disk_shm.h"

#include <sys/sem.h>
#include "logging.h"



#if 0
extern "C"  int mysystem(const char* cmdstring)
{
    pid_t pid;
    int status;

    if (cmdstring == NULL)
        return 1;

    if ( ( pid=vfork() ) < 0 )
    {
        printf("vfork failed\n");
        status = -1;
    }
    else if (pid==0)
    {
        execl("/bin/sh", "sh", "-c", cmdstring, (char*)0);
        _exit(127);
    }
    else
    {
        while(waitpid(pid, &status, 0)<0)
        {
            if(errno!=EINTR)
            {
                status=-1;
                break;
            }
        }
    }

    return status;
}

#else
/**
 *  \brief  Semaphore P OP.
 *  \param  semaphore id
 *  \return 
 */
static int system_p(int semid)
{
	struct sembuf sops = { 0, -1, SEM_UNDO };
	return (semop(semid, &sops, 1));
}

/**
 *  \brief Semaphore V OP.
 *  \param semaphore id
 *  \return 
 */
static int system_v(int semid)
{
	struct sembuf sops = { 0, +1, SEM_UNDO };
	return (semop(semid, &sops, 1));
}

static int system_sem_in(struct system_shareinfo_t *shareinfo)
{
	key_t key;

	int lcshmid, lcsemid;

	void *lcshm;
	
	key = ftok("/tmp/app/bin/systemd", 'w');
	//key = ftok("/lib/systemd", 'w');
	lcshmid = shmget(key, MAX_SYSTEM_CMD_NUM * sizeof (struct system_shminfo_t), IPC_CREAT | 0666);	/* create memory */
	if (FAILURE == lcshmid)
	{
		dbg("create shared momery error.");
		return FAILURE;
	}

	lcshm = shmat(lcshmid, 0, 0);	/* attached memory */
	if (FAILURE == (int) lcshm)
	{
		dbg("attach shared momery error.");
		return FAILURE;
	}

	key = ftok("/tmp/app/bin/systemd", 'x');
	lcsemid = semget(key, 1, IPC_CREAT | 0666);
	if (FAILURE == lcsemid)
	{
		dbg("create semaphore error.");
		return FAILURE;
	}

	system_p(lcsemid);			/* semaphore p. op */

	shareinfo->shm = (system_shminfo_t*)lcshm;
	shareinfo->shmid = lcshmid;
	shareinfo->semid = lcsemid;

	return SUCCESS;
}

static int system_sem_out(struct system_shareinfo_t shareinfo)
{
	void *shm;
	int semid;

	shm = shareinfo.shm;
	semid = shareinfo.semid;

	system_v(semid);
	shmdt(shm);

	return SUCCESS;
}

extern "C" int mysystem(const char *cmdstringl)
{
	int ret;
	int index = MAX_SYSTEM_CMD_NUM, i;
	struct system_shareinfo_t shareinfo;
	struct system_shminfo_t *pchldshminfo;

/**
 * find a  index to execute the command
 */
	system_sem_in(&shareinfo);

	for (i = 0; i < MAX_SYSTEM_CMD_NUM; i++)
	{
		pchldshminfo = (struct system_shminfo_t *) (shareinfo.shm) + i;
		if (1 == pchldshminfo->avail)
		{
			index = i;
			pchldshminfo->avail = 0;
			pchldshminfo->timeout = 0;
			strncpy(pchldshminfo->cmd, cmdstringl, sizeof (pchldshminfo->cmd) - 1);
			pchldshminfo->cmd[sizeof (pchldshminfo->cmd) - 1] = '\0';
			pchldshminfo->status = SYSTEM_READY;
			break;
		}
	}
	system_sem_out(shareinfo);

	if (index == MAX_SYSTEM_CMD_NUM)
	{
		printf("all cmd process are used \n");
		return -1;				//All cmd process are used.
	}


/**
 * waiting for the command result
 */
	while (1)
	{
		system_sem_in(&shareinfo);
		pchldshminfo = (struct system_shminfo_t *) shareinfo.shm + index;
		if (SYSTEM_FINISHED == pchldshminfo->status)
		{
			pchldshminfo->avail = 1;

			if (1 == pchldshminfo->timeout)
			{
				ret = SYS_TIMEOUT;
			}
			else
			{
				ret = pchldshminfo->ret;
			}
			
			pchldshminfo->status = SYSTEM_EMPTY;
			system_sem_out(shareinfo);
			break;
		}
		system_sem_out(shareinfo);
		usleep(10000);			//10ms
	}

	return ret;
}
#endif
