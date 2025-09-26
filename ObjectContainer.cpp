// ObjectContainer.cpp: implementation of the CObjectContainer class.
//
//////////////////////////////////////////////////////////////////////

#include "ObjectContainer.h"
#include "SceneManager.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CObjectContainer::CObjectContainer()
{

}

CObjectContainer::~CObjectContainer()
{
	DeleteAllObject();
}

CMeshObject* CObjectContainer::GetObject(char *strObjectName, bool isAlpha, bool isLight)
{
	int i=0;
	for(;i<(int)m_ObjectContainer.size();i++)
	{
		if( strcmp(m_ObjectContainer[i]->m_strObjectName,strObjectName)==0 )
			//&& m_ObjectContainer[i]->m_isAlpha == isAlpha &&m_ObjectContainer[i]->m_isLight == isLight)
		{
			m_ObjectContainer[i]->m_nUsed++;
			return m_ObjectContainer[i]->m_pObject;
		}
	}	
	Load(strObjectName,isAlpha,isLight);	
	return m_ObjectContainer[i]->m_pObject;	
}

void CObjectContainer::Load(char *strObjectName,bool isAlpha,bool isLight)
{
	CSceneManager::m_MeshObjectContainer.SetPath(OBJECTPATH);	
	CMeshObject *pObject=NULL;

	pObject=CSceneManager::m_MeshObjectContainer.GetMeshObject(strObjectName);
	pObject->ConvertNormal();
	ObjectNode *AddNode=new ObjectNode(pObject,strObjectName,isAlpha,isLight);
	m_ObjectContainer.push_back(AddNode);	
}

void CObjectContainer::DeleteObject(char *strObjectName, bool isAlpha, bool isLight)
{
	for(int i=0;i<(int)m_ObjectContainer.size();i++)
	{
		if( strcmp(m_ObjectContainer[i]->m_strObjectName,strObjectName)==0 &&
			m_ObjectContainer[i]->m_isAlpha==isAlpha &&
			m_ObjectContainer[i]->m_isLight==isLight)
		{
			if(--m_ObjectContainer[i]->m_nUsed==0)
			{				
				ObjectNode *delNode=m_ObjectContainer[i];								
				CSceneManager::m_MeshObjectContainer.DeleteMeshObject(delNode->m_strObjectName);
				delete delNode;
				m_ObjectContainer.erase(m_ObjectContainer.begin()+i);
			}
			return;
		}
	}	
}

void CObjectContainer::DeleteAllObject()
{
	for(int i=0;i<(int)m_ObjectContainer.size();i++)
	{
		ObjectNode *delNode=m_ObjectContainer[i];
		delete delNode;
	}
	m_ObjectContainer.clear();
}
