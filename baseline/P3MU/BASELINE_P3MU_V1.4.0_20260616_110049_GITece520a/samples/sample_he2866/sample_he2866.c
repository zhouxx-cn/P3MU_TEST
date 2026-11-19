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
#include "driver/drv_he2866.h"

int main(int argc, char** argv)
{
    int fd = -1;
    int ret = 0;
    HE2866_DATA_S stData = { 0 };

    fd = open("/dev/he2866", O_RDWR);
    if (fd < 0)
    {
        printf("Open /dev/he2866 failed!\n");
        return -1;
    }

    if (!strcmp(argv[1], "ircut") || !strcmp(argv[1], "step"))
    {
        unsigned int cmd = 0;
        stData.data = (unsigned char)strtoul(argv[2], NULL, 16);

        if (!strcmp(argv[1], "ircut"))
        {
            cmd = HE2866_CTL_IRCUT;
        }
        else
        {
            cmd = HE2866_CTL_STEP;
        }

        printf("%s: %d\n",argv[1], stData.data);

        ret = ioctl(fd, cmd, &stData);
        if (fd < 0)
        {
            printf("ioctl /dev/he2866 failed!\n");
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