#include "Z3DGlowHandler.h"
#include "misc.h"
#include "matrix.h"
#include "GMMemory.h"


#define Z3D_GLOW_MAX_LEVEL 7


IDirect3DDevice8* CZ3DGlowHandler::ms_pDevice = NULL;
IDirect3DTexture8* CZ3DGlowHandler::ms_pRenderTargetTexture = NULL;
IDirect3DSurface8* CZ3DGlowHandler::ms_pRenderTargetDepth = NULL;

IDirect3DSurface8* CZ3DGlowHandler::ms_pOldBackBuffer = NULL;
IDirect3DSurface8* CZ3DGlowHandler::ms_pOldDepthBuffer = NULL;

Z3D_GLOW_CONSTANTS CZ3DGlowHandler::ms_aGlowParameter[Z3D_GLOW_MAX_LEVEL];
std::list< CZ3DGlowHandler* > CZ3DGlowHandler::ms_listInstance;

CUSTOM_TLVERTEX CZ3DGlowHandler::ms_Vert[4];

D3DVIEWPORT8 CZ3DGlowHandler::ms_RenderViewport;
D3DVIEWPORT8 CZ3DGlowHandler::ms_SaveViewport;

D3DMATRIX CZ3DGlowHandler::ms_RenderProjectionMatrix;
D3DMATRIX CZ3DGlowHandler::ms_SaveProjectionMatrix;

bool CZ3DGlowHandler::ms_b16bitMode = false;
bool CZ3DGlowHandler::ms_bSkipGlow = false;


CZ3DGlowHandler::CZ3DGlowHandler()
{
	SetLevel(0);

	ms_listInstance.push_back( this );
	m_itrThis = --ms_listInstance.end();
}

CZ3DGlowHandler::~CZ3DGlowHandler()
{
	ms_listInstance.erase( m_itrThis );
}

void CZ3DGlowHandler::SetLevel( int nLevel )
{
	if( nLevel <= Z3D_GLOW_MAX_LEVEL )
	{
		m_nLevelIndex = nLevel;
	}
	else
	{
		m_nLevelIndex = Z3D_GLOW_MAX_LEVEL;
	}

	if( nLevel > 0 )
	{
		m_CurrentColor = ms_aGlowParameter[m_nLevelIndex-1].vecGlowKeyData[0].c;
	}
	else
	{
		m_CurrentColor = D3DXCOLOR( 0, 0, 0, 1.0f );
	}

	m_nCurrentFrame = 0;
}

void CZ3DGlowHandler::ApplySetting( IDirect3DDevice8* pDevice )
{
	if( m_nLevelIndex == 0 )
	{
		ms_pDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(0xFF,0,0,0) );
		return;
	}

	D3DCOLOR cTemp;
	if( ms_b16bitMode )
	{
		cTemp = D3DCOLOR_COLORVALUE( m_CurrentColor.r/2, m_CurrentColor.g/2, m_CurrentColor.b/2, m_CurrentColor.a );
	}
	else
	{
		cTemp = D3DCOLOR_COLORVALUE( m_CurrentColor.r/6, m_CurrentColor.g/6, m_CurrentColor.b/6, m_CurrentColor.a );
	}


	ms_pDevice->SetRenderState( D3DRS_TEXTUREFACTOR, cTemp );
}


void CZ3DGlowHandler::_Process()
{
	std::list< CZ3DGlowHandler* >::iterator it;

	for( it = ms_listInstance.begin(); it != ms_listInstance.end(); ++it )
	{
		(*it)->FrameMove();
	}
}

bool CZ3DGlowHandler::_Init( IDirect3DDevice8* pDevice, bool bLowSpec )
{
	ms_pDevice = pDevice;

	HRESULT hr;

	float fWidth, fHeight;
	
	if( bLowSpec )
	{
		fWidth = 256;
		fHeight = 256;
	}
	else
	{
		fWidth = 512;
		fHeight = 512;
	}

	ms_RenderViewport.MinZ = 0.0f; ms_RenderViewport.MaxZ = 1.0f;
	ms_RenderViewport.X = 0; ms_RenderViewport.Y = 0;
	ms_RenderViewport.Width = DWORD(fWidth);
	ms_RenderViewport.Height = DWORD(fHeight);


	IDirect3DSurface8* pCurrentDepth;
	ms_pDevice->GetDepthStencilSurface( &pCurrentDepth );
	D3DSURFACE_DESC sd;
	pCurrentDepth->GetDesc(&sd);
	pCurrentDepth->Release();

	hr = ms_pDevice->CreateDepthStencilSurface( (UINT)fWidth, (UINT)fHeight,
										sd.Format, D3DMULTISAMPLE_NONE, &ms_pRenderTargetDepth );
	if( FAILED(hr) )
	{
		ms_bSkipGlow = true;
		return false;
	}

	D3DDISPLAYMODE dm;
	ms_pDevice->GetDisplayMode( &dm );
	if( D3DFMT_R5G6B5 == dm.Format ||
		D3DFMT_X1R5G5B5 == dm.Format ||
		D3DFMT_A1R5G5B5 == dm.Format ||
		D3DFMT_A4R4G4B4 == dm.Format ||
		D3DFMT_X4R4G4B4 == dm.Format )
	{
		ms_b16bitMode = true;
	}

	hr = ms_pDevice->CreateTexture( (UINT)fWidth, (UINT)fHeight, 1, D3DUSAGE_RENDERTARGET, dm.Format,
							D3DPOOL_DEFAULT, &ms_pRenderTargetTexture );
	if( FAILED(hr) )
	{
		ms_bSkipGlow = true;
		return false;
	}

	if( 0 == ms_pRenderTargetTexture )
	{
		ms_bSkipGlow = true;
	}



	ms_Vert[0] = CUSTOM_TLVERTEX(	0.0f,	0.0f, .1f, .1f, 0.0f, 0.0f );
	ms_Vert[1] = CUSTOM_TLVERTEX(	 800,	0.0f, .1f, .1f, 1.0f, 0.0f );
	ms_Vert[2] = CUSTOM_TLVERTEX(	0.0f,	 600, .1f, .1f, 0.0f, 1.0f );
	ms_Vert[3] = CUSTOM_TLVERTEX(	 800,	 600, .1f, .1f, 1.0f, 1.0f );



	// glow 상수 세팅
	// level 1 : A-grade
	_AddGlowConstantKeyframe( 1, D3DXCOLOR(0xFF2F2D27), 60 );
	_AddGlowConstantKeyframe( 1, D3DXCOLOR(0xFF6F3107), 30 );
	_AddGlowConstantKeyframe( 1, D3DXCOLOR(0xFF000000), 150 );
	_AddGlowConstantKeyframe( 1, D3DXCOLOR(0xFF000000), 30 );
	// level 2 : AA-grade
	_AddGlowConstantKeyframe( 2, D3DXCOLOR(0xFF2F2D27), 60 );
	_AddGlowConstantKeyframe( 2, D3DXCOLOR(0xFF416E6F), 30 );
	_AddGlowConstantKeyframe( 2, D3DXCOLOR(0xFF000000), 150 );
	_AddGlowConstantKeyframe( 2, D3DXCOLOR(0xFF000000), 30 );
	// level 3 : AAA-grade
	_AddGlowConstantKeyframe( 3, D3DXCOLOR(0xFF2F2D27), 60 );
	_AddGlowConstantKeyframe( 3, D3DXCOLOR(0xFF836725), 30 );
	_AddGlowConstantKeyframe( 3, D3DXCOLOR(0xFF000000), 150 );
	_AddGlowConstantKeyframe( 3, D3DXCOLOR(0xFF000000), 30 );
	// level 4
	_AddGlowConstantKeyframe( 4, D3DXCOLOR(0xFF263872), 40 );
	_AddGlowConstantKeyframe( 4, D3DXCOLOR(0xFF89767A), 15 );
	// level 5
	_AddGlowConstantKeyframe( 5, D3DXCOLOR(0xFF5F0D77), 46 );
	_AddGlowConstantKeyframe( 5, D3DXCOLOR(0xFFA40E51), 19 );
	// level 6
	_AddGlowConstantKeyframe( 6, D3DXCOLOR(0xFF879E0D), 48 );
	_AddGlowConstantKeyframe( 6, D3DXCOLOR(0xFF323233), 53 );
	_AddGlowConstantKeyframe( 6, D3DXCOLOR(0xFF101375), 21 );
	_AddGlowConstantKeyframe( 6, D3DXCOLOR(0xFF800B8A), 49 );
	// level 7
	_AddGlowConstantKeyframe( 7, D3DXCOLOR(0xFF3B828F), 56 );
	_AddGlowConstantKeyframe( 7, D3DXCOLOR(0xFF918288), 28 );

	return true;
}

void CZ3DGlowHandler::_Close()
{
	SAFE_RELEASE( ms_pRenderTargetDepth );
	SAFE_RELEASE( ms_pRenderTargetTexture );
}


void CZ3DGlowHandler::_PrepareGlowRender()
{
	if( ms_bSkipGlow )
	{
		ms_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		ms_pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
		ms_pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		ms_pDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );

		return;
	}


	ms_pDevice->GetViewport( &ms_SaveViewport );
	ms_pDevice->GetTransform( D3DTS_PROJECTION, &ms_SaveProjectionMatrix );

	// 현재 render target 저장
	ms_pDevice->GetRenderTarget( &ms_pOldBackBuffer );
	ms_pDevice->GetDepthStencilSurface( &ms_pOldDepthBuffer );

	// glow 용 render target texture 세팅
	IDirect3DSurface8* pRenderTarget = NULL;
	ms_pRenderTargetTexture->GetSurfaceLevel( 0, &pRenderTarget );
	HRESULT hr = ms_pDevice->SetRenderTarget( pRenderTarget, ms_pRenderTargetDepth );
	SAFE_RELEASE( pRenderTarget );

//	ms_pDevice->SetViewport( &ms_RenderViewport );
//	ms_pDevice->SetTransform( D3DTS_PROJECTION, &ms_RenderProjectionMatrix );

	// render states 세팅
	ms_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1  );
	ms_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR );
	ms_pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE  );
	ms_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
/*
	ms_pDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
	ms_pDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_USEW );
	ms_pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	ms_pDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
*/
	ms_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	ms_pDevice->SetRenderState( D3DRS_AMBIENT, 0xFFFFFFFF );

	ms_pDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );


	ms_pDevice->SetTexture( 0, NULL );
	ms_pDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0xFF000000, 1.0f, 0L );
//	ms_pDevice->Clear( 0, NULL, D3DCLEAR_TARGET, 0xFF000000, 1.0f, 0L );

}


void CZ3DGlowHandler::_EndGlowRender()
{
	if( ms_bSkipGlow )
	{
		return;
	}

	ms_pDevice->SetRenderTarget( ms_pOldBackBuffer, ms_pOldDepthBuffer );
	SAFE_RELEASE( ms_pOldBackBuffer );
	SAFE_RELEASE( ms_pOldDepthBuffer );

	ms_pDevice->SetViewport( &ms_SaveViewport );
//	ms_pDevice->SetTransform( D3DTS_PROJECTION, &ms_SaveProjectionMatrix );
}


void CZ3DGlowHandler::_ProcessGlow()
{
	if( ms_bSkipGlow )
	{
		return;
	}

	//Minotaurs 아이템 Glow

	ms_pDevice->SetViewport( &ms_SaveViewport );
	ms_pDevice->SetTransform( D3DTS_PROJECTION, &ms_SaveProjectionMatrix );

	// post-process panel render를 위한 render state 세팅
	ms_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1  );
	ms_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	ms_pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE  );

	ms_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

	ms_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	ms_pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
	ms_pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	ms_pDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );

	ms_pDevice->SetRenderState( D3DRS_ALPHAREF, 0x00 );
	ms_pDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_ALWAYS );

	ms_pDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );


	matrix mmm;
	mmm.MakeIdent();
	ms_pDevice->SetTransform( D3DTS_WORLD, mmm );
	ms_pDevice->SetVertexShader( (D3DFVF_XYZRHW | D3DFVF_TEX1) );
	ms_pDevice->SetTexture( 0, ms_pRenderTargetTexture );

	static float aFd[][2] = 
	{
/*		{-0.005f,0}, {0.005f,0},
		{ -0.01f,0}, { 0.01f,0},
		{0,-0.0075f}, {0,0.0075f},*/
		{-0.0025f,0}, {0.0025f,0},
		{-0.005f,0},  {0.005f,0},
		{0,-0.0038f}, {0,0.0038f},
	};
// Minotaurs Glow 테스트
//ms_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
//ms_Vert[0].x=0;
//ms_Vert[1].x=512;
//ms_Vert[2].x=0;
//ms_Vert[3].x=512;
//ms_Vert[0].y=0;
//ms_Vert[1].y=0;
//ms_Vert[2].y=512;
//ms_Vert[3].y=512;
//ms_pDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, ms_Vert, sizeof(CUSTOM_TLVERTEX) );
//return;


	float fWidth = float(ms_SaveViewport.Width);
	float fHeight = float(ms_SaveViewport.Height);

	int nRenderPass = ms_b16bitMode ? 2 : 6;
	for( int j = 0; j < nRenderPass; ++j )
	{
		float fdx = aFd[j][0];
		float fdy = aFd[j][1];

		ms_Vert[0].x=fWidth*fdx;
		ms_Vert[1].x=fWidth*(1.0f+fdx);
		ms_Vert[2].x=fWidth*fdx;
		ms_Vert[3].x=fWidth*(1.0f+fdx);

		ms_Vert[0].y=fHeight*fdy;
		ms_Vert[1].y=fHeight*fdy;
		ms_Vert[2].y=fHeight*(1.0f+fdy);
		ms_Vert[3].y=fHeight*(1.0f+fdy);

		ms_pDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, ms_Vert, sizeof(CUSTOM_TLVERTEX) );
	}
}




Z3D_GLOW_CONSTANTS& CZ3DGlowHandler::_GetGlowConstantField( int nLevelIndex )
{
	static Z3D_GLOW_CONSTANTS tmp;

	if( 0 == nLevelIndex || nLevelIndex > Z3D_GLOW_MAX_LEVEL )
	{
		return tmp;
	}

	return ms_aGlowParameter[nLevelIndex-1];
}

void CZ3DGlowHandler::_AddGlowConstantKeyframe( int nLevelndex, D3DXCOLOR& c, int nDuration )
{
	if( nLevelndex == 0 || nLevelndex > Z3D_GLOW_MAX_LEVEL )
	{
		return;
	}

	Z3D_GLOW_CONSTANTS& rParam = ms_aGlowParameter[nLevelndex-1];

	Z3D_GLOW_KEY glowKey;
	glowKey.nKeyFrame = rParam.nTotalFrame;
	glowKey.c = c;

	if( 0 == rParam.vecGlowKeyData.size() )
	{
		rParam.vecGlowKeyData.push_back(glowKey);
	}
	else
	{
		rParam.vecGlowKeyData[rParam.vecGlowKeyData.size()-1] = glowKey;
	}

	rParam.nTotalFrame += nDuration;

	// 루핑시 마지막 키와 첫 프레임간의 보간을 위한 더미 키
	glowKey.nKeyFrame = rParam.nTotalFrame;
	glowKey.c = rParam.vecGlowKeyData[0].c;
	rParam.vecGlowKeyData.push_back( glowKey );
}

void CZ3DGlowHandler::FrameMove()
{
	if( 0 == m_nLevelIndex )
	{
		return;
	}

	++m_nCurrentFrame;

	Z3D_GLOW_CONSTANTS& rConstants = ms_aGlowParameter[m_nLevelIndex-1];

	if( m_nCurrentFrame >= rConstants.nTotalFrame )
	{
		m_nCurrentFrame %= rConstants.nTotalFrame;
	}

	if( m_nCurrentFrame < rConstants.vecGlowKeyData[0].nKeyFrame )
	{
		// 첫 element 보다 이전
        m_CurrentColor = rConstants.vecGlowKeyData[0].c;
	}
	else
	{
		int i = 0;
		for(; i < (int)rConstants.vecGlowKeyData.size()-1; ++i )
		{
			// element 구간 내
			if( rConstants.vecGlowKeyData[i].nKeyFrame <= m_nCurrentFrame && 
				m_nCurrentFrame < rConstants.vecGlowKeyData[i+1].nKeyFrame )
			{
				float fFactor = float(m_nCurrentFrame - rConstants.vecGlowKeyData[i].nKeyFrame) /
								float(rConstants.vecGlowKeyData[i+1].nKeyFrame - rConstants.vecGlowKeyData[i].nKeyFrame);

				D3DXColorLerp(  &m_CurrentColor,
								&(rConstants.vecGlowKeyData[i].c),
								&(rConstants.vecGlowKeyData[i+1].c),
								fFactor );

				break;
			}
		}

		if( i == rConstants.vecGlowKeyData.size()-1 )
		{
			// 마지막 element 이후
			m_CurrentColor = rConstants.vecGlowKeyData[rConstants.vecGlowKeyData.size()-1].c;
		}
	}
}

void CZ3DGlowHandler::_SetRenderViewport( D3DVIEWPORT8& vp )
{
	ms_RenderViewport = vp;
}


void CZ3DGlowHandler::_SetRenderProjectionMatrix( D3DMATRIX& mat )
{
	ms_RenderProjectionMatrix = mat;
}