// Z3DAniHolder.h: interface for the Z3DAniHolder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Z3DANIHOLDER_H__9061D061_E701_11D4_AD2B_0000E8EB4C69__INCLUDED_)
#define AFX_Z3DANIHOLDER_H__9061D061_E701_11D4_AD2B_0000E8EB4C69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "misc.h"
#include "Z3DAniKeyPack.h"
#include "vector.h"
#include "quaternion.h"


struct Z3DAniHolder  
{
public:
	Z3DAniHolder()
	{
		m_rpThis = this;

		m_rpPack = NULL;
		m_fSpeedFactor = 1.0f;
		m_sTick = 0;
		m_fCurrentFrame = 0;
		m_fPrevFrame = -1;	// 첫프레임 동작에서 차분치 계산을 위해 -1프레임이 세팅된것이 있을수 있으므로.
		m_bIsOverflow = true;
		m_bDirection = true;
	}

	~Z3DAniHolder()
	{
		m_rpThis = NULL;
	};

	bool SetAniPack( Z3DAniKeyPack* pPack )
	{
		if( this != m_rpThis )
		{
			return true;
		}

		if( NULL == pPack )
		{
			MessageBox( NULL, "Z3DAniHolder::SetAniPack() - NULL parameter 설정시도", 0,0 );
		}

		m_rpPack = pPack;

		// frame관련설정 reset
		//fSpeedFactor = 1.0f;
		m_sTick = 0;
		m_fCurrentFrame = 0;
		if( m_bDirection )
		{
			m_fPrevFrame = -1;	// 첫프레임 동작에서 차분치 계산을 위해 -1프레임이 세팅된적것이 있을수 있으므로.
		}
		else
		{
			m_fPrevFrame = 1;
		}
		m_bIsOverflow = false;

		return true;
	}

	bool GetControllerData( int n, vector3& v, quaternion &q )
	{
		if( this != m_rpThis )
		{
			return true;
		}

		//_ASSERT( m_rpPack );
		if( NULL == m_rpPack )
		{
			v.Identity();
			q.Identity();
		}
		else
		{
			// controller갯수보다 인덱스가 크면 오류
			if( m_rpPack->GetControllerCount() <= n )
			{
				return false;
			}

			m_rpPack->GetData(n)->GetInterpolatedKeyframe( m_fCurrentFrame, v, q );
		}

		return true;
	}


	bool GetDiffControllerData( int n, vector3& vDiff, quaternion &q )
	{
		if( this != m_rpThis )
		{
			return true;
		}

		if( NULL == m_rpPack )
		{
			vDiff.Identity();
			//v.Identity();
			q.Identity();
		}
		else
		{
			// controller갯수보다 인덱스가 크면 오류
			if( m_rpPack->GetControllerCount() <= n )
			{
				return false;
			}

			m_rpPack->GetData(n)->GetDifferentiatedKeyframe( m_fCurrentFrame, m_fPrevFrame, vDiff, q );
		}

		return true;
	}


	bool Progress()
	{
		if( this != m_rpThis )
		{
			return true;
		}

		if( !m_bIsOverflow )
		{
			m_sTick++;
		}

		if( m_rpPack )
		{
			m_fPrevFrame = m_fCurrentFrame;
			m_fCurrentFrame = m_fSpeedFactor * m_sTick;

			if( m_fCurrentFrame >= m_rpPack->GetAniLength() )
			{
				m_fCurrentFrame = float(m_rpPack->GetAniLength()-1);
				m_bIsOverflow = true;
				return true;
			}

			//
			m_bIsOverflow = false;
			return true;
		}

		return false;
	}

	bool BackwardProgress()
	{
		if( this != m_rpThis )
		{
			return true;
		}

		m_sTick--;

		if( m_rpPack )
		{
			m_fPrevFrame = m_fCurrentFrame;
			m_fCurrentFrame = m_fSpeedFactor * m_sTick;

			if( m_fCurrentFrame < -1 ) // backward wrap around
			{
				m_bIsOverflow = true;

				m_fPrevFrame = m_fCurrentFrame = -1;
				m_sTick = (short)((float)m_fCurrentFrame / m_fSpeedFactor);
			}

			return true;
		}

		return false;
	}

	bool IsOverflow()
	{
		if( this != m_rpThis )
		{
			return true;
		}

		return m_bIsOverflow;
	}

	void Loop()
	{
		if( this != m_rpThis )
		{
			return;
		}

		m_bIsOverflow = false;

		if( m_rpPack )
		{
			if( m_bDirection ) // forward
			{
				m_fCurrentFrame = 0.0f;
				m_sTick = 0;
				m_fPrevFrame = -1;
			}
			else	// backward
			{
				m_fPrevFrame = float(m_rpPack->GetAniLength() - 1);
				m_fCurrentFrame = m_fPrevFrame - 1;
				m_sTick = (short)((float)m_fCurrentFrame / m_fSpeedFactor);
			}
		}
	}

	void SetCurrentFrame(float f)
	{
		if( this != m_rpThis )
		{
			return;
		}

		if( f < (float)m_rpPack->GetAniLength() )
		{
			m_fCurrentFrame = f;
			m_fPrevFrame = f - 1.0f;
				
			m_sTick = (short)(f/m_fSpeedFactor);
			m_bIsOverflow = false;
		}
		else
		{
			m_fCurrentFrame = m_fPrevFrame = m_rpPack->GetAniLength();
			m_sTick = (short)(m_fCurrentFrame/m_fSpeedFactor);
			m_bIsOverflow = true;
		}
	}

	float GetCurrentFrame()
	{
		if( this != m_rpThis )
		{
			return 0.0f;
		}

		return m_fCurrentFrame;
	}

	float GetPrevFrame()
	{
		if( this != m_rpThis )
		{
			return 0.0f;
		}

		return m_fPrevFrame;
	}

	bool SetSpeedFactor( float f )
	{
		if( this != m_rpThis )
		{
			return true;
		}

		if( f <= 0.0f )
		{
			return false;
		}

		m_sTick = short((m_fSpeedFactor/f)*m_sTick);

		m_fSpeedFactor = f;
		return true;
	}

	const Z3DAniKeyPack* GetAniKeyPackInterface()
	{
		if( this != m_rpThis )
		{
			return NULL;
		}

		return m_rpPack;
	}


protected:
    Z3DAniHolder* m_rpThis;

	Z3DAniKeyPack* m_rpPack;
	float m_fSpeedFactor;	// animation 출력속도설정용
	short m_sTick;			// tick counter
	float m_fCurrentFrame;	// speed factor 설정으로 인해 tick과 frame이 분리됨
	float m_fPrevFrame;		// 이동시 차분키값 계산을 위한 previous frame 계산설정용
	bool m_bIsOverflow;		// 동작이 끝났는가?(출력될 frame번호가 데이터 최후frame번호 이후인가)
	bool m_bDirection;		// true = forward, false = backward
};

#endif // !defined(AFX_Z3DANIHOLDER_H__9061D061_E701_11D4_AD2B_0000E8EB4C69__INCLUDED_)
