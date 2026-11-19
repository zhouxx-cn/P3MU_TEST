
/*!
*\file   CDiskManager.h 
*\brief  磁盘管理器的声明
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/


#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CDiskManager_49A2676A00C7_INCLUDED
#define _INC_CDiskManager_49A2676A00C7_INCLUDED

#include <pthread.h>
#include "CBAlert.h"
#include "CBDisk.h"
#include "tagActionInfo.h"
#include "tagAlertInfo.h"
#include <semaphore.h>

/**************************************************************************
*                            声明                                         *
 **************************************************************************/
extern const int DISKMANGER_MAX_DISK_NUM;

struct tagDiskInfo;

struct tagPartition;

#ifndef HotplugInfo
typedef struct _HOTPLUGINFO HotplugInfo;
#endif
class CAlarm;

class CHardDisk;

class CSnapshot;


//!\brief  1、提供磁盘的操作方法。2、提供分区的操作方法
//##ModelId=49A2676A00C7
class CDiskManager
{
  public:

	//!\brief 设置硬盘错误回调。全部硬盘出现错误时，struct tagAlertInfo保留字为1,部分硬盘错误时为0.
	//!\param int Type 硬盘错误类型
	//!\param CBAlert callabck 回调指针
	//!\return 成功为0，失败为错误码
	int SetDiskErrorCallBack(int AlarmType, CBAlert callabck);


	//!\brief 设置硬盘被卸载或者插入的回调函数
	//!\param callabck 回调指针
	//!\return 成功为0，失败为错误码
	int SetDiskStatusCallBack(void (*CallBack) (char *Dev, bool bAdd));

    /*@brief GetDiskStatus 获取硬盘当前的状态
     *@param int nIndex 硬盘索引号
     *@return 见下表
     eZWDiskStatus_Idle = 0,
     eZWDiskStatus_Writting = 1,
     eZWDiskStatus_Reading,
     eZWDiskStatus_Exception,
     eZWDiskStatus_Error,
     eZWDiskStatus_Readonly,
     eZWDiskStatus_Formatting = 6,
     eZWDiskStatus_Warn,
     eZWDiskStatus_ReadWrite = 8,
     eZWDiskStatus_NoDisk = 9, // 无磁盘
     eZWDiskStatus_ExpNeedFormat = 10, // 磁盘异常需要格式化
     eZWDiskStatus_ExpNeedChangeDisk = 11, // 磁盘异常需要换卡
     eZWDiskStatus_FormatFailNeedReboot = 12 // 磁盘格式化失败需要重启
     */
	int GetDiskStatus(const unsigned int nIndex);
	
	/*@ brief 获取硬盘当前格式化的状态
	 *@ param int nIndex 硬盘索引号
     *@ return  格式化状态:
     *@                  0: 未格式化;
     *@                  1: 格式化中
     *@                  2: 格式化成功
     *@                  3: 格式化失败
     *@                  4: 卸载失败
     *@                  5: 设备忙
     *@                  6: 无SD卡
     */
    int GetDiskFormatStatus(const unsigned int nIndex);

    //!\brief 获取当前硬盘写的状态
    //!\param int nIndex 硬盘索引号
    //!\return  当前硬盘写状态: 0.SD卡空闲, 1.SD卡正在写, 2.SD卡写完成, 3.SD卡写失败
    int GetDiskWriteStatus(const unsigned int nIndex);

   	//!\brief 开启/禁止某个报警联动动作
	//!\praram int AlarmType 报警类型。VIDEO_LOST 
	//!\praram int Action    !      ACTION_SCREEN 屏幕显示
	//  !      ACTION_BEEP 声音提示
	//  !      ACTION_ALARMOUT 报警输出
	//  !      ACTION_SENDTOSERVER 上传报警服务器
	//  !      ACTION_EMAIL 联动Email
	//  !      ACTION_SMS 联动短信
	//!\bool bEnahle true为开启，false为禁止
	//!\return 成功为0，失败为错误码
	//##ModelId=49BDBA5C02A0
	int EnableAction(int AlarmType, tagActionType Type, bool bEnahle);

	//!\brief 判断某个报警联动动作是否开启
	//!\praram int AlarmType 报警类型。
	//!\praram int Action    !      ACTION_SCREEN 屏幕显示
	//  !      ACTION_BEEP 声音提示
	//  !      ACTION_ALARMOUT 报警输出
	//  !      ACTION_SENDTOSERVER 上传报警服务器
	//  !      ACTION_EMAIL 联动Email
	//  !      ACTION_SMS 联动短信
	//!\return false 为禁止联动 true 为开启联动
	//##ModelId=49BDBA5C02A0
	bool IsEnableAction(int AlarmType, tagActionType Type);

	//!\brief 设置某个联动动作涉及的通道。无通道概念的不需要设置
	//!\param int nCh 通道
	//!\praram int AlarmType 报警类型
	//!\praram tagActionType Action   动作类型
	//!\praram int AlarmCh 联动动作涉及到通道。
	//!\praram bool bEnable 启用/不启用该通道的配置
	//!\praram int dwReserved 保留字。对于预置点、轨迹、巡航线路输入上述编号
	//!\return  成功为0，失败为-1
	int SetActionChParam(int AlarmType, tagActionType Action, int AlarmCh, bool bEnable, int nReserved = 0);




	//!\brief 获取某个联动动作涉及的通道或者编号
	//!\praram int AlarmType 报警类型
	//!\praram tagActionType Action 动作类型
	//!\praram bool *szCh 通道数组，true为联动了该通道，false为没有联动该通道
	//!\praram int *szReserved 对应通道涉及到的子类型数据。目前仅相对于预置点编号等
	//!\return  成功为0，失败为-1
	int GetActionChParam(int AlarmType, tagActionType Action, bool * szCh, int *szReserved = NULL);

	//!\brief 当系统插入一个无法识别（没有保留分区）的硬盘时，提示用户
	//!\param 回调函数指针
	//!\return 成功为0，失败为错误码
	//##ModelId=49AF776B026D
	int SetNewDiskCallBack(NEWDISKCALLBACK callback);

	//!\brief 获取硬盘个数
	//!\param 无
	//!\return 成功为0，失败为-1
	//##ModelId=49AF776B026D
	int GetNum(int *Num);

	//!\brief 获取磁盘物理位置编号
	//!\param int nIndex 硬盘索引号
	//!\return 成功为0，失败见错误号
	//##ModelId=49AF7A2C03A7
	int GetBusNumber(const unsigned int nIndex, int *BusNo);

	//!\brief 判断硬盘是否为新硬盘
	//!\param int nIndex 硬盘索引号
	//!\return  成功为0，失败见错误号
	//##ModelId=49AF7AA3024A
	int IsNew(const unsigned int nIndex, bool * bNew);

	//!\brief 获取磁盘信息
	//!\param tagDiskInfo *Info 硬盘信息结构体指针  
	//!\param int nIndex 硬盘索引号
	//!\return  成功为0，失败为-1
	//##ModelId=49AF7AE60373
	int GetInfo(const unsigned int nIndex, tagDiskInfo * Info);

	//!\brief 获取对应磁盘对应分区的信息
	//!\param tagPartition *Info 硬盘信息结构体指针 
	//!\param int nPartitionIndex 分区在硬盘中的索引值
	//!\param int nDiskIndex 硬盘索引号
	//!\return  成功为0，失败为-1
	//##ModelId=49AF808500AB
	int GetPartitionInfo(const unsigned int nDiskIndex, const unsigned int nPartitionIndex, tagPartition * Info);

	//!\brief 格式化一个分区
	//!\param int nPartitionIndex 分区在硬盘中的索引值
	//!\param int nDiskIndex 硬盘索引号
	//!\return  成功为0，失败为-1
	//##ModelId=49AF80C40124
	int FormatPartition(const unsigned int nDiskIndex, const unsigned int nPartitionIndex);

	//!\brief 卸载该硬盘
	//!\param int nPartitionIndex 分区在硬盘中的索引值
	//!\param int nDiskIndex 硬盘的物理位置
	//!\return  成功为0，失败为-1
	//##ModelId=49AF7FD20035
	int UnMount(const unsigned int nIndex);


	//!\brief 挂载该分区
	//!\param int nPartitionIndex 分区在硬盘中的索引值
	//!\param int nDiskIndex 硬盘索引号 
	//!\param int nDiskIndex 分区索引号
	//!\return  成功为0，失败为-1
	//##ModelId=49AF7FD20035
	int Mount(const unsigned int nIndex, const unsigned int nPartitionIndex);

	//!\brief 创建一个分区
	//!\param int nDiskIndex 硬盘索引号
	//!\param unsigned long nSize 分区大小
	//!\return  成功为0，失败为-1
	//##ModelId=49B0CEB7009C
	int CreatePartition(const unsigned int nDiskIndex, const unsigned int nSize);

	//!\brief 创建一个分区
	//!\param int nDiskIndex 硬盘索引号
	//!\param int nPartitionIndex 分区在硬盘中的索引值
	//!\return  成功为0，失败为-1
	//##ModelId=49B0CF6001F4
	int DestroyPartition(const unsigned int nDiskIndex, const unsigned int nIndexPartition);
    //!\brief 暂停SD卡录像
    int PauseSDRecord();
    //!\brief 继续SD卡录像
    int ResumeSDRecord();

	//!\brief 初始化
	//!\return  成功返回0，失败为-1
	//##ModelId=49BF198B0175
	int Init();

	//!\brief 释放资源
	//!\return  成功返回0，失败为-1
	//##ModelId=49BF199202FC
	int UnInit();

	//!\brief 初始化硬盘
	//!\param int nBusNo 硬盘物理位置编号
	//!\return  成功为0，失败为-1
	//##ModelId=49AF7FD20035
	int InitDisk(const unsigned int nBusNo);


	//!\brief 获取实例指针
	//!\param void
	//!\return  成功返回指针，失败为NULL
	//##ModelId=49BDA652022B
	static CDiskManager *GetInstance();

	//!\brief 释放实例指针
	//!\param CDiskGroupManger* handle 实例指针
	//!\return  成功返回0，失败为-1
	//##ModelId=49BDA664024F
	static int ReleaseInstance();


  private:
	//!\brief 卸载该分区
	//!\param int nPartitionIndex 分区在硬盘中的索引值
	//!\param int nDiskIndex 硬盘索引号 
	//!\param int nDiskIndex 分区索引号
	//!\return  成功为0，失败为-1
	//##ModelId=49AF7FD20035
	int UnMount(const unsigned int nIndex, const unsigned int nPartitionIndex);

	bool IsBusy(int nDiskIndex);

	//!\brief 读取配置参数
	//!\return  成功返回0，失败为-1
	int ReadCfg();

	//!\brief 更新disk的信息
	//!\praram 无 
	//!\return  成功为0，失败为-1
	int Update();

	//!\brief 检测硬盘
	//!\praram 无 
	//!\return  成功为0，失败为-1
	int Detect();

	//!\brief 初始化硬盘
	//!\praram 无 
	//!\return  成功为0，失败为-1
	int InitDisk(char *dev);

	//!\brief 向上报警
	//!\praram int DiskErrorType 硬盘的错误类型
	//!\return  成功为0，失败为-1
	int PushAlarm(void *org);

	CAlarm *ADiskError;

	//新硬盘回调函数
	NEWDISKCALLBACK DiskCallBack;

	//互斥量
	pthread_mutex_t mutex;

	//硬盘检测互斥量
	pthread_mutex_t diskmutex;

	int UpdateDiskData(int bAdd);

	//!\brief 获取异常硬盘的个数
	//!\praram 无 
	//!\return  异常硬盘的个数
	int GetErrorDiskNum(int Disks);

	//硬盘被卸载或者插入的回调函数
	DISKSTATUSCALLBACK DiskStatusCallBack;

	friend void *STR_NewDisk(HotplugInfo hpInfo);
	friend void *STR_DelDisk(HotplugInfo hpInfo);
	friend void Sata_AddCallBack(HotplugInfo hpInfo);

	friend void Sata_RemoveCallBack(HotplugInfo hpInfo);

	friend class CDPManager;

	friend class CSnapShot;

#ifdef __JFS_STORAGE__
	friend class CRecordFile;
#endif

	static void *work_thread(void *org);

	static sem_t cond;

	static CDiskManager *theCDiskManager;

	int bWorkTrheadRun;

	int StartWorkThread();

	unsigned int nDiskNum;

	//##ModelId=49AF6BD70199
	class CUsbDisk **hUsbDisks;

	//##ModelId=49BA0F5B0097
	class CHardDisk *theCHardDisk;
#ifndef TEST_MEM_INFO
	//##ModelId=49BE1F170336

	  CDiskManager();

	//##ModelId=49BE1F1802F1
	 ~CDiskManager();
#else
	//##ModelId=49BE1F170336
public:
	  CDiskManager();

	//##ModelId=49BE1F1802F1
	 ~CDiskManager();
#endif

};

#endif							/* _INC_CDiskManager_49A2676A00C7_INCLUDED */
