// ObjectScene.h: interface for the CObjectScene class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECTSCENE_H__27A58A60_BFA1_4AC4_A25F_AA84DFAF0A74__INCLUDED_)
#define AFX_OBJECTSCENE_H__27A58A60_BFA1_4AC4_A25F_AA84DFAF0A74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SceneNode.h"
#include "MeshObject.h"
#include "BaseDataDefine.h"
#include "./NMesh.h"

class CObjectScene : public CSceneNode  
{
public:
	long m_ObjectID;
	bool m_isAlpha,m_isLight;
	bool m_isShadowed;
	bool m_bInit;

	char m_strObjectName[MAX_NAMEBUFFER];
	CROSSM::CNMesh *m_pObject;
	
	
	CObjectScene();
	virtual ~CObjectScene();

};
/*
class CObjectScene : public CSceneNode  
{	
public:
	long m_ObjectID;
	bool m_isAlpha,m_isLight;
	bool m_isShadowed;
	bool m_bInit;

	char m_strObjectName[MAX_NAMEBUFFER];
	CMeshObject *m_pObject;
	
	D3DLIGHT8 m_CurrentLight[4];
	int		  m_iCurrentLight;
	
	CObjectScene();
	virtual ~CObjectScene();
};*/

#endif // !defined(AFX_OBJECTSCENE_H__27A58A60_BFA1_4AC4_A25F_AA84DFAF0A74__INCLUDED_)
