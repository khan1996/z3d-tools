// Z3DMath.h: interface for the Z3DMath class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Z3DMATH_H__05758061_E110_11D4_AD2B_0000E8EB4C69__INCLUDED_)
#define AFX_Z3DMATH_H__05758061_E110_11D4_AD2B_0000E8EB4C69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <math.h>

#include "misc.h"

#include "vector.h"
#include "quaternion.h"
#include "matrix.h"

#include <d3d8.h>

// dummy.. 임시용임.
extern vector3 g_vecDummy;
extern quaternion g_quatDummy;

namespace z3d
{
	inline const vector3 MatrixGetLoc( const matrix& m )
	{
		return vector3(m._41, m._42, m._43);	
	}

	void MatrixTranslation(matrix& result_m, const vector3 &loc);
	void MatrixCameraLookAt(matrix& result_m, const vector3& loc, const vector3& lookAt, const vector3& inUp );
	void MatrixAxisAngle(matrix& result_m, const vector3& inaxis,float angle);

	void MatrixRotationQuaternion(matrix& result, const quaternion& q );
	void MatrixRotationQuaternion(D3DMATRIX& result, const quaternion& q );
	//void D3DMATRIXRotationQuaternion(D3DMATRIX& m, const quaternion& q );
	void VectorMultiplyMatrix( vector3& result_v, const vector3 &v, const matrix &m );

	inline void VectorLerp( vector3& result, vector3& from, vector3& to, float factor )
	{
		result.x = (to.x - from.x)*factor + from.x;
		result.y = (to.y - from.y)*factor + from.y;
		result.z = (to.z - from.z)*factor + from.z;
	}

	inline void MatrixToD3DMATRIX( D3DMATRIX& dest, const matrix& src )
	{
		memcpy( &(dest._11), &(src._11), sizeof(float) * 16 );
	}

	//
	// perform quaternion rotation on vector
	//
	inline void VectorRotate( vector3& result, const vector3& v, const quaternion& q )
	{
		float xx, yy, zz, xy, yz, zx, wx, wy, wz, vx, vy, vz;
		xx = q.x*q.x;
		yy = q.y*q.y;
		zz = q.z*q.z;
		xy = q.x*q.y;
		yz = q.z*q.y;
		zx = q.z*q.x;
		wx = q.w*q.x;
		wy = q.w*q.y;
		wz = q.w*q.z;
		vx = v.x;
		vy = v.y;
		vz = v.z;

		result.x = vx + 2*(-vx*(yy+zz) +  vy*(xy-wz) +  vz*(zx+wy) );
		result.y = vy + 2*( vx*(xy+wz) + -vy*(xx+zz) +  vz*(yz-wx) );
		result.z = vz + 2*( vx*(zx-wy) +  vy*(yz+wx) + -vz*(xx+yy) );
	}

	inline void QuaternionSlerp( quaternion& result, quaternion from, quaternion to, float factor )
	{
		if( from.w < 0 ) from = -from;
		if( to.w < 0 ) to = -to;

		float fCosTheta = from.x*to.x + from.y*to.y + from.z*to.z + from.w*to.w;

		//float fSign = 1.0f;
		if( fCosTheta < 0.0f )
		{
			fCosTheta = -fCosTheta;
			to = -to;
			//fSign = -1.0f;
		}

		float fInvFactor = 1.0f - factor;
	
		if( 1.0f - fCosTheta > 0.001f )
		{
			float fTheta = acosf(fCosTheta);

			factor = sinf( factor*fTheta ) / sinf(fTheta);
			fInvFactor = sinf( fInvFactor*fTheta ) / sinf(fTheta);
		}

		result.x = fInvFactor*from.x + factor*to.x;
		result.y = fInvFactor*from.y + factor*to.y;
		result.z = fInvFactor*from.z + factor*to.z;
		result.w = fInvFactor*from.w + factor*to.w;
	}

	inline void QuaternionRotationMatrix(quaternion& result_q, const matrix& m)
	{
		float f =  sqrtf(m._11 + m._22 + m._33 + m._44) * 2;
		result_q.x = (m._32 - m._23) / f;
		result_q.y = (m._13 - m._31) / f;
		result_q.z = (m._21 - m._12) / f;
		result_q.w = f / 4;
	}

	inline void QuaternionAxisAngle( quaternion& result_q,
		float x, float y, float z, float a )
	{
		a /= 2.0;
		float n = sqrtf(x*x + y*y + z*z);
		n = max( FLOAT_EPSILON, n );
		result_q.w = cosf(a);
		result_q.x = x*sinf(a)/n;
		result_q.y = y*sinf(a)/n;
		result_q.z = z*sinf(a)/n;
	}

	inline void VectorSetValue( vector3& v, float x, float y, float z )
	{
		v.x = x;
		v.y = y;
		v.z = z;
	}

	inline void QuaternionSetValue( quaternion& q, float x, float y, float z, float w )
	{
		q.x = x;
		q.y = y;
		q.z = z;
		q.w = w;
	}
}


#endif // !defined(AFX_Z3DMATH_H__05758061_E110_11D4_AD2B_0000E8EB4C69__INCLUDED_)
