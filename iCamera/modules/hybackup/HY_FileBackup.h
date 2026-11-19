#ifndef	_FILEBACKUP_H_
#define	_FILEBACKUP_H_

#ifdef __cplusplus
extern "C"
{
#endif
//---------------------------------------- 文件备份控制函数 ----------------------------------------------
	int MP4Sys_DecOpenBackUpFile(int nPort, char *szFileName, char *IndexFileName, int bFirstFile);	//时间戳需要重置
	int MP4Sys_DecCloseBackUpFile(int nPort);
	int MP4Sys_DecSetBackUpFileTime(int nPort, unsigned int TimeSec);
	int MP4Sys_DecGetFileHead(int nPort, char **pFileHead, int *FileHeadLenth);
	int MP4Sys_DecGetFrameSize(char *sFileName, unsigned int StartSec, unsigned int StopSec, int *pBackSize);
	int MP4Sys_DecReadOneFrame(int nPort,	//通道编号
							   int nReadFrameType,	//读取帧类型，0为所有帧，1为关键帧
							   int nReadDirection,	//读取方向，0为正向，1为反向
							   int bThrowAudio,	//是否扔掉音频，0读取音频，1丢弃音频
							   char **pFrameHead,	//帧头地址
							   int *pnFrameHeadLenth,	//帧头长度
							   int *pnFrameType,	//帧类型
							   char **pFrameData,	//帧数据地址
							   int *pnFrameDataLenth,	//帧数据长度
							   unsigned int *FrameTime);	//该帧相对于文件第一个I帧的时间(视频帧有效，单位为毫秒)
	int MP4Sys_CreateFileIndex(char *FrameHead, int FilePos, int flag, int *Index_Len);
	int MP4Sys_RebuildFileIndex(int nPort, char *szFileName, int *R_Index_Len);
#ifdef __cplusplus
}
#endif

#endif
