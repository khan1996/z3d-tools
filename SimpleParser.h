// SimpleParser.h: interface for the CSimpleParser class.
//
//////////////////////////////////////////////////////////////////////

// 2000. 1. 4 : CSimpleParser �ϴ� �ϼ�
// 2000. 1. 8 : comment ( '// blah blah...' ) ����
// 2000. 1.11 : ��ȣ�� ���� ��ū�и� ����( '()' '{}' '[]' '<>' )
// 2000. 1.11 : 1���� �����ڿ� ���� ��ū�и� ����( '+' '-' ... ����-_-; )

// edith GCMDS ����ũ ��Ʈ��
#define SIMPLE_PARSER_MASK_STRING "�ƹ��ų�"
 

#if !defined(AFX_SIMPLEPARSER_H__FAFA5CC1_E1BC_11D4_AD2B_0000E8EB4C69__INCLUDED_)
#define AFX_SIMPLEPARSER_H__FAFA5CC1_E1BC_11D4_AD2B_0000E8EB4C69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>

#include "misc.h"

#include <vector>


class CSimpleFileBuffer
{
public:
	CSimpleFileBuffer();
	~CSimpleFileBuffer();


	bool Open( const char* szFileName );
	void Close();
	int GetChar();
	bool CheckEOF();
	void Seek( long idx, int nOrigin );


protected:
	unsigned char* m_pFileContentBuffer;
	long m_lFileContentBufferLength;
	long m_lFileContentBufferIndex;

	char m_szFileName[300];
};


class CSimpleParser  
{
public:
	enum EConstants
	{
		MAX_TOKEN_NUM		= 16,
		MAX_TOKEN_LENGTH	= 128
	} ;

	CSimpleParser();
	virtual ~CSimpleParser();

	bool Reset();
	bool OpenFile( const char* szFileName, const char* szMaskString = SIMPLE_PARSER_MASK_STRING );

	int ReadLine(); // returns # of tokens read
	char* GetToken( int n );
	int GetTokenNum();
	int GetLineNum();
	char* GetFileName();
	void Close()
	{
		Reset();
	}

	char* GetNextToken();	// line�� �����ϰ� token ������� ���

	int GetChrFromFile();	// decrypting read �� ���� fgetc�� ��ü�ϴ� �Լ�


protected:
	char* m_szMaskString;
	long m_lMaskLen;
	long m_lPointerInMask;

	char* m_szFileName;
	//	FILE* m_pFile;
	CSimpleFileBuffer* m_pFile;

	//long m_lTokenCount;
	long m_lLineCount;

	long m_lTokenInLine;	// used for GetNextToken()

// ------------------------------------------------- //
// REFACTORING : by Vincent 2005. 10. 26
//	std::vector<char*> m_vec_szTokens;
	char m_szTokens[ MAX_TOKEN_NUM ][ MAX_TOKEN_LENGTH ] ;
	int m_iTokenNum ;
// ------------------------------------------------- //

	std::vector<char> m_vec_szTmpLine;
};

#endif // !defined(AFX_SIMPLEPARSER_H__FAFA5CC1_E1BC_11D4_AD2B_0000E8EB4C69__INCLUDED_)
