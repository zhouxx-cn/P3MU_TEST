#ifndef __DRV_EXPWM_H__
#define __DRV_EXPWM_H__

typedef enum expwm_cmd
{
    EXPWM_CMD_ENABLE = 100,
    EXPWM_CMD_DISABLE,
    EXPWM_CMD_SET_CONFIG,
    EXPWM_CMD_GET_CONFIG,
} EXPWM_CMD_E;

typedef struct expwm_config
{
    int pwm_id;
    int duty_ns;
    int period_ns;
} EXPWM_CONFIG_S, PEXPWM_CONFIG_S;

#endif // __DRV_EXPWM_H__