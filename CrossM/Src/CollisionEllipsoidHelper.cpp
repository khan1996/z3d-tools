#include "../Include/CollisionEllipsoidHelper.h"


namespace CrossM{
namespace Scene{

CCollisionEllipsoidHelper::CCollisionEllipsoidHelper()
{
	m_vEllipoidCenter.SetValue(0,0,0);
	m_vEllipsoidRaius.SetValue(1,1,1);

	m_fHeightBias = 0.0f;
}

CCollisionEllipsoidHelper::~CCollisionEllipsoidHelper()
{
}

void CCollisionEllipsoidHelper::SetEllipsoidRadius(const Math::VECTOR3& vRadius)
{
	m_vEllipsoidRaius = vRadius;
}

void CCollisionEllipsoidHelper::SetHeightBias(float f)
{
	m_fHeightBias = f;
}

void CCollisionEllipsoidHelper::SetEllipsoidCenter(const Math::VECTOR3& vCenter)
{
	m_vEllipoidCenter = vCenter;
}

void CCollisionEllipsoidHelper::SetPosition(const Math::VECTOR3& vPos)
{
	m_vEllipoidCenter.SetValue(vPos.x, vPos.y-m_fHeightBias, vPos.z);
}

const Math::VECTOR3& CCollisionEllipsoidHelper::GetEllipsoidRadius()
{
	return m_vEllipsoidRaius;
}

const Math::VECTOR3& CCollisionEllipsoidHelper::GetEllipsoidCenter()
{
	return m_vEllipoidCenter;
}


Math::VECTOR3 CCollisionEllipsoidHelper::GetPosition()
{
	return Math::VECTOR3(m_vEllipoidCenter.x, m_vEllipoidCenter.y+m_fHeightBias, m_vEllipoidCenter.z);
}


}}