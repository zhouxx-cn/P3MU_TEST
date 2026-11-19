#ifndef __DRV_HWCONF_H__
#define __DRV_HWCONF_H__

typedef enum hwconf_cmd
{
    HWCONF_CMD_GET_MISC_CONF = 0x0010,
    HWCONF_CMD_SET_MISC_CONF,
    HWCONF_CMD_GET_ALARM_CONF = 0x0020,
    HWCONF_CMD_SET_ALARM_CONF,
    HWCONF_CMD_GET_LIGHT_CONF = 0x0040,
    HWCONF_CMD_SET_LIGHT_CONF,
    HWCONF_CMD_GET_PWM_CONF = 0x0050,
    HWCONF_CMD_SET_PWM_CONF,
    HWCONF_CMD_GET_HE2866_CONF = 0x0060,
    HWCONF_CMD_SET_HE2866_CONF,
    HWCONF_CMD_GET_WIFI_CONF = 0x0070,
    HWCONF_CMD_SET_WIFI_CONF,
    HWCONF_CMD_GET_MOTOR_CONF = 0x0080,
    HWCONF_CMD_SET_MOTOR_CONF,    
    HWCONF_CMD_GET_MX2208A_CONF = 0x0090,
    HWCONF_CMD_SET_MX2208A_CONF,
} HWCONF_CMD_E;

typedef enum hwconf_gpio_dir
{
    HWCONF_GPIO_DIR_OUT = 0,
    HWCONF_GPIO_DIR_IN = 1,
} HWCONF_GPIO_DIR_E;

typedef enum hwconf_gpio_level
{
    HWCONF_GPIO_LEVEL_LOW = 0,
    HWCONF_GPIO_LEVEL_HIGH = 1,
} HWCONF_GPIO_LEVEL_E;

typedef struct hwconf_gpio_conf
{
    int num;  // GPIO引脚序号
    int dir;  // HWCONF_GPIO_DIR_E 0：GPIO输出 1：输入
    int init; // 配置初始状态，0: 不触发，1：触发
    int trig; // HWCONF_GPIO_LEVEL_E 触发电平
} HWCONF_GPIO_CONF_S, *PHWCONF_GPIO_CONF_S;

#define HWCONF_GPIO_INIT_LEVEL(conf) (((conf)->trig == HWCONF_GPIO_LEVEL_HIGH) ? ((conf)->init) : !((conf)->init))
#define HWCONF_GPIO_LEVEL(conf, level) (((conf)->trig == HWCONF_GPIO_LEVEL_HIGH) ? (level) : !(level))

#define HWCONF_MAX_ALARM_CHN (8)
/*
 * 基础硬件配置
 */
#define BOARD_NAME_LEN     (32)
#define SENSOR_NAME_LEN    (32)
#define CHIP_NAME_LEN      (32)
typedef struct hwconf_base_conf
{
    char board[BOARD_NAME_LEN];
    char sensor[SENSOR_NAME_LEN];
    char chip[CHIP_NAME_LEN];
} HWCONF_BASE_CONF_S, * PHWCONF_BASE_CONF_S;

/*
 * 系统相关配置
 */
typedef struct hwconf_zram_conf
{
    unsigned int en;            /* 是否使能 */
    unsigned int size;          /* swap分区大小，MB */
    unsigned int swappiness;    /* vm.swappiness */
} HWCONF_ZRAM_CONF_S, *PHWCONF_ZRAM_CONF_S;

typedef struct hwconf_sys_conf
{
    HWCONF_ZRAM_CONF_S zram;
} HWCONF_SYS_CONF_S, *PHWCONF_SYS_CONF_S;

/*
 * 报警输出相关配置
 */
typedef enum hwconf_alarm_type
{
	HWCONF_ALARM_TYPE_GPIO = 0,
	HWCONF_ALARM_TYPE_HTR33351,
} HWCONF_ALARM_TYPE_E;

typedef struct hwconf_alarm_conf
{
    unsigned int alarmin_num;	
	HWCONF_ALARM_TYPE_E alarmin_type[HWCONF_MAX_ALARM_CHN];
    HWCONF_GPIO_CONF_S alarmin[HWCONF_MAX_ALARM_CHN];
    unsigned int alarmout_num;	
	HWCONF_ALARM_TYPE_E alarmout_type[HWCONF_MAX_ALARM_CHN];
    HWCONF_GPIO_CONF_S alarmout[HWCONF_MAX_ALARM_CHN];	
} HWCONF_ALARM_CONF_S, *PHWCONF_ALARM_CONF_S;

/*
 * 灯板相关配置
 */
typedef enum hwconf_cds_type
{
    HWCONF_CDS_TYPE_GPIO = 0,
    HWCONF_CDS_TYPE_ADC,
    HWCONF_CDS_TYPE_HTR33351,
} HWCONF_CDS_TYPE_E;

typedef struct hwconf_cds_conf
{
    HWCONF_CDS_TYPE_E type;
    int adc_chn;
    HWCONF_GPIO_CONF_S gpio;
} HWCONF_CDS_CONF_S;

typedef enum hwconf_light_ctltype
{
    HWCONF_LIGHT_CTLTYPE_GPIO = 0,
    HWCONF_LIGHT_CTLTYPE_PWM,
    HWCONF_LIGHT_CTLTYPE_HTR33351,
} HWCONF_LIGHT_CTLTYPE_E;

typedef struct hwconf_mlight_conf
{
    HWCONF_LIGHT_CTLTYPE_E ctltype;
    int pwm;
    HWCONF_GPIO_CONF_S gpio;
} HWCONF_MLIGHT_CONF_S;

typedef struct hwconf_light_conf
{
    HWCONF_CDS_CONF_S cds;      // 光敏输入
    HWCONF_CDS_CONF_S cds2;      // 光敏输入
    HWCONF_MLIGHT_CONF_S ir;    // 红外补光灯
    HWCONF_MLIGHT_CONF_S ir2;    // 红外补光灯
    HWCONF_MLIGHT_CONF_S white; // 白光补光灯
    HWCONF_MLIGHT_CONF_S white2; // 白光补光灯
    HWCONF_MLIGHT_CONF_S power; // 电源指示灯
    HWCONF_MLIGHT_CONF_S net;   // 网络指示灯
    HWCONF_MLIGHT_CONF_S flash; // 报警闪烁灯
    HWCONF_MLIGHT_CONF_S back; // 屏幕背光灯
} HWCONF_LIGHT_CONF_S, *PHWCONF_LIGHT_CONF_S;

/*
 * PWM相关配置
 */
typedef struct hwconf_expwm_conf
{
    unsigned int support_mask;
} HWCONF_EXPWM_CONF_S, *PHWCONF_EXPWM_CONF_S;

/*
 * HE2866相关配置
 */
typedef struct hwconf_he2866_conf
{
    HWCONF_GPIO_CONF_S en;
    HWCONF_GPIO_CONF_S sda;
    HWCONF_GPIO_CONF_S scl;
} HWCONF_HE2866_CONF_S, *PHWCONF_HE2866_CONF_S;

/*
 * MX2208A相关配置
 */
typedef struct hwconf_mx2208a_conf
{
    HWCONF_GPIO_CONF_S en;
    HWCONF_GPIO_CONF_S sda;
    HWCONF_GPIO_CONF_S scl;
} HWCONF_MX2208A_CONF_S, *PHWCONF_MX2208A_CONF_S;

/*
 * HTR33351相关配置
 */
typedef struct hwconf_htr33351_conf
{
	int i2c_chn;
} HWCONF_HTR33351_CONF_S, *PHWCONF_HTR33351_CONF_S;

/*
 * MISC驱动模块相关配置
 */
/*
 * IRCut相关配置
 */
typedef enum hwconf_ircut_ctltype
{
    HWCONF_IRCUT_CTLTYPE_GPIO = 0,
    HWCONF_IRCUT_CTLTYPE_HE2866,
    HWCONF_IRCUT_CTLTYPE_HTR33351,
    HWCONF_IRCUT_CTLTYPE_MX2208A,
} HWCONF_IRCUT_CTLTYPE_E;

typedef struct hwconf_ircut_conf
{
    HWCONF_IRCUT_CTLTYPE_E ctltype; // 控制类型
    HWCONF_GPIO_CONF_S pina;        // IRCut引脚A
    HWCONF_GPIO_CONF_S pinb;        // IRCut引脚B
    unsigned int hold_ms;           // 持续时间
    unsigned int revert;            // 反向
} HWCONF_IRCUT_CONF_S, *PHWCONF_IRCUT_CONF_S;

typedef struct hwconf_sensor_conf
{
    HWCONF_GPIO_CONF_S rst_gpio;   // sensor reset
    unsigned int rst_init;    // 初始化时reset一次
    unsigned int rst_hold_ms; // reset保持时间
} HWCONF_SENSOR_CONF_S, *PHWCONF_SENSOR_CONF_S;

#define WIFI_MODEL_NAME_LEN (32)
typedef struct hwconf_wifi_conf
{
    HWCONF_GPIO_CONF_S power;           // wifi电源使能
    unsigned int support;               // 是否支持wifi功能
    char model[WIFI_MODEL_NAME_LEN];    // wifi型号
} HWCONF_WIFI_CONF_S, *PHWCONF_WIFI_CONF_S;

typedef struct hwconf_charge_conf
{
    HWCONF_GPIO_CONF_S enable;   //充电使能
    HWCONF_GPIO_CONF_S reset;    //充电复位
} HWCONF_CHARGE_CONF_S, *PHWCONF_CHARGE_CONF_S;

typedef enum hwconf_keycall_type
{
    HWCONF_KEYCALL_TYPE_GPIO = 0,
    HWCONF_KEYCALL_TYPE_ADC,
    HWCONF_KEYCALL_TYPE_BUTT,
} HWCONF_KEYCALL_TYPE_E;

typedef struct hwconf_keycall_conf
{
    HWCONF_KEYCALL_TYPE_E type;
    int adc_chn;
    HWCONF_GPIO_CONF_S gpio;
} HWCONF_KEYCALL_CONF_S;

typedef struct hwconf_misc_conf {
    HWCONF_IRCUT_CONF_S ircut;
    HWCONF_IRCUT_CONF_S ircut2;
    HWCONF_GPIO_CONF_S sys_rst;    // 系统复位
    HWCONF_GPIO_CONF_S spk_power;  // 喇叭使能
    HWCONF_GPIO_CONF_S sd_power;   // SD卡使能
    HWCONF_KEYCALL_CONF_S keycall; // 一键呼叫
    HWCONF_KEYCALL_CONF_S keycall2;// 一键呼叫
    HWCONF_WIFI_CONF_S wifi;       // WIFI
    HWCONF_CHARGE_CONF_S charge;   // 充电引脚
    HWCONF_SENSOR_CONF_S sensor;   // SENSOR
    unsigned int net4g_support;    // 是否支持4G网络功能
    unsigned int bt_support;       // 是否支持蓝牙功能
    unsigned int usb_detect;       // 是否存在usb设备，进行usb设备类型检测
} HWCONF_MISC_CONF_S, *PHWCONF_MISC_CONF_S;

/*
 * 马达驱动配置
 */
typedef enum hwconf_motor_ctltype
{
    HWCONF_MOTOR_CTLTYPE_GPIO = 0,
    HWCONF_MOTOR_CTLTYPE_HE2866,
    HWCONF_MOTOR_CTLTYPE_HTR33351,
    HWCONF_MOTOR_CTLTYPE_MX2208A,
} HWCONF_MOTOR_CTLTYPE_E;

typedef struct hwconf_motor_conf
{
    unsigned int support;
    HWCONF_MOTOR_CTLTYPE_E ctltype;
    HWCONF_HE2866_CONF_S he2866_cfg;
	HWCONF_MX2208A_CONF_S mx2208a_cfg;
    struct
    {
        unsigned int level;
        HWCONF_GPIO_CONF_S wire1;
        HWCONF_GPIO_CONF_S wire2;
        HWCONF_GPIO_CONF_S wire3;
        HWCONF_GPIO_CONF_S wire4;
    } horizontal;
    struct
    {
        unsigned int level;
        HWCONF_GPIO_CONF_S wire1;
        HWCONF_GPIO_CONF_S wire2;
        HWCONF_GPIO_CONF_S wire3;
        HWCONF_GPIO_CONF_S wire4;
    } vertical;
} HWCONF_MOTOR_CONF_S, *PHWCONF_MOTOR_CONF_S;

#endif // __DRV_HWCONF_H__
