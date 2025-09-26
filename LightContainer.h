// LightContainer.h: interface for the CLightContainer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTCONTAINER_H__0471965C_61AD_4869_89DB_11720B5E2361__INCLUDED_)
#define AFX_LIGHTCONTAINER_H__0471965C_61AD_4869_89DB_11720B5E2361__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LightObject.h"
#include "BaseDataDefine.h"
#include <vector>


class CLightContainer  
{
	class LightObjectNode
	{
	public:
		long m_nUsed;
		char m_strLightName[MAX_NAMEBUFFER];
		CLightObject *m_pLightObject;
		LightObjectNode(CLightObject *pLightObject,char *strLightName)
		{
			strcpy(m_strLightName,strLightName);
			m_pLightObject=pLightObject;
			m_nUsed=1;
		}
	};
	std::vector<LightObjectNode*> m_LightContainer;
public:
	void DeleteAllObject();
	void DeleteLightObject(char *strLightObjectName,float fLightRange,color LightColor);
	void Load(char *strLightObjectName,float fLightRange,color LightColor);
	CLightObject * GetLightObject(char *strLightObjectName,float fLightRange,color LightColor);
	CLightContainer();
	virtual ~CLightContainer();
};

#endif // !defined(AFX_LIGHTCONTAINER_H__0471965C_61AD_4869_89DB_11720B5E2361__INCLUDED_)
