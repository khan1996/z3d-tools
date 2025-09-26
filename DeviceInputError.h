// DeviceInputError.h: interface for the CDeviceInputError class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEVICEINPUTERROR_H__1F661071_76D1_4712_8943_137EA762B1DD__INCLUDED_)
#define AFX_DEVICEINPUTERROR_H__1F661071_76D1_4712_8943_137EA762B1DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Error.h"


class CDeviceInputError  : public CError
{
public:
	CDeviceInputError();
	CDeviceInputError(char *msg)
	{
		SetErrorMsg(msg);
	}
	char *GetErrorMsg()
	{
		char msgtemp[MAX_ERRORBUFFER];
		strcpy(msgtemp,"Device Input Error:");
		strcat(msgtemp,m_strErrormsg);
		strcpy(m_strErrormsg,msgtemp);
		return m_strErrormsg;
	};
	virtual ~CDeviceInputError();

};

#endif // !defined(AFX_DEVICEINPUTERROR_H__1F661071_76D1_4712_8943_137EA762B1DD__INCLUDED_)
