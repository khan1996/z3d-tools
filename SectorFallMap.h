// SectorFallMap.h: interface for the CSectorFallMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SECTORFALLMAP_H__083F5D79_16A3_44D2_8A36_F52BB78BFF24__INCLUDED_)
#define AFX_SECTORFALLMAP_H__083F5D79_16A3_44D2_8A36_F52BB78BFF24__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "3DMath.h"

class CSectorFallMap  
{
public:
	int m_IndexX,m_IndexY;
	std::vector<float> m_FallAddXList;
	std::vector<float> m_FallLeftList;
	std::vector<float> m_FallRightList;
	std::vector<float> m_FallHeightList;
	std::vector<color> m_FallColor;
	float m_fFallPosX,m_fFallPosY,m_fFallRot;

	CSectorFallMap();
	virtual ~CSectorFallMap();
};

#endif // !defined(AFX_SECTORFALLMAP_H__083F5D79_16A3_44D2_8A36_F52BB78BFF24__INCLUDED_)
