// Z3DStringTable.cpp: implementation of the CZ3DStringTable class.
//
//////////////////////////////////////////////////////////////////////

#include "Z3DStringTable.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

long CZ3DStringTable::ms_lTableCount(0L);
const Z3DTOK NULL_TOK(0L);


CZ3DStringTable::CZ3DStringTable()
{
	++ms_lTableCount;
	if( ms_lTableCount > 0x3FF )
	{
		ms_lTableCount = 1;
	}

	m_lTablenum = ms_lTableCount;
	m_lTOKCount = 0;
}

CZ3DStringTable::~CZ3DStringTable()
{
	std::map<char*, Z3DTOK, szi_less>::iterator it;

	char* sz;
	for( it = m_mapString2TOK.begin(); it != m_mapString2TOK.end(); it++ )
	{
		sz = it->first;
		SAFE_DELETEA( sz );
	}

	// map들의 flushing은 자동으로..(소멸자호출되니까-_-a)
}


Z3DTOK CZ3DStringTable::Add( const char* szKey )
{
	std::map<char*, Z3DTOK, szi_less>::iterator it;

	it = m_mapString2TOK.find( const_cast<char*>(szKey) );
	if( m_mapString2TOK.end() != it )
	{
		return it->second;
	}
	
	//else
	char* sz = new char[strlen(szKey)+1];
	strcpy( sz, szKey );
	++m_lTOKCount;
	Z3DTOK tok;
	tok.lTblNum = m_lTablenum;
	tok.lItemNum =  m_lTOKCount;
	m_mapString2TOK[sz] = tok;
	m_mapTOK2String[tok] = sz;

	return tok;
}


Z3DTOK CZ3DStringTable::GetTOK( const char* szKey )
{
	if( NULL == szKey )
	{
		return NULL_TOK;
	}

	std::map<char*, Z3DTOK, szi_less>::iterator it;

	it = m_mapString2TOK.find( const_cast<char*>(szKey) );
	if( m_mapString2TOK.end() == it )
	{
		return NULL_TOK;
	}

	//else
	return it->second;
}


const char* CZ3DStringTable::GetString( const Z3DTOK tok )
{
	std::map<Z3DTOK, char*>::iterator it;

	it = m_mapTOK2String.find( tok );
	if( m_mapTOK2String.end() == it )
	{
		//return NULL;
		return "";
	}

	//else
	return it->second;
}


void CZ3DStringTable::Flush()
{
	std::map<char*, Z3DTOK, szi_less>::iterator it;

	char* sz;
	for( it = m_mapString2TOK.begin(); it != m_mapString2TOK.end(); it++ )
	{
		sz = it->first;
		SAFE_DELETEA( sz );
	}

	m_mapString2TOK.clear();

	m_lTOKCount = 0;
}
