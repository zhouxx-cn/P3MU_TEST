
/*!
*\file   fdisk.c
*\brief  磁盘分区实现
*\author fc
*\date	 2016年12月14日 10:03:02
*\version 0.1
*/

#include <stdio.h>
#include <stdlib.h>				/* atoi, free */
#include <stdarg.h>				/* varargs */
#include <unistd.h>				/* read, write */
#include <fcntl.h>				/* O_RDWR */
#include <errno.h>				/* ERANGE */
#include <string.h>				/* index() */
#include <ctype.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <linux/unistd.h>		/* _syscall */
#include <linux/hdreg.h>		/* HDIO_GETGEO */
#include <endian.h>

#include "fdisk.h"

/**************************************************************************
*                            常量                                        *
**************************************************************************/

/**************************************************************************
*                            宏                                          *
**************************************************************************/

#define __FAT32__

#define PROGNAME "FanCong fdisk"
#define setlocale(x, y)
#define bindtextdomain(x, y)
#define textdomain(x)
#define _(x) x
#define N_(x) x
#define BLKRRPART  _IO(0x12,95)	/* re-read partition table */
#define BLKGETSIZE _IO(0x12,96)	/* return device size */
#define BLKFLSBUF  _IO(0x12,97)	/* flush buffer cache */
#define SIZE(a)	(sizeof(a)/sizeof(a[0]))


#if __BYTE_ORDER == __LITTLE_ENDIAN
#define ArchSwapLE16(X)        (X)
#define ArchSwapLE32(X)        (X)
#else
static inline unsigned short ArchSwapLE16(unsigned short D)
{
	return ((D << 8) | (D >> 8));
}

static inline unsigned int ArchSwapLE32(unsigned int D)
{
	return ((D << 24) | ((D << 8) & 0x00FF0000) | ((D >> 8) & 0x0000FF00) | (D >> 24));
}
#endif


/**************************************************************************
*                          数据类型                                       *
**************************************************************************/


struct systypes
{
	unsigned char type;
	char *name;
};

struct systypes i386_sys_types[] = {
	{0x00, N_("Empty")},
	{0x01, N_("FAT12")},
	{0x02, N_("XENIX root")},
	{0x03, N_("XENIX usr")},
	{0x04, N_("FAT16 <32M")},
	{0x05, N_("Extended")},		/* DOS 3.3+ extended partition */
	{0x06, N_("FAT16")},		/* DOS 16-bit >=32M */
	{0x07, N_("HPFS/NTFS")},	/* OS/2 IFS, eg, HPFS or NTFS or QNX */
	{0x08, N_("AIX")},			/* AIX boot (AIX -- PS/2 port) or SplitDrive */
	{0x09, N_("AIX bootable")},	/* AIX data or Coherent */
	{0x0a, N_("OS/2 Boot Manager")},	/* OS/2 Boot Manager */
	{0x0b, N_("Win95 FAT32")},
	{0x0c, N_("Win95 FAT32 (LBA)")},	/* LBA really is `Extended Int 13h' */
	{0x0e, N_("Win95 FAT16 (LBA)")},
	{0x0f, N_("Win95 Ext'd (LBA)")},
	{0x10, N_("OPUS")},
	{0x11, N_("Hidden FAT12")},
	{0x12, N_("Compaq diagnostics")},
	{0x14, N_("Hidden FAT16 <32M")},
	{0x16, N_("Hidden FAT16")},
	{0x17, N_("Hidden HPFS/NTFS")},
	{0x18, N_("AST Windows swapfile")},
	{0x1b, N_("Hidden Win95 FAT32")},
	{0x1c, N_("Hidden Win95 FAT32 (LBA)")},
	{0x1e, N_("Hidden Win95 FAT16 (LBA)")},
	{0x24, N_("NEC DOS")},
	{0x3c, N_("PartitionMagic recovery")},
	{0x40, N_("Venix 80286")},
	{0x41, N_("PPC PReP Boot")},
	{0x42, N_("SFS")},
	{0x4d, N_("QNX4.x")},
	{0x4e, N_("QNX4.x 2nd part")},
	{0x4f, N_("QNX4.x 3rd part")},
	{0x50, N_("OnTrack DM")},
	{0x51, N_("OnTrack DM6 Aux1")},	/* (or Novell) */
	{0x52, N_("CP/M")},			/* CP/M or Microport SysV/AT */
	{0x53, N_("OnTrack DM6 Aux3")},
	{0x54, N_("OnTrackDM6")},
	{0x55, N_("EZ-Drive")},
	{0x56, N_("Golden Bow")},
	{0x5c, N_("Priam Edisk")},
	{0x61, N_("SpeedStor")},
	{0x63, N_("GNU HURD or SysV")},	/* GNU HURD or Mach or Sys V/386 (such as ISC UNIX) */
	{0x64, N_("Novell Netware 286")},
	{0x65, N_("Novell Netware 386")},
	{0x70, N_("DiskSecure Multi-Boot")},
	{0x75, N_("PC/IX")},
	{0x80, N_("Old Minix")},	/* Minix 1.4a and earlier */
	{0x81, N_("Minix / old Linux")},	/* Minix 1.4b and later */
	{0x82, N_("Linux swap")},	/* also Solaris */
	{0x83, N_("Linux")},
	{0x84, N_("OS/2 hidden C: drive")},
	{0x85, N_("Linux extended")},
	{0x86, N_("NTFS volume set")},
	{0x87, N_("NTFS volume set")},
	{0x8e, N_("Linux LVM")},
	{0x93, N_("Amoeba")},
	{0x94, N_("Amoeba BBT")},	/* (bad block table) */
	{0x9f, N_("BSD/OS")},		/* BSDI */
	{0xa0, N_("IBM Thinkpad hibernation")},
	{0xa5, N_("BSD/386")},
	{0xa6, N_("OpenBSD")},
	{0xa7, N_("NeXTSTEP")},
	{0xb7, N_("BSDI fs")},
	{0xb8, N_("BSDI swap")},
	{0xc1, N_("DRDOS/sec (FAT-12)")},
	{0xc4, N_("DRDOS/sec (FAT-16 < 32M)")},
	{0xc6, N_("DRDOS/sec (FAT-16)")},
	{0xc7, N_("Syrinx")},
	{0xdb, N_("CP/M / CTOS / ...")},	/* CP/M or Concurrent CP/M or Concurrent DOS or CTOS */
	{0xe1, N_("DOS access")},	/* DOS access or SpeedStor 12-bit FAT extended partition */
	{0xe3, N_("DOS R/O")},		/* DOS R/O or SpeedStor */
	{0xe4, N_("SpeedStor")},	/* SpeedStor 16-bit FAT extended partition < 1024 cyl. */
	{0xeb, N_("BeOS fs")},
	{0xf1, N_("SpeedStor")},
	{0xf4, N_("SpeedStor")},	/* SpeedStor large partition */
	{0xf2, N_("DOS secondary")},	/* DOS 3.3+ secondary */
	{0xfd, N_("Linux raid autodetect")},	/* New (2.2.x) raid partition with autodetect
											   using persistent superblock */
	{0xfe, N_("LANstep")},		/* SpeedStor >1024 cyl. or LANstep */
	{0xff, N_("BBT")},			/* Xenix Bad Block Table */
	{0, 0}
};



/**************************************************************************
*                           全局变量                                      *
**************************************************************************/

int exit_status = 0;

int part_current = 0;

unsigned long disk_cylinders;	//lsj

unsigned long part_cylinders;	//lsj

char *part_string[15];

int disk_parts;					//lsj

int force = 0;					/* 1: do what I say, even if it is stupid ... */

int quiet = 0;					/* 1: suppress all warnings */

#undef Linux
int Linux = 0;					/* 1: suppress warnings irrelevant for Linux */

int DOS = 0;					/* 1: shift extended partitions by #sectors, not 1 */

int DOS_extended = 0;			/* 1: use starting cylinder boundary of extd partn */

int dump = 0;					/* 1: list in a format suitable for later input */

int verify = 0;					/* 1: check that listed partition is reasonable */

int no_write = 0;				/* 1: do not actually write to disk */

int no_reread = 0;				/* 1: skip the BLKRRPART ioctl test at startup */

int leave_last = 0;				/* 1: don't allocate the last cylinder */

int opt_list = 0;

char *save_sector_file = NULL;

char *restore_sector_file = NULL;



/**************************************************************************
*                           局部函数原型                                  *
 **************************************************************************/

static int my_open(char *dev, int rw, int silent);

/**************************************************************************
*                           方法实现                                      *
**************************************************************************/


static char *partnamebf(char *dev, int pno, int lth, int bufsiz, char *bufp)
{
	static char buffer[80];

	char *p;

	int w, wp;

	if (!bufp)
	{
		bufp = buffer;
		bufsiz = sizeof (buffer);
	}

	w = strlen(dev);
	p = "";

	if (isdigit(dev[w - 1]))
		p = "p";

	if (strcmp(dev + w - 4, "disc") == 0)
	{
		w -= 4;
		p = "part";
	}

	wp = strlen(p);

	if (lth)
	{
		sprintf(bufp, "%*.*s%s%-2u", lth - wp - 2, w, dev, p, pno);
	}
	else
	{
		sprintf(bufp, "%.*s%s%-2u", w, dev, p, pno);
	}
	return bufp;
}

char *partname(char *dev, int pno, int lth)
{
	return partnamebf(dev, pno, lth, 0, NULL);
}

static int sseek(const char *dev, unsigned int fd, unsigned long s)
{
	long long in, out;

	in = ((long long) s << 9);
	out = 1;

	if ((out = lseek(fd, in, SEEK_SET)) != in)
	{
		FDISK_DBG("in = %d\n", (int)in);
		perror("llseek");
		FDISK_ERR(_("seek error on %s - cannot seek to %lu\n"), dev, s);
		return 0;
	}

	if (in != out)
	{
		FDISK_ERR(_("seek error: wanted 0x%08x%08x, got 0x%08x%08x\n"),
			  (uint) (in >> 32), (uint) (in & 0xffffffff), (uint) (out >> 32), (uint) (out & 0xffffffff));
		return 0;
	}
	return 1;
}

struct sector
{
	struct sector *next;
	unsigned long sectornumber;
	int to_be_written;
	char data[512];
} *sectorhead = NULL;

static void free_sectors(void)
{
	struct sector *s;

	while (sectorhead)
	{
		s = sectorhead;
		sectorhead = s->next;
		free(s);
	}
}

static struct sector *get_sector(char *dev, int fd, unsigned long sno)
{
	struct sector *s;

	for (s = sectorhead; s; s = s->next)
		if (s->sectornumber == sno)
			return s;

	if (!sseek(dev, fd, sno))
		return 0;

	if (!(s = (struct sector *) malloc(sizeof (struct sector))))
		FDISK_ERR(_("out of memory - giving up\n"));

	if (read(fd, s->data, sizeof (s->data)) != sizeof (s->data))
	{
		if (errno)				/* 0 in case we read past end-of-disk */
			perror("read");
		FDISK_ERR(_("read error on %s - cannot read sector %lu\n"), dev, sno);
		free(s);
		return 0;
	}

	s->next = sectorhead;
	sectorhead = s;
	s->sectornumber = sno;
	s->to_be_written = 0;

	return s;
}

static struct sector *get_sector_ex(const char *dev, int fd, unsigned long sno, struct sector *s)
{
	if (!sseek(dev, fd, sno))
		return 0;

	if (read(fd, s->data, sizeof (s->data)) != sizeof (s->data))
	{
		if (errno)				/* 0 in case we read past end-of-disk */
			perror("read");
		FDISK_ERR(_("read error on %s - cannot read sector %lu\n"), dev, sno);
		return 0;
	}

	s->next = NULL;
	s->sectornumber = sno;
	s->to_be_written = 0;

	return s;
}

static int msdos_signature(struct sector *s)
{
	if (*(unsigned short *) (s->data + 0x1fe) != (unsigned short) ArchSwapLE32(0x55aa))
	{
		FDISK_DBG(_("sector %lu does not have an msdos signature\n"), s->sectornumber);
		return 0;
	}
	return 1;
}

static int write_sectors(char *dev, int fd)
{
	struct sector *s;

	for (s = sectorhead; s; s = s->next)
		if (s->to_be_written)
		{
			if (!sseek(dev, fd, s->sectornumber))
				return 0;
			if (write(fd, s->data, sizeof (s->data)) != sizeof (s->data))
			{
				perror("write");
				FDISK_ERR(_("write error on %s - cannot write sector %lu\n"), dev, s->sectornumber);
				return 0;
			}
			s->to_be_written = 0;
		}
	return 1;
}

static void ulong_to_chars(unsigned long u, char *uu)
{
	int i;

	for (i = 0; i < 4; i++)
	{
		uu[i] = (u & 0xff);
		u >>= 8;
	}
}

static int save_sectors(char *dev, int fdin)
{
	struct sector *s;

	char ss[516];

	int fdout;

	fdout = open(save_sector_file, O_WRONLY | O_CREAT, 0444);
	if (fdout < 0)
	{
		perror(save_sector_file);
		FDISK_ERR(_("cannot open partition sector save file (%s)\n"), save_sector_file);
		return 0;
	}

	for (s = sectorhead; s; s = s->next)
		if (s->to_be_written)
		{
			ulong_to_chars(s->sectornumber, ss);
			if (!sseek(dev, fdin, s->sectornumber))
				return 0;
			if (read(fdin, ss + 4, 512) != 512)
			{
				perror("read");
				FDISK_ERR(_("read error on %s - cannot read sector %lu\n"), dev, s->sectornumber);
				return 0;
			}
			if (write(fdout, ss, sizeof (ss)) != sizeof (ss))
			{
				perror("write");
				FDISK_ERR(_("write error on %s\n"), save_sector_file);
				return 0;
			}
		}
	return 1;
}

static int reread_disk_partition(const char *dev, int fd);

struct geometry
{
	unsigned long cylindersize;
	unsigned long heads, sectors, cylinders;
	unsigned long start;
} B, F, U;

static struct geometry get_geometry(const char *dev, int fd, int silent)
{
	struct hd_geometry g;

	unsigned long size;

	struct geometry R;

	if (ioctl(fd, BLKGETSIZE, &size))
	{
		size = 0;
		if (!silent)
		{
			FDISK_DBG(_("Disk %s: cannot get size\n"), dev);
		}
	}

	if (ioctl(fd, HDIO_GETGEO, &g))
	{
		g.heads = g.sectors = g.cylinders = g.start = 0;
		if (!silent)
		{
			FDISK_DBG(_("Disk %s: cannot get geometry\n"), dev);
		}
	}

	R.heads = g.heads;
	R.sectors = g.sectors;
	R.cylindersize = R.heads * R.sectors;
	R.cylinders = (R.cylindersize ? size / R.cylindersize : 0);
	R.start = g.start;
	return R;
}

static void get_cylindersize(char *dev, int fd, int silent)
{
	struct geometry R;

	R = get_geometry(dev, fd, silent);

	B.heads = (U.heads ? U.heads : R.heads);
	B.sectors = (U.sectors ? U.sectors : R.sectors);
	B.cylinders = (U.cylinders ? U.cylinders : R.cylinders);

	B.cylindersize = B.heads * B.sectors;

	if (R.start && !force)
	{
		FDISK_DBG(_("Warning: start=%lu - this looks like a partition rather than\n"
			   "the entire disk. Using fdisk on it is probably meaningless.\n" "[Use the --force option if you really want this]\n"), R.start);
		return;
	}

	if (R.heads && B.heads != R.heads)
		FDISK_DBG(_("Warning: HDIO_GETGEO says that there are %lu heads\n"), R.heads);
	if (R.sectors && B.sectors != R.sectors)
		FDISK_DBG(_("Warning: HDIO_GETGEO says that there are %lu sectors\n"), R.sectors);
	if (R.cylinders && B.cylinders != R.cylinders && B.cylinders < 65536 && R.cylinders < 65536)
		FDISK_DBG(_("Warning: BLKGETSIZE/HDIO_GETGEO says that there are %lu cylinders\n"), R.cylinders);

	if (B.sectors > 63)
		FDISK_DBG(_("Warning: unlikely number of sectors (%lu) - usually at most 63\n"
			   "This will give problems with all software that uses C/H/S addressing.\n"), B.sectors);
	if (!silent)
		FDISK_DBG(_("Disk %s: %lu cylinders, %lu heads, %lu sectors/track\n"),	//lsj
			   dev, B.cylinders, B.heads, B.sectors);
}

chs zero_chs = { 0, 0, 0 };

typedef struct
{
	unsigned long h, s, c;
} longchs;

longchs zero_longchs;

static chs longchs_to_chs(longchs aa, struct geometry G)
{
	chs a;

	if (aa.h < 256 && aa.s < 64 && aa.c < 1024)
	{
		a.h = aa.h;
		a.s = aa.s | ((aa.c >> 2) & 0xc0);
		a.c = (aa.c & 0xff);
	}
	else if (G.heads && G.sectors)
	{
		a.h = G.heads - 1;
		a.s = G.sectors | 0xc0;
		a.c = 0xff;
	}
	else
		a = zero_chs;
	return a;
}

static longchs chs_to_longchs(chs a)
{
	longchs aa;

	aa.h = a.h;
	aa.s = (a.s & 0x3f);
	aa.c = (a.s & 0xc0);
	aa.c = (aa.c << 2) + a.c;
	return aa;
}

static longchs ulong_to_longchs(unsigned long sno, struct geometry G)
{
	longchs aa;

	if (G.heads && G.sectors && G.cylindersize)
	{
		aa.s = 1 + sno % G.sectors;
		aa.h = (sno / G.sectors) % G.heads;
		aa.c = sno / G.cylindersize;
		return aa;
	}
	else
	{
		return zero_longchs;
	}
}

static chs ulong_to_chs(unsigned long sno, struct geometry G)
{
	return longchs_to_chs(ulong_to_longchs(sno, G), G);
}



static int is_equal_chs(chs a, chs b)
{
	return (a.h == b.h && a.s == b.s && a.c == b.c);
}

/*
static int
chs_ok (chs a, char *v, char *w) {
    longchs aa = chs_to_longchs(a);
    int ret = 1;

    if (is_equal_chs(a, zero_chs))
      return 1;
    if (B.heads && aa.h >= B.heads) {
	FDISK_DBG(_("%s of partition %s has impossible value for head: "
	     "%d (should be in 0-%d)\n"), w, v, aa.h, B.heads-1);
	ret = 0;
    }
    if (B.sectors && (aa.s == 0 || aa.s > B.sectors)) {
	FDISK_DBG(_("%s of partition %s has impossible value for sector: "
	     "%d (should be in 1-%d)\n"), w, v, aa.s, B.sectors);
	ret = 0;
    }
    if (B.cylinders && aa.c >= B.cylinders) {
	FDISK_DBG(_("%s of partition %s has impossible value for cylinders: "
	     "%d (should be in 0-%d)\n"), w, v, aa.c, B.cylinders-1);
	ret = 0;
    }
    return ret;
}
*/

#define EMPTY_PARTITION		0
#define EXTENDED_PARTITION	5
#define WIN98_EXTENDED		0x0f
#define DM6_AUX1PARTITION	0x51
#define DM6_AUX3PARTITION	0x53
#define DM6_PARTITION		0x54
#define EZD_PARTITION		0x55
#define LINUX_SWAP              0x82
#define LINUX_NATIVE	        0x83
#define LINUX_EXTENDED		0x85
#define BSD_PARTITION		0xa5


static const char *sysname(unsigned char type)
{
	struct systypes *s;

	for (s = i386_sys_types; s->name; s++)
		if (s->type == type)
			return _(s->name);
	return _("Unknown");
}

static int is_extended(unsigned char type)
{
	return (type == EXTENDED_PARTITION || type == LINUX_EXTENDED || type == WIN98_EXTENDED);
}

static int is_bsd(unsigned char type)
{
	return (type == BSD_PARTITION);
}

static int copy_to_int(unsigned char *cp)
{
	unsigned int m;

	m = *cp++;
	m += (*cp++ << 8);
	m += (*cp++ << 16);
	m += (*cp++ << 24);
	return m;
}

static void copy_from_int(int m, unsigned char *cp)
{
	*cp++ = (m & 0xff);
	m >>= 8;
	*cp++ = (m & 0xff);
	m >>= 8;
	*cp++ = (m & 0xff);
	m >>= 8;
	*cp++ = (m & 0xff);
}

static void copy_to_part(unsigned char *cp, struct partition *p)
{
	p->bootable = *cp++;
	p->begin_chs.h = *cp++;
	p->begin_chs.s = *cp++;
	p->begin_chs.c = *cp++;
	p->sys_type = *cp++;
	p->end_chs.h = *cp++;
	p->end_chs.s = *cp++;
	p->end_chs.c = *cp++;
	p->start_sect = copy_to_int(cp);
	p->nr_sects = copy_to_int(cp + 4);
}

static void copy_from_part(struct partition *p, unsigned char *cp)
{
	*cp++ = p->bootable;
	*cp++ = p->begin_chs.h;
	*cp++ = p->begin_chs.s;
	*cp++ = p->begin_chs.c;
	*cp++ = p->sys_type;
	*cp++ = p->end_chs.h;
	*cp++ = p->end_chs.s;
	*cp++ = p->end_chs.c;
	copy_from_int(p->start_sect, cp);
	copy_from_int(p->nr_sects, cp + 4);
}


struct part_desc
{
	unsigned long start;
	unsigned long size;
	unsigned long sector, offset;	/* disk location of this info */
	struct partition p;
	struct part_desc *ep;		/* extended partition containing this one */
	int ptype;
#define DOS_TYPE	0
#define BSD_TYPE	1
} zero_part_desc;

static struct part_desc *outer_extended_partition(struct part_desc *p)
{
	while (p->ep)
		p = p->ep;
	return p;
}

static int is_parent(struct part_desc *pp, struct part_desc *p)
{
	while (p)
	{
		if (pp == p)
			return 1;
		p = p->ep;
	}
	return 0;
}

struct disk_desc
{
	struct part_desc partitions[128];
	int partno;
} oldp, newp;

static void add_sector_and_offset(struct disk_desc *z)
{
	int pno;

	struct part_desc *p;

	for (pno = 0; pno < z->partno; pno++)
	{
		p = &(z->partitions[pno]);
		p->offset = 0x1be + (pno % 4) * sizeof (struct partition);
		p->sector = (p->ep ? p->ep->start : 0);
	}
}

static int reread_ioctl(int fd)
{
	if (ioctl(fd, BLKRRPART))
	{
		perror("BLKRRPART");
		return -1;
	}
	return 0;
}

static int is_blockdev(int fd)
{
	struct stat statbuf;

	return (fstat(fd, &statbuf) == 0 && S_ISBLK(statbuf.st_mode));
}

static int reread_disk_partition(const char *dev, int fd)
{
	FDISK_DBG("Re-reading the partition table ...\n");
	fflush(stdout);
	sync();
	sleep(5);					/* superfluous since 1.3.20 */

	if (reread_ioctl(fd) && is_blockdev(fd))
	{
		FDISK_ERR("The command to re-read the partition table failed\nReboot your system now, before using mkfs\n");
		return -1;
	}
	printf("\n");
	return 0;
}

static int index_to_linux(int pno, struct disk_desc *z)
{
	int i, ct = 1;

	struct part_desc *p = &(z->partitions[0]);

	for (i = 0; i < pno; i++, p++)
		if (i < 4 || (p->size > 0 && !is_extended(p->p.sys_type)))
			ct++;
	return ct;
}

static int linux_to_index(int lpno, struct disk_desc *z)
{
	int i, ct = 0;

	struct part_desc *p = &(z->partitions[0]);

	for (i = 0; i < z->partno && ct < lpno; i++, p++)
		if ((i < 4 || (p->size > 0 && !is_extended(p->p.sys_type))) && ++ct == lpno)
			return i;
	return -1;
}


#define F_SECTOR   1
#define F_BLOCK    2
#define F_CYLINDER 3
#define F_MEGABYTE 4

int default_format = F_MEGABYTE;

int specified_format = 0;

int show_extended = 0;

int one_only = 0;

int one_only_pno;

int increment = 0;

static unsigned long unitsize(int format)
{
	default_format = (B.cylindersize ? F_CYLINDER : F_MEGABYTE);
	if (!format && !(format = specified_format))
		format = default_format;

	switch (format)
	{
		default:
		case F_CYLINDER:
			if (B.cylindersize)
				return B.cylindersize;
		case F_SECTOR:
			return 1;
		case F_BLOCK:
			return 2;
		case F_MEGABYTE:
			return 2048;
	}
}

static unsigned long get_disksize(int format)
{
	unsigned long cs = B.cylinders;

	if (cs && leave_last)
		cs--;
	return (cs * B.cylindersize) / unitsize(format);
}


static void out_rounddown(int width, unsigned long n, unsigned long unit, int inc)
{
	printf("%*lu", width, inc + n / unit);
	if (unit != 1)
		putchar((n % unit) ? '+' : ' ');
	putchar(' ');
}

static void out_roundup(int width, unsigned long n, unsigned long unit, int inc)
{
	if (n == (unsigned long) (-1))
		printf("%*s", width, "-");
	else
		printf("%*lu", width, inc + n / unit);
	if (unit != 1)
		putchar(((n + 1) % unit) ? '-' : ' ');
	putchar(' ');
}

static void out_roundup_size(int width, unsigned long n, unsigned long unit)
{
	printf("%*lu", width, (n + unit - 1) / unit);
	if (unit != 1)
		putchar((n % unit) ? '-' : ' ');
	putchar(' ');
}


static void out_partition(char *dev, int format, struct part_desc *p, struct disk_desc *z, struct geometry G)
{
	unsigned long start, end, size;

	int pno, lpno;

	if (!format && !(format = specified_format))
		format = default_format;

	pno = p - &(z->partitions[0]);	/* our index */
	lpno = index_to_linux(pno, z);	/* name of next one that has a name */
	if (pno == linux_to_index(lpno, z))	/* was that us? */
		printf("%s", partname(dev, lpno, 10));	/* yes */
	else if (show_extended)
		printf("    -     ");
	else
		return;
	putchar(dump ? ':' : ' ');

	start = p->start;
	end = p->start + p->size - 1;
	size = p->size;

	if (dump)
	{
		printf(" start=%9lu", start);
		printf(", size=%8lu", size);
		if (p->ptype == DOS_TYPE)
		{
			printf(", Id=%2x", p->p.sys_type);
			if (p->p.bootable == 0x80)
				printf(", bootable");
		}
		printf("\n");
		return;
	}

	if (p->ptype != DOS_TYPE || p->p.bootable == 0)
		printf("   ");
	else if (p->p.bootable == 0x80)
		printf(" * ");
	else
		printf(" ? ");			/* garbage */

	switch (format)
	{
		case F_CYLINDER:
			if (G.cylindersize)
			{
				out_rounddown(6, start, G.cylindersize, increment);
				out_roundup(6, end, G.cylindersize, increment);
				out_roundup_size(6, size, G.cylindersize);
				out_rounddown(8, size, 2, 0);
				break;
			}
		default:
		case F_SECTOR:
			out_rounddown(9, start, 1, increment);
			out_roundup(9, end, 1, increment);
			out_rounddown(9, size, 1, 0);
			break;
		case F_BLOCK:
			out_rounddown(8, start, 2, increment);
			out_roundup(8, end, 2, increment);
			out_rounddown(8, size, 2, 0);
			break;
		case F_MEGABYTE:
			out_rounddown(5, start, 2048, increment);
			out_roundup(5, end, 2048, increment);
			out_roundup_size(5, size, 2048);
			out_rounddown(8, size, 2, 0);
			break;
	}
	if (p->ptype == DOS_TYPE)
	{
		printf(" %2x  %s\n", p->p.sys_type, sysname(p->p.sys_type));
	}
	else
	{
		printf("\n");
	}

	if (!quiet && p->ptype == DOS_TYPE)
	{
		chs a, b;

		longchs aa, bb;

		a = (size ? ulong_to_chs(start, G) : zero_chs);
		b = p->p.begin_chs;
		aa = chs_to_longchs(a);
		bb = chs_to_longchs(b);
		if (a.s && !is_equal_chs(a, b))
			printf(_("\t\tstart: (c,h,s) expected (%ld,%ld,%ld) found (%ld,%ld,%ld)\n"), aa.c, aa.h, aa.s, bb.c, bb.h, bb.s);
		a = (size ? ulong_to_chs(end, G) : zero_chs);
		b = p->p.end_chs;
		aa = chs_to_longchs(a);
		bb = chs_to_longchs(b);
		if (a.s && !is_equal_chs(a, b))
			printf(_("\t\tend: (c,h,s) expected (%ld,%ld,%ld) found (%ld,%ld,%ld)\n"), aa.c, aa.h, aa.s, bb.c, bb.h, bb.s);
		if (G.cylinders && G.cylinders < 1024 && bb.c > G.cylinders)
			printf(_("partition ends on cylinder %ld, beyond the end of the disk\n"), bb.c);
	}
}

static void extended_partition(char *dev, int fd, struct part_desc *ep, struct disk_desc *z)
{
	char *cp;

	struct sector *s;

	unsigned long start, here, next;

	int i, moretodo = 1;

	struct partition p;

	struct part_desc *partitions = &(z->partitions[0]);

	int pno = z->partno;

	here = start = ep->start;

	if (B.cylindersize && start % B.cylindersize)
	{
		if (DOS_extended)
		{
			here = start -= (start % B.cylindersize);
			FDISK_DBG(_("Warning: shifted start of the extd partition from %ld to %ld\n"), ep->start, start);
			FDISK_DBG(_("(For listing purposes only. Do not change its contents.)\n"));
		}
		else
		{
			FDISK_DBG(_("Warning: extended partition does not start at a " "cylinder boundary.\n"));
			FDISK_DBG(_("DOS and Linux will interpret the contents differently.\n"));
		}
	}

	while (moretodo)
	{
		moretodo = 0;

		if (!(s = get_sector(dev, fd, here)))
			break;

		if (!msdos_signature(s))
			break;

		cp = s->data + 0x1be;

		if (pno + 4 >= SIZE(z->partitions))
		{
			FDISK_DBG(_("too many partitions - ignoring those past nr (%d)\n"), pno - 1);
			break;
		}

		next = 0;

		for (i = 0; i < 4; i++, cp += sizeof (struct partition))
		{
			partitions[pno].sector = here;
			partitions[pno].offset = cp - s->data;
			partitions[pno].ep = ep;
			copy_to_part(cp, &p);
			if (is_extended(p.sys_type))
			{
				partitions[pno].start = start + p.start_sect;
				if (next)
					FDISK_DBG(_("tree of partitions?\n"));
				else
					next = partitions[pno].start;	/* follow `upper' branch */
				moretodo = 1;
			}
			else
			{
				partitions[pno].start = here + p.start_sect;
			}
			partitions[pno].size = p.nr_sects;
			partitions[pno].ptype = DOS_TYPE;
			partitions[pno].p = p;
			pno++;
		}
		here = next;
	}

	z->partno = pno;
}

/*!
 * \brief
 * 获取硬盘总容量. 以M为单位
 * 
 * \param dev
 * 硬盘设备节点.
 * 
 * \returns
 * 成功返回硬盘总容量，失败返回0.
 * 
 */
unsigned long get_hd_space(const char *dev)
{
	unsigned long size;

	int fd;

	fd = open(dev, /*O_RDWR */ O_RDONLY);
	if (fd < 0)
	{
		FDISK_ERR("open %s error!\n", dev);
		close(fd);
		return 0L;
	}

	if (ioctl(fd, BLKGETSIZE, &size))
	{
		size = 0L;
		FDISK_ERR("%s: can not get size\n", dev);
	}

	close(fd);
	
	return (unsigned long) size / (1024 * 2);
}


/*!
 * \brief
 * 获取硬盘启动分区.
 * 
 * \param dev
 * 硬盘设备节点.
 * 
 * \returns
 * 成功返回0，失败返回-1.
 * 
 */
int Get_Partition_BootInfo(const char *dev,struct boot_sector *info)
{
	int fd;
	int got;

	fd = open(dev, O_RDONLY);
	if (fd < 0)
	{
		FDISK_ERR("open %s error!\n", dev);
		close(fd);
		return -1;
	}

	got = read(fd,(void *)info,sizeof(struct boot_sector));
	if ((got < 0) || (got != sizeof(struct boot_sector)))
	{
		close(fd);
		return -1;
	}

	close(fd);

	return 0;
}


//获取分区设备节点
void Get_Partition_DevNode(const char *dev, int no_part, char *dev_part, int len)
{
	memset(dev_part, 0, len);

	if (IS_SDCARD(dev))
	{
		snprintf(dev_part, len, "%sp%d", dev, no_part);
	}
	else
	{
		snprintf(dev_part, len, "%s%d", dev, no_part);
	}
}


//修改分区表中分区的文件系统类型
int Change_DevFileSystemType(const char *dev,int partition,uint8_t fs_type)
{
	struct geometry tGeometry;
	Mbr_Sector mbr_info;
	int fd;
	int pos = 0;

	if (partition > 4 || partition < 1)
	{
		FDISK_ERR("partition %d  error  range 1-4\n", partition);
		close(fd);
		return -1;
	}

	fd = open(dev, O_RDWR);
	if (fd < 0)
	{
		FDISK_ERR("open %s fail\n", dev);
		close(fd);
		return -1;
	}

	tGeometry = get_geometry(dev, fd, 1);

	pos = tGeometry.start << 9;
	if (lseek(fd, pos, 0) != pos)
	{
		close(fd);
		FDISK_ERR("lseek %s fail\n", dev);
		return -1;
	}
	
	if (read(fd, (void *)&mbr_info, sizeof(Mbr_Sector)) != sizeof(Mbr_Sector))
	{
		close(fd);
		FDISK_ERR("read %s fail\n", dev);
		return -1;
	}

	if (fs_type != mbr_info.ptb[partition-1].fs_type)
	{
		pos = (tGeometry.start << 9) + 446 + 16 * (partition-1) + 4;
		
		if (lseek(fd, pos, 0) != pos)
		{
			close(fd);
			FDISK_ERR("lseek %s fail\n", dev);
			return -1;
		}

		if (write(fd, (void *)(&fs_type), 1) != 1)
		{
			close(fd);
			FDISK_ERR("write %s fail\n", dev);
			return -1;
		}
	}

	close(fd);
	
	return 0;
}

#define BSD_DISKMAGIC   (0x82564557UL)
#define BSD_MAXPARTITIONS       8
#define BSD_FS_UNUSED           0
typedef unsigned char u8;

typedef unsigned short u16;

typedef unsigned int u32;

struct bsd_disklabel
{
	u32 d_magic;
	char d_junk1[4];
	char d_typename[16];
	char d_packname[16];
	char d_junk2[92];
	u32 d_magic2;
	char d_junk3[2];
	u16 d_npartitions;			/* number of partitions in following */
	char d_junk4[8];
	struct bsd_partition
	{							/* the partition table */
		u32 p_size;				/* number of sectors in partition */
		u32 p_offset;			/* starting sector */
		u32 p_fsize;			/* filesystem basic fragment size */
		u8 p_fstype;			/* filesystem type, see below */
		u8 p_frag;				/* filesystem fragments per block */
		u16 p_cpg;				/* filesystem cylinders per group */
	} d_partitions[BSD_MAXPARTITIONS];	/* actually may be more */
};

static void bsd_partition(char *dev, int fd, struct part_desc *ep, struct disk_desc *z)
{
	struct bsd_disklabel *l;

	struct bsd_partition *bp, *bp0;

	unsigned long start = ep->start;

	struct sector *s;

	struct part_desc *partitions = &(z->partitions[0]);

	int pno = z->partno;

	if (!(s = get_sector(dev, fd, start + 1)))
		return;
	l = (struct bsd_disklabel *) (s->data);
	if (l->d_magic != BSD_DISKMAGIC)
		return;

	bp = bp0 = &l->d_partitions[0];
	while (bp - bp0 <= BSD_MAXPARTITIONS)
	{
		if (pno + 1 >= SIZE(z->partitions))
		{
			FDISK_DBG(_("too many partitions - ignoring those " "past nr (%d)\n"), pno - 1);
			break;
		}
		if (bp->p_fstype != BSD_FS_UNUSED)
		{
			partitions[pno].start = bp->p_offset;
			partitions[pno].size = bp->p_size;
			partitions[pno].sector = start + 1;
			partitions[pno].offset = (char *) bp - (char *) bp0;
			partitions[pno].ep = 0;
			partitions[pno].ptype = BSD_TYPE;
			pno++;
		}
		bp++;
	}
	z->partno = pno;
}

#define MAKE_VERSION(p,q,r)     (65536*(p) + 256*(q) + (r))

static int linux_version_code(void)
{
	struct utsname my_utsname;

	int p, q, r;

	if (uname(&my_utsname) == 0)
	{
		p = atoi(strtok(my_utsname.release, "."));
		q = atoi(strtok(NULL, "."));
		r = atoi(strtok(NULL, "."));
		return MAKE_VERSION(p, q, r);
	}
	return 0;
}

static int msdos_partition(char *dev, int fd, unsigned long start, struct disk_desc *z)
{
	int i;

	char *cp;

	struct partition pt;

	struct sector *s;

	struct part_desc *partitions = &(z->partitions[0]);

	int pno = z->partno;

	int bsd_later = (linux_version_code() >= MAKE_VERSION(2, 3, 40));

	if (!(s = get_sector(dev, fd, start)))
		return 0;

	if (!msdos_signature(s))
		return 0;

	cp = s->data + 0x1be;
	copy_to_part(cp, &pt);

	if (pt.sys_type == DM6_PARTITION || pt.sys_type == EZD_PARTITION || pt.sys_type == DM6_AUX1PARTITION || pt.sys_type == DM6_AUX3PARTITION)
	{
		FDISK_DBG(_("detected Disk Manager - unable to handle that\n"));
		return 0;
	}
	{
		unsigned int sig = *(unsigned short *) (s->data + 2);

		if (sig <= 0x1ae && *(unsigned short *) (s->data + sig) == 0x55aa && (1 & *(unsigned char *) (s->data + sig + 2)))
		{
			FDISK_DBG(_("DM6 signature found - giving up\n"));
			return 0;
		}
	}

	for (pno = 0; pno < 4; pno++, cp += sizeof (struct partition))
	{
		partitions[pno].sector = start;
		partitions[pno].offset = cp - s->data;
		copy_to_part(cp, &pt);
		partitions[pno].start = start + pt.start_sect;
		partitions[pno].size = pt.nr_sects;
		partitions[pno].ep = 0;
		partitions[pno].p = pt;
	}

	z->partno = pno;

	for (i = 0; i < 4; i++)
	{
		if (is_extended(partitions[i].p.sys_type))
		{
			if (!partitions[i].size)
			{
				FDISK_DBG(_("strange..., an extended partition of size 0?\n"));
				continue;
			}
			extended_partition(dev, fd, &partitions[i], z);
		}
		if (!bsd_later && is_bsd(partitions[i].p.sys_type))
		{
			if (!partitions[i].size)
			{
				FDISK_DBG(_("strange..., a BSD partition of size 0?\n"));
				continue;
			}
			bsd_partition(dev, fd, &partitions[i], z);
		}
	}

	if (bsd_later)
	{
		for (i = 0; i < 4; i++)
		{
			if (is_bsd(partitions[i].p.sys_type))
			{
				if (!partitions[i].size)
				{
					FDISK_DBG(_("strange..., a BSD partition of size 0?\n"));
					continue;
				}
				bsd_partition(dev, fd, &partitions[i], z);
			}
		}
	}

	return 1;
}

static int osf_partition(char *dev, int fd, unsigned long start, struct disk_desc *z)
{
	return 0;
}

static int sun_partition(char *dev, int fd, unsigned long start, struct disk_desc *z)
{
	return 0;
}

static int amiga_partition(char *dev, int fd, unsigned long start, struct disk_desc *z)
{
	return 0;
}

static void get_partitions(char *dev, int fd, struct disk_desc *z)
{
	z->partno = 0;

	if (!msdos_partition(dev, fd, 0, z) && !osf_partition(dev, fd, 0, z) && !sun_partition(dev, fd, 0, z) && !amiga_partition(dev, fd, 0, z))
	{
		FDISK_DBG(_("%s: unrecognized partition\n"), dev);
		return;
	}
}

static int write_partitions(char *dev, int fd, struct disk_desc *z)
{
	struct sector *s;

	struct part_desc *partitions = &(z->partitions[0]), *p;

	int pno = z->partno;

	if (no_write)
	{
		FDISK_DBG(_("-n flag was given: Nothing changed\n"));
		return 0;
	}

	for (p = partitions; p < partitions + pno; p++)
	{
		s = get_sector(dev, fd, p->sector);
		if (!s)
			return 0;
		s->to_be_written = 1;
#if 0							//lsj 20080220
		copy_from_part(&(p->p), s->data + p->offset);
		*(unsigned short *) (&(s->data[0x1fe])) = 0xaa55;
#else
		copy_from_part(&(p->p), s->data + p->offset);
		s->data[510] = 0x55;
		s->data[511] = 0xaa;
#endif
	}
	if (save_sector_file)
	{
		if (!save_sectors(dev, fd))
		{
			FDISK_ERR(_("Failed saving the old sectors - aborting\n"));
			return 0;
		}
	}
	if (!write_sectors(dev, fd))
	{
		FDISK_ERR(_("Failed writing the partition on %s\n"), dev);
		return 0;
	}
	return 1;
}


int eof, eob;

struct dumpfld
{
	int fldno;
	char *fldname;
	int is_bool;
} dumpflds[] =
{
	{
	0, "start", 0},
	{
	1, "size", 0},
	{
	2, "Id", 0},
	{
	3, "bootable", 1},
	{
	4, "bh", 0},
	{
	5, "bs", 0},
	{
	6, "bc", 0},
	{
	7, "eh", 0},
	{
	8, "es", 0},
	{
	9, "ec", 0}
};

#define RD_EOF (-1)
#define RD_CMD (-2)

static int read_stdin(unsigned char **fields, unsigned char *line, int fieldssize, int linesize)
{
	unsigned char *lp, *ip;

	int c, fno;

	line[0] = '*';
	line[1] = 0;

	for (fno = 0; fno < fieldssize; fno++)
		fields[fno] = line + 1;
	fno = 0;

	lp = strcpy(line + 2, part_string[part_current]);
	part_current++;

	if (lp == NULL)
	{
		eof = 1;
		return RD_EOF;
	}
	if (!(lp = index(lp, '\n')))
		FDISK_ERR(_("long or incomplete input line - quitting\n"));
	*lp = 0;

	if ((lp = index(line + 2, '#')) != 0)
		*lp = 0;

	if (!strcmp(line + 2, "unit: sectors"))
	{
		specified_format = F_SECTOR;
		return RD_CMD;
	}

	if ((ip = index(line + 2, ':')) != 0)
	{
		struct dumpfld *d;

	  nxtfld:
		ip++;
		while (isspace(*ip))
			ip++;
		if (*ip == 0)
			return fno;
		for (d = dumpflds; d - dumpflds < SIZE(dumpflds); d++)
		{
			if (!strncmp(ip, d->fldname, strlen(d->fldname)))
			{
				ip += strlen(d->fldname);
				while (isspace(*ip))
					ip++;
				if (d->is_bool)
					fields[d->fldno] = line;
				else if (*ip == '=')
				{
					while (isspace(*++ip));
					fields[d->fldno] = ip;
					while (isalnum(*ip))	/* 0x07FF */
						ip++;
				}
				else
					FDISK_ERR(_("input error: `=' expected after %s field\n"), d->fldname);
				if (fno <= d->fldno)
					fno = d->fldno + 1;
				if (*ip == 0)
					return fno;
				if (*ip != ',' && *ip != ';')
					FDISK_ERR(_("input error: unexpected character %c after %s field\n"), *ip, d->fldname);
				*ip = 0;
				goto nxtfld;
			}
		}
		FDISK_ERR(_("unrecognized input: %s\n"), ip);
	}

	lp = ip = line + 2;
	fields[fno++] = lp;
	while ((c = *ip++) != 0)
	{
		if (!lp[-1] && (c == '\t' || c == ' '))
			;
		else if (c == '\t' || c == ' ' || c == ',' || c == ';')
		{
			*lp++ = 0;
			if (fno < fieldssize)
				fields[fno++] = lp;
			continue;
		}
		else
			*lp++ = c;
	}

	if (lp == fields[fno - 1])
		fno--;
	return fno;
}

static int get_ul(char *u, unsigned long *up, unsigned long def, int base)
{
	char *nu;

	if (*u)
	{
		errno = 0;
		*up = strtoul(u, &nu, base);
		if (errno == ERANGE)
		{
			FDISK_ERR(_("number too big\n"));
			return -1;
		}
		if (*nu)
		{
			FDISK_ERR(_("trailing junk after number\n"));
			return -1;
		}
	}
	else
		*up = def;
	return 0;
}

int partitions_in_order = 0;

int all_logicals_inside_outermost_extended = 1;

enum
{ NESTED, CHAINED, ONESECTOR } boxes = NESTED;

static unsigned long first_free(int pno, int is_extended, struct part_desc *ep, int format, unsigned long mid, struct disk_desc *z)
{
	unsigned long ff, fff;

	unsigned long unit = unitsize(format);

	struct part_desc *partitions = &(z->partitions[0]), *pp = 0;

	if (ep && ep->p.sys_type == EMPTY_PARTITION)
		ep = ep->ep;

	if (ep)
	{
		if (boxes == NESTED || (boxes == CHAINED && !is_extended))
			pp = ep;
		else if (all_logicals_inside_outermost_extended)
			pp = outer_extended_partition(ep);
	}
#if 0
	ff = pp ? (pp->start + unit - 1) / unit : 0;
#else
	/* rounding up wastes almost an entire cylinder - round down
	   and leave it to compute_start_sect() to fix the difference */
	ff = pp ? pp->start / unit : 0;
#endif
	if (unit == 1)
		ff++;

  again:
	for (pp = partitions; pp < partitions + pno; pp++)
	{
		if (!is_parent(pp, ep) && pp->size > 0)
		{
			if ((partitions_in_order || pp->start / unit <= ff
				 || (mid && pp->start / unit <= mid)) && (fff = (pp->start + pp->size + unit - 1) / unit) > ff)
			{
				ff = fff;
				goto again;
			}
		}
	}

	return ff;
}

static unsigned long max_length(int pno, int is_extended, struct part_desc *ep, int format, unsigned long start, struct disk_desc *z)
{
	unsigned long fu;

	unsigned long unit = unitsize(format);

	struct part_desc *partitions = &(z->partitions[0]), *pp = 0;

	if (ep && ep->p.sys_type == EMPTY_PARTITION)
		ep = ep->ep;

	if (ep)
	{
		if (boxes == NESTED || (boxes == CHAINED && !is_extended))
			pp = ep;
		else if (all_logicals_inside_outermost_extended)
			pp = outer_extended_partition(ep);
	}
	fu = pp ? (pp->start + pp->size) / unit : get_disksize(format);

	for (pp = partitions; pp < partitions + pno; pp++)
		if (!is_parent(pp, ep) && pp->size > 0 && pp->start / unit >= start && pp->start / unit < fu)
			fu = pp->start / unit;

	return (fu > start) ? fu - start : 0;
}

static int compute_start_sect(struct part_desc *p, struct part_desc *ep)
{
	unsigned long base;

	int inc = (DOS && B.sectors) ? B.sectors : 1;

	int delta;

	if (ep && p->start + p->size >= ep->start + 1)
		delta = p->start - ep->start - inc;
	else if (p->start == 0 && p->size > 0)
		delta = -inc;
	else
		delta = 0;
	if (delta < 0)
	{
		p->start -= delta;
		p->size += delta;
		if (is_extended(p->p.sys_type) && boxes == ONESECTOR)
			p->size = inc;
		else if ((int) (p->size) <= 0)
		{
			//FDISK_DBG(_("no room for partition descriptor\n"));
			//return 0;
		}
	}
	base = (!ep ? 0 : (is_extended(p->p.sys_type) ? outer_extended_partition(ep) : ep)->start);
	p->ep = ep;
	if (p->p.sys_type == EMPTY_PARTITION && p->size == 0)
	{
		p->p.start_sect = 0;
		p->p.begin_chs = zero_chs;
		p->p.end_chs = zero_chs;
	}
	else
	{
		p->p.start_sect = p->start - base;
		p->p.begin_chs = ulong_to_chs(p->start, B);
		p->p.end_chs = ulong_to_chs(p->start + p->size - 1, B);
	}
	p->p.nr_sects = p->size;
	return 1;
}

static int build_surrounding_extended(struct part_desc *p, struct part_desc *ep, struct disk_desc *z)
{
	int inc = (DOS && B.sectors) ? B.sectors : 1;

	int format = F_SECTOR;

	struct part_desc *p0 = &(z->partitions[0]), *eep = ep->ep;

	if (boxes == NESTED)
	{
		ep->start = first_free(ep - p0, 1, eep, format, p->start, z);
		ep->size = max_length(ep - p0, 1, eep, format, ep->start, z);
		if (ep->start > p->start || ep->start + ep->size < p->start + p->size)
		{
			FDISK_DBG(_("cannot build surrounding extended partition\n"));
			return 0;
		}
	}
	else
	{
		ep->start = p->start;
		if (boxes == CHAINED)
			ep->size = p->size;
		else
			ep->size = inc;
	}

	ep->p.nr_sects = ep->size;
	ep->p.bootable = 0;
	ep->p.sys_type = EXTENDED_PARTITION;
	if (!compute_start_sect(ep, eep) || !compute_start_sect(p, ep))
	{
		ep->p.sys_type = EMPTY_PARTITION;
		ep->size = 0;
		return 0;
	}

	return 1;
}

static int read_line(int pno, struct part_desc *ep, char *dev, int interactive, struct disk_desc *z)
{
	unsigned char line[1000];

	unsigned char *fields[11];

	int fno, pct = pno % 4;

	struct part_desc p, *orig;

	unsigned long ff, ff1, ul, ml, ml1, def;

	int format, lpno, is_extd;

	if (eof || eob)
		return -1;

	lpno = index_to_linux(pno, z);

	if (interactive)
	{
		if (pct == 0 && (show_extended || pno == 0))
			;
		FDISK_DBG("%s:\n", partname(dev, lpno, 10));
	}

	do
	{
		fno = read_stdin(fields, line, SIZE(fields), SIZE(line));
	}
	while (fno == RD_CMD || (fno == 0 && !interactive));
	if (fno == RD_EOF)
	{
		return -1;
	}
	else if (fno > 10 && *(fields[10]) != 0)
	{
		FDISK_ERR(_("too many input fields\n"));
		return 0;
	}

	if (fno == 1 && !strcmp(fields[0], "."))
	{
		eob = 1;
		return -1;
	}

	format = 0;
	if (B.cylindersize && specified_format == F_MEGABYTE)
		format = F_CYLINDER;

	orig = (one_only ? &(oldp.partitions[pno]) : 0);

	p = zero_part_desc;
	p.ep = ep;

	is_extd = 0;
	if (fno == 0)
	{							/* empty line */
		if (orig && is_extended(orig->p.sys_type))
			is_extd = 1;
		ff = first_free(pno, is_extd, ep, format, 0, z);
		ml = max_length(pno, is_extd, ep, format, ff, z);
		if (ml == 0 && is_extd == 0)
		{
			is_extd = 1;
			ff = first_free(pno, is_extd, ep, format, 0, z);
			ml = max_length(pno, is_extd, ep, format, ff, z);
		}
		if (ml == 0 && pno >= 4)
		{
			FDISK_DBG(_("No room for more\n"));
			return -1;
		}
	}
	if (fno < 3 || !*(fields[2]))
		ul = orig ? orig->p.sys_type : (is_extd || (pno > 3 && pct == 1 && show_extended)) ? EXTENDED_PARTITION : LINUX_NATIVE;
	else if (!strcmp(fields[2], "L"))
		ul = LINUX_NATIVE;
	else if (!strcmp(fields[2], "S"))
		ul = LINUX_SWAP;
	else if (!strcmp(fields[2], "E"))
		ul = EXTENDED_PARTITION;
	else if (!strcmp(fields[2], "X"))
		ul = LINUX_EXTENDED;
	else if (get_ul(fields[2], &ul, LINUX_NATIVE, 16))
		return 0;
	if (ul > 255)
	{
		FDISK_DBG(_("Illegal type\n"));
		return 0;
	}
	p.p.sys_type = ul;
	is_extd = is_extended(ul);

	ff = first_free(pno, is_extd, ep, format, 0, z);
	ff1 = ff * unitsize(format);
	def = orig ? orig->start : (pno > 4 && pct > 1) ? 0 : ff1;
	if (fno < 1 || !*(fields[0]))
		p.start = def;
	else
	{
		if (get_ul(fields[0], &ul, def / unitsize(0), 0))
			return 0;
		p.start = ul * unitsize(0);
		p.start -= (p.start % unitsize(format));
	}

	ml = max_length(pno, is_extd, ep, format, p.start / unitsize(format), z);
	ml1 = ml * unitsize(format);
	def = orig ? orig->size : (pno > 4 && pct > 1) ? 0 : ml1;
	if (fno < 2 || !*(fields[1]))
		p.size = def;
	else
	{
		if (get_ul(fields[1], &ul, def / unitsize(0), 0))
			return 0;
		p.size = ul * unitsize(0) + unitsize(format) - 1;
		p.size -= (p.size % unitsize(format));
	}
	if (p.size > ml1)
	{
		FDISK_DBG(_("Warning: given size (%lu) exceeds max allowable size (%lu)\n"), (p.size + unitsize(0) - 1) / unitsize(0), ml1 / unitsize(0));
		if (!force)
			return 0;
	}
	if (p.size == 0 && pno >= 4 && (fno < 2 || !*(fields[1])))
	{
		FDISK_DBG(_("Warning: empty partition\n"));
		if (!force)
			return 0;
	}
	p.p.nr_sects = p.size;

	if (p.size == 0 && !orig)
	{
		if (fno < 1 || !*(fields[0]))
			p.start = 0;
		if (fno < 3 || !*(fields[2]))
			p.p.sys_type = EMPTY_PARTITION;
	}

	if (p.start < ff1 && p.size > 0)
	{
		FDISK_DBG(_("Warning: bad partition start (earliest %lu)\n"), (ff1 + unitsize(0) - 1) / unitsize(0));
		if (!force)
			return 0;
	}

	if (fno < 4 || !*(fields[3]))
		ul = (orig ? orig->p.bootable : 0);
	else if (!strcmp(fields[3], "-"))
		ul = 0;
	else if (!strcmp(fields[3], "*") || !strcmp(fields[3], "+"))
		ul = 0x80;
	else
	{
		FDISK_DBG(_("unrecognized bootable flag - choose - or *\n"));
		return 0;
	}
	p.p.bootable = ul;

	if (ep && ep->p.sys_type == EMPTY_PARTITION)
	{
		if (!build_surrounding_extended(&p, ep, z))
			return 0;
	}
	else if (!compute_start_sect(&p, ep))
		return 0;

	{
		longchs aa = chs_to_longchs(p.p.begin_chs), bb;

		if (fno < 5)
		{
			bb = aa;
		}
		else if (fno < 7)
		{
			FDISK_DBG(_("partial c,h,s specification?\n"));
			return 0;
		}
		else if (get_ul(fields[4], &bb.c, aa.c, 0) || get_ul(fields[5], &bb.h, aa.h, 0) || get_ul(fields[6], &bb.s, aa.s, 0))
			return 0;
		p.p.begin_chs = longchs_to_chs(bb, B);
	}
	{
		longchs aa = chs_to_longchs(p.p.end_chs), bb;

		if (fno < 8)
		{
			bb = aa;
		}
		else if (fno < 10)
		{
			FDISK_DBG(_("partial c,h,s specification?\n"));
			return 0;
		}
		else if (get_ul(fields[7], &bb.c, aa.c, 0) || get_ul(fields[8], &bb.h, aa.h, 0) || get_ul(fields[9], &bb.s, aa.s, 0))
			return 0;
		p.p.end_chs = longchs_to_chs(bb, B);
	}

	if (pno > 3 && p.size && show_extended && p.p.sys_type != EMPTY_PARTITION && (is_extended(p.p.sys_type) != (pct == 1)))
	{
		FDISK_DBG(_("Extended partition not where expected\n"));
		if (!force)
			return 0;
	}

	z->partitions[pno] = p;
	if (pno >= z->partno)
		z->partno += 4;			/* reqd for out_partition() */

	if (interactive)
		out_partition(dev, 0, &(z->partitions[pno]), z, B);

	return 1;
}

static int read_partition(char *dev, int interactive, int pno, struct part_desc *ep, struct disk_desc *z)
{
	struct part_desc *p = &(z->partitions[pno]);

	int i;

	if (one_only)
	{
		*p = oldp.partitions[pno];
		if (one_only_pno != pno)
			goto ret;
	}
	else if (!show_extended && pno > 4 && pno % 4)
		goto ret;

	while (!(i = read_line(pno, ep, dev, interactive, z)))
		if (!interactive)
			FDISK_ERR(_("bad input\n"));

	if (i < 0)
	{
		p->ep = ep;
		return 0;
	}

  ret:
	p->ep = ep;
	if (pno >= z->partno)
		z->partno += 4;
	return 1;
}

static void read_partition_chain(char *dev, int interactive, struct part_desc *ep, struct disk_desc *z)
{
	int i, base;

	eob = 0;
	while (1)
	{
		base = z->partno;
		if (base + 4 > SIZE(z->partitions))
		{
			FDISK_ERR(_("too many partitions\n"));
			break;
		}
		for (i = 0; i < 4; i++)
			if (!read_partition(dev, interactive, base + i, ep, z))
				return;
		for (i = 0; i < 4; i++)
		{
			ep = &(z->partitions[base + i]);
			if (is_extended(ep->p.sys_type) && ep->size)
				break;
		}
		if (i == 4)
		{
			if (one_only || show_extended)
				break;
			ep = &(z->partitions[base + 1]);
			if (ep->size || ep->p.sys_type != EMPTY_PARTITION)
				break;
		}
	}
}

static void read_input(char *dev, int interactive, struct disk_desc *z)
{
	int i;

	struct part_desc *partitions = &(z->partitions[0]), *ep;

	for (i = 0; i < SIZE(z->partitions); i++)
		partitions[i] = zero_part_desc;
	z->partno = 0;

	eof = 0;
	if (disk_parts > 4)
	{
		for (i = 0; i < 4; i++)
			read_partition(dev, interactive, i, 0, z);
		for (i = 0; i < 4; i++)
		{
			ep = partitions + i;
			if (is_extended(ep->p.sys_type) && ep->size)
				read_partition_chain(dev, interactive, ep, z);
		}
	}
	else
	{
		for (i = 0; i < disk_parts; i++)
			read_partition(dev, interactive, i, 0, z);

	}
	add_sector_and_offset(z);
}



#define PRINT_ID 0400
#define CHANGE_ID 01000

static const struct option long_opts[] = {
	{"change-id", no_argument, NULL, 'c' + CHANGE_ID},
	{"print-id", no_argument, NULL, 'c' + PRINT_ID},
	{"id", no_argument, NULL, 'c'},
	{"dump", no_argument, NULL, 'd'},
	{"force", no_argument, NULL, 'f'},
	{"show-geometry", no_argument, NULL, 'g'},
	{"increment", no_argument, NULL, 'i'},
	{"list", no_argument, NULL, 'l'},
	{"quiet", no_argument, NULL, 'q'},
	{"show-size", no_argument, NULL, 's'},
	{"unit", required_argument, NULL, 'u'},
	{"version", no_argument, NULL, 'v'},
	{"show-extended", no_argument, NULL, 'x'},
	{"help", no_argument, NULL, '?'},
	{"one-only", no_argument, NULL, '1'},
	{"cylinders", required_argument, NULL, 'C'},
	{"heads", required_argument, NULL, 'H'},
	{"sectors", required_argument, NULL, 'S'},
	{"activate", optional_argument, NULL, 'A'},
	{"DOS", no_argument, NULL, 'D'},
	{"DOS-extended", no_argument, NULL, 'E'},
	{"Linux", no_argument, NULL, 'L'},
	{"re-read", no_argument, NULL, 'R'},
	{"list-types", no_argument, NULL, 'T'},
	{"unhide", optional_argument, NULL, 'U'},
	{"no-reread", no_argument, NULL, 160},
	{"IBM", no_argument, NULL, 161},
	{"leave-last", no_argument, NULL, 161},
	{"in-order", no_argument, NULL, 128},
	{"not-in-order", no_argument, NULL, 129},
	{"inside-outer", no_argument, NULL, 130},
	{"not-inside-outer", no_argument, NULL, 131},
	{"nested", no_argument, NULL, 132},
	{"chained", no_argument, NULL, 133},
	{"onesector", no_argument, NULL, 134},
	{NULL, 0, NULL, 0}
};

static int my_open(char *dev, int rw, int silent)
{
	int fd, mode;

	mode = (rw ? O_RDWR : O_RDONLY);
	fd = open(dev, mode);
	if (fd < 0 && !silent)
	{
		perror(dev);
		FDISK_ERR(_("cannot open %s %s\n"), dev, rw ? _("read-write") : _("for reading"));
	}
	return fd;
}

static int do_fdisk(char *dev)
{
	int fd= -1;

	struct stat statbuf;

	int interactive = isatty(0);

	struct disk_desc *z;

	if (stat(dev, &statbuf) < 0)
	{
		perror(dev);
		FDISK_ERR(_("Fatal error: cannot find %s\n"), dev);
		return -1;
	}
	if (!S_ISBLK(statbuf.st_mode))
	{
		FDISK_DBG(_("Warning: %s is not a block device\n"), dev);
		no_reread = 1;
	}
	
	fd = my_open(dev, !no_write, 0);

	if (!no_write && !no_reread)
	{
		FDISK_DBG(_("Checking that no-one is using this disk right now ...\n"));
		if (reread_ioctl(fd))
		{
			FDISK_INFO(_("\nThis disk is currently in use - repartitioning is probably a bad idea.\n"
					 "Umount all file systems, and swapoff all swap partitions on this disk.\n"
					 "Use the --no-reread flag to suppress this check.\n"));
			if (!force)
			{
				FDISK_DBG(_("Use the --force flag to overrule all checks.\n"));
				return -1;
			}
		}
		else
		{
			FDISK_INFO(_("OK\n"));
		}
	}

	z = &oldp;

	free_sectors();
	get_cylindersize(dev, fd, 0);
	get_partitions(dev, fd, z);

	if (one_only && (one_only_pno = linux_to_index(one_only, z)) < 0)
	{
		FDISK_ERR(_("Partition %d does not exist, cannot change it\n"), one_only);
		return -1;
    }
	z = &newp;

	while (1)
	{
		read_input(dev, interactive, z);
		break;
	}

	if (write_partitions(dev, fd, z))
	{
		FDISK_INFO(_("Successfully wrote the new partition table\n\n"));
	}else{
		exit_status = 1;
	}

	reread_disk_partition(dev, fd);	
	close(fd);
	sync();						/* superstition */
	sleep(1);
	
	return 0;
}

static void revert(struct Part_Desc *ptPartDesc, struct partition *p, struct geometry tGeometry, int externpartiton_num)
{
	unsigned int S = tGeometry.sectors;

	unsigned int zm = tGeometry.cylindersize;

	unsigned int nStart_Sec = ptPartDesc->nStart_Sec * zm;

#ifdef __FAT32__
	if (nStart_Sec < 16)
	{
		nStart_Sec = 16;
	}
#else
	if (nStart_Sec < 63)
	{
		nStart_Sec = 63;
	}
#endif

	unsigned int nEnd_Sec = ptPartDesc->nEnd_Sec * zm;

	unsigned int C = nStart_Sec / zm;

	if (ptPartDesc->Type == 0)
	{
		return;
	}

	p->bootable = 0;

	p->begin_chs.h = (nStart_Sec % zm) / S;

	p->begin_chs.s = (((nStart_Sec % zm) % S + 1) & 0x3f);
	p->begin_chs.s |= ((C & 0x300) >> 2);

	p->begin_chs.c = (C & 0xff);

	p->sys_type = ptPartDesc->Type;

	p->end_chs.h = ((nEnd_Sec - 1) % zm) / S;

	p->end_chs.s = ((((nEnd_Sec - 1) % zm) % S + 1) & 0x3f);
	p->end_chs.s |= ((((nEnd_Sec - 1) / zm) & 0x300) >> 2);

	p->end_chs.c = (((nEnd_Sec - 1) / zm) & 0xff);

	if (externpartiton_num)
	{
		p->start_sect = nStart_Sec - externpartiton_num + 0x3f;
	}
	else
	{
		p->start_sect = nStart_Sec;
	}

	if (externpartiton_num)
	{
		p->nr_sects = nEnd_Sec - nStart_Sec - 0x3f;
	}
	else
	{
		p->nr_sects = nEnd_Sec - nStart_Sec;
	}

	if (ptPartDesc->nStart_Sec > 1023)
	{
#ifdef __FAT32__
		p->begin_chs.h = 0x3;
		p->begin_chs.s = 0xd0;
		p->begin_chs.c = 0xff;
#else
		p->begin_chs.h = 0xfe;
		p->begin_chs.s = 0xff;
		p->begin_chs.c = 0xff;
#endif
	}

	if (ptPartDesc->nEnd_Sec > 1023)
	{
#ifdef __FAT32__
		p->end_chs.h = 0x3;
		p->end_chs.s = 0xd0;
		p->end_chs.c = 0xff;
#else
		p->end_chs.h = 0xfe;
		p->end_chs.s = 0xff;
		p->end_chs.c = 0xff;
#endif
	}

}

static void NextPartitionInfo(struct Part_Desc *ptPartDesc, struct partition *p, struct geometry geometry, int externpartiton_num)
{
	unsigned int S = geometry.sectors;

	unsigned int zm = geometry.cylindersize;

	unsigned int nStart_Sec = ptPartDesc->nStart_Sec * geometry.cylindersize;

	FDISK_DBG("geometry.start = %lu\n", geometry.start);
	if (nStart_Sec < 63)
	{
		nStart_Sec = 63;
	}
	nStart_Sec += 1;
	unsigned int nEnd_Sec = ptPartDesc->nEnd_Sec * geometry.cylindersize;

	//externpartiton_num = externpartiton_num* geometry.cylindersize;
	unsigned int C = nStart_Sec / zm;

	FDISK_DBG("ptPartDesc->nStart_Sec = %d, ptPartDesc->nEnd_Sec = %d\n", ptPartDesc->nStart_Sec, ptPartDesc->nEnd_Sec);
	FDISK_DBG("nStart_Sec = %d,nEnd_Sec = %d\n", nStart_Sec, nEnd_Sec);
	FDISK_DBG("externpartiton_num = %d\n", externpartiton_num);

	if (ptPartDesc->Type == 0)
	{
		return;
	}
	p->bootable = 0;

	p->begin_chs.h = (nStart_Sec % zm) / S;

	p->begin_chs.s = (((nStart_Sec % zm) % S + 1) & 0x3f);
	p->begin_chs.s |= ((C & 0x300) >> 2);

	p->begin_chs.c = (C & 0xff);

	p->sys_type = 0x5;			//ptPartDesc->Type;

	p->end_chs.h = ((nEnd_Sec - 1) % zm) / S;

	p->end_chs.s = ((((nEnd_Sec - 1) % zm) % S + 1) & 0x3f);
	FDISK_DBG("p->end_chs.s  = %x\n", p->end_chs.s);
	p->end_chs.s |= ((((nEnd_Sec - 1) / zm) & 0x300) >> 2);
	FDISK_DBG("p->end_chs.s  = %x\n", p->end_chs.s);

	p->end_chs.c = (((nEnd_Sec - 1) / zm) & 0xff);

	p->start_sect = (nStart_Sec - externpartiton_num - 1);

	p->nr_sects = (nEnd_Sec - nStart_Sec);

	if (ptPartDesc->nStart_Sec > 1023)
	{
		p->begin_chs.h = 0xfe;
		p->begin_chs.s = 0xff;
		p->begin_chs.c = 0xff;
	}
	if (ptPartDesc->nEnd_Sec > 1023)
	{
		p->end_chs.h = 0xfe;
		p->end_chs.s = 0xff;
		p->end_chs.c = 0xff;
	}
}

static void printf_partition_info(struct Part_Desc *ptPartDesc)
{
#if 0
	unsigned int nPart;

	for (nPart = 0; nPart < MAX_DISK_PARTITIONS; nPart++, ptPartDesc++)
	{
		if (ptPartDesc->Type != 0)
		{
			FDISK_INFO("nPart = %d\n", nPart + 1);
			FDISK_INFO("Type = %x\n", ptPartDesc->Type);
			FDISK_INFO("nStart_Sec = %d\n", ptPartDesc->nStart_Sec + 1);
			FDISK_INFO("nEnd_Sec = %d\n", ptPartDesc->nEnd_Sec);
			FDISK_INFO("dwSector = 0x%x\n\n", ptPartDesc->dwSector);
		}
	}
#endif
}

static void printfinfo(struct partition p)
{
	FDISK_DBG("\n p->begin_chs.h = %x\n p->begin_chs.s = %x\n p->begin_chs.c = %x\n p->sys_type = %x\n p->end_chs.h = %x\n p->end_chs.s = %x\n p->end_chs.c = %x\n p->start_sect = %x\n p->nr_sects = %x\n",
		 p.begin_chs.h, p.begin_chs.s, p.begin_chs.c, p.sys_type, p.end_chs.h, p.end_chs.s, p.end_chs.c, p.start_sect, p.nr_sects);
}

/*!
 * \brief
 * 将磁盘描述信息结构体中的分区描述信息写入对应的磁盘扇区
 *
 * \param pcDev
 * 设备节点名
 *
 * \param fd
 * 打开的设备描述符.
 *
 * \param ptDiskDesc
 * 磁盘描述信息结构体指针.
 *
 * \return
 * 成功返回1, 失败返回0.
 */
static int write_partition(const char *pcDev, int fd, struct Disk_Desc *ptDiskDesc, struct geometry tGeometry)
{
	struct Part_Desc *ptPartDesc = &(ptDiskDesc->atPartitions[0]);

	struct sector *ptSec;

	struct sector Sec;

	struct partition tPartition;

	char *cp;

	unsigned int iPartNum;		//ptDiskDesc->iPartNum;

	unsigned int iCount = 0;

	unsigned int iPartNo;

	unsigned int externpartiton_num = 0;

	FDISK_INFO("******************* enter\n");

#ifdef __FAT32__
	iPartNum = 4;
#else
	
#ifdef __EXT3__
	
	  iPartNum = 4;
	
#else
	iPartNum = MAX_DISK_PARTITIONS;

	if (ptPartDesc[1].Type == 0x5)
	{
		externpartiton_num = ptPartDesc[iPartNo].nStart_Sec;
		FDISK_INFO("***** externpartiton_num = %d\n", externpartiton_num);
	}
#endif
#endif

	// 将分区表写入磁盘扇区(一个分区表含有4个分区表项)
	for (iPartNo = 0; iPartNo < iPartNum; iPartNo++)
	{
		/* 获取磁盘分区表所在的扇区 */
		ptSec = get_sector_ex(pcDev, fd, ptPartDesc[iPartNo].dwSector, &Sec);
		if (NULL == ptSec)
		{
			FDISK_ERR("%s get_sector_ex fail, iPartNo = %u, dwSector = %u\n", pcDev, iPartNo, ptPartDesc[iPartNo].dwSector);
			return 0;
		}

		FDISK_INFO("#################################\n");
		FDISK_INFO("iPartNo = %d\n", iPartNo);

		cp = ptSec->data + 0x1be;

		// 将分区表项中的信息存入扇区数据中
		for (iCount = iPartNo; iCount < iPartNo + 4; iCount++, cp += sizeof (struct partition))
		{
			memset(&tPartition, 0, sizeof (struct partition));

			FDISK_INFO("iPartNo = %d, iCount = %d\n", iPartNo, iCount);

			if (iCount < 4)
			{
				revert(ptPartDesc + iCount, &tPartition, tGeometry, 0);
				printfinfo(tPartition);
				copy_from_part(&tPartition, cp);
			}
			else
			{					//处理分区链表
				FDISK_INFO("================== 0\n");

				memset(cp, 0, 64);

				if ((ptPartDesc + iCount)->Type != 0)
				{
					revert(ptPartDesc + iCount, &tPartition, tGeometry, externpartiton_num * tGeometry.cylindersize);
					printfinfo(tPartition);
					copy_from_part(&tPartition, cp);
				}

				ptPartDesc[iPartNo].dwSector = externpartiton_num * tGeometry.cylindersize;
				FDISK_INFO("ptPartDesc[%d].dwSector = %d\n", iPartNo, ptPartDesc[iPartNo].dwSector);

				if ((ptPartDesc + iCount + 1)->Type != 0)
				{
					NextPartitionInfo(ptPartDesc + iCount + 1, &tPartition, tGeometry, ptPartDesc[1].nStart_Sec * tGeometry.cylindersize);
					printfinfo(tPartition);
					copy_from_part(&tPartition, cp + sizeof (struct partition));
					printfinfo(tPartition);
					externpartiton_num = (ptPartDesc[iCount + 1].nStart_Sec);
					FDISK_INFO("*****externpartiton_num= %d\n", externpartiton_num);
				}
				break;
			}
		}

		FDISK_INFO("================== 1\n");

		if (ptSec->data[446 + 4] == 0 && iPartNo != 4)
		{
			FDISK_DBG("ptSec->data[446 + 4] == 0 && iPartNo != 4\n");
			break;
		}

		// 分区表的识别标志
		ptSec->data[510] = 0x55;
		ptSec->data[511] = 0xaa;

#if 0
		{
			FDISK_INFO("================== 2\n");

			int i, data;

			for (i = 0; i < 4; i++)
			{
				FDISK_INFO(" **** %s nPart = %d\n", pcDev, i);
				for (data = 0; data < 16; data++)
				{
					FDISK_INFO(" %x", ptSec->data[446 + i * 16 + data]);
					if (data == 0 || data == 3 || data == 4 || data == 7 || data == 11)
					{
						printf("\n");
					}
				}
				printf("\n");
			}
			FDISK_INFO(" data[510]:%x data[511]:%x \n", ptSec->data[510], ptSec->data[511]);
			FDISK_INFO("ptPartDesc[%u].dwSector = %u\n", iPartNo, ptPartDesc[iPartNo].dwSector);

			FDISK_INFO("================== 3\n");
		}
#endif

		// 将文件读取位置定位到相应的扇区
		if (!sseek(pcDev, fd, ptPartDesc[iPartNo].dwSector))
		{
			FDISK_ERR("%s sseek to sector %d fail\n", pcDev, ptPartDesc[iPartNo].dwSector);
			return -1;
		}

		// 将数据写入扇区
		if (write(fd, ptSec->data, sizeof (ptSec->data)) != sizeof (ptSec->data))
		{
			FDISK_ERR("write date to sector fail\n");
			return -1;
		}

		if (iPartNo == 0)
		{
			iPartNo = iPartNo + 3;
			FDISK_INFO("ptPartDesc[%u].dwSector = %u\n", iPartNo, ptPartDesc[iPartNo].dwSector);
		}

		FDISK_INFO("================== to next partition\n\n");
	}

	// 释放扇区节点信息
	while (NULL != sectorhead)
	{
		ptSec = sectorhead->next;
		free(sectorhead);
		sectorhead = ptSec;
	}

	FDISK_DBG("******************* exit 0\n");
	return 0;
}

/*!
 * \brief
 * 读取扩展分区表
 *
 * \param dev
 * 设备节点名
 *
 * \param struct geometry tGeometry 分区的几何信息
 *
 *
 * \return
 * 成功读取分区的个数
 */
static int ReadExternPartiton(const char *dev, int fd, int externstart, struct geometry tGeometry, struct Part_Desc *ptPartDesc)
{
	unsigned int nPart;

	unsigned int start = 0;

	//除去4个主分区，剩下的为逻辑分区
	for (nPart = 0; nPart < (MAX_DISK_PARTITIONS - 4);)
	{
		unsigned int value;

		struct sector *ptSec;

		struct sector Sec;

		ptSec = get_sector_ex(dev, fd, start + externstart, &Sec);
		if (NULL == ptSec)
		{
			FDISK_ERR("%s get sector data fail\n", dev);
			break;
		}

#if 0
		if (ptSec->data[446 + 4] != 0)
		{
			int i;

			FDISK_INFO("externstart = 0x%x, start = 0x%x\n", externstart, start);
			for (i = 0; i < 2; i++)
			{
				int j;

				FDISK_INFO("**** %s nPart = %d, i = %d\n", dev, nPart + 5, i);

				for (j = 0; j < 16; j++)
				{
					FDISK_INFO(" %x", ptSec->data[446 + i * 16 + j]);
					if (j == 0 || j == 3 || j == 4 || j == 7 || j == 11)
					{
						printf("\n");
					}
				}
				printf("\n");
			}
		}
		FDISK_INFO("data[510]:%X data[511]:%X \n", ptSec->data[510], ptSec->data[511]);
#endif

		//读取第一个部分
		if (ptSec->data[446 + 4] != 0)
		{
			//保存分区类型
			ptPartDesc->Type = ptSec->data[446 + 4];

			memcpy(&value, (&ptSec->data[446 + 8]), sizeof (int));

			ptPartDesc->nStart_Sec = ArchSwapLE32(value) + start + externstart;

			memcpy(&value, (&ptSec->data[446 + 12]), sizeof (int));

			ptPartDesc->nEnd_Sec = ptPartDesc->nStart_Sec + ArchSwapLE32(value);

			ptPartDesc->nStart_Sec /= tGeometry.cylindersize;
			ptPartDesc->nEnd_Sec /= tGeometry.cylindersize;

			ptPartDesc->dwSector = ptPartDesc->nStart_Sec * tGeometry.cylindersize;

			nPart++;
			ptPartDesc++;
		}

		//第二部分
		if (ptSec->data[446 + 16 + 4] != 0)
		{
			//保存下一个逻辑分区开始地址

			memcpy(&value, &ptSec->data[446 + 16 + 8], sizeof (int));
			start = ArchSwapLE32(value);

			continue;
		}

		break;
	}

	FDISK_INFO("%s has %d logical partitions\n", dev, nPart);
	return nPart;
}

/*!
* \brief
* 读取分区表
*
* \param pcDev
* 设备节点名
*
* \param struct geometry geometry 分区的几何信息
*
*
* \return
* 成功读取分区的个数
*/
static int ReadPartition(const char *dev, int fd, struct geometry tGeometry, struct Disk_Desc *ptDiskDesc)
{
	struct Part_Desc *ptPartDesc;

	unsigned int nPart;

	unsigned int iPartNum;

	struct sector *ptSec = NULL;

	struct sector Sec;

	FDISK_INFO("enter %s\n", __FUNCTION__);

	memset(ptDiskDesc, 0, sizeof (struct Disk_Desc));

	//读主分区表
	ptSec = get_sector_ex(dev, fd, tGeometry.start, &Sec);
	if (NULL == ptSec)
	{
		FDISK_ERR("%s get sector data fail\n", dev);
		return 0;
	}

#if 0
	for (nPart = 0; nPart < 4; nPart++)
	{
		int info;

		FDISK_INFO(" **** %s nPart = %d\n", dev, nPart + 1);
		for (info = 0; info < 16; info++)
		{
			FDISK_INFO(" %x", ptSec->data[446 + nPart * 16 + info]);
			if (info == 0 || info == 3 || info == 4 || info == 7 || info == 11)
			{
				printf("\n");
			}
		}
		printf("\n");
	}
	FDISK_INFO("data[510]:%x data[511]:%x \n", ptSec->data[510], ptSec->data[511]);
#endif

	ptPartDesc = &(ptDiskDesc->atPartitions[0]);

	for (nPart = 0; nPart < 4; nPart++)
	{
		if (ptSec->data[446 + nPart * 16 + 4] != 0)	/* 不是空分区 */
		{
			unsigned int value;

			FDISK_DBG("\n nPart = %d\n", nPart + 1);

			(ptPartDesc + nPart)->Type = ptSec->data[446 + nPart * 16 + 4];

			memcpy(&value, &ptSec->data[446 + nPart * 16 + 8], sizeof (int));
			(ptPartDesc + nPart)->nStart_Sec = ArchSwapLE32(value);

			memcpy(&value, &ptSec->data[446 + nPart * 16 + 12], sizeof (int));
			(ptPartDesc + nPart)->nEnd_Sec = (ptPartDesc + nPart)->nStart_Sec + ArchSwapLE32(value);

			/* 如果是扩展分区 */
			if (IS_EXTERN_PARTITION(ptSec->data[446 + nPart * 16 + 4]))
			{
				ReadExternPartiton(dev, fd, (ptPartDesc + nPart)->nStart_Sec, tGeometry, ptPartDesc + 4);
			}

			(ptPartDesc + nPart)->nStart_Sec /= tGeometry.cylindersize;
			(ptPartDesc + nPart)->nEnd_Sec /= tGeometry.cylindersize;

			(ptPartDesc + nPart)->dwSector = tGeometry.start;
		}
	}

	iPartNum = 0;
	for (nPart = 0; nPart < MAX_DISK_PARTITIONS; nPart++)
	{
		if ((ptPartDesc + nPart)->Type != 0)
		{
			if (!IS_EXTERN_PARTITION((ptPartDesc + nPart)->Type))
			{
				iPartNum++;
			}
		}
	}

	printf_partition_info(ptPartDesc);
	ptDiskDesc->iPartNum = iPartNum;
	return iPartNum;
}

/*!
* \brief
*找到一个适合分区的位置
*
* \param unsigned long nStart
* 开始位置
* \param unsigned long nEnd
* 结束位置
* \param struct Part_Desc *ptPartDesc
* 已有分区表信息
* \param int sectors
* 需要分配的大小
*
* \param int nPartitionIndex 分区索引
* \return
* 成功返回0,-1为没有找到该分区，-2为分区忙,-3系统保留分区无法删除
*/
static int FindSector(unsigned long nStart, unsigned long nEnd, struct Part_Desc *ptPartDesc, int sectors)
{
	unsigned int nBreakNum = 0xffffffff, j = 0xffffffff;

	const int partions_num = 12;

	struct Part_Desc *Desc[partions_num + 1];

	unsigned int bFind = 0;

	unsigned int i, k;

	FDISK_INFO("enter %s\n", __FUNCTION__);
	FDISK_INFO("nStart = %lu,nEnd = %lu,ptPartDesc->nStart_Sec = %d,ptPartDesc->nEnd_Sec = %d,sectors = %d\n", nStart, nEnd, ptPartDesc->nStart_Sec,
			ptPartDesc->nEnd_Sec, sectors);

	for (i = 0; i < partions_num + 1; i++)
	{
		Desc[i] = NULL;
	}
	Desc[0] = &ptPartDesc[0];
	for (i = 1; i < partions_num; i++)
	{
		for (j = 0; j < i; j++)
		{
			FDISK_DBG("ptPartDesc[%d].nStart_Sec = %d,Desc[%d]->nEnd_Sec = %d\n", i, ptPartDesc[i].nStart_Sec, j, Desc[j]->nEnd_Sec);
			if (ptPartDesc[i].Type == 0x5)
			{
				//continue;
			}

			if (ptPartDesc[i].nStart_Sec < Desc[j]->nEnd_Sec)
			{

				for (k = i; k > j; k--)
				{
					Desc[k] = Desc[k - 1];
				}
				Desc[j] = &ptPartDesc[i];
				break;
			}
		}

		if (j == i)
		{
			Desc[j] = &ptPartDesc[i];
		}
		for (k = 0; k < partions_num; k++)
		{
			if (NULL != Desc[k])
			{
				FDISK_DBG("Desc[%d].nStart_Sec = %d,Desc[%d].nEnd_Sec = %d\n", k, Desc[k]->nStart_Sec, k, Desc[k]->nEnd_Sec);
			}
		}
	}

	for (i = 0; i < partions_num; i++)
	{
		FDISK_DBG("Desc[%d].nStart_Sec = %d,Desc[i].nEnd_Sec = %d\n", i, Desc[i]->nStart_Sec, Desc[i]->nEnd_Sec);
	}
	
	for (i = 0; (i < partions_num && Desc[i] != NULL); i++)
	{
		k = 0;
		FDISK_DBG("Desc[%d].nStart_Sec = %d,Desc[i].nEnd_Sec = %d\n", i, Desc[i]->nStart_Sec, Desc[i]->nEnd_Sec);

		if (0 != Desc[i]->nEnd_Sec)
		{
			if (0 == bFind)
			{
				k = Desc[i]->nStart_Sec - nStart;
				j = nStart;
				bFind = 1;
			}
			else
			{
				k = Desc[i]->nStart_Sec - Desc[i - 1]->nEnd_Sec;
				j = Desc[i - 1]->nEnd_Sec;
			}
		}

		FDISK_DBG("j = %d k = %d nBreakNum = %d\n", j, k, nBreakNum);
		if (k >= sectors)
		{
			if (nBreakNum > j)
				nBreakNum = j;
		}
		FDISK_DBG("j = %d k = %d nBreakNum = %d\n", j, k, nBreakNum);

	}

	if (0 == bFind)
	{
		if (nEnd - nStart >= sectors)
		{
			nBreakNum = nStart;
		}
	}
	else
	{
		k = nEnd - Desc[i - 1]->nEnd_Sec;
		j = Desc[i - 1]->nEnd_Sec;

		FDISK_DBG("j = %d k = %d nBreakNum = %d\n", j, k, nBreakNum);
		if (k >= sectors)
		{
			if (nBreakNum > j)
				nBreakNum = j;
		}
		FDISK_DBG("j = %d k = %d nBreakNum = %d\n", j, k, nBreakNum);
	}


	if (nBreakNum < nStart)
	{
		FDISK_INFO("no should run here\n");
		nBreakNum = nStart;
	}
	FDISK_DBG("nBreakNum = %d\n", nBreakNum);
	return nBreakNum;
}

/*!
 * \brief
 * 判断该设备是否已经初始化
 *
 * \param dev
 * 设备节点名
 *
 * \return
 * 已经初始化为1，未被初始化为0，错误为-1
 */
static int IsInited(const char *dev)
{
	int fd;

	int ret = 0;

	int i;

	struct geometry tGeometry;

	struct Disk_Desc tDiskDesc; // 磁盘信息

	struct Part_Desc *ptPartDesc;

#ifndef __FAT32__
	unsigned int nSize;

	unsigned int sectors;
#endif

	fd = open(dev, O_RDWR);
	if (0 == fd)
	{
		FDISK_ERR("can not open %s\n", dev);
		return -1;
	}

	tGeometry = get_geometry(dev, fd, 1);

	FDISK_INFO("start = %lu\n", tGeometry.start);
	FDISK_INFO("heads = %lu\n", tGeometry.heads);
	FDISK_INFO("sectors = %lu\n", tGeometry.sectors);
	FDISK_INFO("cylinders = %lu\n", tGeometry.cylinders);
	FDISK_INFO("cylindersize = %lu\n", tGeometry.cylindersize);

	ReadPartition(dev, fd, tGeometry, &tDiskDesc);
	FDISK_INFO("iPartNum = %u\n", tDiskDesc.iPartNum);

	close(fd);
	
	//判断标准为至少有1个FAT32 分区
	if (tDiskDesc.iPartNum > 0)
	{
		ptPartDesc = &(tDiskDesc.atPartitions[0]);

		for (i = 0; i < MAX_DISK_PARTITIONS; i++)
		{
			if (IS_FAT_PARTITION(ptPartDesc[i].Type))
			{
				char dev_part[20];

				//获取分区节点名
				Get_Partition_DevNode(dev, i + 1, dev_part, sizeof (dev_part));

				if (0 != get_hd_space(dev_part))
				{
					ret = 1;
					break;
				}
			}
		}
	}

	return ret;
}

int fdisk(const char *hd_device, int parts)
{
	char *dev = NULL;
	char *tmp_string = NULL;
	
	unsigned long from = 0;
	unsigned long to = 0;
	int i = 0;
	int fd = -1;
    int ret = 0;

	struct geometry R;

	if (parts > 12)
	{
		parts = 12;
    }
    
	if (parts > 4)
	{
		disk_parts = parts + 1;
	}else{
		disk_parts = parts;
    }
    
	for (i = 0; i < 15; i++)
	{
		part_string[i] = NULL;
    }
    
	dev = hd_device;
	part_current = 0;

	fd = my_open(dev, 0, 1 /*silent */ );
	if (fd < 0)
	{
	    FDISK_ERR("my_open %s fail\n", dev);
		return -1;
    }
	R = get_geometry(dev, fd, 1 /*silent */ );
	close(fd);

	if (R.cylinders)
	{
		disk_cylinders = R.cylinders;
	}else{
		return -1;
    }
    
	part_cylinders = disk_cylinders / parts;

	if (disk_parts <= 4)
	{
		from = 0L;
		to = part_cylinders;
		for (i = 0; i < disk_parts - 1; i++)
		{
			tmp_string = (char *) malloc(20);
			memset(tmp_string, '\0', 20);
			sprintf(tmp_string, "%lu,%lu,L\n", from, part_cylinders);
			part_string[i] = tmp_string;
			from = from + part_cylinders;
		}
		tmp_string = (char *) malloc(20);
		memset(tmp_string, '\0', 20);
		sprintf(tmp_string, "%lu,,L\n", from);
		part_string[disk_parts - 1] = tmp_string;
		part_string[disk_parts] = NULL;
	}
	else
	{
		from = 0L;
		to = part_cylinders;
		for (i = 0; i < 3; i++)
		{
			tmp_string = (char *) malloc(20);
			memset(tmp_string, '\0', 20);
			sprintf(tmp_string, "%lu,%lu,L\n", from, part_cylinders);
			part_string[i] = tmp_string;
			from = from + part_cylinders;
		}
		tmp_string = (char *) malloc(20);
		memset(tmp_string, '\0', 20);
		sprintf(tmp_string, "%lu,,E\n", from);
		part_string[3] = tmp_string;

		for (i = 4; i < disk_parts - 1; i++)	//5--
		{
			tmp_string = (char *) malloc(20);
			memset(tmp_string, '\0', 20);
			sprintf(tmp_string, "%lu,%lu,L\n", from, part_cylinders);
			part_string[i] = tmp_string;
			from = from + part_cylinders;
		}
		tmp_string = (char *) malloc(20);
		memset(tmp_string, '\0', 20);
		sprintf(tmp_string, "%lu,,L\n", from);
		part_string[disk_parts - 1] = tmp_string;
		tmp_string = (char *) malloc(20);
		memset(tmp_string, '\0', 20);
		sprintf(tmp_string, "\n");
		part_string[disk_parts] = tmp_string;

	}

	ret = do_fdisk(dev);

	//release part_string
	for (i = 0; i < 15; i++)
	{
		if (NULL != part_string[i])
		{
			free(part_string[i]);
		}
	}

	return ret;
}

/*!
 * \brief
 * 初始化硬盘。初始化后，该硬盘只有一个系统保留区域(大小为nSize)一个扩展分区
 *
 * \param pcDev
 * 设备节点名
 *
 * \param int nPartitionIndex 分区索引
 *
 *
 * \return
 * 成功返回0,-1为没有找到该分区，-2为分区忙
 */
int InitDev(const char *dev, unsigned long nSize)
{
	int ret = -1;

	int fd = -1;

	struct geometry tGeometry;

	struct Disk_Desc tDiskDesc; // 磁盘信息

	struct Part_Desc *ptPartDesc;

#ifndef __FAT32__
	unsigned int sectors, total_sectors;
#endif

	fd = open(dev, O_RDWR);
	if (0 == fd)
	{
		FDISK_ERR("open %s fail\n", dev);
		return 0;
	}

	tGeometry = get_geometry(dev, fd, 1);
	FDISK_INFO("start = %lu\n", tGeometry.start);
	FDISK_INFO("heads = %lu\n", tGeometry.heads);
	FDISK_INFO("sectors = %lu\n", tGeometry.sectors);
	FDISK_INFO("cylinders = %lu\n", tGeometry.cylinders);
	FDISK_INFO("cylindersize = %lu\n", tGeometry.cylindersize);

	memset(&tDiskDesc, 0, sizeof (tDiskDesc));

#ifdef __FAT32__

	//初始化为1个FAT32分区
	tDiskDesc.iPartNum = 1;

	ptPartDesc = &(tDiskDesc.atPartitions[0]);
FDISK_INFO("---cylinders=%lu---\n",tGeometry.cylinders);
	ptPartDesc[0].Type = 0xB;
	ptPartDesc[0].nStart_Sec = 0;
	//在sd卡上划分一个1/8大小的分区//-
	ptPartDesc[0].nEnd_Sec = tGeometry.cylinders;//tGeometry.cylinders
	ptPartDesc[0].dwSector = tGeometry.start;

#else

#ifdef __EXT3__ 
		
	//初始化为1个EXT3分区
	tDiskDesc.iPartNum = 1;

	ptPartDesc = &(tDiskDesc.atPartitions[0]);
FDISK_INFO("---cylinders=%lu---\n",tGeometry.cylinders);
	ptPartDesc[0].Type = 0x83;
	ptPartDesc[0].nStart_Sec = 0;
	ptPartDesc[0].nEnd_Sec = tGeometry.cylinders;//tGeometry.cylinders
	ptPartDesc[0].dwSector = tGeometry.start;
#else

	total_sectors = tGeometry.cylinders;
	sectors = (nSize * 1024 * 2) / tGeometry.cylindersize;
	if ((nSize * 1024 * 2) % tGeometry.cylindersize)
	{
		sectors += 1;
	}
	FDISK_INFO("nSize = %lu sectors = %u total_sectors = %u\n", nSize, sectors, total_sectors);

	if (sectors > total_sectors)
	{
		FDISK_ERR("sectors(%d) > total_sectors(%d)\n", sectors, total_sectors);
		close(fd);
		return -1;
	}

	ptPartDesc = &(tDiskDesc.atPartitions[0]);
	ptPartDesc[0].Type = 0x83;
	ptPartDesc[0].nStart_Sec = 0;
	ptPartDesc[0].nEnd_Sec = sectors;
	ptPartDesc[0].dwSector = tGeometry.start;

	ptPartDesc[1].Type = 0x5;
	ptPartDesc[1].nStart_Sec = sectors;
	ptPartDesc[1].nEnd_Sec = total_sectors;
	ptPartDesc[1].dwSector = tGeometry.start;
#endif
#endif

	ret = write_partition(dev, fd, &tDiskDesc, tGeometry);
    if (ret)
    {
        FDISK_ERR("Fail to call write_partition\n");
        return -1;
    }
    
	reread_disk_partition(dev, fd);
	close(fd);
	
	return 0;
}

/*!
 * \brief
 * 添加分区
 *
 * \param dev
 * 设备节点名
 *
 * \param int nSize 大小单位(M)
 *
 * \return
 * 成功返回0,-1分区过大，请缩小分区容量.-2无连续空间分配给该分区-3已经达到最大分区个数
 *-4该硬盘未被初始化，无法添加分区
 */
int AddPartition(char *dev, unsigned long nSize)
{
	int fd;

	unsigned int partition;

	unsigned int i = 0;

	struct geometry geometry;

	struct Disk_Desc *ptDiskDesc;	// 磁盘信息

	struct Part_Desc *ptPartDesc;

	unsigned int sectors, total_sectors;

	FDISK_DBG("enter %s\n", __FUNCTION__);

	if (0 == IsInited(dev))
	{
		FDISK_ERR("%s is not inited\n", dev);
		return -4;
	}

	fd = open(dev, O_RDWR);

	if (0 == fd)
	{
		FDISK_ERR("open %s fail\n", dev);
		return 0;
	}

	ptDiskDesc = (struct Disk_Desc *) malloc(sizeof (struct Disk_Desc));
	memset(ptDiskDesc, 0, sizeof (struct Disk_Desc));
	geometry = get_geometry(dev, fd, 1);
	FDISK_INFO("start = %lu\n", geometry.start);
	FDISK_INFO("heads = %lu\n", geometry.heads);
	FDISK_INFO("sectors = %lu\n", geometry.sectors);
	FDISK_INFO("cylinders = %lu\n", geometry.cylinders);
	FDISK_INFO("cylindersize = %lu\n", geometry.cylindersize);

	sectors = (nSize * 1024 * 2) / geometry.cylindersize;
	if ((nSize * 1024 * 2) % geometry.cylindersize)
	{
		//sectors += 1;
	}
	partition = ReadPartition(dev, fd, geometry, ptDiskDesc);

	if (partition == 1)
	{
		//  sectors -= 1;
	}
	FDISK_INFO("nSize = %lu sectors = %u\n", nSize, sectors);
	total_sectors = (geometry.cylinders);

	if (sectors > total_sectors)
	{
		FDISK_ERR("error\n");
		close(fd);
		free(ptDiskDesc);
		return -1;
	}
	partition = ReadPartition(dev, fd, geometry, ptDiskDesc);

	if (partition == 15)
	{
		FDISK_ERR("error\n");
		close(fd);
		free(ptDiskDesc);
		return -3;
	}
	ptPartDesc = &(ptDiskDesc->atPartitions[0]);
	if (partition < 1)
	{
		FDISK_ERR("[%s %d] error\n", __FILE__, __LINE__);
		close(fd);
		free(ptDiskDesc);
		return -4;
	}
	else
	{
		sectors++;
		unsigned int nStart_Sec = FindSector((ptPartDesc + 1)->nStart_Sec + 1, (ptPartDesc + 1)->nEnd_Sec, ptPartDesc + 4, sectors);

		if (nStart_Sec == -1)
		{
			sectors--;
			nStart_Sec = FindSector((ptPartDesc + 1)->nStart_Sec + 1, (ptPartDesc + 1)->nEnd_Sec, ptPartDesc + 4, sectors);
		}
		FDISK_INFO("nStart_Sec = %d\n", nStart_Sec);
		if (nStart_Sec == -1)
		{
			unsigned int nTmp;

			FDISK_ERR("error sectors = %d\n", sectors);
			for (nTmp = 1; nTmp < 10; nTmp++)
			{
				if (ptPartDesc[nTmp].Type)
				{
					FDISK_INFO("num %d:nStart_Sec = %d nEnd_Sec = %d\n", nTmp, ptPartDesc[nTmp].nStart_Sec, ptPartDesc[nTmp].nEnd_Sec);
				}
			}
			close(fd);
			free(ptDiskDesc);
			return -2;
		}
		else
		{
			for (i = 4; i < MAX_DISK_PARTITIONSEX; i++)
			{
				if (0 == ptPartDesc[i].Type)
				{
					ptPartDesc[i].Type = 0xB;
					ptPartDesc[i].nStart_Sec = nStart_Sec;
					ptPartDesc[i].nEnd_Sec = ptPartDesc[i].nStart_Sec + sectors;
					ptPartDesc[i].dwSector = nStart_Sec * geometry.start;
					FDISK_INFO("ptPartDesc[%d].Type = %x,ptPartDesc[i].nStart_Sec = %d,ptPartDesc[i].nEnd_Sec = %d\n", i, ptPartDesc[i].Type,
							ptPartDesc[i].nStart_Sec, ptPartDesc[i].nEnd_Sec);
					break;
				}
			}
		}
	}

	write_partition(dev, fd, ptDiskDesc, geometry);
	reread_disk_partition(dev, fd);
	close(fd);
	free(ptDiskDesc);
	return 0;

}

/*!
 * \brief
 * 删除分区
 *
 * \param dev		硬盘设备节点名
 *
 * \param part		分区号
 *
 * \return
 * 成功返回0,-1为没有找到该分区，-2为分区忙,-3系统保留分区无法删除
 */
int DelPartition(char *dev, int part)
{
	int fd;

	unsigned int info;

	struct geometry tGeometry;

	struct Disk_Desc tDiskDesc;	// 磁盘信息

	if (part < 0 || part >= MAX_DISK_PARTITIONS)
	{
		FDISK_ERR("invalid partition index %d\n", part);
		return -1;
	}

	fd = open(dev, O_RDWR);
	if (0 == fd)
	{
		FDISK_ERR("can't open %s\n", dev);
		return -1;
	}

	tGeometry = get_geometry(dev, fd, 0);

	ReadPartition(dev, fd, tGeometry, &tDiskDesc);

	FDISK_DBG("delete %s partition %d\n", dev, part);

	if (part <= 4)
	{
		//删除主分区
		memset(&tDiskDesc.atPartitions[part - 1], 0, sizeof (struct Part_Desc));
	}
	else
	{
		//删除逻辑分区
		for (info = part - 1; info < (MAX_DISK_PARTITIONS - 1); info++)
		{
			memcpy(&tDiskDesc.atPartitions[info], &tDiskDesc.atPartitions[info + 1], sizeof (struct Part_Desc));
		}

		//删除最后一个分区
		memset(&tDiskDesc.atPartitions[MAX_DISK_PARTITIONS - 1], 0, sizeof (struct Part_Desc));
	}
	printf_partition_info(tDiskDesc.atPartitions);

	FDISK_DBG("\n删除完成...................\n");

	write_partition(dev, fd, &tDiskDesc, tGeometry);

	reread_disk_partition(dev, fd);
	close(fd);
	
	return 0;

}

int GetPartitionInfo(const char *dev, struct Disk_Desc *ptDiskDesc)
{
	int fd;

	struct geometry tGeometry;

	memset(ptDiskDesc, 0, sizeof (struct Disk_Desc));

	fd = open(dev, O_RDWR);
	FDISK_INFO("GetPartitionInfo dev = %s\n", dev);
	if (0 == fd)
	{
		FDISK_ERR("open %s fail\n", dev);
		return 0;
	}

	tGeometry = get_geometry(dev, fd, 1);
	FDISK_INFO("dev = %s\n", dev);
	FDISK_INFO("start = %lu\n", tGeometry.start);
	FDISK_INFO("heads = %lu\n", tGeometry.heads);
	FDISK_INFO("sectors = %lu\n", tGeometry.sectors);
	FDISK_INFO("cylinders = %lu\n", tGeometry.cylinders);
	FDISK_INFO("cylindersize = %lu\n", tGeometry.cylindersize);

	ReadPartition(dev, fd, tGeometry, ptDiskDesc);
	FDISK_INFO("iPartNum %d\n", ptDiskDesc->iPartNum);

	close(fd);

	return tGeometry.cylindersize;
}
