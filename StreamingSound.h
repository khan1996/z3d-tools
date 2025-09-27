// StreamingSound.h: interface for the CStreamingSound class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STREAMINGSOUND_H__C14F203D_5328_480B_937C_55F88F36E678__INCLUDED_)
#define AFX_STREAMINGSOUND_H__C14F203D_5328_480B_937C_55F88F36E678__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <dsound.h>
#include "DefineSound.h"

class CSound; 
class COggLoader; 

class CStreamingSound 
{
protected:
	friend class CStreamingSound3D; 

	LPDIRECTSOUNDBUFFER8	m_pDSBuffer; 
	COggLoader*						m_pOggFile; 
	DWORD								m_dwDSBufferSize;
	DWORD								m_dwDataCursor;
	DWORD								m_dwBytesPlayed;
	DWORD								m_dwLastReadPos;
	bool										m_bPlaying; 
	bool										m_bRemoveStream;
	bool										m_bLoop; 
	bool										m_bSound3D; 
	float										m_fVolume; 

public:
	CStreamingSound();
    virtual ~CStreamingSound();

protected:
	BYTE GetSilenceData(); 
	void FillBuffer(); 
	HRESULT RestoreBuffer( LPDIRECTSOUNDBUFFER pDSB, BOOL* pbWasRestored );

public:	
	bool Create( char* strFileName ); 
	void Destroy(); 
	void ServiceBuffer(); 
	void Play(); 
	void Stop(); 
    HRESULT Reset(); 
	inline bool IsPlaying() { return m_bPlaying; }
	inline DWORD GetBufferSize() { return m_dwDSBufferSize; }
	inline void SetLooping( bool bLoop ) { m_bLoop = bLoop; }
	inline bool IsLooping() { return m_bLoop; }
	void SetVolume( float fVol ); 
	inline void GetVolume( float& fVol ) { fVol = m_fVolume; }
};

#endif // !defined(AFX_STREAMINGSOUND_H__C14F203D_5328_480B_937C_55F88F36E678__INCLUDED_)
