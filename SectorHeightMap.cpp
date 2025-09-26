// SectorHeightMap.cpp: implementation of the CSectorHeightMap class.
//
//////////////////////////////////////////////////////////////////////

#include "SectorHeightMap.h"
#include <memory.h>
#include <stdio.h>
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSectorHeightMap::CSectorHeightMap()
{
	//memset(m_pHeightData,0,sizeof(m_pHeightData));
	m_pHeightData=NULL;
	m_FilePos=0;
}

CSectorHeightMap::~CSectorHeightMap()
{
	if(m_FilePos==0)
	{
		if(m_pHeightData)
		{
			delete [] m_pHeightData;
		}
	}
}
