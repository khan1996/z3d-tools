// HouseObject.h: interface for the CHouseObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HOUSEOBJECT_H__F85E679D_0A78_40BA_91F8_DABE3690E8F1__INCLUDED_)
#define AFX_HOUSEOBJECT_H__F85E679D_0A78_40BA_91F8_DABE3690E8F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseDataDefine.h"
#include "./NMesh.h"
//#include "MeshObjectContainer.h"
#include "./OctreeScene.h"
#include "LightObjectScene.h"
#include "ObjectScene.h"
#include "InHouseObjectMap.h"
#include "BspScene.h"
#include "SectorLight.h"
#include "NTexture.h"
#include <vector>

class COctreeScene;

class CHouseObject
{
public:
	COctreeScene *m_pOctree;
	CROSSM::CNMesh *m_pInHouseObject;
	CROSSM::CNMesh *m_pMedHouseObject;
	CROSSM::CNMesh *m_pOutHouseObject;
	
	CROSSM::CNMesh *m_pFlagHouseObject;				// 깃발 메쉬.

	CROSSM::CNTexture *m_Lightmap;
	CROSSM::CNTexture *m_InLightmap;
	bool m_isHaveLightmap;
	bool m_isHaveInLightmap;
	bool m_bModulate2X;

	std::vector<vector3*> m_pVertex;
	std::vector<long> m_nVertex;
	std::vector<WORD *> m_pIndices;
	std::vector<long> m_nIndices;

	CHouseObject();
	virtual ~CHouseObject();

	void GetPolygon();
	void CalcBox(vector3 &MinBox,vector3 &MaxBox,int iCalcObject = 0);
	void GenerateOctree();
	void Render(LPDIRECT3DDEVICE8 pd3dDevice);
	void GenerateLightmap(matrix matSet,matrix matHouseTM,int LightmapMode);
	void GenerateNewLightmap(int ivalue,matrix matSet,matrix matHouseTM,CSectorLight *pLights,int iLights);
	
	bool LoadOut(char *strOutName);
	bool LoadIn(char *strInName);
	bool LoadMed(char *strName);
	
	bool LoadFlag(char *strFlagName);

	bool LoadOctree(char *strOct);

	bool LoadLightmap(char *strName);
	bool LoadInLightmap(char *strName);
	
	void ReleaseOut();
	void ReleaseIn();
	void ReleaseMed();
	void ReleaseFlag();

	void ReleaseOctree();


	void RenderOctree(IDirect3DDevice8* pDevice);
};
/*
class CHouseObject
{	
public:

	void RenderGlare(LPDIRECT3DDEVICE8 pd3dDevice);
	static DWORD m_dwShadowMapVertexShader;
	static DWORD m_dwShadowMapPixelShader;

	static DWORD m_dwHouseDiffuseVertexShader;
	static DWORD m_dwHouseDiffusePixelShader;

	static DWORD m_dwHouseSpecularVertexShader;
	static DWORD m_dwHouseSpecularPixelShader;

	static LPDIRECT3DCUBETEXTURE8 m_pNormalizeCubeMap;

	matrix RenderShadow(LPDIRECT3DDEVICE8 pd3dDevice);
	COctreeScene *m_pOctree;		
	void GetPolygon();
	void CalcBox(vector3 &MinBox,vector3 &MaxBox,int iCalcObject = 0);
	//List<CLightObjectScene*> m_LightList;	
	//List<CObjectScene*> m_ObjectList;
	void GenerateOctree();
	void Render(LPDIRECT3DDEVICE8 pd3dDevice);
	void GenerateLightmap(matrix matSet,matrix matHouseTM,int LightmapMode);
	void GenerateNewLightmap(int ivalue,matrix matSet,matrix matHouseTM,CSectorLight *pLights,int iLights);
	
	bool SettingNight(bool bNight);

	CROSSM::CNMesh *m_pInHouseObject;
	CROSSM::CNMesh *m_pMedHouseObject;
	CROSSM::CNMesh *m_pOutHouseObject;
	CROSSM::CNMesh *m_pLodHouseObject;	
	CBspScene *m_pBspObject;
	CInHouseObjectMap *m_pInHouseMap;
	
	CTexture m_Lightmap;
	CTexture m_InLightmap;

	CTexture m_Detailmap;
	bool m_isHaveLightmap;
	bool m_isHaveInLightmap;

	CHouseObject();
	virtual ~CHouseObject();

	//List<vector3*> m_pVertex;
	//List<long> m_nVertex;
	//List<WORD *> m_pIndices;
	//List<long> m_nIndices;
	// Night 용 세팅 되었는지
	bool m_bConvertNight;
	bool m_bNightMap;
	bool m_bNightUv;
	bool m_bModulate2X;

	// Night 용 데이터들
	//List<void *> m_pBackupVertex;
	//List<void *> m_pBackupTmp;
	CTexture m_BackupLightmap;
	

};*/

#endif // !defined(AFX_HOUSEOBJECT_H__F85E679D_0A78_40BA_91F8_DABE3690E8F1__INCLUDED_)
