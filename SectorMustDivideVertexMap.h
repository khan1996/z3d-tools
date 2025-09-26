// SectorMustDivideVertexMap.h: interface for the CSectorMustDivideVertexMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SECTORMUSTDIVIDEVERTEXMAP_H__CC4CCD39_EE16_48C3_BDA3_47C67E741515__INCLUDED_)
#define AFX_SECTORMUSTDIVIDEVERTEXMAP_H__CC4CCD39_EE16_48C3_BDA3_47C67E741515__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <d3d8.h>
#include <vector>
#include "SectorDefine.h"

class CSectorMustDivideVertexMap  
{
public:
	int m_IndexX,m_IndexY;
	std::vector<POINT> m_ptMustDivideVertexPosList;
	int m_pLeftNodeList[1<<(VARIANCE_DEPTH)],m_pRightNodeList[1<<(VARIANCE_DEPTH)];
	//List<int*> m_pLeftNodeList,m_pRightNodeList;
	CSectorMustDivideVertexMap();
	virtual ~CSectorMustDivideVertexMap();

};

#endif // !defined(AFX_SECTORMUSTDIVIDEVERTEXMAP_H__CC4CCD39_EE16_48C3_BDA3_47C67E741515__INCLUDED_)
