// FullSceneEffect.h: interface for the CFullSceneEffect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FULLSCENEEFFECT_H__3E5DCE3F_4B17_40D9_A830_FCE7E55D490A__INCLUDED_)
#define AFX_FULLSCENEEFFECT_H__3E5DCE3F_4B17_40D9_A830_FCE7E55D490A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <d3dx8.h>
#include <d3d8.h>
#include <vector>

#include "Vertex.h"
#include "FullSceneShader.h"

enum FULLSCENE_EFFECTS {
	FULLSCENE_MOTIONBLUR = 0,
	FULLSCENE_BRIGHT,
	FULLSCENE_BLACK,
//	FULLSCENE_CONTRAST,
	FULLSCENE_EDGE,
	FULLSCENE_NEGATIVE,
	FULLSCENE_SEPIA,
//	FULLSCENE_SOLAR,
	FULLSCENE_EDGE2,
	FULLSCENE_EDGE3,
	FULLSCENE_SHARPEN,
//	FULLSCENE_LEVEL,
	FULLSCENE_SATURATION,
	FULLSCENE_SATURATION2,
	FULLSCENE_NUMS,	
};
#define FULLSCENE_BLURTEXNUM 2
#define FULLSCENE_BLURTEXSIZE 1024	
#define FULLSCENE_RENDERSIZE 1024


class CFullSceneEffect  
{
protected:
	LPDIRECT3DDEVICE8	m_pDevice;
	D3DCAPS8 m_pCaps;
	bool				m_bPixelShader;

/*	LPDIRECT3DTEXTURE8	m_pBlurTexture[FULLSCENE_BLURTEXNUM];
	LPDIRECT3DSURFACE8	m_pBlurTextureSurf[FULLSCENE_BLURTEXNUM];
	int					m_iCurrentBlurTex;*/
	int					m_iBlurTexUpdateFrame;	// BlurTex Update 간격
	int					m_iBlurTexUpdateCount;


	/*LPDIRECT3DTEXTURE8  m_pRenderTexture;
	LPDIRECT3DSURFACE8	m_pRenderTextureSurface;
	*/
	LPDIRECT3DTEXTURE8  m_pRenderTexture[FULLSCENE_BLURTEXNUM + 1];
	LPDIRECT3DSURFACE8	m_pRenderTextureSurface[FULLSCENE_BLURTEXNUM + 1];
	int					m_iCurrentTextureNum;
	int					m_iMaxCountNum;



	LPDIRECT3DSURFACE8	m_pRenderZSurface;

	LPDIRECT3DSURFACE8	m_pFrameBuffer;
	LPDIRECT3DSURFACE8	m_pFrameZBuffer;
	bool				m_bFlag[FULLSCENE_NUMS];
	bool				m_bLockFlag[FULLSCENE_NUMS];
	D3DVIEWPORT8		m_FrameViewport;
	D3DVIEWPORT8		m_RenderViewport;
	D3DVIEWPORT8		m_ClearViewport;

	int					m_iFrameWidth;
	int					m_iFrameHeight;

	int					m_iRenderWidth;
	int					m_iRenderHeight;

	LPDIRECT3DVERTEXBUFFER8  m_pVertexBuffer;

	std::vector<CFullSceneShader *> m_lstEffects;
	int					m_iBlurFirstUpdate;
	bool				m_bFullSceneEffect;

public:
	CFullSceneEffect();
	virtual ~CFullSceneEffect();
	
	void Init(LPDIRECT3DDEVICE8 lpDevice);
	
	void CreateVertexBuffer();

	HRESULT BeginRender(DWORD dwClearColor);
	HRESULT EndRender();
	void Render(int i = 0);
	void Update();	
	
	void SetFlag(int i,bool bFlag) { 
		if(!m_bLockFlag[i])		// lock 안걸려있어야 조작가능
			m_bFlag[i] = bFlag;
	
	}
	bool GetFlag(int i) {return m_bFlag[i];}

	void SetLockFlag(int i,bool bFlag) { m_bLockFlag[i] = bFlag;}
	bool GetLockFlag(int i) { return m_bLockFlag[i];}

	void SetBlurUpdateFrame(int iFrame) { m_iBlurTexUpdateFrame = iFrame;}
	bool GetEnable() { return m_bFullSceneEffect;}
	
	int GetRenderWidth() { return m_iRenderWidth;}
	int GetRenderHeight() { return m_iRenderHeight;}

	VOID SetFullSceneEffectEnable( bool bFullScene ) { m_bFullSceneEffect = bFullScene ; }
};

#endif // !defined(AFX_FULLSCENEEFFECT_H__3E5DCE3F_4B17_40D9_A830_FCE7E55D490A__INCLUDED_)
