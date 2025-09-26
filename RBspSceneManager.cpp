// RBspSceneManager.cpp: implementation of the RBspSceneManager class.
//
//////////////////////////////////////////////////////////////////////

#include "RBspSceneManager.h"
#include "SceneManager.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RBspSceneManager::RBspSceneManager()
{
	m_lstBspScene.clear();
	m_iBspSceneNum = 0;
	m_iCurrentSceneIndex = -1;
	m_iPickIndex = -1;

	m_pCurrentBspScene = NULL;
	m_bViewLeafBox = true;

}

RBspSceneManager::~RBspSceneManager()
{
	int i;
	if(m_pCurrentBspScene != NULL)
		m_pCurrentBspScene = NULL;

	if(m_lstBspScene.size() <= 0)
	{
		for(i = 0;i < (int)m_lstBspScene.size(); i++)
		{
			delete m_lstBspScene[i];
			m_lstBspScene[i] = NULL;
		}
	}
	m_lstBspScene.clear();

}
int RBspSceneManager::InsertBseScene(char *strFileName,float fScale)
{
	RBspScene *pNode = NULL;
	pNode = new RBspScene;
	if(pNode->LoadBseScene(strFileName,fScale,fScale,fScale))
	{
		m_lstBspScene.push_back(pNode);
		m_iBspSceneNum++;
		return (m_iBspSceneNum -1);
	}
	else
	{
		delete pNode;
		pNode =  NULL;
	}
	
	return -1;


}
int RBspSceneManager::InsertBspScene(char *strFileName,float fScale)
{
	RBspScene *pNode = NULL;
	pNode = new RBspScene;
	if(pNode->LoadBspScene(strFileName,fScale,fScale,fScale))
	{
		m_lstBspScene.push_back(pNode);
		m_iBspSceneNum++;
		return (m_iBspSceneNum -1);
	}
	else
	{
		delete pNode;
		pNode =  NULL;
	}
	
	return -1;

}
int RBspSceneManager::DeleteBspScene(char *strFileName)
{
	int i;
	for( i = m_iBspSceneNum - 1; i >= 0; i-- )
	{
		if(m_lstBspScene[i] != NULL) 
		{
			if(!strcmp((m_lstBspScene[i]->m_strBspName),strFileName))
			{
				delete m_lstBspScene[i];
				m_lstBspScene[i] = NULL;
				m_lstBspScene.erase(m_lstBspScene.begin() + i);
				m_iBspSceneNum--;
				return i;
			}
		}
		else 
		{
			m_lstBspScene.erase(m_lstBspScene.begin() + i);
			m_iBspSceneNum--;
		}
	}
	return -1;
}
void RBspSceneManager::SetPickIndex(char *strFileName) 
{
	int i;
	for( i = (m_iBspSceneNum -1); i >= 0; i-- )
	{
		if(m_lstBspScene[i] != NULL)
		{
			if(!strcmp((m_lstBspScene[i]->m_strBspName),strFileName))
			{
				m_iPickIndex = i;
				break;
			}
			else
			{
				m_iPickIndex = -1;
			}
		}
		else
		{
			m_lstBspScene.erase(m_lstBspScene.begin() + i);
			m_iBspSceneNum--;
		}
	}
}
RBspScene *RBspSceneManager::SearchBspScene(char *strFileName)
{
	int i;
	for( i = (m_iBspSceneNum -1); i >= 0; i-- )
	{
		if(m_lstBspScene[i] != NULL)
		{
			if(!strcmp((m_lstBspScene[i]->m_strBspName),strFileName))
			{
				//m_pCurrentBspScene = m_lstBspScene[i];
				//m_iCurrentSceneIndex = i;
				return m_lstBspScene[i];

			}
		}
		else
		{
			m_lstBspScene.erase(m_lstBspScene.begin() + i);
			m_iBspSceneNum--;
		}
	}
	return NULL;
}
void RBspSceneManager::BspSceneOut()
{
	m_pCurrentBspScene = NULL;
	m_iCurrentSceneIndex = -1;


}
void RBspSceneManager::SelectBspScene(char *strFileName)
{
	int i;
	for( i = (m_iBspSceneNum -1); i >= 0; i-- )
	{	
		if(m_lstBspScene[i] != NULL) 
		{
			if(!strcmp((m_lstBspScene[i]->m_strBspName),strFileName))
			{
				m_pCurrentBspScene = m_lstBspScene[i];
				m_iCurrentSceneIndex = i;

			}
		}

		else 
		{
			m_lstBspScene.erase(m_lstBspScene.begin() + i);
			m_iBspSceneNum--;
		}
	}

}
void RBspSceneManager::UpdateBsp()
{
	int i;
	for( i = (m_iBspSceneNum -1); i >= 0; i-- )
	{
		if(m_lstBspScene[i] != NULL)
		{
			if( i == m_iCurrentSceneIndex)
			{// 내부에 있는 bsp
				vector3 *vecPPos = CSceneManager::m_ViewCamera->GetPosition();
				D3DXVECTOR3 vecPos = D3DXVECTOR3(vecPPos->x,vecPPos->y,vecPPos->z);
				D3DXMATRIX matTmp;
				D3DXMATRIX matInv;
			
				D3DXMatrixIdentity(&matTmp);
				matTmp._41 = m_lstBspScene[i]->m_vecZeroPos.x;
				matTmp._42 = m_lstBspScene[i]->m_vecZeroPos.y;
				matTmp._43 = m_lstBspScene[i]->m_vecZeroPos.z;
				D3DXMatrixInverse(&matInv,NULL,&matTmp);
				D3DXMatrixIdentity(&matTmp);
				matTmp._41 = vecPos.x;
				matTmp._42 = vecPos.y;
				matTmp._43 = vecPos.z;
				D3DXMatrixMultiply(&matTmp,&matTmp,&matInv);
				vecPos.x = matTmp._41;
				vecPos.y = matTmp._42;
				vecPos.z = matTmp._43;
				
				m_lstBspScene[i]->UpdateScene(vecPos,false);
			}
	
		}
		else 
		{
			m_lstBspScene.erase(m_lstBspScene.begin() + i);
			m_iBspSceneNum--;
		}	
	}
}
void RBspSceneManager::Render()
{
	int i;

	CSceneManager::GetDevice()->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	CSceneManager::GetDevice()->SetRenderState(D3DRS_ZENABLE,TRUE); 
	CSceneManager::GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	CSceneManager::GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	CSceneManager::GetDevice()->SetRenderState( D3DRS_LIGHTING, FALSE );
	CSceneManager::GetDevice()->SetRenderState( D3DRS_SPECULARENABLE , FALSE );
	CSceneManager::GetDevice()->SetRenderState( D3DRS_SHADEMODE , D3DSHADE_GOURAUD );


	for( i = (m_iBspSceneNum -1); i >= 0; i-- )
	{
		if(m_lstBspScene[i] != NULL)
		{
			if( i == m_iCurrentSceneIndex)
			{// 내부에 있는 bsp
				m_lstBspScene[i]->RenderEvent();
				if(m_iPickIndex == i)
					m_lstBspScene[i]->RenderScene(true,true);
				else
					m_lstBspScene[i]->RenderScene(true,false);
				if(m_bViewLeafBox)
					m_lstBspScene[i]->RenderLeafBox();
			}
			else 
			{
				// 외부에서 보는 bsp
				if(m_iPickIndex == i)
					m_lstBspScene[i]->RenderScene(false,true);
				else
					m_lstBspScene[i]->RenderScene(false,false);
			}
		}
		else 
		{
			m_lstBspScene.erase(m_lstBspScene.begin() + i);
			m_iBspSceneNum--;
		}	
	}
}
void RBspSceneManager::ToggleViewLeafBox()
{
	m_bViewLeafBox = (m_bViewLeafBox == true) ? false : true;

}