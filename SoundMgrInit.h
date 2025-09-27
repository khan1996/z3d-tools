#ifndef ___SOUNDMGRINIT_H___
#define ___SOUNDMGRINIT_H___

#include <dsound.h>
#include "DefineSound.h"

class SoundMgrInit
{
public:
	
	HWND m_hWnd; 
	bool m_bUseEAX;
	GUID m_guidSound3DAlgorithm; 
	DWORD m_dwPrimaryFrequency;
	DWORD m_dwPrimaryChannels; 
	DWORD m_dwPrimaryBitRate; 
	DWORD m_dwLimitSound; 
	DWORD m_dwLimitSound3D; 
	DWORD m_dwLimitStreaming; 
	
	SoundMgrInit() { Clear(); }; 
	virtual ~SoundMgrInit() {};
	
	void Clear()
	{
		m_hWnd = 0; 
		m_bUseEAX = false; 
		m_guidSound3DAlgorithm = SOUND_ALGORITHM_NO; 
		m_dwPrimaryFrequency = SOUND_SAMPLE_22k;
		m_dwPrimaryChannels = 2; 
		m_dwPrimaryBitRate = 16; 
		m_dwLimitSound = 5;
		m_dwLimitSound3D = 20;
		m_dwLimitStreaming = 10; 
	}
};

#endif //___SOUNDMGRINIT_H___