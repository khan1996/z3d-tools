// SimpleParser.cpp: implementation of the CSimpleParser class.
//
//////////////////////////////////////////////////////////////////////

#include "SimpleParser.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////////
// CSimpleFileBuffer
//////////////////////////////////////////////////////////////////////////

CSimpleFileBuffer::CSimpleFileBuffer()
{
	m_pFileContentBuffer = NULL;
	m_lFileContentBufferLength = 0;
	m_lFileContentBufferIndex = 0;
}


CSimpleFileBuffer::~CSimpleFileBuffer()
{
	SAFE_DELETEA( m_pFileContentBuffer );
}


bool CSimpleFileBuffer::Open( const char* szFileName )
{
	Close();

	FILE* pFile = fopen(szFileName, "rb");

	if( NULL == pFile )
	{
		return false;
	}

	fseek( pFile, 0, SEEK_END );
	long lFileSize = ftell( pFile );
	if( 0 == lFileSize )
	{
		fclose( pFile );
		return false;
	}

	fseek( pFile, 0, SEEK_SET );
	m_pFileContentBuffer = new unsigned char[lFileSize];
	fread( m_pFileContentBuffer, lFileSize, 1, pFile );
	fclose( pFile );

	m_lFileContentBufferLength = lFileSize;
	m_lFileContentBufferIndex = 0;


	strncpy( m_szFileName, szFileName, 300 );
	m_szFileName[299] = '\0';


	return true;
}


void CSimpleFileBuffer::Close()
{
	SAFE_DELETEA( m_pFileContentBuffer );

	m_lFileContentBufferLength = 0;
	m_lFileContentBufferIndex = 0;

	m_szFileName[0] = '\0';
}


int CSimpleFileBuffer::GetChar()
{
	if( m_lFileContentBufferIndex >= m_lFileContentBufferLength )
	{
		return EOF;
	}

	return m_pFileContentBuffer[m_lFileContentBufferIndex++];
}


bool CSimpleFileBuffer::CheckEOF()
{
	if( m_lFileContentBufferIndex >= m_lFileContentBufferLength )
	{
		return true;
	}

	return false;
}


void CSimpleFileBuffer::Seek( long idx, int nOrigin )
{
	long lIdx;
	if( SEEK_SET == nOrigin )
	{
		lIdx = idx;
	}
	else if( SEEK_CUR == nOrigin )
	{
		lIdx = m_lFileContentBufferIndex+idx;	
	}
	else if( SEEK_END == nOrigin )
	{
		lIdx = m_lFileContentBufferLength-1 + idx;
	}

	m_lFileContentBufferIndex = max( 0, min( m_lFileContentBufferLength, lIdx ) );
}

//////////////////////////////////////////////////////////////////////////
// CSimpleParser
//////////////////////////////////////////////////////////////////////////

CSimpleParser::CSimpleParser()
{
	m_szFileName = NULL;
	m_szMaskString = NULL;
	m_lMaskLen = 0;
	m_lPointerInMask = 0;
	m_pFile = NULL;
	//m_lTokenCount = 0;
	m_lLineCount = 0;
	m_lTokenInLine = 0;

	// ------------------------------------------------- //
	// REFACTORING : by Vincent 2005. 10. 26
//	m_vec_szTokens.reserve( 10 );
	// ------------------------------------------------- //

	m_vec_szTmpLine.reserve( 1000 );
}

CSimpleParser::~CSimpleParser()
{
/*
	if( m_pFile )
	{
		fclose( m_pFile );
		m_pFile = NULL;
	}
	SAFE_DELETEA( m_szFileName );
	SAFE_DELETEA( m_szMaskString );

	for( int i = 0; i < m_vec_szTokens.size(); i++ )
	{
		SAFE_DELETEA( m_vec_szTokens[i] );
	}
	m_vec_szTokens.clear();
	m_vec_szTmpLine.clear();
*/

	Reset();
}

bool CSimpleParser::Reset()
{
/*
	if( m_pFile )
	{
		fclose( m_pFile );
		m_pFile = NULL;
	}
*/

	SAFE_DELETE( m_pFile );
	SAFE_DELETEA( m_szFileName );
	SAFE_DELETEA( m_szMaskString );
	m_lMaskLen = 0;
	m_lPointerInMask = 0;

	//m_lTokenCount = 0;

	// ------------------------------------------------- //
	// REFACTORING : by Vincent 2005. 10. 26
//	for( size_t i = 0; i < m_vec_szTokens.size(); i++ )
//	{
//		SAFE_DELETEA( m_vec_szTokens[i] );
//	}
//	m_vec_szTokens.clear();
	m_iTokenNum = 0 ;
	// ------------------------------------------------- //

	m_vec_szTmpLine.clear();

	m_lLineCount = 0;
	m_lTokenInLine = 0;

	return true;
}

bool CSimpleParser::OpenFile( const char* szFileName, const char* szMaskString )
{
	_ASSERT( szFileName );

/*
	FILE* fp;

	if( szMaskString )
	{
		// decrypting mode
		if( (fp = fopen(szFileName, "rb")) == NULL )
		{
			return false;	// file open error
		}
	}
	else
	{
		// normal mode
		if( (fp = fopen(szFileName, "rt")) == NULL )
		{
			return false;	// file open error
		}
	}
*/

	Reset();

	m_pFile = new CSimpleFileBuffer;
	if( false == m_pFile->Open( szFileName ) )
	{
		SAFE_DELETE( m_pFile );
		return false;
	}

//	m_pFile = fp;
	m_szFileName = new char[strlen(szFileName) + 1];
	strcpy( m_szFileName, szFileName );

	if( szMaskString )
	{
		m_lMaskLen = strlen(szMaskString);
		m_szMaskString = new char[m_lMaskLen+1];
		strcpy( m_szMaskString, szMaskString );
		m_lPointerInMask = 0;

		int iLen = strlen(m_szMaskString);
		for(int k = 0; k < iLen; ++k)
		{
			m_szMaskString[k] = m_szMaskString[k]^0xc8;
		}
	}

	return true;
}


inline static bool IsWhiteSpace( char c )
{
	if( '\n' == c ) return true;
	if( '\t' == c ) return true;
	if( ' ' == c ) return true;

	return false;
}

// white space없이 존재 자체가 하나의 토큰분리이유가 되는 문자를 operator라고 한다.
// (내맘대로-_-;;)
inline static bool IsOperator( char c )
{
	if( '(' == c ) return true;
	if( ')' == c ) return true;
	if( '{' == c ) return true;
	if( '}' == c ) return true;
	if( '[' == c ) return true;
	if( ']' == c ) return true;
	if( '<' == c ) return true;
	if( '>' == c ) return true;
	// +- 기호 제외...-_-; 부호로 쓰일 수 있다.
	/*if( '+' == c ) return true;
	if( '-' == c ) return true;*/
	if( '*' == c ) return true;
	if( '/' == c ) return true;
	if( ',' == c ) return true;
	// '.' 제외. 소수점으로 쓰인다-_-;; 이런..
	//if( '.' == c ) return true;
	if( ':' == c ) return true;
	if( ';' == c ) return true;
	if( '`' == c ) return true;
	if( '\'' == c ) return true;
	if( '=' == c ) return true;
	if( '|' == c ) return true;

	return false;
}


int CSimpleParser::GetChrFromFile()
{
	// edith GCMDS 수정.
	// 입력된 MASK 값을 이용해 암호화된 값을
	// 다시 얻어오는 부분이다.

	int c;

	if( NULL == m_pFile )
	{
		return EOF;
	}

	if( m_szMaskString )
	{
//		c = fgetc(m_pFile);
		c = m_pFile->GetChar();
		if( EOF == c )
		{
			return EOF;
		}

		c ^= m_szMaskString[m_lPointerInMask];
		if( 0xD == c )	// LF/CR combo 처리
		{
			// 마스킹된 다음 캐릭터 얻어오기
//			int c2 = m_szMaskString[(m_lPointerInMask+1)%m_lMaskLen] ^ fgetc(m_pFile);
			int c2 = m_szMaskString[(m_lPointerInMask+1)%m_lMaskLen] ^ m_pFile->GetChar();
			if( 0xA == c2 )
			{
				m_lPointerInMask = (m_lPointerInMask + 2) % m_lMaskLen;
				return '\n';
			}
			else
			{
//				fseek( m_pFile, -1, SEEK_CUR );
				m_pFile->Seek( -1, SEEK_CUR );
			}
		}

		m_lPointerInMask = (m_lPointerInMask + 1) % m_lMaskLen;
		return c;
	}

	// else 블럭이지만 구조적으로 필요없어졌음(decrypting 아니면 normal이니까)
	// normal reading
//	return fgetc(m_pFile);
	return m_pFile->GetChar();
}


int CSimpleParser::ReadLine()
{
	int i;

	// ------------------------------------------------- //
	// REFACTORING : by Vincent 2005. 10. 26

	// clear previously read tokens
//	for( i = 0; i < m_vec_szTokens.size(); i++ )
//	{
//		SAFE_DELETEA( m_vec_szTokens[i] );
//	}
//	m_vec_szTokens.clear();
	for ( i = 0; i < MAX_TOKEN_NUM; ++i )
	{
		m_szTokens[ i ][ 0 ] = '\0' ;
	}
	m_iTokenNum = 0 ;
	// ------------------------------------------------- //

	// check if stream opened
	if( NULL == m_pFile )
	{
		return EOF;
	}

	int c;
	while( (c = GetChrFromFile()) != EOF )
	{
		if( '\n' == c )
		{
//			m_vec_szTmpLine.push_back('\0');
			break;
		}

		m_vec_szTmpLine.push_back( (char)c );
	}
	m_lLineCount++;
	m_lTokenInLine = 0;

//	if( feof(m_pFile) )
//	{
//		fclose(m_pFile);
//		m_pFile = NULL;
//	}

	if( m_pFile->CheckEOF() )
	{
		SAFE_DELETE( m_pFile );
	}

	std::vector<char> vec_szTmpToken;
	vec_szTmpToken.reserve( 100 ); // allocate appropriate space for 1 token
	for( i = 0; i < (int)m_vec_szTmpLine.size(); i++ )
	{
		if( IsWhiteSpace( m_vec_szTmpLine[i] ) )
		{
			continue;
		}

		if( '/' == m_vec_szTmpLine[i] ) // 'comment' check
		{
			if( (i+1) < (int)m_vec_szTmpLine.size() && '/' == m_vec_szTmpLine[i+1] )
			{
				break; // skip parsing remainder of the line
			}
		}

		if( IsOperator(m_vec_szTmpLine[i]) )
		{
			vec_szTmpToken.push_back( m_vec_szTmpLine[i] );
			vec_szTmpToken.push_back('\0');
		}
		else 
		{
			switch( m_vec_szTmpLine[i] )
			{
				case '\"': // beginning of a string
				{
					for( i++ ; i < (int)m_vec_szTmpLine.size(); i++ )
					{
						if( '\"' == m_vec_szTmpLine[i] ) // end of a string
						{
							break;
						}
						vec_szTmpToken.push_back( m_vec_szTmpLine[i] );
					}
					vec_szTmpToken.push_back('\0');
					break;
				}

				default: // otherwise parse normal token
				{
					for( ; i < (int)m_vec_szTmpLine.size(); i++ )
					{
						if( IsWhiteSpace(m_vec_szTmpLine[i]) )
						{
							break;
						}
						else if( IsOperator(m_vec_szTmpLine[i]) )
						{
							i--; // one char backward
							break;
						}
						vec_szTmpToken.push_back( m_vec_szTmpLine[i] );
					}
					vec_szTmpToken.push_back('\0');
					break;
				}
			}
		}

		// ------------------------------------------------- //
		// REFACTORING : by Vincent 2005. 10. 26

		// copy parsed token
//		char* szTmp = new char[vec_szTmpToken.size()];
//		strcpy( szTmp, &(vec_szTmpToken[0]) );
//		vec_szTmpToken.clear();
//		m_vec_szTokens.push_back(szTmp);

		strcpy( &(m_szTokens[ m_iTokenNum++ ][ 0 ]), &(vec_szTmpToken[0]) ) ;
		vec_szTmpToken.clear();
		// ------------------------------------------------- //
	}

	m_vec_szTmpLine.clear();

	// ------------------------------------------------- //
	// REFACTORING : by Vincent 2005. 10. 26
//	return m_vec_szTokens.size();
	return m_iTokenNum ;
	// ------------------------------------------------- //
}

char* CSimpleParser::GetToken( int n )
{
	// ------------------------------------------------- //
	// REFACTORING : by Vincent 2005. 10. 26
//	static char cDummyEmptyToken = '0';
//
//	if( static_cast<size_t>(n+1) > m_vec_szTokens.size() )
//	{
//		//return NULL;
//		return &cDummyEmptyToken;
//	}
//
//	return m_vec_szTokens[n];

	static char cDummyEmptyToken = '0' ;
	if ( n + 1 > m_iTokenNum )
	{
		return &cDummyEmptyToken ;
	}

	return &(m_szTokens[ n ][ 0 ]) ;
	// ------------------------------------------------- //
}

int CSimpleParser::GetTokenNum()
{
	// ------------------------------------------------- //
	// REFACTORING : by Vincent 2005. 10. 26
//	return m_vec_szTokens.size();
	return m_iTokenNum ;
	// ------------------------------------------------- //
}

int CSimpleParser::GetLineNum()
{
	return m_lLineCount;
}

char* CSimpleParser::GetFileName()
{
	return m_szFileName;
}


char* CSimpleParser::GetNextToken()
{
	if( m_lTokenInLine >= GetTokenNum() )
	{
		do
		{
			if( EOF == ReadLine() )
			{
				return NULL;
			}
		} while( 0 == GetTokenNum() );
	}

	return GetToken( m_lTokenInLine++ );
}