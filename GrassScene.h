// GrassScene.h: interface for the CGrassScene class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRASSSCENE_H__9CB57CBD_783D_427E_95F8_EB4C27501B03__INCLUDED_)
#define AFX_GRASSSCENE_H__9CB57CBD_783D_427E_95F8_EB4C27501B03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MeshObject.h"

#include "SectorDefine.h"
#include "SceneNode.h"
#include "Vertex.h"
#include "RenderTexture.h"

class GrassExtVertex 
{
public:
	vector3 v;
	float fw0,fw1;
	float index0,index1;
	float tu,tv;	
	float fColorRate;
};

class CGrassScene  : public CSceneNode  
{
	vector3 *m_vecGrassPos;
	unsigned char *m_GrassShadow;
	unsigned char *m_GrassKind;	
	static CMeshObject *m_NormalGrass[5];
	int m_cGrassElement;
	vector3 m_MinBox,m_MaxBox;	

	float m_fGrassRot;

	LPDIRECT3DVERTEXBUFFER8 m_pGrassVertexBuffer;
	LPDIRECT3DINDEXBUFFER8 m_pGrassIndicesBuffer;

	static DWORD m_dwGrassVertexShader;

	DWORD	m_dwAlphaFactor;
	DWORD	m_dwAlphaUnit;
	DWORD	m_dwLimit;


public:	
	void RenderShadow(LPDIRECT3DDEVICE8 pd3dDevice,CRenderTexture &pShadowTexture);
	static CTexture *m_NormalTexture;
	void Update(float fUpdate);
	int m_GrassPosX,m_GrassPosY;
	void Generate(float fx,float fz,int cGrassElement);
	void Render(LPDIRECT3DDEVICE8 pd3dDevice);
	void Create();
	void UpdateRange();
	CGrassScene();
	virtual ~CGrassScene();	
	void UnloadData();

};

#endif // !defined(AFX_GRASSSCENE_H__9CB57CBD_783D_427E_95F8_EB4C27501B03__INCLUDED_)
