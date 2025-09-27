#include "../Include/MathUtil.h"

#include <algorithm>

namespace CrossM{
namespace Math{


// �ﰢ���� AABB �ȿ� ������ ���ԵǴ��� �˻�
bool IsTriangleInAabb(const VECTOR3& vAabbMin, const VECTOR3& vAabbMax, const VECTOR3& vTri0, const VECTOR3& vTri1, const VECTOR3& vTri2)
{
	if (vAabbMin.x <= vTri0.x && vTri0.x <= vAabbMax.x &&
		vAabbMin.y <= vTri0.y && vTri0.y <= vAabbMax.y &&
		vAabbMin.z <= vTri0.z && vTri0.z <= vAabbMax.z &&
		vAabbMin.x <= vTri1.x && vTri1.x <= vAabbMax.x &&
		vAabbMin.y <= vTri1.y && vTri1.y <= vAabbMax.y &&
		vAabbMin.z <= vTri1.z && vTri1.z <= vAabbMax.z &&
		vAabbMin.x <= vTri2.x && vTri2.x <= vAabbMax.x &&
		vAabbMin.y <= vTri2.y && vTri2.y <= vAabbMax.y &&
		vAabbMin.z <= vTri2.z && vTri2.z <= vAabbMax.z)
	{
		return true;
	}

	return false;
}

// �ﰢ���� AABB �� ���������� �˻�
bool CheckAabbTriangleIntersection(const VECTOR3& vAabbMin, const VECTOR3& vAabbMax, const VECTOR3& vTri0, const VECTOR3& vTri1, const VECTOR3& vTri2)
{
	// Separation of Axes �� ���� AABB - triangle intersection test ����

	// 6 �࿡ ���� AABB �� �ﰢ���� projection �� ��, ���� ��ġ���� ���θ� Ȯ���Ѵ�
	// ��� �࿡ ���� ��ġ�� ���� �����°��̰�, �� �࿡ ���ؼ��� ��ġ�� �ʴ´ٸ� ������ �ʴ� ���̴�

	float fBoxProjectionMin, fBoxProjectionMax;
	float fTriProjectionMin, fTriProjectionMax;

	// AABB �� �� �࿡ ���� projection �� �˻�
	
	// X ��
	fBoxProjectionMin = vAabbMin.x;
	fBoxProjectionMax = vAabbMax.x;
	fTriProjectionMin = std::min(vTri0.x, std::min(vTri1.x, vTri2.x));
	fTriProjectionMax = std::max(vTri0.x, std::max(vTri1.x, vTri2.x));
	if (false == IsRangeOverlap(fTriProjectionMin, fTriProjectionMax, fBoxProjectionMin, fBoxProjectionMax))
	{
		return false;
	}

	// Y ��
	fBoxProjectionMin = vAabbMin.y;
	fBoxProjectionMax = vAabbMax.y;
	fTriProjectionMin = std::min(vTri0.y, std::min(vTri1.y, vTri2.y));
	fTriProjectionMax = std::max(vTri0.y, std::max(vTri1.y, vTri2.y));
	if (false == IsRangeOverlap(fTriProjectionMin, fTriProjectionMax, fBoxProjectionMin, fBoxProjectionMax))
	{
		return false;
	}

	// Z ��
	fBoxProjectionMin = vAabbMin.z;
	fBoxProjectionMax = vAabbMax.z;
	fTriProjectionMin = std::min(vTri0.z, std::min(vTri1.z, vTri2.z));
	fTriProjectionMax = std::max(vTri0.z, std::max(vTri1.z, vTri2.z));
	if (false == IsRangeOverlap(fTriProjectionMin, fTriProjectionMax, fBoxProjectionMin, fBoxProjectionMax))
	{
		return false;
	}


	// �ﰢ���� �� �� �࿡ ���� projection �� �˻�
	VECTOR3 avAxis[3];		// �ﰢ���� �� �𼭸�
	Math::Subtract(avAxis[0], vTri1, vTri0);
	Math::Subtract(avAxis[1], vTri2, vTri0);
	Math::Subtract(avAxis[2], vTri2, vTri1);

	// �ﰢ�� �� ���� �࿡ projection �� ��
	float fProjectionTri0, fProjectionTri1, fProjectionTri2;

	for (int i = 0; i < 3; ++i)
	{
		VECTOR3& vAxis = avAxis[i];

		// AABB �� max/min ���� xyz �� ������ �ִ�/�ּҰ��̹Ƿ�,
		// axis �� �� ������Ʈ�� ���������� �ִ밪�� ���Ϸ���
		// x,y,z ������ ���� ū �� ���� ��� ���ϸ� �ȴ�.
		// (������ AABB �������� xyz ������ �ִ�/�ּҰ��� �����̹Ƿ�,
		// x,y,z �� ���� ���Ƿ� �ִ�/�ּҸ� �����Ѵ� �ص� ��h �� �ȿ� ��� ���ԵǴ� �����̴�.)

		fBoxProjectionMin =
			((vAxis.x > 0) ? vAabbMin.x : vAabbMax.x) * vAxis.x +
			((vAxis.y > 0) ? vAabbMin.y : vAabbMax.y) * vAxis.y +
			((vAxis.z > 0) ? vAabbMin.z : vAabbMax.z) * vAxis.z;

		fBoxProjectionMax =
			((vAxis.x > 0) ? vAabbMax.x : vAabbMin.x) * vAxis.x +
			((vAxis.y > 0) ? vAabbMax.y : vAabbMin.y) * vAxis.y +
			((vAxis.z > 0) ? vAabbMax.z : vAabbMin.z) * vAxis.z;

		fProjectionTri0 = Math::DotProduct(vTri0, vAxis);
		fProjectionTri1 = Math::DotProduct(vTri1, vAxis);
		fProjectionTri2 = Math::DotProduct(vTri2, vAxis);

		fTriProjectionMin = std::min(fProjectionTri0, std::min(fProjectionTri1, fProjectionTri2));
		fTriProjectionMax = std::max(fProjectionTri0, std::max(fProjectionTri1, fProjectionTri2));

		if (false == IsRangeOverlap(fTriProjectionMin, fTriProjectionMax, fBoxProjectionMin, fBoxProjectionMax))
		{
			return false;
		}
	}

    return true;
}

// �� AABB �� �������� �˻�
bool CheckAabbAabbIntersection(const VECTOR3& vAabb1Min, const VECTOR3& vAabb1Max, const VECTOR3& vAabb2Min, const VECTOR3& vAabb2Max)
{
	if (false ==IsRangeOverlap(vAabb1Min.x, vAabb1Max.x, vAabb2Min.x, vAabb2Max.x)) return false;
	if (false ==IsRangeOverlap(vAabb1Min.y, vAabb1Max.y, vAabb2Min.y, vAabb2Max.y)) return false;
	if (false ==IsRangeOverlap(vAabb1Min.z, vAabb1Max.z, vAabb2Min.z, vAabb2Max.z)) return false;

	return true;
}

// CheckTriangleSweepingSphereCollision �� �μ� �Լ�.
// ax^2 + bx + c �� �� �� min~max ���� ���� ���� ���� �ظ� ��ȯ�Ѵ�. �����ϴ� �ذ� ������ false ��ȯ
static bool GetLowestRootInRange(const float a, const float b, const float c, const float fMinRoot, const float fMaxRoot, float& fRoot)
{
	// �Ǽ��ذ� �����ϴ��� �Ǻ��� ���
	float fDeterminant = b*b - 4.0f*a*c;

	// �Ǽ��ذ� �������� ������ �� ����
	if (fDeterminant < 0.0f) return false;

	// �ΰ��� �ظ� ����Ѵ�
	float fSqrtD = sqrtf(fDeterminant);
	float r1 = (-b - fSqrtD) / (2*a);
	float r2 = (-b + fSqrtD) / (2*a);

	// r1 < r2 �� ũ�Ⱑ �ǵ��� ����
	if (r1 > r2) std::swap(r1, r2);

	// �����ʺ��� ���� ������ �Ǻ���, �������ϰ�� �طμ� ����
	if (r1 > fMinRoot && r1 < fMaxRoot)
	{
		fRoot = r1;
		return true;
	}

	// ���� �� ���� �������� ���� ������, ū �� ���� ���������� �˻��Ѵ�
	if (r2 > fMinRoot && r2 < fMaxRoot)
	{
		fRoot = r2;
		return true;
	}

	return false;
}


bool CheckTriangleSweepingSphereCollision(float &fOutT, VECTOR3& vOutContactPoint, bool& bOutContactInsideTriangle, const TriangSweepingSphere& triAndSphere)
{
	// �ణ�� alias
	const Math::VECTOR3 &vBasePoint = triAndSphere.m_vSphereSweepStart;
	const Math::VECTOR3 &vTri0 = triAndSphere.m_vTri0;
	const Math::VECTOR3 &vTri1 = triAndSphere.m_vTri1;
	const Math::VECTOR3 &vTri2 = triAndSphere.m_vTri2;
	const Math::VECTOR3 &vPath = triAndSphere.m_vSphereSweepPath;

	// �ﰢ���� �� �𼭸�
	Math::VECTOR3 vTriEdge01, vTriEdge02, vTriEdge12;

	// �ﰢ���� �����ϴ� ���. ���� plane Ŭ���� ������ ������ �� ��
	Math::VECTOR3 vTriPlaneNormal;
	float fTriPlaneConstant;
	
	// �ﰢ���� �� �𼭸� ���͸� ���ϰ�..
	Math::Subtract(vTriEdge01, vTri1, vTri0);
	Math::Subtract(vTriEdge02, vTri2, vTri0);
	Math::Subtract(vTriEdge12, vTri2, vTri1);

	// �ﰢ���� ���Ե� ����� �Ķ���͵��� ����
	Math::CrossProduct(vTriPlaneNormal, vTriEdge01, vTriEdge02);
	Math::Normalize(vTriPlaneNormal, vTriPlaneNormal);
	fTriPlaneConstant = -Math::DotProduct(vTriPlaneNormal, vTri0);

	// sweeping path �� �浹������ ����
	float fNormalDotPath = Math::DotProduct(vTriPlaneNormal, vPath);

	// sphere �� ��������� ���� �����̸� üũ����
	if (fNormalDotPath > 0.0f)
	{
		return false;
	}

	float t0, t1;
	bool bEmbededInPlane = false;

	float fSignedDistBaseToTriPlane = Math::DotProduct(vTriPlaneNormal, vBasePoint) + fTriPlaneConstant;

	if (0.0f == fNormalDotPath)
	{
		// sphere �� �ﰢ����� �����ϰ� ����

		if (fabs(fSignedDistBaseToTriPlane) >= triAndSphere.m_fSphereRadius)
		{
			return false;	// �ﰢ���鿡�� �ָ� �������� �����̵���
		}
		else
		{
			bEmbededInPlane = true;
			t0 = 0.0f;
			t1 = 1.0f;
		}
	}
	else
	{
		t0 = (-triAndSphere.m_fSphereRadius-fSignedDistBaseToTriPlane)/fNormalDotPath;
		t1 = (triAndSphere.m_fSphereRadius-fSignedDistBaseToTriPlane)/fNormalDotPath;

		// t0 < t1 ���� ��Ʈ
        if (t0 > t1)
		{
			std::swap(t0, t1);
		}

		// t ������ sphere �̵���� ���� ���� ���� �ʴ�
		if (t0 > 1.0f || t1 <0.0f)
		{
			return false;
		}

		// t ���� 0~1 �������� Ŭ����
		t0 = std::max(t0, 0.0f);
		t1 = std::min(t1, 1.0f);
	}

	VECTOR3 vContactPoint;
	bool bFoundCollision = false;
	float t = 1.0f;

    // �ﰢ�� ���ο� üũ
	if (!bEmbededInPlane)
	{
		vContactPoint = vBasePoint + (vPath*t0) - vTriPlaneNormal;

		if (IsPointInTriangle(vContactPoint, vTri0, vTri1, vTri2))
		{
			bFoundCollision = true;
			t = t0;
		}
	}

	// ���� �浹���� ã�� ���ߴٸ� �𼭸��� �������� ���� �׽�Ʈ�� �ؾ��Ѵ�
	if (!bFoundCollision)
	{
		float fSQuaredPathLength = GetSquaredLength(vPath);
        float a, b, c;
		float newT;
		float fSquaredRadius = triAndSphere.m_fSphereRadius*triAndSphere.m_fSphereRadius;

		a = fSQuaredPathLength;

		// vTri0
		b = 2.0f * (DotProduct(vPath, vBasePoint - vTri0));
		c = (Math::GetSquaredLength(vTri0 - vBasePoint) - fSquaredRadius);
		if (GetLowestRootInRange(a, b, c, 0.0f, t, newT))
		{
			t = newT;
			bFoundCollision = true;
            vContactPoint = vTri0;
		}

		// vTri1
		if (bFoundCollision)
		{
			b = 2.0f * (DotProduct(vPath, vBasePoint - vTri1));
			c = (Math::GetSquaredLength(vTri1 - vBasePoint) - fSquaredRadius);
			if (GetLowestRootInRange(a, b, c, 0.0f, t, newT))
			{
				t = newT;
				bFoundCollision = true;
				vContactPoint = vTri1;
			}
		}

		// vTri2
		if (bFoundCollision)
		{
			b = 2.0f * (DotProduct(vPath, vBasePoint - vTri2));
			c = (Math::GetSquaredLength(vTri2 - vBasePoint) - fSquaredRadius);
			if (GetLowestRootInRange(a, b, c, 0.0f, t, newT))
			{
				t = newT;
				bFoundCollision = true;
				vContactPoint = vTri0;
			}
		}

		// �𼭸��� ���� �׽�Ʈ
		VECTOR3 vBaseToVertex;
		float fEdgeSquaredLength;
		float fEdgeDotPath;
		float fEdgeDotBaseToVertex;

		// vTri0 ~ vTri1
		vBaseToVertex = vTri0 - vBasePoint;
        fEdgeSquaredLength = GetSquaredLength(vTriEdge01);
		fEdgeDotPath = DotProduct(vTriEdge01, vPath);
		fEdgeDotBaseToVertex = DotProduct(vTriEdge01, vBaseToVertex);

		a = fEdgeSquaredLength* -fSQuaredPathLength + 
			fEdgeDotPath*fEdgeDotPath;
		b = fEdgeSquaredLength* (2.0f*Math::DotProduct(vPath, vBaseToVertex)) -
			2.0f*fEdgeDotPath*fEdgeDotBaseToVertex;
		c = (fEdgeSquaredLength* (1.0f - Math::GetSquaredLength(vBaseToVertex)) +
			fEdgeDotBaseToVertex*fEdgeDotBaseToVertex);

		if (GetLowestRootInRange(a, b, c, 0.0f, t, newT))
		{
			float f = (fEdgeDotPath*t - fEdgeDotBaseToVertex) / fEdgeSquaredLength;
			if (f >= 0.0f && f <= 1.0f)
			{
				t = newT;
				bFoundCollision = true;
				vContactPoint = vTri0 + vTriEdge01*f;
			}
		}

		// vTri0 ~ vTri2
		vBaseToVertex = vTri0 - vBasePoint;
		fEdgeSquaredLength = GetSquaredLength(vTriEdge02);
		fEdgeDotPath = DotProduct(vTriEdge02, vPath);
		fEdgeDotBaseToVertex = DotProduct(vTriEdge02, vBaseToVertex);

		a = fEdgeSquaredLength* -fSQuaredPathLength + 
			fEdgeDotPath*fEdgeDotPath;
		b = fEdgeSquaredLength* (2.0f*Math::DotProduct(vPath, vBaseToVertex)) -
			2.0f*fEdgeDotPath*fEdgeDotBaseToVertex;
		c = (fEdgeSquaredLength* (1.0f - Math::GetSquaredLength(vBaseToVertex)) +
			fEdgeDotBaseToVertex*fEdgeDotBaseToVertex);

		if (GetLowestRootInRange(a, b, c, 0.0f, t, newT))
		{
			float f = (fEdgeDotPath*t - fEdgeDotBaseToVertex) / fEdgeSquaredLength;
			if (f >= 0.0f && f <= 1.0f)
			{
				t = newT;
				bFoundCollision = true;
				vContactPoint = vTri0 + vTriEdge02*f;
			}
		}


		// vTri1 ~ vTri2
		vBaseToVertex = vTri1 - vBasePoint;
		fEdgeSquaredLength = GetSquaredLength(vTriEdge12);
		fEdgeDotPath = DotProduct(vTriEdge12, vPath);
		fEdgeDotBaseToVertex = DotProduct(vTriEdge12, vBaseToVertex);

		a = fEdgeSquaredLength* -fSQuaredPathLength + 
			fEdgeDotPath*fEdgeDotPath;
		b = fEdgeSquaredLength* (2.0f*Math::DotProduct(vPath, vBaseToVertex)) -
			2.0f*fEdgeDotPath*fEdgeDotBaseToVertex;
		c = (fEdgeSquaredLength* (1.0f - Math::GetSquaredLength(vBaseToVertex)) +
			fEdgeDotBaseToVertex*fEdgeDotBaseToVertex);

		if (GetLowestRootInRange(a, b, c, 0.0f, t, newT))
		{
			float f = (fEdgeDotPath*t - fEdgeDotBaseToVertex) / fEdgeSquaredLength;
			if (f >= 0.0f && f <= 1.0f)
			{
				t = newT;
				bFoundCollision = true;
				vContactPoint = vTri1 + vTriEdge12*f;
			}
		}
	}

	if (bFoundCollision)
	{
		fOutT = t;
		vOutContactPoint = vContactPoint;
		return true;
	}

	return false;
}


// ������ ���� �ﰢ�� ���� ���ԵǴ��� Ȯ���ϴ� �ڵ�
// Fauerby �� �ۿ� Keidy ��� ����� ����� ���� ���� ��ƾ�̶�� �Ѵ�
// �������� ������.. �𸣰ڴ�-_- �ϴ� �̿�
bool IsPointInTriangle(const VECTOR3& p, const VECTOR3& vTri0, const VECTOR3& vTri1, const VECTOR3& vTri2)
{
	// ������ �� �� �ﰢ�� �𼭸�, ������ �������� �� p �� ��ġ����
	VECTOR3 vEdge1, vEdge2, pTri;

	Math::Subtract(vEdge1, vTri1, vTri0);
	Math::Subtract(vEdge2, vTri2, vTri0);
	Math::Subtract(pTri, p, vTri0);

	float a = Math::DotProduct(vEdge1, vEdge1);
	float b = Math::DotProduct(vEdge1, vEdge2);
	float c = Math::DotProduct(vEdge2, vEdge2);
	float d = Math::DotProduct(pTri, vEdge1);
	float e = Math::DotProduct(pTri, vEdge2);

	float x = d*c - e*b;
	float y = e*a - d*b;
	float z = x + y - (a*c - b*b);

	return ( ((unsigned int&)z) & ~ ( ((unsigned int&)x) | ((unsigned int&)y) ) & 0x80000000) != 0 ? true : false;
}


}}
