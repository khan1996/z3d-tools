// GrassManager.h: interface for the CGrassManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRASSMANAGER_H__9CA4057F_447A_4203_876A_E6ACA7C47081__INCLUDED_)
#define AFX_GRASSMANAGER_H__9CA4057F_447A_4203_876A_E6ACA7C47081__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GrassScene.h"

class CGrassManager  
{
	CGrassScene *m_pGrassNode;
	int m_NowPositionX,m_NowPositionY;
	float m_fGrassRot[20];
public:
	void RenderShadow(LPDIRECT3DDEVICE8 pd3dDevice,CRenderTexture &pShadowTexture);
	void Update(float fUpdate);
	void Render(LPDIRECT3DDEVICE8 pd3dDevice);
	void UpdateRange();
	void Create();
	CGrassManager();
	virtual ~CGrassManager();
	void Delete();


};

#endif // !defined(AFX_GRASSMANAGER_H__9CA4057F_447A_4203_876A_E6ACA7C47081__INCLUDED_)
