// SectorWaterMap.h: interface for the CSectorWaterMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SECTORWATERMAP_H__E1E93A06_0C8B_40E6_884E_5112C6980772__INCLUDED_)
#define AFX_SECTORWATERMAP_H__E1E93A06_0C8B_40E6_884E_5112C6980772__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "3DMath.h"

class CSectorWaterMap  
{
public:
	bool m_bWaterRelection;
	int m_IndexX,m_IndexY;
	float m_fWaterHeight;
	float m_fWaterSizeX,m_fWaterSizeY;
	float m_fWaterPosX,m_fWaterPosY;
	color m_WaterColor;
	CSectorWaterMap();
	virtual ~CSectorWaterMap();

};

#endif // !defined(AFX_SECTORWATERMAP_H__E1E93A06_0C8B_40E6_884E_5112C6980772__INCLUDED_)
