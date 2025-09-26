// SmokeParticle.cpp: implementation of the CSmokeParticle class.
//
//////////////////////////////////////////////////////////////////////

#include "SmokeParticle.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSmokeParticle::CSmokeParticle()
{

}

CSmokeParticle::~CSmokeParticle()
{

}

void CSmokeParticle::Create(vector3 vecPos, vector3 vecDir)
{
	m_vecNowPos=vecPos;
	vector3 vecPosRand;
	for(int cParticle=0;cParticle<20;cParticle++)
	{
		vecPosRand=vector3(Random(),Random(),Random());
		vecPosRand.Normalize();
		vecPosRand*=Random()*80.0f;

		ParticleNode AddNode;
		AddNode.m_vecPos0=vecPosRand;
		AddNode.m_vecPos=AddNode.m_vecPos0;

		vector3 vecVelRand=vector3(Random(),Random(),Random());
		vecVelRand.Normalize();
		vecVelRand=vecVelRand*5.0f;
		vecVelRand.y-=2.0f;
		AddNode.m_vecVelocity0=vecDir+vecVelRand;
		AddNode.m_vecVelocity=AddNode.m_vecVelocity0;
		AddNode.m_vecAccelate=vector3(0.005f,0.001f,0.0f);

		AddNode.m_fSize=200+rand()%30;
		AddNode.m_fFadeTime=(Random()+1.0f)*30.0f;
		AddNode.m_fNowTime=0.0f;
		AddNode.m_fFadeTime=100+rand()%300;
		AddNode.m_clrStartDiffuse.c=0xffffffff;
		AddNode.m_clrEndDiffuse.c=0xaaaaaaaa;
		m_ParticleNodeList.push_back(AddNode);
	}
	CTexture::SetPath(FXTEXTUREPATH);
	m_ParticleTexture.Load("Circle.dds");
}

void CSmokeParticle::Render(LPDIRECT3DDEVICE8 pd3dDevice)
{
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);		
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);

	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);


	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE);
	/*
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	*/
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);
	//pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);	

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

	pd3dDevice->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,0);
	pd3dDevice->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX,1);

	pd3dDevice->SetTexture(0,m_ParticleTexture.GetTexture());
	pd3dDevice->SetTexture(1,NULL);

	Vertex[0].spec.c=0x0;Vertex[1].spec.c=0x0;Vertex[2].spec.c=0x0;Vertex[3].spec.c=0x0;

	for(int cParticle=0;cParticle<(int)m_ParticleNodeList.size();cParticle++)
	{
		if(m_ParticleNodeList[cParticle].m_fNowTime>=m_ParticleNodeList[cParticle].m_fFadeTime)		
			continue;
		/*
		vecTL=vector3( cosf( (3.14159f*0.75f)+m_ParticleNodeList[cParticle].m_fSize/400.0f ) , sinf( (3.14159f*0.75f)+m_ParticleNodeList[cParticle].m_fSize/400.0f ) , 0.0f );
		//vecTL=vector3(-1.0f,1.0f,0.0f);
		vecBL=vector3( cosf( (3.14159f*1.25f)+m_ParticleNodeList[cParticle].m_fSize/400.0f ) , sinf( (3.14159f*1.25f)+m_ParticleNodeList[cParticle].m_fSize/400.0f ) , 0.0f );
		//vecBL=vector3(-1.0f,-1.0f,0.0f);
		vecBR=vector3( cosf( (3.14159f*1.75f)+m_ParticleNodeList[cParticle].m_fSize/400.0f ) , sinf( (3.14159f*1.75f)+m_ParticleNodeList[cParticle].m_fSize/400.0f ) , 0.0f );
		//vecBR=vector3(1.0f,-1.0f,0.0f);
		vecTR=vector3( cosf( (3.14159f*0.25f)+m_ParticleNodeList[cParticle].m_fSize/400.0f ) , sinf( (3.14159f*0.25f)+m_ParticleNodeList[cParticle].m_fSize/400.0f ) , 0.0f );
		//vecTR=vector3(1.0f,1.0f,0.0f);
		*/

		

		/*
		NSVertex[0].v.x+=fNsSize*cosf(0.0f + 3.14159f*(0.75f) - m_ViewCamera.GetRotationY() );
	NSVertex[0].v.y+=fNsSize*sinf(0.0f + 3.14159f*(0.75f) - m_ViewCamera.GetRotationY() );

	NSVertex[1].v.x+=fNsSize*cosf(3.1415f*0.5f+ 3.14159f*(0.75f) - m_ViewCamera.GetRotationY() );
	NSVertex[1].v.y+=fNsSize*sinf(3.1415f*0.5f+ 3.14159f*(0.75f) - m_ViewCamera.GetRotationY() );

	NSVertex[2].v.x+=fNsSize*cosf(3.1415f*1.5f+ 3.14159f*(0.75f) - m_ViewCamera.GetRotationY() );
	NSVertex[2].v.y+=fNsSize*sinf(3.1415f*1.5f+ 3.14159f*(0.75f) - m_ViewCamera.GetRotationY() );

	NSVertex[3].v.x+=fNsSize*cosf(3.1415f+ 3.14159f*(0.75f) - m_ViewCamera.GetRotationY() );
	NSVertex[3].v.y+=fNsSize*sinf(3.1415f+ 3.14159f*(0.75f) - m_ViewCamera.GetRotationY() 
		*/
		
		//cosf(m_ParticleNodeList[cParticle].m_fSize);
		//sinf(m_ParticleNodeList[cParticle].m_fSize);
		
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

bool CSmokeParticle::Update(float fUpdateTime)
{
	/*
	m_NowTime+=0.0f;
	m_fNowVelocity+=m_fAcceleration;
	if(m_fNowVelocity>=60.0f)
		m_fNowVelocity=60.0f;


	if(m_NowTime > m_TotalRemainTime)
		return true;
		*/
	
	//m_vecNowPos=(m_vecTarget-m_vecPos)*(m_NowTime/m_TotalRemainTime)+m_vecPos;
	/*
	m_vecNowPos=m_vecPos;
	vector3 vecDirRand,vecPosRand;
	for(int cParticle=0;cParticle<m_ParticleNodeList.num;cParticle++)
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
	*/
	
	for(int cParticle=0;cParticle<(int)m_ParticleNodeList.size();cParticle++)
	{
		if(m_ParticleNodeList[cParticle].m_fNowTime>=m_ParticleNodeList[cParticle].m_fFadeTime)		
		{
			m_ParticleNodeList[cParticle].m_fNowTime=0.0f;
			m_ParticleNodeList[cParticle].m_vecPos=m_ParticleNodeList[cParticle].m_vecPos0;
			m_ParticleNodeList[cParticle].m_vecVelocity=m_ParticleNodeList[cParticle].m_vecVelocity0;
			m_ParticleNodeList[cParticle].m_fSize=100+rand()%30;
		}
		m_ParticleNodeList[cParticle].m_fNowTime+=fUpdateTime;
		m_ParticleNodeList[cParticle].m_vecPos+=m_ParticleNodeList[cParticle].m_vecVelocity;
		m_ParticleNodeList[cParticle].m_vecVelocity+=m_ParticleNodeList[cParticle].m_vecAccelate*m_ParticleNodeList[cParticle].m_fNowTime;
		m_ParticleNodeList[cParticle].m_clrNowDiffuse=color::Interpolation(m_ParticleNodeList[cParticle].m_clrStartDiffuse,
									 m_ParticleNodeList[cParticle].m_clrEndDiffuse,
									 m_ParticleNodeList[cParticle].m_fNowTime/m_ParticleNodeList[cParticle].m_fFadeTime);
		m_ParticleNodeList[cParticle].m_fSize+=(m_ParticleNodeList[cParticle].m_vecAccelate.x*m_ParticleNodeList[cParticle].m_fNowTime)*15.0f;
	}	
	return false;
}
