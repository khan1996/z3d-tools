// RenderTextureMipmap.cpp: implementation of the CRenderTextureMipmap class.
//
//////////////////////////////////////////////////////////////////////

#include "BaseGraphicsLayer.h"
#include "RenderTextureMipmap.h"
#include "Vertex.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRenderTextureMipmap::CRenderTextureMipmap()
{

}

CRenderTextureMipmap::~CRenderTextureMipmap()
{

}

void CRenderTextureMipmap::Create(int nSizeX, int nSizeY)
{	
	D3DDISPLAYMODE mode;
	BaseGraphicsLayer::GetDevice()->GetDisplayMode(&mode);		
	//BaseGraphicsLayer::GetDevice()->CreateTexture(SizeX,SizeY,1,D3DUSAGE_RENDERTARGET,mode.Format,D3DPOOL_DEFAULT,&m_pRenderTexture);
	//m_pRenderTexture->GetSurfaceLevel(0, &m_pRenderSurface);
	BaseGraphicsLayer::GetDevice()->CreateDepthStencilSurface(nSizeX,nSizeY,BaseGraphicsLayer::m_d3dpp.AutoDepthStencilFormat,D3DMULTISAMPLE_NONE,&m_pRenderZBuffer);		

	m_nSize=nSizeX;
	LPDIRECT3DTEXTURE8 pMipmapTexture;
	LPDIRECT3DSURFACE8 pRenderSurface;

	for(int i=nSizeX;i>=32;i=i>>1)
	{		
		BaseGraphicsLayer::GetDevice()->CreateTexture(i,i,1,D3DUSAGE_RENDERTARGET,mode.Format,D3DPOOL_DEFAULT,&pMipmapTexture);
		pMipmapTexture->GetSurfaceLevel(0, &pRenderSurface);
		m_pRenderTextureList.push_back(pMipmapTexture);
		m_pRenderSurfaceList.push_back(pRenderSurface);
	}
	/*
	for(int i=nSizeX;i>=32;i=i>>1)
	{
		CRenderTexture *AddNode=new CRenderTexture();
		AddNode->Create(i,i);
		m_TextureMipmap.Add(AddNode);
	}
	*/
}

LPDIRECT3DTEXTURE8 CRenderTextureMipmap::GetTexture(int nTexture)
{
	if((int)m_pRenderTextureList.size()>0)
	{		
		return m_pRenderTextureList[nTexture];
	}
	return NULL;
}

void CRenderTextureMipmap::GenerateMipmap(LPDIRECT3DDEVICE8 pd3dDevice)
{
	pd3dDevice->EndScene();
	LPDIRECT3DSURFACE8 m_pTempRenderSurface;		
	LPDIRECT3DSURFACE8 m_pTempRenderZBuffer;	
	pd3dDevice->GetRenderTarget(&m_pTempRenderSurface);
	pd3dDevice->GetDepthStencilSurface(&m_pTempRenderZBuffer);

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,D3DTA_DIFFUSE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,D3DTOP_SELECTARG1);

	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState( 3, D3DTSS_COLOROP,D3DTOP_DISABLE);

	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);

	
	TLVertex pVertex[4];		
	int TextureSize=m_nSize>>1;

	pVertex[0].v.x=0.0f;
	pVertex[1].v.x=0.0f;
	pVertex[2].v.x=(float)TextureSize;
	pVertex[3].v.x=(float)TextureSize;

	
	pVertex[1].v.y=0.0f;
	pVertex[3].v.y=0.0f;
	pVertex[0].v.y=(float)TextureSize;
	pVertex[2].v.y=(float)TextureSize;	
		
	pVertex[0].tu=0.0f;
	pVertex[1].tu=0.0f;

	pVertex[3].tu=1.0f;
	pVertex[2].tu=1.0f;

	pVertex[1].tv=0.0f;
	pVertex[3].tv=0.0f;

	pVertex[0].tv=1.0f;
	pVertex[2].tv=1.0f;		

	for(int i=0;i<4;i++)
	{
		pVertex[i].w=0.1f;
		pVertex[i].v.z=0.1f;
		pVertex[i].Diffuse.c=0xffff0000;
		pVertex[i].Specular.c=0xffffffff;
	}	

	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);	

	pd3dDevice->SetTexture(1,NULL);
	pd3dDevice->SetVertexShader(TLVERTEXFVF);	

	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);		
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

	
	for(int i=1;i<(int)m_pRenderTextureList.size();i++,TextureSize=TextureSize>>1)
	{		
		pVertex[2].v.x=(float)TextureSize;
		pVertex[3].v.x=(float)TextureSize;			
		pVertex[0].v.y=(float)TextureSize;
		pVertex[2].v.y=(float)TextureSize;	

		pd3dDevice->SetRenderTarget(m_pRenderSurfaceList[i],m_pTempRenderZBuffer);
		
		pd3dDevice->BeginScene();
		pd3dDevice->Clear(0,NULL,D3DCLEAR_TARGET,0xffffffff,1.0f,0);

		pd3dDevice->SetTexture(0,m_pRenderTextureList[0]);
		pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(TLVertex));
		pd3dDevice->EndScene();				

	}

	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);		
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);

	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);

	pd3dDevice->SetRenderTarget(m_pTempRenderSurface,m_pTempRenderZBuffer);	
	m_pTempRenderSurface->Release();
	m_pTempRenderZBuffer->Release();	

	pd3dDevice->BeginScene();
}

LPDIRECT3DSURFACE8 CRenderTextureMipmap::GetSurface()
{
	if((int)m_pRenderSurfaceList.size()>0)
	{
		return m_pRenderSurfaceList[0];
	}
	return NULL;
}
