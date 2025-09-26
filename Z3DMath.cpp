// Z3DMath.cpp: implementation of the Z3DMath class.
//
//////////////////////////////////////////////////////////////////////

#include "Z3DMath.h"
#include "GMMemory.h"


//////////////////////////////////////////////////////////////////////

// 별 의미없이 자리채움용 더미.. 이거 구조 좀 뜯어고쳐야될텐데-_-a
vector3 g_vecDummy;
quaternion g_quatDummy;

//////////////////////////////////////////////////////////////////////



namespace z3d
{
	// build rotation matrix
	void MatrixRotationQuaternion( matrix& result, const quaternion& q )
	{
		float xx, yy, zz, xy, yz, zx, wx, wy, wz;
		xx = q.x*q.x;
		yy = q.y*q.y;
		zz = q.z*q.z;
		xy = q.x*q.y;
		yz = q.z*q.y;
		zx = q.z*q.x;
		wx = q.w*q.x;
		wy = q.w*q.y;
		wz = q.w*q.z;

		result._11 = 1 - 2*(yy + zz);
		result._12 =     2*(xy + wz);
		result._13 =     2*(zx - wy);

		result._21 =     2*(xy - wz);
		result._22 = 1 - 2*(xx + zz);
		result._23 =     2*(yz + wx);

		result._31 =     2*(zx + wy);
		result._32 =     2*(yz - wx);
		result._33 = 1 - 2*(xx + yy);

		result._14 = result._24 = result._34 = result._41 = result._42 = result._43 = 0.0f;
		result._44 = 1.0f;
	}

	// build rotation D3DMATRIX
	void MatrixRotationQuaternion( D3DMATRIX& result, const quaternion& q )
	{
		float xx, yy, zz, xy, yz, zx, wx, wy, wz;
		xx = q.x*q.x;
		yy = q.y*q.y;
		zz = q.z*q.z;
		xy = q.x*q.y;
		yz = q.z*q.y;
		zx = q.z*q.x;
		wx = q.w*q.x;
		wy = q.w*q.y;
		wz = q.w*q.z;

		result._11 = 1 - 2*(yy + zz);
		result._12 =     2*(xy + wz);
		result._13 =     2*(zx - wy);

		result._21 =     2*(xy - wz);
		result._22 = 1 - 2*(xx + zz);
		result._23 =     2*(yz + wx);

		result._31 =     2*(zx + wy);
		result._32 =     2*(yz - wx);
		result._33 = 1 - 2*(xx + yy);

		result._14 = result._24 = result._34 = result._41 = result._42 = result._43 = 0.0f;
		result._44 = 1.0f;
	}


	void MatrixAxisAngle( matrix& result_m, const vector3& inaxis, float angle)
	{
		vector3 axis = inaxis.Normalized();
		float s=sinf(angle);
		float c=cosf(angle);
		float x=axis.x,y=axis.y,z=axis.z;	

		result_m._11 = x*x*(1-c)+c;
		result_m._21 = x*y*(1-c)-(z*s);
		result_m._31 = x*z*(1-c)+(y*s);
		result_m._41 = 0;
		result_m._12 = y*x*(1-c)+(z*s);
		result_m._22 = y*y*(1-c)+c;
		result_m._32 = y*z*(1-c)-(x*s);
		result_m._42 = 0;
		result_m._13 = z*x*(1-c)-(y*s);
		result_m._23 = z*y*(1-c)+(x*s);
		result_m._33 = z*z*(1-c)+c;
		result_m._43 = 0;
		result_m._14 = 0;
		result_m._24 = 0;
		result_m._34 = 0;
		result_m._44 = 1;
	}

	void MatrixCameraLookAt( matrix& result_m, const vector3 &loc, const vector3 &lookAt, const vector3 &inUp)
	{
		vector3 viewVec=lookAt-loc;
		float mag=viewVec.Mag();
		viewVec/=mag;

		float fDot=inUp*viewVec;

		vector3 upVec=inUp-fDot*viewVec;

		upVec.Normalize();

		vector3 rightVec=upVec^viewVec;

		result_m._11 = rightVec.x; result_m._12 = upVec.x; result_m._13 = viewVec.x;
	    result_m._21 = rightVec.y; result_m._22 = upVec.y; result_m._23 = viewVec.y;
		result_m._31 = rightVec.z; result_m._32 = upVec.z; result_m._33 = viewVec.z; 
    
	    result_m._41 = - (loc * rightVec);
	    result_m._42 = - (loc * upVec);
		result_m._43 = - (loc * viewVec);

	    result_m._14 = 0;
	    result_m._24 = 0;
	    result_m._34 = 0;
	    result_m._44 = 1;
	}

	void MatrixTranslation( matrix& result_m, const vector3 &loc)
	{
		result_m.MakeIdent();

		result_m._41=loc.x;
		result_m._42=loc.y;
		result_m._43=loc.z;
	}


	void VectorMultiplyMatrix( vector3& result_v, const vector3 &v, const matrix &m )
	// call-by-value of parameter v is intended
	{
		result_v.x = v.x*m._11 + v.y*m._21 + v.z*m._31 + m._41;
		result_v.y = v.x*m._12 + v.y*m._22 + v.z*m._32 + m._42;
		result_v.z = v.x*m._13 + v.y*m._23 + v.z*m._33 + m._43;
	}
}
