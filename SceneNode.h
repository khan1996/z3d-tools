// SceneNode.h: interface for the CSceneNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENENODE_H__A6943025_3041_4B98_BADC_35C900B77A94__INCLUDED_)
#define AFX_SCENENODE_H__A6943025_3041_4B98_BADC_35C900B77A94__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <3DMath.h>

class CSceneNode  
{
public:	
	bool isAABBCulling();
	void Init();
	bool isCulling();
	vector3 m_vecMaxBox,m_vecMinBox;
	matrix m_AccumulateTM;
	matrix m_TM;		

	float m_fRad;
	CSceneNode *m_pParent;
	CSceneNode *m_pNext;
	CSceneNode *m_pChild;
	CSceneNode();
	virtual ~CSceneNode();
};

#endif // !defined(AFX_SCENENODE_H__A6943025_3041_4B98_BADC_35C900B77A94__INCLUDED_)
