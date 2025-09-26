// SunScene.cpp: implementation of the CSunScene class.
//
//////////////////////////////////////////////////////////////////////

#include "SunScene.h"
#include "SceneManager.h"
#include <BaseGraphicsLayer.h>
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSunScene::CSunScene()
{
	///*
	m_vecSunPosition.x=-1.0f;
	m_vecSunPosition.y=1.0f;
	m_vecSunPosition.z=1.0f;
	//*/
	/*
	m_vecSunPosition.x=0.0f;
	m_vecSunPosition.y=0.0f;
	m_vecSunPosition.z=1.0f;
	//*/
	m_vecSunPosition.Normalize();

	m_fFlarePos[0]=-0.4f;
	m_fFlarePos[1]=0.2f;
	m_fFlarePos[2]=0.4f;	
	m_fFlarePos[3]=0.8f;

	m_fFlarePos[4]=0.85f;	

	m_fFlarePos[5]=1.5f;
	m_fFlarePos[6]=1.8f;

	m_fFlareSize[0]=60.0f;
	m_fFlareSize[1]=80.0f;
	m_fFlareSize[2]=120.0f;
	m_fFlareSize[3]=80.0f;

	m_fFlareSize[4]=135.0f;

	m_fFlareSize[5]=120.0f;
	m_fFlareSize[6]=300.0f;

	m_FlareColor[0].a=20;
	m_FlareColor[0].r=255;
	m_FlareColor[0].g=150;
	m_FlareColor[0].b=150;

	m_FlareColor[1].a=60;
	m_FlareColor[1].r=140;
	m_FlareColor[1].g=230;
	m_FlareColor[1].b=255;

	m_FlareColor[2].a=60;
	m_FlareColor[2].r=255;
	m_FlareColor[2].g=255;
	m_FlareColor[2].b=255;

	m_FlareColor[3].a=35;
	m_FlareColor[3].r=140;
	m_FlareColor[3].g=140;
	m_FlareColor[3].b=230;

	m_FlareColor[4].a=20;
	m_FlareColor[4].r=255;
	m_FlareColor[4].g=140;
	m_FlareColor[4].b=255;

	m_FlareColor[5].a=40;
	m_FlareColor[5].r=255;
	m_FlareColor[5].g=100;
	m_FlareColor[5].b=100;

	m_FlareColor[6].a=35;
	m_FlareColor[6].r=255;
	m_FlareColor[6].g=200;
	m_FlareColor[6].b=225;

	m_LastFlareColor[0].a=15;
	m_LastFlareColor[0].r=255;
	m_LastFlareColor[0].g=150;
	m_LastFlareColor[0].b=150;

	m_LastFlareColor[1].a=15;
	m_LastFlareColor[1].r=150;
	m_LastFlareColor[1].g=150;
	m_LastFlareColor[1].b=255;

	
}

CSunScene::~CSunScene()
{
	m_SunTexture.Unload();
	m_SunFlareTexture.Unload();
	m_FlareTexture[0].Unload();
	m_FlareTexture[1].Unload();
	m_FlareTexture[2].Unload();
	m_FlareTexture[3].Unload();
	m_FlareTexture[4].Unload();
	m_FlareTexture[5].Unload();
	m_FlareTexture[6].Unload();

	m_MoonTexture.Unload();
	m_MoonFlareTexture.Unload();
}

void CSunScene::Reload()
{
	m_SunTexture.Unload();
	m_SunFlareTexture.Unload();
	m_FlareTexture[0].Unload();
	m_FlareTexture[1].Unload();
	m_FlareTexture[2].Unload();
	m_FlareTexture[3].Unload();
	m_FlareTexture[4].Unload();
	m_FlareTexture[5].Unload();
	m_FlareTexture[6].Unload();

	m_MoonTexture.Unload();
	m_MoonFlareTexture.Unload();


	CTexture::SetPath(NATURETEXTUREPATH);
	m_SunTexture.Load("sun.dds");
	m_SunFlareTexture.Load("sunflare.dds");
	m_FlareTexture[0].Load("Flare0.dds");
	m_FlareTexture[1].Load("Flare1.dds");
	m_FlareTexture[2].Load("Flare2.dds");
	m_FlareTexture[3].Load("Flare3.dds");
	m_FlareTexture[4].Load("Flare4.dds");
	m_FlareTexture[5].Load("Flare5.dds");
	m_FlareTexture[6].Load("Flare6.dds");	

	m_MoonTexture.Load("moon.dds");
	m_MoonFlareTexture.Load("moonflare.dds");	
}

void CSunScene::Create()
{
	//LogMessage("SunScene Create");
	m_SunVertex[0].tu=0.0f;
	m_SunVertex[0].tv=1.0f;
	m_SunVertex[0].Diffuse.c=0xffffffff;
	m_SunVertex[0].Specular.c=0x0;
	
	m_SunVertex[1].tu=0.0f;
	m_SunVertex[1].tv=0.0f;
	m_SunVertex[1].Diffuse.c=0xffffffff;
	m_SunVertex[1].Specular.c=0x0;
	
	m_SunVertex[2].tu=1.0f;
	m_SunVertex[2].tv=1.0f;
	m_SunVertex[2].Diffuse.c=0xffffffff;
	m_SunVertex[2].Specular.c=0x0;
	
	m_SunVertex[3].tu=1.0f;
	m_SunVertex[3].tv=0.0f;
	m_SunVertex[3].Diffuse.c=0xffffffff;
	m_SunVertex[3].Specular.c=0x0;

	
	
	CTexture::SetPath(NATURETEXTUREPATH);
	m_SunTexture.Load("sun.dds");
	m_SunFlareTexture.Load("sunflare.dds");
	m_FlareTexture[0].Load("Flare0.dds");
	m_FlareTexture[1].Load("Flare1.dds");
	m_FlareTexture[2].Load("Flare2.dds");
	m_FlareTexture[3].Load("Flare3.dds");
	m_FlareTexture[4].Load("Flare4.dds");
	m_FlareTexture[5].Load("Flare5.dds");
	m_FlareTexture[6].Load("Flare6.dds");	

	m_MoonTexture.Load("moon.dds");
	m_MoonFlareTexture.Load("moonflare.dds");	

	//LogMessage("SunScene Create Complete");
}

void CSunScene::Render(LPDIRECT3DDEVICE8 pd3dDevice)
{	
	matrix matWorld;
	//pd3dDevice->GetTransform(D3DTS_WORLD,(D3DMATRIX*)(&matWorld));
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_CURRENT );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE  );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );


	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
    pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE);	
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);

	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);

	pd3dDevice->SetVertexShader(TLVERTEXFVF);	
	pd3dDevice->SetTexture(1,NULL);	
	//matrix *matViewPosition=(matrix*)CSceneManager::GetCamera()->GetMatPosition();
	//pd3dDevice->GetTransform(D3DTS_WORLD,(D3DMATRIX*)(&matWorld));
	matWorld.MakeIdent();
	vector3 vecViewPos=(*CSceneManager::GetCamera()->GetPosition());
	//vector3 vecViewPos;
	/*
	vecViewPos.x=matViewPosition->_41;
	vecViewPos.y=matViewPosition->_42;
	vecViewPos.z=matViewPosition->_43;
	*/
	
	// edith 2009.08.26 태양 렌더링
	// Sun Rendering.& Calc Real Sun position.//
	vector3 vecRealSunPosition,vecProjectedPosition;	
	vector3 vecSunPro;
	float fProjectedPositionW;
	vecRealSunPosition=vecViewPos+m_vecSunPosition*fSun_Length;
	matrix matTrans;
	matTrans.Translation(vecRealSunPosition);
	matTrans=matTrans*matWorld;
	vecRealSunPosition=matTrans.GetLoc();
	bool isInScreenSun;
	matrix matIdent;
	matIdent.MakeIdent();
	int ProjectionMode;

	if(matIdent==matWorld)	
		ProjectionMode=0;
	else
		ProjectionMode=1;

	if(m_vecSunPosition.y<0.0f)
		vecRealSunPosition.y=-vecRealSunPosition.y;

	isInScreenSun=BaseGraphicsLayer::TransformVector(vecRealSunPosition,vecProjectedPosition,fProjectedPositionW);	

	vecSunPro=vecProjectedPosition;
	
	float sx=75.0f;
	float sy=75.0f;
	
	if(m_vecSunPosition.y>=0.0f)
	{	
		m_SunVertex[0].v=vecProjectedPosition;
		m_SunVertex[1].v=vecProjectedPosition;
		m_SunVertex[2].v=vecProjectedPosition;
		m_SunVertex[3].v=vecProjectedPosition;	
		
		m_SunVertex[0].v.x-=sx;m_SunVertex[0].v.y+=sy;
		m_SunVertex[1].v.x-=sx;m_SunVertex[1].v.y-=sy;
		m_SunVertex[2].v.x+=sx;m_SunVertex[2].v.y+=sy;
		m_SunVertex[3].v.x+=sx;m_SunVertex[3].v.y-=sy;	
		
		m_SunVertex[0].w=fProjectedPositionW;
		m_SunVertex[1].w=fProjectedPositionW;
		m_SunVertex[2].w=fProjectedPositionW;
		m_SunVertex[3].w=fProjectedPositionW;
		
		m_SunVertex[0].Diffuse.c=0xaaffffff;
		m_SunVertex[1].Diffuse.c=0xaaffffff;
		m_SunVertex[2].Diffuse.c=0xaaffffff;
		m_SunVertex[3].Diffuse.c=0xaaffffff;

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE);	
		
		pd3dDevice->SetTexture(0,m_SunTexture.GetTexture());
		pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,m_SunVertex,sizeof(TLVertex));
		
	}
	else
	{		
		float sx=60.0f;
		float sy=60.0f;
		m_SunVertex[0].v=vecProjectedPosition;
		m_SunVertex[1].v=vecProjectedPosition;
		m_SunVertex[2].v=vecProjectedPosition;
		m_SunVertex[3].v=vecProjectedPosition;
		
		m_SunVertex[0].v.x-=sx;m_SunVertex[0].v.y+=sy;
		m_SunVertex[1].v.x-=sx;m_SunVertex[1].v.y-=sy;
		m_SunVertex[2].v.x+=sx;m_SunVertex[2].v.y+=sy;
		m_SunVertex[3].v.x+=sx;m_SunVertex[3].v.y-=sy;	
		
		m_SunVertex[0].w=fProjectedPositionW;
		m_SunVertex[1].w=fProjectedPositionW;
		m_SunVertex[2].w=fProjectedPositionW;
		m_SunVertex[3].w=fProjectedPositionW;
		
		m_SunVertex[0].Diffuse.c=0xaaffffff;
		m_SunVertex[1].Diffuse.c=0xaaffffff;
		m_SunVertex[2].Diffuse.c=0xaaffffff;
		m_SunVertex[3].Diffuse.c=0xaaffffff;
		
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );

		float fRate=fabsf(m_vecSunPosition.y);
		fRate+=0.2f;

		unsigned char Alpha;
		if(fRate>=1.0f)
			Alpha=255;
		else
		{
			if(fRate-0.5f<=0.0f)			
				Alpha=0;			
			else
				Alpha=(unsigned char)((fRate-0.5f)*512.0f);
			//Alpha=(unsigned char)(fRate*255.0f);
		}

		m_SunVertex[0].Diffuse.a=
		m_SunVertex[1].Diffuse.a=
		m_SunVertex[2].Diffuse.a=
		m_SunVertex[3].Diffuse.a=Alpha;	

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);	
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		
		pd3dDevice->SetTexture(0,m_MoonTexture.GetTexture());
		pd3dDevice->SetTexture(1,NULL);
		pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,m_SunVertex,sizeof(TLVertex));

		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE);	

		pd3dDevice->SetTexture(0,m_MoonFlareTexture.GetTexture());
		//pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,m_SunVertex,sizeof(TLVertex));
	}
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,TRUE);
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);   
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
}

void CSunScene::RenderFlare(LPDIRECT3DDEVICE8 pd3dDevice)
{
	//if(LENSFLARE==0 || m_vecSunPosition.y<0.0f)
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE);	


	matrix matWorld;
	//pd3dDevice->GetTransform(D3DTS_WORLD,(D3DMATRIX*)(&matWorld));
	matWorld.MakeIdent();
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_CURRENT );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);	
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE  );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );


	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
    pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE);	
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);

	pd3dDevice->SetVertexShader(TLVERTEXFVF);	
	pd3dDevice->SetTexture(1,NULL);	
	matrix *matViewPosition=(matrix*)CSceneManager::GetCamera()->GetMatPosition();
	vector3 vecViewPos=(*CSceneManager::GetCamera()->GetPosition());
	//vector3 vecViewPos;
	/*
	vecViewPos.x=matViewPosition->_41;
	vecViewPos.y=matViewPosition->_42;
	vecViewPos.z=matViewPosition->_43;
	*/
	
	// Sun Rendering.& Calc Real Sun position.//
	vector3 vecRealSunPosition,vecProjectedPosition;	
	vector3 vecSunPro;
	float fProjectedPositionW;
	vecRealSunPosition=vecViewPos+m_vecSunPosition*fSun_Length;
	matrix matTrans;
	matTrans.Translation(vecRealSunPosition);
	matTrans=matTrans*matWorld;
	vecRealSunPosition=matTrans.GetLoc();
	bool isInScreenSun;
	matrix matIdent;
	matIdent.MakeIdent();
	int ProjectionMode;

	if(matIdent==matWorld)	
		ProjectionMode=0;
	else
		ProjectionMode=1;

	if(m_vecSunPosition.y<0.0f)
		vecRealSunPosition.y=-vecRealSunPosition.y;

	isInScreenSun=BaseGraphicsLayer::TransformVector(vecRealSunPosition,vecProjectedPosition,fProjectedPositionW);	


	if(isInScreenSun)
	{
		if(vecProjectedPosition.z>=1.0f)		
			isInScreenSun=false;	
	}


	vecSunPro=vecProjectedPosition;
	
	float sx=75.0f;
	float sy=75.0f;
	/*
	
	if(m_vecSunPosition.y<0.0f)
	{

		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,TRUE);
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);    	
		return;
	}
	*/

	vecProjectedPosition.z=vecProjectedPosition.z/fProjectedPositionW;
	if( vecProjectedPosition.z<0.0f ||
		vecProjectedPosition.z>1.0f )
	{
		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,TRUE);
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);    	
		return;
	}

	/*
	if( vecProjectedPosition.x+5 < 0 ||
		vecProjectedPosition.x-5 > BaseGraphicsLayer::m_lScreenSx ||
		vecProjectedPosition.y+5 < 0 ||
		vecProjectedPosition.y+5 > BaseGraphicsLayer::m_lScreenSy)
	{
		
	}
	*/
	vecProjectedPosition.x;
	vecProjectedPosition.y;

	//	is Sun Drawed BackBuffer ? 
	RECT rect;
	rect.left=vecProjectedPosition.x;
	rect.right=vecProjectedPosition.x+1;
	rect.top=vecProjectedPosition.y;
	rect.bottom=vecProjectedPosition.y+1;	
	


//	D3DLOCKED_RECT d3dlocked_rect;
	D3DSURFACE_DESC BackDesc;

	LPDIRECT3DSURFACE8 pBackBuffer;
	pd3dDevice->GetBackBuffer(0,D3DBACKBUFFER_TYPE_MONO,&pBackBuffer);	
	// Buffer Description Get
	pBackBuffer->GetDesc(&BackDesc);

	//pBackBuffer->LockRect(&d3dlocked_rect,&rect,D3DLOCK_READONLY);	
	//pBackBuffer->LockRect(&d3dlocked_rect,&rect,D3DLOCK_NO_DIRTY_UPDATE);
	
	static bool isInScreenSunPer = false;
	static DWORD sunTick = timeGetTime();

	float fRate=0.0f;

	// edith 2009.08.26 태양 플레어 시간별로 밝기 조절하기.
	float fTime = CSceneManager::GetWeatherTime();
	if(6.0f <= fTime && fTime < 7.0f || 
		17.0f <= fTime && fTime < 18.0f) 
		fRate = 40.0;
	else if(7.0f <= fTime && fTime < 8.0f || 
		16.0f <= fTime && fTime < 17.0f) 
		fRate = 50.0;
	else if(8.0f <= fTime && fTime < 9.0f || 
		15.0f <= fTime && fTime < 16.0f) 
		fRate = 60.0;
	else if(9.0f <= fTime && fTime < 10.0f || 
		14.0f <= fTime && fTime < 15.0f)  
		fRate = 70.0;
	else if(10.0f <= fTime && fTime < 11.0f || 
		13.0f <= fTime && fTime < 14.0f) 
		fRate = 80.0;
	else if(11.0f <= fTime && fTime < 13.0f) 
		fRate = 90.0;

/*
	if(isInScreenSun == false)
	{
		isInScreenSunPer = isInScreenSun;
		fRate = 0.0f;
	}
	else if((isInScreenSun && isInScreenSunPer != isInScreenSun) || (abs((int)timeGetTime()-(int)sunTick) > 1000))
	{
		fRate = 0.0f;
		isInScreenSunPer = isInScreenSun;
		sunTick = timeGetTime();
		for(int iy=-5;iy<5;iy++)
		{
			for(int ix=-5;ix<5;ix++)
			{
				rect.left=(int)vecProjectedPosition.x+ix;
				rect.right=(int)vecProjectedPosition.x+ix+1;
				rect.top=(int)vecProjectedPosition.y+iy;
				rect.bottom=(int)vecProjectedPosition.y+iy+1;
				// 저 해상도 월드 크리에이터 작업시 Back Buffer size를 over 하여
				// rect를 얻으려는 경우에 프로그램 종료되는 버그 수정
				// 03.06.19 wizardbug
				if( rect.left>=0 && (unsigned int)rect.left<BackDesc.Width &&
					rect.top>=0 && (unsigned int)rect.top<BackDesc.Height &&
					rect.right>=0 && (unsigned int)rect.right<BackDesc.Width &&
					rect.bottom>=0 && (unsigned int)rect.bottom<BackDesc.Height
					)
				{
					memset(&d3dlocked_rect,0,sizeof(d3dlocked_rect));
					pBackBuffer->LockRect(&d3dlocked_rect,&rect,D3DLOCK_READONLY);

					WORD *e=(WORD*)d3dlocked_rect.pBits;
					if(*e>=0xaaaa)				
						fRate+=1.0f;				
					pBackBuffer->UnlockRect();
				}
			}
		}	
	}
*/
	bool isSunRendering=false;	
	//oat fRate=((double)dwSunSize)/((double)0xffff);
	//fRate*=5.5f;
	if(fRate>=100.0f)
	{
		fRate=100.0f;
	}
	if(fRate>50.0f )
	{
		isSunRendering=true;
	}
		
	pBackBuffer->Release();		
	//	

	vector3 vecFlarePos;
	vector3 vecFlareSunPos;
	vector3 vecInterSunPos;
	vector3 vecEachFlarePos;
	// calculate for Flare-ViewFront position
	matrix matFlare;
	matFlare.Translation(vector3(0.0f,0.0f,fSun_FrontEye));
	matFlare=matFlare*(*matViewPosition);
	vecFlarePos=matFlare.GetLoc();
	if(m_vecSunPosition.y<0.0f)
	{
		vector3 vecMoonPos=m_vecSunPosition;
		vecMoonPos.y=-vecMoonPos.y;
		vecFlareSunPos=vecViewPos+vecMoonPos*fSun_FrontEye;		
	}
	else
	{
		vecFlareSunPos=vecViewPos+m_vecSunPosition*fSun_FrontEye;
	}
	
	vecInterSunPos=vecFlarePos-vecFlareSunPos;	
	vector3 vecNorFlareSunPos=vecFlareSunPos-vecViewPos;
	vector3 vecNorFlarePos=vecFlarePos-vecViewPos;
	vecNorFlareSunPos.Normalize();
	vecNorFlarePos.Normalize();
	float fFlareSize=vecNorFlareSunPos*vecNorFlarePos;
	float fFlareColor=vecNorFlareSunPos*vecNorFlarePos;
	float fSunSize=fFlareSize;

	fFlareColor*=(fRate/100.0f);

	fFlareSize=sqrtf( (1.0f-(fFlareSize*fFlareSize)) );
	fFlareSize+=0.6f;

	// edith 2009.08.26 태양 플레어 크기
	fSunSize=1.0f/( (fSunSize-1.4f)*(fSunSize-1.4f) );
	sx=180.0f*fSunSize;
	sy=180.0f*fSunSize;
	static float fRotateSunFlare[3]={0.0f,0.0f,0.0f};
	fRotateSunFlare[0]+=0.001f;
	fRotateSunFlare[1]-=0.0008f;
	fRotateSunFlare[2]+=0.0006f;

	pd3dDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_ALWAYS);

	//if(LENSFLARE>=3)
	
		// First Sun Flare//

	if(m_vecSunPosition.y<0.0f)
	{
//		float fTime = CSceneManager::GetWeatherTime();
		if((fTime > 20.0f) || (fTime < 4.0f))
		{
			sx=400;
			sy=400;
			pd3dDevice->SetTexture(0,m_MoonFlareTexture.GetTexture());	
			m_SunVertex[0].v=vecProjectedPosition;
			m_SunVertex[1].v=vecProjectedPosition;
			m_SunVertex[2].v=vecProjectedPosition;
			m_SunVertex[3].v=vecProjectedPosition;
			
			m_SunVertex[0].v.x+=sx*cosf(0.0f+fRotateSunFlare[0]);
			m_SunVertex[0].v.y+=sy*sinf(0.0f+fRotateSunFlare[0]);

			m_SunVertex[1].v.x+=sx*cosf(3.1415f*0.5f+fRotateSunFlare[0]);
			m_SunVertex[1].v.y+=sy*sinf(3.1415f*0.5f+fRotateSunFlare[0]);

			m_SunVertex[2].v.x+=sx*cosf(3.1415f*1.5f+fRotateSunFlare[0]);
			m_SunVertex[2].v.y+=sy*sinf(3.1415f*1.5f+fRotateSunFlare[0]);

			m_SunVertex[3].v.x+=sx*cosf(3.1415f+fRotateSunFlare[0]);
			m_SunVertex[3].v.y+=sy*sinf(3.1415f+fRotateSunFlare[0]);

			m_SunVertex[0].w=fProjectedPositionW;
			m_SunVertex[1].w=fProjectedPositionW;
			m_SunVertex[2].w=fProjectedPositionW;
			m_SunVertex[3].w=fProjectedPositionW;
			/////////////////////////
			/*		03.9.2 수정
			color FlareColor;
			FlareColor=color::Interpolation(color(0x0,0x0,0x0,0x0),color(0x99,0x99,0x99,0x99),1.0f);
			
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);	

			m_SunVertex[0].Diffuse.c=FlareColor.c;
			m_SunVertex[1].Diffuse.c=FlareColor.c;
			m_SunVertex[2].Diffuse.c=FlareColor.c;
			m_SunVertex[3].Diffuse.c=FlareColor.c;
			*/
			pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR,0xffffffff);

			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR);

			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X);	
			
			m_SunVertex[0].Diffuse.c=0xffffffff;
			m_SunVertex[1].Diffuse.c=0xffffffff;
			m_SunVertex[2].Diffuse.c=0xffffffff;
			m_SunVertex[3].Diffuse.c=0xffffffff;
			
			
			pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_DESTCOLOR);
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE);	
			////////////////////////
			pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,m_SunVertex,sizeof(TLVertex));	
			////////////////////////
			pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
			pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE);	
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
			///////////////////////
		}
	}
	else
	{
		sx+=rand()%30;
		sy+=rand()%30;
		pd3dDevice->SetTexture(0,m_SunFlareTexture.GetTexture());	
		m_SunVertex[0].v=vecProjectedPosition;
		m_SunVertex[1].v=vecProjectedPosition;
		m_SunVertex[2].v=vecProjectedPosition;
		m_SunVertex[3].v=vecProjectedPosition;
		
		m_SunVertex[0].v.x+=sx*cosf(0.0f+fRotateSunFlare[0]);
		m_SunVertex[0].v.y+=sy*sinf(0.0f+fRotateSunFlare[0]);

		m_SunVertex[1].v.x+=sx*cosf(3.1415f*0.5f+fRotateSunFlare[0]);
		m_SunVertex[1].v.y+=sy*sinf(3.1415f*0.5f+fRotateSunFlare[0]);

		m_SunVertex[2].v.x+=sx*cosf(3.1415f*1.5f+fRotateSunFlare[0]);
		m_SunVertex[2].v.y+=sy*sinf(3.1415f*1.5f+fRotateSunFlare[0]);

		m_SunVertex[3].v.x+=sx*cosf(3.1415f+fRotateSunFlare[0]);
		m_SunVertex[3].v.y+=sy*sinf(3.1415f+fRotateSunFlare[0]);

		m_SunVertex[0].w=fProjectedPositionW;
		m_SunVertex[1].w=fProjectedPositionW;
		m_SunVertex[2].w=fProjectedPositionW;
		m_SunVertex[3].w=fProjectedPositionW;	

		color FlareColor;
		FlareColor=color::Interpolation(color(0x0,0x0,0x0,0x0),color(0xaa,0xaa,0xaa,0xaa),fFlareColor);

		m_SunVertex[0].Diffuse.c=FlareColor.c;
		m_SunVertex[1].Diffuse.c=FlareColor.c;
		m_SunVertex[2].Diffuse.c=FlareColor.c;
		m_SunVertex[3].Diffuse.c=FlareColor.c;
		

		pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,m_SunVertex,sizeof(TLVertex));	

		// edith 2009.08.26 태양 2차 3차 밝게 렌더링
		// 2'nd Sun Flare//
		sx+=rand()%30;
		sy+=rand()%30;
		m_SunVertex[0].v=vecProjectedPosition;
		m_SunVertex[1].v=vecProjectedPosition;
		m_SunVertex[2].v=vecProjectedPosition;
		m_SunVertex[3].v=vecProjectedPosition;			

		m_SunVertex[0].v.x+=sx*cosf(0.0f+fRotateSunFlare[1]);
		m_SunVertex[0].v.y+=sy*sinf(0.0f+fRotateSunFlare[1]);

		m_SunVertex[1].v.x+=sx*cosf(3.1415f*0.5f+fRotateSunFlare[1]);
		m_SunVertex[1].v.y+=sy*sinf(3.1415f*0.5f+fRotateSunFlare[1]);

		m_SunVertex[2].v.x+=sx*cosf(3.1415f*1.5f+fRotateSunFlare[1]);
		m_SunVertex[2].v.y+=sy*sinf(3.1415f*1.5f+fRotateSunFlare[1]);

		m_SunVertex[3].v.x+=sx*cosf(3.1415f+fRotateSunFlare[1]);
		m_SunVertex[3].v.y+=sy*sinf(3.1415f+fRotateSunFlare[1]);	
		
		m_SunVertex[0].w=fProjectedPositionW;
		m_SunVertex[1].w=fProjectedPositionW;
		m_SunVertex[2].w=fProjectedPositionW;
		m_SunVertex[3].w=fProjectedPositionW;
		
		m_SunVertex[0].Diffuse.c=0x18444444;
		m_SunVertex[1].Diffuse.c=0x18444444;
		m_SunVertex[2].Diffuse.c=0x18444444;
		m_SunVertex[3].Diffuse.c=0x18444444;

		pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,m_SunVertex,sizeof(TLVertex));	
/*
		// 3'nd Sun Flare//
		sx-=rand()%30;
		sy-=rand()%30;
		m_SunVertex[0].v=vecProjectedPosition;
		m_SunVertex[1].v=vecProjectedPosition;
		m_SunVertex[2].v=vecProjectedPosition;
		m_SunVertex[3].v=vecProjectedPosition;			

		m_SunVertex[0].v.x+=sx*cosf(0.0f+fRotateSunFlare[2]);
		m_SunVertex[0].v.y+=sy*sinf(0.0f+fRotateSunFlare[2]);

		m_SunVertex[1].v.x+=sx*cosf(3.1415f*0.5f+fRotateSunFlare[2]);
		m_SunVertex[1].v.y+=sy*sinf(3.1415f*0.5f+fRotateSunFlare[2]);

		m_SunVertex[2].v.x+=sx*cosf(3.1415f*1.5f+fRotateSunFlare[2]);
		m_SunVertex[2].v.y+=sy*sinf(3.1415f*1.5f+fRotateSunFlare[2]);

		m_SunVertex[3].v.x+=sx*cosf(3.1415f+fRotateSunFlare[2]);
		m_SunVertex[3].v.y+=sy*sinf(3.1415f+fRotateSunFlare[2]);	
		
		m_SunVertex[0].w=fProjectedPositionW;
		m_SunVertex[1].w=fProjectedPositionW;
		m_SunVertex[2].w=fProjectedPositionW;
		m_SunVertex[3].w=fProjectedPositionW;
		
		m_SunVertex[0].Diffuse.c=0x18444444;
		m_SunVertex[1].Diffuse.c=0x18444444;
		m_SunVertex[2].Diffuse.c=0x18444444;
		m_SunVertex[3].Diffuse.c=0x18444444;

		pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,m_SunVertex,sizeof(TLVertex));
*/
	}
		/*
		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,TRUE);
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);    	
		return;
		*/
	

	pd3dDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);

	//if(isInScreenSun ==false || isSunRendering==false || vecRealSunPosition.y <=10000.0f )
	//if(isInScreenSun ==false || isSunRendering==false || vecRealSunPosition.y <=10000.0f )
	if(isInScreenSun ==false || vecRealSunPosition.y <=10000.0f || m_vecSunPosition.y<0.0f)
	{
		pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,TRUE);
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);    	
		return;
	}	

	// edith 2009.08.26 태양에서 빛이 나오는 원 모양	
	static long LastFlareColor=0;
	LastFlareColor++;
	LastFlareColor%=(2);

	if(fRate>20.0f)
	{
		for(int cFlare=0;cFlare<MAX_FLARE;cFlare++)
		{
			vecEachFlarePos=vecFlareSunPos+vecInterSunPos*m_fFlarePos[cFlare];
			BaseGraphicsLayer::TransformVector(vecEachFlarePos,vecProjectedPosition,fProjectedPositionW);			
			sx=m_fFlareSize[cFlare]*fFlareSize;
			sy=m_fFlareSize[cFlare]*fFlareSize;
			vecRealSunPosition.z=0.0f;
			m_SunVertex[0].v=vecProjectedPosition;
			m_SunVertex[1].v=vecProjectedPosition;
			m_SunVertex[2].v=vecProjectedPosition;
			m_SunVertex[3].v=vecProjectedPosition;		
			m_SunVertex[0].v.x-=sx;m_SunVertex[0].v.y+=sy;
			m_SunVertex[1].v.x-=sx;m_SunVertex[1].v.y-=sy;
			m_SunVertex[2].v.x+=sx;m_SunVertex[2].v.y+=sy;
			m_SunVertex[3].v.x+=sx;m_SunVertex[3].v.y-=sy;		
			m_SunVertex[0].v.z=0.1f;
			m_SunVertex[1].v.z=0.1f;
			m_SunVertex[2].v.z=0.1f;
			m_SunVertex[3].v.z=0.1f;
			m_SunVertex[0].Diffuse=m_FlareColor[cFlare];
			m_SunVertex[1].Diffuse=m_FlareColor[cFlare];
			m_SunVertex[2].Diffuse=m_FlareColor[cFlare];
			m_SunVertex[3].Diffuse=m_FlareColor[cFlare];

			if(cFlare==MAX_FLARE-1)
			{
				m_SunVertex[0].Diffuse=m_LastFlareColor[LastFlareColor];
				m_SunVertex[1].Diffuse=m_LastFlareColor[LastFlareColor];
				m_SunVertex[2].Diffuse=m_LastFlareColor[LastFlareColor];
				m_SunVertex[3].Diffuse=m_LastFlareColor[LastFlareColor];
			}		

			m_SunVertex[0].w=0.001f;
			m_SunVertex[1].w=0.001f;
			m_SunVertex[2].w=0.001f;
			m_SunVertex[3].w=0.001f;		
			pd3dDevice->SetTexture(0,m_FlareTexture[cFlare].GetTexture());		
			pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,m_SunVertex,sizeof(TLVertex));	
		}	
	}
	
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,TRUE);
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);   

}
