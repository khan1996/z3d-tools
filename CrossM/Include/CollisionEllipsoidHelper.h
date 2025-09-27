#pragma once

#include "./Vector3.h"


namespace CrossM{
namespace Scene{

class CCollisionEllipsoidHelper{

public:
	CCollisionEllipsoidHelper();
	~CCollisionEllipsoidHelper();

	void SetEllipsoidRadius(const Math::VECTOR3& vRadius);
	void SetHeightBias(float f);

	// height bias �� ���õ� ��, ��ġ ������ �Ʒ� �� �� �ƹ������γ� �� �� �ִ�
	void SetEllipsoidCenter(const Math::VECTOR3& vCenter);
	void SetPosition(const Math::VECTOR3& vPos);

	const Math::VECTOR3& GetEllipsoidRadius();
	const Math::VECTOR3& GetEllipsoidCenter();
	Math::VECTOR3 GetPosition();	// client ���� �̿��ϴ� ���� ��ġ
	

private:
	// �浹 Ÿ��ü�� �߽ɰ� ����������
	Math::VECTOR3 m_vEllipoidCenter;
	Math::VECTOR3 m_vEllipsoidRaius;

	// client ������ �̿��ϴ� �浹���� ���� Ÿ��ü �߽� ���� ���� bias��
	float m_fHeightBias;
};

}}