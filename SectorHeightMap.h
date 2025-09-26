// SectorHeightMap.h: interface for the CSectorHeightMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SECTORHEIGHTMAP_H__C14E97F3_3D23_4980_B11A_D7171F00D1D6__INCLUDED_)
#define AFX_SECTORHEIGHTMAP_H__C14E97F3_3D23_4980_B11A_D7171F00D1D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "SectorDefine.h"

class CSectorHeightMap  
{	
public:
	int m_IndexX,m_IndexY;
	//float m_pHeightData[SECTORSX*SECTORSY];
	float *m_pHeightData;
	long m_FilePos;
	CSectorHeightMap();
	virtual ~CSectorHeightMap();
};

#endif // !defined(AFX_SECTORHEIGHTMAP_H__C14E97F3_3D23_4980_B11A_D7171F00D1D6__INCLUDED_)
