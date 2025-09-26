// FullSceneShader.h: interface for the CFullSceneShader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FULLSCENESHADER_H__FD09FAF4_5266_40CD_88BA_A291AFDC848F__INCLUDED_)
#define AFX_FULLSCENESHADER_H__FD09FAF4_5266_40CD_88BA_A291AFDC848F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ShaderScene.h"

class CFullScenePShader;
class CFullSceneShader : public CShaderScene  
{
public:
	CFullSceneShader(char *strFileName);
	virtual ~CFullSceneShader();
	virtual void Apply();
	void Apply(int i);
	virtual void UnApply();
protected:
	CFullScenePShader	*m_pPixelShader;



};

#endif // !defined(AFX_FULLSCENESHADER_H__FD09FAF4_5266_40CD_88BA_A291AFDC848F__INCLUDED_)
