/*
 *  avilib.c
 *
 *  Copyright (C) Thomas streich - June 2001
 *  multiple audio track support Copyright (C) 2002 Thomas streich 
 *
 *  Original code:
 *  Copyright (C) 1999 Rainer Johanni <Rainer@Johanni.de> 
 *
 *  This file is part of transcode, a linux video stream processing tool
 *      
 *  transcode is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *   
 *  transcode is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *   
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. 
 *
 */

#include "avilib.h"
//#include <time.h>

#define INFO_LIST

typedef int ssize_t;

/* The following variable indicates the kind of error */

static long AVI_errno = 0;

#define MAX_INFO_STRLEN     (64)
#define FRAME_RATE_SCALE    (1000000)

#define AVILIB_ERR(format, args...)   do{printf("\033[1;31m[AviLib][%s-%d]\033[m" format, __FILE__, __LINE__, ##args);}while(0)

static char id_str[MAX_INFO_STRLEN] = {0};

/*******************************************************************
 *                                                                 *
 *    Utilities for writing an AVI File                            *
 *                                                                 *
 *******************************************************************/

/*@ breif avi_write_timeout 写超时检测函数，不含写操作
 *@ param [IN] fd 文件描述符
 *@ param [IN] wait_seconds 等待超时秒数，如果为0则表示不检测超时
 *@ return 成功(未超时)返回0，失败返回-1，超时返回-1并且errno=ETIMEDOUT
 */
int avi_write_timeout(int fd, unsigned int wait_seconds)
{
    if (fd <= 0)
    {
        return -1;
    }

    /* 默认为0，当wait_seconds==0时，不检测直接返回0 */
    int ret = 0;

    /* 如果需要检测超时 */
    if(wait_seconds > 0)
    {
        fd_set write_fdset;      // 描述符集合
        struct timeval timeout;  // 超时时间

        FD_ZERO(&write_fdset);
        FD_SET(fd, &write_fdset);

        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;

        do
        {
           /* select会阻塞直到检测到事件或则超时，如果超时，select会返回0， 如果检测到事件会返回1，
            * 如果异常会返回-1，如果是由于信号中断引起的异常errno==EINTR 
            */
            ret = select(fd+1, NULL, &write_fdset, NULL, &timeout); 
        }while(ret<0 && errno == EINTR);

        /* 如果是有信号引起的异常则继续阻塞select，直到检测到事件或则超时 */
        if(0 == ret)    // select超时退出
        {
            ret = -1; 
            errno = ETIMEDOUT;
        }
        else if(1 == ret) // select检测到可写事件 
        {
            ret = 0;
        }
    }
    return ret;
}

/*@ breif avi_read_timeout 读超时检测函数，不含读操作
 *@ param [IN] fd 文件描述符
 *@ param [IN] wait_seconds 等待超时秒数，如果为0则表示不检测超时
 *@ return 成功(未超时)返回0，失败返回-1，超时返回-1并且errno=ETIMEDOUT
 */
int avi_read_timeout(int fd, unsigned int wait_seconds)
{
    if (fd <= 0)
    {
        return -1;
    }

    /* 默认为0，当wait_seconds==0时，不检测直接返回0 */
    int ret = 0;

    /* 如果当前需要检测超时 */
    if(wait_seconds > 0)
    {
        fd_set read_fdset;       // 描述符集合
        struct timeval timeout;  // 超时时间

        FD_ZERO(&read_fdset);
        FD_SET(fd, &read_fdset);

        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;

        /* select会阻塞直到检测到事件或则超时，如果超时，select会返回0，如果检测
         * 到事件会返回1，如果异常会返回-1，如果是由于信号中断引起的异常errno==EINTR
         * 如果是有信号引起的异常则继续阻塞select，直到检测到事件或则超时
         */
        do
        {
            ret = select(fd+1, &read_fdset, NULL, NULL, &timeout);
        }while(ret < 0 && errno == EINTR);

        if(0 == ret)      // select超时退出
        {
            ret = -1;
            errno = ETIMEDOUT;
        }
        else if(1 == ret) // select检测到可读事件
        {
            ret = 0;
        }
    }
    return ret;
}


static size_t avi_read(int fd, char *buf, size_t len)
{
    if (fd<0 || !buf)
    {
        AVILIB_ERR("param error! fd = %d, buf = %p\n", fd, buf);
        return -1;
    }

    size_t n = 0;
    size_t r = 0;
    while (r < len) 
    {
        n = read (fd, buf + r, len - r);
        if ((ssize_t)n <= 0)
        {
//          AVILIB_ERR("fd=%d, n=%d : %s\n", fd, n, strerror(errno));
            return r;
        }
        r += n;
    }

    return r;
}

/* WRITE_MAX_BUF: 64KB缓冲区大小
 * rest_len: 缓冲区剩余空间大小，默认64KB表示缓冲区为空
 * tmp_buf: 64KB缓冲区
 * tmp_p: 指向缓冲区的指针
 */
#define WRITE_MAX_BUF  (64*1024)
static unsigned int rest_len = WRITE_MAX_BUF;
static char tmp_buf[WRITE_MAX_BUF] = {0xff};
static char *tmp_p = tmp_buf;

/* 将缓冲区里面的数据刷新写到AVI上面 */
static void avi_fflush(int fd)
{
    if (fd < 0)
    {
        AVILIB_ERR("param error! fd = %d\n", fd);
        return;
    }

    if (WRITE_MAX_BUF - rest_len > 0)
    {
        size_t n = 0;
        size_t r = 0;
        
        while (r < WRITE_MAX_BUF-rest_len) 
        {
            n = write (fd, tmp_buf + r, WRITE_MAX_BUF-rest_len-r);
            if ((ssize_t)n <= 0)
            {
//              AVILIB_ERR("fd=%d, n=%d : %s\n", fd, n, strerror(errno));
                return;
            }
            r += n;
        }
        //memset(tmp_buf, 0, WRITE_MAX_BUF);
        tmp_p = tmp_buf;
        rest_len = WRITE_MAX_BUF;
    }
}

#if 1

/* 为了提高写AVI速度，使其在SD卡上面写更快，需要重写写AVI数据函数(每次写的单位是64KB) */
static size_t avi_write (int fd, char *buf, size_t len)
{
    if (fd<0 || !buf)
    {
        AVILIB_ERR("param error! fd = %d, buf = %p\n", fd, buf);
        return -1;
    }

    size_t n = 0;
    size_t r = 0;
    size_t t = 0;

    t = len;

    /* 以下处理表示一次写64KB数据，不足64KB的先放到缓冲区，累计成64K再写入 */
    for (;;)
    {
        n = r = 0; // 此处必须重新赋初值，否则数据会出错造成花屏
        if (len >= rest_len)
        {
            memcpy(tmp_p, buf, rest_len);
            while (r < WRITE_MAX_BUF) 
            {
#if 0
                if(0 !=  avi_write_timeout(fd, 5))    // 检测到描述符不可写
                {
                    AVILIB_ERR("avi write timeout\n");
                    return n;
                }
#endif
                n = write (fd, tmp_buf + r, WRITE_MAX_BUF - r);
                if ((ssize_t)n <= 0)
                {
                    AVILIB_ERR("fd=%d, n=%d : %s\n", fd, n, strerror(errno));
                    return n;
                }
                r += n;
            }
            //memset(tmp_buf, 0, WRITE_MAX_BUF);
            tmp_p = tmp_buf;
            len -= rest_len;
            buf += rest_len;
            rest_len = WRITE_MAX_BUF;            
        }
        else
        {
            memcpy(tmp_p, buf, len);
            rest_len -= len;
            tmp_p += len;
            break;
        }
    }
    return t;
}

#else

static size_t avi_write (int fd, char *buf, size_t len)
{
    if (fd<0 || !buf)
    {
        AVILIB_ERR("param error! fd = %d, buf = %#x\n", fd, buf);
        return -1;
    }

    size_t n = 0;
    size_t r = 0;

    while (r < len) 
    {
        n = write (fd, buf + r, len - r);
        if ((ssize_t)n < 0)
        {
            return n;
        }
        r += n;
    }
    return r;
}
#endif

/* HEADERBYTES: The number of bytes to reserve for the header */

#define HEADERBYTES (2048)

/* AVI_MAX_LEN: The maximum length of an AVI file, we stay a bit below
   the 2GB limit (Remember: 2*10^9 is smaller than 2 GB) 
 */

#define AVI_MAX_LEN (UINT_MAX-(1<<20)*16-HEADERBYTES)

#define PAD_EVEN(x) ( ((x)+1) & ~1 )


/* Copy n into dst as a 4 byte, little endian number. Should also work on big endian machines */
static void long2str(unsigned char *dst, int n)
{
    dst[0] = (n    )&0xff;
    dst[1] = (n>> 8)&0xff;
    dst[2] = (n>>16)&0xff;
    dst[3] = (n>>24)&0xff;
}

/* Convert a string of 4 or 2 bytes to a number, also working on big endian machines */
static unsigned long str2ulong(unsigned char *str)
{
    return ( str[0] | (str[1]<<8) | (str[2]<<16) | (str[3]<<24) );
}
static unsigned long str2ushort(unsigned char *str)
{
    return ( str[0] | (str[1]<<8) );
}

/* Calculate audio sample size from number of bits and number of channels.
   This may have to be adjusted for eg. 12 bits and stereo 
 */
static int avi_sampsize(avi_t *AVI, int j)
{
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    int s = 0;
    s = ((AVI->track[j].a_bits+7)/8)*AVI->track[j].a_chans;
    // if(s==0) s=1; /* avoid possible zero divisions */
    /* 避免除以零 */ 
    if(s < 4)
    {
       s = 4;
    }
    
    return s;
}

/* 添加一个chunk块(包括tag和数据)到AVI文件,
   returns -1 on write error, 0 on success */
static int avi_add_chunk(avi_t *AVI, unsigned char *tag, unsigned char *data, int length)
{
    if (!AVI || !tag || !data)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    unsigned char c[8] = {0};

    /* 用一个int整型数据c存放tag和length，故只需要执行一次write系统调用即可 */
    memcpy(c, tag, 4);
    long2str(c+4, length);

    /* Output tag, length and data, restore previous position if the write fails */
    length = PAD_EVEN(length);

    if (8 != avi_write(AVI->fdes, (char *)c, 8))
    {
        avi_fflush(AVI->fdes);
        lseek(AVI->fdes, AVI->pos, SEEK_SET);
        AVI_errno = AVI_ERR_WRITE;
        return -1;
    }

    if ((size_t)length != avi_write(AVI->fdes, (char *)data, length))
    {
        avi_fflush(AVI->fdes);
        lseek(AVI->fdes, AVI->pos, SEEK_SET);
        AVI_errno = AVI_ERR_WRITE;
        return -1;
    }

#if 0
    if( avi_write(AVI->fdes, (char *)c, 8) != 8 || avi_write(AVI->fdes, (char *)data, length) != length )
    {
        avi_fflush(AVI->fdes);
        lseek(AVI->fdes,AVI->pos,SEEK_SET);
        AVI_errno = AVI_ERR_WRITE;
        return -1;
    }
#endif

    /* Update file position */
    AVI->pos += 8 + length;
    //fprintf(stderr, "pos=%lu %s\n", AVI->pos, tag);

    return 0;
}

static int avi_add_index_entry(avi_t *AVI, unsigned char *tag, long flags, unsigned long pos, unsigned long len)
{
    if (!AVI || !tag)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    void *ptr = NULL;

    if(AVI->n_idx>=AVI->max_idx) 
    {
        ptr = realloc((void *)AVI->idx,(AVI->max_idx+4096)*16);
        if(NULL == ptr) 
        {
            AVI_errno = AVI_ERR_NO_MEM;
            return -1;
        }
        AVI->max_idx += 4096;
        AVI->idx = (unsigned char((*)[16])) ptr;
    }

    /* Add index entry */
    /* 可以根据此处得到当前帧数据在文件中的实际偏移地址 */
    memcpy(AVI->idx[AVI->n_idx],tag, 4);
    long2str(AVI->idx[AVI->n_idx]+ 4, flags);
    long2str(AVI->idx[AVI->n_idx]+ 8, pos);
    long2str(AVI->idx[AVI->n_idx]+12, len);

    /* Update counter */
    AVI->n_idx++;

    if(len>AVI->max_len)
    {
        AVI->max_len=len;
    }
    
    return 0;
}

//SLM
#ifndef S_IRUSR
#define S_IRWXU       (00700)       /* read, write, execute: owner */
#define S_IRUSR       (00400)       /* read permission: owner */
#define S_IWUSR       (00200)       /* write permission: owner */
#define S_IXUSR       (00100)       /* execute permission: owner */
#define S_IRWXG       (00070)       /* read, write, execute: group */
#define S_IRGRP       (00040)       /* read permission: group */
#define S_IWGRP       (00020)       /* write permission: group */
#define S_IXGRP       (00010)       /* execute permission: group */
#define S_IRWXO       (00007)       /* read, write, execute: other */
#define S_IROTH       (00004)       /* read permission: other */
#define S_IWOTH       (00002)       /* write permission: other */
#define S_IXOTH       (00001)       /* execute permission: other */
#endif

/*
 * 以只写方式打开一个AVI文件，权限为只写，一般用作生成AVI文件
 * returns a pointer to avi_t on success, a zero pointer on error
 */
avi_t* AVI_open_output_file(const char * filename)
{
    if (NULL == filename)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return NULL;
    }

    avi_t *AVI = NULL;
    int i = 0;

    int mask = 0;

    unsigned char AVI_header[HEADERBYTES] = {0};

    /* Allocate the avi_t struct and zero it */
    AVI = (avi_t *)malloc(sizeof(avi_t));
    if(AVI == NULL)
    {
        AVI_errno = AVI_ERR_NO_MEM;
        return NULL;
    }
    
    memset((void *)AVI, 0, sizeof(avi_t));

    /* 由于Linux在删除大文件时需要很长时间, 我们打开文件时不会截断文件, 而是在AVI文件关闭时截断      */
    mask = umask (0);
    umask (mask);

    AVI->fdes = open(filename, O_RDWR|O_CREAT, (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH) &~ mask);
    if (AVI->fdes < 0)
    {
        AVI_errno = AVI_ERR_OPEN;
        free(AVI);
        AVI = NULL;
        return NULL;
    }

    /* 写入HEADERBYTES个字节的空数据，用于占位，当文件写完毕的时候，需要重新填充该HEADERBYTES字节内容 */
    for (i=0; i<HEADERBYTES; i++)
    {
        AVI_header[i] = 0;
    }

    i = avi_write(AVI->fdes, (char *)AVI_header, HEADERBYTES);
    if (i != HEADERBYTES)
    {
        avi_fflush(AVI->fdes);
        close(AVI->fdes);
        AVI_errno = AVI_ERR_WRITE;
        free(AVI);
        AVI = NULL;
        return NULL;
    }
    
    avi_fflush(AVI->fdes);

    AVI->pos  = HEADERBYTES;
    AVI->mode = AVI_MODE_WRITE; /* open for writing */

    //init
    AVI->anum = 0;
    AVI->aptr = 0;

    return AVI;
}

/* 设置视频信息 */
void AVI_set_video(avi_t *AVI, int width, int height, double fps, char *compressor)
{
    if (!AVI || !compressor)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return;
    }

    /* may only be called if file is open for writing */
    if(AVI->mode == AVI_MODE_READ)
    {
        return;
    }

    AVI->width  = width;
    AVI->height = height;
    AVI->fps    = fps;

    if(strncmp(compressor, "RGB", 3)==0) 
    {
        memset(AVI->compressor, 0, 4);
    }
    else
    {
        memcpy(AVI->compressor, compressor, 4);
    }     

    AVI->compressor[4] = 0;

    avi_update_header(AVI);
}

/* 设置音频信息 */
void AVI_set_audio(avi_t *AVI, int channels, long rate, int bits, int format, long mp3rate)
{
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return;
    }

    /* may only be called if file is open for writing */
    if(AVI->mode == AVI_MODE_READ) 
    {
        return;
    }

    //inc audio tracks
    AVI->aptr=AVI->anum;
    ++AVI->anum;

    if(AVI->anum > AVI_MAX_TRACKS) 
    {
        AVILIB_ERR("error - only %d audio tracks supported\n", AVI_MAX_TRACKS);
        exit(1);
    }

    AVI->track[AVI->aptr].a_chans = channels;
    AVI->track[AVI->aptr].a_rate  = rate;
    AVI->track[AVI->aptr].a_bits  = bits;
    AVI->track[AVI->aptr].a_fmt   = format;
    AVI->track[AVI->aptr].mp3rate = mp3rate;

    avi_update_header(AVI);
}

#define OUT4CC(s) \
    if(nhb<=HEADERBYTES-4) memcpy(AVI_header+nhb,s,4); nhb += 4

#define OUTLONG(n) \
    if(nhb<=HEADERBYTES-4) long2str(AVI_header+nhb,n); nhb += 4

#define OUTSHRT(n) \
    if(nhb<=HEADERBYTES-2) { \
        AVI_header[nhb  ] = (n   )&0xff; \
        AVI_header[nhb+1] = (n>>8)&0xff; \
    } \
    nhb += 2


// 更新AVI文件头: 0 frames, max vid/aud size
int avi_update_header(avi_t *AVI)
{
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    int njunk=0, sampsize=0, hasIndex=0, ms_per_frame=0, frate=0, flag=0;
    int movi_len=0, hdrl_start=0, strl_start=0, j=0;
    long nhb = 0;
    unsigned char AVI_header[HEADERBYTES] = {0};

    //假定最大字节
    movi_len = AVI_MAX_LEN - HEADERBYTES + 4;

    //assume index will be written
    hasIndex = 1;

    if(AVI->fps < 0.001) 
    {
        frate=0;
        ms_per_frame=0;
    }
    else
    {
        frate = (int) (FRAME_RATE_SCALE*AVI->fps + 0.5);
        ms_per_frame=(int) (1000000/AVI->fps + 0.5);
    }

    /* Prepare the file header */
    nhb = 0;

    /* The RIFF header */
    OUT4CC ("RIFF");
    OUTLONG(movi_len);    // assume max size
    OUT4CC ("AVI ");

    /* Start the header list */
    OUT4CC ("LIST");
    OUTLONG(0);        /* Length of list in bytes, don't know yet */
    hdrl_start = nhb;  /* Store start position */
    OUT4CC ("hdrl");

    /* The main AVI header */

    /* The Flags in AVI File header */

#define AVIF_HASINDEX           0x00000010      /* Index at end of file */
#define AVIF_MUSTUSEINDEX       0x00000020
#define AVIF_ISINTERLEAVED      0x00000100
#define AVIF_TRUSTCKTYPE        0x00000800      /* Use CKType to find key frames */
#define AVIF_WASCAPTUREFILE     0x00010000
#define AVIF_COPYRIGHTED        0x00020000

    OUT4CC ("avih");
    OUTLONG(56);                 /* # of bytes to follow */
    OUTLONG(ms_per_frame);       /* Microseconds per frame */
    //ThOe ->0 
    //   OUTLONG(10000000);      /* MaxBytesPerSec, I hope this will never be used */
    OUTLONG(0);
    OUTLONG(0);                  /* PaddingGranularity (whatever that might be) */
    /* Other sources call it 'reserved' */
    flag = AVIF_ISINTERLEAVED;
    if(hasIndex)
    {
        flag |= AVIF_HASINDEX;
    }
    
    if(hasIndex && AVI->must_use_index)
    {
        flag |= AVIF_MUSTUSEINDEX;
    }

    OUTLONG(flag);               /* Flags */
    OUTLONG(0);                  // no frames yet
    OUTLONG(0);                  /* InitialFrames */

    OUTLONG(AVI->anum+1);

    OUTLONG(0);                  /* SuggestedBufferSize */
    OUTLONG(AVI->width);         /* Width */
    OUTLONG(AVI->height);        /* Height */
    /* MS calls the following 'reserved': */
    OUTLONG(0);                  /* TimeScale:  Unit used to measure time */
    OUTLONG(0);                  /* DataRate:   Data rate of playback     */
    OUTLONG(0);                  /* StartTime:  Starting time of AVI data */
    OUTLONG(0);                  /* DataLength: Size of AVI data chunk    */

    /* Start the video stream list ---------------------------------- */

    OUT4CC ("LIST");
    OUTLONG(0);        /* Length of list in bytes, don't know yet */
    strl_start = nhb;  /* Store start position */
    OUT4CC ("strl");

    /* The video stream header */

    OUT4CC ("strh");
    OUTLONG(56);                 /* # of bytes to follow */
    OUT4CC ("vids");             /* Type */
    OUT4CC (AVI->compressor);    /* Handler */
    OUTLONG(0);                  /* Flags */
    OUTLONG(0);                  /* Reserved, MS says: wPriority, wLanguage */
    OUTLONG(0);                  /* InitialFrames */
    OUTLONG(FRAME_RATE_SCALE);              /* Scale */
    OUTLONG(frate);              /* Rate: Rate/Scale == samples/second */
    OUTLONG(0);                  /* Start */
    OUTLONG(0);                  // no frames yet
    OUTLONG(0);                  /* SuggestedBufferSize */
    OUTLONG(-1);                 /* Quality */
    OUTLONG(0);                  /* SampleSize */
    OUTLONG(0);                  /* Frame */
    OUTLONG(0);                  /* Frame */
    //OUTLONG(0);                  /* Frame */
    //OUTLONG(0);                  /* Frame */

    /* The video stream format */

    OUT4CC ("strf");
    OUTLONG(40);                 /* # of bytes to follow */
    OUTLONG(40);                 /* Size */
    OUTLONG(AVI->width);         /* Width */
    OUTLONG(AVI->height);        /* Height */
    OUTSHRT(1); OUTSHRT(24);     /* Planes, Count */
    OUT4CC (AVI->compressor);    /* Compression */
    // ThOe (*3)
    OUTLONG(AVI->width*AVI->height*3);  /* SizeImage (in bytes?) */
    OUTLONG(0);                  /* XPelsPerMeter */
    OUTLONG(0);                  /* YPelsPerMeter */
    OUTLONG(0);                  /* ClrUsed: Number of colors used */
    OUTLONG(0);                  /* ClrImportant: Number of colors important */

    /* Finish stream list, i.e. put number of bytes in the list to proper pos */
    long2str(AVI_header+strl_start-4,nhb-strl_start);

    /* Start the audio stream list ---------------------------------- */
    for(j=0; j<AVI->anum; ++j) 
    {
        sampsize = avi_sampsize(AVI, j);

        OUT4CC ("LIST");
        OUTLONG(0);        /* Length of list in bytes, don't know yet */
        strl_start = nhb;  /* Store start position */
        OUT4CC ("strl");

        /* The audio stream header */

        OUT4CC ("strh");
        OUTLONG(56);            /* # of bytes to follow */
        OUT4CC ("auds");

        // -----------
        // ThOe
        OUTLONG(0);             /* Format (Optionally) */
        // -----------

        OUTLONG(0);             /* Flags */
        OUTLONG(0);             /* Reserved, MS says: wPriority, wLanguage */
        OUTLONG(0);             /* InitialFrames */

        // ThOe /4
        OUTLONG(sampsize/4);      /* Scale */
        OUTLONG(1000*AVI->track[j].mp3rate/8);
        OUTLONG(0);             /* Start */
        OUTLONG(4*AVI->track[j].audio_bytes/sampsize);   /* Length */
        OUTLONG(0);             /* SuggestedBufferSize */
        OUTLONG(-1);            /* Quality */

        // ThOe /4
        OUTLONG(sampsize/4);    /* SampleSize */

        OUTLONG(0);             /* Frame */
        OUTLONG(0);             /* Frame */
        //       OUTLONG(0);             /* Frame */
        //OUTLONG(0);             /* Frame */

        /* The audio stream format */

        OUT4CC ("strf");
        OUTLONG(16);                   /* # of bytes to follow */
        OUTSHRT(AVI->track[j].a_fmt);           /* Format */
        OUTSHRT(AVI->track[j].a_chans);         /* Number of channels */
        OUTLONG(AVI->track[j].a_rate);          /* SamplesPerSec */
        // ThOe
        OUTLONG(1000*AVI->track[j].mp3rate/8);

        //ThOe (/4)
        OUTSHRT(sampsize/4);                    /* BlockAlign */

        OUTSHRT(AVI->track[j].a_bits);          /* BitsPerSample */

        /* Finish stream list, i.e. put number of bytes in the list to proper pos */
        long2str(AVI_header+strl_start-4,nhb-strl_start);
    }

    /* Finish header list */
    long2str(AVI_header+hdrl_start-4,nhb-hdrl_start);

    /* Calculate the needed amount of junk bytes, output junk */
    njunk = HEADERBYTES - nhb - 8 - 12;

    /* Safety first: if njunk <= 0, somebody has played with
       HEADERBYTES without knowing what (s)he did.
       This is a fatal error 
     */
    if(njunk<=0)
    {
        AVILIB_ERR("AVI_close_output_file: # of header bytes too small\n");
        exit(1);
    }

    OUT4CC ("JUNK");
    OUTLONG(njunk);
    memset(AVI_header+nhb,0,njunk);

    //2001-11-14 added id string 

    if((size_t)njunk > strlen(id_str)+8) 
    {
        //sprintf(id_str, "%s-%s", PACKAGE, VERSION);
        //memcpy(AVI_header+nhb, id_str, strlen(id_str));
    }

    nhb += njunk;

    /* Start the movi list */
    OUT4CC ("LIST");
    OUTLONG(movi_len); /* Length of list in bytes */
    OUT4CC ("movi");

    /* Output the header, truncate the file to the number of bytes
       actually written, report an error if someting goes wrong 
     */
    
    avi_fflush(AVI->fdes);
    if (lseek(AVI->fdes, 0, SEEK_SET) < 0)
    {
        AVI_errno = AVI_ERR_CLOSE;
        return -1;
    }

    if (avi_write(AVI->fdes, (char *)AVI_header, HEADERBYTES) != HEADERBYTES)
    {
        AVI_errno = AVI_ERR_CLOSE;
        return -1;
    }
    
    avi_fflush(AVI->fdes);
    if (lseek(AVI->fdes, AVI->pos, SEEK_SET) < 0)
    {
        AVI_errno = AVI_ERR_CLOSE;
        return -1;
    }

    return 0;
}

/*
   写AVI头信息并关闭文件
   返回0成功，-1失败
*/
static int avi_close_output_file(avi_t *AVI)
{
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    int ret=0, njunk=0, sampsize=0, hasIndex=0, ms_per_frame=0, frate=0, idxerror=0, flag=0;
    unsigned long movi_len = 0;
    int hdrl_start=0, strl_start=0, j=0;
    unsigned char AVI_header[HEADERBYTES] = {0};
    long nhb = 0;

#ifdef INFO_LIST
    long info_len = 0;
    //   time_t calptr;
#endif

    /* Calculate length of movi list */

    movi_len = AVI->pos - HEADERBYTES + 4;

    /* Try to ouput the index entries. This may fail e.g. if no space
       is left on device. We will report this as an error, but we still
       try to write the header correctly (so that the file still may be
       readable in the most cases 
    */
    idxerror = 0;

    // fprintf(stderr, "pos=%#x, index_len=%ld             \n", AVI->pos, AVI->n_idx*16);
    avi_fflush(AVI->fdes);
    ret = avi_add_chunk(AVI, (unsigned char *)"idx1", (unsigned char *)AVI->idx, AVI->n_idx*16);
    hasIndex = (ret==0);
    avi_fflush(AVI->fdes);
    
    // fprintf(stderr, "pos=%#x, index_len=%d\n", AVI->pos, hasIndex);
    if(ret) 
    {
        idxerror = 1;
        AVI_errno = AVI_ERR_WRITE_INDEX;
    }

    /* 计算每帧的微秒数 */
    if(AVI->fps < 0.001) 
    {
        frate=0;
        ms_per_frame=0;
    }
    else
    {
        frate = (int) (FRAME_RATE_SCALE*AVI->fps + 0.5);
        ms_per_frame=(int) (1000000/AVI->fps + 0.5);
    }

    /* Prepare the file header */

    nhb = 0;

    /* The RIFF header */

    OUT4CC ("RIFF");
    OUTLONG(AVI->pos - 8);    /* # of bytes to follow */
    OUT4CC ("AVI ");

    /* Start the header list */

    OUT4CC ("LIST");
    OUTLONG(0);        /* Length of list in bytes, don't know yet */
    hdrl_start = nhb;  /* Store start position */
    OUT4CC ("hdrl");

    /* The main AVI header */

    /* The Flags in AVI File header */

#define AVIF_HASINDEX           0x00000010      /* Index at end of file */
#define AVIF_MUSTUSEINDEX       0x00000020
#define AVIF_ISINTERLEAVED      0x00000100
#define AVIF_TRUSTCKTYPE        0x00000800      /* Use CKType to find key frames */
#define AVIF_WASCAPTUREFILE     0x00010000
#define AVIF_COPYRIGHTED        0x00020000

    OUT4CC ("avih");
    OUTLONG(56);                 /* # of bytes to follow */
    OUTLONG(ms_per_frame);       /* Microseconds per frame */
    //ThOe ->0 
    //   OUTLONG(10000000);           /* MaxBytesPerSec, I hope this will never be used */
    OUTLONG(0);
    OUTLONG(0);                  /* PaddingGranularity (whatever that might be) */
    /* Other sources call it 'reserved' */
    flag = AVIF_ISINTERLEAVED;
    if(hasIndex) 
    {
        flag |= AVIF_HASINDEX;
    }
    if(hasIndex && AVI->must_use_index) 
    {
        flag |= AVIF_MUSTUSEINDEX;
    }
    OUTLONG(flag);               /* Flags */
    OUTLONG(AVI->video_frames);  /* TotalFrames */
    OUTLONG(0);                  /* InitialFrames */

    OUTLONG(AVI->anum+1);
    //   if (AVI->track[0].audio_bytes)
    //      { OUTLONG(2); }           /* Streams */
    //   else
    //      { OUTLONG(1); }           /* Streams */

    OUTLONG(0);                  /* SuggestedBufferSize */
    OUTLONG(AVI->width);         /* Width */
    OUTLONG(AVI->height);        /* Height */
    /* MS calls the following 'reserved': */
    OUTLONG(0);                  /* TimeScale:  Unit used to measure time */
    OUTLONG(0);                  /* DataRate:   Data rate of playback     */
    OUTLONG(0);                  /* StartTime:  Starting time of AVI data */
    OUTLONG(0);                  /* DataLength: Size of AVI data chunk    */


    /* Start the video stream list ---------------------------------- */

    OUT4CC ("LIST");
    OUTLONG(0);        /* Length of list in bytes, don't know yet */
    strl_start = nhb;  /* Store start position */
    OUT4CC ("strl");

    /* The video stream header */

    OUT4CC ("strh");
    OUTLONG(56);                 /* # of bytes to follow */
    OUT4CC ("vids");             /* Type */
    OUT4CC (AVI->compressor);    /* Handler */
    OUTLONG(0);                  /* Flags */
    OUTLONG(0);                  /* Reserved, MS says: wPriority, wLanguage */
    OUTLONG(0);                  /* InitialFrames */
    OUTLONG(FRAME_RATE_SCALE);              /* Scale */
    OUTLONG(frate);              /* Rate: Rate/Scale == samples/second */
    OUTLONG(0);                  /* Start */
    OUTLONG(AVI->video_frames);  /* Length */
    OUTLONG(0);                  /* SuggestedBufferSize */
    OUTLONG(-1);                 /* Quality */
    OUTLONG(0);                  /* SampleSize */
    OUTLONG(0);                  /* Frame */
    OUTLONG(0);                  /* Frame */
    //   OUTLONG(0);                  /* Frame */
    //OUTLONG(0);                  /* Frame */

    /* The video stream format */

    OUT4CC ("strf");
    OUTLONG(40);                 /* # of bytes to follow */
    OUTLONG(40);                 /* Size */
    OUTLONG(AVI->width);         /* Width */
    OUTLONG(AVI->height);        /* Height */
    OUTSHRT(1); OUTSHRT(24);     /* Planes, Count */
    OUT4CC (AVI->compressor);    /* Compression */
    // ThOe (*3)
    OUTLONG(AVI->width*AVI->height*3);  /* SizeImage (in bytes?) */
    OUTLONG(0);                  /* XPelsPerMeter */
    OUTLONG(0);                  /* YPelsPerMeter */
    OUTLONG(0);                  /* ClrUsed: Number of colors used */
    OUTLONG(0);                  /* ClrImportant: Number of colors important */

    /* Finish stream list, i.e. put number of bytes in the list to proper pos */

    long2str(AVI_header+strl_start-4,nhb-strl_start);

    /* Start the audio stream list ---------------------------------- */

    for(j=0; j<AVI->anum; ++j) 
    {
        //if (AVI->track[j].a_chans && AVI->track[j].audio_bytes)
        {
            sampsize = avi_sampsize(AVI, j);

            OUT4CC ("LIST");
            OUTLONG(0);        /* Length of list in bytes, don't know yet */
            strl_start = nhb;  /* Store start position */
            OUT4CC ("strl");

            /* The audio stream header */

            OUT4CC ("strh");
            OUTLONG(56);            /* # of bytes to follow */
            OUT4CC ("auds");

            // -----------
            // ThOe
            OUTLONG(0);             /* Format (Optionally) */
            // -----------

            OUTLONG(0);             /* Flags */
            OUTLONG(0);             /* Reserved, MS says: wPriority, wLanguage */
            OUTLONG(0);             /* InitialFrames */

            // ThOe /4
            OUTLONG(sampsize/4);      /* Scale */
            OUTLONG(1000*AVI->track[j].mp3rate/8);
            OUTLONG(0);             /* Start */
            OUTLONG(4*AVI->track[j].audio_bytes/sampsize);   /* Length */
            OUTLONG(0);             /* SuggestedBufferSize */
            OUTLONG(-1);            /* Quality */

            // ThOe /4
            OUTLONG(sampsize/4);    /* SampleSize */

            OUTLONG(0);             /* Frame */
            OUTLONG(0);             /* Frame */
            //OUTLONG(0);             /* Frame */
            //OUTLONG(0);             /* Frame */

            /* The audio stream format */

            OUT4CC ("strf");
            OUTLONG(16);                   /* # of bytes to follow */
            OUTSHRT(AVI->track[j].a_fmt);           /* Format */
            OUTSHRT(AVI->track[j].a_chans);         /* Number of channels */
            OUTLONG(AVI->track[j].a_rate);          /* SamplesPerSec */
            // ThOe
            OUTLONG(1000*AVI->track[j].mp3rate/8);
            //ThOe (/4)

            OUTSHRT(sampsize/4);           /* BlockAlign */

            OUTSHRT(AVI->track[j].a_bits);          /* BitsPerSample */

            /* Finish stream list, i.e. put number of bytes in the list to proper pos */
        }
        long2str(AVI_header+strl_start-4,nhb-strl_start);
    }

    /* Finish header list */

    long2str(AVI_header+hdrl_start-4,nhb-hdrl_start);

    // add INFO list --- (0.6.0pre4)

#ifdef INFO_LIST
    OUT4CC ("LIST");

    //FIXME
    info_len = MAX_INFO_STRLEN + 12;
    OUTLONG(info_len);
    OUT4CC ("INFO");

    //   OUT4CC ("INAM");
    //   OUTLONG(MAX_INFO_STRLEN);

    //   sprintf(id_str, "\t");
    //   memset(AVI_header+nhb, 0, MAX_INFO_STRLEN);
    //   memcpy(AVI_header+nhb, id_str, strlen(id_str));
    //   nhb += MAX_INFO_STRLEN;

    OUT4CC ("ISFT");
    OUTLONG(MAX_INFO_STRLEN);

    //sprintf(id_str, "%s-%s", PACKAGE, VERSION);
    memset(AVI_header+nhb, 0, MAX_INFO_STRLEN);
    //memcpy(AVI_header+nhb, id_str, strlen(id_str));
    nhb += MAX_INFO_STRLEN;

    //   OUT4CC ("ICMT");
    //   OUTLONG(MAX_INFO_STRLEN);

    //   calptr=time(NULL); 
    //   sprintf(id_str, "\t%s %s", ctime(&calptr), "");
    //   memset(AVI_header+nhb, 0, MAX_INFO_STRLEN);
    //   memcpy(AVI_header+nhb, id_str, 25);
    //   nhb += MAX_INFO_STRLEN;
#endif

    // ----------------------------

    /* Calculate the needed amount of junk bytes, output junk */

    njunk = HEADERBYTES - nhb - 8 - 12;

    /* Safety first: if njunk <= 0, somebody has played with
       HEADERBYTES without knowing what (s)he did.
       This is a fatal error */

    if(njunk<=0)
    {
        AVILIB_ERR("AVI_close_output_file: # of header bytes too small\n");
        exit(1);
    }

    OUT4CC ("JUNK");
    OUTLONG(njunk);
    memset(AVI_header+nhb,0,njunk);

    nhb += njunk;

    /* Start the movi list */

    OUT4CC ("LIST");
    OUTLONG(movi_len); /* Length of list in bytes */
    OUT4CC ("movi");

    /* Output the header, truncate the file to the number of bytes
       actually written, report an error if someting goes wrong 
     */

    avi_fflush(AVI->fdes);
    if (lseek(AVI->fdes, 0, SEEK_SET) < 0)
    {
        AVI_errno = AVI_ERR_CLOSE;
        return -1;
    }

    if (avi_write(AVI->fdes, (char *)AVI_header, HEADERBYTES) != HEADERBYTES)
    {
        AVI_errno = AVI_ERR_CLOSE;
        return -1;
    }

    avi_fflush(AVI->fdes);
    if (ftruncate(AVI->fdes,AVI->pos) < 0)
    {
        AVI_errno = AVI_ERR_CLOSE;
        return -1;
    }

    if(idxerror)
    {
        return -1;
    }

    return 0;
}

/* 写AVI音视频数据
   成功返回0， 失败返回-1, 对应错误信息可以通过AVI_errno获取
 */
static int avi_write_data(avi_t *AVI, char *data, unsigned long length, int audio, int keyframe)
{
    if (!AVI || !data)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    int n = 0;

    unsigned char astr[24] = {0};

    /* 检测文件长度是否超过最大文件长度限制 */
    if ( (AVI->pos + 8 + length + 8 + (AVI->n_idx+1)*16) > AVI_MAX_LEN ) 
    {
        AVI_errno = AVI_ERR_SIZELIM;
        return -1;
    }

    /* Add index entry */

    //set tag for current audio track
    snprintf((char *)astr, sizeof(astr), "0%1dwb", (int)(AVI->aptr+1));
    if(audio)
    {
        n = avi_add_index_entry(AVI,astr, 0x00, AVI->pos, length);
    }
    else
    {
        n = avi_add_index_entry(AVI,(unsigned char *)"00db", ((keyframe)?0x10:0x0), AVI->pos, length);
    }
    
    if(n) 
    {
        return -1;
    }

    /* Output tag and data */
    if(audio)
    {
        n = avi_add_chunk(AVI, astr, (unsigned char *)data, length);
    }
    else
    {
        n = avi_add_chunk(AVI, (unsigned char *)"00db", (unsigned char *)data, length);
    }

    if (n)
    {
        return -1;
    }

    return 0;
}

/* 写视频帧数据 */
int AVI_write_frame(avi_t *AVI, char *data, long bytes, int keyframe)
{
    if (!AVI || !data)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if(AVI->mode==AVI_MODE_READ) 
    { 
        AVI_errno = AVI_ERR_NOT_PERM; 
        return -1; 
    }
    
    unsigned long pos = 0;

    pos = AVI->pos;

    if(avi_write_data(AVI, data, bytes, 0, keyframe))
    {
        return -1;
    }
    
    AVI->last_pos = pos;
    AVI->last_len = bytes;
    AVI->video_frames++;

    return 0;
}

int AVI_dup_frame(avi_t *AVI)
{
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if(AVI->mode==AVI_MODE_READ) 
    { 
        AVI_errno = AVI_ERR_NOT_PERM; return -1; 
    }

    if(AVI->last_pos==0)
    {
        return 0; /* No previous real frame */
    }
    
    if(avi_add_index_entry(AVI, (unsigned char *)"00db", 0x10, AVI->last_pos, AVI->last_len)) 
    {
        return -1;
    }
    
    AVI->video_frames++;
    AVI->must_use_index = 1;
    
    return 0;
}

/* 写音频帧数据 */
int AVI_write_audio(avi_t *AVI, char *data, long bytes)
{
    if(AVI->mode==AVI_MODE_READ) 
    { 
        AVI_errno = AVI_ERR_NOT_PERM; 
        return -1; 
    }

    if(avi_write_data(AVI, data, bytes, 1, 0))
    {
        return -1;
    }
    
    AVI->track[AVI->aptr].audio_bytes += bytes;

    return 0;
}

/* 追加一个音频帧数据 */
int AVI_append_audio(avi_t *AVI, char *data, long bytes)
{
    if (!AVI || !data)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }
    
    if(AVI->mode == AVI_MODE_READ) 
    { 
        AVI_errno = AVI_ERR_NOT_PERM; 
        return -1; 
    }
    
    long i=0, length=0, pos=0;
    unsigned char c[4] = {0};

    // update last index entry:

    --AVI->n_idx;
    length = str2ulong(AVI->idx[AVI->n_idx]+12);
    pos    = str2ulong(AVI->idx[AVI->n_idx]+8);

    // 更新
    long2str(AVI->idx[AVI->n_idx]+12,length+bytes);   

    ++AVI->n_idx;

    AVI->track[AVI->aptr].audio_bytes += bytes;

    // 更新chunk头
    
    avi_fflush(AVI->fdes);
    lseek(AVI->fdes, pos+4, SEEK_SET);
    
    long2str(c, length+bytes);     
    avi_write(AVI->fdes, (char *)c, 4);
    
    avi_fflush(AVI->fdes);
    lseek(AVI->fdes, pos+8+length, SEEK_SET);

    i=PAD_EVEN(length + bytes);

    bytes = i - length;
    avi_write(AVI->fdes, data, bytes);
    AVI->pos = pos + 8 + i;

    return 0;
}

/* 剩余的AVI字节数(2G减去已经写的AVI数据) */
long AVI_bytes_remain(avi_t *AVI)
{
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if(AVI->mode == AVI_MODE_READ) 
    {
        return 0;
    }

    return (AVI_MAX_LEN - (AVI->pos + 8 + 16*AVI->n_idx));
}

/* 已经写的AVI数据 */
long AVI_bytes_written(avi_t *AVI)
{
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if(AVI->mode == AVI_MODE_READ) 
    {
        return 0;
    }
    
    return (AVI->pos + 8 + 16*AVI->n_idx);
}

int AVI_set_audio_track(avi_t *AVI, int track)
{
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if(track < 0 || track + 1 > AVI->anum)
    {
        return (-1);
    }
    
    //this info is not written to file anyway
    AVI->aptr = track;
    
    return 0;
}

int AVI_get_audio_track(avi_t *AVI)
{
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    return (AVI->aptr);
}

/*******************************************************************
 *                                                                 *
 *    Utilities for reading video and audio from an AVI File       *
 *                                                                 *
 *******************************************************************/

int AVI_close(avi_t *AVI)
{
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    int ret=0, i=0;

    /* If the file was open for writing, the header and index still have to be written */
    if(AVI->mode == AVI_MODE_WRITE)
    {
        ret = avi_close_output_file(AVI);
    }
    else
    {
        ret = 0;
    }
    
    /* Even if there happened an error, we first clean up */
    close(AVI->fdes);
    if(AVI->idx)
    {
        free(AVI->idx);
        AVI->idx = NULL;
    }

    if(AVI->video_index)
    {
        free(AVI->video_index);
        AVI->video_index = NULL;
    }

    //FIXME
    //if(AVI->audio_index) {free(AVI->audio_index); AVI->audio_index = NULL;}
    if (AVI->bitmap_info_header)
    {
        free(AVI->bitmap_info_header);
        AVI->bitmap_info_header = NULL;
    }
    for (i = 0; i < AVI->anum; i++) 
    {
        if (AVI->wave_format_ex[i])
        {
            free(AVI->wave_format_ex[i]);
            AVI->wave_format_ex[i] = NULL;
        }
        if (AVI->track[i].audio_chunks)
        {
            free(AVI->track[i].audio_index);
            AVI->track[i].audio_index = NULL;
        }
    }

    free(AVI);
    AVI = NULL;

    return ret;
}

#define ERR_EXIT(x) \
{                   \
    AVI_close(AVI); \
    AVI = NULL;     \
    AVI_errno = x;  \
    return -1;      \
}

/* 以只读的方式打开一个AVI文件，权限为只读，一般用作读取AVI数据 */
avi_t *AVI_open_input_file(const char *filename, int getIndex)
{
    if (NULL == filename)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return NULL;
    }

    avi_t *AVI = NULL;

    /* Create avi_t structure */

    AVI = (avi_t *) malloc(sizeof(avi_t));
    if(AVI == NULL)
    {
        AVI_errno = AVI_ERR_NO_MEM;
        return NULL;
    }
    memset((void *)AVI, 0, sizeof(avi_t));

    AVI->mode = AVI_MODE_READ; /* open for reading */

    /* Open the file */
    AVI->fdes = open(filename, O_RDONLY);
    if(AVI->fdes < 0)
    {
        AVI_errno = AVI_ERR_OPEN;
        free(AVI);
        AVI = NULL;
        return NULL;
    }

    /* 在出现失败的时候已经释放相关资源，此处不能再次释放 */
    if (avi_parse_input_file(AVI, getIndex))
    {
        AVILIB_ERR("Fail to called avi_parse_input_file!\n");
        AVI = NULL;
        return NULL;
    }

    AVI->aptr = 0; //reset  

    return AVI;
}

avi_t *AVI_open_fd(int fd, int getIndex)
{
    if (-1 == fd)
    {
        AVILIB_ERR("param err! fd = %d\n", fd);
        return NULL;
    }

    avi_t *AVI = NULL;

    /* Create avi_t structure */

    AVI = (avi_t *)malloc(sizeof(avi_t));
    if(AVI==NULL)
    {
        AVI_errno = AVI_ERR_NO_MEM;
        return NULL;
    }
    memset((void *)AVI, 0, sizeof(avi_t));

    AVI->mode = AVI_MODE_READ; /* open for reading */

    // file alread open
    AVI->fdes = fd;

    /* 在出现失败的时候已经释放相关资源，此处不能再次释放 */
    if (avi_parse_input_file(AVI, getIndex))
    {
        AVILIB_ERR("Fail to called avi_parse_input_file!\n");
        AVI = NULL;
        return NULL;
    }

    AVI->aptr = 0; //reset

    return AVI;
}

int avi_parse_input_file(avi_t *AVI, int getIndex)
{
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return (-1);
    }

    long i=0, rate=0, scale=0, idx_type=0;
    off_t n = 0;
    unsigned char *hdrl_data = NULL;
    long header_offset=0, hdrl_len=0;
    long nvi=0, ioff=0;
    long nai[AVI_MAX_TRACKS] = {0};
    long tot[AVI_MAX_TRACKS] = {0};
    int j = 0;
    int lasttag = 0;
    int vids_strh_seen = 0;
    int vids_strf_seen = 0;
    int auds_strh_seen = 0;
    //  int auds_strf_seen = 0;
    int num_stream = 0;
    char data[256] = {0};

    /* 读最开始的的12个字节，检测该文件是否为AVI格式文件 */
    if( avi_read(AVI->fdes,data,12) != 12 )
    {
        ERR_EXIT(AVI_ERR_READ)
    }
    if( strncasecmp(data, "RIFF", 4) !=0 || strncasecmp(data+8, "AVI ", 4) !=0 )
    {
         ERR_EXIT(AVI_ERR_NO_AVI)
    }

    /* 浏览AVI文件获取文件头列表,'movi'列表的开始位置和当前的idx1 tag */
    hdrl_data = 0;

    for(;;)
    {
        if(avi_read(AVI->fdes, data, 8) != 8)
        {
            break; /* 假定到了EOF文件结尾 */
        }
        n = str2ulong((unsigned char *)data+4);
        n = PAD_EVEN(n);

        /*  */
        if(strncasecmp(data,"LIST",4) == 0)
        {
            if( avi_read(AVI->fdes,data,4) != 4)
            {
                ERR_EXIT(AVI_ERR_READ)
            }
            n -= 4;
            if(strncasecmp(data,"hdrl",4) == 0)
            {
                hdrl_len = n;
                hdrl_data = (unsigned char *) malloc(n);
                if(hdrl_data==0)
                {
                   ERR_EXIT(AVI_ERR_NO_MEM);
                }
                memset(hdrl_data, 0, n);

                // offset of header
                header_offset = lseek(AVI->fdes,0,SEEK_CUR);

                if(avi_read(AVI->fdes,(char *)hdrl_data,n) != n)
                {
                    ERR_EXIT(AVI_ERR_READ)
                }
            }
            else if(strncasecmp(data,"movi",4) == 0)
            {
                AVI->movi_start = lseek(AVI->fdes,0,SEEK_CUR);
                lseek(AVI->fdes,n,SEEK_CUR);
            }
            else
            {
                lseek(AVI->fdes,n,SEEK_CUR);
            }
        }
        else if(strncasecmp(data,"idx1",4) == 0)
        {
            /* n 必须是16的倍数, but the reading does not
               break if this is not the case */

            AVI->n_idx = AVI->max_idx = n/16;
            AVI->idx = (unsigned  char((*)[16])) malloc (n);
            if(NULL == AVI->idx)
            {
                ERR_EXIT(AVI_ERR_NO_MEM)
            }

            memset(AVI->idx, 0, n);
            if(avi_read(AVI->fdes, (char *) AVI->idx, n) != n ) 
            {
               ERR_EXIT(AVI_ERR_READ)
            }
        }
        else
        {            
            lseek(AVI->fdes,n,SEEK_CUR);
        }
    }

    if(!hdrl_data)
    {
        ERR_EXIT(AVI_ERR_NO_HDRL)
    }

    if(!AVI->movi_start)
    {
        ERR_EXIT(AVI_ERR_NO_MOVI)
    }
    
    /* 解析头列表 */
    for(i=0; i<hdrl_len; )
    {
        /* 忽略列表tags */
        if(strncasecmp((char *)hdrl_data+i, "LIST", 4)==0) 
        { 
            i+= 12; 
            continue; 
        }

        n = str2ulong(hdrl_data+i+4);
        n = PAD_EVEN(n);

        /* 解析tag和对应的参数 */
        if(0 == strncasecmp((char *)hdrl_data+i, "strh", 4))
        {
            i += 8;
            if(strncasecmp((char *)hdrl_data+i,"vids",4) == 0 && !vids_strh_seen)
            {
                memcpy(AVI->compressor,hdrl_data+i+4,4);
                AVI->compressor[4] = 0;
                AVI->v_codech_off = header_offset + i+4;

                scale = str2ulong(hdrl_data+i+20);
                rate  = str2ulong(hdrl_data+i+24);
                if(0 != scale)
                {
                    AVI->fps = (double)rate/(double)scale;
                }

                AVI->video_frames = str2ulong(hdrl_data+i+32);
                AVI->video_strn = num_stream;
                AVI->max_len = 0;
                vids_strh_seen = 1;
                lasttag = 1; /* vids */
            }
            else if (strncasecmp ((char *)hdrl_data+i,"auds",4) ==0 && ! auds_strh_seen)
            {
                //inc audio tracks
                AVI->aptr=AVI->anum;
                ++AVI->anum;

                if(AVI->anum > AVI_MAX_TRACKS) 
                {
                    AVILIB_ERR("error - only %d audio tracks supported\n", AVI_MAX_TRACKS);
                    return (-1);
                }

                AVI->track[AVI->aptr].audio_bytes = str2ulong(hdrl_data+i+32)*avi_sampsize(AVI, 0);
                AVI->track[AVI->aptr].audio_strn = num_stream;
                //       auds_strh_seen = 1;
                lasttag = 2; /* auds */

                // ThOe
                AVI->track[AVI->aptr].a_codech_off = header_offset + i;

            }
            else
            {
                lasttag = 0;
            }
            num_stream++;
        }
        else if(strncasecmp((char *)hdrl_data+i, "strf", 4)==0)
        {
            i += 8;
            if(lasttag == 1)
            {
                BITMAPINFOHEADER_avilib bih;

                memcpy(&bih, hdrl_data + i, sizeof(BITMAPINFOHEADER_avilib));
                AVI->bitmap_info_header = (BITMAPINFOHEADER_avilib *)malloc(bih.bi_size);
                if (AVI->bitmap_info_header != NULL)
                {
                    memset(AVI->bitmap_info_header, 0, bih.bi_size);
                    memcpy(AVI->bitmap_info_header, hdrl_data + i, bih.bi_size);
                }

                AVI->width  = str2ulong(hdrl_data+i+4);
                AVI->height = str2ulong(hdrl_data+i+8);
                vids_strf_seen = 1;
                //ThOe
                AVI->v_codecf_off = header_offset + i+16;

                memcpy(AVI->compressor2, hdrl_data+i+16, 4);
                AVI->compressor2[4] = 0;
            }
            else if(lasttag == 2)
            {
                int  wfes = 0;                
                char *nwfe = NULL;
                WAVEFORMATEX_avilib *wfe = NULL;

                if ((unsigned int)(hdrl_len - i) < sizeof(WAVEFORMATEX_avilib))
                {
                    wfes = hdrl_len - i;
                }
                else
                {
                    wfes = sizeof(WAVEFORMATEX_avilib);
                }
                wfe = (WAVEFORMATEX_avilib *)malloc(sizeof(WAVEFORMATEX_avilib));
                if (wfe != NULL) 
                {
                    memset(wfe, 0, sizeof(WAVEFORMATEX_avilib));
                    memcpy(wfe, hdrl_data + i, wfes);
                    if (wfe->cb_size != 0) 
                    {
                        nwfe = (char *)realloc(wfe, sizeof(WAVEFORMATEX_avilib) + wfe->cb_size);
                        if (nwfe != 0) 
                        {                            
                            off_t lpos = lseek(AVI->fdes, 0, SEEK_CUR);
                            lseek(AVI->fdes, header_offset + i + sizeof(WAVEFORMATEX_avilib), SEEK_SET);
                            wfe = (WAVEFORMATEX_avilib *)nwfe;
                            nwfe = &nwfe[sizeof(WAVEFORMATEX_avilib)];
                            avi_read(AVI->fdes, nwfe, wfe->cb_size);                            
                            lseek(AVI->fdes, lpos, SEEK_SET);
                        }
                    }
                    AVI->wave_format_ex[AVI->aptr] = wfe;
                }

                AVI->track[AVI->aptr].a_fmt   = str2ushort(hdrl_data+i  );

                //ThOe
                AVI->track[AVI->aptr].a_codecf_off = header_offset + i;

                AVI->track[AVI->aptr].a_chans = str2ushort(hdrl_data+i+2);
                AVI->track[AVI->aptr].a_rate  = str2ulong (hdrl_data+i+4);
                //ThOe: read mp3bitrate
                AVI->track[AVI->aptr].mp3rate = 8*str2ulong(hdrl_data+i+8)/1000;
                //:ThOe
                AVI->track[AVI->aptr].a_bits  = str2ushort(hdrl_data+i+14);
                // auds_strf_seen = 1;
            }
            lasttag = 0;
        }
        else
        {
            i += 8;
            lasttag = 0;
        }

        i += n;
    }

    if (hdrl_data)
    {
        free(hdrl_data);
        hdrl_data = NULL;
    }
    
    if(!vids_strh_seen || !vids_strf_seen) 
    {
        ERR_EXIT(AVI_ERR_NO_VIDS)
    }

    AVI->video_tag[0] = AVI->video_strn/10 + '0';
    AVI->video_tag[1] = AVI->video_strn%10 + '0';
    AVI->video_tag[2] = 'd';
    AVI->video_tag[3] = 'b';

    /* Audio tag is set to "99wb" if no audio present */
    if(!AVI->track[0].a_chans)
    {
        AVI->track[0].audio_strn = 99;
    }

    for(j=0; j<AVI->anum; ++j) 
    {
        AVI->track[j].audio_tag[0] = (j+1)/10 + '0';
        AVI->track[j].audio_tag[1] = (j+1)%10 + '0';
        AVI->track[j].audio_tag[2] = 'w';
        AVI->track[j].audio_tag[3] = 'b';
    }
    
    lseek(AVI->fdes,AVI->movi_start,SEEK_SET);

    /* get index if wanted */
    if(!getIndex)
    {
        return (0);
    }
    
    /* 如果文件有一个idx1，则检测该idx1是否关联着文件开头或者movi列表开头 */
    idx_type = 0;

    if(AVI->idx)
    {
        off_t pos = 0, len = 0;

        /* 在idx1里面寻找第一个视频帧并且查找其在文件里面的位置 */
        for(i=0;i<AVI->n_idx;i++)
        {
            if( strncasecmp((char *)AVI->idx[i],(char *)AVI->video_tag,3)==0 ) 
            {
                break;
            }
        }

        if(i>=AVI->n_idx)
        {
           ERR_EXIT(AVI_ERR_NO_VIDS)
        }
        pos = str2ulong(AVI->idx[i]+ 8);
        len = str2ulong(AVI->idx[i]+12);
        
        lseek(AVI->fdes,pos,SEEK_SET);
        if(avi_read(AVI->fdes,data,8)!=8)
        {
           ERR_EXIT(AVI_ERR_READ)
        }
        if( strncasecmp(data,(char *)AVI->idx[i],4)==0 && str2ulong((unsigned char *)data+4)==len )
        {
            idx_type = 1; /* Index from start of file */
        }
        else
        {
            lseek(AVI->fdes,pos+AVI->movi_start-4,SEEK_SET);
            if(avi_read(AVI->fdes,data,8)!=8)
            {
               ERR_EXIT(AVI_ERR_READ)
            }

            if( strncasecmp(data,(char *)AVI->idx[i],4)==0 && str2ulong((unsigned char *)data+4)==len )
            {
                idx_type = 2; /* Index from start of movi list */
            }
        }
        /* idx_type remains 0 if neither of the two tests above succeeds */
    }

    if(idx_type == 0)
    {
        /* 我们必须遍历查找整个文件去获取索引 */
        lseek(AVI->fdes, AVI->movi_start, SEEK_SET);

        AVI->n_idx = 0;

        for(;;)
        {
            if( avi_read(AVI->fdes,data,8) != 8 )
            {
                break;
            }
            n = str2ulong((unsigned char *)data+4);

            /* 忽略movi列表里面的子列表 */
            if(strncasecmp(data,"LIST",4)==0)
            {
                lseek(AVI->fdes,4,SEEK_CUR);
                continue;
            }

            /* Check if we got a tag ##db, ##dc or ##wb */
            if( ( (data[2]=='d' || data[2]=='D') && (data[3]=='b' || data[3]=='B' || data[3]=='c' || data[3]=='C') ) 
             || ( (data[2]=='w' || data[2]=='W') && (data[3]=='b' || data[3]=='B') ) )
            {        
                avi_add_index_entry(AVI,(unsigned char *)data, 0, lseek(AVI->fdes, 0, SEEK_CUR)-8, n);
            }

            lseek(AVI->fdes,PAD_EVEN(n),SEEK_CUR);
        }
        idx_type = 1;
    }

    /* 创建音频和视频索引数组 */
    nvi = 0;
    for(j=0; j<AVI->anum; ++j)
    {
        nai[j] = 0;
    }
    
    for(i=0;i<AVI->n_idx;i++) 
    {
        if(strncasecmp((char *)AVI->idx[i],AVI->video_tag,3) == 0)
        {
            nvi++;
        }

        for(j=0; j<AVI->anum; ++j) 
        {
            if(strncasecmp((char *)AVI->idx[i], AVI->track[j].audio_tag,4) == 0) 
            {
                nai[j]++;
            }
        }
    }

    AVI->video_frames = nvi;
    for(j=0; j<AVI->anum; ++j) 
    {
        AVI->track[j].audio_chunks = nai[j];
    }

    //fprintf(stderr, "chunks = %ld %d %s\n", AVI->track[0].audio_chunks, AVI->anum, AVI->track[0].audio_tag);

    if(AVI->video_frames==0)
    {
        ERR_EXIT(AVI_ERR_NO_VIDS);
    }
    AVI->video_index = (video_index_entry *) malloc(nvi*sizeof(video_index_entry));
    if(AVI->video_index==0) 
    {
        ERR_EXIT(AVI_ERR_NO_MEM);
    }

    memset(AVI->video_index, 0, nvi*sizeof(video_index_entry));
    for(j=0; j<AVI->anum; ++j) 
    {
        if(AVI->track[j].audio_chunks) 
        {
            AVI->track[j].audio_index = (audio_index_entry *) malloc((nai[j]+1)*sizeof(audio_index_entry));
            memset(AVI->track[j].audio_index, 0, (nai[j]+1)*(sizeof(audio_index_entry)));
            if(AVI->track[j].audio_index==0) 
            {
                ERR_EXIT(AVI_ERR_NO_MEM);
            }
        }
    }   

    nvi = 0;
    for(j=0; j<AVI->anum; ++j)
    {
        nai[j] = tot[j] = 0;
    }
    
    ioff = idx_type == 1 ? 8 : AVI->movi_start+4;

    for(i=0; i<AVI->n_idx; i++) 
    {
        // 视频
        if(strncasecmp((char *)AVI->idx[i],AVI->video_tag,3) == 0) 
        {
            AVI->video_index[nvi].key = str2ulong(AVI->idx[i]+ 4);
            AVI->video_index[nvi].pos = str2ulong(AVI->idx[i]+ 8)+ioff;
            AVI->video_index[nvi].len = str2ulong(AVI->idx[i]+12);
            nvi++;
        }

        // 音频
        for(j=0; j<AVI->anum; ++j) 
        {
            if(strncasecmp((char *)AVI->idx[i],AVI->track[j].audio_tag,4) == 0) 
            {
                AVI->track[j].audio_index[nai[j]].pos = str2ulong(AVI->idx[i]+ 8)+ioff;
                AVI->track[j].audio_index[nai[j]].len = str2ulong(AVI->idx[i]+12);
                AVI->track[j].audio_index[nai[j]].tot = tot[j];
                tot[j] += AVI->track[j].audio_index[nai[j]].len;
                nai[j]++;
            }
        }
    }

    for(j=0; j<AVI->anum; ++j)
    {
        AVI->track[j].audio_bytes = tot[j];
    }
    
    /* 文件指针复位 */
    lseek(AVI->fdes, AVI->movi_start, SEEK_SET);
    AVI->video_pos = 0;

    return (0);
}

/* 获取视频帧数 */
long AVI_video_frames(avi_t *AVI)
{
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    return (AVI->video_frames);
}

/* 获取视频帧宽度 */
int  AVI_video_width(avi_t *AVI)
{
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    return (AVI->width);
}

/* 获取视频帧高度 */
int  AVI_video_height(avi_t *AVI)
{
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    return AVI->height;
}

/* 获取视频帧率 */
double AVI_frame_rate(avi_t *AVI)
{
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    return (AVI->fps);
}

/* 获取视频压缩方式 */
char* AVI_video_compressor(avi_t *AVI)
{
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return NULL;
    }

    return (AVI->compressor2);
}

/* 获取视频最大chunk */
long AVI_max_video_chunk(avi_t *AVI)
{
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    return (AVI->max_len);
}

int AVI_audio_tracks(avi_t *AVI)
{
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    return (AVI->anum);
}

/* 获取音频声道数, 0表示没有音频 */
int AVI_audio_channels(avi_t *AVI)
{
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    return (AVI->track[AVI->aptr].a_chans);
}

/* 获取音频mp3码率(kbs) */
long AVI_audio_mp3rate(avi_t *AVI)
{
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    return (AVI->track[AVI->aptr].mp3rate);
}

/* 获取音频每秒的采样数据位宽(8bit, 16bit, ...) */
int AVI_audio_bits(avi_t *AVI)
{
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    return (AVI->track[AVI->aptr].a_bits);
}

/* 获取音频编码格式, 详见宏定义(若想avi音视频同步，则需要使用pcm编码) */
int AVI_audio_format(avi_t *AVI)
{
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    return (AVI->track[AVI->aptr].a_fmt);
}

/* 获取音频采样率(8k,16k,...) */
long AVI_audio_rate(avi_t *AVI)
{
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    return (AVI->track[AVI->aptr].a_rate);
}

/* 获取音频字节数 */
long AVI_audio_bytes(avi_t *AVI)
{
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    return (AVI->track[AVI->aptr].audio_bytes);
}

/* 获取音频chunks总数 */
long AVI_audio_chunks(avi_t *AVI)
{
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    return (AVI->track[AVI->aptr].audio_chunks);
}

/* 获取音频音频编码绝对偏移信息 */
long AVI_audio_codech_offset(avi_t *AVI)
{    
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    return (AVI->track[AVI->aptr].a_codech_off);
}

/* 获取音频音频编码绝对偏移信息 */
long AVI_audio_codecf_offset(avi_t *AVI)
{
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    return (AVI->track[AVI->aptr].a_codecf_off);
}

long  AVI_video_codech_offset(avi_t *AVI)
{
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    return (AVI->v_codech_off);
}

long  AVI_video_codecf_offset(avi_t *AVI)
{
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    return (AVI->v_codecf_off);
}
    
/* 获取帧号为frame帧的大小 */
long AVI_frame_size(avi_t *AVI, long frame)
{
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if(AVI->mode == AVI_MODE_WRITE) 
    { 
        AVI_errno = AVI_ERR_NOT_PERM; 
        return -1; 
    }
    if(!AVI->video_index)         
    { 
        AVI_errno = AVI_ERR_NO_IDX;   
        return -1; 
    }

    if(frame < 0 || frame >= AVI->video_frames)
    {
        return 0;
    }
    
    return(AVI->video_index[frame].len);
}

/* 获取帧号为frame音频帧的大小 */
long AVI_audio_size(avi_t *AVI, long frame)
{    
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if(AVI->mode == AVI_MODE_WRITE) 
    { 
        AVI_errno = AVI_ERR_NOT_PERM; 
        return -1; 
    }
    
    if(!AVI->track[AVI->aptr].audio_index) { AVI_errno = AVI_ERR_NO_IDX; return -1; }

    if(frame < 0 || frame >= AVI->track[AVI->aptr].audio_chunks) return 0;
    
    return(AVI->track[AVI->aptr].audio_index[frame].len);
}

/* 获取当前视频帧的定位点 */
long AVI_get_video_position(avi_t *AVI, long frame)
{
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if(AVI->mode==AVI_MODE_WRITE)
    {
        AVI_errno = AVI_ERR_NOT_PERM;
        return -1;
    }

    if(!AVI->video_index)
    {
        AVI_errno = AVI_ERR_NO_IDX;
        return -1;
    }

    if(frame < 0 || frame >= AVI->video_frames)
    {
        return 0;
    }
    
    return(AVI->video_index[frame].pos);
}

/* 将视频帧索引定位到文件头，一般在读取avi的时候用到 */
int AVI_seek_start(avi_t *AVI)
{
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if(AVI->mode==AVI_MODE_WRITE)
    { 
        AVI_errno = AVI_ERR_NOT_PERM; 
        return -1; 
    }

    lseek(AVI->fdes, AVI->movi_start, SEEK_SET);
    AVI->video_pos = 0;
    
    return 0;
}


/* 设置音频帧数据的码率 */
int AVI_set_audio_bitrate(avi_t *AVI, long bitrate)
{
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if(AVI->mode==AVI_MODE_READ)
    { 
        AVI_errno = AVI_ERR_NOT_PERM; 
        return -1; 
    }

    AVI->track[AVI->aptr].mp3rate = bitrate;
    
    return 0;
}

/* 将指针定位到frame帧的位置，由此处开始获取视频帧 */
int AVI_set_video_position(avi_t *AVI, long frame)
{
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if(AVI->mode==AVI_MODE_WRITE)
    { 
        AVI_errno = AVI_ERR_NOT_PERM; 
        return -1; 
    }

    if(!AVI->video_index)
    {
        AVI_errno = AVI_ERR_NO_IDX;
        return -1;
    }

    if (frame < 0 ) frame = 0;
    AVI->video_pos = frame;
    
    return 0;
}

/* 读取一帧视频数据 */
long AVI_read_frame(avi_t *AVI, char *vidbuf, int *keyframe)
{
    if (!AVI || !vidbuf || !keyframe)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if(AVI->mode==AVI_MODE_WRITE)
    { 
        AVI_errno = AVI_ERR_NOT_PERM; 
        return -1; 
    }

    if(!AVI->video_index)
    {
        AVI_errno = AVI_ERR_NO_IDX;
        return -1;
    }

    if(AVI->video_pos < 0 || AVI->video_pos >= AVI->video_frames)
    {
        return -1;
    }

    long n = 0;
    n = AVI->video_index[AVI->video_pos].len;

    *keyframe = (AVI->video_index[AVI->video_pos].key==0x10) ? 1:0;

    lseek(AVI->fdes, AVI->video_index[AVI->video_pos].pos, SEEK_SET);

    if (avi_read(AVI->fdes,vidbuf,n) != (size_t)n)
    {
        AVI_errno = AVI_ERR_READ;
        return -1;
    }

    AVI->video_pos++;

    return n;
}

/* 将指针定位到byte的位置，由此处开始获取音频帧 */
int AVI_set_audio_position(avi_t *AVI, long byte)
{
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if(AVI->mode==AVI_MODE_WRITE)
    {
        AVI_errno = AVI_ERR_NOT_PERM;
        return -1;
    }

    if(!AVI->track[AVI->aptr].audio_index)
    {
        AVI_errno = AVI_ERR_NO_IDX;
        return -1;
    }

    if(byte < 0) 
    {
        byte = 0;
    }

    /* 二分法寻找音频的chunks块 */
    long n0=0, n1=0, n=0;

    n0 = 0;
    n1 = AVI->track[AVI->aptr].audio_chunks;

    while(n0 < n1-1)
    {
        n = (n0 + n1) / 2;
        if(AVI->track[AVI->aptr].audio_index[n].tot > byte)
        {
            n1 = n;
        }
        else
        {
            n0 = n;
        }
    }

    AVI->track[AVI->aptr].audio_posc = n0;
    AVI->track[AVI->aptr].audio_posb = byte - AVI->track[AVI->aptr].audio_index[n0].tot;

    return 0;
}

/* 读取一帧音频数据 */
long AVI_read_audio(avi_t *AVI, char *audbuf, long bytes)
{
    if (!AVI || !audbuf)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if(AVI->mode==AVI_MODE_WRITE)
    {
        AVI_errno = AVI_ERR_NOT_PERM;
        return -1;
    }

    if(!AVI->track[AVI->aptr].audio_index)
    {
        AVI_errno = AVI_ERR_NO_IDX;
        return -1;
    }

    long nr=0, pos=0, left=0, todo=0;

    /* 读的总字节数 */
    while(bytes > 0)
    {
        left = AVI->track[AVI->aptr].audio_index[AVI->track[AVI->aptr].audio_posc].len - AVI->track[AVI->aptr].audio_posb;
        if(left == 0)
        {
            if(AVI->track[AVI->aptr].audio_posc>=AVI->track[AVI->aptr].audio_chunks-1)
            {
                return nr;
            }
            AVI->track[AVI->aptr].audio_posc++;
            AVI->track[AVI->aptr].audio_posb = 0;
            continue;
        }
        if(bytes<left)
        {
            todo = bytes;
        }
        else
        {
            todo = left;
        }
        pos = AVI->track[AVI->aptr].audio_index[AVI->track[AVI->aptr].audio_posc].pos + AVI->track[AVI->aptr].audio_posb;
        lseek(AVI->fdes, pos, SEEK_SET);
        if (avi_read(AVI->fdes,audbuf+nr,todo) != (size_t)todo)
        {
            AVI_errno = AVI_ERR_READ;
            return -1;
        }
        bytes -= todo;
        nr    += todo;
        AVI->track[AVI->aptr].audio_posb += todo;
    }

    return nr;
}

/* 读取一个音频chunk块 */
long AVI_read_audio_chunk(avi_t *AVI, char *audbuf)
{
    if (!AVI || !audbuf)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    long pos = 0; 
    long left = 0;

    if(AVI->mode==AVI_MODE_WRITE) 
    { 
        AVI_errno = AVI_ERR_NOT_PERM; 
        return -1; 
    }
    
    if(!AVI->track[AVI->aptr].audio_index)         
    { 
        AVI_errno = AVI_ERR_NO_IDX;   
        return -1; 
    }

    if (AVI->track[AVI->aptr].audio_index[AVI->track[AVI->aptr].audio_posc].len == 0) 
    {
        return 0;
    }

    left = AVI->track[AVI->aptr].audio_index[AVI->track[AVI->aptr].audio_posc].len - AVI->track[AVI->aptr].audio_posb;

    if (audbuf == NULL)
    {
        return left;
    }
    
    if(left == 0)
    {
        return 0;
    }

    pos = AVI->track[AVI->aptr].audio_index[AVI->track[AVI->aptr].audio_posc].pos + AVI->track[AVI->aptr].audio_posb;

    lseek(AVI->fdes, pos, SEEK_SET);

    if (avi_read(AVI->fdes,audbuf,left) != (size_t)left)
    {
        AVI_errno = AVI_ERR_READ;
        return -1;
    }

    AVI->track[AVI->aptr].audio_posc++;
    AVI->track[AVI->aptr].audio_posb = 0;

    return left;
}

/* 设置视频定位的帧号，调用该接口之后，就可以直接调用AVI_read_data接口以便获取复合流信息
 * 这里以视频帧为准，即frame全部以视频帧为基准进行操作，具体步骤如下:
 * 1. 指定当前需要定位到哪一个视频帧，移动文件指针进行定位；
 * 2. 调用AVI_read_data接口弹出frame指定的视频帧；
 * 3. 根据返回值判断当前调用的是什么类型帧；
 * 注意: AVI_read_data接口获取视频帧和AVI_read_frame接口获取视频帧数据不同；
 *       同理, AVI_read_frame接口和AVI_read_audio接口获取的音频帧数据不同，
 *       所以，在定位的时候，需要将文件指针偏移之后，前移8个字节的tag头
 */
int AVI_set_video_chunk_position(avi_t *AVI, int frame)
{
    if (NULL == AVI)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    if(AVI->mode == AVI_MODE_WRITE) 
    {
        AVI_errno = AVI_ERR_NOT_PERM; 
        return -1;
    }

    if(!AVI->video_index)
    {
        AVI_errno = AVI_ERR_NO_IDX;
        return -1;
    }

    AVI->video_pos = frame < 0 ? 0 : frame;
    lseek(AVI->fdes, AVI->video_index[AVI->video_pos].pos-8, SEEK_SET);

    return 0;
}

/* 特殊的函数去读取下一个音频或视频块chunk(针对没有索引的文件) */
int AVI_read_data(avi_t *AVI, char *frame_buf, long max_frame_size, long *frame_size, int *keyframe)
{
    if (!AVI || !frame_buf || !keyframe || !frame_size)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return -1;
    }

    /*
     * Return codes:
     *    1 = 读的是视频
     *    2 = 读的是音频
     *    0 = 到达文件尾
     *   -1 = 视频缓冲区太小
     *   -2 = 音频缓冲区太小
     */

    off_t n = 0;
    char data[8] = {0};

    if(AVI->mode == AVI_MODE_WRITE) 
    {
        AVI_errno = AVI_ERR_NOT_PERM; 
        return -1;
    }
    
    for(;;)
    {
        /* 读tag和长度 */
        if( avi_read(AVI->fdes, data, 8) != 8 )
        {
            return 0;
        }
        
        /* 忽略list tag */
        if(strncasecmp(data, "LIST", 4) == 0)
        {
            lseek(AVI->fdes, 4, SEEK_CUR);
            continue;
        }

        n = PAD_EVEN(str2ulong((unsigned char *)data+4));

        if(strncasecmp(data, AVI->video_tag, 3) == 0)
        {
            *frame_size = n;            
            *keyframe = (AVI->video_index[AVI->video_pos].key==0x10) ? 1:0;
            AVI->video_pos++;
            if(n>max_frame_size)
            {
                lseek(AVI->fdes, n, SEEK_CUR);
                return -1;
            }
            
            if(avi_read(AVI->fdes, frame_buf, n) != n )
            {
                return 0;
            }
            return 1;
        }
        else if(strncasecmp(data,AVI->track[AVI->aptr].audio_tag, 4) == 0)
        {
            *frame_size = n;
            if(n > max_frame_size)
            {
                lseek(AVI->fdes, n, SEEK_CUR);
                return -2;
            }
            if(avi_read(AVI->fdes, frame_buf, n) != n ) 
            {
                return 0;
            }

            return 2;
        }
        else
        {
            if(lseek(AVI->fdes, n, SEEK_CUR) < 0)
            {
                return 0;
            }
        }
    }
}

/* AVI_print_error: Print most recent error (similar to perror) */

char *(avi_errors[]) =
{
    /*  0 */ "avilib - No Error",
    /*  1 */ "avilib - AVI file size limit reached (over 2GB)",
    /*  2 */ "avilib - Error opening AVI file",
    /*  3 */ "avilib - Error reading from AVI file",
    /*  4 */ "avilib - Error writing to AVI file",
    /*  5 */ "avilib - Error writing index (file may still be useable)",
    /*  6 */ "avilib - Error closing AVI file",
    /*  7 */ "avilib - Operation (read/write) not permitted",
    /*  8 */ "avilib - Out of memory (malloc failed)",
    /*  9 */ "avilib - Not an AVI file",
    /* 10 */ "avilib - AVI file has no header list (corrupted?)",
    /* 11 */ "avilib - AVI file has no MOVI list (corrupted?)",
    /* 12 */ "avilib - AVI file has no video data",
    /* 13 */ "avilib - operation needs an index",
    /* 14 */ "avilib - Unkown Error"
};

static int num_avi_errors = sizeof(avi_errors)/sizeof(char*);

static char error_string[4096] = {0};

/* 打印错误消息(str用于提供的错误前缀) */
void AVI_print_error(char *str)
{
    if (NULL == str)
    {
        AVILIB_ERR("NULL POINTER EXCEPTION!\n");
        return;
    }

    int aerrno = 0;

    aerrno = (AVI_errno>=0 && AVI_errno<num_avi_errors) ? AVI_errno : num_avi_errors-1;

    AVILIB_ERR("%s: %s\n", str, avi_errors[aerrno]);

    /* for the following errors, perror should report a more detailed reason: */

    if(AVI_errno == AVI_ERR_OPEN        ||
       AVI_errno == AVI_ERR_READ        ||
       AVI_errno == AVI_ERR_WRITE       ||
       AVI_errno == AVI_ERR_WRITE_INDEX ||
       AVI_errno == AVI_ERR_CLOSE )
    {
        perror("REASON");
    }
}

char *AVI_strerror(void)
{
    int aerrno = 0;

    aerrno = (AVI_errno>=0 && AVI_errno<num_avi_errors) ? AVI_errno : num_avi_errors-1;

    if(AVI_errno == AVI_ERR_OPEN        ||
       AVI_errno == AVI_ERR_READ        ||
       AVI_errno == AVI_ERR_WRITE       ||
       AVI_errno == AVI_ERR_WRITE_INDEX ||
       AVI_errno == AVI_ERR_CLOSE )
    {
        snprintf(error_string, sizeof(error_string), "%s - %s", avi_errors[aerrno], strerror(errno));
        return error_string;
    }
    else
    {
        return avi_errors[aerrno];
    }
}

uint64_t AVI_max_size()
{
    return ((uint64_t)AVI_MAX_LEN);
}
