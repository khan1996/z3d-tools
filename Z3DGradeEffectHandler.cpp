#include ".\z3dgradeeffecthandler.h"
#include "Z3D_GLOBALS.h"
#include "GMMemory.h"


#define Z3D_GRADE_EFFECT_MAX_LEVEL 7

Z3D_GRADE_EFFECT_CONSTANTS CZ3DGradeEffectHandler::ms_aGradeEffectParameter[Z3D_GRADE_EFFECT_MAX_LEVEL] = 
{
	// color				peak	signal  silent
	{ D3DXCOLOR(0xFF5C5C5B),	 0.500,	 120,	   0 },
	{ D3DXCOLOR(0xFF5C5C5B),	 0.500,	 120,	   0 },
	{ D3DXCOLOR(0xFF5C5C5B),	 0.500,	 120,	   0 },
	{ D3DXCOLOR(0xFF5C5C5B),	 0.500,	 120,	   0 },
	{ D3DXCOLOR(0xFF5C5C5B),	 0.500,	 120,	   0 },
	{ D3DXCOLOR(0xFF5C5C5B),	 0.500,	 120,	   0 },
	{ D3DXCOLOR(0xFF5C5C5B),	 0.500,	 120,	   0 },
};


int CZ3DGradeEffectHandler::ms_nTextureAniCounter = 0;
int CZ3DGradeEffectHandler::ms_nTextureAniPeriod = 100;

std::list< CZ3DGradeEffectHandler* > CZ3DGradeEffectHandler::ms_listInstance;

Z3DTexture* CZ3DGradeEffectHandler::ms_pEffectTexture = NULL;
D3DXMATRIX CZ3DGradeEffectHandler::ms_matTextureTransform;


//////////////////////////////////////////////////////////////////////////
void CZ3DGradeEffectHandler::Init()
{
	ms_pEffectTexture = g_ContTexture.GetObject( "GradeEffect_gray.dds" );

	D3DXMatrixIdentity( &ms_matTextureTransform );
}

void CZ3DGradeEffectHandler::Close()
{
	// 텍스처 릴리즈 메소드가 없다!!! 여전히 orz
}



//////////////////////////////////////////////////////////////////////////
CZ3DGradeEffectHandler::CZ3DGradeEffectHandler(void)
{
	SetLevel(0);

	ms_listInstance.push_back( this );
	m_itrThis = --ms_listInstance.end();
}

CZ3DGradeEffectHandler::~CZ3DGradeEffectHandler(void)
{
	ms_listInstance.erase( m_itrThis );
}


void CZ3DGradeEffectHandler::SetLevel( int n )
{
	if( 0 == n )
	{
		m_nCurrentLevel = 0;
		return;
	}

	if( n >= Z3D_GRADE_EFFECT_MAX_LEVEL )	n = Z3D_GRADE_EFFECT_MAX_LEVEL;

	SetColor( ms_aGradeEffectParameter[n-1].c );
	SetSignalGraphParameter(
			ms_aGradeEffectParameter[n-1].fPeakValue,
			ms_aGradeEffectParameter[n-1].nSignalLength,
			ms_aGradeEffectParameter[n-1].nSilentLength );

	m_nCurrentLevel = n;
}


void CZ3DGradeEffectHandler::ApplySetting( IDirect3DDevice8* pDevice )
{
	if( 0 == m_nCurrentLevel )
	{
		DisableSetting(pDevice);
		return;
	}

	pDevice->SetTexture(1, ms_pEffectTexture ->GetD3dTexture());
	pDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL );

	pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_MULTIPLYADD  );
	pDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TFACTOR );
	pDevice->SetTextureStageState( 1, D3DTSS_COLORARG0, D3DTA_CURRENT );

	pDevice->SetTextureStageState( 1, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
	pDevice->SetTextureStageState( 1, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );

	pDevice->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_DISABLE );

	pDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );

	ms_matTextureTransform._41 = float(ms_nTextureAniCounter) / float(ms_nTextureAniPeriod);
	ms_matTextureTransform._42 = ms_matTextureTransform._41;
	pDevice->SetTransform( D3DTS_TEXTURE1, &ms_matTextureTransform );

	float fAmpl;
	if( 0 == m_nSignalLength )
	{
		fAmpl = m_fPeakValue;
	}
	else if( m_nCounter < m_nSignalLength )
	{
		fAmpl = m_fPeakValue * sinf( FLOAT_PHI*float(m_nCounter) / float(m_nSignalLength) );
	}
	else
	{
		fAmpl = 0.0f;
	}

	D3DCOLOR cTemp = D3DCOLOR_COLORVALUE( fAmpl*m_Color.r, fAmpl*m_Color.g, fAmpl*m_Color.b, fAmpl*m_Color.a );
	pDevice->SetRenderState( D3DRS_TEXTUREFACTOR, cTemp );
}


void CZ3DGradeEffectHandler::DisableSetting( IDirect3DDevice8* pDevice )
{
	pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
}

void CZ3DGradeEffectHandler::FrameMove()
{
	++m_nCounter;
	if( m_nCounter >= m_nSignalLength+m_nSilentLength )
	{
		m_nCounter = 0;
	}
}


void CZ3DGradeEffectHandler::Process()
{
	std::list< CZ3DGradeEffectHandler* >::iterator it;

	for( it = ms_listInstance.begin(); it != ms_listInstance.end(); ++it )
	{
		(*it)->FrameMove();
	}

	++ms_nTextureAniCounter;
	if( ms_nTextureAniCounter >= ms_nTextureAniPeriod ) ms_nTextureAniCounter = 0;
}
