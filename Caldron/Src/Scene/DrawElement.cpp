#include "DrawElement.h"
#include <algorithm>


namespace Caldron
{
namespace Scene
{
	IDirect3DDevice8* CDrawElementMgr::ms_pDevice;

	std::vector< DRAW_ELEMENT > CDrawElementMgr::ms_avecDynamicDrawElement[MAX_DRAW_ELEMENT_LAYER];
//	std::vector< DRAW_ELEMENT > CDrawElementMgr::ms_avecStaticDrawElement[MAX_DRAW_ELEMENT_LAYER];

	int CDrawElementMgr::ms_nDynamicVertexBufferCount;
	DWORD CDrawElementMgr::ms_dwDynamicVertexBufferSize;

	int CDrawElementMgr::ms_nDynamicIndexBufferCount;
	DWORD CDrawElementMgr::ms_dwDynamicIndexBufferSize;

//	int CDrawElementMgr::ms_nStaticVertexBufferCount;
//	DWORD CDrawElementMgr::ms_dwStaticVertexBufferSize;

//	int CDrawElementMgr::ms_nStaticIndexBufferCount;
//	DWORD CDrawElementMgr::ms_dwStaticIndexBufferSize;

//	IDirect3DVertexBuffer8** CDrawElementMgr::ms_apDynamicVertexBuffer;
//	IDirect3DIndexBuffer8** CDrawElementMgr::ms_apDynaicIndexBuffer;
	IDirect3DVertexBuffer8* CDrawElementMgr::ms_pDynamicVertexBuffer;
	IDirect3DIndexBuffer8* CDrawElementMgr::ms_pDynamicIndexBuffer;

	std::map< CBaseMesh*, UINT > CDrawElementMgr::ms_mapVBAllocationInfo;
	std::map< CBaseMesh*, UINT > CDrawElementMgr::ms_mapIBAllocationInfo;
	int CDrawElementMgr::ms_nVBUsedInByte;
	int CDrawElementMgr::ms_nIBUsedInByte;


//	IDirect3DVertexBuffer8** CDrawElementMgr::ms_apStaticVertexBuffer;
//	IDirect3DIndexBuffer8** CDrawElementMgr::ms_apStaticIndexBuffer;

	DRAW_ELEMENT CDrawElementMgr::ms_LastTimeDE;


	bool CDrawElementMgr::_Init( IDirect3DDevice8* pDevice, DRAW_ELEMENT_MGR_INIT_PARAM* pInitParam )
	{
		ms_pDevice = pDevice;

		HRESULT hr;

		// dynamic VB �Ҵ�
		hr = pDevice->CreateVertexBuffer( pInitParam->dwDynamicVertexBufferSize, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
			0, D3DPOOL_DEFAULT, &ms_pDynamicVertexBuffer );
		if( FAILED(hr) )
		{
			return false;
		}

		// dynamic IB �Ҵ�
		hr = pDevice->CreateIndexBuffer( pInitParam->dwDynamicIndexBufferSize, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX16, D3DPOOL_DEFAULT, &ms_pDynamicIndexBuffer );
		if( FAILED(hr) )
		{
			return false;
		}

		// static VB �Ҵ�
		// �������

		// static IB �Ҵ�
		// �������


		ms_nDynamicVertexBufferCount = 1;//pInitParam->nDynamicVertexBufferCount;
		ms_dwDynamicVertexBufferSize = pInitParam->dwDynamicVertexBufferSize;

		ms_nDynamicIndexBufferCount = 1;//pInitParam->nDynamicIndexBufferCount;
		ms_dwDynamicIndexBufferSize = pInitParam->dwDynamicIndexBufferSize;


//		ms_nStaticVertexBufferCount = pInitParam->nStaticVertexBufferCount;
//		ms_dwStaticVertexBufferSize = pInitParam->dwStaticVertexBufferSize;

//		ms_nStaticIndexBufferCount = pInitParam->nStaticIndexBufferCount;
//		ms_dwStaticIndexBufferSize = pInitParam->dwStaticIndexBufferSize;


		ms_LastTimeDE.pMesh = NULL;
		ms_LastTimeDE.pTM = NULL;
		ms_LastTimeDE.pTextureTM = NULL;
		ms_LastTimeDE.textureSlot.apTexture[0] = NULL;
		ms_LastTimeDE.textureSlot.nUsed = 1;
		ms_LastTimeDE.hVertexShader = NULL;
		ms_LastTimeDE.hPixelShader = NULL;
		ms_LastTimeDE.pStateSet = NULL;


		return true;
	}


	bool CDrawElementMgr::_RegisterDrawElement( int nLayer, DRAW_ELEMENT& rDE )
	{
		if( nLayer >= MAX_DRAW_ELEMENT_LAYER )
		{
			return false;
		}

		if( false == rDE.ValidityCheck() )
		{
			return false;
		}

		// D/E �� ��ҵ鿡 ���� lock �� �ʿ��ҵ�.. �׷��� �̷��� �Ϸ��� �� ��ҵ� ��� lockable �� ��ü�� �ٲ�����Ѵ�.
		rDE.pMesh->Lock();

		ms_avecDynamicDrawElement[nLayer].push_back( rDE );

		return true;
	}


//	bool CDrawElementMgr::_RegisterStaticDrawElement( int nLayer, DRAW_ELEMENT& rDE )
//	{
//		//
//		// ����� dynamic �϶��� ������ ������ ���� �ٲ� �� ����
//		//
//
//		if( nLayer >= MAX_DRAW_ELEMENT_LAYER )
//		{
//			return false;
//		}
//
//		if( false ==rDE.ValidityCheck() )
//		{
//			return false;
//		}
//
//		// D/E �� ��ҵ鿡 ���� lock �� �ʿ��ҵ�.. �׷��� �̷��� �Ϸ��� �� ��ҵ� ��� lockable �� ��ü�� �ٲ�����Ѵ�.
//
//		ms_avecDynamicDrawElement[nLayer].push_back( rDE );
//
//		return true;
//	}

	void CDrawElementMgr::_Process()
	{
		VIB_PACKET packet;

		// sort
		_Sort();

		// optimize
		_Optimize();



		for( int i = 0; i < MAX_DRAW_ELEMENT_LAYER; ++i )
		{
			// per-layer setting ( ex) Render Target Change )
			// render target, view matrix, projection matrix ���� ����



			std::vector< DRAW_ELEMENT > & rvecDE = ms_avecDynamicDrawElement[i];	// shortcut to i-th layer of D/E vector

			for( int j = 0; j < rvecDE.size(); ++j )
			{
				DRAW_ELEMENT &rCurrentDrawElement = rvecDE[j];

				// mesh data uploading (to VB/IB )
				_UploadMesh( rCurrentDrawElement, packet );

				// draw primitive(include state setting)
				_Render( rCurrentDrawElement, packet );

				// unlock D/E related components
				// ������ CMesh �� Lock/Unlock �����Ǿ�����
				rCurrentDrawElement.pMesh->Unlock();
			}

			// flush D/E list 'cause this is DYNAMIC D/E list
            rvecDE.clear();
		}

		ms_pDevice->SetStreamSource( 0, NULL, 0 );
		ms_pDevice->SetIndices( NULL, 0 );
	}

	void CDrawElementMgr::_Close()
	{
		// VB/IB ����
		SafeRelease( ms_pDynamicVertexBuffer );
		SafeRelease( ms_pDynamicIndexBuffer );
	}

	// D/E sort comparison processor
	bool DE_Ascend( DRAW_ELEMENT rDE1, DRAW_ELEMENT rDE2 )
	{
		if( rDE1.textureSlot.apTexture[0] > rDE2.textureSlot.apTexture[0] ) return false;
		if( rDE1.pTM > rDE2.pTM ) return false;
		if( rDE1.hVertexShader > rDE2.hVertexShader ) return false;
		if( rDE1.hPixelShader > rDE2.hPixelShader ) return false;
		if( rDE1.pStateSet > rDE2.pStateSet ) return false;
		if( rDE1.pMesh > rDE2.pMesh ) return false;

		return true;
	}

	void CDrawElementMgr::_Sort()
	{
		for( int i = 0; i < MAX_DRAW_ELEMENT_LAYER; ++i )
		{
			std::stable_sort( ms_avecDynamicDrawElement[i].begin(), ms_avecDynamicDrawElement[i].end(), DE_Ascend );
		}

	}


	void CDrawElementMgr::_Optimize()
	{
		// nothing now..
	}


	void CDrawElementMgr::_UploadMesh( DRAW_ELEMENT &rDE, VIB_PACKET &rPacket )
	{
		std::map< CBaseMesh*, UINT >::iterator it;
		BYTE* pByteDataInBuffer = NULL;
		UINT uSizeInByte;	// �Ҵ�� mesh data �� byte ������
		UINT uAllocOffset;	// buffer �� upload �� ��ġ
		UINT uRemain;	// buffer �� �����ִ� ����
		HRESULT hr;

		// VB �� �̹� upload �Ǿ��ִ� mesh �ΰ�?
		it = ms_mapVBAllocationInfo.find( rDE.pMesh );
		if( it != ms_mapVBAllocationInfo.end() )
		{
			// �׷��ٸ� ���� ������ VIB packet �� ���
			rPacket.uPositionInVB = it->second;
		}
		else
		{
			// �ƴϸ� �Ҵ� ����
			
			//////////////////////////////////////////////////////////////////////////
			//
			// DrawPrimitive �迭�� offset ���� �Ķ���͵��� vertex ���� �����̹Ƿ�
			// mesh vertex data�� upload �� ��� �� ��ġ�� vertex size�� �°� align
			// �� �ش�.
			//

			uSizeInByte = rDE.pMesh->GetVertexCount() * rDE.pMesh->GetVertexSize();

			UINT uUsedAlignFixed = (ms_nVBUsedInByte % rDE.pMesh->GetVertexSize() ) ? 
										( ( ms_nVBUsedInByte/rDE.pMesh->GetVertexSize() ) + 1 ) * rDE.pMesh->GetVertexSize() :
										ms_nVBUsedInByte; // vertex size ��迡 ���� �ø�
			uRemain = ms_dwDynamicVertexBufferSize - uUsedAlignFixed;

			// Lock VB
			if( uRemain < uSizeInByte )
			{
				// ������ �����Ҷ� -> discard all & reset alloc info
				uAllocOffset = 0;
				hr = ms_pDynamicVertexBuffer->Lock( uAllocOffset, uSizeInByte, &pByteDataInBuffer, D3DLOCK_DISCARD );
				if( FAILED(hr) )
				{
					return;	// Lock ���н� �� ����..-_-
				}

				// VB flush, �Ҵ����� clear
				ms_mapVBAllocationInfo.clear();
				ms_nVBUsedInByte = 0;
			}
			else
			{
				// ������ ����Ҷ� -> �� Lock
				uAllocOffset = uUsedAlignFixed;
				hr = ms_pDynamicVertexBuffer->Lock( uAllocOffset, uSizeInByte, &pByteDataInBuffer, D3DLOCK_NOOVERWRITE );
				if( FAILED(hr) )
				{
					return;	// Lock ���н� �� ����..-_-
				}
			}
			
			// upload, �Ҵ����� ����
			rDE.pMesh->UploadVertexData( pByteDataInBuffer );
			ms_pDynamicVertexBuffer->Unlock();

            ms_mapVBAllocationInfo[rDE.pMesh] = uAllocOffset;
			ms_nVBUsedInByte = uUsedAlignFixed + uSizeInByte;
		}

		// indexed mesh �� �ƴϸ� -> ����
		if( !( rDE.pMesh->IsIndexed() ) )
		{
			return;
		}

		// IB ���� VB �� ���� ���� �ݺ�
		
		// IB �� �̹� upload �Ǿ��ִ� mesh �ΰ�?
		it = ms_mapIBAllocationInfo.find( rDE.pMesh );
		if( it != ms_mapIBAllocationInfo.end() )
		{
			// �׷��ٸ� ���� ������ VIB packet �� ���
			rPacket.uPositionInIB = it->second;
		}
		else
		{
			// �ƴϸ� �Ҵ� ����

			uSizeInByte = rDE.pMesh->GetIndexCount() * sizeof(WORD);
			uRemain = ms_dwDynamicIndexBufferSize - ms_nIBUsedInByte;

			// Lock IB
			if( uRemain < uSizeInByte )
			{
				// ������ �����Ҷ� -> discard all & reset alloc info
				uAllocOffset = 0;
				hr = ms_pDynamicIndexBuffer->Lock( uAllocOffset, uSizeInByte, &pByteDataInBuffer, D3DLOCK_DISCARD );
				if( FAILED(hr) )
				{
					return;	// Lock ���н� �� ����..-_-
				}

				// IB flush, �Ҵ����� clear
				ms_mapIBAllocationInfo.clear();
				ms_nIBUsedInByte = 0;
			}
			else
			{
				// ������ ����Ҷ� -> �� Lock
				uAllocOffset = ms_nIBUsedInByte;
				hr = ms_pDynamicIndexBuffer->Lock( uAllocOffset, uSizeInByte, &pByteDataInBuffer, D3DLOCK_NOOVERWRITE );
				if( FAILED(hr) )
				{
					return;	// Lock ���н� �� ����..-_-
				}
			}

			// upload, �Ҵ����� ����
			rDE.pMesh->UploadIndexData( pByteDataInBuffer );
			ms_pDynamicIndexBuffer->Unlock();

			ms_mapIBAllocationInfo[rDE.pMesh] = uAllocOffset;
			ms_nIBUsedInByte += uSizeInByte;
		}
	}


	void CDrawElementMgr::_Render( DRAW_ELEMENT &rDE, VIB_PACKET &rPacket )
	{
// �ӽ��ڵ�
ms_pDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
ms_pDevice->LightEnable( 0, TRUE );


		// ms_LastTimeDE �� rDE �� ������Ʈ�� �� �� �ٸ� ������Ʈ�� ����

		// rPacket �� VB/IB �� last time �� �ٸ� ��� stream source �� �ٲ����..������
		// ����� VB/IB �� �ϳ����̹Ƿ� ����
		// ...�Ϸ��� ������, SSS�� stride �� �Ķ���ͷ� ���Ƿ� �Ź� ���ִ°����� ����-_-;
		ms_pDevice->SetStreamSource( 0, ms_pDynamicVertexBuffer, rDE.pMesh->GetVertexSize() );

		ms_pDevice->SetTransform( D3DTS_WORLD, rDE.pTM );

		if( NULL == rDE.hVertexShader )
		{
			DWORD dwFVF = rDE.pMesh->GetFVF();
			ms_pDevice->SetVertexShader( dwFVF );
		}
		else
		{
			ms_pDevice->SetVertexShader( rDE.hVertexShader );
		}
		ms_pDevice->SetPixelShader( rDE.hPixelShader );

		if( NULL == rDE.textureSlot.apTexture[0] )
		{
			ms_pDevice->SetTexture( 0, NULL );			
		}
		else
		{
			ms_pDevice->SetTexture( 0, rDE.textureSlot.apTexture[0]->GetTexture() );
		}


		if( rDE.pMesh->IsIndexed() )
		{
			ms_pDevice->SetIndices( ms_pDynamicIndexBuffer, rPacket.uPositionInVB / rDE.pMesh->GetVertexSize() );

			ms_pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, rDE.pMesh->GetVertexCount(),
				rPacket.uPositionInIB / sizeof(WORD), rDE.pMesh->GetIndexCount() / 3 );
		}
		else
		{
			ms_pDevice->DrawPrimitive(  D3DPT_TRIANGLELIST, rPacket.uPositionInVB / rDE.pMesh->GetVertexSize(),
				rDE.pMesh->GetVertexCount()/3 );
		}

		// last time ������ ������Ʈ
		ms_LastTimeDE = rDE;
	}




}
}