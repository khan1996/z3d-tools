// EffectObject.h: interface for the CEffectObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EFFECTOBJECT_H__89D6C0E9_F6E4_4183_A4B0_03903A691A77__INCLUDED_)
#define AFX_EFFECTOBJECT_H__89D6C0E9_F6E4_4183_A4B0_03903A691A77__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <d3d8.h>
#include "SceneNode.h"

class CEffectObject  : public CSceneNode
{
public:
	virtual void Render(LPDIRECT3DDEVICE8 pd3dDevice)=0;
	virtual void Load(char *strFilename)=0;
	CEffectObject();
	virtual ~CEffectObject();
};

#endif // !defined(AFX_EFFECTOBJECT_H__89D6C0E9_F6E4_4183_A4B0_03903A691A77__INCLUDED_)
