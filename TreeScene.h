// TreeScene.h: interface for the CTreeScene class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TREESCENE_H__221EA477_56C9_484F_8C14_D6AE4677B987__INCLUDED_)
#define AFX_TREESCENE_H__221EA477_56C9_484F_8C14_D6AE4677B987__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SectorDefine.h"
#include "SceneNode.h"
#include "Vertex.h"
#include "MeshObject.h"
#include "SectorPlantMap.h"
#include "RenderTexture.h"

struct TreeNode_t {
	int iIndex;
	float fViewDist;
	matrix matRot;
	matrix matTm;
	unsigned char ucKind;
};

class CTreeScene : public CSceneNode  
{	
	//LPDIRECT3DVERTEXBUFFER8 m_pPlantVertex;
	//LPDIRECT3DINDEXBUFFER8 m_pPlantIndices;	

	//int m_BeforeDetail;
	//short m_DetailTree[SECTORSX*SECTORSY];
	//short m_DetailTreeKind[SECTORSX*SECTORSY];
	//int m_cDetailTree;
	
public:
	void Update(float fUpdate);
	void RenderShadow(LPDIRECT3DDEVICE8 pd3dDevice,CRenderTexture &pShadowTexture);
	float m_fRot;
	unsigned char m_usDetailTree[SECTORSX*SECTORSY];
	float *m_pHeight;
	void CalcObjectLens();
	//short m_AlreadyPutPlant[SECTORSX*SECTORSY];
	static CMeshObject *m_NormalTreeMesh[MAX_TREEKIND];
	static CMeshObject *m_NormalTreeMeshLOD[MAX_TREEKIND];
	static CMeshObject *m_NormalTreeLeaves[MAX_TREEKIND];
	static CMeshObject *m_NormalTreeTrunk[MAX_TREEKIND];	
	long m_cPlant;
	
	TreeNode_t *m_ViewPlants;
	int	m_iViewPlantsNum;

	std::vector<int> m_ShadowPolyList;
	//vector3 m_vecPlantPosition[MAX_TREE];
	//short m_PlantKind[SECTORSX*SECTORSY];
	//short m_IndexPlantKind[MAX_TREE];
	
	void GeneratePlant(CSectorPlantMap *PlantMap,float *pHeightData);
	void Render(LPDIRECT3DDEVICE8 pd3dDevice);
	void RenderEnv(LPDIRECT3DDEVICE8 pd3dDevice);
	void GeneratePlant(int SeedX,int SeedY,float *pHeightData,float fAvgHeight);
	void Create();
	CTreeScene();
	virtual ~CTreeScene();
};

int Tree_tCompare(const void *arg1,const void *arg2);

#endif // !defined(AFX_TREESCENE_H__221EA477_56C9_484F_8C14_D6AE4677B987__INCLUDED_)
