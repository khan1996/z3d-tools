// MapStorage.h: interface for the CMapStorage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPSTORAGE_H__E37090A2_3209_4878_8E2D_59AC76DAA3E6__INCLUDED_)
#define AFX_MAPSTORAGE_H__E37090A2_3209_4878_8E2D_59AC76DAA3E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "SectorDungeonMap.h"
#include "SectorHeightMap.h"
#include "SectorMeshMap.h"
#include "SectorPlantMap.h"
#include "SectorWideMap.h"
#include "SectorDefine.h"
#include "SectorHouseMap.h"
#include "SectorWaterMap.h"
#include "SectorFallMap.h"
#include "InHouseObjectMap.h"
#include "SectorMustDivideVertexMap.h"
#include "SectorEffectMap.h"
#include "SectorLandscapeEffectMap.h"
//#include "SectorSoundMap.h"
#include "StateLog.h"
#include "SectorLight.h"

#include <vector.h>

#define MAXSECTOR 144
class CMapStorage  
{
	CSectorDungeonMap* m_DundeonMapTable[MAXSECTOR];
	CSectorMeshMap* m_MeshMapTable[MAXSECTOR];
	CSectorPlantMap* m_PlantMapTable[MAXSECTOR];
	CSectorHouseMap* m_HouseMapTable[MAXSECTOR];
	CSectorWaterMap* m_WaterMapTable[MAXSECTOR];
	CSectorFallMap* m_FallMapTable[MAXSECTOR];
	CInHouseObjectMap *m_InHouseMapTable[MAXSECTOR];
	CSectorMustDivideVertexMap* m_MustDivideVertexMapTable[MAXSECTOR];
	CSectorEffectMap* m_EffectMapTable[MAXSECTOR];
	CSectorLightMap *m_LightMapTable[MAXSECTOR];

	 std::vector<CSectorDungeonMap*> m_DundeonMap;
	 
	 std::vector<CSectorMeshMap*> m_MeshMap;
	 std::vector<CSectorPlantMap*> m_PlantMap;
	 
	 std::vector<CSectorWaterMap*> m_WaterMap;
	 std::vector<CSectorFallMap*> m_FallMap;
	 std::vector<CInHouseObjectMap*> m_InHouseMap;
	 std::vector<CSectorMustDivideVertexMap*> m_MustDivideVertexMap;
	 std::vector<CSectorEffectMap*> m_EffectMap;
	 std::vector<CSectorLightMap *> m_LightMap;

	 int m_LoadMode;
	 char m_strMapFilename[256];
public:
	std::vector<CSectorHouseMap*> m_HouseMap;
	CSectorLandscapeEffectMap* m_LandscapeEffectMapTable[MAXSECTOR];
	CSectorHeightMap *m_HeightMapTable[MAXSECTOR];
	CSectorWideMap *m_WideMapTable[MAXSECTOR];

	std::vector<CSectorLandscapeEffectMap*> m_LandscapeEffectMap;

	std::vector<CSectorHeightMap*> m_HeightMap;
	std::vector<CSectorWideMap*> m_WideMap;//이거 고쳐야 함


	void AddSectorLight(float fAddx,float fAddy,float fAddz,float fR,float fG,float fB,float fRad,int iLights,int Shadows,float fShadow,int iAmb,char *strEffect,float fExpose);
	void DeleteSectorLight(float fAddx,float fAddy,float fAddz);
	CSectorLightMap *FindLightMap(float fFindx, float fFindy);

	void AddLandscapeEffect(float fAddx,float fAddy,float fAddz,int nEffectMethod,DWORD dwBlend[2],float Param[50],int IntParam[20],char *strTextureName,char *eff);
	void DeleteLandscapeEffect(CSectorLandscapeEffectMap*);

	void Read( void* tBuf, INT len, const char* sBuf, LONG& seek );
	void ConvertData(char * data, LONG len);

	void LoadGF3(char *strFilename);
	void LoadGF3Mem(char *strFilename);

	void SaveGF3(char *strFilename);
//	void LoadSoundMap(char *strSoundMapFilename);
//	void SaveSoundMap(char *strSoundMapFilename);
//	CSectorSoundMap* FindSoundMap(float fFindx,float fFindy);
//	void AddSoundMap(float fAddx,float fAddy,float fAddz,char *strFilename,float fMinRange,float fMaxRange);
	void SetLoadMode(int mode){m_LoadMode=mode;};
	 void DeleteAllObject();
	 void BugFix_Tree2(char *strFilename);
	 void BugFix_Hangul2English();
	 void BugFix_Tree();	
	
	 void DelMeshMap(float fDelx,float fDely,float fDelz,long ObjectSceneID);
	 void AddMeshMap(float fAddx,float fAddy,float fAddz,char *strObjectName,matrix matTM,bool isAlpha,bool isLight);
	 void DelPlantMap(float fDelx,float fDelY,float fDelz);
	 void DelInHouseLight(char *strOutName, char *strMedName, char *strInName,long LightID);
	 void DelInHouseObject(char *strOutName, char *strMedName, char *strInName,long ObjectID);
	 void DeleteAllData();
	
	 long m_TotalHouseSceneCount;
	 long m_TotalInHouseLightSceneCount;
	 long m_TotalInHouseObjectSceneCount;
	 long m_TotalObjectSceneCount;
	 long m_totalEffectSceneCount;
	 long m_TotalLightCount;

	 void AddMustDivideVertex(CSectorMustDivideVertexMap *pMustDivideVertex);
	 void AddPlantMap(float fAddx,float fAddy,float fAddz,unsigned char PlantKind);
	 void AddInHouseLight(char *strOutName, char *strMedName, char *strInName, char *strBspName,char *strLightName, matrix matTM,float fLightRange,color LightColor);
	 void AddInHouseObject(char *strOutName,char *strMedName,char *strInName,char *strBspName,char *strObjectName,matrix matTM,bool isAlpha,bool isLight);
	 void AddFallMap(float fAddx,float fAddy,float fAddz,float fFallPosX,float fFallPosY,float fFallRot,std::vector<float> &FallHeight,std::vector<float> &FallLeft,std::vector<float> &FallRight,std::vector<float> &FallAddX,std::vector<color> &WaterFallColor);
	 void AddWideMap(float fAddx,float fAddy,float fAddz,char *strWideMapName,char *strDetailMapName,char *strWideMapName1,char *strDetailMapName1,char *strWideMapName2,char *strDetailMapName2);
	 void AddHouseMap(float fAddx,float fAddy,float fAddz,char *strOutName,char *strMedName,char *strInName,char *strBspName);
	 void AddHeightMap(float fAddx,float fAddy,float fAddz,float *fHeightData);
	 void AddWaterMap(float fAddx,float fAddy,float fAddz,float fWaterHeight,float fWaterSizeX,float fWaterSizeY,float fWaterPosX,float fWaterPosY,color WaterColor,bool bWaterRelection);
	 void AddEffectMap(float fAddx,float fAddy,float fAddz,char *filename,float xrot,float yrot,float zrot);
	
	 void DelEffectMap(float fAddx,float fAddy,float fAddz,char *filename);
	 void DelHouseMap(float fDelx,float fDely,float fDelz,long HouseSceneID);	
	 void DeleteHouseMap(float fDelx,float fDely,float fDelz,char *filename);


	 CSectorMustDivideVertexMap* FindMustDivideVertexMap(float fFindX,float fFindY);
	 CInHouseObjectMap* FindInHouseMap(char *strOutName, char *strMedName, char *strInName,char *strBspName);
	 CSectorHouseMap* FindHouseMap(float fFindx,float fFindy);
	 CSectorDungeonMap* FindDungeonMap(float fFindx,float fFindy);
	 CSectorHeightMap* FindHeightMap(float fFindx, float fFindy);
	 CSectorMeshMap* FindMeshMap(float fFindx, float fFindy);
	 CSectorPlantMap* FindPlantMap(float fFindx, float fFindy);
	 CSectorWideMap* FindWideMap(float fFindx, float fFindy);
	 CSectorWaterMap* FindWaterMap(float fFindx, float fFindy);
	 CSectorFallMap* FindFallMap(float fFindx,float fFindy);
	 CSectorEffectMap* FindEffectMap(float fFindx, float fFindy);
	 CSectorEffectMap* FindEffectMap(int indexX,int indexY);
	 CSectorEffectMap* FindEffectMap(int &index);
	 CSectorLandscapeEffectMap* FindLandscapeEffect(float fFindx,float fFindy);	

	 void Save(char *strFilename);
	 void Load(char *strFilename);
	 void LoadInHouseMap(char *strFilename);
	 void SaveInHouseMap(char *strFilename);
	 void OldLoad(char *strFilename);

	 void SaveSector(char *strFilename,long SectorX,long SectorY);
	 void LoadSector(char *strFilename,long SectorX,long SectorY);
//	 void LoadMapOutHouse();

	CMapStorage();
	virtual ~CMapStorage();
};

#endif // !defined(AFX_MAPSTORAGE_H__E37090A2_3209_4878_8E2D_59AC76DAA3E6__INCLUDED_)
