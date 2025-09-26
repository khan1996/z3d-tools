// RenderTexture.h: interface for the CRenderTexture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RENDERTEXTURE_H__0EC50F46_F5C3_4873_AC41_9702AF503767__INCLUDED_)
#define AFX_RENDERTEXTURE_H__0EC50F46_F5C3_4873_AC41_9702AF503767__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <d3dx8.h>

class CRenderTexture  
{	
	LPDIRECT3DTEXTURE8 m_pRenderTexture;	
	LPDIRECT3DSURFACE8 m_pRenderSurface;		
	static LPDIRECT3DSURFACE8 m_pRenderZBuffer;	

	static LPDIRECT3DTEXTURE8 m_pCopyLockTexture;
	static LPDIRECT3DSURFACE8 m_pCopyLockSurface;	

	LPDIRECT3DSURFACE8 m_pTempRenderSurface;		
	LPDIRECT3DSURFACE8 m_pTempRenderZBuffer;
	D3DVIEWPORT8 m_pTempViewPort;	
public:
	void Unlock();
	void* Lock();
	void Begin(LPDIRECT3DDEVICE8 pd3dDevice);	
	void End(LPDIRECT3DDEVICE8 pd3dDevice);	
	LPDIRECT3DTEXTURE8 GetTexture(){return m_pRenderTexture;};
	LPDIRECT3DSURFACE8 GetSurface(){return m_pRenderSurface;};
	long m_SizeX,m_Sizey;
	void Create(long SizeX,long SizeY);
	CRenderTexture();
	virtual ~CRenderTexture();
};

#endif // !defined(AFX_RENDERTEXTURE_H__0EC50F46_F5C3_4873_AC41_9702AF503767__INCLUDED_)
