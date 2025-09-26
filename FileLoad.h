// FileLoad.h: interface for the CFileLoad class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILELOAD_H__95D97159_9B6D_4BBC_BD14_1DB1825501BF__INCLUDED_)
#define AFX_FILELOAD_H__95D97159_9B6D_4BBC_BD14_1DB1825501BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include <string>

#define MAX_NAMEBUFFER 256

class CFileLoad  
{
public:
	void GetData(void* pData,size_t size);
	void Load(char *strFilename);

	char m_strFilename[MAX_NAMEBUFFER];
	void *m_Data;
	int m_Len;
	char *m_ReadLoc;
	int m_ReadLen;

	CFileLoad();
	virtual ~CFileLoad();

};

#endif // !defined(AFX_FILELOAD_H__95D97159_9B6D_4BBC_BD14_1DB1825501BF__INCLUDED_)
