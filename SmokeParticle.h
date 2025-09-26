// SmokeParticle.h: interface for the CSmokeParticle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SMOKEPARTICLE_H__FD85F26C_D836_4EE6_AEE7_BE28E4BA29AA__INCLUDED_)
#define AFX_SMOKEPARTICLE_H__FD85F26C_D836_4EE6_AEE7_BE28E4BA29AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Particle.h"

class CSmokeParticle : public CParticle  
{
	vector3 m_vecNowPos;
public:
	bool Update(float fUpdateTime);
	void Render(LPDIRECT3DDEVICE8 pd3dDevice);
	void Create(vector3 vecPos,vector3 vecDir);
	CSmokeParticle();
	virtual ~CSmokeParticle();

};

#endif // !defined(AFX_SMOKEPARTICLE_H__FD85F26C_D836_4EE6_AEE7_BE28E4BA29AA__INCLUDED_)
