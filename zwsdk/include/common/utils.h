#ifndef __UTILS_H__
#define __UTILS_H__

#define PAGE_SIZE 0x1000
#define PAGE_SIZE_MASK (~(0xfff))


#define COLOR_BLACK		(1)  //黑色
#define COLOR_WITHE		(2)  //白色
#define BYTE_PIXELS		(2)  //一字节表示像素个数
#define PIXEL_BITS		(4)  //一字像素占用bit个数
#define BYTE_ROUND		(0xff >> (8 - PIXEL_BITS))


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

/***
 * @description: 给字符点阵描边
 * @param {int} width 点阵宽度
 * @param {int} height 点阵高度
 * @param {unsigned short} *data 点阵数据地址
 * @param {unsigned short} front_color 前景色
 * @param {unsigned short} stroke_color 描边色
 * @param {int} storke_width 描边宽度，通常为1
 * @return {int} 0-成功，非0-失败
 */
int font_stroke(int width, int height, unsigned char *data, unsigned char front_color, unsigned char stroke_color, int storke_width,int stride);


/***
 * @description: 去除点阵的描边
 * @param {int} width 点阵宽度
 * @param {int} height 点阵高度
 * @param {unsigned short} *data 点阵数据地址
 * @param {unsigned short} stroke_color 描边色
 * @param {unsigned short} background_color 背景色
 * @return {*}
 */
int font_unstroke(int width, int height, unsigned short *data, unsigned short stroke_color, unsigned short background_color);

#endif
