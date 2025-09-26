// CloudScene.h: interface for the CCloudScene class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLOUDSCENE_H__8881B271_2D89_4DE6_884E_1C2B0277D447__INCLUDED_)
#define AFX_CLOUDSCENE_H__8881B271_2D89_4DE6_884E_1C2B0277D447__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SceneNode.h"
#include "Texture.h"
#include <Vertex.h>
#include "SectorDefine.h"
#include "StateLog.h"
#include "RenderTexture.h"

const int NUMCLOUDOCTAVE=4;

const float CLOUDSIZE=500000.0f;
const long CLOUDDIVIDE=10;
const long CLOUDVERTEX=CLOUDDIVIDE+2;

class CCloudScene : public CSceneNode  
{
	float m_fTrans[NUMCLOUDOCTAVE];
	CTexture m_CloudOctaveTexture[NUMCLOUDOCTAVE];
	LVertex m_CloudVertex[CLOUDVERTEX];	
public:	
	void SetColor(DWORD dwColor,DWORD dwCenterColor,DWORD *dwLayerColor);
	CRenderTexture m_CloudOctaveBlendTexture;
	void Update(LPDIRECT3DDEVICE8 pd3dDevice,float fTime);
	void Render(LPDIRECT3DDEVICE8 pd3dDevice);
	float Noise3D(int x, int y, int z);
	void Create();
	CCloudScene();
	virtual ~CCloudScene();
};

#endif // !defined(AFX_CLOUDSCENE_H__8881B271_2D89_4DE6_884E_1C2B0277D447__INCLUDED_)
