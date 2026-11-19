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
#include "xrsdef.h"
#include "xrsstd.h"
#include "drv_hwconf.h"

static void _Test_Get_Cmd(char* pCmdStr)
{
    SINT32 i = 0;

    while ('\n' != (pCmdStr[i] = getchar()))
    {
        i++;
    }
}

int main(int argc, char** argv)
{
    char CmdStr[8] = { 0 };
    SINT32 RtVal = 0, nCmd = 0;
    int fd = -1;
    int Ret = 0;
    struct hwconf_ircut_conf IRcutCfg = { 0 };

    fd = open("/dev/hwconf", O_RDWR);
    if (fd < 0)
    {
        printf("Open /dev/hwconf failed!\n");
        return -1;
    }

    while (1)
    {
        printf("Press q to quit!: \n"
            "\t1-Get HwConf IRCut.\n"
            "\t2-Set HwConf IRCut(A: 110, B:220).\n"
        );

        _Test_Get_Cmd(CmdStr);

        if ((CmdStr[0] == 'q') || (CmdStr[0] == 'Q'))
        {
            TRACE_DBG("get cmd to exit \n");
            break;
        }
        nCmd = atoi(CmdStr);
        TRACE_DBG("nCmd(%d)\n", nCmd);

        switch (nCmd)
        {
            case 1:
                Ret = ioctl(fd, HWCONF_CMD_GET_IRCUT_CONF, &IRcutCfg);
                if (fd < 0)
                {
                    printf("[ERR] ioctl hwconf cmd(%d) failed!\n", HWCONF_CMD_GET_IRCUT_CONF);
                    return -1;
                }

                printf("[HWCONF] Get IRCut A = %d, B = %d.\n", IRcutCfg.gpio_a, IRcutCfg.gpio_b);

                break;

            case 2:
                IRcutCfg.gpio_a = 110;
                IRcutCfg.gpio_b = 220;

                Ret = ioctl(fd, HWCONF_CMD_SET_IRCUT_CONF, &IRcutCfg);
                if (fd < 0)
                {
                    printf("[ERR] ioctl hwconf cmd(%d) failed!\n", HWCONF_CMD_SET_IRCUT_CONF);
                    return -1;
                }

                printf("[HWCONF] Set IRCut A = %d, B = %d.\n", IRcutCfg.gpio_a, IRcutCfg.gpio_b);

                break;

            default:
                printf("unsupported nCmd, %d\n", nCmd);
                break;
        }
    }

out:
    if (fd >= 0)
    {
        close(fd);
    }
    return 0;
}
