// OggLoader.h: interface for the COggLoader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OGGLOADER_H__EDD0A382_ACBB_4E9B_974E_02D96D520340__INCLUDED_)
#define AFX_OGGLOADER_H__EDD0A382_ACBB_4E9B_974E_02D96D520340__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common.h"
#include <dsound.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

class COggLoader  
{
public:
	DWORD					m_dwBufferSize;
	DWORD					m_dwNumSamples;
	vorbis_info*				m_pVorbisInfo;
	bool							m_bEOF;
	bool							m_bOpen;
	FILE*						m_pFP;  

	WAVEFORMATEX		m_WaveFormatEx;
	OggVorbis_File			m_VorbisFile;

public:
	COggLoader();
	virtual ~COggLoader();

	void Init();
	bool Open( char* strFileName);
	bool Close();
	bool Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead );
	DWORD GetSize();
   bool	 Reset();
   WAVEFORMATEX* GetFormat() { return &m_WaveFormatEx; }; 
	bool	IsEOF() { return m_bEOF; };
	bool	GetStreamInfo();
};

#endif // !defined(AFX_OGGLOADER_H__EDD0A382_ACBB_4E9B_974E_02D96D520340__INCLUDED_)
