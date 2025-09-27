// SMRObj.cpp: implementation of the CSMRObj class.
//
//////////////////////////////////////////////////////////////////////

#include "SMRObj.h"
#include "SceneManager.h"
#include "GMMemory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const DWORD dwExportMeshVertex = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

void CSMRObj::GetAniNode(SGEngine::Core::CMeshObjNode *pNode,FILE *fp)
{
	bool bAni = false;
	bool bWorldTM = false;
	bool bSkin = false;

	int iNum[8];
	int iAniNum[14];

	fread(iNum,sizeof(int) * 8,1,fp);
	pNode->SetNodeID(iNum[0]);
	pNode->SetType(iNum[2],iNum[3],iNum[4]);
	pNode->SetChildNum(iNum[1]);
	
	bAni = (iNum[5] == 1) ? true : false;
	bWorldTM = (iNum[6] == 1) ? true : false;
	bSkin = (iNum[7] == 1) ? true : false;


	pNode->SetWorldTMFlag(bWorldTM);


	fread(pNode->GetNodeName(),sizeof(char) * 256,1,fp);
	fread(&(pNode->m_DefaultNodeTm),sizeof(SGEngine::Core::CNodeTMInfo),1,fp);
	fread(&(pNode->m_matNodeTm),sizeof(D3DXMATRIX),1,fp);
	D3DXMatrixInverse(&(pNode->m_matInvNodeTm),NULL,&(pNode->m_matNodeTm));


	if(bAni)
	{
		SGEngine::Core::CMeshKeyAniNode *pAniNode = new SGEngine::Core::CMeshKeyAniNode;
		fread(iAniNum,sizeof(int) * 14,1,fp);

		pAniNode->m_bPosAni = (iAniNum[0] == 1) ? true : false;
		pAniNode->m_bRotAni = (iAniNum[1] == 1) ? true : false;
		pAniNode->m_bScaleAni = (iAniNum[2] == 1) ? true : false;
		pAniNode->m_bVisAni = (iAniNum[3] == 1) ? true : false;

		pAniNode->m_iPosKeys = iAniNum[4];
		pAniNode->m_iPosType = iAniNum[5];

		pAniNode->m_iRotKeys = iAniNum[6];
		pAniNode->m_iRotType = iAniNum[7];

		pAniNode->m_iScaleKeys = iAniNum[8];
		pAniNode->m_iScaleType = iAniNum[9];

		pAniNode->m_iVisKeys = iAniNum[10];
		pAniNode->m_iVisType = iAniNum[11];

		pAniNode->m_bTmAni = (iAniNum[12] == 1) ? true : false;
		pAniNode->m_iTmKeys = iAniNum[13];

		if(pAniNode->m_iPosKeys > 0)
		{
			pAniNode->m_pPosKeys = new SGEngine::Core::CMeshKeyAniNode::CMeshKeyAniPos[pAniNode->m_iPosKeys];
			fread(pAniNode->m_pPosKeys,sizeof(SGEngine::Core::CMeshKeyAniNode::CMeshKeyAniPos) * pAniNode->m_iPosKeys,1,fp);
		}
		if(pAniNode->m_iRotKeys > 0)
		{
			pAniNode->m_pRotKeys = new SGEngine::Core::CMeshKeyAniNode::CMeshKeyAniRot[pAniNode->m_iRotKeys];
			fread(pAniNode->m_pRotKeys,sizeof(SGEngine::Core::CMeshKeyAniNode::CMeshKeyAniRot) *pAniNode->m_iRotKeys,1,fp);

		}
		if(pAniNode->m_iScaleKeys > 0)
		{
			pAniNode->m_pScaleKeys = new SGEngine::Core::CMeshKeyAniNode::CMeshKeyAniScale[pAniNode->m_iScaleKeys ];
			fread(pAniNode->m_pScaleKeys,sizeof(SGEngine::Core::CMeshKeyAniNode::CMeshKeyAniScale) *pAniNode->m_iScaleKeys ,1,fp);

		}
		if(pAniNode->m_iVisKeys > 0)
		{
			pAniNode->m_pVisKeys = new SGEngine::Core::CMeshKeyAniNode::CMeshKeyAniVis[pAniNode->m_iVisKeys];
			fread(pAniNode->m_pVisKeys,sizeof(SGEngine::Core::CMeshKeyAniNode::CMeshKeyAniVis) *pAniNode->m_iVisKeys,1,fp);

		}
		if(pAniNode->m_iTmKeys > 0)
		{
			pAniNode->m_pTmKeys = new SGEngine::Core::CMeshKeyAniNode::CMeshKeyAniTm[pAniNode->m_iTmKeys];
			fread(pAniNode->m_pTmKeys,sizeof(SGEngine::Core::CMeshKeyAniNode::CMeshKeyAniTm) * pAniNode->m_iTmKeys,1,fp);
		}
		pNode->SetAniNode(pAniNode);

	}
	if(bSkin)
	{
		SGEngine::Core::CMeshSkinData *pSkinData = new SGEngine::Core::CMeshSkinData;
		int iSkinNum[2];
		fread(iSkinNum,sizeof(int) * 2, 1,fp);

		pSkinData->m_iRigid = iSkinNum[0];
	
		pSkinData->m_iBlend = iSkinNum[1];

		if(iSkinNum[0])
		{
			pSkinData->m_pRigid = new SGEngine::Core::CMeshSkinData::CMeshSkinRigid[iSkinNum[0]];
			fread(pSkinData->m_pRigid,sizeof(SGEngine::Core::CMeshSkinData::CMeshSkinRigid) * iSkinNum[0],1,fp);
		}
		if(iSkinNum[1])
		{
			pSkinData->m_pBlend = new SGEngine::Core::CMeshSkinData::CMeshSkinBlend[iSkinNum[1]];
			for(int iBlend = 0; iBlend < iSkinNum[1]; iBlend++)
			{
				int iBlendNum[3];

				fread(iBlendNum,sizeof(int) * 3, 1,fp);
				pSkinData->m_pBlend[iBlend].m_iMatIndex = iBlendNum[0];
				pSkinData->m_pBlend[iBlend].m_iVertexIndex = iBlendNum[1];
				pSkinData->m_pBlend[iBlend].m_iBlendData = iBlendNum[2];
				
				if(iBlendNum[2])
				{
					pSkinData->m_pBlend[iBlend].m_pArrBlendData = new SGEngine::Core::CMeshSkinData::CMeshBlendData[iBlendNum[2]];
					fread(pSkinData->m_pBlend[iBlend].m_pArrBlendData,sizeof(SGEngine::Core::CMeshSkinData::CMeshBlendData) * iBlendNum[2],1,fp);

				}

			}
			
		}
		pNode->SetSkinData(pSkinData);
	}
	for(int i=  0; i < pNode->GetChildNum(); i++)
		GetAniNode(pNode->GetChildNode(i),fp);
	
	
	
}
void CSMRObj::AddMesh(SGEngine::Core::CMeshCoreData *pMesh,SGEngine::Core::CMeshObjNode *pNode)
{
	if(pNode && pMesh)
	{
		if(!strcmp(pNode->GetNodeName() , pMesh->m_strName))
		{
			pNode->AddMeshData(pMesh);
			
			if(pNode->ISSkinData())
			{
				pMesh->CreateDefaultBuffer();

			}
			return;
		}
	
		for(int i = 0; i < pNode->GetChildNum(); i++ )
		{
			AddMesh(pMesh,pNode->GetChildNode(i));
		}
	}
}

void CSMRObj::Load(const char *strFileName)
{
	FILE *fp = fopen(strFileName,"rb");

	int i = 0;
	int j = 0;

	if(fp)
	{
		fread(&m_MeshHeader,sizeof(SGEngine::Core::CMeshHeader),1,fp);

		m_iFrameInfo[0] = m_MeshHeader.m_iExtension[0];			// Start Frame
		m_iFrameInfo[1] = m_MeshHeader.m_iExtension[1];			// End Frame
		m_iFrameInfo[2] = m_MeshHeader.m_iExtension[2];			// Frame Rate
		m_iFrameInfo[3] = m_MeshHeader.m_iExtension[3];			// TickPerFrame

		for(i = 0; i < m_MeshHeader.m_iNodes; i++ )
		{
			SGEngine::Core::CMeshObjNodeHeader ObjHeader;
			fread(&ObjHeader,sizeof(SGEngine::Core::CMeshObjNodeHeader),1,fp);

			if(ObjHeader.m_iNodeType == SGEngine::MESHNODETYPE_MATERIAL)
			{
				m_iMaterial = ObjHeader.m_iNodeSize;
				if(m_iMaterial)
				{
					m_pMaterial = new SGEngine::Core::CMeshMaterialInfo[m_iMaterial];
					fread(m_pMaterial,sizeof(SGEngine::Core::CMeshMaterialInfo) * m_iMaterial,1,fp);
					CTexture *pTexture = NULL;
					CTexture::SetPath(EFFECTTEXTUREPATH);

					for(j = 0; j < m_iMaterial; j++ )
					{
					
						pTexture = new CTexture;
						pTexture->Load(m_pMaterial[j].m_strName);
						m_lstTexture.push_back(pTexture);

					}
					
					

				}
			}
			else if(ObjHeader.m_iNodeType == SGEngine::MESHNODETYPE_MESHDATA)
			{
				int iNum[6];
				m_iMeshData = ObjHeader.m_iNodeSize;
				m_pMeshData = new SGEngine::Core::CMeshCoreData[m_iMeshData];
				m_pVertexBuffer = new LPDIRECT3DVERTEXBUFFER8[m_iMeshData];
				m_pIndexBuffer= new LPDIRECT3DINDEXBUFFER8[m_iMeshData];

				for(j = 0 ; j < (int)ObjHeader.m_iNodeSize; j++ )
				{

					fread(iNum,sizeof(int) * 6,1,fp);
					m_pMeshData[j].m_iVerticesNum = iNum[2];
					m_pMeshData[j].m_iIndicesNum = iNum[3];
					m_pMeshData[j].m_iMaterialIndex = iNum[1];
					m_pMeshData[j].m_iType = iNum[0];
					m_pMeshData[j].m_bMorph = (iNum[4] == 1) ? true : false;
					m_pMeshData[j].m_bSMorph = (iNum[5] == 1) ? true : false;

					fread(m_pMeshData[j].m_strName,sizeof(char) * 256, 1,fp);
					if(strchr(m_pMeshData[j].m_strName,'C'))
					{
						m_pMeshData[j].SetTotalBilboard(true);
					}
					if(strchr(m_pMeshData[j].m_strName,'Z'))
					{
						m_pMeshData[j].SetZWriteOffFlag(true);
					}
					if(strchr(m_pMeshData[j].m_strName,'W'))
					{
						m_pMeshData[j].SetCullOffFlag(true);
					}
					if(strchr(m_pMeshData[j].m_strName,'K'))
					{
						m_pMeshData[j].SetZOffFlag(true);
					}
					m_pMeshData[j].CreateBuffer();
					fread(m_pMeshData[j].m_pIndices,sizeof(WORD) * m_pMeshData[j].m_iIndicesNum,1,fp);
					fread(m_pMeshData[j].m_pVertices,sizeof(SGEngine::CExportMeshVertex) * m_pMeshData[j].m_iVerticesNum,1,fp);
					if(m_pMeshData[j].m_bMorph)
					{
						fread(&m_pMeshData[j].m_iMorph,sizeof(int), 1,fp);
						m_pMeshData[j].m_pMorphData = new SGEngine::Core::CMeshCoreData::CMeshMorphData[m_pMeshData[j].m_iMorph];
						for(int iMorph= 0; iMorph < m_pMeshData[j].m_iMorph; iMorph++)
						{
							
							fread(&m_pMeshData[j].m_pMorphData[iMorph].m_iFrame,sizeof(int),1,fp);
							m_pMeshData[j].m_pMorphData[iMorph].m_pVertices = new SGEngine::CExportMeshVertex[m_pMeshData[j].m_iVerticesNum]; 

							fread(m_pMeshData[j].m_pMorphData[iMorph].m_pVertices,sizeof(SGEngine::CExportMeshVertex) * m_pMeshData[j].m_iVerticesNum,1,fp);

						}
						
					}
					if(m_pMeshData[j].m_bSMorph)
					{
						m_pMeshData[j].CreateDefaultBuffer();
						fread(&(m_pMeshData[j].m_iSMorph),sizeof(int),1,fp);
						m_pMeshData[j].m_pSMorph = new SGEngine::Core::CMeshCoreData::CMeshSMorphData[m_pMeshData[j].m_iSMorph];
						m_pMeshData[j].m_pfSMorphInterValue = new float[m_pMeshData[j].m_iSMorph];
						for(int ism = 0;ism < m_pMeshData[j].m_iSMorph; ism++)
						{
							m_pMeshData[j].m_pfSMorphInterValue[ism] = 0.0f;
							int iSMNum[2];
							fread(m_pMeshData[j].m_pSMorph[ism].m_strName,sizeof(char) * 256, 1,fp);
							fread(iSMNum,sizeof(int) * 2,1,fp);
							m_pMeshData[j].m_pSMorph[ism].m_iModelID = iSMNum[0];
							m_pMeshData[j].m_pSMorph[ism].m_iRelativeNum = iSMNum[1];	
							m_pMeshData[j].m_pSMorph[ism].m_pRelativeVertices = new D3DXVECTOR3[iSMNum[1]];
							m_pMeshData[j].m_pSMorph[ism].m_pRelativeIndex = new int[iSMNum[1]];
							fread(m_pMeshData[j].m_pSMorph[ism].m_pRelativeVertices,sizeof(D3DXVECTOR3) * iSMNum[1],1,fp);
							fread(m_pMeshData[j].m_pSMorph[ism].m_pRelativeIndex,sizeof(int) * iSMNum[1],1,fp);

						}

					}
					CSceneManager::GetDevice()->CreateVertexBuffer(m_pMeshData[j].m_iVerticesNum * sizeof(SGEngine::CExportMeshVertex),D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
									D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1,D3DPOOL_DEFAULT,&(m_pVertexBuffer[j]));

					SGEngine::CExportMeshVertex *pVert = NULL;
					
					m_pVertexBuffer[j]->Lock(0,0,(BYTE **) &pVert,0);
					memcpy(pVert,m_pMeshData[j].m_pVertices,sizeof(SGEngine::CExportMeshVertex) * m_pMeshData[j].m_iVerticesNum);
					m_pVertexBuffer[j]->Unlock();

					CSceneManager::GetDevice()->CreateIndexBuffer(sizeof(WORD) * m_pMeshData[j].m_iIndicesNum,D3DUSAGE_WRITEONLY,D3DFMT_INDEX16,D3DPOOL_DEFAULT,&m_pIndexBuffer[j]);

					WORD *pWord = NULL;
					
					m_pIndexBuffer[j]->Lock(0,0,(BYTE **)&pWord,0);
					memcpy(pWord,m_pMeshData[j].m_pIndices,sizeof(WORD) * m_pMeshData[j].m_iIndicesNum);
					m_pIndexBuffer[j]->Unlock();

					

				}
			}
			else if(ObjHeader.m_iNodeType == SGEngine::MESHNODETYPE_ANINODE)
			{
				m_pRootNode = new SGEngine::Core::CMeshObjNode;
				GetAniNode(m_pRootNode,fp);
				
			}
		}
		fclose(fp);
		for(i = 0; i < m_iMeshData; i++ )
		{
			AddMesh(&m_pMeshData[i],m_pRootNode);
		}
		SetSkinningLink(m_pRootNode);
	}
}
SGEngine::Core::CMeshObjNode *CSMRObj::FindNode(const char *strName,SGEngine::Core::CMeshObjNode *pNode)
{
	int i;
	if(pNode)
	{
		if(pNode->IsNodeSameName(strName))
			return pNode;	
	}
	for(i = 0; i < pNode->GetChildNum(); i++ )
	{
		SGEngine::Core::CMeshObjNode * pN = NULL;
		if((pN = FindNode(strName,pNode->GetChildNode(i))) != NULL)
			return pN;
	}
	return NULL;

}
void CSMRObj::SetSkinningLink(SGEngine::Core::CMeshObjNode *pNode)
{
	int i,j;

	if(pNode->ISSkinData())
	{
		SGEngine::Core::CMeshSkinData *pSkinData = pNode->GetSkinData();
		
		for(i = 0; i < pSkinData->m_iRigid; i++ )
		{
			pSkinData->m_pRigid[i].m_pNode = FindNode(pSkinData->m_pRigid[i].m_strNodeName,m_pRootNode);
			
		}
		for(i =0; i < pSkinData->m_iBlend; i++ )
		{
			for(j = 0; j < (pSkinData->m_pBlend->m_iBlendData); j++ )
			{
				pSkinData->m_pBlend[i].m_pArrBlendData[j].m_pNode = 
					FindNode(pSkinData->m_pBlend[i].m_pArrBlendData[j].m_strNodeName,m_pRootNode);
			}
		}
		
	}
	for(i = 0; i < pNode->GetChildNum(); i++ )
	{
		SetSkinningLink(pNode->GetChildNode(i));
	}
}
void CSMRObj::UpdateSkinning(SGEngine::Core::CMeshObjNode *pNode)
{
	if(pNode->ISSkinData())
	{
		pNode->UpdateSkinning();
	}
	for(int i = 0; i < pNode->GetChildNum(); i++ )
	{
		UpdateSkinning(pNode->GetChildNode(i));
	}
		
}
void CSMRObj::UpdateBuffer(const float &fFrame)
{
	int i;

	SGEngine::CExportMeshVertex *pVert = NULL;
					
	for(i = 0; i < m_iMeshData; i++ )
	{
		m_pMeshData[i].UpdateMeshMorph(fFrame);
		m_pMeshData[i].UpdateMeshSmartMorph();

		if(m_pMeshData[i].m_bUpdateData)
		{
			m_pMeshData[i].SetUpdateFlag(false);
			pVert = NULL;
					
			m_pVertexBuffer[i]->Lock(0,0,(BYTE **) &pVert,0);
			memcpy(pVert,m_pMeshData[i].m_pVertices,sizeof(SGEngine::CExportMeshVertex) * m_pMeshData[i].m_iVerticesNum);
			m_pVertexBuffer[i]->Unlock();

		}
	}
}
void CSMRObj::UpdateTm(SGEngine::Core::CMeshObjNode *pNode,const float &fFrame)
{
	if(pNode->GetParentNode())
		pNode->UpdateTm(fFrame,&(pNode->GetParentNode()->m_matNodeTm));
	else
		pNode->UpdateTm(fFrame,NULL);
	for(int i = 0; i < pNode->GetChildNum(); i++ )
	{
		UpdateTm(pNode->GetChildNode(i),fFrame);
	}
}


void CSMRObj::Render(LPDIRECT3DDEVICE8 lpDevice,D3DXVECTOR3 vecPos,D3DXVECTOR3 vecScale,D3DXQUATERNION quatRot,float fFrame)
{
	


	int i = 0;
	float iFrame = fFrame;

	
	// Scale, Pos ,setting
//	D3DXMATRIX matWorld;
//	D3DXMatrixIdentity(&matWorld);
//	matWorld._41 = vecPos.x;
//	matWorld._42 = vecPos.y;
//	matWorld._43 = vecPos.z;
//	matWorld._11 = vecScale.x;
//	matWorld._22 = vecScale.y;
//	matWorld._33 = vecScale.z;
	D3DXMATRIX matWorld, matScale, matRot;
	D3DXMatrixScaling( &matScale, vecScale.x, vecScale.y, vecScale.z );
	D3DXMatrixRotationQuaternion( &matRot, &quatRot );
	D3DXMatrixMultiply( &matWorld, &matRot, &matScale );
	matWorld._41 = vecPos.x;
	matWorld._42 = vecPos.y;
	matWorld._43 = vecPos.z;


	/*m_pRootNode->m_matNodeTm._41 = vecPos.x;
	m_pRootNode->m_matNodeTm._42 = vecPos.y;
	m_pRootNode->m_matNodeTm._43 = vecPos.z;
	
	m_pRootNode->m_matNodeTm._11 = vecScale.x;
	m_pRootNode->m_matNodeTm._22 = vecScale.y;
	m_pRootNode->m_matNodeTm._33 = vecScale.z;
	*/
	

	UpdateTm(m_pRootNode,iFrame);
	UpdateSkinning(m_pRootNode);				// 순서 지켜주어야 한다.
	UpdateBuffer(iFrame);


	if(iFrame > (float)m_iFrameInfo[1])
	{
		iFrame = (float)m_iFrameInfo[0];

	}
	if(iFrame < (float)m_iFrameInfo[0])
		iFrame = (float)m_iFrameInfo[1];



	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity(&matIdentity);

	lpDevice->SetPixelShader(NULL);
	lpDevice->SetVertexShader(dwExportMeshVertex);

	for(i = 0; i < m_iMeshData; i++ )
	{
		if(m_pMeshData[i].m_iVerticesNum && m_pMeshData[i].m_iIndicesNum)
		{
			if(m_pMeshData[i].m_pNode)
			{
			
				if(m_pMeshData[i].GetTotalBilboard())
				{		// bilboard

				
					D3DXMATRIX view;
					D3DXMATRIX invers_view;

					lpDevice->GetTransform(D3DTS_VIEW,&view);
					view._41 = view._42 = view._43 =0.0;
					D3DXMatrixInverse(&invers_view,NULL,&view);
					D3DXMatrixMultiply(&(m_pMeshData[i].m_pNode->m_matNodeTm),&(m_pMeshData[i].m_pNode->m_matNodeTm),&invers_view);
						

					
				}
				D3DXMatrixMultiply(&(m_pMeshData[i].m_pNode->m_matNodeTm),&(m_pMeshData[i].m_pNode->m_matNodeTm),&matWorld);

				if(m_pMeshData[i].m_pNode->ISSkinData())
				{
					lpDevice->SetTransform(D3DTS_WORLD,&matIdentity);
				}
				else{
					lpDevice->SetTransform(D3DTS_WORLD,&(m_pMeshData[i].m_pNode->m_matNodeTm));

				}

				int iValue = (int)(m_pMeshData[i].m_pNode->m_fVisibility * 255.0f);

				DWORD color = D3DCOLOR_ARGB(iValue,iValue,iValue,iValue);
				lpDevice->SetRenderState(D3DRS_TEXTUREFACTOR,color);
			}
			else
			{

				DWORD color = D3DCOLOR_ARGB(255,255,255,255);
				lpDevice->SetRenderState(D3DRS_TEXTUREFACTOR,color);
			}

			lpDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);

			lpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
			lpDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
			lpDevice->SetRenderState(D3DRS_SRCBLEND,m_dwSrcBlend);
			lpDevice->SetRenderState(D3DRS_DESTBLEND,m_dwDstBlend);
	
			if(m_pMeshData[i].GetCullOffFlag())
				lpDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
			else
				lpDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);

			if(m_pMeshData[i].GetZWriteOffFlag())
			{
				lpDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE);
			}
			else
			{
				lpDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
			}
			if(m_pMeshData[i].GetZOffFlag())
				lpDevice->SetRenderState(D3DRS_ZENABLE,FALSE);
			else
				lpDevice->SetRenderState(D3DRS_ZENABLE,TRUE);

			lpDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TFACTOR);
			lpDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TEXTURE);	
			lpDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
			lpDevice->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);
			lpDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TFACTOR);
			lpDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TEXTURE);
			lpDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
			lpDevice->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);


			lpDevice->SetTexture(0,m_lstTexture[m_pMeshData[i].m_iMaterialIndex]->GetTexture());

			lpDevice->SetStreamSource(0,m_pVertexBuffer[i],sizeof(SGEngine::CExportMeshVertex));
			lpDevice->SetIndices(m_pIndexBuffer[i],0);
			lpDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,m_pMeshData[i].m_iVerticesNum,0,m_pMeshData[i].m_iIndicesNum/3);

			lpDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
			
			lpDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_LESSEQUAL);
			lpDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
			lpDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CW);
			lpDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
			lpDevice->SetRenderState(D3DRS_ZENABLE,TRUE);
			

		}
	}

}
