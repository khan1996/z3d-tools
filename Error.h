// Error.h: interface for the CError class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ERROR_H__0F1AE97A_A373_4DE7_B36A_23BE292D42B2__INCLUDED_)
#define AFX_ERROR_H__0F1AE97A_A373_4DE7_B36A_23BE292D42B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <string>

const long  MAX_ERRORBUFFER=256;


class CError  
{
protected:
	char m_strErrormsg[MAX_ERRORBUFFER];
public:
	CError();
	virtual ~CError();
	void SetErrorMsg(char *msg)
	{
		strcpy(m_strErrormsg,msg);
	}
	virtual char *GetErrorMsg()
	{
		char msgtemp[MAX_ERRORBUFFER];
		strcpy(msgtemp,"General Error:");
		strcat(msgtemp,m_strErrormsg);
		strcpy(m_strErrormsg,msgtemp);
		return m_strErrormsg;
	}
	CError(char *msg)
	{
		SetErrorMsg(msg);
	}
};

#endif // !defined(AFX_ERROR_H__0F1AE97A_A373_4DE7_B36A_23BE292D42B2__INCLUDED_)
