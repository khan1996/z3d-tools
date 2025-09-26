// WeatherManager.h: interface for the CWeatherManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WEATHERMANAGER_H__61B8FDF0_B967_4933_A397_7839B82C6314__INCLUDED_)
#define AFX_WEATHERMANAGER_H__61B8FDF0_B967_4933_A397_7839B82C6314__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "SkyScene.h"
#include "SunScene.h"
#include "FallScene.h"
#include "CloudScene.h"

const long DAYCYCLE=25;
const int RAWTABLESIZE = 512;

class CTexture;
class CWeatherManager  
{		
public:
	class CSubWeatherData
	{
	public:
		CSubWeatherData();
		~CSubWeatherData();

		int m_iIndex;
		CTexture *m_pGrassTexture;
		color m_SkyNeighborColor[DAYCYCLE];
		color m_SkyCenterColor[DAYCYCLE];
		color m_FogColor[DAYCYCLE];
		color m_LightColor[DAYCYCLE];
		color m_AmbientColor[DAYCYCLE];
		float m_FogRangeNear[DAYCYCLE];
		float m_FogRangeFar[DAYCYCLE];
		float m_SkyFogFar[DAYCYCLE];
		color m_FogLayer[DAYCYCLE][3];
		float m_SunPosition[DAYCYCLE];
		color m_ObjectAmbientColor[DAYCYCLE];

		color m_SpecularReflectionRate[DAYCYCLE];
		color m_GrassColor[DAYCYCLE];
		color m_GlareSkyUpper[DAYCYCLE];
		color m_GlareSkyLower[DAYCYCLE];
		color m_CharacterAmbient[DAYCYCLE];
		color m_CharacterLight[DAYCYCLE];
		color m_TerrainColor[DAYCYCLE];
		color m_WaterColor[DAYCYCLE];
		color m_FurGrassColor[DAYCYCLE];
		color m_DetailFixColor[DAYCYCLE];

	};

	float m_fWeatherTime;
	bool m_CustomWaterColor;
	float m_CustomWaterRate;
	void SecondRender(LPDIRECT3DDEVICE8 pd3dDevice);
	void Update(float fUpdate);
	CSkyScene m_SkyScene;	
	CSunScene m_SunScene;
//	CFallScene m_FallScene;
//	CCloudScene m_CloudScene;

	void Save();
	bool Load();
	void LoadSubTable(int *Arr,int iNum);
	void DeleteSubTable();


	color *m_ClearColor;
	float m_InterSkyFogFar;
	float m_InterSkyFogNear;
	float m_InterFogFar;
	float m_InterFogNear;
	color m_InterObjectAmbientColor;
	color m_InterAmbientColor;
	color m_TerrainAdderColor;
	D3DLIGHT8 m_SunLight;
	color m_GF3ClareColor;

	color m_InterSpecularReflectionRate;
	color m_InterGrassColor;
	color m_InterGlareSkyUpper;
	color m_InterGlareSkyLower;
	color m_InterCharacterAmbient;
	color m_InterCharacterLight;
	color m_InterTerrainColor;
	color m_InterWaterColor;
	color m_InterFurGrassColor;
	color m_InterDetailFixColor;

	D3DLIGHT8 m_TerrainLight;

	color m_SkyNeighborColor[DAYCYCLE];
	color m_SkyCenterColor[DAYCYCLE];
	color m_FogColor[DAYCYCLE];
	color m_LightColor[DAYCYCLE];
	color m_AmbientColor[DAYCYCLE];
	float m_FogRangeNear[DAYCYCLE];
	float m_FogRangeFar[DAYCYCLE];
	float m_SkyFogFar[DAYCYCLE];
	color m_FogLayer[DAYCYCLE][3];
	float m_SunPosition[DAYCYCLE];
	color m_ObjectAmbientColor[DAYCYCLE];

	color m_SpecularReflectionRate[DAYCYCLE];
	color m_GrassColor[DAYCYCLE];
	color m_GlareSkyUpper[DAYCYCLE];
	color m_GlareSkyLower[DAYCYCLE];
	color m_CharacterAmbient[DAYCYCLE];
	color m_CharacterLight[DAYCYCLE];
	color m_TerrainColor[DAYCYCLE];
	color m_WaterColor[DAYCYCLE];
	color m_FurGrassColor[DAYCYCLE];
	color m_DetailFixColor[DAYCYCLE];
	
	CSubWeatherData *m_pSubWeatherTable;
	int	m_iSubWeatherTableNum;

	bool m_bSubTable;
	int m_iSubCurrentIndex;
	int m_iBeforeCurrentIndex;

	float m_fInterpol;
	


	unsigned char *m_pWeaterTableRaw;


	/*
	DDX_ColourPicker(pDX,IDC_COLOR_SPECULARREFLRECTIONRATE, m_cSpecularReflectionRate);
	DDX_ColourPicker(pDX,IDC_COLOR_GRASSAMBIENT, m_cGrass);
	DDX_ColourPicker(pDX,IDC_COLOR_GLARESKYUPPER, m_cGlareSkyUpper);
	DDX_ColourPicker(pDX,IDC_COLOR_GLARESKYLOWER, m_cClareSkyLower);
	DDX_ColourPicker(pDX,IDC_COLOR_CHARACTERAMBIENT, m_cCharacterAmbient);
	DDX_ColourPicker(pDX,IDC_COLOR_CHARACTERLIGHT, m_cCharacterLight);
	DDX_ColourPicker(pDX,IDC_COLOR_TERRAINCOLOR,m_cTerrainColor);
	*/


	void SetClearColor(color *ClearColor){m_ClearColor=ClearColor;};

	int GetWeatherCorlor();

	void SetWeather(float fTime,LPDIRECT3DDEVICE8 pd3dDevice);
	void Render(LPDIRECT3DDEVICE8 pd3dDevice);
	void RenderEnv(LPDIRECT3DDEVICE8	pd3dDevice);

	void Reload();

	void Create();
	CWeatherManager();
	virtual ~CWeatherManager();
	int GetSubWeatherNum(vector3 *vecPos);
	CTexture *GetCurrentSubGrassTexture();
	void SaveSub();

};

#endif // !defined(AFX_WEATHERMANAGER_H__61B8FDF0_B967_4933_A397_7839B82C6314__INCLUDED_)
