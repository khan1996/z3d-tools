// ContainerASE.h: interface for the CContainerASE class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTAINERASE_H__9EBF9328_1E27_11D4_891B_0000E8EB4C69__INCLUDED_)
#define AFX_CONTAINERASE_H__9EBF9328_1E27_11D4_891B_0000E8EB4C69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <crtdbg.h>
#include "misc.h"

#include <vector>



// internal constants of ASE reader class
#define ASE_MAX_LINELENGTH	 255	// maximum line length in ASE file
#define ASE_MAX_TOKENLENGTH	  60	// maximum token length of the line in the ASE file
// should modify token reading part of ReadOneLine() func. when modify the value in the next line
#define ASE_MAX_TOKEN_IN_LINE 20	// maximum token number in a line of ASE file
#define ASE_MAX_DEPTH		  20	// maximum indentation depth
// Obesolete due to using STL
//#define ASE_MAX_OBJECTS		1000	// maxumum object number in ASE file
//#define ASE_MAX_MATERIALS	 100	// maxumum material number in ASE file
#define ASE_MAX_ANI_LENGTH 10000	// maximum key number in the ASE file

// ASE reader error type
typedef enum
{
	ASE_ERR_NO_ERROR = 0,
	ASE_ERR_FILE_NOT_FOUND,
	ASE_ERR_EOF,
	ASE_ERR_NOT_ENOUGH_MEMORY
} ASE_ERR_TYPE;


// ASE token type IDs
// Imported & modified from MAX R3 ascii exporter plug-in source
typedef enum
{
	//
	ASE_UNKNOWN = -1,

	// Top level category ID's
	ASE_SCENE = 0,
	ASE_GEOMETRY,
	ASE_SHAPE,
	ASE_CAMERA,
	ASE_LIGHT,
	ASE_HELPER,
	ASE_MATERIAL_LIST,

	// Hierarchy
	ASE_GROUP,
	ASE_NODE_TM,
	ASE_NODE_NAME,
	ASE_NODE_PARENT,

	// Object (node) properties
	ASE_PROP_MOTIONBLUR,
	ASE_PROP_CASTSHADOW,
	ASE_PROP_RECVSHADOW,

	// Mesh related ID's
	ASE_MESH,
	ASE_MESH_NORMALS,
	ASE_MESH_NUMVERTEX,
	ASE_MESH_NUMFACES,
	ASE_MESH_VERTEX_LIST,
	ASE_MESH_VERTEX,
	ASE_MESH_FACE_LIST,
	ASE_MESH_FACE,
	ASE_MESH_SMOOTHING,
	ASE_MESH_MTLID,

	ASE_MESH_NUMTVERTEX,
	ASE_MESH_NUMTVFACES,
	ASE_MESH_TVERTLIST,
	ASE_MESH_TVERT,
	ASE_MESH_TFACELIST,
	ASE_MESH_TFACE,

	ASE_MESH_NUMCVERTEX,
	ASE_MESH_NUMCVFACES,
	ASE_MESH_CVERTLIST,
	ASE_MESH_VERTCOL,
	ASE_MESH_CFACELIST,
	ASE_MESH_CFACE,
	ASE_MESH_MAPPINGCHANNEL,

	ASE_MESH_FACEMAPLIST,
	ASE_MESH_FACEMAP,
	ASE_MESH_FACEVERT,

	ASE_MESH_FACENORMAL,
	ASE_MESH_VERTEXNORMAL,
	ASE_MESH_ANIMATION,

	// Helper objects
	ASE_HELPER_CLASS,

	// Controller ID's
	ASE_CONTROL_POINT3_TCB,
	ASE_CONTROL_POINT3_BEZIER,
	ASE_CONTROL_COLOR_BEZIER,
	ASE_CONTROL_POINT3_SAMPLE,

	ASE_CONTROL_FLOAT_TCB,
	ASE_CONTROL_FLOAT_BEZIER,
	ASE_CONTROL_FLOAT_LINEAR,
	ASE_CONTROL_FLOAT_SAMPLE,

	// "Track" is the identification of a sampled controller
	ASE_POS_TRACK,
	ASE_ROT_TRACK,
	ASE_SCALE_TRACK,

	// Sampled keys
	ASE_POS_SAMPLE,
	ASE_ROT_SAMPLE,
	ASE_SCALE_SAMPLE,

	// Specific controller keys
	ASE_POS_KEY,
	ASE_ROT_KEY,
	ASE_SCALE_KEY,
	ASE_POINT3_KEY,
	ASE_FLOAT_KEY,

	// TCB Keys have Tens, cont, bias, easeIn, easeOut
	ASE_TCB_POINT3_KEY,
	ASE_TCB_FLOAT_KEY,
	ASE_TCB_POS_KEY,
	ASE_TCB_ROT_KEY,
	ASE_TCB_SCALE_KEY,

	// Bezier keys have inTan, outTan
	ASE_BEZIER_FLOAT_KEY,
	ASE_BEZIER_POINT3_KEY,
	ASE_BEZIER_POS_KEY,
	ASE_BEZIER_SCALE_KEY,

	ASE_CONTROL_POS_LINEAR,
	ASE_CONTROL_POS_TCB,
	ASE_CONTROL_POS_BEZIER,
	ASE_CONTROL_ROT_LINEAR,
	ASE_CONTROL_ROT_TCB,
	ASE_CONTROL_ROT_BEZIER,
	ASE_CONTROL_SCALE_LINEAR,
	ASE_CONTROL_SCALE_TCB,
	ASE_CONTROL_SCALE_BEZIER,

	// Material / Texture related ID's
	ASE_WIRECOLOR,
	ASE_MATERIAL,
	ASE_MATERIAL_COUNT,
	ASE_MATERIAL_REF,
	ASE_NUMSUBMTLS,
	ASE_SUBMATERIAL,
	ASE_MATNAME,
	ASE_MATCLASS,

	ASE_AMBIENT,
	ASE_DIFFUSE,
	ASE_SPECULAR,
	ASE_SHINE,
	ASE_SHINE_STRENGTH,
	ASE_TRANSPARENCY,
	ASE_WIRESIZE,
	
	ASE_SHADING,
	ASE_SELFILLUM,
	ASE_TWOSIDED,
	ASE_WIRE,
	ASE_FACEMAP,

	ASE_TEXNAME,
	ASE_TEXCLASS,
	ASE_TEXSUBNO,
	ASE_TEXAMOUNT,

	ASE_BITMAP,

	ASE_MAPTYPE,
	ASE_U_OFFSET,
	ASE_V_OFFSET,
	ASE_U_TILING,
	ASE_V_TILING,
	ASE_ANGLE,

	// Sub texture types
	ASE_MAP_DIFFUSE,
	ASE_MAP_OPACITY,
	ASE_MAP_BUMP,
	ASE_MAP_REFLECT,

	// TM related ID's
	ASE_TM_ROW0,
	ASE_TM_ROW1,
	ASE_TM_ROW2,
	ASE_TM_ROW3,
	ASE_TM_POS,
	ASE_TM_ROTAXIS,
	ASE_TM_ROTANGLE,
	ASE_TM_SCALE,
	ASE_TM_SCALEAXIS,
	ASE_TM_SCALEAXISANG,
	ASE_TM_ANIMATION,

	// TM Inheritance flags
	ASE_INHERIT_POS,
	ASE_INHERIT_ROT,
	ASE_INHERIT_SCL,

	// Scene related ID's
	ASE_FILENAME,
	ASE_FIRSTFRAME,
	ASE_LASTFRAME,
	ASE_FRAMESPEED,
	ASE_TICKSPERFRAME,
	ASE_ENVMAP,
	ASE_STATICBGCOLOR,
	ASE_ANIMBGCOLOR,
	ASE_STATICAMBIENT,
	ASE_ANIMAMBIENT,

	// Generic ID's that can show up here and there
	ASE_TIMEVALUE,
	ASE_COMMENT,
	ASE_FILEID,
	ASE_BOUNDINGBOX_MIN,
	ASE_BOUNDINGBOX_MAX,

	// Physique related ID's
	ASE_PHYSIQUE,
	ASE_PHYSIQUE_NUMVERTEX,
	ASE_PHYSIQUE_LIST,
	ASE_PHYSIQUE_TYPE_BLENDRIGID,
	ASE_PHYSIQUE_NUMNODE,
	ASE_PHYSIQUE_NODELIST,
	ASE_PHYSIQUE_NODE,
	ASE_PHYSIQUE_TYPE_NONBLENDRIGID,

	// User property
	ASE_SKEL_KEY_ID,
	ASE_BODY_PART_ID,
	ASE_PART_KEY_ID,

	// Braces
	ASE_LEFTBRACE,
	ASE_RIGHTBRACE
} ASE_TOKEN_TYPE;


// animation key type
typedef enum
{
	ASE_KEY_UNKNOWN,
	ASE_KEY_LINEAR,
	ASE_KEY_BEZIER,
	ASE_KEY_TCB,
	ASE_KEY_SAMPLE
} ASE_KEY_TYPE;


struct ASE_MATRIX4
{
	float _11, _12, _13, _14;
	float _21, _22, _23, _24;
	float _31, _32, _33, _34;
	float _41, _42, _43, _44;

	ASE_MATRIX4() { ZeroMemory( this, sizeof( ASE_MATRIX4 ) ); }
};

struct ASE_VECTOR3F
{
	float x, y, z;

	void Multiply( ASE_MATRIX4 m )
	{
		float fx = x, fy = y, fz = z;

		x = m._11 * fx + m._21 * fy +
			m._31 * fz + m._41;
		y = m._12 * fx + m._22 * fy +
			m._32 * fz + m._42;
		z = m._13 * fx + m._23 * fy +
			m._33 * fz + m._43;
	}

	void Normalize()
	{
		float n = sqrtf(x*x + y*y + z*z);
		if( n > FLOAT_EPSILON )
		{
			x /= n;
			y /= n;
			z /= n;
		}
	}
};

struct ASE_COLOR3F
{
	float r, g, b;
	// inline con/destructor
	ASE_COLOR3F() { r = g = b = 1.0f; }
	~ASE_COLOR3F() {}
};

struct ASE_UVCOORD
{
	float u, v;
};

struct ASE_QUAT
{
	float x, y, z, w;
	// inline con/destructor
	ASE_QUAT() { x = y = z = 0.0f; w = 1.0f; }
	~ASE_QUAT() {}
};

struct ASE_VECTORKEY
{
	long tick;
	float x, y, z;
};

struct ASE_QUATKEY
{
	long tick;
	float x, y, z, w;
};

// deleted at 00.7.26
//struct ASE_VERTEX
//{
//	ASE_VECTOR3F pos;
//	ASE_VECTOR3F normal; ASE의 야릇한 구조로 인해(텍스춰 버텍스의 이유와 비슷) 페이스 구조체로 옮겨감
//	ASE_COLOR3F color;
//};

struct ASE_FACE
{
	WORD awIndex[3];
	WORD awTIndex[3];
	WORD awCIndex[3];
	ASE_VECTOR3F avecIndexnormals[3];
	ASE_VECTOR3F vecFacenormal;
	WORD wSgroup_id;	// smoothing group id
	WORD wMaterial_id; // actually, this is material id for 'multi-sub' materialed object

	ASE_FACE() { wSgroup_id = -1; wMaterial_id = -1; }
	~ASE_FACE() {}
};

struct ASE_PHYS_NODE
{
	float weight;
	char name[ASE_MAX_TOKENLENGTH+1];
};

struct ASE_PHYS_DATA
{
	WORD nodecount;
	ASE_PHYS_NODE *pNode;

	ASE_PHYS_DATA() { nodecount = 0; pNode = NULL; }
	// copy constructor
	ASE_PHYS_DATA( const ASE_PHYS_DATA &s )
	{
		nodecount = s.nodecount;
		pNode = new ASE_PHYS_NODE[nodecount];
		for( int i = 0; i < nodecount; i++ )
			pNode[i] = s.pNode[i];
	}
	~ASE_PHYS_DATA() { SAFE_DELETEA( pNode ); }
	ASE_PHYS_DATA& operator=( const ASE_PHYS_DATA &s )
	{
		nodecount = s.nodecount;
		pNode = new ASE_PHYS_NODE[nodecount];
		for( int i = 0; i < nodecount; i++ )
			pNode[i] = s.pNode[i];

		return *this;
	}
	
};

struct ASE_MTRL
{
	long lId;
	long lParent_id;
	ASE_COLOR3F ambient;
	ASE_COLOR3F diffuse;
	ASE_COLOR3F specular;
	float fOpacity;
	float fSelfillum;
	char *szTexture;

	ASE_MTRL() { lId = -1; lParent_id = -1; fOpacity = 1.0f; fSelfillum = 0.0f; szTexture = NULL; }
	~ASE_MTRL() { SAFE_DELETE( szTexture ); }
};

struct ASE_OBJECT
{
	// identification & hierarchy
	char szName[ASE_MAX_TOKENLENGTH+1];
	char szParentName[ASE_MAX_TOKENLENGTH+1];

	bool bIsGeomObject;

	// mesh things (vertex, index, texture coord, .... )
	long lVertexCount;
	ASE_VECTOR3F *pVertices;

	long lTVertexCount;
	ASE_UVCOORD *pTVertices;

	long lCVertexCount;
	ASE_COLOR3F *pCVertices;

	long lFaceCount;
	ASE_FACE *pFaces;

	// Material
	ASE_COLOR3F color;
	long lMat_id;	// material reference id

	// TM
	ASE_MATRIX4 TM;

	ASE_VECTORKEY TM_PosKey;
	ASE_QUATKEY TM_RotKey;
	ASE_VECTORKEY TM_SclKey;

	// Animation key
	long lPosKeyCount;
	ASE_KEY_TYPE ePosKeyType;
	ASE_VECTORKEY **ppPosKey;

	long lRotKeyCount;
	ASE_KEY_TYPE eRotKeyType;
	ASE_QUATKEY **ppRotKey;

	long lSclKeyCount;
	ASE_KEY_TYPE eSclKeyType;
	ASE_VECTORKEY **ppSclKey;

	// Physique
	long lPhysDataCount; // same as vertex number when it's properly physique'd
	ASE_PHYS_DATA **ppPhysiqueData;

	long lSkelKeyId;
	long lBodyPartId;
	long lPartKeyId;

	ASE_OBJECT::ASE_OBJECT()
	{
		szName[0] = '\0';
		szParentName[0] = '\0';

		bIsGeomObject = false;

		lVertexCount = 0;
		pVertices = NULL;

		lTVertexCount = 0;
		pTVertices = NULL;

		lCVertexCount = 0;
		pCVertices = NULL;

		lFaceCount = 0;
		pFaces = NULL;

		lMat_id = -1;

		lPosKeyCount = 0;
		ePosKeyType = ASE_KEY_UNKNOWN;
		ppPosKey = NULL;

		lRotKeyCount = 0;
		eRotKeyType = ASE_KEY_UNKNOWN;
		ppRotKey = NULL;

		lSclKeyCount = 0;
		eSclKeyType = ASE_KEY_UNKNOWN;
		ppSclKey = NULL;

		lPhysDataCount = 0;
		ppPhysiqueData = NULL;

		lSkelKeyId = -1;
		lBodyPartId = -1;
		lPartKeyId = -1;
	}

	ASE_OBJECT::~ASE_OBJECT()
	{
		long i;

		SAFE_DELETEA( pVertices );
		SAFE_DELETEA( pFaces );
		SAFE_DELETEA( pTVertices );

		for( i = 0; i < lPosKeyCount; i++ )
			SAFE_DELETE( ppPosKey[i] );
		SAFE_DELETEA( ppPosKey );

		for( i = 0; i < lRotKeyCount; i++ )
			SAFE_DELETE( ppRotKey[i] );
		SAFE_DELETEA( ppRotKey );

		for( i = 0; i < lSclKeyCount; i++ )
			SAFE_DELETE( ppSclKey[i] );
		SAFE_DELETEA( ppSclKey );

		for( i = 0; i < lPhysDataCount; i++ )
			SAFE_DELETE( ppPhysiqueData[i] );
		SAFE_DELETEA( ppPhysiqueData );
	}
};

class CContainerASE  
{
public:
	long GetFirstFrame() { return m_lFirstFrame; }
	long GetLastFrame() { return m_lLastFrame; }
	long GetTicksPerFrame() { return m_lTicksPerFrame; }
	long GetObjectCount() { return m_vecpObjects.size(); }
	long GetMaterialCount() { return m_vecpMaterials.size(); }

	ASE_OBJECT* GetObject( int idx )
	{
		if( idx < (int)m_vecpObjects.size() )
		{
			return m_vecpObjects[idx];
		}
		else
		{
			return NULL;
		}
	}

	ASE_MTRL* GetMaterial( int idx )
	{
		if( idx < (int)m_vecpMaterials.size() )
		{
			return m_vecpMaterials[idx];
		}
		else
		{
			return NULL;
		}
	}

	ASE_ERR_TYPE ReadASE( const char *szFileName );

	CContainerASE();
	virtual ~CContainerASE();
	bool Reset();

protected:
	char m_szFileName[ASE_MAX_LINELENGTH+1];

	long m_lFirstFrame;
	long m_lLastFrame;
	long m_lTicksPerFrame;

	//long m_lObjectCount;
	std::vector<ASE_OBJECT*> m_vecpObjects;
	//long m_lMaterialCount;
	std::vector<ASE_MTRL*> m_vecpMaterials;

	long m_lSkelKeyId;	// user-prop value, yundi custom

	void ReadScene();
	void ReadTM();
	void ReadObject( bool geomflag );	// for reading Geom & helper object
	void ReadMesh();
	void ReadVertices();
	void ReadTextureVertices();
	void ReadColorVertices();
	void ReadFaces();
	void ReadTextureFaces();
	void ReadColorFaces();
	void ReadNormals();
	void ReadAni();
	void ReadPosKey();
	void ReadRotKey();
	void ReadSclKey();
	void ReadPhysique();
	void ReadPhysList();
	void ReadPhysVertex( ASE_PHYS_DATA *pData );
	void ReadPhysNode( ASE_PHYS_NODE *pNode );
	void ReadMaterial();
	void ReadMaterialItem();
	void ReadDiffuseMap();
	void ReadUserProp();

	static char *RipOutString();
	static ASE_TOKEN_TYPE Token2Id( const char *szToken );
	static ASE_ERR_TYPE ReadOneLine();
	static char * GetLastToken();
	static void Skip();

	static void PushDepth( long lDepth );
	static long PeekDepth();
	static long PopDepth();

	static FILE *ms_pFile;
	
	static char ms_szLineBuff[ASE_MAX_LINELENGTH+1];
	static char ms_szTmpBuff[ASE_MAX_LINELENGTH+1];
	static char ms_szTokenBuff[ASE_MAX_TOKEN_IN_LINE][ASE_MAX_TOKENLENGTH+1];
	static long ms_lTokenCount; // 현재 읽어들인 줄에서의 토큰 갯수
	static ASE_ERR_TYPE ms_parseState;

	static long ms_lCurDepth;
	static long ms_alDepthStack[ASE_MAX_DEPTH];
	static long ms_lDepthStackLength;

	static long ms_lLineCount;
	//static ASE_OBJECT *ms_apObjects[ASE_MAX_OBJECTS];
	//static ASE_MTRL *ms_apMaterials[ASE_MAX_MATERIALS];
	static void *ms_apTmpKey[ASE_MAX_ANI_LENGTH];
};



inline void CContainerASE::PushDepth(long lDepth)
{
	if( ms_lDepthStackLength < ASE_MAX_DEPTH )
	{
		ms_alDepthStack[ms_lDepthStackLength] = lDepth;
		ms_lDepthStackLength++;
	}
}


inline long CContainerASE::PeekDepth()
{
	if( ms_lDepthStackLength > 0 )
		return ms_alDepthStack[ms_lDepthStackLength - 1];
	else
		return -1;
}


inline long CContainerASE::PopDepth()
{
	if( ms_lDepthStackLength > 0 )
	{
		ms_lDepthStackLength--;
		return ( ms_alDepthStack[ms_lDepthStackLength] );
	}
	else
		return -1;
}



inline char * CContainerASE::GetLastToken()
{
	return ms_szTokenBuff[ms_lTokenCount - 1];
}


#endif // !defined(AFX_CONTAINERASE_H__9EBF9328_1E27_11D4_891B_0000E8EB4C69__INCLUDED_)
