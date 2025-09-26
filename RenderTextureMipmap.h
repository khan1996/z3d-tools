// RenderTextureMipmap.h: interface for the CRenderTextureMipmap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RENDERTEXTUREMIPMAP_H__C4579775_6CD9_4F2B_A3CC_7E128F86C7D3__INCLUDED_)
#define AFX_RENDERTEXTUREMIPMAP_H__C4579775_6CD9_4F2B_A3CC_7E128F86C7D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RenderTexture.h"
#include <vector>

class CRenderTextureMipmap  
{
	std::vector<LPDIRECT3DTEXTURE8> m_pRenderTextureList;
	std::vector<LPDIRECT3DSURFACE8> m_pRenderSurfaceList;	
	LPDIRECT3DSURFACE8 m_pRenderZBuffer;	
	int m_nSize;
public:
	LPDIRECT3DSURFACE8 GetSurface();
	void GenerateMipmap(LPDIRECT3DDEVICE8 pd3dDevice);
	LPDIRECT3DTEXTURE8 GetTexture(int nTexture=0);
	LPDIRECT3DSURFACE8 GetZBuffer(){return m_pRenderZBuffer;};
	int GetTextureCount(){return (int)m_pRenderTextureList.size();};
	void Create(int nSizeX,int nSizeY);
	CRenderTextureMipmap();
	virtual ~CRenderTextureMipmap();

};

#endif // !defined(AFX_RENDERTEXTUREMIPMAP_H__C4579775_6CD9_4F2B_A3CC_7E128F86C7D3__INCLUDED_)
