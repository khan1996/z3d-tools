#pragma once

#pragma warning( disable : 4786 )
#include "../../include/Caldron.h"

#include "Mesh.h"
//#include "Texture.h"
#include "NTexture.h"
#include "StateSet.h"

#include <vector>


//////////////////////////////////////////////////////////////////////////
//
// 1�������� dynamic buffer ��, VB/IB �� 1�������θ� ����
//

namespace Caldron
{
namespace Scene
{
	const int MAX_DRAW_ELEMENT_LAYER = 5;
	const int MAX_TEXTURE_SLOT = 8;

	struct TEXTURE_SLOT
	{
		//CTexture* apTexture[MAX_TEXTURE_SLOT];
		CROSSM::CNTexture* apTexture[MAX_TEXTURE_SLOT];
        int nUsed;


		TEXTURE_SLOT()
		{
			apTexture[0] = NULL;
			nUsed = 1;
		}
/*
		TEXTURE_SLOT( int n, CTexture pt0, CTexture pt1 = NULL, CTexture pt2 = NULL, CTexture pt3 = NULL )
		{
			apTexture[0] = pt0;
			apTexture[1] = pt1;
			apTexture[2] = pt2;
			apTexture[3] = pt3;

			nUsed = n;
		}
*/
	};


	struct DRAW_ELEMENT
	{
		CBaseMesh* pMesh;
        D3DMATRIX* pTM;
		D3DMATRIX* pTextureTM;
		TEXTURE_SLOT textureSlot;
		DWORD hVertexShader;
		DWORD hPixelShader;
		CStateSet* pStateSet;



		// VB/IB �� upload �� ��ġ ����� �����ͺκ� �ʿ�
		DRAW_ELEMENT() {}


		DRAW_ELEMENT( CBaseMesh* _pMesh, D3DMATRIX* _pTM, D3DMATRIX* _pTextureTM, TEXTURE_SLOT* _pTextureSlot, DWORD _hVertexShader, DWORD _hPixelShader, CStateSet* _pStateSet )
		{
			pMesh = _pMesh;
			pTM = _pTM;
			pTextureTM = _pTextureTM;
			textureSlot = *_pTextureSlot;
			hVertexShader = _hVertexShader;
			hPixelShader = _hPixelShader  ;
			pStateSet = _pStateSet;
		}

		bool ValidityCheck()
		{
			if( NULL == pMesh ) return false;
			//if( NULL == pTextureSlot ) return false;
			// VS, PS ��� null �� ��� ����
            if( NULL == pStateSet ) return false;

			return true;
		}
	};


	struct DRAW_ELEMENT_MGR_INIT_PARAM
	{
		int nDynamicVertexBufferCount;
		DWORD dwDynamicVertexBufferSize;

		int nDynamicIndexBufferCount;
		DWORD dwDynamicIndexBufferSize;

		int nStaticVertexBufferCount;
		DWORD dwStaticVertexBufferSize;

		int nStaticIndexBufferCount;
		DWORD dwStaticIndexBufferSize;
	};

	struct VIB_PACKET
	{
		//IDirect3DVertexBuffer8* pVB;
		//IDirect3DIndexBuffer8* pIB;
		UINT uPositionInVB;
		UINT uPositionInIB;

		VIB_PACKET()
		{
			uPositionInVB = 0;
			uPositionInIB = 0;
		}
	};


	class CDrawElementMgr
	{
	public:
		// �ʱ�ȭ( InitDevice �� �ʱ�ȭ)
		static bool _Init( IDirect3DDevice8* pDevice, DRAW_ELEMENT_MGR_INIT_PARAM* pInitParam );

		// Draw Element ���
		static bool _RegisterDrawElement( int nLayer, DRAW_ELEMENT& rDE );

		// Static Draw Element ���
//		static bool _RegisterStaticDrawElement( int nLayer, DRAW_ELEMENT& rDE );

		// register �� D/E ���� sort �ϰ� ����ȭ�� �� DrawPrimitive ȣ��
		static void _Process();

		// ����( _Init �� pair )
		static void _Close();

	private:
		static IDirect3DDevice8* ms_pDevice;

		static std::vector< DRAW_ELEMENT > ms_avecDynamicDrawElement[MAX_DRAW_ELEMENT_LAYER];
//		static std::vector< DRAW_ELEMENT > ms_avecStaticDrawElement[MAX_DRAW_ELEMENT_LAYER];

		static int ms_nDynamicVertexBufferCount;
		static DWORD ms_dwDynamicVertexBufferSize;

		static int ms_nDynamicIndexBufferCount;
		static DWORD ms_dwDynamicIndexBufferSize;


//		static int ms_nStaticVertexBufferCount;
//		static DWORD ms_dwStaticVertexBufferSize;

//		static int ms_nStaticIndexBufferCount;
//		static DWORD ms_dwStaticIndexBufferSize;

//		static IDirect3DVertexBuffer8** ms_apDynamicVertexBuffer;
//		static IDirect3DIndexBuffer8** ms_apDynaicIndexBuffer;
		static IDirect3DVertexBuffer8* ms_pDynamicVertexBuffer;
		static IDirect3DIndexBuffer8* ms_pDynamicIndexBuffer;

//		static IDirect3DVertexBuffer8** ms_apStaticVertexBuffer;
//		static IDirect3DIndexBuffer8** ms_apStaticIndexBuffer;


		static std::map< CBaseMesh*, UINT > ms_mapVBAllocationInfo;
		static std::map< CBaseMesh*, UINT > ms_mapIBAllocationInfo;
		static int ms_nVBUsedInByte;
		static int ms_nIBUsedInByte;

		static DRAW_ELEMENT ms_LastTimeDE;


		// Process ���� ���� ������Ʈ��
		static void _Sort();
		static void _Optimize();
		static void _UploadMesh( DRAW_ELEMENT &rDE, VIB_PACKET &rPacket );
		static void _Render( DRAW_ELEMENT &rDE, VIB_PACKET &rPacket );

	};



}
}