// LightEffectManager.h: interface for the CLightEffectManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTEFFECTMANAGER_H__94D2B529_2919_44A4_BCB1_145A7831704B__INCLUDED_)
#define AFX_LIGHTEFFECTMANAGER_H__94D2B529_2919_44A4_BCB1_145A7831704B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RenderTextureMipmap.h"
#include "RenderTexture.h"
#include "GlareTexture.h"
#include "MeshObject.h"
#include "RenderEnvTexture.h"
#include "ShadowMap.h"

class CLightEffectManager  
{	
public:

	static int m_RenderSizeX;
	static int m_RenderSizeY;
	static void RenderDOFZBuffer(LPDIRECT3DDEVICE8 pd3dDevice);

	static DWORD m_DepthAlphaShader;

	static bool m_FullSceneAnti;	

	static DWORD m_dwMakeZBufferPixelShader;

	static CRenderTextureMipmap m_GlareTexture;
	static CShadowMap m_FullSceneTexture;
	static CRenderTextureMipmap m_SpecularGlareTexture;
	static CRenderTexture m_GlareCompositeTexture;	
	static CRenderTexture m_DOFZBuffer;
	static CRenderTexture m_NearDOFTexture;
	static CGlareTexture m_DOFSmoothTexture;

	static CGlareTexture m_SmoothTexture;	

	static LPDIRECT3DSURFACE8 m_pTempRenderSurface;		
	static LPDIRECT3DSURFACE8 m_pTempRenderZBuffer;	
	static int m_nScreenSize;

	class CGlareNode
	{
	public:
		CMeshObject *m_pMeshObject;
		matrix m_matPosition;
		bool m_bCubeEnv;
	};
	static List<CGlareNode> m_GlareList;
	static CTexture m_CharcterEnvTexture;
	static LPDIRECT3DCUBETEXTURE8 m_LightCubeTexture;

	static CRenderEnvTexture m_DiffuseLightCubeTexture;
	static CRenderEnvTexture m_SpecularLightTexture;

	static CRenderTexture m_SpGlareTexture;	


	void RenderFullSceneTextureEnd(LPDIRECT3DDEVICE8 pd3dDevice);
	void RenderFullSceneAnti(LPDIRECT3DDEVICE8 pd3dDevice);
	void RenderFullSceneGlare(LPDIRECT3DDEVICE8 pd3dDevice);
	void RenderSpecularGlareTexture(LPDIRECT3DDEVICE8 pd3dDevice);
	void RenderGlareTexture(LPDIRECT3DDEVICE8 pd3dDevice);
	void RenderGlareScene(LPDIRECT3DDEVICE8 pd3dDevice);
	void RenderFullSceneTextureBegin(LPDIRECT3DDEVICE8 pd3dDevice);
	void PrepareLightingEnvmap(LPDIRECT3DDEVICE8 pd3dDevice);
	void Create(int nSize);
	CLightEffectManager();
	virtual ~CLightEffectManager();

};

#endif // !defined(AFX_LIGHTEFFECTMANAGER_H__94D2B529_2919_44A4_BCB1_145A7831704B__INCLUDED_)
