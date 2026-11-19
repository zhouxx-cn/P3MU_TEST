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
#include "drv_light.h"

#ifdef __SUPPORT_FUNC_INSTRUMENT__
void __cyg_profile_func_enter(void *this_fn, void *call_site) {}
void __cyg_profile_func_exit(void *this_fn, void *call_site) {}
#endif

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
    SINT32 nCmd = 0;
    int fd = -1;
    int Ret = 0;
    struct light_attr LightAttr = { 0 };

    fd = open("/dev/light", O_RDWR);
    if (fd < 0)
    {
        printf("Open /dev/light failed!\n");
        return -1;
    }

    while (1)
    {
        printf("Press q to quit!: \n"
            "\t1-Config Light IR, Duty: 0, Freq: 5khz.\n"
            "\t2-Config Light IR, Duty: 50, Freq: 5khz.\n"
            "\t3-Config Light IR, Duty: 100, Freq: 5khz.\n"
            "\t4-Get CDS Stat.\n"
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
                LightAttr.value = 0;
                LightAttr.frq_khz = 5;
                Ret = ioctl(fd, LIGHT_CMD_SET_IR, &LightAttr);
                if (fd < 0)
                {
                    printf("[ERR] ioctl hwconf cmd(%d) failed!\n", LIGHT_CMD_SET_IR);
                    goto out;
                }

                printf("[HWCONF] Set Light-IR Value(%d) Frq(%dkhz)\n", LightAttr.value, LightAttr.frq_khz);
                break;

            case 2:
                LightAttr.value = 50;
                LightAttr.frq_khz = 5;
                Ret = ioctl(fd, LIGHT_CMD_SET_IR, &LightAttr);
                if (fd < 0)
                {
                    printf("[ERR] ioctl hwconf cmd(%d) failed!\n", LIGHT_CMD_SET_IR);
                    goto out;
                }

                printf("[HWCONF] Set Light-IR Value(%d) Frq(%dkhz)\n", LightAttr.value, LightAttr.frq_khz);
                break;

            case 3:
                LightAttr.value = 100;
                LightAttr.frq_khz = 5;
                Ret = ioctl(fd, LIGHT_CMD_SET_IR, &LightAttr);
                if (fd < 0)
                {
                    printf("[ERR] ioctl hwconf cmd(%d) failed!\n", LIGHT_CMD_SET_IR);
                    goto out;
                }

                printf("[HWCONF] Set Light-IR Value(%d) Frq(%dkhz)\n", LightAttr.value, LightAttr.frq_khz);
                break;
            case 4:
                memset(&LightAttr, 0, sizeof(LightAttr));
                Ret = ioctl(fd, LIGHT_CMD_GET_CDS, &LightAttr);
                if (fd < 0)
                {
                    printf("[ERR] ioctl hwconf cmd(%d) failed!\n", LIGHT_CMD_SET_IR);
                    goto out;
                }

                printf("[HWCONF] Get CDS Value(%d).\n", LightAttr.value);
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
