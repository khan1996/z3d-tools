// ObjectScene.cpp: implementation of the CObjectScene class.
//
//////////////////////////////////////////////////////////////////////

#include "ObjectScene.h"
#include "SceneManager.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CObjectScene::CObjectScene()
{

	m_pObject = NULL;
	m_bInit = false;
	

}

CObjectScene::~CObjectScene()
{
	if(m_pObject)
	{
		CSceneManager::_ReleaseResource(CROSSM::RESOURCE_MESH,m_pObject);
		m_pObject = NULL;
	}


}
