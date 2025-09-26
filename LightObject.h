// LightObject.h: interface for the CLightObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTOBJECT_H__97B7497C_0ABB_40C7_A225_DB974DB7ECDC__INCLUDED_)
#define AFX_LIGHTOBJECT_H__97B7497C_0ABB_40C7_A225_DB974DB7ECDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "MeshObject.h"
#include <3DMath.h>
#include "BaseDataDefine.h"


class CLightObject  
{	
public:	
	float m_fLightRange;
	color m_LightColor;
	CMeshObject *m_pLightObject;
	char m_strLightObjectName[MAX_NAMEBUFFER];
	CLightObject();
	virtual ~CLightObject();
};

#endif // !defined(AFX_LIGHTOBJECT_H__97B7497C_0ABB_40C7_A225_DB974DB7ECDC__INCLUDED_)
