// SectorPlant.h: interface for the CSectorPlant class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SECTORPLANT_H__BE5DD3C8_AD95_4D99_B734_F94B603D4F05__INCLUDED_)
#define AFX_SECTORPLANT_H__BE5DD3C8_AD95_4D99_B734_F94B603D4F05__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "3DMath.h"

class CSectorPlantMap  
{
public:
	int m_IndexX,m_IndexY;
	//List<vector3> m_vecPlantPos;
	std::vector<unsigned char> m_PlantPosX;
	std::vector<unsigned char> m_PlantPosZ;
	std::vector<unsigned char> m_PlantKind;
	CSectorPlantMap();
	virtual ~CSectorPlantMap();
};

#endif // !defined(AFX_SECTORPLANT_H__BE5DD3C8_AD95_4D99_B734_F94B603D4F05__INCLUDED_)
