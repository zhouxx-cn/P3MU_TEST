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
#include "drv_expwm.h"

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
    struct expwm_config ExPwmCfg = { 0 };

    fd = open("/dev/expwm", O_RDWR);
    if (fd < 0)
    {
        printf("Open /dev/expwm failed!\n");
        return -1;
    }

    while (1)
    {
        printf("Press q to quit!: \n"
            "\t1-Enable PWM 4.\n"
            "\t2-Disable PWM 4.\n"
            "\t3-Config PWM 4, duty(665312), period(1365312).\n"
            "\t4-Config PWM 4, duty(365312), period(1365312).\n"
            "\t5-Get Config PWM 4.\n"
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
                ExPwmCfg.pwm_id = 4;
                Ret = ioctl(fd, EXPWM_CMD_ENABLE, &ExPwmCfg);
                if (fd < 0)
                {
                    printf("[ERR] ioctl hwconf cmd(%d) failed!\n", EXPWM_CMD_ENABLE);
                    return -1;
                }

                printf("[HWCONF] Enable PWM(%d).\n", ExPwmCfg.pwm_id);
                break;

            case 2:
                ExPwmCfg.pwm_id = 4;
                Ret = ioctl(fd, EXPWM_CMD_DISABLE, &ExPwmCfg);
                if (fd < 0)
                {
                    printf("[ERR] ioctl hwconf cmd(%d) failed!\n", EXPWM_CMD_DISABLE);
                    return -1;
                }

                printf("[HWCONF] Disable PWM(%d).\n", ExPwmCfg.pwm_id);
                break;

            case 3:
                ExPwmCfg.pwm_id = 4;
                ExPwmCfg.duty_ns = 665312;
                ExPwmCfg.period_ns = 1365312;
                Ret = ioctl(fd, EXPWM_CMD_SET_CONFIG, &ExPwmCfg);
                if (fd < 0)
                {
                    printf("[ERR] ioctl hwconf cmd(%d) failed!\n", EXPWM_CMD_SET_CONFIG);
                    return -1;
                }

                printf("[HWCONF] Set PWM(%d) Duty(%d) Period(%d).\n", 
                    ExPwmCfg.pwm_id, ExPwmCfg.duty_ns, ExPwmCfg.period_ns);
                break;

            case 4:
                ExPwmCfg.pwm_id = 4;
                ExPwmCfg.duty_ns = 365312;
                ExPwmCfg.period_ns = 1365312;
                Ret = ioctl(fd, EXPWM_CMD_SET_CONFIG, &ExPwmCfg);
                if (fd < 0)
                {
                    printf("[ERR] ioctl hwconf cmd(%d) failed!\n", EXPWM_CMD_SET_CONFIG);
                    return -1;
                }

                printf("[HWCONF] Set PWM(%d) Duty(%d) Period(%d).\n",
                    ExPwmCfg.pwm_id, ExPwmCfg.duty_ns, ExPwmCfg.period_ns);
                break;
            
            case 5:
                ExPwmCfg.pwm_id = 4;
                Ret = ioctl(fd, EXPWM_CMD_GET_CONFIG, &ExPwmCfg);
                if (fd < 0)
                {
                    printf("[ERR] ioctl hwconf cmd(%d) failed!\n", EXPWM_CMD_GET_CONFIG);
                    return -1;
                }

                printf("[HWCONF] Get PWM(%d) Duty(%d) Period(%d).\n",
                    ExPwmCfg.pwm_id, ExPwmCfg.duty_ns, ExPwmCfg.period_ns);
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
