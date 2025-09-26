// Z3DMultipartPortion.h: interface for the Z3DMultipartPortion class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Z3DMULTIPARTPORTION_H__1B9AE6C4_16F6_11D5_A643_0000E8EB4C69__INCLUDED_)
#define AFX_Z3DMULTIPARTPORTION_H__1B9AE6C4_16F6_11D5_A643_0000E8EB4C69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "misc.h"

#include "Z3DCharacterModel.h"
#include "Z3DTexture.h"
#include "Z3DGradeEffectHandler.h"
#include "Z3DGlowHandler.h"



#include <vector>
#include <map>



#define Z3D_MPID_FACE		10
#define Z3D_MPID_HAIR		11
#define Z3D_MPID_EAR		12
#define Z3D_MPID_SHOULDER	20
#define Z3D_MPID_CHEST		21
#define Z3D_MPID_ARM		23
#define Z3D_MPID_WAIST		24
#define Z3D_MPID_HAND		30
#define Z3D_MPID_LEG		40
#define Z3D_MPID_FOOT		50

#define Z3D_PORTION_BUFFER_BANK_SIZE	500	// 버텍스 버퍼 크기는 500단위로 설정됨

enum Z3D_MULTIPART_PORTION_TYPE {
	Z3D_MPT_TEXPIECE = 0,
	Z3D_MPT_TEXTURE
};


struct Z3DVBCacheNode
{
	long lVertexCount;
	IDirect3DVertexBuffer8* pVB;

	Z3DVBCacheNode* pNext;
};

struct Z3DIBCacheNode
{
	long lIndexCount;
	IDirect3DIndexBuffer8* pIB;

	Z3DIBCacheNode* pNext;
};



struct Z3DMultipartPortion
{
public:

	Z3DMultipartPortion( Z3D_MULTIPART_PORTION_TYPE mpt = Z3D_MPT_TEXPIECE );
	~Z3DMultipartPortion();

	void CheckLoadedStatus( IDirect3DDevice8* pDevice );	// 로딩이 완료되었는지 확인하고 완료되었으면 BuildMesh() 호출

	void BuildMesh( IDirect3DDevice8* pDevice );

	bool SetMesh( const int nPartId, Z3DLODMesh* pMesh );
	bool DeleteMesh( const int nPartId );
	
	bool SetTexPiece( const int nPartId, H3DTexPieceTag tagTexpiece );
	bool SetTexPiece2( const int nPartId, H3DTexPieceTag tagTexpiece );
	bool DeleteTexPiece( const int nPartId );

	bool BatchOpen();
	bool BatchClose( IDirect3DDevice8* pDevice );

	bool SetTexture( const int nPartId, Z3DTexture* pTexture );
	bool SetTexture2( const int nPartId, Z3DTexture* pTexture );
	bool DeleteTexture( const int nPartId );
	bool DeleteTexture2( const int nPartId );

	bool SetGradeEffectIndex( const int nPartId, const int nIndex );
	bool SetGlowIndex( const int nPartId, const int nIndex );

	static IDirect3DVertexBuffer8* _GetVertexBufferInterface( IDirect3DDevice8* pDevice, long& rlVertexCount );
	static void _ReleaseVertexBufferInterface( IDirect3DVertexBuffer8* pVB );

	static IDirect3DIndexBuffer8* _GetIndexBufferInterface( long lLODIndex, IDirect3DDevice8* pDevice, long rlIndexCount );
	static void _ReleaseIndexBufferInterface( long lLODIndex, IDirect3DIndexBuffer8* pIB );
	static void _Close();


public:

	bool m_bNeedLoadedCheck;	// 로딩완료 체크가 필요한지 나타내는 플래그

	Z3DMultipartPortion* m_rpThis;

	long m_lVertexBufferVertexCount;
	long m_alIndexBufferIndexCount[Z3D_LOD_LEVEL];

	std::vector<Z3DLODMesh*> m_vec_pMesh;

//	std::map<int, H3DMeshTag> m_map_IdMeshTag;
	std::map<int, Z3DLODMesh*> m_map_Id2MeshData;
	IDirect3DVertexBuffer8* m_pVertexBuffer;
	long m_lVertexCount;
	
	IDirect3DIndexBuffer8* m_apIndexBuffer[Z3D_LOD_LEVEL];
	long m_alIndexCount[Z3D_LOD_LEVEL];

	std::map<int, H3DTexPieceTag> m_map_IdTexPieceTag;
	std::map<int, H3DTexPieceTag> m_map_IdTexPieceTag2;
	Z3DTexture* m_pTexture;
	Z3DTexture* m_pTexture2;

//	std::map<int, H3DTextureTag> m_map_IdTextureTag;
//	std::map<int, H3DTextureTag> m_map_IdTextureTag2;
	std::map<int, Z3DTexture* > m_map_IdTexture;
	std::map<int, Z3DTexture* > m_map_IdTexture2;
	std::vector<Z3DTexture*> m_vec_pTexture;
	std::vector<Z3DTexture*> m_vec_pTexture2;


	std::map< int, int > m_map_Id2GradeEffectIndex;
	std::vector<CZ3DGradeEffectHandler*> m_vec_pGradeEffectHandler;

	std::map< int, int > m_map_Id2GlowIndex;
	std::vector<CZ3DGlowHandler*> m_vec_pGlowHandler;

	

	Z3D_MULTIPART_PORTION_TYPE m_MPT;


	// vertex buffer 캐시 리스트. SLL 이며 추가는 header, 삭제(오버플로시)는 tail 에서 일어남
	// 검색은 header부터 순차적으로. strategy 는 first match
	static const long ms_lMaxVBCacheCount;
	static long ms_lVBCacheCount;
	static Z3DVBCacheNode* ms_pVBCacheChainHeader;

	// index buffer 캐시 리스트. SLL 이며 추가는 header, 삭제(오버플로시)는 tail 에서 일어남
	// 검색은 header부터 순차적으로. strategy 는 first match
	// LOD 레벨별로 따로 관리함. (배열 인덱스가 해당 LOD level의 데이터)
	static const long ms_alMaxIBCacheCount[Z3D_LOD_LEVEL];
	static long ms_alIBCacheCount[Z3D_LOD_LEVEL];
	static Z3DIBCacheNode* ms_apIBCacheChainHeader[Z3D_LOD_LEVEL];
};

#endif // !defined(AFX_Z3DMULTIPARTPORTION_H__1B9AE6C4_16F6_11D5_A643_0000E8EB4C69__INCLUDED_)
