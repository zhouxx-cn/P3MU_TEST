
/*!
*\file   CPNodeTest.h 
*\brief  该类用于判断对象是否是已经存在的，还是已经被释放的
*\author cwf
*\date	  2009/03/24
*\version 0.1
*/
#include "HANDLE.h"
class CPNodeTest
{
  public:
	int Insert(const HANDLE hNode);
	int Delete(const HANDLE hNode);
	bool IsExist(const HANDLE hNode);
	void print();
	  CPNodeTest();
	 ~CPNodeTest();
  protected:
  private:
	typedef struct _PNode_
	{
		HANDLE hNode;
		struct _PNode_ *next;
	} PNode;
	PNode *m_head;
};
