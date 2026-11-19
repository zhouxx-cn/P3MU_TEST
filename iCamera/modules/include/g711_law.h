#ifndef __G711_LAW_H__
#define __G711_LAW_H__

/* 原始PCM数据编码为G711A */
signed PCM_ENCODE_G711A(unsigned char* dst, signed short* src, size_t srcSize);

/* G711A转化为PCM */
signed G711A_DECODE_PCM(signed short* dst, const unsigned char* src, size_t srcSize);

/* 原始PCM数据编码为G711U */
unsigned PCM_ENCODE_G711U(unsigned char* dst, signed short* src, size_t srcSize);

/* G711U数据解码为PCM */
unsigned G711U_DECODE_PCM(signed short* dst, const unsigned char* src, size_t srcSize);

/* G711A转化为G711U */
unsigned G711A_TO_G711U(unsigned char* dst, const unsigned char* src, size_t srcSize);

/* G711U转化为G711A */
unsigned G711U_TO_G711A(unsigned char* dst, const unsigned char* src, size_t srcSize);

#endif // end of __G711_LAW_H__
