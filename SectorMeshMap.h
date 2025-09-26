// SectorMeshMap.h: interface for the CSectorMeshMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SECTORMESHMAP_H__000E78B9_B6F4_435B_8616_FD813F144438__INCLUDED_)
#define AFX_SECTORMESHMAP_H__000E78B9_B6F4_435B_8616_FD813F144438__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "3DMath.h"

class CSectorMeshMap  
{
public:
	std::vector<matrix> m_TM;
	int m_IndexX,m_IndexY;
	std::vector<long> 	m_ObjectSceneID;
	std::vector<char*> m_strObjectName;
	std::vector<bool> m_isLight;
	std::vector<bool> m_isAlpha;
	CSectorMeshMap();
	virtual ~CSectorMeshMap();

};

#endif // !defined(AFX_SECTORMESHMAP_H__000E78B9_B6F4_435B_8616_FD813F144438__INCLUDED_)
