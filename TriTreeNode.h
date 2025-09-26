// TriTreeNode.h: interface for the TriTreeNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRITREENODE_H__96E4284E_C50A_4590_A894_C9836DCAFB9F__INCLUDED_)
#define AFX_TRITREENODE_H__96E4284E_C50A_4590_A894_C9836DCAFB9F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class TriTreeNode  
{
public:
	TriTreeNode *LeftChild;
	TriTreeNode *RightChild;
	TriTreeNode *BaseNeighbor;
	TriTreeNode *LeftNeighbor;
	TriTreeNode *RightNeighbor;

	TriTreeNode();
	virtual ~TriTreeNode();

};

#endif // !defined(AFX_TRITREENODE_H__96E4284E_C50A_4590_A894_C9836DCAFB9F__INCLUDED_)
