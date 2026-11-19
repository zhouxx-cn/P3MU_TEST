#ifndef __UTILS_H__
#define __UTILS_H__

void * memmap(unsigned long phy_addr, unsigned long size);
int memunmap(void * addr_mapped);

int reg_read(unsigned int phy_addr, unsigned int *value);
int reg_write(unsigned int phy_addr, unsigned int value);

int i2c_write(unsigned int i2c_num, unsigned int dev_addr,
		unsigned int reg_addr, unsigned int data,
		unsigned int reg_width, unsigned int data_width);

int i2c_read(unsigned int i2c_num, unsigned int dev_addr, unsigned int reg_addr,
		unsigned int reg_addr_end, unsigned int reg_width,
		unsigned int data_width, unsigned int *value, int len);
#endif