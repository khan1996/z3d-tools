// Z3DRenderable.cpp: implementation of the CZ3DRenderable class.
//
//////////////////////////////////////////////////////////////////////

#pragma warning(disable:4786)

#include "Z3DRenderable.h"
#include "3DMath.h"
#include "SimpleParser.h"
#include <algorithm>
#include "GMMemory.h"


IDirect3DDevice8* CZ3DRenderable::ms_pDevice;
std::list<CZ3DRenderable*> CZ3DRenderable::ms_listpObjects;
std::vector<CZ3DRenderable*> CZ3DRenderable::ms_vec_pRenderObjects;
std::vector<CZ3DRenderable*> CZ3DRenderable::ms_vec_pSecondRenderObjects;
//unsigned long CZ3DRenderable::ms_ulVertexStateBlockHandle;
//unsigned long CZ3DRenderable::ms_ulPixelStateBlockHandle;
vector3 CZ3DRenderable::ms_vec_CameraPos;
D3DXMATRIX CZ3DRenderable::ms_IdentityD3DMatrix;

vector3 CZ3DRenderable::m_vecCameraPosition;
D3DMATERIAL8 CZ3DRenderable::ms_Material;


bool CZ3DRenderable::_Init(PDIRECT3DDEVICE8 pDevice)
{
	if( !pDevice )
		return false;

	ms_pDevice = pDevice;
	D3DXMatrixIdentity( &ms_IdentityD3DMatrix );

//	ms_pDevice->CreateStateBlock( D3DSBT_PIXELSTATE, &ms_ulPixelStateBlockHandle );
//	ms_pDevice->CreateStateBlock( D3DSBT_VERTEXSTATE, &ms_ulVertexStateBlockHandle );

	return true;
}


bool CZ3DRenderable::_Close()
{
	if( ms_pDevice )
	{
//		ms_pDevice->DeleteStateBlock( ms_ulVertexStateBlockHandle );
//		ms_pDevice->DeleteStateBlock( ms_ulPixelStateBlockHandle );
		
		ms_pDevice = NULL;
	}

	return true;
}


static bool operator< ( CZ3DRenderable& a, CZ3DRenderable& b )
{
	return (a.GetTexturePtr() < b.GetTexturePtr());
}


static DWORD s_dwAmbient;
static DWORD s_dwZWriteEnable;
static DWORD s_dwLighting;
static DWORD s_dwAlphaBlend;
static DWORD s_dwAlphaTest;
static DWORD s_dwSrcBlend;
static DWORD s_dwDestBlend;

static DWORD s_dwTS0Op;
static DWORD s_dwTS0Arg1;
static DWORD s_dwTS0Arg2;
static DWORD s_dwTS1Op;

void CZ3DRenderable::Process()
{
	std::list<CZ3DRenderable*>::iterator iter;

	// get camera position for LOD level calculation
	matrix m;
	ms_pDevice->GetTransform( D3DTS_VIEW, m );
	matrix matForViewPosition;
	matForViewPosition.Inverse(m);

	ms_vec_CameraPos=matForViewPosition.GetLoc();

	// clear pervious work vector
	if( !(ms_vec_pRenderObjects.empty()) )
		ms_vec_pRenderObjects.clear();
	if( !(ms_vec_pSecondRenderObjects.empty()) )
		ms_vec_pSecondRenderObjects.clear();

	// make non-culled object vector
	for( iter = ms_listpObjects.begin(); iter != ms_listpObjects.end(); iter++ )
	{
		if( !((*iter)->IsCulled()) && !((*iter)->IsRenderIndividually()) )
		{
			ms_vec_pRenderObjects.push_back( *iter );

			// check second-renderability
			if( (*iter)->IsSecondRenderable() )
			{
				ms_vec_pSecondRenderObjects.push_back( *iter );
			}
		}
	}
	ptr_greater<CZ3DRenderable*> pg;
	std::sort( ms_vec_pRenderObjects.begin(), ms_vec_pRenderObjects.end(), pg );


	// save current renderstates
//	GetDevice()->CreateStateBlock( D3DSBT_ALL, &ms_ulRenderStateBlockHandle );
//	GetDevice()->CaptureStateBlock( ms_ulPixelStateBlockHandle );
//	GetDevice()->CaptureStateBlock( ms_ulVertexStateBlockHandle );
	GetDevice()->GetRenderState( D3DRS_AMBIENT, &s_dwAmbient );
	GetDevice()->GetRenderState( D3DRS_ZWRITEENABLE, &s_dwZWriteEnable );
	GetDevice()->GetRenderState( D3DRS_LIGHTING, &s_dwLighting );
	GetDevice()->GetRenderState( D3DRS_ALPHABLENDENABLE, &s_dwAlphaBlend );
	GetDevice()->GetRenderState( D3DRS_ALPHATESTENABLE, &s_dwAlphaTest );
	GetDevice()->GetRenderState( D3DRS_SRCBLEND, &s_dwSrcBlend );
	GetDevice()->GetRenderState( D3DRS_DESTBLEND, &s_dwDestBlend );
	GetDevice()->GetTextureStageState( 0, D3DTSS_ALPHAOP, &s_dwTS0Op );
	GetDevice()->GetTextureStageState( 0, D3DTSS_ALPHAARG1, &s_dwTS0Arg1 );
	GetDevice()->GetTextureStageState( 0, D3DTSS_ALPHAARG2, &s_dwTS0Arg2 );
	GetDevice()->GetTextureStageState( 1, D3DTSS_ALPHAOP, &s_dwTS1Op );


	// setting up renderstate here
	GetDevice()->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_ARGB(0xff, 0x80, 0x80, 0x80) );

	ms_Material.Ambient.a = 1.0f;
	ms_Material.Ambient.r = 1.0f;
	ms_Material.Ambient.g = 1.0f;
	ms_Material.Ambient.b = 1.0f;
	ms_Material.Diffuse.a = 1.0f;
	ms_Material.Diffuse.r = 1.0f;
	ms_Material.Diffuse.g = 1.0f;
	ms_Material.Diffuse.b = 1.0f;
	ms_Material.Emissive.a = 1.0f;
	ms_Material.Emissive.r = 0.0f;
	ms_Material.Emissive.g = 0.0f;
	ms_Material.Emissive.b = 0.0f;
	ms_Material.Power = 0;
	//GetDevice()->SetMaterial( &ms_Material );

	GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);
	GetDevice()->SetRenderState( D3DRS_ALPHATESTENABLE,TRUE);
	GetDevice()->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    GetDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
	GetDevice()->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );


	// render	
	CZ3DRenderable* pRenderable;
	GetDevice()->SetTexture( 0, NULL );
	IDirect3DTexture8* pPrevTexture = NULL;
	for( int i = 0; i < (int)ms_vec_pRenderObjects.size(); i++ )
	{
		pRenderable = ms_vec_pRenderObjects[i];

		if( pPrevTexture != pRenderable->GetTexturePtr() &&
			(IDirect3DTexture8*)0xFFFFFFFF != pRenderable->GetTexturePtr() )
		{
			// texture change
			GetDevice()->SetTexture( 0, pRenderable->GetTexturePtr() );
			//GetDevice()->SetTexture( 0, NULL );
			pPrevTexture = pRenderable->GetTexturePtr();
		}

		ms_Material.Diffuse.a = pRenderable->GetTransparency();
		GetDevice()->SetMaterial( &ms_Material );
		pRenderable->Render();
	}
	GetDevice()->SetTexture( 0, NULL );
//	GetDevice()->SetStreamSource( 0, NULL, 0 );
//	GetDevice()->SetIndices( NULL, 0 );

	
	GetDevice()->SetTextureStageState( 1, D3DTSS_ALPHAOP, s_dwTS1Op );
	GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG2, s_dwTS0Arg2 );
	GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG1, s_dwTS0Arg1 );
	GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAOP, s_dwTS0Op );
	GetDevice()->SetRenderState( D3DRS_DESTBLEND, s_dwDestBlend );
	GetDevice()->SetRenderState( D3DRS_SRCBLEND, s_dwSrcBlend );
	GetDevice()->SetRenderState( D3DRS_ALPHATESTENABLE, s_dwAlphaTest );
	GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, s_dwAlphaBlend );
	GetDevice()->SetRenderState( D3DRS_LIGHTING, s_dwLighting );
	GetDevice()->SetRenderState( D3DRS_ZWRITEENABLE, s_dwZWriteEnable );
	GetDevice()->SetRenderState( D3DRS_AMBIENT, s_dwAmbient );
	
//	GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
//	GetDevice()->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);
//	GetDevice()->ApplyStateBlock( ms_ulVertexStateBlockHandle );
//	GetDevice()->ApplyStateBlock( ms_ulPixelStateBlockHandle );
//	GetDevice()->DeleteStateBlock( ms_ulRenderStateBlockHandle );
}


void CZ3DRenderable::SecondProcess()
{
	// setting up second render state( alpha'd object )

	// save current renderstates
//	GetDevice()->CreateStateBlock( D3DSBT_ALL, &ms_ulRenderStateBlockHandle );
//	GetDevice()->CaptureStateBlock( ms_ulPixelStateBlockHandle );
//	GetDevice()->CaptureStateBlock( ms_ulVertexStateBlockHandle );
	GetDevice()->GetRenderState( D3DRS_AMBIENT, &s_dwAmbient );
	GetDevice()->GetRenderState( D3DRS_ZWRITEENABLE, &s_dwZWriteEnable );
	GetDevice()->GetRenderState( D3DRS_LIGHTING, &s_dwLighting );
	GetDevice()->GetRenderState( D3DRS_ALPHABLENDENABLE, &s_dwAlphaBlend );
	GetDevice()->GetRenderState( D3DRS_ALPHATESTENABLE, &s_dwAlphaTest );
	GetDevice()->GetRenderState( D3DRS_SRCBLEND, &s_dwSrcBlend );
	GetDevice()->GetRenderState( D3DRS_DESTBLEND, &s_dwDestBlend );
	GetDevice()->GetTextureStageState( 0, D3DTSS_ALPHAOP, &s_dwTS0Op );
	GetDevice()->GetTextureStageState( 0, D3DTSS_ALPHAARG1, &s_dwTS0Arg1 );
	GetDevice()->GetTextureStageState( 0, D3DTSS_ALPHAARG2, &s_dwTS0Arg2 );
	GetDevice()->GetTextureStageState( 1, D3DTSS_ALPHAOP, &s_dwTS1Op );

	static D3DMATERIAL8 tmpMtrl;
	GetDevice()->GetMaterial( &tmpMtrl );


	// setting up renderstate here
	GetDevice()->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_ARGB(0x00, 0x80, 0x80, 0x80) );
	
	ms_Material.Ambient.a = 1.0f;
	ms_Material.Ambient.r = 0.4f;
	ms_Material.Ambient.g = 0.4f;
	ms_Material.Ambient.b = 0.4f;
	ms_Material.Diffuse.a = 1.0f;
	ms_Material.Diffuse.r = 1.60f;
	ms_Material.Diffuse.g = 1.60f;
	ms_Material.Diffuse.b = 1.60f;
	ms_Material.Emissive.a = 1.0f;
	ms_Material.Emissive.r = 0.0f;
	ms_Material.Emissive.g = 0.0f;
	ms_Material.Emissive.b = 0.0f;
	ms_Material.Power = 0.0f;
	//GetDevice()->SetMaterial( &ms_Material );
	
	// TEST now
	GetDevice()->SetRenderState( D3DRS_ZWRITEENABLE, FALSE);
	GetDevice()->SetRenderState( D3DRS_LIGHTING, FALSE );
	DWORD dwCullState;
	GetDevice()->GetRenderState( D3DRS_CULLMODE, &dwCullState );
	GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	

	GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);
	GetDevice()->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);
	GetDevice()->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	
	GetDevice()->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	GetDevice()->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	GetDevice()->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	GetDevice()->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	GetDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	

	GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
	GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
	GetDevice()->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	
	
	ptr_greater<CZ3DRenderable*> pg;
	std::sort( ms_vec_pSecondRenderObjects.begin(), ms_vec_pSecondRenderObjects.end(), pg );

	// render
	CZ3DRenderable* pRenderable;
	GetDevice()->SetTexture( 0, NULL );
	IDirect3DTexture8* pPrevTexture = NULL;
	for( int i = 0; i < (int)ms_vec_pSecondRenderObjects.size(); i++ )
	{
		pRenderable = ms_vec_pSecondRenderObjects[i];
		if( pPrevTexture != pRenderable->GetTexturePtr() &&
			(IDirect3DTexture8*)0xFFFFFFFF != pRenderable->GetTexturePtr() )
		{
			// texture change
			GetDevice()->SetTexture( 0, pRenderable->GetTexturePtr() );
			pPrevTexture = pRenderable->GetTexturePtr();
		}

		/*if( pRenderable->IsLightenSecondRender() )
		{
			GetDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		}
		else
		{
			GetDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		}*/
		GetDevice()->SetRenderState( D3DRS_SRCBLEND, pRenderable->m_SecondRenderSrcBlend );
		GetDevice()->SetRenderState( D3DRS_DESTBLEND, pRenderable->m_SecondRenderDestBlend );

		ms_Material.Diffuse.a = pRenderable->GetTransparency();
		GetDevice()->SetMaterial( &ms_Material );
		pRenderable->SecondRender();
	}

	GetDevice()->SetMaterial( &tmpMtrl );

	GetDevice()->SetTexture( 0, NULL );
	GetDevice()->SetStreamSource( 0, NULL, 0 );
	GetDevice()->SetIndices( NULL, 0 );

	GetDevice()->SetRenderState( D3DRS_CULLMODE, dwCullState );

	GetDevice()->SetTextureStageState( 1, D3DTSS_ALPHAOP, s_dwTS1Op );
	GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG2, s_dwTS0Arg2 );
	GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAARG1, s_dwTS0Arg1 );
	GetDevice()->SetTextureStageState( 0, D3DTSS_ALPHAOP, s_dwTS0Op );
	GetDevice()->SetRenderState( D3DRS_DESTBLEND, s_dwDestBlend );
	GetDevice()->SetRenderState( D3DRS_SRCBLEND, s_dwSrcBlend );
	GetDevice()->SetRenderState( D3DRS_ALPHATESTENABLE, s_dwAlphaTest );
	GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, s_dwAlphaBlend );
	GetDevice()->SetRenderState( D3DRS_LIGHTING, s_dwLighting );
	GetDevice()->SetRenderState( D3DRS_ZWRITEENABLE, s_dwZWriteEnable );
	GetDevice()->SetRenderState( D3DRS_AMBIENT, s_dwAmbient );
//	GetDevice()->ApplyStateBlock( ms_ulVertexStateBlockHandle );
//	GetDevice()->ApplyStateBlock( ms_ulPixelStateBlockHandle );
//	GetDevice()->DeleteStateBlock( ms_ulRenderStateBlockHandle );
}