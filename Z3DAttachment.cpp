// Z3DAttachment.cpp: implementation of the CZ3DAttachment class.
//
//////////////////////////////////////////////////////////////////////

#include "Z3DAttachment.h"
#include "RenderOption.h"

#include <Src/Scene/D3DBufferPools.h>
#include "SceneManager.h"
#include "GMMemory.h"


#define Z3D_ATTACHMENT_BANK_SIZE	100

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*
CZ3DAttachment::CZ3DAttachment( const char* szMesh, const char* szTex,
							   const vector3 tm_pos, const quaternion tm_dir )
							   : CZ3DRenderable(true)
{
	Set(szMesh, szTex);
	m_Pos = tm_pos;
	m_Dir = tm_dir;
	m_vPivotPos.Identity();
	m_fScaleFactor = 1.0f;
}
*/

CZ3DAttachment::~CZ3DAttachment()
{
	UnLink();

//	SAFE_RELEASE( m_pVertexBuffer );
//	SAFE_RELEASE( m_pIndexBuffer );
	CSceneManager::ms_pBufferPools->UnRef( Caldron::Scene::D3DBUFFEROBJ_VERTEX, m_pVertexBuffer );
	CSceneManager::ms_pBufferPools->UnRef( Caldron::Scene::D3DBUFFEROBJ_INDEX, m_pIndexBuffer );

	if( NULL != m_tagAMesh.GetObject() )
	{
		m_tagAMesh.Release();
	}
	if( NULL != m_pTagTexture )
	{
		// Release() 메소드가 아직 없다
//		m_tagTexture.Release();
	}
	if( NULL != m_pTagTexture2 )
	{
		// Release() 메소드가 아직 없다
//		m_tagTexture2.Release();
	}
}


bool CZ3DAttachment::Set( const char* szMesh, const char* szTex, const char* szTex2 )
{
	if( NULL != m_tagAMesh.GetObject() )
	{
		m_tagAMesh.Release();
	}
	if( NULL != m_pTagTexture )
	{
		// Release() 메소드가 아직 없다
//		m_tagTexture.Release();
	}
	if( NULL != m_pTagTexture2 )
	{
		// Release() 메소드가 아직 없다
//		m_tagTexture2.Release();
	}

	g_ContAMesh.GetObject( m_tagAMesh, szMesh );
	m_pTagTexture = g_ContTexture.GetObject( szTex );
	m_pTagTexture2 = g_ContTexture.GetObject( szTex2 );
	m_pTexture = m_pTagTexture->GetD3dTexture();

	_ASSERT(m_pTexture);

//	SAFE_DELETEA( m_pVertices );
//	m_pVertices = new BumpVertex[m_tagAMesh.GetObject()->m_wVertexCount];

	// Create vertex buffer
	if( m_lVertexCount < m_tagAMesh.GetObject()->m_wVertexCount )
	{
		m_lVertexCount = GetSizeByBank( m_tagAMesh.GetObject()->m_wVertexCount, Z3D_ATTACHMENT_BANK_SIZE );
//
//		SAFE_RELEASE( m_pVertexBuffer );
//		if(CRenderOption::m_CharacterPerPixelLighting)
//		{
//			GetDevice()->CreateVertexBuffer( m_lVertexCount*sizeof(BumpVertex), 
//				D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0,
//				D3DPOOL_DEFAULT, &m_pVertexBuffer );
//		}
//		else
//		{
//			GetDevice()->CreateVertexBuffer( m_lVertexCount*sizeof(D3DVERTEX), 
//				D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_VERTEX,
//				D3DPOOL_DEFAULT, &m_pVertexBuffer );
//		}

		CSceneManager::ms_pBufferPools->UnRef(Caldron::Scene::D3DBUFFEROBJ_VERTEX,m_pVertexBuffer);

		if(CRenderOption::m_CharacterPerPixelLighting)
		{
			m_pVertexBuffer = CSceneManager::ms_pBufferPools->GetVertexBuffer( m_lVertexCount*sizeof(BumpVertex), 0, false );
		}
		else
		{
			m_pVertexBuffer = CSceneManager::ms_pBufferPools->GetVertexBuffer( m_lVertexCount*sizeof(D3DVERTEX), 0, false );
		}
	}

	// Create index buffer
	if( m_lIndexCount < m_tagAMesh.GetObject()->m_wIndexCount )
	{
		m_lIndexCount = GetSizeByBank( m_tagAMesh.GetObject()->m_wIndexCount, Z3D_ATTACHMENT_BANK_SIZE*4 );
//
//		SAFE_RELEASE( m_pIndexBuffer );
//		GetDevice()->CreateIndexBuffer( m_lIndexCount * sizeof(WORD),
//			D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
//			D3DPOOL_DEFAULT, &m_pIndexBuffer );

		CSceneManager::ms_pBufferPools->UnRef(Caldron::Scene::D3DBUFFEROBJ_INDEX,m_pIndexBuffer);

        m_pIndexBuffer = CSceneManager::ms_pBufferPools->GetIndexBuffer( m_lIndexCount * sizeof(WORD), D3DFMT_INDEX16, false );
	}


	if( NULL == m_pIndexBuffer )
	{
		return false;
	}

	// Fill up index data
	WORD* pIndex;
//	m_pIndexBuffer->Lock( 0, 0, (BYTE**)(&pIndex), D3DLOCK_DISCARD );
	m_pIndexBuffer->Lock( 0, 0, (BYTE**)(&pIndex), 0 );

	for( int i = 0; i < m_tagAMesh.GetObject()->m_wIndexCount; i++ )
	{
		pIndex[i] = m_tagAMesh.GetObject()->m_pIndices[i];
	}
	m_pIndexBuffer->Unlock();

	return true;
}


void CZ3DAttachment::ApplyTransform()
{
	// allocation safety check
	if( NULL == m_pVertexBuffer )
	{
		return;
	}

	matrix m;
	m._11 = m_TM._11 * m_fScaleFactor;
	m._12 = m_TM._12 * m_fScaleFactor;
	m._13 = m_TM._13 * m_fScaleFactor;
	m._14 = 0.0f;
	m._21 = m_TM._21 * m_fScaleFactor;
	m._22 = m_TM._22 * m_fScaleFactor;
	m._23 = m_TM._23 * m_fScaleFactor;
	m._24 = 0.0f;
	m._31 = m_TM._31 * m_fScaleFactor;
	m._32 = m_TM._32 * m_fScaleFactor;
	m._33 = m_TM._33 * m_fScaleFactor;
	m._34 = 0.0f;
	m._41 = m_TM._41 * m_fScaleFactor;
	m._42 = m_TM._42 * m_fScaleFactor;
	m._43 = m_TM._43 * m_fScaleFactor;
	m._44 = 1.0f;

	if( CRenderOption::m_CharacterPerPixelLighting )
	{
		BumpVertex* pMeshVertex = m_tagAMesh.GetObject()->m_pVertices;
		BumpVertex* pVertices;
//		m_pVertexBuffer->Lock( 0, 0, (BYTE**)(&pVertices), D3DLOCK_DISCARD );
		m_pVertexBuffer->Lock( 0, 0, (BYTE**)(&pVertices), 0 );
		for( int i = 0; i < m_tagAMesh.GetObject()->m_wVertexCount; ++i )
		{
			pVertices[i].v.x = pMeshVertex[i].v.x*m._11 + pMeshVertex[i].v.y*m._21 + pMeshVertex[i].v.z*m._31 + m._41;
			pVertices[i].v.y = pMeshVertex[i].v.x*m._12 + pMeshVertex[i].v.y*m._22 + pMeshVertex[i].v.z*m._32 + m._42;
			pVertices[i].v.z = pMeshVertex[i].v.x*m._13 + pMeshVertex[i].v.y*m._23 + pMeshVertex[i].v.z*m._33 + m._43;

			pVertices[i].n.x = pMeshVertex[i].n.x*m._11 + pMeshVertex[i].n.y*m._21 + pMeshVertex[i].n.z*m._31;
			pVertices[i].n.y = pMeshVertex[i].n.x*m._12 + pMeshVertex[i].n.y*m._22 + pMeshVertex[i].n.z*m._32;
			pVertices[i].n.z = pMeshVertex[i].n.x*m._13 + pMeshVertex[i].n.y*m._23 + pMeshVertex[i].n.z*m._33;

			pVertices[i].tu = pMeshVertex[i].tu;
			pVertices[i].tv = pMeshVertex[i].tv;

			pVertices[i].u.x = pMeshVertex[i].u.x*m._11 + pMeshVertex[i].u.y*m._21 + pMeshVertex[i].u.z*m._31;
			pVertices[i].u.y = pMeshVertex[i].u.x*m._12 + pMeshVertex[i].u.y*m._22 + pMeshVertex[i].u.z*m._32;
			pVertices[i].u.z = pMeshVertex[i].u.x*m._13 + pMeshVertex[i].u.y*m._23 + pMeshVertex[i].u.z*m._33;
		}
		m_pVertexBuffer->Unlock();
	}
	else
	{
		BumpVertex* pMeshVertex = m_tagAMesh.GetObject()->m_pVertices;
		D3DVERTEX* pVertices;
//		m_pVertexBuffer->Lock( 0, 0, (BYTE**)(&pVertices), D3DLOCK_DISCARD );
		m_pVertexBuffer->Lock( 0, 0, (BYTE**)(&pVertices), 0 );
		for( int i = 0; i < m_tagAMesh.GetObject()->m_wVertexCount; ++i )
		{
			pVertices[i].x = pMeshVertex[i].v.x*m._11 + pMeshVertex[i].v.y*m._21 + pMeshVertex[i].v.z*m._31 + m._41;
			pVertices[i].y = pMeshVertex[i].v.x*m._12 + pMeshVertex[i].v.y*m._22 + pMeshVertex[i].v.z*m._32 + m._42;
			pVertices[i].z = pMeshVertex[i].v.x*m._13 + pMeshVertex[i].v.y*m._23 + pMeshVertex[i].v.z*m._33 + m._43;

			pVertices[i].nx = pMeshVertex[i].n.x*m._11 + pMeshVertex[i].n.y*m._21 + pMeshVertex[i].n.z*m._31;
			pVertices[i].ny = pMeshVertex[i].n.x*m._12 + pMeshVertex[i].n.y*m._22 + pMeshVertex[i].n.z*m._32;
			pVertices[i].nz = pMeshVertex[i].n.x*m._13 + pMeshVertex[i].n.y*m._23 + pMeshVertex[i].n.z*m._33;

			pVertices[i].tu = pMeshVertex[i].tu;
			pVertices[i].tv = pMeshVertex[i].tv;
		}
		m_pVertexBuffer->Unlock();
	}
}


void CZ3DAttachment::Render()
{
//	_ASSERT( m_tagAMesh.GetObject() );
//	_ASSERT( m_tagAMesh.GetObject()->m_pVertices );
//	_ASSERT( m_tagAMesh.GetObject()->m_pIndices );

	// allocation safety check
	if( NULL == m_pVertexBuffer || NULL == m_pIndexBuffer )
	{
		return;
	}

	if(CRenderOption::m_CharacterPerPixelLighting)
	{
//		GetDevice()->SetVertexShader( BUMPVERTEXFVF );
	}
	else
	{
		GetDevice()->SetVertexShader( D3DFVF_VERTEX );
	}

	matrix m;
	m.MakeIdent();
	m._41 = m_vPivotPos.x;
	m._42 = m_vPivotPos.y;
	m._43 = m_vPivotPos.z;
	GetDevice()->SetTransform( D3DTS_WORLD, m );

	m_pTexture = m_pTagTexture->GetD3dTexture();

	if( m_pTagTexture2 )
	{
		GetDevice()->SetTexture( 1, m_pTagTexture2->GetD3dTexture() );
	}
	else
	{
		GetDevice()->SetTexture( 1, NULL );
	}
//	GetDevice()->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0);

	if(CRenderOption::m_CharacterPerPixelLighting)
	{
		GetDevice()->SetStreamSource( 0, m_pVertexBuffer, sizeof(BumpVertex) );
	}			
	else
	{
		GetDevice()->SetStreamSource( 0, m_pVertexBuffer, sizeof(D3DVERTEX) );
	}
	GetDevice()->SetIndices( m_pIndexBuffer, 0 );

	m_GradeEffectHandler.ApplySetting( GetDevice() );

	GetDevice()->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST, 0, m_tagAMesh.GetObject()->m_wVertexCount,
		0, m_tagAMesh.GetObject()->m_wIndexCount/3 );
//	GetDevice()->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1);
}


void CZ3DAttachment::RenderShadow( IDirect3DDevice8* pDevice, DWORD vertexShader )
{
//	_ASSERT( m_tagAMesh.GetObject() );
//	_ASSERT( m_tagAMesh.GetObject()->m_pVertices );
//	_ASSERT( m_tagAMesh.GetObject()->m_pIndices );

	// allocation safety check
	if( NULL == m_pVertexBuffer || NULL == m_pIndexBuffer )
	{
		return;
	}


	if( NULL != vertexShader )	
	{
		if( 0xFFFFFFFF == vertexShader )
		{
			if(CRenderOption::m_CharacterPerPixelLighting)
			{
				GetDevice()->SetVertexShader( BUMPVERTEXFVF );
			}
			else
			{
				GetDevice()->SetVertexShader( D3DFVF_VERTEX );
			}
		}
		else
		{
			GetDevice()->SetVertexShader( vertexShader );
		}
	}

	matrix m;
	m.MakeIdent();
	m._41 = m_vPivotPos.x;
	m._42 = m_vPivotPos.y;
	m._43 = m_vPivotPos.z;
	GetDevice()->SetTransform( D3DTS_WORLD, m );

	if(CRenderOption::m_CharacterPerPixelLighting)
	{
		GetDevice()->SetStreamSource( 0, m_pVertexBuffer, sizeof(BumpVertex) );
	}			
	else
	{
		GetDevice()->SetStreamSource( 0, m_pVertexBuffer, sizeof(D3DVERTEX) );
	}
	GetDevice()->SetIndices( m_pIndexBuffer, 0 );

	pDevice->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST, 0, m_tagAMesh.GetObject()->m_wVertexCount,
		0, m_tagAMesh.GetObject()->m_wIndexCount/3 );
}


void CZ3DAttachment::RenderGlowShape( IDirect3DDevice8* pDevice )
{
	if( NULL == m_pVertexBuffer || NULL == m_pIndexBuffer )
	{
		return;
	}


	GetDevice()->SetVertexShader( D3DFVF_VERTEX );

	matrix m;
	m.MakeIdent();
	m._41 = m_vPivotPos.x;
	m._42 = m_vPivotPos.y;
	m._43 = m_vPivotPos.z;
	GetDevice()->SetTransform( D3DTS_WORLD, m );

	m_GlowHandler.ApplySetting( GetDevice() );

	if(CRenderOption::m_CharacterPerPixelLighting)
	{
		GetDevice()->SetStreamSource( 0, m_pVertexBuffer, sizeof(BumpVertex) );
	}			
	else
	{
		GetDevice()->SetStreamSource( 0, m_pVertexBuffer, sizeof(D3DVERTEX) );
	}
	GetDevice()->SetIndices( m_pIndexBuffer, 0 );

	pDevice->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST, 0, m_tagAMesh.GetObject()->m_wVertexCount,
		0, m_tagAMesh.GetObject()->m_wIndexCount/3 );
}
