// WaterScene.h: interface for the CWaterScene class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WATERSCENE_H__1F05359E_CF6E_4316_BC19_753C0D0819A4__INCLUDED_)
#define AFX_WATERSCENE_H__1F05359E_CF6E_4316_BC19_753C0D0819A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SceneNode.h"
#include <Vertex.h>
#include "SectorDefine.h"
#include <Texture.h>
#include "RenderTexture.h"

const int RippleVertexCount=32;

class CWaterScene : public CSceneNode  
{	
	static long m_lWaterTexture;
	
	static float m_fWaveMove;
	static DWORD m_dwWaterShader;
	static DWORD m_dwWaterPixelShader;
	
	static LPDIRECT3DTEXTURE8 m_pBumpMap[MAX_WATERTEXTURE];
	static LPDIRECT3DCUBETEXTURE8 m_pCubeTexture;	
	static float m_fUpdateWaterTexture;

	//static DWORD m_dwWaterVertexShader;
	//static DWORD m_dwWaterPixelShader;
public:
	static CTexture m_WaterTexture[MAX_WATERTEXTURE];
	void RenderFlare(LPDIRECT3DDEVICE8 pd3dDevice);
	void CalcBasicVector3(Dot3Vertex *pVertex);
	void CreateWaterBumpTexture();
	static void RenderRipple(LPDIRECT3DDEVICE8 pd3dDevice);
	static void UpdateWave();

	static LPDIRECT3DVERTEXBUFFER8 m_pWaterVertexBuffer;
	static LPDIRECT3DTEXTURE8 m_WaterBumpMap;
	static void IncWaterTexture(float fUpdate);
	MultiVertex m_WaterVertex[4];
	
	static CRenderTexture m_pRelectionTexture;
	static CRenderTexture m_pRippleTexture;
	static TLVertex m_RippleVertex[RippleVertexCount*RippleVertexCount];
	static WORD m_RippleIndices[(RippleVertexCount-1)*(RippleVertexCount-1)*6];
	void DeterminWaterTexture(LPDIRECT3DDEVICE8 pd3dDevice,bool isRelection);
	void Render(LPDIRECT3DDEVICE8 pd3dDevice);
	void Create();
	CWaterScene();
	virtual ~CWaterScene();
};

#endif // !defined(AFX_WATERSCENE_H__1F05359E_CF6E_4316_BC19_753C0D0819A4__INCLUDED_)
