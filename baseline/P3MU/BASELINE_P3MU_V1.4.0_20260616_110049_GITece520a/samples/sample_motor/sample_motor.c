
#include <stdio.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "drv_motor.h"

#ifdef __SUPPORT_FUNC_INSTRUMENT__
void __cyg_profile_func_enter(void *this_fn, void *call_site) {}
void __cyg_profile_func_exit(void *this_fn, void *call_site) {}
#endif

#if defined(MOTO_DBG)
#define MOTO_LOG printf
#else
#define MOTO_LOG printf
#endif

#define DO_SELF_TEST_LOOP_CHAR "loop"
#define DO_SELF_TEST_CHAR "selftest"
#define DO_HCCW_CHAR "hccw"
#define DO_HCW_CHAR "hcw"
#define DO_VCCW_CHAR "vccw"
#define DO_VCW_CHAR "vcw"
#define DO_HCCWB_CHAR "hccwb"
#define DO_HCWB_CHAR "hcwb"
#define DO_VCCWB_CHAR "vccwb"
#define DO_VCWB_CHAR "vcwb"
#define DO_HCRUISING_CHAR "hcruising"
#define DO_VCRUISING_CHAR "vcruising"
#define DO_HVCRUISING_CHAR "hvcruising"
#define DO_STOP_CHAR "stop"
#define DO_GETPOS_CHAR "getpos"
#define DO_TOPOS_CHAR "topos"
#define DO_RIRQ_CHAR "regirq"
#define DO_FIRQ_CHAR "freeirq"

#define DO_LEFT_CHAR "left"
#define DO_RIGHT_CHAR "right"
#define DO_UP_CHAR "up"
#define DO_DOWN_CHAR "down"
#define DO_STOP_CHAR "stop"
#define DO_TOPOS_CHAR "topos"
#define DO_SETMAXPOS_CHAR "setmaxpos"

#define DO_SETIOADDR_CHAR "setioaddr"
#define DO_SET_SPEED_CHAR "setspeed"
#define DO_SET_SPEED2_CHAR "setspeed2"
#define DO_SET_SPEEDT_CHAR "setspeedt"

#define DO_TOSTARTPOS_CHAR "tostartpos"
#define DO_GETSTARTSTATUS_CHAR "getstartstate"

#define DO_GETSELFTESTSTATUS_CHAR "getselfteststate"
#define DO_MOTOSELFCHECKBACK_CHAR "checkback"
#define DO_TOPOS_PROD_CHAR "toposprod"
#define DO_SET_LOG_LEVEL "setloglevel"

#define MAX_CHAR_LEN (16)

typedef void (*do_func)(int argc, char *argv[]);

struct func_arr
{
	char *cmd;
	void *func;
};

void test_MotoLeft(int argc, char *argv[]);
void test_MotoRight(int argc, char *argv[]);
void test_MotoUp(int argc, char *argv[]);
void test_MotoDown(int argc, char *argv[]);
void test_MotoDown(int argc, char *argv[]);
void test_ToPos(int argc, char *argv[]);
void test_ToPosProd(int argc, char *argv[]);
void test_SetMaxPos(int argc, char *argv[]);
void test_GetPos(int argc, char *argv[]);
void test_setspeed(int argc, char *argv[]);
void test_setspeed2(int argc, char *argv[]);
void test_setspeedt(int argc, char *argv[]);
void test_set_loglevel(int argc, char *argv[]);
void test_ToStartPos(int argc, char *argv[]);
void test_GetStartState(int argc, char *argv[]);
void test_GetSelfTestState();
void test_MotoSelfCheckBack(int argc, char *argv[]);
void test_MotoSelfCheck();

void do_self_test();
void doMotoStop();
//void doSetMotoAddr();
void doMotoLeft(int nSteps);
void doMotoRight(int nSteps);
void doMotoUp(int nSteps);
void doMotoDown(int nSteps);
void doMotoToPos(int nHPos, int nVPos);
void doMotoToStartPos(int nHPos, int nVPos);
void doSetMotoMaxPos(int nHPos, int nVPos);
void doSetMotoMaxPos(int nHPos, int nVPos);
void doSetMotoSpeed(unsigned int nSpeed);
void doSetMotoSpeed2(unsigned int nSpeedh, unsigned int nSpeedv);
void doSetMotoSpeedType(unsigned int hSpeedt, unsigned int vSpeedt);
void do_self_test_loop();

struct func_arr func_tab[] = {
	{DO_LEFT_CHAR, test_MotoLeft},
	{DO_RIGHT_CHAR, test_MotoRight},
	{DO_UP_CHAR, test_MotoUp},
	{DO_DOWN_CHAR, test_MotoDown},
	/*{DO_SETIOADDR_CHAR, doSetMotoAddr},*/
	{DO_STOP_CHAR, doMotoStop},
	{DO_TOPOS_PROD_CHAR, test_ToPosProd},
	{DO_TOPOS_CHAR, test_ToPos},
	{DO_SETMAXPOS_CHAR, test_SetMaxPos},
	{DO_GETPOS_CHAR, test_GetPos},
	{DO_SELF_TEST_CHAR, do_self_test},
	{DO_SET_SPEED_CHAR, test_setspeed},
	{DO_SET_SPEED2_CHAR, test_setspeed2},
	{DO_SELF_TEST_LOOP_CHAR, do_self_test_loop},
	{DO_TOSTARTPOS_CHAR, test_ToStartPos},
	{DO_GETSTARTSTATUS_CHAR, test_GetStartState},
	{DO_GETSELFTESTSTATUS_CHAR, test_GetSelfTestState},
	{DO_MOTOSELFCHECKBACK_CHAR, test_MotoSelfCheckBack},
	{DO_SET_LOG_LEVEL, test_set_loglevel},
	{DO_SET_SPEEDT_CHAR, test_setspeedt},
	{NULL, NULL},
};

int main_dbg(int argc, char *argv[])
{
	return (0);
}

void do_self_test_loop()
{
	int ret = 0;

	int fd = -1;
	fd = open("/dev/" MOTOR_DEVICE_NAME, O_RDWR);
	if (fd < 0)
	{
		printf("Open " MOTOR_DEVICE_NAME " error!\n");
		return;
	}
	ret = ioctl(fd, MTDRV_SELF_TEST_LOOP);
	close(fd);
	return;
}

void do_self_test()
{
	int ret = 0;

	int fd = -1;
	fd = open("/dev/" MOTOR_DEVICE_NAME, O_RDWR);
	if (fd < 0)
	{
		printf("Open " MOTOR_DEVICE_NAME " error!\n");
		return;
	}
	ret = ioctl(fd, MTDRV_SELF_TEST);
	close(fd);
	return;
}

void doGetPos(int *pHPos, int *pVPos)
{
	int ret = 0;
	struct motor_pos pos = {0};

	int fd = -1;

	fd = open("/dev/" MOTOR_DEVICE_NAME, O_RDWR);
	if (fd < 0)
	{
		printf("Open " MOTOR_DEVICE_NAME " error!\n");
		return;
	}
	ret = ioctl(fd, MTDRV_GET_POS, &pos);
	*pHPos = pos.hpos;
	*pVPos = pos.vpos;
	MOTO_LOG("%s ret=%d, %s\n", __func__, ret, strerror(errno));

	close(fd);
	return;
}

void doGetStartStatus(int *status)
{
	int ret = 0;
	int fd = -1;

	fd = open("/dev/" MOTOR_DEVICE_NAME, O_RDWR);
	if (fd < 0)
	{
		printf("Open " MOTOR_DEVICE_NAME " error!\n");
		return;
	}
	ret = ioctl(fd, MTDRV_GET_START_STATE, status);
	MOTO_LOG("%s ret=%d, %s\n", __func__, ret, strerror(errno));

	close(fd);
	return;
}

void doGetSelfTestStatus(int *status)
{
	int ret = 0;
	int fd = -1;

	fd = open("/dev/" MOTOR_DEVICE_NAME, O_RDWR);
	if (fd < 0)
	{
		printf("Open " MOTOR_DEVICE_NAME " error!\n");
		return;
	}
	ret = ioctl(fd, MTDRV_GET_SELF_CHECK_STATE, status);
	MOTO_LOG("%s ret=%d, %s\n", __func__, ret, strerror(errno));

	close(fd);
	return;
}

int doMotoSelfCheckBack(int nHPos, int nVPos)
{
	int ret = 0;
	struct motor_pos pos = {0};

	int fd = -1;

	fd = open("/dev/" MOTOR_DEVICE_NAME, O_RDWR);
	if (fd < 0)
	{
		printf("Open " MOTOR_DEVICE_NAME " error!\n");
		return -1;
	}
	pos.hpos = nHPos;
	pos.vpos = nVPos;
	ret = ioctl(fd, MTDRV_SELF_CHECK_BACK, &pos);
	if (0 != ret)
	{
		MOTO_LOG("%s ret=%d, %s\n", __func__, ret, strerror(errno));
	}
	printf("-----------------MOTO_ToBeforePos \n\n");

	close(fd);
	return ret;
}

void doSetMotoMaxPos(int nHPos, int nVPos)
{
	int ret = 0;
	struct motor_pos pos = {0};

	int fd = -1;

	fd = open("/dev/" MOTOR_DEVICE_NAME, O_RDWR);
	if (fd < 0)
	{
		printf("Open " MOTOR_DEVICE_NAME " error!\n");
		return;
	}
	pos.hpos = nHPos;
	pos.vpos = nVPos;
	ret = ioctl(fd, MTDRV_SET_MAXPOS, &pos);
	MOTO_LOG("%s ret=%d, %s\n", __func__, ret, strerror(errno));

	close(fd);
	return;
}

void doMotoToPos(int nHPos, int nVPos)
{
	int ret = 0;
	struct motor_pos pos = {0};

	int fd = -1;

	fd = open("/dev/" MOTOR_DEVICE_NAME, O_RDWR);
	if (fd < 0)
	{
		printf("Open " MOTOR_DEVICE_NAME " error!\n");
		return;
	}
	pos.hpos = nHPos;
	pos.vpos = nVPos;
	ret = ioctl(fd, MTDRV_TO_POS, &pos);
	MOTO_LOG("%s ret=%d, %s\n", __func__, ret, strerror(errno));

	close(fd);
	return;
}

void doMotoToPosProd(int nHPos, int nVPos)
{
	int ret = 0;
	struct motor_pos pos = {0};

	int fd = -1;

	fd = open("/dev/" MOTOR_DEVICE_NAME, O_RDWR);
	if (fd < 0)
	{
		printf("Open " MOTOR_DEVICE_NAME " error!\n");
		return;
	}
	pos.hpos = nHPos;
	pos.vpos = nVPos;
	ret = ioctl(fd, MTDRV_TO_POS_PRODUCT, &pos);
	MOTO_LOG("%s ret=%d, %s\n", __func__, ret, strerror(errno));

	close(fd);
	return;
}

void doMotoToStartPos(int nHPos, int nVPos)
{
	int ret = 0;
	struct motor_pos pos = {0};

	int fd = -1;

	fd = open("/dev/" MOTOR_DEVICE_NAME, O_RDWR);
	if (fd < 0)
	{
		printf("Open " MOTOR_DEVICE_NAME " error!\n");
		return;
	}
	pos.hpos = nHPos;
	pos.vpos = nVPos;
	ret = ioctl(fd, MTDRV_TO_START_POS, &pos);
	MOTO_LOG("%s ret=%d, %s\n", __func__, ret, strerror(errno));

	close(fd);
	return;
}

void doMotoStop()
{
	int ret = 0;

	int fd = -1;
	fd = open("/dev/" MOTOR_DEVICE_NAME, O_RDWR);
	if (fd < 0)
	{
		printf("Open " MOTOR_DEVICE_NAME " error!\n");
		return;
	}
	ret = ioctl(fd, MTDRV_STOP);
	close(fd);
	return;
}
#if 0
void doSetMotoAddr()
{
	int ret = 0;
	moto_io stMotoIO;

	MOTO_LOG("%s start\n", __func__);
	int fd = -1;
	fd = open("/dev/" MOTOR_DEVICE_NAME, O_RDWR);
	if (fd < 0)
	{
		printf("Open " MOTOR_DEVICE_NAME " error!\n");
		return;
	}

	// 左右电机
	//  gpio1_4
	int offsetAddr = 0x01;
	offsetAddr = offsetAddr << 4; // gpio1_4 4个bit
	offsetAddr = offsetAddr << 2; // 第一位左移两个bit
	stMotoIO.aRLGpioInfo[0].gpioNO = 1;
	stMotoIO.aRLGpioInfo[0].gpioAddrOffset = offsetAddr; // 管脚对应的GPIO_DATA寄存器地址;
	stMotoIO.aRLGpioInfo[0].gpioMASK = 0x01 << 4;		 // 管脚对应的GPIO_DATA操作哟位置

	// gpio1_5
	offsetAddr = 0x01;
	offsetAddr = offsetAddr << 5; // gpio1_5 5个bit
	offsetAddr = offsetAddr << 2; // 第一位左移两个bit
	stMotoIO.aRLGpioInfo[1].gpioNO = 1;
	stMotoIO.aRLGpioInfo[1].gpioAddrOffset = offsetAddr; // 管脚对应的GPIO_DATA寄存器地址;
	stMotoIO.aRLGpioInfo[1].gpioMASK = 0x01 << 5;		 // 管脚对应的GPIO_DATA操作哟位置

	// gpio1_6
	offsetAddr = 0x01;
	offsetAddr = offsetAddr << 6; // gpio1_6 6个bit
	offsetAddr = offsetAddr << 2; // 第一位左移两个bit
	stMotoIO.aRLGpioInfo[2].gpioNO = 1;
	stMotoIO.aRLGpioInfo[2].gpioAddrOffset = offsetAddr; // 管脚对应的GPIO_DATA寄存器地址;
	stMotoIO.aRLGpioInfo[2].gpioMASK = 0x01 << 6;		 // 管脚对应的GPIO_DATA操作哟位置

	// gpio1_7
	offsetAddr = 0x01;
	offsetAddr = offsetAddr << 7; // gpio1_7 7个bit
	offsetAddr = offsetAddr << 2; // 第一位左移两个bit
	stMotoIO.aRLGpioInfo[3].gpioNO = 1;
	stMotoIO.aRLGpioInfo[3].gpioAddrOffset = offsetAddr; // 管脚对应的GPIO_DATA寄存器地址;
	stMotoIO.aRLGpioInfo[3].gpioMASK = 0x01 << 7;		 // 管脚对应的GPIO_DATA操作哟位置

	/////////////////////////////////////////////////////////////////////////////////////////////
	// 上下电机
	//  gpio6_4
	offsetAddr = 0x01;
	offsetAddr = offsetAddr << 4; // gpio6_4 4个bit
	offsetAddr = offsetAddr << 2; // PIO6_0
	stMotoIO.aUDGpioInfo[0].gpioNO = 6;
	stMotoIO.aUDGpioInfo[0].gpioAddrOffset = offsetAddr; // 管脚对应的GPIO_DATA寄存器地址;
	stMotoIO.aUDGpioInfo[0].gpioMASK = 0x01 << 4;		 // 管脚对应的GPIO_DATA操作哟位置

	// gpio6_5
	offsetAddr = 0x01;
	offsetAddr = offsetAddr << 5; // GPIO6_1
	offsetAddr = offsetAddr << 2; // 第一位左移两个bit
	stMotoIO.aUDGpioInfo[1].gpioNO = 6;
	stMotoIO.aUDGpioInfo[1].gpioAddrOffset = offsetAddr; // 管脚对应的GPIO_DATA寄存器地址;
	stMotoIO.aUDGpioInfo[1].gpioMASK = 0x01 << 5;		 // 管脚对应的GPIO_DATA操作哟位置

	// gpio6_6
	offsetAddr = 0x01;
	offsetAddr = offsetAddr << 6; // GPIO6_2
	offsetAddr = offsetAddr << 2; // 第一位左移两个bit
	stMotoIO.aUDGpioInfo[2].gpioNO = 6;
	stMotoIO.aUDGpioInfo[2].gpioAddrOffset = offsetAddr; // 管脚对应的GPIO_DATA寄存器地址;
	stMotoIO.aUDGpioInfo[2].gpioMASK = 0x01 << 6;		 // 管脚对应的GPIO_DATA操作哟位置

	// gpio6_7
	offsetAddr = 0x01;
	offsetAddr = offsetAddr << 7; // GPIO6_3
	offsetAddr = offsetAddr << 2; // 第一位左移两个bit
	stMotoIO.aUDGpioInfo[3].gpioNO = 6;
	stMotoIO.aUDGpioInfo[3].gpioAddrOffset = offsetAddr; // 管脚对应的GPIO_DATA寄存器地址;
	stMotoIO.aUDGpioInfo[3].gpioMASK = 0x01 << 7;		 // 管脚对应的GPIO_DATA操作哟位置

	ret = ioctl(fd, MTDRV_SET_IO_ADDR, &stMotoIO);
	close(fd);
	MOTO_LOG("%s end\n", __func__);
}
#endif
void doMotoLeft(int nSteps)
{
	int ret = 0;

	int fd = -1;
	fd = open("/dev/" MOTOR_DEVICE_NAME, O_RDWR);
	if (fd < 0)
	{
		printf("Open " MOTOR_DEVICE_NAME " error!\n");
		return;
	}
	ret = ioctl(fd, MTDRV_LEFT, &nSteps);
	close(fd);
	return;
}

void doMotoRight(int nSteps)
{
	int ret = 0;

	int fd = -1;
	fd = open("/dev/" MOTOR_DEVICE_NAME, O_RDWR);
	if (fd < 0)
	{
		printf("Open " MOTOR_DEVICE_NAME " error!\n");
		return;
	}
	ret = ioctl(fd, MTDRV_RIGHT, &nSteps);
	close(fd);
	return;
}

void doMotoUp(int nSteps)
{
	int ret = 0;

	int fd = -1;
	fd = open("/dev/" MOTOR_DEVICE_NAME, O_RDWR);
	if (fd < 0)
	{
		printf("Open " MOTOR_DEVICE_NAME " error!\n");
		return;
	}
	ret = ioctl(fd, MTDRV_UP, &nSteps);
	close(fd);
	return;
}

void doMotoDown(int nSteps)
{
	int ret = 0;

	int fd = -1;
	fd = open("/dev/" MOTOR_DEVICE_NAME, O_RDWR);
	if (fd < 0)
	{
		printf("Open " MOTOR_DEVICE_NAME " error!\n");
		return;
	}
	ret = ioctl(fd, MTDRV_DOWN, &nSteps);
	close(fd);
	return;
}

void doSetMotoSpeed(unsigned int nSpeed)
{
	int ret = 0;

	int fd = -1;
	fd = open("/dev/" MOTOR_DEVICE_NAME, O_RDWR);
	if (fd < 0)
	{
		printf("Open " MOTOR_DEVICE_NAME " error!\n");
	}
	ret = ioctl(fd, MTDRV_SET_SPEED_LEV, &nSpeed);
	close(fd);
}

void doSetMotoSpeed2(unsigned int nSpeedh, unsigned int nSpeedv)
{
	int ret = 0;
	struct motor_speed speed = {0};

	speed.hspeed = nSpeedh;
	speed.vspeed = nSpeedv;

	int fd = -1;
	fd = open("/dev/" MOTOR_DEVICE_NAME, O_RDWR);
	if (fd < 0)
	{
		printf("Open " MOTOR_DEVICE_NAME " error!\n");
	}
	ret = ioctl(fd, MTDRV_SET_SPEED_LEV_V2, &speed);
	close(fd);
}

void doSetMotoSpeedType(unsigned int hSpeedt, unsigned int vSpeedt)
{
	int ret = 0;
	struct motor_speedt speedt = {0};

	speedt.hspeedt = hSpeedt;
	speedt.vspeedt = vSpeedt;

	int fd = -1;
	fd = open("/dev/" MOTOR_DEVICE_NAME, O_RDWR);
	if (fd < 0)
	{
		printf("Open " MOTOR_DEVICE_NAME " error!\n");
	}
	ret = ioctl(fd, MTDRV_SET_SPEED_TYPE, &speedt);
	close(fd);
}

void doSetLogLevel(unsigned int level)
{
	int ret = 0;
	struct motor_ctrl ctl = {0};

	ctl.log_level = level;
	ctl.pwr = 1;

	int fd = -1;
	fd = open("/dev/" MOTOR_DEVICE_NAME, O_RDWR);
	if (fd < 0)
	{
		printf("Open " MOTOR_DEVICE_NAME " error!\n");
	}
	ret = ioctl(fd, MTDRV_CTRL, &ctl);
	close(fd);
}

void test_MotoLeft(int argc, char *argv[])
{
	unsigned int nsteps = 0;

	if (argc < 3)
		return;

	nsteps = atol(argv[2]);

	doMotoLeft(nsteps);
}

void test_MotoRight(int argc, char *argv[])
{
	unsigned int nsteps = 0;

	if (argc < 3)
		return;

	nsteps = atol(argv[2]);

	doMotoRight(nsteps);
}

void test_MotoUp(int argc, char *argv[])
{
	unsigned int nsteps = 0;

	if (argc < 3)
		return;

	nsteps = atol(argv[2]);

	doMotoUp(nsteps);
}

void test_MotoDown(int argc, char *argv[])
{
	unsigned int nsteps = 0;

	if (argc < 3)
		return;

	nsteps = atol(argv[2]);

	doMotoDown(nsteps);
}

void test_GetPos(int argc, char *argv[])
{
	struct motor_pos pos = {0};
	doGetPos(&pos.hpos, &pos.vpos);
	/* wait here for completing */
	MOTO_LOG("h_pos =%d, v_pos =%d\n", pos.hpos, pos.vpos);
}

void test_SetMaxPos(int argc, char *argv[])
{
	doSetMotoMaxPos(atol(argv[2]), atol(argv[3]));
}

void test_ToPos(int argc, char *argv[])
{
	doMotoToPos(atol(argv[2]), atol(argv[3]));
}

void test_ToPosProd(int argc, char *argv[])
{
	doMotoToPosProd(atol(argv[2]), atol(argv[3]));
}

void test_ToStartPos(int argc, char *argv[])
{
	doMotoToStartPos(atol(argv[2]), atol(argv[3]));
}

void test_GetStartState(int argc, char *argv[])
{
	int bStatus = 0;
	doGetStartStatus(&bStatus);
	/* wait here for completing */
	MOTO_LOG("StartStatus = %d\n", bStatus);
}

void test_GetSelfTestState()
{
	int bStatus = 0;
	doGetSelfTestStatus(&bStatus);
	/* wait here for completing */
	MOTO_LOG("SelfTestStatus = %d\n", bStatus);
}

void test_MotoSelfCheckBack(int argc, char *argv[])
{
	doMotoSelfCheckBack(atol(argv[2]), atol(argv[3]));
}
void test_setspeed(int argc, char *argv[])
{
	doSetMotoSpeed(atoi(argv[2]));
}

void test_setspeed2(int argc, char *argv[])
{
	doSetMotoSpeed2(atoi(argv[2]), atoi(argv[3]));
}

void test_setspeedt(int argc, char *argv[])
{
	doSetMotoSpeedType(atoi(argv[2]), atoi(argv[3]));
}

void test_set_loglevel(int argc, char *argv[])
{
	doSetLogLevel(atoi(argv[2]));
}

void *get_func(char *cmd)
{
	struct func_arr *fa = &func_tab[0];

	while (fa->cmd)
	{
		if (strcmp(cmd, fa->cmd) == 0)
		{
			MOTO_LOG("%s OK!!!\n", __func__);
			break;
		}
		fa++;
	}

	return (fa->func);
}

int main_body(int argc, char *argv[])
{
	do_func func = NULL;

	if (argc < 2)
		return -1;

	func = (do_func)get_func(argv[1]);
	MOTO_LOG("%s get func ok\n", __func__);
	if (!func)
		return -1;
	MOTO_LOG("%s exec func ok\n", __func__);
	func(argc, argv);

	return 0;
}

int main(int argc, char *argv[])
{
#if 0 // defined(MOTO_DBG)
	return main_dbg(argc, argv);
#else
	return main_body(argc, argv);
#endif
}
