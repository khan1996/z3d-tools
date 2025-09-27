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

// �ڽĳ�尡 ���� ���� ���� ���� ���
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

	// recursion level �� �ʹ� ��ų�, ���Ե� face ������ �������ͺ��� ������
	// ���̻� sub node �� ������ �ʴ´� (��� ���� ����)
	if (nCurrentRecursionLevel >= nMaximumRecursion ||
		m_vecTriangleIndex.size() <= nMinPolyCount)
	{
nProcessedTri += m_vecTriangleIndex.size();
printf("\r%d / %d", nProcessedTri, vecTriangle.size());
		return;
	}

	// �ڽ� ������ bounding box min/max
	Math::VECTOR3 aSubNodeBoundingMin[8];
	Math::VECTOR3 aSubNodeBoundingMax[8];

	Math::VECTOR3 vMedian = (m_vBoundingMin + m_vBoundingMax)/2.0f;

	// �ٿ�� �ڽ� �� ����
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

	// �Ѱܹ��� �ﰢ�� �ε������� �Ϻ� ���� �������͵�� �� ��忡 ����ɰ͵��� �����س���
	for (j = 0; j < m_vecTriangleIndex.size(); ++j)
	{
		// �ﰢ�� �� �� ���
		const CollisionTriangleInfo &tri = vecTriangle[ m_vecTriangleIndex[j] ];
		const Math::VECTOR3 &vTri0 = tri.m_avVertex[0];
		const Math::VECTOR3 &vTri1 = tri.m_avVertex[1];
		const Math::VECTOR3 &vTri2 = tri.m_avVertex[2];

		// �� �ڽĳ��鿡 ���� �ﰢ���� ���ԵǴ��� üũ�Ѵ�
		bool bIncludedInSubNode = false;	// �ڽ� ��忡 �ﰢ���� ���ԵǴ��� ���θ� ��Ÿ���� �÷���
		for (i = 0; i < 8; ++i)
		{
			if (Math::IsTriangleInAabb(aSubNodeBoundingMin[i], aSubNodeBoundingMax[i], vTri0, vTri1, vTri2))
			{
				// ��尡 �Ҵ���� �ʾ����� �Ҵ�
				COctreeCollisionNode* &pNode = m_apSubNode[i];
				if (NULL == pNode)
				{
					pNode = new COctreeCollisionNode;
					pNode->m_vBoundingMin = aSubNodeBoundingMin[i];
					pNode->m_vBoundingMax = aSubNodeBoundingMax[i];
				}

				// �ڽĳ�忡 �Ѱ��� �ﰢ�� �ε����� ����
				pNode->m_vecTriangleIndex.push_back(m_vecTriangleIndex[j]);
				bIncludedInSubNode = true;
				break;
			}
		}
		if (!bIncludedInSubNode)
		{
			// �� ��忡 ������ ���ԵǴ°��� �ƴ϶� �ټ� ��尣�� ��ġ�� �ﰢ���̹Ƿ�,
			// triangle - AABB intersection �׽�Ʈ�� �Ѵ�
			for (i = 0; i < 8; ++i)
			{
				if (Math::CheckAabbTriangleIntersection(aSubNodeBoundingMin[i], aSubNodeBoundingMax[i], vTri0, vTri1, vTri2))
				{
					// ��尡 �Ҵ���� �ʾ����� �Ҵ�
					COctreeCollisionNode* &pNode = m_apSubNode[i];
					if (NULL == pNode)
					{
						pNode = new COctreeCollisionNode;
						pNode->m_vBoundingMin = aSubNodeBoundingMin[i];
						pNode->m_vBoundingMax = aSubNodeBoundingMax[i];
					}

					// �ڽĳ�忡 �Ѱ��� �ﰢ�� �ε����� ����
					pNode->m_vecTriangleIndex.push_back(m_vecTriangleIndex[j]);
				}
			}
		}
	}
	
	// �ﰢ�� �ε����� �ڽ� ��忡 ��� �Ѱ��־����Ƿ� �� �̻� �ε����� ��� ���� �ʿ䰡 ����
	m_vecTriangleIndex.clear();

	// �ﰢ���� �ִ� �ڽ� ���� BuildSubNode �� ȣ���Ѵ�
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

		return true;	// �޽� �����Ͱ� ��� �־ �ϴ��� ��������..
	}

	// �ϴ� ���� ������ ����
	m_octreeRootNode.ReleaseSubNode();	

	// bounding min/max ���ϱ�
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

	// octree root node �� �ٿ�� �ڽ� �� ����
	m_octreeRootNode.m_vBoundingMin = m_vCollisionBoundingMin;
	m_octreeRootNode.m_vBoundingMax = m_vCollisionBoundingMax;

	// octree root node �� ���Ե� �ﰢ���� �ε���(��ü)�� ����
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

	// �Ϻ� ������ ����
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

	// ellipsoid �� ���� ������ ������ Ŀ���ϴ� �簢��(AABB)�� min/max ��
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
		// �浹 ������ �ִ� �ﰢ������ ����
		vOutRespondingPos = vNewPos;
		return;
	}

	// ellipsoid ��ǥ�� ���� ����. sweep ����, ������ sweep ���⺤��, sweep ������ ����
	Math::VECTOR3 vESweepStart, vESweepEnd, vESweepPath;
	float fESweepLength;

	vESweepStart.SetValue(vPrevPos.x/vEllipsoidRadius.x, vPrevPos.y/vEllipsoidRadius.y, vPrevPos.z/vEllipsoidRadius.z);
	vESweepEnd.SetValue(vNewPos.x/vEllipsoidRadius.x, vNewPos.y/vEllipsoidRadius.y, vNewPos.z/vEllipsoidRadius.z);
	Math::Subtract(vESweepPath, vESweepEnd, vESweepStart);
	fESweepLength = Math::GetLength(vESweepPath);
	// �������� ���� ���	
	if (fESweepLength < Math::F_EPSILON)
	{
		vOutRespondingPos = vPrevPos;
		return;
	}

	// sweeping sphere �� ���� ������ ����
	Math::TriangSweepingSphere triAndSphere;
	triAndSphere.m_vSphereSweepStart = vESweepStart;
	triAndSphere.m_vSphereSweepPath = vESweepPath;
	triAndSphere.m_fSphereRadius = 1.0f;	// ellipsoid ��ǥ���̹Ƿ� �浹Ÿ��ü�� ���� ��ü�� ��ȯ�Ǿ���

	bool bCollision = false;				// ���õ� ���鿡�� �浹�� �Ͼ �ﰢ���� �ִ��� ���θ� ��Ÿ���� �÷���
	float fMinCollisionDistFactor = 9999.0f;// collision �� �Ͼ ���� sweeping path �� ��� ��ġ���� ��Ÿ���� ����
											// 0 �� ������, 1 �� ������
	Math::VECTOR3 vMinContactPoint;			// fMinCollisionDistFactor ������ ������ ��ǥ. ellipsoid ��ǥ����
	bool bMinContactInsideTriangle;			// fMinCollisionDistFactor ���� �ﰢ������ ������ �ﰢ�� ��������(�𼭸��� �������� �ƴ�) ��Ÿ���� �÷���
	for (i = 0; i < m_vecpCollidableNode.size(); ++i)
	{
		COctreeCollisionNode& node = *(m_vecpCollidableNode[i]);
		for (j = 0; j < node.m_vecTriangleIndex.size(); ++j)
		{
			CollisionTriangleInfo& tri = m_vecCollisionTriangle[ node.m_vecTriangleIndex[j] ];

			// �ﰢ�� ������ ����
			Math::VECTOR3& vTri0 = tri.m_avVertex[0];
			Math::VECTOR3& vTri1 = tri.m_avVertex[1];
			Math::VECTOR3& vTri2 = tri.m_avVertex[2];
			// �ﰢ�� ���� ellipsoid ��ǥ��� ������������� ��������-��-
			triAndSphere.m_vTri0.SetValue(vTri0.x/vEllipsoidRadius.x, vTri0.y/vEllipsoidRadius.y, vTri0.z/vEllipsoidRadius.z);
			triAndSphere.m_vTri1.SetValue(vTri1.x/vEllipsoidRadius.x, vTri1.y/vEllipsoidRadius.y, vTri1.z/vEllipsoidRadius.z);
			triAndSphere.m_vTri2.SetValue(vTri2.x/vEllipsoidRadius.x, vTri2.y/vEllipsoidRadius.y, vTri2.z/vEllipsoidRadius.z);

			float fCollisionDistFactor;
			Math::VECTOR3 vContactPoint;
			bool bContactInside;
			// �ﰢ�� - sweeping sphere �浹 �˻�
			if (Math::CheckTriangleSweepingSphereCollision(fCollisionDistFactor, vContactPoint, bContactInside, triAndSphere))
			{
				// �浹�̸�
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
		// �浹 ����
		vOutRespondingPos = vNewPos;
		return;
	}

m_nColTriIndex = nMinCollisionTriangleIndex;

	// collision response phase
	Math::VECTOR3 vMovedPos;	// �ϴ� �浹�� �Ͼ������ �̵��� ��ġ
	Math::Lerp(vMovedPos, vPrevPos, vNewPos, fMinCollisionDistFactor); // �ϴ� �̵� ������ ��ġ�� �Űܳ��� ��..

	// ���� �̵��Ϸ��� �Ÿ��� 90% �̻��� �����߰ų�, recursion �� 4�� ����Ǿ����� �����Ѵ�.
	if (fMinCollisionDistFactor > 0.9f || nRecursionLevel >= 4)
	{
		vOutRespondingPos = vMovedPos;
		return;
	}

	// ������ ��ǥ�� ellipsoid ��ǥ�迡�� ���� ��ǥ��� ȯ��
	vMinContactPoint.x *= vEllipsoidRadius.x;
	vMinContactPoint.y *= vEllipsoidRadius.y;
	vMinContactPoint.z *= vEllipsoidRadius.z;

	// ������ ������� ����
	Math::VECTOR3 vTangentPlaneNormal;
	if (bMinContactInsideTriangle)
	{
		// �ﰢ������ ������ �ﰢ���� ���������, �浹�� �ﰢ���� normal �� �״�� �̿��Ѵ�
		CollisionTriangleInfo& colTriInfo = m_vecCollisionTriangle[nMinCollisionTriangleIndex];
		Math::VECTOR3& vCollTri0 = colTriInfo.m_avVertex[0];
		Math::VECTOR3& vCollTri1 = colTriInfo.m_avVertex[1];
		Math::VECTOR3& vCollTri2 = colTriInfo.m_avVertex[2];
		vTangentPlaneNormal = ((vCollTri1 - vCollTri0) ^ (vCollTri2 - vCollTri0));
	}
	else
	{
		// �𼭸��� �������� �浹�� ���, Ÿ��ü�� ������� ���ϴ� ���� �̿��� ������ normal �� ���Ѵ�
		Math::Subtract(vTangentPlaneNormal, vMovedPos, vMinContactPoint);
		vTangentPlaneNormal.x /= (vEllipsoidRadius.x*vEllipsoidRadius.x);
		vTangentPlaneNormal.y /= (vEllipsoidRadius.y*vEllipsoidRadius.y);
		vTangentPlaneNormal.z /= (vEllipsoidRadius.z*vEllipsoidRadius.z);
		Math::Normalize(vTangentPlaneNormal, vTangentPlaneNormal);
	}
	Math::Normalize(vTangentPlaneNormal, vTangentPlaneNormal);

	// �浹�� ���� �������� ���ϰ� ���� �̵����� ���͸� ����
	Math::VECTOR3 vRemainder;
	Math::Subtract(vRemainder, vNewPos, vMovedPos);

	// �浹�� ���� �����ϼ� ���� ������� �Ǵ� �̵������� ����
	float fVanishingComponentFactor = Math::DotProduct(vTangentPlaneNormal, vRemainder);
	Math::VECTOR3 vVanishingComponent;
	Math::Scale(vVanishingComponent, vTangentPlaneNormal, fVanishingComponentFactor);

	// ���� �̵����п��� �����ϼ� ���� ���������� ������ ����
	Math::Subtract(vRemainder, vRemainder, vVanishingComponent);

	// ���� �̵����͸�ŭ�� �̵��� ���� �浹üũ ���ȣ��
	GetCollisionRespondingPosition(vOutRespondingPos, vMovedPos, vMovedPos+vRemainder, vEllipsoidRadius, nRecursionLevel+1);
}


void COctreeCollider::RenderCollidableNodeTriangles(IDirect3DDevice8* pDevice)
{
	size_t nRenderTriCount; //j, i, nTriFilled; 

	if (0 == m_vecpCollidableNode.size())
	{
		return;
	}

//	// ������ �ﰢ���� ���� ����
// 	nRenderTriCount = 0;
// 	for (i = 0; i < m_vecpCollidableNode.size(); ++i)
// 	{
// 		nRenderTriCount += m_vecpCollidableNode[i]->m_vecTriangleIndex.size();
// 	}
// 
// 	// ���ؽ� ���� Ȯ��
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

	nRenderTriCount = 1;	// �浹�ﰢ�� 1����
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