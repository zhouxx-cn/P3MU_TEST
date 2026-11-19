#ifndef __CBBACKUP_H_
#define __CBBACKUP_H_

/**************************************************************************
 *                    备份回调的定义                                      *
 **************************************************************************/
 //!\brief 备份时系统调用的回调函数
 //!\param nState 备份装态 
 //! 1表示正在备份
 //! 2表示备份出错退出
 //! 3表示手动退出备份
 //! 4表示备份完成
 //!\param nBackupSize 已备份的数据大小 KB
 //!\param FrameTime 备份时间
 //!\return 0表示成功
typedef int (*HY_BACKUP_CALLBACK) (int nState, unsigned long long dwBackupSize, time_t FrameTime);


#endif
