#include "./SMRHeader.h"
#include "GMMemory.h"
namespace SGEngine { namespace Core {
bool ISSameVertex(const D3DXVECTOR3 &vec1,const D3DXVECTOR3 &vec2)
	{
		D3DXVECTOR3 vecSub = vec1 - vec2;
		if((fabs(vecSub.x) < 0.001f) &&
			(fabs(vecSub.y) < 0.001f) &&
			(fabs(vecSub.z) < 0.001f))
			return true;
		return false;

	}

void CMeshCoreData::CMeshSMorphData::SetSmartMorphData(CExportMeshVertex *pSrc,CExportMeshVertex *pDest,const int &iVertices,const int &iModelIndex,char *strModelName)
{
	std::vector<D3DXVECTOR3> ArrRelative;
	std::vector<int> ArrRelativeIndex;
	ArrRelative.clear();
	ArrRelativeIndex.clear();
	int i;
	D3DXVECTOR3 vecRelative;

	for(i = 0; i < iVertices; i++)
	{
		if(!ISSameVertex(pSrc[i].m_vecPos,pDest[i].m_vecPos))
		{
			ArrRelativeIndex.push_back(i);
			vecRelative = pDest[i].m_vecPos - pSrc[i].m_vecPos;
			ArrRelative.push_back(vecRelative);
		}
	}
	
	m_iRelativeNum = (int)ArrRelative.size();
	m_pRelativeVertices = new D3DXVECTOR3[ArrRelative.size()];
	memcpy(m_pRelativeVertices,&ArrRelative[0],sizeof(D3DXVECTOR3) * ArrRelative.size());
	m_pRelativeIndex = new int[ArrRelativeIndex.size()];
	memcpy(m_pRelativeIndex,&ArrRelativeIndex[0],sizeof(int) * ArrRelativeIndex.size());
	m_iModelID = iModelIndex;
	if(strModelName)
	{
		strcpy(m_strName,strModelName);
	}

}
void CMeshCoreData::SetSkinMeshData(const int &iVertex,D3DXMATRIX *matTm,D3DXMATRIX *matInvNodeTm)	// rigid
{
	
	if(iVertex < m_iVerticesNum)
	{
	
		D3DXMATRIX matSkinTm;
	
		m_bUpdateData = true;
		D3DXVECTOR3 *pPos = &(m_pDefaultVertices[iVertex].m_vecPos);
		D3DXVECTOR3 *pDestPos = &(m_pVertices[iVertex].m_vecPos);
		D3DXVECTOR4 vecTrans;
	
		D3DXMatrixMultiply(&matSkinTm,matInvNodeTm,matTm);
		
		D3DXVec3Transform(&vecTrans,pPos,&matSkinTm);
		
		pDestPos->x = vecTrans.x;
		pDestPos->y = vecTrans.y;
		pDestPos->z = vecTrans.z;
		
	}
	
}
void CMeshCoreData::SetSkinMeshData(const int &iVertex,CMeshSkinData::CMeshBlendData *pBlend,const int &iBlend)
{
	int i;
	if(iVertex < m_iVerticesNum)
	{
		D3DXMATRIX matWorldTm;
		D3DXMATRIX matInvNodeTm;

		D3DXMATRIX matSkinTm;

		m_bUpdateData = true;
		D3DXVECTOR3 *pPos = &(m_pDefaultVertices[iVertex].m_vecPos);
		D3DXVECTOR3 *pDestPos = &(m_pVertices[iVertex].m_vecPos);
		D3DXVECTOR4 vecTrans;

		pDestPos->x = pDestPos->y = pDestPos->z = 0.0f;

		for(i = 0; i < iBlend; i++ )
		{
		
			matWorldTm = pBlend[i].m_pNode->m_matNodeTm;
			matWorldTm *= pBlend[i].m_fNodeWeight;

			matInvNodeTm = pBlend[i].m_pNode->m_matInvNodeTm;
			
			D3DXMatrixMultiply(&matSkinTm,&matInvNodeTm,&matWorldTm);
			D3DXVec3Transform(&vecTrans,pPos,&matSkinTm);	
			
			pDestPos->x += vecTrans.x;
			pDestPos->y += vecTrans.y;
			pDestPos->z += vecTrans.z;


		}

	}
}
void CMeshCoreData::SetSmartMorphInterValue(const float &fValue,const int &iIndex)
{
	

	if(m_bSMorph)
	{
		float fInter = fValue;
		if(m_iSMorph > iIndex && m_pfSMorphInterValue)
		{
		/*	if(fInter > 1.0f)
				fInter = 1.0f;
			if(fInter < 0.0f)
				fInter = 0.0f;*/
			m_pfSMorphInterValue[iIndex] = fInter;

		}
	}
}
void CMeshCoreData::UpdateMeshSmartMorph()
{

	if(m_bSMorph && m_pVertices && m_pDefaultVertices)
	{
		int i,j;
		m_bUpdateData = true;
		memcpy(m_pVertices,m_pDefaultVertices,sizeof(CExportMeshVertex) * m_iVerticesNum);

		for(i = 0; i < m_iSMorph; i++ )
		{
			if(fabs(m_pfSMorphInterValue[i]) > 0.00001f)
			{
				for(j = 0; j < m_pSMorph[i].m_iRelativeNum; j++ )
				{
					int iVertexIndex = m_pSMorph[i].m_pRelativeIndex[j];
					m_pVertices[iVertexIndex].m_vecPos += (m_pSMorph[i].m_pRelativeVertices[j] * m_pfSMorphInterValue[i]);
				}
	
			}

		}
	}
}
void CMeshCoreData::UpdateMeshMorph(const float &fFrame)
{
	if(m_bMorph && !m_bSMorph)	// 일단 smorph과 동시에 적용 안되게 해놓는다.. 추후 수정가능
	{
		int i;
		int iSelect = 0;

		int iCurrentFrame = (int)ceil(fFrame);
		int iBeforeFrame = 0;
		int iNextFrame = 0;

		int iBeforeKey = 0;
		int iNextKey = 0;
		
		float fInterval = 0.0f;
	
		CExportMeshVertex *pVertices = NULL;

		for(i = 0; i < m_iMorph; i++ )
		{
			if(m_pMorphData[i].m_iFrame >= iCurrentFrame)
			{
				iSelect = i;
				break;
			}
		}
		if(m_iMorph == i)
		{
			iSelect = m_iMorph - 1;
			pVertices  = m_pMorphData[iSelect].m_pVertices;
			memcpy(m_pVertices,pVertices,sizeof(CExportMeshVertex) * m_iVerticesNum);
			m_bUpdateData = true;


		}
		else if(iSelect == 0)
		{
			pVertices  = m_pMorphData[iSelect].m_pVertices;
			memcpy(m_pVertices,pVertices,sizeof(CExportMeshVertex) * m_iVerticesNum);
			m_bUpdateData = true;

		}
		else
		{
			m_bUpdateData = true;

			iBeforeKey = iSelect - 1;
			iBeforeFrame = m_pMorphData[iBeforeKey].m_iFrame;

			iNextKey = iSelect;
			iNextFrame = m_pMorphData[iNextKey].m_iFrame;

			fInterval = (fFrame - iBeforeFrame) / (float)(iNextFrame - iBeforeFrame);
			
			for(i = 0 ;i < m_iVerticesNum; i++ )
			{
				m_pVertices[i].m_vecPos = m_pMorphData[iNextKey].m_pVertices[i].m_vecPos - m_pMorphData[iBeforeKey].m_pVertices[i].m_vecPos;
				m_pVertices[i].m_vecPos *= fInterval;
				
				m_pVertices[i].m_vecPos += m_pMorphData[iBeforeKey].m_pVertices[i].m_vecPos;


				m_pVertices[i].m_vecNormal = m_pMorphData[iNextKey].m_pVertices[i].m_vecNormal - m_pMorphData[iBeforeKey].m_pVertices[i].m_vecNormal;
				m_pVertices[i].m_vecNormal *= fInterval;
				
				m_pVertices[i].m_vecNormal += m_pMorphData[iBeforeKey].m_pVertices[i].m_vecNormal;

				m_pVertices[i].m_vecTexcoord = m_pMorphData[iNextKey].m_pVertices[i].m_vecTexcoord - m_pMorphData[iBeforeKey].m_pVertices[i].m_vecTexcoord;
				m_pVertices[i].m_vecTexcoord *= fInterval;
				
				m_pVertices[i].m_vecTexcoord += m_pMorphData[iBeforeKey].m_pVertices[i].m_vecTexcoord;


			}

		}


	}
}
// Linear interpolation.
void CMeshObjNode::UpdateTm(const float &fFrame,D3DXMATRIX *matParent)
{
	int i;
	int iSelect = 0;

	D3DXVECTOR3 vecPos;
	D3DXVECTOR3 vecScale;
	D3DXQUATERNION qRot;

	int iCurrentFrame = (int)ceil(fFrame);
	int iBeforeFrame = 0;
	int iNextFrame = 0;

	int iBeforeKey = 0;
	int iNextKey = 0;

	float fInterval = 0.0f;
	D3DXVECTOR3 vecInterval;

	if(m_bAniNode)
	{
	
		if(m_pAniNode->m_bTmAni)
		{
			CNodeTMInfo CurrentTmInfo;

			for(i = 0; i < m_pAniNode->m_iTmKeys; i++)
			{
				if(m_pAniNode->m_pTmKeys[i].m_iFrame >= iCurrentFrame)
				{
					iSelect = i;
					break;
				}
			}
			if(m_pAniNode->m_iTmKeys == i)
			{
				iSelect = m_pAniNode->m_iTmKeys - 1;
				CurrentTmInfo = m_pAniNode->m_pTmKeys[iSelect].m_TmInfo;

			}
			else if(iSelect == 0)
			{
				CurrentTmInfo = m_pAniNode->m_pTmKeys[iSelect].m_TmInfo;
			}
			else
			{

				iBeforeKey = iSelect - 1;
				iBeforeFrame = m_pAniNode->m_pTmKeys[iBeforeKey].m_iFrame;

				iNextKey = iSelect;
				iNextFrame = m_pAniNode->m_pTmKeys[iNextKey].m_iFrame;

				fInterval = (fFrame - iBeforeFrame) / (float)(iNextFrame - iBeforeFrame);
				
				D3DXVECTOR3 vecInterval;

				// Trans
				vecInterval.x = m_pAniNode->m_pTmKeys[iNextKey].m_TmInfo.m_vecTrans.x - m_pAniNode->m_pTmKeys[iBeforeKey].m_TmInfo.m_vecTrans.x;
				vecInterval.y = m_pAniNode->m_pTmKeys[iNextKey].m_TmInfo.m_vecTrans.y - m_pAniNode->m_pTmKeys[iBeforeKey].m_TmInfo.m_vecTrans.y;
				vecInterval.z = m_pAniNode->m_pTmKeys[iNextKey].m_TmInfo.m_vecTrans.z - m_pAniNode->m_pTmKeys[iBeforeKey].m_TmInfo.m_vecTrans.z;

				vecInterval *= fInterval;

				CurrentTmInfo.m_vecTrans.x = m_pAniNode->m_pTmKeys[iBeforeKey].m_TmInfo.m_vecTrans.x + vecInterval.x;
				CurrentTmInfo.m_vecTrans.y = m_pAniNode->m_pTmKeys[iBeforeKey].m_TmInfo.m_vecTrans.y + vecInterval.y;
				CurrentTmInfo.m_vecTrans.z = m_pAniNode->m_pTmKeys[iBeforeKey].m_TmInfo.m_vecTrans.z + vecInterval.z;
				// Scale
				
				vecInterval.x = m_pAniNode->m_pTmKeys[iNextKey].m_TmInfo.m_vecScale.x - m_pAniNode->m_pTmKeys[iBeforeKey].m_TmInfo.m_vecScale.x;
				vecInterval.y = m_pAniNode->m_pTmKeys[iNextKey].m_TmInfo.m_vecScale.y - m_pAniNode->m_pTmKeys[iBeforeKey].m_TmInfo.m_vecScale.y;
				vecInterval.z = m_pAniNode->m_pTmKeys[iNextKey].m_TmInfo.m_vecScale.z - m_pAniNode->m_pTmKeys[iBeforeKey].m_TmInfo.m_vecScale.z;

				vecInterval *= fInterval;

				CurrentTmInfo.m_vecScale.x = m_pAniNode->m_pTmKeys[iBeforeKey].m_TmInfo.m_vecScale.x + vecInterval.x;
				CurrentTmInfo.m_vecScale.y = m_pAniNode->m_pTmKeys[iBeforeKey].m_TmInfo.m_vecScale.y + vecInterval.y;
				CurrentTmInfo.m_vecScale.z = m_pAniNode->m_pTmKeys[iBeforeKey].m_TmInfo.m_vecScale.z + vecInterval.z;
				// qrot

				D3DXQuaternionSlerp(&CurrentTmInfo.m_qRot,
										&(m_pAniNode->m_pTmKeys[iBeforeKey].m_TmInfo.m_qRot),
										&(m_pAniNode->m_pTmKeys[iNextKey].m_TmInfo.m_qRot),
										fInterval);

				// qscaleaxis
				D3DXQuaternionSlerp(&CurrentTmInfo.m_qScaleAxis,
						&(m_pAniNode->m_pTmKeys[iBeforeKey].m_TmInfo.m_qScaleAxis),
						&(m_pAniNode->m_pTmKeys[iNextKey].m_TmInfo.m_qScaleAxis),
						fInterval);
				// fDetermination

				vecInterval.x = m_pAniNode->m_pTmKeys[iNextKey].m_TmInfo.m_fDetermination - m_pAniNode->m_pTmKeys[iBeforeKey].m_TmInfo.m_fDetermination;
				vecInterval.x *= fInterval;
				CurrentTmInfo.m_fDetermination = m_pAniNode->m_pTmKeys[iBeforeKey].m_TmInfo.m_fDetermination + vecInterval.x;

			}

			/* Convert matrix from affine value.
			ptm = position component (t)
			rtm = "essential" rotation (q)
			srtm = "stretch" rotation (u)
			stm = scale component (k)
			ftm = the flip tm -> ScaleMatrix(Point3(ap.f,ap.f,ap.f)); 
			Here's the correct way of reassembling the decomposed matrix:
			Matrix3 srtm, rtm, ptm, stm, ftm;
			ptm.IdentityMatrix();
			ptm.SetTrans(ap.t); 
			ap.q.MakeMatrix(rtm);
			ap.u.MakeMatrix(srtm);
			stm = ScaleMatrix(ap.k);
			mat = Inverse(srtm) * stm * srtm * rtm * ftm * ptm;

			*/

			
			D3DXMATRIX matR,matSA,matScale,matF,matP,matInvSA;

			D3DXMatrixScaling(&matF,CurrentTmInfo.m_fDetermination,
				CurrentTmInfo.m_fDetermination,
				CurrentTmInfo.m_fDetermination);
			D3DXMatrixIdentity(&matP);
			D3DXMatrixTranslation(&matP,CurrentTmInfo.m_vecTrans.x,
				CurrentTmInfo.m_vecTrans.y,
				CurrentTmInfo.m_vecTrans.z);
			D3DXMatrixRotationQuaternion(&matR,&CurrentTmInfo.m_qRot);
			D3DXMatrixRotationQuaternion(&matSA,&CurrentTmInfo.m_qScaleAxis);
			D3DXMatrixInverse(&matInvSA,NULL,&matSA);
			D3DXMatrixScaling(&matScale,CurrentTmInfo.m_vecScale.x,
				CurrentTmInfo.m_vecScale.y,
				CurrentTmInfo.m_vecScale.z);
				
			D3DXMatrixMultiply(&m_matNodeTm,&matInvSA,&matScale);
			D3DXMatrixMultiply(&m_matNodeTm,&m_matNodeTm,&matSA);
			D3DXMatrixMultiply(&m_matNodeTm,&m_matNodeTm,&matR);
			D3DXMatrixMultiply(&m_matNodeTm,&m_matNodeTm,&matF);
			D3DXMatrixMultiply(&m_matNodeTm,&m_matNodeTm,&matP);
			
			

		}
		else
		{
			if(m_pAniNode->m_bPosAni)
			{
				for(i = 0; i < m_pAniNode->m_iPosKeys; i++ )
				{
					if(m_pAniNode->m_pPosKeys[i].m_iFrame >= iCurrentFrame)
					{
						iSelect = i;
						break;
					}
				}
				if(m_pAniNode->m_iPosKeys == i)
				{
					iSelect = m_pAniNode->m_iPosKeys - 1;
					vecPos = m_pAniNode->m_pPosKeys[iSelect].m_vecPos;

				}
				else if(iSelect == 0)
				{
					vecPos = m_pAniNode->m_pPosKeys[iSelect].m_vecPos;
				}
				else
				{

					iBeforeKey = iSelect - 1;
					iBeforeFrame = m_pAniNode->m_pPosKeys[iBeforeKey].m_iFrame;

					iNextKey = iSelect;
					iNextFrame = m_pAniNode->m_pPosKeys[iNextKey].m_iFrame;

					fInterval = (fFrame - iBeforeFrame) / (float)(iNextFrame - iBeforeFrame);
					
					vecInterval.x = m_pAniNode->m_pPosKeys[iNextKey].m_vecPos.x - m_pAniNode->m_pPosKeys[iBeforeKey].m_vecPos.x;
					vecInterval.y = m_pAniNode->m_pPosKeys[iNextKey].m_vecPos.y - m_pAniNode->m_pPosKeys[iBeforeKey].m_vecPos.y;
					vecInterval.z = m_pAniNode->m_pPosKeys[iNextKey].m_vecPos.z - m_pAniNode->m_pPosKeys[iBeforeKey].m_vecPos.z;

					vecInterval *= fInterval;

					vecPos.x = m_pAniNode->m_pPosKeys[iBeforeKey].m_vecPos.x + vecInterval.x;
					vecPos.y = m_pAniNode->m_pPosKeys[iBeforeKey].m_vecPos.y + vecInterval.y;
					vecPos.z = m_pAniNode->m_pPosKeys[iBeforeKey].m_vecPos.z + vecInterval.z;

				}
				
			}
			else
				vecPos = m_DefaultNodeTm.m_vecTrans;
			
			if(m_pAniNode->m_bScaleAni)
			{
				iSelect = 0;

				for(i = 0; i < m_pAniNode->m_iScaleKeys; i++ )
				{
					if(m_pAniNode->m_pScaleKeys[i].m_iFrame >= iCurrentFrame)
					{
						iSelect = i;
						break;
					}
				}
				if(m_pAniNode->m_iScaleKeys == i)
				{
					iSelect = m_pAniNode->m_iScaleKeys - 1;
					vecScale = m_pAniNode->m_pScaleKeys[iSelect].m_vecScale;
				}
				else if(iSelect == 0)
				{
					vecScale = m_pAniNode->m_pScaleKeys[iSelect].m_vecScale;
				}
				else
				{
					iBeforeKey = iSelect - 1;
					iBeforeFrame = m_pAniNode->m_pScaleKeys[iBeforeKey].m_iFrame;

					iNextKey = iSelect;
					iNextFrame = m_pAniNode->m_pScaleKeys[iNextKey].m_iFrame;


					
					fInterval = (fFrame - iBeforeFrame) / (float)(iNextFrame - iBeforeFrame);
					
					vecInterval.x = m_pAniNode->m_pScaleKeys[iNextKey].m_vecScale.x - m_pAniNode->m_pScaleKeys[iBeforeKey].m_vecScale.x;
					vecInterval.y = m_pAniNode->m_pScaleKeys[iNextKey].m_vecScale.y - m_pAniNode->m_pScaleKeys[iBeforeKey].m_vecScale.y;
					vecInterval.z = m_pAniNode->m_pScaleKeys[iNextKey].m_vecScale.z - m_pAniNode->m_pScaleKeys[iBeforeKey].m_vecScale.z;

					vecInterval *= fInterval;

					vecScale.x = m_pAniNode->m_pScaleKeys[iBeforeKey].m_vecScale.x + vecInterval.x;
					vecScale.y = m_pAniNode->m_pScaleKeys[iBeforeKey].m_vecScale.y + vecInterval.y;
					vecScale.z = m_pAniNode->m_pScaleKeys[iBeforeKey].m_vecScale.z + vecInterval.z;


				}

				
			}
			else
				vecScale = m_DefaultNodeTm.m_vecScale;

			if(m_pAniNode->m_bRotAni)
			{
				iSelect = 0;
				for(i = 0; i < m_pAniNode->m_iRotKeys; i++ )
				{
					if(m_pAniNode->m_pRotKeys[i].m_iFrame >= iCurrentFrame)
					{
						iSelect = i;
						break;
					}
				}
				if(m_pAniNode->m_iRotKeys == i)
				{
					iSelect = m_pAniNode->m_iRotKeys - 1;
					qRot = m_pAniNode->m_pRotKeys[iSelect].m_qRotKey;
				}
				else if(iSelect == 0)
				{
					qRot = m_pAniNode->m_pRotKeys[iSelect].m_qRotKey;
				}
				else
				{
					iBeforeKey = iSelect - 1;
					iBeforeFrame = m_pAniNode->m_pRotKeys[iBeforeKey].m_iFrame;

					iNextKey = iSelect;
					iNextFrame = m_pAniNode->m_pRotKeys[iNextKey].m_iFrame;



					fInterval = (fFrame - iBeforeFrame) / (float)(iNextFrame - iBeforeFrame);
					D3DXQuaternionSlerp(&qRot,
										&(m_pAniNode->m_pRotKeys[iBeforeKey].m_qRotKey),
										&(m_pAniNode->m_pRotKeys[iNextKey].m_qRotKey),
										fInterval);
					

				}


			}
			else
				qRot = m_DefaultNodeTm.m_qRot;

			
			D3DXMatrixTransformation(&m_matNodeTm,
				NULL,NULL,&vecScale,NULL,&qRot,&vecPos);

		} // else
		if(m_pAniNode->m_bVisAni)
		{

			float fVis = 1.0f;
			float fInter = 0.0f;

			for(i = 0; i < m_pAniNode->m_iVisKeys; i++ )
			{
				if(m_pAniNode->m_pVisKeys[i].m_iFrame >= iCurrentFrame)
				{
					iSelect = i;
					break;
				}
			}
			if(m_pAniNode->m_iVisKeys == i)
			{
				iSelect = m_pAniNode->m_iVisKeys - 1;
				fVis = m_pAniNode->m_pVisKeys[iSelect].m_fVisibility;

			}
			else if(iSelect == 0)
			{
				fVis = m_pAniNode->m_pVisKeys[iSelect].m_fVisibility;
			}
			else
			{

				iBeforeKey = iSelect - 1;
				iBeforeFrame = m_pAniNode->m_pVisKeys[iBeforeKey].m_iFrame;

				iNextKey = iSelect;
				iNextFrame = m_pAniNode->m_pVisKeys[iNextKey].m_iFrame;

				fInterval = (fFrame - iBeforeFrame) / (float)(iNextFrame - iBeforeFrame);
				
				fInter = m_pAniNode->m_pVisKeys[iNextKey].m_fVisibility - m_pAniNode->m_pVisKeys[iBeforeKey].m_fVisibility;
			
				fInter *= fInterval;

				fVis = m_pAniNode->m_pVisKeys[iBeforeKey].m_fVisibility + fInter;
			
			
			}
			m_fVisibility = fVis;

		}
	}
	else{
	/*	vecPos = m_DefaultNodeTm.m_vecTrans;
		qRot = m_DefaultNodeTm.m_qRot;
		vecScale = m_DefaultNodeTm.m_vecScale;

		D3DXMatrixTransformation(&m_matNodeTm,
			NULL,NULL,&vecScale,NULL,&qRot,&vecPos);
	*/
	/*	D3DXMATRIX mat;

		D3DXMatrixInverse(&mat,NULL,&m_matInvNodeTm);
		D3DXMatrixMultiply(&m_matNodeTm,&m_matNodeTm,&mat);
	*/
		D3DXMatrixInverse(&m_matNodeTm,NULL,&m_matInvNodeTm);
	}

	if(matParent)
	{

		if(m_bWorldTM)
		{
			D3DXMATRIX matInv;
			D3DXMatrixInverse(&matInv,NULL,matParent);				// exporter에서 (bipe 만 ) 처리 해야함.
			D3DXMatrixMultiply(&m_matNodeTm,&m_matNodeTm,&matInv); // LocalTm = WorldTm * Parent InvTm
			
		}
		m_matLocalTm = m_matNodeTm;

		D3DXMatrixMultiply(&m_matNodeTm,&m_matNodeTm,matParent); // WorldTm = LocalTm * ParentTm
	}
}

void CMeshObjNode::UpdateSkinning()
{
	int i;
	D3DXMATRIX matWorldTm;
	D3DXMATRIX matInvNodeTm;

	int iVertexIndex;
	int iMatIndex;

	if(m_bSkinMorph && m_pSkinData)
	{
		for(i = 0; i < m_pSkinData->m_iRigid; i++)
		{
			matWorldTm = m_pSkinData->m_pRigid[i].m_pNode->m_matNodeTm;
			matInvNodeTm = m_pSkinData->m_pRigid[i].m_pNode->m_matInvNodeTm;
		
			iVertexIndex = m_pSkinData->m_pRigid[i].m_iVertexIndex;
			iMatIndex = m_pSkinData->m_pRigid[i].m_iMatIndex;

			SetSkinMeshData(iMatIndex,iVertexIndex,&matWorldTm,&matInvNodeTm);
			
		}
		for(i = 0; i < m_pSkinData->m_iBlend; i++ )
		{
			iVertexIndex = m_pSkinData->m_pBlend[i].m_iVertexIndex;
			iMatIndex = m_pSkinData->m_pBlend[i].m_iMatIndex;
			int iBlend = m_pSkinData->m_pBlend[i].m_iBlendData;
			CMeshSkinData::CMeshBlendData *pBlend = m_pSkinData->m_pBlend[i].m_pArrBlendData;
			SetSkinMeshData(iMatIndex,iVertexIndex,pBlend,iBlend);

			
		}
	}

}
}}