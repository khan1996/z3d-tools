// StateLog.h: interface for the CStateLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATELOG_H__1E93C2C5_AF70_4A6C_8AF2_2B66D46A1A36__INCLUDED_)
#define AFX_STATELOG_H__1E93C2C5_AF70_4A6C_8AF2_2B66D46A1A36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>

class CStateLog  
{
	static FILE *m_fp;
public:
	static void Message(char *strMessage,char *strFilename,int strLine);
	static void Create(char *strFilename);

	CStateLog();
	virtual ~CStateLog();
};

#define LogMessage(Msg) CStateLog::Message(Msg,__FILE__,__LINE__)

#endif // !defined(AFX_STATELOG_H__1E93C2C5_AF70_4A6C_8AF2_2B66D46A1A36__INCLUDED_)
