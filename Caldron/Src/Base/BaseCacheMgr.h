
#if !defined(AFX_BASECACHEMGR_H__C53D076B_14CF_47B5_B21C_F7EF3477C7EA__INCLUDED_)
#define AFX_BASECACHEMGR_H__C53D076B_14CF_47B5_B21C_F7EF3477C7EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( disable : 4786 )
#include "../../include/Caldron.h"
#include "./LoadedObj.h"
#include "./ResourceLoader.h"


namespace Caldron {
	namespace Base {
/* *********************************************************************  

* CCacheObj

* ���� : BaseCacheMgr.h 
* ��� : Caldron���� ���ο��� ���Ǵ� CacheMgr���� data �� �ٷ����� ����
* history : 
	2004.01.16 wizardbug

********************************************************************** */  

class CCacheObj {
public:
	std::vector<CLoadedObj *>	m_lstDatas;
	int							m_iDatasNum;
	CCacheObj() : m_iDatasNum(0)
	{
		m_lstDatas.clear();
	}
	CCacheObj(CLoadedObj *pObj)  : m_iDatasNum(0)
	{
		m_lstDatas.clear();
		m_lstDatas.push_back(pObj);
		pObj->m_iReferenceCount++;
		m_iDatasNum++;
	}
	void Add_Back(CLoadedObj *pObj)
	{
		m_lstDatas.push_back(pObj);
		pObj->m_iReferenceCount++;
		m_iDatasNum++;
	}
	void Remove(int iNum)
	{
		if(m_iDatasNum <= iNum)
			return;
		if(m_lstDatas[iNum] && m_lstDatas[iNum]->m_bLoaded)
		{
			if(!(--m_lstDatas[iNum]->m_iReferenceCount))
			{
				SafeDelete(m_lstDatas[iNum]);
				m_lstDatas.erase(m_lstDatas.begin() + iNum);
				m_iDatasNum--;	
			}
		}
		
	}
	void AddRef(int iNum)
	{
		if(m_iDatasNum <= iNum)
			return;
		if(m_lstDatas[iNum])
		{
			m_lstDatas[iNum]->m_iReferenceCount++;
		}
	}
	~CCacheObj() {
		if(m_iDatasNum > 0)
		{
			for(int i=0;i < m_iDatasNum; i++) 
			{
				if(m_lstDatas[i]) 
				{
					SafeDelete(m_lstDatas[i]);
				}
			}
		}
		m_iDatasNum =  0;
		m_lstDatas.clear();
	}

		
			
};

typedef std::map<unsigned long , CCacheObj *> CACHETABLE;
typedef CACHETABLE::value_type CACHETABLEOBJ;
typedef CACHETABLE::iterator CACHEITER;
/* *********************************************************************  

* CBaseCacheMgr

* ���� : BaseCacheMgr.h 
* ��� : Caldron���� ���ο��� ���Ǵ� ��� CacheMgr���� ��� class
* history : 
	2004.01.16 wizardbug

********************************************************************** */  
const int DEFAULT_MAXCACHENUM = 100;
template <class T>
class CBaseCacheMgr  
{
protected:
	
	unsigned long GetHashID(const char *strFileName)
	{
		unsigned long ulHashId = 0;

		int iLength = (int)strlen(strFileName);

		for(int i=0;i < iLength; i++) {
			ulHashId += (( i + 1) * strFileName[i]);
		}	
		return ulHashId;


	}

	CACHETABLE m_CacheTable;
	int	m_iCachesNum;


public:
	
	CBaseCacheMgr(): m_iCachesNum(0)
	{
		m_CacheTable.clear();
	}
	virtual ~CBaseCacheMgr()
	{
		//if(m_iCachesNum > 0) 
		//{
			for(CACHEITER Itr = m_CacheTable.begin(); Itr != m_CacheTable.end();) 
			{
				if(Itr->second != NULL) 
				{
					SafeDelete(Itr->second);
				}
				
				Itr++;
			}
			m_CacheTable.clear();
			m_iCachesNum = 0;

		//}

		
	}
	void DeleteAllData()
	{
	//	if(m_iCachesNum > 0) 
	//	{
			for(CACHEITER Itr = m_CacheTable.begin(); Itr != m_CacheTable.end();) 
			{
				if(Itr->second != NULL) 
				{
					SafeDelete(Itr->second);
				}
				
				Itr++;
			}
			m_CacheTable.clear();
			m_iCachesNum = 0;

	//	}

	}
	void DeleteData(const char *strFileName)
	{

		unsigned long DataID = GetHashID(strFileName);
			
		Base::CACHEITER itr  = m_CacheTable.find(DataID);
		
		if(itr != m_CacheTable.end()) 
		{
			if((*itr).second != NULL) 
			{
				for(int iNum = 0; iNum < (*itr).second->m_iDatasNum; iNum++)
				{
					if(!strcmp((*itr).second->m_lstDatas[iNum]->m_strName,strFileName))
					{
						((*itr).second)->Remove(iNum);				
					}
				}

			}

		}
	}

	T *GetData(const char *strFileName)
	{
		unsigned long DataID = GetHashID(strFileName);
			
		Base::CACHEITER itr  = m_CacheTable.find(DataID);
		
		if(itr == m_CacheTable.end()) 
		{
			return LoadData(strFileName);	
		}
		else	// Table �ȿ� �̹� texture �� �ε�Ǿ� ������ 
		{
			if((*itr).second != NULL) 
			{
				for(int iNum = 0; iNum < (*itr).second->m_iDatasNum; iNum++)
				{
					if(!strcmp((*itr).second->m_lstDatas[iNum]->m_strName,strFileName))
					{
						(*itr).second->AddRef(iNum);
						return (T *)((*itr).second->m_lstDatas[iNum]);				
					}
				}
				return LoadData(strFileName);	// Table �� obj�� ������ ���� ���ϴ� obj ������
			}

		}
		
		return NULL;


	}
	T *LoadData(const char *strFileName)
	{
		T *pNode = new T;
		strcpy(pNode->m_strName,strFileName);
		CResourceLoader::AddObj(pNode);

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

//	virtual void *GetData(const char *strFileName) = 0;
//	virtual void *LoadData(const char *strFileName) = 0;
	

};
	}
}
#endif // !defined(AFX_BASECACHEMGR_H__C53D076B_14CF_47B5_B21C_F7EF3477C7EA__INCLUDED_)
