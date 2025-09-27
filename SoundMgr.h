// SoundMgr.h: interface for the CSoundMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOUNDMGR_H__5CBFAA8B_CAA9_47E7_8C9C_456C45247983__INCLUDED_)
#define AFX_SOUNDMGR_H__5CBFAA8B_CAA9_47E7_8C9C_456C45247983__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <dsound.h>
#include "DefineSound.h"
#include "Common.h"
#include "Sound.h"
#include "Sound3D.h"
#include "StreamingSound.h"
#include "StreamingSound3D.h"
#include "Listener.h"
#include "SoundMgrInit.h"
using namespace std; 

class CSoundMgr 
{
	enum eCIRCLEBUFTYPE
	{	
		SOUND = 0, 
		SOUND3D, 
		STREAM, 
		STREAM3D, 
	};

private:
	typedef map<DWORD, CSound*> SOUNDMAP;
	typedef map<DWORD, CSound3D*> SOUND3DMAP;
	typedef map<DWORD, CStreamingSound*> STREAMINGMAP;
	typedef map<DWORD, CStreamingSound3D*> STREAMING3DMAP; 
	typedef list<CStreamingSound*> STREAMLIST; 
	typedef list<CStreamingSound3D*> STREAM3DLIST; 
	
	LPDIRECTSOUND8			m_pDS; 
	static CSoundMgr*			ms_pSoundMgr; 
	bool									m_bInit;
	SoundMgrInit					m_InitProp; 
	DSCAPS							m_Caps; 
	CListener*						m_pListener; 
	SOUNDMAP						m_SoundMap; 
	SOUND3DMAP					m_Sound3DMap;
	STREAMINGMAP				m_StreamingMap; 
	STREAMING3DMAP			m_Streaming3DMap; 
	HANDLE							m_hTimeEventThread; 
	DWORD							m_dwTimeEventID; 
	STREAMLIST					m_StreamProcess;
	STREAMLIST					m_StreamRemoval;
	CRITICAL_SECTION			m_csSoundUpdate;
	float									m_fMasterVolume;

public:
	HANDLE							m_hDestroyEvent; 

private:
	bool SetPrimaryBufferFormat(DWORD dwPrimaryChannels, 
		DWORD dwPrimaryFreq, DWORD dwPrimaryBitRate);
	void SimpleCircleBuffer( DWORD dwType );
	DWORD GetHashID( const char* strFileName ); 
	void ServiceStreamingBuffers();

public:
	inline LPDIRECTSOUND8 GetDirectSound() { return m_pDS; }
	inline CRITICAL_SECTION& GetUpdateCS() { return m_csSoundUpdate; }
	void InsertStream( CStreamingSound* pStreamSound );
	void RemoveStream( CStreamingSound* pStreamSound );
	void ThreadFunc(); 

public:
	CSoundMgr() {};
	virtual ~CSoundMgr() {};

	bool Init( SoundMgrInit& Init );
	void ReleaseAllData(); 
	inline bool IsInitialized() { return m_bInit; }
	inline CListener* GetListener() { return m_pListener; }
	bool GetSound( CSound*& pSound, char* strFileName, DWORD dwNumBuffers=5 ); 
	bool GetSound3D( CSound3D*& pSound3D, const char* strFileName, DWORD dwNumBuffers=10 ); 
	void GetStreamingSound( CStreamingSound*& pStreamingSound, char* strFileName ); 
	void GetStreamingSound3D( CStreamingSound3D*& pStreamingSound3D, char* strFileName ); 
	void AllStopStreamingSound(); 
	void AllStopStreamingSound3D(); 
	DWORD GetUseMemory(); 
	DSCAPS GetCaps(); 
	void SetVolume( float fVol ); 
	float GetVolume();
	inline SoundMgrInit* GetInitProp() { return &m_InitProp; }
	
	static CSoundMgr* _GetInstance();
	static void _Destroy(); 
};

#endif // !defined(AFX_SOUNDMGR_H__5CBFAA8B_CAA9_47E7_8C9C_456C45247983__INCLUDED_)
