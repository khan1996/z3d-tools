#include ".\resource.h"
#include "./ResourceMgr.h"
#include "./NTexture.h"
#include "../zalla3d scene class/NMesh.h"
#include "../zalla3d scene class/OctreeScene.h"
#include "GMMemory.h"

namespace CROSSM {
CResource::CResource(void)
{
}

CResource::~CResource(void)
{
	for(RESOURCETABLEITER itr = m_ResourceMgrMap.begin(); itr != m_ResourceMgrMap.end();)
	{
		if(itr->second != NULL)
		{
			SafeDelete(itr->second);
		}
		itr++;
	}
	m_ResourceMgrMap.clear();

}
void CResource::Init()
{
	m_ResourceMgrMap.clear();
	AddResourceMgr(RESOURCE_TEXTURE,new CResourceMgr<CNTexture>);
	AddResourceMgr(RESOURCE_MESH,new CResourceMgr<CNMesh>);
	AddResourceMgr(RESOURCE_OCTREESCENE,new CResourceMgr<COctreeScene>);
}

void CResource::Update()
{
	RESOURCETABLEITER itr = m_ResourceMgrMap.find(RESOURCE_TEXTURE);
	if(itr != m_ResourceMgrMap.end())
	{
		itr->second->Update();
	}

	itr = m_ResourceMgrMap.find(RESOURCE_MESH);
	if(itr != m_ResourceMgrMap.end())
	{
		itr->second->Update();
	}

	itr = m_ResourceMgrMap.find(RESOURCE_OCTREESCENE);
	if(itr != m_ResourceMgrMap.end())
	{
		itr->second->Update();
	}

}

void CResource::AddResourceMgr(int iType,IResourceMgr *pMgr)
{
	RESOURCETABLEITER itr = m_ResourceMgrMap.find(iType);
	if(itr == m_ResourceMgrMap.end())
	{

		if(pMgr)
		{
			pMgr->SetType(iType);	// Resource Type Setting.
			m_ResourceMgrMap.insert(RESOURCETABLEOBJ(iType,pMgr));
		}
	}
	else
	{
//		CLogMgr::_LogError(" CResource::AddResourceMgr(), 이미 같은 이름 으로 resourcemgr이 등록되어 있습니다");

	}
}
void CResource::Release()
{
	for(RESOURCETABLEITER itr = m_ResourceMgrMap.begin(); itr != m_ResourceMgrMap.end();)
	{
		if(itr->second != NULL)
		{
			SafeDelete(itr->second);
		}
		itr++;
	}
	m_ResourceMgrMap.clear();

}
void CResource::ReleaseAllData()
{
	for(RESOURCETABLEITER itr = m_ResourceMgrMap.begin(); itr != m_ResourceMgrMap.end();)
	{
		if(itr->second != NULL)
		{
			itr->second->ReleaseAllData();
		}
		itr++;
	}
}
void *CResource::GetData(int iType,const char *strName)
{
	RESOURCETABLEITER itr = m_ResourceMgrMap.find(iType);
	if(itr != m_ResourceMgrMap.end())
	{
		return itr->second->GetData(strName);

	}
	
	return NULL;
}
void CResource::ReleaseObj(int iType,char *strName)
{

	RESOURCETABLEITER itr = m_ResourceMgrMap.find(iType);
	if(itr != m_ResourceMgrMap.end())
	{
		itr->second->ReleaseData(strName);
	}
}

// 2005.01.10 yundi ImmediateLoad 관련 추가
void CResource::LockImmediateLoad(int iType)
{
	RESOURCETABLEITER itr = m_ResourceMgrMap.find(iType);
	if(itr != m_ResourceMgrMap.end())
	{
		itr->second->LockImmediateLoad();
	}
}
void CResource::UnlockImmediateLoad(int iType)
{
	RESOURCETABLEITER itr = m_ResourceMgrMap.find(iType);
	if(itr != m_ResourceMgrMap.end())
	{
		itr->second->UnlockImmediateLoad();
	}
}
}
