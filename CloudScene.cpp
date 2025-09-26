// CloudScene.cpp: implementation of the CCloudScene class.
//
//////////////////////////////////////////////////////////////////////

#include "CloudScene.h"
#include "SceneManager.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCloudScene::CCloudScene()
{

}

CCloudScene::~CCloudScene()
{

}

void CCloudScene::Create()
{	

	m_fTrans[0]=0.0f;
	m_fTrans[1]=0.0f;
	m_fTrans[2]=0.0f;
	m_fTrans[3]=0.0f;

	
	m_CloudOctaveBlendTexture.Create(SHADOWSIZE,SHADOWSIZE);
	float fDivAng=(3.141597f*2.0f)/CLOUDDIVIDE;
	
	m_CloudVertex[0].v=vector3(0.0f,100000.0f,0.0f);
	m_CloudVertex[0].spec.c=0x0;
	m_CloudVertex[0].diff.c=0xffffffff;
	m_CloudVertex[0].tu=0.0f;
	m_CloudVertex[0].tv=0.0f;
	for(int i=1;i<CLOUDVERTEX;i++)
	{
		m_CloudVertex[i].v.x=(CLOUDSIZE)*sin(-fDivAng*(i-1));
		m_CloudVertex[i].v.z=(CLOUDSIZE)*cos(-fDivAng*(i-1));
		m_CloudVertex[i].v.y=10000.0f;
		m_CloudVertex[i].diff.c=0xffffffff;
		m_CloudVertex[i].tu=(0.7f)*sin(-fDivAng*(i-1));
		m_CloudVertex[i].tv=(0.7f)*cos(-fDivAng*(i-1));
		m_CloudVertex[i].spec.c=0x0;
	}
	/*
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
			m_CloudVertex[cCloud][i].tu=(0.7f)*sin(-fDivAng*(i-1));
			m_CloudVertex[cCloud][i].tv=(0.7f)*cos(-fDivAng*(i-1));		
			m_CloudVertex[cCloud][i].spec.c=0x0;		
			
		}
	}	
	*/
	int nSize=32;
	for(int i=0;i<NUMCLOUDOCTAVE;i++)
	{
		m_CloudOctaveTexture[i].CreateEmpty(nSize,nSize,D3DFMT_A4R4G4B4);
		nSize*=2;
	}	
	nSize=32;
	unsigned char ucIntensity;
	WORD wColor;
	for(int i=0;i<NUMCLOUDOCTAVE;i++)
	{
		WORD *pTexture=(WORD*)m_CloudOctaveTexture[i].Lock();
		for(int y=0;y<nSize;y++)
		{
			for(int x=0;x<nSize;x++)
			{
				ucIntensity=(1.0f+Noise3D(x,y,0))*8;
				wColor=0xf000;
				wColor|=ucIntensity<<8;
				wColor|=ucIntensity<<4;
				wColor|=ucIntensity;
				pTexture[x+y*nSize]=wColor;
				//unsigned char intensity = (char)((1.0f+Noise3D(x,y,iTime)) * 128);
			}
		}
		m_CloudOctaveTexture[i].Unlock();
		nSize*=2;
	}	
}

inline float CCloudScene::Noise3D(int x, int y, int z)
{
	int n = x + y * 57 + z * 131;
    n = (n<<13) ^ n;
    return ( 1.0f - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) 
		* 0.000000000931322574615478515625f);
}

void CCloudScene::Render(LPDIRECT3DDEVICE8 pd3dDevice)
{	
	vector3 *vecViewPos=CSceneManager::GetCamera()->GetPosition();

	matrix mat;
	mat.Translation(vector3(vecViewPos->x,0.0f,vecViewPos->z));
	pd3dDevice->SetTransform(D3DTS_WORLD,mat);

	pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_CURRENT);        
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE|D3DTA_COMPLEMENT);

	//pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SUBTRACT );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,D3DTOP_ADD);
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,D3DTOP_SELECTARG);
    //pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SUBTRACT);
	pd3dDevice->SetVertexShader(LVERTEXFVF);

	//pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);
	
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,D3DBLEND_ONE);
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND,D3DBLEND_SRCCOLOR);
	//	
	//pd3dDevice->SetRenderState( D3DRS_SRCBLEND,D3DBLEND_SRCCOLOR);
    //pd3dDevice->SetRenderState( D3DRS_DESTBLEND,D3DBLEND_INVSRCCOLOR);	
	
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
	
	for(int i=0;i<CLOUDVERTEX;i++)
	{
		m_CloudVertex[i].diff.c=0xff0000ff;
	}
	pd3dDevice->SetTexture(0,m_CloudOctaveBlendTexture.GetTexture());
	pd3dDevice->SetTexture(1,NULL);
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,CLOUDVERTEX,m_CloudVertex,sizeof(LVertex));	
	//pd3dDevice->SetRenderState( D3DRS_FOGENABLE,TRUE);
}

void CCloudScene::Update(LPDIRECT3DDEVICE8 pd3dDevice,float fTime)
{
	TLVertex pVertex[4];		
	float fTextureSize=SHADOWSIZE;

	pVertex[0].v.x=0.0f;
	pVertex[1].v.x=0.0f;
	pVertex[2].v.x=fTextureSize;
	pVertex[3].v.x=fTextureSize;

	pVertex[1].v.y=0.0f;
	pVertex[3].v.y=0.0f;
	pVertex[0].v.y=fTextureSize;
	pVertex[2].v.y=fTextureSize;		

		
	pVertex[0].tu=0.0f;
	pVertex[1].tu=0.0f;

	pVertex[3].tu=1.0f;
	pVertex[2].tu=1.0f;

	pVertex[1].tv=0.0f;
	pVertex[3].tv=0.0f;

	pVertex[0].tv=1.0f;
	pVertex[2].tv=1.0f;		

	for(int i=0;i<4;i++)
	{
		pVertex[i].w=0.1f;
		pVertex[i].v.z=0.1f;
		pVertex[i].Diffuse.c=0xffffffff;
		pVertex[i].Specular.c=0x0;
	}
	static DWORD c=0;
	c++;

	m_CloudOctaveBlendTexture.Begin(pd3dDevice);
	pd3dDevice->Clear(0,NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET,0x000000ff, 1.0f, 0);
	pd3dDevice->BeginScene();	
	
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_CURRENT);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );

	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);

	pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_DISABLE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU);	

	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE);
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE);	

	pd3dDevice->SetTexture(1,NULL);	
	
	pVertex[0].tu=0.0f+m_fTrans[0];
	pVertex[1].tu=0.0f+m_fTrans[0];

	pVertex[3].tu=1.0f+m_fTrans[0];
	pVertex[2].tu=1.0f+m_fTrans[0];

	pVertex[1].tv=0.0f+m_fTrans[0];
	pVertex[3].tv=0.0f+m_fTrans[0];

	pVertex[0].tv=1.0f+m_fTrans[0];
	pVertex[2].tv=1.0f+m_fTrans[0];

	pVertex[0].Diffuse.c=pVertex[1].Diffuse.c=pVertex[2].Diffuse.c=pVertex[3].Diffuse.c=0xffffffff;

	pd3dDevice->SetVertexShader(TLVERTEXFVF);
	pd3dDevice->SetTexture(0,m_CloudOctaveTexture[0].GetTexture());
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(TLVertex));	

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);

	pVertex[0].tu=0.0f+m_fTrans[1];
	pVertex[1].tu=0.0f+m_fTrans[1];

	pVertex[3].tu=1.0f+m_fTrans[1];
	pVertex[2].tu=1.0f+m_fTrans[1];

	pVertex[1].tv=0.0f+m_fTrans[1];
	pVertex[3].tv=0.0f+m_fTrans[1];

	pVertex[0].tv=1.0f+m_fTrans[1];
	pVertex[2].tv=1.0f+m_fTrans[1];

	pVertex[0].Diffuse.c=pVertex[1].Diffuse.c=pVertex[2].Diffuse.c=pVertex[3].Diffuse.c=0xff7f7f7f;

	pd3dDevice->SetTexture(0,m_CloudOctaveTexture[1].GetTexture());
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(TLVertex));	
	
	pVertex[0].tu=0.0f+m_fTrans[2];
	pVertex[1].tu=0.0f+m_fTrans[2];

	pVertex[3].tu=1.0f+m_fTrans[2];
	pVertex[2].tu=1.0f+m_fTrans[2];

	pVertex[1].tv=0.0f+m_fTrans[2];
	pVertex[3].tv=0.0f+m_fTrans[2];

	pVertex[0].tv=1.0f+m_fTrans[2];
	pVertex[2].tv=1.0f+m_fTrans[2];

	pVertex[0].Diffuse.c=pVertex[1].Diffuse.c=pVertex[2].Diffuse.c=pVertex[3].Diffuse.c=0xff3f3f3f;
	pd3dDevice->SetTexture(0,m_CloudOctaveTexture[2].GetTexture());
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(TLVertex));
	

	pVertex[0].tu=0.0f+m_fTrans[3];
	pVertex[1].tu=0.0f+m_fTrans[3];

	pVertex[3].tu=1.0f+m_fTrans[3];
	pVertex[2].tu=1.0f+m_fTrans[3];

	pVertex[1].tv=0.0f+m_fTrans[3];
	pVertex[3].tv=0.0f+m_fTrans[3];

	pVertex[0].tv=1.0f+m_fTrans[3];
	pVertex[2].tv=1.0f+m_fTrans[3];

	pVertex[0].Diffuse.c=pVertex[1].Diffuse.c=pVertex[2].Diffuse.c=pVertex[3].Diffuse.c=0xff1f1f1f;
	pd3dDevice->SetTexture(0,m_CloudOctaveTexture[3].GetTexture());
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(TLVertex));	
	//	
	pd3dDevice->EndScene();
	m_CloudOctaveBlendTexture.End(pd3dDevice);

	m_fTrans[0]+=0.00001f;	
	m_fTrans[1]+=0.00004f;
	m_fTrans[2]+=0.00008f;
	m_fTrans[3]+=0.00010f;	

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,TRUE);
}

void CCloudScene::SetColor(DWORD dwColor, DWORD dwCenterColor, DWORD *dwLayerColor)
{
	m_CloudVertex[0].diff.c=dwCenterColor;
	for(int i=1;i<CLOUDVERTEX;i++)
	{		
		m_CloudVertex[i].diff.c=dwColor;
	}
}
