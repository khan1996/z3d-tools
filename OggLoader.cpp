// OggLoader.cpp: implementation of the COggLoader class.
//
//////////////////////////////////////////////////////////////////////

#include "OggLoader.h"
#include "GMMemory.h"

COggLoader::COggLoader() : 	m_dwBufferSize(0), 
												m_dwNumSamples(0), 
												m_pVorbisInfo(NULL), 
												m_bEOF(false), 
												m_bOpen(false), 
												m_pFP(NULL)
{
}


COggLoader::~COggLoader()
{
	Close(); 
}


bool COggLoader::Open( char* strFileName )
{
	if(m_bOpen) return false;
	
	if( (m_pFP = fopen( strFileName, "rb" )) == NULL) 
		return false;

	m_bOpen = true; 
	if (ov_open(m_pFP, &m_VorbisFile, NULL, 0) < 0)
	{
		char buf[MAX_PATH] = {0}; 
		sprintf( buf, "해당파일을 열수가 없습니다. : %s \n COggLoader::Open", strFileName ); 
		MessageBox( NULL, buf, "사운드에러", MB_OK );
		return false; 
	}

	if( !GetStreamInfo() )
	{
		char buf[MAX_PATH] = {0}; 
		sprintf( buf, "해당파일의 스트림정보를 얻어올수 없습니다. : %s \n COggLoader::Open", strFileName ); 
		MessageBox( NULL, buf, "사운드에러", MB_OK ); 
		return false; 
	}

	return true; 
}


bool COggLoader::Close() 
{
	if(!m_bOpen) return false;

	if( ov_clear(&m_VorbisFile) < 0 )
		return false; 
	
	if( m_pFP ) fclose( m_pFP );
	
	return true; 
}


bool COggLoader::Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead )
{
	if(!m_bOpen) return false;

	char* pCurBuffer = (char*)pBuffer;
	DWORD dwBytesRead = 0;
	int iSection = 0;
	while( (dwBytesRead < dwSizeToRead) && !m_bEOF )
	{
		int iRet = ov_read(&m_VorbisFile, pCurBuffer, dwSizeToRead - dwBytesRead, 0, 2, 1, &iSection);
		if (iRet == 0 || iSection != 0) 
		{
			m_bEOF = true;
		} 
		else if (iRet < 0) 
		{
			return false;
		}
		dwBytesRead += iRet;
		pCurBuffer += iRet;
	}

	*pdwSizeRead = dwBytesRead;
	
	return true; 
}


bool COggLoader::Reset()
{
	if(!m_bOpen)
		return false;

	m_bEOF = false;
	ov_pcm_seek(&m_VorbisFile, 0);

	return true; 
}


DWORD COggLoader::GetSize()
{
	return m_dwNumSamples * m_WaveFormatEx.nChannels * m_WaveFormatEx.wBitsPerSample / 8;
}


bool COggLoader::GetStreamInfo()
{
	// Get vorbis file information
	m_pVorbisInfo = ov_info(&m_VorbisFile,-1);

	if( !m_pVorbisInfo ) return false; 

	// Get the number of PCM samples in this file
	m_dwNumSamples = (long)ov_pcm_total(&m_VorbisFile,-1);
	
	// set up the WaveFormatEx structure
	m_WaveFormatEx.wFormatTag = WAVE_FORMAT_PCM;
	m_WaveFormatEx.nChannels = m_pVorbisInfo->channels;
	m_WaveFormatEx.nSamplesPerSec = m_pVorbisInfo->rate;
	m_WaveFormatEx.wBitsPerSample = 16;
	m_WaveFormatEx.nBlockAlign = m_WaveFormatEx.nChannels * m_WaveFormatEx.wBitsPerSample / 8;
	m_WaveFormatEx.nAvgBytesPerSec = m_WaveFormatEx.nSamplesPerSec * m_WaveFormatEx.nBlockAlign;
	m_WaveFormatEx.cbSize = 0;

	return true;
}

