// ObjectContainer.h: interface for the CObjectContainer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECTCONTAINER_H__0BA964B1_EE00_4AC8_AE46_07623AB11A0C__INCLUDED_)
#define AFX_OBJECTCONTAINER_H__0BA964B1_EE00_4AC8_AE46_07623AB11A0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "BaseDataDefine.h"
#include "MeshObject.h"


class CObjectContainer  
{
	class ObjectNode
	{
	public:
		long m_nUsed;
		char m_strObjectName[MAX_NAMEBUFFER];
		CMeshObject *m_pObject;
		bool m_isAlpha;
		bool m_isLight;
		ObjectNode(CMeshObject *pObject,char *strObjectName,bool isAlpha,bool isLight)
		{
			strcpy(m_strObjectName,strObjectName);
			strupr(m_strObjectName);	// 이름류는 대문자화. 비교시 대문자로 비교할것
			m_pObject=pObject;
			m_isAlpha=isAlpha;
			m_isLight=isLight;
			m_nUsed=1;
		}
	};
	std::vector<ObjectNode*> m_ObjectContainer;
public:
	void DeleteAllObject();
	void DeleteObject(char *strObjectName,bool isAlpha,bool isLight);
	void Load(char *strObjectName,bool isAlpha,bool isLight);
	CMeshObject* GetObject(char *strObjectName,bool isAlpha,bool isLight);
	CObjectContainer();
	virtual ~CObjectContainer();

};

#endif // !defined(AFX_OBJECTCONTAINER_H__0BA964B1_EE00_4AC8_AE46_07623AB11A0C__INCLUDED_)
