
/*
 * =====================================================================================
 *
 *       Filename:  usb_shm.h
 *
 *    Description:  Share memory operation by semaphore
 *
 *        Version:  1.0
 *        Created:  08/31/2007 09:52:02 PM EDT
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  weileng (BigBanbom), weilengsky@163.com
 *        Company:  HighEasy
 *
 * =====================================================================================
 */
#ifndef  USB_SHM_INC
#define  USB_SHM_INC

#include "global_constant.h"



/**
 *  Data Struction Definition
 */

/* usb device status */
enum usb_status_t
{
	USB_ABSENT,					/* absent */
	USB_INIT,					/* init */
	USB_BUSY,					/* probe or busy... */
	USB_NORMAL,					/* present and normal */
	USB_NONE
};

/* share memory initial flag */
enum mem_status_t
{
	MEM_INIT,					/* not shared */
	MEM_SHARE,					/* shared ok */
	MEM_NONE
};

enum shminfo_op_t
{
	OP_HEAD_GET,				/* get head info */
	OP_HEAD_UPDATE,				/* update head info */
	OP_DEV_GET,					/* get device info */
	OP_DEV_UPDATE,				/* update device info */
	OP_CD_GET,					/* get cd info */
	OP_CD_UPDATE,				/* update cd info */
	OP_HARD_GET,				/* get ide info */
	OP_HARD_UPDATE,				/* update ide info */

	OP_NONE
};

/* device plug/unplug gen. action */
enum dev_status_t
{
	DEV_ADD,					/* add usb device */
	DEV_REMOVE,					/* remove usb device */
	DEV_NONE
};

/* devic typ */
enum dev_type_t
{
	USB_DISK,
	USB_CDROM,
	IDE_CDROM,
	SATA_DISK,
	SATA_CDROM,

	TYPE_NONE
};

struct headinfo_t
{
	enum mem_status_t status;	/* mem status */
	unsigned short phyinfo;		/* physical usb disk device number */
	unsigned short cdinfo;		/* physical usb cd/dvd device number */
	unsigned short hardinfo;	/* physical ide cd/dvd device number */
};

struct devinfo_t
{
	enum usb_status_t status;	/* usb device status */
	unsigned short partinfo;	/* usb partition information: 0 for not present, 1 for present
								   maxminum suport partition numbers is 16 */
	int pthd;					/* thread handle for write */
	unsigned short mntsize[MAX_USB_DEV];	/* each partition type */
	char devpath[MAX_NAME_LEN];	/* usb device number */
	char devguid[MAX_NAME_LEN];	/* usb device GUID, terminate by NULL */
	char module[MAX_NAME_LEN];	/* usb module name */
	char vendor[MAX_NAME_LEN];	/* usb vendor name */
};

struct cdinfo_t
{
	enum usb_status_t status;	/* usb device status */
	unsigned short size;		/* each partition type */
	char devpath[MAX_NAME_LEN];	/* usb device number */
	char devguid[MAX_NAME_LEN];	/* usb device GUID, terminate by NULL */
	char module[MAX_NAME_LEN];	/* usb module name */
	char vendor[MAX_NAME_LEN];	/* usb vendor name */
	char mntpnt[MAX_NAME_LEN];	/* usb cdrom mount point */
};

struct hardinfo_t
{
	unsigned short size;		/* ide volume */
	char module[MAX_NAME_LEN];	/* ide module name */
	char vendor[MAX_NAME_LEN];	/* ide vendor name */
	char mntpnt[MAX_NAME_LEN];	/* ide scsi id */
};

struct shminfo_t
{
	struct headinfo_t headinfo;
	struct devinfo_t devinfo[MAX_USB_DEV];
	struct cdinfo_t cdinfo[MAX_IDE_DEV];
	struct hardinfo_t hardinfo[MAX_IDE_DEV];
};

struct shareinfo_t
{
	void *shm;					/* share memory handle */
	int shmid;					/* share memory flag id */
	int semid;					/* semaphore flag id */
};

/**
 *  \brief 
 *  \param  
 *  \return 
 */
extern int idvr_shminfo_conduct_by_opcode(enum shminfo_op_t shminfo_op, unsigned char devindex, void *shminfo);
#endif							/* ----- #ifndef USB_SHM_INC  ----- */
