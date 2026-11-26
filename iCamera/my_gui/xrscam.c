#include <semaphore.h>
#include <sys/signal.h>

#include "common_pub.h"
#include "global_macro.h"
#include "zwsdk.h"
#include "RShellCmd.h"
#include "Authority.h"
#include "mysystem.h"
#include "thumbcam_demo.h"

extern "C"
{
#include "xtrace.h"
}

/* MAIN模块日志接口 */
#define MAIN_LOG_E(format...) LOG_E(LOG_MODULE_MAIN, format)
#define MAIN_LOG_W(format...) LOG_W(LOG_MODULE_MAIN, format)
#define MAIN_LOG_I(format...) LOG_I(LOG_MODULE_MAIN, format)
#define MAIN_LOG_D(format...) LOG_D(LOG_MODULE_MAIN, format)
#define MAIN_LOG_H(desc, data, len) LOG_H(LOG_MODULE_MAIN, desc, data, len)

#define CMD_DROP_CACHE_2 "echo 2 > /proc/sys/vm/drop_caches"

static sem_t m_semStartRebootOpsThread;

static void *jffs2_check(void *param)
{
    PRT_TID();
    pthread_detach(pthread_self());

    char command[64] = {0};
    char buf[256] = {0};
    int nRebootFlag = 0;
    int nCheckFlag = 1;
    FILE *fp;
    char *point;

    while (1)
    {
        if (nCheckFlag)
        {
            sleep(25);
            nCheckFlag = 0;
            continue;
        }

        //获得dmesg信息
        memset(command, 0, sizeof(command));
        snprintf(command, sizeof(command), "dmesg > /tmp/.dmesg");
        mysystem(command);

        fp = fopen("/tmp/.dmesg", "r");
        if (fp == NULL)
        {
            MAIN_LOG_I("dmesg---fopen %s faile!\n", "/tmp/.dmesg");
            sleep(10);
            continue;
        }

        memset(buf, 0, sizeof(buf));
        while (NULL != fgets(buf, 255, fp))
        {
            point = strstr(buf, "check_node_data");
            if (NULL != point)
            {
                nRebootFlag = 1;
                break;
            }
            else
            {
                nRebootFlag = 0;
            }
            memset(buf, 0, sizeof(buf));
        }
        fclose(fp);

        if (0 != access("/mnt/mtd/config/.jffs2Err:2", F_OK))
        {
            if (1 == nRebootFlag)
            {
                if (0 == access("/mnt/mtd/config/.jffs2Err:1", F_OK))
                {
                    MAIN_LOG_I("\n Second dmesg info : jffs2 notice! so touch /mnt/mtd/config/.jffs2Err:2\n");
                    mysystem("rm /mnt/mtd/config/.jffs2Err:1");
                    mysystem("touch /mnt/mtd/config/.jffs2Err:2");
                    sleep(2);
                    mysystem("reboot");
                }
                else
                {
                    MAIN_LOG_I("\n First dmesg info : jffs2 notice! so touch /mnt/mtd/config/.jffs2Err:1\n");
                    mysystem("touch /mnt/mtd/config/.jffs2Err:1");
                    sleep(2);
                    mysystem("reboot");
                }
            }
            else if (0 == access("/mnt/mtd/config/.jffs2Err:1", F_OK) && 0 == nRebootFlag) //第二次正常了，就把标记删掉
            {
                MAIN_LOG_I("\n Second dmesg info is normal, jffs2 is normal \n");
                mysystem("rm /mnt/mtd/config/.jffs2Err:1");
            }
        }
        else if (0 == access("/mnt/mtd/config/.jffs2Err:2", F_OK) && 0 == nRebootFlag) //第三次正常了，就把标记删掉
        {
            MAIN_LOG_I("\n Third dmesg info is normal, jffs2 is normal \n");
            mysystem("rm /mnt/mtd/config/.jffs2Err:2");
        }

        sleep(10);
    }
}

//设备启动后线程监测
int SystemStartThreadManager()
{
    pthread_t threadId_jffs2Err;

    if (pthread_create(&threadId_jffs2Err, NULL, jffs2_check, NULL) != 0)
    {
        MAIN_LOG_I("########### create jffs2_check pthread fail ###########\n");
    }

    return 0;
}

int SetSysStartTag(int Stat)
{
    int nRet = 0;
    nRet = ZWSys_SetSysStartTag(Stat);

    return nRet;
}

static void sigstop(int signo)
{
    int n = 0;
    char buf[128];

    if (signo == SIGSEGV)
    {
        n = snprintf(buf, sizeof(buf), "[Error] Segmentation fault detected! Signal:%d\n", signo);
    }
    else
    {
        n = snprintf(buf, sizeof(buf), "[Error] Signal %d caught, aborting.\n", signo);
    }
    (void)write(STDERR_FILENO, buf, n); // write() 是异步信号安全的

    _exit(1);
}

static void init_signal_handler()
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));

    // SIGPIPE: 忽略
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGPIPE, &sa, NULL);

    // SIGALRM: 保留SA_RESTART,避免超时机制产生的SIGALRM中断系统调用
    sa.sa_handler = SIG_DFL;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGALRM, &sa, NULL);

    // SIGINT/SIGQUIT/SIGTERM: 优雅退出，统一用sigstop
    sa.sa_handler = sigstop;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGTERM);
    sigaddset(&sa.sa_mask, SIGQUIT);
    sigaction(SIGINT, &sa, NULL);

    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGINT);
    sigaddset(&sa.sa_mask, SIGQUIT);
    sigaction(SIGTERM, &sa, NULL);

    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGINT);
    sigaddset(&sa.sa_mask, SIGTERM);
    sigaction(SIGQUIT, &sa, NULL);

    // SIGSEGV: 单独处理，极简且安全
    sa.sa_handler = sigstop;
    sa.sa_flags = SA_RESETHAND;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGSEGV, &sa, NULL);
}

int main(void)
{
    init_signal_handler();

    sem_init(&m_semStartRebootOpsThread, 0, 0);

    /* 初始化日志管理 */
    init_log_level();

    /* 初始化授权管理 */
    InitLicenseAuthorize();

    //默认不启用，需要用时，0改成1即可
    XTrace_Init(0, 10 * 1000, 1000);

    /* 初始化配置管理器 */
    // MPU_PAR_CustomFileDate_Replace(CUSTOM_CONFIGMODE_FILE, 0);
    // MPU_PAR_Init();

    if (0 != ZWSys_Init(0))
    {
        MAIN_LOG_E("ZWSys_Init fail!, need reboot !\n");
    }
    printf("[zxx_for_test] 22222222\n");

    SystemStartThreadManager();

    RShellCmdInit();
    /* 设置设备正常启动标志 */
    SetSysStartTag(0);

    SetDevInitOverFlag(1);

    ZWSys_LCDSetBackLightLevel(100);

    /* 启动拇指相机 UI demo:
     * 从循环缓冲区取 240x320 子码流(H264) -> openh264 软解码(YUV)
     * -> 对接 ui 工程师的 UI 框架(ui_video_refresh) -> 合成整帧 -> LCD 送显。
     * 非阻塞, 内部自建 UI 线程与取流线程。 */
    ThumbCamDemo_Start();

    while (1)
    {
        sleep(60);
    }

    return 0;
}
