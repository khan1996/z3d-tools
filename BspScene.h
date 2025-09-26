// BspScene.h: interface for the CBspScene class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BSPSCENE_H__EFE28EE9_11AC_466C_B92C_4E7930C219D3__INCLUDED_)
#define AFX_BSPSCENE_H__EFE28EE9_11AC_466C_B92C_4E7930C219D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "BSPBaseDataDefine.h"
#include "BspDataDefine.h"
#include "Texture.h"
#include <vector>
#include "ViewCamera.h"
#include "MeshObject.h"

typedef float vector2[2];

class CBspScene  
{	
	class DSurfaceEx
	{
	public:
		DSurface *m_pSurface;
		vector3 m_vecMax,m_vecMin;
		std::vector<int> m_ApplyLightList;
		LPDIRECT3DVERTEXBUFFER8 m_pShadowVertexBuffer;
		int m_nShadowVertex;
		std::vector<int> m_ShadowIndicesList;
	};
	class DLightEx
	{
	public:
		DLight *m_pLight;
		std::vector<int> m_ApplySurface;
		LPDIRECT3DINDEXBUFFER8 m_pEdgeIndexBuffer;
		int m_nEdgePoly;
	};
	class CurveBuffer
	{
	public:
		void Render(LPDIRECT3DDEVICE8 pd3dDevice);
		void FillPatch3(int *numcp, int *size, vector3 *p);
		void FillPatch2(int *numcp, int *size, vector2 *p);
		void FillCurve3(int numcp, int size, int stride,vector3 *p);
		void FillCurve2(int numcp, int size, int stride, vector2 *p);
		void FillPatchVertex();
		int FindLevel(vector3 *v);
		void FindSize();
		void CreateCurveBuffer(DSurface *pSurface,BspVertex *pControlPointVertex);
		BspVertex *m_pVertex;
		WORD *m_pIndices;
		DSurface *m_pSurface;
		vector3 *m_pControlPoint;
		int m_PatchSizeWidth;
		int m_PatchSizeHeight;
		int m_nIndices;
		
	};
	class CBSPShader 
	{
	public:
		CTexture *m_DiffuseTexture;
		CTexture *m_BumpTexture;
		LPDIRECT3DVERTEXBUFFER8 m_pVertexBuffer;
		LPDIRECT3DINDEXBUFFER8 m_pIndicesBuffer;
		int m_nPoly;
		int m_nVertex;
		int m_nIndices;

		int m_nDrawIndices;
		WORD *m_pIndices;
		std::vector<int> m_ApplyLightList;
	};
	std::vector<DLightEx> m_LightEx;
	std::vector<CBSPShader> m_ShaderList;
	std::vector<DSurfaceEx*> m_DSurfaceEx;
	CViewCamera m_Camera;	

	// 현재 사용안함.
//	std::vector<CurveBuffer*> m_CurveBufferList;

	TNode *m_TNodes,*m_TNodes_p;
	SurfaceTestT *m_SurfaceTest[MAX_MAP_DRAW_SURFS];	
	
	LPDIRECT3DVERTEXBUFFER8 m_pVertexBuffer;
	LPDIRECT3DINDEXBUFFER8 m_pIndicesBuffer;
	int m_nLightmap;
	int m_nMaxIndices;	
	bool m_bCull;

	DWORD m_dwBSPAmbientVertexShader;
	DWORD m_dwBSPAmbientPixelShader;

	DWORD m_dwBSPDiffuseVertexShader;
	DWORD m_dwBSPDiffusePixelShader;

	DWORD m_dwBSPSpecularVertexShader;
	DWORD m_dwBSPSpecularPixelShader;

	DWORD m_dwBSPShadowVertexShader;

	LPDIRECT3DCUBETEXTURE8 m_pNormalizeCubeMap;
	LPDIRECT3DVOLUMETEXTURE8 m_pPointFalloffMap;

	std::vector<BspShadowVertex> m_SurfaceEdgeVertexList;
	LPDIRECT3DVERTEXBUFFER8 m_pEdgeVertexBuffer;


public:
	int FindShadowEdgeVertex(BspShadowVertex Vertex);
	void LerpDrawVert(BspVertex *a, BspVertex *b, BspVertex *out);
	SMesh* CopyMesh(SMesh *mesh);
	void ProjectPointOntoVector(vector3 &point, vector3 &vStart, vector3 &vEnd, vector3 &vProj);
	void FreeMesh(SMesh *m);
	void MakeMeshNormals(SMesh in);
	void PutMeshOnCurve(SMesh in);
	SMesh* SubdivideMesh(SMesh in, float maxError, float minLength);
	void AddPointToBounds(vector3 &v, vector3 &max, vector3 &min);
	int PlaneTypeForNormal(vector3 p);
	SMesh* RemoveLinearMeshColumnsRows(SMesh *in);
	void FacetsForPatch(DSurface *dsurf, int nShader, SurfaceTestT *test);
	bool CM_GenerateFacetFor4Points(CFaceT *f, BspVertex *a, BspVertex *b, BspVertex *c, BspVertex *d);
	void TextureMatrixFromPoints(CFaceT *f, BspVertex *a, BspVertex *b, BspVertex *c);
	void CM_GenerateBoundaryForPoints(float boundary[], float plane[], vector3 &a, vector3 &b);
	bool PlaneFromPoints(float *plane, vector3 &a, vector3 &b, vector3 &c);
	bool CM_GenerateFacetFor3Points(CFaceT *f, BspVertex *a, BspVertex *b, BspVertex *c);
	void FacetsForTriangleSurface(DSurface *dsurf, int nShader, SurfaceTestT *test);
	void InitSurfacesForTesting();
	void TraceAgainstFacet(TraceWorkT *tr,int nShader,CFaceT *facet);
	bool SphereCull(vector3 &start, vector3 &stop, vector3 &origin, float radius);
	void TraceAgainstSurface(TraceWorkT *tw, SurfaceTestT *surf);
	int RecurTraceLine(int node, vector3 &start, vector3 &stop, TraceWorkT *tw);
	void TraceLine(vector3 &start, vector3 &stop, TraceT *trace, bool testAll, TraceWorkT *tw);
	void MakeTnode(int nodenum);
	void InitTrace();
	bool RecurTraceLine(int nNode,vector3 &vecStart,vector3 &vecEnd);
	bool TraceLineCollision(vector3 &vecStart,vector3 vecEnd);
	void DetectEdge(int *pEdges,int &nEdges,int nEdge0,int nEdge1,DSurface *pSrcSurface,vector3 vecPlaneNormal,vector3 vecPlaneOrigin);
	void CalcShadowVolume(BspVertex *pVertex,int nVertex,int *pIndices,int nIndices,DSurfaceEx &Surface);
	void SurfaceLightArrange();
	void InitShader();
	void CullCollisionPoly(vector3 vecPos);
	void DeleteLight();
	void RenderLight(LPDIRECT3DDEVICE8 pd3dDevice);
	void UpdateLightmapVector3(int n);
	void AddLight(vector3 vecLightPos,float fRange,color clr);
	bool m_bLightmapVector;
	void LoadLightmapVector3();
	bool m_bSkyRender;
	void RenderBox(LPDIRECT3DDEVICE8 pd3dDevice,DWORD dwColor);
	void CullWalkLeaf(int n);
	void CullWalkNodes(int n);
	void CullPoly(vector3 vecMax,vector3 vecMin,std::vector<vector3> &PolyList);
	bool LeafInFrustum(vector3 &vecMax,vector3 &vecMin);
	void RenderBackend(LPDIRECT3DDEVICE8 pd3dDevice);
	void RenderInit();
	void RenderWalkFace(int n);
	void RenderWalkLeaf(int n);
	void RenderWalkNode(int n);
	int ClassfyPoint(vector3 p, int nPlane);
	int FindCluster(vector3 vecPos);
	void Render(LPDIRECT3DDEVICE8 pd3dDevice,vector3 vecViewPos);
	int CopyLump(DHeader *header,int lump,void **dest,int size);
	void Load(char *strFilename);
	CBspScene();
	virtual ~CBspScene();	

	vector3 m_vecMaxBox,m_vecMinBox;

	int *m_SurfaceInc;	

	int m_EyeCluster;//내 클러스터가 어디냐?//
	vector3 m_vecViewPos;

	DShader *m_Shaders;
	int m_nShader;

	BspVertex *m_DrawVerts;
	int m_nDrawVerts;

	DPlane *m_Planes;
	int m_nPlanes;

	DLeaf *m_Leafs;
	int m_nLeafs;

	DNode *m_Nodes;
	int m_nNodes;
	
	DSurface *m_DrawSurfaces;	
	int m_nDrawSurfaces;		
	
	int *m_LeafSurfaces;
	int m_nLeafSurfaces;
	
	int *m_DrawIndex;
	int m_nDrawIndex;

	DVisiblity *m_Visibility;
	int m_nVisibility;

	DLight *m_Light;
	int m_nLight;

	std::vector<DLight> m_EditLightList;

	vector3 m_vecCenter,m_vecMax,m_vecMin;

	CMeshObject *m_pLightObject;
	int m_SelectLight;

	vector3 *m_pSurfaceMaxBox;
	vector3 *m_pSurfaceMinBox;
};

#endif // !defined(AFX_BSPSCENE_H__EFE28EE9_11AC_466C_B92C_4E7930C219D3__INCLUDED_)
