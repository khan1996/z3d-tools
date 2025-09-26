// CircleParticle.cpp: implementation of the CCircleParticle class.
//
//////////////////////////////////////////////////////////////////////

#include "CircleParticle.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCircleParticle::CCircleParticle()
{

}

CCircleParticle::~CCircleParticle()
{

}

void CCircleParticle::Create(vector3 vecPos, vector3 vecTarget)
{
	vector3 vecDirRand,vecPosRand;
	vecDirRand=vecTarget-vecPos;
	vecDirRand.Normalize();

	m_vecPos=vecPos;	
	for(int cParticle=0;cParticle<1000;cParticle++)
	{		
		vecPosRand=vector3(Random(),Random(),Random());
		vecPosRand.Normalize();		
		vecPosRand*=Random()*20.0f;
		
		ParticleNode AddNode;		
		AddNode.m_vecPos0=vecPosRand;
		AddNode.m_vecPos=AddNode.m_vecPos0;
		AddNode.m_vecVelocity0=vector3(Random()*0.5f,Random()*0.5f,Random()*0.5f);
		AddNode.m_vecVelocity0-=vecDirRand*2.0f;
		AddNode.m_vecVelocity0.Normalize();
		AddNode.m_vecVelocity=AddNode.m_vecVelocity0;

		AddNode.m_vecAccelate=-vecDirRand*0.01f;

		AddNode.m_fSize=4.0f;
		AddNode.m_fFadeTime=(Random()+1.0f)*30.0f;
		AddNode.m_fNowTime=0.0f;
		AddNode.m_clrStartDiffuse.c=0xffffaaaa;
		AddNode.m_clrEndDiffuse.c=0xff000000;
		m_ParticleNodeList.push_back(AddNode);
	}	
	CTexture::SetPath(FXTEXTUREPATH);
	m_ParticleTexture.Load("Circle.dds");
}

bool CCircleParticle::Update(float fUpdateTime)
{
	m_NowTime+=0.0f;
	m_fNowVelocity+=m_fAcceleration;
	if(m_fNowVelocity>=60.0f)
		m_fNowVelocity=60.0f;


	if(m_NowTime > m_TotalRemainTime)
		return true;
	
	//m_vecNowPos=(m_vecTarget-m_vecPos)*(m_NowTime/m_TotalRemainTime)+m_vecPos;
	m_vecNowPos=m_vecPos;
	vector3 vecDirRand,vecPosRand;
	for(int cParticle=0;cParticle<(int)m_ParticleNodeList.size();cParticle++)
	{
		if(m_ParticleNodeList[cParticle].m_fNowTime>=m_ParticleNodeList[cParticle].m_fFadeTime)		
		{
			m_ParticleNodeList[cParticle].m_fNowTime=0.0f;
			m_ParticleNodeList[cParticle].m_vecPos=m_ParticleNodeList[cParticle].m_vecPos0;
			m_ParticleNodeList[cParticle].m_vecVelocity=m_ParticleNodeList[cParticle].m_vecVelocity0;
		}
		m_ParticleNodeList[cParticle].m_fNowTime+=fUpdateTime;		
		m_ParticleNodeList[cParticle].m_vecPos+=m_ParticleNodeList[cParticle].m_vecVelocity;
		m_ParticleNodeList[cParticle].m_vecVelocity+=m_ParticleNodeList[cParticle].m_vecAccelate*m_ParticleNodeList[cParticle].m_fNowTime;
		m_ParticleNodeList[cParticle].m_clrNowDiffuse=color::Interpolation(m_ParticleNodeList[cParticle].m_clrStartDiffuse,
									 m_ParticleNodeList[cParticle].m_clrEndDiffuse,
									 m_ParticleNodeList[cParticle].m_fNowTime/m_ParticleNodeList[cParticle].m_fFadeTime);
	}	
	return false;
}

void CCircleParticle::Render(LPDIRECT3DDEVICE8 pd3dDevice)
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

		Vertex[0].v=m_ParticleNodeList[cParticle].m_vecPos+vecBL*m_ParticleNodeList[cParticle].m_fSize+m_vecNowPos;
		Vertex[1].v=m_ParticleNodeList[cParticle].m_vecPos+vecTL*m_ParticleNodeList[cParticle].m_fSize+m_vecNowPos;		
		Vertex[2].v=m_ParticleNodeList[cParticle].m_vecPos+vecBR*m_ParticleNodeList[cParticle].m_fSize+m_vecNowPos;
		Vertex[3].v=m_ParticleNodeList[cParticle].m_vecPos+vecTR*m_ParticleNodeList[cParticle].m_fSize+m_vecNowPos;	


		Vertex[0].diff.c=Vertex[1].diff.c=Vertex[2].diff.c=Vertex[3].diff.c=
			m_ParticleNodeList[cParticle].m_clrNowDiffuse.c;
		
		pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,Vertex,sizeof(LVertex));
	}
	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
}
