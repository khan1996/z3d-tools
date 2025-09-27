// SoundMgr.cpp: implementation of the CSoundMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "SoundMgr.h"
#include "GMMemory.h"

CSoundMgr* CSoundMgr::ms_pSoundMgr = NULL;

CSoundMgr* CSoundMgr::_GetInstance()
{
	if( ms_pSoundMgr == NULL )
	{
		ms_pSoundMgr = new CSoundMgr(); 
	}
	
	return ms_pSoundMgr; 
}
 
DWORD WINAPI _TimeEventThread( LPVOID lp )
{
	CSoundMgr::_GetInstance()->ThreadFunc(); 
	return 0;
}

bool CSoundMgr::Init( SoundMgrInit& Init )
{
	//============================
	// Initialize variables
	//============================
	m_bInit = false; 
	m_pDS				= NULL;
	m_pListener			= NULL; 
	m_hTimeEventThread	= NULL;   
	m_hDestroyEvent     = NULL; 
	m_dwTimeEventID		= NULL; 
	m_fMasterVolume		= VOLUME_MAX; 
	
	memcpy( &m_InitProp, &Init, sizeof(SoundMgrInit) ); 
	if( m_InitProp.m_hWnd == 0 )
	{
		MessageBox(NULL, "Not Init hWnd!!", "CSoundMgr::Init", MB_OK); 
		return false; 
	}

	//============================
	// Create DirectSound 
	//============================
	if( FAILED(DirectSoundCreate8(NULL, &m_pDS, NULL)) )
	{
		MessageBox(NULL, "Failed to initialize DirectSound. \n CSoundMgr::Init",  
			"DirectSound Device Error", MB_OK); 
		SAFE_RELEASE( m_pDS ); 
		return false; 
	}

	if( FAILED(m_pDS->SetCooperativeLevel(m_InitProp.m_hWnd, /*DSSCL_PRIORITY*/ DSSCL_EXCLUSIVE)) )
	{
		MessageBox(NULL, "Failed to set the DirectSound cooperative levels. \n CSoundMgr::Init", 
			"DirectSound Device Error", MB_OK);
		SAFE_RELEASE( m_pDS ); 
		return false; 
	}

	if( !SetPrimaryBufferFormat(m_InitProp.m_dwPrimaryChannels, 
		m_InitProp.m_dwPrimaryFrequency, m_InitProp.m_dwPrimaryBitRate) )
	{
		MessageBox( NULL, "Failed to create DirectSound buffer. \n CSoundMgr::Init", 
			"DirectSound Device Error", MB_OK );
		SAFE_RELEASE( m_pDS );
		return false; 
	}

	//==============================
	// Get Caps
	//==============================
	ZeroMemory( &m_Caps, sizeof(DSCAPS) ); 
	m_Caps.dwSize = sizeof(DSCAPS); 
	if( FAILED(m_pDS->GetCaps(&m_Caps)) )
	{
		MessageBox( NULL, "Failed to initialize sound driver. \n CSoundMgr::Init", 
			"DirectSound Device Error", MB_OK );
	}
		
	//==============================
	// Create Listener 
	//==============================
	m_pListener = new CListener( m_pDS );

	//==============================
	// Create thread
	//==============================
	m_hTimeEventThread = CreateThread( NULL, 0, _TimeEventThread, NULL, 0, &m_dwTimeEventID );

	//==============================
	// Init CriticalSection
	//==============================
	ZeroMemory(&m_csSoundUpdate, sizeof(CRITICAL_SECTION));
	InitializeCriticalSection(&m_csSoundUpdate);

	//초기화성공
	m_bInit	= true; 

	return true; 
}


bool CSoundMgr::SetPrimaryBufferFormat(DWORD dwPrimaryChannels, DWORD dwPrimaryFreq, DWORD dwPrimaryBitRate)
{
	LPDIRECTSOUNDBUFFER pDSBPrimary = NULL; 
	
	// Get the primary buffer 
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER;
    dsbd.dwBufferBytes = 0;
    dsbd.lpwfxFormat   = NULL;

	if(FAILED(m_pDS->CreateSoundBuffer(&dsbd, &pDSBPrimary, NULL))) return false; 
	
	WAVEFORMATEX wfx;
   ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
   wfx.wFormatTag      = (WORD) WAVE_FORMAT_PCM; 
    wfx.nChannels       = (WORD) dwPrimaryChannels; 
    wfx.nSamplesPerSec  = (DWORD) dwPrimaryFreq; 
    wfx.wBitsPerSample  = (WORD) dwPrimaryBitRate; 
    wfx.nBlockAlign     = (WORD) (wfx.wBitsPerSample / 8 * wfx.nChannels); 
    wfx.nAvgBytesPerSec = (DWORD) (wfx.nSamplesPerSec * wfx.nBlockAlign); 

	if( FAILED(pDSBPrimary->SetFormat(&wfx)) )
	{
		SAFE_RELEASE( pDSBPrimary ); 
		return false; 
	}
	
	SAFE_RELEASE( pDSBPrimary ); 
	return true; 
}


void CSoundMgr::ReleaseAllData()
{
	m_hDestroyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_bInit = false; 
	WaitForSingleObject( m_hDestroyEvent, INFINITE );
	
	SOUNDMAP::iterator iter_snd; 
	SOUND3DMAP::iterator iter_snd3d; 
	STREAMINGMAP::iterator iter_strm; 
	STREAMING3DMAP::iterator iter_strm3d; 
	
	for( iter_snd = m_SoundMap.begin(); iter_snd != m_SoundMap.end(); iter_snd++ )
	{
		SAFE_DELETE( iter_snd->second ); 
	}

	for( iter_snd3d = m_Sound3DMap.begin(); iter_snd3d != m_Sound3DMap.end(); iter_snd3d++ )
	{
		SAFE_DELETE( iter_snd3d->second ); 
	}

	for( iter_strm = m_StreamingMap.begin(); iter_strm != m_StreamingMap.end(); iter_strm++ )
	{
		SAFE_DELETE( iter_strm->second ); 
	}

	for( iter_strm3d = m_Streaming3DMap.begin(); iter_strm3d != m_Streaming3DMap.end(); iter_strm3d++ )
	{
		SAFE_DELETE( iter_strm3d->second ); 
	}

	m_SoundMap.clear(); 
	m_Sound3DMap.clear(); 
	m_StreamingMap.clear(); 
	m_Streaming3DMap.clear();

	CloseHandle( m_hDestroyEvent ); 
	CloseHandle( m_hTimeEventThread );
	DeleteCriticalSection( &m_csSoundUpdate ); 
	
	SAFE_DELETE( m_pListener ); 
	SAFE_RELEASE( m_pDS ); 
}


bool CSoundMgr::GetSound( CSound*& pSound, char* strFileName, DWORD dwNumBuffers )
{
	SimpleCircleBuffer(SOUND); 
	SOUNDMAP::iterator iter; 
	DWORD dwHashID = GetHashID(strFileName); 
	iter = m_SoundMap.find( dwHashID ); 
	
	if( iter != m_SoundMap.end() )
	{
		pSound = iter->second; 
		pSound->SetVolume(1);
		pSound->m_iPreserveScore += 50; 
		return true; 
	}

	pSound = NULL;
	pSound = new CSound(); 

	if( pSound )
	{
		if( !pSound->Create( strFileName, dwNumBuffers ) )
		{
			SAFE_DELETE( pSound ); 
			return false; 
		}

		pSound->SetVolume(1);
		pSound->m_iPreserveScore += 50; 
		m_SoundMap.insert( SOUNDMAP::iterator::value_type(dwHashID, pSound) ); 
	}

	return true; 
}


bool CSoundMgr::GetSound3D( CSound3D*& pSound3D, const char* strFileName, DWORD dwNumBuffers )
{
	SimpleCircleBuffer(SOUND3D); 
	SOUND3DMAP::iterator iter; 
	DWORD dwHashID = GetHashID(strFileName); 
	iter = m_Sound3DMap.find( dwHashID ); 

	if( iter != m_Sound3DMap.end() )
	{
		pSound3D = iter->second; 
		pSound3D->SetVolume(1);
		pSound3D->m_pSound->m_iPreserveScore += 50; 
		return true; 
	}

	pSound3D = NULL; 
	pSound3D = new CSound3D(); 

	if( pSound3D )
	{
		if( !pSound3D->Create( strFileName, dwNumBuffers ) )
		{
			SAFE_DELETE( pSound3D ); 
			return false; 
		}

		pSound3D->SetVolume(1);
		pSound3D->m_pSound->m_iPreserveScore += 50; 
		m_Sound3DMap.insert( SOUND3DMAP::iterator::value_type(dwHashID, pSound3D) ); 
	}

	return true; 
}


void CSoundMgr::GetStreamingSound( CStreamingSound*& pStreamingSound, char* strFileName )
{
	SimpleCircleBuffer(STREAM); 
	STREAMINGMAP::iterator iter; 
	DWORD dwHashID = GetHashID(strFileName); 
	iter = m_StreamingMap.find( dwHashID ); 

	if( iter != m_StreamingMap.end() )
	{
		pStreamingSound = iter->second; 
		pStreamingSound->SetVolume(1);
		return; 
	}

	pStreamingSound = new CStreamingSound(); 

	if( !pStreamingSound->Create( strFileName ) )
	{
		SAFE_DELETE(pStreamingSound); 
		return; 
	}

	pStreamingSound->SetVolume(1);
	m_StreamingMap.insert( STREAMINGMAP::iterator::value_type(dwHashID, pStreamingSound) ); 
}


void CSoundMgr::GetStreamingSound3D( CStreamingSound3D*& pStreamingSound3D, char* strFileName )
{
	SimpleCircleBuffer(STREAM3D); 
	STREAMING3DMAP::iterator iter; 
	DWORD dwHashID = GetHashID(strFileName); 
	iter = m_Streaming3DMap.find( dwHashID ); 

	if( iter != m_Streaming3DMap.end() )
	{
		pStreamingSound3D = iter->second; 
		pStreamingSound3D->SetVolume(1);
		return; 
	}

	pStreamingSound3D = new CStreamingSound3D(); 
	if( !pStreamingSound3D->Create(strFileName) )
	{
		SAFE_DELETE(pStreamingSound3D); 
		return; 
	}

	pStreamingSound3D->SetVolume(1);
	m_Streaming3DMap.insert( STREAMING3DMAP::iterator::value_type(dwHashID, pStreamingSound3D) ); 
}


void CSoundMgr::InsertStream( CStreamingSound* pStreamSound )
{
	m_StreamProcess.push_back( pStreamSound ); 
}


void CSoundMgr::RemoveStream( CStreamingSound* pStreamSound )
{
	m_StreamRemoval.push_back( pStreamSound ); 
}


void CSoundMgr::ServiceStreamingBuffers()
{
	STREAMLIST::iterator itr;
	for(itr = m_StreamRemoval.begin(); itr != m_StreamRemoval.end(); itr++)
	{
		STREAMLIST::iterator itor = find(m_StreamProcess.begin(), m_StreamProcess.end(), *itr);
		if(itor != m_StreamProcess.end())
			m_StreamProcess.erase(itor);
	}

	m_StreamRemoval.clear();
	
	for(itr = m_StreamProcess.begin(); itr != m_StreamProcess.end(); itr++)
	{
		(*itr)->ServiceBuffer();
	}
}


DWORD CSoundMgr::GetUseMemory()
{
	DWORD dwSize = 0; 
	SOUNDMAP::iterator iter_snd; 
	SOUND3DMAP::iterator iter_snd3d; 
	STREAMINGMAP::iterator iter_strm; 
	STREAMING3DMAP::iterator iter_strm3d; 

	for( iter_snd = m_SoundMap.begin(); iter_snd != m_SoundMap.end(); iter_snd++ )
	{
		dwSize += iter_snd->second->GetAllBufferSize(); 
	}

	for( iter_snd3d = m_Sound3DMap.begin(); iter_snd3d != m_Sound3DMap.end(); iter_snd3d++ )
	{
		dwSize += iter_snd3d->second->GetAllBufferSize(); 
	}

	for( iter_strm = m_StreamingMap.begin(); iter_strm != m_StreamingMap.end(); iter_strm++ )
	{
		dwSize += iter_strm->second->GetBufferSize(); 
	}

	for( iter_strm3d = m_Streaming3DMap.begin(); iter_strm3d != m_Streaming3DMap.end(); iter_strm3d++ )
	{
		dwSize += iter_strm3d->second->GetBufferSize(); 
	}

	return dwSize; 
}


void CSoundMgr::SimpleCircleBuffer( DWORD dwType )
{
	switch( dwType )
	{
	case SOUND:
		{
			if( m_SoundMap.size() > m_InitProp.m_dwLimitSound )
			{
				SOUNDMAP::iterator iter_snd; 
//				DWORD dwFlowTime; 
				for( iter_snd = m_SoundMap.begin(); iter_snd != m_SoundMap.end();  )
				{
					if( !iter_snd->second->IsPlaying() )
					{
						--iter_snd->second->m_iPreserveScore; 
						if( iter_snd->second->m_iPreserveScore < 0 )
						{
							SOUNDMAP::iterator iter_delete;
							iter_delete = iter_snd;
							++iter_snd;
							SAFE_DELETE( iter_delete->second ); 
							m_SoundMap.erase( iter_delete );
							if( m_SoundMap.size() <= m_InitProp.m_dwLimitSound ) break;
							continue;
						}
					}
					++iter_snd;
				}
			}
		}
		break; 

	case SOUND3D:
		{
			if( m_Sound3DMap.size() > m_InitProp.m_dwLimitSound3D )
			{
				SOUND3DMAP::iterator iter_snd3d; 
//				DWORD dwFlowTime; 
				for( iter_snd3d = m_Sound3DMap.begin(); iter_snd3d != m_Sound3DMap.end(); )
				{
					if( !iter_snd3d->second->IsPlaying() ) 
					{
						--iter_snd3d->second->m_pSound->m_iPreserveScore;  
						if( iter_snd3d->second->m_pSound->m_iPreserveScore < 0 )
						{
							SOUND3DMAP::iterator iter_delete;
							iter_delete = iter_snd3d;
							++iter_snd3d;
							SAFE_DELETE( iter_delete->second );
							m_Sound3DMap.erase( iter_delete );
							if( m_Sound3DMap.size() <= m_InitProp.m_dwLimitSound3D ) break;
							continue;
						}
					}
					++iter_snd3d;
				}
			}
		}
		break; 

	case STREAM:
		{
			if( m_StreamingMap.size() > m_InitProp.m_dwLimitStreaming )
			{
				STREAMINGMAP::iterator iter_strm; 
				for( iter_strm = m_StreamingMap.begin(); iter_strm != m_StreamingMap.end(); )
				{
					if( !iter_strm->second->IsPlaying() )
					{
						STREAMINGMAP::iterator iter_delete;
						iter_delete = iter_strm;
						++iter_strm;
						SAFE_DELETE( iter_delete->second );
						m_StreamingMap.erase( iter_delete );
						if( m_StreamingMap.size() <= m_InitProp.m_dwLimitStreaming ) break;
						continue; 
					}

					++iter_strm;
				}
			}
		}
		break; 
		
	case STREAM3D:
		{
			if( m_Streaming3DMap.size() > m_InitProp.m_dwLimitStreaming )
			{
				STREAMING3DMAP::iterator iter_strm3d; 
				for( iter_strm3d = m_Streaming3DMap.begin(); iter_strm3d != m_Streaming3DMap.end(); )
				{
					if( !iter_strm3d->second->IsPlaying() )
					{
						STREAMING3DMAP::iterator iter_delete;
						iter_delete = iter_strm3d;
						++iter_strm3d;
						SAFE_DELETE( iter_delete->second );
						m_Streaming3DMap.erase( iter_delete );
						if( m_Streaming3DMap.size() <= m_InitProp.m_dwLimitStreaming ) break;
						continue;
					}

					++iter_strm3d;
				}
			}

		}
		break; 
	}
}


DSCAPS CSoundMgr::GetCaps()
{
	return m_Caps; 
}


void CSoundMgr::SetVolume( float fVol )
{
	m_fMasterVolume = fVol;
}


float CSoundMgr::GetVolume() 
{ 
	return m_fMasterVolume; 
}


void CSoundMgr::_Destroy()
{
	if( ms_pSoundMgr )
	{
		ms_pSoundMgr->ReleaseAllData(); 
	}

	SAFE_DELETE( ms_pSoundMgr ); 
}


void CSoundMgr::AllStopStreamingSound()
{
	STREAMINGMAP::iterator iter; 
	for( iter = m_StreamingMap.begin(); iter != m_StreamingMap.end(); iter++ )
	{
		iter->second->Stop(); 
	}
}

void CSoundMgr::AllStopStreamingSound3D()
{
	STREAMING3DMAP::iterator iter; 
	for( iter = m_Streaming3DMap.begin(); iter != m_Streaming3DMap.end(); iter++ )
	{
		iter->second->Stop(); 
	}
}

DWORD CSoundMgr::GetHashID( const char* strFileName )
{
	DWORD id = 0;
	int iLength = (int)strlen(strFileName);

	for(int i=0;i < iLength; ++i) 
	{
		id += (( i + 1) * strFileName[i]);
	}	
	return id;
}

void CSoundMgr::ThreadFunc()
{
	while(1)
	{
		Sleep( 50 );

		if( !m_bInit )
		{
			SetEvent( m_hDestroyEvent );
			break; 
		}

		//-----------------------------------------
		EnterCriticalSection( &m_csSoundUpdate );
		//-----------------------------------------
		static int iServiceStreams = 0;
		if((iServiceStreams++) % 12 == 1) ServiceStreamingBuffers();
		m_pListener->Update(); 
		//-----------------------------------------
		LeaveCriticalSection( &m_csSoundUpdate );
		//-----------------------------------------
	}
}