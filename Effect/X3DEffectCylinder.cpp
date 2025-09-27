// X3DEffectCylinder.cpp: implementation of the CX3DEffectCylinder class.
//
//////////////////////////////////////////////////////////////////////

#include "X3DEffect.h"
#include "X3DEffectCylinder.h"
#include "SceneManager.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CX3DEffectCylinder::CX3DEffectCylinder()
{
	m_dwSrcBlending = D3DBLEND_SRCALPHA;
	m_dwDestBlending = D3DBLEND_ONE;

	m_bTexAni = FALSE;

	m_lpVertices = NULL;
	m_lpVerticesBlend = NULL;
}

CX3DEffectCylinder::~CX3DEffectCylinder()
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
/*
	if(m_lpVerticesBlend) { m_lpVerticesBlend->Release(); m_lpVerticesBlend = NULL; }
	if(m_lpVertices) { m_lpVertices->Release(); m_lpVertices = NULL; }*/
}

void CX3DEffectCylinder::Create(unsigned long dwStartFrame, unsigned long dwEndFrame)
{
	m_dwStartFrame = dwStartFrame;
	m_dwEndFrame = dwEndFrame;
}

BOOL CX3DEffectCylinder::CreateBuffer(void)
{
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
//	m_lpVertices = CSceneManager::ms_pBufferPools->GetVertexBuffer(((m_dwSidePlane + 1) * 2) * sizeof(LVertex),LVERTEXFVF,true);
//	m_lpVerticesBlend = CSceneManager::ms_pBufferPools->GetVertexBuffer(((m_dwSidePlane + 1) * 2) * sizeof(LVertex),LVERTEXFVF,true);
	m_lpVertices = CSceneManager::ms_pBufferPools->GetVertexBuffer(((m_dwSidePlane + 1) * 2) * sizeof(LVertex),LVERTEXFVF,false);
	m_lpVerticesBlend = CSceneManager::ms_pBufferPools->GetVertexBuffer(((m_dwSidePlane + 1) * 2) * sizeof(LVertex),LVERTEXFVF,false);

/*
	if(m_lpVerticesBlend) { m_lpVerticesBlend->Release(); m_lpVerticesBlend = NULL; }
	if(m_lpVertices) { m_lpVertices->Release(); m_lpVertices = NULL; }

	m_lpD3DDevice->CreateVertexBuffer( ((m_dwSidePlane + 1) * 2) * sizeof(LVertex), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, LVERTEXFVF,
										D3DPOOL_DEFAULT, &m_lpVertices );
	m_lpD3DDevice->CreateVertexBuffer( ((m_dwSidePlane + 1) * 2) * sizeof(LVertex), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, LVERTEXFVF,
										D3DPOOL_DEFAULT, &m_lpVerticesBlend );
*/
	m_dwPrimitive = m_dwSidePlane * 2;

	return TRUE;
}

void CX3DEffectCylinder::Render(void)
{
	if(!m_bVisibility)
		return;
	if(m_lpVertices == NULL) return;

	matrix matWorld;
	matWorld.MakeIdent();
	quaternion *quatAxis = ((CX3DEffect *)m_lpLocalEffect)->GetAxis();
	
	m_lpD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);

	if(quatAxis) { z3d::MatrixRotationQuaternion(matWorld, (*quatAxis)); }
	if(m_QuatSet) {
		z3d::MatrixRotationQuaternion(matWorld, (m_TmpQuat));
	}
	if(((CX3DEffect *)m_lpLocalEffect)->GetCenter())
	{
		vector3 *vecTemp = ((CX3DEffect *)m_lpLocalEffect)->GetCenter();
		matWorld._41 = vecTemp->x;
		matWorld._42 = vecTemp->y;
		matWorld._43 = vecTemp->z;
	}
	m_lpD3DDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX *)&matWorld);
	
	m_lpD3DDevice->SetTexture(0, GetTexture());
	m_lpD3DDevice->SetRenderState(D3DRS_SRCBLEND, m_dwSrcBlending);
	m_lpD3DDevice->SetRenderState(D3DRS_DESTBLEND, m_dwDestBlending);
	m_lpD3DDevice->SetStreamSource(0, m_lpVertices, sizeof(LVertex));
	m_lpD3DDevice->SetVertexShader(LVERTEXFVF);
	m_lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, m_dwPrimitive);

	if(m_bTexAni)
	{
		m_lpD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_lpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		m_lpD3DDevice->SetStreamSource(0, m_lpVerticesBlend, sizeof(LVertex));
		m_lpD3DDevice->SetVertexShader(LVERTEXFVF);
		m_lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, m_dwPrimitive);
	}/**/
}

BOOL CX3DEffectCylinder::Interpolation(float fFrame)
{

///////////////////////////////////////////////////////////////////////////////////
	if(!m_lstCenter.Interpolation(fFrame, m_vecCenter)) return FALSE;
	if(!m_lstAxis.InterpolationQ(fFrame, m_quatAxis)) return FALSE;

	if(!m_lstUpperHeight.Interpolation(fFrame, m_fUpperHeight)) return FALSE;
	if(!m_lstUpperRadius.Interpolation(fFrame, m_fUpperRadius)) return FALSE;
	if(!m_lstLowerHeight.Interpolation(fFrame, m_fLowerHeight)) return FALSE;
	if(!m_lstLowerRadius.Interpolation(fFrame, m_fLowerRadius)) return FALSE;
	if(m_Scale[0] != 1.0f) {
		m_fUpperHeight *=m_Scale[0];
		m_fUpperRadius *=m_Scale[0];
		m_fLowerHeight *=m_Scale[0];
		m_fLowerRadius *=m_Scale[0];
		m_vecCenter.x *=m_Scale[0];
		m_vecCenter.z *=m_Scale[0];
		m_vecCenter.y *=(m_Scale[0]);
	}
	if(!m_lstColor.InterpolationC(fFrame, m_lColor)) return FALSE;
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
			LVertex *pVertices, *pVerticesBlend;
//			if(FAILED( m_lpVertices->Lock( 0, ((m_dwSidePlane + 1) * 2) * sizeof(LVertex), (unsigned char **)&pVertices, D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK) ) )
			if(FAILED( m_lpVertices->Lock( 0, 0, (unsigned char **)&pVertices, 0) ) )
				return FALSE;
//			if(FAILED( m_lpVerticesBlend->Lock( 0, ((m_dwSidePlane + 1) * 2) * sizeof(LVertex), (unsigned char **)&pVerticesBlend,  D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK) ) )
			if(FAILED( m_lpVerticesBlend->Lock( 0, 0, (unsigned char **)&pVerticesBlend,  0) ) )
				return FALSE;

			unsigned long i, tempi;
			float degree;
			float divangle = 360.0f / m_dwSidePlane;
			float f1, f2;
			float fStartU, fStartV, fEndU, fEndV;
			float fStartBU, fStartBV, fEndBU, fEndBV;
			if(m_bTexAni)
			{
			
				
				f1 = (((long)m_fTexFrame) % 4) * 0.25;
				f2 = (((long)m_fTexFrame) / 4) * 0.25;
				fStartU = f1;
				fStartV = f2;
			
				f1 = (((long)ceilf(m_fTexFrame)) % 4) * 0.25;
				f2 = (((long)ceilf(m_fTexFrame)) / 4) * 0.25;
				fStartBU = f1;
				fStartBV = f2;
			
				fEndU = fEndBU = 0.25f;
				fEndV = fEndBV = 0.25f;
			} else
			{
				fStartBU = fStartU = m_fStartU;
				fStartBV = fStartV = m_fStartV;
				fEndBU = fEndU = m_fTileU - fStartBU;
				fEndBV = fEndV = m_fTileV - fStartBV;
			}

			for(i = 0; i < m_dwSidePlane; i++)
			{
				degree = FLOAT_DEG(i * divangle);

				tempi = i * 2 + 1;
				pVertices[tempi].v = vector3(m_fUpperRadius * cosf(degree), m_fUpperHeight, m_fUpperRadius * sinf(degree));
				z3d::VectorRotate(pVertices[tempi].v, pVertices[tempi].v, m_quatAxis);
				pVertices[tempi].v += m_vecCenter;
				pVerticesBlend[tempi].v = pVertices[tempi].v;

				pVertices[tempi].tu = fStartU + fEndU * (float)i / m_dwSidePlane;
				pVertices[tempi].tv = fStartV;
				pVerticesBlend[tempi].tu = fStartBU + fEndBU * (float)i / m_dwSidePlane;
				pVerticesBlend[tempi].tv = fStartBV;

				tempi = i * 2;
				pVertices[tempi].v = vector3(m_fLowerRadius * cosf(degree), -m_fLowerHeight, m_fLowerRadius * sinf(degree));
				z3d::VectorRotate(pVertices[tempi].v, pVertices[tempi].v, m_quatAxis);
				pVertices[tempi].v += m_vecCenter;
				pVerticesBlend[tempi].v = pVertices[tempi].v;

				pVertices[tempi].tu = fStartU + fEndU * (float)i / m_dwSidePlane;
				pVertices[tempi].tv = fEndV;
				pVerticesBlend[tempi].tu = fStartBU + fEndBU * (float)i / m_dwSidePlane;
				pVerticesBlend[tempi].tv = fEndBV;
			}
			tempi = m_dwSidePlane * 2 + 1;
			pVertices[tempi].v = pVertices[1].v;
			pVertices[tempi].tu = fStartU + fEndU;
			pVertices[tempi].tv = fStartV;
			pVerticesBlend[tempi].tu = fStartBU + fEndBU;
			pVerticesBlend[tempi].tv = fStartBV;

			tempi = m_dwSidePlane * 2;
			pVertices[tempi].v = pVertices[0].v;
			pVertices[tempi].tu = fStartU + fEndU;
			pVertices[tempi].tv = fEndV;
			pVerticesBlend[tempi].tu = fStartBU + fEndBU;
			pVerticesBlend[tempi].tv = fEndBV;

			pVertices[0].diff = m_lColor;
			pVertices[0].spec = color(0xFF, 0xFF, 0xFF, 0xFF);
			pVerticesBlend[0].diff = pVertices[0].diff;
			pVerticesBlend[0].spec = pVertices[0].spec;
			if(m_bTexAni)
			{
				pVertices[0].diff.a *= (floorf(m_fTexFrame + 1.0f) - m_fTexFrame);
				pVerticesBlend[0].diff.a *= (m_fTexFrame - floorf(m_fTexFrame));
			}

			for(i = 1; i < (m_dwSidePlane + 1) * 2; i++)
			{
				pVertices[i].diff = pVertices[0].diff;
				pVertices[i].spec = pVertices[0].spec;
				pVerticesBlend[i].diff = pVerticesBlend[0].diff;
				pVerticesBlend[i].spec = pVerticesBlend[0].spec;
			}
			m_lpVerticesBlend->Unlock();
			m_lpVertices->Unlock();
		}
	}
	return TRUE;
}

void CX3DEffectCylinder::Load(FILE *fp, const char *strOriginalPath)
{
	fread(&m_bTexAni, 4, 1, fp);
	fread(&m_dwSrcBlending, 4, 1, fp);
	fread(&m_dwDestBlending, 4, 1, fp);

	m_lstAxis.Load(fp, m_quatAxis);
	m_lstCenter.Load(fp, m_vecCenter);
	fread(&m_dwSidePlane, 4, 1, fp);

	m_lstColor.Load(fp, m_lColor);
	m_lstUpperHeight.Load(fp, m_fUpperHeight);
	m_lstUpperRadius.Load(fp, m_fUpperRadius);
	m_lstLowerHeight.Load(fp, m_fLowerHeight);
	m_lstLowerRadius.Load(fp, m_fLowerRadius);
	if(m_Scale[0] != 1.0f) {
		m_fUpperHeight *=m_Scale[0];
		m_fUpperRadius *=m_Scale[0];
		m_fLowerHeight *=m_Scale[0];
		m_fLowerRadius *=m_Scale[0];
		
		m_vecCenter.x *=m_Scale[0];
		m_vecCenter.z *=m_Scale[0];
		m_vecCenter.y *=(m_Scale[0]);
	}

	m_lstStartU.Load(fp, m_fStartU);
	m_lstStartV.Load(fp, m_fStartV);
	m_lstTileU.Load(fp, m_fTileU);
	m_lstTileV.Load(fp, m_fTileV);
	m_lstTexFrame.Load(fp, m_fTexFrame);
}

void CX3DEffectCylinder::Save(FILE *fp, const char *strOriginalPath)
{
	fwrite(&m_bTexAni, 4, 1, fp);
	fwrite(&m_dwSrcBlending, 4, 1, fp);
	fwrite(&m_dwDestBlending, 4, 1, fp);

	m_lstAxis.Save(fp);
	m_lstCenter.Save(fp);
	fwrite(&m_dwSidePlane, 4, 1, fp);

	m_lstColor.Save(fp);
	m_lstUpperHeight.Save(fp);
	m_lstUpperRadius.Save(fp);
	m_lstLowerHeight.Save(fp);
	m_lstLowerRadius.Save(fp);

	m_lstStartU.Save(fp);
	m_lstStartV.Save(fp);
	m_lstTileU.Save(fp);
	m_lstTileV.Save(fp);
	m_lstTexFrame.Save(fp);
}
