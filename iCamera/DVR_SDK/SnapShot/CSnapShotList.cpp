
/*!
*\file   CSnapShotList.h 
*\brief  该类用于判断对象是否是已经存在的，还是已经被释放的
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/
//#include <iostream.h>
#include "CSnapShotList.h"
#include "debug.h"

#include "SnapShot_global.h"


/**************************************************************************
*                            常量                                        *
**************************************************************************/

/**************************************************************************
*                            宏                                          *
**************************************************************************/

/**************************************************************************
*                          数据类型                                       *
**************************************************************************/


/**************************************************************************
*                           全局变量                                      *
**************************************************************************/

/**************************************************************************
*                           局部函数原型                                  *
 **************************************************************************/

/**************************************************************************
*                           方法实现                                      *
**************************************************************************/


//!\brief 添加一个节点
//!\param char * hNode 节点的指针
//!\return  成功为0，失败为-1
int CSnapShotList::Insert(const EX_SNAPSHOTINFO hNode)
{
	int ret = -1;

	PNode *tmp, *addNode;

/*
	if (IsExist(hNode))
	{
		return -1;
	}
*/

//添加到链表
#ifndef TEST_MEM_INFO
	addNode = new PNode;
#else
	addNode = (PNode *)MemoryMalloc(sizeof(PNode), "SnapShot", __FILE__, __FUNCTION__, __LINE__);
#endif

	if (NULL == addNode)
	{
		ERROR_MSG("申请内存失败\n");
		return ret;
	}


	*addNode = hNode;

	addNode->next = NULL;

	if (NULL == m_head)
	{
		m_head = addNode;
		m_tail = addNode;
	}
	else
	{
		tmp = m_tail;

		while (NULL != tmp->next)
		{
			tmp = tmp->next;
		}

		tmp->next = addNode;
		m_tail = addNode;
	}

	m_NodeCount++;

	return 0;
}


//!\brief 删除所有节点
//!\param char * hNode 节点的指针
//!\return  成功为0，失败为-1
int CSnapShotList::DeleteAll()
{
	int ret = 0;

	PNode *tmp;

//从链表中删除  
	if (NULL == m_head)
	{
		DEBUG_MSG("节点不存在\n");
		return ret;
	}
	else
	{
		while (NULL != m_head)
		{
			tmp = m_head;
			m_head = m_head->next;

			if (m_cur == tmp)
			{
				m_cur = NULL;
				m_nIndex = -1;
			}

#ifndef TEST_MEM_INFO
			delete tmp;
#else
			MemoryFree(tmp);
#endif
			tmp = NULL;
		}
	}

	m_tail = NULL;
	m_NodeCount = 0;

	return 0;
}

//!\brief 删除一个节点
//!\param char * hNode 节点的指针
//!\return  成功为0，失败为-1
int CSnapShotList::Delete(const EX_SNAPSHOTINFO hNode)
{
	int ret = -1;

	PNode *tmp, *delNode;

//从链表中删除  
	if (NULL == m_head)
	{
		DEBUG_MSG("节点不存在\n");
		return ret;
	}
	else if (*m_head == hNode)
	{
		tmp = m_head;
		m_head = m_head->next;
		if (tmp == m_tail)
		{
			m_tail = NULL;
		}

		if (m_cur == tmp)
		{
			m_cur = NULL;
			m_nIndex = -1;
		}

#ifndef TEST_MEM_INFO
		delete tmp;
#else
		MemoryFree(tmp);
#endif
		tmp = NULL;
	}
	else
	{
		tmp = m_head;

		while (NULL != tmp->next)
		{
			if (*tmp->next == hNode)
			{
				delNode = tmp->next;
				tmp->next = tmp->next->next;

				if (m_tail == delNode)
				{
					m_tail = tmp;
				}

				if (m_cur == delNode)
				{
					m_cur = NULL;
					m_nIndex = -1;
				}

#ifndef TEST_MEM_INFO
				delete delNode;
#else
				MemoryFree(delNode);
#endif
				delNode = NULL;

				return 0;
			}

			tmp = tmp->next;
		}
	}

	return ret;
}

//!\brief 设置选择标记
//!\param int nIndex 文件在总个数中的索引。从0开始
//!\param int bMark;TRUE为标记，FALSE取消标记
//!\return 成功为0，失败为-1
//##ModelId=49BEFE8103C1
int CSnapShotList::SetMark(int Index, unsigned int bMark)
{
	//int ret = -1;
	int i = 0;

	PNode *tmp;

	tmp = m_head;
	bMark = (bMark != 0);

	while (NULL != tmp)
	{
		if (i == Index)
		{
			if (tmp->hNode.m_bMark == bMark)
			{
				return 0;
			}

			tmp->hNode.m_bMark = bMark;

			if (0 == bMark)
			{
				m_nMarkSum--;
			}
			else
			{
				m_nMarkSum++;
			}

			return 0;
		}

		i++;
		tmp = tmp->next;
	}

	DEBUG_MSG("Index = %d,m_NodeCount = %d\n", Index, m_NodeCount);
	return -1;

}

//!\brief 设置选择标记
//!\param int nStartIndex 开始索引
//!\param int nStopIndex  结束索引
//!\param int bMark;TRUE为标记，FALSE取消标记
//!\return 成功为0，失败为-1
//##ModelId=49BEFE8103C1
int CSnapShotList::SetMark(int nStartIndex, int nStopIndex, unsigned int bMark)
{
	//int ret = -1;
	int i = 0;

	PNode *tmp;

	tmp = m_head;
	bMark = (bMark != 0);

	while (NULL != tmp)
	{
		if ((i >= nStartIndex) && (i <= nStopIndex))
		{
			if (tmp->hNode.m_bMark == bMark)
			{
				tmp = tmp->next;
				continue;
			}

			tmp->hNode.m_bMark = bMark;

			if (0 == bMark)
			{
				m_nMarkSum--;
			}
			else
			{
				m_nMarkSum++;
			}
		}
		else if (i > nStopIndex)
		{
			return 0;
		}

		i++;
		tmp = tmp->next;
	}

	if (i >= nStartIndex)
	{
		return 0;
	}

	DEBUG_MSG("Index = %d,m_NodeCount = %d\n", Index, m_NodeCount);
	return -1;

}

//!\brief 获取是否被标记
//!\return 标记范围ture，没有标记范围0，失败返回-1
//##ModelId=49BEFE8103C1
int CSnapShotList::IsMark(int Index)
{
	//int ret = -1;
	int i = 0;

	PNode *tmp;

	tmp = m_head;

	if (m_cur != NULL && ((m_nIndex + 1) == Index))
	{
		tmp = m_cur->next;
		i = m_nIndex + 1;
	}

	if (m_cur != NULL && ((m_nIndex) == Index))
	{
		tmp = m_cur;
		i = m_nIndex;
	}

	while (NULL != tmp)
	{
		if (i == Index)
		{
			m_cur = tmp;
			m_nIndex = i;
			return tmp->hNode.m_bMark;
			return 0;
		}

		i++;
		tmp = tmp->next;
	}

	TRACE("Index = %d,m_NodeCount = %d\n", Index, m_NodeCount);
	return -1;
}

//!\brief 获取选择标记总数
//!\return 成功为标记的总数，失败为-1
//##ModelId=49BEFE8103C1
int CSnapShotList::GetMarkSum()
{
	return m_nMarkSum;
}


//!\brief 判断节点是否存在
//!\param char * hNode 节点的指针
//!\return  存在与当前的文件管理器内为1，不存在为0
int CSnapShotList::IsExist(const EX_SNAPSHOTINFO hNode)
{
	int ret = false;

	PNode *tmp;

	tmp = m_head;

	while (NULL != tmp)
	{
		if (*tmp == hNode)
		{
			ret = 1;

			return ret;
		}

		tmp = tmp->next;
	}

	return ret;
}

//!\brief 获取节点路径
//!\param int Index 节点索引
//!\param char *FilePath
//!\return  成功为0，失败为-1
int CSnapShotList::GetNodeInfo(int Index, EX_SNAPSHOTINFO * hNode)
{

	//int ret = -1;
	int i = 0;

	PNode *tmp;

	tmp = m_head;

	if (m_cur != NULL && ((m_nIndex + 1) == Index))
	{
		tmp = m_cur->next;
		i = m_nIndex + 1;
	}

	if (m_cur != NULL && ((m_nIndex) == Index))
	{
		tmp = m_cur;
		i = m_nIndex;
	}

	while (NULL != tmp)
	{
		if (i == Index)
		{
			memcpy(hNode, &tmp->hNode, sizeof (EX_SNAPSHOTINFO));
			m_cur = tmp;
			m_nIndex = i;
			return 0;
		}

		i++;
		tmp = tmp->next;
	}

	TRACE("Index = %d,m_NodeCount = %d\n", Index, m_NodeCount);
	return -1;
}

//!\brief 获取节点个数
unsigned int CSnapShotList::GetNodeCount()
{
	//print();
	return m_NodeCount;
}

//!\brief 打印
void CSnapShotList::print()
{
	int i = 0;

	PNode *tmp;

	tmp = m_head;

	DEBUG_MSG("%p节点信息\n", this);

	while (NULL != tmp)
	{
		i++;
		DEBUG_MSG("节点%d:ch:%d\n", i, tmp->hNode.m_Channel);
		tmp = tmp->next;
	}
}

//初始化
CSnapShotList::CSnapShotList()
{
	m_head = NULL;
	m_tail = NULL;
	m_NodeCount = 0;
	m_nMarkSum = 0;
	m_cur = NULL;
	m_nIndex = -1;
	DEBUG_MSG("sizeof(EX_SNAPSHOTINFO) = %d\n", sizeof (EX_SNAPSHOTINFO));
}

CSnapShotList::~CSnapShotList()
{
	while (NULL != m_head)
	{
		Delete(m_head->hNode);
	}
}
