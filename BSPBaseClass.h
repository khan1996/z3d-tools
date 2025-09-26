#ifndef BSPBASECLASS_H
#define BSPBASECLASS_H
#pragma once

#include "3DMath.h"
#include "Vertex.h"
#include "List.h"
#include "QShader.h"
class Plane
{
public:
	vector3 m_vecPointOnPlane;
	vector3 m_vecNormal;
};

class Poly
{
public:
	LVertex *m_pVertexList;
	vector3 m_vecNormal;
	WORD m_nVertex;
	WORD m_nIndices;
	WORD *m_pIndices;
	bool m_bBeenUsedAsSplitter;
	DWORD m_TextureIndex;

	Poly *m_pNext;
};

class Portal
{
public:
	LVertex *m_pVertexList;
	vector3 m_vecNormal;
	WORD m_nVertex;
	WORD m_nIndices;
	WORD *m_pIndices;
	BYTE m_nLeafs;
	long m_LeafOwnerArray[4];
	Portal *m_pPrev;
	Portal *m_pNext;
};

class BoundingBox
{
public:
	vector3 m_vecBoxMin;
	vector3 m_vecBoxMax;
};

class Leaf
{
public:
	long m_StartPolygon;
	long m_EndPolygon;
	long m_PortalIndexList[50];
	long m_nPortals;
	long m_PVSIndex;
	BoundingBox m_BoundingBox;
};

class Node
{
public:
	unsigned char m_bLeaf;
	unsigned long m_Plane;
	unsigned long m_Front;
	signed long m_Back;
	BoundingBox m_BoundingBox;
};

class ClipPlane
{
public:
	WORD m_nPlane;
	Plane *m_pPlane;
};

const int CP_FRONT=1001;
const int CP_BACK =1002;
const int CP_ONPLANE=1003;
const int CP_SPANNING=1004;


class BSPBaseData
{
public:
	void Load(char *strFilename);
	void Save(char *strFilename,List<CQShader*> &ShaderList);
	void SplitPortal(Portal *pPortal, Plane *pPlane, Portal *pFrontSplit, Portal *pBackSplit);
	void CopyBspData(BSPBaseData *pData);
	static Poly* AddPolygon(Poly* pParent,LVertex  *Vertices,WORD nVertex);
	static Poly* LoadMWM(char *strFilename);
	void DeletePortal(Portal *pPortal);
	void IncreaseNumberOfPortals();
	void IncreaseNumberOfLeafs();
	void IncreaseNumberOfPlanes();
	void IncreaseNumberOfPolygons();
	void IncreaseNumberOfNodes();
	void DeletePolygon(Poly *pPoly);
	void CalculateBox(BoundingBox *pBox,Poly *pPolyList);
	bool GetIntersect (vector3 *vecLineStart,vector3 *vecLineEnd,vector3 *pVertex,vector3 *vecNormal,vector3 *vecIntersection,float *fPercentage);
	ClassifyPoint(vector3 *vecPoint,Plane *pPlane);
	void SplitPolygon(Poly *pPoly, Plane *pPlane, Poly *pFrontSplit, Poly *pBackSplit);
	static int ClassifyPoly(Plane *pPlane,Poly *pPoly);
	static void Create();
	BSPBaseData();	
	static Node *m_pNodeArray;
	static Poly *m_pPolygonArray;
	static Leaf *m_pLeafArray;
	static Plane *m_pPlaneArray;
	static Portal **m_pPortalArray;
	static BYTE *m_pPVSData;
	static long m_nPolygons;
	static long m_nNodes;
	static long m_nLeafs;
	static long m_nPlanes;
	static long m_nPortals;

	static long m_nMaxNodes;
	static long m_nMaxPolygons;	
	static long m_nMaxPlanes;
	static long m_nMaxLeafs;
	static long m_nMaxPortals;

	//static BYTE *m_pPVSData;
	static long m_BytePerSet;
	static long m_PVSCompressedSize;
};

#endif