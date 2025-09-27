#pragma once

#include "./MathConst.h"

#include <math.h>

namespace CrossM{
namespace Math{

struct VECTOR3
{
	VECTOR3() {}
	~VECTOR3() {}
	
	VECTOR3(const float _x, const float _y, const float _z) : x(_x), y(_y), z(_z) {}

	VECTOR3(const VECTOR3& v) : x(v.x), y(v.y), z(v.z) {}

	void SetValue(const float _x, const float _y, const float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

    float x, y, z;
};

//////////////////////////////////////////////////////////////////////////

inline VECTOR3& SetZeroVector(VECTOR3& v)
{
	v.x = v.y = v.z = 0.0f;

	return v;
}

// ���Ϳ� ���� ����
inline VECTOR3& SetValue(VECTOR3& vOut, const float x, const float y, const float z)
{
	vOut.x = x;
	vOut.y = y;
	vOut.z = z;

	return vOut;
}

// ���� ���̸� ����
inline float GetLength(const VECTOR3& v)
{
	return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
}

// ���� ������ ������ ����
inline float GetSquaredLength(const VECTOR3& v)
{
	return v.x*v.x + v.y*v.y + v.z*v.z;
}

// Normalize
inline VECTOR3& Normalize(VECTOR3& vOut, const VECTOR3& vIn)
{
	float fLen = GetLength(vIn);
	if (fLen < F_EPSILON)
	{
		// ������ ���̰� 0�� ����� ��� �������ʹ� x����⺤��
		SetValue(vOut, 1.0f, 0.0f, 0.0f);
		return vOut;
	}

	float fInv = 1.0f / fLen;
	vOut.x = vIn.x * fInv;
	vOut.y = vIn.y * fInv;
	vOut.z = vIn.z * fInv;

	return vOut;
}

// vOut = -vIn;
inline VECTOR3& Negate(VECTOR3& vOut, const VECTOR3& vIn)
{
	vOut.x = -vIn.x;
	vOut.y = -vIn.y;
	vOut.z = -vIn.z;

	return vOut;
}

// vOut = fScale x vIn
inline VECTOR3& Scale(VECTOR3& vOut, const VECTOR3& vIn, const float fScale)
{
	vOut.x = vIn.x * fScale;
	vOut.y = vIn.y * fScale;
	vOut.z = vIn.z * fScale;

	return vOut;
}

// vOut = v1 + v2
inline VECTOR3& Add(VECTOR3& vOut, const VECTOR3& v1, const VECTOR3& v2)
{
	vOut.x = v1.x + v2.x;
	vOut.y = v1.y + v2.y;
	vOut.z = v1.z + v2.z;

	return vOut;
}

// vOut = v1 - v2
inline VECTOR3& Subtract(VECTOR3& vOut, const VECTOR3& v1, const VECTOR3& v2)
{
	vOut.x = v1.x - v2.x;
	vOut.y = v1.y - v2.y;
	vOut.z = v1.z - v2.z;

	return vOut;
}

// v1 �� v2 �� ����.
inline float DotProduct(const VECTOR3& v1, const VECTOR3& v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

// v1 �� v2 �� ����. vOut = v1 x v2
inline VECTOR3& CrossProduct(VECTOR3& vOut, const VECTOR3& v1, const VECTOR3& v2)
{
	VECTOR3 vTmp;	// vOut �� v1 �̳� v2 �� ���� ��� ���� ������ �����ϱ� ���� �ӽú���

	vTmp.x = v1.y*v2.z - v1.z*v2.y;
	vTmp.y = v1.z*v2.x - v1.x*v2.z;
	vTmp.z = v1.x*v2.y - v1.y*v2.x;

	vOut = vTmp;

	return vOut;
}

// v1 ���� v2 �� ���� f��ŭ �������� ��ȭ�� ���͸� ����(��������)
inline VECTOR3& Lerp(VECTOR3& vOut, const VECTOR3& v1, const VECTOR3& v2, const float f)
{
	float fRem = 1.0f - f;
	vOut.x = v1.x*fRem + v2.x*f;
	vOut.y = v1.y*fRem + v2.y*f;
	vOut.z = v1.z*fRem + v2.z*f;

	return vOut;
}


//////////////////////////////////////////////////////////////////////////

// ���� - ������
inline VECTOR3 operator - (const VECTOR3& v)
{
	VECTOR3 vRet;

	return Negate(vRet, v);
}

// ������
inline VECTOR3 operator + (const VECTOR3& v1, const VECTOR3& v2)
{
	VECTOR3 vRet;

	return Add(vRet, v1, v2);
}

// ������
inline VECTOR3 operator - (const VECTOR3& v1, const VECTOR3& v2)
{
	VECTOR3 vRet;

	return Subtract(vRet, v1, v2);
}

// ���� ��Į���(�������)
inline VECTOR3 operator * (const float f, const VECTOR3& v)
{
	VECTOR3 vRet;

	return Scale(vRet, v, f);
}

// ���� ��Į���(���͸���)
inline VECTOR3 operator * (const VECTOR3& v, const float f)
{
	VECTOR3 vRet;

	return Scale(vRet, v, f);
}

// ���� ��Į���(����� ����)
inline VECTOR3 operator / (const VECTOR3& v, const float& f)
{
	VECTOR3 vRet;

	return Scale(vRet, v, 1.0f/f);
}

// ���� ����
inline float operator * (const VECTOR3& v1, const VECTOR3& v2)
{
	return DotProduct(v1, v2);
}

// ���� ����
inline VECTOR3 operator ^ (const VECTOR3& v1, const VECTOR3& v2)
{
	VECTOR3 vRet;

	return CrossProduct(vRet, v1, v2);
}


}}