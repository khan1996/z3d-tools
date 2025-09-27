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
	�� v �� boxMin, boxMax �� �ּ�/�ִ� ������ ������ AABB �� ���ԵǴ��� ���θ� üũ

	���Ե� ��� true, �׷��� ���� ��� false ��ȯ
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
	f1Min ~ f1Max ������ f2Min ~ f2Max ������ ��ġ���� üũ
	��, �ݵ�� f1Min <= f1Max, f2Min <= f2Max ���� �Ѵ�
*/
inline bool IsRangeOverlap(float f1Min, float f1Max, float f2Min, float f2Max)
{
	if (f1Min <= f2Max && f2Min <= f1Max)
	{
		return true;
	}

	return false;
}

// �ﰢ���� AABB �ȿ� ������ ���ԵǴ��� ���θ� Ȯ��
bool IsTriangleInAabb(const VECTOR3& vAabbMin, const VECTOR3& vAabbMax, const VECTOR3& vTri0, const VECTOR3& vTri1, const VECTOR3& vTri2);

// AABB �� �ﰢ���� intersection �׽�Ʈ
bool CheckAabbTriangleIntersection(const VECTOR3& vAabbMin, const VECTOR3& vAabbMax, const VECTOR3& vTri0, const VECTOR3& vTri1, const VECTOR3& vTri2);

// AABB �� AABB �� intersection �׽�Ʈ
bool CheckAabbAabbIntersection(const VECTOR3& vAabb1Min, const VECTOR3& vAabb1Max, const VECTOR3& vAabb2Min, const VECTOR3& vAabb2Max);

// �ﰢ��/Sweeping Sphere �׽�Ʈ�� ���� �Ķ���� ����ü
struct TriangSweepingSphere
{
	Math::VECTOR3 m_vTri0;
	Math::VECTOR3 m_vTri1;
	Math::VECTOR3 m_vTri2;

	Math::VECTOR3 m_vSphereSweepStart;
	Math::VECTOR3 m_vSphereSweepPath;
	float m_fSphereRadius;
};

// �ﰢ���� sweeping sphere �� �浹 �׽�Ʈ. �浹�� ��� �浹�� �Ͼ ������ ��ȯ�Ѵ�
bool CheckTriangleSweepingSphereCollision(float &fOutT, VECTOR3& vOutContactPoint, bool& bOutContactInsideTriangle, const TriangSweepingSphere& triAndSphere);

// �ﰢ�� ���� ���� ���ԵǾ��ִ��� Ȯ��
bool IsPointInTriangle(const VECTOR3& vPoint, const VECTOR3& vTri0, const VECTOR3& vTri1, const VECTOR3& vTri2);

}}



// �ӽÿ� vector3 �迭�� ������ �Լ�
template<class _T>
void ConvVector3(CrossM::Math::VECTOR3& v, const _T& vOld)
{
	v.SetValue(vOld.x, vOld.y, vOld.z);
}