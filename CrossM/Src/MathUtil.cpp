#include "../Include/MathUtil.h"

#include <algorithm>

namespace CrossM{
namespace Math{


// 삼각형이 AABB 안에 완전히 포함되는지 검사
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

// 삼각형과 AABB 가 만나는지를 검사
bool CheckAabbTriangleIntersection(const VECTOR3& vAabbMin, const VECTOR3& vAabbMax, const VECTOR3& vTri0, const VECTOR3& vTri1, const VECTOR3& vTri2)
{
	// Separation of Axes 에 따른 AABB - triangle intersection test 구현

	// 6 축에 대해 AABB 와 삼각형을 projection 한 뒤, 둘이 겹치는지 여부를 확인한다
	// 모든 축에 대해 겹치면 둘은 만나는것이고, 한 축에 대해서라도 겹치지 않는다면 만나지 않는 것이다

	float fBoxProjectionMin, fBoxProjectionMax;
	float fTriProjectionMin, fTriProjectionMax;

	// AABB 의 세 축에 대한 projection 을 검사
	
	// X 축
	fBoxProjectionMin = vAabbMin.x;
	fBoxProjectionMax = vAabbMax.x;
	fTriProjectionMin = std::min(vTri0.x, std::min(vTri1.x, vTri2.x));
	fTriProjectionMax = std::max(vTri0.x, std::max(vTri1.x, vTri2.x));
	if (false == IsRangeOverlap(fTriProjectionMin, fTriProjectionMax, fBoxProjectionMin, fBoxProjectionMax))
	{
		return false;
	}

	// Y 축
	fBoxProjectionMin = vAabbMin.y;
	fBoxProjectionMax = vAabbMax.y;
	fTriProjectionMin = std::min(vTri0.y, std::min(vTri1.y, vTri2.y));
	fTriProjectionMax = std::max(vTri0.y, std::max(vTri1.y, vTri2.y));
	if (false == IsRangeOverlap(fTriProjectionMin, fTriProjectionMax, fBoxProjectionMin, fBoxProjectionMax))
	{
		return false;
	}

	// Z 축
	fBoxProjectionMin = vAabbMin.z;
	fBoxProjectionMax = vAabbMax.z;
	fTriProjectionMin = std::min(vTri0.z, std::min(vTri1.z, vTri2.z));
	fTriProjectionMax = std::max(vTri0.z, std::max(vTri1.z, vTri2.z));
	if (false == IsRangeOverlap(fTriProjectionMin, fTriProjectionMax, fBoxProjectionMin, fBoxProjectionMax))
	{
		return false;
	}


	// 삼각형의 의 세 축에 대한 projection 을 검사
	VECTOR3 avAxis[3];		// 삼각형의 세 모서리
	Math::Subtract(avAxis[0], vTri1, vTri0);
	Math::Subtract(avAxis[1], vTri2, vTri0);
	Math::Subtract(avAxis[2], vTri2, vTri1);

	// 삼각형 각 점을 축에 projection 한 값
	float fProjectionTri0, fProjectionTri1, fProjectionTri2;

	for (int i = 0; i < 3; ++i)
	{
		VECTOR3& vAxis = avAxis[i];

		// AABB 의 max/min 점의 xyz 는 각각의 최대/최소값이므로,
		// axis 의 각 컴포넌트와 내적했을때 최대값을 구하려면
		// x,y,z 각각의 곱이 큰 쪽 끼리 골라서 더하면 된다.
		// (어차피 AABB 여덟점이 xyz 각각의 최대/최소간의 조합이므로,
		// x,y,z 를 각각 임의로 최대/최소를 선택한다 해도 어뎗 점 안에 모두 포함되는 조합이다.)

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

// 두 AABB 가 만나는지 검사
bool CheckAabbAabbIntersection(const VECTOR3& vAabb1Min, const VECTOR3& vAabb1Max, const VECTOR3& vAabb2Min, const VECTOR3& vAabb2Max)
{
	if (false ==IsRangeOverlap(vAabb1Min.x, vAabb1Max.x, vAabb2Min.x, vAabb2Max.x)) return false;
	if (false ==IsRangeOverlap(vAabb1Min.y, vAabb1Max.y, vAabb2Min.y, vAabb2Max.y)) return false;
	if (false ==IsRangeOverlap(vAabb1Min.z, vAabb1Max.z, vAabb2Min.z, vAabb2Max.z)) return false;

	return true;
}

// CheckTriangleSweepingSphereCollision 의 부속 함수.
// ax^2 + bx + c 의 해 중 min~max 범위 내의 가장 작은 해를 반환한다. 만족하는 해가 없으면 false 반환
static bool GetLowestRootInRange(const float a, const float b, const float c, const float fMinRoot, const float fMaxRoot, float& fRoot)
{
	// 실수해가 존재하는지 판별식 계산
	float fDeterminant = b*b - 4.0f*a*c;

	// 실수해가 존재하지 않으면 해 없음
	if (fDeterminant < 0.0f) return false;

	// 두개의 해를 계산한다
	float fSqrtD = sqrtf(fDeterminant);
	float r1 = (-b - fSqrtD) / (2*a);
	float r2 = (-b + fSqrtD) / (2*a);

	// r1 < r2 의 크기가 되도록 정렬
	if (r1 > r2) std::swap(r1, r2);

	// 작은쪽부터 범위 내인지 판별해, 범위내일경우 해로서 리턴
	if (r1 > fMinRoot && r1 < fMaxRoot)
	{
		fRoot = r1;
		return true;
	}

	// 작은 해 쪽이 범위내에 들지 않으면, 큰 해 쪽이 범위내인지 검사한다
	if (r2 > fMinRoot && r2 < fMaxRoot)
	{
		fRoot = r2;
		return true;
	}

	return false;
}


bool CheckTriangleSweepingSphereCollision(float &fOutT, VECTOR3& vOutContactPoint, bool& bOutContactInsideTriangle, const TriangSweepingSphere& triAndSphere)
{
	// 약간의 alias
	const Math::VECTOR3 &vBasePoint = triAndSphere.m_vSphereSweepStart;
	const Math::VECTOR3 &vTri0 = triAndSphere.m_vTri0;
	const Math::VECTOR3 &vTri1 = triAndSphere.m_vTri1;
	const Math::VECTOR3 &vTri2 = triAndSphere.m_vTri2;
	const Math::VECTOR3 &vPath = triAndSphere.m_vSphereSweepPath;

	// 삼각형의 세 모서리
	Math::VECTOR3 vTriEdge01, vTriEdge02, vTriEdge12;

	// 삼각형을 포함하는 평면. 추후 plane 클래스 등으로 만들어야 할 듯
	Math::VECTOR3 vTriPlaneNormal;
	float fTriPlaneConstant;
	
	// 삼각형의 세 모서리 벡터를 구하고..
	Math::Subtract(vTriEdge01, vTri1, vTri0);
	Math::Subtract(vTriEdge02, vTri2, vTri0);
	Math::Subtract(vTriEdge12, vTri2, vTri1);

	// 삼각형이 포함된 평면의 파라미터들을 구함
	Math::CrossProduct(vTriPlaneNormal, vTriEdge01, vTriEdge02);
	Math::Normalize(vTriPlaneNormal, vTriPlaneNormal);
	fTriPlaneConstant = -Math::DotProduct(vTriPlaneNormal, vTri0);

	// sweeping path 와 충돌평면법선 내적
	float fNormalDotPath = Math::DotProduct(vTriPlaneNormal, vPath);

	// sphere 의 진행방향이 같은 방향이면 체크안함
	if (fNormalDotPath > 0.0f)
	{
		return false;
	}

	float t0, t1;
	bool bEmbededInPlane = false;

	float fSignedDistBaseToTriPlane = Math::DotProduct(vTriPlaneNormal, vBasePoint) + fTriPlaneConstant;

	if (0.0f == fNormalDotPath)
	{
		// sphere 가 삼각형면과 평행하게 진행

		if (fabs(fSignedDistBaseToTriPlane) >= triAndSphere.m_fSphereRadius)
		{
			return false;	// 삼각형면에서 멀리 떨어져서 평행이동중
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

		// t0 < t1 으로 소트
        if (t0 > t1)
		{
			std::swap(t0, t1);
		}

		// t 구간이 sphere 이동경로 선분 내에 있지 않다
		if (t0 > 1.0f || t1 <0.0f)
		{
			return false;
		}

		// t 값을 0~1 구간내로 클램핑
		t0 = std::max(t0, 0.0f);
		t1 = std::min(t1, 1.0f);
	}

	VECTOR3 vContactPoint;
	bool bFoundCollision = false;
	float t = 1.0f;

    // 삼각형 내부와 체크
	if (!bEmbededInPlane)
	{
		vContactPoint = vBasePoint + (vPath*t0) - vTriPlaneNormal;

		if (IsPointInTriangle(vContactPoint, vTri0, vTri1, vTri2))
		{
			bFoundCollision = true;
			t = t0;
		}
	}

	// 아직 충돌점을 찾지 못했다면 모서리와 꼭지점에 대해 테스트를 해야한다
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

		// 모서리에 대해 테스트
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


// 지정된 점이 삼각형 내에 포함되는지 확인하는 코드
// Fauerby 의 글에 Keidy 라는 사람이 기고한 가장 빠른 루틴이라고 한다
// 수학적인 증명은.. 모르겠다-_- 일단 이용
bool IsPointInTriangle(const VECTOR3& p, const VECTOR3& vTri0, const VECTOR3& vTri1, const VECTOR3& vTri2)
{
	// 원점을 낀 두 삼각형 모서리, 원점을 기준으로 한 p 의 위치벡터
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
