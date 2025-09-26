// SectorWideMap.cpp: implementation of the CSectorWideMap class.
//
//////////////////////////////////////////////////////////////////////

#include "SectorWideMap.h"
#include "BaseDataDefine.h"
#include "SceneManager.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSectorWideMap::CSectorWideMap()
{
	for(int i=0;i<3;i++)
	{
		strcpy(m_strWidemapName[i],"");
		strcpy(m_strDetailName[i],"");
		m_WideMapTexture[i] = NULL;
		m_DetailTexture[i] = NULL;
	}
}

CSectorWideMap::~CSectorWideMap()
{
	if(CSceneManager::m_pResource)
	{
		for(int i=0;i<3;i++)
		{
	/*		if(m_WideMapTexture[i].GetTexture())
				m_WideMapTexture[i].DeleteTexture();
			if(m_DetailTexture[i].GetTexture())
				m_DetailTexture[i].DeleteTexture();*/
			if(m_WideMapTexture[i])
			{
				CSceneManager::_ReleaseResource(CROSSM::RESOURCE_TEXTURE,m_WideMapTexture[i]);
				m_WideMapTexture[i] = NULL;
			}
			if(m_DetailTexture[i])
			{	
				CSceneManager::_ReleaseResource(CROSSM::RESOURCE_TEXTURE,m_DetailTexture[i]);
				m_DetailTexture[i] = NULL;

			}

		}
	}
}
void CSectorWideMap::DelWideTexture()
{
/*	for(int i=0;i<3;i++)
	{
		if(m_WideMapTexture[i].GetTexture())
			m_WideMapTexture[i].DeleteTexture();
		if(m_DetailTexture[i].GetTexture())
			m_DetailTexture[i].DeleteTexture();
	}*/
	//m_WideMapTexture.DeleteTexture();
	for(int i = 0; i  < 3; i++ ) 
	{
		if(m_WideMapTexture[i])
		{
			CSceneManager::_ReleaseResource(CROSSM::RESOURCE_TEXTURE,m_WideMapTexture[i]);
			m_WideMapTexture[i] = NULL;
		}
		if(m_DetailTexture[i])
		{	
			CSceneManager::_ReleaseResource(CROSSM::RESOURCE_TEXTURE,m_DetailTexture[i]);
			m_DetailTexture[i] = NULL;

		}
	}

}

void CSectorWideMap::GetWideTexture()
{	
	CROSSM::CNTexture::SetPath(WIDETEXTUREPATH);

	m_nDetail=0;

	for(int i=0;i<3;i++)
	{
		if(m_WideMapTexture[i])
		{
			CSceneManager::_ReleaseResource(CROSSM::RESOURCE_TEXTURE,m_WideMapTexture[i]);
			m_WideMapTexture[i] = NULL;
		}
		if(m_DetailTexture[i])
		{	
			CSceneManager::_ReleaseResource(CROSSM::RESOURCE_TEXTURE,m_DetailTexture[i]);
			m_DetailTexture[i] = NULL;

		}
		if(strcmp(m_strDetailName[i],"")!=0)
		{
			//m_WideMapTexture[i].Load(m_strWidemapName[i]);			
			//m_DetailTexture[i].Load(m_strDetailName[i]);
			m_WideMapTexture[i] = (CROSSM::CNTexture *)CSceneManager::_GetResource(CROSSM::RESOURCE_TEXTURE,m_strWidemapName[i]);
			m_DetailTexture[i] = (CROSSM::CNTexture *)CSceneManager::_GetResource(CROSSM::RESOURCE_TEXTURE,m_strDetailName[i]);
			m_nDetail++;
		}
		else
			break;
	}		
}
