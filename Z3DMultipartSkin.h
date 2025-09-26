// Z3DMultipartSkin.h: interface for the CZ3DMultipartSkin class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Z3DMULTIPARTSKIN_H__1B9AE6C5_16F6_11D5_A643_0000E8EB4C69__INCLUDED_)
#define AFX_Z3DMULTIPARTSKIN_H__1B9AE6C5_16F6_11D5_A643_0000E8EB4C69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning (disable:4786)

#include "misc.h"

#include "Z3DRenderable.h"
#include "Z3DMultipartPortion.h"
#include "Z3DSkeletonObject.h"


//#define Z3D_MULTIPART_PORTION_COUNT 3 // 기본,관절,망토치마 이렇게 셋..현재로는
#define Z3D_MULTIPART_PORTION_COUNT 1 // 엎은 이후로 하나로 고정됨..


class CZ3DMultipartSkin : public CZ3DRenderable
{
public:
	CZ3DMultipartSkin( Z3D_MULTIPART_PORTION_TYPE mpt )
		: CZ3DRenderable(true)
	{
		m_rpThis = this;

		// set 0xFFFFFFFF to mean 'instance manages texture'
		m_pTexture = (IDirect3DTexture8*)0xFFFFFFFF;

		m_rpSkeleton = NULL;
		m_rpSkeletonLocalizer = NULL;
		m_lSkelCount = 0;
		m_pMatrixPalette = NULL;

		m_lAniLODFrame = 1; // 0으로 하면 X된다-_-;
		m_lAniLODCounter = 0;
		m_lPrevLODLevel = 0;

		m_bIsBatchMode = false;

		for( int i = 0; i < Z3D_MULTIPART_PORTION_COUNT; ++i )
		{
			m_apPortion[i] = new Z3DMultipartPortion(mpt);
		}

		m_MPT = mpt;

		m_fScaleFactor = 1.0f;
		m_vPivotPos.Identity();

		m_vBoundingBoxMin = m_vBoundingBoxMax = vector3(0,0,0);
	}

	virtual ~CZ3DMultipartSkin()
	{
		SAFE_DELETEA( m_pMatrixPalette );

		for( int i = 0; i < Z3D_MULTIPART_PORTION_COUNT; ++i )
		{
			SAFE_DELETE( m_apPortion[i] );
		}

		m_rpThis = NULL;
	}

	void SetScale( float s )
	{
		if( this != m_rpThis )
		{
			return;
		}

		m_fScaleFactor = s;
	}

	float GetScale()
	{
		if( this != m_rpThis )
		{
			return 1.0f;
		}

		return m_fScaleFactor;
	}

	void SetPivot( vector3& v )
	{
		if( this != m_rpThis )
		{
			return;
		}

		m_vPivotPos = v;
	}

	void SetPivot( float x, float y, float z )
	{
		if( this != m_rpThis )
		{
			return;
		}

		m_vPivotPos.x = x;
		m_vPivotPos.y = y;
		m_vPivotPos.z = z;
	}

	void GetPivot( vector3& v ) const
	{
		if( this != m_rpThis )
		{
			return;
		}

		v = m_vPivotPos;
	}

	bool SetSkeleton( CZ3DSkeletonObject* pSkel, long lSkelCount )
	{
		if( this != m_rpThis )
		{
			return true;
		}

		m_rpSkeleton = pSkel;
		m_lSkelCount = lSkelCount;

		if( NULL == (m_pMatrixPalette = new matrix[m_lSkelCount]) )
		{
			return false;
		}

		return true;
	}

	void SetSkeletonLocalizer( const matrix* pMat )
	{
		if( this != m_rpThis )
		{
			return;
		}

		m_rpSkeletonLocalizer = pMat;
	}

	bool SetMesh( const char* szMeshName );
	bool DeleteMesh( int nPartId );

	bool SetTex( const char* szTexName, const char* szTex2Name = NULL );
	bool DeleteTex( int nPartId );

	bool SetMeshTexture( const char* szMeshName, const char* szTextureName, const char* szTexture2Name, long lGradeEffectIndex, long lGlowIndex );

	bool BatchOpen();
	//bool BatchSet( int nPartId, Z3DHANDLE handleMesh, Z3DHANDLE handleTexpiece );
	//bool BatchSet( Z3DHANDLE handleMesh, Z3DHANDLE handleTexpiece );
	//bool BatchDelete( int nPartId );
	bool BatchClose();

	void ApplyAnimation();

	void Render();

	void SecondRender()
	{
		if( this != m_rpThis )
		{
			return;
		}

		// do nothing-_-;
	}

	// 몇 프레임당 한번씩 blending계산을 할 것인지 설정
	void SetAniLODFrame( long l )
	{
		if( this != m_rpThis )
		{
			return;
		}

		_ASSERT( l > 0 );

		m_lAniLODFrame = l;
		m_lAniLODCounter = 0; // reset counter
	}

	void RenderShadow( IDirect3DDevice8* pDevice, DWORD vertexShader = 0xFFFFFFFF, int nLevel = -1 );
	void RenderGlowShape( IDirect3DDevice8* pDevice );

	bool CheckRecalcByAniLOD()
	{
		if( this != m_rpThis )
		{
			return true;
		}

		int nLODLevel = GetLODLevel();

		if( nLODLevel != m_lPrevLODLevel )
		{
			SetAniLODFrame( ms_alAniLODFrameTable[nLODLevel] );
			m_lPrevLODLevel = nLODLevel;
		}

		return (m_lAniLODCounter == 0);
	}

	void GetBoundingBox( vector3& r_vmin, vector3& r_vmax );

	void GetProcessedVertex( Z3DBlend2Vertex& v_in, vector3& v_out ) const;

	long GetPolyCount( int nLODLevel = -1 ) const;

	void SetTestMode( bool b )
	{
		if( this != m_rpThis )
		{
			return;
		}

		m_bTestMode = b;
	}
	

protected:
	vector3 m_vBoundingBoxMin;
	vector3 m_vBoundingBoxMax;

	CZ3DMultipartSkin* m_rpThis;

	bool m_bTestMode;
	vector3 m_vPivotPos;
	float m_fScaleFactor;
	Z3D_MULTIPART_PORTION_TYPE m_MPT;
	const matrix* m_rpSkeletonLocalizer;
	Z3DMultipartPortion* m_apPortion[Z3D_MULTIPART_PORTION_COUNT];

	CZ3DSkeletonObject* m_rpSkeleton;
	long m_lSkelCount;
	matrix* m_pMatrixPalette;

	long m_lAniLODCounter;
	long m_lAniLODFrame; // 몇 프레임마다 한번씩 blending 계산을 해 줘야되나
	long m_lPrevLODLevel;

	bool m_bIsBatchMode;

	static long ms_alAniLODFrameTable[Z3D_LOD_LEVEL];

	void IncreaseAniLODCounter()
	{
		if( this != m_rpThis )
		{
			return;
		}

		++m_lAniLODCounter;
		m_lAniLODCounter %= m_lAniLODFrame;
		//m_lAniLODCounter = 0;
	}
	static int GetIdFromMeshName( char* szMeshName );

	static int GetPortionIndex( int nPartId )
	{
		if( nPartId > 300 )
		{
			return -1;
		}

		return (nPartId / 100);
	}
};

#endif // !defined(AFX_Z3DMULTIPARTSKIN_H__1B9AE6C5_16F6_11D5_A643_0000E8EB4C69__INCLUDED_)
