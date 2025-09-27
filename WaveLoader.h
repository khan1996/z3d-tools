// WaveLoader.h: interface for the CWaveLoader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAVELOADER_H__3687F4CA_E7BA_48A0_93A9_A32858B43DF2__INCLUDED_)
#define AFX_WAVELOADER_H__3687F4CA_E7BA_48A0_93A9_A32858B43DF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include "DefineSound.h"

#define WAVEFILE_READ   1
#define WAVEFILE_WRITE  2

class CWaveLoader  
{
public:
	WAVEFORMATEX*	m_pwfx;        // Pointer to WAVEFORMATEX structure
	HMMIO						m_hmmio;       // MM I/O handle for the WAVE
	CHAR*						m_pResourceBuffer;
	DWORD					m_dwSize;      // The size of the wave file
	BOOL						m_bIsReadingFromMemory;
	bool							m_bEOF;

	MMCKINFO				m_ck;          // Multimedia RIFF chunk
	MMCKINFO				m_ckRiff;      // Use in opening a WAVE file
	MMIOINFO				m_mmioinfoOut;
	DWORD					m_dwFlags;
	BYTE*						m_pbData;
	BYTE*						m_pbDataCur;
	ULONG						m_ulDataSize;

protected:
    HRESULT ReadMMIO();
    HRESULT WriteMMIO( WAVEFORMATEX *pwfxDest );

public:
    CWaveLoader();
    ~CWaveLoader();

	bool IsEOF() { return m_bEOF; };
	HRESULT Open( LPTSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags );
    HRESULT OpenFromMemory( BYTE* pbData, ULONG ulDataSize, WAVEFORMATEX* pwfx, DWORD dwFlags );
    HRESULT Close();
	HRESULT Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead );
    HRESULT Write( UINT nSizeToWrite, BYTE* pbData, UINT* pnSizeWrote );
	DWORD   GetSize();
    HRESULT ResetFile();
    WAVEFORMATEX* GetFormat() { return m_pwfx; };
};

#endif // !defined(AFX_WAVELOADER_H__3687F4CA_E7BA_48A0_93A9_A32858B43DF2__INCLUDED_)
