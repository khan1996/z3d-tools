// RenderTexture.cpp: implementation of the CRenderTexture class.
//
//////////////////////////////////////////////////////////////////////

#include "RenderTexture.h"

#include "BaseGraphicsLayer.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
LPDIRECT3DSURFACE8 CRenderTexture::m_pRenderZBuffer;
LPDIRECT3DTEXTURE8 CRenderTexture::m_pCopyLockTexture;
LPDIRECT3DSURFACE8 CRenderTexture::m_pCopyLockSurface;

CRenderTexture::CRenderTexture()
{
	m_pRenderTexture=NULL;
	m_pRenderSurface=NULL;
}

CRenderTexture::~CRenderTexture()
{
	if(m_pRenderTexture)
		m_pRenderTexture->Release();	
	if(m_pRenderSurface)
		m_pRenderSurface->Release();		
	if(m_pRenderZBuffer)
	{
		m_pRenderZBuffer->Release();	
		m_pRenderZBuffer=NULL;
	}
}

void CRenderTexture::Create(long SizeX,long SizeY)
{ 
	if(SizeX==0 || SizeY==0)
		return;
	//D3DDISPLAYMODE mode;
	//BaseGraphicsLayer::GetDevice()->GetDisplayMode(&mode);			
	BaseGraphicsLayer::GetDevice()->CreateTexture(SizeX,SizeY,1,D3DUSAGE_RENDERTARGET,BaseGraphicsLayer::m_d3dpp.BackBufferFormat,D3DPOOL_DEFAULT,&m_pRenderTexture);
	if(m_pRenderTexture==NULL)
	{
		SizeX=0;
		SizeY=0;
		return;
	}
	m_pRenderTexture->GetSurfaceLevel(0, &m_pRenderSurface);

	if(m_pRenderZBuffer==NULL)
	{
		BaseGraphicsLayer::GetDevice()->CreateDepthStencilSurface(SizeX,SizeY,BaseGraphicsLayer::m_d3dpp.AutoDepthStencilFormat,D3DMULTISAMPLE_NONE,&m_pRenderZBuffer);	
		BaseGraphicsLayer::GetDevice()->CreateTexture(SizeX,SizeY,1,0,BaseGraphicsLayer::m_d3dpp.BackBufferFormat,D3DPOOL_MANAGED,&m_pCopyLockTexture);
		m_pCopyLockTexture->GetSurfaceLevel(0,&m_pCopyLockSurface);
	}
}

void CRenderTexture::Begin(LPDIRECT3DDEVICE8 pd3dDevice)
{		
	pd3dDevice->GetRenderTarget(&m_pTempRenderSurface);
	pd3dDevice->GetDepthStencilSurface(&m_pTempRenderZBuffer);
	pd3dDevice->GetViewport(&m_pTempViewPort);

	pd3dDevice->SetRenderTarget(m_pRenderSurface,m_pRenderZBuffer);			
	//pd3dDevice->SetRenderTarget(m_pRenderSurface,m_pTempRenderZBuffer);		
	
}

void CRenderTexture::End(LPDIRECT3DDEVICE8 pd3dDevice)
{	
	pd3dDevice->SetRenderTarget(m_pTempRenderSurface,m_pTempRenderZBuffer);	
	m_pTempRenderSurface->Release();
	m_pTempRenderZBuffer->Release();	
	pd3dDevice->SetViewport(&m_pTempViewPort);
	//pd3dDevice->GetViewport(&m_pTempViewPort);
}

void* CRenderTexture::Lock()
{
	D3DLOCKED_RECT pRect;
	//m_pRenderTexture->LockRect(0,&pRect,NULL,D3DLOCK_READONLY);
	D3DDISPLAYMODE mode;
	BaseGraphicsLayer::GetDevice()->GetDisplayMode(&mode);	
	RECT rect;
	rect.left=0;
	rect.right=256;
	rect.top=0;
	rect.bottom=256;
	POINT pt;
	pt.x=0;
	pt.y=0;
	BaseGraphicsLayer::GetDevice()->CopyRects(m_pRenderSurface,&rect,1,m_pCopyLockSurface,&pt);
	m_pCopyLockSurface->LockRect(&pRect,NULL,D3DLOCK_READONLY);
	return pRect.pBits;
	//return (void*)
}

void CRenderTexture::Unlock()
{
	m_pCopyLockSurface->UnlockRect();
}
