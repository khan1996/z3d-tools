// FallScene.cpp: implementation of the CFallScene class.
//
//////////////////////////////////////////////////////////////////////

#include "FallScene.h"
#include "SceneManager.h"
#include <BaseGraphicsLayer.h>
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTexture CFallScene::m_WaterFallTexture;

CFallScene::CFallScene()
{
	m_MapFall=NULL;

}

CFallScene::~CFallScene()
{
	//m_pFallVertex->Release();
	//m_pFallIndices->Release();
}

void CFallScene::Create()
{	
	for(int i=0;i<30;i++)
	{
		m_FallVertex[i].diff.c=0xffffffff;
		m_FallVertex[i].spec.c=0x0;
	}
	CTexture::SetPath(NATURETEXTUREPATH);
	//m_WaterFallTexture.Load("WaterFall.dds");
	/*
	CSceneManager::GetDevice()->CreateVertexBuffer(MAX_FALLPARTICLE*4*sizeof(TLVertex),
												   D3DUSAGE_WRITEONLY,TLVERTEXFVF,D3DPOOL_MANAGED,		
												   &m_pFallVertex);
	CSceneManager::GetDevice()->CreateIndexBuffer(MAX_FALLPARTICLE*6*sizeof(WORD),D3DUSAGE_WRITEONLY,
												  D3DFMT_INDEX16,D3DPOOL_MANAGED,&m_pFallIndices);
	WORD *pIndices;
	m_pFallIndices->Lock(0,MAX_FALLPARTICLE*6*sizeof(WORD),(BYTE**)&pIndices,0);
	int i;
	for(i=0;i<MAX_FALLPARTICLE;i++)
	{
		pIndices[i*6+0]=i*4+0;		
		pIndices[i*6+1]=i*4+1;
		pIndices[i*6+2]=i*4+2;		
		pIndices[i*6+3]=i*4+1;		
		pIndices[i*6+4]=i*4+2;		
		pIndices[i*6+5]=i*4+3;		
	}	
	m_pFallIndices->Unlock();
	for(i=0;i<MAX_FALLPARTICLE;i++)
	{
		
		m_fVelocity[i]=((rand()%100)+1)/200.0f;
		m_vecDir[i].y=-(rand()%10)-1;
		m_vecDir[i].x=0.0f;
		m_vecDir[i].z=0.0f;

		m_vecPos[i].x=(rand()%4000);
		m_vecPos[i].z=0.0f;
		m_vecPos[i].y=20000.0f;
		m_Collition[i]=0;	

	}
	CTexture::SetPath(NATURETEXTUREPATH);	
	m_FallPartTexture.Load("Flare2.dds");
	*/
}

void CFallScene::Update()
{
	/*
	for(int i=0;i<MAX_FALLPARTICLE;i++)
	{
		//m_vecPos[i]+=(m_vecDir[i]*m_fVelocity[i]);
		m_vecPos[i].y+=(m_vecDir[i].y*m_fVelocity[i]);
		m_vecPos[i].x+=(m_vecDir[i].x*100.0f);
		m_vecPos[i].z+=(m_vecDir[i].z*100.0f);
		m_fVelocity[i]*=1.1f;
		m_vecDir[i].y-=0.1f;
		
		if(m_vecPos[i].y<13000.0f && m_Collition[i]==0)
		{
			//m_vecDir[i].x=-cosf(3.1415f/(4000.0f/m_vecPos[i].x));
			//m_vecDir[i].z=sinf(3.1415f/(4000.0f/m_vecPos[i].x));
			float fDivide=3.0f;
			float fAngle=( (3.1415f/fDivide)) / (4000.0f/m_vecPos[i].x);
			fAngle+=(3.1415f/fDivide);
			m_vecDir[i].x=-cosf(fAngle);
			m_vecDir[i].z=sinf(fAngle);

			m_vecDir[i].y=6.0f;
			m_Collition[i]=1;
			m_fVelocity[i]=((rand()%100)+1)/200.0f;			
		}
		
		if(m_vecPos[i].y<=0.0f && m_Collition[i]==1)
		{
			m_vecPos[i].x=(rand()%4000);
			m_vecPos[i].z=rand()%10;
			m_vecPos[i].y=20000.0f;
			m_fVelocity[i]=((rand()%100)+1)/200.0f;
			m_vecDir[i].y=-(rand()%10)-1;
			m_vecDir[i].x=0.0f;
			m_vecDir[i].z=0.0f;
			m_Collition[i]=0;
		}
	}
	//*/
}

void CFallScene::Render(LPDIRECT3DDEVICE8 pd3dDevice)
{
	
	for(int i=0;i<(int)m_MapFall->m_FallHeightList.size();i++)
	{
		m_FallVertex[i*2+0].tv-=0.1f;
		m_FallVertex[i*2+1].tv-=0.1f;
	}
	
	pd3dDevice->SetTexture(0,m_WaterFallTexture.GetTexture());	
	pd3dDevice->SetTexture(1,NULL);
	pd3dDevice->SetVertexShader(LVERTEXFVF);
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2*((int)m_MapFall->m_FallHeightList.size()-1),
		m_FallVertex,sizeof(LVertex));

	/*
	Update();
	const float fParticleSize=20.0f;
	vector3 m_vecRes;
	float w;
	TLVertex* v;		
	
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
    pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE);
	//pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);

	m_pFallVertex->Lock(NULL,NULL,(BYTE**)&v,NULL);

	for(int i=0;i<MAX_FALLPARTICLE;i++)
	{
		BaseGraphicsLayer::TransformVector(m_vecPos[i],m_vecRes,w);		
		

		v[i*4+0].v=m_vecRes;

		v[i*4+0].w=w;
		v[i*4+0].Diffuse.c=0x220000ff;
		v[i*4+0].Specular.c=0x0;
		
		v[i*4+1].v=m_vecRes;
		v[i*4+1].w=w;
		v[i*4+1].Diffuse.c=0x220000ff;
		v[i*4+1].Specular.c=0x0;

		v[i*4+2].v=m_vecRes;
		v[i*4+2].w=w;
		v[i*4+2].Diffuse.c=0x220000ff;
		v[i*4+2].Specular.c=0x0;

		v[i*4+3].v=m_vecRes;
		v[i*4+3].w=w;
		v[i*4+3].Diffuse.c=0x220000ff;
		v[i*4+3].Specular.c=0x0;

		v[i*4+0].v.x-=fParticleSize;
		v[i*4+0].v.y+=fParticleSize;

		v[i*4+1].v.x-=fParticleSize;
		v[i*4+1].v.y-=fParticleSize;

		v[i*4+2].v.x+=fParticleSize;
		v[i*4+2].v.y+=fParticleSize;

		v[i*4+3].v.x+=fParticleSize;
		v[i*4+3].v.y-=fParticleSize;

		v[i*4+0].tu=0.0f;
		v[i*4+0].tv=1.0f;

		v[i*4+1].tu=0.0f;
		v[i*4+1].tv=0.0f;

		v[i*4+2].tu=1.0f;
		v[i*4+2].tv=1.0f;

		v[i*4+3].tu=1.0f;
		v[i*4+3].tv=0.0f;
	}
	m_pFallVertex->Unlock();
	
	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	
	pd3dDevice->SetVertexShader(TLVERTEXFVF);
	pd3dDevice->SetStreamSource(0,m_pFallVertex,sizeof(TLVertex));
	pd3dDevice->SetIndices(m_pFallIndices,0);	
	pd3dDevice->SetTexture(0,m_FallPartTexture.GetTexture());
	pd3dDevice->SetTexture(1,NULL);

	pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,MAX_FALLPARTICLE*4,0,MAX_FALLPARTICLE*2);		
	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);		
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	//pd3dDevice->DrawPrimitive(D3D,0,4*MAX_FALLPARTICLE);
	*/	
}

void CFallScene::MakeFallScene(CSectorFallMap *MapFall)
{
	m_MapFall=MapFall;	
	
	float fInterHeight;
	for(int i=0;i<(int)MapFall->m_FallHeightList.size();i++)
	{
		m_FallVertex[i*2+0].diff=MapFall->m_FallColor[i];
		m_FallVertex[i*2+1].diff=MapFall->m_FallColor[i];
		if(i!=0)
		{
			fInterHeight=abs(MapFall->m_FallHeightList[i]-MapFall->m_FallHeightList[i-1]);

			m_FallVertex[i*2+0].tv=m_FallVertex[(i-1)*2+0].tv+fInterHeight/3000.0f;
			m_FallVertex[i*2+1].tv=m_FallVertex[(i-1)*2+1].tv+fInterHeight/3000.0f;
		}
		else
		{
			m_FallVertex[i*2+0].tv=0.0f;
			m_FallVertex[i*2+1].tv=0.0f;
		}
		m_FallVertex[i*2+0].v.x=MapFall->m_FallAddXList[i];
		m_FallVertex[i*2+0].v.z=MapFall->m_FallLeftList[i];
		m_FallVertex[i*2+0].v.y=MapFall->m_FallHeightList[i];
		m_FallVertex[i*2+0].tu=0.0f;

		m_FallVertex[i*2+1].v.x=MapFall->m_FallAddXList[i];
		m_FallVertex[i*2+1].v.z=MapFall->m_FallRightList[i];
		m_FallVertex[i*2+1].v.y=MapFall->m_FallHeightList[i];
		m_FallVertex[i*2+1].tu=1.0f;		
	}
}
