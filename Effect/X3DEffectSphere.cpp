// X3DEffectSphere.cpp: implementation of the CX3DEffectSphere class.
//
//////////////////////////////////////////////////////////////////////

#include "X3DEffect.h"
#include "X3DEffectSphere.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CX3DEffectSphere::CX3DEffectSphere()
{
	m_dwSrcBlending = D3DBLEND_SRCALPHA;
	m_dwDestBlending = D3DBLEND_ONE;

	m_bTexAni = FALSE;

	m_lpVertices = NULL;
	m_lpVerticesBlend = NULL;
	m_lpVerticeIndex = NULL;
}

CX3DEffectSphere::~CX3DEffectSphere()
{
	if(m_lpVerticeIndex) { m_lpVerticeIndex->Release(); m_lpVerticeIndex = NULL; }
	if(m_lpVerticesBlend) { m_lpVerticesBlend->Release(); m_lpVerticesBlend = NULL; }
	if(m_lpVertices) { m_lpVertices->Release(); m_lpVertices = NULL; }
}

void CX3DEffectSphere::Create(unsigned long dwStartFrame, unsigned long dwEndFrame)
{
	m_dwStartFrame = dwStartFrame;
	m_dwEndFrame = dwEndFrame;
}

BOOL CX3DEffectSphere::CreateBuffer()
{
	if(m_lpVerticeIndex) { m_lpVerticeIndex->Release(); m_lpVerticeIndex = NULL; }
	if(m_lpVerticesBlend) { m_lpVerticesBlend->Release(); m_lpVerticesBlend = NULL; }
	if(m_lpVertices) { m_lpVertices->Release(); m_lpVertices = NULL; }

	if(m_bUpperVis && m_bLowerVis) m_dwTotalSegment = m_dwSegment * 2;
	else if(m_bUpperVis || m_bLowerVis) m_dwTotalSegment = m_dwSegment;
	else return FALSE;

	m_lpD3DDevice->CreateVertexBuffer( (m_dwSidePlane + 1) * (m_dwSegment + 1) * 2 * sizeof(LVertex), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, LVERTEXFVF,
										D3DPOOL_DEFAULT, &m_lpVertices );
	m_lpD3DDevice->CreateVertexBuffer( (m_dwSidePlane + 1) * (m_dwSegment + 1) * 2 * sizeof(LVertex), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, LVERTEXFVF,
										D3DPOOL_DEFAULT, &m_lpVerticesBlend );
	m_lpD3DDevice->CreateIndexBuffer( m_dwSidePlane * m_dwTotalSegment * 2 * 3 * sizeof(unsigned short), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
										D3DPOOL_MANAGED, &m_lpVerticeIndex);

	if(m_dwTotalSegment == m_dwSegment)
	{
		m_dwNumVertex = (m_dwSidePlane + 1) * (m_dwSegment + 1);
	} else
	{
		m_dwNumVertex = (m_dwSidePlane + 1) * (m_dwSegment + 1) * 2;
	}
	m_dwPrimitive = m_dwSidePlane * m_dwTotalSegment * 2;

	unsigned short *pVerticeIndex;
	m_lpVerticeIndex->Lock(0, m_dwSidePlane * m_dwTotalSegment * 2 * 3 * sizeof(unsigned short), (unsigned char **)&pVerticeIndex, 0);

	unsigned long i, j, ti = 0, tv = 0;
	unsigned long temp, t1, t2;

// Upper
	if(m_bUpperVis)
	{
		for(j = 0; j < m_dwSegment; j++)
		{
			for(i = 0; i < m_dwSidePlane; i++)
			{
				temp = j * (m_dwSidePlane * 6) + i * 6;
				t1 = (m_dwSidePlane + 1) * j + i;
				t2 = (m_dwSidePlane + 1) * (j + 1) + i;
				if(m_bUpperUp)
				{
// Up
					pVerticeIndex[temp + 0] = t1 + 0;
					pVerticeIndex[temp + 1] = t1 + 1;
					pVerticeIndex[temp + 2] = t2;

					pVerticeIndex[temp + 3] = t1 + 1;
					pVerticeIndex[temp + 4] = t2 + 1;
					pVerticeIndex[temp + 5] = t2;
				} else
				{
// Center
					pVerticeIndex[temp + 0] = t1 + 0;
					pVerticeIndex[temp + 1] = t2;
					pVerticeIndex[temp + 2] = t1 + 1;

					pVerticeIndex[temp + 3] = t2;
					pVerticeIndex[temp + 4] = t2 + 1;
					pVerticeIndex[temp + 5] = t1 + 1;
				}
			}
		}

		ti = m_dwSidePlane * m_dwSegment * 2 * 3;
		tv = (m_dwSidePlane + 1) * (m_dwSegment + 1);
	}

// Lower
	if(m_bLowerVis)
	{
		for(j = 0; j < m_dwSegment; j++)
		{
			for(i = 0; i < m_dwSidePlane; i++)
			{
				temp = j * (m_dwSidePlane * 6) + i * 6 + ti;
				t1 = (m_dwSidePlane + 1) * j + i;
				t2 = (m_dwSidePlane + 1) * (j + 1) + i;
				if(m_bLowerUp)
				{
// Up
					pVerticeIndex[temp + 0] = tv + t1 + 0;
					pVerticeIndex[temp + 1] = tv + t2;
					pVerticeIndex[temp + 2] = tv + t1 + 1;

					pVerticeIndex[temp + 3] = tv + t2;
					pVerticeIndex[temp + 4] = tv + t2 + 1;
					pVerticeIndex[temp + 5] = tv + t1 + 1;
				} else
				{
// Center
					pVerticeIndex[temp + 0] = tv + t1 + 0;
					pVerticeIndex[temp + 1] = tv + t1 + 1;
					pVerticeIndex[temp + 2] = tv + t2;

					pVerticeIndex[temp + 3] = tv + t1 + 1;
					pVerticeIndex[temp + 4] = tv + t2 + 1;
					pVerticeIndex[temp + 5] = tv + t2;
				}
			}
		}
	}
	m_lpVerticeIndex->Unlock();

	return TRUE;
}

void CX3DEffectSphere::Render(void)
{
	if(!m_bVisibility)
		return;
	if(m_lpVertices == NULL) return;

	matrix matWorld;
	matWorld.MakeIdent();
	quaternion *quatAxis = ((CX3DEffect *)m_lpLocalEffect)->GetAxis();
	if(quatAxis) { z3d::MatrixRotationQuaternion(matWorld, (*quatAxis)); }
	if(((CX3DEffect *)m_lpLocalEffect)->GetCenter())
	{
		vector3 *vecTemp = ((CX3DEffect *)m_lpLocalEffect)->GetCenter();
		matWorld._41 = vecTemp->x;
		matWorld._42 = vecTemp->y;
		matWorld._43 = vecTemp->z;
	}
	m_lpD3DDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX *)&matWorld);
	m_lpD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
	m_lpD3DDevice->SetTexture(0, GetTexture());
	m_lpD3DDevice->SetRenderState(D3DRS_SRCBLEND, m_dwSrcBlending);
	m_lpD3DDevice->SetRenderState(D3DRS_DESTBLEND, m_dwDestBlending);
	m_lpD3DDevice->SetStreamSource(0, m_lpVertices, sizeof(LVertex));
	m_lpD3DDevice->SetVertexShader(LVERTEXFVF);
	m_lpD3DDevice->SetIndices(m_lpVerticeIndex, 0);
	m_lpD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_dwNumVertex, 0, m_dwPrimitive);

	if(m_bTexAni)
	{
		m_lpD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
		m_lpD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_lpD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		m_lpD3DDevice->SetStreamSource(0, m_lpVerticesBlend, sizeof(LVertex));
		m_lpD3DDevice->SetVertexShader(LVERTEXFVF);
		m_lpD3DDevice->SetIndices(m_lpVerticeIndex, 0);
		m_lpD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_dwNumVertex, 0, m_dwPrimitive);
	}/**/
}

BOOL CX3DEffectSphere::Interpolation(float fFrame)
{


	if(m_lpVertices == NULL) return FALSE;

///////////////////////////////////////////////////////////////////////////////////
	if(!m_lstCenter.Interpolation(fFrame, m_vecCenter)) return FALSE;
	if(!m_lstAxis.InterpolationQ(fFrame, m_quatAxis)) return FALSE;
	if(!m_lstHeightFactor.Interpolation(fFrame, m_fHeightFactor)) return FALSE;
	if(!m_lstRadius.Interpolation(fFrame, m_fRadius)) return FALSE;
	if(m_Scale[0] != 1.0f) {
		m_fRadius *= m_Scale[0];

		m_vecCenter.x *=m_Scale[0];
		m_vecCenter.z *=m_Scale[0];
		m_vecCenter.y *=(m_Scale[0]);
	}
	if(!m_lstUpperFactor.Interpolation(fFrame, m_fUpperFactor)) return FALSE;
	if(!m_lstLowerFactor.Interpolation(fFrame, m_fLowerFactor)) return FALSE;
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
///////////////////////////////////////////////////////////////////////////////////
	if(m_bVisibility) {

		{
			LVertex *pVertices, *pVerticesBlend;
			if(FAILED( m_lpVertices->Lock( 0, (m_dwSidePlane + 1) * (m_dwSegment + 1) * 2 * sizeof(LVertex), (unsigned char **)&pVertices, D3DLOCK_DISCARD ) ) )
				return FALSE;
			if(FAILED( m_lpVerticesBlend->Lock( 0, (m_dwSidePlane + 1) * (m_dwSegment + 1) * 2 * sizeof(LVertex), (unsigned char **)&pVerticesBlend, D3DLOCK_DISCARD ) ) )
				return FALSE;

			unsigned long i, tempi, j, t1, tv = 0;
			float degree, hdegree, height, hcos;
			float divangle = FLOAT_PHI * 2.0f / m_dwSidePlane;
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

			float fHEV = fEndV / 2.0f, fSV = fHEV + fStartV, fDSV = fSV * 2.0f;
			float fHEBV = fEndBV / 2.0f, fSBV = fHEBV + fStartBV, fDSBV = fSBV * 2.0f;
			float fSegTemp, fSegBTemp;
			if(m_bUpperVis)
			{
				for(j = 0; j <= m_dwSegment; j++)
				{
					if(m_bUpperUp)
						hdegree = (FLOAT_PHI / 2.0f) - ((j * FLOAT_PHI) / (2.0f * m_dwSegment) * m_fUpperFactor);
					else
						hdegree = ((j * FLOAT_PHI) / (2.0f * m_dwSegment)) * m_fUpperFactor;
					height = m_fRadius * m_fHeightFactor * sinf(hdegree);
					hcos = cosf(hdegree);
					fSegTemp = fHEV * j / m_dwSegment;
					fSegBTemp = fHEBV * j / m_dwSegment;

					for(i = 0; i <= m_dwSidePlane; i++)
					{
						if(i == m_dwSidePlane) tempi = 0; else tempi = i;

						degree = tempi * divangle;
						t1 = (m_dwSidePlane + 1) * j + i;

						pVertices[t1].v = vector3(m_fRadius * cosf(degree) * hcos, height, m_fRadius * sinf(degree) * hcos);
						z3d::VectorRotate(pVertices[t1].v, pVertices[t1].v, m_quatAxis);
						pVertices[t1].v += m_vecCenter;
						pVerticesBlend[t1].v = pVertices[t1].v;

						pVertices[t1].tu = fStartU + fEndU * (float)i / m_dwSidePlane;
						pVerticesBlend[t1].tu = fStartBU + fEndBU * (float)i / m_dwSidePlane;
						if(m_bUpperUp)
						{
							if(m_bUpperTex)
							{
								pVertices[t1].tv = fStartV + fSegTemp * m_fUpperFactor;
								pVerticesBlend[t1].tv = fStartBV + fSegBTemp * m_fUpperFactor;
							} else
							{
								pVertices[t1].tv = fStartV + fSegTemp;
								pVerticesBlend[t1].tv = fStartBV + fSegBTemp;
							}
						}
						else
						{
							if(m_bUpperTex)
							{
								pVertices[t1].tv = (fStartV + fSV) - ((fStartV + fSegTemp) * m_fUpperFactor);
								pVerticesBlend[t1].tv = (fStartBV + fSBV) - ((fStartBV + fSegBTemp) * m_fUpperFactor);
							} else
							{
								pVertices[t1].tv = fSV - fSegTemp;
								pVerticesBlend[t1].tv = fSBV - fSegBTemp;
							}
						}
					}
				}
				tv = (m_dwSidePlane + 1) * (m_dwSegment + 1);
			}

			if(m_bLowerVis)
			{
				for(j = 0; j <= m_dwSegment; j++)
				{
					if(m_bLowerUp)
						hdegree = (FLOAT_PHI / 2.0f) - ((j * FLOAT_PHI) / (2.0f * m_dwSegment) * m_fLowerFactor);
					else
						hdegree = ((j * FLOAT_PHI) / (2.0f * m_dwSegment)) * m_fLowerFactor;
					height = m_fRadius * m_fHeightFactor * sinf(hdegree);
					hcos = cosf(hdegree);
					fSegTemp = fHEV * j / m_dwSegment;
					fSegBTemp = fHEBV * j / m_dwSegment;

					for(i = 0; i <= m_dwSidePlane; i++)
					{
						if(i == m_dwSidePlane) tempi = 0; else tempi = i;

						degree = tempi * divangle;
						t1 = tv + (m_dwSidePlane + 1) * j + i;

						pVertices[t1].v = vector3(m_fRadius * cosf(degree) * hcos, -height, m_fRadius * sinf(degree) * hcos);
						z3d::VectorRotate(pVertices[t1].v, pVertices[t1].v, m_quatAxis);
						pVertices[t1].v += m_vecCenter;
						pVerticesBlend[t1].v = pVertices[t1].v;

						pVertices[t1].tu = fStartU + fEndU * (float)i / m_dwSidePlane;
						pVerticesBlend[t1].tu = fStartBU + fEndBU * (float)i / m_dwSidePlane;
						if(m_bLowerUp)
						{
							if(m_bLowerTex)
							{
								pVertices[t1].tv = fDSV - ((fStartV + fSegTemp) * m_fLowerFactor);
								pVerticesBlend[t1].tv = fDSBV - ((fStartBV + fSegBTemp) * m_fLowerFactor);
							} else
							{
								pVertices[t1].tv = fDSV - (fStartV + fSegTemp);
								pVerticesBlend[t1].tv = fDSBV - (fStartBV + fSegBTemp);
							}
						}
						else
						{
							if(m_bLowerTex)
							{
								pVertices[t1].tv = fSV + fSegTemp * m_fLowerFactor;
								pVerticesBlend[t1].tv = fSBV + fSegBTemp * m_fLowerFactor;
							} else
							{
								pVertices[t1].tv = fSV + fSegTemp;
								pVerticesBlend[t1].tv = fSBV + fSegBTemp;
							}
						}
					}
				}
			}

			pVertices[0].diff = m_lColor;
			pVertices[0].spec = color(0xFF, 0xFF, 0xFF, 0xFF);
			pVerticesBlend[0].diff =pVertices[0].diff;
			pVerticesBlend[0].spec =pVertices[0].spec;
			if(m_bTexAni)
			{
				pVertices[0].diff.a *= (floorf(m_fTexFrame + 1.0f) - m_fTexFrame);
				pVerticesBlend[0].diff.a *= (m_fTexFrame - floorf(m_fTexFrame));
			}

			if(m_dwTotalSegment == m_dwSegment)
			{
				for(i = 1; i < (m_dwSidePlane + 1) * (m_dwSegment + 1); i++)
				{
					pVertices[i].diff = pVertices[0].diff;
					pVertices[i].spec = pVertices[0].spec;
					pVerticesBlend[i].diff = pVerticesBlend[0].diff;
					pVerticesBlend[i].spec = pVerticesBlend[0].spec;
				}
			} else
			{
				for(i = 1; i < (m_dwSidePlane + 1) * (m_dwSegment + 1) * 2; i++)
				{
					pVertices[i].diff = pVertices[0].diff;
					pVertices[i].spec = pVertices[0].spec;
					pVerticesBlend[i].diff = pVerticesBlend[0].diff;
					pVerticesBlend[i].spec = pVerticesBlend[0].spec;
				}
			}
			m_lpVerticesBlend->Unlock();
			m_lpVertices->Unlock();
		}
	}
	return TRUE;
}

void CX3DEffectSphere::Load(FILE *fp, const char *strOriginalPath)
{
	fread(&m_bTexAni, 4, 1, fp);
	fread(&m_dwSrcBlending, 4, 1, fp);
	fread(&m_dwDestBlending, 4, 1, fp);

	m_lstAxis.Load(fp, m_quatAxis);
	m_lstCenter.Load(fp, m_vecCenter);

	fread(&m_dwSidePlane, 4, 1, fp);
	fread(&m_dwSegment, 4, 1, fp);

	fread(&m_bUpperTex, 4, 1, fp);
	fread(&m_bUpperUp, 4, 1, fp);
	fread(&m_bUpperVis, 4, 1, fp);
	fread(&m_bLowerTex, 4, 1, fp);
	fread(&m_bLowerUp, 4, 1, fp);
	fread(&m_bLowerVis, 4, 1, fp);

	m_lstColor.Load(fp, m_lColor);
	m_lstHeightFactor.Load(fp, m_fHeightFactor);
	m_lstRadius.Load(fp, m_fRadius);
	if(m_Scale[0] != 1.0f) {	
		m_fRadius *= m_Scale[0];

		m_vecCenter.x *=m_Scale[0];
		m_vecCenter.z *=m_Scale[0];
		m_vecCenter.y *=(m_Scale[0]);
	}
	m_lstUpperFactor.Load(fp, m_fUpperFactor);
	m_lstLowerFactor.Load(fp, m_fLowerFactor);

	m_lstStartU.Load(fp, m_fStartU);
	m_lstStartV.Load(fp, m_fStartV);
	m_lstTileU.Load(fp, m_fTileU);
	m_lstTileV.Load(fp, m_fTileV);
	m_lstTexFrame.Load(fp, m_fTexFrame);
}

void CX3DEffectSphere::Save(FILE *fp, const char *strOriginalPath)
{
	fwrite(&m_bTexAni, 4, 1, fp);
	fwrite(&m_dwSrcBlending, 4, 1, fp);
	fwrite(&m_dwDestBlending, 4, 1, fp);

	m_lstAxis.Save(fp);
	m_lstCenter.Save(fp);

	fwrite(&m_dwSidePlane, 4, 1, fp);
	fwrite(&m_dwSegment, 4, 1, fp);

	fwrite(&m_bUpperTex, 4, 1, fp);
	fwrite(&m_bUpperUp, 4, 1, fp);
	fwrite(&m_bUpperVis, 4, 1, fp);
	fwrite(&m_bLowerTex, 4, 1, fp);
	fwrite(&m_bLowerUp, 4, 1, fp);
	fwrite(&m_bLowerVis, 4, 1, fp);

	m_lstColor.Save(fp);
	m_lstHeightFactor.Save(fp);
	m_lstRadius.Save(fp);
	m_lstUpperFactor.Save(fp);
	m_lstLowerFactor.Save(fp);

	m_lstStartU.Save(fp);
	m_lstStartV.Save(fp);
	m_lstTileU.Save(fp);
	m_lstTileV.Save(fp);
	m_lstTexFrame.Save(fp);
}
