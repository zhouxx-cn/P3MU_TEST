
/*
 * =====================================================================================
 *
 *       Filename:  usb_shm.c
 *
 *    Description:  Share memory operation by semaphore
 *
 *        Version:  1.0
 *        Created:  08/31/2007 09:10:05 PM EDT
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  weileng (BigBanbom), weilengsky@163.com
 *        Company:  HighEasy
 *
 * =====================================================================================
 */

/**
 *  Include Files
 */
#include <stdio.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>

#include "disk_shm.h"
#include "logging.h"


/**
 *  Data Struction Definition
 */
union semun
{
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

/**
 *  Function Ref.
 */

/**
 *  \brief Create a Semaphore
 *  \param  
 *  \return 
 */
static int sem(key_t key)
{
	union semun lcsem;

	int semid;

	lcsem.val = 0;
	semid = semget(key, 1, IPC_CREAT | 0666);
	if (FAILURE == semid)
	{
		dbg("create¡¡semaphore¡¡error.");
		return FAILURE;
	}

	/* init semaphore */
	semctl(semid, 0, SETVAL, lcsem);
	return semid;
}

#if 0

/**
 *  \brief Delete a Semaphore
 *  \param  
 *  \return 
 */
static void d_sem(int semid)
{
	union semun lcsem;

	lcsem.val = 0;
	semctl(semid, 0, IPC_RMID, 0);
}
#endif

/**
 *  \brief  Semaphore P OP.
 *  \param  semaphore id
 *  \return 
 */
static int p(int semid)
{
	struct sembuf sops = { 0, +1, IPC_NOWAIT };
	return (semop(semid, &sops, 1));
}

/**
 *  \brief Semaphore V OP.
 *  \param semaphore id
 *  \return 
 */
static int v(int semid)
{
	struct sembuf sops = { 0, -1, IPC_NOWAIT };
	return (semop(semid, &sops, 1));
}

static int cli_sem(key_t key)
{
	int semid;

	semid = semget(key, 0, 0);
	if (FAILURE == semid)
	{
		dbg("get semaphore error.");
	}

	return semid;
}

int acquire_shm_and_sem(struct shareinfo_t *shareinfo)
{
	key_t key;

	int lcshmid, lcsemid;

	void *lcshm;

	key = ftok("weileng", 'a');
	lcshmid = shmget(key, SEGSIZE, IPC_CREAT | 0604);	/* create memory */
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

	lcsemid = sem(key);
	if (FAILURE == lcsemid)
	{
		dbg("create semaphore error.");
		return FAILURE;
	}
	p(lcsemid);					/* semaphore p. op */

	shareinfo->shm = lcshm;
	shareinfo->shmid = lcshmid;
	shareinfo->semid = lcsemid;

	return SUCCESS;
}

int release_shm_and_sem(struct shareinfo_t shareinfo)
{
	void *shm;

	int semid;

//  struct shmid_ds buf;

	shm = shareinfo.shm;
	semid = shareinfo.semid;

	v(semid);
	shmdt(shm);

//  shmctl(shmid, IPC_RMID, &buf);
//  d_sem(semid);

	return SUCCESS;
}

int cli_get_shm_info(struct shareinfo_t *shareinfo)
{
	key_t key;

	int shmid, semid;

	char *shm;

	key = ftok("weileng", 'a');
	shmid = shmget(key, SEGSIZE, 0);
	if (FAILURE == shmid)
	{
		dbg("get shared momery error.");
		return FAILURE;
	}

	shm = shmat(shmid, 0, 0);
	if (FAILURE == (int) shm)
	{
		dbg("attach shared momery error.");
		return FAILURE;
	}

	semid = cli_sem(key);

	shareinfo->shm = shm;
	shareinfo->semid = semid;
	shareinfo->shmid = shmid;

	return SUCCESS;
}

int cli_release_shm_info(struct shareinfo_t shareinfo)
{
	void *shm;

//  struct shmid_ds buf;
//  int shmid;

	shm = shareinfo.shm;
//  shmid = shareinfo.shmid;

//  shmctl(shmid, IPC_RMID, &buf);
	shmdt(shm);					/* unattached memory */

	return SUCCESS;
}

void cli_wait_for_sem(struct shareinfo_t shareinfo)
{
	int semid;
	struct sembuf sops = { 0, 0, 0 };

	semid = shareinfo.semid;
	semop(semid, &sops, 1);
}

int shminfo_conduct_by_opcode(enum shminfo_op_t shminfo_op, unsigned char devindex, void *shminfo)
{
	int ret = SUCCESS;

	switch (shminfo_op)
	{
			struct shareinfo_t shareinfo;

			struct shminfo_t *lcshm;

		case OP_HEAD_GET:
			{
				struct headinfo_t *shminfo_head;

				shminfo_head = (struct headinfo_t *) shminfo;

				ret = cli_get_shm_info(&shareinfo);
				if (FAILURE == ret)
				{
					dbg("client get share memory information error!\n");
					goto err;
				}

				/* sync for hotplug or edvr update share memory information */
				cli_wait_for_sem(shareinfo);

				/* get share memory information from devindex number */
				lcshm = (struct shminfo_t *) shareinfo.shm;

				shminfo_head->phyinfo = lcshm->headinfo.phyinfo;
				shminfo_head->hardinfo = lcshm->headinfo.hardinfo;
				shminfo_head->cdinfo = lcshm->headinfo.cdinfo;
				shminfo_head->status = lcshm->headinfo.status;

				cli_release_shm_info(shareinfo);
			}
			break;

		case OP_HEAD_UPDATE:
			{
				struct headinfo_t *shminfo_head;

				shminfo_head = (struct headinfo_t *) shminfo;

				ret = acquire_shm_and_sem(&shareinfo);
				if (FAILURE == ret)
				{
					dbg("cann't acquire share memory or semaphore.");
					goto err;
				}

				lcshm = (struct shminfo_t *) (shareinfo.shm);

				lcshm->headinfo.phyinfo = shminfo_head->phyinfo;
				lcshm->headinfo.hardinfo = shminfo_head->hardinfo;
				lcshm->headinfo.cdinfo = shminfo_head->cdinfo;
				lcshm->headinfo.status = shminfo_head->status;

				release_shm_and_sem(shareinfo);
			}
			break;

		case OP_DEV_GET:
			{
				struct devinfo_t *shminfo_dev;

				shminfo_dev = (struct devinfo_t *) shminfo;

				ret = cli_get_shm_info(&shareinfo);
				if (FAILURE == ret)
				{
					dbg("client get share memory information error!\n");
					goto err;
				}

				/* sync for hotplug or edvr update share memory information */
				cli_wait_for_sem(shareinfo);

				/* get share memory information from index number */
				lcshm = (struct shminfo_t *) shareinfo.shm;

				shminfo_dev->status = lcshm->devinfo[devindex].status;
				shminfo_dev->pthd = lcshm->devinfo[devindex].pthd;
				shminfo_dev->partinfo = lcshm->devinfo[devindex].partinfo;
				memcpy(shminfo_dev->devguid, lcshm->devinfo[devindex].devguid, strlen(lcshm->devinfo[devindex].devguid) + 1);
				memcpy(shminfo_dev->devpath, lcshm->devinfo[devindex].devpath, strlen(lcshm->devinfo[devindex].devpath) + 1);
				memcpy(shminfo_dev->module, lcshm->devinfo[devindex].module, strlen(lcshm->devinfo[devindex].module) + 1);
				memcpy(shminfo_dev->vendor, lcshm->devinfo[devindex].vendor, strlen(lcshm->devinfo[devindex].vendor) + 1);

				memcpy(shminfo_dev->mntsize, lcshm->devinfo[devindex].mntsize, MAX_USB_DEV * sizeof (unsigned short));

				cli_release_shm_info(shareinfo);
			}
			break;

		case OP_DEV_UPDATE:
			{
				struct devinfo_t *shminfo_dev;

				shminfo_dev = (struct devinfo_t *) shminfo;

				ret = acquire_shm_and_sem(&shareinfo);
				if (FAILURE == ret)
				{
					dbg("cann't acquire share memory or semaphore.");
					goto err;
				}

				lcshm = (struct shminfo_t *) (shareinfo.shm);

				/* update share memory information */
				lcshm->devinfo[devindex].status = shminfo_dev->status;
				lcshm->devinfo[devindex].pthd = shminfo_dev->pthd;
				lcshm->devinfo[devindex].partinfo = shminfo_dev->partinfo;
				memcpy(lcshm->devinfo[devindex].devguid, shminfo_dev->devguid, strlen(shminfo_dev->devguid) + 1);
				memcpy(lcshm->devinfo[devindex].devpath, shminfo_dev->devpath, strlen(shminfo_dev->devpath) + 1);
				memcpy(lcshm->devinfo[devindex].module, shminfo_dev->module, strlen(shminfo_dev->module) + 1);
				memcpy(lcshm->devinfo[devindex].vendor, shminfo_dev->vendor, strlen(shminfo_dev->vendor) + 1);
				memcpy(lcshm->devinfo[devindex].mntsize, shminfo_dev->mntsize, MAX_USB_DEV * sizeof (unsigned short));

				release_shm_and_sem(shareinfo);
			}
			break;

		case OP_HARD_GET:
			{
				struct hardinfo_t *shminfo_hard;

				shminfo_hard = (struct hardinfo_t *) shminfo;

				ret = cli_get_shm_info(&shareinfo);
				if (FAILURE == ret)
				{
					dbg("client get share memory information error!\n");
					goto err;
				}

				/* sync for hotplug or edvr update share memory information */
				cli_wait_for_sem(shareinfo);

				/* get share memory information from index number */
				lcshm = (struct shminfo_t *) shareinfo.shm;

				shminfo_hard->size = lcshm->hardinfo[devindex].size;
				memcpy(shminfo_hard->module, lcshm->hardinfo[devindex].module, strlen(lcshm->hardinfo[devindex].module) + 1);
				memcpy(shminfo_hard->vendor, lcshm->hardinfo[devindex].vendor, strlen(lcshm->hardinfo[devindex].vendor) + 1);
				memcpy(shminfo_hard->mntpnt, lcshm->hardinfo[devindex].mntpnt, strlen(lcshm->hardinfo[devindex].mntpnt) + 1);

				cli_release_shm_info(shareinfo);
			}
			break;

		case OP_HARD_UPDATE:
			{
				struct hardinfo_t *shminfo_hard;

				shminfo_hard = (struct hardinfo_t *) shminfo;

				ret = acquire_shm_and_sem(&shareinfo);
				if (FAILURE == ret)
				{
					dbg("cann't acquire share memory or semaphore.");
					goto err;
				}

				lcshm = (struct shminfo_t *) (shareinfo.shm);

				/* update share memory information */
				lcshm->hardinfo[devindex].size = shminfo_hard->size;
				memcpy(lcshm->hardinfo[devindex].module, shminfo_hard->module, strlen(shminfo_hard->module) + 1);
				memcpy(lcshm->hardinfo[devindex].vendor, shminfo_hard->vendor, strlen(shminfo_hard->vendor) + 1);
				memcpy(lcshm->hardinfo[devindex].mntpnt, shminfo_hard->mntpnt, strlen(shminfo_hard->mntpnt) + 1);

				release_shm_and_sem(shareinfo);
			}
			break;

		case OP_CD_GET:
			{
				struct cdinfo_t *shminfo_cd;

				shminfo_cd = (struct cdinfo_t *) shminfo;

				ret = cli_get_shm_info(&shareinfo);
				if (FAILURE == ret)
				{
					dbg("client get share memory information error!\n");
					goto err;
				}

				/* sync for hotplug or edvr update share memory information */
				cli_wait_for_sem(shareinfo);

				/* get share memory information from index number */
				lcshm = (struct shminfo_t *) shareinfo.shm;

				shminfo_cd->status = lcshm->cdinfo[devindex].status;
				shminfo_cd->size = lcshm->cdinfo[devindex].size;
				memcpy(shminfo_cd->devguid, lcshm->cdinfo[devindex].devguid, strlen(lcshm->cdinfo[devindex].devguid) + 1);
				memcpy(shminfo_cd->devpath, lcshm->cdinfo[devindex].devpath, strlen(lcshm->cdinfo[devindex].devpath) + 1);
				memcpy(shminfo_cd->module, lcshm->cdinfo[devindex].module, strlen(lcshm->cdinfo[devindex].module) + 1);
				memcpy(shminfo_cd->vendor, lcshm->cdinfo[devindex].vendor, strlen(lcshm->cdinfo[devindex].vendor) + 1);
				memcpy(shminfo_cd->mntpnt, lcshm->cdinfo[devindex].mntpnt, strlen(lcshm->cdinfo[devindex].mntpnt) + 1);

				cli_release_shm_info(shareinfo);
			}
			break;

		case OP_CD_UPDATE:
			{
				struct cdinfo_t *shminfo_cd;

				shminfo_cd = (struct cdinfo_t *) shminfo;

				ret = acquire_shm_and_sem(&shareinfo);
				if (FAILURE == ret)
				{
					dbg("cann't acquire share memory or semaphore.");
					goto err;
				}

				lcshm = (struct shminfo_t *) (shareinfo.shm);

				/* update share memory information */
				lcshm->cdinfo[devindex].status = shminfo_cd->status;
				lcshm->cdinfo[devindex].size = shminfo_cd->size;
				memcpy(lcshm->cdinfo[devindex].devguid, shminfo_cd->devguid, strlen(shminfo_cd->devguid) + 1);
				memcpy(lcshm->cdinfo[devindex].devpath, shminfo_cd->devpath, strlen(shminfo_cd->devpath) + 1);
				memcpy(lcshm->cdinfo[devindex].module, shminfo_cd->module, strlen(shminfo_cd->module) + 1);
				memcpy(lcshm->cdinfo[devindex].vendor, shminfo_cd->vendor, strlen(shminfo_cd->vendor) + 1);
				memcpy(lcshm->cdinfo[devindex].mntpnt, shminfo_cd->mntpnt, strlen(shminfo_cd->mntpnt) + 1);

				release_shm_and_sem(shareinfo);
			}
			break;

		case OP_NONE:
			ret = FAILURE;
			break;
	}

  err:
	return ret;
}



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

int system_init(struct system_shareinfo_t *shareinfo)
{
	key_t key;

	int lcshmid, lcsemid;

	void *lcshm;

	int i;

	struct system_shminfo_t *pshm;

	key = ftok("/tmp/app/bin/systemd", 'w');

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

	lcsemid = sem(key);
	if (FAILURE == lcsemid)
	{
		dbg("create semaphore error.");
		return FAILURE;
	}

	system_v(lcsemid);			/* semaphore p. op */

	shareinfo->shm = lcshm;
	shareinfo->shmid = lcshmid;
	shareinfo->semid = lcsemid;

	for (i = 0; i < MAX_SYSTEM_CMD_NUM; i++)
	{
		pshm = (struct system_shminfo_t *) lcshm + i;
		pshm->avail = 1;
		pshm->status = SYSTEM_EMPTY;
	}

	return SUCCESS;
}


int system_sem_in(struct system_shareinfo_t *shareinfo)
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

	shareinfo->shm = lcshm;
	shareinfo->shmid = lcshmid;
	shareinfo->semid = lcsemid;

	return SUCCESS;
}

int system_sem_out(struct system_shareinfo_t shareinfo)
{
	void *shm;

	int semid;

//  struct shmid_ds buf;

	shm = shareinfo.shm;
	semid = shareinfo.semid;

	system_v(semid);
	shmdt(shm);

//  shmctl(shmid, IPC_RMID, &buf);
//  d_sem(semid);

	return SUCCESS;
}
