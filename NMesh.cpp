#include ".\nmesh.h"
#include "./SceneManager.h"
#include "./RenderBoundingBox.h"
#include "../zalla3d base class/BaseGraphicsLayer.h"

#include "GMMemory.h"

namespace CROSSM {
CNMesh::CNMesh(void)
{
	
	m_MaxBox.x=-1.0f;
	m_MinBox.x=-1.0f;
	m_isAlreadyLock=false;
	m_dwConvertShader = MultiFVF;
	m_iObjectMethod = 0;

	m_pMatName.clear();

}

CNMesh::~CNMesh(void)
{
	int i;

	if(CSceneManager::ms_pBufferPools)
	{
		for(i=0;i<(int)m_pVertexBuffer.size();i++)
		{
			if(m_pVertexBuffer[i])
			{
				CSceneManager::ms_pBufferPools->UnRef(Caldron::Scene::D3DBUFFEROBJ_VERTEX,m_pVertexBuffer[i]);
				m_pVertexBuffer[i] = NULL;
			}

		}
		for(i=0;i<(int)m_pIndicesBuffer.size();i++)
		{
			if(m_pIndicesBuffer[i])
			{
				CSceneManager::ms_pBufferPools->UnRef(Caldron::Scene::D3DBUFFEROBJ_INDEX,m_pIndicesBuffer[i]);
				m_pIndicesBuffer[i] = NULL;
			}
		}
	}
	else
	{

		for(i=0;i<(int)m_pVertexBuffer.size();i++)
		{
			if(m_pVertexBuffer[i])
			{	
				m_pVertexBuffer[i]->Release();
				m_pVertexBuffer[i] = NULL;
			}
		}
		for(i=0;i<(int)m_pIndicesBuffer.size();i++)
		{
			if(m_pIndicesBuffer[i])
			{
				m_pIndicesBuffer[i]->Release();
				m_pIndicesBuffer[i] = NULL;
			}
		}
	}
	m_pVertexBuffer.clear();
	m_pIndicesBuffer.clear();


	for(i = 0; i < (int)m_pMat.size(); i++)
	{
		if(m_pMat[i])
		{
			CSceneManager::_ReleaseResource(CROSSM::RESOURCE_TEXTURE,m_pMat[i]);
			m_pMat[i] = NULL;
		}
		
	}
	
	m_pMat.clear();

	for(i = 0; i < (int)m_pMatName.size(); i++)
	{
		if(m_pMatName[i])
		{
			delete[] m_pMatName[i];
		}
	}
	m_pMatName.clear();

	for(i = 0; i < (int)m_pIndices.size(); i++ )
	{
		m_pIndices[i] = NULL;
	}
	m_pIndices.clear();

	for(i = 0; i < (int)m_pVertex.size(); i++ )
	{
		m_pVertex[i] = NULL;
	}
	m_pVertex.clear();

	
	for(i = 0; i < (int)m_pLoadedIndices.size(); i++ )
	{
		if(m_pLoadedIndices[i])
		{
			delete[] m_pLoadedIndices[i];
			m_pLoadedIndices[i] = NULL;

		}
	}
	m_pLoadedIndices.clear();

	for(i = 0; i < (int)m_pLoadedVertex.size(); i++ )
	{
		if(m_pLoadedVertex[i])
		{
			delete[] m_pLoadedVertex[i];
			m_pLoadedVertex[i] = NULL;

		}
	}
	m_pLoadedVertex.clear();
	
}
bool CNMesh::Load()
{
	
	if(!m_pByteDataObj)
		return false;

	int nObject,nMat,ObjectMethod;	
	m_pByteDataObj->Read(&nObject,sizeof(int),1);
	m_pByteDataObj->Read(&nMat,sizeof(int),1);
	m_pByteDataObj->Read(&ObjectMethod,sizeof(int),1);

	m_bRender=true;

	// 렌더링단위별 min-max 박스 계산을 위한 리스트 초기화
	m_vecMeshPieceBoundingBoxInfo.resize(nObject);

	if(COLLISIONOBJECTVIEW==0 && strstr(m_strName,"collision")!=NULL)
	{
		m_bRender=false;
	}
	m_iObjectMethod = ObjectMethod;

	if(ObjectMethod==0)
	{
		m_bCompleteMesh=false;		

		char strTextureName[256];
		//CNTexture::SetPath(OBJECTTEXTUREPATH);
		for(int i = 0; i < nMat; i++ )
		{
			m_pByteDataObj->Read(strTextureName,sizeof(char) * 256,1);
			char *pName = new char[256];
			strcpy(pName,strTextureName);
			m_pMatName.push_back(pName);

			if(strcmp(strTextureName,"")==0)
				continue;
			

			//CNTexture *pTexture = (CNTexture *)CSceneManager::_GetResource(CROSSM::RESOURCE_TEXTURE,strTextureName);
			//m_pMat.push_back(pTexture);
		}
		
		char strObjectName[256];
		int MatRef,cVertex,cIndices;	
	
		for(int i=0;i<nObject;i++)
		{
			m_pByteDataObj->Read(strObjectName,sizeof(char)*256,1);
			
			m_pByteDataObj->Read(&MatRef,sizeof(long),1);
			m_pByteDataObj->Read(&cVertex,sizeof(long),1);
			m_pByteDataObj->Read(&cIndices,sizeof(int),1);		
			
			MultiVertex *pTempVertex=new MultiVertex[cVertex];
			WORD *pTempIndices=new WORD[cIndices*3];

			m_pByteDataObj->Read(pTempVertex,sizeof(MultiVertex) * cVertex,1);
			m_pByteDataObj->Read(pTempIndices,sizeof(WORD)* cIndices * 3,1);
				
			//calc box
			if(m_MaxBox.x==-1.0f && m_MinBox.x==-1.0f)
			{
				m_MinBox.x=m_MaxBox.x=pTempVertex[0].v.x;
				m_MinBox.y=m_MaxBox.y=pTempVertex[0].v.y;
				m_MinBox.z=m_MaxBox.z=pTempVertex[0].v.z;
			}

			// 렌더링단위별 바운딩 min-max 값 계산을 위한 초기화
			m_vecMeshPieceBoundingBoxInfo[i].vBoxMin = pTempVertex[0].v;
			m_vecMeshPieceBoundingBoxInfo[i].vBoxMax = pTempVertex[0].v;

			for(int cv = 0; cv < cVertex; cv++)
			{
				// 전체 메시단위 바운딩 min-max 계산
				m_MinBox.x = min(m_MinBox.x, pTempVertex[cv].v.x);
				m_MinBox.y = min(m_MinBox.y, pTempVertex[cv].v.y);
				m_MinBox.z = min(m_MinBox.z, pTempVertex[cv].v.z);

				m_MaxBox.x = max(m_MaxBox.x, pTempVertex[cv].v.x);
				m_MaxBox.y = max(m_MaxBox.y, pTempVertex[cv].v.y);
				m_MaxBox.z = max(m_MaxBox.z, pTempVertex[cv].v.z);

				// 렌더링단위별 바운딩 min-max 계산
				vector3 &vPerObjectMin = m_vecMeshPieceBoundingBoxInfo[i].vBoxMin;	// alias
				vector3 &vPerObjectMax = m_vecMeshPieceBoundingBoxInfo[i].vBoxMax;	// alias

				vPerObjectMin.x = min(vPerObjectMin.x, pTempVertex[cv].v.x);
				vPerObjectMin.y = min(vPerObjectMin.y, pTempVertex[cv].v.y);
				vPerObjectMin.z = min(vPerObjectMin.z, pTempVertex[cv].v.z);

				vPerObjectMax.x = max(vPerObjectMax.x, pTempVertex[cv].v.x);
				vPerObjectMax.y = max(vPerObjectMax.y, pTempVertex[cv].v.y);
				vPerObjectMax.z = max(vPerObjectMax.z, pTempVertex[cv].v.z);
			}

			m_nVertex.push_back(cVertex);
			m_nIndices.push_back(cIndices);
			m_pMatRef.push_back(MatRef);

			m_pIndices.push_back(pTempIndices);
			m_pVertex.push_back(pTempVertex);

		}
	}
	else if(m_iObjectMethod == 3)
	{
		m_bCompleteMesh=false;		

		char strTextureName[256];
		//CNTexture::SetPath(OBJECTTEXTUREPATH);
		for(int i = 0; i < nMat; i++ )
		{
			m_pByteDataObj->Read(strTextureName,sizeof(char) * 256,1);
			
			char *pName = new char[256];
			strcpy(pName,strTextureName);
			m_pMatName.push_back(pName);

			if(strcmp(strTextureName,"")==0)
				continue;
			//CNTexture *pTexture = (CNTexture *)CSceneManager::_GetResource(CROSSM::RESOURCE_TEXTURE,strTextureName);
			//m_pMat.push_back(pTexture);
		}
		
		char strObjectName[256];
		int MatRef,cVertex,cIndices;	
	
		for(int i=0;i<nObject;i++)
		{
			m_pByteDataObj->Read(strObjectName,sizeof(char)*256,1);
			
			m_pByteDataObj->Read(&MatRef,sizeof(long),1);
			m_pByteDataObj->Read(&cVertex,sizeof(long),1);
			m_pByteDataObj->Read(&cIndices,sizeof(int),1);		
			
			LightVertex *pTempVertex=new LightVertex[cVertex];
			WORD *pTempIndices=new WORD[cIndices*3];

			m_pByteDataObj->Read(pTempVertex,sizeof(LightVertex) * cVertex,1);
			m_pByteDataObj->Read(pTempIndices,sizeof(WORD)* cIndices * 3,1);
				
			//calc box
			if(m_MaxBox.x==-1.0f && m_MinBox.x==-1.0f)
			{
				m_MinBox.x=m_MaxBox.x=pTempVertex[0].v.x;
				m_MinBox.y=m_MaxBox.y=pTempVertex[0].v.y;
				m_MinBox.z=m_MaxBox.z=pTempVertex[0].v.z;
			}

			// 렌더링단위별 바운딩 min-max 값 계산을 위한 초기화
			m_vecMeshPieceBoundingBoxInfo[i].vBoxMin = pTempVertex[0].v;
			m_vecMeshPieceBoundingBoxInfo[i].vBoxMax = pTempVertex[0].v;

			for(int cv = 0; cv < cVertex; cv++)
			{
				vector3 &vPerObjectMin = m_vecMeshPieceBoundingBoxInfo[i].vBoxMin;
				vector3 &vPerObjectMax = m_vecMeshPieceBoundingBoxInfo[i].vBoxMax;

				// 전체 메시단위 바운딩 min-max 계산
				m_MinBox.x = min(m_MinBox.x, pTempVertex[cv].v.x);
				m_MinBox.y = min(m_MinBox.y, pTempVertex[cv].v.y);
				m_MinBox.z = min(m_MinBox.z, pTempVertex[cv].v.z);

				m_MaxBox.x = max(m_MaxBox.x, pTempVertex[cv].v.x);
				m_MaxBox.y = max(m_MaxBox.y, pTempVertex[cv].v.y);
				m_MaxBox.z = max(m_MaxBox.z, pTempVertex[cv].v.z);

				// 렌더링단위별 바운딩 min-max 계산
				vPerObjectMin.x = min(vPerObjectMin.x, pTempVertex[cv].v.x);
				vPerObjectMin.y = min(vPerObjectMin.y, pTempVertex[cv].v.y);
				vPerObjectMin.z = min(vPerObjectMin.z, pTempVertex[cv].v.z);

				vPerObjectMax.x = max(vPerObjectMax.x, pTempVertex[cv].v.x);
				vPerObjectMax.y = max(vPerObjectMax.y, pTempVertex[cv].v.y);
				vPerObjectMax.z = max(vPerObjectMax.z, pTempVertex[cv].v.z);

//				if(m_MaxBox.x < pTempVertex[cv].v.x)
//					m_MaxBox.x=pTempVertex[cv].v.x;
//				if(m_MaxBox.y < pTempVertex[cv].v.y)
//					m_MaxBox.y=pTempVertex[cv].v.y;
//				if(m_MaxBox.z < pTempVertex[cv].v.z)
//					m_MaxBox.z=pTempVertex[cv].v.z;
//
//				if(m_MinBox.x > pTempVertex[cv].v.x)
//					m_MinBox.x=pTempVertex[cv].v.x;
//				if(m_MinBox.y > pTempVertex[cv].v.y)
//					m_MinBox.y=pTempVertex[cv].v.y;
//				if(m_MinBox.z > pTempVertex[cv].v.z)
//					m_MinBox.z=pTempVertex[cv].v.z;

			}

			m_nVertex.push_back(cVertex);
			m_nIndices.push_back(cIndices);
			m_pMatRef.push_back(MatRef);

			m_pIndices.push_back(pTempIndices);
			m_pVertex.push_back(pTempVertex);


		}

	}
	else
	{
		return false;
	}

	
	return true;

}
bool CNMesh::Unload()
{
	return true;
}
bool CNMesh::PostLoad()
{
	int i;
	int cVertex,cIndices;
	if(m_iObjectMethod == 0)
	{
		MultiVertex *pVertexData = NULL;
		WORD *pIndicesData =  NULL;


		m_dwShader = MultiFVF;

		for(i = 0;i < (int)m_pVertex.size(); i++ )	// object number
		{
			cVertex =  m_nVertex[i];
			cIndices = m_nIndices[i];
			
			LPDIRECT3DVERTEXBUFFER8 pAllocVB;
			LPDIRECT3DINDEXBUFFER8 pAllocIB;
			
			pAllocVB = CSceneManager::ms_pBufferPools->GetVertexBuffer(cVertex*sizeof(MultiVertex),MultiFVF,false);
			pAllocIB = CSceneManager::ms_pBufferPools->GetIndexBuffer(cIndices*3*sizeof(WORD),D3DFMT_INDEX16,false);

			pAllocVB->Lock(0,0,(BYTE**)&pVertexData,0);
			pAllocIB->Lock(0,cIndices*3*sizeof(WORD),(BYTE**)&pIndicesData,0);
			
			memcpy(pVertexData,(MultiVertex *)m_pVertex[i],cVertex*sizeof(MultiVertex));
			memcpy(pIndicesData,m_pIndices[i],cIndices*3*sizeof(WORD));

			pAllocVB->Unlock();
			pAllocIB->Unlock();
			
			m_pVertexBuffer.push_back(pAllocVB);
			m_pIndicesBuffer.push_back(pAllocIB);	
			delete[] m_pVertex[i];
			m_pVertex[i] = NULL;
			delete[] m_pIndices[i];
			m_pIndices[i] = NULL;

		}
		m_pVertex.clear();
		m_pIndices.clear();

	}
	else if(m_iObjectMethod == 3)
	{

		LightVertex *pVertexData = NULL;
		WORD *pIndicesData =  NULL;


		m_dwShader = LightFVF;

		for(i = 0;i < (int)m_pVertex.size(); i++ )	// object number
		{
			cVertex =  m_nVertex[i];
			cIndices = m_nIndices[i];
			
			LPDIRECT3DVERTEXBUFFER8 pAllocVB;
			LPDIRECT3DINDEXBUFFER8 pAllocIB;
			
			pAllocVB = CSceneManager::ms_pBufferPools->GetVertexBuffer(cVertex*sizeof(LightVertex),LightFVF,false);
			pAllocIB = CSceneManager::ms_pBufferPools->GetIndexBuffer(cIndices*3*sizeof(WORD),D3DFMT_INDEX16,false);

			pAllocVB->Lock(0,0,(BYTE**)&pVertexData,0);
			pAllocIB->Lock(0,cIndices*3*sizeof(WORD),(BYTE**)&pIndicesData,0);
			
			memcpy(pVertexData,(LightVertex *)m_pVertex[i],cVertex*sizeof(LightVertex));
			memcpy(pIndicesData,m_pIndices[i],cIndices*3*sizeof(WORD));

			pAllocVB->Unlock();
			pAllocIB->Unlock();
			
			m_pVertexBuffer.push_back(pAllocVB);
			m_pIndicesBuffer.push_back(pAllocIB);	
			delete[] m_pVertex[i];
			m_pVertex[i] = NULL;
			delete[] m_pIndices[i];
			m_pIndices[i] = NULL;

		}
		m_pVertex.clear();
		m_pIndices.clear();

	}


	CNTexture::SetPath(OBJECTTEXTUREPATH);

	for(int i = 0; i < (int)m_pMatName.size(); i++ )
	{
		if(strcmp(m_pMatName[i],"")==0)
			continue;
		CNTexture *pTexture = (CNTexture *)CSceneManager::_GetResource(CROSSM::RESOURCE_TEXTURE,m_pMatName[i]);
		m_pMat.push_back(pTexture);
	}

	
	if(strstr(m_strName,"tournament_arena_stage1.r3s"))
	{
		ConvertNormal();
	}

/*
	if(m_dwConvertShader == LightFVF)
	{
		ConvertNormal();
	}
*/

	return true;
}
void CNMesh::ConvertNormal()
{
	if(m_bCompleteMesh)
		return;

	if(m_dwShader==LightFVF)
		return;

	SetShader(LightFVF);
	LightVertex *pVertexNormalData;
	MultiVertex *pVertexData;
	WORD *pIndicesData;

	vector3 vecNormal;
	vector3 *v[3];
	for(int i=0;i<(int)m_nVertex.size();i++)
	{
		LPDIRECT3DVERTEXBUFFER8 pAllocVB;		
		pAllocVB = CSceneManager::ms_pBufferPools->GetVertexBuffer(m_nVertex[i]*sizeof(LightVertex),LightFVF,false);

	
		pAllocVB->Lock(0,0,(BYTE**)&pVertexNormalData,0);
		m_pIndicesBuffer[i]->Lock(0,m_nIndices[i]*3*sizeof(WORD),(BYTE**)&pIndicesData,0);
		m_pVertexBuffer[i]->Lock(0,0,(BYTE**)&pVertexData,0);

		for(int cIndices=0;cIndices<m_nIndices[i];cIndices++)
		{
			v[0]=(vector3*)&pVertexData[ pIndicesData[cIndices*3+0] ].v;
			v[1]=(vector3*)&pVertexData[ pIndicesData[cIndices*3+1] ].v;
			v[2]=(vector3*)&pVertexData[ pIndicesData[cIndices*3+2] ].v;

			vecNormal= (*v[1]-*v[0])^(*v[2]-*v[1]);
			vecNormal.Normalize();

			pVertexData[ pIndicesData[cIndices*3+0] ].v;
			pVertexData[ pIndicesData[cIndices*3+1] ].v;
			pVertexData[ pIndicesData[cIndices*3+2] ].v;
			
			pVertexNormalData[pIndicesData[cIndices*3+0]].v=pVertexData[ pIndicesData[cIndices*3+0] ].v;
			pVertexNormalData[pIndicesData[cIndices*3+0]].tu=pVertexData[ pIndicesData[cIndices*3+0] ].tu;
			pVertexNormalData[pIndicesData[cIndices*3+0]].tv=pVertexData[ pIndicesData[cIndices*3+0] ].tv;
			pVertexNormalData[pIndicesData[cIndices*3+0]].spec.c=0x0;
			pVertexNormalData[pIndicesData[cIndices*3+0]].n=vecNormal;


			pVertexNormalData[pIndicesData[cIndices*3+1]].v=pVertexData[ pIndicesData[cIndices*3+1] ].v;
			pVertexNormalData[pIndicesData[cIndices*3+1]].tu=pVertexData[ pIndicesData[cIndices*3+1] ].tu;
			pVertexNormalData[pIndicesData[cIndices*3+1]].tv=pVertexData[ pIndicesData[cIndices*3+1] ].tv;
			pVertexNormalData[pIndicesData[cIndices*3+1]].spec.c=0x0;
			pVertexNormalData[pIndicesData[cIndices*3+1]].n=vecNormal;

			pVertexNormalData[pIndicesData[cIndices*3+2]].v=pVertexData[ pIndicesData[cIndices*3+2] ].v;
			pVertexNormalData[pIndicesData[cIndices*3+2]].tu=pVertexData[ pIndicesData[cIndices*3+2] ].tu;
			pVertexNormalData[pIndicesData[cIndices*3+2]].tv=pVertexData[ pIndicesData[cIndices*3+2] ].tv;
			pVertexNormalData[pIndicesData[cIndices*3+2]].spec.c=0x0;
			pVertexNormalData[pIndicesData[cIndices*3+2]].n=vecNormal;
			
		}

#ifndef _DEBUG
		// smoothing
		int *iTmp = new int[m_nVertex[i]];
		int iCount;

		for(int iVert = 0; iVert < m_nVertex[i]; iVert++)
		{
			memset(iTmp,0,sizeof(int) * m_nVertex[i]);
			iCount = 0;

			vector3 vecPos = pVertexNormalData[iVert].v;
			vector3 vecNormal = vector3(0.0f,0.0f,0.0f);
			for(int j = 0; j < m_nVertex[i]; j++ )
			{
				if(pVertexNormalData[j].v.x == pVertexNormalData[iVert].v.x &&
					pVertexNormalData[j].v.y == pVertexNormalData[iVert].v.y &&
					pVertexNormalData[j].v.z == pVertexNormalData[iVert].v.z)
				{
					if(D3DXVec3Dot((D3DXVECTOR3 *)(&pVertexNormalData[j].n),(D3DXVECTOR3 *)(&pVertexNormalData[iVert].n)) >= 0.5f)
					{

						iTmp[iCount++] = j;
						vecNormal += pVertexNormalData[j].n;
					}
				}
			}
			vecNormal.Normalize();
			for(int iT = 0; iT < iCount; iT++)
			{
				pVertexNormalData[iTmp[iT]].n = vecNormal;
			}

			
		}
		delete[] iTmp;
		iTmp = NULL;
#endif
		
		pAllocVB->Unlock();		
		m_pIndicesBuffer[i]->Unlock();
		m_pVertexBuffer[i]->Unlock();
		//m_pVertexBuffer[i]->Release();
		CSceneManager::ms_pBufferPools->UnRef(Caldron::Scene::D3DBUFFEROBJ_VERTEX,m_pVertexBuffer[i]);
		m_pVertexBuffer[i]=pAllocVB;
	}
}
void CNMesh::ConvertMulti()
{
if(m_bCompleteMesh)
		return;
	if(MultiFVF==m_dwShader)
		return;
	if(LightFVF==m_dwShader)
	{
		SetShader(MultiFVF);
		LightVertex *pVertexNormalData;
		MultiVertex *pVertexData;
		
		for(int i=0;i<(int)m_nVertex.size();i++)
		{
			LPDIRECT3DVERTEXBUFFER8 pAllocVB;		
			pAllocVB = CSceneManager::ms_pBufferPools->GetVertexBuffer(m_nVertex[i]*sizeof(MultiVertex),MultiFVF,false);

		

			pAllocVB->Lock(0,0,(BYTE**)&pVertexData,0);		
			m_pVertexBuffer[i]->Lock(0,0,(BYTE**)&pVertexNormalData,0);
			for(int cVertex=0;cVertex<m_nVertex[i];cVertex++)
			{
				pVertexData[cVertex].v=pVertexNormalData[cVertex].v;
				pVertexData[cVertex].tu=pVertexNormalData[cVertex].tu;
				pVertexData[cVertex].tv=pVertexNormalData[cVertex].tv;
				pVertexData[cVertex].spec.c=0x0;
				pVertexData[cVertex].diff.c=0xffffffff;			
			}		
			pAllocVB->Unlock();		
			m_pVertexBuffer[i]->Unlock();
			CSceneManager::ms_pBufferPools->UnRef(Caldron::Scene::D3DBUFFEROBJ_VERTEX,m_pVertexBuffer[i]);
			//m_pVertexBuffer[i]->Release();
			m_pVertexBuffer[i]=pAllocVB;
		}
	}
	else
	{
		SetShader(MultiFVF);
		DetailVertex *pVertexNormalData;
		MultiVertex *pVertexData;
		
		for(int i=0;i<(int)m_nVertex.size();i++)
		{
			LPDIRECT3DVERTEXBUFFER8 pAllocVB;		

			pAllocVB = CSceneManager::ms_pBufferPools->GetVertexBuffer(m_nVertex[i]*sizeof(MultiVertex),MultiFVF,false);

			pAllocVB->Lock(0,0,(BYTE**)&pVertexData,0);		
			m_pVertexBuffer[i]->Lock(0,0,(BYTE**)&pVertexNormalData,0);
			for(int cVertex=0;cVertex<m_nVertex[i];cVertex++)
			{
				
				pVertexData[cVertex].v=pVertexNormalData[cVertex].v;
				pVertexData[cVertex].tu=pVertexNormalData[cVertex].tu;
				pVertexData[cVertex].tv=pVertexNormalData[cVertex].tv;				
				pVertexData[cVertex].spec.c=0x0;
				pVertexData[cVertex].diff.c=0xffffffff;							
			}		
			pAllocVB->Unlock();		
			m_pVertexBuffer[i]->Unlock();
			//m_pVertexBuffer[i]->Release();
			CSceneManager::ms_pBufferPools->UnRef(Caldron::Scene::D3DBUFFEROBJ_VERTEX,m_pVertexBuffer[i]);	
			m_pVertexBuffer[i]=pAllocVB;
		}
	}
}
void CNMesh::ConvertTree()
{
	SetShader(TREEVERTEXFVF);
	TreeVertex *pVertexNormalData;	
	MultiVertex *pVertexData;
	WORD *pIndicesData;

	vector3 vecNormal;
	vector3 *v[3];
	float fMaxY,fMinY;
	fMaxY=-100000000000.0f;
	fMinY=100000000000.0f;
	for(int i=0;i<(int)m_nVertex.size();i++)
	{
		m_pVertexBuffer[i]->Lock(0,0,(BYTE**)&pVertexData,0);
		for(int cVertex=0;cVertex<m_nVertex[i];cVertex++)
		{
			if(fMaxY<pVertexData[cVertex].v.y)			
				fMaxY=pVertexData[cVertex].v.y;
			if(fMinY>pVertexData[cVertex].v.y)
				fMinY=pVertexData[cVertex].v.y;
			
		}
		m_pVertexBuffer[i]->Unlock();
	}

	float fSize=fMaxY-fMinY;	
	float fRate;//,fEnd;
	for(int i=0;i<(int)m_nVertex.size();i++)
	{
		LPDIRECT3DVERTEXBUFFER8 pAllocVB;		
		pAllocVB = CSceneManager::ms_pBufferPools->GetVertexBuffer(m_nVertex[i]*sizeof(TreeVertex),TREEVERTEXFVF,false);

		/*
		CSceneManager::GetDevice()->CreateVertexBuffer(m_nVertex[i]*sizeof(TreeVertex),D3DUSAGE_WRITEONLY,
													TREEVERTEXFVF,D3DPOOL_MANAGED,&pAllocVB);
		*/
		pAllocVB->Lock(0,0,(BYTE**)&pVertexNormalData,0);
		m_pIndicesBuffer[i]->Lock(0,m_nIndices[i]*3*sizeof(WORD),(BYTE**)&pIndicesData,0);
		m_pVertexBuffer[i]->Lock(0,0,(BYTE**)&pVertexData,0);

		for(int cIndices=0;cIndices<m_nIndices[i];cIndices++)
		{
			v[0]=(vector3*)&pVertexData[ pIndicesData[cIndices*3+0] ].v;
			v[1]=(vector3*)&pVertexData[ pIndicesData[cIndices*3+1] ].v;
			v[2]=(vector3*)&pVertexData[ pIndicesData[cIndices*3+2] ].v;

			vecNormal= (*v[1]-*v[0])^(*v[2]-*v[1]);
			vecNormal.Normalize();

			pVertexData[ pIndicesData[cIndices*3+0] ].v;
			pVertexData[ pIndicesData[cIndices*3+1] ].v;
			pVertexData[ pIndicesData[cIndices*3+2] ].v;
			
			pVertexNormalData[pIndicesData[cIndices*3+0]].v=pVertexData[ pIndicesData[cIndices*3+0] ].v;
			pVertexNormalData[pIndicesData[cIndices*3+0]].tu=pVertexData[ pIndicesData[cIndices*3+0] ].tu;
			pVertexNormalData[pIndicesData[cIndices*3+0]].tv=pVertexData[ pIndicesData[cIndices*3+0] ].tv;			
			pVertexNormalData[pIndicesData[cIndices*3+0]].n=vecNormal;
			fRate=(pVertexNormalData[pIndicesData[cIndices*3+0]].v.y-fMinY)/fSize;
			pVertexNormalData[pIndicesData[cIndices*3+0]].fWeight=fRate*fRate;
		

			pVertexNormalData[pIndicesData[cIndices*3+1]].v=pVertexData[ pIndicesData[cIndices*3+1] ].v;
			pVertexNormalData[pIndicesData[cIndices*3+1]].tu=pVertexData[ pIndicesData[cIndices*3+1] ].tu;
			pVertexNormalData[pIndicesData[cIndices*3+1]].tv=pVertexData[ pIndicesData[cIndices*3+1] ].tv;			
			pVertexNormalData[pIndicesData[cIndices*3+1]].n=vecNormal;
			fRate=(pVertexNormalData[pIndicesData[cIndices*3+1]].v.y-fMinY)/fSize;			
			pVertexNormalData[pIndicesData[cIndices*3+1]].fWeight=fRate*fRate;

			pVertexNormalData[pIndicesData[cIndices*3+2]].v=pVertexData[ pIndicesData[cIndices*3+2] ].v;
			pVertexNormalData[pIndicesData[cIndices*3+2]].tu=pVertexData[ pIndicesData[cIndices*3+2] ].tu;
			pVertexNormalData[pIndicesData[cIndices*3+2]].tv=pVertexData[ pIndicesData[cIndices*3+2] ].tv;			
			pVertexNormalData[pIndicesData[cIndices*3+2]].n=vecNormal;
			fRate=(pVertexNormalData[pIndicesData[cIndices*3+2]].v.y-fMinY)/fSize;			
			pVertexNormalData[pIndicesData[cIndices*3+2]].fWeight=fRate*fRate;
			
		}
		// smoothing
		int *iTmp = new int[m_nVertex[i]];
		int iCount;

		for(int iVert = 0; iVert < m_nVertex[i]; iVert++)
		{
			memset(iTmp,0,sizeof(int) * m_nVertex[i]);
			iCount = 0;

			vector3 vecPos = pVertexNormalData[iVert].v;
			vector3 vecNormal = vector3(0.0f,0.0f,0.0f);
			for(int j = 0; j < m_nVertex[i]; j++ )
			{
				if(pVertexNormalData[j].v.x == pVertexNormalData[iVert].v.x &&
					pVertexNormalData[j].v.y == pVertexNormalData[iVert].v.y &&
					pVertexNormalData[j].v.z == pVertexNormalData[iVert].v.z)
				{
					iTmp[iCount++] = j;
					vecNormal += pVertexNormalData[j].n;

				}
			}
			vecNormal.Normalize();
			for(int iT = 0; iT < iCount; iT++)
			{
				pVertexNormalData[iTmp[iT]].n = vecNormal;
			}

			
		}
		delete[] iTmp;
		iTmp = NULL;

		//

		pAllocVB->Unlock();
		m_pIndicesBuffer[i]->Unlock();
		m_pVertexBuffer[i]->Unlock();
		//m_pVertexBuffer[i]->Release();
		CSceneManager::ms_pBufferPools->UnRef(Caldron::Scene::D3DBUFFEROBJ_VERTEX,m_pVertexBuffer[i]);
		m_pVertexBuffer[i] = NULL;

		m_pVertexBuffer[i]=pAllocVB;
	}
}

void CNMesh::AllUnlockBuffer()
{
	if(m_bCompleteMesh)
		return;
	for(int cVB=0;cVB<(int)m_pVertexBuffer.size();cVB++)
		m_pVertexBuffer[cVB]->Unlock();			

	for(int cIB=0;cIB<(int)m_pIndicesBuffer.size();cIB++)
		m_pIndicesBuffer[cIB]->Unlock();

	m_pVertex.clear();
	m_pIndices.clear();
	m_isAlreadyLock=false;


}
void CNMesh::AllLockBuffer()
{
	if(m_bCompleteMesh)
		return;
	if(m_isAlreadyLock==true)
		return;
	m_isAlreadyLock=true;
	if(m_dwShader==LightFVF)
	{
		LightVertex *pVertexData;
		m_pVertex.clear();
		for(int cVB=0;cVB<(int)m_pVertexBuffer.size();cVB++)
		{
			m_pVertexBuffer[cVB]->Lock(0,0,(BYTE**)&pVertexData,NULL);
			m_pVertex.push_back(pVertexData);
		}
	}	
	else
	{
		MultiVertex *pVertexData;
		m_pVertex.clear();
		for(int cVB=0;cVB<(int)m_pVertexBuffer.size();cVB++)
		{
			m_pVertexBuffer[cVB]->Lock(0,0,(BYTE**)&pVertexData,NULL);
			m_pVertex.push_back(pVertexData);
		}
	}
	WORD *pIndicesData;
	m_pIndices.clear();
	for(int cIB=0;cIB<(int)m_pIndicesBuffer.size();cIB++)
	{		
		m_pIndicesBuffer[cIB]->Lock(0,m_nIndices[cIB]*3*sizeof(WORD),(BYTE**)&pIndicesData,0);
		m_pIndices.push_back(pIndicesData);
	}
}
void CNMesh::RenderBox(LPDIRECT3DDEVICE8 pd3dDevice)
{
	CRenderBoundingBox::Render(pd3dDevice, m_MinBox, m_MaxBox);

//	for(size_t i = 0; i < m_vecMeshPieceBoundingBoxInfo.size(); ++i)
//	{
//		CRenderBoundingBox::Render(pd3dDevice, m_vecMeshPieceBoundingBoxInfo[i].vBoxMin, m_vecMeshPieceBoundingBoxInfo[i].vBoxMax);
//	}
}
void CNMesh::Render(LPDIRECT3DDEVICE8 pd3dDevice)
{
static int only = 1;
	if(!m_bRender)
		return;
// R3S mesh bounding box render. 테스트시만 on
//RenderBox(pd3dDevice);

// R3S mesh name render. 테스트시만 on
//RenderName(pd3dDevice);

	pd3dDevice->SetVertexShader(m_dwShader);
	
	if(m_dwShader==MultiFVF)
	{
		for(int i=0;i<(int)m_pVertexBuffer.size();i++)		
		{
//if (i != only) continue;
			if(m_pMat[m_pMatRef[i]])
				pd3dDevice->SetTexture(0,m_pMat[m_pMatRef[i]]->GetTexture());	
			else
				pd3dDevice->SetTexture(0,NULL);

			pd3dDevice->SetStreamSource(0,m_pVertexBuffer[i],sizeof(MultiVertex));
			pd3dDevice->SetIndices(m_pIndicesBuffer[i],0);
			
			pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
											 0,m_nVertex[i],
											 0,m_nIndices[i]);        
			CSceneManager::m_cRenderPolygon+=m_nIndices[i];
			CSceneManager::m_cRenderVertex+=m_nVertex[i];
			
		}	
		return;
	}
	if(m_dwShader==LightFVF)
	{
		for(int i=0;i<(int)m_pVertexBuffer.size();i++)		
		{
			if(m_pMat[m_pMatRef[i]])
				pd3dDevice->SetTexture(0,m_pMat[m_pMatRef[i]]->GetTexture());	
			else
				pd3dDevice->SetTexture(0,NULL);
			pd3dDevice->SetStreamSource(0,m_pVertexBuffer[i],sizeof(LightVertex));
			
			pd3dDevice->SetIndices(m_pIndicesBuffer[i],0);		
			pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
											 0,m_nVertex[i],
											 0,m_nIndices[i]);        
			CSceneManager::m_cRenderPolygon+=m_nIndices[i];
			CSceneManager::m_cRenderVertex+=m_nVertex[i];
			
		}
		return;
	}
	
	if(m_dwShader==TREEVERTEXFVF)
	{
		for(int i=0;i<(int)m_pVertexBuffer.size();i++)		
		{
			if(m_pMat[m_pMatRef[i]])
				pd3dDevice->SetTexture(0,m_pMat[m_pMatRef[i]]->GetTexture());	
			else
				pd3dDevice->SetTexture(0,NULL);
			pd3dDevice->SetStreamSource(0,m_pVertexBuffer[i],sizeof(LightVertex));			

			pd3dDevice->SetIndices(m_pIndicesBuffer[i],0);			
			pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
											 0,m_nVertex[i],
											 0,m_nIndices[i]);        
			CSceneManager::m_cRenderPolygon+=m_nIndices[i];
			CSceneManager::m_cRenderVertex+=m_nVertex[i];
			
		}
		return;
	}
}

void CNMesh::CalcBox()
{
	if(m_bCompleteMesh)
		return;
	MultiVertex *pVertexData;
	if(m_pVertexBuffer.size()==0)
	{
		throw CSceneLayerError("CMeshObject:CalcBox, This meshobject have not vertex");
	}
	for(int i=0;i<(int)m_pVertexBuffer.size();i++)
	{
		m_pVertexBuffer[i]->Lock(0,0,(BYTE**)&pVertexData,0);				
		if(m_MaxBox.x==-1.0f && m_MinBox.x==-1.0f)
		{
			m_MinBox.x=m_MaxBox.x=pVertexData[0].v.x;
			m_MinBox.y=m_MaxBox.y=pVertexData[0].v.y;
			m_MinBox.z=m_MaxBox.z=pVertexData[0].v.z;
			
		}		
		for(int cv=0;cv<m_nVertex[i];cv++)
		{
			if(m_MaxBox.x < pVertexData[cv].v.x)
				m_MaxBox.x=pVertexData[cv].v.x;
			if(m_MaxBox.y < pVertexData[cv].v.y)
				m_MaxBox.y=pVertexData[cv].v.y;
			if(m_MaxBox.z < pVertexData[cv].v.z)
				m_MaxBox.z=pVertexData[cv].v.z;

			if(m_MinBox.x > pVertexData[cv].v.x)
				m_MinBox.x=pVertexData[cv].v.x;
			if(m_MinBox.y > pVertexData[cv].v.y)
				m_MinBox.y=pVertexData[cv].v.y;
			if(m_MinBox.z > pVertexData[cv].v.z)
				m_MinBox.z=pVertexData[cv].v.z;
			pVertexData[cv].diff.a=0xaa;
		}
		m_pVertexBuffer[i]->Unlock();
	}	
}


void CNMesh::RenderName(IDirect3DDevice8* pDevice)
{
	vector3 vPosition ,vPos2D;
	vPosition.x = (m_MinBox.x + m_MaxBox.x)/2.0f;
	vPosition.z = (m_MinBox.z + m_MaxBox.z)/2.0f;
	vPosition.y = m_MaxBox.y;
	float w;

	D3DMATRIX mat;
	pDevice->GetTransform(D3DTS_WORLD, &mat);
	vPosition.x += mat._41;
	vPosition.y += mat._42;
	vPosition.z += mat._43;

	if (false == BaseGraphicsLayer::TransformVector(vPosition, vPos2D,w))
	{
		return;
	}
	if (vPos2D.z < 0.0f || vPos2D.z > 1.0f)
	{
		return;
	}

	BaseGraphicsLayer::DrawText(vPos2D.x, vPos2D.y, D3DCOLOR_ARGB(0xFF, 0, 0x60, 0xFF), m_strName);
}




}

