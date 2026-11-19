#ifndef __GPIO_CTRL_H__
#define __GPIO_CTRL_H__

typedef struct {
    unsigned int  groupnumber;
    unsigned int  bitnumber;
    unsigned int  value;
}gpio_groupbit_info;

#define GPIO_MAGIC      'G'
#define GPIO_SET_DIR    0x110
#define GPIO_GET_DIR    0x111
#define GPIO_READ_BIT   0x112    //_IOWR(IOC_TYPE_GPIO, 0x03, gpio_groupbit_info)
#define GPIO_WRITE_BIT  0x113    //_IOW(IOC_TYPE_GPIO, 0x04, gpio_groupbit_info)


#define GPIOGET_ALARM_OUT       0x103
#define GPIOGET_ALARM_IN        0x104
#define GPIOGET_BUZZER          0x107		
#define GPIOGET_WATCHDOG        0x109
#define GPIOGET_SERIAL485       0x10A	
#define GPIOGET_WATCHDOGEX      0x10B
#define GPIOGET_IPSTATUS        0x10D
#define GPIOGET_LEDSTATUS	    0x10E

int init_gpio(void);
int deInit_gpio(void);
int gpio_ioctl(int ioct_value, void *pValue);

#endif /* end of gpio_ctrl */
