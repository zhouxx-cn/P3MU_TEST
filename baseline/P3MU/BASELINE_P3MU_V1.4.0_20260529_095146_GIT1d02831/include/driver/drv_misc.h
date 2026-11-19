#ifndef __DRV_MISC_H__
#define __DRV_MISC_H__

typedef enum misc_cmd {
    MISC_CMD_GET_DEFKEY = 0x10,
    MISC_CMD_SET_IRCUT = 0x20,
    MISC_CMD_SET_IRCUT2,
    MISC_CMD_SET_SPK = 0x30,
    MISC_CMD_SET_WIFI = 0x40,
    MISC_CMD_GET_KEYCALL = 0x50,
    MISC_CMD_GET_KEYCALL2 = 0x51,
    MISC_CMD_SET_CHGEN = 0x60,
    MISC_CMD_SET_CHGRST,
} MISC_CMD_E;

typedef struct misc_defkey_attr {
    unsigned int stat;
} MISC_DEFKEY_ATTR_S, *PMISC_DEFKEY_ATTR_S;

typedef struct misc_charge_attr {
    unsigned int stat;
} MISC_CHARGE_ATTR_S, *PMISC_CHARGE_ATTR_S;

// 一键呼叫，易莱孚需求
typedef struct misc_keycall_attr {
    unsigned int stat;
    unsigned int type;
} MISC_KEYCALL_ATTR_S, *PMISC_KEYCALL_ATTR_S;

typedef struct misc_spk_attr {
    unsigned int stat;
} MISC_SPK_ATTR_S, *PMISC_SPK_ATTR_S;

typedef struct misc_wifi_attr {
    unsigned int stat;
} MISC_WIFI_ATTR_S, *PMISC_WIFI_ATTR_S;

typedef enum misc_ircut_mode {
    MISC_IRCUT_MODE_DAY = 0,
    MISC_IRCUT_MODE_NIGHT,
} MISC_IRCUT_MODE_E;

typedef struct misc_ircut_attr {
    unsigned int mode;  // MISC_IRCUT_MODE_E
} MISC_IRCUT_ATTR_S, *PMISC_IRCUT_ATTR_S;

#endif