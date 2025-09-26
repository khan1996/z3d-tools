#include "3DMath.h"
#include "Vertex.h"

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
#define	LUMP_LIGHTMAPS		14
#define	LUMP_LIGHTGRID		15
#define	LUMP_VISIBILITY		16
#define	HEADER_LUMPS		17

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

typedef int BBoxT[6];
typedef float BBoxFT[6];
typedef float TexCoordT[2];
typedef float vec4T[4];

typedef struct 
{
	int Plane;
	int Children[2];
	int Min[3];
	int Max[3];	
}NodeT;

typedef struct
{
	vector3 vec;
	float fOffset;
}PlaneT;

typedef struct
{
	int Cluster;
	int Area;
	int Min[3];
	int Max[3];
	int FirstFace,nFaces;
	int FirstUnknown,nUnknowns;
}LeafT;

typedef struct
{
	int Shader;
	int Unknown1[1];
	int FaceType;
	int FirstVert,nVerts;
	int FirstElem,nElems;
	// Lightmap Info//
	int LM_TextureNum;
	int LM_Offset[2];
	int LM_Size[2];
	vector3 vecOrig;
	BBoxFT BBox;
	vector3 vecNormal;
	int MeshCP[2];
}SurfaceT;

typedef struct
{
	int Shader;
	int Unknown1[1];
	int FaceType;
	int FirstVert,nVerts;
	int FirstElem,nElems;
	// Lightmap Info//
	int LM_TextureNum;
	int LM_Offset[2];
	int LM_Size[2];
	vector3 vecOrig;
	BBoxFT BBox;
	vector3 vecNormal;
	int MeshCP[2];
	MultiVertex *pVertex;	

	LPDIRECT3DVERTEXBUFFER8 m_pVertexBuffer;
	LPDIRECT3DINDEXBUFFER8 m_pIndicesBuffer;
	WORD *pIndices;	
}DrawSurface;

typedef struct
{
	int nClusters;
	int RowSize;
	BYTE data[1];
}VisiblityT;	

typedef struct
{
	int *nSurfaces[100];
	int **pDrawSurfaces[100];
}DrawSurfaceListT;

typedef struct
{
	char m_strTexture[256];
}DShader;