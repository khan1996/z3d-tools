// Z3DSkeletonObject.h: interface for the CZ3DSkeletonObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Z3DSKELETONOBJECT_H__1B9AE6C3_16F6_11D5_A643_0000E8EB4C69__INCLUDED_)
#define AFX_Z3DSKELETONOBJECT_H__1B9AE6C3_16F6_11D5_A643_0000E8EB4C69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4786)

#include "misc.h"

#include "Z3DObject.h"
#include "Z3DAniHolder.h"


class CZ3DSkeletonObject : public CZ3DObject
{
public:
	CZ3DSkeletonObject() : CZ3DObject()
	{
	}
	
	virtual ~CZ3DSkeletonObject()
	{
	}
	
	/*void ApplyAniKey( Z3DAniHolder* pHolder )
	{
		pHolder[m_nBodypartIndex].GetControllerData(m_nPartkeyIndex, m_PosLocal, m_DirLocal );
	}*/
	
	void GetLocalizedTM( matrix& m, const matrix& localizer )
	{
		Multiply( m, const_cast<matrix&>(localizer), m_TM );
	}
	
	void SetIndex( int nBodypartIndex, int nPartkeyIndex )
	{
		m_nBodypartIndex = nBodypartIndex;
		m_nPartkeyIndex = nPartkeyIndex;
	}

	int GetAniHolderIndex()
	{
		return m_nBodypartIndex;
	}

	int GetPartInHolderIndex()
	{
		return m_nPartkeyIndex;
	}
	
	/*void SetLocalizer( matrix& m )
	{
		m_Localizer = m;
	}*/
	
	/*void GetPos( vector3& v )
	{
		v.x = m_Pos.x;
		v.y = m_Pos.y;
		v.z = m_Pos.z;
	}*/
	
	
protected:
	//matrix m_Localizer;	// 소위 'TM inverse' 라고 불리는 물건.
	
	int m_nBodypartIndex;
	int m_nPartkeyIndex;
};

#endif // !defined(AFX_Z3DSKELETONOBJECT_H__1B9AE6C3_16F6_11D5_A643_0000E8EB4C69__INCLUDED_)
