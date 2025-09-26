// LayerFogScene.h: interface for the CLayerFogScene class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LAYERFOGSCENE_H__A18D8107_BE6C_4D54_AD08_FD4692286A6C__INCLUDED_)
#define AFX_LAYERFOGSCENE_H__A18D8107_BE6C_4D54_AD08_FD4692286A6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SectorDefine.h"
#include <Texture.h>
#include <3DMath.h>
#include <d3d8.h>


class CLayerFogScene  
{	
public:
	void Render(LPDIRECT3DDEVICE8 pd3dDevice);
	CTexture m_FogTexture;
	LPDIRECT3DVERTEXBUFFER8 m_pFogVertexBuffer;
	void BuildFogTexture(float fMax,float fMin);
	void Create();
	CLayerFogScene();
	virtual ~CLayerFogScene();

};

#endif // !defined(AFX_LAYERFOGSCENE_H__A18D8107_BE6C_4D54_AD08_FD4692286A6C__INCLUDED_)
