// StreamingSound3D.h: interface for the CStreamingSound3D class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STREAMINGSOUND3D_H__2504463D_A560_4075_AD7C_14975B160E69__INCLUDED_)
#define AFX_STREAMINGSOUND3D_H__2504463D_A560_4075_AD7C_14975B160E69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <dsound.h>

class CStreamingSound; 
class COggLoader; 

class CStreamingSound3D  
{
protected:
	CStreamingSound*				  m_pStreamingSound; 
	LPDIRECTSOUND3DBUFFER8  m_pDS3DBuffer; 
	DS3DBUFFER						  m_Prop;

public:
	CStreamingSound3D();
	virtual ~CStreamingSound3D();

	bool Create( char* strFileName ); 
	void Destroy(); 
	void ServiceBuffer() { m_pStreamingSound->ServiceBuffer(); }; 
	void Play() { m_pStreamingSound->Play(); }; 
	void Stop() { m_pStreamingSound->Stop(); }; 
    HRESULT Reset() { return m_pStreamingSound->Reset(); }; 
	bool IsPlaying() { return m_pStreamingSound->m_bPlaying; }; 
	DWORD GetBufferSize() { return m_pStreamingSound->m_dwDSBufferSize; }; 
	void SetLooping( bool bLoop ) { m_pStreamingSound->SetLooping(bLoop); }; 
	bool IsLooping() { return m_pStreamingSound->IsLooping(); }; 
	void SetVolume( float fVol ) { m_pStreamingSound->SetVolume(fVol); };  
	void GetVolume( float& fVol ) { m_pStreamingSound->GetVolume(fVol); };  

	//3D Property
	void SetPosition(D3DVECTOR& vPos);
	void GetPosition(D3DVECTOR& vPos);

	void SetVelocity(const D3DVECTOR& vVel);
	void GetVelocity(D3DVECTOR& vVel);

	void SetMaxDistance(float fMaxDist);
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

#endif // !defined(AFX_STREAMINGSOUND3D_H__2504463D_A560_4075_AD7C_14975B160E69__INCLUDED_)
