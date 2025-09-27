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

	// height bias 가 세팅된 후, 위치 설정은 아래 둘 중 아무것으로나 할 수 있다
	void SetEllipsoidCenter(const Math::VECTOR3& vCenter);
	void SetPosition(const Math::VECTOR3& vPos);

	const Math::VECTOR3& GetEllipsoidRadius();
	const Math::VECTOR3& GetEllipsoidCenter();
	Math::VECTOR3 GetPosition();	// client 에서 이용하는 실제 위치
	

private:
	// 충돌 타원체의 중심과 반지름벡터
	Math::VECTOR3 m_vEllipoidCenter;
	Math::VECTOR3 m_vEllipsoidRaius;

	// client 측에서 이용하는 충돌점의 실제 타원체 중심 기준 높이 bias값
	float m_fHeightBias;
};

}}