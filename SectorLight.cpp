// SectorLight.cpp: implementation of the CSectorLight class.
//
//////////////////////////////////////////////////////////////////////

#include "SectorLight.h"
#include "SceneManager.h"
#include "Sphere.h"
#include "ViewFrustum.h"
#include "GMMemory.h"

extern D3DXVECTOR3 g_vecCube[8];
extern WORD g_vecCubeIndex[36];

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CViewFrustum g_Frustum;

CSectorLight::CSectorLight()
{
	m_vecPos = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_vecColor = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_fRange = 0.0f;
	m_iShadowSamples = 10;
	m_iLightSamples = 10;
	m_fShadowFactors = 0.95f;
	m_iAmbient = 16;
	m_fExpose = 0.0f;

	memset(m_strEffect,0,sizeof(char) * 256);
	

}

CSectorLight::~CSectorLight()
{

}
CSectorLightMap::CSectorLightMap()
{
	m_IndexX = m_IndexY = -1;
	m_lstLights.clear();

}
CSectorLightMap::~CSectorLightMap()
{
	m_lstLights.clear();

}
void CSectorLightMap::Move(float x,float y,float z,float nx,float ny,float nz)
{
	for(int i = 0; i < (int)m_lstLights.size(); i++ )
	{
		if(m_lstLights[i].m_vecPos.x == x && 
			m_lstLights[i].m_vecPos.y == y &&
			m_lstLights[i].m_vecPos.z == z)
		{
			m_lstLights[i].m_vecPos.x = nx;
			m_lstLights[i].m_vecPos.y = ny;
			m_lstLights[i].m_vecPos.z = nz;

			break;
		}
	}	
}
void CSectorLightMap::Delete(float x,float y,float z)
{
	for(int i = 0; i < (int)m_lstLights.size(); i++ )
	{
		if(m_lstLights[i].m_vecPos.x == x && 
			m_lstLights[i].m_vecPos.y == y &&
			m_lstLights[i].m_vecPos.z == z)
		{
			m_lstLights.erase(m_lstLights.begin() + i);
			break;
		}
	}
}
CSectorLightManager::CSectorLightManager()
{
	m_lstLights.clear();
	m_iLights = 0;
	m_pPickLight = NULL;
	m_pSelectLights = NULL;
	m_iSelectLights = 0;
	
	m_lstSelectObjLights.clear();
	m_iSelectObjLights = 0;
	m_pLightEsf = NULL;
}
CSectorLightManager::~CSectorLightManager()
{
	int i;
	
	m_pPickLight = NULL;
	if(m_pSelectLights != NULL)
	{
		delete[] m_pSelectLights;
		m_pSelectLights = NULL;
	}
	for( i = m_iLights - 1; i >= 0; i-- )
	{
		delete m_lstLights[i];
		m_lstLights[i] = NULL;
		m_lstLights.erase(m_lstLights.begin() + i);		
	}
	m_lstLights.clear();
	m_lstSelectObjLights.clear();
	m_iSelectObjLights = 0;

}
void CSectorLightManager::RenderTerrainLight()
{
	int i;
	matrix *matPos = CSceneManager::m_ViewCamera->GetMatPosition();


	if(m_pLightEsf && m_iLights) {
		g_Frustum.Update();

		m_pLightEsf->ProcessEffect();

		for( i = 0; i < m_iLights; i++)
		{
			if(m_lstLights[i]->m_fExpose > 0.0f)
			{			
			
				//Render
				float fHeight  = m_lstLights[i]->m_fRange - m_lstLights[i]->m_fExpose;

				D3DXVECTOR3 vecLength = D3DXVECTOR3(m_lstLights[i]->m_vecPos.x -  matPos->_41,
												(m_lstLights[i]->m_vecPos.y - fHeight)-  matPos->_42,
												m_lstLights[i]->m_vecPos.z -  matPos->_43);

				
				if(D3DXVec3Length(&vecLength) < 10000.0f) {
					if(g_Frustum.SphereInFrustum(m_lstLights[i]->m_vecPos.x,m_lstLights[i]->m_vecPos.y - fHeight,m_lstLights[i]->m_vecPos.z,900.0f)) {
						m_pLightEsf->SetStartPos(m_lstLights[i]->m_vecPos.x,m_lstLights[i]->m_vecPos.y - fHeight,m_lstLights[i]->m_vecPos.z);
						m_pLightEsf->SetEndPos(m_lstLights[i]->m_vecPos.x,(m_lstLights[i]->m_vecPos.y - fHeight),m_lstLights[i]->m_vecPos.z);
						
						m_pLightEsf->m_EffList[0].m_Scale = (m_lstLights[i]->m_fExpose / 20.0f);
						
						float fAlpha = 1.0f - (m_lstLights[i]->m_fExpose / m_lstLights[i]->m_fRange);
						fAlpha *= 0.8f;

						CSceneManager::m_dwSectorLightFactor = D3DCOLOR_COLORVALUE(((m_lstLights[i]->m_vecColor.x / 255.0f) * fAlpha),
																				(m_lstLights[i]->m_vecColor.y / 255.0f)* fAlpha,
																				(m_lstLights[i]->m_vecColor.z / 255.0f)* fAlpha
																				,1.0f);
						CSceneManager::m_bSectorLightFactor = true;

						m_pLightEsf->Render();

						CSceneManager::m_bSectorLightFactor = false;
					
						
					}

				}
			}
		}
	}

}
void CSectorLightManager::RenderLight()
{
	int i;
	LPDIRECT3DDEVICE8 m_D3Device = CSceneManager::GetDevice();

	m_D3Device->SetRenderState(D3DRS_LIGHTING,FALSE);
	m_D3Device->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	
	m_D3Device->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	m_D3Device->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
	m_D3Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);

	m_D3Device->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	m_D3Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
	m_D3Device->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	m_D3Device->SetVertexShader(D3DFVF_XYZ);
	m_D3Device->SetRenderState(D3DRS_ZENABLE,TRUE);
	m_D3Device->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);

	m_D3Device->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	
	D3DXMATRIX identity;
	D3DXMATRIX worldTm;

	m_D3Device->GetTransform(D3DTS_WORLD,&worldTm);


	

	for( i = 0; i < m_iLights; i++ )
	{
	// Local to World
		CSphere tmpSphere;
		
		tmpSphere.SetTransparency(40);
		tmpSphere.SetColor(m_lstLights[i]->m_vecColor.x / 255.0f,
							m_lstLights[i]->m_vecColor.y / 255.0f,
							m_lstLights[i]->m_vecColor.z / 255.0f);

		tmpSphere.SetRadius(m_lstLights[i]->m_fRange);
		tmpSphere.Create(m_D3Device,10,20);
		
		tmpSphere.SetPosition(m_lstLights[i]->m_vecPos.x,m_lstLights[i]->m_vecPos.y,m_lstLights[i]->m_vecPos.z);


		D3DXMatrixIdentity(&identity);
		m_D3Device->SetTransform(D3DTS_WORLD,&identity);
		tmpSphere.Render();

		identity._41 = m_lstLights[i]->m_vecPos.x;
		identity._42 = m_lstLights[i]->m_vecPos.y;
		identity._43 = m_lstLights[i]->m_vecPos.z;

		m_D3Device->SetTransform(D3DTS_WORLD,&identity);
		
		if(m_lstLights[i] == m_pPickLight) 
		{
			m_D3Device->SetRenderState(D3DRS_TEXTUREFACTOR,0xffff0000);
							

		}	
		else
		{
			m_D3Device->SetRenderState(D3DRS_TEXTUREFACTOR,0xff00ff00);
			
		}

		m_D3Device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 
			0,8,12,g_vecCubeIndex,D3DFMT_INDEX16,g_vecCube,sizeof(D3DXVECTOR3));
		m_D3Device->SetRenderState(D3DRS_FOGENABLE,FALSE);

		m_D3Device->SetRenderState(D3DRS_FOGENABLE,TRUE);


	}
	m_D3Device->SetTransform(D3DTS_WORLD,&worldTm);

	m_D3Device->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	m_D3Device->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	m_D3Device->SetRenderState(D3DRS_ZENABLE,TRUE);
}
void CSectorLightManager::InputLight(D3DXVECTOR3 vecPos,D3DXVECTOR3 vecColor,float fRad,char *strEffect,int iLight,int iShadow,float fFactor,int iAmb,float fExpose)
{

	CSectorLight *pLight = new CSectorLight;
	pLight->m_vecPos = vecPos;
	pLight->m_vecColor = vecColor;
	pLight->m_fRange = fRad;
	pLight->m_fExpose = fExpose;

/*	pLight->m_fShadowFactors = fFactor;
	pLight->m_iAmbient = iAmb;
	pLight->m_iLightSamples = iLight;
	pLight->m_iShadowSamples = iShadow;
	
*/

	strcpy(pLight->m_strEffect,strEffect);
	m_lstLights.push_back(pLight);
	m_iLights++;

}
void CSectorLightManager::DeleteAllLight()
{
	
	int iNum = m_iLights; 
	for(int i = (iNum - 1); i >= 0; i-- )
	{
		delete m_lstLights[i];
		m_lstLights.erase(m_lstLights.begin() + i);
		m_iLights--;
	}
	m_lstLights.clear();
	m_iLights = 0;
	m_pPickLight = NULL;
	m_pLightEsf = NULL;


}
void CSectorLightManager::DeletePickLight()
{
	if(m_pPickLight != NULL)
	{
		int i;
		int iPick = -1;

		for( i = 0; i < m_iLights; i++ )
		{
			if(m_pPickLight == m_lstLights[i])
				iPick = i;
		}
		if( iPick != -1)
		{
			delete m_lstLights[iPick];
			m_lstLights.erase(m_lstLights.begin() + iPick);
			m_iLights--;
			
		}
		m_pPickLight = NULL;
	}
}
int CSectorLightManager::PickLight(D3DXVECTOR3 vecPos)
{
	int i;
	int iPick = -1;
	float fMin = 0.0f;

		
	int indexx=(int)(vecPos.x/SECTORSIZE);
	int indexy=(int)(vecPos.z/SECTORSIZE);

	if((indexx != m_IndexX) ||
		(indexy != m_IndexY))
	{
		m_pPickLight = NULL;
		return -1;
	}
	for( i = 0; i < m_iLights; i++ )
	{	
		D3DXVECTOR3 vecLength = vecPos - m_lstLights[i]->m_vecPos;
		float fLength = D3DXVec3Length(&vecLength);
		if( i == 0)
		{
			fMin = fLength;
			iPick = 0;
		}
		else
		{
			if(fMin > fLength)
			{
				fMin = fLength;
				iPick = i;

			}
		}

	}
	if(iPick >= 0) {
		m_pPickLight = m_lstLights[iPick];
		CSceneManager::m_pPickSectorLight = m_pPickLight;

	}
	return iPick;

}
void CSectorLightManager::SelectObjLight(float fx,float fy,float fz)
{
	int i;
	
	D3DXVECTOR3 vecPos = D3DXVECTOR3(fx,fy,fz);

	D3DXVECTOR3 vecLight;
	float fLength = 0.0f;

	if(m_iSelectObjLights != 0)
	{
		m_iSelectObjLights = 0;
		m_lstSelectObjLights.clear();

	}
	for( i = 0; i < m_iLights; i++ )
	{
		vecLight = m_lstLights[i]->m_vecPos;
		fLength = D3DXVec3Length(&(vecPos - vecLight));
		if(fLength <= m_lstLights[i]->m_fRange + 100.0f)
		{
			CSectorLight AddNode;
			
			AddNode.m_vecPos = m_lstLights[i]->m_vecPos;
			AddNode.m_vecColor = m_lstLights[i]->m_vecColor;
			AddNode.m_fRange = m_lstLights[i]->m_fRange;

			m_lstSelectObjLights.push_back(AddNode);			
			m_iSelectObjLights++;

		}
	}
}
void CSectorLightManager::SelectLight(D3DXVECTOR3 vecMin,D3DXVECTOR3 vecMax,D3DXMATRIX matTm)
{
	int i,j,k;
	m_iSelectLights = 0;

	D3DXVECTOR3 vecCube[8];
	vecCube[0] = D3DXVECTOR3(vecMax.x,vecMax.y,vecMax.z);
	vecCube[1] = D3DXVECTOR3(vecMax.x,vecMax.y,vecMin.z);
	vecCube[2] = D3DXVECTOR3(vecMax.x,vecMin.y,vecMax.z);
	vecCube[3] = D3DXVECTOR3(vecMax.x,vecMin.y,vecMin.z);
	
	vecCube[4] = D3DXVECTOR3(vecMin.x,vecMax.y,vecMax.z);
	vecCube[5] = D3DXVECTOR3(vecMin.x,vecMax.y,vecMin.z);
	vecCube[6] = D3DXVECTOR3(vecMin.x,vecMin.y,vecMax.z);
	vecCube[7] = D3DXVECTOR3(vecMin.x,vecMin.y,vecMin.z);

	if(m_pSelectLights != NULL)
	{
		delete[] m_pSelectLights;
		m_pSelectLights = NULL;

	}
	// Select Light Num 
	for( i = 0; i < m_iLights; i++)
	{
		D3DXVECTOR3 vecPos = m_lstLights[i]->m_vecPos;
		D3DXMATRIX matPos;
		D3DXMATRIX matTrans;

		D3DXMatrixIdentity(&matPos);
		D3DXMatrixInverse(&matTrans,NULL,&matTm);

		matPos._41 = vecPos.x;
		matPos._42 = vecPos.y;
		matPos._43 = vecPos.z;

		D3DXMatrixMultiply(&matPos,&matPos,&matTrans);
		vecPos.x = matPos._41;
		vecPos.y = matPos._42;
		vecPos.z = matPos._43;

		if( (vecPos.x >= vecMin.x) && (vecPos.y >= vecMin.y) && (vecPos.z >= vecMin.z) )
		{
			if( (vecPos.x <= vecMax.x) && (vecPos.y <= vecMax.y) && (vecPos.z <= vecMax.z) )
			{	
				m_iSelectLights++;
				continue;
			}
		}	
		bool bSet = false;
		for( j = 0; j < 8; j++ )
		{	
		

			D3DXVECTOR3 vecLength = vecCube[j] - m_lstLights[i]->m_vecPos;
			float fLength = D3DXVec3Length(&vecLength);
			if(fLength <= m_lstLights[i]->m_fRange)
			{
				m_iSelectLights++;
				break;
			}
			for( k = 0; k < 8; k++ ) {
				if( k != j)
				{
					/////	Line /////
					D3DXVECTOR3 vecCloss;
					D3DXVECTOR3 vec1 = vecCube[j];
					D3DXVECTOR3 vec2 = vecCube[k];
					
					D3DXVECTOR3 vecStart = vecPos - vec1;
					D3DXVECTOR3 vecVector = vec2 - vec1;
					D3DXVec3Normalize(&vecVector,&vecVector);

					float fDistance = D3DXVec3Length(&(vec2 - vec1));
					
					float fDot = D3DXVec3Dot(&vecVector,&vecStart);
					if(fDot <= 0)
						vecCloss = vec1;
					else if(fDot >= fDistance)
						vecCloss = vec2;
					else
						vecCloss = vec1 + vecVector * fDot;
					
					fLength = D3DXVec3Length(&(vecPos - vecCloss));
					
					if(fLength <= m_lstLights[i]->m_fRange)
					{
						m_iSelectLights++;
						bSet = true;
						break;
					}
				}

			}
			if(bSet == true)
				break;

		}

	}
	m_pSelectLights = new CSectorLight[m_iSelectLights];
	int iCount = 0;

	for( i = 0; i < m_iLights; i++)
	{
		D3DXVECTOR3 vecPos = m_lstLights[i]->m_vecPos;
		
		D3DXMATRIX matPos;
		D3DXMATRIX matTrans;

		D3DXMatrixIdentity(&matPos);
		D3DXMatrixInverse(&matTrans,NULL,&matTm);

		matPos._41 = vecPos.x;
		matPos._42 = vecPos.y;
		matPos._43 = vecPos.z;

		D3DXMatrixMultiply(&matPos,&matPos,&matTrans);
		vecPos.x = matPos._41;
		vecPos.y = matPos._42;
		vecPos.z = matPos._43;


		if( (vecPos.x >= vecMin.x) && (vecPos.y >= vecMin.y) && (vecPos.z >= vecMin.z) )
		{
			if( (vecPos.x <= vecMax.x) && (vecPos.y <= vecMax.y) && (vecPos.z <= vecMax.z) )
			{	
				m_pSelectLights[iCount++] = *(m_lstLights[i]);
				continue;
			}
		}
		bool bSet = false;
		for( j = 0; j < 8; j++ )
		{	
			D3DXVECTOR3 vecLength = vecCube[j] - m_lstLights[i]->m_vecPos;
			float fLength = D3DXVec3Length(&vecLength);
			if(fLength <= m_lstLights[i]->m_fRange)
			{
				m_pSelectLights[iCount++] = *(m_lstLights[i]);
				break;
			}
			for( k = 0; k < 8; k++ ) {
				if( k != j)
				{
					/////	Line /////
					D3DXVECTOR3 vecCloss;
					D3DXVECTOR3 vec1 = vecCube[j];
					D3DXVECTOR3 vec2 = vecCube[k];
					
					D3DXVECTOR3 vecStart = vecPos - vec1;
					D3DXVECTOR3 vecVector = vec2 - vec1;
					D3DXVec3Normalize(&vecVector,&vecVector);

					float fDistance = D3DXVec3Length(&(vec2 - vec1));
					
					float fDot = D3DXVec3Dot(&vecVector,&vecStart);
					if(fDot <= 0)
						vecCloss = vec1;
					else if(fDot >= fDistance)
						vecCloss = vec2;
					else
						vecCloss = vec1 + vecVector * fDot;
					
					fLength = D3DXVec3Length(&(vecPos - vecCloss));
					
					if(fLength <= m_lstLights[i]->m_fRange)
					{
						m_pSelectLights[iCount++] = *(m_lstLights[i]);
						bSet = true;	
						break;

					}
				}
			}

			if(bSet == true)
				break;
		}

	}
}
