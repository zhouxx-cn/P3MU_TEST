#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>

#define NEMELEN		256
#define BLOCK_SIZE 1024*1024
#define TEST_FILE  "test_file.mp4"
#define HARD_DISK    0
#define USB_DISK     1

#define TEST_CHECK_SUM

static int total_disknum;
static int work_mode;    //测试u盘还是硬盘
static int dev_filled[16][60];
static int file_num[16];

static char* disk_name[] = {"./"}; 
static char* usb_name[] = {"./"};
static int write_error[16][60];

double tval (struct timeval _tstart,struct timeval _tend)
{
    double t1, t2;

    t1 = (double) _tstart.tv_sec + (double) _tstart.tv_usec / (1000 * 1000);
    t2 = (double) _tend.tv_sec + (double) _tend.tv_usec / (1000 * 1000);
    return (t2 - t1);
}


void usage()
{
	printf("Usage: iotest [OPTION] [FILESIZE]\n");
	printf("Test IO Write/Read Speed(MB/s)\n");

	printf("\n");
	printf("	-h	HARD_DISK_NUM\n");
	printf("	-u	USB_DEVICE_NUM\n");	
	printf("	Top hard disk number is 8, default number: 1\n");
	printf("	Top usb device number is 2, default number: 1\n");	
	printf("\n");
	printf("Make sure that mounted usb device or hard disk, and leaving enough space for testing.\n");
	printf("Mounting usb device likes as: mount /dev/sda1 /mnt/usb1\n");
}

void mmap4Mfile(char *d_Buf,char *filename)
{
    int file_fp;
    int n_op_bytes = 0;	
    int n_bytes = 0;

    file_fp = open(filename,O_RDWR,0640);
    if (file_fp < 0)
    {
        printf("can't open file %s\n", filename);
        exit(-1);
    }	
    while (n_bytes < BLOCK_SIZE)
    {
        n_op_bytes = read (file_fp, d_Buf + n_bytes, BLOCK_SIZE - n_bytes);
        n_bytes += n_op_bytes;
        printf("n_op_bytes = %d\n",n_op_bytes);
    }		
    close(file_fp);
}

void usb_wtest(int cur_disk, char *d_Buf, int buflen)
{
    int i;
    int fd;
    char filepath[NEMELEN];
    char filename[NEMELEN];
    int n_op_bytes = 0;
    int n_bytes = 0;
    int left_len;
    int m = file_num[cur_disk];
    struct timeval _tstart;
    struct timeval _tend;

    printf("\n-------------------------------------------------------\n");
    printf("transfer buffer size unit: %.1f kb\n", (float)buflen / 1024);	  
    if (work_mode == HARD_DISK)
        sprintf(filepath,disk_name[cur_disk]);
    else if(work_mode == USB_DISK)
        sprintf(filepath,usb_name[cur_disk]);
    sprintf(filename,"MP[%d].h264" ,file_num[cur_disk]);
    strcat(filepath, filename);	  
    fd = open(filepath, O_WRONLY | O_CREAT | O_TRUNC);
    if (fd < 0)
    {
        printf("can't open file %s：", filepath);
        if(errno == EINTR)
        {
            printf("disk %d error(Interrupted system call)\n",cur_disk + 1);
            write_error[cur_disk][m] = 1;
            return;
        }
        else if (errno == ENOSPC)
        {
            printf("No space left on %s %d\n",(HARD_DISK == work_mode) ? "hard disk" : "usb device", cur_disk + 1);
            dev_filled[cur_disk][m] = 1;
            write_error[cur_disk][m] = 1;
            return;	
        }
        else 
        {
            printf("disk %d  error, errno = %d\n",cur_disk + 1,errno);
            write_error[cur_disk][m]= 1;
            return;	
        }	
    }	
    gettimeofday (&_tstart, NULL);
    for (i = 0; i < 10; i++)
    { 	
        left_len = buflen;
        n_bytes = 0;
        while (n_bytes < BLOCK_SIZE)
        {		    	
            n_op_bytes = write (fd, d_Buf+n_bytes, left_len);	
            if (n_op_bytes < 0)
            {      
                if(errno == EINTR)
                {
                    printf("disk %d write error(Interrupted system call)\n",cur_disk + 1);
                    write_error[cur_disk][m] = 1;
                    close(fd);
                    return;
                }
                else if (errno == ENOSPC)
                {
                    printf("No space left on %s %d\n",(HARD_DISK == work_mode) ? "hard disk" : "usb device", cur_disk + 1);
                    dev_filled[cur_disk][m] = 1;
                    write_error[cur_disk][m] = 1;
                    close(fd);
                    return;	
                }
                else 
                {
                    printf("disk %d write error, errno = %d\n",cur_disk + 1,errno);
                    write_error[cur_disk][m] = 1;
                    close(fd);
                    return;	
                }				                    			          			          
            }
            else if(n_op_bytes == 0)
            {
                printf("disk %d write function return 0\n",cur_disk + 1);
                write_error[cur_disk][m] = 1;
                close(fd);
                return;
            }
            n_bytes += n_op_bytes;			      
            if ( (n_bytes + buflen) > BLOCK_SIZE)
                left_len = BLOCK_SIZE - n_bytes;
            else
                left_len = buflen;
        }
    }  
    fsync(fd);
    gettimeofday (&_tend, NULL);
    close(fd);
    n_bytes = 10;
    printf("(%03d) write to %s %d %d MB in %.2f seconds = %.2f MB/sec\n", file_num[cur_disk],(HARD_DISK == work_mode) ? "hard disk" : "usb device" ,
        cur_disk + 1, n_bytes, tval (_tstart, _tend), n_bytes / tval ( _tstart,_tend));
}

void usb_rtest(int cur_disk, char *d_Buf, char* r_buf, int buflen)
{
    char filepath[NEMELEN];
    char filename[NEMELEN];
    int fd;
    int n_op_bytes = 0;
    int n_bytes = 0;	
    struct timeval _tstart;
    struct timeval _tend;

    printf("\n-------------------------------------------------------\n");

    if (NULL == r_buf || NULL == d_Buf)
    {
        printf("[usb_rtest] Ptr is NULL\n");
        exit(-1);
    }

    if (work_mode == HARD_DISK)
        sprintf(filepath,disk_name[cur_disk]);
    else if(work_mode == USB_DISK)
        sprintf(filepath,usb_name[cur_disk]);
    
    sprintf(filename,"MP[%d].h264" ,file_num[cur_disk]);
    strcat(filepath, filename);
    printf("filepath %s \n", filepath);
    fd = open(filepath, O_RDONLY);
    if (fd < 0)
    {
        printf("%s: can't open file %s\n", __FUNCTION__, filepath);
        exit(-1);
    }

    gettimeofday (&_tstart, NULL);
    while (n_bytes < BLOCK_SIZE*10)
    {
        n_op_bytes = read (fd, r_buf + n_bytes, buflen);
        if (n_op_bytes < 0)
        {      
            if(errno == EINTR)
            {
                printf("disk %d read error(Interrupted system call)\n",cur_disk + 1);
                close(fd);
                return;
            }
            else 
            {
                printf("disk %d read error, errno = %d\n",cur_disk + 1,errno);
                close(fd);
                return;	
            }	          			          			          
        }
        else if(n_op_bytes == 0)
        {
            printf("disk %d read function return 0\n",cur_disk + 1);	
            close(fd);
            return;
        }
        n_bytes += n_op_bytes;	
    }
    close(fd);
    fsync(fd);
    gettimeofday (&_tend, NULL);

    n_bytes /= 1024*1024;
    printf("(%03d) read from %s %d %d Mb in %.2f seconds = %.2f MB/sec\n", file_num[cur_disk], (HARD_DISK == work_mode) ? "hard disk" : "usb device" ,
        cur_disk + 1, n_bytes, tval (_tstart, _tend), n_bytes / tval (_tstart, _tend));	 		

#ifdef TEST_CHECK_SUM
    int i;
    for (i = 0; i < 10; i++)
    {
        n_bytes = 0;
        while(n_bytes < BLOCK_SIZE)
        {
            if(d_Buf[n_bytes] != r_buf[n_bytes + BLOCK_SIZE*i])
            {
                printf("%s %d read/write error:\n",(HARD_DISK == work_mode) ? "hard disk" : "usb device", cur_disk + 1);
                printf("error pos: %d, error data %d, correct data %d\n",n_bytes + BLOCK_SIZE*i,r_buf[n_bytes + BLOCK_SIZE*i],d_Buf[n_bytes]);
                exit(-1);
            }	
            n_bytes ++;
        }
    }
#endif 

	return;
}

int main(int argc, char *argv[])
{
    int i,j;
    int buflen ;
    char *d_Buf = NULL;
    char *r_Buf = NULL;
    char filepath[NEMELEN];
    char RMH264 [NEMELEN];
    char loopnum;
    
    buflen = 64*1024;//每次写到SD卡的单位长度

    /* default total size */
    total_disknum		= 1;
    memset(write_error, 0x00, 60*16*sizeof(int));
    memset(dev_filled, 0x00, sizeof(int)*60*16);
    loopnum = 0;
	
    for(i = 0; i < 16; i++)
    {
        file_num[i] = 0;
    }

	/* check totalsize or show help information */
	
    if (argc > 1)
    {
        if ((0 == strcmp(argv[1], "-u"))&&(3 == argc))
        {
            work_mode = USB_DISK;
            total_disknum = atoi(argv[2]);
        }
        else if((0 == strcmp(argv[1], "-h"))&&(3 == argc))
        {
            work_mode = HARD_DISK;
            total_disknum = atoi(argv[2]);			
        }
        else
        {
            usage();
            exit(0);
        }
    }

    d_Buf = (char *)malloc(BLOCK_SIZE + buflen);
    r_Buf =  (char*)malloc(BLOCK_SIZE * 10 + buflen);
    if (d_Buf == NULL || r_Buf == NULL)
    {
        printf("d_Buf or r_Buf malloc failed \n"); 
    }

    mmap4Mfile(d_Buf, TEST_FILE);

	/* adjust buffer length for testing the speed */
	
    for(j = 0; j < total_disknum; j++)
    {
        if (work_mode == HARD_DISK)
            sprintf(RMH264,"rm -rf %s*.h264",disk_name[j]);
        else if(work_mode == USB_DISK)
            sprintf(RMH264,"rm -rf %s*.h264",usb_name[j]);	      	
        printf("%s\n",RMH264);
        system(RMH264);
        system("sync");	
    }
	
    i = 0;
    while ( 1 )
    {
        i++;
        printf("\n\n\n");
        printf("%d----------------------------%d--------------------------%d\n",i, i, i); 
        loopnum = 0;
        while (loopnum < 20)
        {
            loopnum ++;
            for (j = 0; j < total_disknum; j++)
            {
                usb_wtest(j,d_Buf,buflen);
                file_num[j]++;
            }	   
        }
        loopnum = 0;
        for (j = 0; j < total_disknum; j++)
        {
            file_num[j] = 0;;
        }
        while (loopnum < 20)
        {
            int m; 
            loopnum ++;
            for (j = 0; j < total_disknum; j++)
            {
                m = file_num[j] ;
                if ((!dev_filled[j][m]) && (!write_error[j][m]))
                {
                    usb_rtest(j, d_Buf, r_Buf, buflen);
                }
                write_error[j][m] = 0;
                file_num[j]++;
            }	   
        }

        printf("cleaning......\n");
        for(j = 0; j < total_disknum; j++)
        {
            if (work_mode == HARD_DISK)
                sprintf(RMH264,"rm -rf %s*.h264",disk_name[j]);
            else if(work_mode == USB_DISK)
                sprintf(RMH264,"rm -rf %s*.h264",usb_name[j]);	      	
            printf("%s\n",RMH264);
            system(RMH264);
            system("sync");	
            file_num[j] = 0;
        }
        memset(dev_filled, 0x00, sizeof(int)*60*16);
    }
    free(d_Buf);
    free(r_Buf);
    return 0;
}

