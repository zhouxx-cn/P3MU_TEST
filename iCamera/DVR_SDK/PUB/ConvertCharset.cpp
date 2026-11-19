
/*!
 *\file		ConvertCharset.cpp
 *\brief		PUB模块，字符集转换函数
 *\author		林云
 *\date		2009/12/22
 */

#include <stdio.h>
#include<stdlib.h>
#include<string.h>
// #include "sys_banned.h"
#include "global_gather.h"
#define	DEFAULT_CHAR	('?')


//!\brief       unicode字符串转utf8字符串
//!\param   dst 目的字符串(utf8编码)
//!\param   src 源字符串(unicode编码)
//!\param   len 源字符串长度(单位:字节)
//!\return  转换后字符串的长度(单位:字节)
int uc16_conv_to_utf8(unsigned char *dst, const unsigned char *src, int len)
{
	const unsigned short *pSrc;

	unsigned short unicode;

	unsigned char ch1, ch2, ch3;

	int dst_len;

	if (NULL == dst)
	{
		return -1;
	}

	if ((NULL == src) || (len < 2))
	{
		*dst = 0;
		return 0;
	}

	pSrc = (const unsigned short *) src;
	dst_len = 0;
	len >>= 1;

	while (len > 0)
	{
		unicode = *pSrc++;

		if (unicode == 0)
		{
			//结束字符
			break;
		}
		else if (unicode < 0x80)
		{
			// 1个字符
			ch1 = (unsigned char) unicode;

			*dst++ = ch1;

			++dst_len;
		}
		else if (unicode < 0x800)
		{
			// 2个字符

			ch1 = ((unicode >> 6) & 0x1F) | 0xC0;
			ch2 = (unicode & 0x3F) | 0x80;

			*dst++ = ch1;
			*dst++ = ch2;

			dst_len += 2;
		}
		else
		{
			// 3个字符

			ch1 = ((unicode >> 12) & 0xF) | 0xE0;
			ch2 = ((unicode >> 6) & 0x3F) | 0x80;
			ch3 = (unicode & 0x3F) | 0x80;

			*dst++ = ch1;
			*dst++ = ch2;
			*dst++ = ch3;

			dst_len += 3;
		}

		--len;
	}

	*dst = 0;

	return dst_len;
}

//!\brief       hecs1字符串转unicode字符串
//!\param   dst 目的字符串(unicode编码)
//!\param   src 源字符串(hecs1编码)
//!\param   len 源字符串长度(单位:字节)
//!\return  转换后字符串的长度(单位:字节)
int hecs1_conv_to_uc16(unsigned char *dst, const unsigned char *src, int len)
{
	unsigned short *pDst;

	unsigned short unicode;

	unsigned char ch1, ch2;

	int dst_len;

	if (NULL == dst)
	{
		return -1;
	}

	pDst = (unsigned short *) dst;

	if ((NULL == src) || (len <= 0))
	{
		*pDst = 0;
		return 0;
	}

	dst_len = 0;

	while (len > 0)
	{
		ch1 = *src;

		if (ch1 == 0)
		{
			//ch1为结束字符
			break;
		}
		else if (ch1 & 0x80)
		{
			if (len < 2)
			{
				//剩余字符串长度不够，则当前字符错误，转换为缺省字符
				*pDst++ = DEFAULT_CHAR;
				++dst_len;
				break;
			}

			ch2 = *(src + 1);

			if (ch2 == 0)
			{
				//ch2为结束字符，则当前字符错误，转换为缺省字符
				*pDst++ = DEFAULT_CHAR;
				++dst_len;
				break;
			}
			else
			{
				const unsigned char ucSpecialChars[] = { '\\', '%', '\n', '\t', '\r', ' ' };
				const unsigned char usSpecialCharsCount = sizeof (ucSpecialChars) / sizeof (ucSpecialChars[0]);

				unicode = ch1 & 0x7F;
				unicode *= 255 - usSpecialCharsCount;

				if (ch2 > (255 - usSpecialCharsCount))
				{
					unicode += ucSpecialChars[255 - ch2] - 1;
				}
				else
				{
					unicode += ch2 - 1;
				}

				if (unicode < HECS1_CharCount_1)
				{
					//中文第一部分
					unicode += HECS1_FirstChar_1;

					*pDst++ = unicode;
					++dst_len;
				}
				else if (unicode < (HECS1_CharCount_1 + HECS1_CharCount_2))
				{
					//第二部分 拉丁文
					unicode -= HECS1_CharCount_1;
					unicode += HECS1_FirstChar_2;

					*pDst++ = unicode;
					++dst_len;
				}
				else
				{
					//暂不支持的字符转换为缺省字符
					*pDst++ = DEFAULT_CHAR;
					++dst_len;
				}

				src += 2;
				len -= 2;
			}
		}
		else
		{
			//ascii 码字符
			*pDst++ = ch1;
			++dst_len;

			++src;
			--len;
		}
	}

	*pDst = 0;

	return dst_len << 1;
}

//!\brief       utf8字符串转unicode字符串
//!\param   dst 目的字符串(unicode编码)
//!\param   src 源字符串(utf8编码)
//!\param   len 源字符串长度(单位:字节)
//!\return  转换后字符串的长度(单位:字节)
int utf8_conv_to_uc16(unsigned char *dst, const unsigned char *src, int len)
{
	unsigned short *pDst;

	unsigned short unicode;

	unsigned char ch1, ch2;

	int dst_len, char_len, n;

	if (NULL == dst)
	{
		return -1;
	}

	pDst = (unsigned short *) dst;

	if ((NULL == src) || (len <= 0))
	{
		*pDst = 0;
		return 0;
	}

	dst_len = 0;

	while (len > 0)
	{
		ch1 = *src;

		if (ch1 == 0)
		{
			//ch1为结束字符
			*pDst = 0;
			return dst_len << 1;
		}
		else if (ch1 & 0x80)
		{
			//统计utf8字符长度
			n = 1;
			while (ch1 & (0x80 >> n))
			{
				++n;
			}

			if ((n > len) || (n == 1))
			{
				//当前字符错误，用缺省字符代替
				*pDst++ = DEFAULT_CHAR;
				++dst_len;

				++src;
				--len;
				continue;
			}

			char_len = n;
			unicode = ch1;
			unicode &= (1 << (8 - n)) - 1;
			while (--n > 0)
			{
				//读取下一个字符
				ch2 = *(src + char_len - n);

				if ((!(ch2 & 0x80)) || (ch2 & 0x40))
				{
					//ch2不是10开始 
					break;
				}

				unicode = (unicode << 6) | (ch2 & 0x3F);
			}

			if (n == 0)
			{
				*pDst++ = unicode;
				++dst_len;

				src += char_len;
				len -= char_len;
			}
			else
			{
				//非正常转换完毕，则当前字符错误，用缺省字符代替
				*pDst++ = DEFAULT_CHAR;
				++dst_len;

				++src;
				--len;
			}
		}
		else
		{
			//ascii 码字符
			*pDst++ = ch1;
			++dst_len;

			++src;
			--len;
		}
	}

	*pDst = 0;
	return dst_len << 1;
}
