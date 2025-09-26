#include ".\clothmgr.h"
#include "SceneManager.h"
#include "dxutil.h"
#include "GMMemory.h"

CClothMgr* CClothMgr::s_pInstance = NULL; 

CQuadClothResource::CQuadClothResource() : m_pVB(NULL), 
m_pIB(NULL), m_pClothVertex(NULL), m_pClothIndex(NULL)
{

}

CQuadClothResource::~CQuadClothResource()
{
	SAFE_RELEASE( m_pVB ); 
	SAFE_RELEASE( m_pIB ); 
	SAFE_DELETE_ARRAY( m_pClothVertex ); 
	SAFE_DELETE_ARRAY( m_pClothIndex ); 
}

void CQuadClothResource::Init( int iRow, int iCol )
{
	m_iCol = iCol; 
	m_iRow = iRow; 

	LPDIRECT3DDEVICE8 pd3dDevice = CSceneManager::GetDevice(); 

	m_iNumVertex = (iRow+1) * (iCol+1); 
	m_iNumFace = iRow * iCol * 2; 

	m_pClothVertex = new ClothVertex[m_iNumVertex]; 
	m_pClothIndex = new WORD[m_iNumFace*3]; 

	int iVertex = 0; 
	int iIndex = 0; 
	int r, c; 
	for( r = 0; r <= iRow; r++ )
	{
		for( c = 0; c <= iCol; c++ )
		{
			m_pClothVertex[iVertex].pos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ); 
			m_pClothVertex[iVertex].n = D3DXVECTOR3( rand()%100, rand()%100, -rand()%100 );

			if( c == 0 ) m_pClothVertex[iVertex].u = 0.0f;
			else m_pClothVertex[iVertex].u = (float)c/(float)iCol; 
			if( r == 0 ) m_pClothVertex[iVertex].v = 0.0f;
			else m_pClothVertex[iVertex].v = (float)r/(float)iRow;  

			iVertex++; 
		}
	}

	for( r = 0; r < iRow; r++ )
	{
		for( c = 0; c < iCol; c++ )
		{
			m_pClothIndex[iIndex++] = c + r*(iCol+1); 
			m_pClothIndex[iIndex++] = c + r*(iCol+1) + 1; 
			m_pClothIndex[iIndex++] = c + (r+1)*(iCol+1);  
			m_pClothIndex[iIndex++] = c + r*(iCol+1) + 1; 
			m_pClothIndex[iIndex++] = c + (r+1)*(iCol+1) + 1; 
			m_pClothIndex[iIndex++] = c + (r+1)*(iCol+1); 
		}
	}

	pd3dDevice->CreateVertexBuffer( sizeof(ClothVertex)*m_iNumVertex, D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, ClothVertex::FVF, 
		D3DPOOL_DEFAULT, &m_pVB ); 
	pd3dDevice->CreateIndexBuffer( sizeof(WORD)*m_iNumFace*3, D3DUSAGE_WRITEONLY, 
		D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB ); 

	void* pVert;  
	m_pVB->Lock( 0, 0, (BYTE**)&pVert, 0 ); 
	memcpy( pVert, m_pClothVertex, sizeof(ClothVertex)*m_iNumVertex ); 
	m_pVB->Unlock(); 

	void* pIndics; 
	m_pIB->Lock( 0, 0, (BYTE**)&pIndics, 0 ); 
	memcpy( pIndics, m_pClothIndex, sizeof(WORD)*iRow*iCol*2*3 ); 
	m_pIB->Unlock(); 
}

void CQuadClothResource::Render()
{
	D3DXMATRIX matWorld; 
	LPDIRECT3DDEVICE8 pd3dDevice = CSceneManager::GetDevice(); 
	D3DXMatrixIdentity( &matWorld );
	pd3dDevice->SetVertexShader( ClothVertex::FVF ); 
	pd3dDevice->SetStreamSource( 0, m_pVB, sizeof(ClothVertex) ); 
	pd3dDevice->SetIndices( m_pIB, 0 ); 
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );	
	pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, m_iNumVertex, 0, m_iNumFace ); 
}

void CQuadClothResource::UpdateLockedVertex()
{
	void* pVert;  
	m_pVB->Lock( 0, 0, (BYTE**)&pVert, D3DLOCK_DISCARD|D3DLOCK_NOSYSLOCK ); 
	memcpy( pVert, m_pClothVertex, sizeof(ClothVertex)*m_iNumVertex ); 
	m_pVB->Unlock(); 
}

void CQuadClothResource::ComputeNormals()
{
	D3DXVECTOR3 a, b, N; 
	int ComputeNum; 

	for( int r = 0; r <= m_iRow; r++ )
	{
		for( int c = 0; c <= m_iCol; c++ )
		{
			ComputeNum = 0; 
			m_pClothVertex[ROWCOL(r,c)].n = D3DXVECTOR3(0,0,-1); 

			if( r > 0 && c > 0 && r < m_iRow && c < m_iCol )
			{
				CalcNormal(r, c, r, c-1, r-1, c);
				ComputeNum++; 

				CalcNormal(r, c, r-1, c, r, c+1);
				ComputeNum++; 

				CalcNormal(r, c, r, c+1, r+1, c);
				ComputeNum++; 

				CalcNormal(r, c, r+1, c, r, c-1);
				ComputeNum++; 
			}
			else 
			{
				//꼭지점
				if( c == 0 && r == 0 )
				{
					CalcNormal(r, c, r, c+1, r+1, c);
					ComputeNum++; 
				}
				if( c == m_iCol && r == 0 )
				{
					CalcNormal(r, c, r+1, c, r, c-1);
					ComputeNum++; 
				}
				if( c == 0 && r == m_iRow )
				{
					CalcNormal(r, c, r-1, c, r, c+1);
					ComputeNum++; 
				}
				if( c == m_iCol && r == m_iRow )
				{
					CalcNormal(r, c, r, c-1, r-1, c);
					ComputeNum++; 
				}
					
				//--------------------------------
				//양옆모서리
				if( c == 0 && r > 0 && r < m_iRow )
				{
					CalcNormal(r, c, r-1, c, r, c+1);
					ComputeNum++; 
					CalcNormal(r, c, r, c+1, r+1, c);
					ComputeNum++; 
				}
				if( c == m_iCol && r > 0 && r < m_iRow )
				{
					CalcNormal(r, c, r, c-1, r-1, c);
					ComputeNum++; 
					CalcNormal(r, c, r+1, c, r, c-1);
					ComputeNum++; 
				}
				//-------------------------------
				//위바닥모서리
				if( r == 0 && c > 0 && c < m_iCol )
				{
					CalcNormal(r, c, r+1, c, r, c-1);
					ComputeNum++; 
					CalcNormal(r, c, r, c+1, r+1, c);
					ComputeNum++; 
				}
				if( r == m_iRow && c > 0 && c < m_iCol )
				{
					CalcNormal(r, c, r, c-1, r-1, c);
					ComputeNum++; 
					CalcNormal(r, c, r-1, c, r, c+1);
					ComputeNum++; 
				}
			}
		
			m_pClothVertex[ROWCOL(r,c)].n /= (float)ComputeNum; 
			D3DXVec3Normalize( &m_pClothVertex[ROWCOL(r,c)].n, &m_pClothVertex[ROWCOL(r,c)].n ); 
			m_pClothVertex[ROWCOL(r,c)].n = -m_pClothVertex[ROWCOL(r,c)].n; 
		}
	}
}

void CQuadClothResource::CalcNormal( int r, int c, int ar, int ac, int br, int bc )
{
	D3DXVECTOR3 a, b, N; 
	a = m_pClothVertex[ROWCOL(ar,ac)].pos - m_pClothVertex[ROWCOL(r,c)].pos; 
	b = m_pClothVertex[ROWCOL(br,bc)].pos - m_pClothVertex[ROWCOL(r,c)].pos; 
	D3DXVec3Cross( &N, &a, &b ); 
	D3DXVec3Normalize( &N, &N ); 
	m_pClothVertex[ROWCOL(r,c)].n += N; 
}

void CQuadClothResource::SetupCloth( CCharacterCape* pCape )
{
	int iVertexIndex = 0; 
	for( int r=0; r <= pCape->m_Prop.m_iRow; r++)
	{
		for(int c=0; c <= pCape->m_Prop.m_iCol; c++)
		{	
			SetVertex(iVertexIndex, pCape->m_pParticles[iVertexIndex].vPos); 
			iVertexIndex ++; 
		}
	}
	
	ComputeNormals(); 
	UpdateLockedVertex(); 
}

/***************************************************************/

CClothMgr::CClothMgr(void)
{
	m_CapeRenderer.Init( 5, 4 ); 
}

CClothMgr::~CClothMgr(void)
{

}

CClothMgr* CClothMgr::_Instance()
{
	if( s_pInstance == NULL )
	{
		s_pInstance = new CClothMgr; 
	}

	return s_pInstance; 
}

void CClothMgr::_Destroy()
{
	if( s_pInstance )
	{
		s_pInstance->ReleaseAllData(); 
		delete s_pInstance; 
	}
}

void CClothMgr::Update()
{
	CLOTHMAP::iterator iter; 
	
	for( iter = m_ClothMap.begin(); iter != m_ClothMap.end(); iter++ )
	{
		iter->second->Update(); 
	}
}

void CClothMgr::AddCharacterCape( CZ3DGeneralChrModel* pChrModel )
{	
	CLOTHMAP::iterator iter; 
	iter = m_ClothMap.find( pChrModel ); 
	if( iter == m_ClothMap.end() )
	{
		vector3 point1, point2, length; 
		pChrModel->GetFixedPoint( 0, point1 ); 
		pChrModel->GetFixedPoint( 1, point2 ); 
		length = point1- point2; 

		CClothProperty Prop; 
		Prop.Init( length.GetLens(), 100, 5, 4, CFT_TRAPEZOID ); 
		CCharacterCape* pCloth = new CCharacterCape( &Prop ); 
		pCloth->Create( CCharacterCape::HUMAN_MAN, pChrModel );
		m_ClothMap.insert( CLOTHMAP::iterator::value_type(pChrModel, pCloth) ); 
	}
}

void CClothMgr::DeleteCharacterCape( CZ3DGeneralChrModel* pChrModel )
{
	CLOTHMAP::iterator iter; 
	iter = m_ClothMap.find( pChrModel ); 
	if( iter != m_ClothMap.end() ) 
	{
		SAFE_DELETE( iter->second ); 
		m_ClothMap.erase( iter ); 
	}
}

void CClothMgr::ReleaseAllData()
{
	if( m_ClothMap.size() != 0 ) 
	{
		CLOTHMAP::iterator iter; 
		for( iter = m_ClothMap.begin(); iter != m_ClothMap.end(); iter++ )
		{
			SAFE_DELETE(iter->second); 
		}
	}
}

CCharacterCape* CClothMgr::GetCharacterCape( CZ3DGeneralChrModel* pModel )
{
	CCharacterCape* pCape=NULL; 
	CLOTHMAP::iterator iter; 
	iter = m_ClothMap.find( pModel ); 
	if( iter != m_ClothMap.end() )
	{
		pCape = iter->second; 
	}
	
	return pCape; 
}

void CClothMgr::Reset()
{
	for(CLOTHMAP::iterator iter = m_ClothMap.begin();
        iter != m_ClothMap.end(); iter++ )
	{
		iter->second->ResetParticle(); 
	}
}