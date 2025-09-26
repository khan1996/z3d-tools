#pragma once
#include "iresourcemgr.h"
#include "CrossMHeader.h"
#include "ResourceObj.h"
#include "../zalla3d scene class/SceneManager.h"


namespace CROSSM {
	

#define RESOURCE_MAXCOUNT	70
#define RESOURCE_MAXDELCOUNT 5

#define SG_RESOURCE_RELEASEPROCESS
class CResourceMgrObj 
{
	

public:

	CResourceMgrObj() : m_iObjsNum(0)
	{
		m_lstObjs.clear();
	}
	CResourceMgrObj(int iResourceType,CResourceObj *pObj) : m_iObjsNum(1)
	{
		m_lstObjs.clear();
		m_lstObjs.push_back(pObj);
		pObj->SetType(iResourceType);
		pObj->AddRef();
	}
	~CResourceMgrObj()
	{
		for(int i = 0; i < m_iObjsNum; i++ )
		{
			//SafeDelete(m_lstObjs[i]);
			if(m_lstObjs[i])
			{
				CSceneManager::_ReleaseObj(m_lstObjs[i]->GetType(),m_lstObjs[i]);
				m_lstObjs[i] = NULL;
			}
		}
		m_lstObjs.clear();
		
	}
	int AddObj(int iResourceType,CResourceObj *pObj)
	{
		m_iObjsNum++;
		m_lstObjs.push_back(pObj);
		pObj->AddRef();
		pObj->SetType(iResourceType);
		return m_iObjsNum;
	}
	CResourceObj *GetObj(int iNum)
	{
		if(iNum < m_iObjsNum)
		{
			m_lstObjs[iNum]->AddRef();
			return m_lstObjs[iNum];

		}
		return NULL;
	}
	bool IsLoaded(int iNum)
	{
		
		if(iNum < m_iObjsNum)
		{
			return m_lstObjs[iNum]->IsLoaded();

		}

		return false;

	}
	bool ReleaseObj(int iNum)
	{
		if(iNum < m_iObjsNum)
		{
			
		//	if(!m_lstObjs[iNum]->IsLoaded())	// Load 가 끝나지 않은객체는 지워지지 않는다.
		//		return false;


			if(m_lstObjs[iNum]->Release() <= 0)
			{
#ifndef SG_RESOURCE_RELEASEPROCESS	
				m_iObjsNum--;
				m_lstObjs.erase(&m_lstObjs[iNum]);
#endif
				return true;

			}

		}
		return false;
	}
#ifdef SG_RESOURCE_RELEASEPROCESS
	bool RealReleaseObj(int iNum)
	{
		if(iNum < m_iObjsNum)
		{
			if(m_lstObjs[iNum]->GetRef() <= 0)		// DelList 에 있는 노드들 지울때 사용하므로 실제적 ref count는 감소하면 안된다.
			{
				if(m_lstObjs[iNum]->RealRelease() <= 0)
				{
					m_iObjsNum--;
					m_lstObjs.erase(m_lstObjs.begin() + iNum);
					return true;

				}
			}

		}
		return false;
	}

#endif

	
	int								m_iObjsNum;
	std::vector<CResourceObj *>		m_lstObjs;
	

};

class CDelResourceInfo
{
public:
	unsigned long	m_iID;
	char			m_strName[256];
	void SetValue(unsigned long iID,const char *strName)
	{
		m_iID = iID;
		strcpy(m_strName,strName);

	}
	CDelResourceInfo() : m_iID(0)
	{
		memset(m_strName,0,sizeof(char) * 256);
	}
	CDelResourceInfo(unsigned long iID,const char *strName)
	{
		m_iID = iID;
		strcpy(m_strName,strName);


	}
	~CDelResourceInfo()
	{}

};


typedef std::map<unsigned long , CResourceMgrObj *> RESOURCEMGRTABLE;
typedef RESOURCEMGRTABLE::value_type RESOURCEMGRTABLEOBJ;
typedef RESOURCEMGRTABLE::iterator RESOURCEMGRTABLEITER;

template <class T>
class CResourceMgr :
	public IResourceMgr
{
public:
	

	CResourceMgr(void) : m_iResourceObjsNum(0),	m_iMaxCount(RESOURCE_MAXCOUNT),m_DelObjNums(0)
	{
		m_ResourceTable.clear();
		m_iDelCount = 0;

		// 2005.01.10 yundi 즉시 로드 상태를 위한 변수 추가
		m_nLockedImmediateLoadCount = 0;
	}
	virtual ~CResourceMgr(void)
	{
		if(m_iResourceObjsNum > 0)
		{
			for(RESOURCEMGRTABLEITER itr = m_ResourceTable.begin(); itr != m_ResourceTable.end();)
			{
				if(itr->second != NULL)
				{
					SafeDelete(itr->second);
					//CSceneManager::_ReleaseObj(POOL_RESOURCEMGROBJ,itr->second);
					itr->second = NULL;

				}
				itr++;
			}
			m_ResourceTable.clear();
			m_iResourceObjsNum = 0;
		}
		while(!m_DelObjQueue.empty())
		{
			
			CDelResourceInfo *pDelObj = m_DelObjQueue.front();
			m_DelObjQueue.pop();
			SafeDelete(pDelObj);
//			CSceneManager::_ReleaseObj(POOL_DELRESOURCEINFOOBJ,pDelObj);
		}

	}
	void SetMaxCount(int iCount)
	{
		m_iMaxCount = iCount;

	}
	virtual void ReleaseAllData()
	{
		if(m_iResourceObjsNum > 0)
		{
			for(RESOURCEMGRTABLEITER itr = m_ResourceTable.begin(); itr != m_ResourceTable.end();)
			{
				if(itr->second != NULL)
				{
					SafeDelete(itr->second);
					//CSceneManager::_ReleaseObj(POOL_RESOURCEMGROBJ,itr->second);
					itr->second = NULL;

				}
				itr++;
			}
			m_ResourceTable.clear();
			m_iResourceObjsNum = 0;
		}
	}
	virtual void ReleaseData(const char *strName)
	{
		
		bool bEraseObj = false;
		unsigned long DataID = GetHashID(strName);
			
		RESOURCEMGRTABLEITER itr  = m_ResourceTable.find(DataID);
		
		if(itr != m_ResourceTable.end()) 
		{
			if((*itr).second != NULL) 
			{
				for(int iNum = 0; iNum < (*itr).second->m_iObjsNum; iNum++)
				{
					if(!strcmp((*itr).second->m_lstObjs[iNum]->m_strName,strName))
					{
						bool bRelease = false;
#ifndef SG_RESOURCE_RELEASEPROCESS
						if((bRelease = ((*itr).second)->ReleaseObj(iNum)) && !(*itr).second->m_iObjsNum)
						{
							// Release 완료(map의 노드 역시 삭제).
							SafeDelete(itr->second);
							//CSceneManager::_ReleaseObj(POOL_RESOURCEMGROBJ,itr->second);
							itr->second = NULL;
							m_ResourceTable.erase(itr);
							
						}
#else
						if(((*itr).second)->ReleaseObj(iNum))
						{
							//CDelResourceInfo *pDel = new CDelResourceInfo(DataID,strName);
							CDelResourceInfo *pDel = new CDelResourceInfo();//(CDelResourceInfo *)CSceneManager::_CreateObj(POOL_DELRESOURCEINFOOBJ);
							pDel->SetValue(DataID,strName);
							m_DelObjQueue.push(pDel);
							m_DelObjNums++;

						}
#endif
						if(bRelease)
						{
							m_iResourceObjsNum--;
						}

					}
				}

			}

		}

	}
	virtual void *GetData(const char *strName)
	{

		unsigned long DataID = GetHashID(strName);
			
		RESOURCEMGRTABLEITER itr  = m_ResourceTable.find(DataID);
		
		if(itr == m_ResourceTable.end()) 
		{
			return LoadData(strName);	
		}
		else	// Table 안에 이미 texture 가 로드되어 있을때 
		{
			if((*itr).second != NULL) 
			{
				for(int iNum = 0; iNum < (*itr).second->m_iObjsNum; iNum++)
				{
					if(!strcmp((*itr).second->m_lstObjs[iNum]->m_strName,strName))
					{
						return (*itr).second->GetObj(iNum);
					}
				}
				return LoadData(strName);	// Table 의 obj는 있지만 내가 원하는 obj 없을시
			}
			else
			{
				CResourceMgrObj *pObj = new CResourceMgrObj();
//				CResourceMgrObj *pObj = (CResourceMgrObj *)CSceneManager::_CreateObj(POOL_RESOURCEMGROBJ);
				(*itr).second = pObj;
				return LoadData(strName);	// Table 의 obj는 있지만 내가 원하는 obj 없을시.				

			}

		}
		
		return NULL;


	}
protected:
#ifdef SG_RESOURCE_RELEASEPROCESS
	void DelObjClear(int iNum)
	{

		for(int i = 0; i < iNum; i++ )
		{
			if(!m_DelObjQueue.empty())
			{
				CDelResourceInfo *pDelObj = m_DelObjQueue.front();
				m_DelObjQueue.pop();
				m_DelObjNums--;

				if(pDelObj)
				{
					if(!ResourceLoaded(pDelObj->m_iID,pDelObj->m_strName))
					{
						m_DelObjQueue.push(pDelObj);
					
					}
					else
					{
						RealRelease(pDelObj->m_iID,pDelObj->m_strName);				
						//SafeDelete(pDelObj);
						SafeDelete(pDelObj);
//						CSceneManager::_ReleaseObj(POOL_DELRESOURCEINFOOBJ,pDelObj);
					}
				}
			}
			else
			{
				break;
			}
		}

	}
	bool ResourceLoaded(unsigned long ID,char *strName)
	{
		RESOURCEMGRTABLEITER itr  = m_ResourceTable.find(ID);
		
		if(itr != m_ResourceTable.end()) 
		{
			if((*itr).second != NULL) 
			{
				for(int iNum = 0; iNum < (*itr).second->m_iObjsNum; iNum++)
				{
					if(!strcmp((*itr).second->m_lstObjs[iNum]->m_strName,strName))
					{
						if(((*itr).second)->IsLoaded(iNum))
							return true;
						else 
							return false;

					}

				}

			}
		}

		return true;

	}
	void RealRelease(unsigned long ID,char *strName)
	{
		bool bEraseObj = false;
			
		RESOURCEMGRTABLEITER itr  = m_ResourceTable.find(ID);
		
		if(itr != m_ResourceTable.end()) 
		{
			if((*itr).second != NULL) 
			{
				for(int iNum = 0; iNum < (*itr).second->m_iObjsNum; iNum++)
				{
					if(!strcmp((*itr).second->m_lstObjs[iNum]->m_strName,strName))
					{
						bool bRelease = false;

						if((bRelease = ((*itr).second)->RealReleaseObj(iNum)) && !(*itr).second->m_iObjsNum)
						{
							// Release 완료(map의 노드 역시 삭제).
							SafeDelete(itr->second);
							//CSceneManager::_ReleaseObj(POOL_RESOURCEMGROBJ,itr->second);
							itr->second = NULL;

							m_ResourceTable.erase(itr);
							if(bRelease)
							{
								m_iResourceObjsNum--;
							}
							break;
							
						}

						if(bRelease)
						{
							m_iResourceObjsNum--;
						}

					}
				}

			}

		}

	}
#endif

	virtual void *LoadData(const char *strName)
	{
		//T *pNode = new T;
		T *pNode = (T *)CSceneManager::_CreateObj(m_iResourceType);
		strcpy(pNode->m_strName,strName);
		if( m_nLockedImmediateLoadCount > 0)
		{
			CSceneManager::_LoadResourceObj(pNode, true);
		}
		else
		{
			CSceneManager::_LoadResourceObj(pNode, false);
		}
		/*Scene::CMainSceneMgr::_GetResourceLoader()->AddObj(pNode);
		*/
		unsigned long DataID = GetHashID(strName);
		RESOURCEMGRTABLEITER itr  = m_ResourceTable.find(DataID);
		
		m_iResourceObjsNum++;
#ifdef SG_RESOURCE_RELEASEPROCESS
		if(m_iMaxCount < m_iResourceObjsNum)
		{
			SetDelCount(m_iResourceObjsNum - m_iMaxCount);
			//DelObjClear(m_iResourceObjsNum - m_iMaxCount);
		}
#endif
		if(itr == m_ResourceTable.end())
		{
			
			//CResourceMgrObj*pObj = new CResourceMgrObj(m_iResourceType,pNode);	
			CResourceMgrObj *pObj = new CResourceMgrObj();//(CResourceMgrObj *)CSceneManager::_CreateObj(POOL_RESOURCEMGROBJ);
			pObj->AddObj(m_iResourceType,pNode);
			
			m_ResourceTable.insert(RESOURCEMGRTABLEOBJ(DataID,pObj));
			
		}
		else 
		{// map 안에 똑같은 Hash Index 를 가지는 요소 존재
			
			((*itr).second)->AddObj(m_iResourceType,pNode);	
		}

		return pNode;


	}
	void SetDelCount(int i)
	{
		m_iDelCount = i;
	}
	virtual void Update()
	{
		int iNum;
		if(m_iDelCount > 0)
		{
			
			iNum = (RESOURCE_MAXDELCOUNT < m_iDelCount) ? RESOURCE_MAXDELCOUNT : m_iDelCount;
			m_iDelCount -= iNum;
			DelObjClear(iNum);
		}
	}

	void LockImmediateLoad()
	{
		++m_nLockedImmediateLoadCount;
	}

	void UnlockImmediateLoad()
	{
		if( m_nLockedImmediateLoadCount > 0 )
		{
			--m_nLockedImmediateLoadCount;
		}
	}

	RESOURCEMGRTABLE				m_ResourceTable;
	int								m_iResourceObjsNum;

	std::queue<CDelResourceInfo *>	m_DelObjQueue;
	int								m_DelObjNums;

	int								m_iMaxCount;
	int								m_iDelCount;

	// 2005.01.10 yundi 즉시 로드 상태를 위한 변수 추가
	int								m_nLockedImmediateLoadCount;



};


}
