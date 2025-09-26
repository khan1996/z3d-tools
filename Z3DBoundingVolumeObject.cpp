// Z3DBoundingVolumeObject.cpp: implementation of the CZ3DBoundingVolumeObject class.
//
//////////////////////////////////////////////////////////////////////

#include "Z3DBoundingVolumeObject.h"
#include "GMMemory.h"



bool CZ3DOBB::ms_bRender = false;
D3DSLVERTEX CZ3DOBB::ms_pOutlineVertex[8] =
{
	D3DSLVERTEX( 0, 0, 0, D3DCOLOR_ARGB(0xff,0xff,0xff,0xff) ),
	D3DSLVERTEX( 1, 0, 0, D3DCOLOR_ARGB(0xff,0xff,0xff,0xff) ),
	D3DSLVERTEX( 1, 0, 1, D3DCOLOR_ARGB(0xff,0xff,0xff,0xff) ),
	D3DSLVERTEX( 0, 0, 1, D3DCOLOR_ARGB(0xff,0xff,0xff,0xff) ),
	D3DSLVERTEX( 0, 1, 0, D3DCOLOR_ARGB(0xff,0xff,0xff,0xff) ),
	D3DSLVERTEX( 1, 1, 0, D3DCOLOR_ARGB(0xff,0xff,0xff,0xff) ),
	D3DSLVERTEX( 1, 1, 1, D3DCOLOR_ARGB(0xff,0xff,0xff,0xff) ),
	D3DSLVERTEX( 0, 1, 1, D3DCOLOR_ARGB(0xff,0xff,0xff,0xff) )
};

WORD CZ3DOBB::ms_pOutlineIndex[24] =
{
	0, 1, 1, 2, 2, 3, 3, 0,
		4, 5, 5, 6, 6, 7, 7, 4,
		0, 4, 1, 5, 2, 6, 3, 7
};

//////////////////////////////////////////////////////////////////////////



bool CZ3DBoundingVolumeObject::ms_bRender = false;
D3DSLVERTEX CZ3DBoundingVolumeObject::ms_pOutlineVertex[8] =
	{
		D3DSLVERTEX( 0, 0, 0, D3DCOLOR_ARGB(0xff,0xff,0xff,0xff) ),
		D3DSLVERTEX( 1, 0, 0, D3DCOLOR_ARGB(0xff,0xff,0xff,0xff) ),
		D3DSLVERTEX( 1, 0, 1, D3DCOLOR_ARGB(0xff,0xff,0xff,0xff) ),
		D3DSLVERTEX( 0, 0, 1, D3DCOLOR_ARGB(0xff,0xff,0xff,0xff) ),
		D3DSLVERTEX( 0, 1, 0, D3DCOLOR_ARGB(0xff,0xff,0xff,0xff) ),
		D3DSLVERTEX( 1, 1, 0, D3DCOLOR_ARGB(0xff,0xff,0xff,0xff) ),
		D3DSLVERTEX( 1, 1, 1, D3DCOLOR_ARGB(0xff,0xff,0xff,0xff) ),
		D3DSLVERTEX( 0, 1, 1, D3DCOLOR_ARGB(0xff,0xff,0xff,0xff) )
	};

WORD CZ3DBoundingVolumeObject::ms_pOutlineIndex[24] =
	{
		0, 1, 1, 2, 2, 3, 3, 0,
		4, 5, 5, 6, 6, 7, 7, 4,
		0, 4, 1, 5, 2, 6, 3, 7
	};




void CZ3DOBB::SetStaticParameter( vector3& vExtent )
{
	m_vExtent = vExtent;
}


void CZ3DOBB::SetDynamicParameter( vector3& vCenter, matrix& rDir )
{
	m_aAxis[0].x = rDir._11; m_aAxis[0].y = rDir._12; m_aAxis[0].z = rDir._13;
	m_aAxis[1].x = rDir._21; m_aAxis[1].y = rDir._22; m_aAxis[1].z = rDir._23;
	m_aAxis[2].x = rDir._31; m_aAxis[2].y = rDir._32; m_aAxis[2].z = rDir._33;

	m_Center = vCenter;
}


bool CZ3DOBB::CheckCollision( const CZ3DOBB& rOBB ) const
{
	// cutoff val. to check if two axes are parallel
    const float fCutoff = (float)0.999999f;
    bool bExistsParallelPair = false;
    int i;


	vector3 kD = rOBB.m_Center - m_Center;

	float aafC[3][3];
	float aafAbsC[3][3];
	float afAD[3];
	float fR0, fR1, fR, fR01;

	// axis Box1 x
	for( i = 0; i < 3; ++i )
	{
		aafC[0][i] = m_aAxis[0] * rOBB.m_aAxis[i];
		aafAbsC[0][i] = fabs( aafC[0][i] );
		if( aafAbsC[0][i] > fCutoff )
		{
			bExistsParallelPair = true;
		}
	}
	afAD[0] = m_aAxis[0] * kD;
	fR = fabs( afAD[0] );
	fR1 = rOBB.m_vExtent.x*aafAbsC[0][0] + rOBB.m_vExtent.y*aafAbsC[0][1] + rOBB.m_vExtent.z*aafAbsC[0][2];
	fR01 = m_vExtent.x + fR1;
	if( fR > fR01 )
	{
		return false;
	}

	// axis Box1 y
	for( i = 0; i < 3; ++i )
	{
		aafC[1][i] = m_aAxis[1] * rOBB.m_aAxis[i];
		aafAbsC[1][i] = fabs( aafC[1][i] );
		if( aafAbsC[1][i] > fCutoff )
		{
			bExistsParallelPair = true;
		}
	}
	afAD[1] = m_aAxis[1] * kD;
	fR = fabs( afAD[1] );
	fR1 = rOBB.m_vExtent.x*aafAbsC[1][0] + rOBB.m_vExtent.y*aafAbsC[1][1] + rOBB.m_vExtent.z*aafAbsC[1][2];
	fR01 = m_vExtent.y + fR1;
	if( fR > fR01 )
	{
		return false;
	}
	
	// axis Box1 z
	for( i = 0; i < 3; ++i )
	{
		aafC[2][i] = m_aAxis[2] * rOBB.m_aAxis[i];
		aafAbsC[2][i] = fabs( aafC[2][i] );
		if( aafAbsC[2][i] > fCutoff )
		{
			bExistsParallelPair = true;
		}
	}
	afAD[2] = m_aAxis[2] * kD;
	fR = fabs( afAD[2] );
	fR1 = rOBB.m_vExtent.x*aafAbsC[2][0] + rOBB.m_vExtent.y*aafAbsC[2][1] + rOBB.m_vExtent.z*aafAbsC[2][2];
	fR01 = m_vExtent.z + fR1;
	if( fR > fR01 )
	{
		return false;
	}

	// axis Box2 x
	fR = fabs( rOBB.m_aAxis[0] * kD );
	fR0 = m_vExtent.x*aafAbsC[0][0] + m_vExtent.y*aafAbsC[1][0] + m_vExtent.z*aafAbsC[2][0];
	fR01 = fR0 + rOBB.m_vExtent.x;
	if( fR > fR01 )
	{
		return false;
	}

	// axis Box2 y
	fR = fabs( rOBB.m_aAxis[1] * kD );
	fR0 = m_vExtent.x*aafAbsC[0][1] + m_vExtent.y*aafAbsC[1][1] + m_vExtent.z*aafAbsC[2][1];
	fR01 = fR0 + rOBB.m_vExtent.y;
	if( fR > fR01 )
	{
		return false;
	}
	
	// axis Box2 z
	fR = fabs( rOBB.m_aAxis[2] * kD );
	fR0 = m_vExtent.x*aafAbsC[0][2] + m_vExtent.y*aafAbsC[1][2] + m_vExtent.z*aafAbsC[2][2];
	fR01 = fR0 + rOBB.m_vExtent.z;
	if( fR > fR01 )
	{
		return false;
	}


	//
	if( bExistsParallelPair )
	{
		return true;
	}
	

	// axis Box1 x ^ Box2 x
	fR = fabs( afAD[2]*aafC[1][0] - afAD[1]*aafC[2][0] );
	fR0 = m_vExtent.y*aafAbsC[2][0] + m_vExtent.z*aafAbsC[1][0];
	fR1 = rOBB.m_vExtent.y*aafAbsC[0][2] + rOBB.m_vExtent.z*aafAbsC[1][0];
	fR01 = fR0 + fR1;
	if( fR > fR01 )
	{
		return false;
	}
	
	// axis Box1 x ^ Box2 y
	fR = fabs( afAD[2]*aafC[1][1] - afAD[1]*aafC[2][1] );
	fR0 = m_vExtent.y*aafAbsC[2][1] + m_vExtent.z*aafAbsC[1][1];
	fR1 = rOBB.m_vExtent.x*aafAbsC[0][2] + rOBB.m_vExtent.z*aafAbsC[0][0];
	fR01 = fR0 + fR1;
	if( fR > fR01 )
	{
		return false;
	}
	
	// axis Box1 x ^ Box2 z
	fR = fabs( afAD[2]*aafC[1][2] - afAD[1]*aafC[2][2] );
	fR0 = m_vExtent.y*aafAbsC[2][2] + m_vExtent.z*aafAbsC[1][2];
	fR1 = rOBB.m_vExtent.x*aafAbsC[0][1] + rOBB.m_vExtent.y*aafAbsC[0][0];
	fR01 = fR0 + fR1;
	if( fR > fR01 )
	{
		return false;
	}
	
	// axis Box1 y ^ Box2 x
	fR = fabs( afAD[0]*aafC[2][0] - afAD[2]*aafC[0][0] );
	fR0 = m_vExtent.x*aafAbsC[2][0] + m_vExtent.z*aafAbsC[0][0];
	fR1 = rOBB.m_vExtent.y*aafAbsC[1][2] + rOBB.m_vExtent.z*aafAbsC[1][1];
	fR01 = fR0 + fR1;
	if( fR > fR01 )
	{
		return false;
	}
	
	// axis Box1 y ^ Box2 y
	fR = fabs( afAD[0]*aafC[2][1] - afAD[2]*aafC[0][1] );
	fR0 = m_vExtent.x*aafAbsC[2][1] + m_vExtent.z*aafAbsC[0][1];
	fR1 = rOBB.m_vExtent.x*aafAbsC[1][2] + rOBB.m_vExtent.z*aafAbsC[1][0];
	fR01 = fR0 + fR1;
	if( fR > fR01 )
	{
		return false;
	}
	
	// axis Box1 y ^ Box2 z
	fR = fabs( afAD[0]*aafC[2][2] - afAD[2]*aafC[0][2] );
	fR0 = m_vExtent.x*aafAbsC[2][2] + m_vExtent.z*aafAbsC[0][2];
	fR1 = rOBB.m_vExtent.x*aafAbsC[1][1] + rOBB.m_vExtent.y*aafAbsC[1][0];
	fR01 = fR0 + fR1;
	if( fR > fR01 )
	{
		return false;
	}
	
	// axis Box1 z ^ Box2 x
	fR = fabs( afAD[1]*aafC[0][0] - afAD[0]*aafC[1][0] );
	fR0 = m_vExtent.x*aafAbsC[1][0] + m_vExtent.y*aafAbsC[0][0];
	fR1 = rOBB.m_vExtent.y*aafAbsC[2][2] + rOBB.m_vExtent.z*aafAbsC[2][1];
	fR01 = fR0 + fR1;
	if( fR > fR01 )
	{
		return false;
	}
	
	// axis Box1 z ^ Box2 y
	fR = fabs( afAD[1]*aafC[0][1] - afAD[0]*aafC[1][1] );
	fR0 = m_vExtent.x*aafAbsC[1][1] + m_vExtent.y*aafAbsC[0][1];
	fR1 = rOBB.m_vExtent.x*aafAbsC[2][2] + rOBB.m_vExtent.z*aafAbsC[2][0];
	fR01 = fR0 + fR1;
	if( fR > fR01 )
	{
		return false;
	}
	
	// axis Box1 z ^ Box2 z
	fR = fabs( afAD[1]*aafC[0][2] - afAD[0]*aafC[1][2] );
	fR0 = m_vExtent.x*aafAbsC[1][2] + m_vExtent.y*aafAbsC[0][2];
	fR1 = rOBB.m_vExtent.x*aafAbsC[2][1] + rOBB.m_vExtent.y*aafAbsC[2][0];
	fR01 = fR0 + fR1;
	if( fR > fR01 )
	{
		return false;
	}
	
	
	return true;
}



void CZ3DOBB::Render()
{
	if( !ms_bRender )
	{
		return;
	}
	
	vector3 vTmp, vResult;
	
	vResult = -m_vExtent.x*m_aAxis[0] - m_vExtent.y*m_aAxis[1] - m_vExtent.z*m_aAxis[2] + m_Center;
	ms_pOutlineVertex[0].x = vResult.x; ms_pOutlineVertex[0].y = vResult.y; ms_pOutlineVertex[0].z = vResult.z;
	
	vResult = m_vExtent.x*m_aAxis[0] - m_vExtent.y*m_aAxis[1] - m_vExtent.z*m_aAxis[2] + m_Center;
	ms_pOutlineVertex[1].x = vResult.x; ms_pOutlineVertex[1].y = vResult.y; ms_pOutlineVertex[1].z = vResult.z;
	
	vResult = m_vExtent.x*m_aAxis[0] - m_vExtent.y*m_aAxis[1] + m_vExtent.z*m_aAxis[2] + m_Center;
	ms_pOutlineVertex[2].x = vResult.x; ms_pOutlineVertex[2].y = vResult.y; ms_pOutlineVertex[2].z = vResult.z;
	
	vResult = -m_vExtent.x*m_aAxis[0] - m_vExtent.y*m_aAxis[1] + m_vExtent.z*m_aAxis[2] + m_Center;
	ms_pOutlineVertex[3].x = vResult.x; ms_pOutlineVertex[3].y = vResult.y; ms_pOutlineVertex[3].z = vResult.z;
	
	vResult = -m_vExtent.x*m_aAxis[0] + m_vExtent.y*m_aAxis[1] - m_vExtent.z*m_aAxis[2] + m_Center;
	ms_pOutlineVertex[4].x = vResult.x; ms_pOutlineVertex[4].y = vResult.y; ms_pOutlineVertex[4].z = vResult.z;
	
	vResult = m_vExtent.x*m_aAxis[0] + m_vExtent.y*m_aAxis[1] - m_vExtent.z*m_aAxis[2] + m_Center;
	ms_pOutlineVertex[5].x = vResult.x; ms_pOutlineVertex[5].y = vResult.y; ms_pOutlineVertex[5].z = vResult.z;
	
	vResult = m_vExtent.x*m_aAxis[0] + m_vExtent.y*m_aAxis[1] + m_vExtent.z*m_aAxis[2] + m_Center;
	ms_pOutlineVertex[6].x = vResult.x; ms_pOutlineVertex[6].y = vResult.y; ms_pOutlineVertex[6].z = vResult.z;
	
	vResult = -m_vExtent.x*m_aAxis[0] + m_vExtent.y*m_aAxis[1] + m_vExtent.z*m_aAxis[2] + m_Center;
	ms_pOutlineVertex[7].x = vResult.x; ms_pOutlineVertex[7].y = vResult.y; ms_pOutlineVertex[7].z = vResult.z;
	
	D3DMATRIX* pMat = GetIdentityD3DMatrix();
	
	GetDevice()->SetTransform( D3DTS_WORLD, pMat );
	GetDevice()->SetVertexShader( D3DFVF_SLVERTEX );
	GetDevice()->DrawIndexedPrimitiveUP( D3DPT_LINELIST, 0, 8, 12, ms_pOutlineIndex, D3DFMT_INDEX16, ms_pOutlineVertex, sizeof(D3DSLVERTEX) );
}







//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CZ3DBoundingVolumeObject::CZ3DBoundingVolumeObject()
	: CZ3DObject(), CZ3DRenderable(false)
{
	m_rpChrModel = NULL;

	m_vExtent.Identity();

	m_fScaleFactor = 1.0f;
}

CZ3DBoundingVolumeObject::~CZ3DBoundingVolumeObject()
{

}


void CZ3DBoundingVolumeObject::UpdateFrame()
{
	CZ3DObject::UpdateFrame();

	if( m_rpChrModel )
	{
		m_Pos = m_fScaleFactor*m_Pos + m_rpChrModel->GetPosition();
	}

	vector3 v = m_fScaleFactor * m_vExtent;
	m_OBB.SetStaticParameter( v );
	m_OBB.SetDynamicParameter( m_Pos, m_TM );
}


void CZ3DBoundingVolumeObject::Render()
{
	if( !ms_bRender )
	{
		return;
	}

	vector3 vTmp, vResult;

	vTmp = vector3( -m_OBB.m_vExtent.x, -m_OBB.m_vExtent.y, -m_OBB.m_vExtent.z );
	z3d::VectorRotate( vResult, vTmp, m_Dir );
	vResult +=  m_OBB.m_Center;
	ms_pOutlineVertex[0].x = vResult.x; ms_pOutlineVertex[0].y = vResult.y; ms_pOutlineVertex[0].z = vResult.z;
	
	vTmp = vector3( m_OBB.m_vExtent.x, -m_OBB.m_vExtent.y, -m_OBB.m_vExtent.z );
	z3d::VectorRotate( vResult, vTmp, m_Dir );
	vResult +=  m_OBB.m_Center;
	ms_pOutlineVertex[1].x = vResult.x; ms_pOutlineVertex[1].y = vResult.y; ms_pOutlineVertex[1].z = vResult.z;
	
	vTmp = vector3( m_OBB.m_vExtent.x, -m_OBB.m_vExtent.y, m_OBB.m_vExtent.z );
	z3d::VectorRotate( vResult, vTmp, m_Dir );
	vResult +=  m_OBB.m_Center;
	ms_pOutlineVertex[2].x = vResult.x; ms_pOutlineVertex[2].y = vResult.y; ms_pOutlineVertex[2].z = vResult.z;
	
	vTmp = vector3( -m_OBB.m_vExtent.x, -m_OBB.m_vExtent.y, m_OBB.m_vExtent.z );
	z3d::VectorRotate( vResult, vTmp, m_Dir );
	vResult +=  m_OBB.m_Center;
	ms_pOutlineVertex[3].x = vResult.x; ms_pOutlineVertex[3].y = vResult.y; ms_pOutlineVertex[3].z = vResult.z;
	
	vTmp = vector3( -m_OBB.m_vExtent.x, m_OBB.m_vExtent.y, -m_OBB.m_vExtent.z );
	z3d::VectorRotate( vResult, vTmp, m_Dir );
	vResult +=  m_OBB.m_Center;
	ms_pOutlineVertex[4].x = vResult.x; ms_pOutlineVertex[4].y = vResult.y; ms_pOutlineVertex[4].z = vResult.z;
	
	vTmp = vector3( m_OBB.m_vExtent.x, m_OBB.m_vExtent.y, -m_OBB.m_vExtent.z );
	z3d::VectorRotate( vResult, vTmp, m_Dir );
	vResult +=  m_OBB.m_Center;
	ms_pOutlineVertex[5].x = vResult.x; ms_pOutlineVertex[5].y = vResult.y; ms_pOutlineVertex[5].z = vResult.z;
	
	vTmp = vector3( m_OBB.m_vExtent.x, m_OBB.m_vExtent.y, m_OBB.m_vExtent.z );
	z3d::VectorRotate( vResult, vTmp, m_Dir );
	vResult +=  m_OBB.m_Center;
	ms_pOutlineVertex[6].x = vResult.x; ms_pOutlineVertex[6].y = vResult.y; ms_pOutlineVertex[6].z = vResult.z;
	
	vTmp = vector3( -m_OBB.m_vExtent.x, m_OBB.m_vExtent.y, m_OBB.m_vExtent.z );
	z3d::VectorRotate( vResult, vTmp, m_Dir );
	vResult +=  m_OBB.m_Center;
	ms_pOutlineVertex[7].x = vResult.x; ms_pOutlineVertex[7].y = vResult.y; ms_pOutlineVertex[7].z = vResult.z;
	
	D3DMATRIX* pMat = GetIdentityD3DMatrix();

	GetDevice()->SetTransform( D3DTS_WORLD, pMat );
	GetDevice()->SetVertexShader( D3DFVF_SLVERTEX );
	GetDevice()->DrawIndexedPrimitiveUP( D3DPT_LINELIST, 0, 8, 12, ms_pOutlineIndex, D3DFMT_INDEX16, ms_pOutlineVertex, sizeof(D3DSLVERTEX) );
}