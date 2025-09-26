// Z3DStringTable.h: interface for the CZ3DStringTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Z3DSTRINGTABLE_H__587C6AC4_766F_4145_8059_21BB7D1550EA__INCLUDED_)
#define AFX_Z3DSTRINGTABLE_H__587C6AC4_766F_4145_8059_21BB7D1550EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#pragma warning(disable:4786)

#include <map>
#include "misc.h"


#pragma pack(push)
#pragma pack(1)

struct Z3DTOK
{
	union
	{
		struct
		{
			long lTblNum:14;
			long lItemNum:18;
		};
		long lNum;
	};

	Z3DTOK( long l )
	{
		lNum = l;
	}

	Z3DTOK()
	{
	}

	bool operator == ( const Z3DTOK &t ) const
	{
		return (lNum == t.lNum);
	}

	bool operator != ( const Z3DTOK &t ) const
	{
		return (lNum != t.lNum);
	}

	bool operator < ( const Z3DTOK &t ) const
	{
		return(lNum < t.lNum);
	}
};

#pragma pack(pop)

extern const Z3DTOK NULL_TOK;


class CZ3DStringTable  
{
public:
	CZ3DStringTable();
	virtual ~CZ3DStringTable();

	Z3DTOK Add( const char* szKey );
	Z3DTOK GetTOK( const char* szKey );
	const char* GetString( const Z3DTOK tok );
	void Flush();


protected:
	// Tablenum도 TOKCount도 모두 0이 될 수 없다
	long m_lTablenum;
	long m_lTOKCount;

	std::map<char*, Z3DTOK, szi_less> m_mapString2TOK;
	std::map<Z3DTOK, char*> m_mapTOK2String;

	static long  ms_lTableCount;
};

#endif // !defined(AFX_Z3DSTRINGTABLE_H__587C6AC4_766F_4145_8059_21BB7D1550EA__INCLUDED_)
