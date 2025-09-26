// Z3DAniKeyPack.h: interface for the Z3DAniKeyPack class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Z3DANIKEYPACK_H__93B85D22_B627_11D4_AD2B_0000E8EB4C69__INCLUDED_)
#define AFX_Z3DANIKEYPACK_H__93B85D22_B627_11D4_AD2B_0000E8EB4C69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Z3DAnimationController.h"
#include "misc.h"


struct Z3DAniKeyPack
{
public:
	Z3DAniKeyPack()
	{
		m_pAniControllers = NULL;
	}

	~Z3DAniKeyPack()
	{
		SAFE_DELETEA( m_pAniControllers );
	}

	int GetId()    
	{
		return m_wId;
	}

	bool Load( char* szFilename );
	bool Save( char* szFileName );
	
	Z3DAnimationController* GetData(int n) { return &(m_pAniControllers[n]); }
	short GetAniLength() const { return m_sAniLength; }
	short GetControllerCount() const { return m_sAniControllerCount; }

	bool GetDisplacement( vector3 &v_disp, long lControllerIndex = 0 );

protected:
	WORD m_wId;
	short m_sAniControllerCount;
	Z3DAnimationController* m_pAniControllers;
	short m_sAniLength; // length(in frames) of animation set(pack)
};

#endif // !defined(AFX_Z3DANIKEYPACK_H__93B85D22_B627_11D4_AD2B_0000E8EB4C69__INCLUDED_)
