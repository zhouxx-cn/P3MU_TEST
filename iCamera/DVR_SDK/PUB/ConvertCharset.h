
/*!
 *\file		ConvertCharset.h
 *\brief		PUB模块，字符集转换函数
 *\author		林云
 *\date		2009/12/22
 */

#ifndef _INC_CONVERT_CHARSET_H_
#define _INC_CONVERT_CHARSET_H_

//!\brief       unicode字符串转utf8字符串
//!\param   dst 目的字符串(utf8编码)
//!\param   src 源字符串(unicode编码)
//!\param   len 源字符串长度(单位:字节)
//!\return  转换后字符串的长度(单位:字节)
int uc16_conv_to_utf8(unsigned char *dst, const unsigned char *src, int len);

//!\brief       hecs1字符串转unicode字符串
//!\param   dst 目的字符串(unicode编码)
//!\param   src 源字符串(hecs1编码)
//!\param   len 源字符串长度(单位:字节)
//!\return  转换后字符串的长度(单位:字节)
int hecs1_conv_to_uc16(unsigned char *dst, const unsigned char *src, int len);

//!\brief       utf8字符串转unicode字符串
//!\param   dst 目的字符串(unicode编码)
//!\param   src 源字符串(utf8编码)
//!\param   len 源字符串长度(单位:字节)
//!\return  转换后字符串的长度(单位:字节)
int utf8_conv_to_uc16(unsigned char *dst, const unsigned char *src, int len);

#endif							/* #ifndef _INC_CONVERT_CHARSET_H_ */
