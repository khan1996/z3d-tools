// TextureContainer.cpp: implementation of the CTextureContainer class.
//
//////////////////////////////////////////////////////////////////////

#include "TextureContainer.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextureContainer::CTextureContainer()
{

}

CTextureContainer::~CTextureContainer()
{
	DeleteAllTexture();
}

void CTextureContainer::AddTexture(const char *strTextName, const LPDIRECT3DBASETEXTURE8 pddTexture)
{
	TextureNode* pAdd = new TextureNode(strTextName,pddTexture);

	m_TextureList.push_back(pAdd);
}

int CTextureContainer::FindTexture(char *strTextName)
{
	for(int i=0;i<(int)m_TextureList.size();i++)
	{
		if(strcmp(m_TextureList[i]->m_strTextureName,strTextName)==0)
			return i;
	}
	return -1;
}

LPDIRECT3DBASETEXTURE8 CTextureContainer::AddUsedTexture(int nUsed)
{
	m_TextureList[nUsed]->m_nUsed++;
	return m_TextureList[nUsed]->m_pddsTexture;
}

void CTextureContainer::DeleteTexture(char *strTextName)
{
	std::vector<TextureNode*>::iterator obj = m_TextureList.begin();

	for(int i=0;i<(int)m_TextureList.size();i++,obj++)
	{
		if(strcmp(m_TextureList[i]->m_strTextureName,strTextName)==0)
		{
			if(--m_TextureList[i]->m_nUsed==0)
			{
				TextureNode *temp=m_TextureList[i];
			
				if(temp != NULL) 
				{
					delete temp;
					temp = NULL;
				}

				m_TextureList.erase(obj);

				return; 
			}
		}
	}
}

void CTextureContainer::DeleteAllTexture()
{
	for(int i=0;i<(int)m_TextureList.size();i++)
	{
		if(m_TextureList[i] != NULL) 
		{
			delete m_TextureList[i];		
			m_TextureList[i] = NULL;
		}
	}

	m_TextureList.clear();
}
