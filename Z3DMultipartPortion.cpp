// Z3DMultipartPortion.cpp: implementation of the Z3DMultipartPortion class.
//
//////////////////////////////////////////////////////////////////////

#include "Z3DMultipartPortion.h"
#include "Vertex.h"
#include "RenderOption.h"
#include <Src/Scene/D3DBufferPools.h>
#include "SceneManager.h"
#include "GMMemory.h"



const long Z3DMultipartPortion::ms_lMaxVBCacheCount = 20;
long Z3DMultipartPortion::ms_lVBCacheCount = 0;
Z3DVBCacheNode* Z3DMultipartPortion::ms_pVBCacheChainHeader = NULL;

const long Z3DMultipartPortion::ms_alMaxIBCacheCount[Z3D_LOD_LEVEL] = { 20, 20, 20, 20 };
long Z3DMultipartPortion::ms_alIBCacheCount[Z3D_LOD_LEVEL] = { 0, 0, 0, 0 };
Z3DIBCacheNode* Z3DMultipartPortion::ms_apIBCacheChainHeader[Z3D_LOD_LEVEL] = { NULL, NULL, NULL, NULL };



Z3DMultipartPortion::Z3DMultipartPortion( Z3D_MULTIPART_PORTION_TYPE mpt )
{
	m_bNeedLoadedCheck = false;

	m_rpThis = this;

	m_lVertexBufferVertexCount = 0;

	m_pVertexBuffer = NULL;
	m_lVertexCount = 0;

	for( int i = 0; i < Z3D_LOD_LEVEL; ++i )
	{
		m_alIndexBufferIndexCount[i] = 0;

		m_apIndexBuffer[i] = NULL;
		m_alIndexCount[i] = NULL;
	}

	if( Z3D_MPT_TEXPIECE == mpt )
	{
		m_pTexture = new Z3DTexture;
		m_pTexture2 = new Z3DTexture;
	}
	else
	{
		m_pTexture = NULL;
		m_pTexture2 = NULL;
	}
	m_MPT = mpt;

	m_vec_pMesh.clear();
}


Z3DMultipartPortion::~Z3DMultipartPortion()
{
	for( int i = 0; i < Z3D_LOD_LEVEL; ++i )
	{
		//SAFE_RELEASE( m_apIndexBuffer[i] );
		_ReleaseIndexBufferInterface( i, m_apIndexBuffer[i] );
	}

	//SAFE_RELEASE( m_pVertexBuffer );
	_ReleaseVertexBufferInterface( m_pVertexBuffer );

	m_vec_pMesh.clear();

//		std::map<int, H3DMeshTag>::iterator it_m;
//		for( it_m = m_map_IdMeshTag.begin(); it_m != m_map_IdMeshTag.end(); it_m++ )
//		{
//			it_m->second.Release();
//		}

	std::map<int, H3DTexPieceTag>::iterator it_t;
	for( it_t = m_map_IdTexPieceTag.begin(); it_t != m_map_IdTexPieceTag.end(); it_t++ )
	{
		it_t->second.Release();
	}
	for( it_t = m_map_IdTexPieceTag2.begin(); it_t != m_map_IdTexPieceTag2.end(); it_t++ )
	{
		it_t->second.Release();
	}

	std::map<int, Z3DTexture* >::iterator it;
	for( it = m_map_IdTexture.begin(); it != m_map_IdTexture.end(); it++ )
	{
		// 현재 Release() 메소드 없음
		//it->second.Release();
	}
	for( it = m_map_IdTexture2.begin(); it != m_map_IdTexture2.end(); it++ )
	{
		// 현재 Release() 메소드 없음
		//it->second.Release();
	}

	for (int i = 0; i < (int)m_vec_pGradeEffectHandler.size(); ++i )
	{
		SAFE_DELETE( m_vec_pGradeEffectHandler[ i ] ) ;
	}
	m_vec_pGradeEffectHandler.clear() ;

	for (int i = 0; i < (int)m_vec_pGlowHandler.size(); ++i )
	{
		SAFE_DELETE( m_vec_pGlowHandler[ i ] ) ;
	}
	m_vec_pGlowHandler.clear() ;

	SAFE_DELETE( m_pTexture );
	SAFE_DELETE( m_pTexture2 );

	m_rpThis = NULL;
}




// 실제로는 LOD단계별 index자료 설정및 vertex를 위한 공간할당
void Z3DMultipartPortion::BuildMesh( IDirect3DDevice8* pDevice )
{
	if( this != m_rpThis )
	{
		return;
	}

	std::map<int, Z3DLODMesh*>::iterator it, itToBeLast;
	int i, j, k;

	// clear previous set
	m_vec_pMesh.clear();
	m_vec_pTexture.clear();
	m_vec_pTexture2.clear();

	m_lVertexCount = 0;
	for( i = 0; i < Z3D_LOD_LEVEL; i++  )
	{
		m_alIndexCount[i] = 0;
	}

	Z3DLODMesh* pMesh;
	// sum index count for each LOD level
	// 그리고 각 파트의 누적순서(=렌더링순서) 리스트도 만든다.
	itToBeLast = m_map_Id2MeshData.end(); // 마지막으로 돌려야될 파트의 id

	size_t nSizeToBe = m_map_Id2MeshData.size();
	// 적을 경우 확장 - grade effect
	while( m_vec_pGradeEffectHandler.size() < nSizeToBe )
	{
		CZ3DGradeEffectHandler* pTemp = new CZ3DGradeEffectHandler;
		m_vec_pGradeEffectHandler.push_back( pTemp );
	}
	// 적을 경우 확장 - glow
	while( m_vec_pGlowHandler.size() < nSizeToBe )
	{
		CZ3DGlowHandler* pTemp = new CZ3DGlowHandler;
		m_vec_pGlowHandler.push_back( pTemp );
	}

	// 많을 경우 축소 - grade effect
	for( i = nSizeToBe; i < (int)m_vec_pGradeEffectHandler.size(); ++i )
	{
		SAFE_DELETE( m_vec_pGradeEffectHandler[i] );
	}
	m_vec_pGradeEffectHandler.resize( nSizeToBe );
	// 많을 경우 축소 - glow
	for( i = nSizeToBe; i < (int)m_vec_pGlowHandler.size(); ++i )
	{
		SAFE_DELETE( m_vec_pGlowHandler[i] );
	}
	m_vec_pGlowHandler.resize( nSizeToBe );




	j = 0;
	for( it = m_map_Id2MeshData.begin(); it != m_map_Id2MeshData.end(); it++ )
	{
		pMesh = it->second;

		_ASSERT( pMesh );

		for( i = 0; i < Z3D_LOD_LEVEL; i++ )
		{
			m_alIndexCount[i] += pMesh->aIndex[i].lIndexCount;
		}

		// 머리카락 파트일 경우, 인덱스 합 계산은 위에서 하고
		// 리스트에는 마지막에 넣기위해 빠짐
/*		if( Z3D_MPID_EAR == it->first  )
		{
			itToBeLast = it;
			continue;
		}
*/
		m_vec_pMesh.push_back( pMesh );
		if( Z3D_MPT_TEXTURE == m_MPT )
		{
			m_vec_pTexture.push_back( m_map_IdTexture[it->first] );
			if( m_map_IdTexture2.end() == m_map_IdTexture2.find( it->first ) )
			{
				m_vec_pTexture2.push_back( NULL );
			}
			else
			{
				m_vec_pTexture2.push_back( m_map_IdTexture2[it->first] );
			}
		}

		m_vec_pGradeEffectHandler[j]->SetLevel( m_map_Id2GradeEffectIndex[it->first] );
		m_vec_pGlowHandler[j]->SetLevel( m_map_Id2GlowIndex[it->first] );
		++j;
	}
	// 마지막으로 돌려야 될 파트가 있으면..( itToBeLast의 값이 바뀌었다면)
/*	if( m_map_IdMeshTag.end() != itToBeLast )
	{
		// 리스트에 마저 집어넣어주지롱(합은 루프안에서 이미 구해져있다)
		m_vec_pMesh.push_back( (itToBeLast->second).GetObject() );
	}
*/
	if( 0 == m_vec_pMesh.size() )
	{
		return;
	}

	// alloc. index buffer space
	for( i = 0; i < Z3D_LOD_LEVEL; i++ )
	{
		if( m_alIndexBufferIndexCount[i] < m_alIndexCount[i] )
		{
			m_alIndexBufferIndexCount[i] = GetSizeByBank( m_alIndexCount[i], Z3D_PORTION_BUFFER_BANK_SIZE*4 );
			
			_ReleaseIndexBufferInterface( i, m_apIndexBuffer[i] );

			m_apIndexBuffer[i] = _GetIndexBufferInterface( i, pDevice, m_alIndexBufferIndexCount[i] );

			if( NULL == m_apIndexBuffer[i] )
			{
				m_apIndexBuffer[i] = NULL;
			}
		}
	}

	// fill up data
	int nCount[Z3D_LOD_LEVEL] = { 0, };

	WORD* apIndices[Z3D_LOD_LEVEL] = { NULL, };
	for( i = 0; i < Z3D_LOD_LEVEL; ++i )
	{
		// allocation safety check
		if( NULL == m_apIndexBuffer[i] )
		{
			continue;
		}

		//m_apIndexBuffer[i]->Lock( 0, 0, (BYTE**)(&apIndices[i]), D3DLOCK_DISCARD );
		m_apIndexBuffer[i]->Lock( 0, 0, (BYTE**)(&apIndices[i]), 0 );
	}
	
	for( k = 0; k < (int)m_vec_pMesh.size(); k++ )
	{
		pMesh = m_vec_pMesh[k];

		for( i = 0; i < Z3D_LOD_LEVEL; i++ )
		{
			for( j = 0; j < pMesh->aIndex[i].lIndexCount; j++ )
			{
				apIndices[i][nCount[i]] =
					pMesh->aIndex[i].pIndices[j] + (WORD)m_lVertexCount;

				(nCount[i])++;
			}
		}
		m_lVertexCount += pMesh->lVertexCount;
	}

	for( i = 0; i < Z3D_LOD_LEVEL; ++i )
	{
		// allocation safety check
		if( NULL == m_apIndexBuffer[i] )
		{
			continue;
		}
		
		m_apIndexBuffer[i]->Unlock();
	}

	// vertex buffer 크기부족시 확장재할당
	if( m_lVertexBufferVertexCount < m_lVertexCount )
	{
		m_lVertexBufferVertexCount = GetSizeByBank( m_lVertexCount, Z3D_PORTION_BUFFER_BANK_SIZE );

		_ReleaseVertexBufferInterface( m_pVertexBuffer );

		m_pVertexBuffer = _GetVertexBufferInterface( pDevice, m_lVertexBufferVertexCount );
	}
}


bool Z3DMultipartPortion::SetGradeEffectIndex( const int nPartId, const int nIndex )
{
	if( this != m_rpThis )
	{
		return true;
	}

	std::map< int, int >::iterator it;

	m_map_Id2GradeEffectIndex[nPartId] = nIndex;
	return true ;
}


bool Z3DMultipartPortion::SetGlowIndex( const int nPartId, const int nIndex )
{
	if( this != m_rpThis )
	{
		return true;
	}

	std::map< int, int >::iterator it;

	m_map_Id2GlowIndex[nPartId] = nIndex;
	return true ;
}


bool Z3DMultipartPortion::SetMesh( const int nPartId, Z3DLODMesh* pMesh )
{
	if( this != m_rpThis )
	{
		return true;
	}

	std::map<int, Z3DLODMesh*>::iterator it_m;
	
	it_m = m_map_Id2MeshData.find( nPartId );
	if( m_map_Id2MeshData.end() != it_m )
	{
		// 슬롯에 세팅될 메시가 이전에 세팅되어있던 메시와 같은가?
		if( (it_m->second) == pMesh )
		{
			// 중복이므로 현재 불러온 메시 release
			// 현재 release 코드 없음-_-
			//pMesh.Release();
			
			// 메시가 바뀌지 않았으므로, blending재계산 필요가 없으므로 false반환
			return false;
		}
		// 이전에 세팅된 mesh release
		else
		{
			// 현재 release 코드 없음-_-
			//it_m->second.Release();
		}
	}
	
	// setting
	m_map_Id2MeshData[nPartId] = pMesh;

	return true;
}


bool Z3DMultipartPortion::DeleteMesh( const int nPartId )
{
	if( this != m_rpThis )
	{
		return true;
	}

	std::map<int, Z3DLODMesh*>::iterator it_m;
	
	it_m = m_map_Id2MeshData.find( nPartId );
	if( m_map_Id2MeshData.end() == it_m )
	{
		return false; // id에 해당하는 mesh가 세팅되어있지 않다.
	}

	// deleting
	// release code 현재 없음
	//it_m->second.Release();
	m_map_Id2MeshData.erase(it_m);

	return true;
}


bool Z3DMultipartPortion::SetTexPiece( const int nPartId, H3DTexPieceTag tagTexpiece )
{
	if( this != m_rpThis )
	{
		return true;
	}

	std::map<int, H3DTexPieceTag>::iterator it_t;
	
	it_t = m_map_IdTexPieceTag.find( nPartId );
	if( m_map_IdTexPieceTag.end() != it_t )
	{
		// same as previous
		if( (it_t->second) == tagTexpiece )
		{
			tagTexpiece.Release();
			return false;
		}
		
		// 이전에 세팅된 texpiece container instance를 release해준다
		else
		{
			it_t->second.Release();
		}
	}
	
	// setting
	m_map_IdTexPieceTag[nPartId] = tagTexpiece;

	return true;
}


bool Z3DMultipartPortion::SetTexPiece2( const int nPartId, H3DTexPieceTag tagTexpiece )
{
	if( this != m_rpThis )
	{
		return true;
	}

	std::map<int, H3DTexPieceTag>::iterator it_t;
	
	it_t = m_map_IdTexPieceTag2.find( nPartId );
	if( m_map_IdTexPieceTag2.end() != it_t )
	{
		// same as previous
		if( (it_t->second) == tagTexpiece )
		{
			tagTexpiece.Release();
			return false;
		}
		
		// 이전에 세팅된 texpiece container instance를 release해준다
		else
		{
			it_t->second.Release();
		}
	}
	
	// setting
	m_map_IdTexPieceTag2[nPartId] = tagTexpiece;

	return true;
}


bool Z3DMultipartPortion::DeleteTexPiece( const int nPartId )
{
	if( this != m_rpThis )
	{
		return true;
	}

	std::map<int, H3DTexPieceTag>::iterator it_t;

	it_t = m_map_IdTexPieceTag.find( nPartId );
	if( m_map_IdTexPieceTag.end() == it_t )
	{
		return false; // id에 해당하는 texpiece세팅이 되어있지 않다.
	}

	it_t->second.Release();
	m_map_IdTexPieceTag.erase(it_t);

	return true;
}


bool Z3DMultipartPortion::SetTexture( const int nPartId, Z3DTexture* pTexture )
{
	if( this != m_rpThis )
	{
		return true;
	}

	std::map<int, Z3DTexture* >::iterator it_t;
	
	it_t = m_map_IdTexture.find( nPartId );
	if( m_map_IdTexture.end() != it_t )
	{
		// same as previous
		if( (it_t->second) == pTexture )
		{
			// Release() 메소드가 아직 없다
			//tagTexture.Release();
			return false;
		}
		
		// 이전에 세팅된 texpiece container instance를 release해준다
		else
		{
			// Release() 메소드가 아직 없다
			//it_t->second.Release();
		}
	}
	
	// setting
	m_map_IdTexture[nPartId] = pTexture;

	return true;
}


bool Z3DMultipartPortion::SetTexture2( const int nPartId, Z3DTexture* pTexture )
{
	if( this != m_rpThis )
	{
		return true;
	}

	std::map<int, Z3DTexture* >::iterator it_t;
	
	it_t = m_map_IdTexture2.find( nPartId );
	if( m_map_IdTexture2.end() != it_t )
	{
		// same as previous
		if( (it_t->second) == pTexture )
		{
			// Release() 메소드가 아직 없다
			//tagTexture.Release();
			return false;
		}
		
		// 이전에 세팅된 texpiece container instance를 release해준다
		else
		{
			// Release() 메소드가 아직 없다
			//it_t->second.Release();
		}
	}
	
	// setting
	m_map_IdTexture2[nPartId] = pTexture;

	return true;
}


bool Z3DMultipartPortion::DeleteTexture( const int nPartId )
{
	if( this != m_rpThis )
	{
		return true;
	}

	std::map<int, Z3DTexture* >::iterator it_t;
	
	it_t = m_map_IdTexture.find( nPartId );
	if( m_map_IdTexture.end() != it_t )
	{
		// Release() 메소드가 아직 없다
		//it_t->second.Release();
		m_map_IdTexture.erase( it_t );
	}

	return true;
}


bool Z3DMultipartPortion::DeleteTexture2( const int nPartId )
{
	if( this != m_rpThis )
	{
		return true;
	}

	std::map<int, Z3DTexture* >::iterator it_t;
	
	it_t = m_map_IdTexture2.find( nPartId );
	if( m_map_IdTexture2.end() != it_t )
	{
		// Release() 메소드가 아직 없다
		//it_t->second.Release();
		m_map_IdTexture2.erase( it_t );
	}

	return true;
}


bool Z3DMultipartPortion::BatchOpen()
{
	if( this != m_rpThis )
	{
		return true;
	}

	/*map<int, Z3DMeshTag>::iterator it_m;
	for( it_m = m_map_IdMeshTag.begin(); it_m != m_map_IdMeshTag.end(); it_m++ )
	{
		g_ContLODMesh.Release(it_m->second);
	}
	m_map_IdMeshTag.clear();*/
	
	/*map<int, Z3DTexPieceTag>::iterator it_t;
	for( it_t = m_map_IdTexPieceTag.begin(); it_t != m_map_IdTexPieceTag.end(); it_t++ )
	{
		g_ContTexturePiece.Release(it_t->second);
	}
	m_map_IdTexPieceTag.clear();*/

	/*m_vec_pMesh.clear();

	m_lVertexCount = 0;
	SAFE_DELETEA( m_pVertices );

	for( int i = 0; i < Z3D_LOD_LEVEL; i++ )
	{
		m_aIndices[i].lIndexCount = 0;
		SAFE_DELETEA( m_aIndices[i].pIndices );
	}*/

	return true;
}


bool Z3DMultipartPortion::BatchClose( IDirect3DDevice8* pDevice )
{
	if( this != m_rpThis )
	{
		return true;
	}

//	thread loading을 위해 BatchClose() 에서 Render() 로 BuildMesh() 호출 위치를 변경
//	BuildMesh( pDevice );
	m_bNeedLoadedCheck = true;




	// support for obsolete function 'TexPiece'
//	std::map<int, H3DTexPieceTag>::iterator it_t;
//	for( it_t = m_map_IdTexPieceTag.begin(); it_t != m_map_IdTexPieceTag.end(); it_t++ )
//	{
//		(it_t->second).GetObject()->Blt2Texture( *m_pTexture );
//	}
//
//	for( it_t = m_map_IdTexPieceTag2.begin(); it_t != m_map_IdTexPieceTag2.end(); it_t++ )
//	{
//		(it_t->second).GetObject()->Blt2Texture( *m_pTexture2 );
//	}

	return true;
}


void Z3DMultipartPortion::CheckLoadedStatus( IDirect3DDevice8* pDevice )
{
	if( false == m_bNeedLoadedCheck )
	{
		return;
	}

	std::map<int, Z3DLODMesh*>::iterator it;

	bool bStillLoading = false;
	for( it = m_map_Id2MeshData.begin(); it != m_map_Id2MeshData.end(); it++ )
	{
		if( it->second )
		{
			if( false == it->second->m_bLoaded )
			{
				bStillLoading = true;
			}
		}
	}

	if( false == bStillLoading )
	{
		BuildMesh( pDevice );
		m_bNeedLoadedCheck = false;
	}
}


IDirect3DVertexBuffer8* Z3DMultipartPortion::_GetVertexBufferInterface( IDirect3DDevice8* pDevice, long& rlVertexCount )
{
	
/*	IDirect3DVertexBuffer8* pVB = NULL;
	Z3DVBCacheNode* pChainNode = ms_pVBCacheChainHeader;
	Z3DVBCacheNode* pPrevNode = NULL;

	while( pChainNode )
	{
		if(	rlVertexCount <= pChainNode->lVertexCount )
		{
			pVB = pChainNode->pVB;
			rlVertexCount = pChainNode->lVertexCount;

			// 삭제될 노드의 앞-뒤 노드간 연결 설정
			if( pPrevNode )
			{
				pPrevNode->pNext = pChainNode->pNext;
			}
			
			// 삭제될 노드가 head일경우 head pointer 조정
			if( ms_pVBCacheChainHeader == pChainNode )
			{
				ms_pVBCacheChainHeader = pChainNode->pNext;
			}

			// 노드 삭제
			SAFE_DELETE( pChainNode );
			--ms_lVBCacheCount;

			break;
		}

		pPrevNode = pChainNode;
		pChainNode = pChainNode->pNext;
	}
	
	if( pVB )
	{
		return pVB;
	}

	if(CRenderOption::m_CharacterPerPixelLighting)
	{
		if( FAILED( pDevice->CreateVertexBuffer( rlVertexCount*sizeof(BumpVertex), 
					D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0,
					D3DPOOL_DEFAULT, &pVB ) ) )
		{
			return NULL;
		}
	}
	else
	{
		if( FAILED( pDevice->CreateVertexBuffer( rlVertexCount*sizeof(D3DVERTEX), 
					D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_VERTEX,
					D3DPOOL_DEFAULT, &pVB ) ) )
		{
			return NULL;
		}
	}

	return pVB;*/
	
	if(CRenderOption::m_CharacterPerPixelLighting)
	{
//		return CSceneManager::ms_pBufferPools->GetVertexBuffer(rlVertexCount*sizeof(BumpVertex),0,true);
		return CSceneManager::ms_pBufferPools->GetVertexBuffer(rlVertexCount*sizeof(BumpVertex),0,false);
	}
	else
	{
//		return CSceneManager::ms_pBufferPools->GetVertexBuffer(rlVertexCount*sizeof(D3DVERTEX),D3DFVF_VERTEX,true);
		return CSceneManager::ms_pBufferPools->GetVertexBuffer(rlVertexCount*sizeof(D3DVERTEX),D3DFVF_VERTEX,false);
	}

	return NULL;

	
}


void Z3DMultipartPortion::_ReleaseVertexBufferInterface( IDirect3DVertexBuffer8* pVB )
{
	

	if( NULL == pVB )
	{
		return;
	}
	CSceneManager::ms_pBufferPools->UnRef(Caldron::Scene::D3DBUFFEROBJ_VERTEX,pVB);
/*
	Z3DVBCacheNode* pNode;
	Z3DVBCacheNode* pPrev = NULL;

	// 캐시 오버플로시 가장 오래된 노드 삭제
	if( ms_lVBCacheCount == ms_lMaxVBCacheCount )
	{
		pNode = ms_pVBCacheChainHeader;

		while( pNode->pNext )
		{
			pPrev = pNode;
			pNode = pNode->pNext;
		}
		SAFE_RELEASE( pNode->pVB );
		SAFE_DELETE( pNode );
		if( pPrev )
		{
			pPrev->pNext = NULL;
		}
		--ms_lVBCacheCount;
	}

	pNode = new Z3DVBCacheNode;
	D3DVERTEXBUFFER_DESC d3dvbd;
	pVB->GetDesc( &d3dvbd );
	pNode->lVertexCount = d3dvbd.Size /
		((CRenderOption::m_CharacterPerPixelLighting) ? sizeof(BumpVertex) : sizeof(D3DVERTEX) );
	pNode->pVB = pVB;
	pNode->pNext = ms_pVBCacheChainHeader;
	ms_pVBCacheChainHeader = pNode;
	++ms_lVBCacheCount;
	*/
}


IDirect3DIndexBuffer8* Z3DMultipartPortion::_GetIndexBufferInterface( long lLODIndex, IDirect3DDevice8* pDevice, long rlIndexCount )
{
/*	IDirect3DIndexBuffer8* pIB = NULL;
	Z3DIBCacheNode* pChainNode = ms_apIBCacheChainHeader[lLODIndex];
	Z3DIBCacheNode* pPrevNode = NULL;

	while( pChainNode )
	{
		if(	rlIndexCount <= pChainNode->lIndexCount )
		{
			pIB = pChainNode->pIB;
			rlIndexCount = pChainNode->lIndexCount;

			// 삭제될 노드의 앞-뒤 노드간 연결 설정
			if( pPrevNode )
			{
				pPrevNode->pNext = pChainNode->pNext;
			}
			
			// 삭제될 노드가 head일경우 head pointer 조정
			if( ms_apIBCacheChainHeader[lLODIndex] == pChainNode )
			{
				ms_apIBCacheChainHeader[lLODIndex] = pChainNode->pNext;
			}

			// 노드 삭제
			SAFE_DELETE( pChainNode );
			--ms_alIBCacheCount[lLODIndex];

			break;
		}

		pPrevNode = pChainNode;
		pChainNode = pChainNode->pNext;
	}
	
	if( pIB )
	{
		return pIB;
	}

	HRESULT hr = pDevice->CreateIndexBuffer( rlIndexCount * sizeof(WORD),
									D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
									D3DPOOL_DEFAULT, &(pIB) );
	if( FAILED( hr ) )
	{
		return NULL;
	}

	return pIB;*/
//	return CSceneManager::ms_pBufferPools->GetIndexBuffer(rlIndexCount * sizeof(WORD),D3DFMT_INDEX16,true);
	return CSceneManager::ms_pBufferPools->GetIndexBuffer(rlIndexCount * sizeof(WORD),D3DFMT_INDEX16,false);
}


void Z3DMultipartPortion::_ReleaseIndexBufferInterface( long lLODIndex, IDirect3DIndexBuffer8* pIB )
{
	if( NULL == pIB )
	{
		return;
	}
	CSceneManager::ms_pBufferPools->UnRef(Caldron::Scene::D3DBUFFEROBJ_INDEX,pIB);
	/*
	Z3DIBCacheNode* pNode;
	Z3DIBCacheNode* pPrev = NULL;

	// 캐시 오버플로시 가장 오래된 노드 삭제
	if( ms_alIBCacheCount[lLODIndex] == ms_alMaxIBCacheCount[lLODIndex] )
	{
		pNode = ms_apIBCacheChainHeader[lLODIndex];

		while( pNode->pNext )
		{
			pPrev = pNode;
			pNode = pNode->pNext;
		}
		SAFE_RELEASE( pNode->pIB );
		SAFE_DELETE( pNode );
		if( pPrev )
		{
			pPrev->pNext = NULL;
		}
		--ms_alIBCacheCount[lLODIndex];
	}

	pNode = new Z3DIBCacheNode;
	D3DINDEXBUFFER_DESC d3dibd;
	pIB->GetDesc( &d3dibd );
	pNode->lIndexCount = d3dibd.Size / sizeof(WORD);
	pNode->pIB = pIB;
	pNode->pNext = ms_apIBCacheChainHeader[lLODIndex];
	ms_apIBCacheChainHeader[lLODIndex] = pNode;
	++ms_alIBCacheCount[lLODIndex];
	*/
}


void Z3DMultipartPortion::_Close()
{
	Z3DVBCacheNode* pVBChainNode = ms_pVBCacheChainHeader;
	Z3DVBCacheNode* pVBTmp;

	while( pVBChainNode )
	{
		pVBTmp = pVBChainNode;
		pVBChainNode = pVBChainNode->pNext;
		SAFE_RELEASE( pVBTmp->pVB );
		SAFE_DELETE( pVBTmp );
	}

	Z3DIBCacheNode* pIBChainNode;
	Z3DIBCacheNode* pIBTmp;

	for( int i = 0; i < Z3D_LOD_LEVEL; ++i )
	{
		pIBChainNode = ms_apIBCacheChainHeader[i];

		while( pIBChainNode )
		{
			pIBTmp = pIBChainNode;
			pIBChainNode = pIBChainNode->pNext;
			SAFE_RELEASE( pIBTmp->pIB );
			SAFE_DELETE( pIBTmp );
		}
	}
}
