// ParticleManager.cpp: implementation of the CParticleManager class.
//
//////////////////////////////////////////////////////////////////////

#include "ParticleManager.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
std::vector<CParticleSequence*> CParticleManager::m_ParticleList;

CParticleManager::CParticleManager()
{

}

CParticleManager::~CParticleManager()
{

}

CParticleSequence* CParticleManager::AddParticle(PartcleType Type,vector3 vecPos,vector3 vecTarget)
{
	CParticleSequence *AddNode=new CParticleSequence;
	CParticle *AddParticle;
	switch(Type)
	{
		case MAGICMISSLE_PARTICLE:			
			/*
			AddParticle=new CScatterParticle();			
			((CScatterParticle*)AddParticle)->Create(vecPos);
			AddNode->m_ParticleList.Add(AddParticle);
			*/
			AddParticle=new CCircleParticle();			
			((CCircleParticle*)AddParticle)->Create(vecPos,vecTarget);
			AddNode->m_ParticleList.push_back(AddParticle);
			AddParticle=new CScatterParticle();
			((CScatterParticle*)AddParticle)->Create(vecTarget);
			AddNode->m_ParticleList.push_back(AddParticle);
			break;					
		case SMOKE_PARTICLE:
			AddParticle=new CSmokeParticle();
			((CSmokeParticle*)AddParticle)->Create(vecPos,vecTarget);
			AddNode->m_ParticleList.push_back(AddParticle);
		default:
			break;
	}	
	m_ParticleList.push_back(AddNode);
	return AddNode;
}

void CParticleManager::Render(LPDIRECT3DDEVICE8 pd3dDevice)
{
	for(int cParticleSeq=0;cParticleSeq<(int)m_ParticleList.size();cParticleSeq++)
	{
		m_ParticleList[cParticleSeq]->Render(pd3dDevice);
	}
}

void CParticleManager::Update(float fUpdateTime)
{
	bool isDelete=false;
	for(int cParticle=0;cParticle<(int)m_ParticleList.size();cParticle++)
	{
		isDelete=false;
		isDelete=m_ParticleList[cParticle]->Update(fUpdateTime);
		if(isDelete)
		{

		}
	}
}

bool CParticleSequence::Update(float fUpdateTime)
{
	if(m_ParticleList[m_ParticleSeq]->Update(fUpdateTime))
	{
		m_ParticleSeq++;
		if(m_ParticleSeq>=(int)m_ParticleList.size())
		{
			return true;
		}
	}
	return false;
}

void CParticleSequence::Render(LPDIRECT3DDEVICE8 pd3dDevice)
{
	if(m_ParticleSeq>=(int)m_ParticleList.size())
		return;
	m_ParticleList[m_ParticleSeq]->Render(pd3dDevice);
}
