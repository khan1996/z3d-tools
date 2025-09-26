#include ".\pool.h"
#include "./PoolMgr.h"
#include "./Z3DByteDataObj.h"
#include "./NTexture.h"
#include "./ResourceMgr.h"
#include "../zalla3d scene class/NMesh.h"
#include "../zalla3d scene class/HouseObjectScene.h"
#include "../zalla3d scene class/HouseObject.h"
#include "../zalla3d scene class/ObjectScene.h"
#include "../zalla3d scene class/OctreeScene.h"
#include "../zalla3d scene class/Octree.h"
#include "GMMemory.h"



namespace CROSSM {
CPool::CPool(void)
{
}

CPool::~CPool(void)
{
	ReleaseAllData();
}
void CPool::AddPool(int iType,IPoolMgr *pMgr)
{
	POOLTABLEITER itr = m_PoolMap.find(iType);
	if(itr == m_PoolMap.end())
	{
		m_PoolMap.insert(POOLTABLEOBJ(iType,pMgr));
	
	}
}
void CPool::Init()
{
	
//	AddPool(POOL_BYTEDATAOBJ,new CPoolMgr<CZ3DByteDataObj>);
//	AddPool(POOL_DELRESOURCEINFOOBJ,new CPoolMgr<CDelResourceInfo>);
//	AddPool(POOL_RESOURCEMGROBJ,new CPoolMgr<CResourceMgrObj>);
//	AddPool(POOL_HOUSEOBJSCENE,new CPoolMgr<CHouseObjectScene>);
//	AddPool(POOL_HOUSEOBJ,new CPoolMgr<CHouseObject>);
//	AddPool(POOL_OBJECTSCENE,new CPoolMgr<CObjectScene>);
//	AddPool(POOL_OCTREEOBJ,new CPoolMgr<COctree>);
//	AddPool(POOL_POLYNODE,new CPoolMgr<PolyNode>);
	AddPool(RESOURCE_TEXTURE,new CPoolMgr<CNTexture>);
	AddPool(RESOURCE_MESH,new CPoolMgr<CNMesh>);
	AddPool(RESOURCE_OCTREESCENE,new CPoolMgr<COctreeScene>);

}

void CPool::Release()
{

}
void CPool::ReleaseAllData()
{
	for(POOLTABLEITER itr = m_PoolMap.begin(); itr != m_PoolMap.end();)
	{
		if(itr->second != NULL)
		{
			itr->second->ReleaseAll();
			SafeDelete(itr->second);
			itr->second = NULL;
		}
		itr++;
	}
	m_PoolMap.clear();

}
void CPool::ReleaseObj(int iType,void *pObj)
{
	if(pObj)
	{
		POOLTABLEITER itr = m_PoolMap.find(iType);
		if(itr != m_PoolMap.end())
		{
			if(itr->second)
			{
				itr->second->ReleaseObj(pObj);

			}
		}


	}
}
void *CPool::GetObj(int iType)
{
	POOLTABLEITER itr = m_PoolMap.find(iType);
	if(itr != m_PoolMap.end())
	{
		if(itr->second)
		{
			return itr->second->GetObj();
		}
	}
	return NULL;
}


}
