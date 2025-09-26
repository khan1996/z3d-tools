// Z3DObject.h: interface for the CZ3DObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Z3DOBJECT_H__6D457985_AA8C_11D4_AD2B_0000E8EB4C69__INCLUDED_)
#define AFX_Z3DOBJECT_H__6D457985_AA8C_11D4_AD2B_0000E8EB4C69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include <list>
#include <d3d8.h>

#include "misc.h"

#include "vector.h"
#include "quaternion.h"
#include "matrix.h"

#include "Z3DMath.h"

#include <vector>
#include <utility>


#define Z3DOBJECT_NOTLINKED ((CZ3DObject*)0xFFFFFFFF)


class CZ3DObject
{
public:
	CZ3DObject();
	virtual ~CZ3DObject();

	// link as a child of specified object
	void Link( CZ3DObject* pObj );
	void UnLink();

	// geometric topology funcs
	void SetPosition( float x, float y, float z )
	{
		m_Pos.x = x;
		m_Pos.y = y;
		m_Pos.z = z;
	}
	void SetPosition( const vector3& pos )
	{
		m_Pos = pos;
	}

	void SetDirection( float x, float y, float z, float w )
	{
		m_Dir.x = x;
		m_Dir.y = y;
		m_Dir.z = z;
		m_Dir.w = w;
	}
	void SetDirection( const quaternion& dir )
	{
		m_Dir = dir;
	}

	void SetRotationModifierCount( int nCount );
	
	void SetRotationModifier( int nIndex, vector3 vRotOrg = vector3( 0.0f, 0.0f, 0.0f ),
		vector3 vRotAxis = vector3( 1.0f, 0.0f, 0.0f ),
		float fAngle = 0.0f );

	void SetPositionLocal( float x, float y, float z )
	{
		m_PosLocal.x = x;
		m_PosLocal.y = y;
		m_PosLocal.z = z;
	}
	void SetPositionLocal( const vector3& pos )
	{
		m_PosLocal = pos;
	}

	void SetDirectionLocal( float x, float y, float z, float w )
	{
		m_DirLocal.x = x;
		m_DirLocal.y = y;
		m_DirLocal.z = z;
		m_DirLocal.w = w;
	}
	void SetDirectionLocal( const quaternion& dir )
	{
		m_DirLocal = dir;
	}
	// only available after 'Process()' update has done
	void GetTM( matrix& m )
	{
		m = m_TM;
	}

	/*const*/ matrix* GetTM()
	{
		return &m_TM;
	}

	void GetPosition( float& x, float& y, float& z)
	{
		x = m_Pos.x;
		y = m_Pos.y;
		z = m_Pos.z;
	}
	void GetPosition( vector3& pos )
	{
		pos = m_Pos;
	}

	void GetDirection( float& x, float& y, float& z, float& w )
	{
		x = m_Dir.x;
		y = m_Dir.y;
		z = m_Dir.z;
		w = m_Dir.w;
	}

	void GetDirection( quaternion& rot )
	{
		rot = m_Dir;
	}

	void GetPositionLocal( float& x, float& y, float& z)
	{
		x = m_PosLocal.x;
		y = m_PosLocal.y;
		z = m_PosLocal.z;
	}
	void GetPositionLocal( vector3& pos )
	{
		pos = m_PosLocal;
	}

	void GetDirectionLocal( float& x, float& y, float& z, float& w )
	{
		x = m_DirLocal.x;
		y = m_DirLocal.y;
		z = m_DirLocal.z;
		w = m_DirLocal.w;
	}

	void GetDirectionLocal( quaternion& rot )
	{
		rot = m_DirLocal;
	}

	CZ3DObject* GetParent()
	{
		return m_rpParent;
	}

	void SetPivot( vector3& v )
	{
		m_vPivotPos = v;
	}

	// 확대기준점 세팅
	void SetPivot( float x, float y, float z )
	{
		m_vPivotPos.x = x;
		m_vPivotPos.y = y;
		m_vPivotPos.z = z;
	}

	void GetPivot( vector3& v )
	{
		v = m_vPivotPos;
	}


	virtual void UpdateFrame();

	void SetDisable( bool bDisabled )
	{
		m_bDisabled = bDisabled;
	}

	bool IsDisabled()
	{
		return m_bDisabled;
	}

////////// static function //////////
	static long GetObjectCount() { return ms_lObjectCount; }
	static void Process(); // batch process whole object in the list


protected:
	bool m_bDisabled;
	// Geometric topology
	vector3 m_Pos;
	quaternion m_Dir;

	vector3 m_PosLocal;
	quaternion m_DirLocal;

	// local rotation modifier
	std::vector< std::pair< vector3, quaternion > > m_vecRotationModifierOrgRot;
	
//	vector3 m_vRotationModifierOrg;
//	quaternion m_qRotationModifierRot;

	matrix m_TM; //

	vector3 m_vPivotPos;	// 확대 기준점

	// Hierarchical topology
	CZ3DObject* m_rpParent;
	CZ3DObject* m_rpSibling;
	CZ3DObject* m_rpChild;

	// only counts the #. of "hierarchycally registered" objects
	static long ms_lObjectCount;	
	static CZ3DObject* ms_rpRootObject;

//	static long ms_lObjectCountCopy1;
//	static long ms_lObjectCountCopy2;
//	static long ms_lObjectCountCopy3;
//	static long ms_lObjectCountCopy4;


	
	void AddChild( CZ3DObject* pObj )
	{
		_ASSERT( this == pObj->m_rpParent );
		
		if( m_rpChild == NULL )
		{
			m_rpChild = pObj;
		}
		else
		{
			m_rpChild->AddSibling( pObj );
		}
	}
	void AddSibling( CZ3DObject* pObj)
	{
		_ASSERT( m_rpParent == pObj->m_rpParent );
		
		// just walk through the sibling route
		CZ3DObject* p = this;
		for( ; p->m_rpSibling; p = p->m_rpSibling );
		
		p->m_rpSibling = pObj;
	}
};


#endif // !defined(AFX_Z3DOBJECT_H__6D457985_AA8C_11D4_AD2B_0000E8EB4C69__INCLUDED_)
