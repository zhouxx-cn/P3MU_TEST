// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_COSD_49A2611B03AA_INCLUDED
#define _INC_COSD_49A2611B03AA_INCLUDED
#include "global_macro.h"
#include "CConfig.h"
#include "tagCharBitMap.h"
#ifdef DO_SATA
#include "hisdk_sata.h"
#else
#include "zwsdk.h"
#endif

#include "AVL_global.h"


typedef struct OSDV2Text_s
{
	int bEnable;										/*!< OSD是否启用 */
	int nX;												/*!< OSD位置x坐标 */
	int nY; 											/*!< OSD位置y坐标 */
	char  szToken[OSDV2_TEXT_TOKEN_LEN];      	/*!< OSD的文字描述 */
	unsigned short strText[OSDV2_TEXT_LENGTH]; /*!< OSD的文字内容,编码必须为unicode */
} OSDV2Text_t;

typedef struct OSDV2_TEXT_OVERLAY_S
{
	int wordLen;
	unsigned short word[OSDV2_TEXT_OVERLAY];
} OSDV2_TEXT_OVERLAY_T;

typedef unsigned short	uint16;
typedef unsigned char	uchar;

extern "C" void HKUniToStr(const uint16 *uni,uchar *str,int len);
extern "C" int gb2unicode(unsigned short int*unicode,char*gb,int len);
extern "C" void UniToStr(const uint16 *uni,uchar *str);
extern "C" int findStrInHZK16(unsigned short *value, char *buf);

#if 0
#ifdef DO_SATA
typedef enum
{
	Mode_YYYY_MM_DD = 0,
	Mode_MM_DD_YYYY = 1,
} OSDTimeMode_t;

typedef struct TimeOSDInfo_t
{
	int FontWidth;				//字符宽度
	int FontHeight;				//字符高度
	int Start_X;				//起始X坐标
	int Start_Y;				//起始Y坐标
	int bTranslant;				//是否透明
	int bHour24;				//是否24小时制
	OSDTimeMode_t DisMod;		//0,表示年月日, =1，表示月日年
	char *pFontPixinfo;			//15个关键字的点阵信息：数字0-9,字母"A"，"P"，"M"，"："，"-"
} TimeOSDInfo_t;

typedef struct TextOSDInfo_t
{
	int TextNum;				//文本字符个数
	int Start_X;				//起始X坐标
	int Start_Y;				//起始Y坐标
	char *pPixinfo;				//点阵信息(每个字符的宽、高各占一个字节)
} TextOSDInfo_t;
#endif
#endif
//点阵顺序：高位左
typedef struct _OSD_PIXEL_T_
{

	int m_iWidth;				//宽度
	int m_iHeight;				//高度
	int m_iBitNum;				//每点占位数

} OSD_PIXEL_T;

//处理OSD业务、有关的操作及配置
//##ModelId=49A2611B03AA
class COsd
{
  public:
	//!\brief 实例化
	//!\praram int chhandle 通道    
	//!\praram void* hHandle 编码通道句柄
	//!\return  成功为0，失败为-1
	//##ModelId=49A261250282
	COsd(int chhandle, void *hHandle);
	 ~COsd();

	//!\brief 初始化
	int Init();

	//!\brief 资源释放
	int UnInit();

	//!\brief 获取OSD显示模式
	//!\praram int* nShowType 
	//0为通道时间均不显示、1为显示通道，2、为显示时间、3为两者
	//均显示
	//!\return  成功为0，失败为-1
	//##ModelId=49A263BC029A
	int GetType(int *nShowType);

	//!\brief 获取时间或者字符的位置
	//!\praram int OSD_TYPE OSD类型 0-时间,1-文本
	//!\praram  int* x osdx位置
	//!\praram  int* y osdxy位置
	//!\return  成功为0，失败为-1
	//##ModelId=49B7675A0290
	int GetPosition(int OSD_TYPE, int *x, int *y);

	//!\brief 获取时间格式
	//!\praram int* nShowFortMat 
	//时间格式。根据底层的实际情况来确定可以设置哪些格式
	//!\return  成功为0，失败为-1
	//##ModelId=49B767A50356
	int GetTimeFormat(int *nShowFortMat);

	//!\brief 获取时间点阵宽高
	//!\praram int* pWidth,int *pHeight
	//时间格式。根据底层的实际情况来确定可以设置哪些格式
	//!\return  成功为0，失败为-1
	//##ModelId=49B767A50356
	int GetTimeWH(int *pWidth, int *pHeight);

	//!\brief 获取通道字符点阵
	//!\param char * CharBitMap BMP文件名
	//!\param int buffSize 缓冲大小
	//!\return  成功为0，失败为-1，当pCharBitMap == NULL 时返回实际大小
	//##ModelId=49B767F9028E
	int GetCharBitMap(char *pCharBitMap, int buffSize);

	//!\brief 设置通道字符点阵
	//!\praram char * CharBitMap BMP文件名
	//!\return  成功为0，失败为-1
	//##ModelId=49B768A80178
	int SetCharBitMap(char *pCharBitMap,unsigned char *byLattice ,int FontSize, int Width, int Height);

	int OSD_SetTextEx(char *OSDToken, unsigned short *strText, int CharSet);
	
	int OSDV2TextParsing(unsigned short *strText, unsigned short *textBuf, int *textCnt, int *totalWidth, int *maxLineWidth, int type);

	int OSDV1TextParsing(unsigned short *strText, int *maxLineWidth);

	//由于sdk支持多行OSD的接口，在这里将TextOSDInfo_t数据转成TextOSDV2Info_t index:多行文本OSD的下标
	int TextOSDToTextOSDV2(int index);
	
	int SetTextOSDV2(int index);
	
	int OSD_GetOSDV2TextByNum(int i, int *bEnable, char *OSDToken, unsigned short *strText, int *nX, int *nY);

	int SetTextOSD_Cprocustom(char *unicde);

	int OSD_SetOSDV2(int index, int bEnable, char *OSDToken, unsigned short *strText, int nX, int nY, int PassFlowEna);
	
	int OSD_GetTextExByToken(char *OSDToken, unsigned short *strText, int *CharSet);
	
	int OSD_GetTextExByNum(int i, char *OSDToken, int tokenSize, unsigned short *strText, int *CharSet);

	int RevertOSDBak();
	
	int SaveOSDBak();

	//!\brief 
	//设置OSD参数配置(需要实现通道别名+位置，时间格式+位置)
	//!\praram int nShowType OSD类型
	//!\return  成功为0，失败为-1
	//##ModelId=49A263BF0226
	int SetType(int nShowType);

	//!\brief 设置时间或者字符的位置
	//!\praram int OSD_TYPE OSD类型 0-时间,1-文本
	//!\return  成功为0，失败为-1
	//##ModelId=49B7686E0188
	int SetPosition(int OSD_TYPE, int x, int y);

	//!\brief 设置时间或者字符的位置
	//!\param int OSD_TYPE OSD类型 1-文本,2-时间,3-时间+文本
	//!\param int Time_x, int Time_y, int Text_x, int Text_y时间与文件的位置
	//!\return  成功为0，失败为-1
	int SetPositionAll(int Time_x, int Time_y, int Text_x, int Text_y, int OSD_TYPE = 3);

	//!\brief 设置时间格式
	//!\praram int nShowFortMat 
	//时间格式。根据底层的实际情况来确定可以设置哪些格式
	//四种模式,年放前还是后，12制还是24制
	//0-YYYYMMDD12,1-YYYYMMDD24,2-MMDDYYYY12,3-MMDDYYYY24
	//!\return  成功为0，失败为-1
	//##ModelId=49B768920389
	int SetTimeFormat(int nShowFortMat);

  	int setTextOsd();
  private:
	//通道号
	int m_iChannel;
	//编码通道句柄
	HANDLE m_hEnc;
	//显示类型
	int m_iShowType;
	//上一次的显示类型
	int m_iOldShowType;
	unsigned short m_strName[DVR_MAX_CHANNELNAME_LEN];

	//是否预留客流的叠加信息
	int m_iOsdFlowEn;

	//当前文本显示位置
	int m_iTextX;
	int m_iTextY;
	//文本显示位置,客户端设置值，主要是为了ONVIF Device Test Tool通过
	int m_iTextNetCfgX;
	int m_iTextNetCfgY;

	//文本宽高
	int m_iTextW;
	int m_iTextH;

	//时间点阵
	TimeOSDInfo_t m_tTimeOsd;
	//文本点阵结构
	TextOSDInfo_t m_tTextOsd;
	//名称点阵结构
	TextOSDInfo_t m_tNameOsd;

	//扩展的Text
	TextOSDInfoEx_t m_tTextOsdEx;
	unsigned short m_strNameEx[OSD_TEXT_EX_MAX_EXTENT_OSD_TEXT_LENGTH];//存放纯OSD内容
	unsigned short m_strNameRaw[OSD_TEXT_EX_MAX_EXTENT_OSD_TEXT_LENGTH];//存放用户设置内容
	unsigned int 	m_iWhichValid;//0:老Text（支持20个的）有效，1：新扩展的Text有效

	//OSD V2
	// 传输给SDK的数据
	TextOSDV2Info_t m_tTextOsdV2Info[OSDV2_TEXT_CNT];

	// 存放用户设置的内容
	OSDV2Text_t m_tTextOsdV2[OSDV2_TEXT_CNT];

	//当前时间显示位置
	int m_iTimeX;
	int m_iTimeY;
	//时间显示位置,客户端设置值，主要是为了ONVIF Device Test Tool通过
	int m_iTimeNetCfgX;
	int m_iTimeNetCfgY;

	//时间宽高
	int m_iTimeW;
	int m_iTimeH;

	//时间格式
	int m_iTimeType;
	
	//点阵文件路径
	const char *FileNameOsdPixel;

	//字符点阵缓冲
	char *m_pixelBuf;
	//字符点阵缓冲长度
	unsigned int m_iPixelBufLen;
	//配置文件
	CConfig *pCConfig;
	//点阵文件信息
	OSD_PIXEL_T m_osdPixel;
	
  private:
	int LoadParam();

	int WriteTextPixeltoFile(char *pbuf);

	//!\brief 检查时间与文件是否重叠
	//!\return 0不重叠,1重叠
	int CheckRect();

	//!\brief 设置通道字符点阵
	//!\praram char * CharBitMap 位图，包括文件头，信息头，数据等
	//!\return  成功为0，失败为-1

	int SetCharPixel(char *pCharBitMap);

	int SetOsd(TextOSDInfo_t *textoi, TimeOSDInfo_t * timeoi,PointOSDInfo_t *point_info, TextOSDInfoEx_t *textEx, TextOSDV2Info_t *OSDV2Info, int index);

};

#endif							/* _INC_COSD_49A2611B03AA_INCLUDED */
