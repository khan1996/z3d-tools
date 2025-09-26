// TextureContainer.h: interface for the CTextureContainer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTURECONTAINER_H__54E7CE78_FE22_4D67_8E7C_F04CFF06DE90__INCLUDED_)
#define AFX_TEXTURECONTAINER_H__54E7CE78_FE22_4D67_8E7C_F04CFF06DE90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <d3d8.h>
#include <string>
#include "Exception.h"
#include <vector>

const long TEXTURENAMEBUFFER=256;


class CTextureContainer  
{
	class TextureNode {
	public:
		long m_nUsed;
		char m_strTextureName[TEXTURENAMEBUFFER];
		LPDIRECT3DBASETEXTURE8 m_pddsTexture;		
		TextureNode(const char *strName,const LPDIRECT3DBASETEXTURE8 pddTexture)
		{
			m_nUsed=1;
			strcpy(m_strTextureName,strName);
			m_pddsTexture=pddTexture;
		};
		~TextureNode()
		{				
			if(m_pddsTexture->Release()!=0)
				throw CGraphicLayerError("TextureNode: ~TextureNode ,This texture used other memory,as yet");			
			
		};
	};
	std::vector<TextureNode*> m_TextureList;
public:	
	void DeleteAllTexture();
	void DeleteTexture(char *strTextName);
	LPDIRECT3DBASETEXTURE8 AddUsedTexture(int nUsed);
	int FindTexture(char *strTextName);
	void AddTexture(const char *strTextName, const LPDIRECT3DBASETEXTURE8 pddTexture);
	CTextureContainer();
	virtual ~CTextureContainer();
};

#endif // !defined(AFX_TEXTURECONTAINER_H__54E7CE78_FE22_4D67_8E7C_F04CFF06DE90__INCLUDED_)
