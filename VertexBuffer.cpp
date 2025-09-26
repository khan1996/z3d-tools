// VertexBuffer.cpp: implementation of the CVertexBuffer class.
//
//////////////////////////////////////////////////////////////////////

#include "VertexBuffer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LPDIRECT3D8 CVertexBuffer::m_pD3D;


CVertexBuffer::CVertexBuffer()
{
	m_pVertexBuffer=NULL;
}

CVertexBuffer::~CVertexBuffer()
{
	if(m_pVertexBuffer->Release()!=0)
		throw CGraphicLayerError("CVertexBuffer:~CVertexBuffer, This vertex-buffer is used other Object , as yet");
}

void CVertexBuffer::Create()
{
	/*
	m_pd3dDevice->CreateVertexBuffer( m_dwNumDolphinVertices * sizeof(D3DVERTEX),
                                      D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED,
                                      &m_pDolphinVB1 );
	if(m_pVertexBuffer)
	{
		if(FAEILED(m_pd3dDevice->CreateVertexBuffer(m_nVertex*sizeof(),
										 D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED,
										 &m_pVertexBuffer)))
		{
			throw CGraphicLayerError("CVertexBuffer:Create, Vertex-Buffer Create Failed");
		}        
    }
	*/
}

void CVertexBuffer::Render(LPDIRECT3DDEVICE8 pd3dDevice,WORD *pIndices,long &nIndices)
{	
	/*
    m_pd3dDevice->SetVertexShader( m_dwDolphinVertexShader );
    m_pd3dDevice->SetStreamSource( 0, m_pDolphinVB1, sizeof(D3DVERTEX) );
    m_pd3dDevice->SetStreamSource( 1, m_pDolphinVB2, sizeof(D3DVERTEX) );
    m_pd3dDevice->SetStreamSource( 2, m_pDolphinVB3, sizeof(D3DVERTEX) );
    m_pd3dDevice->SetIndices( m_pDolphinIB, 0 );
    m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
                                        0, m_dwNumDolphinVertices,
                                        0, m_dwNumDolphinFaces );


	pd3dDevice->DrawIndexedPrimitiveVB( D3DPT_TRIANGLELIST, 
                                              m_pVertexBuffer, 0,
                                              m_nVertex, pIndices,
                                              nIndices*3, D3DDP_WAIT );
	/*
	HRESULT hr=
	pd3dDevice->DrawIndexedPrimitiveVB( D3DPT_TRIANGLELIST, 
                                              m_pVertexBuffer, 0,
                                              m_nVertex, pIndices,
                                              0, D3DDP_WAIT );
											  */
	
}
