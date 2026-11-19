
/*!
*\file   CDiskGroupManager.h
*\brief  提供操作盘组的接口函数
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CDiskGroupManager_49A5F8B70324_INCLUDED
#define _INC_CDiskGroupManager_49A5F8B70324_INCLUDED

#include <pthread.h>
#include "HANDLE.h"
#include "CBAlert.h"
#include "tagActionInfo.h"
#include "tagAlertInfo.h"
#include <semaphore.h>

class CAlarm;

class CDiskGroup;

struct tagDiskGroupInfo;

#ifndef T_BADBLOCKINFO
typedef struct _BAD_BLOCK_INFO T_BADBLOCKINFO;
#endif

//!\brief 提供操作盘组的接口函数
//##ModelId=49A5F8B70324
class CDiskGroupManager
{
  public:
	//!\brief 获取盘组工作模式
	//!\param 无
	//!\return  0为一般模式，1为高级模式
	int GetWorkMode();

	//!\brief 设置盘组工作模式
	//!\param int nWorkMode 0为一般模式，1为高级模式
	//!\return  成功为0，失败为-1
	int SetWorkMode(int nWorkMode);

	//!\brief 创建硬盘分组，主要是注册该新硬盘分组的基本信息。
	//!\param int nDiskGroupType 
	//盘组类型。0为普通盘组，1为报警盘组，2为冗余盘组，3为备份
	//!\param int nCycleMode 盘组满时是覆盖还是停止。0为停止，1为覆盖
	//!\return  成功返回0，失败为-1s
	int Create(const unsigned int nDiskGroupType, const unsigned int nCycleMode = 1);

	//!\brief 销毁硬盘分组。
	//!\param int nDiskGroupType 盘组类型。0为普通盘组，1为报警盘组，2为冗余盘组，3为备份盘组
	//!\return  成功返回0，失败为-1
	//##ModelId=49AF739B0274
	int Destroy(const unsigned int nDiskGroupType);

	//!\brief 获取磁盘组信息
	//!\param int nDiskGroupType 
	//盘组类型。0为普通盘组，1为报警盘组，2为冗余盘组，3为备份
	//盘组
	//!\param tagDiskGroupInfo* Info 磁盘组信息结构体指针
	//!\return  成功返回0，失败为-1
	//##ModelId=49AF74090308
	int GetInfo(const unsigned int nDiskGroupType, tagDiskGroupInfo * Info);

	//!\brief 获取指定分区所含有的录像数据的开始时间和结束时间
	//!\param nDiskGroupType 分组类型
	//!\param pcDevPart 分区节点号
	//!\param tBeginTime 存放开始时间
	//!\param tEndTime 存放结束时间
	//!\return 成功返回0，失败返回小于0的错误码。
	int GetDiskPartDataInfo(const unsigned int nDiskGroupType, char *pcDevPart, time_t * tBeginTime, time_t * tEndTime);

	//!\brief 获取指定分区所含有的录像数据的开始时间和结束时间
	//!\param pcDevPart 分区节点号
	//!\param tBeginTime 存放开始时间
	//!\param tEndTime 存放结束时间
	//!\return 成功返回0，失败返回小于0的错误码。
	int GetDiskPartDataInfoEx(char *pcDevPart, time_t * tBeginTime, time_t * tEndTime);

	//!\brief 绑定某通道数据至该盘组
	//!\param int nDiskGroupType 
	//盘组类型。0为普通盘组，1为报警盘组，2为冗余盘组，3为备份
	//盘组
	//!\param int nCh 通道
	//!\param int RectType 录像类型
	//!\return  成功返回0，失败为-1
	//##ModelId=49AF745D0377
	int BindDataCH(const unsigned int nDiskGroupType, const unsigned int nCh, const unsigned int RectType);

	//!\brief 解除绑定某通道数据至该盘组
	//!\param int nDiskGroupType 盘组类型。0为普通盘组，1为报警盘组，2为冗余盘组，3为备份
	//!\param int nCh 通道
	//!\param int RectType 录像类型
	//!\return  成功返回0，失败为-1
	//##ModelId=49AF745D0377
	int UnBindDataCH(const unsigned int nDiskGroupType, const unsigned int nCh, const unsigned int RectType);

	//!\brief 设置盘组满回调函数
	//!\param int nType 盘组满返回的类型
	//!\param CBAlert CallBack 回调函数句柄
	//!\return  成功返回0，失败为-1
	int SetDiskGroupFullCallBack(int nType, CBAlert CallBack);


	//!\brief 设置盘组异常回调函数
	//!\param int nType 盘组满返回的类型
	//!\param CBAlert CallBack 回调函数句柄
	//!\return  成功返回0，失败为-1
	int SetDiskGroupErrorCallBack(int nType, CBAlert CallBack);

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
	int GetActionChParam(int AlarmType, tagActionType Action, bool * szCh, int *szReserved);


	//!\brief 添加分区至该盘组
	//!\param int nDiskGroupType 盘组类型。0为普通盘组，1为报警盘组，2为冗余盘组，3为备份
	//!\param int nDiskIndex    磁盘索引
	//!\param int nIndexPartition 分区索引
	//!\return  成功返回0，失败为-1
	//##ModelId=49AF749B0236
	int AddPartition(const unsigned int nDiskGroupType, const char *PartitionName);

	//!\brief 删除分区至该盘组
	//!\param int nDiskGroupType 
	//盘组类型。0为普通盘组，1为报警盘组，2为冗余盘组，3为备份
	//盘组
	//!\param int nDiskIndex    磁盘索引
	//!\param int nIndexPartition 分区索引
	//!\return  成功返回0，失败为-1
	//##ModelId=49AF74ED0125
	int DelPartition(const unsigned int nDiskGroupType, const char *PartitionName);


	//!\param int nDataKeepTime 数据保持时间
	//盘组属性。包好读写属性及数据的保持时间。
	//!\return  成功返回0，失败返回小于0的错误码。
	//##ModelId=49AF82750112
	int Set_DataKeepTime(const unsigned int nDiskGroupType, const unsigned int nDataKeepTime);

	//!\brief 获取指定盘组的剩余容量(单位：MB)
	//!\param int nDiskGroupType 盘组类型。0为普通盘组，1为报警盘组，2为冗余盘组，3为备份盘组
	//!\param hDiskGroup 磁盘分组句柄
	//!\param pSize 总容量
	//!\return 成功返回0，失败返回小于0的错误码。
	int GetDiskGroupRemainSize(const unsigned int nDiskGroupType, int *pSize);

	//!\brief 初始化盘组
	//!\return  成功返回0，失败为-1
	//##ModelId=49BF195E0008
	int Init();

	//!\brief 释放资源
	//!\return  成功返回0，失败为-1
	//##ModelId=49BF19610052
	int UnInit();

	//!\brief 获取实例指针
	//!\param void
	//!\return  成功返回指针，失败为NULL
	//##ModelId=49BDA6850206
	static CDiskGroupManager *GetInstance();

	//!\brief 释放实例指针
	//!\param CDiskGroupManager* handle 实例指针
	//!\return  成功返回0，失败为-1
	//##ModelId=49BDA69B001D
	static int ReleaseInstance();

	//!\brief 更新盘组信息
	//!\param 无
	//!\return  无
	void Update();

  private:

	typedef enum
	{
		FULL,
		ERROR
	} enumAlarmType;

	typedef struct
	{
		int AlarmType;
		CAlarm *obj;
	} tagAlarm;

	//盘组的原子操作
	pthread_mutex_t mutex;

	//!\brief 检测该录像类型在DiskGroup盘组内是否可以录像
	//!\param int type 录像类型
	//!\param int DiskGroup 盘组
	//!\return  可以为0，失败为false
	bool IsRecordRun(const unsigned int DiskGroup, int type, int nCh);

	//发送报警后，向上层报警
	int PushAlarm(int AlarmType, void *org, struct tagAlertInfo *Info = NULL);

	tagAlarm m_obj[2];

	//报警类
	CAlarm *m_theGroupFullAlarm;

	//报警类
	CAlarm *m_theGroupErrorAlarm;

	//检测数据
	int detect_data();

	//检测盘组
	int detect_status();

	//检测盘组
	int detect_status(int DiskGroup);

	friend class CDiskGroup;

	CDiskGroup **theCDiskGroup;

	//##ModelId=49BF01BF0291
	static CDiskGroupManager *theDiskGroupManger;
#ifndef TEST_MEM_INFO
	//##ModelId=49BE1EFF00EC
	 ~CDiskGroupManager();

	//static CDiskGroupManager *theDiskGroupManger;
	//##ModelId=49BF01BF02AC
	  CDiskGroupManager();
#else
  public:
  	//##ModelId=49BE1EFF00EC
	 ~CDiskGroupManager();
	
	//static CDiskGroupManager *theDiskGroupManger;
	//##ModelId=49BF01BF02AC
	  CDiskGroupManager();
#endif

	//!\brief 获取磁盘信息, 存入ptDiskInfo.
	//!\param iDiskIndex  磁盘编号(取值范围在0——(硬盘个数-1)).
	//!\param ptDiskInfo 磁盘信息结构体指针.
	//!\return  成功返回0, 失败返回-1.
	int GetDiskGroupInfo(int iDiskIndex);

	//!\brief 获取磁盘信息, 存入ptDiskInfo.
	//!\param iDiskIndex  磁盘编号(取值范围在0——(硬盘个数-1)).
	//!\param ptDiskInfo 磁盘信息结构体指针.
	//!\return  成功返回0, 失败返回-1.
	int GetAllDiskGroupInfo();

	int m_nWorkMode;

	//!\brief 磁盘满回调函数
	//!\param HANDLE handle 盘组的句柄
	//!\return  成功返回0，失败为-1
	friend int DiskGroupFullCallBack(HANDLE handle);


	int m_Status[4];
	int m_Full[4];

	friend void *work_thread_data(void *);
	static sem_t cond;

	unsigned int m_nDiskGroupNum;
	bool bWorkThreadRun;

};

#endif							/* _INC_CDiskGroupManager_49A5F8B70324_INCLUDED */
