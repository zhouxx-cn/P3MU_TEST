
/*!
*\file   CDiskGroup.cpp
*\brief  盘组操作函数
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/
#ifdef __SUPPORT_SD_CARD__
#include <string.h>
//#include <iostream.h>
#include "tagDiskGroupInfo.h"
#include "tagDiskInfo.h"
#include "CDiskGroup.h"
#include "CDiskManager.h"
#include "CDPManager.h"
#include "pkg_interface.h"
#include "str_fs.h"
#include "debug.h"
#include "pkgsdfile.h"
#include "STR_global.h"

/* 消除本文件中的snprintf截断警告 */
#pragma GCC diagnostic ignored "-Wformat-truncation"

#define BIN_CH( bin,type ) ( ( bin ) |= (1 << (type -1) ) )
#define UNBIN_CH( bin,type ) ( (bin) &= ( ~( 1 << (type - 1) ) ) )
#define ISBIN( bin,type ) ( bin & ( 1 << ( type -1 ) ) )


static DISKGROUP_ATTRI attri_tmp[] = { 
{"common", DISKGROUP_NORMAL, 1, 0xf, {1}},
{"alarm", DISKGROUP_NORMAL, 1, 0x1, {1}},
{"redundancy", DISKGROUP_OVERPLUS, 1, 0xf, {0}},
{"backup", DISKGROUP_BACKUP, 1, 0x0, {0}}
};

DISKGROUP_ATTRI *CDiskGroup::Attri;

CDiskGroup *CDiskGroup::handle[MAX_DISK_GROUP_NUM];

char CDiskGroup::section[MAX_DISK_GROUP_NUM][DISKGRUP_NAME_LEN_MAX];

char CDiskGroup::chkey[MAX_BIND_CH_NUM][DISKGRUP_NAME_LEN_MAX];

char CDiskGroup::timekey[DISKGRUP_NAME_LEN_MAX];

//!\brief 
//创建一个物理分组
//!\param 无
//!\return  成功为0，失败为-1
//##ModelId=49A5FE7A0040
int CDiskGroup::Create()
{
	int ret = 0;

	if (NULL != dev)
	{
		DEBUG_MSG("已经存在了物理分组%s无法继续创建\n", GetName());
		return IDVR_STR_ERROR_GROUP_EXIT;
	}

	DEBUG_MSG("创建盘组%d %s\n", nDiskGroupType, Attri[nDiskGroupType].Name);

	dev = Macro_DiskGroupCreate(Attri[nDiskGroupType].Name, Attri[nDiskGroupType].iCycleMode, Attri[nDiskGroupType].type);


	WARN_MSG(dev != NULL);

	if (NULL == dev)
	{
		ret = -1;
	}

	DEBUG_MSG("创建盘组%d结束\n", nDiskGroupType);
	//ret = Macro_SetDiskGroupAttr(dev,Attri[DiskGroupType].attri);
	return ret;
}


//!\brief 获取指定盘组的剩余容量(单位：MB)
//!\param hDiskGroup 磁盘分组句柄
//!\param pSize 总容量
//!\return 成功返回0，失败返回小于0的错误码。
int CDiskGroup::GetDiskGroupRemainSize(int *pSize)
{
	int ret = 0;

	if (NULL == dev)
	{
		DEBUG_MSG("盘组%s未创建\n", Attri[nDiskGroupType].Name);
		return -1;
	}

	ret = Macro_GetDiskGroupRemainSize(dev, (unsigned long *) pSize);

	return ret;
}

//!\brief 
//销毁一个物理分组
//!\param 无
//!\return  成功为0，失败为-1
//##ModelId=49A5FE7A0040
int CDiskGroup::destroy()
{
	int ret = -1;

	if (NULL == dev)
	{
		DEBUG_MSG("盘组%s未创建\n", Attri[nDiskGroupType].Name);
		return -1;
	}

#ifdef __PKGSTREAM_STORTAGE__
	ret = Macro_DiskGroupDestroy(dev);
#endif

#ifdef __JFS_STORAGE__
#ifndef TEST_MEM_INFO
	delete(CRecordFile *) dev;
#else
	CommonKit::TDestroyObject<CRecordFile>::DestroyObject((CRecordFile *) dev);
#endif

	dev = NULL;
	ret = 0;
#endif

	WARN_MSG(ret == 0);
	if (0 == ret)
	{
		dev = NULL;
	}

	return ret;
}

CDiskGroup *CDiskGroup::GetInstance(const unsigned int Index)
{
	CConfig *theCConfig = CConfig::GetInstance();

	int ch;

	int value;

	int nDiskGroupType;

	if (Index > MAX_DISK_GROUP_NUM)
	{
		DEBUG_MSG("超过可以支持的最大分区根数\n");
		return NULL;
	}

	CDiskGroup::Attri = attri_tmp;

	if (NULL == CDiskGroup::handle[Index])
	{
#ifndef TEST_MEM_INFO
		CDiskGroup::handle[Index] = new CDiskGroup(Index);
#else
		CDiskGroup::handle[Index] = CommonKit::TCreateObjectP1<CDiskGroup, const unsigned int>::CreateObject(Index, __FILE__, __FUNCTION__, __LINE__);
#endif
	}

	if (NULL == CDiskGroup::handle[Index])
	{
		DEBUG_MSG("实例化%d失败\n", Index);
	}

	DEBUG_MSG("Run Here\n");

	nDiskGroupType = handle[Index]->nDiskGroupType;

	for (ch = 0; ch < MAX_BIND_CH_NUM; ch++)
	{
		value = Attri[handle[Index]->nDiskGroupType].Ch[ch];

		if (NULL != theCConfig)
		{
			value = theCConfig->GetValue(section[nDiskGroupType], chkey[ch], Attri[nDiskGroupType].Ch[ch]);
		}

		if (0 == nDiskGroupType)
		{
			value = Attri[nDiskGroupType].BindType;
		}
		handle[Index]->nBindCh[ch] = value;
		handle[Index]->DiskGroupInfo.bCh[ch] = (handle[Index]->nBindCh[ch] != 0);
	}

	DEBUG_MSG("Run Here\n");
	value = CConfig::GetInstance()->GetValue(section[nDiskGroupType], timekey, 0x7fffffff);
	value = (value & 0xffffffff);
	handle[Index]->DiskGroupInfo.nDataKeepTime = value;

	return CDiskGroup::handle[Index];
}

CDiskGroup::CDiskGroup(const unsigned int Index)
{
	int i;

	nDiskGroupType = Index;
	DEBUG_MSG("初始化盘组%d\n", nDiskGroupType);
	dev = NULL;
	memset(&DiskGroupInfo, 0, sizeof (tagDiskGroupInfo));
	snprintf(DiskGroupInfo.Name, sizeof(DiskGroupInfo.Name), "%s", Attri[nDiskGroupType].Name);
	snprintf(CDiskGroup::section[Index], sizeof(CDiskGroup::section[Index]), "disk_group_%s", CDiskGroup::Attri[nDiskGroupType].Name);

	for (i = 0; i < MAX_BIND_CH_NUM; i++)
	{
		snprintf(CDiskGroup::chkey[i], sizeof(CDiskGroup::chkey[i]), "ch%d", i);
	}

	snprintf(CDiskGroup::timekey, sizeof(CDiskGroup::timekey), "%s", "keeptime");

	m_nPartNum = 0;
}

//##ModelId=49A5FE7A0040
int CDiskGroup::GetInfo(tagDiskGroupInfo * Info, int *iGroupFull)
{
	// TODO: Add your specialized code here.
	PSS_DISKGROUPINFO ptDiskGroupInfo = {0};

	int ch;

	int partitions;

	DEBUG_MSG("Run Here\n");
	if (DiskGroupIndex != ERR_DISKGROUP_INDEX)
	{
		dev = Macro_GetDiskGroupInfo(DiskGroupIndex, &ptDiskGroupInfo);
		if (NULL != iGroupFull)
		{
			*iGroupFull = ptDiskGroupInfo.iGroupFull;
		}
	}
	else
	{
		dev = NULL;
		if (NULL != iGroupFull)
		{
			*iGroupFull = ptDiskGroupInfo.iGroupFull;
		}
	}

	DEBUG_MSG("Run Here\n");

	DEBUG_MSG("DiskGroupIndex = %d\n", DiskGroupIndex);
	DEBUG_MSG("nDiskGroupType = %d\n", nDiskGroupType);

	for (ch = 0; ch < MAX_BIND_CH_NUM; ch++)
	{
		DEBUG_MSG("nDiskGroupType = %d nBindCh[%d] = %d\n", nDiskGroupType, ch, nBindCh[ch]);
		DiskGroupInfo.bCh[ch] = (nBindCh[ch] != 0);
	}

	if (NULL == dev)
	{
		DEBUG_MSG("盘组%s未创建\n", Attri[nDiskGroupType].Name);
		memset(Info, 0, sizeof (tagDiskGroupInfo));
		DiskGroupInfo.Partitions = 0;
		DiskGroupInfo.tCreateTime = 0;
		memcpy(Info, &DiskGroupInfo, sizeof (tagDiskGroupInfo));
		return 0;
	}

	DEBUG_MSG("Run Here\n");
	DiskGroupInfo.Partitions = ptDiskGroupInfo.iValidPartNum;
	DiskGroupInfo.tCreateTime = ptDiskGroupInfo.tCreateTime;

	DEBUG_MSG("Run Here\n");
	for (partitions = 0; partitions < ptDiskGroupInfo.iValidPartNum; partitions++)
	{
		snprintf(DiskGroupInfo.tPartitions[partitions].acSerialNo, sizeof(DiskGroupInfo.tPartitions[partitions].acSerialNo), "%s", ptDiskGroupInfo.atPartInfo[partitions].acSerialNo);
		snprintf(DiskGroupInfo.tPartitions[partitions].acModelName, sizeof(DiskGroupInfo.tPartitions[partitions].acModelName), "%s", ptDiskGroupInfo.atPartInfo[partitions].acModelName);
		snprintf(DiskGroupInfo.tPartitions[partitions].acPartName, sizeof(DiskGroupInfo.tPartitions[partitions].acPartName), "%s", ptDiskGroupInfo.atPartInfo[partitions].acPartName);

		DiskGroupInfo.tPartitions[partitions].dwPartSize = ptDiskGroupInfo.atPartInfo[partitions].dwPartSize / 2 / 1024;
		DEBUG_MSG("ptDiskGroupInfo.atPartInfo[partitions].dwPartSize = %lu\n", ptDiskGroupInfo.atPartInfo[partitions].dwPartSize);
		DEBUG_MSG("DiskGroupInfo = %lu\n", DiskGroupInfo.tPartitions[partitions].dwPartSize);
		DiskGroupInfo.tPartitions[partitions].iDevNo = ptDiskGroupInfo.atPartInfo[partitions].iDevNo;

	}

	DEBUG_MSG("Run Here\n");


	DEBUG_MSG("DiskGroupInfo = %lu\n", DiskGroupInfo.tPartitions[0].dwPartSize);
	memcpy(Info, &DiskGroupInfo, sizeof (tagDiskGroupInfo));
	DEBUG_MSG("Info = %lu\n", Info->tPartitions[0].dwPartSize);

	print();
	return (int) 0;
}


//!\brief 获取盘组的状态
//!\return  正常为0，异常为1，部分分区丢失为2,3为盘组满
int CDiskGroup::GetStatus()
{
	tagDiskGroupInfo Info;

	int iGroupFull;				//分组是否已经录满(只有在属性是录满停止时有效)1表示录满，0为没有录满

	GetInfo(&Info, &iGroupFull);
	DEBUG_MSG("Run Here\n");

	if (Info.Partitions > m_nPartNum)
	{
		m_nPartNum = Info.Partitions;
	}

	if (Info.Partitions == 0 && m_nPartNum != 0)
	{
		return 1;
	}
	else if (Info.Partitions == 0 && m_nPartNum == 0)
	{
		return 0;
	}
	else if (iGroupFull == 1)		//盘组的条件
	{
		return 3;
	}
	else if (iGroupFull == 2)		//盘组的条件
	{
		return 1;
	}

	return 0;
}

int CDiskGroup::UpdateBindCh()
{
	int nCh, ret = -1;

	int RecType;

	tagDiskGroupInfo Info;

	if (dev == NULL)
	{
		DEBUG_MSG("盘组%s未创建\n", Attri[nDiskGroupType].Name);
		return ret;
	}

	GetInfo(&Info);

	for (RecType = GROUP_RECORDE_TYPE_ALARM; RecType <= GROUP_RECORDE_TYPE_MOTION; RecType++)
	{
		if (Attri[nDiskGroupType].BindType & (1 << (RecType - 1)))
		{
			for (nCh = 0; nCh < MAX_BIND_CH_NUM; nCh++)
			{
				if (ISBIN(nBindCh[nCh], RecType))
				{
					DEBUG_MSG("nDiskGroupType = %d nCh = %d,RecType = %d dev = %p\n", nDiskGroupType, nCh, RecType, dev);
					ret = Macro_DiskGroupBind(nCh, RecType, dev);
					WARN_MSG(ret == 0);
				}
			}
		}
	}

	return 0;
}

void CDiskGroup::print()
{
#if 0
	int i;

	cout << "\n盘组名称\t" << DiskGroupInfo.Name << endl;
	cout << "创建时间\t" << DiskGroupInfo.tCreateTime << endl;
	cout << "绑定通道\t";

	for (i = 0; i < MAX_BIND_CH_NUM; i++)
	{
		cout << " " << i << ":" << (int) nBindCh[i];
	}

	cout << endl;
	cout << "数据保留时间\t" << DiskGroupInfo.nDataKeepTime << endl;
	cout << "分区个数\t" << DiskGroupInfo.Partitions << endl;
	for (i = 0; i < DiskGroupInfo.Partitions; i++)
	{
		cout << "分区名称\t" << DiskGroupInfo.tPartitions[i].acPartName << endl;
		cout << "磁盘型号\t" << DiskGroupInfo.tPartitions[i].acModelName << endl;
		cout << "磁盘序列号\t" << DiskGroupInfo.tPartitions[i].acSerialNo << endl;
		cout << "大小\t" << DiskGroupInfo.tPartitions[i].dwPartSize << endl;
		cout << "分区号\t" << DiskGroupInfo.tPartitions[i].iDevNo << endl;
	}
#endif
}

//##ModelId=49A5FEA2017E
int CDiskGroup::BindDataCH(const unsigned int nCh, const unsigned int RecType)
{
	// TODO: Add your specialized code here.
	int ret = -1;

	if (dev == NULL)
	{
		DEBUG_MSG("盘组%s未创建\n", Attri[nDiskGroupType].Name);
		return ret;
	}

	if (Attri[nDiskGroupType].BindType & (1 << (RecType - 1)))
	{
		BIN_CH(nBindCh[nCh], RecType);
		DiskGroupInfo.bCh[nCh] = (nBindCh[nCh] != 0);
		CConfig::GetInstance()->SetValue(section[nDiskGroupType], chkey[nCh], nBindCh[nCh]);

		DEBUG_MSG("nDiskGroupType = %d RecType = %d nBindCh[%d] = %d\n", nDiskGroupType, RecType, nCh, nBindCh[nCh]);
		ret = Macro_DiskGroupBind(nCh, RecType, dev);
		DEBUG_MSG("绑定完成:ret = %d\n", ret);
		WARN_MSG(ret == 0);

		if (0 != ret)
		{
			DEBUG_MSG("盘组%s\n", Attri[nDiskGroupType].Name);
			DEBUG_MSG("盘组类型%d\n", Attri[nDiskGroupType].type);
			DEBUG_MSG("nCh = %d,RecType = %d,dev = %p\n", nCh, RecType, dev);
		}
	}
	else
	{
		DEBUG_MSG("该磁盘分组无法绑定。盘组：%s通道%d类型%d\n", DiskGroupInfo.Name, nCh, RecType);
	}
	return 0;
}

//##ModelId=49A5FEA2017E
int CDiskGroup::TmpBindDataCH(const unsigned int nCh, const unsigned int RecType)
{
	// TODO: Add your specialized code here.
	int ret = -1;

	if (dev == NULL)
	{
		DEBUG_MSG("盘组%s未创建\n", Attri[nDiskGroupType].Name);
		return ret;
	}


	if (Attri[nDiskGroupType].BindType & (1 << (RecType - 1)))
	{
		DEBUG_MSG("TmpBindDataCH nDiskGroupType = %d nCh = %d,RecType = %d nBindCh[nCh] = %d\n", nDiskGroupType, nCh, RecType, nBindCh[nCh]);
		ret = Macro_DiskGroupBind(nCh, RecType, dev);
		WARN_MSG(ret == 0);
	}
	else
	{
		DEBUG_MSG("该磁盘分组无法绑定。盘组：%s通道%d类型%d\n", DiskGroupInfo.Name, nCh, RecType);
	}

	return ret;
}

//##ModelId=49A5FEA70366
int CDiskGroup::TmpUnBindDataCH(const unsigned int nCh, const unsigned int RecType)
{
	// TODO: Add your specialized code here.
	int ret = -1;

	if (dev == NULL)
	{
		DEBUG_MSG("盘组%s未创建\n", Attri[nDiskGroupType].Name);
		return ret;
	}

	DEBUG_MSG("nDiskGroupType = %d nCh = %d,RecType = %d nBindCh[nCh] = %d\n", nDiskGroupType, nCh, RecType, nBindCh[nCh]);
	ret = Macro_DiskGroupUnBind(nCh, RecType, dev);
	WARN_MSG(ret == 0);
	return ret;
}


int CDiskGroup::UnBindDataCH(const unsigned int nCh, const unsigned int RecType)
{
	// TODO: Add your specialized code here.
	int ret = -1;

	if (dev == NULL)
	{
		DEBUG_MSG("盘组%s未创建\n", Attri[nDiskGroupType].Name);
		return ret;
	}

	if (!ISBIN(nBindCh[nCh], RecType))
	{
		DEBUG_MSG("该盘组%s没有绑定通道%d类型%d\n", DiskGroupInfo.Name, nCh, RecType);
		return 0;
	}

	UNBIN_CH(nBindCh[nCh], RecType);

	DiskGroupInfo.bCh[nCh] = (nBindCh[nCh] != 0);

	CConfig::GetInstance()->SetValue(section[nDiskGroupType], chkey[nCh], nBindCh[nCh]);
	DEBUG_MSG("nDiskGroupType = %d nCh = %d,RecType = %d nBindCh[nCh] = %d\n", nDiskGroupType, nCh, RecType, nBindCh[nCh]);
	ret = Macro_DiskGroupUnBind(nCh, RecType, dev);
	WARN_MSG(ret == 0);
	return 0;
}


int CDiskGroup::AddPartition(const char *Name)
{
	// TODO: Add your specialized code here.
	int ret = -1;

	CDiskManager *theCDiskManager = CDiskManager::GetInstance();

	tagDiskInfo DiskInfo;

	int DiskNum;

	int i, FindPartition = -1;

	unsigned int partitions;

	if (dev == NULL)
	{
		DEBUG_MSG("盘组%s未创建\n", Attri[nDiskGroupType].Name);
		return ret;
	}

	//查找是否属于系统保留分区
	if (NULL != theCDiskManager)
	{
		theCDiskManager->GetNum(&DiskNum);
		for (i = 0; i < DiskNum; i++)
		{
			memset(&DiskInfo, 0, sizeof (tagDiskInfo));
			theCDiskManager->GetInfo(i, &DiskInfo);
			for (partitions = 0; partitions < DiskInfo.Partitions; partitions++)
			{
				DEBUG_MSG("Name = %s,DiskInfo.tPartitions[%d].Name = %s\n", Name, partitions, DiskInfo.tPartitions[partitions].Name);
				if (0 == strcmp(DiskInfo.tPartitions[partitions].Name, Name))
				{
					DEBUG_MSG("Name = %s,DiskInfo.tPartitions[%d].Name = %s\n", Name, partitions, DiskInfo.tPartitions[partitions].Name);
					FindPartition = partitions;
					break;
				}
			}
			if (-1 != FindPartition)
			{
				break;
			}
		}
	}

	if ((0 == FindPartition))
	{
		DEBUG_MSG("系统保留分区%s，无法添加\n", DiskInfo.tPartitions[0].Name);
		return IDVR_STR_ERROR_SYS_PARTITION_ADD;
	}

	if ((-1 == FindPartition))
	{
		DEBUG_MSG("没有找到该分区%s，无法添加\n", Name);
		return IDVR_STR_ERROR_PARTITION_NOFIND;
	}

	ret = Macro_DiskGroupPartAdd(dev, Name);
	WARN_MSG(ret == 0);
	Update(DiskGroupIndex);
	return (int) ret;
}


int CDiskGroup::DelPartition(const char *Name)
{
	// TODO: Add your specialized code here.
	int ret = IDVR_STR_ERROR_PARTITION_NOFIND;

	int i = 0;

	if (dev == NULL)
	{
		DEBUG_MSG("盘组%s未创建\n", Attri[nDiskGroupType].Name);
		return ret;
	}

	print();

	for (; i < DiskGroupInfo.Partitions; i++)
	{
		if (0 == strcmp(DiskGroupInfo.tPartitions[i].acPartName, Name))
		{
			ret = Macro_DiskGroupPartDel(dev, i);
			WARN_MSG(ret == 0);
			break;
		}
	}

	WARN_MSG(ret == 0);
	Update(DiskGroupIndex);
	return (int) ret;
}

//!\brief 设置数据保持的时间
//!\param int DataKeepTime 数据保持的时间
//!\return  成功返回硬盘组坏块的个数，失败返回小于0的错误码。
int CDiskGroup::Set_DataKeepTime(const unsigned int DataKeepTime)
{
	int ret = -1;

	DiskGroupInfo.nDataKeepTime = (DataKeepTime & 0xffffffff);
	CConfig::GetInstance()->SetValue(section[nDiskGroupType], timekey, DataKeepTime);

	if (dev == NULL)
	{
		DEBUG_MSG("盘组%s未创建\n", Attri[nDiskGroupType].Name);
		return ret;
	}

	if (KEEP_FOREVER == DiskGroupInfo.nDataKeepTime)
	{
#ifdef USE_SD_PKG
		ZW_PkgSd_SetDiskGroupCycleMode(PSS_CYCLEMODE_STOP);
#else
		Macro_SetDiskGroupCycleMode(dev, PSS_CYCLEMODE_STOP);
#endif
	}
	else
	{
#ifdef USE_SD_PKG
		ZW_PkgSd_SetDiskGroupCycleMode(PSS_CYCLEMODE_COVER);
#else
		Macro_SetDiskGroupCycleMode(dev, PSS_CYCLEMODE_COVER);
#endif
	}

	return 0;
}

//!\brief 更新盘组信息
//!\param int DataKeepTime 数据保持的时间
//!\param  PbadBlockInfo[out] : 坏块链表表头指针。
//!\return  成功返回硬盘组坏块的个数，失败返回小于0的错误码。
int CDiskGroup::Update(const unsigned int index)
{
	tagDiskGroupInfo info;

	//更新盘组对应物理盘组的索引值
	DiskGroupIndex = index;

	if (DiskGroupIndex == ERR_DISKGROUP_INDEX)
	{
		dev = NULL;
	}
	else
	{
		GetInfo(&info);

		if (KEEP_FOREVER == DiskGroupInfo.nDataKeepTime)
		{
#ifdef USE_SD_PKG
			ZW_PkgSd_SetDiskGroupCycleMode(PSS_CYCLEMODE_STOP);
#else
			Macro_SetDiskGroupCycleMode(dev, PSS_CYCLEMODE_STOP);
#endif
		}
		else
		{
#ifdef USE_SD_PKG
			ZW_PkgSd_SetDiskGroupCycleMode(PSS_CYCLEMODE_COVER);
#else
			Macro_SetDiskGroupCycleMode(dev, PSS_CYCLEMODE_COVER);
#endif
		}


		print();
	}


	return 0;
}


int CDiskGroup::SetAttri(const unsigned int NAttr)
{
	// TODO: Add your specialized code here.
	return (int) 0;
}

//!\brief 
//获取名称
//!\param 无
//!\return  返回名称
char *CDiskGroup::GetName()
{
	return Attri[nDiskGroupType].Name;
}

//!\brief 检测该盘组中的数据是否超过了设置的时间
//!\param 无
//!\return  无
void CDiskGroup::detect_data()
{
//  int i;
//  int rectype;
	unsigned long now;

	if (dev == NULL)
	{
		DEBUG_MSG("盘组%s未创建\n", Attri[nDiskGroupType].Name);
		return;
	}

	now = time(NULL);
	if (now > DiskGroupInfo.nDataKeepTime)
	{
		DEBUG_MSG("盘组%d数据生存时间为%d秒\n", nDiskGroupType, DiskGroupInfo.nDataKeepTime);
		CDPManager::GetInstance()->DR_DelRecordData(nDiskGroupType, 0, now - DiskGroupInfo.nDataKeepTime);
	}
	else
	{
		//DEBUG_MSG("nDiskGroupType = %d 此时间%d为尽可能保存\n",nDiskGroupType,DiskGroupInfo.nDataKeepTime);
	}
}
#endif