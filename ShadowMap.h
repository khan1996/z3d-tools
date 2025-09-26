// ShadowMap.h: interface for the CShadowMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHADOWMAP_H__13E99194_5645_4093_87D0_B4BCD22B0562__INCLUDED_)
#define AFX_SHADOWMAP_H__13E99194_5645_4093_87D0_B4BCD22B0562__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <d3dx8.h>

class CShadowMap  
{
public:
	void End(LPDIRECT3DDEVICE8 pd3dDevice);
	void Begin(LPDIRECT3DDEVICE8 pd3dDevice);
	LPDIRECT3DSURFACE8 m_pShadowMapColorSurface,m_pShadowMapZSurface;
	LPDIRECT3DTEXTURE8 m_pShadowMapColorTexture,m_pShadowMapZTexture;	

	static LPDIRECT3DSURFACE8 m_pTempRenderSurface;		
	static LPDIRECT3DSURFACE8 m_pTempRenderZBuffer;
	static D3DVIEWPORT8 m_pTempViewPort;

	D3DVIEWPORT8 m_Viewport;
	
	LPDIRECT3DTEXTURE8 GetShadowMapZTexture(){return m_pShadowMapZTexture;};
	LPDIRECT3DTEXTURE8 GetShadowMapColorTexture(){return m_pShadowMapColorTexture;};
	void Create(long SizeX,long SizeY);
	CShadowMap();
	virtual ~CShadowMap();

};

#endif // !defined(AFX_SHADOWMAP_H__13E99194_5645_4093_87D0_B4BCD22B0562__INCLUDED_)
