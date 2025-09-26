// quaternion.h: interface for the quaternion class.
//
// Only for unit quaternions( norm(q) = 1 )
//
// result with non-unit quaternion by this routine is undefined
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUATERNION_H__B048AE81_B0A6_11D4_AD2B_0000E8EB4C69__INCLUDED_)
#define AFX_QUATERNION_H__B048AE81_B0A6_11D4_AD2B_0000E8EB4C69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MathBase.h"


struct quaternion  
{
	union
	{
		struct 
		{
			float x, y, z, w;
		};
		float v[4];
	};

	quaternion() {};
	quaternion( float fx, float fy, float fz, float fw )
	{
		x = fx;
		y = fy;
		z = fz;
		w = fw;
	}
	// initialize to identity quaternion
	void Identity()
	{
		x = y = z = 0.0f;
		w = 1.0f;
	}

	quaternion& operator*=(const quaternion& op);
	quaternion operator-()
	{
		return quaternion( -x, -y, -z, -w );
	}

	// perform quaternion rotation on vector
	//void RotateVector( vector3& vec );
	// perform axis-angle tpye rotation
	void Pitch( float a )
	{
		quaternion x( sinf(a/2), 0.0f, 0.0f, cosf(a/2) );
		(*this) *= x;
	}
	void Yaw( float a )
	{
		quaternion x( 0.0f, sinf(a/2), 0.0f, cosf(a/2) );
		(*this) *= x;
	}
	void Roll( float a )
	{
		quaternion x( 0.0f, 0.0f, sinf(a/2), cosf(a/2) );
		(*this) *= x;
	}
	
};

// 'this = op * this', not 'this = this * op'
// due to the relation between rotation matrix&quaternion
inline quaternion& quaternion::operator*=(const quaternion& op)
{
	float fx, fy,fz, fw;
	fw = op.w*w - op.x*x - op.y*y - op.z*z;
	fx = op.w*x + op.x*w + op.y*z - op.z*y;
	fy = op.w*y - op.x*z + op.y*w + op.z*x;
	fz = op.w*z + op.x*y - op.y*x + op.z*w;
	w = fw;
	x = fx;
	y = fy;
	z = fz;
	return *this;
}


// 'result = op2 * op1', not 'result = op1 * op2'
// due to the inverse-relation between rotation matrix&quaternion
inline void Multiply( quaternion& result, quaternion& op1, quaternion& op2 )
{
	result.w = op2.w*op1.w - op2.x*op1.x - op2.y*op1.y - op2.z*op1.z;
	result.x = op2.w*op1.x + op2.x*op1.w + op2.y*op1.z - op2.z*op1.y;
	result.y = op2.w*op1.y - op2.x*op1.z + op2.y*op1.w + op2.z*op1.x;
	result.z = op2.w*op1.z + op2.x*op1.y - op2.y*op1.x + op2.z*op1.w;
}


// 'result = op2 * op1', not 'result = op1 * op2'
// due to the inverse-relation between rotation matrix&quaternion
inline const quaternion operator*( quaternion& op1, quaternion& op2 )
{
	quaternion result;

	result.w = op2.w*op1.w - op2.x*op1.x - op2.y*op1.y - op2.z*op1.z;
	result.x = op2.w*op1.x + op2.x*op1.w + op2.y*op1.z - op2.z*op1.y;
	result.y = op2.w*op1.y - op2.x*op1.z + op2.y*op1.w + op2.z*op1.x;
	result.z = op2.w*op1.z + op2.x*op1.y - op2.y*op1.x + op2.z*op1.w;

	return result;
}



#endif // !defined(AFX_QUATERNION_H__B048AE81_B0A6_11D4_AD2B_0000E8EB4C69__INCLUDED_)
