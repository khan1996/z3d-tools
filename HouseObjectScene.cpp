// HouseObjectNode.cpp: implementation of the CHouseObjectNode class.
//
//////////////////////////////////////////////////////////////////////

#include "HouseObjectScene.h"
#include "SceneManager.h"
#include "ImposterScene.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*
bool m_SelfLoadOut,m_SelfLoadMed,m_SelfLoadIn,m_SelfLoadInObject;
	void Render(LPDIRECT3DDEVICE8 pd3dDevice);
	long m_HouseSceneID;
	CHouseObject *m_HouseObject;		
	char m_strInName[MAX_NAMEBUFFER];
	char m_strMedName[MAX_NAMEBUFFER];
	char m_strOutName[MAX_NAMEBUFFER];

	bool m_bOctree;
	bool m_bNotCull;
	bool m_bSkip;
	bool m_bMedCull;

	CHouseObjectScene();
	virtual ~CHouseObjectScene();

	void SetSkipFlag(bool bSkip) { m_bSkip = bSkip;}
	bool GetSkipFlag() { return m_bSkip;}
}



*/
CHouseObjectScene::CHouseObjectScene() : m_SelfLoadOut(false),m_SelfLoadMed(false),m_SelfLoadIn(false),m_SelfLoadInObject(false), m_bFlag(false),
										m_bOctree(false),m_bNotCull(false),m_bSkip(false),m_bMedCull(true),m_HouseObject(NULL), 
										m_pImposterScene(NULL)
{
	m_bInit = false;

}

CHouseObjectScene::~CHouseObjectScene()
{
	if(m_HouseObject)
	{
		CROSSM::SafeDelete(m_HouseObject);
//		CSceneManager::_ReleaseObj(CROSSM::POOL_HOUSEOBJ,m_HouseObject);
		m_HouseObject = NULL;
	}
	
	CImposterScene::DeleteScene( m_pImposterScene ); 
	m_pImposterScene = NULL; 
}

void CHouseObjectScene::Render(LPDIRECT3DDEVICE8 pd3dDevice)
{
	if(isCulling()==false || m_bSkip || !m_HouseObject)	
		return;

/*
	vector3 vPos; 
	CSceneManager::m_CharacterManager.m_CharacterList[0].m_pChrmodel->GetPosition( vPos.x, vPos.y, vPos.z );
	D3DXVECTOR3 vCharPos = D3DXVECTOR3( vPos.x, vPos.y, vPos.z ); 
	D3DXVECTOR3 vHousePos = D3DXVECTOR3( m_AccumulateTM._41, m_AccumulateTM._42, m_AccumulateTM._43 ); 
	D3DXVECTOR3 vCharToHouseDir = vHousePos - vCharPos;	
	float fDist = D3DXVec3Length( &vCharToHouseDir ); 

	if( CImposterScene::IsRenderEnable(fDist, m_fRad) ) 
	{
		if( !m_pImposterScene  )
		{
			m_pImposterScene = CImposterScene::GetScene( this ); 
		}

		if( m_pImposterScene )
		{
			CImposterScene::PushRenderScene( m_pImposterScene, vCharPos, vHousePos ); 
			return; 
		}
	}	
*/

	if( CImposterScene::IsActivated() )
	{
		vector3 vPos; 
		CSceneManager::m_CharacterManager.m_CharacterList[0].m_pChrmodel->GetPosition( vPos.x, vPos.y, vPos.z );
		D3DXVECTOR3 vCharPos = D3DXVECTOR3( vPos.x, vPos.y, vPos.z ); 
		D3DXVECTOR3 vHousePos = D3DXVECTOR3( m_AccumulateTM._41, m_AccumulateTM._42, m_AccumulateTM._43 ); 
		D3DXVECTOR3 vCharToHouseDir = vHousePos - vCharPos;	
		float fDist = D3DXVec3Length( &vCharToHouseDir ); 

		if( CImposterScene::IsRenderEnable(fDist, this) ) 
		{
			if( !m_pImposterScene  )
			{
				m_pImposterScene = CImposterScene::GetScene( this ); 
			}

			if( m_pImposterScene )
			{
				m_pImposterScene->Render( vCharPos, vHousePos ); 
				return; 
			}
		}
	}	


	pd3dDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&m_AccumulateTM);	
	m_HouseObject->Render(pd3dDevice);

// Octree 렌더. Octree 문제 확인할때 on
//m_HouseObject->RenderOctree(pd3dDevice);
}



