// Z3DMapTok2FileName.h: interface for the CZ3DMapTok2FileName class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Z3DMAPTOK2FILENAME_H__41DD22BD_543D_411F_9CD0_4A22EE6784ED__INCLUDED_)
#define AFX_Z3DMAPTOK2FILENAME_H__41DD22BD_543D_411F_9CD0_4A22EE6784ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( disable:4786 )

#include <map>
#include "Z3DStringTable.h"


class CZ3DMapTok2FileName  
{
public:
	CZ3DMapTok2FileName();
	virtual ~CZ3DMapTok2FileName();

	bool Load( const char* szFileName, const char* szFilePath, CZ3DStringTable& rTable );
	const char* GetFileName( Z3DTOK tok );

protected:
	std::map<Z3DTOK, char* > m_mapTok2FileName;
};

#endif // !defined(AFX_Z3DMAPTOK2FILENAME_H__41DD22BD_543D_411F_9CD0_4A22EE6784ED__INCLUDED_)
