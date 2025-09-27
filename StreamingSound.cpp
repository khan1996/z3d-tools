// StreamingSound.cpp: implementation of the CStreamingSound class.
//
//////////////////////////////////////////////////////////////////////

#include "StreamingSound.h"
#include "Sound.h"
#include "WaveLoader.h"
#include "OggLoader.h"
#include "SoundMgr.h"
#include "GMMemory.h"


CStreamingSound::CStreamingSound() : m_pDSBuffer(NULL), 
																 m_pOggFile(NULL), 
																 m_dwDSBufferSize(0), 
																 m_dwDataCursor(0), 
																 m_dwBytesPlayed(0), 
																 m_dwLastReadPos(0), 
																 m_bPlaying(false), 
																 m_bRemoveStream(true), 
																 m_bLoop(false), 
																 m_bSound3D(false), 
																 m_fVolume(VOLUME_MAX)
{
}


CStreamingSound::~CStreamingSound()
{
	Destroy(); 
}


void CStreamingSound::Destroy()
{
	EnterCriticalSection(&CSoundMgr::_GetInstance()->GetUpdateCS());
	CSoundMgr::_GetInstance()->RemoveStream(this);
	LeaveCriticalSection(&CSoundMgr::_GetInstance()->GetUpdateCS());

	SAFE_RELEASE( m_pDSBuffer ); 
	SAFE_DELETE( m_pOggFile );
}


HRESULT CStreamingSound::Reset()
{
	m_dwDataCursor = 0;
	m_dwBytesPlayed = 0;
	m_dwLastReadPos = 0;
	m_bRemoveStream = false;

    m_pOggFile->Reset();
	
    return m_pDSBuffer->SetCurrentPosition( 0L );  
}


bool CStreamingSound::Create( char* strFileName )
{
	LPDIRECTSOUND8 pDS = CSoundMgr::_GetInstance()->GetDirectSound(); 

    m_pOggFile = new COggLoader();
    if( !m_pOggFile->Open( strFileName ) )
	{
//		char buf[MAX_PATH] = {0}; 
//		sprintf( buf, "해당파일을 열수가 없습니다. %s \n CStreamingSound::Create", strFileName ); 
//		MessageBox( NULL, buf, "사운드에러", MB_OK ); 

		return false; 
	}
	m_dwDSBufferSize = m_pOggFile->m_WaveFormatEx.nAvgBytesPerSec*3; //3초 버퍼생성

	if( m_bSound3D && m_pOggFile->GetFormat()->nChannels > 1 )
	{
		MessageBox( NULL, "3DSound Only Acquire One Channel", "CStreamingSound::Create", MB_OK ); 
		SAFE_DELETE(m_pOggFile); 
		return false;
	}

    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize = sizeof(DSBUFFERDESC);
    dsbd.dwFlags = DSBCAPS_CTRLVOLUME|DSBCAPS_GETCURRENTPOSITION2; 
	 if( m_bSound3D ) dsbd.dwFlags |= DSBCAPS_CTRL3D|DSBCAPS_MUTE3DATMAXDISTANCE; 
	 dsbd.dwBufferBytes   = m_dwDSBufferSize;
    dsbd.guid3DAlgorithm = m_bSound3D ? 
		CSoundMgr::_GetInstance()->GetInitProp()->m_guidSound3DAlgorithm:GUID_NULL;
    dsbd.lpwfxFormat     = &m_pOggFile->m_WaveFormatEx;


	LPDIRECTSOUNDBUFFER tempBuffer;
	if( FAILED(pDS->CreateSoundBuffer( &dsbd, &tempBuffer, NULL )) )
	{
		SAFE_RELEASE(tempBuffer); 
		Destroy(); 
		return false; 
	}

	if( FAILED(tempBuffer->QueryInterface( IID_IDirectSoundBuffer8, (LPVOID*)&m_pDSBuffer )) )
	{
		SAFE_RELEASE(tempBuffer); 
		Destroy(); 
		return false; 
	}
	tempBuffer->Release();

	/*
    if( FAILED(pDS->CreateSoundBuffer(&dsbd, &m_pDSBuffer, NULL) ))
    {
		//MessageBox( NULL, "CreateSoundBuffer", "streaming", MB_OK ); 
		SAFE_RELEASE( m_pDSBuffer ); 
		SAFE_DELETE( m_pOggFile );
		return; 
    }
	*/

	FillBuffer(); 
	return true; 
}


void CStreamingSound::Play()
{
	if( m_bPlaying ) return; 

	EnterCriticalSection(&CSoundMgr::_GetInstance()->GetUpdateCS());
	m_bRemoveStream = false;
	CSoundMgr::_GetInstance()->InsertStream(this);
	LeaveCriticalSection(&CSoundMgr::_GetInstance()->GetUpdateCS());
	
	if( FAILED(m_pDSBuffer->Play(0,0,DSBPLAY_LOOPING)) )
	{
//		MessageBox( NULL, "m_pDSBuffer->Play(0,0,0)", "Play()", MB_OK ); 
		m_bPlaying = false;
		return;
	}

	m_bPlaying = true;
}


HRESULT CStreamingSound::RestoreBuffer( LPDIRECTSOUNDBUFFER pDSB, BOOL* pbWasRestored )
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


void CStreamingSound::ServiceBuffer()
{
	if(m_bRemoveStream)
	{
		if(!m_pDSBuffer || !m_pOggFile) 
			return;
		
		m_bRemoveStream = false;
		CSoundMgr::_GetInstance()->RemoveStream(this);
		m_pOggFile->Reset();
		Reset(); 
		FillBuffer(); 
	
		return;
	}

	if(!m_pDSBuffer || !m_pOggFile)
		return;

	DWORD dwReadCursor;
	DWORD dwWriteCursor;
	if( FAILED(m_pDSBuffer->GetCurrentPosition(&dwReadCursor, &dwWriteCursor)) )
	{
		// edith 2009.09.16 사운드 오류를 발생하는 유저가 있어서 강제로 리턴하기
//		MessageBox( NULL, "FAILED(m_pDSBuffer->GetCurrentPosition(&dwReadCursor, &dwWriteCursor))", 
//		"ServiceBuffer()", MB_OK ); 
		return;
	}

	if(dwReadCursor > m_dwLastReadPos)
		m_dwBytesPlayed += dwReadCursor - m_dwLastReadPos;
	else
		m_dwBytesPlayed += (m_dwDSBufferSize - m_dwLastReadPos) + dwReadCursor;

	if(m_dwBytesPlayed >= m_pOggFile->GetSize())
	{
		if( m_bLoop )
		{
			m_dwBytesPlayed -= m_pOggFile->GetSize();
		}
		else
		{
			Stop();
			m_bPlaying = false; 
			return;
		}
	}

	DWORD dwDataToCopy;
	if(m_dwDataCursor < dwReadCursor)
		dwDataToCopy = dwReadCursor - m_dwDataCursor;
	else
		dwDataToCopy = (m_dwDSBufferSize - m_dwDataCursor) + dwReadCursor;

	if(dwDataToCopy > (m_dwDSBufferSize / 2))
		dwDataToCopy = m_dwDSBufferSize / 2;

	LPVOID  pPtr1; 
	DWORD   dwBytes1; 
	LPVOID  pPtr2; 
	DWORD   dwBytes2; 
	if( FAILED(m_pDSBuffer->Lock(m_dwDataCursor, dwDataToCopy, 
		&pPtr1, &dwBytes1, &pPtr2, &dwBytes2, 0)) )
	{
		// edith 2009.09.16 사운드 오류를 발생하는 유저가 있어서 강제로 리턴하기
//		MessageBox( NULL, "m_pDSBuffer->Lock(m_dwDataCursor, dwDataToCopy, &pPtr1, &dwBytes1, &pPtr2, &dwBytes2, 0)", 
//			"ServiceBuffer()", MB_OK );
		return;
	}

	if(m_pOggFile->IsEOF())
	{
		memset(pPtr1, GetSilenceData(), dwBytes1);
		if(pPtr2) memset(pPtr2, GetSilenceData(), dwBytes2);
		m_dwDataCursor += (dwBytes1 + dwBytes2);
	}
	else
	{
		DWORD dwBytesRead = 0;
		if( FAILED(m_pOggFile->Read((BYTE*)pPtr1, dwBytes1, &dwBytesRead)) )
		{
			// edith 2009.09.16 사운드 오류를 발생하는 유저가 있어서 강제로 리턴하기
//			MessageBox( NULL, "m_pWaveFile->Read((BYTE*)pPtr1, dwBytes1, &dwBytesRead)", 
//				"ServiceBuffer()", MB_OK ); 

			m_pDSBuffer->Unlock(pPtr1, dwBytes1, pPtr2, dwBytes2);
			return;
		}
		
		m_dwDataCursor += dwBytesRead;
		
		if(pPtr2 && (dwBytes1 == dwBytesRead))
		{
			if( FAILED(m_pOggFile->Read((BYTE*)pPtr2, dwBytes2, &dwBytesRead)) )
			{  
				// edith 2009.09.16 사운드 오류를 발생하는 유저가 있어서 강제로 리턴하기
//				MessageBox( NULL, "m_pWaveFile->Read((BYTE*)pPtr2, dwBytes2, &dwBytesRead)", 
//					"ServiceBuffer()", MB_OK ); 

				m_pDSBuffer->Unlock(pPtr1, dwBytes1, pPtr2, dwBytes2);
				return;
			}
			m_dwDataCursor += dwBytesRead;
		}
	}

	m_pDSBuffer->Unlock(pPtr1, dwBytes1, pPtr2, dwBytes2);

	if( m_bLoop && m_pOggFile->IsEOF())
		m_pOggFile->Reset();

	m_dwDataCursor %= m_dwDSBufferSize;
	m_dwLastReadPos = dwReadCursor;
}



void CStreamingSound::Stop()
{
	if(!m_pDSBuffer)
		return;

	m_pDSBuffer->Stop();
	m_bRemoveStream = true;
    m_bPlaying = false;
}


BYTE CStreamingSound::GetSilenceData()
{
	if(m_pOggFile->m_WaveFormatEx.wBitsPerSample == 8)
		return 0x80;
	else if(m_pOggFile->m_WaveFormatEx.wBitsPerSample == 16)
		return 0x00;
	return 0;
}


void CStreamingSound::FillBuffer()
{
	// Lock the buffer
	void* pData;
	DWORD  dwBytes;
	if( FAILED(m_pDSBuffer->Lock(0, 0, &pData, &dwBytes, NULL, NULL, DSBLOCK_ENTIREBUFFER)) )
		MessageBox( NULL, "m_pDSBuffer->Lock(0, 0, &pData, &nBytes, NULL, NULL, DSBLOCK_ENTIREBUFFER)", 
		"FillBuffer", MB_OK ); 
	
	// Fill the entire buffer with audio data from source
	DWORD dwBytesToRead;
	DWORD dwBytesRead;

	dwBytesToRead = dwBytes;

	if( FAILED(m_pOggFile->Read((BYTE*)pData, dwBytesToRead, &dwBytesRead)) )
		MessageBox( NULL, "m_pWaveFile->Read((BYTE*)pData, dwBytesToRead, &dwBytesRead)", 
		"FillBuffer", MB_OK );

	m_dwDataCursor += dwBytesRead;
	m_dwDataCursor %= dwBytes;

	if(dwBytesRead < dwBytes)
		memset(((BYTE*)pData) + dwBytesRead, GetSilenceData(), dwBytes - dwBytesRead);

	if( FAILED(m_pDSBuffer->Unlock(pData, dwBytes, NULL, 0)) )
		MessageBox( NULL, "m_pDSBuffer->Unlock(pData, nBytes, NULL, 0)", "FillBuffer", MB_OK ); 
}


void CStreamingSound::SetVolume( float fVol )
{
	m_fVolume = Clamp( fVol, VOLUME_MIN, VOLUME_MAX ); 
	float fMasterVol = CSoundMgr::_GetInstance()->GetVolume(); 
	fMasterVol = Clamp( fMasterVol, VOLUME_MIN, VOLUME_MAX ); 
	int iVol = LinearToLogVol(m_fVolume * fMasterVol);

	if(iVol < -10000)
		iVol = -10000;


	if( m_pDSBuffer )
	{
		if( FAILED(m_pDSBuffer->SetVolume(iVol)) )
			MessageBox( NULL, "FAILED(m_pDSBuffer->SetVolume(iVol))", 
			"CStreamingSound::SetVolume", MB_OK ); 
	}
}
