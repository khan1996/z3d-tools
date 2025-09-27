#include "../Include/OctreeCollider.h"
#include "../Include/MathUtil.h"

#include <algorithm>



#include <windows.h>
#include <d3d8.h>
#include <d3dx8.h>


namespace CrossM{
namespace Scene{



COctreeCollisionNode::COctreeCollisionNode()
{
	for (int i = 0; i < 8; ++i)
	{
		m_apSubNode[i] = NULL;
	}

	m_vBoundingMin.SetValue(0,0,0);
	m_vBoundingMax.SetValue(0,0,0);
}

COctreeCollisionNode::~COctreeCollisionNode()
{
	ReleaseSubNode();
}

void COctreeCollisionNode::ReleaseSubNode()
{
	for (int i = 0; i < 8; ++i)
	{
		if (NULL != m_apSubNode[i])
		{
			delete m_apSubNode[i];
			m_apSubNode[i] = NULL;
		}
	}
}

// 자식노드가 전혀 없는 것이 리프 노드
bool COctreeCollisionNode::IsLeafNode()
{
	for (int i = 0; i < 8; ++i)
	{
		if (NULL != m_apSubNode[i])
		{
			return false;
		}
	}
			
	return true;
}


void COctreeCollisionNode::BuildSubNode(
	const std::vector< CollisionTriangleInfo >& vecTriangle,
	const size_t nMaximumRecursion, const size_t nMinPolyCount,
	size_t nCurrentRecursionLevel)
{
static size_t nProcessedTri = 0;

	unsigned int i, j;

	// recursion level 이 너무 깊거나, 포함된 face 갯수가 정해진것보다 작으면
	// 더이상 sub node 를 나누지 않는다 (재귀 종료 조건)
	if (nCurrentRecursionLevel >= nMaximumRecursion ||
		m_vecTriangleIndex.size() <= nMinPolyCount)
	{
nProcessedTri += m_vecTriangleIndex.size();
printf("\r%d / %d", nProcessedTri, vecTriangle.size());
		return;
	}

	// 자식 노드들의 bounding box min/max
	Math::VECTOR3 aSubNodeBoundingMin[8];
	Math::VECTOR3 aSubNodeBoundingMax[8];

	Math::VECTOR3 vMedian = (m_vBoundingMin + m_vBoundingMax)/2.0f;

	// 바운딩 박스 값 설정
	for (i = 0; i < 8; ++i)
	{
		if (0 == (i & 1))
		{
			aSubNodeBoundingMin[i].x = m_vBoundingMin.x;
			aSubNodeBoundingMax[i].x = vMedian.x;
		}
		else
		{
			aSubNodeBoundingMin[i].x = vMedian.x;
			aSubNodeBoundingMax[i].x = m_vBoundingMax.x;
		}
		if (0 == (i & 2))
		{
			aSubNodeBoundingMin[i].y = m_vBoundingMin.y;
			aSubNodeBoundingMax[i].y = vMedian.y;
		}
		else
		{
			aSubNodeBoundingMin[i].y = vMedian.y;
			aSubNodeBoundingMax[i].y = m_vBoundingMax.y;
		}
		if (0 == (i & 4))
		{
			aSubNodeBoundingMin[i].z = m_vBoundingMin.z;
			aSubNodeBoundingMax[i].z = vMedian.z;
		}
		else
		{
			aSubNodeBoundingMin[i].z = vMedian.z;
			aSubNodeBoundingMax[i].z = m_vBoundingMax.z;
		}
	}

	// 넘겨받은 삼각형 인덱스에서 하부 노드로 내려갈것들과 이 노드에 저장될것들을 구분해낸다
	for (j = 0; j < m_vecTriangleIndex.size(); ++j)
	{
		// 삼각형 세 점 얻기
		const CollisionTriangleInfo &tri = vecTriangle[ m_vecTriangleIndex[j] ];
		const Math::VECTOR3 &vTri0 = tri.m_avVertex[0];
		const Math::VECTOR3 &vTri1 = tri.m_avVertex[1];
		const Math::VECTOR3 &vTri2 = tri.m_avVertex[2];

		// 각 자식노드들에 대해 삼각형이 포함되는지 체크한다
		bool bIncludedInSubNode = false;	// 자식 노드에 삼각형이 포함되는지 여부를 나타내는 플래그
		for (i = 0; i < 8; ++i)
		{
			if (Math::IsTriangleInAabb(aSubNodeBoundingMin[i], aSubNodeBoundingMax[i], vTri0, vTri1, vTri2))
			{
				// 노드가 할당되지 않았으면 할당
				COctreeCollisionNode* &pNode = m_apSubNode[i];
				if (NULL == pNode)
				{
					pNode = new COctreeCollisionNode;
					pNode->m_vBoundingMin = aSubNodeBoundingMin[i];
					pNode->m_vBoundingMax = aSubNodeBoundingMax[i];
				}

				// 자식노드에 넘겨줄 삼각형 인덱스에 저장
				pNode->m_vecTriangleIndex.push_back(m_vecTriangleIndex[j]);
				bIncludedInSubNode = true;
				break;
			}
		}
		if (!bIncludedInSubNode)
		{
			// 한 노드에 완전히 포함되는것이 아니라 다수 노드간에 걸치는 삼각형이므로,
			// triangle - AABB intersection 테스트를 한다
			for (i = 0; i < 8; ++i)
			{
				if (Math::CheckAabbTriangleIntersection(aSubNodeBoundingMin[i], aSubNodeBoundingMax[i], vTri0, vTri1, vTri2))
				{
					// 노드가 할당되지 않았으면 할당
					COctreeCollisionNode* &pNode = m_apSubNode[i];
					if (NULL == pNode)
					{
						pNode = new COctreeCollisionNode;
						pNode->m_vBoundingMin = aSubNodeBoundingMin[i];
						pNode->m_vBoundingMax = aSubNodeBoundingMax[i];
					}

					// 자식노드에 넘겨줄 삼각형 인덱스에 저장
					pNode->m_vecTriangleIndex.push_back(m_vecTriangleIndex[j]);
				}
			}
		}
	}
	
	// 삼각형 인덱스를 자식 노드에 모두 넘겨주었으므로 더 이상 인덱스를 들고 있을 필요가 없다
	m_vecTriangleIndex.clear();

	// 삼각형이 있는 자식 노드는 BuildSubNode 를 호출한다
	for (i = 0; i < 8; ++i)
	{
		if (NULL != m_apSubNode[i])
		{
			m_apSubNode[i]->BuildSubNode(vecTriangle, nMaximumRecursion, nMinPolyCount, nCurrentRecursionLevel+1);
		}
	}
}


void COctreeCollisionNode::CollectCollidableNodes(
							const Math::VECTOR3& vSweptVolumeMin, const Math::VECTOR3& vSweptVolumeMax,
							std::vector< COctreeCollisionNode* >& vecCollidableNode)
{
	if(!Math::CheckAabbAabbIntersection(vSweptVolumeMin, vSweptVolumeMax, m_vBoundingMin, m_vBoundingMax))
	{
		return;
	}

	if (IsLeafNode())
	{
		vecCollidableNode.push_back(this);
	}
	else
	{
		for (int i = 0; i < 8; ++i)
		{
			if (NULL != m_apSubNode[i])
			{
				m_apSubNode[i]->CollectCollidableNodes(vSweptVolumeMin, vSweptVolumeMax, vecCollidableNode);
			}
		}
	}
}







//////////////////////////////////////////////////////////////////////////

COctreeCollider::COctreeCollider()
{
	m_nColTriIndex = 0;
}

COctreeCollider::~COctreeCollider()
{
}

void COctreeCollider::SetTriangleCount(unsigned int uiTriangleCount)
{
	m_vecCollisionTriangle.resize(uiTriangleCount);
}

void COctreeCollider::GetTriangleDataPtr(CollisionTriangleInfo*& pTriangleData)
{
	if (m_vecCollisionTriangle.size() > 0)
	{
		pTriangleData = &(m_vecCollisionTriangle[0]);
	}
	else
	{
		pTriangleData = NULL;
	}
}

bool COctreeCollider::BuildOctree(const size_t nMaximumRecursion, const size_t nMinPolyCount)
{
	unsigned int i, j;

	if (0 == m_vecCollisionTriangle.size())
	{
		m_vCollisionBoundingMin.SetValue(0,0,0);
		m_vCollisionBoundingMax.SetValue(0,0,0);

		return true;	// 메시 데이터가 비어 있어도 일단은 정상종료..
	}

	// 일단 기존 노드들을 삭제
	m_octreeRootNode.ReleaseSubNode();	

	// bounding min/max 구하기
	m_vCollisionBoundingMin = m_vecCollisionTriangle[0].m_avVertex[0];
	m_vCollisionBoundingMax = m_vecCollisionTriangle[0].m_avVertex[0];

	for (i = 0; i < m_vecCollisionTriangle.size(); ++i)
	{
		for (j = 0; j < 3; ++j)
		{
			CrossM::Math::VECTOR3& v = m_vecCollisionTriangle[i].m_avVertex[j];

			if (v.x < m_vCollisionBoundingMin.x) m_vCollisionBoundingMin.x = v.x;
			if (v.y < m_vCollisionBoundingMin.y) m_vCollisionBoundingMin.y = v.y;
			if (v.z < m_vCollisionBoundingMin.z) m_vCollisionBoundingMin.z = v.z;

			if (v.x > m_vCollisionBoundingMax.x) m_vCollisionBoundingMax.x = v.x;
			if (v.y > m_vCollisionBoundingMax.y) m_vCollisionBoundingMax.y = v.y;
			if (v.z > m_vCollisionBoundingMax.z) m_vCollisionBoundingMax.z = v.z;            
		}
	}

	// octree root node 에 바운딩 박스 값 세팅
	m_octreeRootNode.m_vBoundingMin = m_vCollisionBoundingMin;
	m_octreeRootNode.m_vBoundingMax = m_vCollisionBoundingMax;

	// octree root node 에 포함된 삼각형의 인덱스(전체)를 세팅
	m_octreeRootNode.m_vecTriangleIndex.resize(m_vecCollisionTriangle.size());
	for (i = 0; i < m_octreeRootNode.m_vecTriangleIndex.size(); ++i)
	{
		m_octreeRootNode.m_vecTriangleIndex[i] = i;
	}

// DWORD dwTime = timeGetTime();
// for (i = 0; i < m_vecCollisionTriangle.size(); ++i)
// {
// 	CollisionTriangleInfo& tri = m_vecCollisionTriangle[i];
// 	Math::VECTOR3& vTri0 = m_vecCollisionVertex[ tri.m_lIndex[0] ];
// 	Math::VECTOR3& vTri1 = m_vecCollisionVertex[ tri.m_lIndex[1] ];
// 	Math::VECTOR3& vTri2 = m_vecCollisionVertex[ tri.m_lIndex[2] ];
// 
// 	Math::CheckAabbTriangleIntersection(m_vCollisionBoundingMin, m_vCollisionBoundingMax, vTri0, vTri1, vTri2);
// }
// DWORD dwElapsed = timeGetTime() - dwTime;
// 
// dwTime = timeGetTime();
// for (i = 0; i < m_vecCollisionTriangle.size(); ++i)
// {
// 	CollisionTriangleInfo& tri = m_vecCollisionTriangle[i];
// 	Math::VECTOR3& vTri0 = m_vecCollisionVertex[ tri.m_lIndex[0] ];
// 	Math::VECTOR3& vTri1 = m_vecCollisionVertex[ tri.m_lIndex[1] ];
// 	Math::VECTOR3& vTri2 = m_vecCollisionVertex[ tri.m_lIndex[2] ];
// 
// 	Math::IsTriangleInAabb(m_vCollisionBoundingMin, m_vCollisionBoundingMax, vTri0, vTri1, vTri2);
// }
// DWORD dwElapsed2 = timeGetTime() - dwTime;

	// 하부 노드들을 생성
	m_octreeRootNode.BuildSubNode(m_vecCollisionTriangle, nMaximumRecursion, nMinPolyCount, 0);

	return true;
}




void COctreeCollider::GetCollisionRespondingPosition(
						Math::VECTOR3& vOutRespondingPos,
						const Math::VECTOR3& vPrevPos, const Math::VECTOR3& vNewPos,
						const Math::VECTOR3& vEllipsoidRadius,
						const unsigned int nRecursionLevel)
{
	size_t i, j, nMinCollisionTriangleIndex;

	// ellipsoid 가 쓸고 지나간 궤적을 커버하는 사각형(AABB)의 min/max 점
	Math::VECTOR3 vSweptVolumeMin, vSweptVolumeMax;

	vSweptVolumeMin.x = min(vPrevPos.x, vNewPos.x) - vEllipsoidRadius.x;
	vSweptVolumeMax.x = max(vPrevPos.x, vNewPos.x) + vEllipsoidRadius.x;
	vSweptVolumeMin.y = min(vPrevPos.y, vNewPos.y) - vEllipsoidRadius.y;
	vSweptVolumeMax.y = max(vPrevPos.y, vNewPos.y) + vEllipsoidRadius.y;
	vSweptVolumeMin.z = min(vPrevPos.z, vNewPos.z) - vEllipsoidRadius.z;
	vSweptVolumeMax.z = max(vPrevPos.z, vNewPos.z) + vEllipsoidRadius.z;

	m_vecpCollidableNode.clear();
    m_octreeRootNode.CollectCollidableNodes(vSweptVolumeMin, vSweptVolumeMax, m_vecpCollidableNode);

	if (0 == m_vecpCollidableNode.size())
	{
		// 충돌 여지가 있는 삼각형조차 없다
		vOutRespondingPos = vNewPos;
		return;
	}

	// ellipsoid 좌표계 상의 값들. sweep 시작, 끝점과 sweep 방향벡터, sweep 구간의 길이
	Math::VECTOR3 vESweepStart, vESweepEnd, vESweepPath;
	float fESweepLength;

	vESweepStart.SetValue(vPrevPos.x/vEllipsoidRadius.x, vPrevPos.y/vEllipsoidRadius.y, vPrevPos.z/vEllipsoidRadius.z);
	vESweepEnd.SetValue(vNewPos.x/vEllipsoidRadius.x, vNewPos.y/vEllipsoidRadius.y, vNewPos.z/vEllipsoidRadius.z);
	Math::Subtract(vESweepPath, vESweepEnd, vESweepStart);
	fESweepLength = Math::GetLength(vESweepPath);
	// 움직임이 없는 경우	
	if (fESweepLength < Math::F_EPSILON)
	{
		vOutRespondingPos = vPrevPos;
		return;
	}

	// sweeping sphere 에 대한 정보를 설정
	Math::TriangSweepingSphere triAndSphere;
	triAndSphere.m_vSphereSweepStart = vESweepStart;
	triAndSphere.m_vSphereSweepPath = vESweepPath;
	triAndSphere.m_fSphereRadius = 1.0f;	// ellipsoid 좌표계이므로 충돌타원체는 단위 구체로 변환되어짐

	bool bCollision = false;				// 선택된 노드들에서 충돌이 일어난 삼각형이 있는지 여부를 나타내는 플래그
	float fMinCollisionDistFactor = 9999.0f;// collision 이 일어난 점이 sweeping path 의 어느 위치인지 나타내는 숫자
											// 0 이 시작점, 1 이 끝점임
	Math::VECTOR3 vMinContactPoint;			// fMinCollisionDistFactor 에서의 접점의 좌표. ellipsoid 좌표계임
	bool bMinContactInsideTriangle;			// fMinCollisionDistFactor 에서 삼각형과의 접점이 삼각형 내부인지(모서리나 꼭지점이 아닌) 나타내는 플래그
	for (i = 0; i < m_vecpCollidableNode.size(); ++i)
	{
		COctreeCollisionNode& node = *(m_vecpCollidableNode[i]);
		for (j = 0; j < node.m_vecTriangleIndex.size(); ++j)
		{
			CollisionTriangleInfo& tri = m_vecCollisionTriangle[ node.m_vecTriangleIndex[j] ];

			// 삼각형 정보를 설정
			Math::VECTOR3& vTri0 = tri.m_avVertex[0];
			Math::VECTOR3& vTri1 = tri.m_avVertex[1];
			Math::VECTOR3& vTri2 = tri.m_avVertex[2];
			// 삼각형 역시 ellipsoid 좌표계로 설정해줘야함을 잊지말것-ㅂ-
			triAndSphere.m_vTri0.SetValue(vTri0.x/vEllipsoidRadius.x, vTri0.y/vEllipsoidRadius.y, vTri0.z/vEllipsoidRadius.z);
			triAndSphere.m_vTri1.SetValue(vTri1.x/vEllipsoidRadius.x, vTri1.y/vEllipsoidRadius.y, vTri1.z/vEllipsoidRadius.z);
			triAndSphere.m_vTri2.SetValue(vTri2.x/vEllipsoidRadius.x, vTri2.y/vEllipsoidRadius.y, vTri2.z/vEllipsoidRadius.z);

			float fCollisionDistFactor;
			Math::VECTOR3 vContactPoint;
			bool bContactInside;
			// 삼각형 - sweeping sphere 충돌 검사
			if (Math::CheckTriangleSweepingSphereCollision(fCollisionDistFactor, vContactPoint, bContactInside, triAndSphere))
			{
				// 충돌이면
				bCollision = true;
				
				if(fCollisionDistFactor < fMinCollisionDistFactor)
				{
					vMinContactPoint = vContactPoint;
					fMinCollisionDistFactor = fCollisionDistFactor;
					nMinCollisionTriangleIndex = node.m_vecTriangleIndex[j];
					bMinContactInsideTriangle = bContactInside;
				}				
			}
		}
	}

	if (!bCollision)
	{
		// 충돌 없심
		vOutRespondingPos = vNewPos;
		return;
	}

m_nColTriIndex = nMinCollisionTriangleIndex;

	// collision response phase
	Math::VECTOR3 vMovedPos;	// 일단 충돌이 일어난점까지 이동한 위치
	Math::Lerp(vMovedPos, vPrevPos, vNewPos, fMinCollisionDistFactor); // 일단 이동 가능한 위치로 옮겨놓은 뒤..

	// 원래 이동하려한 거리의 90% 이상을 진행했거나, recursion 이 4번 수행되었으면 종료한다.
	if (fMinCollisionDistFactor > 0.9f || nRecursionLevel >= 4)
	{
		vOutRespondingPos = vMovedPos;
		return;
	}

	// 접점의 좌표를 ellipsoid 좌표계에서 원래 좌표계로 환산
	vMinContactPoint.x *= vEllipsoidRadius.x;
	vMinContactPoint.y *= vEllipsoidRadius.y;
	vMinContactPoint.z *= vEllipsoidRadius.z;

	// 접접의 접평면을 구함
	Math::VECTOR3 vTangentPlaneNormal;
	if (bMinContactInsideTriangle)
	{
		// 삼각형과의 접점이 삼각형면 내에서라면, 충돌한 삼각형의 normal 을 그대로 이용한다
		CollisionTriangleInfo& colTriInfo = m_vecCollisionTriangle[nMinCollisionTriangleIndex];
		Math::VECTOR3& vCollTri0 = colTriInfo.m_avVertex[0];
		Math::VECTOR3& vCollTri1 = colTriInfo.m_avVertex[1];
		Math::VECTOR3& vCollTri2 = colTriInfo.m_avVertex[2];
		vTangentPlaneNormal = ((vCollTri1 - vCollTri0) ^ (vCollTri2 - vCollTri0));
	}
	else
	{
		// 모서리나 꼭지점과 충돌한 경우, 타원체의 접평면을 구하는 식을 이용해 접면의 normal 을 구한다
		Math::Subtract(vTangentPlaneNormal, vMovedPos, vMinContactPoint);
		vTangentPlaneNormal.x /= (vEllipsoidRadius.x*vEllipsoidRadius.x);
		vTangentPlaneNormal.y /= (vEllipsoidRadius.y*vEllipsoidRadius.y);
		vTangentPlaneNormal.z /= (vEllipsoidRadius.z*vEllipsoidRadius.z);
		Math::Normalize(vTangentPlaneNormal, vTangentPlaneNormal);
	}
	Math::Normalize(vTangentPlaneNormal, vTangentPlaneNormal);

	// 충돌에 의해 진행하지 못하고 남은 이동성분 벡터를 구함
	Math::VECTOR3 vRemainder;
	Math::Subtract(vRemainder, vNewPos, vMovedPos);

	// 충돌에 의해 움직일수 없어 사라지게 되는 이동성분을 구함
	float fVanishingComponentFactor = Math::DotProduct(vTangentPlaneNormal, vRemainder);
	Math::VECTOR3 vVanishingComponent;
	Math::Scale(vVanishingComponent, vTangentPlaneNormal, fVanishingComponentFactor);

	// 남은 이동성분에서 움직일수 없는 방향으로의 성분을 제거
	Math::Subtract(vRemainder, vRemainder, vVanishingComponent);

	// 남은 이동벡터만큼의 이동을 위해 충돌체크 재귀호출
	GetCollisionRespondingPosition(vOutRespondingPos, vMovedPos, vMovedPos+vRemainder, vEllipsoidRadius, nRecursionLevel+1);
}


void COctreeCollider::RenderCollidableNodeTriangles(IDirect3DDevice8* pDevice)
{
	size_t nRenderTriCount; //j, i, nTriFilled; 

	if (0 == m_vecpCollidableNode.size())
	{
		return;
	}

//	// 렌더할 삼각형의 갯수 구함
// 	nRenderTriCount = 0;
// 	for (i = 0; i < m_vecpCollidableNode.size(); ++i)
// 	{
// 		nRenderTriCount += m_vecpCollidableNode[i]->m_vecTriangleIndex.size();
// 	}
// 
// 	// 버텍스 공간 확보
// 	m_vecRenderVertex.resize(nRenderTriCount*3);
// 
// 	nTriFilled = 0;
// 	for (i = 0; i < m_vecpCollidableNode.size(); ++i)
// 	{
// 		COctreeCollisionNode& node = *(m_vecpCollidableNode[i]);
// 
// 		for (j = 0; j < node.m_vecTriangleIndex.size(); ++j)
// 		{
// 			size_t nTriIndex = node.m_vecTriangleIndex[j];
// 			CollisionTriangleInfo& tri = m_vecCollisionTriangle[nTriIndex];
// 
// 			m_vecRenderVertex[nTriFilled*3] = tri.m_avVertex[0];
// 			m_vecRenderVertex[nTriFilled*3 + 1] = tri.m_avVertex[1];
// 			m_vecRenderVertex[nTriFilled*3 + 2] = tri.m_avVertex[2];
// 			++nTriFilled;
// 		}
// 	}

	nRenderTriCount = 1;	// 충돌삼각형 1개만
	m_vecRenderVertex.resize(nRenderTriCount*3);
	CollisionTriangleInfo& tri = m_vecCollisionTriangle[m_nColTriIndex];

	m_vecRenderVertex[0] = tri.m_avVertex[0];
	m_vecRenderVertex[1] = tri.m_avVertex[1];
	m_vecRenderVertex[2] = tri.m_avVertex[2];

	D3DXMATRIX mTmp;
	D3DXMatrixIdentity(&mTmp);
	pDevice->SetTransform(D3DTS_WORLD, &mTmp);

	DWORD dwFillMode, dwZFunc;
	pDevice->GetRenderState(D3DRS_FILLMODE, &dwFillMode);
	pDevice->GetRenderState(D3DRS_ZFUNC, &dwZFunc);
	pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
	pDevice->SetVertexShader(D3DFVF_XYZ);
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, (UINT)nRenderTriCount, &(m_vecRenderVertex[0]), sizeof(Math::VECTOR3));
	pDevice->SetRenderState(D3DRS_FILLMODE, dwFillMode);
	pDevice->SetRenderState(D3DRS_ZFUNC, dwZFunc);
}


}}