// InHouseObjectMap.cpp: implementation of the CInHouseObjectMap class.
//
//////////////////////////////////////////////////////////////////////

#include "InHouseObjectMap.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInHouseObjectMap::CInHouseObjectMap()
{

}

CInHouseObjectMap::~CInHouseObjectMap()
{
	for(int i=0;i<(int)m_strObjectNameList.size();i++)
	{
		if(m_strObjectNameList[i]) {
			delete[] m_strObjectNameList[i];
			m_strObjectNameList[i] = NULL;
		}
	}
	m_strObjectNameList.clear();

	for(int i=0;i<(int)m_strLightNameList.size();i++)
	{
		if(m_strLightNameList[i]) {
			delete[] m_strLightNameList[i];
			m_strLightNameList[i] = NULL;
		}
	}	
	m_strLightNameList.clear();
}