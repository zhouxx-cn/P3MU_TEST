#ifndef __CRC32_H__
#define __CRC32_H__

/**
 * @description: 支持内存分块计算crc
 * @param {unsigned int} crc crc起始值
 * @param {unsigned char*} buf 需计算crc内存的起始地址
 * @param {unsigned int} size 内存大小
 * @return {unsigned int} crc32结果
 */
unsigned int utils_crc32(unsigned int crc, const unsigned char* buf, unsigned int size);

#endif // __CRC32_H__