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
#include <errno.h>

#define TARGET_BAUD         115200
#define USB_PORT            "/dev/ttyUSB3"
#define USB_PORT_1            "/dev/ttyUSB4"
#define WRITE_CHUNK_SIZE    1024  // 1KB一块，稳一点
#define FOTA_ATCMD          "AT+QFOTADL=\"FILE:%d\"\r\n"

int uart_set(int fd, int baud) {
    struct termios tio;
    tcgetattr(fd, &tio);
    cfmakeraw(&tio);
    tio.c_cflag &= ~CSIZE;
    tio.c_cflag |= CS8;
    tio.c_cflag &= ~PARENB;
    tio.c_cflag &= ~CSTOPB;
    tio.c_cc[VTIME] = 0;
    tio.c_cc[VMIN]  = 0;
    tio.c_cflag |= (CLOCAL | CREAD);
    tio.c_iflag &= ~(IXON | IXOFF | IXANY);
    cfsetispeed(&tio, B115200);
    cfsetospeed(&tio, B115200);
    tcflush(fd, TCIOFLUSH);
    tcsetattr(fd, TCSANOW, &tio);
    tcflush(fd, TCIOFLUSH);
    return 0;
}

int uart_write(int fd, const char *buf, int len) {
    int total = 0, ret;
    while(total < len) {
        ret = write(fd, buf + total, len - total);
        if(ret > 0) total += ret;
        else if(ret < 0 && (errno == EAGAIN || errno == EINTR)) continue;
        else if(ret < 0) { perror("write"); return -1; }
    }
    return total;
}

int main(int argc, char *argv[]) {
    if(argc < 2) { printf("Usage: %s <firmware>\n", argv[0]); return -1; }
    
    // 1. 打开文件
    FILE *fp = fopen(argv[1], "rb");
    if(!fp) { perror("open file"); return -1; }
    fseek(fp, 0, SEEK_END);
    int file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *file_buf = malloc(file_size);
    fread(file_buf, 1, file_size, fp);
    fclose(fp);
    printf("File loaded: %d bytes\n", file_size);

    // 2. 打开串口
    int fd = open(USB_PORT, O_RDWR | O_NOCTTY | O_SYNC);
    if(fd < 0) { perror("open uart"); free(file_buf); return -1; }
    uart_set(fd, TARGET_BAUD);
    printf("UART opened\n");

    // 3. 发FOTA AT命令
    char at_buf[64] = {0};
    snprintf(at_buf, sizeof(at_buf), FOTA_ATCMD, file_size);
    uart_write(fd, at_buf, strlen(at_buf));
    printf("Sent AT cmd: %s", at_buf);

    // 4. 【核心要求】直接等3秒，不做任何阻塞读取
    printf("Wait 3 seconds...\n");
    sleep(3);

    // 5. 直接开始发文件，不管模组有没有响应
    printf("Start sending file...\n");
    int offset = 0;
    while(offset < file_size) {
        int len = (file_size - offset) > WRITE_CHUNK_SIZE ? WRITE_CHUNK_SIZE : (file_size - offset);
        uart_write(fd, file_buf + offset, len);
        offset += len;
        // 打印进度
        if(offset % (10*WRITE_CHUNK_SIZE) == 0) {
            printf("Sent: %d/%d (%.1f%%)\n", offset, file_size, (float)offset/file_size*100);
        }
        usleep(1000); // 稍微喘口气，避免发太快
    }
    printf("File sent complete!\n");

    // 6. 最后等一会儿，看看模组有没有升级成功（可选）
    printf("Wait for modem update (180s timeout)...\n");
    char recv_buf[512] = {0};
    for(int i = 0; i < 180; i++) {
        int len = read(fd, recv_buf, sizeof(recv_buf)-1);
        if(len > 0) {
            recv_buf[len] = '\0';
            printf("Modem: %s", recv_buf);
            if(strstr(recv_buf, "\"END\"")) {
                printf("=====================================\n");
                printf("          FOTA SUCCESS!\n");
                printf("=====================================\n");
                break;
            }
            if(strstr(recv_buf, "\"ERROR\"")) {
                printf("FOTA ERROR!\n");
                break;
            }
        }
        sleep(1);
    }

    // 清理
    close(fd);
    free(file_buf);
	system("ls /dev/ttyUSB*");
	if(access(USB_PORT,F_OK)==0||access(USB_PORT_1,F_OK)==0)
	{
		printf("=============================\n");
		printf("ZX088HG FOTA SUCCESSS!\n");
		printf("=============================\n");
		system("touch /mnt/mtd/4gUpgSucc");
	}
    return 0;
}
