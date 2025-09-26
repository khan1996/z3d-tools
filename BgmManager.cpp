// BgmManager.cpp: implementation of the CBgmManager class.
//
//////////////////////////////////////////////////////////////////////

#include "BaseGraphicsLayer.h"
#include "SceneManager.h"
#include "BgmManager.h"
#include "SoundMgr.h"
#include "GMMemory.h"

//#define VOLUME_GAP 0.005

CBgmManager* CBgmManager::ms_pInstance = NULL; 

struct  TESTSCREEN_VERTEX 
{
	float x, y, z, rhw;
	float u, v;
};

CBgmManager::CBgmManager()
{
}

CBgmManager::~CBgmManager()
{
}

CBgmManager* CBgmManager::_GetInstance()
{
	if( ms_pInstance == NULL ) { ms_pInstance = new CBgmManager; }
	return ms_pInstance; 
}

void CBgmManager::_Destroy()
{
	if( ms_pInstance ) ms_pInstance->ReleaseAllData(); 
	SAFE_DELETE( ms_pInstance ); 
}

void CBgmManager::Create( bool bTestMode )
{	
	m_bTestMode = bTestMode; 

	if( m_bTestMode )
	{
		m_pBGMTexture = NULL; 
		m_bTestShow = false; 
	}

	m_Trigger.Create();
	m_bInPlace = false; 
	m_fBgmVol = 0.0f;
	m_fMasterVolume = 1.0f; 
	m_ePlaceStat = BS_ZERO; 
	m_eBgmStat = BS_ZERO; 
	memset( m_strCurBgm, 0, sizeof(m_strCurBgm) ); 
	m_bBgmTurnEvent = false; 
	m_bActivate = true; 
	m_bActBGM = FALSE;
}


void CBgmManager::ReleaseAllData() 
{
	m_Trigger.Destroy(); 

	if( m_bTestMode )
	{
		SAFE_RELEASE( m_pBGMTexture ); 
	}
}

static char strOldFile[MAX_PATH]; 
static CStreamingSound* g_pSound = NULL; 

void CBgmManager::Update( D3DXVECTOR3& Pos, float fSkip, bool bCharSel)
{
	int SectorX, SectorY; 
	PerSectorTriggerInfo* pInfo=NULL; 
	
	GetSectorXYFromPos( SectorX, SectorY, Pos ); 
	pInfo = m_Trigger.GetTrigger( SectorX, SectorY ); 

	static int EventKey; 
	static DWORD EventColor; 
	static char* strFile; 
	static char* strESF; 
	bool bPossible=false; 

	m_EventTable.Update(); 

	float fVolumeGap = 0.01f;

	if(m_bActBGM && g_pSound)
		g_pSound->SetVolume( m_fMasterVolume ); 

	if( pInfo )
	{
		bPossible = GetBGMEventInfo( Pos, pInfo, strFile, EventKey, strESF, EventColor); 

		if( bPossible && !bCharSel && EventKey != EK_CAMP_UNCREATE)
		{
			if(strlen(strESF) > 0 && m_EventColor != EventColor)
			{
				m_EventColor = EventColor;

				// edith 2008.01.26 사운드스크립트에서 지형 이름 출력 로직
				CEffScript* test_script = new CEffScript;
				test_script->GetScriptBinData(strESF);//	"inter_interface-type-keyboard.esf"
				test_script->m_InterfacePos[0] = (float)CSceneManager::m_ScreenWidth / 2.0f;
				test_script->m_InterfacePos[1] = (float)CSceneManager::m_ScreenHeight / 5.0f;
				CSceneManager::m_EffectManager.AddInterfaceScript(test_script);
			}

			bPossible = !CSceneManager::ms_pSceneEventMgr->m_bEventMusic;
		}
		else
			m_EventColor = 0;

		if( bPossible )
		{
			// 이팩트만 출력
			if(EventKey == EK_NOTBGM_SAFE || EventKey >= EK_ESF_SAFE)
			{
				StopBGM();
				return;
			}

			m_fBgmVol += fVolumeGap;
			m_eBgmStat = BS_BGM_PLAY;
		}
		else 
		{
AGAIN:
			// 이팩트만 출력
			if(EventKey == EK_NOTBGM_SAFE || EventKey >= EK_ESF_SAFE)
			{
				StopBGM();
				return;
			}

			m_fBgmVol -= fVolumeGap; 
			 
			if( m_fBgmVol <= 0.0f && m_eBgmStat == BS_BGM_PLAY ) 
			{
				m_EventColor = 0;
				StopBGM();
			}
		}
		// 이팩트만 출력
		if(EventKey == EK_NOTBGM_SAFE || EventKey >= EK_ESF_SAFE)
			return;

		if( m_fBgmVol > 1.0f ) m_fBgmVol = 1.0f; 
		if( m_fBgmVol < 0.0f ) m_fBgmVol = 0.0f;  
		
		if( m_eBgmStat == BS_BGM_PLAY ) 
		{
			PlayBGM( strFile, EventKey ); 
		}
	}
	else if( m_fBgmVol > 0.0f )
	{
		goto AGAIN; 
	}
}


void CBgmManager::GetSectorXYFromPos( int& outX , int& outY, D3DXVECTOR3 inPos )
{
	outX = (int)(inPos.x / SECTORSIZE);
	outY = (int)(inPos.z / SECTORSIZE); 
}


void CBgmManager::GetTextureLockPosFromPos( int& outX, int& outY, D3DXVECTOR3 inPos )
{
	float Magnification = (float)SECTORSIZE / (float)BGM_TEXTURESIZE; 
	
	outX = (int)inPos.x % SECTORSIZE; 
	outY = (int)inPos.z % SECTORSIZE; 

	outX = (float)outX / Magnification; 
	outY = (float)outY / Magnification; 
		
	outY = BGM_TEXTURESIZE - outY -1; //Convert Texture LockPos
}


bool CBgmManager::GetBGMEventInfo( D3DXVECTOR3 inPos, 
								  PerSectorTriggerInfo* pinInfo, char*& outStrFile, int& outEventKey, char*& outESFFile, DWORD& outEventColor)
{
	int LockPosX, LockPosY; 
	GetTextureLockPosFromPos( LockPosX, LockPosY, inPos ); 

	if( m_bTestMode ) LoadTexture( pinInfo ); 

	vector<DWORD>::iterator iter; 
	int index = 0; 
	DWORD BgmColor = pinInfo->ColorTable[LockPosY][LockPosX] & 0x00ffffff; 
	m_EventTable.dwColor = pinInfo->ColorTable[LockPosY][LockPosX]; 
	DWORD ColorT;

	for( iter = pinInfo->BGMColorKeyList.begin(); iter != pinInfo->BGMColorKeyList.end(); iter++ )
	{
		ColorT = (*iter);
		if( BgmColor == ColorT )	
		{
			outStrFile = pinInfo->BGMFileList[index]; 
			m_EventTable.nEvent = outEventKey = pinInfo->EventKeyList[index]; 
			outEventColor = pinInfo->BGMColorKeyList[index]; 
			outESFFile = pinInfo->ESFFileList[index];

			return true; 
		}

		index ++; 
	}

	return false; 
}


void CBgmManager::LoadTexture( PerSectorTriggerInfo* pInfo )
{
	static PerSectorTriggerInfo* pOldInfo = pInfo; 
	static bool bOnce = false; 

	if( !bOnce )
	{
	 	D3DXCreateTextureFromFileEx( BaseGraphicsLayer::GetDevice(), pInfo->strTexFilename, 
								 D3DX_DEFAULT, 
								 D3DX_DEFAULT, 1, 0,
								 D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE, 
								 D3DX_FILTER_NONE, 0x00000000, NULL, NULL, 
								 &m_pBGMTexture ); 
		bOnce = true; 
	}

	if( pOldInfo->iSectorX != pInfo->iSectorX || pOldInfo->iSectorY != pInfo->iSectorY )
	{
		pOldInfo = pInfo; 
		SAFE_RELEASE( m_pBGMTexture ); 
		D3DXCreateTextureFromFileEx( BaseGraphicsLayer::GetDevice(), pInfo->strTexFilename, 
								 D3DX_DEFAULT, 
								 D3DX_DEFAULT, 1, 0,
								 D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE, 
								 D3DX_FILTER_NONE, 0x00000000, NULL, NULL, 
								 &m_pBGMTexture ); 
	}
}

void CBgmManager::StopBGM()
{
	if(m_eBgmStat == BS_BGM_STOP)
		return;

	m_eBgmStat = BS_BGM_STOP; 
	m_ePlaceStat = BS_PLACE_OUT;

	if(strlen(strOldFile) == 0)
		return;

	CStreamingSound* pSound; 
	CSoundMgr::_GetInstance()->GetStreamingSound( pSound, strOldFile ); 
	if(pSound)
		pSound->Stop(); 

	m_bBgmTurnEvent = false; 
	strcpy( strOldFile, "" ); 
}

void CBgmManager::PlayBGM( char* strFilename, int EventKey, BOOL bAct )
{
	static bool bOnce = false; 

	m_bActBGM = bAct; 

	if( !bOnce )
	{
		CSoundMgr::_GetInstance()->GetStreamingSound( g_pSound, strFilename ); 
		
		if(!g_pSound)
			return;

		switch( EventKey )
		{
		case EK_BGM_ONCE_SAFE:
		case EK_BGM_ONCE_UNSAFE:
			{
				g_pSound->SetLooping( false ); 
			}
			break;

		case EK_BGM_TURN_AMB_SAFE:
		case EK_BGM_TURN_AMB_UNSAFE:
			{
				g_pSound->SetLooping( false ); 
				strcpy( m_strCurBgm, strFilename ); 
				m_bBgmTurnEvent = true; 
			}
			break; 

		case EK_BGM_LOOP_SAFE:
		case EK_BGM_LOOP_UNSAFE:
			{
				g_pSound->SetLooping( true ); 
			}
			break; 
		}

		strcpy( strOldFile, strFilename ); 
		g_pSound->SetVolume( m_fBgmVol*m_fMasterVolume );
		g_pSound->Play();
		bOnce = true; 
		m_ePlaceStat = BS_PLACE_IN;
	}

	if( strcmp(strOldFile, strFilename) != 0 )
	{
		// strOldFile 이 StopBGM 이 호출되면 NULL 이기 때문에.. 여기서 보안처리
		if(strlen(strOldFile) != 0)
		{
			CSoundMgr::_GetInstance()->GetStreamingSound( g_pSound, strOldFile ); 

			if(!g_pSound)
				return;

			g_pSound->Stop(); 
			m_bBgmTurnEvent = false; 
		}

		CSoundMgr::_GetInstance()->GetStreamingSound( g_pSound, strFilename ); 

		if(!g_pSound)
			return;

		switch( EventKey )
		{
		case EK_BGM_TURN_AMB_SAFE:
		case EK_BGM_TURN_AMB_UNSAFE:
			{
				m_bBgmTurnEvent = true; 
				g_pSound->SetLooping( false ); 
				strcpy( m_strCurBgm, strFilename ); 
			}
			break; 

		case EK_BGM_ONCE_SAFE:
		case EK_BGM_ONCE_UNSAFE:
			{
				g_pSound->SetLooping( false ); 
			}
			break;

		case EK_BGM_LOOP_SAFE:
		case EK_BGM_LOOP_UNSAFE:
			{
				g_pSound->SetLooping( true ); 
			}
			break; 
		}
		strcpy( strOldFile, strFilename ); 
		g_pSound->SetVolume( m_fBgmVol*m_fMasterVolume );
		g_pSound->Play();
		m_ePlaceStat = BS_PLACE_IN;
	}

	if( strcmp(strOldFile, strFilename) == 0 && m_ePlaceStat == BS_PLACE_OUT )
	{
		m_ePlaceStat = BS_PLACE_IN; 
		CSoundMgr::_GetInstance()->GetStreamingSound( g_pSound, strFilename ); 

		if(!g_pSound)
			return;

		switch( EventKey )
		{
		case EK_BGM_TURN_AMB_SAFE:
		case EK_BGM_TURN_AMB_UNSAFE:
			{
				m_bBgmTurnEvent = true; 
				g_pSound->SetLooping( false ); 
				strcpy( m_strCurBgm, strFilename ); 
			}
			break; 

		case EK_BGM_ONCE_SAFE:
		case EK_BGM_ONCE_UNSAFE:
			{
				g_pSound->SetLooping( false ); 
			}
			break;

		case EK_BGM_LOOP_SAFE:
		case EK_BGM_LOOP_UNSAFE:
			{
				g_pSound->SetLooping( true ); 
			}
			break; 
		}
		strcpy( strOldFile, strFilename ); 
		g_pSound->SetVolume( m_fBgmVol*m_fMasterVolume );
		g_pSound->Play();
	}

	if(!g_pSound)
		return;

	g_pSound->SetVolume( m_fBgmVol*m_fMasterVolume ); 
}



void CBgmManager::TestScreen( D3DXVECTOR3 Pos )
{
	if( !m_bTestShow ) return; 

	LPDIRECT3DDEVICE8 pd3dDevice = BaseGraphicsLayer::GetDevice(); 

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
 	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE ); 
	pd3dDevice->SetVertexShader( D3DFVF_XYZRHW | D3DFVF_TEX1 ); 
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ); 
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE ); 
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE ); 
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE ); 
	D3DVIEWPORT8 TempViewport; 
	pd3dDevice->GetViewport( &TempViewport ); 
	
	float center = TempViewport.Width - 300; 

	TESTSCREEN_VERTEX pVertex[4]; 
	pVertex[0].x = center; 
	pVertex[0].y = 0; 
	pVertex[0].z = 0.0f; 
	pVertex[0].rhw = 1.0f; 
	pVertex[0].u = 0.0f; 
	pVertex[0].v = 0.0f; 

	pVertex[1].x = 300.0f + center; 
	pVertex[1].y = 0; 
	pVertex[1].z = 0.0f; 
	pVertex[1].rhw = 1.0f; 
	pVertex[1].u = 1.0f; 
	pVertex[1].v = 0.0f; 

	pVertex[2].x = center; 
	pVertex[2].y = 300.0f; 
	pVertex[2].z = 0.0f; 
	pVertex[2].rhw = 1.0f; 
	pVertex[2].u = 0.0f; 
	pVertex[2].v = 1.0f; 

	pVertex[3].x = 300.0f + center; 
	pVertex[3].y = 300.0f; 
	pVertex[3].z = 0.0f; 
	pVertex[3].rhw = 1.0f; 
	pVertex[3].u = 1.0f; 
	pVertex[3].v = 1.0f; 

	pd3dDevice->SetTexture( 0, m_pBGMTexture );
	pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, pVertex, sizeof(TESTSCREEN_VERTEX) ); 
		
	//Set things back to normal
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW ); 
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ); 
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE ); 
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, TRUE ); 
}


void CBgmManager::AllStop()
{
	m_bInPlace = false; 
	m_fBgmVol = 0.0f;
	m_ePlaceStat = BS_ZERO; 
	m_eBgmStat = BS_ZERO; 
	CSoundMgr::_GetInstance()->AllStopStreamingSound(); 
}


bool CBgmManager::IsSafetyZone( D3DXVECTOR3& Pos )
{
	int SectorX, SectorY; 
	PerSectorTriggerInfo* pInfo=NULL; 

	GetSectorXYFromPos( SectorX, SectorY, Pos ); 
	pInfo = m_Trigger.GetTrigger( SectorX, SectorY ); 

	if( pInfo )
	{
//		DWORD ColorKey;
		int LockPosX, LockPosY; 
		GetTextureLockPosFromPos( LockPosX, LockPosY, Pos ); 
		DWORD BgmColor = pInfo->ColorTable[LockPosY][LockPosX] & 0x00ffffff; 

		for( size_t i = 0; i < pInfo->BGMColorKeyList.size(); ++i )
		{
//			ColorKey = pInfo->BGMColorKeyList[i];
			if( BgmColor == pInfo->BGMColorKeyList[i] )			
			{
				switch( pInfo->EventKeyList[i] )
				{
				case EK_BGM_LOOP_SAFE:
				case EK_BGM_ONCE_SAFE:
				case EK_NOTBGM_SAFE:
				case EK_BGM_TURN_AMB_SAFE:
				case EK_ESF_SAFE:
					return true; 

				default:
					return false; 
				}
			}
		}

		return false; 
	}

	return false;
}

bool CBgmManager::IsCurBgmTurnAmbEvent()
{ 

	if( m_bBgmTurnEvent )
	{
		CStreamingSound* pSound; 
		CSoundMgr::_GetInstance()->GetStreamingSound( pSound, m_strCurBgm ); 
		return pSound->IsPlaying(); 
	}
	return false; 
}

void CBgmManager::SetActivate( bool bAct )
{
	m_bActivate = bAct; 
	if( bAct == false )
		m_fMasterVolume = 0.0f; 
}