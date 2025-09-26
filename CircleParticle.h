// CircleParticle.h: interface for the CCircleParticle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CIRCLEPARTICLE_H__46C31B75_D404_429C_9E90_E3AFE811CE52__INCLUDED_)
#define AFX_CIRCLEPARTICLE_H__46C31B75_D404_429C_9E90_E3AFE811CE52__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Particle.h"

class CCircleParticle : public CParticle
{
public:
	void Render(LPDIRECT3DDEVICE8 pd3dDevice);
	vector3 m_vecPos,m_vecTarget,m_vecNowPos;
	float m_fNowVelocity,m_fAcceleration;
	float m_TotalRemainTime;
	float m_NowTime;
	bool Update(float fUpdateTime);
	void Create(vector3 vecPos,vector3 vecTarget);
	CCircleParticle();
	virtual ~CCircleParticle();

};

#endif // !defined(AFX_CIRCLEPARTICLE_H__46C31B75_D404_429C_9E90_E3AFE811CE52__INCLUDED_)
