/* *********************************************************************  

* CBaseCacheMgr , CCacheObj 

* ���� : BaseCacheMgr.cpp 
* ��� : Caldron���� ���ο��� ���Ǵ� ��� CacheMgr���� ��� class
* history : 
	2004.01.16 wizardbug

********************************************************************** */  

#include "BaseCacheMgr.h"
#include "SceneManager.h"
#include "GMMemory.h"

namespace Caldron {namespace Base {
/*template < class T >
T *CBaseCacheMgr<T>::LoadData(const char *strFileName)
{
	T *pNode = new T;
	strcpy(pNode->m_strName,strFileName);
	CSceneManager::m_pNewMeshObjectContainer->AddObj(Base::CResourceLoaderObj(strFileName,pNode));

	unsigned long DataID = GetHashID(strFileName);
	Base::CACHEITER itr  = m_CacheTable.find(DataID);
	
	if(itr == m_CacheTable.end())
	{
		
		Base::CCacheObj *pObj = new Base::CCacheObj(pNode);
	
		m_CacheTable.insert(Base::CACHETABLEOBJ(DataID,pObj));
		m_iCachesNum++;
	}
	else 
	{// map �ȿ� �Ȱ��� Hash Index �� ������ ��� ����
		
		((*itr).second)->Add_Back(pNode);	
	}

	return pNode;

}
*/
/*
	template < class T >
unsigned long CBaseCacheMgr<T>::GetHashID(const char *strFileName) 
{
	unsigned long ulHashId = 0;

	int iLength = (int)strlen(strFileName);

	for(int i=0;i < iLength; i++) {
		ulHashId += (( i + 1) * strFileName[i]);
	}	
	return ulHashId;

}
*/
}}
