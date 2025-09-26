// ContainerASE.cpp: implementation of the CContainerASE class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"

#include "ContainerASE.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>


struct ASE_TOKENnID {	char *szToken;	ASE_TOKEN_TYPE nId; } TokenTable[] = 
{
	// Braces
	"{",							ASE_LEFTBRACE,
	"}",							ASE_RIGHTBRACE,

	// Top level category ID's
	"*SCENE",						ASE_SCENE,
	"*GEOMOBJECT",					ASE_GEOMETRY,
	"*SHAPEOBJECT",					ASE_SHAPE,
	"*CAMERAOBJECT",				ASE_CAMERA,
	"*LIGHTOBJECT",					ASE_LIGHT,
	"*HELPEROBJECT",				ASE_HELPER,
	"*MATERIAL_LIST",				ASE_MATERIAL_LIST,

	// Hierarchy
	"*GROUP",						ASE_GROUP,
	"*NODE_TM",						ASE_NODE_TM,
	"*NODE_NAME",					ASE_NODE_NAME,
	"*NODE_PARENT",					ASE_NODE_PARENT,

	// Object (node) properties
	"*PROP_MOTIONBLUR",				ASE_PROP_MOTIONBLUR,
	"*PROP_CASTSHADOW",				ASE_PROP_CASTSHADOW,
	"*PROP_RECVSHADOW",				ASE_PROP_RECVSHADOW,

	// Mesh related ID's
	"*MESH",						ASE_MESH,
	"*MESH_NORMALS",				ASE_MESH_NORMALS,
	"*MESH_NUMVERTEX",				ASE_MESH_NUMVERTEX,
	"*MESH_NUMFACES",				ASE_MESH_NUMFACES,
	"*MESH_VERTEX_LIST",			ASE_MESH_VERTEX_LIST,
	"*MESH_VERTEX",					ASE_MESH_VERTEX,
	"*MESH_FACE_LIST",				ASE_MESH_FACE_LIST,
	"*MESH_FACE",					ASE_MESH_FACE,
	"*MESH_SMOOTHING",				ASE_MESH_SMOOTHING,
	"*MESH_MTLID",					ASE_MESH_MTLID,

	"*MESH_NUMTVERTEX",				ASE_MESH_NUMTVERTEX,
	"*MESH_NUMTVFACES",				ASE_MESH_NUMTVFACES,
	"*MESH_TVERTLIST",				ASE_MESH_TVERTLIST,
	"*MESH_TVERT",					ASE_MESH_TVERT,
	"*MESH_TFACELIST",				ASE_MESH_TFACELIST,
	"*MESH_TFACE",					ASE_MESH_TFACE,

	"*MESH_NUMCVERTEX",				ASE_MESH_NUMCVERTEX,
	"*MESH_NUMCVFACES",				ASE_MESH_NUMCVFACES,
	"*MESH_CVERTLIST",				ASE_MESH_CVERTLIST,
	"*MESH_VERTCOL",				ASE_MESH_VERTCOL,
	"*MESH_CFACELIST",				ASE_MESH_CFACELIST,
	"*MESH_CFACE",					ASE_MESH_CFACE,

	"*MESH_MAPPINGCHANNEL",			ASE_MESH_MAPPINGCHANNEL,

	"*MESH_FACEMAPLIST",			ASE_MESH_FACEMAPLIST,
	"*MESH_FACEMAP",				ASE_MESH_FACEMAP,
	"*MESH_FACEMAPVERT",			ASE_MESH_FACEVERT,

	"*MESH_FACENORMAL",				ASE_MESH_FACENORMAL,
	"*MESH_VERTEXNORMAL",			ASE_MESH_VERTEXNORMAL,
	"*MESH_ANIMATION",				ASE_MESH_ANIMATION,

	// Helper objects
	"*HELPER_CLASS",				ASE_HELPER_CLASS,

	// Controller ID's
	"*CONTROL_POINT3_TCB",			ASE_CONTROL_POINT3_TCB,
	"*CONTROL_POINT3_BEZIER",		ASE_CONTROL_POINT3_BEZIER,
	"*CONTROL_COLOR_BEZIER",		ASE_CONTROL_COLOR_BEZIER,
	"*CONTROL_POINT3_SAMPLE",		ASE_CONTROL_POINT3_SAMPLE,	

	"*CONTROL_FLOAT_TCB",			ASE_CONTROL_FLOAT_TCB,
	"*CONTROL_FLOAT_BEZIER",		ASE_CONTROL_FLOAT_BEZIER,
	"*CONTROL_FLOAT_LINEAR",		ASE_CONTROL_FLOAT_BEZIER,
	"*CONTROL_FLOAT_SAMPLE",		ASE_CONTROL_FLOAT_SAMPLE,

	// "Track" is the identification of a sampled controller
	"*CONTROL_POS_TRACK",			ASE_POS_TRACK,
	"*CONTROL_ROT_TRACK",			ASE_ROT_TRACK,
	"*CONTROL_SCALE_TRACK",			ASE_SCALE_TRACK,

	// Sampled keys
	"*CONTROL_POS_SAMPLE",			ASE_POS_SAMPLE,
	"*CONTROL_ROT_SAMPLE",			ASE_ROT_SAMPLE,
	"*CONTROL_SCALE_SAMPLE",		ASE_SCALE_SAMPLE,

	// Specific controller keys
	"*CONTROL_POS_KEY",				ASE_POS_KEY,
	"*CONTROL_ROT_KEY",				ASE_ROT_KEY,
	"*CONTROL_SCALE_KEY",			ASE_SCALE_KEY,
	"*CONTROL_POINT3_KEY",			ASE_POINT3_KEY,
	"*CONTROL_FLOAT_KEY",			ASE_FLOAT_KEY,

	// TCB Keys have Tens, cont, bias, easeIn, easeOut
	"*CONTROL_TCB_POINT3_KEY",		ASE_TCB_POINT3_KEY,
	"*CONTROL_TCB_FLOAT_KEY",		ASE_TCB_FLOAT_KEY,
	"*CONTROL_TCB_POS_KEY",			ASE_TCB_POS_KEY,
	"*CONTROL_TCB_ROT_KEY",			ASE_TCB_ROT_KEY,
	"*CONTROL_TCB_SCALE_KEY",		ASE_TCB_SCALE_KEY,

	// Bezier keys have inTan, outTan
	"*CONTROL_BEZIER_FLOAT_KEY",	ASE_BEZIER_FLOAT_KEY,
	"*CONTROL_BEZIER_POINT3_KEY",	ASE_BEZIER_POINT3_KEY,
	"*CONTROL_BEZIER_POS_KEY",		ASE_BEZIER_POS_KEY,
	"*CONTROL_BEZIER_SCALE_KEY",	ASE_BEZIER_SCALE_KEY,

	"*CONTROL_POS_LINEAR",			ASE_CONTROL_POS_LINEAR,
	"*CONTROL_POS_TCB",				ASE_CONTROL_POS_TCB,
	"*CONTROL_POS_BEZIER",			ASE_CONTROL_POS_BEZIER,
	"*CONTROL_ROT_LINEAR",			ASE_CONTROL_ROT_LINEAR,
	"*CONTROL_ROT_TCB",				ASE_CONTROL_ROT_TCB,
	"*CONTROL_ROT_BEZIER",			ASE_CONTROL_ROT_BEZIER,
	"*CONTROL_SCALE_LINEAR",		ASE_CONTROL_SCALE_LINEAR,
	"*CONTROL_SCALE_TCB",			ASE_CONTROL_SCALE_TCB,
	"*CONTROL_SCALE_BEZIER",		ASE_CONTROL_SCALE_BEZIER,

	// Material / Texture related ID's
	"*WIREFRAME_COLOR",				ASE_WIRECOLOR,
	"*MATERIAL",					ASE_MATERIAL,
	"*MATERIAL_COUNT",				ASE_MATERIAL_COUNT,
	"*MATERIAL_REF",				ASE_MATERIAL_REF,
	"*NUMSUBMTLS",					ASE_NUMSUBMTLS,
	"*SUBMATERIAL",					ASE_SUBMATERIAL,
	"*MATERIAL_NAME",				ASE_MATNAME,
	"*MATERIAL_CLASS",				ASE_MATCLASS,

	"*MATERIAL_AMBIENT",			ASE_AMBIENT,
	"*MATERIAL_DIFFUSE",			ASE_DIFFUSE,
	"*MATERIAL_SPECULAR",			ASE_SPECULAR,
	"*MATERIAL_SHINE",				ASE_SHINE,
	"*MATERIAL_SHINESTRENGTH",		ASE_SHINE_STRENGTH,
	"*MATERIAL_TRANSPARENCY",		ASE_TRANSPARENCY,
	"*MATERIAL_WIRESIZE",			ASE_WIRESIZE,
	
	"*MATERIAL_SHADING",			ASE_SHADING,
	"*MATERIAL_SELFILLUM",			ASE_SELFILLUM,
	"*MATERIAL_TWOSIDED",			ASE_TWOSIDED,
	"*MATERIAL_WIRE",				ASE_WIRE,
	"*MATERIAL_FACEMAP",			ASE_FACEMAP,

	"*MAP_NAME",					ASE_TEXNAME,
	"*MAP_CLASS",					ASE_TEXCLASS,
	"*MAP_SUBNO",					ASE_TEXSUBNO,
	"*MAP_AMOUNT",					ASE_TEXAMOUNT,

	"*BITMAP",						ASE_BITMAP,

	"*MAP_TYPE",					ASE_MAPTYPE,
	"*UVW_U_OFFSET",				ASE_U_OFFSET,
	"*UVW_V_OFFSET",				ASE_V_OFFSET,
	"*UVW_U_TILING",				ASE_U_TILING,
	"*UVW_V_TILING",				ASE_V_TILING,
	"*UVW_ANGLE",					ASE_ANGLE,

	// Sub texture types
	"*MAP_DIFFUSE",					ASE_MAP_DIFFUSE,
	"*MAP_OPACITY",					ASE_MAP_OPACITY,
	"*MAP_BUMP",					ASE_MAP_BUMP,
	"*MAP_REFLECT",					ASE_MAP_REFLECT,

	// TM related ID's
	"*TM_ROW0",						ASE_TM_ROW0,
	"*TM_ROW1",						ASE_TM_ROW1,
	"*TM_ROW2",						ASE_TM_ROW2,
	"*TM_ROW3",						ASE_TM_ROW3,
	"*TM_POS",						ASE_TM_POS,
	"*TM_ROTAXIS",					ASE_TM_ROTAXIS,
	"*TM_ROTANGLE",					ASE_TM_ROTANGLE,
	"*TM_SCALE",					ASE_TM_SCALE,
	"*TM_SCALEAXIS",				ASE_TM_SCALEAXIS,
	"*TM_SCALEAXISANG",				ASE_TM_SCALEAXISANG,
	"*TM_ANIMATION",				ASE_TM_ANIMATION,

	// TM Inheritance flags
	"*INHERIT_POS",					ASE_INHERIT_POS,
	"*INHERIT_ROT",					ASE_INHERIT_ROT,
	"*INHERIT_SCL",					ASE_INHERIT_SCL,

	// Scene related ID's
	"*SCENE_FILENAME",				ASE_FILENAME,
	"*SCENE_FIRSTFRAME",			ASE_FIRSTFRAME,
	"*SCENE_LASTFRAME",				ASE_LASTFRAME,
	"*SCENE_FRAMESPEED",			ASE_FRAMESPEED,
	"*SCENE_TICKSPERFRAME",			ASE_TICKSPERFRAME,
	"*SCENE_ENVMAP",				ASE_ENVMAP,
	"*SCENE_BACKGROUND_STATIC",		ASE_STATICBGCOLOR,
	"*SCENE_BACKGROUND_ANIM",		ASE_ANIMBGCOLOR,
	"*SCENE_AMBIENT_STATIC",		ASE_STATICAMBIENT,
	"*SCENE_AMBIENT_ANIM",			ASE_ANIMAMBIENT,

	// Generic ID's that can show up here and there
	"*TIMEVALUE",					ASE_TIMEVALUE,
	"*COMMENT",						ASE_COMMENT,
	"*3DSMAX_ASCIIEXPORT",			ASE_FILEID,
	"*BOUNDINGBOX_MIN",				ASE_BOUNDINGBOX_MIN,
	"*BOUNDINGBOX_MAX",				ASE_BOUNDINGBOX_MAX,

	// Physique ID's
	"*PHYSIQUE",					ASE_PHYSIQUE,
	"*PHYSIQUE_NUMVERTEXASSIGNMENT",
									ASE_PHYSIQUE_NUMVERTEX,
	"*PHYSIQUE_VERTEXASSIGNMENT_LIST",
									ASE_PHYSIQUE_LIST,
	"*PHYSIQUE_VERTEXASSIGNMENT_BLENDED_RIGIDTYPE",
									ASE_PHYSIQUE_TYPE_BLENDRIGID,
	"*PHYSIQUE_VERTEXASSIGNMENT_NUMNODE",
									ASE_PHYSIQUE_NUMNODE,
	"*PHYSIQUE_VERTEXASSIGNMENT_NODELIST",
									ASE_PHYSIQUE_NODELIST,
	"*PHYSIQUE_VERTEXASSIGNMENT_NODE",
									ASE_PHYSIQUE_NODE,
	"*PHYSIQUE_VERTEXASSIGNMENT_NONBLENDED_RIGIDTYPE",
									ASE_PHYSIQUE_TYPE_NONBLENDRIGID,

	// User properties
	"*SKEL_KEY_ID",					ASE_SKEL_KEY_ID,
	"*BODY_PART_ID",				ASE_BODY_PART_ID,
	"*PART_KEY_ID",					ASE_PART_KEY_ID,

	// Identifies end of item.
	NULL,							ASE_UNKNOWN
};


// static 멤버 변수 초기화
FILE *CContainerASE::ms_pFile = NULL;
char CContainerASE::ms_szLineBuff[ASE_MAX_LINELENGTH+1] = { 0, };
char CContainerASE::ms_szTmpBuff[ASE_MAX_LINELENGTH+1] = { 0, };
char CContainerASE::ms_szTokenBuff[ASE_MAX_TOKEN_IN_LINE][ASE_MAX_TOKENLENGTH+1] = { 0, };
long CContainerASE::ms_lTokenCount = 0;
ASE_ERR_TYPE CContainerASE::ms_parseState = ASE_ERR_NO_ERROR;
long CContainerASE::ms_lCurDepth = 0;
long CContainerASE::ms_alDepthStack[ASE_MAX_DEPTH] = { 0, };
long CContainerASE::ms_lDepthStackLength = 0;
long CContainerASE::ms_lLineCount = 0;
//ASE_OBJECT *CContainerASE::ms_apObjects[ASE_MAX_OBJECTS] = { 0, };
//ASE_MTRL *CContainerASE::ms_apMaterials[ASE_MAX_MATERIALS] = { 0, };
void *CContainerASE::ms_apTmpKey[ASE_MAX_ANI_LENGTH] = { 0, };


/////////////////////////////////////////////////////////////
//  Utility& utility func.
/////////////////////////////////////////////////////////////

// ASE계층구조 읽어들이기 루틴용 header/trailer 정의
// Header ->
#define ASE_PARSER_HEADER								\
	ASE_TOKEN_TYPE token;								\
	PushDepth( ms_lCurDepth );							\
	while( !feof( ms_pFile ) )							\
	{													\
		if( ASE_ERR_NO_ERROR != ReadOneLine() ) break;	\
		token = Token2Id( ms_szTokenBuff[0] );			\
		if( ASE_RIGHTBRACE == token )					\
			if( PopDepth() == (ms_lCurDepth+1) )		\
				break;									\
		switch( token )									\
		{
// <-

// Trailer ->
#define ASE_PARSER_TRAILER		\
			case ASE_UNKNOWN:	\
			default:			\
				Skip();			\
				break;			\
		}						\
	}
// <-
	

// 최대 토큰 길이를 넘지 않도록 안전하게 복사
/*#define SAFE_COPY( dest, src ) {					\
	strncpy( dest, src, ASE_MAX_TOKENLENGTH - 1 );	\
	dest[min(ASE_MAX_TOKENLENGTH,strlen(src))-1] = '\0';}*/
inline static void SAFE_COPY( char* dest, const char* src )
{
	strncpy( dest, src, ASE_MAX_TOKENLENGTH-1 );
	dest[max(1, min(ASE_MAX_TOKENLENGTH, strlen(src)))-1];
}





// 블럭의 나머지부분을 가뿐하게 skip~ -_-;;
void CContainerASE::Skip()
{
	if( ASE_LEFTBRACE == Token2Id( GetLastToken() ) )
	{
		ASE_TOKEN_TYPE token;
		PushDepth( ms_lCurDepth );
		while( !feof( ms_pFile ) )
		{
			if( ASE_ERR_NO_ERROR != ReadOneLine() ) break;

			token = Token2Id( ms_szTokenBuff[0] );
			if( ASE_RIGHTBRACE == token && PeekDepth() == (ms_lCurDepth+1) )
			{
				PopDepth();
				break;
			}
		}
	}
	//else
	// 블럭이 아닐 경우는 달리 해 줄 일이 없다....
}




//
//	Token2Id()
//
//	주어진 토큰문자열에 해당하는 Id번호를 반환
//
ASE_TOKEN_TYPE CContainerASE::Token2Id(const char *szToken)
{
	int i, j;
	for( i = 0; TokenTable[i].szToken; i++ )
	{
		for( j = 0; TokenTable[i].szToken[j]; j++ )
		{
			if( TokenTable[i].szToken[j] != szToken[j] )
				break;
		}
		if( TokenTable[i].szToken[j] == '\0' && szToken[j] == '\0' ) 
			// 해당되는 토큰 발견
			return TokenTable[i].nId;
	}

	// 해당 토큰 없음.
	return ASE_UNKNOWN;
}



//
//	RipOutString()
//
//	한 줄 읽어낸 내용에서 따옴표 안의 내용만 추려내 임시장소에 저장
//
char *CContainerASE::RipOutString()
{
	int len, i;

	// '"'(double quat.) 앞부분을 건너뜀
	for( i = 0; (ms_szLineBuff[i] != 0) && (ms_szLineBuff[i] != '\"'); i++ );

	if( ms_szLineBuff[i] == 0 ) return 0;

	// 한칸 더 건너뛰어야 실질적인 문자열 부분
	i++;

	// 문자열 부분 복사 
	for( len = 0; (ms_szLineBuff[i] != 0) && (ms_szLineBuff[i] != '\"') &&
		(i < ASE_MAX_LINELENGTH); i++, len++)
		ms_szTmpBuff[len] = ms_szLineBuff[i];

	// terminating
	ms_szTmpBuff[len] = '\0';
		
	//return len;
	return ms_szTmpBuff;
}


ASE_ERR_TYPE CContainerASE::ReadOneLine()
{
	_ASSERTE( ms_pFile );

	fgets( ms_szLineBuff, ASE_MAX_LINELENGTH, ms_pFile );
	ms_lLineCount++;

	ms_lTokenCount = sscanf( ms_szLineBuff,
		"%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
		ms_szTokenBuff[0], ms_szTokenBuff[1], ms_szTokenBuff[2], ms_szTokenBuff[3],
		ms_szTokenBuff[4], ms_szTokenBuff[5], ms_szTokenBuff[6], ms_szTokenBuff[7],
		ms_szTokenBuff[8], ms_szTokenBuff[9], ms_szTokenBuff[10], ms_szTokenBuff[11],
		ms_szTokenBuff[12], ms_szTokenBuff[13], ms_szTokenBuff[14], ms_szTokenBuff[15],
		ms_szTokenBuff[16], ms_szTokenBuff[17], ms_szTokenBuff[18], ms_szTokenBuff[19]);


	if( ASE_RIGHTBRACE == Token2Id( ms_szTokenBuff[0] ) )
		ms_lCurDepth--;
	if( ASE_LEFTBRACE == Token2Id( GetLastToken() ) )
		ms_lCurDepth++;

	return ASE_ERR_NO_ERROR;
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction/Reseting
//////////////////////////////////////////////////////////////////////
CContainerASE::CContainerASE()
{
	m_lFirstFrame = 0;
	m_lLastFrame = 0;
	m_lTicksPerFrame = 1;
}

CContainerASE::~CContainerASE()
{
	int i;

	if( m_vecpObjects.size() )
	{
		for( i = 0; i < (int)m_vecpObjects.size(); i++ )
		{
			SAFE_DELETE( m_vecpObjects[i] );
		}
	}

	if( m_vecpMaterials.size() )
	{
		for( i = 0; i < (int)m_vecpMaterials.size(); i++ )
		{
			SAFE_DELETE( m_vecpMaterials[i] );
		}
	}
}


bool CContainerASE::Reset()
{
	// release every allocated space
	int i;

	if( m_vecpObjects.size() )
	{
		for( i = 0; i < (int)m_vecpObjects.size(); i++ )
		{
			SAFE_DELETE( m_vecpObjects[i] );
		}
	}
	m_vecpObjects.clear();

	if( m_vecpMaterials.size() )
	{
		for( i = 0; i < (int)m_vecpMaterials.size(); i++ )
		{
			SAFE_DELETE( m_vecpMaterials[i] );
		}
	}
	m_vecpMaterials.clear();

	// reset values
	m_lFirstFrame = 0;
	m_lLastFrame = 0;
	m_lTicksPerFrame = 1;

	return true;
}


///////////////////////////////////////////////
//
///////////////////////////////////////////////
ASE_ERR_TYPE CContainerASE::ReadASE( const char *szFileName )
{
	// Reset static members
	ms_lCurDepth = 0;
	ms_lDepthStackLength = 0;
	ms_lTokenCount = 0;
	ms_lLineCount = 0;
	
	ms_pFile = fopen( szFileName, "rt" );
	if( ms_pFile == NULL )
		return ASE_ERR_FILE_NOT_FOUND;

	SAFE_COPY( m_szFileName, szFileName );
	


	ASE_PARSER_HEADER
//	{
		case ASE_SCENE:
			ReadScene();
			break;

		case ASE_GEOMETRY:
			ReadObject( true );
			break;

		case ASE_HELPER:
			ReadObject( false );
			break;

		case ASE_MATERIAL_LIST:
			ReadMaterial();
			break;
//	}
	ASE_PARSER_TRAILER


	fclose( ms_pFile );
	return ASE_ERR_NO_ERROR;
}


void CContainerASE::ReadScene()
{
	ASE_PARSER_HEADER
//	{
		case ASE_FIRSTFRAME:
			m_lFirstFrame = atoi( ms_szTokenBuff[1] );
			break;

		case ASE_LASTFRAME:
			m_lLastFrame = atoi( ms_szTokenBuff[1] );
			break;

		case ASE_TICKSPERFRAME:
			m_lTicksPerFrame = atoi( ms_szTokenBuff[1] );
			break;
//	}
	ASE_PARSER_TRAILER
}


void CContainerASE::ReadObject( bool geomflag )
{
	ASE_OBJECT* pObj = new ASE_OBJECT;
	pObj->bIsGeomObject = geomflag;
	m_vecpObjects.push_back( pObj );

	ASE_PARSER_HEADER
//	{
		case ASE_NODE_NAME:
			SAFE_COPY( m_vecpObjects.back()->szName, RipOutString() );
			break;
		
		case ASE_NODE_PARENT:
			SAFE_COPY( m_vecpObjects.back()->szParentName, RipOutString() );
			break;

		case ASE_NODE_TM:
			ReadTM();
			break;

		case ASE_MESH:
			ReadMesh();
			break;

		case ASE_TM_ANIMATION:
			ReadAni();
			break;

		case ASE_WIRECOLOR:
			m_vecpObjects.back()->color.r = (float)atof( ms_szTokenBuff[1] );
			m_vecpObjects.back()->color.g = (float)atof( ms_szTokenBuff[2] );
			m_vecpObjects.back()->color.b = (float)atof( ms_szTokenBuff[3] );
			break;

		case ASE_MATERIAL_REF:
			m_vecpObjects.back()->lMat_id = atoi( ms_szTokenBuff[1] );
			break;

		case ASE_PHYSIQUE:
			ReadPhysique();
			break;

		case ASE_SKEL_KEY_ID:
			m_vecpObjects.back()->lSkelKeyId = atoi( ms_szTokenBuff[1] );
			break;

		case ASE_BODY_PART_ID:
			m_vecpObjects.back()->lBodyPartId = atoi( ms_szTokenBuff[1] );
			break;

		case ASE_PART_KEY_ID:
			m_vecpObjects.back()->lPartKeyId = atoi( ms_szTokenBuff[1] );
			break;
//	}
	ASE_PARSER_TRAILER
}


static void ConvAngAxis2Quat( float& x, float& y, float& z, float& w )
{
	float n = sqrtf( x*x + y*y + z*z );
	if( n < FLOAT_EPSILON ) n = 1.0f;
	w = w / 2;
	x *= sinf(w)/n;
	y *= sinf(w)/n;
	z *= sinf(w)/n;
	w = cosf(w);
}

void CContainerASE::ReadTM()
{
	ZeroMemory( &(m_vecpObjects.back()->TM), sizeof( ASE_MATRIX4 ) );
	m_vecpObjects.back()->TM._14 =
	m_vecpObjects.back()->TM._24 =
	m_vecpObjects.back()->TM._34 = 0.0f;
	m_vecpObjects.back()->TM._44 = 1.0f;

	ASE_PARSER_HEADER
//	{
		case ASE_TM_ROW0:
			m_vecpObjects.back()->TM._11 = (float)atof( ms_szTokenBuff[1] );
			m_vecpObjects.back()->TM._12 = (float)atof( ms_szTokenBuff[2] );
			m_vecpObjects.back()->TM._13 = (float)atof( ms_szTokenBuff[3] );
			break;

		case ASE_TM_ROW1:
			m_vecpObjects.back()->TM._21 = (float)atof( ms_szTokenBuff[1] );
			m_vecpObjects.back()->TM._22 = (float)atof( ms_szTokenBuff[2] );
			m_vecpObjects.back()->TM._23 = (float)atof( ms_szTokenBuff[3] );
			break;

		case ASE_TM_ROW2:
			m_vecpObjects.back()->TM._31 = (float)atof( ms_szTokenBuff[1] );
			m_vecpObjects.back()->TM._32 = (float)atof( ms_szTokenBuff[2] );
			m_vecpObjects.back()->TM._33 = (float)atof( ms_szTokenBuff[3] );
			break;

		case ASE_TM_ROW3:
			m_vecpObjects.back()->TM._41 = (float)atof( ms_szTokenBuff[1] );
			m_vecpObjects.back()->TM._42 = (float)atof( ms_szTokenBuff[2] );
			m_vecpObjects.back()->TM._43 = (float)atof( ms_szTokenBuff[3] );
			break;

		case ASE_TM_POS:
			m_vecpObjects.back()->TM_PosKey.tick = 0;
			m_vecpObjects.back()->TM_PosKey.x = (float)atof( ms_szTokenBuff[1] );
			m_vecpObjects.back()->TM_PosKey.y = (float)atof( ms_szTokenBuff[2] );
			m_vecpObjects.back()->TM_PosKey.z = (float)atof( ms_szTokenBuff[3] );
			break;

		case ASE_TM_ROTAXIS:
			m_vecpObjects.back()->TM_RotKey.tick = 0;
			m_vecpObjects.back()->TM_RotKey.x = (float)atof( ms_szTokenBuff[1] );
			m_vecpObjects.back()->TM_RotKey.y = (float)atof( ms_szTokenBuff[2] );
			m_vecpObjects.back()->TM_RotKey.z = (float)atof( ms_szTokenBuff[3] );
			break;

		case ASE_TM_ROTANGLE:
			m_vecpObjects.back()->TM_RotKey.w = (float)atof( ms_szTokenBuff[1] );
			// axis&angle to quaternion
			ConvAngAxis2Quat( m_vecpObjects.back()->TM_RotKey.x,
				m_vecpObjects.back()->TM_RotKey.y,
				m_vecpObjects.back()->TM_RotKey.z,
				m_vecpObjects.back()->TM_RotKey.w );
			break;

		case ASE_TM_SCALE:
			m_vecpObjects.back()->TM_SclKey.tick = 0;
			m_vecpObjects.back()->TM_SclKey.x = (float)atof( ms_szTokenBuff[1] );
			m_vecpObjects.back()->TM_SclKey.y = (float)atof( ms_szTokenBuff[2] );
			m_vecpObjects.back()->TM_SclKey.z = (float)atof( ms_szTokenBuff[3] );
			break;
//	}
	ASE_PARSER_TRAILER
}


void CContainerASE::ReadMesh()
{
	ASE_PARSER_HEADER
//	{
		case ASE_MESH_NUMVERTEX:
			m_vecpObjects.back()->lVertexCount = atoi( ms_szTokenBuff[1] );
			m_vecpObjects.back()->pVertices = new ASE_VECTOR3F[m_vecpObjects.back()->lVertexCount];
			break;

		case ASE_MESH_NUMTVERTEX:
			m_vecpObjects.back()->lTVertexCount = atoi( ms_szTokenBuff[1] );
			if( m_vecpObjects.back()->lTVertexCount )
				m_vecpObjects.back()->pTVertices = new ASE_UVCOORD[m_vecpObjects.back()->lTVertexCount];
			break;

		case ASE_MESH_NUMCVERTEX:
			m_vecpObjects.back()->lCVertexCount = atoi( ms_szTokenBuff[1] );
			if( m_vecpObjects.back()->lTVertexCount )
				m_vecpObjects.back()->pCVertices = new ASE_COLOR3F[m_vecpObjects.back()->lCVertexCount];
			break;

		case ASE_MESH_NUMFACES:
			m_vecpObjects.back()->lFaceCount = atoi( ms_szTokenBuff[1] );
 			m_vecpObjects.back()->pFaces = new ASE_FACE[m_vecpObjects.back()->lFaceCount];
			break;

		case ASE_MESH_VERTEX_LIST:
			ReadVertices();
			break;

		case ASE_MESH_TVERTLIST:
			ReadTextureVertices();
			break;

		case ASE_MESH_CVERTLIST:
			ReadColorVertices();
			break;

		case ASE_MESH_FACE_LIST:
			ReadFaces();
			break;

		case ASE_MESH_TFACELIST:
			ReadTextureFaces();
			break;

		case ASE_MESH_CFACELIST:
			ReadColorFaces();
			break;

		case ASE_MESH_NORMALS:
			ReadNormals();
			break;
//	}
	ASE_PARSER_TRAILER
}


void CContainerASE::ReadVertices()
{
	ASE_VECTOR3F *p;

	ASE_PARSER_HEADER
//	{
		case ASE_MESH_VERTEX:
			p = &(m_vecpObjects.back()->pVertices[atoi(ms_szTokenBuff[1])]);
			p->x = (float)atof( ms_szTokenBuff[2] );
			p->y = (float)atof( ms_szTokenBuff[3] );
			p->z = (float)atof( ms_szTokenBuff[4] );
			break;
//	}
	ASE_PARSER_TRAILER
}


void CContainerASE::ReadTextureVertices()
{
	ASE_UVCOORD *p;

	ASE_PARSER_HEADER
//	{
		case ASE_MESH_TVERT:
			p = &(m_vecpObjects.back()->pTVertices[atoi(ms_szTokenBuff[1])]);
			p->u = (float)atof( ms_szTokenBuff[2] );
			p->v = (float)atof( ms_szTokenBuff[3] );
			break;
//	}
	ASE_PARSER_TRAILER
}


void CContainerASE::ReadColorVertices()
{
	ASE_COLOR3F *p;

	ASE_PARSER_HEADER
//	{
		case ASE_MESH_VERTCOL:
			p = &(m_vecpObjects.back()->pCVertices[atoi(ms_szTokenBuff[1])]);
			p->r = (float)atof( ms_szTokenBuff[2] );
			p->g = (float)atof( ms_szTokenBuff[3] );
			p->b = (float)atof( ms_szTokenBuff[4] );
			break;
//	}
	ASE_PARSER_TRAILER
}


void CContainerASE::ReadFaces()
{
	ASE_FACE *p;
	long i;

	ASE_PARSER_HEADER
//	{
		case ASE_MESH_FACE:
			p = &(m_vecpObjects.back()->pFaces[atoi(ms_szTokenBuff[1])]);
			p->awIndex[0] = atoi(ms_szTokenBuff[3]);
			p->awIndex[1] = atoi(ms_szTokenBuff[5]);
			p->awIndex[2] = atoi(ms_szTokenBuff[7]);
			for( i = 8; i < ms_lTokenCount; i++ )
			{
				if( ASE_MESH_SMOOTHING == Token2Id( ms_szTokenBuff[i] ) )
					if( atoi( ms_szTokenBuff[i+1] ) )  p->wSgroup_id = atoi( ms_szTokenBuff[i+1] );
				if( ASE_MESH_MTLID == Token2Id( ms_szTokenBuff[i] ) )
					p->wMaterial_id = atoi( ms_szTokenBuff[i+1] );
					// if( atoi( ms_szTokenBuff[i+1] ) )  p->material_id = atoi( ms_szTokenBuff[i+1] );
					// delete this line because material id could be zero
			}
			break;
//	}
	ASE_PARSER_TRAILER
}


void CContainerASE::ReadTextureFaces()
{
	ASE_FACE *p;

	ASE_PARSER_HEADER
//	{
		case ASE_MESH_TFACE:
			p = &(m_vecpObjects.back()->pFaces[atoi(ms_szTokenBuff[1])]);
			p->awTIndex[0] = atoi(ms_szTokenBuff[2]);
			p->awTIndex[1] = atoi(ms_szTokenBuff[3]);
			p->awTIndex[2] = atoi(ms_szTokenBuff[4]);
			break;
//	}
	ASE_PARSER_TRAILER
}


void CContainerASE::ReadColorFaces()
{
	ASE_FACE *p;

	ASE_PARSER_HEADER
//	{
		case ASE_MESH_CFACE:
			p = &(m_vecpObjects.back()->pFaces[atoi(ms_szTokenBuff[1])]);
			p->awCIndex[0] = atoi(ms_szTokenBuff[2]);
			p->awCIndex[1] = atoi(ms_szTokenBuff[3]);
			p->awCIndex[2] = atoi(ms_szTokenBuff[4]);
			break;
//	}
	ASE_PARSER_TRAILER
}


void CContainerASE::ReadNormals()
{
	ASE_FACE *p;
	static long i;

	ASE_PARSER_HEADER
//	{
		case ASE_MESH_FACENORMAL:
			p = &(m_vecpObjects.back()->pFaces[atoi(ms_szTokenBuff[1])]);
			p->vecFacenormal.x = (float)atof( ms_szTokenBuff[2] );
			p->vecFacenormal.y = (float)atof( ms_szTokenBuff[3] );
			p->vecFacenormal.z = (float)atof( ms_szTokenBuff[4] );
			i = 0;
			break;

		case ASE_MESH_VERTEXNORMAL:
			// p is set when read facenormal info
			// because facenormal is followed by 3 vertex normal of face
			p->avecIndexnormals[i].x = (float)atof( ms_szTokenBuff[2] );
			p->avecIndexnormals[i].y = (float)atof( ms_szTokenBuff[3] );
			p->avecIndexnormals[i].z = (float)atof( ms_szTokenBuff[4] );
			i++;
			break;
//	}
	ASE_PARSER_TRAILER
}


void CContainerASE::ReadAni()
{
	ASE_PARSER_HEADER
//	{
	// sampled key data
		case ASE_POS_TRACK:
			m_vecpObjects.back()->ePosKeyType = ASE_KEY_SAMPLE;
			ReadPosKey();
			m_vecpObjects.back()->ppPosKey =
				new ASE_VECTORKEY *[m_vecpObjects.back()->lPosKeyCount];
			// copy from static buffer
			memcpy( m_vecpObjects.back()->ppPosKey, ms_apTmpKey,
				sizeof( ASE_VECTORKEY * ) * m_vecpObjects.back()->lPosKeyCount );
			break;

		case ASE_ROT_TRACK:
			m_vecpObjects.back()->eRotKeyType = ASE_KEY_SAMPLE;
			ReadRotKey();
			m_vecpObjects.back()->ppRotKey =
				new ASE_QUATKEY *[m_vecpObjects.back()->lRotKeyCount];
			// copy from static buffer
			memcpy( m_vecpObjects.back()->ppRotKey, ms_apTmpKey,
				sizeof( ASE_QUATKEY * ) * m_vecpObjects.back()->lRotKeyCount );
			break;

		case ASE_SCALE_TRACK:
			m_vecpObjects.back()->eSclKeyType = ASE_KEY_SAMPLE;
			ReadSclKey();
			m_vecpObjects.back()->ppSclKey =
				new ASE_VECTORKEY *[m_vecpObjects.back()->lSclKeyCount];
			// copy from static buffer
			memcpy( m_vecpObjects.back()->ppSclKey, ms_apTmpKey,
				sizeof( ASE_VECTORKEY * ) * m_vecpObjects.back()->lSclKeyCount );
			break;

	// linear key
		case ASE_CONTROL_POS_LINEAR:
			m_vecpObjects.back()->ePosKeyType = ASE_KEY_LINEAR;
			ReadPosKey();
			m_vecpObjects.back()->ppPosKey =
				new ASE_VECTORKEY *[m_vecpObjects.back()->lPosKeyCount];
			// copy from static buffer
			memcpy( m_vecpObjects.back()->ppPosKey, ms_apTmpKey,
				sizeof( ASE_VECTORKEY * ) * m_vecpObjects.back()->lPosKeyCount );
			break;

		case ASE_CONTROL_ROT_LINEAR:
			m_vecpObjects.back()->eRotKeyType = ASE_KEY_LINEAR;
			ReadRotKey();
			m_vecpObjects.back()->ppRotKey =
				new ASE_QUATKEY *[m_vecpObjects.back()->lRotKeyCount];
			// copy from static buffer
			memcpy( m_vecpObjects.back()->ppRotKey, ms_apTmpKey,
				sizeof( ASE_QUATKEY * ) * m_vecpObjects.back()->lRotKeyCount );
			break;

		case ASE_CONTROL_SCALE_LINEAR:
			m_vecpObjects.back()->eSclKeyType = ASE_KEY_LINEAR;
			ReadSclKey();
			m_vecpObjects.back()->ppSclKey =
				new ASE_VECTORKEY *[m_vecpObjects.back()->lSclKeyCount];
			// copy from static buffer
			memcpy( m_vecpObjects.back()->ppSclKey, ms_apTmpKey,
				sizeof( ASE_VECTORKEY * ) * m_vecpObjects.back()->lSclKeyCount );
			break;

	// TCB key
		case ASE_CONTROL_POS_TCB:
			m_vecpObjects.back()->ePosKeyType = ASE_KEY_TCB;
			ReadPosKey();
			m_vecpObjects.back()->ppPosKey =
				new ASE_VECTORKEY *[m_vecpObjects.back()->lPosKeyCount];
			// copy from static buffer
			memcpy( m_vecpObjects.back()->ppPosKey, ms_apTmpKey,
				sizeof( ASE_VECTORKEY * ) * m_vecpObjects.back()->lPosKeyCount );
			//Skip();
			break;

		case ASE_CONTROL_ROT_TCB:
			m_vecpObjects.back()->eRotKeyType = ASE_KEY_TCB;
			ReadRotKey();
			m_vecpObjects.back()->ppRotKey =
				new ASE_QUATKEY *[m_vecpObjects.back()->lRotKeyCount];
			// copy from static buffer
			memcpy( m_vecpObjects.back()->ppRotKey, ms_apTmpKey,
				sizeof( ASE_QUATKEY * ) * m_vecpObjects.back()->lRotKeyCount );
			//Skip();
			break;

		case ASE_CONTROL_SCALE_TCB:
			m_vecpObjects.back()->eSclKeyType = ASE_KEY_TCB;
			ReadSclKey();
			m_vecpObjects.back()->ppSclKey =
				new ASE_VECTORKEY *[m_vecpObjects.back()->lSclKeyCount];
			// copy from static buffer
			memcpy( m_vecpObjects.back()->ppSclKey, ms_apTmpKey,
				sizeof( ASE_VECTORKEY * ) * m_vecpObjects.back()->lSclKeyCount );
			//Skip();
			break;

	// Bezier key
		case ASE_CONTROL_POS_BEZIER:
			m_vecpObjects.back()->ePosKeyType = ASE_KEY_BEZIER;
			ReadPosKey();
			m_vecpObjects.back()->ppPosKey =
				new ASE_VECTORKEY *[m_vecpObjects.back()->lPosKeyCount];
			// copy from static buffer
			memcpy( m_vecpObjects.back()->ppPosKey, ms_apTmpKey,
				sizeof( ASE_VECTORKEY * ) * m_vecpObjects.back()->lPosKeyCount );
			//Skip();
			break;

		case ASE_CONTROL_ROT_BEZIER:
			m_vecpObjects.back()->eRotKeyType = ASE_KEY_BEZIER;
			ReadRotKey();
			m_vecpObjects.back()->ppRotKey =
				new ASE_QUATKEY *[m_vecpObjects.back()->lRotKeyCount];
			// copy from static buffer
			memcpy( m_vecpObjects.back()->ppRotKey, ms_apTmpKey,
				sizeof( ASE_QUATKEY * ) * m_vecpObjects.back()->lRotKeyCount );
			//Skip();
			break;

		case ASE_CONTROL_SCALE_BEZIER:
			m_vecpObjects.back()->eSclKeyType = ASE_KEY_BEZIER;
			ReadSclKey();
			m_vecpObjects.back()->ppSclKey =
				new ASE_VECTORKEY *[m_vecpObjects.back()->lSclKeyCount];
			// copy from static buffer
			memcpy( m_vecpObjects.back()->ppSclKey, ms_apTmpKey,
				sizeof( ASE_VECTORKEY * ) * m_vecpObjects.back()->lSclKeyCount );
			//Skip();
			break;
//	}
	ASE_PARSER_TRAILER
}


void CContainerASE::ReadPosKey()
{
	ASE_VECTORKEY *p;

	ASE_PARSER_HEADER
//	{
		case ASE_POS_SAMPLE:
			// falls through

		case ASE_BEZIER_POS_KEY:
			// falls through
			
		case ASE_TCB_POS_KEY:
			// falls through
			
		case ASE_POS_KEY:
			p = new ASE_VECTORKEY;
			ms_apTmpKey[m_vecpObjects.back()->lPosKeyCount] = p;
			p->tick = atoi( ms_szTokenBuff[1] );
			p->x = (float)atof( ms_szTokenBuff[2] );
			p->y = (float)atof( ms_szTokenBuff[3] );
			p->z = (float)atof( ms_szTokenBuff[4] );
			m_vecpObjects.back()->lPosKeyCount++;
			break;
//	}
	ASE_PARSER_TRAILER
}


void CContainerASE::ReadRotKey()
{
	ASE_QUATKEY *p;

	ASE_PARSER_HEADER
//	{
		case ASE_ROT_SAMPLE:
			// falls through

		case ASE_TCB_ROT_KEY:
			// falls through
			
		case ASE_ROT_KEY:
			p = new ASE_QUATKEY;
			ms_apTmpKey[m_vecpObjects.back()->lRotKeyCount] = p;
			p->tick = atoi( ms_szTokenBuff[1] );
			p->x = (float)atof( ms_szTokenBuff[2] );
			p->y = (float)atof( ms_szTokenBuff[3] );
			p->z = (float)atof( ms_szTokenBuff[4] );
			p->w = (float)atof( ms_szTokenBuff[5] );

			// axis&angle to quaternion
			ConvAngAxis2Quat( p->x, p->y, p->z, p->w );

			m_vecpObjects.back()->lRotKeyCount++;
			break;
//	}
	ASE_PARSER_TRAILER
}


void CContainerASE::ReadSclKey()
{
	ASE_VECTORKEY *p;

	ASE_PARSER_HEADER
//	{
		case ASE_SCALE_SAMPLE:
			// falls through

		case ASE_TCB_SCALE_KEY:
			// falls through

		case ASE_BEZIER_SCALE_KEY:
			// falls through

		case ASE_SCALE_KEY:
			p = new ASE_VECTORKEY;
			ms_apTmpKey[m_vecpObjects.back()->lSclKeyCount] = p;
			p->tick = atoi( ms_szTokenBuff[1] );
			p->x = (float)atof( ms_szTokenBuff[2] );
			p->y = (float)atof( ms_szTokenBuff[3] );
			p->z = (float)atof( ms_szTokenBuff[4] );
			m_vecpObjects.back()->lSclKeyCount++;
			break;
//	}
	ASE_PARSER_TRAILER
}


void CContainerASE::ReadPhysique()
{
	ASE_PARSER_HEADER
//	{
		case ASE_PHYSIQUE_NUMVERTEX:
			m_vecpObjects.back()->lPhysDataCount = atoi( ms_szTokenBuff[1] );
			m_vecpObjects.back()->ppPhysiqueData =
				new ASE_PHYS_DATA *[m_vecpObjects.back()->lPhysDataCount];
			break;

		case ASE_PHYSIQUE_LIST:
			ReadPhysList();
			break;
//	}
	ASE_PARSER_TRAILER
}


void CContainerASE::ReadPhysList()
{
	ASE_PHYS_DATA *p;

	ASE_PARSER_HEADER
//	{
		case ASE_PHYSIQUE_TYPE_BLENDRIGID:
			p = new ASE_PHYS_DATA;
			m_vecpObjects.back()->ppPhysiqueData[atoi(ms_szTokenBuff[1])] = p;
			ReadPhysVertex( p );
			break;

		case ASE_PHYSIQUE_TYPE_NONBLENDRIGID:
			p = new ASE_PHYS_DATA;
			m_vecpObjects.back()->ppPhysiqueData[atoi(ms_szTokenBuff[1])] = p;
			p->nodecount = 1;
			p->pNode = new ASE_PHYS_NODE;
			p->pNode->weight = 1.0f;
			strcpy( p->pNode->name, RipOutString() );
			break;
//	}
	ASE_PARSER_TRAILER
}


void CContainerASE::ReadPhysVertex( ASE_PHYS_DATA *pData )
{
	ASE_PARSER_HEADER
//	{
		case ASE_PHYSIQUE_NUMNODE:
			pData->nodecount = atoi( ms_szTokenBuff[1] );
			pData->pNode = new ASE_PHYS_NODE[pData->nodecount];
			break;

		case ASE_PHYSIQUE_NODELIST:
			ReadPhysNode( pData->pNode );
			break;
//	}
	ASE_PARSER_TRAILER
}


void CContainerASE::ReadPhysNode( ASE_PHYS_NODE *pNode )
{
	ASE_PARSER_HEADER
//	{
		case ASE_PHYSIQUE_NODE:
			pNode[atoi( ms_szTokenBuff[1] )].weight = (float)atof( ms_szTokenBuff[2] );
			strcpy( pNode[atoi( ms_szTokenBuff[1] )].name, RipOutString() );
			break;
//	}
	ASE_PARSER_TRAILER
}


void CContainerASE::ReadMaterial()
{
	ASE_PARSER_HEADER
//	{
		case ASE_MATERIAL:
			m_vecpMaterials.push_back(new ASE_MTRL);
			m_vecpMaterials.back()->lId = atoi( ms_szTokenBuff[1] );
			ReadMaterialItem();
			break;
//	}
	ASE_PARSER_TRAILER
}


void CContainerASE::ReadMaterialItem()
{
	ASE_PARSER_HEADER
//	{
		case ASE_AMBIENT:
			m_vecpMaterials.back()->ambient.r = (float)atof(ms_szTokenBuff[1]);
			m_vecpMaterials.back()->ambient.g = (float)atof(ms_szTokenBuff[2]);
			m_vecpMaterials.back()->ambient.b = (float)atof(ms_szTokenBuff[3]);
			break;

		case ASE_DIFFUSE:
			m_vecpMaterials.back()->diffuse.r = (float)atof(ms_szTokenBuff[1]);
			m_vecpMaterials.back()->diffuse.g = (float)atof(ms_szTokenBuff[2]);
			m_vecpMaterials.back()->diffuse.b = (float)atof(ms_szTokenBuff[3]);
			break;

		case ASE_SPECULAR:
			m_vecpMaterials.back()->specular.r = (float)atof(ms_szTokenBuff[1]);
			m_vecpMaterials.back()->specular.g = (float)atof(ms_szTokenBuff[2]);
			m_vecpMaterials.back()->specular.b = (float)atof(ms_szTokenBuff[3]);
			break;

		case ASE_TRANSPARENCY:
			m_vecpMaterials.back()->fOpacity = 1.0f - (float)atof(ms_szTokenBuff[1]);
			break;

		case ASE_SELFILLUM:
			m_vecpMaterials.back()->fSelfillum = (float)atof(ms_szTokenBuff[1]);
			break;

		case ASE_MAP_DIFFUSE:
			ReadDiffuseMap();
			break;

		/* for future support -_-;;;
		case ASE_MAP_OPACITY:

		case ASE_MAP_BUMP:

		case ASE_MAP_REFLECT:*/

//	}
	ASE_PARSER_TRAILER
}


void CContainerASE::ReadDiffuseMap()
{
	ASE_PARSER_HEADER
//	{
		case ASE_BITMAP:
			m_vecpMaterials.back()->szTexture = 
				new char[strlen(RipOutString())+1];
			strcpy( m_vecpMaterials.back()->szTexture, ms_szTmpBuff );
			break;
//	}
	ASE_PARSER_TRAILER

}



/*
void Read()
{
	ASE_PARSER_HEADER
//	{

//	}
	ASE_PARSER_TRAILER

}
*/