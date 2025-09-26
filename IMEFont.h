// IMEFont.h: interface for the CIMEFont class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMEFONT_H__C8D7C988_BD4A_4270_AE9D_829679CEB5F4__INCLUDED_)
#define AFX_IMEFONT_H__C8D7C988_BD4A_4270_AE9D_829679CEB5F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <d3d8.h>
#include <d3dx8.h>
#include "Vertex.h"

class CIMEFont  
{
	TLVertex m_pVertex[4];
	char m_strPrint[20][256];
	char m_strPrinted[20][256];
	int m_cLine;
public:	
	void Render(LPDIRECT3DDEVICE8 pd3dDevice,DWORD dwColor);
	void MakeTexture();
	bool isRewirte();
	void PrintToTexture(char *str,long line);	
	void Create(LPDIRECT3DDEVICE8 pd3dDevice,long sizeX,long sizeY);
	void Render(LPDIRECT3DDEVICE8 pd3dDevice);
	LPDIRECT3DTEXTURE8 m_pTexture;
	CIMEFont();
	virtual ~CIMEFont();	
	long m_RenderX,m_RenderY;
	long m_SizeX,m_SizeY;
	void SetRenderPos(int RenderX,int RenderY)
	{
		m_RenderX=RenderX;
		m_RenderY=RenderY;
	};
	char* GetPrintText(int nText)
	{
		return m_strPrint[nText];
	};
};

#endif // !defined(AFX_IMEFONT_H__C8D7C988_BD4A_4270_AE9D_829679CEB5F4__INCLUDED_)
