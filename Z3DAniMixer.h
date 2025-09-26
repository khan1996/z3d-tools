// Z3DAniMixer.h: interface for the CZ3DAniMixer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Z3DANIMIXER_H__04DC980C_7996_47FE_BF88_BF0EB534AEA5__INCLUDED_)
#define AFX_Z3DANIMIXER_H__04DC980C_7996_47FE_BF88_BF0EB534AEA5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Z3DSkeletonObject.h"
#include "Z3DAniHolder.h"

#include "vector.h"
#include "quaternion.h"


struct Z3D_POSNDIR
{
	vector3	pos;
	quaternion dir;
};

class CZ3DAniMixer  
{
public:
	CZ3DAniMixer();
	virtual ~CZ3DAniMixer();

	bool Init( CZ3DSkeletonObject* pSkel, const matrix* pLocalizer, Z3DAniHolder* pAniHolder, Z3DAniHolder* pAddonAniHolder, long lSkelCount, long lAniHolderCount );
	void Swap( long lAniHolderIndex, bool bTransitFlag = true );
	void Mix();

	void SetAddonFactorPeak( float f )
	{
		m_fAddonFactorPeak = f;
	}

	void SustainLast( bool b )
	{
		m_bSustainLast = b;
	}

protected:
	Z3D_POSNDIR* m_apOperand[2];
	long* m_pActiveIndex;
	bool* m_pTransitFlag;

	long m_lSkelCount;
	long m_lAniHolderCount;

	float m_fAddonFactorPeak;
	bool m_bSustainLast;

	CZ3DSkeletonObject* m_rpSkeleton;
	Z3DAniHolder* m_rpAniHolder;
	Z3DAniHolder* m_rpAddonAniHolder;

	float GetAddFactorCurve( float fCurrent, float fAniLength );
};

#endif // !defined(AFX_Z3DANIMIXER_H__04DC980C_7996_47FE_BF88_BF0EB534AEA5__INCLUDED_)
