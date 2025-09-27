/*
	Collision detection + response 를 처리하는 클래스
    충돌 삼각형의 culling 은 octree 를 이용

	collision detection 과 response 알고리즘은 Kasper Fauerby 의
	'Improved Collision detection and Response' 에 기초하는 코드이다
*/

#pragma once

#include "./Vector3.h"

#include <vector>

// forward decl.
struct IDirect3DDevice8;


namespace CrossM{
namespace Scene{

struct CollisionTriangleInfo
{
	Math::VECTOR3 m_avVertex[3];
	// 부가적 정보..이를테면 이 삼각형이 속한 오브젝트의 id 라던가. 그런것이 필요할듯
	// 오브젝트 클래스(지형/하우스/오브젝트)+한 클래스 내에서의 오브젝트ID(각 하우스별 고유번호)
	// 같은 식의 자료형이 될지도?

	Math::VECTOR3 m_vFaceNormal;
};

// octree 의 node. 리프 노드뿐 아니라 줄기 노드도 삼각형의 인덱스를 가질 수 있다.
// 바운딩 박스 내에 삼각형의 세 점이 완전히 포함되는 삼각형만 그 노드의 삼각형 인덱스 리스트에 기록된다.
// (경계에 걸치는 그 삼각형의 세 점을 다 포함하는 상위 노드에 기록됨)
class COctreeCollisionNode
{
public:
	COctreeCollisionNode();
	~COctreeCollisionNode();

	// 현 노드의 하위 노드들을 재귀적으로 모두 해제
	void ReleaseSubNode();

	// leaf node 인지 체크
	bool IsLeafNode();

	// 자식 노드들을 생성하고, 자신의 노드에 포함된 삼각형의 인덱스들을 자식 노드로 분산시킨다.
	// 자기 자신의 노드에는 바운딩박스 값이 세팅된 상태여야 한다.
	void BuildSubNode(		
			const std::vector< CollisionTriangleInfo >& vecTriangle,
			const size_t nMaximumRecursion, const size_t nMinPolyCount,
			size_t nCurrentRecursionLevel);


	// 충돌 객체의 swept volume 에 걸리는 충돌 노드들의 리스트를 vecCollidableNode 에 저장
	void CollectCollidableNodes(
		const Math::VECTOR3& vSweptVolumeMin, const Math::VECTOR3& vSweptVolumeMax,
		std::vector< COctreeCollisionNode* >& vecCollidableNode);

	Math::VECTOR3 m_vBoundingMin;
	Math::VECTOR3 m_vBoundingMax;
	std::vector< size_t > m_vecTriangleIndex;

	COctreeCollisionNode* m_apSubNode[8];
};


class COctreeCollider
{
public:
	COctreeCollider();
	~COctreeCollider();

	//////////////////////////////////////////////////////////////////////////
	//
	// Octree Build 관련 메소드

	/*
		충돌 삼각형 갯수를 설정.
		충돌 삼각형이 내부적으로 저장될 공간이 할당되고, octree 정보가 파괴된다.
		BuildOctree() 가 호출되기 전까지 COctreeCollider 는 정상작동하지 않는다
	*/
	void SetTriangleCount(unsigned int uiTriangleCount);

	/*
		삼각형 데이터 저장소의 포인터를 반환.
		이 메소드를 호출하면 삼각형 데이터가 변형된것으로 간주, octree 정보가 파괴된다.
		BuildOctree() 가 호출되기 전까지 COctreeCollider 는 정상작동하지 않는다
	*/
	void GetTriangleDataPtr(CollisionTriangleInfo*& pTriangleData);

	// 세팅된 삼각형 정보를 기초로 octree 정보를 구축
	bool BuildOctree(const size_t nMaximumRecursion, const size_t nMinPolyCount);



	//////////////////////////////////////////////////////////////////////////
	//
	// 충돌검출 및 반응 메소드

	/*
		주어진 타원체의 이전 위치(prevPos)와 이동할 위치(nextPos) 를 파라미터로 넘기면,
		충돌을 체크하고 충돌이 일어났을 경우 적절한 위치로 수정된 이동 위치를 반환한다.
	*/
	void GetCollisionRespondingPosition(
			Math::VECTOR3& vRespondingPos,
			const Math::VECTOR3& vPrevPos, const Math::VECTOR3& vNewPos,
			const Math::VECTOR3& vEllipsoidRadius,
			const unsigned int nRecursionLevel = 1);


// 테스트용 메소드, 충돌이 일어날 수 있는것으로 확인된 노드의 삼각형들을 렌더
void RenderCollidableNodeTriangles(IDirect3DDevice8* pDevice);


private:
	std::vector< CollisionTriangleInfo > m_vecCollisionTriangle;

	Math::VECTOR3 m_vCollisionBoundingMin;
	Math::VECTOR3 m_vCollisionBoundingMax;

	COctreeCollisionNode m_octreeRootNode;


// 임시용, 충돌이 일어날 수 있는것으로 확인된 leaf 노드들
std::vector< COctreeCollisionNode* > m_vecpCollidableNode;

// 임시용, 렌더용 버텍스 데이터
std::vector< Math::VECTOR3 > m_vecRenderVertex;

// 임시용. 충돌이 일어난 삼각형의 인덱스
size_t m_nColTriIndex;

};


}}