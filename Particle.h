// Particle.h: interface for the CParticle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARTICLE_H__B3B3EC93_033B_41B4_A21F_5C8245083E52__INCLUDED_)
#define AFX_PARTICLE_H__B3B3EC93_033B_41B4_A21F_5C8245083E52__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "3DMath.h"
#include <vector>
#include "Vertex.h"
#include "Texture.h"
#include "BaseDataDefine.h"

#define Random() (float)(rand()%10000-5000)/5000.0f

class CParticle  
{
protected:
	static matrix *m_matPosition;
	class ParticleNode
	{
	public:
		vector3 m_vecPos0;
		vector3 m_vecPos;

		vector3 m_vecVelocity0;
		vector3 m_vecVelocity;
		vector3 m_vecAccelate;
		
		float m_fSize;		
		float m_fFadeTime;
		float m_fNowTime;
		color m_clrStartDiffuse;
		color m_clrEndDiffuse;
		color m_clrNowDiffuse;
	};
	CTexture m_ParticleTexture;
public:
	std::vector<ParticleNode> m_ParticleNodeList;
	static void SetMatPosition(matrix *matPosition){m_matPosition=matPosition;};

	virtual bool Update(float fUpdateTime);
	virtual void Render(LPDIRECT3DDEVICE8 pd3dDevice);
	virtual void Create();
	CParticle();
	virtual ~CParticle();

};

#endif // !defined(AFX_PARTICLE_H__B3B3EC93_033B_41B4_A21F_5C8245083E52__INCLUDED_)
