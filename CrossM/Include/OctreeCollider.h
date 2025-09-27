/*
	Collision detection + response �� ó���ϴ� Ŭ����
    �浹 �ﰢ���� culling �� octree �� �̿�

	collision detection �� response �˰����� Kasper Fauerby ��
	'Improved Collision detection and Response' �� �����ϴ� �ڵ��̴�
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
	// �ΰ��� ����..�̸��׸� �� �ﰢ���� ���� ������Ʈ�� id �����. �׷����� �ʿ��ҵ�
	// ������Ʈ Ŭ����(����/�Ͽ콺/������Ʈ)+�� Ŭ���� �������� ������ƮID(�� �Ͽ콺�� ������ȣ)
	// ���� ���� �ڷ����� ������?

	Math::VECTOR3 m_vFaceNormal;
};

// octree �� node. ���� ���� �ƴ϶� �ٱ� ��嵵 �ﰢ���� �ε����� ���� �� �ִ�.
// �ٿ�� �ڽ� ���� �ﰢ���� �� ���� ������ ���ԵǴ� �ﰢ���� �� ����� �ﰢ�� �ε��� ����Ʈ�� ��ϵȴ�.
// (��迡 ��ġ�� �� �ﰢ���� �� ���� �� �����ϴ� ���� ��忡 ��ϵ�)
class COctreeCollisionNode
{
public:
	COctreeCollisionNode();
	~COctreeCollisionNode();

	// �� ����� ���� ������ ��������� ��� ����
	void ReleaseSubNode();

	// leaf node ���� üũ
	bool IsLeafNode();

	// �ڽ� ������ �����ϰ�, �ڽ��� ��忡 ���Ե� �ﰢ���� �ε������� �ڽ� ���� �л��Ų��.
	// �ڱ� �ڽ��� ��忡�� �ٿ���ڽ� ���� ���õ� ���¿��� �Ѵ�.
	void BuildSubNode(		
			const std::vector< CollisionTriangleInfo >& vecTriangle,
			const size_t nMaximumRecursion, const size_t nMinPolyCount,
			size_t nCurrentRecursionLevel);


	// �浹 ��ü�� swept volume �� �ɸ��� �浹 ������ ����Ʈ�� vecCollidableNode �� ����
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
	// Octree Build ���� �޼ҵ�

	/*
		�浹 �ﰢ�� ������ ����.
		�浹 �ﰢ���� ���������� ����� ������ �Ҵ�ǰ�, octree ������ �ı��ȴ�.
		BuildOctree() �� ȣ��Ǳ� ������ COctreeCollider �� �����۵����� �ʴ´�
	*/
	void SetTriangleCount(unsigned int uiTriangleCount);

	/*
		�ﰢ�� ������ ������� �����͸� ��ȯ.
		�� �޼ҵ带 ȣ���ϸ� �ﰢ�� �����Ͱ� �����Ȱ����� ����, octree ������ �ı��ȴ�.
		BuildOctree() �� ȣ��Ǳ� ������ COctreeCollider �� �����۵����� �ʴ´�
	*/
	void GetTriangleDataPtr(CollisionTriangleInfo*& pTriangleData);

	// ���õ� �ﰢ�� ������ ���ʷ� octree ������ ����
	bool BuildOctree(const size_t nMaximumRecursion, const size_t nMinPolyCount);



	//////////////////////////////////////////////////////////////////////////
	//
	// �浹���� �� ���� �޼ҵ�

	/*
		�־��� Ÿ��ü�� ���� ��ġ(prevPos)�� �̵��� ��ġ(nextPos) �� �Ķ���ͷ� �ѱ��,
		�浹�� üũ�ϰ� �浹�� �Ͼ�� ��� ������ ��ġ�� ������ �̵� ��ġ�� ��ȯ�Ѵ�.
	*/
	void GetCollisionRespondingPosition(
			Math::VECTOR3& vRespondingPos,
			const Math::VECTOR3& vPrevPos, const Math::VECTOR3& vNewPos,
			const Math::VECTOR3& vEllipsoidRadius,
			const unsigned int nRecursionLevel = 1);


// �׽�Ʈ�� �޼ҵ�, �浹�� �Ͼ �� �ִ°����� Ȯ�ε� ����� �ﰢ������ ����
void RenderCollidableNodeTriangles(IDirect3DDevice8* pDevice);


private:
	std::vector< CollisionTriangleInfo > m_vecCollisionTriangle;

	Math::VECTOR3 m_vCollisionBoundingMin;
	Math::VECTOR3 m_vCollisionBoundingMax;

	COctreeCollisionNode m_octreeRootNode;


// �ӽÿ�, �浹�� �Ͼ �� �ִ°����� Ȯ�ε� leaf ����
std::vector< COctreeCollisionNode* > m_vecpCollidableNode;

// �ӽÿ�, ������ ���ؽ� ������
std::vector< Math::VECTOR3 > m_vecRenderVertex;

// �ӽÿ�. �浹�� �Ͼ �ﰢ���� �ε���
size_t m_nColTriIndex;

};


}}