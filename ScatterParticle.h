// ScatterParticle.h: interface for the CScatterParticle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCATTERPARTICLE_H__030425F7_475B_4AE0_B584_4E6235AD84F9__INCLUDED_)
#define AFX_SCATTERPARTICLE_H__030425F7_475B_4AE0_B584_4E6235AD84F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Particle.h"

class CScatterParticle : public CParticle  
{
	bool m_bGravity;
public:
	void Create(vector3 vecPos);
	void SetGravity(bool bGravity){m_bGravity=bGravity;};
	bool Update(float fUpdateTime);
	CScatterParticle();
	virtual ~CScatterParticle();

};

#endif // !defined(AFX_SCATTERPARTICLE_H__030425F7_475B_4AE0_B584_4E6235AD84F9__INCLUDED_)
