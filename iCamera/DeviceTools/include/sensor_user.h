#ifndef __SENSOR_CTRL_H__
#define __SENSOR_CTRL_H__

int sensor_user_i2c_init(char *sensor_name);

int sensor_user_i2c_exit(void);

int sensor_user_i2c_read_register(int addr);

int sensor_user_i2c_get_id(char *pszSensorName, int NameSizeMax);

int sensor_user_i2c_write_register(int addr, int data);

int sensor_user_spi_write_register(unsigned int addr, unsigned char data);

int sensor_user_spi_read_register(unsigned int addr);

#endif /* end of __SENSOR_CTRL_H__ */

