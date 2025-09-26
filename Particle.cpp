// Particle.cpp: implementation of the CParticle class.
//
//////////////////////////////////////////////////////////////////////

#include "Particle.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

matrix *CParticle::m_matPosition;


CParticle::CParticle()
{

}

CParticle::~CParticle()
{

}

void CParticle::Create()
{
	
}

void CParticle::Render(LPDIRECT3DDEVICE8 pd3dDevice)
{	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);		
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);	

	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);	
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);	

	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE);
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE);
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);

	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	matrix matInit;
	matInit.MakeIdent();
	pd3dDevice->SetTransform(D3DTS_WORLD,matInit);
	
	vector3 vecTL(-1.0f,1.0f,0.0f);
	vector3 vecBL(-1.0f,-1.0f,0.0f);
	vector3 vecBR(1.0f,-1.0f,0.0f);
	vector3 vecTR(1.0f,1.0f,0.0f);

	matrix matPosition=*m_matPosition;
	matPosition._41=0.0f;
	matPosition._42=0.0f;
	matPosition._43=0.0f;

	matrix matTL,matBL,matBR,matTR;
	matTL.Translation(vecTL);
	matBL.Translation(vecBL);
	matBR.Translation(vecBR);
	matTR.Translation(vecTR);

	matTL=matTL*matPosition;
	matBL=matBL*matPosition;
	matBR=matBR*matPosition;
	matTR=matTR*matPosition;
	/*
	matTL=matPosition*matTL;
	matBL=matPosition*matBL;
	matBR=matPosition*matBR;
	matTR=matPosition*matTR;
	*/	

	vecTL=matTL.GetLoc();
	vecBL=matBL.GetLoc();
	vecBR=matBR.GetLoc();
	vecTR=matTR.GetLoc();

	LVertex Vertex[4];	
	Vertex[0].tu=0.0f;Vertex[0].tv=1.0f;
	Vertex[1].tu=0.0f;Vertex[1].tv=0.0f;
	Vertex[2].tu=1.0f;Vertex[2].tv=1.0f;	
	Vertex[3].tu=1.0f;Vertex[3].tv=0.0f;

	pd3dDevice->SetVertexShader(LVERTEXFVF);	
	
	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	pd3dDevice->SetTexture(0,m_ParticleTexture.GetTexture());
	pd3dDevice->SetTexture(1,NULL);

	Vertex[0].spec.c=0x0;Vertex[1].spec.c=0x0;Vertex[2].spec.c=0x0;Vertex[3].spec.c=0x0;

	for(int cParticle=0;cParticle<(int)m_ParticleNodeList.size();cParticle++)
	{
		if(m_ParticleNodeList[cParticle].m_fNowTime>=m_ParticleNodeList[cParticle].m_fFadeTime)		
			continue;

		Vertex[0].v=m_ParticleNodeList[cParticle].m_vecPos+vecBL*m_ParticleNodeList[cParticle].m_fSize;
		Vertex[1].v=m_ParticleNodeList[cParticle].m_vecPos+vecTL*m_ParticleNodeList[cParticle].m_fSize;		
		Vertex[2].v=m_ParticleNodeList[cParticle].m_vecPos+vecBR*m_ParticleNodeList[cParticle].m_fSize;
		Vertex[3].v=m_ParticleNodeList[cParticle].m_vecPos+vecTR*m_ParticleNodeList[cParticle].m_fSize;		

		Vertex[0].diff.c=Vertex[1].diff.c=Vertex[2].diff.c=Vertex[3].diff.c=
			m_ParticleNodeList[cParticle].m_clrNowDiffuse.c;
		
		pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,Vertex,sizeof(LVertex));
	}
	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
}

bool CParticle::Update(float fUpdateTime)
{	
	return false;
}
