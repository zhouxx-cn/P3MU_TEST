#ifndef __DRV_ALARM_H__
#define __DRV_ALARM_H__

typedef enum alarm_cmd {
    ALARM_CMD_SET_ALARM = 0x100,
    ALARM_CMD_GET_ALARM,
} ALARM_CMD_E;

typedef enum alarm_type {
    ALARM_TYPE_IN = 0,
    ALARM_TYPE_OUT,
} ALARM_TYPE_E;

typedef struct alarm_attr {
    ALARM_TYPE_E type;
    unsigned int chn;
    unsigned int value;
} ALARM_ATTR_S, *PALARM_ATTR_S;

#endif  // __ALARM_H__