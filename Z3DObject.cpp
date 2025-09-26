// Z3DObject.cpp: implementation of the CZ3DObject class.
//
//////////////////////////////////////////////////////////////////////

#include "Z3DObject.h"
#include <stack>
#include "GMMemory.h"



//////////////////////////////////////////////////////////////////////
// Static member definition
//////////////////////////////////////////////////////////////////////
//ObjectList CZ3DObject::ms_ObjectList;
long CZ3DObject::ms_lObjectCount = 0;
//long CZ3DObject::ms_lObjectCountCopy1 = 0;
//long CZ3DObject::ms_lObjectCountCopy2 = 0;
//long CZ3DObject::ms_lObjectCountCopy3 = 0;
//long CZ3DObject::ms_lObjectCountCopy4 = 0;
CZ3DObject* CZ3DObject::ms_rpRootObject = NULL;



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CZ3DObject::CZ3DObject()
{
	m_bDisabled = false;

	// initialize member data
	m_Pos.Identity();
	m_Dir.Identity();

	m_PosLocal.Identity();
	m_DirLocal.Identity();

	//m_MovLocal.Identity();
	//m_RotLocal.Identity();

	//m_Mov.Identity();
	//m_Rot.Identity();

	m_vPivotPos.Identity();


	m_rpParent = Z3DOBJECT_NOTLINKED; // to represent 'not linked'
	m_rpChild = NULL;
	m_rpSibling = NULL;
}


CZ3DObject::~CZ3DObject()
{
	ms_lObjectCount--;

//	ms_lObjectCountCopy1--;
//	ms_lObjectCountCopy2--;
//	ms_lObjectCountCopy3--;
//	ms_lObjectCountCopy4--;
}



void CZ3DObject::Link( CZ3DObject* pParentObj )
{
	//_ASSERT( m_rpParent == Z3DOBJECT_NOTLINKED );
	bool bChangeLink = false;

	// 현재 부모와 같은 경우 - 암것도 안함
	if( m_rpParent == pParentObj )
	{
		return;
	}

	// link된적이 있다면 링크를 바꾸는 경우로 체크
	if( m_rpParent != Z3DOBJECT_NOTLINKED )
	{
		UnLink();
		bChangeLink = true;
	}

	// hierarchy setting
	m_rpParent = pParentObj;

	if( pParentObj == NULL ) // link as scene-root ?
	{
		if( GetObjectCount() == 0 )
		{
			// no object in list - this object is the first root obj.
			ms_rpRootObject = this;
		}
		else
		{
			// set as sibling of other scene-root objects
			ms_rpRootObject->AddSibling( this );
		}
	}
	else // link as a child of an object
	{
		pParentObj->AddChild( this );
	}

	// 링크를바꾸는 경우 - pos및 dir 재설정
	if( bChangeLink )
	{
		if( m_rpParent )
		{
			matrix m, m1, m2;
			// m1 : inverse of all of the parents
			m = m_rpParent->m_TM;
			m1.Inverse( m );
			// m2 : TM_world;
			m2 = m_TM;
			// m = TM_world x inv_all_parent
			m_TM = m2 * m1;
		}
		// decompose
		z3d::QuaternionRotationMatrix( m_Dir, m_TM );
		m_Pos.x = m_TM._41;
		m_Pos.y = m_TM._42;
		m_Pos.z = m_TM._43;
	}
	// 바꾸는 경우가 아니라면 신규등록
	else
	{
		ms_lObjectCount++;

//		ms_lObjectCountCopy1++;
//		ms_lObjectCountCopy2++;
//		ms_lObjectCountCopy3++;
//		ms_lObjectCountCopy4++;
	}
}


void CZ3DObject::UnLink()
{
	CZ3DObject* pObj;

	if( Z3DOBJECT_NOTLINKED == m_rpParent )
	{
		return; // nothing to do 
	}

	// setting child ptr. of parent object and get first sibling
	// pObj = first sibling in the same level as this object
	if( m_rpParent == NULL ) // scene root
	{
		pObj = ms_rpRootObject;
	}
	else // not a scene root
	{
		pObj = m_rpParent->m_rpChild;
		if( this == pObj ) // this == first in the siblings?
		{
			m_rpParent->m_rpChild = m_rpSibling;
		}
	}

	// adjust sibling relation
	if( this != pObj ) // non-first sibling
	{
		// find previous silbing
		while( pObj->m_rpSibling != this )
		{
			pObj = pObj->m_rpSibling;
		}

		pObj->m_rpSibling = m_rpSibling;
	}
	
	else // first sibling
	{
		if( m_rpParent == NULL ) // scene root
		{
			ms_rpRootObject = m_rpSibling;
		}
	}
	

	m_rpParent = Z3DOBJECT_NOTLINKED;
	m_rpSibling = NULL;
}


void CZ3DObject::UpdateFrame()
{
	vector3 v;
	quaternion q;
	
	if( m_rpParent )
	{
		m_rpParent->GetPosition( v );
		m_rpParent->GetDirection( q );
		z3d::VectorRotate( m_Pos, m_PosLocal, q );
		m_Pos += v;
		m_Dir = m_DirLocal * q;
	}
	else
	{
		m_Pos = m_PosLocal;
		m_Dir = m_DirLocal;
	}
	
	// rotation modifier accumulation
	// - origin-specific rotation
	vector3 vMod = vector3( 0, 0, 0 );
	quaternion qMod( 0, 0, 0, 1 );

	vector3 vTmp;
	for( int i = 0; i < (int)m_vecRotationModifierOrgRot.size(); ++i )
	{
		vTmp = vMod - m_vecRotationModifierOrgRot[i].first;
		z3d::VectorRotate( vMod, vTmp, m_vecRotationModifierOrgRot[i].second );
		vMod += m_vecRotationModifierOrgRot[i].first;

		q = qMod;
		qMod = q * m_vecRotationModifierOrgRot[i].second;
	}

	// applying rotation modifier
	z3d::VectorRotate( v, vMod, m_Dir );
	m_Pos = v + m_Pos;
	
	q = m_Dir;
	m_Dir = qMod * q;

	
	
	z3d::MatrixRotationQuaternion( m_TM, m_Dir );
	m_TM._41 = m_Pos.x;
	m_TM._42 = m_Pos.y;
	m_TM._43 = m_Pos.z;
}


void CZ3DObject::Process()
{
	static std::stack<CZ3DObject*> stackpObj;

	if( NULL == ms_rpRootObject ) // nothing to do when nothing's in the hierarchy tree
		return;

	// implementing recursion with STL stack
	CZ3DObject* pObj = ms_rpRootObject;

	stackpObj.push( pObj );
	while( !stackpObj.empty() )
	{
		pObj = stackpObj.top();
		stackpObj.pop();

		if( pObj->m_rpSibling )
			stackpObj.push( pObj->m_rpSibling );

		if( !(pObj->IsDisabled()) )
		{
			pObj->UpdateFrame();

			if( pObj->m_rpChild )
				stackpObj.push( pObj->m_rpChild );
		}
	}
}


void CZ3DObject::SetRotationModifierCount( int nCount )
{
	for( int i = m_vecRotationModifierOrgRot.size(); i < nCount; ++i )
	{
		m_vecRotationModifierOrgRot.push_back(
			std::pair< vector3, quaternion >(vector3(0,0,0),quaternion(0,0,0,1) ) );
	}
}


void CZ3DObject::SetRotationModifier( int nIndex, vector3 vRotOrg, vector3 vRotAxis, float fAngle )
{
	if( nIndex < (int)m_vecRotationModifierOrgRot.size() )
	{
		m_vecRotationModifierOrgRot[nIndex].first = vRotOrg;
		
		z3d::QuaternionAxisAngle( m_vecRotationModifierOrgRot[nIndex].second,
			vRotAxis.x, vRotAxis.y, vRotAxis.z, FLOAT_DEG(fAngle) );
	}
}