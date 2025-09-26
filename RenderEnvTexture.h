// RenderEnvTexture.h: interface for the CRenderEnvTexture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RENDERENVTEXTURE_H__39EF09BE_861F_466D_9D47_63BFFE575865__INCLUDED_)
#define AFX_RENDERENVTEXTURE_H__39EF09BE_861F_466D_9D47_63BFFE575865__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <D3DX8.h>
#include "Vertex.h"
#include "Texture.h"

class CRenderEnvTexture  
{
	ID3DXRenderToEnvMap* m_pRenderToEnvMap;		
	CTexture m_SkyTexture[6];
	bool m_bCube;
public:
	IDirect3DCubeTexture8* m_pCubeMap;
	IDirect3DTexture8* m_pSphereMap;
	void Create(LPDIRECT3DDEVICE8 pd3dDevice,char strSkyTexturename[256],char strTexturePath[256],bool bCube=true);
	void RenderSkymap(LPDIRECT3DDEVICE8 pd3dDevice);
	void RenderCubeMap(LPDIRECT3DDEVICE8 pd3dDevice);
	LPDIRECT3DBASETEXTURE8 GetTexture()
	{
		if(m_bCube)
			return (LPDIRECT3DBASETEXTURE8)m_pCubeMap;
		return (LPDIRECT3DBASETEXTURE8)m_pSphereMap;
	};
	CRenderEnvTexture();
	virtual ~CRenderEnvTexture();

};

#endif // !defined(AFX_RENDERENVTEXTURE_H__39EF09BE_861F_466D_9D47_63BFFE575865__INCLUDED_)
