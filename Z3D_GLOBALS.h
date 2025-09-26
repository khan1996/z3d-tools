// Global variables definitions
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Z3D_GLOBALS_H__C9E8B261_2F2A_11D5_A644_0000E8EB4C69__INCLUDED_)
#define AFX_Z3D_GLOBALS_H__C9E8B261_2F2A_11D5_A644_0000E8EB4C69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//#include "Z3DContainer.h"
#include "Z3DCacheMgr.h"

#include "H3DContainer.h"
#include "Z3DTypes.h"
#include "Z3DTexturePiece.h"
#include "Z3DAnikeyPack.h"
#include "Z3DTexture.h"
#include "H3DOutfitTable.h"
#include "H3DWeaponTable.h"
#include "Z3DStringTable.h"
#include "Z3DMapTok2FileName.h"


// typedef.s

//typedef CH3DContainer<Z3DLODMesh> CH3DMeshContainer;
typedef CZ3DCacheMgr<Z3DLODMesh, Caldron::Base::CBaseCacheMgr<Z3DLODMesh> > CZ3DMeshContainer;
typedef CH3DContainer<Z3DTexturePiece> CH3DTexPieceContainer;
typedef CH3DContainer<Z3DAniKeyPack> CH3DAniKeyPackContainer;
typedef CH3DContainer<Z3DAMesh> CH3DAMeshContainer;
typedef CZ3DCacheMgr<Z3DTexture, Caldron::Base::CBaseCacheMgr<Z3DTexture> > CZ3DTextureContainer;

//typedef CH3DContainer<Z3DTexture> CH3DTextureContainer;
/*typedef CH3DContainer<CH3DOutfitTable> CH3DOutfitTableContainer;
typedef CH3DContainer<CH3DWeaponTable> CH3DWeaponTableContainer;*/


//typedef H3DContainerTag<Z3DLODMesh> H3DMeshTag;
typedef H3DContainerTag<Z3DTexturePiece> H3DTexPieceTag;
typedef H3DContainerTag<Z3DAniKeyPack> H3DAniKeyPackTag;
typedef H3DContainerTag<Z3DAMesh> H3DAMeshTag;
//typedef H3DContainerTag<Z3DTexture> H3DTextureTag;
/*typedef H3DContainerTag<CH3DOutfitTable> H3DOutfitTableTag;
typedef H3DContainerTag<CH3DWeaponTable> H3DWeaponTableTag;*/

// global var. declarations

//extern CH3DMeshContainer g_ContLODMesh;
extern CZ3DMeshContainer g_ContLODMesh;
extern CH3DTexPieceContainer g_ContTexturePiece;
extern CH3DAniKeyPackContainer g_ContAniKeyPack;
extern CH3DAMeshContainer g_ContAMesh;
//extern CH3DTextureContainer g_ContTexture;
extern CZ3DTextureContainer g_ContTexture;
/*extern CH3DOutfitTableContainer g_ContOutfitTable;
extern CH3DWeaponTableContainer g_ContWeaponTable;*/

// global TOK declarations

//extern CZ3DStringTable g_TokGCMDSName;	// general character model description sheet name
extern CZ3DStringTable g_TokSkeletonPartName;
extern CZ3DStringTable g_TokAttachmentName;
extern CZ3DStringTable g_TokSheetName;
extern CZ3DStringTable g_TokMotionName;
extern CZ3DStringTable g_TokFileName;
extern CZ3DStringTable g_TokSlotName;
extern CZ3DStringTable g_TokOutfitName;
extern CZ3DStringTable g_TokAttachmentName;
extern CZ3DStringTable g_TokEventName;
extern CZ3DStringTable g_TokTemp;


extern CZ3DMapTok2FileName g_MapEvent2SndFile;

//extern DWORD g_dwAMeshVertexShader;

bool _Z3DGlobalInit( const char* szDataRoot, IDirect3DDevice8* pDevice );
void _Z3DGlobalClose( IDirect3DDevice8* pDevice );

#endif // !defined(AFX_Z3D_GLOBALS_H__C9E8B261_2F2A_11D5_A644_0000E8EB4C69__INCLUDED_)
