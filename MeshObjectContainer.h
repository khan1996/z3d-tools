// MeshObjectContainer.h: interface for the CMeshObjectContainer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHOBJECTCONTAINER_H__6A180B72_E538_4180_A520_B01C7B7844EC__INCLUDED_)
#define AFX_MESHOBJECTCONTAINER_H__6A180B72_E538_4180_A520_B01C7B7844EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseDataDefine.h"
#include "MeshObject.h"
#include <vector>


class CMeshObjectContainer  
{
	char m_strObjectPathName[MAX_NAMEBUFFER];
	class MeshObjectNode 
	{
	public:
		long m_nUsed;
		char m_strMeshName[MAX_NAMEBUFFER];
		CMeshObject *m_MeshObject;		
		MeshObjectNode(CMeshObject *pMeshObject,char *strMeshName)
		{
			strcpy(m_strMeshName,strMeshName);
			m_MeshObject=pMeshObject;
			m_nUsed=1;
		};
		~MeshObjectNode()
		{
			delete m_MeshObject;
		};
	};
	std::vector<MeshObjectNode*> m_MeshContainer;
public:
	void SetPath(char *strPath)
	{	
		strcpy(m_strObjectPathName,strPath);
	}
	void DeleteAllObject();
	void DeleteMeshObject(char *strMeshName);
	CMeshObject* GetMeshObject(char *strMeshName);
	void Load(char *strMeshName);
	CMeshObjectContainer();
	virtual ~CMeshObjectContainer();

};

#endif // !defined(AFX_MESHOBJECTCONTAINER_H__6A180B72_E538_4180_A520_B01C7B7844EC__INCLUDED_)
