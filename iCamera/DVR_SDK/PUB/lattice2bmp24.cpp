
/*!
*\file		lattice2bmp24.cpp
*\brief		PUB模块，时间转换函数
*\author		林云
*\date		2009/09/15
*/

#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include "global_macro.h"
// #include "sys_banned.h"
#include "global_gather.h"
#if 0

#define PUB_DBG(x...)												\
do																\
{																\
    printf("[%s-%s:%d]: ", __FILE__, __FUNCTION__, __LINE__);				\
    printf(x);														\
}																\
while (0)

#else

#define PUB_DBG(x...)	((void)0)

#endif


typedef struct tag_rgb24_pixel
{
	unsigned char b;
	unsigned char g;
	unsigned char r;
}
RGB_PIXEL;


typedef struct tag_BITMAPFILEHEADER
{
	unsigned short bfType;
	unsigned long bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned long bfOffBits;
} _BITMAPFILEHEADER;


typedef struct tag_WINBMPINFOHEADER	/* size: 40 */
{
	unsigned long biSize;
	unsigned long biWidth;
	unsigned long biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned long biCompression;
	unsigned long biSizeImage;
	unsigned long biXPelsPerMeter;
	unsigned long biYPelsPerMeter;
	unsigned long biClrUsed;
	unsigned long biClrImportant;
}
_WINBMPINFOHEADER;


#define BI_RGB					0
#define SIZEOF_BMFH		14
#define SIZEOF_BMIH		40

#define GetBitN(pSrc, iIndex) (0 != (pSrc[iIndex / 8] & (1 << (iIndex % 8))))


//!\brief       点阵保存为bmp文件，RGB24格式
//!\param   nWidth 点阵宽度(单位:像素)
//!\param   nHeight 点阵高度(单位:像素)
//!\param   pLattice 点阵数据缓冲区
//!\param   szFileName 保存的位图文件名
//!\param   iAddOutLine 是否加边
//!\return  0表示成功，其它为错误码
int lattice_saveas_bmp24(int nWidth, int nHeight, unsigned char *pLattice, const char *szFileName, int iAddOutLine)
{
	if (nWidth <= 0)
	{
		PUB_DBG("[ERROR]: nWidth = %d\n", nWidth);
		return -1;
	}

	if (nHeight <= 0)
	{
		PUB_DBG("[ERROR]: nHeight = %d\n", nHeight);
		return -2;
	}

	if (NULL == pLattice)
	{
		PUB_DBG("[ERROR]: NULL == pLattice\n");
		return -3;
	}

	if (NULL == szFileName)
	{
		PUB_DBG("[ERROR]: NULL == szFileName\n");
		return -4;
	}

	PUB_DBG("nWidth %d, nHeight %d, %s\n", nWidth, nHeight, szFileName);

	int nPitch = ((((nWidth * 3) + 3) >> 2) << 2);

	unsigned char *pDstHead = NULL;
	
	unsigned char *pDst = NULL;

	RGB_PIXEL tRgbBack;			// 背景色

	RGB_PIXEL tRgbFore;			// 前景色

	RGB_PIXEL tRgbOut;			// 边框色

	if (0 != iAddOutLine)
	{
		tRgbBack.r = 0xff;
		tRgbBack.g = 0xff;
		tRgbBack.b = 0xff;
	}
	else
	{
		tRgbBack.r = 0;
		tRgbBack.g = 0;
		tRgbBack.b = 0;
	}
	tRgbFore.r = 0xF0;
	tRgbFore.g = 0xF0;
	tRgbFore.b = 0xF0;

	tRgbOut.r = 0x0;
	tRgbOut.g = 0x0;
	tRgbOut.b = 0x0;

	// 申请内存

	pDstHead = (unsigned char *) malloc(nPitch * nHeight);
	if (NULL == pDstHead)
	{
		PUB_DBG("[ERROR]: malloc %d\n", (nPitch * nHeight));
		return -5;
	}


	//写bmp 头
	_BITMAPFILEHEADER hdr;

	_WINBMPINFOHEADER hbi;

	memset(&hdr, 0, sizeof (_BITMAPFILEHEADER));
	memset(&hbi, 0, sizeof (_WINBMPINFOHEADER));
	hdr.bfType = iDvrArchSwapLE16(0x4d42);	//BM
	hdr.bfSize = iDvrArchSwapLE32(SIZEOF_BMFH + SIZEOF_BMIH + nPitch * nHeight);
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;
	hdr.bfOffBits = iDvrArchSwapLE32(SIZEOF_BMIH + SIZEOF_BMFH);

	hbi.biSize = iDvrArchSwapLE32(SIZEOF_BMIH);
	hbi.biWidth = iDvrArchSwapLE32(nWidth);
	hbi.biHeight = iDvrArchSwapLE32(nHeight);
	hbi.biPlanes = iDvrArchSwapLE16(1);
	hbi.biBitCount = iDvrArchSwapLE16(24);
	hbi.biCompression = iDvrArchSwapLE32(BI_RGB);
	hbi.biSizeImage = iDvrArchSwapLE32(nPitch * nHeight);
	hbi.biXPelsPerMeter = 0;
	hbi.biYPelsPerMeter = 0;
	hbi.biClrUsed = 0;
	hbi.biClrImportant = 0;

	//写bmp数据
	//由RGB16转为RGB24，图像需要翻转
	int x = 0;

	int y = 0;

	int iLatticePitch = ((nWidth + 7) / 8 * 8);

	for (y = 1; y <= nHeight; y++)
	{
		pDst = pDstHead + (nHeight - y) * nPitch;	//目的数据  y行首地址

		for (x = 0; x < nWidth; x++)
		{
			if (0 == iAddOutLine)
			{
				if (0 == GetBitN(pLattice, ((iLatticePitch) * (y - 1) + x)))	// 无点
				{
					*pDst++ = tRgbBack.b;
					*pDst++ = tRgbBack.g;
					*pDst++ = tRgbBack.r;
				}
				else
				{
					*pDst++ = tRgbFore.b;
					*pDst++ = tRgbFore.g;
					*pDst++ = tRgbFore.r;
				}
			}
			else
			{
				if (0 == GetBitN(pLattice, ((iLatticePitch) * (y - 1) + x)))	// 无点
				{
					// 左 ;上; 右;下  有字，需要加边
					if ((x > 0 && 1 == GetBitN(pLattice, ((iLatticePitch) * (y - 1) + x - 1)))
						|| (y > 1 && 1 == GetBitN(pLattice, ((iLatticePitch) * (y - 2) + x)))
						|| ((x < (nWidth - 1)) && 1 == GetBitN(pLattice, ((iLatticePitch) * (y - 1) + x + 1)))
						|| (y < nHeight && 1 == GetBitN(pLattice, ((iLatticePitch) * (y) + x))))
					{
						*pDst++ = tRgbOut.b;
						*pDst++ = tRgbOut.g;
						*pDst++ = tRgbOut.r;
					}
					else
					{
						*pDst++ = tRgbBack.b;
						*pDst++ = tRgbBack.g;
						*pDst++ = tRgbBack.r;
					}

				}
				else
				{
					*pDst++ = tRgbFore.b;
					*pDst++ = tRgbFore.g;
					*pDst++ = tRgbFore.r;
				}

			}
		}
	}


	//写入文件
	FILE *fp = NULL;
/*
	//my_test
pDstHead_2 = (unsigned char *) malloc(nPitch * nHeight);
	if (NULL == pDstHead_2)
	{
		PUB_DBG("[ERROR]: malloc %d\n", (nPitch * nHeight));
		return -5;
	}
memcpy(pDstHead_2,pDstHead,nPitch * nHeight);

int m=0,n=0,tmp=0,flag=0;
unsigned char tmp_ch;
int  len=nPitch * nHeight;
printf("\n===111111111========len=%d==============================\n",len);
	for (m=0;m<len;m++)
	{
	printf("0x%02x,",pDstHead_2[m]);

	if (m%10 ==0)
		printf("\n");
	}
printf("\n===111111111========================================\n");
	
*/	
	fp = fopen(szFileName, "wb");
	if (NULL != fp)
	{
		fwrite(&hdr.bfType, 2, 1, fp);
		fwrite(&hdr.bfSize, 4, 1, fp);
		fwrite(&hdr.bfReserved1, 2, 1, fp);
		fwrite(&hdr.bfReserved1, 2, 1, fp);
		fwrite(&hdr.bfOffBits, 4, 1, fp);

		fwrite(&hbi, SIZEOF_BMIH, 1, fp);

		fwrite(pDstHead, 1, nPitch * nHeight, fp);

		fclose(fp);
		PUB_DBG("save file : %s\n", szFileName);
	}
	else
	{
		PUB_DBG("[ERROR]: fopen %s\n", szFileName);
		free(pDstHead);
		return -6;
	}
	//释放内存
	free(pDstHead);

	return 0;
}

//!\brief   点阵保存为bmp文件，RGB24格式
//!\param   nWidth 点阵宽度(单位:像素)
//!\param   nHeight 点阵高度(单位:像素)
//!\param   pLattice 点阵数据缓冲区
//!\param   pRGB 输出的rgb24数据
//!\return  0表示成功，其它为错误码
int lattice_to_rgb24(int nWidth, int nHeight, unsigned char *pLattice, unsigned char *pRGB)
{
	if (nWidth <= 0)
	{
		PUB_DBG("[ERROR]: nWidth = %d\n", nWidth);
		return -1;
	}

	if (nHeight <= 0)
	{
		PUB_DBG("[ERROR]: nHeight = %d\n", nHeight);
		return -2;
	}

	if (NULL == pLattice)
	{
		PUB_DBG("[ERROR]: NULL == pLattice\n");
		return -3;
	}

	if (NULL == pRGB)
	{
		PUB_DBG("[ERROR]: NULL == pRGB\n");
		return -4;
	}

	PUB_DBG("nWidth %d, nHeight %d\n", nWidth, nHeight);

	return 0;
}
