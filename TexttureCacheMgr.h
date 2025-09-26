#pragma once
#include <d3d8.h>
#include <vector>
#include <map>

class CTextureNode
{

public:
	char m_strName[256];
	char m_strPath[256];

	LPDIRECT3DBASETEXTURE8 m_lpTexture;
	unsigned int m_iReferenceCount;
	CTextureNode() : m_lpTexture(0),m_iReferenceCount(0)
	{
		memset(m_strName,0,sizeof(char) * 256);
		memset(m_strPath,0,sizeof(char) * 256);

	}
	CTextureNode(const char *strName,const char *strPath,LPDIRECT3DBASETEXTURE8 lpTexture)
	{
		m_lpTexture = lpTexture;
		strcpy(m_strName,strName);
		strcpy(m_strPath,strPath);

		m_iReferenceCount = 0;
		

	}
	~CTextureNode()
	{
		if(m_lpTexture)
		{
			m_lpTexture->Release();
			m_lpTexture = NULL;
		}
		m_iReferenceCount = 0;

	}
};

class CTextureCacheObj {
public:
	std::vector<CTextureNode *>	m_lstDatas;
	int							m_iDatasNum;
	CTextureCacheObj() : m_iDatasNum(0)
	{
		m_lstDatas.clear();
	}
	CTextureCacheObj(CTextureNode *pObj)  : m_iDatasNum(0)
	{
		m_lstDatas.clear();
		m_lstDatas.push_back(pObj);
		pObj->m_iReferenceCount++;
		m_iDatasNum++;
	}
	void Add_Back(CTextureNode *pObj)
	{
		m_lstDatas.push_back(pObj);
		pObj->m_iReferenceCount++;
		m_iDatasNum++;
	}
	void DeleteNotRef(int iNum)
	{
		if(m_iDatasNum <= iNum)
			return;
		if(m_lstDatas[iNum])
		{
		
			if(m_lstDatas[iNum])
			{
				delete m_lstDatas[iNum];
				m_lstDatas[iNum] = NULL;
			}
			m_lstDatas.erase(m_lstDatas.begin() + iNum);
			m_iDatasNum--;	
		}
	}
	void Remove(int iNum)
	{
		if(m_iDatasNum <= iNum)
			return;
		if(m_lstDatas[iNum])
		{
			if(!(--m_lstDatas[iNum]->m_iReferenceCount))
			{
				if(m_lstDatas[iNum])
				{
					delete m_lstDatas[iNum];
					m_lstDatas[iNum] = NULL;
				}
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
	~CTextureCacheObj() {
		if(m_iDatasNum > 0)
		{
			for(int i=0;i < m_iDatasNum; i++) 
			{
				if(m_lstDatas[i]) 
				{
					delete m_lstDatas[i];
					m_lstDatas[i] = NULL;
				}
			}
		}
		m_iDatasNum =  0;
		m_lstDatas.clear();
	}

		
			
};

typedef std::map<unsigned long , CTextureCacheObj *> TEXTURECACHETABLE;
typedef TEXTURECACHETABLE::value_type TEXTURECACHETABLEOBJ;
typedef TEXTURECACHETABLE::iterator TEXTURECACHEITER;

class CTextureCacheMgr  
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

	TEXTURECACHETABLE m_CacheTable;
	int	m_iCachesNum;


public:
	
	CTextureCacheMgr(): m_iCachesNum(0)
	{
		m_CacheTable.clear();
	}
	virtual ~CTextureCacheMgr()
	{
		for(TEXTURECACHEITER Itr = m_CacheTable.begin(); Itr != m_CacheTable.end();) 
		{
			if(Itr->second != NULL) 
			{
				if(Itr->second)
				{
					delete Itr->second;
					Itr->second = NULL;
				}

			}
			
			Itr++;
		}
		m_CacheTable.clear();
		m_iCachesNum = 0;


		
	}
	void DeleteAllData()
	{
		for(TEXTURECACHEITER Itr = m_CacheTable.begin(); Itr != m_CacheTable.end();) 
		{
			if(Itr->second != NULL) 
			{
				delete Itr->second;
				Itr->second = NULL;

			}
			
			Itr++;
		}
		m_CacheTable.clear();
		m_iCachesNum = 0;

	}
	void DeleteAllDataPath(const char *strPath)
	{
	
			for(TEXTURECACHEITER Itr = m_CacheTable.begin(); Itr != m_CacheTable.end();) 
			{
				if(Itr->second != NULL) 
				{
					for(int iNum = (*Itr).second->m_iDatasNum - 1; iNum >= 0; iNum--)
					{
						if(!strcmp((*Itr).second->m_lstDatas[iNum]->m_strPath,strPath))
						{
							((*Itr).second)->DeleteNotRef(iNum);	
						}
					}
				}
				Itr++;
			}
			
	
	}
	void DeleteData(const char *strName)
	{
	/*	*/
		if(m_CacheTable.size())
		{
			unsigned long DataID = GetHashID(strName);
				
			TEXTURECACHEITER itr  = m_CacheTable.find(DataID);
		
			if(itr != m_CacheTable.end()) 
			{
				if((*itr).second != NULL) 
				{
					for(int iNum = 0; iNum < (*itr).second->m_iDatasNum; iNum++)
					{
						if(!strcmp((*itr).second->m_lstDatas[iNum]->m_strName,strName))
						{
							((*itr).second)->Remove(iNum);	
						}
					}

				}

			}
		}
	}
	void AddTexture(const char *strName,const char *strPath,LPDIRECT3DBASETEXTURE8 lpTexture)
	{
		unsigned long DataID = GetHashID(strName);
		TEXTURECACHEITER itr  = m_CacheTable.find(DataID);
		
		CTextureNode *pNode = new CTextureNode(strName,strPath,lpTexture);
		m_iCachesNum++;
		if(itr == m_CacheTable.end())
		{

			CTextureCacheObj *pObj = new CTextureCacheObj(pNode);
			m_CacheTable.insert(TEXTURECACHETABLEOBJ(DataID,pObj));
			
		}
		else 
		{// map 안에 똑같은 Hash Index 를 가지는 요소 존재
			
			for(int iNum = 0; iNum < (*itr).second->m_iDatasNum; iNum++)
			{
				if(!strcmp((*itr).second->m_lstDatas[iNum]->m_strName,strName))
				{			
					return;
				}
			}

			((*itr).second)->Add_Back(pNode);	
		}

	}
	LPDIRECT3DBASETEXTURE8 FindTexture(const char *strName)
	{

		unsigned long DataID = GetHashID(strName);
			
		TEXTURECACHEITER itr  = m_CacheTable.find(DataID);
		
		if(itr == m_CacheTable.end()) 
		{
			return NULL;		
		}
		else
		{
			if((*itr).second != NULL) 
			{
				for(int iNum = 0; iNum < (*itr).second->m_iDatasNum; iNum++)
				{
					if(!strcmp((*itr).second->m_lstDatas[iNum]->m_strName,strName))
					{			
						(*itr).second->AddRef(iNum);

						return ((*itr).second->m_lstDatas[iNum]->m_lpTexture);	
					}
				}
			}
			else
			{
				return NULL;
			}

		}
		return NULL;
	}


};


