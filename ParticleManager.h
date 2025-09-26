// ParticleManager.h: interface for the CParticleManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARTICLEMANAGER_H__27AE095D_D9AF_4EE3_8304_21789A261BDB__INCLUDED_)
#define AFX_PARTICLEMANAGER_H__27AE095D_D9AF_4EE3_8304_21789A261BDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Particle.h"
#include "ScatterParticle.h"
#include "CircleParticle.h"
#include "SmokeParticle.h"
#include <vector>

enum PartcleType 
{
	MAGICMISSLE_PARTICLE=0,	
	SMOKE_PARTICLE		
};

class CParticleSequence
{
public:
	CParticleSequence()
	{
		m_ParticleSeq=0;
	};
	void Render(LPDIRECT3DDEVICE8 pd3dDevice);
	bool Update(float fUpdateTime);
	std::vector<CParticle*> m_ParticleList;
	long m_ParticleSeq;
};
class CParticleManager
{	
	//static List<CParticle*> m_ParticleList;
	static std::vector<CParticleSequence*> m_ParticleList;
public:
	void Update(float fUpdateTime);
	void Render(LPDIRECT3DDEVICE8 pd3dDevice);
	static CParticleSequence* AddParticle(PartcleType Type,vector3 vecPos,vector3 vecTarget);
	CParticleManager();
	virtual ~CParticleManager();
};

#endif // !defined(AFX_PARTICLEMANAGER_H__27AE095D_D9AF_4EE3_8304_21789A261BDB__INCLUDED_)
