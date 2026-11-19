
/*!
*\file   CDiskGroup.h 
*\brief  盘组的声明
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/

#include "CDiskGroupManager.h"
#include "tagDiskGroupInfo.h"
#include "CPartition.h"
#include "HANDLE.h"
#include "str_fs.h"
#include "PAR/CConfig.h"
#include "debug.h"

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CDISKGROUP_49A5F8A0033F_INCLUDED
#define _INC_CDISKGROUP_49A5F8A0033F_INCLUDED

#define ERR_DISKGROUP_INDEX      (0xff)


enum
{
	//报警录像类型
	GROUP_RECORDE_TYPE_ALARM = 1,

	//普通类型
	GROUP_RECORDE_TYPE_GENERAL,

	//手动录像类型
	//##ModelId=49AC9F5902F6
	GROUP_RECORDE_TYPE_MANUAL,

	//移动录像
	GROUP_RECORDE_TYPE_MOTION
};

//0为普通盘组，1为报警盘组，2为冗余盘组，3为备份
typedef struct _DISKGROUP_ATTRI_
{
	char Name[DISKGRUP_NAME_LEN_MAX];	/*!<盘组名称 */
	PKGSTREAMGROUPTYPE type;	/*!<盘组类型 */
	int iCycleMode;				/*!<轮转方式 */
	int BindType;				/*!<可以绑定的类型 */
	unsigned char Ch[MAX_BIND_CH_NUM];	/*!<默认绑定的通道 */
} DISKGROUP_ATTRI;

class CDPManager;

class CDiskManager;

struct tagDiskGroupInfo;

#define PBADBLOCKINFO void*
//!\brief 
//该类类方法为虚函数，不提供针对某一文件系统的方法，具体到不同
//的文件系统需要不同的实现。
//##ModelId=49A5F8A0033F
class CDiskGroup
{
  public:

	//!\brief 
	//创建一个物理分组
	//!\param 无
	//!\return  成功为0，失败为-1
	//##ModelId=49A5FE7A0040
	int Create();

	//!\brief 
	//销毁一个物理分组
	//!\param 无
	//!\return  成功为0，失败为-1
	//##ModelId=49A5FE7A0040
	int destroy();

	//!\brief 
	//设置指定硬盘分组的属性，硬盘分组属性包括读写、只读、冗余
	//三种。
	//!\param tagDiskGroupInfo* Info 盘组信息指针
	//!\return  成功为0，失败为-1
	//##ModelId=49A5FE7A0040
	int GetInfo(tagDiskGroupInfo * Info, int *iGroupFull = NULL);

	//!\brief 获取指定盘组的剩余容量(单位：MB)
	//!\param hDiskGroup 磁盘分组句柄
	//!\param pSize 总容量
	//!\return 成功返回0，失败返回小于0的错误码。
	int GetDiskGroupRemainSize(int *pSize);

	//!\brief 获取盘组的状态
	//!\return  正常为0，异常为1，部分分区丢失为2,盘组满为3
	//##ModelId=49A5FE7A0040
	int GetStatus();

	//!\brief 绑定某通道数据至该盘组
	//!\param nCH：int 通道
	//!\return  成功为0，失败为-1
	//##ModelId=49A5FEA2017E
	int BindDataCH(const unsigned int nCh, const unsigned int RecType);

	//!\brief 解除绑定某通道数据
	//!\param nCH：int 通道
	//!\return  成功为0，失败为-1
	//##ModelId=49A5FEA70366
	int UnBindDataCH(const unsigned int nCh, const unsigned int RecType);

	//!\brief 添加分区至该盘组
	//!\param CPartition *hHandle 分区对象指针
	//!\return  成功为0，失败为-1
	//##ModelId=49AF6C53031E
	int AddPartition(const char *Name);

	//!\brief 删除分区至该盘组
	//!\param CPartition *hHandle 分区对象指针
	//!\return  成功为0，失败为-1
	//##ModelId=49AF6CA800D1
	int DelPartition(const char *Name);

	//!\brief 设置数据保持的时间
	//!\param int DataKeepTime 数据保持的时间
	//!\param  PbadBlockInfo[out] : 坏块链表表头指针。
	//!\return  成功返回硬盘组坏块的个数，失败返回小于0的错误码。
	int Set_DataKeepTime(const unsigned int DataKeepTime);

	//!\brief 
	//功能：设置指定硬盘分组的属性，硬盘分组属性包括读写、只读
	//、冗余三种。(1)读写属性标示硬盘分组可查询，回放，数据可
	//写入；
	//!\brief(2)只读属性标示硬盘分组可查询，回放，但数据不能写
	//入；
	//!\brief(3)冗余属性标示硬盘分组可写入，但不可查询和回放。
	//!\brief冗余硬盘分组可同时标示为只读，此时原只读硬盘分组
	//和读写硬
	//!\brief盘分组的可查询和回放功能屏蔽。冗余硬盘分组转为读
	//写属性时
	//!\brief，其中原有数据将被格式化。
	//!\param int nAttr 盘组属性
	//!\return  成功返回0，失败返回小于0的错误码。
	//##ModelId=49AF822E0070
	int SetAttri(const unsigned int NAttr);


	static CDiskGroup *GetInstance(const unsigned int Index);


	/*!<盘组对应的物理盘组句柄 */
	HANDLE dev;

  private:

	  friend class CDiskGroupManager;
	//!\brief 打印盘组信息
	//!\param 无
	//!\return  无
	void print();

	//!\brief 更新
	//!\param char *Name 名称
	//!\return  成功为0，失败为-1
	int Update(const unsigned int index);

	//!\brief 临时绑定某通道数据至该盘组
	//!\param nCH：int 通道
	//!\return  成功为0，失败为-1
	//##ModelId=49A5FEA2017E
	int TmpBindDataCH(const unsigned int nCh, const unsigned int RecType);

	//!\brief 临时解除绑定某通道数据
	//!\param nCH：int 通道
	//!\return  成功为0，失败为-1
	//##ModelId=49A5FEA70366
	int TmpUnBindDataCH(const unsigned int nCh, const unsigned int RecType);

	//!\brief 更新绑定关系
	//!\param 无
	//!\return  成功为0，失败为-1
	int UpdateBindCh();

	//!\brief 
	//获取名称
	//!\param 无
	//!\return  返回名称
	char *GetName();

	//!\分区个数
	int m_nPartNum;

	//!\brief 检测该盘组中的数据是否超过了设置的时间
	//!\param 无
	//!\return  无
	void detect_data();

	/*!<盘组属性表 */
	static DISKGROUP_ATTRI *Attri;

	/*!<盘组实例句柄 */
	static CDiskGroup *handle[MAX_DISK_GROUP_NUM];

	/*!<盘组参数在配置文件中的段描述 */
	static char section[MAX_DISK_GROUP_NUM][DISKGRUP_NAME_LEN_MAX];

	/*!<盘组参数在配置文件中的绑定通道key描述 */
	static char chkey[MAX_BIND_CH_NUM][DISKGRUP_NAME_LEN_MAX];

	/*!<盘组参数在配置文件中的时间key描述 */
	static char timekey[DISKGRUP_NAME_LEN_MAX];

	/*!<盘组对应的物理盘组索引 */
	int DiskGroupType;

	/*!<盘组对应的物理盘组索引 */
	int DiskGroupIndex;

	/*!<盘组绑定的通道 */
	int nBindCh[MAX_BIND_CH_NUM];

	/*!<盘组的覆盖模式 */
	int iCycleMode;

	/*!<盘组的类型 */
	int nDiskGroupType;

	/*!<盘组信息结构体 */
	tagDiskGroupInfo DiskGroupInfo;
#ifndef TEST_MEM_INFO
	  CDiskGroup(const unsigned int Index);
#else
	public:
	  CDiskGroup(const unsigned int Index);
#endif  

	friend class CDPManager;
	friend class CHardDisk;
	friend class CDiskManager;

};

#endif							/* _INC_CDISKGROUP_49A5F8A0033F_INCLUDED */
