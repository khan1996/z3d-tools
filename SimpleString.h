// SimpleString.h: interface for the SimpleString class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMPLESTRING_H__F55EEDA2_FCE0_11D4_AD2B_0000E8EB4C69__INCLUDED_)
#define AFX_SIMPLESTRING_H__F55EEDA2_FCE0_11D4_AD2B_0000E8EB4C69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "misc.h"
#include <string>


struct SimpleString  
{
public:
	SimpleString()
	{
		m_lAllocLength = 1;
		m_szString = new char[1];
		m_szString[0] = '\0';
	}

	SimpleString( const char* szString )
	{
		_ASSERT( szString );

		m_lAllocLength = static_cast<long>(strlen( szString )+1);

		m_szString = new char[m_lAllocLength];
		strcpy( m_szString, szString );
	}
	
	SimpleString( const char* szString, int n )
	{
		_ASSERT( szString );
		
		m_lAllocLength = n+1;
		
		m_szString = new char[m_lAllocLength];
		strncpy( m_szString, szString, n );
		m_szString[n] = '\0';
	}
	
	SimpleString( const char* szString1, const char* szString2 )
	{
		_ASSERT( szString1 );
		_ASSERT( szString2 );

		m_lAllocLength = static_cast<long>(strlen(szString1) + strlen(szString2) + 1);

		m_szString = new char[m_lAllocLength];
		strcpy( m_szString, szString1 );
		strcat( m_szString, szString2 );
	}

	~SimpleString()
	{
		SAFE_DELETEA( m_szString );
	}

	long Length()
	{
		return static_cast<long>(strlen(m_szString));
	}

	operator const char*() const
	{
		return m_szString;
	}

	void operator ()(const char* szString)
	{
		_ASSERT( szString );

		if( (long)strlen(szString) < m_lAllocLength )
		{
			strcpy( m_szString, szString );
		}
		else
		{
			SAFE_DELETEA( m_szString );
			m_lAllocLength = static_cast<long>(strlen(szString)+1);

			m_szString = new char[m_lAllocLength];
			strcpy( m_szString, szString );
		}
	}

	void operator ()( const char* szString1, const char* szString2 )
	{
		_ASSERT( szString1 );
		_ASSERT( szString2 );

		if( (long)(strlen(szString1)+strlen(szString2)) < m_lAllocLength )
		{
			strcpy( m_szString, szString1 );
			strcat( m_szString, szString2 );
		}
		else
		{
			SAFE_DELETEA( m_szString );
			m_lAllocLength = static_cast<long>(strlen(szString1) + strlen(szString2) + 1);

			m_szString = new char[m_lAllocLength];
			strcpy( m_szString, szString1 );
			strcat( m_szString, szString2 );
		}
	}

	SimpleString& operator =(const char* szString)
	{
		_ASSERT( szString );

		if( (long)strlen(szString) < m_lAllocLength )
		{
			strcpy( m_szString, szString );
		}
		else
		{
			SAFE_DELETEA( m_szString );
			m_lAllocLength = static_cast<long>(strlen(szString)+1);

			m_szString = new char[m_lAllocLength];
			strcpy( m_szString, szString );
		}

		return *this;
	}

	

protected:
	char* m_szString;
	long m_lAllocLength;
};

#endif // !defined(AFX_SIMPLESTRING_H__F55EEDA2_FCE0_11D4_AD2B_0000E8EB4C69__INCLUDED_)
