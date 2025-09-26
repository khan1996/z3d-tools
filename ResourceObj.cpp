#include ".\resourceobj.h"
#include "../zalla3d scene class/SceneManager.h"
#include "GMMemory.h"

namespace CROSSM {

CResourceObj::CResourceObj(char *strName) : m_bLoaded(false),m_iReloadingCount(0),m_pByteDataObj(NULL),m_iRefCount(0)
{
	strcpy(m_strName,strName);

}
CResourceObj::~CResourceObj(void)
{	
	if(m_pByteDataObj)
	{
		delete m_pByteDataObj;
		m_pByteDataObj = NULL;
	}

//	CSceneManager::_ReleaseObj(POOL_BYTEDATAOBJ,m_pByteDataObj);	
	m_pByteDataObj = NULL;
}

void CResourceObj::Init()
{
	if(m_pByteDataObj)
	{
		delete m_pByteDataObj;
		m_pByteDataObj = NULL;
//		CSceneManager::_ReleaseObj(POOL_BYTEDATAOBJ,m_pByteDataObj);
	}

	m_pByteDataObj = new CZ3DByteDataObj();
//	m_pByteDataObj = (CZ3DByteDataObj *)CSceneManager::_CreateObj(POOL_BYTEDATAOBJ);
}
bool CResourceObj::LoadByte(const char *strName)
{
	if(m_pByteDataObj)
	{
		if(strName)
			return m_pByteDataObj->LoadByte(strName);
		else
			return m_pByteDataObj->LoadByte(m_strName);

	}
	return false;
}
void CResourceObj::ReleaseByte()
{
	if(m_pByteDataObj)
	{
		delete m_pByteDataObj;
		m_pByteDataObj = NULL;
	}

//	CSceneManager::_ReleaseObj(POOL_BYTEDATAOBJ,m_pByteDataObj);
	m_pByteDataObj = NULL;
}

int CResourceObj::Release() 
	{
		if(m_iRefCount > 0)
			--m_iRefCount;

		int iReturn = m_iRefCount;
		
		if(m_iRefCount <= 0)
		{

#ifndef SG_RESOURCE_RELEASEPROCESS

			CSceneManager::_ReleaseObj(m_iResourceType,this);

#endif
		}
		return iReturn;
	}
#ifdef SG_RESOURCE_RELEASEPROCESS
int CResourceObj::RealRelease()
	{
		//if(m_iRefCount > 0)
		//	--m_iRefCount;
		int iReturn = m_iRefCount;
		if(m_iRefCount <= 0)
		{
			CSceneManager::_ReleaseObj(m_iResourceType,this);	
		}
		return iReturn;
	}
#endif



}

