// Z3DAniMixer.cpp: implementation of the CZ3DAniMixer class.
//
//////////////////////////////////////////////////////////////////////

#include "Z3DAniMixer.h"
#include "GMMemory.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CZ3DAniMixer::CZ3DAniMixer()
{
	m_apOperand[0] = NULL;
	m_apOperand[1] = NULL;

	m_pActiveIndex = NULL;
	m_pTransitFlag = NULL;
	
	m_lSkelCount = 0;
	m_lAniHolderCount = 0;

	m_rpSkeleton = NULL;
	m_rpAniHolder = NULL;
	m_rpAddonAniHolder = NULL;

	m_fAddonFactorPeak = 0.5f;
	m_bSustainLast = false;
}

CZ3DAniMixer::~CZ3DAniMixer()
{
	SAFE_DELETEA( m_apOperand[0] );
	SAFE_DELETEA( m_apOperand[1] );

	SAFE_DELETEA( m_pActiveIndex );
	SAFE_DELETEA( m_pTransitFlag );
}


bool CZ3DAniMixer::Init( CZ3DSkeletonObject* pSkel, const matrix* pLocalizer, Z3DAniHolder* pAniHolder, Z3DAniHolder* pAddonAniHolder, long lSkelCount, long lAniHolderCount )
{
	int i;

	if( NULL == pSkel || NULL == pAniHolder )
	{
		return false;
	}

	m_rpSkeleton = pSkel;
	m_rpAniHolder = pAniHolder;
	m_rpAddonAniHolder = pAddonAniHolder;

	m_lSkelCount = lSkelCount;
	m_lAniHolderCount = lAniHolderCount;

	m_apOperand[0] = new Z3D_POSNDIR[m_lSkelCount];
	m_apOperand[1] = new Z3D_POSNDIR[m_lSkelCount];

	matrix matInitialTM;
	for( i = 0; i < m_lSkelCount; ++i )
	{
		matInitialTM.Inverse(pLocalizer[i]);
		
//		m_apOperand[0][i].pos.Identity();
//		m_apOperand[0][i].dir.Identity();
//
//		m_apOperand[1][i].pos.Identity();
//		m_apOperand[1][i].dir.Identity();

		m_apOperand[0][i].pos = matInitialTM.GetLoc();
		z3d::QuaternionRotationMatrix( m_apOperand[0][i].dir, matInitialTM );

		m_apOperand[1][i] = m_apOperand[0][i];
	}

	m_pActiveIndex = new long[m_lAniHolderCount];
	m_pTransitFlag = new bool[m_lAniHolderCount];
	for( i = 0; i < m_lAniHolderCount; ++i )
	{
		m_pActiveIndex[i] = 0;
		m_pTransitFlag[i] = true;
	}

	return true;
}


void CZ3DAniMixer::Swap( long lAniHolderIndex, bool bTransitFlag )
{
	if( lAniHolderIndex >= m_lAniHolderCount )
	{
		return;
	}

	m_pActiveIndex[lAniHolderIndex] = (m_pActiveIndex[lAniHolderIndex] + 1) % 2;
	m_pTransitFlag[lAniHolderIndex] = bTransitFlag;
}


float CZ3DAniMixer::GetAddFactorCurve( float fCurrent, float fAniLength )
{
	float fInterval = min( 10.0f, (fAniLength) * 0.3f );

	if( fCurrent < fInterval )
	{
		return (fCurrent / fInterval) * m_fAddonFactorPeak;
	}
	else if( (fAniLength - fCurrent) < fInterval && m_bSustainLast )
	{
		return ( (fAniLength-fCurrent) / fInterval) * m_fAddonFactorPeak;
	}

	return m_fAddonFactorPeak;
}


void CZ3DAniMixer::Mix()
{
	int i;
	int nAniHolderIndex, nPartInHolderIndex;
	float fDuration, fAniLength, fCurrent, fTmp;

	vector3 posAdd;
	quaternion dirAdd;

	for( i = 0; i < m_lSkelCount; i++ )
	{
		nAniHolderIndex = m_rpSkeleton[i].GetAniHolderIndex();
		nPartInHolderIndex = m_rpSkeleton[i].GetPartInHolderIndex();
		
		// anikeypack 이 null이면 pos와 dir는 identity설정이 됨
		m_rpAniHolder[nAniHolderIndex].GetControllerData(
			nPartInHolderIndex,
			m_apOperand[m_pActiveIndex[nAniHolderIndex]][i].pos,
			m_apOperand[m_pActiveIndex[nAniHolderIndex]][i].dir );

		if( !m_rpAniHolder[nAniHolderIndex].IsOverflow() )
		{
			fAniLength = m_rpAniHolder[nAniHolderIndex].GetAniKeyPackInterface()->GetAniLength();
			fCurrent = m_rpAniHolder[nAniHolderIndex].GetCurrentFrame();
			fDuration = min( 6.0f, (fAniLength) * 0.2f );
			if( fCurrent < fDuration && m_pTransitFlag[nAniHolderIndex] )
			{
				fTmp = fCurrent / fDuration;
				z3d::VectorLerp( 
					m_apOperand[m_pActiveIndex[nAniHolderIndex]][i].pos,
					m_apOperand[((m_pActiveIndex[nAniHolderIndex]+1)%2)][i].pos,
					m_apOperand[m_pActiveIndex[nAniHolderIndex]][i].pos, fTmp );

				z3d::QuaternionSlerp(
					m_apOperand[m_pActiveIndex[nAniHolderIndex]][i].dir,
					m_apOperand[((m_pActiveIndex[nAniHolderIndex]+1)%2)][i].dir,
					m_apOperand[m_pActiveIndex[nAniHolderIndex]][i].dir, fTmp );
			}
			// else
			// do nothing
		}

		if( !m_rpAddonAniHolder[nAniHolderIndex].IsOverflow() )
		{
			fAniLength = m_rpAddonAniHolder[nAniHolderIndex].GetAniKeyPackInterface()->GetAniLength();
			fCurrent = m_rpAddonAniHolder[nAniHolderIndex].GetCurrentFrame();

			m_rpAddonAniHolder[nAniHolderIndex].GetControllerData(
				nPartInHolderIndex, posAdd, dirAdd );

			fTmp = GetAddFactorCurve( fCurrent, fAniLength );

			z3d::VectorLerp( 
				m_apOperand[m_pActiveIndex[nAniHolderIndex]][i].pos,
				m_apOperand[m_pActiveIndex[nAniHolderIndex]][i].pos,
				posAdd, fTmp );

			z3d::QuaternionSlerp(
				m_apOperand[m_pActiveIndex[nAniHolderIndex]][i].dir,
				m_apOperand[m_pActiveIndex[nAniHolderIndex]][i].dir,
				dirAdd, fTmp );
		}
	}

	for( i = 0; i < m_lSkelCount; i++ )
	{
		nAniHolderIndex = m_rpSkeleton[i].GetAniHolderIndex();

		m_rpSkeleton[i].SetPositionLocal(
				m_apOperand[m_pActiveIndex[nAniHolderIndex]][i].pos );
		m_rpSkeleton[i].SetDirectionLocal(
				m_apOperand[m_pActiveIndex[nAniHolderIndex]][i].dir );
	}
}
