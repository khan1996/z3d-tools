// SkyScene.h: interface for the CSkyScene class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKYSCENE_H__B949810B_EC65_46A5_B49C_99EC7C2B3D05__INCLUDED_)
#define AFX_SKYSCENE_H__B949810B_EC65_46A5_B49C_99EC7C2B3D05__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SceneNode.h"
#include "Texture.h"
#include <Vertex.h>
#include "SectorDefine.h"
#include "StateLog.h"

const float SKYSIZE=500000;
const long SKYDIVIDE=10;
const long SKYVERTEX=SKYDIVIDE+2;

const long STARVERTEX=200;


class CSkyScene : public CSceneNode  
{
	//D3DLVERTEX m_skymesh[SKYVERTEX];	
	CTexture m_SkyTexture[5];
	LVertex m_SkyVertex[SKYVERTEX];
	LVertex m_CloudVertex[MAX_CLOUD][SKYVERTEX];
	LVertex m_StarVertex[STARVERTEX];
	//LVertex m_SkyBoxVertex[
	//color m_FogLayer[MAX_CLOUD];
	float m_fFogLayerHeight[MAX_CLOUD];
public:
	void RenderForGlare(LPDIRECT3DDEVICE8 pd3dDevice);
	void Update(float fUpdate);
	static CTexture m_CloudTexture;
	void RenderCloud(LPDIRECT3DDEVICE8 pd3dDevice);
	void RenderSkyBox(LPDIRECT3DDEVICE8 pd3dDevice);
	void SetColor(DWORD dwColor,DWORD dwCenterColor,DWORD *dwLayerColor);
	void Render(LPDIRECT3DDEVICE8 pd3dDevice);
	void Create();
	CSkyScene();
	virtual ~CSkyScene();	
};

#endif // !defined(AFX_SKYSCENE_H__B949810B_EC65_46A5_B49C_99EC7C2B3D05__INCLUDED_)
