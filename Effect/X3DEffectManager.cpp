// X3DEffectManager.cpp: implementation of the CX3DEffectManager class.
//
//////////////////////////////////////////////////////////////////////

#include "X3DEffectManager.h"
#include "X3DEffectMesh.h"
#include "SceneNode.h"
#include "SceneManager.h"
//#include <crtdbg.h>
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CX3DEffectManager::CX3DEffectManager()
{
	m_lstExist.clear();
	m_lstFree.clear();
	m_lstEffect.clear();
	m_lstEffect.reserve(40);
	m_lstScript.clear();
	m_lstScript.reserve(40);
	m_lstWScript.clear();
	m_lstIScript.clear();
	m_lstLightning.clear();

	m_EffScriptLimitNum = DEFAULTLIMITSCRIPT;
	m_EffScriptLimitCount = 0;
	
	m_pFrustum = new CViewFrustum;

}

CX3DEffectManager::~CX3DEffectManager()
{
	DeleteAllEffect();
	DeleteAllEffectScript();
	DeleteAllWorldScript();
	DeleteAllInterfaceScript();
	DeleteAllLightning();
	
	delete m_pFrustum;
	m_pFrustum = NULL;

//	CX3DEffectMesh::DeleteAllCash();			// Mesh Cash List Delete


}

void CX3DEffectManager::UpdateFrame(void)
{
	EffectHandleList::iterator it;
	long count = m_lstExist.size() - 1, j;
	CX3DEffect *pEffect;

	m_lpD3DDevice->GetTransform(D3DTS_VIEW, (D3DMATRIX *)&m_matView);

	m_dwTick = timeGetTime();

	for(it = m_lstExist.begin(), j = 0; j <= count; it++, j++)
	{
		pEffect = m_lstEffect[(*it)];
		// 시작 되었으면 update
		if(pEffect->GetEffectStart()) {
			if(m_dwTick - pEffect->GetTick() >= pEffect->GetFrameTick())
			{
				if(!pEffect->Interpolation(m_dwTick - pEffect->GetTick()))
				{
					m_lstFree.push_back((*it));
					delete pEffect;
					pEffect = NULL;
					m_lstExist.erase(it);
					it--;
				} else
				{
					pEffect->SetTick(m_dwTick);
				}
			}
		}
	}
}

void CX3DEffectManager::Render(int num)
{
	EffectHandleList::iterator it;
	int i=0;

	m_lpD3DDevice->SetVertexShader(LVERTEXFVF);

	m_lpD3DDevice->SetTexture(0, NULL);
	m_lpD3DDevice->SetTexture(1, NULL);

	m_lpD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_lpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	m_lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
	m_lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	m_lpD3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
    m_lpD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);

	m_lpD3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_lpD3DDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	m_lpD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_lpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	m_lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_lpD3DDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_ALWAYS);
	m_lpD3DDevice->SetRenderState(D3DRS_ALPHAREF,0x00000000);
	m_lpD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_lpD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	for(it = m_lstExist.begin(); it != m_lstExist.end(); it++,i++)
	{
		if(i== num) {
			m_lpD3DDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
			
		}
		//if(effectmap->EffectSectorIn(
		//camera pos
		matrix *matViewPosition=CSceneManager::GetCamera()->GetMatPosition();	
		vector3 vecViewPos=matViewPosition->GetLoc();
		
		int indexx=(int)(vecViewPos.x/SECTORSIZE);
		int indexy=(int)(vecViewPos.z/SECTORSIZE);

		//culling 
		CSceneNode CheckNode;

		CheckNode.m_fRad=100.0f;
		CheckNode.m_AccumulateTM.Translation(*m_lstEffect[(*it)]->GetCenter());
		if(m_lstEffect[(*it)]->map_effect == true) {
			if(CheckNode.isCulling())
			{
			//if(effectmap->EffectSectorIn(indexx,indexy,i)) {
				if(m_lstEffect[(*it)]) m_lstEffect[(*it)]->Render();
			//}
			
			}
		}
		else {
			if(m_lstEffect[(*it)]) m_lstEffect[(*it)]->Render();
		}
		if(i == num)
			m_lpD3DDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
		
	}

	matrix matWorld;
	matWorld.MakeIdent();
	m_lpD3DDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX *)&matWorld);

	m_lpD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_lpD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);	
}

void CX3DEffectManager::Render()
{
	EffectHandleList::iterator it;
	int i=0;

	m_lpD3DDevice->SetVertexShader(LVERTEXFVF);

	m_lpD3DDevice->SetTexture(0, NULL);
	m_lpD3DDevice->SetTexture(1, NULL);

	m_lpD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_lpD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_lpD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	m_lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	m_lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
	m_lpD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	m_lpD3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
    m_lpD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);

	m_lpD3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_lpD3DDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	m_lpD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_lpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_lpD3DDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_ALWAYS);
	m_lpD3DDevice->SetRenderState(D3DRS_ALPHAREF,0x00000000);
	m_lpD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_lpD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	
	for(it = m_lstExist.begin(); it != m_lstExist.end(); it++,i++)
	{
		if(!(m_lstEffect[(*it)]->GetEffectStart()))
			continue;
		//if(effectmap->EffectSectorIn(
		//camera pos
		matrix *matViewPosition=CSceneManager::GetCamera()->GetMatPosition();	
		vector3 vecViewPos=matViewPosition->GetLoc();
		
		int indexx=(int)(vecViewPos.x/SECTORSIZE);
		int indexy=(int)(vecViewPos.z/SECTORSIZE);

		//culling 
		CSceneNode CheckNode;

		CheckNode.m_fRad=100.0f;
		CheckNode.m_AccumulateTM.Translation(*m_lstEffect[(*it)]->GetCenter());
		if(m_lstEffect[(*it)]->map_effect == true) {
			if(CheckNode.isCulling())
			{
			//if(effectmap->EffectSectorIn(indexx,indexy,i)) {
				if(m_lstEffect[(*it)]) m_lstEffect[(*it)]->Render();
			//}
			
			}
		}
		else {
			if(m_lstEffect[(*it)]) m_lstEffect[(*it)]->Render();
		}
		
	}

	matrix matWorld;
	matWorld.MakeIdent();
	m_lpD3DDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX *)&matWorld);

	m_lpD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_lpD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);	
}

XEFFECT CX3DEffectManager::AddEffect(CX3DEffect *lpNewEffect)
{
	XEFFECT hEffect;
	if(m_lstFree.empty())
	{
		hEffect = m_lstEffect.size();
		m_lstEffect.push_back(lpNewEffect);
	} else
	{
		hEffect = m_lstFree.back();
		m_lstEffect[hEffect] = lpNewEffect;
		m_lstFree.pop_back();
	}

	lpNewEffect->SetDevice(m_lpD3DDevice);
	lpNewEffect->SetViewMatrix(&m_matView);
	lpNewEffect->SetEffectSetting();
	lpNewEffect->SetTick(timeGetTime());
	m_lstExist.push_back(hEffect);
	return hEffect;
}
char *CX3DEffectManager::GetEsfName(int index) {
	if(m_lstScript[index] != NULL) {
		return m_lstScript[index]->m_FileName;
	}
	return NULL;	
}
void CX3DEffectManager::AddLightning(CLightning *new_light) {
	m_lstLightning.push_back(new_light);
}
CEffScript *CX3DEffectManager::AddEffScript(CEffScript *new_script) {
//	_ASSERTE( _CrtCheckMemory());
	if(!new_script->GetMine()) { // 내가 사용한 스킬이 아니면 제한 갯수 적용
		if(new_script->GetBSkill() && (!new_script->GetChant())) { // skill effect 이면 적용
			if(m_EffScriptLimitCount > m_EffScriptLimitNum) {
				return NULL;
			}
			else
				m_EffScriptLimitCount++;
		}
	}
	m_lstScript.push_back(new_script);
//	_ASSERTE( _CrtCheckMemory()); 
///////// 인터페이스 이팩트 테스트///
/*	
		// interface effect
		vector3 pos;
		matrix *matpos = NULL;
		vector3 camera_fwd;
		matpos = CSceneManager::GetCamera()->GetMatPosition();

		camera_fwd = CSceneManager::GetCamera()->GetViewTowardVector();
		
		pos.x = matpos->_41;
		pos.y = matpos->_42;
		pos.z = matpos->_43;

		pos.x += (camera_fwd.x * 300.0f);
		pos.y += (camera_fwd.y * 300.0f); 
		pos.z += (camera_fwd.z * 300.0f);

		new_script->SetStartPos(pos.x,pos.y,pos.z);
		
		// interface effect
		//screen projection
		D3DXMATRIX t_project;
		D3DXMATRIX t_view;
		D3DXMATRIX t_world;
		D3DXVECTOR3 t_pos;

		D3DVIEWPORT8 t_viewport;

		t_pos.x = pos.x;
		t_pos.y = pos.y;
		t_pos.z = pos.z;

		m_lpD3DDevice->GetViewport(&t_viewport);
		m_lpD3DDevice->GetTransform(D3DTS_WORLD,&t_world);
		m_lpD3DDevice->GetTransform(D3DTS_VIEW,&t_view);
		m_lpD3DDevice->GetTransform(D3DTS_PROJECTION,&t_project);

		D3DXVec3Project(&(new_script->m_Projection),
						&t_pos,
						&t_viewport,
						&t_project,
						&t_view,
						&t_world);

	new_script->SetScriptValue(ESINTERFACE);
*/
//////////
	new_script->SetDevice(m_lpD3DDevice);
	//return (m_lstScript.size() - 1);
//	_ASSERTE( _CrtCheckMemory());
//	_ASSERTE( _CrtCheckMemory());
	return new_script;
	
}
int CX3DEffectManager::AddWorldEffScript(CEffScript *new_script) {
	m_lstWScript.push_back(new_script);
	new_script->SetDevice(m_lpD3DDevice);
	new_script->SetWorldEffect(true);
	return (m_lstWScript.size() - 1);

}
CEffScript *CX3DEffectManager::AddInterfaceScript(CEffScript *new_script) {
	// 특정 포지션이 설정되어있지 않으면

/*	if(!(new_script->m_bInterfaceSet)) { 	
		vector3 pos;
		matrix *matpos = NULL;
		vector3 camera_fwd;
		vector3 camera_up;
		vector3 camera_right;

		matpos = CSceneManager::GetCamera()->GetMatPosition();

		camera_fwd = CSceneManager::GetCamera()->GetViewTowardVector();
		camera_up = CSceneManager::GetCamera()->GetViewUpVector();

		camera_right = camera_fwd^camera_up;

		//camera_fwd = CSceneManager::GetCamera()->GetFwdVector();

		pos.x = matpos->_41;
		pos.y = matpos->_42;
		pos.z = matpos->_43;

		pos.x += (camera_fwd.x * 100.0f);
		pos.y += (camera_fwd.y * 100.0f); 
		pos.z += (camera_fwd.z * 100.0f);

		if(new_script->m_bInterfacePos) {
			pos += (camera_up * (new_script->m_InterfacePos[1]));
			pos += ((-camera_right) * (new_script->m_InterfacePos[0]));
		}
		//pos *= 100.0f;
		
		new_script->SetStartPos(pos.x,pos.y,pos.z);
		
		// interface effect
		//screen projection
		D3DXMATRIX t_project;
		D3DXMATRIX t_view;
		D3DXMATRIX t_world;
		D3DXVECTOR3 t_pos;

		D3DVIEWPORT8 t_viewport;

		t_pos.x = pos.x;
		t_pos.y = pos.y;
		t_pos.z = pos.z;


		m_lpD3DDevice->GetViewport(&t_viewport);
		m_lpD3DDevice->GetTransform(D3DTS_WORLD,&t_world);
		m_lpD3DDevice->GetTransform(D3DTS_VIEW,&t_view);
		m_lpD3DDevice->GetTransform(D3DTS_PROJECTION,&t_project);

		D3DXVec3Project(&(new_script->m_Projection),
						&t_pos,
						&t_viewport,
						&t_project,
						&t_view,
						&t_world);


		new_script->SetScriptValue(ESINTERFACE);
		for(int i=0;i<new_script->m_SubScriptNum;i++) {
			new_script->m_SubScript[i].m_Projection = new_script->m_Projection;
		}
	}
*/
	new_script->SetInterfaceSet(true);
	if(new_script->m_SubScriptNum >0) {
		new_script->m_bInterfacePos = true; // sub interface effect 를 이용하기 위한 setting
	}
	m_lstIScript.push_back(new_script);
	
	new_script->SetDevice(m_lpD3DDevice);
	return new_script;

}

int CX3DEffectManager::ProcessScript(int i) {// return value 0: 스크립트 종료 
											 //	1: 스크립트 유지
											 //	2: effect target에 충돌


	int index = 0;

	if(((int)m_lstScript.size() - 1)< i) {
		return 1;
	}

	if(m_lstScript[i] == NULL) {
		DeleteEndScript(i);
		return 0;
	}
	index = m_lstScript[i]->ProcessEffect(CSceneManager::m_SkillWeatherEffect);
	if(!index) 
		DeleteEndScript(i);
	else if(index == 2) {
		m_lstScript[i]->SetCrash(true);
	}
	else
		m_lstScript[i]->SetCrash(false);
	return index;	
}
// 두번째 타입의 이팩트 검사 :: NULL 이면 처리 끝이라는 의미로 true return
bool CX3DEffectManager::CheckNullScript(CEffScript *t) {
	int script_num = m_lstScript.size();
	int i;
	for(i=0;i<script_num;i++) {
		if(m_lstScript[i] == t) 
			return false;
	}
	return true;
}
bool CX3DEffectManager::CheckNullInterfaceScript(CEffScript *t) {
	int script_num = m_lstIScript.size();
	int i;
	for(i=0;i<script_num;i++) {
		if(m_lstIScript[i] == t) 
			return false;
	}
	return true;
}


bool CX3DEffectManager::CheckScript(int index,CEffScript *t) {
	if(((int)m_lstScript.size() -1) < index)
		return false;
	if(m_lstScript[index] == t)
		return true;
	return false;

}

bool CX3DEffectManager::ProcessWScript(int i,bool bCull) {
	int index = 0;
	
	if(((int)m_lstWScript.size() - 1)< i)
		return false;
	
	if(m_lstWScript[i] == NULL) {
		DeleteWScript(i);
		return false;
	}
	/*if(bCull == false) {	// Culling 적용 
		if(m_lstWScript[i]->GetSectorCull())
			return false;
	}*/
	index = m_lstWScript[i]->ProcessEffect();
	
	if(!index)
		DeleteWScript(i);
	else if(index == 2) {
		m_lstWScript[i]->SetCrash(true);
		return true;
	}
	else
		m_lstWScript[i]->SetCrash(false);
	return false;	
}
bool CX3DEffectManager::ProcessLightning(int index) {
	
	if(((int)m_lstLightning.size() - 1)< index)
		return false;
	
	if(m_lstWScript[index] == NULL) {
		DeleteWScript(index);
		return false;
	}
	D3DXVECTOR3 dermy;

	m_lstLightning[index]->UpdateLightning(dermy,false);

	return true;
	
}
bool CX3DEffectManager::ProcessInterfaceScript(int i) {
	int index = 0;
	
	if(((int)m_lstIScript.size() - 1)< i)
		return false;
	
	if(m_lstIScript[i] == NULL) {
		DeleteInterfaceScript(i);
		return false;
	}


	index = m_lstIScript[i]->ProcessEffect(CSceneManager::m_SkillWeatherEffect);
	if(!index)
		DeleteInterfaceScript(i);
	else if(index == 2) {
		m_lstIScript[i]->SetCrash(true);
		return true;
	}
	else
		m_lstIScript[i]->SetCrash(false);
	return false;	
}

bool CX3DEffectManager::CheckInterfaceScript(int index,CEffScript *t) {
	if(m_lstIScript[index] == t)
		return true;
	return false;

}

int CX3DEffectManager::GetLightningNum() {
	int size = m_lstLightning.size();
	return size;
}

int CX3DEffectManager::GetScriptNum() {
	int size = m_lstScript.size();
	return size;
}
int CX3DEffectManager::GetWScriptNum() {
	int size = m_lstWScript.size();
	return size;

}
int CX3DEffectManager::GetInterfaceScriptNum() {
	int size = m_lstIScript.size();
	return size;
}
void CX3DEffectManager::RenderScript() {

/*	if(m_pFrustum != NULL)
		m_pFrustum->Update();
*/

	int size = m_lstScript.size();
	int i;

	for(i=0;i<size;i++) {
		if(m_lstScript[i] != NULL) {
		
			// Frustum Culling
/*		
			int iCount = 0;
			int iFalse = 0;

			if(m_pFrustum != NULL) {
				if(m_lstScript[i]->m_StartPosSet)
				{
					iCount++;
					
					if(!m_pFrustum->SphereInFrustum(m_lstScript[i]->m_StartPos.x,
												m_lstScript[i]->m_StartPos.y,
												m_lstScript[i]->m_StartPos.z,
												1600.0f))
						iFalse++;						
				}
				if(m_lstScript[i]->m_EndPosNum > 0)
				{
					iCount++;

					if(!m_pFrustum->SphereInFrustum(m_lstScript[i]->m_EndPos[0].x,
												m_lstScript[i]->m_EndPos[0].y,
												m_lstScript[i]->m_EndPos[0].z,
												1600.0f))
						iFalse++;

				}
			}
			
			if(iFalse != iCount)*/
				m_lstScript[i]->Render();
			
		}
	}
}
void CX3DEffectManager::RenderLightning() {
	int i;
	for(i=0;i<(int)m_lstLightning.size();i++) {
		if(m_lstLightning[i] != NULL)
			m_lstLightning[i]->Render(0);
	
	}
}
void CX3DEffectManager::RenderWorldScript(bool bCull) {
	
	if(m_pFrustum != NULL)
		m_pFrustum->Update();

	int size = m_lstWScript.size();
	int i;
	for(i=0;i<size;i++) {
		if(m_lstWScript[i] != NULL) {
		
			if((bCull == false) && (m_lstWScript[i]->m_bFrustumCull) ) {	// Culling 적용 
				if(m_lstWScript[i]->GetSectorCull())
					continue;
			}

			if(m_lstWScript[i]->m_bPlayTime) {			// Play Time Setting 되어 있을때
				if(m_lstWScript[i]->m_bVisTime) {
					// Frustum Culling
					if(m_pFrustum != NULL) {
						if(m_lstWScript[i]->m_StartPosSet)
						{
							if(m_lstWScript[i]->m_bFrustumCull) {
								if(!m_pFrustum->SphereInFrustum(m_lstWScript[i]->m_StartPos.x,
															m_lstWScript[i]->m_StartPos.y,
															m_lstWScript[i]->m_StartPos.z,
															600.0f))
									continue;						
							}
						}
					}
					m_lstWScript[i]->Render();
				}
			}
			else {
				// Frustum Culling
				if(m_pFrustum != NULL) {
					if(m_lstWScript[i]->m_StartPosSet)
					{
					
						if(m_lstWScript[i]->m_bFrustumCull) {
							if(!m_pFrustum->SphereInFrustum(m_lstWScript[i]->m_StartPos.x,
														m_lstWScript[i]->m_StartPos.y,
														m_lstWScript[i]->m_StartPos.z,
														600.0f))
								continue;						

						}
					}
				}

				m_lstWScript[i]->Render();
			}
		}

	}
}
void CX3DEffectManager::RenderInterfaceScript() {
	int i;
	int size = m_lstIScript.size();

	for(i=0;i<size;i++) {
		
		if(m_lstIScript[i] != NULL) {
			// unprojection 
			
			D3DXMATRIX t_project;
			D3DXMATRIX t_view;
			D3DXMATRIX t_world;
			
			D3DXMATRIX t_ortho;
			D3DXMATRIX t_oworld;
			D3DXMATRIX t_oview;

			D3DXMatrixIdentity(&t_oworld);
			D3DXMatrixIdentity(&t_oview);

			D3DXVECTOR3 t_pos;

			D3DVIEWPORT8 t_viewport;

			m_lpD3DDevice->GetViewport(&t_viewport);
			m_lpD3DDevice->GetTransform(D3DTS_WORLD,&t_world);
			m_lpD3DDevice->GetTransform(D3DTS_VIEW,&t_view);
			m_lpD3DDevice->GetTransform(D3DTS_PROJECTION,&t_project);
			
			D3DXMatrixOrthoLH(&t_ortho,100,100,0.1f, 10000.0f);
			m_lpD3DDevice->SetTransform(D3DTS_PROJECTION,&t_ortho);
			m_lpD3DDevice->SetTransform(D3DTS_WORLD,&t_oworld);
			m_lpD3DDevice->SetTransform(D3DTS_VIEW,&t_oview);

			D3DXVECTOR3 t_proj(0.0f,0.0f,0.2f);
			if(m_lstIScript[i]->m_bInterfacePos) {
				if(m_lstIScript[i]->m_SubScriptNum >0) { //subscript use
					for(int j=0;j<(m_lstIScript[i]->m_SubScriptNum);j++) {
						D3DXMatrixOrthoLH(&t_ortho,100,100,0.1f, 10000.0f);
						
						m_lpD3DDevice->SetTransform(D3DTS_PROJECTION,&t_ortho);
						
						D3DXMatrixIdentity(&t_oworld);
						D3DXMatrixIdentity(&t_oview);

						m_lpD3DDevice->SetTransform(D3DTS_WORLD,&t_oworld);
						m_lpD3DDevice->SetTransform(D3DTS_VIEW,&t_oview);

						t_proj.x = m_lstIScript[i]->m_SubScript[j].m_InterfacePos[0];
						t_proj.y = m_lstIScript[i]->m_SubScript[j].m_InterfacePos[1];
						
						D3DXVec3Unproject(
							&t_pos,
							&t_proj,
							&t_viewport,
							&t_ortho,
							&t_oview,
							&t_oworld);
					
						m_lstIScript[i]->SetStartPos(t_pos.x,t_pos.y,t_pos.z);
					}
				}
				else { // sub script not use
					t_proj.x = m_lstIScript[i]->m_InterfacePos[0];
					t_proj.y = m_lstIScript[i]->m_InterfacePos[1];
					D3DXVec3Unproject(
						&t_pos,
						&t_proj,
						&t_viewport,
						&t_ortho,
						&t_oview,
						&t_oworld);
				
					m_lstIScript[i]->SetStartPos(t_pos.x,t_pos.y,t_pos.z);


				}
			}
		
			m_lstIScript[i]->Render();
			m_lpD3DDevice->SetTransform(D3DTS_PROJECTION,&t_project);
			m_lpD3DDevice->SetTransform(D3DTS_WORLD,&t_world);
			m_lpD3DDevice->SetTransform(D3DTS_VIEW,&t_view);
		}

	}
}
void CX3DEffectManager::DeleteEndScript(int index) {
	int i;
	for(i=0;i<(int)m_lstScript.size();i++) {
		if(i== index) {
			if(m_lstScript[i] != NULL) {
				
				if(!m_lstScript[i]->GetMine()) { // 내가 사용한 스킬이 아니면 제한 갯수 적용
					if(m_lstScript[i]->GetBSkill()) { // skill effect 이면 적용
						if(m_EffScriptLimitCount)
							m_EffScriptLimitCount--;
					}
				}
				delete m_lstScript[i];
				m_lstScript[i] = NULL;
//				_ASSERTE( _CrtCheckMemory()); 
			}

			m_lstScript.erase(m_lstScript.begin() + i);
//			_ASSERTE( _CrtCheckMemory()); 
			break;
		}
	}
//	_ASSERTE( _CrtCheckMemory());
}
void CX3DEffectManager::DeleteLightning(int index) {
	int i;
	for(i=0;i<(int)m_lstLightning.size();i++) {
		if(i == index) {
			if(m_lstLightning[i] != NULL) {
				delete m_lstLightning[i];
				m_lstLightning[i] = NULL;
				
			}
			m_lstLightning.erase(m_lstLightning.begin() + i);
			break;
		}
	}
}
void CX3DEffectManager::DeleteEndScript(CEffScript *del) {
	int i;
	for(i=0;i<(int)m_lstScript.size();i++) {
		if(m_lstScript[i] == del) {
			if(!m_lstScript[i]->GetMine()) { // 내가 사용한 스킬이 아니면 제한 갯수 적용
				if(m_lstScript[i]->GetBSkill()) { // skill effect 이면 적용
					if(m_EffScriptLimitCount)
						m_EffScriptLimitCount--;
				}
			}
			
			delete m_lstScript[i];
//			_ASSERTE( _CrtCheckMemory()); 
			m_lstScript[i] = NULL;

			m_lstScript.erase(m_lstScript.begin() + i);
//			_ASSERTE( _CrtCheckMemory()); 
			break;
		}
	}
//	_ASSERTE( _CrtCheckMemory());
}
void CX3DEffectManager::DeleteWScript(int index) {

	int i;
	for(i=0;i<(int)m_lstWScript.size();i++) {
		if(i== index) {
			if(m_lstWScript[i] != NULL) {
				delete m_lstWScript[i];
				m_lstWScript[i] = NULL;
			}
			m_lstWScript.erase(m_lstWScript.begin() + i);
			break;
		}
	}


}
void CX3DEffectManager::DeleteInterfaceScript(int index) {
	int i;
	for(i=0;i<(int)m_lstIScript.size();i++) {
		if(i== index) {
			if(m_lstIScript[i] != NULL) {
				delete m_lstIScript[i];
				m_lstIScript[i] = NULL;
			}
			m_lstIScript.erase(m_lstIScript.begin() + i);
			break;
		}
	}

}
void CX3DEffectManager::DeleteInterfaceScript(CEffScript *del) {
	int i;
	for(i=0;i<(int)m_lstIScript.size();i++) {
		if(m_lstIScript[i] == del) {
				delete m_lstIScript[i];
				m_lstIScript[i] = NULL;
				m_lstIScript.erase(m_lstIScript.begin() + i);
				break;
		}
	}
}

void CX3DEffectManager::DeleteEffect(XEFFECT hEffect)
{
	m_lstFree.push_back(hEffect);
	delete (CX3DEffect *)m_lstEffect[hEffect];

	EffectHandleList::iterator it;
	for(it = m_lstExist.begin(); it != m_lstExist.end(); it++)
	{	
		if(hEffect == (*it)) { 
			m_lstExist.erase(it); 
			break; 
		}
	}
}

BOOL CX3DEffectManager::IsLive(XEFFECT hEffect)
{
	EffectHandleList::iterator it;

	for(it = m_lstFree.begin(); it != m_lstFree.end(); it++)
	{
		if(hEffect == (*it)) return FALSE;
	}

	return TRUE;
}

void CX3DEffectManager::DeleteAllEffect(void)
{
	EffectHandleList::iterator it;

	if(m_lstExist.size())
	{
		for(it = m_lstExist.begin(); it != m_lstExist.end(); it++)
		{
			delete (CX3DEffect *)m_lstEffect[(*it)];
		}

		m_lstExist.clear();
	}
	m_lstFree.clear();
	m_lstEffect.clear();
	//m_lstScript.clear();

}

void CX3DEffectManager::DeleteAllEffectScript() {

	int i;
	
	// Pool 사용시에 메모리 해제가 제대로 안되고 있음.. by Vincent
	//if(CEffScript::ms_myPool.m_nTotalInUse) {
		for(i=0;i<(int)m_lstScript.size();i++) {
				if(m_lstScript[i] != NULL) {
					delete m_lstScript[i];
	//				_ASSERTE( _CrtCheckMemory()); 
					m_lstScript[i] = NULL;
				}
				
		}
	//}
	m_lstScript.clear();
//	_ASSERTE( _CrtCheckMemory()); 
}

void CX3DEffectManager::DeleteAllLightning() {
	int i;
	for(i=0;i<(int)m_lstLightning.size();i++) {
		if(m_lstLightning[i] != NULL) {
			delete m_lstLightning[i];
			m_lstLightning[i] = NULL;
		}
	}
	m_lstLightning.clear();

}

void CX3DEffectManager::DeleteAllWorldScript() {

	int i;
	// Pool 사용시에 메모리 해제가 제대로 안되고 있음.. by Vincent
	//if(CEffScript::ms_myPool.m_nTotalInUse) {
		for(i=0;i<(int)m_lstWScript.size();i++) {
				if(m_lstWScript[i] != NULL) {
					delete m_lstWScript[i];
					m_lstWScript[i] = NULL;
				}
				
		}
	//}
	m_lstWScript.clear();


}

void CX3DEffectManager::DeleteAllInterfaceScript() {
	int i;
	// Pool 사용시에 메모리 해제가 제대로 안되고 있음.. by Vincent
	//if(CEffScript::ms_myPool.m_nTotalInUse) {	
		for(i=0;i<(int)m_lstIScript.size();i++) {
				if(m_lstIScript[i] != NULL) {
					delete m_lstIScript[i];
					m_lstIScript[i] = NULL;
				}
				
		}
	//}
	m_lstIScript.clear();
}
