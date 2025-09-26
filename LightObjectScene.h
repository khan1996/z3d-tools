// LightObjectScene.h: interface for the CLightObjectScene class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTOBJECTSCENE_H__13B65361_54E6_433B_AA8A_CCB1D4B068C2__INCLUDED_)
#define AFX_LIGHTOBJECTSCENE_H__13B65361_54E6_433B_AA8A_CCB1D4B068C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SceneNode.h"
#include "LightObject.h"


class CLightObjectScene : public CSceneNode 
{
public:
	long m_LightID;
	CLightObject *m_pLightObject;
	CLightObjectScene();
	virtual ~CLightObjectScene();

};

#endif // !defined(AFX_LIGHTOBJECTSCENE_H__13B65361_54E6_433B_AA8A_CCB1D4B068C2__INCLUDED_)
