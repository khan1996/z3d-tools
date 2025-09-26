// QShader.h: interface for the CQShader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QSHADER_H__B873FFA4_D143_46EF_B114_6A02DBF23A06__INCLUDED_)
#define AFX_QSHADER_H__B873FFA4_D143_46EF_B114_6A02DBF23A06__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Texture.h"

class CQShader  
{
	CTexture *m_pTexture;
	char m_strTextureName[256];
public:
	void LoadTexture();
	void SetTextureName(char *strTextureName)
	{
		strcpy(m_strTextureName,strTextureName);
	};
	char *GetTextureName()
	{
		return m_strTextureName;
	};
	CTexture *GetTexture()
	{
		return m_pTexture;
	};
	CQShader();
	virtual ~CQShader();
};

#endif // !defined(AFX_QSHADER_H__B873FFA4_D143_46EF_B114_6A02DBF23A06__INCLUDED_)
