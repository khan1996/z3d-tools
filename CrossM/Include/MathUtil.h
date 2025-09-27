#include "./MathConst.h"
#include "./Vector3.h"

#pragma once

namespace CrossM{
namespace Math{


inline bool IsEquivalent(const float f1, const float f2, const float fEpsilon = F_EPSILON)
{
	float d = f1 - f2;
	return (d < fEpsilon && d > -fEpsilon);
}


/*!
	점 v 가 boxMin, boxMax 를 최소/최대 점으로 가지는 AABB 에 포함되는지 여부를 체크

	포함될 경우 true, 그렇지 않을 경우 false 반환
*/
inline bool IsPointInAABB(const VECTOR3& vPoint, const VECTOR3& vAABBMin , const VECTOR3& vAABBMax)
{
	if (vAABBMin.x <= vPoint.x && vPoint.x < vAABBMax.x &&
		vAABBMin.y <= vPoint.y && vPoint.y < vAABBMax.y &&
		vAABBMin.z <= vPoint.z && vPoint.z < vAABBMax.z)
	{
		return true;
	}

	return false;
}

/* 
	f1Min ~ f1Max 구간과 f2Min ~ f2Max 구간이 겹치는지 체크
	단, 반드시 f1Min <= f1Max, f2Min <= f2Max 여야 한다
*/
inline bool IsRangeOverlap(float f1Min, float f1Max, float f2Min, float f2Max)
{
	if (f1Min <= f2Max && f2Min <= f1Max)
	{
		return true;
	}

	return false;
}

// 삼각형이 AABB 안에 완전히 포함되는지 여부를 확인
bool IsTriangleInAabb(const VECTOR3& vAabbMin, const VECTOR3& vAabbMax, const VECTOR3& vTri0, const VECTOR3& vTri1, const VECTOR3& vTri2);

// AABB 와 삼각형의 intersection 테스트
bool CheckAabbTriangleIntersection(const VECTOR3& vAabbMin, const VECTOR3& vAabbMax, const VECTOR3& vTri0, const VECTOR3& vTri1, const VECTOR3& vTri2);

// AABB 와 AABB 의 intersection 테스트
bool CheckAabbAabbIntersection(const VECTOR3& vAabb1Min, const VECTOR3& vAabb1Max, const VECTOR3& vAabb2Min, const VECTOR3& vAabb2Max);

// 삼각형/Sweeping Sphere 테스트를 위한 파라미터 구조체
struct TriangSweepingSphere
{
	Math::VECTOR3 m_vTri0;
	Math::VECTOR3 m_vTri1;
	Math::VECTOR3 m_vTri2;

	Math::VECTOR3 m_vSphereSweepStart;
	Math::VECTOR3 m_vSphereSweepPath;
	float m_fSphereRadius;
};

// 삼각형과 sweeping sphere 의 충돌 테스트. 충돌일 경우 충돌이 일어난 지점도 반환한다
bool CheckTriangleSweepingSphereCollision(float &fOutT, VECTOR3& vOutContactPoint, bool& bOutContactInsideTriangle, const TriangSweepingSphere& triAndSphere);

// 삼각형 내에 점이 포함되어있는지 확인
bool IsPointInTriangle(const VECTOR3& vPoint, const VECTOR3& vTri0, const VECTOR3& vTri1, const VECTOR3& vTri2);

}}



// 임시용 vector3 계열간 컨버전 함수
template<class _T>
void ConvVector3(CrossM::Math::VECTOR3& v, const _T& vOld)
{
	v.SetValue(vOld.x, vOld.y, vOld.z);
}