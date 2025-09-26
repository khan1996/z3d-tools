// BillboardParticle.h: interface for the CBillboardParticle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BILLBOARDPARTICLE_H__76021856_2A4A_4818_B687_CC374D0D028D__INCLUDED_)
#define AFX_BILLBOARDPARTICLE_H__76021856_2A4A_4818_B687_CC374D0D028D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Particle.h"

class CBillboardParticle  : public CParticle
{
	vector3 m_vecPos;	
public:
	void Create(vector3 vecPos);
	CBillboardParticle();
	virtual ~CBillboardParticle();

};

#endif // !defined(AFX_BILLBOARDPARTICLE_H__76021856_2A4A_4818_B687_CC374D0D028D__INCLUDED_)
