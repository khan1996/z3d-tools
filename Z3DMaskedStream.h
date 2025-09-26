// Z3DMaskedStream.h: interface for the CZ3DMaskedStream class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Z3DMASKEDSTREAM_H__2DFA8D79_3B15_4028_9DF5_C57BC24FA678__INCLUDED_)
#define AFX_Z3DMASKEDSTREAM_H__2DFA8D79_3B15_4028_9DF5_C57BC24FA678__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include <stdarg.h>

#include "SimpleParser.h"	// just for 'SIMPLE_PARSER_MASK_STRING' to be used as a default param. of Open() method


class CZ3DMaskedStream  
{
public:
	CZ3DMaskedStream();
	virtual ~CZ3DMaskedStream();

	void Reset();

	bool Open( const char* szFileName, const char* szMaskString = SIMPLE_PARSER_MASK_STRING );
	void Close()
	{
		Reset();
	}

	void Printf( char* szTypes, ... );

protected:
	char* m_szFileName;
	FILE* m_pFile;

	char* m_szMaskString;
	long m_lMaskStringLength;
	long m_lPointerInMask;

	void SetMaskString( const char* szString );
	char GetMaskChar();
};

#endif // !defined(AFX_Z3DMASKEDSTREAM_H__2DFA8D79_3B15_4028_9DF5_C57BC24FA678__INCLUDED_)
