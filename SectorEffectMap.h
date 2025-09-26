// SectorEffectMap.h: interface for the CSectorEffectMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SECTOREFFECTMAP_H__D21C88F9_C058_4BDC_A004_F6CF723B88DE__INCLUDED_)
#define AFX_SECTOREFFECTMAP_H__D21C88F9_C058_4BDC_A004_F6CF723B88DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <d3d8.h>
#include <vector>
#include "3DMath.h"
#include "SectorDefine.h"

class CSectorEffectMap  
{
public:
	int Sector_EffectNum;
	int m_IndexX,m_IndexY;	
	std::vector<vector3> m_vecEffectPos;
	std::vector<vector3> m_vecEffectRot;
	std::vector<char*> m_strEffectName;
	int pick_index;
	CSectorEffectMap();
	virtual ~CSectorEffectMap();

};

#endif // !defined(AFX_SECTOREFFECTMAP_H__D21C88F9_C058_4BDC_A004_F6CF723B88DE__INCLUDED_)
