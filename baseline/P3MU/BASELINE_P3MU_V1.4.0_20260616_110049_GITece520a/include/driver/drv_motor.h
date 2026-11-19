#ifndef __DRIVER_MOTOR_H__
#define __DRIVER_MOTOR_H__
#include <linux/ioctl.h>
#include <linux/types.h>

// #define MOTO_DBG
#define MOTOR_DEVICE_NAME "motor"
#define MOTOR_IOCTL_BASE (0x11)

typedef enum motor_type
{
	MOTOR_HORIZONTAL,
	MOTOR_VERTICAL,
	MOTOR_CNT,
} MOTOR_TYPE_E;

typedef enum motor_log_level {
    MOTOR_LOG_ERR = 0,
    MOTOR_LOG_WRN,
    MOTOR_LOG_INF,
    MOTOR_LOG_DBG,
} MOTOR_LOG_LEVEL_E;

typedef struct motor_pos
{
	int hpos;
	int vpos;
} MOTOR_POS_S;

typedef struct motor_speed
{
	unsigned int hspeed;
	unsigned int vspeed;
} MOTOR_SPEED_S;

typedef enum
{
	MOTOR_SPEED_TYPE_1_64,		/* 1:64 */
	MOTOR_SPEED_TYPE_1_25_64,	/* 1:64电机，额外加一组齿轮 */
	MOTOR_SPEED_TYPE_CNT,
} MOTOR_SPEED_TYPE_E;

typedef struct motor_speedt
{
	unsigned int hspeedt;
	unsigned int vspeedt;
} MOTOR_SPEED_TYPE_S;

typedef struct motor_ctrl
{
	unsigned int pwr;
	unsigned int log_level;
} MOTOR_CTRL_S;

enum
{
	MTDRV_NONE = _IO(MOTOR_IOCTL_BASE, 0),
	MTDRV_SELF_TEST = _IO(MOTOR_IOCTL_BASE, 1),
	MTDRV_LEFT = _IOW(MOTOR_IOCTL_BASE, 2, unsigned int),
	MTDRV_RIGHT = _IOW(MOTOR_IOCTL_BASE, 3, unsigned int),
	MTDRV_UP = _IOW(MOTOR_IOCTL_BASE, 4, unsigned int),
	MTDRV_DOWN = _IOW(MOTOR_IOCTL_BASE, 5, unsigned int),
	MTDRV_TO_POS = _IOW(MOTOR_IOCTL_BASE, 6, struct motor_pos),
	MTDRV_GET_POS = _IOR(MOTOR_IOCTL_BASE, 7, struct motor_pos),
	MTDRV_STOP = _IO(MOTOR_IOCTL_BASE, 8),
	MTDRV_SET_MAXPOS = _IOW(MOTOR_IOCTL_BASE, 9, struct motor_pos),
	MTDRV_GET_STATE = _IOR(MOTOR_IOCTL_BASE, 10, unsigned int),
	MTDRV_SET_SPEED_LEV = _IOW(MOTOR_IOCTL_BASE, 11, unsigned int),
	MTDRV_SELF_TEST_LOOP = _IO(MOTOR_IOCTL_BASE, 12),
	MTDRV_TO_START_POS = _IOW(MOTOR_IOCTL_BASE, 13, struct motor_pos),
	MTDRV_GET_START_STATE = _IOR(MOTOR_IOCTL_BASE, 14, unsigned int),
	MTDRV_SET_SPEED_LEV_V2 = _IOW(MOTOR_IOCTL_BASE, 15, struct motor_speed),
	MTDRV_GET_SELF_CHECK_STATE = _IOR(MOTOR_IOCTL_BASE, 16, unsigned int),
	MTDRV_SELF_CHECK_BACK = _IOW(MOTOR_IOCTL_BASE, 17, struct motor_pos),
	MTDRV_TO_POS_PRODUCT = _IOW(MOTOR_IOCTL_BASE, 18, struct motor_pos),
	MTDRV_SET_POS = _IOW(MOTOR_IOCTL_BASE, 19, struct motor_pos),
	MTDRV_CTRL = _IOW(MOTOR_IOCTL_BASE, 20, struct motor_ctrl),
	MTDRV_GET_MAXPOS = _IOR(MOTOR_IOCTL_BASE, 21, struct motor_pos),
	MTDRV_GET_CTRL = _IOR(MOTOR_IOCTL_BASE, 22, struct motor_ctrl),
	MTDRV_SET_SPEED_TYPE = _IOW(MOTOR_IOCTL_BASE, 23, struct motor_speedt),
};

typedef enum motor_speed_level
{
	MOTOR_SPEED_LV1 = 0,
	MOTOR_SPEED_LV2,
	MOTOR_SPEED_LV3,
	MOTOR_SPEED_LV4,
	MOTOR_SPEED_LV5,
	MOTOR_SPEED_LV6,
	MOTOR_SPEED_LV7,
	MOTOR_SPEED_LV8,
	MOTOR_SPEED_LV9,
	MOTOR_SPEED_LV_MAX
} MOTOR_SPEED_LEVEL_E;

typedef enum motor_state
{
	MOTOR_STATE_IDLE = 0,
	MOTOR_STATE_SELF_TEST,
	MOTOR_STATE_PRESETTING,
	MOTOR_STATE_TRIMMING,
	MOTOR_STATE_CRUISING,
	MOTOR_STATE_CONTROLLING,
	MOTOR_STATE_TOPOS,
	MOTOR_STATE_TOBOUND
} MOTOR_STATE_E;

typedef enum motor_cmd
{
	MOTOR_CMD_NONE = 0,
	MOTOR_CMD_LEFT,
	MOTOR_CMD_RIGHT,
	MOTOR_CMD_UP,
	MOTOR_CMD_DOWN,
	MOTOR_CMD_TOPOS,
	MOTOR_CMD_TESTSELF,
	MOTOR_CMD_TESTSELF_LOOP,
	MOTOR_CMD_TOSTARTPOS,
	MOTOR_CMD_SELF_CHECK_BACK,
	MOTOR_CMD_TOPOS_PRODUCT,
	MOTOR_CMD_MAX
} MOTOR_CMD_E;

#endif /* __DRIVER_MOTOR_H__ */
