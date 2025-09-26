// SceneManager.cpp: implementation of the CSceneManager class.
//
//////////////////////////////////////////////////////////////////////

#include "SceneManager.h"
#include <d3dx8.h>
#include "ConvertTexture.h"
#include "BaseGraphicsLayer.h"
//#include <SectorSoundMap.h>
//#include <BGMController.h>
//#include "RenderOption.h"
//#include <Utility/Exception.h>
#include "ChristmasParticle.h"
#include "ChristmasParticleManager.h"
#include "FullSceneEffect.h"
#include "SoundMgr.h"
#include "BgmManager.h"
#include "AmbienceManager.h"
#include "ImposterScene.h"
#include "Z3DGlowHandler.h"
#include "GMMemory.h"

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CViewCamera* CSceneManager::m_ViewCamera;

#define KeyPressed( key )    HIBYTE( GetAsyncKeyState( key ) )	
 
// SceneData Container
CHeightFieldScene CSceneManager::m_HeightField;
CMeshObjectContainer CSceneManager::m_MeshObjectContainer;
CLightContainer CSceneManager::m_LightObjectContainer;
CObjectContainer CSceneManager::m_ObjectContainer;
CWeatherManager CSceneManager::m_WeatherManager;
CInstanceObjectManager CSceneManager::m_InstanceObjectManager;

float CSceneManager::m_fWeatherTimeAdder;

CCharacterLightShadowManager CSceneManager::m_CharacterManager;
//CMapStorage CSceneManager::m_MapStorage;
//Pick Member
long CSceneManager::m_bIsPickObjectMoving;
CHouseObjectScene *CSceneManager::m_PickHouseScene;
CLightObjectScene *CSceneManager::m_PickLightScene;
CSectorLandscapeEffectMap *CSceneManager::m_PickLandscape;

CObjectScene *CSceneManager::m_PickObjectScene;
matrix CSceneManager::m_matUndo;
bool CSceneManager::m_bPickFix=false;
//Lod Member
int CSceneManager::m_isTessellate;
//Detail Member
long CSceneManager::m_DetailCharShadow;
long CSceneManager::m_DetailMapShadow;
long CSceneManager::m_DetailBuildShadow;
long CSceneManager::m_DetailTerrain;
long CSceneManager::m_DetailChrMesh;
long CSceneManager::m_DetailViewRange;
long CSceneManager::m_DetailWaterReflection;
long CSceneManager::m_DetailLensflare;
long CSceneManager::m_DetailTreeDetail;
//
LPDIRECT3DDEVICE8 CSceneManager::m_pd3dDevice;
CCollisionDetection CSceneManager::m_CollisionDetection;
bool CSceneManager::m_isCollisionDetection;
//

//Particle Manager//
CParticleManager CSceneManager::m_ParticleManager;


int CSceneManager::m_ViewerMode;


	// 해상도
long CSceneManager::m_ScreenWidth;
long CSceneManager::m_ScreenHeight;

long CSceneManager::m_cRenderVertex;
long CSceneManager::m_cRenderPolygon;
long CSceneManager::m_cRenderSector;

long CSceneManager::m_cRenderSectorPoly;
long CSceneManager::m_cRenderSectorVertex;
long CSceneManager::m_cRenderPrimitive;

vector3 CSceneManager::m_vecPickRayStart;
vector3 CSceneManager::m_vecPickRayDir;

float CSceneManager::m_fRemainFrame;

bool CSceneManager::m_bEditor;

CTexture CSceneManager::m_NsTexture;

float CSceneManager::m_fChrCameraInter;
float CSceneManager::m_fWeatherTime;

CBoidScene CSceneManager::m_Boid;

CHouseObjectScene *CSceneManager::m_pBspScene=NULL;
std::vector<vector3> CSceneManager::m_vecBspConvertList;
std::vector<CHouseObjectScene*> CSceneManager::m_pBspConvertList;
std::vector<vector3> CSceneManager::m_vecFieldConvertList;
CX3DEffectManager CSceneManager::m_EffectManager;


CSectorEffectMap *CSceneManager::m_Effect_Map;

CSectorEffectMap *CSceneManager::m_PickEffect;
BOOL CSceneManager::m_bPickEffect;
int CSceneManager::m_EffectNum;

CTexture CSceneManager::m_ChracterEnvTexture;

LPDIRECT3DCUBETEXTURE8 CSceneManager::m_LightCubeTexture;
std::vector<CSceneManager::CGlareNode> CSceneManager::m_GlareList;

CRenderTexture CSceneManager::m_SpGlareTexture;


color CSceneManager::m_FullSceneFade;

char CSceneManager::m_SceneManagerPath[256];
//크리스마스 눈
CSnowFall CSceneManager::m_SnowFall;
CSnowFall CSceneManager::m_Rain;
CRenderOption::ZoneInfo CSceneManager::m_ZoneInfoData;
// RBSP
RBspSceneManager CSceneManager::m_RBspSceneManager;
bool CSceneManager::m_bRBspSceneIn = false;

// 캐릭터 셀렉트
bool CSceneManager::m_bCharSelect = false;

//// Sector Light
CSectorLight *CSceneManager::m_pPickSectorLight = NULL;
bool CSceneManager::m_bRenderSectorLight = false;

bool CSceneManager::m_SkillWeatherEffect = true;


D3DLIGHT8 CSceneManager::m_CharacterLight;
bool CSceneManager::m_bCharacterLight = true;
float CSceneManager::m_fGlowPlaneRange;
bool CSceneManager::m_bCharacterLightOp = true;

bool CSceneManager::m_bSectorLightFactor = false;
DWORD CSceneManager::m_dwSectorLightFactor = 0xffffffff;
CCameraScript *CSceneManager::m_pCameraScript = NULL;
CChristmasParticleManager *CSceneManager::m_pChristmasParticleManager = NULL;
CFullSceneEffect *CSceneManager::m_pFullSceneEffect =NULL;
float CSceneManager::m_fLife = 1.0f;
// 추가
Caldron::Scene::CD3DBufferPools *CSceneManager::ms_pBufferPools = NULL;
CEffectCacheMgr *CSceneManager::ms_pEffectCacheMgr = NULL;
Caldron::Base::CResourceLoader *CSceneManager::ms_pResourceLoader = NULL;
Caldron::Base::CBaseCacheMgr<CMeshObject> *CSceneManager::ms_pNewMeshContainer = NULL;
CMapStorage CSceneManager::m_MapStorage;
//dynamic cube water
IDirect3DCubeTexture8* CSceneManager::m_pWorldCubemap = NULL;
ID3DXRenderToEnvMap* CSceneManager::m_pRenderEnvMap = NULL;

//Event
CSceneEventMgr* CSceneManager::ms_pSceneEventMgr = NULL; 
// 내부씬 관련.
bool CSceneManager::m_bInView = false;		// true 일때 내부 씬 
CHouseObjectScene *CSceneManager::m_pInViewHouseScene = NULL;

// Refactorying
CROSSM::CPool *CSceneManager::m_pPool = NULL;
CROSSM::CResource *CSceneManager::m_pResource = NULL;
CROSSM::CResourceLoader *CSceneManager::m_pRLoader = NULL;

unsigned char CSceneManager::m_cNationFlag = 0;

CSceneManager::CSceneManager()
{	
	m_bWireFrameMode = false;

	//LogMessage("Constructor SceneManager");
	m_isTessellate=1;
	m_PickHouseScene=NULL;
	m_bIsPickObjectMoving=0;
	m_fWeatherTimeAdder=0.001f;
	m_ViewerMode=0;
	m_fRemainFrame=0;
	m_isCollisionDetection=false;	
	m_bEditor=false;
	m_fChrCameraInter=350.0f;
	m_fWeatherTime=0.0f;
	m_fWeatherTimeAdder=0.0f;
	m_EffectNum = 0;
	m_bPickEffect = NULL;
	m_PickLandscape = NULL;
	m_ScreenSize=1024;
	m_FullSceneFade.c=0x0;	

	strcpy(m_SceneManagerPath,"");
	m_bSnowFall = false;

	memset(&m_CharacterLight,0,sizeof(D3DLIGHT8));
	m_CharacterLight.Type=D3DLIGHT_POINT;					
/*	m_CharacterLight.Attenuation0=0.2f;					
	m_CharacterLight.Attenuation1=0.005f;
	m_CharacterLight.Attenuation2=0.0f;
*/	

	m_CharacterLight.Attenuation0=0.6f;					
	m_CharacterLight.Attenuation1=0.001f;
	m_CharacterLight.Attenuation2=0.0f;

	m_CharacterLight.Diffuse.r = 227.0f / 255.0f;
	m_CharacterLight.Diffuse.g = 186.0f / 255.0f;
	m_CharacterLight.Diffuse.b = 31.0f / 255.0f;

	m_CharacterLight.Position.x = 0.0f;
	m_CharacterLight.Position.y = 0.0f;
	m_CharacterLight.Position.z = 0.0f;
	m_CharacterLight.Range= 1500.0f;
	m_CharacterLight.Falloff = 1.0f;

	m_fGlowPlaneRange = 800.0f;

	m_bCharacterLight = true;
	m_bCharacterLightOp = true;

	m_pCameraScript = NULL;
	m_bChristmas = true;
	m_pChristmasParticleManager = NULL;
	m_pFullSceneEffect = NULL;

	ms_pBufferPools = NULL;
	ms_pEffectCacheMgr = NULL;
	ms_pResourceLoader = NULL;
	ms_pNewMeshContainer = NULL;
	
	m_pWorldCubemap = NULL;
	m_pRenderEnvMap = NULL;

	m_bInView = false;
	m_pInViewHouseScene = NULL;

	// Refactorying
	m_pPool = NULL;
	m_pResource = NULL;
	m_pRLoader = NULL;

	m_fSkipMaxFrame = 10.0f;
	m_iSkipCount = 0;
	m_iSkipMaxNum = 5;
}

CSceneManager::~CSceneManager()
{	
	//CMapStorage::DeleteAllData();	
	if(m_pRLoader)
	{
		m_pRLoader->Clear();
		SafeDelete(m_pRLoader);
		m_pRLoader = NULL;
	}
	if(m_pResource)		// Pool mgr 보다는 무조건 앞에서 release
	{
		m_pResource->ReleaseAllData();
		SafeDelete(m_pResource);
		m_pResource = NULL;
	}

	if(m_pPool)
	{
		m_pPool->ReleaseAllData();
		SafeDelete(m_pPool);
		m_pPool = NULL;
	}

	m_MapStorage.DeleteAllObject();
	m_LightObjectContainer.DeleteAllObject();
	m_ObjectContainer.DeleteAllObject();	
	m_MeshObjectContainer.DeleteAllObject();	
	m_HeightField.DeleteSectorNotCull();
	if(CGrassScene::m_NormalTexture != NULL)
	{	
		delete CGrassScene::m_NormalTexture;
		CGrassScene::m_NormalTexture = NULL;
	}
	if(m_pCameraScript != NULL) {
		delete m_pCameraScript;
		m_pCameraScript = NULL;

	}
	if(m_pChristmasParticleManager != NULL) {
		delete m_pChristmasParticleManager ;
		m_pChristmasParticleManager = NULL;
	}

	if(m_pFullSceneEffect) {
		delete m_pFullSceneEffect;
		m_pFullSceneEffect = NULL;
	}

	//=======================
	// Bgm & Ambience & Evnet
	//=======================
	CBgmManager::_Destroy(); 
	CAmbienceManager::_Destroy(); 
	SAFE_DELETE( ms_pSceneEventMgr ); 
	CImposterScene::Shutdown(); 

	// 추가
	if(ms_pBufferPools)
	{
		delete ms_pBufferPools;
		ms_pBufferPools = NULL;

	}
	if(ms_pEffectCacheMgr)
	{
		delete ms_pEffectCacheMgr;
		ms_pEffectCacheMgr = NULL;

	}
	if(ms_pResourceLoader)
	{
		delete ms_pResourceLoader;
		ms_pResourceLoader = NULL;
	}
	if(ms_pNewMeshContainer)
	{
		delete ms_pNewMeshContainer;
		ms_pNewMeshContainer = NULL;
		
	}
	if(m_pWorldCubemap)
	{
		m_pWorldCubemap->Release();	
	}
	m_pWorldCubemap = NULL;
	if(m_pRenderEnvMap)
	{
		m_pRenderEnvMap->Release();
	}
	m_pRenderEnvMap = NULL;




}

void CSceneManager::Create()
{

	m_bEditor=true;
	m_DetailCharShadow=4;
	m_DetailMapShadow=4;
	m_DetailBuildShadow=4;
	m_DetailTerrain=4;
	m_DetailChrMesh=4;
	m_DetailViewRange=4;
	m_DetailWaterReflection=4;
	m_DetailLensflare=4;
	m_DetailTreeDetail=4;
	CPerlinNoise::Init();

	SHADOWSIZE=256;

	
	// 추가 effect 최적화
	if(ms_pBufferPools)
	{
		delete ms_pBufferPools;
	}
	if(ms_pEffectCacheMgr)
	{
		delete ms_pEffectCacheMgr;
	}
	if(ms_pResourceLoader)
	{
		delete ms_pResourceLoader;
	}

	if(ms_pNewMeshContainer)
	{
		delete ms_pNewMeshContainer;
	}
	
	if(!m_pPool)
	{
		m_pPool = new CROSSM::CPool;
		m_pPool->Init();

	}
	if(!m_pResource)
	{
		m_pResource = new CROSSM::CResource;
		m_pResource->Init();
	
	}
	if(!m_pRLoader)
	{
		m_pRLoader = new CROSSM::CResourceLoader;
		m_pRLoader->StartLoader();
	}
	


	ms_pNewMeshContainer = new Caldron::Base::CBaseCacheMgr<CMeshObject>;

	ms_pResourceLoader = new Caldron::Base::CResourceLoader;
	ms_pResourceLoader->StartLoader();

	ms_pBufferPools = new Caldron::Scene::CD3DBufferPools;
	ms_pBufferPools->InitBuffer();

	ms_pEffectCacheMgr = new CEffectCacheMgr;

	m_HeightField.Create(CRenderOption::m_TerrainRange,CRenderOption::m_TerrainRange,&m_MapStorage);		
	m_HeightField.CheckingNextTerrain();		
	m_CollisionDetection.m_HeightField=&m_HeightField;	

	CParticle::SetMatPosition(&m_ViewCamera->m_matPosition);
	m_InstanceObjectManager.Create(m_pd3dDevice);
	DETAILGROUND=25;

	m_EffectManager.SetDevice(m_pd3dDevice);
	
	ms_pEffectCacheMgr->InitLoad();

	//m_Boid.Create(100,0);

//	CRenderTargetTexture::Create(m_pd3dDevice,SHADOWSIZE,SHADOWSIZE);
//	LENS_REALPOLYTREE=12000.0f;
/*
	// edith LOD 거리 설정
	// 어차피 이값은 m_TerrainRange가 7일때만 사용되므로 초기값 1만으로 고정
	TERRAINLOD = 5000.0f;
	if(CRenderOption::m_TerrainRange == 7)
		TERRAINLOD = 10000.0f;
	else if(CRenderOption::m_TerrainRange == 5)
		TERRAINLOD = 7000.0f;
*/
	//m_CharacterManager.Create();
	//m_CharacterManager.SetHeightField(&m_HeightField);

	CHRTEXTURESIZE=0;

	m_LightCubeTexture=NULL;

	if(CRenderOption::m_FullSceneGlare)
	{
		/*
		m_GlareTexture.Create(m_ScreenSize,m_ScreenSize);
		m_FullSceneTexture.Create(m_ScreenSize,m_ScreenSize);
		m_SpecularGlareTexture.Create(m_ScreenSize,m_ScreenSize);

		m_GlareCompositeTexture.Create(256,256);	

		m_FullSceneAnti=true;

		CTexture::SetPath(NATURETEXTUREPATH);
		m_ChracterEnvTexture.Load("Env.dds");

		char strCubeTextureName[256];
		sprintf(strCubeTextureName,"%s\\%s",NATURETEXTUREPATH,"LightCubemap.dds");

		D3DXCreateCubeTextureFromFileExA(m_pd3dDevice,
				strCubeTextureName, 
				D3DX_DEFAULT,
				0,
				0,
				D3DFMT_UNKNOWN,
				D3DPOOL_MANAGED,
				D3DX_FILTER_LINEAR,
				D3DX_FILTER_LINEAR,
				0,
				NULL,
				NULL,
				&m_LightCubeTexture);

		//sprintf(strCubeTextureName,"%s\\%s",NATURETEXTUREPATH,"LightCubemap.dds");

		m_DiffuseLightCubeTexture.Create(m_pd3dDevice,"sky",NATURETEXTUREPATH);		
		m_SpecularLightTexture.Create(m_pd3dDevice,"sp_light",NATURETEXTUREPATH,false);

		m_SpGlareTexture.Create(256,256);

		m_SmoothGlareTexture.Create(128);
		*/
		
	}

	

	//m_GlareTexture.Create(256,256);

	//CShadowMap::Create(512,512);

	//m_CharacterManager.CreateEditor();
	
	//CParticleManager::AddParticle(SMOKE_PARTICLE,vector3(0.0f,0.0f,0.0f),vector3(1.0f,1.0f,0.0f));


	///m_GrassScene.Create();	
	//	
	//m_pShip.Load("c:\\MP-Project\\Objects\\Object\\flying-trans-1.R3S");
	//CTexture::SetPath(OBJECTTEXTUREPATH);
	//m_pShip.TextureLoad();
	//m_pShip.ConvertNormal();

	//m_AniObject.Load("c:\\test.R3A");

	SetSceneManagerPath("c:\\MP-Project");	
	//strcpy(CRenderOption::m_strBaseGraphicsDataPath,"Zone2");
	strcpy(CRenderOption::m_strBaseGraphicsDataPath,"");
	BaseGraphicsDataLoad(CRenderOption::m_strBaseGraphicsDataPath);
	m_WeatherManager.Create();
	m_CharacterManager.Create();
	m_CharacterManager.SetHeightField(&m_HeightField);
	if(m_pCameraScript == NULL) {
		m_pCameraScript = new CCameraScript;
		m_pCameraScript->LoadScript("stone.cst");
	}
	if(m_bChristmas) {
		m_pChristmasParticleManager = new CChristmasParticleManager;
		
	}

	
	if(m_pFullSceneEffect) {
		delete m_pFullSceneEffect;
		m_pFullSceneEffect = NULL;
	}
	m_pFullSceneEffect = new CFullSceneEffect;
	bool bSceneFilter = (CRenderOption::m_ScreenFilter > 0) ? true : false;
	m_pFullSceneEffect->SetFullSceneEffectEnable(bSceneFilter);
	m_pFullSceneEffect->Init(GetDevice());
	///////////////
		
	// Cubemap Generation
	//////////////
	if(CRenderOption::m_WaterBumpEnvRendering)
	{
		if(m_pRenderEnvMap == NULL)
		{
			D3DXCreateRenderToEnvMap( BaseGraphicsLayer::GetDevice(), 256, 
				BaseGraphicsLayer::m_d3dpp.BackBufferFormat, TRUE, D3DFMT_D16, &m_pRenderEnvMap );
			
			D3DXCreateCubeTexture( BaseGraphicsLayer::GetDevice(), 256, 1,
					D3DUSAGE_RENDERTARGET,BaseGraphicsLayer::m_d3dpp.BackBufferFormat, D3DPOOL_DEFAULT, &m_pWorldCubemap );
		}
	}
	//////////////



	
}	


D3DXMATRIX D3DUtil_RGetCubeMapViewMatrix( DWORD dwFace )
{
    D3DXVECTOR3 vEyePt   = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vLookDir;
    D3DXVECTOR3 vUpDir;

    switch( dwFace )
    {
        case D3DCUBEMAP_FACE_POSITIVE_X:
            vLookDir = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            break;
        case D3DCUBEMAP_FACE_NEGATIVE_X:
            vLookDir = D3DXVECTOR3(-1.0f, 0.0f, 0.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            break;
        case D3DCUBEMAP_FACE_POSITIVE_Y:
            vLookDir = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 0.0f,-1.0f );
            break;
        case D3DCUBEMAP_FACE_NEGATIVE_Y:
            vLookDir = D3DXVECTOR3( 0.0f,-1.0f, 0.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
            break;
        case D3DCUBEMAP_FACE_POSITIVE_Z:
            vLookDir = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            break;
        case D3DCUBEMAP_FACE_NEGATIVE_Z:
            vLookDir = D3DXVECTOR3( 0.0f, 0.0f,-1.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            break;
    }

    // Set the view transform for this cubemap surface
    D3DXMATRIX matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookDir, &vUpDir );
    return matView;
}

D3DXMATRIX DD3DUtil_GetCubeMapViewMatrix( DWORD dwFace )
{
    D3DXVECTOR3 vEyePt   = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vLookDir;
    D3DXVECTOR3 vUpDir;

    switch( dwFace )
    {
        case D3DCUBEMAP_FACE_POSITIVE_X:
            vLookDir = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            break;
        case D3DCUBEMAP_FACE_NEGATIVE_X:
            vLookDir = D3DXVECTOR3(-1.0f, 0.0f, 0.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            break;
        case D3DCUBEMAP_FACE_POSITIVE_Y:
            vLookDir = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 0.0f,-1.0f );
            break;
        case D3DCUBEMAP_FACE_NEGATIVE_Y:
            vLookDir = D3DXVECTOR3( 0.0f,-1.0f, 0.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
            break;
        case D3DCUBEMAP_FACE_POSITIVE_Z:
            vLookDir = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            break;
        case D3DCUBEMAP_FACE_NEGATIVE_Z:
            vLookDir = D3DXVECTOR3( 0.0f, 0.0f,-1.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            break;
    }

    // Set the view transform for this cubemap surface
    D3DXMATRIX matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookDir, &vUpDir );
    return matView;
};



const long T3LVERTEXFVF=D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE3(0);
class T3LVertex
{
public:
	vector3 v;
	float w;
	color Diffuse;
	color Specular;
	float tu,tv,tw;	
};


void CSceneManager::Render()
{	
	if (true == m_bWireFrameMode)
	{
		m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}
	else
	{
		m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}


	if(m_pFullSceneEffect) {
		m_pFullSceneEffect->BeginRender(BaseGraphicsLayer::m_ClearColor.c);

	}

	if(m_pCameraScript != NULL) {

		if(m_pCameraScript->ISPlay()) {
			D3DXMATRIX tmp = m_pCameraScript->GetCurrentViewMat();
			D3DXVECTOR3 vecCameraPos = m_pCameraScript->GetVecPos();
			D3DXVECTOR3 vecCameraUp = m_pCameraScript->GetVecUp();
			D3DXVECTOR3 vecCameraLook = m_pCameraScript->GetVecLook();
			if((fabs(vecCameraPos.x) < 0.0000001f) &&
				(fabs(vecCameraPos.y) < 0.0000001f) &&
				(fabs(vecCameraPos.z) < 0.0000001f)) {

			}
			else {
				
				m_ViewCamera->LookAt(vector3(vecCameraPos.x,vecCameraPos.y,vecCameraPos.z),
									vector3(vecCameraLook.x,vecCameraLook.y,vecCameraLook.z),
									vector3(vecCameraUp.x,vecCameraUp.y,vecCameraUp.z));
				
				m_pd3dDevice->SetTransform(D3DTS_VIEW,m_ViewCamera->GetMatView());
			}
			//m_pd3dDevice->SetTransform(  D3DTS_VIEW ,&tmp);
			
		}
	}
	if(!GetInView())
		m_WeatherManager.Render(m_pd3dDevice);
	else
	{
		// 성 내부로 들어갔을때 깨져나오는 문제 해결
		m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
		m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	}
	D3DVIEWPORT8 m_pTempViewPort;
	m_pd3dDevice->GetViewport(&m_pTempViewPort);

	m_HeightField.Render(m_pd3dDevice);
//return;	// 56/20 선

// 충돌처리 루틴 교체 테스트용. 충돌가능 노드의 충돌삼각형들을 렌더
//m_octreeCollider.RenderCollidableNodeTriangles(m_pd3dDevice);


	m_pd3dDevice->GetViewport(&m_pTempViewPort);

	if(SHADOWSIZE!=0)
	{
		if(strcmp(CRenderOption::m_strBaseGraphicsDataPath, "Zone100"))
		{
			m_CharacterManager.HeightFieldShadowRender(m_pd3dDevice);	// 캐릭터그림자를 RTT 로 생성한뒤, 그림자가 드리워질 배경 폴리건을 구해 그림자를 출력
			m_CharacterManager.HeightFieldCharacterLightRender(m_pd3dDevice);	// 캐릭터 라이팅(캐릭터 주변에 환하게 원형으로 그려지는거) 그림
		}
	}		
	m_InstanceObjectManager.Render(m_pd3dDevice);	// 땅바닥에 떨어진 아이템 등 렌더

	m_HeightField.RenderShadow(m_pd3dDevice);		// 섹터의 오브젝트(돌더미 등..)과 나무의 그림자를 RTT 로 생성한뒤,
													// 섹터단위로 렌더. RTT가 섹터당 하나.
													// 그림자 드리워질 폴리건 구하는 코드 없이 섹터의 그림자용 폴리건 이용
													// 하지만 저 그림자용 폴리건이 중복데이터가 아닌지 의심스러움
//return;	// 48/8 선
	m_pd3dDevice->GetViewport(&m_pTempViewPort);
	//m_Boid.Render(m_pd3dDevice);

	if(!GetInView())
		m_HeightField.m_GrassSceneManager.Render(m_pd3dDevice);

	m_pd3dDevice->GetViewport(&m_pTempViewPort);
	m_WeatherManager.SecondRender(m_pd3dDevice);	// 현재는..렌즈플레어그리는거밖에 없던데?
	m_pd3dDevice->GetViewport(&m_pTempViewPort);

	m_EffectManager.Render();
	m_EffectManager.RenderScript();
	m_HeightField.RenderMapEffect();

	// 크리스마스 눈 
	//if(m_bSnowFall) {
	//	//m_SnowFall.Render();
	//}
	//// Minotaurs 눈 비 등등등 그리기
	m_SnowFall.Render();
	m_Rain.Render();


	if(m_bChristmas) {
		m_pChristmasParticleManager->Render();
	}

	// Minotaurs Glow
	m_CharacterManager.RenderGlow(m_pd3dDevice);
	CZ3DGlowHandler::_ProcessGlow();	

	//m_EffectManager.RenderWorldScript();

	if(m_FullSceneFade.c!=0x0)
	{
		TLVertex pVertex[8];		

		if((m_pFullSceneEffect == NULL) || !m_pFullSceneEffect->GetEnable()) {
			pVertex[0].v.x=0.0f;
			pVertex[1].v.x=0.0f;
			pVertex[2].v.x=BaseGraphicsLayer::m_lScreenSx;
			pVertex[3].v.x=BaseGraphicsLayer::m_lScreenSx;

			pVertex[1].v.y=0.0f;
			pVertex[3].v.y=0.0f;
			pVertex[0].v.y=BaseGraphicsLayer::m_lScreenSy;
			pVertex[2].v.y=BaseGraphicsLayer::m_lScreenSy;		
		}
		else {
			pVertex[0].v.x=0.0f;
			pVertex[1].v.x=0.0f;
			pVertex[2].v.x=(float)m_pFullSceneEffect->GetRenderWidth();
			pVertex[3].v.x=(float)m_pFullSceneEffect->GetRenderWidth();

			pVertex[1].v.y=0.0f;
			pVertex[3].v.y=0.0f;
			pVertex[0].v.y=(float)m_pFullSceneEffect->GetRenderHeight();
			pVertex[2].v.y=(float)m_pFullSceneEffect->GetRenderHeight();

		}
		for(int i=0;i<4;i++)
		{
			pVertex[i].w=0.1f;
			pVertex[i].v.z=0.1f;
			pVertex[i].Diffuse.c=m_FullSceneFade.c;
			pVertex[i].Specular.c=0x0;		
		}			
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);    
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);		
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);								

		m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,D3DBLEND_ONE);
		m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,D3DBLEND_ONE);		

		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,TRUE);	
		m_pd3dDevice->SetTexture(0,NULL);
		m_pd3dDevice->SetTexture(1,NULL);
		m_pd3dDevice->SetVertexShader(TLVERTEXFVF);		

		m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);
		m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(TLVertex));		
	}
	
	
	
	CSceneNode CheckNode;


	TLVertex pVertex[8];
	float fTextureSize=256.0f;

	pVertex[0].v.x=0.0f;
	pVertex[1].v.x=0.0f;
	pVertex[2].v.x=fTextureSize;
	pVertex[3].v.x=fTextureSize;

	pVertex[1].v.y=0.0f;
	pVertex[3].v.y=0.0f;
	pVertex[0].v.y=fTextureSize;
	pVertex[2].v.y=fTextureSize;		

	static float fasdf=0.0f;
		
	pVertex[0].tu=0.0f+fasdf;
	pVertex[1].tu=0.0f+fasdf;

	pVertex[3].tu=1.0f+fasdf;
	pVertex[2].tu=1.0f+fasdf;

	pVertex[1].tv=0.0f+fasdf;
	pVertex[3].tv=0.0f+fasdf;

	pVertex[0].tv=1.0f+fasdf;
	pVertex[2].tv=1.0f+fasdf;		

	static float fD=0.0f;		
	for(int i=0;i<4;i++)
	{
		pVertex[i].w=0.1f;
		pVertex[i].v.z=0.1f;
		pVertex[i].Diffuse.c=0xffffffff;
		pVertex[i].Specular.c=0xffffffff;		
	}	

	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );    
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);		
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);

	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);				

	m_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX,0);
	m_pd3dDevice->SetTextureStageState(0 ,D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	m_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS,FALSE);
	m_pd3dDevice->SetRenderState(D3DRS_LOCALVIEWER,FALSE);


	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);	    	
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2);

	m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);	
	//m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);	

	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);	
	m_pd3dDevice->SetTexture(1,NULL);
	m_pd3dDevice->SetVertexShader(TLVERTEXFVF);
	//m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(TLVertex));		

	pVertex[0].v.x=0.0f;
	pVertex[1].v.x=0.0f;
	pVertex[2].v.x=fTextureSize;
	pVertex[3].v.x=fTextureSize;

	pVertex[1].v.y=0.0f+256.0f;
	pVertex[3].v.y=0.0f+256.0f;// 
	pVertex[0].v.y=fTextureSize+256.0f;
	pVertex[2].v.y=fTextureSize+256.0f;

	//m_pd3dDevice->SetTexture(0,CSceneManager::m_SpGlareTexture.GetTexture());
	//m_pd3dDevice->SetTexture(0,m_SmoothGlareTexture.GetTexture());

	//m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(TLVertex));		
	
	if(m_pFullSceneEffect) {
		m_pFullSceneEffect->EndRender();
		m_pFullSceneEffect->Render();


	}

	
	m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	// 20/5 선
}	// end of Render()

/*
void CSceneManager::MouseMove(int dx, int dy)
{
	switch(m_ViewerMode)
	{
		case 0:
			if(CSceneManager::m_isCollisionDetection)
			{
				CollisionType CT;
				matrix* matOldMat=m_ViewCamera->GetMatPosition();
				vector3 vecOldPos=matOldMat->GetLoc();
				m_ViewCamera->InterfaceFreelook(dx,dy);
				vector3 vecNewPos=matOldMat->GetLoc();

				vector3 vecVelocity=vecNewPos-vecOldPos;
				vecVelocity.y-=10.0f;
				vecNewPos=m_CollisionDetection.GetPosition(vecOldPos,vecVelocity,CT,false);
				matOldMat->_41=vecNewPos.x;
				matOldMat->_42=vecNewPos.y;
				matOldMat->_43=vecNewPos.z;
				matrix matView;
				matView.Inverse(*matOldMat);
				m_ViewCamera->m_matView=matView;
				m_ViewCamera->MoveFrustum();
			}
			m_ViewCamera->InterfaceFreelook(dx,dy);
			break;
		case 1:
			m_ViewCamera->InterfaceCharlook2(-dx,-dy);
			break;
		case 2:
			m_ViewCamera->InterfaceFreeCamera(-dx,-dy);
			//m_ViewCamera->InterfaceFreelook(dx,dy);
			break;
	}
	/*
	if (m_ViewerMode!=0)
	{			
		
	}
	else
	{		
		if(
		if(CSceneManager::m_isCollisionDetection==false)
			m_ViewCamera->InterfaceFreelook(dx,dy);		
		else
		{
			
			vector3 vecOldPos=(*m_ViewCamera->GetPosition());			
			m_ViewCamera->InterfaceFreelook(dx,dy);
						
			
		}
	}
	*/	

	//m_ViewCamera->InterfaceCharlook(dx,dy,100.0f);
	/*
	D3DXMATRIX *matPosition=(D3DXMATRIX*)m_ViewCamera->GetMatPosition();
	float fDeltaY=dx/300.0f;
	float fDeltaX=dy/300.0f;

	D3DXVECTOR3 vecT(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vecR(0.0f, 0.0f, 0.0f);

	D3DXMATRIX matT, matR;
	D3DXQUATERNION qR;		
	
	if(KeyPressed(VK_SHIFT))
	{
		if(KeyPressed('A') || KeyPressed(VK_NUMPAD1) || KeyPressed(VK_LEFT))  vecT.x -= 341.0f; // Slide Left			
		if(KeyPressed('D') || KeyPressed(VK_NUMPAD3) || KeyPressed(VK_RIGHT)) vecT.x += 341.0f; // Slide Right
		if(KeyPressed(VK_DOWN))                                       vecT.y += 341.0f; // Slide Down
		if(KeyPressed(VK_UP))                                         vecT.y -= 341.0f; // Slide Up
		if(KeyPressed('W'))
			vecT.z += 341.0f; // Move Forward
		if(KeyPressed('S'))
			vecT.z -= 341.0f; // Move Backward	
	}
	else
	{
		if(KeyPressed('A') || KeyPressed(VK_NUMPAD1) || KeyPressed(VK_LEFT))  vecT.x -= 5.0f; // Slide Left
		if(KeyPressed('D') || KeyPressed(VK_NUMPAD3) || KeyPressed(VK_RIGHT)) vecT.x += 5.0f; // Slide Right
		if(KeyPressed(VK_DOWN))                                       vecT.y += 5.0f; // Slide Down
		if(KeyPressed(VK_UP))                                         vecT.y -= 5.0f; // Slide Up
		if(KeyPressed('W'))
			vecT.z += 5.0f; // Move Forward
		if(KeyPressed('S'))
			vecT.z -= 5.0f; // Move Backward	
	}
	if(KeyPressed('P'))
	{		
		m_pd3dDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
	}
	if(KeyPressed('O'))
	{
		m_pd3dDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);		
	}
	vector3 vecOldPos;
	vecOldPos.x=matPosition->_41;
	vecOldPos.y=matPosition->_42;
	vecOldPos.z=matPosition->_43;

	D3DXMatrixTranslation(&matT, vecT.x, vecT.y, vecT.z);
	D3DXMatrixMultiply(matPosition, &matT, matPosition);	
	if(m_ViewerMode!=0)		
	{	
		vector3 vecPos;
		vecPos.x=matPosition->_41;
		vecPos.y=matPosition->_42;
		vecPos.z=matPosition->_43;		
		vector3 vecDir=vecPos-vecOldPos;
		vecDir.y-=10.0f;		
		vecPos=m_CollisionDetection.GetPosition(vecOldPos,vecDir);
		matPosition->_41=vecPos.x;
		matPosition->_42=vecPos.y;
		matPosition->_43=vecPos.z;		
	}

	float fRotationX=m_ViewCamera->GetRotationX();
	float fRotationY=m_ViewCamera->GetRotationY();
	
	D3DXQuaternionRotationYawPitchRoll(&qR, 0.0f,-fRotationX ,0.0f);
	D3DXMatrixRotationQuaternion(&matR, &qR);
	D3DXMatrixMultiply(matPosition, &matR, matPosition);		

	fRotationX-=fDeltaX;
	fRotationY-=fDeltaY;
	
	D3DXQuaternionRotationYawPitchRoll(&qR,-fDeltaY,fRotationX, 0.0f);
	D3DXMatrixRotationQuaternion(&matR, &qR);
	D3DXMatrixMultiply(matPosition, &matR, matPosition);	

	m_ViewCamera->SetRotaitionX(fRotationX);
	m_ViewCamera->SetRotaitionY(fRotationY);
	
	m_ViewCamera->SetVecPosition(vector3(m_ViewCamera->GetMatPosition()->_41,m_ViewCamera->GetMatPosition()->_42,m_ViewCamera->GetMatPosition()->_43));	
	
	matrix *matPos=m_ViewCamera->GetMatPosition();
	matrix matInv;
	matInv.Inverse(*matPos);	
	m_ViewCamera->SetMatView(matInv);
	m_ViewCamera->MoveFrustum();
	*/
//}

void CSceneManager::UpdateScene(float fUpdateFrame)
{		
	//m_WeatherManager.m_CloudScene.Update(m_pd3dDevice,0.0f);
	matrix *matViewPosition=GetCamera()->GetMatPosition();	
	vector3 vecViewPos=matViewPosition->GetLoc();
	
	vector3 vecCamera = m_ViewCamera->GetViewTowardVector();
	vector3 UpCamera = m_ViewCamera->GetViewUpVector();
	vecCamera.Normalize();
	UpCamera.Normalize();

	/*--------------------------------------
	// by ichabod
	
	//CSectorSoundMap & sndMap = CSectorSoundMap::GetInstance();	
	CSectorSoundMap::GetInstance().UpdateListener( vecCamera.x, vecCamera.y, vecCamera.z
							, UpCamera.x, UpCamera.y, UpCamera.z
							, vecViewPos.x, vecViewPos.y, vecViewPos.z );
	CSoundManager::GetInstance().Update();
	*/

	D3DVECTOR vListenerPos, vListenerUp, vListenerFront;
	vListenerFront.x = vecCamera.x; 
	vListenerFront.y = vecCamera.y; 
	vListenerFront.z = vecCamera.z;
	vListenerPos.x = vecViewPos.x; 
	vListenerPos.y = vecViewPos.y; 
	vListenerPos.z = vecViewPos.z;
	vListenerUp.x = UpCamera.x; 
	vListenerUp.y = UpCamera.y; 
	vListenerUp.z = UpCamera.z; 
	CSoundMgr::_GetInstance()->GetListener()->SetPosition(vListenerPos); 
	CSoundMgr::_GetInstance()->GetListener()->SetOrientation( vListenerFront, vListenerUp ); 
	m_CharacterManager.m_CharacterList[0].m_pChrmodel->GetPosition( vListenerPos.x, vListenerPos.y, vListenerPos.z );


	// edith 2008.02.14 캐릭터 선택화면이 아닐때만 BGMManager을 업데이트 한다.
	CAmbienceManager::_GetInstance()->Update( D3DXVECTOR3(vListenerPos.x, vListenerPos.y, vListenerPos.z), fUpdateFrame, m_bCharSelect ); 
	CBgmManager::_GetInstance()->Update( D3DXVECTOR3(vListenerPos.x, vListenerPos.y, vListenerPos.z), fUpdateFrame, m_bCharSelect); 


	ms_pSceneEventMgr->Update(); 

	if(m_isTessellate==1)
	{
		m_HeightField.CheckingNextTerrain();
		bool bTerrainTessellate=false;
		bTerrainTessellate=m_HeightField.CheckingTessellate();
		if(bTerrainTessellate)
		{
			AllObjectCalcLens();
			m_HeightField.RebuildShadowPoly();
			//m_CharacterManager.HeightFiledOnChrFixPos();
		}
	}	
	/*
	static float WeatherTime=0.0f;
	WeatherTime+=0.001f;
	if(WeatherTime>=24.0f)
	{
		WeatherTime=0.0f; 
	}	
	m_ParticleManager.Update(fUpdateFrame+1.0f);
	*/

	CSectorScene::m_fCloudMove+=fUpdateFrame*0.0001f;
	CWaterScene::IncWaterTexture(((int)fUpdateFrame)/2.5f);

	m_WeatherManager.Update((float)((int)fUpdateFrame));

	m_HeightField.Update((float)((int)fUpdateFrame));

	if(m_bChristmas) {
	
	/*	if( KeyPressed('Q') && !bPushed) {
			CChristmasParticle *pChristmasParticle = NULL;
			pChristmasParticle = new CChristmasParticle;
			pChristmasParticle->Init(900,"CParticle.dds");
			pChristmasParticle->SetMaxLife(75.0f);
			pChristmasParticle->SetVisible(1.0f,0.0f);
			pChristmasParticle->SetBaseMass(1.2f);
			pChristmasParticle->SetFraction(0.0f);
			pChristmasParticle->SetGravity( 0.0f, -0.1f, 0.0f );
			pChristmasParticle->SetColor(1.0f,1.0f,1.0f,0.0f,0.0f,0.0f);
			pChristmasParticle->SetSize(4.0f,4.0f,4.0f,4.0f);
			m_pChristmasParticleManager->AddParticle(pChristmasParticle);
			bPushed = true;

		}
		else {
			bPushed = false;
		}*/
		m_pChristmasParticleManager->ProcessParticle();
	}

	//m_Boid.Update(fUpdateFrame);


	for(int cUpdate=1;cUpdate<fUpdateFrame-1;cUpdate++)
	{	
		m_fWeatherTime+=m_fWeatherTimeAdder;
		if(m_fWeatherTime>=24.0f)
			m_fWeatherTime=0.0f;
	}
	m_WeatherManager.SetWeather(m_fWeatherTime,m_pd3dDevice);

	if(ms_pResourceLoader)
	{
		ms_pResourceLoader->Process();
	}
	if(m_pResource)
	{
		m_pResource->Update();
	}

	if(m_pRLoader)
	{
		m_pRLoader->Process();

	}

	if(fUpdateFrame >= m_fSkipMaxFrame)
	{
		m_iSkipCount++;
		if(m_iSkipCount == m_iSkipMaxNum)
		{
			m_iSkipCount = 0;
			if(m_fSkipMaxFrame < 60.0f)
				m_fSkipMaxFrame += 10.0f;
			
		}
		return;
	}

	m_iSkipCount = 0;

	
	//////////////////////////////////////////////////////////////////////////
	//
	// by yundi 2004.12.14
	//
	// flush accumulated(unused) events
	m_CharacterManager.FlushAccumulatedCharacterEvents();

	if(m_ViewerMode==0)
	{
		vector3 vecPos=m_ViewCamera->m_matPosition.GetLoc();
		//CZ3DSound::SetListenerPosition(vecPos);
		//CZ3DSound::SetListenerDirection(CSceneManager::m_ViewCamera->GetRotationY()+3.14159f);
		//CZ3DSound::Process();

		for(int cUpdate=1;cUpdate<fUpdateFrame-1;cUpdate++)
		{	
			m_CharacterManager.ActionProcess();

			//m_fWeatherTime+=m_fWeatherTimeAdder;
			//if(m_fWeatherTime>=24.0f)
			//	m_fWeatherTime=0.0f;
		}		
		if(fUpdateFrame>=1)
			m_CharacterManager.UpdateFrame(-1);		
	}
	if(m_ViewerMode==1)
	{		
		for(int cUpdate=1;cUpdate<fUpdateFrame-1;cUpdate++)
		{	
			m_CharacterManager.ActionProcess();
			/*
			m_fWeatherTime+=m_fWeatherTimeAdder;
			if(m_fWeatherTime>=24.0f)
				m_fWeatherTime=0.0f;
				*/
		}
		if(fUpdateFrame>=1)
			m_CharacterManager.UpdateFrame(-1);
	}	
	if(m_ViewerMode==2)
	{
		for(int cUpdate=1;cUpdate<fUpdateFrame;cUpdate++)
		{
			m_CharacterManager.ActionProcess();
			/*
			m_fWeatherTime+=m_fWeatherTimeAdder;
			if(m_fWeatherTime>=24.0f)
				m_fWeatherTime=0.0f;
				*/
		}
	}	
	
	/*

	vector3 vecPos=CSceneManager::m_ViewCamera->m_matPosition.GetLoc();	
	CSectorScene *pSector=PickTerrain(vecPos.x,vecPos.y,vecPos.z);
	if(pSector)
	{
		if(pSector->m_MapWater)
		{
			if(pSector->m_MapWater->m_fWaterHeight>vecPos.y)
			{
				m_WeatherManager.m_InterFogFar=1000.0f;					
				m_WeatherManager.m_InterSkyFogFar=1000.0f;
				m_WeatherManager.m_InterFogNear=100.0f;
			}
		}
	}
	*/
	//Landscape effect update
	CSectorScene *tmp_sector = PickTerrain();

	for(int cUpdate=0;cUpdate<(int)fUpdateFrame;cUpdate++)
	{
		//Landscape effect
		if(tmp_sector != NULL) 
		{
			//tmp_sector->UpdateLandscapeEffect();
			tmp_sector->UpdateEffect();
		}
		//effect
		m_EffectManager.UpdateFrame();
		if(m_pCameraScript != NULL) 
			m_pCameraScript->PlayScript(0,0,0);


		////////////////////////////// Minotaurs 눈 비 등등등 업데이트
		D3DXMATRIX tmpview;
		m_pd3dDevice->GetTransform(D3DTS_VIEW,&tmpview);
		D3DXMatrixInverse(&tmpview,NULL,&tmpview);
		
		vector3 tmppos; 
		tmppos.x = tmpview._41;
		tmppos.y = tmpview._42;
		tmppos.z = tmpview._43;
		
		vector3 tmptoward = m_ViewCamera->GetViewTowardVector();
		vector3 tmpup = m_ViewCamera->GetViewUpVector();
		tmptoward.Normalize();
		tmpup.Normalize();

		///////////// 눈
		if(m_WeatherManager.GetWeatherCorlor() == 67 ) 
			m_SnowFall.Update(tmppos.x + tmptoward.x * 700.0f,tmppos.y,tmppos.z + tmptoward.z * 700.0f,1000.0f,1000.0f,tmppos.y + 700.0f,true);
		else
			m_SnowFall.Update(tmppos.x + tmptoward.x * 700.0f,tmppos.y,tmppos.z + tmptoward.z * 700.0f,1000.0f,1000.0f,tmppos.y + 700.0f,false);
		//m_SnowFall.Update(tmppos.x,tmppos.y,tmppos.z,800.0f,800.0f,tmppos.y + 100.0f);

		////////////////////비

		if(m_WeatherManager.GetWeatherCorlor() == 101 ) 
			m_Rain.Update(tmppos.x + tmptoward.x * 700.0f,tmppos.y,tmppos.z + tmptoward.z * 700.0f,1000.0f,1000.0f,tmppos.y + 700.0f,true);
		else
			m_Rain.Update(tmppos.x + tmptoward.x * 700.0f,tmppos.y,tmppos.z + tmptoward.z * 700.0f,1000.0f,1000.0f,tmppos.y + 700.0f,false);


		////////////////// Minotaurs  Weather에 따라 눈 비;;등등등;;
		//////////////크리스마스 눈//
/*
		if(m_bSnowFall) 
		{
			D3DXMATRIX tmpview;
			m_pd3dDevice->GetTransform(D3DTS_VIEW,&tmpview);
			D3DXMatrixInverse(&tmpview,NULL,&tmpview);
			
			vector3 tmppos;
			tmppos.x = tmpview._41;
			tmppos.y = tmpview._42;
			tmppos.z = tmpview._43;
			
			vector3 tmptoward = m_ViewCamera->GetViewTowardVector();
			vector3 tmpup = m_ViewCamera->GetViewUpVector();
			tmptoward.Normalize();
			tmpup.Normalize();

			m_SnowFall.Update(tmppos.x + tmptoward.x * 700.0f,tmppos.y,tmppos.z + tmptoward.z * 700.0f,1000.0f,1000.0f,tmppos.y + 700.0f);
			//m_SnowFall.Update(tmppos.x,tmppos.y,tmppos.z,800.0f,800.0f,tmppos.y + 100.0f);
		}
		///////////////////////////////////////////
*/		
	//	int snum;/* = m_EffectManager.GetScriptNum();
	/*	for(int scnt = 0;scnt<snum;scnt++)
			m_EffectManager.ProcessScript(scnt);*/
		//snum = m_EffectManager.GetWScriptNum();
	/*	for(int scnt = 0;scnt<snum;scnt++)
			m_EffectManager.ProcessWScript(scnt);
	*/
		
	}

	if(m_pFullSceneEffect) 
	{
	
		char SoundBuf[256] = {0};
		static bool bStartSound = false;
		if(m_fLife <= 0.3f) 
		{
	
			if((m_fLife >= 0.0f) && bStartSound == false)
			{
				bStartSound = true;
				sprintf(SoundBuf,"%s%s",EFFECTSOUNDMPATH,"heart.wav");				// 심장소리
				CSound* pSound; 
				CSoundMgr::_GetInstance()->GetSound( pSound, SoundBuf, 1 ); 
				pSound->SetLooping( true ); 
				pSound->Play(); 
			}
			
			m_pFullSceneEffect->SetFlag(FULLSCENE_SATURATION2,true);
			if(m_fLife <= 0.2f) 
			{
				m_pFullSceneEffect->SetFlag(FULLSCENE_MOTIONBLUR,true);
			}
			else 
			{
				m_pFullSceneEffect->SetFlag(FULLSCENE_MOTIONBLUR,false);
			}
			if((bStartSound == true) && (m_fLife <= 0.0f)) 
			{
				sprintf(SoundBuf,"%s%s",EFFECTSOUNDMPATH,"heart.wav");				// 심장소리
				CSound* pSound; 
				CSoundMgr::_GetInstance()->GetSound( pSound, SoundBuf, 1 ); 
				pSound->Stop(); 
				bStartSound = false;
			}
		}
		else if((m_pFullSceneEffect->GetFlag(FULLSCENE_SATURATION2)))
		{
			m_pFullSceneEffect->SetFlag(FULLSCENE_SATURATION2,false);
			m_pFullSceneEffect->SetFlag(FULLSCENE_MOTIONBLUR,false);
			// sound off
			if(bStartSound) 
			{
				sprintf(SoundBuf,"%s%s",EFFECTSOUNDMPATH,"heart.wav");				// 심장소리
				CSound* pSound; 
				CSoundMgr::_GetInstance()->GetSound( pSound, SoundBuf, 1 ); 
				pSound->Stop(); 
				bStartSound = false; 
			}
		}
	}


	// 2005.05.10 yundi Octree에 대한 Lock Load 추가(존 로딩시)
	_UnlockImmediateResourceLoad(CROSSM::RESOURCE_OCTREESCENE);
	m_CollisionDetection.UnlockDetection();
}

void CSceneManager::PickObject(float fPx, float fPy, float fPz,int PickMode)
{
	
	if(PickMode==-1)
		return;
	
	if(m_bPickFix)
	{
		return;
	}

	float fNearestInterval=LENS_NEAR_OBJECT;
	int ix,iy;
	int cObject;
	// picking pos
	vector3 vecPick(fPx,fPy,fPz);
	vector3 vecLens;
	vector3 vecObjectPos;

	
/*	if(PickMode == 7) {
					fNearestInterval=LENS_NEAR_OBJECT;
					for(int i=0;i<m_EffectNum;i++) {

						CX3DEffect *PickEffect = m_EffectManager.m_lstEffect[i];

						vector3 *vecTargetObjectPos = PickEffect->GetCenter();
						vecLens= (*vecTargetObjectPos) - vecPick;
						
						if(	vecLens.GetLens() <= fNearestInterval)
						{
							m_PickEffect = m_EffectManager.m_lstEffect[i];
							m_PickHouseScene=NULL;
							fNearestInterval=vecLens.GetLens();
							m_bPickEffect = TRUE;
						}
					}
					if(fNearestInterval!=LENS_NEAR_OBJECT)
						return;
	}//PickMode == 7*/
	if(m_PickHouseScene)
	{
		fNearestInterval=LENS_NEAR_OBJECT;
		matrix matInvTM,matTM;
		matTM.Translation(vector3(fPx,fPy,fPz));
		matInvTM.Inverse(m_PickHouseScene->m_AccumulateTM);
		matTM=matTM*matInvTM;
		vector3 vecPickPos=matTM.GetLoc();			

		matrix matObjectTM;		
		
	

		
		if(fNearestInterval!=LENS_NEAR_OBJECT)
			return;
		if(PickMode==2)
			return;					
	}
	fNearestInterval=LENS_NEAR_HOUSE;
	
	for(ix=0;ix<m_HeightField.GetLSizeX();ix++)
	{
		for(iy=0;iy<m_HeightField.GetLSizeY();iy++)
		{	//picking point가 sector 내부인지 검사			
			if( m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_TM._41 <= fPx &&
				m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_TM._41+SECTORSIZE > fPx &&
				m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_TM._43 <= fPz &&
				m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_TM._43+SECTORSIZE > fPz)			
			{	//내부
				if(PickMode==1)
				{	//house list 탐색
					for(cObject=0;cObject<(int)m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_HouseObjectNode.size();cObject++)
					{
						vector3 vecTargetObjectPos(
						m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_HouseObjectNode[cObject]->m_AccumulateTM._41,
						m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_HouseObjectNode[cObject]->m_AccumulateTM._42,
						m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_HouseObjectNode[cObject]->m_AccumulateTM._43);
						vecLens=vecTargetObjectPos-vecPick;
						if(	vecLens.GetLens() <= fNearestInterval)
						{	// 해당하는 house m_PickHouseScene으로 복사
							m_PickHouseScene=m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_HouseObjectNode[cObject];
							m_PickObjectScene=NULL;
							fNearestInterval=vecLens.GetLens();
						}
					}
					if(fNearestInterval!=LENS_NEAR_HOUSE)
						return;
				}
				if(PickMode==2)
				{
					fNearestInterval=LENS_NEAR_OBJECT;
					for(cObject=0;cObject<(int)m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_ObjectSceneNode.size();cObject++)
					{					
						vector3 vecTargetObjectPos(
						m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_ObjectSceneNode[cObject]->m_TM._41,
						m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_ObjectSceneNode[cObject]->m_TM._42,
						m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_ObjectSceneNode[cObject]->m_TM._43);
						vecLens=vecTargetObjectPos-vecPick;
						if(	vecLens.GetLens() <= fNearestInterval)
						{
							m_PickObjectScene=m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_ObjectSceneNode[cObject];							
							m_PickHouseScene=NULL;
							fNearestInterval=vecLens.GetLens();
						}
					}
					// object가 picking되었다면 sound object 인지 체크
					/*----------------------------------------------
					// by ichabod
					if( m_PickObjectScene )
					{
						if( strcmp( m_PickObjectScene->m_strObjectName, "Sound.R3S" ) == 0 )
						{
							CSectorSoundMap & sndMap = CSectorSoundMap::GetInstance();
							vector3 loc = m_PickObjectScene->m_TM.GetLoc();
							sndMap.PickAmbience( loc.x, loc.y, loc.z );
						}
			
						if( strcmp( m_PickObjectScene->m_strObjectName, "Cube.R3S" ) == 0 )
						{
							CBGMController & BGMC = CBGMController::GetInstance();
							vector3 loc = m_PickObjectScene->m_TM.GetLoc();
							BGMC.SelectClosestPoint( loc.x, loc.y, loc.z, 10.0f );
						}
					}
					*/


					if(fNearestInterval!=LENS_NEAR_OBJECT)
						return;
				}
				/*if(PickMode == 7) {
					fNearestInterval=LENS_NEAR_OBJECT;
					for(int i=0;i<m_EffectNum;i++) {

						CX3DEffect *PickEffect = m_EffectManager.m_lstEffect[i];

						vector3 *vecTargetObjectPos = PickEffect->GetCenter();
						vecLens= (*vecTargetObjectPos) - vecPick;
						
						if(	vecLens.GetLens() <= fNearestInterval)
						{
							m_PickEffect = m_EffectManager.m_lstEffect[i];
							m_PickHouseScene=NULL;
							fNearestInterval=vecLens.GetLens();
							m_bPickEffect = TRUE;
						}
					}
					if(fNearestInterval!=LENS_NEAR_OBJECT)
						return;
				}//PickMode == 7*/
			}
		}
	}

	//m_PickEffect = NULL;
	m_PickHouseScene=NULL;
	m_PickLightScene=NULL;
	m_PickObjectScene=NULL;
}
void CSceneManager::PickObjectMove(float fMx, float fMy, float fMz,float fAngle,float fAddy,int MoveMode)
{
	if(m_PickLightScene)
	{
		CInHouseObjectMap *InHouseMap=NULL;/*m_MapStorage.FindInHouseMap(m_PickHouseScene->m_strOutName,
																  m_PickHouseScene->m_strMedName,
																  m_PickHouseScene->m_strInName,
																  m_PickHouseScene->m_strBSPName);*/
		if(InHouseMap==NULL)
			return;
		/*
		if(MoveMode==1)
		{
			matrix AddAngleY;
			AddAngleY.YRotation(fAngle);
			m_PickLightScene->m_TM=AddAngleY*m_PickLightScene->m_TM;			
			
			for(int cLight=0;cLight<InHouseMap->m_LightID.num;cLight++)
			{
				if(m_PickLightScene->m_LightID==InHouseMap->m_LightID[cLight])
				{
					InHouseMap->m_matLightList[cLight]=m_PickLightScene->m_TM;
				}
			}			
			return;
		}
		if(MoveMode==2)
		{
			m_PickLightScene->m_TM._42+=fAddy;
			for(int cLight=0;cLight<InHouseMap->m_LightID.num;cLight++)
			{
				if(m_PickLightScene->m_LightID==InHouseMap->m_LightID[cLight])
				{
					InHouseMap->m_matLightList[cLight]=m_PickLightScene->m_TM;
				}
			}
			return;
		}
		*/
		if(MoveMode==3)
		{
			if(KeyPressed(VK_LEFT)) m_PickLightScene->m_TM._41+=1.0f;
			if(KeyPressed(VK_RIGHT)) m_PickLightScene->m_TM._41-=1.0f;
			if(KeyPressed(VK_UP)) m_PickLightScene->m_TM._43+=1.0f;
			if(KeyPressed(VK_DOWN)) m_PickLightScene->m_TM._43-=1.0f;
			if(KeyPressed(VK_INSERT)) m_PickLightScene->m_TM._42+=1.0f;
			if(KeyPressed(VK_DELETE)) m_PickLightScene->m_TM._42-=1.0f;

			for(int cLight=0;cLight<(int)InHouseMap->m_LightID.size();cLight++)
			{
				if(m_PickLightScene->m_LightID==InHouseMap->m_LightID[cLight])
				{
					InHouseMap->m_matLightList[cLight]=m_PickLightScene->m_TM;
				}
			}
			return;
		}
		/*
		matrix matTM,matInvTM;
		matTM.Translation(vector3(fMx,fMy,fMz));
		matInvTM.Inverse(m_PickHouseScene->m_AccumulateTM);		
		
		matTM=matTM*matInvTM;
		m_PickLightScene->m_TM._41=matTM._41;
		m_PickLightScene->m_TM._42=matTM._42;
		m_PickLightScene->m_TM._43=matTM._43;
		m_bIsPickObjectMoving=MOVE_LIGHT;

		for(int cLight=0;cLight<InHouseMap->m_LightID.num;cLight++)
		{
			if(m_PickLightScene->m_LightID==InHouseMap->m_LightID[cLight])
			{
				InHouseMap->m_matLightList[cLight]=m_PickLightScene->m_TM;
			}
		}
		*/
		return;
	}
	if(m_PickObjectScene)
	{
		if(m_PickHouseScene)
		{/*
			CInHouseObjectMap *InHouseMap=m_MapStorage.FindInHouseMap(m_PickHouseScene->m_strOutName,
																	  m_PickHouseScene->m_strMedName,
																	  m_PickHouseScene->m_strInName,
																	  m_PickHouseScene->m_strBSPName);*/
			CInHouseObjectMap *InHouseMap = NULL;
			if(InHouseMap==NULL)
				return;
			
			if(MoveMode==1)
			{
				matrix AddAngleY;
				AddAngleY.YRotation(fAngle); 

				if(KeyPressed(VK_LEFT)) fAngle=0.2f;
				if(KeyPressed(VK_RIGHT)) fAngle=-0.2f;

				m_PickObjectScene->m_TM=AddAngleY*m_PickObjectScene->m_TM;

				for(int cObject=0;cObject<(int)InHouseMap->m_ObjectID.size();cObject++)
				{
					if(m_PickObjectScene->m_ObjectID==InHouseMap->m_ObjectID[cObject])
					{
						InHouseMap->m_matObjectList[cObject]=m_PickObjectScene->m_TM;
					}
				}
				return;
			}
			/*
			if(MoveMode==2)
			{
				m_PickObjectScene->m_TM._42+=fAddy;

				for(int cObject=0;cObject<InHouseMap->m_ObjectID.num;cObject++)
				{
					if(m_PickObjectScene->m_ObjectID==InHouseMap->m_ObjectID[cObject])
					{
						InHouseMap->m_matObjectList[cObject]=m_PickObjectScene->m_TM;
					}
				}

				return;
			}		
			*/
			if(MoveMode==3)
			{
				if(KeyPressed(VK_LEFT)) m_PickObjectScene->m_TM._41+=1.0f;
				if(KeyPressed(VK_RIGHT)) m_PickObjectScene->m_TM._41-=1.0f;
				if(KeyPressed(VK_UP)) m_PickObjectScene->m_TM._43+=1.0f;
				if(KeyPressed(VK_DOWN)) m_PickObjectScene->m_TM._43-=1.0f;
				if(KeyPressed(VK_INSERT)) m_PickObjectScene->m_TM._42+=1.0f;
				if(KeyPressed(VK_DELETE)) m_PickObjectScene->m_TM._42-=1.0f;


				for(int cObject=0;cObject<(int)InHouseMap->m_ObjectID.size();cObject++)
				{
					if(m_PickObjectScene->m_ObjectID==InHouseMap->m_ObjectID[cObject])
					{
						InHouseMap->m_matObjectList[cObject]=m_PickObjectScene->m_TM;
					}
				}
				return;
			}
			/*
			matrix matTM,matInvTM;
			matTM.Translation(vector3(fMx,fMy,fMz));
			matInvTM.Inverse(m_PickHouseScene->m_AccumulateTM);		
			matTM=matTM*matInvTM;
			m_bIsPickObjectMoving=MOVE_OBJECT;			
			m_PickObjectScene->m_TM._41=matTM._41;
			m_PickObjectScene->m_TM._42=matTM._42;
			m_PickObjectScene->m_TM._43=matTM._43;


			for(int cObject=0;cObject<InHouseMap->m_ObjectID.num;cObject++)
			{
				if(m_PickObjectScene->m_ObjectID==InHouseMap->m_ObjectID[cObject])
				{
					InHouseMap->m_matObjectList[cObject]=m_PickObjectScene->m_TM;
				}
			}
			*/
			return;
		}
	/*	else if(m_bPickEffect) 
		{
			if(m_PickEffect == NULL) 
				return;
		
		/*	if(MoveMode == 3) 
			{
						
				CMainFrame *mf=(CMainFrame*)AfxGetApp()->m_pMainWnd;		
				CWorldCreatorView *av=(CWorldCreatorView *)mf->GetActiveView();
				
				PickMakeRay(point.x,point.y,m_BaseGraphicLayer.m_lScreenSx,m_BaseGraphicLayer.m_lScreenSy);		
				vector3 vecPos=m_SceneManager->PickPointTerrain();
			}
			
		}*/
		else 
		{		
			//object moving 처리

			// object mesh informain 읽기
			CSectorMeshMap *pMapMesh=m_MapStorage.FindMeshMap(m_PickObjectScene->m_TM._41,m_PickObjectScene->m_TM._43);
			if(pMapMesh==NULL)
				return;
/*
			int nIndex=-1;
			CSectorSoundMap *pSoundMap=NULL;
			if(strcmp(m_PickObjectScene->m_strObjectName,"Sound.R3S")==0)
			{
				pSoundMap=m_MapStorage.FindSoundMap(m_PickObjectScene->m_TM._41,m_PickObjectScene->m_TM._43);
				float fLens=10.0f;				
				vector3 vecLens;
				for(int i=0;i<pSoundMap->m_SoundPosList.num;i++)
				{
					vecLens=m_PickObjectScene->m_TM.GetLoc()-pSoundMap->m_SoundPosList[i];
					if(fLens>=vecLens.GetLens())
					{
						fLens=vecLens.GetLens();
						nIndex=i;
					}
				}
				if(nIndex==-1)
					return;
			}
*/			
			//y축 회전
			if(MoveMode==1)
			{
				matrix AddAngleY;
				AddAngleY.YRotation(fAngle);
				m_PickObjectScene->m_TM=AddAngleY*m_PickObjectScene->m_TM;				
				//x축 회전	
				if(KeyPressed('7'))
				{
					matrix AddAngleX;
					AddAngleX.XRotation(0.05f);
					m_PickObjectScene->m_TM=AddAngleX*m_PickObjectScene->m_TM;
				}
				if(KeyPressed('8'))
				{
					matrix AddAngleX;
					AddAngleX.XRotation(-0.05f);
					m_PickObjectScene->m_TM=AddAngleX*m_PickObjectScene->m_TM;
				}		
				//z축 회전
				if(KeyPressed('9'))
				{
					matrix AddAngleZ;
					AddAngleZ.ZRotation(0.05f);
					m_PickObjectScene->m_TM=AddAngleZ*m_PickObjectScene->m_TM;
				}
				if(KeyPressed('0'))
				{
					matrix AddAngleZ;
					AddAngleZ.ZRotation(-0.05f);
					m_PickObjectScene->m_TM=AddAngleZ*m_PickObjectScene->m_TM;
				}	
				// 회전 상태복귀
				if(KeyPressed('1'))
				{
					matrix AddAngle;					
					AddAngle.MakeIdent();
					AddAngle._41=m_PickObjectScene->m_TM._41;
					AddAngle._42=m_PickObjectScene->m_TM._42;
					AddAngle._43=m_PickObjectScene->m_TM._43;
					m_PickObjectScene->m_TM=AddAngle;
				}
				
				for(int cObject=0;cObject<(int)pMapMesh->m_ObjectSceneID.size();cObject++)
				{
					if(pMapMesh->m_ObjectSceneID[cObject]==m_PickObjectScene->m_ObjectID)
					{
						pMapMesh->m_TM[cObject]=m_PickObjectScene->m_TM;						
					}
				}

				m_PickObjectScene->m_AccumulateTM=m_PickObjectScene->m_TM;
				/*
				if(pSoundMap && nIndex!=-1)
				{
					pSoundMap->m_SoundPosList[nIndex]=m_PickObjectScene->m_TM.GetLoc();
					vector3 vecPos=m_PickObjectScene->m_TM.GetLoc();
					for(int i=0;i<LSIZEX*LSIZEY;i++)
					{
						if(m_HeightField.m_SectorScene[i].m_MapSound==pSoundMap)
						{
							m_HeightField.m_SectorScene[i].m_SoundObjectNode[nIndex]->SetPosition(vecPos);
							break;
						}
					}					
				}*/
				

				/*----------------------------------------------------
				// by ichabod
				if( strcmp( m_PickObjectScene->m_strObjectName, "Sound.R3S" ) == 0 )
				{
					CSectorSoundMap & sndMap = CSectorSoundMap::GetInstance();
					vector3 loc = m_PickObjectScene->m_TM.GetLoc();
					sndMap.MoveAmbience( loc.x, loc.y, loc.z );
				}
				if( strcmp( m_PickObjectScene->m_strObjectName, "Cube.R3S" ) == 0 )
				{
					CBGMController & BGMC = CBGMController::GetInstance();
					vector3 loc = m_PickObjectScene->m_TM.GetLoc();
					BGMC.MovePoint( loc.x, loc.y, loc.z );
				}
				*/
				return;
			}//수직 이동
			if(MoveMode==2)
			{
				m_PickObjectScene->m_TM._42+=fAddy;				
				for(int cObject=0;cObject<(int)pMapMesh->m_ObjectSceneID.size();cObject++)
				{
					if(pMapMesh->m_ObjectSceneID[cObject]==m_PickObjectScene->m_ObjectID)
					{
						pMapMesh->m_TM[cObject]=m_PickObjectScene->m_TM;
					}
				}
				m_PickObjectScene->m_AccumulateTM=m_PickObjectScene->m_TM;
				/*
				if(pSoundMap && nIndex!=-1)
				{
					pSoundMap->m_SoundPosList[nIndex]=m_PickObjectScene->m_TM.GetLoc();
					vector3 vecPos=m_PickObjectScene->m_TM.GetLoc();
					for(int i=0;i<LSIZEX*LSIZEY;i++)
					{
						if(m_HeightField.m_SectorScene[i].m_MapSound==pSoundMap)
						{
							m_HeightField.m_SectorScene[i].m_SoundObjectNode[nIndex]->SetPosition(vecPos);
							break;
						}
					}					
				}
				*/
				
				/*--------------------------------------------
				// by ichabod
				if( strcmp( m_PickObjectScene->m_strObjectName, "Sound.R3S" ) == 0 )
				{
					CSectorSoundMap & sndMap = CSectorSoundMap::GetInstance();
					vector3 loc = m_PickObjectScene->m_TM.GetLoc();
					sndMap.MoveAmbience( loc.x, loc.y, loc.z );
				}
				if( strcmp( m_PickObjectScene->m_strObjectName, "Cube.R3S" ) == 0 )
				{
					CBGMController & BGMC = CBGMController::GetInstance();
					vector3 loc = m_PickObjectScene->m_TM.GetLoc();
					BGMC.MovePoint( loc.x, loc.y, loc.z );
				}
				*/

				return;
			}// x-z축 이동
			if(MoveMode==3)
			{
				if(KeyPressed(VK_LEFT)) m_PickObjectScene->m_TM._41+=1.0f;
				if(KeyPressed(VK_RIGHT)) m_PickObjectScene->m_TM._41-=1.0f;
				if(KeyPressed(VK_UP)) m_PickObjectScene->m_TM._43+=1.0f;
				if(KeyPressed(VK_DOWN)) m_PickObjectScene->m_TM._43-=1.0f;		

				
				for(int cObject=0;cObject<(int)pMapMesh->m_ObjectSceneID.size();cObject++)
				{
					if(pMapMesh->m_ObjectSceneID[cObject]==m_PickObjectScene->m_ObjectID)
					{
						pMapMesh->m_TM[cObject]=m_PickObjectScene->m_TM;
					}
				}
				m_PickObjectScene->m_AccumulateTM=m_PickObjectScene->m_TM;
				/*
				if(pSoundMap && nIndex!=-1)
				{
					pSoundMap->m_SoundPosList[nIndex]=m_PickObjectScene->m_TM.GetLoc();
					vector3 vecPos=m_PickObjectScene->m_TM.GetLoc();
					for(int i=0;i<LSIZEX*LSIZEY;i++)
					{
						if(m_HeightField.m_SectorScene[i].m_MapSound==pSoundMap)
						{
							m_HeightField.m_SectorScene[i].m_SoundObjectNode[nIndex]->SetPosition(vecPos);
							break;
						}
					}					
				}
				*/

				/*--------------------------------------
				// by ichabod
				if( strcmp( m_PickObjectScene->m_strObjectName, "Sound.R3S" ) == 0 )
				{
					CSectorSoundMap & sndMap = CSectorSoundMap::GetInstance();
					vector3 loc = m_PickObjectScene->m_TM.GetLoc();
					sndMap.MoveAmbience( loc.x, loc.y, loc.z );
				}
				if( strcmp( m_PickObjectScene->m_strObjectName, "Cube.R3S" ) == 0 )
				{
					CBGMController & BGMC = CBGMController::GetInstance();
					vector3 loc = m_PickObjectScene->m_TM.GetLoc();
					BGMC.MovePoint( loc.x, loc.y, loc.z );
				}
				*/
				return; 
			}

			int indexx=(int)(m_PickObjectScene->m_AccumulateTM._41/SECTORSIZE);
			int indexy=(int)(m_PickObjectScene->m_AccumulateTM._43/SECTORSIZE);
		
			int newindexx=(int)(fMx/SECTORSIZE);
			int newindexy=(int)(fMz/SECTORSIZE);

			if(indexx!=newindexx || indexy!=newindexy)
				return;

			matrix matTM,matInvTM;
			matTM.Translation(vector3(fMx,fMy,fMz));
			m_bIsPickObjectMoving=MOVE_OBJECT;

			m_PickObjectScene->m_TM._41=matTM._41;
			m_PickObjectScene->m_TM._42=matTM._42;
			m_PickObjectScene->m_TM._43=matTM._43;

			for(int cObject=0;cObject<(int)pMapMesh->m_ObjectSceneID.size();cObject++)
			{
				if(pMapMesh->m_ObjectSceneID[cObject]==m_PickObjectScene->m_ObjectID)
				{
					pMapMesh->m_TM[cObject]=m_PickObjectScene->m_TM;						
				}
			}
			m_PickObjectScene->m_AccumulateTM=m_PickObjectScene->m_TM;
			/*
			if(pSoundMap && nIndex!=-1)
			{
				pSoundMap->m_SoundPosList[nIndex]=m_PickObjectScene->m_TM.GetLoc();
				vector3 vecPos=m_PickObjectScene->m_TM.GetLoc();
				for(int i=0;i<LSIZEX*LSIZEY;i++)
				{
					if(m_HeightField.m_SectorScene[i].m_MapSound==pSoundMap)
					{
						m_HeightField.m_SectorScene[i].m_SoundObjectNode[nIndex]->SetPosition(vecPos);
						break;
					}
				}					
			}*/

			/*--------------------------------------
			// by ichabod
			if( strcmp( m_PickObjectScene->m_strObjectName, "Sound.R3S" ) == 0 )
			{
				CSectorSoundMap & sndMap = CSectorSoundMap::GetInstance();
				vector3 loc = m_PickObjectScene->m_TM.GetLoc();
				sndMap.MoveAmbience( loc.x, loc.y, loc.z );
			}
			if( strcmp( m_PickObjectScene->m_strObjectName, "Cube.R3S" ) == 0 )
			{
				CBGMController & BGMC = CBGMController::GetInstance();
				vector3 loc = m_PickObjectScene->m_TM.GetLoc();
				BGMC.MovePoint( loc.x, loc.y, loc.z );
			}
			*/
			return;
			///////dfgdfgsdrgserg
		}
	}
	if(m_PickHouseScene)
	{			
		if(MoveMode==1)
		{
			float fSaveX,fSaveY,fSaveZ;
			CSectorHouseMap *SectorHouseMap=m_MapStorage.FindHouseMap(m_PickHouseScene->m_AccumulateTM._41,
																	  m_PickHouseScene->m_AccumulateTM._43);
			if(SectorHouseMap)
			{
				fSaveX=m_PickHouseScene->m_AccumulateTM._41;
				fSaveY=m_PickHouseScene->m_AccumulateTM._42;
				fSaveZ=m_PickHouseScene->m_AccumulateTM._43;
				m_PickHouseScene->m_AccumulateTM._41=0.0f;
				m_PickHouseScene->m_AccumulateTM._42=0.0f;
				m_PickHouseScene->m_AccumulateTM._43=0.0f;
				matrix AddAngleY;
				AddAngleY.YRotation(fAngle);
				m_PickHouseScene->m_AccumulateTM=m_PickHouseScene->m_AccumulateTM*AddAngleY;
				m_PickHouseScene->m_AccumulateTM._41=fSaveX;
				m_PickHouseScene->m_AccumulateTM._42=fSaveY;
				m_PickHouseScene->m_AccumulateTM._43=fSaveZ;
				for(int cObject=0;cObject<(int)SectorHouseMap->m_TM.size();cObject++)
				{					
					if( SectorHouseMap->m_HouseSceneID[cObject] == m_PickHouseScene->m_HouseSceneID )
					{
						SectorHouseMap->m_TM[cObject]=m_PickHouseScene->m_AccumulateTM;						
					}					
				}
			}
			return;
		}
		if(MoveMode==2)
		{
			CSectorHouseMap *SectorHouseMap=m_MapStorage.FindHouseMap(m_PickHouseScene->m_AccumulateTM._41,
																	  m_PickHouseScene->m_AccumulateTM._43);
			if(SectorHouseMap)
			{
				for(int cObject=0;cObject<(int)SectorHouseMap->m_TM.size();cObject++)
				{					
					if( SectorHouseMap->m_HouseSceneID[cObject] == m_PickHouseScene->m_HouseSceneID )
					{
						m_PickHouseScene->m_AccumulateTM._42+=fAddy;
						SectorHouseMap->m_TM[cObject]=m_PickHouseScene->m_AccumulateTM;						
					}
				}						
			}
			return;
		}		
		if(MoveMode==3)
		{	
			CSectorHouseMap *SectorHouseMap=m_MapStorage.FindHouseMap(m_PickHouseScene->m_AccumulateTM._41,
																	  m_PickHouseScene->m_AccumulateTM._43);
			if(SectorHouseMap)
			{
				for(int cObject=0;cObject<(int)SectorHouseMap->m_TM.size();cObject++)
				{					
					if( SectorHouseMap->m_HouseSceneID[cObject] == m_PickHouseScene->m_HouseSceneID )
					{
						if(KeyPressed(VK_LEFT)) m_PickHouseScene->m_AccumulateTM._41+=1.0f;
						if(KeyPressed(VK_RIGHT)) m_PickHouseScene->m_AccumulateTM._41-=1.0f;
						if(KeyPressed(VK_UP)) m_PickHouseScene->m_AccumulateTM._43+=1.0f;
						if(KeyPressed(VK_DOWN)) m_PickHouseScene->m_AccumulateTM._43-=1.0f;					

						SectorHouseMap->m_TM[cObject]=m_PickHouseScene->m_AccumulateTM;						
					}
				}						
			}						
			return;
		}
		int indexx=(int)(m_PickHouseScene->m_AccumulateTM._41/SECTORSIZE);
		int indexy=(int)(m_PickHouseScene->m_AccumulateTM._43/SECTORSIZE);
		
		int newindexx=(int)(fMx/SECTORSIZE);
		int newindexy=(int)(fMz/SECTORSIZE);		
		
		CSectorHouseMap *SectorHouseMap=m_MapStorage.FindHouseMap(m_PickHouseScene->m_AccumulateTM._41,
																  m_PickHouseScene->m_AccumulateTM._43);
		
		if(SectorHouseMap && newindexx==indexx && newindexy==indexy)
		{
			for(int cObject=0;cObject<(int)SectorHouseMap->m_TM.size();cObject++)
			{				
				if( SectorHouseMap->m_HouseSceneID[cObject] == m_PickHouseScene->m_HouseSceneID )
				{
					SectorHouseMap->m_TM[cObject]._41=fMx;
					SectorHouseMap->m_TM[cObject]._42=fMy;
					SectorHouseMap->m_TM[cObject]._43=fMz;
				}
				m_PickHouseScene->m_AccumulateTM._41=fMx;
				m_PickHouseScene->m_AccumulateTM._42=fMy;
				m_PickHouseScene->m_AccumulateTM._43=fMz;
			}
		}
		m_bIsPickObjectMoving=MOVE_HOUSE;				
	}
}

void CSceneManager::CalculateLightmap(int LightmapMode)
{
	if(m_PickHouseScene)
	{
		matrix mat;
		mat=m_PickHouseScene->m_AccumulateTM;
		mat._43=mat._42=mat._41=0.0f;
		
		m_PickHouseScene->m_HouseObject->GenerateLightmap(mat,m_PickHouseScene->m_AccumulateTM,LightmapMode);
		char HouseMeshObjectPath[MAX_NAMEBUFFER];
		
		if(m_PickHouseScene->m_HouseObject->m_pOutHouseObject)		
		{			
			sprintf(HouseMeshObjectPath,"%s%s",HOUSEOBJECTPATH,m_PickHouseScene->m_strOutName);			
			m_PickHouseScene->m_HouseObject->m_pOutHouseObject->Save(HouseMeshObjectPath);
			
		}
		if(m_PickHouseScene->m_HouseObject->m_pMedHouseObject)
		{
			sprintf(HouseMeshObjectPath,"%s%s",HOUSEOBJECTPATH,m_PickHouseScene->m_strMedName);
			m_PickHouseScene->m_HouseObject->m_pMedHouseObject->Save(HouseMeshObjectPath);
		}
		if(m_PickHouseScene->m_HouseObject->m_pInHouseObject)
		{
			sprintf(HouseMeshObjectPath,"%s%s",HOUSEOBJECTPATH,m_PickHouseScene->m_strInName);
			m_PickHouseScene->m_HouseObject->m_pInHouseObject->Save(HouseMeshObjectPath);
		}
		
		/*
		CConvertTexture ConvertTexture;
		ConvertTexture.Init(m_pd3dDevice);
		ConvertTexture.m_ptexOrig=m_PickHouseScene->m_HouseObject->m_Lightmap.GetTexture();
		ConvertTexture.m_dwHeight=m_PickHouseScene->m_HouseObject->m_Lightmap.m_dwHeight;
		ConvertTexture.m_dwWidth=m_PickHouseScene->m_HouseObject->m_Lightmap.m_dwWidth;
		ConvertTexture.Compress(D3DFMT_DXT3);
		ConvertTexture.GenerateMipMaps();
		char strDDSName[MAX_NAMEBUFFER];
		sprintf(strDDSName,"%s\\%s",LIGHTMAPTEXTUREPATH,m_PickHouseScene->m_strOutName);
		strDDSName[strlen(strDDSName)-1]='S';
		strDDSName[strlen(strDDSName)-2]='D';
		strDDSName[strlen(strDDSName)-3]='D';
		ConvertTexture.SaveDDS(ConvertTexture.m_ptexNew,strDDSName);
		*/
	}
}

void CSceneManager::DeleteAllObject()
{
	m_InstanceObjectManager.DeleteAllObject();
}

void CSceneManager::InstanceObjectRotation(int InstanceObjectID,float fDist)
{
	return m_InstanceObjectManager.InstanceObjectRotation(InstanceObjectID,fDist);
}

void CSceneManager::CalculateOctree()
{
	if(m_PickHouseScene)
	{
		m_PickHouseScene->m_HouseObject->GenerateOctree();

		FILE *fp;
		char strOctreeFilename[MAX_NAMEBUFFER];
		sprintf(strOctreeFilename,"%s\\%s",OCTREEPATH,m_PickHouseScene->m_strOutName);		

		strOctreeFilename[strlen(strOctreeFilename)-1]='T';
		strOctreeFilename[strlen(strOctreeFilename)-2]='C';
		strOctreeFilename[strlen(strOctreeFilename)-3]='O';

		fp=fopen(strOctreeFilename,"wb");
		//m_PickHouseScene->m_HouseObject->m_pOctree->m_pOctree->Save(fp);
		m_PickHouseScene->m_HouseObject->m_pOctree->Save(fp);
		fclose(fp);
	}
}

void CSceneManager::DeleteAllData()
{
	
	m_HeightField.Destroy();
	m_GrassScene.UnloadData();
	
	CSceneManager::ms_pResourceLoader->Clear();
	m_MapStorage.DeleteAllObject();
	CSceneManager::ms_pNewMeshContainer->DeleteAllData();
	CSceneManager::m_MeshObjectContainer.DeleteAllObject();
	

	CTexture::m_TextureContainer.DeleteAllDataPath(OBJECTTEXTUREPATH);
	CTexture::m_TextureContainer.DeleteAllDataPath(TERRAINTEXTUREPATH);
	CTexture::m_TextureContainer.DeleteAllDataPath(LIGHTMAPTEXTUREPATH);

	CTexture::m_TextureContainer.DeleteAllDataPath(UNSHADOWWIDETEXTUREPATH);
	CTexture::m_TextureContainer.DeleteAllDataPath(WIDETEXTUREPATH);
	


//char NATURETEXTUREPATH[256]="c:\\MP-project\\Texture\\NatureObject";




}

CSectorScene* CSceneManager::PickTerrain(float fXp, float fYp, float fZp)
{
	int ix,iy;
	for(ix=0;ix<m_HeightField.GetLSizeX();ix++)
	{
		for(iy=0;iy<m_HeightField.GetLSizeX();iy++)
		{
			if( m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_AccumulateTM._41 <= fXp &&
				m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_AccumulateTM._41 + SECTORSIZE > fXp &&
				m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_AccumulateTM._43 <= fZp &&
				m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_AccumulateTM._43 + SECTORSIZE > fZp)
			{
				return &m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()];
			}
			
		}
	}
	return NULL;
}

void CSceneManager::DeletePickingObject()
{
	if(m_PickObjectScene)
	{	
		if(m_PickHouseScene)
		{
			int HouseSceneID=m_PickHouseScene->m_HouseSceneID;

			
			m_MapStorage.DelInHouseObject(m_PickHouseScene->m_strOutName,
										 m_PickHouseScene->m_strMedName,
										 m_PickHouseScene->m_strInName,
										 m_PickObjectScene->m_ObjectID);
			
			int delindexx=m_PickHouseScene->m_AccumulateTM._41/SECTORSIZE;
			int delindexy=m_PickHouseScene->m_AccumulateTM._43/SECTORSIZE;
			
		
			/*
			m_PickHouseScene=NULL;			
			for(int cSector=0;cSector<LSIZEX*LSIZEY;cSector++)
			{
				sectorindexx=m_HeightField.m_SectorScene[cSector].m_AccumulateTM._41/SECTORSIZE;
				sectorindexy=m_HeightField.m_SectorScene[cSector].m_AccumulateTM._43/SECTORSIZE;
				if( delindexx == sectorindexx && delindexy == sectorindexy)
				{
					m_HeightField.m_SectorScene[cSector].GenerateHouseObject();					
					for(int cHouse=0;cHouse<m_HeightField.m_SectorScene[cSector].m_HouseObjectNode.num;cHouse++)
					{
						if(HouseSceneID==m_HeightField.m_SectorScene[cSector].m_HouseObjectNode[cHouse]->m_HouseSceneID)
						{
							m_PickHouseScene=m_HeightField.m_SectorScene[cSector].m_HouseObjectNode[cHouse];
							
						}
					}
				}
			}
			m_PickObjectScene=NULL;
			*/
			m_PickObjectScene=NULL;
		}
		else
		{	
			/*
			CSectorSoundMap *pSoundMap=NULL;
			int nIndex=-1;
			if(strcmp(m_PickObjectScene->m_strObjectName,"Sound.R3S")==0)
			{
				pSoundMap=m_MapStorage.FindSoundMap(m_PickObjectScene->m_TM._41,m_PickObjectScene->m_TM._43);
				float fLens=10.0f;
				vector3 vecLens;
				for(int i=0;i<pSoundMap->m_SoundPosList.num;i++)
				{
					vecLens=m_PickObjectScene->m_TM.GetLoc()-pSoundMap->m_SoundPosList[i];
					if(fLens>=vecLens.GetLens())
					{
						fLens=vecLens.GetLens();
						nIndex=i;
					}
				}
				if(nIndex==-1)
					return;
			}
			if(pSoundMap && nIndex!=-1)
			{
				pSoundMap->m_SoundPosList[nIndex]=m_PickObjectScene->m_TM.GetLoc();
				pSoundMap->m_SoundPosList.DelIndex(nIndex);
				pSoundMap->m_SoundMinRangeList.DelIndex(nIndex);
				pSoundMap->m_SoundMaxRangeList.DelIndex(nIndex);
				delete [] pSoundMap->m_strSoundFilenameList[nIndex];
				pSoundMap->m_strSoundFilenameList.DelIndex(nIndex);

				vector3 vecPos=m_PickObjectScene->m_TM.GetLoc();
				for(int i=0;i<LSIZEX*LSIZEY;i++)
				{
					if(m_HeightField.m_SectorScene[i].m_MapSound==pSoundMap)
					{
						delete m_HeightField.m_SectorScene[i].m_SoundObjectNode[nIndex];
						m_HeightField.m_SectorScene[i].m_SoundObjectNode.DelIndex(nIndex);
						break;
					}
				}					
			}			
			*/

			/*--------------------------------------
			// by ichabod
			if( strcmp( m_PickObjectScene->m_strObjectName, "Sound.R3S" ) == 0 )
			{
				CSectorSoundMap & sndMap = CSectorSoundMap::GetInstance();
				vector3 loc = m_PickObjectScene->m_TM.GetLoc();
				sndMap.DeleteAmbience( loc.x, loc.y, loc.z );
			}

			if( strcmp( m_PickObjectScene->m_strObjectName, "Cube.R3S" ) == 0 )
			{
				try 
				{
					CBGMController & BGMC = CBGMController::GetInstance();
					vector3 loc = m_PickObjectScene->m_TM.GetLoc();
					BGMC.DeletePoint();
				}
				catch( exception & e )
				{
					MessageBox( NULL, e.what(), "Error!!", MB_OK );
					return;
				}
			}
			*/

			m_MapStorage.DelMeshMap(m_PickObjectScene->m_TM._41,m_PickObjectScene->m_TM._42,m_PickObjectScene->m_TM._43,
									m_PickObjectScene->m_ObjectID);

			int delindexx=m_PickObjectScene->m_TM._41/SECTORSIZE;
			int delindexy=m_PickObjectScene->m_TM._43/SECTORSIZE;
			m_HeightField.GenerateSectorSceneObjects( delindexx, delindexy );

			m_PickObjectScene=NULL;

			/*
			CSectorMeshMap *pMapMesh=CMapStorage::FindMeshMap(m_PickObjectScene->m_TM._41,m_PickObjectScene->m_TM._43);
			if(pMapMesh==NULL)
				return;

			if(MoveMode==1)
			{
				matrix AddAngleY;
				AddAngleY.YRotation(fAngle);
				m_PickObjectScene->m_TM=AddAngleY*m_PickObjectScene->m_TM;				
				for(int cObject=0;cObject<pMapMesh->m_ObjectSceneID.num;cObject++)
				{
					if(pMapMesh->m_ObjectSceneID[cObject]==m_PickObjectScene->m_ObjectID)
					{
						pMapMesh->m_TM[cObject]=m_PickObjectScene->m_TM;						
					}
				}
			
				return;
			}
			*/
		}
		return;
	}
	if(m_PickLightScene)
	{
		if(m_PickHouseScene==NULL)
			return;
		int HouseSceneID=m_PickHouseScene->m_HouseSceneID;
		m_MapStorage.DelInHouseLight(m_PickHouseScene->m_strOutName,
									 m_PickHouseScene->m_strMedName,
									 m_PickHouseScene->m_strInName,
									 m_PickLightScene->m_LightID);

		
		m_PickLightScene=NULL;
		/*
		int delindexx=m_PickHouseScene->m_AccumulateTM._41/SECTORSIZE;
		int delindexy=m_PickHouseScene->m_AccumulateTM._43/SECTORSIZE;

		int sectorindexx,sectorindexy;
		for(int cSector=0;cSector<LSIZEX*LSIZEY;cSector++)
		{
			sectorindexx=m_HeightField.m_SectorScene[cSector].m_AccumulateTM._41/SECTORSIZE;
			sectorindexy=m_HeightField.m_SectorScene[cSector].m_AccumulateTM._43/SECTORSIZE;
			if( delindexx == sectorindexx && delindexy == sectorindexy)
			{
				m_HeightField.m_SectorScene[cSector].GenerateHouseObject();
				for(int cHouse=0;cHouse<m_HeightField.m_SectorScene[cSector].m_HouseObjectNode.num;cHouse++)
				{
					if(HouseSceneID==m_HeightField.m_SectorScene[cSector].m_HouseObjectNode[cHouse]->m_HouseSceneID)
					{
						m_PickHouseScene=m_HeightField.m_SectorScene[cSector].m_HouseObjectNode[cHouse];
					}
				}
			}
		}
		*/
		return;
	}
	if(m_PickHouseScene)
	{	
		int delindexx=m_PickHouseScene->m_AccumulateTM._41/SECTORSIZE;
		int delindexy=m_PickHouseScene->m_AccumulateTM._43/SECTORSIZE;

		m_MapStorage.DelHouseMap(m_PickHouseScene->m_AccumulateTM._41,
									 m_PickHouseScene->m_AccumulateTM._42,
									 m_PickHouseScene->m_AccumulateTM._43,
									 m_PickHouseScene->m_HouseSceneID);	
		m_PickHouseScene=NULL;		
		int sectorindexx,sectorindexy;
		for(int cSector=0;cSector<m_HeightField.GetLSizeX()*m_HeightField.GetLSizeY();cSector++)
		{
			sectorindexx=m_HeightField.m_SectorScene[cSector].m_AccumulateTM._41/SECTORSIZE;
			sectorindexy=m_HeightField.m_SectorScene[cSector].m_AccumulateTM._43/SECTORSIZE;
			if( delindexx == sectorindexx && delindexy == sectorindexy)
			{
				m_HeightField.m_SectorScene[cSector].GenerateHouseObject();
			}
		}
		return;
	}
}
	
CZ3DGeneralChrModel* CSceneManager::AddCharacter( const char* szGCMDSName, const char* szFaceType, const char* szHairStyle, int iValue, CZ3DGeneralChrModel **ppOld,bool bHouse,int iHouseType,D3DXVECTOR3 vecPos)
{
	return m_CharacterManager.AddCharacter(szGCMDSName, szFaceType, szHairStyle, iValue, ppOld,bHouse,iHouseType,vecPos ) ;
}

CZ3DGeneralChrModel* CSceneManager::ReplaceCharacter( CZ3DGeneralChrModel* pChrOldModel, const char* szGCMDSName, vector3& vecPos )
{
	return m_CharacterManager.ReplaceCharacter(pChrOldModel, szGCMDSName, vecPos );
}

CZ3DGeneralChrModel *CSceneManager::SwitchingModel( int iNum,CZ3DGeneralChrModel *pNew )
{
	return m_CharacterManager.SwitchingModel( iNum, pNew ) ;
}

CCharacterLightShadowManager::CCharacterDataNode* CSceneManager::GetCharacterDataNodeEx( CZ3DGeneralChrModel *pModel ) 
{
	return m_CharacterManager.GetCharacterDataNodeEx( pModel ) ;
}

CCharacterLightShadowManager::CCharacterDataNode CSceneManager::UndeleteListChr( CZ3DGeneralChrModel* pModel ) 
{
	return m_CharacterManager.UndeleteListChr( pModel ) ;
}

void		CSceneManager::UnallocListChr( CCharacterLightShadowManager::CCharacterDataNode pModel, BOOL bFirst ) 
{
	m_CharacterManager.UnallocListChr( pModel, bFirst ) ;
}

void		CSceneManager::DeleteElement( CZ3DGeneralChrModel* pDelChr ) 
{
	m_CharacterManager.DeleteElement( pDelChr ) ;
}

void CSceneManager::Undo()
{
	matrix matUndoData=m_matUndo;
	MakeUndoData();
	if(m_PickLightScene)
	{
		m_PickLightScene->m_TM=matUndoData;
					
		
		return;
	}
	if(m_PickObjectScene)
	{
		if(m_PickHouseScene)
		{
			
			return;
		}
		else
		{			
			m_PickObjectScene->m_TM=matUndoData;
			CSectorMeshMap *pMapMesh=m_MapStorage.FindMeshMap(m_PickObjectScene->m_TM._41,m_PickObjectScene->m_TM._43);
			if(pMapMesh==NULL)
				return;
			for(int cObject=0;cObject<(int)pMapMesh->m_ObjectSceneID.size();cObject++)
			{
				if(pMapMesh->m_ObjectSceneID[cObject]==m_PickObjectScene->m_ObjectID)
				{
					pMapMesh->m_TM[cObject]=m_PickObjectScene->m_TM;		
				}
			}
			m_PickObjectScene->m_AccumulateTM=m_PickObjectScene->m_TM;
			return;
		}
	}
	if(m_PickHouseScene)
	{			
		CSectorHouseMap *SectorHouseMap=m_MapStorage.FindHouseMap(m_PickHouseScene->m_AccumulateTM._41,
																  m_PickHouseScene->m_AccumulateTM._43);
		m_PickHouseScene->m_AccumulateTM=matUndoData;
		for(int cObject=0;cObject<(int)SectorHouseMap->m_TM.size();cObject++)
		{
			if( SectorHouseMap->m_HouseSceneID[cObject] == m_PickHouseScene->m_HouseSceneID )
			{
				SectorHouseMap->m_TM[cObject]=matUndoData;
			}
		}
	}
}

void CSceneManager::MakeUndoData()
{
	if(m_PickLightScene)
	{
		m_matUndo=m_PickLightScene->m_TM;
	}
	if(m_PickObjectScene)
	{	
		m_matUndo=m_PickObjectScene->m_TM;
	}
	if(m_PickHouseScene && m_PickLightScene==NULL && m_PickObjectScene==NULL)
	{
		m_matUndo=m_PickHouseScene->m_AccumulateTM;
	}
}

void CSceneManager::Create(CInitValue initValue,char *strPath)
{		
	/*
	CRenderOption::m_GrassRendering=1;
	CRenderOption::m_GrassAnimationUseVertexShader=1;
	CRenderOption::m_GrassRenderRange=9;
	CRenderOption::m_RangeGrassRender=0

	CRenderOption::m_FullSceneGlare=1;
	CRenderOption::m_FullSceneAnti=0;
	CRenderOption::m_FullSceneSpecularGlare=1;

	CRenderOption::m_WaterBumpEnvRendering=1;	

	CRenderOption::m_TerrainMultiDetalTexture=1;

	CRenderOption::m_TerrainRange=7;

	CRenderOption::m_BuildingLightmap=1;

	CRenderOption::m_TreeAnimation=1;

	CRenderOption::m_CharacterPerPixelLighting=1;
	CRenderOption::m_CharacterPerPixelSpecularLighting=1;
	CRenderOption::m_CharacterSelfShadow=1;

	CRenderOption::m_DepthOfField=0;
	CRenderOption::m_CharacterProjectShadowTerrain=1;
	CRenderOption::m_CharacterProjectShadowBuilding=1;

	CRenderOption::m_ObjectLOD=4;
	CRenderOption::m_TextureMethod=1;
	CRenderOption::m_strBaseGraphicsDataPath[256];

	CRenderOption::m_ObjectProjectShadow=1;
	CRenderOption::m_TreeProjectShadow=1;
	CRenderOption::m_AllObjectBump=1;
	*/
	CRenderOption::m_AllObjectBump=0;

	CRenderOption::m_ObjectProjectShadow=0;
	CRenderOption::m_TreeProjectShadow=0;
	
	m_bEditor=false;	
	/*
	m_DetailCharShadow=initValue.m_CharShadow;
	m_DetailMapShadow=initValue.m_MapShadow;
	m_DetailBuildShadow=initValue.m_BuildShadow;
	m_DetailTerrain=initValue.m_TerrainDetail;
	m_DetailChrMesh=initValue.m_CharDetail;
	m_DetailViewRange=initValue.m_ViewRange;
	m_DetailWaterReflection=initValue.m_WaterReflection;
	m_DetailLensflare=initValue.m_Lensflare;
	m_DetailTreeDetail=initValue.m_TreeDetail;
	*/
	DETAILGROUND=25;

/*
	// edith LOD 거리 설정
	// 어차피 이값은 m_TerrainRange가 7일때만 사용되므로 초기값 1만으로 고정
	TERRAINLOD = 5000.0f;
	if(CRenderOption::m_TerrainRange == 7)
		TERRAINLOD = 10000.0f;
	else if(CRenderOption::m_TerrainRange == 5)
		TERRAINLOD = 7000.0f;
*/

	// edith Option LOD
	switch(CRenderOption::m_ObjectLOD)
	{
		case 0:
			MAX_HOUSEVIEWRANGE=10000.0f;
//			MAX_HOUSEVIEWRANGE=16000.0f;
			MAX_HOUSEDISAPPER=MAX_HOUSEVIEWRANGE-1000.0f;
			MAX_OUTVIEWRANGE=15000.0f;
			MAX_MEDVIEWRANGE=15000.0f;
			MAX_INVIEWRANGE=4200.0f;
			MAX_OBJECTVIEWRANBE=5000.0f; 
			MAX_OBJECTDISAPPER=MAX_OBJECTVIEWRANBE-400.0f;
			LENS_REALPOLYTREE=5000.0f;
			break;
		case 1:
			MAX_HOUSEVIEWRANGE=30000.0f;
			MAX_HOUSEDISAPPER=MAX_HOUSEVIEWRANGE-2000.0f;
			MAX_OUTVIEWRANGE=15000.0f;
			MAX_MEDVIEWRANGE=15000.0f;
			MAX_INVIEWRANGE=4200.0f;
			MAX_OBJECTVIEWRANBE=5000.0f;
//			MAX_OBJECTVIEWRANBE=7000.0f;
			MAX_OBJECTDISAPPER=MAX_OBJECTVIEWRANBE-1000.0f;
			LENS_REALPOLYTREE=6000.0f;
//			LENS_REALPOLYTREE=10000.0f;
			break;
		case 2:
			MAX_HOUSEVIEWRANGE=60000.0f;
			MAX_HOUSEDISAPPER=MAX_HOUSEVIEWRANGE-3000.0f;
			MAX_OUTVIEWRANGE=15000.0f;
			MAX_MEDVIEWRANGE=15000.0f;
			MAX_INVIEWRANGE=4200.0f;
			MAX_OBJECTVIEWRANBE=5000.0f;
//			MAX_OBJECTVIEWRANBE=9000.0f;
			MAX_OBJECTDISAPPER=MAX_OBJECTVIEWRANBE-1000.0f;
			LENS_REALPOLYTREE=8000.0f;
//			LENS_REALPOLYTREE=15000.0f;
			break;
		case 3:
			MAX_HOUSEVIEWRANGE=80000.0f;
			MAX_HOUSEDISAPPER=MAX_HOUSEVIEWRANGE-4000.0f;
			MAX_OUTVIEWRANGE=15000.0f;
			MAX_MEDVIEWRANGE=15000.0f;
			MAX_INVIEWRANGE=4200.0f;
			MAX_OBJECTVIEWRANBE=10000.0f;
//			MAX_OBJECTVIEWRANBE=12000.0f;
			MAX_OBJECTDISAPPER=MAX_OBJECTVIEWRANBE-1000.0f;
			LENS_REALPOLYTREE=10000.0f;
//			LENS_REALPOLYTREE=20000.0f;
			break;
	}	
	//LogMessage(strPath);	
	/*
	char strNsTexturePath[256];
	
	sprintf(strNsTexturePath,"%s\\Texture\\Interface\\",strPath);
	CTexture::SetPath(strNsTexturePath);	
	*/
	///////////////////////////////
	// 추가 effect 최적화
	if(ms_pBufferPools)
	{
		delete ms_pBufferPools;
	}
	if(ms_pEffectCacheMgr)
	{
		delete ms_pEffectCacheMgr;
	}
	if(ms_pResourceLoader)
	{
		delete ms_pResourceLoader;
	}

	if(ms_pNewMeshContainer)
	{
		delete ms_pNewMeshContainer;
	}

	
	if(!m_pPool)
	{
		m_pPool = new CROSSM::CPool;
		m_pPool->Init();

	}
	if(!m_pResource)
	{
		m_pResource = new CROSSM::CResource;
		m_pResource->Init();
	
	}
	if(!m_pRLoader)
	{
		m_pRLoader = new CROSSM::CResourceLoader;
		m_pRLoader->StartLoader();
	}
	

	ms_pNewMeshContainer = new Caldron::Base::CBaseCacheMgr<CMeshObject>;

	ms_pResourceLoader = new Caldron::Base::CResourceLoader;
	ms_pResourceLoader->StartLoader();


	ms_pBufferPools = new Caldron::Scene::CD3DBufferPools;
	ms_pBufferPools->InitBuffer();

	ms_pEffectCacheMgr = new CEffectCacheMgr;
	//////////////////////////////

	
	SetSceneManagerPath(strPath);
	
	SHADOWSIZE=256;
	//LogMessage("Perlin Noise Create");
	CPerlinNoise::Init();
	//LogMessage("HeightField Create");	
	//m_HeightField.Create(7,7,&m_MapStorage);
	m_HeightField.Create(CRenderOption::m_TerrainRange,CRenderOption::m_TerrainRange,&m_MapStorage);		
	//LogMessage("WeatherManager Create");	
	m_CollisionDetection.m_HeightField=&m_HeightField;
	//LogMessage("CharacterMan2ger Create");	
	m_InstanceObjectManager.Create(m_pd3dDevice);
	m_WeatherManager.Load();
	
///////		
	char strTableName[256] = {0,};
	char strBuffer[256] = {0,};
	char *strPtr = NULL;
	strcpy(strTableName,WEATHERPATH);
	strPtr = strrchr(strTableName,'.');
	strPtr++;
	(*strPtr) = 't';
	strPtr++;
	(*strPtr) = 'a';
	strPtr++;
	(*strPtr) = 'b';
	strPtr++;
	(*strPtr) = 'l';
	strPtr++;
	(*strPtr) = 'e';
	strPtr++;
	(*strPtr) = 0;

	FILE *fp = fopen(strTableName,"rt");
	if(fp)
	{
		
		int *iPtr;
		int iPtrNum;
		char *str;
		fgets(strBuffer,256,fp);
		iPtrNum = atoi(strBuffer);
		iPtr = new int[iPtrNum];
		memset(strBuffer,0,sizeof(char) * 256);
		fgets(strBuffer,256,fp);
		int iNum;
	
		for(iNum = 0; iNum < iPtrNum; iNum++)
		{
			if(iNum == 0)
				str = strtok(strBuffer," \n");
			else
				str = strtok(NULL," \n");
			iPtr[iNum] = atoi(str);
		}
		
		fclose(fp);

		m_WeatherManager.LoadSubTable(iPtr,iPtrNum);
		delete[] iPtr;

		
	}
	else 
	{
		m_WeatherManager.DeleteSubTable();
	}
////////

	m_MapStorage.SetLoadMode(1);

	m_EffectManager.SetDevice(m_pd3dDevice);
	//////	
	ms_pEffectCacheMgr->InitLoad();

	//CParticle::SetMatPosition(&m_ViewCamera->m_matPosition);

	//SetSceneManagerPath("c:\\MP-Project");	
	strcpy(CRenderOption::m_strBaseGraphicsDataPath,"Zone15");
	///strcpy(CRenderOption::m_strBaseGraphicsDataPath,"");	
	BaseGraphicsDataLoad(CRenderOption::m_strBaseGraphicsDataPath);
	m_WeatherManager.Create();
	m_CharacterManager.Create();
	m_CharacterManager.SetHeightField(&m_HeightField);
	if(m_pCameraScript == NULL) {
		m_pCameraScript = new CCameraScript;
		m_pCameraScript->LoadScript("stone.cst");
	}
	
	if(m_bChristmas) {
		m_pChristmasParticleManager = new CChristmasParticleManager;
		
	}
	////////////Minotaurs 눈 비 등등등 만들기
	D3DXMATRIX tmpview;
	m_pd3dDevice->GetTransform(D3DTS_VIEW,&tmpview);
	D3DXMatrixInverse(&tmpview,NULL,&tmpview);
	
	vector3 tmppos;
	tmppos.x = tmpview._41;
	tmppos.y = tmpview._42;
	tmppos.z = tmpview._43;
	//m_SnowFall.SetWorldValue(0.0f,-2.0f,0.0f,8.0f,-3.0f,8.0f,"snow.dds");
	////m_SnowFall.Init(GetDevice(),300,tmppos.x,tmppos.y,tmppos.z,800.0f,800.0f,tmppos.y + 100.0f);
	//m_SnowFall.Init(GetDevice(),500,tmppos.x,tmppos.y,tmppos.z,800.0f,800.0f,tmppos.y + 2000.0f);
	//
	// 눈 의 초기화
  	m_SnowFall.SetWorldValue(0.0f,-2.0f,0.0f,1.0f,-0.5f,1.0f,"snow.dds");
	//m_SnowFall.Init(GetDevice(),300,tmppos.x,tmppos.y,tmppos.z,800.0f,800.0f,tmppos.y + 100.0f);
	m_SnowFall.Init(GetDevice(),100,tmppos.x,tmppos.y,tmppos.z,800.0f,800.0f,tmppos.y + 2000.0f);

	// 비 의 초기화
 	m_Rain.SetWorldValue(0.0f,-40.0f,0.0f,1.0f,-0.5f,1.0f,"RainPar.dds"); 
	//m_SnowFall.Init(GetDevice(),300,tmppos.x,tmppos.y,tmppos.z,800.0f,800.0f,tmppos.y + 100.0f);
	m_Rain.Init(GetDevice(),120,tmppos.x,tmppos.y,tmppos.z,1000.0f,1000.0f,tmppos.y + 2000.0f);

	///////////////////크리스마스 눈 
	/*
	if(m_bSnowFall) {
		D3DXMATRIX tmpview;
		m_pd3dDevice->GetTransform(D3DTS_VIEW,&tmpview);
		D3DXMatrixInverse(&tmpview,NULL,&tmpview);
		
		vector3 tmppos;
		tmppos.x = tmpview._41;
		tmppos.y = tmpview._42;
		tmppos.z = tmpview._43;
		/*m_SnowFall.SetWorldValue(0.0f,-2.0f,0.0f,8.0f,-3.0f,8.0f,"snow.dds");
		//m_SnowFall.Init(GetDevice(),300,tmppos.x,tmppos.y,tmppos.z,800.0f,800.0f,tmppos.y + 100.0f);
		m_SnowFall.Init(GetDevice(),500,tmppos.x,tmppos.y,tmppos.z,800.0f,800.0f,tmppos.y + 2000.0f);
		
		
		m_SnowFall.SetWorldValue(0.0f,-2.0f,0.0f,1.0f,-0.5f,1.0f,"snow.dds");
		//m_SnowFall.Init(GetDevice(),300,tmppos.x,tmppos.y,tmppos.z,800.0f,800.0f,tmppos.y + 100.0f);
		m_SnowFall.Init(GetDevice(),100,tmppos.x,tmppos.y,tmppos.z,800.0f,800.0f,tmppos.y + 2000.0f);
	}
	*/
	//////////////////
	if(m_pFullSceneEffect) {
		delete m_pFullSceneEffect;
		m_pFullSceneEffect = NULL;
	}
	bool bSceneFilter = (CRenderOption::m_ScreenFilter > 0) ? true : false;
	m_pFullSceneEffect = new CFullSceneEffect;
	m_pFullSceneEffect->SetFullSceneEffectEnable(bSceneFilter);
	m_pFullSceneEffect->Init(GetDevice());
	
	//================================
	// Bgm &  Ambience & Event
	//================================
	CAmbienceManager::_GetInstance()->Create(); 
	CBgmManager::_GetInstance()->Create(); 
	SAFE_DELETE( ms_pSceneEventMgr ); 
	ms_pSceneEventMgr = new CSceneEventMgr;
	CImposterScene::Init(); 

	///////////////
		
	// Cubemap Generation
	//////////////
	if(CRenderOption::m_WaterBumpEnvRendering)
	{
		if(m_pRenderEnvMap == NULL)
		{
			D3DXCreateRenderToEnvMap( BaseGraphicsLayer::GetDevice(), 256, 
				BaseGraphicsLayer::m_d3dpp.BackBufferFormat, TRUE, D3DFMT_D16, &m_pRenderEnvMap );
			
			D3DXCreateCubeTexture( BaseGraphicsLayer::GetDevice(), 256, 1,
					D3DUSAGE_RENDERTARGET,BaseGraphicsLayer::m_d3dpp.BackBufferFormat, D3DPOOL_DEFAULT, &m_pWorldCubemap );
		}
	}
	//////////////
}

CParticleSequence * CSceneManager::AddParticle(vector3 vecPos,vector3 vecTarget)
{
	return m_ParticleManager.AddParticle(SMOKE_PARTICLE,vecPos,vecTarget);
	//return m_Particle.
}

void CSceneManager::DeleteCharacter(CZ3DGeneralChrModel *pDelChr)
{
	m_CharacterManager.DeleteCharacter(pDelChr);	
}

int CSceneManager::AddInstanceObject(char *strObjectName, vector3 vecPos, float fAngle)
{
	return m_InstanceObjectManager.AddInstanceObject(strObjectName,vecPos, fAngle);
}

void CSceneManager::DeleteInstanceObject(int InstanceObjectID)
{
	m_InstanceObjectManager.DeleteInstanceObject(InstanceObjectID);
}

int CSceneManager::MouseOnCheckInstanceObject(int px, int py)
{
	return m_InstanceObjectManager.MouseOnCheckInstanceObject(px,py);
}

void CSceneManager::SetIntanceObjectColor(int ID,color Color)
{
	m_InstanceObjectManager.SetInstanceObjectColor(ID,Color);
}

void CSceneManager::SetAllInstanceObjectColor(color Color)
{
	m_InstanceObjectManager.SetAllObjectColor(Color);
}

void CSceneManager::PickMakeRay(int dx, int dy,int ScreenX,int ScreenY)
{	
	matrix matProj,matView,matInv;
	m_pd3dDevice->GetTransform(D3DTS_PROJECTION,matProj);
	m_pd3dDevice->GetTransform(D3DTS_VIEW,matView);

	vector3 v;
	v.x= ( ( (2.0f*dx) / ScreenX) -1 ) / matProj._11;
	v.y=-( ( (2.0f*dy) / ScreenY) -1 ) / matProj._22;
	v.z= 1.0f;
	
	matInv.Inverse(matView);
	m_vecPickRayStart.x=matInv._41;
	m_vecPickRayStart.y=matInv._42;
	m_vecPickRayStart.z=matInv._43;

	m_vecPickRayDir.x=v.x*matInv._11 + v.y*matInv._21 + v.z*matInv._31;
	m_vecPickRayDir.y=v.x*matInv._12 + v.y*matInv._22 + v.z*matInv._32;
	m_vecPickRayDir.z=v.x*matInv._13 + v.y*matInv._23 + v.z*matInv._33;
}

CSectorScene* CSceneManager::PickTerrain()
{
	m_vecPickRayStart;
	m_vecPickRayDir;

	float fIntersection;

	int ix,iy;
	for(ix=0;ix<m_HeightField.GetLSizeX();ix++)
	{
		for(iy=0;iy<m_HeightField.GetLSizeY();iy++)
		{
			vector3 vecSectorPoly[3];
			vecSectorPoly[0].x=m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_AccumulateTM._41;
			vecSectorPoly[0].z=m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_AccumulateTM._43;
			vecSectorPoly[0].y=0.0f;

			vecSectorPoly[1].x=m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_AccumulateTM._41+SECTORSIZE;
			vecSectorPoly[1].z=m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_AccumulateTM._43;
			vecSectorPoly[1].y=0.0f;

			vecSectorPoly[2].x=m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_AccumulateTM._41;
			vecSectorPoly[2].z=m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_AccumulateTM._43+SECTORSIZE;
			vecSectorPoly[2].y=0.0f;

			if(CIntersection::PolygonRay(m_vecPickRayStart,m_vecPickRayStart+m_vecPickRayDir*100000.0f,vecSectorPoly,fIntersection)==1)
			{
				return &m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()];
			}

			vecSectorPoly[0].x=m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_AccumulateTM._41+SECTORSIZE;
			vecSectorPoly[0].z=m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_AccumulateTM._43;
			vecSectorPoly[0].y=0.0f;

			vecSectorPoly[1].x=m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_AccumulateTM._41;
			vecSectorPoly[1].z=m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_AccumulateTM._43+SECTORSIZE;
			vecSectorPoly[1].y=0.0f;

			vecSectorPoly[2].x=m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_AccumulateTM._41+SECTORSIZE;
			vecSectorPoly[2].z=m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_AccumulateTM._43+SECTORSIZE;
			vecSectorPoly[2].y=0.0f;			

			if(CIntersection::PolygonRay(m_vecPickRayStart,m_vecPickRayStart+m_vecPickRayDir*100000.0f,vecSectorPoly,fIntersection)==1)
			{
				return &m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()];
			}
		}
	}
	return NULL;
	/*
	int ix,iy;
	for(ix=0;ix<LSIZEX;ix++)
	{
		for(iy=0;iy<LSIZEY;iy++)
		{
			if( m_HeightField.m_SectorScene[ix+iy*LSIZEX].m_AccumulateTM._41 <= fXp &&
				m_HeightField.m_SectorScene[ix+iy*LSIZEX].m_AccumulateTM._41 + SECTORSIZE > fXp &&
				m_HeightField.m_SectorScene[ix+iy*LSIZEX].m_AccumulateTM._43 <= fZp &&
				m_HeightField.m_SectorScene[ix+iy*LSIZEX].m_AccumulateTM._43 + SECTORSIZE > fZp)
			{
				return &m_HeightField.m_SectorScene[ix+iy*LSIZEX];
			}
			
		}
	}
	*/

	//m_vecPickRayStart=;
	//m_vecPickRayEnd=;
}

int CSceneManager::GetCharacterCount()
{
	//return m_CharacterManager.m_CharacterScreenPositionList.num;
	return (int)m_CharacterManager.m_CharacterList.size();
}

POINT CSceneManager::GetChacterScreenPos(int nCount)
{
	//return m_CharacterManager.m_CharacterScreenPositionList[nCount];
	return m_CharacterManager.m_CharacterList[nCount].m_ptScreenPosition;

}

vector3 CSceneManager::GetCharacterPos(int nCount)
{
	vector3 vecPos;
	m_CharacterManager.m_CharacterList[nCount].m_pChrmodel->GetPosition(vecPos.x,vecPos.y,vecPos.z);

	return vecPos;
}

vector3 CSceneManager::PickPointTerrain()
{
	CSectorScene *pSelectSector=PickTerrain();
	if(pSelectSector==NULL)
		return vector3(0.0f,0.0f,0.0f);
	
	SectorVertex *pEditSectorVertex;	
	pSelectSector->m_pROAMVertex->Lock(0,0,(BYTE**)&pEditSectorVertex,0);	

	vector3 vecTerrainPos,vecLens;
	vector3 vecMousePos;	

	vector3 vecStart=m_vecPickRayStart;
	vector3 vecDir=m_vecPickRayDir;

	float fIntersection;

	vector3 vecIntersectionPoint;
	vecIntersectionPoint.x=-1.0f;

	vector3 vecCollisionVertex[3];

	matrix matTerrainTM,matTM;
	matTerrainTM=pSelectSector->m_AccumulateTM;
	matTM.Translation(vecStart);
	matTerrainTM.Inverse(matTerrainTM);
	matTM=matTM*matTerrainTM;
	vecStart=matTM.GetLoc();

	float fLens=100000000.0f;
	bool bFound=false;

	for(int cIndices=0;cIndices<pSelectSector->m_UsedIndices/3;cIndices++)
	{
		vecCollisionVertex[0]=pSelectSector->m_pDrawVertex[pSelectSector->m_pDrawIndices[cIndices*3+0]].v;
		vecCollisionVertex[1]=pSelectSector->m_pDrawVertex[pSelectSector->m_pDrawIndices[cIndices*3+1]].v;
		vecCollisionVertex[2]=pSelectSector->m_pDrawVertex[pSelectSector->m_pDrawIndices[cIndices*3+2]].v;
		if(CIntersection::PolygonRay(vecStart,vecStart+100000.0f*vecDir,vecCollisionVertex,fIntersection)==1)
		{				
			if(fLens>fIntersection)
			{
				vecIntersectionPoint=vecStart+vecDir*fIntersection;
				fLens=fIntersection;
				bFound=true;
			}
		}
	}
	vecIntersectionPoint.x+=pSelectSector->m_AccumulateTM._41;
	vecIntersectionPoint.z+=pSelectSector->m_AccumulateTM._43;
	pSelectSector->m_pROAMVertex->Unlock();
	if(!bFound)
		return vector3(0.0f,0.0f,0.0f);
	return vecIntersectionPoint;
}

void CSceneManager::CharacterSoundPlay(CZ3DGeneralChrModel *pChrmodel, char *strSoundFile)
{
	m_CharacterManager.CharacterSoundPlay(pChrmodel,strSoundFile);
}

void CSceneManager::CharacterHeightFix()
{
	m_CharacterManager.HeightFiledOnChrFixPos();
}

void CSceneManager::LoadMap(int type, char *szZoneFilename, char* szBaseGraphicsDataPath, vector3 vZoneBoundMin, vector3 vZoneBoundMax)
{
	BaseGraphicsDataUnload();
	DeleteAllData();

	BaseGraphicsDataLoad(szBaseGraphicsDataPath);
	if(type == 0)
		m_MapStorage.LoadGF3(szZoneFilename);
	else
		m_MapStorage.LoadGF3Mem(szZoneFilename);
// sector 갯수를 하나로 놓고 지형충돌코드 디버깅할때 이용
//m_MapStorage.m_HeightMap.num = 1;

	// 충돌 정보 로드
// 	unsigned int uiTriCount;
// 	CollectCollisionTriangleCount(uiTriCount);
// 	FillCollisionTriangle(uiTriCount);
// 	BuildCollisionOctree();

	// 할로윈 이벤트 (날씨정보는 이벤트이든 아니든 모두 로드하게 한다)
	m_WeatherManager.Reload();

    SetMapSize(vZoneBoundMin, vZoneBoundMax);
}

void CSceneManager::AllObjectCalcLens()
{
	m_HeightField.AllObjectCalcLens();
}

int CSceneManager::GetCharacterEvent(CZ3DGeneralChrModel *pChrmodel,DWORD Event)
{
	return m_CharacterManager.GetCharacterEvent(pChrmodel,Event);
}

void CSceneManager::AddDelayTimer(CZ3DGeneralChrModel *pChr, int nDelay)
{
	m_CharacterManager.AddDelayTimer(pChr,nDelay);
}

void CSceneManager::AddChrLight(CZ3DGeneralChrModel *pChrmodel,vector3 vecPos, DWORD dwColor, float fLens, DWORD dwTime)
{
	m_CharacterManager.AddChrLight(pChrmodel,vecPos,dwColor,fLens,dwTime);
}

void CSceneManager::PickingObjectReload()
{	
}

//void CSceneManager::MeshObjectReload(CMeshObject *pMesh)
//{
//	CTexture::SetPath(OBJECTTEXTUREPATH);
//	char strReload[256];
//	for(int i=0;i<pMesh->m_pMat.num;i++)
//	{
//		if(strstr(pMesh->m_pMat[i]->m_strName,"_Reload")==NULL)
//		{			
//			sprintf(strReload,"%s%s",pMesh->m_pMat[i]->m_strName," _Reload");
//			strcpy(pMesh->m_pMat[i]->m_strName,strReload);
//		}
//		else
//		{
//			pMesh->m_pMat[i]->m_pddTexture->Release();
//		}
//		sscanf(pMesh->m_pMat[i]->m_strName,"%s _Reload",strReload);
//		strcpy(pMesh->m_pMat[i]->m_strName,strReload);
//		pMesh->m_pMat[i]->ReadDDSTexture(pMesh->m_pMat[i]->m_pddTexture,1);		
//		sprintf(strReload,"%s%s",pMesh->m_pMat[i]->m_strName," _Reload");
//		strcpy(pMesh->m_pMat[i]->m_strName,strReload);
//	}
//	pMesh->MeshReload();
//}
void CSceneManager::DeleteAllEffect(){

	m_EffectManager.DeleteAllEffect();
	m_EffectManager.DeleteAllWorldScript();
}
void CSceneManager::DeleteAllWorldEffect() {

	m_EffectManager.DeleteAllWorldScript();

}
XEFFECT CSceneManager::AddEffect(CX3DEffect *lpNewEffect)
{
	m_EffectNum++;
	lpNewEffect->SetEffectStart(true);
	return m_EffectManager.AddEffect(lpNewEffect);
}
CEffScript *CSceneManager:: AddEffectScript (CEffScript *lpnewScript) {
	return m_EffectManager.AddEffScript(lpnewScript);
}
void CSceneManager:: AddInterfaceScript (CEffScript *lpnewScript) {
	m_EffectManager.AddInterfaceScript(lpnewScript);

}
void CSceneManager:: AddWorldEffect(CEffScript *lpnewScript) {
	m_EffectManager.AddWorldEffScript(lpnewScript);
}
void CSceneManager::DeleteEffect(XEFFECT hEffect)
{
	m_EffectNum--;
	m_EffectManager.DeleteEffect(hEffect);
}

BOOL CSceneManager::EffectIsLive(XEFFECT hEffect)
{
	return  m_EffectManager.IsLive(hEffect);
}

void CSceneManager::DeleteEffect(CX3DEffect *del)
{
	
	for(int i=0;i<m_EffectNum;i++) {
		if(m_EffectManager.m_lstEffect[i] == del) {
			DeleteEffect(i);
			m_EffectNum--;
			break;
		}
	}
}
void CSceneManager::UnPickEffect() {
	if(m_PickEffect) {
		m_PickEffect->pick_index = -1;
		m_PickEffect = NULL;
	}
}
void CSceneManager::PickEffect(float fMx,float fMy,float fMz) {
	vector3 vecPos;
	float length = 0.0f;
	float line_length = 0.0f;
	float min = 0.0f;
	float pick_range = 50.0f;
	float e_dot = 0.0f;
	int min_index = -1;
	
	vector3 tmp;
	// 라인에서 점에 가장 가까운 위치
	vector3 e_Closet;

	vector3 e_RayStart = m_vecPickRayStart;
	vector3 e_RayEnd = e_RayStart + m_vecPickRayDir * pick_range;
	
	vector3 e_vector1,e_vector2,e_vector3;
	vector3 test_vector;
	m_PickEffect = m_MapStorage.FindEffectMap(fMx,fMz);
	
	if(m_PickEffect) {
		for(int i= 0 ;i<(int)m_PickEffect->m_vecEffectPos.size();i++) {
			tmp = m_PickEffect->m_vecEffectPos[i];
			e_vector1 = tmp - e_RayStart;
			e_vector2 = e_RayEnd - e_RayStart;
			line_length = e_vector2.GetLens();

			e_vector2.Normalize();
			
			e_dot = e_vector1 * e_vector2;
			if(e_dot <=0)
				e_Closet = e_RayStart;
			else if(e_dot >= line_length) 
				e_Closet = e_RayEnd;
			// 거리
			else {
				e_vector3 = e_vector2 * e_dot;
				e_Closet =  e_vector1 + e_vector3;
			}
			//레이에서 포인트 까지의 vector
			test_vector = tmp - e_Closet;
			length = test_vector.GetLens();

			if(i == 0) {
				min = length;
				min_index = i;
			}
			else if(min >= length) {
				min_index = i;
				min = length;
			}
		}//for		
		if(min_index >=0) {
			m_PickEffect->pick_index = min_index;
		}
	}


//	vecPos.x = fMx;
//	vecPos.y = fMy;
//	vecPos.z = fMz;
	

/*	
	if(m_PickEffect) {
		for(int i= 0 ;i<m_PickEffect->m_vecEffectPos.num;i++) {
			tmp = vecPos - m_PickEffect->m_vecEffectPos[i];
							
			length = tmp.GetLens();
			if(i == 0)
				min = length;
			if(min >= length)
				min_index = i;
		}//for		
	}
	if(min_index >=0) {
		m_PickEffect->pick_index = min_index;
	}
*/

}
void CSceneManager::PickLandscapeEffect(float fPx,float fPy,float fPz,int PickMode)
{
		vector3 vecPos;

		vecPos.x = fPx;
		vecPos.y = fPy;
		vecPos.z = fPz;


		if(PickMode == 0) { // vfog
			float length = 0.0f;
			float min = 0.0f;
			int min_index = -1;
			vector3 tmp;

			m_PickLandscape = m_MapStorage.FindLandscapeEffect(fPx,fPz);
			if(m_PickLandscape) {
				for(int i = 0;i<(int)m_PickLandscape->m_LandscapeEffect.size();i++) {
					tmp = vecPos - m_PickLandscape->m_LandscapeEffect[i].m_vecCenterPos;
					m_PickLandscape->m_LandscapeEffect[i].m_IntParam[1] = false;
							
					length = tmp.GetLens();
					if(i == 0)
						min = length;
					if(min >= length)
						min_index = i;
				}
			}
			if(min_index>=0) { // find closet landeffect
				m_PickLandscape->m_LandscapeEffect[min_index].m_IntParam[1] = true;
			}
		}
		else if(PickMode == 1) {// boid
			float length = 0.0f;
			float min = 0.0f;
			int min_index = -1;
			vector3 tmp;

			m_PickLandscape = m_MapStorage.FindLandscapeEffect(fPx,fPz);
			if(m_PickLandscape) {
				for(int i = 0;i<(int)m_PickLandscape->m_LandscapeEffect.size();i++) {
					tmp = vecPos - m_PickLandscape->m_LandscapeEffect[i].m_vecCenterPos;
					m_PickLandscape->m_LandscapeEffect[i].m_IntParam[1] = false;
							
					length = tmp.GetLens();
					if(i == 0)
						min = length;
					if(min >= length)
						min_index = i;
				}
			}
			if(min_index>=0) { // find closet landeffect
				m_PickLandscape->m_LandscapeEffect[min_index].m_IntParam[1] = true;
			}
		}
		else if(PickMode == 2) { //mesh
			float length = 0.0f;
			float min = 0.0f;
			int min_index = -1;
			vector3 tmp;

			m_PickLandscape = m_MapStorage.FindLandscapeEffect(fPx,fPz);
			if(m_PickLandscape) {
				for(int i = 0;i<(int)m_PickLandscape->m_LandscapeEffect.size();i++) {
					tmp = vecPos - m_PickLandscape->m_LandscapeEffect[i].m_vecCenterPos;
					m_PickLandscape->m_LandscapeEffect[i].m_IntParam[2] = false;
							
					length = tmp.GetLens();
					if(i == 0)
						min = length;
					if(min >= length)
						min_index = i;
				}
			}
			if(min_index>=0) { // find closet landeffect
				m_PickLandscape->m_LandscapeEffect[min_index].m_IntParam[2] = true;
			}
		}
		else if(PickMode == 3) {// wave
			float length = 0.0f;
			float min = 0.0f;
			int min_index = -1;
			vector3 tmp;

			m_PickLandscape = m_MapStorage.FindLandscapeEffect(fPx,fPz);
			if(m_PickLandscape) {
				for(int i = 0;i<(int)m_PickLandscape->m_LandscapeEffect.size();i++) {
					tmp = vecPos - m_PickLandscape->m_LandscapeEffect[i].m_vecCenterPos;
					m_PickLandscape->m_LandscapeEffect[i].m_IntParam[0] = false;
							
					length = tmp.GetLens();
					if(i == 0)
						min = length;
					if(min >= length)
						min_index = i;
				}
			}
			if(min_index>=0) { // find closet landeffect
				m_PickLandscape->m_LandscapeEffect[min_index].m_IntParam[0] = true;
			}
		}
}
void CSceneManager::DeletePickingLandscapeEffect() {
	CSectorScene *pSector;

	if(m_PickLandscape != NULL) {
		for(int i = 0;i<(int)m_PickLandscape->m_LandscapeEffect.size();i++) {
			if(m_PickLandscape->m_LandscapeEffect[i].m_nEffectMethod == 1) { 
				// vol-fog
				if(m_PickLandscape->m_LandscapeEffect[i].m_IntParam[1] == 1) {
					m_PickLandscape->m_LandscapeEffect.erase(m_PickLandscape->m_LandscapeEffect.begin()+i);
					break;
				}
			}
			else if(m_PickLandscape->m_LandscapeEffect[i].m_nEffectMethod == 2) {
				// boid
				if(m_PickLandscape->m_LandscapeEffect[i].m_IntParam[1] == 1) {
					m_PickLandscape->m_LandscapeEffect.erase(m_PickLandscape->m_LandscapeEffect.begin()+i);
					break;
				}
			}
			else if(m_PickLandscape->m_LandscapeEffect[i].m_nEffectMethod == 3) {
				//mesh ani
				if(m_PickLandscape->m_LandscapeEffect[i].m_IntParam[2] == 1) {
					m_PickLandscape->m_LandscapeEffect.erase(m_PickLandscape->m_LandscapeEffect.begin()+i);	
					break;
				}
			}
			else if(m_PickLandscape->m_LandscapeEffect[i].m_nEffectMethod == 4) {
				// wave
				if(m_PickLandscape->m_LandscapeEffect[i].m_IntParam[0] == 1) {
					m_PickLandscape->m_LandscapeEffect.erase(m_PickLandscape->m_LandscapeEffect.begin()+i);
					break;
				}
			}
		}
		if(m_PickLandscape->m_LandscapeEffect.size()== 0) {
			m_MapStorage.DeleteLandscapeEffect(m_PickLandscape);
		}

		m_PickLandscape = NULL;
		pSector=PickTerrain();
		pSector->GenerateLandscapeEffect();
	}
	
}
void CSceneManager::DeletePickEffect() {
	CSectorScene *pSector;
	
	vector3 *pvecPos = GetCamera()->GetPosition();
	vector3 vecPos = (*pvecPos);

	pSector=PickTerrain(vecPos.x,vecPos.y,vecPos.z);

	if(m_PickEffect != NULL) {
		m_PickEffect->m_vecEffectPos.erase(m_PickEffect->m_vecEffectPos.begin()+m_PickEffect->pick_index);
		m_PickEffect->m_strEffectName.erase(m_PickEffect->m_strEffectName.begin()+m_PickEffect->pick_index);
		
		m_PickEffect->pick_index = -1;
		m_PickEffect = NULL;

		pSector->GenerateEffect();
	}

}
void CSceneManager::PickEffectMove(float fMx,float fMy,float fMz,float fAngle,float fAddy,int MoveMode){
	CSectorScene *pSector;

	m_vecPickRayDir.x = 0.0f;
	m_vecPickRayDir.y = -1.0f;
	m_vecPickRayDir.z = 0.0f;

	vector3 *pvecPos = GetCamera()->GetPosition();
	vector3 vecPos = (*pvecPos);

	pSector=PickTerrain(vecPos.x,vecPos.y,vecPos.z);


	if(m_PickEffect) {
		if(MoveMode == 0) {//  땅에서 이동
			m_PickEffect->m_vecEffectPos[m_PickEffect->pick_index].x = fMx;
			//m_PickEffect->m_vecEffectPos.x = fMx;
			m_PickEffect->m_vecEffectPos[m_PickEffect->pick_index].z = fMz;
		}
		else if(MoveMode == 1) {// y 축 이동
			m_PickEffect->m_vecEffectPos[m_PickEffect->pick_index].y+=fAddy;
		}
		pSector->GenerateEffect();
	}
}
void CSceneManager::PickEffectRot(float x,float y,float z,int MoveMode) {
	static float xrot = 0.0f;
	static float yrot = 0.0f;
	static float zrot = 0.0f;

	CSectorScene *pSector;

	m_vecPickRayDir.x = 0.0f;
	m_vecPickRayDir.y = -1.0f;
	m_vecPickRayDir.z = 0.0f;

	vector3 *pvecPos = GetCamera()->GetPosition();
	vector3 vecPos = (*pvecPos);

	pSector=PickTerrain(vecPos.x,vecPos.y,vecPos.z);

	if(m_PickEffect) {
		if(MoveMode == 0) {//  x축 rot 	
			m_PickEffect->m_vecEffectRot[m_PickEffect->pick_index].x +=x;
			if(m_PickEffect->m_vecEffectRot[m_PickEffect->pick_index].x >= 360.0f) {
				m_PickEffect->m_vecEffectRot[m_PickEffect->pick_index].x = m_PickEffect->m_vecEffectRot[m_PickEffect->pick_index].x - 360.0f;
			}
		}
		else if(MoveMode == 1) {// y 축 rot
			m_PickEffect->m_vecEffectRot[m_PickEffect->pick_index].y +=y;
			
			if(m_PickEffect->m_vecEffectRot[m_PickEffect->pick_index].y >= 360.0f) {
				m_PickEffect->m_vecEffectRot[m_PickEffect->pick_index].y = m_PickEffect->m_vecEffectRot[m_PickEffect->pick_index].y - 360.0f;
			}
		}
		else if(MoveMode == 2) { //z 축 rot
			m_PickEffect->m_vecEffectRot[m_PickEffect->pick_index].z +=z;
			
			if(m_PickEffect->m_vecEffectRot[m_PickEffect->pick_index].z >= 360.0f) {
				m_PickEffect->m_vecEffectRot[m_PickEffect->pick_index].z = m_PickEffect->m_vecEffectRot[m_PickEffect->pick_index].z - 360.0f;
			}
		}
		pSector->GenerateEffect();
	}
}

void CSceneManager::PickLandscapeEffectMove(float fMx,float fMy,float fMz,float fAngle,float fAddy,int MoveMode){
	int i;
	CSectorScene *pSector;
	pSector=PickTerrain();

	if(m_PickLandscape) {
		for(i=0;i<(int)m_PickLandscape->m_LandscapeEffect.size();i++) {
			switch(m_PickLandscape->m_LandscapeEffect[i].m_nEffectMethod) {

			case 1:
				if(m_PickLandscape->m_LandscapeEffect[i].m_IntParam[1] == 1) {
					if(MoveMode == 0) { // x,z move
						m_PickLandscape->m_LandscapeEffect[i].m_vecCenterPos.x =fMx;
						//m_PickLandscape->m_LandscapeEffect[i].m_vecCenterPos.y =fMy;
						m_PickLandscape->m_LandscapeEffect[i].m_vecCenterPos.z =fMz;
						
					}
					else if(MoveMode == 1) {// y axis move
						m_PickLandscape->m_LandscapeEffect[i].m_vecCenterPos.y +=fAddy;
					}
					pSector->GenerateLandscapeEffect();
				}
				break;
			case 3:

				if(m_PickLandscape->m_LandscapeEffect[i].m_IntParam[2] == 1) {
					if(MoveMode == 0) { // x,z move
						m_PickLandscape->m_LandscapeEffect[i].m_vecCenterPos.x =fMx;
						m_PickLandscape->m_LandscapeEffect[i].m_vecCenterPos.y =fMy;
						m_PickLandscape->m_LandscapeEffect[i].m_vecCenterPos.z =fMz;
					}
					else if(MoveMode == 1) {// y axis move
						m_PickLandscape->m_LandscapeEffect[i].m_vecCenterPos.y +=fAddy;
					}
					else if(MoveMode == 2) {//x axis rot
						m_PickLandscape->m_LandscapeEffect[i].m_Param[4] +=fAngle;
						if(m_PickLandscape->m_LandscapeEffect[i].m_Param[4] >360.0f)
							m_PickLandscape->m_LandscapeEffect[i].m_Param[4] = 0.0f;
						pSector->GenerateLandscapeEffect();
					}
					else if(MoveMode == 3) {//y axis rot
						m_PickLandscape->m_LandscapeEffect[i].m_Param[5] +=fAngle;
						if(m_PickLandscape->m_LandscapeEffect[i].m_Param[5] >360.0f)
							m_PickLandscape->m_LandscapeEffect[i].m_Param[5] = 0.0f;
						pSector->GenerateLandscapeEffect();
					}
					else if(MoveMode == 4) {//z axis rot
						m_PickLandscape->m_LandscapeEffect[i].m_Param[6] +=fAngle;					
						if(m_PickLandscape->m_LandscapeEffect[i].m_Param[6] >360.0f)
							m_PickLandscape->m_LandscapeEffect[i].m_Param[6] = 0.0f;
						pSector->GenerateLandscapeEffect();
					}
				}				
				break;
				
			}//switch
		}//for
	}//if
}

void CSceneManager::CharacterMovementVector(CZ3DGeneralChrModel *pChrmodel, vector3 vecMove,float fAccelate)
{
	m_CharacterManager.CharacterMovementVector(pChrmodel,vecMove,fAccelate);
}

vector3 CSceneManager::GetCharacterMovementVector(CZ3DGeneralChrModel *pChrmodel)
{
	return m_CharacterManager.GetCharacterMovementVector(pChrmodel);
}

void CSceneManager::CharacterRandomPos(CZ3DGeneralChrModel *pChrmodel,int nTimer, vector3 vecRand)
{
	m_CharacterManager.CharacterRandomPos(pChrmodel,nTimer,vecRand);
}

void CSceneManager::RenderGlareTexture()
{	
	m_pd3dDevice->EndScene();	

	LPDIRECT3DSURFACE8 m_pTempRenderSurface;		
	LPDIRECT3DSURFACE8 m_pTempRenderZBuffer;	
	m_pd3dDevice->GetRenderTarget(&m_pTempRenderSurface);
	m_pd3dDevice->GetDepthStencilSurface(&m_pTempRenderZBuffer);

	m_pd3dDevice->SetRenderTarget(m_GlareTexture.GetSurface(),m_pTempRenderZBuffer);	
	

	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
	m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);

	m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);

	////////////////////////////////////////////
	m_pd3dDevice->BeginScene();
	m_pd3dDevice->Clear(0,NULL,D3DCLEAR_TARGET,0xff000000,1.0f,0);
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,D3DTA_DIFFUSE);
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,D3DTOP_SELECTARG2);

	TLVertex pVertex[8];		
	float fTextureSize=128.0f;

	pVertex[0].v.x=0.0f;
	pVertex[1].v.x=0.0f;
	pVertex[2].v.x=fTextureSize;
	pVertex[3].v.x=fTextureSize;

	pVertex[1].v.y=0.0f;
	pVertex[3].v.y=0.0f;
	pVertex[0].v.y=fTextureSize;
	pVertex[2].v.y=fTextureSize;	
		
	pVertex[0].tu=0.0f;
	pVertex[1].tu=0.0f;

	pVertex[3].tu=1.0f;
	pVertex[2].tu=1.0f;

	pVertex[1].tv=0.0f;
	pVertex[3].tv=0.0f;

	pVertex[0].tv=1.0f;
	pVertex[2].tv=1.0f;		

	for(int i=0;i<4;i++)
	{
		pVertex[i].w=0.1f;
		pVertex[i].v.z=0.1f;
		pVertex[i].Diffuse.c=0xff0000ff;
		pVertex[i].Specular.c=0xffffffff;
	}	

	m_pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);	

	m_pd3dDevice->SetTexture(1,NULL);
	m_pd3dDevice->SetVertexShader(TLVERTEXFVF);				
	//m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(TLVertex));	

	//m_CharacterManager.Render(m_pd3dDevice);

	m_WeatherManager.m_SkyScene.RenderForGlare(m_pd3dDevice);	

	//m_pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);	
	
/*
	for(int cGlareNode=0;cGlareNode<m_GlareList.size();cGlareNode++)
	{
		
	}
*/	
	
	m_GlareList.clear();

	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);

	m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);	

	m_pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);	


	for(int ix=0;ix<m_HeightField.GetLSizeX();ix++)
	{
		for(int iy=0;iy<m_HeightField.GetLSizeY();iy++)
		{				
			if( m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_MapWater)
			{				
				m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].RenderWater(m_pd3dDevice,true);				
			}			
		}
	}	
	m_pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);	

	m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	//m_CharacterManager.RenderGlare(m_pd3dDevice);
	/*
	pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,TRUE);
	pd3dDevice->SetRenderState( D3DRS_ALPHAREF,0xee);
	*/
	//m_pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);	


	//m_WeatherManager.Render(m_pd3dDevice);

	m_pd3dDevice->EndScene();
	/////////////////////////////////////////////////
	m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	m_pd3dDevice->SetRenderTarget(m_pTempRenderSurface,m_pTempRenderZBuffer);

	m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

	m_pTempRenderSurface->Release();
	m_pTempRenderZBuffer->Release();


	m_pd3dDevice->BeginScene();
}

void CSceneManager::RenderFullSceneTextureBegin()
{	
	m_pd3dDevice->GetRenderTarget(&m_pTempRenderSurface);
	m_pd3dDevice->GetDepthStencilSurface(&m_pTempRenderZBuffer);

	m_pd3dDevice->SetRenderTarget(m_FullSceneTexture.GetSurface(),m_FullSceneTexture.GetZBuffer());			
		
	m_pd3dDevice->Clear(0,NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,m_WeatherManager.m_GF3ClareColor.c,1.0f,0);
}

void CSceneManager::RenderFullSceneTextureEnd()
{	
	if(m_FullSceneAnti)
		return;

	m_pd3dDevice->SetRenderTarget(m_pTempRenderSurface,m_pTempRenderZBuffer);
	m_pTempRenderSurface->Release();
	m_pTempRenderZBuffer->Release();
	
	TLVertex pVertex[4];	

	pVertex[0].v.x=0.0f;
	pVertex[1].v.x=0.0f;
	pVertex[2].v.x=(float)BaseGraphicsLayer::m_lScreenSx;
	pVertex[3].v.x=(float)BaseGraphicsLayer::m_lScreenSx;

	pVertex[1].v.y=0.0f;
	pVertex[3].v.y=0.0f;
	pVertex[0].v.y=(float)BaseGraphicsLayer::m_lScreenSy;
	pVertex[2].v.y=(float)BaseGraphicsLayer::m_lScreenSy;

	static float fasdf=0.0f;
		
	pVertex[0].tu=0.0f;
	pVertex[1].tu=0.0f;

	pVertex[3].tu=1.0f;
	pVertex[2].tu=1.0f;

	pVertex[1].tv=0.0f;
	pVertex[3].tv=0.0f;

	pVertex[0].tv=1.0f;
	pVertex[2].tv=1.0f;		

	for(int i=0;i<4;i++)
	{
		pVertex[i].w=0.1f;
		pVertex[i].v.z=0.1f;
		pVertex[i].Diffuse.c=0xffffffff;
		pVertex[i].Specular.c=0xffffffff;
	}
	
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );    
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);		
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);

	m_pd3dDevice->SetTexture(0,m_FullSceneTexture.GetTexture());
	m_pd3dDevice->SetTexture(1,NULL);
	m_pd3dDevice->SetVertexShader(TLVERTEXFVF);
	m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(TLVertex));	
}
/*
void CSceneManager::RenderFullSceneGlare()
{
	m_GlareTexture.GenerateMipmap(m_pd3dDevice);

	float fTextureSize=256.0f;
	
	TLVertex pVertex[4];	
	pVertex[0].v.x=0.0f;
	pVertex[1].v.x=0.0f;
	pVertex[2].v.x=(float)fTextureSize;
	pVertex[3].v.x=(float)fTextureSize;

	pVertex[1].v.y=0.0f;
	pVertex[3].v.y=0.0f;
	pVertex[0].v.y=(float)fTextureSize;
	pVertex[2].v.y=(float)fTextureSize;

	static float fasdf=0.0f;
		
	pVertex[0].tu=0.0f;
	pVertex[1].tu=0.0f;

	pVertex[3].tu=1.0f;
	pVertex[2].tu=1.0f;

	pVertex[1].tv=0.0f;
	pVertex[3].tv=0.0f;

	pVertex[0].tv=1.0f;
	pVertex[2].tv=1.0f;		

	for(int i=0;i<4;i++)
	{
		pVertex[i].w=0.1f;
		pVertex[i].v.z=0.1f;
		pVertex[i].Diffuse.c=0xffffffff;
		pVertex[i].Specular.c=0x0;
	}
	
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);    
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);		
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);

	m_pd3dDevice->SetTexture(1,NULL);
	m_pd3dDevice->SetVertexShader(TLVERTEXFVF);

	m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,D3DBLEND_ONE);
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,D3DBLEND_ONE);
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);	
	//m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);	

	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);	
	

	m_pd3dDevice->EndScene();				

	m_GlareCompositeTexture.Begin(m_pd3dDevice);

	m_pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET,0x0,1.0f, 0);	

	m_pd3dDevice->BeginScene();

	m_pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);

	color VertexColor;

	for(int cTexture=1;cTexture<m_GlareTexture.GetTextureCount();cTexture++)
	//for(int cTexture=0;cTexture<1;cTexture++)
	{
		VertexColor.r=(unsigned char)(255.0f/((float)m_GlareTexture.GetTextureCount()-1));
		//VertexColor.r=0x80;
		//VertexColor.r=0xff;
		VertexColor.a=VertexColor.b=VertexColor.g=VertexColor.r;
		VertexColor.a=255;
		////////				
		pVertex[0].Diffuse.c=pVertex[1].Diffuse.c=pVertex[2].Diffuse.c=pVertex[3].Diffuse.c
			=VertexColor.c;
		////////
		m_pd3dDevice->SetTexture(0,m_GlareTexture.GetTexture(cTexture));
		m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(TLVertex));	
	}
	
	m_pd3dDevice->EndScene();
	m_GlareCompositeTexture.End(m_pd3dDevice);	

	m_pd3dDevice->BeginScene();			

	if(m_FullSceneAnti)
	{
		pVertex[2].v.x=(float)m_ScreenSize-3;
		pVertex[3].v.x=(float)m_ScreenSize-3;
		
		pVertex[0].v.y=(float)m_ScreenSize-3;
		pVertex[2].v.y=(float)m_ScreenSize-3;
	}
	else
	{	
		pVertex[2].v.x=(float)BaseGraphicsLayer::m_lScreenSx-3;
		pVertex[3].v.x=(float)BaseGraphicsLayer::m_lScreenSx-3;
		
		pVertex[0].v.y=(float)BaseGraphicsLayer::m_lScreenSy-3;
		pVertex[2].v.y=(float)BaseGraphicsLayer::m_lScreenSy-3;
	}

	pVertex[0].Diffuse.c=pVertex[1].Diffuse.c=pVertex[2].Diffuse.c=pVertex[3].Diffuse.c
			=0xffaaaaaa;

	m_pd3dDevice->SetTexture(0,m_GlareCompositeTexture.GetTexture());
	//m_pd3dDevice->SetTexture(0,m_GlareTexture.GetTexture(2));
	m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(TLVertex));

	m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,D3DBLEND_ONE);
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,D3DBLEND_ONE);
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);	

	pVertex[0].Diffuse.c=pVertex[1].Diffuse.c=pVertex[2].Diffuse.c=pVertex[3].Diffuse.c
			=0xffffffff;

	m_pd3dDevice->SetTexture(0,m_SpecularGlareTexture.GetTexture());	
	//m_pd3dDevice->SetTexture(0,m_GlareTexture.GetTexture(2));
	m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(TLVertex));

	//m_SmoothGlareTexture.GenerateGlareTexture(m_pd3dDevice,m_SpecularGlareTexture.GetTexture(),);
	
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);	

	pVertex[0].Diffuse.c=pVertex[1].Diffuse.c=pVertex[2].Diffuse.c=pVertex[3].Diffuse.c
			=0xff555555;

	m_pd3dDevice->SetTexture(0,m_SmoothGlareTexture.GetTexture());	
	//m_pd3dDevice->SetTexture(0,m_GlareTexture.GetTexture(2));
	m_pd3dDevice->SetTexture(1,NULL);
	m_pd3dDevice->SetVertexShader(TLVERTEXFVF);
	m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(TLVertex));



	////////////////////////

	//m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(TLVertex));	


	m_pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,FALSE);	
	m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
}

void CSceneManager::RenderFullSceneAnti()
{
	if(!m_FullSceneAnti)
		return;

	m_pd3dDevice->SetRenderTarget(m_pTempRenderSurface,m_pTempRenderZBuffer);
	m_pTempRenderSurface->Release();
	m_pTempRenderZBuffer->Release();
	
	TLVertex pVertex[4];	

	pVertex[0].v.x=0.0f;
	pVertex[1].v.x=0.0f;
	pVertex[2].v.x=(float)BaseGraphicsLayer::m_lScreenSx;
	pVertex[3].v.x=(float)BaseGraphicsLayer::m_lScreenSx;

	pVertex[1].v.y=0.0f;
	pVertex[3].v.y=0.0f;
	pVertex[0].v.y=(float)BaseGraphicsLayer::m_lScreenSy;
	pVertex[2].v.y=(float)BaseGraphicsLayer::m_lScreenSy;

	pVertex[0].tu=0.0f;
	pVertex[1].tu=0.0f;

	pVertex[3].tu=1.0f;
	pVertex[2].tu=1.0f;

	pVertex[1].tv=0.0f;
	pVertex[3].tv=0.0f;

	pVertex[0].tv=1.0f;
	pVertex[2].tv=1.0f;

	for(int i=0;i<4;i++)
	{
		pVertex[i].w=0.1f;
		pVertex[i].v.z=0.1f;
		pVertex[i].Diffuse.c=0xffffffff;
		pVertex[i].Specular.c=0xffffffff;
	}

	m_pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,FALSE);

	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);    
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);		
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);

	m_pd3dDevice->SetTexture(1,NULL);
	m_pd3dDevice->SetVertexShader(TLVERTEXFVF);

	m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,D3DBLEND_ONE);
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,D3DBLEND_ONE);
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);	

	color VertexColor;

	for(int cTexture=0;cTexture<9;cTexture++)	
	{
		VertexColor.r=(unsigned char)(32.0f/(float)8.0f);		
		VertexColor.a=VertexColor.b=VertexColor.g=VertexColor.r;
		VertexColor.a=255;
		////////				
		pVertex[0].Diffuse.c=pVertex[1].Diffuse.c=pVertex[2].Diffuse.c=pVertex[3].Diffuse.c
			=VertexColor.c;
		////////
		m_pd3dDevice->SetTexture(0,m_FullSceneTexture.GetTexture());

		switch(cTexture)
		{
			case 0:
				pVertex[0].Diffuse.c=pVertex[1].Diffuse.c=pVertex[2].Diffuse.c=pVertex[3].Diffuse.c
					=0xffeeeeee;
				pVertex[0].v.x=0.0f;
				pVertex[1].v.x=0.0f;				
				pVertex[2].v.x=(float)BaseGraphicsLayer::m_lScreenSx;
				pVertex[3].v.x=(float)BaseGraphicsLayer::m_lScreenSx;
				pVertex[1].v.y=0.0f;
				pVertex[3].v.y=0.0f;
				pVertex[0].v.y=(float)BaseGraphicsLayer::m_lScreenSy;
				pVertex[2].v.y=(float)BaseGraphicsLayer::m_lScreenSy;
				break;
			case 1:
				pVertex[0].v.x=-1;
				pVertex[1].v.x=-1;
				pVertex[2].v.x=(float)BaseGraphicsLayer::m_lScreenSx-1;
				pVertex[3].v.x=(float)BaseGraphicsLayer::m_lScreenSx-1;
				pVertex[1].v.y=-1;
				pVertex[3].v.y=-1;
				pVertex[0].v.y=(float)BaseGraphicsLayer::m_lScreenSy-1;
				pVertex[2].v.y=(float)BaseGraphicsLayer::m_lScreenSy-1;
				break;
			case 2:
				pVertex[0].v.x=0;
				pVertex[1].v.x=0;
				pVertex[2].v.x=(float)BaseGraphicsLayer::m_lScreenSx;
				pVertex[3].v.x=(float)BaseGraphicsLayer::m_lScreenSx;
				pVertex[1].v.y=-1;
				pVertex[3].v.y=-1;
				pVertex[0].v.y=(float)BaseGraphicsLayer::m_lScreenSy-1;
				pVertex[2].v.y=(float)BaseGraphicsLayer::m_lScreenSy-1;
				break;				
			case 3:
				pVertex[0].v.x=1;
				pVertex[1].v.x=1;
				pVertex[2].v.x=(float)BaseGraphicsLayer::m_lScreenSx+1;
				pVertex[3].v.x=(float)BaseGraphicsLayer::m_lScreenSx+1;
				pVertex[1].v.y=-1;
				pVertex[3].v.y=-1;
				pVertex[0].v.y=(float)BaseGraphicsLayer::m_lScreenSy-1;
				pVertex[2].v.y=(float)BaseGraphicsLayer::m_lScreenSy-1;
				break;
				////////////
			case 4:
				pVertex[0].v.x=-1;
				pVertex[1].v.x=-1;
				pVertex[2].v.x=(float)BaseGraphicsLayer::m_lScreenSx-1;
				pVertex[3].v.x=(float)BaseGraphicsLayer::m_lScreenSx-1;
				pVertex[1].v.y=0;
				pVertex[3].v.y=0;
				pVertex[0].v.y=(float)BaseGraphicsLayer::m_lScreenSy;
				pVertex[2].v.y=(float)BaseGraphicsLayer::m_lScreenSy;
				break;
			case 5:
				pVertex[0].v.x=1;
				pVertex[1].v.x=1;
				pVertex[2].v.x=(float)BaseGraphicsLayer::m_lScreenSx+1;
				pVertex[3].v.x=(float)BaseGraphicsLayer::m_lScreenSx+1;
				pVertex[1].v.y=0;
				pVertex[3].v.y=0;
				pVertex[0].v.y=(float)BaseGraphicsLayer::m_lScreenSy;
				pVertex[2].v.y=(float)BaseGraphicsLayer::m_lScreenSy;
				break;
				///
			case 6:
				pVertex[0].v.x=-1;
				pVertex[1].v.x=-1;
				pVertex[2].v.x=(float)BaseGraphicsLayer::m_lScreenSx-1;
				pVertex[3].v.x=(float)BaseGraphicsLayer::m_lScreenSx-1;
				pVertex[1].v.y=1;
				pVertex[3].v.y=1;
				pVertex[0].v.y=(float)BaseGraphicsLayer::m_lScreenSy+1;
				pVertex[2].v.y=(float)BaseGraphicsLayer::m_lScreenSy+1;
				break;
			case 7:
				pVertex[0].v.x=0;
				pVertex[1].v.x=0;
				pVertex[2].v.x=(float)BaseGraphicsLayer::m_lScreenSx;
				pVertex[3].v.x=(float)BaseGraphicsLayer::m_lScreenSx; 
				pVertex[1].v.y=1;
				pVertex[3].v.y=1;
				pVertex[0].v.y=(float)BaseGraphicsLayer::m_lScreenSy+1;
				pVertex[2].v.y=(float)BaseGraphicsLayer::m_lScreenSy+1;
				break;
			case 8:
				pVertex[0].v.x=1;
				pVertex[1].v.x=1;
				pVertex[2].v.x=(float)BaseGraphicsLayer::m_lScreenSx+1;
				pVertex[3].v.x=(float)BaseGraphicsLayer::m_lScreenSx+1;
				pVertex[1].v.y=1;
				pVertex[3].v.y=1;
				pVertex[0].v.y=(float)BaseGraphicsLayer::m_lScreenSy+1;
				pVertex[2].v.y=(float)BaseGraphicsLayer::m_lScreenSy+1;
				break;

		}

		

		m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(TLVertex));	
	}
	m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	
}

void CSceneManager::RenderSpecularGlareTexture()
{
	m_pd3dDevice->EndScene();	

	LPDIRECT3DSURFACE8 m_pTempRenderSurface;		
	LPDIRECT3DSURFACE8 m_pTempRenderZBuffer;	
	m_pd3dDevice->GetRenderTarget(&m_pTempRenderSurface);
	m_pd3dDevice->GetDepthStencilSurface(&m_pTempRenderZBuffer);

	m_pd3dDevice->SetRenderTarget(m_SpecularGlareTexture.GetSurface(),m_pTempRenderZBuffer);	
	

	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,FALSE);
	m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,FALSE);

	m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);

	////////////////////////////////////////////
	m_pd3dDevice->BeginScene();
	m_pd3dDevice->Clear(0,NULL,D3DCLEAR_TARGET,0xff000000,1.0f,0);
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,D3DTA_DIFFUSE);
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,D3DTOP_SELECTARG2);

	TLVertex pVertex[8];		
	float fTextureSize=128.0f;

	pVertex[0].v.x=0.0f;
	pVertex[1].v.x=0.0f;
	pVertex[2].v.x=fTextureSize;
	pVertex[3].v.x=fTextureSize;

	pVertex[1].v.y=0.0f;
	pVertex[3].v.y=0.0f;
	pVertex[0].v.y=fTextureSize;
	pVertex[2].v.y=fTextureSize;	
		
	pVertex[0].tu=0.0f;
	pVertex[1].tu=0.0f;

	pVertex[3].tu=1.0f;
	pVertex[2].tu=1.0f;

	pVertex[1].tv=0.0f;
	pVertex[3].tv=0.0f;

	pVertex[0].tv=1.0f;
	pVertex[2].tv=1.0f;		

	for(int i=0;i<4;i++)
	{
		pVertex[i].w=0.1f;
		pVertex[i].v.z=0.1f;
		pVertex[i].Diffuse.c=0xff0000ff;
		pVertex[i].Specular.c=0xffffffff;
	}	

	m_pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);	

	m_pd3dDevice->SetTexture(1,NULL);
	m_pd3dDevice->SetVertexShader(TLVERTEXFVF);				
	//m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,pVertex,sizeof(TLVertex));	

	//m_CharacterManager.Render(m_pd3dDevice);
	

	//m_pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);			
		
	m_pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);	

	m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	
	//m_pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);	


	//m_WeatherManager.Render(m_pd3dDevice);

	m_pd3dDevice->EndScene();
	/////////////////////////////////////////////////
	m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	m_pd3dDevice->SetRenderTarget(m_pTempRenderSurface,m_pTempRenderZBuffer);

	m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

	m_pTempRenderSurface->Release();
	m_pTempRenderZBuffer->Release();


	m_pd3dDevice->BeginScene();
}*/

vector3 CSceneManager::PickMousePos(int nMx, int nMy)
{
	//이거 졸라 비싼 연산이다.. 유의하시길..!!!!!!!!!!!!!!

	PickMakeRay(nMx,nMy,BaseGraphicsLayer::m_lScreenSx,BaseGraphicsLayer::m_lScreenSy);

	CSectorScene *pSector;
	
	SectorVertex *pDrawVertex;
	WORD *pDrawIndices;

	vector3 vecStart,vecEnd;
	vector3 vecDirection;

	vecStart=m_vecPickRayStart;
	vecDirection=m_vecPickRayDir;
	vecDirection.Normalize();	
	vecEnd=vecStart+vecDirection*100000.0f;	
	vecStart=vecStart+vecDirection*100.0f;
	vector3 vecTargetPosition;

	float fInterval,fMinInterval=100000.0f;		

	vector3 vecPoly[3];

	vecTargetPosition=vector3(-1000000000.0f,-100000000000.0f,-100000000.0f);

	vector3 vecMaxBox,vecMinBox;
	
	for(int ix=0;ix<m_HeightField.GetLSizeX();ix++)
	{
		for(int iy=0;iy<m_HeightField.GetLSizeY();iy++)
		{
			vecMinBox.x=m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_AccumulateTM._41;
			vecMaxBox.x=vecMinBox.x+SECTORSIZE;

			vecMinBox.y=m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_vecMinBox.y;
			vecMaxBox.y=m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_vecMaxBox.y;	

			vecMinBox.z=m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_AccumulateTM._43;
			vecMaxBox.z=vecMinBox.z+SECTORSIZE;
			//if(m_SectorScene[ix+iy*m_LSizeX].isCulling())
			if( CIntersection::PlaneAABBBox(CSceneManager::GetCamera()->m_vecFrustumTop,-CSceneManager::GetCamera()->m_vecFrustumNormal[0],vecMaxBox,vecMinBox) < 0.0f &&
				CIntersection::PlaneAABBBox(CSceneManager::GetCamera()->m_vecFrustumBottom,-CSceneManager::GetCamera()->m_vecFrustumNormal[1],vecMaxBox,vecMinBox) < 0.0f &&
				CIntersection::PlaneAABBBox(CSceneManager::GetCamera()->m_vecFrustumLeft,-CSceneManager::GetCamera()->m_vecFrustumNormal[2],vecMaxBox,vecMinBox) < 0.0f &&
				CIntersection::PlaneAABBBox(CSceneManager::GetCamera()->m_vecFrustumRight,-CSceneManager::GetCamera()->m_vecFrustumNormal[3],vecMaxBox,vecMinBox) < 0.0f)
			{	
				
			}
			else
			{
				continue;
			}

			pSector=&m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()];
			pSector->m_pROAMVertex;
			pSector->m_pROAMIndices;

			//m_UsedVertex,m_UsedIndices/3;

			pSector->m_pROAMVertex->Lock(0,0,(BYTE**)&pDrawVertex,0);
			pSector->m_pROAMIndices->Lock(0,(SECTORSX-1)*(SECTORSY-1)*ROAMBUFFERSIZE*sizeof(WORD),(BYTE**)&pDrawIndices,0);
			
			for(int cPoly=0;cPoly<pSector->m_UsedIndices/3;cPoly++)
			{
				vecPoly[0]=pDrawVertex[pDrawIndices[cPoly*3+0]].v+pSector->m_AccumulateTM.GetLoc();
				vecPoly[1]=pDrawVertex[pDrawIndices[cPoly*3+1]].v+pSector->m_AccumulateTM.GetLoc();
				vecPoly[2]=pDrawVertex[pDrawIndices[cPoly*3+2]].v+pSector->m_AccumulateTM.GetLoc();

				if(CIntersection::PolygonRay(vecStart,vecEnd,vecPoly,fInterval))
				{
					if(fInterval <= fMinInterval)
					{
						fMinInterval=fInterval;
						vecTargetPosition=vecStart+vecDirection*fMinInterval;
					}
				}
			}
			pSector->m_pROAMVertex->Unlock();
			pSector->m_pROAMIndices->Unlock();
		}
	}

	///////////////////////////////
	//Picking In Building 
	///////////////////////////////
	std::vector<vector3> vecPolyList;
	for(int ix=0;ix<m_HeightField.GetLSizeX();ix++)
	{
		for(int iy=0;iy<m_HeightField.GetLSizeY();iy++)
		{
			vecMinBox.x=m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_AccumulateTM._41;
			vecMaxBox.x=vecMinBox.x+SECTORSIZE;

			vecMinBox.y=m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_vecMinBox.y;
			vecMaxBox.y=m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_vecMaxBox.y;	

			vecMinBox.z=m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].m_AccumulateTM._43;
			vecMaxBox.z=vecMinBox.z+SECTORSIZE;
			//if(m_SectorScene[ix+iy*m_LSizeX].isCulling())
			if( CIntersection::PlaneAABBBox(CSceneManager::GetCamera()->m_vecFrustumTop,-CSceneManager::GetCamera()->m_vecFrustumNormal[0],vecMaxBox,vecMinBox) < 0.0f &&
				CIntersection::PlaneAABBBox(CSceneManager::GetCamera()->m_vecFrustumBottom,-CSceneManager::GetCamera()->m_vecFrustumNormal[1],vecMaxBox,vecMinBox) < 0.0f &&
				CIntersection::PlaneAABBBox(CSceneManager::GetCamera()->m_vecFrustumLeft,-CSceneManager::GetCamera()->m_vecFrustumNormal[2],vecMaxBox,vecMinBox) < 0.0f &&
				CIntersection::PlaneAABBBox(CSceneManager::GetCamera()->m_vecFrustumRight,-CSceneManager::GetCamera()->m_vecFrustumNormal[3],vecMaxBox,vecMinBox) < 0.0f)
			{	
				
			}
			else
			{
				continue;
			}
			
			//GetPickHousePoly(vector3 vecStart,vector3 vecEnd,List<vector3> &vecPolyList)
			m_HeightField.m_SectorScene[ix+iy*m_HeightField.GetLSizeX()].GetPickHousePoly(vecStart,vecEnd,vecPolyList);
			//GetInHouseShadowPoly(vecEnd,vecPolyList,vecStart);

		}
	}
	if(m_CharacterManager.m_CharacterList.empty())
		return vecTargetPosition;

	vector3 vecChrPos;

	m_CharacterManager.m_CharacterList[0].m_pChrmodel->GetPosition(vecChrPos);

	vecChrPos.y;

	vector3 vecCheckPoly[3];
	float fInterLens;
	float fMaxInterLens=1000000000.0f;
	vecDirection=vecEnd-vecStart;
	vecDirection.Normalize();
	vector3 vecPlaneNormal;

	

	
	
	for(int cPoly=0;cPoly<(int)vecPolyList.size();cPoly+=3)
	{
		vecCheckPoly[0]=vecPolyList[cPoly+0];
		vecCheckPoly[1]=vecPolyList[cPoly+1];
		vecCheckPoly[2]=vecPolyList[cPoly+2];

		vecPlaneNormal=(vecCheckPoly[0]-vecCheckPoly[1])^(vecCheckPoly[2]-vecCheckPoly[1]);
		vecPlaneNormal.Normalize();

		if(fabsf(vecPlaneNormal.y) >= fabsf(vecPlaneNormal.x) && fabsf(vecPlaneNormal.y) >= fabsf(vecPlaneNormal.z))
		{
			if(CIntersection::PolygonRay(vecStart,vecEnd,vecCheckPoly,fInterLens))
			{
				vector3 vecTarPos=vecStart+fInterLens*vecDirection;				

				if(fabsf(vecChrPos.y-vecTarPos.y) < fMaxInterLens)
				{				
					vecTargetPosition=vecStart+fInterLens*vecDirection;				
					fMaxInterLens=fabsf(vecChrPos.y-vecTarPos.y);
				}
			}
		}
	}
	return vecTargetPosition;
}

CollisionType CSceneManager::GetCharacterCollisionType(CZ3DGeneralChrModel *pChrmodel)
{
	return m_CharacterManager.GetCharacterCollisionType(pChrmodel);
}

void CSceneManager::BaseGraphicsDataLoad(char *strZoneName, char* szWeatherTableOverride)
{
	// 2005.05.10 yundi Octree에 대한 Lock Load 추가(존 로딩시)
	for( int i = 0; i <  60; ++i )
	{
		_LockImmediateResourceLoad(CROSSM::RESOURCE_OCTREESCENE);
		m_CollisionDetection.LockDetection();
	}

	m_bCharSelect = false;
	if(strcmp(strZoneName, "Zone15") == 0)
	{
		// 캐릭터 선택화면이다.
		m_bCharSelect = true;
//		CAmbienceManager::_GetInstance()->AllStop(); 
//		CBgmManager::_GetInstance()->AllStop(); 
	}


	if(strcmp(strZoneName,"")==0)
	{
		if(CGrassScene::m_NormalTexture==NULL)
		{
			CTexture::SetPath(NATURETEXTUREPATH);
			CGrassScene::m_NormalTexture=new CTexture();
			CGrassScene::m_NormalTexture->Load("grass.dds");			
		}

		CTexture::SetPath(TERRAINTEXTUREPATH);
		/*
		m_TerrainTexture[0].Load("hig0.dds");
		m_TerrainTexture[1].Load("hig1.dds");
		m_TerrainTexture[2].Load("hig2.dds");
		m_TerrainTexture[3].Load("hig3.dds");	

		m_TerrainTexture[4].Load("med0.dds");		
		m_TerrainTexture[5].Load("med1.dds");
		m_TerrainTexture[6].Load("med2.dds");
		m_TerrainTexture[7].Load("med3.dds");

		m_TerrainTexture[8].Load("low0.dds");
		m_TerrainTexture[9].Load("low1.dds");
		m_TerrainTexture[10].Load("low2.dds");
		m_TerrainTexture[11].Load("low3.dds");
		*/
		CSectorScene::m_DetailTexture.Load("detail.dds");	
		
		if(CRenderOption::m_RangeGrassRender)
		{		
			CTexture::SetPath(NATURETEXTUREPATH);

			CSectorScene::m_LODGrassTexture.Load("LODGrass.dds");

			CSectorScene::m_GrassRangeTexture=new CTexture();
			CSectorScene::m_GrassRangeTexture->Load("GrassRange.dds");
		}

		CTexture::SetPath(NATURETEXTUREPATH);
		CSkyScene::m_CloudTexture.Load("Clouds.dds");	

		if(CTreeScene::m_NormalTreeMesh[0]==NULL)
		{			
			CTexture::SetPath(OBJECTTEXTUREPATH);
			CSceneManager::m_MeshObjectContainer.SetPath(TREEOBJECTPATH);			

			char strName[256];
			for(int i=0;i<MAX_TREEKIND;i++)
			{
				sprintf(strName,"NormalTree%d.R3S",i+1);
				CTreeScene::m_NormalTreeMesh[i]=CSceneManager::m_MeshObjectContainer.GetMeshObject(strName);		
				if(CRenderOption::m_TreeAnimation)
					CTreeScene::m_NormalTreeMesh[i]->ConvertTree();	
				else
					CTreeScene::m_NormalTreeMesh[i]->ConvertNormal();
				if(i+1<10)
					sprintf(strName,"tree0%d_lod_fr.R3S",i+1);
				else
					sprintf(strName,"tree%d_lod_fr.R3S",i+1);				

				CTreeScene::m_NormalTreeMeshLOD[i]=CSceneManager::m_MeshObjectContainer.GetMeshObject(strName);
				if(CRenderOption::m_TreeAnimation)
					CTreeScene::m_NormalTreeMeshLOD[i]->ConvertTree();
				else
					CTreeScene::m_NormalTreeMeshLOD[i]->ConvertNormal();				
			}
			/*
			CTreeScene::m_NormalTreeLeaves[0]=CSceneManager::m_MeshObjectContainer.GetMeshObject("leaves.R3S");
			CTreeScene::m_NormalTreeLeaves[0]->ConvertTree();
			CTreeScene::m_NormalTreeTrunk[0]=CSceneManager::m_MeshObjectContainer.GetMeshObject("trunk.R3S");		
			CTreeScene::m_NormalTreeTrunk[0]->ConvertTree();
			*/

		}		
		m_WeatherManager.Load();
		///////		
		char strTableName[256] = {0,};
		char strBuffer[256] = {0,};
		char *strPtr = NULL;
		strcpy(strTableName,WEATHERPATH);
		strPtr = strrchr(strTableName,'.');
		strPtr++;
		(*strPtr) = 't';
		strPtr++;
		(*strPtr) = 'a';
		strPtr++;
		(*strPtr) = 'b';
		strPtr++;
		(*strPtr) = 'l';
		strPtr++;
		(*strPtr) = 'e';
		strPtr++;
		(*strPtr) = 0;
		
		FILE *fp = fopen(strTableName,"rt");
		if(fp)
		{
			
			int *iPtr;
			int iPtrNum;
			char *str;
			fgets(strBuffer,256,fp);
			iPtrNum = atoi(strBuffer);
			iPtr = new int[iPtrNum];
			memset(strBuffer,0,sizeof(char) * 256);
			fgets(strBuffer,256,fp);
			int iNum;
		
			for(iNum = 0; iNum < iPtrNum; iNum++)
			{
				if(iNum == 0)
					str = strtok(strBuffer," \n");
				else
					str = strtok(NULL," \n");
				iPtr[iNum] = atoi(str);
			}
			
			fclose(fp);

			m_WeatherManager.LoadSubTable(iPtr,iPtrNum);
			delete[] iPtr;
	
		}
		else 
		{
			m_WeatherManager.DeleteSubTable();
		}
		//////

	}
	else
	{
		// 03.03.04 존 파일에 따라 날씨와 culling 다르게 해준다 (Zone5)
		// Init Zone Value
		char strSetPath[256] = {0};

		m_ZoneInfoData.m_bZoneInfo = false;
		memset(m_ZoneInfoData.m_fZoneInfo,0,sizeof(float) * 30);
		memset(m_ZoneInfoData.m_iZoneInfo,0,sizeof(int) * 30);

		if(CRenderOption::LoadZoneInfo(&m_ZoneInfoData))
		{
			// Int 0 : weather time
			//	   1 : House Cull
			//	   2 : Object Cull
			//     3 : Effect Cull
			//	   4 : Path Setting

			// float 0 1 2 ->map min size
			//		 3 4 5 ->map max size
			//		 6 7 ->map Y Bound		
			SetWeatherTime(m_ZoneInfoData.m_iZoneInfo[0]);
			m_HeightField.SetHouseCulling((m_ZoneInfoData.m_iZoneInfo[1]) ? true : false);
			m_HeightField.SetObjectCulling((m_ZoneInfoData.m_iZoneInfo[2]) ? true : false);
			m_HeightField.SetEffectCulling((m_ZoneInfoData.m_iZoneInfo[3]) ? true : false);
			// Map Size Setting
			SetMapSize(vector3(m_ZoneInfoData.m_fZoneInfo[0],m_ZoneInfoData.m_fZoneInfo[1],m_ZoneInfoData.m_fZoneInfo[2]),
						vector3(m_ZoneInfoData.m_fZoneInfo[3],m_ZoneInfoData.m_fZoneInfo[4],m_ZoneInfoData.m_fZoneInfo[5]));
			// Zone Y Bound
			m_CollisionDetection.SetZoneBound(m_ZoneInfoData.m_fZoneInfo[6],m_ZoneInfoData.m_fZoneInfo[7]);
			
			if(m_ZoneInfoData.m_iZoneInfo[4] != 0) {
				sprintf(strSetPath,"Zone%d",m_ZoneInfoData.m_iZoneInfo[4]);
			}
		}
		else {
			m_HeightField.SetHouseCulling(true);
			m_HeightField.SetObjectCulling(true);
			m_HeightField.SetEffectCulling(true);
			m_CollisionDetection.SetZoneBound(-123.0f,-123.0f);
		}
		if(strlen(strSetPath) > 1) { // 존 path Setting
			if( szWeatherTableOverride )
			{
				sprintf(WEATHERPATH,"%s\\WeatherColorTable_%s.dat",m_SceneManagerPath,szWeatherTableOverride);		
			}
			else
			{
				sprintf(WEATHERPATH,"%s\\WeatherColorTable_%s.dat",m_SceneManagerPath,strZoneName);		
			}
			sprintf(TREEOBJECTPATH,"%s\\Objects\\NatureObject\\%s\\",m_SceneManagerPath,strSetPath);		
			sprintf(MAPDATAPATH,"%s\\MapData\\%s",m_SceneManagerPath,strZoneName);		
			if(CRenderOption::m_TextureMethod)
			{		
				sprintf(NATURETEXTUREPATH,"%s\\Texture\\NatureObject\\%s",m_SceneManagerPath,strSetPath);
				sprintf(WIDETEXTUREPATH,"%s\\Texture\\WideTexture\\%s",m_SceneManagerPath,strZoneName);
				sprintf(TERRAINTEXTUREPATH,"%s\\Texture\\Terrain\\%s",m_SceneManagerPath,strSetPath);
			}
			else
			{
				sprintf(NATURETEXTUREPATH,"%s\\LowTexture\\NatureObject\\%s",m_SceneManagerPath,strSetPath);
				sprintf(WIDETEXTUREPATH,"%s\\LowTexture\\WideTexture\\%s",m_SceneManagerPath,strZoneName);
				sprintf(TERRAINTEXTUREPATH,"%s\\LowTexture\\Terrain\\%s",m_SceneManagerPath,strSetPath);
			}		

		}
		else {
			if( szWeatherTableOverride )
			{
				sprintf(WEATHERPATH,"%s\\WeatherColorTable_%s.dat",m_SceneManagerPath,szWeatherTableOverride);		
			}
			else
			{
				sprintf(WEATHERPATH,"%s\\WeatherColorTable_%s.dat",m_SceneManagerPath,strZoneName);		
			}
			sprintf(TREEOBJECTPATH,"%s\\Objects\\NatureObject\\%s\\",m_SceneManagerPath,strZoneName);		
			sprintf(MAPDATAPATH,"%s\\MapData\\%s",m_SceneManagerPath,strZoneName);		
			
			if(strstr(strZoneName,"Zone8") != NULL)	// Zone 8 Setting
				MAX_OBJECTVIEWRANBE *= 2.0f;
			
			if(CRenderOption::m_TextureMethod)
			{		
				sprintf(NATURETEXTUREPATH,"%s\\Texture\\NatureObject\\%s",m_SceneManagerPath,strZoneName);
				sprintf(WIDETEXTUREPATH,"%s\\Texture\\WideTexture\\%s",m_SceneManagerPath,strZoneName);
				sprintf(TERRAINTEXTUREPATH,"%s\\Texture\\Terrain\\%s",m_SceneManagerPath,strZoneName);
			}
			else
			{
				sprintf(NATURETEXTUREPATH,"%s\\LowTexture\\NatureObject\\%s",m_SceneManagerPath,strZoneName);
				sprintf(WIDETEXTUREPATH,"%s\\LowTexture\\WideTexture\\%s",m_SceneManagerPath,strZoneName);
				sprintf(TERRAINTEXTUREPATH,"%s\\LowTexture\\Terrain\\%s",m_SceneManagerPath,strZoneName);
			}		
		}

		m_WeatherManager.Load();		

		///////		
		char strTableName[256] = {0,};
		char strBuffer[256] = {0,};
		char *strPtr = NULL;
		strcpy(strTableName,WEATHERPATH);
		strPtr = strrchr(strTableName,'.');
		strPtr++;
		(*strPtr) = 't';
		strPtr++;
		(*strPtr) = 'a';
		strPtr++;
		(*strPtr) = 'b';
		strPtr++;
		(*strPtr) = 'l';
		strPtr++;
		(*strPtr) = 'e';
		strPtr++;
		(*strPtr) = 0;
		FILE *fp = fopen(strTableName,"rt");
		if(fp)
		{
			
			int *iPtr;
			int iPtrNum;
			char *str;
			fgets(strBuffer,256,fp);
			iPtrNum = atoi(strBuffer);
			iPtr = new int[iPtrNum];
			memset(strBuffer,0,sizeof(char) * 256);
			fgets(strBuffer,256,fp);
			int iNum;
		
			for(iNum = 0; iNum < iPtrNum; iNum++)
			{
				if(iNum == 0)
					str = strtok(strBuffer," \n");
				else
					str = strtok(NULL," \n");
				iPtr[iNum] = atoi(str);
			}
			
			fclose(fp);

			m_WeatherManager.LoadSubTable(iPtr,iPtrNum);
			delete[] iPtr;

			
		}
		else 
		{
			m_WeatherManager.DeleteSubTable();
		}
		//////

		CTexture::SetPath(TERRAINTEXTUREPATH);
		/*
		m_TerrainTexture[0].Load("hig0.dds");
		m_TerrainTexture[1].Load("hig1.dds");
		m_TerrainTexture[2].Load("hig2.dds");
		m_TerrainTexture[3].Load("hig3.dds");	

		m_TerrainTexture[4].Load("med0.dds");		
		m_TerrainTexture[5].Load("med1.dds");
		m_TerrainTexture[6].Load("med2.dds");
		m_TerrainTexture[7].Load("med3.dds");

		m_TerrainTexture[8].Load("low0.dds");
		m_TerrainTexture[9].Load("low1.dds");
		m_TerrainTexture[10].Load("low2.dds");
		m_TerrainTexture[11].Load("low3.dds");
		*/
		CSectorScene::m_DetailTexture.Load("detail.dds");	

		if(CRenderOption::m_GrassRendering)
		{
			if(CGrassScene::m_NormalTexture==NULL)
			{
				CTexture::SetPath(NATURETEXTUREPATH);
				CGrassScene::m_NormalTexture=new CTexture();
				CGrassScene::m_NormalTexture->Load("grass.dds");			
			}
		}
		
		if(CRenderOption::m_RangeGrassRender)
		{		
			CTexture::SetPath(NATURETEXTUREPATH);

			CSectorScene::m_LODGrassTexture.Load("LODGrass.dds");

			CSectorScene::m_GrassRangeTexture=new CTexture();
			CSectorScene::m_GrassRangeTexture->Load("GrassRange.dds");
		}

		CTexture::SetPath(NATURETEXTUREPATH);
		CSkyScene::m_CloudTexture.Load("Clouds.dds");	

		if(CTreeScene::m_NormalTreeMesh[0]==NULL)
		{			
			CTexture::SetPath(OBJECTTEXTUREPATH);
			CSceneManager::m_MeshObjectContainer.SetPath(TREEOBJECTPATH);			

			char strName[256];
			for(int i=0;i<MAX_TREEKIND;i++)
			{
				sprintf(strName,"NormalTree%d.R3S",i+1);
				CTreeScene::m_NormalTreeMesh[i]=CSceneManager::m_MeshObjectContainer.GetMeshObject(strName);		
				if(CRenderOption::m_TreeAnimation)
					CTreeScene::m_NormalTreeMesh[i]->ConvertTree();	
				else
					CTreeScene::m_NormalTreeMesh[i]->ConvertNormal();
				if(i+1<10)
					sprintf(strName,"tree0%d_lod_fr.R3S",i+1);
				else
					sprintf(strName,"tree%d_lod_fr.R3S",i+1);				

				CTreeScene::m_NormalTreeMeshLOD[i]=CSceneManager::m_MeshObjectContainer.GetMeshObject(strName);
				if(CRenderOption::m_TreeAnimation)
					CTreeScene::m_NormalTreeMeshLOD[i]->ConvertTree();		
				else
					CTreeScene::m_NormalTreeMeshLOD[i]->ConvertNormal();				
			}
		}

		/*
		CTreeScene::m_NormalTreeLeaves[0]=CSceneManager::m_MeshObjectContainer.GetMeshObject("leaves.R3S");
		CTreeScene::m_NormalTreeLeaves[0]->ConvertTree();
		CTreeScene::m_NormalTreeTrunk[0]=CSceneManager::m_MeshObjectContainer.GetMeshObject("trunk.R3S");		
		CTreeScene::m_NormalTreeTrunk[0]->ConvertTree();
		*/

		char strWaterTextureName[256];

		CTexture::SetPath(NATURETEXTUREPATH);
		
		for(int i=0;i<MAX_WATERTEXTURE;i++)
		{
			sprintf(strWaterTextureName,"water%4d.dds",i);
			int strLens=strlen(strWaterTextureName);
			for(int cStr=0;cStr<strLens;cStr++)
			{
				if(strWaterTextureName[cStr]==' ')
					strWaterTextureName[cStr]='0';
			}
			CWaterScene::m_WaterTexture[i].Load(strWaterTextureName);
		}

	}
}

void CSceneManager::BaseGraphicsDataUnload()
{
	CSectorScene::m_DetailTexture.Unload();	

	if(CRenderOption::m_GrassRendering)
	{
		delete CGrassScene::m_NormalTexture;
		CGrassScene::m_NormalTexture=NULL;	
	}	
	if(CTreeScene::m_NormalTreeMesh[0]!=NULL)
	{	
		char strName[256];
		for(int i=0;i<MAX_TREEKIND;i++)
		{
			sprintf(strName,"NormalTree%d.R3S",i+1);
			m_MeshObjectContainer.DeleteMeshObject(strName);		
			CTreeScene::m_NormalTreeMesh[i]=NULL;
			
			if(i+1<10)
				sprintf(strName,"tree0%d_lod_fr.R3S",i+1);
			else
				sprintf(strName,"tree%d_lod_fr.R3S",i+1);				

			m_MeshObjectContainer.DeleteMeshObject(strName);		

			CTreeScene::m_NormalTreeMeshLOD[i]=NULL;	
		}		
		
	}	
	


}

void CSceneManager::SetSceneManagerPath(char *strSceneManagerPath)
{
	strcpy(m_SceneManagerPath,strSceneManagerPath);
	sprintf(ROOTPATH,"%s\\",strSceneManagerPath);
	sprintf(CHARACTERDATAPATH,"%s\\Character\\Data\\",strSceneManagerPath);//char CHARACTERDATAPATH[256]="c:\\MP-Project\\Character\\Data\\";
	sprintf(CHARACTERTEXTUREDATAPATH,"%s\\Character\\Data\\tex\\",strSceneManagerPath);//char CHARACTERTEXTUREDATAPATH[256]="c:\\MP-Project\\Character\\Data\\tex\\";
	sprintf(CHARACTERMESHTEX,"%s\\Character\\Data\\meshtex.htable",strSceneManagerPath);//char CHARACTERMESHTEX[256]="c:\\MP-Project\\Character\\Data\\meshtex.htable";
	sprintf(CHARACTERANIPACK,"%s\\Character\\Data\\ani\\APack.ntable",strSceneManagerPath);//char CHARACTERANIPACK[256]="c:\\MP-Project\\Character\\Data\\ani\\APack.ntable";
	sprintf(CHARACTERLODMESH,"%s\\Character\\Data\\mesh\\LODMesh.ntable",strSceneManagerPath);//char CHARACTERLODMESH[256]="c:\\MP-Project\\Character\\Data\\mesh\\LODMesh.ntable";
	sprintf(CHARACTERTEXPIECE,"%s\\Character\\Data\\tex\\Texpiece.ntable",strSceneManagerPath);//char CHARACTERTEXPIECE[256]="c:\\MP-Project\\Character\\Data\\tex\\Texpiece.ntable";
	sprintf(CHARACTERATTACHMENTMESH,"%s\\Character\\Data\\Amesh\\amesh.ntable",strSceneManagerPath);//char CHARACTERATTACHMENTMESH[256]="c:\\MP-Project\\Character\\Data\\Amesh\\amesh.ntable";
	sprintf(CHARACTERTEXTURE,"%s\\Character\\Data\\Texture\\texture.ntable",strSceneManagerPath);//char CHARACTERTEXTURE[256]="c:\\MP-Project\\Character\\Data\\Texture\\texture.ntable";
	
	sprintf(HOUSEOBJECTPATH,"%s\\Objects\\House\\",strSceneManagerPath);//char HOUSEOBJECTPATH[256]="c:\\MP-Project\\Objects\\House\\";
	sprintf(LIGHTOBJECTPATH,"%s\\Objects\\Object\\",strSceneManagerPath);//char LIGHTOBJECTPATH[256]="c:\\MP-Project\\Objects\\Object\\";
	sprintf(TREEOBJECTPATH,"%s\\Objects\\NatureObject\\",strSceneManagerPath);//char TREEOBJECTPATH[256]="c:\\MP-Project\\Objects\\NatureObject\\";
	sprintf(OBJECTPATH,"%s\\Objects\\Object\\",strSceneManagerPath);//char OBJECTPATH[256]="c:\\MP-Project\\Objects\\Object\\";
	sprintf(INSTANCEOBJECTPATH,"%s\\Objects\\Instance\\",strSceneManagerPath);//ect\\Objects\Instance\\;

	sprintf(NEOINTERFACETEXTUREPATH,"%s\\Texture\\Interface\\NeoInterface",strSceneManagerPath);//char INTERFACETEXTUREPATH[256]="c:\\MP-Project\\Texture\\Interface";
	if(CRenderOption::m_TextureMethod==1)
	{
		sprintf(CHARACTERTEXTURE,"%s\\Character\\Data\\Texture\\texture.ntable",strSceneManagerPath);//char CHARACTERTEXTURE[256]="c:\\MP-Project\\Character\\Data\\Texture\\texture.ntable";
		sprintf(TERRAINTEXTUREPATH,"%s\\Texture\\Terrain",strSceneManagerPath);//char TERRAINTEXTUREPATH[256]="c:\\MP-project\\Texture\\Terrain";
		sprintf(OBJECTTEXTUREPATH,"%s\\Texture\\Object",strSceneManagerPath);//char OBJECTTEXTUREPATH[256]="c:\\MP-Project\\Texture\\Object";
		sprintf(NATURETEXTUREPATH,"%s\\Texture\\NatureObject",strSceneManagerPath);//char NATURETEXTUREPATH[256]="c:\\MP-project\\Texture\\NatureObject";
		sprintf(WIDETEXTUREPATH,"%s\\Texture\\WideTexture",strSceneManagerPath);//char WIDETEXTUREPATH[256]="C:\\MP-Project\\Texture\\WideTexture";	
		sprintf(INTERFACETEXTUREPATH,"%s\\Texture\\Interface",strSceneManagerPath);//char INTERFACETEXTUREPATH[256]="c:\\MP-Project\\Texture\\Interface";
		sprintf(FXTEXTUREPATH,"%s\\Texture\\FX",strSceneManagerPath);//char FXTEXTUREPATH[256]="c:\\MP-Project\\Texture\\FX";
		sprintf(LIGHTMAPTEXTUREPATH,"%s\\Texture\\Lightmap",strSceneManagerPath);//char LIGHTMAPTEXTUREPATH[256]="c:\\MP-Project\\Texture\\Lightmap";
		sprintf( EFFECTTEXTUREPATH, "%s\\Texture\\Effect\\",strSceneManagerPath );
	}
	else
	{
		sprintf(CHARACTERTEXTURE,"%s\\Character\\Data\\LowTexture\\texture.ntable",strSceneManagerPath);//char CHARACTERTEXTURE[256]="c:\\MP-Project\\Character\\Data\\Texture\\texture.ntable";
		sprintf(TERRAINTEXTUREPATH,"%s\\LowTexture\\Terrain",strSceneManagerPath);//char TERRAINTEXTUREPATH[256]="c:\\MP-project\\Texture\\Terrain";
		sprintf(OBJECTTEXTUREPATH,"%s\\LowTexture\\Object",strSceneManagerPath);//char OBJECTTEXTUREPATH[256]="c:\\MP-Project\\Texture\\Object";
		sprintf(NATURETEXTUREPATH,"%s\\LowTexture\\NatureObject",strSceneManagerPath);//char NATURETEXTUREPATH[256]="c:\\MP-project\\Texture\\NatureObject";
		sprintf(WIDETEXTUREPATH,"%s\\LowTexture\\WideTexture",strSceneManagerPath);//char WIDETEXTUREPATH[256]="C:\\MP-Project\\Texture\\WideTexture";	
		sprintf(INTERFACETEXTUREPATH,"%s\\LowTexture\\Interface",strSceneManagerPath);//char INTERFACETEXTUREPATH[256]="c:\\MP-Project\\Texture\\Interface";
		sprintf(FXTEXTUREPATH,"%s\\LowTexture\\FX",strSceneManagerPath);//char FXTEXTUREPATH[256]="c:\\MP-Project\\Texture\\FX";
		sprintf(LIGHTMAPTEXTUREPATH,"%s\\LowTexture\\Lightmap",strSceneManagerPath);//char LIGHTMAPTEXTUREPATH[256]="c:\\MP-Project\\Texture\\Lightmap";
		sprintf( EFFECTTEXTUREPATH, "%s\\LowTexture\\Effect\\",strSceneManagerPath );
	}
	sprintf(TERRAINVERTEXSHADEROATH,"%s\\VertexShader\\LayerFog\\LayerFogTerrain.vsh",strSceneManagerPath);//char TERRAINVERTEXSHADEROATH[256]="c:\\MP-Project\\VertexShader\\LayerFog\\LayerFogTerrain.vsh";
	sprintf(WEATHERPATH,"%s\\WeatherColorTable.dat",strSceneManagerPath);//char WEATHERPATH[256]="c:\\MP-Project\\WeatherColorTable.dat";
	sprintf(OCTREEPATH,"%s\\Octree",strSceneManagerPath);//char OCTREEPATH[256]="c:\\MP-Project\\Octree";
	sprintf(SOUNDFILEPATH,"%s\\Sound\\",strSceneManagerPath);//char SOUNDFILEPATH[256]="c:\\MP-Project\\Sound\\";

	sprintf( EFFECTPATH, "%s\\Effect\\", strSceneManagerPath );
	sprintf( EFFECTSOUNDSPATH,"%s\\Sound\\Effect\\Ssnd\\",strSceneManagerPath);
	sprintf( EFFECTSOUNDMPATH,"%s\\Sound\\Effect\\Msnd\\",strSceneManagerPath);

	// effect texture path
	sprintf( EFFECTSCRIPTPATH,"%s\\Effect\\Esf\\",strSceneManagerPath);
	
	//mesh morph
	sprintf( OBJECTGEMPATH, "%s\\Objects\\Gem\\",strSceneManagerPath );

	//Sound path 
	sprintf( BGMPATH, "%s\\SOUND\\Bgm\\",strSceneManagerPath );
	sprintf( AMBIENCEPATH, "%s\\SOUND\\Amb\\",strSceneManagerPath );
}

void CSceneManager::CharacterFirstAccelate(CZ3DGeneralChrModel *pChrmodel, bool bFirstAccelate)
{
	m_CharacterManager.CharacterFirstAccelate(pChrmodel,bFirstAccelate);
}

CZ3DGeneralChrModel* CSceneManager::PickingCharacter(int MouseX, int MouseY)
{
	PickMakeRay(MouseX,MouseY,BaseGraphicsLayer::m_lScreenSx,BaseGraphicsLayer::m_lScreenSy);

	vector3 vecChrPos;

	
	float fRad=50.0f;
	float fNearestInterval=100.0f;

	CZ3DGeneralChrModel *pResult=NULL;

	vector3 vecMousePos;
	vecMousePos.x=MouseX;
	vecMousePos.y=MouseY;
	vecMousePos.z=0.0f;

	vector3 vecLens;

	vector3 vecViewPos=*CSceneManager::GetCamera()->GetPosition();

	for(int cChr=0;cChr<(int)m_CharacterManager.m_CharacterList.size();cChr++)
	{
		m_CharacterManager.m_CharacterList[cChr].m_pChrmodel->GetPosition(vecChrPos);		
		vecChrPos.y+=100.0f;

		vector3 vecResult;
		float fW;
		if(BaseGraphicsLayer::TransformVector(vecChrPos,vecResult,fW))
		{			
			if(vecResult.z<0.0f || vecResult.z>1.0f)
				continue;

			vecLens=vecViewPos-vecChrPos;

			float fInterCamera=vecLens.GetLens();
			
			float fZRate=-(fInterCamera/10000.0f)+1;

			vecResult.z=0.0f;
			
			vecLens=vecMousePos-vecResult;

			float fTemp=vecLens.GetLens();

			if(fTemp <= fRad*fZRate)
			{
				pResult=m_CharacterManager.m_CharacterList[cChr].m_pChrmodel;
			}		
			
			/*
			float fTransRad=fRad/vecResult.z;
			
			float fTemp=vecResult.z;
			vecResult.z=0.0f;
			vecLens=vecMousePos-vecResult;

			if(vecLens.GetLens() <= fTransRad)
			{
  				pResult=m_CharacterManager.m_CharacterList[cChr].m_pChrmodel;
			}
			*/
			/*
			vecResult.z=0.0f;
			vecLens=vecMousePos-vecResult;
			if(vecLens.GetLens()<=fNearestInterval)
			{
				pResult=m_CharacterManager.m_CharacterList[cChr].m_pChrmodel;
				fNearestInterval=vecLens.GetLens();
			}
			*/
		}
		/*
		vecChrPos.y+=150.0f;
		vector3 vecInter=m_vecPickRayStart-vecChrPos;
		float fC=(vecInter*vecInter)-fRad;
		float fB=m_vecPickRayDir*(m_vecPickRayStart-vecChrPos);

		if(fB*fB-fC>=0.0f)
		{
			if(vecInter.GetLens()<=fNearestInterval)
			{
				pResult=m_CharacterManager.m_CharacterList[cChr].m_pChrmodel;

				fNearestInterval=vecInter.GetLens();
			}
		}
		*/
	}
	return pResult;
	/*

	

	vector3 vecInter=m_vecPickRayStart-vecObjectPos;

			float fC=(vecInter*vecInter)-fRad;
			float fB=m_vecPickRayDir*(m_vecPickRayStart-vecObjectPos);

			vecObjectPos=m_vecPickRayStart-vecObjectPos;
			
			if(fB*fB-fC>=0.0f)
			{
				if(vecObjectPos.GetLens() <= fNearestInterval) 
				{
					if(PickMode==2)
					{						
						m_pBspScene->m_HouseObject->m_pBspObject->m_SelectLight=cLight;
						fNearestInterval=vecObjectPos.GetLens();						
					}
				}
			}			
			*/

}

void CSceneManager::CharacterCollisionAble(CZ3DGeneralChrModel *pChrmodel, bool bCollisionDetectionAble)
{
	m_CharacterManager.CharacterCollisionAble(pChrmodel,bCollisionDetectionAble);
}

void CSceneManager::CharacterGravityAble(CZ3DGeneralChrModel *pChrmodel, bool bGravityAble)
{
	m_CharacterManager.CharacterGravityAble(pChrmodel,bGravityAble);
}

void CSceneManager::ResetTerrain()
{

}

void CSceneManager::SetMapSize(vector3 vecMinSize, vector3 vecMaxSize)
{	
	m_CollisionDetection.m_vecMinMove=vecMinSize;
	m_CollisionDetection.m_vecMaxMove=vecMaxSize;
}

void CSceneManager::CharacterCollisionDetectType(CZ3DGeneralChrModel *pChrmodel, int nType)
{
	m_CharacterManager.CharacterCollisionDetectType(pChrmodel,nType);
}

CCharacterLightShadowManager::CCharacterDataNode CSceneManager::GetCharacterDataNode(CZ3DGeneralChrModel *pChrmodel)
{
	return m_CharacterManager.GetCharacterDataNode(pChrmodel);
}

void CSceneManager::SetCharacterDataNode(CCharacterLightShadowManager::CCharacterDataNode SetNode)
{
	m_CharacterManager.SetCharacterDataNode(SetNode);
}
void CSceneManager::PickSectorEffect(int b) {			// 03.03.04 추가
	static int iPickSector = 0;							// Effect Sectormap select
	
	if(iPickSector < 0)									// b가 0 이면 인덱스증가 1이면 감소
		iPickSector = 0;

	m_PickEffect = m_MapStorage.FindEffectMap(iPickSector);
	if(!b)
		iPickSector++;
	else
		iPickSector--;


}
void CSceneManager::PickEffect2(int b) {				// 03.03.04 추가
	static int iPick = 0;								// m_PickEffect 안의 effect 들을 순회한다.
														// b가 0 이면 인덱스증가 1이면 감소 
	if(iPick >= (int)m_PickEffect->m_vecEffectPos.size())
		iPick = 0;
	
	else if(iPick < 0)
		iPick = 0;

	m_PickEffect->pick_index = iPick;
	
	if(!b)
		iPick++;
	else
		iPick--;

}
// RBSP
// Bsp Scene Move
void CSceneManager::PickBspMove(float fMx,float fMy,float fMz,float fAngle,float fAddy,int MoveMode) 
{
	if(m_RBspSceneManager.m_pCurrentBspScene != NULL) 
	{
		if(MoveMode == 0) {//  땅에서 이동
			float fx = fMx - m_RBspSceneManager.m_pCurrentBspScene->m_vecZeroPos.x;
			float fz = fMz - m_RBspSceneManager.m_pCurrentBspScene->m_vecZeroPos.z;

			m_RBspSceneManager.m_pCurrentBspScene->m_vecZeroPos.x = fMx;
			m_RBspSceneManager.m_pCurrentBspScene->m_vecZeroPos.z = fMz;
			m_RBspSceneManager.m_pCurrentBspScene->UpdateLeafBox(fx,0.0f,fz);

		}
		else if(MoveMode == 1) {// y 축 이동
			m_RBspSceneManager.m_pCurrentBspScene->m_vecZeroPos.y+=fAddy;
			m_RBspSceneManager.m_pCurrentBspScene->UpdateLeafBox(0.0f,fAddy,0.0f);
		}
		
/*		m_RBspSceneManager.m_pCurrentBspScene->SetZeroPosition(m_RBspSceneManager.m_pCurrentBspScene->m_vecZeroPos.x,
																m_RBspSceneManager.m_pCurrentBspScene->m_vecZeroPos.y,
																m_RBspSceneManager.m_pCurrentBspScene->m_vecZeroPos.z,true);
*/
	}
	else if(m_RBspSceneManager.m_iPickIndex != -1)
	{
		int iPick = m_RBspSceneManager.m_iPickIndex;

		if(MoveMode == 0) {//  땅에서 이동
			float fx = fMx - m_RBspSceneManager.m_lstBspScene[iPick]->m_vecZeroPos.x;
			float fz = fMz - m_RBspSceneManager.m_lstBspScene[iPick]->m_vecZeroPos.z;

			m_RBspSceneManager.m_lstBspScene[iPick]->m_vecZeroPos.x = fMx;
			m_RBspSceneManager.m_lstBspScene[iPick]->m_vecZeroPos.z = fMz;
			m_RBspSceneManager.m_lstBspScene[iPick]->UpdateLeafBox(fx,0.0f,fz);

		}
		else if(MoveMode == 1) {// y 축 이동
			m_RBspSceneManager.m_lstBspScene[iPick]->m_vecZeroPos.y += fAddy;
			m_RBspSceneManager.m_lstBspScene[iPick]->UpdateLeafBox(0.0f,fAddy,0.0f);
		}
/*		m_RBspSceneManager.m_lstBspScene[iPick]->SetZeroPosition(m_RBspSceneManager.m_lstBspScene[iPick]->m_vecZeroPos.x,
																m_RBspSceneManager.m_lstBspScene[iPick]->m_vecZeroPos.y,
																m_RBspSceneManager.m_lstBspScene[iPick]->m_vecZeroPos.z,true);
*/
	}
}

vector3 CSceneManager::GetPickBspCenter()
{
	if(m_RBspSceneManager.m_pCurrentBspScene != NULL) 
	{
		return (m_RBspSceneManager.m_pCurrentBspScene->m_vecZeroPos);
	}
	return vector3(0.0f,0.0f,0.0f);
}
void CSceneManager::UnPickRBspScene()
{
	m_PickObjectScene = NULL;

	if(m_RBspSceneManager.m_pCurrentBspScene != NULL)
	{
		
		RBspScene *pScene = m_RBspSceneManager.m_pCurrentBspScene;
		int i;
		for( i = 0; i < pScene->m_iLeafsNum; i++)
		{
			pScene->m_pLeafExs[i].UnPickEventEffect();
			pScene->m_pLeafExs[i].UnPickEventObject();
			pScene->m_pLeafExs[i].UnPickEventSound();
			pScene->m_pLeafExs[i].UnPickLight();
			pScene->m_pLeafExs[i].UnPickTrigger();
			pScene->m_pLeafExs[i].UnPickR3SObject();

		}
		
	}
}
void CSceneManager::PickRBspObject(float x,float y,float z,int iPickValue)
{
	if(m_RBspSceneManager.m_pCurrentBspScene != NULL)
	{
		D3DXVECTOR3 vecPos = D3DXVECTOR3(x,y,z);
		/*
		D3DXVECTOR3 vecLocal = vecPos;
		D3DXMATRIX matTmp;
		D3DXMATRIX matInv;

		D3DXMatrixIdentity(&matTmp);
		matTmp._41 = CSceneManager::m_RBspSceneManager.m_pCurrentBspScene->m_vecZeroPos.x;
		matTmp._42 = CSceneManager::m_RBspSceneManager.m_pCurrentBspScene->m_vecZeroPos.y;
		matTmp._43 = CSceneManager::m_RBspSceneManager.m_pCurrentBspScene->m_vecZeroPos.z;

		D3DXMatrixInverse(&matInv,NULL,&matTmp);
		D3DXMatrixIdentity(&matTmp);
		matTmp._41 = vecLocal.x;
		matTmp._42 = vecLocal.y;
		matTmp._43 = vecLocal.z;
		D3DXMatrixMultiply(&matTmp,&matTmp,&matInv);
		vecLocal.x = matTmp._41;
		vecLocal.y = matTmp._42;
		vecLocal.z = matTmp._43;						
	
		int iLeaf = m_RBspSceneManager.m_pCurrentBspScene->FindLeafNode(vecLocal);
		*/

		int iLeaf = m_RBspSceneManager.m_pCurrentBspScene->m_iRBspLeafIndex;
		int i;
		int iPickIndex = -1;
		float fPickLength = 0.0f;

		RBspScene::RBspLeafEx *pLeaf = &(m_RBspSceneManager.m_pCurrentBspScene->m_pLeafExs[iLeaf]);

		if(iPickValue == 0)				// Object
		{
			for( i= 0; i < pLeaf->m_iEventNum[0]; i++)
			{
				D3DXVECTOR3 vecTmp;
				float fTmp;
				vecTmp = (pLeaf->m_lstObject[i]->m_vecCenter) - vecPos;
				fTmp = D3DXVec3Length(&vecTmp);
				if( i == 0) 
				{
					fPickLength = fTmp;
					iPickIndex = i;
				}
				else if(fTmp < fPickLength)
				{
					fPickLength = fTmp;
					iPickIndex = i;
				}
			}
			pLeaf->m_pPickEventObject = pLeaf->m_lstObject[iPickIndex];
			pLeaf->m_pPickEventEffect = NULL;
			pLeaf->m_pPickEventSound = NULL;
			pLeaf->m_pPickLight = NULL;
			pLeaf->m_pPickTrigger = NULL;

		}
		else if(iPickValue == 1)		// Effect
		{
			for( i= 0; i < pLeaf->m_iEventNum[1]; i++)
			{
				D3DXVECTOR3 vecTmp;
				float fTmp;
				vecTmp = (pLeaf->m_lstEffect[i]->m_vecCenter) - vecPos;
				fTmp = D3DXVec3Length(&vecTmp);
				if( i == 0) 
				{
					fPickLength = fTmp;
					iPickIndex = i;
				}
				else if(fTmp < fPickLength)
				{
					fPickLength = fTmp;
					iPickIndex = i;
				}
			}
			pLeaf->m_pPickEventEffect = pLeaf->m_lstEffect[iPickIndex];
			pLeaf->m_pPickEventObject = NULL;
			pLeaf->m_pPickLight = NULL;
			pLeaf->m_pPickEventSound = NULL;
			pLeaf->m_pPickTrigger = NULL;

		}
		else if(iPickValue == 2)		// Light
		{
			for( i= 0; i < pLeaf->m_iLightNum; i++)
			{
				D3DXVECTOR3 vecTmp;
				float fTmp;
				vecTmp = (pLeaf->m_lstLight[i].m_vecLight) - vecPos;
				fTmp = D3DXVec3Length(&vecTmp);
				if( i == 0) 
				{
					fPickLength = fTmp;
					iPickIndex = i;
				}
				else if(fTmp < fPickLength)
				{
					fPickLength = fTmp;
					iPickIndex = i;
				}
			}
			pLeaf->m_pPickLight = &(pLeaf->m_lstLight[iPickIndex]);
			pLeaf->m_pPickEventEffect = NULL;
			pLeaf->m_pPickEventObject = NULL;
			pLeaf->m_pPickEventSound = NULL;
			pLeaf->m_pPickTrigger = NULL;

		}
	}
}
void CSceneManager::PickRBspObjectMove(float fMx,float fMy,float fMz,float fAngle,float fAddy,int iValue,int MoveMode) 
{
	if(m_RBspSceneManager.m_pCurrentBspScene != NULL) 
	{

		int iLeaf = m_RBspSceneManager.m_pCurrentBspScene->m_iRBspLeafIndex;
		RBspScene::RBspLeafEx *pLeaf = &(m_RBspSceneManager.m_pCurrentBspScene->m_pLeafExs[iLeaf]);
		
		if(iValue == 0)				// Object
		{
			RBspScene::RBspEventObject *pObject = pLeaf->m_pPickEventObject;
			
			if(pObject != NULL) {
				if(MoveMode == 0) {//  땅에서 이동
					pObject->m_vecCenter.x = fMx;
					pObject->m_vecCenter.z = fMz;

				}
				else if(MoveMode == 1) {// y 축 이동
					pObject->m_vecCenter.y += fAddy;
				}
			}
		}
		else if(iValue == 1)				// Effect
		{
			RBspScene::RBspEventEffect *pEffect = pLeaf->m_pPickEventEffect;
			
			if(pEffect != NULL) {
				if(MoveMode == 0) {//  땅에서 이동
					pEffect->m_vecCenter.x = fMx;
					pEffect->m_vecCenter.z = fMz;

				}
				else if(MoveMode == 1) {// y 축 이동
					pEffect->m_vecCenter.y += fAddy;
				}
			}
		}
		else if(iValue == 2)				// Light
		{
			RBspScene::RBspLight *pLight = pLeaf->m_pPickLight;
			
			if(pLight != NULL) {
				if(MoveMode == 0) {//  땅에서 이동
					pLight->m_vecLight.x = fMx;
					pLight->m_vecLight.z = fMz;

				}
				else if(MoveMode == 1) {// y 축 이동
					pLight->m_vecLight.y += fAddy;
				}
			}
		}
	}
}
void CSceneManager::HeightPosInit() {
	m_HeightField.InitNowPos();
}
void CSceneManager::RenderInCubemap()
{
	if(CRenderOption::m_WaterBumpEnvRendering)
	{
		D3DXMATRIX matOldProj,matOldView,matOldWorld;

		D3DXMATRIX matProj;
		D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI * 0.5f, 1.0f, 50.0f,620000.0f);

		D3DVIEWPORT8 Viewport;
		D3DVIEWPORT8 CubeViewport;
		   
		CubeViewport.X = 0;
		CubeViewport.Y = 0;
		CubeViewport.Width = 256;
		CubeViewport.Height = 256;
		CubeViewport.MinZ = 0.0f;
		CubeViewport.MaxZ = 1.0f;
		m_pd3dDevice->GetViewport(&Viewport);

		m_pd3dDevice->GetTransform(D3DTS_VIEW,&matOldView);
		m_pd3dDevice->GetTransform(D3DTS_PROJECTION,&matOldProj);
		m_pd3dDevice->GetTransform(D3DTS_WORLD,&matOldWorld);

		D3DXMATRIX matTmp;
		vector3 vecCameraPos;

		D3DXMatrixInverse(&matTmp,NULL,&matOldView);
		vecCameraPos.x = matTmp._41;
		vecCameraPos.y = matTmp._42;
		vecCameraPos.z = matTmp._43;

		//// Get ViewDir
		D3DXMATRIX matT,matResult;
		D3DXVECTOR3 vecT(0.0f, 0.0f, 1.0f);	
		D3DXMatrixTranslation(&matT, vecT.x, vecT.y, vecT.z);	
		
		D3DXMatrixMultiply(&matResult, &matT, &matOldView);	
		//vector3 *vecViewPos=CSceneManager::GetCamera()->GetPosition();
		vector3 vecToward = CSceneManager::GetCamera()->GetViewTowardVector();
		vecToward.Normalize();
		
		////
		D3DXMATRIX matViewDir( matOldView );
		matViewDir._41 = 0.0f; matViewDir._42 = 0.0f; matViewDir._43 = 0.0f;

		m_pRenderEnvMap->BeginCube( m_pWorldCubemap );
		m_pd3dDevice->SetViewport(&CubeViewport);
		for( UINT i = 0; i < 6; i++ )
		{
			m_pRenderEnvMap->Face( (D3DCUBEMAP_FACES) i );

			// Set the view transform for this cubemap surface
			D3DXMATRIX matView;
			D3DXMATRIX matWorld;
			D3DXMatrixIdentity(&matWorld);
			vector3 vecDir;

			matView = GetCubeMatrix(  i ,vecDir);
			
			//D3DXMatrixMultiply( &matView, &matViewDir, &matView );
	
			D3DXMatrixInverse(&matView,NULL,&matView);
			//vector3 *vecViewPos=CSceneManager::GetCamera()->GetPosition();
			matView._41 = vecCameraPos.x;
			matView._42 = vecCameraPos.y;
			//matView._42 = 600.0f;
			matView._43 = vecCameraPos.z;
			D3DXMatrixInverse(&matView,NULL,&matView);
			
			m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET, BaseGraphicsLayer::m_ClearColor.c, 1.0f, 0 );
			m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
			m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
			m_pd3dDevice->SetTransform( D3DTS_WORLD,&matWorld);
			
			if(i == 3)	// Negative Y
				continue;
			else if(i != 2)	// Positive Y always Render
			{	// View Direction Check
				float fDot = vecDir * vecToward;
				if(fDot < 0)
					continue;

			}
		
			
			m_WeatherManager.RenderEnv(m_pd3dDevice);
			//m_WeatherManager.Render(m_pd3dDevice);
			m_HeightField.RenderEnv(m_pd3dDevice);
		//	m_HeightField.Render(m_pd3dDevice,true);
		//	m_WeatherManager.SecondRender(m_pd3dDevice);


		}

		m_pRenderEnvMap->End();
		
	//	HRESULT hr = D3DXSaveTextureToFile("c:/testEnv.dds", D3DXIFF_DDS ,m_pWorldCubemap,NULL);

		m_pd3dDevice->SetTransform( D3DTS_VIEW, &matOldView );
		m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matOldProj );
		m_pd3dDevice->SetTransform(D3DTS_WORLD,&matOldWorld);
		m_pd3dDevice->SetViewport(&Viewport);
	}


}
D3DXMATRIX CSceneManager::GetCubeMatrix(int i,vector3 &vecDir)
{
	D3DXVECTOR3 vEyePt   = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vLookDir;
    D3DXVECTOR3 vUpDir;

    switch( i )
    {
        case 0:
            vLookDir = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            break;
        case 1:
            vLookDir = D3DXVECTOR3(-1.0f, 0.0f, 0.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            break;
        case 2:
            vLookDir = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 0.0f,-1.0f );
            break;
        case 3:
            vLookDir = D3DXVECTOR3( 0.0f,-1.0f, 0.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
            break;
        case 4:
            vLookDir = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            break;
        case 5:
            vLookDir = D3DXVECTOR3( 0.0f, 0.0f,-1.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            break;
    }
	vecDir.x = vLookDir.x;
	vecDir.y = vLookDir.y;
	vecDir.z = vLookDir.z;

    D3DXMATRIX matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookDir, &vUpDir );
    return matView;
}
void CSceneManager::SetSkipHouse(float fx,float fy,float fz,bool bSkip)
{
	for(int cSector=0;cSector<((CSceneManager::m_HeightField.m_LSizeX) * (CSceneManager::m_HeightField.m_LSizeY));cSector++)
	//for(int cSector=0;cSector<LSIZEX*LSIZEY;cSector++)
	{
		if(	m_HeightField.m_SectorScene[cSector].m_AccumulateTM._41 <= fx &&
			m_HeightField.m_SectorScene[cSector].m_AccumulateTM._41+SECTORSIZE > fx &&
			m_HeightField.m_SectorScene[cSector].m_AccumulateTM._43 <= fz &&
			m_HeightField.m_SectorScene[cSector].m_AccumulateTM._43+SECTORSIZE > fz)
		{
			m_HeightField.m_SectorScene[cSector].SetSkipHouse(fx,fy,fz,bSkip);
		}
	}

}
bool CSceneManager::CheckRayCollision(const D3DXVECTOR3 &vecPos1,const D3DXVECTOR3 &vecPos2)
{
	
	return m_HeightField.CheckRayCollision(vecPos1,vecPos2);

}

//////////////////////////////////////////////////////////////////////////


void CSceneManager::SetNullTextureMode(bool bNullTextureMode)
{
	CTexture::NullTextureMode(bNullTextureMode);
	CROSSM::CNTexture::NullTextureMode(bNullTextureMode);
	Z3DTexture::NullTextureMode(bNullTextureMode);
}

void CSceneManager::SetWireFrameMode(bool bWireFrame)
{
	m_bWireFrameMode = bWireFrame;
}


void CSceneManager::AddHouseMap(float fAddx, float fAddy, float fAddz,char *strOutName,char *strMedName,char *strInName,char *strBspName)
{
	m_MapStorage.AddHouseMap(fAddx, fAddy, fAddz, strOutName, strMedName, strInName, strBspName);
}

void CSceneManager::DeleteHouseMap(float fDelx,float fDely,float fDelz,char *filename)
{
	m_MapStorage.DeleteHouseMap(fDelx, fDely, fDelz, filename);
}


void CSceneManager::CollectCollisionTriangleCount(unsigned int& uiOutTriCount)
{
	size_t nVertexCount = 0;
	size_t nTriangleCount = 0;


	// height 정보에 따른 삼각형 갯수 카운트
//	nTriangleCount += (SECTORSX - 1)*(SECTORSY - 1)*2 * m_MapStorage.m_HeightMap.num;

	// house 들의 octree 에 따른 삼각형 갯수 카운트
	char szOctreeFullPathFileName[MAX_PATH];
	for (size_t i = 0; i < (size_t)m_MapStorage.m_HouseMap.size(); ++i)
	{
		CSectorHouseMap& houseMap =	*(m_MapStorage.m_HouseMap[i]);
		for (size_t j = 0; j < (size_t)houseMap.m_strOutName.size(); ++j)
		{
			// out name 으로부터 경로명을 덧붙이고 확장자를 바꾸어 octree 파일의 fullpath 경로를 만듬
			sprintf(szOctreeFullPathFileName,"%s\\%s", OCTREEPATH, houseMap.m_strOutName[j]);
			size_t nLen = strlen(szOctreeFullPathFileName);
			szOctreeFullPathFileName[nLen-3] = 'O';
			szOctreeFullPathFileName[nLen-2] = 'C';
			szOctreeFullPathFileName[nLen-1] = 'T';

			FILE* fp = fopen(szOctreeFullPathFileName, "rb");
			if (NULL == fp)
			{
				continue;
			}

			// 파일 첫 머리가 폴리건 갯수 필드다
			size_t nTotalTri;
			fread(&nTotalTri, sizeof(size_t), 1, fp);
			fclose(fp);

			nVertexCount += nTotalTri*3;
			nTriangleCount += nTotalTri;
		}
	}

	// object 로 쓰이는 mesh 로 부터의 삼각형 갯수 카운트
//	m_MapStorage.m_MeshMap;

	uiOutTriCount = nTriangleCount;
}

void CSceneManager::FillCollisionTriangle(unsigned int uiTriCount)
{
	int i, j, k;//, x, y;
	size_t nTriFilled = 0;

	CrossM::Scene::CollisionTriangleInfo* pTriangle;

 	m_octreeCollider.SetTriangleCount(uiTriCount);		// 버텍스, 삼각형 데이터 공간 할당.
	m_octreeCollider.GetTriangleDataPtr(pTriangle);		// 버텍스, 삼각형 데이터 포인터 얻음


	// heitmap 에 의한 지형 collision triangle 정보 세팅
// 	for (i = 0; i < m_MapStorage.m_HeightMap.num; ++i)
// 	{
// 		CSectorHeightMap& heightMap = *(m_MapStorage.m_HeightMap[i]);
// 
// 		// 한 섹터에 해당하는 삼각형의 인덱스 정보를 구성해 충돌삼각형 정보 리스트에 저장
// 		for (y = 0; y < SECTORSY-1; ++y)
// 		{
// 			for (x = 0; x < SECTORSX-1; ++x)
// 			{
// 				CrossM::Math::VECTOR3 vLowerLeft, vLowerRight, vUpperLeft, vUpperRight;
// 				size_t indexX, indexY;
// 				
// 				indexX = x; indexY = y;
// 				vLowerLeft.SetValue(
// 					heightMap.m_IndexX*SECTORSIZE + indexX*LINTERVAL,
// 					heightMap.m_pHeightData[indexY*SECTORSX + indexX],
// 					heightMap.m_IndexY*SECTORSIZE + indexY*LINTERVAL);
// 
// 				indexX = x+1; indexY = y;
// 				vLowerRight.SetValue(
// 					heightMap.m_IndexX*SECTORSIZE + indexX*LINTERVAL,
// 					heightMap.m_pHeightData[indexY*SECTORSX + indexX],
// 					heightMap.m_IndexY*SECTORSIZE + indexY*LINTERVAL);
// 
// 				indexX = x; indexY = y+1;
// 				vUpperLeft.SetValue(
// 					heightMap.m_IndexX*SECTORSIZE + indexX*LINTERVAL,
// 					heightMap.m_pHeightData[indexY*SECTORSX + indexX],
// 					heightMap.m_IndexY*SECTORSIZE + indexY*LINTERVAL);
// 
// 				indexX = x+1; indexY = y+1;
// 				vUpperRight.SetValue(
// 					heightMap.m_IndexX*SECTORSIZE + indexX*LINTERVAL,
// 					heightMap.m_pHeightData[indexY*SECTORSX + indexX],
// 					heightMap.m_IndexY*SECTORSIZE + indexY*LINTERVAL);
// 
// 				// 지형조각, 좌상단 삼각형
// 				pTriangle[nTriFilled].m_avVertex[0] = vLowerLeft;
// 				pTriangle[nTriFilled].m_avVertex[1] = vUpperLeft;
// 				pTriangle[nTriFilled].m_avVertex[2] = vUpperRight;
// 
// 				// 지형조각, 우하단 삼각형
// 				pTriangle[nTriFilled + 1].m_avVertex[0] = vLowerLeft;
// 				pTriangle[nTriFilled + 1].m_avVertex[1] = vUpperRight;
// 				pTriangle[nTriFilled + 1].m_avVertex[2] = vLowerRight;
// 
// 				nTriFilled += 2;
// 			}
// 		}
// 	}

	// house 들의 octree 에 따른 collision triangle setting
	std::vector< CrossM::Math::VECTOR3 > vTriLoadBuffer;
	char szOctreeFullPathFileName[MAX_PATH];
	for (i = 0; i < (int)m_MapStorage.m_HouseMap.size(); ++i)
	{
		CSectorHouseMap& houseMap =	*(m_MapStorage.m_HouseMap[i]);
		for (j = 0; j < (int)houseMap.m_strOutName.size(); ++j)
		{
			// out name 으로부터 경로명을 덧붙이고 확장자를 바꾸어 octree 파일의 fullpath 경로를 만듬
			sprintf(szOctreeFullPathFileName,"%s\\%s", OCTREEPATH, houseMap.m_strOutName[j]);
			size_t nLen = strlen(szOctreeFullPathFileName);
			szOctreeFullPathFileName[nLen-3] = 'O';
			szOctreeFullPathFileName[nLen-2] = 'C';
			szOctreeFullPathFileName[nLen-1] = 'T';

			FILE* fp = fopen(szOctreeFullPathFileName, "rb");
			if (NULL == fp)
			{
				continue;
			}

			// 파일 첫 머리가 폴리건 갯수 필드다
			int nTotalTri;
			fread(&nTotalTri, sizeof(size_t), 1, fp);
			vTriLoadBuffer.resize(nTotalTri*3);
			fread(&vTriLoadBuffer[0], sizeof(CrossM::Math::VECTOR3)*nTotalTri*3, 1, fp);
			fclose(fp);

			matrix& mTm = houseMap.m_TM[j];

			for (k = 0; k < nTotalTri; ++k)
			{
				CrossM::Scene::CollisionTriangleInfo& tri = pTriangle[ nTriFilled ];

				for (int l = 0; l < 3; ++l)
				{
					CrossM::Math::VECTOR3& vDest = tri.m_avVertex[l];
					CrossM::Math::VECTOR3& vSrc = vTriLoadBuffer[k*3 + l];
					vDest.SetValue(
						vSrc.x*mTm._11 + vSrc.y*mTm._21 + vSrc.z*mTm._31 + mTm._41,
						vSrc.x*mTm._12 + vSrc.y*mTm._22 + vSrc.z*mTm._32 + mTm._42,
						vSrc.x*mTm._13 + vSrc.y*mTm._23 + vSrc.z*mTm._33 + mTm._43);
				}
				++nTriFilled;
			}
		}
	}

}

void CSceneManager::BuildCollisionOctree()
{
	m_octreeCollider.BuildOctree(15, 160);
}


void CSceneManager::GetCollisionRespondingPosition(
									CrossM::Math::VECTOR3& vRespondingPos,
									const CrossM::Math::VECTOR3& vPrevPos, const CrossM::Math::VECTOR3& vNewPos,
									const CrossM::Math::VECTOR3& vCollisionEllipsoidRadius)
{
	m_octreeCollider.GetCollisionRespondingPosition(vRespondingPos, vPrevPos, vNewPos, vCollisionEllipsoidRadius);
}
