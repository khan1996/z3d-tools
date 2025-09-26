// LayerFogScene.cpp: implementation of the CLayerFogScene class.
//
//////////////////////////////////////////////////////////////////////

#include "LayerFogScene.h"
#include "SceneManager.h"
#include "GMMemory.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLayerFogScene::CLayerFogScene()
{
	m_pFogVertexBuffer=NULL;
}

CLayerFogScene::~CLayerFogScene()
{

}

void CLayerFogScene::Create()
{
	m_FogTexture.CreateEmpty(SIZE_FOGTEXTURE,SIZE_FOGTEXTURE);

	CSceneManager::GetDevice()->CreateVertexBuffer( sizeof(MultiVertex)*4,
													D3DUSAGE_WRITEONLY,MultiFVF,D3DPOOL_MANAGED,
													&m_pFogVertexBuffer);	
	MultiVertex *pFogVertex;
	m_pFogVertexBuffer->Lock( 0, 0, (BYTE**)&pFogVertex, 0 );
	pFogVertex[0].v.x=0.0f;
	pFogVertex[0].v.z=0.0f;	
	pFogVertex[0].diff.c=0xffffffff;
	pFogVertex[0].spec.c=0x0;
	pFogVertex[0].v.y=fFogHeight+fFogAddHeight;
	pFogVertex[0].tu=0.0f;
	pFogVertex[0].tv=0.0f;

	pFogVertex[1].v.x=SECTORSIZE;
	pFogVertex[1].v.z=0.0f;
	pFogVertex[1].diff.c=0xffffffff;
	pFogVertex[1].spec.c=0x0;
	pFogVertex[1].v.y=fFogHeight+fFogAddHeight;
	pFogVertex[1].tu=10.0f;
	pFogVertex[1].tv=0.0f;

	pFogVertex[2].v.x=0.0f;
	pFogVertex[2].v.z=SECTORSIZE;
	pFogVertex[2].diff.c=0xffffffff;
	pFogVertex[2].spec.c=0x0;
	pFogVertex[2].v.y=fFogHeight+fFogAddHeight;
	pFogVertex[2].tu=10.0f;
	pFogVertex[2].tv=10.0f;

	pFogVertex[3].v.x=SECTORSIZE;
	pFogVertex[3].v.z=SECTORSIZE;
	pFogVertex[3].diff.c=0xffffffff;
	pFogVertex[3].spec.c=0x0;
	pFogVertex[3].v.y=fFogHeight+fFogAddHeight;
	pFogVertex[3].tu=0.0f;
	pFogVertex[3].tv=10.0f;

	m_pFogVertexBuffer->Unlock();
}

void CLayerFogScene::BuildFogTexture(float fMax, float fMin)
{		
	LPDIRECT3DTEXTURE8 pFogTexture;
	m_FogTexture.GetTexture()->QueryInterface(IID_IDirect3DTexture8, (void**)&pFogTexture);	

	D3DLOCKED_RECT LockedRect;
	pFogTexture->LockRect(0, &LockedRect, NULL, 0);
	float fObjectHeight=fMax-fMin;
	float fVertexHeight,fVertexRange;

	matrix *matViewPosition=(matrix*)CSceneManager::GetCamera()->GetMatPosition();

	vector3 vecViewPos;
	vecViewPos.x=matViewPosition->_41;
	vecViewPos.y=matViewPosition->_42;
	vecViewPos.z=matViewPosition->_43;
	color FogColor;

	float fEyeHeight=vecViewPos.y;
	float fDeltaY,fDeltaX;	
	float fFog;
	DWORD *pFog=(DWORD*)LockedRect.pBits;

	for(int iy=0;iy<SIZE_FOGTEXTURE;iy++)
	{
		fVertexHeight=fObjectHeight*((float)iy/(float)SIZE_FOGTEXTURE)+fMin;
		for(int ix=0;ix<SIZE_FOGTEXTURE;ix++)
		{
			fVertexRange=fFogXRange*( (float)ix/(float)SIZE_FOGTEXTURE );				
			if(fEyeHeight>fFogHeight)			
			{				
				if(fVertexHeight<fFogHeight)
				{										
					fDeltaY = fFogHeight - fVertexHeight;					
					fDeltaX = fVertexRange;										
				}
				else
				{
					fDeltaY=0.0f;
					fDeltaX=0.0f;					
				}
			}
			else
			{				
				
				if(fVertexHeight<fFogHeight)
				{					
					float fDeltaA=fFogHeight-fEyeHeight;
					float fDeltaB=fFogHeight-fVertexHeight;					
					//fDeltaY = fabs(fDeltaA - fDeltaB);					
					fDeltaY=0.0f;
					fDeltaX = fVertexRange;
				}
				else
				{
					//fDeltaY=(fabs(fFogHeight-fEyeHeight))*(0.5);
					//fDeltaX=fDeltaY;					
					fDeltaY=0.0f;
					fDeltaX=0.0f;
				}								
			}		
			if(fDeltaY>fFogYRange || fDeltaX>fFogXRange)
			{
				fFog=1.0f;
			}			
			else
			{

				fFog=fDeltaY/fFogYRange+fDeltaX/fFogXRange;
			}
			if(fFog>0.80f)
				fFog=0.80f;
			FogColor=FogColor.Interpolation(color(0,0,0),color(255,255,255),fFog);			
			pFog[ix+iy*SIZE_FOGTEXTURE]=FogColor.c;						
		}
	}	
	pFogTexture->UnlockRect(NULL);
	pFogTexture->Release();
}

void CLayerFogScene::Render(LPDIRECT3DDEVICE8 pd3dDevice)
{
	matrix *matViewPosition=(matrix*)CSceneManager::GetCamera()->GetMatPosition();

	vector3 vecViewPos;
	vecViewPos.x=matViewPosition->_41;
	vecViewPos.y=matViewPosition->_42;
	vecViewPos.z=matViewPosition->_43;
	if(vecViewPos.y > fFogHeight)
		return;
	float fFogDepth=fFogHeight-vecViewPos.y;
	fFogDepth=fFogDepth/fFogXRange;
	if(fFogDepth>1.0f)
		fFogDepth=1.0f;
	color FogColor=color::Interpolation(color(0,0,0),color(255,255,255),fFogDepth);
	float FogRate=fabs(vecViewPos.y-fFogHeight);

	if(FogRate>fFogYRange)
	{
		FogRate=fFogYRange;
	}
	FogRate/=fFogYRange;
	MultiVertex *pFogVertex;
	m_pFogVertexBuffer->Lock( 0, 0, (BYTE**)&pFogVertex, 0 );			
	pFogVertex[0].diff.a=(COLORVALUE)(FogRate*(0xff*0.8f));
	pFogVertex[1].diff.a=(COLORVALUE)(FogRate*(0xff*0.8f));
	pFogVertex[2].diff.a=(COLORVALUE)(FogRate*(0xff*0.8f));
	pFogVertex[3].diff.a=(COLORVALUE)(FogRate*(0xff*0.8f));

	pFogVertex[0].tu+=0.0002f;
	pFogVertex[1].tu+=0.0002f;
	pFogVertex[2].tu+=0.0002f;
	pFogVertex[3].tu+=0.0002f;

	m_pFogVertexBuffer->Unlock();	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE | D3DTA_COMPLEMENT );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
    pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	
	
	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	pd3dDevice->SetStreamSource(0, m_pFogVertexBuffer, sizeof(SimpleVertex));
    pd3dDevice->SetVertexShader(MultiFVF);
	pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2 );
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );
	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);		
}
