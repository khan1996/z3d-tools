// GlareTexture.h: interface for the CGlareTexture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLARETEXTURE_H__18B36EBC_004F_4F55_A006_DE21B3FC4CCB__INCLUDED_)
#define AFX_GLARETEXTURE_H__18B36EBC_004F_4F55_A006_DE21B3FC4CCB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RenderTexture.h"
#include "D3DX8.h"
#include "3DMath.h"

class CGlareTexture  
{
	class QuadVertex
	{
	public:
		D3DXVECTOR3 Position;
        D3DXVECTOR2 Tex;
	};	

	class QuadVertex2
	{
	public:
		vector3 v;
		float w;
		float tu0,tv0;
		float tu1,tv1;
		float tu2,tv2;
		float tu3,tv3;
	};


	LPDIRECT3DTEXTURE8 m_pRenderTexture[2];	
	LPDIRECT3DSURFACE8 m_pRenderSurface[2];		
	static LPDIRECT3DSURFACE8 m_pRenderZBuffer;	
	DWORD m_BlurPixelShader,m_BlurVertexShader;	
	LPDIRECT3DVERTEXBUFFER8 m_pVertexBuffer;

	int m_nSize;
	int m_nFinalRenderTexture;
public:
	void GenerateGlareTexture2(LPDIRECT3DDEVICE8 pd3dDevice,LPDIRECT3DBASETEXTURE8 pTexture,vector3 vecNeighbor,int nDepth);
	void ProcedualGenerateGlareTexture(LPDIRECT3DDEVICE8 pd3dDevice, LPDIRECT3DBASETEXTURE8 pTexture,vector3 vecNeighbor,int nDepth);
	LPDIRECT3DTEXTURE8 GetTexture(){return m_pRenderTexture[m_nFinalRenderTexture];};
	
	void CreateAndWriteUVOffsets(int width, int height);
	void Create(int nSize);
	void GenerateGlareTexture(LPDIRECT3DDEVICE8 pd3dDevice,LPDIRECT3DBASETEXTURE8 pTexture,vector3 vecNeighbor,int nDepth);
	CGlareTexture();
	virtual ~CGlareTexture();
};

#endif // !defined(AFX_GLARETEXTURE_H__18B36EBC_004F_4F55_A006_DE21B3FC4CCB__INCLUDED_)
