// WeatherManager.cpp: implementation of the CWeatherManager class.
//
//////////////////////////////////////////////////////////////////////

#include "WeatherManager.h"
#include "BaseDataDefine.h"
#include "RenderOption.h"
#include "SectorScene.h"
#include "SceneManager.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeatherManager::CSubWeatherData::CSubWeatherData()
{
	m_pGrassTexture = NULL;
}
CWeatherManager::CSubWeatherData::~CSubWeatherData()
{
	if(m_pGrassTexture)
	{
		delete m_pGrassTexture;	
		m_pGrassTexture = NULL;
	}

}

CWeatherManager::CWeatherManager()
{
	ZeroMemory(&m_SunLight,sizeof(D3DLIGHT8));
	m_SunLight.Type=D3DLIGHT_DIRECTIONAL;    	
    m_SunLight.Diffuse.r   = 1.0f;
    m_SunLight.Diffuse.g   = 1.0f;
    m_SunLight.Diffuse.b   = 1.0f;    
    m_SunLight.Position.x   = 1.0f;
    m_SunLight.Position.y   = 0.0f;
    m_SunLight.Position.z   = 0.0f;
	m_SunLight.Direction.x = 1.0f;
	m_SunLight.Direction.y = 1.0f;
	m_SunLight.Direction.z = 1.0f;
    m_SunLight.Range        = 1000.0f;
	
	m_pSubWeatherTable = NULL;
	m_bSubTable = false;
	m_pWeaterTableRaw = NULL;
	m_fInterpol = 0.0f;
	
	m_iSubCurrentIndex = 0;
	m_iBeforeCurrentIndex = 0;

	for(int i = 0; i < DAYCYCLE; ++i)
	{
		m_SkyCenterColor[i].a=255;
		m_SkyCenterColor[i].r=56;
		m_SkyCenterColor[i].g=80;
		m_SkyCenterColor[i].b=175;	

		m_SkyNeighborColor[i].a=255;
		m_SkyNeighborColor[i].r=175;
		m_SkyNeighborColor[i].g=224;
		m_SkyNeighborColor[i].b=250;
		
		m_FogColor[i].a=255;
		m_FogColor[i].r=190;
		m_FogColor[i].g=210;
		m_FogColor[i].b=220;

		m_LightColor[i].a=0xff;
		m_LightColor[i].r=200;
		m_LightColor[i].g=200;
		m_LightColor[i].b=200;
		m_AmbientColor[i].a=0xff;
		m_AmbientColor[i].r=70;
		m_AmbientColor[i].g=70;
		m_AmbientColor[i].b=70;

		m_FogRangeNear[i]=1.0f;
		m_FogRangeFar[i]=100000.0f;
		m_SkyFogFar[i]=400000.0f;

		m_FogLayer[i][0].c=0xffffffff;
		m_FogLayer[i][1].c=0xffffffff;
		m_FogLayer[i][2].c=0xffffffff;
		m_SunPosition[i]=(3.1415f/3.0f)*2.0f-0.1f;

		m_ObjectAmbientColor[i].c=0xffaaaaaa;

		m_DetailFixColor[i].a=0xff;
		m_DetailFixColor[i].r=105;
		m_DetailFixColor[i].g=105;
		m_DetailFixColor[i].b=105;

		m_SpecularReflectionRate[i].a=0xff;
		m_SpecularReflectionRate[i].r=105;
		m_SpecularReflectionRate[i].g=105;
		m_SpecularReflectionRate[i].b=105;

		m_GrassColor[i].a=0xff;
		m_GrassColor[i].r=105;
		m_GrassColor[i].g=105;
		m_GrassColor[i].b=105;

		m_GlareSkyUpper[i].a=0xff;
		m_GlareSkyUpper[i].r=105;
		m_GlareSkyUpper[i].g=105;
		m_GlareSkyUpper[i].b=105;

		m_GlareSkyLower[i].a=0xff;
		m_GlareSkyLower[i].r=105;
		m_GlareSkyLower[i].g=105;
		m_GlareSkyLower[i].b=105;

		m_CharacterAmbient[i].a=0xff;
		m_CharacterAmbient[i].r=150;
		m_CharacterAmbient[i].g=150;
		m_CharacterAmbient[i].b=150;

		m_CharacterLight[i].a=0xff;
		m_CharacterLight[i].r=150;
		m_CharacterLight[i].g=150;
		m_CharacterLight[i].b=150;

		m_TerrainColor[i].a=0xff;
		m_TerrainColor[i].r=150;
		m_TerrainColor[i].g=150;
		m_TerrainColor[i].b=150;

		m_WaterColor[i].a=0xff;
		m_WaterColor[i].r=150;
		m_WaterColor[i].g=150;
		m_WaterColor[i].b=150;

		m_FurGrassColor[i].a=0xff;
		m_FurGrassColor[i].r=105;
		m_FurGrassColor[i].g=105;
		m_FurGrassColor[i].b=105;
	}
	CSectorScene::m_TerrainDetailFixColor.a=0xff;
	CSectorScene::m_TerrainDetailFixColor.r=105;
	CSectorScene::m_TerrainDetailFixColor.g=105;
	CSectorScene::m_TerrainDetailFixColor.b=105;

	if(Load()==false)	
	{
		//대낮~
		m_SkyCenterColor[0].a=255;
		m_SkyCenterColor[0].r=56;
		m_SkyCenterColor[0].g=80;
		m_SkyCenterColor[0].b=175;	

		m_SkyNeighborColor[0].a=255;
		m_SkyNeighborColor[0].r=175;
		m_SkyNeighborColor[0].g=224;
		m_SkyNeighborColor[0].b=250;
		
		m_FogColor[0].a=255;
		m_FogColor[0].r=190;
		m_FogColor[0].g=210;
		m_FogColor[0].b=220;

		m_LightColor[0].a=0xff;
		m_LightColor[0].r=200;
		m_LightColor[0].g=200;
		m_LightColor[0].b=200;
		m_AmbientColor[0].a=0xff;
		m_AmbientColor[0].r=70;
		m_AmbientColor[0].g=70;
		m_AmbientColor[0].b=70;

		m_FogRangeNear[0]=1.0f;
		m_FogRangeFar[0]=100000.0f;
		m_SkyFogFar[0]=400000.0f;

		m_FogLayer[0][0].c=0xffffffff;
		m_FogLayer[0][1].c=0xffffffff;
		m_FogLayer[0][2].c=0xffffffff;
		m_SunPosition[0]=(3.1415f/3.0f)*2.0f-0.1f;

		m_ObjectAmbientColor[0].c=0xffaaaaaa;
		//노을 노란색
		m_SkyCenterColor[1].a=255;
		m_SkyCenterColor[1].r=62;
		m_SkyCenterColor[1].g=3;
		m_SkyCenterColor[1].b=105;	

		m_SkyNeighborColor[1].a=255;
		m_SkyNeighborColor[1].r=62;
		m_SkyNeighborColor[1].g=3;
		m_SkyNeighborColor[1].b=105;
		
		m_FogColor[1].a=255;
		m_FogColor[1].r=248;
		m_FogColor[1].g=226;
		m_FogColor[1].b=141;
		
		m_LightColor[1].a=0xff;
		m_LightColor[1].r=240;
		m_LightColor[1].g=126;
		m_LightColor[1].b=041;

		m_AmbientColor[1].a=0xff;
		m_AmbientColor[1].r=10;
		m_AmbientColor[1].g=10;
		m_AmbientColor[1].b=10;

		m_FogRangeNear[1]=70000.0f;
		m_FogRangeFar[1]=400000.0f;
		m_FogRangeFar[1]=100000.0f;
		m_SkyFogFar[1]=400000.0f;

		m_FogLayer[1][0].c=0xffdf7600;
		m_FogLayer[1][1].c=0xfff8e28d;
		m_FogLayer[1][2].c=0xff000000;

		m_SunPosition[1]=3.1415f;

		m_ObjectAmbientColor[1].c=0xffddaaaa;
		///노을 빨간색
		m_SkyCenterColor[2].a=255;
		m_SkyCenterColor[2].r=3;
		m_SkyCenterColor[2].g=18;
		m_SkyCenterColor[2].b=54;	

		m_SkyNeighborColor[2].a=255;
		m_SkyNeighborColor[2].r=3;
		m_SkyNeighborColor[2].g=18;
		m_SkyNeighborColor[2].b=54;	
		m_FogColor[2].a=255;
		m_FogColor[2].r=215;
		m_FogColor[2].g=84;
		m_FogColor[2].b=13;
		
		m_LightColor[2].a=0xff;
		m_LightColor[2].r=60;
		m_LightColor[2].g=36;
		m_LightColor[2].b=011;

		m_AmbientColor[2].a=0xff;
		m_AmbientColor[2].r=3;
		m_AmbientColor[2].g=3;
		m_AmbientColor[2].b=3;

		m_FogRangeNear[2]=70000.0f;
		m_FogRangeFar[2]=400000.0f;
		m_FogRangeFar[2]=100000.0f;
		m_SkyFogFar[2]=400000.0f;

		m_FogLayer[2][0].c=0xffbf5600;
		m_FogLayer[2][1].c=0xfff5c26d;
		m_FogLayer[2][2].c=0xff000000;

		m_SunPosition[2]=3.1415f+0.5;
		m_ObjectAmbientColor[2].c=0xff555555;

		//노을 - 약간 파란색
		m_SkyCenterColor[3].a=255;
		m_SkyCenterColor[3].r=0;
		m_SkyCenterColor[3].g=0;
		m_SkyCenterColor[3].b=0;	

		m_SkyNeighborColor[3].a=255;
		m_SkyNeighborColor[3].r=0;
		m_SkyNeighborColor[3].g=0;
		m_SkyNeighborColor[3].b=0;
		
		m_FogColor[3].a=255;
		m_FogColor[3].r=8;
		m_FogColor[3].g=26;
		m_FogColor[3].b=42;
		
		m_LightColor[3].a=0xff;
		m_LightColor[3].r=15;
		m_LightColor[3].g=15;
		m_LightColor[3].b=40;

		m_AmbientColor[3].a=0xff;
		m_AmbientColor[3].r=15;
		m_AmbientColor[3].g=15;
		m_AmbientColor[3].b=40;

		m_FogRangeNear[3]=1000.0f;
		m_FogRangeFar[3]=400000.0f;
		m_FogRangeFar[3]=100000.0f;
		m_SkyFogFar[3]=400000.0f;

		m_FogLayer[3][0].c=0xff000000;
		m_FogLayer[3][1].c=0xff000000;
		m_FogLayer[3][2].c=0xff000000;
		m_SunPosition[3]=3.1415f+1.0f;
		m_ObjectAmbientColor[3].c=0xff444444;

		//밤
		m_SkyCenterColor[4].a=255;
		m_SkyCenterColor[4].r=8;
		m_SkyCenterColor[4].g=26;
		m_SkyCenterColor[4].b=72;	

		m_SkyNeighborColor[4].a=255;
		m_SkyNeighborColor[4].r=8;
		m_SkyNeighborColor[4].g=26;
		m_SkyNeighborColor[4].b=72;
		
		m_FogColor[4].a=255;
		m_FogColor[4].r=8;
		m_FogColor[4].g=26;
		m_FogColor[4].b=40;
		
		m_LightColor[4].a=0xff;
		m_LightColor[4].r=15;
		m_LightColor[4].g=15;
		m_LightColor[4].b=40;

		m_AmbientColor[4].a=0xff;
		m_AmbientColor[4].r=15;
		m_AmbientColor[4].g=15;
		m_AmbientColor[4].b=40;

		m_FogRangeNear[4]=1000.0f;
		m_FogRangeFar[4]=400000.0f;
		m_FogRangeFar[4]=100000.0f;
		m_SkyFogFar[4]=400000.0f;

		m_FogLayer[4][0].c=0xff000000;
		m_FogLayer[4][1].c=0xff000000;
		m_FogLayer[4][2].c=0xff000000;	
		m_SunPosition[4]=3.1415f+(3.1415f/2.0f);

		m_ObjectAmbientColor[4].c=0xff444444;
		
		//새벽약간 파란색
		m_SkyCenterColor[5].a=255;
		m_SkyCenterColor[5].r=0;
		m_SkyCenterColor[5].g=0;
		m_SkyCenterColor[5].b=0;	

		m_SkyNeighborColor[5].a=255;
		m_SkyNeighborColor[5].r=0;
		m_SkyNeighborColor[5].g=0;
		m_SkyNeighborColor[5].b=0;
		
		m_FogColor[5].a=255;
		m_FogColor[5].r=8;
		m_FogColor[5].g=26;
		m_FogColor[5].b=42;
		
		m_LightColor[5].a=0xff;
		m_LightColor[5].r=15;
		m_LightColor[5].g=15;
		m_LightColor[5].b=40;

		m_AmbientColor[5].a=0xff;
		m_AmbientColor[5].r=15;
		m_AmbientColor[5].g=15;
		m_AmbientColor[5].b=40;

		m_FogRangeNear[5]=1000.0f;
		m_FogRangeFar[5]=400000.0f;
		m_FogRangeFar[5]=100000.0f;
		m_SkyFogFar[5]=400000.0f;

		m_FogLayer[5][0].c=0xff000000;
		m_FogLayer[5][1].c=0xff000000;
		m_FogLayer[5][2].c=0xff000000;

		m_SunPosition[5]=0.0f;

		m_ObjectAmbientColor[5].c=0xff555555;

		//아침 노란색

		m_SkyCenterColor[6].a=255;
		m_SkyCenterColor[6].r=11;
		m_SkyCenterColor[6].g=2;
		m_SkyCenterColor[6].b=53;	

		m_SkyNeighborColor[6].a=255;
		m_SkyNeighborColor[6].r=11;
		m_SkyNeighborColor[6].g=2;
		m_SkyNeighborColor[6].b=53;

		m_FogColor[6].a=255;
		m_FogColor[6].r=154;
		m_FogColor[6].g=134;
		m_FogColor[6].b=109;
		
		m_LightColor[6].a=0xff;
		m_LightColor[6].r=140;
		m_LightColor[6].g=140;
		m_LightColor[6].b=140;

		m_AmbientColor[6].a=0xff;
		m_AmbientColor[6].r=30;
		m_AmbientColor[6].g=30;
		m_AmbientColor[6].b=30;

		m_FogRangeNear[6]=70000.0f;
		m_FogRangeFar[6]=400000.0f;
		m_FogRangeFar[6]=100000.0f;
		m_SkyFogFar[6]=400000.0f;

		m_FogLayer[6][0].c=0xffc8c8ff;
		m_FogLayer[6][1].c=0xffc8c8ff;
		m_FogLayer[6][2].c=0xff88724d;
		m_SunPosition[6]=3.14f/5.0f;

		m_ObjectAmbientColor[6].c=0xff777777;

		// WeatherColorTable.dat 로 저장.
//		Save();
	}
	
	m_CustomWaterColor=false;
}

CWeatherManager::~CWeatherManager()
{
	if(m_pSubWeatherTable)
	{
		delete[] m_pSubWeatherTable;
		m_pSubWeatherTable = NULL;
	}
	if(m_pWeaterTableRaw)
	{
		delete[] m_pWeaterTableRaw;
		m_pWeaterTableRaw = NULL;
	}
}

void CWeatherManager::Reload()
{
	m_SunScene.Reload();
}

void CWeatherManager::Create()
{	
	m_SkyScene.Create();
	m_SunScene.Create();
//	m_FallScene.Create();
//	m_CloudScene.Create();
}
void CWeatherManager::RenderEnv(LPDIRECT3DDEVICE8	pd3dDevice)
{
	pd3dDevice->LightEnable(0,FALSE);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);		
	pd3dDevice->SetRenderState( D3DRS_FOGSTART, *((DWORD *)(&m_InterFogNear)) );
	pd3dDevice->SetRenderState( D3DRS_FOGEND,   *((DWORD *)(&m_InterSkyFogFar)) );
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	
	matrix matInit;
	matInit.MakeIdent();
	pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)(&matInit));	
	m_SkyScene.Render(pd3dDevice);
	//m_SunScene.Render(pd3dDevice);	
	pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)(&matInit));
	m_SkyScene.RenderCloud(pd3dDevice);	
	pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)(&matInit));	

	pd3dDevice->SetRenderState( D3DRS_FOGEND,   *((DWORD *)(&m_InterFogFar)) );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);		
	pd3dDevice->LightEnable(0,TRUE);
}
void CWeatherManager::Render(LPDIRECT3DDEVICE8 pd3dDevice)
{	
	///*
	pd3dDevice->LightEnable(0,FALSE);
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);		
	pd3dDevice->SetRenderState( D3DRS_FOGSTART, *((DWORD *)(&m_InterFogNear)) );
	pd3dDevice->SetRenderState( D3DRS_FOGEND,   *((DWORD *)(&m_InterSkyFogFar)) );
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	
	matrix matInit;
	matInit.MakeIdent();
	pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)(&matInit));	
	m_SkyScene.Render(pd3dDevice);
	m_SunScene.Render(pd3dDevice);	
	pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)(&matInit));
	m_SkyScene.RenderCloud(pd3dDevice);	
	pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)(&matInit));	

	pd3dDevice->SetRenderState( D3DRS_FOGEND,   *((DWORD *)(&m_InterFogFar)) );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE);		
	pd3dDevice->LightEnable(0,TRUE);
	//*/
}
int CWeatherManager::GetSubWeatherNum(vector3 *vecPos)
{
	if(vecPos == NULL || m_bSubTable == false)
	{
		return -1;
	}
	
	float indexx=(vecPos->x/SECTORSIZE) -1.0f;
	float indexy=(vecPos->z/SECTORSIZE) -1.0f;
	indexx /= 11.0f;
	indexy /= 11.0f;
	if(indexx < 0.0f)
		indexx = 0.0f;
	if(indexx > 1.0f)
		indexx = 1.0f;

	if(indexy < 0.0f)
		indexy = 0.0f;
	if(indexy > 1.0f)
		indexy = 1.0f;
	
	int ix = 512 *indexx;
	int iy = 512 *indexy;

	if(ix < 0)
		ix = 0;
	if(ix >= 512)
		ix = 511;

	if(iy < 0)
		iy = 0;
	if(iy >= 512)
		iy = 511;


	return m_pWeaterTableRaw[iy *  512 + ix];

	
}
CTexture *CWeatherManager::GetCurrentSubGrassTexture()
{
	if(m_bSubTable)
	{
		if(m_pSubWeatherTable[m_iSubCurrentIndex].m_pGrassTexture)
		{
			return m_pSubWeatherTable[m_iSubCurrentIndex].m_pGrassTexture;
		}
	}
	
	return NULL;
}
void CWeatherManager::SetWeather(float fTime,LPDIRECT3DDEVICE8 pd3dDevice)
{	
	if(m_CustomWaterColor)
	{		
		int nLastWeatherTime=(int)m_fWeatherTime;

		int nWeatherEffectTime=24;

		color dwFogLayerColor[3];
		color AmbientColor,LightColor,FogColor,SkyCenterColor,SkyNeighborColor;
		float fFogStart,fFogEnd,fSkyFogFar;
		float fSunTime=fTime;
		
		AmbientColor=color::Interpolation(m_AmbientColor[nLastWeatherTime],m_AmbientColor[nWeatherEffectTime],m_CustomWaterRate);		
		LightColor=color::Interpolation(m_LightColor[nLastWeatherTime],m_LightColor[nWeatherEffectTime],m_CustomWaterRate);
		FogColor=color::Interpolation(m_FogColor[nLastWeatherTime],m_FogColor[nWeatherEffectTime],m_CustomWaterRate);

		SkyCenterColor=color::Interpolation(m_SkyCenterColor[nLastWeatherTime],m_SkyCenterColor[nWeatherEffectTime],m_CustomWaterRate);
		SkyNeighborColor=color::Interpolation(m_SkyNeighborColor[nLastWeatherTime],m_SkyNeighborColor[nWeatherEffectTime],m_CustomWaterRate);

		fFogStart=m_FogRangeNear[nLastWeatherTime]+(m_FogRangeNear[nWeatherEffectTime]-m_FogRangeNear[nLastWeatherTime])*(m_CustomWaterRate-(int)m_CustomWaterRate);
		fFogEnd=m_FogRangeFar[nLastWeatherTime]+(m_FogRangeFar[nWeatherEffectTime]-m_FogRangeFar[nLastWeatherTime])*(m_CustomWaterRate-(int)m_CustomWaterRate);
		fSkyFogFar=m_SkyFogFar[nLastWeatherTime]+(m_SkyFogFar[nWeatherEffectTime]-m_SkyFogFar[nLastWeatherTime])*(m_CustomWaterRate-(int)m_CustomWaterRate);
		
		m_InterObjectAmbientColor=color::Interpolation(m_ObjectAmbientColor[nLastWeatherTime],m_ObjectAmbientColor[nWeatherEffectTime],m_CustomWaterRate);			

		dwFogLayerColor[0]=color::Interpolation(m_FogLayer[nLastWeatherTime][0],m_FogLayer[nWeatherEffectTime][0],m_CustomWaterRate);
		dwFogLayerColor[1]=color::Interpolation(m_FogLayer[nLastWeatherTime][1],m_FogLayer[nWeatherEffectTime][1],m_CustomWaterRate);
		dwFogLayerColor[2]=color::Interpolation(m_FogLayer[nLastWeatherTime][2],m_FogLayer[nWeatherEffectTime][2],m_CustomWaterRate);		

		m_InterSpecularReflectionRate=color::Interpolation(m_SpecularReflectionRate[nLastWeatherTime],m_SpecularReflectionRate[nWeatherEffectTime],fTime);
		m_InterGrassColor=color::Interpolation(m_GrassColor[nLastWeatherTime],m_GrassColor[nWeatherEffectTime],fTime);
		m_InterGlareSkyUpper=color::Interpolation(m_GlareSkyUpper[nLastWeatherTime],m_GlareSkyUpper[nWeatherEffectTime],fTime);
		m_InterGlareSkyLower=color::Interpolation(m_GlareSkyLower[nLastWeatherTime],m_GlareSkyLower[nWeatherEffectTime],fTime);
		m_InterCharacterAmbient=color::Interpolation(m_CharacterAmbient[nLastWeatherTime],m_CharacterAmbient[nWeatherEffectTime],fTime);
		m_InterCharacterLight=color::Interpolation(m_CharacterLight[nLastWeatherTime],m_CharacterLight[nWeatherEffectTime],fTime);
		m_InterTerrainColor=color::Interpolation(m_TerrainColor[nLastWeatherTime],m_TerrainColor[nWeatherEffectTime],fTime);		
		m_InterWaterColor=color::Interpolation(m_WaterColor[nLastWeatherTime],m_WaterColor[nWeatherEffectTime],fTime);
		m_InterFurGrassColor=color::Interpolation(m_FurGrassColor[nLastWeatherTime],m_FurGrassColor[nWeatherEffectTime],fTime);	
		m_InterDetailFixColor = color::Interpolation(m_DetailFixColor[nLastWeatherTime],m_DetailFixColor[nWeatherEffectTime],fTime);

		
		m_SkyScene.SetColor(SkyNeighborColor.c,SkyNeighborColor.c,(DWORD*)dwFogLayerColor);	
		if(m_ClearColor)
			m_ClearColor->c=FogColor.c;
		if(CRenderOption::m_FullSceneGlare)
		{	
			m_ClearColor->c=0x0;
			m_GF3ClareColor.c=FogColor.c;	
		}
		if(!pd3dDevice)
		{
			return;
		}
		pd3dDevice->SetRenderState( D3DRS_AMBIENT, AmbientColor.c);		
		m_InterAmbientColor=AmbientColor;	
	
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,    TRUE );
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,     FogColor.c);
		pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_LINEAR );
		pd3dDevice->SetRenderState( D3DRS_FOGVERTEXMODE,  D3DFOG_LINEAR );
		pd3dDevice->SetRenderState( D3DRS_FOGSTART, *((DWORD *)(&fFogStart)) );
		pd3dDevice->SetRenderState( D3DRS_FOGEND,   *((DWORD *)(&fFogEnd)) );
		
		m_InterSkyFogFar=fSkyFogFar;
		m_InterFogNear=fFogStart;
		m_InterFogFar=fFogEnd;
	
		float fAngle=cosf((fSunTime/24.0f)*3.14159f*2.0f);
		vector3 pos=vector3(0.5f,-fAngle,0.5f);
		pos.Normalize();

	
		m_SunScene.SetSunDirection(pos);

		D3DLIGHT8 light; 
		ZeroMemory( &light, sizeof(D3DLIGHT8) );
		light.Type        = D3DLIGHT_DIRECTIONAL;
		light.Diffuse.r   = LightColor.r/255.0f;
		light.Diffuse.g   = LightColor.g/255.0f;
		light.Diffuse.b   = LightColor.b/255.0f;
		
		vector3 vecLight(-1.0f,-1.0f,-1.0f);
		vecLight.Normalize();
		light.Position.x   = light.Direction.x =vecLight.x;
		light.Position.y   = light.Direction.y =vecLight.y;
		light.Position.z   = light.Direction.z =vecLight.z;
		light.Range        = 1000.0f;
		
		pd3dDevice->SetLight( 0,&light );		
		m_SunLight=light;

		return;
	}
	if(m_bSubTable == true)
	{
		int iCurrent;
		vector3 *ViewPos = CSceneManager::m_ViewCamera->GetPosition();
		
		// CurrentRaw 구한다
		iCurrent = GetSubWeatherNum(ViewPos);
	
		CSubWeatherData *pTable = NULL;
		CSubWeatherData Interpol;
		int iMin = -1;
		int iValue;
		int iTarget; 
		for(int ik= 0 ; ik < m_iSubWeatherTableNum; ik++)
		{
			iTarget = abs(iCurrent - m_pSubWeatherTable[ik].m_iIndex);
			if(ik == 0)
			{
				iMin = 0;
				iValue = iTarget;
			}
			else if(iTarget < iValue)
			{
				iValue = iTarget;
				iMin = ik;
			}
		}
		if(iMin != m_iSubCurrentIndex)
		{
			m_iBeforeCurrentIndex = m_iSubCurrentIndex;
			m_iSubCurrentIndex = iMin;
			m_fInterpol = 0.0f;
		}

		if(m_iSubWeatherTableNum < m_iBeforeCurrentIndex)
			m_iBeforeCurrentIndex = m_iSubWeatherTableNum;

		if(m_fInterpol >= 1.0f)
		{
			pTable = &(m_pSubWeatherTable[iMin]);
		}
		else
		{
			for(int iW = 0; iW < DAYCYCLE; iW++)
			{
		
	
				Interpol.m_SkyCenterColor[iW] = color::Interpolation(m_pSubWeatherTable[m_iBeforeCurrentIndex].m_SkyCenterColor[iW],m_pSubWeatherTable[m_iSubCurrentIndex].m_SkyCenterColor[iW],m_fInterpol);
				Interpol.m_SkyNeighborColor[iW] = color::Interpolation(m_pSubWeatherTable[m_iBeforeCurrentIndex].m_SkyNeighborColor[iW],m_pSubWeatherTable[m_iSubCurrentIndex].m_SkyNeighborColor[iW],m_fInterpol);

				Interpol.m_AmbientColor[iW]	= color::Interpolation(m_pSubWeatherTable[m_iBeforeCurrentIndex].m_AmbientColor[iW],m_pSubWeatherTable[m_iSubCurrentIndex].m_AmbientColor[iW],m_fInterpol);
				Interpol.m_CharacterAmbient[iW]	= color::Interpolation(m_pSubWeatherTable[m_iBeforeCurrentIndex].m_CharacterAmbient[iW],m_pSubWeatherTable[m_iSubCurrentIndex].m_CharacterAmbient[iW],m_fInterpol);
				Interpol.m_CharacterLight[iW]	= color::Interpolation(m_pSubWeatherTable[m_iBeforeCurrentIndex].m_CharacterLight[iW],m_pSubWeatherTable[m_iSubCurrentIndex].m_CharacterLight[iW],m_fInterpol);
				Interpol.m_DetailFixColor[iW]	= color::Interpolation(m_pSubWeatherTable[m_iBeforeCurrentIndex].m_DetailFixColor[iW],m_pSubWeatherTable[m_iSubCurrentIndex].m_DetailFixColor[iW],m_fInterpol);
				Interpol.m_FogColor[iW] = color::Interpolation(m_pSubWeatherTable[m_iBeforeCurrentIndex].m_FogColor[iW],m_pSubWeatherTable[m_iSubCurrentIndex].m_FogColor[iW],m_fInterpol);
				Interpol.m_FogLayer[iW][0] = color::Interpolation(m_pSubWeatherTable[m_iBeforeCurrentIndex].m_FogLayer[iW][0],m_pSubWeatherTable[m_iSubCurrentIndex].m_FogLayer[iW][0],m_fInterpol);
				Interpol.m_FogLayer[iW][1] = color::Interpolation(m_pSubWeatherTable[m_iBeforeCurrentIndex].m_FogLayer[iW][1],m_pSubWeatherTable[m_iSubCurrentIndex].m_FogLayer[iW][1],m_fInterpol);
				Interpol.m_FogLayer[iW][2] = color::Interpolation(m_pSubWeatherTable[m_iBeforeCurrentIndex].m_FogLayer[iW][2],m_pSubWeatherTable[m_iSubCurrentIndex].m_FogLayer[iW][2],m_fInterpol);

				Interpol.m_FurGrassColor[iW] = color::Interpolation(m_pSubWeatherTable[m_iBeforeCurrentIndex].m_FurGrassColor[iW],m_pSubWeatherTable[m_iSubCurrentIndex].m_FurGrassColor[iW],m_fInterpol);
				Interpol.m_GlareSkyLower[iW] = color::Interpolation(m_pSubWeatherTable[m_iBeforeCurrentIndex].m_GlareSkyLower[iW],m_pSubWeatherTable[m_iSubCurrentIndex].m_GlareSkyLower[iW],m_fInterpol);
				Interpol.m_GlareSkyUpper[iW] = color::Interpolation(m_pSubWeatherTable[m_iBeforeCurrentIndex].m_GlareSkyUpper[iW],m_pSubWeatherTable[m_iSubCurrentIndex].m_GlareSkyUpper[iW],m_fInterpol);

				Interpol.m_LightColor[iW] = color::Interpolation(m_pSubWeatherTable[m_iBeforeCurrentIndex].m_LightColor[iW],m_pSubWeatherTable[m_iSubCurrentIndex].m_LightColor[iW],m_fInterpol);
				Interpol.m_ObjectAmbientColor[iW] = color::Interpolation(m_pSubWeatherTable[m_iBeforeCurrentIndex].m_ObjectAmbientColor[iW],m_pSubWeatherTable[m_iSubCurrentIndex].m_ObjectAmbientColor[iW],m_fInterpol);
		
				Interpol.m_SpecularReflectionRate[iW] = color::Interpolation(m_pSubWeatherTable[m_iBeforeCurrentIndex].m_SpecularReflectionRate[iW],m_pSubWeatherTable[m_iSubCurrentIndex].m_SpecularReflectionRate[iW],m_fInterpol);
				Interpol.m_GrassColor[iW] = color::Interpolation(m_pSubWeatherTable[m_iBeforeCurrentIndex].m_GrassColor[iW],m_pSubWeatherTable[m_iSubCurrentIndex].m_GrassColor[iW],m_fInterpol);
				Interpol.m_TerrainColor[iW] = color::Interpolation(m_pSubWeatherTable[m_iBeforeCurrentIndex].m_TerrainColor[iW],m_pSubWeatherTable[m_iSubCurrentIndex].m_TerrainColor[iW],m_fInterpol);
				Interpol.m_WaterColor[iW] = color::Interpolation(m_pSubWeatherTable[m_iBeforeCurrentIndex].m_WaterColor[iW],m_pSubWeatherTable[m_iSubCurrentIndex].m_WaterColor[iW],m_fInterpol);

				Interpol.m_SunPosition[iW] = m_pSubWeatherTable[m_iSubCurrentIndex].m_SunPosition[iW];
	
				Interpol.m_FogRangeFar[iW] = m_pSubWeatherTable[m_iBeforeCurrentIndex].m_FogRangeFar[iW] + ( m_pSubWeatherTable[m_iSubCurrentIndex].m_FogRangeFar[iW] - m_pSubWeatherTable[m_iBeforeCurrentIndex].m_FogRangeFar[iW]) * m_fInterpol;
				Interpol.m_FogRangeNear[iW] = m_pSubWeatherTable[m_iBeforeCurrentIndex].m_FogRangeNear[iW] + ( m_pSubWeatherTable[m_iSubCurrentIndex].m_FogRangeNear[iW] - m_pSubWeatherTable[m_iBeforeCurrentIndex].m_FogRangeNear[iW]) * m_fInterpol;
				
				Interpol.m_SkyFogFar[iW] = m_pSubWeatherTable[m_iBeforeCurrentIndex].m_SkyFogFar[iW] + ( m_pSubWeatherTable[m_iSubCurrentIndex].m_SkyFogFar[iW] - m_pSubWeatherTable[m_iBeforeCurrentIndex].m_SkyFogFar[iW]) * m_fInterpol;
					
			}
			m_fInterpol += 0.004f;
			pTable = &Interpol;
		}
		if(pTable == NULL)
		{
			//m_bSubTable = false;
			//SetWeather(fTime,pd3dDevice);
			pTable = &m_pSubWeatherTable[0];
		}
			
		m_fWeatherTime=fTime;
		color dwFogLayerColor[3];
		color AmbientColor,LightColor,FogColor,SkyCenterColor,SkyNeighborColor;
		float fFogStart,fFogEnd,fSkyFogFar;
		float fSunTime=fTime;

		const int REALDAYCYCLE=24;

		for(int i=0;i<REALDAYCYCLE-1;i++)
		{
			if(fTime>=i && fTime <i+1)
			{
				fTime-=i;
				AmbientColor=color::Interpolation(pTable->m_AmbientColor[i],pTable->m_AmbientColor[i+1],fTime);		
				LightColor=color::Interpolation(pTable->m_LightColor[i],pTable->m_LightColor[i+1],fTime);
				FogColor=color::Interpolation(pTable->m_FogColor[i],pTable->m_FogColor[i+1],fTime);

				SkyCenterColor=color::Interpolation(pTable->m_SkyCenterColor[i],pTable->m_SkyCenterColor[i+1],fTime);
				SkyNeighborColor=color::Interpolation(pTable->m_SkyNeighborColor[i],pTable->m_SkyNeighborColor[i+1],fTime);

				fFogStart=pTable->m_FogRangeNear[i]+(pTable->m_FogRangeNear[i+1]-pTable->m_FogRangeNear[i])*(fTime-(int)fTime);
				fFogEnd=pTable->m_FogRangeFar[i]+(pTable->m_FogRangeFar[i+1]-pTable->m_FogRangeFar[i])*(fTime-(int)fTime);
				fSkyFogFar=pTable->m_SkyFogFar[i]+(pTable->m_SkyFogFar[i+1]-pTable->m_SkyFogFar[i])*(fTime-(int)fTime);
				/*
				if(m_SunPosition[i+1]==0.0f)
				{
					fSunTime=m_SunPosition[i]+(3.1415f*2.0f-m_SunPosition[i])*(fTime-(int)fTime);
				}
				else
				{
					fSunTime=m_SunPosition[i]+(m_SunPosition[i+1]-m_SunPosition[i])*(fTime-(int)fTime);
				}
				*/
				m_InterObjectAmbientColor=color::Interpolation(pTable->m_ObjectAmbientColor[i],pTable->m_ObjectAmbientColor[i+1],fTime);			

				dwFogLayerColor[0]=color::Interpolation(pTable->m_FogLayer[i][0],pTable->m_FogLayer[i+1][0],fTime);
				dwFogLayerColor[1]=color::Interpolation(pTable->m_FogLayer[i][1],pTable->m_FogLayer[i+1][1],fTime);
				dwFogLayerColor[2]=color::Interpolation(pTable->m_FogLayer[i][2],pTable->m_FogLayer[i+1][2],fTime);

				m_InterSpecularReflectionRate=color::Interpolation(pTable->m_SpecularReflectionRate[i],pTable->m_SpecularReflectionRate[i+1],fTime);
				m_InterGrassColor=color::Interpolation(pTable->m_GrassColor[i],pTable->m_GrassColor[i+1],fTime);
				m_InterGlareSkyUpper=color::Interpolation(pTable->m_GlareSkyUpper[i],pTable->m_GlareSkyUpper[i+1],fTime);
				m_InterGlareSkyLower=color::Interpolation(pTable->m_GlareSkyLower[i],pTable->m_GlareSkyLower[i+1],fTime);
				m_InterCharacterAmbient=color::Interpolation(pTable->m_CharacterAmbient[i],pTable->m_CharacterAmbient[i+1],fTime);
				m_InterCharacterLight=color::Interpolation(pTable->m_CharacterLight[i],pTable->m_CharacterLight[i+1],fTime);
				m_InterTerrainColor=color::Interpolation(pTable->m_TerrainColor[i],pTable->m_TerrainColor[i+1],fTime);
				m_InterWaterColor=color::Interpolation(pTable->m_WaterColor[i],pTable->m_WaterColor[i+1],fTime);	

				m_InterFurGrassColor=color::Interpolation(pTable->m_FurGrassColor[i],pTable->m_FurGrassColor[i+1],fTime);	

				m_InterDetailFixColor = color::Interpolation(pTable->m_DetailFixColor[i],pTable->m_DetailFixColor[i+1],fTime);
				

				
			}
		}
		if(fTime>=REALDAYCYCLE-1)
		//if(DAYCYCLE-1 <= fTime)
		{
			fTime-=REALDAYCYCLE-1;
			AmbientColor=color::Interpolation(pTable->m_AmbientColor[DAYCYCLE-1],pTable->m_AmbientColor[0],fTime);		
			LightColor=color::Interpolation(pTable->m_LightColor[DAYCYCLE-1],pTable->m_LightColor[0],fTime);
			FogColor=color::Interpolation(pTable->m_FogColor[DAYCYCLE-1],pTable->m_FogColor[0],fTime);

			SkyCenterColor=color::Interpolation(pTable->m_SkyCenterColor[DAYCYCLE-1],pTable->m_SkyCenterColor[0],fTime);
			SkyNeighborColor=color::Interpolation(pTable->m_SkyNeighborColor[DAYCYCLE-1],pTable->m_SkyNeighborColor[0],fTime);

			fFogStart=m_FogRangeNear[DAYCYCLE-1]+(pTable->m_FogRangeNear[0]-pTable->m_FogRangeNear[DAYCYCLE-1])*(fTime-(int)fTime);
			fFogEnd=m_FogRangeFar[DAYCYCLE-1]+(pTable->m_FogRangeFar[0]-pTable->m_FogRangeFar[DAYCYCLE-1])*(fTime-(int)fTime);
			fSkyFogFar=m_SkyFogFar[DAYCYCLE-1]+(pTable->m_SkyFogFar[0]-pTable->m_SkyFogFar[DAYCYCLE-1])*(fTime-(int)fTime);

			//fSunTime=m_SunPosition[DAYCYCLE-1]+(m_SunPosition[0]-m_SunPosition[DAYCYCLE-1])*(fTime-(int)fTime);

			m_InterObjectAmbientColor=color::Interpolation(pTable->m_ObjectAmbientColor[DAYCYCLE-1],pTable->m_ObjectAmbientColor[0],fTime);

			dwFogLayerColor[0]=color::Interpolation(pTable->m_FogLayer[DAYCYCLE-1][0],pTable->m_FogLayer[0][0],fTime);
			dwFogLayerColor[1]=color::Interpolation(pTable->m_FogLayer[DAYCYCLE-1][1],pTable->m_FogLayer[0][1],fTime);
			dwFogLayerColor[2]=color::Interpolation(pTable->m_FogLayer[DAYCYCLE-1][2],pTable->m_FogLayer[0][2],fTime);

			m_InterSpecularReflectionRate=color::Interpolation(pTable->m_SpecularReflectionRate[DAYCYCLE-1],pTable->m_SpecularReflectionRate[0],fTime);
			m_InterGrassColor=color::Interpolation(pTable->m_GrassColor[DAYCYCLE-1],pTable->m_GrassColor[0],fTime);
			m_InterGlareSkyUpper=color::Interpolation(pTable->m_GlareSkyUpper[DAYCYCLE-1],pTable->m_GlareSkyUpper[0],fTime);
			m_InterGlareSkyLower=color::Interpolation(pTable->m_GlareSkyLower[DAYCYCLE-1],pTable->m_GlareSkyLower[0],fTime);
			m_InterCharacterAmbient=color::Interpolation(pTable->m_CharacterAmbient[DAYCYCLE-1],pTable->m_CharacterAmbient[0],fTime);
			m_InterCharacterLight=color::Interpolation(pTable->m_CharacterLight[DAYCYCLE-1],pTable->m_CharacterLight[0],fTime);
			m_InterTerrainColor=color::Interpolation(pTable->m_TerrainColor[DAYCYCLE-1],pTable->m_TerrainColor[0],fTime);		
			m_InterWaterColor=color::Interpolation(pTable->m_WaterColor[DAYCYCLE-1],pTable->m_WaterColor[0],fTime);
			m_InterFurGrassColor=color::Interpolation(pTable->m_FurGrassColor[DAYCYCLE-1],pTable->m_FurGrassColor[0],fTime);	
			m_InterDetailFixColor = color::Interpolation(pTable->m_DetailFixColor[DAYCYCLE-1],pTable->m_DetailFixColor[0],fTime);

		}	
		m_SkyScene.SetColor(SkyNeighborColor.c,SkyNeighborColor.c,(DWORD*)dwFogLayerColor);
		//m_CloudScene.SetColor(SkyNeighborColor.c,SkyNeighborColor.c,(DWORD*)dwFogLayerColor);
		if(m_ClearColor)
			m_ClearColor->c=FogColor.c;
		if(CRenderOption::m_FullSceneGlare)
		{	
			if(m_ClearColor)
				m_ClearColor->c=0x0;
			m_GF3ClareColor.c=FogColor.c;	
		}
		if(!pd3dDevice)
		{
			return;
		}
		pd3dDevice->SetRenderState( D3DRS_AMBIENT, AmbientColor.c);		
		m_InterAmbientColor=AmbientColor;
		
		
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,    TRUE );
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,     FogColor.c);
		pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_LINEAR );
		pd3dDevice->SetRenderState( D3DRS_FOGVERTEXMODE,  D3DFOG_LINEAR );
		pd3dDevice->SetRenderState( D3DRS_FOGSTART, *((DWORD *)(&fFogStart)) );
		pd3dDevice->SetRenderState( D3DRS_FOGEND,   *((DWORD *)(&fFogEnd)) );	

		m_InterSkyFogFar=fSkyFogFar;
		m_InterFogNear=fFogStart;
		m_InterFogFar=fFogEnd;
		
		
		

		/*
		if(m_SunPosition[i+1]==0.0f)
		{
			fSunTime=m_SunPosition[i]+(3.1415f*2.0f-m_SunPosition[i])*(fTime-(int)fTime);
		}
		else
		{
			fSunTime=m_SunPosition[i]+(m_SunPosition[i+1]-m_SunPosition[i])*(fTime-(int)fTime);
		}	
		// Calc Sun position
		float x=sinf(fSunTime);
		float z=cosf(fSunTime);
		float y=0.0f;	
		matrix matPosSun,matRotSun;
		matPosSun.Translation(vector3(x,y,z));	
		matRotSun.Rotation(vector3(1.0f,1.0f,1.0f),-3.1415f/3.0f);
		matPosSun=matPosSun*matRotSun;
		vector3 pos=matPosSun.GetLoc();
		//vector3 pos=vector3(1.0f,1.0f,1.0f);
		*/
		//matRotSun.Rotation(vector3(1.0f,1.0f,1.0f),-3.1415f/3.0f);
		float fAngle=cosf((fSunTime/24.0f)*3.14159f*2.0f);
		vector3 pos=vector3(0.5f,-fAngle,0.5f);
		pos.Normalize();

		//pos.Normalize();
		m_SunScene.SetSunDirection(pos);

		D3DLIGHT8 light; 
		ZeroMemory( &light, sizeof(D3DLIGHT8) );
		light.Type        = D3DLIGHT_DIRECTIONAL;
		light.Diffuse.r   = LightColor.r/255.0f;
		light.Diffuse.g   = LightColor.g/255.0f;
		light.Diffuse.b   = LightColor.b/255.0f;
		/*
		light.Position.x   = light.Direction.x = -pos.x;
		light.Position.y   = light.Direction.y = -pos.y;
		light.Position.z   = light.Direction.z = -pos.z;	
		*/	
		vector3 vecLight(-1.0f,-1.0f,-1.0f);
		vecLight.Normalize();
		light.Position.x   = light.Direction.x =vecLight.x;
		light.Position.y   = light.Direction.y =vecLight.y;
		light.Position.z   = light.Direction.z =vecLight.z;
		light.Range        = 1000.0f;	
		pd3dDevice->SetLight( 0,&light );		
		m_SunLight=light;

		light.Diffuse.r   = m_InterTerrainColor.r/255.0f;
		light.Diffuse.g   = m_InterTerrainColor.g/255.0f;
		light.Diffuse.b   = m_InterTerrainColor.b/255.0f;	
		m_TerrainLight=light;
		CSectorScene::m_TerrainDetailFixColor = m_InterDetailFixColor;
	}
	else
	{
		m_fWeatherTime=fTime;
		color dwFogLayerColor[3];
		color AmbientColor,LightColor,FogColor,SkyCenterColor,SkyNeighborColor;
		float fFogStart,fFogEnd,fSkyFogFar;
		float fSunTime=fTime;

		const int REALDAYCYCLE=24;

		for(int i=0;i<REALDAYCYCLE-1;i++)
		{
			if(fTime>=i && fTime <i+1)
			{
				fTime-=i;
				AmbientColor=color::Interpolation(m_AmbientColor[i],m_AmbientColor[i+1],fTime);		
				LightColor=color::Interpolation(m_LightColor[i],m_LightColor[i+1],fTime);
				FogColor=color::Interpolation(m_FogColor[i],m_FogColor[i+1],fTime);

				SkyCenterColor=color::Interpolation(m_SkyCenterColor[i],m_SkyCenterColor[i+1],fTime);
				SkyNeighborColor=color::Interpolation(m_SkyNeighborColor[i],m_SkyNeighborColor[i+1],fTime);

				fFogStart=m_FogRangeNear[i]+(m_FogRangeNear[i+1]-m_FogRangeNear[i])*(fTime-(int)fTime);
				fFogEnd=m_FogRangeFar[i]+(m_FogRangeFar[i+1]-m_FogRangeFar[i])*(fTime-(int)fTime);
				fSkyFogFar=m_SkyFogFar[i]+(m_SkyFogFar[i+1]-m_SkyFogFar[i])*(fTime-(int)fTime);
				/*
				if(m_SunPosition[i+1]==0.0f)
				{
					fSunTime=m_SunPosition[i]+(3.1415f*2.0f-m_SunPosition[i])*(fTime-(int)fTime);
				}
				else
				{
					fSunTime=m_SunPosition[i]+(m_SunPosition[i+1]-m_SunPosition[i])*(fTime-(int)fTime);
				}
				*/
				m_InterObjectAmbientColor=color::Interpolation(m_ObjectAmbientColor[i],m_ObjectAmbientColor[i+1],fTime);			

				dwFogLayerColor[0]=color::Interpolation(m_FogLayer[i][0],m_FogLayer[i+1][0],fTime);
				dwFogLayerColor[1]=color::Interpolation(m_FogLayer[i][1],m_FogLayer[i+1][1],fTime);
				dwFogLayerColor[2]=color::Interpolation(m_FogLayer[i][2],m_FogLayer[i+1][2],fTime);

				m_InterSpecularReflectionRate=color::Interpolation(m_SpecularReflectionRate[i],m_SpecularReflectionRate[i+1],fTime);
				m_InterGrassColor=color::Interpolation(m_GrassColor[i],m_GrassColor[i+1],fTime);
				m_InterGlareSkyUpper=color::Interpolation(m_GlareSkyUpper[i],m_GlareSkyUpper[i+1],fTime);
				m_InterGlareSkyLower=color::Interpolation(m_GlareSkyLower[i],m_GlareSkyLower[i+1],fTime);
				m_InterCharacterAmbient=color::Interpolation(m_CharacterAmbient[i],m_CharacterAmbient[i+1],fTime);
				m_InterCharacterLight=color::Interpolation(m_CharacterLight[i],m_CharacterLight[i+1],fTime);
				m_InterTerrainColor=color::Interpolation(m_TerrainColor[i],m_TerrainColor[i+1],fTime);
				m_InterWaterColor=color::Interpolation(m_WaterColor[i],m_WaterColor[i+1],fTime);	

				m_InterFurGrassColor=color::Interpolation(m_FurGrassColor[i],m_FurGrassColor[i+1],fTime);	

				m_InterDetailFixColor = color::Interpolation(m_DetailFixColor[i],m_DetailFixColor[i+1],fTime);
				

				
			}
		}
		if(fTime>=REALDAYCYCLE-1)
		//if(DAYCYCLE-1 <= fTime)
		{
			fTime-=REALDAYCYCLE-1;
			AmbientColor=color::Interpolation(m_AmbientColor[DAYCYCLE-1],m_AmbientColor[0],fTime);		
			LightColor=color::Interpolation(m_LightColor[DAYCYCLE-1],m_LightColor[0],fTime);
			FogColor=color::Interpolation(m_FogColor[DAYCYCLE-1],m_FogColor[0],fTime);

			SkyCenterColor=color::Interpolation(m_SkyCenterColor[DAYCYCLE-1],m_SkyCenterColor[0],fTime);
			SkyNeighborColor=color::Interpolation(m_SkyNeighborColor[DAYCYCLE-1],m_SkyNeighborColor[0],fTime);

			fFogStart=m_FogRangeNear[DAYCYCLE-1]+(m_FogRangeNear[0]-m_FogRangeNear[DAYCYCLE-1])*(fTime-(int)fTime);
			fFogEnd=m_FogRangeFar[DAYCYCLE-1]+(m_FogRangeFar[0]-m_FogRangeFar[DAYCYCLE-1])*(fTime-(int)fTime);
			fSkyFogFar=m_SkyFogFar[DAYCYCLE-1]+(m_SkyFogFar[0]-m_SkyFogFar[DAYCYCLE-1])*(fTime-(int)fTime);

			//fSunTime=m_SunPosition[DAYCYCLE-1]+(m_SunPosition[0]-m_SunPosition[DAYCYCLE-1])*(fTime-(int)fTime);

			m_InterObjectAmbientColor=color::Interpolation(m_ObjectAmbientColor[DAYCYCLE-1],m_ObjectAmbientColor[0],fTime);

			dwFogLayerColor[0]=color::Interpolation(m_FogLayer[DAYCYCLE-1][0],m_FogLayer[0][0],fTime);
			dwFogLayerColor[1]=color::Interpolation(m_FogLayer[DAYCYCLE-1][1],m_FogLayer[0][1],fTime);
			dwFogLayerColor[2]=color::Interpolation(m_FogLayer[DAYCYCLE-1][2],m_FogLayer[0][2],fTime);

			m_InterSpecularReflectionRate=color::Interpolation(m_SpecularReflectionRate[DAYCYCLE-1],m_SpecularReflectionRate[0],fTime);
			m_InterGrassColor=color::Interpolation(m_GrassColor[DAYCYCLE-1],m_GrassColor[0],fTime);
			m_InterGlareSkyUpper=color::Interpolation(m_GlareSkyUpper[DAYCYCLE-1],m_GlareSkyUpper[0],fTime);
			m_InterGlareSkyLower=color::Interpolation(m_GlareSkyLower[DAYCYCLE-1],m_GlareSkyLower[0],fTime);
			m_InterCharacterAmbient=color::Interpolation(m_CharacterAmbient[DAYCYCLE-1],m_CharacterAmbient[0],fTime);
			m_InterCharacterLight=color::Interpolation(m_CharacterLight[DAYCYCLE-1],m_CharacterLight[0],fTime);
			m_InterTerrainColor=color::Interpolation(m_TerrainColor[DAYCYCLE-1],m_TerrainColor[0],fTime);		
			m_InterWaterColor=color::Interpolation(m_WaterColor[DAYCYCLE-1],m_WaterColor[0],fTime);
			m_InterFurGrassColor=color::Interpolation(m_FurGrassColor[DAYCYCLE-1],m_FurGrassColor[0],fTime);	
			m_InterDetailFixColor = color::Interpolation(m_DetailFixColor[DAYCYCLE-1],m_DetailFixColor[0],fTime);

		}	
		m_SkyScene.SetColor(SkyNeighborColor.c,SkyNeighborColor.c,(DWORD*)dwFogLayerColor);
		//m_CloudScene.SetColor(SkyNeighborColor.c,SkyNeighborColor.c,(DWORD*)dwFogLayerColor);
		if(m_ClearColor)
			m_ClearColor->c=FogColor.c;
		if(CRenderOption::m_FullSceneGlare)
		{	
			if(m_ClearColor)
				m_ClearColor->c=0x0;
			m_GF3ClareColor.c=FogColor.c;	
		}
		if(!pd3dDevice)
		{
			return;
		}
		pd3dDevice->SetRenderState( D3DRS_AMBIENT, AmbientColor.c);		
		m_InterAmbientColor=AmbientColor;
		
		
		pd3dDevice->SetRenderState( D3DRS_FOGENABLE,    TRUE );
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,     FogColor.c);
		pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_LINEAR );
		pd3dDevice->SetRenderState( D3DRS_FOGVERTEXMODE,  D3DFOG_LINEAR );
		pd3dDevice->SetRenderState( D3DRS_FOGSTART, *((DWORD *)(&fFogStart)) );
		pd3dDevice->SetRenderState( D3DRS_FOGEND,   *((DWORD *)(&fFogEnd)) );	

		m_InterSkyFogFar=fSkyFogFar;
		m_InterFogNear=fFogStart;
		m_InterFogFar=fFogEnd;
		
		
		

		/*
		if(m_SunPosition[i+1]==0.0f)
		{
			fSunTime=m_SunPosition[i]+(3.1415f*2.0f-m_SunPosition[i])*(fTime-(int)fTime);
		}
		else
		{
			fSunTime=m_SunPosition[i]+(m_SunPosition[i+1]-m_SunPosition[i])*(fTime-(int)fTime);
		}	
		// Calc Sun position
		float x=sinf(fSunTime);
		float z=cosf(fSunTime);
		float y=0.0f;	
		matrix matPosSun,matRotSun;
		matPosSun.Translation(vector3(x,y,z));	
		matRotSun.Rotation(vector3(1.0f,1.0f,1.0f),-3.1415f/3.0f);
		matPosSun=matPosSun*matRotSun;
		vector3 pos=matPosSun.GetLoc();
		//vector3 pos=vector3(1.0f,1.0f,1.0f);
		*/
		//matRotSun.Rotation(vector3(1.0f,1.0f,1.0f),-3.1415f/3.0f);
		float fAngle=cosf((fSunTime/24.0f)*3.14159f*2.0f);
		vector3 pos=vector3(0.5f,-fAngle,0.5f);
		pos.Normalize();

		//pos.Normalize();
		m_SunScene.SetSunDirection(pos);

		D3DLIGHT8 light; 
		ZeroMemory( &light, sizeof(D3DLIGHT8) );
		light.Type        = D3DLIGHT_DIRECTIONAL;
		light.Diffuse.r   = LightColor.r/255.0f;
		light.Diffuse.g   = LightColor.g/255.0f;
		light.Diffuse.b   = LightColor.b/255.0f;
		/*
		light.Position.x   = light.Direction.x = -pos.x;
		light.Position.y   = light.Direction.y = -pos.y;
		light.Position.z   = light.Direction.z = -pos.z;	
		*/	
		vector3 vecLight(-1.0f,-1.0f,-1.0f);
		vecLight.Normalize();
		light.Position.x   = light.Direction.x =vecLight.x;
		light.Position.y   = light.Direction.y =vecLight.y;
		light.Position.z   = light.Direction.z =vecLight.z;
		light.Range        = 1000.0f;	
		pd3dDevice->SetLight( 0,&light );		
		m_SunLight=light;

		light.Diffuse.r   = m_InterTerrainColor.r/255.0f;
		light.Diffuse.g   = m_InterTerrainColor.g/255.0f;
		light.Diffuse.b   = m_InterTerrainColor.b/255.0f;	
		m_TerrainLight=light;
		CSectorScene::m_TerrainDetailFixColor = m_InterDetailFixColor;
	}
}
void CWeatherManager::DeleteSubTable()
{
	m_iSubWeatherTableNum = 0;
	if(m_pWeaterTableRaw) 
	{
		delete[] m_pWeaterTableRaw;
		m_pWeaterTableRaw = NULL;
	}
	if(m_pSubWeatherTable)
	{
		delete[] m_pSubWeatherTable;
		m_pSubWeatherTable = NULL;

	}

	m_iSubCurrentIndex = 0;
	m_iBeforeCurrentIndex = 0;

	m_bSubTable = false;

}
void CWeatherManager::LoadSubTable(int *Arr,int iNum)
{
	m_bSubTable = false;
	DeleteSubTable();

	char strRawName[256] = {0,};
	char strSubTableName[256] = {0,};
	char strGrassName[256] = {0,};

	char *strPtr;
	FILE *fp = NULL;
	m_iSubWeatherTableNum = iNum;

	strcpy(strRawName,WEATHERPATH);
	strPtr = strrchr(strRawName,'.');
	strPtr++;
	*(strPtr) = 'r';
	strPtr++;
	*(strPtr) = 'a';
	strPtr++;
	*(strPtr) = 'w';
	strPtr++;
	
	fp = fopen(strRawName,"rb");
	if(fp == NULL)
	{
		return;
	}
	m_pWeaterTableRaw = new unsigned char[RAWTABLESIZE * RAWTABLESIZE];
	fread((void *)m_pWeaterTableRaw,sizeof(unsigned char) * RAWTABLESIZE * RAWTABLESIZE,1,fp);
	fclose(fp);

	if(Arr == NULL)
		return;
	
	m_pSubWeatherTable = new CSubWeatherData[iNum];
	for(int ik = 0; ik < iNum; ik++ )
	{
		int iCurrentIndex = Arr[ik];
		memset(strSubTableName,0,sizeof(char) * 256);
		memset(strGrassName,0,sizeof(char) * 256);
		strcpy(strSubTableName,WEATHERPATH);
		strPtr = strrchr(strSubTableName,'.');
		sprintf(strPtr,"_%d.dat",iCurrentIndex);
		fp = fopen(strSubTableName,"rb");
		if(fp == NULL)
		{
			delete[] m_pSubWeatherTable;
			m_pSubWeatherTable = NULL;
			delete[] m_pWeaterTableRaw;
			m_pWeaterTableRaw = NULL;

			return;
		}
		m_pSubWeatherTable[ik].m_iIndex = iCurrentIndex;
		
		sprintf(strGrassName,"grass_%d.dds",iCurrentIndex);	
		CTexture::SetPath(NATURETEXTUREPATH);		
		char strTestGrass[256] = {0,};
		sprintf(strTestGrass,"%s/%s",NATURETEXTUREPATH,strGrassName);
		FILE *fTest = fopen(strTestGrass,"rb");
		if(fTest)
		{
			fclose(fTest);
			m_pSubWeatherTable[ik].m_pGrassTexture = new CTexture();
			m_pSubWeatherTable[ik].m_pGrassTexture->Load(strGrassName);
		}
		else
		{
			m_pSubWeatherTable[ik].m_pGrassTexture = NULL;
		}
		DWORD color;
		float lens;
		char strNew[3];

		fread((char *)&strNew,sizeof(char) * 3,1,fp);
		if(strNew[0] == 'N' &&
			strNew[1] == 'E' &&
			strNew[2] == 'W')
		{
			for(int i=0;i<DAYCYCLE;i++)
			{
				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_SkyCenterColor[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_SkyNeighborColor[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_FogColor[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_LightColor[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_AmbientColor[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_FogLayer[i][0].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_FogLayer[i][1].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_FogLayer[i][2].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_ObjectAmbientColor[i].c=color;

				fread(&lens,sizeof(float),1,fp);
				m_pSubWeatherTable[ik].m_FogRangeNear[i]=lens;

				fread(&lens,sizeof(float),1,fp);
				m_pSubWeatherTable[ik].m_FogRangeFar[i]=lens;

				fread(&lens,sizeof(float),1,fp);
				m_pSubWeatherTable[ik].m_SkyFogFar[i]=lens;

				fread(&lens,sizeof(float),1,fp);
				m_pSubWeatherTable[ik].m_SunPosition[i]=lens;
				
				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_DetailFixColor[i].c = color;
				
				//CSectorScene::m_TerrainDetailFixColor.c=color;		

			}

			for(int i=0;i<DAYCYCLE;i++)
			{
				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_SpecularReflectionRate[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_GrassColor[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_GlareSkyUpper[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_GlareSkyLower[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_CharacterAmbient[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_CharacterLight[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_TerrainColor[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_WaterColor[i].c=color;		
				
				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_FurGrassColor[i].c=color;
			}
			if(!feof(fp))
			{
				char tmpHeader[2];
				fread((char *)tmpHeader,sizeof(char) * 2, 1, fp);
				if(tmpHeader[0] == 'N' &&
					tmpHeader[1] == 'C')
				{
					float fColor[5];
					fread((float *)fColor,sizeof(float) * 5,1,fp);
					
					CSceneManager::m_CharacterLight.Diffuse.r = fColor[0];
					CSceneManager::m_CharacterLight.Diffuse.g = fColor[1];
					CSceneManager::m_CharacterLight.Diffuse.b = fColor[2];
					CSceneManager::m_CharacterLight.Range = fColor[3];
					CSceneManager::m_fGlowPlaneRange = fColor[4];

				}

			}

		}
		else {
			fseek(fp,0,SEEK_SET);

			for(int i=0;i<DAYCYCLE;i++)
			{
				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_SkyCenterColor[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_SkyNeighborColor[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_FogColor[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_LightColor[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_AmbientColor[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_FogLayer[i][0].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_FogLayer[i][1].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_FogLayer[i][2].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_ObjectAmbientColor[i].c=color;

				fread(&lens,sizeof(float),1,fp);
				m_pSubWeatherTable[ik].m_FogRangeNear[i]=lens;

				fread(&lens,sizeof(float),1,fp);
				m_pSubWeatherTable[ik].m_FogRangeFar[i]=lens;

				fread(&lens,sizeof(float),1,fp);
				m_pSubWeatherTable[ik].m_SkyFogFar[i]=lens;

				fread(&lens,sizeof(float),1,fp);
				m_pSubWeatherTable[ik].m_SunPosition[i]=lens;
			}
			fread(&color,sizeof(DWORD),1,fp);
			CSectorScene::m_TerrainDetailFixColor.c=color;		

			for(int i=0;i<DAYCYCLE;i++)
			{
				m_pSubWeatherTable[ik].m_DetailFixColor[i].c = CSectorScene::m_TerrainDetailFixColor.c;

				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_SpecularReflectionRate[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_GrassColor[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_GlareSkyUpper[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_GlareSkyLower[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_CharacterAmbient[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_CharacterLight[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_TerrainColor[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_WaterColor[i].c=color;		
				
				fread(&color,sizeof(DWORD),1,fp);
				m_pSubWeatherTable[ik].m_FurGrassColor[i].c=color;
			}
		}
		fclose(fp);
	}
	m_bSubTable = true;

	
}
bool CWeatherManager::Load()
{
	FILE *fp=fopen(WEATHERPATH,"rb");
	if(fp!=NULL)
	{
		DWORD color;
		float lens;
		char strNew[3];

		fread((char *)&strNew,sizeof(char) * 3,1,fp);
		if(strNew[0] == 'N' &&
			strNew[1] == 'E' &&
			strNew[2] == 'W')
		{
			for(int i=0;i<DAYCYCLE;i++)
			{
				fread(&color,sizeof(DWORD),1,fp);
				m_SkyCenterColor[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_SkyNeighborColor[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_FogColor[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_LightColor[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_AmbientColor[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_FogLayer[i][0].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_FogLayer[i][1].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_FogLayer[i][2].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_ObjectAmbientColor[i].c=color;

				fread(&lens,sizeof(float),1,fp);
				m_FogRangeNear[i]=lens;

				fread(&lens,sizeof(float),1,fp);
				m_FogRangeFar[i]=lens;

				fread(&lens,sizeof(float),1,fp);
				m_SkyFogFar[i]=lens;

				fread(&lens,sizeof(float),1,fp);
				m_SunPosition[i]=lens;
				
				fread(&color,sizeof(DWORD),1,fp);
				m_DetailFixColor[i].c = color;
				
				//CSectorScene::m_TerrainDetailFixColor.c=color;		

			}

			for(int i=0;i<DAYCYCLE;i++)
			{
				fread(&color,sizeof(DWORD),1,fp);
				m_SpecularReflectionRate[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_GrassColor[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_GlareSkyUpper[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_GlareSkyLower[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_CharacterAmbient[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_CharacterLight[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_TerrainColor[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_WaterColor[i].c=color;		
				
				fread(&color,sizeof(DWORD),1,fp);
				m_FurGrassColor[i].c=color;
			}
			if(!feof(fp))
			{
				char tmpHeader[2];
				fread((char *)tmpHeader,sizeof(char) * 2, 1, fp);
				if(tmpHeader[0] == 'N' &&
					tmpHeader[1] == 'C')
				{
					float fColor[5];
					fread((float *)fColor,sizeof(float) * 5,1,fp);
					
					CSceneManager::m_CharacterLight.Diffuse.r = fColor[0];
					CSceneManager::m_CharacterLight.Diffuse.g = fColor[1];
					CSceneManager::m_CharacterLight.Diffuse.b = fColor[2];
					CSceneManager::m_CharacterLight.Range = fColor[3];
					CSceneManager::m_fGlowPlaneRange = fColor[4];

				}

			}

		}
		else {
			fseek(fp,0,SEEK_SET);

			for(int i=0;i<DAYCYCLE;i++)
			{
				fread(&color,sizeof(DWORD),1,fp);
				m_SkyCenterColor[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_SkyNeighborColor[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_FogColor[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_LightColor[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_AmbientColor[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_FogLayer[i][0].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_FogLayer[i][1].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_FogLayer[i][2].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_ObjectAmbientColor[i].c=color;

				fread(&lens,sizeof(float),1,fp);
				m_FogRangeNear[i]=lens;

				fread(&lens,sizeof(float),1,fp);
				m_FogRangeFar[i]=lens;

				fread(&lens,sizeof(float),1,fp);
				m_SkyFogFar[i]=lens;

				fread(&lens,sizeof(float),1,fp);
				m_SunPosition[i]=lens;
			}
			fread(&color,sizeof(DWORD),1,fp);
			CSectorScene::m_TerrainDetailFixColor.c=color;		

			for(int i=0;i<DAYCYCLE;i++)
			{
				m_DetailFixColor[i].c = CSectorScene::m_TerrainDetailFixColor.c;

				fread(&color,sizeof(DWORD),1,fp);
				m_SpecularReflectionRate[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_GrassColor[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_GlareSkyUpper[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_GlareSkyLower[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_CharacterAmbient[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_CharacterLight[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_TerrainColor[i].c=color;

				fread(&color,sizeof(DWORD),1,fp);
				m_WaterColor[i].c=color;		
				
				fread(&color,sizeof(DWORD),1,fp);
				m_FurGrassColor[i].c=color;
			}
		}
		fclose(fp);
		return true;
	}
	return false;
}
void CWeatherManager::SaveSub()
{
	if(m_bSubTable)
	{
		int iSub = m_iSubCurrentIndex;
		char strName[256] = {0,};
		char strN[256] = {0,};
		char *strPtr;

		strcpy(strName,WEATHERPATH);
		strPtr = strrchr(strName,'.');
		*(strPtr) = 0;
		strPtr++;
		sprintf(strN,"%s_%d.dat",strName,m_pSubWeatherTable[iSub].m_iIndex);

		FILE *fp=fopen(strN,"wb");
		if(fp!=NULL)
		{
			char strNew[4] = "NEW";
			fwrite((char *)strNew,sizeof(char) * 3,1,fp);

			DWORD color;
			float lens;
			for(int i=0;i<DAYCYCLE;i++)
			{
				color=m_pSubWeatherTable[iSub].m_SkyCenterColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);						

				color=m_pSubWeatherTable[iSub].m_SkyNeighborColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);		

				color=m_pSubWeatherTable[iSub].m_FogColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);			

				color=m_pSubWeatherTable[iSub].m_LightColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);		

				color=m_pSubWeatherTable[iSub].m_AmbientColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);			

				color=m_pSubWeatherTable[iSub].m_FogLayer[i][0].c;
				fwrite(&color,sizeof(DWORD),1,fp);			

				color=m_pSubWeatherTable[iSub].m_FogLayer[i][1].c;
				fwrite(&color,sizeof(DWORD),1,fp);			

				color=m_pSubWeatherTable[iSub].m_FogLayer[i][2].c;
				fwrite(&color,sizeof(DWORD),1,fp);			

				color=m_pSubWeatherTable[iSub].m_ObjectAmbientColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);			

				lens=m_pSubWeatherTable[iSub].m_FogRangeNear[i];
				fwrite(&lens,sizeof(float),1,fp);			

				lens=m_pSubWeatherTable[iSub].m_FogRangeFar[i];
				fwrite(&lens,sizeof(float),1,fp);			

				lens=m_pSubWeatherTable[iSub].m_SkyFogFar[i];
				fwrite(&lens,sizeof(float),1,fp);		

				lens=m_pSubWeatherTable[iSub].m_SunPosition[i];
				fwrite(&lens,sizeof(float),1,fp);
			
				color=m_pSubWeatherTable[iSub].m_DetailFixColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);	

			}
			

			for(int i=0;i<DAYCYCLE;i++)
			{
				color=m_pSubWeatherTable[iSub].m_SpecularReflectionRate[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);			
				
				color=m_pSubWeatherTable[iSub].m_GrassColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);		

				color=m_pSubWeatherTable[iSub].m_GlareSkyUpper[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);
				
				color=m_pSubWeatherTable[iSub].m_GlareSkyLower[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);
				
				color=m_pSubWeatherTable[iSub].m_CharacterAmbient[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);
				
				color=m_pSubWeatherTable[iSub].m_CharacterLight[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);			
				
				color=m_pSubWeatherTable[iSub].m_TerrainColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);

				color=m_pSubWeatherTable[iSub].m_WaterColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);			

				color=m_pSubWeatherTable[iSub].m_FurGrassColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);			
			}

			char tmpHeader[2];
			tmpHeader[0] = 'N';
			tmpHeader[1] = 'C';

			fwrite((char *)tmpHeader,sizeof(char) * 2, 1, fp);
			float fColor[5];
					
			fColor[0] = CSceneManager::m_CharacterLight.Diffuse.r;
			fColor[1] = CSceneManager::m_CharacterLight.Diffuse.g;
			fColor[2] = CSceneManager::m_CharacterLight.Diffuse.b;
			fColor[3] = CSceneManager::m_CharacterLight.Range;
			fColor[4] = CSceneManager::m_fGlowPlaneRange;

			fwrite((float *)fColor,sizeof(float) * 5,1,fp);
				

			/*for(int i=0;i<DAYCYCLE;i++)
			{
				color=m_SkyCenterColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);						

				color=m_SkyNeighborColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);		

				color=m_FogColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);			

				color=m_LightColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);		

				color=m_AmbientColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);			

				color=m_FogLayer[i][0].c;
				fwrite(&color,sizeof(DWORD),1,fp);			

				color=m_FogLayer[i][1].c;
				fwrite(&color,sizeof(DWORD),1,fp);			

				color=m_FogLayer[i][2].c;
				fwrite(&color,sizeof(DWORD),1,fp);			

				color=m_ObjectAmbientColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);			

				lens=m_FogRangeNear[i];
				fwrite(&lens,sizeof(float),1,fp);			

				lens=m_FogRangeFar[i];
				fwrite(&lens,sizeof(float),1,fp);			

				lens=m_SkyFogFar[i];
				fwrite(&lens,sizeof(float),1,fp);		

				lens=m_SunPosition[i];
				fwrite(&lens,sizeof(float),1,fp);
			}
			color=CSectorScene::m_TerrainDetailFixColor.c;
			fwrite(&color,sizeof(DWORD),1,fp);	

			for(i=0;i<DAYCYCLE;i++)
			{
				color=m_SpecularReflectionRate[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);			
				
				color=m_GrassColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);		

				color=m_GlareSkyUpper[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);
				
				color=m_GlareSkyLower[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);
				
				color=m_CharacterAmbient[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);
				
				color=m_CharacterLight[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);			
				
				color=m_TerrainColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);

				color=m_WaterColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);			

				color=m_FurGrassColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);			
			}
			*/
			fclose(fp);		
		}

		

	}
	else
	{
		FILE *fp=fopen(WEATHERPATH,"wb");
		if(fp!=NULL)
		{
			char strNew[4] = "NEW";
			fwrite((char *)strNew,sizeof(char) * 3,1,fp);

			DWORD color;
			float lens;
			for(int i=0;i<DAYCYCLE;i++)
			{
				color=m_SkyCenterColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);						

				color=m_SkyNeighborColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);		

				color=m_FogColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);			

				color=m_LightColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);		

				color=m_AmbientColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);			

				color=m_FogLayer[i][0].c;
				fwrite(&color,sizeof(DWORD),1,fp);			

				color=m_FogLayer[i][1].c;
				fwrite(&color,sizeof(DWORD),1,fp);			

				color=m_FogLayer[i][2].c;
				fwrite(&color,sizeof(DWORD),1,fp);			

				color=m_ObjectAmbientColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);			

				lens=m_FogRangeNear[i];
				fwrite(&lens,sizeof(float),1,fp);			

				lens=m_FogRangeFar[i];
				fwrite(&lens,sizeof(float),1,fp);			

				lens=m_SkyFogFar[i];
				fwrite(&lens,sizeof(float),1,fp);		

				lens=m_SunPosition[i];
				fwrite(&lens,sizeof(float),1,fp);
			
				color=m_DetailFixColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);	

			}
			

			for(int i=0;i<DAYCYCLE;i++)
			{
				color=m_SpecularReflectionRate[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);			
				
				color=m_GrassColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);		

				color=m_GlareSkyUpper[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);
				
				color=m_GlareSkyLower[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);
				
				color=m_CharacterAmbient[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);
				
				color=m_CharacterLight[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);			
				
				color=m_TerrainColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);

				color=m_WaterColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);			

				color=m_FurGrassColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);			
			}

			char tmpHeader[2];
			tmpHeader[0] = 'N';
			tmpHeader[1] = 'C';

			fwrite((char *)tmpHeader,sizeof(char) * 2, 1, fp);
			float fColor[5];
					
			fColor[0] = CSceneManager::m_CharacterLight.Diffuse.r;
			fColor[1] = CSceneManager::m_CharacterLight.Diffuse.g;
			fColor[2] = CSceneManager::m_CharacterLight.Diffuse.b;
			fColor[3] = CSceneManager::m_CharacterLight.Range;
			fColor[4] = CSceneManager::m_fGlowPlaneRange;

			fwrite((float *)fColor,sizeof(float) * 5,1,fp);
				

			/*for(int i=0;i<DAYCYCLE;i++)
			{
				color=m_SkyCenterColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);						

				color=m_SkyNeighborColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);		

				color=m_FogColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);			

				color=m_LightColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);		

				color=m_AmbientColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);			

				color=m_FogLayer[i][0].c;
				fwrite(&color,sizeof(DWORD),1,fp);			

				color=m_FogLayer[i][1].c;
				fwrite(&color,sizeof(DWORD),1,fp);			

				color=m_FogLayer[i][2].c;
				fwrite(&color,sizeof(DWORD),1,fp);			

				color=m_ObjectAmbientColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);			

				lens=m_FogRangeNear[i];
				fwrite(&lens,sizeof(float),1,fp);			

				lens=m_FogRangeFar[i];
				fwrite(&lens,sizeof(float),1,fp);			

				lens=m_SkyFogFar[i];
				fwrite(&lens,sizeof(float),1,fp);		

				lens=m_SunPosition[i];
				fwrite(&lens,sizeof(float),1,fp);
			}
			color=CSectorScene::m_TerrainDetailFixColor.c;
			fwrite(&color,sizeof(DWORD),1,fp);	

			for(i=0;i<DAYCYCLE;i++)
			{
				color=m_SpecularReflectionRate[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);			
				
				color=m_GrassColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);		

				color=m_GlareSkyUpper[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);
				
				color=m_GlareSkyLower[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);
				
				color=m_CharacterAmbient[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);
				
				color=m_CharacterLight[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);			
				
				color=m_TerrainColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);

				color=m_WaterColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);			

				color=m_FurGrassColor[i].c;
				fwrite(&color,sizeof(DWORD),1,fp);			
			}
			*/
			fclose(fp);		
		}
	}
}
void CWeatherManager::Save()
{
//	FILE *fp=fopen("c:\\WeatherColorTable.dat","wb");
	FILE *fp=fopen(WEATHERPATH,"wb");

	if(fp!=NULL)
	{
		char strNew[4] = "NEW";
		fwrite((char *)strNew,sizeof(char) * 3,1,fp);

		DWORD color;
		float lens;
		for(int i=0;i<DAYCYCLE;i++)
		{
			color=m_SkyCenterColor[i].c;
			fwrite(&color,sizeof(DWORD),1,fp);						

			color=m_SkyNeighborColor[i].c;
			fwrite(&color,sizeof(DWORD),1,fp);		

			color=m_FogColor[i].c;
			fwrite(&color,sizeof(DWORD),1,fp);			

			color=m_LightColor[i].c;
			fwrite(&color,sizeof(DWORD),1,fp);		

			color=m_AmbientColor[i].c;
			fwrite(&color,sizeof(DWORD),1,fp);			

			color=m_FogLayer[i][0].c;
			fwrite(&color,sizeof(DWORD),1,fp);			

			color=m_FogLayer[i][1].c;
			fwrite(&color,sizeof(DWORD),1,fp);			

			color=m_FogLayer[i][2].c;
			fwrite(&color,sizeof(DWORD),1,fp);			

			color=m_ObjectAmbientColor[i].c;
			fwrite(&color,sizeof(DWORD),1,fp);			

			lens=m_FogRangeNear[i];
			fwrite(&lens,sizeof(float),1,fp);			

			lens=m_FogRangeFar[i];
			fwrite(&lens,sizeof(float),1,fp);			

			lens=m_SkyFogFar[i];
			fwrite(&lens,sizeof(float),1,fp);		

			lens=m_SunPosition[i];
			fwrite(&lens,sizeof(float),1,fp);
		
			color=m_DetailFixColor[i].c;
			fwrite(&color,sizeof(DWORD),1,fp);	

		}
		

		for(int i=0;i<DAYCYCLE;i++)
		{
			color=m_SpecularReflectionRate[i].c;
			fwrite(&color,sizeof(DWORD),1,fp);			
			
			color=m_GrassColor[i].c;
			fwrite(&color,sizeof(DWORD),1,fp);		

			color=m_GlareSkyUpper[i].c;
			fwrite(&color,sizeof(DWORD),1,fp);
			
			color=m_GlareSkyLower[i].c;
			fwrite(&color,sizeof(DWORD),1,fp);
			
			color=m_CharacterAmbient[i].c;
			fwrite(&color,sizeof(DWORD),1,fp);
			
			color=m_CharacterLight[i].c;
			fwrite(&color,sizeof(DWORD),1,fp);			
			
			color=m_TerrainColor[i].c;
			fwrite(&color,sizeof(DWORD),1,fp);

			color=m_WaterColor[i].c;
			fwrite(&color,sizeof(DWORD),1,fp);			

			color=m_FurGrassColor[i].c;
			fwrite(&color,sizeof(DWORD),1,fp);			
		}

		char tmpHeader[2];
		tmpHeader[0] = 'N';
		tmpHeader[1] = 'C';

		fwrite((char *)tmpHeader,sizeof(char) * 2, 1, fp);
		float fColor[5];
				
		fColor[0] = CSceneManager::m_CharacterLight.Diffuse.r;
		fColor[1] = CSceneManager::m_CharacterLight.Diffuse.g;
		fColor[2] = CSceneManager::m_CharacterLight.Diffuse.b;
		fColor[3] = CSceneManager::m_CharacterLight.Range;
		fColor[4] = CSceneManager::m_fGlowPlaneRange;

		fwrite((float *)fColor,sizeof(float) * 5,1,fp);
			

		/*for(int i=0;i<DAYCYCLE;i++)
		{
			color=m_SkyCenterColor[i].c;
			fwrite(&color,sizeof(DWORD),1,fp);						

			color=m_SkyNeighborColor[i].c;
			fwrite(&color,sizeof(DWORD),1,fp);		

			color=m_FogColor[i].c;
			fwrite(&color,sizeof(DWORD),1,fp);			

			color=m_LightColor[i].c;
			fwrite(&color,sizeof(DWORD),1,fp);		

			color=m_AmbientColor[i].c;
			fwrite(&color,sizeof(DWORD),1,fp);			

			color=m_FogLayer[i][0].c;
			fwrite(&color,sizeof(DWORD),1,fp);			

			color=m_FogLayer[i][1].c;
			fwrite(&color,sizeof(DWORD),1,fp);			

			color=m_FogLayer[i][2].c;
			fwrite(&color,sizeof(DWORD),1,fp);			

			color=m_ObjectAmbientColor[i].c;
			fwrite(&color,sizeof(DWORD),1,fp);			

			lens=m_FogRangeNear[i];
			fwrite(&lens,sizeof(float),1,fp);			

			lens=m_FogRangeFar[i];
			fwrite(&lens,sizeof(float),1,fp);			

			lens=m_SkyFogFar[i];
			fwrite(&lens,sizeof(float),1,fp);		

			lens=m_SunPosition[i];
			fwrite(&lens,sizeof(float),1,fp);
		}
		color=CSectorScene::m_TerrainDetailFixColor.c;
		fwrite(&color,sizeof(DWORD),1,fp);	

		for(i=0;i<DAYCYCLE;i++)
		{
			color=m_SpecularReflectionRate[i].c;
			fwrite(&color,sizeof(DWORD),1,fp);			
			
			color=m_GrassColor[i].c;
			fwrite(&color,sizeof(DWORD),1,fp);		

			color=m_GlareSkyUpper[i].c;
			fwrite(&color,sizeof(DWORD),1,fp);
			
			color=m_GlareSkyLower[i].c;
			fwrite(&color,sizeof(DWORD),1,fp);
			
			color=m_CharacterAmbient[i].c;
			fwrite(&color,sizeof(DWORD),1,fp);
			
			color=m_CharacterLight[i].c;
			fwrite(&color,sizeof(DWORD),1,fp);			
			
			color=m_TerrainColor[i].c;
			fwrite(&color,sizeof(DWORD),1,fp);

			color=m_WaterColor[i].c;
			fwrite(&color,sizeof(DWORD),1,fp);			

			color=m_FurGrassColor[i].c;
			fwrite(&color,sizeof(DWORD),1,fp);			
		}
		*/
		fclose(fp);
	}	
}

void CWeatherManager::Update(float fUpdate)
{
	m_SkyScene.Update(fUpdate);	
}

void CWeatherManager::SecondRender(LPDIRECT3DDEVICE8 pd3dDevice)
{
	matrix matInit;
	matInit.MakeIdent();
	pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)(&matInit));	
	m_SunScene.RenderFlare(pd3dDevice);
}
// Minotaurs WeatherCorlor얻어오기
int CWeatherManager::GetWeatherCorlor()
{
	if(m_pSubWeatherTable == NULL )
		return 0;

	return m_pSubWeatherTable[m_iSubCurrentIndex].m_iIndex;
}