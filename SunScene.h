// SunScene.h: interface for the CSunScene class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SUNSCENE_H__E24C3299_5FD3_4FBA_A987_E283D093A2FB__INCLUDED_)
#define AFX_SUNSCENE_H__E24C3299_5FD3_4FBA_A987_E283D093A2FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SceneNode.h"
#include <Vertex.h>
#include <Texture.h>
#include "SectorDefine.h"
#include "StateLog.h"

class CSunScene : public CSceneNode  
{
	float m_fFlareSize[MAX_FLARE];
	float m_fFlarePos[MAX_FLARE];	
	
	
	//LPDIRECT3DVERTEXBUFFER8 m_pSunVertex;
	TLVertex m_SunVertex[4];
	vector3 m_vecSunPosition;
	color m_FlareColor[MAX_FLARE];
	color m_LastFlareColor[2];
public:
	CTexture m_SunTexture;
	CTexture m_SunFlareTexture;
	CTexture m_FlareTexture[MAX_FLARE];

	CTexture m_MoonTexture;
	CTexture m_MoonFlareTexture;

	void RenderFlare(LPDIRECT3DDEVICE8 pd3dDevice);
	void SetSunDirection(vector3 &vecSunDir){m_vecSunPosition=vecSunDir;};
	void Render(LPDIRECT3DDEVICE8 pd3dDevice);

	void Reload();
	void Create();
	CSunScene();
	virtual ~CSunScene();

};

#endif // !defined(AFX_SUNSCENE_H__E24C3299_5FD3_4FBA_A987_E283D093A2FB__INCLUDED_)
