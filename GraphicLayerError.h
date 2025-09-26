// GraphicLayerError.h: interface for the CGraphicLayerError class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHICLAYERERROR_H__D06B571E_F0F8_41E8_AD22_257A312F48E0__INCLUDED_)
#define AFX_GRAPHICLAYERERROR_H__D06B571E_F0F8_41E8_AD22_257A312F48E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Error.h"


class CGraphicLayerError : public CError  
{
public:
	CGraphicLayerError();
	virtual ~CGraphicLayerError();
	char *GetErrorMsg()
	{
		char msgtemp[MAX_ERRORBUFFER];
		strcpy(msgtemp,"Graphics Layer Error:");
		strcat(msgtemp,m_strErrormsg);
		strcpy(m_strErrormsg,msgtemp);
		return m_strErrormsg;
	};
	CGraphicLayerError(char *msg)
	{
		SetErrorMsg(msg);
	};
};

#endif // !defined(AFX_GRAPHICLAYERERROR_H__D06B571E_F0F8_41E8_AD22_257A312F48E0__INCLUDED_)
