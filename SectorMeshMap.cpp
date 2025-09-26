// SectorMeshMap.cpp: implementation of the CSectorMeshMap class.
//
//////////////////////////////////////////////////////////////////////

#include "SectorMeshMap.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSectorMeshMap::CSectorMeshMap()
{

}

CSectorMeshMap::~CSectorMeshMap()
{
	for(int i=0;i<(int)m_strObjectName.size();i++)
	{
		delete[] m_strObjectName[i];
	}
	m_strObjectName.clear();
}
