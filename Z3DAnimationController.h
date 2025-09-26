// Z3DAnimationController.h: interface for the CZ3DAnimationController class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Z3DANIMATIONCONTROLLER_H__3CA8D861_B23B_11D4_AD2B_0000E8EB4C69__INCLUDED_)
#define AFX_Z3DANIMATIONCONTROLLER_H__3CA8D861_B23B_11D4_AD2B_0000E8EB4C69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "vector.h"
#include "quaternion.h"
#include "misc.h"

#include "Z3DMath.h"

#pragma pack(push, Z3D_ALIGN4)
#pragma pack(4)


struct Z3DPosKey
{
	short tick;
	vector3 pos;
};

struct Z3DRotKey
{
	short tick;
	quaternion rot;
};

struct Z3DAnimationController  
{
	//vector3 m_TMPos;
	//quaternion m_TMRot;
	
	Z3DPosKey* m_pPosKeys;
	short m_sPosKeyCount;

	Z3DRotKey* m_pRotKeys;
	short m_sRotKeyCount;

	void GetInterpolatedKeyframe( float t, vector3& v, quaternion& q );
	void GetDifferentiatedKeyframe( float t, float t_prev, vector3& vDiff, quaternion& q );

	Z3DAnimationController()
	{
		m_pPosKeys = NULL;
		m_pRotKeys = NULL;

		m_sPosKeyCount = 0;
		m_sRotKeyCount = 0;
	}

	~Z3DAnimationController()
	{
		SAFE_DELETEA( m_pPosKeys );
		SAFE_DELETEA( m_pRotKeys );
	}
};



#pragma pack(pop, Z3D_ALIGN4)

#endif // !defined(AFX_Z3DANIMATIONCONTROLLER_H__3CA8D861_B23B_11D4_AD2B_0000E8EB4C69__INCLUDED_)
