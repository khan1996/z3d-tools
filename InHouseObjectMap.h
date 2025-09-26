// InHouseObjectMap.h: interface for the CInHouseObjectMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INHOUSEOBJECTMAP_H__EBFE3FAB_0291_4FC1_83C5_403C61804DA4__INCLUDED_)
#define AFX_INHOUSEOBJECTMAP_H__EBFE3FAB_0291_4FC1_83C5_403C61804DA4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "3DMath.h"
#include "BaseDataDefine.h"
#include <vector>


class CInHouseObjectMap  
{	
public:
	char m_strInName[MAX_NAMEBUFFER];
	char m_strMedName[MAX_NAMEBUFFER];
	char m_strOutName[MAX_NAMEBUFFER];
	char m_strBspName[MAX_NAMEBUFFER];
	std::vector<char*> m_strObjectNameList;
	std::vector<matrix> m_matObjectList;
	std::vector<bool> m_isAlpha;
	std::vector<bool> m_isLight;
	std::vector<long> m_ObjectID;	

	std::vector<char*> m_strLightNameList;
	std::vector<matrix> m_matLightList;
	std::vector<float> m_fLightRange;
	std::vector<color> m_LightColor;
	std::vector<long> m_LightID;
	
	CInHouseObjectMap();
	virtual ~CInHouseObjectMap();
};

#endif // !defined(AFX_INHOUSEOBJECTMAP_H__EBFE3FAB_0291_4FC1_83C5_403C61804DA4__INCLUDED_)
