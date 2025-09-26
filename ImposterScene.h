#pragma once

#include "RenderTexture.h"
#include "HouseObjectScene.h"
#include <queue>
using namespace std; 

const int IMP_LTEXTURE_NUM = 1; 
const int IMP_LTEXTURE_SIZE = 1024;
const int IMP_STEXTURE_SIZE = 64; 
const int IMP_PERTEXTURE_SCENE_NUM = (IMP_LTEXTURE_SIZE/IMP_STEXTURE_SIZE)*(IMP_LTEXTURE_SIZE/IMP_STEXTURE_SIZE); 
const int IMP_TOTALSCENE_NUM = IMP_PERTEXTURE_SCENE_NUM*IMP_LTEXTURE_NUM; 

class CImposterScene
{
	typedef std::queue<int> COUNT_QUEUE; 

public:
	static CImposterScene*				ms_pScenesPool[IMP_TOTALSCENE_NUM]; 
	static COUNT_QUEUE					ms_UnusedSceneQueue; 
	static LPDIRECT3DTEXTURE8		ms_pSceneTexture[IMP_LTEXTURE_NUM];  
	static LPDIRECT3DSURFACE8		ms_pSceneZBuffer[IMP_LTEXTURE_NUM]; 
	static LPDIRECT3DSURFACE8		ms_pSceneSurface[IMP_LTEXTURE_NUM]; 
	static LPDIRECT3DSURFACE8		ms_pTempZBuffer; 
	static LPDIRECT3DSURFACE8		ms_pTempSurface; 
	static D3DVIEWPORT8					ms_TempViewport; 
	static float										ms_fViewRatio; 
	static float										ms_fViewDist; 
	static bool									ms_bActivate; 

	int													m_nIndex; 
	int													m_nTextureIndex; 
	D3DXVECTOR3								m_vOldCamToScene; 
	CHouseObjectScene*					m_pSrcScene; 
	D3DVIEWPORT8							m_ViewPort; 

private:
	CImposterScene( int nIndex ); 
	virtual ~CImposterScene(); 
	void BeginImposter(); 
	void EndImposter(); 
	bool GenerateRenderTexture( D3DXVECTOR3& vCamPos, D3DXVECTOR3& vScenePos );
	void Clear(); 
	
public:
	static bool Init(); 
	static void Shutdown(); 
	static CImposterScene* GetScene( CHouseObjectScene* pScene ); 
	static void DeleteScene( CImposterScene* pScene ); 
	static void SetViewRatio( float f ); 
	static float GetViewRatio();
	static void SetViewDistance( float f );
	static float GetViewDistance(); 
	static int GetSceneNum(); 
	static bool IsRenderEnable( float fDist, CHouseObjectScene* pScene ); 
	static bool IsActivated(); 
	
	////////////////////////////////////////////////////////////////////////////

	void Render( D3DXVECTOR3& vCamPos, D3DXVECTOR3& vScenePos ); 
};
