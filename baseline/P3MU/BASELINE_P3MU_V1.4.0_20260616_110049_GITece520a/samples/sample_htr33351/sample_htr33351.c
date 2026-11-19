#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include "driver/drv_htr33351.h"

int main(int argc, char** argv)
{
    int fd = -1;
    int ret = 0;	
	unsigned int cmd = 0;
	unsigned int val = 0;
    HTR33351_DATA_S stData = { 0 };

    fd = open("/dev/htr33351", O_RDWR);
    if (fd < 0)
    {
        printf("Open /dev/htr33351 failed!\n");
        return -1;
    }

	printf("Usage:\n");
	printf("\t./xxx <get> <dir|val> <pin>\n");
	printf("\t./xxx <set> <dir|val> <pin> <0|1>\n");
	printf("\t./xxx <reset>\n");
	printf("\tpin: 0 ~ 15\n");

    if (!strcmp(argv[1], "get") || !strcmp(argv[1], "set"))
    {
        if (!strcmp(argv[1], "get"))
        {
			if (!strcmp(argv[2], "dir")) {
				cmd = HTR33351_CTL_GET_PIN_IO;
				stData.pin = (unsigned int)strtoul(argv[3], NULL, 10);
			} else {
				cmd = HTR33351_CTL_GET_PIN_VAL;
				stData.pin = (unsigned int)strtoul(argv[3], NULL, 10);
			}
			printf("Usr Cmd: %s %s %s\n", argv[1], argv[2], argv[3]);
        }
        else
        {
            if (!strcmp(argv[2], "dir")) {
				cmd = HTR33351_CTL_SET_PIN_IO;
				stData.pin = (unsigned int)strtoul(argv[3], NULL, 10);
				stData.memb.dir = (unsigned int)strtoul(argv[4], NULL, 10);
			} else {
				cmd = HTR33351_CTL_SET_PIN_VAL;
				stData.pin = (unsigned int)strtoul(argv[3], NULL, 10);
				stData.memb.val = (unsigned int)strtoul(argv[4], NULL, 10);
			}
			printf("Usr Cmd: %s %s %s %s\n", argv[1], argv[2], argv[3], argv[4]);
        }

        ret = ioctl(fd, cmd, &stData);
        if (fd < 0)
        {
            printf("ioctl /dev/htr33351 failed!\n");
            return -1;
        }

		if (!strcmp(argv[1], "get")) {
			if (!strcmp(argv[2], "dir")) {
				printf("dir result: %d\n", stData.memb.dir);
			} else {
				printf("val result: %d\n", stData.memb.val);
			}
		}
    }
	else if (!strcmp(argv[1], "reset"))
	{
		cmd = HTR33351_CTL_ALL_RESET;
		ret = ioctl(fd, cmd, &stData);
		if (fd < 0)
		{
			printf("ioctl /dev/htr33351 failed!\n");
			return -1;
		}
	}
    else
    {
        printf("Unknow cmd!\n");
    }

    close(fd);

    return 0;
}
