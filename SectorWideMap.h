// SectorWideMap.h: interface for the CSectorWideMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SECTORWIDEMAP_H__86AC8D7B_53C6_482C_82AD_A25B3E39F4D0__INCLUDED_)
#define AFX_SECTORWIDEMAP_H__86AC8D7B_53C6_482C_82AD_A25B3E39F4D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <NTexture.h>

class CSectorWideMap  
{			
public:
	CROSSM::CNTexture *m_WideMapTexture[3];
	CROSSM::CNTexture *m_DetailTexture[3];

	int m_nDetail;

	char m_strWidemapName[3][256];
	char m_strDetailName[3][256];
	int m_IndexX,m_IndexY;
	void GetWideTexture();
	void DelWideTexture();
	CSectorWideMap();
	virtual ~CSectorWideMap();

};

#endif // !defined(AFX_SECTORWIDEMAP_H__86AC8D7B_53C6_482C_82AD_A25B3E39F4D0__INCLUDED_)
