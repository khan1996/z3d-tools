// FallScene.h: interface for the CFallScene class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FALLSCENE_H__6A50DFC3_6D31_40A1_AC38_148155D94CD9__INCLUDED_)
#define AFX_FALLSCENE_H__6A50DFC3_6D31_40A1_AC38_148155D94CD9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "SceneNode.h"
#include "SectorDefine.h"
#include <Vertex.h>
#include <Texture.h>
#include "SectorFallMap.h"

class CFallScene : public CSceneNode
{
	/*
	CTexture m_FallPartTexture;
	LPDIRECT3DVERTEXBUFFER8 m_pFallVertex;
	LPDIRECT3DINDEXBUFFER8 m_pFallIndices;
	vector3 m_vecPos[MAX_FALLPARTICLE];
	vector3 m_vecDir[MAX_FALLPARTICLE];
	float m_fVelocity[MAX_FALLPARTICLE];	
	long m_Collition[MAX_FALLPARTICLE];	
	*/
	LVertex m_FallVertex[30];
	CSectorFallMap *m_MapFall;
	static CTexture m_WaterFallTexture;
public:
	void MakeFallScene(CSectorFallMap *MapFall);
	void Render(LPDIRECT3DDEVICE8 pd3dDevice);
	void Update();
	void Create();
	CFallScene();
	virtual ~CFallScene();
};

#endif // !defined(AFX_FALLSCENE_H__6A50DFC3_6D31_40A1_AC38_148155D94CD9__INCLUDED_)
