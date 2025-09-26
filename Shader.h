// Shader.h: interface for the CShader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHADER_H__E786457B_3C89_4409_BE23_BEED910CD5D3__INCLUDED_)
#define AFX_SHADER_H__E786457B_3C89_4409_BE23_BEED910CD5D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "Texture.h"

class CShader  
{
	CTexture *m_pTexture;
	char m_strTextureName[256];
public:
	void SetTextureName(char *strTextureName)
	{
		strcpy(m_strTextureName,strTextureName);
	};
	char *GetTextureName()
	{
		return m_strTextureName;
	};
	CShader();
	virtual ~CShader();
};

#endif // !defined(AFX_SHADER_H__E786457B_3C89_4409_BE23_BEED910CD5D3__INCLUDED_)
