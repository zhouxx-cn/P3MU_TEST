
/*!
 *\file		lattice2bmp24.h
 *\brief		PUB模块，时间转换函数
 *\author		林云
 *\date		2009/09/15
 */

#ifndef _INC_LATTICE_2_BMP24_H_
#define _INC_LATTICE_2_BMP24_H_

//!\brief       点阵保存为bmp文件，RGB24格式
//!\param   nWidth 点阵宽度(单位:像素)
//!\param   nHeight 点阵高度(单位:像素)
//!\param   pLattice 点阵数据缓冲区
//!\param   szFileName 保存的位图文件名
//!\param   iAddOutLine 是否加边
//!\return  0表示成功，其它为错误码
int lattice_saveas_bmp24(int nWidth, int nHeight, unsigned char *pLattice, const char *szFileName, int iAddOutLine);

//!\brief       点阵保存为bmp文件，RGB24格式
//!\param   nWidth 点阵宽度(单位:像素)
//!\param   nHeight 点阵高度(单位:像素)
//!\param   pLattice 点阵数据缓冲区
//!\param   pRGB 输出的rgb24数据
//!\return  0表示成功，其它为错误码
int lattice_to_rgb24(int nWidth, int nHeight, unsigned char *pLattice, unsigned char *pRGB);

#endif							/* #ifndef _INC_LATTICE_2_BMP24_H_ */
