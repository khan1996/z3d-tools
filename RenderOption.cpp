// RenderOption.cpp: implementation of the CRenderOption class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include "RenderOption.h"
#include "BaseDataDefine.h"

#include <stdio.h>
#include <string>
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int CRenderOption::m_GrassRendering = 1;
int CRenderOption::m_GrassAnimationUseVertexShader = 0;
int CRenderOption::m_GrassRenderRange = 9;
int CRenderOption::m_RangeGrassRender = 0;

int CRenderOption::m_FullSceneGlare = 0;
int CRenderOption::m_FullSceneAnti = 0;
int CRenderOption::m_FullSceneSpecularGlare = 0;

int CRenderOption::m_WaterBumpEnvRendering = 0;

int CRenderOption::m_TerrainMultiDetalTexture = 1;

int CRenderOption::m_TerrainRange = 7;

int CRenderOption::m_BuildingLightmap = 1;

int CRenderOption::m_TreeAnimation = 1;

int CRenderOption::m_CharacterPerPixelLighting = 0;
int CRenderOption::m_CharacterPerPixelSpecularLighting = 0;
int CRenderOption::m_CharacterSelfShadow = 0;

int CRenderOption::m_DepthOfField = 0;
int CRenderOption::m_CharacterProjectShadowTerrain = 1;
int CRenderOption::m_CharacterProjectShadowBuilding = 1;

int CRenderOption::m_ObjectLOD = 4;
int CRenderOption::m_TextureMethod = 1;
char CRenderOption::m_strBaseGraphicsDataPath[256];

int CRenderOption::m_ObjectProjectShadow = 0;
int CRenderOption::m_TreeProjectShadow = 0;

int CRenderOption::m_ScreenFilter = 0;

int CRenderOption::m_BSPDiffusePerPixel = 0;
int CRenderOption::m_BSPSpecularPerPixel = 0;

int CRenderOption::m_AllObjectBump = 0;

int CRenderOption::m_bWeather = 0;
int CRenderOption::m_bFullSceneEffect = 0;

//bool CRenderOption::LoadZoneInfo(CRenderOption::ZoneInfo *);		// � info File Load

CRenderOption::CRenderOption()
{
	 
}

CRenderOption::~CRenderOption()
{

}

bool CRenderOption::Save(HKEY &hReg)
{
	if(!hReg) return false;

	long lResult;
//	long lResult = RegDeleteValue(hReg, "RenderOption");
//	if(lResult != ERROR_SUCCESS) return false;

	unsigned long dwRegValue[23];
	dwRegValue[0] = m_GrassRendering;
	dwRegValue[1] = m_GrassAnimationUseVertexShader;
	dwRegValue[2] = m_GrassRenderRange;
	dwRegValue[3] = m_RangeGrassRender;
	dwRegValue[4] = m_FullSceneGlare;
	dwRegValue[5] = m_FullSceneAnti;
	dwRegValue[6] = m_FullSceneSpecularGlare;
	dwRegValue[7] = m_WaterBumpEnvRendering;
	dwRegValue[8] = m_TerrainMultiDetalTexture;
	dwRegValue[9] = m_TerrainRange;
	dwRegValue[10] = m_BuildingLightmap;
	dwRegValue[11] = m_TreeAnimation;
	dwRegValue[12] = m_CharacterPerPixelLighting;
	dwRegValue[13] = m_CharacterPerPixelSpecularLighting;
	dwRegValue[14] = m_CharacterSelfShadow;
	dwRegValue[15] = m_DepthOfField;
	dwRegValue[16] = m_CharacterProjectShadowTerrain;
	dwRegValue[17] = m_CharacterProjectShadowBuilding;
	dwRegValue[18] = m_ObjectLOD;
	dwRegValue[19] = m_TextureMethod;
	dwRegValue[20] = m_ScreenFilter;
	dwRegValue[21] = m_bWeather;
	dwRegValue[22] = m_bFullSceneEffect;
	lResult = RegSetValueEx(hReg, "RenderOption", 0, REG_BINARY, (LPBYTE)&dwRegValue, sizeof(dwRegValue));
	if(lResult == ERROR_SUCCESS) return true;

	return false;
}
/*	FILE *fp=fopen(strFilename,"wb");
	if(fp)
	{
		fwrite(&m_GrassRendering,sizeof(int), 1, fp);
		fwrite(&m_GrassAnimationUseVertexShader,sizeof(int), 1, fp);
		fwrite(&m_GrassRenderRange,sizeof(int), 1, fp);
		fwrite(&m_RangeGrassRender,sizeof(int), 1, fp);
		fwrite(&m_FullSceneGlare,sizeof(int), 1, fp);
		fwrite(&m_FullSceneAnti,sizeof(int), 1, fp);
		fwrite(&m_FullSceneSpecularGlare,sizeof(int), 1, fp);
		fwrite(&m_WaterBumpEnvRendering,sizeof(int), 1, fp);
		fwrite(&m_TerrainMultiDetalTexture,sizeof(int), 1, fp);
		fwrite(&m_TerrainRange,sizeof(int), 1, fp);
		fwrite(&m_BuildingLightmap,sizeof(int), 1, fp);
		fwrite(&m_TreeAnimation,sizeof(int), 1, fp);
		fwrite(&m_CharacterPerPixelLighting,sizeof(int), 1, fp);
		fwrite(&m_CharacterPerPixelSpecularLighting,sizeof(int), 1, fp);
		fwrite(&m_CharacterSelfShadow,sizeof(int), 1, fp);
		fwrite(&m_DepthOfField,sizeof(int), 1, fp);
		fwrite(&m_CharacterProjectShadowTerrain,sizeof(int), 1, fp);
		fwrite(&m_CharacterProjectShadowBuilding,sizeof(int), 1, fp);
		fwrite(&m_ObjectLOD,sizeof(int), 1, fp);
		fwrite(&m_TextureMethod,sizeof(int), 1, fp);		
		fwrite(&m_ScreenFilter,sizeof(int), 1, fp);		
		fclose(fp);
	}	
}*/


bool CRenderOption::Load(HKEY &hReg)
{
	if(!hReg) return false;

	unsigned long dwRegValue[23], dwReadLens = sizeof(dwRegValue);
	long lResult = RegQueryValueEx(hReg, "RenderOption", 0, NULL, (LPBYTE)&dwRegValue, &dwReadLens);
	if(lResult != ERROR_SUCCESS) return false;
	m_GrassRendering = dwRegValue[0];
	m_GrassAnimationUseVertexShader = dwRegValue[1];
	m_GrassRenderRange = dwRegValue[2];
	m_RangeGrassRender = dwRegValue[3];
	m_FullSceneGlare = dwRegValue[4];
	m_FullSceneAnti = dwRegValue[5];
	m_FullSceneSpecularGlare = dwRegValue[6];
	m_WaterBumpEnvRendering = dwRegValue[7];
	m_TerrainMultiDetalTexture = dwRegValue[8];
	m_TerrainRange = dwRegValue[9];
	m_BuildingLightmap = dwRegValue[10];
	m_TreeAnimation = dwRegValue[11];
	m_CharacterPerPixelLighting = dwRegValue[12];
	m_CharacterPerPixelSpecularLighting = dwRegValue[13];
	m_CharacterSelfShadow = dwRegValue[14];
	m_DepthOfField = dwRegValue[15];
	m_CharacterProjectShadowTerrain = dwRegValue[16];
	m_CharacterProjectShadowBuilding = dwRegValue[17];
	m_ObjectLOD = dwRegValue[18];
	m_TextureMethod = dwRegValue[19];
	m_ScreenFilter = dwRegValue[20];
	m_bWeather = dwRegValue[21];
	m_bFullSceneEffect = dwRegValue[22];

	return true;
/*
	if(!hReg) return false;

	unsigned long dwReadLens = sizeof(unsigned long);
	long lResult;
	bool bReturn = true;
	lResult = RegQueryValueEx(hReg, "GR", 0, NULL, (LPBYTE)&m_GrassRendering, &dwReadLens);
	if(lResult != ERROR_SUCCESS) bReturn = false;
	lResult = RegQueryValueEx(hReg, "GA", 0, NULL, (LPBYTE)&m_GrassAnimationUseVertexShader, &dwReadLens);
	if(lResult != ERROR_SUCCESS) bReturn = false;
	lResult = RegQueryValueEx(hReg, "GRR", 0, NULL, (LPBYTE)&m_GrassRenderRange, &dwReadLens);
	if(lResult != ERROR_SUCCESS) bReturn = false;
	lResult = RegQueryValueEx(hReg, "RGR", 0, NULL, (LPBYTE)&m_RangeGrassRender, &dwReadLens);
	if(lResult != ERROR_SUCCESS) bReturn = false;
	lResult = RegQueryValueEx(hReg, "FSG", 0, NULL, (LPBYTE)&m_FullSceneGlare, &dwReadLens);
	if(lResult != ERROR_SUCCESS) bReturn = false;
	lResult = RegQueryValueEx(hReg, "FSA", 0, NULL, (LPBYTE)&m_FullSceneAnti, &dwReadLens);
	if(lResult != ERROR_SUCCESS) bReturn = false;
	lResult = RegQueryValueEx(hReg, "FSSG", 0, NULL, (LPBYTE)&m_FullSceneSpecularGlare, &dwReadLens);
	if(lResult != ERROR_SUCCESS) bReturn = false;
	lResult = RegQueryValueEx(hReg, "WBER", 0, NULL, (LPBYTE)&m_WaterBumpEnvRendering, &dwReadLens);
	if(lResult != ERROR_SUCCESS) bReturn = false;
	lResult = RegQueryValueEx(hReg, "TMDT", 0, NULL, (LPBYTE)&m_TerrainMultiDetalTexture, &dwReadLens);
	if(lResult != ERROR_SUCCESS) bReturn = false;
	lResult = RegQueryValueEx(hReg, "TR", 0, NULL, (LPBYTE)&m_TerrainRange, &dwReadLens);
	if(lResult != ERROR_SUCCESS) bReturn = false;
	lResult = RegQueryValueEx(hReg, "BL", 0, NULL, (LPBYTE)&m_BuildingLightmap, &dwReadLens);
	if(lResult != ERROR_SUCCESS) bReturn = false;
	lResult = RegQueryValueEx(hReg, "TA", 0, NULL, (LPBYTE)&m_TreeAnimation, &dwReadLens);
	if(lResult != ERROR_SUCCESS) bReturn = false;
	lResult = RegQueryValueEx(hReg, "CPPL", 0, NULL, (LPBYTE)&m_CharacterPerPixelLighting, &dwReadLens);
	if(lResult != ERROR_SUCCESS) bReturn = false;
	lResult = RegQueryValueEx(hReg, "CPPSL", 0, NULL, (LPBYTE)&m_CharacterPerPixelSpecularLighting, &dwReadLens);
	if(lResult != ERROR_SUCCESS) bReturn = false;
	lResult = RegQueryValueEx(hReg, "CSS", 0, NULL, (LPBYTE)&m_CharacterSelfShadow, &dwReadLens);
	if(lResult != ERROR_SUCCESS) bReturn = false;
	lResult = RegQueryValueEx(hReg, "DOF", 0, NULL, (LPBYTE)&m_DepthOfField, &dwReadLens);
	if(lResult != ERROR_SUCCESS) bReturn = false;
	lResult = RegQueryValueEx(hReg, "CPST", 0, NULL, (LPBYTE)&m_CharacterProjectShadowTerrain, &dwReadLens);
	if(lResult != ERROR_SUCCESS) bReturn = false;
	lResult = RegQueryValueEx(hReg, "CPSB", 0, NULL, (LPBYTE)&m_CharacterProjectShadowBuilding, &dwReadLens);
	if(lResult != ERROR_SUCCESS) bReturn = false;
	lResult = RegQueryValueEx(hReg, "OLOD", 0, NULL, (LPBYTE)&m_ObjectLOD, &dwReadLens);
	if(lResult != ERROR_SUCCESS) bReturn = false;
	lResult = RegQueryValueEx(hReg, "TM", 0, NULL, (LPBYTE)&m_TextureMethod, &dwReadLens);
	if(lResult != ERROR_SUCCESS) bReturn = false;
	lResult = RegQueryValueEx(hReg, "SF", 0, NULL, (LPBYTE)&m_ScreenFilter, &dwReadLens);
	if(lResult != ERROR_SUCCESS) bReturn = false;
	lResult = RegQueryValueEx(hReg, "W", 0, NULL, (LPBYTE)&m_bWeather, &dwReadLens);
	if(lResult != ERROR_SUCCESS) bReturn = false;
*/
//	return bReturn;
/*	FILE *fp=fopen(strFilename,"rb");
	if(fp)
	{
		fread(&m_GrassRendering,sizeof(int), 1, fp);
		fread(&m_GrassAnimationUseVertexShader,sizeof(int), 1, fp);
		fread(&m_GrassRenderRange,sizeof(int), 1, fp);
		fread(&m_RangeGrassRender,sizeof(int), 1, fp);
		fread(&m_FullSceneGlare,sizeof(int), 1, fp);
		fread(&m_FullSceneAnti,sizeof(int), 1, fp);
		fread(&m_FullSceneSpecularGlare,sizeof(int), 1, fp);
		fread(&m_WaterBumpEnvRendering,sizeof(int), 1, fp);
		fread(&m_TerrainMultiDetalTexture,sizeof(int), 1, fp);
		fread(&m_TerrainRange,sizeof(int), 1, fp);
		fread(&m_BuildingLightmap,sizeof(int), 1, fp);
		fread(&m_TreeAnimation,sizeof(int), 1, fp);
		fread(&m_CharacterPerPixelLighting,sizeof(int), 1, fp);
		fread(&m_CharacterPerPixelSpecularLighting,sizeof(int), 1, fp);
		fread(&m_CharacterSelfShadow,sizeof(int), 1, fp);
		fread(&m_DepthOfField,sizeof(int), 1, fp);
		fread(&m_CharacterProjectShadowTerrain,sizeof(int), 1, fp);
		fread(&m_CharacterProjectShadowBuilding,sizeof(int), 1, fp);
		fread(&m_ObjectLOD,sizeof(int), 1, fp);
		fread(&m_TextureMethod,sizeof(int), 1, fp);
		fread(&m_ScreenFilter,sizeof(int), 1, fp);
		//m_WaterBumpEnvRendering = 1;
		fclose(fp);
		return true;
	}
	return false;*/
}
bool CRenderOption::LoadZoneInfo(ZoneInfo *pZoneInfo)				// � info File Load
{
	if(pZoneInfo == NULL)
		return false;
	if(!strcmp(CRenderOption::m_strBaseGraphicsDataPath,""))
	{
		pZoneInfo->m_bZoneInfo = false;
		return false;
	}
	FILE *fp = NULL;
	char strInfo[256] = {0};
	
	sprintf(strInfo,"%s%s.info",ROOTPATH,m_strBaseGraphicsDataPath);
	
	fp = fopen(strInfo,"rb");
	if(fp == NULL) 
	{
		pZoneInfo->m_bZoneInfo = false;
		return false;
	}
	// Data Load
	pZoneInfo->m_bZoneInfo = true;
	fread((int *)pZoneInfo->m_iZoneInfo,sizeof(int),30,fp);			// Int Value Load
	fread((float *)pZoneInfo->m_fZoneInfo,sizeof(float),30,fp);		// float Value Load
	fclose(fp);
	return true;

}