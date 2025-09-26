// Z3DMaskedStream.cpp: implementation of the CZ3DMaskedStream class.
//
//////////////////////////////////////////////////////////////////////

#include "Z3DMaskedStream.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CZ3DMaskedStream::CZ3DMaskedStream()
{
	m_szFileName = NULL;
	m_pFile = NULL;
	m_szMaskString = NULL;
	m_lMaskStringLength = 0;
	m_lPointerInMask = 0;
}

CZ3DMaskedStream::~CZ3DMaskedStream()
{
	Reset();
}


void CZ3DMaskedStream::Reset()
{
	if( m_pFile )
	{
		fclose( m_pFile );
		m_pFile = NULL;
	}

	SAFE_DELETEA( m_szFileName );
	SAFE_DELETEA( m_szMaskString );
	m_lMaskStringLength = 0;
	m_lPointerInMask = 0;
}


bool CZ3DMaskedStream::Open( const char* szFileName, const char* szMaskString )
{
	Reset();

	if( NULL ==  (m_pFile = fopen( szFileName, "wb" )) )
	{
		return false;
	}

	m_szFileName = new char[strlen(szFileName) + 1];
	strcpy( m_szFileName, szFileName );

	SetMaskString( szMaskString );
	
	return true;
}


void CZ3DMaskedStream::Printf( char* szTypes, ... )
{
	if( NULL == m_pFile )
	{
		return;	// file not opened
	}

	const long MAX_STRING_SIZE = 300;

	char szTmp[MAX_STRING_SIZE];
	va_list vl;
	
	long l = 0;
	va_start( vl, szTypes );
	l = _vsnprintf( szTmp, MAX_STRING_SIZE, szTypes, vl );
	va_end( vl );

	if( l < 0 )
	{
		return;	// _vsnprintf() error
	}

	for( int i = 0; i < l; ++i )
	{
		if( '\n' == szTmp[i] )
		{
			// handle 'new line' char.
			fputc( 0x0D ^ GetMaskChar(), m_pFile );
			fputc( 0x0A ^ GetMaskChar(), m_pFile );
		}
		else
		{
			fputc( szTmp[i] ^ GetMaskChar(), m_pFile );
		}
	}
}


void CZ3DMaskedStream::SetMaskString( const char* szString )
{
	m_lPointerInMask = 0;

	if( m_szMaskString )
	{
		SAFE_DELETEA( m_szMaskString )
	}
	if( NULL == szString )
	{
		m_lMaskStringLength = 0;
		return;
	}

	m_lMaskStringLength = strlen(szString);

	m_szMaskString = new char[m_lMaskStringLength+1];
	strcpy( m_szMaskString, szString );

	int iLen = strlen(m_szMaskString);
	for(int k = 0; k < iLen; ++k)
	{
		m_szMaskString[k] = m_szMaskString[k]^0xc8;
	}
}


char CZ3DMaskedStream::GetMaskChar()
{
	if( NULL == m_szMaskString )
	{
		return 0;
	}

	long l = m_lPointerInMask;
	m_lPointerInMask = ( m_lPointerInMask + 1 ) % m_lMaskStringLength;
		
	return m_szMaskString[l];
}