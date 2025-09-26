// HeightFieldScene.h: interface for the CHeightFieldScene class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HEIGHTFIELDSCENE_H__7E864095_E229_4C61_BE5A_0F54763B2B66__INCLUDED_)
#define AFX_HEIGHTFIELDSCENE_H__7E864095_E229_4C61_BE5A_0F54763B2B66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "SceneNode.h"
#include "SectorDefine.h"
#include "SectorScene.h"
#include "MapStorage.h"
#include "GrassManager.h"
#include "StateLog.h"
#include <vector>

struct SectorSortNode_t {
	int iSectorIndex;
	float fViewDist;
};

class CHeightFieldScene : public CSceneNode
{	
private:

	int m_NowTessellatePosX,m_NowTessellatePosY;
	// Arena1 관련 House와 World Effect object culling flag 03.03.04
	bool m_bHouseCulling;
	bool m_bObjectCulling;				
	bool m_bEffectCulling;

	SectorSortNode_t *m_pSectorSorts;
	int	m_iSortsNum;

	
public:			
	int *UpdateSector;//=new int[m_LSizeX*m_LSizeY];
	int *NotUpdateSector;//=new int[m_LSizeX*m_LSizeY];
	bool *IsCulling;//=new bool[m_LSizeX*m_LSizeY];

	int m_LSizeX,m_LSizeY;	
	int m_NowPositionX,m_NowPositionY;
	void RenderGlare(LPDIRECT3DDEVICE8 pd3dDevice);
	void RebuildShadowPoly();
	
	class CShadowedPoly
	{
	public:
		
		std::vector<vector3> m_ShadowedPolyList;
		vector3 m_vecShadowPos;
		int m_nShadowID;
		int m_nGatherMethod;
		bool m_bActive;
	};

	void DeleteShadowPolyList(int nID);
	CHeightFieldScene::CShadowedPoly* GetShadowPolyList(int nID);
	std::vector<CShadowedPoly> m_ShadowedVertexList;
	int AddShadowPoly(vector3 vecPos,int nMethod);

	void RenderShadow(LPDIRECT3DDEVICE8 pd3dDevice);
	CGrassManager m_GrassSceneManager;
	void Update(float fUpdate);
	char* GetHeightFieldShadowCollision(vector3 vecPos);
	void GetHeightFieldRangePoly(vector3 vecPos,std::vector<vector3> &vecPolyList,float fRange);
	void AllObjectCalcLens();
	int GetLSizeX(){return m_LSizeX;};
	int GetLSizeY(){return m_LSizeY;};
	void GetLight(vector3 vecPos,std::vector<D3DLIGHT8> &LightList);
	void GetLineIntersectPoly(vector3 vecStart,vector3 vecEnd, std::vector<vector3> &vecPolyList);
	void GetHeightFieldShadowPoly(vector3 vecPos,std::vector<vector3> &vecPolyList);
	void GetInHouseShadowPoly(vector3 vecPos,std::vector<vector3> &vecPolyList,vector3 vecLightPos);

	float GetHeight(vector3 vecPos);
	CSectorScene *m_QuadSector[LSIZEX*LSIZEY];	
	CSectorScene *m_SectorScene;
	std::vector<char*> m_AlreadyLoad;
	bool CheckingTessellate();
	void Tessellate();
	void Render(LPDIRECT3DDEVICE8 pd3dDevice);
	void RenderEnv(LPDIRECT3DDEVICE8	pd3dDevice);

	void RenderMapEffect();
	
	void Create(int LSizeX,int LSizeY,CMapStorage *pMapStorage);
	void Destroy(void);
	void CheckingNextTerrain();
	CHeightFieldScene();
	virtual ~CHeightFieldScene();

	void GenerateSectorSceneObjects( int sectorX, int sectorY );
	// Arena1 관련 House와 World Effect culling flag 03.03.04
	void SetHouseCulling( bool bCull ) { m_bHouseCulling  = bCull; } 
	void SetObjectCulling( bool bCull ) { m_bObjectCulling  = bCull; } 
	void SetEffectCulling( bool bCull ) { m_bEffectCulling  = bCull; } 

	bool GetHouseCullingFlag() { return m_bHouseCulling;}

	void DeleteSectorNotCull();
	void InitNowPos();
	
	// ray tracing (true 시 충돌 일어난 것)
	bool CheckRayCollision(const D3DXVECTOR3 &vecPos1,const D3DXVECTOR3 &vecPos2);
	int GetSectorIndex(const int iSX,const int iSY);

	void SetNationFlag(unsigned char cNation);
};

int Sector_tCompare(const void *arg1,const void *arg2);

#endif // !defined(AFX_HEIGHTFIELDSCENE_H__7E864095_E229_4C61_BE5A_0F54763B2B66__INCLUDED_)
