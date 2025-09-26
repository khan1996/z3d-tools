// AmbienceManager.cpp: implementation of the CAmbienceManager class.
//
//////////////////////////////////////////////////////////////////////

#include "BaseGraphicsLayer.h"
#include "SceneManager.h"
#include "SoundMgr.h"
#include "AmbienceManager.h"
#include "BgmManager.h"
#include "GMMemory.h"

//#define AMB_GAP 0.005

CAmbienceManager* CAmbienceManager::ms_pInstance = NULL; 

struct TESTSCREEN_VERTEX
{
	float x, y, z, rhw;
	float u, v;
};

CAmbienceManager* CAmbienceManager::_GetInstance()
{
	if( ms_pInstance == NULL )
	{ ms_pInstance = new CAmbienceManager; }
	return ms_pInstance; 
}

CAmbienceManager::CAmbienceManager()
{
}

CAmbienceManager::~CAmbienceManager()
{
}

void CAmbienceManager::_Destroy()
{
	if( ms_pInstance ) ms_pInstance->ReleaseAllData(); 
	SAFE_DELETE( ms_pInstance ); 
}


void CAmbienceManager::Create( bool bTestMode )
{
	m_bTestMode = bTestMode; 
	if( m_bTestMode )
	{
		m_pAmbTexture = NULL;	
		m_bTestShow = false; 
		m_ptFont= new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
		m_ptFont->InitDeviceObjects( BaseGraphicsLayer::GetDevice() ); 
		m_ptFont->RestoreDeviceObjects();
		D3DXCreateSprite( BaseGraphicsLayer::GetDevice(), &m_pMouseArrow ); 
		D3DXCreateTextureFromFileEx( BaseGraphicsLayer::GetDevice(), "MouseArrow.bmp", 
									 D3DX_DEFAULT, 
									 D3DX_DEFAULT, 1, 0,
									 D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE, 
									 D3DX_FILTER_NONE, 0xff000000, NULL, NULL, 
									 &m_pArrowTexture );
	}

	m_pSectorAmbMap = new CSectorAmbienceMap; 
	m_bFadeIn = false; 
	m_bFadeOut = false; 
	m_fFadeInVolume = 0.0f; 
	m_fFadeOutVolume = 0.0f; 
	strcpy( m_strFadeInFileName, "" ); 
	strcpy( m_strFadeOutFileName, "" ); 
	strcpy( m_strOldFile, "" ); 
	m_bAmbPlay = false; 
	strcpy( m_strAmbPlayFile, "" ); 
	m_fAmbVolume =0.0f; 
	m_fMasterVolume = 1.0f; 
	m_pCurSchedule = NULL; 
}


void CAmbienceManager::ReleaseAllData()
{
	SAFE_DELETE( m_pSectorAmbMap ); 

	if( m_bTestMode )
	{
		SAFE_RELEASE( m_pAmbTexture ); 
		SAFE_RELEASE( m_pMouseArrow ); 
		SAFE_RELEASE( m_pArrowTexture ); 
		delete m_ptFont; 
	}
}



void CAmbienceManager::Update( D3DXVECTOR3& Pos, float fSkip, bool bCharSel )
{

	int SectorX, SectorY; 
	SectorAmbience* pInfo=NULL; 
	float fVolume; 
	D3DVECTOR vPoint;
	DWORD dwMode; 
	char* strPlayFile; 
	float fVolumeGap = 0.01f; 

	SectorX = (int)(Pos.x / SECTORSIZE);
	SectorY = (int)(Pos.z / SECTORSIZE); 
	
	pInfo = m_pSectorAmbMap->GetSectorAmbience( SectorX, SectorY ); 
	
	if( pInfo && !bCharSel)
	{
		bool bPlay = GetAmbienceInfo( pInfo, Pos, strPlayFile, vPoint, fVolume, dwMode ); 

		if( CBgmManager::_GetInstance()->IsCurBgmTurnAmbEvent() || 
			CSceneManager::ms_pSceneEventMgr->m_bEventMusic ) 
		{
			bPlay = false; 
		}

		/*
		if( m_bTestMode )
		{
			if( bPlay )
			{
				m_ptFont->DrawText( 2, 175, D3DCOLOR_ARGB(255,255,255,0), "AmbienceOn" ); 
				if( strPlayFile ) m_ptFont->DrawText( 2, 200, D3DCOLOR_ARGB(255,255,255,0), strPlayFile ); 
				char buf[MAX_PATH]; 
				sprintf( buf, "%f, %f, %f", vPoint.x, vPoint.y, vPoint.z ); 
				m_ptFont->DrawText( 2, 225, D3DCOLOR_ARGB(255,255,255,0), buf ); 
				sprintf( buf, "VOLUME:%f", fVolume ); 
				m_ptFont->DrawText( 2, 250, D3DCOLOR_ARGB(255,255,255,0), buf ); 
			}
		}
		*/
		
		CStreamingSound3D* pSound3D;
		if( bPlay )
		{
			PlayAmbience( strPlayFile, vPoint, fVolume, dwMode );
			RandomPlaySound(); 
		}

		if( m_bFadeIn )
		{
			m_fFadeInVolume += fVolumeGap; 
			if( m_fFadeInVolume >= fVolume ) 
			{
				m_bFadeIn = false; 
			}

			CSoundMgr::_GetInstance()->GetStreamingSound3D( pSound3D, m_strFadeInFileName );
			pSound3D->SetVolume( m_fFadeInVolume*m_fMasterVolume );
		}
		else if( bPlay )
		{
			CSoundMgr::_GetInstance()->GetStreamingSound3D( pSound3D, m_strFadeInFileName );
			pSound3D->SetVolume( fVolume*m_fMasterVolume );
			m_fAmbVolume = fVolume; 
		}

		if( m_bFadeOut )
		{
			m_fFadeOutVolume -= fVolumeGap; 
			
			CSoundMgr::_GetInstance()->GetStreamingSound3D( pSound3D, m_strFadeOutFileName ); 
			if( m_fFadeOutVolume >= 0.0f ) pSound3D->SetVolume( m_fFadeOutVolume*m_fMasterVolume ); 

			if( m_fFadeOutVolume <= 0.0f ) 
			{
				m_bFadeOut = false; 
				pSound3D->Stop(); 
			}
		}

		static float fAmbVolume=0.0f; 
		static bool bInit = false; 
		if( !bPlay && m_bAmbPlay )
		{
			if( !bInit )
			{	
				fAmbVolume = m_fAmbVolume; 
				bInit = true; 
			}
 			fAmbVolume -= fVolumeGap; 
	
			CSoundMgr::_GetInstance()->GetStreamingSound3D( pSound3D, m_strAmbPlayFile ); 
			if( fAmbVolume <= 0.0f )
			{ 
				pSound3D->Stop(); 
				m_bAmbPlay = false; 
				bInit = false; 
				strcpy( m_strOldFile, "" ); 
			}
			else pSound3D->SetVolume( fAmbVolume*m_fMasterVolume );
		}
		
		//if( m_bTestMode ) TestScreen( Pos );
	}
}


void CAmbienceManager::LoadTexture( SectorAmbience* pInfo )
{
	static SectorAmbience* pOldInfo = pInfo; 
	static bool bOnce = false; 

	if( !bOnce )
	{
		SAFE_RELEASE(m_pAmbTexture); 
		D3DXCreateTextureFromFileEx( BaseGraphicsLayer::GetDevice(), pInfo->strTexFilename, 
								 D3DX_DEFAULT, 
								 D3DX_DEFAULT, 1, 0,
								 D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE, 
								 D3DX_FILTER_NONE, 0x00000000, NULL, NULL, 
								 &m_pAmbTexture ); 
		bOnce = true; 
	}

	if( pOldInfo->iSectorX != pInfo->iSectorX || pOldInfo->iSectorY != pInfo->iSectorY )
	{
		pOldInfo = pInfo; 
		SAFE_RELEASE(m_pAmbTexture); 
		D3DXCreateTextureFromFileEx( BaseGraphicsLayer::GetDevice(), pInfo->strTexFilename, 
								 D3DX_DEFAULT, 
								 D3DX_DEFAULT, 1, 0,
								 D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE, 
								 D3DX_FILTER_NONE, 0x00000000, NULL, NULL, 
								 &m_pAmbTexture ); 
	}
}



void CAmbienceManager::PlayAmbience( char* strFile, D3DVECTOR vPos, float fVol, DWORD dwMode )
{

	static CStreamingSound3D* pSound3D;

	if( strcmp(m_strOldFile, strFile) != 0 )
	{
		m_bFadeIn = true; 
		strcpy( m_strFadeInFileName, strFile ); 
		m_fFadeInVolume = 0.0f; 

		m_bAmbPlay = true; 
		strcpy( m_strAmbPlayFile, strFile ); 

		if( strlen(m_strOldFile) )
		{
			m_bFadeOut = true; 
			strcpy( m_strFadeOutFileName, m_strOldFile ); 
			CSoundMgr::_GetInstance()->GetStreamingSound3D( pSound3D, m_strOldFile ); 
			pSound3D->GetVolume( m_fFadeOutVolume );
		}

		CSoundMgr::_GetInstance()->GetStreamingSound3D( pSound3D, strFile ); 
		strcpy( m_strOldFile, strFile ); 
		pSound3D->SetPosition( vPos ); 
		pSound3D->SetVolume( 0.0f ); 
		pSound3D->SetMode( dwMode );  
		pSound3D->SetLooping( true ); 
		pSound3D->Play(); 
	}
}


bool CAmbienceManager::GetAmbienceInfo( SectorAmbience* pinInfo, D3DXVECTOR3& inPos, char*& outStrFile, 
									    D3DVECTOR& outPos, float& outVol, DWORD& outMode )
{
	int LockPosX, LockPosY; 
	GetTextureLockPos( LockPosX, LockPosY, inPos ); 

	if( m_bTestMode ) LoadTexture( pinInfo ); 
		
	vector<AmbienceInfo*>::iterator iter; 
	for( iter = pinInfo->AmbInfoList.begin(); iter != pinInfo->AmbInfoList.end(); iter++ )
	{
		DWORD AmbColor = pinInfo->ColorTable[LockPosY][LockPosX] & 0x00ffffff; 
		
		if( AmbColor == (*iter)->dwColor || AmbColor == (*iter)->dwPointColor ) 
		{
			//1.찾은 엠비언스의 음원들중 케릭터와 가장가까운 음원을 찾는다.  
			int PointNum = (*iter)->AmbPointList.size();
				
			//엠비언스의 음원이 없다면 
			if( PointNum == 0 )
			{
				outMode = MODE_DISABLE; 
			}
			else  
			{
				//가장 가까운 음원을 찾는다. HeadPoint에 저장 
				D3DXVECTOR3 HeadPoint;
				float OldLength; 
				D3DXVECTOR3	Point; 
				D3DXVECTOR3 Dist; 
				float length; 

				for( int i = 0; i < PointNum; i++ )
				{
					Point = *((*iter)->AmbPointList[i]); 
					Dist = inPos - Point;  
					length = D3DXVec3Length( &Dist ); 
						
					if(!i)
					{
						OldLength = length; 
						HeadPoint = Point; 
					}

					if( OldLength >= length && i )
					{
						OldLength = length;
						HeadPoint = Point; 
					}
				}
				
				outPos = HeadPoint; // HeadPoint 이것이 가장가까운 음원 좌표 
				outMode = MODE_NORMAL; 
			}

			//2. 다음으로 스케줄에 따른 재생파일이름을 얻는다. 
			float fCurTime = CSceneManager::m_WeatherManager.m_fWeatherTime;  
		
			outStrFile = NULL; 

			// edith 2009.05.05 AMB 사운드의 스케쥴 처리 로직임.
			if( fCurTime >= 6.0f && fCurTime < 18.0f )
			{
				m_pCurSchedule = (*iter)->ScheduleList[0]; 
				outStrFile = m_pCurSchedule->strFilename; 
			}
			else if( (*iter)->ScheduleList.size() > 1 )
			{
				m_pCurSchedule = (*iter)->ScheduleList[1]; 
				outStrFile = m_pCurSchedule->strFilename; 
			}

			/*
			for( int i = 0; i < (int)((*iter)->ScheduleList.size()); i++ )
			{
				pSchedule = (*iter)->ScheduleList[i]; 
				m_pCurSchedule = pSchedule; 
				
				if( fCurTime >= pSchedule->fStartTime && fCurTime <= pSchedule->fEndTime )
				{
					outStrFile = pSchedule->strFilename; 
					break;  
				}
				else 
				{
					outStrFile = NULL; 
				}
			}
			*/
			
			if( outStrFile == NULL ) return false; 
		
			//3. 마지막으로 볼륨을 얻는다. 볼륨은 알파값으로 설정. 
			DWORD Volume = pinInfo->ColorTable[LockPosY][LockPosX]; 
			Volume = Volume>>24; 
			outVol = (float)Volume; 
			outVol = outVol/255.0f; //최소음과 최대음사이 0 ~ 1 
			
			if( outVol > 1.0f ) outVol = 1.0f; 
			if( outVol < 0.0f ) outVol = 0.0f; 

			return true; 
		}
	}	
	
	return false; 
}



void CAmbienceManager::GetTextureLockPos( int& outX, int& outY, D3DXVECTOR3 inPos )
{
	float Magnification = (float)SECTORSIZE / (float)AMB_TEXTURESIZE; 
	
	outX = (int)inPos.x % SECTORSIZE; 
	outY = (int)inPos.z % SECTORSIZE; 

	outX = (float)outX / Magnification; 
	outY = (float)outY / Magnification; 
		
	outY = AMB_TEXTURESIZE - outY -1; //Convert Texture LockPos
}



void CAmbienceManager::TestScreen( D3DXVECTOR3 Pos )
{
	LPDIRECT3DDEVICE8 pd3dDevice = BaseGraphicsLayer::GetDevice(); 
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
 	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE ); 
	pd3dDevice->SetVertexShader( D3DFVF_XYZRHW | D3DFVF_TEX1 ); 
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ); 
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE ); 
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE ); 
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE ); 
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
	
	D3DVIEWPORT8 TempViewport; 
	pd3dDevice->GetViewport( &TempViewport ); 
	
	float center = TempViewport.Width - 128; 

	TESTSCREEN_VERTEX pVertex[4]; 
	pVertex[0].x = center; 
	pVertex[0].y = 0; 
	pVertex[0].z = 0.0f; 
	pVertex[0].rhw = 1.0f; 
	pVertex[0].u = 0.0f; 
	pVertex[0].v = 0.0f; 

	pVertex[1].x = 128.0f + center; 
	pVertex[1].y = 0; 
	pVertex[1].z = 0.0f; 
	pVertex[1].rhw = 1.0f; 
	pVertex[1].u = 1.0f; 
	pVertex[1].v = 0.0f; 

	pVertex[2].x = center; 
	pVertex[2].y = 128.0f; 
	pVertex[2].z = 0.0f; 
	pVertex[2].rhw = 1.0f; 
	pVertex[2].u = 0.0f; 
	pVertex[2].v = 1.0f; 

	pVertex[3].x = 128.0f + center; 
	pVertex[3].y = 128.0f; 
	pVertex[3].z = 0.0f; 
	pVertex[3].rhw = 1.0f; 
	pVertex[3].u = 1.0f; 
	pVertex[3].v = 1.0f; 

	int LockPosX, LockPosY; 
	GetTextureLockPos( LockPosX, LockPosY, Pos ); 
	
	pd3dDevice->SetTexture( 0, m_pAmbTexture ); 
	pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, pVertex, sizeof(TESTSCREEN_VERTEX) ); 
		
	//Set things back to normal
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW ); 
	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ); 
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE ); 
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, TRUE ); 
		
	//준희를 위한 화살표그리기ㅎㅎㅎ
	RECT rect__; 
	SetRect( &rect__, 0, 0, 16, 16 ); 
	D3DXMATRIX mat; 
	D3DXMatrixTranslation( &mat, LockPosX+center, LockPosY, 0 ); 
	m_pMouseArrow->DrawTransform( m_pArrowTexture, &rect__, &mat, 0xffffffff ); 
}


void CAmbienceManager::AllStop()
{
	m_bFadeIn = false; 
	m_bFadeOut = false; 
	m_fFadeInVolume = 0.0f; 
	m_fFadeOutVolume = 0.0f; 
	strcpy( m_strFadeInFileName, "" ); 
	strcpy( m_strFadeOutFileName, "" ); 
	strcpy( m_strOldFile, "" ); 
	m_bAmbPlay = false; 
	strcpy( m_strAmbPlayFile, "" ); 
	m_fAmbVolume =0.0f; 
	m_fMasterVolume = 1.0f; 

	CSoundMgr::_GetInstance()->AllStopStreamingSound3D(); 
}

void CAmbienceManager::RandomPlaySound()
{

	if( m_pCurSchedule->iRandSndCycle < 0 ) return; 

	static DWORD dwOldTime; 
	static DWORD dwCurTime;  
	dwCurTime = GetTickCount(); 

	if( dwCurTime - dwOldTime > (DWORD) m_pCurSchedule->iRandSndCycle * 1000 )
	{
		if(m_pCurSchedule->RandomSndList.empty())
			return;

		char* strRndSndFile = m_pCurSchedule->RandomSndList[rand()%m_pCurSchedule->RandomSndList.size()]; 
		char ext[_MAX_EXT]; 
		_splitpath( strRndSndFile, NULL, NULL, NULL, ext );
		dwOldTime = dwCurTime; 
		char tempBuf[256]; 
		sprintf( tempBuf, "%s%s", AMBIENCEPATH, strRndSndFile ); 

		if( !strcmp(ext, ".ogg") )
		{
			CStreamingSound* pStream; 
			CSoundMgr::_GetInstance()->GetStreamingSound( pStream, tempBuf ); 
			pStream->Play(); 
			pStream->SetVolume( m_fMasterVolume ); 
		}
		else 
		{
			CSound* pSound; 
			CSoundMgr::_GetInstance()->GetSound( pSound, tempBuf, 1 ); 
			pSound->Play(); 
			pSound->SetVolume( m_fMasterVolume ); 
		}
	}
}
