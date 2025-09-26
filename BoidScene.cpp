// BoidScene.cpp: implementation of the CBoidScene class.
//
//////////////////////////////////////////////////////////////////////

#include <time.h>
#include "BoidScene.h"
#include "SceneManager.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CBoidScene::CBoidScene()
{

}

CBoidScene::~CBoidScene()
{

}

void CBoidScene::Create(int nBoid,int BoidObject)
{	    
    srand(time(NULL));

	m_nBoids=nBoid;
	m_pBoids=new BoidNode[nBoid];

	m_BoidObject=BoidObject;

	m_vecGoal=vector3(0.0f,0.0f,0.0f);

	for(int i=0;i<m_nBoids;i++)
	{
		m_pBoids[i].m_vecLoc=vector3(i+10,i+10+150,i+10);
		m_pBoids[i].m_vecDir=vector3(1.0f,0.0f,0.0f);
		m_pBoids[i].m_fYaw=0.0f;
		m_pBoids[i].m_fPitch=0.0f;
		m_pBoids[i].m_fRoll=0.0f;
		m_pBoids[i].m_fDYaw=0.0f;
		m_pBoids[i].m_fSpeed=0.1f;
		m_pBoids[i].m_fAni=rnd()*2.0f;
		m_pBoids[i].m_fDist=new float[m_nBoids];
	}    	
	CSceneManager::m_MeshObjectContainer.SetPath(OBJECTPATH);
	m_pBoidObject[0]=CSceneManager::m_MeshObjectContainer.GetMeshObject("butter01.R3S");
	m_pBoidObject[1]=CSceneManager::m_MeshObjectContainer.GetMeshObject("butter02.R3S");

	m_InfluenceRadius        = 20.0f;
	m_InfluenceRadiusSquared = m_InfluenceRadius * m_InfluenceRadius;
	m_CollisionFraction      = 0.8f;
	m_InvCollisionFraction   = 1.0f/(1.0f-m_CollisionFraction);
	m_NormalSpeed            = 0.1f;
	m_AngleTweak             = 0.02f;
	m_PitchToSpeedRatio      = 0.002f;
}

void CBoidScene::Update(float fUpdate)
{
	/*
	UpdateFlock();

	matrix matWorld;
	matrix matRotateX,matRotateY,matRotateZ,matTemp;
	for(int i=0;i<m_nBoids;i++)
	{
		vector3 vecStep;
		matWorld.Translation(m_pBoids[i].m_vecLoc);

		matRotateY.YRotation(-m_pBoids[i].m_fYaw);
		matRotateX.XRotation(-m_pBoids[i].m_fPitch);
		matRotateZ.ZRotation(-m_pBoids[i].m_fRoll);

		matTemp=matRotateX*matRotateY;
		matTemp=matRotateZ*matTemp;
		matWorld=matTemp*matWorld;

		m_pBoids[i].m_matLocal=matWorld;

		m_pBoids[i].m_vecDir.x=matWorld._31;
		m_pBoids[i].m_vecDir.y=matWorld._32;
		m_pBoids[i].m_vecDir.z=matWorld._33;

		m_pBoids[i].m_vecLoc+=m_pBoids[i].m_vecDir*m_pBoids[i].m_fSpeed;

		m_pBoids[i].m_fAni+=0.3f;
	}	
	static float tic = -200.0f * 1;
    tic += 0.01f;

	m_vecGoal.x = 405.0f * (float)sin(tic*0.1f)+800;
    m_vecGoal.y = 150.0f;
    m_vecGoal.z = 405.0f * (float)cos(tic*0.1f)+800;
	*/

}
void CBoidScene::UpdateFlock()
{
	vector3 vecLens;
	float fDist;

	for(int i=0;i<m_nBoids;i++)
	{
		for(int j=i+1;j<m_nBoids;j++)
		{
			vecLens=m_pBoids[i].m_vecLoc-m_pBoids[j].m_vecLoc;			
			fDist=vecLens*vecLens;
			fDist=m_InfluenceRadiusSquared-fDist;

			if(fDist<0.0f)
				fDist=0.0f;
			else
				fDist/=m_InfluenceRadiusSquared;

			m_pBoids[i].m_fDist[j]=m_pBoids[j].m_fDist[i]=fDist;
		}
		m_pBoids[i].m_fDist[i]=0.0f;
		m_pBoids[i].m_vecDeltaDir=vector3(0.0f,0.0f,0.0f);
		m_pBoids[i].m_vecDeltaPos=vector3(0.0f,0.0f,0.0f);

		m_pBoids[i].m_DeltaCnt=0;
	}
	
	for(int i=0;i<m_nBoids;i++)
	{
		for(int j=i+1;j<m_nBoids;j++)
		{
			if(m_pBoids[i].m_fDist[j]>0.0f)
			{
				vector3 vecDiff=m_pBoids[i].m_vecLoc-m_pBoids[j].m_vecLoc;
				vecDiff.Normalize();
				
				vector3 vecDelta;

				float fCollWeight=0.0f;

				if(m_pBoids[i].m_fDist[j]-m_CollisionFraction > 0.0f)
					fCollWeight=(m_pBoids[i].m_fDist[j]-m_CollisionFraction)
								*m_InvCollisionFraction;

				if(m_pBoids[i].m_fDist[j]-(1.0f-m_CollisionFraction) >0.0f)
					fCollWeight-=m_pBoids[i].m_fDist[j]*(1.0f-fCollWeight);	

				
				vecDelta=fCollWeight*vecDiff;

				m_pBoids[i].m_vecDeltaPos+=vecDelta;
				m_pBoids[j].m_vecDeltaPos-=vecDelta;

				m_pBoids[i].m_vecDeltaDir+=m_pBoids[j].m_vecDir*m_pBoids[i].m_fDist[j];
				m_pBoids[j].m_vecDeltaDir+=m_pBoids[i].m_vecDir*m_pBoids[i].m_fDist[j];

				m_pBoids[i].m_DeltaCnt++;
				m_pBoids[j].m_DeltaCnt++;
			}
		}
	}
    
	for(int i=0;i<m_nBoids;i++)
	{
		if(m_pBoids[i].m_DeltaCnt)
		{
			m_pBoids[i].m_vecDeltaDir/=(float)m_pBoids[i].m_DeltaCnt;
			m_pBoids[i].m_vecDeltaDir-=m_pBoids[i].m_vecDir;
			m_pBoids[i].m_vecDeltaDir*=1.5f;
		}
		vector3 vecDelta=m_pBoids[i].m_vecDeltaDir+m_pBoids[i].m_vecDeltaPos;
		vector3 vecOffset;

		vecLens=m_vecGoal-m_pBoids[i].m_vecLoc;
		vector3 vecGoal=0.5f*vecLens.Normalized();

		vecDelta+=vecGoal;
		
		if(vecDelta.y>0.01f)
		{
			m_pBoids[i].m_fPitch+=m_AngleTweak;
			if(m_pBoids[i].m_fPitch > 0.8f)
				m_pBoids[i].m_fPitch=0.8f;				
		}
		else if(vecDelta.y<-0.01f)
		{
			m_pBoids[i].m_fPitch-=m_AngleTweak;
			if(m_pBoids[i].m_fPitch<-0.8f)
				m_pBoids[i].m_fPitch=-0.8f;
		}
		else
		{
			m_pBoids[i].m_fPitch*=0.98f;
		}
		m_pBoids[i].m_fSpeed-=m_pBoids[i].m_fPitch*m_PitchToSpeedRatio;
		m_pBoids[i].m_fSpeed=(m_pBoids[i].m_fSpeed-m_NormalSpeed)*0.99f+m_NormalSpeed;

		if(m_pBoids[i].m_fSpeed<m_NormalSpeed/2.0f)
			m_pBoids[i].m_fSpeed=m_NormalSpeed/2.0f;
		if(m_pBoids[i].m_fSpeed>m_NormalSpeed*5.0f)
			m_pBoids[i].m_fSpeed=m_NormalSpeed*5.0f;

		vecOffset=vecDelta;
		vecOffset.y=0.0f;
		vecDelta=m_pBoids[i].m_vecDir;
		vecOffset.Normalize();

		float fDot=vecOffset*vecDelta;

		if(fDot>0.7f)
		{
			fDot-=0.7f;
			m_pBoids[i].m_fSpeed+=fDot*0.005f;
		}

		vecOffset=vecOffset^vecDelta;
		fDot=(1.0f-fDot)/2.0f*0.07f;

		if(vecOffset.y>0.05f)
			m_pBoids[i].m_fDYaw=(m_pBoids[i].m_fDYaw*19.0f+fDot)*0.05f;
		else if(vecOffset.y<-0.05f)
			m_pBoids[i].m_fDYaw=(m_pBoids[i].m_fDYaw*19.0f-fDot)*0.05f;
		else
			m_pBoids[i].m_fDYaw*=0.98f;

		m_pBoids[i].m_fYaw+=m_pBoids[i].m_fDYaw;
		m_pBoids[i].m_fRoll=-m_pBoids[i].m_fDYaw*20.0f;	
	}    
}

void CBoidScene::Render(LPDIRECT3DDEVICE8 pd3dDevice)
{
	/*
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);	
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,TRUE);

	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);	

	for(int i=0;i<m_nBoids;i++)
	{
		pd3dDevice->SetTransform(D3DTS_WORLD,m_pBoids[i].m_matLocal);

		if(cosf(m_pBoids[i].m_fAni)>0.0f)
			m_pBoidObject[0]->Render(pd3dDevice);
		else
			m_pBoidObject[1]->Render(pd3dDevice);
	}
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);	
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);
	*/
}

void CBoidScene::DeleteBoid()
{
	if(m_pBoids) {
		for(int i=0;i<m_nBoids;i++)
		{
			if(m_pBoids[i].m_fDist != NULL) {
				delete [] m_pBoids[i].m_fDist;	
				m_pBoids[i].m_fDist = NULL;
			}
		}
		
		delete [] m_pBoids;
		m_pBoids = NULL;
	}
}
