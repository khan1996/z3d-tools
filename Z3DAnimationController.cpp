// Z3DAnimationController.cpp: implementation of the CZ3DAnimationController class.
//
//////////////////////////////////////////////////////////////////////

#include "Z3DAnimationController.h"
#include "misc.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



void Z3DAnimationController::GetInterpolatedKeyframe(float t, vector3 &v, quaternion &q)
{
	static int i;
	static float f;

	// over-the-end-of-the-data
	if( t < m_pPosKeys[0].tick )
	{
		v = m_pPosKeys[0].pos;
	}
	else if( t >= m_pPosKeys[m_sPosKeyCount-1].tick ) // require pos-keyframe past the end of the data
	{
		v = m_pPosKeys[m_sPosKeyCount-1].pos;
	}
	else // do interpolation for inbetween keyframe
	{
		for( i = 0; i < m_sPosKeyCount-1; i++ ) // last item was processed by above portion of code
		{
			if( m_pPosKeys[i].tick == t )
			{
				v = m_pPosKeys[i].pos;
				break;
			}
			else if( m_pPosKeys[i].tick < t && t < m_pPosKeys[i+1].tick )
			{
				f = (float)(t - m_pPosKeys[i].tick) /
					(float)(m_pPosKeys[i+1].tick - m_pPosKeys[i].tick);
				z3d::VectorLerp( v, m_pPosKeys[i].pos, m_pPosKeys[i+1].pos, f );
				break;
			}
		}
	}

	if( t < m_pRotKeys[0].tick )
	{
		q = m_pRotKeys[0].rot;
	}
	else if( t >= m_pRotKeys[m_sRotKeyCount-1].tick ) // require rot-keyframe past the end of the data
	{
		q = m_pRotKeys[m_sRotKeyCount-1].rot;
	}
	else
	{
		for( i = 0; i < m_sRotKeyCount-1; i++ ) // last item was processed by above portion of code
		{
			if( m_pRotKeys[i].tick == t )
			{
				q = m_pRotKeys[i].rot;
				break;
			}
			else if( m_pRotKeys[i].tick < t && t < m_pRotKeys[i+1].tick )
			{
				f = (float)(t - m_pRotKeys[i].tick) /
					(float)(m_pRotKeys[i+1].tick - m_pRotKeys[i].tick);
				z3d::QuaternionSlerp( q, m_pRotKeys[i].rot, m_pRotKeys[i+1].rot, f );
				break;
			}
		}
	}
}


// �̵����� ���ؼ��� ���� �����Ӱ��� ���̰��� ��ȯ�Ѵ�.(ȸ���� GetInterpol..�� ����)
void Z3DAnimationController::GetDifferentiatedKeyframe( float t, float t_prev, vector3& vDiff,
													   quaternion& q )
{
	static int i;
	static float f;
	vector3 v1, v2;


	// ��ġ�� ����ġ ��� - ���� �������� ��ġ���� v1�� ����
	if( t < m_pPosKeys[0].tick )
	{
		v1 = m_pPosKeys[0].pos;
	}
	else if( t >= m_pPosKeys[m_sPosKeyCount-1].tick ) // require pos-keyframe past the end of the data
	{
		v1 = m_pPosKeys[m_sPosKeyCount-1].pos;
	}
	else // do interpolation for inbetween keyframe
	{
		for( i = 0; i < m_sPosKeyCount-1; i++ ) // last item was processed by above portion of code
		{
			if( m_pPosKeys[i].tick == t )
			{
				v1 = m_pPosKeys[i].pos;
				break;
			}
			else if( m_pPosKeys[i].tick < t && t < m_pPosKeys[i+1].tick )
			{
				f = (float)(t - m_pPosKeys[i].tick) /
					(float)(m_pPosKeys[i+1].tick - m_pPosKeys[i].tick);
				z3d::VectorLerp( v1, m_pPosKeys[i].pos, m_pPosKeys[i+1].pos, f );
				break;
			}
		}
	}

	// ���� ƽ �ٷ� ���� ƽ�� ���� ���� v2�� ����

	if( t_prev < m_pPosKeys[0].tick )
	{
		v2 = m_pPosKeys[0].pos;
	}
	else if( t_prev >= m_pPosKeys[m_sPosKeyCount-1].tick ) // require pos-keyframe past the end of the data
	{
		v2 = m_pPosKeys[m_sPosKeyCount-1].pos;
	}
	else // do interpolation for inbetween keyframe
	{
		for( i = 0; i < m_sPosKeyCount-1; i++ ) // last item was processed by above portion of code
		{
			if( m_pPosKeys[i].tick == t_prev )
			{
				v2 = m_pPosKeys[i].pos;
				break;
			}
			else if( m_pPosKeys[i].tick < t_prev && t_prev < m_pPosKeys[i+1].tick )
			{
				f = (float)(t_prev - m_pPosKeys[i].tick) /
					(float)(m_pPosKeys[i+1].tick - m_pPosKeys[i].tick);
				z3d::VectorLerp( v2, m_pPosKeys[i].pos, m_pPosKeys[i+1].pos, f );
				break;
			}
		}
	}

	// ����
	vDiff.x = v1.x - v2.x;
	vDiff.y = v1.y;// - v2.y; y���� *����ġ�� �ƴϴ�*
	vDiff.z = v1.z - v2.z;


	// ȸ��Ű ����(GetInterpol.. �� ����)
	if( t < m_pRotKeys[0].tick )
	{
		q = m_pRotKeys[0].rot;
	}
	else if( t >= m_pRotKeys[m_sRotKeyCount-1].tick ) // require rot-keyframe past the end of the data
	{
		q = m_pRotKeys[m_sRotKeyCount-1].rot;
	}
	else
	{
		for( i = 0; i < m_sRotKeyCount-1; i++ ) // last item was processed by above portion of code
		{
			if( m_pRotKeys[i].tick == t )
			{
				q = m_pRotKeys[i].rot;
				break;
			}
			else if( m_pRotKeys[i].tick < t && t < m_pRotKeys[i+1].tick )
			{
				f = (float)(t - m_pRotKeys[i].tick) /
					(float)(m_pRotKeys[i+1].tick - m_pRotKeys[i].tick);
				z3d::QuaternionSlerp( q, m_pRotKeys[i].rot, m_pRotKeys[i+1].rot, f );
				break;
			}
		}
	}
}
