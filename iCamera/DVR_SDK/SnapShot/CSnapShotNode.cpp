
/*!
*\file   CSnapShotNode.h 
*\brief  该类用于判断对象是否是已经存在的，还是已经被释放的
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/
//#include <iostream.h>
#include "CSnapShotNode.h"
#include "debug.h"

#include "SnapShot_global.h"

//!\brief 添加一个节点
//!\param HANDLE hNode 节点的指针
//!\return  成功为0，失败为-1
int CSnapShotNode::Insert(const HANDLE hNode)
{
	int ret = -1;

	PNode *tmp, *addNode;

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

	addNode->hNode = hNode;
	addNode->next = NULL;

	if (NULL == m_head)
	{
		m_head = addNode;
	}
	else
	{
		tmp = m_head;

		while (NULL != tmp->next)
		{
			tmp = tmp->next;
		}

		tmp->next = addNode;
	}
	return 0;
}

//!\brief 删除一个节点
//!\param HANDLE hNode 节点的指针
//!\return  成功为0，失败为-1
int CSnapShotNode::Delete(const HANDLE hNode)
{
	int ret = -1;

	PNode *tmp, *delNode;

	if (NULL == m_head)
	{
		ERROR_MSG("节点不存在\n");
		return ret;
	}
	else if (hNode == m_head->hNode)
	{
		tmp = m_head;
		m_head = m_head->next;
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
			if (hNode == tmp->next->hNode)
			{
				delNode = tmp->next;
				tmp->next = tmp->next->next;
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

//!\brief 判断节点是否存在
//!\param HANDLE hNode 节点的指针
//!\return  成功为0，失败为-1
bool CSnapShotNode::IsExist(const HANDLE hNode)
{
	bool ret = false;

	PNode *tmp;

	tmp = m_head;

	while (NULL != tmp)
	{
		if (hNode == tmp->hNode)
		{
			return true;
		}

		tmp = tmp->next;
	}

	DEBUG_MSG("判断指针%p\n", hNode);
	print();
	return ret;
}

//!\brief 打印
void CSnapShotNode::print()
{

	PNode *tmp;

	tmp = m_head;

	DEBUG_MSG("%p节点信息\n", this);

	while (NULL != tmp)
	{
		DEBUG_MSG("节点%p node %p\n", tmp, tmp->hNode);
		tmp = tmp->next;
	}
}

CSnapShotNode::CSnapShotNode()
{
	m_head = NULL;
}

CSnapShotNode::~CSnapShotNode()
{
	while (NULL != m_head)
	{
		Delete(m_head->hNode);
		m_head->hNode = NULL;
	}
}
