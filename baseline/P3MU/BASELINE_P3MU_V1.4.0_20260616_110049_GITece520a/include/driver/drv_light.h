#ifndef __DRV_LIGHT_H__
#define __DRV_LIGHT_H__

typedef enum light_cmd
{
    LIGHT_CMD_SET_IR = 0x100,
    LIGHT_CMD_GET_IR,
    LIGHT_CMD_SET_IR2,
    LIGHT_CMD_GET_IR2,
    LIGHT_CMD_SET_WHITE = 0x110,
    LIGHT_CMD_GET_WHITE,
    LIGHT_CMD_SET_WHITE2,
    LIGHT_CMD_GET_WHITE2,
    LIGHT_CMD_SET_POWER = 0x120,
    LIGHT_CMD_GET_POWER,
    LIGHT_CMD_SET_NET = 0x130,
    LIGHT_CMD_GET_NET,
    LIGHT_CMD_SET_FLASH = 0x140,
    LIGHT_CMD_GET_FLASH,
    LIGHT_CMD_SET_BACK = 0x150,
    LIGHT_CMD_GET_BACK,    
    LIGHT_CMD_GET_CDS = 0x200,
    LIGHT_CMD_GET_CDS2,
} LIGHT_CMD_E;

typedef struct light_attr
{
    unsigned int value;     // PWM: 0-100, CDS: 0/1, GPIO: 0, 100
    unsigned int frq_khz;   // PWM频率, CDS: 无效, GPIO无效
    unsigned int frq_hz;    /* 为了兼容需要更低频率的灯控需求，最终生效值为frq_khz * 1000 + frq_hz */
} LIGHT_ATTR_S, * PLIGHT_ATTR_S;

#endif
