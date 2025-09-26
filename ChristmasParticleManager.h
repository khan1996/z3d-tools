// hristmasParticleManager.h: interface for the ChristmasParticleManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HRISTMASPARTICLEMANAGER_H__114DD580_853B_4680_B82E_A16AFBC204A2__INCLUDED_)
#define AFX_HRISTMASPARTICLEMANAGER_H__114DD580_853B_4680_B82E_A16AFBC204A2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ChristmasParticle.h"
#include <vector>

class CChristmasParticleManager  
{
protected:
	int m_iNodesNum;
	std::vector<CChristmasParticle *> m_Nodes;
	
public:
	CChristmasParticleManager();
	virtual ~CChristmasParticleManager();

	void AddParticle(CChristmasParticle *,float fX,float fY,float fZ);
	void ProcessParticle();
	void DeleteParticle(int iNum);
	void Render();
	void DeleteEnd();

};

#endif // !defined(AFX_HRISTMASPARTICLEMANAGER_H__114DD580_853B_4680_B82E_A16AFBC204A2__INCLUDED_)
