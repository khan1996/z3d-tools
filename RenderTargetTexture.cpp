// RenderTargetTexture.cpp: implementation of the CRenderTargetTexture class.
//
//////////////////////////////////////////////////////////////////////

#include "RenderTargetTexture.h"
#include "BaseGraphicsLayer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LPDIRECT3DTEXTURE8 CRenderTargetTexture::m_pTexture;

CRenderTargetTexture::CRenderTargetTexture()
{

}

CRenderTargetTexture::~CRenderTargetTexture()
{

}

void CRenderTargetTexture::Create(LPDIRECT3DDEVICE8 pd3dDevice,int nSizeX,int nSizeY)
{
	pd3dDevice->CreateTexture(nSizeX,nSizeY,1,0,BaseGraphicsLayer::m_d3dpp.BackBufferFormat,D3DPOOL_DEFAULT,&m_pTexture);	
}

LPDIRECT3DTEXTURE8 CRenderTargetTexture::GetTexture(LPDIRECT3DDEVICE8 pd3dDevice,RECT rcCopy)
{	
	LPDIRECT3DSURFACE8 pSurface;
	m_pTexture->GetSurfaceLevel(0,&pSurface);	

	LPDIRECT3DSURFACE8 pBackBuffer;
	pd3dDevice->GetBackBuffer(0,D3DBACKBUFFER_TYPE_MONO,&pBackBuffer);
	
	POINT pt={0,0};
	pd3dDevice->CopyRects(pBackBuffer,&rcCopy,1,pSurface,&pt);	
	pSurface->Release();
	pBackBuffer->Release();		
	
	return m_pTexture;
}
