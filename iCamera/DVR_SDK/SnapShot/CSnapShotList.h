
/*!
*\file   CSnapShotList.h 
*\brief  该类用于判断对象是否是已经存在的，还是已经被释放的
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/
#ifndef _CSNAPSHOTLIST_H_
#define _CSNAPSHOTLIST_H_

//#include <iostream.h>
#include <string.h>
#include "SNAPSHOTINFO.h"



/**************************************************************************
*                            常量                                        *
**************************************************************************/

/**************************************************************************
*                            宏                                          *
**************************************************************************/

/**************************************************************************
*                          数据类型                                       *
**************************************************************************/

typedef struct _EX_SNAPSHOTINFO_
{
	//!<文件名称时间
	//##ModelId=49A7969A002E
	int m_NameTime;

	//!<抓拍时间
	//##ModelId=49A7969A002E
	int m_nTime;

	//!<所属通道
	//##ModelId=49A7969A002D
	unsigned char m_Channel;
#ifndef __CI785__
	//!<分区
	//##ModelId=49A78C120172
	char m_Dev;

	//!<分区编号
	//##ModelId=49A78C120172
	char m_DevNo;
#else
	//!<挂载点
	//##ModelId=49A78C120172
	char m_Mount[20];

#endif
	char m_bMark:1;
	char m_FileTypoe:7;

} EX_SNAPSHOTINFO;

class CSnapShotList
{
  public:

	//!\brief 添加一个节点
	//!\param char* hNode 节点的指针
	//!\return  成功为0，失败为-1
	int Insert(const EX_SNAPSHOTINFO hNode);

	//!\brief 删除一个节点
	//!\param char* hNode 节点的指针
	//!\return  成功为0，失败为-1
	int Delete(const EX_SNAPSHOTINFO hNode);

	//!\brief 判断节点是否存在
	//!\param char* hNode 节点的指针
	//!\return  存在与当前的文件管理器内为1，存在与其他文件管理器内为2，不存在为0
	int IsExist(const EX_SNAPSHOTINFO hNode);

	//!\brief 删除所有节点
	//!\param char* hNode 节点的指针
	//!\return  成功为0，失败为-1
	int DeleteAll();

	//!\brief 打印节点信息
	//!\return  无
	void print();

	//!\brief 获取节点总数
	//!\return  节点总数
	unsigned int GetNodeCount();


	//!\brief 获取节点路径
	//!\param int Index 节点索引
	//!\param char *FilePath
	//!\return  成功为0，失败为-1
	int GetNodeInfo(int Index, EX_SNAPSHOTINFO * Info);

	//!\brief 设置选择标记
	//!\param int nIndex 文件在总个数中的索引。从0开始
	//!\param int bMark;TRUE为标记，FALSE取消标记
	//!\return 成功为0，失败为-1
	//##ModelId=49BEFE8103C1
	int SetMark(int Index, unsigned int bMark);

	//!\brief 设置选择标记
	//!\param int nStartIndex 开始索引
	//!\param int nStopIndex  结束索引
	//!\param int bMark;TRUE为标记，FALSE取消标记
	//!\return 成功为0，失败为-1
	//##ModelId=49BEFE8103C1
	int SetMark(int nStartIndex, int nStopIndex, unsigned int bMark);

	//!\brief 获取选择标记总数
	//!\return 成功为标记的总数，失败为-1
	//##ModelId=49BEFE8103C1
	int GetMarkSum();

	//!\brief 获取是否被标记
	//!\return 标记范围ture，没有标记范围0，失败返回-1
	//##ModelId=49BEFE8103C1
	int IsMark(int Index);

	  CSnapShotList();
	 ~CSnapShotList();
  protected:
  private:
	//节点总数
	int m_NodeCount;

	//标记的总数
	int m_nMarkSum;



	typedef struct _PNode_
	{
		EX_SNAPSHOTINFO hNode;
		struct _PNode_ *next;

		struct _PNode_ operator=(EX_SNAPSHOTINFO tmp)
		{
			memcpy(&hNode, &tmp, sizeof (EX_SNAPSHOTINFO));
			return *this;
		}

		bool operator==(EX_SNAPSHOTINFO tmp)
		{
			//printf("tmp = %s,hNode = %s\n",tmp,hNode);
			if (0 == memcmp(&tmp, &hNode, sizeof (EX_SNAPSHOTINFO)))
			{
				return true;
			}
			else
			{
				return false;
			}
		}

	}
	PNode;

	//头节点
	PNode *m_head;

	//尾节点
	PNode *m_tail;

	//当前访问的节点
	PNode *m_cur;

	//当前访问的节点的索引值
	int m_nIndex;;
};

#endif
