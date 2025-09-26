// hristmasParticleManager.cpp: implementation of the ChristmasParticleManager class.
//
//////////////////////////////////////////////////////////////////////

#include "ChristmasParticleManager.h"
#include "SceneManager.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
const D3DXVECTOR3 g_ChristmasParticleColor[4] = {
	D3DXVECTOR3(1.0f, 0.22f, 0.16f),
	D3DXVECTOR3(0.05f, 0.38f,1.0f),
	D3DXVECTOR3(0.07f,1.0f, 0.34f),
	D3DXVECTOR3(1.0f, 0.83f,0.38f),
};
CChristmasParticleManager::CChristmasParticleManager()
{
	m_iNodesNum = 0;
	m_Nodes.clear();

}

CChristmasParticleManager::~CChristmasParticleManager()
{
	for(int i = 0; i < (int)m_Nodes.size(); i++ ) {
		if(m_Nodes[i] != NULL) {
			delete m_Nodes[i];
			m_Nodes[i] = NULL;
		}
	}
	m_Nodes.clear();

}

void CChristmasParticleManager::AddParticle(CChristmasParticle *pNode,float fX,float fY,float fZ) {

	if(pNode != NULL) {
		pNode->SetMaxLife(75.0f);
		pNode->SetVisible(1.0f,0.0f);
		pNode->SetBaseMass(1.2f);
		pNode->SetFraction(0.0f);
		pNode->SetGravity( 0.0f, -0.1f, 0.0f );
		pNode->SetColor(1.0f,1.0f,1.0f,0.0f,0.0f,0.0f);
		pNode->SetSize(4.0f,4.0f,4.0f,4.0f);

		D3DXVECTOR3 vecCenter;
		
		vector3 vecDir = CSceneManager::m_ViewCamera->GetViewTowardVector();
		vecDir.Normalize();


		vecCenter.x = fX + vecDir.x * 1500.0f;
		vecCenter.y = fY + 100;//m_pChristmasParticle->m_fHeight;

		vecCenter.z = fZ + vecDir.z * 1500.0f;
		pNode->SetCenter(vecCenter.x,vecCenter.y,vecCenter.z);

		m_Nodes.push_back(pNode);
		m_iNodesNum = (int)m_Nodes.size();
	}
}
void CChristmasParticleManager::DeleteEnd() {
	int i;
	for( i= (m_iNodesNum - 1); i  >= 0; i-- ) {
		if((m_Nodes[i] != NULL) && !m_Nodes[i]->m_bStart && m_Nodes[i]->GetEnd()) {
			delete m_Nodes[i];
			m_Nodes[i] = NULL;
			m_Nodes.erase(m_Nodes.begin() + i);
			m_iNodesNum = (int)m_Nodes.size();

		}
	}
}
void CChristmasParticleManager::DeleteParticle(int iNum) {
	if(m_iNodesNum <= iNum) {
		return;
	}

	if(m_Nodes[iNum] != NULL) {
		delete m_Nodes[iNum];
		m_Nodes[iNum] = NULL;
		
	}
	m_Nodes.erase(m_Nodes.begin() + iNum);
	m_iNodesNum = (int)m_Nodes.size();

}
void CChristmasParticleManager::Render() {

	int i;
	for( i = 0; i < m_iNodesNum; i++ ) {
		if(m_Nodes[i] != NULL) {
			m_Nodes[i]->UpdateBuffer();
			m_Nodes[i]->Render();
		}

	}

}
void CChristmasParticleManager::ProcessParticle() {

	int i,j;


	DWORD   dwNowTime;
	dwNowTime = timeGetTime();


	for( i = 0; i < m_iNodesNum; i++ ) {
		float fDeltaTime = 0.0f;
		int iSkipFrame = 1;

		if(m_Nodes[i] == NULL)
			continue;
		
		if(m_Nodes[i]->m_bFirst != true) {
			
			if( (dwNowTime - m_Nodes[i]->m_dwStartTime)  >= 50) {//m_TickFrame) {
				iSkipFrame =((int)(dwNowTime - m_Nodes[i]->m_dwStartTime)/ 50); 
			}
			else {
				iSkipFrame = 0;
			}


			/*fDeltaTime = static_cast<float>((NowTime.millitm - m_Nodes[i]->m_StartTime.millitm));
			iSkipFrame = (int)(fDeltaTime * 0.0001f);		
			
			if(iSkipFrame <= 0)
				iSkipFrame = 1;
			*/
		}
		for(j = 0; j < iSkipFrame; j++ ) { 
			if((m_Nodes[i]->m_bStart) && (m_Nodes[i]->m_fHeight < 600.0f)) {
				m_Nodes[i]->m_fHeight += 20.0f;

			}
			if(m_Nodes[i]->m_bFirst == true) {	// 맨 처음 업데이트
				
				m_Nodes[i]->m_bFirst = false;
				D3DXMATRIX matView,matInvView;

				CSceneManager::GetDevice()->GetTransform(D3DTS_VIEW,&matView);
				D3DXMatrixInverse(&matInvView,NULL,&matView);
				D3DXVECTOR3 vecViewPos = D3DXVECTOR3(matInvView._41,matInvView._42,matInvView._43);
				

				vector3 vecDir = CSceneManager::m_ViewCamera->GetViewTowardVector();
				vecDir.Normalize();
				vector3 vecUp = CSceneManager::m_ViewCamera->GetViewUpVector();
				vecUp.Normalize();
				
				vector3 vecCross = vecDir ^ vecUp;
				vecCross.Normalize();
				

				/*
				vecViewPos.x = CSceneManager::m_CharacterManager.m_CharacterList[0].m_vecRealPosition.x + vecDir.x * 1500.0f;
				vecViewPos.y = CSceneManager::m_CharacterManager.m_CharacterList[0].m_vecRealPosition.y + 100;//m_pChristmasParticle->m_fHeight;

				vecViewPos.z = CSceneManager::m_CharacterManager.m_CharacterList[0].m_vecRealPosition.z + vecDir.z * 1500.0f;
				*/
				//m_Nodes[i]->m_vecCenter = D3DXVECTOR3(vecViewPos.x,vecViewPos.y,vecViewPos.z);
				m_Nodes[i]->m_vecRight = D3DXVECTOR3(vecCross.x,vecCross.y,vecCross.z);
				m_Nodes[i]->m_vecUp = D3DXVECTOR3(vecUp.x,vecUp.y,vecUp.z);
				m_Nodes[i]->SetStart(true);	
				///Sound
				CEffScript *pCristmas = new CEffScript;
				pCristmas->GetScriptBinData("Christmaspiyu.esf");
				
				CSceneManager::AddInterfaceScript(pCristmas);
				vector3 *vecCameraPos = CSceneManager::m_ViewCamera->GetPosition();

				pCristmas->SetStartPos(vecCameraPos->x,vecCameraPos->y,vecCameraPos->z);
				pCristmas->SetEndPos(vecCameraPos->x,vecCameraPos->y,vecCameraPos->z);
				
				pCristmas->SetChr(NULL,NULL);
				pCristmas->SetChrFwd(0.0f,0.0f,1.0f);	
								


			}
					
			if(m_Nodes[i]->m_fHeight >= 600.0f) {	// 목표 위치까지 올라갔을때
				m_Nodes[i]->SetExtUse(true);
				m_Nodes[i]->SetStart(false);
				m_Nodes[i]->SetMaxLife(120.0f);
				
				///Sound
				CEffScript *pCristmas = new CEffScript;
			

				int iRand = rand() % 4;
				
				switch(iRand) {
				case 0:
					m_Nodes[i]->SetColor(g_ChristmasParticleColor[0].x,g_ChristmasParticleColor[0].y,g_ChristmasParticleColor[0].z,
										g_ChristmasParticleColor[0].x,g_ChristmasParticleColor[0].y,g_ChristmasParticleColor[0].z);
					
					break;
				case 1:
					m_Nodes[i]->SetColor(g_ChristmasParticleColor[1].x,g_ChristmasParticleColor[1].y,g_ChristmasParticleColor[2].z,
									g_ChristmasParticleColor[1].x,g_ChristmasParticleColor[1].y,g_ChristmasParticleColor[2].z);

					
					break;
				case 2:
					m_Nodes[i]->SetColor(g_ChristmasParticleColor[2].x,g_ChristmasParticleColor[2].y,g_ChristmasParticleColor[2].z,
									g_ChristmasParticleColor[2].x,g_ChristmasParticleColor[2].y,g_ChristmasParticleColor[2].z);

					
					break;
				case 3:
					m_Nodes[i]->SetColor(g_ChristmasParticleColor[3].x,g_ChristmasParticleColor[3].y,g_ChristmasParticleColor[3].z,
									g_ChristmasParticleColor[3].x,g_ChristmasParticleColor[3].y,g_ChristmasParticleColor[3].z);
					
					
					break;
					
				}
					
				pCristmas->GetScriptBinData("ChristParticle.esf");
				CSceneManager::AddInterfaceScript(pCristmas);
				vector3 *vecCameraPos = CSceneManager::m_ViewCamera->GetPosition();

				pCristmas->SetStartPos(vecCameraPos->x,vecCameraPos->y,vecCameraPos->z);
				pCristmas->SetEndPos(vecCameraPos->x,vecCameraPos->y,vecCameraPos->z);
				
				pCristmas->SetChr(NULL,NULL);
				pCristmas->SetChrFwd(0.0f,0.0f,1.0f);	
								
				
				m_Nodes[i]->SetVisible(1.0f,0.0f);
				m_Nodes[i]->CreateChristmasParticle(m_Nodes[i]->m_vecCenter.x,
					(m_Nodes[i]->m_vecCenter.y + (m_Nodes[i]->m_fHeight * 2.0f)),m_Nodes[i]->m_vecCenter.z,
					5.0f,850,
					m_Nodes[i]->m_vecRight.x,m_Nodes[i]->m_vecRight.y,m_Nodes[i]->m_vecRight.z,
					m_Nodes[i]->m_vecUp.x,m_Nodes[i]->m_vecUp.y,m_Nodes[i]->m_vecUp.z,true);

				m_Nodes[i]->m_fHeight = 0.0f;	
			}
			else if(m_Nodes[i]->m_bStart) {
				m_Nodes[i]->SetMaxLife(10.0f);
				m_Nodes[i]->SetColor(1.0f,1.0f,1.0f,1.0f,1.0f,1.0f);
				m_Nodes[i]->SetVisible(0.3f,0.0f);
				m_Nodes[i]->CreateChristmasParticle(m_Nodes[i]->m_vecCenter.x,
					(m_Nodes[i]->m_vecCenter.y + m_Nodes[i]->m_fHeight),m_Nodes[i]->m_vecCenter.z,0.1f,10,
					m_Nodes[i]->m_vecRight.x,m_Nodes[i]->m_vecRight.y,m_Nodes[i]->m_vecRight.z,
					m_Nodes[i]->m_vecUp.x,m_Nodes[i]->m_vecUp.y,m_Nodes[i]->m_vecUp.z);

			}

			m_Nodes[i]->m_dwStartTime = timeGetTime();
			m_Nodes[i]->Update(1.0f,m_Nodes[i]->m_fHeight);

		}
	}
	DeleteEnd();

		
}
