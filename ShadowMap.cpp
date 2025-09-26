// ShadowMap.cpp: implementation of the CShadowMap class.
//
//////////////////////////////////////////////////////////////////////

#include "ShadowMap.h"
#include "BaseGraphicsLayer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//LPDIRECT3DSURFACE8 CShadowMap::m_pShadowMapColorSurface=NULL;
//LPDIRECT3DSURFACE8 CShadowMap::m_pShadowMapZSurface=NULL;
//LPDIRECT3DTEXTURE8 CShadowMap::m_pShadowMapColorTexture=NULL;
//LPDIRECT3DTEXTURE8 CShadowMap::m_pShadowMapZTexture=NULL;

LPDIRECT3DSURFACE8 CShadowMap::m_pTempRenderSurface;		
LPDIRECT3DSURFACE8 CShadowMap::m_pTempRenderZBuffer;
D3DVIEWPORT8 CShadowMap::m_pTempViewPort;
//D3DVIEWPORT8 CShadowMap::m_Viewport;


CShadowMap::CShadowMap()
{	

}

CShadowMap::~CShadowMap()
{

}

void CShadowMap::Create(long SizeX, long SizeY)
{
	if( BaseGraphicsLayer::m_d3dpp.BackBufferFormat==D3DFMT_A8R8G8B8 ||
		BaseGraphicsLayer::m_d3dpp.BackBufferFormat==D3DFMT_X8R8G8B8 )
	{
		BaseGraphicsLayer::GetDevice()->CreateTexture(SizeX,SizeY,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&m_pShadowMapColorTexture);
		BaseGraphicsLayer::GetDevice()->CreateTexture(SizeX,SizeY,1,D3DUSAGE_DEPTHSTENCIL,D3DFMT_D24S8,D3DPOOL_DEFAULT,&m_pShadowMapZTexture);
	}
	if( BaseGraphicsLayer::m_d3dpp.BackBufferFormat==D3DFMT_R5G6B5)
	{
		BaseGraphicsLayer::GetDevice()->CreateTexture(SizeX,SizeY,1,D3DUSAGE_RENDERTARGET,D3DFMT_R5G6B5,D3DPOOL_DEFAULT,&m_pShadowMapColorTexture);
		BaseGraphicsLayer::GetDevice()->CreateTexture(SizeX,SizeY,1,D3DUSAGE_DEPTHSTENCIL,D3DFMT_D16,D3DPOOL_DEFAULT,&m_pShadowMapZTexture);
	}	

	m_pShadowMapColorTexture->GetSurfaceLevel(0,&m_pShadowMapColorSurface);
	m_pShadowMapZTexture->GetSurfaceLevel(0,&m_pShadowMapZSurface);
	
	m_Viewport.X=0;
	m_Viewport.Y=0;
	m_Viewport.Width=SizeX;
	m_Viewport.Height=SizeY;
	m_Viewport.MinZ=0.0f;
	m_Viewport.MaxZ=1.0f;

}

void CShadowMap::Begin(LPDIRECT3DDEVICE8 pd3dDevice)
{
	pd3dDevice->GetViewport(&m_pTempViewPort);
	pd3dDevice->GetRenderTarget(&m_pTempRenderSurface);
	pd3dDevice->GetDepthStencilSurface(&m_pTempRenderZBuffer);
	pd3dDevice->SetRenderTarget(m_pShadowMapColorSurface,m_pShadowMapZSurface);			
	
	pd3dDevice->SetViewport(&m_Viewport);
}

void CShadowMap::End(LPDIRECT3DDEVICE8 pd3dDevice)
{	
	pd3dDevice->SetRenderTarget(m_pTempRenderSurface,m_pTempRenderZBuffer);	
	pd3dDevice->SetViewport(&m_pTempViewPort);
	m_pTempRenderSurface->Release();
	m_pTempRenderZBuffer->Release();	
}
