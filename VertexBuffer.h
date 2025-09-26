// VertexBuffer.h: interface for the CVertexBuffer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VERTEXBUFFER_H__17331CD7_3932_4E36_8DE5_FB7F04C996AA__INCLUDED_)
#define AFX_VERTEXBUFFER_H__17331CD7_3932_4E36_8DE5_FB7F04C996AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <d3d8.h>
#include "Exception.h"


class CVertexBuffer  
{		
	static LPDIRECT3D8 m_pD3D;
public:
	void Render(LPDIRECT3DDEVICE8 pd3dDevice,WORD *pIndices,long &nIndices);
	DWORD m_nVertex;
	LPDIRECT3DVERTEXBUFFER8 m_pVertexBuffer;
	DWORD m_FVF;
	void Create();
	CVertexBuffer();
	virtual ~CVertexBuffer();
};

#endif // !defined(AFX_VERTEXBUFFER_H__17331CD7_3932_4E36_8DE5_FB7F04C996AA__INCLUDED_)
