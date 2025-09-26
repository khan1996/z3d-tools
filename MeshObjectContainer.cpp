// MeshObjectContainer.cpp: implementation of the CMeshObjectContainer class.
//
//////////////////////////////////////////////////////////////////////

#include "MeshObjectContainer.h"
#include "GMMemory.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//List<CMeshObjectContainer::MeshObjectNode*> CMeshObjectContainer::m_MeshContainer;

CMeshObjectContainer::CMeshObjectContainer()
{

}

CMeshObjectContainer::~CMeshObjectContainer()
{
	DeleteAllObject();
}

void CMeshObjectContainer::Load(char *strMeshName)
{
	CMeshObject *pMeshObject=new CMeshObject();
	char strPathAndFilename[MAX_NAMEBUFFER];
	strcpy(strPathAndFilename,m_strObjectPathName);
	strcat(strPathAndFilename,strMeshName);
	CTexture::SetPath(OBJECTTEXTUREPATH);
	pMeshObject->Load(strPathAndFilename);	
	pMeshObject->TextureLoad();
	if(pMeshObject->m_bCompleteMesh==false)
	{
		pMeshObject->m_dwShader=MultiFVF;
		pMeshObject->CalcBox();
	}
	MeshObjectNode *AddNode=new MeshObjectNode(pMeshObject,strMeshName);
	m_MeshContainer.push_back(AddNode);
}

CMeshObject* CMeshObjectContainer::GetMeshObject(char *strMeshName)
{
	int i=0;
	for(;i<(int)m_MeshContainer.size();i++)
	{
		if(strcmp(m_MeshContainer[i]->m_strMeshName,strMeshName)==0)
		{
			m_MeshContainer[i]->m_nUsed++;
			return m_MeshContainer[i]->m_MeshObject;
		}
	}
	Load(strMeshName);
	return m_MeshContainer[i]->m_MeshObject;
}

void CMeshObjectContainer::DeleteMeshObject(char *strMeshObject)
{
	if(strcmp(strMeshObject,"")==0)
		return;
	for(int i=0;i<(int)m_MeshContainer.size();i++)
	{
		if(strcmp(m_MeshContainer[i]->m_strMeshName,strMeshObject)==0)
		{			
			if(--m_MeshContainer[i]->m_nUsed==0)
			{				
				//MessageBox(NULL,strMeshObject,0,0);
				MeshObjectNode *delNode=m_MeshContainer[i];
				m_MeshContainer.erase(m_MeshContainer.begin()+i);
				delete delNode;
			}
			return;
		}
	}
}

void CMeshObjectContainer::DeleteAllObject()
{
	for(int i=0;i<(int)m_MeshContainer.size();i++)
	{
		MeshObjectNode *delNode=m_MeshContainer[i];		
		delete delNode;		
	}
	m_MeshContainer.clear();
}
