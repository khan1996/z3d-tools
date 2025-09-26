// Z3DMultipartSkin.cpp: implementation of the CZ3DMultipartSkin class.
//
//////////////////////////////////////////////////////////////////////
//#include "FunctionPerformanceCheck.h"


#include "Z3DMultipartSkin.h"
#include "Vertex.h"
//#include "BaseDataDefine.h"
#include "RenderOption.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// LOD 레벨당 몇 프레임에 한번씩 blending 계산을 할 것인지 결정할때 쓰이는 테이블
long CZ3DMultipartSkin::ms_alAniLODFrameTable[Z3D_LOD_LEVEL] = { 1, 3, 5, 10 };


bool CZ3DMultipartSkin::SetMesh( const char* szMeshName )
{
	if( this != m_rpThis )
	{
		return true;
	}

	int id;
	int nPartId;

	//H3DMeshTag tagMesh;
	//g_ContLODMesh.GetObject( tagMesh, szMeshName );
	//nPartId = tagMesh.GetObject()->GetId();
	
	Z3DLODMesh* pMesh;
	pMesh = g_ContLODMesh.GetObject( szMeshName );
	nPartId = pMesh->GetId();

	id = GetPortionIndex( nPartId );
	if( -1 == id )	// 잘못된 id
	{
		// 현재 CBaseCacheMgr 은 release 관련 메소드가 없다
		//tagMesh.Release();
		return false;
	}
	if( m_apPortion[id]->SetMesh( nPartId, pMesh ) )	
	//if( m_apPortion[id]->SetMesh( nPartId, tagMesh ) )
	{
		if( !m_bIsBatchMode )
		{
			m_lAniLODCounter = 0; // 메쉬가 바뀌었을 경우 blending 재계산이 필요하므로
			m_apPortion[id]->BuildMesh( GetDevice() );
		}

		return true;
	}

	return false;
}
	

bool CZ3DMultipartSkin::SetTex( const char* szTexName, const char* szTex2Name )
{
	if( this != m_rpThis )
	{
		return true;
	}

	int id;
	int nPartId;

	H3DTexPieceTag tagTexPiece;
	g_ContTexturePiece.GetObject( tagTexPiece, szTexName );
	nPartId = tagTexPiece.GetObject()->GetId();
	id = GetPortionIndex( nPartId );
	if( -1 == id )	// 잘못된 id
	{
		tagTexPiece.Release();
		return false;
	}

	if( !m_bIsBatchMode )
	{
		// 텍스처가 비어 있을때 null 텍스처 생성
		if( true == m_apPortion[id]->m_pTexture->IsEmpty() )
		{
			if( 0 == id )
			{
//				if( false == m_apPortion[id]->m_pTexture->Load( "NULL_512x512.dds" ) )
//				{
//					_ASSERT(false); // error reading default texture!
//					return false;
//				}
//			}
//			else
//			{
//				if( false == m_apPortion[id]->m_pTexture->Load( "NULL_256x512.dds" ) )
//				{
//					_ASSERT(false); // error reading default texture!
//					return false;
//				}
			}
		}
	}
			
	if( m_apPortion[id]->SetTexPiece( nPartId, tagTexPiece ) )
	{
		if( !m_bIsBatchMode )
		{
			tagTexPiece.GetObject()->Blt2Texture( m_apPortion[id]->m_pTexture->GetD3dTexture() );
		}
	}

	// secondary texture 처리
	if( NULL == szTex2Name )
	{
		return true;
	}

	g_ContTexturePiece.GetObject( tagTexPiece, szTex2Name );
	nPartId = tagTexPiece.GetObject()->GetId();
	id = GetPortionIndex( nPartId );
	if( -1 == id )	// 잘못된 id
	{
		tagTexPiece.Release();
		return false;
	}

	if( !m_bIsBatchMode )
	{
		// 텍스처가 비어 있을때 null 텍스처 생성
		if( true == m_apPortion[id]->m_pTexture2->IsEmpty() )
		{
			if( 0 == id )
			{
//				if( false == m_apPortion[id]->m_pTexture2->Load( "BUMP_512x512.dds" ) )
//				{
//					_ASSERT(false); // error reading default texture!
//					return false;
//				}
//			}
//			else
//			{
//				if( false == m_apPortion[id]->m_pTexture2->Load( "BUMP_256x512.dds" ) )
//				{
//					_ASSERT(false); // error reading default texture!
//					return false;
//				}
			}
		}
	}
			
	if( m_apPortion[id]->SetTexPiece2( nPartId, tagTexPiece ) )
	{
		if( !m_bIsBatchMode )
		{
			tagTexPiece.GetObject()->Blt2Texture( m_apPortion[id]->m_pTexture2->GetD3dTexture() );
		}

		return true;
	}

	return false;
}

int CZ3DMultipartSkin::GetIdFromMeshName( char* szMeshName )
{
	char* pNumberBegin = strrchr( szMeshName, '_' );

	if( NULL == pNumberBegin )
	{
		return 0;
	}

	++pNumberBegin;	// 숫자는 '_' 바로 다음 문자부터 시작이므로

	char* pNumberEnd = strrchr( szMeshName, '.' );

	if( NULL == pNumberEnd )
	{
		return 0;
	}

	int nNumLen = pNumberEnd - pNumberBegin;
	if( nNumLen < 0 )
	{
		return 0;
	}

	char szTmp[10];

	strncpy( szTmp, pNumberBegin, nNumLen );
	szTmp[nNumLen] = '\0';

	return atoi( szTmp );	
}


bool CZ3DMultipartSkin::SetMeshTexture( const char* szMeshName, const char* szTextureName, const char* szTexture2Name, long lGradeEffectIndex, long lGlowIndex )
{
	if( this != m_rpThis )
	{
		return true;
	}

//DeclareBlockTimingCheck("mesh GetObject", aa);
//DeclareBlockTimingCheck("tex1 GetObject", bb);
//DeclareBlockTimingCheck("tex2 GetObject", cc);

	int id;
	int nPartId;

	Z3DLODMesh* pMesh;

//	H3DMeshTag tagMesh;
//BlockTimingCheckStart(aa);
	//g_ContLODMesh.GetObject( tagMesh, szMeshName );
	pMesh = g_ContLODMesh.GetObject( szMeshName );

//BlockTimingCheckStop(aa);
	//nPartId = tagMesh.GetObject()->GetId();
	nPartId = GetIdFromMeshName(const_cast<char*>(szMeshName));

	id = GetPortionIndex( nPartId );
	if( -1 == id )	// 잘못된 id
	{
		//  릴리즈
		//tagMesh.Release();
		return false;
	}

	m_apPortion[id]->SetGradeEffectIndex( nPartId, lGradeEffectIndex );
	m_apPortion[id]->SetGlowIndex( nPartId, lGlowIndex );

	if( m_apPortion[id]->SetMesh( nPartId, pMesh )  )
	//if( m_apPortion[id]->SetMesh( nPartId, tagMesh )  )
	{
		if( !m_bIsBatchMode )
		{
			m_lAniLODCounter = 0; // 메쉬가 바뀌었을 경우 blending 재계산이 필요하므로

			m_apPortion[id]->BuildMesh( GetDevice() );

		}
	}

	Z3DTexture* pTexture;
//	H3DTextureTag tagTexture;
	if( NULL == szTextureName )
	{
		m_apPortion[id]->DeleteTexture( nPartId );
	}
	else
	{
//BlockTimingCheckStart(bb);
		pTexture = g_ContTexture.GetObject( szTextureName );
		//g_ContTexture.GetObject( tagTexture, szTextureName );
//BlockTimingCheckStop(bb);
		m_apPortion[id]->SetTexture( nPartId, pTexture );
	/*	if( NULL == tagTexture.GetObject() )
		{
			WRONG_WAY( "Texture file not found!" );
		}
		m_apPortion[id]->SetTexture( nPartId, tagTexture );*/

	}

	if( NULL == szTexture2Name )
	{
		m_apPortion[id]->DeleteTexture2( nPartId );
		return true;
	}

//BlockTimingCheckStart(cc);
	pTexture = g_ContTexture.GetObject( szTexture2Name );
//BlockTimingCheckStop(cc);

	return m_apPortion[id]->SetTexture2( nPartId, pTexture );
/*
//BlockTimingCheckStart(cc);
	g_ContTexture.GetObject( tagTexture, szTexture2Name );
//BlockTimingCheckStop(cc);
	if( NULL == tagTexture.GetObject() )
	{
		WRONG_WAY( "Texture file not found!" );
	}

	return m_apPortion[id]->SetTexture2( nPartId, tagTexture );*/
}


bool CZ3DMultipartSkin::DeleteMesh( int nPartId )
{
	if( this != m_rpThis )
	{
		return true;
	}

	int id = GetPortionIndex( nPartId );
	if( -1 == id )
	{
		return false;
	}

	if( m_apPortion[id]->DeleteMesh( nPartId ) )
	{
		if( !m_bIsBatchMode )
		{
			m_apPortion[id]->BuildMesh( GetDevice() );
			m_lAniLODCounter = 0; // 메쉬가 바뀌었을 경우 blending 재계산이 필요하므로
		}

		return true;
	}

	return false;
}

bool CZ3DMultipartSkin::DeleteTex( int nPartId )
{
	if( this != m_rpThis )
	{
		return true;
	}

	int id = GetPortionIndex( nPartId );
	if( -1 == id )
	{
		return false;
	}

	if( m_apPortion[id]->DeleteTexPiece( nPartId ) )
	{
		if( !m_bIsBatchMode )
		{
			if( m_apPortion[id]->m_map_IdTexPieceTag.empty() )
			{
				m_apPortion[id]->m_pTexture->Flush();
			}
		}

		return true;
	}

	return false;
}


bool CZ3DMultipartSkin::BatchOpen()
{
	if( this != m_rpThis )
	{
		return true;
	}

	//_ASSERTE( !m_bIsBatchMode );
	if( m_bIsBatchMode )
	{
		return false;
	}

	// batchopen으로 값 리셋하는 부분은 build mesh에 모두 포함되므로, 따로 호출하지 않는다.
	/*for( int i = 0; i < Z3D_MULTIPART_PORTION_COUNT; i++ )
	{
		m_apPortion[i]->BatchOpen();
	}*/

	m_bIsBatchMode = true;

	return true;
}


bool CZ3DMultipartSkin::BatchClose()
{
	if( this != m_rpThis )
	{
		return true;
	}

	//_ASSERTE( m_bIsBatchMode );
	if( false == m_bIsBatchMode )
	{
		return false;
	}


	for( int i = 0; i < Z3D_MULTIPART_PORTION_COUNT; i++ )
	{
		if( Z3D_MPT_TEXPIECE == m_MPT )
		{
			if( m_apPortion[i]->m_map_IdTexPieceTag.empty() )
			{
				m_apPortion[i]->m_pTexture->Flush();
			}
			else if( m_apPortion[i]->m_pTexture->IsEmpty() )
			{
//				if( 0 == i )
//				{
//					if( false == m_apPortion[i]->m_pTexture->Load( "NULL_512x512.dds" ) )
//					{
//						return false;
//					}
//				}
//				else
//				{
//					if( false == m_apPortion[i]->m_pTexture->Load( "NULL_256x512.dds" ) )
//					{
//						return false;
//					}
//				}
			}

			if( m_apPortion[i]->m_map_IdTexPieceTag2.empty() )
			{
				m_apPortion[i]->m_pTexture2->Flush();
			}
			else if( m_apPortion[i]->m_pTexture2->IsEmpty() )
			{
//				if( 0 == i )
//				{
//					if( false == m_apPortion[i]->m_pTexture2->Load( "BUMP_512x512.dds" ) )
//					{
//						return false;
//					}
//				}
//				else
//				{
//					if( false == m_apPortion[i]->m_pTexture2->Load( "BUMP_256x512.dds" ) )
//					{
//						return false;
//					}
//				}
			}
		}

		m_apPortion[i]->BatchClose( GetDevice() );
	}

	m_lAniLODCounter = 0; // 메쉬가 바뀌어 blending 재계산이 필요하므로

	m_bIsBatchMode = false;

	return true;
}

vector3 ComputeTangentVector(BumpVertex pVtxA, BumpVertex pVtxB,BumpVertex pVtxC )
{
	vector3 vAB=pVtxB.v-pVtxA.v;
	vector3 vAC=pVtxC.v-pVtxA.v;

	vector3 n=pVtxA.n;
    
	vector3 vProjAB=vAB-( (n*vAB)*n );
	vector3 vProjAC=vAC-( (n*vAC)*n );

	float duAB=pVtxB.tu-pVtxA.tu;
	float duAC=pVtxC.tu-pVtxA.tu;

	float dvAB=pVtxB.tv-pVtxA.tv;
	float dvAC=pVtxC.tv-pVtxA.tv;

	if( duAC*dvAB > duAB*dvAC )
	{
		duAC=-duAC;
		duAB=-duAB;
	}
    
	vector3 vTangent=duAC*vProjAB-duAB*vProjAC;

	vTangent.Normalize();
	return vTangent;   
    
}


void CalcTagentSpaceVector(BumpVertex *pVertices,WORD *pIndices,int cVertex,int cIndices,vector3 *pU)
{
	memset(pU,0,sizeof(vector3)*cVertex);

	for(int i=0;i<cIndices*3;i+=3)
	{
		WORD a=pIndices[i+0];
		WORD b=pIndices[i+1];
		WORD c=pIndices[i+2];

		pU[a]+=ComputeTangentVector(pVertices[a],pVertices[b],pVertices[c]);
		pU[b]+=ComputeTangentVector(pVertices[b],pVertices[a],pVertices[c]);
		pU[c]+=ComputeTangentVector(pVertices[c],pVertices[a],pVertices[b]);
	}

	for(int i=0;i<cVertex;i++)
	{
		pU[i].Normalize();

	}
}


void CZ3DMultipartSkin::ApplyAnimation()
{
		
	if( this != m_rpThis )
	{
		return;
	}

	if( m_bIsBatchMode )
	{
		return;
	}
	for( int i = 0; i < Z3D_MULTIPART_PORTION_COUNT; i++ )
	{
		m_apPortion[i]->CheckLoadedStatus( GetDevice() );
	}


	// allocation safety check
	for(int i = 0; i < Z3D_MULTIPART_PORTION_COUNT; ++i )
	{
		if( NULL == m_apPortion[i]->m_pVertexBuffer )
		{
			return;
		}
	}


	// bounding box 최소값은 가장 큰 값으로, 최대값은 가장 작은 값으로 초기화
	m_vBoundingBoxMin = vector3( 4.2e9f, 4.2e9f, 4.2e9f );
	m_vBoundingBoxMax = vector3( -4.2e9f, -4.2e9f, -4.2e9f );

	if( CRenderOption::m_CharacterPerPixelLighting )
	{
		//static bFirstRender=true;
		//static vector3 vecU[2000];		
		

		BumpVertex* pVertices; // shortcut to render vertex pool of portion
		Z3DBlend2Vertex* pBlVertex;
		matrix* pMat1,* pMat2; // fixed as 2 for now
		float factor1, factor2;
		int i, j, k, l;
		Z3DLODMesh* pMesh;
		
		float f1, f2, f3, f4;

		// LOD level 구함 : 뼈다구의 root위치를 기준으로 - 차후에 다른 방법으로 바꿀지도?
		// 외부에서 LOD 레벨 설정해주는것으로 바뀜.
		//pMat1 = m_rpSkeleton[0].GetTM();
		int nLODLevel = GetLODLevel();

		if( 1 )//CheckRecalcByAniLOD() )
		{
			// 원TM의 역행렬을 구해진 ani TM에 곱해준다.
			for( i = 0; i < m_lSkelCount; i++ )
			{
				if( m_bTestMode )
				{
					m_pMatrixPalette[i].MakeIdent();
				}
				else
				{
					m_rpSkeleton[i].GetLocalizedTM( m_pMatrixPalette[i], m_rpSkeletonLocalizer[i] );
				}
			}

			for( i = 0; i < Z3D_MULTIPART_PORTION_COUNT; i++ )
			{
				if( 0 == m_apPortion[i]->m_vec_pMesh.size() )
				{
					continue;
				}

				j = 0;
				//pVertices = m_apPortion[i]->m_pVertices;
//				m_apPortion[i]->m_pVertexBuffer->Lock( 0, 0, (BYTE**)(&pVertices), D3DLOCK_DISCARD );
				m_apPortion[i]->m_pVertexBuffer->Lock( 0, 0, (BYTE**)(&pVertices), 0 );
				
				for( l = 0; l < (int)m_apPortion[i]->m_vec_pMesh.size(); l++ )
				{
					pMesh = m_apPortion[i]->m_vec_pMesh[l];

					for( k = 0; k < pMesh->lVertexCount; k++ )
					{
						if( pMesh->pVertices[k].lodLevel >= nLODLevel )
						{
							pBlVertex = &(pMesh->pVertices[k]);
/*
							// edith 2008.06.19 스켈레탈 처리
							if(m_lSkelCount <= pBlVertex->mtrx_id[0])
								continue;
							else if(m_lSkelCount <= pBlVertex->mtrx_id[1])
								continue;
*/							
							pMat1 = &(m_pMatrixPalette[pBlVertex->mtrx_id[0]]);
							pMat2 = &(m_pMatrixPalette[pBlVertex->mtrx_id[1]]);

							factor1 = pBlVertex->bfactor;
							factor2 = 1.0f - factor1;

							f1 = (factor1*pMat1->_11) + (factor2*pMat2->_11);
							f2 = (factor1*pMat1->_21) + (factor2*pMat2->_21);
							f3 = (factor1*pMat1->_31) + (factor2*pMat2->_31);
							f4 = (factor1*pMat1->_41) + (factor2*pMat2->_41);
							pVertices[j].v.x = (pBlVertex->pos.x*f1 + pBlVertex->pos.y*f2 + pBlVertex->pos.z*f3 + f4) * m_fScaleFactor;
							pVertices[j].u.x = (pBlVertex->u.x*f1 + pBlVertex->u.y*f2 + pBlVertex->u.z*f3);
							pVertices[j].n.x = pBlVertex->normal.x*f1 + pBlVertex->normal.y*f2 + pBlVertex->normal.z*f3;

							f1 = (factor1*pMat1->_12) + (factor2*pMat2->_12);
							f2 = (factor1*pMat1->_22) + (factor2*pMat2->_22);
							f3 = (factor1*pMat1->_32) + (factor2*pMat2->_32);
							f4 = (factor1*pMat1->_42) + (factor2*pMat2->_42);
							pVertices[j].v.y = (pBlVertex->pos.x*f1 + pBlVertex->pos.y*f2 + pBlVertex->pos.z*f3 + f4) * m_fScaleFactor;
							pVertices[j].u.y = (pBlVertex->u.x*f1 + pBlVertex->u.y*f2 + pBlVertex->u.z*f3);
							pVertices[j].n.y = pBlVertex->normal.x*f1 + pBlVertex->normal.y*f2 + pBlVertex->normal.z*f3;

							f1 = (factor1*pMat1->_13) + (factor2*pMat2->_13);
							f2 = (factor1*pMat1->_23) + (factor2*pMat2->_23);
							f3 = (factor1*pMat1->_33) + (factor2*pMat2->_33);
							f4 = (factor1*pMat1->_43) + (factor2*pMat2->_43);
							pVertices[j].v.z = (pBlVertex->pos.x*f1 + pBlVertex->pos.y*f2 + pBlVertex->pos.z*f3 + f4) * m_fScaleFactor;
							pVertices[j].u.z = (pBlVertex->u.x*f1 + pBlVertex->u.y*f2 + pBlVertex->u.z*f3);
							pVertices[j].n.z = pBlVertex->normal.x*f1 + pBlVertex->normal.y*f2 + pBlVertex->normal.z*f3;

							pVertices[j].tu = pBlVertex->tu;
							pVertices[j].tv = pBlVertex->tv;

							// bounding box min/max 값 구함
							m_vBoundingBoxMin.x = min( pVertices[j].v.x, m_vBoundingBoxMin.x );
							m_vBoundingBoxMin.y = min( pVertices[j].v.y, m_vBoundingBoxMin.y );
							m_vBoundingBoxMin.z = min( pVertices[j].v.z, m_vBoundingBoxMin.z );

							m_vBoundingBoxMax.x = max( pVertices[j].v.x, m_vBoundingBoxMax.x );
							m_vBoundingBoxMax.y = max( pVertices[j].v.y, m_vBoundingBoxMax.y );
							m_vBoundingBoxMax.z = max( pVertices[j].v.z, m_vBoundingBoxMax.z );
						}
						j++;
					}
				}

				m_apPortion[i]->m_pVertexBuffer->Unlock();
			}
		}					
	}
	else
	{
		// blending 계산 및 텍스처 세팅, 렌더링
		D3DVERTEX* pVertices; // shortcut to render vertex pool of portion
		Z3DBlend2Vertex* pBlVertex;
		matrix* pMat1,* pMat2; // fixed as 2 for now
		float factor1, factor2;
		int i, j, k, l;
		Z3DLODMesh* pMesh;
		matrix m;
		float f1, f2, f3, f4;

		// LOD level 구함 : 뼈다구의 root위치를 기준으로 - 차후에 다른 방법으로 바꿀지도?
		// 외부에서 LOD 레벨 설정해주는것으로 바뀜.
		//pMat1 = m_rpSkeleton[0].GetTM();
		int nLODLevel = GetLODLevel();

		if( 1 )//CheckRecalcByAniLOD() )
		{
			// 원TM의 역행렬을 구해진 ani TM에 곱해준다.
			for( i = 0; i < m_lSkelCount; i++ )
			{
				if( m_bTestMode )
				{
					m_pMatrixPalette[i].MakeIdent();
				}
				else
				{
					m_rpSkeleton[i].GetLocalizedTM( m_pMatrixPalette[i], m_rpSkeletonLocalizer[i] );
				}
			}

			for( i = 0; i < Z3D_MULTIPART_PORTION_COUNT; i++ )
			{
				if( 0 == m_apPortion[i]->m_vec_pMesh.size() )
				{
					continue;
				}

				j = 0;
//				m_apPortion[i]->m_pVertexBuffer->Lock( 0, 0, (BYTE**)(&pVertices), D3DLOCK_DISCARD );
				m_apPortion[i]->m_pVertexBuffer->Lock( 0, 0, (BYTE**)(&pVertices), 0 );

				for( l = 0; l < (int)m_apPortion[i]->m_vec_pMesh.size(); l++ )
				{
					pMesh = m_apPortion[i]->m_vec_pMesh[l];

					for( k = 0; k < pMesh->lVertexCount; k++ )
					{
						if( pMesh->pVertices[k].lodLevel >= nLODLevel )
						{
							pBlVertex = &(pMesh->pVertices[k]);
/*
							// edith 스킨
							// edith 2008.06.19 스켈레탈 처리
							if(m_lSkelCount <= pBlVertex->mtrx_id[0])
								continue;
							else if(m_lSkelCount <= pBlVertex->mtrx_id[1])
								continue;
*/
							pMat1 = &(m_pMatrixPalette[pBlVertex->mtrx_id[0]]);
							pMat2 = &(m_pMatrixPalette[pBlVertex->mtrx_id[1]]);

							factor1 = pBlVertex->bfactor;
							factor2 = 1.0f - factor1;

							f1 = (factor1*pMat1->_11) + (factor2*pMat2->_11);
							f2 = (factor1*pMat1->_21) + (factor2*pMat2->_21);
							f3 = (factor1*pMat1->_31) + (factor2*pMat2->_31);
							f4 = (factor1*pMat1->_41) + (factor2*pMat2->_41);
							pVertices[j].x = (pBlVertex->pos.x*f1 + pBlVertex->pos.y*f2 + pBlVertex->pos.z*f3 + f4) * m_fScaleFactor;
							pVertices[j].nx = pBlVertex->normal.x*f1 + pBlVertex->normal.y*f2 + pBlVertex->normal.z*f3;

							f1 = (factor1*pMat1->_12) + (factor2*pMat2->_12);
							f2 = (factor1*pMat1->_22) + (factor2*pMat2->_22);
							f3 = (factor1*pMat1->_32) + (factor2*pMat2->_32);
							f4 = (factor1*pMat1->_42) + (factor2*pMat2->_42);
							pVertices[j].y = (pBlVertex->pos.x*f1 + pBlVertex->pos.y*f2 + pBlVertex->pos.z*f3 + f4) * m_fScaleFactor;
							pVertices[j].ny = pBlVertex->normal.x*f1 + pBlVertex->normal.y*f2 + pBlVertex->normal.z*f3;

							f1 = (factor1*pMat1->_13) + (factor2*pMat2->_13);
							f2 = (factor1*pMat1->_23) + (factor2*pMat2->_23);
							f3 = (factor1*pMat1->_33) + (factor2*pMat2->_33);
							f4 = (factor1*pMat1->_43) + (factor2*pMat2->_43);
							pVertices[j].z = (pBlVertex->pos.x*f1 + pBlVertex->pos.y*f2 + pBlVertex->pos.z*f3 + f4) * m_fScaleFactor;
							pVertices[j].nz = pBlVertex->normal.x*f1 + pBlVertex->normal.y*f2 + pBlVertex->normal.z*f3;

							pVertices[j].tu = pBlVertex->tu;
							pVertices[j].tv = pBlVertex->tv;

							// bounding box min/max 값 구함
							m_vBoundingBoxMin.x = min( pVertices[j].x, m_vBoundingBoxMin.x );
							m_vBoundingBoxMin.y = min( pVertices[j].y, m_vBoundingBoxMin.y );
							m_vBoundingBoxMin.z = min( pVertices[j].z, m_vBoundingBoxMin.z );

							m_vBoundingBoxMax.x = max( pVertices[j].x, m_vBoundingBoxMax.x );
							m_vBoundingBoxMax.y = max( pVertices[j].y, m_vBoundingBoxMax.y );
							m_vBoundingBoxMax.z = max( pVertices[j].z, m_vBoundingBoxMax.z );
						}
						j++;
					}
				}

				m_apPortion[i]->m_pVertexBuffer->Unlock();
			}
		}
	}

	m_vBoundingBoxMin += m_vPivotPos;
	m_vBoundingBoxMax += m_vPivotPos;
}


void CZ3DMultipartSkin::Render()
{
	if( this != m_rpThis )
	{
		return;
	}

	int i, j;
	int nLODLevel = GetLODLevel();

	for( i = 0; i < Z3D_MULTIPART_PORTION_COUNT; ++i )
	{
		if( NULL == m_apPortion[i]->m_pVertexBuffer )
		{
			return;
		}

		if( NULL == m_apPortion[i]->m_apIndexBuffer[nLODLevel] )
		{
			return;
		}
	}

	if( m_bIsBatchMode )
	{
		return;
	}

	long lVertexOffset, lIndexOffset;
	Z3DLODMesh* pMesh = NULL;

	if(CRenderOption::m_CharacterPerPixelLighting)
	{
		//GetDevice()->SetVertexShader( BUMPVERTEXFVF );
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

	for( i = 0; i < Z3D_MULTIPART_PORTION_COUNT; i++ )
	{
		if( m_apPortion[i]->m_alIndexCount[nLODLevel] )
		{
			if( Z3D_MPT_TEXPIECE == m_apPortion[i]->m_MPT )
			{
				GetDevice()->SetTexture( 0, m_apPortion[i]->m_pTexture->GetD3dTexture() );
				GetDevice()->SetTexture( 1, m_apPortion[i]->m_pTexture2->GetD3dTexture() );
			}
			
			if(CRenderOption::m_CharacterPerPixelLighting)
			{
				GetDevice()->SetStreamSource( 0, m_apPortion[i]->m_pVertexBuffer, sizeof(BumpVertex) );
			}			
			else
			{
				GetDevice()->SetStreamSource( 0, m_apPortion[i]->m_pVertexBuffer, sizeof(D3DVERTEX) );
			}
			GetDevice()->SetIndices( m_apPortion[i]->m_apIndexBuffer[nLODLevel], 0 );

			lVertexOffset = 0;
			lIndexOffset = 0;
			for( j = 0; j < (int)m_apPortion[i]->m_vec_pMesh.size(); ++j )
			{
				pMesh = m_apPortion[i]->m_vec_pMesh[j];
				if( pMesh->aIndex[nLODLevel].lIndexCount )
				{

					if( Z3D_MPT_TEXTURE == m_apPortion[i]->m_MPT )
					{
						if( m_apPortion[i]->m_vec_pTexture[j] )
						{
							GetDevice()->SetTexture( 0, m_apPortion[i]->m_vec_pTexture[j]->GetD3dTexture() );
						}
						else
						{
							GetDevice()->SetTexture( 0, NULL );
						}
//						if( m_apPortion[i]->m_vec_pTexture2[j] )
//						{
//							GetDevice()->SetTexture( 1, *(m_apPortion[i]->m_vec_pTexture2[j]) );
//						}
//						else
//						{
//							GetDevice()->SetTexture( 1, NULL );
//						}
					}


//static int nTexTransformCounter = 0;
//int cTemp = (nTexTransformCounter < 1200) ? 384*sinf(3.1415926535f*nTexTransformCounter/1200.0f) : 0;
//if( cTemp > 255 ) cTemp = 255;
//GetDevice()->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( cTemp, 0, cTemp, 0 ) );
////GetDevice()->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
////GetDevice()->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
//if( ++nTexTransformCounter == 2000 )
//{
//	nTexTransformCounter = 0;
//}

                    m_apPortion[i]->m_vec_pGradeEffectHandler[j]->ApplySetting( GetDevice() );

					HRESULT hr = GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, lVertexOffset,
								pMesh->lVertexCount, lIndexOffset,
								(pMesh->aIndex[nLODLevel].lIndexCount)/3 );

				}
				lVertexOffset += pMesh->lVertexCount;
				lIndexOffset += pMesh->aIndex[nLODLevel].lIndexCount;
			}
		}
	}
	IncreaseAniLODCounter();
}


void CZ3DMultipartSkin::RenderShadow( IDirect3DDevice8* pDevice, DWORD vertexShader, int nLevel )
{
	if( this != m_rpThis )
	{
		return;
	}

	int i, j;
//	int nLODLevel = ( -1 == nLevel ) ? ( (m_lLODLevel+Z3D_LOD_LEVEL-1)/2 ) : min( Z3D_LOD_LEVEL, nLevel );
	int nLODLevel = ( -1 == nLevel ) ? m_lLODLevel : min( Z3D_LOD_LEVEL, nLevel );
	

	for( i = 0; i < Z3D_MULTIPART_PORTION_COUNT; ++i )
	{
		if( NULL == m_apPortion[i]->m_pVertexBuffer )
		{
			return;
		}

		if( NULL == m_apPortion[i]->m_apIndexBuffer[nLODLevel] )
		{
			return;
		}
	}

	//_ASSERTE( !m_bIsBatchMode );

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

	Z3DLODMesh* pMesh;
	

	long lVertexOffset, lIndexOffset;
	for( i = 0; i < Z3D_MULTIPART_PORTION_COUNT; i++ )
	{
		if( m_apPortion[i]->m_alIndexCount[nLODLevel] )
		{
			if(CRenderOption::m_CharacterPerPixelLighting)
			{
				GetDevice()->SetStreamSource( 0, m_apPortion[i]->m_pVertexBuffer, sizeof(BumpVertex) );
			}
			else
			{
				GetDevice()->SetStreamSource( 0, m_apPortion[i]->m_pVertexBuffer, sizeof(D3DVERTEX) );
			}
			
			lVertexOffset = 0;
			lIndexOffset = 0;
			for( j = 0; j < (int)m_apPortion[i]->m_vec_pMesh.size(); ++j )
			{
				pMesh = m_apPortion[i]->m_vec_pMesh[j];
				if( pMesh->aIndex[nLODLevel].lIndexCount )
				{
					GetDevice()->SetIndices( m_apPortion[i]->m_apIndexBuffer[nLODLevel], 0 );

					GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, lVertexOffset,
								pMesh->lVertexCount, lIndexOffset,
								(pMesh->aIndex[nLODLevel].lIndexCount)/3 );
				}
				lVertexOffset += pMesh->lVertexCount;
				lIndexOffset += pMesh->aIndex[nLODLevel].lIndexCount;
			}

			GetDevice()->SetStreamSource( 0, NULL, 0 );
			GetDevice()->SetIndices( NULL, 0 );
		}
	}
}


void CZ3DMultipartSkin::RenderGlowShape( IDirect3DDevice8* pDevice )
{
	if( this != m_rpThis )
	{
		return;
	}

	int i, j;

	for( i = 0; i < Z3D_MULTIPART_PORTION_COUNT; ++i )
	{
		if( NULL == m_apPortion[i]->m_pVertexBuffer )
		{
			return;
		}

		if( NULL == m_apPortion[i]->m_apIndexBuffer[m_lLODLevel] )
		{
			return;
		}
	}

	//_ASSERTE( !m_bIsBatchMode );

	GetDevice()->SetVertexShader( D3DFVF_VERTEX );

	matrix m;
	m.MakeIdent();
	m._41 = m_vPivotPos.x;
	m._42 = m_vPivotPos.y;
	m._43 = m_vPivotPos.z;
	GetDevice()->SetTransform( D3DTS_WORLD, m );

	Z3DLODMesh* pMesh;


	long lVertexOffset, lIndexOffset;
	for( i = 0; i < Z3D_MULTIPART_PORTION_COUNT; i++ )
	{
		if( m_apPortion[i]->m_alIndexCount[m_lLODLevel] )
		{
			GetDevice()->SetStreamSource( 0, m_apPortion[i]->m_pVertexBuffer, sizeof(D3DVERTEX) );

			lVertexOffset = 0;
			lIndexOffset = 0;
			for( j = 0; j < (int)m_apPortion[i]->m_vec_pMesh.size(); ++j )
			{
				pMesh = m_apPortion[i]->m_vec_pMesh[j];
				m_apPortion[i]->m_vec_pGlowHandler[j]->ApplySetting( GetDevice() );
				if( pMesh->aIndex[m_lLODLevel].lIndexCount )
				{
					GetDevice()->SetIndices( m_apPortion[i]->m_apIndexBuffer[m_lLODLevel], 0 );

					GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, lVertexOffset,
						pMesh->lVertexCount, lIndexOffset,
						(pMesh->aIndex[m_lLODLevel].lIndexCount)/3 );
				}
				lVertexOffset += pMesh->lVertexCount;
				lIndexOffset += pMesh->aIndex[m_lLODLevel].lIndexCount;
			}

			GetDevice()->SetStreamSource( 0, NULL, 0 );
			GetDevice()->SetIndices( NULL, 0 );
		}
	}
}


void CZ3DMultipartSkin::GetBoundingBox( vector3& r_vmin, vector3& r_vmax )
{
	if( this != m_rpThis )
	{
		return;
	}

	int i, j;
	r_vmin = vector3( 0, 0, 0 );
	r_vmax = vector3( 0, 0, 0 );

	D3DVERTEXBUFFER_DESC vd;
	
	if(CRenderOption::m_CharacterPerPixelLighting)
	{
		BumpVertex* pVertices;
		
		for( i = 0; i < Z3D_MULTIPART_PORTION_COUNT; i++ )
		{
			if( 0 == m_apPortion[i]->m_vec_pMesh.size() )
			{
				continue;
			}
			// allocation safety check
			if( NULL == m_apPortion[i]->m_pVertexBuffer )
			{
				continue;
			}
			m_apPortion[i]->m_pVertexBuffer->GetDesc( &vd );
//			m_apPortion[i]->m_pVertexBuffer->Lock( 0, 0, (BYTE**)(&pVertices), D3DLOCK_DISCARD );
			m_apPortion[i]->m_pVertexBuffer->Lock( 0, 0, (BYTE**)(&pVertices), 0 );
			
			for( j = 0; j < (int)(vd.Size/sizeof(BumpVertex)); ++j )
			{
				if( pVertices[j].v.x < r_vmin.x )
				{
					r_vmin.x = pVertices[j].v.x;
				}
				if( pVertices[j].v.y < r_vmin.y )
				{
					r_vmin.y = pVertices[j].v.y;
				}
				if( pVertices[j].v.z < r_vmin.z )
				{
					r_vmin.z = pVertices[j].v.z;
				}
				
				if( pVertices[j].v.x > r_vmax.x )
				{
					r_vmax.x = pVertices[j].v.x;
				}
				if( pVertices[j].v.y > r_vmax.y )
				{
					r_vmax.y = pVertices[j].v.y;
				}
				if( pVertices[j].v.z > r_vmax.z )
				{
					r_vmax.z = pVertices[j].v.z;
				}
			}
			
			m_apPortion[i]->m_pVertexBuffer->Unlock();
		}
	}
	else
	{
		D3DVERTEX* pVertices;

		for( i = 0; i < Z3D_MULTIPART_PORTION_COUNT; i++ )
		{
			if( 0 == m_apPortion[i]->m_vec_pMesh.size() )
			{
				continue;
			}
			// allocation safety check
			if( NULL == m_apPortion[i]->m_pVertexBuffer )
			{
				continue;
			}

			m_apPortion[i]->m_pVertexBuffer->GetDesc( &vd );
//			m_apPortion[i]->m_pVertexBuffer->Lock( 0, 0, (BYTE**)(&pVertices), D3DLOCK_DISCARD );
			m_apPortion[i]->m_pVertexBuffer->Lock( 0, 0, (BYTE**)(&pVertices), 0 );
			
			for( j = 0; j < (int)(vd.Size/sizeof(D3DVERTEX)); ++j )
			{
				if( pVertices[j].x < r_vmin.x )
				{
					r_vmin.x = pVertices[j].x;
				}
				if( pVertices[j].y < r_vmin.y )
				{
					r_vmin.y = pVertices[j].y;
				}
				if( pVertices[j].z < r_vmin.z )
				{
					r_vmin.z = pVertices[j].z;
				}
				
				if( pVertices[j].x > r_vmax.x )
				{
					r_vmax.x = pVertices[j].x;
				}
				if( pVertices[j].y > r_vmax.y )
				{
					r_vmax.y = pVertices[j].y;
				}
				if( pVertices[j].z > r_vmax.z )
				{
					r_vmax.z = pVertices[j].z;
				}
			}
			
			m_apPortion[i]->m_pVertexBuffer->Unlock();
		}
	}
}


void CZ3DMultipartSkin::GetProcessedVertex( Z3DBlend2Vertex& v_in, vector3& v_out ) const
{
	if( this != m_rpThis )
	{
		return;
	}

	matrix m2[2];

	m_rpSkeleton[v_in.mtrx_id[0]].GetLocalizedTM( m2[0], m_rpSkeletonLocalizer[v_in.mtrx_id[0]] );
	m_rpSkeleton[v_in.mtrx_id[1]].GetLocalizedTM( m2[1], m_rpSkeletonLocalizer[v_in.mtrx_id[1]] );

	float factor1 = v_in.bfactor;
	float factor2 = 1.0f - factor1;

	float f1 = (factor1*m2[0]._11) + (factor2*m2[1]._11);
	float f2 = (factor1*m2[0]._21) + (factor2*m2[1]._21);
	float f3 = (factor1*m2[0]._31) + (factor2*m2[1]._31);
	float f4 = (factor1*m2[0]._41) + (factor2*m2[1]._41);
	v_out.x = (v_in.pos.x*f1 + v_in.pos.y*f2 + v_in.pos.z*f3 + f4);

	f1 = (factor1*m2[0]._12) + (factor2*m2[1]._12);
	f2 = (factor1*m2[0]._22) + (factor2*m2[1]._22);
	f3 = (factor1*m2[0]._32) + (factor2*m2[1]._32);
	f4 = (factor1*m2[0]._42) + (factor2*m2[1]._42);
	v_out.y = (v_in.pos.x*f1 + v_in.pos.y*f2 + v_in.pos.z*f3 + f4);

	f1 = (factor1*m2[0]._13) + (factor2*m2[1]._13);
	f2 = (factor1*m2[0]._23) + (factor2*m2[1]._23);
	f3 = (factor1*m2[0]._33) + (factor2*m2[1]._33);
	f4 = (factor1*m2[0]._43) + (factor2*m2[1]._43);
	v_out.z = (v_in.pos.x*f1 + v_in.pos.y*f2 + v_in.pos.z*f3 + f4);
}


long CZ3DMultipartSkin::GetPolyCount( int nLODLevel ) const
{
	if( this != m_rpThis )
	{
		return 0;
	}

	if( -1 == nLODLevel )
	{
		nLODLevel = m_lLODLevel;
	}

	if( nLODLevel >= Z3D_LOD_LEVEL )
	{
		return 0;
	}

	long lSum = 0;

	for( int i = 0; i < Z3D_MULTIPART_PORTION_COUNT; ++i )
	{
		lSum += m_apPortion[i]->m_alIndexCount[nLODLevel];
	}

	lSum /= 3;

	return lSum;
}
