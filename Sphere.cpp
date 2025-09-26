
#include "Sphere.h"

#include <math.h>
//#include <Utility/Exception.h>
#include <d3dx8.h>
#include <stdio.h>
#include "matrix.h"
#include "GMMemory.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////
//

struct CUSTOMVERTEX
{
    FLOAT x, y, z;      // The untransformed, 3D position for the vertex
    DWORD color;        // The vertex color
};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)

////////////////////////////////////////////////////////////////////////////
//

static void Error( const char * Msg, ... )
{
	char szBuffer[1024];

	va_list args;

	va_start( args, Msg );

	vsprintf( szBuffer, Msg, args );

	va_end( args );

//	throw bad_exception( szBuffer );
}

static void Error( const char * str, HRESULT hr )
{
	Error( "%s, ErrorCode : 0x%x", str, hr );
}



////////////////////////////////////////////////////////////////////////////
//

CSphere::CSphere()
	: m_pd3dDevice( NULL )
	, m_pVB( NULL )
	, m_uiRings( 0 )
	, m_uiSegments( 0 )
	, m_fPosX( 0.0f )
	, m_fPosY( 0.0f )
	, m_fPosZ( 0.0f )
	, m_fRadius( 0.0f )
	, m_fRed( 0.0f )
	, m_fGreen( 0.0f )
	, m_fBlue( 0.0f )
	, m_uiSphereVertices( 0 )
	, m_ucTransparency( 80 )
{
}

////////////////////////////////////////////////////////////////////////////
//

CSphere::~CSphere()
{
	Destroy();
}

////////////////////////////////////////////////////////////////////////////
//

void	CSphere::Create( IDirect3DDevice8 * pd3dDevice, unsigned rings, unsigned segments )
{
	m_pd3dDevice = pd3dDevice;
	m_uiRings = rings;
	m_uiSegments = segments;
	m_uiSphereVertices = ( 2 * m_uiRings * ( m_uiSegments + 1 ) );

	HRESULT hr = m_pd3dDevice->CreateVertexBuffer( m_uiSphereVertices * sizeof( CUSTOMVERTEX ),
													0, D3DFVF_CUSTOMVERTEX, 
													D3DPOOL_MANAGED, &m_pVB );
	if( FAILED( hr ) )
	{
		Error( "CreateVertexBuffer Failed!!(CSphere::Create)", hr );
	}

    CUSTOMVERTEX* pVertices;

	hr = m_pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 );

    if( FAILED( hr ) )
        Error( "VertexBuffer Lock Failed!!(CSphere::Create) ErrorCode : 0x%x", hr );

    float fDeltaRingAngle = ( D3DX_PI / m_uiRings );
    float fDeltaSegAngle  = ( 2.0f * D3DX_PI / m_uiSegments );

	for( DWORD ring = 0; ring < m_uiRings; ring++ )
	{
		float r0 = sinf( (ring+0) * fDeltaRingAngle );
		float r1 = sinf( (ring+1) * fDeltaRingAngle );
		float y0 = cosf( (ring+0) * fDeltaRingAngle );
		float y1 = cosf( (ring+1) * fDeltaRingAngle );

		for( DWORD seg = 0; seg < m_uiSegments; seg++ )
		{
			pVertices->x = r0 * sinf( seg * fDeltaSegAngle );
			pVertices->y = y0;
			pVertices->z = r0 * cosf( seg * fDeltaSegAngle );
			pVertices->color = 0xffffffff;
			pVertices++;

			pVertices->x = r1 * sinf( seg * fDeltaSegAngle );
			pVertices->y = y1;
			pVertices->z = r1 * cosf( seg * fDeltaSegAngle );
			pVertices->color = 0xffffffff;
			pVertices++;
		}

	}

    m_pVB->Unlock();
}

////////////////////////////////////////////////////////////////////////////
//

void	CSphere::Destroy()
{
	m_pd3dDevice = NULL;
	if( m_pVB )
	{
		m_pVB->Release();
		m_pVB = NULL;
	}
	m_uiRings = m_uiSegments = 0;
}

////////////////////////////////////////////////////////////////////////////
//

void	CSphere::SetPosition( float x, float y, float z )
{
	m_fPosX = x;
	m_fPosY = y;
	m_fPosZ = z;
}

////////////////////////////////////////////////////////////////////////////
//

void	CSphere::SetColor( float r, float g, float b )
{
	m_fRed = r;
	m_fGreen = g;
	m_fBlue = b;
}

////////////////////////////////////////////////////////////////////////////
//

HRESULT SetShader( LPDIRECT3DDEVICE8 pd3dDevice, DWORD color )
{
	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR,color);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 2, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	DWORD dwNumPasses;
	return pd3dDevice->ValidateDevice( &dwNumPasses );
}


void	CSphere::Render()
{
//	HRESULT hr = m_pd3dDevice->BeginScene();
//	if( FAILED( hr ) )
//        Error( "BeginScene Failed!!( CSphere::Render )", hr );

	D3DXMATRIX matWorld, oldMatrix, scaleMat, moveMat;
	m_pd3dDevice->GetTransform( D3DTS_WORLD, &oldMatrix );	

	D3DXMatrixScaling( &scaleMat, m_fRadius, m_fRadius, m_fRadius );
    D3DXMatrixTranslation( &moveMat, m_fPosX, m_fPosY, m_fPosZ );
	D3DXMatrixMultiply( &matWorld, &scaleMat, &moveMat );
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	//D3DMATERIAL8 material = { 0xff0000ff, 0x00000000, 0x00000000, 0x00000000, 0.0f };
	//m_pd3dDevice->SetMaterial( &material );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
	m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);    		
	m_pd3dDevice->SetRenderState( D3DRS_FILLMODE,D3DFILL_WIREFRAME);
	DWORD color = 0;
	color |= ( m_ucTransparency << 24 );
	color |= ( int( m_fRed * 255 ) << 16 );					//Red
	color |= ( int( m_fGreen * 255 ) << 8 );				//Green
	color |= ( int( m_fBlue * 255 ) << 0 );					//Blue
	SetShader( m_pd3dDevice, color );

    m_pd3dDevice->SetStreamSource( 0, m_pVB, sizeof(CUSTOMVERTEX) );
    m_pd3dDevice->SetVertexShader( D3DFVF_CUSTOMVERTEX );
    m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, m_uiSphereVertices - 2 );

	m_pd3dDevice->SetTransform( D3DTS_WORLD, &oldMatrix );

	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE);
	m_pd3dDevice->SetRenderState( D3DRS_FILLMODE,D3DFILL_SOLID);
//	m_pd3dDevice->EndScene();
}


////////////////////////////////////////////////////////////////////////////
