// Z3DCharacterModel.h: interface for the CZ3DCharacterModel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Z3DCHARACTERMODEL_H__FBE26EA1_C206_11D4_AD2B_0000E8EB4C69__INCLUDED_)
#define AFX_Z3DCHARACTERMODEL_H__FBE26EA1_C206_11D4_AD2B_0000E8EB4C69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( disable : 4786 )

//#include "Z3DObject.h"
#include "Z3DRenderable.h"
#include "misc.h"

#include "vector.h"
#include "quaternion.h"
#include "matrix.h"

#include "Z3DTexturePiece.h"
#include "Z3DAniKeyPack.h"
#include "Z3dTypes.h"

#include "SimpleString.h"

#include <list>
#include <queue>


#include "Z3D_CONSTANTS.h"
#include "Z3D_Globals.h"

#include "Z3DGradeEffectHandler.h"
#include "Z3DGlowHandler.h"



// Implementing outfit of character(engine-level character implementation)
class CZ3DCharacterModel  
{
public:
	CZ3DCharacterModel()
	{
		m_bDisabled = false;
		ms_list_pModel.push_back( this );
		m_iterThis = --(ms_list_pModel.end());

		m_Pos.Identity();
		m_PrevPos.Identity();
		m_fDir = 0.0f;

		m_fDisplacementFactor = 1.0f;
		m_fDisplacementRotation = 0.0f;
	}
	virtual ~CZ3DCharacterModel()
	{
		ms_list_pModel.erase( m_iterThis );
	}

	virtual void SetDisable( bool bDisabled )
	{
		m_bDisabled = bDisabled;			
	}

	bool IsDisabled()
	{
		return m_bDisabled;
	}

	void SetNotification( Z3D_CHR_NOTIFICATION noty )
	{
		m_qNotification.push( noty );
	}

	Z3D_CHR_NOTIFICATION GetNotification()
	{
		Z3D_CHR_NOTIFICATION noty;

		if( m_qNotification.empty() )
		{
			return Z3D_CN_NOTHING;
		}
		else
		{
			noty = m_qNotification.front();
			m_qNotification.pop();
		}

		return noty;
	}

	void SetPosition( float x, float y, float z )
	{
		if( 0L == (long)x || 0L == (long)z )
		{
			return;
		}

		m_PrevPos = m_Pos;

		m_Pos.x = x;
		m_Pos.y = y;
		m_Pos.z = z;
	}

	void SetPosition( const vector3 &v )
	{
		if( 0L == (long)v.x || 0L == (long)v.z )
		{
			return;
		}

		m_PrevPos = m_Pos;

		m_Pos = v;
	}

	void GetPosition( float &x, float &y, float &z )
	{
		x = m_Pos.x;
		y = m_Pos.y;
		z = m_Pos.z;
	}

	void GetPosition( vector3 &v )
	{
		v = m_Pos;
	}
	
	vector3 GetPosition()
	{
		return m_Pos;
	}
	
	void GetPrevPosition( float &x, float &y, float &z )
	{
		x = m_PrevPos.x;
		y = m_PrevPos.y;
		z = m_PrevPos.z;
	}
	
	void GetPrevPosition( vector3 &v )
	{
		v = m_PrevPos;
	}
	
	vector3 GetPrevPosition()
	{
		return m_PrevPos;
	}
	
	void SetDirection( float fDir )
	{
		//_ASSERT( -FLOAT_PHI <= fDir && fDir <= FLOAT_PHI );
		
		m_fDir = fDir+ 2*FLOAT_PHI;

		while( m_fDir > (2*FLOAT_PHI) )
		{
			m_fDir -= 2*FLOAT_PHI;
		}
		while( m_fDir < (-2*FLOAT_PHI) )
		{
			m_fDir += 2*FLOAT_PHI;
		}
	}

	float GetDirection()
	{
		return m_fDir;
	}


	void Rotate( float fDr ) { m_fDir += fDr; }

	//void Move( float x, float z, float y = INVALID_HEIGHT )
	void Move( float x, float y, float z )
	{
		float sf = sinf(m_fDir), cf = cosf(m_fDir);

		m_Pos.z += cf*z - sf*x;
		m_Pos.x += sf*z + cf*x;

		//_ASSERT( ms_rpStub );
		//m_Pos.y = (y == INVALID_HEIGHT) ? ms_rpStub->GetHeight(m_Pos.x, m_Pos.z) : m_Pos.y+y;
		m_Pos.y = y;
	}

	//void MoveWorld( float x, float z, float y = INVALID_HEIGHT )
	void MoveWorld( float x, float y, float z )
	{
		m_Pos.x += x;
		m_Pos.z += z;

		//_ASSERT( ms_rpStub );
		//m_Pos.y = (y == INVALID_HEIGHT) ? ms_rpStub->GetHeight(m_Pos.x, m_Pos.z) : m_Pos.y+y;
		m_Pos.y = y;
	}

	void SetDisplacementFactor( float f )
	{
		m_fDisplacementFactor = f;
	}

	float GetDisplacementFactor()
	{
		return m_fDisplacementFactor;
	}

	void SetDisplacementRotation( float f )
	{
		m_fDisplacementRotation = f;
	}

	 float GetDisplacementRotation()
	{
		return m_fDisplacementRotation;
	}

	virtual void FrameMove() = 0;

	// ¾×¼Ç..
	virtual void Act( Z3D_CHR_ACTION action, long lParam1 = 0, long lParam2 = 0 )
	{
		// do noting
		switch( action )
		{
		case Z3D_CA_NOTIFY_HIT_BY_FIREBALL:
			SetNotification( Z3D_CN_HIT_BY_FIREBALL );
			break;
		}
	}

	
	//////////////////////////////////////////////////////////

	static bool _Init( const char* szDataPath );
	//static void Close();


	static void Process()
	{
		std::list<CZ3DCharacterModel*>::iterator iter;

		for( iter = ms_list_pModel.begin(); iter != ms_list_pModel.end(); iter++ )
		{
			if( (*iter)->IsDisabled() )
			{
				continue;
			}

			(*iter)->FrameMove();
		}

		CZ3DGradeEffectHandler::Process();
		CZ3DGlowHandler::_Process();
	}

	static long GetTotalCharacterCount()
	{
		return static_cast<long>(ms_list_pModel.size());
	}


protected:
	bool m_bDisabled;
	std::queue<Z3D_CHR_NOTIFICATION> m_qNotification;
	vector3 m_Pos;
	vector3 m_PrevPos;
	float m_fDir;	// -phi ~ phi

	float m_fDisplacementFactor;
	float m_fDisplacementRotation;

	//vector3	m_posDiff;
	//float	m_dirDiff;

	std::list<CZ3DCharacterModel*>::iterator m_iterThis;

	//////////////////////////////////////////////////////////
	static std::list<CZ3DCharacterModel*> ms_list_pModel;

	static SimpleString ms_strDataPath;
};



#endif // !defined(AFX_Z3DCHARACTERMODEL_H__FBE26EA1_C206_11D4_AD2B_0000E8EB4C69__INCLUDED_)
