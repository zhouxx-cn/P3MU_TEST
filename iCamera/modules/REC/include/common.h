#ifndef	_COMMEN_H_
#define	_COMMEN_H_

#include <errno.h>

#define PRTCOLOR
#define MODENAME   "MEMRECORD"

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define MAX_CHANNEL_CNT 16

#define SDK_INIT_MAGIC  0x13145200

#define	HIEASY_FREE(a)\
{\
    if ((a) != NULL)\
    {\
        free((a));\
        (a) = NULL;\
    }\
}

#define	HIEASY_FCLOSE(a)\
{\
    if ((a) != NULL)\
    {\
        fclose((a));\
        (a) = NULL;\
    }\
}

#define	HIEASY_CLOSE(a)\
{\
    if ((a) > 0)\
    {\
        close((a));\
        (a) = 0;\
    }\
}

#define COUNT_REAL_INTERVAL(a, b, c)\
{\
    if ((b) >= (c))\
        (a) = (b) - (c);\
    else\
        (a) = (c) - (b);\
    if ((a) >= 0x80000000)\
        (a) = 0xFFFFFFFF - (a);\
}

#define MAX_FRMAESIZE   (512 * 1024)	                    //最大的一帧Size

#define		I_FRAME						(0x01)
#define		P_FRAME						(0x02)
#define		QCIFIFrames                 (0x0010)
#define		QCIFPFrames                 (0x0040)

#define		AUDIO_FRAME					(0x08)
#define		HIGHEASY_ADPCM_IMA			(6)

#define FILE_HEAD_LENTH     (sizeof(MPEG4FILEHEADER))	    //文件头的长度
#define FRAME_HEAD_LENTH    (sizeof(H264DATAPACKETHEADER))	//帧头的长度
#define STARTCODE                       0xB7010000
#define Index_End_Flag                  0x12345678
#define I_Frame_Index_Num               1800
#include <pthread.h>

/* SDK接口资源 */
typedef struct
{
	int m_bSDKInit;				//SDK是否初始化
	int m_bSDKInitLock;			//SDK入口与出口函数互斥
} SDK_ResInfo_t;

/* 文件索引信息 */
typedef struct tagIndex_File
{
	int I_Frame_Num;			//当前文件中第几个I帧
	int I_FRAME_Pos;			//I帧在文件中的偏移
} Index_File, *PIndex_File;

/* 文件头信息 */
typedef struct tagMP4NewFileHeader
{
    unsigned int FileStartCode;		        //固定为0xB6010000
    unsigned int FileID;					//固定为0x34363248,即H264
    unsigned int FileHeadLen;				//媒体文件头的长度(包含本身4Byte)
    unsigned char Version;					//媒体文件头的版本信息.初始版本为1
    /*
    2Bytes, 表示媒体数据包及媒体索引信息
    MSB B15 		  0:没有索引 1:有索引
    	B1			  0:no Private Data 1:with Private Data
    	B0			  0: Video Only 1:Video&Audio
    */
    short int	DataFlag;		  
    short int	dwImageWidth;			    //视频媒体数据的图像宽度
    short int	dwImageHight; 		        //视频媒体数据的图像高度
} __attribute__ ((packed)) MPEG4FILEHEADER;

/* 帧头信息 */
typedef struct tagH264DataPacketHeader
{
    unsigned int        DataPacketStartCode;    //固定为0xB7010000
    unsigned int        FrameNo;				//媒体数据包的序号(音视频独立编号)
    unsigned char       FrameType;
    unsigned char       byLoadType;				//媒体数据包的信息,具体含义依赖于帧类型
    unsigned int        ShowTime;				//表示媒体数据包相对于上一帧的偏差时间(频率90Khz)
    unsigned long       u32RealTimeSec;
    unsigned long       u32RealTimeUSec;
    unsigned short      uReserved;
    unsigned int        nOverloadLen;			//表示媒体数据包的数据长度
}__attribute__((packed)) H264DATAPACKETHEADER, *PH264DATAPACKETHEADER;

/* 文件备份句柄信息 */
typedef struct CFileRaw
{
	int m_nPort;				            //通道编号
	pthread_mutex_t m_Lock;

	int m_bFileOK;				            //文件是否被正常打开
	int m_bFileEnd;				            //文件读取完毕
	FILE *m_File;				            //文件指针
	int m_dwFileSize;			            //文件大小
	MPEG4FILEHEADER m_FileHead;	            //文件头
	H264DATAPACKETHEADER m_FrameHead;	    //帧头
	char *pFrameData;			            //帧数据空间

	long nIFrameNumber;			            //I帧个数
	int *IFrameIndex;
	int m_PreIFrameNo;

	unsigned int m_FirstIFrameNumStamp;	    //文件第一个I帧帧号戳
	unsigned int m_FirstIFrameTimeStamp;	//文件第一个I帧时间戳
	int m_nFirstIFramePos;		            //文件第一个I帧的位置

	int m_bValidIFrame;			            //已经有合法I帧做基准
	int m_bDiscardErrFrame;		            //检测到文件异常后丢帧直到新的I帧
	unsigned int m_LastValidIFrameNum;	    //最后一个合法I帧的帧号
	unsigned int m_LastValidIFrameTime;	    //最后一个合法I帧的时间戳

	/* 为了文件无缝连接修改 */
	unsigned int m_CurVideoTimeStamp;	    //所有备份文件的视频累积时间戳
	unsigned int m_CurAudioTimeStamp;	    //所有备份文件的视频累积时间戳
	unsigned int m_LastVideoTimeStamp;	    //最后一个视频时间戳
	unsigned int m_LastAudioTimeStamp;	    //最后一个视频时间戳

	unsigned int m_CurVideoNumStamp;	//所有备份文件的视频累积时间戳
	unsigned int m_CurAudioNumStamp;	//所有备份文件的视频累积时间戳
	unsigned int m_LastVideoNumStamp;	//最后一个视频时间戳
	unsigned int m_LastAudioNumStamp;	//最后一个视频时间戳
} CFileRaw_t;

#endif

