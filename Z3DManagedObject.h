// Z3DManagedObject.h: interface for the CZ3DManagedObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Z3DMANAGEDOBJECT_H__F72CD383_4755_11D5_A644_0000E8EB4C69__INCLUDED_)
#define AFX_Z3DMANAGEDOBJECT_H__F72CD383_4755_11D5_A644_0000E8EB4C69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( disable : 4786 )

#include <list>
#include "misc.h"
#include <queue>


class CZ3DManagedObject  
{
public:
	CZ3DManagedObject()
	{
		//m_bDead = false;
		ms_listThis.push_back( this );
		m_itThis = --(ms_listThis.end());
	}

	virtual ~CZ3DManagedObject()
	{
	}

	void Die()
	{
		ms_listThis.erase( m_itThis );
		ms_qDead.push( this );
	}

	/*bool IsDead()
	{
		return m_bDead;
	}*/

	static void Process()
	{
		while( !(ms_qDead.empty()) )
		{
			CZ3DManagedObject* p = ms_qDead.front();
			ms_qDead.pop();
			
			SAFE_DELETE( p );
		}
	}

	static void Close()
	{
		std::list<CZ3DManagedObject*>::iterator it;

		CZ3DManagedObject* p;

		for( it = ms_listThis.begin(); it != ms_listThis.end(); it++ )
		{
			p = *it;
			SAFE_DELETE( p );
		}

		ms_listThis.clear();
	}

protected:
	std::list<CZ3DManagedObject*>::iterator m_itThis;
	
	static std::list<CZ3DManagedObject*> ms_listThis;
	static std::queue<CZ3DManagedObject*> ms_qDead;
};

#endif // !defined(AFX_Z3DMANAGEDOBJECT_H__F72CD383_4755_11D5_A644_0000E8EB4C69__INCLUDED_)
