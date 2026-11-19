#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define PTS_DEV_PATH "/dev/pts/0"
#define CONSOLE_DEV_PATH "/dev/ttyS0"
/*\!
 *
 *          在telenet窗口执行 该 源码 编译出来的 执行 文件，
 *          redirect on后，linux脚本启动的进程的 printf 内容会 打印到当前 telnet窗口
 *
 * */

int main(int argc, char *argv[])
{
    int ttyout = -1;
	int ttyerr = -1;
    char *ttyout_name = NULL;
	char *ttyerr_name = NULL;

    if(argc < 2)
    {
        printf("miss argument\n");
        return 0;

    }

    /* 获取当前tty名称 */
    ttyout_name = ttyname(STDOUT_FILENO);
    printf("ttyout_name: %s\n", ttyout_name);
	ttyerr_name = ttyname(STDERR_FILENO);
    printf("ttyerr_name: %s\n", ttyerr_name);

    if(!strcmp(argv[1], "on"))
    {
        /* 重定向console到当前tty */
        ttyout = open(ttyout_name, O_RDONLY | O_WRONLY);
        ioctl(ttyout, TIOCCONS);
        perror("ioctl TIOCCONS");
		
		ttyerr = open(ttyerr_name, O_RDONLY | O_WRONLY);
        ioctl(ttyerr, TIOCCONS);
        perror("ioctl TIOCCONS");

    }
    else if(!strcmp(argv[1], "off"))
    {
        /* 恢复console */
        ttyout = open("/dev/console", O_RDONLY | O_WRONLY);
        ioctl(ttyout, TIOCCONS);
        perror("ioctl TIOCCONS");
		
		ttyerr = open("/dev/console", O_RDONLY | O_WRONLY);
        ioctl(ttyerr, TIOCCONS);
        perror("ioctl TIOCCONS");

    }
    else
    {
        printf("error argument\n");
        return 0;

    }
	
	close(ttyout);
    close(ttyerr);
    return 0;
}
