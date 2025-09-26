// SectorHouseMap.h: interface for the CSectorHouseMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SECTORHOUSEMAP_H__3FF5C24F_2642_4ABC_8D81_4C68A712BFD8__INCLUDED_)
#define AFX_SECTORHOUSEMAP_H__3FF5C24F_2642_4ABC_8D81_4C68A712BFD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "BaseDataDefine.h"
#include <vector>
#include <3DMath.h>

class CSectorHouseMap  
{	
public:
	int m_IndexX,m_IndexY;

	std::vector<matrix> m_TM;
	std::vector<long> 	m_HouseSceneID;
	std::vector<char*> m_strOutName;
	std::vector<char*> m_strMedName;
	std::vector<char*> m_strInName;
	std::vector<char*> m_strBspName;

	CSectorHouseMap();
	virtual ~CSectorHouseMap();
};

#endif // !defined(AFX_SECTORHOUSEMAP_H__3FF5C24F_2642_4ABC_8D81_4C68A712BFD8__INCLUDED_)
