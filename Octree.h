// Octree.h: interface for the COctree class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCTREE_H__E2D2D94E_00CE_4D60_B3C2_56C46A1EF03D__INCLUDED_)
#define AFX_OCTREE_H__E2D2D94E_00CE_4D60_B3C2_56C46A1EF03D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Vertex.h>
#include <3DMath.h>
#include <vector>
#include "ViewCamera.h"
//#include "SceneManager.h"
#include "../zalla3d base class/Z3DByteDataObj.h"

class PolyNode {
public:
	vector3 m_vecPoly[3];
	int m_used;
	PolyNode(vector3 *vecPoly)
	{
		m_vecPoly[0]=vecPoly[0];
		m_vecPoly[1]=vecPoly[1];
		m_vecPoly[2]=vecPoly[2];
		m_used=0;
	};
	PolyNode()
	{};
	~PolyNode()
	{};
	void Set(vector3 *vecPoly)
	{
		m_vecPoly[0]=vecPoly[0];
		m_vecPoly[1]=vecPoly[1];
		m_vecPoly[2]=vecPoly[2];
		m_used=0;
	};
};

class COctree
{		
	vector3 m_MinBox,m_MaxBox;
public:	
	
	std::vector<long> m_PolyNodeID;
	COctree *m_pChild[8];

	void CullSphere(vector3 vecCenter,float fRad,std::vector<PolyNode *> &PolyList,std::vector<int> &PolyID);
	void CullFrustum(CViewCamera *pCamera,std::vector<PolyNode *> &PolyList,std::vector<int> &PolyID);
	void CullRay(vector3 vecStart,vector3 vecDir,float fLens,std::vector<PolyNode *> &PolyList,std::vector<int> &PolyID);	
	
	void CalcBoundBox(std::vector<PolyNode *> &PolyList);
	void SplitNode(int DepthNode,std::vector<PolyNode *> &PolyList);
	
	int	IntersectionRay(const D3DXVECTOR3 &vecRayOrigin,const D3DXVECTOR3 &vecRayDir,float &fNear,float &fFar);

	void Render(LPDIRECT3DDEVICE8 pd3dDevice);

	void Save(FILE *fp);
	void Load(FILE *fp);
	void Load(CROSSM::CZ3DByteDataObj *pDataObj);

	COctree();
	virtual ~COctree();
};

#endif // !defined(AFX_OCTREE_H__E2D2D94E_00CE_4D60_B3C2_56C46A1EF03D__INCLUDED_)
