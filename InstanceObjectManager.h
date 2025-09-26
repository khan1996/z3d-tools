// InstanceObjectManager.h: interface for the CInstanceObjectManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INSTANCEOBJECTMANAGER_H__6B3FA12E_A24E_49BC_8887_79BD0D6F0C70__INCLUDED_)
#define AFX_INSTANCEOBJECTMANAGER_H__6B3FA12E_A24E_49BC_8887_79BD0D6F0C70__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "3DMath.h"
#include <vector>
#include "MeshObject.h"

class CInstanceObjectManager  
{
	class InstanceObjectNode
	{
	public:
		CMeshObject *m_pMeshObject;
		vector3 m_vecPosition;
		POINT m_ptMousePick;
		char m_strMeshName[256];
		int m_ID;
		color m_Color;
		float m_fAngle;
	};
	DWORD m_ObjectRenderStateBlock;
public:
	void SetAllObjectColor(color Color);
	void SetInstanceObjectColor(int ID,color Color);
	int MouseOnCheckInstanceObject(int px,int py);
	void Create(LPDIRECT3DDEVICE8 pd3dDevice);
	void Render(LPDIRECT3DDEVICE8 pd3dDevice);
	long m_TotalInstanceObjectID;
	std::vector<InstanceObjectNode> m_InstanceObjectList;
	void DeleteInstanceObject(int InstanceObjectID);
	int AddInstanceObject(char *strObjectName,vector3 vecPos, float fAngle = 0.0f);

	///// Minotaurs Ãß°¡
	void DeleteAllObject();
	void InstanceObjectRotation(int InstanceObjectID,float fDist = 0.0f);

	CInstanceObjectManager();
	virtual ~CInstanceObjectManager();

};

#endif // !defined(AFX_INSTANCEOBJECTMANAGER_H__6B3FA12E_A24E_49BC_8887_79BD0D6F0C70__INCLUDED_)
