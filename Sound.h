// Sound.h: interface for the CSound class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOUND_H__C6C8A751_6637_4E39_9525_125C0632F1D0__INCLUDED_)
#define AFX_SOUND_H__C6C8A751_6637_4E39_9525_125C0632F1D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <dsound.h>
#include "DefineSound.h"

class CWaveLoader; 
class CSoundMgr; 

class CSound  
{
	friend class CSound3D; 
	friend class CSoundMgr; 

protected:
	LPDIRECTSOUNDBUFFER8*	m_ppDSBuffer; 
	CWaveLoader*						m_pWaveFile;
	DWORD								m_dwNumBuffers;
	DWORD								m_dwDSBufferSize; 	
	bool										m_bSound3D;
	bool										m_bLoop;
	float										m_fVolume; 
	DWORD								m_dwLastPlayTime; 
	DWORD								m_dwLatestPlayIndex;
	int											m_iPreserveScore; 

public:
	CSound(); 
    virtual ~CSound();

	bool Create( const char* strFileName, DWORD dwNumBuffers ); 
	void Destroy(); 
	void Play(); 
	
	inline void SetLooping( bool bLoop ) { m_bLoop = bLoop; }
	inline bool IsLooping() { return m_bLoop; }
	void SetVolume( float fVol ); 
	void SetVolume(LPDIRECTSOUNDBUFFER pDSBuffer, float fVol ); 
	inline void GetVolume( float& fVol ) { fVol = m_fVolume; }

	HRESULT RestoreBuffer( LPDIRECTSOUNDBUFFER pDSB, bool* pbWasRestored );
	void FillBuffer( LPDIRECTSOUNDBUFFER pDSBuffer ); 
    LPDIRECTSOUNDBUFFER GetFreeBuffer();
    LPDIRECTSOUNDBUFFER GetBuffer( DWORD dwIndex );
	void Stop();
    void Reset();
    bool IsPlaying();
	inline DWORD GetAllBufferSize() { return (m_dwDSBufferSize*m_dwNumBuffers); }
};

#endif // !defined(AFX_SOUND_H__C6C8A751_6637_4E39_9525_125C0632F1D0__INCLUDED_)
