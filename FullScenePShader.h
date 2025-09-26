// FullScenePShader.h: interface for the CFullScenePShader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FULLSCENEPSHADER_H__C0DFAFF9_A1C0_47DF_8E6B_3E2112AFD22E__INCLUDED_)
#define AFX_FULLSCENEPSHADER_H__C0DFAFF9_A1C0_47DF_8E6B_3E2112AFD22E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ShaderScene.h"

class CFullScenePShader : public CPixelShader  
{
public:
	CFullScenePShader(char *);
	virtual ~CFullScenePShader();
	virtual void Apply();
	void Apply(int i);

protected:	
	virtual void SetupPixelShaderConstants(){}
	void SetupPixelShaderConstants(int i);
	char m_strName[256];


};

#endif // !defined(AFX_FULLSCENEPSHADER_H__C0DFAFF9_A1C0_47DF_8E6B_3E2112AFD22E__INCLUDED_)
