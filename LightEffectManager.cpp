// LightEffectManager.cpp: implementation of the CLightEffectManager class.
//
//////////////////////////////////////////////////////////////////////

#include "BaseDataDefine.h"
#include "LightEffectManager.h"
#include "BaseGraphicsLayer.h"
#include "SceneManager.h"
#include "RenderOption.h"

char strMakeZBufferPixelShader[]=	
"ps.1.1\n"
"tex t0\n"
//"mov r0,c0\n";
"dp3 r0,t0,c0\n";

char strDepthAlphaPixelShader[]=
"ps.1.1\n"
"def c0,1.0f,1.0f,1.0f,1.0f\n"
"tex t0\n"
"mov r0,t0\n"
"mov r0.a,c0\n";


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRenderTextureMipmap CLightEffectManager::m_GlareTexture;
CShadowMap CLightEffectManager::m_FullSceneTexture;
CRenderTextureMipmap CLightEffectManager::m_SpecularGlareTexture;
CRenderTexture CLightEffectManager::m_GlareCompositeTexture;	

CGlareTexture CLightEffectManager::m_SmoothTexture;	

LPDIRECT3DSURFACE8 CLightEffectManager::m_pTempRenderSurface;		
LPDIRECT3DSURFACE8 CLightEffectManager::m_pTempRenderZBuffer;	
int CLightEffectManager::m_nScreenSize;

List<CLightEffectManager::CGlareNode> CLightEffectManager::m_GlareList;
CTexture CLightEffectManager::m_CharcterEnvTexture;
LPDIRECT3DCUBETEXTURE8 CLightEffectManager::m_LightCubeTexture;

CRenderEnvTexture CLightEffectManager::m_DiffuseLightCubeTexture;
CRenderEnvTexture CLightEffectManager::m_SpecularLightTexture;

CRenderTexture CLightEffectManager::m_SpGlareTexture;
CRenderTexture CLightEffectManager::m_DOFZBuffer;

bool CLightEffectManager::m_FullSceneAnti=true;

DWORD CLightEffectManager::m_dwMakeZBufferPixelShader=0xffffffff;

CGlareTexture CLightEffectManager::m_DOFSmoothTexture;

DWORD CLightEffectManager::m_DepthAlphaShader=0xffffffff;
CRenderTexture CLightEffectManager::m_NearDOFTexture;

int CLightEffectManager::m_RenderSizeX=1024;
int CLightEffectManager::m_RenderSizeY=1024;

CLightEffectManager::CLightEffectManager()
{

}

CLightEffectManager::~CLightEffectManager()
{

}

void CLightEffectManager::Create(int nSize)
{
	if(CRenderOption::m_FullSceneGlare)
	{
		m_nScreenSize=nSize;	
		m_SmoothTexture.Create(128);
		//m_GlareTexture.Create(128,128);
		//m_SpecularGlareTexture.Create(128,128);
		m_GlareTexture.Create(m_nScreenSize,m_nScreenSize);
		m_SpecularGlareTexture.Create(m_nScreenSize,m_nScreenSize);
		m_FullSceneTexture.Create(m_nScreenSize,m_nScreenSize);
		
		
		
		//m_DOFZBuffer.Create(m_nScreenSize,m_nScreenSize);
		//m_DOFSmoothTexture.Create(m_nScreenSize>>1);
		/*		
		m_NearDOFTexture.Create(m_nScreenSize,m_nScreenSize);
		m_GlareCompositeTexture.Create(256,256);	

		//m_FullSceneAnti=true;

		CTexture::SetPath(NATURETEXTUREPATH);
		m_CharcterEnvTexture.Load("Env.dds");

		char strCubeTextureName[256];
		sprintf(strCubeTextureName,"%s\\%s",NATURETEXTUREPATH,"LightCubemap.dds");

		D3DXCreateCubeTextureFromFileExA(BaseGraphicsLayer::GetDevice(),
				strCubeTextureName, 
				D3DX_DEFAULT,
				0,
				0,
				D3DFMT_UNKNOWN,
				D3DPOOL_MANAGED,
				D3DX_FILTER_LINEAR,
				D3DX_FILTER_LINEAR,
				0,
				NULL,
				NULL,
				&m_LightCubeTexture);

		//sprintf(strCubeTextureName,"%s\\%s",NATURETEXTUREPATH,"LightCubemap.dds");

		m_DiffuseLightCubeTexture.Create(BaseGraphicsLayer::GetDevice(),"sky",NATURETEXTUREPATH);		
		m_SpecularLightTexture.Create(BaseGraphicsLayer::GetDevice(),"sp_light",NATURETEXTUREPATH,false);

		m_SpGlareTexture.Create(256,256);		


		LPD3DXBUFFER pCode;	
		D3DXAssembleShader(strMakeZBufferPixelShader,strlen(strMakeZBufferPixelShader),0,NULL,&pCode,NULL);										 
		BaseGraphicsLayer::GetDevice()->CreatePixelShader((DWORD*)pCode->GetBufferPointer(),&m_dwMakeZBufferPixelShader);    
		pCode->Release();		

		D3DXAssembleShader(strDepthAlphaPixelShader,strlen(strDepthAlphaPixelShader),0,NULL,&pCode,NULL);
		BaseGraphicsLayer::GetDevice()->CreatePixelShader((DWORD*)pCode->GetBufferPointer(),&m_DepthAlphaShader);    
		pCode->Release();				
		*/
	}
}

void CLightEffectManager::PrepareLightingEnvmap(LPDIRECT3DDEVICE8 pd3dDevice)
{
	if(CRenderOption::m_FullSceneGlare)
	{
		/*
		pd3dDevice->EndScene();
		m_DiffuseLightCubeTexture.RenderCubeMap(pd3dDevice);	
		pd3dDevice->BeginScene();	

		pd3dDevice->EndScene();	
		m_SpGlareTexture.Begin(pd3dDevice);
		pd3dDevice->BeginScene();	
		pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET,0x0,1.0f, 0);	

		D3DXMATRIX matProj;

		D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/2.0f , 1.0f, 100.0f, 2000.0f );

		D3DXMATRIX matOldView,matOldProjection;
		pd3dDevice->GetTransform(D3DTS_VIEW,&matOldView);
		pd3dDevice->GetTransform(D3DTS_PROJECTION,&matOldProjection);	

		D3DXMATRIX matViewDir( matOldView);
		matViewDir._41 = 0.0f; matViewDir._42 = 0.0f; matViewDir._43 = 0.0f;

		
		D3DXMATRIX matView;	
		matView =matViewDir;
		//matView = DD3DUtil_GetCubeMapViewMatrix( (D3DCUBEMAP_FACES) 1 );
		//D3DXMatrixMultiply( &matView, &matViewDir, &matView );
		//D3DXMatrixMultiply( &matView, &matView ,&matViewDir );

		

		pd3dDevice->SetTransform(D3DTS_VIEW,&matView);
		pd3dDevice->SetTransform(D3DTS_PROJECTION,&matProj);
		
		m_SpecularLightTexture.RenderSkymap(pd3dDevice);
		//m_DiffuseLightCubeTexture.RenderSkymap(m_pd3dDevice);

		vector3 vecSunPos;
		vector3 vecProjPos;
		float w;
		vecSunPos=vector3(-1000.0f,-500.0f,-1000.0f);
		bool isInScreenSun=BaseGraphicsLayer::TransformVector(vecSunPos,vecProjPos,w);

		TLVertex m_SunVertex[4];

		float sx=100.0f;
		float sy=100.0f;
		m_SunVertex[0].v=vecProjPos;
		m_SunVertex[1].v=vecProjPos;
		m_SunVertex[2].v=vecProjPos;
		m_SunVertex[3].v=vecProjPos;
			
		m_SunVertex[0].v.x-=sx;m_SunVertex[0].v.y+=sy;
		m_SunVertex[1].v.x-=sx;m_SunVertex[1].v.y-=sy;
		m_SunVertex[2].v.x+=sx;m_SunVertex[2].v.y+=sy;
		m_SunVertex[3].v.x+=sx;m_SunVertex[3].v.y-=sy;	
			
		m_SunVertex[0].w=w;
		m_SunVertex[1].w=w;
		m_SunVertex[2].w=w;
		m_SunVertex[3].w=w;
		
		m_SunVertex[0].Diffuse.c=0xffffffff;
		m_SunVertex[1].Diffuse.c=0xffffffff;
		m_SunVertex[2].Diffuse.c=0xffffffff;
		m_SunVertex[3].Diffuse.c=0xffffffff;

		m_SunVertex[0].tu=0.0f;
		m_SunVertex[0].tv=1.0f;	
		m_SunVertex[0].Specular.c=0x0;
		
		m_SunVertex[1].tu=0.0f;
		m_SunVertex[1].tv=0.0f;	
		m_SunVertex[1].Specular.c=0x0;
		
		m_SunVertex[2].tu=1.0f;
		m_SunVertex[2].tv=1.0f;	
		m_SunVertex[2].Specular.c=0x0;
		
		m_SunVertex[3].tu=1.0f;
		m_SunVertex[3].tv=0.0f;	
		m_SunVertex[3].Specular.c=0x0;

			
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);

		//m_pd3dDevice->SetTexture(0,);

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);    
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);		
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
		pd3dDevice->SetVertexShader(TLVERTEXFVF);	

		pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);	

		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);

		pd3dDevice->SetTexture(0,CSceneManager::m_WeatherManager.m_SunScene.m_SunTexture.GetTexture());

		pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,m_SunVertex,sizeof(TLVertex));

		pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);		


		pd3dDevice->SetTransform(D3DTS_VIEW,&matOldView);
		pd3dDevice->SetTransform(D3DTS_PROJECTION,&matOldProjection);	
		pd3dDevice->EndScene();	
		m_SpGlareTexture.End(pd3dDevice);
		pd3dDevice->BeginScene();		

		pd3dDevice->EndScene();
		m_SpecularLightTexture.RenderCubeMap(pd3dDevice);	
		pd3dDevice->BeginScene();	
		*/
	
		RenderFullSceneTextureBegin(pd3dDevice);	
	}
}

void CLightEffectManager::RenderFullSceneTextureBegin(LPDIRECT3DDEVICE8 pd3dDevice)
{
	if(CRenderOption::m_FullSceneGlare)
	{
		pd3dDevice->GetRenderTarget(&m_pTempRenderSurface);
		pd3dDevice->GetDepthStencilSurface(&m_pTempRenderZBuffer);

		pd3dDevice->SetRenderTarget(m_FullSceneTexture.m_pShadowMapColorSurface,m_FullSceneTexture.m_pShadowMapZSurface);			

		D3DVIEWPORT8 viewData = { 0, 0, m_RenderSizeX, m_RenderSizeY, 0.0f, 1.0f };
		pd3dDevice->SetViewport(&viewData);		

		matrix matProj;
		float fAspect=(float)m_RenderSizeX/(float)m_RenderSizeY;
		fAspect=1.0f/fAspect;
		matProj.MakeProjection(3.1415f/3.0f,fAspect,40.0f,620000.0f);	
		//pd3dDevice->SetTransform(D3DTS_PROJECTION,matProj);

		pd3dDevice->Clear(0,NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,CSceneManager::m_WeatherManager.m_GF3ClareColor.c,1.0f,0);
	}
}

void CLightEffectManager::RenderGlareScene(LPDIRECT3DDEVICE8 pd3dDevice)
{
	if(CRenderOption::m_FullSceneGlare)
	{
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);
		D3DVIEWPORT8 viewData = { 0, 0, m_RenderSizeX, m_RenderSizeY, 0.0f, 1.0f };
		pd3dDevice->SetViewport(&viewData);	

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );		

		pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );

		pd3dDevice->SetTextureStageState( 2, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
		pd3dDevice->SetTextureStageState( 2, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );

		pd3dDevice->SetTextureStageState( 3, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
		pd3dDevice->SetTextureStageState( 3, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
		
		RenderGlareTexture(pd3dDevice);		
		RenderSpecularGlareTexture(pd3dDevice);
		RenderFullSceneGlare(pd3dDevice);
		//RenderDOFZBuffer(pd3dDevice);		
		RenderFullSceneTextureEnd(pd3dDevice);
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,TRUE);

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );		

		pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );

		pd3dDevice->SetTextureStageState( 2, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
		pd3dDevice->SetTextureStageState( 2, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );

		pd3dDevice->SetTextureStageState( 3, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
		pd3dDevice->SetTextureStageState( 3, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );

		/*
		RenderFullSceneAnti(pd3dDevice);		
		
		RenderDOFZBuffer(pd3dDevice);
		*/
	}
}

void CLightEffectManager::RenderGlareTexture(LPDIRECT3DDEVICE8 pd3dDevice)
{
	if(CRenderOption::m_FullSceneGlare)
	{		
		//pd3dDevice->EndScene();	

		LPDIRECT3DSURFACE8 m_pTempRenderSurface;
		LPDIRECT3DSURFACE8 m_pTempRenderZBuffer;	
		pd3dDevice->GetRenderTarget(&m_pTempRenderSurface);
		pd3dDevice->GetDepthStencilSurface(&m_pTempRenderZBuffer);

		pd3dDevice->SetRenderTarget(m_GlareTexture.GetSurface(),m_pTempRenderZBuffer);
		D3DVIEWPORT8 viewData = { 0, 0, m_RenderSizeX, m_RenderSizeY, 0.0f, 1.0f };
		pd3dDevice->SetViewport(&viewData);

		pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);
		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);

		////////////////////////////////////////////
		//pd3dDevice->BeginScene();
		pd3dDevice->Clear(0,NULL,D3DCLEAR_TARGET,0xff000000,1.0f,0);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,D3DTA_DIFFUSE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,D3DTOP_SELECTARG2);

		TLVertex pVertex[8];		
		float fTextureSize=128.0f;

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
			pVertex[i].Diffuse.c=0xff0000ff;
			pVertex[i].Specular.c=0xffffffff;
		}	

		pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);	

		pd3dDevice->SetTexture(1,NULL);
		pd3dDevice->SetVertexShader(TLVERTEXFVF);				
		//m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(TLVertex));	

		//m_CharacterManager.Render(m_pd3dDevice);

		CSceneManager::m_WeatherManager.m_SkyScene.RenderForGlare(pd3dDevice);	



		//m_pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);	
		/*
		
		for(int cGlareNode=0;cGlareNode<m_GlareList.num;cGlareNode++)
		{
			if(m_GlareList[cGlareNode].m_bCubeEnv)
			{			
				m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR|1);
				m_pd3dDevice->SetTextureStageState( 1,D3DTSS_TEXTURETRANSFORMFLAGS,D3DTTFF_COUNT3);

				m_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS,TRUE);
				m_pd3dDevice->SetRenderState(D3DRS_LOCALVIEWER,TRUE);

				m_pd3dDevice->SetTexture(1,m_DiffuseLightCubeTexture.GetTexture());

				//m_pd3dDevice->SetTexture(1,m_LightCubeTexture);			
				
				m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,D3DTOP_SELECTARG1);					

				m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,D3DTOP_SELECTARG1);
				m_pd3dDevice->SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
				m_pd3dDevice->SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
				m_pd3dDevice->SetTextureStageState(1, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);			

				m_pd3dDevice->SetTransform(D3DTS_WORLD,m_GlareList[cGlareNode].m_matPosition);
				
				m_GlareList[cGlareNode].m_pMeshObject->Render(m_pd3dDevice);			

				m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX,1);
				m_pd3dDevice->SetTextureStageState(1 ,D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

				m_pd3dDevice->SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
				m_pd3dDevice->SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
				m_pd3dDevice->SetTextureStageState(1, D3DTSS_ADDRESSW, D3DTADDRESS_WRAP);

				m_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS,FALSE);
				m_pd3dDevice->SetRenderState(D3DRS_LOCALVIEWER,FALSE);
				
			}
			else
			{

			}
		}
		*/
		
		
		m_GlareList.num=0;

		pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);

		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);	
		pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);	
		

		for(int ix=0;ix<CSceneManager::m_HeightField.GetLSizeX();ix++)
		{
			for(int iy=0;iy<CSceneManager::m_HeightField.GetLSizeY();iy++)
			{				
				if( CSceneManager::m_HeightField.m_SectorScene[ix+iy*CSceneManager::m_HeightField.GetLSizeX()].m_MapWater)
				{				
					CSceneManager::m_HeightField.m_SectorScene[ix+iy*CSceneManager::m_HeightField.GetLSizeX()].RenderWater(pd3dDevice,true);
				}			
			}
		}
		
		pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);	

		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

		//m_CharacterManager.RenderGlare(m_pd3dDevice);
		/*
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,TRUE);
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0xee);
		*/
		//m_pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);	


		//m_WeatherManager.Render(m_pd3dDevice);

		//pd3dDevice->EndScene();
		/////////////////////////////////////////////////
		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
		pd3dDevice->SetRenderTarget(m_pTempRenderSurface,m_pTempRenderZBuffer);		
		pd3dDevice->SetViewport(&viewData);	

		pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

		m_pTempRenderSurface->Release();
		m_pTempRenderZBuffer->Release();


		//pd3dDevice->BeginScene();
	}
}

void CLightEffectManager::RenderSpecularGlareTexture(LPDIRECT3DDEVICE8 pd3dDevice)
{
	if(CRenderOption::m_FullSceneSpecularGlare)
	{
		//pd3dDevice->EndScene();	

		LPDIRECT3DSURFACE8 m_pTempRenderSurface;		
		LPDIRECT3DSURFACE8 m_pTempRenderZBuffer;	
		pd3dDevice->GetRenderTarget(&m_pTempRenderSurface);
		pd3dDevice->GetDepthStencilSurface(&m_pTempRenderZBuffer);

		pd3dDevice->SetRenderTarget(m_SpecularGlareTexture.GetSurface(),m_pTempRenderZBuffer);	

		D3DVIEWPORT8 viewData = { 0, 0, m_RenderSizeX, m_RenderSizeY, 0.0f, 1.0f };
		pd3dDevice->SetViewport(&viewData);			

		pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);

		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);

		////////////////////////////////////////////
		//pd3dDevice->BeginScene();
		pd3dDevice->Clear(0,NULL,D3DCLEAR_TARGET,0xff000000,1.0f,0);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,D3DTA_DIFFUSE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,D3DTOP_SELECTARG2);

		TLVertex pVertex[8];		
		float fTextureSize=128.0f;

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
			pVertex[i].Diffuse.c=0xff0000ff;
			pVertex[i].Specular.c=0xffffffff;
		}	

		pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);	

		pd3dDevice->SetTexture(1,NULL);
		pd3dDevice->SetVertexShader(TLVERTEXFVF);				
		//m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(TLVertex));	

		//m_CharacterManager.Render(m_pd3dDevice);
		

		//m_pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);			
			
		pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);	

		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

		//pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
		
		if(CRenderOption::m_CharacterPerPixelSpecularLighting)
			CSceneManager::m_CharacterManager.RenderGlare(pd3dDevice);
		
		if(CRenderOption::m_AllObjectBump)
			CSceneManager::m_HeightField.RenderGlare(pd3dDevice);

		/*
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,TRUE);
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0xee);
		*/
		//m_pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);	


		//m_WeatherManager.Render(m_pd3dDevice);

		//pd3dDevice->EndScene();

		pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);			

		for(int ix=0;ix<CSceneManager::m_HeightField.GetLSizeX();ix++)
		{
			for(int iy=0;iy<CSceneManager::m_HeightField.GetLSizeY();iy++)
			{				
				if( CSceneManager::m_HeightField.m_SectorScene[ix+iy*CSceneManager::m_HeightField.GetLSizeX()].m_MapWater)
				{				
					//CSceneManager::m_HeightField.m_SectorScene[ix+iy*CSceneManager::m_HeightField.GetLSizeX()].RenderWater(pd3dDevice,true);
				}			
			}
		}		
		pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);	

		/////////////////////////////////////////////////
		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
		pd3dDevice->SetRenderTarget(m_pTempRenderSurface,m_pTempRenderZBuffer);		

		m_pTempRenderSurface->Release();
		m_pTempRenderZBuffer->Release();


		//pd3dDevice->BeginScene();
	}
}

void CLightEffectManager::RenderFullSceneGlare(LPDIRECT3DDEVICE8 pd3dDevice)
{
	if(CRenderOption::m_FullSceneGlare)
	{		
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);	
		m_SmoothTexture.GenerateGlareTexture2(pd3dDevice,m_GlareTexture.GetTexture(),vector3(0.25f,0.3f,0.3f),6);

		float fTextureSize=256.0f;
		
		TLVertex pVertex[4];	
		pVertex[0].v.x=0.0f;
		pVertex[1].v.x=0.0f;
		pVertex[2].v.x=(float)fTextureSize;
		pVertex[3].v.x=(float)fTextureSize;

		pVertex[1].v.y=0.0f;
		pVertex[3].v.y=0.0f;
		pVertex[0].v.y=(float)fTextureSize;
		pVertex[2].v.y=(float)fTextureSize;

		static float fasdf=0.0f;
			
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
		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);    
		//pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);		
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);

		pd3dDevice->SetTexture(1,NULL);
		pd3dDevice->SetVertexShader(TLVERTEXFVF);

		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,D3DBLEND_ONE);
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,D3DBLEND_ONE);
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);			
		pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

		pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);		
		
		pVertex[2].v.x=(float)m_RenderSizeX;
		pVertex[3].v.x=(float)m_RenderSizeX;
			
		pVertex[0].v.y=(float)m_RenderSizeY;
		pVertex[2].v.y=(float)m_RenderSizeY;				
		

		pVertex[0].Diffuse.c=pVertex[1].Diffuse.c=pVertex[2].Diffuse.c=pVertex[3].Diffuse.c
				=0xffffffff;
		
		pd3dDevice->SetTexture(0,m_SmoothTexture.GetTexture());

//		pd3dDevice->SetTexture(0,m_GlareTexture.GetTexture());

		
		
		pd3dDevice->SetTexture(1,NULL);
		//D3DVIEWPORT8 viewData = { 0, 0, 800, 600, 0.0f, 1.0f };
		//pd3dDevice->SetViewport(&viewData);		

		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
		pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);		

		pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(TLVertex));

		if(CRenderOption::m_FullSceneSpecularGlare)
		{
			
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);    
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);		
			pd3dDevice->SetRenderState( D3DRS_SRCBLEND,D3DBLEND_ONE);
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND,D3DBLEND_ONE);
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);				

			pVertex[0].Diffuse.c=pVertex[1].Diffuse.c=pVertex[2].Diffuse.c=pVertex[3].Diffuse.c
					=0xffffffff;

			D3DVIEWPORT8 viewData = { 0, 0, m_RenderSizeX, m_RenderSizeY, 0.0f, 1.0f };
			pd3dDevice->SetViewport(&viewData);		

			pd3dDevice->SetTexture(0,m_SpecularGlareTexture.GetTexture());	
			//m_pd3dDevice->SetTexture(0,m_GlareTexture.GetTexture(2));

			pVertex[0].tu=0.0f;
			pVertex[1].tu=0.0f;

			pVertex[3].tu=m_RenderSizeX/1024.0f;
			pVertex[2].tu=m_RenderSizeX/1024.0f;

			pVertex[1].tv=0.0f;
			pVertex[3].tv=0.0f;

			pVertex[0].tv=m_RenderSizeY/1024.0f;
			pVertex[2].tv=m_RenderSizeY/1024.0f;		

			pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(TLVertex));
			
			m_SmoothTexture.GenerateGlareTexture2(pd3dDevice,m_SpecularGlareTexture.GetTexture(),vector3(0.35f,0.35f,0.35f),5);
			
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);	

			pVertex[0].Diffuse.c=pVertex[1].Diffuse.c=pVertex[2].Diffuse.c=pVertex[3].Diffuse.c
					=0xff333333;

			pd3dDevice->SetTexture(0,m_SmoothTexture.GetTexture());	
			//m_pd3dDevice->SetTexture(0,m_GlareTexture.GetTexture(2));

			//pd3dDevice->SetTexture(0,m_SpecularGlareTexture.GetTexture());

			pd3dDevice->SetTexture(1,NULL);
			pd3dDevice->SetVertexShader(TLVERTEXFVF);			
			pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
			pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);

			pVertex[0].tu=0.0f;
			pVertex[1].tu=0.0f;

			pVertex[3].tu=1.0f;
			pVertex[2].tu=1.0f;

			pVertex[1].tv=0.0f;
			pVertex[3].tv=0.0f;

			pVertex[0].tv=1.0f;
			pVertex[2].tv=1.0f;		

			pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(TLVertex));
			
			
		}
		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);			

		/*
		for(int ix=0;ix<30;ix++)
		{
			for(int iy=0;iy<1;iy++)
			{
				pVertex[0].v.x=0.0f+ix-15.0f;
				pVertex[1].v.x=0.0f+ix-15.0f;
				pVertex[2].v.x=(float)fTextureSize+ix-15.0f;
				pVertex[3].v.x=(float)fTextureSize+ix-15.0f;

				pVertex[1].v.y=0.0f+iy;
				pVertex[3].v.y=0.0f+iy;
				pVertex[0].v.y=(float)fTextureSize+iy;
				pVertex[2].v.y=(float)fTextureSize+iy;

				m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(TLVertex));
			}
		}
		*/

		////////////////////////

		//m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(TLVertex));	


		pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);	
		pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	}
}

void CLightEffectManager::RenderFullSceneAnti(LPDIRECT3DDEVICE8 pd3dDevice)
{
	if(CRenderOption::m_FullSceneGlare)
	{
		if(!m_FullSceneAnti)
			return;
		pd3dDevice->SetRenderTarget(m_pTempRenderSurface,m_pTempRenderZBuffer);

		m_pTempRenderSurface->Release();
		m_pTempRenderZBuffer->Release();
		
		TLVertex pVertex[4];	

		pVertex[0].v.x=0.0f;
		pVertex[1].v.x=0.0f;
		pVertex[2].v.x=(float)BaseGraphicsLayer::m_lScreenSx;
		pVertex[3].v.x=(float)BaseGraphicsLayer::m_lScreenSx;

		pVertex[1].v.y=0.0f;
		pVertex[3].v.y=0.0f;
		pVertex[0].v.y=(float)BaseGraphicsLayer::m_lScreenSy;
		pVertex[2].v.y=(float)BaseGraphicsLayer::m_lScreenSy;

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
			pVertex[i].Specular.c=0xffffffff;
		}

		pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
		pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);    
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);		
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);

		pd3dDevice->SetTexture(1,NULL);
		pd3dDevice->SetVertexShader(TLVERTEXFVF);

		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,D3DBLEND_ONE);
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,D3DBLEND_ONE);
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);	

		color VertexColor;

		for(int cTexture=0;cTexture<9;cTexture++)	
		{
			VertexColor.r=(unsigned char)(32.0f/(float)8.0f);		
			VertexColor.a=VertexColor.b=VertexColor.g=VertexColor.r;
			VertexColor.a=255;
			////////				
			pVertex[0].Diffuse.c=pVertex[1].Diffuse.c=pVertex[2].Diffuse.c=pVertex[3].Diffuse.c
				=VertexColor.c;
			////////
			pd3dDevice->SetTexture(0,m_FullSceneTexture.m_pShadowMapColorTexture);

			switch(cTexture)
			{
				case 0:
					pVertex[0].Diffuse.c=pVertex[1].Diffuse.c=pVertex[2].Diffuse.c=pVertex[3].Diffuse.c
						=0xffeeeeee;
					pVertex[0].v.x=0.0f;
					pVertex[1].v.x=0.0f;				
					pVertex[2].v.x=(float)BaseGraphicsLayer::m_lScreenSx;
					pVertex[3].v.x=(float)BaseGraphicsLayer::m_lScreenSx;
					pVertex[1].v.y=0.0f;
					pVertex[3].v.y=0.0f;
					pVertex[0].v.y=(float)BaseGraphicsLayer::m_lScreenSy;
					pVertex[2].v.y=(float)BaseGraphicsLayer::m_lScreenSy;
					break;
				case 1:
					pVertex[0].v.x=-1;
					pVertex[1].v.x=-1;
					pVertex[2].v.x=(float)BaseGraphicsLayer::m_lScreenSx-1;
					pVertex[3].v.x=(float)BaseGraphicsLayer::m_lScreenSx-1;
					pVertex[1].v.y=-1;
					pVertex[3].v.y=-1;
					pVertex[0].v.y=(float)BaseGraphicsLayer::m_lScreenSy-1;
					pVertex[2].v.y=(float)BaseGraphicsLayer::m_lScreenSy-1;
					break;
				case 2:
					pVertex[0].v.x=0;
					pVertex[1].v.x=0;
					pVertex[2].v.x=(float)BaseGraphicsLayer::m_lScreenSx;
					pVertex[3].v.x=(float)BaseGraphicsLayer::m_lScreenSx;
					pVertex[1].v.y=-1;
					pVertex[3].v.y=-1;
					pVertex[0].v.y=(float)BaseGraphicsLayer::m_lScreenSy-1;
					pVertex[2].v.y=(float)BaseGraphicsLayer::m_lScreenSy-1;
					break;				
				case 3:
					pVertex[0].v.x=1;
					pVertex[1].v.x=1;
					pVertex[2].v.x=(float)BaseGraphicsLayer::m_lScreenSx+1;
					pVertex[3].v.x=(float)BaseGraphicsLayer::m_lScreenSx+1;
					pVertex[1].v.y=-1;
					pVertex[3].v.y=-1;
					pVertex[0].v.y=(float)BaseGraphicsLayer::m_lScreenSy-1;
					pVertex[2].v.y=(float)BaseGraphicsLayer::m_lScreenSy-1;
					break;
					////////////
				case 4:
					pVertex[0].v.x=-1;
					pVertex[1].v.x=-1;
					pVertex[2].v.x=(float)BaseGraphicsLayer::m_lScreenSx-1;
					pVertex[3].v.x=(float)BaseGraphicsLayer::m_lScreenSx-1;
					pVertex[1].v.y=0;
					pVertex[3].v.y=0;
					pVertex[0].v.y=(float)BaseGraphicsLayer::m_lScreenSy;
					pVertex[2].v.y=(float)BaseGraphicsLayer::m_lScreenSy;
					break;
				case 5:
					pVertex[0].v.x=1;
					pVertex[1].v.x=1;
					pVertex[2].v.x=(float)BaseGraphicsLayer::m_lScreenSx+1;
					pVertex[3].v.x=(float)BaseGraphicsLayer::m_lScreenSx+1;
					pVertex[1].v.y=0;
					pVertex[3].v.y=0;
					pVertex[0].v.y=(float)BaseGraphicsLayer::m_lScreenSy;
					pVertex[2].v.y=(float)BaseGraphicsLayer::m_lScreenSy;
					break;
					///
				case 6:
					pVertex[0].v.x=-1;
					pVertex[1].v.x=-1;
					pVertex[2].v.x=(float)BaseGraphicsLayer::m_lScreenSx-1;
					pVertex[3].v.x=(float)BaseGraphicsLayer::m_lScreenSx-1;
					pVertex[1].v.y=1;
					pVertex[3].v.y=1;
					pVertex[0].v.y=(float)BaseGraphicsLayer::m_lScreenSy+1;
					pVertex[2].v.y=(float)BaseGraphicsLayer::m_lScreenSy+1;
					break;
				case 7:
					pVertex[0].v.x=0;
					pVertex[1].v.x=0;
					pVertex[2].v.x=(float)BaseGraphicsLayer::m_lScreenSx;
					pVertex[3].v.x=(float)BaseGraphicsLayer::m_lScreenSx; 
					pVertex[1].v.y=1;
					pVertex[3].v.y=1;
					pVertex[0].v.y=(float)BaseGraphicsLayer::m_lScreenSy+1;
					pVertex[2].v.y=(float)BaseGraphicsLayer::m_lScreenSy+1;
					break;
				case 8:
					pVertex[0].v.x=1;
					pVertex[1].v.x=1;
					pVertex[2].v.x=(float)BaseGraphicsLayer::m_lScreenSx+1;
					pVertex[3].v.x=(float)BaseGraphicsLayer::m_lScreenSx+1;
					pVertex[1].v.y=1;
					pVertex[3].v.y=1;
					pVertex[0].v.y=(float)BaseGraphicsLayer::m_lScreenSy+1;
					pVertex[2].v.y=(float)BaseGraphicsLayer::m_lScreenSy+1;
					break;

			}
			pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(TLVertex));	
		}
		pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);	
	}
}

void CLightEffectManager::RenderFullSceneTextureEnd(LPDIRECT3DDEVICE8 pd3dDevice)
{
	if(CRenderOption::m_FullSceneGlare)
	{
		//if(m_FullSceneAnti)
		//	return;		

		pd3dDevice->SetRenderTarget(m_pTempRenderSurface,m_pTempRenderZBuffer);
		m_pTempRenderSurface->Release();
		m_pTempRenderZBuffer->Release();
		
		TLVertex pVertex[4];	

		pVertex[0].v.x=0.0f;
		pVertex[1].v.x=0.0f;
		pVertex[2].v.x=(float)BaseGraphicsLayer::m_lScreenSx;
		pVertex[3].v.x=(float)BaseGraphicsLayer::m_lScreenSx;

		pVertex[1].v.y=0.0f;
		pVertex[3].v.y=0.0f;
		pVertex[0].v.y=(float)BaseGraphicsLayer::m_lScreenSy;
		pVertex[2].v.y=(float)BaseGraphicsLayer::m_lScreenSy;

		static float fasdf=0.0f;

		pVertex[0].tu=0.0f;
		pVertex[1].tu=0.0f;

		pVertex[1].tv=0.0f;
		pVertex[3].tv=0.0f;

		pVertex[3].tu=1.0f;
		pVertex[2].tu=1.0f;

		
		pVertex[3].tu=m_RenderSizeX/1024.0f;
		pVertex[2].tu=m_RenderSizeX/1024.0f;			
		
		
		pVertex[0].tv=1.0f;
		pVertex[2].tv=1.0f;

		pVertex[0].tv=m_RenderSizeY/1024.0f;
		pVertex[2].tv=m_RenderSizeY/1024.0f;		

		for(int i=0;i<4;i++)
		{
			pVertex[i].w=0.1f;
			pVertex[i].v.z=0.1f;
			pVertex[i].Diffuse.c=0xffffffff;
			pVertex[i].Specular.c=0xffffffff;
		}
		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );    
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);		
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);

		if(CRenderOption::m_DepthOfField)
			pd3dDevice->SetTexture(0,m_DOFZBuffer.GetTexture());
		else
			pd3dDevice->SetTexture(0,m_FullSceneTexture.m_pShadowMapColorTexture);

		//D3DVIEWPORT8 viewData = { 0, 0, BaseGraphicsLayer::m_lScreenSx, BaseGraphicsLayer::m_lScreenSy, 0.0f, 1.0f };
		//pd3dDevice->SetViewport(&viewData);		

		pd3dDevice->SetRenderState(D3DRS_ZENABLE,D3DZB_FALSE);
		pd3dDevice->SetTexture(1,NULL);
		pd3dDevice->SetVertexShader(TLVERTEXFVF);

		pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);
		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);	

		pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(TLVertex));	

		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
		pd3dDevice->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);
	}
}

void CLightEffectManager::RenderDOFZBuffer(LPDIRECT3DDEVICE8 pd3dDevice)
{
	if(CRenderOption::m_DepthOfField)
	{			
		//pd3dDevice->EndScene();
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);
		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
		
		LPDIRECT3DSURFACE8 pTempRenderSurface;
		LPDIRECT3DSURFACE8 pTempRenderZBuffer;	
		pd3dDevice->GetRenderTarget(&pTempRenderSurface);
		pd3dDevice->GetDepthStencilSurface(&pTempRenderZBuffer);		

		pd3dDevice->SetRenderTarget(m_DOFZBuffer.GetSurface(),pTempRenderZBuffer);		

		D3DVIEWPORT8 viewData = { 0, 0, m_RenderSizeX, m_RenderSizeY, 0.0f, 1.0f };
		pd3dDevice->SetViewport(&viewData);
		
		//pd3dDevice->BeginScene();
		pd3dDevice->Clear(0,NULL,D3DCLEAR_TARGET,0xff000000,1.0f,0);

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,D3DTA_DIFFUSE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,D3DTOP_SELECTARG1);

		TLVertex pVertex[8];				

		pVertex[0].v.x=0.0f;
		pVertex[1].v.x=0.0f;
		pVertex[2].v.x=m_RenderSizeX;
		pVertex[3].v.x=m_RenderSizeX;

		pVertex[1].v.y=0.0f;
		pVertex[3].v.y=0.0f;
		pVertex[0].v.y=m_RenderSizeY;
		pVertex[2].v.y=m_RenderSizeY;	
			
		pVertex[0].tu=0.0f;
		pVertex[1].tu=0.0f;

		pVertex[3].tu=m_RenderSizeX/1024.0f;
		pVertex[2].tu=m_RenderSizeX/1024.0f;

		pVertex[1].tv=0.0f;
		pVertex[3].tv=0.0f;

		pVertex[0].tv=m_RenderSizeY/1024.0f;
		pVertex[2].tv=m_RenderSizeY/1024.0f;		

		static float fD=0.0f;
		fD+=0.003f;

		if(fD>=1.0f)
		{
			fD=0.0f;
		}

		for(int i=0;i<4;i++)
		{
			pVertex[i].w=1.0f;
			pVertex[i].v.z=0.0001f;
			pVertex[i].Diffuse.c=0xff0000ff;
			pVertex[i].Specular.c=0xffffffff;
		}

		pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);	

		pd3dDevice->SetTexture(1,NULL);
		pd3dDevice->SetVertexShader(TLVERTEXFVF);				
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);

		pd3dDevice->SetTexture(0,m_FullSceneTexture.GetShadowMapColorTexture());

		pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
		pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(TLVertex));		
		
		
		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,D3DTA_DIFFUSE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,D3DTOP_SELECTARG1);	

		//m_DOFSmoothTexture.GenerateGlareTexture(pd3dDevice,m_FullSceneTexture.GetShadowMapColorTexture(),vector3(0.25f,0.25f,0.25f),10);
		
		float m_fDepth[4];
		m_fDepth[0]=0.994f;
		m_fDepth[1]=0.995f;
		m_fDepth[2]=0.996f;
		m_fDepth[3]=0.998f;

		vector3 vecBlurRate;
		vecBlurRate=vector3(0.252f,0.253f,0.252f);

		pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

		pVertex[3].tu=1.0f;
		pVertex[2].tu=1.0f;

		pVertex[0].tv=1.0f;
		pVertex[2].tv=1.0f;

		
		for(int nDepth=0;nDepth<3;nDepth++)
		{						
			pVertex[0].v.z=pVertex[1].v.z=pVertex[2].v.z=pVertex[3].v.z=m_fDepth[nDepth];

			if(nDepth==0)
			{				
				m_DOFSmoothTexture.ProcedualGenerateGlareTexture(pd3dDevice,m_FullSceneTexture.GetShadowMapColorTexture(),vecBlurRate,1);
			}
			else
			{				
				m_DOFSmoothTexture.ProcedualGenerateGlareTexture(pd3dDevice,NULL,vecBlurRate,1);
			}

			pd3dDevice->SetVertexShader(TLVERTEXFVF);			
			pd3dDevice->SetTexture(0,m_DOFSmoothTexture.GetTexture());			
			pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(TLVertex));			
		}		
		
		float m_fNearDepth[3];
		
		m_fNearDepth[0]=0.85f;
		m_fNearDepth[1]=0.80f;
		m_fNearDepth[2]=0.68f;
		/*

		for(int nNearDepth=0;nNearDepth<3;nNearDepth++)
		{		
			pd3dDevice->EndScene();

			pd3dDevice->SetRenderTarget(m_NearDOFTexture.GetSurface(),pTempRenderZBuffer);		
			pd3dDevice->Clear(0,NULL,D3DCLEAR_TARGET,0x00000000,1.0f,0);

			pd3dDevice->SetRenderState(D3DRS_LASTPIXEL,FALSE);

			pd3dDevice->BeginScene();
			pd3dDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_GREATER);
			pVertex[0].v.z=pVertex[1].v.z=pVertex[2].v.z=pVertex[3].v.z=m_fNearDepth[nNearDepth];
			pd3dDevice->SetTexture(0,m_FullSceneTexture.GetShadowMapColorTexture());
			pd3dDevice->SetVertexShader(TLVERTEXFVF);
			pd3dDevice->SetPixelShader(m_DepthAlphaShader);		
			pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE,D3DCOLORWRITEENABLE_ALPHA|D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_RED);
			pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(TLVertex));		
			pd3dDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);	
			pd3dDevice->SetPixelShader(NULL);
			pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE,D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_RED);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,D3DTA_TEXTURE);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,D3DTA_DIFFUSE);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,D3DTOP_SELECTARG1);	
			pd3dDevice->SetRenderState(D3DRS_LASTPIXEL,TRUE);
			pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
			pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(TLVertex));
			pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);		
			pd3dDevice->EndScene();		


			pd3dDevice->SetRenderTarget(m_DOFZBuffer.GetSurface(),pTempRenderZBuffer);		
			pd3dDevice->BeginScene();

			m_DOFSmoothTexture.ProcedualGenerateGlareTexture(pd3dDevice,m_NearDOFTexture.GetTexture(),vecBlurRate,nNearDepth*10);

			pd3dDevice->SetRenderState( D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);	
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);	

			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,D3DTA_TEXTURE);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,D3DTA_DIFFUSE);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,D3DTOP_SELECTARG1);	
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,D3DTOP_DISABLE);	

			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );   
			pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

			pd3dDevice->SetTexture(0,m_DOFSmoothTexture.GetTexture());
			pd3dDevice->SetVertexShader(TLVERTEXFVF);		
			pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
			pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(TLVertex));		
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);			
			pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
		}
		*/
		////////////////////////near End
		/*
		m_DOFSmoothTexture.ProcedualGenerateGlareTexture(pd3dDevice,NULL,vecBlurRate,15);
		pd3dDevice->SetRenderState(D3DRS_ZBIAS,16);
		pd3dDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_GREATEREQUAL);
		pVertex[0].v.z=pVertex[1].v.z=pVertex[2].v.z=pVertex[3].v.z=0.8f;

		pd3dDevice->SetTexture(0,m_DOFSmoothTexture.GetTexture());
		pd3dDevice->SetVertexShader(TLVERTEXFVF);
		pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(TLVertex));
		pd3dDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
		pd3dDevice->SetRenderState(D3DRS_ZBIAS,0);
		*/

		



		//m_DOFSmoothTexture.ProcedualGenerateGlareTexture(pd3dDevice,NULL,vector3(0.25f,0.25f,0.25f),1);
		//m_DOFSmoothTexture.ProcedualGenerateGlareTexture(pd3dDevice,NULL,vector3(0.25f,0.25f,0.25f),1);

		//ProcedualGenerateGlareTexture(LPDIRECT3DDEVICE8 pd3dDevice, LPDIRECT3DBASETEXTURE8 pTexture, vector3 vecNeighbor, int nDepth)
		
		//pd3dDevice->SetTexture(0,m_DOFSmoothTexture.GetTexture());	
		
		
		
		pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);	

		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);	

		//pd3dDevice->EndScene();
		/////////////////////////////////////////////////
		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);

		pd3dDevice->SetRenderTarget(pTempRenderSurface,pTempRenderZBuffer);		
		//pd3dDevice->BeginScene();
	}
}
