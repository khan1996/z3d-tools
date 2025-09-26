// Z3DTYPES.h: interface for the Z3DTYPES class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Z3DTYPES_H__0B082281_CC66_11D4_AD2B_0000E8EB4C69__INCLUDED_)
#define AFX_Z3DTYPES_H__0B082281_CC66_11D4_AD2B_0000E8EB4C69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "vector.h"
#include "misc.h"
#include "vertex.h"

#include <Src/Base/LoadedObj.h>
#include <Src/Base/CaldronByteDataObj.h>


#pragma pack(push, Z3D_ALIGN4)
#pragma pack(4)


#define Z3D_LOD_LEVEL 4


struct Z3DBlend2Vertex_
{
	vector3	pos;
	float	bfactor;
	BYTE	mtrx_id[2];
	vector3 normal;
	float	u;
	float	v;
	BYTE	lodLevel;
};


struct Z3DBlend2Vertex
{
	vector3	pos;
	float	bfactor;
	BYTE	mtrx_id[2];
	vector3 normal;
	vector3 u;
	float	tu;
	float	tv;
	BYTE	lodLevel;
};


struct Z3DMeshIndex
{
	WORD* pIndices;
	long lIndexCount;

	Z3DMeshIndex()
	{
		pIndices = NULL;
		lIndexCount = 0;
	}

	~Z3DMeshIndex()
	{
		SAFE_DELETEA( pIndices );
	}
};


struct Z3DLODMesh_
{
	WORD wId;
	Z3DBlend2Vertex_* pVertices;
	long lVertexCount;

	Z3DMeshIndex aIndex[Z3D_LOD_LEVEL];

	Z3DLODMesh_()
	{
		wId = 999;
		pVertices = NULL;
		lVertexCount = 0;
	}

	~Z3DLODMesh_()
	{
		SAFE_DELETEA( pVertices );
	}

	bool Load( const char* szFilename );

	int GetId()
	{
		return wId;
	}
};


struct Z3DLODMesh : public Caldron::Base::CLoadedObj
{
	WORD wId;
	Z3DBlend2Vertex* pVertices;
	long lVertexCount;

	Z3DMeshIndex aIndex[Z3D_LOD_LEVEL];

	Z3DLODMesh()
	{
		wId = 999;
		pVertices = NULL;
		lVertexCount = 0;
	}

	~Z3DLODMesh()
	{
		SAFE_DELETEA( pVertices );
	}

	//	bool Load( const char* szFilename );
	virtual bool Load(Caldron::Base::CCaldronByteDataObj *pDataObj);
	virtual bool PostLoad(){ 
		return true;}

	bool Save( const char* szFilename );

	/*	
	bool Unload()
	{
	return true;
	}
	*/

	int GetId()
	{
		return wId;
	}
};


struct Z3DAMesh
{
	WORD m_wVertexCount;
	BumpVertex* m_pVertices;

	WORD m_wIndexCount;
	WORD* m_pIndices;

	WORD m_wMarker1;
	WORD m_wMarker2;

	Z3DAMesh()
	{
		m_wVertexCount = 0;
		m_pVertices = NULL;

		m_wIndexCount = 0;
		m_pIndices = NULL;

		m_wMarker1 = 0;
		m_wMarker2 = 0;
	}

	~Z3DAMesh()
	{
		SAFE_DELETEA(m_pVertices);
		SAFE_DELETEA(m_pIndices);
	}

	bool Load( const char* szFileName );
	bool Save( const char* szFileName );
};

/*
struct Z3DMesh
{
Z3DBlend2Vertex* pVertices;
long lVertexCount;

Z3DMeshIndex index;
};
*/


#pragma pack(pop, Z3D_ALIGN4)

#endif // !defined(AFX_Z3DTYPES_H__0B082281_CC66_11D4_AD2B_0000E8EB4C69__INCLUDED_)
