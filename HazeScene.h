// HazeScene.h: interface for the CHazeScene class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HAZESCENE_H__A777792E_43C9_4572_A06A_2A452B741D79__INCLUDED_)
#define AFX_HAZESCENE_H__A777792E_43C9_4572_A06A_2A452B741D79__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <d3d8.h>
#include "3DMath.h"
#include "vertex.h"

class CHazeScene  
{
public:
	DWORD m_dwSampleFilterPixelShader;
	void Create(LPDIRECT3DDEVICE8 pd3dDevice);
	void Render(LPDIRECT3DDEVICE8 pd3dDevice);
	CHazeScene();
	virtual ~CHazeScene();

};

#endif // !defined(AFX_HAZESCENE_H__A777792E_43C9_4572_A06A_2A452B741D79__INCLUDED_)
