// Sound3D.h: interface for the CSound3D class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOUND3D_H__9100BA05_DC25_4ECD_A2A7_4E34F81982C6__INCLUDED_)
#define AFX_SOUND3D_H__9100BA05_DC25_4ECD_A2A7_4E34F81982C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <dsound.h>
#include "EAXBuffer.h"

class CSound; 
class CWaveLoader; 
class CSoundMgr; 

class CSound3D 
{
protected:
	friend class CSoundMgr;		
	CSound*									 m_pSound; 
	LPDIRECTSOUND3DBUFFER8*   m_ppDS3DBuffer; 
	//CEAXBuffer								 m_EAXBuffer; 

protected:
	LPDIRECTSOUND3DBUFFER8 Get3DSoundBufferInterface( DWORD dwIndex ); 

public:
	CSound3D();
	virtual ~CSound3D();

	bool Create( const char* strFileName, DWORD dwNumBuffers ); 
	void Destroy(); 
	void Play() { m_pSound->Play(); }; 
	void Stop() { m_pSound->Stop(); }; 
    void Reset() { m_pSound->Reset(); }; 
    bool IsPlaying() { return m_pSound->IsPlaying(); }; 
	inline DWORD GetAllBufferSize() { return m_pSound->GetAllBufferSize(); }; 
	LPDIRECTSOUNDBUFFER GetFreeBuffer() { return m_pSound->GetFreeBuffer(); };
    LPDIRECTSOUNDBUFFER GetBuffer( DWORD dwIndex ) { return m_pSound->GetBuffer( dwIndex ); };
	inline void SetVolume( float fVol ) { m_pSound->SetVolume(fVol); }; 
	void GetVolume( float& fVol ) { m_pSound->GetVolume(fVol); }; 
	inline void SetLooping( bool bLoop ) { m_pSound->SetLooping(bLoop); }; 
	inline bool IsLooping() { return m_pSound->IsLooping(); }; 
	
	//3D Property
	void SetPosition(D3DVECTOR& vPos);
	void GetPosition(D3DVECTOR& vPos);

	void SetVelocity(const D3DVECTOR& vVel);
	void GetVelocity(D3DVECTOR& vVel);

	void SetMaxDistance(float fMaxDist );
	void GetMaxDistance(float& fMaxDist);

	void SetMinDistance(float fMinDist);
	void GetMinDistance(float& fMinDist);

	void SetConeAngles(DWORD dwInside, DWORD dwOutside);
	void GetConeAngles(DWORD& dwInside, DWORD& dwOutside);

	void SetConeOrientation(D3DVECTOR& vOrientation);
	void GetConeOrientation(D3DVECTOR& vOrientation);

	void SetConeOutsideVolume(float fVolume);
	void GetConeOutsideVolume(float& fVolume);

	void SetMode(DWORD dwMode);
	void GetMode(DWORD& dwMode);
};

#endif // !defined(AFX_SOUND3D_H__9100BA05_DC25_4ECD_A2A7_4E34F81982C6__INCLUDED_)
