#pragma once

#include "3DMath.h"
#include "Vertex.h"

/*
*/

#define	CONTENTS_DETAIL			0x8000000	

#define	MAX_SURFACE_VERTS		256
#define MAX_MAP_SHADER			0xff
#define	MAX_MAP_MODELS			0x400
#define	MAX_MAP_DRAW_SURFS		0x20000
#define	MAX_MAP_DRAW_VERTS		0x80000
#define	MAX_MAP_DRAW_INDEXES	0x80000
#define	MAX_MAP_NODES			0x20000
#define	MAX_MAP_LEAFS			0x20000
#define	MAX_MAP_LEAFFACES		0x20000
#define	MAX_MAP_LEAFBRUSHES		0x40000
#define	MAX_MAP_BRUSHSIDES		0x20000
#define	MAX_MAP_BRUSHES			0x8000
#define	MAX_MAP_VISIBILITY	0x100000

#define MAX_STARTPOSITION 100
#define MAX_LIGHT 0xff

#define	MAX_PATCH_SIZE	32


#define	LUMP_ENTITIES		0
#define	LUMP_SHADERS		1
#define	LUMP_PLANES			2
#define	LUMP_NODES			3
#define	LUMP_LEAFS			4
#define	LUMP_LEAFSURFACES	5
#define	LUMP_LEAFBRUSHES	6
#define	LUMP_MODELS			7
#define	LUMP_BRUSHES		8
#define	LUMP_BRUSHSIDES		9
#define	LUMP_DRAWVERTS		10
#define	LUMP_DRAWINDEXES	11
#define	LUMP_FOGS			12
#define	LUMP_SURFACES		13
#define	LUMP_LIGHT			14
#define	LUMP_LIGHTGRID		15
#define	LUMP_VISIBILITY		16
#define	HEADER_LUMPS		17

#define	MAX_EXPANDED_AXIS		128
#define	SUBDIVISION_LIMIT		8.0

#define	PLANE_X			0
#define	PLANE_Y			1
#define	PLANE_Z			2
#define	PLANE_NON_AXIAL	3

#define	ON_EPSILON	0.01

class Lump
{
public:
	int m_Fileofs;
	int m_Filelen;
};

class DHeader
{
public:
	int m_Ident;
	int m_Version;
	Lump m_Lump[HEADER_LUMPS];
};

#define	PLANE_HASHES	1024
#define	NORMAL_EPSILON	0.0001
#define	DIST_EPSILON	0.02

class SPlane {
public:
	vector3 m_vecNormal;
	float m_fDist;
	int m_Type;
	SPlane *m_pHash_Chain;
};

#define MAX_POINTS_ON_WINDING	64
#define	MAX_POINTS_ON_FIXED_WINDING	12

class SWinding {
public:
	int m_nPoints;
	vector3 m_vecPoints[MAX_POINTS_ON_FIXED_WINDING];
	//vector3 m_vecPoints[4];
};

class SBspFace {
public:
	SBspFace *m_pNext;
	int m_nPlane;
	int m_Priority;
	bool m_bChecked;
	bool m_bHint;
	SWinding *m_pWinding;
};


class SSide {
public:
	int m_nPlane;
	float m_fTexMat[2][2];// Brush Primitive Texture matrix;
	float m_fvecs[2][4];// Texture Coordinate mapping

	SWinding *m_pWinding;
	SWinding *m_pVisibleHull;

	int m_nShader;

	//struct shaderInfo_s	*shaderInfo;

	int m_Contents;
	int m_SurfaceFlags;
	int m_Value;

	bool m_bVisible;
	bool m_bBevel;
	bool m_bBackSide;
};

#define	MAX_BRUSH_SIDES		1024

class SBspBrush {
public:
	SBspBrush *m_pNext;
	int m_nEntity;
	int m_nBrush;

	int m_nShader;
	//struct shaderInfo_s	*contentShader;

	int m_Contents;
	bool m_bDetail;
	bool m_bOpaque;
	int m_nOutPut;
	int m_PortalAreas[2];

	SBspBrush *m_pOriginal;

	vector3 m_vecMin,m_vecMax;
	int m_nSides;

	SSide m_Sides[6];
};

class SNode;

class SDrawSurRef
{
public:
	SDrawSurRef *m_pNextRef;
	int m_OutputNumber;
};

class SPortal {
public:
	SPlane m_Plane;
	SNode *m_pOnNode;
	SNode *m_pNodes[2];
	SPortal *m_pNext[2];
	SWinding *m_pWinding;

	bool m_bSideFound;
	bool m_bHint;
	SSide *m_pSide;
};

class SNode {
public:
	int m_nPlane;
	SNode *m_pParent;
	vector3 m_vecMin,m_vecMax;
	SBspBrush *m_pVolume;

	SSide *m_Side;
	SNode *m_pChildren[2];
	bool m_bHint;
	int m_nTinyPortals;
	vector3 m_vecReferencePoint;

	bool m_bOpaque;
	bool m_bAreaPortal;
	int m_Cluster;
	int m_Area;
	SBspBrush *m_pBrushList;
	SDrawSurRef *m_pDrawSurfReferences;	
	int m_Occupied;	
	SPortal *m_pPortal;	
};

class STree {
public:
	SNode *m_pHeadNode;
	SNode m_OutSideNode;
	vector3 m_vecMin,m_vecMax;
};

class SDrawSurf 
{
public:
	int m_nShader;
	SBspBrush *m_MapBrush;
	SSide *m_Side;
	SDrawSurf *m_pNextOnShader;

	int m_nFog;
	int m_nLightmap;
	int m_nLightmapX,m_nLightmapY;
	int m_LightmapWidth,m_LightmapHeight;

	int m_nVerts;
	BspVertex *m_pVerts;

	int m_nIndex;
	int *m_pIndex;

	int m_nPlane;

	vector3 m_vecLightmapOrigin;
	vector3 m_vecLightmapVecs[3];

	bool m_bPatch;
	int m_PatchWidth;
	int m_PatchHeight;

	bool m_bMiscModel;
	bool m_bFlareSurface;
};

class SEdgePoint
{
public:
	float m_fIntercept;
	vector3 m_vecXYZ;
	SEdgePoint *m_pPrev,*m_pNext;
};

class SEdgeLine
{
public:
	vector3 m_vecNormal1;
	float m_fDist1;
	vector3 m_vecNormal2;
	float m_fDist2;

	vector3 m_vecOrigin;
	vector3 m_vecDir;

	SEdgePoint m_Chain;
};

class SOriginalEdge {
public:
	float m_fLength;
	BspVertex *m_dv[2];
};

class SMesh 
{
public:
	int m_nWidth,m_nHeight;
	BspVertex *m_pVerts;
};

class SParseMesh
{
public:
	SParseMesh *m_pNext;
	SMesh m_Mesh;

	int m_nShader;

	bool m_bGrouped;
	SParseMesh *m_pGrupChain;
};

class DSurface 
{
public:
	int m_nShader;
	int m_nFog;
	int m_SurfaceType;

	int m_FirstVert;
	int m_nVerts;

	int m_FirstIndex;
	int m_nIndex;

	int m_nLightmap;
	int m_LightmapX,m_LightmapY;
	int m_LightmapWidth,m_LightmapHeight;

	vector3 m_vecLightmapOrigin;
	vector3 m_vecLightmap[3];

	int m_PatchWidth;
	int m_PatchHeight;
};

class DPlane
{
public:
	vector3 m_vecNormal;
	float m_fDist;
};

class DModel
{
public:
	vector3 m_vecMin,m_vecMax;
	int m_nFirstSurface,m_nSurfaces;
	int m_nFirstBrush,m_nBrushes;
};

class DNode
{
public:
	int m_nPlane;
	int m_Children[2];
	vector3 m_min;
	vector3 m_max;
};

class DLeaf
{
public:
	int m_Cluster;
	int m_Area;

	vector3 m_min;
	vector3 m_max;

	int m_FirstLeafSurface;
	int m_nLeafSurfaces;
	int m_FirstLeafBrush;
	int m_nLeafBrushes;
};

class DBrush 
{
public:
	int m_FirstSide;
	int m_nSides;
	int m_nShader;
};

class DBrushSide
{
public:
	int m_nPlane;
	int m_nShader;
};

class DVisiblity
{
public:
	int nClusters;
	int RowSize;
	unsigned char data[1];
};	



class SPassage
{
public:
	SPassage *m_pNext;
	BYTE m_Cansee[1];
};

typedef enum 
{
	stat_none,
	stat_working,
	stat_done
}Vstatus; 

class VPortal
{
public:
	int m_Num;
	bool m_bHint;
	bool m_bRemoved;
	SPlane m_Plane;
	int m_Leaf;

	vector3 m_vecOrigin;
	float m_fRadius;

	SWinding *m_pWinding;
	Vstatus m_Status;
	BYTE *m_pPortalFront;
	BYTE *m_pPortalFlood;
	BYTE *m_pPortalVis;

	int m_nMightsee;
	SPassage *m_Passage;
};

#define	MAX_PORTALS_ON_LEAF		128

class Leaf
{
public:
	int m_nPortals;
	int m_Merged;
	VPortal *m_pPortals[MAX_PORTALS_ON_LEAF];
};

#define	MAX_MAP_PORTALS		65536
#define MAX_SEPERATORS		64
#define	MAX_PORTALS	32768

class PStack
{
public:
	BYTE m_Mightsee[MAX_PORTALS/8];
	PStack *m_pNext;
	Leaf *m_pLeaf;
	VPortal *m_pPortal;
	SWinding *m_pSource;
	SWinding *m_pPass;

	SWinding m_Winding[3];
	int m_FreeWindings[3];
	
	SPlane m_PortalPlane;
	int m_Depth;	

	SPlane m_Seperators[2][MAX_SEPERATORS];
	int m_nSeperators[2];
};

class ThreadData
{
public:
	VPortal *m_pBase;
	int m_cChains;
	PStack m_PStackHead;
};

class DShader
{
public:
	char m_strTextureName[256];
};

class DLight
{
public:
	vector3 m_vecLightPos;
	color m_LightColor;
	float m_fLightRange;
	int m_Photons;
};

class TraceT
{
public:
	vector3 m_vecFilter;
	vector3 m_vecHit;
	float m_fHitFraction;
	bool m_bPassSolid;
};

class TraceWorkT
{
public:
	vector3 m_vecStart,m_vecEnd;
	int m_nOpenLeafs;
	int m_OpenLeafNumbers[MAX_MAP_LEAFS];
	TraceT  *m_Trace;
	int m_PatchShadows;
};


#define	TRACE_ON_EPSILON	0.1

class TNode
{
public:
	int m_Type;
	vector3 m_vecNormal;
	float m_fDist;
	int m_Children[2];
	int m_nPlaneNum;
};

#define	MAX_TNODES	(MAX_MAP_NODES*4)
#define	TRACE_ON_EPSILON	0.1

class CFaceT
{
public:
	float m_fSurface[4];
	int m_nBoundaries;
	float m_fBoundaries[4][4];

	vector3 m_vecPoint[4];
	float m_fTextureMatrix[2][4];
};

class SurfaceTestT
{
public:
	vector3 m_vecMins,m_vecMaxs;
	vector3 m_vecOrigins;
	float m_fRadius;

	bool m_bPatch;
	int m_nFacets;
	CFaceT *m_Facets;

	int m_nShader;
};