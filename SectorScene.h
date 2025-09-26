// SectorScene.h: interface for the CSectorScene class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SECTORSCENE_H__DB1905EF_7F19_458A_8EB7_725758651CB2__INCLUDED_)
#define AFX_SECTORSCENE_H__DB1905EF_7F19_458A_8EB7_725758651CB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SceneNode.h"
#include "MapStorage.h"
#include "MeshObjectContainer.h"
#include "SectorDefine.h"
#include "PerlinNoise.h"
#include "MapStorage.h"
#include "TriTreeNode.h"
#include "HouseObjectScene.h"
#include "TreeScene.h"
#include "WaterScene.h"
#include "FallScene.h"
#include "GrassScene.h"
#include "LayerFogScene.h"
#include "StateLog.h"
//#include "Z3DSound.h"
#include "X3DEffect.h"
#include "X3DEffectManager.h"
#include "ShadowVolume.h"
#include "boidscene2.h"
#include "SectorLight.h"


class CSectorScene : public CSceneNode  
{	


	LPDIRECT3DBASETEXTURE8 m_MapTexture;	
	
	static DWORD m_dwObjectDiffuseVertexShader;
	static DWORD m_dwObjectSpecularVertexShader;

	static DWORD m_dwObjectDiffusePixelShader;
	static DWORD m_dwObjectSpecularPixelShader;

	static LPDIRECT3DCUBETEXTURE8 m_pNormalizeCubeMap;

public:	
	CMapStorage *m_pMapStorage;

	void RenderGlare(LPDIRECT3DDEVICE8 pd3dDevice);
	std::vector<int> m_ShadowPolyList;
	static CRenderTexture m_pShadowTexture;//그림자
	void RenderObjectShadow(LPDIRECT3DDEVICE8 pd3dDevice);
	void UnloadAllData();
	void GetPickHousePoly(vector3 vecStart,vector3 vecEnd,std::vector<vector3> &vecPolyList);
	static color m_TerrainDetailFixColor;
	void GenerateLandscapeEffect();
	void RenderLandscapeEffect(LPDIRECT3DDEVICE8 pd3dDevice);
	void UpdateLandscapeEffect();
	void FullVertexROAMVertex();
	CShadowVolume m_ShadowVolume;
	void RenderTerrainSelfShadow(LPDIRECT3DDEVICE8 pd3dDevice);
	float GetHighPolyHeight(vector3 vecPos);
	void GenerateTerrainLightmap(LPDIRECT3DDEVICE8 pd3dDevice,CSectorScene *NeighborSector[8]);
	void RenderDetail(LPDIRECT3DDEVICE8 pd3dDevice);
	void GenerateObjectShadowed();
	void GetHeightFieldRangePoly(vector3 vecPos,std::vector<vector3> &vecPolyList,float fRange);
//	void GenerateSound();
	void AllObjectCalcLens();
	void GenerateEffect();
	void GenerateMustDivideVertex();
	void RenderGrass(LPDIRECT3DDEVICE8 pd3dDevice);	
	void DynamicLoad();
	void GetLight(vector3 vecPos,std::vector<D3DLIGHT8> &LightList);
	void GetLineIntersectPoly(vector3 vecStart,vector3 vecEnd, std::vector<vector3> &vecPolyList);
	void GetHeightFieldShadowPoly(vector3 vecPos,std::vector<vector3> &vecPolyList);
	void GetInHouseShadowPoly(vector3 vecPos,std::vector<vector3> &vecPolyList,vector3 vecChrLightPos);
	int m_ShadowUsed;
	// Shadow Texture//
	static CRenderTexture m_SwapShadowTexture;
	static CRenderTexture m_ObjectShadowTexture;
	static CRenderTexture m_SectorShadowTexture[9];
	static CRenderTexture m_TerrainShadeTexture;
	static char m_SectorShadowCollision[9][256*256];
	//static CTexture m_CompleteShadowAndTerrainTexture[9];
	static int m_UsedShadowTerrain[9];
	//Cloud-Shadowed Texture//
	static CTexture m_CloudShadow;
	static float m_fCloudMove;
	static CTexture m_LODGrassTexture;
	CTexture *m_pLODGrassAlpha;
	static CTexture *m_GrassRangeTexture;

	//follow data for ROAM //	
	int m_RenderUsedVertex[SECTORSX*SECTORSY];
	float m_fAvgHeight;
	float m_HeightData[SECTORSX*SECTORSY];
	LPDIRECT3DVERTEXBUFFER8 m_pROAMVertex;
	LPDIRECT3DINDEXBUFFER8 m_pROAMIndices;
	long m_UsedIndices,m_UsedVertex;
	SectorVertex m_SectorVertex[SECTORSX*SECTORSY];	
	int m_VarianceLeft[1<<(VARIANCE_DEPTH)];
	int m_VarianceRight[1<<(VARIANCE_DEPTH)];
	int *m_CurrentVariance;
	static TriTreeNode m_TriPool[POOL_SIZE];		
	SectorVertex *m_pDrawVertex;
	WORD *m_pDrawIndices;
	static long m_NextTriNode;
	TriTreeNode m_BaseLeft;
	TriTreeNode m_BaseRight;

	//Following Function is used ROAM//
	TriTreeNode * AllocateTri();
	void ComputeVariance();
	void Tessellate();
	void PrepareRender();
	void Reset();
	void Split(TriTreeNode *tri);
	void RecursRender(TriTreeNode *tri, int leftX, int leftY, int rightX, int rightY, int apexX, int apexY);
	void RecursTessellate(TriTreeNode *tri,int leftx,int leftY,int rightX,int rightY,int apexX,int apexY,int node);
	int RecursComputeVariance(int leftX,int leftY,float leftZ,int rightX,int rightY,float rightZ,int apexX,int apexY,float apexZ,int node);

	// sector, 충돌 있으면 true
	bool CheckRayCollisionHouse(const D3DXVECTOR3 &vecPos1,const D3DXVECTOR3 &vecPos2);

	//Editing data//
	bool m_isEdit;	
	//Scene List //
	std::vector<CHouseObjectScene*> m_HouseObjectNode;
	std::vector<CObjectScene*> m_ObjectSceneNode;	
	CX3DEffectManager m_WorldEffectManager;

	std::vector<void*> m_LandscapeEffectNode;
	std::vector<int> m_LandscapeEffectNodeValue;

	CTreeScene m_TreeObjectNode;
	//CGrassScene m_GrassObjectNode;
	static CWaterScene m_WaterScene;
	CFallScene m_FallScene;
//	std::vector<CZ3DSound*> m_SoundObjectNode;
	//Scene MapStorage Pointer//
	CSectorDungeonMap *m_MapDungeon;
	CSectorHeightMap *m_MapHeight;
	CSectorMeshMap *m_MapMash;
	CSectorPlantMap *m_MapPlant;
	CSectorWideMap *m_MapWide;
	CSectorHouseMap *m_MapHouse;
	CSectorWaterMap *m_MapWater;
	CSectorFallMap *m_MapFall;
	CSectorMeshMap *m_MapMesh;
	CSectorMustDivideVertexMap *m_MapMustDivideVertex;
	CSectorEffectMap *m_MapEffect;
	CSectorLandscapeEffectMap *m_MapLandscapeEffect;
	//boid cash
	static CGemRender *m_Boid[3];
	static bool m_bBoidMeshSet;
	// Sector Light manager - wizardbug (03. 08. 04)	
	CSectorLightManager m_SectorLightManager;

	void SetNationFlag(unsigned char cNation);

//	CSectorSoundMap *m_MapSound;
	
	//Scene Generate Func//
	void Generate(float fStartX,float fStartY);
	void GenerateHouseObject();	
	void GenerateHouseObjectLastIn();
	void DeleteHouse(float fX,float fY,float fZ);
	void SetSkipHouse(float fx,float fy,float fz,bool bSkip);
	void GenerateWideMap();	
	float GenerateHeight();
	void GeneratePlant();
	void GenerateObject();	
	void GenerateFall();	
	void GenerateWater();
	void GenerateShadow(LPDIRECT3DDEVICE8 pd3dDevice);	
	void GenerateSectorLight();

	void CalcNormal();
	float GetHeight(vector3 vecPos);


	//Terrain & Detail Texture, Terrain Texture Determin func//
	static CTexture m_DetailTexture;
	static CTexture m_TerrainTexture[12];
	static CTexture m_NormalPlantTexture;
	static CTexture m_LayerFogTexture;
	void DeterminTerrainTexture(float fHeight,float fPosx,float fPosy);

	//Scene & Terrain Rendering func//
	void Render(LPDIRECT3DDEVICE8 pd3dDevice);
	void RenderEffect(bool bCull = true);
	void RenderSectorLight();

	void UpdateEffect(bool bCull = true);

	void RenderNotCullHouseObject(LPDIRECT3DDEVICE8 pd3dDevice);

	void RenderHouseObject(LPDIRECT3DDEVICE8 pd3dDevice);
	void RenderEnvHouseObject(LPDIRECT3DDEVICE8 pd3dDevice);
	
	void RenderPlant(LPDIRECT3DDEVICE8 pd3dDevice);
	void RenderEnvPlant(LPDIRECT3DDEVICE8	pd3dDevice);

	void RenderObject(LPDIRECT3DDEVICE8 pd3dDevice);
	void RenderFall(LPDIRECT3DDEVICE8 pd3dDevice);
	void RenderWater(LPDIRECT3DDEVICE8 pd3dDevice,bool bFlare);
	//boid
	void SetBoidMesh();
	void DeleteBoidMesh();
	void DeleteCullHouseIndex();

	vector3 m_SectorPosition;	
	void Create(CMapStorage *pMapStorage);
	CSectorScene();
	virtual ~CSectorScene();
	// night
	void SettingNight(bool bNight);
	bool	m_bNight;

	static CEffScript *m_pLightEsf;
	
	int *m_pNotCullHouseIndex;
	int	m_iNotCullHouseIndexCnt;

	int m_iNotCullHouseIndex;

};


#endif // !defined(AFX_SECTORSCENE_H__DB1905EF_7F19_458A_8EB7_725758651CB2__INCLUDED_)
