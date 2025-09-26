// Z3DMapTok2FileName.cpp: implementation of the CZ3DMapTok2FileName class.
//
//////////////////////////////////////////////////////////////////////

#include "Z3DMapTok2FileName.h"
#include "SimpleParser.h"
#include "GMMemory.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CZ3DMapTok2FileName::CZ3DMapTok2FileName()
{

}

CZ3DMapTok2FileName::~CZ3DMapTok2FileName()
{
	std::map<Z3DTOK, char* >::iterator it;

	for( it =  m_mapTok2FileName.begin(); it != m_mapTok2FileName.end(); it++ )
	{
		SAFE_DELETEA( it->second );
	}
}

bool CZ3DMapTok2FileName::Load( const char* szFileName, const char* szFilePath, CZ3DStringTable& rTable )
{
	CSimpleParser parser;
	if( false == parser.OpenFile( szFileName ) )
	{
		return false;
	}

	char* szPrefix = NULL;
	long lLength = strlen( szFilePath );
	if( '/' != szFilePath[lLength-1] && '\\' != szFilePath[lLength-1] )
	{
		szPrefix = new char[lLength+2];
		sprintf( szPrefix, "%s/", szFilePath );
		++lLength;
	}
	else
	{
		szPrefix = new char[lLength+1];
		strcpy( szPrefix, szFilePath );
	}

	char* szToken;
	char* szStr;
	Z3DTOK tok;
	while( NULL != (szToken = parser.GetNextToken()) )
	{
		tok = rTable.Add( szToken );
		if( m_mapTok2FileName.end() != m_mapTok2FileName.find(tok) )
		{
			// ม฿บน
			return false;
		}
		szToken = parser.GetNextToken();
		if( NULL == szToken )
		{
			return false;
		}
		szStr = new char[lLength + strlen(szToken) + 1];
		sprintf( szStr, "%s%s", szPrefix, szToken );
		m_mapTok2FileName[tok] = szStr;
	}

	SAFE_DELETEA( szPrefix );
	return true;
}

const char* CZ3DMapTok2FileName::GetFileName( Z3DTOK tok )
{
	std::map<Z3DTOK, char* >::iterator it;

	it = m_mapTok2FileName.find( tok );

	if( it == m_mapTok2FileName.end() )
	{
		//return NULL;
		return "";
	}

	return it->second;
}