// Sound.cpp: implementation of the CSound class.
//
//////////////////////////////////////////////////////////////////////

#include "WaveLoader.h"
#include "Sound.h"
#include "SoundMgr.h"
#include "GMMemory.h"

CSound::CSound() : m_ppDSBuffer( NULL ),  
								 m_pWaveFile( NULL ), 		  
								 m_dwNumBuffers( 0 ), 	
								 m_dwDSBufferSize( 0 ), 	
								 m_bSound3D( false ), 
								 m_bLoop( false ),  
								 m_fVolume( VOLUME_MAX ), 
								 m_dwLastPlayTime( 0 ), 
								 m_dwLatestPlayIndex( 0 ), 
								 m_iPreserveScore( 0 )
{
}


CSound::~CSound()
{
	Destroy(); 
}


HRESULT CSound::RestoreBuffer( LPDIRECTSOUNDBUFFER pDSB, bool* pbWasRestored )
{
    HRESULT hr;

    if( pDSB == NULL )
        return CO_E_NOTINITIALIZED;
    if( pbWasRestored )
        *pbWasRestored = FALSE;

    DWORD dwStatus;
    if( FAILED( hr = pDSB->GetStatus( &dwStatus ) ) )
        return E_FAIL;

    if( dwStatus & DSBSTATUS_BUFFERLOST )
    {
        do 
        {
            hr = pDSB->Restore();
            if( hr == DSERR_BUFFERLOST )
                Sleep( 10 );
        }
        while( ( hr = pDSB->Restore() ) == DSERR_BUFFERLOST );

        if( pbWasRestored != NULL )
            *pbWasRestored = TRUE;

        return S_OK;
    }
    else
    {
        return S_FALSE;
    }
}


LPDIRECTSOUNDBUFFER CSound::GetFreeBuffer()
{
    if( m_ppDSBuffer == NULL )
        return FALSE; 

	DWORD i=0;
    for( ; i<m_dwNumBuffers; i++ )
    {
        if( m_ppDSBuffer[i] )
        {  
            DWORD dwStatus = 0;
            m_ppDSBuffer[i]->GetStatus( &dwStatus );
            if ( ( dwStatus & DSBSTATUS_PLAYING ) == 0 )
                break;
        }
    }

    if( i != m_dwNumBuffers )
	{
		m_dwLatestPlayIndex = i; 
		return m_ppDSBuffer[ i ];
	}
    else
        return m_ppDSBuffer[ rand() % m_dwNumBuffers ];
}


LPDIRECTSOUNDBUFFER CSound::GetBuffer( DWORD dwIndex )
{
    if( m_ppDSBuffer == NULL )
        return NULL;
    if( dwIndex >= m_dwNumBuffers )
        return NULL;

    return m_ppDSBuffer[dwIndex];
}


void CSound::Stop()
{
    HRESULT hr = 0;

    for( DWORD i=0; i<m_dwNumBuffers; i++ )
        hr |= m_ppDSBuffer[i]->Stop();

	if( FAILED(hr) )
		MessageBox( NULL, "hr |= m_ppDSBuffer[i]->Stop()", "CSound::Stop", MB_OK ); 
}


void CSound::Reset()
{
    HRESULT hr = 0;

    for( DWORD i=0; i<m_dwNumBuffers; i++ )
        hr |= m_ppDSBuffer[i]->SetCurrentPosition( 0 );

	if( FAILED(hr) )
		MessageBox( NULL, "hr |= m_ppDSBuffer[i]->SetCurrentPosition", 
		"CSound::Stop", MB_OK ); 
}


bool CSound::IsPlaying()
{
    bool bIsPlaying = false;

    if( m_ppDSBuffer == NULL )
        return false; 

    for( DWORD i=0; i<m_dwNumBuffers; i++ )
    {
        if( m_ppDSBuffer[i] )
        {  
            DWORD dwStatus = 0;
            m_ppDSBuffer[i]->GetStatus( &dwStatus );
            bIsPlaying |= ( ( dwStatus & DSBSTATUS_PLAYING ) != 0 );
        }
    }

    return bIsPlaying;
}


bool CSound::Create( const char* strFileName, DWORD dwNumBuffers )
{
	DWORD i; 
	DWORD dwDSBufferSize = NULL; 
	m_dwNumBuffers = dwNumBuffers; 
	
	LPDIRECTSOUND8 pDS = CSoundMgr::_GetInstance()->GetDirectSound(); 
	m_ppDSBuffer = new LPDIRECTSOUNDBUFFER8[dwNumBuffers];
	for( i = 0; i < dwNumBuffers; ++i )
	{
		m_ppDSBuffer[i] = 0;
	}
	m_pWaveFile = new CWaveLoader; 

	if( FAILED(m_pWaveFile->Open((LPSTR)strFileName, NULL, WAVEFILE_READ)) ) 
	{
		char buf[MAX_PATH] = {0}; 
		sprintf( buf, "해당 파일을 열수가 없습니다. : %s \n CSound::Create", strFileName ); 
		MessageBox( NULL, buf, "사운드에러", MB_OK ); 
		SAFE_DELETE(m_pWaveFile); 
		return false; 
	}

	m_dwDSBufferSize = m_pWaveFile->GetSize(); 
	
	WAVEFORMATEX	wfx;
	ZeroMemory( &wfx, sizeof(WAVEFORMATEX) );
	wfx.nChannels = m_pWaveFile->m_pwfx->nChannels;
	wfx.nSamplesPerSec = m_pWaveFile->m_pwfx->nSamplesPerSec;
	wfx.wBitsPerSample = m_pWaveFile->m_pwfx->wBitsPerSample;
	wfx.wFormatTag = WAVE_FORMAT_PCM; 
	wfx.nBlockAlign = wfx.wBitsPerSample / 8 * wfx.nChannels;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

	if( m_bSound3D && wfx.nChannels != 1 )
	{
		MessageBox( NULL, "3DSound Only Acquire One Channel", "Sound::Create", MB_OK ); 
		wfx.nChannels = 1;
		Destroy(); 
		return false; 
	}

	//Create Buffer 
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize          = sizeof(DSBUFFERDESC);
    dsbd.dwFlags         = DSBCAPS_CTRLVOLUME; 
	 if( m_bSound3D ) dsbd.dwFlags |= DSBCAPS_CTRL3D|DSBCAPS_MUTE3DATMAXDISTANCE; 
    dsbd.dwBufferBytes   = m_dwDSBufferSize;
    dsbd.guid3DAlgorithm = m_bSound3D ? CSoundMgr::_GetInstance()->GetInitProp()->m_guidSound3DAlgorithm : GUID_NULL;
    dsbd.lpwfxFormat     = &wfx;

	
	//========================================
	LPDIRECTSOUNDBUFFER tempBuffer;
	if( FAILED(pDS->CreateSoundBuffer( &dsbd, &tempBuffer, NULL )) )
	{
		//MessageBox( NULL, "pDS->CreateSoundBuffer( &dsbd, &tempBuffer, NULL )", "CSound::Create", MB_OK ); 
		SAFE_RELEASE( tempBuffer ); 
		Destroy(); 
		return false; 
	}

	if( FAILED(tempBuffer->QueryInterface( IID_IDirectSoundBuffer8, (LPVOID*)m_ppDSBuffer )) )
	{
		//MessageBox( NULL, "pDS->CreateSoundBuffer( &dsbd, &tempBuffer, NULL )", "CSound::Create", MB_OK ); 
		SAFE_RELEASE( tempBuffer ); 
		Destroy(); 
		return false; 
	}
	
	SAFE_RELEASE(tempBuffer)
	
	for( i = 1; i < dwNumBuffers; i++ )
	{
		if( FAILED(pDS->DuplicateSoundBuffer( m_ppDSBuffer[0], (LPDIRECTSOUNDBUFFER*)&m_ppDSBuffer[i])) )
		{
			//MessageBox( NULL, "pDS->DuplicateSoundBuffer( m_ppDSBuffer[0], &m_ppDSBuffer[i])", 
			//	"CSound::Create", MB_OK );
			Destroy();
			return false; 
		}
	}

	FillBuffer( m_ppDSBuffer[0] ); 
   //===============================================
 

	/*
	if( FAILED(pDS->CreateSoundBuffer( &dsbd, &m_ppDSBuffer[0], NULL )) )
	{
		//MessageBox( NULL, "pDS->CreateSoundBuffer( &dsbd, &m_ppDSBuffer[0], NULL )", 
		//	"CSound::Create", MB_OK ); 
		Destroy(); 
		return false; 
	}

	for( i = 1; i < dwNumBuffers; i++ )
    {
		if( FAILED(pDS->DuplicateSoundBuffer( m_ppDSBuffer[0], &m_ppDSBuffer[i])) )
       {
		   //MessageBox( NULL, "pDS->DuplicateSoundBuffer( m_ppDSBuffer[0], &m_ppDSBuffer[i])", 
			//	"CSound::Create", MB_OK );
			Destroy(); 
			return false; 
		}
    }

	FillBuffer( m_ppDSBuffer[0] ); 
	*/

	return true; 
}


void CSound::Destroy()
{
	for( DWORD i = 0; i < m_dwNumBuffers; i++ )
	{
		SAFE_RELEASE( m_ppDSBuffer[i] ); 
	}
	
	SAFE_DELETE_ARRAY( m_ppDSBuffer );
	SAFE_DELETE( m_pWaveFile ); 
}


void CSound::Play()
{
	if( 0 == this )
	{
		return;
	}

	m_dwLastPlayTime = timeGetTime(); 

	bool bRestored;
	LPDIRECTSOUNDBUFFER pDSB = GetFreeBuffer();

    if( FAILED(RestoreBuffer(pDSB, &bRestored)) )
	{
		MessageBox(NULL, "FAILED(RestoreBuffer(pDSB, &bRestored))", 
		"CSound::Play", MB_OK );
		return;
	}

	if( bRestored )
	{
		FillBuffer( pDSB ); 
	}
    
	SetVolume(pDSB, m_fVolume);

	pDSB->Play(0,0, m_bLoop ? DSBPLAY_LOOPING:0 );
}


void CSound::FillBuffer( LPDIRECTSOUNDBUFFER pDSBuffer )
{
    VOID*   pDSLockedBuffer      = NULL; 
    DWORD   dwDSLockedBufferSize = 0;    
    DWORD   dwWavDataRead        = 0;    

	if( FAILED(RestoreBuffer( pDSBuffer, NULL)) )
	{
		MessageBox( NULL, "FAILED(RestoreBuffer( pDSBuffer, NULL))", 
			"CSound::FillBuffer", MB_OK ); 
		return;
	}
       
    if( FAILED(pDSBuffer->Lock(0, 0, &pDSLockedBuffer, &dwDSLockedBufferSize, 
                                 NULL, NULL, DSBLOCK_ENTIREBUFFER ) ) )
	{
		MessageBox( NULL, "FAILED(pDSBuffer->Lock", "CSound::FillBuffer", MB_OK ); 
		return; 
	}

    m_pWaveFile->ResetFile();

    if( FAILED(m_pWaveFile->Read((BYTE*)pDSLockedBuffer, dwDSLockedBufferSize, &dwWavDataRead)) )
	{
		MessageBox( NULL, "FAILED(m_pWaveFile->Read", "CSound::FillBuffer", MB_OK ); 
		return; 
	}

    if( dwWavDataRead == 0 )
    {
		MessageBox( NULL, "dwWavDataRead == 0", "CSound::FillBuffer", MB_OK ); 
		return; 
    }

    pDSBuffer->Unlock( pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0 );
}

void CSound::SetVolume(LPDIRECTSOUNDBUFFER pDSBuffer, float fVol )
{
	if( !pDSBuffer ) return; 

	m_fVolume = Clamp( fVol, VOLUME_MIN, VOLUME_MAX ); 
	float fMasterVol = CSoundMgr::_GetInstance()->GetVolume(); 
	fMasterVol = Clamp( fMasterVol, VOLUME_MIN, VOLUME_MAX ); 
	int iVol = LinearToLogVol(m_fVolume * fMasterVol);

	if(iVol < -10000)
		iVol = -10000;

	if( FAILED(pDSBuffer->SetVolume(iVol)) )
	{
		MessageBox( NULL, "FAILED(pDSBuffer->SetVolume(iVol))", 
			"CSound::SetVolume", MB_OK ); 
	}
}

void CSound::SetVolume( float fVol )
{
	if( !m_ppDSBuffer ) return; 

	m_fVolume = Clamp( fVol, VOLUME_MIN, VOLUME_MAX ); 
}
