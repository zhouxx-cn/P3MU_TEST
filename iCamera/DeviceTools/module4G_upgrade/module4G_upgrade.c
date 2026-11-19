#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/wait.h>

#define LOG(format, ...) printf("[%s:%d]" format "\r\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define FOTA_ATCMD          "AT+QFOTADL=\"FILE:%d\"\r\n" // FILE SIZE
#define USB_PORT0           "/dev/ttyUSB0"
#define USB_PORT1           "/dev/ttyUSB0"
#define USB_PORT2           "/dev/ttyUSB2"
#define FOTA_STR_OK         "OK"
#define FOTA_STR_FOTA       "\"FOTA\""
#define FOTA_STR_START      "\"FILESTART\""
#define FOTA_STR_DOWNLOAD   "\"DOWNLOADING\""
#define FOTA_STR_FILEEND    "\"FILEEND\""
#define FOTA_STR_UPDATING   "\"UPDATING\""
#define FOTA_STR_END        "\"END\""
#define FOTA_STR_ERROR      "\"ERROR\""

typedef enum
{
    STATUS_NORMAL_START,
    STATUS_NORMAL_END,
    STATUS_FOTA_START,
    STATUS_FOTA_DOWNLOAD,
    STATUS_FOTA_FILEEND,
    STATUS_FOTA_UPDATING,
    STATUS_FOTA_END,
    STATUS_FOTA_ERROR,
} FOTA_STATUS;

int setport(int fd, int baud, int databits, int stopbits, int parity)
{
    int baudrate;
    struct termios newtio;
    switch (baud)
    {
    case 300:
        baudrate = B300;
        break;
    case 600:
        baudrate = B600;
        break;
    case 1200:
        baudrate = B1200;
        break;
    case 2400:
        baudrate = B2400;
        break;
    case 4800:
        baudrate = B4800;
        break;
    case 9600:
        baudrate = B9600;
        break;
    case 115200:
        baudrate = B115200;
    case 19200:
        baudrate = B19200;
        break;
    case 38400:
        baudrate = B38400;
        break;
    default:
        baudrate = B9600;
        break;
    }
    tcgetattr(fd, &newtio);
    bzero(&newtio, sizeof(newtio));
    // setting   c_cflag
    newtio.c_cflag &= ~CSIZE;
    switch (databits)
    {
    case 7:
        newtio.c_cflag |= CS7;
        break;
    case 8:
        newtio.c_cflag |= CS8;
        break;
    default:
        newtio.c_cflag |= CS8;
        break;
    }
    switch (parity)
    {
    case 'n':
    case 'N':
        newtio.c_cflag &= ~PARENB; /* Clear parity enable */
        newtio.c_iflag &= ~INPCK;  /* Enable parity checking */
        break;
    case 'o':
    case 'O':
        newtio.c_cflag |= (PARODD | PARENB);
        newtio.c_iflag |= INPCK; /* Disnable parity checking */
        break;
    case 'e':
    case 'E':
        newtio.c_cflag |= PARENB; /* Enable parity */
        newtio.c_cflag &= ~PARODD;
        newtio.c_iflag |= INPCK; /* Disnable parity checking */
        break;
    case 'S':
    case 's': /*as no parity*/
        newtio.c_cflag &= ~PARENB;
        newtio.c_cflag &= ~CSTOPB;
        break;
    default:
        newtio.c_cflag &= ~PARENB; /* Clear parity enable */
        newtio.c_iflag &= ~INPCK;  /* Enable parity checking */
        break;
    }
    switch (stopbits) 
    {
    case 1:
        newtio.c_cflag &= ~CSTOPB; // 1
        break;
    case 2:
        newtio.c_cflag |= CSTOPB; // 2
        break;
    default:
        newtio.c_cflag &= ~CSTOPB;
        break;
    }
    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN] = 0;
    newtio.c_cflag |= (CLOCAL | CREAD);
    // newtio.c_oflag|=OPOST;
    newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Input, RawData mode
    newtio.c_oflag &= ~OPOST;                          // Output
    newtio.c_iflag &= ~(IXON | IXOFF | IXANY);
    cfsetispeed(&newtio, baudrate);
    cfsetospeed(&newtio, baudrate);
    tcflush(fd, TCIFLUSH);
    if (tcsetattr(fd, TCSANOW, &newtio) != 0)
    {
        perror("SetupSerial 3");
        return -1;
    }
    return 0;
}

int readport(int fd, char *buf, size_t bufSize, int waitMs)
{
    int rc;
    struct timeval tv;
    fd_set readfd;
    tv.tv_sec = waitMs / 1000;         // SECOND
    tv.tv_usec = waitMs % 1000 * 1000; // USECOND
    FD_ZERO(&readfd);
    FD_SET(fd, &readfd);
    // fcntl(fd,F_SETFL,0); //block mode

    rc = select(fd + 1, &readfd, NULL, NULL, &tv);
    if (rc > 0)
    {
        rc = read(fd, buf, bufSize);
        printf("%s", buf);
        return rc;
    }
    else
    {
        return -1;
    }
}

int writeport(int fd, char *buf, size_t bufSize)
{
    int rc;
    fd_set writefd;
    FD_ZERO(&writefd);
    FD_SET(fd, &writefd);
    // fcntl(fd,F_SETFL,0); //block mode
    rc = select(fd + 1, NULL, &writefd, NULL, NULL);
    if (rc > 0)
    {
        rc = write(fd, buf, bufSize);
        return rc;
    }
    else
    {
        return -1;
    }
    return 0;
}

int openport(char *device)
{
    if (access(device, F_OK) != 0)
    {
        // LOG("wait for port %s", device);
        return -1;
    }

    int fd = open(device, O_RDWR | O_SYNC | O_NOCTTY | O_NDELAY);
    if (fd > 0)
    {
        // Set serial port
        if (setport(fd, 9600, 8, 1, 'n') < 0)
        {
            // LOG("Can't Set Serial Port!");
            return -1;
        }
    }
    else
    {
        // LOG("Can't Open port");
        return -1;
    }

    return fd;
}

size_t getFileSize(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        return -1;
    }

    fseek(file, 0L, SEEK_END);
    size_t size = ftell(file);
    fclose(file);

    return size;
}

int waitForPortOpen(char *port, int wiatSecond)
{
    int fd = 0;
    int n = wiatSecond;
    while (n--)
    {
        fd = openport(port);
        if (fd > 0)
        {
            return fd;
        }
        sleep(1);
    }
    return -1;
}

FOTA_STATUS getStatus(int fd, char *buffer, size_t bufSize)
{
    memset(buffer, 0, bufSize);
    readport(fd, buffer, bufSize, 3000);

    if(strstr(buffer, FOTA_STR_OK))
    {
        return STATUS_NORMAL_END;
    }
    if(strstr(buffer, FOTA_STR_END))
    {
        return STATUS_FOTA_END;
    }
    if(strstr(buffer, FOTA_STR_UPDATING))
    {
        return STATUS_FOTA_UPDATING;
    }
    if(strstr(buffer, FOTA_STR_FILEEND))
    {
        return STATUS_FOTA_FILEEND;
    }
    if(strstr(buffer, FOTA_STR_DOWNLOAD))
    {
        return STATUS_FOTA_DOWNLOAD;
    }
    if (strstr(buffer, FOTA_STR_START))
    {
        return STATUS_FOTA_START;
    }
    if(strstr(buffer, FOTA_STR_ERROR))
    {
        return STATUS_FOTA_ERROR;
    }
       
    return STATUS_NORMAL_START;

}

static int mysystem(const char* cmdstring)
{
	pid_t pid = -1;
	int status = -1;

	if (cmdstring == NULL)
	{
		return 1;
    }
    
	if ((pid=vfork()) < 0 )
	{
		printf("vfork failed.cmd:%s\n", cmdstring);
		status = -1;
	}
	else if (pid==0)
	{
		execl("/bin/sh", "sh", "-c", cmdstring, (char*)0);
		_exit(127);
	}
	else
	{
		int i = 0;
		while (0 == waitpid(pid, &status, WNOHANG))	//check if the the time is out
		{
			if (i++ < 5*60*100)
			{
				usleep(10000);	//Here, this sleep period cannot be too long, otherwise, mysystem cannot return immediately.
			}
			else
			{
				printf("\n[SYSTEMD]command %s executing time out!\n",cmdstring);
				kill(pid, SIGKILL);
				waitpid(pid, &status, WNOHANG);
				break;
			}
		}
	}

	return status;
}

int main(int argc, char const *argv[])
{
    const char *file        = argv[1];
    char        buffer[128] = {0};
    char       *pfileBuffer = NULL;
    FILE       *pFile       = NULL;
    char       *fileBuf     = NULL;
    int         fd          = 0;
    int         filesize    = 0;
    int         nwrite      = 0;
    int         totalwrite  = 0;
    FOTA_STATUS status      = STATUS_NORMAL_START;

    // 检查文件是否存在
    if (argc < 2 || access(file, F_OK) != 0)
    {
        perror("Usage: ./cis_fota_tool <file>");
        return -1;
    }

    // 获取文件大小
    filesize = getFileSize(file);
    if (filesize == -1)
    {
        perror("get file size failed");
        return -1;
    }

    while (1)
    {
        if (fd <= 0)
        {
            fd = waitForPortOpen(USB_PORT0, 10);
            if (fd <= 0)
            {
                perror("open port failed");
            }
        }

        status = getStatus(fd, buffer, sizeof(buffer));
        LOG("get status = %d", status);
        switch (status)
        {
            case STATUS_NORMAL_START:
            {
                // 发送升级AT指令
                snprintf(buffer, sizeof(buffer), FOTA_ATCMD, filesize);
                write(fd, buffer, strlen(buffer));
                LOG("send at cmd: %s", buffer);
                break;
            }
            case STATUS_NORMAL_END:
            {
                // 关闭串口, 等待模组重启
                close(fd);
                fd = 0;
                sleep(2);
                break;
            }
            case STATUS_FOTA_START:
            {
                pFile = fopen(file, "rb");
                if (pFile == NULL)
                {
                    perror("open file failed");
                    goto fota_error;
                }
                pfileBuffer = (char *)malloc(filesize);
                if (pfileBuffer == NULL)
                {
                    perror("malloc failed");
                    goto fota_error;
                }
                memset(pfileBuffer, 0, filesize);
                fread(pfileBuffer, sizeof(char), filesize, pFile);
                for (totalwrite = 0; totalwrite < filesize;)
                {
                    nwrite = writeport(fd, pfileBuffer + totalwrite, filesize - totalwrite);
                    if (nwrite > 0)
                    {
                        totalwrite += nwrite;
                        LOG("nwrite=%d, totalwrite=%d\r\n", nwrite, totalwrite);
                    }
                }
                free(pfileBuffer);
                break;
            }
            case STATUS_FOTA_FILEEND:
            case STATUS_FOTA_DOWNLOAD:
            case STATUS_FOTA_UPDATING:
            {
                //do nothing 等待升级结束
                break;
            }
            case STATUS_FOTA_END:
            {
                LOG("fota success!");
                fclose(pFile);
                close(fd);
                mysystem("touch /mnt/mtd/4gUpgSucc");
                return 0;
            }
                
            case STATUS_FOTA_ERROR:
            {
                LOG("fota error!");
                goto fota_error;
            }
        }
        
    }

    sleep(1); //防止过快结束进程

fota_error:
    if (fd > 0)
    {
        close(fd);
    }
    if (pFile != NULL)
    {
        fclose(pFile);
    }
    return -1;
}
