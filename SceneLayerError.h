// SceneLayerError.h: interface for the CSceneLayerError class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENELAYERERROR_H__1FF8B734_844A_420E_84CD_D6EA19F9D74F__INCLUDED_)
#define AFX_SCENELAYERERROR_H__1FF8B734_844A_420E_84CD_D6EA19F9D74F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <error.h>

class CSceneLayerError : public CError
{
public:
	char *GetErrorMsg()
	{
		char msgtemp[MAX_ERRORBUFFER];
		strcpy(msgtemp,"Scene Layer Error:");
		strcat(msgtemp,m_strErrormsg);
		strcpy(m_strErrormsg,msgtemp);
		return m_strErrormsg;
	};
	CSceneLayerError();
	virtual ~CSceneLayerError();
	CSceneLayerError(char *msg)
	{
		SetErrorMsg(msg);
	};

};

#endif // !defined(AFX_SCENELAYERERROR_H__1FF8B734_844A_420E_84CD_D6EA19F9D74F__INCLUDED_)
