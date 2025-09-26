// Z3DWeapon.h: interface for the CZ3DWeapon class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Z3DWEAPON_H__23D7C421_26A5_11D5_A644_0000E8EB4C69__INCLUDED_)
#define AFX_Z3DWEAPON_H__23D7C421_26A5_11D5_A644_0000E8EB4C69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Z3DAttachment.h"
#include "Z3DBladeTrail.h"
//#include "Z3DFireBlade.h"


class CZ3DWeapon : public CZ3DAttachment  
{
public:
	CZ3DWeapon( CZ3DObject* pManipulator ) : CZ3DAttachment()
	{
		m_pBladeTrail = new CZ3DBladeTrail( this, "blade_trail_1.dds", 7, 4 );
		m_pBowString = new CZ3DBowString(  this, "blade_trail_1.dds", pManipulator );
	}

	virtual ~CZ3DWeapon()
	{
		SAFE_DELETE( m_pBladeTrail );
		SAFE_DELETE( m_pBowString );
	}

	CZ3DBladeTrail* GetBladeTrailInterface()
	{
		return m_pBladeTrail;
	}

	void SetBowMode( bool bBowMode )
	{
		m_bBowMode = bBowMode;

		if( m_bBowMode )
		{
			// bow mode
			m_pBowString->Enable();
			m_pBladeTrail->Disable();
		}
		else
		{
			// sword mode
			m_pBladeTrail->Enable();
			m_pBowString->Disable();
		}
	}

	void TurnOnTrail()
	{
		if( m_bBowMode )
		{
			m_pBowString->GripString();
		}
		else
		{
			m_pBladeTrail->Activate();
		}
	}

	void TurnOffTrail()
	{
		if( m_bBowMode )
		{
			m_pBowString->ReleaseString();
		}
		else
		{
			m_pBladeTrail->Stop();
		}
	}

	void TurnOnFire()
	{
		/*if( m_pFire )
		{
			return;
		}*/

		//m_pFire = new CZ3DFireBlade( this );
	}

	void TurnOffFire()
	{
		//SAFE_DELETE( m_pFire );
	}

	void SetCullByLODLevel( long l )
	{
		if( l < 2 )
		{
			m_bCull = false;
		}
		else
		{
			m_bCull = true;
		}
	}
	
protected:
	bool m_bBowMode;
	CZ3DBladeTrail* m_pBladeTrail;
	CZ3DBowString* m_pBowString;
	//CZ3DFireBlade* m_pFire;
};

#endif // !defined(AFX_Z3DWEAPON_H__23D7C421_26A5_11D5_A644_0000E8EB4C69__INCLUDED_)
