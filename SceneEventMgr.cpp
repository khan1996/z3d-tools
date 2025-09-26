#include ".\sceneeventmgr.h"
#include "AmbienceManager.h"
#include "BgmManager.h"
#include "SoundMgr.h"
#include "GMMemory.h"

CSceneEventMgr::CSceneEventMgr(void)
{
	m_bEventMusic = false; 
	memset( m_strEventMusicName, 0, sizeof(m_strEventMusicName) ); 
}

CSceneEventMgr::~CSceneEventMgr(void)
{
}

bool CSceneEventMgr::IsEnableStructCamp()
{
	DWORD dwColor = CBgmManager::_GetInstance()->m_EventTable.dwColor; 
	dwColor = dwColor >> 24; 

	if( dwColor & EB1_DISABLE_CAMP ) 
	{
		return false; 
	}

	return true; 
}

bool CSceneEventMgr::IsSafetyZone()
{
	switch( CBgmManager::_GetInstance()->m_EventTable.nEvent )
	{
	case EK_BGM_LOOP_SAFE:
	case EK_BGM_ONCE_SAFE:
	case EK_NOTBGM_SAFE:
	case EK_BGM_TURN_AMB_SAFE:
	case EK_ESF_SAFE:
		return true; 
	}
	
	return false; 
}

void CSceneEventMgr::PlayEventMusic( char* strFileName, bool bLoop )
{
	CStreamingSound* pSound = NULL;
	CSoundMgr::_GetInstance()->GetStreamingSound( pSound, strFileName ); 
	if( pSound )
	{
		pSound->SetLooping( bLoop );
		pSound->SetVolume( CBgmManager::_GetInstance()->m_fMasterVolume ); 
		pSound->Play();
		m_bEventMusic = true; 
		strcpy( m_strEventMusicName, strFileName ); 
	}
}

/*
void CSceneEventMgr::PlayEvent3DMusic( char* strFileName, bool bLoop, D3DXVECTOR3& vPos, float fMinDist, float fMaxDist )
{
	CStreamingSound3D* pSound = NULL;
	CSoundMgr::_GetInstance()->GetStreamingSound3D( pSound, strFileName ); 
	if( pSound )
	{
		pSound->SetLooping( bLoop );
		pSound->SetMaxDistance( fMaxDist );
		pSound->SetMinDistance( fMinDist );
		pSound->SetPosition( vPos );
		pSound->SetVolume( CAmbienceManager::_GetInstance()->m_fMasterVolume ); 
		pSound->Play();
		m_bEventMusic = true; 
		strcpy( m_strEventMusicName, strFileName ); 
	}
	else 
	{
		char buf[MAX_PATH];
		sprintf( buf, "에러파일 : %s\n이벤트사운드파일은 1채널, ogg파일이어야만 합니다", strFileName );
		MessageBox( 0, buf, "사운드에러", 0 );
	}
}

void CSceneEventMgr::StopEvent3DMusic( char* strFileName )
{

	CStreamingSound3D* pSound = NULL;
	CSoundMgr::_GetInstance()->GetStreamingSound3D( pSound, strFileName ); 
	if( pSound )
	{
		pSound->Stop(); 
		m_bEventMusic = false; 
	}
}
*/

void CSceneEventMgr::StopEventMusic( char* strFileName )
{

	CStreamingSound* pSound = NULL;
	CSoundMgr::_GetInstance()->GetStreamingSound( pSound, strFileName );
	if( pSound )
	{
		pSound->Stop(); 
		m_bEventMusic = false; 
	}
}

void CSceneEventMgr::Update()
{

	if( m_bEventMusic )
	{
		CStreamingSound* pSound = NULL;
		CSoundMgr::_GetInstance()->GetStreamingSound( pSound, m_strEventMusicName ); 
		if( pSound )
		{
			if( !pSound->IsPlaying() )
			{
				m_bEventMusic = false; 
			}
		}
	}
}

bool CSceneEventMgr::IsVillage()
{
	return (CBgmManager::_GetInstance()->m_EventTable.nEvent >= 0); 
}