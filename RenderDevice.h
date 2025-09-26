// RenderDevice.h: interface for the CRenderDevice class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RENDERDEVICE_H__30AB566D_E435_4433_A0C2_7491B2733FB3__INCLUDED_)
#define AFX_RENDERDEVICE_H__30AB566D_E435_4433_A0C2_7491B2733FB3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <d3d8.h>
#include "3DMath.h"

enum RenderState
{

};

class CRenderDevice  
{
	LPDIRECT3DDEVICE8 m_pd3dDevice;
public:
	matrix m_matWorld[4],m_matView,m_matProj;
	LPDIRECT3DDEVICE8 GetDeivce(){return m_pd3dDevice;};
	void Init(LPDIRECT3DDEVICE8 pd3dDevice);
	CRenderDevice();
	virtual ~CRenderDevice();

};

#endif // !defined(AFX_RENDERDEVICE_H__30AB566D_E435_4433_A0C2_7491B2733FB3__INCLUDED_)
