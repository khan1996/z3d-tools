// SkyScene.cpp: implementation of the CSkyScene class.
//
//////////////////////////////////////////////////////////////////////

#include "SkyScene.h"
#include "SceneManager.h"
#include "GMMemory.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTexture CSkyScene::m_CloudTexture;

CSkyScene::CSkyScene()
{
	m_fFogLayerHeight[0]=50000.0f;
	m_fFogLayerHeight[1]=40000.0f;
	m_fFogLayerHeight[2]=0.0f;
}

CSkyScene::~CSkyScene()
{
	
}

void CSkyScene::Create()
{
	//LogMessage("SkyScene Create");
	float fDivAng=(3.141597f*2.0f)/SKYDIVIDE;
	int i;
	m_SkyVertex[0].v=vector3(0.0f,fSky_Height,0.0f);
	m_SkyVertex[0].spec.c=0.0f;
	for(i=1;i<SKYVERTEX;i++)
	{
		m_SkyVertex[i].v.x=(SKYSIZE)*sin(-fDivAng*(i-1));
		m_SkyVertex[i].v.z=(SKYSIZE)*cos(-fDivAng*(i-1));
		m_SkyVertex[i].v.y=fSky_Height-100000.0f;
		m_SkyVertex[i].spec.c=0x0;
	}
	for(int cCloud=0;cCloud<MAX_CLOUD;cCloud++)
	{
		m_CloudVertex[cCloud][0].v=vector3(0.0f,fCloud_Height+m_fFogLayerHeight[cCloud],0.0f);
		m_CloudVertex[cCloud][0].spec.c=0x0;		
		m_CloudVertex[cCloud][0].tu=0.0f;
		m_CloudVertex[cCloud][0].tv=0.0f;
		for(i=1;i<SKYVERTEX;i++)
		{
			m_CloudVertex[cCloud][i].v.x=(SKYSIZE)*sin(-fDivAng*(i-1));
			m_CloudVertex[cCloud][i].v.z=(SKYSIZE)*cos(-fDivAng*(i-1));			
			m_CloudVertex[cCloud][i].v.y=fCloud_Height+m_fFogLayerHeight[cCloud]-80000.0f;			
			m_CloudVertex[cCloud][i].tu=(3.5f)*sin(-fDivAng*(i-1));
			m_CloudVertex[cCloud][i].tv=(3.5f)*cos(-fDivAng*(i-1));		
			m_CloudVertex[cCloud][i].spec.c=0x0;		
			
		}
	}	
	unsigned char Alpha;
	for(i=0;i<STARVERTEX;i++)
	{
		m_StarVertex[i].v.x=((rand()%200)/200.0f)-0.5f;
		m_StarVertex[i].v.z=((rand()%200)/200.0f)-0.5f;
		m_StarVertex[i].v.y=((rand()%100)/100.0f)-0.2f;
		
		Alpha=rand()%100;
		Alpha+=155;
		//m_StarVertex[i].diff.c=0xffffffff;
		m_StarVertex[i].diff.a=0xff;
		m_StarVertex[i].diff.r=m_StarVertex[i].diff.g=m_StarVertex[i].diff.b=Alpha;
		m_StarVertex[i].spec.c=0x0;

		m_StarVertex[i].v.Normalize();

		m_StarVertex[i].v=m_StarVertex[i].v*100000.0f;
	}		
	/*
	CTexture::SetPath(NATURETEXTUREPATH);
	m_CloudTexture.Load("Clouds.dds");	
	*/
	//LogMessage("SkyScene Create complete");
	/*
	LVertex *pVertex;	
	CSceneManager::GetDevice()->CreateVertexBuffer(SKYVERTEX*sizeof(LVertex),
												   D3DUSAGE_WRITEONLY,LVERTEXFVF,D3DPOOL_MANAGED,
												   &m_pSkyVertex);
	m_pSkyVertex->Lock(0,0,(BYTE**)&pVertex,0);
	pVertex[0].v.x=0.0f;
	pVertex[0].v.z=0.0f;
	pVertex[0].v.y=fSky_Height;
	pVertex[0].diff.c=0xff0000af;
	pVertex[0].spec.c=0x0;
	

	float fDivAng=(3.141597f*2.0f)/SKYDIVIDE;
	for(int i=1;i<SKYVERTEX;i++)
	{
		pVertex[i].v.x=(SKYSIZE/1.0)*sin(-fDivAng*(i-1));
		pVertex[i].v.z=(SKYSIZE/1.0)*cos(-fDivAng*(i-1));		
		pVertex[i].v.y=fSky_Height-50000.0f;
		pVertex[i].diff.c=0xff0000af;		
		pVertex[i].spec.c=0x0;
	}
	m_pSkyVertex->Unlock();
	CSceneManager::GetDevice()->CreateVertexBuffer(SKYVERTEX*sizeof(LVertex),
												   D3DUSAGE_WRITEONLY,LVERTEXFVF,D3DPOOL_MANAGED,
												   &m_pCloudVertex);
	

	m_pCloudVertex->Lock(0,0,(BYTE**)&pVertex,0);
	pVertex[0].v.x=0.0f;
	pVertex[0].v.z=0.0f;
	pVertex[0].v.y=fCloud_Height;
	pVertex[0].diff.c=0x0;
	pVertex[0].spec.c=0x0;	
	pVertex[0].tu=0.0f;
	pVertex[0].tv=0.0f;
	for(i=1;i<SKYVERTEX;i++)
	{
		pVertex[i].v.x=(SKYSIZE)*sin(-fDivAng*(i-1));
		pVertex[i].v.z=(SKYSIZE)*cos(-fDivAng*(i-1));		
		pVertex[i].v.y=fCloud_Height-50000.0f;
		pVertex[i].diff.c=0xffffff00;		
		pVertex[i].spec.c=0x0;
		pVertex[i].tu=(0.7f)*sin(-fDivAng*(i-1));
		pVertex[i].tv=(0.7f)*cos(-fDivAng*(i-1));
	}
	m_pCloudVertex->Unlock();

	
	/*
	m_skymesh[0].x=0.0f;
	m_skymesh[0].z=0.0f;
	m_skymesh[0].y=80000;	
	m_skymesh[0].color=0xff0000af;
	m_skymesh[0].specular=0x0;

	float fDivAng=(3.141597f*2.0f)/SKYDIVIDE;
	for(int i=1;i<SKYVERTEX;i++)
	{
		m_skymesh[i].x=(SKYSIZE/1.0)*sin(-fDivAng*(i-1));
		m_skymesh[i].z=(SKYSIZE/1.0)*cos(-fDivAng*(i-1));		
		m_skymesh[i].y=80000.0f;
		m_skymesh[i].color=0xff0000af;		
		m_skymesh[i].specular=0x0;
	}
	*/
}

void CSkyScene::Render(LPDIRECT3DDEVICE8 pd3dDevice)
{
	//RenderSkyBox(pd3dDevice);
	///*
	
	matrix matWorld;
	pd3dDevice->GetTransform(D3DTS_WORLD,(D3DMATRIX*)(&matWorld));
	vector3 *vecViewPos=CSceneManager::GetCamera()->GetPosition();
	matrix mat;
	mat.Translation(vector3(vecViewPos->x,0.0f,vecViewPos->z));
	mat=mat*matWorld;
	pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&mat);
	pd3dDevice->SetTexture(0,NULL);
	pd3dDevice->SetTexture(1,NULL);

	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,TRUE);
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);

	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);	    
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );	
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);


	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_CURRENT);    
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);		
	
	pd3dDevice->SetVertexShader(LVERTEXFVF);
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,SKYDIVIDE,m_SkyVertex,sizeof(LVertex));	

	//
	//pd3dDevice->SetRenderState( D3DRS_FOGENABLE,TRUE);		
	/*
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
    pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);		
	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,D3DTOP_SELECTARG2);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	pd3dDevice->SetTexture(0,m_CloudTexture.GetTexture());	

	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,SKYDIVIDE,m_CloudVertex[0],sizeof(LVertex));	
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,SKYDIVIDE,m_CloudVertex[1],sizeof(LVertex));	

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,TRUE);
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,SKYDIVIDE,m_CloudVertex[2],sizeof(LVertex));	
	*/
	//pd3dDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_ALWAYS);
	pd3dDevice->DrawPrimitiveUP(D3DPT_POINTLIST,STARVERTEX,m_StarVertex,sizeof(LVertex));
	//pd3dDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);	
	

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);	
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	//*/
}

void CSkyScene::SetColor(DWORD dwColor,DWORD dwCenterColor,DWORD *dwLayerColor)
{	
	int i=0;

	m_SkyVertex[0].diff.c=dwCenterColor;
	for(i=1;i<SKYVERTEX;i++)	
		m_SkyVertex[i].diff.c=dwColor;	
	
	for(i=0;i<SKYVERTEX;i++)
	{
		m_CloudVertex[0][i].diff.c=dwLayerColor[0];
	}

	for(i=0;i<SKYVERTEX;i++)
	{
		m_CloudVertex[1][i].diff.c=dwLayerColor[1];
	}

	for(i=0;i<SKYVERTEX;i++)
	{
		m_CloudVertex[2][i].diff.c=dwLayerColor[2];
	}	
	 /*
	LVertex *pVertex;
	m_pSkyVertex->Lock(0,0,(BYTE**)&pVertex,0);
	pVertex[0].diff.c=dwCenterColor;
	for(int i=1;i<SKYVERTEX;i++)
		pVertex[i].diff.c=dwColor;
	m_pSkyVertex->Unlock();
	m_FogLayer[0].c=dwLayerColor[0];
	m_FogLayer[1].c=dwLayerColor[1];
	m_FogLayer[2].c=dwLayerColor[2];
	/*
	for(int i=0;i<SKYVERTEX;i++)	
		m_skymesh[i].color=dwColor;					
		*/
}

void CSkyScene::RenderSkyBox(LPDIRECT3DDEVICE8 pd3dDevice)
{
	float fSize=200000.0f;
	LVertex Vertex[4];
	WORD Indices[6];
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);    
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);

	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);		
	
	matrix matWorld;
	matrix matOldWorld;
	pd3dDevice->GetTransform(D3DTS_WORLD,matOldWorld);
	vector3 *vecViewPos=CSceneManager::GetCamera()->GetPosition();
	pd3dDevice->SetTexture(1,NULL);	
	
	matWorld.Translation(vector3(vecViewPos->x,0.0f,vecViewPos->z));
	matWorld=matWorld*matOldWorld;	

	pd3dDevice->SetTransform(D3DTS_WORLD,matWorld);
	//pd3dDevice->SetRenderState( D3DRS_LIGHT,FALSE);
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);

	Indices[0]=0;
	Indices[1]=2;
	Indices[2]=1;
	Indices[3]=0;
	Indices[4]=3;
	Indices[5]=2;

	Vertex[0].v=vector3(-fSize,fSize,fSize);
	Vertex[0].tu=0.0f;
	Vertex[0].tv=1.0f;
	Vertex[1].v=vector3(fSize,fSize,fSize);
	Vertex[1].tu=1.0f;
	Vertex[1].tv=1.0f;
	Vertex[2].v=vector3(fSize,fSize,-fSize);
	Vertex[2].tu=1.0f;
	Vertex[2].tv=0.0f;
	Vertex[3].v=vector3(-fSize,fSize,-fSize);
	Vertex[3].tu=0.0f;
	Vertex[3].tv=0.0f;

	pd3dDevice->SetVertexShader(LVERTEXFVF);
	pd3dDevice->SetTexture(0,m_SkyTexture[4].GetTexture());
	pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,
		0,4,2,Indices,D3DFMT_INDEX16,Vertex, sizeof(LVertex));	

	Vertex[0].v=vector3(-fSize,0.0f,fSize);	
	Vertex[0].tu=0.0;
	Vertex[0].tv=1.0;

	Vertex[1].v=vector3(fSize,0.0f,fSize);
	Vertex[1].tu=1.0;
	Vertex[1].tv=1.0;

	Vertex[2].v=vector3(fSize,fSize,fSize);
	Vertex[2].tu=1.0;
	Vertex[2].tv=0.0;

	Vertex[3].v=vector3(-fSize,fSize,fSize);	
	Vertex[3].tu=0.0;
	Vertex[3].tv=0.0;

	pd3dDevice->SetTexture(0,m_SkyTexture[0].GetTexture());
	pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,
		0,4,2,Indices,D3DFMT_INDEX16,Vertex, sizeof(LVertex));	

	Vertex[0].v=vector3(fSize,0.0f,fSize);
	Vertex[1].v=vector3(fSize,0.0f,-fSize);
	Vertex[2].v=vector3(fSize,fSize,-fSize);
	Vertex[3].v=vector3(fSize,fSize,fSize);
	pd3dDevice->SetTexture(0,m_SkyTexture[1].GetTexture());
	pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,
		0,4,2,Indices,D3DFMT_INDEX16,Vertex, sizeof(LVertex));	


	Vertex[0].v=vector3(fSize,0.0f,-fSize);
	Vertex[1].v=vector3(-fSize,0.0f,-fSize);
	Vertex[2].v=vector3(-fSize,fSize,-fSize);
	Vertex[3].v=vector3(fSize,fSize,-fSize);
	pd3dDevice->SetTexture(0,m_SkyTexture[2].GetTexture());
	pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,
		0,4,2,Indices,D3DFMT_INDEX16,Vertex, sizeof(LVertex));	

	Vertex[0].v=vector3(-fSize,0.0f,-fSize);
	Vertex[1].v=vector3(-fSize,0.0f,fSize);
	Vertex[2].v=vector3(-fSize,fSize,fSize);
	Vertex[3].v=vector3(-fSize,fSize,-fSize);
	pd3dDevice->SetTexture(0,m_SkyTexture[3].GetTexture());
	pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,
		0,4,2,Indices,D3DFMT_INDEX16,Vertex, sizeof(LVertex));	

	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,TRUE);	
}

void CSkyScene::RenderCloud(LPDIRECT3DDEVICE8 pd3dDevice)
{
	matrix matWorld;
	pd3dDevice->GetTransform(D3DTS_WORLD,(D3DMATRIX*)(&matWorld));
	vector3 *vecViewPos=CSceneManager::GetCamera()->GetPosition();
	matrix mat;
	mat.Translation(vector3(vecViewPos->x,0.0f,vecViewPos->z));

	mat=mat*matWorld;

	pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&mat);
	pd3dDevice->SetTexture(0,NULL);
	pd3dDevice->SetTexture(1,NULL);
	

	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,TRUE);
	pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0x11);	
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);	
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,TRUE);

	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);	    
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );	
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
	

	pd3dDevice->SetVertexShader(LVERTEXFVF);

	//
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);		
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
    pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);			
	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,D3DTOP_SELECTARG2);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	pd3dDevice->SetTexture(0,m_CloudTexture.GetTexture());	
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,TRUE);

	pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);		

	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,SKYDIVIDE,m_CloudVertex[0],sizeof(LVertex));	
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,SKYDIVIDE,m_CloudVertex[1],sizeof(LVertex));	

	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,TRUE);
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,SKYDIVIDE,m_CloudVertex[2],sizeof(LVertex));	

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);	
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0xee);
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);
}

void CSkyScene::Update(float fUpdate)
{
	float fDivAng=(3.141597f*2.0f)/SKYDIVIDE;
	float fCloudFlow[3];
	fCloudFlow[0]=0.0002f;
	fCloudFlow[1]=0.00008f;
	fCloudFlow[2]=0.00006f;
	
	for(int cCloud=0;cCloud<MAX_CLOUD;cCloud++)
	{		
		for(int i=0;i<SKYVERTEX;i++)
		{			
			m_CloudVertex[cCloud][i].tu+=fCloudFlow[cCloud]*fUpdate;
			m_CloudVertex[cCloud][i].tv+=fCloudFlow[cCloud]*fUpdate;					
		}
	}
}

void CSkyScene::RenderForGlare(LPDIRECT3DDEVICE8 pd3dDevice)
{
	matrix matWorld;
	pd3dDevice->GetTransform(D3DTS_WORLD,(D3DMATRIX*)(&matWorld));
	vector3 *vecViewPos=CSceneManager::GetCamera()->GetPosition();
	matrix mat;
	mat.Translation(vector3(vecViewPos->x,0.0f,vecViewPos->z));
	mat=mat*matWorld;
	pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&mat);
	pd3dDevice->SetTexture(0,NULL);
	pd3dDevice->SetTexture(1,NULL);

	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,TRUE);

	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);	    
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );	
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);


	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_CURRENT);    
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);		

	DWORD dwColor[3];
	dwColor[0]=0xff222222;
	dwColor[1]=0xff222222;
	dwColor[2]=0xff222222;

	SetColor(CSceneManager::m_WeatherManager.m_InterGlareSkyLower.c,CSceneManager::m_WeatherManager.m_InterGlareSkyUpper.c,dwColor);
	//SetColor(0xffffffff,0xffffffff,dwColor);

	//SetColor(0xff000000,0xff000000,dwColor);	
	vector3 vecSunPos=vector3(1.0f,0.0f,1.0f);
	vecSunPos.Normalize();

	color clrRate;
	clrRate.c=0xff222222;

	vector3 vecPos;
	/*
	for(int i=0;i<SKYDIVIDE;i++)
	{		
		vecPos=m_SkyVertex[0].v;
		vecPos.Normalize();		
		float fRate=vecPos*vecSunPos;
		if(fRate<=0.0f)
			fRate=0.0f;

		fRate*=3.0f;
		m_SkyVertex[i].diff.r=fRate*clrRate.r;
		m_SkyVertex[i].diff.g=fRate*clrRate.g;
		m_SkyVertex[i].diff.b=fRate*clrRate.b;
		m_SkyVertex[i].diff.a=0xff;
		
	}
	*/
	/*
		m_SkyVertex[0].diff.c=dwColor;	
		m_SkyVertex[0].diff.c=dwColor;	
		m_SkyVertex[0].diff.c=dwColor;	
		m_SkyVertex[0].diff.c=dwColor;	
		m_SkyVertex[0].diff.c=dwColor;	
		m_SkyVertex[0].diff.c=dwColor;	
		m_SkyVertex[0].diff.c=dwColor;	
		m_SkyVertex[0].diff.c=dwColor;	
		m_SkyVertex[0].diff.c=dwColor;	
		*/

	
	pd3dDevice->SetVertexShader(LVERTEXFVF);
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,SKYDIVIDE,m_SkyVertex,sizeof(LVertex));	


	pd3dDevice->SetTexture(0,m_CloudTexture.GetTexture());	

	pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
	pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);			
	//pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR,0xffffffff);

	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);	
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);	

	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,D3DBLEND_INVSRCALPHA);
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND,D3DBLEND_SRCALPHA);
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,  TRUE);

	/*
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);

	//pd3dDevice->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_SUBTRACT);	
	

	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,SKYDIVIDE,m_CloudVertex[0],sizeof(LVertex));		

	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,SKYDIVIDE,m_CloudVertex[1],sizeof(LVertex));	
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,SKYDIVIDE,m_CloudVertex[2],sizeof(LVertex));		

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,  FALSE);

	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

	//pd3dDevice->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);
	*/



}
