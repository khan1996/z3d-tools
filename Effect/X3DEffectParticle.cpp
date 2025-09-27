// X3DEffectParticle.cpp: implementation of the CX3DEffectParticle class.
//
//////////////////////////////////////////////////////////////////////

#include "X3DEffect.h"
#include "X3DEffectParticle.h"
#include "SceneManager.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CX3DEffectParticle::CX3DEffectParticle()
{
	m_dwSrcBlending = D3DBLEND_SRCALPHA;
	m_dwDestBlending = D3DBLEND_ONE;

	m_bTexAni = FALSE;

	m_lpVertices = NULL;
	m_lpVerticesBlend = NULL;
	m_lpVerticeIndex = NULL;
	m_lpVerticeInfo = NULL;
}

CX3DEffectParticle::~CX3DEffectParticle()
{
	if(m_lpVerticeInfo) { delete[] m_lpVerticeInfo; m_lpVerticeInfo = NULL; }
/*	if(m_lpVerticeIndex) { m_lpVerticeIndex->Release(); m_lpVerticeIndex = NULL; }
	if(m_lpVerticesBlend) { m_lpVerticesBlend->Release(); m_lpVerticesBlend = NULL; }
	if(m_lpVertices) { m_lpVertices->Release(); m_lpVertices = NULL; }*/

	if(m_lpVerticeIndex) { 
		if(CSceneManager::ms_pBufferPools)
			CSceneManager::ms_pBufferPools->UnRef(Caldron::Scene::D3DBUFFEROBJ_INDEX,m_lpVerticeIndex);
		else
			m_lpVerticeIndex->Release(); 
		m_lpVerticeIndex = NULL;
	}
	if(m_lpVerticesBlend) { 
		if(CSceneManager::ms_pBufferPools)
			CSceneManager::ms_pBufferPools->UnRef(Caldron::Scene::D3DBUFFEROBJ_VERTEX,m_lpVerticesBlend);
		else
			m_lpVerticesBlend->Release(); 
		m_lpVerticesBlend = NULL; 
	}
	if(m_lpVertices) { 
		if(CSceneManager::ms_pBufferPools)
			CSceneManager::ms_pBufferPools->UnRef(Caldron::Scene::D3DBUFFEROBJ_VERTEX,m_lpVertices);
		else
			m_lpVertices->Release(); 
		m_lpVertices = NULL; 
	}
}

void CX3DEffectParticle::Create(unsigned long dwStartFrame, unsigned long dwEndFrame)
{
	m_dwStartFrame = dwStartFrame;
	m_dwEndFrame = dwEndFrame;

	if (m_lpVerticeInfo) { delete[] m_lpVerticeInfo; m_lpVerticeInfo = NULL; }
	m_lpVerticeInfo = NULL;

	m_dwMaxAmount = m_dwDrawAmount = 0;

	m_dwVolumeType = 0;
	m_fVolX = m_fVolY = m_fVolZ = m_fRadius = m_fInnerRadius = 0.0f;
}

BOOL CX3DEffectParticle::CreateBuffer(void)
{
	FloatKeyList::iterator fIt;
	unsigned long i;

	m_dwMaxAmount = m_dwDrawAmount = 0;
	for(fIt = m_lstAmount.Begin(); fIt != m_lstAmount.End(); fIt++)
	{
		m_dwMaxAmount += ((*fIt).second);
	}

	if(m_lpVerticeInfo) { delete[] m_lpVerticeInfo; m_lpVerticeInfo = NULL; }
/*	if(m_lpVerticeIndex) { m_lpVerticeIndex->Release(); m_lpVerticeIndex = NULL; }
	if(m_lpVerticesBlend) { m_lpVerticesBlend->Release(); m_lpVerticesBlend = NULL; }
	if(m_lpVertices) { m_lpVertices->Release(); m_lpVertices = NULL; }
*/
	if(m_lpVerticeIndex) { 
		CSceneManager::ms_pBufferPools->UnRef(Caldron::Scene::D3DBUFFEROBJ_INDEX,m_lpVerticeIndex);
		//m_lpVerticeIndex->Release(); 
		m_lpVerticeIndex = NULL;
	}
	if(m_lpVerticesBlend) { 
		CSceneManager::ms_pBufferPools->UnRef(Caldron::Scene::D3DBUFFEROBJ_VERTEX,m_lpVerticesBlend);
		//m_lpVerticesBlend->Release(); 
		m_lpVerticesBlend = NULL; 
	}
	if(m_lpVertices) { 
		CSceneManager::ms_pBufferPools->UnRef(Caldron::Scene::D3DBUFFEROBJ_VERTEX,m_lpVertices);
	//	m_lpVertices->Release(); 
		m_lpVertices = NULL; 
	}
//	m_lpVertices = CSceneManager::ms_pBufferPools->GetVertexBuffer(m_dwMaxAmount * 4 * sizeof(LVertex),LVERTEXFVF,true);
//	m_lpVerticesBlend = CSceneManager::ms_pBufferPools->GetVertexBuffer(m_dwMaxAmount * 4 * sizeof(LVertex),LVERTEXFVF,true);
//	m_lpVerticeIndex = CSceneManager::ms_pBufferPools->GetIndexBuffer(m_dwMaxAmount * 2 * 3 * sizeof(unsigned short),D3DFMT_INDEX16,false);
	m_lpVertices = CSceneManager::ms_pBufferPools->GetVertexBuffer(m_dwMaxAmount * 4 * sizeof(LVertex),LVERTEXFVF,false);
	m_lpVerticesBlend = CSceneManager::ms_pBufferPools->GetVertexBuffer(m_dwMaxAmount * 4 * sizeof(LVertex),LVERTEXFVF,false);
	m_lpVerticeIndex = CSceneManager::ms_pBufferPools->GetIndexBuffer(m_dwMaxAmount * 2 * 3 * sizeof(unsigned short),D3DFMT_INDEX16,false);
	/*
	m_lpD3DDevice->CreateVertexBuffer( m_dwMaxAmount * 4 * sizeof(LVertex), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, LVERTEXFVF,
										D3DPOOL_DEFAULT , &m_lpVertices );
	m_lpD3DDevice->CreateVertexBuffer( m_dwMaxAmount * 4 * sizeof(LVertex), D3DUSAGE_DYNAMIC |D3DUSAGE_WRITEONLY, LVERTEXFVF,
										D3DPOOL_DEFAULT, &m_lpVerticesBlend );
	m_lpD3DDevice->CreateIndexBuffer( m_dwMaxAmount * 2 * 3 * sizeof(unsigned short), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
										D3DPOOL_MANAGED, &m_lpVerticeIndex);*/
	m_lpVerticeInfo = new Particle[m_dwMaxAmount];

	unsigned short *pVerticeIndex;
//	m_lpVerticeIndex->Lock(0, m_dwMaxAmount * 2 * 3 * sizeof(unsigned short), (unsigned char **)&pVerticeIndex, 0);
	m_lpVerticeIndex->Lock(0, 0, (unsigned char **)&pVerticeIndex, 0);

	for(i = 0; i < m_dwMaxAmount; i++)
	{
		m_lpVerticeInfo[i].fLifetime = -1.0f;

		pVerticeIndex[i * 6 + 0] = i * 4 + 0;
		pVerticeIndex[i * 6 + 1] = i * 4 + 1;
		pVerticeIndex[i * 6 + 2] = i * 4 + 2;

		pVerticeIndex[i * 6 + 3] = i * 4 + 1;
		pVerticeIndex[i * 6 + 4] = i * 4 + 3;
		pVerticeIndex[i * 6 + 5] = i * 4 + 2;
	}

	m_lpVerticeIndex->Unlock();

	return TRUE;
}

BOOL CX3DEffectParticle::CreateParticle(LPParticle lpParticle)
{
	unsigned long i;

	if(m_lpVerticeInfo == NULL)
		return FALSE;

	for(i = 0; i < m_dwMaxAmount; i++)
	{
		if(m_lpVerticeInfo[i].fLifetime > 0.0f) continue;

		m_lpVerticeInfo[i].vecPos = lpParticle->vecPos;
		m_lpVerticeInfo[i].vecVel = lpParticle->vecVel;
		m_lpVerticeInfo[i].lColor = lpParticle->lColor;
		m_lpVerticeInfo[i].fRotation = lpParticle->fRotation;
		m_lpVerticeInfo[i].dwRotationCount = rand() % 128;
		m_lpVerticeInfo[i].fWidth = lpParticle->fWidth;
		m_lpVerticeInfo[i].fHeight = lpParticle->fHeight;
		m_lpVerticeInfo[i].fMaxLife = m_lpVerticeInfo[i].fLifetime = lpParticle->fLifetime;
		m_lpVerticeInfo[i].fTexFrame = lpParticle->fTexFrame;

		return TRUE;
	}

	return FALSE;
}

void CX3DEffectParticle::SetVolumeNone(void)
{
	m_dwVolumeType = 0;
}

void CX3DEffectParticle::SetVolumeSquare(float fVolX, float fVolY, float fVolZ)
{
	m_dwVolumeType = 1;
	m_fVolX = fVolX;
	m_fVolY = fVolY;
	m_fVolZ = fVolZ;
}

void CX3DEffectParticle::SetVolumeCircle(float fRadius, float fInnerRadius)
{
	m_dwVolumeType = 2;
	m_fRadius = fRadius;
	m_fInnerRadius = fInnerRadius;
}

void CX3DEffectParticle::Render(void)
{
	if(!m_bVisibility)
		return;

	if(m_lpVertices == NULL) return;
	DWORD cullmode,zmode;
	DWORD amode;
	matrix matWorld;
	matWorld.MakeIdent();
	m_lpD3DDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX *)&matWorld);
	m_lpD3DDevice->GetRenderState(D3DRS_ZWRITEENABLE,&zmode);

	//m_lpD3DDevice->SetRenderState(D3DRS_ZENABLE,FALSE);

	m_lpD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
	m_lpD3DDevice->SetTexture(0, GetTexture());
	m_lpD3DDevice->GetRenderState(D3DRS_ALPHABLENDENABLE,&amode);
	m_lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);

	m_lpD3DDevice->SetRenderState(D3DRS_SRCBLEND, m_dwSrcBlending);
	m_lpD3DDevice->SetRenderState(D3DRS_DESTBLEND, m_dwDestBlending);
	m_lpD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	
	m_lpD3DDevice->GetRenderState(D3DRS_CULLMODE,&cullmode);
	m_lpD3DDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);

	m_lpD3DDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
	m_lpD3DDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TEXTURE);	
	m_lpD3DDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
	m_lpD3DDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
	m_lpD3DDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TEXTURE);
	m_lpD3DDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
	
	m_lpD3DDevice->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
	m_lpD3DDevice->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);

	m_lpD3DDevice->SetStreamSource(0, m_lpVertices, sizeof(LVertex));
	m_lpD3DDevice->SetVertexShader(LVERTEXFVF);
	m_lpD3DDevice->SetIndices(m_lpVerticeIndex, 0);
	if(m_dwDrawAmount)
		m_lpD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_dwDrawAmount * 4, 0, m_dwDrawAmount * 2);
/**/
	if(m_fTexSpeed)
	{
//		m_lpD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
//		m_lpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		m_lpD3DDevice->SetStreamSource(0, m_lpVerticesBlend, sizeof(LVertex));
		m_lpD3DDevice->SetVertexShader(LVERTEXFVF);
		m_lpD3DDevice->SetIndices(m_lpVerticeIndex, 0);
		if(m_dwDrawAmount)
			m_lpD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_dwDrawAmount * 4, 0, m_dwDrawAmount * 2);
	}//***/
	m_lpD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,zmode);
	m_lpD3DDevice->SetRenderState(D3DRS_CULLMODE,cullmode);
	m_lpD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,amode);
	m_lpD3DDevice->SetRenderState(D3DRS_ZENABLE,TRUE);

}

BOOL CX3DEffectParticle::Interpolation(float fFrame)
{

	float local_volx; 
	float local_voly;
	float local_volz;
	float local_rad;
	float local_inrad;

	if(m_lpVertices == NULL) return FALSE;

///////////////////////////////////////////////////////////////////////////////////
	float fTempFrame;

	fTempFrame = ((CX3DEffect *)m_lpLocalEffect)->GetFrame();
	if(m_StopParticleFrame != -1) { 
		// extension3 :: mid effect 의 파티클이 end effect 가 끝나더라도 계속 자신의 생명이 
		// 끝날때 까지 유지 	
		
		if(!m_lstCenter.Interpolation(fTempFrame, m_vecCenter)) return FALSE;
		if(!m_lstEForce.Interpolation(fTempFrame, m_vecEForce)) return FALSE;
		if(!m_lstAlpha.Interpolation(fTempFrame, m_fAlpha)) return FALSE;
		if(!m_lstAxis.InterpolationQ(fTempFrame, m_quatAxis)) return FALSE;
		if(!m_lstDirection.InterpolationQ(fTempFrame, m_quatDir)) return FALSE;
		if(!m_lstPower.Interpolation(fTempFrame, m_fPower)) return FALSE;
		if(!m_lstAngle.Interpolation(fTempFrame, m_fAngle)) return FALSE;

		if(m_Scale[0] != 1.0f) { 
			m_vecCenter.x *=m_Scale[0];
			m_vecCenter.z *=m_Scale[0];
			m_vecCenter.y *=(m_Scale[0]);
		}
		if(m_bWorldEffect) {
			if(m_Scale[0] != 1.0f) {
				
				m_fPower *=m_Scale[0];
				m_vecEForce.x *=m_Scale[0];
				m_vecEForce.y *=m_Scale[0];
				m_vecEForce.z *=m_Scale[0];
				
				m_fAngle *=m_Scale[0];
				
				local_volx= m_fVolX * m_Scale[0];
				local_voly = m_fVolY * m_Scale[0];
				local_volz = m_fVolZ * m_Scale[0];
				local_rad = m_fRadius * m_Scale[0];
				local_inrad = m_fInnerRadius * m_Scale[0];

			}
			else {
				local_volx = m_fVolX;
				local_voly = m_fVolY;
				local_volz = m_fVolZ;
				local_rad = m_fRadius;
				local_inrad = m_fInnerRadius;

			}
		}
		else {
			local_volx = m_fVolX;
			local_voly = m_fVolY;
			local_volz = m_fVolZ;
			local_rad = m_fRadius;
			local_inrad = m_fInnerRadius;

		}
		if(m_bVisibility) {
			matrix *matView = ((CX3DEffect *)m_lpLocalEffect)->GetViewMatrix();

			LVertex *pVertices, *pVerticesBlend;
//			if(FAILED( m_lpVertices->Lock( 0, m_dwMaxAmount * 4 * sizeof(LVertex), (unsigned char **)&pVertices, D3DLOCK_DISCARD ) ) )
			if(FAILED( m_lpVertices->Lock( 0, 0, (unsigned char **)&pVertices, 0 ) ) )
				return FALSE;
//			if(FAILED( m_lpVerticesBlend->Lock( 0, m_dwMaxAmount * 4 * sizeof(LVertex), (unsigned char **)&pVerticesBlend, D3DLOCK_DISCARD ) ) )
			if(FAILED( m_lpVerticesBlend->Lock( 0, 0, (unsigned char **)&pVerticesBlend, 0 ) ) )
				return FALSE;

			m_dwDrawAmount = 0;
			unsigned long mul;
			float w, h, d, degree, cosx, siny;
			vector3 vecCenter, pVer[4];
			
			if(m_lpVerticeInfo == NULL)
				return FALSE;

			for(unsigned long i = 0; i < m_dwMaxAmount; i++)
			{
			
				if(m_lpVerticeInfo[i].fLifetime < 0.0f) continue;
				else if(m_lpVerticeInfo[i].fLifetime < ((CX3DEffect *)m_lpLocalEffect)->GetIncFrame())
					m_lpVerticeInfo[i].fLifetime = 0.0f;
			
				m_lpVerticeInfo[i].vecVel += m_vecEForce;
				m_lpVerticeInfo[i].vecPos += m_lpVerticeInfo[i].vecVel;
				
				if(!ParticleInterpolation(i)) return FALSE;

				w = m_lpVerticeInfo[i].fWidth / 2;
				h = m_lpVerticeInfo[i].fHeight / 2;

				d = m_lpVerticeInfo[i].fRotation * m_lpVerticeInfo[i].dwRotationCount;
				degree = d - (((unsigned long)(d / 360)) * 360.0f);
				cosx = cosf(degree);
				siny = sinf(degree);

				mul = m_dwDrawAmount << 2;

				pVer[0] = vector3(-w * cosx - h * siny, -w * siny + h * cosx, 0.0f);
				pVer[1] = vector3(w * cosx - h * siny, w * siny + h * cosx, 0.0f);
				pVer[2] = vector3(-w * cosx + h * siny, -w * siny - h * cosx, 0.0f);
				pVer[3] = vector3(w * cosx + h * siny, w * siny - h * cosx, 0.0f);

		/*		if(((CX3DEffect *)m_lpLocalEffect)->GetAxis())
				{
					z3d::VectorRotate(pVer[0], pVer[0], *((CX3DEffect *)m_lpLocalEffect)->GetAxis());
					z3d::VectorRotate(pVer[1], pVer[1], *((CX3DEffect *)m_lpLocalEffect)->GetAxis());
					z3d::VectorRotate(pVer[2], pVer[2], *((CX3DEffect *)m_lpLocalEffect)->GetAxis());
					z3d::VectorRotate(pVer[3], pVer[3], *((CX3DEffect *)m_lpLocalEffect)->GetAxis());
				}*/

				pVertices[mul + 0].v = vector3(pVer[0].x * matView->_11 + pVer[0].y * matView->_12, pVer[0].x * matView->_21 + pVer[0].y * matView->_22, pVer[0].x * matView->_31 + pVer[0].y * matView->_32);
				pVertices[mul + 1].v = vector3(pVer[1].x * matView->_11 + pVer[1].y * matView->_12, pVer[1].x * matView->_21 + pVer[1].y * matView->_22, pVer[1].x * matView->_31 + pVer[1].y * matView->_32);
				pVertices[mul + 2].v = vector3(pVer[2].x * matView->_11 + pVer[2].y * matView->_12, pVer[2].x * matView->_21 + pVer[2].y * matView->_22, pVer[2].x * matView->_31 + pVer[2].y * matView->_32);
				pVertices[mul + 3].v = vector3(pVer[3].x * matView->_11 + pVer[3].y * matView->_12, pVer[3].x * matView->_21 + pVer[3].y * matView->_22, pVer[3].x * matView->_31 + pVer[3].y * matView->_32);
				
				pVertices[mul + 0].v += m_lpVerticeInfo[i].vecPos;
				pVertices[mul + 1].v += m_lpVerticeInfo[i].vecPos;
				pVertices[mul + 2].v += m_lpVerticeInfo[i].vecPos;
				pVertices[mul + 3].v += m_lpVerticeInfo[i].vecPos;
				
				pVertices[mul + 0].diff = pVertices[mul + 1].diff =
				pVertices[mul + 2].diff = pVertices[mul + 3].diff = m_lpVerticeInfo[i].lColor;

				pVertices[mul + 3].diff.a *= m_fAlpha;
				pVertices[mul + 0].diff.a = pVertices[mul + 1].diff.a =
				pVertices[mul + 2].diff.a = pVertices[mul + 3].diff.a;

				
				pVertices[mul + 0].spec = pVertices[mul + 1].spec =
				pVertices[mul + 2].spec = pVertices[mul + 3].spec = color(0xFF, 0xFF, 0xFF, 0xFF);

				if(m_fTexSpeed)
				{
					pVerticesBlend[mul + 0].v = pVertices[mul + 0].v;
					pVerticesBlend[mul + 1].v = pVertices[mul + 1].v;
					pVerticesBlend[mul + 2].v = pVertices[mul + 2].v;
					pVerticesBlend[mul + 3].v = pVertices[mul + 3].v;

					pVerticesBlend[mul + 0].diff = pVerticesBlend[mul + 1].diff =
					pVerticesBlend[mul + 2].diff = pVerticesBlend[mul + 3].diff =	pVertices[mul + 0].diff;

					pVerticesBlend[mul + 0].spec = pVerticesBlend[mul + 1].spec =
					pVerticesBlend[mul + 2].spec = pVerticesBlend[mul + 3].spec =	pVertices[mul + 0].spec;

					long t = (long)m_lpVerticeInfo[i].fTexFrame;
					float f1 = (t % 4) * 0.25;
					float f2 = (t / 4) * 0.25;
					pVertices[mul + 0].tu = f1;			pVertices[mul + 0].tv = f2;
					pVertices[mul + 1].tu = f1 + 0.25f;	pVertices[mul + 1].tv = f2;
					pVertices[mul + 2].tu = f1;			pVertices[mul + 2].tv = f2 + 0.25f;
					pVertices[mul + 3].tu = f1 + 0.25f;	pVertices[mul + 3].tv = f2 + 0.25f;

					if(15 == t) { if(0.16f < m_fTexSpeed) t = 0; else t = 15; } else t++;
					f1 = (t % 4) * 0.25;
					f2 = (t / 4) * 0.25;
					pVerticesBlend[mul + 0].tu = f1;			pVerticesBlend[mul + 0].tv = f2;
					pVerticesBlend[mul + 1].tu = f1 + 0.25f;	pVerticesBlend[mul + 1].tv = f2;
					pVerticesBlend[mul + 2].tu = f1;			pVerticesBlend[mul + 2].tv = f2 + 0.25f;
					pVerticesBlend[mul + 3].tu = f1 + 0.25f;	pVerticesBlend[mul + 3].tv = f2 + 0.25f;

		///////////////////////////////////////////
		//			float fAlpha = ;
					pVertices[mul + 3].diff.a *= (floorf(m_lpVerticeInfo[i].fTexFrame + 1.0f) - m_lpVerticeInfo[i].fTexFrame);
					pVertices[mul + 0].diff.a = pVertices[mul + 1].diff.a = pVertices[mul + 2].diff.a = pVertices[mul + 3].diff.a;

					pVerticesBlend[mul + 3].diff.a *= (m_lpVerticeInfo[i].fTexFrame - floorf(m_lpVerticeInfo[i].fTexFrame));
		//			pVerticesBlend[mul + 3].diff.a *= (1.0f - fAlpha);
					pVerticesBlend[mul + 0].diff.a = pVerticesBlend[mul + 1].diff.a = pVerticesBlend[mul + 2].diff.a = pVerticesBlend[mul + 3].diff.a;
		///////////////////////////////////////////
				} else
				{
					pVertices[mul + 0].tu = 0.0f;	pVertices[mul + 0].tv = 0.0f;
					pVertices[mul + 1].tu = 1.0f;	pVertices[mul + 1].tv = 0.0f;
					pVertices[mul + 2].tu = 0.0f;	pVertices[mul + 2].tv = 1.0f;
					pVertices[mul + 3].tu = 1.0f;	pVertices[mul + 3].tv = 1.0f;
				}
				
				m_lpVerticeInfo[i].fLifetime -= ((CX3DEffect *)m_lpLocalEffect)->GetIncFrame();
				m_lpVerticeInfo[i].dwRotationCount++;

				// 알파값이 5이하이면 draw list에서 제외
				if((pVertices[mul +3].diff.a)<1) {
					continue;
				}
				
				m_dwDrawAmount++;

				
			}
		///////////////////////////////////////////////////////////////////////////////////
			m_lpVerticesBlend->Unlock();
			m_lpVertices->Unlock();
		}	//if(m_bVisibility)

	}
	else { // 일반 particle

		if(!m_lstCenter.Interpolation(fTempFrame, m_vecCenter)) return FALSE;
		if(!m_lstEForce.Interpolation(fTempFrame, m_vecEForce)) return FALSE;
		if(!m_lstAlpha.Interpolation(fTempFrame, m_fAlpha)) return FALSE;
		if(!m_lstAxis.InterpolationQ(fTempFrame, m_quatAxis)) return FALSE;
		if(!m_lstDirection.InterpolationQ(fTempFrame, m_quatDir)) return FALSE;
		if(!m_lstPower.Interpolation(fTempFrame, m_fPower)) return FALSE;
		if(!m_lstAngle.Interpolation(fTempFrame, m_fAngle)) return FALSE;
	
		if(m_Scale[0] != 1.0f) { 
			m_vecCenter.x *=m_Scale[0];
			m_vecCenter.z *=m_Scale[0];
			m_vecCenter.y *=(m_Scale[0]);
		}
		if(m_bWorldEffect) {
			if(m_Scale[0] != 1.0f) {
				
				m_fPower *=m_Scale[0];
				m_vecEForce.x *=m_Scale[0];
				m_vecEForce.y *=m_Scale[0];
				m_vecEForce.z *=m_Scale[0];
				
				m_fAngle *=m_Scale[0];
				
				local_volx= m_fVolX * m_Scale[0];
				local_voly = m_fVolY * m_Scale[0];
				local_volz = m_fVolZ * m_Scale[0];
				local_rad = m_fRadius * m_Scale[0];
				local_inrad = m_fInnerRadius * m_Scale[0];

			}
			else {
				local_volx = m_fVolX;
				local_voly = m_fVolY;
				local_volz = m_fVolZ;
				local_rad = m_fRadius;
				local_inrad = m_fInnerRadius;

			}
		}
		else {
			local_volx = m_fVolX;
			local_voly = m_fVolY;
			local_volz = m_fVolZ;
			local_rad = m_fRadius;
			local_inrad = m_fInnerRadius;

		}
	///////////////////////////////////////////////////////////////////////////////////
		if(m_bVisibility) {
			float fAmount;
			Particle pNewParticle;
			
			if(m_lstAmount.Find(fTempFrame, fAmount))
			{
				float a, b, r, c, a1, p;

				a1 = FLOAT_PHI / fAmount;
				c = ((2 * FLOAT_PHI) / fAmount);

				for(unsigned long i = 0; i < (unsigned long)fAmount; i++)
				{
					p = m_fPower * (1.0f - m_fPowerSeed * (1.0f - (((float)rand()) / RAND_MAX) * 2.0f));
					a = (m_fAngle * (((float)rand()) / RAND_MAX));
					b = c * i;
					r = p * sinf(a);

					pNewParticle.vecVel = vector3(r * cosf(b), p * cosf(a), r * sinf(b));
					z3d::VectorRotate(pNewParticle.vecVel, pNewParticle.vecVel, m_quatDir);
					z3d::VectorRotate(pNewParticle.vecVel, pNewParticle.vecVel, m_quatAxis);
					if(((CX3DEffect *)m_lpLocalEffect)->GetAxis())
						z3d::VectorRotate(pNewParticle.vecVel, pNewParticle.vecVel, *((CX3DEffect *)m_lpLocalEffect)->GetAxis());
					pNewParticle.fMaxLife = pNewParticle.fLifetime = m_fLifetime * (1.0f - m_fLifetimeSeed * (1.0f - (((float)rand()) / RAND_MAX) * 2.0f));

					pNewParticle.fRotation = m_fRotation * (1.0f - m_fPowerSeed * (1.0f - (((float)rand()) / RAND_MAX) * 2.0f));
					pNewParticle.fTexFrame = 01.0;

					switch(m_dwVolumeType)
					{
						case 0:
							pNewParticle.vecPos = m_vecCenter;
							break;

						case 1:
							pNewParticle.vecPos = vector3(((float)rand() / RAND_MAX) * local_volx,
								((float)rand() / RAND_MAX) * local_voly,
								((float)rand() / RAND_MAX) * local_volz) - vector3(local_volx / 2, local_voly / 2, local_volz / 2);
							z3d::VectorRotate(pNewParticle.vecPos, pNewParticle.vecPos, m_quatAxis);
							pNewParticle.vecPos += m_vecCenter;
							break;

						case 2:
							{
								float degree = 2.0f * FLOAT_PHI * (((float)rand()) / RAND_MAX);
								float radius = local_inrad + (local_rad - local_inrad * (((float)rand()) / RAND_MAX));

								pNewParticle.vecPos = vector3(radius * cosf(degree), 0, radius * sinf(degree));
								z3d::VectorRotate(pNewParticle.vecPos, pNewParticle.vecPos, m_quatAxis);
								pNewParticle.vecPos += m_vecCenter;
							}
							break;
					}
					if(((CX3DEffect *)m_lpLocalEffect)->GetAxis()) z3d::VectorRotate(pNewParticle.vecPos, pNewParticle.vecPos, *((CX3DEffect *)m_lpLocalEffect)->GetAxis());
					if(((CX3DEffect *)m_lpLocalEffect)->GetCenter()) pNewParticle.vecPos += (*((CX3DEffect *)m_lpLocalEffect)->GetCenter());

					CreateParticle(&pNewParticle);
				}
			}
		///////////////////////////////////////////////////////////////////////////////////
			
			matrix *matView = ((CX3DEffect *)m_lpLocalEffect)->GetViewMatrix();

			LVertex *pVertices, *pVerticesBlend;
//			if(FAILED( m_lpVertices->Lock( 0, m_dwMaxAmount * 4 * sizeof(LVertex), (unsigned char **)&pVertices, 0 ) ) )
			if(FAILED( m_lpVertices->Lock( 0, 0, (unsigned char **)&pVertices, 0 ) ) )
				return FALSE;
//			if(FAILED( m_lpVerticesBlend->Lock( 0, m_dwMaxAmount * 4 * sizeof(LVertex), (unsigned char **)&pVerticesBlend, 0 ) ) )
			if(FAILED( m_lpVerticesBlend->Lock( 0, 0, (unsigned char **)&pVerticesBlend, 0 ) ) )
				return FALSE;

			m_dwDrawAmount = 0;
			unsigned long mul;
			float w, h, d, degree, cosx, siny;
			vector3 vecCenter, pVer[4];

			if(m_lpVerticeInfo == NULL)
				return FALSE;

			for(unsigned long i = 0; i < m_dwMaxAmount; i++)
			{
			
				if(m_lpVerticeInfo[i].fLifetime < 0.0f) continue;
				else if(m_lpVerticeInfo[i].fLifetime < ((CX3DEffect *)m_lpLocalEffect)->GetIncFrame())
					m_lpVerticeInfo[i].fLifetime = 0.0f;
			
				m_lpVerticeInfo[i].vecVel += m_vecEForce;
				m_lpVerticeInfo[i].vecPos += m_lpVerticeInfo[i].vecVel;
			
				if(!ParticleInterpolation(i)) return FALSE;

				w = m_lpVerticeInfo[i].fWidth / 2;
				h = m_lpVerticeInfo[i].fHeight / 2;

				d = m_lpVerticeInfo[i].fRotation * m_lpVerticeInfo[i].dwRotationCount;
				degree = d - (((unsigned long)(d / 360)) * 360.0f);
				cosx = cosf(degree);
				siny = sinf(degree);

				mul = m_dwDrawAmount << 2;

				pVer[0] = vector3(-w * cosx - h * siny, -w * siny + h * cosx, 0.0f);
				pVer[1] = vector3(w * cosx - h * siny, w * siny + h * cosx, 0.0f);
				pVer[2] = vector3(-w * cosx + h * siny, -w * siny - h * cosx, 0.0f);
				pVer[3] = vector3(w * cosx + h * siny, w * siny - h * cosx, 0.0f);

		/*		if(((CX3DEffect *)m_lpLocalEffect)->GetAxis())
				{
					z3d::VectorRotate(pVer[0], pVer[0], *((CX3DEffect *)m_lpLocalEffect)->GetAxis());
					z3d::VectorRotate(pVer[1], pVer[1], *((CX3DEffect *)m_lpLocalEffect)->GetAxis());
					z3d::VectorRotate(pVer[2], pVer[2], *((CX3DEffect *)m_lpLocalEffect)->GetAxis());
					z3d::VectorRotate(pVer[3], pVer[3], *((CX3DEffect *)m_lpLocalEffect)->GetAxis());
				}*/

				pVertices[mul + 0].v = vector3(pVer[0].x * matView->_11 + pVer[0].y * matView->_12, pVer[0].x * matView->_21 + pVer[0].y * matView->_22, pVer[0].x * matView->_31 + pVer[0].y * matView->_32);
				pVertices[mul + 1].v = vector3(pVer[1].x * matView->_11 + pVer[1].y * matView->_12, pVer[1].x * matView->_21 + pVer[1].y * matView->_22, pVer[1].x * matView->_31 + pVer[1].y * matView->_32);
				pVertices[mul + 2].v = vector3(pVer[2].x * matView->_11 + pVer[2].y * matView->_12, pVer[2].x * matView->_21 + pVer[2].y * matView->_22, pVer[2].x * matView->_31 + pVer[2].y * matView->_32);
				pVertices[mul + 3].v = vector3(pVer[3].x * matView->_11 + pVer[3].y * matView->_12, pVer[3].x * matView->_21 + pVer[3].y * matView->_22, pVer[3].x * matView->_31 + pVer[3].y * matView->_32);

				pVertices[mul + 0].v += m_lpVerticeInfo[i].vecPos;
				pVertices[mul + 1].v += m_lpVerticeInfo[i].vecPos;
				pVertices[mul + 2].v += m_lpVerticeInfo[i].vecPos;
				pVertices[mul + 3].v += m_lpVerticeInfo[i].vecPos;

				pVertices[mul + 0].diff = pVertices[mul + 1].diff =
				pVertices[mul + 2].diff = pVertices[mul + 3].diff = m_lpVerticeInfo[i].lColor;

				pVertices[mul + 3].diff.a *= m_fAlpha;
				pVertices[mul + 0].diff.a = pVertices[mul + 1].diff.a =
				pVertices[mul + 2].diff.a = pVertices[mul + 3].diff.a;

				
				pVertices[mul + 0].spec = pVertices[mul + 1].spec =
				pVertices[mul + 2].spec = pVertices[mul + 3].spec = color(0xFF, 0xFF, 0xFF, 0xFF);

				if(m_fTexSpeed)
				{
					pVerticesBlend[mul + 0].v = pVertices[mul + 0].v;
					pVerticesBlend[mul + 1].v = pVertices[mul + 1].v;
					pVerticesBlend[mul + 2].v = pVertices[mul + 2].v;
					pVerticesBlend[mul + 3].v = pVertices[mul + 3].v;

					pVerticesBlend[mul + 0].diff = pVerticesBlend[mul + 1].diff =
					pVerticesBlend[mul + 2].diff = pVerticesBlend[mul + 3].diff =	pVertices[mul + 0].diff;

					pVerticesBlend[mul + 0].spec = pVerticesBlend[mul + 1].spec =
					pVerticesBlend[mul + 2].spec = pVerticesBlend[mul + 3].spec =	pVertices[mul + 0].spec;

					long t = (long)m_lpVerticeInfo[i].fTexFrame;
					float f1 = (t % 4) * 0.25;
					float f2 = (t / 4) * 0.25;
					pVertices[mul + 0].tu = f1;			pVertices[mul + 0].tv = f2;
					pVertices[mul + 1].tu = f1 + 0.25f;	pVertices[mul + 1].tv = f2;
					pVertices[mul + 2].tu = f1;			pVertices[mul + 2].tv = f2 + 0.25f;
					pVertices[mul + 3].tu = f1 + 0.25f;	pVertices[mul + 3].tv = f2 + 0.25f;

					if(15 == t) { if(0.16f < m_fTexSpeed) t = 0; else t = 15; } else t++;
					f1 = (t % 4) * 0.25;
					f2 = (t / 4) * 0.25;
					pVerticesBlend[mul + 0].tu = f1;			pVerticesBlend[mul + 0].tv = f2;
					pVerticesBlend[mul + 1].tu = f1 + 0.25f;	pVerticesBlend[mul + 1].tv = f2;
					pVerticesBlend[mul + 2].tu = f1;			pVerticesBlend[mul + 2].tv = f2 + 0.25f;
					pVerticesBlend[mul + 3].tu = f1 + 0.25f;	pVerticesBlend[mul + 3].tv = f2 + 0.25f;

		///////////////////////////////////////////
		//			float fAlpha = ;
					pVertices[mul + 3].diff.a *= (floorf(m_lpVerticeInfo[i].fTexFrame + 1.0f) - m_lpVerticeInfo[i].fTexFrame);
					pVertices[mul + 0].diff.a = pVertices[mul + 1].diff.a = pVertices[mul + 2].diff.a = pVertices[mul + 3].diff.a;

					pVerticesBlend[mul + 3].diff.a *= (m_lpVerticeInfo[i].fTexFrame - floorf(m_lpVerticeInfo[i].fTexFrame));
		//			pVerticesBlend[mul + 3].diff.a *= (1.0f - fAlpha);
					pVerticesBlend[mul + 0].diff.a = pVerticesBlend[mul + 1].diff.a = pVerticesBlend[mul + 2].diff.a = pVerticesBlend[mul + 3].diff.a;
		///////////////////////////////////////////
				} else
				{
					pVertices[mul + 0].tu = 0.0f;	pVertices[mul + 0].tv = 0.0f;
					pVertices[mul + 1].tu = 1.0f;	pVertices[mul + 1].tv = 0.0f;
					pVertices[mul + 2].tu = 0.0f;	pVertices[mul + 2].tv = 1.0f;
					pVertices[mul + 3].tu = 1.0f;	pVertices[mul + 3].tv = 1.0f;
				}
				
				m_lpVerticeInfo[i].fLifetime -= ((CX3DEffect *)m_lpLocalEffect)->GetIncFrame();
				m_lpVerticeInfo[i].dwRotationCount++;

				// 알파값이 1이하이면 draw list에서 제외
				if((pVertices[mul +3].diff.a)<1) {
					continue;
				}
				
				m_dwDrawAmount++;

				
			}
		///////////////////////////////////////////////////////////////////////////////////
			m_lpVerticesBlend->Unlock();
			m_lpVertices->Unlock();
		}
	}	//if(m_bVisibility)
	return TRUE;
}

BOOL CX3DEffectParticle::ParticleInterpolation(unsigned long dwNumber)
{
	if(m_lpVerticeInfo == NULL)
		return FALSE;

	float fFrame = 100.0f * (1.0f - (m_lpVerticeInfo[dwNumber].fLifetime / m_lpVerticeInfo[dwNumber].fMaxLife));

	if(!m_lstWidth.Interpolation(fFrame, m_lpVerticeInfo[dwNumber].fWidth)) return FALSE;
	if(!m_lstHeight.Interpolation(fFrame, m_lpVerticeInfo[dwNumber].fHeight)) return FALSE;
	m_lpVerticeInfo[dwNumber].fWidth *=m_Scale[0];
	m_lpVerticeInfo[dwNumber].fHeight *=m_Scale[0];

	if(!m_lstColor.InterpolationC(fFrame, m_lpVerticeInfo[dwNumber].lColor)) return FALSE;

	if(m_fTexSpeed)
	{
		if(fFrame == 100.0f)
		{
			m_lpVerticeInfo[dwNumber].fTexFrame = 15.0f;
		} else
		{
			float a = fFrame * m_fTexSpeed;
			m_lpVerticeInfo[dwNumber].fTexFrame = a - (((unsigned long)(a / 16)) * 16.0f);
		}
	}

	return TRUE;
}

void CX3DEffectParticle::Load(FILE *fp, const char *strOriginalPath)
{
	fread(&m_bTexAni, 4, 1, fp);
	fread(&m_dwSrcBlending, 4, 1, fp);
	fread(&m_dwDestBlending, 4, 1, fp);

	fread(&m_fPowerSeed, 4, 1, fp);
	fread(&m_fLifetime, 4, 1, fp);
	fread(&m_fLifetimeSeed, 4, 1, fp);

	fread(&m_fRotation, 4, 1, fp);
	fread(&m_fTexSpeed, 4, 1, fp);

// 볼륨
	fread(&m_dwVolumeType, 4, 1, fp);		// 0 : None, 1 : Square, 2 : Circle
	fread(&m_fVolX, 4, 1, fp);
	fread(&m_fVolY, 4, 1, fp);
	fread(&m_fVolZ, 4, 1, fp);
	fread(&m_fRadius, 4, 1, fp);
	fread(&m_fInnerRadius, 4, 1, fp);
	if(m_bWorldEffect) {
		if(m_Scale[0] != 1.0f) {
			m_fVolX *=m_Scale[0];
			m_fVolY *=m_Scale[0];
			m_fVolZ *=m_Scale[0];
			m_fRadius *=m_Scale[0];
			m_fInnerRadius *=m_Scale[0];
		}
	}
	m_lstAxis.Load(fp, m_quatAxis);
	m_lstCenter.Load(fp, m_vecCenter);
	if(m_Scale[0] != 1.0f) {
		m_vecCenter.x *=m_Scale[0];
		m_vecCenter.z *=m_Scale[0];
		m_vecCenter.y *=(m_Scale[0]);
	}
	float fAmount;
	m_lstAmount.Load(fp, fAmount);
	m_lstAlpha.Load(fp, m_fAlpha);
	m_lstPower.Load(fp, m_fPower);
	m_lstAngle.Load(fp, m_fAngle);
	m_lstEForce.Load(fp, m_vecEForce);
	if(m_bWorldEffect) {
		if(m_Scale[0] != 1.0f) {
			m_fPower *=m_Scale[0];
			m_vecEForce.x *=m_Scale[0];
			m_vecEForce.y *=m_Scale[0];
			m_vecEForce.z *=m_Scale[0];
		}
	}
	m_lstDirection.Load(fp, m_quatDir);

// Scenario용 리스트
	m_lstWidth.Load(fp, m_fAlpha);
	m_lstHeight.Load(fp, m_fAlpha);
	m_lstColor.Load(fp, m_lColor);
}

void CX3DEffectParticle::Save(FILE *fp, const char *strOriginalPath)
{
	fwrite(&m_bTexAni, 4, 1, fp);
	fwrite(&m_dwSrcBlending, 4, 1, fp);
	fwrite(&m_dwDestBlending, 4, 1, fp);

	fwrite(&m_fPowerSeed, 4, 1, fp);
	fwrite(&m_fLifetime, 4, 1, fp);
	fwrite(&m_fLifetimeSeed, 4, 1, fp);

	fwrite(&m_fRotation, 4, 1, fp);
	fwrite(&m_fTexSpeed, 4, 1, fp);

// 볼륨
	fwrite(&m_dwVolumeType, 4, 1, fp);		// 0 : None, 1 : Square, 2 : Circle
	fwrite(&m_fVolX, 4, 1, fp);
	fwrite(&m_fVolY, 4, 1, fp);
	fwrite(&m_fVolZ, 4, 1, fp);
	fwrite(&m_fRadius, 4, 1, fp);
	fwrite(&m_fInnerRadius, 4, 1, fp);

	m_lstAxis.Save(fp);
	m_lstCenter.Save(fp);

	m_lstAmount.Save(fp);
	m_lstAlpha.Save(fp);
	m_lstPower.Save(fp);
	m_lstAngle.Save(fp);
	m_lstEForce.Save(fp);
	m_lstDirection.Save(fp);

// Scenario용 리스트
	m_lstWidth.Save(fp);
	m_lstHeight.Save(fp);
	m_lstColor.Save(fp);
}
