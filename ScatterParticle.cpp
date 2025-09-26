// ScatterParticle.cpp: implementation of the CScatterParticle class.
//
//////////////////////////////////////////////////////////////////////

#include "ScatterParticle.h"
#include "GMMemory.h"

///////////////////false//////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScatterParticle::CScatterParticle()
{
	m_bGravity=false;
}

CScatterParticle::~CScatterParticle()
{

}

//DEL void CScatterParticle::Create()
//DEL {
//DEL 	
//DEL }

bool CScatterParticle::Update(float fUpdateTime)
{
	/*
	for(int cRepeat=0;cRepeat<3;cRepeat++)
	{
		for(int cParticle=0;cParticle<m_ParticleNodeList.num;cParticle++)
		{
			if(m_ParticleNodeList[cParticle].m_fNowTime>=m_ParticleNodeList[cParticle].m_fFadeTime)		
				continue;
			m_ParticleNodeList[cParticle].m_fNowTime+=64.0f;
			m_ParticleNodeList[cParticle].m_vecSource+=m_ParticleNodeList[cParticle].m_vecDir;
			m_ParticleNodeList[cParticle].m_clrNowDiffuse=
				color::Interpolation(m_ParticleNodeList[cParticle].m_clrStartDiffuse,
									 m_ParticleNodeList[cParticle].m_clrEndDiffuse,
									 m_ParticleNodeList[cParticle].m_fNowTime/m_ParticleNodeList[cParticle].m_fFadeTime);								 
			if(m_bGravity)
			{
				m_ParticleNodeList[cParticle].m_vecDir+=vector3(0.0f,-0.3f,0.0f);
			}
		}
	}
	*/
	return false;
}

void CScatterParticle::Create(vector3 vecPos)
{
	/*
	vector3 vecDirRand,vecPosRand;
	for(int cParticle=0;cParticle<500;cParticle++)
	{
		vecDirRand=vector3(Random()*100.0f,Random()*100.0f,Random()*100.0f);
		vecDirRand.Normalize();
		vecDirRand*=Random()*5.0f;
		//vecDirRand+=vector3(0.0f,4.0f,0.0f);

		vecPosRand=vector3(Random()*100.0f,Random()*100.0f,Random()*100.0f);
		vecPosRand.Normalize();
		vecPosRand*=Random()*60.0f;
		
		ParticleNode AddNode;
		AddNode.m_vecSource=vecPos+vecPosRand;
		AddNode.m_vecDir=vecDirRand;
		AddNode.m_fSize=5.0f;
		AddNode.m_fFadeTime=2000.0f;
		AddNode.m_fNowTime=0.0f;
		AddNode.m_clrStartDiffuse.c=0xffffffff;
		AddNode.m_clrEndDiffuse.c=0xff000000;
		m_ParticleNodeList.Add(AddNode);
	}
	*/
	CTexture::SetPath(FXTEXTUREPATH);
	m_ParticleTexture.Load("Circle.dds");
}
