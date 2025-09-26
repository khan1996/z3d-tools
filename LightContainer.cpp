// LightContainer.cpp: implementation of the CLightContainer class.
//
//////////////////////////////////////////////////////////////////////

#include "LightContainer.h"
#include "SceneManager.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLightContainer::CLightContainer()
{

}

CLightContainer::~CLightContainer()
{

}

CLightObject * CLightContainer::GetLightObject(char *strLightObjectName,float fLightRange,color LightColor)
{
	int i=0;
	for(;i<(int)m_LightContainer.size();i++)
	{
		if( strcmp(m_LightContainer[i]->m_strLightName,strLightObjectName)==0 &&
			m_LightContainer[i]->m_pLightObject->m_fLightRange==fLightRange &&
			m_LightContainer[i]->m_pLightObject->m_LightColor==LightColor)
		{
			m_LightContainer[i]->m_nUsed++;
			return m_LightContainer[i]->m_pLightObject;
		}
	}		
	Load(strLightObjectName,fLightRange,LightColor);
	return m_LightContainer[i]->m_pLightObject;	
}

void CLightContainer::Load(char *strLightObjectName,float fLightRange, color LightColor)
{
	CLightObject *pLightObject=new CLightObject();	
	CSceneManager::m_MeshObjectContainer.SetPath(LIGHTOBJECTPATH);	
	pLightObject->m_pLightObject=CSceneManager::m_MeshObjectContainer.GetMeshObject(strLightObjectName);
	pLightObject->m_fLightRange=fLightRange;
	pLightObject->m_LightColor=LightColor;
	strcpy(pLightObject->m_strLightObjectName,strLightObjectName);
	LightObjectNode *AddNode=new LightObjectNode(pLightObject,strLightObjectName);
	m_LightContainer.push_back(AddNode);	
}

void CLightContainer::DeleteLightObject(char *strLightObjectName, float fLightRange, color LightColor)
{
	for(int i=0;i<(int)m_LightContainer.size();i++)
	{
		if( strcmp(m_LightContainer[i]->m_strLightName,strLightObjectName)==0 &&
			m_LightContainer[i]->m_pLightObject->m_fLightRange==fLightRange &&
			m_LightContainer[i]->m_pLightObject->m_LightColor==LightColor)
		{
			if(--m_LightContainer[i]->m_nUsed==0)
			{
				LightObjectNode *delNode=m_LightContainer[i];				
				if(strcmp(delNode->m_strLightName,"hstone05.R3S")==0)
				{
					char debug[256];
					sprintf(debug,"File=%s,Line=%s",__FILE__,__LINE__);
					MessageBox(NULL,debug,0,0);
				}

				CSceneManager::m_MeshObjectContainer.DeleteMeshObject(delNode->m_strLightName);
				delNode->m_pLightObject->m_pLightObject=NULL;

				if(delNode->m_pLightObject) { 
					delete delNode->m_pLightObject;
					delNode->m_pLightObject = NULL;
				}
				if(delNode) {
					delete delNode;
					delNode = NULL;
				}
				
				m_LightContainer.erase(m_LightContainer.begin()+i);
			}
			return;
		}
	}	
}

void CLightContainer::DeleteAllObject()
{
	for(int i=0;i<(int)m_LightContainer.size();i++)
	{
		LightObjectNode *delNode=m_LightContainer[i];
		if(delNode) {
			delete delNode;
			delNode = NULL;
		}
	}
	m_LightContainer.clear();
}
