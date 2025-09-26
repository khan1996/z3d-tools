// Global variables
//
//////////////////////////////////////////////////////////////////////

#include "Z3D_GLOBALS.h"
#include "RenderOption.h"
#include "Z3DCharacterModel.h"
#include "Z3DGeneralChrModel.h"
#include "Z3DManagedObject.h"
#include "Z3DGlowHandler.h"

#include "D3dx8.h"

#include "BaseGraphicsLayer.h"
#include "GMMemory.h"



static DWORD D3DVERTEX_DECL[] = // for AMesh vertex shader(D3DVERTEX)
{
	D3DVSD_STREAM(0),
	D3DVSD_REG( 0, D3DVSDT_FLOAT3 ),	// position
	D3DVSD_REG( 1, D3DVSDT_FLOAT3 ),	// normal
	D3DVSD_REG( 2, D3DVSDT_FLOAT2 ),	// uv
	D3DVSD_END()
};

static char szAMeshVertexShader[] =
	"vs.1.0\n"
	"m4x4 r0, v0, c4\n"		// applying world transform
	"m4x4 oPos, r0, c0\n"	// applying view&projection
	"m4x3 r0, v1, c4\n"		// apply world transform to normal
	"dp3 oD0, r0, c8\n"		// normal & light inner prod.
	"mov oT0.xy, v2\n";
// c0 : view x proj
// c4 : world
// c8 : light
// c11 : diffuse(or nothing)


//CH3DMeshContainer g_ContLODMesh;
CZ3DMeshContainer g_ContLODMesh;
CH3DTexPieceContainer g_ContTexturePiece;
CH3DAniKeyPackContainer g_ContAniKeyPack;
CH3DAMeshContainer g_ContAMesh;
//CH3DTextureContainer g_ContTexture;
CZ3DTextureContainer g_ContTexture;
/*CH3DOutfitTableContainer g_ContOutfitTable;
CH3DWeaponTableContainer g_ContWeaponTable;*/

//CZ3DStringTable g_TokGCMDSName;
CZ3DStringTable g_TokSkeletonPartName;
CZ3DStringTable g_TokSheetName;
CZ3DStringTable g_TokMotionName;
CZ3DStringTable g_TokFileName;
CZ3DStringTable g_TokSlotName;
CZ3DStringTable g_TokOutfitName;
CZ3DStringTable g_TokAttachmentName;
CZ3DStringTable g_TokEventName;
CZ3DStringTable g_TokTemp;



CZ3DMapTok2FileName g_MapEvent2SndFile;


//DWORD g_dwAMeshVertexShader = 0;



bool _Z3DGlobalInit( const char* szDataRoot, IDirect3DDevice8* pDevice )
{
	char szFullPath[300];

	sprintf( szFullPath, "%s%s", szDataRoot, "/Ani/" );
	g_ContAniKeyPack.SetFilePath( szFullPath );
	//g_ContAniKeyPack.PreLoadAll( "*.APack" );

	sprintf( szFullPath, "%s%s", szDataRoot, "/Mesh/" );
	g_ContLODMesh.SetFilePath( szFullPath );
	//g_ContLODMesh.PreLoadAll( "*.Z3M" );
	
	// attachment mesh
	sprintf( szFullPath, "%s%s", szDataRoot, "/AMesh/" );
	g_ContAMesh.SetFilePath( szFullPath );
	//g_ContAMesh.PreLoadAll( "*.Z3AM" );

	// texture
	if(CRenderOption::m_TextureMethod==1)
	{
		sprintf( szFullPath, "%s%s", szDataRoot, "/Texture/" );
	}
	else
	{
		sprintf( szFullPath, "%s%s", szDataRoot, "/LowTexture/" );
	}
	g_ContTexture.SetFilePath( szFullPath );

	Z3DTexture::_Init( pDevice, szFullPath, 0, -1 );
	CZ3DRenderable::_Init( pDevice );

	CZ3DGlowHandler::_Init( pDevice, (CRenderOption::m_TextureMethod==1)?false:true );

	if( FALSE == CZ3DCharacterModel::_Init( szDataRoot ) )
	{
		MessageBox( NULL, "CZ3DCharacterModel::_Init() error", "Z3D", MB_ICONEXCLAMATION );
		return false;
	}

	if( false == CZ3DGeneralChrModel::_BuildCMDSList() )
	{
		MessageBox( NULL, "CZ3DGeneralChrModel::_BuildCMDSList() error", "Z3D", MB_ICONEXCLAMATION );
		return false;
	}

	sprintf( szFullPath, "%s%s", szDataRoot, "/ChrEvent.EDS" );
	if( false == CZ3DChrEventGenerator::_Load( szFullPath ) )
	{
		MessageBox( NULL, "CZ3DChrEventGenerator::_Load() error", "Z3D", MB_ICONEXCLAMATION );
		return false;
	}

	/*ID3DXBuffer* pCode;
	HRESULT hr = D3DXAssembleShader( szAMeshVertexShader, strlen(szAMeshVertexShader),
				0, NULL, &pCode, NULL );
	pDevice->CreateVertexShader( D3DVERTEX_DECL, (DWORD*)pCode->GetBufferPointer(),
							&g_dwAMeshVertexShader, 0 );
	SAFE_RELEASE( pCode );*/

	return true;
}


void _Z3DGlobalClose( IDirect3DDevice8* pDevice )
{
	/*if( g_dwAMeshVertexShader )
	{
		pDevice->DeleteVertexShader( g_dwAMeshVertexShader );
	}*/

	CZ3DGlowHandler::_Close();

	g_ContTexture.FlushAll();
	g_ContAMesh.FlushAll();
	g_ContLODMesh.FlushAll();
	g_ContAniKeyPack.FlushAll();
	
	CZ3DRenderable::_Close();
	CZ3DManagedObject::Close();
	Z3DTexture::_Close();
	Z3DMultipartPortion::_Close();

	CZ3DChrEventGenerator::_Close();
	CZ3DGeneralChrModel::_ClearCMDSList();
}
