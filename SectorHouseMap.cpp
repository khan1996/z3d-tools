// SectorHouseMap.cpp: implementation of the CSectorHouseMap class.
//
//////////////////////////////////////////////////////////////////////

#include "SectorHouseMap.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSectorHouseMap::CSectorHouseMap()
{
	m_IndexX = 0;
	m_IndexY = 0;
}

CSectorHouseMap::~CSectorHouseMap()
{
	int i = 0;
	int iSize = m_strOutName.size();
	for(i=0;i<(int)iSize;++i)
	{
		if(m_strOutName[i])
			delete[] m_strOutName[i];
	}

	iSize = m_strMedName.size();
	for(i=0;i<(int)iSize;++i)
	{
		if(m_strMedName[i])
			delete[] m_strMedName[i];
	}

	iSize = m_strInName.size();
	for(i=0;i<(int)iSize;++i)
	{
		if(m_strInName[i])
			delete[] m_strInName[i];
	}

	iSize = m_strBspName.size();
	for(i=0;i<(int)iSize;++i)
	{
		if(m_strBspName[i])
			delete[] m_strBspName[i];
	}

	m_TM.clear();
	m_HouseSceneID.clear();
	m_strInName.clear();
	m_strMedName.clear();
	m_strOutName.clear();
	m_strBspName.clear();
}
