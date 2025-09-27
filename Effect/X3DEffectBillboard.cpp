// X3DEffectBillboard.cpp: implementation of the CX3DEffectBillboard class.
//
//////////////////////////////////////////////////////////////////////

#include "X3DEffect.h"
#include "X3DEffectBillboard.h"
#include "SceneManager.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CX3DEffectBillboard::CX3DEffectBillboard()
{
	m_dwSrcBlending = D3DBLEND_SRCALPHA;
	m_dwDestBlending = D3DBLEND_ONE;

	m_bTexAni = FALSE;

	m_lpVertices = NULL;
	m_lpVerticesBlend = NULL;
}

CX3DEffectBillboard::~CX3DEffectBillboard()
{
	
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

//	if(m_lpVerticesBlend) { m_lpVerticesBlend->Release(); m_lpVerticesBlend = NULL; }
//	if(m_lpVertices) { m_lpVertices->Release(); m_lpVertices = NULL; }
}

void CX3DEffectBillboard::Create(unsigned long dwStartFrame, unsigned long dwEndFrame)
{
	m_dwStartFrame = dwStartFrame;
	m_dwEndFrame = dwEndFrame;
}

BOOL CX3DEffectBillboard::CreateBuffer(void)
{
//	if(m_lpVerticesBlend) { m_lpVerticesBlend->Release(); m_lpVerticesBlend = NULL; }
//	if(m_lpVertices) { m_lpVertices->Release(); m_lpVertices = NULL; }

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
//	m_lpVertices = CSceneManager::ms_pBufferPools->GetVertexBuffer(4 * sizeof(LVertex),LVERTEXFVF,true);
//	m_lpVerticesBlend = CSceneManager::ms_pBufferPools->GetVertexBuffer(4 * sizeof(LVertex),LVERTEXFVF,true);
	m_lpVertices = CSceneManager::ms_pBufferPools->GetVertexBuffer(4 * sizeof(LVertex),LVERTEXFVF,false);
	m_lpVerticesBlend = CSceneManager::ms_pBufferPools->GetVertexBuffer(4 * sizeof(LVertex),LVERTEXFVF,false);
/*
	m_lpD3DDevice->CreateVertexBuffer( 4 * sizeof(LVertex), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, LVERTEXFVF, D3DPOOL_DEFAULT, &m_lpVertices );
	m_lpD3DDevice->CreateVertexBuffer( 4 * sizeof(LVertex), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, LVERTEXFVF, D3DPOOL_DEFAULT, &m_lpVerticesBlend );
*/
	return TRUE;
}

void CX3DEffectBillboard::Render(void)
{
	if(!m_bVisibility)
		return;

	if(m_lpVertices == NULL) return;

	matrix matWorld;
	matWorld.MakeIdent();
	m_lpD3DDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX *)&matWorld);
	
	m_lpD3DDevice->SetTexture(0, GetTexture());
	m_lpD3DDevice->SetRenderState(D3DRS_SRCBLEND, m_dwSrcBlending);
	m_lpD3DDevice->SetRenderState(D3DRS_DESTBLEND, m_dwDestBlending);
	m_lpD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
	m_lpD3DDevice->SetStreamSource(0, m_lpVertices, sizeof(LVertex));
	m_lpD3DDevice->SetVertexShader(LVERTEXFVF);
	m_lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	if(m_bTexAni)
	{
		m_lpD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_lpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		m_lpD3DDevice->SetStreamSource(0, m_lpVerticesBlend, sizeof(LVertex));
		m_lpD3DDevice->SetVertexShader(LVERTEXFVF);
		m_lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	}/**/
}

BOOL CX3DEffectBillboard::Interpolation(float fFrame)
{
///////////////////////////////////////////////////////////////////////////////////
	if(!m_lstCenter.Interpolation(fFrame, m_vecCenter)) return FALSE;
	if(!m_lstAxis.InterpolationQ(fFrame, m_quatAxis)) return FALSE;
	
	if(!m_lstWidth.Interpolation(fFrame, m_fWidth)) return FALSE;
	if(!m_lstHeight.Interpolation(fFrame, m_fHeight)) return FALSE;

	if(!m_lstColor.InterpolationC(fFrame, m_lColor)) return FALSE;
	if(m_Scale[0] != 1.0f) {
		m_fWidth *=m_Scale[0];
		m_fHeight *=m_Scale[0];

		m_vecCenter.x *=m_Scale[0];
		m_vecCenter.z *=m_Scale[0];
		m_vecCenter.y *=(m_Scale[0]);
	}
	if(m_bTexAni)
	{
		if(!m_lstTexFrame.Interpolation(fFrame, m_fTexFrame)) return FALSE;
	} else
	{
		if(!m_lstStartU.Interpolation(fFrame, m_fStartU)) return FALSE;
		if(!m_lstStartV.Interpolation(fFrame, m_fStartV)) return FALSE;
		if(!m_lstTileU.Interpolation(fFrame, m_fTileU)) return FALSE;
		if(!m_lstTileV.Interpolation(fFrame, m_fTileV)) return FALSE;
	}
	if(m_bVisibility) {
///////////////////////////////////////////////////////////////////////////////////

		{
	///////////////////////////////////////////////////////////////////////////////////
			LVertex *pVertices, *pVerticesBlend;
//			if(FAILED( m_lpVertices->Lock( 0, 4 * sizeof(LVertex), (unsigned char **)&pVertices, D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK) ) ) return FALSE;
//			if(FAILED( m_lpVerticesBlend->Lock( 0, 4 * sizeof(LVertex), (unsigned char **)&pVerticesBlend, D3DLOCK_DISCARD  | D3DLOCK_NOSYSLOCK) ) ) return FALSE;
			if(FAILED( m_lpVertices->Lock( 0, 0, (unsigned char **)&pVertices, 0) ) ) return FALSE;
			if(FAILED( m_lpVerticesBlend->Lock( 0, 0, (unsigned char **)&pVerticesBlend, 0) ) ) return FALSE;

			if(m_bAxisAligned)
			{
				m_vecAxisT = vector3(0.0f, (m_fHeight / 2), 0.0f);
				m_vecEyeT = vector3(0.0f, 0.0f, -1.0f);
				
				z3d::VectorRotate(m_vecAxisT, m_vecAxisT, m_quatAxis);
				
				if(((CX3DEffect *)m_lpLocalEffect)->GetAxis()) z3d::VectorRotate(m_vecAxisT, m_vecAxisT, (*((CX3DEffect *)m_lpLocalEffect)->GetAxis()));
				m_vecEyeT = vector3(m_vecEyeT.x * m_matLocalView->_11 + m_vecEyeT.y * m_matLocalView->_12 + m_vecEyeT.z * m_matLocalView->_13,
									m_vecEyeT.x * m_matLocalView->_21 + m_vecEyeT.y * m_matLocalView->_22 + m_vecEyeT.z * m_matLocalView->_23,
									m_vecEyeT.x * m_matLocalView->_31 + m_vecEyeT.y * m_matLocalView->_32 + m_vecEyeT.z * m_matLocalView->_33);
				m_vecWidthT = m_vecAxisT ^ m_vecEyeT;
				m_vecWidthT.Normalize();
				m_vecWidthT *= (m_fWidth / 2);

				pVertices[0].v = m_vecAxisT + m_vecWidthT + m_vecCenter;
				pVertices[1].v = m_vecAxisT - m_vecWidthT + m_vecCenter;
				pVertices[2].v = -m_vecAxisT + m_vecWidthT + m_vecCenter;
				pVertices[3].v = -m_vecAxisT - m_vecWidthT + m_vecCenter;
			} else
			{
				matrix *matView = ((CX3DEffect *)m_lpLocalEffect)->GetViewMatrix();

				float w = m_fWidth / 2;
				float h = m_fHeight / 2;

				pVertices[0].v = vector3(-w, h, 0.0f);
				pVertices[1].v = vector3(w, h, 0.0f);
				pVertices[2].v = vector3(-w, -h, 0.0f);
				pVertices[3].v = vector3(w, -h, 0.0f);

				vector3 vecCenter;

				z3d::VectorRotate(pVertices[0].v, pVertices[0].v, m_quatAxis);
				z3d::VectorRotate(pVertices[1].v, pVertices[1].v, m_quatAxis);
				z3d::VectorRotate(pVertices[2].v, pVertices[2].v, m_quatAxis);
				z3d::VectorRotate(pVertices[3].v, pVertices[3].v, m_quatAxis);
				if(((CX3DEffect *)m_lpLocalEffect)->GetAxis())
				{
					z3d::VectorRotate(pVertices[0].v, pVertices[0].v, *((CX3DEffect *)m_lpLocalEffect)->GetAxis());
					z3d::VectorRotate(pVertices[1].v, pVertices[1].v, *((CX3DEffect *)m_lpLocalEffect)->GetAxis());
					z3d::VectorRotate(pVertices[2].v, pVertices[2].v, *((CX3DEffect *)m_lpLocalEffect)->GetAxis());
					z3d::VectorRotate(pVertices[3].v, pVertices[3].v, *((CX3DEffect *)m_lpLocalEffect)->GetAxis());

					z3d::VectorRotate(vecCenter, m_vecCenter, *((CX3DEffect *)m_lpLocalEffect)->GetAxis());
				} else
				{
					vecCenter = m_vecCenter;
				}

				pVertices[0].v = vector3(pVertices[0].v.x * matView->_11 + pVertices[0].v.y * matView->_12, pVertices[0].v.x * matView->_21 + pVertices[0].v.y * matView->_22, pVertices[0].v.x * matView->_31 + pVertices[0].v.y * matView->_32);
				pVertices[1].v = vector3(pVertices[1].v.x * matView->_11 + pVertices[1].v.y * matView->_12, pVertices[1].v.x * matView->_21 + pVertices[1].v.y * matView->_22, pVertices[1].v.x * matView->_31 + pVertices[1].v.y * matView->_32);
				pVertices[2].v = vector3(pVertices[2].v.x * matView->_11 + pVertices[2].v.y * matView->_12, pVertices[2].v.x * matView->_21 + pVertices[2].v.y * matView->_22, pVertices[2].v.x * matView->_31 + pVertices[2].v.y * matView->_32);
				pVertices[3].v = vector3(pVertices[3].v.x * matView->_11 + pVertices[3].v.y * matView->_12, pVertices[3].v.x * matView->_21 + pVertices[3].v.y * matView->_22, pVertices[3].v.x * matView->_31 + pVertices[3].v.y * matView->_32);

				pVertices[0].v += vecCenter;
				pVertices[1].v += vecCenter;
				pVertices[2].v += vecCenter;
				pVertices[3].v += vecCenter;
			}
	///////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////
			if(((CX3DEffect *)m_lpLocalEffect)->GetCenter())
			{
				pVerticesBlend[0].v = pVertices[0].v += ((*((CX3DEffect *)m_lpLocalEffect)->GetCenter()));
				pVerticesBlend[1].v = pVertices[1].v += ((*((CX3DEffect *)m_lpLocalEffect)->GetCenter()));
				pVerticesBlend[2].v = pVertices[2].v += ((*((CX3DEffect *)m_lpLocalEffect)->GetCenter()));
				pVerticesBlend[3].v = pVertices[3].v += ((*((CX3DEffect *)m_lpLocalEffect)->GetCenter()));
			} else
			{
				pVerticesBlend[0].v = pVertices[0].v;
				pVerticesBlend[1].v = pVertices[1].v;
				pVerticesBlend[2].v = pVertices[2].v;
				pVerticesBlend[3].v = pVertices[3].v;
			}
	///////////////////////////////////////////////////////////////////////////////////
			float f1, f2;
			float fStartU, fStartV, fEndU, fEndV;
			float fStartBU, fStartBV, fEndBU, fEndBV;
			if(m_bTexAni)
			{
				f1 = (((long)m_fTexFrame) % 4) * 0.25;
				f2 = (((long)m_fTexFrame) / 4) * 0.25;
				fStartU = f1;
				fStartV = f2;
				fEndU = f1 + 0.25f;
				fEndV = f2 + 0.25f;

				f1 = (((long)ceilf(m_fTexFrame)) % 4) * 0.25;
				f2 = (((long)ceilf(m_fTexFrame)) / 4) * 0.25;
				fStartBU = f1;
				fStartBV = f2;
				fEndBU = f1 + 0.25f;
				fEndBV = f2 + 0.25f;
			} else
			{
				fStartBU = fStartU = m_fStartU;
				fStartBV = fStartV = m_fStartV;
				fEndBU = fEndU = m_fTileU;
				fEndBV = fEndV = m_fTileV;
			}
			pVertices[0].tu = fStartU;					pVertices[0].tv = fStartV;
			pVertices[1].tu = fEndU;					pVertices[1].tv = fStartV;
			pVertices[2].tu = fStartU;					pVertices[2].tv = fEndV;
			pVertices[3].tu = fEndU;					pVertices[3].tv = fEndV;
			pVerticesBlend[0].tu = fStartBU;			pVerticesBlend[0].tv = fStartBV;
			pVerticesBlend[1].tu = fEndBU;				pVerticesBlend[1].tv = fStartBV;
			pVerticesBlend[2].tu = fStartBU;			pVerticesBlend[2].tv = fEndBV;
			pVerticesBlend[3].tu = fEndBU;				pVerticesBlend[3].tv = fEndBV;
	///////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////
			pVertices[0].diff = pVertices[1].diff = pVertices[2].diff = pVertices[3].diff = m_lColor;
			pVerticesBlend[0].diff = pVerticesBlend[1].diff = pVerticesBlend[2].diff = pVerticesBlend[3].diff = m_lColor;
			if(m_bTexAni)
			{
				pVertices[3].diff.a *= (floorf(m_fTexFrame + 1.0f) - m_fTexFrame);
				pVertices[0].diff.a = pVertices[1].diff.a = pVertices[2].diff.a = pVertices[3].diff.a;
				pVerticesBlend[3].diff.a *= (m_fTexFrame - floorf(m_fTexFrame));
				pVerticesBlend[0].diff.a = pVerticesBlend[1].diff.a = pVerticesBlend[2].diff.a = pVerticesBlend[3].diff.a;
			}
			pVertices[0].spec = pVertices[1].spec = pVertices[2].spec = pVertices[3].spec = color(0xFF, 0xFF, 0xFF, 0xFF);
			pVerticesBlend[0].spec = pVerticesBlend[1].spec = pVerticesBlend[2].spec = pVerticesBlend[3].spec = color(0xFF, 0xFF, 0xFF, 0xFF);
	///////////////////////////////////////////////////////////////////////////////////
			m_lpVerticesBlend->Unlock();
			m_lpVertices->Unlock();
		}
	}
	return TRUE;
}

void CX3DEffectBillboard::Load(FILE *fp, const char *strOriginalPath)
{
	fread(&m_bTexAni, 4, 1, fp);
	fread(&m_dwSrcBlending, 4, 1, fp);
	fread(&m_dwDestBlending, 4, 1, fp);
	fread(&m_bAxisAligned, 4, 1, fp);
	m_lstAxis.Load(fp, m_quatAxis);
	m_lstCenter.Load(fp, m_vecCenter);
	m_lstColor.Load(fp, m_lColor);
	m_lstWidth.Load(fp, m_fWidth);
	m_lstHeight.Load(fp, m_fHeight);
	m_lstStartU.Load(fp, m_fStartU);
	m_lstStartV.Load(fp, m_fStartV);
	m_lstTileU.Load(fp, m_fTileU);
	m_lstTileV.Load(fp, m_fTileV);
	m_lstTexFrame.Load(fp, m_fTexFrame);
	if(m_Scale[0] != 1.0f) {
		m_vecCenter.x *=m_Scale[0];
		m_vecCenter.z *=m_Scale[0];
		m_vecCenter.y *=(m_Scale[0]);
		m_fWidth *=m_Scale[0];
		m_fHeight *=m_Scale[0];
	}

}

void CX3DEffectBillboard::Save(FILE *fp, const char *strOriginalPath)
{
	fwrite(&m_bTexAni, 4, 1, fp);
	fwrite(&m_dwSrcBlending, 4, 1, fp);
	fwrite(&m_dwDestBlending, 4, 1, fp);
	fwrite(&m_bAxisAligned, 4, 1, fp);
	m_lstAxis.Save(fp);
	m_lstCenter.Save(fp);
	m_lstColor.Save(fp);
	m_lstWidth.Save(fp);
	m_lstHeight.Save(fp);
	m_lstStartU.Save(fp);
	m_lstStartV.Save(fp);
	m_lstTileU.Save(fp);
	m_lstTileV.Save(fp);
	m_lstTexFrame.Save(fp);
}
