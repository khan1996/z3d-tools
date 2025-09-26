#include ".\imposterscene.h"
#include "BaseGraphicsLayer.h"
#include "SceneManager.h"
#include "NTexture.h"
#include "GMMemory.h"

using namespace CROSSM; 

struct ImposterVertex
{
	enum { FVF = D3DFVF_XYZ|D3DFVF_TEX1 };
	D3DXVECTOR3 p;
	float u, v; 
};

CImposterScene*								CImposterScene::ms_pScenesPool[IMP_TOTALSCENE_NUM]; 
CImposterScene::COUNT_QUEUE	CImposterScene::ms_UnusedSceneQueue; 
LPDIRECT3DTEXTURE8						CImposterScene::ms_pSceneTexture[IMP_LTEXTURE_NUM];  
LPDIRECT3DSURFACE8					CImposterScene::ms_pSceneZBuffer[IMP_LTEXTURE_NUM]; 
LPDIRECT3DSURFACE8					CImposterScene::ms_pSceneSurface[IMP_LTEXTURE_NUM]; 
LPDIRECT3DSURFACE8					CImposterScene::ms_pTempZBuffer; 
LPDIRECT3DSURFACE8					CImposterScene::ms_pTempSurface; 
D3DVIEWPORT8								CImposterScene::ms_TempViewport; 
float													CImposterScene::ms_fViewRatio = 550.0f; //값이 클수록 보이는 비주얼이 커짐
float													CImposterScene::ms_fViewDist = 20000.0f; //거리 제한값
bool													CImposterScene::ms_bActivate; 

CImposterScene::CImposterScene( int nIndex ) : m_nIndex(nIndex), m_vOldCamToScene(D3DXVECTOR3(0, 0, 0)), m_pSrcScene(NULL)
{
	m_nTextureIndex = nIndex / IMP_PERTEXTURE_SCENE_NUM;  
	int nTemp = m_nIndex % IMP_PERTEXTURE_SCENE_NUM; 
	m_ViewPort.Width = IMP_STEXTURE_SIZE; 
	m_ViewPort.Height = IMP_STEXTURE_SIZE; 
	m_ViewPort.MinZ = 0;
	m_ViewPort.MaxZ = 1;
	m_ViewPort.X = (nTemp % (IMP_LTEXTURE_SIZE/IMP_STEXTURE_SIZE)) * IMP_STEXTURE_SIZE; 
	m_ViewPort.Y = (nTemp / (IMP_LTEXTURE_SIZE/IMP_STEXTURE_SIZE)) * IMP_STEXTURE_SIZE;
}

CImposterScene::~CImposterScene()
{
}

bool CImposterScene::Init()
{
	LPDIRECT3DDEVICE8 pDevice = BaseGraphicsLayer::GetDevice(); 
	
    int i; 
	HRESULT hr; 
	
    for( i = 0; i < IMP_LTEXTURE_NUM; i++ )
	{
		hr = pDevice->CreateTexture( IMP_LTEXTURE_SIZE, IMP_LTEXTURE_SIZE, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, //BaseGraphicsLayer::m_d3dpp.BackBufferFormat, 
			D3DPOOL_DEFAULT, &ms_pSceneTexture[i] ); 
		if( FAILED(hr) )
		{
//			MessageBox( 0, "텍스처 생성 실패 : imposter", "Error", 0 ); 
			return false; 
		}
		
		hr = ms_pSceneTexture[i]->GetSurfaceLevel( 0, &ms_pSceneSurface[i] ); 
		if( FAILED(hr) )
		{
			Shutdown();
			MessageBox( 0, "서페이스 생성 실패 : imposter", "Error", 0 ); 
			return false; 
		}

		pDevice->CreateDepthStencilSurface( IMP_LTEXTURE_SIZE, IMP_LTEXTURE_SIZE, BaseGraphicsLayer::m_d3dpp.AutoDepthStencilFormat, 
			D3DMULTISAMPLE_NONE, &ms_pSceneZBuffer[i] ); 
		if( FAILED(hr) )
		{
			Shutdown();
			MessageBox( 0, "Z버퍼 생성 실패 : imposter", "Error", 0 ); 
			return false; 
		}
	}

	for( i = 0; i < IMP_TOTALSCENE_NUM; i++ )
	{
		ms_pScenesPool[i] = new CImposterScene(i); 
	}

	while( !ms_UnusedSceneQueue.empty() )
	{
		ms_UnusedSceneQueue.pop(); 
	}

	for( i = 0; i < IMP_TOTALSCENE_NUM; i++ )
	{
		ms_UnusedSceneQueue.push(i); 
	}

	ms_bActivate = true; 

	return true; 
}

void CImposterScene::Shutdown()
{
	for( int i = 0; i < IMP_LTEXTURE_NUM; i++ )
	{
		SAFE_RELEASE( ms_pSceneTexture[i] ); 
		SAFE_RELEASE( ms_pSceneZBuffer[i] );
		SAFE_RELEASE( ms_pSceneSurface[i] ); 
	}

	for(int i = 0; i < IMP_TOTALSCENE_NUM; i++ )
	{
		SAFE_DELETE( ms_pScenesPool[i] ); 
	}

	ms_bActivate = false; 
}

CImposterScene* CImposterScene::GetScene( CHouseObjectScene* pScene )
{
	if( ms_bActivate == false || ms_UnusedSceneQueue.empty() )
		return NULL; 

	int nCount = ms_UnusedSceneQueue.front(); 
	ms_UnusedSceneQueue.pop(); 

	CImposterScene* pImposter = ms_pScenesPool[nCount];
	pImposter->m_pSrcScene = pScene; 
	
	return pImposter; 
}

void CImposterScene::DeleteScene( CImposterScene* pScene )
{
	if( pScene && ms_bActivate )
	{
		pScene->Clear(); 
		ms_UnusedSceneQueue.push( pScene->m_nIndex );
	}
}

void CImposterScene::SetViewRatio( float f )
{
	ms_fViewRatio = f; 
}

float CImposterScene::GetViewRatio()
{
	return ms_fViewRatio; 
}

void CImposterScene::Render( D3DXVECTOR3& vCamPos, D3DXVECTOR3& vScenePos )
{	
	//텍스쳐를 갱신했을경우 바로 그리지 않고 다음프레임에 그린다 
	if( GenerateRenderTexture(vCamPos, vScenePos) ) return; 

	LPDIRECT3DDEVICE8 pDevice = BaseGraphicsLayer::GetDevice();

	//Set RenderState !!
	pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 ); 
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE ); 
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 ); 
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTA_TEXTURE ); 
	pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE ); 
	pDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE ); 
	
	pDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE ); 
	pDevice->SetRenderState( D3DRS_ALPHAREF, 0xee ); 
	pDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL ); 
	pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ); 
	
	pDevice->SetRenderState( D3DRS_ZENABLE, TRUE ); 
	pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE ); 

	ImposterVertex Quad[4]; 
	Quad[0].p = D3DXVECTOR3( -m_pSrcScene->m_fRad, m_pSrcScene->m_fRad, 0 ); 
	Quad[0].u = (float)m_ViewPort.X / (float)IMP_LTEXTURE_SIZE; 
	Quad[0].v = (float)m_ViewPort.Y / (float)IMP_LTEXTURE_SIZE;  
	Quad[1].p = D3DXVECTOR3( m_pSrcScene->m_fRad, m_pSrcScene->m_fRad, 0 ); 
	Quad[1].u = ((float)m_ViewPort.X + (float)m_ViewPort.Width) / (float)IMP_LTEXTURE_SIZE; 
	Quad[1].v = (float)m_ViewPort.Y / (float)IMP_LTEXTURE_SIZE; 
	Quad[2].p = D3DXVECTOR3( -m_pSrcScene->m_fRad, -m_pSrcScene->m_fRad, 0 ); 
	Quad[2].u = (float)m_ViewPort.X / (float)IMP_LTEXTURE_SIZE; 
	Quad[2].v = ((float)m_ViewPort.Y + (float)m_ViewPort.Height) / (float)IMP_LTEXTURE_SIZE;
	Quad[3].p = D3DXVECTOR3( m_pSrcScene->m_fRad, -m_pSrcScene->m_fRad, 0 ); 
	Quad[3].u = ((float)m_ViewPort.X + (float)m_ViewPort.Width) / (float)IMP_LTEXTURE_SIZE; 
	Quad[3].v = ((float)m_ViewPort.Y + (float)m_ViewPort.Height) / (float)IMP_LTEXTURE_SIZE;

	//Set RenderState
	pDevice->SetTexture( 0, ms_pSceneTexture[m_nTextureIndex] ); 

	//DrawPrimitive
	D3DXMATRIX mBillboard; 
	D3DXMatrixIdentity( &mBillboard ); 
	mBillboard._11 = CSceneManager::m_ViewCamera->GetMatView()->_11; 
	mBillboard._13 = CSceneManager::m_ViewCamera->GetMatView()->_13; 
	mBillboard._31 = CSceneManager::m_ViewCamera->GetMatView()->_31; 
	mBillboard._33 = CSceneManager::m_ViewCamera->GetMatView()->_33; 
	D3DXMatrixInverse( &mBillboard, NULL, &mBillboard ); 
	mBillboard._41 = m_pSrcScene->m_AccumulateTM._41; 
	mBillboard._42 = m_pSrcScene->m_AccumulateTM._42; 
	mBillboard._43 = m_pSrcScene->m_AccumulateTM._43; 
	pDevice->SetVertexShader( ImposterVertex::FVF ); 
	pDevice->SetTransform( D3DTS_WORLD, &mBillboard );
	pDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, Quad, sizeof(ImposterVertex) ); 
	pDevice->SetVertexShader(NULL);

	//Back RenderState 
	pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA ); 
	pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ); 
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );		
	pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	pDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE);	
	pDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE  );
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );
	pDevice->SetRenderState( D3DRS_LIGHTING,TRUE);
}

void CImposterScene::BeginImposter()
{
	LPDIRECT3DDEVICE8 pDevice = BaseGraphicsLayer::GetDevice();
	pDevice->GetViewport( &ms_TempViewport ); 
	pDevice->GetRenderTarget( &ms_pTempSurface ); 
	pDevice->GetDepthStencilSurface( &ms_pTempZBuffer ); 
	pDevice->SetRenderTarget( ms_pSceneSurface[m_nTextureIndex], ms_pSceneZBuffer[m_nTextureIndex] );		
	pDevice->EndScene(); 
	
	D3DRECT rect; 
	rect.x1 = m_ViewPort.X; 
	rect.y1 = m_ViewPort.Y; 
	rect.x2 = m_ViewPort.X + m_ViewPort.Width; 
	rect.y2 = m_ViewPort.Y + m_ViewPort.Height; 
	pDevice->Clear( 1, &rect, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0x00, 1.0f, 0 );
	pDevice->BeginScene(); 
}

void CImposterScene::EndImposter()
{
	LPDIRECT3DDEVICE8 pDevice = BaseGraphicsLayer::GetDevice();
	pDevice->EndScene(); 

	pDevice->SetRenderTarget( ms_pTempSurface, ms_pTempZBuffer ); 
	pDevice->SetViewport( &ms_TempViewport ); 
	SAFE_RELEASE( ms_pTempZBuffer ); 
	SAFE_RELEASE( ms_pTempSurface ); 

	pDevice->BeginScene(); 
}

bool CImposterScene::GenerateRenderTexture( D3DXVECTOR3& vCamPos, D3DXVECTOR3& vScenePos )
{
	D3DXVECTOR3 vCamToScene = vScenePos - vCamPos; 
	D3DXVec3Normalize( &vCamToScene, &vCamToScene ); 
	if( D3DXVec3Dot(&m_vOldCamToScene, &vCamToScene) > 0.5f ) return false; 
	
	static int nLoadSchedule; 
	++nLoadSchedule; 
	if( nLoadSchedule % 5 != 0 ) return false; 

	//벌거숭이 메쉬를 찍지 않기 위해 쓰레드로딩이 다될때까지 기다린다
	int nCompleteLoad = 0; 

	// 2005/01/06 yundi. CHouseObjectScene::m_HouseObject 가 NULL 일 경우 처리
	if( NULL == m_pSrcScene->m_HouseObject ||
		NULL == m_pSrcScene->m_HouseObject->m_pOutHouseObject ||
		false == m_pSrcScene->m_HouseObject->m_pOutHouseObject->IsLoaded() )
	{
		return false;
	}
	else
	{
		for( int i = 0; i < (int)m_pSrcScene->m_HouseObject->m_pOutHouseObject->m_pMat.size(); ++i )
		{
			CNTexture* pTex = m_pSrcScene->m_HouseObject->m_pOutHouseObject->m_pMat[i]; 
			if( pTex->IsLoaded() )
			{
				++nCompleteLoad; 
			}
		}
		if( nCompleteLoad < (int)m_pSrcScene->m_HouseObject->m_pOutHouseObject->m_pMat.size() )
		{
			return false; 
		}
	}

	m_vOldCamToScene = vCamToScene; 	

	LPDIRECT3DDEVICE8 pd3dDevice = BaseGraphicsLayer::GetDevice(); 
	D3DXMATRIX mView, mSaveView; 
	D3DXMatrixIdentity( &mView ); 
	D3DXMatrixLookAtLH( &mView, &vCamPos, &vScenePos, &D3DXVECTOR3(0, 1, 0) ); 
	pd3dDevice->GetTransform( D3DTS_VIEW, &mSaveView ); 
	pd3dDevice->SetTransform( D3DTS_VIEW, &mView ); 
	pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&m_pSrcScene->m_AccumulateTM);	

	D3DXMATRIX mProj, mSaveProj; 
	D3DXMatrixOrthoLH( &mProj,  m_pSrcScene->m_fRad*2,  m_pSrcScene->m_fRad*2, 1.0f, 100000.0f);	
	pd3dDevice->GetTransform( D3DTS_PROJECTION, &mSaveProj ); 
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &mProj ); 

	BeginImposter(); 
	pd3dDevice->SetViewport( &m_ViewPort ); 

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );		
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2  );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );	
	pd3dDevice->SetRenderState( D3DRS_ALPHAREF, 0xee );
	pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );	
	pd3dDevice->SetRenderState( D3DRS_LIGHTING,TRUE );
	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xFFFFFFFF ); 
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ); 
	pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0x0000000F );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE ); 
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE ); 
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE ); 
	m_pSrcScene->m_HouseObject->Render( pd3dDevice ); 
	EndImposter();

	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	pd3dDevice->SetTransform( D3DTS_VIEW, &mSaveView ); 
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &mSaveProj ); 
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE ); 
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE ); 
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, TRUE ); 

	return true; 
}

int CImposterScene::GetSceneNum()
{
	return IMP_TOTALSCENE_NUM - ms_UnusedSceneQueue.size(); 
}

void CImposterScene::Clear()
{
	memset( m_vOldCamToScene, 0, sizeof(D3DXVECTOR3) ); 
	m_pSrcScene = NULL; 
}

void CImposterScene::SetViewDistance( float f )
{
	ms_fViewDist = f; 	
}

float CImposterScene::GetViewDistance()
{
	return ms_fViewDist; 
}

bool CImposterScene::IsRenderEnable( float fDist, CHouseObjectScene* pScene )
{
	if( ms_bActivate == false )
		return false;

	if( fDist > ms_fViewDist && 
		pScene->m_fRad*pScene->m_fRad/fDist < ms_fViewRatio )
		return true; 

	return false; 
}

bool CImposterScene::IsActivated()
{
	return ms_bActivate;
}
