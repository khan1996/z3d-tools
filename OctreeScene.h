// OctreeScene.h: interface for the COctreeScene class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCTREESCENE_H__87C08805_348B_4734_8F20_9353BDC89A05__INCLUDED_)
#define AFX_OCTREESCENE_H__87C08805_348B_4734_8F20_9353BDC89A05__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./Octree.h"
#include "../zalla3d base class/ResourceObj.h"

#include <vector>

//#include <src/base/LoadedObj.h>

class COctreeScene : public CROSSM::CResourceObj
{
public:

	void CullRay(vector3 vecStart,vector3 vecDir,float fLens,std::vector<PolyNode*> &CullPoly);
	void CullFrustum(CViewCamera *pCamera,std::vector<PolyNode*> &CullPoly);
	void CullSphere(vector3 vecCenter,float fRad,std::vector<PolyNode*> &CullPoly);
	void Load(FILE *fp);
	void Save(FILE *fp);
	void Render(LPDIRECT3DDEVICE8 pd3dDevice);
	void RenderOctreeTriangle(IDirect3DDevice8* pDevice);
	void GenerateOctree();
	void AddPoly(vector3 *vecPoly);
	COctreeScene();
	virtual ~COctreeScene();

	virtual bool Load();
	virtual bool PostLoad();
	virtual bool Unload();

	COctree *m_pOctree;

	// 충돌 일어날 때 true
	bool CollisionToRay(const D3DXVECTOR3 &vecStart,const D3DXVECTOR3 &vecDir,const float &fDist);
	bool CollisionTriangleToRay(long iFaceIndex,const D3DXVECTOR3 &vecStart,const D3DXVECTOR3 &vecDir,const float &fDist);

protected:
	void BuildRenderVertex();

	std::vector<PolyNode *> m_PolyList;
	std::vector<vector3> m_vecRenderVertex;
};


#endif // !defined(AFX_OCTREESCENE_H__87C08805_348B_4734_8F20_9353BDC89A05__INCLUDED_)

