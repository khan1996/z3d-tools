// RenderTargetTexture.h: interface for the CRenderTargetTexture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RENDERTARGETTEXTURE_H__2704A75E_C01D_4D6D_86C0_08F1D6E224AB__INCLUDED_)
#define AFX_RENDERTARGETTEXTURE_H__2704A75E_C01D_4D6D_86C0_08F1D6E224AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <d3d8.h>

class CRenderTargetTexture  
{
	static LPDIRECT3DTEXTURE8 m_pTexture;
public:
	static LPDIRECT3DTEXTURE8 GetTexture(LPDIRECT3DDEVICE8 pd3dDevice,RECT rcCopy);
	static void Create(LPDIRECT3DDEVICE8 pd3dDevice,int nSizeX,int nSizeY);
	CRenderTargetTexture();
	virtual ~CRenderTargetTexture();

};

#endif // !defined(AFX_RENDERTARGETTEXTURE_H__2704A75E_C01D_4D6D_86C0_08F1D6E224AB__INCLUDED_)
