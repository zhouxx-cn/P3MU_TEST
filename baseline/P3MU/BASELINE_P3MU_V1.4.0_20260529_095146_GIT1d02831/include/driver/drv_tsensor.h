#ifndef __DRV_TSENSOR_H__
#define __DRV_TSENSOR_H__

#define MAX_TEMPERATURE_NUMBER			(8)

typedef enum tsensor_cmd {
	TSIOC_GETSTATUS 	= 0x10,				// get tsensor enable/mode/circletime/temperature 
	TSIOC_GETENABLE 	= 0x20,				// get tsensor enable
	TSIOC_GETMODE	 	= 0x30,				// get tsensor mode
	TSIOC_GETCIRCLETIME	= 0x40,				// get tsensor circle time
	TSIOC_SETENABLE		= 0x50,				// set tsensor enable
	TSIOC_SETMODE		= 0x60,				// set tsensor mode
	TSIOC_SETCIRCLETIME	= 0x70,				// set tsensor circletime
} TSENSOR_CMD_E;

typedef struct tsensor_attr {
	int support;
	int enable;
	int mode;				/* 0 单次采集模式，1 循环采集模式*/
	int circletime;
	int temperature[MAX_TEMPERATURE_NUMBER];
	int version;
} TSENSOR_ATTR_S;

#endif
