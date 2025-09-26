// SceneManager.h: interface for the CSceneManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENEMANAGER_H__CCA3E492_5F66_478A_9BD4_891A99FEB846__INCLUDED_)
#define AFX_SCENEMANAGER_H__CCA3E492_5F66_478A_9BD4_891A99FEB846__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( disable : 4786 )

// 새로운 octree 충돌처리루틴을 위해 추가. 2006.02.15. yundi
#include "OctreeCollider.h"

#include <ViewCamera.h>
#include "HeightFieldScene.h"
#include "MeshObjectContainer.h"
//#include "HouseCacheMgr.h"
#include "WeatherManager.h"
#include "LightContainer.h"
#include "ObjectScene.h"
#include "ObjectContainer.h"
#include "CollisionDetection.h"
#include "RenderTexture.h"
#include "Z3DGeneralChrModel.h"
#include "CharacterLightShadowManager.h"
#include "InitValue.h"
#include "Particle.h"
#include "ParticleManager.h"
#include "StateLog.h"
#include "InstanceObjectManager.h"
#include "BspScene.h"
#include "snowfall.h"
#include "NTexture.h"

#include "GrassScene.h"




#include "BoidScene.h"
#include "X3DEffectManager.h"
#include "SectorEffectMap.h"	// Added by ClassView
#include "RenderTextureMipmap.h"

#include "RenderOption.h"


// RBSP
#include "RBspSceneManager.h"
#include "CCameraScript.h"

// 추가
#include <src/base/BaseCacheMgr.h>
#include <src/base/ResourceLoader.h>
#include <src/scene/d3dbufferpools.h>

//Event
#include "SceneEventMgr.h"


//Refactory
#include "../zalla3d base class/Resource.h"
#include "../zalla3d base class/Pool.h"
#include "../zalla3d base class/ResourceObj.h"
#include "../zalla3d base class/ResourceLoader.h"

class CChristmasParticle;
class CChristmasParticleManager;
class CFullSceneEffect;
class CEffectCacheMgr;


class CSceneManager  
{
public:
	static void AddHouseMap(float fAddx, float fAddy, float fAddz,char *strOutName,char *strMedName,char *strInName,char *strBspName);
	static void DeleteHouseMap(float fDelx,float fDely,float fDelz,char *filename);

	void SetNullTextureMode(bool bNullTextureMode);

	void SetWireFrameMode(bool bWireFrame);

	////// Refactory
	static void _ReleaseObj(int iType,void *pObj) 
	{
		if(pObj && m_pPool)
		{
			m_pPool->ReleaseObj(iType,pObj);
		}
	}
	static void *_CreateObj(int iType)
	{
		if(m_pPool)
			return m_pPool->GetObj(iType);
		return NULL;

	}
	static void _ReleaseResource(int iType,char *pObj)
	{
		if(pObj && m_pResource)
		{
			m_pResource->ReleaseObj(iType,pObj);
		}
	}
	static void _ReleaseResource(int iType,CROSSM::CResourceObj *pObj)
	{
		if(pObj && m_pResource)
		{
			m_pResource->ReleaseObj(iType,pObj->GetName());
		}
	}
	static void *_GetResource(int iType,char *strName)
	{
		if(iType == CROSSM::RESOURCE_TEXTURE)
		{
			char strFullName[256];
			memset(strFullName, 0, 256);
			if(strcmp(CROSSM::CNTexture::GetPath(), ""))
			{
				strcpy(strFullName,CROSSM::CNTexture::GetPath());
				strcat(strFullName,"\\");
			}
			strcat(strFullName,strName);
			int lens=(int)strlen(strFullName);
			strFullName[lens-1]='s';
			strFullName[lens-2]='d';
			strFullName[lens-3]='d';	
			return m_pResource->GetData(iType,strFullName);
		}
		else
			return m_pResource->GetData(iType,strName);
		
		return NULL;
	}
	static void _LoadResourceObj(CROSSM::CResourceObj *pObj, bool bImmediate = false)
	{
		if(m_pRLoader)
		{
			// 2005.01.10 yundi ImmediateLoad 를 위한 코드변경
			if( bImmediate )
			{
				m_pRLoader->ImmediateLoad(pObj);
			}
			else
			{
				m_pRLoader->AddObj(pObj);
			}
		}

	}
	static void _LockImmediateResourceLoad(int iType)
	{
		if(m_pResource)
		{
			m_pResource->LockImmediateLoad(iType);
		}
	}
	static void _UnlockImmediateResourceLoad(int iType)
	{
		if(m_pResource)
		{
			m_pResource->UnlockImmediateLoad(iType);
		}
	}

	/////
	static void SetCharacterDataNode(CCharacterLightShadowManager::CCharacterDataNode SetNode);
	static CCharacterLightShadowManager::CCharacterDataNode GetCharacterDataNode(CZ3DGeneralChrModel *pChrmodel);
	static void CharacterCollisionDetectType(CZ3DGeneralChrModel *pChrmodel, int nType);
	void SetMapSize(vector3 vecMinSize,vector3 vecMaxSize);
	void ResetTerrain();
	static void CharacterGravityAble(CZ3DGeneralChrModel *pChrmodel, bool bGravityAble);
	static void CharacterCollisionAble(CZ3DGeneralChrModel *pChrmodel,bool bCollisionDetectionAble);
	static CZ3DGeneralChrModel* PickingCharacter(int MouseX,int MouseY);
	static void CharacterFirstAccelate(CZ3DGeneralChrModel *pChrmodel,bool bFirstAccelate);
	void SetSceneManagerPath(char *strSceneManagerPath);
	static char m_SceneManagerPath[256];
	void BaseGraphicsDataUnload();
	void BaseGraphicsDataLoad(char *strZoneName, char* szWeatherTableOverride = NULL);
	static color m_FullSceneFade;
	static CollisionType GetCharacterCollisionType(CZ3DGeneralChrModel *pChrmodel);
	static vector3 PickMousePos(int nMx,int nMy);
	
/// RBSP 
	static RBspSceneManager m_RBspSceneManager;			// Bsp Scene Manager
	static bool				m_bRBspSceneIn;				// Bsp 내부에 들어와 있는지 Flag

	static bool				m_bCharSelect;



	class CGlareNode
	{
	public:
		CMeshObject *m_pMeshObject;
		matrix m_matPosition;
		bool m_bCubeEnv;
	};
	static std::vector<CGlareNode> m_GlareList;
	static CTexture m_ChracterEnvTexture;
	static LPDIRECT3DCUBETEXTURE8 m_LightCubeTexture;

	

	static CRenderTexture m_SpGlareTexture;
	static void AddGlareNode(CGlareNode AddNode)
	{
		m_GlareList.push_back(AddNode);
	};
	void RenderFullSceneTextureEnd();
	void RenderFullSceneTextureBegin();
	void RenderGlareTexture();
	static void CharacterRandomPos(CZ3DGeneralChrModel *pChrmodel,int nTimer, vector3 vecRand);
	vector3 GetCharacterMovementVector(CZ3DGeneralChrModel *pChrmodel);
	static void CharacterMovementVector(CZ3DGeneralChrModel *pChrmodel, vector3 vecMove,float fAccelate=0.0f);
	static void DeleteEffect(CX3DEffect *);
	static CSectorEffectMap* m_Effect_Map;

	static CSectorEffectMap *m_PickEffect;
	static BOOL m_bPickEffect;
	static int m_EffectNum;
	static BOOL EffectIsLive(XEFFECT hEffect);
	static void DeleteEffect(XEFFECT hEffect);
	static XEFFECT AddEffect(CX3DEffect *lpNewEffect);

	static CEffScript *AddEffectScript (CEffScript *lpnewScript);
	static void AddInterfaceScript (CEffScript *lpnewScript);

	static void AddWorldEffect(CEffScript *lpnewScript);
	static void DeleteAllEffect();
	static void DeleteAllWorldEffect();
	static CX3DEffectManager m_EffectManager;

//	static void MeshObjectReload(CMeshObject *pMesh);
	static void PickingObjectReload();
	static std::vector<vector3> m_vecBspConvertList;
	static std::vector<CHouseObjectScene*> m_pBspConvertList;
	static std::vector<vector3> m_vecFieldConvertList;
	static CHouseObjectScene *m_pBspScene;
	static CBoidScene m_Boid;
	static void AddChrLight(CZ3DGeneralChrModel *pChrmodel,vector3 vecPos,DWORD dwColor,float fLens,DWORD dwTime);	

	static float GetWeatherTime(){return m_fWeatherTime;};
	static void SetWeatherTime(float fWeatherTime){m_fWeatherTime=fWeatherTime;};
	static float GetWeatherTimeAdder(){return m_fWeatherTimeAdder;};
	static void SetWeatherTimeAdder(float fWeatherTimeAdder){m_fWeatherTimeAdder=fWeatherTimeAdder;};	
	static void AddDelayTimer(CZ3DGeneralChrModel *pChr,int nDelay);
	static int GetCharacterEvent(CZ3DGeneralChrModel *pChrmodel,DWORD Event);
	void AllObjectCalcLens();

	void LoadMap(int type, char *szZoneFilename, char* szBaseGraphicsDataPath, vector3 vZoneBoundMin, vector3 vZoneBoundMax);
	// 9.30 추가
	void HeightPosInit();
	static void CharacterHeightFix();
	static void CharacterSoundPlay(CZ3DGeneralChrModel* pChrmodel,char *strSoundFile);
	vector3 PickPointTerrain();
	static float GetCameraInter(){return m_fChrCameraInter;};
	static void SetCameraInter(float fInter){m_fChrCameraInter=fInter;};
	static vector3 GetCharacterPos(int nCount);
	static POINT GetChacterScreenPos(int nCount);
	static int GetCharacterCount();
	CSectorScene* PickTerrain();
	static CTexture m_NsTexture;
	static bool m_bEditor;
	static void SetAllInstanceObjectColor(color Color);
	static void SetIntanceObjectColor(int ID,color Color);
	static int MouseOnCheckInstanceObject(int px,int py);
	static void DeleteInstanceObject(int InstanceObjectID);
	static int AddInstanceObject(char *strObjectName,vector3 vecPos, float fAngle = 0.0f);	
	
	// 파티클
	static CParticleSequence* AddParticle(vector3 vecPos,vector3 vecTarget);
	static CParticleManager m_ParticleManager;
	//캐릭터	
	static CCharacterLightShadowManager m_CharacterManager;
	static CZ3DGeneralChrModel* AddCharacter( const char* szGCMDSName, const char* szFaceType, const char* szHairStyle, int iValue = -1, CZ3DGeneralChrModel **ppOld = NULL,bool bHouse = false,int iHouseType = -1,D3DXVECTOR3 vecPos = D3DXVECTOR3(0.0f,0.0f,0.0f) );	
	static CZ3DGeneralChrModel* ReplaceCharacter( CZ3DGeneralChrModel* pChrOldModel, const char* szGCMDSName, vector3& vecPos = vector3(0.0f,0.0f,0.0f) );	
	static CZ3DGeneralChrModel* SwitchingModel( int iNum,CZ3DGeneralChrModel *pNew ) ;
	static void DeleteCharacter(CZ3DGeneralChrModel* pDelChr);
	static CollisionType CharacterCollisionType(CZ3DGeneralChrModel* pFindCharacter){return m_CharacterManager.CharacterCollisionType(pFindCharacter);};
	static float m_fRemainFrame;
	static CViewCamera *m_ViewCamera;

	//------------------------------------------------------
	// by ichabod
	static CCharacterLightShadowManager::CCharacterDataNode* GetCharacterDataNodeEx( CZ3DGeneralChrModel *pModel ) ;
	static CCharacterLightShadowManager::CCharacterDataNode  UndeleteListChr( CZ3DGeneralChrModel* pModel ) ;
	static void		UnallocListChr( CCharacterLightShadowManager::CCharacterDataNode pModel, BOOL bFirst = FALSE ) ;
	static void		DeleteElement( CZ3DGeneralChrModel* pDelChr ) ; 


	// 현재 얼마나 찍히냐?
	static long m_cRenderVertex;
	static long m_cRenderPolygon;
	static long m_cRenderSector;
	static long m_cRenderPrimitive;

	static long m_cRenderSectorPoly;
	static long m_cRenderSectorVertex;

	// 해상도
	static long m_ScreenWidth;
	static long m_ScreenHeight;

	//
	static bool m_isCollisionDetection;
	static int m_ViewerMode;	
	static CCollisionDetection m_CollisionDetection;	
	
	void DeleteAllData();
	CSectorScene* PickTerrain(float fXp,float fYp,float fZp);	
	static void CalculateOctree();
	static void CalculateLightmap(int LightmapMode);		

	//Picking Object Instance//
	static long m_bIsPickObjectMoving;
	static bool m_bPickFix;
	static CHouseObjectScene *m_PickHouseScene;
	static CLightObjectScene *m_PickLightScene;
	static CSectorLandscapeEffectMap *m_PickLandscape;
	static CObjectScene *m_PickObjectScene;
	static matrix m_matUndo;
	static vector3 m_vecPickRayStart,m_vecPickRayDir;
	static void PickMakeRay(int dx,int dy,int ScreenX,int ScreenY);
	
	void PickLandscapeEffect(float fMx,float fMy,float fMz,int PickMode);
	void PickLandscapeEffectMove(float fMx,float fMy,float fMz,float fAngle,float fAddy,int MoveMode=0);
	void DeletePickingLandscapeEffect();

	void PickEffect(float fMx,float fMy,float fMz);
	void UnPickEffect();
	
	void PickSectorEffect(int b);
	void PickEffect2(int b);
	
	void PickEffectMove(float fMx,float fMy,float fMz,float fAngle,float fAddy,int MoveMode=0);
	void PickEffectRot(float x,float y,float z,int MoveMode = 0);
	void DeletePickEffect();

	// RBSP 
	void PickBspMove(float fMx,float fMy,float fMz,float fAngle,float fAddy,int MoveMode=0);
	vector3 GetPickBspCenter();
	void PickRBspObject(float x,float y,float z,int iPickValue);
	void PickRBspObjectMove(float fMx,float fMy,float fMz,float fAngle,float fAddy,int iValue,int MoveMode=0);
	void UnPickRBspScene();


	void PickObject(float fPx,float fPy,float fPz,int PickMode);
	void PickObjectMove(float fMx,float fMy,float fMz,float fAngle,float fAddy,int MoveMode=0);
	void DeletePickingObject();
	void Undo();
	static void SetSkipHouse(float fx,float fy,float fz,bool bSkip);

	static void MakeUndoData();
	// LOD Setting Data//
	static int m_isTessellate;
	//
	// Detail Data//
	static long m_DetailCharShadow,m_DetailMapShadow,m_DetailBuildShadow;
	static long m_DetailTerrain,m_DetailChrMesh,m_DetailViewRange;
	static long m_DetailWaterReflection,m_DetailLensflare,m_DetailTreeDetail;
	//

	///// Minotaurs 추가 
	void DeleteAllObject();
	static void InstanceObjectRotation(int InstanceObjectID,float fDist = 0.0f);

	static CHeightFieldScene m_HeightField;
	
	bool m_bSnowFall;
	static CSnowFall m_SnowFall; // 크리스마스 눈 내리기
	static CSnowFall m_Rain;
	static CMeshObjectContainer m_MeshObjectContainer;
	static CLightContainer m_LightObjectContainer;
	static CObjectContainer m_ObjectContainer;
	static CWeatherManager m_WeatherManager;	
	static CInstanceObjectManager m_InstanceObjectManager;

	static CViewCamera* GetCamera(){return m_ViewCamera;};
	static void SetCamera(CViewCamera* pCamera){m_ViewCamera=pCamera;};

	void UpdateScene(float fUpdateFrame);
	void MouseMove(int dx,int dy);
	
	void Render();
	// return true 이면 충돌
	static bool CheckRayCollision(const D3DXVECTOR3 &vecPos1,const D3DXVECTOR3 &vecPos2);
	
	void Create();	
	void Create(CInitValue initValue,char *strPath);

	CSceneManager();
	virtual ~CSceneManager();

	static LPDIRECT3DDEVICE8 GetDevice(){return m_pd3dDevice;};
	static void SetDevice(LPDIRECT3DDEVICE8 pd3dDevice){m_pd3dDevice=pd3dDevice;};

	static CSectorLight *m_pPickSectorLight;
	static bool m_bRenderSectorLight;
	

	static bool m_SkillWeatherEffect;


	static bool	m_bCharacterLight;
	static D3DLIGHT8 m_CharacterLight;			// 밤용 character light
	static float m_fGlowPlaneRange;				// 밤용 glow Texture size
	static bool m_bCharacterLightOp;			// option 처리

	static bool	m_bSectorLightFactor;
	static DWORD m_dwSectorLightFactor;

	static CCameraScript *m_pCameraScript;

	static CRenderOption::ZoneInfo m_ZoneInfoData;
	// Christmas 폭죽 관련 
	static CChristmasParticleManager *m_pChristmasParticleManager;
	bool	m_bChristmas;

	static CFullSceneEffect *m_pFullSceneEffect;
	static float			m_fLife;

	static Caldron::Scene::CD3DBufferPools *ms_pBufferPools;
	static CEffectCacheMgr *ms_pEffectCacheMgr;
	static Caldron::Base::CResourceLoader *ms_pResourceLoader;
	static Caldron::Base::CBaseCacheMgr<CMeshObject> *ms_pNewMeshContainer;
	
	void RenderInCubemap();
	static D3DXMATRIX GetCubeMatrix(int i,vector3 &vecDir);

	static IDirect3DCubeTexture8* m_pWorldCubemap;
	static ID3DXRenderToEnvMap* m_pRenderEnvMap;

	static CSceneEventMgr* ms_pSceneEventMgr;  //총괄 이벤트 메니저
	
	// 내부 씬일때 외부씬 컬링 관련.
	static void SetInView(bool bView) { m_bInView = bView; }
	static bool GetInView() { return m_bInView;}

	static void SetInViewHouseScene(CHouseObjectScene *pHouseScene) { m_pInViewHouseScene = pHouseScene;}
	static CHouseObjectScene *GetInViewHouseScene() { return  m_pInViewHouseScene; }

	static bool m_bInView;		// true 일때 내부 씬 
	static CHouseObjectScene *m_pInViewHouseScene;

	/////////// REFACTORY
	static CROSSM::CPool *m_pPool;
	static CROSSM::CResource *m_pResource;
	static CROSSM::CResourceLoader *m_pRLoader;
	/// Skip Frame//
	float m_fSkipMaxFrame;
	int m_iSkipCount;
	int m_iSkipMaxNum;

	static void SetNationFlag(unsigned char cNationFlag) { m_cNationFlag = cNationFlag; }
	static unsigned char GetNationFlag(void) { return m_cNationFlag; }

	void GetCollisionRespondingPosition(
		CrossM::Math::VECTOR3& vRespondingPos,
		const CrossM::Math::VECTOR3& vPrevPos, const CrossM::Math::VECTOR3& vNewPos,
		const CrossM::Math::VECTOR3& vCollisionEllipsoidRadius);

private:
	// 충돌처리 루틴 교체. 충돌정보 생성 관련 메소드들. 2006.02.21. yundi
	void CollectCollisionTriangleCount(unsigned int& uiOutTriCount);
	void FillCollisionTriangle(unsigned int uiTriCount);
	void BuildCollisionOctree();

	static CMapStorage m_MapStorage;
	static LPDIRECT3DDEVICE8 m_pd3dDevice;
	static unsigned char m_cNationFlag;

	CrossM::Scene::COctreeCollider m_octreeCollider;

	CSceneNode *m_pSceneNodeRoot;		
	CGrassScene m_GrassScene;
	static float m_fChrCameraInter;
	static float m_fWeatherTime;
	static float m_fWeatherTimeAdder;
	CRenderTextureMipmap m_GlareTexture;
	CRenderTextureMipmap m_FullSceneTexture;
	CRenderTextureMipmap m_SpecularGlareTexture;
	CRenderTexture m_GlareCompositeTexture;	

	LPDIRECT3DSURFACE8 m_pTempRenderSurface;		
	LPDIRECT3DSURFACE8 m_pTempRenderZBuffer;	

	bool m_FullSceneAnti;
	int m_ScreenSize;

	bool m_bWireFrameMode;
};

#endif // !defined(AFX_SCENEMANAGER_H__CCA3E492_5F66_478A_9BD4_891A99FEB846__INCLUDED_)
