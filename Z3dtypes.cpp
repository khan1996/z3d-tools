
#include "Z3DTYPES.h"

#include <stdio.h>
#include "GMMemory.h"



bool Z3DLODMesh_::Load( const char* szFilename )
{
	FILE* fp; 
	if((fp = fopen(szFilename, "rb")) == NULL )
	{
		return false;
	}
	
	fread( &wId, sizeof(WORD), 1, fp );
	fread( &lVertexCount, sizeof(long), 1, fp );
	pVertices = new Z3DBlend2Vertex_[lVertexCount];
	if( NULL == pVertices )
	{
		fclose( fp );
		return false;
	}
	
	for(int i = 0; i <  Z3D_LOD_LEVEL; i++ )
	{
		fread( &(aIndex[i].lIndexCount), sizeof(long), 1, fp );
		if( aIndex[i].lIndexCount )
		{
			aIndex[i].pIndices = new WORD[aIndex[i].lIndexCount];
			if( NULL == aIndex[i].pIndices )
			{
				fclose( fp );
				return false;
			}
		}
	}
	
	fread( pVertices, sizeof(Z3DBlend2Vertex_), lVertexCount, fp );
	
	for(int i = 0; i <  Z3D_LOD_LEVEL; i++ )
	{
		if( aIndex[i].lIndexCount )
		{
			fread( aIndex[i].pIndices, sizeof(WORD), aIndex[i].lIndexCount, fp );
		}
	}
	
	fclose( fp );
	return true;
}


//bool Z3DLODMesh::Load( const char* szFilename )
bool Z3DLODMesh::Load(Caldron::Base::CCaldronByteDataObj *pDataObj)
{
	if( 0 == pDataObj->GetByteSize() )
	{
		return false;
	}
	
	pDataObj->Read( &wId, sizeof(WORD), 1 );
	pDataObj->Read( &lVertexCount, sizeof(long), 1 );

	pVertices = new Z3DBlend2Vertex[lVertexCount];
	if( NULL == pVertices )
	{
		return false;
	}
	
	for(int i = 0; i <  Z3D_LOD_LEVEL; i++ )
	{
		pDataObj->Read( &(aIndex[i].lIndexCount), sizeof(long), 1 );

		if( aIndex[i].lIndexCount )
		{
			aIndex[i].pIndices = new WORD[aIndex[i].lIndexCount];
			if( NULL == aIndex[i].pIndices )
			{
				return false;
			}
		}
	}
	
	pDataObj->Read( pVertices, sizeof(Z3DBlend2Vertex), lVertexCount );
	
	for(int i = 0; i <  Z3D_LOD_LEVEL; i++ )
	{
		if( aIndex[i].lIndexCount )
		{
			pDataObj->Read( aIndex[i].pIndices, sizeof(WORD), aIndex[i].lIndexCount );
		}
	}
	
	m_bLoaded = true;
	return true;
}


bool Z3DLODMesh::Save( const char* szFilename )
{
	FILE* fp; 
	if((fp = fopen(szFilename, "wb")) == NULL )
	{
		return false;
	}
	
	// writing id
	fwrite( &wId, sizeof(WORD), 1, fp );
	// writing vertex count
	fwrite( &lVertexCount, sizeof(long), 1, fp );
	if( NULL == pVertices )
	{
		fclose( fp );
		return false;
	}
	
	// writing index count of each LOD level
	for(int i = 0; i <  Z3D_LOD_LEVEL; i++ )
	{
		fwrite( &(aIndex[i].lIndexCount), sizeof(long), 1, fp );
		if( aIndex[i].lIndexCount )
		{
			if( NULL == aIndex[i].pIndices )
			{
				fclose( fp );
				return false;
			}
		}
	}
	
	// writing vertex content
	fwrite( pVertices, sizeof(Z3DBlend2Vertex), lVertexCount, fp );
	
	// writing index content of each LOD level
	for(int i = 0; i <  Z3D_LOD_LEVEL; i++ )
	{
		if( aIndex[i].lIndexCount )
		{
			fwrite( aIndex[i].pIndices, sizeof(WORD), aIndex[i].lIndexCount, fp );
		}
	}
	
	fclose( fp );
	return true;
}




bool Z3DAMesh::Load( const char* szFileName )
{
	FILE* fp; 
	if((fp = fopen(szFileName, "rb")) == NULL )
	{
		return false;
	}
	
	fread( &m_wVertexCount, sizeof(WORD), 1, fp );
	fread( &m_wIndexCount, sizeof(WORD), 1, fp );
	
	m_pVertices = new BumpVertex[m_wVertexCount];
	if( NULL == m_pVertices )
	{
		fclose( fp );
		return false;
	}
	
	m_pIndices = new WORD[m_wIndexCount];
	if( NULL == m_pIndices )
	{
		fclose( fp );
		return false;
	}
	
	fread( m_pVertices, sizeof(BumpVertex), m_wVertexCount, fp );
	fread( m_pIndices, sizeof(WORD), m_wIndexCount, fp );
	
	fread( &m_wMarker1, sizeof(WORD), 1, fp );
	fread( &m_wMarker2, sizeof(WORD), 1, fp );
	
	fclose(fp);
	
	return true;		
}

bool Z3DAMesh::Save( const char* szFileName )
{
	FILE* fp; 
	if((fp = fopen(szFileName, "wb")) == NULL )
	{
		return false;
	}
	
	fwrite( &m_wVertexCount, sizeof(WORD), 1, fp );
	fwrite( &m_wIndexCount, sizeof(WORD), 1, fp );
	
	if( NULL == m_pVertices )
	{
		fclose( fp );
		return false;
	}
	
	if( NULL == m_pIndices )
	{
		fclose( fp );
		return false;
	}
	
	fwrite( m_pVertices, sizeof(BumpVertex), m_wVertexCount, fp );
	fwrite( m_pIndices, sizeof(WORD), m_wIndexCount, fp );
	
	fwrite( &m_wMarker1, sizeof(WORD), 1, fp );
	fwrite( &m_wMarker2, sizeof(WORD), 1, fp );
	
	fclose(fp);
	
	return true;
}
